//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
Copyright 2009-2016 Zhao Dexian


-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <Windows.h>
#include <dshow.h>
#include "PhantomWin32.h"
#endif

#include "PhantomOS.h"
#include "PhantomManager.h"
#ifndef EXPORT_ENGINE
namespace	Phantom{

#ifdef WIN32_OS
	class PhantomMusicOS: public MusicControl
	{
	public:
		PhantomMusicOS();
		virtual ~PhantomMusicOS();
		//
		virtual	VOID					Play(INT nLoopCount = -1, float fVolume = 1.0f, float fPan = 0.0f, float fRate = 1.0f);
		virtual	VOID					Stop();
		virtual	VOID					Pause(){Pause(true);}
		virtual	VOID					Resume(){Pause(false);}
		virtual	BOOL					IsPlaying();
		virtual	VOID					SetVolume(float fVolume);
		virtual	VOID					SetPan(float fPan);
		virtual	BOOL					Open(const char* szFile);
		virtual	BOOL					IsPause(){return m_bIsPause;}
		//
		BOOL			m_bIsPause;
		void			Pause(bool b)
		{
			if(IsPlaying())
			{
				m_bIsPause	=	b;
				m_bUserPlay	=	b;
				if(m_pMediaControl)
				{
					if(b)
						m_pMediaControl->Pause();
					else
						m_pMediaControl->Run();
				}
			}
		}
		VOID											SetLoopCount(INT loopCount){m_nLoopCount = loopCount;}
		INT												GetLoopCount(){return m_nLoopCount;}
		float											GetVolume();
		float											GetPan();

		bool											PlayMusic();
		bool											IsPlayOver(){return m_bPlayOver;}
		void											SetPlaySpeed(float fSpeed);

		void											FrameMove();
		BOOL											CreateMedia();
		void											ReleaseMedia();
	public:
		BOOL											m_bPlaying;
		enum {EBC_DEFAULT, EBC_EXIT, EBC_STOP};
		DWORD											m_dwCmd;
		//
		float											m_fVolume;
		float											m_fPan;
		//
		Phantom::FileName										m_fileName;
		bool											m_bPlayOver;
		INT												m_nSaveGlobalVolume;
		INT												m_nLoopCount;
		IGraphBuilder*									m_pGraph;
		IMediaControl*									m_pMediaControl;
		IMediaEvent*									m_pEvent;
		IMediaSeeking*									m_pSeek;
		IBasicAudio*									m_pAudio;
		BOOL											m_bUserPlay;
		//
		BOOL											m_bIsLoadOK;
		BOOL											m_bIsInit;
		float											m_fPlayRate;
		BOOL											m_bIsUsing;
	};

	PhantomMusicOS::PhantomMusicOS()
	{
		m_bIsPause	=	false;
		m_bIsUsing	=	false;
		m_bPlayOver	=	false;
		m_nSaveGlobalVolume	=	0;
		m_bPlaying	=	FALSE;
		m_dwCmd		=	EBC_DEFAULT;
		m_pGraph	=	NULL;
		m_pMediaControl	=	NULL;
		m_pSeek		=	NULL;
		m_pAudio	=	NULL;
		m_fVolume	=	1.0f;
		m_fPan		=	0.0f;
		m_nLoopCount	=	-1;
		m_bUserPlay	=	false;
		m_bIsLoadOK	=	false;
		m_bIsInit	=	false;
		m_fPlayRate	=	1.0f;
	}

	PhantomMusicOS::~PhantomMusicOS()
	{
		ReleaseMedia();
	}

	struct W
	{
		wchar_t* buf;
		W(const char* w)
		{

			int slen = (int)strlen(w);
			int len = (int)slen * 2 + 1;
			buf = new wchar_t[len];
			len = ::MultiByteToWideChar(CP_ACP, 0, w, slen, buf, len);
			buf[len] = 0;
		}
		inline	operator const wchar_t*(){return buf;}
		~W(){if(buf)delete[] buf;}
	};

