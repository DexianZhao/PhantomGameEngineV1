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


// CMTreeCtrl

#define	MAX_FRAME_COLOR		255
#define	TreeImageNone		1000

class CMTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CMTreeCtrl)

public:
	CMTreeCtrl();
	virtual ~CMTreeCtrl();

	VOID					SetIconImage(UINT nIDRes);
	CImage					m_background;
	HFONT					m_hFont;
	//
	BOOL					IsSelected(HTREEITEM hItem);
	VOID					AddSelect(HTREEITEM hItem, BOOL bExpand = FALSE);
	VOID					RemoveSelect(HTREEITEM hItem);
	INT						GetSelectCount(){return m_selects.size();}
	HTREEITEM				GetSelect(INT index){if(index < 0 || index >= (int)m_selects.size())return 0; return m_selects[index];}
	VOID					ClearSelect(){m_selects.clear();}
	virtual	VOID			OnChangeSelect(HTREEITEM hItem){}
	virtual	VOID			OnDoubleClick(HTREEITEM hItem){}
	VOID					SetFrameBitmap(UINT nRes, INT nNumFrame);
	VOID					SetFrameTextColor(INT nIndex, COLORREF clrText);
protected:
	VOID					DrawCross(CDC& dc, HTREEITEM hItem);
	VOID					DrawLine(CDC& dc, HTREEITEM hItem);
	VOID					GetCrossRect(HTREEITEM hItem, CRect& rc);
	VOID					CheckSelect(CPoint pt);
	//
	BOOL					AddAllChilds(std::vector<HTREEITEM>& sels, HTREEITEM hItem, HTREEITEM hSelect);
	std::vector<HTREEITEM>	m_selects;
	CImage					m_imageIcon;
	CPoint					m_selectPt;
	CRect					m_selectRect;
	BOOL					m_bSelectInvalidate;
	BOOL					m_bIsSelected;
	bool					m_bLButtonDown;
	CImage					m_imgFrame;
	INT						m_nNumFrame;
	COLORREF				m_textColor[MAX_FRAME_COLOR];	//
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


