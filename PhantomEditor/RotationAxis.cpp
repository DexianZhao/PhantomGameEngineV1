//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   RotationAxis.cpp
	软件名称   :   幻影游戏引擎（www.aixspace.com）

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	Copyright 2009-2016 Zhao Dexian
	
	
	
	
	

	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\rotationaxis.h"
#include "GCSceneEditor3D.h"

CRotationAxis::CRotationAxis(void)
{
	m_bAxisLButtonDown	=	FALSE;
	m_newRotation.setxyz(0, 0, 0);
	m_downRotation.setxyz(0,0,0);
	m_scene3D	=	0;
}

CRotationAxis::~CRotationAxis(void)
{
}

VOID			CRotationAxis::DrawAxis(float fSize)
{
	if(!m_axisCallback)return;
	Phantom::matrix4x4 mat;
	mat.identity();
	Phantom::g_manager.SetWorldMatrix(&mat);
	//
	Phantom::g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
	Phantom::Camera* c	=	Phantom::GetCameraPtr();
	//
	Phantom::float3 pos		=	m_viewWorld.GetPosition();
	Phantom::float3 vDir		=	c->GetEye() - pos;
	float fLength	=	m_fCameraLength	=	vDir.length() * 0.1f *  fSize;
	m_axisOrig		=	pos;
	m_axisDirX		=	Phantom::float3(fLength, 0, 0);
	m_axisDirY		=	Phantom::float3(0, fLength, 0);
	m_axisDirZ		=	Phantom::float3(0, 0, fLength);
	//
	Phantom::color4	x,y,z;
	x				=	Phantom::color4(1,0,0,.3);
	y				=	Phantom::color4(0,1,0,.3);
	z				=	Phantom::color4(0,0,1,.3);
	//
	if(m_selectAxisShow & Axis_X)
		x.a			=	1.0f;
	if(m_selectAxisShow & Axis_Y)
		y.a			=	1.0f;
	if(m_selectAxisShow & Axis_Z)
		z.a			=	1.0f;
	//
	unsigned int cX	=	x;
	unsigned int cY	=	y;
	unsigned int cZ	=	z;

	for(int i=0;i<=AXIS_POINT;i++)
	{
		float f = (float)i / (float)AXIS_POINT * 2.0f * PI;
		this->m_axisCircleX[i]	=	Phantom::float3(0.0f, cosf(f) * m_fCameraLength, sinf(f) * m_fCameraLength) + m_axisOrig;
		this->m_axisCircleY[i]	=	Phantom::float3(cosf(f) * m_fCameraLength, 0.0f, sinf(f) * m_fCameraLength) + m_axisOrig;
		this->m_axisCircleZ[i]	=	Phantom::float3(cosf(f) * m_fCameraLength, sinf(f) * m_fCameraLength, 0.0f) + m_axisOrig;
		float sf					=	sinf(f);
		if(sf > 0)
		{
			m_axisColorX[i]	=	cX;
			m_axisColorY[i]	=	cY;
		}
		else
		{
			m_axisColorX[i]	=	0;
			m_axisColorY[i]	=	0;
		}
		m_axisColorZ[i]	=	cZ;
	}
	Phantom::g_manager.SetRenderMode(Phantom::RenderMode_Color);
	Phantom::g_manager->DrawNoIndexed(Phantom::g_manager.GetCurrProgram(), Phantom::DrawVInfos(AXIS_POINT, m_axisCircleX, m_axisColorX), AXIS_POINT, Phantom::DrawMode_LineStrip);
	Phantom::g_manager->DrawNoIndexed(Phantom::g_manager.GetCurrProgram(), Phantom::DrawVInfos(AXIS_POINT, m_axisCircleY, m_axisColorY), AXIS_POINT, Phantom::DrawMode_LineStrip);
	Phantom::g_manager->DrawNoIndexed(Phantom::g_manager.GetCurrProgram(), Phantom::DrawVInfos(AXIS_POINT, m_axisCircleZ, m_axisColorZ), AXIS_POINT, Phantom::DrawMode_LineStrip);
}

