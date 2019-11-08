//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   ScaleAxis.cpp
	软件名称   :   幻影游戏引擎（www.aixspace.com）

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	Copyright 2009-2016 Zhao Dexian
	
	
	
	
	

	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include ".\scaleaxis.h"

CScaleAxis::CScaleAxis(void)
{
	m_bAxisLButtonDown	=	FALSE;
	m_newScale.setxyz(1,1,1);
	m_downScale.setxyz(1,1,1);
}

CScaleAxis::~CScaleAxis(void)
{
}

VOID			CScaleAxis::DrawAxis(float fSize)
{
	if(!this->m_axisCallback)return;
	Phantom::matrix4x4 mat;
	mat.identity();
	Phantom::g_manager.SetWorldMatrix(&mat);
	//
	Phantom::g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
	Phantom::Camera* c	=	Phantom::GetCameraPtr();
	//
	Phantom::float3 pos		=	m_viewWorld.GetPosition();
	Phantom::float3 vDir		=	c->GetEye() - pos;
	float fLength	=	m_fCameraLength	=	vDir.length() * 0.1f * fSize;
	m_axisOrig		=	pos;
	m_axisDirX		=	Phantom::float3(fLength, 0, 0);
	m_axisDirY		=	Phantom::float3(0, fLength, 0);
	m_axisDirZ		=	Phantom::float3(0, 0, fLength);
	//
	Phantom::color4	x,y,z;
	x				=	Phantom::color4(1,0,0,.3f);
	y				=	Phantom::color4(0,1,0,.3f);
	z				=	Phantom::color4(0,0,1,.3f);
	//
	if(m_selectAxisShow & Axis_X)
		x.a			=	1.0f;
	if(m_selectAxisShow & Axis_Y)
		y.a			=	1.0f;
	if(m_selectAxisShow & Axis_Z)
		z.a			=	1.0f;
	Phantom::g_manager.DrawLine( m_axisOrig, m_axisDirX + m_axisOrig, &x);
	Phantom::g_manager.DrawLine( m_axisOrig, m_axisDirY + m_axisOrig, &y);
	Phantom::g_manager.DrawLine( m_axisOrig, m_axisDirZ + m_axisOrig, &z);
	//
	float	fArrow			=	fLength * 0.05f;
	float	fArrowLength	=	fArrow * 3.0f;
	for(int i=0;i<2;i++)
	{
		m_triangleX[i].v0	=	m_triangleX[i].v1	=	m_triangleX[i].v2	=	m_triangleX[i].v3	=	m_axisDirX + m_axisOrig;
		m_triangleY[i].v0	=	m_triangleY[i].v1	=	m_triangleY[i].v2	=	m_triangleY[i].v3	=	m_axisDirY + m_axisOrig;
		m_triangleZ[i].v0	=	m_triangleZ[i].v1	=	m_triangleZ[i].v2	=	m_triangleZ[i].v3	=	m_axisDirZ + m_axisOrig;
	}
	m_triangleX[0].v0.x		=	m_triangleX[0].v3.x	=	m_triangleX[0].v1.x - fArrowLength;
	m_triangleX[1].v0.x		=	m_triangleX[1].v3.x	=	m_triangleX[1].v1.x - fArrowLength;
	m_triangleX[0].v0.y		=	m_triangleX[0].v1.y	=	m_triangleX[0].v1.y + fArrowLength;
	m_triangleX[1].v0.z		=	m_triangleX[1].v1.z =	m_triangleX[1].v1.z + fArrowLength;
	//
	Phantom::g_manager.DrawTriangle( m_triangleX[1].v0, m_triangleX[1].v1, m_triangleX[1].v2, &x);
	Phantom::g_manager.DrawTriangle( m_triangleX[1].v2, m_triangleX[1].v3, m_triangleX[1].v0, &x);
	Phantom::g_manager.DrawTriangle( m_triangleX[0].v0, m_triangleX[0].v1, m_triangleX[0].v2, &x);
	Phantom::g_manager.DrawTriangle( m_triangleX[0].v2, m_triangleX[0].v3, m_triangleX[0].v0, &x);

	m_triangleY[0].v0.y		=	m_triangleY[0].v3.y	=	m_triangleY[0].v1.y - fArrowLength;
	m_triangleY[1].v0.y		=	m_triangleY[1].v3.y	=	m_triangleY[1].v1.y - fArrowLength;
	m_triangleY[0].v0.x		=	m_triangleY[0].v1.x	=	m_triangleY[0].v1.x + fArrowLength;
	m_triangleY[1].v0.z		=	m_triangleY[1].v1.z =	m_triangleY[1].v1.z + fArrowLength;

	Phantom::g_manager.DrawTriangle( m_triangleY[1].v0, m_triangleY[1].v1, m_triangleY[1].v2, &y);
	Phantom::g_manager.DrawTriangle( m_triangleY[1].v2, m_triangleY[1].v3, m_triangleY[1].v0, &y);
	Phantom::g_manager.DrawTriangle( m_triangleY[0].v0, m_triangleY[0].v1, m_triangleY[0].v2, &y);
	Phantom::g_manager.DrawTriangle( m_triangleY[0].v2, m_triangleY[0].v3, m_triangleY[0].v0, &y);

	m_triangleZ[0].v0.z		=	m_triangleZ[0].v3.z	=	m_triangleZ[0].v1.z - fArrowLength;
	m_triangleZ[1].v0.z		=	m_triangleZ[1].v3.z	=	m_triangleZ[1].v1.z - fArrowLength;
	m_triangleZ[0].v0.x		=	m_triangleZ[0].v1.x	=	m_triangleZ[0].v1.x + fArrowLength;
	m_triangleZ[1].v0.y		=	m_triangleZ[1].v1.y =	m_triangleZ[1].v1.y + fArrowLength;
	Phantom::g_manager.DrawTriangle( m_triangleZ[1].v0, m_triangleZ[1].v1, m_triangleZ[1].v2, &z);
	Phantom::g_manager.DrawTriangle( m_triangleZ[1].v2, m_triangleZ[1].v3, m_triangleZ[1].v0, &z);
	Phantom::g_manager.DrawTriangle( m_triangleZ[0].v0, m_triangleZ[0].v1, m_triangleZ[0].v2, &z);
	Phantom::g_manager.DrawTriangle( m_triangleZ[0].v2, m_triangleZ[0].v3, m_triangleZ[0].v0, &z);

	//
	m_planeXY.v0	=	m_planeXY.v1	=	m_planeXY.v2	=	m_planeXY.v3	=	m_axisOrig;
	m_planeYZ.v0	=	m_planeYZ.v1	=	m_planeYZ.v2	=	m_planeYZ.v3	=	m_axisOrig;
	m_planeZX.v0	=	m_planeZX.v1	=	m_planeZX.v2	=	m_planeZX.v3	=	m_axisOrig;
	//
	Phantom::color4 xy(1, 1, 0, 1);
	Phantom::color4 yz(0, 1, 1, 1);
	Phantom::color4 zx(1, 0, 1, 1);
	xy 					*= 0.35f;
	yz 					*= 0.35f;
	zx 					*= 0.35f;
	//
	if((this->m_selectAxisShow & (Axis_X | Axis_Y)) == (Axis_X | Axis_Y))
		xy				+=	0.65f;
	if((this->m_selectAxisShow & (Axis_Z | Axis_Y)) == (Axis_Z | Axis_Y))
		yz				+=	0.65f;
	if((this->m_selectAxisShow & (Axis_X | Axis_Z)) == (Axis_X | Axis_Z))
		zx				+=	0.65f;

	float fBoxLength	=	fLength * 0.3f;
	m_planeXY.v0.x		+=	fBoxLength;
	m_planeXY.v1.x		+=	fBoxLength;
	m_planeXY.v1.y		+=	fBoxLength;
	m_planeXY.v2.y		+=	fBoxLength;

	Phantom::g_manager.DrawTriangle( m_planeXY.v0, m_planeXY.v1, m_planeXY.v2, &xy);
	Phantom::g_manager.DrawTriangle( m_planeXY.v2, m_planeXY.v3, m_planeXY.v0, &xy);
	//
	m_planeYZ.v0.y		+=	fBoxLength;
	m_planeYZ.v1.y		+=	fBoxLength;
	m_planeYZ.v1.z		+=	fBoxLength;
	m_planeYZ.v2.z		+=	fBoxLength;
	Phantom::g_manager.DrawTriangle( m_planeYZ.v0, m_planeYZ.v1, m_planeYZ.v2, &yz);
	Phantom::g_manager.DrawTriangle( m_planeYZ.v2, m_planeYZ.v3, m_planeYZ.v0, &yz);
	//
	m_planeZX.v0.z		+=	fBoxLength;
	m_planeZX.v1.z		+=	fBoxLength;
	m_planeZX.v1.x		+=	fBoxLength;
	m_planeZX.v2.x		+=	fBoxLength;
	Phantom::g_manager.DrawTriangle( m_planeZX.v0, m_planeZX.v1, m_planeZX.v2, &zx);
	Phantom::g_manager.DrawTriangle( m_planeZX.v2, m_planeZX.v3, m_planeZX.v0, &zx);
	//
	float fBoxLine		=	0.5f;
	Phantom::g_manager.DrawLine(m_axisOrig + m_axisDirX * fBoxLine, m_axisOrig + m_axisDirX * fBoxLine + m_axisDirY * fBoxLine, &xy);
	Phantom::g_manager.DrawLine(m_axisOrig + m_axisDirY * fBoxLine, m_axisOrig + m_axisDirX * fBoxLine + m_axisDirY * fBoxLine, &xy);
	//
	Phantom::g_manager.DrawLine(m_axisOrig + m_axisDirZ * fBoxLine, m_axisOrig + m_axisDirZ * fBoxLine + m_axisDirY * fBoxLine, &yz);
	Phantom::g_manager.DrawLine(m_axisOrig + m_axisDirY * fBoxLine, m_axisOrig + m_axisDirZ * fBoxLine + m_axisDirY * fBoxLine, &yz);
	//
	Phantom::g_manager.DrawLine(m_axisOrig + m_axisDirX * fBoxLine, m_axisOrig + m_axisDirX * fBoxLine + m_axisDirZ * fBoxLine, &zx);
	Phantom::g_manager.DrawLine(m_axisOrig + m_axisDirZ * fBoxLine, m_axisOrig + m_axisDirX * fBoxLine + m_axisDirZ * fBoxLine, &zx);
	//
//	Render->SetAlphaMode(ALPHAMODE_DISABLE);
}

