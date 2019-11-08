//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_SOCKET_H__
#define __PHANTOM_SOCKET_H__
#ifdef WIN32
#include <Windows.h>
#endif

#include <vector>
#include <string>
#include "PhantomMath.h"
#include "PhantomBase.h"
#ifdef WIN32
#define	InvalidSocket		INVALID_SOCKET
typedef	HANDLE				ThreadHandle;
typedef	CRITICAL_SECTION	Critical;
typedef	SOCKET				Socket;
#elif defined(IPHONE_OS)
#include "netinet/in.h"
#include "arpa/inet.h"
#include "sys/socket.h"
#include <netdb.h>
#include <pthread.h>
#define	InvalidSocket		-1
typedef	pthread_t			ThreadHandle;
typedef	pthread_mutex_t		Critical;
typedef	int					Socket;
#define	closesocket			close
#elif defined(ANDROID_OS)
#include "netinet/in.h"
#include "arpa/inet.h"
#include "sys/socket.h"
#include <netdb.h>
#include <pthread.h>
#define	InvalidSocket		-1
typedef	pthread_t			ThreadHandle;
typedef	pthread_mutex_t		Critical;
typedef	int					Socket;
#define	closesocket			close
#endif
extern	void				SleepThread(unsigned int millisecond);	//睡眠一段时间
extern	BOOL				GetLocalIP(char* ret);//返回本机局域网IP


#define MAKEDWORD(l, h)     ((DWORD)(((WORD)(((DWORD)(l)) & 0xffff)) | ((DWORD)((WORD)(((DWORD)(h)) & 0xffff))) << 16))
#define LWORD(l)           ((WORD)(((DWORD)(l)) & 0xffff))
#define HWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xffff))

#define	SOCKET_PACKAGE_MAX					141000

namespace Phantom{

	enum{
		MsgID_cs_ServerInfo	=	30000,//请求服务器信息
	};
	//连接状态定义
	enum SocketError{
		SocketError_NoError = 0,
		SocketError_CreateFail,	//不能创建句柄
		SocketError_FailIP,	
		SocketError_FailConnect,
	};
	extern	BOOL		SetLastSockError(DWORD dwError);
	extern	DWORD		GetLastSockError();
	extern	const char*	GetLastSockErrorText();
	extern	void		Block(Socket s, BOOL bBlock);//设置是否进入阻塞模式
	//
	class	NetToAI;
	//网络钩子接口
	class	NetAICB
	{
	public:
		virtual VOID	OnSocketConnect(BOOL bError, NetToAI * pIClientSocket) = 0;//网络连接消息
		virtual VOID	OnSocketRead(void * pBuffer, WORD wDataSize, NetToAI * pIClientSocket) = 0;//网络读取消息
		virtual VOID	OnSocketClose(BOOL bCloseByServer, NetToAI * pSock) = 0;//网络关闭消息
	};
	typedef VOID	(*FnLink)(DWORD linkid);//某个客户端连接时被调用
	typedef VOID	(*FnDislink)(DWORD linkid);//某个客户端断开连接时被调用
	typedef VOID	(*FnMessage)(DWORD linkid, WORD wID, const char* dataPtr, unsigned int wSize);//某个客户端处理消息时被调用
#pragma pack(push, 1)
	struct	ServerInfo
	{
		NameT<32>	name;//服务器名字
		NameT<16>	ip;//ip地址
		int			players;//用户数量
		NameT<32>	state;//当前的状态描述，总共32个字节,文本形式的描述
		ServerInfo(){players = 0;}
	};
	//网络连接类
	struct	NetClientInfo
	{
		enum EnumSocketState
		{
			SocketState_NoConnect	=	0,	//没有连接
			SocketState_Connected,			//成功连接
		};
		double			dLastSendTime;
		double			dLastRecvTime;
		unsigned int	sendPacket;	//发送包数量
		unsigned int	recvPacket;	//接收包数量
		unsigned int	sendBytes;	//发送字节数量
		unsigned int	recvBytes;	//接收字节数量
		EnumSocketState	state;	//当前状态
	};
	struct	MessageID
	{
		unsigned int	wSize;	//包大小
		WORD	wID;	//包ID
	};
	struct	SocketID
	{
		short	index;
		WORD	id;
		SocketID(){index=-1; id = 0;}
		SocketID(short _index, WORD _id){index = _index; id = _id;}
	};

