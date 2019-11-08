#include "PhantomMusic2D.h"
#include "PhantomManager.h"
#include "PhantomOS.h"

namespace Phantom{
	MultiMusic::MultiMusic()
	{
		memset(m_musicPtr, 0, sizeof(m_musicPtr));
		m_nPlayID	=	1;
		m_bCreateFail	=	false;
	}
	MultiMusic::~MultiMusic()
	{
		for(int i=0;i<MAX_MUSIC_DUP;i++)
			safe_delete(m_musicPtr[i]);
	}
	PtrID							MultiMusic::Play(INT nLoopCount, float fVolume, float fPan, float fRate)
	{
		if(m_bCreateFail || g_manager.GetMute())
			return PtrID(0, 0);
		if(nLoopCount < 0)
			nLoopCount	=	10;
		for(int i=0;i<MAX_MUSIC_DUP;i++)
		{
#ifndef EXPORT_ENGINE
			if(!m_musicPtr[i])
				m_musicPtr[i]	=	CreateMusicPtr(m_fileName);
#endif
			if(m_musicPtr[i])
			{
				//if(!m_musicPtr[i]->IsPlaying())
				{
					m_musicPtr[i]->Stop();
					m_musicPtr[i]->Play(nLoopCount, fVolume, fPan, fRate);
					return PtrID(i, m_musicPtr[i]->m_nID = (m_nPlayID++));
				}
			}
		}
		return PtrID(0, 0);
	}
    VOID                            MultiMusic::FrameMove()
    {
        for(int i=0;i<MAX_MUSIC_DUP;i++)
        {
            if(m_musicPtr[i])
                m_musicPtr[i]->FrameMove();
        }
    }
	VOID							MultiMusic::DestroyAll()
	{
		for(int i=0;i<MAX_MUSIC_DUP;i++)
		{
			safe_delete(m_musicPtr[i]);
		}
	}
	BOOL							MultiMusic::Open(const char* szFile)
	{
		if(m_musicPtr[0])
			return true;
		m_fileName = szFile;
		m_fileName.setIsFile();
		for(int i=0;i<MAX_MUSIC_DUP;i++)
		{
#ifndef EXPORT_ENGINE
			if(!m_musicPtr[i])
				m_musicPtr[i]	=	CreateMusicPtr(m_fileName);
#endif
		}
		return true;
	}
	VOID							MultiMusic::Pause(PtrID& pID)
	{
		MusicControl*	m	=	GetMusicPtr(pID);
		if(m)
			m->Pause();
	}
	VOID							MultiMusic::Resume(PtrID& pID)
	{
		MusicControl*	m	=	GetMusicPtr(pID);
		if(m)
			m->Resume();
	}
	VOID							MultiMusic::SetPlaySpeed(PtrID& pID, float f)
	{
		MusicControl*	m	=	GetMusicPtr(pID);
		if(m)
			m->SetPlaySpeed(f);
	}
	VOID							MultiMusic::Stop(PtrID& pID)
	{
		MusicControl*	m	=	GetMusicPtr(pID);
		if(m)
			m->Stop();
	}
	VOID							MultiMusic::SetVolumePan(PtrID& pID, float fVolume, float fPan)
	{
		MusicControl*	m	=	GetMusicPtr(pID);
		if(m)
		{
			m->SetVolume(fVolume);
			m->SetPan(fPan);
		}
	}
	BOOL							MultiMusic::IsPlaying(PtrID& pID)
	{
		MusicControl*	m	=	GetMusicPtr(pID);
		if(m)
			return m->IsPlaying();
		return false;
	}		//播放文件
	VOID							MultiMusic::StopAll()
	{
		for(int i=0;i<MAX_MUSIC_DUP;i++)
		{
			if(m_musicPtr[i])
			{
				m_musicPtr[i]->Stop();
			}
		}
	}	//全部停止并删除
	VOID							MultiMusic::PauseAll()	//全部停止并删除
	{
		for(int i=0;i<MAX_MUSIC_DUP;i++)
		{
			if(m_musicPtr[i])
			{
				m_musicPtr[i]->Pause();
			}
		}
	}
	VOID							MultiMusic::ResumeAll()	//全部停止并删除
	{
		for(int i=0;i<MAX_MUSIC_DUP;i++)
		{
			if(m_musicPtr[i] && m_musicPtr[i]->IsPause())
			{
				m_musicPtr[i]->Resume();
			}
		}
	}
	MusicControl*			MultiMusic::GetMusicPtr(PtrID& id)
	{
		if(id.id == 0)
			return 0;
		if(id.index < MAX_MUSIC_DUP&&m_musicPtr[id.index]  && id.id == m_musicPtr[id.index]->m_nID)
			return m_musicPtr[id.index];
		for(int i=0;i<MAX_MUSIC_DUP;i++)
		{
			if(!m_musicPtr[i])
				continue;
			if(id.id == m_musicPtr[i]->m_nID)
			{
				id.index	=	i;
				id.id = m_musicPtr[i]->m_nID;
				return m_musicPtr[i];
			}
		}
		id.id	=	0;
		return 0;
	}

