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
#include "CodeComboBox.h"
#include "CodeEditor.h"
// CCodeComboBox

IMPLEMENT_DYNAMIC(CCodeComboBox, CComboBox)

CCodeComboBox::CCodeComboBox()
{
	m_bShowIndex	=	false;
	m_bFocus		=	true;
	m_bBorder		=	true;
	m_bFullEnum	=	false;
	m_bIsGlobal	=	false;
}

CCodeComboBox::~CCodeComboBox()
{
	for(int i=0;i<m_items.size();i++)
		delete m_items[i];
	m_items.clear();
}

BEGIN_MESSAGE_MAP(CCodeComboBox, CComboBox)
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CCodeComboBox::OnCbnSelchange)
END_MESSAGE_MAP()

VOID		CCodeComboBox::SetIconRes(UINT nID)
{
	if(!m_icon.IsNull())
		m_icon.Destroy();
	m_icon.LoadFromResource(AfxGetResourceHandle(), nID);
}

INT		CCodeComboBox::AddString(const wchar_t* szItem, INT nIcon, COLORREF bgColor, COLORREF textColor, int data)
{
	int index	=	__super::AddString(szItem);
	ComboItemData* d	=	new ComboItemData();
	d->text		=	szItem;
	d->nIcon	=	nIcon;
	d->textColor	=	textColor;
	d->bgColor		=	bgColor;
	d->data		=	data;
	m_items.push_back(d);
	this->SetItemHeight(index, 17);
	return (m_items.size() - 1);
}
VOID	CCodeComboBox::DeleteString(INT nIndex)
{
	if(nIndex < 0 || nIndex >= m_items.size())
		return;
	__super::DeleteString(nIndex);
	delete m_items[nIndex];
	m_items.erase(m_items.begin() + nIndex);
}
VOID	CCodeComboBox::ResetContent()
{
	__super::ResetContent();
	for(int i=0;i<m_items.size();i++)
	{
		delete m_items[i];
	}
	m_items.clear();
}
INT		CCodeComboBox::GetText(INT nIndex, LPTSTR str)
{
	if(nIndex < 0 || nIndex >= m_items.size())
		return 0;
	wcscpy(str, m_items[nIndex]->text);
	return wcslen(str);
}
VOID	CCodeComboBox::GetText(INT nIndex, CString& str)
{
	if(nIndex < 0 || nIndex >= m_items.size())
		return;
	str	=	m_items[nIndex]->text;
}

// CCodeComboBox 消息处理程序

BOOL CCodeComboBox::Create(const CRect& rect, CWnd* wndParent, DWORD dwStyle)
{
	if(!__super::Create(dwStyle, rect, wndParent, 0))
		return false;
	return true;
}

void CCodeComboBox::DrawItem(LPDRAWITEMSTRUCT lpds)
{
   ASSERT(lpds->CtlType == ODT_COMBOBOX);
   if(lpds->itemID < 0 || lpds->itemID > this->m_items.size())
	   return;
   ComboItemData* item	=	this->m_items[lpds->itemID];
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


BOOL CCodeComboBox::OnEraseBkgnd(CDC* pDC)
{
	//CRect rc;
	//GetClientRect(&rc);
	//pDC->FillRect(&rc, &CBrush(RGB(255,255,255)));
	////rc.InflateRect(-1, -1);
	//if(m_bBorder)
	//	pDC->Draw3dRect(&rc, RGB(88, 88, 88), RGB(88, 88, 88));
	return false;//CComboBox::OnEraseBkgnd(pDC);
}



void CCodeComboBox::MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/)
{

	// TODO:  添加您的代码以确定指定项的大小
}


void CCodeComboBox::OnSetFocus(CWnd* pOldWnd)
{
	if(this->m_bFocus)
		CComboBox::OnSetFocus(pOldWnd);
	else if(pOldWnd)
		pOldWnd->SetFocus();
}

void CCodeComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCodeComboBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_RETURN || nChar == VK_SPACE)
	{
		ShowWindow(SW_HIDE);
	}
	CComboBox::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CCodeComboBox::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CComboBox::OnShowWindow(bShow, nStatus);
}

VOID CCodeComboBox::GetCodeText(CString& str)
{
	CString s;
	GetText(GetCurSel(), s);
	CCodeMember* m	=	(CCodeMember*)GetItemData(GetCurSel());
	if(m_bIsGlobal)
	{
		str	=	s;
		return;
	}
	switch(m->type)
	{
	case CCodeMember::CodeMemberType_Void:
		str	=	L":";
		break;
	case CCodeMember::CodeMemberType_Variant:
		str	=	L".";
		break;
	case CCodeMember::CodeMemberType_Enum_Value:
		{
			if(m_bFullEnum)
				str	=	L"=";
			else
				str	=	L".";
		}
		break;
	case CCodeMember::CodeMemberType_Struct:
	case CCodeMember::CodeMemberType_Class:
	case CCodeMember::CodeMemberType_Module:
		break;
	}
	if(m_bFullEnumType)
		str	=	L"=";
	str	+=	s;
}

VOID CCodeComboBox::SetCurSel(INT n)
{
	__super::SetCurSel(n);
	OnLbnSelchange();
}

void CCodeComboBox::OnLbnSelchange()
{
	int sel	=	GetCurSel();
	if(sel >= 0)
	{
		CCodeMember* m	=	(CCodeMember*)GetItemData(GetCurSel());
		CRect rc;
		GetWindowRect(&rc);
		//m_codePtr->SetMemberToolTip(m, CPoint(rc.right, rc.top));
	}
}


void CCodeComboBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CComboBox::OnLButtonDblClk(nFlags, point);
	if(GetCurSel() >= 0)
	{
		//m_codePtr->SelectFromListBox();
	}
}


void CCodeComboBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CComboBox::OnLButtonDown(nFlags, point);
}


void CCodeComboBox::OnCbnSelchange()
{
	int sel = this->GetCurSel();
	if(sel<0)return;
	CString str;
	this->GetText(sel, str);
	this->m_codePtr->OnComboSelect(this, str, this->m_items[sel]->data);
}
