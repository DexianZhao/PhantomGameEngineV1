//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
	Design Writer :   赵德贤 Dexian Zhao
	Email: yuzhou_995@hotmail.com
	 
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOMSCENEDEFINE3D_H__
#define __PHANTOMSCENEDEFINE3D_H__
#include "PhantomBase.h"
#include "PhantomUnit3D.h"
#include "PhantomCamera.h"

#define	MAX_DETAIL_TEXTURE			8//细节图片
#define	MAX_LAYER_TEXTURE			MAX_DETAIL_TEXTURE//一个场景内最大图片数量(MAX_DETAIL_TEXTURE + 1)//加三张混合图片
//
#define	MAX_SCENE_TEXTURE			512

//#define	MAX_REGION_WIDTH_COUNT	512	//区域块最多, 不能太大，否则会占用过多内存
//#define	MAX_REGION_ALLOC		(MAX_REGION_WIDTH_COUNT * MAX_REGION_WIDTH_COUNT)

#define	MAX_REGION_LEVEL			12
#define	MAX_GRASS_TEXTURES			255	//草地图片最大数量

#define	GRID_GRASS_WIDTH			6//一个格子细分数量
#define	MAX_GRID_GRASS_COUNT		(GRID_GRASS_WIDTH*GRID_GRASS_WIDTH)	//一个格子草地最多多少?

#define	REGION_GRID_COUNT_25d		16	//每一个区域固定格子数量
#define	REGION_FULLFACE_COUNT_25d	(REGION_GRID_COUNT_25d*REGION_GRID_COUNT_25d*2)//每一个区域固定格子数量
#define	REGION_VERTEX_COUNT_25d		(REGION_GRID_COUNT_25d + 1)//每一个区域固定格子数量
#define	REGION_FULL_VERTICE_25d		(REGION_VERTEX_COUNT_25d * REGION_VERTEX_COUNT_25d)
#define	REGION_GULLGRID_COUNT_25d	(REGION_GRID_COUNT_25d*REGION_GRID_COUNT_25d)

#define	MAX_SHADOWMAP_LEVEL			1	//阴影贴图最大层次
#define	GRASS_INFO_COUNT			(REGION_GRID_COUNT_25d*REGION_GRID_COUNT_25d)

