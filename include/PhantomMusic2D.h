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
#ifndef ___PHANTOM_MUSIC2D__H___
#define ___PHANTOM_MUSIC2D__H___
#include "PhantomBase.h"

namespace Phantom{
	class	MultiMusic;

	class	MusicControl
	{
	public:
		//
		virtual	VOID					Play(INT nLoopCount = -1, float fVolume = 1.0f, float fPan = 0.0f, float fRate = 1.0f)	=	0;
		virtual	VOID					Stop()	=	0;
		virtual	BOOL					IsPause() = 0;
		virtual	VOID					Pause()	=	0;
		virtual	VOID					Resume()	=	0;
		virtual	BOOL					IsPlaying()	=	0;
		virtual	VOID					SetVolume(float fVolume)	=	0;
		virtual	VOID					SetPan(float fPan)	=	0;
		virtual	VOID					SetPlaySpeed(float f){}//设置加速播放
        virtual VOID                    FrameMove(){}
	protected:
		int								m_nID;
		friend class	MultiMusic;
		MusicControl(){}
		virtual ~MusicControl(){}
	};
	//
	class	MultiMusic
	{
	public:
		MultiMusic();
		virtual ~MultiMusic();
		virtual	BOOL					Open(const char* szFile);
		virtual	PtrID					Play(INT nLoopCount = -1, float fVolume = 1.0f, float fPan = 0.0f, float fRate = 1.0f);
		virtual	VOID					Pause(PtrID& pID);
		virtual	VOID					Resume(PtrID& pID);
		virtual	VOID					Stop(PtrID& pID);
		virtual	VOID					DestroyAll();
		virtual	VOID					SetVolumePan(PtrID& pID, float fVolume, float fPan);
		virtual	VOID					SetPlaySpeed(PtrID& pID, float f);
		virtual	BOOL					IsPlaying(PtrID& pIn);		//播放文件
		virtual	VOID					StopAll();	//全部停止并删除
		virtual	VOID					PauseAll();	//全部停止并删除
		virtual	VOID					ResumeAll();	//全部停止并删除
        virtual VOID                    FrameMove();
		virtual	VOID					Setup(INT nDup){}
		//
		FileName						m_fileName;
	protected:
		MusicControl*			GetMusicPtr(PtrID& id);
	protected:
		static const int	MAX_MUSIC_DUP	=		1;
		MusicControl*			m_musicPtr[MAX_MUSIC_DUP];
		int								m_nPlayID;
		BOOL							m_bCreateFail;
	};

	class Music2D
	{
	public:
		BOOL					Load(const char* szFile);
		VOID					Destroy();
		//
		VOID					Play(INT nLoopCount = -1, float fVolume = 1.0f, float fPan = 0.0f, float fRate = 1.0f);
		VOID					SetPlaySpeed(float f);
		VOID					Stop();
		VOID					Pause();
		VOID					Resume();
		VOID					SetVolume(float fVolume);
		VOID					SetMute();
		VOID					SetPan(float fPan);
		float					GetVolume();
		float					GetPan();
		float					GetRate(){return m_fPlaySpeed;}
		BOOL					IsPlaying();
		BOOL					IsPaused(){return m_bIsPaused;}
		BOOL					IsLoaded(){return (m_mediaPtr != 0);}
		MultiMusic*		GetMediaPtr(){return m_mediaPtr;}
		Music2D();
		virtual	~Music2D();
	protected:
		MultiMusic*		m_mediaPtr;
		PtrID					m_playID;
		BOOL					m_bIsPaused;
		float					m_fPlaySpeed;
		Name					m_name;
		float					m_fVolume;
		float					m_fPan;
		int						m_flags;
	};

};

#endif
