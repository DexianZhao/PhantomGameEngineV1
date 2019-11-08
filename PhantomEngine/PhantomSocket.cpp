//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <Windows.h>
#endif
#include "PhantomSocket.h"
#include "PhantomManager.h"

namespace Phantom{

	FnLink							g_link = 0;
	FnDislink						g_dislink = 0;
	FnMessage						g_message = 0;
	WORD							g_wClientID = 1;
	CDynamicArray<NetClient*>		g_clients;
	NetServer*						g_server = 0;
	DWORD							g_dwGuid = 0x1a2b3c4d;
	DWORD       g_dwLastSockError = 0;
	BOOL		SetLastSockError(DWORD dwError)
	{
		g_dwLastSockError = dwError;
		return true;
	}
	DWORD		GetLastSockError()
	{
		return g_dwLastSockError;
	}
	const char*	GetLastSockErrorText()
	{
		return "";
	}
	NetRecvBase::NetRecvBase()
	{
		m_bStopSend =	true;
		m_id		=	SocketID(-1,0);
		m_hSocket	=	InvalidSocket;
		m_guid		=	0;
		m_bClosed	=	false;
		m_apptime	=	0;
		m_message	=	0;//消息触发函数
		m_link		=	0;//连接相应函数
		m_dislink	=	0;//连接断开函数
	}
	VOID	NetRecvBase::Close(){
		m_bStopSend =	true;
		m_bClosed	=	true;
		if(m_hSocket	!=	InvalidSocket)
		{
			closesocket(m_hSocket);
			m_hSocket	=	InvalidSocket;
		}
	}

