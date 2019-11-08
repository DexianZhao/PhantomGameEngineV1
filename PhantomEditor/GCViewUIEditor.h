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
#include "TrackAnimationCtrl.h"
#include "GCTrackEditor.h"
#include "PhantomSocket.h"
#include "PhantomManager.h"

typedef	VOID	(*ProcDialog3D)(Phantom::UIDialog* data, LPARAM nParameter, LPARAM nParameter2, UndoPtr* ptr);
typedef	VOID	(*ProcControl3D)(Phantom::UIControl* data, LPARAM nParameter, LPARAM nParameter2, UndoPtr* ptr);
typedef	VOID	(*ProcGroup3D)(Phantom::UIGroup* data, LPARAM nParameter, LPARAM nParameter2, UndoPtr* ptr);
typedef	VOID	(*ProcControlElement3D)(Phantom::UIControl* data, Phantom::UIElement* element, LPARAM nParameter, LPARAM nParameter2, UndoPtr* ptr);
typedef	VOID	(*ProcControlAnimation3D)(Phantom::UIControl* data, Phantom::UIAnimationTrack* c, LPARAM nParameter, LPARAM nParameter2, UndoPtr* ptr);
typedef	VOID	(*ProcControlUVAnimation)(Phantom::UIControl* data, Phantom::UIElement* e, Phantom::UVAnimation* eUV, LPARAM nParameter, LPARAM nParameter2, UndoPtr* ptr);
typedef	VOID	(*ProcControlUVAnimationR)(Phantom::UIControl* data, Phantom::UIElement* e, Phantom::UVAnimation* eUV, INT nIndex, LPARAM nParameter, LPARAM nParameter2, UndoPtr* ptr);
#ifdef USE_BOX
typedef	VOID	(*ProcBox2DBody)(Phantom::B2Body* data, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*ProcBox2DShape)(Phantom::B2Body* data, Phantom::B2Shape* shape, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*ProcBox2DJoint)(Phantom::B2Joint* data, LPARAM nParameter, LPARAM nParameter2);
#endif
#define	UIEditorFlags_TabIndex			10001
#define	UIEditorFlags_TrackAnimation	10002

