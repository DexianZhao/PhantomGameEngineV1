#include "PhantomMap2D.h"
#include "LuaManager.h"
#include "APILua.h"

namespace Phantom{
	MapItem_Unit::MapItem_Unit(Map2D_2x1* m2, MapUnit_Data* m)
		:MapItem_2x1(m2, false)
	{
		m_downScale = 105;
		m_mapPtr = m2;
		m_projid = m->id;
		m_centerPos.Set(0, 0);	//中心点
		m_textureRc.Set(0, 0, 0, 0);
		m_scale			=	100;
		m_texID.id = m_texID.index = 0;
		m_textureRc.Set(0, 0, 0, 0);
		m_centerPos.Set(0,0);
		m_scale = 0;
		m_offset.Set(0, 0);
		m_gridCount = 0;
		m_pos.Set(0, 0);
		m_bRenderMode	=	0;
		m_type	=	0;
		m_bPush	=	false;
		m_runtimeid = m_mapPtr->GetNewID();
		m_currentAction	=	0;
		m_currentAni	=	0;
		m_fSpeed		=	1;
		m_unitPtr		=	m;
		m_currentAngle	=	0;
		m_currentDir.Set(1,1);
		this->m_name = m->name;
		m_nPlayIndex = 0;
		this->m_fPlayLength = 0;
		m_last.Set(-1,-1);
		m_fActionSpeed	=	1;
	}
	MapItem_Unit::~MapItem_Unit()
	{
	}
	VOID					MapItem_Unit::SetActionByIndex(int index)
	{CPUTime(MapItem_Unit);
		if(index<0||index>=m_unitPtr->actions.size())return;
		if(m_unitPtr->actions[index].texid.id == 0)
		{
			if(m_unitPtr->actions[index].textureFile.t[0])
			{
				m_texID = m_unitPtr->actions[index].texid = this->m_mapPtr->GetTexture(m_unitPtr->actions[index].textureFile);
			}
		}
		else
			m_texID = m_unitPtr->actions[index].texid;
	}
	VOID					MapItem_Unit::SetActionByName(const char* action)
	{CPUTime(MapItem_Unit);
		if(!this->m_unitPtr)return;
		m_texID.id = this->m_texID.index = 0;
		for(int i=0;i<this->m_unitPtr->actions.size();i++)
		{
			if(this->m_unitPtr->actions[i].name == action)
			{
				SetActionByIndex(i);
				return;
			}
		}
	}
	VOID					MapItem_Unit::SetAngleDir(const Pixel& dir)
	{CPUTime(MapItem_Unit);
		if(dir.x==0&&dir.y==0)return;
		float3 v(dir.x,dir.y,0);
		SetAngle(Degress(getRotateZ(v)));
	}
	BOOL					MapItem_Unit::AddMoveTo(int mapx, int mapy, LPCSTR szFunc)//自动寻路，到地图目标点，带参数
	{CPUTime(MapItem_Unit);
		if(szFunc&&szFunc[0])
			this->m_moveEndFunc = szFunc;
		else
			this->m_moveEndFunc.t[0] = 0;
		return AddMoveTo(mapx,mapy);
	}
	BOOL					MapItem_Unit::AddMoveTo(int mapx, int mapy)//自动寻路，到地图目标点
	{CPUTime(MapItem_Unit);
		Pixel dest(mapx, mapy);
		m_ways.SetArrayCount(0);
		m_nPlayIndex = 0;
		m_fPlayLength = 0;
		if(m_mapPtr->FindPath(this->m_ways, this->m_pos, dest))
		{
			for(int i=0;i<m_ways.size()-1;i++)
			{
				m_ways[i].dir = m_ways[i+1].pos - m_ways[i].pos;
				m_ways[i].length = m_ways[i].dir.length();
				if(m_ways[i].length > 0.01f)
				{
					float3 v(m_ways[i].dir.x,m_ways[i].dir.y,0);
					m_ways[i].fAngle	=	Degress(getRotateZ(v));
				}
				m_ways[i].dir.normalize();
			}
			if(m_unitPtr->action_move < 0 || m_unitPtr->action_move>=m_unitPtr->actions.size())
			{
				m_unitPtr->action_move=-1;
				for(int i=0;i<m_unitPtr->actions.size();i++)
				{
					if(m_unitPtr->actions[i].name == "move")
					{
						m_unitPtr->action_move = i;
						break;
					}
				}
			}
			if(m_unitPtr->action_move>=0 && m_unitPtr->action_move<m_unitPtr->actions.size())
			{
				this->m_currentAction = m_unitPtr->action_move;
				m_texID = m_unitPtr->actions[m_currentAction].texid = this->m_mapPtr->GetTexture(m_unitPtr->actions[m_currentAction].textureFile);
			}
			return true;
		}
		return false;
	}
	VOID					MapItem_Unit::AddStandTo(float fTime)
	{CPUTime(MapItem_Unit);
	}
	VOID					MapItem_Unit::SetAngle(float fAngle)
	{CPUTime(MapItem_Unit);
		fAngle		=	((int)fAngle % 360)-180;
		if(fAngle < 0.0f)
			fAngle	+=	360.0f;
		int nAngle		=	0;
		Unit2DAngle* dir = 0;
		nAngle = (int)((fAngle + 22.5f) / 45.0f) % Unit2DAngle_Count;
		static Unit2DAngle s_dir[Unit2DAngle_Count] = {Unit2DAngle_Bottom, Unit2DAngle_LeftBottom, Unit2DAngle_Left, Unit2DAngle_LeftTop, Unit2DAngle_Top, Unit2DAngle_RightTop, Unit2DAngle_Right, Unit2DAngle_RightBottom};
		this->m_currentAngle = s_dir[nAngle];
	}
	LPCSTR					MapItem_Unit::GetActionName()
	{CPUTime(MapItem_Unit);
		if(m_currentAction >= m_unitPtr->actions.size())
			return "";
		return m_unitPtr->actions[m_currentAction].name.t;
	}
	VOID					MapItem_Unit::OnUnitFrameMove()//必须要计算当前要渲染的矩形
	{CPUTime(MapItem_Unit);
		if(m_ways.size() > 0)
		{
			WayPoint2D& w2d = m_ways[m_nPlayIndex];
			m_fPlayLength += g_manager.GetElapsedTime() * this->m_fSpeed * (float)m_mapPtr->m_gridSize.cx;
			float2 pos;
			float rot = 0;
			if(this->m_fPlayLength >= w2d.length)
			{
				m_fPlayLength -= w2d.length;
				m_nPlayIndex++;
				pos = m_ways[m_nPlayIndex].pos;
				rot = m_ways[m_nPlayIndex].fAngle;
			}
			else
			{
				pos = w2d.pos + w2d.dir*this->m_fPlayLength;
				rot = w2d.fAngle;
				this->SetAngle(rot);
			}
			Pixel pp(pos.x, pos.y);
			this->SetPos(pp);
			if(m_nPlayIndex>=(m_ways.size()-1))//移动结束
			{
				m_ways.SetArrayCount(0);
				this->m_currentAction = 0;
				m_texID = m_unitPtr->actions[m_currentAction].texid = this->m_mapPtr->GetTexture(m_unitPtr->actions[m_currentAction].textureFile);
				if(m_moveEndFunc.t[0])
				{
					//lua_Map2DItem_s mgr;
					//SetIPtr(&mgr.m_data, this, MapItem_2x1, IMap2DItemPtr);
					//lua_tinker::call<void>(g_luamanager.GetLuaState(), m_moveEndFunc.t, mgr);
				}
			}
		}
	}
	BOOL					MapItem_Unit::FrameMove(const Pixel& p1)//必须要返回当前要渲染的矩形
	{CPUTime(MapItem_Unit);
		if(!this->m_unitPtr)
			return false;
		Texture* tex = m_mapPtr->GetTexturePtr(this->m_texID);
		if(!tex)return false;
		if(m_currentAction >= m_unitPtr->actions.size())
			return false;
		MapUnitAction& action = this->m_unitPtr->actions[m_currentAction];
		if(action.w<1||action.h<1)return false;
		Pixel pixel(m_pos.x - m_mapPtr->m_renderRect.left, m_pos.y - m_mapPtr->m_renderRect.top);
		int w = tex->GetWidth() / action.w;
		int h = tex->GetHeight() / action.h;
		m_currentAni += (g_manager.GetElapsedTime() * 10 * m_fActionSpeed);
		MapUnitAction::TexIndex & tindex = action.ani[this->m_currentAngle];
		int nIndex = ((int)m_currentAni % (tindex.end - tindex.begin)) + tindex.begin;
		int aw = nIndex % action.w;
		int ah = nIndex / action.w;
		m_textureRc.Set(aw*w,ah*h,(aw+1)*w,(ah+1)*h);
		int cx = action.center.x;//m_centerPos.x;
		int cy = action.center.y;//m_centerPos.y;
		m_centerPos = action.center;
		if(m_scale==0||m_scale==100){
		}
		else
		{
			w = w * m_scale / 100;
			h = h * m_scale / 100;
			cx = cx * m_scale / 100;
			cy = cy * m_scale / 100;
		}
		if(m_mapPtr && m_mapPtr->m_downItem == this && m_downScale != 100)
		{
			w =   w * m_downScale / 100;
			h =   h * m_downScale / 100;
			cx = cx * m_downScale / 100;
			cy = cy * m_downScale / 100;
		}
		m_itemRect.left = pixel.x - cx;
		m_itemRect.bottom = pixel.y + cy;
		m_itemRect.top = m_itemRect.bottom - h;
		m_itemRect.right = m_itemRect.left + w;
		return true;
	}
	VOID					MapItem_Unit::SetPos(const Pixel& pos)
	{CPUTime(MapItem_Unit);
		m_pos = pos;
		GridIndex gr;
		if(m_mapPtr->GetGridInMapPoint(gr, m_pos.x, m_pos.y))
		{
			if(m_last.x==gr.x&&m_last.y==gr.y)
			{
			}
			else
			{
				m_mapPtr->SetUnit(gr.x, gr.y, this, this->m_unitPtr->layer);
				m_last = gr;
			}
		}
	}
	VOID					MapItem_Unit::Render()	//渲染到这个位置上,像素点
	{CPUTime(MapItem_Unit);
		Texture* tex = m_mapPtr->GetTexturePtr(this->m_texID);
		if(!tex)return;
		BOOL bEnable = this->m_bEnabled;
		if(m_bRenderMode == 1)
			bEnable = false;
		g_manager.Draw2D(tex, m_itemRect, m_textureRc, &m_color, 0, 1, bEnable);
		for(int i=0;i<m_binds.size();i++)
		{
			if(m_binds[i]->m_bVisible)
				m_binds[i]->Render(0, m_itemRect);
		}
	}
	VOID	MapItem_2x1::SetPos(const Pixel& p)
	{CPUTime(MapItem_2x1);
		GridIndex r;
		if(m_mapPtr->PickGrid(r, p.x, p.y))
			return;
		m_pos = p;
		m_mapPtr->SetGridItem(r.x, r.y, this);
	}
	MapItem_2x1::MapItem_2x1(Map2D_2x1* m2, BOOL bPush)
	{
		m_mapPtr = m2;
		m_centerPos.Set(0, 0);	//中心点
		m_textureRc.Set(0, 0, 0, 0);
		m_texID.id = m_texID.index = 0;
		m_scale			=	100;
		m_projid			=	0;
		m_offset.Set(0,0);
		if(bPush)
			m_mapPtr->PushItem(this);
		m_pos.Set(0, 0);
		m_downScale = 105;
		m_bRenderMode	=	0;
		m_type	=	0;
		m_runtimeid	=	0;
		m_bPush = true;
	}
	MapItem_2x1::MapItem_2x1(Map2D_2x1* m2, const MapItemData* m)
	{
		m_downScale = 105;
		m_mapPtr = m2;
		m_projid = m->id;
		m_centerPos.Set(0, 0);	//中心点
		m_textureRc.Set(0, 0, 0, 0);
		m_scale			=	100;
		this->m_texID	=	m_mapPtr->GetTexture(m->textureFile.t);
		m_textureRc = m->textureRect;
		m_centerPos = m->center;
		m_scale = m->scale;
		m_offset		=	m->offset;
		this->m_gridCount = m->gridCount;
		this->m_alignBottom = m->alignBottom;
		m_pos.Set(0, 0);
		m_bRenderMode	=	0;
		m_type	=	0;
		m_runtimeid = m_mapPtr->GetNewID();
		m_bPush = false;
		this->m_bDoorExist = m->bDoorExist;
		this->m_door = m->door;
	}
	MapItem_2x1::~MapItem_2x1()
	{
		for(int i=0;i<m_binds.size();i++)
			safe_release(m_binds[i]);
		if(m_mapPtr && m_bPush)
			m_mapPtr->PopItem(this);
	}
	VOID					MapItem_2x1::AddDrawMgr(UIDrawMgr* mgr)
	{CPUTime(MapItem_2x1);
		for(int i=0;i<m_binds.size();i++)
			if(m_binds[i] == mgr)
				return;
		safe_addref(mgr);
		m_binds.push_back(mgr);
	}
	BOOL					MapItem_2x1::PickPixel(int px, int py)//像素级别点击这个对象,设置屏幕像素点
	{CPUTime(MapItem_2x1);
		Rect rc = m_itemRect;
		if(!rc.isPtIn(px, py))
			return false;
		Texture* tex = m_mapPtr->GetTexturePtr(this->m_texID);
		if(!tex)return false;
		if(!tex->GetImageBuffer().IsAlphaImage())
			return true;
		if(!tex->GetImageBuffer().alphas)
			tex->GenericAlphas();//构造透明
		int x = px - rc.left;
		int y = py - rc.top;
		int w = m_textureRc.GetWidth();
		int h = m_textureRc.GetHeight();
		if(m_scale==0||m_scale==100){
		}
		else
		{
			x = x * 100 / m_scale;
			y = y * 100 / m_scale;
		}
		return tex->GetImageBuffer().IsAlpha((int)x + m_textureRc.left, (int)y + m_textureRc.top);
	}
	VOID					MapItem_2x1::Save(CSafeFileHelperW& w)
	{CPUTime(MapItem_2x1);
		int version = 8;
		w << version << m_centerPos << m_textureRc << m_scale << m_projid << m_texID << m_offset << m_name << m_type << m_runtimeid;
		w << m_bDoorExist << m_door;
		w << m_pos;
	}
	VOID					MapItem_2x1::Load(CSafeFileHelperR& r)
	{CPUTime(MapItem_2x1);
		int version = 1;
		r >> version >> m_centerPos >> m_textureRc >> m_scale >> m_projid >> m_texID;
		if(version > 1)
			r >> m_offset;
		if(version > 2)
			r >> m_name;
		if(version > 3)
			r >> m_type;
		if(version > 4)
			r >> m_runtimeid;
		if(m_runtimeid == 0 || version < 6)
			m_runtimeid = m_mapPtr->GetNewID();
		if(version>6)
			r>> m_bDoorExist >> m_door;
		if(version>7)
			r>>m_pos;
		else{
			Pixel p(m_mapPtr->m_mapOffset.x + m_mapPtr->m_gridSize.cx * this->GetGridInMap().x + m_mapPtr->m_gridSize.cx / 2, m_mapPtr->m_mapOffset.y + (m_mapPtr->m_gridSize.cy / 2) * this->GetGridInMap().y + m_mapPtr->m_gridSize.cy / 2);
			if((this->GetGridInMap().y%2) == 0)
				p.x -= m_mapPtr->m_gridSize.cx / 2;
			this->m_pos = p;
		}
	}
	VOID					MapItem_2x1::SetTexture(const PtrID& id, const Rect& textureRect, const Pixel& center)
	{CPUTime(MapItem_2x1);
		m_texID		= id;
		m_textureRc = textureRect;
		m_centerPos = center;
	}
	BOOL					MapItem_2x1::FrameMove(const Pixel& pixel)//必须要返回当前要渲染的矩形
	{CPUTime(MapItem_2x1);
		Texture* tex = m_mapPtr->GetTexturePtr(this->m_texID);
		if(!tex)return false;
		int w = m_textureRc.GetWidth();
		int h = m_textureRc.GetHeight();
		int cx = m_centerPos.x;
		int cy = m_centerPos.y;
		if(m_scale==0||m_scale==100){
		}
		else
		{
			w = w * m_scale / 100;
			h = h * m_scale / 100;
			cx = cx * m_scale / 100;
			cy = cy * m_scale / 100;
		}
		if(m_mapPtr && m_mapPtr->m_downItem == this && m_downScale != 100)
		{
			w =   w * m_downScale / 100;
			h =   h * m_downScale / 100;
			cx = cx * m_downScale / 100;
			cy = cy * m_downScale / 100;
		}
		m_itemRect.left = pixel.x - cx - m_offset.cx;
		m_pos = pixel;
		if(this->m_alignBottom)
		{
			m_itemRect.bottom = pixel.y + cy - m_offset.cy;;
			m_itemRect.top = m_itemRect.bottom - h;
			m_pos.y = pixel.y+cy-m_offset.cy;
		}
		else
		{
			m_itemRect.top = pixel.y - cy - m_offset.cy;
			m_itemRect.bottom = m_itemRect.top + h;
		}
		m_itemRect.right = m_itemRect.left + w;
		//m_pos.Set((m_itemRect.left+m_itemRect.right)/2, (m_itemRect.top+m_itemRect.bottom)/2);
		m_pos.x += m_mapPtr->m_renderRect.left;
		m_pos.y += m_mapPtr->m_renderRect.top;
		return true;
	}
	VOID					MapItem_2x1::AddEvent(int id, LPCSTR szFunc)
	{CPUTime(MapItem_2x1);
		if(id==EventID_None)return;
		for(int i=0;i<m_events.size();i++)
			if(m_events[i].id==id){
				if(szFunc)
					m_events[i].func = szFunc;
				else
					m_events.erase(m_events.begin() + i);
				return;
			}
		if(!szFunc)
			return;
		UILuaEvent e;
		e.id = id;
		e.func = szFunc;
		m_events.push_back(e);
	}
	VOID					MapItem_2x1::Render()	//渲染到这个位置上,像素点
	{CPUTime(MapItem_2x1);
		Texture* tex = m_mapPtr->GetTexturePtr(this->m_texID);
		if(!tex)return;
		BOOL bEnable = this->m_bEnabled;
		if(m_bRenderMode == 1)
			bEnable = false;
		g_manager.Draw2D(tex, m_itemRect, m_textureRc, &m_color, 0, 1, bEnable);
		for(int i=0;i<m_binds.size();i++)
		{
			if(m_binds[i]->m_bVisible)
				m_binds[i]->Render(0, m_itemRect);
		}
	}
	//PhantomMapCross2D::PhantomMapCross2D(){
	//	float fPI = PI / 4.0f;
	//	m_trot.setxy(cosf(fPI),sinf(fPI));
	//	m_rot.setxy(cosf(-fPI),sinf(-fPI));
	//	m_gridSize.Set(64,64);
	//	m_textureMap = 0;
	//	m_renderRect.Set(0, 0, g_manager.GetUIDialogSizeW(), g_manager.GetUIDialogSizeH());
	//}
	//PhantomMapCross2D::~PhantomMapCross2D(){
	//	safe_release(m_textureMap);
	//}
	//VOID					PhantomMapCross2D::RenderTexture(Texture* tex, const Rect& rc, const Pixel& pixel)
	//{
	//	Rect dest(pixel.x - m_gridSize.cx / 2, pixel.y - m_gridSize.cy / 4, pixel.x + m_gridSize.cx / 2, pixel.y + m_gridSize.cy / 4);
	//	g_manager.Draw2D(tex, dest, rc);
	//}
	//VOID					PhantomMapCross2D::Render()//渲染
	//{
	//	m_renderRect.left = m_cameraPos.x - g_manager.GetUIDialogSizeW() / 2;
	//	m_renderRect.top = m_cameraPos.y - g_manager.GetUIDialogSizeH() / 2;
	//	m_renderRect.right = m_renderRect.left + g_manager.GetUIDialogSizeW();
	//	m_renderRect.bottom = m_renderRect.top + g_manager.GetUIDialogSizeH();
	//	if(m_textureMap)
	//	{
	//		int tx = m_textureMap->GetWidth();
	//		int ty = m_textureMap->GetHeight();
	//		if(tx > m_mapPixelSize.cx)
	//			tx = m_mapPixelSize.cx;
	//		if(ty > m_mapPixelSize.cy)
	//			ty = m_mapPixelSize.cy;
	//		Rect rc = m_renderRect;
	//		tx -= rc.left;
	//		ty -= rc.top;
	//		if(tx < 0)
	//			rc.right += tx;
	//		if(ty < 0)
	//			rc.bottom += ty;
	//		g_manager.Draw2D(m_textureMap, Rect(0, 0, rc.GetWidth(), rc.GetHeight()), rc);
	//	}
	//	for(int i=0;i<this->m_tilesIds.size();i++)
	//	{
	//		Pixel& p = m_tilesIds[i];
	//		MapGrid& grid = m_mapGrids[p.x + p.y * m_tileCount.cx];
	//		if(grid.texturePtr)
	//		{
	//			Pixel p(m_mapRect.left + m_gridSize.cx * p.x + m_gridSize.cx / 2, m_mapRect.top + m_gridSize.cy * p.y + m_gridSize.cy / 2);
	//			p = this->PixelToTPixel(p);
	//			p.x -= m_renderRect.left;
	//			p.y -= m_renderRect.top;
	//			this->RenderTexture(grid.texturePtr, grid.textureRc, p);
	//		}
	//	}
	//}
	//VOID					PhantomMapCross2D::SetMapTexture(Texture* tex)	//设置地图背景
	//{
	//	safe_addref(tex);
	//	safe_release(m_textureMap);
	//	m_textureMap = tex;
	//}
	//VOID					PhantomMapCross2D::OnCreateMap()//创建地图
	//{
	//	int maxSize = m_tileCount.cx + m_tileCount.cy;
	//	CDynamicArray<Pixel>* pixels = new CDynamicArray<Pixel>[maxSize];
	//	for(int x=0;x<m_tileCount.cx;x++){
	//		for(int y=0;y<m_tileCount.cy;y++)
	//		{
	//			int xy = x + y;
	//			CDynamicArray<Pixel>& pd = pixels[xy];
	//			pd.push_back(Pixel(x,y));
	//		}
	//	}
	//	m_tilesIds.SetArrayCount(m_tileCount.cx * m_tileCount.cy);
	//	int index = 0;
	//	for(int i=0;i<maxSize;i++)
	//	{
	//		CDynamicArray<Pixel>& pd = pixels[i];
	//		for(int j=0;j<pd.size();j++)
	//		{
	//			m_tilesIds[index++] = pd[j];
	//		}
	//		pixels[i].clear();
	//	}
	//	assert(m_tilesIds.size() == index);
	//	delete[] pixels;
	//	m_gridSize.Set(m_mapPixelSize.cx / m_tileCount.cx, m_mapPixelSize.cy / m_tileCount.cy / 2);
	//	int centerX = m_mapPixelSize.cx / 2;
	//	int centerY = m_mapPixelSize.cy / 2;
	//	m_mapRect.left = centerX - (float)m_gridSize.cx * ((float)m_tileCount.cx / 2);
	//	m_mapRect.top = centerY - (float)m_gridSize.cy * ((float)m_tileCount.cy / 2);
	//	m_mapRect.right = m_mapRect.left + m_gridSize.cx * m_tileCount.cx;
	//	m_mapRect.bottom = m_mapRect.top + m_gridSize.cy * m_tileCount.cy;
	//}
	//VOID					PhantomMapCross2D::SetGridSize(const Size& s){
	//	m_gridSize = s;
	//	int centerX = m_mapPixelSize.cx / 2;
	//	int centerY = m_mapPixelSize.cy / 2;
	//	m_mapRect.left = centerX - (float)m_gridSize.cx * ((float)m_tileCount.cx / 2);
	//	m_mapRect.top = centerY - (float)m_gridSize.cy * ((float)m_tileCount.cy / 2);
	//	m_mapRect.right = m_mapRect.left + m_gridSize.cx * m_tileCount.cx;
	//	m_mapRect.bottom = m_mapRect.top + m_gridSize.cy * m_tileCount.cy;
	//}//格子大小，需要根据图片大小来设定
	//Pixel					PhantomMapCross2D::PixelToTPixel(const Pixel& pixel)//从像素点转换成地图渲染坐标点
	//{
	//	float centerX = this->m_mapRect.left + m_mapRect.GetWidth() / 2;
	//	float centerY = this->m_mapRect.top + m_mapRect.GetHeight() / 2;
	//	float fx = (float)pixel.x - centerX;
	//	float fy = (float)pixel.y - centerY;
	//	//
	//	float dx = (m_trot.x*fx+m_trot.y*fy) * 0.75 + centerX;
	//	float dy = (-m_trot.y*fx+m_trot.x*fy) * 0.5*0.75  + centerY;//1/2纵向
	//	return Pixel((int)dx, (int)dy);
	//	//{	float fx=x,fy=y;	float cf=cosf(fRads),sf=sinf(fRads);	dest.x=cf*fx+sf*fy;	dest.y=-sf*fx+cf*fy;	dest.z=z;	return dest;	}
	//}
	//Pixel					PhantomMapCross2D::TPixelToPixel(const Pixel& tilePixel)//从渲染坐标点转换成坐标点
	//{
	//	float centerX = (float)this->m_mapPixelSize.cx / 2;
	//	float centerY = (float)this->m_mapPixelSize.cy / 2;
	//	float fx = (float)tilePixel.x - centerX;
	//	float fy = ((float)tilePixel.y - centerY) * 2.0f;
	//	//
	//	float dx = m_rot.x*fx+m_rot.y*fy + centerX;
	//	float dy = (-m_rot.y*fx+m_rot.x*fy) + centerY;//1/2纵向
	//	return Pixel((int)dx, (int)dy);
	//}




