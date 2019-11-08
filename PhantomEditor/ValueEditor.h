/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "resource.h"
#include "MyMFCPropertyGridProperty.h"

class CValueEditor : public CDialogEx
{
	DECLARE_DYNAMIC(CValueEditor)

public:
	CValueEditor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CValueEditor();

// 对话框数据
	enum { IDD = IDD_PROPERTY_VALUE_EDITOR };
	CRect		m_rectTrack;
	BOOL		m_bIsDown;
	int			m_nLeft;
	//
	VOID		SetTrackLeft(INT nLeft);
	VOID		SetValue(float f);
	//
	float		GetMaxValue();
	float		GetMinValue();
	float		GetValue();
	//
	float		m_fSaveValue;
	VOID		SetFloatPtr(CMFCPropertyGridFloatProperty* g);
	VOID		SetIntPtr(CMFCPropertyGridIntProperty* g);
	CMFCPropertyGridFloatProperty*	m_floatPtr;
	CMFCPropertyGridIntProperty*	m_intPtr;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};
