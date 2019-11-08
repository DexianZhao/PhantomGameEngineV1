/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// ImageButton.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageButton.h"
#include "GCViewManager.h"

// CImageButton

IMPLEMENT_DYNAMIC(CImageButton, CButton)

	CImageButton::CImageButton()
{
	m_image				=	0;
	m_imageWidthCount	=	0;
	m_imageHeightCount	=	0;
	m_imageHeightIndex	=	0;
	m_bDrawBorder		=	false;
	m_clrBorder			=	0;
}

CImageButton::~CImageButton()
{
}

VOID		CImageButton::SetImage(INT nHeightIndex)
{
	m_imageHeightIndex	=	nHeightIndex;
	this->Invalidate();
}

VOID		CImageButton::SetImage(CImage& img, INT nImageWidthCount, INT nImageHeightCount, INT nHeightIndex, CPoint* pos, BOOL bIsLeft, BOOL b)
{
	m_bDrawBorder		=	b;
	m_image				=	&img;
	if(nImageHeightCount < 0)
		nImageHeightCount	=	m_image->GetHeight() / (m_image->GetWidth() / nImageWidthCount);
	this->m_imageWidthCount	=	nImageWidthCount;
	m_imageHeightCount	=	nImageHeightCount;
	m_imageHeightIndex	=	nHeightIndex;
	//
	int nWidth	=	m_image->GetWidth() / nImageWidthCount;
	int nHeight	=	m_image->GetHeight() / nImageHeightCount;
	CRect rc;
	this->GetWindowRect(&rc);
	this->GetParent()->ScreenToClient(&rc);
	if(pos)
	{
		rc.left		=	pos->x;
		rc.top		=	pos->y;
		if(bIsLeft)
			pos->x	+=	nWidth;
		else
			pos->y	+=	nHeight;
	}
	rc.right	=	rc.left + nWidth;
	rc.bottom	=	rc.top + nHeight;
	this->MoveWindow(&rc);
}

BEGIN_MESSAGE_MAP(CImageButton, CButton)
	//ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CImageButton 消息处理程序




void CImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(m_image)
	{
		UINT uStyle = DFCS_BUTTONPUSH;
		ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);
		int nWidth	=	m_image->GetWidth() / this->m_imageWidthCount;
		int nHeight	=	m_image->GetHeight() / this->m_imageHeightCount;
		CRect	rcImage(0, nHeight * this->m_imageHeightIndex, nWidth, nHeight * (this->m_imageHeightIndex + 1));
		if(!IsWindowEnabled())//lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			rcImage.OffsetRect(nWidth, 0);
		}
		CRect rc;
		GetClientRect(&rc);
		m_image->BitBlt(lpDrawItemStruct->hDC, 0, 0, rcImage.Width(), rcImage.Height(), rcImage.left, rcImage.top, SRCCOPY);
		if(lpDrawItemStruct->itemState & ODS_SELECTED)
			DrawEdge(lpDrawItemStruct->hDC, &rc, BDR_SUNKENINNER, BF_LEFT | BF_TOP | BF_RIGHT | BF_BOTTOM);
	}
	else
	{
		UINT uStyle = DFCS_BUTTONPUSH;
		ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
			uStyle |= DFCS_PUSHED;
		::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, DFC_BUTTON, uStyle);
		CString strText;
		GetWindowText(strText);
		COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC, RGB(255,0,0));
		::DrawText(lpDrawItemStruct->hDC, strText, strText.GetLength(), &lpDrawItemStruct->rcItem, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		::SetTextColor(lpDrawItemStruct->hDC, crOldColor);
	}
	if(m_bDrawBorder)
		CDC::FromHandle(lpDrawItemStruct->hDC)->Draw3dRect(&lpDrawItemStruct->rcItem, this->m_clrBorder, m_clrBorder);
}


void CImageButton::PreSubclassWindow()
{
	CButton::PreSubclassWindow();
}


void CImageButton::OnPaint()
{
	CPaintDC dc(this);
}


void CImageButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CButton::OnLButtonDblClk(nFlags, point);
	OnLButtonDown(nFlags, point);
	this->Invalidate();
}


void CImageButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	CButton::OnLButtonDown(nFlags, point);
}
