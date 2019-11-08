
#include "stdafx.h"

#include "PropertiesRuntimeWnd.h"
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

CPropertiesRuntimeWnd*		g_pPropertyRuntimeWnd	=	0;
CPropertiesRuntimeWnd*		GetPropertiesRuntimeWindow()
{
	return g_pPropertyRuntimeWnd;
}

CPropertiesRuntimeWnd::CPropertiesRuntimeWnd()
{
	g_pPropertyRuntimeWnd	=	this;
}

CPropertiesRuntimeWnd::~CPropertiesRuntimeWnd()
{
	g_pPropertyRuntimeWnd	=	0;
}

BEGIN_MESSAGE_MAP(CPropertiesRuntimeWnd, CDockablePane)
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

void CPropertiesRuntimeWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	int cyCmb = 0;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	if(m_wndCurrentProp)
		m_wndCurrentProp->SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesRuntimeWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建组合:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	m_wndCurrentProp	=	&this->m_wndPropList;

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("未能创建属性网格\n");
		return -1;      // 未能创建
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 已锁定*/);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 锁定*/);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

VOID CPropertiesRuntimeWnd::SetCurrentProperties(GCPropertyBase* base, const wchar_t* szPropertyName)
{
	if(m_wndCurrentProp)
		m_wndCurrentProp->ShowWindow(SW_HIDE);
	this->m_wndCurrentProp	=	base;
	if(!this->m_wndCurrentProp)
		this->m_wndCurrentProp	=	&this->m_wndPropList;
	if(m_wndCurrentProp)
		m_wndCurrentProp->ShowWindow(SW_SHOW);
	//
	CRect rectClient,rectCombo;
	GetClientRect(rectClient);
	int cyCmb = 0;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	if(m_wndCurrentProp)
	{
		m_wndCurrentProp->SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
		m_wndCurrentProp->SetFont(&m_fntPropList);
	}
	if(szPropertyName)
		this->SetWindowText(szPropertyName);
	else
		this->SetWindowText(Language(L"运行时测试"));
}

void CPropertiesRuntimeWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesRuntimeWnd::OnExpandAllProperties()
{
	if(m_wndCurrentProp)
		m_wndCurrentProp->ExpandAll();
}

void CPropertiesRuntimeWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesRuntimeWnd::OnSortProperties()
{
	if(m_wndCurrentProp)
		m_wndCurrentProp->SetAlphabeticMode(!m_wndCurrentProp->IsAlphabeticMode());
}

void CPropertiesRuntimeWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	if(m_wndCurrentProp)
		pCmdUI->SetCheck(m_wndCurrentProp->IsAlphabeticMode());
}

void CPropertiesRuntimeWnd::OnProperties1()
{
	// TODO: 在此处添加命令处理程序代码
}

void CPropertiesRuntimeWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: 在此处添加命令更新 UI 处理程序代码
}

void CPropertiesRuntimeWnd::OnProperties2()
{
	// TODO: 在此处添加命令处理程序代码
}

void CPropertiesRuntimeWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: 在此处添加命令更新 UI 处理程序代码
}

void CPropertiesRuntimeWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	this->SetWindowText(Language(L"无可测试属性"));
	//
}

void CPropertiesRuntimeWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesRuntimeWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesRuntimeWnd::SetPropListFont()
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

	if(m_wndCurrentProp)
		m_wndCurrentProp->SetFont(&m_fntPropList);
}
