/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "PropertyDlgWndBase.h"

// CPropertiesDialogWnd

class CPropertiesDialogWnd : public CDockablePane
{
	DECLARE_DYNAMIC(CPropertiesDialogWnd)

public:
	CPropertiesDialogWnd();
	virtual ~CPropertiesDialogWnd();

	CPropertyDlgWndBase		m_wndBase;

	VOID					SetCurrentDialog(CDialogEx* dlgCurrent, const wchar_t* szName, BOOL bDestroy = FALSE);
	HWND					m_dlgCurrent;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


CPropertiesDialogWnd*	GetPropertiesDlgWindowPtr();
