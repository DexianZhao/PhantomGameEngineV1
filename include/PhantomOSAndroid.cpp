//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   MyGameApp.cpp
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#include <MyInclude.h>
#include "PhantomOS.h"
#include "PhantomManager.h"
using namespace Phantom;

#ifdef ANDROID_OS
#include <jni.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
int	g_nScreenDiv = 2;
int	g_openid = 0;

namespace Phantom{
	extern	VOID	SetAuthCode(unsigned int* r);
};

struct AndroidLock
{
	AndroidLock(){pthread_mutex_init(&crit, 0);}
	~AndroidLock(){pthread_mutex_destroy(&crit);}
	VOID				Lock(){::pthread_mutex_lock(&crit);}
	VOID				Unlock(){::pthread_mutex_unlock(&crit);}
	//
	pthread_mutex_t		crit;
};
std::string	g_inputText;
VOID	SetInputText(const char* s){
	g_inputText = s;
}
const char*	GetInputText(){
	return g_inputText.c_str();
}
JNIEnv*		g_envPtr = 0;
jobject		g_surfacePtr = 0;
BOOL						GetLocalIP(char* ip)//返回本机局域网IP
{
	LogInfo("Call:GetLocalIP");
	strcpy(ip, "");
	jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
	jmethodID fnDeviceName = g_envPtr->GetStaticMethodID(surfaceClass, "GetLocalIP", "()Ljava/lang/String;");  
	jstring ret = static_cast<jstring>(g_envPtr->CallStaticObjectMethod(surfaceClass, fnDeviceName));
	if(ret==0)
		return false;
	const char* pname = g_envPtr->GetStringUTFChars(ret, NULL);
	if(!pname||!pname[0])
		return false;
	strcpy(ip, pname);
	LogInfo("GetLocalIP(%s)", ip);
	return true;
}
	VOID			FromLuaMessage(LPCSTR szName/*消息名称*/, LPCSTR szParams/*消息参数1*/, LPCSTR szParams2/*消息参数2*/)
	{
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "FromLuaMessage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
		jstring sname = g_envPtr->NewStringUTF(szName);
		jstring sparam = g_envPtr->NewStringUTF(szParams);
		jstring sparam2 = g_envPtr->NewStringUTF(szParams2);
		g_envPtr->CallStaticIntMethod(surfaceClass, fnTemp, sname, sparam, sparam2);
	}
