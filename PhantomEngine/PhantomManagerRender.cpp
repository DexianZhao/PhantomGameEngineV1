//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   Manager.cpp
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "PhantomManager.h"
#include "PhantomFile.h"

extern "C"{
    void    UpdateMotion(float gravityX,float gravityY,float gravityZ, float zTheta, float xyTheta, float rotationRateX, float rotationRateY,float rotationRateZ,
                         float roll, float pitch, float yaw)
    {
        Phantom::sensordata& data = Phantom::g_manager.m_sensordata;
        //data.gravity_x = gravityX;
        //data.gravity_y = gravityY;
        //data.gravity_z = gravityZ;
        data.theta_z = zTheta;
        data.theta_xy = xyTheta;
        data.rotation_rate_x = rotationRateX;
        data.rotation_rate_y = rotationRateY;
        data.rotation_rate_z = rotationRateZ;
        data.yaw = Phantom::Degress(yaw);
        data.pitch = Phantom::Degress(pitch);
        data.roll = Phantom::Degress(roll);
		Phantom::g_manager.AddGravity(gravityX,gravityY,gravityZ);
    }
    void    UpdateAcc(float accx,float accy,float accz, long timestamp)
    {
        Phantom::GetCameraPtr()->processAcc(accx, accy, accz, timestamp);
    }
    void    UpdateGryo(float accx,float accy,float accz, long timestamp)
    {
        Phantom::GetCameraPtr()->processGyro(accx, accy, accz, timestamp);
    }
};

namespace Phantom{
	static	unsigned short	s_aabox_indices3[36] = {4,0,3, 3,7,4, 1,5,6, 6,2,1, 1,0,4, 4,5,1, 3,2,6, 6,7,3, 0,1,2, 2,3,0, 6,5,4, 4,7,6};
	static	unsigned short	s_aabox_indices2[24] = {0,1, 1,2, 2,3, 4,5, 5,6, 6,7, 3,0, 7,4, 0,4, 1,5, 2,6, 3,7};
	static	unsigned short	s_aabox_indicesF[36] = {0,1,2,2,3,0, 4,5,6,6,7,4, 8,9,10,10,11,8, 12,13,14,14,15,12, 16,17,18,18,19,16, 20,21,22,22,23,20};
	//

	void								Manager::ClearAlphaObjects()
	{CPUTimeT(Manager);
		//m_renderNormalizeCount	=	0;
		m_renderAlphaCount		=	0;
		m_renderNoZBufferCount	=	0;
		for(int i=0;i<MAX_RENDER_SORTLIST;i++)
		{
			if(m_renderAlpha[i])
				m_renderAlpha[i]->m_pNext = 0;
			if(m_renderNoZBuffer[i])
				m_renderNoZBuffer[i]->m_pNext = 0;
		}
		memset(m_renderAlpha, 0, sizeof(m_renderAlpha));
		memset(m_renderNoZBuffer, 0, sizeof(m_renderNoZBuffer));
	}
	void								Manager::Push3DDrawMgr(UISortBase* mgr, const float3& pos)
	{
		if(!mgr || mgr->m_pNext)
			return;
		const float3&		eye		=	m_camera->GetEye();
		mgr->m_pNext	=	0;
		float3 dir = eye - pos;
		float fDist	=	dir.R();
		int		dit	=	(int)(fDist / this->m_fRenderAlphaDistance);
		if(dit >= MAX_RENDER_SORTLIST)
			dit	=	MAX_RENDER_SORTLIST - 1;
		else if(dit < 0)
			dit	=	0;
		if(m_renderDrawMgr[dit]==mgr)return;
		mgr->m_pNext		=	m_renderDrawMgr[dit];
		m_renderDrawMgr[dit]	=	mgr;
	}
	void								Manager::RenderAll3DDrawMgr()
	{CPUTimeT(Manager);
		for(int i=0;i<m_scene3ds.size();i++)
		{
			if(m_scene3ds[i]->IsEnabled(Scene3D::Enable_ShowScene)&&m_scene3ds[i]->m_dlgScene)
			{
				m_scene3ds[i]->m_dlgScene->BeginRender();
				break;
			}
		}
		for(int i=MAX_RENDER_SORTLIST - 1;i >= 0;i--)
		{
			UISortBase* pRender	=	m_renderDrawMgr[i];
			while(pRender)
			{
				UISortBase* old = pRender;
				pRender->Render3D();
				pRender	=	pRender->m_pNext;
				old->m_pNext = 0;
			}
		}
		m_renderDrawMgrCount = 0;
		for(int i=0;i<MAX_RENDER_SORTLIST;i++)
		{
			if(m_renderDrawMgr[i])
				m_renderDrawMgr[i]->m_pNext = 0;
		}
		memset(m_renderDrawMgr, 0, sizeof(m_renderDrawMgr));
	}
	void								Manager::PushAlphaObject(RenderBase* objPtr, BOOL bZWriteEnable)
	{
		if(!objPtr || objPtr->m_renderIdx == m_renderIdx)
			return;
		objPtr->m_renderIdx = m_renderIdx;
		const float3&		eye		=	m_camera->GetEye();
		const matrix4x4&	world	=	objPtr->GetWorldMatrix();
		objPtr->m_pNext	=	0;
		float3 dir = eye - world.GetPosition();
		float fDist	=	dir.R();
		int		dit	=	(int)(fDist / this->m_fRenderAlphaDistance);
		if(dit >= MAX_RENDER_SORTLIST)
			dit	=	MAX_RENDER_SORTLIST - 1;
		else if(dit < 0)
			dit	=	0;
		if(bZWriteEnable)
		{
			if(m_renderAlpha[dit]==objPtr)return;
			objPtr->m_pNext		=	m_renderAlpha[dit];
			m_renderAlpha[dit]	=	objPtr;
		}
		else
		{
			if(m_renderNoZBuffer[dit]==objPtr)return;
			objPtr->m_pNext		=	m_renderNoZBuffer[dit];
			m_renderNoZBuffer[dit]	=	objPtr;
		}
	}
	void			Manager::RenderAllAlphas()
	{CPUTimeT(Manager);
		for(int i=MAX_RENDER_SORTLIST - 1;i >= 0;i--)
		{
			RenderBase* pRender	=	m_renderAlpha[i];
			while(pRender)
			{
				RenderBase* old = pRender;
				pRender->drawObject(0);
				pRender	=	pRender->m_pNext;
			}
		}
		for(int i=MAX_RENDER_SORTLIST - 1;i >= 0;i--)
		{
			RenderBase* pRender	=	m_renderNoZBuffer[i];
			while(pRender)
			{
				pRender->drawObject(0);
				pRender	=	pRender->m_pNext;
			}
		}
	}

	VOID			Manager::DrawTriangles(const DrawVInfos& info, int numVertices, const Triangle* tList, int numTriangles, const matrix4x4* world)
	{CPUTimeT(Manager);
		SetWorldMatrix(world);
		if(tList)
			m_device->DrawIndexed(m_currProg, info, (const unsigned short*)tList, numTriangles * 3, DrawMode_Triangles);
		else
			m_device->DrawNoIndexed(m_currProg, info, numVertices, DrawMode_Triangles);
	}
	VOID			Manager::DrawPoints(const DrawVInfos& info, int numVertices, const matrix4x4* world)
	{CPUTimeT(Manager);
		SetWorldMatrix(world);
		m_device->DrawNoIndexed(m_currProg, info, numVertices, DrawMode_Points);
	}

