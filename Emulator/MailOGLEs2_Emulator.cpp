/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009 - 2011 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#ifdef DIRECT_X_9//²»Ö§³ÖDirectX9

#else
#include "MailOGLEs2_Emulator.h"
#include "ES2/gl.h"
#include "EGL/egl.h"
#include "MyInclude.h"

extern	BOOL	OnWin32Proc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
HWND hWindow;
HDC  hDisplay;
HWND	g_hwndParent	=	0;
VOID	SetParentHWND(HWND hWndParent)
{
	g_hwndParent	=	hWndParent;
}
HWND	GetDeviceWindow()
{
	return hWindow;
}

EGLDisplay	sEGLDisplay;
EGLContext	sEGLContext;
EGLSurface	sEGLSurface;

VOID	OnRender()
{
	if(!OnAppRender())
		PostQuitMessage(0);
	else
		eglSwapBuffers(sEGLDisplay, sEGLSurface);
}

extern	VOID	InitSoundManager(HWND hWnd);
VOID	InitWindow()
{
    const unsigned int uiWidth  = 640;
    const unsigned int uiHeight = 480;
    hWindow = create_window(uiWidth, uiHeight);
	InitSoundManager(hWindow);
}
VOID	InitDevice()
{
    EGLint aEGLAttributes[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    EGLint aEGLContextAttributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLConfig	aEGLConfigs[1];
    EGLint		cEGLConfigs;
	hDisplay = EGL_DEFAULT_DISPLAY;
    sEGLDisplay = EGL_CHECK(eglGetDisplay(hDisplay));
    EGL_CHECK(eglInitialize(sEGLDisplay, NULL, NULL));
    EGL_CHECK(eglChooseConfig(sEGLDisplay, aEGLAttributes, aEGLConfigs, 1, &cEGLConfigs));
    if (cEGLConfigs == 0) {
        printf("No EGL configurations were returned.\n");
        exit(-1);
    }
    sEGLSurface = EGL_CHECK(eglCreateWindowSurface(sEGLDisplay, aEGLConfigs[0], (EGLNativeWindowType)hWindow, NULL));
    if (sEGLSurface == EGL_NO_SURFACE) {
        printf("Failed to create EGL surface.\n");
        exit(-1);
    }
    sEGLContext = EGL_CHECK(eglCreateContext(sEGLDisplay, aEGLConfigs[0], EGL_NO_CONTEXT, aEGLContextAttributes));
    if (sEGLContext == EGL_NO_CONTEXT) {
        printf("Failed to create EGL context.\n");
        exit(-1);
    }
    EGL_CHECK(eglMakeCurrent(sEGLDisplay, sEGLSurface, sEGLSurface, sEGLContext));
}

VOID	InitDevice(const RECT& rc)
{
	InitDevice();
	MoveWindow(hWindow, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 0);
	OnAppResize(3, rc.right, rc.bottom, rc.right, rc.bottom);
	OnAppRestore();
}
VOID	RunDemo()
{
	InitDevice();
	{
		RECT rc;
		GetClientRect(hWindow, &rc);
		OnAppResize(3, rc.right, rc.bottom, rc.right, rc.bottom);
		OnAppRestore();
		MSG sMessage;
		SetTimer(hWindow, 0, 50, 0);
		int bDone = 0;
		while(!bDone)
		{
			if(PeekMessage(&sMessage, NULL, 0, 0, PM_REMOVE)) {
				if(sMessage.message == WM_QUIT) {
					bDone = 1;
				} else {
					TranslateMessage(&sMessage);
					DispatchMessage(&sMessage);
				}
			}
			if(!OnAppFrameMove())
			{
				SendMessage(hWindow, WM_CLOSE, 0, 0);
				break;
			}
			OnRender();
			//Sleep(50);
		};
		OnAppLost();
		
	}
    /* EGL clean up */
    EGL_CHECK(eglMakeCurrent(sEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
    EGL_CHECK(eglDestroySurface(sEGLDisplay, sEGLSurface));
    EGL_CHECK(eglDestroyContext(sEGLDisplay, sEGLContext));
    EGL_CHECK(eglTerminate(sEGLDisplay));
}

VOID	OnIdleRender()
{
	if(!OnAppFrameMove())
	{
		SendMessage(hWindow, WM_CLOSE, 0, 0);
		return;
	}
	OnRender();
}

HWND create_window(int uiWidth, int uiHeight) {
	WNDCLASS wc;
	RECT wRect;
	HWND sWindow;
	HINSTANCE hInstance;

	wRect.left = 0L;
	wRect.right = (long)uiWidth;
	wRect.top = 0L;
	wRect.bottom = (long)uiHeight;

	hInstance = GetModuleHandle(NULL);

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)process_window;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"OGLES";

	RegisterClass(&wc);

	AdjustWindowRectEx(&wRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
	DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	if(g_hwndParent)
		dwStyle	=	WS_CHILD;
	sWindow = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, L"OGLES", L"main", dwStyle, 0, 0, uiWidth, uiHeight, g_hwndParent, NULL, hInstance, NULL);

	ShowWindow(sWindow, SW_SHOW);
	SetForegroundWindow(sWindow);
	SetFocus(sWindow);

	return sWindow;
}

LRESULT CALLBACK process_window(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(hWnd, &pos);
	OnWin32Proc(hWnd, uiMsg, wParam, lParam);
	switch(uiMsg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_LBUTTONDBLCLK:
		//OnAppInputEvent(2, pos.x, pos.y);
		return 0;
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		OnAppInputEvent(2, pos.x, pos.y, 0);
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		OnAppInputEvent(4, pos.x, pos.y, 0);
		break;
	case WM_MOUSEMOVE:
		OnAppInputEvent(1, pos.x, pos.y, 0);
		break;
	case WM_MOUSEWHEEL:
		OnAppInputEvent(5, (short)HIWORD(wParam), 0, 0);
		break;
	case WM_SETFOCUS:
		break;
	case WM_KILLFOCUS:
		break;
	case WM_ACTIVATEAPP:
		return 0;
	case WM_ACTIVATE:
		return 0;
	case WM_KEYUP:
		return 0;
	case WM_TIMER:
		{
			static BOOL bRender	=	false;
			if(!bRender)
			{
				bRender	=	true;
				bRender	=	false;
			}
		}
		break;
	case WM_KEYDOWN:
		if(wParam == VK_RETURN)
		{
			OnAppInputEvent(5, 0, 0, 0);
		}
		break;
	case WM_SIZE:
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			OnAppResize(3, rc.right, rc.bottom, rc.right, rc.bottom);
		}
		return 0;
	}
	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

#endif
