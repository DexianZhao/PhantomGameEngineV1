/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "GCBitmapMgr.h"


GCBitmapMgr*	g_pBitmapManager = 0;
GCBitmapMgr*	GetBitmapMgr()
{
	return g_pBitmapManager;
}

GCBitmapMgr::GCBitmapMgr(void)
{
	g_pBitmapManager	=	this;
	m_imageList.Create(20, 20, ILC_COLOR24, 1, 1);
	m_imageListTree.Create(TREE_IMAGE_HEIGHT, TREE_IMAGE_HEIGHT, ILC_COLOR24, 1, 1);
	//
	LoadAllResources();
}

VOID			GCBitmapMgr::LoadAllResources()
{
	GetFontPtr(Language(L"Font", L"宋体"), GetLanguageFontSize(), false, false, FONT_MODE_NORMAL);
	GetFontPtr(Language(L"Font", L"宋体"), GetLanguageFontSize(), true, false, FONT_MODE_BOLD);
}

GCBitmapMgr::~GCBitmapMgr(void)
{
	for(int i=0;i<m_fonts.size();i++)
	{
		delete m_fonts[i];
	}
	for(int i=0;i<m_images.size();i++)
	{
		delete m_images.at(i);
	}
	for(int i=0;i<m_treeImages.size();i++)
	{
		delete m_treeImages.at(i);
	}
}

CFont*			GCBitmapMgr::GetFontPtr(INT nMode)
{
	for(int i=0;i<m_fonts.size();i++)
	{
		if(m_fonts[i]->nID == nMode)
			return &m_fonts[i]->font;
	}
	return NULL;
}

const wchar_t*	GCBitmapMgr::GetFontFace(INT nMode)
{
	for(int i=0;i<m_fonts.size();i++)
	{
		if(m_fonts[i]->nID == nMode)
			return m_fonts[i]->fontName;
	}
	return L"Arial";
}

CFont*			GCBitmapMgr::GetFontPtr(const wchar_t* szFontName, INT nHeight, BOOL bBold, BOOL bUnderline, INT nID)
{
	for(int i=0;i<m_fonts.size();i++)
	{
		if(m_fonts[i]->nID == nID)
			return &m_fonts[i]->font;
	}
	for(int i=0;i<m_fonts.size();i++)
	{
		if(m_fonts[i]->bBold == bBold && m_fonts[i]->bUnderline == bUnderline && m_fonts[i]->nHeight == nHeight && wcsicmp(m_fonts[i]->fontName, szFontName) == 0)
			return &m_fonts[i]->font;
	}
	GCFontData* pData	=	new GCFontData;
	pData->bBold	=	bBold;
	pData->nHeight	=	nHeight;
	pData->bUnderline	=	bUnderline;
	pData->nID		=	nID;
	wcscpy(pData->fontName, szFontName);
	//
	pData->font.CreateFont(nHeight,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		bBold ? FW_BOLD : FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		bUnderline,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		szFontName);                 // lpszFacename
	m_fonts.push_back( pData );
	return &pData->font;
}

CImage*			GCBitmapMgr::GetImageByResID(INT nResID)
{
	for(int i=0;i<m_images.size();i++)
	{
		if(m_images[i]->nResourceID == nResID)
			return &m_images[i]->vImage;
	}
	return 0;
}

INT				GCBitmapMgr::GetIndexByResID(INT nResID)
{
	for(int i=0;i<m_images.size();i++)
	{
		if(m_images[i]->nImageID == nResID)
			return m_images[i]->nImageIndex;
	}
	return -1;
}

VOID			GCBitmapMgr::AddFromFile(LPCTSTR szFile)	//从文件加入
{
	GCImageData* img = new GCImageData;
	if(FAILED(img->vImage.Load(szFile)))
	{
		delete img;
		return;
	}
	img->strFile	=	szFile;
	this->m_images.push_back( img );
}

CImage*			GCBitmapMgr::GetImageByFile(LPCTSTR szFile)
{
	for(int i=0;i<m_images.size();i++)
	{
		if(m_images[i]->strFile == szFile)
			return &m_images[i]->vImage;
	}
	return NULL;
}

