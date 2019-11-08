/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/

// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "phantom-gc.h"
#include "GCRenderSystem.h"
#include "MainFrm.h"
#include "GCViewManager.h"
#include "GCBitmapMgr.h"
#include "CreateFileDlg.h"
#include "GCViewUIEditor.h"
#include "UIPngMaker.h"
#include "../PhantomEngine/encrypt.h"
#include "DlgSelectDevice.h"
#include "IDEHttp.h"
#include "io.h"
#include "LoginDialog.h"
#include "MyLuaEditor.h"
#include "DlgEditBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern	wchar_t *	GetProjectName();
const char*	GetAppID()
{
	static char s_appid[256];
	CString strAppID = AfxGetApp()->GetProfileStringW(L"AppID", GetProjectName());
	strcpy(s_appid, A(strAppID).buf);
	return s_appid;
}

BOOL										LoadResData(CDynamicArray<char>& rets, const wchar_t* szResID, const wchar_t* szResType, HMODULE hModule)
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
//std::vector<unsigned char> g_authCodeVec;

CMainFrame*	g_pMainFrame	=	0;
CMainFrame*	GetMainFrame()
{
	return g_pMainFrame;
}
VOID MessageLowVersion(int result)
{
	if(result == -5)
	{
		g_pMainFrame->MessageBoxW(Language(L"lowversion", L"您的编辑器版本过低，需要更新到最新版本才能打开这个文件"));
	}
	if(result == -3)
	{
		g_pMainFrame->MessageBoxW(Language(L"lowversion", L"您的文件版本过低，需要使用2.0版本的编辑器打开并保存一次，否则无法继续使用此文件。"));
	}
}
extern BOOL IsUIEditorMode();
extern HWND	GetDeviceWindow();
CDlgEditBox	g_dlgEdit;
extern "C"
{
	BOOL CreatePhantom(int nDiv, int nOS, int* authcode)
	{
		return false;
		//return OnAppCreate(0, nDiv, OSType_Windows, authcode);
	}
	extern	const char*		call_lua(const char* type, const char* params);//调用lua内部的函数oncall
    extern	void    showAd(int x, int y, int type, int bShow){
		call_lua("jifen_score", "100");
		bool b = false;
	}
    extern	void    shopBuy(const char* buyName){}
	extern	void    shopRestore(const char* buyName){}
	void	gotoBuyRemoveAD(int){}
	void	gotoComment(){}
	void	openRank(const char* rank){}
	void	setRankScore(const char* rank, int value){}
	void	setAchievement(const char* achievement, int persent){}
	void	openUrl(const char* url){
		::ShellExecuteA(g_pMainFrame->m_hWnd, "open", url, 0, 0, SW_SHOW);
	}
	void	closeEdit(int bIsCancel)//是否取消编辑
	{
		if(g_dlgEdit.m_hWnd&&g_dlgEdit.IsWindowVisible()){
			CString str;
			g_dlgEdit.m_edit.GetWindowTextW(str);
			Autf8 a(str);
			Phantom::GetDlgManager()->OnEditClose(g_dlgEdit.m_id, bIsCancel, a.buf);
			g_dlgEdit.ShowWindow(SW_HIDE);
		}
	}
	void	openEdit(const char* szDefault, int x, int y, int w, int h, int id)
	{
		if(IsUIEditorMode())return;
		//if(g_dlgEdit.m_hWnd&&g_dlgEdit.IsWindowVisible())
		//	closeEdit(false);
		CRect rc(x, y, x + w, y + h);
		CRect rcClient;
		GetClientRect(GetDeviceWindow(), &rcClient);
		CRect temp(rcClient.left+x, rcClient.top+y, rcClient.left+x+w, rcClient.top+y+h);
		ClientToScreen(GetDeviceWindow(), (LPPOINT)&temp);
		ClientToScreen(GetDeviceWindow(), ((LPPOINT)&temp)+1);
		if(!g_dlgEdit.m_hWnd){
			g_dlgEdit.Create(g_dlgEdit.IDD, CWnd::FromHandle(GetDeviceWindow()));
		}
		g_dlgEdit.MoveWindow(&temp);
		g_dlgEdit.ShowWindow(SW_SHOW);
		g_dlgEdit.m_edit.SetWindowTextW(Wutf8(szDefault).buf);
		g_dlgEdit.m_edit.SetFocus();
		g_dlgEdit.m_id = id;
	}

	const char*	onAlert(const char* message, const char* title, const char* type)
	{
		int nType = MB_OK;
		if(stricmp(type, "yesno") == 0)
			nType = MB_YESNO;
		else if(stricmp(type, "okcancel") == 0)
			nType = MB_OKCANCEL;
		else if(stricmp(type, "yesnocancel") == 0)
			nType = MB_YESNOCANCEL;
		int nRet = g_pMainFrame->MessageBoxW(W(message).buf, W(title).buf, nType);
		if(nRet == IDCANCEL)return "cancel";
		if(nRet == IDYES)return "yes";
		if(nRet == IDNO)return "no";
		return "ok";
	}

    int            DrawTextIOS(const unsigned short* text, int textLength, int fontHeight, int fontStyle, const char* fontName, int*inoutW, int*inoutH, int*outImageW, int*outImageH, unsigned char** ret)
	{
		return 0;
	}
    int            DrawTextIOSA(const char* text, int textLength, int fontHeight, int fontStyle, const char* fontName, int*inoutW, int*inoutH, int*outImageW, int*outImageH, unsigned char** ret)
	{
		return 0;
	}
	void	StrEncode(std::string& ret, const char* val){
		std::vector<unsigned char> rets;
		unsigned char* val2 = (unsigned char*)val;
		int len = strlen(val) + 1;
		for(int i=0;i<len;i++)
			rets.push_back(val2[i]);
		char* bret = new char[rets.size()*2+1];
		HexDecode(bret, &rets.at(0), rets.size());
		ret = bret;
		delete[] bret;
	}
	BOOL	StrDecode(std::string& ret, const char* val){
		std::vector<unsigned char> rets;
		//std::string temp = val;
		HexEncode(rets, val);
		int len = rets.size();
		if(len<=0)return false;
		ret.resize(len+1);
		strcpy((char*)ret.c_str(), (const char*)&rets.at(0));
		return true;
	}
    void    saveConfig(const char* key, const char* value)
	{
		std::string skey,svalue;
		StrEncode(skey, key);
		StrEncode(svalue, value);
		AfxGetApp()->WriteProfileString(L"config", Wutf8(skey.c_str()).buf, Wutf8(svalue.c_str()).buf);
	}
    int     loadConfig(const char* key,char* retvalue)
	{
		std::string skey,svalue;
		StrEncode(skey, key);
		CString str = AfxGetApp()->GetProfileStringW(L"config", Wutf8(skey.c_str()).buf, L"");
		retvalue[0] = 0;
		if(!StrDecode(svalue, Autf8(str).buf))
			return false;
		strcpy(retvalue, svalue.c_str());
		return true;
	}
	const char* GetDeviceName(char*ch)
	{
		strcpy(ch, "Windows");
		return ch;
	}
	void    SetViewOrientation(int ori)
	{
	}
	void	onVibrator(float f){}
};

