#ifndef __PHANTOM_MAP3D_H__
#define	__PHANTOM_MAP3D_H__
#include "PhantomMapBase.h"
#include "PhantomUIManager.h"
#include "PhantomAStarPath.h"

#define	MAP3D_REGION_GRIDs		32	//每一个区域固定格子数量

namespace Phantom{
	enum 
	{
		TileProp_HIDE		= 0x00000001, //隐藏
		TileProp_BLOCK		= 0x00000002, //是否可走
		TileProp_TEXROT0	= 0x00000040, 
		TileProp_TEXROT1	= 0x00000080, 
		TileProp_TEXROT2	= 0x00000100, 
		TileProp_TEXROT3	= 0x00000200, // 转动
		TileProp_TEXINV		= 0x00000400, // 反转图片
		TileProp_REST_STAND = 0x00000800,
		TileProp_REST_SIT	= 0x00001000,
		TileProp_SAFEZONE	= 0x00002000,
		TileProp_PLAYERKILL = 0x00004000,
		TileProp_FIGHTZONE	= 0x00008000
	};
#pragma pack(push,1)

	struct	MapTile
	{
		unsigned int	attr;//块属性
		unsigned char	texID;//图片ID
	};
	struct	MapObjectData
	{
		int				id;
		int				objID;
		int				type;
		int				subtype;
		float3			pos;
		int				rot;//angle
	};

