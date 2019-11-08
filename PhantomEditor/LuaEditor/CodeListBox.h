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
// CCodeListBox
#define		CODE_TOOLTIP_ID			1
class CodeEditor;
struct	CodeListItemData
{
	CString			text;
	INT				nIcon;
	COLORREF		textColor;
	COLORREF		bgColor;
	int				data;
};

class CCodeListBox : public CListBox
{
	DECLARE_DYNAMIC(CCodeListBox)
public:
	CCodeListBox();
	virtual ~CCodeListBox();
	VOID							SetIconRes(UINT nID);
	BOOL							Create(const CRect& rect, CWnd* wndParent, DWORD dwStyle = WS_CHILD|LBS_HASSTRINGS|WS_BORDER|LBS_DISABLENOSCROLL|WS_VSCROLL | LBS_OWNERDRAWVARIABLE);
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
	std::vector<CodeListItemData*>		m_items;
	BOOL							m_bIsToolTip;
	BOOL							m_bIsString;
public:
	VOID							AutoSelect(const wchar_t* szSel);
	//
	BOOL							AutoPopup(CCodeClass* classPtr, const CPoint& pos, wchar_t ch);
	//
	VOID							SetCurSel(INT n);
	VOID							GetCodeText(CString& str);
public:
	//CCodeXml						m_codeXml;
	BOOL							m_bIsGlobal;
	BOOL							m_bFullEnum;
	BOOL							m_bFullEnumType;
	//
	CodeEditor*						m_codePtr;
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
};