const wchar_t*	GetLoginUserName()
{
	return L"未登陆";
}

BOOL	GetCopyKeyboard(){
	return HIBYTE(GetKeyState(VK_SHIFT));
}
BOOL	GetSelectKeyboard(){
	return HIBYTE(GetKeyState(VK_CONTROL));
}

VOID	UpdateMainTitle()
{
	if(GetViewManager() && GetViewManager()->GetActiveView())
		GetViewManager()->GetActiveView()->UpdateMainTitle();
	else
		UpdateMainText(L"");
}

extern	const char*	GetDocumentPath();
//CSqliteDB		g_systemDB;
//CSqliteDB*		GetSystemDB()
//{
//	if(g_systemDB.IsCreate())
//		return &g_systemDB;
//	char buf[512];
//	strcpy(buf, GetDocumentPath());
//	strcat(buf, "phantomVD.db");
//	if(!g_systemDB.Create(buf))
//		return 0;
//	return &g_systemDB;
//}
//
//int	g_nAuthMode	=	AuthGUID::AuthFlags_Error;
//int	g_nAuthModeTemp	=	AuthGUID::AuthFlags_Error;

//VOID	GetTitle(wchar_t* text)
//{
//	//if(g_nAuthModeTemp == AuthGUID::AuthFlags_Professional || g_nAuthModeTemp == AuthGUID::AuthFlags_Business)
//	{
//		unsigned char g_text[] = {0xb8,0xb5,0x5f,0x1e,0xdb,0x28,0x34,0xeb,0x98,0x86,0xa0,0x3c,0x9b,0x0c,0xaf,0x93,0xb1,0x74,0x51,0xbc,0xac,0xc6,0x6b,0x7d,0x79,0x66,0x45,0xbf,0x8e,0x03,0xd0,0x09};
//		unsigned char g_password[] = {0x1b,0x6e,0x94,0xb6,0x10,0xfe,0xe3,0x44,0x52,0x63,0x7f,0xc2,0xd5,0x26,0x99,0xbb,0x6a,0x8e,0xfe,0x75,0x7a,0x00,0xbc,0xbb,0xb7,0xb8,0xe2,0x0d,0x21,0xb9,0x78,0xf7};
//		wcscpy(text, W(CTextCrypts(g_password, sizeof(g_password), g_text, sizeof(g_text))).buf);
//	}
//	//else
//	{
//		//unsigned char g_textTitle[] = {0x93,0xe1,0xb2,0xbf,0x4d,0x39,0x7e,0xc8,0x45,0x2e,0xed,0x4a,0x98,0xda,0xab,0x36,0x13,0x6a,0x61,0x4d,0xb0,0xef,0x69,0x4f,0x93,0xd0,0xd6,0x6a,0x4e,0x6f,0xb3,0x2f};
//		//unsigned char g_passwordTitle[] = {0x30,0x3a,0x79,0x17,0xea,0xe8,0xac,0x03,0xe6,0x99,0x3c,0xba,0x3c,0x04,0x75,0xd9,0x39,0x42,0x48,0x67,0xf3,0x34,0x93,0xe0,0x5a,0x07,0x10,0xa4,0x90,0xc7,0x4d,0x6a};
//		//wcscpy(text, W(CTextCrypts(g_passwordTitle, sizeof(g_passwordTitle), g_textTitle, sizeof(g_textTitle))).buf);
//	}
//}
//
//VOID	ShowFreeMessage(unsigned char* text, int textLen, unsigned char* textPass, int textPassLen)
//{
//	wchar_t ret[512];
//	GetCryptText(ret, text, textLen, textPass, textPassLen);
//	ShowMainMessage(ret);
//}

