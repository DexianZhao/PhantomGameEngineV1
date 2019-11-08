/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/

// ChildFrm.cpp : CChildFrame 类的实现
//

#include "stdafx.h"
#include "phantom-gc.h"
#include "GCViewManager.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	ON_WM_MDIACTIVATE()
	ON_WM_DROPFILES()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CChildFrame 构造/析构

std::vector<CChildFrame*>	g_childFrames;
CChildFrame*		GetChildFrame(HWND hWnd)
{
	for(int i=0;i<g_childFrames.size();i++)
	{
		if(g_childFrames[i]->m_hWnd == hWnd)
			return g_childFrames[i];
		if(g_childFrames[i]->GetActiveFrame())
			if(g_childFrames[i]->GetActiveFrame()->m_hWnd == hWnd)
				return g_childFrames[i];
	}
	return 0;
}

CChildFrame::CChildFrame()
{
	m_hIcon	=	0;
	g_childFrames.push_back(this);
}

CChildFrame::~CChildFrame()
{
	for(int i=0;i<g_childFrames.size();i++)
	{
		if(g_childFrames[i] == this)
		{
			g_childFrames.erase(g_childFrames.begin() + i);
			break;
		}
	}
}

HICON CChildFrame::GetFrameIcon() const
{
	if(m_hIcon)
		return m_hIcon;
	return __super::GetFrameIcon();
}

HBITMAP CChildFrame::OnGetIconicThumbnail(int nWidth, int nHeight)
{
	if(m_pViewActive)
	{
		//GCViewBase* view	=	GetViewManager()->FindDocument(m_pViewActive->m_hWnd);
		//view->m_iconImage.
	}
	return NULL;
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame 诊断

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 消息处理程序


void CChildFrame::ActivateFrame(int nCmdShow)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(GetActiveView())
	{
		GCViewBase* pView	=	GetViewManager()->FindDocument(GetActiveView()->m_hWnd);
		if(pView)
			pView->SetActiveFrame(this);
	}

	CMDIChildWndEx::ActivateFrame(nCmdShow);
}


void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWndEx::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	if(bActivate && pActivateWnd == this)
	{
		GetViewManager()->OnActiveDocument(this->m_pViewActive->m_hWnd);//pActivateWnd->m_hWnd);
	}

	// TODO: 在此处添加消息处理程序代码
}


void CChildFrame::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CMDIChildWndEx::OnDropFiles(hDropInfo);
}


int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	this->DragAcceptFiles(TRUE);
	return 0;
}

VOID CChildFrame::SetToolTip(const wchar_t* szText)
{
	if(!this->m_pRelatedTabGroup)
		return;
	m_pRelatedTabGroup->EnableCustomToolTips(FALSE);
}

void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CMDIChildWndEx::OnUpdateFrameTitle(bAddToTitle);
}

extern	void AFXAPI AfxCancelModes(HWND hWndRcvr);
BOOL CChildFrame::PreTranslateMessage(MSG* pMsg)
{
	return CMDIChildWndEx::PreTranslateMessage(pMsg);
}
