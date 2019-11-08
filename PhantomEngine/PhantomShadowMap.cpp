#include "PhantomShadowMap.h"
#include "PhantomManager.h"

namespace Phantom{

	ShadowMap::ShadowMap(void)
	{
		m_shadowMapTarget		=	0;
		m_bIsProjection			=	false;
		m_fSaveZNear			=	1.0f;
		m_fSaveZFar				=	10.0f;
		m_fSaveWidth			=	1.0f;
		m_fSaveHeight			=	1.0f;
		m_fSaveFov				=	1.0f;
		m_fLightDistanceFar		=	0;
		m_shadowUpdateIdx		=	0;
		m_lightProjection.identity();
	}

	ShadowMap::~ShadowMap(void)
	{
		Destroy();
	}
	extern int	GetNearestSize(int n);
	BOOL	ShadowMap::Create(INT nMapSize)	//初始化阴影渲染
	{CPUTime(ShadowMap);
		nMapSize = GetNearestSize(nMapSize);
		if(nMapSize <= 32)
			return FALSE;
		if(m_shadowMapTarget)
		{
			//如果已经创建则无须重复创建
			if(nMapSize == (int)m_fShadowMapSize)
				return TRUE;
		}
		m_fShadowMapSize	=	(float)nMapSize;
		if(!m_shadowMapTarget)
			m_shadowMapTarget	=	g_manager->CreateShadowMap();
		return m_shadowMapTarget->Create(m_fShadowMapSize, m_fShadowMapSize);
	}
	BOOL	ShadowMap::BeginShadow(const float3& lightPos, const float3& lightDir, float fMaxViewDistance, float fBias)
	{CPUTime(ShadowMap);
		BOOL bIsFirst		=	true;
		Camera* c			=	GetCameraPtr();
		if(!m_shadowMapTarget)
			return false;
		if(!g_manager.m_shadowMapViewMode){
			if(!m_shadowMapTarget->Begin())
				return false;
            //g_manager->Clear(CLEAR_TARGET | CLEAR_ZBUFFER);
		}
		m_saveEye				=	c->GetEye();
		m_saveTarget			=	c->GetTarget();
		m_bIsProjection			=	c->IsPerspectiveProjection();
		m_fSaveZNear			=	c->GetZNearest();
		m_fSaveZFar				=	c->GetZFar();
        m_fSaveWidth			=	c->GetWidth();
        m_fSaveHeight			=	c->GetHeight();
		m_saveView				=	c->m_matView;
//#ifdef IPHONE_OS
//        m_fSaveWidth            *=  2.0f;
//        m_fSaveHeight           *=  2.0f;
//#endif
		m_fSaveFov				=	c->GetFov();
		//
		float fMaxDistance		=	1.0f;
		
		//ClearScreen(*(DWORD*)&fMaxDistance);
		//------------------------------------------------------------------------------------------------------------------------------------------------------
		float fLightViewDistance	=	fMaxViewDistance * fBias;
		float3	ldir				=	lightDir;
		ldir.normalize();
		//
		float3	lightEye			=	lightPos + ldir * (fMaxViewDistance * 1.5f);
		static float fLL			=	0.32f;
		m_fLightDistanceFar			=	fLightViewDistance * fLL;
		//------------------------------------------------------------------------------------------------------------------------------------------------------
		c->SetPerspectiveProjection(PI*0.25f, m_fSaveWidth, m_fSaveHeight, m_fSaveZNear, m_fSaveZFar);
		c->SetOrthoProjection(fLightViewDistance, fLightViewDistance, 0.01f, fMaxViewDistance * 100.0f);
		c->SetViewMatrix(&lightEye, &lightPos, 0);
		m_lightProjection			=	c->GetProjMatrix();
		m_shadowMiddlePos			=	lightPos;
		m_lightMatrix				=	c->GetViewMatrix();
		m_shadowUpdateIdx++;
		return TRUE;
	}

	VOID	ShadowMap::EndShadow()
	{CPUTime(ShadowMap);
		Camera* c			=	GetCameraPtr();
		if(!m_shadowMapTarget)
			return;
		if(!g_manager.m_shadowMapViewMode)
			m_shadowMapTarget->End();
		//
		c->m_cameraData.vTarget = m_saveTarget;
		c->m_cameraData.vEye = m_saveEye;
		c->SetViewMatrix(m_saveView);
		if(!m_bIsProjection)
			c->SetOrthoProjection(m_fSaveWidth, m_fSaveHeight, m_fSaveZNear, m_fSaveZFar);
		else
			c->SetPerspectiveProjection(m_fSaveFov, m_fSaveWidth, m_fSaveHeight, m_fSaveZNear, m_fSaveZFar);
		matrixInverse(&m_toLightProj, NULL, &c->GetViewMatrix());
		matrixMultiply(&m_toLightProj, &m_toLightProj, &m_lightMatrix);
		matrixMultiply(&m_toLightProj, &m_toLightProj, &m_lightProjection);
		matrixMultiply(&m_toLightProj, &c->GetViewMatrix(), &m_toLightProj);
	}

	VOID	ShadowMap::Destroy()
	{CPUTime(ShadowMap);
		safe_release(m_shadowMapTarget);
	}

	//VOID	ShadowMap::CommitToShader(Program* prog)	//将贴图和各种参数信息写入到所有的shader
	//{
		//if(!m_shadowMapTarget)return;
		//float texelPos	=	1.0f / m_fShadowMapSize;
		//const char* viewToLightProj	=	"m_viewToLightProj";
		//const char* shadowMapLevel = "m_shadowMapLevel";
		//const char* shadowLightPosition = "m_shadowLightPosition";
		//const char* lightDistanceFar = "m_lightDistanceFar";
		//for(int i=0;i<GetRenderDevicePtr()->GetShaderCount();i++)
		//{
		//	GetRenderDevicePtr()->GetShaderPtr(i)->SetMatrix(m_shaderMatrixVar[i], m_toLightProj, viewToLightProj[nLevel]);
		//	GetRenderDevicePtr()->GetShaderPtr(i)->SetTexture(m_shaderMapTexture[i], m_shadowMapTarget->GetTargetTexture(), shadowMapLevel[nLevel]);//"m_shadowMapLevel_1");
		//	GetRenderDevicePtr()->GetShaderPtr(i)->SetFloat3(m_shadowLightPosition[i], m_shadowMiddlePos, shadowLightPosition[nLevel]);//, "m_shadowLightPosition");
		//	GetRenderDevicePtr()->GetShaderPtr(i)->SetFloat(m_varLightDistanceFar[i], this->m_fLightDistanceFar, lightDistanceFar[nLevel]);//, "m_shadowLightPosition");
		//	GetRenderDevicePtr()->GetShaderPtr(i)->SetFloat(m_shadowMapSizeInv[i], 1.0f / m_fShadowMapSize, "m_shadowMapSizeInv");
		//}
	//}
};