#define	CURSOR_HAND		MAKEINTRESOURCE(32649)

BOOL			CRotationAxis::OnLButtonDown(POINT p)
{
	if(!m_axisCallback)return FALSE;
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
		m_downRotation.setxyz(0,0,0);
		if(!this->m_axisCallback->BeginEditor(m_editorWorld, AxisOT_Rotation))
			return false;
		m_bAxisLButtonDown	=	TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL			CRotationAxis::OnLButtonUp(POINT p)
{
	if(!m_axisCallback)return FALSE;
	RECT rc;
	GetClientRect(GetDeviceWindow(), &rc);
	if(PtInRect(&rc, p) && m_bAxisLButtonDown)
	{
		m_axisCallback->EndEditor(m_editorWorld, AxisOT_Rotation, m_newRotation);
		m_bAxisLButtonDown	=	FALSE;
		return TRUE;
	}
	m_bAxisLButtonDown	=	FALSE;
	return FALSE;
}

BOOL			CRotationAxis::OnMouseMove(POINT p, BOOL bPick)
{
	if(!m_axisCallback)return FALSE;
	RECT rc;
	GetClientRect(GetDeviceWindow(), &rc);
	if(PtInRect(&rc, p) && HIBYTE(GetKeyState(VK_LBUTTON)) && m_bAxisLButtonDown && m_selectAxis)
	{
		//
		Phantom::float2 pt((float)(p.x - m_downPoint.x), (float)(p.y - m_downPoint.y));
		//
		Phantom::Camera* c		=	Phantom::GetCameraPtr();
		//
		Phantom::matrix4x4 mat	=	m_editorWorld;
		Phantom::matrix4x4 matRot;
		if(m_scene3D&&m_scene3D->m_scene3D){
			if(m_selectAxis & Axis_X)
			{
				float fMove = 0.0f;
				fMove		=	(float)pt.y / 200 * PI;
				fMove		=	Radians((int)Degress(fMove));
				matrixRotationX(&matRot, -fMove);
				m_newRotation.x	=	m_downRotation.x - fMove;
				matrixMultiply(&mat, &matRot, &mat);
			}
			if(m_selectAxis & Axis_Y)
			{
				float fMove = 0.0f;
				fMove		=	(float)pt.y / 200 * PI;
				fMove		=	Phantom::Radians((int)Phantom::Degress(fMove));
				matrixRotationY(&matRot, fMove);
				m_newRotation.y	=	m_downRotation.y + fMove;
				matrixMultiply(&mat, &matRot, &mat);
			}
			if(m_selectAxis & Axis_Z)
			{
				float fMove = 0.0f;
				fMove		=	(float)pt.x / 200 * PI;
				fMove		=	Phantom::Radians((int)Phantom::Degress(fMove));
				matrixRotationZ(&matRot, fMove);
				m_newRotation.z	=	m_downRotation.z + fMove;
				matrixMultiply(&mat, &matRot, &mat);
			}
		}
		else
		{
			if(m_selectAxis & Axis_X)
			{
				float fMove = 0.0f;
				fMove		=	(float)pt.y / 100.0f;
				matrixRotationX(&matRot, -fMove);
				m_newRotation.x	=	m_downRotation.x - fMove;
				matrixMultiply(&mat, &matRot, &mat);
			}
			if(m_selectAxis & Axis_Y)
			{
				float fMove = 0.0f;
				fMove		=	(float)pt.y / 100.0f;
				matrixRotationY(&matRot, fMove);
				m_newRotation.y	=	m_downRotation.y + fMove;
				matrixMultiply(&mat, &matRot, &mat);
			}
			if(m_selectAxis & Axis_Z)
			{
				float fMove = 0.0f;
				fMove		=	(float)pt.x / 100.0f;
				matrixRotationZ(&matRot, fMove);
				m_newRotation.z	=	m_downRotation.z + fMove;
				matrixMultiply(&mat, &matRot, &mat);
			}
		}
		//matrixRotationXYZ(&matRot, m_newRotation);
		//matrixMultiply(&mat, &matRot, &mat);
		if(this->m_axisCallback)
			m_axisCallback->OnEditor(m_editorWorld, mat, AxisOT_Rotation, m_newRotation);
		//
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

//
INT								CRotationAxis::GetSelectAxis(int x, int y)
{
	POINT p;
	p.x	= x;
	p.y = y;
	POINT l0, l1;
	int selectAxis	=	0;
	Phantom::Camera* c		=	Phantom::GetCameraPtr();
	for(int i=0;i<AXIS_POINT;i++)
	{
		c->Get2DFrom3D((Phantom::Pixel&)l0, m_axisCircleX[i]);
		c->Get2DFrom3D((Phantom::Pixel&)l1, m_axisCircleX[i + 1]);
		if(m_axisColorX[i] == 0 || m_axisColorX[i + 1] == 0)
			continue;
		float fDistance;
		if(PointLineDistance(p, l0, l1, 0, &fDistance) && fDistance < 5.0f)
		{
			selectAxis	|=	Axis_X;
			break;
		}
	}
	if(selectAxis == 0)
	{
		for(int i=0;i<AXIS_POINT;i++)
		{
			c->Get2DFrom3D((Phantom::Pixel&)l0, m_axisCircleY[i]);
			c->Get2DFrom3D((Phantom::Pixel&)l1, m_axisCircleY[i + 1]);
			if(m_axisColorY[i] == 0 || m_axisColorY[i + 1] == 0)
				continue;
			float fDistance;
			if(PointLineDistance(p, l0, l1, 0, &fDistance) && fDistance < 5.0f)
			{
				selectAxis	|=	Axis_Y;
				break;
			}
		}
	}
	if(selectAxis == 0)
	{
		for(int i=0;i<AXIS_POINT;i++)
		{
			c->Get2DFrom3D((Phantom::Pixel&)l0, m_axisCircleZ[i]);
			c->Get2DFrom3D((Phantom::Pixel&)l1, m_axisCircleZ[i + 1]);
			if(m_axisColorZ[i + 1] == 0)
				continue;
			float fDistance;
			if(PointLineDistance(p, l0, l1, 0, &fDistance) && fDistance < 5.0f)
			{
				selectAxis	|=	Axis_Z;
				break;
			}
		}
	}
	//
	//PIntersectData d;
	//float dist = 100000.0f;
	//memset(&d, 0, sizeof(d));
	//d.t = &dist;
	//Phantom::float3 orig, dir;
	//c->Get3DFrom2D(orig, dir, Phantom::Pixel(x,y));
	//m_fCameraLength
	//for(int i=0;i<AXIS_POINT;i++)
	//{
	//	if(::intersectTriangle(&d, orig, dir, this->m_axisOrig, this->m_axisCircleX[i], m_axisCircleX[i + 1], NULL, true) || ::intersectTriangle(&d, orig, dir, this->m_axisOrig, this->m_axisCircleX[i + 1], m_axisCircleX[i], NULL, true))
	//		selectAxis	=	Axis_X;
	//	if(::intersectTriangle(&d, orig, dir, this->m_axisOrig, this->m_axisCircleY[i], m_axisCircleY[i + 1], NULL, true) || ::intersectTriangle(&d, orig, dir, this->m_axisOrig, this->m_axisCircleY[i + 1], m_axisCircleY[i], NULL, true))
	//		selectAxis	=	Axis_Y;
	//	if(::intersectTriangle(&d, orig, dir, this->m_axisOrig, this->m_axisCircleZ[i], m_axisCircleZ[i + 1], NULL, true) || ::intersectTriangle(&d, orig, dir, this->m_axisOrig, this->m_axisCircleZ[i + 1], m_axisCircleZ[i], NULL, true))
	//		selectAxis	=	Axis_Z;
	//}
	m_selectAxisShow	=	selectAxis;
	return selectAxis;
}
