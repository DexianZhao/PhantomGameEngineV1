//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   MoveAxis.cpp
	软件名称   :   幻影游戏引擎（www.aixspace.com）

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	Copyright 2009-2016 Zhao Dexian
	
	
	
	
	

	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "MainFrm.h"
#include ".\moveaxis.h"

TRANS_MODE	g_transMode	=	TRANS_MODE_WORLD;
VOID						SetTransMode(TRANS_MODE mode)	//坐标对齐转变模式，Local
{
	g_transMode	=	mode;
}

TRANS_MODE					GetTransMode()
{
	return g_transMode;
}


CMoveAxis::CMoveAxis(void)
{
	m_bAxisLButtonDown	=	FALSE;
	this->m_newPosition.setxyz(0,0,0);
	this->m_downPosition.setxyz(0,0,0);
	memset(&m_moveBegin, 0, sizeof(m_moveBegin));
}

CMoveAxis::~CMoveAxis(void)
{
}

VOID			CMoveAxis::DrawAxis(float fSize)
{
	Phantom::float3 pos		=	this->m_viewWorld.GetPosition();
	DrawAxis(pos, fSize);
}

VOID			CMoveAxis::DrawAxis(const Phantom::float3& pos, float fSize, float fColor)
{
	if(!m_axisCallback)
		return;
	Phantom::matrix4x4 mat;
	mat.identity();
	Phantom::g_manager.SetWorldMatrix(&mat);
	Phantom::Camera* camera	=	Phantom::GetCameraPtr();
	//
	Phantom::g_manager->SetCullMode(Phantom::CullMode_Double);
	Phantom::g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
	//
	Phantom::float3 vDir		=	camera->GetEye() - pos;
	float fLength	=	m_fCameraLength	=	vDir.length() * 0.2f * fSize;
	m_axisOrig		=	pos;
	m_axisDirX		=	Phantom::float3(fLength, 0, 0);
	m_axisDirY		=	Phantom::float3(0, fLength, 0);
	m_axisDirZ		=	Phantom::float3(0, 0, fLength);
	//
	Phantom::color4	x,y,z;
	x				=	Phantom::color4(fColor,0,0,1);
	y				=	Phantom::color4(0,fColor,0,1);
	z				=	Phantom::color4(0,0,fColor,1);
	//
	float	fArrowX			=	fLength * .1f;
	float	fArrowY			=	fArrowX;
	float	fArrowZ			=	fArrowX;
	//
	if(m_selectAxisShow & Axis_X)
		fArrowX		*=	2.0f;
	else
		x.a			=	.3f;
	if(m_selectAxisShow & Axis_Y)
		fArrowY		*=	2.0f;
	else
		y.a			=	.3f;
	if(m_selectAxisShow & Axis_Z)
		fArrowZ		*=	2.0f;
	else
		z.a			=	.3f;
	//
	float fLengthMul	=	3.0f;
	m_triangleX[0].v1	=	m_triangleX[1].v1	=	m_axisDirX;
	m_triangleX[0].v0	=	m_triangleX[0].v2	=	m_triangleX[0].v1;
	m_triangleX[1].v0	=	m_triangleX[1].v2	=	m_triangleX[1].v1;
	m_triangleX[0].v0.x	=	m_triangleX[0].v2.x	=	m_triangleX[0].v1.x	-	fArrowX * fLengthMul;
	m_triangleX[1].v0.x	=	m_triangleX[1].v2.x	=	m_triangleX[1].v1.x	-	fArrowX * fLengthMul;
	m_triangleX[0].v0.y	+=	fArrowX;
	m_triangleX[1].v2.z	+=	fArrowX;
	//
	m_triangleY[0].v1	=	m_triangleY[1].v1	=	m_axisDirY;
	m_triangleY[0].v0	=	m_triangleY[0].v2	=	m_triangleY[0].v1;
	m_triangleY[1].v0	=	m_triangleY[1].v2	=	m_triangleY[1].v1;
	m_triangleY[0].v0.y	=	m_triangleY[0].v2.y	=	m_triangleY[0].v1.y	-	fArrowY * fLengthMul;
	m_triangleY[1].v0.y	=	m_triangleY[1].v2.y	=	m_triangleY[1].v1.y	-	fArrowY * fLengthMul;
	m_triangleY[0].v0.z	+=	fArrowY;
	m_triangleY[1].v2.x	+=	fArrowY;
	//
	m_triangleZ[0].v1	=	m_triangleZ[1].v1	=	m_axisDirZ;
	m_triangleZ[0].v0	=	m_triangleZ[0].v2	=	m_triangleZ[0].v1;
	m_triangleZ[1].v0	=	m_triangleZ[1].v2	=	m_triangleZ[1].v1;
	m_triangleZ[0].v0.z	=	m_triangleZ[0].v2.z	=	m_triangleZ[0].v1.z	-	fArrowZ * fLengthMul;
	m_triangleZ[1].v0.z	=	m_triangleZ[1].v2.z	=	m_triangleZ[1].v1.z	-	fArrowZ * fLengthMul;
	m_triangleZ[0].v0.x	+=	fArrowZ;
	m_triangleZ[1].v2.y	+=	fArrowZ;
	//
	//
	m_planeXY.v0	=	m_planeXY.v1	=	m_planeXY.v2	=	m_planeXY.v3	=	Phantom::float3(0, 0, 0);//m_axisOrig;
	m_planeYZ.v0	=	m_planeYZ.v1	=	m_planeYZ.v2	=	m_planeYZ.v3	=	Phantom::float3(0, 0, 0);//m_axisOrig;
	m_planeZX.v0	=	m_planeZX.v1	=	m_planeZX.v2	=	m_planeZX.v3	=	Phantom::float3(0, 0, 0);//m_axisOrig;
	//
	Phantom::color4 xy(fColor, fColor, 0, 1);
	Phantom::color4 yz(0, fColor, fColor, 1);
	Phantom::color4 zx(fColor, 0, fColor, 1);
	//
	xy.from(0xffffffff);
	xy	=	yz	=	zx;
	//
	xy 					*= 0.25f;
	yz 					*= 0.25f;
	zx 					*= 0.25f;
	//
	if(this->m_selectAxisShow == (Axis_X | Axis_Y))
		xy				+=	0.65f;
	else if(this->m_selectAxisShow == (Axis_Z | Axis_Y))
		yz				+=	0.65f;
	else if(this->m_selectAxisShow == (Axis_X | Axis_Z))
		zx				+=	0.65f;
	float fBoxLength	=	fLength * 0.2f;
	m_planeXY.v0.x		+=	fBoxLength * 2.0f;
	m_planeXY.v1.x		+=	fBoxLength * 2.0f;
	m_planeXY.v1.y		+=	fBoxLength * 2.0f;
	m_planeXY.v2.y		+=	fBoxLength * 2.0f;
	//
	m_planeYZ.v0.y		+=	fBoxLength * 0.6f;
	m_planeYZ.v1.y		+=	fBoxLength * 0.6f;
	m_planeYZ.v1.z		+=	fBoxLength * 0.6f;
	m_planeYZ.v2.z		+=	fBoxLength * 0.6f;
	//								   * 0.6f
	m_planeZX.v0.z		+=	fBoxLength * 0.6f;
	m_planeZX.v1.z		+=	fBoxLength * 0.6f;
	m_planeZX.v1.x		+=	fBoxLength * 0.6f;
	m_planeZX.v2.x		+=	fBoxLength * 0.6f;
	//
	float fScaleX	=	1.0f;
	float fScaleY	=	1.0f;
	float fScaleZ	=	1.0f;
			for(int i=0;i<2;i++)
			{
				m_triangleX[i].v0	+=	m_axisOrig;
				m_triangleX[i].v1	+=	m_axisOrig;
				m_triangleX[i].v2	+=	m_axisOrig;
				m_triangleY[i].v0	+=	m_axisOrig;
				m_triangleY[i].v1	+=	m_axisOrig;
				m_triangleY[i].v2	+=	m_axisOrig;
				m_triangleZ[i].v0	+=	m_axisOrig;
				m_triangleZ[i].v1	+=	m_axisOrig;
				m_triangleZ[i].v2	+=	m_axisOrig;
				//
			}
			//
			m_planeXY.v0	+=	m_axisOrig;
			m_planeXY.v1	+=	m_axisOrig;
			m_planeXY.v2	+=	m_axisOrig;
			m_planeXY.v3	+=	m_axisOrig;
			m_planeYZ.v0	+=	m_axisOrig;
			m_planeYZ.v1	+=	m_axisOrig;
			m_planeYZ.v2	+=	m_axisOrig;
			m_planeYZ.v3	+=	m_axisOrig;
			m_planeZX.v0	+=	m_axisOrig;
			m_planeZX.v1	+=	m_axisOrig;
			m_planeZX.v2	+=	m_axisOrig;
			m_planeZX.v3	+=	m_axisOrig;
	Phantom::g_manager.DrawLine( m_axisOrig, m_axisDirX + m_axisOrig, &x );
	Phantom::g_manager.DrawLine( m_axisOrig, m_axisDirY + m_axisOrig, &y );
	Phantom::g_manager.DrawLine( m_axisOrig, m_axisDirZ + m_axisOrig, &z );
	//
	Phantom::g_manager.DrawTriangle(m_triangleX[0].v0, m_triangleX[0].v1, m_triangleX[0].v2, &x);
	Phantom::g_manager.DrawTriangle(m_triangleX[1].v0, m_triangleX[1].v1, m_triangleX[1].v2, &x);
	//
	Phantom::g_manager.DrawTriangle(m_triangleY[0].v0, m_triangleY[0].v1, m_triangleY[0].v2, &y);
	Phantom::g_manager.DrawTriangle(m_triangleY[1].v0, m_triangleY[1].v1, m_triangleY[1].v2, &y);
	//
	Phantom::g_manager.DrawTriangle(m_triangleZ[0].v0, m_triangleZ[0].v1, m_triangleZ[0].v2, &z);
	Phantom::g_manager.DrawTriangle(m_triangleZ[1].v0, m_triangleZ[1].v1, m_triangleZ[1].v2, &z);
	//
	Phantom::g_manager.DrawTriangle( m_planeXY.v0, m_planeXY.v1, m_planeXY.v2, &xy);
	Phantom::g_manager.DrawTriangle( m_planeXY.v2, m_planeXY.v3, m_planeXY.v0, &xy);
	//
	Phantom::g_manager.DrawTriangle( m_planeYZ.v0, m_planeYZ.v1, m_planeYZ.v2, &yz);
	Phantom::g_manager.DrawTriangle( m_planeYZ.v2, m_planeYZ.v3, m_planeYZ.v0, &yz);
	//
	Phantom::g_manager.DrawTriangle( m_planeZX.v0, m_planeZX.v1, m_planeZX.v2, &zx);
	Phantom::g_manager.DrawTriangle( m_planeZX.v2, m_planeZX.v3, m_planeZX.v0, &zx);
	Phantom::g_manager->SetAlphaMode(Phantom::AlphaMode_None);
}

