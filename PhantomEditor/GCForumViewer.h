#pragma once
#include "GCViewBase.h"
#include "IUnit3D.h"
#include "TrackAnimationCtrl.h"
#include "DlgViewerHttp.h"

class GCForumViewer: public GCViewBase
{
public:
	GCForumViewer(CPhantomGCDoc* docPtr);
	~GCForumViewer(void);
	virtual	VOID				OnActive();		//被激活
	virtual	VOID				OnDeActive();	//失去激活
	virtual	BOOL				OpenFile(const wchar_t* szFile);	//打开文件,如果失败则会自动删除
	virtual	BOOL				SaveFile(BOOL bSaveAs);	//保存文件
	virtual void				OnFrameMove(const RenderParameter& r);	//每次更新
	virtual void				OnPushRenderObjects(const RenderParameter& r);	//渲染
	virtual	FileViewerType		GetViewerType(){return FileViewerType_HomeViewer;}
	virtual	HICON				GetViewerIcon();
	virtual	VOID		GetNewFileName(wchar_t* szFile);
	virtual void		OnRenderingEnd(const RenderParameter& r);	//渲染
	//
	VOID				RebuildTrees();
	enum{
		TEXTURE_NONE	=	0,
		TEXTURE_MAIN,
	};
	void				OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp);
	VOID				OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem);	//更新选择树形控件
	virtual	void		OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI);
	virtual	void		OnCommandUI(INT nMenuID);
	//
	VOID				BuildAll();
	//
public:
	CDlgViewerHttp		m_viewerHttp;
};