	Map2D_Event::Map2D_Event(Map2D_2x1* ptr)
	{
		m_id = 0;
		m_type = 0;
		m_grid.x = 0;
		m_grid.y = 0;
		m_pos.Set(0,0);//位置
		m_mapPtr = ptr;
	}
	Map2D_Event::~Map2D_Event()
	{
	}
	VOID	Map2D_Event::SetPos(const Pixel& pos)
	{CPUTime(Map2D_Event);
		m_pos = pos;
		m_mapPtr->GetGridInMapPoint(m_grid, m_pos.x, m_pos.y);
	}

	Map2D_2x1::Map2D_2x1(){
		m_autoid = 1;
		m_mapOffset.Set(0,0);
		m_gridSize.Set(64,64);
		m_renderRect.Set(0, 0, g_manager.GetUIDialogSizeW(), g_manager.GetUIDialogSizeH());
		m_textureExtend.Set(0, 0);
		m_bShowBackTexture	=	true;
		m_bShowFloor		=	true;
		m_bShowBuilding		=	true;
		m_bWhiteCanMove		=	false;
		m_bShowUnits		=	true;
		m_downItem			=	0;
		TexturePtr t;
		g_manager.LoadTexture(t, "default_init_zip/white.png");
		this->m_textureWhite = t;
		safe_addref(m_textureWhite);
		m_bWhiteGrid		=	false;
		m_colorWhiteGrid.setRGBA(1,1,1,1);
		m_pathSize.Set(0,0);
		m_pathalign.Set(0,0);
		m_astarcb.m_mapPtr = this;
		m_astar.setupMap(&m_astarcb);
	}
	Map2D_2x1::~Map2D_2x1(){
		LogInfo("system->mapfree %s", this->m_fileName.t);
		Destroy();
		safe_release(m_textureWhite);
		for(int i=0;i<m_textureMaps.size();i++)
			safe_release(m_textureMaps[i].tex);
	}
	Map2D_Event*			Map2D_2x1::SearchEvent(const char* name)
	{CPUTime(Map2D_2x1);
		for(int i=0;i<m_events.size();i++)
			if(m_events[i]->m_name == name)
				return m_events[i];
		return 0;
	}
	Map2D_Event*			Map2D_2x1::AddEvent(const char* name, const Pixel& mapPos)
	{CPUTime(Map2D_2x1);
		Map2D_Event* e = new Map2D_Event(this);
		e->m_name = name;
		e->m_pos = mapPos;
		e->m_id = GetNewID();
		m_events.push_back(e);
		return e;
	}
	VOID					Map2D_2x1::RemovePoint(Map2D_Event* e)
	{CPUTime(Map2D_2x1);
		for(int i=0;i<m_events.size();i++)
		{
			if(m_events[i] == e)
			{
				delete e;
				m_events.eraseByIndex(i);
				break;
			}
		}
	}
	//CDynamicArray<Map2D_Event*>		m_events;
	VOID					Map2D_2x1::Destroy()
	{CPUTime(Map2D_2x1);
		for(int i=0;i<m_units.size();i++)
		{
			if(m_units[i]){
				m_units[i]->m_mapPtr = 0;
				RemoveUnit(m_units[i]);
				delete m_units[i];
			}
		}
		m_units.clear();
		for(int i=0;i<m_events.size();i++)
			delete m_events[i];
		m_events.clear();
		for(int i=0;i<m_projUnits.size();i++)
			delete m_projUnits[i];
		m_projUnits.clear();
		for(int i=0;i<m_items.size();i++)
			m_items[i]->m_mapPtr = 0;
		m_items.clear();
		MapBase::Destroy();
	}
	VOID					Map2D_2x1::RemoveAllItems()
	{CPUTime(Map2D_2x1);
		for(int i=0;i<m_items.size();i++)
			m_items[i]->m_mapPtr = 0;
		m_items.clear();
		for(int i=0;i<m_mapGrids.size();i++)
		{
			for(int j=0;j<MAX_GRID_LAYER;j++)
				if(m_mapGrids[i].item[j])
				{
					m_mapGrids[i].item[j]->m_inMap.Set(-1, -1);
					safe_release(m_mapGrids[i].item[j]);
				}
		}
	}
	VOID					Map2D_2x1::MapToWindow(Rect& windowRect, const Rect& mapRect)
	{CPUTime(Map2D_2x1);
		windowRect = mapRect;
		windowRect.Offset(-m_renderRect.left, -m_renderRect.top);
	}
	VOID					Map2D_2x1::WindowToMap(Rect& mapRect, const Rect& windowRect)
	{CPUTime(Map2D_2x1);
		mapRect = windowRect;
		mapRect.Offset(m_renderRect.left, m_renderRect.top);
	}
	VOID					Map2D_2x1::RenderTexture2(Texture* tex, const Rect& rc, const Pixel& pixel, const color4* c, int offsetX, int offsetY, int drawMode)
	{CPUTime(Map2D_2x1);
		int w2 = m_gridSize.cx / 2 + m_textureExtend.cx;
		int h2 = m_gridSize.cy / 2 + m_textureExtend.cy;
		Rect dest(pixel.x - w2, pixel.y - h2, pixel.x - w2 + rc.GetWidth(), pixel.y - h2 + rc.GetHeight());
		g_manager.Draw2D(tex, dest, rc, c);
	}
	VOID					Map2D_2x1::RenderTexture(Texture* tex, const Rect& rc, const Pixel& pixel, const color4* c, int offsetX, int offsetY)
	{CPUTime(Map2D_2x1);
		int w2 = m_gridSize.cx / 2 + m_textureExtend.cx;
		int h2 = m_gridSize.cy / 2 + m_textureExtend.cy;
		Rect dest(pixel.x - w2, pixel.y - h2, pixel.x + w2, pixel.y + h2);
		g_manager.Draw2D(tex, dest, rc, c);
	}
	Pixel					Map2D_2x1::GetMapGridCenter(int gridX, int gridY)
	{CPUTime(Map2D_2x1);
		if(gridX<0|| gridY<0||gridX>=m_tileCount.cx ||gridY>=m_tileCount.cy)
			return Pixel(0, 0);
		MapGrid& grid = m_mapGrids[gridX + gridY * m_tileCount.cx];
		Pixel p(m_mapOffset.x + m_gridSize.cx * gridX + m_gridSize.cx / 2, m_mapOffset.y + (m_gridSize.cy / 2) * gridY + m_gridSize.cy / 2);
		if((gridY%2) == 0)
			p.x -= m_gridSize.cx / 2;
		return p;
	}
	Pixel					Map2D_2x1::GetPixelGridCenter(int gridX, int gridY)
	{CPUTime(Map2D_2x1);
		if(gridX<0|| gridY<0||gridX>=m_tileCount.cx ||gridY>=m_tileCount.cy)
			return Pixel(0, 0);
		MapGrid& grid = m_mapGrids[gridX + gridY * m_tileCount.cx];
		Pixel p(m_mapOffset.x + m_gridSize.cx * gridX + m_gridSize.cx / 2, m_mapOffset.y + (m_gridSize.cy / 2) * gridY + m_gridSize.cy / 2);
		if((gridY%2) == 0)
			p.x -= m_gridSize.cx / 2;
		p.x -= m_renderRect.left;
		p.y -= m_renderRect.top;
		return p;
	}
	inline	BOOL	IsRectVisible(const Phantom::Rect& obj, const Phantom::Rect& clip)
	{
		if(obj.right < clip.left || obj.left > clip.right || obj.top > clip.bottom || obj.bottom < clip.top)
			return false;
		return true;
	}
	VOID					Map2D_2x1::Render()//渲染
	{CPUTime(Map2D_2x1);
		for(int i=0;i<this->m_units.size();i++)
		{
			if(!m_units[i])
				continue;
			if(m_units[i]->m_bEnabled && m_units[i]->m_bVisible)
			{
				m_units[i]->OnUnitFrameMove();
			}
		}
		g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		static Phantom::BlendMode src = Phantom::BlendMode_SRCALPHA;
		static Phantom::BlendMode dest = Phantom::BlendMode_INVSRCALPHA;
		g_manager->SetEnableZ(false);
		g_manager->SetBlendMode(src, dest);
		m_renderRect.left = m_cameraPos.x - g_manager.GetUIDialogSizeW() / 2;
		m_renderRect.top = m_cameraPos.y - g_manager.GetUIDialogSizeH() / 2;
		m_renderRect.right = m_renderRect.left + g_manager.GetUIDialogSizeW();
		m_renderRect.bottom = m_renderRect.top + g_manager.GetUIDialogSizeH();
		if(m_bShowBackTexture)
		{
			for(int i=0;i<m_textureMaps.size();i++)
			{
				MapTexture& tex = m_textureMaps[i];
				if(!tex.tex)
					continue;
				Rect rct = tex.textureRect;
				if(rct.right == 0)
					rct.right = tex.tex->GetWidth();
				if(rct.bottom == 0)
					rct.bottom = tex.tex->GetHeight();
				//
				Rect dest = tex.destRect;
				dest.Offset(-m_renderRect.left, -m_renderRect.top);
				int backWidth = g_manager.GetUIDialogSizeW();
				int backHeight = g_manager.GetUIDialogSizeH();
				if(dest.right < 0 || dest.bottom < 0 || dest.left >= backWidth || dest.top >= backHeight)
					continue;
				if(dest.left < 0)
				{
					rct.left -= dest.left;
					dest.left = 0;
				}
				if(dest.top < 0)
				{
					rct.top -= dest.top;
					dest.top = 0;
				}
				if(dest.right > backWidth)
				{
					rct.right -= (dest.right - backWidth);
					dest.right = backWidth;
				}
				if(dest.bottom > backHeight)
				{
					rct.bottom -= (dest.bottom - backHeight);
					dest.bottom = backHeight;
				}
				g_manager.Draw2D(tex.tex, dest, rct);
			}
		}
		
		int screenHeight = g_manager.GetUIDialogSizeH();
		if(m_bWhiteGrid && this->m_textureWhite)
		{
			int beginX = ((m_renderRect.left-m_mapOffset.x) / m_gridSize.cx)-2;
			int beginY = ((m_renderRect.top-m_mapOffset.y) / (m_gridSize.cy / 2))-2;
			int endX = ((m_renderRect.right-m_mapOffset.x) / m_gridSize.cx)+2;
			int endY = ((m_renderRect.bottom-m_mapOffset.y) / (m_gridSize.cy / 2))+10;
			if(beginX<0)beginX=0;
			if(beginY<0)beginY=0;
			if(endX>m_tileCount.cx)endX=m_tileCount.cx;
			if(endY>m_tileCount.cy)endY=m_tileCount.cy;
			Rect textureRc(0, 0, m_textureWhite->GetWidth(), m_textureWhite->GetHeight());
			color4 c(1,1,1,1);
			int w2 = m_gridSize.cx / 2 + m_textureExtend.cx;
			int h2 = m_gridSize.cy / 2 + m_textureExtend.cy;
			for(int y=beginY;y<endY;y++)
			{
				//for(int x=beginX;x<endX;x++)
				{
					Pixel p(m_mapOffset.x + m_gridSize.cx * beginX + m_gridSize.cx / 2, m_mapOffset.y + (m_gridSize.cy / 2) * y + m_gridSize.cy / 2);
					if((y%2) == 0)
						p.x -= m_gridSize.cx / 2;
					p.x -= m_renderRect.left;
					p.y -= m_renderRect.top;
					Rect dest(p.x - w2, p.y - h2, p.x + w2 + (endX-beginX)*m_gridSize.cx, p.y + h2);
					textureRc.right = textureRc.left + (endX-beginX) * m_textureWhite->GetWidth();
					if(dest.top > screenHeight)
						break;
					g_manager.Draw2D(m_textureWhite, dest, textureRc, &c);
					//this->RenderTexture(m_textureWhite, textureRc, p, &c, 0);
				}
				//for(int x=beginX;x<endX;x++)
				//{
				//	Pixel p(m_mapOffset.x + m_gridSize.cx * x + m_gridSize.cx / 2, m_mapOffset.y + (m_gridSize.cy / 2) * y + m_gridSize.cy / 2);
				//	if((y%2) == 0)
				//		p.x -= m_gridSize.cx / 2;
				//	p.x -= m_renderRect.left;
				//	p.y -= m_renderRect.top;
				//	this->RenderTexture(m_textureWhite, textureRc, p, &c, 0);
				//}
			}
		}
		//render floor
		if(m_bShowFloor){
			int beginX = ((m_renderRect.left-m_mapOffset.x) / m_gridSize.cx)-2;
			int beginY = ((m_renderRect.top-m_mapOffset.y) / (m_gridSize.cy / 2))-2;
			int endX = ((m_renderRect.right-m_mapOffset.x) / m_gridSize.cx)+2;
			int endY = ((m_renderRect.bottom-m_mapOffset.y) / (m_gridSize.cy / 2))+10;
			if(beginX<0)beginX=0;
			if(beginY<0)beginY=0;
			if(endX>m_tileCount.cx)endX=m_tileCount.cx;
			if(endY>m_tileCount.cy)endY=m_tileCount.cy;

			for(int gg=0;gg<MAX_GRID_TEXTURE_LAYER;gg++)
			{
				for(int y=beginY;y<endY;y++)
				{
					for(int x=beginX;x<endX;x++)
					{
						MapGrid& grid = this->m_mapGrids[x + y * m_tileCount.cx];
						Pixel p(m_mapOffset.x + m_gridSize.cx * x + m_gridSize.cx / 2, m_mapOffset.y + (m_gridSize.cy / 2) * y + m_gridSize.cy / 2);
						if((y%2) == 0)
							p.x -= m_gridSize.cx / 2;
						p.x -= m_renderRect.left;
						p.y -= m_renderRect.top;
						MapGrid::MapGridTexture& tex = grid.tex[gg];
						if(tex.texID.id == 0)
							continue;
						if((p.y-m_gridSize.cy) > screenHeight)
							break;
						Texture* texPtr = GetTexturePtr(tex.texID);
						if(texPtr)
							this->RenderTexture2(texPtr, tex.textureRc, p, 0, tex.offsetX, tex.offsetY, tex.drawMode);
					}
				}
			}
		}
		if(m_bWhiteCanMove&&this->m_textureWhite)
		{
			int beginX = ((m_renderRect.left-m_mapOffset.x) / m_gridSize.cx)-2;
			int beginY = ((m_renderRect.top-m_mapOffset.y) / (m_gridSize.cy / 2))-2;
			int endX = ((m_renderRect.right-m_mapOffset.x) / m_gridSize.cx)+2;
			int endY = ((m_renderRect.bottom-m_mapOffset.y) / (m_gridSize.cy / 2))+10;
			if(beginX<0)beginX=0;
			if(beginY<0)beginY=0;
			if(endX>m_tileCount.cx)endX=m_tileCount.cx;
			if(endY>m_tileCount.cy)endY=m_tileCount.cy;
			Rect textureRc(0, 0, m_textureWhite->GetWidth(), m_textureWhite->GetHeight());
			color4 c(1,0,0,0.5);
			int w2 = m_gridSize.cx / 2 + m_textureExtend.cx;
			int h2 = m_gridSize.cy / 2 + m_textureExtend.cy;
			for(int y=beginY;y<endY;y++)
			{
				for(int x=beginX;x<endX;x++)
				{
					MapGrid& grid = this->m_mapGrids[x + y * m_tileCount.cx];
					Pixel p(m_mapOffset.x + m_gridSize.cx * x + m_gridSize.cx / 2, m_mapOffset.y + (m_gridSize.cy / 2) * y + m_gridSize.cy / 2);
					if((y%2) == 0)
						p.x -= m_gridSize.cx / 2;
					p.x -= m_renderRect.left;
					p.y -= m_renderRect.top;
					if(grid.pathIndex<0||grid.pathIndex >=m_pathPoints.size() || m_pathPoints[grid.pathIndex].type==0)
						c.setRGBA(1,0,0,0.5);
					else
					{
						if(m_pathPoints[grid.pathIndex].type==PathPoint::type_door)
							c.setRGBA(1,1,0,0.5);
						else
							c.setRGBA(0,1,0,0.5);
					}
					int w2 = m_gridSize.cx / 2 + m_textureExtend.cx;
					int h2 = m_gridSize.cy / 2 + m_textureExtend.cy;
					Rect dest(p.x - w2, p.y - h2, p.x + w2, p.y + h2);
					g_manager.Draw2D(m_textureWhite, dest, textureRc, &c, 0, 1, grid.pathIndex>=0);
				}
			}
			c.setRGBA(0,1,1,1);
			for(int i=0;i<m_events.size();i++)
			{
				GridIndex& g = m_events[i]->m_grid;
				Pixel p(m_mapOffset.x + m_gridSize.cx * g.x + m_gridSize.cx / 2, m_mapOffset.y + (m_gridSize.cy / 2) * g.y + m_gridSize.cy / 2);
				if((g.y%2) == 0)
					p.x -= m_gridSize.cx / 2;
				p.x -= m_renderRect.left;
				p.y -= m_renderRect.top;
				int w2 = m_gridSize.cx / 2 + m_textureExtend.cx;
				int h2 = m_gridSize.cy / 2 + m_textureExtend.cy;
				Rect dest(p.x - w2, p.y - h2, p.x + w2, p.y + h2);
				g_manager.Draw2D(m_textureWhite, dest, textureRc, &c, 0, 1, true);
			}
		}

		//render items
		int bx = (m_renderRect.left / m_gridSize.cx)-32;
		int by = (m_renderRect.top / (m_gridSize.cy / 2))-32;
		int ex = (m_renderRect.right / m_gridSize.cx)+64;
		int ey = (m_renderRect.bottom / (m_gridSize.cy / 2))+64;
		if(bx<0)bx=0;
		if(by<0)by=0;
		if(ex>m_tileCount.cx)ex=m_tileCount.cx;
		if(ey>m_tileCount.cy)ey=m_tileCount.cy;
		Rect screen(0, 0, g_manager.GetUIDialogSizeW(), g_manager.GetUIDialogSizeH());
		if(this->m_bShowBuilding||this->m_bShowUnits){
			for(int k=0;k<MAX_GRID_LAYER;k++)
			{
				for(int y=by;y<ey;y++)
				{
					for(int x=bx;x<ex;x++)
					{
						MapGrid& grid = m_mapGrids[x + y * m_tileCount.cx];
						if(m_bShowBuilding){
							if(grid.item[k] && grid.item[k]->m_bVisible)
							{
								Pixel p(m_mapOffset.x + m_gridSize.cx * x + m_gridSize.cx / 2, m_mapOffset.y + (m_gridSize.cy / 2) * y + m_gridSize.cy / 2);
								if((y%2) == 0)
									p.x -= m_gridSize.cx / 2;
								p.x -= m_renderRect.left;
								p.y -= m_renderRect.top;
								if(!grid.item[k]->FrameMove(p))
									continue;
								if(IsRectVisible(grid.item[k]->m_itemRect, screen))
									grid.item[k]->Render();
							}
						}
						if(m_bShowUnits)
						{
							MapItem* first = grid.units[k];
							while(first)
							{
								if(first->m_bVisible)
								{
									Pixel p(m_mapOffset.x + m_gridSize.cx * x + m_gridSize.cx / 2, m_mapOffset.y + (m_gridSize.cy / 2) * y + m_gridSize.cy / 2);
									if((y%2) == 0)
										p.x -= m_gridSize.cx / 2;
									p.x -= m_renderRect.left;
									p.y -= m_renderRect.top;
									if(first->FrameMove(p))
									{
										if(IsRectVisible(first->m_itemRect, screen))
											first->Render();
									}
								}
								first = first->m_next;
							}
						}
					}
				}
			}
		}
	}
	VOID							Map2D_2x1::AddMap(Texture* tex)	//设置地图背景
	{CPUTime(Map2D_2x1);
		MapTexture mt;
		memset(&mt, 0, sizeof(mt));
		mt.tex = tex;
		safe_addref(mt.tex);
		mt.textureRect.Set(0, 0, tex->GetWidth(), tex->GetHeight());
		mt.destRect = mt.textureRect;
		m_textureMaps.push_back(mt);
	}
	VOID							Map2D_2x1::RemoveMap(int nIndex)//删除一个图片
	{CPUTime(Map2D_2x1);
		if(nIndex < 0 || nIndex >= m_textureMaps.size())
			return;
		safe_release(m_textureMaps[nIndex].tex);
		m_textureMaps.eraseByIndex(nIndex);
	}
	VOID							Map2D_2x1::SetMapRect(int nIndex, const Rect& renderRect, const Rect& textureRect)//渲染位置
	{CPUTime(Map2D_2x1);
		if(nIndex < 0 || nIndex >= m_textureMaps.size())
			return;
		m_textureMaps[nIndex].destRect = renderRect;
		m_textureMaps[nIndex].textureRect = textureRect;
	}
	VOID					Map2D_2x1::OnCreateMap()//创建地图
	{CPUTime(Map2D_2x1);
		if(m_gridSize.cx==0 || m_gridSize.cy==0)
		{
			m_gridSize.Set(m_mapPixelSize.cx / m_tileCount.cx, m_mapPixelSize.cy / m_tileCount.cy / 2);
			SetGridSize(m_gridSize);
		}
	}
	VOID					Map2D_2x1::SetGridSize(const Size& s){CPUTime(Map2D_2x1);
		m_gridSize = s;
		int centerX = m_mapPixelSize.cx / 2;
		int centerY = m_mapPixelSize.cy / 2;
		RecalcGridStates();
	}//格子大小，需要根据图片大小来设定

