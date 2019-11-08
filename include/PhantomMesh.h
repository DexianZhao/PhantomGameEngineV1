//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_MODELGROUP_H__
#define __PHANTOM_MODELGROUP_H__
#include "PhantomDevice.h"
#include "PhantomSubMesh.h"
#include "PhantomUIControl.h"

namespace Phantom{
	class	PakDirectory;

struct	BoneName
{
	char							szName[128];
	int								meshid;
};

class Mesh;
class MeshData;

#define MAX_RIBBON_EDGE					32
#define PHANTOM_EMITTER_MAX_PARTICLE	64
enum 
{
	PARTICLE_HEAD	= 0,
	PARTICLE_TAIL	= 1,
	PARTICLE_BOTH	= 2,
};

enum ParticleEmitterFlag
{
	ParticleFlag_Squirt				=	1<<13,//是否一次性全部发射出去所有粒子
	ParticleFlag_LocalAxis			=	1<<18,
	ParticleFlag_LockEmitter		=	1<<20,
	ParticleFlag_AlphaTest			=	1<<2,//是否使用alphatest模式
	//
	EmitterType_Head				=	1<<15,
	EmitterType_Tail				=	1<<16,
};

//刀光效果
#pragma pack(push, 1)

struct	PPointColorUV
{
	float3			p;
	unsigned int	color;
	float			u,v;
	static	const unsigned int			c_fvf	=	Phantom::FVF_XYZ|Phantom::FVF_TEX1|Phantom::FVF_DIFFUSE;
};

struct	RibbonEmitterDataOld
{
	char	texture[128];
	int		boneIndex;
	float3	line[2];//刀光效果
	int		countPerSec;//每秒多少次
	int		countLifeSec;//总共多少秒
	int		maxLifeSec;
	int		srcBlend;
	int		destBlend;
	float	fAlpha;
};
struct	RibbonEmitterData:RibbonEmitterDataOld
{
	int		boneIndex2;
	char	bIsExport;//是否从max导出的，如果是导出的则在max重新导出的时候自动删除
	RibbonEmitterData();
};
struct RibbonEmitterData2: RibbonEmitterData
{
	Texture*	texturePtr;
	~RibbonEmitterData2(){safe_release(texturePtr);}
	RibbonEmitterData2();
};

struct RibbonEdge
{
	unsigned int	dwBornTime;
	BOOL			bStart;
	unsigned int	dwStartTime;
	unsigned int	dwLife;
	float			fU;
	float3			vPoint[2];
	RibbonEdge()
	{
		dwBornTime = 0;
		bStart = false;
		dwStartTime = 0x7fffffff;
		dwLife = 0;
		fU = 0;
	}
};

struct ParticleData
{
	DWORD			dwBornTime;		// 出生时间
	DWORD			dwLife;			// 生命
	float3			vBornPos;		// 出生时的位置
	float3			vPos;			// 位置, 相对于模型坐标系的，
	float3			vPosWorld;		// 位置，模型坐标系经过变换以后的结果
	float			fSpeed;			// 速率
	float3			vDir;			// 运动方向
	float			fScale;			// 缩放
	DWORD			dwColor;
	int				nCellId;		// 贴图cell编号
	int				nRepeatTimes;	// 贴图动画已经循环播放的次数
	DWORD			dwLastUpdateTime;	// 上次更新时间
	ParticleData*	next;
};

struct ParticleEmitterData
{
	DWORD		dwFlag;//选项,替换
	int			nBindBoneID;//骨骼绑定点(select)
	float3		vCenter;//中心点(val)
	int			nParticleCount;//暂无作用
	float		speed;//粒子发射速度(fani)
	float		speedvar;//速度变化范围(speed*speedvar/100)百分比(fani)
	float		fConeAngle;//发射角度(fani)
	float		fGravity;//(没用上)重新使用起来,让他默认值是0
	float		fLifeSpan;//粒子生命周期(val)
	float		fTimeLength;//总发射周期
	float		fPlaneWidth;//平面宽度
	float		fPlaneHeight;//平面高度
	DWORD		dwBlendMode;//
	int			nUVRow;//UV动画宽度数量，默认是1
	int			nUVCol;//UV动画高度数量，默认是1
	DWORD		nEmitterType;//发射器类型
	float		fTailLength;//tail发射模式下有效，拖尾长度
	float		fDecayTime;//开始衰减时间
	int			srcBlend;
	int			destBlend;
	DWORD		dwUVAnimFps;//改成float(/1000)
	int			nSegmentColor[3][3];//颜色变化，0=开始，1=鼎盛，2=衰减后
	int			nAlpha[3];//透明变化
	float		fParticleScaling[3];//缩放变化
	DWORD		dwLifeSpanUVAnim[3];//UV动画生命值衰减过程
	DWORD		dwDecayUVAnim[3];//UV衰减动画
	char		szTexture[128];//图片
	float3		vPlaneNormal;//发射平面法线方向
	float3		vPlaneXAxis;//平面X方向
	float3		vPlaneYAxis;//平面Y方向
	//
	inline	BOOL	IsAlphaTest(){return dwBlendMode == 0x0002;}
};

struct ParticleEmitterData2: ParticleEmitterData
{
	Texture*	texturePtr;
	ParticleEmitterData2()
	{
		memset(this, 0, sizeof(ParticleEmitterData2));
		srcBlend = Phantom::BlendMode_SRCALPHA;
		destBlend = Phantom::BlendMode_INVSRCALPHA;
	}
	~ParticleEmitterData2(){safe_release(texturePtr);}
};
#pragma pack(pop)


class  CParticleEmitter
{
public:
	CParticleEmitter();
	~CParticleEmitter();
	void					Init(ParticleEmitterData2* d, MeshData* gd);
	void					Update( int nFrameID, Mesh* group );
	void					Render( float fAlpha );
	void					SetActive( BOOL bActive ){ m_bIsActive = bActive; }
public:
	inline	BOOL			IsParticleDead( ParticleData* pParticle, int nTime ){return (pParticle->dwBornTime+pParticle->dwLife) < nTime;}
	void					GenParticle( ParticleData* pParticle, int nTime );
	void					UpdateParticle( ParticleData* pParticle, int nTime, const matrix4x4* pmatModel = NULL );
	void					GenDirection( float3* axis, float fAngle, float3* dir );
	void					MakeARenderParticle( ParticleData* pParticle, float3* pos, unsigned int * colors, float2* uvs, float fAlpha );
	void					MakeARenderParticle( ParticleData* pParticle, PosColorUV* vertices, float fAlpha );
	void					Reset(BOOL bAll);
protected:
	ParticleEmitterData2*	m_dataPtr;
	float3					m_vPos;
	float					m_fScale;
	float3					m_vPlaneNormal;
	float3					m_vPlaneXAxis;
	float3					m_vPlaneYAxis;
	float3					m_vCameraSpaceUp;
	float3					m_vCameraSpaceRight;
	DWORD					m_dwLastUpdateTime;
	DWORD					m_dwLastGenPartsTime;
	int						m_nNumPart;
	ParticleData*			m_particles;
	BOOL					m_bIsActive;
	BOOL					m_bSquirtted;
	friend class			Mesh;
};

class bz_geometryGroup;
class	RibbonEmitter
{
public:
	RibbonEmitter()	
	{
		m_nEdgeCount = 0;
		m_nMaxEdgeCount = 0;
		m_dwLastGenEdgeTime = 0;
		m_dwLastUpdateTime = 0;
		m_nLastFrameID = -1;
		m_bStart = false;
	}
	void			Update( int nFrameID, Mesh* group );
	void			Render( float fAlpha);
	void			Init(RibbonEmitterData2* data, MeshData* gd);
	void			SetEnable(BOOL bEnable);
protected:
	RibbonEmitterData2*	m_dataPtr;//在数据中的索引
	int				m_nEdgeCount;
	int				m_nMaxEdgeCount;
	RibbonEdge		m_edges[MAX_RIBBON_EDGE];
	int				m_nLastFrameID;
	BOOL			m_bStart;
	DWORD			m_dwLastGenEdgeTime;
	DWORD			m_dwLastUpdateTime;
	void			GenEdge( RibbonEdge* pEdge, matrix4x4* pParentTransform );
	BOOL			AddNewEdge( RibbonEdge* pEdge );
	void			UpdateEdge( RibbonEdge* pEdge, matrix4x4* pmatModel = NULL );
	void			MakeARenderEdge( RibbonEdge* pEdge, PPointColorUV* vertex );
	friend class	Mesh;
};

class MeshParts
{
public:
	//
	char						LoadGroupData(CSafeFileHelperR& r, char bLodEnable, char bShadowEnable, int version, BOOL bUseVertexCache);
	char						SaveGroupData(CSafeFileHelperW& w);
	//
	SubMeshData*				findGeometry(int geometryID);
	SubMeshData*				findGeometry(const char* szName);
	//
	void						destroy();
	//
	MeshParts();
	~MeshParts();
public:
	Name						m_name;
	SubMeshData					m_geometryOriginal;
	std::vector<SubMeshData*>	m_modifyMap;
	MeshData*					m_geometryGroupData;
};
#pragma pack(push,1)
struct	MeshPoint{//导出位置信息
	NameT<64>		name;
	float3			pos;
};
struct	MeshCamera{//导出镜头信息数据
	int				boneid;//如果摄像头有动作则此项大于等于0
	matrix4x4		mat;
	float			fov;
};
#pragma pack(pop)
class MeshData: public Base
{
public:
	MeshData();
	virtual ~MeshData(void);
	//
	void							destroy();
	ImplementComValue();
	Mesh*							CreateRuntime();
	enum MeshType{
		MeshType_StaticMesh	=	0,//静态模型
		MeshType_SceneUnit = 1,//场景对象
		MeshType_Role = 2,//角色对象
	};
public:
	char							IsBlendBoneGeometry(){return m_bCharacter;}		//是否带骨骼绑定
	void							SetIsBlendBoneGeometry(char b){m_bCharacter = b;}	//设置为带骨骼绑定的模型
	//
	char							IsLodRenderEnable(){return m_bLodEnable;}		//是否lod有效
	void							setLodEnable(char b){m_bLodEnable = b;}
	BOOL							IsRenderToShadowMap(){return m_bRenderToShadowMap;}			//是否渲染到阴影贴图
	VOID							SetRenderToShadowMap(char b){m_bRenderToShadowMap = b;}	//是否渲染到阴影贴图
	//
	BOOL							m_bRenderToShadowMap;
	int								addBoneName(const char* szName);
	const char*						GetBoneName(int id);
	int								findBoneIndex(const char* szName);
	int								findBoneIndexLike(const char* szName);
	//
	float							GetMaxVisibleLength();	//设置最长可见距离,超过这个距离将会自动隐藏，主要用于大场景中的快速过滤渲染物体时使用
	VOID							SetMaxVisibleLength(float f);
	//
	inline	int						GetSubmeshCount(){return m_geometrys.size();}
	SubMeshData*					GetSubmeshPtr(int equipID);//根据装备ID查找的可换装模型
	//
	float							GetLodRangeLength(){return m_fLodLength;}
	void							SetLodRangeLength(float f){m_fLodLength = f;}
	BOOL							IsLodEnableMaxVisible();	//LOD是否有效
	VOID							SetLodEnableMaxVisible(BOOL b);//设置LOD有效
	//
	MeshActionRange*				findAction(int actionID);
	MeshActionRange*				findAction(const char* szActionName);
	int								findActionID(const char* szActionName);
	//
	void							RemoveAction(int actionID);
	VOID							ResetActionByName();
	//
	VOID							SetGeometryTypes(unsigned int nTypes){m_nGeometryTypes = nTypes;}//0x00000001,0x00000002~0x80000000, 最多共可以设置30种类型GeomType_...，并且在渲染时或者渲染阴影时单独指定哪些类型需要渲染或者不渲染
	unsigned int					GetGeometryTypes(){return m_nGeometryTypes;}
	//
	int								GetActionCount();
	int								GetActionID(int index);
	MeshActionRange*				GetActionPtr(int id);	//返回动作指针
	MeshActionRange*				GetActionPtr(PtrID& id);	//返回动作指针
	MeshActionRange*				GetActionPtr(const char* szName);	//返回动作指针, 根据名称
	int								GetActionIDByType(int actionType, int index);
	int								GetDefaultActionID(){return m_nDefaultActionID;}							//返回默认动作
	void							SetDefaultActionID(int actionID){m_nDefaultActionID = actionID;}				//设置默认动作
	//
	int								GetBoneCount();
	const char*						GetBoneNameFromIndex(int index);
	//
	BOOL							IsClipVisible(){return m_bClipVisible;}
	VOID							SetClipVisible(BOOL b){m_bClipVisible = b;}
	//
	char							IsAffectByShadowMap(){return m_bCanSelfShadow;}
	void							SetAffectByShadowMap(char b){m_bCanSelfShadow = b;}
	//
	float							GetShadowEpsilon(){return m_shadowEpsilon;}
	VOID							SetShadowEpsilon(float f){m_shadowEpsilon = f;}
	VOID							SetOriginalScale(float fScale);	//设置缩放倍数
	float							GetOriginalScale(){return this->m_fOriginalScale;}	//取得缩放倍数
	//
	INT								GetNewIdentity(){return m_nID++;}
	//
	VOID							SetShadowVisible(BOOL b);
	BOOL							IsShadowVisible(){return m_bShadowVisible;}
	VOID							SetShadowUVOffset(const float2& v){m_shadowOffset = v;}
	const float2&					GetShadowUVOffset(){return m_shadowOffset;}
	VOID							SetShadowTexture(const char* szTexture);
	const char*						GetShadowTexture(){return m_shadowTexture;}	//设置阴影图片，默认是default_init_zip/shadow.dds
	Texture*						GetShadowTexturePtr(){return m_shadowTexturePtr;}	//设置阴影图片，默认是default_init_zip/shadow.dds
	VOID							SetShadowSize(float v){m_fShadowSize = v;}
	float							GetShadowSize(){return m_fShadowSize;}
	ParticleEmitterData*			GetParticlePtr(INT nIndex){return m_particles[nIndex];}
	INT								GetParticleCount(){return m_particles.size();}
	//
	inline	BOOL					UseBaseMatrix(){return (m_bTurn180 || (int)(m_fBaseScale*1000)!=1000);}
public:
	char							LoadGroupData(const char* szFile, ImportResult* result = 0);
	char							LoadGroupData(CSafeFileHelperR& r, ImportResult* result = 0);
	char							SaveGroupData(const char* szFile, ImportResult* result = 0);
	char							SaveGroupData(CSafeFileHelperW& w, ImportResult* result = 0);
	BOOL							LoadMixFile(int actionID, const char* szMixFile);		//返回指定索引的动作id
	//
	BOOL							LoadTexture(TexturePtr& r, char* szTextureFile);
	PakDirectory*					m_pakdir;//是否压缩文件
	char							ExportPak(const char* szFile);
	//
	VOID							RebuildActions(BOOL bResetID = false);
	SubMeshPolygon*					CreatePolygon();
	SubMeshPolygon*					SearchPolygon(int id);
	VOID							RebuildBaseMatrix();
	void							ScaleAll(float fScale);
	void							SetEmissiveAll(const color4& c);
	void							RemoveMesh(int index);
public:
	short							m_actionTypeIDs[Phantom::ActionType_Count][Phantom::ActionType_Max];
	char							m_actionTypeCounts[Phantom::ActionType_Count];
	float							m_fShadowSize;
	BOOL							m_bShadowVisible;
	BOOL							m_bTurn180;
	float							m_fBaseScale;
	matrix4x4						m_baseMatrix;
	float2							m_shadowOffset;
	FileName						m_shadowTexture;//默认default_init_zip\\shadow.dds
	AutoPtr<Texture>				m_shadowTexturePtr;
	CDynamicArrayS<RibbonEmitterData2*,0>		m_ribbons;
	CDynamicArrayS<ParticleEmitterData2*,0>		m_particles;
	CDynamicArrayS<MeshPoint,0>		m_points;//位置信息数据
	CDynamicArrayS<MeshCamera,0>	m_cameras;//镜头信息数据
	char							m_bCharacter;
	Name							m_name;
	CDynamicArrayS<SubMeshData*, 0>	m_geometrys;
	CDynamicArrayS<SubMeshPolygon*, 0>	m_polygons;
	PObjectRegion					m_region;
	PBoxVertices					m_aaboxV;
	char							m_bOptimized;
	BOOL							m_bEnableLODVisible;
	BOOL							m_bClipVisible;
	//
	char							m_bUseOriginal;
	float							m_fOriginalScale;
	matrix4x4						m_originalMatrix;
	char							m_bExistCollision;
	char							m_bExistHeight;
	char							m_bExistRegion;
	char							m_bUseBoneIndex;//是否使用骨骼index作为计算机本绑定值
	float							m_idleHeight;//站立时候的角色高度和盒子，主要是用来进行碰撞检测和高度取值
	//
	float							m_fMaxVisibleLength;
	float							m_fLodLength;
	FileName						m_filename;
	//
	CDynamicArrayS<MeshActionRange*, 0>	m_actionMap;
	CDynamicArrayS<MeshActionData*, 0>	m_actionIO;
	CDynamicArrayS<BoneName, 0>			m_boneNameMap;
	int								m_nID;
	unsigned int					m_nGeometryTypes;
	char							m_bCanSelfShadow;	//是否可以自身受到阴影影响
	char							m_bLodEnable;
	char							m_bShadowEnable;
	float							m_shadowEpsilon;
	int								m_nDefaultActionID;
	BOOL							m_isAlphaBlend;
	//
	char							m_meshType;//模型类型
	int								m_lineID;//模型表格中的行ID
};

class	Mesh;
class	MeshPlaneShadow: public RenderBase
{
public:
	virtual	void							drawObjectShadow(const matrix4x4* matPtr = 0){}
	virtual	char							IsAffectByShadowMap(){return false;}
	virtual	void							drawToShadowMap(const matrix4x4* matPtr, Program* vsps, int nDrawState = RenderNode_SetState){}
	virtual	void							drawObject(const matrix4x4* matPtr = 0);
	virtual	const matrix4x4&				GetWorldMatrix();
	MeshPlaneShadow(){m_ptr = 0;}
	Mesh*	m_ptr;
};

typedef	VOID	(*ProcRenderSubMesh)(Mesh* group, SubMesh* meshPtr);	//当渲染完一个模型是产生

class MeshR: public RenderBase
{
public:
	Mesh*				m_groupPtr;
	virtual	void					drawObject(const matrix4x4* matPtr = 0);
	virtual	const matrix4x4&		GetWorldMatrix();
	virtual	void					drawObjectShadow(const matrix4x4* matPtr = 0){}
	virtual	char					IsAffectByShadowMap(){return 0;}		//是否支持自身阴影渲染
	virtual	void					drawToShadowMap(const matrix4x4* matPtr, Program* vsps, int nDrawState = RenderNode_SetState){}

};

class Unit3D;
class MeshCallback: public Base
{
public:
	virtual	VOID		ActionPlayOver(Mesh* m, int actiontype, const char* actionname){}
	virtual	VOID		ActionAttackBy(Mesh* m, int actionID, int playtype){}
	virtual	VOID		ActionPlayOver(Unit3D* m, int actiontype, const char* actionname){}
	virtual	BOOL		ActionMoveOver(Mesh* m){return false;}
	virtual	VOID		ActionEventRaise(Mesh* m, int actiontype){}
};

class Mesh: public RenderNode
{
public:
	MeshR							m_renderR;
	MeshCallback*					m_callback;
	virtual	void					drawObject(const matrix4x4* matPtr = 0);
	void							Render(BOOL bAutoPush = true);
	void							RenderToShadowMap();
	void							RenderToShadowMap(RenderBase* obj);
	void							AutoPushRender(RenderBase* obj, float fScale);
	void							RenderAlphaOnly(RenderBase* obj, float fScale);
	void							RenderShadow();
	void							FrameMove();	//由地形调用
	void							SetWorldMatrix(const matrix4x4& m, BOOL bResetSavePos = false);
	void							SetCurrentLodLevel(int nLodLevel = -1){m_nCurrentLodLevel = nLodLevel;}	//从0到MAX_LOD_LEVEL_COUNT(6), 如果是-1则自动计算lod
	//SubMeshData*					GetPartEquipPtr(int geometryIndex, int index);//根据装备ID查找的可换装模型
	void							SetObjectRegionEnable(char b){m_bObjectRegion = b;}
	void							SetYawPitchRoll(const float3& v);
	void							SetScale(const float3& v);
	void							SetPosition(const float3& v);
	void							ReplaceImage(const char* szImageSrc, const char* szImageReplace);
	char							IsVisible(){return m_bVisible;}
	void							SetVisible(char b){m_bVisible = b;if(!m_bVisible)this->ResetRibbonParticle();}
	const float3&					GetPosition() const{return this->m_world.GetPosition();}						//得到位置
	int								GetCullMode();
	const color4&					GetDiffuseColor();
	const color4&					GetSpecularColor();
	const color4&					GetEmissiveColor();
	int								GetDestBlendMode(){return m_nDestBlend;}
	int								GetSrcBlendMode(){return m_nSrcBlend;}
	void							SetCullMode(int c);
	void							SetDiffuseColor(const color4& c);
	void							SetAlpha(float fAlpha);
	void							SetSpecularColor(const color4& c);
	void							SetEmissiveColor(const color4& c);
	void							SetDestBlendMode(int b);
	void							SetSrcBlendMode(int b);
	int								m_nSrcBlend,m_nDestBlend;
	//
	int								GetDiffuseIndex(){return m_diffuseIndex;}
	VOID							ChangeDiffuse(int index);
	//
	PObjectRegion&					GetRegion();
	PBoxVertices&					GetBoxVertices();
	//
	inline	const matrix4x4&		GetWorldMatrix(){return m_world;}
	//
	const float3&					GetYawPitchRoll(){return this->m_rotation;}
	const float3&					GetScale(){return m_scale;}
	//
	enum MeshPick{
		MeshPick_Collision	=	0x0001,
		MeshPick_Height		=	0x0002,
		MeshPick_Box		=	0x0004,
		MeshPick_Mesh		=	0x0008,
		MeshPick_Full		=	MeshPick_Collision|MeshPick_Height|MeshPick_Mesh,//最大开销，会进行所有骨骼模型碰撞，如果是带骨骼动画的则只进行盒子碰撞
	};
	char							PickRay(IntersectData* p, matrix4x4* mat, PObjectRegion& sphere, MeshPick mp);	//只点击有collision盒子的物体
	char							PickRay(IntersectData* p, MeshPick mp);	//只点击有collision盒子的物体
	char							GetHeight(IntersectData* inter);
	char							GetHeight(IntersectData* inter, matrix4x4* mat, PObjectRegion& region);
	int								PickIsMoveRegion(IntersectData* p);
	char							IsPointDisableMove(const float3& pos, float* retHeight);
	//
	char							IsShadowVisible(){return m_bShadowVisible;}
	void							shadowVisible(char b){m_bShadowVisible	=	b;}
	BOOL							RemoveBoneIfOne();//如果只有一个关键帧则去掉所有动作
	BOOL							RecalcIdleBox();//重新计算所有角色的盒子
	VOID							RecalcCollisionMesh(SubMesh* smesh);//如果是骨骼动画模型则可以按照这个模型为主模型来生产碰撞检测盒子关键帧
	BOOL							GetCollisionBox(AABox& ret);
	///
	void							destroy();
	void							setGeometryAction(int geometryIdx);
	char							SetActionType(int type, int nLoop, int index);
	char							SetCurrentAction(int actionID, int nLoop);
	char							SetCurrentAction(const char* szName, int nLoop);	//设置当前动作
	void							SetAnimationKey(int nActionID, float fFrame, BOOL bDisableAutoKey = false);
	char							m_bDisableAutoKey;
	float							m_fSetFrameIndex;
	color4							m_diffuse,m_emissive;
	int								m_nPlayType;
	MeshActionRange*				GetCurrentAction();
	char							SetActionInterval(int elapseActionID, int bLoop, int* elapseBoneIndexs, int elapseBoneCount);	//动作合成
	int								GetCurrentActionID();
	//int							GetBoneParentID(int boneID);	//返回父物体boneID
	int								GetBoneIDFromName(const char* szName);
	int								GetBoneIDLike(const char* szName);
	BOOL							GetBoneMatrix(int boneIndex, matrix4x4* pOut);
	const matrix4x4*				GetBoneMatrix(int boneIndex);	//返回骨骼矩阵指针,默认返回模型矩阵指针
	float							GetActionSpeed();
	void							SetActionSpeed(float fSpeed);
	void							SetActionTime(float fTime);
	//
	void							PlayAction(int keyBegin, int keyEnd, int nLoop);
	//
	int								GetLoop();
	void							SetLoop(int nLoop);
	void							Play(char bPlay);
	char							IsPlay(){return m_bPlay;}
	void							Pause(char bPause);
	char							IsPause(){return m_bPause;}
	inline	MeshData*				GetDataPtr(){return m_groupData;}
	RenderNode*						Clone();	//克隆一个自身
	//char							ChangePartEquip(int geometryIndex, int equipID);
	//char							ChangePartEquip(int geometryIndex, const char* szName);
	//char							ChangePartEquip(int equipID);	//替换模型，根据装备ID,在整个模型列表中搜索
	inline	int						GetSubmeshCount(){return m_geometryCount;}
	SubMesh*						GetSubmeshPtr(int index){if(index<0 || index>=m_geometryCount)return 0;if(m_geometryMap)return &m_geometryMap[index];return 0;}
	const matrix4x4*				GetSubmeshMatrix(int geometryIndex);
	SubMesh*						SearchSubmesh(const char* szMeshName);	//根据名字搜索渲染器
	//
	const PObjectRegion&			GetObjectRegion(){return m_objectRegion;}
	const char*						GetBoneName(int boneID);
	const char*						GetName(){return this->m_groupData->m_filename;}
	void							createBindPlugins();
	int								GetDefaultActionID(){return m_defaultActionID;}							//返回默认动作
	void							SetDefaultActionID(int act){m_defaultActionID	=	act;}
	VOID							BindDraw(LPCSTR szBoneName, UIDrawMgr* mgr);
	int								GetBoneCount();	//返回骨骼数量
	struct BindBoneMesh
	{
		int		boneIndex;
		Mesh*	mesh;
	};
	struct BindBoneDraw
	{
		int			boneIndex;
		UIDrawMgr*	mgr;
	};
	BOOL							BindMesh(const char* szBoneName, Mesh* mesh);
	BOOL							UnbindMesh(Mesh* mesh);
	BOOL							SearchBindMesh(Mesh* mesh);
	CDynamicArray<BindBoneMesh,int,0>		m_binds;
	CDynamicArray<BindBoneDraw,int,0>		m_drawMgrs;
	//
	VOID							ResetRibbonParticle();//重置刀光和粒子效果
	VOID							SetParticleScale(float f);
	//
	RibbonEmitter*					m_ribbons;
	CParticleEmitter*				m_particles;
	char							m_bEnabled;//是否有效
	char							m_bFollowCamera;
	//
	VOID							SetParent(Mesh* obj);
	Mesh*							GetParent();
	VOID							AddChild(Mesh* obj);
	VOID							RemoveChild(Mesh* obj);
	VOID							ClearChilds();
	GameID							m_parentID;//父对象ID
	CDynamicArrayS<GameID,0>		m_childs;//子场景数据
	LightAffectData*				m_lightPtr;//灯光数据指针
public://移动计算
	void							MoveTo(const float3& pos, int action = Phantom::ActionType_Walk);//设置移动目标点
	void							MoveDir(const float3& dir, int action = Phantom::ActionType_Walk);
	void							MoveDirStop();
	void							SetRotation(float f);
	float							m_fMoveLength;//移动距离
	float3							m_savedMovePos;
	float3							m_targetMovePos;
	char							m_bMoveMode;//移动模式，在移动模式中移动完毕所有的距离之后会自动停止
	char							m_bAutoMove;//是否可以自动移动
	char							m_bMoveDir;
	char							m_bLoopEndMode;
	//
	void							onChangeWorld();
protected:
	char							CreateGeometry();
	void							transformWorld();
	void							recalcObjectRegion();
	char							m_bRRRegion;
	void							beforeObjectRegion();
	void							recalcBoneMatrix();
	//
	Mesh(MeshData* groupData);
	~Mesh();
	friend class	MeshData;
	GameID							m_gameid;
	friend class	Manager;
public:
	float							m_fMeshIndex;
	unsigned int					m_renderIdx;
	short							m_geometryCount;
	SubMesh*						m_geometryMap;
	char							m_bObjectRegion;
	int								m_nOnceLodLevel;
	char							m_bHasNormalMap;
	int								m_nCurrentLodLevel;
	MeshAction						m_actionDo;
	float							m_fAlphaValue;
	char							m_bGeometryVisible;
	char							m_bUseSystemProgram;//是否使用系统着色器方法
	char							m_nState;
public:
	matrix4x4						m_world;
	char							m_diffuseIndex;
	//
	friend class					SubMesh;
	friend class					MeshAction;
	//
	char							m_bLodVisible;//使用LOD计算之后是否可见
	//
	float3							m_scale;
	float3							m_rotation;
	int								m_defaultActionID;
	char							m_bPlay;
	char							m_bPause;
	char							m_bVisible;
	char							m_bShadowVisible;
	//MeshPlaneShadow					m_falseShadow;
	//PtrID							m_actionID;
	float							m_fSpeed;
	AutoPtr<MeshData>				m_groupData;
	PObjectRegion					m_objectRegion;//只有在group中才进行区域运算
};

};

#endif
