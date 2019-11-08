/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include <atlimage.h>
#include <vector>

// CListBoxIcon

struct	ListItemData
{
	CString			text;
	INT				nIcon;
	COLORREF		textColor;
	COLORREF		bgColor;
};

class CListBoxIcon : public CListBox
{
	DECLARE_DYNAMIC(CListBoxIcon)
public:
	CListBoxIcon();
	virtual ~CListBoxIcon();
	VOID						SetIconRes(UINT nID);
	//
	BOOL						Create(const CRect& rect, CWnd* wndParent, DWORD dwStyle = WS_CHILD|LBS_HASSTRINGS|WS_BORDER|LBS_DISABLENOSCROLL|WS_VSCROLL | LBS_OWNERDRAWVARIABLE);
	INT							AddString(const wchar_t* szItem, INT nIcon, COLORREF bgColor = RGB(255,255,255), COLORREF textColor = RGB(0, 0, 0));
	VOID						DeleteString(INT nIndex);
	VOID						ResetContent();
	INT							GetText(INT nIndex, LPTSTR str);
	VOID						GetText(INT nIndex, CString& str);
	BOOL						m_bShowIndex;
	//
	BOOL						m_bFocus;
	BOOL						m_bBorder;
protected:
	CImage						m_icon;
	std::vector<ListItemData*>	m_items;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