	struct	Package
	{
		enum{
			PackageType_ConnectFail	=	0,
			PackageType_ConnectOK,
			PackageType_RecvMessage,
			PackageType_DisconnectByServer,
			PackageType_Users,
			PackageType_AcceptClose,
		};
		unsigned char	type;
		WORD			msgid;
		DWORD			id;
		unsigned int	readBytes;
		FnMessage		message;
		Package(){type = 0; readBytes = 0; msgid = 0; id = 0; message = 0;}
	};
#define	SOCKET_PACKAGE_MAX_AI				8100
	struct	PackageAI
	{
		enum{
			PackageType_ConnectFail	=	0,
			PackageType_ConnectOK,
			PackageType_RecvMessage,
			PackageType_DisconnectByServer,
			PackageType_Users,
		};
		unsigned char	type;
		WORD			msgid;
		DWORD			id;
		unsigned short	readBytes;
		FnMessage		message;
		char			bytes[SOCKET_PACKAGE_MAX_AI];
		PackageAI(){type = 0; readBytes = 0; msgid = 0; id = 0; message = 0;}
	};
	//
#pragma pack(pop)
	typedef VOID	(*FnSearchComplete)(const char* szIP);//ServerInfo* servers, int count);//搜索完所有服务器之后返回的调用
	struct PhantomLock
	{
		PhantomLock();
		~PhantomLock();
		VOID				Lock();
		VOID				Unlock();
		Critical			crit;
	};
	struct	Lock
	{
		Lock(PhantomLock& l){lk = &l;lk->Lock();}
		~Lock(){if(lk)lk->Unlock();}
		VOID	Unlock(){if(lk)lk->Unlock();lk = 0;}
		PhantomLock* lk;
	};

	typedef VOID	(*PThreadCallback)(void* parameter);
	VOID			createThread(PThreadCallback cb, void* parameter);
	//线程控制类
	class	ThreadProc;
	class	Thread
	{
	public:
		Thread();
		virtual ~Thread(){Stop();}
		virtual	VOID			Start(ThreadProc* t);
		virtual	VOID			Stop(BOOL bImmediately = true);//make it immediately stop or wait next loop in thread onetime 
		virtual	VOID			OnThread();
		inline	BOOL			IsRunning(){return m_bThreadRunning;}
		static BOOL				s_bThreadAllExit;	//停止一切线程运行
	protected:
		ThreadHandle			m_hThread;
		BOOL                    m_bThreadRunning;
		ThreadProc*				m_proc;
	};

	class	ThreadProc: public Base
	{
	public:
		ImplementComValue();
		ThreadProc(){}
		virtual ~ThreadProc(){m_thread.Stop();}
		virtual	VOID			OnThread() = 0;
		virtual	VOID			Start(){m_thread.Stop(); m_thread.Start(this);}
		virtual	VOID			Stop(BOOL bImmediately = true){m_thread.Stop(bImmediately);}
	public:
		Thread			m_thread;
	};
	class	NetRecvBase: public ThreadProc
	{
	public:
		NetRecvBase();
		virtual	~NetRecvBase(){};
		virtual BOOL					Send(WORD id, const char* pData, unsigned int wDataSize);//发送数据
		virtual VOID					Close();
	public://必须继承
		virtual	VOID					OnMessage(WORD wID, const char* dataPtr, unsigned int wSize){}
		virtual	VOID					RecvBuffer(unsigned char type, WORD wMessageID, void * pData, unsigned int wDataSize, SocketID* id = 0) = 0;	//接收消息
	public:
		SocketID						m_id;
		FnMessage						m_message;//消息触发函数
		FnLink							m_link;//连接相应函数
		FnDislink						m_dislink;//连接断开函数
		//
		virtual	VOID					OnLoopRecv();
		virtual	int						onRecv(char* buffer);
		BOOL							m_bStopSend;
		unsigned int					m_guid;
		Socket							m_hSocket;
		BOOL							m_bClosed;
		int								m_apptime;
	};
	class NetRecvMessage: public NetRecvBase
	{
	public:
		NetRecvMessage();
		~NetRecvMessage(){}
		//基础接口
		static	VOID					DoMessageQueue();
		static CDynamicArray<unsigned char>		s_recvList[2];		//接收到的消息
		static CDynamicArray<unsigned char>*	s_recvCurrent;
		static PhantomLock				s_recvLock;

