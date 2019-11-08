//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   GCUnit2DEditor.h
	软件名称   :   幻影游戏引擎

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	
	Copyright 2009-2016 Zhao Dexian
	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___GCGAMEMAKER__H___
#define ___GCGAMEMAKER__H___
#pragma once
#pragma once
#include "GCViewBase.h"
#include "TrackAnimationCtrl.h"

typedef	VOID	(*ProcUnit2DEquip)(IUnit2DEquip* data, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*ProcUnit2DAnimation)(IUnit2DAnimationData* data, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*ProcUnit2DTexture)(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2);

class GCViewUnit2DEditor: public GCViewBase
{
public:
	GCViewUnit2DEditor(CPhantomGCDoc* docPtr);
	~GCViewUnit2DEditor(void);
	virtual	VOID				OnActive();		//被激活
	virtual	VOID				OnDeActive();	//失去激活
	virtual	VOID				CloseFile();
	virtual	BOOL				OpenFile(const wchar_t* szFile);	//打开文件,如果失败则会自动删除
	virtual	BOOL				SaveFile(BOOL bSaveAs);	//保存文件
	virtual void				OnFrameMove();	//每次更新
	virtual void				OnPushRenderObjects();	//渲染
	virtual	FileViewerType		GetViewerType(){return FileViewerType_Unit2D;}
	virtual	HICON				GetViewerIcon();
	virtual	INT					GetPopupMenuID(){return 3;}
	virtual void				OnRenderingEnd();	//渲染
	virtual	bool				OnWindowProc(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam);
	//
	VOID						RebuildTrees();
	enum{
		UNIT2D_NONE	=	0,
		UNIT2D_MAIN,
		UNIT2D_EQUIP_LIST,			//装备列表
		UNIT2D_EQUIP,
		UNIT2D_ANIMATION,
		UNIT2D_TEXTURE,
		UNIT2D_ANIMATION_LIST,		//动画定义
		UNIT2D_ANIMATION_ID,		//动画
	};
	VOID						GetNewFileName(wchar_t* szFile);
	void						OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp);
	VOID						OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem);	//更新选择树形控件
	virtual	void				OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI);
	virtual	void				OnCommandUI(INT nMenuID);
	void						OnTreeViewKeyDown(INT nChar);
	//
	VOID						OnRenameAnimations(IUnit2DAnimationData* ani);
	//
	VOID						LoopAllUnit2DEquips(ProcUnit2DEquip proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE);
	VOID						LoopAllUnit2DAnimations(ProcUnit2DAnimation proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE);
	VOID						LoopAllUnit2DTexture(ProcUnit2DTexture proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRect = false);
	//
	VOID						BuildAll();
	//
	GCPropertyBase				m_unitProp;
	GCPropertyBase				m_equipProp;
	GCPropertyBase				m_textureProp;
	GCPropertyBase				m_animationIDProp;
	//
	VOID						InitUnitProp();
	VOID						InitEquipProp();
	VOID						InitTextureProp();
	VOID						InitAnimaionProp();
	//
	PropertyPtr					m_equipName;
	PropertyPtr					m_equipID;
	PropertyPtr					m_equipLayer;
	//
	PropertyPtr					m_textureUVCountW;
	PropertyPtr					m_textureUVCountH;
	PropertyPtr					m_textureUVBeginIndex;
	PropertyPtr					m_textureUVPlayCount;
	PropertyPtr					m_textureTileCountW;
	PropertyPtr					m_textureTileCountH;
	PropertyPtr					m_textureScale;
	PropertyFilePtr				m_textureFile;
	PropertyPtr					m_textureFileClear;//设置空图片
	PropertyPtr					m_textureRectL;
	PropertyPtr					m_textureRectT;
	PropertyPtr					m_textureRectW;
	PropertyPtr					m_textureRectH;
	PropertyPtr					m_textureRenderSide;
	PropertyPtr					m_texturePick;
	PropertyPtr					m_textureRegetRect;
	//
	PropertyPtr					m_animationName;
	PropertyPtr					m_animationID;
	//
	PropertyPtr					m_unitSizeW;
	PropertyPtr					m_unitSizeH;
	PropertyPtr					m_unitCenterX;
	PropertyPtr					m_unitCenterY;
	//
	VOID						RefreshEquips();
	VOID						RefreshAnimations();
	//
	virtual	VOID				OnCopyTree();
	virtual	VOID				OnPasteTree();
	virtual	BOOL				IsCopyTree();
	virtual	BOOL				IsPasteTree();
	virtual	VOID				OnDeleteTree();
	virtual	BOOL				IsDeleteTree();
	//
	VOID						RefreshEquip(IUnit2DEquip* ele, HTREEITEM hEquip);
	//
	HTREEITEM					m_hEquipList;
	HTREEITEM					m_hAnimationList;
public:
	IUnit2DData*				m_dataPtr;
	IUnit2DRender*				m_renderPtr;
	BOOL						m_bLButtonDown;
	CPoint						m_downPoint;
	CPoint						m_downOffset;
	enum EditorMode{
		EditorMode_None	=	0,
		EditorMode_CenterPos,
	};
	EditorMode					m_editMode;
};



#endif
