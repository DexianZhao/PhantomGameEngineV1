/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include <vector>
#include "GCViewBase.h"
//
//#ifdef FREE_MODE
//#define	MAX_DOCUMENT_OPEN		3
//#else
//#define	MAX_DOCUMENT_OPEN		1000
//#endif
#define	MAX_FREE_DOCUMENT_OPEN		3

class GCViewManager
{
public:
	GCViewManager(void);
	~GCViewManager(void);
	GCViewBase*					FindDocument(CPhantomGCDoc* docPtr);
	GCViewBase*					FindDocument(HWND hWndView);
	BOOL						InsertDocument(CPhantomGCDoc* docPtr, const wchar_t* strFile);	//如果不存在自动创建
	BOOL						InsertDocument(CPhantomGCDoc* docPtr, FileViewerType type, const wchar_t* strFile);	//如果不存在自动创建
	VOID						RemoveDocument(CPhantomGCDoc* docPtr);
	//
	VOID						OnActiveDocument(CPhantomGCDoc* docPtr);
	VOID						OnActiveDocument(HWND hWnd);
	//
	VOID						DestroyAllDocuments();
	//
	VOID						RefreshRenderSize();	//重新设定游戏窗口大小时引发
	GCViewBase*					GetActiveView(){return m_curActive;}
	//
	INT							GetViewCount(){return (int)m_documentCount;}
	GCViewBase*					GetView(int index){if(index < 0 || index >= m_documentCount)return 0; return m_documents[index];}
	//
	VOID						SaveAll();
public:
	GCViewBase*					m_documents[1000];
	int							m_documentCount;
	//std::vector<GCViewBase*>	m_documents;
protected:
	VOID						OnNoActiveDocument();	//没有一个被激活时调用
	GCViewBase*					m_curActive;
};

GCViewManager*					GetViewManager();
