//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
	Design Writer :   赵德贤 Dexian Zhao
	Email: yuzhou_995@hotmail.com
	 
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_DEFINE_H__
#define __PHANTOM_DEFINE_H__

#ifndef NULL
#define NULL	0
#endif

#ifdef __IPHONE_NA
#ifndef IPHONE_OS
#define	IPHONE_OS
#define	USE_WCHAR
#endif

#elif defined(ANDROID)
#ifndef ANDROID_OS
#define	ANDROID_OS
#endif

#elif defined(__MAC_NA)
#ifndef MAC_OS
#define	MAC_OS
#endif

#else
#ifndef WIN32_OS
#define	WIN32_OS
#define	USE_WCHAR
#include <Windows.h>
#endif

#endif

#if defined(IPHONE_OS) || defined(ANDROID_OS)
#define	stricmp			strcasecmp
#define	strnicmp		strncasecmp
#define wcsicmp         wcscasecmp
#define wcsnicmp        wcsncasecmp
#define _wtoi(s)        wcstol(s, 0, 10)
#define _wtof(s)        wcstod(s, 0)
#else

#endif


//声明Windows使用的大写例子
//#ifdef WIN32_OS
//
//#else
#ifndef WIN32_OS
#define	VOID			void
#define	DWORD			unsigned int
#define	UINT			unsigned int
#define	INT				int
#define	LPCSTR			const char*
#define	WORD			unsigned short
#define	BYTE			unsigned char
#define	BOOL			int
#define	TRUE			1
#define	FALSE			0
#define	LONG			int
#else
#pragma warning(disable:4018)
#pragma warning(disable:4995)
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4305)
#pragma warning(disable:4309)
#endif

//#endif
#define MAKEWORD_t(a, b)      ((WORD)(((BYTE)(((DWORD)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD)(b)) & 0xff))) << 8))
#define MAKELONG_t(a, b)      ((LONG)(((WORD)(((DWORD)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD)(b)) & 0xffff))) << 16))
#define LOWORD_t(l)           ((WORD)(((DWORD)(l)) & 0xffff))
#define HIWORD_t(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xffff))
#define LOBYTE_t(w)           ((BYTE)(((DWORD)(w)) & 0xff))
#define HIBYTE_t(w)           ((BYTE)((((DWORD)(w)) >> 8) & 0xff))

#define getmax(a,b)            (((a) > (b)) ? (a) : (b))
#define getmin(a,b)            (((a) < (b)) ? (a) : (b))
template <typename T>
inline	T						abs_t(T t){if(t>0.0)return t; else if(t<0.0)return -t; return t;}

#define	safe_release_vec(v)	{int num = (v).size(); for(size_t iV=0;iV<num;iV++){(v).at(iV)->Release();} (v).clear();}
#define	safe_delete_vec(v)	{int num = (v).size(); for(size_t iV=0;iV<num;iV++){delete (v).at(iV);} (v).clear();}
#define	safe_release(p)	if(p){(p)->Release();p=0;}
#define	safe_addref(p)	if(p){(p)->AddRef();}
#define	safe_delete(p)	if(p){delete (p);p=0;}
#define	safe_deleteList(p)	if(p){delete[] (p);p=0;}
#define	safe_free(p)	if(p){free_s(p);p=0;}

#endif

