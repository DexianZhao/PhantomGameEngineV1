/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// MyEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "phantom-gc.h"
#include ".\MyEdit.h"
#include "math.h"

// CMyEdit2

IMPLEMENT_DYNAMIC(CMyEdit2, CEdit)
CMyEdit2::CMyEdit2()
{
	m_bFloat	=	false;
	m_fMinValue	=	0.0f;
	m_fMaxValue	=	1.0f;
	m_bLButtonDown	=	false;
	m_btLButtonDown.x	=	0;
	m_btLButtonDown.y	=	0;
	m_nValueWidth	=	3;
	m_fSaveValue	=	0;
	m_buttonPtr		=	0;
	m_buttonCtrlID	=	0;
}

CMyEdit2::~CMyEdit2()
{
}


BEGIN_MESSAGE_MAP(CMyEdit2, CEdit)
	ON_WM_CHAR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CMyEdit2 消息处理程序


void CMyEdit2::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

VOID CMyEdit2::SetFloat(double dMinValue, double dMaxValue, CWnd* dlg, INT ctrlID)
{
	this->m_fMinValue	=	dMinValue;
	this->m_fMaxValue	=	dMaxValue;
	this->m_buttonPtr	=	dlg;
	m_buttonCtrlID		=	ctrlID;
	//assert(m_buttonPtr != NULL);
}

void CMyEdit2::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bLButtonDown)
	{
		POINT pos;
		GetCursorPos(&pos);
		int height = m_btLButtonDown.y - pos.y;
		if(abs(height) > 20)//超过20的数值就开始进行拖动
		{
			if(height < 0)
				height	+=	20;
			else
				height	-=	20;
			float fMinChange	=	fabsf(1.0f / 300.0f * (m_fMaxValue - m_fMinValue) * ((float)height / 300.0f));
			float fValue;
			fValue		=	m_fSaveValue	+	(float)height * fMinChange;
			fValue		=	min(fValue, m_fMaxValue);
			fValue		=	max(fValue, m_fMinValue);
			//
			wchar_t fmt[100];
			wchar_t szBuf[255];
			swprintf(fmt, L"%%.%02df",m_nValueWidth);
			swprintf(szBuf, fmt, fValue);
			SetWindowText(szBuf);
			if(m_buttonPtr)
				m_buttonPtr->SendMessage(WM_COMMAND, MAKEWPARAM(m_buttonCtrlID, BN_CLICKED), (LPARAM)m_buttonPtr->GetDlgItem(m_buttonCtrlID)->m_hWnd);
		}
	}
	CEdit::OnMouseMove(nFlags, point);
}


void CMyEdit2::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bLButtonDown	=	true;
	CString str;
	GetWindowText(str);
	m_fSaveValue	=	_wtof(str);
	GetCursorPos(&m_btLButtonDown);
	SetCapture();
	CEdit::OnLButtonDown(nFlags, point);
}


void CMyEdit2::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bLButtonDown	=	false;
	ReleaseCapture();
	CEdit::OnLButtonUp(nFlags, point);
}