	BOOL	NetRecvBase::Send(WORD id, const char* pData, unsigned int wDataSize)//发送数据
	{
		if(m_hSocket != InvalidSocket && !m_bStopSend)
		{
			if(wDataSize>SOCKET_PACKAGE_MAX)
				return false;
			assert(sizeof(MessageID) == 6);
			char*	bytes = new char[SOCKET_PACKAGE_MAX+sizeof(MessageID)+12+512];
			MessageID* msg = (MessageID*)bytes;
			msg->wID = id;
			msg->wSize = wDataSize;
			if(wDataSize > 0)
				memcpy(bytes + sizeof(MessageID), pData, wDataSize);
			*(unsigned int*)(bytes+wDataSize+sizeof(MessageID)) = wDataSize;//验证长度
			*(unsigned int*)(bytes+wDataSize+sizeof(MessageID)+sizeof(unsigned int)) = m_guid;//验证代码
			//
			if(!m_bStopSend)
				send(m_hSocket, bytes, wDataSize+sizeof(MessageID)+sizeof(unsigned int)+sizeof(unsigned int), 0);
			int hour,minute,second;
			GetTime(hour, minute, second);
			this->m_apptime = hour*3600 + minute*60+second;
			delete[] bytes;
			return true;
		}
		return false;
	}
	int		NetRecvBase::onRecv(char* buffer)
	{
		MessageID msg;
		char* msg_c = (char*)&msg;
		int wLength = sizeof(MessageID);
		int nRecv	=	0;//recv(m_hSocket, (char*)&msg, sizeof(MessageID), 0);
		while(true)
		{
			int once = recv(m_hSocket, msg_c+nRecv, wLength-nRecv, 0);
			if(once <= 0)
				return 1;
			nRecv += once;
			if(nRecv >= wLength)
				break;
		}
		assert(msg.wSize <= SOCKET_PACKAGE_MAX);
		if(msg.wSize > SOCKET_PACKAGE_MAX)
			return 2;
		wLength = msg.wSize + sizeof(unsigned int) + sizeof(unsigned int);
		nRecv = 0;
		while(true)
		{
			int once = recv(m_hSocket, buffer+nRecv, wLength-nRecv, 0);
			if(once<=0)
				return 1;
			nRecv += once;
			if(nRecv >= wLength)
				break;
		}
		if(nRecv <= 0 || nRecv != wLength)
			return 3;
		if(nRecv == wLength)
		{
			unsigned int wSize2 = *(unsigned int*)(buffer + msg.wSize);
			unsigned int guid = *(unsigned int*)(buffer + msg.wSize + sizeof(unsigned int));
			if(wSize2 != msg.wSize || guid != m_guid)
				return 4;
			RecvBuffer(Package::PackageType_RecvMessage, msg.wID, buffer, msg.wSize, &m_id);
		}
		return 0;
	}
	VOID	NetRecvBase::OnLoopRecv()
	{
		m_bStopSend = false;
		int tv = 6000;//600000;//10分钟内如果没有任何消息响应则默认是断开，自动关闭连接
		//setsockopt(m_hSocket,	SOL_SOCKET,	SO_RCVTIMEO,	(char*)&tv,sizeof(int));
		int	length		=	SOCKET_PACKAGE_MAX + 1055;
		char* buffer = new char[SOCKET_PACKAGE_MAX + 1055];//	=	(char*)temp.bytes;
		unsigned int	wPoint	=	0;
		while(!g_manager.IsExitGame() && !this->m_bClosed)
		{
			int ret = onRecv(buffer);
			if(ret!=0){
				m_bStopSend = true;
				RecvBuffer(Package::PackageType_DisconnectByServer, ret, 0, 0, &m_id);
				break;
			}
		}
		m_bStopSend = true;
		delete[] buffer;
	}
	NetRecvMessage::NetRecvMessage()
	{
	}
	extern void		DoSearchMessage();
	VOID			NetRecvMessage::DoMessageQueue()
	{
		DoSearchMessage();
		CDynamicArray<unsigned char>* doRecv	=	0;
		{
			Lock l(s_recvLock);
			if(s_recvCurrent == &s_recvList[0])
			{
				doRecv			=	&s_recvList[0];
				s_recvCurrent	=	&s_recvList[1];
			}
			else
			{
				s_recvCurrent	=	&s_recvList[0];
				doRecv			=	&s_recvList[1];
			}
		}
		bool bSetZero = false;
		int maxsize = doRecv->size();
		int index = 0;
		char* temp = new char[SOCKET_PACKAGE_MAX];
		while(index<maxsize)//for(int i=0;i<doRecv->counts;i++)
		{
			bSetZero = true;
			Package p;//			=	doRecv->recvs[i];
			memcpy(&p, doRecv->GetPtr()+index, sizeof(Package));
			index += sizeof(Package);
			if(p.readBytes>0)			{
				memcpy(temp, doRecv->GetPtr()+index, p.readBytes);
				index += p.readBytes;
			}
            if(p.type==Package::PackageType_DisconnectByServer) {
                if(p.msgid==1){
                    //LogInfo("disconnBySvr");
                }
                else if(p.msgid==2){
                    //LogInfo("disconn Overflow");
                }
                else if(p.msgid==3){
                    //LogInfo("disconn data err");
                }
                else if(p.msgid==4){
                    //LogInfo("disconn size err");
                }
            }
			NetRecvBase* ptr	=	GetRecvByID(p.id);
			if(!ptr)
			{
				if(p.type == Package::PackageType_RecvMessage)
				{
					if(p.message)
						(*p.message)(p.id, p.msgid, (const char*)temp, p.readBytes);
					else if(g_message)
						(*g_message)(p.id, p.msgid, (const char*)temp, p.readBytes);
				}
				else
				{
					SocketID temp;
					temp.index = LWORD(p.id);
					temp.id = HWORD(p.id);
					switch(p.type)
					{
					case Package::PackageType_ConnectFail:
						if(g_dislink)
							(*g_dislink)(p.id);
						break;
					case Package::PackageType_ConnectOK:
						if(g_link)
							(*g_link)(p.id);
						break;
					case Package::PackageType_DisconnectByServer:
						if(g_dislink)
							(*g_dislink)(p.id);
						break;
					}

				}
				continue;
			}
			if(p.type == Package::PackageType_RecvMessage)
			{
				(*ptr->m_message)(p.id, p.msgid, (const char*)temp, p.readBytes);
				ptr	=	GetRecvByID(p.id);
				if(ptr)
					ptr->OnMessage(p.msgid, (const char*)temp, p.readBytes);
			}
			else
			{
				SocketID temp;
				temp.index = LWORD(p.id);
				temp.id = HWORD(p.id);
				switch(p.type)
				{
				case Package::PackageType_ConnectFail:
					(*ptr->m_dislink)(p.id);
					CloseLink(p.id);
					break;
				case Package::PackageType_ConnectOK:
					(*ptr->m_link)(p.id);
					break;
				case Package::PackageType_DisconnectByServer:
					(*ptr->m_dislink)(p.id);
					CloseLink(p.id);
					break;
				case Package::PackageType_AcceptClose:
					break;
				}
			}
		}
		assert(index==maxsize);
		if(bSetZero)
		{
			Lock l(s_recvLock);
			doRecv->SetArrayCount(0);
		}
		delete[] temp;
	}
	CDynamicArray<unsigned char>	NetRecvMessage::s_recvList[2];		//接收到的消息
	CDynamicArray<unsigned char>*	NetRecvMessage::s_recvCurrent = &s_recvList[0];
	PhantomLock				NetRecvMessage::s_recvLock;

	VOID			NetRecvMessage::RecvBuffer(unsigned char type, WORD wMessageID, void * pData, unsigned int wDataSize, SocketID* id)	//接收消息
	{
		Lock l(s_recvLock);
		int oldSize = s_recvCurrent->size();
		s_recvCurrent->SetArrayCount(oldSize+wDataSize+sizeof(Package));
		//if(s_recvCurrent->counts >= s_recvCurrent->alloced)
		//{
		//	s_recvCurrent->alloced += 100;
		//	s_recvCurrent->recvs.SetArrayCount(s_recvCurrent->alloced);
		//}
		{
			assert(wDataSize <= SOCKET_PACKAGE_MAX);
			Package c;
			c.type	=	type;
			c.msgid	=	wMessageID;
			c.readBytes	=	getmin(wDataSize, SOCKET_PACKAGE_MAX);
			if(id)
				c.id = MAKEDWORD(id->index, id->id);;
			//if(pData)
				//memcpy(c.bytes, pData, c.readBytes);
			c.message = this->m_message;
			memcpy(s_recvCurrent->GetPtr()+oldSize, &c, sizeof(Package));
			oldSize += sizeof(Package);
			if(c.readBytes>0)
				memcpy(s_recvCurrent->GetPtr()+oldSize, pData, c.readBytes);
			//
			int hour,minute,second;
			GetTime(hour, minute, second);
			this->m_apptime = hour*3600 + minute*60+second;
		}
	}
	VOID		NetServerLink::OnMessage(WORD wID, const char* dataPtr, unsigned int wSize)
	{
		if(m_server)
			this->m_server->OnMessage(wID, dataPtr, wSize, this->m_id);
	}
	//
	NetClient::NetClient()
	{
		m_state				=	0;
		m_bCloseByServer	=	false;					//关闭方式
		m_wPort				=	0;
	}
	NetClient::~NetClient()
	{
		Close();
	}
	VOID					NetClient::OnRecvBuffer(unsigned char type, const SocketID& id)
	{
		switch(type)
		{
		case Package::PackageType_ConnectFail:
			this->m_state	=	NetClientInfo::SocketState_NoConnect;
			OnConnected(false);
			LogInfo("system->connect-fail");
			break;
		case Package::PackageType_ConnectOK:
			this->m_state	=	NetClientInfo::SocketState_Connected;
			OnConnected(true);
			LogInfo("system->connect-ok");
			break;
		case Package::PackageType_DisconnectByServer:
			this->m_state	=	NetClientInfo::SocketState_NoConnect;
			OnDisconnect();
			LogInfo("system->disconnect");
			break;
		}
	}
	VOID					NetClient::StartThread()
	{
		Close();
		m_thread.Start(this);
	}

