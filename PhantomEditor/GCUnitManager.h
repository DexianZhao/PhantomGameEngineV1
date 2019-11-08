//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   GCSelectEditor.h
	软件名称   :   幻影游戏引擎

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	
	Copyright 2009-2016 Zhao Dexian
	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___GCSELECTEDITORBASE__H___
#define ___GCSELECTEDITORBASE__H___

#pragma once
#include <PhantomProject.h>
//
//class GCUnitEditor
//{
//public:
//	GCUnitEditor();
//	virtual	~GCUnitEditor();
//	//
//	const PRect&					GetSaveRect(){return m_saveRect;}
//	virtual	GCUnitManager*			GetUnitManager()	=	0;	//返回对象管理器
//	//
//public://重写函数
//	virtual	VOID					SaveRect()		=	0;		//保存对象位置
//	virtual	const PRect*			GetUnitRect()	=	0;		//返回对象矩阵
//	virtual	VOID					SetUnitRect(const PRect& rc)	=	0;
//	virtual	PPixel					WindowToUnit(const PPixel& p)	=	0;	//从窗口坐标转换到对象坐标
//	virtual	PPixel					UnitToWindow(const PPixel& p)	=	0;	//从对象坐标转换到窗口坐标
//	virtual	BOOL					IsRotation()				=	0;		//是否支持转动
//	virtual	VOID					SetRotation(BOOL b)			=	0;		//设置转动支持
//	virtual	float					GetRotationAngle()			=	0;
//	virtual	VOID					SetRotationAngle(float f)	=	0;
//public:
//	PRect							m_saveRect;
//};
//
//class GCSelectEditor
//{
//public:
//	GCSelectEditor(void);
//	virtual ~GCSelectEditor(void);
//	//
//	enum PickType
//	{
//		PickType_Body			=	1,
//		PickType_MoveSide,
//		PickType_Unit2DCenter,
//	};
//	//
//	enum MouseMode{
//		MouseMode_None	=	0,
//		MouseMode_InsertControl,
//		MouseMode_MoveControl,
//		MouseMode_TabIndex,
//		MouseMode_SelectRect,
//		MouseMode_SizeControl,
//		MouseMode_MoveCopy,
//		MouseMode_Unit2DCenterPos,
//		MouseMode_SetFloor,
//		MouseMode_InsertUnit2D,
//	};
//	//
//	enum ToAlign{
//		ToAlign_None	=	0,
//		ToAlign_Left	=	0x01,
//		ToAlign_Right	=	0x02,
//		ToAlign_Top		=	0x04,
//		ToAlign_Bottom	=	0x08,
//	};
//	enum MovePointSide{
//		MovePointSide_Left	=	0,
//		MovePointSide_Right,
//		MovePointSide_Top,
//		MovePointSide_Bottom,
//		MovePointSide_LeftTop,
//		MovePointSide_LeftBottom,
//		MovePointSide_RightTop,
//		MovePointSide_RightBottom,
//		MovePointSide_Center,
//		MovePointSide_Rotation,
//		MovePointSide_Count,
//	};
//	//
//	struct	PickInfo
//	{
//		int			nMovePointSide;
//		int			nPickType;
//	};
//	//
//	//
//	virtual	GCUnitEditor*			PickInSelect(const PPixel& pos, PickInfo& ret);	//在已经选择的列表中选择一个
//	virtual	BOOL					IsPickIsSelect(const PPixel& pos, GCUnitEditor* c, PickInfo& ret);
//	//
//	virtual	GCUnitEditor*			PickNew(const PPixel& pos);
//	//
//	virtual	VOID					GetRectPoints(const PRect& rc, GCUnitEditor* c, PRect* rets);
//	//
//	VOID							ClearSelect();
//	VOID							AddSelect(GCUnitEditor* ed);
//public://管理器重写
//	virtual	GCUnitEditor*			CloneUnit(GCUnitEditor* ed);
//	virtual	GCUnitEditor*			GetUnitPtr(VOID* ptr);	//自动创建一个对象
//	virtual	GCUnitEditor*			GetUnitPtr(INT index);
//	virtual	INT						GetUnitCount();			//返回对象数量
//protected:
//	GCUnitEditor*					m_moveUnitPtr;			//移动时选择的对象
//	PPixel							m_downPoint;
//	PPixel							m_lastPoint;
//	//
//public:
//};



#endif
