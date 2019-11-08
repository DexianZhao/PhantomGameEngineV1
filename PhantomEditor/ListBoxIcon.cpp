/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// ListBoxIcon.cpp : 实现文件
//

#include "stdafx.h"
#include "ListBoxIcon.h"


// CListBoxIcon

IMPLEMENT_DYNAMIC(CListBoxIcon, CListBox)

CListBoxIcon::CListBoxIcon()
{
	m_bShowIndex	=	false;
	m_bFocus		=	true;
	m_bBorder		=	false;
}

CListBoxIcon::~CListBoxIcon()
{
	for(int i=0;i<m_items.size();i++)
		delete m_items[i];
	m_items.clear();
}

BEGIN_MESSAGE_MAP(CListBoxIcon, CListBox)
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

VOID		CListBoxIcon::SetIconRes(UINT nID)
{
	if(!m_icon.IsNull())
		m_icon.Destroy();
	m_icon.LoadFromResource(AfxGetResourceHandle(), nID);
}

INT		CListBoxIcon::AddString(const wchar_t* szItem, INT nIcon, COLORREF bgColor, COLORREF textColor)
{
	int index	=	__super::AddString(szItem);
	ListItemData* d	=	new ListItemData();
	d->text		=	szItem;
	d->nIcon	=	nIcon;
	d->textColor	=	textColor;
	d->bgColor		=	bgColor;
	m_items.push_back(d);
	this->SetItemHeight(index, 17);
	return (m_items.size() - 1);
}
VOID	CListBoxIcon::DeleteString(INT nIndex)
{
	if(nIndex < 0 || nIndex >= m_items.size())
		return;
	__super::DeleteString(nIndex);
	delete m_items[nIndex];
	m_items.erase(m_items.begin() + nIndex);
}
VOID	CListBoxIcon::ResetContent()
{
	__super::ResetContent();
	for(int i=0;i<m_items.size();i++)
	{
		delete m_items[i];
	}
	m_items.clear();
}
INT		CListBoxIcon::GetText(INT nIndex, LPTSTR str)
{
	if(nIndex < 0 || nIndex >= m_items.size())
		return 0;
	wcscpy(str, m_items[nIndex]->text);
	return wcslen(str);
}
VOID	CListBoxIcon::GetText(INT nIndex, CString& str)
{
	if(nIndex < 0 || nIndex >= m_items.size())
		return;
	str	=	m_items[nIndex]->text;
}

// CListBoxIcon 消息处理程序

BOOL CListBoxIcon::Create(const CRect& rect, CWnd* wndParent, DWORD dwStyle)
{
	if(!__super::Create(dwStyle, rect, wndParent, 0))
		return false;
	return true;
}

void CListBoxIcon::DrawItem(LPDRAWITEMSTRUCT lpds)
{
   ASSERT(lpds->CtlType == ODT_LISTBOX);
   if(lpds->itemID < 0 || lpds->itemID > this->m_items.size())
	   return;
   ListItemData* item	=	this->m_items[lpds->itemID];
   CDC dc;
   dc.Attach(lpds->hDC);
   CFont* oldFont	=	dc.SelectObject(GetFont());
   //
   COLORREF crOldTextColor = dc.GetTextColor();
   COLORREF crOldBkColor = dc.GetBkColor();
   CRect	rcItem	=	lpds->rcItem;
   if(m_bBorder)
   {
	   rcItem.left++;
	   rcItem.right--;
   }
   if ((lpds->itemAction | ODA_SELECT) && (lpds->itemState & ODS_SELECTED))
   {
      dc.SetTextColor(item->textColor);//::GetSysColor(COLOR_HIGHLIGHTTEXT));
      dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
      dc.FillSolidRect(&rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
   }
   else
   {
	   dc.SetTextColor(item->textColor);
	   dc.FillSolidRect(&rcItem, item->bgColor);//crOldBkColor);
   }
   dc.SetBkMode(TRANSPARENT);
   //dc.Draw3dRect(&lpds->rcItem, RGB(128, 128, 128), RGB(255, 255, 255));
   CRect rcText	=	rcItem;
   int width	=	this->m_icon.GetWidth();
   int height	=	this->m_icon.GetHeight();
   if(width > 0 && height > 0)
   {
	   if(m_bShowIndex)
	   {
		   rcText.left	+=	height * 3 / 2;
		   wchar_t szIndex[128];
		   swprintf(szIndex, L"%d.", lpds->itemID + 1);
		   dc.DrawText( szIndex, wcslen(szIndex), &CRect(rcItem.left, rcItem.top, rcText.left, rcItem.bottom), DT_RIGHT|DT_SINGLELINE|DT_VCENTER);
	   }
	   if(item->nIcon >= 0)
		   m_icon.TransparentBlt(dc.m_hDC, rcText.left + 1, rcText.top + 1, height, height, item->nIcon * height, 0, height, height, RGB(77,77,77));
	   rcText.left	+=	height + 5;
   }
   if ((lpds->itemAction | ODA_FOCUS) && (lpds->itemState & ODS_FOCUS))
   {
      CBrush br(RGB(255, 0, 0));
      dc.FrameRect(&rcItem, &br);
	  //dc.DrawFocusRect(&lpds->rcItem);
   }
   //
   int left	=	rcText.left;
   dc.DrawText( item->text, item->text.GetLength(), &rcText, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
   ////
   //const wchar_t* szText = item->text;
   //const wchar_t* szLast = szText;
   //while(*szText)
   //{
	  // if(szText == L'{')
	  // {
		 //  //
		 //  CRect rc	=	rcText;
		 //  dc.DrawText( szLast, szText - szLast, &rc, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_CALCRECT);
		 //  dc.DrawText( szLast, szText - szLast, &rc, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		 //  rcText.OffsetRect(rc.Width(), 0);
		 //  szLast	=	szText + 1;
	  // }
	  // else if(szText == L'}')
	  // {
		 //  int len = szText - szLast;
		 //  if(len > 0)
		 //  {
			//   CRect rc	=	rcText;
			//   dc.DrawText( szLast, szText - szLast, &rc, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_CALCRECT);
			//   dc.DrawText( szLast, szText - szLast, &rc, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			//   rcText.OffsetRect(rc.Width(), 0);
		 //  }
		 //  else
		 //  {
			//   CRect rc	=	rcText;
			//   dc.DrawText( szText, szText - szLast, &rc, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_CALCRECT);
			//   dc.DrawText( szText, szText - szLast, &rc, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			//   rcText.OffsetRect(rc.Width(), 0);
		 //  }
		 //  break;
	  // }
   //}
   //
   dc.SetTextColor(crOldTextColor);
   dc.SetBkColor(crOldBkColor);
   if(oldFont)
	   dc.SelectObject(oldFont);
   dc.Detach();
}


BOOL CListBoxIcon::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&rc, &CBrush(RGB(255,255,255)));
	//rc.InflateRect(-1, -1);
	if(m_bBorder)
		pDC->Draw3dRect(&rc, RGB(88, 88, 88), RGB(88, 88, 88));
	return TRUE;//CListBox::OnEraseBkgnd(pDC);
}



void CListBoxIcon::MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/)
{

	// TODO:  添加您的代码以确定指定项的大小
}


void CListBoxIcon::OnSetFocus(CWnd* pOldWnd)
{
	if(this->m_bFocus)
		CListBox::OnSetFocus(pOldWnd);
	else if(pOldWnd)
		pOldWnd->SetFocus();
}
