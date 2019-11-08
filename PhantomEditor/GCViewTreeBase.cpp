/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// GCViewTreeBase.cpp : 实现文件
//

#include "stdafx.h"
#include "phantom-gc.h"
#include "GCViewTreeBase.h"
#include "GCBitmapMgr.h"

// GCViewTreeBase

IMPLEMENT_DYNAMIC(GCViewTreeBase, CMTreeCtrl)

GCViewTreeBase::GCViewTreeBase(INT nBmpID)
{
	m_uiBmpID			=	nBmpID;
	m_callback			=	0;
}

GCViewTreeBase::~GCViewTreeBase()
{
}


BEGIN_MESSAGE_MAP(GCViewTreeBase, CMTreeCtrl)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// GCViewTreeBase 消息处理程序




void GCViewTreeBase::OnDestroy()
{
	CMTreeCtrl::OnDestroy();
}


int GCViewTreeBase::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	this->SetIconImage(this->m_uiBmpID);
	this->m_hFont	=	(HFONT)GetBitmapMgr()->GetFontPtr(FONT_MODE_NORMAL)->m_hObject;
	this->SetItemHeight(16);
	return 0;
}


VOID			GCViewTreeBase::OnChangeSelect(HTREEITEM hItem)
{
	if(this->m_callback)
		this->m_callback->OnChangeTreeSelect(this, hItem);
}


void GCViewTreeBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_callback)
		m_callback->OnTreeViewKeyDown(nChar);
	CMTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

VOID			GCViewTreeBase::OnDoubleClick(HTREEITEM hItem)
{
	if(this->m_callback)
		this->m_callback->OnDoubleClick(this, hItem);
}
