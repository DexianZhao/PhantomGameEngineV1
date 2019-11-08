//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   PhantomAPI.h
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------
API接口回调类

-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___PHANTOMAPI__H___
#define ___PHANTOMAPI__H___
/*IMPORT*/#include "../include/PhantomEnum.h"
/*IMPORT*/#include "../include/PhantomBase.h"
/*IMPORT*/#include "../include/PhantomMath.h"
/*IMPORT*/#include "../include/PhantomCamera.h"
#include "PhantomDevice.h"
#include "PhantomFont.h"
#include "PhantomUIManager.h"
#include "PhantomDB.h"

#ifndef WINAPI
#define WINAPI
#endif


/*CLASS Create(float2,float2_,二维向量_,struct, true)*/
/*CLASS Create(float3,float3_,三维向量_,struct, true)*/
/*CLASS Create(float4,float4_,四维向量_,struct, true)*/
/*CLASS Create(color4,color4_,颜色_,struct, true)*/
/*CLASS Create(quaternion,quaternion_,四元数_,struct, true)*/
/*CLASS Create(Plane,Plane_,平面_,struct, true)*/
/*CLASS Create(matrix4x4,matrix4x4_,矩阵_,struct, true)*/
/*CLASS Create(Pixel,PPoint_,屏幕坐标数据_,struct, true)*/
/*CLASS Create(PixelColor,PPointColor_,屏幕坐标颜色_,struct, true)*/
/*CLASS Create(Rect,RectangleData_,矩形数据_,struct, true)*/

/*CLASS Create(IMeshMaterialPtr,MeshMaterial_,子模型材质_,interface)*/
/*CLASS Create(ISubMeshPtr,SubMesh_,子模型_,interface)*/
/*CLASS Create(IMeshActionPtr,MeshAction_,模型动作_,interface)*/
/*CLASS Create(IMeshPtr,Mesh_,模型_,interface, true)*/
/*CLASS Create(ITexturePtr,Texture_,纹理_,interface, true)*/
/*CLASS Create(IUIDrawMgrPtr,UIDrawMgr_,渲染2D_,interface, true)*/
/*CLASS Create(ITableReaderPtr,Table_,表格数据_,interface, true)*/
/*CLASS Create(IElementPtr,UIElement_,界面元素_,interface)*/
/*CLASS Create(IControlPtr,UIControl_,界面控件_,interface)*/
/*CLASS Create(IUIGroupPtr,UIGroup_,界面控件组_,interface)*/
/*CLASS Create(IDialogPtr,UIDialog_,界面对话框_,interface, true)*/
/*CLASS Create(IFindWayPtr,FindWay_,寻路_,interface)*/
/*CLASS Create(IDataControlPtr,DataControl_,数据控制_,interface)*/
/*CLASS Create(IDataArrayPtr,DataArray_,数据数组_,interface)*/
/*CLASS Create(IMap3DEnvPtr,Map3DEnv_,场景环境3D_,interface)*/
/*CLASS Create(IMap3DPtr,Map3D_,地图3D_,interface)*/
/*CLASS Create(IMap3DItemPtr,Map3DItem_,地图对象3D_,interface)*/
/*CLASS Create(IMap3DPointPtr,Map3DPoint_,地图移动点3D_,interface)*/
/*CLASS Create(IMap3DSkillPtr,Map3DSkill_,地图技能3D_,interface)*/
/*CLASS Create(IGameObjectPtr,GameObject_,游戏对象_,interface, true)*/
//
/*CLASS Create(Camera,Camera_,镜头_,module, true)*/
/*CLASS Create(System,System_,系统_,module, true)*/
/*CLASS Create(GameMgr,GameMgr_,游戏_,module, true)*/
/*CLASS Create(Global,Global_,全局_,module, true)*/
/*CLASS Create(MyGlobal,MyGlobal_,我的全局_,module, true)*/

namespace Phantom{

#pragma pack(push,1)
/*STRUCT*/struct	Object/*对象指针*/
{
	short			s1;
	short			s2;
	int				i1;
	int				i2;
	Object(){s1=s2=i1=i2=0;}
};/*END_STRUCT*/

/*STRUCT*/struct	PixelUV/*屏幕坐标数据UV*/
{
	int		x/*左*/;
	int		y/*上*/;
	float	u/*贴图坐标u*/;
	float	v/*贴图坐标v*/;
	color4	c/*颜色*/;
	/*END_STRUCT*/
};

/*STRUCT*/struct	floatPtr/*小数型数据*/
{
	float				value/*值*/;
};/*END_STRUCT*/

/*ENUM*/enum EnumInterfaces/*接口类型*/
{
	ISubMeshPtr_Class/*接口类型_子模型*/				=	1,
	IMeshMaterialPtr_Class/*接口类型_子模型材质*/		=	2,
	IMeshActionPtr_Class/*接口类型_模型数据*/			=	3,
	IMeshPtr_Class/*接口类型_模型*/						=	4,
	ITexturePtr_Class/*接口类型_纹理*/					=	6,
	//
	ITimePtr_Class/*接口类型_时间点*/					=	7,
	IAffectPtr_Class/*接口类型_影响器*/					=	8,
	IEmissionProjectPtr_Class/*接口类型_喷射器方案*/	=	9,
	IEffectProjectPtr_Class/*接口类型_特效方案*/		=	10,
	IEmissionPtr_Class/*接口类型_喷射器*/				=	11,
	IEffectPtr_Class/*接口类型_特效*/					=	12,
	IMusic3DPtr_Class/*接口类型_音乐3D*/				=	13,
	ISound3DPtr_Class/*接口类型_声效3D*/				=	14,
	IMusic2DPtr_Class/*接口类型_音乐播放器*/			=	16,
	IVertexElementPtr_Class/*接口类型_顶点元素*/		=	19,
	IMyMeshPtr_Class/*接口类型_动态模型*/				=	20,
	IGameObjectPtr_Class/*接口类型_游戏对象*/			=	21,
	ITableReaderPtr_Class/*接口类型_表格读取*/			=	24,
	IVideoPlayerPtr_Class/*接口类型_视频播放*/			=	25,
	IVideoWriterPtr_Class/*接口类型_视频数据*/			=	26,
	IVideoWriter3DPtr_Class/*接口类型_视频3D*/			=	27,
	IElementPtr_Class/*接口类型_界面元素*/				=	28,
	IUIAnimationPtr_Class/*接口类型_界面动画*/			=	29,
	IComboBoxPtr_Class/*接口类型_组合框*/				=	30,
	IEditBoxPtr_Class/*接口类型_编辑框*/				=	31,
	IFormatTextPtr_Class/*接口类型_格式化文本*/			=	32,
	IListBoxPtr_Class/*接口类型_列表框*/				=	33,
	IScrollBarPtr_Class/*接口类型_滚动条*/				=	34,
	IControlPtr_Class/*接口类型_控件*/					=	35,
	IDialogPtr_Class/*接口类型_对话框*/					=	36,
	//
	INetServerPtr_Class/*接口类型_服务器端*/			=	38,
	INetClientPtr_Class/*接口类型_客户端*/				=	39,
	INetEventPtr_Class/*接口类型_事件*/					=	50,
	IIntersectPtr_Class/*接口类型_碰撞指针*/			=	51,
	ILogicLinePtr_Class/*接口类型_逻辑数据行*/			=	52,
	ILogicTablePtr_Class/*接口类型_逻辑数据表*/			=	53,
	IPlotStatePtr_Class/*接口类型_剧情状态类*/			=	54,
	IPlotVariantPtr_Class/*接口类型_剧情变量类*/		=	55,
	IPlotActionPtr_Class/*接口类型_剧情动作类*/			=	56,
	IPlotDramaPtr_Class/*接口类型_剧情篇章类*/			=	57,
	IPlotChapterPtr_Class/*接口类型_剧情章节类*/		=	58,
	IPlotProjectPtr_Class/*接口类型_剧情项目类*/		=	59,
	ICursorPtr_Class/*接口类型_光标类*/					=	60,
	IPlotModulePtr_Class/*接口类型_剧情模块类*/			=	61,
	IListCtrlPtr_Class/*接口类型_行列框*/				=	62,
	IUnitPtr_Class/*接口类型_场景单位*/					=	63,
	IScene2DPtr_Class/*接口类型_场景2D*/				=	64,
	ITerrainPtr_Class/*接口类型_场景地形*/				=	65,
	ISliderPtr_Class/*接口类型_拖动条*/					=	66,
	IProgressPtr_Class/*接口类型_进度条*/				=	67,
	ICheckBoxPtr_Class/*接口类型_单选框*/				=	68,
	IMap3DItemPtr_Class/*接口类型_场景对象3D*/			=	69,
	IMap3DPtr_Class/*接口类型_场景3D*/					=	70,
	ISkyPtr_Class/*接口类型_场景天空3D*/				=	71,
	IMap3DEnvPtr_Class/*接口类型_场景环境3D*/			=	72,
	IUnit3DPtr_Class/*接口类型_单位3D*/					=	73,
	IUnit3DActionPtr_Class/*接口类型_单位动作3D*/		=	74,
	//
	INetWriterPtr_Class/*接口类型_网络数据包写入*/		=	75,
	INetReaderPtr_Class/*接口类型_网络数据包读取*/		=	76,
	IUnit3DElementPtr_Class/*接口类型_单位动作元素3D*/	=	77,
	IUnit3DMeshPtr_Class/*接口类型_单位模型3D*/			=	78,
	ILogicDataPtr_Class/*接口类型_逻辑数据系统*/		=	79,
	//
	IUIGroupPtr_Class/*接口类型_界面控件组*/			=	80,
	IFindWayPtr_Class/*接口类型_寻路接口*/				=	81,
	IUIDrawMgrPtr_Class/*接口类型_渲染2D接口*/			=	82,
	IMap2DPtr_Class/*接口类型_地图2D接口*/				=	83,
	IMap2DItemPtr_Class/*接口类型_地图对象2D接口*/		=	84,
	IValueControlPtr_Class/*接口类型_值控制接口*/		=	85,
	IDataControlPtr_Class/*接口类型_数据控制接口*/		=	86,
	IMap2DEventPtr_Class/*接口类型_地图事件2D接口*/		=	87,
	IDataArrayPtr_Class/*接口类型_数据数组控制*/		=	88,
	IMap3DPointPtr_Class/*接口类型_地图移动点3D接口*/	=	89,
	IMap3DUnitPtr_Class/*接口类型_地图单位3D接口*/		=	90,
	IMap3DSkillPtr_Class/*接口类型_地图技能3D接口*/		=	91,
	//
	IBodyPartPtr_Class/*接口类型_身体部件*/				=	92,
	IAvatarPtr_Class/*接口类型_纸娃娃*/					=	93,
};

#define	CLASS_MAX	120

template <typename T>	void	SetPtrID(T& t, Base* autoPtr, int classType)
{
	if(!autoPtr)
		return;
	PtrID id	=	autoPtr->GetPtrID();
	t.idx		=	id.id;
	t.ptr		=	id.index;
	t.t			=	classType;
}
template <typename T>	void	SetPtrID(T& t, int index, int id, int classType)
{
	t.idx		=	id;
	t.ptr		=	index;
	t.t			=	classType;
}
template <typename T>	void	CreatePtrID(T& t, Base* autoPtr, int classType)
{
	if(!autoPtr)
		return;
	PtrID id	=	autoPtr->GetPtrID();
	t.idx		=	id.id;
	t.ptr		=	id.index;
	t.t			=	classType;
}

template <typename T>	bool	IsExistPtr(T& t)
{
	PtrID id;
	id.id		=	t.idx;
	id.index	=	t.ptr;
	Base* autoPtr	=	GetBasePtr(id);
	if(!autoPtr)
	{
		t.idx	=	0;
		t.ptr	=	0;
		return false;
	}
	return true;
}

template <typename T>	int		GetContext(T& t)
{
	PtrID id;
	id.id		=	t.idx;
	id.index	=	t.ptr;
	return GetBaseContext(id);
}
template <typename T>	void	SetContext(T& t, int n)
{
	PtrID id;
	id.id		=	t.idx;
	id.index	=	t.ptr;
	SetBaseContext(id, n);
}
template <typename Tn, typename T>	T*	GetPtrID(Tn* t, int classType)
{
	if(!t)
		return 0;
	if((t->t % CLASS_MAX) != classType)
		return 0;
	if(t->idx <= 0)
		return 0;
	PtrID id;
	id.id		=	t->idx;
	id.index	=	t->ptr;
	Base* autoPtr	=	GetBasePtr(id);
	if(!autoPtr)
		return 0;
	return static_cast<T*>(autoPtr);
}

template <typename Tn, typename T, typename thisType>	thisType	GetPtrIDT(Tn* t, int classType)
{
	T* t2 = GetPtrID<Tn,T>(t, classType);
	if(!t2)
		return 0;
	return t2->ptr;
}
template <typename Tn, typename T>	T*	GetReleasePtrID(Tn& t, int classType)
{
	if(t.t < CLASS_MAX || t.t != classType)
		return 0;
	if(t.idx <= 0)
		return 0;
	PtrID id;
	id.id		=	t.idx;
	id.index	=	t.ptr;
	Base* autoPtr	=	GetBasePtr(id);
	if(!autoPtr)
		return 0;
	return static_cast<T*>(autoPtr);
}

#define	SetIPtr(unk, _ptr, type, unk_type)		{SetPtrID<unk_type>(*static_cast<unk_type*>(unk), static_cast<type*>(_ptr), (_ptr ? unk_type##_Class : 0));}
#define	SetIPtrI(unk, _ptr, type, unk_type, rid)		{SetPtrID<unk_type>(*static_cast<unk_type*>(unk), static_cast<type*>(_ptr), (_ptr ? unk_type##_Class : 0)); unk->reserved = rid;}
#define	SetIPtr2(unk, _ptr, type, unk_type)		{SetPtrID<unk_type>(*static_cast<unk_type*>(unk), _ptr.index, _ptr.id, unk_type##_Class);}
#define	CreateIPtr(unk, _ptr, type, unk_type)	{CreatePtrID<unk_type>(*static_cast<unk_type*>(unk), static_cast<type*>(_ptr), (_ptr ? (unk_type##_Class + CLASS_MAX) : 0));}

#define	GetIPtr(unk, type, unk_type)			GetPtrID<unk_type, type>(static_cast<unk_type*>(unk), unk_type##_Class)
#define	GetIPtrT(unk, type, unk_type)			GetPtrIDT<unk_type, type, type::thisType>(static_cast<unk_type*>(unk), unk_type##_Class)
#define	ReleaseIPtr(unk, type, unk_type)		{type* t = GetReleasePtrID<unk_type, type>(*static_cast<unk_type*>(unk), (unk_type##_Class + CLASS_MAX)); if(t){t->Release();}; unk->t = 0;unk->ptr = 0;unk->idx = 0;}

#define	SetNull(unk)							{if(unk){unk->t = 0;unk->ptr = 0;unk->idx = 0;}}
#define	DefineIPtr(type)						type	ret;memset(&ret, 0, sizeof(type));
//
#define	GetIPtr2(unk, type, unk_type1, unk_type2)							(((unk->t % CLASS_MAX) == unk_type1##_Class || (unk->t % CLASS_MAX) == unk_type2##_Class) ? (type*)IntToPtr(unk->ptr) : NULL)
#define	GetIPtr3(unk, type, unk_type1, unk_type2, unk_type3)				(((unk->t % CLASS_MAX) == unk_type1##_Class || (unk->t % CLASS_MAX) == unk_type2##_Class || (unk->t % CLASS_MAX) == unk_type3##_Class) ? (type*)IntToPtr(unk->ptr) : NULL)
#define	GetIPtr4(unk, type, unk_type1, unk_type2, unk_type3, unk_type4)		(((unk->t % CLASS_MAX) == unk_type1##_Class || (unk->t % CLASS_MAX) == unk_type2##_Class || (unk->t % CLASS_MAX) == unk_type3##_Class || (unk->t % CLASS_MAX) == unk_type4##_Class) ? (type*)IntToPtr(unk->ptr) : NULL)

inline	UIComboBox*		GetComboBox2D(UIControl* c){if(c)return static_cast<UIComboBox*>(c); return 0;}
inline	UIListBox*		GetListBox2D(UIControl* c){if(c)return static_cast<UIListBox*>(c); return 0;}
inline	UIScrollBar*		GetScrollBar2D(UIControl* c){if(c)return static_cast<UIScrollBar*>(c); return 0;}
inline	UIListCtrl*		GetListCtrl2D(UIControl* c){if(c)return static_cast<UIListCtrl*>(c); return 0;}


/*STRUCT*/struct ITexturePtr/*接口_纹理指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	ITexturePtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IUIDrawMgrPtr/*接口_渲染指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IUIDrawMgrPtr(){idx=ptr=t=0;}
};
/*STRUCT*/struct IMap3DPointPtr/*接口_地图事件3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IMap3DPointPtr(){idx=ptr=t=0;}
};
/*STRUCT*/struct IValueControlPtr/*接口_值控制*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IValueControlPtr(){idx=ptr=t=0;}
};
/*STRUCT*/struct IDataControlPtr/*接口_数据控制*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IDataControlPtr(){idx=ptr=t=0;}
};
/*STRUCT*/struct IDataArrayPtr/*接口_数据数组*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IDataArrayPtr(){idx=ptr=t=0;}
};
/*STRUCT*/struct IMeshMaterialPtr/*接口_子模型材质指针*/
{
	unsigned char	t;
	unsigned short	reserved;
	INT				ptr;
	INT				idx;
	IMeshMaterialPtr(){idx=ptr=t=reserved=0;}
};

/*STRUCT*/struct IUnitPtr/*接口_场景单位指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IUnitPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct ITerrainPtr/*接口_场景地形指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	ITerrainPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IUnit3DElementPtr/*接口_单位动作元素3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IUnit3DElementPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IUnit3DMeshPtr/*接口_单位模型3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IUnit3DMeshPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct ISubMeshPtr/*接口_子模型指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	ISubMeshPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IMeshActionPtr/*接口_模型数据指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IMeshActionPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct ICursorPtr/*接口_光标指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	ICursorPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IUnit3DActionPtr/*接口_单位动作3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IUnit3DActionPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IUnit3DPtr/*接口_单位3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IUnit3DPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct ISkyPtr/*接口_场景天空3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	unsigned char	reserved;
	ISkyPtr(){idx=ptr=t=0;reserved=0;}
};

/*STRUCT*/struct IMap3DEnvPtr/*接口_场景环境3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	unsigned char	reserved;
	IMap3DEnvPtr(){idx=ptr=t=0;reserved=0;}
};

/*STRUCT*/struct IMap3DPtr/*接口_场景3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IMap3DPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IMeshPtr/*接口_模型指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IMeshPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct ITimePtr/*接口_时间点指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	ITimePtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IAffectPtr/*接口_影响器指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IAffectPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IEmissionProjectPtr/*接口_喷射器方案指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IEmissionProjectPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IEffectProjectPtr/*接口_特效方案指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IEffectProjectPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IEmissionPtr/*接口_喷射器指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IEmissionPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IEffectPtr/*接口_特效指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IEffectPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IShaderPtr/*接口_着色器指针*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IShaderPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IMyMeshPtr/*接口_动态模型*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IMyMeshPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IMap3DItemPtr/*接口_场景物体3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IMap3DItemPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IMap3DSkillPtr/*接口_场景技能3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IMap3DSkillPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IMap3DUnitPtr/*接口_场景单位3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IMap3DUnitPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IMap3DSkillProjPtr/*接口_场景技能方案3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IMap3DSkillProjPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct ITableReaderPtr/*接口_表格读取*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	ITableReaderPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IVideoPlayerPtr/*接口_视频播放*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IVideoPlayerPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IVideoWriterPtr/*接口_视频数据*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IVideoWriterPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IVideoWriter3DPtr/*接口_视频3D*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IVideoWriter3DPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IElementPtr/*接口_界面元素*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IElementPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IUIAnimationPtr/*接口_界面动画*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IUIAnimationPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IStaticAnimationPtr/*接口_静态动画框*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IStaticAnimationPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IComboBoxPtr/*接口_组合框*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IComboBoxPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IEditBoxPtr/*接口_编辑框*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IEditBoxPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IFormatTextPtr/*接口_格式化文本*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IFormatTextPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IListBoxPtr/*接口_列表框*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IListBoxPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IListCtrlPtr/*接口_行列框*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IListCtrlPtr(){idx=ptr=t=0;}
};


/*STRUCT*/struct ISliderPtr/*接口_拖动条*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	ISliderPtr(){idx=ptr=t=0;}
};
/*STRUCT*/struct IProgressPtr/*接口_进度条*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IProgressPtr(){idx=ptr=t=0;}
};
/*STRUCT*/struct ICheckBoxPtr/*接口_单选框*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	ICheckBoxPtr(){idx=ptr=t=0;}
};
/*STRUCT*/struct IControlPtr/*接口_控件*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IControlPtr(){idx=ptr=t=0;}
};
/*STRUCT*/struct IBodyPartPtr/*接口_身体部件*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IBodyPartPtr(){idx=ptr=t=0;}
};
/*STRUCT*/struct IAvatarPtr/*接口_纸娃娃*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IAvatarPtr(){idx=ptr=t=0;}
};
#ifdef LUA_GEN
/*STRUCT*/struct	IGameObjectPtr/*游戏对象ID*/
{
	unsigned short id;
	unsigned short index;
	/*END_STRUCT*/
};
#else
typedef	GameID	IGameObjectPtr;
#endif
/*STRUCT*/struct IUIGroupPtr/*接口_控件组*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IUIGroupPtr(){idx=ptr=t=0;}
};
/*STRUCT*/struct IFindWayPtr/*接口_寻路*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IFindWayPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IDialogPtr/*接口_对话框*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IDialogPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct	INetWriterPtr/*接口_网络数据包写入*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	INetWriterPtr(){idx=ptr=t=0;}
};/*END_STRUCT*/

/*STRUCT*/struct	INetReaderPtr/*接口_网络数据包读取*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	INetReaderPtr(){idx=ptr=t=0;}
};/*END_STRUCT*/

/*STRUCT*/struct INetServerPtr/*接口_服务器端*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	INetServerPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct INetClientPtr/*接口_客户端*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	INetClientPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct INetEventPtr/*接口_事件*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	INetEventPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct IIntersectPtr/*接口_碰撞检测*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	IIntersectPtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct ILogicLinePtr/*接口_逻辑行*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	ILogicLinePtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct ILogicTablePtr/*接口_逻辑表*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	ILogicTablePtr(){idx=ptr=t=0;}
};

/*STRUCT*/struct ILogicDataPtr/*接口_逻辑数据*/
{
	unsigned char	t;
	INT				ptr;
	INT				idx;
	ILogicDataPtr(){idx=ptr=t=0;}
};
#pragma pack(pop)

///////////////////////数学库
/*API*/VOID			WINAPI	float2_setup/*二维向量_设置*/(float2* _self/*返回*/, float x, float y);
/*API*/float		WINAPI	float2_length/*二维向量_长度*/(float2* _self/*参数1*/);//返回向量长度
/*API*/float		WINAPI	float2_dot/*二维向量_点乘*/(float2* _self/*参数1*/, const float2* v2/*参数2*/);//返回向量点乘
/*API*/float		WINAPI	float2_cross/*二维向量_叉乘*/(const float2* _self/*参数1*/, const float2* v2/*参数2*/);//返回向量叉乘
/*API*/VOID			WINAPI	float2_normalize/*二维向量_归一*/(float2* _self/*参数1*/);//向量归一
/*API*/VOID			WINAPI	float2_lerp/*二维向量_插值*/(float2* _self/*返回*/, const float2* v1/*参数1*/, const float2* v2/*参数2*/, float fLerp/*变量*/);//向量插值，变量0~1之间
/*API*/VOID			WINAPI	float2_hermite/*二维向量_曲线插值*/( float2 *_self/*返回*/, const float2 *pV1, const float2 *pT1,const float2 *pV2, const float2 *pT2, float s );// Hermite interpolation between position V1, tangent T1 (when s == 0// and position V2, tangent T2 (when s == 1).
/*API*/VOID			WINAPI	float2_catmullrom/*二维向量_Catmull_Rom插值*/( float2 *_self/*返回*/, const float2 *pV0, const float2 *pV1,const float2 *pV2, const float2 *pV3, float s );// CatmullRom interpolation between V1 (when s == 0) and V2 (when s == 1)
/*API*/VOID			WINAPI	float2_barycentric/*二维向量_重心*/( float2 *_self/*返回*/, const float2 *pV1, const float2 *pV2,const float2 *pV3, float f, float g);// Barycentric coordinates.  V1 + f(V2-V1) + g(V3-V1)
/*API*/VOID			WINAPI	float2_transformcoord/*二维向量_变换*/( float2 *_self/*返回*/, const float2 *pV, const matrix4x4 *pM );// Transform (x, y, 0, 1) by matrix, project result back into w=1.
/*API*/VOID			WINAPI	float2_transformnormal/*二维向量_单位化*/( float2 *_self/*返回*/, const float2 *pV, const matrix4x4 *pM );// Transform (x, y, 0, 0) by matrix.
//
/*API*/VOID			WINAPI	float3_setup/*三维向量_设置*/(float3* _self/*返回*/, float x, float y, float z);
/*API*/float		WINAPI	float3_length/*三维向量_长度*/(float3* _self/*参数1*/);//返回向量长度
/*API*/float		WINAPI	float3_dot/*三维向量_点乘*/(const float3* _self/*参数1*/, const float3* v2/*参数2*/);//返回向量点乘
/*API*/VOID			WINAPI	float3_cross/*三维向量_叉乘*/(float3* _self/*返回*/, const float3* v1/*参数1*/, const float3* v2/*参数2*/);//返回向量叉乘
/*API*/VOID			WINAPI	float3_normalize/*三维向量_归一*/(float3* _self/*参数1*/);//向量归一
/*API*/VOID			WINAPI	float3_lerp/*三维向量_插值*/(float3* _self/*返回*/, const float3* v1/*参数1*/, const float3* v2/*参数2*/, float fLerp/*变量*/);//向量插值，变量0~1之间
/*API*/VOID			WINAPI	float3_transform/*三维向量_矩阵变换*/(float3* _self/*返回*/, const float3* v2/*参数1*/, const matrix4x4* m/*矩阵1*/);//向量与矩阵进行变换
/*API*/VOID			WINAPI	float3_transform3x3/*三维向量_矩阵旋转变换*/(float3* _self/*返回*/, const float3* v2/*参数1*/, const matrix4x4* m/*矩阵1*/);//向量与矩阵进行变换
/*API*/VOID			WINAPI	float3_rotateX/*三维向量_转动X轴*/(float3* _self/*返回*/, const float3* v2/*参数1*/, float fDegress/*角度*/);//向量随X轴转动
/*API*/VOID			WINAPI	float3_rotateY/*三维向量_转动Y轴*/(float3* _self/*返回*/, const float3* v2/*参数1*/, float fDegress/*角度*/);//向量随Y轴转动
/*API*/VOID			WINAPI	float3_rotateZ/*三维向量_转动Z轴*/(float3* _self/*返回*/, const float3* v2/*参数1*/, float fDegress/*角度*/);//向量随Z轴转动
/*API*/VOID		 	WINAPI	float3_hermite/*三维向量_曲线插值*/( float3 *_self/*返回*/, const float3 *pV1, const float3 *pT1,const float3 *pV2, const float3 *pT2, float s );// Hermite interpolation between position V1, tangent T1 (when s == 0)// and position V2, tangent T2 (when s == 1).
/*API*/VOID			WINAPI	float3_catmullrom/*三维向量_Catmull_Rom插值*/( float3 *_self/*返回*/, const float3 *pV0, const float3 *pV1,const float3 *pV2, const float3 *pV3, float s );// CatmullRom interpolation between V1 (when s == 0) and V2 (when s == 1)
/*API*/VOID			WINAPI	float3_barycentric/*三维向量_重心*/( float3 *_self/*返回*/, const float3 *pV1, const float3 *pV2,const float3 *pV3, float f, float g);// Barycentric coordinates.  V1 + f(V2-V1) + g(V3-V1)
/*API*/VOID			WINAPI	float3_transformcoord/*三维向量_变换*/( float3 *_self/*返回*/, const float3 *pV, const matrix4x4 *pM );// Transform (x, y, z, 1) by matrix, project result back into w=1.
/*API*/VOID			WINAPI	float3_transformnormal/*三维向量_单位化*/( float3 *_self/*返回*/, const float3 *pV, const matrix4x4 *pM );// Transform (x, y, z, 0) by matrix.  If you transforming a normal by a // non-affine matrix, the matrix you pass to this function should be the // transpose of the inverse of the matrix you would use to transform a coord.
/*API*/float		WINAPI	float3_getrotate_z/*三维向量_返回角度Z*/(const float3* _self/*值*/);
//
/*API*/VOID			WINAPI	float4_setup/*四维向量_设置*/(float4* _self/*返回*/, float x, float y, float z, float w);
/*API*/float		WINAPI	float4_length/*四维向量_长度*/(float4* _self/*参数1*/);//返回向量长度
/*API*/float		WINAPI	float4_dot/*四维向量_点乘*/(const float4* _self/*参数1*/, const float4* v2/*参数2*/);//返回向量点乘
/*API*/VOID			WINAPI	float4_lerp/*四维向量_插值*/(float4* _self/*返回*/, const float4* v1/*参数1*/, const float4* v2/*参数2*/, float fLerp/*变量*/);//向量插值，变量0~1之间
/*API*/VOID			WINAPI	float4_copy/*四维向量_拷贝*/(float4* _self/*返回*/, const float4* v1/*参数1*/);//向量拷贝
/*API*/VOID		 	WINAPI	float4_hermite/*四维向量_曲线插值*/( float4 *_self/*返回*/, const float4 *pV1, const float4 *pT1,const float4 *pV2, const float4 *pT2, float s );// Hermite interpolation between position V1, tangent T1 (when s == 0)// and position V2, tangent T2 (when s == 1).
/*API*/VOID			WINAPI	float4_catmullrom/*四维向量_Catmull_Rom插值*/( float4 *_self/*返回*/, const float4 *pV0, const float4 *pV1,const float4 *pV2, const float4 *pV3, float s );// CatmullRom interpolation between V1 (when s == 0) and V2 (when s == 1)
/*API*/VOID			WINAPI	float4_barycentric/*四维向量_重心*/( float4 *_self/*返回*/, const float4 *pV1, const float4 *pV2,const float4 *pV3, float f, float g);// Barycentric coordinates.  V1 + f(V2-V1) + g(V3-V1)
/*API*/VOID			WINAPI	float4_transform/*四维向量_单位化*/( float4 *_self/*返回*/, const float3 *pV, const matrix4x4 *pM );// Transform (x, y, z, 0) by matrix.  If you transforming a normal by a // non-affine matrix, the matrix you pass to this function should be the // transpose of the inverse of the matrix you would use to transform a coord.
//
/*API*/VOID			WINAPI	color4_setup/*颜色_设置*/(color4* _self/*返回*/, float r, float g, float b, float a);
/*API*/INT			WINAPI	color4_rgb/*颜色_返回RGB色*/(color4* _self/*颜色*/);//
/*API*/INT			WINAPI	color4_Getrgba/*颜色_返回ARGB色*/(color4* _self/*颜色*/);//
/*API*/VOID			WINAPI	color4_fromrgb/*颜色_设置RGB色*/(color4* _self/*颜色*/, INT r/*红色*/, INT g/*绿色*/, INT b/*蓝色*/);//
/*API*/VOID			WINAPI	color4_fromrgba/*颜色_从RGB色*/(color4* _self/*返回*/, INT rgb/*RGB颜色*/, float fAlpha/*透明*/);//
/*API*/VOID			WINAPI	color4_Setrgba/*颜色_从BGRA色*/(color4* _self/*返回*/, INT bgra/*BGRA颜色*/);
/*API*/VOID			WINAPI	color4_set/*颜色_设置Global_rgba*/(color4* _self/*返回*/, float r, float g, float b, float a);
///
/*API*/VOID		 	WINAPI	quaternion_rotationmatrix/*四元数_旋转矩阵创建*/( quaternion *_self/*返回*/, const matrix4x4 *pM);// Build a quaternion from a rotation matrix.
/*API*/VOID		 	WINAPI	quaternion_rotationaxis/*四元数_绕轴旋转*/( quaternion *_self/*返回*/, const float3 *pV, float fDegress/*角度*/ );// Rotation about arbitrary axis.
/*API*/VOID		 	WINAPI	quaternion_rotation_yawpitchroll/*四元数_绕XYZ旋转创建*/( quaternion *_self/*返回*/, float Yaw, float Pitch, float Roll );// 角度Yaw around the Y axis, a pitch around the X axis,// and a roll around the Z axis.
/*API*/VOID		 	WINAPI	quaternion_multiply/*四元数_相乘*/( quaternion *_self/*返回*/, const quaternion *pQ1,const quaternion *pQ2 );// Quaternion multiplication.  The result represents the rotation Q2// followed by the rotation Q1.  (Out = Q2 * Q1)
/*API*/VOID		 	WINAPI	quaternion_normalize/*四元数_归一化*/( quaternion *_self/*返回*/, const quaternion *pQ );// Conjugate and re-norm
/*API*/VOID		 	WINAPI	quaternion_inverse/*四元数_共轭变换并规格化*/( quaternion *_self/*返回*/, const quaternion *pQ );// Expects unit quaternions.// if q = (cos(theta), sin(theta) * v); ln(q) = (0, theta * v)
/*API*/VOID		 	WINAPI	quaternion_ln/*四元数_自然对数*/( quaternion *_self/*返回*/, const quaternion *pQ );
/*API*/VOID		 	WINAPI	quaternion_exp/*四元数_指数*/( quaternion *_self/*返回*/, const quaternion *pQ );// Expects pure quaternions. (w == 0)  w is ignored in calculation.// if q = (0, theta * v); exp(q) = (cos(theta), sin(theta) * v)
/*API*/VOID		 	WINAPI	quaternion_slerp/*四元数_球面插值*/( quaternion *_self/*返回*/, const quaternion *pQ1,const quaternion *pQ2, float t );// Spherical linear interpolation between Q1 (t == 0) and Q2 (t == 1).// Expects unit quaternions.
/*API*/VOID		 	WINAPI	quaternion_squad/*四元数_球面四边形*/( quaternion *_self/*返回*/, const quaternion *pQ1,const quaternion *pA, const quaternion *pB,const quaternion *pC, float t );// Spherical quadrangle interpolation.// Slerp(Slerp(Q1, C, t), Slerp(A, B, t), 2t(1-t))
/*API*/VOID		 	WINAPI	quaternion_barycentric/*四元数_重心*/( quaternion *_self/*返回*/, const quaternion *pQ1,const quaternion *pQ2, const quaternion *pQ3,float f, float g );// Barycentric interpolation.// Slerp(Slerp(Q1, Q2, f+g), Slerp(Q1, Q3, f+g), g/(f+g))
//
/*API*/VOID		 	WINAPI	Plane_normalize/*平面_归一化*/( Plane *_self/*返回*/, const Plane *pP);//规格化平面方程系数以求单位法线
/*API*/VOID		 	WINAPI	Plane_intersectline/*平面_计算交点*/( float3 *r/*返回*/, const Plane *_self, const float3 *pV1,const float3 *pV2);//找出直线平面交点
/*API*/VOID		 	WINAPI	Plane_from_pointnormal/*平面_根据点和法线创建*/( Plane *_self/*返回*/, const float3 *pPoint, const float3 *pNormal);//根据点和法线创建平面
/*API*/VOID		 	WINAPI	Plane_from_points/*平面_根据三个点创建*/( Plane *_self/*返回*/, const float3 *pV1, const float3 *pV2,const float3 *pV3);//根据三点创建平面
/*API*/VOID		 	WINAPI	Plane_transform/*平面_矩阵转制*/( Plane *_self/*返回*/, const Plane *pP, const matrix4x4 *pM );// Transform a Plane by a matrix.  The vector (a,b,c) must be normal.// M should be the inverse transpose of the transformation desired.
/*API*/float		WINAPI	Plane_dot/*平面_与4D向量点乘*/(const Plane* _self, const float4* p2);// ax + by + cz + dw平面方程与4D向量的点乘
/*API*/float		WINAPI	Plane_dotcoord/*平面_与3D向量点乘_w1*/(const Plane* _self, const float3* p2);// ax + by + cz + d平面与3D向量的点乘,w=1
/*API*/float		WINAPI	Plane_dotnormal/*平面_与3D向量点乘_w0*/(const Plane* _self, const float3* p2);// ax + by + cz平面与3D向量的点乘,w=0
/*API*/VOID			WINAPI	Plane_scale/*平面_缩放*/(Plane* _self, const Plane* p1, float s);
//
/*API*/VOID			WINAPI	matrix4x4_Setpos/*矩阵_设置位置*/(matrix4x4* _self/*返回*/, const float3* v/*值*/);//
/*API*/VOID			WINAPI	matrix4x4_Getpos/*矩阵_取得位置*/(const matrix4x4* _self/*矩阵*/, float3* _return/*返回*/);//
/*API*/VOID			WINAPI	matrix4x4_identity/*矩阵_设置单位矩阵*/(matrix4x4* _self/*指针*/);//
/*API*/VOID			WINAPI	matrix4x4_lerp/*矩阵_插值*/(matrix4x4* _self/*返回*/, const matrix4x4* v1/*参数1*/, const matrix4x4* v2/*参数2*/, float fLerp/*变量*/);//矩阵插值，变量0~1之间
/*API*/VOID		 	WINAPI	matrix4x4_transform/*矩阵_根据平移缩放转动创建*/(matrix4x4* _self/*返回*/, const float3* pos, const quaternion* rot, const float3* scale);	//转变,根据参数
/*API*/BOOL			WINAPI	matrix4x4_decompose/*矩阵_返回缩放旋转平移向量*/( float3 *pOutScale, quaternion *pOutRotation, float3 *pOutTranslation, const matrix4x4 *_self );
/*API*/VOID			WINAPI	matrix4x4_transpose/*矩阵_转置*/( matrix4x4 *pOut, const matrix4x4 *_self );
/*API*/VOID 		WINAPI	matrix4x4_multiply_transpose/*矩阵_乘积的转置*/( matrix4x4 *_self/*返回*/, const matrix4x4 *pM1, const matrix4x4 *pM2 );// Matrix multiplication, followed by a transpose. (Out = T(M1 * M2))
/*API*/VOID 		WINAPI	matrix4x4_inverse/*矩阵_逆矩阵*/( matrix4x4 *_self/*返回*/, const matrix4x4 *pM );// Calculate inverse of matrix.  Inversion my fail, in which case NULL will// be returned.  The determinant of pM is also returned it pfDeterminant// is non-NULL.
/*API*/VOID 		WINAPI	matrix4x4_inverse2/*矩阵_逆矩阵2*/( matrix4x4 *_self/*返回*/, float3* pDeterminant, const matrix4x4 *pM );// Calculate inverse of matrix.  Inversion my fail, in which case NULL will// be returned.  The determinant of pM is also returned it pfDeterminant// is non-NULL.
/*API*/VOID 		WINAPI	matrix4x4_scaling/*矩阵_缩放*/( matrix4x4 *_self/*返回*/, float sx, float sy, float sz );// Build a matrix which scales by (sx, sy, sz)
/*API*/VOID 		WINAPI	matrix4x4_translation/*矩阵_平移*/( matrix4x4 *_self/*返回*/, float x, float y, float z );// Build a matrix which translates by (x, y, z)
/*API*/VOID 		WINAPI	matrix4x4_rotationx/*矩阵_绕X轴旋转*/( matrix4x4 *_self/*返回*/, float fDegress/*角度*/ );// Build a matrix which rotates around the X axis
/*API*/VOID 		WINAPI	matrix4x4_rotationy/*矩阵_绕Y轴旋转*/( matrix4x4 *_self/*返回*/, float fDegress/*角度*/ );// Build a matrix which rotates around the Y axis
/*API*/VOID 		WINAPI	matrix4x4_rotationz/*矩阵_绕Z轴旋转*/( matrix4x4 *_self/*返回*/, float fDegress/*角度*/ );// Build a matrix which rotates around the Z axis
/*API*/VOID 		WINAPI	matrix4x4_rotationaxis/*矩阵_绕任意轴旋转*/( matrix4x4 *_self/*返回*/, const float3 *pV, float fDegress/*角度*/ );// Build a matrix which rotates around an arbitrary axis
/*API*/VOID 		WINAPI	matrix4x4_rotationquaternion/*矩阵_根据四元组创建*/( matrix4x4 *_self/*返回*/, const quaternion *pQ);// Build a matrix from a quaternion
/*API*/VOID 		WINAPI	matrix4x4_rotation_yawpitchroll/*矩阵_绕XYZ角度旋转*/( matrix4x4 *_self/*返回*/, float Yaw, float Pitch, float Roll );// 角度Yaw around the Y axis, a pitch around the X axis,// and a roll around the Z axis.
/*API*/VOID 		WINAPI	matrix4x4_transformation/*矩阵_变换*/( matrix4x4 *_self/*返回*/, const float3 *pScalingCenter,const quaternion *pScalingRotation, const float3 *pScaling,const float3 *pRotationCenter, const quaternion *pRotation,const float3 *pTranslation);// Build transformation matrix.  NULL arguments are treated as identity.// Mout = Msc-1 * Msr-1 * Ms * Msr * Msc * Mrc-1 * Mr * Mrc * Mt
/*API*/VOID 		WINAPI	matrix4x4_transformation2d/*矩阵_平面变换*/( matrix4x4 *_self/*返回*/, const float2* pScalingCenter, float ScalingRotation, const float2* pScaling, const float2* pRotationCenter, float Rotation, const float2* pTranslation);// Build 2D transformation matrix in XY Plane.  NULL arguments are treated as identity.// Mout = Msc-1 * Msr-1 * Ms * Msr * Msc * Mrc-1 * Mr * Mrc * Mt
/*API*/VOID 		WINAPI	matrix4x4_affineTransformation/*矩阵_在平面创建放射变换*/( matrix4x4 *_self/*返回*/, float Scaling, const float3 *pRotationCenter,const quaternion *pRotation, const float3 *pTranslation);// Build affine transformation matrix.  NULL arguments are treated as identity.// Mout = Ms * Mrc-1 * Mr * Mrc * Mt
/*API*/VOID 		WINAPI	matrix4x4_affineTransformation2d/*矩阵_创建放射变换*/( matrix4x4 *_self/*返回*/, float Scaling, const float2* pRotationCenter, float Rotation, const float2* pTranslation);// Build 2D affine transformation matrix in XY Plane.  NULL arguments are treated as identity.// Mout = Ms * Mrc-1 * Mr * Mrc * Mt
/*API*/VOID 		WINAPI	matrix4x4_lookat_rh/*矩阵_右手系观察矩阵*/( matrix4x4 *_self/*返回*/, const float3 *pEye, const float3 *pAt,const float3 *pUp );// Build a lookat matrix. (right-handed)
/*API*/VOID 		WINAPI	matrix4x4_lookat_lh/*矩阵_左手系观察矩阵*/( matrix4x4 *_self/*返回*/, const float3 *pEye, const float3 *pAt,const float3 *pUp );// Build a lookat matrix. (left-handed)
/*API*/VOID 		WINAPI	matrix4x4_perspective_rh/*矩阵_右手系投影矩阵*/( matrix4x4 *_self/*返回*/, float w, float h, float zn, float zf );// Build a perspective projection matrix. (right-handed)
/*API*/VOID 		WINAPI	matrix4x4_perspective_lv/*矩阵_左手系投影矩阵*/( matrix4x4 *_self/*返回*/, float w, float h, float zn, float zf );// Build a perspective projection matrix. (left-handed)
/*API*/VOID 		WINAPI	matrix4x4_perspective_fovrh/*矩阵_右手系投影矩阵_广角*/( matrix4x4 *_self/*返回*/, float fovy, float Aspect, float zn, float zf );// Build a perspective projection matrix. (right-handed)
/*API*/VOID 		WINAPI	matrix4x4_perspectivefov_lh/*矩阵_左手系投影矩阵_广角*/( matrix4x4 *_self/*返回*/, float fovy, float Aspect, float zn, float zf );// Build a perspective projection matrix. (left-handed)
/*API*/VOID 		WINAPI	matrix4x4_perspective_offcenter_rh/*矩阵_右手系投影矩阵_非中心点*/( matrix4x4 *_self/*返回*/, float l, float r, float b, float t, const float2* znearfar );// Build a perspective projection matrix. (right-handed)
/*API*/VOID 		WINAPI	matrix4x4_perspective_offcenter_lh/*矩阵_左手系投影矩阵_非中心点*/( matrix4x4 *_self/*返回*/, float l, float r, float b, float t, const float2* znearfar );// Build a perspective projection matrix. (left-handed)
/*API*/VOID 		WINAPI	matrix4x4_ortho_rh/*矩阵_右手系正交投影*/( matrix4x4 *_self/*返回*/, float w, float h, float zn, float zf );// Build an ortho projection matrix. (right-handed)
/*API*/VOID 		WINAPI	matrix4x4_ortho_lh/*矩阵_左手系正交投影*/( matrix4x4 *_self/*返回*/, float w, float h, float zn, float zf );// Build an ortho projection matrix. (left-handed)
/*API*/VOID 		WINAPI	matrix4x4_ortho_offcenter_rh/*矩阵_右手系正交投影_非中心点*/( matrix4x4 *_self/*返回*/, float l, float r, float b, float t, const float2* znearfar);// Build an ortho projection matrix. (right-handed)
/*API*/VOID 		WINAPI	matrix4x4_orth_offcenter_lh/*矩阵_左手系正交投影_非中心点*/( matrix4x4 *_self/*返回*/, float l, float r, float b, float t, const float2* znearfar);// Build an ortho projection matrix. (left-handed)
/*API*/VOID 		WINAPI	matrix4x4_reflect/*矩阵_平面反射矩阵*/( matrix4x4 *_self/*返回*/, const Plane *pPlane );// Build a matrix which reflects the coordinate system about a Plane


/*API*/VOID			WINAPI	MeshMaterial_Getcolor/*子模型材质_设置漫射光颜色*/(IMeshMaterialPtr* _self/*子模型ID*/, color4* _return/*颜色*/);
/*API*/VOID			WINAPI	MeshMaterial_Getspecular/*子模型材质_设置高光色*/(IMeshMaterialPtr* _self/*子模型ID*/, color4* _return/*颜色*/);
/*API*/VOID			WINAPI	MeshMaterial_Getselfluminous/*子模型材质_设置自发光色*/(IMeshMaterialPtr* _self/*子模型ID*/, color4* _return/*颜色*/);
/*API*/BlendMode	WINAPI	MeshMaterial_Getsrcblend/*子模型材质_设置源混合*/(IMeshMaterialPtr* _self/*子模型ID*/);
/*API*/BlendMode	WINAPI	MeshMaterial_Getdestblend/*子模型材质_设置目标混合*/(IMeshMaterialPtr* _self/*子模型ID*/);
/*API*/VOID			WINAPI	MeshMaterial_Setcolor/*子模型材质_设置漫射光颜色*/(IMeshMaterialPtr* _self/*子模型ID*/, const color4* c/*颜色*/);
/*API*/VOID			WINAPI	MeshMaterial_Setspecular/*子模型材质_设置高光色*/(IMeshMaterialPtr* _self/*子模型ID*/, const color4* c/*颜色*/);
/*API*/VOID			WINAPI	MeshMaterial_Setselfluminous/*子模型材质_设置自发光色*/(IMeshMaterialPtr* _self/*子模型ID*/, const color4* c/*颜色*/);
/*API*/VOID			WINAPI	MeshMaterial_Setsrcblend/*子模型材质_设置源混合*/(IMeshMaterialPtr* _self/*子模型ID*/, BlendMode blend/*值*/);
/*API*/VOID			WINAPI	MeshMaterial_Setdestblend/*子模型材质_设置目标混合*/(IMeshMaterialPtr* _self/*子模型ID*/, BlendMode blend/*值*/);
/*API*/VOID			WINAPI	MeshMaterial_Settexture/*子模型材质_设置默认纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* texturePtr/*纹理ID*/);
/*API*/VOID			WINAPI	MeshMaterial_Setnormal_texture/*子模型材质_设置法线纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* texturePtr/*纹理ID*/);
/*API*/VOID			WINAPI	MeshMaterial_Setspecular_texture/*子模型材质_设置高光纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* texturePtr/*纹理ID*/);
/*API*/VOID			WINAPI	MeshMaterial_Setreflect_texture/*子模型材质_设置反射纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* texturePtr/*纹理ID*/);
//
/*API*/VOID			WINAPI	MeshMaterial_Gettexture/*子模型材质_取默认纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* _return/*返回*/);
/*API*/VOID			WINAPI	MeshMaterial_Getnormal_texture/*子模型材质_取法线纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* _return/*返回*/);
/*API*/VOID			WINAPI	MeshMaterial_Getspecular_texture/*子模型材质_取高光纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* _return/*返回*/);
/*API*/VOID			WINAPI	MeshMaterial_Getreflect_texture/*子模型材质_取反射纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* _return/*返回*/);
//
/*API*/AlphaMode	WINAPI	MeshMaterial_Getopacitymode/*子模型材质_取透明模式*/(IMeshMaterialPtr* _self/*子模型ID*/);//值
/*API*/VOID			WINAPI	MeshMaterial_Setopacitymode/*子模型材质_设置透明模式*/(IMeshMaterialPtr* _self/*子模型ID*/, AlphaMode alpha/*值*/);
/*API*/CullMode		WINAPI	MeshMaterial_Getcullmode/*子模型材质_渲染面*/(IMeshMaterialPtr* _self/*子模型ID*/);
/*API*/VOID			WINAPI	MeshMaterial_Setcullmode/*子模型材质_设置渲染面*/(IMeshMaterialPtr* _self/*子模型ID*/, CullMode cull/*正面或背面或双面渲染*/);


