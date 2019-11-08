//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   Scene3D.h
软件名称   :   幻影游戏引擎

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___PHANTOMSCENE3D__H___
#define ___PHANTOMSCENE3D__H___

#include "PhantomBase.h"
#include "PhantomUnit3D.h"
#include "PhantomCamera.h"
#include "PhantomUIControl.h"
#include "PhantomSceneDefine3D.h"
#include "PhantomAStarPath.h"
#include "PhantomScene3DUnits.h"


namespace Phantom{

	class	QuadBox;
	class	Scene3D;


	class SceneStaticMesh: public RenderBase
	{
	public:
		virtual	void				Render();
		virtual	void				RenderToShadowMap();
		virtual	void				drawObject(const matrix4x4* matPtr = 0);
		virtual	void				drawObjectShadow(const matrix4x4* matPtr = 0){}
		virtual	char				IsAffectByShadowMap(){return false;}
		virtual	void				drawToShadowMap(const matrix4x4* matPtr, Program* vsps, int nDrawState = RenderNode_SetState){}
		virtual	const matrix4x4&	GetWorldMatrix(){return m_mat;}
		VOID						OnFrameMove();
		VOID						SetWorld(const matrix4x4& mat);
		//
		char						GetHeight(IntersectData* inter);
		inline	BOOL				IsVisible(){return m_bIsVisible;}
		inline	BOOL				IsEnabled(){return m_bIsEnabled;}
		inline	VOID				SetVisible(BOOL b){m_bIsVisible = b;}
		inline	VOID				SetEnabled(BOOL b){m_bIsEnabled = b;}
		SceneStaticMesh(Scene3D* s);
		~SceneStaticMesh();
		VOID						onChange();
	public:
		int							m_id;
		Mesh	*					m_meshPtr;
		PObjectRegion				m_region;
		char						m_bIsClone;
		char						m_bIsFrameMove;
		char						m_bIsVisible;//是否现实
		char						m_bIsEnabled;//是否有效
		char						m_bIsFogVisible;
		matrix4x4					m_mat;
		matrix4x4					m_saveMat;
		LightAffectData				m_light;
		Scene3D*					m_scenePtr;
		unsigned int				m_lightChanged;
		char						m_saveVisible;
	};

	class SceneObjectSkill: public MeshCallback
	{
	public:
		SceneObjectSkill(Scene3D* s);
		~SceneObjectSkill();
		VOID						SetTarget(SceneObject* target, const char* szBoneLike);//目标对象
		SceneObject*				GetTarget();
		VOID						SetTargetPos(const float3& pos);
		const float3&				GetTargetPos(){return m_targetPos;}
		VOID						StartSkill(LPCSTR szFinishFunc);//对某个对象，也可以是多个对象
		VOID						OnFrameMove();
		VOID						OnRender();
		//
		VOID						SetSource(SceneObject* unit, const char* szBoneLike);
		SceneObject*				GetSource();
		VOID						SetSourcePos(const float3& pos);
		const float3&				GetSourcePos(){return m_sourcePos;}
		BOOL						IsFree(){return (m_bFree&&m_fWaitFreeTime<=0);}
		virtual	VOID				ActionPlayOver(Mesh* m, int actiontype, const char* actionname);
		virtual	VOID				ActionAttackBy(Mesh* m, int actionID, int playtype);
		VOID						SetSkillMesh(const char* szFile);
		VOID						SetFree();
		VOID						BindCollision(int classID, const char* szFunc);
	public:
		PtrID						m_sourceUnit;//
		PtrID						m_targetUnit;//第一个目标对象
		float						m_fSpeed;
		char						m_bHasTarget;
		short						m_sourceBoneID;
		short						m_targetBoneID;
		float						m_fHeight;
		float						m_fHeightTarget;
		float						m_fWaitTime;
		float						m_fRotationZ;
		float						m_fWaitFreeTime;
		float						m_fContinueTime;//持续时间设置
		char						m_bContinueTime;
		float						m_fSourceScale;
		//
		NameT<32>					m_finishFunc;
		short						m_collisionClassID;
		NameT<32>					m_collisionFunc;
		float3						m_collisionPos;//技能开始点
		float3						m_sourcePos;//技能开始点
		float3						m_targetPos;
		float						m_readyTime;
		char						m_bFree;
		AutoPtr<Mesh>				m_mesh;
		Scene3D*					m_scenePtr;
	};

#define	QUAD_BOX_COUNT	4

