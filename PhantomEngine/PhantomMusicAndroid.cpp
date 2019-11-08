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

#ifdef ANDROID_OS
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


extern "C"{
	int		CallOpenMusic(const char* szFile);
	VOID	CallPlayMusic(int id, int nLoopCount, float fVolume);
	VOID	CallStopMusic(int id);
	VOID	CallMusicVolume(int id, float fVolume);
	VOID	CallCloseMusic(int id);
	int		CallOpenWave(const char* szFile);
	VOID	CallPlayWave(int id, float fVolume);
	VOID	CallCloseWave(int id);
	VOID	CallPauseMusic(int id);
	VOID	CallResumeMusic(int id);
	void	SetupWave(unsigned int w, int dupCount);
};

#endif
namespace	Phantom{

	VOID	GetExistFile(char* ret, const char* file){
		LFileName retf;
		if(fexist(file, &retf))
			strcpy(ret, retf.t);
		else
			strcpy(ret, file);
	}
#ifdef ANDROID_OS
	char* JStringToChars(JNIEnv* env, jstring jstr)
	{
		char* rtn = NULL;
		jclass clsstring = env->FindClass("java/lang/String");
		jstring strencode = env->NewStringUTF("utf-8");
		jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
		jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
		jsize alen = env->GetArrayLength(barr);
		jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
		if (alen > 0)
		{
			rtn = (char*)malloc(alen + 1);

			memcpy(rtn, ba, alen);
			rtn[alen] = 0;
		}
		env->ReleaseByteArrayElements(barr, ba, 0);
		return rtn;
	}
	jobject NewJavaClass(JNIEnv* env, jclass obj_class){  
		jmethodID construction_id = env->GetMethodID(obj_class, "<init>", "()V");  
		jobject obj = env->NewObject(obj_class, construction_id);  
		return obj;  
	}