class GCViewUIEditor: public GCViewBase
{
public:
	GCViewUIEditor(CPhantomGCDoc* docPtr);
	~GCViewUIEditor(void);
	//
	struct	PickInfo
	{
		int			nMovePointSide;
		int			nPickType;
	};
	virtual	VOID				OnActive();		//被激活
	virtual	VOID				OnDeActive();	//失去激活
	virtual	VOID				CloseFile();
	virtual	BOOL				OpenFile(const wchar_t* szFile);	//打开文件,如果失败则会自动删除
	virtual	BOOL				SaveFile(BOOL bSaveAs);	//保存文件
	virtual void				OnFrameMove();	//每次更新
	virtual void				OnRenderingEnd();	//渲染
	virtual	void				OnRenderEndUI();	//界面渲染之后
	virtual void				OnPushRenderObjects();	//渲染
	virtual	bool				OnWindowProc(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam);
	//
	virtual	void				OnEditorMode(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam);
	//virtual	void				OnEditorFloorMode(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam);
	virtual	void				OnEditorUnitMode(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam);
	virtual	VOID				BeginUndoFloat(Phantom::FloatAnimation* a1, Phantom::FloatAnimation* a2, Phantom::FloatAnimation* a3);
	virtual	VOID				EndUndoFloat(Phantom::FloatAnimation* a1, Phantom::FloatAnimation* a2, Phantom::FloatAnimation* a3);
	//
	virtual	FileViewerType		GetViewerType(){return FileViewerType_UI;}
	virtual	HICON				GetViewerIcon();
	virtual	INT					GetPopupMenuID(){return 2;}
	//
	VOID						LoopAllDialog(ProcDialog3D proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0);
	VOID						LoopAllGroup(ProcGroup3D proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE, BOOL bGroup = FALSE);
	Phantom::UIControl*			LoopAllControls(ProcControl3D proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE, BOOL bGroup = FALSE, BOOL bControl = FALSE);
	Phantom::UIElement*			LoopAllElements(ProcControlElement3D proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = false);
	VOID						LoopAllAnimations(ProcControlAnimation3D proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE);
	VOID						LoopAllUVAnimations(ProcControlUVAnimation proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE, BOOL bRebuildUVRects = false, BOOL bRefreshControl = false);
	VOID						LoopAllUVAnimationsR(ProcControlUVAnimationR proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE);
	//
#ifdef USE_BOX
	VOID						LoopAllBodys(ProcBox2DBody proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = false);
	VOID						LoopAllShapes(ProcBox2DShape proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = false);
	VOID						LoopAllJoints(ProcBox2DJoint proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = false);
#endif
	//
	VOID						RebuildTrees();
	VOID						ChangeSelectControls();
	VOID						OnChangeSelect(BOOL bTree = true);
	VOID						OnChangeBodySelect(BOOL bTree = true);
	void						OnDoubleClick(CMTreeCtrl* treePtr, HTREEITEM hItem);	//更新选择树形控件
	std::vector<Phantom::UIControl*>	m_controlSelects;
	//
	enum ToAlign{
		ToAlign_None	=	0,
		ToAlign_Left	=	0x01,
		ToAlign_Right	=	0x02,
		ToAlign_Top		=	0x04,
		ToAlign_Bottom	=	0x08,
	};
	VOID				GetMovePointRect(Phantom::UIControl* c, const Phantom::Rect& rc, Phantom::Rect* rets);
	//
	virtual	VOID		AlignObjects(AlignType type, int value);
	virtual	INT			GetAlignValue(AlignType type);
	virtual	BOOL		IsAlignEnable();
	//VOID				AlignControl(AlignType type, int value);
	//
	BOOL				AlignToGrid(CPoint& pos, int align = 0xff);
	BOOL				AlignRectToGrid(CPoint& offsetRet, const CPoint& pos, const Phantom::Rect& rc);
	//
	VOID				OnSetFloorMode(const CPoint& pos);
	int					m_nAlignType;
	CPoint				m_alignPointW;
	CPoint				m_alignPointH;
	BOOL				IsCenterAlign();
	//
	CPoint						m_downPoint;
	CPoint						m_lastPoint;
	std::vector<CPoint>			m_movePoints;
	std::vector<CRect>			m_moveCopys;
	std::vector<CPoint>			m_moveRotCents;
	std::vector<Phantom::float2>			m_moveCenters;
	//
	Phantom::UIControl*			m_moveControl;
	Phantom::UIControl*			m_insertControl;
	BOOL				m_bLButtonDown;
	int					m_nTabIndex;
	PickInfo			m_movePointSide;
	Phantom::UIControl*			PickControl(CPoint pos);
	//
	enum PickType
	{
		PickType_Body			=	1,
		PickType_MoveSide,
		PickType_Unit2DCenter,
	};
	Phantom::UIControl*			PickControls(PickInfo& ret, CPoint pos);
	BOOL				IsPickControl(CPoint pos, Phantom::UIControl* c, PickInfo& ret);
	//
	int					m_editorMode;
	//
	enum{
		UI_PROP_MAIN	=	1,
		UI_PROP_ACTION_PLAY,
		UI_PROP_ACTION_TYPE,
		UI_PROP_GROUP,
		UI_PROP_CONTROL,
		UI_PROP_IMAGELIST,
		UI_PROP_ELEMENTS,
		UI_PROP_ANIMATION_LIST,
		UI_PROP_ANIMATION,
		UI_PROP_UV_ANIMATION_LIST,
		UI_PROP_UV_ANIMATION,
		UI_PROP_EVENT_LIST,
		UI_PROP_EVENT,
		UI_PROP_UV_ANIMATION_RECT,
		UI_PROP_BOX2D_BODY,
		UI_PROP_BOX2D_JOINT,
		UI_PROP_B2_BODY,
		UI_PROP_B2_SHAPE,
		UI_PROP_B2_JOINT,
		UI_PROP_ANIMATION_FLOAT,
		UI_PROP_MOVE_CONTROL
	};
	int					m_trackType;
	int					m_trackP1,m_trackP2,m_trackP3,m_trackP4;
#ifdef USE_BOX
	VOID				RefreshBoxBodys();
	VOID				RefreshBoxJoints();
	VOID				RefreshBoxShapes(HTREEITEM hItem, Phantom::B2Body* b);
	HTREEITEM			m_hBoxBodyList, m_hBoxJointList;
#endif
	//
	void				OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp);
	VOID				OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem);	//更新选择树形控件
	VOID				OnChangeTreeSelect();	//更新选择树形控件
	virtual	void		OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI);
	virtual	void		OnCommandUI(INT nMenuID);
	void				OnTreeViewKeyDown(INT nChar);
	//
	VOID				BuildAll();
	//
	VOID				InitEffectProp();
	VOID				SetModifyed(BOOL bModifyed = TRUE);	//设置修改状态
	//
	int					GetEditorMode(){return m_nCurrentEditorMode;}			//是否编辑控件模式
	VOID				SetEditorMode(int mode);	//设置编辑模式
	//
	VOID				OnTreeSelectGroup(Phantom::UIGroup* group);
	VOID				OnTreeSelectControl(Phantom::UIControl* c);
	VOID				ResetCenter();
	//
	virtual	BOOL				IsExportPak(){return true;}
	virtual	VOID				ExportPak();
	//
	enum EditorMode
	{
		EditorMode_None	=	0,
		EditorMode_Default,
		EditorMode_TerrainFloor,
		EditorMode_TerrainUnit,
	};
	//
	BOOL				IsEditorFloorMode(){return (m_nCurrentEditorMode == EditorMode_TerrainFloor);}			//是否编辑控件模式
	BOOL				IsEditorUnitMode(){return (m_nCurrentEditorMode == EditorMode_TerrainUnit);}			//是否编辑控件模式
	BOOL				IsEditorDefaultMode(){return (m_nCurrentEditorMode == EditorMode_Default);}			//是否编辑控件模式
	VOID				SetEditorFlags(int type);
	//
	virtual	VOID		GetNewFileName(wchar_t* szFile);
	INT					m_nCurrentEditorMode;
	int					m_nEditorFlags;
	//
	float				m_fFloorSetRadius;
	//
	BOOL				m_bDisableMoveMode;
	MapGridInfo			m_gridInfo;
	GCPropertyBase		m_dlgProp;
	GCPropertyBase		m_controlProp;
	GCPropertyBase		m_imageProp;
	GCPropertyBase		m_animationProp;
	GCPropertyBase		m_uvmapProp;
	GCPropertyBase		m_texAniProp;
	GCPropertyBase		m_groupProp;
	GCPropertyBase		m_uvmapRectProp;
	
	//
	GCPropertyBase		m_bodyProp;
	GCPropertyBase		m_jointProp;
	GCPropertyBase		m_shapeProp;
	//
	VOID				RefreshImageScale();
	VOID				InitDialogProp();
	VOID				InitControlProp();
	VOID				InitImageProp();
	VOID				InitAnimationProp();
	VOID				InitUvmapProp();
	VOID				InitTexAniProp();
	VOID				InitGroupProp();
	//
	VOID				SetCurFrame(float fFrame);
	//
	virtual	VOID		OnCopyTree();
	virtual	VOID		OnPasteTree();
	virtual	BOOL				IsCopyTree();
	virtual	BOOL				IsPasteTree();
	virtual	VOID				OnDeleteTree();
	virtual	BOOL				IsDeleteTree();
	//
	virtual	VOID		SetShowSelectOnly(BOOL b);
	std::map<int, BOOL>	m_controlVisibleMap;
	VOID				RefreshAnimations(Phantom::UIControl* ele, HTREEITEM hAnimationList);
	VOID				RefreshUVAnimations(Phantom::UIControl* ele, Phantom::UIElement* element, int eIndex, HTREEITEM hUVAnimationList);
	VOID				RefreshUVRects(Phantom::UIControl* ele, Phantom::UIElement* element, int eIndex, Phantom::UVAnimation* uvAni, int aIndex, HTREEITEM hUVAnimation);
	VOID				RefreshGroup(int nGroup);
	VOID				RefreshControl(Phantom::UIControl* control, HTREEITEM hControl);
	VOID				RefreshElement(Phantom::UIControl* ele, HTREEITEM hImageList);
	//更新事件列表
	VOID				RefreshEvents(Phantom::UIControl* ele, HTREEITEM hImageList);
	VOID				AddSelectToTree(Phantom::UIControl* ele);
	//
	std::map<int, HTREEITEM>	m_groupItems;
	GCAnimationCtrl	m_trackCtrl;
	GCFloatAnimation	m_floatAni;
	//EffectProp
	PropertyPtr			m_propEffectFrameCount;
	INT					GetControlIcon(INT nControlType);
	//
	//dialog3d properties
	BOOL				m_bAlwaysVisible;
	PropertyPtr			m_dlgAlwaysVisible;
	PropertyPtr			m_dlgAlignLR;
	PropertyPtr			m_dlgAlignTB;
	PropertyPtr			m_dlgOffsetX;
	PropertyPtr			m_dlgOffsetY;
	PropertyPtr			m_dlgViewScale;
	PropertyPtr			m_dlgIs2DGame;
	PropertyPtr			m_dlgRecalcRects;
	//PropertyPtr			m_dlgViewScaleX;
	//PropertyPtr			m_dlgViewScaleY;
	PropertyPtr			m_dlgAnimationEnable;	//跟着控件一起动画
	PropertyPtr			m_dlgX;
	PropertyPtr			m_dlgY;
	PropertyColorPtr	m_dlgColor;
	PropertyPtr			m_dlgAlpha;
	//
	PropertyPtr			m_dlgMapEnable;
	PropertyPtr			m_dlgMap25DMode;
	PropertyPtr			m_dlgMapSizeX;
	PropertyPtr			m_dlgMapSizeY;
	PropertyPtr			m_dlgMapCountX;
	PropertyPtr			m_dlgMapCountY;
	PropertyPtr			m_dlgMapOffsetX;
	PropertyPtr			m_dlgMapOffsetY;
	//control3D properties
	//view only
	PropertyPtr			m_controlID;
	//modify
	PropertyPtr			m_controlToolTipText;
	PropertyPtr			m_controlToolTipAlign;
	//
	PropertyPtr			m_controlName;
	PropertyPtr			m_controlLeft;
	PropertyPtr			m_controlTop;
	PropertyPtr			m_controlWidth;
	PropertyPtr			m_controlHeight;
	//
	PropertyPtr			m_controlAlignModeW,m_controlAlignModeH;
	PropertyPtr			m_controlScale;	//按比例缩放值
	PropertyPtr			m_controlMoveDialog;//是否可以控制对话框移动
	PropertyPtr			m_controlBackground;//是否可以控制对话框移动
	PropertyPtr			m_controlLayer;	//层级
	PropertyPtr			m_controlIsGrid;	//层级
	PropertyPtr			m_controlGridOffset;	//层级
	PropertyPtr			m_controlContext;	//层级
	PropertyPtr			m_controlVisibleOnScript;
	PropertyPtr			m_controlTextGroup;
	PropertyPtr			m_controlRotGroup;
	PropertyPtr			m_controlTipGroup;
	PropertyPtr			m_controlTextAlignLR;
	PropertyPtr			m_controlTextAlignTB;
	PropertyPtr			m_controlImageFont;
	PropertyPtr			m_controlVisible;
	PropertyPtr			m_controlCanActive;
	PropertyPtr			m_controlEnable;
	PropertyPtr			m_controlSelect;
	PropertyPtr			m_controlAniCenter;//是否以中心点
	PropertyPtr			m_controlEnableImgText;
	PropertyPtr			m_controlImgTextBegin;
	PropertyPtr			m_controlImgSizeAdd;
	//PropertyPtr			m_controlShadowWidth;
	//PropertyColorPtr	m_controlShadowColor;
	//PropertyPtr			m_controlShadowAlpha;
	PropertyPtr			m_controlText;
	PropertyColorPtr	m_controlTextColor;
	PropertyFontPtr		m_controlFont;	//设置字体
	PropertyColorPtr	m_controlTextureColor;
	PropertyPtr			m_controlTextAlpha;
	PropertyColorPtr	m_controlDisableColor;
	PropertyPtr			m_controlTextureAlpha;
	PropertyPtr			m_controlTabIndex;	//TAB顺序
	PropertyPtr			m_controlCursorName;
	PropertyPtr			m_controlGroupID;
	PropertyPtr			m_controlParent;
	PropertyPtr			m_controlPixel;//按照像素精确点击
	//
	PropertyPtr			m_controlRotation;
	PropertyPtr			m_controlCenterX;
	PropertyPtr			m_controlCenterY;
	PropertyPtr			m_controlAngle;
	PropertyPtr			m_controlDrawFocus;
	PropertyPtr			m_controlSleepTime;
	PropertyPtr			m_controlUnitAngle8;	//是否使用8个方向
	PropertyPtr			m_controlIs2DUnit;
	PropertyPtr			m_controlIsGroupCenter;
	//editor
	PropertyPtr			m_editGroup;
	PropertyPtr			m_editShowBorder;
	PropertyPtr			m_editPassword;
	PropertyPtr			m_editEnableIME;
	PropertyPtr			m_editReadOnly;
	PropertyPtr			m_editIndicator;
	PropertyPtr			m_editCandList;
	PropertyPtr			m_editBorderWidth;
	PropertyPtr			m_editSBWidth;	//
	//
	PropertyColorPtr	m_editCaretColor;
	PropertyColorPtr	m_editSelectTextColor;
	PropertyColorPtr	m_editSelectBackColor;
	//progress
	PropertyPtr			m_progressGroup;
	PropertyPtr			m_progressMode;	//进度类型
	PropertyPtr			m_progressValue;	//进度类型
	PropertyPtr			m_progressSize;	//进度类型
	//combobox
	PropertyPtr			m_comboGroup;
	PropertyPtr			m_comboDropHeight;	//
	PropertyListPtr		m_comboValues;	//成员列表
	//format edit
	PropertyPtr			m_formatEditGroup;
	PropertyColorPtr	m_formatEditEventColor;	//
	PropertyColorPtr	m_formatEditSelectColor;	//
	PropertyPtr			m_formatEditLineHeight;	//
	//listbox
	PropertyPtr			m_listboxGroup;
	PropertyPtr			m_listboxLineHeight;	//
	PropertyPtr			m_listboxTextLeft;	//
	PropertyListPtr		m_listboxValues;	//成员列表
	//listctrl
	PropertyPtr			m_listctrlGroup;
	PropertyPtr			m_listctrlLineHeight;	//
	PropertyPtr			m_listctrlTextLeft;	//
	PropertyPtr			m_listctrlSortEnable;	//
	PropertyPtr			m_listctrlColumnHeight;	//
	PropertyListPtr		m_listctrlColumns;	//成员列表
	PropertyListPtr		m_listctrlItems;	//成员列表
	//
	PropertyPtr			m_terrainGroup;
	PropertyPtr			m_terrainElementW;
	PropertyPtr			m_terrainElementH;
	PropertyPtr			m_terrainE_RenderLines;
	PropertyPtr			m_terrainE_RenderBlends;
	PropertyPtr			m_terrainE_GridText;
	PropertyPtr			m_terrainE_MapTexture;
	PropertyPtr			m_terrainE_AutoMoveMap;
	PropertyPtr			m_terrainE_RenderObject;
	PropertyPtr			m_terrainE_RenderFloor;
	PropertyPtr			m_terrainE_RenderEvent;
	PropertyPtr			m_terrainE_RenderCenters;
	PropertyPtr			m_terrainMapScale;
	PropertyPtr			m_terrainAutoMoveSpeed;
	PropertyPtr			m_terrainAutoMoveSize;
	PropertyPtr			m_terrainUnitSizeW;
	PropertyPtr			m_terrainUnitSizeH;
	//unit2d
	PropertyPtr			m_unit2DGroup;
	PropertyPtr			m_unit2DTextureCount;
	PropertyPtr			m_unit2DCenterPosX;
	PropertyPtr			m_unit2DCenterPosY;
	PropertyPtr			m_unit2DRenderGrid;
	//checkbox
	PropertyPtr			m_checkboxGroup;	//
	PropertyPtr			m_checkboxChecked;	//
	//effect
	PropertyPtr			m_effectGroup;	//
	PropertyPtr			m_effectFrameCount;	//
	PropertyPtr			m_effectLoopCount;	//
	PropertyPtr			m_effectPlaySpeed;	//
	//static
	PropertyPtr			m_staticGroup;	//
	PropertyPtr			m_staticBorderWidth;	//
	//PropertyPtr			m_staticMouseSrcBlend;
	//PropertyPtr			m_staticMouseDestBlend;
	//PropertyPtr			m_staticMouseEnable;
	PropertyColorPtr	m_staticTextureColor;
	PropertyPtr			m_staticTextureAlpha;
	//button
	PropertyPtr			m_buttonGroup;
	PropertyFilePtr		m_buttonClickSound;
	PropertyFilePtr		m_buttonMouseMoveSound;
	//
	PropertyPtr			m_buttonClickSoundClear;
	PropertyPtr			m_buttonMouseMoveSoundClear;
	PropertyPtr			m_buttonType;
	//texture
	PropertyFilePtr		m_imageSet;	//设置图片
	PropertyPtr			m_imageClear;	//设置空图片
	PropertyPtr			m_imageDefault;	//设置空图片
	PropertyPtr			m_imageOpen;	//打开图片
	//
	PropertyPtr			m_imageName;		//设置名称
	PropertyPtr			m_imageUnitSide;	//设置单位位置
	PropertyPtr			m_imageRandomMode;	//设置单位位置
	//
	PropertyPtr			m_imageScale;		//是否支持缩放
	PropertyPtr			m_imageContScale;	//是否四边扩散方式所仿
	PropertyPtr			m_imageContW;	//是否四边扩散方式所仿
	PropertyPtr			m_imageContH;	//是否四边扩散方式所仿
	//
	PropertyPtr			m_imageTileW;	//是否四边扩散方式所仿
	PropertyPtr			m_imageTileH;	//是否四边扩散方式所仿
	PropertyPtr			m_imageTileOffX;	//是否四边扩散方式所仿
	PropertyPtr			m_imageTileOffY;	//是否四边扩散方式所仿
	PropertyPtr			m_imageTileScaleX;	//是否四边扩散方式所仿
	PropertyPtr			m_imageTileScaleY;	//是否四边扩散方式所仿
	PropertyPtr			m_imageTileLingXing;
	//
	PropertyPtr			m_imageLeft;
	PropertyPtr			m_imageTop;
	PropertyPtr			m_imageWidth;
	PropertyPtr			m_imageHeight;
	PropertyPtr			m_imageLists;
	//
	PropertyPtr			m_imageOffsetLeft;
	PropertyPtr			m_imageOffsetTop;
	PropertyPtr			m_imageOffsetWidth;
	PropertyPtr			m_imageOffsetHeight;
	//
	PropertyPtr			m_imageRegetRect;
	PropertyColorPtr	m_imageTextureColor;
	PropertyPtr			m_imageTextureAlpha;
	PropertyColorPtr	m_imageTextColor;
	PropertyPtr			m_imageTextAlpha;
	PropertyColorPtr	m_imageShadowColor;
	PropertyPtr			m_imageShadowAlpha;
	PropertyPtr			m_imageColorOP;
	PropertyPtr			m_imageSrcBlend;
	PropertyPtr			m_imageDestBlend;
	PropertyPtr			m_imageFilter;
	PropertyPtr			m_imageInvW;//横向镜像
	PropertyPtr			m_imageInvH;//横向镜像
	//
	PropertyPtr			m_imageUCount;	//贴图动画设置
	PropertyPtr			m_imageVCount;	//
	PropertyPtr			m_imageUVPlaySpeed;	//每行的高度
	//
	PropertyPtr			m_groupName;
	PropertyPtr			m_groupVisible;
	PropertyPtr			m_groupIsUnit;	//自动排序不？
	PropertyPtr			m_groupSleepTime;
	PropertyPtr			m_groupEnable;
	PropertyPtr			m_groupAngle;
	PropertyPtr			m_groupSpeed;
	PropertyPtr			m_groupAction;
	//PropertyPtr			m_groupIsDiv;//是否作为独立控件组使用，在html构造的时候将会在组合内部构造所有控件，这样可以作为list的单行来使用,其layer是内部控件的最大个layer
	//animation
	//PropertyPtr			m_animationAdd;
	//PropertyPtr			m_animationRemove;
	//PropertyPtr			m_animationPlay;
	PropertyPtr			m_animationName;
	PropertyPtr			m_animationFrameCount;
	PropertyPtr			m_animationIsDefault;
	PropertyPtr			m_animationHideAtSleep;
	PropertyPtr			m_animationHideOnOver;
	PropertyPtr			m_animationSpeed;
	PropertyPtr			m_animationSleepTime;
	PropertyPtr			m_animationElement;
	PropertyPtr			m_animationElementAni;
	PropertyPtr			m_animationLoopCount;
	PropertyPtr			m_animationWaveSleepTime;
	PropertyPtr			m_animationNextType;
	PropertyPtr			m_animationType;
	PropertyPtr			m_animationWaitEnable;
	//
	PropertyFilePtr		m_animationWaveFile;
	PropertyPtr			m_animationWaveFileClear;
	PropertyPtr			m_animationWaveVolume;
	PropertyPtr			m_animationWaveSpeed;
	//
