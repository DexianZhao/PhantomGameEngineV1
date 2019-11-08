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
#ifndef ___PHANTOM_OS__H___
#define ___PHANTOM_OS__H___
#include "PhantomMath.h"
#include "PhantomMusic2D.h"
#ifdef ANDROID_OS
#include <jni.h>
#include <android/log.h>
#endif

namespace Phantom{


#ifdef ANDROID_OS
	char* JStringToChars(JNIEnv* env, jstring jstr);
	jobject NewJavaClass(JNIEnv* env, jclass obj_class);
	jstring CharsToJString(JNIEnv* env, const char* pat);
	struct	WaveCommand
	{
		enum command_id{
			command_id_open	=	1,
			command_id_play,
			command_id_stop,
			command_id_pause,
			command_id_resume,
			command_id_openurl,
			command_id_openedit,
			command_id_closeedit,
			command_id_showad,
			command_id_hidead,
			command_id_buy,
			command_id_restore
		};

		int			c;
		int			id;
		FileName	file;
	};
	int				GetWaveCommandCount();
	void			GetWaveCommandIndex(WaveCommand& ret, int index);
	void			ClearWaveCommands();
	void			AddWaveCommand(int cmdid, int param, const char* f);
#endif
	MusicControl* CreateMusicPtr(const char* szFile);
};


#endif
