//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   GCSceneEditor3D.h
	软件名称   :   幻影游戏引擎

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	
	Copyright 2009-2016 Zhao Dexian
	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___GCSCENEEDITOR3D__H___
#define ___GCSCENEEDITOR3D__H___


#pragma once
#include "GCViewBase.h"
#include "TrackAnimationCtrl.h"
#include "ViewLayerImages.h"
#include "ScaleAxis.h"
#include "RotationAxis.h"
#include "DlgSceneObject.h"
#include "MyLuaEditor.h"

struct	ImageItem;

class GCSceneEditor3D;

//typedef	VOID	(*ProSceneShadow)(Phantom::Scene3D* scene, SceneShadowData* data, LPARAM nParameter, LPARAM nParameter2);
//typedef	VOID	(*ProSceneAdvanced)(Phantom::Scene3D* scene, IAdvancedEffect* data, LPARAM nParameter, LPARAM nParameter2);
//
typedef	VOID	(*ProSceneSky)(Phantom::Scene3D* scene, Phantom::SceneSkyData* data, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*ProSceneFlare)(Phantom::Scene3D* scene, Phantom::SceneSkyData* data, INT nFlareIndex, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*ProSceneCamera)(Phantom::Scene3D* scene, Phantom::SceneCameraData* data, LPARAM nParameter, LPARAM nParameter2, UndoPtr* ptr);
//typedef	VOID	(*ProSceneGrass)(Phantom::Scene3D* scene, ISceneGrassTexture* data, LPARAM nParameter, LPARAM nParameter2);
//typedef	VOID	(*ProSceneWater)(Phantom::Scene3D* scene, SceneWaterData* data, LPARAM nParameter, LPARAM nParameter2);
//typedef	VOID	(*ProSceneAnimation)(Phantom::Scene3D* scene, ISCA_Animation* ani, LPARAM nParameter, LPARAM nParameter2);
//typedef	VOID	(*ProSceneSCAs)(Phantom::Scene3D* scene, ISCA_Animation* ani, ISCA_Base* base, LPARAM nParameter, LPARAM nParameter2);
//typedef	VOID	(*ProSceneSCATracks)(Phantom::Scene3D* scene, ISCA_Animation* ani, ISCA_Base* base, ISCA_Track* t, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*ProSceneObject)(Phantom::Scene3D* scene, Phantom::SceneObjectFile* data, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*ProSceneEnv)(Phantom::Scene3D* scene, Phantom::SceneEnv* data, LPARAM nParameter, LPARAM nParameter2, UndoPtr* ptr);
typedef	VOID	(*ProSceneRegion)(Phantom::Scene3D* scene, Phantom::SceneRegionData* data, LPARAM nParameter, LPARAM nParameter2, UndoPtr* ptr);
typedef	VOID	(*ProSceneLight)(Phantom::Scene3D* scene, Phantom::SceneLight* data, LPARAM nParameter, LPARAM nParameter2, UndoPtr* ptr);
struct SelectEditor;
typedef	VOID	(*ProSceneUnit)(Phantom::Scene3D* scene, SelectEditor& se, LPARAM nParameter, LPARAM nParameter2, UndoPtr* ptr);
struct	SceneEditorInfo
{
	float						fVar;
	float						fNormalVar;
	float						fPower;
	bool						bEnable;
	float						fMinValue;
	float						fMaxValue;
	Phantom::color4						color;
	SceneEditorInfo(){fVar	=	fNormalVar	=	1.0f; fPower = 1.0f; bEnable = false; fMinValue = 0.0f; fMaxValue = 1.0f;color.setRGBA(1,1,1,1);}
};

typedef	VOID	(*ProTerrainGrids)(GCSceneEditor3D* dlg, INT nGridX, INT nGridY, const Phantom::float3& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info);
typedef	VOID	(*ProTerrainGridsSaved)(GCSceneEditor3D* dlg, INT nGridX, INT nGridY, const Phantom::float3& gridMiddle, const Phantom::float3& dir, Phantom::GVertexData& valRet, SceneEditorInfo& info);

typedef	VOID	(*ProTerrainTextureGrids)(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info);
typedef	VOID	(*ProTerrainTextureGridsSaved)(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info, float& valRet);

struct GridSaveValue
{
	Phantom::GVertexData		val;
	float				f;
	int					gridX;
	int					gridY;
};

enum{
	EventType_SceneEvent = 1,//场景事件类型
};

struct BrushImage
{
public:
	CString			name;
	BrushImage(){heightBuffer = 0;}
	~BrushImage(){if(heightBuffer)free(heightBuffer);}
	float			GetHeight(float fX, float fY);
	float			GetHeightPixel(INT nX, INT nY);
protected:
	friend class	CBrushConfigDLG;
	CImage			imageSmall;
	//
	VOID			LoadBuffer();
	VOID			FreeBuffer();
	//
	CString			fileName;
	unsigned char*	heightBuffer;
	INT				width, height;
};

