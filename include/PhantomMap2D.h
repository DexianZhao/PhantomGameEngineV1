#ifndef __PHANTOM_MAP2D_H__
#define	__PHANTOM_MAP2D_H__
#include "PhantomMapBase.h"
#include "PhantomUIManager.h"
#include "PhantomAStarPath.h"

namespace Phantom{

//class	PhantomMapCross2D: public MapBase//斜角的地图,标准2D地图
//{
//public:
//	PhantomMapCross2D();
//	virtual ~PhantomMapCross2D();
//	VOID							SetGridSize(const Size& s);//格子大小，需要根据图片大小来设定
//public://必须继承的函数
//	Pixel							PixelToTPixel(const Pixel& pixel);//从像素点转换成地图渲染坐标点
//	Pixel							TPixelToPixel(const Pixel& tilePixel);//从渲染坐标点转换成坐标点
//	virtual	VOID					RenderTexture(Texture* tex, const Rect& rc, const Pixel& pixel);
//	virtual	VOID					Render();//渲染
//	VOID							SetMapTexture(Texture* tex);	//设置地图背景
//protected:
//	virtual	VOID					OnCreateMap();//创建地图
//	float2							m_trot,m_rot;
//	Size							m_gridSize;	//格子大小
//	CDynamicArray<Pixel>			m_tilesIds;	//格子渲染顺序
//	Texture*					m_textureMap;//地图背景
//	Rect							m_mapRect;//地图格子区域
//	Rect							m_renderRect;//渲染区域
//};
#pragma pack(push,1)
struct PathPoint{
	short	gx,gy;
	enum pp_type{
		type_none = 0,
		type_enable = 1,
		type_door = 2,
	};
	short	type;
	unsigned short objid;
};
#pragma pack(pop)
struct MapItemData;
class	Map2D_2x1;
#define	ItemType_2x1	1
#define	ItemType_Unit	2
class	MapItem_2x1: public MapItem
{
public:
	MapItem_2x1(Map2D_2x1* m, const MapItemData* d);
	MapItem_2x1(Map2D_2x1* m, BOOL bPush = true);
	~MapItem_2x1();
	BOOL					FrameMove(const Pixel& pixel);//必须要计算当前要渲染的矩形
	VOID					Render();	//渲染到这个位置上,像素点
	VOID					SetTexture(const PtrID& texID, const Rect& textureRect, const Pixel& center);
	int						GetItemType(){return ItemType_2x1;}
	VOID					Save(CSafeFileHelperW& w);
	VOID					Load(CSafeFileHelperR& r);
	BOOL					PickPixel(int pixelX, int pixelY);//像素级别点击这个对象,设置屏幕像素点
	virtual	VOID			SetPos(const Pixel& pos);
	//
	VOID					AddEvent(int id, LPCSTR szFunc);
	//
	VOID					AddDrawMgr(UIDrawMgr* mgr);
	VOID					RenderAllMgrs();
	INT						m_bRenderMode;
	BOOL					m_bPush;
public:
	Pixel					m_centerPos;	//中心点
	Rect					m_textureRc;
	int						m_scale;
	int						m_downScale;
	int						m_type;
	int						m_runtimeid;
public://runtime data
	Map2D_2x1*				m_mapPtr;
	std::vector<UILuaEvent>	m_events;
	std::vector<UIDrawMgr*> m_binds;
};
class	WayPoints;
struct	MapUnit_Data;



class	MapItem_Unit: public MapItem_2x1
{
public:
	MapItem_Unit(Map2D_2x1* m, MapUnit_Data* data);
	~MapItem_Unit();
	VOID					OnUnitFrameMove();//必须要计算当前要渲染的矩形
	BOOL					FrameMove(const Pixel& pixel);//必须要计算当前要渲染的矩形
	VOID					Render();	//渲染到这个位置上,像素点
	//
	virtual	VOID			SetPos(const Pixel& pos);
	VOID					SetActionByIndex(int index);//播放动作
	VOID					SetActionByName(const char* action);//播放动作
	VOID					SetAngleDir(const Pixel& dir);//设置转向
	VOID					SetAngle(float a);
	BOOL					AddMoveTo(int mapx, int mapy);//自动寻路，到地图目标点
	BOOL					AddMoveTo(int mapx, int mapy, LPCSTR szFunc);//自动寻路，到地图目标点，带参数
	VOID					AddStandTo(float fTime);//添加静止
	int						GetItemType(){return ItemType_Unit;}
	LPCSTR					GetActionName();
public:
	GridIndex				m_last;
	float					m_fSpeed;
	float					m_fActionSpeed;
	int						m_currentAction;
	float					m_currentAni;
	int						m_currentAngle;
	Pixel					m_currentDir;
	CDynamicArray<WayPoint2D>	m_ways;
	Name					m_moveEndFunc;//移动结束之后函数
	float					m_fPlayLength;
	int						m_nPlayIndex;
	MapUnit_Data*			m_unitPtr;
};

struct	MapUnitAction
{
	FileName				textureFile;
	int						w,h;
	Name					name;//动作名称
	struct TexIndex{
		short	begin,end;
		void				set(short _b, short _e){begin = _b; end = _e;}
	};
	TexIndex				ani[Unit2DAngle_Count];//x=begin,y=end
	PtrID					texid;
	Pixel					center;//中心点坐标
};
struct	MapUnit_Data
{
	int							id;
	Name						name;
	int							type;//自定义类型
	int							layer;//对应层
	CDynamicArray<MapUnitAction>	actions;
	int							action_move;
	int							action_stand;
};

struct	MapTexture
{
	Texture*		tex;
	Rect			destRect;
	Rect			textureRect;
};
struct PathIndex{
	short x,y;
};

class Map2DAstar: public AstarCallback
{
public:
	virtual	int				IsAstarMoveEnable(int x, int y);
	virtual	int				GetAstarWidth();
	virtual	int				GetAstarHeight();
	Map2D_2x1*				m_mapPtr;
};

//事件类
class	Map2D_Event: public Base
{
public:
	Map2D_Event(Map2D_2x1* ptr);
	~Map2D_Event();
	VOID						SetPos(const Pixel& pos);
public:
	int							m_id;
	Name						m_name;
	int							m_type;
	GridIndex					m_grid;
	Pixel						m_pos;//位置
	Map2D_2x1*					m_mapPtr;
	//CDynamicArray<EventInvoke>	m_invokes;
};

class Map2D_2x1: public MapBase//斜角非规则分布
{
public:
	Map2D_2x1();
	virtual ~Map2D_2x1();
	inline	Size&					GetGridSize(){return m_gridSize;}
	VOID							SetGridSize(const Size& s);//格子大小，需要根据图片大小来设定
	virtual	VOID					Destroy();
public://必须继承的函数
	Pixel							GetPixelGridCenter(int gridX, int gridY);
	Pixel							GetMapGridCenter(int gridX, int gridY);
	virtual	VOID					RenderTexture(Texture* tex, const Rect& rc, const Pixel& pixel, const color4* c = 0, int offsetX = 0, int offsetY = 0);
	virtual	VOID					RenderTexture2(Texture* tex, const Rect& rc, const Pixel& pixel, const color4* c = 0, int offsetX = 0, int offsetY = 0, int drawMode = 0);
	virtual	VOID					Render();//渲染
	VOID							AddMap(Texture* tex);	//设置地图背景
	VOID							RemoveMap(int nIndex);//删除一个图片
	VOID							SetMapRect(int nIndex, const Rect& renderRect, const Rect& textureRect);//渲染位置
	//
	//const MapGrid_2x1*				GetGrid_2x1(int nGridX, int nGridY, int layer = 0);//
	const MapItemData*				GetGrid_Item(int nGridX, int nGridY, int layer = 0);//
	//Size							GetBoxSize(){return Size(m_gridSize.cx, m_gridSize.cy);}
	//
	VOID							SetAllMove(INT bMove);
	VOID							SetTextureAllMove(MapGrid_2x1& t, INT bMove);
	inline	const MapTexture*		GetMap(int nIndex){if(nIndex < 0 || nIndex >= m_textureMaps.size())return 0; return &m_textureMaps[nIndex];}
	inline	int						GetMapCount(){return m_textureMaps.size();}
	//
	const Size&						GetTextureExtend(){return m_textureExtend;}
	VOID							SetTextureExtend(const Size& s){m_textureExtend = s;}
	BOOL							IsItemIn(int pixelX, int pixelY, GridIndex& itemGrid, MapItem* item);//对象是否被点击
	//BOOL							PickBox(GridIndex& ret, int pixelX, int pixelY);//点击格子,当前屏幕所在位置开始计算
	BOOL							PickGrid(GridIndex& ret, int pixelX, int pixelY);//点击格子,当前屏幕所在位置开始计算
	MapItem*						PickItem(int pixelX, int pixelY, int layer = 0);//点击格子内的地形对象,当前屏幕所在位置开始计算
	MapItem*						PickItemPixel(int pixelX, int pixelY, int layer = 0);//点击格子内的地形对象,当前屏幕所在位置开始计算
	BOOL							GetGridInMapPoint(GridIndex& ret, int x, int y);//整个地形开始点开始计算
	//
	VOID							MapToWindow(Rect& windowRect, const Rect& mapRect);
	VOID							WindowToMap(Rect& mapRect, const Rect& windowRect);
	MapItem_Unit*					AddUnit(int unitid, const GridIndex& g);//创建一个角色
	MapItem_Unit*					AddUnit(LPCSTR szUnitName, const GridIndex& g);//创建一个角色
	VOID							DeleteUnit(MapItem_Unit* unit);//创建一个角色
	VOID							DeleteFromUnitID(int unitid);//删除所有对应的角色
	//
	VOID							RecalcGridStates();//重新计算所有的可走格子信息
	BOOL							IsItemCanInsert(int gridX, int gridY, MapItem* item);
	VOID							RenderDrawGrid(int gridX, int gridY, MapItem* item, const color4* color);
	VOID							RenderOneGrid(int gridX, int gridY, const color4* color);
	VOID							SetWhiteTexture(const char* szWhiteTexture);
	const Pixel&					GetMapOffset(){return m_mapOffset;}
	VOID							SetMapOffset(const Pixel& p){m_mapOffset = p;}
	//
	const Rect&						GetRenderRect(){return m_renderRect;}
	//
	VOID							RemoveAllItems();
	VOID							CommitAllItems(const MapItemData* data);//刷新所有的对象数据的变化
	int								GetCreateCount(int id);
	Name							m_funcClick;
	//寻路工具
	VOID							SetPathAlign(const GridIndex& g);//设置可移动格子初始格子位置
	VOID							SetGridMoveEnable(int gridx, int gridy, int state);//设置可走格子属性
	INT								GetGridMoveEnable(int gridx, int gridy);
	BOOL							PathFromGrid(PathIndex& ret, const GridIndex& g);
	BOOL							PathFromPixel(PathIndex& ret, int pixelX, int pixelY);
	BOOL							PathFromMap(PathIndex& ret, int mapX, int mapY);
	Pixel							MapPointFromPixel(const Pixel& p){return Pixel(p.x+m_renderRect.left,p.y+m_renderRect.top);}
	//
	BOOL							FindPath(CDynamicArray<WayPoint2D>& ways, const Pixel& mapFrom, const Pixel& mapTo);
	BOOL							GridFromPath(GridIndex& ret, const PathIndex& p);
	VOID							SetPathSize(int w,int h);//设置可移动格子大小
	VOID							RecalcMoveStates();
	Size							m_pathSize;
	CDynamicArray<PathPoint>		m_pathPoints;//可移动格子设置
	GridIndex						m_pathalign;
	//
	Map2D_Event*					AddEvent(const char* name, const Pixel& mapPos);//
	VOID							RemovePoint(Map2D_Event* e);
	Map2D_Event*					SearchEvent(const char* name);
	CDynamicArray<Map2D_Event*>		m_events;
public:
	BOOL							m_bShowFloor;//显示地板
	BOOL							m_bShowBuilding;//显示建筑物
	BOOL							m_bShowBackTexture;//显示背景图片
	BOOL							m_bShowUnits;
	BOOL							m_bWhiteGrid;//是否渲染白色格子
	BOOL							m_bWhiteCanMove;//是否显示可走格子
	color4							m_colorWhiteGrid;//白板颜色
	//
	CDynamicArray<MapItemData>		m_projItems;//对象方案列表
	CDynamicArray<MapGrid_2x1>		m_projGrids;//格子图片方案
	CDynamicArray<MapUnit_Data*>	m_projUnits;//单位方案列表
	MapItem_2x1*					m_downItem;
	AStarPath						m_astar;
	Map2DAstar						m_astarcb;
public:
	virtual	BOOL					LoadR(CSafeFileHelperR& r);
	virtual	VOID					SaveW(CSafeFileHelperW& w);
	virtual	MapItem*				CreateMapItem(int type);	//根据类型创建对象
	//
	VOID							PushItem(MapItem_2x1* item);
	VOID							PopItem(MapItem_2x1* item);
	CDynamicArray<MapItem_2x1*>		m_items;
	CDynamicArray<MapItem_Unit*>	m_units;
protected:
	virtual	VOID					OnCreateMap();//创建地图
	Size							m_gridSize;	//格子大小
	CDynamicArray<MapTexture>		m_textureMaps;//地图背景
	Texture*						m_textureWhite;//地图背景
	Pixel							m_mapOffset;//地图格子区域
	Rect							m_renderRect;//渲染区域
	Size							m_textureExtend;
	friend class					MapItem_2x1;
	friend class					MapItem_Unit;
};

class	MyMap2D: public Map2D_2x1
{
public:
	MyMap2D();
	~MyMap2D();
	virtual	BOOL			OnInputEvent(Phantom::InputEventID evt, int x, int y, unsigned int touchIndex);
	virtual	VOID			OnRender(double dElapsedTime);
	BOOL					m_bVisible;
	BOOL					m_bEnableMove;
	BOOL					m_bIsButtonDown;
	BOOL					m_bMouseMove;
	Pixel					m_downPos;
public:
};

};

#endif