CImage*			GCBitmapMgr::LoadFromResToAtlImage(INT nResourceID)
{
	for(int i=0;i<m_images.size();i++)
	{
		if(m_images[i]->nResourceID == nResourceID)
			return &m_images[i]->vImage;
	}
	GCImageData* img = new GCImageData;
	img->vImage.LoadFromResource(AfxGetInstanceHandle(), nResourceID);
	img->nResourceID	=	nResourceID;
	this->m_images.push_back( img );
	return &img->vImage;
}

INT			GCBitmapMgr::LoadFromResToImageList(INT nResourceID)
{
	for(int i=0;i<m_images.size();i++)
	{
		if(m_images[i]->nImageID == nResourceID)
			return m_images[i]->nImageIndex;
	}
	GCImageData* img = new GCImageData;
	CBitmap bm;
	bm.LoadBitmap(nResourceID);
	img->nImageID	=	nResourceID;
	img->nImageIndex	=	this->m_imageList.Add(&bm, RGB(0,0,0));
	this->m_images.push_back( img );
	return img->nImageIndex;
}

INT			GCBitmapMgr::LoadFromResToImageTreeList(INT nResourceID)
{
	for(int i=0;i<m_treeImages.size();i++)
	{
		if(m_treeImages[i]->nImageID == nResourceID)
			return m_treeImages[i]->nImageIndex;
	}
	GCImageData* img = new GCImageData;
	CBitmap bm;
	bm.LoadBitmap(nResourceID);
	img->nImageID	=	nResourceID;
	img->nImageIndex	=	this->m_imageListTree.Add(&bm, RGB(0,0,0));
	this->m_treeImages.push_back( img );
	return img->nImageIndex;
}

VOID		GCBitmapMgr::SetCurrentCursor(LPCWSTR szResID)
{
	for(int i=0;i<m_cursors.size();i++)
	{
		if(m_cursors[i].szWID == szResID)
		{
			SetCursor(m_cursors[i].hCursor);
			return;
		}
	}
	GCCursorData d;
	d.szWID		=	szResID;
	d.hCursor	=	LoadCursor(NULL, szResID);
	SetCursor(d.hCursor);
	m_cursors.push_back( d );
}

VOID		GCBitmapMgr::SetCurrentCursor(INT nResID)
{
	for(int i=0;i<m_cursors.size();i++)
	{
		if(m_cursors[i].nID == nResID)
		{
			SetCursor(m_cursors[i].hCursor);
			return;
		}
	}
	GCCursorData d;
	d.nID		=	nResID;
	d.hCursor	=	LoadCursor(AfxGetInstanceHandle(), (LPCTSTR)nResID);
	SetCursor(d.hCursor);
	m_cursors.push_back( d );
}

VOID			WrapWidthDraw(HDC dest, const CRect& rcDest, HDC dcSrc, const CRect& rcSrc, int wrapWidth)
{
	int width	=	rcSrc.Width() - (wrapWidth * 2);
	int x = rcDest.left;
	if(wrapWidth > 0)
		BitBlt(dest, x, rcDest.top, wrapWidth, rcSrc.Height(), dcSrc, 0, 0, SRCCOPY);
	x += wrapWidth;
	int num = rcDest.Width() / width + 1;
	for(int i=0;i<num;i++)
	{
		width = min(width, (rcDest.right - x));
		BitBlt(dest, x, rcDest.top, width, rcSrc.Height(), dcSrc, wrapWidth, 0, SRCCOPY);
		x += width;
		if(width >= rcDest.right)
			break;
	}
	if(wrapWidth > 0)
		BitBlt(dest, rcDest.right - wrapWidth, rcDest.top, wrapWidth, rcSrc.Height(), dcSrc, rcSrc.Width() - wrapWidth, 0, SRCCOPY);
}

VOID			WrapWidthDraw(HDC dest, const CRect& rcDest, CImage& img, int wrapWidth)
{
	WrapWidthDraw(dest, rcDest, img.GetDC(), CRect(0, 0, img.GetWidth(), img.GetHeight()), wrapWidth);
	img.ReleaseDC();
}

VOID			FillWidth(HDC dest, const CRect& rcDest, CImage& img, int wrapWidth)
{
	FillWidth(dest, rcDest, img.GetDC(), CRect(0, 0, img.GetWidth(), img.GetHeight()), wrapWidth);
	img.ReleaseDC();
}

VOID			FillHeight(HDC dest, const CRect& rcDest, CImage& img)
{
	FillHeight(dest, rcDest, img.GetDC(), CRect(0, 0, img.GetWidth(), img.GetHeight()));
	img.ReleaseDC();
}