#define	CURSOR_HAND		MAKEINTRESOURCE(32649)

Phantom::float3			PickPlanePoint(const Phantom::Pixel& point, const Phantom::float3& center, const Phantom::float3& normal, const Phantom::float3& left, const Phantom::float3& top)
{
	Phantom::float3 orig, dir;
	Phantom::GetCameraPtr()->Get3DFrom2D(orig, dir, point);
	dir.normalize();
	//dir				*=	To3DValue(10000.0f);
	//
	Phantom::float3 vLeft	=	left * To3DValue(10000.0f);
	Phantom::float3 vTop		=	top	* To3DValue(10000.0f);
	Phantom::float3 vRight	=	-vLeft;
	Phantom::float3 vBottom	=	-vTop;
	Phantom::float3 v0		=	vLeft + vTop + center;
	Phantom::float3 v1		=	vRight + vTop + center;
	Phantom::float3 v2		=	vRight + vBottom + center;
	Phantom::float3 v3		=	vLeft + vBottom + center;
	//
	Phantom::PIntersectData data;
	Phantom::float3 newPos = center;
	data.Pick	=	&newPos;
	if(Phantom::IntersectTriangle(&data, orig, dir, v0, v1, v2, 0, true))
		return newPos;
	if(Phantom::IntersectTriangle(&data, orig, dir, v2, v3, v0, 0, true))
		return newPos;
	if(Phantom::IntersectTriangle(&data, orig, dir, v2, v1, v0, 0, true))
		return newPos;
	if(Phantom::IntersectTriangle(&data, orig, dir, v0, v3, v2, 0, true))
		return newPos;
	return newPos;
	//PPlane plane;
	//planeFromPointNormal(&plane, &center, &normal);
	////
	//Phantom::float3 orig, dir;
	//Phantom::GetCameraPtr()->Get3DFrom2D(orig, dir, point);
	//dir.normalize();
	//dir				*=	To3DValue(10000.0f);
	////
	//Phantom::float3 newPos;
	//planeIntersectLine(&newPos, &plane, &orig, &dir);
	//return newPos;
}