#define	CURSOR_HAND		MAKEINTRESOURCE(32649)

BOOL			CScaleAxis::OnLButtonDown(POINT p)
{
	if(!this->m_axisCallback)return FALSE;
	m_downPoint.x	=	p.x;
	m_downPoint.y	=	p.y;
	m_selectAxis	=	GetSelectAxis(p.x, p.y);
	if(m_selectAxis > 0)
	{
		::SetCursor(::LoadCursor(NULL, CURSOR_HAND));
		Phantom::Camera* c		=	Phantom::GetCameraPtr();
		//
		if(m_selectAxis & Axis_X)
		{
			POINT l0, l1;
			if(!c->Get2DFrom3D((Phantom::Pixel&)l0, m_axisOrig))
				return FALSE;
			if(!c->Get2DFrom3D((Phantom::Pixel&)l1, m_axisOrig + m_axisDirX * 0.1f))
				return FALSE;
			m_axisDirX2D.setxy( (float)(l1.x - l0.x), (float)(l1.y - l0.y) );
			m_axisDirX2D	*=	10.0f;
		}
		if(m_selectAxis & Axis_Y)
		{
			POINT l0, l1;
			if(!c->Get2DFrom3D((Phantom::Pixel&)l0, m_axisOrig))
				return FALSE;
			if(!c->Get2DFrom3D((Phantom::Pixel&)l1, m_axisOrig + m_axisDirY * 0.1f))
				return FALSE;
			m_axisDirY2D.setxy( (float)(l1.x - l0.x), (float)(l1.y - l0.y) );
			m_axisDirY2D	*=	10.0f;
		}
		if(m_selectAxis & Axis_Z)
		{
			POINT l0, l1;
			if(!c->Get2DFrom3D((Phantom::Pixel&)l0, m_axisOrig))
				return FALSE;
			if(!c->Get2DFrom3D((Phantom::Pixel&)l1, m_axisOrig + m_axisDirZ * 0.1f))
				return FALSE;
			m_axisDirZ2D.setxy( (float)(l1.x - l0.x), (float)(l1.y - l0.y) );
			m_axisDirZ2D	*=	10.0f;
		}
		if(!this->m_axisCallback->BeginEditor(m_editorWorld, AxisOT_Scale))
			return false;
		m_bAxisLButtonDown	=	TRUE;
		return TRUE;
	}
	return FALSE;
}
extern HWND	GetDeviceWindow();
BOOL			CScaleAxis::OnLButtonUp(POINT p)
{
	if(!this->m_axisCallback)return FALSE;
	RECT rc;
	GetClientRect(GetDeviceWindow(), &rc);
	if(PtInRect(&rc, p) && m_bAxisLButtonDown)
	{
		this->m_axisCallback->EndEditor(m_editorWorld, AxisOT_Scale, m_newScale);
		m_bAxisLButtonDown	=	FALSE;
		return TRUE;
	}
	m_bAxisLButtonDown	=	FALSE;
	return FALSE;
}

