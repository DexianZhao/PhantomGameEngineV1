//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
软件名称   :   幻影游戏引擎, PhantomV1 Game Engine, PhantomV1 Engine（官方网站：www.aixspace.com）
设计编写者 :   赵德贤(英文:Zhao De Xian)
MSN        :   yuzhou_995@hotmail.com
版权所有 赵德贤.  保留所有权利. 在购买幻影游戏引擎正版授权版本之后才可以使用本系统功能. 按照幻影的授权形式，可以获得本系统终身使用授权和无限制项目数量的开发和使用功能。
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __IDE_SOCKET_H__
#define __IDE_SOCKET_H__
#ifdef WIN32
#include <Windows.h>
#endif

#include <vector>
#include <string>

#define	InvalidSocket		INVALID_SOCKET
typedef	HANDLE				ThreadHandle;
typedef	CRITICAL_SECTION	Critical;
typedef	SOCKET				Socket;
extern	void				IDESleepThread(unsigned int millisecond);	//睡眠一段时间
#define	SOCKET_PACKAGE_MAX					141000

namespace PhantomV1{

	////连接状态定义
	//enum SocketError{
	//	SocketError_NoError = 0,
	//	SocketError_CreateFail,	//不能创建句柄
	//	SocketError_FailIP,	
	//	SocketError_FailConnect,
	//};
	extern	BOOL		IDESetLastSockError(DWORD dwError);
	extern	DWORD		IDEGetLastSockError();
	extern	const char*	IDEGetLastSockErrorText();
#pragma pack(push, 1)
	//网络连接类
	//struct	MessageID
	//{
	//	unsigned int	wSize;	//包大小
	//	WORD	wID;	//包ID
	//};
	//struct	Package
	//{
	//	enum{
	//		PackageType_ConnectFail	=	0,
	//		PackageType_ConnectOK,
	//		PackageType_RecvMessage,
	//		PackageType_DisconnectByServer,
	//		PackageType_Users,
	//	};
	//	unsigned char	type;
	//	WORD			wMessageID;
	//	Phantom::PtrID			id;
	//	unsigned int	readBytes;
	//	//unsigned char	bytes[SOCKET_PACKAGE_MAX];
	//	Package(){type = 0; readBytes = 0; wMessageID = 0; id = Phantom::PtrID(-1,-1);}
	//};
	//
#pragma pack(pop)
	struct IDE_Lock
	{
		IDE_Lock();
		~IDE_Lock();
		VOID				Lock();
		VOID				Unlock();
		Critical			crit;
	};
	struct	Lock
	{
		Lock(IDE_Lock& l){lk = &l;lk->Lock();}
		~Lock(){if(lk)lk->Unlock();}
		VOID	Unlock(){if(lk)lk->Unlock();lk = 0;}
		IDE_Lock* lk;
	};

	typedef VOID	(*PThreadCallback)(void* parameter);
	VOID			IDE_createThread(PThreadCallback cb, void* parameter);
	//线程控制类
	class	IDEThreadProc;
	class	IDEThread
	{
	public:
		IDEThread();
		virtual ~IDEThread(){Stop();}
		virtual	VOID			Start(IDEThreadProc* t);
		virtual	VOID			Stop(BOOL bImmediately = true);//make it immediately stop or wait next loop in thread onetime 
		virtual	VOID			OnThread();
		inline	BOOL			IsRunning(){return m_bThreadRunning;}
		static BOOL				s_IDEbThreadAllExit;	//停止一切线程运行
	protected:
		ThreadHandle			m_hThread;
		BOOL                    m_bThreadRunning;
		IDEThreadProc*				m_proc;
	};

	class	IDEThreadProc
	{
	public:
		IDEThreadProc(){}
		virtual ~IDEThreadProc(){m_thread.Stop();}
		virtual	VOID			OnThread() = 0;
		virtual	VOID			Start(){m_thread.Stop(); m_thread.Start(this);}
		virtual	VOID			Stop(BOOL bImmediately = true){m_thread.Stop(bImmediately);}
	public:
		IDEThread			m_thread;
	};

};

#endif
