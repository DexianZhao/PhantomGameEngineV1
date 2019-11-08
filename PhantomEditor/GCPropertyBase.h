/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "ValueEditor.h"

// GCPropertyBase

class GCPropertyBase;
class CMTreeCtrl;
class CMFCPropertyGridIntProperty;
class CMFCPropertyGridFloatProperty;

__interface	IViewBaseCallback
{
	void	OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp);
	void	OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem);	//更新选择树形控件
	void	OnDoubleClick(CMTreeCtrl* treePtr, HTREEITEM hItem);	//更新选择树形控件
	void	OnTreeViewKeyDown(INT nChar);
	//
	CMFCPropertyGridFloatProperty*	FindFloatPtr(CMFCPropertyGridProperty* g);
	CMFCPropertyGridIntProperty*	FindIntPtr(CMFCPropertyGridProperty* g);
};

class GCPropertyBase : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(GCPropertyBase)

public:
	GCPropertyBase();
	virtual ~GCPropertyBase();

	VOID					EnableRender(BOOL b);
	BOOL					m_bEnableRender;
	IViewBaseCallback*		m_callback;
	VOID					SetNameWidth(int n){this->m_nLeftColumnWidth = n;}
	//
	VOID					SetDescription(const wchar_t* szText);
	VOID					SetChildWindow(CWnd* wnd, INT nTop);
	//
	CValueEditor			m_valueEditor;
	CToolTipCtrl			m_toolTip;
	VOID					DisableEditor();
	//BOOL					m_bEnabled;
protected:
	VOID					AutoCreateDesc();
	CEdit					m_edDesc;
	CWnd*					m_childWnd;
	INT						m_nTop;
	virtual void AdjustLayout();
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