	BOOL					Map2D_2x1::LoadR(CSafeFileHelperR& r)
	{CPUTime(Map2D_2x1);
		for(int i=0;i<m_textureMaps.size();i++)
			safe_release(m_textureMaps[i].tex);
		m_textureMaps.clear();
		//
		if(!MapBase::LoadR(r))
			return false;
		int version = 1;
		r >> version >> m_autoid >> m_gridSize >> m_mapOffset >>m_renderRect >> m_textureExtend >> m_bShowFloor >> m_bShowBuilding >> m_bShowBackTexture >> m_bWhiteGrid >> m_colorWhiteGrid;
		if(version>=2)
			r >> this->m_pathSize >> this->m_pathalign;
		if(version>=3)
			r >> m_bWhiteCanMove;
		if(version>=5)
			r >> m_bShowUnits;
		int numTex;
		r >> numTex;
		for(int i=0;i<numTex;i++)
		{
			FileName f;
			MapTexture mt;
			r >> f;
			r >> mt.textureRect >> mt.destRect;
			Texture* t = 0;
			g_manager.LoadTexture(&t, f);
			mt.tex = t;
			m_textureMaps.push_back(mt);
		}
		if(version >= 4)
		{
			int nUnits;
			r >> nUnits;
			for(int i=0;i<nUnits;i++)
			{
				MapUnit_Data* data = new MapUnit_Data();
				m_projUnits.push_back(data);
				r >> data->id >> data->name;
				if(version <= 6)
				{
					Pixel center;
					r >> center;
				}
				r >> data->type >> data->layer;
				int action;
				r >> action;
				int len;
				if(version > 4)
					r >> len;
				MapUnitAction act;
				memset(&act, 0, sizeof(act));
				for(int i=0;i<action;i++)
				{
					if(version > 4)
						r.pStream->read(&act, len);
					else
					{
						int temp;
						r >> temp;
						r.pStream->read(&act, sizeof(MapUnitAction));
					}
					act.texid.id = act.texid.index = 0;
					data->actions.push_back(act);
				}	
			}
		}
		int nItem = 0;
		r >> nItem;
		if(nItem > 0)
		{
			m_projItems.SetArrayCount(nItem);
			int nSize;
			r >> nSize;
			for(int k=0;k<nItem;k++)
			{
				MapItemData data;
				memset(&data, 0, sizeof(data));
				r.pStream->read(&data, nSize);
				m_projItems[k] = data;
			}
		}
		int nGrid = 0;
		r >> nGrid;
		if(nGrid > 0)
		{
			m_projGrids.SetArrayCount(nGrid);
			int nSize;
			r >> nSize;
			for(int k=0;k<nGrid;k++)
			{
				MapGrid_2x1 data;
				memset(&data, 0, sizeof(data));
				r.pStream->read(&data, nSize);
				m_projGrids[k] = data;
			}
		}
		int nWhite = 0;
		r >> nWhite;
		if(nWhite)
		{
			FileName f;
			r >> f;
			TexturePtr t;
			g_manager.LoadTexture(t, f);
			m_textureWhite = t;
			safe_addref(m_textureWhite);
		}
		if(version >= 6)
		{
			int units;
			r >> units;
			for(int i=0;i<units;i++)
			{
				int id = 0;
				r >> id;
				MapUnit_Data* data = m_projUnits[0];
				for(int j=0;j<this->m_projUnits.size();j++)
				{
					if(m_projUnits[j]->id == id)
					{
						data = m_projUnits[j];
						break;
					}
				}
				MapItem_Unit* unit = new MapItem_Unit(this, data);
				unit->Load(r);
				m_units.push_back(unit);
				unit->SetPos(unit->m_pos);
			}
			int events;
			r >> events;
			for(int i=0;i<events;i++)
			{
				Map2D_Event* e = new Map2D_Event(this);
				r >> e->m_name >> e->m_id >> e->m_pos >> e->m_grid;
				m_events.push_back(e);
			}

		}
		this->RecalcGridStates();
		return true;
	}
	VOID					Map2D_2x1::SaveW(CSafeFileHelperW& w)
	{CPUTime(Map2D_2x1);
		MapBase::SaveW(w);
		int version = 7;
		w << version << m_autoid << m_gridSize << m_mapOffset <<m_renderRect << m_textureExtend << m_bShowFloor << m_bShowBuilding << m_bShowBackTexture << m_bWhiteGrid << m_colorWhiteGrid;
		w << this->m_pathSize << this->m_pathalign;
		w << this->m_bWhiteCanMove;
		w << m_bShowUnits;
		int numTex = m_textureMaps.size();
		w << numTex;
		for(int i=0;i<numTex;i++)
		{
			MapTexture& mt = m_textureMaps[i];
			w << mt.tex->GetFileName();
			w << mt.textureRect << mt.destRect;
		}
		int nUnits = this->m_projUnits.size();
		w << nUnits;
		for(int i=0;i<nUnits;i++)
		{
			MapUnit_Data* data = m_projUnits[i];
			w << data->id << data->name << data->type << data->layer;
			int action = data->actions.size();
			w << action;
			int len = sizeof(MapUnitAction);
			w << len;
			for(int i=0;i<action;i++)
			{
				w.pStream->write(&data->actions[i], sizeof(MapUnitAction));
			}	
		}
		int nItem = m_projItems.size();
		w << nItem;
		if(nItem)
		{
			int nSize = sizeof(MapItemData);
			w << nSize;
			for(int k=0;k<m_projItems.size();k++)
				w.pStream->write(&m_projItems[k], nSize);
		}
		int nGrid = m_projGrids.size();
		w << nGrid;
		if(nGrid)
		{
			int nSize = sizeof(MapGrid_2x1);
			w << nSize;
			for(int k=0;k<m_projGrids.size();k++)
				w.pStream->write(&m_projGrids[k], nSize);
		}
		int nWhite = (m_textureWhite != 0);
		w << nWhite;
		if(m_textureWhite)
			w << m_textureWhite->GetFileName();
		//
		int units = 0;
		for(int i=0;i<m_units.size();i++)
			if(m_units[i])
				units++;
		w << units;
		for(int i=0;i<m_units.size();i++)
		{
			if(!m_units[i])
				continue;
			w << m_units[i]->m_unitPtr->id;
			m_units[i]->Save(w);
		}
		int events = this->m_events.size();
		w << events;
		for(int i=0;i<m_events.size();i++)
		{
			Map2D_Event* e = m_events[i];
			w << e->m_name << e->m_id << e->m_pos << e->m_grid;
		}
	}
	BOOL					Map2D_2x1::GetGridInMapPoint(GridIndex& ret, int x, int y)//整个地形开始点开始计算
	{CPUTime(Map2D_2x1);
		x -= m_mapOffset.x;
		y -= m_mapOffset.y;
		int yy = y / (m_gridSize.cy / 2);
		if((yy % 2) == 0)
			x += m_gridSize.cx / 2;
		int xx = x / m_gridSize.cx;
		//
		if(xx < 0 || yy < 0 || xx > this->m_tileCount.cx || yy >= this->m_tileCount.cy)
			return false;
		int offx = (x % m_gridSize.cx) - m_gridSize.cx/2;
		int offy = (y % (m_gridSize.cy / 2)) - m_gridSize.cy / 2;
		int offx2=offx;
		if(offx2<0)offx2=-offx2;
		int offy2=offy;
		if(offy2<0)offy2=-offy2;
		float fX = (float)offx2 / (float)(m_gridSize.cx/2);
		float fY = (float)offy2 / (float)(m_gridSize.cy/2);
		float fVal = fX + fY;
		if(fVal<1)
		{
			ret.Set(xx,yy);
		}
		else if(offx>0)
		{
			if(offy>0)
				ret.Set(xx + 1,yy+1);
			else
			{
				if((yy%2) == 0)
					ret.Set(xx, yy - 1);
				else
					ret.Set(xx + 1, yy - 1);
			}
		}
		else
		{
			if(offy>0)
				ret.Set(xx,yy+1);
			else
			{
				if((yy%2) == 0)
					ret.Set(xx - 1,yy-1);
				else
					ret.Set(xx,yy-1);
			}
		}
		if(ret.x<0 || ret.y<0||ret.x >=m_tileCount.cx || ret.y >= m_tileCount.cy)
			return false;
		return true;
	}
	MapItem*				Map2D_2x1::PickItem(int pixelX, int pixelY, int layer)//点击格子内的地形对象,当前屏幕所在位置开始计算
	{CPUTime(Map2D_2x1);
		GridIndex ret;
		if(!PickGrid(ret, pixelX, pixelY))
			return 0;
		MapItem* item = 0;
		if(layer > 0)
			item = this->GetGridItem(ret.x, ret.y, layer);
		else if(m_mapGrids[ret.x + ret.y * m_tileCount.cx].item[0])
			item = m_mapGrids[ret.x + ret.y * m_tileCount.cx].item[0];
		else
		{
			GridIndex& s = m_mapGrids[ret.x + ret.y * m_tileCount.cx].state;
			if(s.x<0||s.y<0||s.x>=m_tileCount.cx||s.y>=m_tileCount.cy)
				return 0;
			item = m_mapGrids[s.x + s.y * m_tileCount.cx].item[0];
		}
		if(!item || !item->m_bEnabled || !item->m_bVisible)
			return 0;
		return item;
	}
	MapItem*				Map2D_2x1::PickItemPixel(int pixelX, int pixelY, int layer)//点击格子内的地形对象,当前屏幕所在位置开始计算
	{CPUTime(Map2D_2x1);
		for(int i=0;i<m_items.size();i++)
		{
			MapItem_2x1 * item = m_items[i];
			if(!item->m_bVisible || !item->m_bEnabled)
				continue;
			if(!item->m_itemRect.isPtIn(pixelX, pixelY))
				continue;
			if(item->PickPixel(pixelX, pixelY))
				return item;
		}
		return 0;
	}
	//BOOL					Map2D_2x1::PickBox(GridIndex& ret, int x, int y)//点击格子,当前屏幕所在位置开始计算
	//{
	//	if(PickGrid(ret, x, y))
	//	{
	//		ret.y--;
	//		return true;
	//	}
	//	return false;
	//	x += m_renderRect.left;
	//	y += m_renderRect.top;
	//	x -= m_mapOffset.x;
	//	y -= m_mapOffset.y;
	//	int cx = m_gridSize.cx;
	//	ret.x = (x / cx);
	//	ret.y = (y / m_gridSize.cy);
	//	return (ret.x >= 0 && ret.y >= 0 && ret.x < (m_tileCount.cx) && ret.y < (m_tileCount.cy/2));
	//}
	BOOL					Map2D_2x1::PickGrid(GridIndex& ret, int x, int y)//点击格子
	{CPUTime(Map2D_2x1);
		x += m_renderRect.left;
		y += m_renderRect.top;
		return GetGridInMapPoint(ret, x, y);
	}
	BOOL					Map2D_2x1::IsItemIn(int x, int y, GridIndex& itemGrid, MapItem* item)//对象是否被点击
	{CPUTime(Map2D_2x1);
		x += m_renderRect.left;
		y += m_renderRect.top;
		GridIndex ret;
		if(!GetGridInMapPoint(ret, x, y))
			return false;
		int grids = item->GetGridCount();
		int gridX = itemGrid.x;
		int gridY = itemGrid.y;
		int x1 = gridX;
		for(int i=0;i<grids;i++)
		{
			int y1 = gridY + i;
			int x2 = x1;
			for(int j=0;j<grids;j++)
			{
				int y2 = y1 - j;
				if(x2 == ret.x && y2 == ret.y)
				{
					return true;
				}
				if((y2%2)==1)
					x2++;
			}
			
			if((y1%2)==1)
				x1++;
		}
		return false;
	}
	MapItem*				Map2D_2x1::CreateMapItem(int type)	//根据类型创建对象
	{CPUTime(Map2D_2x1);
		switch(type){
		case 1:
			return new MapItem_2x1(this);
		}
		assert(false);
		return 0;
	}
	VOID					Map2D_2x1::RecalcGridStates()//重新计算所有的可走格子信息
	{CPUTime(Map2D_2x1);
		RecalcMoveStates();
		for(int i=0;i<m_mapGrids.size();i++)
			m_mapGrids[i].state.Set(-1, -1);
		for(int gridX=0;gridX<m_tileCount.cx;gridX++)
		{
			for(int gridY=0;gridY<m_tileCount.cy;gridY++)
			{
				//仅仅计算第一个物品，其他上层的物品默认只是显示用的
				MapItem* item = m_mapGrids[gridX + gridY * m_tileCount.cx].item[0];
				if(item && item->m_bVisible)
				{
					int grids = item->GetGridCount();
					int x1 = gridX;
					for(int i=0;i<grids;i++)
					{
						int y1 = gridY + i;
						int x2 = x1;
						for(int j=0;j<grids;j++)
						{
							int y2 = y1 - j;
							if(x2>=0&&y2>=0&&x2<m_tileCount.cx&&y2<m_tileCount.cy)
								m_mapGrids[x2 + y2*m_tileCount.cx].state.Set(gridX,gridY);// = MapGrid::GridState_DisableMove;//如果有任何
							//RenderOneGrid(x2, y2);
							if((y2%2)==1)
								x2++;
						}
						if((y1%2)==1)
							x1++;
					}
				}
			}
		}
	}
	BOOL					Map2D_2x1::IsItemCanInsert(int gridX, int gridY, MapItem* item)
	{CPUTime(Map2D_2x1);
		int grids = item->GetGridCount();
		int x1 = gridX;
		for(int i=0;i<grids;i++)
		{
			int y1 = gridY + i;
			int x2 = x1;
			for(int j=0;j<grids;j++)
			{
				int y2 = y1 - j;
				if(x2>=0&&y2>=0&&x2<m_tileCount.cx&&y2<m_tileCount.cy)
				{
					if(m_mapGrids[x2 + y2*m_tileCount.cx].state.exist())
						return false;
				}
				else
					return false;
				if((y2%2)==1)
					x2++;
			}
			if((y1%2)==1)
				x1++;
		}
		return true;
	}
	VOID					Map2D_2x1::SetWhiteTexture(const char* szWhiteTexture)
	{CPUTime(Map2D_2x1);
		TexturePtr t;
		g_manager.LoadTexture(t, szWhiteTexture);
		safe_release(m_textureWhite);
		m_textureWhite = t;
		safe_addref(m_textureWhite);
	}
	VOID					Map2D_2x1::RenderOneGrid(int gridX, int gridY, const color4* color)
	{CPUTime(Map2D_2x1);
		color4 c(0,1,0,0.8f);
		if(gridX>=0&&gridY>=0&&gridX<m_tileCount.cx&&gridY<m_tileCount.cy)
		{
			if(m_mapGrids[gridX + gridY*m_tileCount.cx].state.exist())
				c.setRGBA(1,0,0,0.8);
		}
		else
			c.setRGBA(1,0,0,0.8);
		if(color)c *= *color;
		Pixel p(m_mapOffset.x + m_gridSize.cx * gridX + m_gridSize.cx / 2, m_mapOffset.y + (m_gridSize.cy / 2) * gridY + m_gridSize.cy / 2);
		if((gridY%2) == 0)
			p.x -= m_gridSize.cx / 2;
		p.x -= m_renderRect.left;
		p.y -= m_renderRect.top;
		g_manager.Draw2D(m_textureWhite, Rect(p.x-m_gridSize.cx / 2-1, p.y-m_gridSize.cy/2-1, p.x+m_gridSize.cx / 2+1, p.y+m_gridSize.cy/2+1), Rect(0, 0, m_textureWhite->GetWidth(), m_textureWhite->GetHeight()), &c);
	}
	VOID					Map2D_2x1::RenderDrawGrid(int gridX, int gridY, MapItem* item, const color4* color)
	{CPUTime(Map2D_2x1);
		int grids = item->GetGridCount();
		int x1 = gridX;
		for(int i=0;i<grids;i++)
		{
			int y1 = gridY + i;
			int x2 = x1;
			for(int j=0;j<grids;j++)
			{
				int y2 = y1 - j;
				if(x2==gridX&&y2==gridY)
				{
					color4 first(0,0,1,1);
					RenderOneGrid(x2, y2, &first);
				}
				else
					RenderOneGrid(x2, y2, color);
				if((y2%2)==1)
					x2++;
			}
			
			if((y1%2)==1)
				x1++;
		}
		if(item->m_bDoorExist)
		{
			Pixel pos = item->m_pos + item->m_door;
			pos.x -= this->m_renderRect.left;
			pos.y -= this->m_renderRect.top;
			GridIndex ret;
			if(this->PickGrid(ret, pos.x, pos.y))
			{
				Pixel p(m_mapOffset.x + m_gridSize.cx * ret.x + m_gridSize.cx / 2, m_mapOffset.y + (m_gridSize.cy / 2) * ret.y + m_gridSize.cy / 2);
				if((ret.y%2) == 0)
					p.x -= m_gridSize.cx / 2;
				p.x -= m_renderRect.left;
				p.y -= m_renderRect.top;
				color4 c(1,1,0,1);
				g_manager.Draw2D(m_textureWhite, Rect(p.x-m_gridSize.cx / 2-1, p.y-m_gridSize.cy/2-1, p.x+m_gridSize.cx / 2+1, p.y+m_gridSize.cy/2+1), Rect(0, 0, m_textureWhite->GetWidth(), m_textureWhite->GetHeight()), &c);
			}
		}
	}
	VOID					Map2D_2x1::CommitAllItems(const MapItemData* data)//刷新所有的对象数据的变化
	{CPUTime(Map2D_2x1);
		for(int y=0;y<this->m_tileCount.cy;y++)
		{
			for(int x=0;x<this->m_tileCount.cx;x++)
			{
				MapGrid& grid = m_mapGrids[x + y * m_tileCount.cx];
				for(int k=0;k<MAX_GRID_LAYER;k++)
					if(grid.item[k] && grid.item[k]->m_projid == data->id)
					{
						if(grid.item[k]->GetItemType() == ItemType_2x1)
						{
							MapItem_2x1* item = static_cast<MapItem_2x1*>(grid.item[k]);
							item->SetTexture(GetTexture(data->textureFile.t), data->textureRect, data->center);
							item->m_scale	=	data->scale;
							item->m_offset	=	data->offset;
							item->m_type	=	data->type;
							item->m_alignBottom = data->alignBottom;
							item->m_door	=	data->door;
							item->m_bDoorExist = data->bDoorExist;
							if(data->layer<0||data->layer>=MAX_GRID_LAYER)
								continue;
							if(item->m_name.t[0] == 0)
								sprintf(item->m_name.t, "%s[%d,%d]", data->name.t, item->GetGridInMap().x, item->GetGridInMap().y);
							if(k == data->layer)
								continue;
							safe_release(grid.item[data->layer]);
							grid.item[data->layer] = grid.item[k];
							grid.item[k] = 0;
						}
					}
			}
		}
	}
	//const MapGrid_2x1*		Map2D_2x1::GetGrid_2x1(int nGridX, int nGridY, int layer)
	//{
	//	int id = this->GetGridTextureID(nGridX, nGridY, layer);
	//	if(id <= 0)return 0;
	//	for(int i=0;i<m_projGrids.size();i++)
	//		if(id == m_projGrids[i].id)
	//			return &m_projGrids[i];
	//	return 0;
	//}
	int						Map2D_2x1::GetCreateCount(int id)
	{CPUTime(Map2D_2x1);
		int ret = 0;
		for(int i=0;i<this->m_items.size();i++)
			if(m_items[i]->m_projid == id)
				ret++;
		return ret;
	}