	//四叉树管理器
	class QuadBox
	{
	public:
		QuadBox(void);
		virtual	~QuadBox(void);
		//
		VOID				Setup(Scene3D* scene, QuadBox* pParent, INT nLevel, INT nBeginX, INT nBeginY, INT nRegionWidth, float fRegionSize, INT nLevelIndex);	//构造
		VOID				DestroyData();	//清除数据
		//
		BOOL				InsertObject(SceneObject* obj);
		//
		BOOL				IsBoxInRegion(const AABox& box);	//测试盒子是否在区域内
		BOOL				IsBoxCollision(const AABox& box);//测试盒子是否交叉
		BOOL				IsPointInBoxXY(const float3& pos);//测试盒子是否交叉
		//多选一组模型
		VOID				SelectObjects(CDynamicArray<SceneObject*>& rets, const Rect& rc);
		//
		SceneObject*		Intersect(IntersectData* intersect, BOOL bCollisionOnly);	//只点击带碰撞盒的模型
		BOOL				IntersectStatic(IntersectData* intersect);	//只点击带碰撞盒的模型
		BOOL				IntersectStaticHeight(IntersectData* intersect);	//只点击带碰撞盒的模型
		BOOL				GetHeight(IntersectData* intersect);		//取得当前点的高度
		//
		SceneObject*		GetFirstObject(){return m_pSceneFirst;}
		SceneObject*		GetNextObject(SceneObject* obj){if(!obj)return NULL; return obj->m_pNext;}
		QuadBox*			GetChildRegion(INT nIndex){if(nIndex < 0 || nIndex >= QUAD_BOX_COUNT)return NULL; return m_childRegion[nIndex];}
		//
		VOID				InsertToRenderList(float fRegionSize, QuadBox** renderList, INT& renderCount, INT maxRenderCount, INT minX, INT minY, INT maxX, INT maxY);
		//
		VOID				RecalcRegionBox();
		//
		const AABox&		GetRegionBox(){return m_regionBox;}	//返回区域方块
		//
		virtual	BOOL		Load(CSafeFileHelperR& r, int level);
		virtual	BOOL		Save(CSafeFileHelperW& w, int level);
		virtual	BOOL		Create(int level);
		//
		SceneStaticMesh*	SearchMesh(int id);
	protected:
		VOID				SetDefaultEnv();
		VOID				RenderAllLevels(Camera* c);
		virtual	VOID		RenderToShadowMap();
		VOID				FrameMoveAllLevels( Camera* c, INT nRenderLevel = -1);
		//渲染所有物体
		VOID				RenderBox( Camera* c, INT nSelectType = 0);
		//
		virtual	VOID		Render( Camera* c, const AABox* clipRegion = NULL);
		virtual	VOID		FrameMove( Camera* c, INT nRenderLevel = -1);
		BOOL				AutoDestroyObject(const AABox& box);
		//插入场景物体
		VOID				InsertSceneNode(SceneObject* obj);
		//移出场景物体
		VOID				RemoveSceneNode(SceneObject* obj);
		//
		VOID				AddObjectCount(INT nCountAdd);
		//
		BOOL				OnChangeSceneObject(SceneObject* obj);
		//
		VOID				ForseVisible(BOOL bVisible);//强制显示或隐藏
		VOID				ResetClipVisible(BOOL bHideOnly = FALSE);//重新计算区块物体,是否只计算不可见物体
		//
		friend class		Scene3D;
		friend class		SceneObject;
		friend class		PhantomTerrainBox;
	protected:
		CDynamicArrayS<SceneStaticMesh*,0>	m_staticObjects;
		QuadBox*			m_childRegion[QUAD_BOX_COUNT];	//子盒子数量
		QuadBox*			m_pParent;
		SceneObject*		m_pSceneFirst;			//场景物体基类
		//
		short				m_nChildObjectCount;
		AABox				m_regionBox;
		short				m_nMaxLevel;
		char				m_nLevelIndex;
		Scene3D*			m_scenePtr;
	};