BOOL			CMoveAxis::OnLButtonDown(POINT p)
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
		Phantom::Pixel cursor;
		::GetCursorPos((LPPOINT)&cursor);
		ScreenToClient((HWND)GetDeviceWindow(), (LPPOINT)&cursor);
		{
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
		}
		if(!m_axisCallback->BeginEditor(m_editorWorld, AxisOT_Move))
			return false;
		//
		Phantom::Pixel center;
		if(!c->Get2DFrom3D(center, this->m_editorWorld.GetPosition()))
		{
			center.x	=	cursor.x;
			center.y	=	cursor.y;
		}
		m_moveBegin.x	=	cursor.x - center.x;
		m_moveBegin.y	=	cursor.y - center.y;
		//
		m_newPosition = m_editorWorld.GetPosition();
		this->m_downOffset[0]	=	PickPlanePoint(cursor, this->m_editorWorld.GetPosition(), Phantom::float3(1, 0, 0), Phantom::float3(0, 0, 1), Phantom::float3(0, 1, 0)) - m_editorWorld.GetPosition();
		this->m_downOffset[1]	=	PickPlanePoint(cursor, this->m_editorWorld.GetPosition(), Phantom::float3(0, 1, 0), Phantom::float3(1, 0, 0), Phantom::float3(0, 0, 1)) - m_editorWorld.GetPosition();
		this->m_downOffset[2]	=	PickPlanePoint(cursor, this->m_editorWorld.GetPosition(), Phantom::float3(0, 0, 1), Phantom::float3(1, 0, 0), Phantom::float3(0, 1, 0)) - m_editorWorld.GetPosition();
		//
		m_bAxisLButtonDown	=	TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL			CMoveAxis::OnLButtonUp(POINT p)
{
	if(!m_axisCallback)return FALSE;
	RECT rc;
	GetClientRect(GetDeviceWindow(), &rc);
	if(PtInRect(&rc, p) && m_bAxisLButtonDown)
	{
		m_axisCallback->EndEditor(m_editorWorld, AxisOT_Move, m_newPosition - m_editorWorld.GetPosition());
		m_bAxisLButtonDown	=	FALSE;
		return TRUE;
	}
	m_bAxisLButtonDown	=	FALSE;
	return FALSE;
}

