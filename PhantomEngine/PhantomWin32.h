//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   PhantomWin32.h
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___PHANTOMWIN32__H___
#define ___PHANTOMWIN32__H___
#include <Windows.h>
#include "PhantomOS.h"
#include "dsound.h"
#ifdef WIN32

#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2

namespace	Phantom{
	class CWaveFile: public Base
	{
	public:
		ImplementComValue();
		CWaveFile();
		~CWaveFile();
		BOOL			OpenWaveFile( const char* strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
		BOOL			Close();

		BOOL			ReadBuffer( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
		BOOL			WriteBuffer( UINT nSizeToWrite, BYTE* pbData, UINT* pnSizeWrote );

		DWORD   		GetSize();
		BOOL	 		ResetFile();
		WAVEFORMATEX*	GetFormat() { return m_wavefmtEx; };
		//
		BOOL	 		ReadMMIO();
		BOOL			WriteMMIO( WAVEFORMATEX *pwfxDest );
	public:
		WAVEFORMATEX* 	m_wavefmtEx;
		HMMIO         	m_hmmio;
		MMCKINFO      	m_ckInfo;
		MMCKINFO      	m_ckRiff;
		DWORD         	m_dwSize;
		MMIOINFO      	m_mmioinfoOut;
		DWORD         	m_dwFlags;
		CHAR*         	m_pResourceBuffer;
	};


	class CSoundManager;
	class CSound;


#define	MAX_SOUND_BUFFER		10
	class CSoundManager: public MultiMusic
	{
	public:
		CSoundManager();
		~CSoundManager();

		void						SetSoundBufferCount(DWORD dwCount){m_nMaxSoundBuffer = min(dwCount, MAX_SOUND_BUFFER);}
		//
		CSound*						LoadWaveFile(const char* szFile);
		bool						Play(int fileIndex, PtrID* pOut = 0);
		bool						PlayLoop(int fileIndex, PtrID* pOut = 0, int nLoopCount = 1);
		bool						Stop(int fileIndex, PtrID* pID);
		void						SetPlaySpeed(int fileIndex, PtrID* pID, float fSpeed);
		bool						SetVolume(int fileIndex, PtrID* pID, float f);
		bool						SetPan(int fileIndex, PtrID* pID, float f);
		bool						IsPlayOver(int fileIndex, PtrID* pIn);		//播放文件
		bool						GetVolume(int fileIndex, PtrID* pID, float& f);	//设置声音大小，返回false表示此声音已经过期无效
		bool						GetPan(int fileIndex, PtrID* pID, float& f);		//设置声音声道，返回false表示此声音已经过期无效

		void						SetListener(const float3& vPosition, const float3& vVelocity);
		void						SetListenerParams(float fDopplerFactor, float fRolloffFactor);
		void						GetListenerParams(float3& vPosition, float3& vVelocity, float& fDopplerFactor, float& fRolloffFactor);
		void*						GetSound8_Interface(){return m_pDS;}
		//
		VOID						SetWavePath(const char* szPath);
		const char*					GetWavePath(){return m_wavePath;}

		DS3DLISTENER				m_dsListenerParams;

		HRESULT 					Initialize( HWND hWnd, DWORD dwCoopLevel = DSSCL_PRIORITY );
		inline  					LPDIRECTSOUND8 GetDirectSound() { return m_pDS; }
		HRESULT 					SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
		HRESULT 					Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener );
		
		HRESULT 					Create( CSound** ppSound, const char* strWaveFileName, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1);
		//
		VOID						Destroy();
	protected:
		FileName					m_wavePath;
		IDirectSound8*				m_pDS;
		float						m_fRefreshTime;
		DWORD						m_nMaxSoundBuffer;
		LPDIRECTSOUND3DLISTENER		m_pListener;
		//
		std::vector<CSound*>		m_playMap;
	};

	class CSound: public MultiMusic
	{
	public:
		CSound( CSoundManager* pManager, LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, CWaveFile* pWaveFile, DWORD dwCreationFlags );
		virtual ~CSound();
		virtual	PtrID					Play(INT nLoopCount = -1, float fVolume = 1.0f, float fPan = 0.0f, float fRate = 1.0f);
		virtual	VOID					Pause(PtrID& pID){}
		virtual	VOID					Resume(PtrID& pID){}
		virtual	VOID					Stop(PtrID& pID);
		virtual	VOID					SetVolumePan(PtrID& pID, float fVolume, float fPan);
		virtual	VOID					SetPlaySpeed(PtrID& pID, float f);
		//
		virtual	BOOL					IsPlaying(PtrID& pIn);
		virtual	VOID					StopAll(){}
		virtual	VOID					PauseAll(){}
		virtual	VOID					ResumeAll(){}
	public:
		HRESULT 				Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer );
		HRESULT 				FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger );
		LPDIRECTSOUNDBUFFER 	GetFreeBuffer(LPDIRECTSOUND3DBUFFER* p3DSndRet = 0);
		//
		DWORD					GetFreeBufferIndex();

		BOOL					IsPlaying(PtrID* pID);
		BOOL					SetSoundParam(PtrID* pID, DS3DBUFFER* pBuffer);
		//
		LPDIRECTSOUNDBUFFER 	GetBuffer( DWORD dwIndex );

		HRESULT 				Play( DWORD dwPriority = 0, DWORD dwFlags = 0, LONG lVolume = 0, LONG lFrequency = -1, LONG lPan = 0 );
		HRESULT 				Stop();
		HRESULT 				Reset();
		BOOL    				IsSoundPlaying();
		virtual	VOID				Setup(INT nDup);

		BOOL					Play3D( PtrID* pID, int nLoopCount, DS3DBUFFER* pBuffer );

		FileName				m_filename;//char					m_szSoundFile[255];
		CSoundManager*			m_pManager;

		VOID					Destroy();
		void					SetVolume(PtrID* pID, float f, int nSndType);
		void					SetPan(PtrID* pID, float f);
		BOOL					GetVolume(PtrID* pID, float& f);
		BOOL					GetPan(PtrID* pID, float& f);
		void					SetPercentage(PtrID* pID, float f);
	protected:
		LPDIRECTSOUNDBUFFER* 	m_apDSBuffer;
		LPDIRECTSOUND3DBUFFER* 	m_apDS3DBuffer;
		INT						m_playRID[MAX_SOUND_BUFFER];
		INT						m_nMaxSoundBuffer;
		//
		float					m_fVolume;
		float					m_fPan;
		DWORD                	m_dwDSBufferSize;
		CWaveFile*				m_pWaveFile;
		DWORD                	m_dwNumBuffers;
		DWORD                	m_dwCreationFlags;
		DWORD					m_dwDefaultFreq;

		HRESULT					RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored );

	};
	CSoundManager*		GetSoundManager();

};

#endif

#endif