	VOID			Manager::DrawTriangle(const float3& t1, const float3& t2, const float3& t3, const color4* c, const matrix4x4* world)
	{CPUTimeT(Manager);
		Program* prog = this->SetRenderMode(Phantom::RenderMode_Color);
		float3 t[3];
		t[0]	=	t1;
		t[1]	=	t2;
		t[2]	=	t3;
		unsigned int color[3];
		if(c){
			color[0] = *c;
			color[1] = *c;
			color[2] = *c;
		}
		SetWorldMatrix(world);
		m_device->DrawNoIndexed(prog, DrawVInfos(3, t, c?color:0), 3, DrawMode_Triangles);
	}
	VOID			Manager::DrawLine(const float3& l1, const float3& l2, const color4* c, const matrix4x4* world)
	{CPUTimeT(Manager);
		Program* prog = this->SetRenderMode(Phantom::RenderMode_Color);
		float3 l[2];
		l[0]	=	l1;
		l[1]	=	l2;
		unsigned int color[2];
		if(c){
			color[0] = *c;
			color[1] = *c;
		}
		SetWorldMatrix(world);
		m_device->DrawNoIndexed(prog, DrawVInfos(2, l, c?color:0), 2, DrawMode_Lines);
	}
	VOID			Manager::DrawBox(const AABox& box, unsigned int color, BOOL bFill)
	{CPUTimeT(Manager);
		Program* prog = this->SetRenderMode(Phantom::RenderMode_Color);
		PBoxVertices v;
		v.setup(box._min, box._max);
		float3	pos[8];
		for(int i=0;i<8;i++)
			pos[i]	=	v.v[i];
        unsigned int colors[8];
		for(int i=0;i<8;i++)
			colors[i] = color;
		//SetWorldMatrix(world);
		if(bFill)
			m_device->DrawIndexed(m_currProg, DrawVInfos(8, pos, colors), s_aabox_indices3, sizeof(s_aabox_indices3) / sizeof(unsigned short), DrawMode_Triangles);
		else
			m_device->DrawIndexed(m_currProg, DrawVInfos(8, pos, colors), s_aabox_indices2, sizeof(s_aabox_indices2) / sizeof(unsigned short), DrawMode_Lines);
	}
	VOID			Manager::DrawBox(const AABox& box, const unsigned int* colors, const matrix4x4* world, BOOL bFill)
	{CPUTimeT(Manager);
		Program* prog = this->SetRenderMode(Phantom::RenderMode_Color);
		PBoxVertices v;
		v.setup(box._min, box._max);
		float3	pos[8];
		for(int i=0;i<8;i++)
			pos[i]	=	v.v[i];
        unsigned int colorTemp[8];
        if(!colors)
        {
            colors = colorTemp;
            for(int i=0;i<8;i++)
                colorTemp[i] = 0xffffffff;
        }
		SetWorldMatrix(world);
		if(bFill)
			m_device->DrawIndexed(m_currProg, DrawVInfos(8, pos, colors), s_aabox_indices3, sizeof(s_aabox_indices3) / sizeof(unsigned short), DrawMode_Triangles);
		else
			m_device->DrawIndexed(m_currProg, DrawVInfos(8, pos, colors), s_aabox_indices2, sizeof(s_aabox_indices2) / sizeof(unsigned short), DrawMode_Lines);
	}
	VOID			Manager::DrawGrids3D(const float3& eye, unsigned int uiGridColor,float fGridWidthHeight,int numW, int numH)
	{CPUTimeT(Manager);
		float3 _eye = eye;
		_eye.z = 0;
		float fWidth2	=	fGridWidthHeight* (float)numW;
		float fHeight2	=	fGridWidthHeight* (float)numH;
		//
		color4 c1(1.0f,0.0f,0.0f,1.0f);
		color4 c2(0.0f,1.0f,1.0f,0.5f);
		color4 c3(0.0f,1.0f,0.0f,1.0f);
		color4 c4(1.0f,0.0f,1.0f,0.5f);
		color4 c5(0.0f,0.0f,1.0f,1.0f);
		color4 c6(1.0f,1.0f,0.0f,0.5f);
		DrawLine(float3(fWidth2,0.0f,0.0f),	float3(0.0f,0.0f,0.0f),	&c1);
		DrawLine(float3(0.0f,0.0f,0.0f), float3(-fWidth2,0.0f,0.0f),	&c2);
		DrawLine(float3(0.0f,fHeight2,0.0f), float3(0.0f,0.0f,0.0f),	&c3);
		DrawLine(float3(0.0f,0.0f,0.0f), float3(0.0f,-fHeight2,0.0f), &c4);
		DrawLine(float3(0.0f,0.0f,0.0f), float3(0.0,0.0f,getmin(fWidth2,fHeight2) * 0.25f),	&c5);
		DrawLine(float3(0.0f,0.0f,0.0f), float3(0.0,0.0f,-getmin(fWidth2,fHeight2)* 0.25f),	&c6);
		//
		int	nWidth	=	numW*2;
		int nHeight	=	numH*2;
		//
		float fWDist	=	fGridWidthHeight;
		float fHDist	=	fGridWidthHeight;
		//
		color4 cSubGrid(0x2f2f2f2f);
		color4 cGrid(uiGridColor);
		for(int i=0;i<numW;i++)
		{
			{
				float fw	=	(float)(i+1)*fWDist;
				DrawLine(float3(fw,fHeight2,0.0f),	float3(fw,-fHeight2,0.0f),	&cGrid);
				for(int x=1;x<5;x++)
				{
					fw	=	(float)i*fWDist + (float)x * (fWDist * 0.2f);
					DrawLine(float3(fw,fHeight2,0.0f),	float3(fw,-fHeight2,0.0f),	&cSubGrid);
				}
			}
			{
				float fw	=	(float)(-(i+1))*fWDist;
				DrawLine(float3(fw,fHeight2,0.0f),	float3(fw,-fHeight2,0.0f),	&cGrid);
				for(int x=1;x<5;x++)
				{
					fw	=	(float)-i*fWDist - (float)x * (fWDist * 0.2f);
					DrawLine(float3(fw,fHeight2,0.0f),	float3(fw,-fHeight2,0.0f),	&cSubGrid);
				}
			}
		}
		for(int j=0;j<numH;j++)
		{
			{
				float fh=(float)(j+1)*fHDist;
				DrawLine(float3(fWidth2,fh,0.0f),float3(-fWidth2,fh,0.0f), &cGrid);
				for(int y=1;y<5;y++)
				{
					fh=(float)j*fHDist + (float)y * (fHDist * 0.2f);
					DrawLine(float3(fWidth2,fh,0.0f),float3(-fWidth2,fh,0.0f), &cSubGrid);
				}
			}
			{
				float fh=(float)(-(j+1))*fHDist;
				DrawLine(float3(fWidth2,fh,0.0f),float3(-fWidth2,fh,0.0f), &cGrid);
				for(int y=1;y<5;y++)
				{
					fh=-(float)j*fHDist - (float)y * (fHDist * 0.2f);
					DrawLine(float3(fWidth2,fh,0.0f),float3(-fWidth2,fh,0.0f), &cSubGrid);
				}
			}
		}
	}
	VOID			Manager::DrawBoxTexture(const AABox& box, const unsigned int* colors/*24个*/, const float2* uvs/*24个*/, const matrix4x4* world)
	{CPUTimeT(Manager);
		Program* prog = this->SetRenderMode(Phantom::RenderMode_ColorTex1);
		PBoxVertices v;
		v.setup(box._min, box._max);
		float3	temp[8];
		for(int i=0;i<8;i++)
			temp[i]	=	v.v[i];
		float3	pos[24];
		int index	=	0;
        unsigned int colorTemp[24];
        if(!colors)
        {
            colors = colorTemp;
            for(int i=0;i<24;i++)
                colorTemp[i] = 0xffffffff;
        }
		for(int i=0;i<6;i++)
		{
			pos[i * 4 + 0] = temp[s_aabox_indices3[i * 6 + 0]];
			pos[i * 4 + 1] = temp[s_aabox_indices3[i * 6 + 1]];
			pos[i * 4 + 2] = temp[s_aabox_indices3[i * 6 + 2]];
			pos[i * 4 + 3] = temp[s_aabox_indices3[i * 6 + 4]];
		}
		SetWorldMatrix(world);
		m_device->DrawIndexed(m_currProg, DrawVInfos(24, pos, colors, uvs), s_aabox_indicesF, sizeof(s_aabox_indicesF) / sizeof(unsigned short), DrawMode_Triangles);
	}
	VOID			Manager::DrawTriangle(const PointColor& t1, const PointColor& t2, const PointColor& t3, const matrix4x4* world)
	{CPUTimeT(Manager);
		Program* prog = this->SetRenderMode(Phantom::RenderMode_Color);
		static float3 t[3];
		static unsigned int c[3];
		t[0]	=	t1.pos;
		t[1]	=	t2.pos;
		t[2]	=	t3.pos;
		c[0]	=	t1.c;
		c[1]	=	t2.c;
		c[2]	=	t3.c;
		SetWorldMatrix(world);
		m_device->DrawNoIndexed(m_currProg, DrawVInfos(3, t, c), 3, DrawMode_Triangles);
	}
	VOID			Manager::DrawLine(const PointColor& l1, const PointColor& l2, const matrix4x4* world)
	{CPUTimeT(Manager);
		Program* prog = this->SetRenderMode(Phantom::RenderMode_Color);
		float3 t[2];
		unsigned int c[2];
		t[0]	=	l1.pos;
		t[1]	=	l2.pos;
		c[0]	=	l1.c;
		c[1]	=	l2.c;
		SetWorldMatrix(world);
		m_device->DrawNoIndexed(m_currProg, DrawVInfos(2, t, c), 2, DrawMode_Lines);
	}
	VOID			Manager::SetWorldMatrix2D(const matrix4x4* world)
	{CPUTimeT(Manager);
		if(!m_currProg)
			return;
		if(world)
		{
			matrix4x4 temp;
			//temp = *world;
			matrixScaling(&temp, m_uiScale.x, m_uiScale.y, 0);

			float fZ	=	0;
			double fovY	=	m_fTan2DFov;
			double fovX	=	fovY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
			float fBackWidth = (float)this->m_bufferWidth;
			float fBackHeight = (float)this->m_bufferHeight;
			temp._41	=	((world->_41 / (float)fBackWidth)) * fovX;
			temp._42	=	( - (world->_42 / (float)fBackHeight)) * fovY;
			temp._41	+=	m_uiOffset.x;
			temp._42	+=	m_uiOffset.y;
			matrixMultiply(&temp, &temp, &m_viewProjection2D);
			m_currProg->SetMatrix(m_currProg->m_varMVP, &temp, "MVP");
		}
		else if(m_nSetMVP != 3 || m_pSetMVP != m_currProg)
		{
			m_currProg->SetMatrix(m_currProg->m_varMVP, &m_matrixDef2D, "MVP");
			m_pSetMVP			=	m_currProg;
			m_nSetMVP			=	3;
		}
	}
	VOID			Manager::SetWorldDlgMatrix2D(const matrix4x4* mat)	//设置对话框的矩阵
	{CPUTimeT(Manager);
		matrixMultiply(&m_viewProjection2D, &m_view2D, &m_projection2D);
		if(mat)
			matrixMultiply(&m_viewProjection2D, mat, &m_viewProjection2D);
		m_matrixDef2D.identity();
		matrixMultiply(&m_matrixDef2D, &m_matrixDef2D, &m_viewProjection2D);
		m_nSetMVP	=	-1;
	}
	float2			Manager::GetPixelToTixel(const Pixel& pixel)//0~width之间缩放比例
	{CPUTimeNone();
		float fZ	=	0;
		float fY	=	m_fTan2DFov;
		float fX	=	fY * (float)this->m_bufferWidth / (float)this->m_bufferHeight;
		return	float2((float)pixel.x / (float)m_bufferWidth * fX, (float)pixel.y / (float)m_bufferHeight * fY);
	}
	Pixel			Manager::GetTixelToPixel(const float2& pos)
	{CPUTimeNone();
		float fZ	=	0;
		float fY	=	m_fTan2DFov;
		float fX	=	fY * (float)this->m_bufferWidth / (float)this->m_bufferHeight;
		return Pixel((int)((pos.x / fX) * (float)m_bufferWidth), (int)((pos.y / fY) * (float)m_bufferHeight));
	}
	float2			Manager::ConvertPixelToTixel(const Pixel& pixel)
	{CPUTimeNone();
		float fZ	=	0;
		float fY	=	m_fTan2DFov;
		float fX	=	fY * (float)this->m_bufferWidth / (float)this->m_bufferHeight;
		return	float2(((float)pixel.x / (float)m_bufferWidth - 0.5f) * fX, (0.5f - (float)pixel.y / (float)m_bufferHeight) * fY);
	}
	Pixel			Manager::ConvertTixelToPixel(const float2& pos)
	{CPUTimeNone();
		float fZ	=	0;
		float fY	=	m_fTan2DFov;
		float fX	=	fY * (float)this->m_bufferWidth / (float)this->m_bufferHeight;
		return Pixel((int)((pos.x / fX + 0.5f) * (float)m_bufferWidth), (int)((0.5f - pos.y / fY) * (float)m_bufferHeight));
	}
	BOOL			Manager::DrawRectPoints2D(Texture* tex, const float2& leftTop, const float2& rightTop, const float2& leftBottom, const float2& rightBottom
		, const rect_f* newUV, const color4* c, const matrix4x4* world)
	{CPUTimeT(Manager);
		if(!tex)
			return false;
		rect_f texRc(0, 0, 1, 1);
		if(newUV)
			texRc	=	*newUV;
		//
		if(!SetRenderMode(RenderMode_ColorTex1))
			return true;
		m_device->SetCullVisible(CullMode_CW);
		float3 pos[4];
		float2 uv[4];
		unsigned int color[4];
		float fZ	=	0;
		float fY	=	m_fTan2DFov;
		float fX	=	fY * (float)this->m_bufferWidth / (float)this->m_bufferHeight;
		//
		pos[0].setxyz(leftTop.x, leftTop.y, fZ);
		pos[1].setxyz(rightTop.x, rightTop.y, fZ);
		pos[2].setxyz(rightBottom.x, rightBottom.y, fZ);
		pos[3].setxyz(leftBottom.x, leftBottom.y, fZ);
		for(int i=0;i<4;i++)
		{
			pos[i].x	=	(pos[i].x - 0.5f) * fX;
			pos[i].y	=	(0.5f - pos[i].y) * fY;
		}
		//
		uv[0]	=	float2(texRc.left, texRc.top);
		uv[1]	=	float2(texRc.right, texRc.top);
		uv[2]	=	float2(texRc.right, texRc.bottom);
		uv[3]	=	float2(texRc.left, texRc.bottom);
		unsigned int temp = 0xffffffff;
		if(c)
			temp	=	*c;
		for(int i=0;i<4;i++)
			color[i]	=	temp;
		if(tex)
			this->SetTexture(tex);
		SetWorldMatrix2D(world);
		static unsigned short indexBuffer[] ={3,1,0,3,2,1, 0,1,3,1,2,3};
		int numIndices	=	6;
		m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color, uv), indexBuffer, numIndices, DrawMode_Triangles);
		return true;
	}
	BOOL			Manager::Draw2D(Texture* tex, const Pixel* points, const float2* uvs, unsigned int * colors, int count, BOOL bHighlight, const matrix4x4* world, BOOL bEnabled, CullMode cull)
	{CPUTimeT(Manager);
		if(!tex || count <= 0)
			return false;
		if(bEnabled)
		{
			if(!SetRenderMode(RenderMode_ColorTex1))
				return false;
		}
		else
		{
			if(!SetRenderMode(RenderMode_ColorTex1Disable))
				return false;
		}
		m_device->SetCullVisible(cull);
		float fZ	=	0;
		double fY	=	m_fTan2DFov;
		double fX	=	fY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
		if(tex)
			this->SetTexture(tex);
		SetWorldMatrix2D(world);
		float3 pos[4];
		for(int i=0;i<4;i++)
		{
			float2 val;
			val.setxy((float)points[i].x * m_uiScale.x + m_uiOffset.x, (float)points[i].y * m_uiScale.y + m_uiOffset.y);
			val.x /= (float)m_bufferWidth;
			val.y /= (float)m_bufferHeight;
			pos[i].setxyz((val.x - 0.5f) * fX, (0.5f - val.y) * fY, fZ);
		}
		static unsigned short indexBuffer[] ={3,1,0,3,2,1};
		m_device->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_INVSRCALPHA);
		int numIndices	=	6;
		m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, colors, uvs), indexBuffer, numIndices, DrawMode_Triangles);
		if(bHighlight > 0)
		{
			m_device->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_ONE);
			m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, colors, uvs), indexBuffer, numIndices, DrawMode_Triangles);
		}
		return true;
	}
	BOOL			Manager::Draw2D(Texture* tex, const rect_f* rects, int rectCount, const Rect& texRcIn, const color4* c, const matrix4x4* world, BOOL bEnabled, CullMode cull)	//渲染2D图片
	{CPUTimeT(Manager);
		if(!tex || rectCount <= 0)
			return false;
		float tWidth	=	tex->GetWidth();
		float tHeight	=	tex->GetHeight();
		if(tWidth < 1.0f || tHeight < 1.0f)
			return false;
#ifdef DIRECT_X_9
		float offT	=	0.5f;
#else
		float offT	=	0;
#endif
		rect_f texRc((float)((float)texRcIn.left + offT) / tWidth, (float)((float)texRcIn.top + offT) / tHeight, (float)((float)texRcIn.right + offT) / tWidth, (float)((float)texRcIn.bottom + offT) / tHeight);
		//
		if(bEnabled)
		{
			if(!SetRenderMode(RenderMode_ColorTex1))
				return false;
		}
		else
		{
			if(!SetRenderMode(RenderMode_ColorTex1Disable))
				return false;
		}
		m_device->SetCullVisible(cull);
		float2 uv[4];
		unsigned int color[4];
		float fZ	=	0;
		double fY	=	m_fTan2DFov;
		double fX	=	fY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
		uv[0]	=	float2(texRc.left, texRc.top);
		uv[1]	=	float2(texRc.right, texRc.top);
		uv[2]	=	float2(texRc.right, texRc.bottom);
		uv[3]	=	float2(texRc.left, texRc.bottom);
		unsigned int temp = 0xffffffff;
		if(c)
			temp	=	*c;
		for(int i=0;i<4;i++)
			color[i]	=	temp;
		if(tex)
			this->SetTexture(tex);
		SetWorldMatrix2D(world);
		static unsigned short indexBuffer[] ={3,1,0,3,2,1, 0,1,3,1,2,3};
		int numIndices	=	6;
		float3 pos[4];
		for(int i=0;i<rectCount;i++)
		{
			const rect_f& screen	=	rects[i];
			float l	=	(screen.left - 0.5f) * fX;
			float t	=	(0.5f - screen.top) * fY;
			float r	=	(screen.right - 0.5f) * fX;
			float b	=	(0.5f - screen.bottom) * fY;
			//
			pos[0].setxyz(l, t, fZ);
			pos[1].setxyz(r, t, fZ);
			pos[2].setxyz(r, b, fZ);
			pos[3].setxyz(l, b, fZ);
			m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color, uv), indexBuffer, numIndices, DrawMode_Triangles);
		}
		return true;
	}
	BOOL			Manager::Draw2D(Texture* tex, const rect_f& screen, const rect_f& texRc, const color4* c, const matrix4x4* world, BOOL bEnabled, CullMode cull)	//渲染2D图片
	{CPUTimeT(Manager);
		if(bEnabled)
		{
			if(!SetRenderMode(RenderMode_ColorTex1))
				return false;
		}
		else
		{
			if(!SetRenderMode(RenderMode_ColorTex1Disable))
				return false;
		}
		m_device->SetCullVisible(cull);
		float3 pos[4];
		float2 uv[4];
		unsigned int color[4];
		float fZ	=	0;
		double fY	=	m_fTan2DFov;
		double fX	=	fY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
		double l	=	(screen.left - 0.5f) * fX;
		double t	=	(0.5f - screen.top) * fY;
		double r	=	(screen.right - 0.5f) * fX;
		double b	=	(0.5f - screen.bottom) * fY;
		//
		pos[0].setxyz(l, t, fZ);
		pos[1].setxyz(r, t, fZ);
		pos[2].setxyz(r, b, fZ);
		pos[3].setxyz(l, b, fZ);
		for(int i=0;i<4;i++)
		{
			//pos[i].x	*=	m_uiScale.x;
			//pos[i].y	*=	m_uiScale.y;
			//pos[i].x	+=	m_uiOffset.x;
			//pos[i].y	+=	m_uiOffset.y;
		}
		//
		uv[0]	=	float2(texRc.left, texRc.top);
		uv[1]	=	float2(texRc.right, texRc.top);
		uv[2]	=	float2(texRc.right, texRc.bottom);
		uv[3]	=	float2(texRc.left, texRc.bottom);
		unsigned int temp = 0xffffffff;
		if(c)
			temp	=	*c;
		for(int i=0;i<4;i++)
			color[i]	=	temp;
		if(tex)
			this->SetTexture(tex);
		SetWorldMatrix2D(world);
		static unsigned short indexBuffer[] ={3,1,0,3,2,1, 0,1,3,1,2,3};
		int numIndices	=	6;
		m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color, uv), indexBuffer, numIndices, DrawMode_Triangles);
		return true;
	}
	//

	BOOL			Manager::DrawTexts(RenderText* textPtr, BOOL bEnabled, Rect& drr, unsigned int dtFormat, const color4& c, const matrix4x4* world, int nHighlight)
	{CPUTimeT(Manager);
		if(!textPtr ||textPtr->IsEmpty()||!textPtr->IsInit())return false;
		m_device->SetCullVisible(Phantom::CullMode_CW);
		m_device->SetAlphaMode(Phantom::AlphaMode_Blend);
		m_device->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_INVSRCALPHA);
		if(textPtr->IsFigFont()){
			return static_cast<RenderTextFig*>(textPtr)->DrawTexts(drr, dtFormat, c, world, nHighlight);
		}
		if(bEnabled)
		{
			if(!SetRenderMode(RenderMode_ColorTex1))
				return false;
		}
		else
		{
			if(!SetRenderMode(RenderMode_ColorTex1Disable))
				return false;
		}
		Rect textRect = textPtr->GetTextureRect();
		int dw = drr.GetWidth();
		int dh = drr.GetHeight();
		int tw = textRect.GetWidth();
		int th = textRect.GetHeight();
		Rect temp = drr;
		if(dw!=tw)
		{
			if(dtFormat&Phantom::DrawTextAlign_RIGHT)
				drr.left = drr.right - tw;
			else if(dtFormat&Phantom::DrawTextAlign_CENTER)
				drr.left += (dw-tw)/2;
		}
		if(dh!=th)
		{
			if(dtFormat&Phantom::DrawTextAlign_BOTTOM)
				drr.top = drr.bottom - th;
			else if(dtFormat&Phantom::DrawTextAlign_VCENTER)
				drr.top += (dh-th)/2;
		}
		drr.right = drr.left + tw;
		drr.bottom = drr.top + th;
		if(drr.left < temp.left)
		{
			textRect.left += (temp.left - drr.left);
			drr.left = temp.left;
		}
		if(drr.right > temp.right)
		{
			textRect.right -= (drr.right - temp.right);
			drr.right = temp.right;
		}
		if(drr.top < temp.top)
		{
			textRect.top += (temp.top - drr.top);
			drr.top = temp.top;
		}
		if(drr.bottom > temp.bottom)
		{
			textRect.bottom -= (drr.bottom - temp.bottom);
			drr.bottom = temp.bottom;
		}
		rect_f screen((float)drr.left * m_uiScale.x / (float)this->m_bufferWidth, (float)drr.top * m_uiScale.y / (float)this->m_bufferHeight, (float)drr.right * m_uiScale.x / (float)this->m_bufferWidth, (float)drr.bottom * m_uiScale.y / (float)this->m_bufferHeight);
		int tWidth = textPtr->GetRealWidth();
		int tHeight= textPtr->GetRealHeight();