#ifdef USE_BOX
	VOID				InitBodyProp();
	VOID				InitShapeProp();
	//body
	PropertyPtr			m_body_id;
	PropertyPtr			m_body_name;
	PropertyPtr			m_body_bodyType;//b2BodyType
	PropertyPtr			m_body_position_x,m_body_position_y;
	PropertyPtr			m_body_angle;
	PropertyPtr			m_body_linearVelocity_x,m_body_linearVelocity_y;
	PropertyPtr			m_body_angularVelocity;
	PropertyPtr			m_body_linearDamping;
	PropertyPtr			m_body_angularDamping;
	PropertyPtr			m_body_gravityScale;
	PropertyPtr			m_body_allowSleep;
	PropertyPtr			m_body_awake;
	PropertyPtr			m_body_fixedRotation;
	PropertyPtr			m_body_bullet;
	PropertyPtr			m_body_active;
	//shape
	PropertyPtr			m_shape_id;
	PropertyPtr			m_shape_type;//b2Shape::Type
	PropertyPtr			m_shape_control;//b2Shape::Type
	PropertyPtr			m_shape_m_density;
	PropertyPtr			m_shape_m_radius;//
	PropertyPtr			m_shape_m_p_x,m_shape_m_p_y;//position
	PropertyPtr			m_shape_m_fAngle;//box
	PropertyPtr			m_shape_m_boxWidth;//box
	PropertyPtr			m_shape_m_boxHeight;//box
	PropertyPtr			m_shape_m_bChainLoop;
	PropertyPtr			m_shape_m_hasVertex0;
	PropertyPtr			m_shape_m_hasVertex3;
	VOID				OnShapeMode(Phantom::B2Shape* shape);
	//joint
	PropertyPtr					m_joint_id;
	PropertyPtr					m_joint_name;
	PropertyPtr					m_joint_jointType;//b2JointType
	//
	PropertyPtr					m_joint_bodyA;
	PropertyPtr					m_joint_bodyB;
	PropertyPtr					m_joint_collideConnected;
	PropertyPtr						m_joint_localAnchorA_x,m_joint_localAnchorA_y;//锚and prismatic ,pulley, revolute
	PropertyPtr						m_joint_localAnchorB_x,m_joint_localAnchorB_y;//锚and prismatic ,pulley, revolute
	PropertyPtr					m_joint_maxForce;//强制and mouse
	PropertyPtr					m_joint_maxTorque;//转矩
	PropertyPtr					m_joint_joint1;
	PropertyPtr					m_joint_joint2;//id
	PropertyPtr					m_joint_ratio;//or mouse dumping Ratio, or pulley radio
	PropertyPtr						m_joint_target_x,m_joint_target_y;
	PropertyPtr					m_joint_frequencyHz;
	PropertyPtr						m_joint_localAxisA_x,m_joint_localAxisA_y;//轴
	PropertyPtr					m_joint_referenceAngle;//revolute
	PropertyPtr					m_joint_enableLimit;//revolute
	PropertyPtr					m_joint_lowerTranslation;
	PropertyPtr					m_joint_upperTranslation;
	PropertyPtr					m_joint_enableMotor;//revolute
	PropertyPtr					m_joint_maxMotorForce;
	PropertyPtr					m_joint_motorSpeed;//and revolute
	//pulley joint only滑轮
	PropertyPtr						m_joint_groundAnchorA_x,m_joint_groundAnchorA_y;
	PropertyPtr						m_joint_groundAnchorB_x,m_joint_groundAnchorB_y;
	PropertyPtr					m_joint_lengthA;
	PropertyPtr					m_joint_lengthB;
	PropertyPtr					m_joint_lowerAngle;
	PropertyPtr					m_joint_upperAngle;
	PropertyPtr					m_joint_maxMotorTorque;
	PropertyPtr					m_joint_maxLength;
	VOID				OnJointMode(Phantom::B2Joint* joint);
	VOID				InitJointProp();
