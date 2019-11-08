/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// ToolTipDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ToolTipDialog.h"
#include "afxdialogex.h"


// CToolTipDialog 对话框

IMPLEMENT_DYNAMIC(CToolTipDialog, CDialogEx)

CToolTipDialog::CToolTipDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CToolTipDialog::IDD, pParent)
{
	m_tipWindow			=	0;
	m_nTimeMillisecond	=	0;
}

CToolTipDialog::~CToolTipDialog()
{
}

void CToolTipDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CToolTipDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CToolTipDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CToolTipDialog::OnBnClickedCancel)
	ON_WM_SETFOCUS()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CToolTipDialog 消息处理程序


void CToolTipDialog::OnBnClickedOk()
{
}


void CToolTipDialog::OnBnClickedCancel()
{
}


void CToolTipDialog::OnSetFocus(CWnd* pOldWnd)
{
	//CDialogEx::OnSetFocus(pOldWnd);
	if(pOldWnd)
		pOldWnd->SetFocus();
	this->ShowWindow(SW_HIDE);

	// TODO: 在此处添加消息处理程序代码
}

VOID CToolTipDialog::ShowToolTip(CWnd* wnd, const wchar_t* szText, const POINT& pos, int nTimeMillisecond)
{
	m_tipWindow	=	wnd ? wnd->m_hWnd : 0;
	this->m_strToolTip	=	szText;
	if(m_backImage.IsNull())
	{
		CRect rc;
		GetClientRect(&rc);
		m_backImage.Create(rc.Width(), rc.Height(), 24);
	}
	HDC dc	=	m_backImage.GetDC();
	CRect rcText(0, 0, 500, 500);
	DrawTextW(dc, m_strToolTip, -1, &rcText, DT_LEFT | DT_TOP | DT_CALCRECT | DT_EXPANDTABS | DT_WORDBREAK);
	m_backImage.ReleaseDC();
	rcText.InflateRect(2, 2);
	MoveWindow(&CRect(pos.x, pos.y, pos.x + rcText.right, pos.y + rcText.bottom));
	SetTimer(0, 100, 0);
	m_nTimeMillisecond	=	nTimeMillisecond;
	this->ShowWindow(SW_SHOWNA);
	this->Invalidate();
}

void CToolTipDialog::OnTimer(UINT_PTR nIDEvent)
{
	//if(::GetFocus() != m_tipWindow)
	//	m_nTimeMillisecond	=	0;
	m_nTimeMillisecond	-=	100;
	if(m_nTimeMillisecond <= 0)
	{
		ShowWindow(SW_HIDE);
		KillTimer(nIDEvent);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CToolTipDialog::OnSize(UINT nType, int cx, int cy)
{
	if(!m_backImage.IsNull())
		m_backImage.Destroy();
	m_backImage.Create(cx, cy, 24);
	CDialogEx::OnSize(nType, cx, cy);
}


void CToolTipDialog::OnPaint()
{
	CPaintDC dc(this);
	if(m_backImage.IsNull())
		return;
	CRect rc, rcClient;
	GetClientRect(&rc);
	rcClient	=	rc;
	if(m_backImage.GetWidth() != rc.Width() || m_backImage.GetHeight() != rc.Height())
	{
		m_backImage.Destroy();
		m_backImage.Create(rc.Width(), rc.Height(), 24);
	}
	HDC dcImage	=	m_backImage.GetDC();
	FillRect(dcImage, &rc, CBrush(RGB(255, 255, 255)));
	rc.InflateRect(-2, -2);
	CFont* font	=	GetFont();
	HGDIOBJ old	=	0;
	if(font)
		old	=	SelectObject(dcImage, font->m_hObject);
	//
	SetTextColor(dcImage, 0);
	SetBkMode(dcImage, TRANSPARENT);
	DrawTextW(dcImage, this->m_strToolTip, -1, &rc, DT_LEFT | DT_TOP | DT_EXPANDTABS | DT_WORDBREAK);
	//
	BitBlt(dc.m_hDC, 0, 0, rcClient.Width(), rcClient.Height(), dcImage, 0, 0, SRCCOPY);
	dc.Draw3dRect(&rcClient, RGB(128, 128, 128), RGB(128, 128, 128));
	if(old)
		SelectObject(dcImage, old);
	m_backImage.ReleaseDC();
}


BOOL CToolTipDialog::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


void CToolTipDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	HWND hWnd	=	::GetFocus();
	this->ShowWindow(SW_HIDE);
	::SetFocus(hWnd);
	CDialogEx::OnLButtonDown(nFlags, point);
}
