/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "GCRenderSystem.h"
#include "GCStateDefault.h"
#include "GCLanguage.h"
#include "resource.h"
#include "PhantomManager.h"
#include "GCStateDefault.h"
#include "GCViewManager.h"
extern	VOID	InitDevice(const RECT& rc);
extern	VOID	SetParentHWND(HWND hWndParent);
extern	VOID	InitWindow();
extern	BOOL	InitPhantomEngine3D();
extern	VOID	OnDeviceInit();
extern	HWND	GetDeviceWindow();

//
//BOOL	IsExistLicense()
//{
//	char buf[20480];
//	memset(buf, 0, sizeof(buf));
//#ifdef Developer_Authorize
//	DWORD dw	=	::GetProfileStringA(SOFTWARE_NAME, "license_dev", "", buf, 20480);
//#else
//	DWORD dw	=	::GetProfileStringA(SOFTWARE_NAME, "license", "", buf, 20480);
//#endif
//	if(dw == 0 || !buf[0])
//		return false;
//	return true;
//}
//
//VOID	WriteLicense(const char* license)
//{
//#ifdef Developer_Authorize
//	DWORD dw	=	::WriteProfileStringA(SOFTWARE_NAME, "license_dev", license);
//#else
//	DWORD dw	=	::WriteProfileStringA(SOFTWARE_NAME, "license", license);
//#endif
//}
//
//BOOL	ReadLicense(char* license)
//{
//	char buf[20480];
//	memset(buf, 0, sizeof(buf));
//#ifdef Developer_Authorize
//	DWORD dw	=	::GetProfileStringA(SOFTWARE_NAME, "license_dev", "", buf, 20480);
//#else
//	DWORD dw	=	::GetProfileStringA(SOFTWARE_NAME, "license", "", buf, 20480);
//#endif
//	if(dw == 0 || !buf[0])
//		return false;
//	//
//	PhantomV1::Logic::SplitText ret[8];
//	int r = ::SplitTextBySymbol(buf, ret, 8, '|');
//	if(r != 8)
//	{
//		return false;
//	}
//	int i8[8];
//	memset(i8, 0, sizeof(i8));
//	for(int i=0;i<8;i++)
//		i8[i] = atoi(ret[i].texts.GetPtr());
//	//
//	memcpy(license, i8, sizeof(i8));
//	return true;
//}

//CPhantomProject* s_proj = 0;
INT		g_nGameWindowWidth = 800;
INT		g_nGameWindowHeight = 600;
INT		GetRenderWindowWidth()
{
	return g_nGameWindowWidth;
}

INT		GetRenderWindowHeight()
{
	return g_nGameWindowHeight;
}

VOID	SetRenderWindowSize(INT w, INT h)
{
	g_nGameWindowWidth	=	w;
	g_nGameWindowHeight	=	h;
	RECT rc;
	HWND hWnd	=	GetDeviceWindow();
	MoveWindow(hWnd, VIEW_LEFT_RESERVED, VIEW_TOP_RESERVED, g_nGameWindowWidth, g_nGameWindowHeight, true);
}

//struct StartupData;
//BOOL	InitRenderSystem(HWND hWndMain, const StartupData* cbProcs, LPCSTR szTitle, const PRect* rc, BOOL bShowWindow)
//{
//	return false;
//}

HWND	g_hCreateMainWindow	=	0;	//创建渲染窗口的句柄
HWND	g_hWndCurrentFrame	=	0;	//当前使用渲染窗口的视图
extern BOOL	InitPhantomEngine3D();
//ISystemZipFile*								g_configZip = 0;	//配置压缩文件
//PHANTOM_API			BOOL					EngineStartup()
//{
//	CDynamicArray<char> defaultZip;
//	if(!LoadResData(defaultZip, MAKEINTRESOURCE(IDR_INIT_RENDER), L"IDR_INIT_DATA", AfxGetResourceHandle()))
//		return false;
//	//
//	ZipResult r;
//	g_configZip	=	openZipFile(defaultZip.GetPtr(), defaultZip.size(), "./", 0, r);
//	if(!g_configZip)
//		return false;
//	insertZipFile(g_configZip);
//	return InitPhantomEngine3D();
//}
//
//PHANTOM_API		VOID						EngineCleanup()
//{
//	if(g_configZip)
//		removeZipFile(g_configZip);
//	safe_release(g_configZip);
//}

