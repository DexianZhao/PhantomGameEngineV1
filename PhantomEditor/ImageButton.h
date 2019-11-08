/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once


// CImageButton

class CImageButton : public CButton
{
	DECLARE_DYNAMIC(CImageButton)

public:
	CImageButton();
	virtual ~CImageButton();
	//
	VOID		SetImage(CImage& img, INT nImageWidthCount, INT nImageHeightCount, INT nHeightIndex, CPoint* pos = 0, BOOL bIsLeft = TRUE, BOOL bDrawBorder = false);
	VOID		SetImage(INT nHeightIndex);
	//
	BOOL		m_bDrawBorder;
	COLORREF	m_clrBorder;
protected:
	int			m_imageHeightCount;
	int			m_imageHeightIndex;
	int			m_imageWidthCount;
	CImage*		m_image;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void PreSubclassWindow();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


