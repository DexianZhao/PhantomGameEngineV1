//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
软件名称   :   幻影游戏引擎, PhantomGameEngine（www.aixspace.com）
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "IDESocket.h"

namespace PhantomV1{

	DWORD       g_dwLastSockError = 0;
	BOOL		IDESetLastSockError(DWORD dwError)
	{
		g_dwLastSockError = dwError;
		return true;
	}
	DWORD		IDEGetLastSockError()
	{
		return g_dwLastSockError;
	}
	const char*	IDEGetLastSockErrorText()
	{
		return "";
	}

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
	VOID			IDE_createThread(PThreadCallback cb, void* parameter)
	{
		ProcIn* p = new ProcIn();
		p->cb = cb;
		p->parameter = parameter;
		DWORD dwID = 0;
		p->h = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientSocketThreadProc2, p, 0, (LPDWORD)&dwID);
	}
	DWORD	WINAPI	ClientSocketThreadProc(LPVOID lpParameter)
	{
		IDEThread* s = (IDEThread*)lpParameter;
		s->OnThread();
		return 0;
	}

	IDE_Lock::IDE_Lock(){InitializeCriticalSection(&crit);}
	IDE_Lock::~IDE_Lock(){DeleteCriticalSection(&crit);}
	VOID			IDE_Lock::Lock(){::EnterCriticalSection(&crit);}
	VOID			IDE_Lock::Unlock(){::LeaveCriticalSection(&crit);}
	IDEThread::IDEThread()
	{
		m_proc				=	0;
		m_hThread			=	INVALID_HANDLE_VALUE;
		m_bThreadRunning	=	false;
	}
	BOOL			IDEThread::s_IDEbThreadAllExit = false;
	VOID			IDEThread::Start(IDEThreadProc* t)
	{
		if(m_bThreadRunning)
			Stop();
		//
		DWORD dwID	=	0;
		m_bThreadRunning	=	true;
		m_proc		=	t;
		m_hThread	=	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientSocketThreadProc, this, 0, (LPDWORD)&dwID);
	}
	VOID			IDEThread::Stop(BOOL bImmediately)
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
	VOID			IDEThread::OnThread()
	{
		m_proc->OnThread();
		CloseHandle(m_hThread);
		m_hThread			=	INVALID_HANDLE_VALUE;
		m_bThreadRunning	=	false;
	}
};

void						IDESleepThread(unsigned int millisecond)	//睡眠一段时间
{
	Sleep(millisecond);
}
