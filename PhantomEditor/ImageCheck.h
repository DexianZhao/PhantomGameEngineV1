/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once


// CImageCheck

class CImageCheck : public CButton
{
	DECLARE_DYNAMIC(CImageCheck)

public:
	CImageCheck();
	virtual ~CImageCheck();
	//
	VOID		SetImage(CImage& img, INT nImageWidthCount, INT nImageHeightCount, INT nHeightIndex, BOOL bDrawBorder = false);
	//
	BOOL		m_bDrawBorder;
	COLORREF	m_clrBorder;
	BOOL		m_bDrawTextInLeft;
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
};