extern "C" {
	extern	void	RefreshAppScore(int type, int point);
	extern	int		OnAndroidBack();
	extern	VOID	CreateMainSDK(BOOL bDyn);
	const char*	onAlert(const char* message, const char* title, const char* type)
	{
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "onAlert", "(Ljava/lang/String;)V");
		jstring strkey = g_envPtr->NewStringUTF(message);
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, strkey);
		LogInfo("onAlert(%s)", message);
		return "ok";
	}
	const char*		call_lua(const char* type, const char* params, const char* params2);//调用lua内部的函数oncall
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onResize(JNIEnv * env, jobject obj,  jint ori, jint width, jint height, jint devwidth, jint devheight);
	JNIEXPORT jboolean JNICALL Java_com_android_phantom_Phantom_onFrameMove(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onRender(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onCreate(JNIEnv * env, jobject obj, jstring text, jstring filedir, jintArray codes);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onDestroy(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onLost(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_setScorePoint(JNIEnv * env, jobject obj, jint type, jint score);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onRestore(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onPause(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onResume(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onMouseEvent(JNIEnv * env, jobject obj, jint index, jint evt, jint x, jint y);
	JNIEXPORT void	JNICALL Java_com_android_phantom_Phantom_setDocumentDir(JNIEnv * env, jobject obj, jstring str);
	JNIEXPORT void	JNICALL Java_com_android_phantom_Phantom_closeEdit(JNIEnv * env, jobject obj, jint iscancel, jint id, jstring str);
	//
	extern	VOID	SetUserID(int id);
	BOOL		RunProj(int index, const char* ip);
	int		g_runIndex = -1;
	JNIEXPORT void	JNICALL Java_com_android_phantom_Phantom_resetconn(JNIEnv * env, jobject obj, jstring str)
	{
		char *t =JStringToChars(env, str);
		RunProj(g_runIndex, t);
		if(t)free(t);
	}
	JNIEXPORT void	JNICALL Java_com_android_phantom_Phantom_setRunIndex(JNIEnv * env, jobject obj, jint index)
	{
		g_runIndex = index;
	}
	
	extern VOID	researchProjects();
	JNIEXPORT void	JNICALL Java_com_android_phantom_Phantom_researchProj(JNIEnv * env, jobject obj, jint userid)
	{
		SetUserID(userid);
		researchProjects();
	}
	JNIEXPORT jint	JNICALL Java_com_android_phantom_Phantom_onBack(JNIEnv * env, jobject obj);
	JNIEXPORT jint	JNICALL Java_com_android_phantom_Phantom_getCommandCount(JNIEnv * env, jobject obj);
	JNIEXPORT jint	JNICALL Java_com_android_phantom_Phantom_getCommand(JNIEnv * env, jobject obj, jint index);
	JNIEXPORT jint	JNICALL Java_com_android_phantom_Phantom_getCommandID(JNIEnv * env, jobject obj, jint index);
	JNIEXPORT jstring	JNICALL Java_com_android_phantom_Phantom_getCommandFile(JNIEnv * env, jobject obj, jint index);
	JNIEXPORT void	JNICALL Java_com_android_phantom_Phantom_clearCommand(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_setInputText(JNIEnv * env, jobject obj, jstring text);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_setPixels(JNIEnv * env, jobject obj, jintArray pixels, jint width, jint height, jint textWidth, jint textHeight);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_updateGravity(JNIEnv * env, jobject obj, jfloat gx, jfloat gy, jfloat gz);
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_sendMessage(JNIEnv * env, jobject obj, jstring stype, jstring sparam, jstring sparam2);

	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_updateAcc(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z, jlong timestamp)
	{
		GetCameraPtr()->processAcc(x,y,z, timestamp);
	}
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_updateGyro(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z, jlong timestamp)
	{
		GetCameraPtr()->processGyro(x,y,z, timestamp);
	}

	VOID	AppExitGame(INT nParam)
	{
		LogInfo("user terminate.");
		exit(0);
	}

	void    saveConfig(const char* key, const char* value)
	{
		LogInfo("Call:SaveConfig");
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnSaveConfig = g_envPtr->GetStaticMethodID(surfaceClass, "SaveConfig", "(Ljava/lang/String;Ljava/lang/String;)V");  
		jstring strkey = g_envPtr->NewStringUTF(key);
		jstring strvalue = g_envPtr->NewStringUTF(value);
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnSaveConfig, strkey, strvalue);
	}
	int     loadConfig(const char* key,char* retvalue)
	{
		LogInfo("Call:LoadConfig");
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnLoadConfig = g_envPtr->GetStaticMethodID(surfaceClass, "LoadConfig", "(Ljava/lang/String;)Ljava/lang/String;");  
		jstring strkey = g_envPtr->NewStringUTF(key);
		jstring ret = static_cast<jstring>(g_envPtr->CallStaticObjectMethod(surfaceClass, fnLoadConfig, strkey));
		if(ret==0)
		{
			retvalue[0] = 0;
			return 0;
		}
		const char* pname = g_envPtr->GetStringUTFChars(ret, NULL);
		if(!pname||!pname[0])
			return 0;
		strcpy(retvalue, pname);
		LogInfo("LoadConfig(%s,%s)", key, retvalue);
		return 1;
	}
	const char* GetDeviceName(char*ch)
	{
		LogInfo("Call:GetDeviceName");
		strcpy(ch, "Android");
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnDeviceName = g_envPtr->GetStaticMethodID(surfaceClass, "GetDeviceName", "()Ljava/lang/String;");  
		jstring ret = static_cast<jstring>(g_envPtr->CallStaticObjectMethod(surfaceClass, fnDeviceName));
		if(ret==0)
			return ch;
		const char* pname = g_envPtr->GetStringUTFChars(ret, NULL);
		if(!pname||!pname[0])
			return ch;
		strcpy(ch, pname);
		LogInfo("GetDeviceName(%s)", ch);
		return ch;
	}
	const char* GetDeviceID(char*ch)
	{
		LogInfo("Call:GetDeviceID");
		strcpy(ch, "Android");
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnDeviceName = g_envPtr->GetStaticMethodID(surfaceClass, "GetDeviceID", "()Ljava/lang/String;");  
		jstring ret = static_cast<jstring>(g_envPtr->CallStaticObjectMethod(surfaceClass, fnDeviceName));
		if(ret==0)
			return ch;
		const char* pname = g_envPtr->GetStringUTFChars(ret, NULL);
		if(!pname||!pname[0])
			return ch;
		strcpy(ch, pname);
		LogInfo("GetDeviceID(%s)", ch);
		return ch;
	}

	void    SetViewOrientation(int ori)
	{
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnSaveConfig = g_envPtr->GetStaticMethodID(surfaceClass, "SetViewOrientation", "(I)V");  
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnSaveConfig, ori);
	}
	int		CallOpenMusic(const char* szFile)
	{
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "OpenMp3", "(Ljava/lang/String;)I");
		jstring strkey = g_envPtr->NewStringUTF(szFile);
		return g_envPtr->CallStaticIntMethod(surfaceClass, fnTemp, strkey);
	}
	VOID	CallPlayMusic(int id, int loopCount, float fVolume)
	{
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "PlayMp3", "(IIF)V");  
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, id, loopCount, fVolume);
	}
	VOID	CallStopMusic(int id)
	{
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "StopMp3", "(I)V");  
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, id);
	}
	VOID	CallMusicVolume(int id, float fVolume)
	{
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "SetVolumeMp3", "(IF)V");  
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, id, fVolume);
	}
	VOID	CallCloseMusic(int id)
	{
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "CloseMp3", "(I)V");  
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, id);
	}
	int		CallOpenWave(const char* szFile)
	{
		LogInfo("CallOpenWave(%s)", szFile);
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "OpenWave", "(Ljava/lang/String;)I");
		jstring strkey = g_envPtr->NewStringUTF(szFile);
		return g_envPtr->CallStaticIntMethod(surfaceClass, fnTemp, strkey);
	}
	VOID	CallPlayWave(int id, float fVolume)
	{
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "PlayWave", "(IF)V");
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, id, fVolume);
	}
	VOID	CallCloseWave(int id)
	{
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "CloseWave", "(I)V");  
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, id);
	}
	VOID	CallPauseMusic(int id)
	{
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "PauseMp3", "(I)V");  
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, id);
	}
	VOID	CallResumeMusic(int id)
	{
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "ResumeMp3", "(I)V");  
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, id);
	}

	void	openRank(const char* rank){}
	void	setRankScore(const char* rank, int value){}
	void	setAchievement(const char* achievement, int persent){}
	void    showAd(int x, int y, int type, int bShow)
	{
		LogInfo("showAd(%d,%d)", type,bShow);
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "ShowAd", "(II)V");
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, type, bShow);
	}
	void    shopBuy(const char* buyName)
	{
		LogInfo("shopBuy(%s)", buyName);
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "ShowBuy", "(Ljava/lang/String;)V");
		jstring strkey = g_envPtr->NewStringUTF(buyName);
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, strkey);
	}
	void    shopRestore(const char* buyName)
	{
		LogInfo("shopRestore(%s)", buyName);
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "RestoreBuy", "(Ljava/lang/String;)V");
		jstring strkey = g_envPtr->NewStringUTF(buyName);
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, strkey);
	}
	void	showMyAD(const char* image, const char* url, int advid, int type)
	{
	}
	void	setAdMode(int mode, const char** urls, const char** images, int count, int myadv_gailv)//设置我的广告条列表
	{
	}
	void	gotoBuyRemoveAD(int)
	{
	}
	void	openUrl(const char* url)
	{
		LogInfo("openUrl(%s)", url);
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "OpenUrl", "(Ljava/lang/String;)V");
		jstring strkey = g_envPtr->NewStringUTF(url);
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, strkey);
		//AddWaveCommand(WaveCommand::command_id_openurl, 0, url);
	}
	void	gotoComment()
	{
		openUrl("");
	}
	void	openEdit(const char* szDefault, int x, int y, int w, int h, int id)
	{
		LogInfo("openEdit()");
		g_openid = id;
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "OpenEdit", "(Ljava/lang/String;IIIII)V");
		jstring strkey = g_envPtr->NewStringUTF(szDefault);
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, strkey,x,y,w,h, id);
	}
	void	closeEdit(int bIsCancel)
	{
		LogInfo("Call:closeEdit");
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "CloseEdit", "(I)V");  
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, (jint)bIsCancel);
	}

	int            DrawTextAndroidA(const char* text, int textLength, int fontHeight, int fontStyle, const char* fontName, int nTextAlignment)
	{
		//LogAndroid("Call:DrawTexts");
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnSaveConfig = g_envPtr->GetStaticMethodID(surfaceClass, "DrawTexts", "(Ljava/lang/String;Ljava/lang/String;III)V");
		int len = strlen(text);
		char buf[128];
		sprintf(buf, "TEXT:%d", len);
		__android_log_print(ANDROID_LOG_DEBUG, "libc", buf);
		jstring strtext = g_envPtr->NewStringUTF(text);
		jstring strname = g_envPtr->NewStringUTF(fontName);
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnSaveConfig, strname, strtext, fontHeight, fontStyle, nTextAlignment);
		return 0;
	}

	//   int            DrawTextAndroid(const unsigned short* text, int textLength, int fontHeight, int fontStyle, const char* fontName)
	//{
	//	char* buf = new char[textLength*2+1];
	//	memset(buf, 0, textLength*2+1);
	//	int len = WToUtf8(text, buf, textLength*2+1);
	//	buf[len] = 0;
	//	LogAndroid("Text:%s", buf);
	//	jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
	//	jmethodID fnSaveConfig = g_envPtr->GetStaticMethodID(surfaceClass, "DrawTexts", "(Ljava/lang/String;II)V");
	//	jstring strtext = g_envPtr->NewStringUTF(buf);
	//	g_envPtr->CallStaticVoidMethod(surfaceClass, fnSaveConfig, strtext, fontHeight, fontStyle);
	//	delete[] buf;
	//	return 0;
	//}
	void	onVibrator(float fSecond)
	{
		int nMill = fSecond * 1000;
		LogInfo("onVibrator(%d)", nMill);
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "OnVibrator", "(I)V");
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, nMill);
	}
	void	OnDrawText(int* pixels, int width, int height, int textWidth, int textHeight);

	void	GetHomeUrl(char* ret)
	{
		LogInfo("Call:GetHomeUrl");
		strcpy(ret, "www.aixspace.com");
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnDeviceName = g_envPtr->GetStaticMethodID(surfaceClass, "GetHomeUrl", "()Ljava/lang/String;");  
		jstring jret = static_cast<jstring>(g_envPtr->CallStaticObjectMethod(surfaceClass, fnDeviceName));
		if(jret==0)
			return;
		const char* pname = g_envPtr->GetStringUTFChars(jret, NULL);
		if(!pname||!pname[0])
			return;
		strcpy(ret, pname);
		LogInfo("GetHomeUrl(%s)", ret);
	}
	void* g_tempRet = 0;
	int	g_tempRetLen = 0;
	JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_setByteArray(JNIEnv * env, jobject obj, jbyteArray texts)
	{if(!g_tempRet)return;
	jboolean isCp;
	jbyte* arr=env->GetByteArrayElements(texts,&isCp);
	jint length=env->GetArrayLength(texts);
	const char* src = (const char*)arr;
	if(src[0]==-1&&src[1]==-2)
	{
		length-=2;
		src+=2;
	}
	memcpy(g_tempRet, src, length);
	g_tempRetLen = length;
	env->ReleaseByteArrayElements(texts, arr, 0);
	}
	int		Utf8ToWideOS(const char* text, int textlen, unsigned short* ret)
	{
		g_tempRetLen = 0;
		g_tempRet = ret;
		memset(ret, 0, textlen*2);
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "Utf8ToWideOS", "(Ljava/lang/String;)V");
		jstring strkey = g_envPtr->NewStringUTF(text);
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, strkey);
		g_tempRet = 0;
		return g_tempRetLen/2;
	}
	int		WideToUtf8OS(const unsigned short* text, int textlen, char* ret)
	{
		return 0;
	}
	void	gotoMain(){
		LogInfo("gotoMain");
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "OnGotoMain", "()V");
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp);
	}
	VOID	ClearProject()
	{
		LogInfo("ClearProject");
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "ClearProject", "()V");
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp);
	}
	VOID	UpdateProject(){
		LogInfo("UpdateProject");
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "UpdateProject", "()V");
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp);
	}
	VOID	AddProject(const char* projName, const char* tm, int index)
	{
		LogInfo("AddProject");
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jstring sproj = g_envPtr->NewStringUTF(projName);
		jstring stm = g_envPtr->NewStringUTF(tm);
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "AddProject", "(Ljava/lang/String;Ljava/lang/String;I)V");
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, sproj,stm,(jint)index);
	}
	void	SetupWave(unsigned int w, int dupCount)
	{
		LogInfo("SetupWave");
		jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
		jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "SetupWave", "(II)V");
		g_envPtr->CallStaticVoidMethod(surfaceClass, fnTemp, w, dupCount);
	}
	void SetRealDocumentDir(const char* t);
};
INT		GetMemoryFree(){
	jclass surfaceClass = g_envPtr->FindClass("com/android/phantom/PhantomSurface");
	jmethodID fnTemp = g_envPtr->GetStaticMethodID(surfaceClass, "GetMemoryFree", "()I");
	return g_envPtr->CallStaticIntMethod(surfaceClass, fnTemp);
}
JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_setPixels(JNIEnv * env, jobject obj, jintArray pixels, jint width, jint height, jint textWidth, jint textHeight)
{
	jboolean isCp;
	jint* arr=env->GetIntArrayElements(pixels,&isCp);
	jint length=env->GetArrayLength(pixels);
	//LogAndroid("setPixels:%d,%d,%d,%d,%d", width,height,textWidth,textHeight,isCp);
	OnDrawText(arr, width,height,textWidth,textHeight);

	env->ReleaseIntArrayElements(pixels, arr, 0);
}