wchar_t g_szTitle[128]={0};
VOID	SetMainTitle(const char* s){
	wcscpy(g_szTitle, W(s).buf);
	UpdateMainTitle();
}
VOID	UpdateMainText(const wchar_t* szEditorName, const wchar_t* szFileName, BOOL bIsModifyed)
{
	wchar_t szTitle[512];
	wchar_t szT[512];
	if (GetLanguagesPtr()->m_language <= Languages_Chiness)
	{
		wcscpy(szT, L"幻影游戏引擎 V2.0");
	}
	else
	{
		wcscpy(szT, L"Phantom Game Engine V2.0");
	}
	const wchar_t* wwt = szT;
	if (g_szTitle[0])
		wwt = g_szTitle;
	swprintf(szTitle, L"%s - {%s} - %s[%s]%s", wwt, GetProjectName(), szEditorName, szFileName, bIsModifyed ? L"*" : L"");
	g_pMainFrame->SetWindowText(szTitle);
	g_pMainFrame->SetTitle(szTitle);
}
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 1;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;
//
//CDropTarget::CDropTarget()
//{
//	m_ulRef	=	1;
//}
//CDropTarget::~CDropTarget()
//{
//}
//HRESULT STDMETHODCALLTYPE CDropTarget::QueryInterface( REFIID riid, __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
//{
//	return E_FAIL;
//}
//ULONG STDMETHODCALLTYPE CDropTarget::AddRef( void)
//{
//	return (m_ulRef++);
//}
//ULONG STDMETHODCALLTYPE CDropTarget::Release( void)
//{
//	if(m_ulRef > 0)
//		return (--m_ulRef);
//	return 0;
//}
//HRESULT STDMETHODCALLTYPE CDropTarget::DragEnter( __RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect)
//{
//	return S_OK;
//}
//HRESULT STDMETHODCALLTYPE CDropTarget::DragOver( DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect)
//{
//	return S_OK;
//}
//HRESULT STDMETHODCALLTYPE CDropTarget::DragLeave( void)
//{
//	return S_OK;
//}
//HRESULT STDMETHODCALLTYPE CDropTarget::Drop(  __RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect)
//{
//	return S_OK;
//}

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CMainFrame::OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CMainFrame::OnUpdateFileSaveAs)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_NEW, &CMainFrame::OnFileNew)
	ON_WM_DROPFILES()
	ON_WM_COPYDATA()
	ON_UPDATE_COMMAND_UI(IDM_INSERT_STATIC, &CMainFrame::OnUpdateInsertStatic)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_FORMATSTATIC, &CMainFrame::OnUpdateInsertFormatstatic)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_IMEEDIT, &CMainFrame::OnUpdateInsertImeedit)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_FORMATEDIT, &CMainFrame::OnUpdateInsertFormatedit)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_CHECKBOX, &CMainFrame::OnUpdateInsertCheckbox)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_COMBOBOX, &CMainFrame::OnUpdateInsertCombobox)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_SLIDER, &CMainFrame::OnUpdateInsertSlider)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_LISTBOX, &CMainFrame::OnUpdateInsertListbox)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_SCROLLBAR, &CMainFrame::OnUpdateInsertScrollbar)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_PROGRESS, &CMainFrame::OnUpdateInsertProgress)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_LISTCTRL, &CMainFrame::OnUpdateInsertListctrl)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_BUTTON, &CMainFrame::OnUpdateInsertButton)
	//
	ON_COMMAND(IDM_INSERT_FORMATSTATIC, &CMainFrame::OnInsertFormatstatic)
	ON_COMMAND(IDM_INSERT_IMEEDIT, &CMainFrame::OnInsertImeedit)
	ON_COMMAND(IDM_INSERT_FORMATEDIT, &CMainFrame::OnInsertFormatedit)
	ON_COMMAND(IDM_INSERT_CHECKBOX, &CMainFrame::OnInsertCheckbox)
	ON_COMMAND(IDM_INSERT_COMBOBOX, &CMainFrame::OnInsertCombobox)
	ON_COMMAND(IDM_INSERT_SLIDER, &CMainFrame::OnInsertSlider)
	ON_COMMAND(IDM_INSERT_LISTBOX, &CMainFrame::OnInsertListbox)
	ON_COMMAND(IDM_INSERT_SCROLLBAR, &CMainFrame::OnInsertScrollbar)
	ON_COMMAND(IDM_INSERT_PROGRESS, &CMainFrame::OnInsertProgress)
	ON_COMMAND(IDM_INSERT_LISTCTRL, &CMainFrame::OnInsertListctrl)
	ON_COMMAND(IDM_INSERT_BUTTON, &CMainFrame::OnInsertButton)
	ON_COMMAND(IDM_INSERT_STATIC, &CMainFrame::OnInsertStatic)
	ON_COMMAND(ID_REFRESH_FILES, &CMainFrame::OnRefreshFiles)
	ON_COMMAND(ID_UI_PNG_MAKER, &CMainFrame::OnUiPngMaker)
	ON_COMMAND(IDC_EXPORT_UI, &CMainFrame::OnExportUi)
	ON_COMMAND(ID_SET_PROJECT, &CMainFrame::OnSetProject)
	ON_COMMAND(ID_USER_INFO, &CMainFrame::OnUserInfo)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SCRIPT, &CMainFrame::OnUpdateShowScript)
	ON_COMMAND(ID_SHOW_SCRIPT, &CMainFrame::OnShowScript)
	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_UPDATE_COMMAND_UI(ID_BUY_RESOURCE, &CMainFrame::OnUpdateBuyResource)
	ON_COMMAND(ID_BUY_RESOURCE, &CMainFrame::OnBuyResource)
	ON_COMMAND(ID_CONVERT_ALL_FBX, &CMainFrame::OnConvertAllFbx)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	g_pMainFrame	=	this;
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
	//
	m_pViewManager		=	0;
	m_pBitmapManager	=	0;
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	return CMDIFrameWndEx::OnWndMsg(message, wParam, lParam, pResult);
}
BOOL CMainFrame::OnMenuButtonToolHitTest(CMFCToolBarButton* button, TOOLINFO* info)
{
	return false;
}

