//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

设计编写�?:   赵德�?英文:Zhao De Xian)
Email: yuzhou_995@hotmail.com
版权所�?2006 赵德�?  保留所有权�?


-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <Windows.h>
#include <dshow.h>
#include "PhantomWin32.h"
#endif

#include "PhantomManager.h"
#include "PhantomOS.h"

namespace	Phantom{

#ifdef IPHONE_OS
	extern "C"{
		unsigned int	OpenWave(const char* szDir, const char* szFileName, const char* szExt, int maxDup);
		void			CloseWave(unsigned int w);
		void			PlayWave(unsigned int w, float fVolume, float fPan, float fRate);
		void			StopWave(unsigned int w);
		void			SetupWave(unsigned int w, int dupCount);
		//
		unsigned int	OpenMp3(const char* szDir, const char* szFileName, const char* szExt);
		void			CloseMp3(unsigned int w);
		void			PlayMp3(unsigned int w, float fVolume, float fPan, float fRate, int loopcount);
		void			StopMp3(unsigned int w);
		void			PauseMp3(unsigned int w);
		void			ResumeMp3(unsigned int w);
		void			SetVolumeMp3(unsigned int w, float fVolume);
		void			SetPanMp3(unsigned int w, float fPan);
        int             IsPlayingMp3(unsigned int w);
	};
	class	PhantomMusicOS: public MusicControl
	{
	public:
		PhantomMusicOS();
		~PhantomMusicOS();
		virtual	VOID					Play(INT nLoopCount, float fVolume, float fPan, float fRate);
		virtual	VOID					Stop();
		virtual	VOID					Pause();
		virtual	BOOL					IsPause();
		virtual	VOID					Resume();
		virtual	BOOL					IsPlaying();
		virtual	VOID					SetVolume(float fVolume);
		virtual	VOID					SetPan(float fPan);
		virtual	BOOL					Open(const char* szFile);
        virtual VOID                    FrameMove();
	public:
        LFileName                       m_mp3File;
		unsigned int					m_resID;
		BOOL							m_bIsPlaying;
		BOOL							m_bIsPause;
		float							m_fVolume;
		float							m_fPan;
		float							m_fRate;
        int                             m_nLoopCount;
	};
	#define	MAX_WAVE_BUFFER	1
	class	PhantomIOSWaves: public MultiMusic
	{
	public:
		PhantomIOSWaves();
		~PhantomIOSWaves();
		virtual	BOOL					Open(const char* szFile);
		virtual	PtrID					Play(INT nLoopCount = -1, float fVolume = 1.0f, float fPan = 0.0f, float fRate = 1.0f);
		virtual	VOID					Pause(PtrID& pID);
		virtual	VOID					Resume(PtrID& pID);
		virtual	VOID					Stop(PtrID& pID);
		virtual	VOID					SetVolumePan(PtrID& pID, float fVolume, float fPan);
		virtual	VOID					SetPlaySpeed(PtrID& pID, float f);
		//
        int m_nMaxDup;
		virtual	VOID					Setup(INT nDup){
			if(nDup<3)
				nDup=3;
			else if(nDup>20)
				nDup = 20;
			if(m_res[0].soundID>0)
				SetupWave(m_res[0].soundID, nDup);
            m_nMaxDup = nDup;
		}
		virtual	BOOL					IsPlaying(PtrID& pIn){return true;}
		virtual	VOID					StopAll(){}
		virtual	VOID					PauseAll(){}
		virtual	VOID					ResumeAll(){}
        VOID							DestroyAll();
	public:
		FileName						m_fileName;
		struct WaveFileID
		{
			unsigned int				soundID;
			unsigned int				playID;
			bool						bFail;
			double						dLastTime;	//如果小于0.5秒内的都不会自动暂停
			WaveFileID(){soundID = 0; playID = 1; bFail = 0;dLastTime = 0;}
		};
		WaveFileID*						FindID(PtrID& id);
		//
		WaveFileID						m_res[MAX_WAVE_BUFFER];
	};
    VOID							PhantomIOSWaves::DestroyAll()
    {
		for(int i=0;i<MAX_WAVE_BUFFER;i++)
		{
			if(m_res[i].soundID)
				CloseWave(m_res[i].soundID);
            m_res[i].soundID = 0;
            m_res[i].bFail = false;
		}
    }
	PhantomIOSWaves::PhantomIOSWaves()
	{
        m_nMaxDup = 10;
	}
	PhantomIOSWaves::~PhantomIOSWaves()
	{
		for(int i=0;i<MAX_WAVE_BUFFER;i++)
		{
			if(m_res[i].soundID)
				CloseWave(m_res[i].soundID);
		}
	}
	BOOL					PhantomIOSWaves::Open(const char* szFile)
	{
		m_fileName	=	szFile;
		for(int i=0;i<MAX_WAVE_BUFFER;i++)
		{
			if(m_res[i].soundID)
				CloseWave(m_res[i].soundID);
			m_res[i].soundID	=	0;
			m_res[i].bFail		=	false;
		}
		return true;
	}
	PtrID					PhantomIOSWaves::Play(INT nLoopCount, float fVolume, float fPan, float fRate)
	{
        if(!g_manager.IsWavEnabled())
            return PtrID(-1, -1);
		double dTime = g_manager.GetAppTime();
		for(int i=0;i<MAX_WAVE_BUFFER;i++)
		{
			if(m_res[i].soundID > 0)
			{
				//double d = dTime - m_res[i].dLastTime;
				//if(d > 0.1f)
				{
					m_res[i].playID = this->m_nPlayID++;
					m_res[i].dLastTime	=	dTime;
                    PlayWave(m_res[i].soundID, fVolume, fPan, fRate);
					return PtrID(i, m_res[i].playID);
				}
			}
		}
		for(int i=0;i<MAX_WAVE_BUFFER;i++)
		{
			if(m_res[i].soundID == 0 && !m_res[i].bFail)
			{
                LFileName existFile;
                if(!fexist(m_fileName.t, &existFile))
                {
                    m_res[i].bFail	=	true;
                    return PtrID(0, 0);
                }
                LFileName file, ext, dir;
				existFile.splitpath(&dir, &file, &ext);
				m_res[i].soundID	=	OpenWave(dir.t, file.t, ext.t, m_nMaxDup);
				if(m_res[i].soundID == 0)
				{
					m_res[i].bFail	=	true;
					return PtrID(0, 0);
				}
				m_res[i].dLastTime	=	dTime;
				m_res[i].playID		=	this->m_nPlayID++;
                PlayWave(m_res[i].soundID, fVolume, fPan, fRate);
				return PtrID(i, m_res[i].playID);
			}
		}
		return PtrID(0, 0);
	}
	PhantomIOSWaves::WaveFileID*				PhantomIOSWaves::FindID(PtrID& id)
	{
		if(id.index < 0 || id.index >= MAX_WAVE_BUFFER)
		{
			id.id = 0;
			return 0;
		}
		if(m_res[id.index].playID == id.id)
			return &m_res[id.index];
		for(int i=0;i<MAX_WAVE_BUFFER;i++)
		{
			if(this->m_res[i].playID == id.id)
			{
				id.index	=	i;
				return &m_res[i];
			}
		}
		id.id	=	0;
		return 0;
	}
	VOID					PhantomIOSWaves::Pause(PtrID& pID)
	{
	}
	VOID					PhantomIOSWaves::Resume(PtrID& pID)
	{
	}
	VOID					PhantomIOSWaves::Stop(PtrID& pID)
	{
		WaveFileID*	w = FindID(pID);
		if(!w)return;
		StopWave(w->soundID);
	}
	VOID					PhantomIOSWaves::SetVolumePan(PtrID& pID, float fVolume, float fPan)
	{
	}
	VOID					PhantomIOSWaves::SetPlaySpeed(PtrID& pID, float f)
	{
		WaveFileID*	w = FindID(pID);
		if(!w)return;
	}