#endif
	//GCPropertyBase			m_bodyProp;
	//GCPropertyBase			m_jointProp;
	//GCPropertyBase			m_shapeProp;
	//
	//播放参数
	virtual	VOID		PlayAnimation();
	virtual	VOID		StopAnimation();
	virtual	VOID		PauseAnimation(BOOL bPause);
	virtual	VOID		SetAnimationLoop(BOOL bLoop);
	virtual	BOOL		IsAnimationCanPlayStop();
	virtual	BOOL		IsAnimationCanPause();
	virtual	BOOL		IsAnimationCanLoop();
	virtual	BOOL		IsAnimationPlaying();
	virtual	BOOL		IsAnimationPause();
	virtual	BOOL		IsAnimationLoop();
	//
	GCTrackEditor		m_animationTrack;
	enum{
		UI_VALUE_TEXT_COLOR,
		UI_VALUE_TEXT_ALPHA,
		UI_VALUE_TEXTURE_COLOR,
		UI_VALUE_TEXTURE_ALPHA,
		UI_VALUE_Position,
		UI_VALUE_WidthHeight,
		UI_VALUE_Center,
		UI_VALUE_Rotation,
		//
		UI_VALUE_TextureColorMin,
		UI_VALUE_OffsetMin,
		UI_VALUE_SizeMin,
		UI_VALUE_RotationMin,
		UI_VALUE_LifeMin,
		UI_VALUE_EmitCount,
	};
	//PropertyPtr			m_animationAutoPlay;
	//PropertyPtr			m_animationIsTrack;
	//PropertyPtr			m_animationPlayTime;
	//PropertyPtr			m_animationTrackES;	//每秒添加轨迹
	//
	PropertyPtr			m_uvmapName;	//激活并播放
	PropertyPtr			m_uvmapKeyLoopCount;
	PropertyPtr			m_uvmapKeyPlaySpeed;
	PropertyPtr			m_uvmapBeginIndex;
	PropertyPtr			m_uvmapFrameCount;
	PropertyPtr			m_uvmapDefault;
	PropertyPtr			m_uvmapAngle;	//８个方向的角度,2D单位有效
	//
	PropertyPtr			m_uvmapCustomEnable;
	PropertyPtr			m_uvmapLayerAdd;
	PropertyPtr			m_uvmapCustomGroup;
	PropertyPtr			m_uvmapAutoImages;
	PropertyPtr			m_uvmapInvW;//横向镜像
	PropertyPtr			m_uvmapInvH;//横向镜像
	PropertyFilePtr		m_uvmapSet;	//设置图片
	PropertyPtr			m_uvmapClear;	//设置空图片
	PropertyPtr			m_uvmapLeft;
	PropertyPtr			m_uvmapTop;
	PropertyPtr			m_uvmapWidth;
	PropertyPtr			m_uvmapHeight;
	PropertyPtr			m_uvmapOffsetX;
	PropertyPtr			m_uvmapOffsetY;
	PropertyFilePtr		m_uvmapTexture;
	PropertyPtr			m_uvmapTextureClear;
	PropertyPtr			m_uvmapImageLists;
	//
	PropertyPtr			m_uvmapUCount;	//贴图动画设置
	PropertyPtr			m_uvmapVCount;	//
	PropertyPtr			m_uvmapActionType;	//
	PropertyPtr			m_uvmapNextActionType;	//
	//
	PropertyPtr			m_uvmapNextActionRand;	//
	PropertyPtr			m_uvmapMoveOffset;	//
	PropertyPtr			m_uvmapMoveOffsetX;	//
	PropertyPtr			m_uvmapMoveOffsetY;	//
	PropertyPtr			m_uvmapMoveOffsetSpeed;	//
	PropertyPtr			m_uvmapRegetRects;	//
	//Text Animation
	PropertyPtr			m_textAniEnable;
	PropertyPtr			m_textAniCount;
	PropertyPtr			m_textAniTextShowSpeed;
	PropertyPtr			m_textAniLineShowSpeed;
	PropertyPtr			m_textAniMaxCount;
	PropertyPtr			m_textAniPlay;
	//PropertyPtr			m_animationAddKey;
	//PropertyPtr			m_animationRemoveKey;
	//关键帧
	PropertyPtr			m_keyContinueTime;
	PropertyPtr			m_keyPlaySpeed;
	PropertyPtr			m_keyTextAlpha;
	PropertyPtr			m_keyTextureAlpha;
	PropertyPtr			m_keyTextColor;
	PropertyPtr			m_keyTextureColor;
	PropertyPtr			m_keyLeft;
	PropertyPtr			m_keyTop;
	PropertyPtr			m_keyWidth;
	PropertyPtr			m_keyHeight;
	PropertyPtr			m_keyCenterX;
	PropertyPtr			m_keyCenterY;
	PropertyPtr			m_keyRotationEnable;
	PropertyPtr			m_keyAngle;
	BOOL				m_bSelectElementMode;
	//ImageTexture,StaticAnimation3D
	//PropertyPtr			m_uvmapAdd;
	//PropertyPtr			m_uvmapRemove;
	//PropertyPtr			m_uvmapClone;
	//