BOOL CMainFrame::GetToolbarButtonToolTipText(CMFCToolBarButton* button, CString& str)
{
	return false;
}
extern	VOID	InitDevice(const RECT& rc);
extern	VOID	SetParentHWND(HWND hWndParent);
extern	VOID	InitWindow();
extern	BOOL	InitPhantomEngine3D();
extern	VOID	OnDeviceInit();
extern	VOID	SetMainWindow(HWND hWnd);
extern "C"{
extern void MyCreatePhantom(int nDiv, int nOS);
};

bool	g_bIsStartup	=	false;
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pViewManager		=	new GCViewManager();
	m_pBitmapManager	=	new GCBitmapMgr();
	SetMainWindow(m_hWnd);
	BOOL bNameValid;
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 其他可用样式...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
	mdiTabParams.m_bTabIcons = TRUE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = FALSE;    // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
	mdiTabParams.m_bDocumentMenu = TRUE; // 在选项卡区域的右边缘启用文档菜单
	mdiTabParams.m_bTabCustomTooltips	=	TRUE;
	EnableMDITabbedGroups(TRUE, mdiTabParams);
	//m_wndRibbonBar.Create(this);
	//m_wndRibbonBar.LoadFromResource(IDR_RIBBON);
	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}
	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME_256))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	int nToolBarID	=	uiFirstUserToolBarId + 1;
	if (!m_wndToolBarUI.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1,1,1,1), nToolBarID++) ||
		!m_wndToolBarUI.LoadToolBar(IDR_UI_INSERT))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	if (!m_wndToolBarScene.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1,1,1,1), nToolBarID++) ||
		!m_wndToolBarScene.LoadToolBar(IDR_SCENE2D_INSERT))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	if (!m_wndToolBarAlign.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1,1,1,1), nToolBarID++) ||
		!m_wndToolBarAlign.LoadToolBar(IDR_ALIGN_BAR))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	//if (!m_wndToolBarProj.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1,1,1,1), nToolBarID++) ||
	//	!m_wndToolBarProj.LoadToolBar(IDR_GAME_RUNTIME))
	//{
	//	TRACE0("未能创建工具栏\n");
	//	return -1;      // 未能创建
	//}
	if (!m_wndToolBarScene3D.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1,1,1,1), nToolBarID++) ||
		!m_wndToolBarScene3D.LoadToolBar(IDR_SCENE3D_EDITOR))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	if (!m_wndToolBarReport.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1,1,1,1), nToolBarID++) ||
		!m_wndToolBarReport.LoadToolBar(IDR_REPORT_TOOLBAR))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	m_wndToolBar.SetWindowText(Language(L"标准"));
	m_wndToolBarUI.SetWindowText(Language(L"界面"));
	m_wndToolBarScene.SetWindowText(Language(L"场景"));
	m_wndToolBarAlign.SetWindowText(Language(L"对齐"));
	//m_wndToolBarProj.SetWindowText(Language(L"项目"));
	m_wndToolBarScene3D.SetWindowText(Language(L"3D场景"));
	m_wndToolBarReport.SetWindowText(Language(L"发布"));
	//
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, Language(L"自定义"));
	m_wndToolBarUI.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, Language(L"自定义"));
	m_wndToolBarScene.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, Language(L"自定义"));
	m_wndToolBarAlign.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, Language(L"自定义"));
	//m_wndToolBarProj.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, Language(L"自定义"));
	m_wndToolBarScene3D.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, Language(L"自定义"));
	m_wndToolBarReport.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, Language(L"自定义"));
	// 允许用户定义的工具栏操作:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	m_wndToolBar.EnableToolTips(TRUE);
	m_wndToolBarUI.EnableToolTips(TRUE);
	m_wndToolBarScene.EnableToolTips(TRUE);
	m_wndToolBarAlign.EnableToolTips(TRUE);
	//m_wndToolBarProj.EnableToolTips(TRUE);
	m_wndToolBarScene3D.EnableToolTips(TRUE);
	m_wndToolBarReport.EnableToolTips(TRUE);
	//
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	//m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, L"状态1", TRUE), L"状态1");
	//m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, L"状态2", TRUE), L"状态2");
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarUI.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarScene.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarAlign.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBarProj.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarScene3D.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarReport.EnableDocking(CBRS_ALIGN_ANY);
	//
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);
	DockPane(&m_wndToolBarUI);
	DockPane(&m_wndToolBarScene);
	DockPane(&m_wndToolBarAlign);
	//DockPane(&m_wndToolBarProj);
	DockPane(&m_wndToolBarScene3D);
	DockPane(&m_wndToolBarReport);
	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 加载菜单项图像(不在任何标准工具栏上):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndFileBrowser.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	DockPane(&m_wndFileBrowser);
	CDockablePane* pTabbedBar = NULL;
	m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	//m_wndClassView.AttachToTabWnd(&m_wndFileBrowser, DM_SHOW, TRUE, &pTabbedBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);
	//m_wndRuntimeProperties.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndRuntimeProperties);

	m_wndPropertiesDialog.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndPropertiesDialog);
	m_wndOutput.AttachToTabWnd(&m_wndPropertiesDialog, DM_SHOW, TRUE, &pTabbedBar);
	//
	//m_wndDlg2.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndDlg2);
	//m_wndOutput.AttachToTabWnd(&m_wndDlg2, DM_SHOW, TRUE, &pTabbedBar);
	//m_uiProperties.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_uiProperties);
	//m_wndProperties.AttachToTabWnd(&m_uiProperties, DM_SHOW, TRUE, &pTabbedBar);
	//
	// 启用增强的窗口管理对话框
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, Language(L"自定义"), ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 加载用户定义的工具栏图像
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}
	//m_screenEffect.Create(m_screenEffect.IDD);
	//m_screenEffect.ShowWindow(SW_HIDE);

	m_bShowMessage	=	false;

	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(IDM_UI_INSERT_NULL);
	lstBasicCommands.AddTail(IDR_UI_INSERT);
	lstBasicCommands.AddTail(ID_SHOW_UI_GRID);
	lstBasicCommands.AddTail(ID_TAB_SET);
	lstBasicCommands.AddTail(IDM_INSERT_STATIC);
	lstBasicCommands.AddTail(IDM_INSERT_FORMATSTATIC);
	lstBasicCommands.AddTail(IDM_INSERT_IMEEDIT);
	lstBasicCommands.AddTail(IDM_INSERT_FORMATEDIT);
	lstBasicCommands.AddTail(IDM_INSERT_CHECKBOX);
	lstBasicCommands.AddTail(IDM_INSERT_COMBOBOX);
	lstBasicCommands.AddTail(IDM_INSERT_SLIDER);
	lstBasicCommands.AddTail(IDM_INSERT_LISTBOX);
	lstBasicCommands.AddTail(IDM_INSERT_SCROLLBAR);
	lstBasicCommands.AddTail(IDM_INSERT_PROGRESS);
	lstBasicCommands.AddTail(IDM_INSERT_LISTCTRL);
	lstBasicCommands.AddTail(IDM_INSERT_BUTTON);
	//
	//if(g_authCode.size() > 256)
	//	g_authCode.clear();
	//
	lstBasicCommands.AddTail(ID_MESH_SHOW_ALL);
	lstBasicCommands.AddTail(IDM_EDIT_UI_MODE);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	// 将文档名和应用程序名称在窗口标题栏上的顺序进行交换。这
	// 将改进任务栏的可用性，因为显示的文档名带有缩略图。
	ModifyStyle(0, FWS_PREFIXTITLE);

	if(!m_alignDlg.Create(m_alignDlg.IDD, this))
		return -1;
	m_alignDlg.ShowWindow(SW_HIDE);
	if(!m_alignDlg3D.Create(m_alignDlg3D.IDD, this))
		return -1;
	m_alignDlg3D.ShowWindow(SW_HIDE);
	//
	//if((g_authCode.size() % 2) != 0)
	//	g_authCode.clear();
	//HexEncode(g_authCodeVec, g_authCode);
	//渲染系统初始化
	SetProjectPath();
	//if(!InitRenderSystem(m_hWnd))
	//{
	//	TRACE0("不能初始化渲染系统\n");
	//	return -1;
	//}
	SetParentHWND(m_hWnd);
	InitWindow();
	InitPhantomEngine3D();
	MyCreatePhantom(1,0);
	RECT rc;
	GetClientRect(&rc);
	InitDevice(rc);
	OnDeviceInit();
	//{
	//	DefineAuth_1();
	//	try{
	//	if(!(g_nAuthMode = CheckAuth_1(readAuth)))
	//	{
	//		WriteRegValue("ac19", "");
	//		g_authCodeVec.clear();
	//	}
	//	}
	//	catch(...)
	//	{
	//		WriteRegValue("ac19", "");
	//		g_authCodeVec.clear();
	//	}
	//}
	//GetLuaCodeManager()->LoadContent();
	SetTimer(168, 10, 0);
	SetTimer(1, 100, 0);
	//
	g_bIsStartup	=	true;
	GetLanguagesPtr()->SetChildsText(m_hWnd);
	for(int i=0;i<m_wndMenuBar.GetAllButtons().GetSize();i++)
	{
		CString str	=	m_wndMenuBar.GetButtonText(i);
		m_wndMenuBar.SetButtonText(i, GetLanguagesPtr()->GetText(str, str));
	}
	SetRenderWindowSize(GetConfigInt("screenX", 800), GetConfigInt("screenY", 600));
	this->m_dlgLog.Create(m_dlgLog.IDD, this);
	this->m_dlgFind.Create(IDD_FIND_TEXTs, this);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 创建类视图
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(L"脚本编辑器", this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“类视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建文件视图
	if (!m_wndFileView.Create(L"操作视图", this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“操作视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建文件视图
	if (!m_wndFileBrowser.Create(L"文件浏览窗口", this, CRect(0, 0, 200, 200), TRUE, 135, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“操作视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建输出窗口
	if (!m_wndOutput.Create(Language(L"输出"), this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}

	// 创建属性窗口
	if (!m_wndProperties.Create(Language(L"属性"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; // 未能创建
	}
	//// 创建属性窗口
	//if (!m_wndRuntimeProperties.Create(Language(L"属性"), this, CRect(0, 0, 200, 200), TRUE, IDS_PROPERTIES_RUNTIME_WND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("未能创建“属性”窗口\n");
	//	return FALSE; // 未能创建
	//}
	// 创建属性窗口
	if (!m_wndPropertiesDialog.Create(Language(L"属性对话框"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESDLGWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; // 未能创建
	}
	// 创建属性窗口
	//if (!m_wndDlg2.Create(Language(L"属性对话框2"), this, CRect(0, 0, 200, 200), TRUE, 350, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		//TRACE0("未能创建“属性”窗口\n");
		//return FALSE; // 未能创建
	}
	//if (!m_uiProperties.Create(Language(L"UI编辑框"), this, CRect(0, 0, 200, 200), TRUE, 360, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		//TRACE0("未能创建“属性”窗口\n");
		//return FALSE; // 未能创建
	}
	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);
	m_wndFileBrowser.SetIcon(hFileViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_CLASS_VIEW_HC), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);
	m_wndPropertiesDialog.SetIcon(hPropertiesBarIcon, FALSE);
	//m_wndDlg2.SetIcon(hPropertiesBarIcon, FALSE);
	//
	//HICON hUIProperties	= (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_UI_FILE_ICON), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//m_uiProperties.SetIcon(hUIProperties, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	//pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, Language(L"自定义"));
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;
	}
	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			//pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, Language(L"自定义"));
		}
	}
	//
	//color4 color(GetConfigInt("BgColor"));
	//CMFCRibbonBaseElement* cmd	=	m_wndRibbonBar.FindByID(IDM_BG_COLOR);
	//if(cmd)
	//	static_cast<CMFCRibbonColorButton*>(cmd)->SetColor(color.getRGB());

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
}


void CMainFrame::OnDestroy()
{
	GetViewManager()->DestroyAllDocuments();
	if(m_pViewManager)
	{
		delete m_pViewManager;
		delete m_pBitmapManager;
		//
		m_pViewManager		=	0;
		m_pBitmapManager	=	0;
	}
	DestroyRenderSystem();
	CMDIFrameWndEx::OnDestroy();
}


void CMainFrame::ActivateFrame(int nCmdShow)
{
	// TODO: 在此添加专用代码和/或调用基类

	CMDIFrameWndEx::ActivateFrame(nCmdShow);
}


void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMDIFrameWndEx::OnLButtonDown(nFlags, point);
}
extern	BOOL	RunGame();
extern	const char*	GetAuthKey();
VOID	RequestRuntime()//请求在机器中运行
{
	//if(!GetAuthKey()[0])
	//{
	//	GetMainFrame()->MessageBox(Language(L"未初始化调试，再等一会初始化完毕之后再尝试，或者联系幻影官方技术人员。"));
	//	return;
	//}
	if(!RunGame())
	{
		//DlgSelectDevice dlg;//选择设备
		//if(dlg.DoModal()==IDCANCEL)
		//	return;
		//RunGame();
	}
	//GetMainFrame()->m_authclient.Connect(
}
extern VOID	OnIdleRender();
void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		static BOOL bD = false;
		if(!bD){
			bD = true;
			bD = false;
		}
	}
	if(nIDEvent == 168)
	{
		if(::IsWindowVisible(GetDeviceWindow())&&!IsIconic())
			OnIdleRender();
	}
	CMDIFrameWndEx::OnTimer(nIDEvent);
}


