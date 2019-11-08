//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   MoveAxis.h
	软件名称   :   幻影游戏引擎（www.aixspace.com）

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	Copyright 2009-2016 Zhao Dexian
	
	
	
	
	

	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___MOVEAXIS__H___
#define ___MOVEAXIS__H___


#pragma once
#include <PhantomManager.h>
#include <windows.h>

enum TRANS_MODE{
	TRANS_MODE_WORLD	=	0,//世界坐标对其
	TRANS_MODE_LOCAL	=	1,//模型自身坐标对齐
};

enum{
	Axis_X		=	0x01,
	Axis_Y		=	0x02,
	Axis_Z		=	0x04,
};

struct STriangle
{
	Phantom::float3		v0,v1,v2;
};

struct SPlane
{
	Phantom::float3		v0,v1,v2,v3;
};
enum AxisOperatorType
{
	AxisOT_Move	=	1,
	AxisOT_Scale,
	AxisOT_Rotation,
};
__interface	IAxisEditCallback
{
	BOOL					BeginEditor(Phantom::matrix4x4& matOut, AxisOperatorType ot);
	VOID					OnEditor(const Phantom::matrix4x4& downMat, const Phantom::matrix4x4& newMat, AxisOperatorType ot, const Phantom::float3& newValOffset);
	VOID					EndEditor(const Phantom::matrix4x4& downMat, AxisOperatorType ot, const Phantom::float3& newValOffset);
	BOOL					PickTerrain(int x, int y, Phantom::float3& ret);
};

VOID						SetTransMode(TRANS_MODE mode);	//坐标对齐转变模式，Local
TRANS_MODE					GetTransMode();

class AxisOperation
{
public:
	AxisOperation(){m_axisCallback =	0;m_viewWorld.identity();m_editorWorld.identity();}
	virtual ~AxisOperation(){}
	virtual	VOID			DrawAxis(float fSize = 1.0f){}
	virtual	BOOL			OnLButtonDown(POINT p){return FALSE;}
	virtual	BOOL			OnLButtonUp(POINT p){return FALSE;}
	virtual	BOOL			OnMouseMove(POINT p, BOOL bPick = false){return FALSE;}
public:
	Phantom::matrix4x4				m_viewWorld;
	Phantom::matrix4x4				m_editorWorld;
	IAxisEditCallback*		m_axisCallback;
};

class CMoveAxis: public AxisOperation
{
public:
	CMoveAxis(void);
	~CMoveAxis(void);
public:
	virtual	VOID			DrawAxis(float fSize = 1.0f);
	virtual	VOID			DrawAxis(const Phantom::float3& pos, float fSize, float fColor = 1.0f);
	virtual	BOOL			OnLButtonDown(POINT p);
	virtual	BOOL			OnLButtonUp(POINT p);
	virtual	BOOL			OnMouseMove(POINT p, BOOL bPick = false);
	//
	INT						GetSelectAxis(int x, int y);
	//
	Phantom::float3					m_newPosition, m_downPosition;
	Phantom::float3					m_axisOrig, m_axisDirX, m_axisDirY, m_axisDirZ;
	Phantom::float2					m_axisDirX2D, m_axisDirY2D, m_axisDirZ2D;
	float					m_fCameraLength;
	//
	STriangle				m_triangleX[2];
	STriangle				m_triangleY[2];
	STriangle				m_triangleZ[2];
	//
	SPlane					m_planeXY;
	SPlane					m_planeYZ;
	SPlane					m_planeZX;
	//
	Phantom::float3					m_downOffset[3];
	//
	POINT					m_moveBegin;
	POINT					m_downPoint;
	BOOL					m_bAxisLButtonDown;
	//
	INT						m_selectAxisShow;
	INT						m_selectAxis;
};

BOOL				PointLineDistance(const POINT& p0, const POINT& line0, const POINT& line1, Phantom::float2* pOutputCross, float* pOutputDistance);


#endif
