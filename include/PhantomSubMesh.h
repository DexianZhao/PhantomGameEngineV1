//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_MESH__H__
#define __PHANTOM_MESH__H__
#include "PhantomBone.h"

namespace Phantom{

enum RenderQueue{
	RenderQueue_Default				=	0,
	RenderQueue_NoZBuffer			=	1,
	RenderQueue_AlphaBlend			=	2,
	RenderQueue_GlowMode			=	3,
};
#define	MAX_SHADER_VAR			12

#pragma pack(push,1)
	struct	LightAffectData{
		float4					lightPos[MAX_LIGHT_COUNT];//灯光位置列表xyz+距离
		float4					lightColor[MAX_LIGHT_COUNT];//灯光颜色
		float4					rangeAngle[MAX_LIGHT_COUNT];//如果是方向光则需要这个,near,far,angle s,angle big
		char					count;
		LightAffectData(){memset(this, 0, sizeof(LightAffectData));}
	};
#pragma pack(pop)

class	RenderBase: public Base//渲染物体基础接口
{
public:
	RenderBase(){m_pNext = 0; m_renderIdx = 0;}
	virtual ~RenderBase(){}
	ImplementComValue();
	virtual	void							drawObject(const matrix4x4* matPtr = 0) = 0;
	virtual	const matrix4x4&				GetWorldMatrix() = 0;
protected:
	friend class	Manager;
	RenderBase*		m_pNext;
	unsigned int	m_renderIdx;
};
class	RenderNode;
class	RenderNodeManager
{
public:
	RenderNodeManager();
	virtual ~RenderNodeManager();
	virtual	VOID							PushNode(RenderNode* n);
	virtual	VOID							PopNode(RenderNode* n);
	virtual	void							Render();		//所有设置都有效并进行渲染(包括矩阵数据，透明设置等等)
	virtual	void							FrameMove();				//计算
public:
	CDynamicArray<RenderNode*>	m_renders;//渲染对象
};

class	RenderNode: public Base
{
public:
	ImplementComValue();
	RenderNode(){m_pNext = 0;m_pManager=0;}
	virtual	~RenderNode(){if(m_pManager)m_pManager->PopNode(this);m_pManager=0;}
	virtual	void							Render(BOOL bAutoPush = true) = 0;		//所有设置都有效并进行渲染(包括矩阵数据，透明设置等等)
	virtual	char							IsVisible() = 0;
	virtual	void							SetVisible(char b) = 0;
	virtual	void							RenderShadow() = 0;	//渲染阴影
	virtual	const PObjectRegion&			GetObjectRegion() = 0;	//返回物体区间
	virtual	const matrix4x4&				GetWorldMatrix() = 0;									//取得矩阵
	virtual	void							SetWorldMatrix(const matrix4x4& mat, BOOL bResetSavePos = true) = 0;
	virtual	void							FrameMove() = 0;				//计算
	virtual	const char*						GetName() = 0;					//返回名称，可返回""
	//virtual	char							Pick(IntersectData* p) = 0;	//点击
	virtual	RenderNode*						Clone() = 0;	//克隆一个自身
	//
protected:
	friend class	Manager;
	friend class	RenderNodeManager;
	RenderNode*								m_pNext;
	RenderNodeManager*						m_pManager;
};

class LODMesh
{
public:
	LODMesh();
	~LODMesh();
	//
	BOOL					IsLODCreated(){return (m_pLODMeshs[0].t != NULL);}
	VOID					LoadLODMesh(StreamBase* s, BOOL bOldVersion = FALSE);
	VOID					SaveLODMesh(StreamBase* s);
	VOID					DestroyLODMesh();
	VOID					DestroyAllData();
	INT						GetLODMeshCount(){return MAX_LOD_LEVEL_COUNT;}
	//BOOL					IsLODDataExist(){return (m_nSeekPointer.LowPart > 0);}
	DeviceMesh*				GetLODMesh(unsigned int levelIndex, int& faceCount);
#ifdef DIRECT_X_9
	BOOL					GenericLod(void* p1, void* p2);
#endif
protected:
	AutoPtr<DeviceMesh>		m_pLODMeshs[MAX_LOD_LEVEL_COUNT];
	unsigned short			m_nLodLevelFaces[MAX_LOD_LEVEL_COUNT];
};

class bzExportMeshData;

struct	AnimationVertex
{
	float3						p;
	float3						dir;
	unsigned int				diffuse;
	unsigned int				diffuseDest;
	float2						t;
	float2						tdir;
	float3						normal;
	float3						normalDir;
	//static	const unsigned int			c_fvf	=	Phantom::FVF_XYZ|Phantom::FVF_NORMAL|Phantom::FVF_DIFFUSE|Phantom::FVF_SPECULAR|Phantom::FVF_TEX5;
	//
	inline	AnimationVertex()															{}
	inline	AnimationVertex&			operator = (const AnimationVertex& v)		{memcpy(this,&v,sizeof(AnimationVertex));return *this;}
};

struct	AnimationVertexMul
{
	int						faceStart;
	int						faceCount;
	char					diffuse[128];
	Texture*			texture;
	AnimationVertexMul(){faceStart	=	faceCount	=	0;	texture	=	0; diffuse[0] = 0;}
	~AnimationVertexMul(){safe_release(texture);}
};

class VertexAnimationMesh
{
public:
	VertexAnimationMesh(void);
	virtual	~VertexAnimationMesh(void);
	//
	void					initializeFrom(bzExportMeshData* data, const char* szTexturePath);
	void					destroy();
	//
	void					RenderNoShader(Program* vsps, float fAnimationTime, BOOL bSetFrame = false);
	void					RenderNoShader(int attrID, Program* vsps, float fAnimationTime, BOOL bSetFrame = false);
	//
	void					Render(Program* vsps, float fAnimationTime);
	//
	char					IsSelfAnimationMesh(){return (m_vertexMap != NULL);}
	//
	void					SaveGroupData(StreamBase* file);
	void					LoadGroupData(StreamBase* file);
	//
public:
	int						m_vertexFullCount;
	int						m_animationKeyCount;
	int						m_meshVertexCount;
	int						m_materialCount;
	int						m_faceCount;
	float					m_fAnimationStep;
	_shader_var_name		m_vertexLerp;
	char					m_bTwoSide;
	_shader_var_name		m_technique;
	//
	AnimationVertex*		m_vertexMap;
	AnimationVertexMul*		m_materialMap;
	Triangle*				m_faceMap;
public:
	Program*				m_lastVsps;
};

struct BoneMatrix
{
	matrix4x4						*boneMatrix;
	int								*boneIdx;
	int								localBoneIdx;
	void							reset(int numBone);
	BoneMatrix();
	~BoneMatrix();
};

struct	PShaderValue
{
	int					id;
	Name				name;
	int					type;
	float4				value;
};

class	MeshMaterial: public MeshMaterialAttribute
{
public:
	MeshMaterial();
	~MeshMaterial();
	void								SetTexture(TEX_ID id, Texture* tex);	//设置贴图
	Texture*							GetTexture(TEX_ID id);	//取得贴图
	char								IsVisible(){return this->visible;}
	VOID								SetVisible(char b){this->visible = b;}
	const color4*						GetDiffuseColor(int index);
public:
	int									programID[2];
	int									programToSMID;//to shadow map id
	AutoPtr<Texture>					texturePtr[TEXTURE_MAX_COUNT];
	AutoPtr<Texture>					diffusePtr[DIFFUSE_TEXTURE_COUNT];
	CDynamicArrayS<Texture*,0>			m_uvmaps;//UV动画中的图片列表，此项跟着第一个图片的载入会全部被自动载入，必须同时存在两个条件，第一个条件是图片必须uvmap_开头，第二个是这个图片对应的.txt文件中列出所有uv动画序列
	char								m_uvmapLoop;
	float								m_uvmapSpeed;
	_shader_var							shadowTextureSize;
	MaterialAnim						anims;//材质动画
	char								bEmissiveZero;
};

class SubMeshPolygon: public Base
{
public:
	SubMeshPolygon();
	virtual ~SubMeshPolygon(void);
	char								SaveData(CSafeFileHelperW& w);
	char								LoadData(CSafeFileHelperR& r);
	char								IsLodExists(){return m_bLodEnable;}//this->m_lodMesh.IsLODCreated();}	//是否创建lod
	int									GetLodLevelCount(){return MAX_LOD_LEVEL_COUNT;}
	int									GetLevelLodIndex(int level);	//返回lod对应的模型index
	void								SetLevelLodIndex(int level, int lodIndex);
	int									GetLodLevelFaceCount(int level);	//返回lod对应的模型显示的面数量
	//
	int									GetVertexCount(){if(m_pRenderMesh)return this->m_pRenderMesh->GetNumVertices();return 0;}
	const Vertex*						GetVertexPtr(int index);
	int									GetFaceCount(){if(m_pRenderMesh)return m_pRenderMesh->GetNumFaces();return 0;}
	const Triangle*						GetFacePtr16(int index);
	const Triangle32*					GetFacePtr32(int index);
	char								Is32BitMesh(){return m_b32BitMesh;}	//是否是32位索引缓冲区
	char								IsBlendBoneMesh(){return this->m_bBlendVertex;}
	char								IsHasTangentBinormal(){return this->m_bHasTBN;}	//是否存在Tangent和binormal信息
	void								GenericTangentBinormal(char bHasTBN = true);	//生成Tangent和binormal信息
	int									GetBoneCount(){return m_boneCount;}
	const MeshBoneHelper&				GetBonePtr(int index){return m_boneMap[index];}
	VOID								ScaleAll(float fScale);
#ifdef DIRECT_X_9
	VOID								GenericLodMesh(BOOL bGen);
	BOOL								OptimizeMesh(BOOL bGenLod);
#endif
	void								ComputeNormals();
public:
	int									m_id;
	char								m_levelLodIndex[MAX_LOD_LEVEL_COUNT];
	LODMesh								m_lodMesh;
	int									m_boneCount;
	MeshBoneHelper*						m_boneMap;
	AutoPtr<DeviceMesh>					m_pRenderMesh;
	PObjectRegion						m_objectRegion;
	PBoxVertices						m_aaboxVertex;
	char								m_b32BitMesh;
	char								m_bLodEnable;//lod是否开启过
	char								m_bBlendVertex;		//是否是blend模型
	char								m_bHasTBN;			//是否存在tangent，binormal，normal
	char								m_bOptimized;
};

#define	MAX_SHOW_ON_ACTION				5
class SubMeshData: public Base
{
public:
	SubMeshData();
	virtual ~SubMeshData(void);
public:
	ImpComValue2();
	void								destroy();	//释放所有的数据
	//
	char								pickGeometry(const PObjectRegion* region, BoneMatrix* boneMatrix, matrix4x4* vWorld, IntersectData* p, void* pickData, char bDoubleSide);
	char								pickBox(const PObjectRegion* region, BoneMatrix* boneMatrix, matrix4x4* vWorld, IntersectData* p, void* pickData, char bDoubleSide);
	//
	int									GetNewIdentity();
	void								renderLODData(Program* prog, int attribID, int nLod);
	void								renderShadowData();
	//
	char								SaveGroupData(CSafeFileHelperW& w);
	char								LoadGroupData(CSafeFileHelperR& r);
	char								GetExistMeshAnim(){return m_bExistMeshAnim;}
	void								SetExistMeshAnim(char b){m_bExistMeshAnim = b;}
	//
#ifdef DIRECT_X_9
	VOID								ConvertToPng(const char* szTexturePath, const char* srcFile, const char* destFile);
#endif
	char								ReloadAllTextures(const char* szSrcPath = NULL);
	Texture*							GetDiffuseTexture(int material, int index, float uvmapIndex, BOOL& bRender);
	void								SortAlphaBlendGeometrys();
	//
	BOOL								AddRenderTriangles(int nLod);
	char								IsAlphaBlend();
	//
public:
	MeshMaterial*						GetMaterialPtr(int index);
	unsigned int						GetMaterialCount(){return this->m_numMaterials;}
	void								SetName(const char* str){m_name = str;}//strcpy(m_strName, str); strlwr_t(m_strName);}
	const char*							GetName(){return m_name;}
	const MeshBoneHelper&				GetBoneLocal(){return m_boneLocal;}
	//
	int									GetID(){return m_nID;}
	void								setGeometryID(int id){m_nID	=	id;}
	//
	int									GetEquipType(){return m_geometryType;}		//返回装备类型，在geometryIDMap.editor中定义的
	void								SetEquipType(int type){m_geometryType = type;}
	//
	char								IsVisible(){return m_bVisible;}
	void								SetVisible(char b){m_bVisible = b;}
	//
public:
	char								IsZWriteEnable(){return this->m_bZWriteEnable;}	//是否开关Z轴写入
	void								SetZWriteEnable(char b){m_bZWriteEnable = b;}
public:
	//char								m_bLookatCamera;
	//float								m_fLookatCameraSize;
	//
	char								m_bZWriteEnable;
	char								m_bVisible;
	char								m_bExistMeshAnim;
	int									m_geometryType;
	char								m_bEnableShadow;
	char								m_bToShadowMap;
	//
	int									m_nID;
	Name								m_name;
	matrix4x4							m_world;
	bool								m_bIsAlphaBlend;
	MeshBoneHelper						m_boneLocal;
	MeshMaterial*						m_materials;//直接在这里排序
	int									m_numMaterials;
	SubMeshPolygon*						m_polygons;
	MeshData*							m_groupPtr;
	int*								m_renderMaterialIndex;
	int									m_showOnAction[MAX_SHOW_ON_ACTION];
	char								m_showOnActionCount;
	//
	//
	//float								m_fSlopeScaleDepthBias, m_fDepthBias;
	//char								m_bBiasEnable;//设置斜率是否有效
	char								m_bExistShadowMap;
	char								m_bIsCollision;//是否碰撞模型
	char								m_bIsHeight;//是否高度获取模型
	char								m_bIsDisable;//是否禁止行走的模型
	//char								m_bUseObjectRegion;
	char								m_bUseWorld;
	char								m_lodEnable;
	//
	VOID								CheckTypeFromName();
protected:
	char								buildDatas();
	friend class						MeshData;
	friend class						MeshParts;
	friend class						SubMesh;
	//
};

class SubMesh: public RenderBase
{
public:
	SubMesh();
	virtual ~SubMesh(void);
public://implements
	void							Create(SubMeshData* data, Mesh* groupPtr);
	//
	void							drawObject(const matrix4x4* matPtr = 0);
	void							drawObject(const matrix4x4* matPtr, int nLodLevel);
	char							IsAffectByShadowMap();			//是否支持自身阴影渲染
	void							drawToShadowMap(const matrix4x4* matPtr = 0, int nLodLevel = -1);
	float4*							GetShaderVar(PtrID& id);
	//
	Program*						drawSetShader(const matrix4x4* matPtr, int& programID, int nProgramType);
	//
	void							SetAlphaValue(float fAlphaValue);
	char							pickGeometry(IntersectData* p, matrix4x4* matWorld, void* pickData);//在 group 中调用
	char							pickBox(IntersectData* p, matrix4x4* matWorld, void* pickData);//在 group 中调用
	//
	void							RemoveBoneIfOne();//如果只有一个关键帧则去掉所有动作
	AABox							RecalcIdleBox();//
	void							refreshValues(Program* vsps, MeshShaderType t);
	VOID							RebuildShadowTechniques(Program* shader);
	inline	BOOL					IsCollision(){return m_geometryData->m_bIsCollision;}
	void							recalcObjectRegion();
	//
	//const color4&					GetDiffuseColor();
	//const color4&					GetSpecularColor();
	//const color4&					GetEmissiveColor();
	//void							SetDiffuseColor(const color4& c);
	//void							SetSpecularColor(const color4& c);
	//void							SetEmissiveColor(const color4& c);
	void							ReplaceImage(const char* szImageSrc, const char* szImageReplace);
	AABox							GetAABox();
	inline	BOOL					IsVisibled(){return (this->m_bVisible||this->m_bShowOnAction);}
public:
	const char*						GetName(){return m_geometryData->GetName();}
	MeshMaterial*					GetMaterialPtr(unsigned int nIndex);
	unsigned int					GetMaterialCount();
	//
	SubMeshData*					GetDataPtr(){return m_geometryData;}
	//
	char							IsVisible(){return m_bVisible;}
	void							SetVisible(char b){m_bVisible = b;}
	const matrix4x4&				GetWorldMatrix();
	VOID							SetWorldMatrix(const matrix4x4& mat);
	//
	VOID							ResetData(SubMeshData* data);
	Texture**						m_customTexture;
	matrix4x4						m_worldLocal;
	Mesh*							m_groupPtr;
	char							m_bExistMeshAnim;
	color4*							m_diffusePtr;
	color4*							m_emissivePtr;
	float							m_fAlpha;
	short							m_nMtrlFrameID;
protected:
	friend class					Mesh;
	SubMeshData*					m_geometryData;
	BoneMatrix						m_boneMatrix;
	PSphereData						m_sphere;
	char							m_isAlphaBlend;
	//
	float							m_fVertexAnimationTime;
	char							m_bVisible;
	char							m_bShowOnAction;
	//
	char							m_bHasNormalMap;
};


class	MeshShaderProject
{
public:
	MeshShaderProject();
	virtual	~MeshShaderProject();
	INT									GetID();
	int									GetType(){return 0;}
	VOID								CloneFrom(MeshShaderProject* s);
public:
	const char*							GetTehcnique(){return m_defaultTechnique.name;}
	void								SetTehcnique(const char* s);
	//
	const char*							GetName(){return m_name;}
	VOID								SetName(const char* szName){m_name = szName;}
	VOID								Save(CFileHelperW& w);
	VOID								Load(CFileHelperR& r);
	//
	VOID								SetVariantChanged(){m_nShaderVariantChanged++;}
	DataManager<PShaderValue, SubMeshData>*	GetVariants(){return &m_variants;}
	//
	BOOL								IsForceAlphaBlend(){return m_bIsForceAlphaBlend;}		//是否强制透明混合
	VOID								SetForceAlphaBlend(BOOL b){m_bIsForceAlphaBlend = b;}
	//
	Phantom::BlendMode						GetSrcBlend(){return m_srcBlend;}		//是否强制透明混合
	VOID								SetSrcBlend(Phantom::BlendMode op){m_srcBlend = op;}
	Phantom::BlendMode						GetDestBlend(){return m_destBlend;}		//是否强制透明混合
	VOID								SetDestBlend(Phantom::BlendMode op){m_destBlend = op;}
	//
	Phantom::CullMode						GetCullMode(){return m_cullMode;}		//是否强制透明混合
	VOID								SetCullMode(Phantom::CullMode op){m_cullMode = op;}
	//
	char								IsZWriteEnable(){return m_bZWriteEnable;}	//是否开关Z轴写入
	void								SetZWriteEnable(char b){m_bZWriteEnable = b;}
public:
	Name								m_name;
	int									m_id;
	_shader_var_name					m_defaultTechnique;
	INT									m_nShaderVariantChanged;
	INT									m_nShaderTechniqueChanged;
	DataManager<PShaderValue, SubMeshData>			m_variants;
	//
	BOOL								m_bIsForceAlphaBlend;
	Phantom::CullMode					m_cullMode;
	Phantom::BlendMode					m_srcBlend;
	Phantom::BlendMode					m_destBlend;
	BOOL								m_bZWriteEnable;
};

};

#endif
