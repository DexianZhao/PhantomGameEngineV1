/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#ifndef __MY_EDIT_H__
#define	__MY_EDIT_H__
#pragma once


// CMyEdit2

class CMyEdit2 : public CEdit
{
	DECLARE_DYNAMIC(CMyEdit2)

public:
	CMyEdit2();
	virtual ~CMyEdit2();
	VOID	SetFloat(double dMinValue, double dMaxValue, CWnd* dlg, INT ctrlID);
	bool	m_bFloat;
	double	m_fMinValue;
	double	m_fMaxValue;
	bool	m_bLButtonDown;
	POINT	m_btLButtonDown;
	int		m_nValueWidth;
	float	m_fSaveValue;
	CWnd*	m_buttonPtr;
	int		m_buttonCtrlID;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};



#endif