namespace Phantom{


#pragma pack(push, 1)

struct	SceneVertices25d
{
	float3								p;
	float3								n;
	float2								uv;
};
struct	SceneObjectDB{
	double					val;//
	char*					textPtr;//如果不是空则表示有数据在
	SceneObjectDB(){val=0; textPtr = 0;}
	~SceneObjectDB(){if(textPtr)delete textPtr;}
};
struct	SceneMoveGrids
{
	float3								p;
	unsigned int						color;
};

struct	SceneObjectFile
{
	FileName				name;
	int						id;
};

struct LayerAttribute
{
	short							textureID;
	float							uvPositionX;
	float							uvPositionY;
	float							uvScaleX;
	float							uvScaleY;
	float							uvRotate;
};

struct	SceneVertexAttribute		//格子的属性
{
	float3						position;
	float3						normal;
	unsigned char				indexX, indexY;
	unsigned char				rx1, ry1;
	unsigned char				x1, y1;
	unsigned char				x2, y2;
	unsigned int				diffuse;
	float						blend1,blend2;
};

struct RegionXY
{
	unsigned short	x, y;
};

#define	CHILD_GRID_COUNT		4
struct	GVertexData
{
	short						height;								//高度值，按照1点为1厘米计算，最高3千米最低-3千米
	inline	float				GetHeight(){return (float)height * 0.01f;}
	inline	VOID				SetHeight(float fHeight){height = (int)(fHeight * 100.0f)%30000;}
	inline	bool	operator	==	(const GVertexData& v){return (v.height == height);}
};
#define	MAX_REGION_COUNT		8
struct	GMoveData{
	enum{
		MOVE_ENABLE	=	0x10,
	};
	unsigned char				m;
	inline	bool				isMoveEnable(){return (m&MOVE_ENABLE)==MOVE_ENABLE;}
	inline	void				setMoveEnable(BOOL b){
		if(b)
			m |= MOVE_ENABLE;
		else if(m&MOVE_ENABLE)
			m^=MOVE_ENABLE;
	}
	inline	char				GetRegionType(){return (m%8);}//8以下都是地区类型索引总共可以存在7个地块
	inline	void				SetRegionType(char index){
		if(index<0)index=0;
		else if(index>=8)index = 7;
		m = index | (m&0xf0);
	}
};

struct	GridAttr25d
{
	unsigned char				texid;
	unsigned char				hide;		//属性，是否显示，是否可行走等，是否分割
	bool						isVisible()				{return !hide;}
	void						SetVisible(bool b)		{hide = !b;}
	unsigned char				r,g,b;
	inline	VOID				SetRGB(unsigned char _r, unsigned char _g, unsigned char _b)
	{
		r	=	_r;
		g	=	_g;
		b	=	_b;
	}
	unsigned char				fullindex;
	//unsigned char				tex[MAX_DETAIL_TEXTURE];
	//unsigned char				blend[MAX_DETAIL_TEXTURE];
};
	struct	GridAttributeT
	{
		unsigned char				texid;
		unsigned char				visible;		//属性，是否显示，是否可行走等，是否分割
		unsigned char				r,g,b;
		//unsigned char				fullindex;
		unsigned char				tex[MAX_DETAIL_TEXTURE];
		unsigned char				blend[MAX_DETAIL_TEXTURE];
	};
struct GridAttributeRuntime
{
	unsigned char				beginX, beginY;
};

struct	SceneTextureData
{
	char						minFilter;
	char						magFilter;
	char						mipFilter;
	float						mipMapLodBias;
	char						bMoveEnable;//是否可移动格子
};

struct	SceneTextureRuntime: SceneTextureData
{
	FileName					fileName;
	Texture*					texturePtr;
	char						bExist;
	char						bIsMask;//是否8个格子计算
};

struct	SkyFlareInfo
{
	FileName		textureFile;
	color4			color;//一个光圈的颜色
	float			fRadius;//一个光圈的大小
	float			fPosition;//位置参数
	bool			bIsSun;//是否太阳贴图
};

#define	MAX_FLARE_COUNT		10
struct	SceneSkyData
{
	int				id;
	Name			name;
	BOOL			bEnable;
	FileName		file;
	FileName		flareFile;
	unsigned int	skyColor;
	float			fHeight;
	float			fAngleX;
	float			fAngleZ;
	unsigned int	ambientColor;
	//
	float			fWindDirection;
	float			fWindPower;
	int				flareCount;
	BOOL			bFlareEnable;
	SkyFlareInfo	flares[MAX_FLARE_COUNT];
	SceneSkyData(){memset(this, 0, sizeof(SceneSkyData)); ambientColor = 0xff3f3f3f; fWindDirection = 0; fWindPower = 1.0f;}
};

struct	SceneFogDataOld
{
	int				id;
	NameT<32>		name;
	unsigned char	fogType;
	unsigned int	fogColor;
	float			fNear;
	float			fFar;
	SceneFogDataOld(){memset(this, 0, sizeof(SceneFogDataOld));}
};

struct	SceneCameraData: CameraDataLocal
{
	int				id;
	Name			name;
	SceneCameraData(){memset(this, 0, sizeof(SceneCameraData));}
};

struct	SceneRegionData{
	Name			name;
	char			bCustomName;//是否自定义名字
	short			regionid;//在表格中的地区id
};

struct SceneGridTriangles
{
	VOID							CreateGridFaces();	//创建对其三角形列表
	//
	enum{
		TRI_SIDE_TOP	=	0,	//顶部索引
		TRI_SIDE_RIGHT,			//右边索引
		TRI_SIDE_BOTTOM,		//底部索引
		TRI_SIDE_LEFT,			//左边索引
		TRI_SIDE_COUNT,
		//
		TRI_SIDE_LEFTTOP	=	0,
		TRI_SIDE_RIGHTTOP	=	1,
		TRI_SIDE_LEFTBOTTOM	=	2,
		TRI_SIDE_RIGHT_BOTTOM	=	3,
	};
	Triangle						tri[2];
	Triangle						split[TRI_SIDE_COUNT];//两个切割的三角形
	unsigned char					quad_index[TRI_SIDE_COUNT];//四边临界区格子的三角形索引,tri[0],tri[1],0和1值
	/*
	\  0 /			     /		\   	|------|-----|
	 \  /  			    / 		 \    	|	0  |  1	 |
	  \/  			0  /  		  \  0	|______|_____|
	3 /\ 1			  /  1		1  \ 	|	   |	 |
     /  \   		 /	  		    \   |	2  |  3	 |
    /  2 \  	    /	  		     \  |------|-----|
	*/
	//
	unsigned char					x,y;//单位1格子，最高层值64
	unsigned char					width;//宽度
	int								alloc_index;
	char							level;
	char							gridWidth;
	//
	SceneGridTriangles*				childPtr[TRI_SIDE_COUNT];//4个子格子
	SceneGridTriangles*				parentPtr;//父格子
};

struct GrassInfo
{
	unsigned char					x,y;	//xy位置
	char							offsetX, offsetY;
	float							height;	//高度位移
	unsigned int					id;
	unsigned char					rnd;
	unsigned char					index;	//贴图类型
	enum GrassFlags
	{
		GrassFlags_Visibled			=	0x0001,//曾经显示过
	};
	unsigned short					flags;	//标识符
};

struct	GridGrassInfo
{
	GrassInfo						list[MAX_GRID_GRASS_COUNT];
	unsigned char					grassCount;
	bool							bInit;
	GridGrassInfo(){grassCount = 0; bInit = false; memset(list, 0, sizeof(list));}
};

#pragma pack(pop)

};

#endif

