/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// ImageListViewDLG.cpp : 实现文件
//

#include "stdafx.h"
#include ".\ImageListViewDLG.h"
#include "GCSceneEditor3D.h"

IMPLEMENT_DYNAMIC(CImageListViewDLG, CDialogBase)
CImageListViewDLG::CImageListViewDLG(CWnd* pParent /*=NULL*/)
	: CDialogBase(CImageListViewDLG::IDD, pParent)
{
	m_imageSize			=	64;
	m_imageViewSelect	=	0;
	m_iClickCallback	=	0;
	m_nParameter		=	0;
}

CImageListViewDLG::~CImageListViewDLG()
{
}

void CImageListViewDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CImageListViewDLG, CDialogBase)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CImageListViewDLG 消息处理程序

#define	IMAGE_VIEW_WIDTH		360
#define	IMAGE_VIEW_HEIGHT		512

void	CImageListViewDLG::showPopup(ImageItem* pItem, int x, int y, GCSceneEditor3D* cb, INT nParameter)
{
	m_imageViews.clear();
	this->rebuildImageViews();
 	m_imageViewSelect	=	-1;
	const int widthCount	=	8;
	for(size_t i=0;i<m_imageViews.size();i++)
	{
		if(m_imageViews.at(i).pImageItem == pItem)
		{
			m_imageViewSelect	=	i;
			break;
		}
	}
	int height	=	m_imageManager.getImageCount() / widthCount;
	if((m_imageManager.getImageCount() % widthCount) != 0)
		height++;
	if(height > 5)
		height = 5;
	CRect rc(x/* - IMAGE_VIEW_WIDTH*/, y - height * m_imageSize, x + m_imageSize * widthCount + 20/*IMAGE_VIEW_WIDTH*/, y);
	this->MoveWindow(&rc);
	this->ShowWindow(SW_SHOW);
	m_iClickCallback			=	cb;
	m_nParameter				=	nParameter;
	this->Invalidate();
}

void	CImageListViewDLG::rebuildImageViews()
{
	this->m_imageViews.clear();
	int imageIndex	=	0;
	const int widthCount	=	8;
	for(int i=0;i<m_imageManager.getImageCount();i++)
	{
		CRect rc((i % widthCount) * m_imageSize, (i / widthCount) * m_imageSize, 0, 0);
		rc.right	=	rc.left + m_imageSize;
		rc.bottom	=	rc.top + m_imageSize;
		ImageViewData da;
		da.rcImage	=	rc;
		da.pImageItem	=	m_imageManager.getImagePtr( i );
		this->m_imageViews.push_back( da );
	}
	int height	=	m_imageManager.getImageCount() / widthCount;
	if((m_imageManager.getImageCount() % widthCount) != 0)
		height++;
	height		-=	5;
	if(height < 0)
		this->SetScrollRange(SB_VERT, 0, 0);
	else
		this->SetScrollRange(SB_VERT, 0, height * m_imageSize);
}

#define	COLUMN_WIDTH	10

void CImageListViewDLG::OnPaint()
{
	CPaintDC dc(this);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));
	//
	RECT rc;
	GetClientRect(&rc);
	//dc.FillRect(&rc, &CBrush(RGB(0,0,0)));
	CFont* font = GetFont();
	CFont* old = dc.SelectObject(font);
	int maxRight	=	0;
	int maxBottom	=	0;
	for(size_t i=0;i<m_imageViews.size();i++)
	{
		ImageViewData& data	=	m_imageViews.at(i);
		ImageItem* pItem	=	data.pImageItem;
		if(!pItem)
			pItem		=	m_imageManager.getDefaultImage();
		CRect rcImage	=	data.rcImage;
		rcImage.top		-=	this->GetScrollPos(SB_VERT);
		rcImage.bottom	-=	this->GetScrollPos(SB_VERT);
		if(rcImage.bottom <= 0)
			continue;
		HDC dcSrc		=	pItem->img.GetDC();
		//
		::SetStretchBltMode( dc.m_hDC, COLORONCOLOR );
		::SetStretchBltMode( dcSrc, COLORONCOLOR );
		::StretchBlt(dc.m_hDC, rcImage.left, rcImage.top, rcImage.Width(), rcImage.Height(), dcSrc, 0, 0, pItem->img.GetWidth(), pItem->img.GetHeight(), SRCCOPY);
		//
		if(i == m_imageViewSelect)
			dc.Draw3dRect(&rcImage, RGB(255,0,0), RGB(255,0,0));
		else
			dc.Draw3dRect(&rcImage, RGB(0,0,0), RGB(0,0,0));
		dc.SetBkMode(TRANSPARENT);
		//
		::DrawTextA(dc.m_hDC, pItem->name, -1, &rcImage, DT_CENTER | DT_BOTTOM);
		//
		maxBottom	=	rcImage.bottom;
		maxRight	=	rcImage.right;
		pItem->img.ReleaseDC();
		//
	}
	if(maxBottom < rc.bottom)
	{
		dc.FillRect(&CRect(maxRight, maxBottom - m_imageSize, rc.right, rc.bottom), &CBrush(RGB(255,255,255)));
		dc.FillRect(&CRect(0, maxBottom, rc.right, rc.bottom), &CBrush(RGB(255,255,255)));
	}
	if(old)
		dc.SelectObject(old);
}

