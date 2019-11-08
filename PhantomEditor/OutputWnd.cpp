/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/

#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "GCMFCHelper.h"
#include "GCLanguage.h"
#include "GCBitmapMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar
//
//INT			GetPlotTypeIcon(INT nType)
//{
//	int nIcon	=	0;
//	switch(nType)
//	{
//	case PlotType_Drama:
//		nIcon	=	14;
//		break;
//	case PlotType_Chapter:
//		nIcon	=	12;
//		break;
//	case PlotType_Director:
//		nIcon	=	5;
//		break;
//	case PlotType_Module:
//		nIcon	=	3;
//		break;
//	case PlotType_Action:
//		nIcon	=	9;
//		break;
//	case PlotType_Event:
//		nIcon	=	6;
//		break;
//	case PlotType_Properties:
//		nIcon	=	29;
//		break;
//	case PlotType_Variant:
//		nIcon	=	27;
//		break;
//	case PlotType_PlotCall:
//		nIcon	=	10;
//		break;
//	case PlotType_ElementUI:
//		nIcon	=	24;
//		break;
//	case PlotType_Element2DScene:
//		nIcon	=	26;
//		break;
//	case PlotType_ElementMusic:
//		nIcon	=	18;
//		break;
//	case PlotType_Element3DScene:
//		nIcon	=	15;
//		break;
//	case PlotType_Element3DMesh:
//		nIcon	=	1;
//		break;
//	case PlotType_ElementData:
//		nIcon	=	23;
//		break;
//	case PlotType_ElementCursor:
//		nIcon	=	25;
//		break;
//	case PlotType_ElementCSV:
//		nIcon	=	21;
//		break;
//	case PlotType_Script:
//		nIcon	=	34;
//		break;
//	case PlotType_EventCall:
//		nIcon	=	35;
//		break;
//	}
//	return nIcon;
//}
//
//BOOL		g_bOutputDebugText	=	TRUE;
//VOID		OutputDebugText(INT nType, CPlotSystemBaseRun* run, const wchar_t* szText, ...)
//{
//	wchar_t wbuf[4096];
//	wchar_t szBuffer[4096];
//	if(!g_bOutputDebugText)
//		return;
//	if(!run && GetActiveProject())
//		run	=	GetActiveProject()->GetCurrentState();
//	const wchar_t* szName	=	run ? GetPlotName(run->GetPlotType(), run->GetSystemBasePtr(), 0) : L"";
//	_vsnwprintf(szBuffer,4096,szText,(va_list)(&szText+1));
//	swprintf(wbuf, L"{%s}%s", szName, szBuffer);
//	COLORREF textColor	=	0;
//	if(wcsnicmp(szBuffer, L"Error", 5) == 0)
//		textColor	=	RGB(255,0,0);
//	GetMainFrame()->m_wndOutput.AddMessage(GetPlotTypeIcon(nType), wbuf, run ? run->GetID() : 0, textColor);
//}
//VOID		OutputDebugTextSys(CPlotSystemBaseRun* run, const wchar_t* szText, ...)
//{
//	wchar_t wbuf[4096];
//	wchar_t szBuffer[4096];
//	if(!run && GetActiveProject())
//		run	=	GetActiveProject()->GetCurrentState();
//	const wchar_t* szName	=	run ? GetPlotName(run->GetPlotType(), run->GetSystemBasePtr(), 0) : L"";
//	_vsnwprintf(szBuffer,4096,szText,(va_list)(&szText+1));
//	swprintf(wbuf, L"{%s}%s", szName, szBuffer);
//	COLORREF textColor	=	0;
//	if(wcsnicmp(szBuffer, L"Error", 5) == 0)
//		textColor	=	RGB(255,0,0);
//	//
//	GetMainFrame()->m_wndOutput.AddMessage(GetPlotTypeIcon(run ? run->GetPlotType() : 0), wbuf, run ? run->GetID() : 0, textColor);
//}
//
//std::string	g_luaOutputText;
//std::string	g_luaOutputErrorText;
//extern "C"
//{
//void	__stdcall my_print(const char* szText)
//{
//	if(szText[0] == '\n' && szText[1] == 0)
//	{
//		CPlotSystemBaseRun* run = 0;
//		if(GetActiveProject())
//			run	=	GetActiveProject()->GetCurrentState();
//		//
//		wchar_t wbuf[4096];
//		const wchar_t* szName	=	run ? GetPlotName(run->GetPlotType(), run->GetSystemBasePtr(), 0) : L"";
//		swprintf(wbuf, L"{%s}print: %s", szName, W(g_luaOutputText.c_str()).buf);
//		GetMainFrame()->m_wndOutput.AddMessage(GetPlotTypeIcon(PlotType_Script), wbuf, run ? run->GetID() : 0, 0);
//		//
//		g_luaOutputText	=	"";
//	}
//	else
//	{
//		g_luaOutputText	+=	szText;
//	}
//}
//void	__stdcall my_print_error(const char* szText)
//{
//	if(szText[0] == '\n' && szText[1] == 0)
//	{
//		CPlotSystemBaseRun* run = 0;
//		if(GetActiveProject())
//			run	=	GetActiveProject()->GetCurrentState();
//		//
//		wchar_t wbuf[4096];
//		const wchar_t* szName	=	run ? GetPlotName(run->GetPlotType(), run->GetSystemBasePtr(), 0) : L"";
//		swprintf(wbuf, L"{%s}error: %s", szName, W(g_luaOutputErrorText.c_str()).buf);
//		GetMainFrame()->m_wndOutput.AddMessage(GetPlotTypeIcon(PlotType_Script), wbuf, run ? run->GetID() : 0, RGB(255, 0, 0));
//		//
//		g_luaOutputText	=	"";
//	}
//	else
//	{
//		g_luaOutputErrorText	+=	szText;
//	}
//}
//}
COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

