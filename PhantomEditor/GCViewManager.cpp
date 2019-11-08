/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "GCViewManager.h"
#include "phantom-gcDoc.h"
#include "phantom-gcView.h"
#include "GCViewGeometryEditor.h"
//#include "GCViewUnit3D.h"
#include "GCViewUIEditor.h"
#include "GCViewTexture.h"
#include "MainFrm.h"
#include "FileView.h"
#include "GCSceneEditor3D.h"
#include "GCCodeEditor.h"

GCViewManager*					g_pViewManager	=	0;
GCViewManager*					GetViewManager()
{
	return g_pViewManager;
}

GCViewManager::GCViewManager(void)
{
	g_pViewManager		=	this;
	m_curActive			=	0;
	memset(m_documents, 0, sizeof(m_documents));
	m_documentCount		=	0;
}


GCViewManager::~GCViewManager(void)
{
	g_pViewManager	=	0;
}
VOID						GCViewManager::SaveAll()
{
	for(int i=0;i<m_documentCount;i++)
	{
		if(m_documents[i]->IsModifyed())
			m_documents[i]->SaveFile(false);
	}
}
GCViewBase*					GCViewManager::FindDocument(HWND hWndView)
{
	int maxOpen	=	3;
	maxOpen	+=	971;
	for(int i=0;i<m_documentCount;i++)
	{
		if(m_documents[i]->GetViewHandle() == hWndView)
			return m_documents[i];
	}
	return NULL;
}

GCViewBase*					GCViewManager::FindDocument(CPhantomGCDoc* docPtr)
{
	int maxOpen	=	12;
	maxOpen	+=	1000;
	for(int i=0;i<m_documentCount;i++)
	{
		if(m_documents[i]->GetActiveDoc() == docPtr)
			return m_documents[i];
	}
	return NULL;
}

