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
#include "ImageCheck.h"


// CImageCheck

IMPLEMENT_DYNAMIC(CImageCheck, CButton)

CImageCheck::CImageCheck()
{
	m_image				=	0;
	m_imageWidthCount	=	0;
	m_imageHeightCount	=	0;
	m_imageHeightIndex	=	0;
	m_bDrawBorder		=	false;
	m_clrBorder			=	0;
	m_bDrawTextInLeft	=	false;
}

CImageCheck::~CImageCheck()
{
}

VOID		CImageCheck::SetImage(CImage& img, INT nImageWidthCount, INT nImageHeightCount, INT nHeightIndex, BOOL b)
{
	m_bDrawBorder		=	b;
	m_image	=	&img;
	this->m_imageWidthCount	=	nImageWidthCount;
	m_imageHeightCount	=	nImageHeightCount;
	m_imageHeightIndex	=	nHeightIndex;
	//
	int nWidth	=	m_image->GetWidth() / nImageWidthCount;
	int nHeight	=	m_image->GetHeight() / nImageHeightCount;
	CRect rc;
	this->GetWindowRect(&rc);
	this->GetParent()->ScreenToClient(&rc);
	rc.right	=	rc.left + nWidth;
	rc.bottom	=	rc.top + nHeight;
	this->MoveWindow(&rc);
}

BEGIN_MESSAGE_MAP(CImageCheck, CButton)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CImageCheck 消息处理程序




void CImageCheck::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}


void CImageCheck::PreSubclassWindow()
{
	CButton::PreSubclassWindow();
}


void CImageCheck::OnPaint()
{
	CPaintDC dc(this);
	CRect rcItem;
	GetClientRect(&rcItem);
	if(m_image)
	{
		int nWidth	=	m_image->GetWidth() / this->m_imageWidthCount;
		int nHeight	=	m_image->GetHeight() / this->m_imageHeightCount;
		CRect	rcImage(0, nHeight * this->m_imageHeightIndex, nWidth, nHeight * (this->m_imageHeightIndex + 1));
		if(this->GetCheck())
		{
			rcImage.OffsetRect(nWidth, 0);
			//uStyle |= DFCS_PUSHED;
		}
		CDC& imageDC	=	*CDC::FromHandle(m_image->GetDC());
		//
		CRect rcFill	=	rcItem;
		rcFill.left		+=	nWidth;
		dc.FillRect(&rcFill, &CBrush(RGB(255, 255, 255)));
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(0);
		CFont* old	=	0;
		if(GetFont())
			old	=	dc.SelectObject(GetFont());
		if(this->IsWindowEnabled())
		{
			if(m_bDrawTextInLeft)
			{
				CString str;
				GetWindowText(str);
				dc.DrawText(str, &rcFill, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			}
			BitBlt(dc.m_hDC, 0, 0, rcImage.Width(), rcImage.Height(), imageDC.m_hDC, rcImage.left, rcImage.top, SRCCOPY);
		}
		else
		{
			dc.FillRect(&CRect(0, 0, nWidth, nHeight), &CBrush(RGB(200,200,200)));
			BitBlt(dc.m_hDC, 0, 0, rcImage.Width(), rcImage.Height(), imageDC.m_hDC, rcImage.left, rcImage.top, SRCPAINT);
		}
		if(this->m_bDrawBorder)
		{
			if(GetCheck())
			{
				dc.Draw3dRect(&rcItem, RGB(128, 128, 128), RGB(128, 128, 128));
				//dc.DrawEdge(&rcItem, BDR_SUNKENINNER, BF_LEFT | BF_TOP | BF_RIGHT | BF_BOTTOM);
			}
			else
			{
				dc.Draw3dRect(&rcItem, RGB(255, 255, 255), RGB(255, 255, 255));
				//dc.DrawEdge(&rcItem, BDR_SUNKENOUTER, BF_LEFT | BF_TOP | BF_RIGHT | BF_BOTTOM);
			}
		}
		if(old)
			dc.SelectObject(old);
		m_image->ReleaseDC();
		//::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, DFC_BUTTON, uStyle);
	}
	else
	{
		UINT uStyle = DFCS_BUTTONPUSH;
		if(this->GetCheck())
			uStyle |= DFCS_PUSHED;
		::DrawFrameControl(dc.m_hDC, &rcItem, DFC_BUTTON, uStyle);
		CString strText;
		GetWindowText(strText);
		SetBkMode(dc.m_hDC, TRANSPARENT);
		COLORREF crOldColor = ::SetTextColor(dc.m_hDC, RGB(255,0,0));
		::DrawText(dc.m_hDC, strText, strText.GetLength(), &rcItem, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		::SetTextColor(dc.m_hDC, crOldColor);
	}
	//if(m_bDrawBorder && this->GetCheck())
	//	dc.Draw3dRect(&rcItem, this->m_clrBorder, m_clrBorder);
}
