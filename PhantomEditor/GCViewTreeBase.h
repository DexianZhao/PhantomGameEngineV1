/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "MTreeCtrl.h"
#include "GCPropertyBase.h"
// GCViewTreeBase

class GCViewTreeBase : public CMTreeCtrl
{
	DECLARE_DYNAMIC(GCViewTreeBase)

public:
	GCViewTreeBase(INT nBmpID);
	virtual ~GCViewTreeBase();
	//
	IViewBaseCallback*		m_callback;
	virtual	VOID			OnChangeSelect(HTREEITEM hItem);
	virtual	VOID			OnDoubleClick(HTREEITEM hItem);
protected:
	INT						m_uiBmpID;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


