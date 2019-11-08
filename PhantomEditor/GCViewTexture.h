#pragma once
#include "GCViewBase.h"
#include "TrackAnimationCtrl.h"

class GCViewTextureEditor: public GCViewBase
{
public:
	GCViewTextureEditor(CPhantomGCDoc* docPtr);
	~GCViewTextureEditor(void);
	virtual	VOID				OnActive();		//被激活
	virtual	VOID				OnDeActive();	//失去激活
	virtual	VOID				CloseFile();
	virtual	BOOL				OpenFile(const wchar_t* szFile);	//打开文件,如果失败则会自动删除
	virtual	BOOL				SaveFile(BOOL bSaveAs);	//保存文件
	virtual void				OnFrameMove();	//每次更新
	virtual void				OnPushRenderObjects();	//渲染
	virtual	FileViewerType		GetViewerType(){return FileViewerType_Texture;}
	virtual	HICON				GetViewerIcon();
	virtual void				OnRenderingEnd();	//渲染
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
	//std::vector<IGeometryGroup*>	m_saveGeometrys;//保存模型队列
	//IGeometryGroup*		m_geometryPtr;
	//IDialog2D*			m_dialogPtr;
	Phantom::Texture*		m_texturePtr;
};