		virtual	VOID					RecvBuffer(unsigned char type, WORD wMessageID, void * pData, unsigned int wDataSize, SocketID* id = 0);	//接收消息
	protected://必须继承
		virtual	VOID					OnRecvBuffer(unsigned char type, const SocketID& id) = 0;
	protected:
		//
	};

	class NetClient: public NetRecvMessage
	{
	public:
		NetClient();//构造函数
		virtual ~NetClient();//析构函数
	public:
		//const NetClientInfo&			GetInfo(){return m_info;}
		virtual VOID					Connect(const char* szServerIP, WORD wPort, unsigned int guid = 0x14a3565);//连接服务器
		virtual VOID					Close();
		//辅助函数
		inline	BOOL					IsConnected(){return (m_state == NetClientInfo::SocketState_Connected);}
	protected://必须继承
		virtual	VOID					OnConnected(BOOL bConnectOK);
		virtual	VOID					OnDisconnect();
	protected:
		friend class	AIControls;
		virtual	VOID					OnThread();
	protected:
		virtual	VOID					StartThread();
		virtual BOOL					OnConnect(const char* szServerIP, WORD wPort);
		virtual	VOID					OnRecvBuffer(unsigned char type, const SocketID& id);
	protected:
		int								m_state;
		//NetClientInfo					m_info;
		Name							m_szIP;
		WORD							m_wPort;
	protected:
		BOOL							m_bCloseByServer;					//关闭方式
	};
	#define	MAX_SERVICE_LINKS		256
	class	NetServerLink;
	class	NetServer;
	//网络钩子接口
	class NetServerLink: public NetRecvBase
	{
	public:
		virtual	VOID				Close();
		inline	BOOL				IsRunning(){return (m_hSocket != InvalidSocket);}
		inline	INT					GetIP(){return m_nIP;}
		inline	VOID				GetIP(char* buf);
		NetServerLink(NetServer* s);
		~NetServerLink();
		virtual	VOID				OnMessage(WORD wID, const char* dataPtr, unsigned int wSize);
	protected:
		virtual	VOID				OnThread();
		void						onAccept(unsigned int guid, unsigned int ip, const SocketID& id);
		virtual	VOID				OnClose();
		virtual	VOID				RecvBuffer(unsigned char type, WORD wMessageID, void * pData, unsigned int wDataSize, SocketID* id);	//接收消息
	protected:
		friend class	NetServer;
		NetServer*					m_server;
		unsigned int				m_nIP;
	};

	class NetServer: public NetRecvMessage
	{
	public:
		NetServer();//构造函数
		virtual ~NetServer();//析构函数
		//基础接口
		virtual BOOL					StartServer(WORD wPort, unsigned int guid);//连接服务器
		virtual VOID					StopServer();//关闭服务器
		virtual	VOID					SetMaxLinks(int count){m_nMaxLinks = count;}//设置最大客户连接数量，不能超过32个
		inline	INT						GetMaxLinks(){return m_nMaxLinks;}
		inline	BOOL					IsRunning(){return (m_hSocket != InvalidSocket);}
		VOID							SetEnableListen(BOOL b){m_bListen = b;}//是否接受新的连接进来，比如在游戏过程中不会接受新的连接
		inline	BOOL					IsEnableListen(){return m_bListen;}
		virtual BOOL					Send(const SocketID& sid, WORD id, const char* pData, unsigned int wDataSize);//发送数据
		int								GetCurrLinks(){return m_nCurrLinks;}
		NetServerLink*					GetLink(const SocketID& id);
		BOOL							GetLinkIP(const SocketID& id, char* ip);
		VOID							StopLink(SocketID& sid);
		VOID							CloseWait(SocketID& sid, int nWait);
		inline	WORD					GetPort(){return m_wPort;}
		//
		VOID							SetState(const char* s){m_info.state = s;}//设置当前状态
		const char*						GetState(){return m_info.state;}//返回当前状态
		ServerInfo						m_info;
	protected://可继承函数
		virtual VOID					OnAccept(const SocketID& id);										//网络连接消息，单线程中运作
		virtual VOID					OnClose(const SocketID& id);										//网络关闭消息，单线程中运作
		virtual	VOID					OnMessage(WORD wID, const char* dataPtr, unsigned int wSize, const SocketID& id){}
	protected://本地处理函数
		friend class	NetServerLink;
		virtual	VOID					OnThread();
		virtual	VOID					OnRecvBuffer(unsigned char type, const SocketID& id);
		virtual	NetServerLink*			CreateNewLink();
	protected:
		NetServerLink*					m_links[MAX_SERVICE_LINKS];//最大链接列表，不能超过256个人连接，否则将不继续监听
		int								m_nCurrLinks;
		int								m_nMaxLinks;
		BOOL							m_bListen;
		WORD							m_wPort;
		PhantomLock						m_lockAccept;
		static WORD						s_wLinked;
	};