void CImageListViewDLG::OnBnClickedOk()
{
}

void CImageListViewDLG::OnBnClickedCancel()
{
}

void CImageListViewDLG::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialogBase::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CImageListViewDLG::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int minpos;
	int maxpos;

	GetScrollRange(SB_VERT, &minpos, &maxpos); 

	// Get the current position of scroll box.
	int curpos = GetScrollPos(SB_VERT);

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		curpos = minpos;
		break;

	case SB_RIGHT:      // Scroll to far right.
		curpos = maxpos;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if (curpos > minpos)
			curpos--;
		break;

	case SB_LINERIGHT:   // Scroll right.
		if (curpos < maxpos)
			curpos++;
		break;


	case SB_PAGEUP:
		if (curpos > minpos)
			curpos	-=	m_imageSize * 2;
		break;

	case SB_PAGEDOWN:
		if (curpos < maxpos)
			curpos	+=	m_imageSize * 2;
		break;

	case SB_THUMBPOSITION:
		curpos = nPos;
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		curpos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	// Set the new position of the thumb (scroll box).
	SetScrollPos(SB_VERT, curpos);
	this->Invalidate();

	CDialogBase::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CImageListViewDLG::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogBase::OnShowWindow(bShow, nStatus);
}

void CImageListViewDLG::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_iClickCallback && this->m_imageViewSelect >= 0)
	{
		m_iClickCallback->OnSelectImage( this->m_imageViews.at(this->m_imageViewSelect).pImageItem , m_nParameter);
	}
	this->ShowWindow(SW_HIDE);
	CDialogBase::OnLButtonDown(nFlags, point);
}

void CImageListViewDLG::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CDialogBase::OnLButtonDblClk(nFlags, point);
}

void CImageListViewDLG::OnShowMovedisable()
{
}

void CImageListViewDLG::OnUpdateShowMovedisable(CCmdUI *pCmdUI)
{
}

void CImageListViewDLG::OnMouseMove(UINT nFlags, CPoint point)
{
	static CPoint pos(0,0);
	if(pos.x == point.x && pos.y == point.y)
		return;
	pos = point;
	int oldSelect		=	m_imageViewSelect;
	m_imageViewSelect	=	-1;
	for(size_t i=0;i<this->m_imageViews.size();i++)
	{
		CRect rcImage	=	m_imageViews.at(i).rcImage;
		rcImage.top		-=	this->GetScrollPos(SB_VERT);
		rcImage.bottom	-=	this->GetScrollPos(SB_VERT);
		if(rcImage.bottom <= 0)
			continue;
		if(rcImage.PtInRect(point))
		{
			static CPoint pos(0,0);
			CPoint point2 = point;
			ClientToScreen(&point2);
			if(pos.x != point2.x || pos.y != point2.y)
			{
				this->m_imageViewSelect	=	i;
				if(m_imageViewSelect != oldSelect)
					this->Invalidate();
				ImageItem* pItem	=	m_imageViews.at(i).pImageItem;
				char buf[512];
				sprintf(buf, "%s\n%s", (const char*)pItem->name.t, (const char*)pItem->filePath.t);
				m_toolTip.UpdateTipText(W(buf), this);
				pos = point2;
			}
			break;
		}
	}
	CDialogBase::OnMouseMove(nFlags, point);
}

void CImageListViewDLG::OnKillFocus(CWnd* pNewWnd)
{
	this->ShowWindow(SW_HIDE);
	CDialogBase::OnKillFocus(pNewWnd);
}

BOOL CImageListViewDLG::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CDialogBase::OnEraseBkgnd(pDC);
}

void CImageListViewDLG::OnRButtonDown(UINT nFlags, CPoint point)
{
	this->ShowWindow(SW_HIDE);
	CDialogBase::OnRButtonDown(nFlags, point);
}

BOOL CImageListViewDLG::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(zDelta > 0)
	{
		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) - m_imageSize * 2);
	}
	else
	{
		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) + m_imageSize * 2);
	}
	this->Invalidate();
	return CDialogBase::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CImageListViewDLG::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	m_toolTip.AddTool(this, L"图片");
	return TRUE;
}