BOOL			CScaleAxis::OnMouseMove(POINT p, BOOL bPick)
{
	if(!this->m_axisCallback)return FALSE;
	RECT rc;
	GetClientRect(GetDeviceWindow(), &rc);
	if(PtInRect(&rc, p) && HIBYTE(GetKeyState(VK_LBUTTON)) && m_selectAxis && m_bAxisLButtonDown)
	{
		//
		Phantom::float2 pt((float)(p.x - m_downPoint.x), (float)(p.y - m_downPoint.y));
		//
		Phantom::Camera* c		=	Phantom::GetCameraPtr();
		//
		Phantom::matrix4x4 mat		=	m_editorWorld;
		Phantom::matrix4x4 matS;
		if(m_selectAxis == (Axis_X | Axis_Y | Axis_Z))
		{
			float fMove	= (float)pt.x / 1000.0f * m_fCameraLength * 0.1f;
			m_newScale	=	m_downScale + fMove;
			matrixScaling(&matS, 1.0f + fMove, 1.0f + fMove, 1.0f + fMove);
			matrixMultiply(&mat, &matS, &mat);
		}
		else if(m_selectAxis == (Axis_X | Axis_Y))
		{
			float fMove	= (float)pt.x / 1000.0f * m_fCameraLength * 0.1f;
			m_newScale	=	m_downScale + fMove;
			matrixScaling(&matS, 1.0f + fMove, 1.0f + fMove, 1.0f);
			matrixMultiply(&mat, &matS, &mat);
		}
		else
		{
			if(m_selectAxis & Axis_X)
			{
				float fMove = 0.0f;
				if(fabsf(m_axisDirX2D.x) > fabsf(m_axisDirX2D.y))
					fMove	= m_axisDirX2D.x != 0.0f ? pt.x / m_axisDirX2D.x : 0;
				else
					fMove	= m_axisDirX2D.y != 0.0f ? pt.y / m_axisDirX2D.y : 0;
				fMove	*= m_fCameraLength	/	200.0f;
				m_newScale.x		=	m_downScale.x + fMove;
				matrixScaling(&matS, 1.0f + fMove, 1, 1);
				matrixMultiply(&mat, &matS, &mat);
			}
			if(m_selectAxis & Axis_Y)
			{
				float fMove = 0.0f;
				if(fabsf(m_axisDirY2D.x) > fabsf(m_axisDirY2D.y))
					fMove	= m_axisDirY2D.x != 0.0f ? pt.x / m_axisDirY2D.x : 0;
				else
					fMove	= m_axisDirY2D.y != 0.0f ? pt.y / m_axisDirY2D.y : 0;
				fMove	*= m_fCameraLength	/	200.0f;
				m_newScale.y		=	m_downScale.y + fMove;
				matrixScaling(&matS, 1, 1.0f + fMove, 1);
				matrixMultiply(&mat, &matS, &mat);
			}
			if(m_selectAxis & Axis_Z)
			{
				float fMove = 0.0f;
				if(fabsf(m_axisDirZ2D.x) > fabsf(m_axisDirZ2D.y))
					fMove	= m_axisDirZ2D.x != 0.0f ? pt.x / m_axisDirZ2D.x : 0;
				else
					fMove	= m_axisDirZ2D.y != 0.0f ? pt.y / m_axisDirZ2D.y : 0;
				fMove	*= m_fCameraLength	/	200.0f;
				m_newScale.z		=	m_downScale.z + fMove;
				matrixScaling(&matS, 1, 1, 1.0f + fMove);
				matrixMultiply(&mat, &matS, &mat);
			}
		}
		m_axisCallback->OnEditor(m_editorWorld, mat, AxisOT_Scale, m_newScale);
		::SetCursor(::LoadCursor(NULL, CURSOR_HAND));
		return TRUE;
	}
	else
	{
		int select	=	GetSelectAxis(p.x, p.y);
		if(select)
		{
			::SetCursor(::LoadCursor(NULL, CURSOR_HAND));
		}
	}
	return FALSE;
}

