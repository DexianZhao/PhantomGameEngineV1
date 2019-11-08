
#ifdef DIRECT_X_9//
#include "DXUT.h"
#include <commdlg.h>
#include "MyInclude.h"
extern	BOOL	OnWin32Proc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnLostDevice( void* pUserContext );
void CALLBACK OnDestroyDevice( void* pUserContext );
void InitApp();

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
HWND	g_hWndParent	=	0;
HWND	GetDeviceWindow()
{
	return DXUTGetHWND();
}
VOID	SetParentHWND(HWND hWndParent)
{
	g_hWndParent	=	hWndParent;
}

extern	VOID	InitSoundManager(HWND hWnd);

int		g_nWidth = 320;
int		g_nHeight = 480;
wchar_t	g_szTitle2[256];
VOID	InitWindowWH(int w, int h, const wchar_t* szTitle)
{
	if(szTitle)
		wcscpy(g_szTitle2, szTitle);
	else
		wcscpy(g_szTitle2, L"PhantomGameEngine SDK Demos");
	g_nWidth	=	w;
	g_nHeight	=	h;
    DXUTSetCallbackD3D9DeviceAcceptable( IsDeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnCreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnResetDevice );
    DXUTSetCallbackD3D9FrameRender( OnFrameRender );
    DXUTSetCallbackD3D9DeviceLost( OnLostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( KeyboardProc );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCursorSettings( true, true );
    InitApp();
    DXUTInit( true, true ); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling( true, true, true );  // handle the defaul hotkeys
    DXUTCreateWindow(g_hWndParent, g_szTitle2 );
	InitSoundManager(DXUTGetHWND());
}
VOID	InitWindow()
{
	InitWindowWH(320, 480, 0);
}
VOID	InitDevice(const RECT& rc)
{
	DXUTCreateDevice( true, rc.right - rc.left, rc.bottom - rc.top );
	MoveWindow(DXUTGetHWND(), 0, 0, rc.right - rc.left, rc.bottom - rc.top, true);
}

VOID	RunDemo()
{
	DXUTCreateDevice( true, g_nWidth, g_nHeight );
	RECT rcWindow, rcClient;
	GetWindowRect(DXUTGetHWND(), &rcWindow);
	GetClientRect(DXUTGetHWND(), &rcClient);
	int bW = (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
	int bH = (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);
	MoveWindow(DXUTGetHWND(), 0, 0, g_nWidth + bW, g_nHeight + bH, true);
	DXUTMainLoop();
}

void InitApp()
{
}

IDirect3DDevice9* g_pd3dDevice = 0;
IDirect3DDevice9 * GetDevice9(void)
{
	return g_pd3dDevice;
}
HWND	GetHWND()
{
	return DXUTGetHWND();
}

bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}

bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    assert( DXUT_D3D9_DEVICE == pDeviceSettings->ver );

    HRESULT hr;
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    D3DCAPS9 caps;
    V( pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal,
                            pDeviceSettings->d3d9.DeviceType,
                            &caps ) );
    if( ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
        caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
    {
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
    return true;
}

HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	g_pd3dDevice	=	pd3dDevice;
    return S_OK;
}
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	g_pd3dDevice	=	pd3dDevice;
	OnAppResize(pBackBufferSurfaceDesc->Width>pBackBufferSurfaceDesc->Height?3:1,pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
	OnAppRestore();
    return S_OK;
}

void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	if(!OnAppFrameMove())
	{
		SendMessage(DXUTGetHWND(), WM_CLOSE, 0, 0);
	}
}

void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	g_pd3dDevice	=	pd3dDevice;
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		//pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xf000000, 1.0f, 0L );
		if(!OnAppRender())
		{
			
		}
        pd3dDevice->EndScene();
    }
}
BOOL g_bLButtonDown2 = false;
BOOL g_bRButtonDown2 = false;
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(hWnd, &pos);
	switch(uMsg)
	{
	case WM_LBUTTONDBLCLK:
		OnAppInputEvent(2, pos.x, pos.y, 0);
		break;
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		OnAppInputEvent(2, pos.x, pos.y, 0);
		g_bLButtonDown2 = true;
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		OnAppInputEvent(4, pos.x, pos.y, 0);
		g_bLButtonDown2 = false;
		break;
	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		OnAppInputEvent(2, pos.x, pos.y, 1);
		g_bRButtonDown2 = true;
		break;
	case WM_RBUTTONUP:
		ReleaseCapture();
		OnAppInputEvent(4, pos.x, pos.y, 1);
		g_bRButtonDown2 = false;
		break;
	case WM_MOUSEMOVE:
		{
			if(g_bLButtonDown2)
				OnAppInputEvent(1, pos.x, pos.y, 0);
			if(g_bRButtonDown2)
				OnAppInputEvent(1, pos.x, pos.y, 1);
		}
		break;
	case WM_MOUSEWHEEL:
		OnAppInputEvent(5, (short)HIWORD(wParam), 0, 0);
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		OnAppDestroy();
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
			OnAppResize((rc.right-rc.left)>(rc.bottom-rc.top)?3:1,rc.right, rc.bottom, rc.right, rc.bottom);
		}
		break;
	}
	if(OnWin32Proc(hWnd, uMsg, wParam, lParam))
		*pbNoFurtherProcessing = 1;
	return 0;
}

void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
}

void CALLBACK OnLostDevice( void* pUserContext )
{
	OnAppLost();
}
void CALLBACK OnDestroyDevice( void* pUserContext )
{
}

#endif