#ifdef DIRECT_X_9
		float offT	=	0.5f;
#else
		float offT	=	0;
#endif
		rect_f texRc((float)((float)textRect.left+offT) / tWidth, (float)((float)textRect.top+offT) / tHeight, (float)((float)textRect.right+offT) / tWidth, (float)((float)textRect.bottom +offT)/ tHeight);

		//
		float3 pos[4];
		float2 uv[4];
		unsigned int color[4];
		float fZ	=	0;
		double fY	=	m_fTan2DFov;
		double fX	=	fY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
		double l	=	(screen.left - 0.5f) * fX;
		double t	=	(0.5f - screen.top) * fY;
		double r	=	(screen.right - 0.5f) * fX;
		double b	=	(0.5f - screen.bottom) * fY;
		//
		pos[0].setxyz(l, t, fZ);
		pos[1].setxyz(r, t, fZ);
		pos[2].setxyz(r, b, fZ);
		pos[3].setxyz(l, b, fZ);
		uv[0]	=	float2(texRc.left, texRc.top);
		uv[1]	=	float2(texRc.right, texRc.top);
		uv[2]	=	float2(texRc.right, texRc.bottom);
		uv[3]	=	float2(texRc.left, texRc.bottom);
		if(world){
			for(int i=0;i<4;i++)
			{
				pos[i].transform(pos[i], *world);
			}
		}
		for(int i=0;i<4;i++)
			color[i]	=	c;
		if(m_currProg)
			m_currProg->SetTextTexture(Phantom::TextureIndex_0, textPtr);
		SetWorldMatrix2D(0);
		static unsigned short indexBuffer[] ={3,1,0,3,2,1, 0,1,3,1,2,3};
		int numIndices	=	6;
		m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color, uv), indexBuffer, numIndices, DrawMode_Triangles);
		if(nHighlight > 0)
		{
			m_device->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_ONE);
			m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color, uv), indexBuffer, numIndices, DrawMode_Triangles);
		}
		return true;
	}

	BOOL			Manager::DrawSprite2D(Texture* tex, const Rect& textureRc, const float3* center, const color4* c, const matrix4x4* world, const rect_f* scale, int nHighlight, BOOL bEnabled, BOOL bInvW, BOOL bInvH, BOOL dlgMatrix)	//渲染2D图片
	{CPUTimeT(Manager);
		if(bEnabled)
		{
			if(!SetRenderMode(RenderMode_ColorTex1))
				return false;
		}
		else
		{
			if(!SetRenderMode(RenderMode_ColorTex1Disable))
				return false;
		}
        if(dlgMatrix)
            m_device->SetCullVisible(CullMode_Double);
        else
            m_device->SetCullVisible(CullMode_CW);
		float textureWidth	=	(float)tex->GetWidth();
		float textureHeight	=	(float)tex->GetHeight();
		//
		float	left		=	0;
		float	top			=	0;
		if(world)
		{
			left		=	world->_41;
			top			=	world->_42;
		}
		else if(scale)
		{
			left		=	scale->right;
			top			=	scale->bottom;
		}
		float	right		=	left + (float)textureRc.GetWidth();
		float	bottom		=	top + (float)textureRc.GetHeight();
		//
		rect_f screen;
		if(center)
			screen.set(-center->x, -center->y, (float)textureRc.GetWidth() - center->x, (float)textureRc.GetHeight() - center->y);
		else
			screen.set(0, 0, textureRc.GetWidth(), textureRc.GetHeight());
		//
#ifdef DIRECT_X_9
		float off	=	0;
		float offT	=	0.5f;
#else
		float off	=	0;
		float offT	=	0;
#endif
		rect_f texRc((float)((float)textureRc.left + offT) / textureWidth, (float)((float)textureRc.top + offT) / textureHeight, (float)((float)textureRc.right + offT) / textureWidth, (float)((float)textureRc.bottom + offT) / textureHeight);
		float3 pos[4];
		float2 uv[4];
		unsigned int color[4];
		double fovY	=	m_fTan2DFov;
		double fovX	=	fovY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
		pos[0].setxyz(screen.left, screen.top, 0);
		pos[1].setxyz(screen.right, screen.top, 0);
		pos[2].setxyz(screen.right, screen.bottom, 0);
		pos[3].setxyz(screen.left, screen.bottom, 0);
        float uisx=m_uiScale.x;
        float uisy=m_uiScale.y;
		for(int i=0;i<4;i++)
		{
			float3& p	=	pos[i];
			if(world)
			{
				p.transform(p, *world);
			}
			else if(scale)
			{
				p.x	=	p.x * scale->left + scale->right+off;
				p.y	=	p.y * scale->top + scale->bottom+off;
			}
            p.x	*=	uisx;
            p.y	*=	uisy;
            p.x	+=	m_uiOffset.x;
            p.y	+=	m_uiOffset.y;
			p.x	=	((p.x / (float)m_bufferWidth) - 0.5f) * fovX;
			p.y	=	(0.5f - (p.y / (float)m_bufferHeight)) * fovY;
			p.z	=	0;
		}
		//
		if(bInvW)
		{
			float fTemp = texRc.left;
			texRc.left = texRc.right;
			texRc.right = fTemp;
		}
		if(bInvH)
		{
			float fTemp = texRc.top;
			texRc.top = texRc.bottom;
			texRc.bottom = fTemp;
		}
		uv[0]	=	float2(texRc.left, texRc.top);
		uv[1]	=	float2(texRc.right, texRc.top);
		uv[2]	=	float2(texRc.right, texRc.bottom);
		uv[3]	=	float2(texRc.left, texRc.bottom);
		unsigned int temp = 0xffffffff;
		if(c)
			temp	=	*c;
		for(int i=0;i<4;i++)
			color[i]	=	temp;
		if(tex)
			this->SetTexture(tex);
		SetWorldMatrix2D(0);
		static unsigned short indexBuffer[] ={3,1,0,3,2,1};
		int numIndices	=	6;
		m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color, uv), indexBuffer, numIndices, DrawMode_Triangles);
		if(nHighlight > 0)
		{
			for(int i=0;i<nHighlight;i++)
			{
				m_device->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_ONE);
				m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color, uv), indexBuffer, numIndices, DrawMode_Triangles);
			}
		}
		return true;
	}
	extern unsigned int	g_uvScaleIndex;
	BOOL			Manager::DrawSprite2D(BOOL bEnabled, const color4& c, const matrix4x4* world, int nHighlight, BOOL bDlgMatrix)
	{CPUTimeT(Manager);
		if(!SetRenderMode(RenderMode_ColorRect))
			return false;
        if(bDlgMatrix)	
            m_device->SetCullVisible(CullMode_Double);
        else
            m_device->SetCullVisible(CullMode_CW);
		static _shader_var var_uv_matrix, var_uv_det, var_uv_scale;
		m_currProg->SetMatrix(var_uv_matrix, world, "sUVMatrix");
		static unsigned int s_renderIdx = 0;
		if(s_renderIdx != g_manager.m_renderIdx){
			s_renderIdx = g_manager.m_renderIdx;
			//
			double fovY	=	g_manager.m_fTan2DFov;
			double fovX	=	fovY * (double)g_manager.m_bufferWidth / (double)g_manager.m_bufferHeight;
			float4 screenDet(g_manager.m_bufferWidth, g_manager.m_bufferHeight, fovX, fovY);
			m_currProg->SetFloat4(var_uv_det, screenDet, "sScreenDet");
		}
		static unsigned int s_uvScaleIndex = 0;
		if(g_uvScaleIndex != s_uvScaleIndex)
		{
			s_uvScaleIndex = g_uvScaleIndex;
			float4 uvScale(g_manager.m_uiScale.x,g_manager.m_uiScale.y,g_manager.m_uiOffset.x,g_manager.m_uiOffset.y);
			m_currProg->SetFloat4(var_uv_scale, uvScale, "sUIScale");
		}
		if(!bEnabled){
			color4 temp = c;
			DisableColor(temp);
			m_currProg->SetDiffuse(temp);
		}
		else
			m_currProg->SetDiffuse(c);
		g_manager.SetWorldMatrix2D(0);

		this->m_textureMesh->RenderSubset(m_currProg, 0);
		return true;
	}
	BOOL			Manager::SetTextMode(BOOL bEnabled, const float4& textureRc, const color4& c, const matrix4x4* world, BOOL bDlgMatrix)
	{
		if(bEnabled){
			if(!SetRenderMode(RenderMode_TextureRect))
				return false;
			static _shader_var var_uv_matrix, var_uv_det, var_uv_scale, var_uv;
			static unsigned int s_renderIdx = 0;
			static unsigned int s_uvScaleIndex = 0;
			m_currProg->SetMatrix(var_uv_matrix, world, "sUVMatrix");
			if(s_renderIdx != g_manager.m_renderIdx){
				s_renderIdx = g_manager.m_renderIdx;
				//
				double fovY	=	g_manager.m_fTan2DFov;
				double fovX	=	fovY * (double)g_manager.m_bufferWidth / (double)g_manager.m_bufferHeight;
				float4 screenDet(g_manager.m_bufferWidth, g_manager.m_bufferHeight, fovX, fovY);
				m_currProg->SetFloat4(var_uv_det, screenDet, "sScreenDet");
			}
			if(g_uvScaleIndex != s_uvScaleIndex)
			{
				s_uvScaleIndex = g_uvScaleIndex;
				float4 uvScale(g_manager.m_uiScale.x,g_manager.m_uiScale.y,g_manager.m_uiOffset.x,g_manager.m_uiOffset.y);
				m_currProg->SetFloat4(var_uv_scale, uvScale, "sUIScale");
			}
			m_currProg->SetFloat4(var_uv, textureRc, "sUV");
		}
		else{
			if(!SetRenderMode(RenderMode_TextureRectDisable))
				return false;
			static _shader_var var_uv_matrix, var_uv_det, var_uv_scale, var_uv;
			static unsigned int s_renderIdx = 0;
			static unsigned int s_uvScaleIndex = 0;
			m_currProg->SetMatrix(var_uv_matrix, world, "sUVMatrix");
			if(s_renderIdx != g_manager.m_renderIdx){
				s_renderIdx = g_manager.m_renderIdx;
				//
				double fovY	=	g_manager.m_fTan2DFov;
				double fovX	=	fovY * (double)g_manager.m_bufferWidth / (double)g_manager.m_bufferHeight;
				float4 screenDet(g_manager.m_bufferWidth, g_manager.m_bufferHeight, fovX, fovY);
				m_currProg->SetFloat4(var_uv_det, screenDet, "sScreenDet");
			}
			if(g_uvScaleIndex != s_uvScaleIndex)
			{
				s_uvScaleIndex = g_uvScaleIndex;
				float4 uvScale(g_manager.m_uiScale.x,g_manager.m_uiScale.y,g_manager.m_uiOffset.x,g_manager.m_uiOffset.y);
				m_currProg->SetFloat4(var_uv_scale, uvScale, "sUIScale");
			}
			m_currProg->SetFloat4(var_uv, textureRc, "sUV");
		}
        if(bDlgMatrix)
            m_device->SetCullVisible(CullMode_Double);
        else
            m_device->SetCullVisible(CullMode_CW);
		m_currProg->SetDiffuse(c);
		g_manager.SetWorldMatrix2D(0);
        return true;
	}
	BOOL			Manager::DrawSprite2D(Texture* tex, const Rect& textureRc, BOOL bEnabled, const color4& c, const matrix4x4* world, int nHighlight, BOOL bDlgMatrix)
	{CPUTimeT(Manager);
		//
		float fW = (float)tex->GetWidth();
		float fH = (float)tex->GetHeight();
		float4 trc((float)textureRc.left / fW, (float)textureRc.top / fH, (float)textureRc.right / fW, (float)textureRc.bottom / fH);
		if(!SetTextMode(bEnabled, trc, c, world, bDlgMatrix))
			return false;
		m_currProg->SetTexture(TextureIndex_0, tex);
		this->m_textureMesh->RenderSubset(m_currProg, 0);
		return true;
	}
	BOOL			Manager::DrawSprite2D(RenderText* t, const Rect& textureRc, BOOL bEnabled, const color4& c, const matrix4x4* world, int nHighlight, BOOL bDlgMatrix)
	{CPUTimeT(Manager);
		float fW = (float)t->GetRealWidth();
		float fH = (float)t->GetRealHeight();
		float4 trc((float)textureRc.left / fW, (float)textureRc.top / fH, (float)textureRc.right / fW, (float)textureRc.bottom / fH);
		if(!SetTextMode(bEnabled, trc, c, world, bDlgMatrix))
			return false;
		m_currProg->SetTextTexture(TextureIndex_0, t);
		this->m_textureMesh->RenderSubset(m_currProg, 0);
		return true;
	}
	BOOL			Manager::DrawSprite2D(RenderText* tex, const Rect& textureRc, const float3* center, const color4* c, const matrix4x4* world, const rect_f* scale, int nHighlight, BOOL bEnabled, BOOL dlgMatrix)	//渲染2D图片
	{CPUTimeT(Manager);
        if(dlgMatrix)
            m_device->SetCullVisible(CullMode_Double);
        else
            m_device->SetCullVisible(CullMode_CW);
		if(bEnabled)
		{
			if(!SetRenderMode(RenderMode_ColorTex1))
				return false;
		}
		else
		{
			if(!SetRenderMode(RenderMode_ColorTex1Disable))
				return false;
		}
		float textureWidth	=	(float)tex->GetRealWidth();
		float textureHeight	=	(float)tex->GetRealHeight();
		//
		rect_f drr;
		if(center)
			drr.set(-center->x, -center->y, (float)textureRc.GetWidth() - center->x, (float)textureRc.GetHeight() - center->y);
		else
			drr.set(0, 0, textureRc.GetWidth(), textureRc.GetHeight());
		//
#ifdef DIRECT_X_9
		float off	=	0;
		float offT	=	0.5f;
#else
		float off	=	0;
		float offT	=	0;
#endif
		rect_f texRc((float)((float)textureRc.left + offT) / textureWidth, (float)((float)textureRc.top + offT) / textureHeight, (float)((float)textureRc.right + offT+1) / textureWidth, (float)((float)textureRc.bottom + offT+1) / textureHeight);
		float3 pos[4];
		float2 uv[4];
		unsigned int color[4];
		double fovY	=	m_fTan2DFov;
		double fovX	=	fovY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
		pos[0].setxyz(drr.left + off, drr.top + off, 0);
		pos[1].setxyz(drr.right + off, drr.top + off, 0);
		pos[2].setxyz(drr.right + off, drr.bottom + off, 0);
		pos[3].setxyz(drr.left + off, drr.bottom + off, 0);
		for(int i=0;i<4;i++)
		{
			float3& p	=	pos[i];
			if(world)
			{
				p.transform(p, *world);
			}
			else if(scale)
			{
				p.x	=	p.x * scale->left*0.5f + scale->right;
				p.y	=	p.y * scale->top*0.5f + scale->bottom;
			}
            p.x	*=	m_uiScale.x;
            p.y	*=	m_uiScale.y;
            p.x	+=	m_uiOffset.x;
            p.y	+=	m_uiOffset.y;
			p.x	=	((p.x / (float)m_bufferWidth) - 0.5f) * fovX;
			p.y	=	(0.5f - (p.y / (float)m_bufferHeight)) * fovY;
			p.z	=	0;
		}
		uv[0]	=	float2(texRc.left, texRc.top);
		uv[1]	=	float2(texRc.right, texRc.top);
		uv[2]	=	float2(texRc.right, texRc.bottom);
		uv[3]	=	float2(texRc.left, texRc.bottom);
		unsigned int tempc = 0xffffffff;
		if(c)
			tempc	=	*c;
		for(int i=0;i<4;i++)
			color[i]	=	tempc;
		if(m_currProg)
			m_currProg->SetTextTexture(TextureIndex_0, tex);
		SetWorldMatrix2D(0);
		static unsigned short indexBuffer[] ={3,1,0,3,2,1};
		int numIndices	=	6;
		m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color, uv), indexBuffer, numIndices, DrawMode_Triangles);
		if(nHighlight > 0)
		{
			for(int i=0;i<nHighlight;i++)
			{
				m_device->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_ONE);
				m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color, uv), indexBuffer, numIndices, DrawMode_Triangles);
			}
		}
		return true;
	}
	
	BOOL			Manager::DrawSprite2D(const Rect& textureRc, const float3* center, const color4* c, const matrix4x4* world, const rect_f* scale, int nHighlight, BOOL bEnabled, BOOL bInvW, BOOL bInvH, BOOL dlgMatrix)	//渲染2D图片
	{CPUTimeT(Manager);
		if(!SetRenderMode(RenderMode_Color))
			return false;
        if(dlgMatrix)	
            m_device->SetCullVisible(CullMode_Double);
        else
            m_device->SetCullVisible(CullMode_CW);
		float	left		=	0;
		float	top			=	0;
		if(world)
		{
			left		=	world->_41;
			top			=	world->_42;
		}
		else if(scale)
		{
			left		=	scale->right;
			top			=	scale->bottom;
		}
		float	right		=	left + (float)textureRc.GetWidth();
		float	bottom		=	top + (float)textureRc.GetHeight();
		//
		rect_f screen;
		if(center)
			screen.set(-center->x, -center->y, (float)textureRc.GetWidth() - center->x, (float)textureRc.GetHeight() - center->y);
		else
			screen.set(0, 0, textureRc.GetWidth(), textureRc.GetHeight());
		//
#ifdef DIRECT_X_9
		float off	=	0;
		float offT	=	0.5f;
#else
		float off	=	0;
		float offT	=	0;
#endif
		float3 pos[4];
		unsigned int color[4];
		double fovY	=	m_fTan2DFov;
		double fovX	=	fovY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
		pos[0].setxyz(screen.left, screen.top, 0);
		pos[1].setxyz(screen.right, screen.top, 0);
		pos[2].setxyz(screen.right, screen.bottom, 0);
		pos[3].setxyz(screen.left, screen.bottom, 0);
        float uisx=m_uiScale.x;
        float uisy=m_uiScale.y;
		for(int i=0;i<4;i++)
		{
			float3& p	=	pos[i];
			if(world)
			{
				p.transform(p, *world);
			}
			else if(scale)
			{
				p.x	=	p.x * scale->left + scale->right+off;
				p.y	=	p.y * scale->top + scale->bottom+off;
			}
            p.x	*=	uisx;
            p.y	*=	uisy;
            p.x	+=	m_uiOffset.x;
            p.y	+=	m_uiOffset.y;
			p.x	=	((p.x / (float)m_bufferWidth) - 0.5f) * fovX;
			p.y	=	(0.5f - (p.y / (float)m_bufferHeight)) * fovY;
			p.z	=	0;
		}
		unsigned int temp = 0xffffffff;
		if(c)
			temp	=	*c;
		for(int i=0;i<4;i++)
			color[i]	=	temp;
		SetWorldMatrix2D(0);
		static unsigned short indexBuffer[] ={3,1,0,3,2,1};
		int numIndices	=	6;
		m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color), indexBuffer, numIndices, DrawMode_Triangles);
		if(nHighlight > 0)
		{
			for(int i=0;i<nHighlight;i++)
			{
				m_device->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_ONE);
				m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color), indexBuffer, numIndices, DrawMode_Triangles);
			}
		}
		return true;
	}