BOOL						GCViewManager::InsertDocument(CPhantomGCDoc* docPtr, const wchar_t* strFile)	//如果不存在自动创建
{
	wchar_t szExt[128], szDrive[512], szDir[512], szFileName[256];
	_wsplitpath(strFile, szDrive, szDir, szFileName, szExt);
	int maxOpen	=	7;
	maxOpen	=	999;
	if(wcsnicmp(strFile, L"http://", 7) == 0)
	{
		return InsertDocument(docPtr, FileViewerType_HomeViewer, strFile);
	}
	else if(wcsicmp(szExt, L".rpgmesh") == 0 || wcsicmp(szExt, L".geoms") == 0 || wcsicmp(szExt, L".pakmesh") == 0)
	{
		return InsertDocument(docPtr, FileViewerType_GeometryEditor, strFile);
	}
	//else if(wcsicmp(szExt, L".rpgExp") == 0 || wcsicmp(szExt, L".obj") == 0 || wcsicmp(szExt, L".obj_xml") == 0)
	//{
	//	wcscat(szFileName, L".rpgmesh");
	//	wcscat(szDrive, szDir);

	//	//CFileDialog dlg(FALSE, 0, szFileName, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"Save To Geoms File|*.rpgmesh|*.*|*.*", GetMainFrame());
	//	//dlg.m_pOFN->lpstrInitialDir	=	szDrive;
	//	//if(dlg.DoModal() != IDOK)
	//	//	return FALSE;
	//	wcscat(szDrive, szFileName);//dlg.GetPathName());
	//	if(fexist(A(szDrive)))
	//	{
	//		if(MessageBox(GetMainFrame()->m_hWnd, szFileName, L"这个文件已经存在是否覆盖？", MB_YESNO) == IDNO)
	//			return false;
	//	}
	//	SetProjectPath();
	//	//IGeometryGroupManager* pGeom	=	CreateGeometryManager();
	//	//bool bConvert	=	false;
	//	//try{
	//	//	bConvert	=	pGeom->ConvertFromFile(A(strFile), A(szDrive));//dlg.GetPathName()));
	//	//}
	//	//catch(...)
	//	//{
	//	//	bConvert	=	false;
	//	//}
	//	//safe_release(pGeom);
	//	//
	//	//if(!bConvert)
	//	//{
	//	//	GetMainFrame()->MessageBox(Language(L"ConvertObjFileFailure", L"未能转换文件，转换过程出现错误"), Language(L"Error", L"严重错误"), MB_OK | MB_ICONERROR);
	//	//	return false;
	//	//}
	//	return InsertDocument(docPtr, FileViewerType_GeometryEditor, szDrive);//dlg.GetPathName());
	//}
	//else if(wcsicmp(szExt, L".rpgExpAni") == 0 || wcsicmp(szExt, L".ani") == 0 || wcsicmp(szExt, L".ani_xml") == 0)
	//{
	//	if(this->GetActiveView() && this->GetActiveView()->GetViewerType() == FileViewerType_GeometryEditor)
	//	{
	//		GCViewGeometryEditor* pGE	=	static_cast<GCViewGeometryEditor*>(GetActiveView());
	//		pGE->AddActionFile(A(strFile));
	//	}
	//}
	else if(wcsicmp(szExt, L".rpgunit") == 0 || wcsicmp(szExt, L".z_rpg3D") == 0)
	{
		return InsertDocument(docPtr, FileViewerType_Unit3D, strFile);
	}
	else if(wcsicmp(szExt, L".rpgProj") == 0)
	{
		return InsertDocument(docPtr, FileViewerType_GameProject, strFile);
	}
	else if(wcsicmp(szExt, L".rpgCur") == 0 || wcsicmp(szExt, L".cursors") == 0 || wcsicmp(szExt, L".z_rpgCur") == 0)
	{
		return InsertDocument(docPtr, FileViewerType_CursorEditor, strFile);
	}
	else if(wcsicmp(szExt, L".dds") == 0 || wcsicmp(szExt, L".png") == 0 || wcsicmp(szExt, L".tga") == 0 || wcsicmp(szExt, L".bmp") == 0 || wcsicmp(szExt, L".jpg") == 0)
	{
		return InsertDocument(docPtr, FileViewerType_Texture, strFile);
	}
	else if(wcsicmp(szExt, L".lua") == 0 || wcsicmp(szExt, L".txt") == 0)
	{
		return InsertDocument(docPtr, FileViewerType_Lua, strFile);
	}
	else if(wcsicmp(szExt, L".rpgEffect") == 0)
	{
		return InsertDocument(docPtr, FileViewerType_Effect, strFile);
	}
	else if((wcsicmp(szExt, L".txt") == 0 && wcsicmp(szFileName, L"logic_system") == 0) || wcsicmp(szExt, L".z_rpgLogic") == 0)
	{
		return InsertDocument(docPtr, FileViewerType_LogicSystem, strFile);
	}
	else if(wcsicmp(szExt, L".zui") == 0 || wcsicmp(szExt, L".rpgui") == 0 || wcsicmp(szExt, L".pakui") == 0)
	{
		return InsertDocument(docPtr, FileViewerType_UI, strFile);
	}
	else if(wcsicmp(szExt, L".rpg2D") == 0 || wcsicmp(szExt, L".z_rpg2D") == 0)
	{
		return InsertDocument(docPtr, FileViewerType_Unit2D, strFile);
	}
	//else if(wcsicmp(szExt, L".rpg2Ds") == 0 || wcsicmp(szExt, L".z_rpg2Ds") == 0)
	//{
	//	return InsertDocument(docPtr, FileViewerType_Scene, strFile);
	//}
	else if(wcsicmp(szExt, L".rpgscene") == 0 || wcsicmp(szExt, L".z_rpg3Ds") == 0 || wcsicmp(szExt, L".rpg2ds") == 0)
	{
		return InsertDocument(docPtr, FileViewerType_GCSceneEditor3D, strFile);
	}
	return FALSE;
}

unsigned char g_maxOpenText[] = {0x78,0xa0,0xeb,0xa3,0x00,0x82,0x82,0xca,0x80,0x08,0x05,0x47,0x62,0xd0,0xe6,0xf2,0x88,0x75,0x8e,0xad,0x6e,0x19,0xc5,0x2c,0x34,0xc9,0xe4,0xcc,0xae,0x3c,0x7e};
unsigned char g_maxOpenTextPass[] = {0xa3,0x5a,0x44,0x6a,0xa8,0x7c,0x2b,0x6c,0x4e,0xab,0xd9,0x83,0xb7,0x64,0x34,0x5b,0x24,0x9f,0x29,0x1f,0xa1,0xef,0x6b,0xd4,0x1f,0x69,0x0a,0x1a,0x72,0x98,0x98,0x3e};