void CMainFrame::OnFileOpen()
{
	SetProjectPath();
	CFileDialog dlg(TRUE, 0, 0, OFN_EXPLORER|OFN_ENABLESIZING|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT, GetPhantomFilters(), this);
	W init(GetProjectPath());
	dlg.m_pOFN->lpstrInitialDir	=	init;
	std::wstring temp;
	temp.resize(1024000);
	dlg.m_pOFN->lpstrFile	=	(wchar_t*)temp.c_str();
	dlg.m_pOFN->lpstrFile[0]	=	0;
	dlg.m_pOFN->nMaxFile	=	temp.size();
	if(dlg.DoModal() != IDOK)
		return;
	SetProjectPath();
	//
	POSITION pos	=	dlg.GetStartPosition();
	int openCount	=	0;
	while(pos)
	{
		CString str = dlg.GetNextPathName(pos);
		wchar_t wext[128];
		_wsplitpath(str, 0, 0, 0, wext);
		//if(wcsicmp(wext, L".lua")==0){
		//	CodeEditor* editor = GetLuaEditor()->Open(A(str));
		//	GetLuaEditor()->ShowWindow(SW_SHOW);
		//}
		//else{
		AfxGetApp()->OpenDocumentFile(str);
		//}
	}
	UpdateMainTitle();
}