	BOOL PhantomMusicOS::CreateMedia()
	{
		if(m_pGraph)
			return true;
		if (FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 	IID_IGraphBuilder, (void **)&m_pGraph)))
		{
			return FALSE;
		}
		m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
		m_pGraph->QueryInterface(IID_IMediaSeeking, (void **)&m_pSeek);
		m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&m_pEvent);
		m_pGraph->QueryInterface(IID_IBasicAudio, (void **)&m_pAudio);
		if ((!m_pMediaControl) || (!m_pEvent) || (!m_pSeek) || (!m_pAudio))
		{
			return FALSE;
		}
		if (FAILED(m_pGraph->RenderFile(W(m_fileName), NULL)))
		{
			LFileName f	=	GetProjectDir();
			f	+=	m_fileName.t;
			if (FAILED(m_pGraph->RenderFile(W(f), NULL)))
				return FALSE;
		}
		return TRUE;
	}

	void	PhantomMusicOS::ReleaseMedia()
	{
		if (m_pGraph)
		{
			safe_release(m_pAudio);
			safe_release(m_pEvent);
			safe_release(m_pSeek);
			safe_release(m_pMediaControl);
			safe_release(m_pGraph);
		}
	}
	//
	BOOL	PhantomMusicOS::IsPlaying()
	{
		if(!m_pEvent || !m_bPlaying)
			return false;
		long lEventCode;
		LONG_PTR lParam1;
		LONG_PTR lParam2;
		m_pEvent->GetEvent(&lEventCode, &lParam1, &lParam2, 0);
		return (EC_COMPLETE != lEventCode);
	}

	extern	BOOL	IsPlayMusic();

	void	PhantomMusicOS::FrameMove()
	{
		bool bRet	=	true;
		if(!m_bIsLoadOK)
			return;
		if(!this->m_pGraph)
			return;
		if(!this->m_bIsInit)
		{
			m_bIsInit	=	true;
			if(m_bUserPlay)
			{
				PlayMusic();
				this->SetVolume(this->m_fVolume);
				this->SetPan(this->m_fPan);
				if(m_pSeek)
					m_pSeek->SetRate(m_fPlayRate);
			}
			bRet		=	false;
		}
		if(!m_pEvent)return;
		if(!m_bPlaying)return;
		//
		long lEventCode;
		LONG_PTR lParam1;
		LONG_PTR lParam2;
		//
		m_pEvent->GetEvent(&lEventCode, &lParam1, &lParam2, 0);
		m_bPlaying = (EC_COMPLETE != lEventCode);
		if(!m_bPlaying)
		{
			//if(m_nLoopCount == 0)
			//	PlayMusic();
			//else
			if(m_nLoopCount > 0)
			{
				PlayMusic();
			}
			else
			{
				m_bPlayOver	=	true;
			}
		}
	}

	VOID	PhantomMusicOS::Play(INT nLoopCount, float fVolume, float fPan, float fRate)
	{
		m_bPlayOver	=	false;
		if(nLoopCount != 0)
			this->m_nLoopCount	=	nLoopCount;
		//
        if(!g_manager.IsMp3Enabled())
            return;
		m_dwCmd = EBC_DEFAULT;
		//
		PlayMusic();
		SetVolume(fVolume);
//		SetPan(fPan);
		SetPlaySpeed(fRate);
	}

	bool	PhantomMusicOS::PlayMusic()
	{
		this->m_nLoopCount--;
		if(m_bPlaying)
			return true;
		m_bUserPlay	=	true;
		m_bPlaying = TRUE;
		if(!this->m_bIsLoadOK || !m_pSeek)
			return true;
		//
		LONGLONG pos = 0;
		m_pSeek->SetPositions(&pos, AM_SEEKING_AbsolutePositioning|AM_SEEKING_NoFlush, NULL, AM_SEEKING_NoPositioning);
		m_pSeek->SetRate(m_fPlayRate);
		//
		m_pMediaControl->Run();
		return TRUE;
	}

	void PhantomMusicOS::SetPlaySpeed(float fSpeed)
	{
		m_fPlayRate = Phantom::clamp(fSpeed, 0.1f, 10.0f);
		if(!this->m_bIsLoadOK || !m_pSeek)
			return;
		m_pSeek->SetRate(m_fPlayRate);
	}

	void PhantomMusicOS::Stop()
	{
		m_bPlaying	=	false;
		m_bUserPlay	=	false;
		//
		if(!this->m_bIsLoadOK)
			return;
		if ((m_pMediaControl) && (m_pSeek))
		{
			m_dwCmd = EBC_STOP;
			m_pMediaControl->Pause();
			LONGLONG pos = 0;
			m_pSeek->SetPositions(&pos, AM_SEEKING_AbsolutePositioning|AM_SEEKING_NoFlush, 
				NULL, AM_SEEKING_NoPositioning);
		}
	}

	void PhantomMusicOS::SetVolume(float fVolume)
	{
		this->m_fVolume = fVolume;
		float fNewVolume = fVolume;
		//if(fNewVolume > 0.01f)fNewVolume	=	sqrtf(fNewVolume);
		if(!this->m_bIsLoadOK)
			return;
		if (m_pAudio)
			m_pAudio->put_Volume((long)(fNewVolume*10000.0f - 10000.0f));
	}

	float	PhantomMusicOS::GetVolume()
	{
		return m_fVolume;
	}

	void PhantomMusicOS::SetPan( float fPan)
	{
		m_fPan = fPan;
		if(!this->m_bIsLoadOK)
			return;
		if(m_pAudio)
			m_pAudio->put_Balance( (long)(m_fPan*10000.0f));
	}

	float	PhantomMusicOS::GetPan()
	{
		return m_fPan;
	}

	BOOL	PhantomMusicOS::Open(const char* szFile)
	{
		m_fileName	=	szFile;
		if(!CreateMedia())
			return false;
		m_bIsLoadOK	=	true;
		return true;
	}
	MultiMusic*	CreateMusicByFile(const char* szFile)
	{
		LFileName f = szFile;
		f.setIsFile();
		LFileName ext;
		f.splitpath(0, 0, &ext);
		if(ext == ".wav")
		{
			return GetSoundManager()->LoadWaveFile(szFile);
		}
		MultiMusic* ret	=	new MultiMusic();
		ret->Open(szFile);
		return ret;
	}
	MusicControl* CreateMusicPtr(const char* szFile)
	{
		PhantomMusicOS* m = new PhantomMusicOS();
		if(!m->Open(szFile))
			return 0;
		return m;
	}
	CSoundManager		g_soundManager;
	CSoundManager*		GetSoundManager()
	{
		return &g_soundManager;
	}
#endif

};

#ifdef WIN32
	VOID	InitSoundManager(HWND hWnd)
	{
		Phantom::g_soundManager.Initialize(hWnd);
	}

#endif
#endif