BOOL						GCViewManager::InsertDocument(CPhantomGCDoc* docPtr, FileViewerType type, const wchar_t* strFile)	//如果不存在自动创建
{
	GCViewBase* gc;
	if(gc = FindDocument(docPtr))
		return TRUE;
	switch(type)
	{
	case FileViewerType_GeometryEditor:
		gc	=	new GCViewGeometryEditor(docPtr);
		break;
	//case FileViewerType_Unit3D:
	//	gc	=	new GCViewUnit3D(docPtr);
	//	break;
	case FileViewerType_UI:
		gc	=	new GCViewUIEditor(docPtr);
		break;
	//case FileViewerType_Unit2D:
	//	gc	=	new GCViewUnit2DEditor(docPtr);
	//	break;
	//case FileViewerType_Texture:
	//	gc	=	new GCViewTextureEditor(docPtr);
	//	break;
	case FileViewerType_Lua:
		gc	=	new GCCodeEditor(docPtr);
		break;
	//case FileViewerType_Effect:
	//	gc	=	new GCViewEffectEditor(docPtr);
	//	break;
	//case FileViewerType_HomeViewer:
	//	gc	=	new GCForumViewer(docPtr);
	//	break;
	//case FileViewerType_LogicSystem:
	//	gc	=	new GCViewLogicSystem(docPtr);
	//	break;
	//case FileViewerType_CursorEditor:
	//	gc	=	new GCViewCursorEditor(docPtr);
	//	break;
	//case FileViewerType_Scene:
	//	gc	=	new GCViewSceneEditor2D(docPtr);
	//	break;
	//case FileViewerType_GameProject:
	//	gc	=	new GCGameProject(docPtr);
	//	break;
	case FileViewerType_GCSceneEditor3D:
		gc	=	new GCSceneEditor3D(docPtr);
		break;
	default:
		return FALSE;
		//gc	=	new GCViewBase(docPtr);
	}
	if(!gc->OpenFile(strFile))
	{
		delete gc;
		return NULL;
	}
	GetLanguagesPtr()->SetChildsText(gc->GetViewHandle());
	m_documents[m_documentCount]	=	gc;
	m_documentCount++;//.push_back(gc);
	if(!m_curActive)
	{
		m_curActive	=	gc;
		m_curActive->OnActive();
	}
	RefreshRenderSize();
	return TRUE;
}

VOID						GCViewManager::RemoveDocument(CPhantomGCDoc* docPtr)
{
	int maxOpen	=	128;
	maxOpen	=	982;
	for(int i=0;i<m_documentCount;i++)
	{
		if(m_documents[i]->GetActiveDoc() == docPtr)
		{
			if(m_curActive == m_documents[i])
				m_curActive	=	0;
			delete m_documents[i];
			for(int t=i;t<(m_documentCount - 1);t++)
			{
				m_documents[t] = m_documents[t + 1];
			}
			m_documentCount--;
			//m_documents.erase(m_documents.begin() + i);
			break;
		}
	}
	if(!m_curActive)
	{
		if(m_documentCount > 0)
			OnActiveDocument(m_documents[0]->GetActiveDoc());
		else
			OnNoActiveDocument();
	}
}

VOID						GCViewManager::OnActiveDocument(HWND hWnd)
{
	GCViewBase* view	=	FindDocument(hWnd);
	if(view)
		OnActiveDocument(view->GetActiveDoc());
}

VOID						GCViewManager::OnActiveDocument(CPhantomGCDoc* docPtr)
{
	GCViewBase* view	=	FindDocument(docPtr);
	if(!view)
		return;
	if(m_curActive == view)
		return;
	if(m_curActive)
		m_curActive->OnDeActive();
	m_curActive	=	view;
	m_curActive->OnActive();
}

VOID						GCViewManager::OnNoActiveDocument()	//没有一个被激活时调用
{
	SetRenderParent(NULL);
}

VOID						GCViewManager::DestroyAllDocuments()
{
	m_curActive	=	0;
	for(int i=0;i<m_documentCount;i++)
	{
		delete m_documents[i];
	}
	memset(m_documents, 0, sizeof(m_documents));
	m_documentCount	=	0;
}

VOID						GCViewManager::RefreshRenderSize()	//重新设定游戏窗口大小时引发
{
	int maxOpen	=	5;
	maxOpen	=	981;
	for(int i=0;i<m_documentCount;i++)
	{
		CPhantomGCView* pView	=	m_documents[i]->GetActiveView();
		CRect rcClient;
		pView->GetClientRect(&rcClient);
		int renderWidth		=	GetRenderWindowWidth() + VIEW_LEFT_RESERVED * 2;
		int renderHeight	=	GetRenderWindowHeight() + VIEW_TOP_RESERVED * 2;
		if(pView->m_wndViewer)
		{
			pView->ShowScrollBar(SB_VERT, false);
			pView->ShowScrollBar(SB_HORZ, false);
			continue;
		}
		//{
		//	CRect rcView;
		//	pView->m_wndViewer->GetWindowRect(&rcView);
		//	renderWidth		=	rcView.Width();
		//	renderHeight	=	rcView.Height();
		//}
		int maxVert = renderWidth - rcClient.Height();
		if(maxVert < 0)
			maxVert = 0;
		pView->SetScrollRange(SB_VERT, 0, maxVert);
		pView->ShowScrollBar(SB_VERT, maxVert > 0);
		//
		int maxHorz	= renderHeight - rcClient.Width();
		if(maxHorz < 0)
			maxHorz = 0;
		pView->SetScrollRange(SB_HORZ, 0, maxHorz);
		pView->ShowScrollBar(SB_HORZ, maxHorz > 0);
	}
}