BOOL			CMoveAxis::OnMouseMove(POINT p, BOOL bPick)
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
		Phantom::matrix4x4 mat		=	m_editorWorld;
		Phantom::Pixel cursor;
		::GetCursorPos((LPPOINT)&cursor);
		ScreenToClient((HWND)GetDeviceWindow(), (LPPOINT)&cursor);

		if(m_selectAxis == (Axis_X | Axis_Y) || m_selectAxis == (Axis_X | Axis_Z) || m_selectAxis == (Axis_Y | Axis_Z) || m_selectAxis==(Axis_X | Axis_Y | Axis_Z))
		{
			if(bPick || GetSelectKeyboard())
			{
				Phantom::float3 pos;
				if(!this->m_axisCallback->PickTerrain(p.x, p.y, pos))
					return true;
				mat.SetPosition( pos );// - m_editorWorld.GetPosition() );
			}
			else
			{
				if(m_selectAxis == (Axis_X | Axis_Y))
				{
					Phantom::float3	pos		=	PickPlanePoint(cursor, m_editorWorld.GetPosition(), Phantom::float3(0, 0, 1), Phantom::float3(1, 0, 0), Phantom::float3(0, 1, 0));
					mat.SetPosition( pos - m_downOffset[2] );
				}
				else if(m_selectAxis == (Axis_X | Axis_Z))
				{
					Phantom::float3	pos		=	PickPlanePoint(cursor, m_editorWorld.GetPosition(), Phantom::float3(0, 1, 0), Phantom::float3(1, 0, 0), Phantom::float3(0, 0, 1));
					mat.SetPosition( pos - m_downOffset[1] );
				}
				else if(m_selectAxis == (Axis_Y | Axis_Z))
				{
					Phantom::float3	pos		=	PickPlanePoint(cursor, m_editorWorld.GetPosition(), Phantom::float3(1, 0, 0), Phantom::float3(0, 0, 1), Phantom::float3(0, 1, 0));
					mat.SetPosition( pos - m_downOffset[0] );
				}
			}
			m_newPosition	=	mat.GetPosition();
			this->m_axisCallback->OnEditor( m_editorWorld, mat, AxisOT_Move, m_newPosition - m_editorWorld.GetPosition() );
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
				mat.SetPosition(mat.GetPosition() + this->m_axisDirX * (fMove * m_fCameraLength / this->m_axisDirX.R()));
			}
			else if(m_selectAxis & Axis_Y)
			{
				float fMove = 0.0f;
				if(fabsf(m_axisDirY2D.x) > fabsf(m_axisDirY2D.y))
					fMove	= m_axisDirY2D.x != 0.0f ? pt.x / m_axisDirY2D.x : 0;
				else
					fMove	= m_axisDirY2D.y != 0.0f ? pt.y / m_axisDirY2D.y : 0;
				mat.SetPosition(mat.GetPosition() + this->m_axisDirY * (fMove * m_fCameraLength / this->m_axisDirY.R()));
			}
			else if(m_selectAxis & Axis_Z)
			{
				float fMove = 0.0f;
				fMove	= m_axisDirZ2D.y != 0.0f ? pt.y / m_axisDirZ2D.y : 0;
				mat.SetPosition(mat.GetPosition() + this->m_axisDirZ * (fMove * m_fCameraLength / this->m_axisDirZ.R()));
			}
			m_newPosition	=	mat.GetPosition();
			this->m_axisCallback->OnEditor( m_editorWorld, mat, AxisOT_Move, m_newPosition - m_editorWorld.GetPosition() );
		}
		//
		//switch(GetTransMode())
		//{
		//case TRANS_MODE_WORLD:
		//	{
				Phantom::matrix4x4 mat2;
				mat2.identity();
				mat2.SetPosition( mat.GetPosition() );
				m_viewWorld		=	mat2;
		//	}
		//	break;
		//case TRANS_MODE_LOCAL:
		//	{
		//		m_viewWorld		=	mat;
		//	}
		//	break;
		//}
		//
		//m_downPoint	=	p;
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

BOOL				PointLineDistance(const POINT& p0, const POINT& line0, const POINT& line1, Phantom::float2* pOutputCross, float* pOutputDistance)
{
	Phantom::float2	d((float)(line1.x - line0.x), (float)(line1.y - line0.y));
	Phantom::float2	yp((float)(p0.x - line0.x), (float)(p0.y - line0.y));
	float	d2	= (d.x*d.x + d.y*d.y);
	float	t	= d.dot(yp) / d2;
	if(t < 0 || t > 1)
		return FALSE;
	Phantom::float2 vPick			=	Phantom::float2((float)line0.x + d.x * t, (float)line0.y + d.y * t);
	if(pOutputCross)
		*pOutputCross		=	vPick;
	if(pOutputDistance)
	{
		float dx			=	(float)p0.x - vPick.x;
		float dy			=	(float)p0.y - vPick.y;
		*pOutputDistance	=	sqrtf(dx*dx + dy*dy);
	}
	return TRUE;
}

//
INT								CMoveAxis::GetSelectAxis(int x, int y)
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
	//
	m_selectAxisShow	=	selectAxis;
	return selectAxis;
}