/*API*/INT			WINAPI	SubMesh_vertexcount/*子模型_取顶点数量*/(ISubMeshPtr* _self/*子模型材质ID*/);//HELP[返回模型中的顶点数量]用途：如果想获得模型的顶点数据，需要根据顶点数量一个一个循环获取，使用[SubMeshAction_GetVertex]
/*API*/INT			WINAPI	SubMesh_trianglecount/*子模型_取三角形数量*/(ISubMeshPtr* _self/*子模型材质ID*/);//HELP[返回模型中的三角形数量]用途：如果想获得模型的三角形索引数据，需要根据三角形数量一个一个循环获取，使用[SubMeshAction_GetFacePtr]
/*API*/VOID			WINAPI	SubMesh_vertex_at/*子模型_取顶点数据*/(ISubMeshPtr* _self/*子模型材质ID*/, INT nIndex/*顶点索引*/, Vertex* _return/*取*/);//HELP[返回模型顶点数据]用途：如果想获得模型的顶点数据，需要根据[SubMeshAction_GetVertexCount]获得顶点数量一个一个循环获取， nIndex(顶点索引)实际上是索引值
/*API*/VOID			WINAPI	SubMesh_triangle_at/*子模型_取三角形数据*/(ISubMeshPtr* _self/*子模型材质ID*/, INT nIndex/*面索引*/, Triangle* _return/*取*/);//HELP[返回模型中的三角形索引数据]用途：如果想获得模型的三角形数据，需要根据[SubMeshAction_GetFaceCount]获得三角形数量一个一个循环获取， nIndex(面索引)实际上是索引值
/*API*/BOOL			WINAPI	SubMesh_isskeleton/*子模型_是否有骨骼动画*/(ISubMeshPtr* _self/*子模型材质ID*/);//HELP[是否有骨骼动画]用途：获取子模型是否存在骨骼动画，很少用到
/*API*/BOOL			WINAPI	SubMesh_lod_exists/*子模型_是否有LOD信息*/(ISubMeshPtr* _self/*子模型材质ID*/);//HELP[是否存在LOD模型]用途：返回对象是否存在LOD模型
/*API*/INT			WINAPI	SubMesh_materialcount/*子模型_取材质数量*/(ISubMeshPtr* _self/*子模型ID*/);//用途：取一个子模型的纹理数量，一个子模型可以由多个纹理渲染而成，此项可以查看3dsmax的设置
/*API*/VOID			WINAPI	SubMesh_material_at/*子模型_取子模型材质*/(ISubMeshPtr* _self/*渲染组ID*/, INT index/*索引*/, IMeshMaterialPtr* _return/*返回*/);

/*API*/VOID			WINAPI	SubMesh_Setworldmat/*子模型_取子模型矩阵*/(ISubMeshPtr* _self/*渲染组ID*/, matrix4x4* mat/*取*/);
/*API*/VOID			WINAPI	SubMesh_Getworldmat/*子模型_取子模型矩阵*/(ISubMeshPtr* _self/*渲染组ID*/, matrix4x4* _return/*取*/);
/*API*/LPCSTR		WINAPI	SubMesh_getname/*子模型_取名称*/(ISubMeshPtr* _self/*子模型材质ID*/);//HELP[取名称]用途：取得一个子模型名称，很少用到。
/*API*/BOOL			WINAPI	SubMesh_Getdisplay/*子模型_是否显示*/(ISubMeshPtr* _self/*子模型ID*/);
/*API*/VOID			WINAPI	SubMesh_Setdisplay/*子模型_显示*/(ISubMeshPtr* _self/*子模型ID*/, BOOL b/*是否显示*/);
//
/*API*/VOID		WINAPI	MeshAction_Setspeed/*模型动作_共享数据_设置动作速度*/(IMeshActionPtr* _self/*模型动作ID*/, float fSpeed/*速度*/);//共享数据返回之后，如果其中的某个值改变，则所有模型的值也会改变
/*API*/float	WINAPI	MeshAction_Getspeed/*模型动作_共享数据_取动作速度*/(IMeshActionPtr* _self/*模型动作ID*/);
/*API*/VOID		WINAPI	MeshAction_Setkeybegin/*模型动作_共享数据_动作循环设置*/(IMeshActionPtr* _self/*模型动作ID*/, INT beginIndex/*开始关键帧*/);
/*API*/VOID		WINAPI	MeshAction_Setkeyend/*模型动作_共享数据_动作循环设置*/(IMeshActionPtr* _self/*模型动作ID*/, INT endIndex/*结束关键帧*/);
/*API*/INT		WINAPI	MeshAction_keycount/*模型动作_共享数据_取动作关键帧数量*/(IMeshActionPtr* _self/*模型动作ID*/);
/*API*/INT		WINAPI	MeshAction_Getkeybegin/*模型动作_共享数据_取循环开始关键帧*/(IMeshActionPtr* _self/*模型动作ID*/);
/*API*/INT		WINAPI	MeshAction_Getkeyend/*模型动作_共享数据_取循环结束关键帧*/(IMeshActionPtr* _self/*模型动作ID*/);
/*API*/float	WINAPI	MeshAction_Getlerptime/*模型数据_共享数据_取动作平滑过渡时间*/(IMeshActionPtr* _self/*模型数据ID*/);//取连接到下一个动作的平滑过渡时间
/*API*/VOID		WINAPI	MeshAction_Setlerptime/*模型数据_共享数据_设置动作平滑过渡时间*/(IMeshActionPtr* _self/*模型数据ID*/, float f/*过度时间*/);//设置连接到下一个动作的平滑过渡时间
//
/*API*/BOOL		WINAPI	Mesh_isskeleton/*模型_是否有骨骼绑定*/(IMeshPtr* _self/*模型数据ID*/);
/*API*/float	WINAPI	Mesh_Gets_lodinterval_length/*模型_共享数据_取LOD区间长度*/(IMeshPtr* _self/*模型数据ID*/);//LOD区间长度是每个LOD等级之间的长度，比如此值是10米，则每隔10米都会降低一个LOD等级
/*API*/VOID		WINAPI	Mesh_Sets_lodinterval_length/*模型_共享数据_设置LOD区间长度*/(IMeshPtr* _self/*模型数据ID*/, float f/*LOD区间长度*/);//共享数据返回之后，如果其中的某个值改变，则所有模型的值也会改变
/*API*/float	WINAPI	Mesh_Gets_lodmaxvisible/*模型_共享数据_取LOD最大可见距离*/(IMeshPtr* _self/*模型数据ID*/);//当超过这个距离时自动隐藏
/*API*/VOID		WINAPI	Mesh_Sets_lodmaxvisible/*模型_共享数据_设置最大可见长度*/(IMeshPtr* _self/*模型数据ID*/, float f/*最大可见距离*/);
/*API*/BOOL		WINAPI	Mesh_Gets_lodhide_onexceed/*模型_共享数据_LOD最大距离是否有效*/(IMeshPtr* _self/*模型数据ID*/);//共享数据返回之后，如果其中的某个值改变，则所有模型的值也会改变
/*API*/VOID		WINAPI	Mesh_Sets_lodhide_onexceed/*模型_共享数据_设置LOD最大距离有效*/(IMeshPtr* _self/*模型数据ID*/, BOOL b/*是否有效*/);//共享数据返回之后，如果其中的某个值改变，则所有模型的值也会改变
/*API*/VOID		WINAPI	Mesh_Getpos/*模型_取位置*/(IMeshPtr* _self/*渲染组ID*/, float3* _return/*ret*/);
/*API*/VOID		WINAPI	Mesh_Setpos/*模型_设置位置*/(IMeshPtr* _self/*渲染组ID*/, const float3* pos/*位置*/);
/*API*/VOID		WINAPI	Mesh_Setrot/*模型_设置旋转*/(IMeshPtr* _self/*渲染组ID*/, const float3* rot/*旋转弧度*/);
/*API*/VOID		WINAPI	Mesh_Getrot/*模型_取旋转*/(IMeshPtr* _self/*渲染组ID*/, float3* _return/*ret*/);
/*API*/VOID		WINAPI	Mesh_Setscale/*模型_设置缩放*/(IMeshPtr* _self/*渲染组ID*/, const float3* scale/*缩放*/);
/*API*/VOID		WINAPI	Mesh_Getscale/*模型_取缩放*/(IMeshPtr* _self/*渲染组ID*/, float3* _return/*ret*/);
/*API*/VOID		WINAPI	Mesh_Setcolor/*模型_设置漫射光颜色*/(IMeshPtr* _self/*渲染组ID*/, const color4* c/*颜色*/);
/*API*/VOID		WINAPI	Mesh_Getcolor/*模型_取漫射光颜色*/(IMeshPtr* _self/*渲染组ID*/, color4* _return/*ret*/);
/*API*/VOID			WINAPI	Mesh_Getspecular/*模型_设置高光色*/(IMeshPtr* _self/*子模型ID*/,  color4* _return/*颜色*/);
/*API*/VOID			WINAPI	Mesh_Getselfluminous/*模型_设置自发光色*/(IMeshPtr* _self/*子模型ID*/,  color4* _return/*颜色*/);
/*API*/BlendMode	WINAPI	Mesh_Getsrcblend/*模型_设置源混合*/(IMeshPtr* _self/*子模型ID*/);
/*API*/BlendMode	WINAPI	Mesh_Getdestblend/*模型_设置目标混合*/(IMeshPtr* _self/*子模型ID*/);
/*API*/VOID			WINAPI	Mesh_Setspecular/*模型_设置高光色*/(IMeshPtr* _self/*子模型ID*/,  const color4* c/*颜色*/);
/*API*/VOID			WINAPI	Mesh_Setselfluminous/*模型_设置自发光色*/(IMeshPtr* _self/*子模型ID*/,  const color4* c/*颜色*/);
/*API*/VOID			WINAPI	Mesh_Setsrcblend/*模型_设置源混合*/(IMeshPtr* _self/*子模型ID*/,  BlendMode blend/*值*/);
/*API*/VOID			WINAPI	Mesh_Setdestblend/*模型_设置目标混合*/(IMeshPtr* _self/*子模型ID*/,  BlendMode blend/*值*/);
/*API*/VOID		WINAPI	Mesh_Setactionby/*模型_设置当前动作*/(IMeshPtr* _self/*渲染组ID*/, LPCSTR szName/*动作名称*/);//
/*API*/LPCSTR	WINAPI	Mesh_Getactionby/*模型_取当前动作*/(IMeshPtr* _self/*渲染组ID*/);
/*API*/VOID		WINAPI	Mesh_Setaction/*模型_设置当前动作根据动作ID*/(IMeshPtr* _self/*渲染组ID*/, ActionType t/*动作ID*/);//根据动作类型播放
/*API*/ActionType		WINAPI	Mesh_Getaction/*模型_取当前动作根据动作ID*/(IMeshPtr* _self/*渲染组ID*/);
/*API*/VOID		WINAPI	Mesh_Setdefaultaction/*模型_设置默认动作*/(IMeshPtr* _self/*渲染组ID*/, INT actionID/*动作ID*/);//actionIDMap.editor中定义
/*API*/INT		WINAPI	Mesh_Getdefaultaction/*模型_取默认动作*/(IMeshPtr* _self/*渲染组ID*/);
/*API*/INT		WINAPI	Mesh_searchboneid/*模型_查找骨骼索引*/(IMeshPtr* _self/*渲染组ID*/, LPCSTR szName/*骨骼名*/);
/*API*/INT		WINAPI	Mesh_searchboneid_like/*模型_查找写入文本名类似的骨骼ID*/(IMeshPtr* _self/*渲染组ID*/, LPCSTR szName/*类似名*/);//比如有一个骨骼bone_abc_1,写入_abc_会搜索到这个
/*API*/INT		WINAPI	Mesh_bonecount/*模型_取骨骼ID数量*/(IMeshPtr* _self/*渲染组ID*/);
/*API*/LPCSTR	WINAPI	Mesh_bonename_at/*模型_根据骨骼ID取骨骼名称*/(IMeshPtr* _self/*渲染组ID*/, INT index/*索引*/);
/*API*/VOID		WINAPI	Mesh_bonemat_at/*模型_根据ID取骨骼矩阵*/(IMeshPtr* _self/*渲染组ID*/, INT index/*索引*/, matrix4x4* _return/*取*/);
/*API*/VOID		WINAPI	Mesh_Setspeed/*模型_动作播放速度*/(IMeshPtr* _self/*渲染组ID*/, float fSpeed/*速度*/);//动作文件在actionIDMap.editor中定义，记事本打开
/*API*/float	WINAPI	Mesh_Getspeed/*模型_取动作播放速度*/(IMeshPtr* _self/*渲染组ID*/);
/*API*/INT		WINAPI	Mesh_s_actioncount/*模型_共享数据_取动作数量*/(IMeshPtr* _self/*模型数据ID*/);
/*API*/VOID		WINAPI	Mesh_s_action_at/*模型_共享数据_取动作*/(IMeshPtr* _self/*模型数据ID*/, INT index/*索引*/, IMeshActionPtr* _return/*模型动作*/);//共享数据返回之后，如果其中的某个值改变，则所有模型的值也会改变
/*API*/VOID		WINAPI	Mesh_playaction/*模型_播放某段动作*/(IMeshPtr* _self/*渲染组ID*/, int keyBegin/*开始关键帧*/, int keyEnd/*结束关键帧*/, int nLoop/*循环次数*/);//按照设定值播放一次动作，从开始关键帧到结束关键帧，nLoop=-1表示无限循环，nLoop=1表示只播放一次，始终都使用第一个动作

/*API*/BOOL		WINAPI	Mesh_isunwalkable/*模型_是否禁止移动*/(IMeshPtr* _self/*渲染组ID*/, const float3* pos/*当前位置*/);//根据disable_开头的模型，检测是否可以移动到此点
/*API*/float	WINAPI	Mesh_height/*模型_返回模型高度*/(IMeshPtr* _self/*渲染组ID*/);//取得模型高度
/*API*/VOID		WINAPI	Mesh_aabox/*模型_返回碰撞盒子*/(IMeshPtr* _self/*渲染组ID*/, AABox* _return);//取得模型碰撞盒子
/*API*/BOOL		WINAPI	Mesh_ray/*模型_碰撞检测_仅与碰撞子模型*/(IMeshPtr* _self/*渲染组ID*/, const Pixel* pixel/*屏幕坐标位置*/);//collision_开头的子模型都属于碰撞模型，对这些模型进行相交运算
/*API*/BOOL		WINAPI	Mesh_ray_dir/*模型_碰撞检测_仅与碰撞子模型*/(IMeshPtr* _self/*渲染组ID*/, const float3* orig/*原点*/, const float3* dir/*方向*/);//collision_开头的子模型都属于碰撞模型，对这些模型进行相交运算