	class SceneBox: public QuadBox
	{
	public:
		SceneBox();
		~SceneBox();
		BOOL					InitRegion(INT nRegionX, INT nRegionY);
		VOID					Render( Camera* c, const AABox* clipRegion);
		virtual	VOID			RenderToShadowMap();
		//
		virtual	BOOL			Load(CSafeFileHelperR& r, int level);
		virtual	BOOL			Save(CSafeFileHelperW& w, int level);
		GVertexData*			GetVertexPtr(int x, int y);
		GVertexData*			GetGridPtr(int x, int y);
		BOOL					IntersectAll(INT nGridX, INT nGridY, const float3& orig, const float3& dir, PickPoint& out);
		//
		GVertexData*			GetVertexAttributePtr(INT nGridX, INT nGridY);
		float					GetVertexHeight(INT nVertX, INT nVertY);
		VOID					SetVertexAttributeValue(INT nGridX, INT nGridY, const GVertexData* attr);	//设置单元格属性
		VOID					SetModifyed(BOOL b);
		//
		VOID					CreateTerrain();//创建地面数据
		VOID					DeleteTerrain();
		VOID					DeleteMoveGrids();
#ifdef EDITOR_OS
		VOID					CreateMoveGrids();//创建寻路数据
		char					m_bModifyed;//是否被修改
		GridAttr25d*			m_attrs;//每个格子的属性数据
		VOID					RebuildMeshs(BOOL bOptimize);//是否优化，保存的时候使用优化
		VOID					RebuildMoveGrids();
		VOID					RebuildRegionSets(int selindex);
		AutoPtr<DeviceMesh>		m_moveEnable;//可以动格子数据
		VOID					SetBlend(GridAttributeT* attrs, int x, int y, int texid, int offx, int offy, int blend);
		VOID					BuildNormals(CDynamicArray<SceneVertices25d>& vertices, CDynamicArray<Triangle>& faces, INT nWidth, INT nHeight);
#endif
		inline	INT				GetRegionIndexX(){return m_regionX;}
		inline	INT				GetRegionIndexY(){return m_regionY;}
	public:
		char					m_bVisible;
		short					m_regionX,m_regionY;
		unsigned int			m_lightChanged;
		LightAffectData		m_light;//灯光列表
		VOID					RebuildLight();
	public://可保存数据
		GVertexData		*		m_grids;
		GMoveData*				m_moveGrids;//移动寻路数据
		AutoPtr<DeviceMesh>		m_mesh[MAX_LAYER_TEXTURE];
		AutoPtr<DeviceMesh>		m_meshAlpha[MAX_LAYER_TEXTURE];
	};
	struct	SceneMeshFile
	{
		Mesh*			mesh;//模型对象
		char			bFail;
		short			nCount;//创建数量
	};

	class Scene3DAstar: public AstarCallback
	{
	public:
		virtual	int				IsAstarMoveEnable(int x, int y);
		virtual	int				GetAstarWidth();
		virtual	int				GetAstarHeight();
		Scene3D*				m_mapPtr;
	};
	class	ScenePoint: public Base{
	public:
		ScenePoint(Scene3D* scenePtr);
		~ScenePoint();//场景事件对象
		virtual	BOOL				Save(CSafeFileHelperW& w);
		virtual	BOOL				Load(CSafeFileHelperR& r);
		//
		inline	const float3&	GetPosition(){return m_pos;}
		inline	float			GetRange(){return m_range;}
		VOID					SetPosition(const float3& v);
		VOID					SetRange(float f);
		inline	INT				GetID(){return m_id;}
		inline	BOOL			GetEnabled(){return m_bEnabled;}
		inline	VOID			SetEnabled(BOOL b){m_bEnabled = b;}
		ScenePoint*				Clone();
	public:
		float3					m_pos;
		float					m_range;
		int						m_eventid;//表格中的事件ID,event_table.txt
		NameT<32>				m_name;
	protected:
		friend class			Scene3D;
		friend class			SceneEnv;
		int						m_id;
		char					m_bEnabled;//是否有效
		Scene3D*				m_scenePtr;
	};

	extern BOOL			GetSceneEventName(char* retFunc, char* retFull, int id, const char* name);//根据场景名字进行修订
	extern int			GetSceneEventCount();