	PhantomMusicOS::PhantomMusicOS()
	{
        m_nLoopCount = 0;
		m_fRate		=	1.0f;
		m_fVolume	=	1.0f;
		m_fPan		=	0;
		m_bIsPlaying	=	false;
		m_bIsPause	=	false;
		m_resID		=	-1;
	}
	PhantomMusicOS::~PhantomMusicOS()
	{
		if(m_resID)
			CloseMp3(m_resID);
	}
	VOID		PhantomMusicOS::Play(INT nLoopCount, float fVolume, float fPan, float fRate)
	{
		if(m_resID  && g_manager.IsMp3Enabled())
		{
            m_nLoopCount = nLoopCount;
            StopMp3(m_resID);
			PlayMp3(m_resID, m_fVolume = fVolume, m_fPan = fPan, m_fRate = fRate, m_nLoopCount);
		}
		m_bIsPlaying	=	true;
	}
	VOID		PhantomMusicOS::Stop()
	{
		if(m_resID)
			StopMp3(m_resID);
        m_bIsPlaying = false;
	}
	VOID		PhantomMusicOS::Pause()
	{
		if(!m_bIsPause)
		{
			if(m_resID)
				PauseMp3(m_resID);
			m_bIsPause	=	true;
		}
	}
	BOOL		PhantomMusicOS::IsPause()
	{
		return m_bIsPause;
	}
    VOID        PhantomMusicOS::FrameMove()
    {
        if(m_nLoopCount == 0)
            return;
        if(this->m_bIsPlaying && m_resID && !IsPlayingMp3(m_resID))
        {
            //m_nLoopCount--;
            //Stop();
            //if(m_nLoopCount>0)
            //{
               // PlayMp3(m_resID, m_fVolume, m_fPan, m_fRate);
            //}
        }
    }
	VOID		PhantomMusicOS::Resume()
	{
		if(m_bIsPause)
		{
			if(m_resID)
				ResumeMp3(m_resID);
			m_bIsPause	=	false;
		}
	}
	BOOL		PhantomMusicOS::IsPlaying()
	{
		return m_bIsPlaying;
        //if(m_resID)
          //  return IsPlayingMp3(m_resID);//this->m_bIsPlaying;
        //return false;
	}
	VOID		PhantomMusicOS::SetVolume(float fVolume)
	{
        if(m_resID)
            SetVolumeMp3(m_resID, fVolume);
	}
	VOID		PhantomMusicOS::SetPan(float fPan)
	{
        if(m_resID)
            SetPanMp3(m_resID, fPan);
	}
	BOOL		PhantomMusicOS::Open(const char* szFile)
	{
		if(m_resID)
			CloseMp3(m_resID);
        
        LFileName existFile;
        if(!fexist(szFile, &existFile))
        {
            return false;
        }
        m_mp3File = szFile;
        LFileName file, ext, dir;
        existFile.splitpath(&dir, &file, &ext);
		m_resID	=	OpenMp3(dir.t, file.t, ext.t);
		return (m_resID > 0);
	}
    
	MultiMusic*	CreateMusicByFile(const char* szFile)
	{
		LFileName f = szFile;
		f.setIsFile();
		LFileName ext;
		f.splitpath(0, 0, &ext);
		if(ext == ".wav")
		{
			PhantomIOSWaves* ret	=	new PhantomIOSWaves();
			ret->Open(szFile);
			return ret;
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
#endif
};