	class	AIControls;
	class	NetToAI: public ThreadProc//发送给AI控制器
	{
	public:
		NetToAI();//构造函数
		virtual ~NetToAI();//析构函数
		virtual	VOID					DoMessageQueue();
		virtual	VOID					ConnectAI(AIControls* aiPtr, NetAICB* cb);	//连接AI控制器指针
		virtual BOOL					SendBuffer(void * pData, WORD wDataSize);//发送数据,会发送到AI控制器消息队列中
		virtual VOID					Close();
	protected:
		virtual	VOID					OnThread();		//不处理线程
		virtual	VOID					StartThread();	//不开线程
		virtual BOOL					OnConnect(const char* szServerIP, WORD wPort);//连接服务器//默认成功
		virtual	INT						Recv(char* buffer, int bufferLength);	//从AI控制器获得数据
		///
		friend class	AIControls;
		virtual	VOID					RecvBuffer(unsigned char type, void * pData, WORD wDataSize);	//接收消息
	protected:
		CDynamicArray<PackageAI>			m_recvs[2];		//接收到的消息
		CDynamicArray<PackageAI>*			m_current;
		NetClientInfo					m_info;
		NetAICB*						m_callback;
		PhantomLock						m_lock;
	protected:
		AutoPtr<AIControls>		m_aiControl;	//AI控制器
	};
	//
	class	AIControls: public ThreadProc//AI控制器，所有AI都在这里控制
	{
	public:
		AIControls();
		virtual ~AIControls(){}
		virtual BOOL					SendBuffer(void * pData, WORD wDataSize);//发送数据,会发送到AI控制器消息队列中
		virtual	VOID					Stop(BOOL bImmediately = true);
		virtual	VOID					OnLoopOnce();
	protected:
		virtual VOID					OnInit(){}//网络连接消息
		virtual VOID					OnMessage(void * pBuffer, WORD wDataSize){}//网络读取消息
		virtual VOID					OnClose(){}//网络关闭消息
	protected:
		friend class					NetToAI;
		virtual	VOID					OnThink(double dAppTime, double dElapsedTime) = 0;	//思考问题
		virtual	VOID					OnThread();//AI控制器,开线程
		virtual	VOID					RecvBuffer(unsigned char type, void * pData, WORD wDataSize);	//接收消息
	protected:
		double							m_dLastAppTime;
		PhantomLock						m_lock;
		CDynamicArray<PackageAI>			m_recvs[2];		//接收到的消息
		CDynamicArray<PackageAI>*			m_current;
		AutoPtr<NetToAI>				m_netPtr;	//网络指针
	};
	BOOL						Send(DWORD linkid, WORD wID, const char* dataPtr, unsigned int wSize);//一次最多可以发送8190个字节，发送到某个连接,wID必须小于30000且大于0
	VOID						CloseWait(DWORD linkid, INT nMillisecond);//设置关闭时间
	NetRecvBase*				GetRecvByID(DWORD linkid);//获取某个连接的指针
	VOID						CloseLink(DWORD linkid);//关闭某个连接
	VOID						StopAllSocket();//关闭所有的服务器跟客户端
	VOID						SetupSocket(FnLink link, FnDislink dislink, FnMessage message, DWORD dwGUID);//初始化网络模块
	BOOL						StartServer(const char* name, WORD wPort);//开启服务器
	DWORD						StartClient(const char* szIP, WORD wPort, FnLink link, FnDislink dislink, FnMessage message, DWORD dwGUID);//开启客户端，返回一个客户端唯一linkid
	BOOL						SearchServers(FnSearchComplete fn, WORD wPort);//搜索所有的网段内的服务器，搜索完毕之后调用
	extern	NetServer*			g_server;
};

#endif