	VOID					Map2D_2x1::PushItem(MapItem_2x1* item)
	{
		m_items.push_back(item);
	}
	VOID					Map2D_2x1::PopItem(MapItem_2x1* item)
	{
		for(int i=0;i<m_items.size();i++)
			if(m_items[i] == item)
				m_items.eraseByIndex(i, 1, true);
	}
	const MapItemData*		Map2D_2x1::GetGrid_Item(int nGridX, int nGridY, int layer)
	{CPUTime(Map2D_2x1);
		MapItem* item = this->GetGridItem(nGridX, nGridY, layer);
		if(!item)return 0;
		for(int i=0;i<m_projItems.size();i++)
			if(item->m_projid == m_projItems[i].id)
				return &m_projItems[i];
		return 0;
	}
	VOID					Map2D_2x1::RecalcMoveStates()
	{CPUTime(Map2D_2x1);
		int gridx = m_pathalign.x;
		int gridy = m_pathalign.y;
		int x1 = gridx;
		for(int i=0;i<m_mapGrids.size();i++)
			m_mapGrids[i].pathIndex = -1;
		if(this->m_pathSize.cx*this->m_pathSize.cy>0)
			m_pathPoints.SetArrayCount(this->m_pathSize.cx*this->m_pathSize.cy);
		if(m_pathPoints.size() > 0)
			memset(m_pathPoints.GetPtr(), 0, sizeof(PathPoint)*m_pathPoints.size());
		for(int i=0;i<m_pathSize.cx;i++)
		{
			int y1 = gridy - i;
			int x2 = x1;
			for(int j=0;j<m_pathSize.cy;j++)
			{
				int y2 = y1 - j;
				PathPoint& pp = m_pathPoints[i+j*m_pathSize.cx];
				pp.gx = x2;
				pp.gy = y2;
				pp.type = 0;
				pp.objid = 0;
				if(x2>=0 && y2>=0&& x2<m_tileCount.cx&&y2<m_tileCount.cy)
				{
					MapGrid& gg = m_mapGrids[x2 + y2*m_tileCount.cx];
					if(gg.state.exist())
						pp.type = 0;
					else
					{
						pp.type = gg.moveEnable;
					}
					gg.pathIndex = i+j*m_pathSize.cx;
				}
				//RenderOneGrid(x2, y2, color);
				if((y2%2)==1)
					x2++;
			}
			
			if((y1%2)==0)
				x1--;
		}
		for(int i=0;i<this->m_items.size();i++)
		{
			if(m_items[i]->m_bVisible && m_items[i]->m_bDoorExist)
			{
				Pixel pos = m_items[i]->m_pos + m_items[i]->m_door;
				pos.x -= this->m_renderRect.left;
				pos.y -= this->m_renderRect.top;
				GridIndex ret;
				if(this->PickGrid(ret, pos.x, pos.y))
				{
					MapGrid& gg = m_mapGrids[ret.x + ret.y*m_tileCount.cx];
					if(gg.pathIndex>=0&&gg.pathIndex<m_pathPoints.size())
						m_pathPoints[gg.pathIndex].type = PathPoint::type_door;
				}
			}
		}

	}
	VOID					Map2D_2x1::SetGridMoveEnable(int gridx, int gridy, int state)
	{CPUTime(Map2D_2x1);
		if(gridx<0||gridy<0||gridx>=m_tileCount.cx||gridy>=m_tileCount.cy)
			return;
		this->m_mapGrids[gridx+gridy*m_tileCount.cx].moveEnable = state;
	}
	INT						Map2D_2x1::GetGridMoveEnable(int gridx, int gridy)
	{CPUTime(Map2D_2x1);
		if(gridx<0||gridy<0||gridx>=m_tileCount.cx||gridy>=m_tileCount.cy)
			return -1;
		return m_mapGrids[gridx+gridy*m_tileCount.cx].moveEnable;
	}
	VOID					Map2D_2x1::SetPathAlign(const GridIndex& g)//设置可移动格子初始格子位置
	{CPUTime(Map2D_2x1);
		m_pathalign = g;
		RecalcMoveStates();
	}
	BOOL					Map2D_2x1::PathFromGrid(PathIndex& ret, const GridIndex& g)
	{CPUTime(Map2D_2x1);
		if(g.x<0||g.y<0||g.x>=m_tileCount.cx||g.y>=m_tileCount.cy)
			return false;
		if(m_mapGrids[g.x+g.y*m_tileCount.cx].pathIndex<0)return false;
		ret.x = m_mapGrids[g.x+g.y*m_tileCount.cx].pathIndex % m_pathSize.cx;
		ret.y = m_mapGrids[g.x+g.y*m_tileCount.cx].pathIndex / m_pathSize.cx;
		if(ret.x < 0 || ret.y < 0 || ret.y >= m_pathSize.cy)
			return false;
		return true;
	}
	VOID					Map2D_2x1::SetAllMove(INT bMove)
	{CPUTime(Map2D_2x1);
		for(int i=0;i<this->m_mapGrids.size();i++)
		{
			m_mapGrids[i].moveEnable = bMove;
		}
		this->RecalcMoveStates();
	}
	VOID					Map2D_2x1::SetTextureAllMove(MapGrid_2x1& t, INT bMove)
	{CPUTime(Map2D_2x1);
		for(int i=0;i<this->m_mapGrids.size();i++)
		{
			if(m_mapGrids[i].tex[0].id == t.id)
			{
				m_mapGrids[i].moveEnable = bMove;
			}
		}
		this->RecalcMoveStates();
	}
	int						Map2DAstar::IsAstarMoveEnable(int x, int y)
	{CPUTime(Map2DAstar);
		if(x<0||x>=m_mapPtr->m_pathSize.cx||y<0||y>=m_mapPtr->m_pathSize.cy)
			return 0;
		return m_mapPtr->m_pathPoints[x+y*m_mapPtr->m_pathSize.cx].type;
	}
	int						Map2DAstar::GetAstarWidth()
	{CPUTime(Map2DAstar);
		return m_mapPtr->m_pathSize.cx;
	}
	int						Map2DAstar::GetAstarHeight()
	{CPUTime(Map2DAstar);
		return m_mapPtr->m_pathSize.cy;
	}
	BOOL					Map2D_2x1::FindPath(CDynamicArray<WayPoint2D>& ways, const Pixel& mapFrom, const Pixel& mapTo)
	{CPUTime(Map2D_2x1);
		if(this->m_pathPoints.size() == 0)
			return false;
		PathIndex begin, end;
		PathIndex oldbegin;
		oldbegin.x=-1;
		oldbegin.y=-1;
		begin = oldbegin;
		if(!PathFromMap(end, mapTo.x, mapTo.y))
			return false;
		BOOL bBeginD = false;
		Pixel newBegin(0,0);
		if(!PathFromMap(begin, mapFrom.x, mapFrom.y) || !m_astarcb.IsAstarMoveEnable(begin.x, begin.y))
		{
			oldbegin = begin;
			bBeginD = true;
			int num = 1;
			while(true){
				bool bfind = false;
				for(int x=-num;x<=num;x++)
				{
					for(int y=-num;y<=num;y++)
					{
						if(m_astarcb.IsAstarMoveEnable(begin.x+x,begin.y+y))
						{
							GridIndex g;
							PathIndex pp;
							pp.x = begin.x+x;
							pp.y = begin.y+y;
							if(GridFromPath(g,pp))
							{
								begin = pp;
								newBegin = this->GetMapGridCenter(g.x, g.y);
								bfind = true;
								break;
							}
						}
					}
				}
				if(bfind)
					break;
				num++;
				if(num>=10)
					return false;
			}
		}
		m_astar.setupMap(&m_astarcb);
		if(!m_astar.find_linear(begin.x, begin.y, end.x, end.y, true))
			return false;
		float2 vfrom(mapFrom.x, mapFrom.y);
		float2 vto(mapTo.x, mapTo.y);
		float2 vnewb(newBegin.x, newBegin.y);
		if(m_astar.getfindPath().getNumPath() <= 2)
		{
			ways.SetArrayCount(2);
			ways[0].Setup(vfrom);
			if(bBeginD)
			{
				ways[1].Setup(vnewb);
				ways[2].Setup(vto);
			}
			else
				ways[1].Setup(vto);
			return true;
		}
		int index = 0;
		ways.SetArrayCount(m_astar.getfindPath().getNumPath()+5);
		PathIndex last;
		last.x = oldbegin.x;
		last.y = oldbegin.y;
		ways[index++].Setup(vfrom);
		if(bBeginD)
		{
			ways[index++].Setup(vnewb);
			last = begin;
		}
		for(int i= m_astar.getfindPath().getNumPath() - 1;i >= 0; i--)// - 1; i > 0; i--)
		{
			Pixel* p	=	m_astar.getfindPath().getPath(i);
			PathIndex pindex;
			pindex.x = p->x;
			pindex.y = p->y;
			GridIndex g;
			if(!GridFromPath(g,pindex))
				continue;
			if(pindex.x==last.x&&pindex.y==last.y)
				continue;
			last = pindex;
			Pixel point(m_mapOffset.x + m_gridSize.cx * g.x + m_gridSize.cx / 2, m_mapOffset.y + (m_gridSize.cy / 2) * g.y + m_gridSize.cy / 2);
			if((g.y%2) == 0)
				point.x -= m_gridSize.cx / 2;
			float2 v(point.x, point.y);
			ways[index++].Setup(v);
		}
		if(last.x!=end.x&&last.y!=end.y)
			ways[index++].Setup(vto);
		ways.SetArrayCount(index);
		return true;
	}
	BOOL					Map2D_2x1::PathFromMap(PathIndex& ret, int mapX, int mapY)
	{CPUTime(Map2D_2x1);
		GridIndex g;
		if(!GetGridInMapPoint(g, mapX, mapY))
			return false;
		return PathFromGrid(ret, g);
	}
	BOOL					Map2D_2x1::PathFromPixel(PathIndex& ret, int pixelX, int pixelY)
	{CPUTime(Map2D_2x1);
		GridIndex g;
		if(!this->PickGrid(g, pixelX, pixelY))
			return false;
		return PathFromGrid(ret, g);
	}
	BOOL					Map2D_2x1::GridFromPath(GridIndex& ret, const PathIndex& p)
	{CPUTime(Map2D_2x1);
		if(p.x <0 || p.y<0||p.x>=m_pathSize.cx||p.y>=m_pathSize.cy)
			return false;
		PathPoint& pp = this->m_pathPoints[p.x+p.y*m_pathSize.cx];
		ret.x = pp.gx;
		ret.y = pp.gy;
		if(ret.x<0||ret.y<0||ret.x>=m_tileCount.cx||ret.y>=m_tileCount.cy)
			return false;
		return true;
	}
	MapItem_Unit*			Map2D_2x1::AddUnit(LPCSTR szUnitName, const GridIndex& g)
	{CPUTime(Map2D_2x1);
		for(int i=0;i<this->m_projUnits.size();i++)
		{
			if(m_projUnits[i]->name == szUnitName)
			{
				MapItem_Unit* unit = new MapItem_Unit(this, m_projUnits[i]);
				bool bFind = false;
				for(int j=0;j<m_units.size();j++)
				{
					if(m_units[j] == 0)
					{
						m_units[j] = unit;
						bFind = true;
						break;
					}
				}
				if(!bFind)
					m_units.push_back(unit);
				unit->m_pos = this->GetMapGridCenter(g.x, g.y);
				this->SetUnit(g.x, g.y, unit, m_projUnits[i]->layer);
				return unit;
			}
		}
		return 0;
	}
	VOID					Map2D_2x1::DeleteUnit(MapItem_Unit* unit)
	{CPUTime(Map2D_2x1);
		for(int i=0;i<m_units.size();i++)
		{
			if(m_units[i] == unit)
			{
				m_units[i]->m_mapPtr = 0;
				RemoveUnit(m_units[i]);
				safe_release(m_units[i]);
				m_units[i] = 0;
				break;
			}
		}
	}
	VOID					Map2D_2x1::DeleteFromUnitID(int unitid)//删除所有对应的角色
	{CPUTime(Map2D_2x1);
		CDynamicArray<MapItem_Unit*> deletes;
		for(int i=0;i<m_units.size();i++)
		{
			if(m_units[i]->m_projid == unitid)
			{
				m_units[i]->m_mapPtr = 0;
				this->RemoveUnit(m_units[i]);
				safe_release(m_units[i]);
				m_units[i] = 0;
			}
		}
	}
	MapItem_Unit*			Map2D_2x1::AddUnit(int unitid, const GridIndex& g)
	{CPUTime(Map2D_2x1);
		for(int i=0;i<this->m_projUnits.size();i++)
		{
			if(m_projUnits[i]->id == unitid)
			{
				MapItem_Unit* unit = new MapItem_Unit(this, m_projUnits[i]);
				unit->m_pos = this->GetMapGridCenter(g.x, g.y);
				bool bFind = false;
				for(int j=0;j<m_units.size();j++)
				{
					if(m_units[j] == 0)
					{
						m_units[j] = unit;
						bFind = true;
						break;
					}
				}
				if(!bFind)
					m_units.push_back(unit);
				this->SetUnit(g.x, g.y, unit, m_projUnits[i]->layer);
				return unit;
			}
		}
		return 0;
	}
	VOID					Map2D_2x1::SetPathSize(int w,int h)
	{CPUTime(Map2D_2x1);
		CDynamicArray<PathPoint>			old;
		old = m_pathPoints;
		Size oldsize = m_pathSize;
		m_pathSize.Set(w,h);
		if(w*h>0)
		{
			m_pathPoints.SetArrayCount(w*h);
			for(int x=0;x<oldsize.cx;x++)
			{
				if(x>=w)continue;
				for(int y=0;y<oldsize.cy;y++)
				{
					if(y>=h)continue;
					m_pathPoints[x+y*w] = old[x+y*oldsize.cx];
				}
			}
		}
	}