/*API*/BOOL		WINAPI	Mesh_Getplay/*模型_是否播放*/(IMeshPtr* _self/*渲染组ID*/);
/*API*/VOID		WINAPI	Mesh_Setplay/*模型_播放*/(IMeshPtr* _self/*渲染组ID*/, BOOL b/*播放与否*/);
/*API*/BOOL		WINAPI	Mesh_Getpause/*模型_是否暂停*/(IMeshPtr* _self/*渲染组ID*/);
/*API*/VOID		WINAPI	Mesh_Setpause/*模型_暂停*/(IMeshPtr* _self/*渲染组ID*/, BOOL b/*暂停与否*/);
//
/*API*/INT		WINAPI	Mesh_submeshcount/*模型_子模型数量*/(IMeshPtr* _self/*渲染组ID*/);
/*API*/VOID		WINAPI	Mesh_submesh_at/*模型_取子模型*/(IMeshPtr* _self/*渲染组ID*/, INT index/*子模型索引*/, ISubMeshPtr* _return/*ret*/);
//
/*API*/float	WINAPI	Mesh_Getopacity/*模型_设置所有子模型透明值*/(IMeshPtr* _self/*渲染组ID*/);//设置透明值
/*API*/VOID		WINAPI	Mesh_Setopacity/*模型_设置所有子模型透明值*/(IMeshPtr* _self/*渲染组ID*/,float fAlpha/*透明值*/);//设置透明值
//
/*API*/BOOL		WINAPI	Mesh_Getdisplay/*模型_是否显示*/(IMeshPtr* _self/*渲染组ID*/);
/*API*/VOID		WINAPI	Mesh_Setdisplay/*模型_显示*/(IMeshPtr* _self/*渲染组ID*/, BOOL b/*是否显示*/);
/*API*/BOOL		WINAPI	Mesh_Getenabled/*模型_是否有效*/(IMeshPtr* _self/*渲染组ID*/);//设置无效之后将不会被点击和选取
/*API*/VOID		WINAPI	Mesh_Setenabled/*模型_有效*/(IMeshPtr* _self/*渲染组ID*/, BOOL b/*是否有效*/);//设置无效之后将不会被点击和选取
/*API*/VOID		WINAPI	Mesh_Getworldmat/*模型_取矩阵*/(IMeshPtr* _self/*渲染组ID*/, matrix4x4* _return/*矩阵取*/);
/*API*/VOID		WINAPI	Mesh_Setworldmat/*模型_设置矩阵*/(IMeshPtr* _self/*渲染组ID*/, const matrix4x4* m/*矩阵*/);
/*API*/LPCSTR	WINAPI	Mesh_getfilepath/*模型_取名称*/(IMeshPtr* _self/*渲染组ID*/);
/*API*/VOID		WINAPI	Mesh_clone/*模型_克隆*/(IMeshPtr* _self/*渲染组ID*/, IMeshPtr* _return/*ret*/);
/*API*/BOOL		WINAPI	Mesh_bind_mesh/*模型_绑定模型*/(IMeshPtr* _self/*渲染组*/, LPCSTR szBoneName/*骨骼名*/, IMeshPtr* mesh/*模型*/);//绑定模型到一个骨骼上
/*API*/VOID		WINAPI	Mesh_unbind_mesh/*模型_取消绑定模型*/(IMeshPtr* _self/*渲染组*/, IMeshPtr* mesh/*模型*/);//绑定模型到一个骨骼上
/*API*/VOID		WINAPI	Mesh_bind_draw/*模型_绑定渲染队列*/(IMeshPtr* _self/*场景对象*/, IUIDrawMgrPtr* drawPtr/*组合*/, LPCSTR szBoneName/*骨骼名字*/);//绑定渲染队列到某个对象
/*API*/VOID		WINAPI	Mesh_replace_image/*模型_绑定图片*/(IMeshPtr* _self/*场景对象*/, LPCSTR szImageSrc/*原图片文件路径*/, LPCSTR szImageReplace/*替换图片文件路径*/);//替换图片
/*API*/VOID		WINAPI	Mesh_Setparent/*模型_设置父模型*/(IMeshPtr* _self/*渲染组*/, IMeshPtr* mesh/*模型*/);//设置父模型,设置之后托管给父物体进行位置设置
/*API*/VOID		WINAPI	Mesh_Getparent/*模型_取消父模型*/(IMeshPtr* _self/*渲染组*/, IMeshPtr* _return/*模型*/);//取消父模型
/*API*/VOID		WINAPI	Mesh_clearParent/*模型_清除父模型*/(IMeshPtr* _self/*渲染组*/);//取消父模型
/*API*/VOID		WINAPI	Mesh_lookat/*模型_设置Y方向面向目标*/(IMeshPtr* _self/*场景对象*/, const float3* m/*目标*/);//设置面向目标的值
/*API*/BOOL		WINAPI	Mesh_Getfollow_camera/*模型_是否跟着镜头移动*/(IMeshPtr* _self/*渲染组ID*/);
/*API*/VOID		WINAPI	Mesh_Setfollow_camera/*模型_跟着镜头移动*/(IMeshPtr* _self/*渲染组ID*/, BOOL b/*是否显示*/);

/*API*/INT		WINAPI	Texture_getwidth/*纹理_取宽度*/(ITexturePtr* _self/*纹理ID*/);
/*API*/INT		WINAPI	Texture_getheight/*纹理_取高度*/(ITexturePtr* _self/*纹理ID*/);
/*API*/BOOL		WINAPI	Texture_commit/*纹理_更新到设备*/(ITexturePtr* _self/*纹理ID*/, TextureStage samplerIndex/*设备材质索引*/);
/*API*/LPCSTR	WINAPI	Texture_getfilepath/*纹理_取纹理路径*/(ITexturePtr* _self/*纹理ID*/);

