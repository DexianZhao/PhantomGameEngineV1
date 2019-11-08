
// BaiWanGameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCGame.h"
#include "MFCGameDlg.h"
#include "afxdialogex.h"
#include "MyInclude.h"
#include "GameManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBaiWanGameDlg 对话框




CBaiWanGameDlg::CBaiWanGameDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBaiWanGameDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBaiWanGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBaiWanGameDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_MOVING()
	ON_BN_CLICKED(IDOK, &CBaiWanGameDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CBaiWanGameDlg::OnBnClickedCancel)
	ON_WM_MOVE()
END_MESSAGE_MAP()


// CBaiWanGameDlg 消息处理程序
extern	VOID	InitDevice(const RECT& rc);
extern	VOID	SetParentHWND(HWND hWndParent);
extern	VOID	InitWindow();

CBaiWanGameDlg*	g_dlg = 0;
BOOL	g_bIsInit = false;

VOID ThisLogPrint(const char* szText)//输出信息
{
#ifdef USE_DLG
	if(g_dlg)
		g_dlg->m_log.AddLog(szText);
#endif
}
extern	const char*	GetProjectName();
extern "C"{
	extern void MyCreatePhantom(int nDiv, int nOS);
    const char* GetDeviceName(char*ch)
	{
		return "windows";
	}
    void    saveConfig(const char* key, const char* value)
	{
		
	}
    int     loadConfig(const char* key,char* retvalue)
	{
		return 0;
	}
	int		GetMemoryFree()//返回系统空闲内存
	{
		return 1024*1024*1024;
	}
};


BOOL		LoadResData(CDynamicArray<char>& rets, const wchar_t* szResID, const wchar_t* szResType, HMODULE hModule)
{
	HRSRC hRsrc	=	::FindResourceW(hModule, szResID, szResType);
	//获取资源的大小
	DWORD dwSize = SizeofResource(hModule, hRsrc);
	if(0 == dwSize)
		return FALSE;
	//加载资源
	HGLOBAL hResHandle = LoadResource(hModule, hRsrc);
	if(NULL == hResHandle)
		return FALSE;
	LPVOID pBuffer = LockResource(hResHandle);
	if(NULL == pBuffer)
		return FALSE;
	rets.SetArrayCount(dwSize, false);
	memcpy(rets.GetPtr(), pBuffer, dwSize);
	UnlockResource(hResHandle);
	FreeResource(hResHandle);
	return TRUE;
}
BOOL CBaiWanGameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//::SetCurrentDirectoryA("../bin");
	g_dlg = this;
	SetLogPrint(ThisLogPrint);
#ifdef USE_DLG
	m_log.Create(m_log.IDD, 0);
	m_log.ShowWindow(SW_SHOW);
