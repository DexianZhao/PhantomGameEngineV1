/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// PropertiesDialogWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "phantom-gc.h"
#include "resource.h"
#include "PropertiesDialogWnd.h"
#include "GCLanguage.h"


// CPropertiesDialogWnd

CPropertiesDialogWnd*	g_wndPropertiesDlg = 0;
CPropertiesDialogWnd*	GetPropertiesDlgWindowPtr()
{
	return g_wndPropertiesDlg;
}

IMPLEMENT_DYNAMIC(CPropertiesDialogWnd, CDockablePane)

CPropertiesDialogWnd::CPropertiesDialogWnd()
{
	g_wndPropertiesDlg	=	this;
	m_dlgCurrent	=	this->m_wndBase.m_hWnd;
}

CPropertiesDialogWnd::~CPropertiesDialogWnd()
{
	g_wndPropertiesDlg	=	0;
}


BEGIN_MESSAGE_MAP(CPropertiesDialogWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEHWHEEL()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CPropertiesDialogWnd 消息处理程序




int CPropertiesDialogWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_wndBase.Create(m_wndBase.IDD, this);
	m_wndBase.ShowWindow(SW_SHOW);
	// TODO:  在此添加您专用的创建代码

	return 0;
}


void CPropertiesDialogWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	if(m_dlgCurrent != NULL)
		::MoveWindow(m_dlgCurrent, 0, 0, cx, cy, TRUE);
	if(m_wndBase.m_hWnd)
		m_wndBase.MoveWindow(0, 0, cx, cy, TRUE);
	// TODO: 在此处添加消息处理程序代码
}

BOOL	CALLBACK	EnumProcsHides(HWND hWndChild, LPARAM lpParam)
{
	//ShowWindow(hWndChild, SW_HIDE);
	return true;
}

VOID CPropertiesDialogWnd::SetCurrentDialog(CDialogEx* dlgCurrent, const wchar_t* szName, BOOL bDestroy)
{
	if(m_dlgCurrent && ::IsWindow(m_dlgCurrent) && !bDestroy)
		::ShowWindow(m_dlgCurrent, SW_HIDE);
	if(!bDestroy)
		EnumChildWindows(m_hWnd, EnumProcsHides, 0);
	this->m_dlgCurrent	=	dlgCurrent ? dlgCurrent->m_hWnd : 0;
	if(!m_dlgCurrent)
		m_dlgCurrent	=	this->m_wndBase.m_hWnd;
	CRect rc;
	GetClientRect(&rc);
	if(m_dlgCurrent)
	{
		::SetParent(m_dlgCurrent, m_hWnd);
		::MoveWindow(m_dlgCurrent, 0, 0, rc.Width()-1, rc.Height()-1, TRUE);
		::MoveWindow(m_dlgCurrent, 0, 0, rc.Width(), rc.Height(), TRUE);
		::ShowWindow(m_dlgCurrent, SW_SHOW);
	}
	if(!szName)
		szName	=	Language(L"属性");
	wchar_t szBuf[512];
	wcscpy(szBuf, szName);
	wcscat(szBuf, Language(L"对话框"));
	this->SetWindowTextW(szBuf);
}


void CPropertiesDialogWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDockablePane::OnRButtonDown(nFlags, point);
}


void CPropertiesDialogWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDockablePane::OnRButtonUp(nFlags, point);
}


void CPropertiesDialogWnd::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDockablePane::OnMouseHWheel(nFlags, zDelta, pt);
}

void CPropertiesDialogWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDockablePane::OnKeyDown(nChar, nRepCnt, nFlags);
}