void CMainFrame::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL);
}

void CMainFrame::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWndEx::OnSize(nType, cx, cy);
	UpdateMainTitle();
}

extern	VOID	CreateNewFile(int nType);

void CMainFrame::OnFileNew()
{
	CCreateFileDlg dlg(this);
	if(dlg.DoModal() != IDOK)
		return;
	if(dlg.m_createType == FileViewerType_GameProject)
	{
		AfxGetApp()->OpenDocumentFile(dlg.m_strGameProj);
	}
	//else if(dlg.m_createType == FileViewerType_GCSceneEditor3D)
	//{
	//	AfxGetApp()->OpenDocumentFile(dlg.m_strScene3D);
	//}
	else
		CreateNewFile(dlg.m_createType);
	UpdateMainTitle();
}


void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	CMDIFrameWndEx::OnDropFiles(hDropInfo);
}


BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(pCopyDataStruct->dwData)
	{
	case WM_DDE_FILEOPEN:
		{
			W w((const char*)pCopyDataStruct->lpData);
			if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->OnDragDropFile(w))
			{
			}
			else
			{
				AfxGetApp()->OpenDocumentFile(w);
				UpdateMainTitle();
			}
			::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			//
			ShowWindow(SW_RESTORE);
			::BringWindowToTop(m_hWnd);
			::SetForegroundWindow(m_hWnd);
			::SetActiveWindow(m_hWnd);
		}
		break;
	}
	return CMDIFrameWndEx::OnCopyData(pWnd, pCopyDataStruct);
}


