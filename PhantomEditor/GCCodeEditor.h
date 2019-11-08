/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "GCViewBase.h"
#include "MyLuaEditor.h"

class GCCodeEditor: public GCViewBase
{
public:
	GCCodeEditor(CPhantomGCDoc* docPtr);
	~GCCodeEditor(void);
	virtual	VOID				OnActive();		//被激活
	virtual	VOID				OnDeActive();	//失去激活
	virtual	VOID				CloseFile();
	virtual	BOOL				OpenFile(const wchar_t* szFile);	//打开文件,如果失败则会自动删除
	virtual	BOOL				SaveFile(BOOL bSaveAs=false);	//保存文件
	virtual void				OnFrameMove();	//每次更新
	virtual void				OnPushRenderObjects();	//渲染
	virtual	FileViewerType		GetViewerType(){return FileViewerType_Lua;}
	virtual	HICON				GetViewerIcon();
	virtual	VOID		OnSetFocus();
	virtual void		OnRenderingEnd();	//渲染
	//
	EditorCallbackT<GCCodeEditor>	m_cb;
	VOID				RebuildTrees();
	enum{
		FUNCTION_NONE	=	0,
		FUNCTION_MAIN,
	};
	void				OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp);
	VOID				OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem);	//更新选择树形控件
	virtual	void		OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI);
	virtual	void		OnCommandUI(INT nMenuID);
	//
	VOID				BuildAll();
	//
public:
	CMyLuaEditor		m_luaEditor;
};

