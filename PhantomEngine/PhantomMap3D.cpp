#include "PhantomMap3D.h"
#include "LuaManager.h"
#include "APILua.h"

namespace Phantom{

	CMapObject::CMapObject()
	{
		memset(&m_obj, 0, sizeof(m_obj));
		m_mat	=	matrix4x4::s_identity();
		m_mesh	=	0;//模型对象
		m_pCurrentRegion	=	0;
		m_pNext	=	0;
	}
	CMapObject::~CMapObject()
	{
	}
	VOID	CMapObject::GetAABox(AABox& _ret)
	{CPUTime(CMapObject);
		if(!m_mesh)return;
		const AABox& box = m_mesh->GetDataPtr()->m_objectRegion.aabox;
		_ret._min.z = box._min.z + this->m_obj.pos.z;
		_ret._max.z = box._max.z + this->m_obj.pos.z;
		_ret._min.x = m_rect.left;
		_ret._min.y = m_rect.top;
		_ret._max.x = m_rect.right;
		_ret._max.y = m_rect.bottom;
	}
	BOOL	CMapObject::Intersect(IntersectData* intersect, BOOL bCollisionOnly)		//取得当前点的高度
	{CPUTime(CMapObject);
		if(!m_mesh)
			return false;
		if(bCollisionOnly)
			return m_mesh->PickCollisionMeshOnly(intersect, m_sphere, &m_mat);
		return m_mesh->Pick(intersect, m_sphere, &m_mat);
}
	BOOL	CMapObject::GetHeight(IntersectData* inter)		//取得当前点的高度
	{CPUTime(CMapObject);
		if(!m_mesh)
			return false;
		if(inter->GetOriginal().x < m_rect.left || inter->GetOriginal().y < m_rect.top || inter->GetOriginal().x > m_rect.right || inter->GetOriginal().y > m_rect.bottom)
			return false;
		return m_mesh->GetHeight(inter, &m_mat);
	}
	BOOL	CMapObject::GetSizeZ(float& _retmin, float& _retmax)
	{CPUTime(CMapObject);
		if(!m_mesh)
		{
			_retmin = 0;
			_retmax = 1;
			return false;
		}
		const AABox& box = m_mesh->GetDataPtr()->m_objectRegion.aabox;
		_retmin = box._min.z + this->m_obj.pos.z;
		_retmax = box._max.z + this->m_obj.pos.z;
		return true;
	}
	VOID	CMapObject::FrameMove()
	{CPUTime(CMapObject);
	}
	VOID	CMapObject::onChange()
	{CPUTime(CMapObject);
		if(!m_mesh)
			return;
		m_mat.RotationZ(ToDegress(m_obj.rot));
		m_mat.SetPosition(m_obj.pos);
		PObjectRegion obj;
		obj.aabox._min.setxyz(100000000.0f, 100000000.0f, 100000000.0f);
		obj.aabox._max.setxyz(-100000000.0f, -100000000.0f, -100000000.0f);
		for(int i=0;i<8;i++)
		{
			float3 v;
			m_mesh->GetDataPtr()->m_aaboxVertex.v[i].transform(v, m_mat);
			obj.aabox._max.maxv(obj.aabox._max, v);
			obj.aabox._min.minv(obj.aabox._min, v);
		}
		//
		float3 vM = (obj.aabox._max + obj.aabox._min)*0.5f;
		float fS = (obj.aabox._max - vM).R();
		m_sphere.setup(vM, fS);
		m_rect.Set(obj.aabox._min.x + m_obj.pos.x,obj.aabox._min.y+m_obj.pos.y, obj.aabox._max.x+m_obj.pos.x, obj.aabox._max.y+m_obj.pos.y);
	}
	void	CMapObject::Render()
	{CPUTime(CMapObject);
		if(!m_mesh)
			return;
		m_mesh->AutoPushRender(this, 1.0f);
	}
	void	CMapObject::drawObject(const matrix4x4* matPtr)
	{CPUTime(CMapObject);
		if(!m_mesh)
			return;
		m_mesh->RenderObject(this, 1.0f);
	}


	Map3DQuadBox::Map3DQuadBox(void)
	{
		m_pSceneFirst		=	0;
		m_nChildObjectCount	=	0;
		m_pParent			=	0;
		memset(m_childRegion, 0, sizeof(m_childRegion));
		m_nMaxLevel			=	0;
		m_nLevelIndex		=	0;
	}

	Map3DQuadBox::~Map3DQuadBox(void)
	{
		while(m_pSceneFirst)
		{
			m_pSceneFirst->m_pCurrentRegion	=	NULL;
			m_pSceneFirst	=	m_pSceneFirst->m_pNext;
		}
		DestroyData();
	}

	VOID			Map3DQuadBox::DestroyData()
	{CPUTime(Map3DQuadBox);
		for(int i=0;i<QUAD_BOX_COUNT;i++)
			m_childRegion[i]	=	0;
	}

	VOID			Map3DQuadBox::Setup(Map3DQuadBox* pParent, INT nLevel, INT nBeginX, INT nBeginY, INT nRegionWidth, float fRegionSize, INT nLevelIndex)	//构造
	{CPUTime(Map3DQuadBox);
		DestroyData();
		//
		this->m_nLevelIndex	=	nLevelIndex;
		this->m_nMaxLevel	=	nLevel;
		this->m_pParent		=	pParent;
		//
		float fLevelHeight	=	(float)m_nLevelIndex;
		AABox box;
		box._min.setxyz((float)nBeginX * fRegionSize, (float)nBeginY * fRegionSize, -fLevelHeight);
		box._max.setxyz((float)nBeginX * fRegionSize + (float)nRegionWidth * fRegionSize, (float)nBeginY * fRegionSize + (float)nRegionWidth * fRegionSize, fLevelHeight);
		m_regionBox			=	box;
		m_rect.Set(box._min.x, box._min.y, box._max.x, box._max.y);
	}