	VOID					NetClient::Connect(const char* szServerIP, WORD wPort, unsigned int guid)//连接服务器
	{
		Close();
		m_szIP		=	szServerIP;
		m_wPort		=	wPort;
		m_guid		=	guid;
		StartThread();
	}
	VOID					NetClient::OnThread()
	{
		if(!OnConnect(this->m_szIP, m_wPort))
		{
			RecvBuffer(Package::PackageType_ConnectFail, 0, 0, 0, &m_id);
			return;
		}
		{
			RecvBuffer(Package::PackageType_ConnectOK, 0, 0, 0, &m_id);
		}
		this->OnLoopRecv();
		NetRecvBase::Close();
	}
	BOOL					NetClient::OnConnect(const char* szServerIP, WORD wPort)
	{
		m_hSocket	=	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);//socket(m_info.af, m_info.type, m_info.protocol);//, NULL, 0, 0);
		if(m_hSocket == InvalidSocket)
		{
			SetLastSockError(SocketError_CreateFail);
			return false;
		}
		int maxBuffer	=	1024 * 512;
		int yes=0;
		setsockopt(m_hSocket,	SOL_SOCKET,	SO_REUSEADDR,	(char*)&yes,		sizeof(int));
		setsockopt(m_hSocket,	SOL_SOCKET,	SO_RCVBUF,		(char*)&maxBuffer,sizeof(int));
		setsockopt(m_hSocket,	SOL_SOCKET,	SO_SNDBUF,		(char*)&maxBuffer,sizeof(int));
		//
		sockaddr_in addr;
		memset(&addr,0,sizeof(sockaddr_in));
		addr.sin_addr.s_addr		=	inet_addr( szServerIP );
		if(addr.sin_addr.s_addr 	== 0)
		{
			hostent* ent = gethostbyname( szServerIP );
			if(ent == NULL)
			{
				SetLastSockError(SocketError_FailIP);
				closesocket(m_hSocket);
				m_hSocket = InvalidSocket;
				return false;
			}

			addr.sin_addr.s_addr	=	((in_addr*)ent->h_addr_list)->s_addr;
		}
		addr.sin_family					=	AF_INET;
		addr.sin_port					=	htons( wPort );
		//Block(m_hSocket, false);
		int	nError	=	::connect(m_hSocket, (sockaddr*)&addr, sizeof(sockaddr_in));
		if(nError<0){
			//LogInfo("A1(%s)", szServerIP);
			//LPVOID lpMsgBuf; 
			//FormatMessage( 
			//	FORMAT_MESSAGE_ALLOCATE_BUFFER |  
			//	FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,  
			//	NULL,  
			//	WSAGetLastError(),  
			//	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			//	(LPTSTR)&lpMsgBuf,   
			//	0,   
			//	NULL 
			//	);   
			//::MessageBox( NULL, (LPCTSTR)lpMsgBuf,"授权", MB_OK | MB_ICONINFORMATION );
			//LocalFree( lpMsgBuf);
			closesocket(m_hSocket);
			m_hSocket	=	InvalidSocket;
			SetLastSockError(SocketError_FailConnect);
			return false;
		}
		fd_set r;
		FD_ZERO(&r);
		FD_SET(m_hSocket,&r);
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec =0;
		nError = select(0,0,&r,0,&timeout);
		if(nError < 0)
		{
			closesocket(m_hSocket);
			m_hSocket	=	InvalidSocket;
			SetLastSockError(SocketError_FailConnect);
			return false;
		}
		//Block(m_hSocket, true);
		m_bClosed	=	false;
		return true;
	}
	VOID					NetClient::Close()
	{
		if(this->m_hSocket != InvalidSocket){
			//LogInfo("system->disconnect-own");
			this->OnDisconnect();
		}
	}

	NetServerLink::NetServerLink(NetServer* s){
		m_id			=	SocketID(-1,0);
		m_nIP			=	0;
		this->m_server	=	s;
	}
	NetServerLink::~NetServerLink()
	{
		this->Stop();
	}
	VOID				NetServerLink::GetIP(char* buf)
	{
		char* c = (char*)&m_nIP;
		sprintf(buf, "%d.%d.%d.%d", c[0], c[1], c[2], c[3]);
	}
	VOID				NetServerLink::Close()
	{
		m_bClosed = true;
		/*
		if(m_hSocket!=InvalidSocket)*/
		{
			//{
			//	Lock l(m_server->m_lockAccept);
			//	NetRecvBase::Close();
			//}
			OnClose();
			//Stop();
		}
	}
	void				NetServerLink::onAccept(unsigned int guid, unsigned int ip, const SocketID& id)
	{
		m_bClosed		=	false;
		this->m_guid	=	guid;
		this->m_nIP		=	ip;
		this->m_id		=	id;
	}
	VOID				NetServerLink::OnClose()
	{
		Socket sock = m_hSocket;
		if(m_hSocket!=InvalidSocket)
		{
			Lock l(m_server->m_lockAccept);
			m_hSocket = InvalidSocket;
			m_id = SocketID(-1,0);
			if(m_server->m_nCurrLinks>0)
				m_server->m_nCurrLinks--;
		}
		if(sock!=InvalidSocket)
			closesocket(sock);
	}
	VOID				NetServerLink::OnThread()
	{
		if(m_hSocket == InvalidSocket)return;
		this->OnLoopRecv();
		OnClose();
	}
	VOID				NetServerLink::RecvBuffer(unsigned char type, WORD wMessageID, void * pData, unsigned int wDataSize, SocketID* id)
	{
		this->m_server->RecvBuffer(type, wMessageID, pData, wDataSize, &this->m_id);
	}

	WORD	NetServer::s_wLinked = 1;
	NetServer::NetServer()
	{
		m_nCurrLinks	=	0;
		m_nMaxLinks	=	5;
		m_bListen	=	false;
		m_wPort		=	0;
		memset(m_links, 0, sizeof(m_links));
	}
	NetServer::~NetServer()
	{
		StopServer();
		for(int i=0;i<MAX_SERVICE_LINKS;i++)
		{
			if(m_links[i])
				m_links[i]->Close();
		}
	}
	NetServerLink*			NetServer::GetLink(const SocketID& id)
	{
		if(id.index<0||id.index>=MAX_SERVICE_LINKS)
			return 0;
		if(!m_links[id.index] || m_links[id.index]->m_id.id != id.id)
			return 0;
		return m_links[id.index];
	}
	BOOL					NetServer::GetLinkIP(const SocketID& id, char* ip)
	{
		if(id.index<0||id.index>=MAX_SERVICE_LINKS)
			return 0;
		if(!m_links[id.index] || m_links[id.index]->m_id.id != id.id)
			return 0;
		m_links[id.index]->GetIP(ip);
		return true;
	}

	NetServerLink*			NetServer::CreateNewLink()
	{
		return new NetServerLink(this);
	}
	//基础接口
	BOOL					NetServer::StartServer(WORD wPort, unsigned int guid)
	{
		this->m_guid	=	guid;
		StopServer();
		this->m_wPort	=	wPort;
		m_hSocket	=	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(m_hSocket == InvalidSocket){
			SetLastSockError(SocketError_CreateFail);
			return false;
		}
		int maxBuffer	=	5*1024 * 1024;
		int yes=1;
		setsockopt(m_hSocket,	SOL_SOCKET,	SO_REUSEADDR,	(char*)&yes, sizeof(int));
		setsockopt(m_hSocket,	SOL_SOCKET,	SO_RCVBUF,		(char*)&maxBuffer,sizeof(int));
		setsockopt(m_hSocket,	SOL_SOCKET,	SO_SNDBUF,		(char*)&maxBuffer,sizeof(int));
		sockaddr_in addr;
		memset(&addr,0,sizeof(sockaddr_in));
		addr.sin_addr.s_addr		=	htonl(INADDR_ANY);
		addr.sin_family				=	AF_INET;
		addr.sin_port				=	htons( wPort );
		int nbind = 0;
		if((nbind=bind(m_hSocket, (sockaddr*)&addr, sizeof(sockaddr_in))) == -1)
		{
			LogInfo("system->server bind fail(%d)", nbind);
			closesocket(m_hSocket);
			m_hSocket = InvalidSocket;
			return false;
		}
		if((nbind=listen(m_hSocket, 20)) == -1)
		{
			LogInfo("system->server listen fail(%d)", nbind);
			closesocket(m_hSocket);
			m_hSocket = InvalidSocket;
			return false;
		}
		m_nCurrLinks	=	0;
		this->Start();
		LogInfo("system->server Success(%d)!", wPort);
		return true;
	}
	VOID					NetServer::StopServer()
	{
		if(m_hSocket!=InvalidSocket)
		{
			LogInfo("system->server StopSocket()");
			this->Stop();
			m_bListen		=	false;
			for(int i=0;i<MAX_SERVICE_LINKS;i++)
			{
				if(m_links[i])
					m_links[i]->Close();
			}
			closesocket(m_hSocket);
			shutdown(m_hSocket, 1);
			m_hSocket	=	InvalidSocket;
		}
	}
	VOID					NetServer::OnRecvBuffer(unsigned char type, const SocketID& id)
	{
		if(Package::PackageType_DisconnectByServer == type)
		{
			this->OnClose(id);
			LogInfo("system->link-close(%d)", id.id);
		}
		else if(Package::PackageType_ConnectOK == type)
		{
			this->OnAccept(id);
			char szip[32];
			szip[0] = 0;
			GetLinkIP(id, szip);
			LogInfo("system->link-accept(%d)(%s)", id.id, szip);
		}
	}
	BOOL					NetServer::Send(const SocketID& sid, WORD id, const char* pData, unsigned int wDataSize)//发送数据
	{
		if(sid.index<0||sid.index>=MAX_SERVICE_LINKS)
			return 0;
		if(!m_links[sid.index] || m_links[sid.index]->m_id.id != sid.id)
			return 0;
		return m_links[sid.index]->Send(id, pData, wDataSize);
	}
	VOID					NetServer::OnThread()
	{
		while(!g_manager.IsExitGame() && IsRunning() && m_thread.IsRunning())
		{
			sockaddr_in addr;
			memset(&addr,0,sizeof(sockaddr_in));
#ifdef WIN32_OS
			int len = sizeof(sockaddr_in);
#else
			socklen_t len = sizeof(sockaddr_in);
#endif
			Socket fd = accept(m_hSocket, (sockaddr*)&addr, &len);
			if(fd == -1)
			{
                RecvBuffer(Package::PackageType_AcceptClose, 0, 0, 0, 0);
				//LogInfo("accept close");
				Close();//关闭服务器
				break;
			}
			if(m_nCurrLinks >= this->m_nMaxLinks)
			{
				closesocket(fd);
				continue;
			}
			SocketID id;
			{
				
				this->m_nCurrLinks++;
				NetServerLink* links = 0;
				int nFree = -1;
				for(int i=0;i<MAX_SERVICE_LINKS;i++)
				{
					if(!m_links[i])
					{
						if(nFree==-1)
							nFree = i;
						continue;
					}
					{
						Lock l(m_lockAccept);
						if(this->m_links[i]->IsRunning())
							continue;
					}
					links = m_links[i];
					id = SocketID(i, s_wLinked++);
					break;
				}
				if(links == 0 && nFree < 0)
				{
					//连接用户过多
					closesocket(fd);
					continue;
				}
				if(!links){
					Lock l(m_lockAccept);
					m_links[nFree] = this->CreateNewLink();
					links = m_links[nFree];
					id = SocketID(nFree, s_wLinked++);
				}
				links->m_link = m_link ? m_link : g_link;
				links->m_dislink = m_dislink ? m_dislink : g_dislink;
				links->m_message = m_message ? m_message : g_message;
				links->onAccept(m_guid, addr.sin_addr.s_addr, id);//.S_un.S_addr, id);
				{
					Lock l(m_lockAccept);
					links->m_hSocket	=	fd;
				}
                int maxBuffer	=	(20 * 1024 * 1024/m_nMaxLinks);
                if(maxBuffer<1024*128)
                    maxBuffer=1024*128;
                setsockopt(links->m_hSocket,	SOL_SOCKET,	SO_RCVBUF,		(char*)&maxBuffer,sizeof(int));
				links->Start();
			}
			RecvBuffer(Package::PackageType_ConnectOK, 0, 0, 0, &id);
		}
		RecvBuffer(Package::PackageType_AcceptClose, 0, 0, 0, 0);
		//LogInfo("accept thread close");
	}
	VOID		NetServer::CloseWait(SocketID& sid, int nWait)
	{
		if(sid.index<0||sid.index>=MAX_SERVICE_LINKS)
			return;
		if(!m_links[sid.index] || m_links[sid.index]->m_id.id != sid.id)
			return;
		setsockopt(m_links[sid.index]->m_hSocket,	SOL_SOCKET,	SO_RCVTIMEO, (char*)&nWait, sizeof(int));
	}
	VOID		NetServer::StopLink(SocketID& sid)
	{
		if(sid.index<0||sid.index>=MAX_SERVICE_LINKS)
			return;
		if(!m_links[sid.index] || m_links[sid.index]->m_id.id != sid.id)
			return;
		m_links[sid.index]->Close();
		//PhantomLock l(m_lockAccept);
		//delete m_links[sid.index];
		//m_links[sid.index] = 0;
	}
	VOID		NetServer::OnAccept(const SocketID& id)										//网络连接消息，单线程中运作
	{
		if(g_link)
			(*g_link)(MAKEDWORD(id.index, id.id));
	}
	VOID		NetServer::OnClose(const SocketID& id)										//网络关闭消息，单线程中运作
	{
		if(g_dislink)
			(*g_dislink)(MAKEDWORD(id.index, id.id));
	}
	VOID		NetClient::OnConnected(BOOL bConnectOK)
	{
		if(m_link && bConnectOK)
			(*m_link)(MAKEDWORD(m_id.index, m_id.id));
		else if(!bConnectOK&&m_dislink)
			(*m_dislink)(MAKEDWORD(m_id.index, m_id.id));
	}
	VOID		NetClient::OnDisconnect()
	{
		if(m_dislink)
			(*m_dislink)(MAKEDWORD(m_id.index, m_id.id));
		m_thread.Stop();
		NetRecvBase::Close();
	}
	BOOL	Send(DWORD linkid, WORD wID, const char* dataPtr, unsigned int wSize)//一次最多可以发送8190个字节
	{
		SocketID temp;
		temp.index = LWORD(linkid);
		temp.id = HWORD(linkid);
		if(temp.index >= 10000)//小于10000是客户端
		{
			int newIndex = temp.index-10000;
			if(newIndex>=g_clients.size() || g_clients[newIndex] == 0 || g_clients[newIndex]->m_id.id != temp.id)
				return false;
			return g_clients[newIndex]->Send(wID, dataPtr, wSize);
		}
		else if(g_server){
			return g_server->Send(temp, wID, dataPtr, wSize);
		}
		return false;
	}

	NetRecvBase*								GetRecvByID(DWORD linkid)
	{
		SocketID temp;
		temp.index = LWORD(linkid);
		temp.id = HWORD(linkid);
		if(temp.index >= 10000)//小于10000是客户端
		{
			int newIndex = temp.index-10000;
			if(newIndex>=g_clients.size() || g_clients[newIndex] == 0 || g_clients[newIndex]->m_id.id != temp.id)
				return 0;
			return g_clients[newIndex];
		}
		else if(g_server){
			return g_server->GetLink(temp);
		}
		return 0;
	}

	VOID	CloseLink(DWORD linkid)//关闭服务器
	{
		SocketID temp;
		temp.index = LWORD(linkid);
		temp.id = HWORD(linkid);
		if(temp.index >= 10000)//小于10000是客户端
		{
			int newIndex = temp.index-10000;
			if(newIndex>=g_clients.size() || g_clients[newIndex] == 0 || g_clients[newIndex]->m_id.id != temp.id)
				return;
			delete g_clients[newIndex];
			g_clients[newIndex] = 0;
		}
		else if(g_server){
			g_server->StopLink(temp);
		}
	}
	VOID	StopAllSocket()//关闭所有的服务器跟客户端
	{
		for(int i=0;i<g_clients.size();i++)
		{
			if(g_clients[i]){
				delete g_clients[i];
				g_clients[i] = 0;
			}
		}
		if(g_server)
			delete g_server;
		g_server = 0;
	}
	VOID	SetupSocket(FnLink link, FnDislink dislink, FnMessage message, DWORD dwGUID)
	{
		g_dwGuid = dwGUID;
		g_link = link;
		g_dislink = dislink;
		g_message = message;
	}
	//服务器端接口逻辑
	BOOL	StartServer(const char* name, WORD wPort)//开启服务器
	{
		if(g_server)
			return true;
		g_server = new NetServer();
		g_server->m_info.name = name;
		g_server->m_info.state = "normal";
		if(!g_server->StartServer(wPort, g_dwGuid))
		{
			delete g_server;
			g_server = 0;
			return false;
		}
		return true;
	}
	//客户端接口逻辑
	DWORD	StartClient(const char* szIP, WORD wPort, FnLink link, FnDislink dislink, FnMessage message, DWORD dwGUID)//开启客户端，返回一个客户端唯一linkid
	{
		int cid = -1;
		for(int i=0;i<g_clients.size();i++)
		{
			if(g_clients[i] == 0)
			{
				cid = i;
				break;
			}
		}
		if(cid<0)
		{
			cid = g_clients.size();
			NetClient* sj = 0;
			g_clients.push_back(sj);
		}
		if(!link)
			link = g_link;
		if(!dislink)
			dislink = g_dislink;
		if(!message)
			message = g_message;
		g_clients[cid] = new NetClient();
		g_clients[cid]->m_id.index = cid + 10000;
		g_clients[cid]->m_id.id = g_wClientID++;
		if(dwGUID==0)
			dwGUID = g_dwGuid;
		g_clients[cid]->Connect(szIP, wPort, dwGUID);
		g_clients[cid]->m_link = link;
		g_clients[cid]->m_dislink = dislink;
		g_clients[cid]->m_message = message;
		return MAKEDWORD(g_clients[cid]->m_id.index, g_clients[cid]->m_id.id);
	}