VOID			COutputWnd::AddMessage(INT nIcon, const wchar_t* szText, INT id, COLORREF textColor)
{
	wchar_t wbuf[4096];
	//SYSTEMTIME sysTime;
	//GetLocalTime(&sysTime);
	//swprintf(wbuf, L"%02d:%02d:%02d[%d]%s", sysTime.wHour, sysTime.wMinute, sysTime.wSecond, m_wndOpOutput.GetCount() + 1, szText);
	swprintf(wbuf, L"[%d]%s", m_wndOpOutput.GetCount() + 1, szText);
	int nIndex	=	m_wndOpOutput.AddString(wbuf, nIcon, RGB(255,255,255), textColor);
	m_wndOpOutput.SetItemData(nIndex, id);
	if(::GetFocus() != m_wndOpOutput.m_hWnd)
	{
		m_wndOpOutput.SetTopIndex(nIndex);
		m_wndOpOutput.SetCurSel(nIndex);
		//
		CString str;
		m_wndOpOutput.GetText(nIndex, str);
		m_wndErrorText.SetWindowTextW(str);
	}
}
VOID			COutputWnd::ClearMessage()
{
	m_wndOpOutput.ResetContent();
}

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	GetClientRect(&rectDummy);
	this->m_wndOpOutput.SetIconRes(IDB_GAME_VIEWER);
	//// 创建选项卡窗口:
	//if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	//{
	//	TRACE0("未能创建输出选项卡窗口\n");
	//	return -1;      // 未能创建
	//}

	// 创建输出窗格:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndOpOutput.Create(rectDummy, this) )
	{
		TRACE0("未能创建输出窗口\n");
		return -1;      // 未能创建
	}

	//m_wndTabs.AddTab(&m_wndErrorOutput, Language(L"错误信息"), (UINT)0);
	//m_wndTabs.AddTab(&m_wndOpOutput, Language(L"运行信息"), (UINT)1);
	//m_wndTabs.SetFont(GetBitmapMgr()->GetFontPtr(FONT_MODE_NORMAL));
	//m_wndErrorOutput.SetFont(GetBitmapMgr()->GetFontPtr(FONT_MODE_NORMAL));
	m_wndOpOutput.SetFont(GetBitmapMgr()->GetFontPtr(FONT_MODE_NORMAL));
	m_wndOpOutput.m_wndOutput	=	this;
	//
	m_wndErrorText.Create(ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_READONLY, rectDummy, this, 0);
	m_wndOpOutput.ShowWindow(SW_SHOW);
	m_wndErrorText.SetFont(GetBitmapMgr()->GetFontPtr(FONT_MODE_NORMAL));
	//
	CRect rc;
	GetClientRect(&rc);
	m_wndOpOutput.MoveWindow(&CRect(rc.left, rc.top, rc.right - 180, rc.bottom));
	m_wndErrorText.MoveWindow(&CRect(rc.right - 180, rc.top, rc.right, rc.bottom));

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	if(m_wndOpOutput.m_hWnd)
	{
		CRect rc;
		GetClientRect(&rc);
		m_wndOpOutput.MoveWindow(&CRect(rc.left, rc.top, rc.right - 180, rc.bottom));
		m_wndErrorText.MoveWindow(&CRect(rc.right - 180, rc.top, rc.right, rc.bottom));
	}

	// 选项卡控件应覆盖整个工作区:
	//m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, &COutputList::OnLbnSelchange)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList 消息处理程序

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	int sel = this->GetCurSel();
	if(sel >= 0)
	{
		CString str;
		this->GetText(sel, str);
		char buf[1024];
		CopyTextToClipboard(WToA(str, buf, 1024));
	}
	//MessageBox(_T("复制输出"));
}

void COutputList::OnEditClear()
{
	this->ResetContent();
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();
	}
}


void COutputList::OnLbnSelchange()
{
}


void COutputList::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListBoxIcon::OnLButtonDown(nFlags, point);
	//
	int sel	=	GetCurSel();
	CString str;
	if(sel >= 0)
		this->GetText(sel, str);
	else
		str	=	L"";
	m_wndOutput->m_wndErrorText.SetWindowTextW(str);
}