	VOID			Map3DQuadBox::InsertToRenderList(float fRegionSize, Map3DQuadBox** renderList, INT& renderCount, INT maxRenderCount, INT minX, INT minY, INT maxX, INT maxY)
	{CPUTime(Map3DQuadBox);
		int regionLeft		=	(int)(m_regionBox._min.x / fRegionSize);
		int regionTop		=	(int)(m_regionBox._min.y / fRegionSize);
		int regionRight		=	(int)(m_regionBox._max.x / fRegionSize);
		int regionBottom	=	(int)(m_regionBox._max.y / fRegionSize);
		if(regionRight < minX || regionLeft > maxX || regionBottom < minY || regionTop > maxY || renderCount >= maxRenderCount)
			return;
		renderList[renderCount++]	=	this;
		for(int i=0;i<QUAD_BOX_COUNT;i++)
		{
			if(m_childRegion[i])
				m_childRegion[i]->InsertToRenderList(fRegionSize, renderList, renderCount, maxRenderCount, minX, minY, maxX, maxY);
		}
	}

	VOID			Map3DQuadBox::FrameMoveAllLevels( Camera* c, INT nRenderLevel)
	{CPUTime(Map3DQuadBox);
		if(this->m_nChildObjectCount <= 0)
			return;
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
					m_childRegion[i]->FrameMoveAllLevels(c, nRenderLevel);
			}
		}
		FrameMove( c, nRenderLevel);
	}

	VOID			Map3DQuadBox::FrameMove( Camera* c, INT nRenderLevel)
	{CPUTime(Map3DQuadBox);
		if(this->m_nChildObjectCount <= 0)
			return;
		CMapObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			pFirst->FrameMove();
			pFirst	=	pFirst->m_pNext;
		}
	}
	VOID			Map3DQuadBox::RenderAllLevels(Camera* c)
	{CPUTime(Map3DQuadBox);
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
					m_childRegion[i]->RenderAllLevels(c);
			}
		}
		Render( c );
	}
	VOID			Map3DQuadBox::Render( Camera* c )
	{CPUTime(Map3DQuadBox);
		if(!this->m_mapPtr->m_bShowObject)return;
		if(this->m_nChildObjectCount <= 0)
			return;
		CMapObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			pFirst->Render();
			pFirst	=	pFirst->m_pNext;
		}
	}
	//插入场景物体
	VOID			Map3DQuadBox::InsertSceneNode(CMapObject* obj)
	{CPUTime(Map3DQuadBox);
		if(obj->m_pCurrentRegion)
			obj->m_pCurrentRegion->RemoveSceneNode(obj);
		//
		CMapObject* objFirst	=	m_pSceneFirst;
		while(objFirst)
		{
			if(objFirst == obj)
				return;
			objFirst	=	objFirst->m_pNext;
		}
		//
		if(m_pSceneFirst)
			obj->m_pNext	=	m_pSceneFirst;
		//
		float fmin,fmax;
		if(obj->GetSizeZ(fmin, fmax))
		{
			float3& _min		=	m_regionBox._min;
			float3& _max		=	m_regionBox._max;
			float fLevelHeight	=	(float)m_nLevelIndex;
			float minZ			=	fmin - fLevelHeight;
			float maxZ			=	fmax - fLevelHeight;
			float fMax			=	getmax(maxZ, _max.z);
			float fMin			=	getmin(minZ, _min.z);
			m_regionBox._max.z	=	fMax;
			m_regionBox._min.z	=	fMin;
		}
		m_pSceneFirst	=	obj;
		obj->m_pCurrentRegion		=	this;
		this->AddObjectCount(1);
	}

	//移出场景物体
	VOID			Map3DQuadBox::RemoveSceneNode(CMapObject* obj)
	{CPUTime(Map3DQuadBox);
		if(obj->m_pCurrentRegion != this)
		{
			obj->m_pCurrentRegion->RemoveSceneNode(obj);
			return;
		}
		if(obj == m_pSceneFirst)
		{
			m_pSceneFirst	=	m_pSceneFirst->m_pNext;
		}
		else
		{
			BOOL bFind	=	false;
			CMapObject* objFirst	=	m_pSceneFirst;
			while(objFirst)
			{
				if(objFirst->m_pNext == obj)
				{
					objFirst->m_pNext	=	obj->m_pNext;
					bFind	=	TRUE;
					break;
				}
				objFirst	=	objFirst->m_pNext;
			}
			assert(bFind == TRUE);
		}
		obj->m_pNext				=	NULL;
		obj->m_pCurrentRegion		=	NULL;
		this->AddObjectCount(-1);
	}

	BOOL			Map3DQuadBox::InsertObject(CMapObject* obj)
	{CPUTime(Map3DQuadBox);
		if(!obj)return FALSE;
		if(obj->m_pCurrentRegion)
			obj->m_pCurrentRegion->RemoveSceneNode(obj);
		//
		if(IsBoxInRegion(obj->m_rect))	//如果全部在此物体内
		{
			if(this->m_nMaxLevel > 0)
			{
				for(int i=0;i<QUAD_BOX_COUNT;i++)
				{
					if(this->m_childRegion[i])
					{
						if(this->m_childRegion[i]->IsBoxInRegion(obj->m_rect))
						{
							return m_childRegion[i]->InsertObject(obj);
						}
					}
				}
			}
			InsertSceneNode(obj);
			return true;
		}
		if(m_pParent)	//让父区域添加物体
			return m_pParent->InsertObject(obj);
		else
		{
			InsertSceneNode(obj);
			return true;
		}
		return false;
	}

	BOOL			Map3DQuadBox::OnChangeSceneObject(CMapObject* obj)
	{CPUTime(Map3DQuadBox);
		if(IsBoxInRegion(obj->m_rect))	//如果全部在此物体内
		{
			if(this->m_nMaxLevel > 0)
			{
				for(int i=0;i<QUAD_BOX_COUNT;i++)
				{
					if(this->m_childRegion[i])
					{
						if(this->m_childRegion[i]->IsBoxInRegion(obj->m_rect))
						{
							return m_childRegion[i]->InsertObject(obj);
						}
					}
				}
			}
			//如果还是在原来的盒子内则返回
			if(obj->m_pCurrentRegion == this)
				return TRUE;
			InsertSceneNode(obj);
			return TRUE;
		}
		if(m_pParent)	//让父区域添加物体
			return m_pParent->InsertObject(obj);
		else
		{
			InsertSceneNode(obj);
			return true;
		}
		return false;
	}

	BOOL			Map3DQuadBox::IsBoxCollision(const Rect& rc)//测试盒子是否交叉
	{CPUTime(Map3DQuadBox);
		if(rc.right < m_rect.left || rc.left > m_rect.right || rc.bottom < m_rect.top || rc.top > m_rect.bottom)
			return false;
		return true;
	}

	VOID			Map3DQuadBox::RecalcRegionBox()
	{CPUTime(Map3DQuadBox);
		if(this->m_nChildObjectCount <= 0)
			return;
		float fMaxHeight = -100000.0f;
		float fMinHeight = 100000.0f;
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
				{
					m_childRegion[i]->RecalcRegionBox();
					fMaxHeight	=	getmax(m_childRegion[i]->m_regionBox._max.z, fMaxHeight);
					fMinHeight	=	getmin(m_childRegion[i]->m_regionBox._min.z, fMinHeight);
				}
			}
		}
		//
		CMapObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			//if(pFirst->IsAllowSelect(SELECT_OBJECT_GEOMETRY))
			float fmin, fmax;
			if(pFirst->GetSizeZ(fmin, fmax))
			{
				fMaxHeight	=	getmax(fMaxHeight, fmax);
				fMinHeight	=	getmin(fMinHeight, fmin);
			}
			pFirst	=	pFirst->m_pNext;
		}
		this->m_regionBox._max.z	=	fMaxHeight;
		this->m_regionBox._min.z	=	fMinHeight;
	}

	BOOL			Map3DQuadBox::IsBoxInRegion(const Rect& rc)	//测试盒子是否在区域内
	{CPUTime(Map3DQuadBox);
		if(rc.left >= m_rect.left && rc.top >= m_rect.top && rc.bottom <= m_rect.bottom && rc.right <= m_rect.right)
			return true;
		return false;
	}

	VOID			Map3DQuadBox::AddObjectCount(INT nCountAdd)
	{CPUTime(Map3DQuadBox);
		this->m_nChildObjectCount	+=	nCountAdd;
		if(this->m_pParent)
		{
			m_pParent->AddObjectCount( nCountAdd );
			m_pParent->m_regionBox._min.z	=	getmin(m_pParent->m_regionBox._min.z, this->m_regionBox._min.z);
			m_pParent->m_regionBox._max.z	=	getmax(m_pParent->m_regionBox._max.z, this->m_regionBox._max.z);
		}
		assert(m_nChildObjectCount >= 0);
	}

	VOID			Map3DQuadBox::Intersects(CDynamicArray<CMapObject*>& objs, IntersectData* intersect)
	{CPUTime(Map3DQuadBox);
		if(this->m_nChildObjectCount <= 0)
			return;
		float3& _min		=	m_regionBox._min;
		float3& _max		=	m_regionBox._max;
		if(!IntersectBox(intersect->GetOriginal(), intersect->GetDir(), _min, _max, NULL))
			return;
		CMapObject* ret = 0;
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
				{
					m_childRegion[i]->Intersects(objs, intersect);
				}
			}
		}
		//
		CMapObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			if(pFirst->Intersect( intersect ))
			{
				objs.push_back( pFirst );
			}
			pFirst	=	pFirst->m_pNext;
		}
	}
	CMapObject*	Map3DQuadBox::Intersect(IntersectData* intersect, BOOL bCollisionOnly)
	{CPUTime(Map3DQuadBox);
		if(this->m_nChildObjectCount <= 0)
			return NULL;
		float3& _min		=	m_regionBox._min;
		float3& _max		=	m_regionBox._max;
		if(!IntersectBox(intersect->GetOriginal(), intersect->GetDir(), _min, _max, NULL))
			return NULL;
		CMapObject* ret = 0;
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
				{
					CMapObject* obj = m_childRegion[i]->Intersect(intersect, bCollisionOnly);
					if(obj)
						ret = obj;
				}
			}
		}
		//
		CMapObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			{
				if(pFirst->Intersect( intersect, bCollisionOnly ))
					ret = pFirst;
			}
			pFirst	=	pFirst->m_pNext;
		}
		return ret;
	}

	VOID			Map3DQuadBox::SelectObjects(CDynamicArray<CMapObject*>& objs, const Rect& rc)
	{CPUTime(Map3DQuadBox);
		if(this->m_nChildObjectCount <= 0)
			return;
		CMapObject* ret = 0;
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
					m_childRegion[i]->SelectObjects(objs, rc);
			}
		}
		//
		CMapObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			Pixel p;
			if(GetCameraPtr()->Get2DFrom3D(p, pFirst->m_obj.pos))
			{
				if(rc.isPtIn(p.x, p.y))
				{
					objs.push_back( pFirst );
				}
			}
			pFirst	=	pFirst->m_pNext;
		}
	}
	BOOL					Map3DQuadBox::IsPointInBoxXY(const float3& pos)//测试盒子是否交叉
	{CPUTime(Map3DQuadBox);
		float3& _min		=	m_regionBox._min;
		float3& _max		=	m_regionBox._max;
		return ((pos.x >= _min.x && pos.x <= _max.x)
			&& (pos.y >= _min.y && pos.y <= _max.y));
	}

	BOOL					Map3DQuadBox::GetHeight(IntersectData* intersect)
	{CPUTime(Map3DQuadBox);
		if(this->m_nChildObjectCount <= 0)
			return NULL;
		if(!IsPointInBoxXY(intersect->GetOriginal()))
			return NULL;
		//
		BOOL bRet	=	FALSE;
		CMapObject* ret = 0;
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
				{
					if(m_childRegion[i]->GetHeight(intersect))
						bRet = TRUE;
				}
			}
		}
		//
		CMapObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			if(pFirst->GetHeight(intersect))
				bRet	=	TRUE;
			pFirst	=	pFirst->m_pNext;
		}
		return bRet;
	}
	Map3DQuadBoxScene::Map3DQuadBoxScene()
	{
		//m_tiles = new MapTile[MAP3D_REGION_GRIDs*MAP3D_REGION_GRIDs];
		//m_heights = new unsigned char[(MAP3D_REGION_GRIDs+1)*(MAP3D_REGION_GRIDs+1)];
	}
	Map3DQuadBoxScene::~Map3DQuadBoxScene()
	{
		//if(m_tiles)
		//	delete m_tiles;
		//if(m_heights)
		//	delete m_heights;
	}
	/*VOID		Map3DQuadBoxScene::SetTile(unsigned int x, unsigned int y, const MapTile& t)
	{
		if(x>MAP3D_REGION_GRIDs||y>MAP3D_REGION_GRIDs)return;
		m_tiles[x+y*MAP3D_REGION_GRIDs] = t;
	}
	VOID		Map3DQuadBoxScene::SetHeight(unsigned int x, unsigned int y, unsigned char height)
	{CPUTime(Map3DQuadBoxScene);
		if(x>(MAP3D_REGION_GRIDs+1)||y>(MAP3D_REGION_GRIDs+1))return;
		m_heights[x+y*(MAP3D_REGION_GRIDs+1)] = height;
		int heightScale = 10;
		float fH = (height-128)*heightScale;
		if(fH<this->m_regionBox._min.x)
			m_regionBox._min.x = fH;
		if(fH>m_regionBox._max.x)
			m_regionBox._max.x = fH;
	}*/
	VOID		Map3DQuadBoxScene::Render( Camera* c )
	{CPUTime(Map3DQuadBoxScene);
		if(this->m_mapPtr->m_bShowTerrain){
			PBoxVertices vb;
			vb.setup(m_regionBox._min, m_regionBox._max);
			if(c->IsAABoxVisibleLRNF(vb))
			{
				g_manager.SetWorldMatrix(&matrix4x4::s_identity());
				m_terrain.Render();
				/*int tileSize = m_mapPtr->m_tileSize;
				int heightScale = 10;
				int tileH = (MAP3D_REGION_GRIDs+1);
				int beginX = this->m_rect.left;
				int beginY = this->m_rect.top;
				for(int x=0;x<MAP3D_REGION_GRIDs;x++)
				{
				for(int y=0;y<MAP3D_REGION_GRIDs;y++)
				{
				MapTile& tile = this->m_tiles[x+y*MAP3D_REGION_GRIDs];
				if(tile.attr & TileProp_HIDE)
				continue;
				Texture* tex = m_mapPtr->GetTexturePtr(tile.texID);
				if(tex){
				float3 pos[4];
				pos[0].setxyz(beginX + x*tileSize, beginY + y*tileSize, ((int)m_heights[y*tileH+x]-128)*heightScale);
				pos[1].setxyz(beginX + x*tileSize, beginY + (y+1)*tileSize, ((int)m_heights[(y+1)*tileH+x]-128)*heightScale);
				pos[2].setxyz(beginX + (x+1)*tileSize, beginY + (y+1)*tileSize, ((int)m_heights[(y+1)*tileH+x+1]-128)*heightScale);
				pos[3].setxyz(beginX + (x+1)*tileSize, beginY + y*tileSize, ((int)m_heights[y*tileH+x+1]-128)*heightScale);
				float2 uv[4];
				if (tile.attr & TileProp_TEXINV)
				{
				if (tile.attr & TileProp_TEXROT0)
				{
				uv[0].setxy(1, 0);
				uv[1].setxy(1, 1);
				uv[2].setxy(0, 1);
				uv[3].setxy(0, 0);
				}
				else if (tile.attr & TileProp_TEXROT1)
				{
				uv[3].setxy(1, 0);
				uv[0].setxy(1, 1);
				uv[1].setxy(0, 1);
				uv[2].setxy(0, 0);
				}
				else if (tile.attr & TileProp_TEXROT2)
				{
				uv[2].setxy(1, 0);
				uv[3].setxy(1, 1);
				uv[0].setxy(0, 1);
				uv[1].setxy(0, 0);
				}
				else if (tile.attr & TileProp_TEXROT3)
				{
				uv[1].setxy(1, 0);
				uv[2].setxy(1, 1);
				uv[3].setxy(0, 1);
				uv[0].setxy(0, 0);
				}
				}
				else
				{
				if (tile.attr & TileProp_TEXROT0)
				{
				uv[0].setxy(0, 0);
				uv[1].setxy(0, 1);
				uv[2].setxy(1, 1);
				uv[3].setxy(1, 0);
				}
				else if (tile.attr & TileProp_TEXROT1)
				{
				uv[1].setxy(0, 0);
				uv[2].setxy(0, 1);
				uv[3].setxy(1, 1);
				uv[0].setxy(1, 0);
				}
				else if (tile.attr & TileProp_TEXROT2)
				{
				uv[2].setxy(0, 0);
				uv[3].setxy(0, 1);
				uv[0].setxy(1, 1);
				uv[1].setxy(1, 0);
				}
				else if (tile.attr & TileProp_TEXROT3)
				{
				uv[3].setxy(0, 0);
				uv[0].setxy(0, 1);
				uv[1].setxy(1, 1);
				uv[2].setxy(1, 0);
				}
				}
				unsigned int colors[4];
				for(int i=0;i<4;i++)
				colors[i] = 0xffffffff;
				g_manager.Draw3D(tex, pos, uv, colors, 0, Phantom::CullMode_CW);
				}
				}
				}*/
			}
		}
		Map3DQuadBox::Render(c);
	}
	Map3D::Map3D()
	{
		m_tiles.setGrow(0);
		m_heights.setGrow(0);
		m_objects.setGrow(0);
		m_triggers.setGrow(0);
		m_cameraPos.Set(0,0);
		m_bShowTerrain	=	true;
		m_bShowObject	=	true;
		m_bShowBlock	=	true;
		m_id	=	1;
		m_tileSize	=	100;
		m_mapSize.Set(1,1);
		m_pRegionPtr		=	0;
		m_regions			=	0;
		m_sceneRegions		=	0;
		m_nMaxRegionWidth = 0;
		this->m_fCameraLength	=	m_tileSize*10.0f;
	}
	Map3D::~Map3D()
	{
		Destroy();
	}

	VOID	Map3D::Save(const char* szFileName)
	{CPUTime(Map3D);
	}
	VOID	Map3D::Destroy()
	{CPUTime(Map3D);
		if(m_sceneRegions)
			delete[] m_sceneRegions;
		m_sceneRegions	=	0;
		if(m_regions)
			delete[] m_regions;
		m_regions		=	0;
		m_pRegionPtr	=	0;
		for(int i=0;i<m_objects.size();i++)
			delete m_objects[i];
		this->m_objects.clear();
		this->m_heights.clear();
		this->m_tiles.clear();
		for(int i=0;i<m_textureFiles.size();i++)
			safe_release(m_textureFiles[i].texture);
		for(int i=0;i<this->m_objectFiles.size();i++)
			safe_release(m_objectFiles[i].mesh);
		m_textureFiles.clear();
		m_objectFiles.clear();
		//m_fogDataPtr.ClearDatas();
		//m_cameraDataPtr.ClearDatas();
	}
	VOID	Map3D::SetCameraPos(const Pixel& p)
	{CPUTime(Map3D);
		m_cameraPos = p;
	}
	BOOL	Map3D::Load(const char* szFileName)
	{CPUTime(Map3D);
		Destroy();
		CFileHelperR r;
		m_mapFile = szFileName;
		m_mapFile.setIsFile();
		if(!r.open(m_mapFile.t))
			return false;
		int version = 1;
		char head[4] = {'m','a','3','d'};
		char headr[4];
		r.pStream->read(headr, sizeof(headr));
		for(int i=0;i<4;i++)
			if(head[i] != headr[i])
				return false;
		r >> version;
		if(version<=1)//载入早期版本
		{
			int sizew,sizeh,tilew;
			r >> m_id >> sizew >> sizeh >> tilew;
			m_mapSize.Set(sizew,sizeh);
			m_tileSize = tilew;
			m_tiles.SetArrayCount(sizew*sizeh);
			r.pStream->read(m_tiles.GetPtr(), sizeof(MapTile)*sizew*sizeh);
			m_heights.SetArrayCount((sizew+1)*(sizeh+1));
			r.pStream->read(m_heights.GetPtr(), sizeof(unsigned char)*(sizew+1)*(sizeh+1));
			//
			int numObjFile = 0;
			int numTextureFile = 0;
			r >> numObjFile >> numTextureFile;
			m_textureFiles.SetArrayCount(numTextureFile);
			for(int i=0;i<numTextureFile;i++)
				r >> m_textureFiles[i].file;
			m_objectFiles.SetArrayCount(numObjFile);
			for(int i=0;i<numObjFile;i++)
				r >> m_objectFiles[i].id >> m_objectFiles[i].file;
			int numObj = 0;
			r >> numObj;
			this->m_objects.SetArrayCount(numObj);
			for(int i=0;i<numObj;i++)
			{
				m_objects[i] = new CMapObject();
				MapObjectData m;
				r >> m;
				m.pos.y = m.pos.z;
				m.pos.z = 0;
				m_objects[i]->m_obj = m;
			}
			int numTrigger = 0;
			r >> numTrigger;
			this->m_triggers.SetArrayCount(numTrigger);
			for(int i=0;i<numTrigger;i++)
			{
				int l=0,t=0,r1=0,b=0;
				r >> l >> t >> r1 >> b;
				MapTrigger tr;
				tr.id = m_id++;
				tr.name.t[0] = 0;
				tr.rect.Set(l,t,r1,b);
				m_triggers[i] = tr;
			}
			char check[10];
			r >> numTrigger;
			r.pStream->read(check, sizeof(check));
		}
		else
		{
		}
		int nBoxLevel;
		int cx = m_mapSize.cx/MAP3D_REGION_GRIDs;
		int cy = m_mapSize.cy/MAP3D_REGION_GRIDs;
		m_nMaxRegionWidth	=	getmax(cx,cy);
		nBoxLevel			=	m_nMaxRegionWidth / 2;
		INT nRegionCount	=	0;
		while(nBoxLevel > 0)
		{
			nRegionCount	+=	nBoxLevel * nBoxLevel;
			nBoxLevel		=	nBoxLevel / 2;
		}
		m_sceneRegions		=	new Map3DQuadBoxScene[m_nMaxRegionWidth * m_nMaxRegionWidth];
		m_regions			=	new Map3DQuadBox[nRegionCount];
		for(int i=0;i<m_nMaxRegionWidth * m_nMaxRegionWidth;i++)
			m_sceneRegions[i].m_mapPtr = this;
		for(int i=0;i<nRegionCount;i++)
			m_regions[i].m_mapPtr = this;
		INT nIndex			=	0;
		m_pRegionPtr		=	&m_regions[nIndex++];
		//
		BuildRegions(m_pRegionPtr, NULL, nIndex, 1, 0, 0, 0);
		assert( nIndex == nRegionCount );
		for(int i=0;i<m_objects.size();i++)
		{
			m_objects[i]->m_mesh = GetObjectPtr(m_objects[i]->m_obj);
			m_objects[i]->onChange();
		}
		CDynamicArray<float3> normals;
		normals.SetArrayCount((m_mapSize.cx+1)*(m_mapSize.cy+1));
		int tileH = m_mapSize.cx+1;
		int	heightScale = 10;
		int tileSize = this->m_tileSize;
		for(int x=1;x<this->m_mapSize.cx;x++)
		{
			for(int y=1;y<this->m_mapSize.cy;y++)
			{
				float3 pos[5];
				pos[0].setxyz(x*tileSize, y*tileSize, ((int)m_heights[y*tileH+x]-128)*heightScale);
				pos[1].setxyz(x*tileSize, (y+1)*tileSize, ((int)m_heights[(y+1)*tileH+x]-128)*heightScale);
				pos[2].setxyz((x-1)*tileSize, y*tileSize, ((int)m_heights[y*tileH+x-1]-128)*heightScale);
				pos[3].setxyz(x*tileSize, (y-1)*tileSize, ((int)m_heights[(y-1)*tileH+x]-128)*heightScale);
				pos[4].setxyz((x+1)*tileSize, y*tileSize, ((int)m_heights[y*tileH+x+1]-128)*heightScale);
				float3 dir[4];
				for(int i=0;i<4;i++)
					dir[i] = pos[i+1] - pos[0];
				float3 c = dir[0].cross(dir[1]);
				c += dir[1].cross(dir[2]);
				c += dir[2].cross(dir[3]);
				c.normalize();
				normals[x+y*(m_mapSize.cx+1)] = float3(0,0,1);//c;
			}
		}
		for(int sx=0;sx<m_nMaxRegionWidth;sx++)
		{
			for(int sy=0;sy<m_nMaxRegionWidth;sy++)
			{
				Map3DQuadBoxScene& scene = m_sceneRegions[sx+sy*m_nMaxRegionWidth];
				scene.m_terrain.Create(Phantom::FVF_NORMAL|Phantom::FVF_TEX1|Phantom::FVF_XYZ|Phantom::FVF_DIFFUSE, 
					MAP3D_REGION_GRIDs*MAP3D_REGION_GRIDs*4);
				scene.m_terrain.SetNumTriangles(MAP3D_REGION_GRIDs*MAP3D_REGION_GRIDs*2);
				int index = 0;
				int tindex = 0;
				int tileSize = m_tileSize;
				int heightScale = 10;
				int tileH = (m_mapSize.cy+1);
				int beginX = sx*MAP3D_REGION_GRIDs;
				int beginY = sy*MAP3D_REGION_GRIDs;
				for(int tx=0;tx<MAP3D_REGION_GRIDs;tx++)
				{
					for(int ty=0;ty<MAP3D_REGION_GRIDs;ty++)
					{
						float3 pos[4];
						int x = beginX + tx;
						int y = beginY + ty;
						MapTile& tile = this->m_tiles[x+y*m_mapSize.cx];
						if(tile.attr & TileProp_HIDE)
							continue;
						Texture* tex = this->GetTexturePtr(tile.texID);
						if(tex){
							int attrID = scene.m_terrain.AddAttribute(tex);
							pos[0].setxyz(beginX*tileSize + tx*tileSize, beginY*tileSize + ty*tileSize, ((int)m_heights[y*tileH+x]-128)*heightScale);
							pos[1].setxyz(beginX*tileSize + tx*tileSize, beginY*tileSize + (ty+1)*tileSize, ((int)m_heights[(y+1)*tileH+x]-128)*heightScale);
							pos[2].setxyz(beginX*tileSize + (tx+1)*tileSize, beginY*tileSize + (ty+1)*tileSize, ((int)m_heights[(y+1)*tileH+x+1]-128)*heightScale);
							pos[3].setxyz(beginX*tileSize + (tx+1)*tileSize, beginY*tileSize + ty*tileSize, ((int)m_heights[y*tileH+x+1]-128)*heightScale);
							float2 uv[4];
							if (tile.attr & TileProp_TEXINV)
							{
								if (tile.attr & TileProp_TEXROT0)
								{
									uv[0].setxy(1, 0);
									uv[1].setxy(1, 1);
									uv[2].setxy(0, 1);
									uv[3].setxy(0, 0);
								}
								else if (tile.attr & TileProp_TEXROT1)
								{
									uv[3].setxy(1, 0);
									uv[0].setxy(1, 1);
									uv[1].setxy(0, 1);
									uv[2].setxy(0, 0);
								}
								else if (tile.attr & TileProp_TEXROT2)
								{
									uv[2].setxy(1, 0);
									uv[3].setxy(1, 1);
									uv[0].setxy(0, 1);
									uv[1].setxy(0, 0);
								}
								else if (tile.attr & TileProp_TEXROT3)
								{
									uv[1].setxy(1, 0);
									uv[2].setxy(1, 1);
									uv[3].setxy(0, 1);
									uv[0].setxy(0, 0);
								}
							}
							else
							{
								if (tile.attr & TileProp_TEXROT0)
								{
									uv[0].setxy(0, 0);
									uv[1].setxy(0, 1);
									uv[2].setxy(1, 1);
									uv[3].setxy(1, 0);
								}
								else if (tile.attr & TileProp_TEXROT1)
								{
									uv[1].setxy(0, 0);
									uv[2].setxy(0, 1);
									uv[3].setxy(1, 1);
									uv[0].setxy(1, 0);
								}
								else if (tile.attr & TileProp_TEXROT2)
								{
									uv[2].setxy(0, 0);
									uv[3].setxy(0, 1);
									uv[0].setxy(1, 1);
									uv[1].setxy(1, 0);
								}
								else if (tile.attr & TileProp_TEXROT3)
								{
									uv[3].setxy(0, 0);
									uv[0].setxy(0, 1);
									uv[1].setxy(1, 1);
									uv[2].setxy(1, 0);
								}
							}
							unsigned int colors[4];
							Triangle tr(index+3,index+1,index);
							scene.m_terrain.SetAttribute(tindex, attrID);
							scene.m_terrain.SetTriangle(tindex++, tr);
							tr.set(index+3,index+2,index+1);
							scene.m_terrain.SetAttribute(tindex, attrID);
							scene.m_terrain.SetTriangle(tindex++, tr);
							for(int i=0;i<4;i++)
							{
								scene.m_terrain.SetPos(index, pos[i]);
								scene.m_terrain.SetDiffuse(index, 0xffffffff);
								scene.m_terrain.SetUV(index, uv[i]);
								scene.m_terrain.SetNormal(index, normals[x+y*(m_mapSize.cx+1)]);
								index++;
							}
						}
					}
				}
				scene.m_terrain.RebuildAttributes();
			}
		}
		for(int i=0;i<m_objects.size();i++)
		{
			this->m_pRegionPtr->InsertObject(m_objects[i]);
		}
		return true;
	}
	Texture*						Map3D::GetTexturePtr(int id)
	{CPUTime(Map3D);
		if(id<0||id>=this->m_textureFiles.size())
			return 0;
		if(m_textureFiles[id].texture)
			return m_textureFiles[id].texture;
		if(m_textureFiles[id].bFail)
			return 0;
		FileName dir;
		m_mapFile.splitpath(&dir, 0, 0);
		dir += m_textureFiles[id].file.t;
		g_manager.LoadTexture(&m_textureFiles[id].texture, dir.t, true);
		return m_textureFiles[id].texture;
	}

	Mesh*							Map3D::LoadMesh(MapObjectData& obj)
	{CPUTime(Map3D);
		int index = -1;
		for(int i=0;i<m_objectFiles.size();i++)
		{
			if(m_objectFiles[i].id == obj.objID)
			{
				index = i;
				break;
			}
		}
		if(index<0)
			return 0;
		if(m_objectFiles[index].bFail)
			return 0;
		AutoPtr<Mesh> temp;
		FileName dir;
		m_mapFile.splitpath(&dir, 0, 0);
		dir += m_objectFiles[index].file.t;
		if(!g_manager.LoadMesh(temp, dir))
		{
			m_objectFiles[index].bFail = true;
			return 0;
		}
		temp->SetPosition(obj.pos);
		Mesh* mesh = temp;
		safe_addref(mesh);
		return mesh;
	}
	Mesh*							Map3D::GetObjectPtr(MapObjectData& obj)
	{CPUTime(Map3D);
		int index = -1;
		for(int i=0;i<m_objectFiles.size();i++)
		{
			if(m_objectFiles[i].id == obj.objID)
			{
				index = i;
				break;
			}
		}
		if(index<0)
		{
			return 0;
		}
		if(m_objectFiles[index].mesh)
			return m_objectFiles[index].mesh;
		if(m_objectFiles[index].bFail)
			return 0;
		AutoPtr<Mesh> temp;
		FileName dir;
		m_mapFile.splitpath(&dir, 0, 0);
		dir += m_objectFiles[index].file.t;
		if(!g_manager.LoadMesh(temp, dir))
		{
			m_objectFiles[index].bFail = true;
			return 0;
		}
		temp->SetPosition(obj.pos);
		m_objectFiles[index].mesh = temp;
		safe_addref(m_objectFiles[index].mesh);
		return m_objectFiles[index].mesh;
	}
	CMapObject*						Map3D::Intersect(IntersectData* intersect, BOOL bCollisionOnly)
	{CPUTime(Map3D);
		if(this->m_pRegionPtr)
			return m_pRegionPtr->Intersect(intersect, bCollisionOnly);
		return 0;
	}
	VOID							Map3D::OnFrameMove()
	{CPUTime(Map3D);
	}
	VOID							Map3D::Render()
	{CPUTime(Map3D);
		float3 t(this->m_cameraPos.x, this->m_cameraPos.y, 0);
		float3 e(this->m_cameraPos.x, this->m_cameraPos.y-10,m_fCameraLength);
		float3 dir = GetCameraPtr()->GetEye()-GetCameraPtr()->GetTarget();
		GetCameraPtr()->SetTarget(t);
		GetCameraPtr()->SetEye(t+dir);
		GetCameraPtr()->SetMinWheelDistance(100.0f);
		GetCameraPtr()->SetMaxWheelDistance(100000.0f);
		GetCameraPtr()->SetDistance(m_fCameraLength);
		if(this->m_pRegionPtr)
			m_pRegionPtr->RenderAllLevels(GetCameraPtr());
		if(m_bShowBlock){
			g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
			int tileH = m_mapSize.cx+1;
			int	heightScale = 10;
			for(int x=0;x<this->m_mapSize.cx;x++)
			{
				for(int y=0;y<this->m_mapSize.cy;y++)
				{
					MapTile& tile = this->m_tiles[x+y*m_mapSize.cx];
					if(tile.attr & TileProp_HIDE)
						continue;
						float3 pos[4];
						pos[0].setxyz(x*m_tileSize, y*m_tileSize, ((int)m_heights[y*tileH+x]-128)*heightScale);
						pos[1].setxyz(x*m_tileSize, (y+1)*m_tileSize, ((int)m_heights[(y+1)*tileH+x]-128)*heightScale);
						pos[2].setxyz((x+1)*m_tileSize, (y+1)*m_tileSize, ((int)m_heights[(y+1)*tileH+x+1]-128)*heightScale);
						pos[3].setxyz((x+1)*m_tileSize, y*m_tileSize, ((int)m_heights[y*tileH+x+1]-128)*heightScale);
						unsigned int colors[4];
						for(int i=0;i<4;i++)
						{
							if(tile.attr&TileProp_BLOCK)
								colors[i] = 0x8fff0000;
							else
								colors[i] = 0x8f00ff00;
						}
						g_manager.Draw3D(0, pos, 0, colors, 0, Phantom::CullMode_CW);
				}
			}
		}
		/*
		if(m_bShowTerrain)
		{
			int tileH = m_mapSize.cx+1;
			int	heightScale = 10;
			for(int x=0;x<this->m_mapSize.cx;x++)
			{
				for(int y=0;y<this->m_mapSize.cy;y++)
				{
					MapTile& tile = this->m_tiles[x+y*m_mapSize.cx];
					if(tile.attr & TileProp_HIDE)
						continue;
					Texture* tex = GetTexturePtr(tile.texID);
					if(tex){
						float3 pos[4];
						pos[0].setxyz(x*m_tileSize, y*m_tileSize, ((int)m_heights[y*tileH+x]-128)*heightScale);
						pos[1].setxyz(x*m_tileSize, (y+1)*m_tileSize, ((int)m_heights[(y+1)*tileH+x]-128)*heightScale);
						pos[2].setxyz((x+1)*m_tileSize, (y+1)*m_tileSize, ((int)m_heights[(y+1)*tileH+x+1]-128)*heightScale);
						pos[3].setxyz((x+1)*m_tileSize, y*m_tileSize, ((int)m_heights[y*tileH+x+1]-128)*heightScale);
						float2 uv[4];
						if (tile.attr & TileProp_TEXINV)
						{
							if (tile.attr & TileProp_TEXROT0)
							{
								uv[0].setxy(1, 0);
								uv[1].setxy(1, 1);
								uv[2].setxy(0, 1);
								uv[3].setxy(0, 0);
							}
							else if (tile.attr & TileProp_TEXROT1)
							{
								uv[3].setxy(1, 0);
								uv[0].setxy(1, 1);
								uv[1].setxy(0, 1);
								uv[2].setxy(0, 0);
							}
							else if (tile.attr & TileProp_TEXROT2)
							{
								uv[2].setxy(1, 0);
								uv[3].setxy(1, 1);
								uv[0].setxy(0, 1);
								uv[1].setxy(0, 0);
							}
							else if (tile.attr & TileProp_TEXROT3)
							{
								uv[1].setxy(1, 0);
								uv[2].setxy(1, 1);
								uv[3].setxy(0, 1);
								uv[0].setxy(0, 0);
							}
						}
						else
						{
							if (tile.attr & TileProp_TEXROT0)
							{
								uv[0].setxy(0, 0);
								uv[1].setxy(0, 1);
								uv[2].setxy(1, 1);
								uv[3].setxy(1, 0);
							}
							else if (tile.attr & TileProp_TEXROT1)
							{
								uv[1].setxy(0, 0);
								uv[2].setxy(0, 1);
								uv[3].setxy(1, 1);
								uv[0].setxy(1, 0);
							}
							else if (tile.attr & TileProp_TEXROT2)
							{
								uv[2].setxy(0, 0);
								uv[3].setxy(0, 1);
								uv[0].setxy(1, 1);
								uv[1].setxy(1, 0);
							}
							else if (tile.attr & TileProp_TEXROT3)
							{
								uv[3].setxy(0, 0);
								uv[0].setxy(0, 1);
								uv[1].setxy(1, 1);
								uv[2].setxy(1, 0);
							}
						}
						unsigned int colors[4];
						for(int i=0;i<4;i++)
							colors[i] = 0xffffffff;
						g_manager.Draw3D(tex, pos, uv, colors, 0, Phantom::CullMode_Double);
					}
				}
			}
		}
		if(m_bShowObject)
		{
			for(int i=0;i<m_objects.size();i++)
			{
				m_objects[i]->Render();
			}
		}*/
	}

	Map3DQuadBox*	Map3D::GetSceneRegion(INT xRegion, INT yRegion)	//返回地形区域
	{CPUTime(Map3D);
		if(xRegion < 0 || yRegion < 0 || xRegion >= m_nMaxRegionWidth || yRegion >= m_nMaxRegionWidth)
			return NULL;
		return &m_sceneRegions[xRegion + yRegion * m_nMaxRegionWidth];
	}

	VOID	Map3D::BuildRegions(Map3DQuadBox* pRegion, Map3DQuadBox* pRegionParent, INT& nIndex, INT nLevel, INT nBeginX, INT nBeginY, INT nLevelIndex)
	{CPUTime(Map3D);
		INT nWidth		=	m_nMaxRegionWidth / nLevel;
		pRegion->Setup(pRegionParent, nLevel, nBeginX, nBeginY, nWidth, m_tileSize * (float)MAP3D_REGION_GRIDs, nLevelIndex);
		//
		nLevel				=	nLevel * 2;
		if(nLevel >= m_nMaxRegionWidth)
		{
			assert(nWidth == 2);
			pRegion->m_childRegion[0]	=	GetSceneRegion(nBeginX, nBeginY);
			pRegion->m_childRegion[1]	=	GetSceneRegion(nBeginX + 1, nBeginY);
			pRegion->m_childRegion[2]	=	GetSceneRegion(nBeginX, nBeginY + 1);
			pRegion->m_childRegion[3]	=	GetSceneRegion(nBeginX + 1, nBeginY + 1);
			//
			pRegion->m_childRegion[0]->Setup( pRegion, nLevel, nBeginX, nBeginY, nWidth / 2, this->m_tileSize * (float)MAP3D_REGION_GRIDs, nLevelIndex + 1); 
			pRegion->m_childRegion[1]->Setup( pRegion, nLevel, nBeginX + 1, nBeginY, nWidth / 2, this->m_tileSize * (float)MAP3D_REGION_GRIDs, nLevelIndex + 1); 
			pRegion->m_childRegion[2]->Setup( pRegion, nLevel, nBeginX, nBeginY + 1, nWidth / 2, this->m_tileSize * (float)MAP3D_REGION_GRIDs, nLevelIndex + 1); 
			pRegion->m_childRegion[3]->Setup( pRegion, nLevel, nBeginX + 1, nBeginY + 1, nWidth / 2, this->m_tileSize * (float)MAP3D_REGION_GRIDs, nLevelIndex + 1); 
			//定位
			return;
		}
		for(int i=0;i<QUAD_BOX_COUNT;i++)
			pRegion->m_childRegion[i]	=	&m_regions[nIndex++];
		//
		BuildRegions(pRegion->m_childRegion[0], pRegion, nIndex, nLevel, nBeginX, nBeginY, nLevelIndex + 1);
		BuildRegions(pRegion->m_childRegion[1], pRegion, nIndex, nLevel, nBeginX + nWidth / 2, nBeginY, nLevelIndex + 1);
		BuildRegions(pRegion->m_childRegion[2], pRegion, nIndex, nLevel, nBeginX, nBeginY + nWidth / 2, nLevelIndex + 1);
		BuildRegions(pRegion->m_childRegion[3], pRegion, nIndex, nLevel, nBeginX + nWidth / 2, nBeginY + nWidth / 2, nLevelIndex + 1);
	}
};