	MyMap2D::MyMap2D()
	{
		m_bEnableMove	=	true;
		m_bVisible		=	true;
		m_bMouseMove	=	false;
		m_downItem		=	0;
		m_bIsButtonDown =	false;
		g_manager.PushMap(this);
	}
	MyMap2D::~MyMap2D()
	{
		g_manager.PopMap(this);
		safe_release(m_downItem);
	}

	BOOL			MyMap2D::OnInputEvent(Phantom::InputEventID evt, int x, int y, unsigned int touchIndex)
	{CPUTime(MyMap2D);
		if(touchIndex != 0)
			return false;
		BOOL bRet = false;
		Pixel ret;
		g_manager.GetCursorPos2D(ret);
		if(evt == Phantom::InputEventID_ButtonDown)
		{
			m_downItem = 0;
			int maxY = 0;
			MapItem_2x1* itemPick = 0;// = static_cast<MapItem_2x1*>(PickItemPixel(ret.x, ret.y, 0));
			BOOL bIn = false;
			for(int i=0;i<m_items.size();i++)
			{
				MapItem_2x1 * item = m_items[i];
				if(!item->m_bVisible || !item->m_bEnabled)
					continue;
				for(int j=0;j<item->m_binds.size();j++)
				{
					if(!item->m_binds[j]->m_bEnabled || !item->m_binds[j]->m_bVisible)
						continue;
					if(item->m_binds[j]->m_fnclick.t[0])
					{
						if(item->m_binds[j]->m_renderRect.isPtIn(ret.x,ret.y))
						{
							itemPick = item;
							break;
							bIn = true;
						}
					}
				}
				if(!bIn){
					if(!item->m_itemRect.isPtIn(ret.x, ret.y))
						continue;
					if(/*item->m_events.size() > 0 && */item->PickPixel(ret.x, ret.y))
					{
						if(item->GetGridInMap().y > maxY)
						{
							maxY = item->GetGridInMap().y;
							itemPick = item;
						}
					}
				}
				else
					break;
			}

			if(itemPick)
			{
				if(bIn){
					m_downItem = itemPick;
				}
				else
				{
					for(int i=0;i<itemPick->m_events.size();i++)
					{
						if(itemPick->m_events[i].id == EventID_Click && itemPick->m_events[i].func.t[0])
						{
							m_downItem = itemPick;
							bRet = true;
							break;
						}
					}
				}
				if(m_downItem){
					safe_addref(m_downItem);
					for(int i=0;i<m_downItem->m_binds.size();i++)
					{
						if(!m_downItem->m_binds[i]->m_bEnabled || !m_downItem->m_binds[i]->m_bVisible)
							continue;
						if(m_downItem->m_binds[i]->OnDownClick(ret, EventID_Click))
							break;
					}
				}
			}
			m_bMouseMove	= false;
			m_bIsButtonDown = false;
			if(this->m_bEnableMove)
			{
				m_bIsButtonDown = true;
				m_downPos = ret;
			}
		}
		else if(evt == Phantom::InputEventID_ButtonUp)
		{
			if(m_downItem && m_bMouseMove < 25)
			{
				for(int i=0;i<m_downItem->m_events.size();i++)
				{
					if(m_downItem->m_events[i].id == EventID_Click)
					{
						if(m_downItem->m_events[i].func.t[0])
						{
							if(m_downItem->m_itemRect.isPtIn(ret.x, ret.y) && m_downItem->PickPixel(ret.x, ret.y))
							{
								//lua_Map2DItem_s mgr;
								//SetIPtr(&mgr.m_data, m_downItem, MapItem_2x1, IMap2DItemPtr);
								//lua_tinker::call<void>(g_luamanager.GetLuaState(), m_downItem->m_events[i].func.t, mgr);
								bRet = true;
							}
						}
					}
				}
				for(int i=0;i<m_downItem->m_binds.size();i++)
				{
					if(!m_downItem->m_binds[i]->m_bEnabled || !m_downItem->m_binds[i]->m_bVisible)
						continue;
					if(m_downItem->m_binds[i]->OnClick(ret, EventID_Click))
						break;
				}
				for(int i=0;i<m_downItem->m_binds.size();i++)
				{
					if(!m_downItem->m_binds[i]->m_bEnabled || !m_downItem->m_binds[i]->m_bVisible)
						continue;
					m_downItem->m_binds[i]->OnUpClick(ret, EventID_Click);
				}
			}
			else if(m_bMouseMove < 25 && m_downItem == 0)
			{
				if(m_funcClick.t[0])
				{
					//lua_Map2D_s mgr;
					//SetIPtr(&mgr.m_data, this, Map2D_2x1, IMap2DPtr);
					//lua_Pixel pos;
					//pos.Set(ret.x + this->m_renderRect.left, ret.y + this->m_renderRect.top);
					//lua_tinker::call<void>(g_luamanager.GetLuaState(), m_funcClick.t, mgr, pos);
				}
			}
			safe_release(m_downItem);
			m_bIsButtonDown = false;
		}
		else if(evt == Phantom::InputEventID_MouseMove)
		{
			if(m_bIsButtonDown && m_bEnableMove)
			{
				Pixel offset = ret - m_downPos;
				m_downPos = ret;
				Pixel cam = this->GetCameraPos() - offset;
				this->SetCameraPos(cam);
				m_bMouseMove = offset.x*offset.x+offset.y*offset.y;
				bRet = true;
			}
		}
		return bRet;
	}
	VOID			MyMap2D::OnRender(double dElapsedTime)
	{CPUTime(MyMap2D);
		this->Render();
	}

};
