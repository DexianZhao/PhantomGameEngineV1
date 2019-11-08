//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <Windows.h>
#include "PhantomSocket.h"
#include "PhantomManager.h"

namespace Phantom{
	struct ProcIn{
		PThreadCallback cb;
		void* parameter;
		HANDLE h;
	};
	DWORD	WINAPI	ClientSocketThreadProc2(LPVOID lpParameter)
	{
		ProcIn* s = (ProcIn*)lpParameter;
		(*s->cb)(s->parameter);
		CloseHandle(s->h);
		delete s;
		return 0;
	}
	VOID			createThread(PThreadCallback cb, void* parameter)
	{
		ProcIn* p = new ProcIn();
		p->cb = cb;
		p->parameter = parameter;
		DWORD dwID = 0;
		p->h = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientSocketThreadProc2, p, 0, (LPDWORD)&dwID);
	}
	DWORD	WINAPI	ClientSocketThreadProc(LPVOID lpParameter)
	{
		Thread* s = (Thread*)lpParameter;
		s->OnThread();
		return 0;
	}
	void		Block(Socket s, BOOL bBlock)//设置是否进入阻塞模式
	{
		u_long mode = 0;
		if(!bBlock)
			mode = 1;
		ioctlsocket(s, FIONBIO,&mode);
	}
	PhantomLock::PhantomLock(){InitializeCriticalSection(&crit);}
	PhantomLock::~PhantomLock(){DeleteCriticalSection(&crit);}
	VOID			PhantomLock::Lock(){::EnterCriticalSection(&crit);}
	VOID			PhantomLock::Unlock(){::LeaveCriticalSection(&crit);}
	Thread::Thread()
	{
		m_proc				=	0;
		m_hThread			=	INVALID_HANDLE_VALUE;
		m_bThreadRunning	=	false;
	}
	BOOL			Thread::s_bThreadAllExit = false;
	VOID			Thread::Start(ThreadProc* t)
	{
		if(m_bThreadRunning)
			Stop();
		//
		DWORD dwID	=	0;
		m_bThreadRunning	=	true;
		m_proc		=	t;
		m_hThread	=	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientSocketThreadProc, this, 0, (LPDWORD)&dwID);
	}
	VOID			Thread::Stop(BOOL bImmediately)
	{
		if(this->m_bThreadRunning)
		{
			if(m_hThread != INVALID_HANDLE_VALUE && bImmediately)
			{
				TerminateThread(m_hThread, 0);
				CloseHandle(m_hThread);
				m_hThread			=	INVALID_HANDLE_VALUE;
			}
			m_bThreadRunning	=	false;
		}
	}
	VOID			Thread::OnThread()
	{
		m_proc->OnThread();
		CloseHandle(m_hThread);
		m_hThread			=	INVALID_HANDLE_VALUE;
		m_bThreadRunning	=	false;
	}


};

void						SleepThread(unsigned int millisecond)	//睡眠一段时间
{
	Sleep(millisecond);
}
BOOL						GetLocalIP(char* ret)//返回本机局域网IP
{
	::gethostname(ret, 128);
	hostent* ent = ::gethostbyname(ret);
	if(!ent)
		return 0;
	DWORD addr = ((in_addr*)ent->h_addr_list)->s_addr;
	sockaddr_in in;
	memcpy(&in.sin_addr.s_addr, ent->h_addr_list[0], ent->h_length);
	strcpy(ret, inet_ntoa(in.sin_addr));
	return true;
}
#endif

