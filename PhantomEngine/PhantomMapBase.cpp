#include "PhantomMapBase.h"

namespace Phantom{
	VOID		MapGrid::AddUnit(MapItem* item, int layer)
	{CPUTime(MapGrid);
		MapItem* first = units[layer];
		units[layer] = item;
		assert(item->m_next == 0);
		item->m_next = first;
		safe_addref(item);
	}
	BOOL		MapGrid::RemoveUnit(MapItem* item, int layer)
	{CPUTime(MapGrid);
		MapItem* first = units[layer];
		MapItem* last = 0;
		while(first)
		{
			if(first == item)
			{
				if(first == units[layer])
					units[layer] = first->m_next;
				else
					last->m_next = first->m_next;
				first->m_next = 0;
				safe_release(first);
				return true;
			}
			last = first;
			first = first->m_next;
		}
		return false;
	}
	MapItem::MapItem()
	{
		m_inMap.Set(-1,-1);
		m_gridCount = 0;
		m_color.setRGBA(1,1,1,1);
		m_itemRect.Set(0, 0, 0, 0);
		m_bEnabled = true;
		m_bVisible = true;
		m_alignBottom=0;
		m_next = 0;
		this->m_bDoorExist = false;
		this->m_door.Set(0,0);
	}
	MapBase::MapBase()
	{
		m_mapPixelSize.Set(0,0);
		m_tileCount.Set(0,0);
		m_cameraPos.Set(0,0);
		m_autoid = 1;
	}
	MapBase::~MapBase()
	{
		Destroy();
	}
	VOID			MapBase::SetTileCount(const Size& size)//设置格子数量
	{CPUTime(MapBase);
		if(size.cx < 2 || size.cy < 2)
			return;
		Size oldTile = m_tileCount;
		m_tileCount = size;
		CDynamicArray<MapGrid> oldMaps = m_mapGrids;
		m_mapGrids.clear();
		m_mapGrids.SetArrayCount(m_tileCount.cx * m_tileCount.cy);
		for(int i=0;i<m_mapGrids.size();i++)
		{
			m_mapGrids[i].pathIndex = -1;
			m_mapGrids[i].moveEnable = -1;
		}
		for(int x=0;x<m_tileCount.cx;x++)
		{
			for(int y=0;y<m_tileCount.cy;y++)
			{
				if(x<oldTile.cx && y<oldTile.cy)
				{
					MapGrid& g = m_mapGrids[x+y*m_tileCount.cx];
					g = oldMaps[x + y * oldTile.cx];
					for(int i=0;i<MAX_GRID_LAYER;i++)
					{
						if(g.item[i])
							g.item[i]->m_inMap.Set(x, y);
						safe_addref(g.item[i]);
					}
					//for(int i=0;i<MAX_GRID_TEXTURE_LAYER;i++)
						//ReleaseTexture(g.tex[i].texID);
				}
			}
		}
		for(int x=0;x<oldTile.cx;x++)
		{
			for(int y=0;y<oldTile.cy;y++)
			{
				MapGrid& g = oldMaps[x + y * oldTile.cx];
				for(int i=0;i<MAX_GRID_LAYER;i++)
					safe_release(g.item[i]);
				//for(int i=0;i<MAX_GRID_TEXTURE_LAYER;i++)
					//ReleaseTexture(g.tex[i].texID);
			}
		}
		OnCreateMap();
	}
	PtrID			MapBase::GetTexture(const char* szFileName)
	{CPUTime(MapBase);
		FileName f = szFileName;
		f.setIsFile();
		for(int i=0;i<m_textures.size();i++)
		{
			if(m_textures[i].file == f)
			{
				return PtrID(i, m_textures[i].id);
			}
		}
		MapTextureData t;
		t.file = f;
		t.id = GetNewID();
		t.tex = 0;
		m_textures.push_back(t);
		return PtrID(m_textures.size()-1, t.id);
	}
	Texture*		MapBase::GetTexturePtr(PtrID& id)
	{CPUTime(MapBase);
		if(id.id == 0)
			return 0;
		if(id.index < m_textures.size() && m_textures[id.index].id == id.id)
			return m_textures[id.index].tex;
		int index = -1;
		for(int i=0;i<m_textures.size();i++)
		{
			if(m_textures[i].id == id.id)
			{
				id.index = i;
				index = i;
				id.id = m_textures[i].id;
				break;
			}
		}
		if(index < 0)
			return 0;
		if(m_textures[index].tex)
			return m_textures[index].tex;
		if(g_manager.LoadTexture(&m_textures[index].tex, m_textures[index].file))
		{
			return m_textures[index].tex;
		}
		return 0;
	}
	VOID			MapBase::SetOffsetAll(int xx, int yy)//全部整体偏移
	{CPUTime(MapBase);
		Size oldTile = m_tileCount;
		CDynamicArray<MapGrid> oldMaps = m_mapGrids;
		m_mapGrids.clear();
		m_mapGrids.SetArrayCount(m_tileCount.cx * m_tileCount.cy);
		for(int i=0;i<m_mapGrids.size();i++)
		{
			m_mapGrids[i].pathIndex = -1;
			m_mapGrids[i].moveEnable = -1;
		}
		for(int x=0;x<m_tileCount.cx;x++)
		{
			for(int y=0;y<m_tileCount.cy;y++)
			{
				int newx = x+xx;
				int newy = y+yy;
				if(newx < 0 || newy < 0 || newx >= m_tileCount.cx || newy >= m_tileCount.cy)
					continue;
				MapGrid& g = m_mapGrids[newx+newy*m_tileCount.cx];
				g = oldMaps[x + y * oldTile.cx];
				for(int i=0;i<MAX_GRID_LAYER;i++)
				{
					if(g.item[i])
						g.item[i]->m_inMap.Set(newx, newy);
					safe_addref(g.item[i]);
				}
			}
		}
		for(int x=0;x<m_tileCount.cx;x++)
		{
			for(int y=0;y<m_tileCount.cy;y++)
			{
				MapGrid& g = oldMaps[x + y * m_tileCount.cx];
				for(int i=0;i<MAX_GRID_LAYER;i++)
					safe_release(g.item[i]);
			}
		}
	}
	VOID			MapBase::SetMapSize(int width, int height)
	{CPUTime(MapBase);
		m_mapPixelSize.Set(width, height);
	}
	VOID			MapBase::Create(Size& mapSize/*地图像素大小*/, Size& tiledCount/*地图叠加数量*/)
	{CPUTime(MapBase);
		m_mapPixelSize = mapSize;
		m_tileCount = tiledCount;
		m_mapGrids.SetArrayCount(m_tileCount.cx * m_tileCount.cy);
		for(int i=0;i<m_mapGrids.size();i++)
		{
			m_mapGrids[i].pathIndex = -1;
			m_mapGrids[i].moveEnable = -1;
		}
		m_cameraPos.Set(m_mapPixelSize.cx / 2, m_mapPixelSize.cy / 2);
		//m_mapBoxTextures.SetArrayCount(GetBoxTile().cx * GetBoxTile().cy);
		OnCreateMap();
	}
	VOID			MapBase::Destroy()
	{CPUTime(MapBase);
		for(int i=0;i<m_textures.size();i++)
		{
			safe_release(m_textures[i].tex);
		}
		m_textures.clear();
		for(int i=0;i<m_mapGrids.size();i++)
		{
			for(int j=0;j<MAX_GRID_LAYER;j++)
				if(m_mapGrids[i].item[j])
				{
					m_mapGrids[i].item[j]->m_inMap.Set(-1, -1);
					safe_release(m_mapGrids[i].item[j]);
				}
		}
		m_mapGrids.clear();
	}
	BOOL			MapBase::SetUnit(int nGridX, int nGridY, MapItem* item, int nLayer)
	{CPUTime(MapBase);
		safe_addref(item);
		if(item)
		{
			GridIndex& p = item->m_inMap;
			if(p.x >= 0 && p.y >= 0 && p.x < m_tileCount.cx && p.y < m_tileCount.cy)
			{
				int nIndex = p.x + p.y * m_tileCount.cx;
				this->m_mapGrids[nIndex].RemoveUnit(item, nLayer);
				item->m_inMap.Set(-1,-1);
			}
		}
		if(nGridX >= 0 && nGridY >= 0 && nGridX < m_tileCount.cx && nGridY < m_tileCount.cy)
		{
			int nIndex = nGridX + nGridY * m_tileCount.cx;
			m_mapGrids[nIndex].AddUnit(item, nLayer);
			if(item)
				item->m_inMap.Set(nGridX, nGridY);
		}
		safe_release(item);
		return true;
	}
	VOID			MapBase::RemoveUnit(MapItem* item)
	{CPUTime(MapBase);
		if(item)
		{
			GridIndex& p = item->m_inMap;
			if(p.x >= 0 && p.y >= 0 && p.x < m_tileCount.cx && p.y < m_tileCount.cy)
			{
				int nIndex = p.x + p.y * m_tileCount.cx;
				for(int j=0;j<MAX_GRID_LAYER;j++)
					if(this->m_mapGrids[nIndex].RemoveUnit(item, j))
					{
						item->m_inMap.Set(-1,-1);
						break;
					}
			}
		}
	}
	BOOL			MapBase::SetGridItem(int nGridX, int nGridY, MapItem* item, int nLayer)
	{CPUTime(MapBase);
		if(item)
		{
			GridIndex& p = item->m_inMap;
			if(p.x >= 0 && p.y >= 0 && p.x < m_tileCount.cx && p.y < m_tileCount.cy)
			{
				int nIndex = p.x + p.y * m_tileCount.cx;
				for(int j=0;j<MAX_GRID_LAYER;j++)
					if(this->m_mapGrids[nIndex].item[j] == item)
					{
						if(nLayer == j && p.x == nGridX && p.y == nGridY)//如果新的位置和旧位置一样则不变
							return true;
						item->m_inMap.Set(-1,-1);
						safe_release(this->m_mapGrids[nIndex].item[j]);
						break;
					}
				
			}
		}
		if(nGridX >= 0 && nGridY >= 0 && nGridX < m_tileCount.cx && nGridY < m_tileCount.cy)
		{
			if(nLayer == 0)
			{
				if(item && !IsItemCanInsert(nGridX,nGridY, item))
					return false;
			}
			int nIndex = nGridX + nGridY * m_tileCount.cx;
			if(m_mapGrids[nIndex].item[nLayer])
			{
				m_mapGrids[nIndex].item[nLayer]->m_inMap.Set(-1,-1);
				safe_release(m_mapGrids[nIndex].item[nLayer]);
			}
			m_mapGrids[nIndex].item[nLayer] = item;
			safe_addref(m_mapGrids[nIndex].item[nLayer]);
			if(item)
				item->m_inMap.Set(nGridX, nGridY);
		}
		return true;
	}
	VOID			MapBase::RemoveGridItem(MapItem* item)
	{CPUTime(MapBase);
		if(item)
		{
			GridIndex& p = item->m_inMap;
			if(p.x >= 0 && p.y >= 0 && p.x < m_tileCount.cx && p.y < m_tileCount.cy)
			{
				int nIndex = p.x + p.y * m_tileCount.cx;
				for(int j=0;j<MAX_GRID_LAYER;j++)
					if(this->m_mapGrids[nIndex].item[j] == item)
					{
						safe_release(this->m_mapGrids[nIndex].item[j]);
						item->m_inMap.Set(-1,-1);
						break;
					}
				
			}
		}
	}
	MapItem*	MapBase::GetGridItem(int nGridX, int nGridY, int nLayer)
	{CPUTime(MapBase);
		if(nGridX >= 0 && nGridY >= 0 && nGridX < m_tileCount.cx && nGridY < m_tileCount.cy)
		{
			int nIndex = nGridX + nGridY * m_tileCount.cx;
			return m_mapGrids[nIndex].item[nLayer];
		}
		return 0;
	}
/*
	VOID			MapBase::SetBoxTexture(int nBoxX, int nBoxY, const MapGrid_2x1* m)
	{CPUTime(MapBase);
		if(m){
			SetBoxTexture(nBoxX, nBoxY, m->id, &GetTexture(m->textureFile.t), &m->textureRect, m->offsetX, m->layer, m->offsetY, m->drawMode);
		}
		else
		{
			SetBoxTexture(nBoxX, nBoxY, 0, 0, 0, 0);
		}
	}
	VOID			MapBase::SetBoxTexture(int nBoxX, int nBoxY, int id, const PtrID* texID, const Rect* textureRect, int scale, int layer, int scaleY, int drawMode)
	{CPUTime(MapBase);
		if(layer < 0 || layer >= MAX_GRID_TEXTURE_LAYER)
			return;
		if(nBoxX >= 0 && nBoxY >= 0 && nBoxX < GetBoxTile().cx && nBoxY < GetBoxTile().cy)
		{
			int nIndex = nBoxX + nBoxY * GetBoxTile().cx;
			MapBoxTexture::MapGridTexture& tex = m_mapBoxTextures[nIndex].tex[layer];
			tex.id = id;
			tex.offsetX = scale;
			tex.offsetY = scaleY;
			tex.drawMode = drawMode;
			if(texID)
				tex.texID = *texID;
			else
				tex.texID.id = tex.texID.index = -1;
			if(textureRect)
				tex.textureRc = *textureRect;
			tex.textureRc.Offset(scale, scaleY);
		}
	}*/
	VOID			MapBase::SetGridTexture(int nGridX, int nGridY, const MapGrid_2x1* m)
	{CPUTime(MapBase);
		if(m){
			PtrID pid;
			if(m->textureFile.t[0])
				pid = GetTexture(m->textureFile.t);
			else
				pid.id=pid.index=0;
			
			SetGridTexture(nGridX, nGridY, m->id, &pid, &m->textureRect, m->offsetX, m->layer, m->offsetY, m->drawMode);
		}
		else
		{
			SetGridTexture(nGridX, nGridY, 0, 0, 0, 0);
		}
	}
	VOID			MapBase::SetGridTexture(int nGridX, int nGridY, int id, const PtrID* texID, const Rect* textureRect, int scale, int layer, int scaleY, int drawMode)
	{CPUTime(MapBase);
		if(layer < 0 || layer >= MAX_GRID_TEXTURE_LAYER)
			return;
		if(nGridX >= 0 && nGridY >= 0 && nGridX < m_tileCount.cx && nGridY < m_tileCount.cy)
		{
			int nIndex = nGridX + nGridY * m_tileCount.cx;
			MapGrid::MapGridTexture& tex = m_mapGrids[nIndex].tex[layer];
			tex.id = id;
			tex.offsetX = scale;
			tex.offsetY = scaleY;
			tex.drawMode = drawMode;
			if(texID)
				tex.texID = *texID;
			else
				tex.texID.id = tex.texID.index = 0;
			if(textureRect)
				tex.textureRc = *textureRect;
			tex.textureRc.Offset(scale, scaleY);
		}
	}
	VOID			MapBase::SetGridData(int nGridX, int nGridY, int data)
	{CPUTime(MapBase);
		if(nGridX >= 0 && nGridY >= 0 && nGridX < m_tileCount.cx && nGridY < m_tileCount.cy)
		{
			int nIndex = nGridX + nGridY * m_tileCount.cx;
			m_mapGrids[nIndex].data = data;
		}
	}
	INT				MapBase::GetGridData(int nGridX, int nGridY)
	{CPUTime(MapBase);
		if(nGridX >= 0 && nGridY >= 0 && nGridX < m_tileCount.cx && nGridY < m_tileCount.cy)
		{
			int nIndex = nGridX + nGridY * m_tileCount.cx;
			return m_mapGrids[nIndex].data;
		}
		return 0;
	}
	VOID			MapBase::SetCameraPos(const Pixel& center)//设置摄像头
	{CPUTime(MapBase);
		this->m_cameraPos = center;
		int sw = g_manager.GetUIDialogSizeW() / 2;
		int sh = g_manager.GetUIDialogSizeH() / 2;
		if(m_cameraPos.x < sw)
			m_cameraPos.x = sw;
		if(m_cameraPos.y < sh)
			m_cameraPos.y = sh;
		int mw = m_mapPixelSize.cx - sw;
		int mh = m_mapPixelSize.cy - sh;
		if(m_cameraPos.x > mw)
			m_cameraPos.x = mw;
		if(m_cameraPos.y > mh)
			m_cameraPos.y = mh;
	}
	VOID			MapBase::Save(const char* szFileName)
	{CPUTime(MapBase);
		CSafeFileHelperW w;
		if(!w.create(szFileName))
			return;
		m_fileName = szFileName;
		m_fileName.setIsFile();
		SaveW(w);
	}
	BOOL			MapBase::Load(const char* szFileName)
	{CPUTime(MapBase);
		CSafeFileHelperR r;
		if(!r.open(szFileName))
			return false;
		m_fileName = szFileName;
		m_fileName.setIsFile();
		LogInfo("system->mapload %s", this->m_fileName.t);
		return LoadR(r);
	}
	BOOL			MapBase::LoadR(CSafeFileHelperR& r)
	{CPUTime(MapBase);
		Destroy();
		char head[4] = {'m','2','x','1'};
		char headnew[4];
		int nVersion = 1;
		r.pStream->read(headnew, sizeof(headnew));
		if(head[0] != headnew[0] || head[1] != headnew[1] || head[2] != headnew[2] || head[3] != headnew[3])
			return false;
		r >> nVersion;
		r >> m_tileCount >> m_mapPixelSize >> m_cameraPos;
		if(nVersion > 1)
			r >> m_autoid;
		int texture = 0;
		r >> texture;
		if(texture > 0)
			m_textures.SetArrayCount(texture);
		for(int i=0;i<texture;i++)
		{
			r >> m_textures[i].id >> m_textures[i].file;
		}
		int count = 0;
		r >> count;
		m_mapGrids.SetArrayCount(count);
		for(int i=0;i<count;i++)
		{
			MapGrid& g = m_mapGrids[i];
			g.moveEnable = -1;
			g.pathIndex = -1;
			r >> g.data;
			for(int k=0;k<MAX_GRID_TEXTURE_LAYER;k++)
			{
				MapGrid::MapGridTexture& tex = g.tex[k];
				int len = 0;
				r >> len;
				r.pStream->read(&tex, len);
			}
			if(nVersion > 3)
				r >> g.moveEnable;
			for(int j=0;j<MAX_GRID_LAYER;j++)
			{
				int nItem1;
				r >> nItem1;
				if(nItem1 > 0){
					g.item[j] = CreateMapItem(nItem1);
					r >> g.item[j]->m_inMap >> g.item[j]->m_gridCount >> g.item[j]->m_color;
					if(nVersion > 2)
						r >> g.item[j]->m_alignBottom;
					g.item[j]->Load(r);
				}
			}
		}
		return true;
	}
	VOID			MapBase::SaveW(CSafeFileHelperW& w)
	{CPUTime(MapBase);
		char head[4] = {'m','2','x','1'};
		int nVersion = 4;
		w.pStream->write(head, sizeof(head));
		w << nVersion;
		w << m_tileCount << m_mapPixelSize << m_cameraPos;
		w << m_autoid;
		int texture = this->m_textures.size();
		w << texture;
		for(int i=0;i<texture;i++)
		{
			w << m_textures[i].id << m_textures[i].file;
		}
		int count = m_mapGrids.size();
		w << count;
		for(int i=0;i<count;i++)
		{
			MapGrid& g = m_mapGrids[i];
			w << g.data;
			for(int k=0;k<MAX_GRID_TEXTURE_LAYER;k++)
			{
				MapGrid::MapGridTexture& tex = g.tex[k];
				int len = sizeof(MapGrid::MapGridTexture);
				w << len;
				w.pStream->write(&tex, len);
			}
			w << g.moveEnable;
			for(int j=0;j<MAX_GRID_LAYER;j++)
			{
				int nItem1 = (g.item[j]?g.item[j]->GetItemType():0);
				w << nItem1;
				if(g.item[j])
				{
					w << g.item[j]->m_inMap << g.item[j]->m_gridCount << g.item[j]->m_color;
					w << g.item[j]->m_alignBottom;
					g.item[j]->Save(w);
				}
			}
		}
	}

	int		MapBase::GetGridTextureID(int nGridX, int nGridY, int layer)
	{CPUTime(MapBase);
		if(layer < 0 || layer >= MAX_GRID_TEXTURE_LAYER)
			return 0;
		if(nGridX >= 0 && nGridY >= 0 && nGridX < m_tileCount.cx && nGridY < m_tileCount.cy)
		{
			int nIndex = nGridX + nGridY * m_tileCount.cx;
			return m_mapGrids[nIndex].tex[layer].id;
		}
		return 0;
	}







};