	struct	MapTrigger//触发器
	{
		int				id;
		Name			name;
		Rect			rect;//触发区域
	};
#pragma pack(pop)
	class Map3DQuadBox;
	class CMapObject: public RenderBase
	{
	public:
		CMapObject();
		~CMapObject();
		virtual	void				Render();
		virtual	void				drawObject(const matrix4x4* matPtr = 0);
		virtual	void				drawObjectShadow(const matrix4x4* matPtr = 0){}
		virtual	char				IsAffectByShadowMap(){return false;}
		virtual	void				drawToShadowMap(const matrix4x4* matPtr, Program* vsps, int nDrawState = RenderNode_SetState){}
		virtual	const matrix4x4&	GetWorldMatrix(){return m_mat;}
		VOID						onChange();
		VOID						FrameMove();
		BOOL						GetSizeZ(float& _retmin, float& _retmax);//返回最高和最低z值
		BOOL						GetHeight(IntersectData* intersect);		//取得当前点的高度
		BOOL						Intersect(IntersectData* intersect, BOOL bCollisionOnly = true);		//取得当前点的高度
		VOID						GetAABox(AABox& _ret);
	public:
		MapObjectData				m_obj;
		Map3DQuadBox*				m_pCurrentRegion;
		PSphereData					m_sphere;
		Rect						m_rect;
		matrix4x4					m_mat;
		CMapObject*					m_pNext;
		Mesh*						m_mesh;//模型对象
	};
	struct	MapTextureFile
	{
		FileName		file;
		BOOL			bFail;//加载失败
		Texture*		texture;//图片对象
	};
	struct	MapObjectFile
	{
		int				id;
		FileName		file;
		Mesh*			mesh;//模型对象
		BOOL			bFail;
	};
	//四叉树管理器
	class Map3DQuadBox
	{
	public:
		Map3DQuadBox(void);
		virtual ~Map3DQuadBox(void);
		//
		VOID			Setup(Map3DQuadBox* pParent, INT nLevel, INT nBeginX, INT nBeginY, INT nRegionWidth, float fRegionSize, INT nLevelIndex);	//构造
		VOID			DestroyData();	//清除数据
		//
		BOOL			InsertObject(CMapObject* obj);
		//
		BOOL			IsBoxInRegion(const Rect& rc);	//测试盒子是否在区域内
		BOOL			IsBoxCollision(const Rect& rc);//测试盒子是否交叉
		BOOL			IsPointInBoxXY(const float3& pos);//测试盒子是否交叉
		//渲染地形
		//VOID			RenderTerrain(Camera* c,  INT nRenderLevel = -1);
		//
		CMapObject*		Intersect(IntersectData* intersect, BOOL bCollisionOnly = true);
		VOID			Intersects(CDynamicArray<CMapObject*>& rets, IntersectData* intersect);
		//多选一组模型
		VOID			SelectObjects(CDynamicArray<CMapObject*>& rets, const Rect& rc);
		//
		BOOL			GetHeight(IntersectData* intersect);		//取得当前点的高度
		//
		CMapObject*		GetFirstObject(){return m_pSceneFirst;}
		CMapObject*		GetNextObject(CMapObject* obj){if(!obj)return NULL; return obj->m_pNext;}
		Map3DQuadBox*		GetChildRegion(INT nIndex){if(nIndex < 0 || nIndex >= QUAD_BOX_COUNT)return NULL; return m_childRegion[nIndex];}
		//
		VOID			InsertToRenderList(float fRegionSize, Map3DQuadBox** renderList, INT& renderCount, INT maxRenderCount, INT minX, INT minY, INT maxX, INT maxY);
		//
		VOID			RecalcRegionBox();
		//
		const AABox&	GetRegionBox(){return m_regionBox;}	//返回区域方块
		//
	protected:
		VOID			RenderAllLevels(Camera* c);
		VOID			FrameMoveAllLevels( Camera* c, INT nRenderLevel = -1);
		//渲染所有物体
		VOID			RenderBox( Camera* c, INT nSelectType = 0);
		//
		virtual	VOID	Render( Camera* c );
		VOID			FrameMove( Camera* c, INT nRenderLevel = -1);
		BOOL			AutoDestroyObject(const AABox& box);
		//插入场景物体
		VOID			InsertSceneNode(CMapObject* obj);
		//移出场景物体
		VOID			RemoveSceneNode(CMapObject* obj);
		//
		VOID			AddObjectCount(INT nCountAdd);
		//
		BOOL			OnChangeSceneObject(CMapObject* obj);
		//
		VOID			ForseVisible(BOOL bVisible);//强制显示或隐藏
		VOID			ResetClipVisible(BOOL bHideOnly = FALSE);//重新计算区块物体,是否只计算不可见物体
		//
		friend class	Map3D;
		friend class	CMapObject;
		friend class	PhantomTerrainBox;
	protected:
		Map3D*				m_mapPtr;
		Map3DQuadBox*		m_childRegion[QUAD_BOX_COUNT];	//子盒子数量
		Map3DQuadBox*		m_pParent;
		CMapObject*			m_pSceneFirst;			//场景物体基类
		//
		int					m_nChildObjectCount;
		Rect				m_rect;
		AABox				m_regionBox;
		short				m_nMaxLevel;
		char				m_nLevelIndex;
		friend class		Map3D;
	};
	class	Map3DQuadBoxScene: public Map3DQuadBox
	{
	public:
		Map3DQuadBoxScene();
		~Map3DQuadBoxScene();
		VOID							SetTile(unsigned int x, unsigned int y, const MapTile& t);
		VOID							SetHeight(unsigned int x, unsigned int y, unsigned char height);
	protected:
		virtual	VOID	Render( Camera* c );
	protected:
		AutoPtr<DeviceMesh>				m_terrain;
		//MapTile*						m_tiles;
		//unsigned char*					m_heights;
		friend class					Map3D;
	};
	class	Map3D
	{
	public:
		Map3D();
		~Map3D();
		VOID							Save(const char* szFileName);
		BOOL							Load(const char* szFileName);//加载
		VOID							Destroy();
		inline	const Pixel&			GetCameraPos(){return m_cameraPos;}
		VOID							SetCameraPos(const Pixel& p);
		//
		VOID							OnFrameMove();
		VOID							Render();
		Texture*						GetTexturePtr(int id);
		Mesh*							LoadMesh(MapObjectData& id);
		Mesh*							GetObjectPtr(MapObjectData& id);
		Map3DQuadBox*					GetSceneRegion(INT xRegion, INT yRegion);	//返回地形区域
		inline	int						GetMaxRegionWidth(){return m_nMaxRegionWidth;}
		VOID							BuildRegions(Map3DQuadBox* pRegion, Map3DQuadBox* pRegionParent, INT& nIndex, INT nLevel, INT nBeginX, INT nBeginY, INT nLevelIndex);
		CMapObject*						InsertObject(const MapObjectData& data);
		CMapObject*						Intersect(IntersectData* intersect, BOOL bCollisionOnly = true);
	public://runtime datas
		BOOL							m_bShowTerrain;
		BOOL							m_bShowObject;
		BOOL							m_bShowBlock;
		CDynamicArray<MapTextureFile>	m_textureFiles;
		CDynamicArray<MapObjectFile>	m_objectFiles;
		FileName						m_mapFile;
		//float2						m_brightContrast;
		float							m_fCameraLength;
	protected:
		Map3DQuadBox*					m_pRegionPtr;
		Map3DQuadBox*					m_regions;
		Map3DQuadBoxScene*				m_sceneRegions;
		int								m_nMaxRegionWidth;
	protected://save data
		Pixel							m_cameraPos;
		int								m_id;
		Size							m_mapSize;//场景大小
		int								m_tileSize;//格子大小
		CDynamicArray<MapTile>			m_tiles;
		CDynamicArray<unsigned char>	m_heights;
		CDynamicArray<CMapObject*>		m_objects;
		CDynamicArray<MapTrigger>		m_triggers;
		//
		friend class					Map3DQuadBox;
		friend class					Map3DQuadBoxScene;
	};


};

#endif