	class SceneLight:public Base
	{
	public:
		enum LightType{
			LightType_Omni	=	0,//点光
			LightType_Spot	=	1,//聚光
			LightType_Direction = 2,//方向光
		};
		SceneLight(Scene3D* s = 0);
		~SceneLight();
		VOID						SetSrcObject(SceneObject* obj);//灯光点对象ptr
		VOID						SetTargetObject(SceneObject* obj);//灯光目标点对象ptr
		VOID						SetLightType(LightType type);//
		VOID						Save(CSafeFileHelperW& w);
		VOID						Load(CSafeFileHelperR& r);
		inline	INT					GetID(){return m_id;}
		SceneLight*					Clone();
	public://light data
		NameT<32>					m_name;
		float3						m_pos;//灯光位置
		float3						m_dir;//灯光目标
		char						m_bEnabled;//是否灯光模型
		char						m_lightType;//灯光类型
		//
		float3						m_lightColor;
		inline	float				GetNear(){return m_lightNear;}
		inline	float				GetFar(){return m_lightFar;}
		inline	float				GetAngleSmall(){return m_lightAngleSmall;}
		inline	float				GetAngleBig(){return m_lightAngleBig;}
		void						SetNear(float f){m_lightNear = f; m_lightFar = getmax(m_lightFar, m_lightNear+0.1);}
		void						SetFar(float f){m_lightFar = f; m_lightNear = getmin(m_lightNear, m_lightFar-0.1);}
		void						SetAngleSmall(float fAngle){m_lightAngleSmall = fAngle; m_lightAngleBig = getmax(m_lightAngleBig, m_lightAngleSmall+0.1);}
		void						SetAngleBig(float fSmall){m_lightAngleBig = fSmall; m_lightAngleSmall = getmin(m_lightAngleSmall, m_lightAngleBig-0.1);}
		float						m_angleZ,m_angleY;
		void						recalcAngle();
	protected:
		float						m_lightNear;
		float						m_lightFar;
		float						m_lightAngleSmall;//灯光大小
		float						m_lightAngleBig;//灯光大小
		unsigned short				m_id;
		Scene3D*					m_scenePtr;
		friend class				Scene3D;
		friend class				SceneEnv;
	};
	enum ShadowEnable{
		ShadowEnable_MeshToMap		=	0x0001,//模型是否渲染到阴影图
		ShadowEnable_TerrainToMap	=	0x0002,//地形是否渲染到阴影图
		ShadowEnable_MeshWithMap	=	0x0004,//模型是否接受阴影
		ShadowEnable_TerrainWithMap	=	0x0008,//地形是否接受阴影
		ShadowEnable_UseShadowMap	=	0x0010,//是否使用阴影
	};
	//enum ShadowPCF
	//{
	//	PCF_0	=	0,
	//	PCF_3x3,
	//	PCF_5x5,
	//	PCF_7x7,
	//};
	//场景环境,保存环境对应的所有的角色位置和转动等属性,包括矩阵,以后还可以增加其他环境设定,比如光线,亮度等
	class SceneEnv: public Base
	{
	public:
		SceneEnv(Scene3D* s = 0);
		virtual ~SceneEnv();
		VOID						AddObject(SceneObject* s);//保存所有
		BOOL						RemoveObject(int id);
		const SavePosRot*			GetObjectByID(int id);
		//
		VOID						Save(CSafeFileHelperW& w);
		VOID						Load(CSafeFileHelperR& r);
		//
		inline	int					GetID(){return m_id;}
		inline	const char*			GetName(){return m_name.t;}
		inline	int					GetNumObject(){return m_objects.size();}
		const SavePosRot*			GetObjectPtr(unsigned int index){if(index>=m_objects.size())return 0; return &m_objects[index];}
		//
		ScenePoint*					InsertPoint(const float3& pos);
		VOID						RemovePoint(ScenePoint* obj);
		ScenePoint*					GetPointByName(const char* szName);
		ScenePoint*					GetPointByID(INT id);
		ScenePoint*					GetPoint(PtrID& id);
		ScenePoint*					GetPointByEventID(INT eventID);
		inline	Scene3D*			GetScenePtr(){return m_scenePtr;}
		BOOL						IsActive();
		//
		SceneLight*					CreateLight();//创建一个灯光设定
		VOID						RemoveLight(int id);//根据id删除灯光
		SceneLight*					GetLightPtr(GameID& id);//返回灯光类指针
		SceneLight					m_defaultLight;
		CDynamicArrayS<SceneLight*,0>	m_lights;//灯光列表
		SceneID						m_shadowCenterObj;//阴影中心点对象
		float3						m_shadowCenter;
		char						m_shadowMode;
		//float						m_shadowFront;
		//
		inline	BOOL				IsShadowEnable(int s){return (m_shadowEnabled&s)!=0;}
		inline	VOID				SetShadowEnable(int s, BOOL bEnable){if(bEnable)m_shadowEnabled|=s;else if(m_shadowEnabled&s)m_shadowEnabled^=s;}
		inline	BOOL				IsShadowEnable(){return IsShadowEnable(ShadowEnable_UseShadowMap);}
		int							m_shadowMapSize;//阴影贴图大小
		float						m_shadowViewSize;//阴影贴图长度修正
		int							m_shadowEnabled;//ShadowEnable enum
		float						m_shadowDensity;//设置阴影浓度
		float						m_shadowBias;//阴影精确程度
		char						m_shadowPCF;//软化程度
	public:
		enum ShaodwMode{
			ShadowMode_Object = 1,
			ShadowMode_Center,
			ShadowMode_Front,
		};
		NameT<32>					m_name;//名字
		CDynamicArray<ScenePoint*>	m_points;
		//亮度对比度
		char						m_bBrightness;
		char						m_bContrast;
		float						m_fBrightness;//亮度
		float						m_fContrast;//
		char						m_fogEnable;//是否开启雾设置
		unsigned char				m_fogType;
		unsigned int				m_fogColor;
		float						m_fNear;
		float						m_fFar;
		unsigned int				m_ambientColor;
		//
		VOID						CommitBrightness();
		VOID						CommitContrast();
		VOID						CommitFog();
		VOID						CommitShadowMap();
	protected:
		VOID						Active();//载入所有设定到所有场景对象中
		int							m_id;//环境ID
		CDynamicArray<SavePosRot>	m_objects;//环境对象数据类
		Scene3D*					m_scenePtr;
		friend class	Scene3D;
	};
	class Scene3D: public Base
	{
	public:
		Scene3D();
		ImplementComValue();
		SceneObject*			GetObjectByName(const char* szName);
		SceneObject*			GetObjectByContentID(INT id);
		SceneObject*			GetObjectByID(INT id);
		SceneObject*			GetObject(PtrID& id);
		SceneObject*			SearchObject(SceneID& id);
		VOID					GetObjectByName(const char* szName, CDynamicArray<SceneObject*>& rets);//返回相关所有对象数组
		SceneObject*			GetObjectPtr(int index);
		INT						GetObjectCount();
		SceneObject*			CloneObject(SceneObject* obj);
		SceneObject*			CloneAndBind(SceneObject* obj);
		//多选一组模型
		SceneObject*			InsertObject(const char* szFile, const float3& pos, const matrix4x4* mat = 0);
		VOID					AddObjectType(SceneObject* obj);
		VOID					RebuildTypes();
		VOID					RemoveObject(SceneObject* obj);
		//根据模型和出生点创建一个角色,通常用于创建主角
		SceneObject*			InsertObject(const char* szMeshFile, int nEventLineID);//事件行ID
		//
		VOID					SelectObjects(CDynamicArray<SceneObject*>& rets, const Rect& rc);
		VOID					SelectEvents(CDynamicArray<ScenePoint*>& rets, const Rect& rc);
		SceneObject*			Intersect(IntersectData* intersect, BOOL bCollisionOnly);	//只点击带碰撞盒的模型
		BOOL					IntersectStatic(IntersectData* intersect);	//只和静态模型进行碰撞
		BOOL					IntersectStaticHeight(IntersectData* intersect);	//只和静态模型进行碰撞
		BOOL					IntersectTerrain(const float3& orig, const float3& dir, PickPoint& out);	//点击地形
		BOOL					IntersectTerrain(PickPoint& out);	//点击地形
		BOOL					GetHeight(const float3& pos, float& fHeight, float3* pNormalRet = 0);		//取得当前点的高度
		//
		VOID					SaveScene(const char* szFile);//保存场景
		VOID					CreateTerrain();//创建地表数据
		VOID					DeleteTerrain();//释放地表数据，释放之后不能被恢复，会清空所有的地表数据
		VOID					CreateMoveGrids();//创建地表可移动格子数据,用于findway寻路操作,格子数量是每个普通格子占用16(4x4)个可移动格子数据
		VOID					DeleteMoveGrids();//删除地表可移动格子数据,用于findway寻路操作
		//
		inline	FileName&		GetFileName(){return m_fileName;}
		SceneBox*				GetSceneRegion(INT xRegion, INT yRegion);	//返回地形区域
		inline	int				GetMaxRegionWidth(){return m_nMaxRegionWidth;}
		inline	INT				GetMaxGridCount(){return m_nMaxRegionWidth * REGION_GRID_COUNT_25d;}
		INT						GetSceneRegionCount();
		SceneBox*				GetSceneRegionPtr(int index);
		VOID					BuildRegions(QuadBox* pRegion, QuadBox* pRegionParent, INT& nIndex, INT nLevel, INT nBeginX, INT nBeginY, INT nLevelIndex);
		//
		void					StartSkill(GameSceneID& srcid, GameSceneID& targetid, float3 tpos, char toTarget, unsigned short meshfileindex, float fContinueTime);
		//
		virtual	VOID			OnFrameMove();
		virtual	VOID			RenderScene();
		virtual	VOID			RenderToShadowMap();
		int						GetNewIdentity(){return (m_saveObjectID++);}
		GVertexData*			GetVertexAttribute(INT nGridX, INT nGridY);
		//
		VOID					SetVertexAttribute(INT xRegion, INT yRegion, INT nGridX, INT nGridY, const GVertexData* v);	//设置单元格属性//返回单元格属性
		VOID					SetVertexAttribute(INT nGridX, INT nGridY, const GVertexData* v);	//设置单元格属性
		VOID					SetNormalBuildVar(float f){m_fNormalVar = f;}//设置法线长度
		inline	float			GetNormalBuildVar(){return m_fNormalVar;}
		VOID					RefreshEditVertices(INT xRegion, INT yRegion);
		VOID					RebuildRegionFaces(INT xRegion, INT yRegion, BOOL bOptimize = false);		//刷新颜色
		BOOL					RecalcRegionGrassHeight(INT xRegion, INT yRegion);
		VOID					RebuildMoveGrids(INT xRegion, INT yRegion);		//刷新颜色
		VOID					RebuildAllMoveGrids();
		VOID					RebuildRegionSets(int selindex);
		VOID					RebuildRegionSets(INT xRegion, INT yRegion, int selindex);
		VOID					SetGridRegionIndex(int mx, int my, int index);
		BOOL					GetVertexPosition(INT nGridX, INT nGridY, float3& out);
		//
		BOOL					GetGridMiddle(INT nGridX, INT nGridY, float3& out);	//返回单元格中心点
		SceneStaticMesh*		SearchStaticObject(int nID);
		//
		INT						GetClassCount(int nID);
		SceneObject*			GetClassPtr(int nID, int nIndex);
		VOID					ResortClass();
		//
		VOID					SetActiveScene(const char* szActiveCamera);
		VOID					SetActiveCamera(const char* szActiveCamera);
		//
		VOID					SetActiveLight(const char* szLight);
		DataManager<SceneSkyData,Scene3D>*		GetSkyDataPtr(){ return &m_skyDataPtr;}
		DataManager<SceneCameraData,Scene3D>*	GetCameraDataPtr(){return &m_cameraDataPtr;}
		//
		VOID					OnInputEvent(Phantom::InputEventID btn, int x, int y, unsigned int touchIndex);
		//
		VOID					SetCameraPos(const Pixel& pos);
		const Pixel&			GetCameraPos(){return m_cameraPos;}
		inline	float			GetRegionSize(){return m_fGridSize * (float)REGION_GRID_COUNT_25d;}
		inline	float			GetWorldSize(){return m_fGridSize * (float)REGION_GRID_COUNT_25d * (float)m_nMaxRegionWidth;}
		//
		SceneObjectFile*		GetObjectFile(int id);
		Mesh*					GetStaticMeshPtr(int index, char& bIsClone, char& bIsFramMove);
		inline	float			GetGridSize(){return this->m_fGridSize;}
		float					GetHeight(const float3& pos);
		inline	int				GetNumofGridW(){return (REGION_GRID_COUNT_25d*this->m_nMaxRegionWidth);}
		//
		inline	int				GetMoveGridsW(){return GetNumofGridW();}
		inline	INT				GetRegionWidthMoveGrids(){return REGION_GRID_COUNT_25d;}
		inline	float			GetMoveGridSize(){return (this->m_fGridSize);}
		BOOL					IsMoveGridEnable(int mx, int my);
		BOOL					IsMoveGridEnable(const float3& pos);
		GMoveData*				GetMoveAttribute(INT mx, INT my);
		VOID					SetMoveGridEnable(int mx, int my, BOOL bEnable);
		int						GetRegionIndex(const float3& pos);
		//
		BOOL					FindPath(CDynamicArrayS<WayPoint2D>& ways, const float3& from, const float3& to);
		VOID					bind_event(int eventID, const char* szLuaFunc);
		//
		//VOID					SetProtagonist(SceneObject* obj);
		VOID					SetCameraFollow(SceneObject* obj);
		//
		SceneGameSkill*			GetFreeGameSkill(unsigned short fileid);
		SceneObjectSkill*		GetFreeSkill();
		SceneTextureRuntime*	GetLayerTextureData(int nLayer);
		Texture*				GetLayerTexture(int nLayer);//
		VOID					SetLayerTexture(int nLayer, const char* szFileName);			//设置一个贴图
		inline	INT				GetRegionWidthGrids(){return REGION_GRID_COUNT_25d;}
#ifdef EDITOR_OS
		color4					GetGridAttrColor(int x, int y);
		VOID					SetGridAttrColor(int x, int y, const color4& rgb);
		INT						GetGridAttrIndex(int x, int y);
		VOID					SetGridAttrIndex(int x, int y, int nTexture);
		VOID					SetGridAttrVisible(int x, int y, int visible);
#endif
		//events
		BOOL					IsEventExist(unsigned char index);
		VOID					SetEventExist(unsigned char index, BOOL b);
		enum SceneEnable{
			Enable_ShowBox			=	0x0001,//是否显示场景对象的盒子
			Enable_ShowCollisions	=	0x0002,//是否显示场景对象的碰撞盒子
			Enable_ShowObject		=	0x0004,//是否显示场景对象
			Enable_ShowTerrain		=	0x0008,//是否显示地形
			Enable_ShowWater		=	0x0010,//是否显示水面
			Enable_TerrainData		=	0x0020,//是否创建地形数据
			Enable_MoveGridsData	=	0x0040,//是否创建寻路数据
			Enable_ShowScene		=	0x0080,//场景是否可显示
			Enable_EnableScene		=	0x0100,//场景是否可点击
			Enable_ShowMoveGrids	=	0x0200,//显示移动格子
			Enable_ShowRegionSet	=	0x0400,//显示区域格子
		};
		inline	BOOL			HasTerrainData(){return IsEnabled(Enable_TerrainData);}
		inline	BOOL			IsEnabled(SceneEnable n){return (m_nEnables&n)==n;}
		VOID					SetEnabled(SceneEnable n, BOOL b);
		virtual	VOID			BindAllObjects();//绑定所有的角色
		VOID					Pause(BOOL b);
		//
#ifdef EDITOR_OS
		VOID					OnChangeWorld(SceneObject* obj);//保存位置的时候
		char					m_bChangeEnable;
#endif
		SceneEnv				m_defaultEnv;//游戏环境
		CDynamicArrayS<SceneEnv*,0>	m_sceneEnvs;//环境设定列表
		inline	SceneEnv*		GetActiveEnv(){if(!m_activeEnv)return &m_defaultEnv; return m_activeEnv;}
		SceneEnv*				CreateEnv();//创建一个环境设定
		VOID					RemoveEnv(int id);//根据id删除env
		VOID					ActiveEnv(SceneEnv* env);
		SceneEnv*				GetEnvByID(int id);
		//
		VOID					RebuildLight(LightAffectData& light, const float3& pos, float fsize/*对象大小*/);//重新计算光源
		//
		struct ObjectClass{
			short	begin;
			short	count;
		};
		Pixel					m_cameraPos;
		float3					m_sunDir;
		ObjectClass				m_classIDs[MAX_CLASS_IDs];
		char					m_classChanged;
		SceneObject*			m_selectObj;
		SceneObject*			m_cameraFollow;
		char					m_bPause;
		//
		GameID					m_sceneLineID;//场景行数据ID
		CDynamicArrayS<SceneGameSkill*>		m_gameSkills;
		CDynamicArrayS<SceneObjectSkill*>	m_skills;//技能方案列表,每个技能方案都会被自动载入数据，只要被使用过就会被载入，没有被使用过，第一次使用会被载入
		UIDrawMgr				m_bloodDraw,m_nameDraw;
		//
		//inline	INT				GetLayerTextureID(int index){if(index<0||index>=MAX_LAYER_TEXTURE)return -1;return m_layerTextureID[index];}
		//VOID					SetLayerTextureID(int index, int id);//设置层级图片
		char					m_bShowBlood;
		float					m_fBloodSize;
		char					m_bHitDisplay;//是否显示伤害对话框
		//
		UIControl*				AddHit(SceneObject* obj, int val);
		VOID					RemoveHit(UIControl* c);
		BOOL					LoadBloodDlg(const char* szDlg);//载入场景设置对话框,其中blood控件是血条控件,hit控件是伤害控件
		UIProgress*				m_bloodProgress;//血条控件
		UIControl*				m_hitControl;//被击特效控件第一个
		struct HitObject{
			SceneID			id;
			UIControl*		hit;
			HitObject(){hit=0;}
		};
		CDynamicArrayS<HitObject>	m_hits;//被击特效播放控件列表
		AutoPtr<UIDialog>		m_dlgScene;//场景对象对话框
		//界面顺序错误,设置不了图片,使用界面渲染血条和打击蹦字效果,角色之间碰撞
		const char*				GetRegionName(unsigned int index);//获取区域名字
		SceneRegionData*		GetRegionPtr(unsigned int index);
		Program*				SetRenderMode();
		Program*				SetRenderToShadowMode();
	protected://GameScene
		virtual	VOID			OnMessage(GameMessage* msg);
	protected:
		//只能通过Manager调用
		BOOL					CreateScene(float fGridSize, int nSceneSize);
		BOOL					LoadScene(const char* szScene);
		SceneObject*			CreateObject(MeshData::MeshType type);
		VOID					DestroyScene();
		Mesh*					GetMeshPtr(int id);
		BOOL					IsObjectFileExist(const char* szFileName, char* szOut);
		virtual	~Scene3D();
		friend class			Manager;
		friend class			QuadBox;
		friend class			GameMgr;
		//
		QuadBox*				m_pRegionPtr;
		QuadBox*				m_regions;
		AStarPath				m_astar;
		Pixel					m_downPos;
		SceneEnv*				m_activeEnv;//当前激活的环境
		//
		SceneBox*				m_sceneRegions;
		Scene3DAstar			m_astarcb;
		FileName				m_fileName;
		NameT<32>				m_name;
		CDynamicArrayS<BindEvent>		m_binds;//由模型属性来设置并生成到m_binds
		CDynamicArrayS<SceneMeshFile>	m_staticMeshs;
		CDynamicArrayS<SceneObject*>	m_sceneObjects;
		SceneRegionData			m_regionList[MAX_REGION_COUNT];
		//
		int						m_programID;
		int						m_programTSM_ID;//to shadow map
		unsigned int			m_nEventExist;
	public:
#ifdef EDITOR_OS
		int						m_renderModeMoveEnable;
#endif
	public://需要保存的数据
		char					m_bEnabled;
		char					m_bEnableTerrainEvent;
		float					m_fTerrainDownRadius;//
		int						m_saveObjectID;
		unsigned short			m_saveLightID;
		float					m_fNormalVar;
		int						m_nMaxRegionWidth;
		float					m_fGridSize;
		unsigned int			m_nEnables;
		SceneTextureRuntime		m_layerTextures[MAX_LAYER_TEXTURE];
		char					m_bIs2DFindway;
		color4					m_sceneDiffuse;//漫射光色
		float					m_specularExponent;//高光指数
		float					m_specularPower;//高光权重
		float					m_fMaxViewDistance;//最大可见距离
		PtrID					m_activeCamera;
		PtrID					m_activeSky;
		PtrID					m_activeFog;
		char					m_bClipCamera;
		float					m_fClipCameraSize;
		unsigned int			m_lightChanged;
		int											m_skyActiveIndex;
		DataManager<SceneSkyData,Scene3D>			m_skyDataPtr;
		DataManager<SceneCameraData,Scene3D>		m_cameraDataPtr;
		DataManager<SceneObjectFile,Scene3D>		m_objectFilePtr;
	};
};

#endif