VOID		Manager::DrawCircle( const Pixel& center, float fRadius, const color4& c, int smooth, BOOL bFill)//渲染一个圆
{CPUTimeT(Manager);
	INT nNumPoints	=	smooth + 1;
	if(nNumPoints <= 1)return;
	if(!SetRenderMode(RenderMode_Color))
		return;
	//
	m_tempPosList.SetArrayCount(nNumPoints + 1);
	m_tempColorList.SetArrayCount(nNumPoints + 1);
	double fZ	=	0;
	double fY	=	m_fTan2DFov;
	double fX	=	fY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
	unsigned int temp	=	c;
	for( unsigned int i=0; i < smooth; i++ )
	{
		float f = (float)i/(float)smooth * 2.0f * PI;
		float vx = cosf(f)*fRadius + center.x;
		float vy = sinf(f)*fRadius + center.y;
		float x	=	(float)vx / (float)this->m_bufferWidth;
		float y	=	(float)vy / (float)this->m_bufferHeight;
		x	*=	m_uiScale.x;
		y	*=	m_uiScale.y;
		x	+=	m_uiOffset.x;
		y	+=	m_uiOffset.y;
		m_tempPosList[i].x	=	(x - 0.5f) * fX;
		m_tempPosList[i].y	=	(0.5f - y) * fY;
		m_tempPosList[i].z	=	fZ;
		m_tempColorList[i]	=	temp;
	}
	m_tempPosList[smooth] = m_tempPosList[0];
	m_tempColorList[smooth] = m_tempColorList[0];
	SetWorldMatrix2D(0);
	if(bFill)
		m_device->DrawNoIndexed(m_currProg, DrawVInfos(nNumPoints, m_tempPosList.GetPtr(), m_tempColorList.GetPtr()), nNumPoints, DrawMode_TriangleFan);
	else
		m_device->DrawNoIndexed(m_currProg, DrawVInfos(nNumPoints, m_tempPosList.GetPtr(), m_tempColorList.GetPtr()), nNumPoints, DrawMode_LineStrip);
}

	VOID			Manager::DrawLines2D(const Pixel* xys, int count, const color4& c, BOOL bFill/*是否填充*/)	//渲染2D线条
	{CPUTimeT(Manager);
		if(!SetRenderMode(RenderMode_Color))
			return;
		m_device->SetCullVisible(CullMode_CW);
		m_tempPosList.SetArrayCount(count);
		m_tempColorList.SetArrayCount(count);
		double fZ	=	0;
		double fY	=	m_fTan2DFov;
		double fX	=	fY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
		unsigned int temp	=	c;
		for(int i=0;i<count;i++)
		{
			float x	=	(float)xys[i].x / (float)this->m_bufferWidth;
			float y	=	(float)xys[i].y / (float)this->m_bufferHeight;
			x	*=	m_uiScale.x;
			y	*=	m_uiScale.y;
			x	+=	m_uiOffset.x;
			y	+=	m_uiOffset.y;
			m_tempPosList[i].x	=	(x - 0.5f) * fX;
			m_tempPosList[i].y	=	(0.5f - y) * fY;
			m_tempPosList[i].z	=	fZ;
			m_tempColorList[i]	=	temp;
		}
		SetWorldMatrix2D(0);
		if(bFill)
			m_device->DrawNoIndexed(m_currProg, DrawVInfos(count, m_tempPosList.GetPtr(), m_tempColorList.GetPtr()), count, DrawMode_TriangleFan);
		else
			m_device->DrawNoIndexed(m_currProg, DrawVInfos(count, m_tempPosList.GetPtr(), m_tempColorList.GetPtr()), count, DrawMode_LineStrip);
	}
	VOID			Manager::DrawLines2DColor(const PixelColor* xys, int count, BOOL bFill/*是否填充*/)	//渲染2D线条
	{CPUTimeT(Manager);
		if(!SetRenderMode(RenderMode_Color))
			return;
		m_device->SetCullVisible(CullMode_CW);
		m_tempPosList.SetArrayCount(count);
		m_tempColorList.SetArrayCount(count);
		double fZ	=	0;
		double fY	=	m_fTan2DFov;
		double fX	=	fY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
		for(int i=0;i<count;i++)
		{
			float x	=	(float)xys[i].x / (float)this->m_bufferWidth;
			float y	=	(float)xys[i].y / (float)this->m_bufferHeight;
			x	*=	m_uiScale.x;
			y	*=	m_uiScale.y;
			x	+=	m_uiOffset.x;
			y	+=	m_uiOffset.y;
			m_tempPosList[i].x	=	(x - 0.5f) * fX;
			m_tempPosList[i].y	=	(0.5f - y) * fY;
			m_tempPosList[i].z	=	fZ;
			m_tempColorList[i]	=	xys[i].color;
		}
		SetWorldMatrix2D(0);
		if(bFill)
			m_device->DrawNoIndexed(m_currProg, DrawVInfos(count, m_tempPosList.GetPtr(), m_tempColorList.GetPtr()), count, DrawMode_TriangleFan);
		else
			m_device->DrawNoIndexed(m_currProg, DrawVInfos(count, m_tempPosList.GetPtr(), m_tempColorList.GetPtr()), count, DrawMode_LineStrip);
	}
	VOID			Manager::DrawRect2D(const Rect& rc, const color4& c, BOOL bFill/*是否填充*/, BOOL bLingXing)	//渲染2D线条
	{CPUTimeT(Manager);
		if(!SetRenderMode(RenderMode_Color))
			return;
		m_device->SetCullVisible(CullMode_CW);
		rect_f screen((float)rc.left / (float)this->m_bufferWidth, (float)rc.top / (float)this->m_bufferHeight,
			(float)rc.right / (float)this->m_bufferWidth, (float)rc.bottom / (float)this->m_bufferHeight);
		float3 pos[4];
		unsigned int color[4];
		double fZ	=	0;
		double fY	=	m_fTan2DFov;
		double fX	=	fY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
		double l	=	(screen.left - 0.5f) * fX;
		double t	=	(0.5f - screen.top) * fY;
		double r	=	(screen.right - 0.5f) * fX;
		double b	=	(0.5f - screen.bottom) * fY;
		//
		if(bLingXing){
			float cy = (t+b)/2;
			float cx = (l+r)/2;
			pos[0].setxyz(cx, t, fZ);
			pos[1].setxyz(r, cy, fZ);
			pos[2].setxyz(cx, b, fZ);
			pos[3].setxyz(l, cy, fZ);
		}
		else{
			pos[0].setxyz(l, t, fZ);
			pos[1].setxyz(r, t, fZ);
			pos[2].setxyz(r, b, fZ);
			pos[3].setxyz(l, b, fZ);
		}
		for(int i=0;i<4;i++)
		{
			pos[i].x	*=	m_uiScale.x;
			pos[i].y	*=	m_uiScale.y;
			pos[i].x	+=	m_uiOffset.x;
			pos[i].y	+=	m_uiOffset.y;
		}
		unsigned int temp = c;
		for(int i=0;i<4;i++)
			color[i]	=	temp;
		SetWorldMatrix2D(0);
		static unsigned short indexBuffer[] ={3,1,0,3,2,1};
		static unsigned short indexBufferLine[] ={0,1,1,2,2,3,3,0};
		if(bFill)
			m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color), indexBuffer, 6, DrawMode_Triangles);
		else
			m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color), indexBufferLine, 8, DrawMode_Lines);
	}
	inline	void			ScreenToRender(float3& ret, const screenVertex3D& in, float fovX, float fovY, float fBackWidth, float fBackHeight)
	{
		ret.x	=	((in.x / (float)fBackWidth) - 0.5f) * fovX;
		ret.y	=	(0.5f - (in.y / (float)fBackHeight)) * fovY;
		ret.z	=	0;
	}
	VOID			Manager::SetupScreenPosList(float3* inOut, int numVertices)
	{CPUTimeNone();
		float fZ	=	0;
		double fovY	=	m_fTan2DFov;
		double fovX	=	fovY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
		double fBackWidth = (double)this->m_bufferWidth;
		double fBackHeight = (double)this->m_bufferHeight;
		for(int i=0;i<numVertices;i++)
		{
			float3& ret = inOut[i];
			float3 in = inOut[i];
			ret.x	=	((in.x / (float)fBackWidth) - 0.5f) * fovX;
			ret.y	=	(0.5f - (in.y / (float)fBackHeight)) * fovY;
			ret.z	=	0;
			ret.x	*=	m_uiScale.x;
			ret.y	*=	m_uiScale.y;
			ret.x	+=	m_uiOffset.x;
			ret.y	+=	m_uiOffset.y;
		}
	}
	VOID			Manager::SetupScreenPos(float3& inOut)
	{CPUTimeNone();
		float fZ	=	0;
		float fovY	=	m_fTan2DFov;
		float fovX	=	fovY * (float)this->m_bufferWidth / (float)this->m_bufferHeight;
		float fBackWidth = (float)this->m_bufferWidth;
		float fBackHeight = (float)this->m_bufferHeight;
		float3& ret = inOut;
		ret.x	*=	m_uiScale.x;
		ret.y	*=	m_uiScale.y;
		ret.x	+=	m_uiOffset.x;
		ret.y	+=	m_uiOffset.y;
		ret.x	=	((ret.x / (float)fBackWidth) - 0.5f) * fovX;
		ret.y	=	(0.5f - (ret.y / (float)fBackHeight)) * fovY;
		ret.z	=	0;
	}
	VOID			Manager::DrawTriangle2D(Texture* tex, const screenVertex3D& t1, const screenVertex3D& t2, const screenVertex3D& t3, BOOL bFill)
	{CPUTimeT(Manager);
		if(tex){
			if(!SetRenderMode(RenderMode_ColorTex1))
				return;
		}
		else
		{
			if(!SetRenderMode(RenderMode_Color))
				return;
		}
		m_device->SetCullVisible(CullMode_CW);
		float3 pos[3];
		float2 uv[3];
		unsigned int color[3];
		float fZ	=	0;
		double fY	=	m_fTan2DFov;
		double fX	=	fY * (double)this->m_bufferWidth / (double)this->m_bufferHeight;
		//ScreenToRender(pos[0], t1, fX, fY, (float)this->m_bufferWidth, (float)this->m_bufferHeight);
		//ScreenToRender(pos[1], t2, fX, fY, (float)this->m_bufferWidth, (float)this->m_bufferHeight);
		//ScreenToRender(pos[2], t3, fX, fY, (float)this->m_bufferWidth, (float)this->m_bufferHeight);
		pos[0].setxyz(t1.x, t1.y, t1.z);
		pos[1].setxyz(t2.x, t2.y, t2.z);
		pos[2].setxyz(t3.x, t3.y, t3.z);
		for(int i=0;i<3;i++)
		{
			pos[i].x	*=	m_uiScale.x;
			pos[i].y	*=	m_uiScale.y;
			pos[i].x	+=	m_uiOffset.x;
			pos[i].y	+=	m_uiOffset.y;
			pos[i].x	=	((pos[i].x / (float)m_bufferWidth) - 0.5f) * fX;
			pos[i].y	=	(0.5f - (pos[i].y / (float)m_bufferHeight)) * fY;
			pos[i].z	=	0;
		}
		//
		uv[0].setxy(t1.tu, t1.tv);
		uv[1].setxy(t2.tu, t2.tv);
		uv[2].setxy(t3.tu, t3.tv);
		color[0]	=	t1.color;
		color[1]	=	t2.color;
		color[2]	=	t3.color;
		if(tex)
			this->SetTexture(tex);
		SetWorldMatrix2D(0);
		static unsigned short indexBuffer[] ={2,1,0};
		float2* puv = uv;
		if(!tex)
			puv = 0;
		m_device->DrawIndexed(m_currProg, DrawVInfos(3, pos, color, puv), indexBuffer, 3, DrawMode_Triangles);
	}
	VOID			Manager::DrawTriangleList2D(Texture* tex, DrawVInfos& d, int numVertices, BOOL bFill)
	{CPUTimeT(Manager);
		if(!SetRenderMode(RenderMode_ColorTex1))
			return;
		m_device->SetCullVisible(CullMode_CW);
		if(tex)
			this->SetTexture(tex);
		SetWorldMatrix2D(0);
		m_device->DrawNoIndexed(m_currProg, d, numVertices, Phantom::DrawMode_TriangleStrip);
	}
	BOOL			Manager::Draw2D(Texture* t, const Rect& screen, const rect_f& texRc, const color4* c, const matrix4x4* world, BOOL bTo2DConv, BOOL bEnabled, CullMode cull)
	{CPUTimeNone();
		Rect drr;
		if(bTo2DConv)
		{
			drr.Set((float)screen.left * this->m_uiScale.x, (float)screen.top * this->m_uiScale.y, (float)screen.right * this->m_uiScale.x, (float)screen.bottom * this->m_uiScale.y);
			drr.Offset(this->m_uiOffset.x, this->m_uiOffset.y);
		}
		else
			drr = screen;
		return Draw2D(t, rect_f((float)drr.left / (float)this->m_bufferWidth, (float)drr.top / (float)this->m_bufferHeight, (float)drr.right / (float)this->m_bufferWidth, (float)drr.bottom / (float)this->m_bufferHeight), texRc, c, world, bEnabled, cull);
	}
	BOOL			Manager::Draw2D(Texture* t, const rect_f& screen, const Rect& texRc, const color4* c, const matrix4x4* world, BOOL bEnabled, CullMode cull)
	{CPUTimeNone();
		if(!t)
			return false;
		float tWidth	=	t->GetWidth();
		float tHeight	=	t->GetHeight();
		if(tWidth < 1.0f || tHeight < 1.0f)
			return false;
#ifdef DIRECT_X_9
		float offT	=	0.5f;
#else
		float offT	=	0;
#endif
		return Draw2D(t, screen, rect_f((float)((float)texRc.left+offT) / tWidth, (float)((float)texRc.top+offT) / tHeight, (float)((float)texRc.right+offT) / tWidth, (float)((float)texRc.bottom +offT)/ tHeight), c, world, bEnabled, cull);
	}
	BOOL			Manager::Draw2D(Texture* t, const Rect& screen, const Rect& texRc, const color4* c, const matrix4x4* world, BOOL bTo2DConv, BOOL bEnabled, CullMode cull)
	{CPUTimeNone();
		if(!t)
			return false;
		float tWidth	=	t->GetWidth();
		float tHeight	=	t->GetHeight();
		if(tWidth < 1.0f || tHeight < 1.0f)
			return false;
		Rect drr;
		if(bTo2DConv)
		{
			drr.Set(screen.left * this->m_uiScale.x, screen.top * this->m_uiScale.y, screen.right * this->m_uiScale.x, screen.bottom * this->m_uiScale.y);
			drr.Offset(this->m_uiOffset.x, this->m_uiOffset.y);
		}
		else
			drr = screen;
#ifdef DIRECT_X_9
		float offT	=	0.5f;
#else
		float offT	=	0;
#endif
		return Draw2D(t, rect_f((float)drr.left / (float)this->m_bufferWidth, (float)drr.top / (float)this->m_bufferHeight, (float)drr.right / (float)this->m_bufferWidth, (float)drr.bottom / (float)this->m_bufferHeight), 
			rect_f((float)((float)texRc.left+offT) / tWidth, (float)((float)texRc.top+offT) / tHeight, (float)((float)texRc.right+offT) / tWidth, (float)((float)texRc.bottom +offT)/ tHeight), c, world, bEnabled, cull);
	}
	BOOL			Manager::DrawTexts(const char* szUTF8, Rect& destRect, int nLength, float fontSize, float fItalic, const color4* c, unsigned int dtFormat, const matrix4x4* world)
	{CPUTimeNone();
        if(!m_tempText)
            m_tempText = new RenderTextFig();//
        m_tempText->SetFontSize(fontSize);
        m_tempText->SetFontStyle(0);
        m_tempText->SetText(szUTF8);
        color4 temp(1,1,1,1);
        if(!c)c=&temp;
        return DrawTexts(m_tempText, true, destRect, dtFormat, *c, world,0);
	}
	BOOL			Manager::Draw3D(Texture* tex, const float3* pos, const float2* uvs, unsigned int* colors, const matrix4x4* world, CullMode cull)
	{CPUTimeT(Manager);
		if(tex){
			if(!SetRenderMode(RenderMode_Default))
				return false;
		}
		else
		{
			if(!SetRenderMode(RenderMode_Color))
				return false;
		}
		m_device->SetCullVisible(cull);
		if(!GetCameraPtr()->IsPlaneVisible(pos[0], pos[1], pos[2], pos[3], false))
			return false;
		if(tex)
			this->SetTexture(tex);
		if(world)
			SetWorldMatrix(world);
		else
			SetWorldMatrix(&matrix4x4::s_identity());
		static unsigned short indexBuffer[] ={3,1,0,3,2,1, 0,1,3,1,2,3};
		int numIndices	=	6;
		m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, colors, uvs), indexBuffer, numIndices, DrawMode_Triangles);
		return true;
	}
	BOOL			Manager::Draw3D(Texture* tex, const Rect& xyRect, const float2* uvs, const color4* c, float* heights, const matrix4x4* world, CullMode cull)
	{CPUTimeT(Manager);
		if(!SetRenderMode(RenderMode_Default))
			return false;
		m_device->SetCullVisible(cull);
		float3 pos[4];
		unsigned int color[4];
		pos[0].setxyz(xyRect.left, xyRect.top, 0);
		pos[1].setxyz(xyRect.right, xyRect.top, 0);
		pos[2].setxyz(xyRect.right, xyRect.bottom, 0);
		pos[3].setxyz(xyRect.left, xyRect.bottom, 0);
		if(heights)
		{
			for(int i=0;i<4;i++){
				pos[i].z = heights[i];
			}
		}
		if(!GetCameraPtr()->IsPlaneVisible(pos[0], pos[1], pos[2], pos[3]))
			return false;
		unsigned int temp = 0xffffffff;
		if(c)
			temp	=	*c;
		for(int i=0;i<4;i++)
			color[i]	=	temp;
		if(tex)
			this->SetTexture(tex);
		if(world)
			SetWorldMatrix(world);
		else
			SetWorldMatrix(&matrix4x4::s_identity());
		static unsigned short indexBuffer[] ={3,1,0,3,2,1, 0,1,3,1,2,3};
		int numIndices	=	6;
		m_device->DrawIndexed(m_currProg, DrawVInfos(4, pos, color, uvs), indexBuffer, numIndices, DrawMode_Triangles);
		return true;
	}
	BOOL			Manager::Draw3D(Texture* tex, const Rect& xyRect, const rect_f& texRc, const color4* c, float* heights, const matrix4x4* world, CullMode cull)
	{CPUTimeNone();
		float2 uv[4];
		uv[0]	=	float2(texRc.left, texRc.top);
		uv[1]	=	float2(texRc.right, texRc.top);
		uv[2]	=	float2(texRc.right, texRc.bottom);
		uv[3]	=	float2(texRc.left, texRc.bottom);
		return Draw3D(tex, xyRect, uv, c, heights, world, cull);
	}
};
