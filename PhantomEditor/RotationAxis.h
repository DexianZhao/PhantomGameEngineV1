//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   RotationAxis.h
	软件名称   :   幻影游戏引擎（www.aixspace.com）

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	Copyright 2009-2016 Zhao Dexian
	
	
	
	
	

	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___ROTATIONAXIS__H___
#define ___ROTATIONAXIS__H___


#pragma once

#include "MoveAxis.h"

#define	AXIS_POINT			64

class GCSceneEditor3D;

class CRotationAxis: public AxisOperation
{
public:
	CRotationAxis(void);
	~CRotationAxis(void);
public:
	virtual	VOID			DrawAxis(float fSize = 1.0f);
	virtual	BOOL			OnLButtonDown(POINT p);
	virtual	BOOL			OnLButtonUp(POINT p);
	virtual	BOOL			OnMouseMove(POINT p, BOOL bPick = true);
	//
	INT						GetSelectAxis(int x, int y);
	//
	Phantom::float3					m_axisOrig, m_axisDirX, m_axisDirY, m_axisDirZ;
	Phantom::float2					m_axisDirX2D, m_axisDirY2D, m_axisDirZ2D;
	float					m_fCameraLength;
	Phantom::float3					m_newRotation, m_downRotation;
	//
	unsigned int			m_axisColorX[AXIS_POINT + 1];
	unsigned int			m_axisColorY[AXIS_POINT + 1];
	unsigned int			m_axisColorZ[AXIS_POINT + 1];
	Phantom::float3			m_axisCircleX[AXIS_POINT + 1];
	Phantom::float3			m_axisCircleY[AXIS_POINT + 1];
	Phantom::float3			m_axisCircleZ[AXIS_POINT + 1];
	//
	GCSceneEditor3D*		m_scene3D;
	POINT					m_downPoint;
	BOOL					m_bAxisLButtonDown;
	//
	INT						m_selectAxisShow;
	INT						m_selectAxis;
};


#endif
