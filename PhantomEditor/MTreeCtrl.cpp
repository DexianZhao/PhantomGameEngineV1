/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// MTreeCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "MTreeCtrl.h"

// CMTreeCtrl

IMPLEMENT_DYNAMIC(CMTreeCtrl, CTreeCtrl)

CMTreeCtrl::CMTreeCtrl()
{
	m_bIsSelected	=	false;
	m_selectPt.SetPoint(0, 0);
	m_bSelectInvalidate	=	false;
	m_bLButtonDown	=	false;
	for(int i=0;i<MAX_FRAME_COLOR;i++)
		m_textColor[i]	=	0x00ffffff;
	m_nNumFrame		=	0;
}

CMTreeCtrl::~CMTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMTreeCtrl, CTreeCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, &CMTreeCtrl::OnTvnDeleteitem)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CMTreeCtrl::OnTvnSelchanged)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CMTreeCtrl 消息处理程序


BOOL CMTreeCtrl::IsSelected(HTREEITEM hItem)
{
	for(int i=0;i<m_selects.size();i++)
	{
		if(m_selects[i] == hItem)
			return true;
	}
	return false;
}

VOID CMTreeCtrl::SetFrameTextColor(INT nIndex, COLORREF clrText)
{
	if(nIndex < 0 || nIndex >= MAX_FRAME_COLOR)
		return;
	this->m_textColor[nIndex]	=	clrText;
}

VOID CMTreeCtrl::SetFrameBitmap(UINT nRes, INT nNumFrame)
{
	if(!m_imgFrame.IsNull())
		m_imgFrame.Destroy();
	m_nNumFrame	=	nNumFrame;
	m_imgFrame.LoadFromResource(AfxGetResourceHandle(), nRes);
}

VOID CMTreeCtrl::AddSelect(HTREEITEM hItem, BOOL bExpand)
{
	for(int i=0;i<m_selects.size();i++)
		if(m_selects[i] == hItem)
			return;
	m_selects.push_back(hItem);
	if(bExpand)
	{
		HTREEITEM hmyItem	=	hItem;
		while(hItem && (hItem != TVI_ROOT))
		{
			hItem	=	this->GetParentItem(hItem);
			if(hItem && hItem != TVI_ROOT)
			{
				BOOL bExpanded	=	(this->GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) == 0;
				if(!bExpanded)
				{
					this->Expand(hItem, TVE_EXPAND);
					EnsureVisible(hmyItem);
				}
			}
			else
				break;
		}
	}
	this->Invalidate();
}

VOID CMTreeCtrl::RemoveSelect(HTREEITEM hItem)
{
	for(int i=0;i<m_selects.size();i++)
		if(m_selects[i] == hItem)
		{
			m_selects.erase(m_selects.begin() + i);
			return;
		}
}

VOID CMTreeCtrl::SetIconImage(UINT nIDRes)
{
	if(!m_imageIcon.IsNull())
		m_imageIcon.Destroy();
	m_imageIcon.LoadFromResource(AfxGetInstanceHandle(), nIDRes);
}

VOID	DrawLine(CDC& dc, CPoint& begin, CPoint& end, CPen& pen)
{
	CPen* old	=	dc.SelectObject(&pen);
	dc.MoveTo(begin);
	dc.LineTo(end);
	if(old)
		dc.SelectObject(old);
}

VOID CMTreeCtrl::GetCrossRect(HTREEITEM hItem, CRect& rc)
{
	GetItemRect(hItem, &rc, true);
	rc.right	=	rc.left;
	rc.left		=	rc.left	-	16;
	rc.InflateRect(-4,-4);
	rc.right++;
	rc.bottom++;
	rc.OffsetRect(-4, -1);
	//
}