#define	MAX_KEY_PARAMETER		20
#pragma pack(push,1)
struct	SSData{
	Phantom::float3 m_downPosition,m_downRotation,m_downScale;
};
#pragma pack(pop)
struct	SelectEditor	//选择的物体数据
{
	SelectEditor();
	VOID						Init(Phantom::SceneObject* obj);
	VOID						Init(Phantom::ScenePoint* evt);
	VOID						Init(Phantom::SceneLight* light, int bTarget);
	//
	BOOL						CanMove();//是否可以移动
	BOOL						CanRotation();//是否可以转动
	BOOL						CanScale();//是否可以缩放
	VOID						BeginEditor(AxisOperatorType ot);
	VOID						EndEditor(AxisOperatorType ot, const Phantom::float3& val);
	VOID						OnEditor(AxisOperatorType ot, const Phantom::float3& val);
	BOOL						Clone(SelectEditor& ret);
	BOOL						GetObjectBox(Phantom::AABox& ret, Phantom::float3& off, Phantom::matrix4x4* newMat = 0);
	BOOL						IsEnabled();
	BOOL						IsVisible();
	VOID						SetEnabled(BOOL b);
	VOID						SetVisible(BOOL b);
	VOID						SetSize(float f);
	float						GetSize();
	const Phantom::float3&		GetPosition();
	VOID						SetPosition(const Phantom::float3& v);
	const char*					GetName();
	VOID						SetName(const char* s, BOOL bCustom);
	BOOL						IsCustomName();
	BOOL						GetAABox(Phantom::AABox& ret);
	inline	Phantom::SceneObject*	GetObjectPtr(){return m_objectPtr;}
	inline	Phantom::ScenePoint*	GetEventPtr(){return m_eventPtr;}
	//
	inline Phantom::float3&		GetScale(){return m_downScale;}
	inline Phantom::float3&		GetRotation(){return m_downRotation;}
	VOID						SetScale(const Phantom::float3& s);
	VOID						SetRotation(const Phantom::float3& r);
	INT							GetID();
	BOOL	operator	==(const SelectEditor& ed);
	//
	Phantom::matrix4x4			m_newMatrix;
	Phantom::matrix4x4			m_downMatrix;
	Phantom::float3				m_downPosition,m_downRotation,m_downScale;
	Phantom::ScenePoint*		m_eventPtr;//事件类型
	Phantom::SceneObject*		m_objectPtr;//对象类型
	Phantom::SceneLight*		m_lightPtr;//灯光对象
	char						m_bLightTarget;
};
class GCSceneEditor3D: public GCViewBase, public IAxisEditCallback, public ITrackAnimationCallback
{
public:
	GCSceneEditor3D(CPhantomGCDoc* docPtr);
	~GCSceneEditor3D(void);
	virtual	VOID				OnActive();		//被激活
	virtual	VOID				OnDeActive();	//失去激活
	virtual	VOID				CloseFile();
	virtual	BOOL				OpenFile(const wchar_t* szFile);	//打开文件,如果失败则会自动删除
	virtual	BOOL				SaveFile(BOOL bSaveAs=false);	//保存文件
	virtual void				OnFrameMove();	//每次更新
	virtual void				OnPushRenderObjects();	//渲染
	virtual	FileViewerType		GetViewerType(){return FileViewerType_GCSceneEditor3D;}
	virtual	HICON				GetViewerIcon();
	virtual void				OnRenderingEnd();	//渲染
	void						OnDoubleClick(CMTreeCtrl* treePtr, HTREEITEM hItem);	//更新选择树形控件
	//
	virtual	INT					GetPopupMenuID(){return 13;}
	VOID						RebuildTrees();
	//
	VOID						OnUndoRedo(UndoRedoBuffer* buffer, int ur_mode);
	VOID						LoopAllSkys(ProSceneSky proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
//	VOID						LoopAllShadows(ProSceneShadow proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
//	VOID						LoopAllAdvanceds(ProSceneAdvanced proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
	//
	VOID						LoopAllFlares(ProSceneFlare proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
	VOID						LoopAllCameras(ProSceneCamera proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
//	VOID						LoopAllWaters(ProSceneWater proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
//	VOID						LoopAllGrass(ProSceneGrass proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
	VOID						LoopAllEnv(ProSceneEnv proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
	VOID						LoopAllRegion(ProSceneRegion proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
	VOID						LoopAllLight(ProSceneLight proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
	//
//	VOID						LoopAllAnimations(ProSceneAnimation proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
//	VOID						LoopAllSCAs(ProSceneSCAs proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
//	VOID						LoopAllSCATracks(ProSceneSCATracks proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
	//
	VOID						LoopAllObjects(ProSceneObject proc, LPARAM nParameter = 0, BOOL bRename = FALSE, LPARAM nParameter2 = 0);
	VOID						LoopAllUnits(ProSceneUnit proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0);
	//
	VOID						InsertGeometry(const wchar_t* szFileName);
	//
	virtual	VOID				AlignObjects(AlignType type, int value);
	virtual	INT					GetAlignValue(AlignType type);
	virtual	BOOL				IsAlignEnable();
	INT							GetKeyFrameCount();
	BOOL						OnTrackChange(INT id, INT nBeginFrame, INT nEndFrame);	//如果返回TRUE表示可以被改变
	VOID						EnumTracks(std::vector<TrackItem>& rets);
	//
	enum EditorMode
	{
		EditorMode_None			=	0,
		EditorMode_HeightAdd,		//地形能编辑模式
		EditorMode_HeightAvg,
		EditorMode_HeightAlign,
		EditorMode_HeightSet,
		EditorMode_HeightGridVisible,
		EditorMode_HeightGridEnable,
		EditorMode_HeightMapAdd,
		EditorMode_HeightMapSet,
		EditorMode_RegionSet,
		//
		EditorMode_TextureAdd,
		EditorMode_TextureSet,
		EditorMode_TextureAvg,
		EditorMode_TextureUVMoving,
		EditorMode_TextureUVScaling,
		EditorMode_TextureUVRotation,
		EditorMode_SetGridColor,
		//
		EditorMode_ObjectMove,
		EditorMode_ObjectScale,
		EditorMode_ObjectRotation,
		//
		EditorMode_AddGrass,
		EditorMode_RemoveGrass,
		EditorMode_LineEditorMode,
		//
		//EditorMode_TextureWaterAlpha,
		EditorMode_Count,
	};
	//
	//CSmallMapInfoDialog		m_smallMapInfo;
	//CShadowMapInfoDialog	m_shadowMapInfo;
	//CRenderBufferSettings	m_renderBufferInfo;
	//
	BOOL						IsDoEvent();
	EditorMode					m_editorMode;
	VOID						SetEditorMode(int mode);
	int							GetEditorMode(){return m_editorMode;}
	virtual	bool				OnWindowProc(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam);
	//
	const Phantom::color4&				GetWireframeColor(){return m_wireframeColor;}
	VOID						SetWireframeColor(const Phantom::color4& c){m_wireframeColor = c;}
	Phantom::color4						m_wireframeColor;
	//
	VOID						RenderCircle(const Phantom::float3& vCenter, float fSize, unsigned int color);
	VOID						DrawCircle(const Phantom::float3& vCenter, float fRadius, unsigned int color, int smooth = 64);
	VOID						RenderRect(const Phantom::float3& vCenter, float fRadius, unsigned int color, int smooth = 16);
	VOID						RenderLine(const Phantom::float3& from, const Phantom::float3& to, unsigned int color, float fGridSize2);
	VOID						OnSelectLayer(INT nLayer);
	void						OnSelectImage(ImageItem* pItem, INT nParameter);
	VOID						DrawOmni(const Phantom::float3& pos, unsigned int color, float fSize);
	//
	VOID						RefreshLightProp(int id);
public://格子编辑
	SceneEditorInfo				m_editorInfo[EditorMode_Count];
	//ITexture*					m_pointTexture;
	//
	VOID						SetPlay();
	VOID						SetStop();
	VOID						SetLoopMode(BOOL b);
	VOID						SetPause(BOOL b);
	BOOL						m_bIsPlaying;
	BOOL						m_bIsPause;
	BOOL						m_bLoopMode;
	//
//	VOID						PlayActive(ISCA_Animation* ani, ISCA_Base* b, ISCA_Track* t);
public://刷子
	enum BrushType{
		BrushType_Circle		=	0,
		BrushType_Box,
	};
	BrushType					m_brushType;
	BOOL						m_bUseBrushTexture;
	BOOL						m_bBrush01Mode;
	std::vector<BrushImage*>	m_brushs;
	BrushImage*					m_brushPtr;
	BOOL						m_bDoEvent;
	BOOL						m_bMouseMove;
	//
	CViewLayerImages			m_layerImages;
public://高度编辑模式
	float						m_fHeightRadius;
	POINT						m_vRefreshHeightPoints[10000];
	int							m_nHeightPointCount;
	//
	Phantom::PtrID						m_selectGrassPtrID;
	struct	SaveRegionV{
		Phantom::GVertexData data[REGION_VERTEX_COUNT_25d*REGION_VERTEX_COUNT_25d];
	};
	struct	SaveRegionT{
		Phantom::GridAttr25d data[REGION_VERTEX_COUNT_25d*REGION_VERTEX_COUNT_25d];
	};
	POINT						m_vEditRegions[10000];
	SaveRegionV*				m_savedData[10000];
	SaveRegionT*				m_savedDataT[10000];
	int							m_nEditRegionCount;
	VOID						SaveRegionD(SaveRegionV& v, Phantom::SceneBox* r);
	VOID						SaveRegionTs(SaveRegionT& v, Phantom::SceneBox* r);
	//
	Phantom::PickPoint				m_vHeightCenter;
	//POINT						m_vSelectRegionPoint;
	INT							m_nTextureBlendChannel;
	float						m_fCalculateTime;
	//
	//std::vector<Phantom::Pixel>		m_selectRegions;
	//
	virtual	Phantom::Scene3D*		GetStateScene(){return this->m_scene3D;;}
	bool						OnWindowProcHeight(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam);
	VOID						OnRenderingHeight();
	VOID						OnFrameMoveHeight();
	BOOL						GetHeightPoint(int nGridX, int nGridY, Phantom::float3& ret);
	float						GetHeightAmount(const Phantom::float3& dir, float fEditRange, float fPower);
	//
	VOID						RefreshLayerImages();
	//
	VOID						LoopHeightGrids(ProTerrainGrids proc, BOOL bIsRender = false);
	VOID						LoopHeightGridsSaved(ProTerrainGridsSaved proc);
	//
	VOID						LoopTextureRange(ProTerrainTextureGrids proc);
	VOID						LoopTextureSavedRange(ProTerrainTextureGridsSaved proc);
	//
	VOID						LoopMoveEnableRange(ProTerrainTextureGrids proc);
	virtual	VOID				GetNewFileName(wchar_t* szFile);
	//
	VOID						ShowRegions();
	VOID						ShowRegionTextures();
	BOOL						IsTextureEditorMode();
	//
	VOID						SetViewCenter();
public://linear editor
	bool						OnWindowProcLine(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam);
	VOID						OnRenderingLine();
	VOID						OnFrameMoveLine();
	//
	BOOL						BeginEditorLine(Phantom::matrix4x4& matOut);
	VOID						OnEditorLine(const Phantom::matrix4x4& downMat, const Phantom::matrix4x4& newMat);
	VOID						EndEditorLine(const Phantom::matrix4x4& downMat);
	//
	struct	LinearData
	{
		//ISCA_Animation* ani;
		//ISCA_Base*		base;
		//ISCA_Track*		track;
		//ISCC_Position*	sccPoint;
		int				id;
		Phantom::float3			downPos, newPos;
	};
	std::vector<LinearData>		m_selectLines;
	VOID						AddToLinear(LinearData* d);
	BOOL						IsSelectLinear(int id);
	VOID						DrawLine3DArrow(const Phantom::float3& from, const Phantom::float3& to, unsigned int color, float fArrow);
public://object editor
	virtual void				OnRendering();	//渲染
	bool						OnWindowProcObject(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam);
	VOID						OnRenderingObject();
	VOID						OnFrameMoveObject();
	VOID						OnSelectObject(std::vector<SelectEditor>& objects, BOOL bSelTrue = true);
	BOOL						RebuildObjectsCenter();
	VOID						AddSelectObject(std::vector<SelectEditor>& objects, BOOL bSelTrue = true);
	//
	BOOL						BeginEditor(Phantom::matrix4x4& matOut, AxisOperatorType ot);
	VOID						OnEditor(const Phantom::matrix4x4& downMat, const Phantom::matrix4x4& newMat, AxisOperatorType ot, const Phantom::float3& newValOffset);
	VOID						EndEditor(const Phantom::matrix4x4& downMat, AxisOperatorType ot, const Phantom::float3& newValOffset);
	BOOL						PickTerrain(int x, int y, Phantom::float3& ret);
	//
	enum ObjectEditMode
	{
		ObjectEditMode_None	=	0,
		ObjectEditMode_Copy,
		ObjectEditMode_Select,
		ObjectEditMode_GetHeight,
	};
	std::vector<SelectEditor>		m_selects;
	CPoint							m_downPoint;
	BOOL							m_bLButtonDown;
	AxisOperation*					m_axisCurrent;
	CMoveAxis						m_axisMoving;
	CRotationAxis					m_axisRotation;
	CScaleAxis						m_axisScaling;
	Phantom::float3							m_selectCenter;
	ObjectEditMode					m_objectEditMode;
	bool							m_bShowDisableMove;
	bool							m_bShowObjectFile;
	HTREEITEM						m_hGrassList;
	char							m_regionIndex;
public:
	struct	CopySceneTextureData
	{
		Phantom::float2							uvPosition,uvScaling;
		float							uvRotation;
		int								nTextureIDs;
		float							fTextureMipMaps;
	};
	BOOL								m_bCopyTextureData;
	CopySceneTextureData				m_copyTextureData[MAX_LAYER_TEXTURE];
	//
	void						OnTreeViewKeyDown(INT nChar);
	virtual	VOID				OnCopyTree();
	virtual	VOID				OnPasteTree();
	virtual	BOOL				IsCopyTree();
	virtual	BOOL				IsPasteTree();
	virtual	VOID				OnDeleteTree();
	virtual	BOOL				IsDeleteTree();
	enum{
		SCENE3D_NONE	=	0,
		SCENE3D_MAIN,
		SCENE3D_EFFECT,
		SCENE3D_EFFECT_SHADOW_LIST,
		SCENE3D_EFFECT_SHADOW,
		SCENE3D_EFFECT_ADVANCED_LIST,
		SCENE3D_EFFECT_ADVANCED,
		SCENE3D_EFFECT_SMALLMAP,
		SCENE3D_EFFECT_PROFESSIONAL,
		//
		SCENE3D_FOG_LIST,
		SCENE3D_FOG,
		SCENE3D_CAMERA_LIST,
		SCENE3D_CAMERA,
		SCENE3D_SKY_LIST,	//天空体中存在太阳光线的调整
		SCENE3D_SKY,
		SCENE3D_FLARE,
		SCENE3D_LIGHT_LIST,
		SCENE3D_LIGHT,
		SCENE3D_WATER_LIST,
		SCENE3D_WATER,
		SCENE3D_GRASS_LIST,
		SCENE3D_GRASS,
		SCENE3D_TEXTURE_LIST,
		SCENE3D_TEXTURE,
		SCENE3D_OBJECT_LIST,	//对象列表
		SCENE3D_OBJECT,
		//
		SCENE3D_UNIT_LIST,		//单位列表
		SCENE3D_UNIT,			//单位
		//
		SCENE3D_ANIMATION_LIST,	//动画列表
		SCENE3D_ANIMATION,		//动画
		SCENE3D_SCA,
		SCENE3D_SCA_TRACK,
		//
		SCENE3D_POINT_LIST,
		SCENE3D_POINT,
		//
		SCENE3D_ENV_LIST,//环境列表
		SCENE3D_ENV,
		SCENE3D_REGION_LIST,//区域列表
		SCENE3D_REGION,
		SCENE3D_HEIGHT_SET,
		SCENE3D_TEXTURE_SET,
		SCENE3D_SELECT_LAYER,
	};
	void				OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp);
	VOID				OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem);	//更新选择树形控件
	VOID				OnChangeTreeSelect();	//更新选择树形控件
	virtual	void		OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI);
	virtual	void		OnCommandUI(INT nMenuID);
	//
	VOID				RefreshTextures(HTREEITEM hItem);
	VOID				RefreshObjects();
	VOID				RefreshUnits(HTREEITEM hItem);
	VOID				RefreshFogs(HTREEITEM hItem);
	VOID				RefreshRegions(HTREEITEM hItem);
	VOID				RefreshCameras();
	VOID				RefreshSkys(HTREEITEM hItem);
	VOID				RefreshFlares(Phantom::SceneSkyData* sky, HTREEITEM hItem);
	VOID				RefreshLights();
	VOID				RefreshWaters(HTREEITEM hItem);
	VOID				RefreshGrasss(HTREEITEM hItem);
	VOID				RefreshEnvs();
	//
	VOID				RefreshAnimationList(HTREEITEM hItem);
//	VOID				RefreshAnimation(ISCA_Animation* ani, HTREEITEM hItem);
//	VOID				RefreshSCA(ISCA_Animation* ani, ISCA_Base* sca, HTREEITEM hItem);
	VOID				RebuildSubDir(HTREEITEM hItem, const wchar_t* szDir, const wchar_t* szPath);
	//
	VOID				BuildAll();
	//
	VOID				OnSelectObject(SelectEditor & ed);//Phantom::SceneObject* obj);
	VOID				AutoAddRegion();
	//
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
	VOID				PasteAlls();
public:
	VOID				InitSceneProperties();	//初始化场景属性
	VOID				InitSkyProperties();
	VOID				InitFogProperties();
	VOID				InitCameraProperties();
	VOID				InitObjectProperties();
	VOID				InitUnitProperties();
	VOID				InitTextureProperties();
	VOID				InitWaterProperties();
	VOID				InitLightProperties();
	VOID				InitEditorProperties();
	VOID				InitGrassProperties();
	VOID				InitEnvProperties();
	//
	VOID				InitAnimationProperties();
	VOID				InitScaProperties();
	VOID				InitScaTrackProperties();
	VOID				InitSmallMapProperties();
	VOID				InitShadowProperties();
	//
	GCPropertyBase		m_sceneProperties;
	//GCPropertyBase		m_skyProperties;
	//GCPropertyBase		m_flareProperties;
	GCPropertyBase		m_fogProperties;
	GCPropertyBase		m_cameraProperties;
	GCPropertyBase		m_objectProperties;
	GCPropertyBase		m_unitProperties;
	GCPropertyBase		m_textureProperties;
	//GCPropertyBase		m_waterProperties;
	//GCPropertyBase		m_grassProperties;
	GCPropertyBase		m_envProperties;
	GCPropertyBase		m_regionProperties;
	GCPropertyBase		m_lightProperties;
	//
	PropertyPtr			m_objectFileOpen;
	PropertyPtr			m_objectTable;
	PropertyPtr			m_objectTableLine;
	//
public:
	VOID				RefreshUnits();
	VOID				AddUnitProp(Phantom::SceneObject* u);
	VOID				RemoveUnits(INT nID);
	//HTREEITEM			m_hUnits;
	//
	PropertyPtr			m_envID,m_envName,m_envObjects;
	PropertyColorPtr	m_envAmbientColor;
	PropertyPtr			m_envFogMode;
	PropertyPtr			m_envFogEnable;
	PropertyColorPtr	m_envFogColor;
	PropertyPtr			m_envFogNear;
	PropertyPtr			m_envFogFar;
	PropertyPtr			m_envBrightnessEnable;
	PropertyPtr			m_envContrastEnable;
	PropertyPtr			m_envBrightness;
	PropertyPtr			m_envContrast;
	//
	PropertyPtr			m_envShadowEnable;
	PropertyPtr			m_envShadowMapSize;
	PropertyPtr			m_envShadowViewLength;
	PropertyPtr			m_envShadowBias;
	PropertyPtr			m_envShadowPCF;
	PropertyPtr			m_envShadowDensity;
	PropertyPtr			m_envTerrainToMap;
	PropertyPtr			m_envObjectToMap;
	PropertyPtr			m_envCenterObj;
	PropertyPtr			m_envTerrainWithMap;
	PropertyPtr			m_envObjectWithMap;
	VOID				RefreshShadowEnable(BOOL b);
	//
	PropertyPtr			m_regionNameEnable,m_regionName,m_regionList;
	CTypeOption			m_typeSceneID;
	//
	PropertyPtr			m_unitID;
	PropertyPtr			m_unitPositionX;
	PropertyPtr			m_unitPositionY;
	PropertyPtr			m_unitPositionZ;
	PropertyPtr			m_unitScaleFull;
	PropertyPtr			m_unitDiffuseIndex;
	PropertyPtr			m_unitFileOpen;
	PropertyPtr			m_unitEvents;
	PropertyPtr			m_unitType;
	PropertyPtr			m_unitAutoHeight;
	PropertyPtr			m_unitResetMatrix;//重置回默认矩阵
	PropertyPtr			m_unitResetToGame;//重置回默认矩阵
	//
	PropertyPtr			m_unitEnable;	//是否有效
	PropertyPtr			m_unitVisible;	//是否显示
	PropertyPtr			m_unitRange;	//是否显示
	PropertyPtr			m_unitAtkRange;	//是否显示
	//PropertyPtr			m_unitWar//Range;	//是否显示
	//PropertyPtr			m_unitType;		//是否显示
	//设置和编辑单位动作，
	//PropertyPtr			m_unitAction;	//动作列表
	PropertyPtr			m_unitName;		//单位名字
	PropertyPtr			m_unitNameEnable;		//单位名字
	//
	//PropertyPtr			m_unitName;
	PropertyPtr			m_unitRoles;
	DlgSceneObject		m_dlgObjects;
public:
	//
	//GCPropertyBase		m_animationProperties;
	//GCPropertyBase		m_scaProperties;
	//GCPropertyBase		m_scaTrackProperties;
	//
	GCPropertyBase		m_editorProperties;
	PropertyPtr			m_editorRadius;
	PropertyColorPtr	m_editorColor;
	PropertyPtr			m_editorVar;
	PropertyPtr			m_editorNormalVar;
	PropertyPtr			m_editorPower;
	PropertyPtr			m_editorEnable;
	PropertyPtr			m_editorGrassSelect;
	//
	PropertyPtr			m_editorMinValue;
	PropertyPtr			m_editorMaxValue;
public:
	//
	HTREEITEM			m_hShadowItem;
	VOID				RefreshShadows();
	HTREEITEM			m_hAdvancedItem;
	VOID				RefreshAdvanced();
	//
	//GCPropertyBase		m_advancedProperties;
	//PropertyPtr			m_advancedType;
	//PropertyPtr			m_advancedName;
	//PropertyPtr			m_advancedEnabled;
	//PropertyPtr			m_advancedTarget;
	//PropertyFloatPtr	m_advancedKeys[MAX_KEY_PARAMETER];
	//GCPropertyBase		m_smallMapProperties;
public:
	PropertyPtr			m_sceneSpecularExponent;
	PropertyPtr			m_sceneSpecularPower;
	PropertyColorPtr	m_sceneColor;
	PropertyPtr			m_sceneMaxVisible;
	//PropertyPtr			m_sceneHeightAlign;
	//PropertyPtr			m_sceneLODDetailMax;
	//PropertyPtr			m_sceneLODDetailMin;
	//PropertyPtr			m_sceneLODBuildGrids;	//LOD重新创建格子数量
	//PropertyPtr			m_sceneLODEnable;
	//
	//PropertyPtr			m_sceneHeightMap;
	//PropertyPtr			m_sceneTextureMap;
	//PropertyPtr			m_sceneTextureMapBlur;
	//PropertyPtr			m_sceneHeightMapBlur;
	//PropertyPtr			m_sceneHeightMapHeight;
	//PropertyPtr			m_sceneTHBuildVisibleAll;
	PropertyPtr			m_sceneClipEnable;
	PropertyPtr			m_sceneClipSize;//按照场景比例取得
	PropertyPtr			m_sceneLevelTex0;
	PropertyPtr			m_sceneLevelTex1;
	PropertyPtr			m_sceneLevelTex2;
	PropertyPtr			m_sceneLevelTex3;
	PropertyPtr			m_sceneLevelTex4;
	PropertyPtr			m_sceneLevelTex5;
	PropertyPtr			m_sceneLevelTexHeight0;
	PropertyPtr			m_sceneLevelTexHeight1;
	PropertyPtr			m_sceneLevelTexHeight2;
	PropertyPtr			m_sceneLevelTexHeight3;
	PropertyPtr			m_sceneLevelTexHeight4;
	PropertyPtr			m_sceneLevelTexHeight5;
	//fwfewfew
	//PropertyPtr			m_sceneTHBuildAllScene;
	//
	//PropertyFilePtr		m_sceneLogicSystem;
	PropertyPtr			m_sceneShadowDensity;
	//PropertyPtr			m_sceneUnitNameVisible;
	//PropertyPtr			m_sceneUnitBloodVisible;
	//PropertyPtr			m_sceneUnitNameSize;
	//PropertyPtr			m_sceneUnitBloodSize;
	//PropertyPtr			m_sceneUnitNameVisibleLength;
	//PropertyFilePtr		m_sceneUnitBloodTexture;
	//
	//toolbar中 m_sceneTerrainVisible, , 光线调整,天空体设置,都需要使用对话框
	//PropertyPtr			m_geometryLODDetail;
	//PropertyPtr			m_geometryLODMaxVisible;
	//
	//PropertyPtr			m_skyName;
	//PropertyFilePtr		m_skyTextureFile;
	//PropertyPtr			m_skyVisible;
	//PropertyColorPtr	m_skyColor;
	//PropertyPtr			m_skyHeightVar;
	//PropertyFilePtr		m_skyFlareFile;
	//PropertyPtr			m_skyFlareXAngle;
	//PropertyPtr			m_skyFlareYAngle;
	//PropertyColorPtr	m_skyAmbientColor;
	//PropertyPtr			m_skyWindDirection;
	//PropertyPtr			m_skyWindStrength;
	////
	//PropertyPtr			m_skyFlareEnable;
	//PropertyFilePtr		m_flareTexture;
	//PropertyPtr			m_flarePosition;
	//PropertyPtr			m_flareSize;
	//PropertyColorPtr	m_flareColor;
	//PropertyPtr			m_flareAlpha;
	////
	//PropertyFilePtr		m_grassTextureFile;
	//PropertyPtr			m_grassAlphaBlend;
	//PropertyPtr			m_grassAlphaMask;
	//PropertyPtr			m_grassAlphaSizeW;
	//PropertyPtr			m_grassAlphaSizeH;
	//PropertyPtr			m_grassWind;
	//PropertyPtr			m_grassMaxVisible;
	//PropertyColorPtr	m_grassColor;
	//PropertyColorPtr	m_grassFromColor;
	//PropertyPtr			m_grassDensity;
	//PropertyPtr			m_grassTextureCenterY;
	//PropertyPtr			m_grassTextureLeft;
	//PropertyPtr			m_grassTextureTop;
	//PropertyPtr			m_grassTextureWidth;
	//PropertyPtr			m_grassTextureHeight;
	//
	//PropertyPtr			m_fogName;
	//PropertyPtr			m_fogMode;
	//PropertyColorPtr	m_fogColor;
	//PropertyPtr			m_fogNear;
	//PropertyPtr			m_fogFar;
	//
	Phantom::SceneCameraData	m_currentCamera;
	//
	PropertyPtr			m_cameraName;
	PropertyPtr			m_cameraFPS;
	PropertyPtr			m_cameraOrtho;
	PropertyPtr			m_cameraYRotEnable;
	PropertyPtr			m_cameraXRotEnable;
	PropertyPtr			m_cameraEnableMove;
	PropertyPtr			m_cameraEnableDistance;
	//
	PropertyPtr			m_cameraCursorClipInRotMove;
	PropertyPtr			m_cameraCenterCross;
	PropertyPtr			m_cameraShiftMoveMul;
	PropertyPtr			m_cameraMoveHeightIsZ;
	PropertyPtr			m_cameraFixedZAxis;
	//
	PropertyPtr			m_cameraRotationMouseButton;	//转动按钮
	PropertyPtr			m_cameraMoveMouseButton;	//移动按钮
	//
	//PropertyPtr			m_cameraMoveLeft;	//移动按钮
	//PropertyPtr			m_cameraMoveRight;	//移动按钮
	//PropertyPtr			m_cameraMoveForward;	//移动按钮
	//PropertyPtr			m_cameraMoveBack;	//移动按钮
	//PropertyPtr			m_cameraMoveUp;	//移动按钮
	//PropertyPtr			m_cameraMoveDown;	//移动按钮
	//PropertyPtr			m_cameraShiftMoveMulValue;
	//
	PropertyPtr			m_cameraRotateX;
	PropertyPtr			m_cameraRotateZ;
	PropertyPtr			m_cameraTargetX;
	PropertyPtr			m_cameraTargetY;
	PropertyPtr			m_cameraTargetZ;
	PropertyPtr			m_cameraEyeX;
	PropertyPtr			m_cameraEyeY;
	PropertyPtr			m_cameraEyeZ;
	PropertyPtr			m_cameraFov;
	PropertyPtr			m_cameraDistance;
	PropertyPtr			m_cameraMinDistance;
	PropertyPtr			m_cameraMaxDistance;
	PropertyPtr			m_cameraMoveSpeed;
	PropertyPtr			m_cameraRotationSpeed;
	//
	//PropertyPtr			m_waterName;
	//PropertyFilePtr		m_waterTexture;
	//PropertyPtr			m_waterEdgeAlpha;
	////
	//PropertyPtr			m_waterRotation_1;
	//PropertyPtr			m_waterSpeed_1;
	//PropertyPtr			m_waterRotation_2;
	//PropertyPtr			m_waterSpeed_2;
	//PropertyPtr			m_waterWaveX_1;
	//PropertyPtr			m_waterWaveY_1;
	//PropertyPtr			m_waterWaveX_2;
	//PropertyPtr			m_waterWaveY_2;
	//PropertyPtr			m_waterReflectBump;
	//PropertyPtr			m_waterRefractBump;
	//PropertyPtr			m_waterReflectWeight;
	//PropertyPtr			m_waterRefractWeight;
	//PropertyPtr			m_waterFadeBias;
	//PropertyPtr			m_waterFadeExponent;
	////
	//PropertyPtr			m_waterHeight;
	//PropertyPtr			m_waterVisible;
	//PropertyColorPtr	m_waterColor;
	//PropertyPtr			m_waterAlpha;
	//
	//PropertyPtr			m_regionGroup;
	//PropertyPtr			m_regionName;
	//PropertyPtr			m_regionVisible;
	//PropertyColorPtr	m_regionColor;
	////
	//PropertyPtr			m_regionWater;
	//PropertyPtr			m_regionRebuildHT;
	//
	//PropertyPtr			m_textureGroup;
	//PropertyPtr			m_textureHighlight;
	//PropertyPtr			m_textureMipmapBias;
	//					  texture
	//PropertyPtr			m_texturePositionX;
	//PropertyPtr			m_texturePositionY;
	//PropertyPtr			m_textureScalingX;
	//PropertyPtr			m_textureScalingY;
	//PropertyPtr			m_textureRotationZ;
	int					m_bBrushMode;
	//
	//PropertyPtr			m_textureToAll;
	PropertyPtr			m_textureMinFilter;
	PropertyPtr			m_textureMagFilter;
	PropertyPtr			m_textureMipFilter;
	//
public:
	//对象列表中，选择，Unit
	PropertyPtr			m_lightName;
	PropertyPtr			m_lightType;
	PropertyPtr			m_lightEnable;
	PropertyColorPtr	m_lightDiffuse;
	PropertyPtr			m_lightNear;
	PropertyPtr			m_lightFar;
	PropertyPtr			m_lightAngleSmall;
	PropertyPtr			m_lightAngleBig;
	PropertyPtr			m_lightRotX,m_lightRotY;
	VOID				SetLightEnable( int type );
public:
	PropertyPtr			m_animationName;
	PropertyPtr			m_animationTimeLength;
	PropertyPtr			m_animationTimeBegin;
	PropertyPtr			m_animationEnable;
	PropertyPtr			m_scaName;
	PropertyPtr			m_scaEnable;		//
	PropertyPtr			m_trackName;
	PropertyPtr			m_trackEnable;		//
	PropertyPtr			m_trackFrameTimeLength;
	PropertyPtr			m_trackScriptEnable;
	PropertyPtr			m_trackBlendTime;
	//
	PropertyPtr			m_trackUnitSelect;	//单位选择
	//PropertyPtr			m_sccPosGroup;
	//PropertyPtr			m_sccActionGroup;
	//PropertyPtr			m_sccParentGroup;
	////
	//PropertyPtr			m_sccPosEnable;
	//PropertyPtr			m_sccPosLookatTarget;
	//PropertyPtr			m_sccPosMoveSpeed;
	//PropertyPtr			m_sccPosResetFrames;
	//PropertyPtr			m_sccActionID;
	//PropertyPtr			m_sccActionEnable;
	//PropertyPtr			m_sccParentEnable;
	//PropertyPtr			m_sccParentID;
	Phantom::PtrID		m_controlObject;
	//
public:
	VOID				RebuildPoints();
	VOID				RebuildUnits();
	Phantom::AutoPtr<Phantom::Scene3D>		m_scene3D;
	HTREEITEM			m_hObjectList;
	HTREEITEM			m_hEventList;
	HTREEITEM			m_hEnv;
	HTREEITEM			m_hCameraList;
	HTREEITEM			m_hRegionList;
	HTREEITEM			m_hLightList;
	char				m_lightSelect;
	//
	//CTrackAnimationCtrl					m_trackCtrl;
	//CTrackEditorDlg						m_trackDlg;
};



#endif