INT								CScaleAxis::GetSelectAxis(int x, int y)
{
	POINT p;
	p.x	= x;
	p.y = y;
	POINT l0, l1;
	int selectAxis	=	0;
	Phantom::Camera* c		=	Phantom::GetCameraPtr();
	c->Get2DFrom3D((Phantom::Pixel&)l0, m_axisOrig);
	{
		c->Get2DFrom3D((Phantom::Pixel&)l1, m_axisOrig + m_axisDirX);
		float fDistance;
		if(PointLineDistance(p, l0, l1, 0, &fDistance) && fDistance < 15.0f)
		{
			selectAxis	|=	Axis_X;
		}
	}
	{
		c->Get2DFrom3D((Phantom::Pixel&)l1, m_axisOrig + m_axisDirY);
		float fDistance;
		if(PointLineDistance(p, l0, l1, 0, &fDistance) && fDistance < 15.0f)
		{
			selectAxis	|=	Axis_Y;
		}
	}
	{
		c->Get2DFrom3D((Phantom::Pixel&)l1, m_axisOrig + m_axisDirZ);
		float fDistance;
		if(PointLineDistance(p, l0, l1, 0, &fDistance) && fDistance < 15.0f)
		{
			selectAxis	|=	Axis_Z;
		}
	}
	{
		Phantom::PIntersectData d;
		float dist = 100000.0f;
		memset(&d, 0, sizeof(d));
		d.t = &dist;

		Phantom::float3 vOrig, vDir;
		c->Get3DFrom2D( vOrig, vDir, (Phantom::Pixel&)p );
		if(Phantom::IntersectTriangle(&d, vOrig, vDir, m_planeXY.v0, m_planeXY.v1, m_planeXY.v2, NULL , TRUE) || Phantom::IntersectTriangle(&d, vOrig, vDir, m_planeXY.v2, m_planeXY.v1, m_planeXY.v0, NULL , TRUE))
			selectAxis	=	Axis_X | Axis_Y;
		else if(Phantom::IntersectTriangle(&d, vOrig, vDir, m_planeYZ.v0, m_planeYZ.v1, m_planeYZ.v2, NULL , TRUE) || Phantom::IntersectTriangle(&d, vOrig, vDir, m_planeYZ.v2, m_planeYZ.v1, m_planeYZ.v1, NULL , TRUE))
			selectAxis	=	Axis_Y | Axis_Z;
		else if(Phantom::IntersectTriangle(&d, vOrig, vDir, m_planeZX.v0, m_planeZX.v1, m_planeZX.v2, NULL , TRUE) || Phantom::IntersectTriangle(&d, vOrig, vDir, m_planeZX.v2, m_planeZX.v1, m_planeZX.v0, NULL , TRUE))
			selectAxis	=	Axis_Z | Axis_X;
	}
	float fx	=	(float)p.x - l0.x;
	float fy	=	(float)p.y - l0.y;
	if(sqrtf(fx*fx + fy*fy) < 15.0f)
	{
		selectAxis	=	Axis_Z | Axis_X | Axis_Y;
	}
	//
	m_selectAxisShow	=	selectAxis;
	return selectAxis;
}