JNIEXPORT void	JNICALL Java_com_android_phantom_Phantom_closeEdit(JNIEnv * env, jobject obj, jint iscancel, jint id, jstring str)
{
	g_envPtr		=	env;
	g_surfacePtr	=	obj;
	char *t =JStringToChars(env, str);
	if(!t)
		return;
	GetDlgManager()->OnEditClose(id, iscancel, t);
	free(t);
}

JNIEXPORT void	JNICALL Java_com_android_phantom_Phantom_setDocumentDir(JNIEnv * env, jobject obj, jstring text)
{
	g_envPtr		=	env;
	g_surfacePtr	=	obj;
	char *t =JStringToChars(env, text);
	if(!t)
		return;
	SetDocumentDir(t);
	SetRealDocumentDir(t);
	free(t);
}

JNIEXPORT void	JNICALL Java_com_android_phantom_Phantom_clearCommand(JNIEnv * env, jobject obj)
{
	ClearWaveCommands();
}
JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_updateGravity(JNIEnv * env, jobject obj, jfloat gx, jfloat gy, jfloat gz)
{
	Phantom::g_manager.AddGravity(gx,gy,gz);
	//Phantom::g_manager.m_sensordata.gravity_x = gx;
	//Phantom::g_manager.m_sensordata.gravity_y = gy;
	//Phantom::g_manager.m_sensordata.gravity_z = gz;
}
JNIEXPORT jint	JNICALL Java_com_android_phantom_Phantom_getCommandCount(JNIEnv * env, jobject obj)
{
	return GetWaveCommandCount();
}

