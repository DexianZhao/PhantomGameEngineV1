/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/

#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "phantom-gc.h"
#include "GCLanguage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd*		g_pPropertyWnd	=	0;
CPropertiesWnd*		GetPropertiesWindow()
{
	return g_pPropertyWnd;
}

CPropertiesWnd::CPropertiesWnd()
{
	g_pPropertyWnd		=	this;
	m_wndCurrent		=	0;
	m_wndCurrentProp	=	0;
	m_wndBottom			=	0;
	m_nTopHeight		=	0;
}

CPropertiesWnd::~CPropertiesWnd()
{
	g_pPropertyWnd	=	0;
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 消息处理程序

VOID CPropertiesWnd::AddBottomWindow(HWND hWnd, INT nHeight)
{
	if(m_wndBottom)
		::ShowWindow(m_wndBottom, SW_HIDE);
	m_wndBottom			=	hWnd;
	if(m_wndBottom)
		::ShowWindow(m_wndBottom, SW_SHOW);
	m_nTopHeight		=	nHeight;
	AdjustLayout();
}

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	int cyCmb = 0;
	int cyTlb = 0;//m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	//m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	if(m_wndCurrent)
	{
		CRect rcCurrent(rectClient.left, rectClient.top + cyCmb + cyTlb, 0, 0);
		rcCurrent.right		=	rcCurrent.left + rectClient.Width();
		rcCurrent.bottom	=	rcCurrent.top + rectClient.Height() -(cyCmb+cyTlb);
		//
		if(m_wndBottom && ::IsWindow(m_wndBottom))
		{
			CRect rcBottom	=	rcCurrent;
			rcCurrent.bottom	=	rcCurrent.top + m_nTopHeight;
			rcBottom.top	+=	m_nTopHeight;
			::SetWindowPos(m_wndBottom, NULL, rcBottom.left, rcBottom.top, rcBottom.Width(), rcBottom.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
		}
		m_wndCurrent->SetWindowPos(NULL, rcCurrent.left, rcCurrent.top, rcCurrent.Width(), rcCurrent.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建组合:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	m_wndCurrentProp	=	&this->m_wndPropList;
	m_wndCurrent		=	m_wndCurrentProp;

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("未能创建属性网格\n");
		return -1;      // 未能创建
	}

	InitPropList();

	//m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	//m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 已锁定*/);
	//m_wndToolBar.CleanUpLockedImages();
	//m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 锁定*/);

	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	//m_wndToolBar.SetOwner(this);

	//// 所有命令将通过此控件路由，而不是通过主框架路由:
	//m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

VOID CPropertiesWnd::SetCurrentWindow(CWnd* base, const wchar_t* szPropertyName, BOOL bDestroy)
{
	if(m_wndCurrent && !bDestroy)
	{
		m_wndCurrent->ShowWindow(SW_HIDE);
		if(IsWindow(m_wndBottom))
			::ShowWindow(m_wndBottom, SW_HIDE);
	}
	m_wndBottom		=	0;
	this->m_wndCurrentProp	=	0;
	m_wndCurrent	=	base;
	if(m_wndCurrent)
		m_wndCurrent->ShowWindow(SW_SHOW);
	//
	CRect rectClient,rectCombo;
	GetClientRect(rectClient);
	int cyCmb = 0;
	int cyTlb = 0;//m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	if(m_wndCurrent)
	{
		m_wndCurrent->SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
		m_wndCurrent->SetFont(&m_fntPropList);
	}
	if(szPropertyName)
		this->SetWindowText(szPropertyName);
	else
		this->SetWindowText(Language(L"无属性"));
}

VOID CPropertiesWnd::SetCurrentProperties(GCPropertyBase* base, const wchar_t* szPropertyName, BOOL bDestroy)
{
	if(m_wndCurrent && IsWindow(m_wndCurrent->m_hWnd) && !bDestroy)
		m_wndCurrent->ShowWindow(SW_HIDE);
	this->m_wndCurrentProp	=	base;
	if(!this->m_wndCurrentProp)
		this->m_wndCurrentProp	=	&this->m_wndPropList;
	m_wndCurrent	=	m_wndCurrentProp;
	if(m_wndCurrent)
		m_wndCurrent->ShowWindow(SW_SHOW);
	//
	CRect rectClient,rectCombo;
	GetClientRect(rectClient);
	int cyCmb = 0;
	int cyTlb = 0;//m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	if(m_wndCurrent)
	{
		m_wndCurrent->SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
		m_wndCurrent->SetFont(&m_fntPropList);
	}
	if(szPropertyName)
		this->SetWindowText(szPropertyName);
	else
		this->SetWindowText(Language(L"无属性"));
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	if(m_wndCurrentProp)
		m_wndCurrentProp->ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	if(m_wndCurrentProp)
		m_wndCurrentProp->SetAlphabeticMode(!m_wndCurrentProp->IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	if(m_wndCurrentProp)
		pCmdUI->SetCheck(m_wndCurrentProp->IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: 在此处添加命令处理程序代码
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: 在此处添加命令更新 UI 处理程序代码
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: 在此处添加命令处理程序代码
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: 在此处添加命令更新 UI 处理程序代码
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableRender(TRUE);
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	this->SetWindowText(Language(L"无属性"));
	//
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	if(m_wndCurrent)
		m_wndCurrent->SetFont(&m_fntPropList);
}