/*API*/VOID		WINAPI	UIDrawMgr_image/*渲染2D_设置图片*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR szTexture/*图片*/);
/*API*/VOID		WINAPI	UIDrawMgr_image_rect/*渲染2D_设置图片矩形*/(IUIDrawMgrPtr* _self/*纹理ID*/, int left/*left*/, int top/*top*/, int width/*right*/, int height/*bottom*/);
/*API*/VOID		WINAPI	UIDrawMgr_image_mode/*渲染2D_设置图片矩形*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR szMode/*图片配置信息*/);
/*API*/VOID		WINAPI	UIDrawMgr_image_uvmap/*渲染2D_设置图片矩形*/(IUIDrawMgrPtr* _self/*纹理ID*/, int width/*width*/, int height/*height*/, int wcount/*左右数量*/, int hcount/*上下数量*/);
/*API*/VOID		WINAPI	UIDrawMgr_image_uvani/*渲染2D_设置图片矩形*/(IUIDrawMgrPtr* _self/*纹理ID*/, int start/*开始索引*/, int count/*图片数量*/, float speed/*播放速度*/);//初始化uv动画
/*API*/VOID		WINAPI	UIDrawMgr_number_mode/*渲染2D_设置数字渲染模式*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR szModeFirst, int numberHeight/*数字高度*/, int spaceX/*宽度字间隔*/);
/*API*/VOID		WINAPI	UIDrawMgr_number_align/*渲染2D_设置数字对齐模式*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR szModeFirst, LPCSTR szAlign/*对齐模式*/);//szAlign可以是(left,top,right,bottom,center,vcenter,middle)其中middle是绝对中间位置是默认值
/*API*/VOID		WINAPI	UIDrawMgr_Setnumber/*渲染2D_设置数字渲染模式*/(IUIDrawMgrPtr* _self/*纹理ID*/, int num/*设置数字*/);
/*API*/INT		WINAPI	UIDrawMgr_Getnumber/*渲染2D_返回数字渲染模式*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/VOID		WINAPI	UIDrawMgr_Settext/*渲染2D_设置数字渲染模式*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR text/*设置数字*/);
/*API*/LPCSTR	WINAPI	UIDrawMgr_Gettext/*渲染2D_返回数字渲染模式*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/VOID		WINAPI	UIDrawMgr_add_child/*渲染2D_添加子渲染*/(IUIDrawMgrPtr* _self/*纹理ID*/, IUIDrawMgrPtr* _return/*返回*/);//添加一个子渲染，从这个渲染继承
/*API*/VOID		WINAPI	UIDrawMgr_delete_child/*渲染2D_添加子渲染*/(IUIDrawMgrPtr* _self/*纹理ID*/, INT nIndex/*索引*/);//删除一个子渲染
/*API*/VOID		WINAPI	UIDrawMgr_get_child/*渲染2D_添加子渲染*/(IUIDrawMgrPtr* _self/*纹理ID*/, INT nIndex/*索引*/, IUIDrawMgrPtr* _return/*返回*/);//返回索引个子渲染
/*API*/INT		WINAPI	UIDrawMgr_child_count/*渲染2D_子渲染数量*/(IUIDrawMgrPtr* _self/*纹理ID*/);
//
/*API*/VOID		WINAPI	UIDrawMgr_Setrotx/*渲染2D_转动x*/(IUIDrawMgrPtr* _self/*纹理ID*/, int fDegress/*角度*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setroty/*渲染2D_转动x*/(IUIDrawMgrPtr* _self/*纹理ID*/, int fDegress/*角度*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setrotz/*渲染2D_转动x*/(IUIDrawMgrPtr* _self/*纹理ID*/, int fDegress/*角度*/);
/*API*/int		WINAPI	UIDrawMgr_Getrotx/*渲染2D_转动x*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/int		WINAPI	UIDrawMgr_Getroty/*渲染2D_转动x*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/int		WINAPI	UIDrawMgr_Getrotz/*渲染2D_转动x*/(IUIDrawMgrPtr* _self/*纹理ID*/);
//
/*API*/VOID		WINAPI	UIDrawMgr_Setcenter/*渲染2D_设置中心坐标*/(IUIDrawMgrPtr* _self/*纹理ID*/, Pixel* center/*center*/);
/*API*/VOID		WINAPI	UIDrawMgr_Getcenter/*渲染2D_取得中心坐标*/(IUIDrawMgrPtr* _self/*纹理ID*/, Pixel* _return/*ret*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setpos/*渲染2D_设置位置*/(IUIDrawMgrPtr* _self/*纹理ID*/, Pixel* center/*pos*/);
/*API*/VOID		WINAPI	UIDrawMgr_Getpos/*渲染2D_取得位置*/(IUIDrawMgrPtr* _self/*纹理ID*/, Pixel* _return/*ret*/);
/*API*/VOID		WINAPI	UIDrawMgr_draw_rect/*渲染2D_取得矩形区域*/(IUIDrawMgrPtr* _self/*纹理ID*/, Rect* _return/*pos*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setcolor/*渲染2D_设置颜色*/(IUIDrawMgrPtr* _self/*纹理ID*/, color4* c/*color*/);
/*API*/VOID		WINAPI	UIDrawMgr_Getcolor/*渲染2D_取得颜色*/(IUIDrawMgrPtr* _self/*纹理ID*/, color4* _return/*ret*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setopacity/*渲染2D_设置透明*/(IUIDrawMgrPtr* _self/*纹理ID*/, float fAlpha/*fAlpha*/);
/*API*/float	WINAPI	UIDrawMgr_Getopacity/*渲染2D_取得透明*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/VOID		WINAPI	UIDrawMgr_Sethighlight/*渲染2D_设置高亮*/(IUIDrawMgrPtr* _self/*纹理ID*/, int highlight/*nHighlight*/);
/*API*/int		WINAPI	UIDrawMgr_Gethighlight/*渲染2D_取得高亮*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setdisplay/*渲染2D_设置显示*/(IUIDrawMgrPtr* _self/*纹理ID*/, BOOL bDisplay/*bDisplay*/);
/*API*/BOOL		WINAPI	UIDrawMgr_Getdisplay/*渲染2D_取得显示*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setenabled/*渲染2D_设置有效*/(IUIDrawMgrPtr* _self/*纹理ID*/, BOOL bEnabled/*bEnabled*/);
/*API*/BOOL		WINAPI	UIDrawMgr_Getenabled/*渲染2D_取得有效*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setlayer/*渲染2D_设置渲染层次*/(IUIDrawMgrPtr* _self/*纹理ID*/, int layer/*layer*/);
/*API*/int		WINAPI	UIDrawMgr_Getlayer/*渲染2D_取得渲染层次*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setcullmode/*渲染2D_设置渲染面*/(IUIDrawMgrPtr* _self/*纹理ID*/, CullMode layer/*layer*/);
/*API*/CullMode	WINAPI	UIDrawMgr_Getcullmode/*渲染2D_取得渲染面*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setscale/*渲染2D_设置縮放*/(IUIDrawMgrPtr* _self/*纹理ID*/, float fScale/*fScale*/);
/*API*/float	WINAPI	UIDrawMgr_Getscale/*渲染2D_取得縮放*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setscalex/*渲染2D_设置縮放x*/(IUIDrawMgrPtr* _self/*纹理ID*/, float fScalex/*fScalex*/);
/*API*/float	WINAPI	UIDrawMgr_Getscalex/*渲染2D_取得縮放x*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setscaley/*渲染2D_设置縮放y*/(IUIDrawMgrPtr* _self/*纹理ID*/, float fScaley/*fScaley*/);
/*API*/float	WINAPI	UIDrawMgr_Getscaley/*渲染2D_取得縮放y*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/VOID		WINAPI	UIDrawMgr_Setoffset3d_z/*渲染2D_位移3D坐标Z*/(IUIDrawMgrPtr* _self/*纹理ID*/, float fOffsetZ/*z*/);
/*API*/float	WINAPI	UIDrawMgr_Getoffset3d_z/*渲染2D_位移3D坐标Z*/(IUIDrawMgrPtr* _self/*纹理ID*/);
//
/*API*/VOID		WINAPI	UIDrawMgr_get_point/*渲染2D_取得点*/(IUIDrawMgrPtr* _self/*纹理ID*/, int index/*点索引*/, Pixel* _return/*y位置*/);//最多可以设置最多count个点
/*API*/VOID		WINAPI	UIDrawMgr_get_uv/*渲染2D_取得uv*/(IUIDrawMgrPtr* _self/*纹理ID*/, int index/*点索引*/, float2* _return/*v坐标*/);//最多可以设置count个点
/*API*/VOID		WINAPI	UIDrawMgr_get_color/*渲染2D_取得颜色*/(IUIDrawMgrPtr* _self/*纹理ID*/, int index/*点索引*/, color4* _return/*颜色*/);//最多可以设置count个点
/*API*/VOID		WINAPI	UIDrawMgr_set_rect/*渲染2D_设置矩形*/(IUIDrawMgrPtr* _self/*纹理ID*/, int left/*left*/, int top/*left*/, int right/*right*/, int bottom/*bottom*/);
/*API*/VOID		WINAPI	UIDrawMgr_set_point/*渲染2D_设置点*/(IUIDrawMgrPtr* _self/*纹理ID*/, int index/*点索引*/, int x/*x位置*/, int y/*y位置*/);//最多可以设置最多count个点
/*API*/VOID		WINAPI	UIDrawMgr_set_uv/*渲染2D_设置uv*/(IUIDrawMgrPtr* _self/*纹理ID*/, int index/*点索引*/, float u/*u坐标*/, float v/*v坐标*/);//最多可以设置count个点
/*API*/VOID		WINAPI	UIDrawMgr_set_color/*渲染2D_设置颜色*/(IUIDrawMgrPtr* _self/*纹理ID*/, int index/*点索引*/, color4* c/*颜色*/);//最多可以设置count个点
/*API*/VOID		WINAPI	UIDrawMgr_Setclickscale/*渲染2D_点击缩放值*/(IUIDrawMgrPtr* _self/*纹理ID*/, float fscale/*点击缩放值*/);
/*API*/float	WINAPI	UIDrawMgr_Getclickscale/*渲染2D_点击缩放值*/(IUIDrawMgrPtr* _self/*纹理ID*/);
/*API*/VOID		WINAPI	UIDrawMgr_bind_click/*渲染2D_绑定点击*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR szFunc/*脚本函数*/);//,事件[szFunc(obj)--(obj=UIDrawMgr)控件单击事件]
/*API*/VOID		WINAPI	UIDrawMgr_bind_dblclick/*渲染2D_双击点击*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR szFunc/*脚本函数*/);//,事件[szFunc(obj)--(obj=UIDrawMgr)控件双击事件]
/*API*/VOID		WINAPI	UIDrawMgr_text_mode/*渲染2D_设置文本*/(IUIDrawMgrPtr* _self/*接口*/, LPCSTR szValue/*值*/, float fFontScale/*字体缩放*/);//默认是有效的
/*API*/VOID		WINAPI	UIDrawMgr_progress_mode/*渲染2D_设置进度条*/(IUIDrawMgrPtr* _self/*接口*/, float fValue/*值*/);//默认是有效的
/*API*/float	WINAPI	UIDrawMgr_progress/*渲染2D_设置进度条*/(IUIDrawMgrPtr* _self/*接口*/);//默认是有效的


/*API*/INT		WINAPI	Table_linecount/*表格数据_取行数*/(ITableReaderPtr* _self/*指针*/);
/*API*/LPCSTR	WINAPI	Table_read_t/*表格数据_读文本*/(ITableReaderPtr* _self/*指针*/, INT row/*行索引*/, int col/*列索引*/);
/*API*/INT		WINAPI	Table_read_i/*表格数据_读整数*/(ITableReaderPtr* _self/*指针*/, INT row/*行索引*/, int col/*列索引*/);
/*API*/float	WINAPI	Table_read_f/*表格数据_读小数*/(ITableReaderPtr* _self/*指针*/, INT row/*行索引*/, int col/*列索引*/);
/*API*/LPCSTR	WINAPI	Table_id_t/*表格数据_读文本根据ID*/(ITableReaderPtr* _self/*指针*/, INT id/*ID值*/, LPCSTR colName/*返回列名*/);
/*API*/INT		WINAPI	Table_id_i/*表格数据_读整数根据ID*/(ITableReaderPtr* _self/*指针*/, INT id/*ID值*/, LPCSTR colName/*返回列名*/);
/*API*/float	WINAPI	Table_id_f/*表格数据_读小数根据ID*/(ITableReaderPtr* _self/*指针*/, INT id/*ID值*/, LPCSTR colName/*返回列名*/);
/*API*/LPCSTR	WINAPI	Table_name_t/*表格数据_读文本根据Name*/(ITableReaderPtr* _self/*指针*/, LPCSTR szName/*名值*/, LPCSTR colName/*返回列名*/);
/*API*/INT		WINAPI	Table_name_i/*表格数据_读整数根据Name*/(ITableReaderPtr* _self/*指针*/, LPCSTR szName/*名值*/, LPCSTR colName/*返回列名*/);
/*API*/float	WINAPI	Table_name_f/*表格数据_读小数根据Name*/(ITableReaderPtr* _self/*指针*/, LPCSTR szName/*名值*/, LPCSTR colName/*返回列名*/);
/*API*/LPCSTR	WINAPI	Table_search_t/*表格数据_读文本根据搜索*/(ITableReaderPtr* _self/*指针*/, LPCSTR st_colName/*列标题*/, LPCSTR st_rowValue/*行值*/, LPCSTR colName/*返回列名*/);//搜索szHeaderName列的szLineValue行，然后返回szFindHeader列的值
/*API*/INT		WINAPI	Table_search_i/*表格数据_读整数根据搜索*/(ITableReaderPtr* _self/*指针*/, LPCSTR st_colName/*列标题*/, LPCSTR st_rowValue/*行值*/, LPCSTR colName/*返回列名*/);//搜索szHeaderName列的szLineValue行，然后返回szFindHeader列的值
/*API*/float	WINAPI	Table_search_f/*表格数据_读小数根据搜索*/(ITableReaderPtr* _self/*指针*/, LPCSTR st_colName/*列标题*/, LPCSTR st_rowValue/*行值*/, LPCSTR colName/*返回列名*/);//搜索szHeaderName列的szLineValue行，然后返回szFindHeader列的值
/*API*/INT		WINAPI	Table_search_col/*表格数据_取列标题索引*/(ITableReaderPtr* _self/*指针*/, LPCSTR colName/*列标题*/);



/*API*/VOID		WINAPI	UIElement_Setimage/*界面元素_设置纹理*/(IElementPtr* _self/*接口*/, LPCSTR szFileName/*纹理文件*/);//
/*API*/LPCSTR	WINAPI	UIElement_Getimage/*界面元素_取得纹理*/(IElementPtr* _self/*接口*/);//
/*API*/INT		WINAPI	UIElement_Gettextureid/*界面元素_取纹理ID*/(IElementPtr* _self/*接口*/);//在界面管理器中能找到对应的纹理文件
/*API*/VOID		WINAPI	UIElement_Settextureid/*界面元素_设置纹理ID*/(IElementPtr* _self/*接口*/, INT nID/*id*/);//在界面管理器中能找到对应的纹理ID
/*API*/VOID		WINAPI	UIElement_Setfontid/*界面元素_设置字体ID*/(IElementPtr* _self/*接口*/, INT nFontID/*字体ID*/);
/*API*/INT		WINAPI	UIElement_Getfontid/*界面元素_取字体ID*/(IElementPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIElement_Gettexturerect/*界面元素_取得纹理区域*/(IElementPtr* _self/*接口*/, Rect* _return/*ret*/);
/*API*/VOID		WINAPI	UIElement_Settexturerect/*界面元素_设置纹理区域*/(IElementPtr* _self/*接口*/, const Rect* _s/*矩形*/);
/*API*/LPCSTR	WINAPI	UIElement_Gettexturemode/*界面元素_取得方案名字*/(IElementPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIElement_Settexturemode/*界面元素_设置方案名字*/(IElementPtr* _self/*接口*/, LPCSTR szMode/*模式*/);
/*API*/VOID		WINAPI	UIElement_Gettextcolor/*界面元素_取文本颜色*/(IElementPtr* _self/*接口*/,color4* _return/*ret*/);
/*API*/VOID		WINAPI	UIElement_Settextcolor/*界面元素_设置文本颜色*/(IElementPtr* _self/*接口*/, const color4* c/*颜色*/);
/*API*/VOID		WINAPI	UIElement_Getimagecolor/*界面元素_取纹理颜色*/(IElementPtr* _self/*接口*/, color4* _return/*ret*/);
/*API*/VOID		WINAPI	UIElement_Setimagecolor/*界面元素_设置纹理颜色*/(IElementPtr* _self/*接口*/, const color4* c/*颜色*/);
/*API*/BOOL		WINAPI	UIElement_isscale/*界面元素_是否缩放*/(IElementPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIElement_setscale/*界面元素_设置缩放*/(IElementPtr* _self/*接口*/, BOOL bScale/*缩放*/, BOOL bWrap/*可扩展*/, INT wrapWidth/*扩展宽度*/, INT wrapHeight/*扩展高度*/);
/*API*/INT			WINAPI	UIElement_uvactioncount/*界面元素_取UV动画数量*/(IElementPtr* _self/*接口*/);
/*API*/LPCSTR		WINAPI	UIElement_uvaction_at/*界面元素_取UV动画名字*/(IElementPtr* _self/*接口*/, INT nIndex/*索引*/);
/*API*/LPCSTR		WINAPI	UIElement_Getuvaction/*界面元素_取激活UV动画*/(IElementPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIElement_Setuvaction/*界面元素_设置激活UV动画*/(IElementPtr* _self/*接口*/, LPCSTR map/*UV动画名字*/);
/*API*/VOID		WINAPI	UIElement_Settilewidth/*界面元素_设置横向平铺*/(IElementPtr* _self/*接口*/, INT nTileWidth/*横向平铺*/);
/*API*/VOID		WINAPI	UIElement_Settileheight/*界面元素_设置纵向平铺*/(IElementPtr* _self/*接口*/, INT nTileHeight/*纵向平铺*/);
/*API*/INT		WINAPI	UIElement_Gettilewidth/*界面元素_取横向平铺*/(IElementPtr* _self/*接口*/);
/*API*/INT		WINAPI	UIElement_Gettileheight/*界面元素_取纵向平铺*/(IElementPtr* _self/*接口*/);
//
/*API*/VOID		WINAPI	UIElement_Setuvspeed/*界面元素_设置UV播放速度*/(IElementPtr* _self/*接口*/, float f/*播放速度*/);
/*API*/float	WINAPI	UIElement_Getuvspeed/*界面元素_取UV播放速度*/(IElementPtr* _self/*接口*/);
/*API*/BlendMode	WINAPI	UIElement_Getsrcblend/*界面元素_取源混合*/(IElementPtr* _self/*接口*/);
/*API*/BlendMode	WINAPI	UIElement_Getdestblend/*界面元素_取目标混合*/(IElementPtr* _self/*接口*/);
/*API*/TextureBlend	WINAPI	UIElement_Gettextureblend/*界面元素_取图片混合*/(IElementPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIElement_Setsrcblend/*界面元素_设置源混合*/(IElementPtr* _self/*接口*/, BlendMode m/*方式*/);
/*API*/VOID		WINAPI	UIElement_Setdestblend/*界面元素_设置目标混合*/(IElementPtr* _self/*接口*/, BlendMode m/*方式*/);
/*API*/VOID		WINAPI	UIElement_Settextureblend/*界面元素_设置图片混合*/(IElementPtr* _self/*接口*/, TextureBlend op/*方式*/);

//
/*API*/VOID		WINAPI	UIControl_Setenabled/*界面控件_设置有效*/(IControlPtr* _self/*接口*/, BOOL b/*有效*/);
/*API*/BOOL		WINAPI	UIControl_Getenabled/*界面控件_是否有效*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setdisplay/*界面控件_设置显示*/(IControlPtr* _self/*接口*/, BOOL b/*有效*/);
/*API*/BOOL		WINAPI	UIControl_Getdisplay/*界面控件_是否显示*/(IControlPtr* _self/*接口*/);
/*API*/UIControlType		WINAPI	UIControl_gettype/*界面控件_取控件类型*/(IControlPtr* _self/*接口*/);
/*API*/INT		WINAPI	UIControl_getId/*界面控件_取控件ID*/(IControlPtr* _self/*接口*/);
/*API*/BOOL		WINAPI	UIControl_Getmovedialog/*界面控件_设置移动对话框*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setmovedialog/*界面控件_取移动对话框ID*/(IControlPtr* _self/*接口*/, BOOL b/*移动对话框*/);

/*API*/BOOL		WINAPI	UIControl_Getpixelpick/*界面控件_设置像素级点击*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setpixelpick/*界面控件_取像素级点击ID*/(IControlPtr* _self/*接口*/, BOOL b/*移动对话框*/);
/*API*/BOOL		WINAPI	UIControl_pick/*界面控件_像素级点击*/(IControlPtr* _self/*接口*/, INT x, INT y);//像素级别的点击函数

/*API*/INT		WINAPI	UIControl_Getleft/*界面控件_取左*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setleft/*界面控件_设置左*/(IControlPtr* _self/*接口*/, INT nValue/*值*/);
/*API*/INT		WINAPI	UIControl_Gettop/*界面控件_取上*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Settop/*界面控件_设置上*/(IControlPtr* _self/*接口*/, INT nValue/*值*/);
/*API*/INT		WINAPI	UIControl_Getwidth/*界面控件_取宽*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setwidth/*界面控件_设置宽*/(IControlPtr* _self/*接口*/, INT nValue/*值*/);
/*API*/INT		WINAPI	UIControl_Getheight/*界面控件_取高*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setheight/*界面控件_设置高*/(IControlPtr* _self/*接口*/, INT nValue/*值*/);
/*API*/INT		WINAPI	UIControl_Getright/*界面控件_取右*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setright/*界面控件_设置右*/(IControlPtr* _self/*接口*/, INT nValue/*值*/);
/*API*/INT		WINAPI	UIControl_Getbottom/*界面控件_取下*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setbottom/*界面控件_设置下*/(IControlPtr* _self/*接口*/, INT nValue/*值*/);

/*API*/INT		WINAPI	UIControl_Getmoveleft/*界面控件_取左*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setmoveleft/*界面控件_设置左*/(IControlPtr* _self/*接口*/, INT nValue/*值*/);
/*API*/INT		WINAPI	UIControl_Getmovetop/*界面控件_取上*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setmovetop/*界面控件_设置上*/(IControlPtr* _self/*接口*/, INT nValue/*值*/);
/*API*/INT		WINAPI	UIControl_Getmoveright/*界面控件_取右*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setmoveright/*界面控件_设置右*/(IControlPtr* _self/*接口*/, INT nValue/*值*/);
/*API*/INT		WINAPI	UIControl_Getmovebottom/*界面控件_取下*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setmovebottom/*界面控件_设置下*/(IControlPtr* _self/*接口*/, INT nValue/*值*/);

/*API*/VOID		WINAPI	UIControl_clone/*界面控件_克隆*/(IControlPtr* _self/*接口*/, float fTime/*克隆周期，如果是负的则是无限时间*/, IControlPtr* _return/*ret*/);//克隆一个对象，并且在fTime时间之后自动失效
/*API*/VOID		WINAPI	UIControl_cloneto/*界面控件_克隆到其他对话框*/(IControlPtr* _self/*接口*/, float fTime/*克隆周期，如果是负的则是无限时间*/, IDialogPtr* dest/*目标对话框*/, IControlPtr* _return/*ret*/);
/*API*/VOID		WINAPI	UIControl_Setname/*界面控件_设置名字*/(IControlPtr* _self/*接口*/, LPCSTR szName/*名字*/);
/*API*/LPCSTR	WINAPI	UIControl_Getname/*界面控件_取名字*/(IControlPtr* _self/*接口*/);
/*API*/DrawTextAlign		WINAPI	UIControl_Gettextalign/*界面控件_设置文本格式*/(IControlPtr* _self/*接口*/);//
/*API*/VOID		WINAPI	UIControl_Settextalign/*界面控件_设置文本格式*/(IControlPtr* _self/*接口*/, DrawTextAlign fmt/*格式*/);//
/*API*/VOID		WINAPI	UIControl_Getrect/*界面控件_取控件区域*/(IControlPtr* _self/*接口*/, Rect* _return/*ret*/);
/*API*/VOID		WINAPI	UIControl_Setrect/*界面控件_设置控件区域*/(IControlPtr* _self/*接口*/, const Rect* rc/*矩形区域*/);
//
/*API*/INT		WINAPI	UIControl_Getlayer/*界面控件_取层级*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setlayer/*界面控件_设置层级*/(IControlPtr* _self/*接口*/, INT layer/*层级*/);
/*API*/INT		WINAPI	UIControl_elementcount/*界面控件_取元素数量*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_element_at/*界面控件_取元素*/(IControlPtr* _self/*接口*/, INT index/*索引*/, IElementPtr* _return/*ret*/);
//
/*API*/VOID		WINAPI	UIControl_Settextcolor/*界面控件_设置文本颜色*/(IControlPtr* _self/*接口*/, const color4* c/*颜色*/);//
/*API*/VOID		WINAPI	UIControl_Gettextcolor/*界面控件_取文本颜色*/(IControlPtr* _self/*接口*/, color4* _return/*ret*/);//
/*API*/VOID		WINAPI	UIControl_Setimagecolor/*界面控件_设置纹理颜色*/(IControlPtr* _self/*接口*/, const color4* c/*颜色*/);//
/*API*/VOID		WINAPI	UIControl_Getimagecolor/*界面控件_取纹理颜色*/(IControlPtr* _self/*接口*/, color4* _return/*ret*/);//
/*API*/VOID		WINAPI	UIControl_Setimage/*界面控件_设置纹理*/(IControlPtr* _self/*接口*/, LPCSTR szFile/*图片路径*/);//设置所有元素的图片
/*API*/LPCSTR	WINAPI	UIControl_Getimage/*界面控件_取纹理*/(IControlPtr* _self/*接口*/);//取第一个元素的图片
/*API*/VOID		WINAPI	UIControl_Setcolor/*界面控件_设置颜色*/(IControlPtr* _self/*接口*/, const color4* c/*颜色*/);//
/*API*/VOID		WINAPI	UIControl_Getcolor/*界面控件_取颜色*/(IControlPtr* _self/*接口*/, color4* _return/*ret*/);//
/*API*/VOID		WINAPI	UIControl_Setopacity/*界面控件_设置透明值*/(IControlPtr* _self/*接口*/, float v/*颜色*/);//
/*API*/float	WINAPI	UIControl_Getopacity/*界面控件_取透明值*/(IControlPtr* _self/*接口*/);//
/*API*/VOID		WINAPI	UIControl_Setscroll_v/*界面控件_设置纵向滚动条*/(IControlPtr* _self/*接口*/, IControlPtr* scrollV/*滚动条纵向*/);//设置子窗体在控件内部显示滚动条
/*API*/VOID		WINAPI	UIControl_Getscroll_v/*界面控件_设置纵向滚动条*/(IControlPtr* _self/*接口*/, IControlPtr* _return/*滚动条纵向*/);//设置子窗体在控件内部显示滚动条
/*API*/VOID		WINAPI	UIControl_Setscroll_h/*界面控件_设置横向滚动条*/(IControlPtr* _self/*接口*/, IControlPtr* scrollH/*滚动条纵向*/);//设置子窗体在控件内部显示滚动条
/*API*/VOID		WINAPI	UIControl_Getscroll_h/*界面控件_设置横向滚动条*/(IControlPtr* _self/*接口*/, IControlPtr* _return/*滚动条纵向*/);//设置子窗体在控件内部显示滚动条
/*API*/VOID		WINAPI	UIControl_Setscroll_min/*界面控件_滚动条最小*/(IControlPtr* _self/*接口*/, INT nMin/*滚动条最小*/);//
/*API*/VOID		WINAPI	UIControl_Setscroll_max/*界面控件_滚动条最大*/(IControlPtr* _self/*接口*/, INT nMax/*滚动条最大*/);//
/*API*/INT		WINAPI	UIControl_Getscroll_min/*界面控件_滚动条最小*/(IControlPtr* _self/*接口*/);
/*API*/INT		WINAPI	UIControl_Getscroll_max/*界面控件_滚动条最大*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_setChild/*界面控件_设置子窗体*/(IControlPtr* _self/*接口*/, IDialogPtr* dlg/*子窗体*/);//设置子窗体在控件内部显示
/*API*/VOID		WINAPI	UIControl_removeChild/*界面控件_取消子窗体*/(IControlPtr* _self/*接口*/);////设置子窗体在控件内部显示
/*API*/VOID		WINAPI	UIControl_Setscroll_speed/*界面控件_滚动速度*/(IControlPtr* _self/*接口*/, float fSpeed/*滚动速度*/);//设置滚动条滚动速度
/*API*/float	WINAPI	UIControl_Getscroll_speed/*界面控件_取滚动速度*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setscroll_firstspeed/*界面控件_滚动初始速度*/(IControlPtr* _self/*接口*/, float fSpeed/*滚动速度*/);//设置滚动条滚动初始速度
/*API*/float	WINAPI	UIControl_Getscroll_firstspeed/*界面控件_取滚动初始速度*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setscroll_extend/*界面控件_滚动条是否可以拽出*/(IControlPtr* _self/*接口*/, BOOL bExtend/*滚动拽出*/);//是否可拽出
/*API*/BOOL		WINAPI	UIControl_Getscroll_extend/*界面控件_取滚动速度*/(IControlPtr* _self/*接口*/);
//
/*API*/VOID		WINAPI	UIControl_Setgroupname/*界面控件_设置组*/(IControlPtr* _self/*接口*/, LPCSTR szName/*名字*/);
/*API*/LPCSTR	WINAPI	UIControl_Getgroupname/*界面控件_取组*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setgroupid/*界面控件_设置组ID*/(IControlPtr* _self/*接口*/, INT nID/*ID*/);
/*API*/INT		WINAPI	UIControl_Getgroupid/*界面控件_取组ID*/(IControlPtr* _self/*接口*/);

/*API*/VOID		WINAPI	UIControl_progress_time/*界面控件_时间内转进度条*/(IControlPtr* _self/*接口*/, float fTimeSecond/*秒*/);//在这个指定的秒内转完进度条,自动显示并在播放完毕之后自动隐藏
/*API*/VOID		WINAPI	UIControl_Setcenter/*界面控件_设置中心位置*/(IControlPtr* _self/*接口*/, const Pixel* pos/*ID*/);
/*API*/VOID		WINAPI	UIControl_Getcenter/*界面控件_取得中心位置*/(IControlPtr* _self/*接口*/, Pixel* _return/*ret*/);

/*API*/float	WINAPI	UIControl_Getrot/*界面控件_设置转动率*/(IControlPtr* _self/*接口*/);//
/*API*/VOID		WINAPI	UIControl_Setrot/*界面控件_设置转动率*/(IControlPtr* _self/*接口*/, float fDegress/*角度*/);//
/*API*/BOOL		WINAPI	UIControl_Getrotenable/*界面控件_设置转动率*/(IControlPtr* _self/*接口*/);//
/*API*/VOID		WINAPI	UIControl_Setrotenable/*界面控件_设置转动率*/(IControlPtr* _self/*接口*/, BOOL bEnable/*设置*/);//
/*API*/VOID		WINAPI	UIControl_Setrotcenter/*界面控件_设置转动中心*/(IControlPtr* _self/*接口*/, const Pixel* v/*相对于控件的中心位置*/);
/*API*/VOID		WINAPI	UIControl_Getrotcenter/*界面控件_取转动中心*/(IControlPtr* _self/*接口*/, Pixel* _return/*相对于控件的中心位置*/);
//
/*API*/LPCSTR	WINAPI	UIControl_Gettext/*界面控件_取文本*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Settext/*界面控件_设置文本*/(IControlPtr* _self/*接口*/, LPCSTR szText/*文本*/);
/*API*/VOID		WINAPI	UIControl_Setactive/*界面控件_设置是否可以被激活*/(IControlPtr* _self/*接口*/, BOOL b/*设置*/);
/*API*/BOOL		WINAPI	UIControl_Getactive/*界面控件_取是否可以被激活*/(IControlPtr* _self/*接口*/);
/*API*/BOOL		WINAPI	UIControl_isfocus/*界面控件_取是否被激活*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_seteventsound/*界面控件_设置2D声效*/(IControlPtr* _self/*接口*/, UISoundPlay st/*类型*/, LPCSTR szFile/*文件*/);
/*API*/LPCSTR	WINAPI	UIControl_geteventsound/*界面控件_取2D声效*/(IControlPtr* _self/*接口*/, UISoundPlay st/*类型*/);
/*API*/VOID		WINAPI	UIControl_Getgroup/*界面控件_取控件组*/(IControlPtr* _self/*接口*/, IUIGroupPtr* _return/*ret*/);//
/*API*/VOID		WINAPI	UIControl_Setgroup/*界面控件_设置控件组*/(IControlPtr* _self/*接口*/, IUIGroupPtr* g/*ret*/);//
/*API*/ActionType	WINAPI	UIControl_Getaction/*界面控件_取播放动作*/(IControlPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIControl_Setaction/*界面控件_设置播放动作*/(IControlPtr* _self/*接口*/, ActionType nType/*动作*/);
/*API*/ActionType	WINAPI	UIControl_Getnextaction/*界面控件_取下一个播放动作*/(IControlPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIControl_Setnextaction/*界面控件_设置下一个播放动作*/(IControlPtr* _self/*接口*/, ActionType nType/*动作*/);
/*API*/float		WINAPI	UIControl_Getactiontime/*界面控件_取播放动作时间*/(IControlPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIControl_Setactiontime/*界面控件_设置播放动作时间*/(IControlPtr* _self/*接口*/, float f/*动作*/);
/*API*/VOID			WINAPI	UIControl_dialog/*界面控件_取对话框*/(IControlPtr* _self/*接口*/, IDialogPtr* _return/*ret*/);
/*API*/LPCSTR		WINAPI	UIControl_Getactionby/*界面控件_取当前动作根据名称*/(IControlPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIControl_Setactionby/*界面控件_设置当前动作根据名称*/(IControlPtr* _self/*接口*/, LPCSTR ani/*当前动作*/);
/*API*/float		WINAPI	UIControl_Getspeed/*界面控件_取得动作播放速度*/(IControlPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIControl_Setspeed/*界面控件_设置动作播放速度*/(IControlPtr* _self/*接口*/, float fSpeed/*速度*/);//1=正常速度
/*API*/float		WINAPI	UIControl_Getwaitbefore_actionplay/*界面控件_取得播放前等待*/(IControlPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIControl_Setwaitbefore_actionplay/*界面控件_设置播放前等待*/(IControlPtr* _self/*接口*/, float fTime/*等待时间*/);
/*API*/float	WINAPI	UIControl_Getrot2d/*界面控件_取得角度*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_Setrot2d/*界面控件_设置角度*/(IControlPtr* _self/*接口*/, float fDegress/*角度*/);
/*API*/VOID		WINAPI	UIControl_rot2d_dir/*界面控件_设置角度根据方向*/(IControlPtr* _self/*接口*/, const float2* dir/*角度*/);

/*API*/float		WINAPI	UIControl_Getscale/*界面控件_缩放*/(IControlPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIControl_Setscale/*界面控件_缩放*/(IControlPtr* _self/*接口*/, float fScale/*缩放*/);//1=正常速度
/*API*/float		WINAPI	UIControl_Getscalex/*界面控件_缩放x*/(IControlPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIControl_Setscalex/*界面控件_缩放x*/(IControlPtr* _self/*接口*/, float fScale/*缩放*/);//1=正常速度
/*API*/float		WINAPI	UIControl_Getscaley/*界面控件_缩放y*/(IControlPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIControl_Setscaley/*界面控件_缩放y*/(IControlPtr* _self/*接口*/, float fScale/*缩放*/);//1=正常速度
/*API*/VOID			WINAPI	UIControl_bind_draw/*界面控件_绑定渲染队列*/(IControlPtr* _self/*接口*/, IUIDrawMgrPtr* drawPtr/*组合*/);//绑定渲染队列到某个控件
/*API*/VOID			WINAPI	UIControl_bind_image/*界面控件_绑定图片*/(IControlPtr* _self/*接口*/, LPCSTR szImageFile/*图片文件路径*/);//只绑定到第一个图片，其他图片自己调用element元素类的相关设置
/*API*/VOID			WINAPI	UIControl_bind_eventby/*界面控件_绑定脚本到按下事件*/(IControlPtr* _self/*接口*/, LPCSTR szEvent/*事件名*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//绑定一个事件,事件查看幻影脚本帮助中的eventid枚举常数说明,事件[szLuaFunc(obj,eid,pos)--(obj=UIControl,eid=int,pos=Pixel)控件事件,eid=EventID]
/*API*/VOID			WINAPI	UIControl_bind_event/*界面控件_绑定脚本到按下事件*/(IControlPtr* _self/*接口*/, EventID nid/*事件id*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//绑定一个事件,事件查看幻影脚本帮助中的eventid枚举常数说明,事件[szLuaFunc(obj,eid,pos)--(obj=UIControl,eid=int,pos=Pixel)控件事件,eid=EventID]
/*API*/VOID			WINAPI	UIControl_bind_click/*界面控件_绑定脚本到事件*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//,事件[szLuaFunc(obj,eid,pos)--(obj=UIControl,eid=int,pos=Pixel)控件事件,eid=EventID]
/*API*/VOID			WINAPI	UIControl_sort_draws/*界面控件_排序渲染列表*/(IControlPtr* _self/*接口*/);

/*API*/VOID		WINAPI	UIControl_saved_rect/*界面控件_取载入时位置矩形区域*/(IControlPtr* _self/*接口*/, Rect* _return/*矩形*/);
/*API*/VOID		WINAPI	UIControl_saved_pos/*界面控件_取载入时位置信息*/(IControlPtr* _self/*接口*/, Pixel* _return/*矩形*/);
/*API*/float	WINAPI	UIControl_saved_rot/*界面控件_取载入时转动值*/(IControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIControl_reset_trans/*界面控件_重置所有位置和转动等信息到载入之初*/(IControlPtr* _self/*接口*/);//重置所有位置和转动等信息到载入之初

/*API*/VOID			WINAPI	UIControl_bind_3d_maxdist/*界面控件_绑定3D的最远距离显示设置*/(IControlPtr* _self/*接口*/, float fMaxDist/*最远距离*/);//如果超过这个最远距离将不会显示出来，控件大小会按照这个距离自动调整，如果设置成小于1则始终固定大小，默认是100米
/*API*/VOID			WINAPI	UIControl_bind_3d_pos/*界面控件_绑定3D位置*/(IControlPtr* _self/*接口*/, float3* pos/*固定位置*/);//按照最新的镜头值自动调整3D位置转换2D位置
/*API*/VOID			WINAPI	UIControl_bind_3d_obj/*界面控件_绑定3D对象位置*/(IControlPtr* _self/*接口*/, IGameObjectPtr* obj, float fHeightAdd/*高度增加*/);//按照最新的镜头值自动调整3D位置转换2D位置
/*API*/VOID			WINAPI	UIControl_bind_3d_clear/*界面控件_绑定3D清除*/(IControlPtr* _self/*接口*/);//清除所有3D位置绑定

/*API*/VOID			WINAPI	UIControl_Setvalt/*界面控件_设置文本值*/(IControlPtr* _self/*接口*/, LPCSTR szValue/*值*/);//设置文本值
/*API*/VOID			WINAPI	UIControl_Setvali/*界面控件_设置整数值*/(IControlPtr* _self/*接口*/, int nValue/*值*/);//设置整数值，如果是进度条则设置0或1
/*API*/VOID			WINAPI	UIControl_Setvalf/*界面控件_设置浮点值*/(IControlPtr* _self/*接口*/, float fValue/*值*/);//设置浮点值,对不同的控件有不同的效果，比如进度条控件是0到1之间的值，拖动条也一样
/*API*/LPCSTR		WINAPI	UIControl_Getvalt/*界面控件_取得文本值*/(IControlPtr* _self/*接口*/);//返回文本值
/*API*/INT			WINAPI	UIControl_Getvali/*界面控件_取得整数值*/(IControlPtr* _self/*接口*/);//返回整数值，如果是选项框则返回0，1
/*API*/float		WINAPI	UIControl_Getvalf/*界面控件_取得浮点值*/(IControlPtr* _self/*接口*/);//返回浮点值，如果是进度条或者拖动条返回0到1之间
/*API*/VOID			WINAPI	UIControl_Setpos/*界面控件_设置对话框位置*/(IControlPtr* _self/*接口*/, Pixel* pos/*最新位置*/);
/*API*/VOID			WINAPI	UIControl_Getpos/*界面控件_取位置左*/(IControlPtr* _self/*接口*/, Pixel* _return/*矩形*/);
/*API*/VOID			WINAPI	UIControl_innermesh_clear/*界面控件_清除控件内部模型*/(IControlPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIControl_Setinnermesh/*界面控件_设置控件内部模型*/(IControlPtr* _self/*接口*/, IMeshPtr* mesh/*模型*/);//设置内部显示模型,设置之后会自动克隆一个新的模型,模型的大小和显示位置需要自己设置,使用pos在0点对齐
/*API*/VOID			WINAPI	UIControl_Getinnermesh/*界面控件_取控件内部模型*/(IControlPtr* _self/*接口*/, IMeshPtr* _return/*矩形*/);

/*API*/VOID			WINAPI	UIControl_mousepoint/*界面控件_按照控件对齐的鼠标点*/(IControlPtr* _self/*接口*/, Pixel* _return/*矩形*/);

/*API*/int			WINAPI	UIControl_Geteffect_loop/*界面控件_特效播放次数*/(IControlPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIControl_Seteffect_loop/*界面控件_特效播放次数*/(IControlPtr* _self/*接口*/, int nLoop/*播放时间*/);
/*API*/BOOL			WINAPI	UIControl_Geteffect_stop/*界面控件_停止释放粒子*/(IControlPtr* _self/*接口*/);
/*API*/VOID			WINAPI	UIControl_Seteffect_stop/*界面控件_停止释放粒子*/(IControlPtr* _self/*接口*/, BOOL bStop/*是否停止*/);

/*API*/INT			WINAPI	UIControl_timeout/*界面控件_定时引发*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/);//定时引发，只引发一次，毫秒记,脚本无参数,一个函数可以创建多个实例,事件[szLuaFunc(obj)--(obj=UIControl)定期事件,只引发一次];返回TimeID
/*API*/INT			WINAPI	UIControl_interval/*界面控件_间隔引发*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用次数*/);//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(obj,tm,endof,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
/*API*/INT			WINAPI	UIControl_framemove/*界面控件_循环引发*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/);//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(obj,tm,endof,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
/*API*/INT			WINAPI	UIControl_timeout2/*界面控件_定时引发带一个文本参数*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, LPCSTR param/*文本参数*/);//定时引发，只引发一次，毫秒记,代替以前的timeout_1,脚本第一个参数是传入的参数,文本,一个函数可以创建多个实例,事件[szLuaFunc(obj,param)--(obj=UIControl)传入的自定义参数];返回TimeID
/*API*/INT			WINAPI	UIControl_interval2/*界面控件_间隔引发带一个文本参数*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/);//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,代替以前的interval_1,参数为之有所改动,事件[szLuaFunc(obj,tm,endof,param,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID
/*API*/INT			WINAPI	UIControl_framemove2/*界面控件_循环引发*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/);//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,第二个参数是传入的参数,文本,一个函数只能创建一个,事件[szLuaFunc(obj,tm,endof,param,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID

/*API*/INT		WINAPI	FindWay_Getpointcount/*寻路_取路经点数量*/(IFindWayPtr* _self/*接口*/);
/*API*/VOID		WINAPI	FindWay_Setpointcount/*寻路_设置路经点数量*/(IFindWayPtr* _self/*接口*/, INT nCount/*路径点*/);
/*API*/BOOL		WINAPI	FindWay_Getis2dway/*寻路_取2D模式*/(IFindWayPtr* _self/*接口*/);
/*API*/VOID		WINAPI	FindWay_Setis2dway/*寻路_设置2D模式*/(IFindWayPtr* _self/*接口*/, BOOL bMode/*2D模式*/);
/*API*/VOID		WINAPI	FindWay_point_at/*寻路_取路经点*/(IFindWayPtr* _self/*接口*/, INT nIndex/*索引*/, float3* _return/*点*/);
/*API*/VOID		WINAPI	FindWay_setpoint_at/*寻路_设置路经点*/(IFindWayPtr* _self/*接口*/, INT nIndex/*索引*/, const float3* pos/*点*/);
/*API*/BOOL		WINAPI	FindWay_Getplay/*寻路_是否播放*/(IFindWayPtr* _self/*接口*/);
/*API*/VOID		WINAPI	FindWay_Setplay/*寻路_设置播放*/(IFindWayPtr* _self/*接口*/, BOOL bPlay/*播放*/);
/*API*/BOOL		WINAPI	FindWay_Getpause/*寻路_是否暂停*/(IFindWayPtr* _self/*接口*/);
/*API*/VOID		WINAPI	FindWay_Setpause/*寻路_设置暂停*/(IFindWayPtr* _self/*接口*/, BOOL bPause/*暂停*/);
/*API*/float	WINAPI	FindWay_Getspeed/*寻路_取得速度*/(IFindWayPtr* _self/*接口*/);
/*API*/VOID		WINAPI	FindWay_Setspeed/*寻路_设置速度*/(IFindWayPtr* _self/*接口*/, float fSpeed/*速度*/);
/*API*/BOOL		WINAPI	FindWay_recalc/*寻路_更新时间*/(IFindWayPtr* _self/*接口*/, float3* ret/*返回最新位置*/, floatPtr* retRotation/*返回转动角度*/);//如果返回false表示移动完毕

//IUIGroupPtr
/*API*/VOID		WINAPI	UIGroup_dialog/*界面控件组_是否显示*/(IUIGroupPtr* _self/*接口*/, IDialogPtr* _return/*取父对话框*/);

/*API*/BOOL		WINAPI	UIGroup_Getdisplay/*界面控件组_是否显示*/(IUIGroupPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIGroup_Setdisplay/*界面控件组_设置显示*/(IUIGroupPtr* _self/*接口*/, BOOL bVisible/*是否显示*/);
/*API*/BOOL		WINAPI	UIGroup_Getenabled/*界面控件组_是否有效*/(IUIGroupPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIGroup_Setenabled/*界面控件组_设置有效*/(IUIGroupPtr* _self/*接口*/, BOOL bEnable/*是否有效*/);
/*API*/LPCSTR	WINAPI	UIGroup_Getname/*界面控件组_取名字*/(IUIGroupPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIGroup_Setname/*界面控件组_设置名字*/(IUIGroupPtr* _self/*接口*/, LPCSTR szName/*组名字*/);
/*API*/VOID		WINAPI	UIGroup_clone/*界面控件组_克隆*/(IUIGroupPtr* _self/*接口*/, IUIGroupPtr* _return/*ret*/);
/*API*/VOID		WINAPI	UIGroup_cloneto/*界面控件组_克隆到其他对话框*/(IUIGroupPtr* _self/*接口*/, IDialogPtr* dest/*目标对话框*/, IUIGroupPtr* _return/*ret*/);
/*API*/VOID		WINAPI	UIGroup_Getcenter/*界面控件组_取得中心位置*/(IUIGroupPtr* _self/*接口*/, Pixel* _return/*ret*/);
/*API*/VOID		WINAPI	UIGroup_Setcenter/*界面控件组_设置中心位置*/(IUIGroupPtr* _self/*接口*/, const Pixel* pos/*位置*/);
/*API*/INT		WINAPI	UIGroup_count/*界面控件组_取得控件数量*/(IUIGroupPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIGroup_at/*界面控件组_取得控件*/(IUIGroupPtr* _self/*接口*/, INT nIndex/*索引*/, IControlPtr* _return/*ret*/);
/*API*/VOID		WINAPI	UIGroup_search/*界面控件组_查找组内控件指针*/(IUIGroupPtr* _self/*接口*/, LPCSTR szName/*名称*/, IControlPtr* _return/*ret*/);

/*API*/INT		WINAPI	UIGroup_Getleft/*界面控件组_取得左*/(IUIGroupPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIGroup_Setleft/*界面控件组_设置左*/(IUIGroupPtr* _self/*接口*/, INT nLeft/*左*/);
/*API*/INT		WINAPI	UIGroup_Gettop/*界面控件组_取得上*/(IUIGroupPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIGroup_Settop/*界面控件组_设置上*/(IUIGroupPtr* _self/*接口*/, INT nTop/*上*/);
/*API*/INT		WINAPI	UIGroup_width/*界面控件组_取得宽度*/(IUIGroupPtr* _self/*接口*/);
/*API*/INT		WINAPI	UIGroup_height/*界面控件组_取得高度*/(IUIGroupPtr* _self/*接口*/);

/*API*/float	WINAPI	UIGroup_Getopacity/*界面控件组_取得透明值*/(IUIGroupPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIGroup_Setopacity/*界面控件组_设置透明值*/(IUIGroupPtr* _self/*接口*/, float fOpacity/*透明值*/);

/*API*/float	WINAPI	UIGroup_Getspeed/*界面控件组_取得播放速度*/(IUIGroupPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIGroup_Setspeed/*界面控件组_设置播放速度*/(IUIGroupPtr* _self/*接口*/, float fSpeed/*速度*/);
/*API*/float	WINAPI	UIGroup_Getcurrenttime/*界面控件组_取得播放时间*/(IUIGroupPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIGroup_Setcurrenttime/*界面控件组_设置播放时间*/(IUIGroupPtr* _self/*接口*/, float fTime/*位置*/);
//
/*API*/float	WINAPI	UIGroup_Getrot/*界面控件组_取得角度*/(IUIGroupPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIGroup_Setrot/*界面控件组_设置角度*/(IUIGroupPtr* _self/*接口*/, float fDegress/*角度*/);
/*API*/VOID		WINAPI	UIGroup_setRotDir/*界面控件组_设置角度根据方向*/(IUIGroupPtr* _self/*接口*/, const float2* dir/*角度*/);
/*API*/ActionType	WINAPI	UIGroup_Getaction/*界面控件组_取播放动作*/(IUIGroupPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIGroup_Setaction/*界面控件组_设置播放动作*/(IUIGroupPtr* _self/*接口*/, ActionType nType/*动作*/);
/*API*/LPCSTR	WINAPI	UIGroup_Getactionby/*界面控件组_取播放动作根据名字*/(IUIGroupPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIGroup_Setactionby/*界面控件组_设置播放动作根据名字*/(IUIGroupPtr* _self/*接口*/, LPCSTR szName/*名字*/);

/*API*/VOID		WINAPI	UIGroup_setvalt/*界面控件组_设置变换有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szValue/*值*/);//设置文本值
/*API*/VOID		WINAPI	UIGroup_setvali/*界面控件组_设置变换有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, int nValue/*值*/);//设置整数值，如果是进度条则设置0或1
/*API*/VOID		WINAPI	UIGroup_setvalf/*界面控件组_设置变换有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, float fValue/*值*/);//设置浮点值,对不同的控件有不同的效果，比如进度条控件是0到1之间的值，拖动条也一样
/*API*/LPCSTR	WINAPI	UIGroup_getvalt/*界面控件组_设置变换有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/);//返回文本值
/*API*/INT		WINAPI	UIGroup_getvali/*界面控件组_设置变换有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/);//返回整数值，如果是选项框则返回0，1
/*API*/float	WINAPI	UIGroup_getvalf/*界面控件组_设置变换有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/);//返回浮点值，如果是进度条或者拖动条返回0到1之间
/*API*/VOID		WINAPI	UIGroup_Setpos/*界面控件组_设置对话框位置*/(IUIGroupPtr* _self/*接口*/, const Pixel* _val/*矩形*/);
/*API*/VOID		WINAPI	UIGroup_Getpos/*界面控件组_取位置左*/(IUIGroupPtr* _self/*接口*/, Pixel* _return/*矩形*/);
/*API*/VOID		WINAPI	UIGroup_bind_draw/*界面控件组_绑定渲染队列*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, IUIDrawMgrPtr* drawPtr/*组合*/);//绑定渲染队列到某个控件
/*API*/VOID		WINAPI	UIGroup_bind_image/*界面控件组_绑定图片*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szImageFile/*图片文件路径*/);//只绑定到第一个图片，其他图片自己调用element元素类的相关设置
/*API*/VOID		WINAPI	UIGroup_image_mode/*界面控件组_绑定图片模式*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szMode/*图片模式*/, DrawTextAlign align/*对齐*/);//只绑定到第一个图片，其他图片自己调用element元素类的相关设置
/*API*/VOID		WINAPI	UIGroup_bind_eventby/*界面控件组_绑定脚本到按下事件*/(IUIGroupPtr* _self/*接口*/, LPCSTR szEvent/*事件名*/, LPCSTR szControl/*控件名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//绑定一个事件,事件查看幻影脚本帮助中的eventid枚举常数说明事件[szLuaFunc(obj,eid,pos)--(obj=UIControl,eid=int,pos=Pixel)控件事件,eid=EventID]
/*API*/VOID		WINAPI	UIGroup_bind_event/*界面控件组_绑定脚本到按下事件*/(IUIGroupPtr* _self/*接口*/, EventID nid/*事件ID*/, LPCSTR szControl/*控件名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//绑定一个事件,事件查看幻影脚本帮助中的eventid枚举常数说明,事件[szLuaFunc(obj,eid,pos)--(obj=UIControl,eid=int,pos=Pixel)控件事件,eid=EventID]
/*API*/VOID		WINAPI	UIGroup_bind_click/*界面控件组_绑定脚本到事件*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//,事件[szLuaFunc(obj,eid,pos)--(obj=UIControl,eid=int,pos=Pixel)控件事件,eid=EventID]
/*API*/VOID		WINAPI	UIGroup_show/*界面控件组_显示控件*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, BOOL bShow/*是否显示*/);
/*API*/VOID		WINAPI	UIGroup_enable/*界面控件组_让控件有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, BOOL bEnable/*是否有效*/);
/*API*/VOID		WINAPI	UIGroup_playani/*界面控件组_播放控件动画*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szAniName/*动画名称*/);

/*API*/INT			WINAPI	UIGroup_timeout/*界面控件组_定时引发*/(IUIGroupPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/);//定时引发，只引发一次，毫秒记,脚本无参数,一个函数可以创建多个实例,事件[szLuaFunc(obj)--(obj=UIGroup)定期事件,只引发一次];返回TimeID
/*API*/INT			WINAPI	UIGroup_interval/*界面控件组_间隔引发*/(IUIGroupPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用次数*/);//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(obj,tm,endof,maxtm)--(obj=UIGroup,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
/*API*/INT			WINAPI	UIGroup_framemove/*界面控件组_循环引发*/(IUIGroupPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/);//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(obj,tm,endof,maxtm)--(obj=UIGroup,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
/*API*/INT			WINAPI	UIGroup_timeout2/*界面控件组_定时引发带一个文本参数*/(IUIGroupPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, LPCSTR param/*文本参数*/);//定时引发，只引发一次，毫秒记,代替以前的timeout_1,脚本第一个参数是传入的参数,文本,一个函数可以创建多个实例,事件[szLuaFunc(obj,param)--(obj=UIGroup)传入的自定义参数];返回TimeID
/*API*/INT			WINAPI	UIGroup_interval2/*界面控件组_间隔引发带一个文本参数*/(IUIGroupPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/);//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,代替以前的interval_1,参数为之有所改动,事件[szLuaFunc(obj,tm,endof,param,maxtm)--(obj=UIGroup,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID
/*API*/INT			WINAPI	UIGroup_framemove2/*界面控件组_循环引发*/(IUIGroupPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/);//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,第二个参数是传入的参数,文本,一个函数只能创建一个,事件[szLuaFunc(obj,tm,endof,param,maxtm)--(obj=UIGroup,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID


/*API*/VOID		WINAPI	UIDialog_Sethideon_killfocus/*界面对话框_设置失去焦点自动隐藏*/(IDialogPtr* _self/*接口*/, BOOL b/*设置*/);
/*API*/BOOL		WINAPI	UIDialog_Gethideon_killfocus/*界面对话框_是否失去焦点自动隐藏*/(IDialogPtr* _self/*接口*/);
/*API*/BOOL		WINAPI	UIDialog_destroy/*界面对话框_释放*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_deleteall/*界面对话框_删除所有控件*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_pick/*界面对话框_点选控件*/(IDialogPtr* _self/*接口*/, INT x/*x*/, INT y/*y*/, IControlPtr* _return/*ret*/);
/*API*/INT		WINAPI	UIDialog_count/*界面对话框_取控件数量*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_at/*界面对话框_取控件指针*/(IDialogPtr* _self/*接口*/, INT index/*索引*/, IControlPtr* _return/*ret*/);
/*API*/VOID		WINAPI	UIDialog_create/*界面对话框_创建控件*/(IDialogPtr* _self/*接口*/, UIControlType type/*界面类型*/, const Rect* rc/*创建区域*/, IControlPtr* _return/*ret*/);
/*API*/VOID		WINAPI	UIDialog_delete_at/*界面对话框_移除控件*/(IDialogPtr* _self/*接口*/, INT index/*索引*/);
/*API*/VOID		WINAPI	UIDialog_deleteby/*界面对话框_移除控件*/(IDialogPtr* _self/*接口*/, IControlPtr* control/*控件指针*/);
/*API*/VOID		WINAPI	UIDialog_search/*界面对话框_查找控件指针*/(IDialogPtr* _self/*接口*/, LPCSTR szName/*名称*/, IControlPtr* _return/*ret*/);
/*API*/VOID		WINAPI	UIDialog_searchbyid/*界面对话框_查找控件指针*/(IDialogPtr* _self/*接口*/, INT nID/*名称*/, IControlPtr* _return/*ret*/);

/*API*/LPCSTR	WINAPI	UIDialog_Getname/*界面对话框_取名字*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setname/*界面对话框_设置名字*/(IDialogPtr* _self/*接口*/, LPCSTR szName/*名字*/);
/*API*/ActionType	WINAPI	UIDialog_Getaction/*界面对话框_取播放动作*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setaction/*界面对话框_设置播放动作*/(IDialogPtr* _self/*接口*/, ActionType nType/*动作*/);
/*API*/LPCSTR	WINAPI	UIDialog_Getactionby/*界面对话框_取播放动作按照动作名*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setactionby/*界面对话框_设置播放动作按照动作名*/(IDialogPtr* _self/*接口*/, LPCSTR szName/*动作名*/);
/*API*/VOID		WINAPI	UIDialog_gcreate/*界面对话框_创建新组合*/(IDialogPtr* _self/*接口*/, IUIGroupPtr* _return/*组合*/);
/*API*/VOID		WINAPI	UIDialog_gdelete/*界面对话框_删除新组合*/(IDialogPtr* _self/*接口*/, IUIGroupPtr* group/*组合*/);
/*API*/INT		WINAPI	UIDialog_gcount/*界面对话框_取得控件组数量*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_gat/*界面对话框_取得控件组*/(IDialogPtr* _self/*接口*/, INT nIndex/*索引*/, IUIGroupPtr* _return/*组合*/);
/*API*/VOID		WINAPI	UIDialog_gsearch/*界面对话框_取得控件组根据名字*/(IDialogPtr* _self/*接口*/, LPCSTR szName/*名字*/, IUIGroupPtr* _return/*组合*/);
/*API*/VOID		WINAPI	UIDialog_gsearchbyid/*界面对话框_取得控件组根据名字*/(IDialogPtr* _self/*接口*/, INT nID/*名字*/, IUIGroupPtr* _return/*组合*/);
/*API*/BOOL		WINAPI	UIDialog_istooltip_window/*界面对话框_是否是提示对话框*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_settooltip_window/*界面对话框_设置是提示对话框*/(IDialogPtr* _self/*接口*/, BOOL b/*设置*/);
/*API*/BOOL		WINAPI	UIDialog_Getdisplay/*界面对话框_是否显示*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setdisplay/*界面对话框_显示*/(IDialogPtr* _self/*接口*/, BOOL b/*设置*/);
/*API*/VOID		WINAPI	UIDialog_Setleft/*界面对话框_设置位置左*/(IDialogPtr* _self/*接口*/, int _val/*值*/);
/*API*/int		WINAPI	UIDialog_Getleft/*界面对话框_取位置左*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Settop/*界面对话框_设置位置上*/(IDialogPtr* _self/*接口*/, int _val/*值*/);
/*API*/int		WINAPI	UIDialog_Gettop/*界面对话框_取位置上*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setwidth/*界面对话框_设置位置宽*/(IDialogPtr* _self/*接口*/, int _val/*值*/);
/*API*/int		WINAPI	UIDialog_Getwidth/*界面对话框_取位置宽*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setheight/*界面对话框_设置位置高*/(IDialogPtr* _self/*接口*/, int _val/*值*/);
/*API*/int		WINAPI	UIDialog_Getheight/*界面对话框_取位置高*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Getcolor/*界面对话框_取对话框颜色透明值*/(IDialogPtr* _self/*接口*/, color4* _return/*返回*/);
/*API*/VOID		WINAPI	UIDialog_Setcolor/*界面对话框_设置对话框颜色透明值*/(IDialogPtr* _self/*接口*/, const color4* val/*值*/);
/*API*/float	WINAPI	UIDialog_Getopacity/*界面对话框_取对话框透明值*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setopacity/*界面对话框_设置对话框透明值*/(IDialogPtr* _self/*接口*/, float val/*值*/);
/*API*/VOID		WINAPI	UIDialog_recalc_rect/*界面对话框_重新计算矩形区域*/(IDialogPtr* _self/*接口*/, BOOL bIncludeHide/*是否包含隐藏控件*/);//重新计算对话框位置和大小等，计算之后对应滚动条自动调整

/*API*/float	WINAPI	UIDialog_Getscale/*界面对话框_缩放*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setscale/*界面对话框_缩放*/(IDialogPtr* _self/*接口*/, float fScale/*缩放*/);//1=正常速度
/*API*/float	WINAPI	UIDialog_Getscalex/*界面对话框_缩放x*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setscalex/*界面对话框_缩放x*/(IDialogPtr* _self/*接口*/, float fScale/*缩放*/);//1=正常速度
/*API*/float	WINAPI	UIDialog_Getscaley/*界面对话框_缩放y*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setscaley/*界面对话框_缩放y*/(IDialogPtr* _self/*接口*/, float fScale/*缩放*/);//1=正常速度

/*API*/VOID		WINAPI	UIDialog_Getcenter/*界面对话框_取中心点位置*/(IDialogPtr* _self/*接口*/, Pixel* _return/*矩形*/);
/*API*/VOID		WINAPI	UIDialog_Setcenter/*界面对话框_设置中心点位置*/(IDialogPtr* _self/*接口*/, const Pixel* _val/*矩形*/);
/*API*/float	WINAPI	UIDialog_Getrotx/*界面对话框_取转动X*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setrotx/*界面对话框_设置转动X*/(IDialogPtr* _self/*接口*/, float fDegress/*角度*/);
/*API*/float	WINAPI	UIDialog_Getroty/*界面对话框_取转动Y*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setroty/*界面对话框_设置转动Y*/(IDialogPtr* _self/*接口*/, float fDegress/*角度*/);
/*API*/float	WINAPI	UIDialog_Getrotz/*界面对话框_取转动Z*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setrotz/*界面对话框_设置转动Z*/(IDialogPtr* _self/*接口*/, float fDegress/*角度*/);
/*API*/BOOL		WINAPI	UIDialog_Getenabled/*界面对话框_取有效*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setenabled/*界面对话框_设置有效*/(IDialogPtr* _self/*接口*/, BOOL bEnable/*是否有效*/);//默认是有效的

/*API*/float	WINAPI	UIDialog_Getrot2d/*界面对话框_取转动2D*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setrot2d/*界面对话框_设置转动2D*/(IDialogPtr* _self/*接口*/, float fDegress/*角度*/);
/*API*/VOID		WINAPI	UIDialog_rotdir/*界面对话框_转动2DDir*/(IDialogPtr* _self/*接口*/, Pixel* dir);

/*API*/BOOL		WINAPI	UIDialog_Getpause/*界面对话框_取有效*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setpause/*界面对话框_设置有效*/(IDialogPtr* _self/*接口*/, BOOL bPaused/*是否有效*/);//默认是没有静止的

/*API*/VOID		WINAPI	UIDialog_setvalt/*界面对话框_设置变换有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szValue/*值*/);//设置文本值
/*API*/VOID		WINAPI	UIDialog_setvali/*界面对话框_设置变换有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, int nValue/*值*/);//设置整数值，对选项框是0，1值
/*API*/VOID		WINAPI	UIDialog_setvalf/*界面对话框_设置变换有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, float fValue/*值*/);//设置浮点值,对不同的控件有不同的效果，比如进度条控件是0到1之间的值，拖动条也一样
/*API*/LPCSTR	WINAPI	UIDialog_getvalt/*界面对话框_设置变换有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/);//默认是有效的
/*API*/INT		WINAPI	UIDialog_getvali/*界面对话框_设置变换有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/);//默认是有效的
/*API*/float	WINAPI	UIDialog_getvalf/*界面对话框_设置变换有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/);//默认是有效的

/*API*/VOID		WINAPI	UIDialog_Setpos/*界面对话框_设置对话框位置*/(IDialogPtr* _self/*接口*/, const Pixel* _val/*矩形*/);
/*API*/VOID		WINAPI	UIDialog_Getpos/*界面对话框_取位置左*/(IDialogPtr* _self/*接口*/, Pixel* _return/*矩形*/);
/*API*/VOID		WINAPI	UIDialog_Setrect/*界面对话框_设置对话框矩形区域*/(IDialogPtr* _self/*接口*/, const Rect* _val/*矩形*/);
/*API*/VOID		WINAPI	UIDialog_Getrect/*界面对话框_取位置矩形区域*/(IDialogPtr* _self/*接口*/, Rect* _return/*矩形*/);

/*API*/VOID		WINAPI	UIDialog_bind_draw/*界面对话框_绑定渲染队列*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, IUIDrawMgrPtr* drawPtr/*组合*/);//绑定渲染队列到某个控件
/*API*/VOID		WINAPI	UIDialog_bind_image/*界面对话框_绑定图片*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szImageFile/*图片文件路径*/);//只绑定到第一个图片，其他图片自己调用element元素类的相关设置
/*API*/VOID		WINAPI	UIDialog_image_mode/*界面对话框_绑定图片模式*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szMode/*图片模式*/, DrawTextAlign align/*对齐*/);//只绑定到第一个图片，其他图片自己调用element元素类的相关设置

/*API*/VOID		WINAPI	UIDialog_bind_eventby/*界面对话框_绑定脚本到按下事件*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szEvent/*事件名*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//绑定一个事件,事件查看幻影脚本帮助中的eventid枚举常数说明,事件[szLuaFunc(obj,eid,pos)--(obj=UIControl,eid=int,pos=Pixel)控件事件,eid=EventID]
/*API*/VOID		WINAPI	UIDialog_bind_event/*界面对话框_绑定脚本到按下事件*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, EventID nid/*事件id*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//绑定一个事件,事件查看幻影脚本帮助中的eventid枚举常数说明,事件[szLuaFunc(obj,eid,pos)--(obj=UIControl,eid=int,pos=Pixel)控件事件,eid=EventID]
/*API*/VOID		WINAPI	UIDialog_bind_click/*界面对话框_绑定脚本到事件*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//,事件[szLuaFunc(obj,eid,pos)--(obj=UIControl,eid=int,pos=Pixel)控件事件,eid=EventID]
/*API*/VOID		WINAPI	UIDialog_show/*界面对话框_显示控件*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, BOOL bShow/*是否显示*/);
/*API*/VOID		WINAPI	UIDialog_enable/*界面对话框_让控件有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, BOOL bEnable/*是否有效*/);
/*API*/VOID		WINAPI	UIDialog_playani/*界面对话框_播放控件动画*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szAniName/*动画名称*/);
/*API*/VOID		WINAPI	UIDialog_move/*界面对话框_移动控件*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, INT left/*左*/, INT top/*上*/);

/*API*/VOID		WINAPI	UIDialog_Settopmost/*界面对话框_设置成最顶层*/(IDialogPtr* _self/*接口*/, BOOL bTopMost/*是否在最顶层*/);
/*API*/BOOL		WINAPI	UIDialog_Gettopmost/*界面对话框_设置成最顶层*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setdomodal/*界面对话框_有模式显示*/(IDialogPtr* _self/*接口*/, BOOL bModel/*是否有模式*/);
/*API*/BOOL		WINAPI	UIDialog_Getdomodal/*界面对话框_有模式显示*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_Setlayer/*界面对话框_设置显示层级*/(IDialogPtr* _self/*接口*/, INT nLayer/*层级，从-30000到30000*/);
/*API*/INT		WINAPI	UIDialog_Getlayer/*界面对话框_设置显示层级*/(IDialogPtr* _self/*接口*/);
/*API*/VOID		WINAPI	UIDialog_hold/*界面对话框_对话框不被自动删除*/(IDialogPtr* _self/*接口*/);//设置对话框不会随着脚本自动删除
//
/*API*/INT			WINAPI	UIDialog_timeout/*界面对话框_定时引发*/(IDialogPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/);//定时引发，只引发一次，毫秒记,脚本无参数,一个函数可以创建多个实例,事件[szLuaFunc(obj)--(obj=UIDialog)定期事件,只引发一次];返回TimeID
/*API*/INT			WINAPI	UIDialog_interval/*界面对话框_间隔引发*/(IDialogPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用次数*/);//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(obj,tm,endof,maxtm)--(obj=UIDialog,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
/*API*/INT			WINAPI	UIDialog_framemove/*界面对话框_循环引发*/(IDialogPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/);//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(obj,tm,endof,maxtm)--(obj=UIDialog,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
/*API*/INT			WINAPI	UIDialog_timeout2/*界面对话框_定时引发带一个文本参数*/(IDialogPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, LPCSTR param/*文本参数*/);//定时引发，只引发一次，毫秒记,代替以前的timeout_1,脚本第一个参数是传入的参数,文本,一个函数可以创建多个实例,事件[szLuaFunc(obj,param)--(obj=UIDialog)传入的自定义参数];返回TimeID
/*API*/INT			WINAPI	UIDialog_interval2/*界面对话框_间隔引发带一个文本参数*/(IDialogPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/);//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,代替以前的interval_1,参数为之有所改动,事件[szLuaFunc(obj,tm,endof,param,maxtm)--(obj=UIDialog,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID
/*API*/INT			WINAPI	UIDialog_framemove2/*界面对话框_循环引发*/(IDialogPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/);//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,第二个参数是传入的参数,文本,一个函数只能创建一个,事件[szLuaFunc(obj,tm,endof,param,maxtm)--(obj=UIDialog,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID

/*API*/BOOL		WINAPI	UIDialog_map_searchway/*界面对话框_执行寻路*/(IDialogPtr* _self/*接口*/, IFindWayPtr* fw/*寻路接口*/, const Pixel* from/*开始*/, const Pixel* to/*目标*/);
/*API*/BOOL		WINAPI	UIDialog_map_searchway2/*界面对话框_执行寻路2*/(IDialogPtr* _self/*接口*/, IFindWayPtr* fw/*寻路接口*/, int fareax, int fareay, int tareax, int tareay);
/*API*/VOID		WINAPI	UIDialog_map_gridsize/*界面对话框_取地图格子大小*/(IDialogPtr* _self/*接口*/, Pixel* _return/*矩形*/);
/*API*/VOID		WINAPI	UIDialog_map_count/*界面对话框_取地图方块数量*/(IDialogPtr* _self/*接口*/, Pixel* _return/*矩形*/);
/*API*/VOID		WINAPI	UIDialog_map_center/*界面对话框_取某个地图坐标方块的中心位置*/(IDialogPtr* _self/*接口*/, Pixel* pos/*矩形*/, Pixel* _return/*矩形*/);
/*API*/VOID		WINAPI	UIDialog_map_areaindex/*界面对话框_取某个索引的寻路坐标*/(IDialogPtr* _self/*接口*/, int mapx, int mapy, Pixel* _return/*矩形*/);
/*API*/VOID		WINAPI	UIDialog_map_mapindex/*界面对话框_取某个寻路索引的地图坐标*/(IDialogPtr* _self/*接口*/, int areax, int areay, Pixel* _return/*矩形*/);
/*API*/VOID		WINAPI	UIDialog_map_pickmap/*界面对话框_点击一个位置获得地图坐标*/(IDialogPtr* _self/*接口*/, Pixel* pos, Pixel* _return/*矩形*/);//返回mapx,mapy
/*API*/VOID		WINAPI	UIDialog_map_pickarea/*界面对话框_点击一个位置获得寻路坐标*/(IDialogPtr* _self/*接口*/, Pixel* pos, Pixel* _return/*矩形*/);//返回mapx,mapy
/*API*/VOID		WINAPI	UIDialog_map_add2dchild/*界面对话框_添加一个子对话框*/(IDialogPtr* _self/*接口*/, IDialogPtr* child/*子对话框*/);//子对话框中存在[组中心对象]的控件坐标作为中心点进行排序，作为2D对象排序
/*API*/VOID		WINAPI	UIDialog_map_clear2dchild/*界面对话框_清除所有的子对象*/(IDialogPtr* _self/*接口*/);

/*API*/BOOL		WINAPI	UIDialog_map_moveable/*界面对话框_是否可以移动到此格子内*/(IDialogPtr* _self/*接口*/, int areax, int areay);
/*API*/VOID		WINAPI	UIDialog_map_set_moveable/*界面对话框_设置可以移动到此格子内*/(IDialogPtr* _self/*接口*/, int areax, int areay, BOOL bMoveable);
/*API*/VOID		WINAPI	UIDialog_map_clear_moveable/*界面对话框_清除所有新增加的可以动格子数据*/(IDialogPtr* _self/*接口*/);

/*API*/VOID		WINAPI	UIDialog_map_beginmoveto/*界面对话框_添加新移动点*/(IDialogPtr* _self/*接口*/, float fSpeed, LPCSTR szLuaFunc/*脚本函数名*/);//按照寻路数据自己移动,事件[szLuaFunc(obj,endof)--(obj=UIDialog,endof=bool)移动时被调用，最后endof返回true表示移动完毕]
/*API*/VOID		WINAPI	UIDialog_map_addmoveto/*界面对话框_添加新移动点*/(IDialogPtr* _self/*接口*/, Pixel* fw);//
/*API*/VOID		WINAPI	UIDialog_map_addfindway/*界面对话框_添加新寻路数据*/(IDialogPtr* _self/*接口*/, IFindWayPtr* fw/*寻路接口*/);//
/*API*/VOID		WINAPI	UIDialog_map_stopmove/*界面对话框_结束所有移动*/(IDialogPtr* _self/*接口*/);

/*API*/VOID		WINAPI	ValueControl_addf/*值控制_下一个浮点数*/(IValueControlPtr* _self/*接口*/, float fEnd/*结束*/, float timeLength/*时钟*/, int loopCount/*循环次数*/);//返回对象数量
/*API*/VOID		WINAPI	ValueControl_addfloat3/*值控制_下一个浮点*/(IValueControlPtr* _self/*接口*/, const float3* fEnd/*结束*/, float timeLength/*时钟*/, int loopCount/*循环次数*/);//返回对象数量
/*API*/VOID		WINAPI	ValueControl_addcolor/*值控制_下一个浮点*/(IValueControlPtr* _self/*接口*/, const color4* fEnd/*结束*/, float timeLength/*时钟*/, int loopCount/*循环次数*/);//返回对象数量
/*API*/VOID		WINAPI	ValueControl_reset/*值控制_重新从第一个点开始*/(IValueControlPtr* _self/*接口*/);
/*API*/VOID		WINAPI	ValueControl_clear/*值控制_删除所有点*/(IValueControlPtr* _self/*接口*/);
//
/*API*/BOOL		WINAPI	ValueControl_Getenabled/*值控制_是否有效*/(IValueControlPtr* _self/*接口*/);//
/*API*/VOID		WINAPI	ValueControl_Setenabled/*值控制_设置有效*/(IValueControlPtr* _self/*接口*/, BOOL bEnabled/*有效*/);//

//
/*API*/BOOL		WINAPI	Camera_get2d_from3d/*镜头_三维坐标转屏幕坐标*/(Pixel* point/*取屏幕坐标*/, const float3* pos/*三维坐标*/);
/*API*/VOID		WINAPI	Camera_get3d_from2d/*镜头_屏幕坐标转3D坐标*/(float3* orig/*射线原点*/, float3* dir/*射线方向*/, const Pixel* point/*屏幕坐标*/);
/*API*/BOOL		WINAPI	Camera_isdisplay/*镜头_判断是否可见*/(const float3* pos/*位置*/, float fRadius/*物体半径*/, float fMaxDistance/*最远距离*/);
/*API*/VOID		WINAPI	Camera_orthoproj/*镜头_设置为2D投影视图*/(int w/*宽度*/,int h/*高度*/,float znear/*最近可见*/,float zfar/*最远可见*/);
/*API*/VOID		WINAPI	Camera_perspectiveproj/*镜头_设置为3D投影视图*/(float fov/*广角_弧度*/, int w/*宽度*/,int h/*高度*/,float znear/*最近可见*/,float zfar/*最远可见*/);
/*API*/BOOL		WINAPI	Camera_isperspective/*镜头_是否3D投影视图*/();
/*API*/BOOL		WINAPI	Camera_isortho/*镜头_是否2D投影视图*/();
/*API*/VOID		WINAPI	Camera_Setviewmat/*镜头_强制设置观察矩阵*/(const matrix4x4* view/*观察矩阵*/);
/*API*/VOID		WINAPI	Camera_Setprojmat/*镜头_强制设置投影矩阵*/(const matrix4x4* view/*投影矩阵*/);
/*API*/VOID		WINAPI	Camera_Seteye/*镜头_设置观察点*/(const float3* eye/*参数1*/);
/*API*/VOID		WINAPI	Camera_Geteye/*镜头_取观察点*/(float3* _return/*ret*/);
/*API*/VOID		WINAPI	Camera_Settarget/*镜头_设置目标点*/(const float3* target/*参数1*/);
/*API*/VOID		WINAPI	Camera_Gettarget/*镜头_取目标点*/(float3* _return/*ret*/);
/*API*/VOID		WINAPI	Camera_Setlookat/*镜头_设置视点*/(const float3* vLookat/*参数1*/);
/*API*/VOID		WINAPI	Camera_Getlookat/*镜头_取视点*/(float3* _return/*ret*/);
/*API*/VOID		WINAPI	Camera_Setintersect_eye/*镜头_设置镜头碰撞*/(BOOL bEnabled/*参数1*/);//镜头碰撞就是视点与模型的碰撞，并取得最佳角度
/*API*/BOOL		WINAPI	Camera_Getintersect_eye/*镜头_取镜头碰撞*/();
/*API*/float	WINAPI	Camera_Getfov/*镜头_取广角*/();
/*API*/INT		WINAPI	Camera_Getwidth/*镜头_取宽度*/();
/*API*/INT		WINAPI	Camera_Getheight/*镜头_取高度*/();
/*API*/float	WINAPI	Camera_Getznearest/*镜头_最近可见距离*/();
/*API*/float	WINAPI	Camera_Getzfar/*镜头_取最远可见距离*/();
/*API*/VOID		WINAPI	Camera_Setfov/*镜头_设置广角*/(float fValue/*值*/);
/*API*/VOID		WINAPI	Camera_Setznearest/*镜头_设置最近可见距离*/(float fValue/*值*/);
/*API*/VOID		WINAPI	Camera_Setzfar/*镜头_设置取最远可见距离*/(float fValue/*值*/);
/*API*/VOID		WINAPI	Camera_Setwidth/*镜头_设置宽度*/(INT fValue/*值*/);
/*API*/VOID		WINAPI	Camera_Setheight/*镜头_设置高度*/(INT fValue/*值*/);
/*API*/VOID		WINAPI	Camera_Getviewmat/*镜头_取观察矩阵*/(matrix4x4* _return/*ret*/);
/*API*/VOID		WINAPI	Camera_Getprojmat/*镜头_取投影矩阵*/(matrix4x4* _return/*ret*/);
/*API*/BOOL		WINAPI	Camera_pointin/*镜头_检测点是否可见*/(const float3* p/*点*/);
/*API*/BOOL		WINAPI	Camera_spherein/*镜头_检测圆是否可见*/(const float3* p/*点*/, float fRadius/*半径*/);
/*API*/BOOL		WINAPI	Camera_aaboxin/*镜头_检测盒子是否可见*/(const AABox* p/*点*/);
/*API*/BOOL		WINAPI	Camera_planein/*镜头_检测面是否可见*/(const float3* p1/*点1*/, const float3* p2/*点2*/, const float3* p3/*点3*/, const float3* p4/*点4*/);
/*API*/VOID		WINAPI	Camera_Getdata/*镜头_取数据*/(CameraData* _return/*ret*/);
/*API*/VOID		WINAPI	Camera_Setdata/*镜头_设置数据*/(const CameraData* p/*数据*/);
/*API*/VOID		WINAPI	Camera_dither/*镜头_设置进行抖动值*/(const float3* offset/*抖动值*/, float fMaxOffsetTime/*抖动的时间*/);
/*API*/VOID		WINAPI	Camera_Setarea/*镜头_设置目标点限制区域*/(const Rect* rc/*参数1*/);//设置镜头的限制区域，移动时不能超过这个区域移动，只有在移动的时候有效,如果全部输入值是0的话会自动设置成无效
/*API*/VOID		WINAPI	Camera_Getarea/*镜头_取观目标点限制区域*/(Rect* _return/*ret*/);//设置镜头的限制区域，移动时不能超过这个区域移动
//
/*API*/VOID		WINAPI	Camera_Setoffset_enable/*镜头_设置自动移动速度有效*/(BOOL bEnable/*参数1*/);//当移动有效之后，点击释放按钮时自动移动场景对象的速度降低速度,0到10之间的数字
/*API*/BOOL		WINAPI	Camera_Getoffset_enable/*镜头_取观自动移动速度有效*/();//当移动有效之后，点击释放按钮时自动移动场景对象的速度降低速度,0到10之间的数字
/*API*/VOID		WINAPI	Camera_Setoffset_down/*镜头_设置自动移动速度降速*/(float fSpeed/*参数1*/);//当移动有效之后，点击释放按钮时自动移动场景对象的速度降低速度,0到10之间的数字
/*API*/float	WINAPI	Camera_Getoffset_down/*镜头_取观自动移动速度降速*/();//当移动有效之后，点击释放按钮时自动移动场景对象的速度降低速度,0到10之间的数字
/*API*/VOID		WINAPI	Camera_Setdistance/*镜头_设置视距离*/(float fLength/*参数1*/);
/*API*/float	WINAPI	Camera_Getdistance/*镜头_取视距离*/();
/*API*/VOID		WINAPI	Camera_Setdistance_min/*镜头_设置视距离最近*/(float fLength/*参数1*/);
/*API*/float	WINAPI	Camera_Getdistance_min/*镜头_取视距离最近*/();
/*API*/VOID		WINAPI	Camera_Setdistance_max/*镜头_设置视距离最远*/(float fLength/*参数1*/);
/*API*/float	WINAPI	Camera_Getdistance_max/*镜头_取视距离最远*/();
/*API*/VOID		WINAPI	Camera_Setenabled/*镜头_设置屏幕控制有效性*/(BOOL bEnabled/*参数1*/);
/*API*/BOOL		WINAPI	Camera_Getenabled/*镜头_取视屏幕控制有效性*/();

/*API*/VOID		WINAPI	Camera_Setrot/*镜头_设置转动率*/(const float2* rot);
/*API*/VOID		WINAPI	Camera_Getrot/*镜头_取得转动率*/(float2* _return);
/*API*/VOID		WINAPI	Camera_Setpos/*镜头_设置位置*/(const float3* pos/*位置*/);//设置镜头的位置
/*API*/VOID		WINAPI	Camera_Getpos/*镜头_取观位置*/(float3* _return/*ret*/);//设置镜头的位置
/*API*/VOID		WINAPI	Camera_nextto/*镜头_加入下一个位置*/(float3* pos/*位置*/, float fMoveSpeed/*移动速度*/);//设置镜头的位置移动
/*API*/VOID		WINAPI	Camera_nextrot/*镜头_加入下一个转动*/(float rotZ/*Z轴转动*/, float rotX/*X轴转动*/, float fRotSpeed/*转动速度角度*/);//设置镜头的转动
/*API*/VOID		WINAPI	Camera_nextdist/*镜头_加入下一个远近*/(float fDistance/*目标距离*/, float fMoveSpeed/*远近移动速度*/);//设置镜头的远近
/*API*/VOID		WINAPI	Camera_nextdither/*镜头_加入下一个剧烈抖动*/(float offset/*抖动范围*/, float fMaxOffsetTime/*抖动的时间*/);//设置镜头的抖动
/*API*/VOID		WINAPI	Camera_play/*镜头_开始播放镜头动作*/(LPCSTR szLuaFunc/*脚本函数*/, INT nParam/*参数*/);//设置镜头的远近,事件[szLuaFunc(param)--(param=int)镜头动作播放完毕]
/*API*/VOID		WINAPI	Camera_stop/*镜头_停止播放镜头动作并且清除所有动作*/();//停止的时候不会自动调用结束脚本函数,并且清除所有动作

/*API*/VOID			WINAPI	Map3DEnv_Setname/*场景环境3D_设置名字*/(IMap3DEnvPtr* _self/*场景天空*/, LPCSTR szName/*名字*/);
/*API*/LPCSTR		WINAPI	Map3DEnv_Getname/*场景环境3D_取名字*/(IMap3DEnvPtr* _self/*场景天空*/);
/*API*/VOID			WINAPI	Map3DEnv_Setcolor/*场景环境3D_设置颜色*/(IMap3DEnvPtr* _self/*场景天空*/, const color4* c/*颜色*/);//雾颜色
/*API*/VOID			WINAPI	Map3DEnv_Getcolor/*场景环境3D_取颜色*/(IMap3DEnvPtr* _self/*场景天空*/, color4* _return/*ret*/);
/*API*/VOID			WINAPI	Map3DEnv_Setnear/*场景环境3D_设置最近*/(IMap3DEnvPtr* _self/*场景天空*/, float c/*最近*/);//雾最近距离
/*API*/float		WINAPI	Map3DEnv_Getnear/*场景环境3D_取最近*/(IMap3DEnvPtr* _self/*场景天空*/);
/*API*/VOID			WINAPI	Map3DEnv_Setfar/*场景环境3D_设置最远*/(IMap3DEnvPtr* _self/*场景天空*/, float c/*最远*/);//雾最远距离
/*API*/float		WINAPI	Map3DEnv_Getfar/*场景环境3D_取最远*/(IMap3DEnvPtr* _self/*场景天空*/);
/*API*/VOID			WINAPI	Map3DEnv_Setmode/*场景环境3D_设置雾模式*/(IMap3DEnvPtr* _self/*场景天空*/, FogMode mode/*模式*/);//0=3D雾，1=2D雾，2=2D雾+灰度
/*API*/FogMode		WINAPI	Map3DEnv_Getmode/*场景环境3D_取雾模式*/(IMap3DEnvPtr* _self/*场景天空*/);//0=3D雾，1=2D雾，2=2D雾+灰度
/*API*/BOOL			WINAPI	Map3DEnv_Getenabled_brightness/*场景环境3D_是否有效亮度*/(IMap3DEnvPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3DEnv_Setenabled_brightness/*场景环境3D_是否有效亮度*/(IMap3DEnvPtr* _self/*接口*/, BOOL bEnabled/*是否有效*/);
/*API*/BOOL			WINAPI	Map3DEnv_Getenabled_contrast/*场景环境3D_是否有效对比度*/(IMap3DEnvPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3DEnv_Setenabled_contrast/*场景环境3D_是否有效对比度*/(IMap3DEnvPtr* _self/*接口*/, BOOL bEnabled/*是否有效*/);
/*API*/float		WINAPI	Map3DEnv_Getbrightness/*场景环境3D_是否亮度*/(IMap3DEnvPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3DEnv_Setbrightness/*场景环境3D_是否亮度*/(IMap3DEnvPtr* _self/*接口*/, float fBrightness/*亮度*/);
/*API*/float		WINAPI	Map3DEnv_Getcontrast/*场景环境3D_是否对比度*/(IMap3DEnvPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3DEnv_Setcontrast/*场景环境3D_是否对比度*/(IMap3DEnvPtr* _self/*接口*/, float fContrast/*对比度*/);

/*API*/VOID			WINAPI	Map3DEnv_Setshadow_center_item/*场景环境3D_设置阴影中心点对象*/(IMap3DEnvPtr* _self/*接口*/, IMap3DItemPtr* obj);//设置之后，阴影会自动取得该场景对象的位置进行渲染
/*API*/VOID			WINAPI	Map3DEnv_Getshadow_center_item/*场景环境3D_取得阴影中心点对象*/(IMap3DEnvPtr* _self/*接口*/, IMap3DItemPtr* _return);
/*API*/VOID			WINAPI	Map3DEnv_Setshadow_center/*场景环境3D_设置阴影位置*/(IMap3DEnvPtr* _self/*接口*/, const float3* pos);//设置位置之后之后，阴影跟踪对象会自动被取消
/*API*/VOID			WINAPI	Map3DEnv_Getshadow_center/*场景环境3D_取得阴影位置*/(IMap3DEnvPtr* _self/*接口*/, float3* _return);
/*API*/VOID			WINAPI	Map3DEnv_Setshadow_size/*场景环境3D_设置阴影扩散范围*/(IMap3DEnvPtr* _self/*接口*/, float fSize/*影响范围*/);//设置阴影影响范围大小，单位米
/*API*/float		WINAPI	Map3DEnv_Getshadow_size/*场景环境3D_取得阴影扩散范围*/(IMap3DEnvPtr* _self/*接口*/);


/*API*/VOID				WINAPI	Map3DItem_Getworldmat/*地图对象3D_取世界矩阵*/(IMap3DItemPtr* _self/*场景对象*/, matrix4x4* _return/*ret*/);
/*API*/VOID				WINAPI	Map3DItem_Setworldmat/*地图对象3D_设置世界矩阵*/(IMap3DItemPtr* _self/*场景对象*/, const matrix4x4* m/*设置*/);
/*API*/VOID				WINAPI	Map3DItem_Getpos3d/*地图对象3D_取位置*/(IMap3DItemPtr* _self/*场景对象*/, float3* _return/*ret*/);
/*API*/VOID				WINAPI	Map3DItem_Setpos3d/*地图对象3D_设置位置*/(IMap3DItemPtr* _self/*场景对象*/, const float3* m/*设置*/);
/*API*/VOID				WINAPI	Map3DItem_Getpos/*地图对象3D_取位置*/(IMap3DItemPtr* _self/*场景对象*/, Pixel* _return/*ret*/);
/*API*/VOID				WINAPI	Map3DItem_Setpos/*地图对象3D_设置位置*/(IMap3DItemPtr* _self/*场景对象*/, const Pixel* m/*设置*/);
/*API*/INT				WINAPI	Map3DItem_id/*地图对象3D_取ID*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/LPCSTR			WINAPI	Map3DItem_Getname/*地图对象3D_取名字*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/VOID				WINAPI	Map3DItem_Setname/*地图对象3D_设置名字*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szName/*名字*/);
/*API*/BOOL				WINAPI	Map3DItem_Getdisplay/*地图对象3D_是否显示*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/VOID				WINAPI	Map3DItem_Setdisplay/*地图对象3D_显示*/(IMap3DItemPtr* _self/*场景对象*/, BOOL bVisible/*显示与否*/);
/*API*/BOOL				WINAPI	Map3DItem_Getenabled/*地图对象3D_是否有效*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/VOID				WINAPI	Map3DItem_Setenabled/*地图对象3D_有效*/(IMap3DItemPtr* _self/*场景对象*/, BOOL bVisible/*显示与否*/);
/*API*/VOID				WINAPI	Map3DItem_Setheight/*地图对象3D_设置飞行高度*/(IMap3DItemPtr* _self/*场景对象*/, float fFly/*飞行高度*/);//设置飞行高度，也可以作为模型有过于贴近时有乱花现象，向上稍微提升某个模型来避免
/*API*/float			WINAPI	Map3DItem_Getheight/*地图对象3D_取飞行高度*/(IMap3DItemPtr* _self/*场景对象*/);

/*API*/VOID				WINAPI	Map3DItem_lookat/*地图对象3D_设置面向目标*/(IMap3DItemPtr* _self/*场景对象*/, const Pixel* m/*设置*/);//设置面向目标的值

/*API*/VOID				WINAPI	Map3DItem_clone/*地图对象3D_克隆对象*/(IMap3DItemPtr* _self/*场景对象*/, IMap3DItemPtr* _return/*返回*/);//返回克隆一个新的对象，所有属性将会相同
/*API*/VOID				WINAPI	Map3DItem_deleteme/*地图对象3D_删除对象*/(IMap3DItemPtr* _self/*场景对象*/, IMap3DItemPtr* _return/*返回*/);//删除一个对象
/*API*/VOID				WINAPI	Map3DItem_mesh/*地图对象3D_返回模型*/(IMap3DItemPtr* _self/*场景对象*/, IMeshPtr* _return/*返回*/);//返回模型类
/*API*/float			WINAPI	Map3DItem_Getrot/*地图对象3D_取转动Z值角度*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/VOID				WINAPI	Map3DItem_Setrot/*地图对象3D_设置转动Z值角度*/(IMap3DItemPtr* _self/*场景对象*/, float fRotZ/*转动Z角度*/);
/*API*/VOID				WINAPI	Map3DItem_moveto/*地图对象3D_移动到指定位置*/(IMap3DItemPtr* _self/*场景对象*/, const Pixel* pos/*目标点*/, LPCSTR szFunc/*移动结束后事件*/);//会自动寻路计算,会打断movedir的移动,事件[szFunc(obj)--(obj=Map3DItem)移动完毕之后]
/*API*/VOID				WINAPI	Map3DItem_movedir/*地图对象3D_自动移动*/(IMap3DItemPtr* _self/*场景对象*/, const Pixel* dir/*移动到目标*/);//自动按照速度和方向移动,无止境的移动,会打断moveto的移动
/*API*/VOID				WINAPI	Map3DItem_stopmove/*地图对象3D_停止移动*/(IMap3DItemPtr* _self/*场景对象*/);//立即停止所有移动操作

/*API*/float			WINAPI	Map3DItem_Getscale/*地图对象3D_取缩放*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/VOID				WINAPI	Map3DItem_Setscale/*地图对象3D_设置缩放*/(IMap3DItemPtr* _self/*场景对象*/, float fScale/*缩放*/);

/*API*/VOID				WINAPI	Map3DItem_Setactionby/*地图对象3D_设置当前动作*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szName/*动作名称*/);//播放指定动作,通过bind_action来绑定动作回调函数，如果没有特别指令则无限循环播放最后一个动作
/*API*/VOID				WINAPI	Map3DItem_Setaction/*地图对象3D_设置当前动作根据动作类型*/(IMap3DItemPtr* _self/*场景对象*/, ActionType type/*动作类型*/);//播放指定动作,通过bind_action来绑定动作回调函数，如果没有特别指令则无限循环播放最后一个动作
/*API*/LPCSTR			WINAPI	Map3DItem_Getactionby/*地图对象3D_取得当前动作*/(IMap3DItemPtr* _self/*场景对象*/);//取得当前动作
/*API*/ActionType		WINAPI	Map3DItem_Getaction/*地图对象3D_取得当前动作根据动作类型*/(IMap3DItemPtr* _self/*场景对象*/);//取得当前动作
/*API*/VOID				WINAPI	Map3DItem_pushactionby/*地图对象3D_设置下一个动作*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szName/*动作名称*/);//下一个动作，可以叠加多个，会一个一个自动播放,每个动作播放完毕会自动调用之前设置好的actionby或action中设置的lua回调函数,如果传入是空值则动作将停止，不再进行任何差值操作直到调用actionby或action为止，如果没有特别指令则无限循环播放最后一个动作
/*API*/VOID				WINAPI	Map3DItem_pushaction/*地图对象3D_设置下一个动作根据动作类型*/(IMap3DItemPtr* _self/*场景对象*/, ActionType type/*动作类型*/);//下一个动作，可以叠加多个，会一个一个自动播放,每个动作播放完毕会自动调用之前设置好的actionby或action中设置的lua回调函数,如果传入是0值则动作将停止，不再进行任何差值操作直到调用actionby或action为止，如果没有特别指令则无限循环播放最后一个动作

/*API*/float			WINAPI	Map3DItem_Getaspeed/*地图对象3D_取动作播放速度*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/VOID				WINAPI	Map3DItem_Setaspeed/*地图对象3D_设置动作播放速度*/(IMap3DItemPtr* _self/*场景对象*/, float fSpeed/*动作播放速度*/);
/*API*/float			WINAPI	Map3DItem_Getspeed/*地图对象3D_取移动速度*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/VOID				WINAPI	Map3DItem_Setspeed/*地图对象3D_设置移动速度*/(IMap3DItemPtr* _self/*场景对象*/, float fSpeed/*移动速度*/);
/*API*/BOOL				WINAPI	Map3DItem_Getloop/*地图对象3D_是否循环播放*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/VOID				WINAPI	Map3DItem_Setloop/*地图对象3D_设置循环播放*/(IMap3DItemPtr* _self/*场景对象*/, INT nLoop/*循环与否*/);
/*API*/BOOL				WINAPI	Map3DItem_Getplay/*地图对象3D_是否播放*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/VOID				WINAPI	Map3DItem_Setplay/*地图对象3D_播放*/(IMap3DItemPtr* _self/*场景对象*/, BOOL b/*播放与否*/);
/*API*/BOOL				WINAPI	Map3DItem_Getpause/*地图对象3D_是否暂停*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/VOID				WINAPI	Map3DItem_Setpause/*地图对象3D_暂停*/(IMap3DItemPtr* _self/*场景对象*/, BOOL b/*暂停与否*/);
/*API*/float			WINAPI	Map3DItem_Getopacity/*地图对象3D__设置所有子模型透明值*/(IMap3DItemPtr* _self/*场景对象*/);//设置透明值
/*API*/VOID				WINAPI	Map3DItem_Setopacity/*地图对象3D__设置所有子模型透明值*/(IMap3DItemPtr* _self/*场景对象*/,float fAlpha/*透明值*/);//设置透明值
/*API*/BOOL				WINAPI	Map3DItem_bind_mesh/*地图对象3D_绑定模型*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szBoneName/*骨骼名*/, IMeshPtr* mesh/*模型*/);//绑定模型到一个骨骼上
/*API*/BOOL				WINAPI	Map3DItem_unbind_mesh/*地图对象3D_解除绑定模型*/(IMap3DItemPtr* _self/*场景对象*/, IMeshPtr* mesh/*模型*/);//解除绑定模型
/*API*/VOID				WINAPI	Map3DItem_bind_action_begin/*地图对象3D_设置动作开始的脚本回调*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szLuaBegin/*动作开始脚本函数*/);//播放指定动作,每次循环完毕之后都会被调用调用szLuaFunc脚本函数, 脚本参数有三个,第一个是map3ditem,第二个是动作名称,第三个是actiontype,事件[szLuaBegin(obj,type,name)--(obj=Map3DSkill,type=actiontype,name=string)这个调用会清除所有的已经编辑好的actionby,nextaction,nextaction调用]
/*API*/VOID				WINAPI	Map3DItem_bind_action_end/*地图对象3D_设置动作结束的脚本回调*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szLuaEnd/*动作结束脚本函数*/);//播放指定动作,每次循环完毕之后都会被调用调用szLuaFunc脚本函数, 脚本参数有三个,第一个是map3ditem,第二个是动作名称,第三个是actiontype,事件[szLuaEnd(obj,type,name)--(obj=Map3DSkill,type=actiontype,name=string)这个调用会清除所有的已经编辑好的actionby,nextaction,nextaction调用]
/*API*/VOID				WINAPI	Map3DItem_bind_eventby/*地图对象3D_绑定事件*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szEvent/*事件名*/, LPCSTR szLuaFunc/*函数*/);//绑定一个事件,事件查看幻影脚本帮助中的eventid枚举常数说明,事件[szLuaFunc(obj)--(obj=Map3DItem)]
/*API*/VOID				WINAPI	Map3DItem_bind_event/*地图对象3D_绑定事件*/(IMap3DItemPtr* _self/*场景对象*/, EventID nEvent/*事件ID*/, LPCSTR szLuaFunc/*函数*/);//绑定一个事件,事件查看幻影脚本帮助中的eventid枚举常数说明,事件[szLuaFunc(obj)--(obj=Map3DItem)]
/*API*/VOID				WINAPI	Map3DItem_bind_draw/*地图对象3D_绑定渲染队列*/(IMap3DItemPtr* _self/*场景对象*/, IUIDrawMgrPtr* drawPtr/*组合*/, LPCSTR szBindBone/*绑定骨骼*/);//绑定渲染队列到某个对象
/*API*/VOID				WINAPI	Map3DItem_replace_image/*地图对象3D_绑定图片*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szImageSrc/*原图片文件路径*/, LPCSTR szImageReplace/*替换图片文件路径*/);//替换图片
/*API*/VOID				WINAPI	Map3DItem_bind_click/*地图对象3D_绑定脚本到事件*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//绑定点击事件,事件[szLuaFunc(obj)--(obj=Map3DItem)]
/*API*/VOID				WINAPI	Map3DItem_Setclassid/*地图对象3D_设置类聚*/(IMap3DItemPtr* _self/*场景对象*/, INT nClass/*类聚*/);//设置类聚,classID是0到100之间的任意数字
/*API*/INT				WINAPI	Map3DItem_Getclassid/*地图对象3D_设置类聚*/(IMap3DItemPtr* _self/*场景对象*/);//返回类聚,classID是0到100之间的任意数字
/*API*/VOID				WINAPI	Map3DItem_Setblood_display/*地图对象3D_显示血条*/(IMap3DItemPtr* _self/*场景对象*/, BOOL bShow/*血条*/);//设置是否显示血条
/*API*/BOOL				WINAPI	Map3DItem_Getblood_display/*地图对象3D_显示血条*/(IMap3DItemPtr* _self/*场景对象*/);//设置是否显示血条
/*API*/VOID				WINAPI	Map3DItem_Setblood/*地图对象3D_设置血条值*/(IMap3DItemPtr* _self/*场景对象*/, float fValue/*血条*/);//设置血条值，0到1
/*API*/float			WINAPI	Map3DItem_Getblood/*地图对象3D_返回血条值*/(IMap3DItemPtr* _self/*场景对象*/);//设置血条值，0到1
/*API*/VOID				WINAPI	Map3DItem_Setbloodcolor/*地图对象3D_设置血条颜色*/(IMap3DItemPtr* _self/*场景对象*/, const color4* c/*血条颜色*/);//设置血条颜色
/*API*/VOID				WINAPI	Map3DItem_Getbloodcolor/*地图对象3D_返回血条颜色*/(IMap3DItemPtr* _self/*场景对象*/, color4* _return);//得到血条颜色
/*API*/VOID				WINAPI	Map3DItem_Setname_display/*地图对象3D_显示名字*/(IMap3DItemPtr* _self/*场景对象*/, BOOL bShow/*名字*/);//设置是否显示名字
/*API*/BOOL				WINAPI	Map3DItem_Getname_display/*地图对象3D_显示名字*/(IMap3DItemPtr* _self/*场景对象*/);//设置是否显示名字
/*API*/VOID				WINAPI	Map3DItem_addhit/*地图对象3D_添加被击效果*/(IMap3DItemPtr* _self/*场景对象*/, int val/*值*/);//自动添加被击效果,场景开启hitdisplay和载入了血条对话框之后有效,setblood
/*API*/VOID				WINAPI	Map3DItem_Setparent/*地图对象3D_设置父对象*/(IMap3DItemPtr* _self/*场景对象*/, IMap3DItemPtr* c/*父对象*/);//设置父对象之后，模型的位置按照父对象的位置进行变化,可以设置为null
/*API*/VOID				WINAPI	Map3DItem_Getparent/*地图对象3D_返回父对象*/(IMap3DItemPtr* _self/*场景对象*/, IMap3DItemPtr* _return);//父对象
/*API*/BOOL				WINAPI	Map3DItem_issceneobj/*地图对象3D_返回是否场景对象*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/BOOL				WINAPI	Map3DItem_isgameobj/*地图对象3D_返回是否游戏对象*/(IMap3DItemPtr* _self/*场景对象*/);
/*API*/VOID				WINAPI	Map3DItem_gameobj/*地图对象3D_返回游戏对象*/(IMap3DItemPtr* _self/*场景对象*/, IGameObjectPtr* _return);//返回角色对象，如果是游戏对话的话，场景对象不作为角色对象返回


/*API*/VOID			WINAPI	Map3DSkill_source/*地图技能3D_设置源对象*/(IMap3DSkillPtr* _self/*接口*/, LPCSTR szSkillMesh/*技能模型*/, IMap3DItemPtr* item/*目标对象*/, LPCSTR szBoneLike/*骨骼名字*/);//设置发射技能、准备技能、命中技能等的对象，这个调用之后sourcepos设置的值将无效,骨骼名字可以类似r hand abc可以填r hand
/*API*/VOID			WINAPI	Map3DSkill_getsource/*地图技能3D_设置源对象*/(IMap3DSkillPtr* _self/*接口*/, IMap3DItemPtr* _return/*目标对象*/);//设置
/*API*/VOID			WINAPI	Map3DSkill_Setsourcepos/*地图技能3D_设置源对象位置*/(IMap3DSkillPtr* _self/*接口*/, float3* pos/*目标对象*/);//设置发射技能、准备技能、命中技能等的3D坐标，这个调用之后source设置的对象将无效
/*API*/VOID			WINAPI	Map3DSkill_Getsourcepos/*地图技能3D_取得源对象位置*/(IMap3DSkillPtr* _self/*接口*/, float3* _return/*目标对象*/);//设置
/*API*/VOID			WINAPI	Map3DSkill_Setsource_zscale/*地图技能3D_设置准备技能高度位移*/(IMap3DSkillPtr* _self/*接口*/, float fScale/*缩放*/);//设置命中位置是按照角色高度百分比，如果骨骼位置无效可以使用这个，只有在source对象模式下有效，在sourcepos设置的时候无效,对target也有效
/*API*/float		WINAPI	Map3DSkill_Getsource_zscale/*地图技能3D_设置准备技能高度位移*/(IMap3DSkillPtr* _self/*接口*/);//设置命中位置是按照角色高度百分比，如果骨骼位置无效可以使用这个
/*API*/VOID			WINAPI	Map3DSkill_Settarget_zscale/*地图技能3D_设置准备技能高度位移*/(IMap3DSkillPtr* _self/*接口*/, float fScale/*缩放*/);//设置命中位置是按照角色高度百分比，如果骨骼位置无效可以使用这个，只有在source对象模式下有效，在sourcepos设置的时候无效,对target也有效
/*API*/float		WINAPI	Map3DSkill_Gettarget_zscale/*地图技能3D_设置准备技能高度位移*/(IMap3DSkillPtr* _self/*接口*/);//设置命中位置是按照角色高度百分比，如果骨骼位置无效可以使用这个
/*API*/VOID			WINAPI	Map3DSkill_Setsize/*地图技能3D_设置准备技能高度位移*/(IMap3DSkillPtr* _self/*接口*/, float fScale/*缩放*/);//设置技能模型的大小
/*API*/float		WINAPI	Map3DSkill_Getsize/*地图技能3D_设置准备技能高度位移*/(IMap3DSkillPtr* _self/*接口*/);//设置技能模型的大小

/*API*/VOID			WINAPI	Map3DSkill_Setmovespeed/*地图技能3D_设置跟踪速度*/(IMap3DSkillPtr* _self/*接口*/, float speed/*速度*/);//设置
/*API*/float		WINAPI	Map3DSkill_Getmovespeed/*地图技能3D_取得跟踪速度*/(IMap3DSkillPtr* _self/*接口*/);//设置
/*API*/VOID			WINAPI	Map3DSkill_target/*地图技能3D_设置目标对象*/(IMap3DSkillPtr* _self/*接口*/, IMap3DItemPtr* item/*目标对象*/, LPCSTR szBoneLike/*骨骼名字*/);//设置发射技能技能的目标对象，设置后即能将会始终跟踪目标直到到达目标才会消失，这个调用之后targetpos设置的值将无效
/*API*/VOID			WINAPI	Map3DSkill_gettarget/*地图技能3D_设置目标对象*/(IMap3DSkillPtr* _self/*接口*/, IMap3DItemPtr* _return/*目标对象*/);//设置
/*API*/VOID			WINAPI	Map3DSkill_Settargetpos/*地图技能3D_设置目标对象位置*/(IMap3DSkillPtr* _self/*接口*/, float3* pos/*目标对象*/);//设置发射技能技能的目标3D坐标，设置后即能将会始终跟踪目标直到到达目标才会消失，这个调用之后target设置的值将无效
/*API*/VOID			WINAPI	Map3DSkill_Gettargetpos/*地图技能3D_设置目标对象位置*/(IMap3DSkillPtr* _self/*接口*/, float3* _return/*目标对象*/);//设置
/*API*/VOID			WINAPI	Map3DSkill_Setwaittime/*地图技能3D_设置等待时间*/(IMap3DSkillPtr* _self/*接口*/, float fwaittime/*速度*/);//设置
/*API*/float		WINAPI	Map3DSkill_Getwaittime/*地图技能3D_取得等待时间*/(IMap3DSkillPtr* _self/*接口*/);//设置
/*API*/VOID			WINAPI	Map3DSkill_Setcontinuetime/*地图技能3D_设置持续时间*/(IMap3DSkillPtr* _self/*接口*/, float ftime/*时间*/);//设置，如果设置了目标，这个参数将无效，设置技能持续播放时间
/*API*/float		WINAPI	Map3DSkill_Getcontinuetime/*地图技能3D_取得持续时间*/(IMap3DSkillPtr* _self/*接口*/);//设置
/*API*/VOID			WINAPI	Map3DSkill_start/*地图技能3D_开始播放技能*/(IMap3DSkillPtr* _self/*接口*/, LPCSTR szFinishFunc/*到达目标点之后被调用的函数*/);//设置,事件[szFinishFunc(skill)--(skill=Map3DSkill)到达目标点之后被调用的函数]
/*API*/VOID			WINAPI	Map3DSkill_stop/*地图技能3D_停止播放技能*/(IMap3DSkillPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3DSkill_bind_collision/*地图技能3D_绑定碰撞检测*/(IMap3DSkillPtr* _self/*接口*/, int classid/*要进行碰撞的目标类型如果是0则全部类型*/, LPCSTR szLuaFunc/*当在技能移动时碰撞了就会被调用*/);//只有在targetpos设置有效值的时候才会触发碰撞检测函数，其他任何情况都不会触发,事件[szLuaFunc(skill,obj,pos)--(skill=Map3DSkill,obj=Map3DItem,pos=PickPoint)碰撞对象，如果碰撞场景则此项返回空，Map3DItem::isnull函数来判断,PickPoint位置,碰撞面法线等,碰撞检测之后将立即结束技能]

/*API*/INT			WINAPI	Map3DPoint_id/*地图移动点3D_返回ID*/(IMap3DPointPtr* _self/*接口*/);
/*API*/LPCSTR		WINAPI	Map3DPoint_Getname/*地图移动点3D_返回名字*/(IMap3DPointPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3DPoint_Setname/*地图移动点3D_设置名字*/(IMap3DPointPtr* _self/*接口*/, LPCSTR szName/*名字*/);//设置
/*API*/VOID			WINAPI	Map3DPoint_Getpos/*地图移动点3D_返回位置*/(IMap3DPointPtr* _self/*接口*/, float3* _return/*地图位置*/);//设置
/*API*/VOID			WINAPI	Map3DPoint_Setpos/*地图移动点3D_设置位置*/(IMap3DPointPtr* _self/*接口*/, const float3* mappos/*地图位置*/);//设置
/*API*/float		WINAPI	Map3DPoint_Getrange/*地图移动点3D_返回区域*/(IMap3DPointPtr* _self/*接口*/);//设置
/*API*/VOID			WINAPI	Map3DPoint_Setrange/*地图移动点3D_设置区域*/(IMap3DPointPtr* _self/*接口*/, float range/*大小*/);//设置

/*API*/VOID			WINAPI	Map3D_freeskill/*地图3D_返回空闲技能*/(IMap3DPtr* _self/*场景3D*/, IMap3DSkillPtr* _return/*返回*/);//创建一个新的技能，每次技能发起之前都需要调用这个来取得新的空闲技能指针
/*API*/INT			WINAPI	Map3D_skycount/*地图3D_取天空数量*/(IMap3DPtr* _self/*场景3D*/);
/*API*/VOID			WINAPI	Map3D_skyat/*地图3D_取天空*/(IMap3DPtr* _self/*场景3D*/, INT nIndex/*索引*/, ISkyPtr* _return/*ret*/);
/*API*/INT			WINAPI	Map3D_envcount/*地图3D_取雾数量*/(IMap3DPtr* _self/*场景3D*/);
/*API*/VOID			WINAPI	Map3D_envat/*地图3D_取雾*/(IMap3DPtr* _self/*场景3D*/, INT nIndex/*索引*/, IMap3DEnvPtr* _return/*ret*/);
/*API*/LPCSTR		WINAPI	Map3D_Getactivecamera/*地图3D_取得激活摄像机方案*/(IMap3DPtr* _self/*场景3D*/);
/*API*/VOID			WINAPI	Map3D_Setactivecamera/*地图3D_设置激活摄像机方案*/(IMap3DPtr* _self/*场景3D*/, LPCSTR szCamera/*设置*/);
/*API*/LPCSTR		WINAPI	Map3D_GetactiveEnvBy/*地图3D_取得激活雾方案*/(IMap3DPtr* _self/*场景3D*/);
/*API*/VOID			WINAPI	Map3D_SetactiveEnvBy/*地图3D_设置激活雾方案*/(IMap3DPtr* _self/*场景3D*/, LPCSTR szEnv/*设置*/);
/*API*/VOID			WINAPI	Map3D_GetactiveEnv/*地图3D_取得激活雾方案*/(IMap3DPtr* _self/*场景3D*/, IMap3DEnvPtr* _return/*设置*/);
/*API*/VOID			WINAPI	Map3D_SetactiveEnv/*地图3D_设置激活雾方案*/(IMap3DPtr* _self/*场景3D*/, IMap3DEnvPtr* env/*设置*/);
/*API*/LPCSTR		WINAPI	Map3D_Getsky/*地图3D_取得激活天空方案*/(IMap3DPtr* _self/*场景3D*/);
/*API*/VOID			WINAPI	Map3D_Setsky/*地图3D_设置激活天空方案*/(IMap3DPtr* _self/*场景3D*/, LPCSTR szCamera/*设置*/);
/*API*/VOID			WINAPI	Map3D_blend_camera/*地图3D_激活摄像机方案*/(IMap3DPtr* _self/*场景3D*/, LPCSTR szCamera/*设置*/, float fBlendTime/*混合时间*/);
/*API*/VOID			WINAPI	Map3D_blend_fog/*地图3D_激活雾方案*/(IMap3DPtr* _self/*场景3D*/, LPCSTR szCamera/*设置*/, float fBlendTime/*混合时间*/);
/*API*/VOID			WINAPI	Map3D_blend_sky/*地图3D_激活天空方案*/(IMap3DPtr* _self/*场景3D*/, LPCSTR szCamera/*设置*/, float fBlendTime/*混合时间*/);
/*API*/VOID			WINAPI	Map3D_camerafollow/*地图3D_镜头跟随设置*/(IMap3DPtr* _self/*接口*/, IMap3DItemPtr* obj/*对象*/);//镜头跟随对象设置
/*API*/INT			WINAPI	Map3D_count/*地图3D_返回对象数量*/(IMap3DPtr* _self/*接口*/);//返回对象数量
/*API*/VOID			WINAPI	Map3D_at/*地图3D_返回对象*/(IMap3DPtr* _self/*接口*/, INT index/*索引*/, IMap3DItemPtr* _return/*返回*/);//返回第N个对象
/*API*/VOID			WINAPI	Map3D_searchby/*地图3D_搜素对象*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*返回第一个找到的对象*/, IMap3DItemPtr* _return/*返回*/);//返回第N个对象
/*API*/VOID			WINAPI	Map3D_search/*地图3D_搜素对象ByID*/(IMap3DPtr* _self/*接口*/, INT cID/*返回第一个找到的对象*/, IMap3DItemPtr* _return/*返回*/);//返回第N个对象
/*API*/VOID			WINAPI	Map3D_searchpointby/*地图3D_搜素移动点*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*返回第一个找到的对象*/, IMap3DPointPtr* _return/*返回*/);//返回搜素移动点
/*API*/VOID			WINAPI	Map3D_searchpoint/*地图3D_搜素移动点ByID*/(IMap3DPtr* _self/*接口*/, INT id/*返回第一个找到的对象*/, IMap3DPointPtr* _return/*返回*/);//返回搜素移动点
/*API*/VOID			WINAPI	Map3D_searchpointby_eid/*地图3D_搜素移动点根据事件类型ID*/(IMap3DPtr* _self/*接口*/, INT eventid/*返回第一个找到的对象*/, IMap3DPointPtr* _return/*返回*/);//返回搜素移动点,事件ID在event_table.txt中定义，并在编辑器内给移动点设置，只返回第一个
/*API*/BOOL			WINAPI	Map3D_Getdisplay/*地图3D_是否显示*/(IMap3DPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3D_Setdisplay/*地图3D_是否隐藏*/(IMap3DPtr* _self/*接口*/, BOOL bVisible/*是否隐藏*/);
/*API*/BOOL			WINAPI	Map3D_Getpause/*地图3D_是否暂停*/(IMap3DPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3D_Setpause/*地图3D_是否暂停*/(IMap3DPtr* _self/*接口*/, BOOL bPause/*是否暂停*/);
/*API*/BOOL			WINAPI	Map3D_Getblooddisplay/*地图3D_是否显示血条*/(IMap3DPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3D_Setblooddisplay/*地图3D_是否隐藏血条*/(IMap3DPtr* _self/*接口*/, BOOL bVisible/*是否隐藏*/);
/*API*/BOOL			WINAPI	Map3D_Gethitdisplay/*地图3D_是否显示被击效果*/(IMap3DPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3D_Sethitdisplay/*地图3D_是否隐藏被击效果*/(IMap3DPtr* _self/*接口*/, BOOL bVisible/*是否隐藏*/);
/*API*/float		WINAPI	Map3D_Getbloodsize/*地图3D_取得血条大小*/(IMap3DPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3D_Setbloodsize/*地图3D_设置血条大小*/(IMap3DPtr* _self/*接口*/, float fSize/*大小*/);
/*API*/BOOL			WINAPI	Map3D_Getfindway2d/*地图3D_是否2D寻路*/(IMap3DPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3D_Setfindway2d/*地图3D_设置2D寻路*/(IMap3DPtr* _self/*接口*/, BOOL b2DMode/*是否*/);
/*API*/VOID			WINAPI	Map3D_Getcamera/*地图3D_返回镜头*/(IMap3DPtr* _self/*接口*/, Pixel* _return/*位置*/);
/*API*/VOID			WINAPI	Map3D_Setcamera/*地图3D_设置镜头*/(IMap3DPtr* _self/*接口*/, Pixel* pos/*位置*/);
/*API*/INT			WINAPI	Map3D_classcount/*地图3D_返回类聚对象数量*/(IMap3DPtr* _self/*接口*/, INT classID/*类ID*/);//返回类聚的数量,classID是0到100之间的任意数字
/*API*/VOID			WINAPI	Map3D_classat/*地图3D_返回类聚对象*/(IMap3DPtr* _self/*接口*/, INT classID/*类ID*/, INT index/*索引*/, IMap3DItemPtr* _return/*返回*/);//返回第N个类的对象,classID是0到100之间的任意数字
/*API*/BOOL			WINAPI	Map3D_Getdisplayterrain/*地图3D_是否显示地形*/(IMap3DPtr* _self/*接口*/);
/*API*/VOID			WINAPI	Map3D_Setdisplayterrain/*地图3D_是否隐藏地形*/(IMap3DPtr* _self/*接口*/, BOOL bVisible/*是否隐藏*/);
/*API*/BOOL			WINAPI	Map3D_Getenablemyclick/*地图3D_是否可以点击地形*/(IMap3DPtr* _self/*接口*/);//不对gamemgr的场景点击进行判断,针对地形系统和地形对象有效,Map3DItem,Map3D
/*API*/VOID			WINAPI	Map3D_Setenablemyclick/*地图3D_是否可以点击地形*/(IMap3DPtr* _self/*接口*/, BOOL bEnabled/*是否有效*/);//不对gamemgr的场景点击进行判断,针对地形系统和地形对象有效,Map3DItem,Map3D
/*API*/BOOL			WINAPI	Map3D_Getenabled/*地图3D_是否有效并能点击*/(IMap3DPtr* _self/*接口*/);//不对gamemgr的场景点击进行判断,针对地形系统和地形对象有效,Map3DItem,Map3D
/*API*/VOID			WINAPI	Map3D_Setenabled/*地图3D_是否有效并能点击*/(IMap3DPtr* _self/*接口*/, BOOL bEnabled/*是否有效*/);//不对gamemgr的场景点击进行判断,针对地形系统和地形对象有效,Map3DItem,Map3D
/*API*/int			WINAPI	Map3D_Getclickradius/*地图3D_返回点击有效范围*/(IMap3DPtr* _self/*接口*/);//当按下弹起时如果超过这个距离就不计算为点击，默认10像素
/*API*/VOID			WINAPI	Map3D_Setclickradius/*地图3D_设置点击有效范围*/(IMap3DPtr* _self/*接口*/, int radius/*范围*/);//当按下弹起时如果超过这个距离就不计算为点击，默认10像素
//
/*API*/VOID			WINAPI	Map3D_replace_image/*地图3D_绑定图片*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*对象名字*/, LPCSTR szImageSrc/*原图片文件路径*/, LPCSTR szImageReplace/*替换图片文件路径*/);//替换图片
/*API*/VOID			WINAPI	Map3D_bind_eventby/*地图3D_绑定脚本到按下事件*/(IMap3DPtr* _self/*接口*/, LPCSTR szEvent/*事件名*/, LPCSTR szItemName/*对象名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//绑定一个事件,事件查看幻影脚本帮助中的eventid枚举常数说明,事件[szLuaFunc(obj)--(obj=Map3DItem)对象点击事件]
/*API*/VOID			WINAPI	Map3D_bind_event/*地图3D_绑定脚本到事件*/(IMap3DPtr* _self/*接口*/, EventID nid/*事件名*/, LPCSTR szItemName/*对象名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//绑定一个事件,事件查看幻影脚本帮助中的eventid枚举常数说明,事件[szLuaFunc(obj)--(obj=Map3DItem)对象点击事件]
/*API*/VOID			WINAPI	Map3D_bind_click/*地图3D_绑定脚本到按下事件*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*对象名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//对象点击事件,事件[szLuaFunc(obj)--(obj=Map3DItem)对象点击事件]
/*API*/VOID			WINAPI	Map3D_bind_draw/*地图3D_绑定渲染队列*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*对象名字*/, IUIDrawMgrPtr* drawPtr/*组合*/, LPCSTR szBoneName/*骨骼名字*/);//绑定渲染队列到某个对象
/*API*/VOID			WINAPI	Map3D_bind_mapeventby/*地图3D_绑定场景脚本事件*/(IMap3DPtr* _self/*接口*/, LPCSTR szEvent/*事件名*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//绑定一个事件到场景,事件[szLuaFunc(pos,normal,length)--(pos=Pixel,normal=Pixel)参数是pos是float3类型的命中点位置,normal是命中点法线，length是命中点与视点的距离]
/*API*/VOID			WINAPI	Map3D_bind_mapevent/*地图3D_绑定场景脚本事件*/(IMap3DPtr* _self/*接口*/, EventID nid/*事件ID*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//绑定一个事件到场景,事件[szLuaFunc(pos,normal,length)--(pos=Pixel,normal=Pixel)参数是pos是float3类型的命中点位置,normal是命中点法线，length是命中点与视点的距离]
/*API*/VOID			WINAPI	Map3D_scenepos/*地图3D_返回场景中的位置*/(IMap3DPtr* _self/*接口*/, Pixel* screen/*屏幕位置*/, Pixel* _return/*位置*/);
/*API*/VOID			WINAPI	Map3D_screenpos/*地图3D_返回屏幕中的位置*/(IMap3DPtr* _self/*接口*/, Pixel* scene/*场景位置*/, Pixel* _return/*位置*/);

/*API*/VOID			WINAPI	Map3D_insert/*地图3D_插入对象*/(IMap3DPtr* _self/*接口*/, LPCSTR szFileName/*模型路径*/, Pixel* mappos/*地图坐标*/, IMap3DItemPtr* _return/*返回*/);
/*API*/VOID			WINAPI	Map3D_remove/*地图3D_删除对象*/(IMap3DPtr* _self/*接口*/, IMap3DItemPtr* _item/*返回*/);
/*API*/VOID			WINAPI	Map3D_show/*地图3D_显示对象*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*对象名字*/, BOOL bShow/*是否显示*/);
/*API*/VOID			WINAPI	Map3D_enable/*地图3D_让对象有效*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*对象名字*/, BOOL bEnable/*是否有效*/);
/*API*/VOID			WINAPI	Map3D_show_s/*地图3D_显示静态对象*/(IMap3DPtr* _self/*接口*/, INT nID/*对象ID*/, BOOL bShow/*是否显示*/);
/*API*/VOID			WINAPI	Map3D_enable_s/*地图3D_让静态对象有效*/(IMap3DPtr* _self/*接口*/, INT nID/*对象ID*/, BOOL bEnable/*是否有效*/);
/*API*/BOOL			WINAPI	Map3D_setblood/*地图3D_设置场景血条对话框*/(IMap3DPtr* _self/*接口*/, LPCSTR szDlgName/*对象名字*/);//设置场景血条对话框,其中blood控件是血条进度条控件,hit控件是伤害控件

/*API*/void			WINAPI	GameObject_action/*游戏对象_动作*/(IGameObjectPtr* _self/*游戏对象*/, ActionType type/*动作*/, float fTime/*时间*/);//动作进行一定时间的静止
/*API*/void			WINAPI	GameObject_action2/*游戏对象_动作2*/(IGameObjectPtr* _self/*游戏对象*/, ActionType type/*动作*/, int nLoop);//按照循环次数播放一个动作
/*API*/BOOL			WINAPI	GameObject_moveto/*游戏对象_移动到目标点*/(IGameObjectPtr* _self/*游戏对象*/, const float3* pos/*位置*/);//移动到目标点，如果可以行走到目标点返回true
/*API*/BOOL			WINAPI	GameObject_moveto_t/*游戏对象_移动到目标对象*/(IGameObjectPtr* _self/*游戏对象*/);//首先要设置目标对象target，然后调用这个函数，否则返回false，如果目标对象死亡也返回false
/*API*/VOID			WINAPI	GameObject_moveto_b/*游戏对象_移动到出生点*/(IGameObjectPtr* _self/*游戏对象*/);//对象移动到出生点
/*API*/BOOL			WINAPI	GameObject_moveto_e/*游戏对象_移动到事件点*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szEvent/*事件点*/);//对象移动到事件点
/*API*/int			WINAPI	GameObject_nextto_e/*游戏对象_下一个移动到事件点*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szEvent/*事件点*/);//下一个对象移动到事件点
/*API*/int			WINAPI	GameObject_nextto/*游戏对象_下一个移动到点*/(IGameObjectPtr* _self/*游戏对象*/, const float3* pos/*位置*/);//下一个对象移动到点
/*API*/int			WINAPI	GameObject_nextidle/*游戏对象_下一个静止*/(IGameObjectPtr* _self/*游戏对象*/, float fTime/*时间*/);//下一个动作进行一定时间的静止
/*API*/int			WINAPI	GameObject_nextaction/*游戏对象_下一个动作*/(IGameObjectPtr* _self/*游戏对象*/, ActionType type/*动作*/, float fTime/*时间*/);//下一个动作进行一定时间的静止
/*API*/int			WINAPI	GameObject_nextaction2/*游戏对象_下一个动作*/(IGameObjectPtr* _self/*游戏对象*/, ActionType type/*动作*/, int nLoop);//按照循环次数播放一个动作
/*API*/int			WINAPI	GameObject_nextlookat/*游戏对象_循环所有设定*/(IGameObjectPtr* _self/*游戏对象*/, const float3* pos/*目标点*/);//盯着目标点
/*API*/int			WINAPI	GameObject_nextlookat_t/*游戏对象_循环所有设定*/(IGameObjectPtr* _self/*游戏对象*/, IGameObjectPtr* obj/*目标对象*/);//盯着目标对象
/*API*/int			WINAPI	GameObject_nextloop/*游戏对象_循环所有设定*/(IGameObjectPtr* _self/*游戏对象*/, int push_begin/*push_系列返回的索引值*/, int push_end/*push_系列返回的索引值结束*/, int push_loop/*循环多少次这个动作*/);//设置下一个要播放的动作
/*API*/void			WINAPI	GameObject_nextclear/*游戏对象_下一个播放*/(IGameObjectPtr* _self/*游戏对象*/);//清除所有下一步编程
/*API*/void			WINAPI	GameObject_Setnextpause/*游戏对象_暂停播放*/(IGameObjectPtr* _self/*游戏对象*/, BOOL bPause/*暂定与否*/);
/*API*/BOOL			WINAPI	GameObject_Getnextpause/*游戏对象_暂停播放*/(IGameObjectPtr* _self/*游戏对象*/);
/*API*/void			WINAPI	GameObject_next_setid/*游戏对象_设置某个索引Next的ID*/(IGameObjectPtr* _self/*游戏对象*/, int index/*索引*/, int nid/*NID*/);
/*API*/void			WINAPI	GameObject_next_setrandom/*游戏对象_设置某个ID的随机值*/(IGameObjectPtr* _self/*游戏对象*/, int index/*索引*/, float fRandom/*发生概率*/);//发生概率在0到100之间，值越大发生概率越高
/*API*/int			WINAPI	GameObject_roleid/*游戏对象_角色表对应ID*/(IGameObjectPtr* _self/*游戏对象*/);
/*API*/int			WINAPI	GameObject_sceneid/*游戏对象_场景编辑器对应场景对象ID*/(IGameObjectPtr* _self/*游戏对象*/);

/*API*/VOID			WINAPI	GameObject_bind_idle_over/*游戏对象_绑定_idle事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//游戏对象休闲时间结束时调用,事件[szLuaFunc(obj)--(obj=GameObject)游戏对象休闲时间结束时调用]
/*API*/VOID			WINAPI	GameObject_bind_move_over/*游戏对象_绑定_move事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//游戏对象移动结束时调用,事件[szLuaFunc(obj)--(obj=GameObject)游戏对象移动结束时调用]
/*API*/VOID			WINAPI	GameObject_bind_die_over/*游戏对象_绑定_die事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//游戏对象死亡结束时调用,事件[szLuaFunc(obj)--(obj=GameObject)游戏对象死亡结束时调用]
/*API*/VOID			WINAPI	GameObject_bind_action_over/*游戏对象_绑定_action事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//游戏对象的动作结束事件触发时被调用,事件[szLuaFunc(obj,atype)--(obj=GameObject,atype=ActionType)游戏对象的动作结束事件触发时被调用]
/*API*/VOID			WINAPI	GameObject_bind_attack_over/*游戏对象_绑定_attack事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//游戏对象攻击结束时调用,事件[szLuaFunc(obj,tobj)--(obj=GameObject,tobj=GameObject)游戏对象攻击结束时调用]
/*API*/VOID			WINAPI	GameObject_bind_hit_over/*游戏对象_绑定_hit事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//游戏对象被击的时候被调用,事件[szLuaFunc(obj,tobj)--(obj=GameObject,tobj=GameObject)游戏对象被击的时候被调用,tobj是攻击方对象]
/*API*/VOID			WINAPI	GameObject_bind_nextbegin/*游戏对象_绑定_nextbegin事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//游戏对象下一个动作执行前被调用,nid是之前设置好的自定义,事件[szLuaFunc(obj,nid)--(obj=GameObject,nid=int)游戏对象下一个动作执行前被调用,nid是之前设置好的自定义]
/*API*/VOID			WINAPI	GameObject_bind_action_event/*游戏对象_绑定_actionevent事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/);//游戏对象的动作事件触发时被调用,事件[szLuaFunc(obj,atype)--(obj=GameObject,atype=ActionType)游戏对象的动作事件触发时被调用]

//
/*API*/VOID			WINAPI	GameObject_idle/*游戏对象_休闲动作*/(IGameObjectPtr* _self/*游戏对象*/, float fTime/*休闲动作持续时间*/);//休闲动作自动选择
/*API*/VOID			WINAPI	GameObject_onhit/*游戏对象_被击动作*/(IGameObjectPtr* _self/*游戏对象*/, IGameObjectPtr* enemy/*敌人*/, int val/*造成伤害的值*/, float fPlayTime/*播放动作最长时间*/);//对象被攻击的时候调用
/*API*/VOID			WINAPI	GameObject_onhit2/*游戏对象_被击动作返回控件指针*/(IGameObjectPtr* _self/*游戏对象*/, IGameObjectPtr* enemy/*敌人*/, int val/*造成伤害的值*/, float fPlayTime/*播放动作最长时间*/, IControlPtr* _return);//对象被攻击的时候调用
/*API*/BOOL			WINAPI	GameObject_Gethit_text/*游戏对象_取得是否显示被击文字*/(IGameObjectPtr* _self/*游戏对象*/);//设置是否显示被击文字，角色头顶上在被调用onhit时自动显示被击文字
/*API*/VOID			WINAPI	GameObject_Sethit_text/*游戏对象_设置是否显示被击文字*/(IGameObjectPtr* _self/*游戏对象*/, BOOL bText/*显示被击文字*/);

/*API*/VOID			WINAPI	GameObject_clone/*游戏对象_克隆对象*/(IGameObjectPtr* _self/*游戏对象*/, IGameObjectPtr* _return/*游戏对象*/);
/*API*/VOID			WINAPI	GameObject_die/*游戏对象_死亡*/(IGameObjectPtr* _self/*游戏对象*/);
/*API*/BOOL			WINAPI	GameObject_attack/*游戏对象_攻击*/(IGameObjectPtr* _self/*游戏对象*/, int actionindex/*攻击动作组的索引*/);//进行攻击动作,动作索引是按照顺序出现的动作类型为"攻击"的索引
/*API*/BOOL			WINAPI	GameObject_doskill_r/*游戏对象_使用技能攻击目标点群体*/(IGameObjectPtr* _self/*游戏对象*/, int skillid/*技能表中的技能ID*/, const float3* target/*技能施放目标点*/);//如果技能ID是0的话则使用默认的物理攻击,技能表skill_table.txt,只有群体技能才能使用这个
/*API*/BOOL			WINAPI	GameObject_doskill_t/*游戏对象_使用技能攻击当前目标*/(IGameObjectPtr* _self/*游戏对象*/, int skillid/*技能表中的技能ID*/);//如果技能ID是0的话则使用默认的物理攻击,技能表skill_table.txt,会自动走进目标最佳攻击距离点
/*API*/VOID			WINAPI	GameObject_lookat/*游戏对象_面向目标*/(IGameObjectPtr* _self/*游戏对象*/, const float3* target/*位置*/);//面向目标转动
/*API*/VOID			WINAPI	GameObject_lookat_t/*游戏对象_面向目标对象*/(IGameObjectPtr* _self/*游戏对象*/);//面向目标对象转动
/*API*/VOID			WINAPI	GameObject_Gettarget/*游戏对象_取得目标对象*/(IGameObjectPtr* _self/*游戏对象*/, IGameObjectPtr* _return/*游戏对象*/);
/*API*/VOID			WINAPI	GameObject_Settarget/*游戏对象_设置目标对象*/(IGameObjectPtr* _self/*游戏对象*/, const IGameObjectPtr* target/*游戏对象*/);
/*API*/VOID			WINAPI	GameObject_Getbornpos/*游戏对象_取得出生点坐标*/(IGameObjectPtr* _self/*游戏对象*/, float3* _return/*位置*/);
/*API*/VOID			WINAPI	GameObject_Setbornpos/*游戏对象_设置出生点坐标*/(IGameObjectPtr* _self/*游戏对象*/, const float3* target/*位置*/);
/*API*/VOID			WINAPI	GameObject_Getpos/*游戏对象_取得坐标*/(IGameObjectPtr* _self/*游戏对象*/, float3* _return/*位置*/);
/*API*/VOID			WINAPI	GameObject_Setpos/*游戏对象_设置坐标*/(IGameObjectPtr* _self/*游戏对象*/, const float3* pos/*位置*/);
/*API*/VOID			WINAPI	GameObject_update_blood/*游戏对象_更新场景对象血条信息*/(IGameObjectPtr* _self/*游戏对象*/, float fProgress/*血条显示进度*/);//更新血条信息,通常是当前生命值/最大生命值(0到1之间)
/*API*/BOOL			WINAPI	GameObject_nextenemy/*游戏对象_选择下一个敌人*/(IGameObjectPtr* _self/*游戏对象*/);//自动选择下一个敌人，从最后一个攻击我的角色开始选择，然后是在攻击范围内的角色
/*API*/VOID			WINAPI	GameObject_pause_t/*游戏对象_暂停一段时间*/(IGameObjectPtr* _self/*游戏对象*/, float fSecond/*秒*/);//暂停计算一段时间,过了这个时间将自动继续
/*API*/float		WINAPI	GameObject_read/*游戏对象_读取一个小数变量*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szName/*变量名*/);//读取一个小数变量
/*API*/VOID			WINAPI	GameObject_write/*游戏对象_写入一个小数变量*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szName/*变量名*/, float nValue/*值*/);//写入一个小数变量
/*API*/VOID			WINAPI	GameObject_sceneobj/*游戏对象_返回场景对象类*/(IGameObjectPtr* _self/*游戏对象*/, IMap3DItemPtr* _return/*返回3D场景对象*/);//从这里返回的场景对象不要直接进行游戏内容和逻辑还有位置设置等操作，将会无效
/*API*/float		WINAPI	GameObject_Getaffrange/*游戏对象_取得影响范围变量*/(IGameObjectPtr* _self/*游戏对象*/);//在编辑器内设置的影响范围值
/*API*/VOID			WINAPI	GameObject_Setaffrange/*游戏对象_设置影响范围变量*/(IGameObjectPtr* _self/*游戏对象*/, float affRange/*位置*/);
/*API*/BOOL			WINAPI	GameObject_Getmoverun/*游戏对象_取得是否跑步*/(IGameObjectPtr* _self/*游戏对象*/);//设置是否进入跑步模式
/*API*/VOID			WINAPI	GameObject_Setmoverun/*游戏对象_设置是否跑步*/(IGameObjectPtr* _self/*游戏对象*/, BOOL bRun/*是否跑步*/);
/*API*/float		WINAPI	GameObject_height/*游戏对象_取得角色高度*/(IGameObjectPtr* _self/*游戏对象*/);//返回角色高度
/*API*/float		WINAPI	GameObject_Getatkrange/*游戏对象_取得攻击范围变量*/(IGameObjectPtr* _self/*游戏对象*/);//在编辑器内设置的攻击范围值
/*API*/VOID			WINAPI	GameObject_Setatkrange/*游戏对象_设置攻击范围变量*/(IGameObjectPtr* _self/*游戏对象*/, float atkRange/*位置*/);
/*API*/BOOL			WINAPI	GameObject_Getmeshheight/*游戏对象_取得计算取得模型碰撞盒高度*/(IGameObjectPtr* _self/*游戏对象*/);//是否启动模型高度取得，不能所有的角色都是用这个，否则效率开销很大，所以一般只需要设置主角的高度从模型高度取得就可以了
/*API*/VOID			WINAPI	GameObject_Setmeshheight/*游戏对象_设置计算取得模型碰撞盒高度*/(IGameObjectPtr* _self/*游戏对象*/, BOOL bEnable/*是否有效*/);//是否启动模型高度取得，不能所有的角色都是用这个，否则效率开销很大，所以一般只需要设置主角的高度从模型高度取得就可以了
/*API*/BOOL			WINAPI	GameObject_Getsearchway/*游戏对象_取得是否进行寻路*/(IGameObjectPtr* _self/*游戏对象*/);//取得是否进行寻路
/*API*/VOID			WINAPI	GameObject_Setsearchway/*游戏对象_设置是否进行寻路*/(IGameObjectPtr* _self/*游戏对象*/, BOOL bEnable/*是否有效*/);//取得是否进行寻路
/*API*/BOOL			WINAPI	GameObject_Getmovedir_enable/*游戏对象_取得是否自动按照方向移动*/(IGameObjectPtr* _self/*游戏对象*/);//取得是否自动按照方向移动
/*API*/VOID			WINAPI	GameObject_Setmovedir_enable/*游戏对象_设置是否自动按照方向移动*/(IGameObjectPtr* _self/*游戏对象*/, BOOL bRun/*是否跑步*/);//取得是否自动按照方向移动
/*API*/VOID			WINAPI	GameObject_movedir/*游戏对象_设置自动方向移动*/(IGameObjectPtr* _self/*游戏对象*/, const float3* dir/*方向*/);//自动按照方向移动
/*API*/float		WINAPI	GameObject_Getrot/*游戏对象_取得沿Z轴转动率变量*/(IGameObjectPtr* _self/*游戏对象*/);//在编辑器内设置的影响范围值
/*API*/VOID			WINAPI	GameObject_Setrot/*游戏对象_设置沿Z轴转动率变量*/(IGameObjectPtr* _self/*游戏对象*/, float fRot/*转动率*/);

/*API*/VOID			WINAPI	System_Setopacitymode/*系统_设置透明混合模式*/(AlphaMode nMode/*AlphaBlend_开头*/);
/*API*/AlphaMode	WINAPI	System_Getopacitymode/*系统_取透明混合模式*/();
/*API*/VOID			WINAPI	System_Setshowfps/*系统_显示FPS*/(BOOL b/*设置*/);
/*API*/BOOL			WINAPI	System_Getshowfps/*系统_是否显示FPS*/();
/*API*/INT			WINAPI	System_fps/*系统_返回当前FPS*/();
/*API*/VOID			WINAPI	System_Setmaxfps/*系统_设置最高FPS*/(INT n/*设置*/);//设置最高FPS(每秒渲染次数),可以很好的节省电池使用两，一般2D游戏20fps,3D游戏30fps,FPS游戏则不限制，最高时1000
/*API*/INT			WINAPI	System_Getmaxfps/*系统_取得最高FPS*/();//设置最高FPS(每秒渲染次数),可以很好的节省电池使用两，一般2D游戏20fps,3D游戏30fps,FPS游戏则不限制，最高时1000
/*API*/VOID			WINAPI	System_Setbackcolor/*系统_设置背景清除色*/(const color4* c/*颜色*/);
/*API*/VOID			WINAPI	System_Getbackcolor/*系统_设置背景清除色*/(color4* _return/*ret*/);
/*API*/VOID			WINAPI	System_Setshowui/*系统_渲染UI*/(BOOL b/*设置*/);
/*API*/BOOL			WINAPI	System_Getshowui/*系统_是否渲染UI*/();
/*API*/VOID			WINAPI	System_Setenablez/*系统_开启Z缓冲*/(BOOL b/*开关*/);
/*API*/BOOL			WINAPI	System_Getenablez/*系统_是否开启Z缓冲*/();
/*API*/VOID			WINAPI	System_Setcullmode/*系统_设置渲染面*/(CullMode nCull/*CullMode_开头*/);
/*API*/CullMode		WINAPI	System_Getcullmode/*系统_取得渲染面*/();
/*API*/VOID			WINAPI	System_Setmute/*系统_设置静音*/(BOOL bMute/*是否静音*/);
/*API*/BOOL			WINAPI	System_Getmute/*系统_是否静音*/();
/*API*/VOID			WINAPI	System_Setuioffset/*系统_设置界面总体位移*/(const float2* off/*值*/);
/*API*/VOID			WINAPI	System_Getuioffset/*系统_取界面总体位移*/(float2* _return/*值*/);
/*API*/VOID			WINAPI	System_Setuiscale/*系统_设置界面总体缩放*/(const float2* s/*值*/);
/*API*/VOID			WINAPI	System_Getuiscale/*系统_取界面总体缩放*/(float2* _return/*值*/);
/*API*/VOID			WINAPI	System_Setenablevr/*系统_开启VR渲染模式*/(BOOL b/*开关*/);//开始眼睛模式3D虚拟现实左右屏幕渲染功能
/*API*/BOOL			WINAPI	System_Getenablevr/*系统_是否开启VR渲染模式*/();
/*API*/VOID			WINAPI	System_Setenablevr2/*系统_开启VR渲染模式*/(BOOL b/*开关*/);//开始虚拟现实非眼睛模式，无左右屏幕
/*API*/BOOL			WINAPI	System_Getenablevr2/*系统_是否开启VR渲染模式*/();
/*API*/VOID			WINAPI	System_Setvrdistance/*系统_开启VR间距*/(float f/*间距*/);//设置两眼之间的间距
/*API*/float		WINAPI	System_Getvrdistance/*系统_是否开启VR间距*/();
/*API*/VOID			WINAPI	System_Setvr_rotx/*系统_开启VR转动*/(float f/*转动X*/);//
/*API*/float		WINAPI	System_Getvr_rotx/*系统_是否VR转动*/();
/*API*/VOID			WINAPI	System_Setvr_smooth/*系统_开启VR平滑数*/(int s/*间距*/);//
/*API*/int			WINAPI	System_Getvr_smooth/*系统_是否开启VR间距*/();
/*API*/VOID			WINAPI	System_vr_beginsmooth/*系统_开始VR缓慢移动*/();//
/*API*/VOID			WINAPI	System_vr_front/*系统_设置VR正前方*/(const float3* dir);//将这个方向设置成VR的正前方
/*API*/VOID			WINAPI	System_sensor/*系统_获取传感器数据*/(sensordata* _return/*返回*/);//获取传感器数据，包括陀螺仪角速度和空间位置等数据
/*API*/VOID			WINAPI	System_gravity/*系统_获取传感器调整之后重力感应值*/(float2* _return/*返回*/);//获取传感器重力感应数据，左右和前后控制，向左是x值接近-1，向右是x值接近1，直立是y值接近0，前倾是y值接近-1，后倾y值接近1
/*API*/VOID			WINAPI	System_vibrator/*系统_振动器*/(float fSecond/*秒*/);//手机振动
/*API*/VOID			WINAPI	System_Setgravity_smooth/*系统_开启Gravity平滑数*/(int s/*间距*/);//开启
/*API*/int			WINAPI	System_Getgravity_smooth/*系统_是否开启Gravity间距*/();
/*API*/VOID			WINAPI	System_sensor_rotator/*系统_获取传感器转动角度*/(float3* _return/*返回*/);//
//
/*API*/BOOL			WINAPI	GameMgr_create/*游戏_创建游戏系统数据*/(int sceneID/*场景ID*/, LPCSTR szLuaFunc/*进度条回调参数*/);//创建游戏系统数据，需要提供scene_table.txt文件中对应的场景ID, 如果“进度条回调参数”不是空文本的话会使用线程载入场景，并回调到这个函数中,事件[szLuaFunc(scene,progress,endof)--(scene=Map3D,progress=float,endof=bool)scene返回正在载入的当前场景指针，progress返回0到1之间的值接近1表示快读取完毕，最后一次endof将返回true表示载入成功了]
/*API*/VOID			WINAPI	GameMgr_scene/*游戏_返回场景*/(IMap3DPtr* _return/*返回*/);//返回当前场景指针
/*API*/INT			WINAPI	GameMgr_objectcount/*游戏_返回对象数量*/();//返回当前场景可控制对象数量
/*API*/VOID			WINAPI	GameMgr_objectat/*游戏_返回对象*/(int index/*索引*/, IGameObjectPtr* _return/*返回*/);//返回某个对象数据
/*API*/VOID			WINAPI	GameMgr_searchby/*游戏_返回对象*/(LPCSTR szName/*名字*/, IGameObjectPtr* _return/*返回*/);//返回某个对象数据
/*API*/VOID			WINAPI	GameMgr_search/*游戏_返回对象根据场景对象ID*/(INT nID/*ID*/, IGameObjectPtr* _return/*返回*/);//返回对象根据场景对象ID
/*API*/VOID			WINAPI	GameMgr_searchby_sceneid/*游戏_返回场景对象ID搜索游戏对象*/(INT sceneID/*ID*/, IGameObjectPtr* _return/*返回*/);//返回对象根据场景对象ID
/*API*/VOID			WINAPI	GameMgr_reborn/*游戏_重新刷怪*/(IGameObjectPtr* obj/*对象*/, float fRebornTime/*重新复活等待时间*/);//重新复活计时某个游戏对象
/*API*/VOID			WINAPI	GameMgr_leader/*游戏_返回主角对象*/(IGameObjectPtr* _return/*返回*/);//返回主角对象数据
/*API*/BOOL			WINAPI	GameMgr_Getcamerafollow/*游戏_返回镜头跟随对象是否有效*/();//返回镜头跟随对象是否有效
/*API*/VOID			WINAPI	GameMgr_Setcamerafollow/*游戏_设置镜头跟随对象是否有效*/(BOOL bEnable/*有效*/);//设置镜头跟随对象是否有效
/*API*/VOID			WINAPI	GameMgr_Getcameratarget/*游戏_返回镜头跟随对象*/(IGameObjectPtr* _return/*返回*/);//返回镜头跟随对象
/*API*/VOID			WINAPI	GameMgr_Setcameratarget/*游戏_设置镜头跟随对象*/(IGameObjectPtr* obj/*对象*/);//设置镜头跟随对象
/*API*/BOOL			WINAPI	GameMgr_Getplotenable/*游戏_返回剧情是否有效*/();//剧情有效之后才会引发onscene_idleover,onscene_moveover,onscene_next,onscene_finish
/*API*/VOID			WINAPI	GameMgr_Setplotenable/*游戏_设置剧情是否有效*/(BOOL bEnable/*设置*/);//剧情有效之后才会引发onscene_idleover,onscene_moveover,onscene_next,onscene_finish
/*API*/VOID			WINAPI	GameMgr_plotnext/*游戏_进入下一个剧情脚本*/();//调用onscene_plot
/*API*/VOID			WINAPI	GameMgr_plotto/*游戏_进入下一个剧情脚本*/(int id/*剧情id*/);//调用onscene_plot
/*API*/INT			WINAPI	GameMgr_plotid/*游戏_当前剧情脚本ID*/();//返回当前剧情脚本
/*API*/VOID			WINAPI	GameMgr_plotfinish/*游戏_结束剧情系统*/();//调用onscene_plotfinish
/*API*/LPCSTR		WINAPI	GameMgr_loadtextby/*游戏_返回文本表中的文本*/(LPCSTR szName/*名字*/, LPCSTR szCol/*列*/);//返回文本表中的文本,text_list.txt
/*API*/LPCSTR		WINAPI	GameMgr_loadtext/*游戏_返回文本表中的文本根据ID*/(INT nID/*ID*/, LPCSTR szCol/*列*/);//返回文本表中的文本,text_list.txt
/*API*/VOID			WINAPI	GameMgr_resetAll/*游戏_重置所有角色的位置信息血条，状态等*/();//重置所有角色的位置信息血条（除了主角），状态等，适用于一段剧情结束之后
//时间函数列表
/*API*/INT			WINAPI	GameMgr_timeout/*游戏_定时引发*/(IGameObjectPtr* obj/*对象*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/);//定时引发，只引发一次，毫秒记,脚本无参数,一个函数可以创建多个实例,事件[szLuaFunc(obj)--(obj=GameObject)定期事件,只引发一次];返回TimeID
/*API*/INT			WINAPI	GameMgr_interval/*游戏_间隔引发*/(IGameObjectPtr* obj/*对象*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用次数*/);//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(obj,tm,endof,maxtm)--(obj=GameObject,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
/*API*/INT			WINAPI	GameMgr_framemove/*游戏_循环引发*/(IGameObjectPtr* obj/*对象*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/);//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(obj,tm,endof,maxtm)--(obj=GameObject,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID

/*API*/float		WINAPI	GameMgr_getheight_3d/*游戏_根据碰撞子模型取高度*/(const float3* pos/*位置*/);//在3dsmax导出时已collision_开头的子模型都属于碰撞模型
/*API*/VOID			WINAPI	GameMgr_ray3d/*游戏_碰撞检测_仅与碰撞子模型*/(const Pixel* pixel/*屏幕坐标位置*/, IMap3DItemPtr* _return/*返回*/);//collision_开头的子模型都属于碰撞模型，对这些模型进行相交运算，如果没有碰撞任何模型则返回空类，要用isEmpty判断
/*API*/VOID			WINAPI	GameMgr_ray3d_dir/*游戏_碰撞检测_仅与碰撞子模型*/(const float3* orig/*原点*/, const float3* dir/*方向*/, IMap3DItemPtr* _return/*返回*/);//collision_开头的子模型都属于碰撞模型，对这些模型进行相交运算，如果没有碰撞任何模型则返回空类，要用isEmpty判断
/*API*/VOID			WINAPI	GameMgr_ray3d_dir2/*游戏_碰撞检测_仅与碰撞子模型限制距离*/(const float3* orig/*原点*/, const float3* dir/*方向*/, float fMaxLength/*计算最大距离*/, IMap3DItemPtr* _return/*返回*/);//collision_开头的子模型都属于碰撞模型，对这些模型进行相交运算，如果没有碰撞任何模型则返回空类，要用isEmpty判断
/*API*/VOID			WINAPI	GameMgr_ray3d_point/*游戏_返回碰撞检测之后的值*/(float3* _return/*返回*/);//返回上一次碰撞检测之后的碰撞点
/*API*/VOID			WINAPI	GameMgr_ray3d_normal/*游戏_返回碰撞检测之后的法线*/(float3* _return/*返回*/);//返回上一次碰撞检测之后的碰撞点
/*API*/float		WINAPI	GameMgr_ray3d_length/*游戏_返回碰撞检测之后的距离*/();//返回上一次碰撞检测之后的碰撞点
/*API*/LPCSTR		WINAPI	GameMgr_ray3d_getname/*游戏_返回碰撞检测之后的碰撞模型名字*/();//

//
/*API*/VOID			WINAPI	DataControl_add_child/*数据控制_创建子数据*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项子数据名字*/, IDataControlPtr*_return/*返回*/);//创建文件数据子项
/*API*/VOID			WINAPI	DataControl_clear/*数据控制_清除所有的数据*/(IDataControlPtr* _self/*自身*/);
/*API*/LPCSTR		WINAPI	DataControl_readt/*数据控制_读取一个文本*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项名字*/);
/*API*/double		WINAPI	DataControl_readf/*数据控制_读取一个浮点数*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项名字*/);
/*API*/int			WINAPI	DataControl_readi/*数据控制_读取一个整数*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项名字*/);
/*API*/VOID			WINAPI	DataControl_writet/*数据控制_写入一个文本*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项名字*/, LPCSTR szValue/*值*/);
/*API*/VOID			WINAPI	DataControl_writef/*数据控制_写入一个浮点*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项名字*/, double fValue/*值*/);
/*API*/VOID			WINAPI	DataControl_writei/*数据控制_写入一个整数*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项名字*/, INT nValue/*值*/);
/*API*/INT			WINAPI	DataControl_count/*数据控制_子数据数量*/(IDataControlPtr* _self/*自身*/);
/*API*/VOID			WINAPI	DataControl_at/*数据控制_子数据指针*/(IDataControlPtr* _self/*自身*/, INT index/*配置项子数据名字*/, IDataControlPtr*_return/*返回*/);//创建文件数据子项
/*API*/INT			WINAPI	DataControl_Getlcount/*数据控制_读取数组长度*/(IDataControlPtr* _self/*自身*/);
/*API*/VOID			WINAPI	DataControl_Setlcount/*数据控制_设置数组长度*/(IDataControlPtr* _self/*自身*/, INT nCount/*值*/);
/*API*/LPCSTR		WINAPI	DataControl_lreadt/*数据控制_读取数组文本*/(IDataControlPtr* _self/*自身*/, INT index/*索引*/);
/*API*/double		WINAPI	DataControl_lreadf/*数据控制_读取数组浮点数*/(IDataControlPtr* _self/*自身*/, INT index/*索引*/);
/*API*/int			WINAPI	DataControl_lreadi/*数据控制_读取数组整数*/(IDataControlPtr* _self/*自身*/, INT index/*索引*/);
/*API*/VOID			WINAPI	DataControl_lwritet/*数据控制_写入数组文本*/(IDataControlPtr* _self/*自身*/, INT index/*索引*/, LPCSTR szValue/*值*/);
/*API*/VOID			WINAPI	DataControl_lwritef/*数据控制_写入数组浮点*/(IDataControlPtr* _self/*自身*/, INT index/*索引*/, double fValue/*值*/);
/*API*/VOID			WINAPI	DataControl_lwritei/*数据控制_写入数组整数*/(IDataControlPtr* _self/*自身*/, INT index/*索引*/, INT nValue/*值*/);

/*API*/double		WINAPI	Global_apptime/*全局_当前时间*/();//返回当前全局时间[R:时间控制]
/*API*/float		WINAPI	Global_elapsedtime/*全局_当前更新时间差*/();//返回当前全局更新时间差
/*API*/INT			WINAPI	Global_timeout/*全局_定时引发*/(LPCSTR szLuaFunc/*脚本函数名*/,INT nMillisecond/*毫秒*/);//定时引发，只引发一次，毫秒记,脚本无参数,一个函数可以创建多个实例,事件[szLuaFunc()--定期事件,只引发一次];返回TimeID
/*API*/INT			WINAPI	Global_interval/*全局_间隔引发*/(LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用次数*/);//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(tm,endof,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
/*API*/INT			WINAPI	Global_framemove/*全局_循环引发*/(LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/);//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(tm,endof,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
/*API*/INT			WINAPI	Global_timeout2/*全局_定时引发带一个文本参数*/(LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, LPCSTR param/*文本参数*/);//定时引发，只引发一次，毫秒记,代替以前的timeout_1,脚本第一个参数是传入的参数,文本,一个函数可以创建多个实例,事件[szLuaFunc(param)--传入的自定义参数];返回TimeID
/*API*/INT			WINAPI	Global_interval2/*全局_间隔引发带一个文本参数*/(LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/);//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,代替以前的interval_1,参数为之有所改动,事件[szLuaFunc(tm,endof,param,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID
/*API*/INT			WINAPI	Global_framemove2/*全局_循环引发*/(LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/);//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,第二个参数是传入的参数,文本,一个函数只能创建一个,事件[szLuaFunc(tm,endof,param,maxtm)--tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID
/*API*/VOID			WINAPI	Global_cleartimeby/*全局_关闭任何定时和间隔引发*/(LPCSTR szFunc);//关闭任何定时和间隔引发,相同函数的注册都会全部清除
/*API*/VOID			WINAPI	Global_cleartimeby2/*全局_关闭任何定时和间隔引发*/(LPCSTR szFunc, LPCSTR param/*删除对应参数的*/);//关闭任何定时和间隔引发,替代cleartime_1,如果param=""空字符串则相关的计时器全部被删除
/*API*/VOID			WINAPI	Global_cleartime/*全局_关闭任何定时和间隔引发根据ID*/(INT timeid/*定时器返回ID*/);//关闭任何定时和间隔引发,只清除指定id的一个定时器,返回自timeout或者interval等函数
/*API*/VOID			WINAPI	Global_breaktime/*全局_关闭任何定时和间隔引发*/();//关闭当前的时钟计时器,在interval和timeout等计时器函数内部使用
/*API*/VOID			WINAPI	Global_cleartime_all/*全局_清除所有定时器*/();//清除所有定时器
/*API*/VOID			WINAPI	Global_localtime/*全局_返回当前时间*/(Time* _return/*返回*/);//返回当前时间
/*API*/int			WINAPI	Global_timediff_d/*全局_返回时间差_日*/(Time* t1/*时间1*/, Time* t2/*时间2*/);//返回当前时间
/*API*/int			WINAPI	Global_timediff_h/*全局_返回时间差_时*/(Time* t1/*时间1*/, Time* t2/*时间2*/);//返回当前时间
/*API*/int			WINAPI	Global_timediff_m/*全局_返回时间差_分*/(Time* t1/*时间1*/, Time* t2/*时间2*/);//返回当前时间
/*API*/int			WINAPI	Global_timediff_s/*全局_返回时间差_秒*/(Time* t1/*时间1*/, Time* t2/*时间2*/);//返回当前时间
/*API*/VOID			WINAPI	Global_pause/*全局_暂停整个游戏*/(BOOL bPause/*是否暂停*/);//暂停整个游戏,所有的物体将停止动作
/*API*/VOID			WINAPI	Global_speed/*全局_单个时间的加速*/(float fSpeed/*速度*/);//设置整个过程的速度,如果速度0则和暂停一样的效果

/*API*/VOID			WINAPI	Global_mousepoint/*全局_返回鼠标或者手指的位置*/(INT nIndex/*手指索引*/, Pixel* _return/*返回*/);//[R:系统相关]手机全局中可以很多个手指位置同时获取
/*API*/VOID			WINAPI	Global_mousepoint2d/*全局_返回2D鼠标或者手指的位置*/(INT nIndex/*手指索引*/, Pixel* _return/*返回*/);//手机全局中可以很多个手指位置同时获取，点选对话框之类的时候有用
/*API*/INT			WINAPI	Global_screenwidth/*全局_取得窗体宽度*/();
/*API*/INT			WINAPI	Global_screenheight/*全局_取得窗体高度*/();
/*API*/INT			WINAPI	Global_gamewidth/*全局_取得游戏需要宽度*/();
/*API*/INT			WINAPI	Global_gameheight/*全局_取得游戏需要高度*/();
/*API*/VOID			WINAPI	Global_exit/*全局_调用退出游戏函数*/();
/*API*/LPCSTR		WINAPI	Global_alert/*全局_消息框*/(LPCSTR szMessage, LPCSTR szTitle, LPCSTR szType);//弹出系统消息框，szTitle是标题，szType是类型比如(ok,yesno,okcancel,yesnocancel)，返回被按下的按钮类型
/*API*/void			WINAPI	Global_msgbox/*全局_弹出游戏信息框*/(LPCSTR szTitle, LPCSTR szMsg, LPCSTR szFlags, LPCSTR script);
/*API*/LPCSTR		WINAPI	Global_lan/*全局_读取文本*/(LPCSTR szName/*指针*/);
/*API*/VOID			WINAPI	Global_setlanguage/*全局_设置地区语言所在目录*/(LPCSTR szlanPath/*函数*/);//cn=dlgs/cn/...rpgui,language/cn.txt等
/*API*/LPCSTR		WINAPI	Global_language_image/*全局_返回当前语言对应的路径名*/(LPCSTR szFileName/*名字*/);
/*API*/VOID			WINAPI	Global_bind_input/*全局_全局绑定*/(LPCSTR szEvent/*事件名*/, LPCSTR szFunc/*函数*/);//szEvent="down,up,move,onsize", 参数是，Pixel,事件[szFunc(pos)--(pos=Pixel)pos是Pixel类型参数，在onsize中增加两个参数,begin和fsize，begin是0表示开始，1表示持续，fsize是当前放大倍率，这两个参数需要自己添加onsize(pos,begin,size)]
/*API*/BOOL			WINAPI	Global_infile/*全局_是否在某个脚本中运行*/(LPCSTR szFile);//是否在某个文件中，比如infile("main");是否在sc/main.lua文件中运行
/*API*/BOOL			WINAPI	Global_insystem/*全局_是否在某个系统中*/(LPCSTR szSystem);//是否在某个系统中，参数"windows","ios","android"
/*API*/VOID			WINAPI	Global_goto/*全局_进入下一个脚本*/(LPCSTR szLuaFile);
/*API*/VOID			WINAPI	Global_sendmessage/*全局_向核心底层发送消息*/(LPCSTR szName/*消息名称*/, LPCSTR szParams/*消息参数1*/, LPCSTR szParams2/*消息参数2*/);//向代码核心层发送消息,代码核心层使用onmessage来处理消息,其中buy和restore默认是用来购买和恢复购买的函数
/*API*/VOID			WINAPI	Global_ad_show/*全局_显示广告条*/(INT nType/*类型*/, BOOL bShow/*显示*/);//nType 0=banner,1=插屏,2=积分墙
/*API*/VOID			WINAPI	Global_ad_setpos/*全局_设置显示位置*/(INT x/*x*/, INT y/*y*/);//banner的位置
/*API*/VOID			WINAPI	Global_openurl/*全局_打开浏览器*/(LPCSTR url);
/*API*/VOID			WINAPI	Global_gotocomment/*全局_进入评论页面*/();
/*API*/VOID			WINAPI	Global_openrank/*全局_显示排行榜*/(LPCSTR rank);//显示排行榜，设置排行榜名字，IOS开启gamecenter才会有效，android没有这个功能
/*API*/VOID			WINAPI	Global_rankscore/*全局_设置排行榜值*/(LPCSTR rank, int nValue);//设置排行榜积分，IOS开启gamecenter才会有效，android没有这个功能
/*API*/VOID			WINAPI	Global_achievement/*全局_设置取得成就*/(LPCSTR achievement, int percent);//设置某个成就成功取得和其值，IOS开启gamecenter才会有效，android没有这个功能

/*API*/VOID			WINAPI	Global_worldmat/*全局_取世界矩阵*/(matrix4x4* _return/*ret*/);//[R:计算相关]
/*API*/VOID			WINAPI	Global_viewmat/*全局_取观察矩阵*/(matrix4x4* _return/*ret*/);
/*API*/VOID			WINAPI	Global_projmat/*全局_取投影矩阵*/(matrix4x4* _return/*ret*/);
/*API*/int			WINAPI	Global_round/*全局_四舍五入*/(float d);//四舍五入
/*API*/int			WINAPI	Global_abs/*全局_取正值*/(double n);//取得正值
/*API*/int			WINAPI	Global_symbol/*全局_取符号*/(double n);//取得数字正负符号
/*API*/int			WINAPI	Global_atoi/*全局_文本到数字*/(LPCSTR v);//文本到数字
/*API*/BOOL			WINAPI	Global_isstr/*全局_A文本中是否存在这段B文本*/(LPCSTR a, LPCSTR b);//比如a=abc.efg,b=efg,则返回true,也就是a文本中存在efg这三个连续出现的文本
/*API*/float		WINAPI	Global_rand/*全局_取得随机数*/(float fmin,float fmax);//两者中取得随机数
/*API*/int			WINAPI	Global_int/*全局_浮点到整数*/(double d);//浮点数到整数，非四舍五入,0.1=0,1.9=1,2.0=2,2.1=2,...
/*API*/BOOL			WINAPI	Global_point_in/*全局_判断点是否在矩形内*/(const Rect* rect/*矩形*/, const Pixel* point/*点*/);
/*API*/float		WINAPI	Global_tovalue/*全局_到坐标*/(float meter/*米*/);//从米计算到坐标(返回 meter * 10);
/*API*/float		WINAPI	Global_tometer/*全局_到米*/(float v/*坐标*/);//从坐标计算到米(返回 v * 0.1)
/*API*/INT			WINAPI	Global_rgba/*全局_rgba*/(INT r, INT g, INT b, INT a);//0~255,r=红，g=绿，b=蓝
/*API*/INT			WINAPI	Global_rgbaf/*全局_rgba_F*/(float r, float g, float b, float a);//0~1,r=红，g=绿，b=蓝，a=透明值
/*API*/float		WINAPI	Global_clamp_angle/*全局_弧度限制*/(float fDegress);
/*API*/float		WINAPI	Global_clamp/*全局_值限制*/(float fValue/*值*/, float fMin/*最小*/, float fMax/*最大*/);//设置值在某个范围内

/*API*/VOID			WINAPI	Global_dlg_create/*全局_创建空对话框*/(IDialogPtr* _return/*返回*/);//[R:2D资源相关]
/*API*/VOID			WINAPI	Global_dlg_delete/*全局_删除对话框*/(IDialogPtr* ptr/*对话框*/);
/*API*/VOID			WINAPI	Global_dlg_load/*全局_载入对话框仅一次*/(LPCSTR szFileName/*文件名*/, IDialogPtr* _return/*返回*/);//如果之前已经载入过则不再载入
/*API*/VOID			WINAPI	Global_dlg_loadnew/*全局_载入新的对话框*/(LPCSTR szFileName/*文件名*/, IDialogPtr* _return/*返回*/);
/*API*/VOID			WINAPI	Global_dlg_loadAndProgress/*游戏_载入对话框仅一次使用进度*/(LPCSTR szFileName/*文件名*/, IDialogPtr* _return/*返回*/);//如果之前已经载入过则不再载入，并使用进度条读取内部的图片文件，首先需要使用gamemgr.create函数设置好进度条脚本
/*API*/VOID			WINAPI	Global_dlg_search/*全局_搜索对话框*/(LPCSTR szDlgName/*对话框名字*/, IDialogPtr* _return/*返回*/);
/*API*/VOID			WINAPI	Global_dlg_at/*全局_返回索引对话框*/(INT index/*索引值*/, IDialogPtr* _return/*返回*/);
/*API*/INT			WINAPI	Global_dlg_count/*全局_返回对话框个数*/();
/*API*/VOID			WINAPI	Global_draw_create/*全局_穿件自定义渲染类*/(IUIDrawMgrPtr* _return/*接口*/);
/*API*/VOID			WINAPI	Global_findway_create/*全局_创建寻路类*/(IFindWayPtr* _return/*接口*/);
/*API*/VOID			WINAPI	Global_findway_delete/*全局_删除寻路类*/(IFindWayPtr* ptr/*接口*/);

/*API*/VOID			WINAPI	Global_table_load/*全局_表格数据_载入*/(LPCSTR szFile/*表格文件*/, ITableReaderPtr* _return/*返回指针*/);//[R:资源相关]
/*API*/VOID			WINAPI	Global_table_delete/*全局_表格数据_释放*/(ITableReaderPtr* ptr/*指针*/);
/*API*/VOID			WINAPI	Global_texture_load/*全局_纹理_载入*/(LPCSTR szFile/*文件名*/, ITexturePtr* _return/*纹理ID*/);//
/*API*/VOID			WINAPI	Global_texture_delete/*全局_纹理_释放*/(ITexturePtr* ptr/*纹理ID*/);
/*API*/VOID			WINAPI	Global_mesh_load/*全局_模型_载入*/(LPCSTR szFile/*rpgmesh文件路径*/, BOOL bAutoRender/*是否自动添加到渲染队列*/, IMeshPtr* _return/*取渲染组ID*/);//载入并分配段内存
/*API*/VOID			WINAPI	Global_mesh_delete/*全局_模型_释放*/(IMeshPtr* mesh/*渲染组ID*/);
/*API*/VOID			WINAPI	Global_map3d_load/*全局_地图3d_载入*/(LPCSTR szName/*文件名字*/, IMap3DPtr* _return/*取渲染组ID*/);//载入并分配段内存
/*API*/VOID			WINAPI	Global_map3d_delete/*全局_地图3d_释放*/(IMap3DPtr* mesh/*渲染组ID*/);
/*API*/int			WINAPI	Global_text_create/*全局_文本创建*/(LPCSTR szText/*文本*/, int x/*左*/, int y/*上*/);//返回文本ID，文本最长不能超过250个字节,文本必须使用utf8("中文")才能显示中文,文本实时渲染效率低，请在测试时使用，在UIControl控件中固定的文本渲染效率是很快的
/*API*/int			WINAPI	Global_text_create2/*全局_文本创建2*/(LPCSTR szText/*文本*/, int x/*左*/, int y/*上*/, const color4* c/*颜色*/, int size/*文字大小*/);//返回文本ID，文本最长不能超过250个字节,文本必须使用utf8("中文")才能显示中文，请在测试时使用，在UIControl控件中固定的文本渲染效率是很快的
/*API*/VOID			WINAPI	Global_text_update/*全局_文本创建*/(int nID, LPCSTR szText/*文本*/);//刷新文本内容,文本必须使用utf8("中文")才能显示中文，请在测试时使用，在UIControl控件中固定的文本渲染效率是很快的
/*API*/VOID			WINAPI	Global_text_delete/*全局_文本删除*/(int nID);//根据ID删除文本，从text_create中返回
/*API*/BOOL			WINAPI	Global_include/*全局_载入脚本文件*/(LPCSTR szFile/*脚本文件*/);//动态载入脚本文件到脚本系统，默认要放到sc目录下面，比如sc/base.lua则参数需要设置为base.lua,而且脚本文件必须得是utf8格式的

/*API*/VOID			WINAPI	Global_config/*全局_返回配置文件的类*/(LPCSTR name/*配置项子数据名字*/, IDataControlPtr*_return/*返回*/);//[R:IO相关]创建文件数据子项
/*API*/BOOL			WINAPI	Global_isconfig/*全局_从配置文件查找变量是否存在*/(LPCSTR name/*配置项名字*/);//从配置文件查找变量是否存在
/*API*/LPCSTR		WINAPI	Global_readt/*全局_从配置文件读取一个文本*/(LPCSTR name/*配置项名字*/);
/*API*/double		WINAPI	Global_readf/*全局_从配置文件读取一个浮点数*/(LPCSTR name/*配置项名字*/);
/*API*/int			WINAPI	Global_readi/*全局_从配置文件读取一个整数*/(LPCSTR name/*配置项名字*/);
/*API*/VOID			WINAPI	Global_writet/*全局_写入一个文本到配置文件*/(LPCSTR name/*配置项名字*/, LPCSTR szValue/*值*/);
/*API*/VOID			WINAPI	Global_writef/*全局_写入一个浮点到配置文件*/(LPCSTR name/*配置项名字*/, double fValue/*值*/);
/*API*/VOID			WINAPI	Global_writei/*全局_写入一个整数到配置文件*/(LPCSTR name/*配置项名字*/, INT nValue/*值*/);
/*API*/VOID			WINAPI	Global_configfile/*全局_设置配置文件名字*/(LPCSTR name/*配置文件名字*/);
/*API*/VOID			WINAPI	Global_configsave/*全局_设置配置数据是否自动保存*/(BOOL b/*自动保存*/);
//
/*API*/VOID			WINAPI	Global_valdata/*全局_返回内存变量的类*/(LPCSTR name/*配置项子数据名字*/, IDataControlPtr*_return/*返回*/);//[R:内存数据脚本之间共享相关]创建文件数据子项
/*API*/BOOL			WINAPI	Global_isval/*全局_从内存查找变量是否存在*/(LPCSTR name/*配置项名字*/);//从内存查找变量是否存在
/*API*/LPCSTR		WINAPI	Global_getvalt/*全局_从内存读取一个文本*/(LPCSTR name/*配置项名字*/);
/*API*/double		WINAPI	Global_getvalf/*全局_从内存读取一个浮点数*/(LPCSTR name/*配置项名字*/);
/*API*/int			WINAPI	Global_getvali/*全局_从内存读取一个整数*/(LPCSTR name/*配置项名字*/);
/*API*/VOID			WINAPI	Global_setvalt/*全局_写入一个文本到内存*/(LPCSTR name/*配置项名字*/, LPCSTR szValue/*值*/);
/*API*/VOID			WINAPI	Global_setvalf/*全局_写入一个浮点到内存*/(LPCSTR name/*配置项名字*/, double fValue/*值*/);
/*API*/VOID			WINAPI	Global_setvali/*全局_写入一个整数到内存*/(LPCSTR name/*配置项名字*/, INT nValue/*值*/);

/*API*/INT			WINAPI	Global_playwave/*全局_播放*/(LPCSTR szName/*WAV文件*/);//必须得是名字比如beep会自动选择sound\\beep.wav[R:声音相关]
/*API*/INT			WINAPI	Global_playmp3/*全局_播放*/(LPCSTR szFile/*WAV_MP3文件*/);//必须得是名字比如beep会自动选择sound\\beep.mp3，只播放一次
/*API*/INT			WINAPI	Global_playbgm/*全局_循环播放背景音乐*/(LPCSTR szFile/*WAV_MP3文件*/);//必须得是名字比如beep会自动选择sound\\beep.mp3,使用stopsnd来停止播放音乐
/*API*/INT			WINAPI	Global_playbgm2/*全局_循环播放背景音乐2*/(LPCSTR szFile/*WAV_MP3文件*/, float fVolume);//必须得是名字比如beep会自动选择sound\\beep.mp3,使用stopsnd来停止播放音乐
/*API*/INT			WINAPI	Global_playmp3_2/*全局_播放*/(LPCSTR szFile/*WAV_MP3文件*/, INT nLoop/*循环次数*/);//必须得是名字比如beep会自动选择sound\\beep.mp3
/*API*/INT			WINAPI	Global_playsnd/*全局_播放2*/(LPCSTR szFile/*WAV_MP3文件*/, float fVolume/*音量*/, float fPan/*声道*/, float fRate/*频率*/);//可以直接设置名字，比如sound\\beep.wav，音量0~1，声道-1~1,频率1正常
/*API*/VOID			WINAPI	Global_stopsnd/*全局_停止音乐*/(INT s2d/*音效2D*/);
/*API*/VOID			WINAPI	Global_stopsndall/*全局_停止所有音乐*/();
/*API*/VOID			WINAPI	Global_setsndvolume/*全局_设置音量*/(INT snd/*音效2D*/, float fVolume/*音量*/);//音量值是0到1之间，取false表示此声音已经过期无效,声道值是-1到1之间,-1左声道，1右声道，取false表示此声音已经过期无效
/*API*/VOID			WINAPI	Global_setsndvolumepan/*全局_设置音量声道*/(INT s2d/*音效2D*/, float fVolume/*音量*/, float fPan);//音量值是0到1之间，取false表示此声音已经过期无效,声道值是-1到1之间,-1左声道，1右声道，取false表示此声音已经过期无效
/*API*/VOID			WINAPI	Global_setsndrate/*全局_设置频率*/(INT snd/*音效2D*/, float fRate/*速度*/);//音量值是0到1之间，播放速度
/*API*/VOID			WINAPI	Global_setupwave/*全局_设置音效属性*/(LPCSTR szName/*WAV文件*/, INT nDuplicateCount/*拷贝数量*/);//设置wave文件重复创建数量,默认是10
//
/*API*/VOID			WINAPI	Global_draw_bind/*全局_绑定渲染函数*/(LPCSTR szLuaFunc/*渲染函数*/);//[R:渲染相关]注册渲染函数，只能注册一次，如果要去掉设置空文本就可以了,事件[szLuaFunc(elapsed)--每次渲染循环函数中被调用];返回TimeID
/*API*/VOID			WINAPI	Global_draw2d_rect/*全局_渲染矩形*/(const Rect* _rect/*矩形*/, const color4* c/*颜色*/, BOOL bFill/*是否填充内部*/);//仅在Global_draw_bind函数的回调函数中调用有效
/*API*/VOID			WINAPI	Global_draw2d_line/*全局_渲染2D线条*/(const Pixel* p1/*屏幕坐标1*/, const Pixel* p2/*屏幕坐标2*/, const color4* c/*颜色*/);//仅在Global_draw_bind函数的回调函数中调用有效
/*API*/VOID			WINAPI	Global_draw2d_linec/*全局_渲染一个带颜色线条*/(const PixelColor* p1/*屏幕坐标1*/, const PixelColor* p2/*屏幕坐标2*/);//仅在Global_draw_bind函数的回调函数中调用有效
/*API*/VOID			WINAPI	Global_draw2d_text/*全局_渲染文本*/(Rect* _rect/*屏幕区域*/, LPCSTR szText/*文本*/, const color4* c/*颜色*/);//仅在Global_draw_bind函数的回调函数中调用有效
/*API*/VOID			WINAPI	Global_draw2d_texture/*全局_渲染纹理*/(ITexturePtr* texture/*纹理*/, const Rect* _rect/*屏幕区域*/, const Rect* _textureRect/*纹理区域_默认0*/, const color4* c/*颜色_默认负1*/);//仅在Global_draw_bind函数的回调函数中调用有效
/*API*/VOID			WINAPI	Global_draw2d_triangle/*全局_渲染屏幕中的三角形*/(ITexturePtr* texture/*纹理*/, const PixelUV* p1/*点1*/, const PixelUV* p2/*点2*/, const PixelUV* p3/*点3*/, BOOL bFill/*是否填充*/);//仅在Global_draw_bind函数的回调函数中调用有效
/*API*/VOID			WINAPI	Global_draw3d_triangle/*全局_渲染三角形*/(const float3* p1/*点1*/, const float3* p2/*点2*/, const float3* p3/*点3*/, const color4* c/*颜色*/);//仅在Global_draw_bind函数的回调函数中调用有效
/*API*/VOID			WINAPI	Global_draw3d_box/*全局_渲染盒子*/(const AABox* box/*盒子*/, const color4* c/*颜色*/, BOOL bFill/*是否填充*/);//仅在Global_draw_bind函数的回调函数中调用有效
/*API*/VOID			WINAPI	Global_draw3d_line/*全局_渲染3D线条*/(const float3* _from/*开始点*/, const float3* _to/*结束点*/, const color4* c/*颜色*/);//仅在Global_draw_bind函数的回调函数中调用有效

//json操作
/*API*/void			WINAPI	Global_jinit/*全局_初始化json设置http地址*/(LPCSTR szHttpInPhpFile);//绑定一个入口php文件地址，或者其他的asp都可以，只要能返回幻影要求的json数据就可以[R:Ajax+Json网络数据相关]
/*API*/void			WINAPI	Global_jbind/*全局_绑定json返回数据*/(int id, LPCSTR szFunc);//绑定一个lua函数到ajax返回的id响应,其内部可以调用jget*函数获取数据
/*API*/LPCSTR		WINAPI	Global_jget/*全局_读取json文本*/(LPCSTR szName);
/*API*/int			WINAPI	Global_jgeti/*全局_读取json整数*/(LPCSTR szName);
/*API*/float		WINAPI	Global_jgetf/*全局_读取json浮点*/(LPCSTR szName);
/*API*/VOID			WINAPI	Global_jclear/*全局_json记录清空*/(LPCSTR szName, float fValue);
/*API*/VOID			WINAPI	Global_jset/*全局_设置json文本*/(LPCSTR szName, LPCSTR szValue);
/*API*/VOID			WINAPI	Global_jseti/*全局_设置json整数*/(LPCSTR szName, int nValue);
/*API*/VOID			WINAPI	Global_jsetf/*全局_设置json浮点*/(LPCSTR szName, float fValue);
/*API*/VOID			WINAPI	Global_jrequest/*全局_json请求数据*/();//jrequest之后自动清空记录，就是自动调用一次jclear
/*API*/VOID			WINAPI	Global_download/*全局_从网络下载文件*/(LPCSTR httpRequest/*请求信息*/, LPCSTR inDocFile/*文档中的文件相对路径*/);//ondownload在完成下载之后被调用脚本参数是(文件名,状态[ok,fail]);//jrequest之后自动清空记录，就是自动调用一次jclear
//网络模块
/*API*/void			WINAPI	Global_net_init/*全局_net初始化*/(int dwGUID);//打开一个端口作为服务器，并设置服务器名字,所有的触发函数都有一个linkid，第一个参数都是linkid, onmessage第二个参数是消息ID,dwGUID是一个整数，用来识别连接数据，服务器端和客户端必须一致[R:Socket网络数据相关]
/*API*/void			WINAPI	Global_net_onlink/*全局_netOnLink*/(LPCSTR szFunc/*连接时触发*/);//,事件[szFunc(linkid)--(linkid=int)当一个客户端连接时被调用]
/*API*/void			WINAPI	Global_net_ondislink/*全局_netOnDislink*/(LPCSTR szFunc/*断开连接时触发*/);//,事件[szFunc(linkid)--(linkid=int)当一个客户端断开连接时被调用]
/*API*/void			WINAPI	Global_net_onmessage/*全局_netOnMessage*/(LPCSTR szFunc/*消息接收时触发*/);//,事件[szFunc(linkid,msgid)--(linkid=int,msgid=int)当一个客户端发送来消息时被调用]
/*API*/BOOL			WINAPI	Global_net_server/*全局_net启动服务器*/(LPCSTR name, int port);//打开一个端口作为服务器，并设置服务器名字,返回1表示创建成功
/*API*/void			WINAPI	Global_net_search/*全局_net搜索服务器*/(LPCSTR szFunc, int port);//搜索服务器,事件[szFunc(ip)--(ip=string)如果ip是空表示没有搜索到任何服务器]
/*API*/void			WINAPI	Global_net_server_max_links/*全局_net设置服务器最大可连接客户端数量*/(INT nMaxLinks);//数量
/*API*/int			WINAPI	Global_net_connect/*全局_net启动客户端*/(LPCSTR szIP, int port);//启动客户端，并连接指定ip的指定端口，会开启一个线程进行连接，反馈在net_onlink中设置好的回调函数中,返回linkid
/*API*/void			WINAPI	Global_net_close/*全局_net关闭连接*/(INT linkid);//关闭连接
/*API*/void			WINAPI	Global_net_stopall/*全局_net关闭所有服务器和客户端*/();//关闭所有服务器和客户端
/*API*/LPCSTR		WINAPI	Global_net_get/*全局_net读取文本*/();//只有消息回调函数(_onmessage)中才能调用
/*API*/int			WINAPI	Global_net_geti/*全局_net读取整数*/();//只有消息回调函数(_onmessage)中才能调用
/*API*/float		WINAPI	Global_net_getf/*全局_net读取浮点*/();//只有消息回调函数(_onmessage)中才能调用
/*API*/VOID			WINAPI	Global_net_begin/*全局_net清除缓存数据*/();//清除缓存数据之后数据就可以重新写入
/*API*/VOID			WINAPI	Global_net_set/*全局_net写入文本*/(LPCSTR szValue);//任何地方都能调用，但是必须先sclear清除之前的数据
/*API*/VOID			WINAPI	Global_net_seti/*全局_net写入整数*/(int nValue);//任何地方都能调用，但是必须先sclear清除之前的数据
/*API*/VOID			WINAPI	Global_net_setf/*全局_net写入浮点*/(float fValue);//任何地方都能调用，但是必须先sclear清除之前的数据
/*API*/VOID			WINAPI	Global_net_send/*全局_net发送数据*/(INT linkid, short msgid);//发送到某个连接,使用消息ID发送，对方会接收到这个消息ID,消息ID必须大于0
//
/*API*/VOID			WINAPI	Global_random_init/*全局_随机数权重初始化*/(int number);//[R:随机数相关];
/*API*/VOID			WINAPI	Global_random_weight/*全局_随机数设置权重*/(int index, int weight);
/*API*/VOID			WINAPI	Global_random_do/*全局_随机数计算*/();
/*API*/VOID			WINAPI	Global_random_table/*全局_随机数非权重初始化*/(int number);
/*API*/int			WINAPI	Global_random_get/*全局_返回随机数*/(int b);
/*API*/VOID			WINAPI	Global_c_add/*全局_统计计数器加1*/(LPCSTR szName/*计数器*/);//[R:统计相关]
/*API*/VOID			WINAPI	Global_c_set/*全局_统计设置记录*/(LPCSTR szName/*计数器*/, INT nValue/*值*/);
/*API*/INT			WINAPI	Global_c_get/*全局_统计返回记录*/(LPCSTR szName/*计数器*/);

//VOID			WINAPI	System_call/*系统_调用本地函数*/(LPCSTR szName/*方法名字*/, LPCSTR szParameter/*方法参数列表*/);//返回上一次碰撞检测之后的值，给用户的接口
//VOID			WINAPI	System_setevent(LPCSTR eventName/**/, LPCSTR luaMethodName/*lua函数的名字*/);绑定一个事件，参数可以是onframemove,onrender,oncreate,ondestroy,onloadtexture,onloadmesh,onloadmusic,ondeletetexture,ondeletemesh,ondeletemusic等等得一个一个全部列举出来，还有参数的类型也得列举出来
//
//CALLBACK VOID	WINAPI	ON_selchanged/*系统_返回上一次碰撞检测之后的值*/(raydata* _return/*返回*/);//返回上一次碰撞检测之后的值		EventID_Click			/*界面事件_左键点击*/=	1,
//		EventID_Changed		/*界面事件_复选框改变*/=	11,
//		EventID_Changed		/*界面事件_单选框改变*/=	4,
//		EventID_Changed			/*界面事件_Slider值改变*/=	5,
//		EventID_DoubleClick			/*界面事件_列表框双击*/=	8,
//		EventID_Changed		/*界面事件_列表框选择*/=	9,
//		EventID_PlayOver		/*界面事件_动画播放完毕*/=	20,
//		EventID_DoubleClick			/*界面事件_左键双击*/=	24,
//		EventID_UVPlayOver		/*界面事件_UV动画播放完毕*/=	27,
//		EventID_PlayOver				/*播放事件_声音播放完毕*/=	61,		//
//		//
//		EventID_NetClientConnect			/*网络客户端_服务器连接成功*/ = 73,
//		EventID_NetClientRecv				/*网络客户端_服务器消息*/ = 74,
//		EventID_NetClientDisconnect		/*网络客户端_服务器断开*/ = 75,
//

//需要全部重新命名名字了
//生成类似xml的帮助文档，转换成word并可以输出成chm文档和html网页的，网页的话直接更新到幻影主页中的帮助系统中，使用wiki模式
//添加setTimeout和setInterval函数

};
using namespace Phantom;

#endif
