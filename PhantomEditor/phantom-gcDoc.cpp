
// phantom-gcDoc.cpp : CPhantomGCDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "phantom-gc.h"
#endif

#include "GCViewManager.h"

#include "phantom-gcDoc.h"

#include <propkey.h>
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPhantomGCDoc

IMPLEMENT_DYNCREATE(CPhantomGCDoc, CDocument)

BEGIN_MESSAGE_MAP(CPhantomGCDoc, CDocument)
END_MESSAGE_MAP()


// CPhantomGCDoc 构造/析构

CPhantomGCDoc::CPhantomGCDoc()
{
	// TODO: 在此添加一次性构造代码

}

CPhantomGCDoc::~CPhantomGCDoc()
{
}

int		g_createFileType	=	0;
VOID	CreateNewFile(int nType)
{
	g_createFileType	=	nType;
	AfxGetApp()->m_pDocManager->OnFileNew();
}

BOOL CPhantomGCDoc::OnNewDocument()
{
	if(g_createFileType == 0)
		return false;///
	int ct	=	g_createFileType;
	g_createFileType	=	0;
	if (!CDocument::OnNewDocument())
		return FALSE;
	if(!GetViewManager()->InsertDocument(this, (FileViewerType)ct, L""))
		return false;
	return TRUE;
}




// CPhantomGCDoc 序列化

void CPhantomGCDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

// CPhantomGCDoc 诊断

#ifdef _DEBUG
void CPhantomGCDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPhantomGCDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPhantomGCDoc 命令


void CPhantomGCDoc::OnChangedViewList()
{
	CDocument::OnChangedViewList();
}

BOOL CPhantomGCDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	if(!GetViewManager()->InsertDocument(this, lpszPathName))
		return false;
	return true;
}


BOOL CPhantomGCDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	GCViewBase* base	=	GetViewManager()->FindDocument(this);
	if(base)
		return base->SaveFile();
	return false;
}


void CPhantomGCDoc::OnCloseDocument()
{
	CDocument::OnCloseDocument();
	GetViewManager()->RemoveDocument(this);
	UpdateMainTitle();
}