JNIEXPORT jint	JNICALL Java_com_android_phantom_Phantom_getCommand(JNIEnv * env, jobject obj, jint index)
{
	WaveCommand wc;
	GetWaveCommandIndex(wc,index);
	return wc.c;
}

JNIEXPORT jint	JNICALL Java_com_android_phantom_Phantom_getCommandID(JNIEnv * env, jobject obj, jint index)
{
	WaveCommand wc;
	GetWaveCommandIndex(wc,index);
	return wc.id;
}

JNIEXPORT jstring	JNICALL Java_com_android_phantom_Phantom_getCommandFile(JNIEnv * env, jobject obj, jint index)
{
	WaveCommand wc;
	GetWaveCommandIndex(wc,index);
	jstring jstr = NULL;
	jstr			=	env->NewStringUTF(wc.file.t);
	return jstr;
}
JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_setScorePoint(JNIEnv * env, jobject obj, jint type, jint score)
{
	RefreshAppScore(type, score);
}
JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onMouseEvent(JNIEnv * env, jobject obj, jint index, jint evt, jint x, jint y)
{
	OnAppInputEvent(evt, x, y, index);
}

extern	int	InitGame();
JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_sendMessage(JNIEnv * env, jobject obj, jstring stype, jstring sparam, jstring sparam2)
{
	char *_type =JStringToChars(env, stype);
	if(!_type)
		return;
	char *_param =JStringToChars(env, sparam);
	if(!_param)
		_param="";
	char *_param2 =JStringToChars(env, sparam2);
	if(!_param2)
		_param2="";
	call_lua(_type, _param, _param2);
	free(_type);
	free(_param);
	free(_param2);
}
JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_setInputText(JNIEnv * env, jobject obj, jstring text)
{
	char *t =JStringToChars(env, text);
	if(!t)
		return;
	SetInputText(t);
	free(t);
}
extern VOID ResetAllMain();
JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onCreate(JNIEnv * env, jobject obj, jstring text, jstring filedir, jintArray codes)
{
	g_envPtr		=	env;
	g_surfacePtr	=	obj;
	__android_log_print(ANDROID_LOG_DEBUG, "libc", "OnCreate");
	if(g_manager.IsInitialized())
	{
		//g_manager.AppDestroy();
		ResetAllMain();
		//CreateMainSDK(true);
		return;
	}
	char *t =JStringToChars(env, text);
	if(!t)
		return;
#ifdef ENGINE_LIB_MODE
	jint* arr=env->GetIntArrayElements(codes,NULL);
	jint length=env->GetArrayLength(codes);
	//
	if(length!=512)
	{
		free(t);
		LogAndroid("FailAuth");
		return;
	}
	unsigned char guid[512];
	for(int i=0;i<512;i++){
		guid[i] = arr[i];
	}
	SetPhantomGUID((unsigned int*)guid);
	SetAuthCode((unsigned int*)guid);
#endif
	//
	char* fdir = JStringToChars(env, filedir);
	LogInfo("FileDir[%s]", fdir);
	LogInfo("SetDocumentDir[%d]", fdir);
	SetDocumentDir(fdir);
	LogInfo("free[%d]", fdir);
	free(fdir);
	//要重新设置程序所在目录的doc目录
	//SetDocumentDir("/sdcard/");//要设置成data目录
	LogInfo("OpenZip[%s]", t);
	ZipFilePackage* zip = open_zip(t, "");
	LogInfo("OpenZip2[%d]", zip);
	zip->addDir("assets");
	SetCharLocale("en_US.UTF-8", 1);
	LogInfo("InitGame");
	g_nScreenDiv	=	InitGame();
	LogInfo("CreateOK1");
	OnAppCreate(0, g_nScreenDiv, OSType_Android, 0);
	LogInfo("CreateOK2");
	CreateMainSDK(true);
	free(t);
	LogInfo("CreateOK");
}

JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onDestroy(JNIEnv * env, jobject obj)
{
	OnAppDestroy();
}
JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onLost(JNIEnv * env, jobject obj)
{
	OnAppLost();
}
VOID			GoBackClear();
JNIEXPORT jint	JNICALL Java_com_android_phantom_Phantom_onBack(JNIEnv * env, jobject obj)
{
	if(OnAndroidBack())
		return 1;
	GoBackClear();
	return 0;
}
JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onRestore(JNIEnv * env, jobject obj)
{
	g_envPtr = env;
	OnAppRestore();
}
JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onResize(JNIEnv * env, jobject obj,  jint ori, jint width, jint height, jint devwidth, jint devheight)
{
	g_envPtr = env;
	OnAppResize(ori, width, height, devwidth, devheight);
}
JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onPause(JNIEnv * env, jobject obj)
{
	OnAppPause();
}
JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onResume(JNIEnv * env, jobject obj)
{
	OnAppResume();
}

JNIEXPORT jboolean JNICALL Java_com_android_phantom_Phantom_onFrameMove(JNIEnv * env, jobject obj)
{
	g_envPtr = env;
	return OnAppFrameMove();//
}

JNIEXPORT void JNICALL Java_com_android_phantom_Phantom_onRender(JNIEnv * env, jobject obj)
{
	g_envPtr = env;
	OnAppRender();
}

#endif
