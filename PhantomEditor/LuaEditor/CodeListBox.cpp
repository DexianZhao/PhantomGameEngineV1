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
#include "CodeListBox.h"
#include "CodeEditor.h"

// CCodeListBox

IMPLEMENT_DYNAMIC(CCodeListBox, CListBox)

	CCodeListBox::CCodeListBox()
{
	m_bShowIndex	=	false;
	m_bFocus		=	true;
	m_bBorder		=	false;
	m_bFullEnum		=	false;
	m_bIsGlobal		=	false;
	m_bIsToolTip	=	true;
}

CCodeListBox::~CCodeListBox()
{
	for(int i=0;i<m_items.size();i++)
		delete m_items[i];
	m_items.clear();
}

BEGIN_MESSAGE_MAP(CCodeListBox, CListBox)
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()
int		SortMemberProc2(const void* v1, const void* v2)
{
	CCodeMember* m1 = *(CCodeMember**)v1;
	CCodeMember* m2 = *(CCodeMember**)v2;
	if(m1->type < m2->type)
		return -1;
	if(m1->type > m2->type)
		return 1;
	return wcscmp(m1->name, m2->name);
}

VOID		AddMember(std::vector<CCodeMember*>& members, CCodeMember* m)
{
	for(int i=0;i<members.size();i++)
		if(members[i] == m)
			return;
	members.push_back(m);
}
VOID		CCodeListBox::SetIconRes(UINT nID)
{
	if(!m_icon.IsNull())
		m_icon.Destroy();
	m_icon.LoadFromResource(AfxGetResourceHandle(), nID);
}

INT		CCodeListBox::AddString(const wchar_t* szItem, INT nIcon, COLORREF bgColor, COLORREF textColor, int data)
{
	int index	=	__super::AddString(szItem);
	CodeListItemData* d	=	new CodeListItemData();
	d->text		=	szItem;
	d->nIcon	=	nIcon;
	d->textColor	=	textColor;
	d->bgColor		=	bgColor;
	d->data			=	data;
	m_items.push_back(d);
	this->SetItemHeight(index, 17);
	return (m_items.size() - 1);
}
VOID	CCodeListBox::DeleteString(INT nIndex)
{
	if(nIndex < 0 || nIndex >= m_items.size())
		return;
	__super::DeleteString(nIndex);
	delete m_items[nIndex];
	m_items.erase(m_items.begin() + nIndex);
}
VOID	CCodeListBox::ResetContent()
{
	__super::ResetContent();
	for(int i=0;i<m_items.size();i++)
	{
		delete m_items[i];
	}
	m_items.clear();
}
INT		CCodeListBox::GetText(INT nIndex, LPTSTR str)
{
	if(nIndex < 0 || nIndex >= m_items.size())
		return 0;
	wcscpy(str, m_items[nIndex]->text);
	return wcslen(str);
}
VOID	CCodeListBox::GetText(INT nIndex, CString& str)
{
	if(nIndex < 0 || nIndex >= m_items.size())
		return;
	str	=	m_items[nIndex]->text;
}

// CCodeListBox 消息处理程序

BOOL CCodeListBox::Create(const CRect& rect, CWnd* wndParent, DWORD dwStyle)
{
	if(!__super::Create(dwStyle, rect, wndParent, 0))
		return false;
	return true;
}

void CCodeListBox::DrawItem(LPDRAWITEMSTRUCT lpds)
{
	ASSERT(lpds->CtlType == ODT_LISTBOX);
	if(lpds->itemID < 0 || lpds->itemID > this->m_items.size())
		return;
	CodeListItemData* item	=	this->m_items[lpds->itemID];
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


BOOL CCodeListBox::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&rc, &CBrush(RGB(255,255,255)));
	//rc.InflateRect(-1, -1);
	if(m_bBorder)
		pDC->Draw3dRect(&rc, RGB(88, 88, 88), RGB(88, 88, 88));
	return TRUE;//CListBox::OnEraseBkgnd(pDC);
}



void CCodeListBox::MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/)
{

	// TODO:  添加您的代码以确定指定项的大小
}


