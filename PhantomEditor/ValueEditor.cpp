/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// ValueEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "ValueEditor.h"
#include "afxdialogex.h"

// CValueEditor 对话框

IMPLEMENT_DYNAMIC(CValueEditor, CDialogEx)

CValueEditor::CValueEditor(CWnd* pParent /*=NULL*/)
	: CDialogEx(CValueEditor::IDD, pParent)
{
	m_rectTrack.SetRect(0, 0, 12, 12);
	m_floatPtr		=	0;
	m_nLeft			=	0;
	m_fSaveValue	=	0;
	m_bIsDown		=	false;
	m_intPtr		=	0;
}

CValueEditor::~CValueEditor()
{
}

float		CValueEditor::GetMaxValue()
{
	if(m_floatPtr)
		return m_floatPtr->m_fMax;
	if(m_intPtr)
		return m_intPtr->m_nMax;
	return 1.0f;
}
float		CValueEditor::GetMinValue()
{
	if(m_floatPtr)
		return m_floatPtr->m_fMin;
	if(m_intPtr)
		return m_intPtr->m_nMin;
	return 0.0f;
}
float		CValueEditor::GetValue()
{
	if(m_floatPtr)
		return m_floatPtr->GetValue().fltVal;
	if(m_intPtr)
		m_intPtr->GetValue().lVal;
	return 0;
}

VOID		CValueEditor::SetIntPtr(CMFCPropertyGridIntProperty* g)
{
	m_floatPtr	=	0;
	m_intPtr	=	g;
	if(m_intPtr)
		this->SetValue(m_fSaveValue = GetValue());
}

VOID		CValueEditor::SetFloatPtr(CMFCPropertyGridFloatProperty* g)
{
	m_intPtr	=	0;
	m_floatPtr	=	g;
	if(m_floatPtr)
		this->SetValue(m_fSaveValue = GetValue());
}

#define	VALUE_BUTTON_WIDTH	5
VOID		CValueEditor::SetTrackLeft(INT nLeft)
{
	CRect rc;
	this->GetClientRect(&rc);
	rc.InflateRect(-1,-1);
	int nWidth	=	VALUE_BUTTON_WIDTH;
	int beginx	=	nWidth + rc.left;
	int nValue	=	nLeft - beginx + VALUE_BUTTON_WIDTH;
	float fValue	=	(float)nValue / (float)(rc.Width() - VALUE_BUTTON_WIDTH - beginx);
	fValue		=	fValue * (GetMaxValue() - GetMinValue()) + GetMinValue();
	if(m_floatPtr)
		m_floatPtr->OnUpdate(COleVariant(fValue));
	if(m_intPtr)
	{
		long l = (long)fValue;
		m_intPtr->OnUpdate(COleVariant(l));
	}
	SetValue(fValue);
}

VOID		CValueEditor::SetValue(float f)
{
	float fValue	=	GetValue();
	CRect rc;
	this->GetClientRect(&rc);
	rc.InflateRect(-1,-1);
	int nWidth	=	VALUE_BUTTON_WIDTH;
	int beginx	=	nWidth + rc.left;
	int	left	=	(int)((fValue - GetMinValue())/(GetMaxValue() - GetMinValue()) * (rc.Width() - VALUE_BUTTON_WIDTH - beginx)) + beginx;
	m_rectTrack.SetRect(left - nWidth, 1, left + nWidth, rc.Height() + 1);
	this->Invalidate();
}

void CValueEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CValueEditor, CDialogEx)
	ON_BN_CLICKED(IDOK, &CValueEditor::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CValueEditor::OnBnClickedCancel)
	//ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


// CValueEditor 消息处理程序


void CValueEditor::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


void CValueEditor::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CValueEditor::OnSetFocus(CWnd* pOldWnd)
{
	if(pOldWnd)
		pOldWnd->SetFocus();
	//CDialogEx::OnSetFocus(pOldWnd);
	

	// TODO: 在此处添加消息处理程序代码
}


BOOL CValueEditor::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


void CValueEditor::OnPaint()
{
	CPaintDC dc(this);
	CRect rc;
	GetClientRect(&rc);
	dc.Draw3dRect(&rc, RGB(58, 58, 58), RGB(58, 58, 58));
	rc.InflateRect(-1,-1);
	dc.FillRect(&rc, &CBrush(GetSysColor(COLOR_BTNFACE)));
	dc.FillRect(&m_rectTrack, &CBrush(GetSysColor(COLOR_3DSHADOW)));//BDR_SUNKENINNER, BF_LEFT | BF_TOP | BF_RIGHT | BF_BOTTOM);
	dc.DrawEdge(&m_rectTrack, BDR_SUNKENINNER, BF_LEFT | BF_TOP | BF_RIGHT | BF_BOTTOM);
}


void CValueEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(&pos);
	if(this->m_rectTrack.PtInRect(pos))
	{
		SetCapture();
		m_bIsDown	=	true;
		this->m_nLeft	=	pos.x - m_rectTrack.left;
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CValueEditor::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bIsDown)
	{
		ReleaseCapture();
		m_bIsDown	=	false;
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CValueEditor::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bIsDown)
	{
		CRect rc;
		GetClientRect(&rc);
		POINT pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		this->SetTrackLeft(pos.x - m_nLeft);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CValueEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_ESCAPE)
	{
		if(m_floatPtr)
			m_floatPtr->SetValue(COleVariant(m_fSaveValue));
		if(m_intPtr)
		{
			long l = (long)m_fSaveValue;
			m_intPtr->SetValue(COleVariant(l));
		}
		ShowWindow(SW_HIDE);
	}
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CValueEditor::OnKillFocus(CWnd* pNewWnd)
{
	ShowWindow(SW_HIDE);
	CDialogEx::OnKillFocus(pNewWnd);
}