VOID CMTreeCtrl::DrawCross(CDC& dc, HTREEITEM hItem)
{
	if(this->GetChildItem(hItem) == 0)
		return;
	BOOL bExpand	=	(this->GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) == 0;
	CRect rc;
	GetCrossRect(hItem, rc);
	dc.Draw3dRect(&rc, RGB(150, 150, 150), RGB(150, 150, 150));
	rc.right--;
	rc.bottom--;
	CPen pen(PS_SOLID, 1, RGB(50,50,150));
	::DrawLine(dc, CPoint(rc.left + 2, rc.top + rc.Height() / 2), CPoint(rc.right - 1, rc.top + rc.Height() / 2), pen);
	if(bExpand)
		::DrawLine(dc, CPoint(rc.left + rc.Width() / 2, rc.top + 2), CPoint(rc.left + rc.Width() / 2, rc.bottom - 1), pen);
}

VOID CMTreeCtrl::DrawLine(CDC& dc, HTREEITEM hItem)
{
	HTREEITEM hParent	=	this->GetParentItem(hItem);
	BOOL bFirst		=	false;
	if(hParent)
		bFirst		=	(GetChildItem(hParent) == hItem);
	BOOL bHasChild	=	(GetChildItem(hItem) != 0);
	BOOL bExpand	=	(this->GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) == 0;
	CRect itemRc;
	GetItemRect(hItem, &itemRc, FALSE);
	//
	CRect rc, parentRc;
	GetCrossRect(hItem, rc);
	GetCrossRect(hParent, parentRc);
	CPen pen(PS_SOLID, 1, RGB(150,150,150));
	//if(bFirst)
	INT nTop	=	rc.top;
	INT nBottom	=	rc.bottom;
	INT nRight	=	rc.right;
	if(!bHasChild)
	{
		nTop	=	rc.top + rc.Height() / 2;
		nBottom	=	nTop - 1;
		//
		nRight	+=	5;
		::DrawLine(dc, CPoint(rc.left + rc.Width() / 2, rc.top + rc.Height() / 2), CPoint(nRight, nTop), pen);
	}
	else
		::DrawLine(dc, CPoint(rc.right, rc.top + rc.Height() / 2), CPoint(nRight + 5, rc.top + rc.Height() / 2), pen);

	{
		::DrawLine(dc, CPoint(rc.left + rc.Width() / 2, nTop), CPoint(rc.left + rc.Width() / 2, itemRc.top - 1), pen);
	}
	HTREEITEM hItemNext	=	this->GetNextSiblingItem(hItem);
	if(hItemNext)
	{
		CRect nextRc;
		GetItemRect(hItemNext, &nextRc, FALSE);
		::DrawLine(dc, CPoint(rc.left + rc.Width() / 2, nBottom), CPoint(rc.left + rc.Width() / 2, nextRc.top), pen);
	}
}

VOID	BitBlt(HDC dcDest, INT x, INT y, INT cx, INT cy, HDC dcSrc, INT srcX, INT srcY, const CSize& clipRect)
{
	CRect rcDraw(x, y, x + cx, y + cy);
	if(rcDraw.right < 0 || rcDraw.bottom < 0 || rcDraw.left > clipRect.cx || rcDraw.top > clipRect.cy)
		return;
	if(rcDraw.left < 0)
	{
		srcX			-=	rcDraw.left;
		//rcDraw.right	+=	rcDraw.left;
		rcDraw.left		=	0;
	}
	if(rcDraw.top < 0)
	{
		srcY			-=	rcDraw.top;
		//rcDraw.bottom	+=	rcDraw.top;
		rcDraw.top		=	0;
	}
	if(rcDraw.right > clipRect.cx)
		rcDraw.right	=	clipRect.cx;
	if(rcDraw.bottom > clipRect.cy)
		rcDraw.bottom	=	clipRect.cy;
	if(rcDraw.Width() == 0 || rcDraw.Height() == 0)
		return;
	BitBlt(dcDest, rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height(), dcSrc, srcX, srcY, SRCCOPY);
}