#ifdef USE_BOX
	struct	BodySelect
	{
		Phantom::B2Body*			bodyPtr;
		Phantom::B2Joint*		jointPtr;
		Phantom::Pixel			downPoint;
		Phantom::Pixel			offset;
		int				nSide;
		BodySelect(){bodyPtr = 0; jointPtr = 0; nSide = 0; downPoint.Set(0, 0);offset.Set(0, 0);}
	};
#endif
	virtual	VOID		OnUndoRedo(UndoRedoBuffer* buffer, int ur_mode);
	VOID				OffsetControls();	//显示移动控件对话框
#ifdef USE_BOX
	Phantom::float2				PtoB(const Phantom::Pixel& p);	//Pixel to Box2D
	Phantom::Pixel				BtoP(const Phantom::float2& v);
	float				PtoB(int);	//Pixel to Box2D
	int					BtoP(float);
	BOOL				GetMovePointRect(Phantom::B2Body* b, const Phantom::Pixel& cursor, Phantom::Pixel& ret, int & nSide);
	BOOL				GetMovePointRect(Phantom::B2Joint* b, const Phantom::Pixel& cursor, Phantom::Pixel& ret, int & nSide);
	BOOL				PickBox2D(BodySelect& ret, const Phantom::Pixel& cur);
	BOOL				OnMoveBody(BodySelect& bs, const Phantom::Pixel& newPos);
	BOOL				IsPtIn(const Phantom::float2& v, const Phantom::Pixel& cursor,  Phantom::Pixel& ret);
	VOID				RenderRect(const Phantom::float2& v);
	VOID				RenderRect(Phantom::B2Body* b);
#endif
	VOID				GenericCPPCode(BOOL bByName = false);	//创建可以在IPhone系统开始时使用的对话框自动初始化代码
	VOID				GenericHtml5Code();	//创建html5格式的读取信息文件. js文件
	VOID				GenericUIECode();	//

public:
#ifdef USE_BOX
	std::vector<BodySelect>	m_selectBodys;
#endif
	//
	Phantom::Pixel				m_downCenter;
	BOOL				m_bShowCenter;
	Phantom::Pixel				m_downDlg;
	BOOL				m_bDownMButton;
	BOOL				m_bSetLevelMode;
	Phantom::Dialog		m_dialogPtr;
};

