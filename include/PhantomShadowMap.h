//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
	Design Writer :   赵德贤 Dexian Zhao
	Email: yuzhou_995@hotmail.com
	 
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOMSHADOWMAP_H__
#define __PHANTOMSHADOWMAP_H__
#include "PhantomBase.h"
#include "PhantomCamera.h"
#include "PhantomDevice.h"

namespace Phantom{
	
	class ShadowMap:public Base
	{
	public:
		ShadowMap(void);
		~ShadowMap(void);
		BOOL							Create(INT nMapSize = 512);	//初始化阴影渲染
		VOID							Destroy();
		//
		BOOL							BeginShadow(const float3& lightPos, const float3& lightDir, float fMaxViewDistance, float fBias = 1.0f);
		VOID							EndShadow();
		//
		inline	BOOL					IsInit(){return (m_shadowMapTarget != NULL);}
		inline	RenderTarget*			GetRenderTarget(){return m_shadowMapTarget;}
		inline	float					GetShadowMapSize(){return m_fShadowMapSize;}
		inline	const matrix4x4&		GetToLightProjMatrix(){return m_toLightProj;}	//返回转换成灯光坐标系的矩阵
		inline	const float				GetLightDistanceFar(){return this->m_fLightDistanceFar;}	//中心点
		inline	const float3&			GetLightPosition(){return m_shadowMiddlePos;}
		unsigned int					m_shadowUpdateIdx;
	protected:
		RenderTarget*					m_shadowMapTarget;
		float							m_fShadowMapSize;
		matrix4x4						m_lightProjection;
		matrix4x4						m_saveProjection;
		matrix4x4						m_lightMatrix;
		matrix4x4						m_toLightProj;
		//
		float							m_fLightDistanceFar;
		bool							m_bIsProjection;
		float							m_fSaveZNear, m_fSaveZFar, m_fSaveWidth, m_fSaveHeight, m_fSaveFov;
		float3							m_saveEye, m_saveTarget;
		matrix4x4						m_saveView;
		float3							m_shadowMiddlePos;
		//
		friend class					Program;
	};
};

#endif