	jstring CharsToJString(JNIEnv* env, const char* pat)
	{
		jclass strClass = env->FindClass("Ljava/lang/String;");
		jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
		jbyteArray bytes = env->NewByteArray(strlen(pat));
		env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
		jstring encoding = env->NewStringUTF("utf-8");
		return (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
	}

	CDynamicArray<WaveCommand>		g_commands;
	int				GetWaveCommandCount()
	{
		return g_commands.size();
	}
	void			GetWaveCommandIndex(WaveCommand& ret, int index)
	{
		if(index < 0 || index >= g_commands.size())
			return;
		ret	=	g_commands[index];
	}
	void			ClearWaveCommands()
	{
		g_commands.SetArrayCount(0);
	}
	void			AddWaveCommand(int c, int id, const char* f)
	{
		WaveCommand wc;
		wc.c	=	c;
		wc.id	=	id;
		if(f)
			wc.file	=	f;
		g_commands.push_back(&wc);
	}

	class	PhantomMusicOS: public MusicControl
	{
	public:
		PhantomMusicOS();
		~PhantomMusicOS();
		BOOL							Create();	//初始化
		//
		virtual	VOID					Play(INT nLoopCount, float fVolume, float fPan, float fRate);
		virtual	VOID					Stop();
		virtual	VOID					Pause();
		virtual	BOOL					IsPause(){return m_bIsPause;}
		virtual	VOID					Resume();
		virtual	BOOL					IsPlaying();
		virtual	VOID					SetVolume(float fVolume);
		virtual	VOID					SetPan(float fPan);
		virtual	BOOL					Open(const char* szFile);
	public:
		int			m_nMusicID;
		BOOL		m_bIsPlaying;
		BOOL		m_bIsPause;
		float		m_fVolume;
		float		m_fPan;
		FileName		m_fileName;
	};
	PhantomMusicOS::PhantomMusicOS()
	{
		static int g_nMusicID = 1;
		m_nMusicID		=	0;//g_nMusicID++;
		m_fVolume	=	1.0f;
		m_fPan		=	0;
		m_bIsPlaying	=	false;
		m_bIsPause	=	false;
	}
	PhantomMusicOS::~PhantomMusicOS()
	{
		if(m_nMusicID>0)
			CallCloseMusic(m_nMusicID);
	}
	BOOL							PhantomMusicOS::Create()	//初始化
	{
		return true;
	}
	VOID					PhantomMusicOS::Play(INT nLoopCount, float fVolume, float fPan, float fRate)
	{
		if(m_bIsPlaying || !g_manager.IsMp3Enabled() || m_nMusicID <= 0)
			return;
		//LogInfo("system->playmp3(%s,%d)", m_fileName.t, m_nMusicID);
		//AddWaveCommand(WaveCommand::command_id_play, m_nMusicID, 0);
		m_fPan	=	clamp(fPan, -1, 1);
		m_fVolume = clamp(fVolume, 0, 1);
		CallPlayMusic(m_nMusicID, nLoopCount, m_fVolume);
		m_bIsPlaying	=	true;
	}
	VOID					PhantomMusicOS::Stop()
	{
		if(!m_bIsPlaying || m_nMusicID <= 0)
			return;
		LogInfo("system->stopmp3(%s,%d)", m_fileName.t, m_nMusicID);
		m_bIsPlaying	=	false;
		CallStopMusic(m_nMusicID);
		//AddWaveCommand(WaveCommand::command_id_stop, m_nMusicID, 0);
	}
	VOID					PhantomMusicOS::Pause()
	{
		if(!m_bIsPlaying || m_bIsPause || m_nMusicID <= 0)
			return;
		//LogInfo("music:Pause %s", m_fileName.t);
		LogInfo("system->pausesnd(%s)", m_fileName.t);
		m_bIsPause	=	true;
		CallPauseMusic(m_nMusicID);
		//AddWaveCommand(WaveCommand::command_id_stop, m_nMusicID, 0);
	}
	VOID					PhantomMusicOS::Resume()
	{
		if(!m_bIsPlaying || m_bIsPause == false || m_nMusicID <= 0)
			return;
		//LogInfo("music:Resume %s", m_fileName.t);
		LogInfo("system->resumesnd(%s)", m_fileName.t);
		CallResumeMusic(m_nMusicID);
		//AddWaveCommand(WaveCommand::command_id_resume, m_nMusicID, 0);
		m_bIsPause	=	false;
	}
	BOOL					PhantomMusicOS::IsPlaying()
	{
		return m_bIsPlaying;//
	}
	VOID					PhantomMusicOS::SetVolume(float fVolume)
	{
		if(!m_bIsPlaying || m_bIsPause == false || m_nMusicID <= 0)
			return;
		CallMusicVolume(m_nMusicID, fVolume);
	}
	VOID					PhantomMusicOS::SetPan(float fPan)
	{
		m_fPan	=	clamp(fPan, -1.0f, 1.0f);
		if(!m_bIsPlaying || m_bIsPause == false || m_nMusicID <= 0)
			return;
	}
	BOOL					PhantomMusicOS::Open(const char* szFile)
	{
		m_fileName	=	szFile;
		m_fileName.setIsFile();
		m_bIsPlaying = false;
		char buf[1024];
		GetExistFile(buf, m_fileName.t);
		m_nMusicID	=	CallOpenMusic(buf);
		//AddWaveCommand(WaveCommand::command_id_open, m_nMusicID, szFile);
		return true;
	}

	class	PhantomAndroidWavs: public MultiMusic
	{
	public:
		PhantomAndroidWavs();
		~PhantomAndroidWavs();
		BOOL							Create();
		virtual	BOOL					Open(const char* szFile);
		virtual	PtrID					Play(INT nLoopCount = -1, float fVolume = 1.0f, float fPan = 0.0f, float fRate = 1.0f);
		virtual	VOID					Pause(PtrID& pID);
		virtual	VOID					Resume(PtrID& pID);
		virtual	VOID					Stop(PtrID& pID);
		virtual	VOID					SetVolumePan(PtrID& pID, float fVolume, float fPan);
		virtual	VOID					SetPlaySpeed(PtrID& pID, float f);
		//
		virtual	BOOL					IsPlaying(PtrID& pIn){return true;}
		virtual	VOID					StopAll(){}
		virtual	VOID					PauseAll(){}
		virtual	VOID					ResumeAll(){}
		virtual	VOID					Setup(INT nDup){
			SetupWave(m_nID, nDup);
		}
	public:
		int			m_soundID;
		int			m_nID;
	};
	PhantomAndroidWavs::PhantomAndroidWavs()
	{
		//static int nSoundID = 1;
		m_nID		=	0;//nSoundID++;
		m_soundID	=	-1;
	}
	PhantomAndroidWavs::~PhantomAndroidWavs()
	{
		if(m_nID>0)
			CallCloseWave(m_nID);
	}
	BOOL							PhantomAndroidWavs::Create()	//初始化
	{
		return true;
	}

	PtrID				PhantomAndroidWavs::Play(INT nLoopCount, float fVolume, float fPan, float fRate)
	{
		if(m_nID > 0 && g_manager.IsWavEnabled())
		{
			if(nLoopCount < 0)nLoopCount = 10;
			fVolume	=	clamp(fVolume, 0, 1);
			jfloat v1 = fVolume;
			jfloat v2 = fVolume;
			if(fPan < 0)
				v2	=	fVolume * (1.0f + fPan);
			else
				v1	=	(.0f - fPan) * fVolume;
			//jint loop	=	nLoopCount;
			//jfloat rate	=	fRate;
			//jint soundID	=	m_soundID;
			//LogInfo("system->playwave(%s)", m_fileName.t);
			CallPlayWave(m_nID, fVolume);
			//AddWaveCommand(WaveCommand::command_id_play + 100, m_nID, 0);
			return PtrID(0, m_nID);
		}
		return PtrID(0, 0);
	}
	VOID					PhantomAndroidWavs::Stop(PtrID& pID)
	{
		//if(pID.id > 0)
		//{
		//	LogInfo("StopWave %s", m_fileName.t);
		//	AddWaveCommand(WaveCommand::command_id_play + 100, m_nID, 0);
		//}
	}
	VOID					PhantomAndroidWavs::Pause(PtrID& pID)
	{
	}
	VOID					PhantomAndroidWavs::Resume(PtrID& pID)
	{
	}
	VOID					PhantomAndroidWavs::SetPlaySpeed(PtrID& pID, float f)
	{
	}
	VOID					PhantomAndroidWavs::SetVolumePan(PtrID& pID, float fVolume, float fPan)
	{
	}
	BOOL					PhantomAndroidWavs::Open(const char* szFile)
	{
		if(m_nID>0)return false;
		m_fileName	=	szFile;
		m_fileName.setIsFile();
		LogInfo("music:OpenWave %s", m_fileName.t);
		char buf[1024];
		GetExistFile(buf, m_fileName.t);
		m_nID = CallOpenWave(buf);
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
			PhantomAndroidWavs* ret	=	new PhantomAndroidWavs();
			ret->Create();
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
