/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "PhantomManager.h"
#include "resource.h"
#include "MyInclude.h"
#include "GameManager.h"

//
//BOOL										LoadResData(CDynamicArray<char>& rets, const wchar_t* szResID, const wchar_t* szResType, HMODULE hModule)
//{
//	HRSRC hRsrc	=	::FindResourceW(hModule, szResID, szResType);
//	//获取资源的大小
//	DWORD dwSize = SizeofResource(hModule, hRsrc);
//	if(0 == dwSize)
//		return FALSE;
//	//加载资源
//	HGLOBAL hResHandle = LoadResource(hModule, hRsrc);
//	if(NULL == hResHandle)
//		return FALSE;
//	LPVOID pBuffer = LockResource(hResHandle);
//	if(NULL == pBuffer)
//		return FALSE;
//	rets.SetArrayCount(dwSize, false);
//	memcpy(rets.GetPtr(), pBuffer, dwSize);
//	UnlockResource(hResHandle);
//	FreeResource(hResHandle);
//	return TRUE;
//}

extern "C"{
	extern void MyCreatePhantom(int nDiv, int nOS);
};
extern BOOL	LoadResData(CDynamicArray<char>& rets, const wchar_t* szResID, const wchar_t* szResType, HMODULE hModule);
BOOL	InitPhantomEngine3D()
{
	CDynamicArray<char> defaultZip;
	if(!LoadResData(defaultZip, MAKEINTRESOURCE(INIT_DATA), L"IDR_INIT_DATA", AfxGetResourceHandle()))
		return false;
	//
	open_zip_mem(defaultZip.GetPtr(), defaultZip.size());
	OnAppCreate(0, 1, 0, 0);//MyCreatePhantom(1,0);
	CString strColor = AfxGetApp()->GetProfileStringW(L"PhantomVD", L"BgColor");
	Phantom::color4 color2(_wtoi(strColor));
	color2.a	=	1.0f;
	Phantom::g_manager->SetClearColor(color2);
	return true;
}
VOID	OnDeviceInit(){
	g_manager.SetActiveSDK(GetGameManager());
}
VOID	DestroyPhantomEngine3D()
{
	
}
VOID	OnIdleRender2()
{
	if(!OnAppFrameMove())
	{
		//SendMessage(hWindow, WM_CLOSE, 0, 0);
		return;
	}
	OnAppRender();
}

