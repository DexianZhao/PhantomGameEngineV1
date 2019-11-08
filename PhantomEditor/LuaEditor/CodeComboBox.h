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
#include "LuaXmlModule.h"
// CCodeComboBox
#define		CODE_TOOLTIP_ID			1
class CodeEditor;
struct	ComboItemData
{
	CString			text;
	INT				nIcon;
	COLORREF		textColor;
	COLORREF		bgColor;
	int				data;
};

class CCodeComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CCodeComboBox)
public:
	CCodeComboBox();
	virtual ~CCodeComboBox();
	VOID							SetIconRes(UINT nID);
	BOOL							Create(const CRect& rect, CWnd* wndParent, DWORD dwStyle = WS_CHILD|CBS_HASSTRINGS|CBS_DISABLENOSCROLL|WS_VSCROLL | CBS_OWNERDRAWVARIABLE|CBS_DROPDOWNLIST);
	INT								AddString(const wchar_t* szItem, INT nIcon, COLORREF bgColor = RGB(255,255,255), COLORREF textColor = RGB(0, 0, 0), int data = 0);
	VOID							DeleteString(INT nIndex);
	VOID							ResetContent();
	INT								GetText(INT nIndex, LPTSTR str);
	VOID							GetText(INT nIndex, CString& str);
	BOOL							m_bShowIndex;
	//
	BOOL							m_bFocus;
	BOOL							m_bBorder;
	CImage							m_imgVar;
	CImage							m_icon;
	std::vector<ComboItemData*>		m_items;
	CodeEditor*						m_codePtr;
public:
	VOID							SetCurSel(INT n);
	VOID							GetCodeText(CString& str);
public:
	//CCodeXml						m_codeXml;
	BOOL							m_bIsGlobal;
	BOOL							m_bFullEnum;
	BOOL							m_bFullEnumType;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLbnSelchange();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCbnSelchange();
};


