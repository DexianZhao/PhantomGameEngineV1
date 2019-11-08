#pragma once
#include "MoveAxis.h"

class CScaleAxis: public AxisOperation
{
public:
	CScaleAxis(void);
	~CScaleAxis(void);
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
	Phantom::float3					m_newScale, m_downScale;
	//
	SPlane					m_triangleX[2];
	SPlane					m_triangleY[2];
	SPlane					m_triangleZ[2];
	//
	SPlane					m_planeXY;
	SPlane					m_planeYZ;
	SPlane					m_planeZX;
	//
	POINT					m_downPoint;
	BOOL					m_bAxisLButtonDown;
	//
	INT						m_selectAxisShow;
	INT						m_selectAxis;
};