VOID			FillWidth(HDC dest, const CRect& rcDest, HDC dcSrc, const CRect& rcSrc, int wrapWidth)
{
	int width	=	rcSrc.Width() - (wrapWidth * 2);
	int x = rcDest.left;
	if(wrapWidth > 0)
		BitBlt(dest, x, rcDest.top, wrapWidth, rcSrc.Height(), dcSrc, rcSrc.left, rcSrc.top, SRCCOPY);
	x += wrapWidth;
	int num = rcDest.Width() / width + 1;
	for(int i=0;i<num;i++)
	{
		width = min(width, (rcDest.right - x));
		BitBlt(dest, x, rcDest.top, width, rcSrc.Height(), dcSrc, rcSrc.left + wrapWidth, rcSrc.top, SRCCOPY);
		x += width;
		if(width >= rcDest.right)
			break;
	}
	if(wrapWidth > 0)
		BitBlt(dest, rcDest.right - wrapWidth, rcDest.top, wrapWidth, rcSrc.Height(), dcSrc, rcSrc.left + rcSrc.Width() - wrapWidth, rcSrc.top, SRCCOPY);
}

VOID			FillHeight(HDC dest, const CRect& rcDest, HDC dcSrc, const CRect& rcSrc)
{
	int height	=	rcSrc.Height();
	int num		=	rcDest.Height() / height + 1;
	int y		=	rcDest.top;
	for(int i=0;i<num;i++)
	{
		height = min(height, (rcDest.bottom - y));
		BitBlt(dest, rcDest.left, y, rcSrc.Width(), height, dcSrc, rcSrc.left, rcSrc.top, SRCCOPY);
		y += height;
		if(height >= rcDest.bottom)
			break;
	}
}

VOID			FillBackground(HDC dest, const CRect& rcDest, HDC dcSrc, const CRect& rcSrc, int wrapWidth, int wrapHeight)
{
	int left	=	wrapWidth;
	int top		=	wrapHeight;
	if(left > 0 && top > 0)
	{
		//
		FillHeight(dest, CRect(rcDest.left, rcDest.top + top, left, rcDest.bottom - top), dcSrc, CRect(rcSrc.left, rcSrc.top + top, left, rcSrc.bottom - top));
		FillHeight(dest, CRect(rcDest.right - left, rcDest.top + top, rcDest.right, rcDest.bottom - top), dcSrc, CRect(rcSrc.right - left, rcSrc.top + top, rcSrc.right, rcSrc.bottom - top));
		FillWidth(dest, CRect(rcDest.left, rcDest.top, rcDest.right, rcDest.top + top), dcSrc, CRect(rcSrc.left, rcSrc.top, rcSrc.right, rcSrc.top + top), left);
		FillWidth(dest, CRect(rcDest.left, rcDest.bottom - top, rcDest.right, rcDest.bottom), dcSrc, CRect(rcSrc.left, rcSrc.bottom - top, rcSrc.right, rcSrc.bottom), left);
		//
		int srcHeight		=	rcSrc.Height() - top * 2;
		int height			=	rcDest.Height() - top * 2;
		int srcWidth		=	rcSrc.Width() - left * 2;
		int width			=	rcDest.Width() - left * 2;
		//
		int numHeight		=	height / srcHeight;
		int numWidth		=	width / srcWidth;
		for(int x=0;x<=numWidth;x++)
		{
			for(int y=0;y<=numHeight;y++)
			{
				CRect rc(left + srcWidth * x + rcDest.left, top + srcHeight * y + rcDest.top, left + srcWidth * (x + 1) + rcDest.left, top + srcHeight * (y + 1) + rcDest.top);
				if(rc.right > (rcDest.right - left))
					rc.right = rcDest.right - left;
				if(rc.bottom > (rcDest.bottom - top))
					rc.bottom = rcDest.bottom - top;
				//
				if(rc.right <= rc.left)
					continue;
				if(rc.bottom <= rc.top)
					continue;
				BitBlt(dest, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, dcSrc, left, top, SRCCOPY);
			}
		}
		//
	}
}

VOID			FillBackground(HDC dest, const CRect& rcDest, CImage& img, int wrapWidth, int wrapHeight)
{
	FillBackground(dest, rcDest, img.GetDC(), CRect(0, 0, img.GetWidth(), img.GetHeight()), wrapWidth, wrapHeight);
	img.ReleaseDC();
}

