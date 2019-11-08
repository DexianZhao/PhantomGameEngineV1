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
#include <atlimage.h>
#include <tchar.h>
#include <dshow.h>
#include "PhantomWin32.h"
#include "PhantomManager.h"
#ifndef EXPORT_ENGINE
namespace	Phantom{

	CWaveFile::CWaveFile()
	{
		m_wavefmtEx				=	0;
		m_hmmio					=	0;
		m_dwSize				=	0;
		m_dwFlags				=	0;
		m_pResourceBuffer		=	0;
	}

	CWaveFile::~CWaveFile()
	{
		Close();
		safe_deleteList( m_wavefmtEx );
	}

	BOOL	CWaveFile::OpenWaveFile( const char* strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags )
	{
		m_dwFlags	=	WAVEFILE_READ;
		StreamBase* stream	=	open_s(strFileName, true);
		if(!stream)
			return false;
		safe_deleteList( m_wavefmtEx );
		DWORD dwSize	=	stream->size();
		m_pResourceBuffer = (char*)malloc(dwSize);
		stream->read(m_pResourceBuffer, dwSize );
		MMIOINFO mmioInfo;
		ZeroMemory( &mmioInfo, sizeof(mmioInfo) );
		mmioInfo.fccIOProc = FOURCC_MEM;
		mmioInfo.cchBuffer = dwSize;
		mmioInfo.pchBuffer = (CHAR*) m_pResourceBuffer;
		m_hmmio = mmioOpen( NULL, &mmioInfo, MMIO_ALLOCBUF | MMIO_READ );
		//
		close_s(&stream);
		//
		if(!ReadMMIO())
		{
			mmioClose( m_hmmio, 0 );
			return FALSE;
		}
		if(!ResetFile())
			return FALSE;
		m_dwSize = m_ckInfo.cksize;
		return true;
	}

