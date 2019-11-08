//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomSocket.h"
#include "PhantomManager.h"

#ifdef ANDROID_OS

#include "netinet/in.h"
#include "arpa/inet.h"
#include "sys/socket.h"
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

void* SocketThreadProc(void* param)
{
    Phantom::Thread* v = (Phantom::Thread*)param;
    v->OnThread();
    pthread_detach(pthread_self());
    return 0;
}
VOID SleepThread(unsigned int s)
{
    usleep(s * 1000);
}

namespace Phantom{

	struct ProcIn{
		PThreadCallback cb;
		void* parameter;
	};
	void		Block(Socket s, BOOL bBlock)//设置是否进入阻塞模式
	{
		int blockFlag = fcntl(s, F_GETFL, 0);
		if(bBlock){
			blockFlag &= ~O_NONBLOCK;
			fcntl(s, F_SETFL, blockFlag);
		}
		else
		{
			blockFlag |= O_NONBLOCK;
			fcntl(s, F_SETFL, blockFlag);
		}
	}

	void* SocketThreadProc2(void* param)
	{
		ProcIn* s = (ProcIn*)param;
		(*s->cb)(s->parameter);
		delete s;
		pthread_detach(pthread_self());
		return 0;
	}
	VOID			createThread(PThreadCallback cb, void* parameter)
	{
		ProcIn* p = new ProcIn();
		p->cb = cb;
		p->parameter = parameter;
		pthread_t temp;
		memset(&temp, 0, sizeof(pthread_t));
		pthread_create(&temp, 0, SocketThreadProc2, (void*)p);
	}
	PhantomLock::PhantomLock(){pthread_mutex_init(&crit, 0);}
	PhantomLock::~PhantomLock(){pthread_mutex_destroy(&crit);}
	VOID				PhantomLock::Lock(){::pthread_mutex_lock(&crit);}
	VOID				PhantomLock::Unlock(){::pthread_mutex_unlock(&crit);}
	Thread::Thread()
	{
		m_proc	=	0;
		memset(&m_hThread, 0, sizeof(pthread_t));
		m_bThreadRunning	=	false;
	}
	BOOL			Thread::s_bThreadAllExit = false;
	VOID			Thread::Start(ThreadProc* t)
	{
		if(m_bThreadRunning)
			Stop();
		//
		m_bThreadRunning	=	true;
		m_proc	=	t;
		pthread_create(&m_hThread, 0, SocketThreadProc, (void*)this);
	}
	VOID			Thread::Stop(BOOL bImmediately)
	{
		if(this->m_bThreadRunning)
		{
			if(bImmediately)
			{
				pthread_kill(m_hThread, 0);
				memset(&m_hThread, 0, sizeof(m_hThread));
			}
			m_bThreadRunning	=	false;
		}
	}
	VOID			Thread::OnThread()
	{
		m_proc->OnThread();
		m_bThreadRunning	=	false;
	}

};

#endif