VOID	RenderPlotFrame(HDC dcDest, const CRect& rcDest, CImage& img, const CRect& imgRect, INT nWrapWidth, const CSize& clipSize)
{
	int imgWidth	=	imgRect.Width();
	int imgHeight	=	imgRect.Height();
	HDC dcSrc		=	img.GetDC();
	//
	CRect	rcLeft(0, 0, nWrapWidth, imgHeight);
	CRect	rcRight(imgWidth - nWrapWidth, 0, imgWidth, imgHeight);
	CRect	rcCenter(nWrapWidth, 0, imgWidth - nWrapWidth, imgHeight);
	rcLeft.OffsetRect(imgRect.left, imgRect.top);
	rcRight.OffsetRect(imgRect.left, imgRect.top);
	rcCenter.OffsetRect(imgRect.left, imgRect.top);
	//
	int nCount;
	//center
	nCount	=	rcDest.Width() / rcCenter.Width() + 1;
	for(int i=0;i<nCount;i++)
	{
		int left	=	rcDest.left + i * rcCenter.Width();
		int	right	=	left + rcCenter.Width();
		if(right > rcDest.right)
			right	=	rcDest.right;
		int width	=	right - left;
		//
		if(width > 0)
			BitBlt(dcDest, left, rcDest.top, width, rcCenter.Height(), dcSrc, rcCenter.left, rcCenter.top, clipSize);
		else
			break;
	}
	BitBlt(dcDest, rcDest.left, rcDest.top, rcLeft.Width(), rcLeft.Height(), dcSrc, rcLeft.left, rcLeft.top, clipSize);
	BitBlt(dcDest, rcDest.right - rcRight.Width(), rcDest.top, rcRight.Width(), rcRight.Height(), dcSrc, rcRight.left, rcRight.top, clipSize);
	img.ReleaseDC();
}

void CMTreeCtrl::OnPaint()
{
	CPaintDC dcPaint(this);
	CRect clientRC;
	GetClientRect(&clientRC);
	if(m_background.IsNull())
		m_background.Create(clientRC.Width(), clientRC.Height(), 24);
	//
	CDC dc;
	dc.Attach(m_background.GetDC());
	dc.FillRect(&clientRC, &CBrush(RGB(255,255,255)));
	CFont* oldFont	=	dc.SelectObject(CFont::FromHandle(m_hFont));
	HTREEITEM hItem	=	this->GetFirstVisibleItem();
	
	const int iconWidth	=	16;
	int iconWC		=	m_imageIcon.GetWidth() / iconWidth;
	INT nImageCount	=	(iconWC) * (m_imageIcon.GetHeight() / iconWidth);
	HTREEITEM hPrev	=	this->GetPrevSiblingItem(hItem);
	if(hPrev)
		DrawLine(dc, hPrev);
	while(hItem != NULL)
	{
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(0,0,0));
		CString str = this->GetItemText(hItem);
		CRect rectText, rectItem;
		this->GetItemRect(hItem, &rectText, TRUE);
		this->GetItemRect(hItem, &rectItem, FALSE);
		rectItem.left	=	rectText.left;
		BOOL	bSelect	=	this->IsSelected(hItem);
		int imgNormal = -1;
		int imgSelect = -1;
		BOOL bImage	=	this->GetItemImage(hItem, imgNormal, imgSelect);
		if(bImage && m_nNumFrame > 0 && imgSelect >= 0)
		{
			if(!m_imgFrame.IsNull() && imgSelect < TreeImageNone)
			{
				int nHeight	=	m_imgFrame.GetHeight() / m_nNumFrame;
				int nIndex	=	(imgSelect % m_nNumFrame);
				RenderPlotFrame(dc.m_hDC, CRect(rectItem.left + 16, rectItem.top, rectItem.right, rectItem.bottom), m_imgFrame, CRect(0, nIndex * nHeight, m_imgFrame.GetWidth(), (nIndex + 1) * nHeight), 5, CSize(clientRC.Width(), clientRC.Height()));
				dc.SetTextColor(this->m_textColor[nIndex]);
			}
		}
		if(bSelect)
		{
			if(!m_imgFrame.IsNull())
			{
				dc.Draw3dRect(CRect(rectItem.left + 16, rectItem.top, rectItem.right, rectItem.bottom), RGB(250,160,160), RGB(250,160,160));
				rectItem.InflateRect(-1,-1);
				FillRect(dc.m_hDC, &CRect(1, rectItem.top, 17, rectItem.bottom), GetSysColorBrush(COLOR_HIGHLIGHT));//dc.DrawFocusRect(CRect(rectItem.left + 16, rectItem.top, rectItem.right, rectItem.bottom));
			}
			else
			{
				dc.Draw3dRect(rectItem, RGB(160,160,160), RGB(160,160,160));
				rectItem.InflateRect(-1,-1);
				dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				FillRect(dc.m_hDC, &rectItem, GetSysColorBrush(COLOR_HIGHLIGHT));
			}
		}
		DrawCross(dc, hItem);
		if(bImage)
		{
			if(imgNormal >= 0 && imgNormal < TreeImageNone)//m_imageIcon.GetWidth())
			{
				BOOL bInfo	=	FALSE;
				CRect iconRC;
				//if(bSelect)
				//	imgNormal	=	imgSelect;
				iconRC.SetRect(imgNormal * iconWidth, (imgNormal / iconWC) * iconWidth, 0, 0);
				iconRC.right	=	iconRC.left + iconWidth;
				iconRC.bottom	=	iconRC.top + iconWidth;
				{
					HDC dcImage	=	m_imageIcon.GetDC();
					TransparentBlt(dc.m_hDC, rectText.left, rectText.top, iconWidth, iconWidth, dcImage, iconRC.left, iconRC.top, iconWidth, iconWidth, RGB(1,1,1));
					m_imageIcon.ReleaseDC();
				}
			}
			rectText.OffsetRect(iconWidth + 2, 0);
		}
		DrawLine(dc, hItem);
		//
		HTREEITEM hPrev	=	this->GetPrevSiblingItem(hItem);
		if(hPrev)
			DrawLine(dc, hPrev);
		rectText.right	+=	100;
		dc.DrawText(str, str.GetLength(), &rectText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		hItem	=	this->GetNextVisibleItem(hItem);
	}
	//
	if(oldFont)
		dc.SelectObject(oldFont);
	dcPaint.BitBlt(0, 0, clientRC.Width(), clientRC.Height(), &dc, 0, 0, SRCCOPY);
	if(m_bSelectInvalidate)
	{
		dcPaint.Draw3dRect(&this->m_selectRect, RGB(50,50,50), RGB(50,50,50));
		m_bSelectInvalidate	=	false;
	}
	m_background.ReleaseDC();
	dc.Detach();
}