VOID	UI_SetEnableUI(CCmdUI *pCmdUI, INT nType)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		if(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->GetEditorMode() == GCViewUIEditor::EditorMode_Default)
		{
			pCmdUI->Enable(true);
			pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_nEditorFlags == nType);
		}
	}
}

VOID	UI_SetInsertUI(INT nType)
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->SetEditorFlags(nType);
		UpdateRobbinNow();
	}
}

void CMainFrame::OnUpdateInsertFormatstatic(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_VIEW_FORMAT_TEXT);}
void CMainFrame::OnUpdateInsertImeedit(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_IMEEDITBOX);}
void CMainFrame::OnUpdateInsertFormatedit(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_FORMAT_EDITBOX);}
void CMainFrame::OnUpdateInsertCheckbox(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_CHECKBOX);}
void CMainFrame::OnUpdateInsertCombobox(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_COMBOBOX);}
void CMainFrame::OnUpdateInsertSlider(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_SLIDER);}
void CMainFrame::OnUpdateInsertListbox(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_LISTBOX);}
void CMainFrame::OnUpdateInsertScrollbar(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_SCROLLBAR);}
void CMainFrame::OnUpdateInsertProgress(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_PROGRESS);}
void CMainFrame::OnUpdateInsertListctrl(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_LISTCTRL3D);}
void CMainFrame::OnUpdateInsertButton(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_BUTTON);}
void CMainFrame::OnUpdateInsertStatic(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_STATIC);}
void CMainFrame::OnInsertFormatstatic(){UI_SetInsertUI(UIControlType_VIEW_FORMAT_TEXT);}
void CMainFrame::OnInsertImeedit(){MessageBox(L"暂时不支持输入框控件");}//UI_SetInsertUI(UIControlType_IMEEDITBOX);}
void CMainFrame::OnInsertFormatedit(){UI_SetInsertUI(UIControlType_FORMAT_EDITBOX);}
void CMainFrame::OnInsertCheckbox(){UI_SetInsertUI(UIControlType_CHECKBOX);}
void CMainFrame::OnInsertCombobox(){UI_SetInsertUI(UIControlType_COMBOBOX);}
void CMainFrame::OnInsertSlider(){UI_SetInsertUI(UIControlType_SLIDER);}
void CMainFrame::OnInsertListbox(){UI_SetInsertUI(UIControlType_LISTBOX);}
void CMainFrame::OnInsertScrollbar(){UI_SetInsertUI(UIControlType_SCROLLBAR);}
void CMainFrame::OnInsertProgress(){UI_SetInsertUI(UIControlType_PROGRESS);}
void CMainFrame::OnInsertListctrl(){UI_SetInsertUI(UIControlType_LISTCTRL3D);}
void CMainFrame::OnInsertButton(){UI_SetInsertUI(UIControlType_BUTTON);}
void CMainFrame::OnInsertStatic(){UI_SetInsertUI(UIControlType_STATIC);}