void CCodeListBox::OnSetFocus(CWnd* pOldWnd)
{
	if(this->m_bFocus)
		CListBox::OnSetFocus(pOldWnd);
	else if(pOldWnd)
		pOldWnd->SetFocus();
}
BOOL		CCodeListBox::AutoPopup(CCodeClass* classPtr, const CPoint& pos, wchar_t ch)
{
	this->ResetContent();
	GetCodeXmlManager()->SortMembers();
	std::vector<CCodeMember*> members;
	m_bFullEnum		=	false;
	m_bFullEnumType	=	false;
	m_bIsString		=	false;
	if(classPtr == 0)
	{
		members = GetCodeXmlManager()->m_variants;
		for(int i=0;i<GetCodeXmlManager()->m_methods.size();i++)
			members.push_back(GetCodeXmlManager()->m_methods[i]);
		m_bIsGlobal	=	true;
	}
	else
	{
		m_bIsString			=	classPtr->name==L"string";
		m_bIsGlobal			=	false;
		if(ch == L'=')
			m_bFullEnum		=	true;
		members	=	classPtr->members;
		if(m_bFullEnum&& !classPtr->IsEnumMember())
			return false;
		//else if(!m_bFullEnum&&classPtr->IsEnumMember())
		//	return false;
		//if(m_bFullEnum)
		//{
		//	if(!classPtr->IsEnumMember())
		//	{
		//		members.clear();
		//		for(int i=0;i<xml.m_contents.size();i++)
		//		{
		//			if(xml.m_contents[i]->classPtr == classPtr)
		//			{
		//				m_bFullEnumType	=	true;
		//				members.push_back(xml.m_contents[i]);
		//			}
		//		}
		//		if(members.size() == 0)
		//			return false;
		//	}
		//}
	}
	if(members.size() > 0)
	{
		for(int j=0;j<members.size();j++)
		{
			CCodeMember* m	=	members.at(j);
			wchar_t name[256];
			wcscpy(name, m->name);
			if(m_bFullEnum&&m->classPtr&&wcsicmp(m->classPtr->name, L"bool")!=0){
				swprintf(name, L"%s.%s", m->classPtr->name.t, m->name.t);
			}
			else{
				wcscpy(name, m->name);
			}
			int index	=	AddString(name, m->type);
			this->SetItemData(index, (DWORD_PTR)m);
		}
		int height	=	this->GetCount() * this->GetItemHeight(0) + 50;
		if(height > 300)
			height	=	300;
		int width	=	100;
		CDC* dc	=	this->GetDC();
		if(dc)
		{
			CFont* oldFont	=	dc->SelectObject(GetFont());
			for(int i=0;i<GetCount();i++)
			{
				CString str;
				this->GetText(i, str);
				CRect rc(0, 0, 1000, 20);
				dc->DrawText(str, &rc, DT_LEFT | DT_TOP | DT_CALCRECT | DT_SINGLELINE);
				if(rc.right > width)
					width	=	rc.right;
			}
			if(oldFont)
				dc->SelectObject(oldFont);
			this->ReleaseDC(dc);
		}
		int bottom	=	pos.y + height;
		int top = pos.y;
		if(bottom > GetSystemMetrics(SM_CYSCREEN))
		{
			top		=	pos.y - height-20;
			bottom	=	top + height;
			//bottom	=	GetSystemMetrics(SM_CYSCREEN);
		}
		MoveWindow(&CRect(pos.x, top, pos.x + width + 50, bottom));
		this->ShowWindow(SW_SHOWNA);
		::SetWindowPos(m_hWnd, HWND_TOPMOST,0,0,0,0,SWP_NOSIZE+SWP_NOMOVE);
		this->SetCurSel(0);
		OnLbnSelchange();
		return true;
	}
	return false;
}

