//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   BitmapManager.cpp
	软件名称   :   幻影游戏引擎

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	
	Copyright 2009-2016 Zhao Dexian
	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "resource.h"
#include ".\bitmapmanager.h"

CResourceManager*	g_pBitmapManager = 0;
CResourceManager*	GetBitmapManagerPtr()
{
	return g_pBitmapManager;
}

CResourceManager::CResourceManager(void)
{
	g_pBitmapManager	=	this;
	m_imageList.Create(20, 20, ILC_COLOR24, 1, 1);
	m_imageListTree.Create(TREE_IMAGE_HEIGHT, TREE_IMAGE_HEIGHT, ILC_COLOR24, 1, 1);
	//
}

CHotkeyManager*			g_pHotkeyManager	=	0;
CHotkeyManager*			GetHotkeyManagerPtr()
{
	return g_pHotkeyManager;
}

CHotkeyManager::CHotkeyManager()
{
	g_pHotkeyManager	=	this;
}
CHotkeyManager::~CHotkeyManager()
{
}

BOOL				CHotkeyManager::DoHotkey(INT vkKey)
{
	BOOL bShift		=	HIBYTE(GetKeyState(VK_SHIFT));
	BOOL bControl	=	HIBYTE(GetKeyState(VK_CONTROL));
	BOOL bAlt		=	HIBYTE(GetKeyState(VK_MENU));
	for(int i=0;i<m_hotkeys.size();i++)
	{
		HotkeyData& hk	=	m_hotkeys[i];
		if(hk.vkKey == vkKey)
		{
			if(hk.bShift)
				if(!bShift)
					continue;
			if(hk.bCtrl)
				if(!bControl)
					continue;
			if(hk.bAlt)
				if(!bAlt)
					continue;
			hk.pMenuItem->BeforePopup(0);
			hk.pMenuItem->SendCommand(0);
			return true;
		}
	}
	return false;
}

VOID				CHotkeyManager::RegisterHotkey(INT vkKey, bool bShift, bool bCtrl, bool bAlt, CMenuItem* pMenuItem)
{
	HotkeyData hk;
	hk.bShift	=	bShift;
	hk.bCtrl	=	bCtrl;
	hk.bAlt		=	bAlt;
	hk.vkKey	=	vkKey;
	hk.pMenuItem	=	pMenuItem;
	this->m_hotkeys.push_back( hk );
}

VOID			CResourceManager::LoadAllResources()
{
	this->LoadFromResToAtlImage(IDB_TAB_NOT_SELECT);
	this->LoadFromResToAtlImage(IDB_TAB_SELECT);
	this->LoadFromResToAtlImage(IDB_TAB_SELECTBEFORE);
	this->LoadFromResToAtlImage(IDB_TAB_OTHER);
	this->LoadFromResToAtlImage(IDB_CLIENT_HEIGHT);
	GetFontPtr(Lan("宋体"), GetLanguageFontSize(), false, false, FONT_MODE_NORMAL);
	GetFontPtr(Lan("宋体"), GetLanguageFontSize(), true, false, FONT_MODE_BOLD);
}

CResourceManager::~CResourceManager(void)
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

CFont*			CResourceManager::GetFontPtr(INT nMode)
{
	for(int i=0;i<m_fonts.size();i++)
	{
		if(m_fonts[i]->nID == nMode)
			return &m_fonts[i]->font;
	}
	return NULL;
}

CFont*			CResourceManager::GetFontPtr(const wchar_t* szFontName, INT nHeight, BOOL bBold, BOOL bUnderline, INT nID)
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
	FontData* pData	=	new FontData;
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

CImage*			CResourceManager::GetImageByResID(INT nResID)
{
	for(int i=0;i<m_images.size();i++)
	{
		if(m_images[i]->nResourceID == nResID)
			return &m_images[i]->vImage;
	}
	return 0;
}

INT				CResourceManager::GetIndexByResID(INT nResID)
{
	for(int i=0;i<m_images.size();i++)
	{
		if(m_images[i]->nImageID == nResID)
			return m_images[i]->nImageIndex;
	}
	return -1;
}

VOID			CResourceManager::AddFromFile(LPCTSTR szFile)	//从文件加入
{
	IDEImageData* img = new IDEImageData;
	if(FAILED(img->vImage.Load(szFile)))
	{
		delete img;
		return;
	}
	img->strFile	=	szFile;
	this->m_images.push_back( img );
}

CImage*			CResourceManager::GetImageByFile(LPCTSTR szFile)
{
	for(int i=0;i<m_images.size();i++)
	{
		if(m_images[i]->strFile == szFile)
			return &m_images[i]->vImage;
	}
	return NULL;
}

CImage*			CResourceManager::LoadFromResToAtlImage(INT nResourceID)
{
	for(int i=0;i<m_images.size();i++)
	{
		if(m_images[i]->nResourceID == nResourceID)
			return &m_images[i]->vImage;
	}
	IDEImageData* img = new IDEImageData;
	img->vImage.LoadFromResource(AfxGetInstanceHandle(), nResourceID);
	img->nResourceID	=	nResourceID;
	this->m_images.push_back( img );
	return &img->vImage;
}

INT			CResourceManager::LoadFromResToImageList(INT nResourceID)
{
	for(int i=0;i<m_images.size();i++)
	{
		if(m_images[i]->nImageID == nResourceID)
			return m_images[i]->nImageIndex;
	}
	IDEImageData* img = new IDEImageData;
	CBitmap bm;
	bm.LoadBitmap(nResourceID);
	img->nImageID	=	nResourceID;
	img->nImageIndex	=	this->m_imageList.Add(&bm, RGB(0,0,0));
	this->m_images.push_back( img );
	return img->nImageIndex;
}

INT			CResourceManager::LoadFromResToImageTreeList(INT nResourceID)
{
	for(int i=0;i<m_treeImages.size();i++)
	{
		if(m_treeImages[i]->nImageID == nResourceID)
			return m_treeImages[i]->nImageIndex;
	}
	IDEImageData* img = new IDEImageData;
	CBitmap bm;
	bm.LoadBitmap(nResourceID);
	img->nImageID	=	nResourceID;
	img->nImageIndex	=	this->m_imageListTree.Add(&bm, RGB(0,0,0));
	this->m_treeImages.push_back( img );
	return img->nImageIndex;
}

VOID		CResourceManager::SetCurrentCursor(LPCWSTR szResID)
{
	for(int i=0;i<m_cursors.size();i++)
	{
		if(m_cursors[i].szWID == szResID)
		{
			SetCursor(m_cursors[i].hCursor);
			return;
		}
	}
	CursorData d;
	d.szWID		=	szResID;
	d.hCursor	=	LoadCursor(NULL, szResID);
	SetCursor(d.hCursor);
	m_cursors.push_back( d );
}

VOID		CResourceManager::SetCurrentCursor(INT nResID)
{
	for(int i=0;i<m_cursors.size();i++)
	{
		if(m_cursors[i].nID == nResID)
		{
			SetCursor(m_cursors[i].hCursor);
			return;
		}
	}
	CursorData d;
	d.nID		=	nResID;
	d.hCursor	=	LoadCursor(AfxGetInstanceHandle(), (LPCTSTR)nResID);
	SetCursor(d.hCursor);
	m_cursors.push_back( d );
}