VOID	SetBackgroundColor(const Phantom::color4& c)
{
	//GetProjectPtr()->m_renderState.uClearColor	=	c;
}
extern VOID	OnDeviceInit();
extern	VOID	InitDevice(const RECT& rc);
extern	VOID	SetParentHWND(HWND hWndParent);
extern	VOID	InitWindow();
BOOL	InitRenderSystem(HWND hWndMain)
{
//	EngineStartup();
	g_hCreateMainWindow	=	hWndMain;
	//if(!s_proj)
	//	s_proj	=	new CPhantomProject();
	//static GCStateDefault	def;
	//s_proj->RegisterState(def.C_StateID, &def);
	//s_proj->SetFirstState(def.C_StateID);
	//s_proj->m_hwndParent	=	hWndMain;
	//s_proj->m_uWindowStyle	=	WS_CHILD;
	//wchar_t name[256];
	//swprintf(name, L"幻影可视化设计器2012");
	////
	////if(fexist(0, "..\\execute\\default\\default.zip"))
	////	SetCurrentDirectory(L"..\\execute\\default\\");
	//if(!s_proj->StartupSystem(name, &PRect(0, 0, 800, 600), FALSE))
	//	return false;
	//SetProjectPath();
	//g_nGameWindowWidth	=	GetRenderSystemPtr()->Phantom::g_manager.GetBufferWidth();
	//g_nGameWindowHeight	=	GetRenderSystemPtr()->Phantom::g_manager.GetBufferHeight();
	//GetRenderSystemPtr()->SetRenderEnable(true);
	//SetRenderParent(NULL);
	////
	//color4 color(GetConfigInt("BgColor", GetProjectPtr()->m_renderState.uClearColor));
	//color.a	=	1.0f;
	//GetProjectPtr()->m_renderState.uClearColor	=	color;
	OnDeviceInit();
	CString strColor = AfxGetApp()->GetProfileStringW(L"PhantomVD", L"BgColor");
	Phantom::color4 color2(_wtoi(strColor));
	color2.a	=	1.0f;
	Phantom::g_manager->SetClearColor(color2);
	return true;
}

VOID	DestroyRenderSystem()
{
	//if(s_proj)
	//{
	//	GetRenderSystemPtr()->DestroySystem();
	//	delete s_proj;
	//}
}
VOID	SetMainWindow(HWND hWnd){
	g_hCreateMainWindow = hWnd;
}
VOID	SetRenderParent(HWND hWndParent)
{
	BOOL bShow = TRUE;
	if(!hWndParent)
	{
		bShow		=	FALSE;
		hWndParent	=	g_hCreateMainWindow;
	}
	g_hWndCurrentFrame	=	hWndParent;
	HWND hWndRender	=	GetDeviceWindow();//GetRenderSystemPtr()->GetDeviceWindow();
	MoveWindow(hWndRender, VIEW_LEFT_RESERVED, VIEW_TOP_RESERVED, GetRenderWindowWidth(), GetRenderWindowHeight(), TRUE);
	SetParent(hWndRender, hWndParent);
	ShowRender(bShow);
}

VOID	ShowRender(BOOL bShow)
{
	HWND hWndRender	=	GetDeviceWindow();//GetRenderSystemPtr()->GetDeviceWindow();
	ShowWindow(hWndRender, bShow ? SW_SHOW : SW_HIDE);
}

VOID	MoveRenderWindow(HWND hWndCurrent, INT offsetX, INT offsetY)
{
	if(hWndCurrent != g_hWndCurrentFrame)
		return;
	HWND hWndRender	=	GetDeviceWindow();//GetRenderSystemPtr()->GetDeviceWindow();
	MoveWindow(hWndRender, VIEW_LEFT_RESERVED + offsetX, VIEW_TOP_RESERVED + offsetY, GetRenderWindowWidth(), GetRenderWindowHeight(), TRUE);
}
extern VOID	OnAppMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL	OnWin32Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!GetViewManager())
		return false;
	GCViewBase* view	=	GetViewManager()->GetActiveView();
	if(view)
	{
		if(uMsg==WM_MBUTTONDOWN||uMsg==WM_RBUTTONDOWN||uMsg==WM_LBUTTONDOWN)
			SetFocus(hWnd);
		if(view->OnWindowProc((HWND)hWnd, uMsg, wParam, lParam) && view->GetEditorMode() >= 0)
		{
			switch(uMsg)
			{
			case WM_KEYUP:
			case WM_KEYDOWN:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MBUTTONDBLCLK:
			case WM_MOUSEWHEEL:
			case WM_MOUSEMOVE:
				return true;
			}
		}
	}
	OnAppMsgProc((HWND)hWnd, uMsg, wParam, lParam);
	return false;
}
