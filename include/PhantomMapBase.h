#ifndef __PHANTOM_MAPBASE_H__
#define	__PHANTOM_MAPBASE_H__
#include "PhantomManager.h"

namespace Phantom{

#pragma pack(push, 1)
#define	MAX_ITEM_INMAP_SIZE			256//对象最大占格子数量，不会超过这个数量，也就是16x16格子数量大小
struct	MapItemData
{
	int									id;
	Name								name;
	FileName							textureFile;
	Rect								textureRect;//图片区域
	int									scale;//场景中的大小缩放,100=1倍缩放,0=不使用
	int									gridCount;//占格子数量，1，2，3，4，5，6//6=占6x6个格子
	Pixel								center;//中心点坐标
	int									layer;//对应的层
	int									type;//自定义类型
	Size								offset;
	int									alignBottom;//按照图下角对齐
	Pixel								door;//门的位置
	BOOL								bDoorExist;//是否存在门
};
struct	MapGrid_2x1
{
	int									id;
	Name								name;
	FileName							textureFile;
	Rect								textureRect;//图片区域
	int									layer;//对应的层
	int									offsetX;//缩放
	int									type;//自定义类型
	int									offsetY;//缩放
	int									drawMode;//渲染模式
};

#pragma pack(pop)

#define	MAX_GRID_LAYER				3	//一个格子最大3个层
struct	GridIndex
{
	short		x,y;
	inline	void		Set(short _x, short _y){x = _x; y = _y;}
	inline	bool		exist(){return (x>0&&y>0);}
	GridIndex(short _x, short _y){x = _x; y = _y;}
	GridIndex(){x = y = -1;}
};
class MapBase;
class	MapItem: public Base
{
public:
	ImplementComValue();
	MapItem();
	virtual ~MapItem(){}
	inline	const GridIndex&		GetGridInMap(){return m_inMap;}//返回在地图中所处的某个格子位置
	inline	int						GetGridCount(){return m_gridCount;}
	color4							m_color;
	int								m_projid;
	Rect							m_itemRect;
	PtrID							m_texID;//图片索引
	Size							m_offset;
	BOOL							m_bEnabled;
	BOOL							m_bVisible;
	Name							m_name;
	int								m_alignBottom;
	//
	BOOL							m_bDoorExist;//是否存在门
	Pixel							m_door;
	Pixel							m_pos;
public://必须继承的函数
	virtual	int						GetItemType() = 0;
	virtual	BOOL					FrameMove(const Pixel& pixel) = 0;//必须要计算当前要渲染的矩形
	virtual	VOID					Render(){}	//渲染到这个位置上,像素点
	virtual	VOID					Save(CSafeFileHelperW& w){}
	virtual	VOID					Load(CSafeFileHelperR& r){}
	MapItem*						m_next;
protected:
	int								m_gridCount;
protected:
	GridIndex						m_inMap;//上一次的位置
	friend class MapBase;
	friend class Map2D_2x1;
};

#define	MAX_GRID_TEXTURE_LAYER		2
struct	MapGrid
{
	MapItem*						item[MAX_GRID_LAYER];
	MapItem*						units[MAX_GRID_LAYER];//单位数据
	int								data;
	enum GridState{
		GridState_Normal = 0,
		GridState_DisableMove,
	};
	GridIndex						state;//格子状态，是否可走等GridState
	struct	MapGridTexture
	{
		int							id;//格子id
		Rect						textureRc;//图片区域
		PtrID						texID;
		short						offsetX,offsetY;//100=1倍
		short						drawMode;//0=剧中对齐，1=左上角对齐
	};
	char							moveEnable;//是否可以移动到,-1不可见或无效
	int								pathIndex;//动态数据
	MapGridTexture					tex[MAX_GRID_TEXTURE_LAYER];
	//
	VOID							AddUnit(MapItem* item, int layer);
	BOOL							RemoveUnit(MapItem* item, int layer);
};

class	MapBase: public Base
{
public:
	MapBase();
	virtual ~MapBase();
	ImplementComValue();
	virtual	VOID					Create(Size& mapSize/*地图像素大小*/, Size& tiledCount/*地图叠加数量*/);	//创建一个地图
	virtual	VOID					Destroy();
	//如果不能被插入则返回false，仅仅对nLayer层级是0的有效，0层级作为贴地表对象而计算其格子是否有效
	virtual	BOOL					SetGridItem(int nGridX, int nGridY, MapItem* item, int nLayer = 0);//设置某个格子的对象，层级最大3层，如果这个物体在最新的位置上则会自动从上一次的位置中删除这个对象，如果item是0则把原来的对象也删除
	virtual	VOID					RemoveGridItem(MapItem* item);	//把这个对象从格子队列中去掉
	virtual	MapItem*				GetGridItem(int nGridX, int nGridY, int nLayer = 0);//返回某个各自的对象，层级最大3层
	//
	virtual	BOOL					SetUnit(int nGridX, int nGridY, MapItem* item, int nLayer = 0);//设置某个格子的对象，层级最大3层，如果这个物体在最新的位置上则会自动从上一次的位置中删除这个对象，如果item是0则把原来的对象也删除
	virtual	VOID					RemoveUnit(MapItem* item);	//把这个对象从格子队列中去掉
	//
	virtual	VOID					SetGridTexture(int nGridX, int nGridY, int id, const PtrID* texID, const Rect* textureRect = 0, int offsetX = 0, int layer = 0, int offsetY = 0, int drawMode = 0);//设置某个格子的图片,如果是0则会去掉原来的图片
	virtual	VOID					SetGridTexture(int nGridX, int nGridY, const MapGrid_2x1* m);//设置某个格子的图片,如果是0则会去掉原来的图片
	int								GetGridTextureID(int nGridX, int nGridY, int layer = 0);//
	virtual	VOID					SetGridData(int nGridX, int nGridY, int data);//设置某个格子的用户数据
	virtual	INT						GetGridData(int nGridX, int nGridY);	//返回某个格子的用户数据
	virtual	VOID					SetCameraPos(const Pixel& center);	//设置摄像头
	const Pixel&					GetCameraPos(){return m_cameraPos;}	//返回摄像头位置
	inline	const Size&				GetMapPixelSize(){return m_mapPixelSize;}
	VOID							SetMapSize(int width, int height);
	VOID							SetOffsetAll(int x, int y);//全部整体偏移
	inline	const Size&				GetTileCount(){return m_tileCount;}
	//inline	const Size&				GetBoxTile(){return m_tileCount;}//Size(m_tileCount.cx*2, m_tileCount.cy/2);}
	virtual	BOOL					IsItemCanInsert(int gridX, int gridY, MapItem* item){return true;}//在这个位置是否可以加入对象
	virtual	VOID					SetTileCount(const Size& size);//设置格子数量
	//
	VOID							Save(const char* szFileName);
	BOOL							Load(const char* szFileName);//加载
	virtual	BOOL					LoadR(CSafeFileHelperR& r);
	virtual	VOID					SaveW(CSafeFileHelperW& w);
	FileName						m_fileName;
	PtrID							GetTexture(const char* szFileName);
	Texture*						GetTexturePtr(PtrID& id);
	inline	int						GetNewID(){return (m_autoid++);}
public://必须继承的函数
	virtual	VOID					RenderTexture(Texture* tex, const Rect& rc, const Pixel& pixel, const color4* c = 0, int offsetX = 0, int offsetY = 0) = 0;
	virtual	VOID					Render() = 0;//渲染
	virtual	MapItem*				CreateMapItem(int type) = 0;	//根据类型创建对象
protected:
	virtual	VOID					OnCreateMap(){};//创建地图
protected:
	struct MapTextureData
	{
		int							id;
		FileName					file;
		Texture*					tex;
		MapTextureData(){tex = 0;}
	};
	int								m_autoid;
	CDynamicArray<MapTextureData>	m_textures;
	CDynamicArray<MapGrid>			m_mapGrids;
	//CDynamicArray<MapBoxTexture>	m_mapBoxTextures;
	Size							m_mapPixelSize;
	Size							m_tileCount;
	Pixel							m_cameraPos;
};

};

#endif