extern	void AFXAPI AfxCancelModes(HWND hWndRcvr);

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	//if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_NCLBUTTONDOWN)
	//	AfxCancelModes(pMsg->hwnd);    // filter clicks
	//if (CWnd::PreTranslateMessage(pMsg))
	//	return TRUE;
	//CMDIChildWnd* pActiveChild = MDIGetActive();
	//if (pActiveChild != NULL && pActiveChild->PreTranslateMessage(pMsg))
	//	return TRUE;
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST && GetViewManager()->GetActiveView() && !GetViewManager()->GetActiveView()->IsCanHotkey())//::GetFocus() == GetDeviceWindow())
		return false;
	//{
	//	if (m_hAccelTable != NULL && ::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
	//	{
	//		return TRUE;
	//	}
	//	if (GetActiveView() == NULL)
	//	{
	//		if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	//		{
	//			// the MDICLIENT window may translate it
	//			if (::TranslateMDISysAccel(m_hWndMDIClient, pMsg))
	//				return TRUE;
	//		}
	//	}
	//}
	//return FALSE;
	return CMDIFrameWndEx::PreTranslateMessage(pMsg);
}


void CMainFrame::OnRefreshFiles()
{
	this->m_wndFileBrowser.Refresh(TRUE);
}


void CMainFrame::OnUiPngMaker()
{
	CUIPngMaker mk(this);
	mk.DoModal();
}
extern	BOOL	GetRuntimeAuthGUID(Phantom::GenuineGUID* ret);
extern	VOID	SetMainLua(const char* file);
BOOL g_bIsInit = false;
std::string		g_szAppCode;
extern VOID		md5_2(const char* str, unsigned char* ret);
#define	USE_LOCAL_AUTH
BOOL	GetRuntimeAuthGUID(Phantom::GenuineGUID* ret)
{
#ifdef USE_LOCAL_AUTH
	memset(ret, 0, sizeof(Phantom::GenuineGUID));
	return true;
#endif
	if(g_szAppCode.size()==0)return false;
	std::vector<unsigned char>	rets;
	HexEncode(rets, g_szAppCode.c_str());
	assert(rets.size() == sizeof(Phantom::GenuineGUID));
	memcpy(ret, &rets.at(0), sizeof(Phantom::GenuineGUID));
	int* n = (int*)ret;
	char buf[4096];
	char temp[128];
	buf[0] = 0;
	for(int i=0;i<128;i++){
		sprintf(temp, "%d", n[i]);
		if(i>0)
			strcat(buf, ",");
		strcat(buf, temp);
	}
	//unsigned char buf[32];
	//md5_2(GetAppID(), buf);
	//unsigned char* r = (unsigned char*)ret;
	//for(int i=0;i<sizeof(Phantom::GenuineGUID);i++){
	//	r[i]^=buf[i%16];
	//}
	return true;
}

VOID	ReportFiles()
{
	GetViewManager()->SaveAll();
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	GetMainFrame()->m_dlgLog.ResetServer();
	//obj["key"] = GetHttpKey();
	//obj["appid"] = "ZhaoDexian";//Autf8(strAppID).buf;
	//Request("pvd_appauth", obj, OnUserAppAuth, false);
	if(g_szAppCode.size()==0)
		return;
	//在此处md5加密从php获取加密密码，然后手机端...
	//RequestAuth();
	InitReport();
	PMyFileList* files = enum_files(GetProjectPath(), 1000, 0, 0);
	if(!files)return;
	int len = strlen(GetProjectPath());
	std::vector<Phantom::GameFileInfo> names;
	for(int i=0;i<files->vFiles.size();i++)
	{
		const char* szFile = files->vFiles[i].szFileName + len;
		char ret[256];
		strcpy(ret, szFile);
		if(ReportLockFile(szFile, ret))
		{
			HANDLE hFile=::CreateFileA(files->vFiles[i].szFileName, GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			DWORD dwHigh = 0;
			DWORD dwSize = ::GetFileSize(hFile, &dwHigh);
			FILETIME c, l, w;
			::GetFileTime(hFile, &c, &l, &w);
			CloseHandle(hFile);
			//info.w = w;
			FileName f = ret;
			f.setIsFile();
			Phantom::GameFileInfo fi;
			strcpy(fi.file, f.t);
			fi.low = w.dwLowDateTime;
			fi.high = w.dwHighDateTime;
			names.push_back(fi);
		}
	}
	destroyFileList(files);
	SaveReport();
	//
	RequestRuntime();//请求在机器中运行
}
extern VOID	ClearDeviceIp();
void CMainFrame::OnExportUi()
{
	ClearDeviceIp();
	ReportFiles();
}

extern	const wchar_t*	GetExecutePath();
void CMainFrame::OnSetProject()
{
}
void CMainFrame::OnUserInfo()
{
}


void CMainFrame::OnUpdateShowScript(CCmdUI *pCmdUI)
{
}


void CMainFrame::OnShowScript()
{
}


void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CMDIFrameWndEx::OnActivate(nState, pWndOther, bMinimized);
	if(GetViewManager()->GetActiveView())
		GetViewManager()->GetActiveView()->OnSetFocus();

	// TODO: 在此处添加消息处理程序代码
}


void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CMDIFrameWndEx::OnActivateApp(bActive, dwThreadID);
	// TODO: 在此处添加消息处理程序代码
}


void CMainFrame::OnUpdateBuyResource(CCmdUI *pCmdUI)
{
}


void CMainFrame::OnBuyResource()
{
}
void CMainFrame::OnConvertAllFbx()
{
}
