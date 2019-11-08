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
#include "GCLanguage.h"
#include <vector>
#include <string>

struct GCImageData
{
	INT				nResourceID;
	CImage			vImage;
	INT				nImageID;		//在CImageList中
	INT				nImageIndex;	//在CImageList中
	CString			strFile;
	GCImageData(){nImageID = -1; nResourceID = -1; nImageIndex = -1; }
};

enum{
	FONT_MODE_NORMAL	=	1,
	FONT_MODE_BOLD,
};

struct GCFontData
{
	wchar_t					fontName[32];
	INT						nHeight;
	BOOL					bBold;
	BOOL					bUnderline;
	CFont					font;
	INT						nID;
};

struct GCCursorData
{
	HCURSOR					hCursor;
	INT						nID;
	LPCWSTR					szWID;
	GCCursorData(){hCursor = 0; nID = 0; szWID = 0;}
};

#define	TREE_IMAGE_HEIGHT		17

class GCBitmapMgr
{
public:
	GCBitmapMgr(void);
	~GCBitmapMgr(void);
	CImage*					GetImageByResID(INT nResID);
	INT						GetIndexByResID(INT nResID);
	//
	CImage*					GetImageByFile(LPCTSTR szFile);
	VOID					AddFromFile(LPCTSTR szFile);	//从文件加入
	CImage*					LoadFromResToAtlImage(INT nResourceID);
	INT						LoadFromResToImageList(INT nResourceID);
	INT						LoadFromResToImageTreeList(INT nResourceID);
	//
	VOID					LoadAllResources();
	//
	CImageList*				GetImageListPtr(){return &m_imageList;}
	CImageList*				GetImageListTreePtr(){return &m_imageListTree;}
	//
	CFont*					GetFontPtr(const wchar_t* szFontName, INT nHeight, BOOL bBold, BOOL bUnderline, INT nID);
	CFont*					GetFontPtr(INT nMode);
	const wchar_t*			GetFontFace(INT nMode);
	VOID					SetCurrentCursor(INT nResID);
	VOID					SetCurrentCursor(LPCWSTR szResID);
protected:
	std::vector<GCImageData*>	m_images;
	std::vector<GCFontData*>	m_fonts;
	std::vector<GCCursorData>	m_cursors;
	std::vector<GCImageData*>	m_treeImages;
	CImageList				m_imageList;//全局唯一
	CImageList				m_imageListTree;//全局唯一
public:
};

VOID			WrapWidthDraw(HDC dest, const CRect& rcDest, HDC dcSrc, const CRect& rcSrc, int wrapWidth);
VOID			WrapWidthDraw(HDC dest, const CRect& rcDest, CImage& img, int wrapWidth);

VOID			FillWidth(HDC dest, const CRect& rcDest, CImage& img, int wrapWidth = 0);
VOID			FillWidth(HDC dest, const CRect& rcDest, HDC dcSrc, const CRect& rcSrc, int wrapWidth = 0);
VOID			FillHeight(HDC dest, const CRect& rcDest, CImage& img);
VOID			FillHeight(HDC dest, const CRect& rcDest, HDC dcSrc, const CRect& rcSrc);

VOID			FillBackground(HDC dest, const CRect& rcDest, HDC dcSrc, const CRect& rcSrc, int wrapWidth = 0, int wrapHeight = 0);
VOID			FillBackground(HDC dest, const CRect& rcDest, CImage& img, int wrapWidth = 0, int wrapHeight = 0);

GCBitmapMgr*	GetBitmapMgr();