	BOOL	CWaveFile::ReadMMIO()
	{
		MMCKINFO        ckIn;
		PCMWAVEFORMAT   pcmWaveFormat;
		m_wavefmtEx = NULL;
		if( ( 0 != mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) ) )
			return FALSE;
		if( (m_ckRiff.ckid != FOURCC_RIFF) || (m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
			return FALSE;
		ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if( 0 != mmioDescend( m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
			return FALSE;
		if( ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT) )
			return FALSE;
		if( mmioRead( m_hmmio, (HPSTR) &pcmWaveFormat, sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
			return FALSE;
		if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
		{
			m_wavefmtEx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
			if( NULL == m_wavefmtEx )
				return FALSE;
			memcpy( m_wavefmtEx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
			m_wavefmtEx->cbSize = 0;
		}
		else
		{
			WORD cbExtraBytes = 0L;
			if( mmioRead( m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD) )
				return FALSE;
			m_wavefmtEx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) + cbExtraBytes ];
			if( NULL == m_wavefmtEx )
				return FALSE;
			memcpy( m_wavefmtEx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
			m_wavefmtEx->cbSize = cbExtraBytes;
			if( mmioRead( m_hmmio, (CHAR*)(((BYTE*)&(m_wavefmtEx->cbSize))+sizeof(WORD)), cbExtraBytes ) != cbExtraBytes )
			{
				safe_delete( m_wavefmtEx );
				return FALSE;
			}
		}
		if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
		{
			safe_delete( m_wavefmtEx );
			return FALSE;
		}
		return TRUE;
	}

	DWORD	CWaveFile::GetSize()
	{
		return m_dwSize;
	}

	BOOL	CWaveFile::ResetFile()
	{
		if( m_hmmio == NULL )
			return FALSE;

		if( m_dwFlags == WAVEFILE_READ )
		{
			if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC), SEEK_SET ) )
				return FALSE;
			m_ckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
			if( 0 != mmioDescend( m_hmmio, &m_ckInfo, &m_ckRiff, MMIO_FINDCHUNK ) )
				return FALSE;
		}
		else
		{
			m_ckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
			m_ckInfo.cksize = 0;
			if( 0 != mmioCreateChunk( m_hmmio, &m_ckInfo, 0 ) )
				return FALSE;
			if( 0 != mmioGetInfo( m_hmmio, &m_mmioinfoOut, 0 ) )
				return FALSE;
		}
		return TRUE;
	}

	BOOL	CWaveFile::ReadBuffer( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
	{
		MMIOINFO mmioinfoIn;
		if( m_hmmio == NULL )
			return FALSE;
		if( pBuffer == NULL || pdwSizeRead == NULL )
			return FALSE;
		if( pdwSizeRead != NULL )
			*pdwSizeRead = 0;
		if( mmioGetInfo( m_hmmio, &mmioinfoIn, 0 ) != 0 )
			return FALSE;
		UINT cbDataIn = dwSizeToRead;
		if( cbDataIn > m_ckInfo.cksize )
			cbDataIn = m_ckInfo.cksize;
		m_ckInfo.cksize -= cbDataIn;
		for( DWORD cT = 0; cT < cbDataIn; cT++ )
		{
			if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
			{
				if( mmioAdvance( m_hmmio, &mmioinfoIn, MMIO_READ ) != 0 )
					return FALSE;
				if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
					return FALSE;
			}
			*((BYTE*)pBuffer+cT) = *((BYTE*)mmioinfoIn.pchNext);
			mmioinfoIn.pchNext++;
		}
		if( mmioSetInfo( m_hmmio, &mmioinfoIn, 0 ) != 0 )
			return FALSE;
		if( pdwSizeRead != NULL )
			*pdwSizeRead = cbDataIn;
		return TRUE;
	}

	BOOL	CWaveFile::Close()
	{
		if( m_dwFlags == WAVEFILE_READ )
		{
			mmioClose( m_hmmio, 0 );
			m_hmmio = NULL;
			if(m_pResourceBuffer)
				free( m_pResourceBuffer );
			m_pResourceBuffer	=	0;
		}
		else
		{
			m_mmioinfoOut.dwFlags |= MMIO_DIRTY;
			if( m_hmmio == NULL )
				return FALSE;
			if( mmioSetInfo( m_hmmio, &m_mmioinfoOut, 0 ) != 0 )
				return FALSE;
			if( mmioAscend( m_hmmio, &m_ckInfo, 0 ) != 0 )
				return FALSE;
			if( mmioAscend( m_hmmio, &m_ckRiff, 0 ) != 0 )
				return FALSE;

			mmioSeek( m_hmmio, 0, SEEK_SET );

			if( (INT)mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) != 0 )
				return FALSE;
			m_ckInfo.ckid = mmioFOURCC('f', 'a', 'c', 't');
			if( mmioDescend( m_hmmio, &m_ckInfo, &m_ckRiff, MMIO_FINDCHUNK ) == 0 )
			{
				DWORD dwSamples = 0;
				mmioWrite( m_hmmio, (HPSTR)&dwSamples, sizeof(DWORD) );
				mmioAscend( m_hmmio, &m_ckInfo, 0 );
			}
			if( mmioAscend( m_hmmio, &m_ckRiff, 0 ) != 0 )
				return FALSE;
			mmioClose( m_hmmio, 0 );
			m_hmmio = NULL;
		}
		return TRUE;
	}

	BOOL	CWaveFile::WriteMMIO( WAVEFORMATEX *pwfxDest )
	{
		DWORD    dwFactChunk;
		MMCKINFO ckOut1;
		dwFactChunk = (DWORD)-1;
		m_ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		m_ckRiff.cksize = 0;
		if( 0 != mmioCreateChunk( m_hmmio, &m_ckRiff, MMIO_CREATERIFF ) )
			return FALSE;
		m_ckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		m_ckInfo.cksize = sizeof(PCMWAVEFORMAT);
		if( 0 != mmioCreateChunk( m_hmmio, &m_ckInfo, 0 ) )
			return FALSE;
		if( pwfxDest->wFormatTag == WAVE_FORMAT_PCM )
		{
			if( mmioWrite( m_hmmio, (HPSTR) pwfxDest, sizeof(PCMWAVEFORMAT)) != sizeof(PCMWAVEFORMAT))
				return FALSE;
		}
		else
		{
			if( (UINT)mmioWrite( m_hmmio, (HPSTR) pwfxDest, sizeof(*pwfxDest) + pwfxDest->cbSize ) != ( sizeof(*pwfxDest) + pwfxDest->cbSize ) )
				return FALSE;
		}
		if( 0 != mmioAscend( m_hmmio, &m_ckInfo, 0 ) )
			return FALSE;
		ckOut1.ckid = mmioFOURCC('f', 'a', 'c', 't');
		ckOut1.cksize = 0;
		if( 0 != mmioCreateChunk( m_hmmio, &ckOut1, 0 ) )
			return FALSE;
		if( mmioWrite( m_hmmio, (HPSTR)&dwFactChunk, sizeof(dwFactChunk)) != sizeof(dwFactChunk) )
			return FALSE;
		if( 0 != mmioAscend( m_hmmio, &ckOut1, 0 ) )
			return FALSE;

		return TRUE;
	}

	BOOL	CWaveFile::WriteBuffer( UINT nSizeToWrite, BYTE* pbSrcData, UINT* pnSizeWrote )
	{
		UINT cT;
		if( this->m_pResourceBuffer )
			return FALSE;
		if( m_hmmio == NULL )
			return FALSE;
		if( pnSizeWrote == NULL || pbSrcData == NULL )
			return FALSE;
		*pnSizeWrote = 0;
		for( cT = 0; cT < nSizeToWrite; cT++ )
		{
			if( m_mmioinfoOut.pchNext == m_mmioinfoOut.pchEndWrite )
			{
				m_mmioinfoOut.dwFlags |= MMIO_DIRTY;
				if( 0 != mmioAdvance( m_hmmio, &m_mmioinfoOut, MMIO_WRITE ) )
					return FALSE;
			}
			*((BYTE*)m_mmioinfoOut.pchNext) = *((BYTE*)pbSrcData+cT);
			(BYTE*)m_mmioinfoOut.pchNext++;
			(*pnSizeWrote)++;
		}
		return TRUE;
	}

	VOID						CSoundManager::SetWavePath(const char* szPath)
	{
		if(szPath)
		{
			char szDrive[256], szDir[256];
			_splitpath(szPath, szDrive, szDir, 0, 0);
			strcat(szDrive, szDir);
			m_wavePath	=	szDrive;
		}
		else
			m_wavePath	=	"";
	}

	CSound*						CSoundManager::LoadWaveFile(const char* szFileName)
	{
		FileName filename;
		filename	=	szFileName;
		filename.setIsFile();
		CSound* pData	=	0;
		for(size_t i=0;i<m_playMap.size();i++)
		{
			if(m_playMap.at(i)->m_filename.isFile(filename))
				return m_playMap.at(i);
		}
		DWORD	dwFlags	=	0;
		GUID	guid	=	GUID_NULL;
		dwFlags		= DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN|DSBCAPS_CTRLFREQUENCY;
		guid		= GUID_NULL;
		CSound* pSound	=	0;
		if(FAILED(Create(&pSound, filename, dwFlags, guid, m_nMaxSoundBuffer)))
			return 0;
		pSound->m_filename	=	filename;
		m_playMap.push_back( pSound );
		return pSound;//(int)(m_playMap.size() - 1);
	}
	VOID						CSound::Setup(INT nDup){
		if(nDup<3)
			nDup=3;
		m_nMaxSoundBuffer = min(nDup, 10);
	}
	bool						CSoundManager::Play(int fileIndex, PtrID* pOut)
	{
		if(fileIndex < 0 || fileIndex >= (int)m_playMap.size() || ! g_manager.IsWavEnabled())
			return false;
		PtrID id;
		id.index	=	0;
		id.id		=	0;
		if(!m_playMap.at(fileIndex)->Play3D(&id, 1, NULL))
			return false;
		if(pOut)
			*pOut	=	id;
		return true;
	}

	bool						CSoundManager::PlayLoop(int fileIndex, PtrID* pOut, int nLoopCount)
	{
		if(fileIndex < 0 || fileIndex >= (int)m_playMap.size() || ! g_manager.IsWavEnabled())
			return false;
		PtrID id;
		id.index	=	0;
		id.id		=	0;
		if(!m_playMap.at(fileIndex)->Play3D(&id, nLoopCount, NULL))
			return false;
		if(pOut)
			*pOut	=	id;
		m_playMap.at(fileIndex)->SetVolume(&id, 0, SoundType_SOUND2D);
		m_playMap.at(fileIndex)->SetPan(&id, 0.0f);
		return true;
	}

	bool						CSoundManager::IsPlayOver(int fileIndex, PtrID* pIn)		//播放文件
	{
		if(fileIndex < 0 || fileIndex >= (int)m_playMap.size())
			return false;
		return (m_playMap.at(fileIndex)->IsPlaying(*pIn) != FALSE);
	}
	bool						CSoundManager::GetVolume(int fileIndex, PtrID* pID, float& f)	//设置声音大小，返回false表示此声音已经过期无效
	{
		if(fileIndex < 0 || fileIndex >= (int)m_playMap.size())
			return false;
		return (m_playMap.at(fileIndex)->GetVolume(pID, f) != FALSE);
	}
	bool						CSoundManager::GetPan(int fileIndex, PtrID* pID, float& f)		//设置声音声道，返回false表示此声音已经过期无效
	{
		if(fileIndex < 0 || fileIndex >= (int)m_playMap.size())
			return false;
		return (m_playMap.at(fileIndex)->GetPan(pID, f) != FALSE);
	}

	bool						CSoundManager::Stop(int fileIndex, PtrID* pID)
	{
		if(fileIndex < 0 || fileIndex >= (int)m_playMap.size())
			return false;
		m_playMap.at(fileIndex)->Stop(*pID);
		return true;
	}

	void						CSoundManager::SetPlaySpeed(int fileIndex, PtrID* pID, float fSpeed)
	{
		if(fileIndex < 0 || fileIndex >= (int)m_playMap.size())
			return;
		m_playMap.at(fileIndex)->SetPlaySpeed(*pID, fSpeed);
	}
	bool						CSoundManager::SetVolume(int fileIndex, PtrID* pID, float f)
	{
		if(fileIndex < 0 || fileIndex >= (int)m_playMap.size())
			return false;
		m_playMap.at(fileIndex)->SetVolume(pID, f, SoundType_SOUND2D);
		return true;
	}

	bool						CSoundManager::SetPan(int fileIndex, PtrID* pID, float f)
	{
		if(fileIndex < 0 || fileIndex >= (int)m_playMap.size())
			return false;
		m_playMap.at(fileIndex)->SetPan(pID, f);
		return true;
	}

	CSoundManager::CSoundManager()
	{
		m_nMaxSoundBuffer	=	10;
		m_pListener	=	NULL;
		m_pDS = NULL;
		memset(&m_dsListenerParams, 0, sizeof(m_dsListenerParams));
		m_dsListenerParams.dwSize	=	sizeof(m_dsListenerParams);
		this->m_dsListenerParams.flDopplerFactor	=	1.0f;
		this->m_dsListenerParams.flRolloffFactor	=	1.0f;
		m_fRefreshTime	=	0.1f;
		m_wavePath		=	"sound\\";
	}

	CSoundManager::~CSoundManager()
	{
		//for(int i=0;i<(int)m_playMap.size();i++)
		//{
		//	m_playMap.at(i)->m_pManager		=	0;
		//	//delete m_playMap.at(i);
		//	//safe_release(m_playMap.at(i));
		//}
		//Destroy();
	}

	VOID	CSoundManager::SetListenerParams(float fDopplerFactor, float fRolloffFactor)
	{
	}

	VOID	CSoundManager::GetListenerParams(float3& vPosition, float3& vVelocity, float& fDopplerFactor, float& fRolloffFactor)
	{
		vPosition		=	(float3&)this->m_dsListenerParams.vPosition;
		vVelocity		=	(float3&)this->m_dsListenerParams.vVelocity;
		fDopplerFactor	=	this->m_dsListenerParams.flDopplerFactor;
		fRolloffFactor	=	this->m_dsListenerParams.flRolloffFactor;
	}

	VOID	CSoundManager::SetListener(const float3& vPosition, const float3& vVelocity)
	{
		this->m_dsListenerParams.vPosition	=	(D3DVECTOR&)vPosition;
	}

	VOID	CSoundManager::Destroy()
	{
		safe_release(m_pListener);
		safe_release( m_pDS );
	}

	HRESULT CSoundManager::Initialize( HWND  hWnd,
		DWORD dwCoopLevel )
	{
		if(m_pDS)
			return S_OK;
		HRESULT             hr;

		safe_release(m_pListener);
		safe_release( m_pDS );

		if( FAILED( hr = DirectSoundCreate8( NULL, &m_pDS, NULL ) ) )
			return hr;
		if( FAILED( hr = m_pDS->SetCooperativeLevel( hWnd, dwCoopLevel ) ) )
			return hr;

		this->Get3DListenerInterface(&m_pListener);
		m_pListener->GetAllParameters( &m_dsListenerParams );
		m_dsListenerParams.flRolloffFactor	=	1.0f;
		m_dsListenerParams.vOrientFront.x	=	0;
		m_dsListenerParams.vOrientFront.y	=	-1;
		m_dsListenerParams.vOrientFront.z	=	0;
		m_dsListenerParams.vOrientTop.x		=	0;
		m_dsListenerParams.vOrientTop.y		=	0;
		m_dsListenerParams.vOrientTop.z		=	1;
		return S_OK;
	}

	HRESULT CSoundManager::SetPrimaryBufferFormat( DWORD dwPrimaryChannels,
		DWORD dwPrimaryFreq,
		DWORD dwPrimaryBitRate )
	{
		HRESULT             hr;
		LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

		if( m_pDS == NULL )
			return CO_E_NOTINITIALIZED;
		DSBUFFERDESC dsbd;
		ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
		dsbd.dwSize        = sizeof(DSBUFFERDESC);
		dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
		dsbd.dwBufferBytes = 0;
		dsbd.lpwfxFormat   = NULL;

		if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
			return hr;

		WAVEFORMATEX wfx;
		ZeroMemory( &wfx, sizeof(WAVEFORMATEX) );
		wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM;
		wfx.nChannels       = (WORD) dwPrimaryChannels;
		wfx.nSamplesPerSec  = (DWORD) dwPrimaryFreq;
		wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate;
		wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
		wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

		if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
			return hr;

		safe_release( pDSBPrimary );

		return S_OK;
	}

	HRESULT CSoundManager::Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener )
	{
		HRESULT             hr;
		DSBUFFERDESC        dsbdesc;
		LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

		if( ppDSListener == NULL )
			return E_INVALIDARG;
		if( m_pDS == NULL )
			return CO_E_NOTINITIALIZED;

		*ppDSListener = NULL;

		ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
		dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
		if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) )
			return hr;

		if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener,
			(VOID**)ppDSListener ) ) )
		{
			safe_release( pDSBPrimary );
			return hr;
		}
		safe_release( pDSBPrimary );

		return S_OK;
	}

	HRESULT CSoundManager::Create( CSound** ppSound,
		const char* strWaveFileName,
		DWORD dwCreationFlags,
		GUID guid3DAlgorithm,
		DWORD dwNumBuffers )
	{
		HRESULT hr;
		HRESULT hrRet = S_OK;
		DWORD   i;
		LPDIRECTSOUNDBUFFER* apDSBuffer     = NULL;
		DWORD                dwDSBufferSize = NULL;
		CWaveFile*           pWaveFile      = NULL;

		if( m_pDS == NULL )
			return CO_E_NOTINITIALIZED;
		if( strWaveFileName == NULL || ppSound == NULL || dwNumBuffers < 1 )
			return E_INVALIDARG;

		apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
		if( apDSBuffer == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto LFail;
		}

		pWaveFile = new CWaveFile();
		if( pWaveFile == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto LFail;
		}

		pWaveFile->OpenWaveFile( strWaveFileName, NULL, WAVEFILE_READ );

		if( pWaveFile->GetSize() == 0 )
		{
			hr = E_FAIL;
			goto LFail;
		}

		dwDSBufferSize = pWaveFile->GetSize();

		DSBUFFERDESC dsbd;
		ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
		dsbd.dwSize          = sizeof(DSBUFFERDESC);
		dsbd.dwFlags         = dwCreationFlags;
		dsbd.dwBufferBytes   = dwDSBufferSize;
		dsbd.guid3DAlgorithm = guid3DAlgorithm;
		dsbd.lpwfxFormat     = pWaveFile->GetFormat();

		hr = m_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL );

		if( hr == DS_NO_VIRTUALIZATION )
			hrRet = DS_NO_VIRTUALIZATION;

		if( FAILED(hr) )
		{
			goto LFail;
		}

		if( (dwCreationFlags & DSBCAPS_CTRLFX) == 0 )
		{
			for( i=1; i<dwNumBuffers; i++ )
			{
				if( FAILED( hr = m_pDS->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
				{
					goto LFail;
				}
			}
		}
		else
		{
			for( i=1; i<dwNumBuffers; i++ )
			{
				hr = m_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer[i], NULL );
				if( FAILED(hr) )
				{
					goto LFail;
				}
			}
		}

		*ppSound = new CSound( this, apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile, dwCreationFlags );
		safe_deleteList( apDSBuffer );
		return hrRet;

LFail:

		safe_delete( pWaveFile );
		safe_deleteList( apDSBuffer );
		return hr;
	}


	CSound::CSound( CSoundManager* pManager, LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize,
		DWORD dwNumBuffers, CWaveFile* pWaveFile, DWORD dwCreationFlags )
	{
		m_pManager	=	pManager;

		DWORD i;

		if( dwNumBuffers <= 0 )
			return;
		m_apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];

		m_apDS3DBuffer	=	0;
		if( NULL != m_apDSBuffer )
		{
			for( i=0; i<dwNumBuffers; i++ )
			{
				m_apDSBuffer[i] = apDSBuffer[i];
			}
			if(FAILED(m_apDSBuffer[0]->GetFrequency((LPDWORD)&m_dwDefaultFreq)))
			{
				m_dwDefaultFreq	=	pWaveFile->GetFormat()->nSamplesPerSec;
			}
			m_dwDSBufferSize = dwDSBufferSize;
			m_dwNumBuffers   = dwNumBuffers;
			m_pWaveFile      = pWaveFile;
			m_dwCreationFlags = dwCreationFlags;
			m_nMaxSoundBuffer = 10;
			FillBufferWithSound( m_apDSBuffer[0], FALSE );
		}
		memset(m_playRID, 0, sizeof(m_playRID));
		for(int i=0;i<MAX_SOUND_BUFFER;i++)
			m_playRID[i]	=	-1;
		m_fVolume	=	1.0f;
		m_fPan		=	1.0f;
	}

	VOID	CSound::Destroy()
	{
		for( DWORD i=0; i<m_dwNumBuffers; i++ )
		{
			safe_release( m_apDSBuffer[i] );
		}

		safe_deleteList( m_apDSBuffer );
		safe_release( m_pWaveFile );
	}

	CSound::~CSound()
	{
		Destroy();
	}

	HRESULT CSound::FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger )
	{
		HRESULT hr;
		VOID*   pDSLockedBuffer      = NULL;
		DWORD   dwDSLockedBufferSize = 0;
		DWORD   dwWavDataRead        = 0;

		if( pDSB == NULL )
			return CO_E_NOTINITIALIZED;
		if( FAILED( hr = RestoreBuffer( pDSB, NULL ) ) )
			return hr;
		if( FAILED( hr = pDSB->Lock( 0, m_dwDSBufferSize, &pDSLockedBuffer, (LPDWORD)&dwDSLockedBufferSize,NULL, NULL, DSBLOCK_ENTIREBUFFER ) ) )
			return hr;
		m_pWaveFile->ResetFile();

		if( !m_pWaveFile->ReadBuffer( (BYTE*) pDSLockedBuffer,dwDSLockedBufferSize,&dwWavDataRead) )
			return E_FAIL;

		if( dwWavDataRead == 0 )
		{
			FillMemory( (BYTE*) pDSLockedBuffer,dwDSLockedBufferSize,(BYTE)(m_pWaveFile->GetFormat()->wBitsPerSample == 8 ? 128 : 0 ) );
		}
		else if( dwWavDataRead < dwDSLockedBufferSize )
		{
			if( bRepeatWavIfBufferLarger )
			{
				DWORD dwReadSoFar = dwWavDataRead;
				while( dwReadSoFar < dwDSLockedBufferSize )
				{
					if( !m_pWaveFile->ResetFile() )
						return E_FAIL;

					if(!m_pWaveFile->ReadBuffer( (BYTE*)pDSLockedBuffer + dwReadSoFar, dwDSLockedBufferSize - dwReadSoFar, &dwWavDataRead ))
						return E_FAIL;

					dwReadSoFar += dwWavDataRead;
				}
			}
			else
			{
				FillMemory( (BYTE*) pDSLockedBuffer + dwWavDataRead, dwDSLockedBufferSize - dwWavDataRead, (BYTE)(m_pWaveFile->GetFormat()->wBitsPerSample == 8 ? 128 : 0 ) );
			}
		}
		pDSB->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );
		return S_OK;
	}

	HRESULT CSound::RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored )
	{
		HRESULT hr;

		if( pDSB == NULL )
			return CO_E_NOTINITIALIZED;
		if( pbWasRestored )
			*pbWasRestored = FALSE;

		DWORD dwStatus;
		if( FAILED( hr = pDSB->GetStatus( (LPDWORD)&dwStatus ) ) )
			return hr;

		if( dwStatus & DSBSTATUS_BUFFERLOST )
		{
			do
			{
				hr = pDSB->Restore();
				if( hr == DSERR_BUFFERLOST )
					Sleep( 10 );
			}
			while( ( hr = pDSB->Restore() ) == DSERR_BUFFERLOST );

			if( pbWasRestored != NULL )
				*pbWasRestored = TRUE;

			return S_OK;
		}
		else
		{
			return S_FALSE;
		}
	}

	LPDIRECTSOUNDBUFFER CSound::GetFreeBuffer(LPDIRECTSOUND3DBUFFER* p3DSndRet)
	{
		if( m_apDSBuffer == NULL )
			return FALSE;

		DWORD i;
		for( i=0; i<m_dwNumBuffers; i++ )
		{
			if( m_apDSBuffer[i] )
			{
				DWORD dwStatus = 0;
				m_apDSBuffer[i]->GetStatus( (LPDWORD)&dwStatus );
				if ( ( dwStatus & DSBSTATUS_PLAYING ) == 0 )
					break;
			}
		}

		if( i != m_dwNumBuffers )
		{
			if(p3DSndRet)
				*p3DSndRet	=	NULL;
			return m_apDSBuffer[ i ];
		}
		return NULL;
	}

	DWORD	CSound::GetFreeBufferIndex()
	{
		if( m_apDSBuffer == NULL )
			return 0xffffffff;

		DWORD i;
		for( i=0; i<m_dwNumBuffers; i++ )
		{
			if( m_apDSBuffer[i] )
			{
				DWORD dwStatus = 0;
				m_apDSBuffer[i]->GetStatus( (LPDWORD)&dwStatus );
				DWORD dwPlay = 0, dwWrite = 0;
				m_apDSBuffer[i]->GetCurrentPosition((LPDWORD)&dwPlay, (LPDWORD)&dwWrite);
				if(dwPlay >= this->m_dwDSBufferSize)
				{
					m_apDSBuffer[i]->Stop();
				}
				if ( ( dwStatus & DSBSTATUS_PLAYING ) == 0 )
					return i;
			}
		}
		DWORD dwRet	=	(DWORD)rand() % m_dwNumBuffers;
		return dwRet;
	}

	LPDIRECTSOUNDBUFFER CSound::GetBuffer( DWORD dwIndex )
	{
		if( m_apDSBuffer == NULL )
			return NULL;
		if( dwIndex >= m_dwNumBuffers )
			return NULL;

		return m_apDSBuffer[dwIndex];
	}

	HRESULT CSound::Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer )
	{
		if( m_apDSBuffer == NULL )
			return CO_E_NOTINITIALIZED;
		if( dwIndex >= m_dwNumBuffers )
			return E_INVALIDARG;

		*ppDS3DBuffer = NULL;

		return m_apDSBuffer[dwIndex]->QueryInterface( IID_IDirectSound3DBuffer,
			(VOID**)ppDS3DBuffer );
	}

	HRESULT CSound::Play( DWORD dwPriority, DWORD dwFlags, LONG lVolume, LONG lFrequency, LONG lPan )
	{
		HRESULT hr;
		BOOL    bRestored;

		if( m_apDSBuffer == NULL )
			return CO_E_NOTINITIALIZED;

		LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();

		if( pDSB == NULL )
			return E_FAIL;

		if( FAILED( hr = RestoreBuffer( pDSB, &bRestored ) ) )
			return hr;

		if( bRestored )
		{

			if( FAILED( hr = FillBufferWithSound( pDSB, FALSE ) ) )
				return hr;
		}

		if( m_dwCreationFlags & DSBCAPS_CTRLVOLUME )
		{
			pDSB->SetVolume( lVolume );
		}

		if( lFrequency != -1 &&
			(m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY) )
		{
			pDSB->SetFrequency( lFrequency );
		}

		if( m_dwCreationFlags & DSBCAPS_CTRLPAN )
		{
			pDSB->SetPan( lPan );
		}

		return pDSB->Play( 0, dwPriority, dwFlags );
	}

	HRESULT CSound::Stop()
	{
		if( m_apDSBuffer == NULL )
			return CO_E_NOTINITIALIZED;

		HRESULT hr = 0;

		for( DWORD i=0; i<m_dwNumBuffers; i++ )
			hr |= m_apDSBuffer[i]->Stop();

		return hr;
	}

	HRESULT CSound::Reset()
	{
		if( m_apDSBuffer == NULL )
			return CO_E_NOTINITIALIZED;

		HRESULT hr = 0;

		for( DWORD i=0; i<m_dwNumBuffers; i++ )
			hr |= m_apDSBuffer[i]->SetCurrentPosition( 0 );

		return hr;
	}

	BOOL CSound::IsSoundPlaying()
	{
		BOOL bIsPlaying = FALSE;

		if( m_apDSBuffer == NULL )
			return FALSE;

		for( DWORD i=0; i<m_dwNumBuffers; i++ )
		{
			if( m_apDSBuffer[i] )
			{
				DWORD dwStatus = 0;
				m_apDSBuffer[i]->GetStatus( (LPDWORD)&dwStatus );
				bIsPlaying |= ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
			}
		}

		return bIsPlaying;
	}

	VOID					CSound::SetVolumePan(PtrID& pID, float fVolume, float fPan)
	{
		this->SetVolume(&pID, fVolume, 0);
		this->SetPan(&pID, fPan);
	}

	BOOL					CSound::SetSoundParam(PtrID* pID, DS3DBUFFER* pBuffer)
	{
		if(pID->index >= this->m_dwNumBuffers)return FALSE;
		if(m_playRID[pID->index] != pID->id)return FALSE;
		return false;
	}

	BOOL					CSound::IsPlaying(PtrID& pID)
	{
		if(pID.index >= this->m_dwNumBuffers)return FALSE;
		if(m_playRID[pID.index] != pID.id)return FALSE;
		//
		DWORD dwStatus = 0;
		m_apDSBuffer[pID.index]->GetStatus( (LPDWORD)&dwStatus );
		if ( ( dwStatus & DSBSTATUS_PLAYING ) == 0 )
			return FALSE;
		return TRUE;
	}

	VOID					CSound::Stop(PtrID& pID)
	{
		if(pID.index >= this->m_dwNumBuffers)return;
		if(m_playRID[pID.index] != pID.id)return;
		m_apDSBuffer[pID.index]->Stop();
	}

	PtrID					CSound::Play(INT nLoopCount, float fVolume, float fPan, float fRate)
	{
        if(!g_manager.IsWavEnabled())
            return PtrID(-1, -1);
		PtrID id;
		if(!Play3D(&id, nLoopCount, 0))
			return PtrID(-1,-1);
		SetVolume(&id, fVolume, SoundType_SOUND2D);
		SetPan(&id, fPan);
		this->SetPlaySpeed(id, fRate);
		return id;
	}

	BOOL					CSound::Play3D( PtrID* pID, int nLoopCount, DS3DBUFFER* pBuffer )
	{
		if( m_apDSBuffer == NULL )
			return FALSE;
		if(IsPlaying(*pID))
			return FALSE;
		//
		DWORD dwIndex	=	GetFreeBufferIndex();
		if(dwIndex >= this->m_nMaxSoundBuffer)
			return FALSE;
		LPDIRECTSOUNDBUFFER		pDSB = this->m_apDSBuffer[ dwIndex ];
		if( pDSB == NULL /*|| p3DSnd == NULL*/ )
			return FALSE;
		pDSB->Stop();
		BOOL bRestored;
		if( FAILED( RestoreBuffer( pDSB, &bRestored ) ) )
			return FALSE;
		if( bRestored )
		{
			if( FAILED( FillBufferWithSound( pDSB, FALSE ) ) )
				return FALSE;
		}
		if( m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY )
		{
			pDSB->SetFrequency(m_dwDefaultFreq);
		}
		if(pBuffer)
		{
			pBuffer->dwSize	=	sizeof(DS3DBUFFER);
			pBuffer->dwMode = DS3DMODE_HEADRELATIVE;
		}
		//
		pID->index		=	dwIndex;
		static int id	=	1;
		pID->id		=	id++;
		this->m_playRID[ dwIndex ] = pID->id;
		pDSB->SetCurrentPosition(0);
		pDSB->Play( 0, 0, (nLoopCount < 0) ? DSBPLAY_LOOPING : 0 );
		//
		return TRUE;
	}

	void	CSound::SetPercentage(PtrID* pID, float f)
	{
		if(pID->index >= this->m_dwNumBuffers)return;
		if(m_playRID[pID->index] != pID->id)return;
		LPDIRECTSOUNDBUFFER pBuffer = m_apDSBuffer[pID->index];
		f	=	clamp(f, 0.0f, 1.0f);
		pBuffer->SetCurrentPosition((DWORD)(f * (float)m_dwDSBufferSize));
	}

	void	CSound::SetPlaySpeed(PtrID& pID, float fSpeed)
	{
		if(pID.index >= this->m_dwNumBuffers)return;
		if(m_playRID[pID.index] != pID.id)return;
		LPDIRECTSOUNDBUFFER pBuffer = m_apDSBuffer[pID.index];
		DWORD dwFreq		=	(DWORD)(getmax(fSpeed, 0.1f) * (float)m_dwDefaultFreq);
		if( m_dwCreationFlags & DSBCAPS_CTRLFREQUENCY )
			pBuffer->SetFrequency(dwFreq);
	}

	void	CSound::SetVolume(PtrID* pID, float f, int nSndType)
	{
		if(pID->index >= this->m_dwNumBuffers)return;
		if(m_playRID[pID->index] != pID->id)return;
		LPDIRECTSOUNDBUFFER pBuffer = m_apDSBuffer[pID->index];
		if(f > 1.0f)f = 1.0f;
		else if(f < .0f)f = 0.0f;
		m_fVolume	=	f;
		if( m_dwCreationFlags & DSBCAPS_CTRLVOLUME )
		{
			float fNewVolume = f;// * GetGlobalVolume(nSndType);
			if(fNewVolume > 0.01f)fNewVolume	=	sqrtf(fNewVolume);
			pBuffer->SetVolume( (long)(fNewVolume*10000.0f - 10000.0f) );
		}
	}

	void	CSound::SetPan(PtrID* pID, float f)
	{
		if(pID->index >= this->m_dwNumBuffers)return;
		if(m_playRID[pID->index] != pID->id)return;
		LPDIRECTSOUNDBUFFER pBuffer = m_apDSBuffer[pID->index];
		m_fPan	=	f;
		if( m_dwCreationFlags & DSBCAPS_CTRLVOLUME )
		{
			pBuffer->SetPan( (long)(f*10000.0f) );
		}
	}

	BOOL	CSound::GetVolume(PtrID* pID, float& f)
	{
		if(pID->index >= this->m_dwNumBuffers)return false;
		if(m_playRID[pID->index] != pID->id)return false;
		f	=	m_fVolume;
		return true;
	}

	BOOL	CSound::GetPan(PtrID* pID, float& f)
	{
		if(pID->index >= this->m_dwNumBuffers)return false;
		if(m_playRID[pID->index] != pID->id)return false;
		f	=	m_fPan;
		return true;
	}

};


#endif
#endif