#endif
	//m_bind.Create(m_bind.IDD, 0);
	//m_bind.ShowWindow(SW_SHOW);
	// IDM_ABOUTBOX 必须在系统命令范围内。
	assert((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	assert(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		assert(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CRect client, window;
	GetClientRect(&client);
	GetWindowRect(&window);
	int w	=	GetSystemMetrics(SM_CXSCREEN);
	int h	=	GetSystemMetrics(SM_CYSCREEN);
	int windowWidth	=	960;
	int windowHeight	=	640;
	char wdrive[256], wdir[256];
	char szIni[256];// = 
	char screenW[128],screenH[128], current[128];
	char szExePath[1024];
	int len = ::WideCharToMultiByte(CP_ACP, 0, _wpgmptr, -1, szExePath, 1024, "", 0);
	szExePath[len] = 0;

	_splitpath(szExePath, wdrive, wdir, 0, 0);
	strcat(wdrive, wdir);
	strcpy(szIni, wdrive);
	strcat(szIni, "Phantom.ini");
	char szWidth[128], szHeight[128];
	char file[128];
	GetPrivateProfileStringA("active", "name", "", file, 128, szIni);
	::GetPrivateProfileStringA(file, "window_width", "960", szWidth, 128, szIni);
	::GetPrivateProfileStringA(file, "window_height", "640", szHeight, 128, szIni);
	windowWidth = atoi(szWidth);
	windowHeight = atoi(szHeight);
	CRect loadRc = LoadRect("main", CRect(300, 0, 0, 0));
	if(loadRc.left<0)loadRc.left=0;
	if(loadRc.top<0)loadRc.top=0;
	if(loadRc.left>1024)loadRc.left=1024;
	if(loadRc.top>800)loadRc.top=800;
	MoveWindow(loadRc.left, loadRc.top, windowWidth + (window.Width() - client.Width()), windowHeight + (window.Height() - client.Height()));
	//
	ShowWindow(SW_SHOW);
	SetTimer(0, 20, 0);
	//
	SetParentHWND(m_hWnd);
	InitWindow();

		CDynamicArray<char> defaultZip;
		if(!LoadResData(defaultZip, MAKEINTRESOURCE(IDR_INIT_DATA), L"INIT_DATA", AfxGetResourceHandle()))
			return false;
		//
		Phantom::open_zip_mem(defaultZip.GetPtr(), defaultZip.size());

	//AddZipFile("..\\android\\PhantomApp\\data.zip", "assets");
	MyCreatePhantom(1,0);
	RECT rc;
	GetClientRect(&rc);
	InitDevice(rc);
	m_edit.Create(m_edit.IDD, this);
	m_edit.ShowWindow(SW_HIDE);
	//
	g_manager.SetActiveSDK(GetGameManager());
	wchar_t wbuf[1024];
	len = ::MultiByteToWideChar(CP_ACP, 0, GetGameManager()->GetGameName(), -1, wbuf, 1024);
	wbuf[len] = 0;
	wchar_t buf[512];
	swprintf(buf, L"幻影多平台模拟器[%s]", wbuf);
	SetWindowText(buf);
	g_bIsInit = true;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

extern "C"
{
	BOOL CreatePhantom(int nDiv, int nOS, int* authcode)
	{
		return OnAppCreate(0, nDiv, OSType_Windows, authcode);
	}
	extern	const char*		call_lua(const char* type, const char* params);//调用lua内部的函数oncall
    extern	void    showAd(int x, int y, int type, int bShow){
		call_lua("jifen_score", "100");
		bool b = false;
	}
    extern	void    shopBuy(const char* buyName){}
	extern	void    shopRestore(const char* buyName){}
	extern void SetBuyADOK(BOOL bFail);
	extern	void	onVibrator(float fSecond){}
	void	gotoBuyRemoveAD(int){SetBuyADOK(false);}
	void	gotoComment(){}
	void	openRank(const char* rank){}
	void	setRankScore(const char* rank, int value){}
	void	setAchievement(const char* achievement, int persent){}
	void	openUrl(const char* url){
		::ShellExecuteA(g_dlg->m_hWnd, "open", url, 0, 0, SW_SHOW);
	}
	void	openEdit(const char* szDefault, int x, int y, int w, int h)
	{
		CRect rc(x, y, x + w, y + h);
		g_dlg->ClientToScreen(&rc);
		rc.OffsetRect(-::GetSystemMetrics(SM_CXBORDER), -::GetSystemMetrics(SM_CYCAPTION) - GetSystemMetrics(SM_CYBORDER));
		g_dlg->m_edit.MoveWindow(&rc);
		g_dlg->m_edit.m_ed.MoveWindow(0, 0, w, h);
		g_dlg->m_edit.ShowWindow(SW_SHOW);
	}
	void	closeEdit(char* ret)
	{
		//g_dlg->m_edit.m_ed.GetWindowTextA(ret, 256);
		g_dlg->m_edit.ShowWindow(SW_HIDE);
	}
	const char*	onAlert(const char* message, const char* title, const char* type)
	{
		wchar_t wmessage[2048];
		wchar_t wtitle[2048];
		wtitle[0] = 0;
		wmessage[0] = 0;
		if(message)
		{
			int len = strlen(message);
			len = ::MultiByteToWideChar(CP_UTF8, 0, message, len, wmessage, len+1);
			wmessage[len] = 0;
		}
		if(message)
		{
			int len = strlen(title);
			len = ::MultiByteToWideChar(CP_UTF8, 0, title, len, wtitle, len+1);
			wtitle[len] = 0;
		}
		int nType = MB_OK;
		if(stricmp(type, "yesno") == 0)
			nType = MB_YESNO;
		else if(stricmp(type, "okcancel") == 0)
			nType = MB_OKCANCEL;
		else if(stricmp(type, "yesnocancel") == 0)
			nType = MB_YESNOCANCEL;
		int nRet = g_dlg->MessageBox(wmessage, wtitle, nType);
		if(nRet == IDCANCEL)return "cancel";
		if(nRet == IDYES)return "yes";
		if(nRet == IDNO)return "no";
		return "ok";
	}
};

int __cdecl OnWin32Proc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if(uiMsg == WM_MOUSEMOVE){
		RECT rc;
		GetClientRect(hWnd, &rc);
		POINT pos;
		GetCursorPos(&pos);
		ScreenToClient(hWnd, &pos);
		float h2 = (rc.bottom - rc.top)/2;
		float w = (rc.right - rc.left);
		g_manager.m_sensordata.gravity_z = (float)(pos.y - h2)/h2;
		g_manager.m_sensordata.theta_z = (float)pos.x / w * 360.0;
	}
	return 0;
}

void CBaiWanGameDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBaiWanGameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBaiWanGameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



extern	VOID	OnIdleRender();
void CBaiWanGameDlg::OnTimer(UINT_PTR nIDEvent)
{
	static BOOL b = false;
	if(!b)
	{
		b	=	true;
		OnIdleRender();
		b	=	false;
	}
	CDialogEx::OnTimer(nIDEvent);
}

extern	HWND	GetDeviceWindow();
void CBaiWanGameDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if(cx > 0 && cy > 0 && GetDeviceWindow())
		::MoveWindow(GetDeviceWindow(), 0, 0, cx, cy, true);
	//OnAppResize(cx, cy);
}


void CBaiWanGameDlg::OnMoving(UINT fwSide, LPRECT pRect)
{
//#ifdef USE_DLG
//	if(m_log.m_hWnd){
//		int w	=	GetSystemMetrics(SM_CXSCREEN);
//		int h	=	GetSystemMetrics(SM_CYSCREEN);
//		CRect window;
//		CRect rcB, rcL;
//		GetWindowRect(&window);
//		m_log.GetWindowRect(&rcL);
//		m_log.MoveWindow(0, window.top, rcL.Width(), rcL.Height());
//		//m_bind.GetWindowRect(&rcB);
//		if(m_bind.m_hWndBind)
//		{
//			//::GetWindowRect(m_bind.m_hWndBind, &rcB);
//			//::MoveWindow(m_bind.m_hWndBind, window.right, window.top, w - window.right, rcB.Height(), true);
//		}
//		char buf[128];
//		sprintf(buf, "%d", window.left);
//		::WriteProfileStringA("DWZB", "left", buf);
//		sprintf(buf, "%d", window.top);
//		::WriteProfileStringA("DWZB", "top", buf);
//		sprintf(buf, "%d", rcL.Height());
//		::WriteProfileStringA("DWZB", "logHeight", buf);
//		sprintf(buf, "%d", rcB.Height());
//		::WriteProfileStringA("DWZB", "bindHeight", buf);
//	}
//#endif
	CDialogEx::OnMoving(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}


void CBaiWanGameDlg::OnBnClickedOk()
{
}


void CBaiWanGameDlg::OnBnClickedCancel()
{
	if(MessageBox(L"是否真的要退出？", 0, MB_YESNO) == IDYES)
		CDialogEx::OnCancel();
}


void CBaiWanGameDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
	if(g_bIsInit)
		SaveRect(this, "main");
	// TODO: 在此处添加消息处理程序代码
}