	Music2D::Music2D()
	{
		m_fPlaySpeed		=	1.0f;
		m_bIsPaused			=	false;
		m_fVolume			=	1.0f;
		m_fPan				=	0.0f;
		m_flags				=	0;
		m_mediaPtr			=	0;
		g_manager.PushMusic(this);
	}
	Music2D::~Music2D()
	{
		g_manager.PopMusic(this);
		Destroy();
	}
	VOID	Music2D::Destroy()
	{
		if(m_mediaPtr)
			m_mediaPtr->Stop(m_playID);
		m_mediaPtr			=	0;
		m_bIsPaused			=	false;
	}

	BOOL	Music2D::Load(const char* szFile)
	{
		Destroy();
		m_mediaPtr		=	g_manager.GetMusicFilePtr(szFile);
		return true;
	}

	BOOL	Music2D::IsPlaying()
	{
		if(m_mediaPtr)
			return m_mediaPtr->IsPlaying(m_playID);
		return false;
	}

	VOID	Music2D::Play(INT nLoopCount, float fVolume, float fPan, float fRate)
	{
		if(m_mediaPtr)
		{
			m_fVolume = clamp(fVolume, 0, 1);
			m_mediaPtr->Stop(m_playID);
			m_playID	=	m_mediaPtr->Play(nLoopCount, g_manager.GetMute() ? 0 : m_fVolume, m_fPan = fPan, m_fPlaySpeed = fRate);
		}
	}

	VOID	Music2D::SetPlaySpeed(float f)
	{
	}

	VOID	Music2D::Stop()
	{
		if(m_mediaPtr)
			m_mediaPtr->Stop(m_playID);
	}
	VOID	Music2D::Pause()
	{
		this->m_bIsPaused	=	true;
		if(m_mediaPtr)
			m_mediaPtr->Pause(m_playID);
	}
	VOID	Music2D::Resume()
	{
		this->m_bIsPaused	=	false;
		if(m_mediaPtr)
			m_mediaPtr->Resume(m_playID);
	}

	float	Music2D::GetVolume()
	{
		return m_fVolume;
	}

	float	Music2D::GetPan()
	{
		return m_fPan;
	}

	VOID	Music2D::SetVolume(float fVolume)
	{
		m_fVolume = clamp(fVolume, 0, 1);
		if(m_mediaPtr)
			m_mediaPtr->SetVolumePan(m_playID, g_manager.GetMute() ? 0 : m_fVolume, m_fPan);
	}
	VOID	Music2D::SetMute()
	{
		if(m_mediaPtr)
			m_mediaPtr->SetVolumePan(m_playID, 0, m_fPan);
	}
	VOID	Music2D::SetPan(float fPan)
	{
		if(m_mediaPtr)
			m_mediaPtr->SetVolumePan(m_playID, g_manager.GetMute() ? 0 : m_fVolume, m_fPan = clamp(fPan, -1, 1));
	}

};