struct SearchData{
	char	ip[16];
	DWORD	linkid;
};
std::vector<ServerInfo>		g_searchInfos;
std::vector<SearchData>		g_searchDatas;
FnSearchComplete			g_searchComplete = 0;
DWORD	g_searchLinkID = 0;
int		g_searchDisconnect = 0;
BOOL	g_searchBegin = 0;
VOID	CloseWait(DWORD linkid, INT nMillisecond)//设置关闭时间
{
	SocketID temp;
	temp.index = LWORD(linkid);
	temp.id = HWORD(linkid);
	if(temp.index >= 10000)//小于10000是客户端
	{
		int newIndex = temp.index-10000;
		if(newIndex>=g_clients.size() || g_clients[newIndex] == 0 || g_clients[newIndex]->m_id.id != temp.id)
			return;
		if(nMillisecond>0)
			setsockopt(g_clients[newIndex]->m_hSocket,	SOL_SOCKET,	SO_RCVTIMEO, (char*)&nMillisecond, sizeof(int));
		else
		{
			g_clients[newIndex]->Close();
			delete g_clients[newIndex];
			g_clients[newIndex] = 0;
		}
	}
	else if(g_server){
		g_server->StopLink(temp);
	}
}
VOID	OnSearchLink(DWORD linkid)//某个客户端连接时被调用
{
	//LogInfo("SearchLink(%d)", linkid);
	//Send(linkid, MsgID_cs_ServerInfo, 0, 0);
	g_searchLinkID = linkid;
	CloseWait(linkid, 1000);//最多1秒钟之后强制关闭
}
int	g_searchConnect = 0;
VOID	DoSearchMessage(){
	if(g_searchBegin){
		if(g_searchLinkID){
			//LogInfo("SearchOK(%d)", g_searchLinkID);
			for(int i=0;i<g_searchDatas.size();i++)
			{
				if(g_searchDatas[i].linkid == g_searchLinkID)
				{
					(*g_searchComplete)(g_searchDatas[i].ip);//&g_searchInfos.at(0), g_searchInfos.size());//info->ip = g_searchDatas[i].ip;
					break;
				}
			}
			g_searchBegin = false;
		}
	}
}
VOID	OnSearchDislink(DWORD linkid)//某个客户端断开连接时被调用
{
	g_searchDisconnect++;
	if(g_searchDisconnect>=g_searchConnect && g_searchComplete)
	{
		//if(g_searchLinkID>0)//g_searchInfos.size() > 0)
		//{
		BOOL bOK = (g_searchLinkID>0);
		//for(int i=0;i<g_searchDatas.size();i++)
		//{
		//	if(g_searchDatas[i].linkid == linkid)
		//	{
		//		bOK = true;
		//		(*g_searchComplete)(g_searchDatas[i].ip);//&g_searchInfos.at(0), g_searchInfos.size());//info->ip = g_searchDatas[i].ip;
		//		break;
		//	}
		//}
		//(*g_searchComplete)(&g_searchInfos.at(0), g_searchInfos.size());
		//}
		//else
		if(!bOK)
			(*g_searchComplete)(0);
		g_searchComplete = 0;
	}
}
VOID	OnSearchMessage(DWORD linkid, WORD wID, const char* dataPtr, unsigned int wSize)//某个客户端处理消息时被调用
{
	switch(wID)
	{
	case MsgID_cs_ServerInfo:
		{
			ServerInfo* info = (ServerInfo*)dataPtr;
			for(int i=0;i<g_searchDatas.size();i++)
			{
				if(g_searchDatas[i].linkid == linkid)
				{
					info->ip = g_searchDatas[i].ip;
					break;
				}
			}
			g_searchInfos.push_back(*info);
			CloseLink(linkid);
		}
		break;
	}
}