bool	In(wchar_t   start,   wchar_t   end,   wchar_t   code)
{
	if(code >= start && code <= end){
		return true;
	}
	return false;
}
char	convertCN(wchar_t n)
{
	wchar_t temp[2];
	temp[0] = n;
	temp[1] = 0;
	char buf[16];
	int len = ::WideCharToMultiByte(CP_ACP, 0, temp, 2, buf, 16, 0, 0);
	buf[len] = 0;
	char s=buf[0];
	buf[0]=buf[1];
	buf[1]=s;
	n = *(wchar_t*)buf;
	if(In(0xB0A1,0xB0C4,n))return 'a';
	if(In(0XB0C5,0XB2C0,n))return 'b';
	if(In(0xB2C1,0xB4ED,n))return 'c';
	if(In(0xB4EE,0xB6E9,n))return 'd';
	if(In(0xB6EA,0xB7A1,n))return 'e';
	if(In(0xB7A2,0xB8c0,n))return 'f';
	if(In(0xB8C1,0xB9FD,n))return 'g';
	if(In(0xB9FE,0xBBF6,n))return 'h';
	if(In(0xBBF7,0xBFA5,n))return 'j';
	if(In(0xBFA6,0xC0AB,n))return 'k';
	if(In(0xC0AC,0xC2E7,n))return 'l';
	if(In(0xC2E8,0xC4C2,n))return 'm';
	if(In(0xC4C3,0xC5B5,n))return 'n';
	if(In(0xC5B6,0xC5BD,n))return 'o';
	if(In(0xC5BE,0xC6D9,n))return 'p';
	if(In(0xC6DA,0xC8BA,n))return 'q';
	if(In(0xC8BB,0xC8F5,n))return 'r';
	if(In(0xC8F6,0xCBF0,n))return 's';
	if(In(0xCBFA,0xCDD9,n))return 't';
	if(In(0xCDDA,0xCEF3,n))return 'w';
	if(In(0xCEF4,0xD188,n))return 'x';
	if(In(0xD1B9,0xD4D0,n))return 'y';
	if(In(0xD4D1,0xD7F9,n))return 'z';
	return '\0';
}


VOID CCodeListBox::AutoSelect(const wchar_t* szSel)
{
	wchar_t buf[512];
	int select	=	-1;
	int findCount	=	wcslen(szSel);
	if(findCount > 0)
	{
		for(int i=0;i<this->GetCount();i++)
		{
			this->GetText(i, buf);
			wcslwr(buf);
			bool bFind	=	true;
			for(int j=0;j<findCount;j++)
			{
				wchar_t ch = buf[j];
				if(!isascii(ch))
					ch=convertCN(ch);
				if(szSel[j] != ch)
				{
					bFind	=	false;
					break;
				}
			}
			if(bFind)
			{
				if(select < 0)
					select	=	i;
				if(findCount == wcslen(buf))
				{
					select	=	i;
					break;
				}
			}
		}
	}
	this->SetCurSel(select);
}

void CCodeListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCodeListBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_RETURN || nChar == VK_SPACE)
	{
		ShowWindow(SW_HIDE);
	}
	CListBox::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CCodeListBox::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CListBox::OnShowWindow(bShow, nStatus);
}

VOID CCodeListBox::GetCodeText(CString& str)
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
		if(m->m_bIsModule)
			str = L".";
		else
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

VOID CCodeListBox::SetCurSel(INT n)
{
	__super::SetCurSel(n);
	OnLbnSelchange();
}

void CCodeListBox::OnLbnSelchange()
{
	int sel	=	GetCurSel();
	if(sel >= 0)
	{
		if(m_bIsToolTip){
			CCodeMember* m	=	(CCodeMember*)GetItemData(GetCurSel());
			CRect rc;
			GetWindowRect(&rc);
			m_codePtr->SetMemberToolTip(m, CPoint(rc.right, rc.top), false);
		}
		else
		{
			CString str;
			this->GetText(sel, str);
			m_codePtr->OnComboSelect(this, str, this->m_items[sel]->data);
		}
	}
}


void CCodeListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonDblClk(nFlags, point);
	if(GetCurSel() >= 0&&m_bIsToolTip)
	{
		m_codePtr->SelectFromListBox(0);
	}
}


void CCodeListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonDown(nFlags, point);
	for(int i=0;i<this->GetCount();i++)
	{
		CRect rc;
		this->GetItemRect(i, &rc);
		if(rc.PtInRect(point))
		{
			this->SetCurSel(i);
			break;
		}
	}
	OnLbnSelchange();
}
