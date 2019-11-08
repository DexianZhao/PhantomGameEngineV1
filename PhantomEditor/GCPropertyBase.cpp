/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// GCPropertyBase.cpp : 实现文件
//

#include "stdafx.h"
#include "phantom-gc.h"
#include "GCPropertyBase.h"
#include "GCBitmapMgr.h"
#include "MainFrm.h"

// GCPropertyBase

IMPLEMENT_DYNAMIC(GCPropertyBase, CMFCPropertyGridCtrl)

GCPropertyBase::GCPropertyBase()
{
	m_callback	=	0;
	m_bEnableRender	=	true;
	m_childWnd	=	0;
	m_nTop		=	0;
}

GCPropertyBase::~GCPropertyBase()
{
}


BEGIN_MESSAGE_MAP(GCPropertyBase, CMFCPropertyGridCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()



// GCPropertyBase 消息处理程序




void GCPropertyBase::OnPropertyChanged(CMFCPropertyGridProperty* pProp) const
{
	if(m_callback)
		m_callback->OnPropertyChanged(this, pProp);
	CMFCPropertyGridCtrl::OnPropertyChanged(pProp);
}


void GCPropertyBase::OnPaint()
{
	if(m_bEnableRender)
		CMFCPropertyGridCtrl::OnPaint();
}


BOOL GCPropertyBase::OnEraseBkgnd(CDC* pDC)
{
	if(!m_bEnableRender)
		return true;
	return CMFCPropertyGridCtrl::OnEraseBkgnd(pDC);
}

VOID GCPropertyBase::EnableRender(BOOL b)
{
	m_bEnableRender	=	b;
	if(m_bEnableRender)
		this->Invalidate();
}

VOID GCPropertyBase::SetChildWindow(CWnd* wnd, INT nTop)
{
	m_childWnd	=	wnd;
	m_nTop		=	nTop;
	m_childWnd->SetParent(this);
	CRect rcChild;
	GetClientRect(rcChild);
	rcChild.top		+=	m_nTop;
	rcChild.bottom	=	m_rectList.bottom;
	m_childWnd->MoveWindow(&rcChild);
	m_childWnd->ShowWindow(SW_SHOW);
}

VOID GCPropertyBase::AutoCreateDesc()
{
	CRect rectClient;
	GetClientRect(rectClient);
	if (m_bDescriptionArea)
	{
		if(!m_edDesc.m_hWnd)
		{
			m_edDesc.Create(ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_READONLY, CRect(0, 0, 1, 1), this, 0);
			m_edDesc.SetFont(GetBitmapMgr()->GetFontPtr(FONT_MODE_NORMAL));
			InitTool(m_toolTip, this);
			AddTool(m_toolTip, m_edDesc, L"");
		}
		CRect rectDescr = rectClient;
		rectDescr.top = m_rectList.bottom;
		if (rectDescr.Height() > 0)
		{
			m_edDesc.ShowWindow(SW_HIDE);
			rectDescr.InflateRect(-4, -4);
			rectDescr.top	+=	5;
			m_edDesc.MoveWindow(&rectDescr);
		}
		else
			m_edDesc.ShowWindow(SW_HIDE);
	}
	if(m_childWnd)
	{
		CRect rcChild	=	rectClient;
		rcChild.top		+=	m_nTop;
		rcChild.bottom	=	m_rectList.bottom;
		m_childWnd->MoveWindow(&rcChild);
	}
}

VOID GCPropertyBase::SetDescription(const wchar_t* szText)
{
	if(!m_bDescriptionArea)
		return;
	AutoCreateDesc();
	m_edDesc.SetWindowTextW(szText);
	m_edDesc.ShowWindow(SW_SHOW);
}

void GCPropertyBase::AdjustLayout()
{
	__super::AdjustLayout();
	AutoCreateDesc();
}

#define	VALUE_BUTTON_HEIGHT	12
VOID GCPropertyBase::DisableEditor()
{
	for(int i=0;i<this->GetPropertyCount();i++){
		CMFCPropertyGridProperty* prop = this->GetProperty(i);
		for(int j=0;j<prop->GetSubItemsCount();j++){
			CMFCPropertyGridProperty* sub = prop->GetSubItem(j);
			sub->Enable(false);
		}
	}
}
void GCPropertyBase::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
	CMFCPropertyGridProperty* sel	=	this->GetCurSel();
	if(sel)
		m_edDesc.ShowWindow(SW_HIDE);
	if(m_callback)
	{
		CMFCPropertyGridFloatProperty* fp	=	m_callback->FindFloatPtr(sel);
		CMFCPropertyGridIntProperty* ip	=	m_callback->FindIntPtr(sel);
		if(fp)
		{
			CRect rc	=	fp->GetRect();
			rc.OffsetRect(0, rc.Height());
			rc.left		+=	(GetLeftColumnWidth() - 50);
			rc.bottom	=	rc.top + VALUE_BUTTON_HEIGHT;
			m_valueEditor.SetFloatPtr(fp);
			m_valueEditor.MoveWindow(&rc);
			//m_valueEditor.ShowWindow(SW_SHOWNA);
		}
		else if(ip)
		{
			CRect rc	=	ip->GetRect();
			rc.OffsetRect(0, rc.Height());
			rc.left		+=	(GetLeftColumnWidth() - 50);
			rc.bottom	=	rc.top + VALUE_BUTTON_HEIGHT;
			m_valueEditor.SetIntPtr(ip);
			m_valueEditor.MoveWindow(&rc);
			//m_valueEditor.ShowWindow(SW_SHOWNA);
		}
		else
			m_valueEditor.ShowWindow(SW_HIDE);
	}
}

BOOL GCPropertyBase::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(m_childWnd)
	{
		CRect rc;
		m_childWnd->GetWindowRect(&rc);
		CPoint pos;
		GetCursorPos(&pos);
		if(rc.PtInRect(pos))
		{
			return CWnd::OnSetCursor(pWnd, nHitTest, message);
		}
	}
	return __super::OnSetCursor(pWnd, nHitTest, message);
}


BOOL GCPropertyBase::PreTranslateMessage(MSG* pMsg)
{
	return CMFCPropertyGridCtrl::PreTranslateMessage(pMsg);
}