BOOL	SearchServers(FnSearchComplete fn, WORD wPort)
{
	if(g_searchComplete)
		return false;
	g_searchComplete = fn;
	char ip[128];
	if(!GetLocalIP(ip))
		return false;
	if(ip[0]==0)
		return 0;
	unsigned char ucip[8];
	const char* last = ip;
	int index = 0;
	for(int i=0;i<16;i++)
	{
		if(ip[i] == '.' || ip[i] == 0)
		{
			ucip[index++] = atoi(last);
			ip[i] = 0;
			last = ip + i+1;
		}
	}
	g_searchInfos.clear();
	g_searchDisconnect = 0;
	g_searchConnect = 0;
	int searchcnt = 256;
	g_searchDatas.clear();
	g_searchBegin = true;
	g_searchLinkID = 0;
	for(int i=2;i<searchcnt;i++){
		g_searchConnect++;
	}
	for(int i=2;i<searchcnt;i++){
		sprintf(ip, "%d.%d.%d.%d", ucip[0], ucip[1], ucip[2], i);
		SearchData sd;
		sd.linkid = StartClient(ip, wPort, OnSearchLink, OnSearchDislink, OnSearchMessage, g_dwGuid);
		strcpy(sd.ip, ip);
		//LogInfo("Search(%d)%s", i, ip);
		g_searchDatas.push_back(sd);
	}
	return true;
}














	NetToAI::NetToAI()//构造函数
	{
		memset(&m_info, 0, sizeof(m_info));
		m_callback			=	0;
		m_current		=	&m_recvs[0];
	}
	NetToAI::~NetToAI()
	{
		m_aiControl	=	0;
	}//析构函数

	//基础接口
	VOID					NetToAI::DoMessageQueue()
	{
		CDynamicArray<PackageAI>*			doRecv	=	0;
		m_lock.Lock();
		{
			if(m_current == &m_recvs[0])
			{
				doRecv		=	&m_recvs[0];
				m_current	=	&m_recvs[1];
			}
			else
			{
				m_current	=	&m_recvs[0];
				doRecv		=	&m_recvs[1];
			}
		}
		m_lock.Unlock();
		BOOL bSetZero = false;
		if(m_callback)
		{
			for(int i=0;i<doRecv->size();i++)
			{
				bSetZero = true;
				PackageAI& p	=	(*doRecv)[i];
				LogInfo("system->User DoMessage PackageAI Buffer size(%d)", p.readBytes);
				switch(p.type)
				{
				case PackageAI::PackageType_ConnectFail:
					this->m_info.state	=	NetClientInfo::SocketState_NoConnect;
					m_callback->OnSocketConnect(true, this);
					break;
				case PackageAI::PackageType_ConnectOK:
					this->m_info.state	=	NetClientInfo::SocketState_Connected;
					m_callback->OnSocketConnect(false, this);
					break;
				case PackageAI::PackageType_RecvMessage:
					m_callback->OnSocketRead(p.bytes, p.readBytes, this);
					break;
				case PackageAI::PackageType_DisconnectByServer:
					this->m_info.state	=	NetClientInfo::SocketState_NoConnect;
					m_callback->OnSocketClose(true, this);
					break;
				}
				LogInfo("system->User DoMessage OK(%d)", p.readBytes);
			}
		}
		if(bSetZero)
		{
			LogInfo("system->User SetZero Begin");
			m_lock.Lock();
			doRecv->SetArrayCount(0);
			m_lock.Unlock();
			LogInfo("system->User SetZero End");
		}
	}
	VOID					NetToAI::RecvBuffer(unsigned char type, void * pData, WORD wDataSize)	//接收消息
	{
		m_lock.Lock();
		if(m_current && m_current->size() < 10000)
		{
			assert(wDataSize < SOCKET_PACKAGE_MAX);
			PackageAI c;
			c.type	=	type;
			c.readBytes	=	getmin(wDataSize, SOCKET_PACKAGE_MAX);
			if(pData)
				memcpy(c.bytes, pData, c.readBytes);
			m_current->push_back(c);
			//LogInfo("system->User Recv Buffer size(%d)", wDataSize);
		}
		m_lock.Unlock();
	}
	VOID					NetToAI::ConnectAI(AIControls* aiPtr, NetAICB* cb)
	{
		m_callback	=	cb;
		m_aiControl	=	aiPtr;
		RecvBuffer(PackageAI::PackageType_ConnectOK, 0, 0);
		if(aiPtr)
			aiPtr->m_netPtr	=	this;
	}	//连接AI控制器指针
	BOOL					NetToAI::SendBuffer(void * pData, WORD wDataSize)
	{
		if(!m_aiControl.t)
			return false;
		//LogInfo("system->User Send Buffer size(%d)", wDataSize);
		m_aiControl->RecvBuffer(PackageAI::PackageType_RecvMessage, pData, wDataSize);
		return true;
	}//发送数据,会发送到AI控制器消息队列中
	VOID					NetToAI::OnThread()
	{
	}		//不处理线程
	VOID					NetToAI::StartThread()
	{
	}	//不开线程
	BOOL					NetToAI::OnConnect(const char* szServerIP, WORD wPort)
	{
		return true;
	}//连接服务器//默认成功
	INT						NetToAI::Recv(char* buffer, int bufferLength)
	{
		return 0;
	}	//从AI控制器获得数据
	VOID					NetToAI::Close()
	{
		m_aiControl	=	0;
	}
	AIControls::AIControls()
	{
		m_dLastAppTime	=	g_manager.GetAppTime();
		m_current	=	&m_recvs[0];
		for(int i=0;i<2;i++)
			m_recvs[i].SetArrayCount(0);
	}
	VOID					AIControls::OnLoopOnce()
	{
		{
			CDynamicArray<PackageAI>*			doRecv = 0;
			m_lock.Lock();
			{
				if(m_current == &m_recvs[0])
				{
					doRecv		=	&m_recvs[0];
					m_current	=	&m_recvs[1];
				}
				else
				{
					m_current	=	&m_recvs[0];
					doRecv		=	&m_recvs[1];
				}
			}
			m_lock.Unlock();
			//
			for(int i=0;i<doRecv->size();i++)
			{
				PackageAI& p	=	(*doRecv)[i];
				switch(p.type)
				{
				case PackageAI::PackageType_RecvMessage:
					assert(p.readBytes < SOCKET_PACKAGE_MAX);
					OnMessage(p.bytes, p.readBytes);
					break;
				}
			}
			doRecv->SetArrayCount(0);
		}
		double dAppTime		=	g_manager.GetAppTime();
		double dElapsedTime	=	dAppTime - m_dLastAppTime;
		m_dLastAppTime		=	dAppTime;
		this->OnThink(dAppTime, dElapsedTime);
	}
	VOID					AIControls::OnThread()//AI控制器,开线程
	{
		m_dLastAppTime	=	g_manager.GetAppTime();
		//处理队列中的所有消息
		OnInit();
		m_current	=	&m_recvs[0];
		for(int i=0;i<2;i++)
			m_recvs[i].SetArrayCount(0);
		while(!g_manager.IsExitGame() && this->m_thread.IsRunning())
		{
			OnLoopOnce();
			SleepThread(25);//休息0.025秒，减少计算压力,每秒钟理论上40此计算
		}
	}

	BOOL					AIControls::SendBuffer(void * pData, WORD wDataSize)//发送数据,会发送到AI控制器消息队列中
	{
		if(!m_netPtr.t)
			return false;
		//LogInfo("system->AI Send Buffer size(%d)", wDataSize);
		m_netPtr->RecvBuffer(PackageAI::PackageType_RecvMessage, pData, wDataSize);
		return true;
	}
	VOID					AIControls::Stop(BOOL bImmediately)
	{
		ThreadProc::Stop(bImmediately);
		this->m_netPtr	=	0;
	}
	VOID					AIControls::RecvBuffer(unsigned char type, void * pData, WORD wDataSize)	//接收消息
	{
		m_lock.Lock();
		if(m_current && m_current->size() < 10000)
		{
			//LogInfo("system->AI Recv Buffer size(%d)", wDataSize);
			PackageAI c;
			c.type	=	type;
			assert(wDataSize < SOCKET_PACKAGE_MAX);
			c.readBytes	=	getmin(wDataSize, SOCKET_PACKAGE_MAX);
			if(pData)
				memcpy(c.bytes, pData, c.readBytes);
			m_current->push_back(c);
		}
		m_lock.Unlock();
	}
};