BOOL CMTreeCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CTreeCtrl::OnEraseBkgnd(pDC);
}


void CMTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	if(!m_background.IsNull())
		m_background.Destroy();
	CTreeCtrl::OnSize(nType, cx, cy);
}


void CMTreeCtrl::OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	RemoveSelect(pNMTreeView->itemNew.hItem);
	this->Invalidate();
	*pResult = 0;
}


void CMTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if(GetCopyKeyboard())
	{
	}
	else
	{
	}
	*pResult = 0;
}

VOID CMTreeCtrl::CheckSelect(CPoint point)
{
	HTREEITEM hItem		=	this->GetFirstVisibleItem();
	HTREEITEM hSelect	=	0;
	while(hItem != NULL)
	{
		CRect itemRect;
		this->GetItemRect(hItem, &itemRect, FALSE);
		if(itemRect.PtInRect(point))
		{
			hSelect	=	hItem;
			break;
		}
		hItem	=	this->GetNextVisibleItem(hItem);
	}
	{
		BOOL bShift	=	HIBYTE(GetKeyState(VK_SHIFT));//GetCopyKeyboard();
		BOOL bCtrl	=	HIBYTE(GetKeyState(VK_CONTROL));//GetSelectKeyboard();
		if(bCtrl)
		{
			if(hSelect)
			{
				if(this->IsSelected(hSelect))
				{
					this->RemoveSelect(hSelect);
					this->OnChangeSelect(NULL);
				}
				else
				{
					this->AddSelect(hSelect);
					this->OnChangeSelect(hSelect);
				}
			}
		}
		else if(bShift)
		{
			if(hSelect)
			{
				HTREEITEM hItemSel	=	0;
				if(m_selects.size() > 0)
					hItemSel	=	m_selects[0];
				std::vector<HTREEITEM> sels;
				BOOL bFind	=	false;
				{
					HTREEITEM hNext	=	hSelect;
					while(hItemSel)
					{
						sels.push_back(hItem);
						if(hItemSel == hNext)
						{
							bFind	=	true;
							break;
						}
						if(AddAllChilds(sels, hItemSel, hNext))
						{
							bFind	=	true;
							break;
						}
						hItemSel = this->GetNextSiblingItem(hItemSel);
					}
				}
				//
				this->m_selects.clear();
				if(sels.size() > 0 && bFind)
				{
					for(int i=0;i<sels.size();i++)
						this->AddSelect(sels[i]);
					this->OnChangeSelect(sels[0]);
				}
				else
				{
					this->AddSelect(hSelect);
					this->OnChangeSelect(hSelect);
				}
			}
		}
		else
		{
			this->m_selects.clear();
			if(hSelect)
			{
				this->AddSelect(hSelect);
				this->OnChangeSelect(hSelect);
			}
		}
		this->Invalidate();
	}
}

void CMTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->CheckSelect(point);
	SetCapture();
	m_selectPt	=	point;
	m_bLButtonDown	=	true;
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

BOOL CMTreeCtrl::AddAllChilds(std::vector<HTREEITEM>& sels, HTREEITEM hItem, HTREEITEM hSelect)
{
	sels.push_back(hItem);
	if((this->GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) == 0)
		return false;
	HTREEITEM hChild	=	this->GetChildItem(hItem);
	while(hChild)
	{
		sels.push_back(hChild);
		if(hSelect == hChild)
			return true;
		hChild	=	this->GetNextSiblingItem(hChild);
	}
	return false;
}


void CMTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//this->CheckSelect(point);
	if(this->m_selects.size() > 0)
		OnDoubleClick(m_selects[0]);
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}


void CMTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if(nFlags == MK_LBUTTON && m_bLButtonDown)
	{
		m_selectRect.SetRect(min(point.x, m_selectPt.x), min(point.y, m_selectPt.y), max(point.x, m_selectPt.x), max(point.y, m_selectPt.y));
		this->m_bSelectInvalidate	=	true;
		this->m_bIsSelected	=	true;
		this->Invalidate();
	}
	CTreeCtrl::OnMouseMove(nFlags, point);
}


void CMTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	this->Invalidate();
	m_bLButtonDown	=	false;
	if(m_bIsSelected)
	{
		m_bIsSelected	=	false;
		std::vector<HTREEITEM> sels;
		HTREEITEM hNext	=	this->GetFirstVisibleItem();
		while(hNext)
		{
			sels.push_back(hNext);
			hNext = this->GetNextVisibleItem(hNext);
		}
		//
		if(sels.size() > 0)
		{
			std::vector<HTREEITEM> inRects;
			for(int i=0;i<sels.size();i++)
			{
				CRect rectText;
				this->GetItemRect(sels[i], &rectText, TRUE);
				const int iconWidth = 16;
				rectText.OffsetRect(iconWidth + 2, 0);
				CPoint p1(rectText.left, rectText.top);
				CPoint p2(rectText.right, rectText.top);
				CPoint p3(rectText.left, rectText.bottom);
				CPoint p4(rectText.right, rectText.bottom);
				if(m_selectRect.PtInRect(p1) || m_selectRect.PtInRect(p2) || m_selectRect.PtInRect(p3) || m_selectRect.PtInRect(p4))
					inRects.push_back(sels[i]);
			}
			if(inRects.size() > 0)
			{
				this->m_selects.clear();
				for(int i=0;i<inRects.size();i++)
					this->AddSelect(inRects[i]);
				this->OnChangeSelect(inRects[0]);
			}
		}
		this->Invalidate();
	}
	CTreeCtrl::OnLButtonUp(nFlags, point);
}
