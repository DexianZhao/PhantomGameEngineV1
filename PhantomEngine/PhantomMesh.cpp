//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomMesh.h"
#include "PhantomManager.h"
#include "zlib.h"
#include "encrypt.h"

namespace Phantom{

#define MAX_EMITTER_INDEX	(512*6)
unsigned short* 	GetCWIndexBuffer()
{
	static unsigned short nCWIndexBuffer[MAX_EMITTER_INDEX];
	static BOOL bInit = false;
	if(!bInit){
		bInit = true;
		for( int i = 0; i < MAX_EMITTER_INDEX/6; i++ )
		{
			//nCWIndexBuffer[i*6+0] = i*4+1;
			//nCWIndexBuffer[i*6+1] = i*4+0;
			//nCWIndexBuffer[i*6+2] = i*4+2;

			//nCWIndexBuffer[i*6+3] = i*4+2;
			//nCWIndexBuffer[i*6+4] = i*4+3;
			//nCWIndexBuffer[i*6+5] = i*4+1;

			nCWIndexBuffer[i*6+0] = i*4+0;
			nCWIndexBuffer[i*6+1] = i*4+1;
			nCWIndexBuffer[i*6+2] = i*4+2;

			nCWIndexBuffer[i*6+3] = i*4+3;
			nCWIndexBuffer[i*6+4] = i*4+2;
			nCWIndexBuffer[i*6+5] = i*4+1;
		}
	}
	return nCWIndexBuffer;
}
float3	GetViewDir()
{
	float3 v = GetCameraPtr()->GetTarget() - GetCameraPtr()->GetEye();
	v.normalize();
	return v;
}

CParticleEmitter::CParticleEmitter()
{
	m_nNumPart = 0;
	m_fScale = 1.0f;
	m_particles = 0;
	m_dataPtr = 0;
	Reset(true);
};
CParticleEmitter::~CParticleEmitter()
{
	if( m_particles )
	{
		delete[] m_particles;
		m_particles = NULL;
	}
}

void CParticleEmitter::Reset(BOOL bAll)
{CPUTimeNone();
	m_bIsActive = TRUE;
	m_bSquirtted = FALSE;
	m_vPos = float3( 0, 0, 0 );
	m_dwLastUpdateTime = 0;
	m_dwLastGenPartsTime = 0;
	if(bAll){
	for( int i = 0; i < m_nNumPart; i++ )
	{
		m_particles[i].dwBornTime = 0;
		m_particles[i].dwLife = 0;
		m_particles[i].fSpeed = 0;
		m_particles[i].fScale = 1.0f;
		m_particles[i].dwLastUpdateTime = 0;
	}
	}
}

float3 RandomDirection()
{
	float3 vDirection( -0.5+RandomF(0, 1), -0.5+RandomF(0, 1), -0.5+RandomF(0, 1) );
	vDirection.normalize();
	return vDirection;
}

inline float Slerp( float a, float b, float t ){ return a+t*(b-a); }

void CParticleEmitter::GenDirection( float3* axis, float fAngle, float3* dir )
{CPUTimeNone();
	float rad = fAngle*PI/180.0f;
	float fRadius = tanf( rad );
	float3 vStart = *axis;
	vStart.normalize();
	float3 vRandomDir = RandomDirection() * fRadius;
	float3 vDir = vStart+vRandomDir;
	vDir.normalize();
	*dir = vDir;
}
void CParticleEmitter::GenParticle( ParticleData* pParticle, int nTime )
{CPUTimeNone();
	pParticle->dwBornTime = nTime;
	pParticle->dwLastUpdateTime = nTime;
	pParticle->dwLife = (DWORD)(m_dataPtr->fLifeSpan*1000);
	float fSpeedVar = m_dataPtr->speed*m_dataPtr->speedvar/100.0f;
	pParticle->fSpeed = ( m_dataPtr->speed+RandomF( -fSpeedVar, fSpeedVar ) );
	// 初始uv cell
	if( m_dataPtr->nEmitterType == EmitterType_Head || m_dataPtr->nEmitterType == EmitterType_Tail )
		pParticle->nCellId = m_dataPtr->dwLifeSpanUVAnim[0];
	// 粒子的初始大小一定是1，不过在运动过程中要*scale
	pParticle->fScale = 1.0f;
	pParticle->vPos = m_vPos + m_vPlaneXAxis*(m_fScale*RandomF( -m_dataPtr->fPlaneWidth/2, m_dataPtr->fPlaneWidth/2 )) + m_vPlaneYAxis*(m_fScale*RandomF( -m_dataPtr->fPlaneHeight/2, m_dataPtr->fPlaneHeight/2 ));
	pParticle->vBornPos = pParticle->vPos;
	if( m_dataPtr->dwFlag & ParticleFlag_LocalAxis )
		GenDirection( &m_dataPtr->vPlaneNormal, m_dataPtr->fConeAngle, &pParticle->vDir );
	else
		GenDirection( &m_vPlaneNormal, m_dataPtr->fConeAngle, &pParticle->vDir );
}

void CParticleEmitter::UpdateParticle( ParticleData* pParticle, int nTime, const matrix4x4* pmatModel )
{CPUTimeT(CParticleEmitter);
	DWORD dwMiddle = pParticle->dwBornTime+(DWORD)(m_dataPtr->fDecayTime*pParticle->dwLife);
	DWORD dwEnd = pParticle->dwBornTime+pParticle->dwLife;
	int a = 0, r = 0, g = 0, b = 0;
	if( nTime < dwMiddle )
	{
		int nTimePassed = nTime-pParticle->dwBornTime;
		int nPassedFrameCount = (int)( nTimePassed*m_dataPtr->dwUVAnimFps/1000.0f );
		float t = (float)(nTime-pParticle->dwBornTime)/( dwMiddle-pParticle->dwBornTime );
		if( t > 0 )
		{
			int k = 0;
		}
		pParticle->fScale = Slerp( m_dataPtr->fParticleScaling[0], m_dataPtr->fParticleScaling[1], t );

		if( m_dataPtr->nEmitterType == EmitterType_Head  || m_dataPtr->nEmitterType == EmitterType_Tail )
		{
			int nCircleFrameCount = m_dataPtr->dwLifeSpanUVAnim[1]-m_dataPtr->dwLifeSpanUVAnim[0]+1;
			pParticle->nCellId = m_dataPtr->dwLifeSpanUVAnim[0]+nPassedFrameCount%nCircleFrameCount;
		}

		a = (int)Slerp( m_dataPtr->nAlpha[0], m_dataPtr->nAlpha[1], t );
		r = (int)Slerp( m_dataPtr->nSegmentColor[0][0], m_dataPtr->nSegmentColor[1][0], t );
		g = (int)Slerp( m_dataPtr->nSegmentColor[0][1], m_dataPtr->nSegmentColor[1][1], t );
		b = (int)Slerp( m_dataPtr->nSegmentColor[0][2], m_dataPtr->nSegmentColor[1][2], t );
	}
	else
	{
		int nTimePassed = nTime-dwMiddle;
		int nPassedFrameCount = (int)( nTimePassed*m_dataPtr->dwUVAnimFps/1000.0f );

		float t = (float)(nTime-dwMiddle)/( dwEnd-dwMiddle );
		if( t > 0 )
		{
			int k = 0;
		}
		pParticle->fScale = 
			Slerp( m_dataPtr->fParticleScaling[1], m_dataPtr->fParticleScaling[2], t );

		if( m_dataPtr->nEmitterType == EmitterType_Head  || m_dataPtr->nEmitterType == EmitterType_Tail )
		{
			int nCircleFrameCount = m_dataPtr->dwDecayUVAnim[1]-m_dataPtr->dwDecayUVAnim[0]+1;
			pParticle->nCellId = m_dataPtr->dwDecayUVAnim[0]+nPassedFrameCount%nCircleFrameCount;
		}


		a = (int)Slerp( m_dataPtr->nAlpha[1], m_dataPtr->nAlpha[2], t );
		r = (int)Slerp( m_dataPtr->nSegmentColor[1][0], m_dataPtr->nSegmentColor[2][0], t );
		g = (int)Slerp( m_dataPtr->nSegmentColor[1][1], m_dataPtr->nSegmentColor[2][1], t );
		b = (int)Slerp( m_dataPtr->nSegmentColor[1][2], m_dataPtr->nSegmentColor[2][2], t );
	}

	if( a < 0 )		a = 0;
	if( a > 255 )	a = 255;
	if( r < 0 )		r = 0;
	if( r > 255 )	r = 255;
	if( g < 0 )		g = 0;
	if( g > 255 )	g = 255;
	if( b < 0 )		b = 0;
	if( b > 255 )	b = 255;

	pParticle->dwColor = a<<24|r<<16|g<<8|b;
	float fUpdateTimePassed = nTime-pParticle->dwBornTime;
	fUpdateTimePassed /= 1000.0f;
	// 只在模型空间中变换
	if( m_dataPtr->dwFlag & ParticleFlag_LocalAxis )
	{
		pParticle->vPos = pParticle->vBornPos + (pParticle->vDir*pParticle->fSpeed*fUpdateTimePassed)/**HR3D_Config::m_fMdlSpaceScale*/;
		vec3TransformCoord( &pParticle->vPosWorld, &pParticle->vPos,	pmatModel );
	}
	else if( m_dataPtr->dwFlag & ParticleFlag_LockEmitter )
	{
		pParticle->vPos = m_vPos;
		pParticle->vPosWorld = m_vPos;
	}
	else
	{
		pParticle->vPos = pParticle->vBornPos + (pParticle->vDir*pParticle->fSpeed*fUpdateTimePassed)*m_fScale;
		pParticle->vPosWorld = pParticle->vPos;
	}
	pParticle->dwLastUpdateTime = nTime;
}

void CParticleEmitter::Update( int nFrameID, Mesh* group )
{CPUTimeT(CParticleEmitter);
	DWORD nTime = (int)(g_manager.GetAppTime() * 1000.0f);
	//float fScale = 1;
	//m_fScale = fScale;
	// 计算发射器最新位置
	const matrix4x4* pmatModel = &group->GetWorldMatrix();
	// 模型空间中
	matrix4x4 temp;
	const matrix4x4* matBone = group->m_actionDo.GetBoneMatrix(m_dataPtr->nBindBoneID);
	if(!matBone)
	{
		matBone = &temp;
		temp.identity();
	}
	vec3TransformCoord(&m_vPos, &m_dataPtr->vCenter,	matBone);
	// view的反方向
	float3 vAntiView = GetViewDir();
	vAntiView *= -1;
	//
	m_vPlaneNormal = float3( 0, 0, 1 );
	// 发射平面的法线辅助点
	float3 vNormalPoint = m_dataPtr->vCenter+m_dataPtr->vPlaneNormal;
	// 发射平面的x轴辅助点
	float3 vPlaneX = m_dataPtr->vCenter+m_dataPtr->vPlaneXAxis;
	// 发射平面的y轴辅助点
	float3 vPlaneY = m_dataPtr->vCenter+m_dataPtr->vPlaneYAxis;
	// 把发射平面法线辅助点变换到空间中
	vec3TransformCoord( &vNormalPoint, &vNormalPoint, matBone);
	// 把发射平面x轴辅助点变换到空间中
	vec3TransformCoord( &vPlaneX, &vPlaneX,	matBone);
	// 把发射平面y轴辅助点变换到空间中
	vec3TransformCoord( &vPlaneY, &vPlaneY,	matBone);
	if( ( m_dataPtr->dwFlag & ParticleFlag_LocalAxis ) )
	{
	}
	else
	{
		// 在世界空间中，还需要进行世界变换
		vec3TransformCoord( &m_vPos, &m_vPos, pmatModel );
		// 变换法线辅助点，x，y轴的辅助点
		vec3TransformCoord( &vNormalPoint, &vNormalPoint, pmatModel );
		vec3TransformCoord( &vPlaneX, &vPlaneX,	pmatModel );
		vec3TransformCoord( &vPlaneY, &vPlaneY,	pmatModel );
	}
	// 得到最终空间中的法线及x，y轴
	m_vPlaneNormal = vNormalPoint-m_vPos;
	m_vPlaneNormal.normalize();
	m_vPlaneXAxis = vPlaneX-m_vPos;
	m_vPlaneXAxis.normalize();
	m_vPlaneYAxis = vPlaneY-m_vPos;
	m_vPlaneYAxis.normalize();

	// 得到右轴和上轴，建立摄像机变换的矩阵
	float3 vf = GetViewDir();
	vec3Cross( &m_vCameraSpaceRight, &m_vPlaneNormal, &vf );
	vec3Cross( &m_vCameraSpaceUp,  &vf, &m_vCameraSpaceRight );

	vec3Normalize( &m_vCameraSpaceRight, &m_vCameraSpaceRight );
	vec3Normalize( &m_vCameraSpaceUp, &m_vCameraSpaceUp );
	if( m_dataPtr->nBindBoneID == -1 )
		return;

	BOOL bVisible = true;//pSrcMdx->IsBoneVisible( pSrcMdx->GetFrameId(), m_dataPtr->nBindBoneID );
	// 仅在bVisible时才会生成新的粒子
	if( bVisible && m_bIsActive && !m_bSquirtted && !group->IsPause())
	{
		if( m_dataPtr->dwFlag & ParticleFlag_Squirt )
		{
			m_bSquirtted = TRUE;
			// 在一瞬间创建所有的粒子
			for( int i = 0; i < m_nNumPart/*m_dataPtr->nCount*/; i++ )
			{
				// 粒子已经死亡
				if( IsParticleDead( &m_particles[i], nTime ) )
				{
					GenParticle( &m_particles[i], nTime );
				}
			}

		}
		else
		{
			static const DWORD L = 50;
			DWORD dwGenInterval = 0;
			// 期望的每秒产生粒子数
			float fPartPerSec = m_dataPtr->fTimeLength;
			// 每秒产生次数

			// 每个粒度期望产生的粒子数
			DWORD dwWillGenPartCount = 0;
			// 如果更新间隔>最小粒度,则按照更新间隔计算
			if( 1000.0f/fPartPerSec > L )
			{
				dwGenInterval = (DWORD)(1000.0f/fPartPerSec);
				dwWillGenPartCount = 1;
			}
			else
			{
				DWORD dwGenTimesPerSec = 1000.0f/L;
				dwGenInterval = L;
				dwWillGenPartCount = (DWORD)(fPartPerSec/dwGenTimesPerSec);
			}
			//if(	nTime-dwLastGenPartsTime >= (int)(1000.0f/m_dataPtr->fTimeLength) )
			if(	nTime-m_dwLastGenPartsTime >= dwGenInterval )
			{
				m_dwLastGenPartsTime = nTime;
				//int nGenCount = 0;
				for( int nGenCount = 0; nGenCount < dwWillGenPartCount; nGenCount++ )
				{
					BOOL bHasDeadPart = FALSE;
					for( int i = 0; i < m_nNumPart/*m_dataPtr->nCount*/; i++ )
					{
						// 粒子已经死亡
						if( IsParticleDead( &m_particles[i], nTime ) )
						{
							bHasDeadPart = TRUE;
							GenParticle( &m_particles[i], nTime );
							break;
						}
					}
					// 如果已经没有空闲的粒子,那么就不用继续了,
					// 等待下一个时机
					if( !bHasDeadPart )
						break;
				}
			}
		}
	}

	for( int i = 0; i < m_nNumPart/*m_dataPtr->nCount*/; i++ )
	{
		if( IsParticleDead( &m_particles[i], nTime ) )continue;
		UpdateParticle( &m_particles[i], nTime, pmatModel );
	}
}

void CParticleEmitter::MakeARenderParticle( ParticleData* pParticle, PosColorUV* vertices, float fAlpha )
{CPUTimeT(CParticleEmitter);
	float du = 1.0f/m_dataPtr->nUVRow;
	float dv = 1.0f/m_dataPtr->nUVCol;
	float3 c = pParticle->vPosWorld;
	if( m_dataPtr->nEmitterType == EmitterType_Head )
	{
		float fSize = pParticle->fScale*m_fScale;
		float3 up = m_vCameraSpaceUp*fSize;
		float3 right = m_vCameraSpaceRight*fSize;
		DWORD color = pParticle->dwColor;
		DWORD dwAlpha = (color&0xff000000)>>24;
		dwAlpha = (DWORD)(dwAlpha*fAlpha);
		if( dwAlpha > 255 )
			dwAlpha = 255;
		DWORD dwParticleColor = dwAlpha<<24|(color&0x00ffffff);
		int row = pParticle->nCellId/m_dataPtr->nUVCol;
		int col = pParticle->nCellId%m_dataPtr->nUVCol;
		vertices[0].pos = c-up-right;
		vertices[0].color = dwParticleColor;
		vertices[0].uv.x = du*col;;
		vertices[0].uv.y = dv*row+dv;

		vertices[1].pos = c+up-right;
		vertices[1].color = dwParticleColor;
		vertices[1].uv.x = du*col;
		vertices[1].uv.y = dv*row;


		vertices[2].pos = c-up+right;
		vertices[2].color = dwParticleColor;
		vertices[2].uv.x = du*col+du;
		vertices[2].uv.y = dv*row+dv;


		vertices[3].pos = c+up+right;
		vertices[3].color = dwParticleColor;
		vertices[3].uv.x = du*col+du;
		vertices[3].uv.y = dv*row;
	}
	else if( m_dataPtr->nEmitterType == EmitterType_Tail )
	{
		// view的反方向
		
		float3 vAntiView = GetViewDir();
		vAntiView *= -1;

		float3 vRight;
		vec3Cross( &vRight, &vAntiView, &pParticle->vDir );
		vec3Normalize( &vRight, &vRight );
		float3 up = pParticle->vDir*m_dataPtr->fTailLength/**pParticle->fSpeed*/*m_fScale;
		float3 right = vRight*pParticle->fScale*0.5f*m_fScale;
		DWORD color = pParticle->dwColor;

		DWORD dwAlpha = (color&0xff000000)>>24;
		dwAlpha = (DWORD)(dwAlpha*fAlpha);
		if( dwAlpha > 255 )
			dwAlpha = 255;
		DWORD dwParticleColor = dwAlpha<<24|(color&0x00ffffff);
		int row = pParticle->nCellId/m_dataPtr->nUVCol;
		int col = pParticle->nCellId%m_dataPtr->nUVCol;
		vertices[0].pos = c-up-right;
		vertices[0].color = dwParticleColor;
		vertices[0].uv.x = du*col;;
		vertices[0].uv.y = dv*row+dv;

		vertices[1].pos = c-right;
		vertices[1].color = dwParticleColor;
		vertices[1].uv.x = du*col;
		vertices[1].uv.y = dv*row;

		vertices[2].pos = c-up+right;
		vertices[2].color = dwParticleColor;
		vertices[2].uv.x = du*col+du;
		vertices[2].uv.y = dv*row+dv;

		vertices[3].pos = c+right;
		vertices[3].color = dwParticleColor;
		vertices[3].uv.x = du*col+du;
		vertices[3].uv.y = dv*row;
	}
}

void CParticleEmitter::MakeARenderParticle( ParticleData* pParticle, float3* pos, unsigned int * colors, float2* uvs, float fAlpha )
{CPUTimeT(CParticleEmitter);
	float du = 1.0f/m_dataPtr->nUVRow;
	float dv = 1.0f/m_dataPtr->nUVCol;

	float3 c = pParticle->vPosWorld;
	if( m_dataPtr->nEmitterType == EmitterType_Head )
	{
		float fSize = pParticle->fScale*m_fScale;
		float3 up = m_vCameraSpaceUp*fSize;
		float3 right = m_vCameraSpaceRight*fSize;
		DWORD color = pParticle->dwColor;

		DWORD dwAlpha = (color&0xff000000)>>24;
		dwAlpha = (DWORD)(dwAlpha*fAlpha);
		if( dwAlpha > 255 )
			dwAlpha = 255;
		//		DWORD dwParticleColor = dwAlpha<<24|(0xffffffff&0x00ffffff);
		DWORD dwParticleColor = dwAlpha<<24|(color&0x00ffffff);

		int row = pParticle->nCellId/m_dataPtr->nUVCol;
		int col = pParticle->nCellId%m_dataPtr->nUVCol;

		pos[0] = c-up-right;
		colors[0] = dwParticleColor;
		uvs[0].x = du*col;;
		uvs[0].y = dv*row+dv;

		pos[1] = c+up-right;
		colors[1] = dwParticleColor;
		uvs[1].x = du*col;
		uvs[1].y = dv*row;


		pos[2] = c-up+right;
		colors[2] = dwParticleColor;
		uvs[2].x = du*col+du;
		uvs[2].y = dv*row+dv;


		pos[3] = c+up+right;
		colors[3] = dwParticleColor;
		uvs[3].x = du*col+du;
		uvs[3].y = dv*row;
	}
	else if( m_dataPtr->nEmitterType == EmitterType_Tail )
	{
		// view的反方向
		
		float3 vAntiView = GetViewDir();
		vAntiView *= -1;

		float3 vRight;
		vec3Cross( &vRight, &vAntiView, &pParticle->vDir );
		vec3Normalize( &vRight, &vRight );
		float3 up = pParticle->vDir*m_dataPtr->fTailLength/**pParticle->fSpeed*/*m_fScale;
		float3 right = vRight*pParticle->fScale*0.5f*m_fScale;
		DWORD color = pParticle->dwColor;

		DWORD dwAlpha = (color&0xff000000)>>24;
		dwAlpha = (DWORD)(dwAlpha*fAlpha);
		if( dwAlpha > 255 )
			dwAlpha = 255;
		DWORD dwParticleColor = dwAlpha<<24|(color&0x00ffffff);


		int row = pParticle->nCellId/m_dataPtr->nUVCol;
		int col = pParticle->nCellId%m_dataPtr->nUVCol;

		pos[0] = c-up-right;
		colors[0] = dwParticleColor;
		uvs[0].x = du*col;;
		uvs[0].y = dv*row+dv;

		pos[1] = c-right;
		colors[1] = dwParticleColor;
		uvs[1].x = du*col;
		uvs[1].y = dv*row;


		pos[2] = c-up+right;
		colors[2] = dwParticleColor;
		uvs[2].x = du*col+du;
		uvs[2].y = dv*row+dv;


		pos[3] = c+right;
		colors[3] = dwParticleColor;
		uvs[3].x = du*col+du;
		uvs[3].y = dv*row;
	}
}

#define	MAX_PARTICLE_COUNT		512

void CParticleEmitter::Init(ParticleEmitterData2* d, MeshData* gd)
{CPUTimeNone();
	this->m_dataPtr	=	d;
	if(m_dataPtr->szTexture[0] && !m_dataPtr->texturePtr)
	{
		TexturePtr ret;
		if(gd->LoadTexture(ret, m_dataPtr->szTexture))
		{
			Texture* old = m_dataPtr->texturePtr;
			m_dataPtr->texturePtr	=	ret.GetPtr();
			safe_addref(m_dataPtr->texturePtr);
			safe_release(old);
		}
		else
			m_dataPtr->szTexture[0] = 0;
	}
	m_nNumPart = (int)(m_dataPtr->fTimeLength*m_dataPtr->fLifeSpan);
	if(m_nNumPart > MAX_PARTICLE_COUNT)
		m_nNumPart = MAX_PARTICLE_COUNT;
	if(m_particles)
		delete[] m_particles;
	m_particles = 0;
	if(m_nNumPart > 0)
	{
		m_particles = new ParticleData[m_nNumPart];
		Reset(true);
	}
}
void CParticleEmitter::Render( float fAlpha )
{CPUTimeT(CParticleEmitter);
	DWORD nTime = (int)(g_manager.GetAppTime() * 1000.0f);
	float du = 1.0f/m_dataPtr->nUVRow;
	float dv = 1.0f/m_dataPtr->nUVCol;
	int const MAX_Particle = MAX_PARTICLE_COUNT*4;
	static PosColorUV	vertices[MAX_Particle];
	static AutoPtr<DeviceMesh>	s_particle;
	if(s_particle.t == 0){
		memset(vertices, 0, sizeof(vertices));
		MeshPtr temp;
		g_manager.CreateMesh(temp);
		s_particle = temp;
		s_particle->Create(MAX_Particle, MAX_EMITTER_INDEX/3, Phantom::FVF_XYZ|Phantom::FVF_DIFFUSE|Phantom::FVF_TEX1);
		assert(sizeof(Phantom::PosColorUV)==s_particle->GetStrideSize());
		s_particle->WriteVertexBuffer(vertices, MAX_Particle, sizeof(Phantom::PosColorUV));
		s_particle->WriteIndexBuffer((unsigned short*)GetCWIndexBuffer(), MAX_EMITTER_INDEX, 0);
		s_particle->Commit();
	}
	int nParts = 0;
	for( int i = 0; i < m_nNumPart/*m_dataPtr->nCount*/; i++ )
	{
		if( IsParticleDead( &m_particles[i], nTime ) )
			continue;
		MakeARenderParticle( &m_particles[i], &vertices[nParts*4], fAlpha);//&vertices[nParts*4], &colors[nParts*4], &uvs[nParts*4], fAlpha );
		nParts++;
	}
	//static float3	vertices[MAX_PARTICLE_COUNT * 4];
	//static unsigned int	colors[MAX_PARTICLE_COUNT * 4];
	//static float2	uvs[MAX_PARTICLE_COUNT * 4];
	//int nParts = 0;
	//for( int i = 0; i < m_nNumPart/*m_dataPtr->nCount*/; i++ )
	//{
	//	if( IsParticleDead( &m_particles[i], nTime ) )
	//		continue;
	//	MakeARenderParticle( &m_particles[i], &vertices[nParts*4], &colors[nParts*4], &uvs[nParts*4], fAlpha );
	//	nParts++;
	//}
	if( nParts > 0 )
	{
		if(m_dataPtr->IsAlphaTest())
			g_manager->SetAlphaMode(Phantom::AlphaMode_Test);
		else
			g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		g_manager->SetBlendMode((BlendMode)m_dataPtr->srcBlend, (BlendMode)m_dataPtr->destBlend);
		//
		if(!g_manager.SetRenderMode(RenderMode_PlaneShadow))
			return;
		Program* prog	=	g_manager.GetCurrProgram();
		if(m_dataPtr->texturePtr)
			prog->SetTexture(TextureIndex_0, m_dataPtr->texturePtr);
		prog->SetWorldMatrix(&matrix4x4::s_identity());
		if(!m_dataPtr->IsAlphaTest())
			g_manager->SetZWriteEnable(false);
		g_manager->SetCullMode(CullMode_Double);
		s_particle->UpdateVertexBuffer(vertices, 4*nParts);
		s_particle->RenderIndexed(prog, 2*nParts);
		//g_manager->DrawIndexed(prog, DrawVInfos(4*nParts, vertices, colors, uvs), (unsigned short*)GetCWIndexBuffer(), 2*nParts, DrawMode_Triangles);
		if(!m_dataPtr->IsAlphaTest())
			g_manager->SetZWriteEnable(true);
	}
}

RibbonEmitterData2::RibbonEmitterData2()
{
	texturePtr	=	0;
	countPerSec	=	4;
	countLifeSec = 0;
	boneIndex	=	0;
	texture[0]=	0;
	fAlpha = 1;
	srcBlend = Phantom::BlendMode_SRCALPHA;
	destBlend=Phantom::BlendMode_INVSRCALPHA;
}

void RibbonEmitter::Init(RibbonEmitterData2* data, MeshData* gd)
{CPUTimeNone();
	m_dataPtr = data;
	if(m_dataPtr->texture[0] && !m_dataPtr->texturePtr)
	{
		TexturePtr ret;
		if(gd->LoadTexture(ret, m_dataPtr->texture))
		{
			Texture* old = m_dataPtr->texturePtr;
			m_dataPtr->texturePtr	=	ret.GetPtr();
			safe_addref(m_dataPtr->texturePtr);
			safe_release(old);
		}
		else
			m_dataPtr->texture[0] = 0;
	}
}

void RibbonEmitter::GenEdge( RibbonEdge* pEdge, matrix4x4* pParentTransform )
{CPUTimeNone();
	vec3TransformCoord(&pEdge->vPoint[0], &this->m_dataPtr->line[0], pParentTransform );
	vec3TransformCoord(&pEdge->vPoint[1], &this->m_dataPtr->line[1], pParentTransform );
}
BOOL RibbonEmitter::AddNewEdge( RibbonEdge* pEdge )
{CPUTimeNone();
	if(m_dataPtr->maxLifeSec < 4)m_dataPtr->maxLifeSec = 4;
	if( m_nEdgeCount > 0 )
	{
		if( m_nEdgeCount < m_dataPtr->maxLifeSec )
			memcpy( &m_edges[1], &m_edges[0], (m_nEdgeCount)*sizeof( RibbonEdge ) );
		else
			memcpy( &m_edges[1], &m_edges[0], (m_dataPtr->maxLifeSec-1)*sizeof( RibbonEdge ) );
	}
	m_edges[0] = *pEdge;
	m_nEdgeCount++;
	if( m_nEdgeCount > m_dataPtr->maxLifeSec )
		m_nEdgeCount = m_dataPtr->maxLifeSec;
	return TRUE;
}
void RibbonEmitter::Update( int nFrameID, Mesh* group  )
{CPUTimeT(RibbonEmitter);
	BOOL bNeedGen = FALSE;
	if( nFrameID != m_nLastFrameID )
		bNeedGen = TRUE;
	m_nLastFrameID = nFrameID;
	if( bNeedGen && !group->IsPause() )
	{
		if(m_dataPtr->boneIndex2>=0)
		{
			matrix4x4 matCombine1;
			matrix4x4 matCombine2;
			const matrix4x4* mat = 0;
			int meshid1 = group->GetDataPtr()->m_boneNameMap[m_dataPtr->boneIndex].meshid;
			if(meshid1 >= 0)
				mat = &group->m_geometryMap[meshid1].m_worldLocal;
			else
			{
				mat = group->m_actionDo.GetBoneMatrixByGlobalID(m_dataPtr->boneIndex);
				if(!mat)return;
				matrixMultiply(&matCombine1, mat, &group->GetWorldMatrix() );
				mat = &matCombine1;
			}
			const matrix4x4* mat2 = 0;
			int meshid2 = group->GetDataPtr()->m_boneNameMap[m_dataPtr->boneIndex2].meshid;
			if(meshid2 >= 0)
				mat2 = &group->m_geometryMap[meshid2].m_worldLocal;
			else
			{
				mat2 = group->m_actionDo.GetBoneMatrixByGlobalID(m_dataPtr->boneIndex2);
				if(!mat2)return;
				matrixMultiply(&matCombine2, mat2, &group->GetWorldMatrix() );
				mat2 = &matCombine2;
			}
			RibbonEdge edge;
			edge.vPoint[0] = mat->GetPosition();
			edge.vPoint[1] = mat2->GetPosition();
			AddNewEdge( &edge );
		}
		else
		{
			const matrix4x4* mat = group->m_actionDo.GetBoneMatrixByGlobalID(m_dataPtr->boneIndex);
			if(!mat)return;
			matrix4x4 matCombine;
			matrixMultiply(&matCombine, mat, &group->GetWorldMatrix() );
			RibbonEdge edge;
			GenEdge( &edge, &matCombine );
			AddNewEdge( &edge );
		}
	}
}
void RibbonEmitter::SetEnable(BOOL bEnable)
{CPUTimeNone();
	if(bEnable != m_bStart)
	{
		m_bStart = bEnable;
		if(!m_bStart)
			this->m_nEdgeCount = 0;
	}
}
void RibbonEmitter::Render( float fAlpha )
{CPUTimeT(RibbonEmitter);
	if( m_nEdgeCount < 2 )
		return;
	float k = 1.0f/(m_nEdgeCount-1);
	DWORD color = 0xffffffff;
	int const MAX_POINT = (MAX_RIBBON_EDGE+2)*4;
	static PosColorUV	vertices[MAX_POINT];
	static AutoPtr<DeviceMesh>	s_emitter;
	if(s_emitter.t == 0){
		memset(vertices, 0, sizeof(vertices));
		MeshPtr temp;
		g_manager.CreateMesh(temp);
		s_emitter = temp;
		s_emitter->Create(MAX_POINT, (MAX_POINT*6/4)/3, Phantom::FVF_XYZ|Phantom::FVF_DIFFUSE|Phantom::FVF_TEX1);
		assert(s_emitter->GetStrideSize()==sizeof(Phantom::PosColorUV));
		s_emitter->WriteVertexBuffer(vertices, MAX_POINT, sizeof(Phantom::PosColorUV));
		s_emitter->WriteIndexBuffer((unsigned short*)GetCWIndexBuffer(), (MAX_POINT*6/4), 0);
		s_emitter->Commit();
	}
	DWORD dwAlpha = (DWORD)(m_dataPtr->fAlpha * 255.0f);
	if( dwAlpha > 255 )
		dwAlpha = 255;
	DWORD dwRibbonColor = dwAlpha<<24|(color&0x00ffffff);
	for(int i = 0; i < m_nEdgeCount-1; i++ )
	{
		vertices[i*4].pos = m_edges[i].vPoint[0];
		vertices[i*4].color = dwRibbonColor;
		vertices[i*4].uv.x = k*i;
		vertices[i*4].uv.y = 1.0f;

		vertices[i*4+1].pos = m_edges[i].vPoint[1];
		vertices[i*4+1].color = dwRibbonColor;
		vertices[i*4+1].uv.x = k*i;
		vertices[i*4+1].uv.y = 0;

		vertices[i*4+2].pos = m_edges[i+1].vPoint[0];
		vertices[i*4+2].color = dwRibbonColor;
		vertices[i*4+2].uv.x = k*i+k;
		vertices[i*4+2].uv.y = 1.0f;

		vertices[i*4+3].pos = m_edges[i+1].vPoint[1];
		vertices[i*4+3].color = dwRibbonColor;
		vertices[i*4+3].uv.x = k*i+k;
		vertices[i*4+3].uv.y = 0;
	}
	//static float3	vertices[MAX_POINT];
	//static unsigned int	colors[MAX_POINT];
	//static float2	uvs[MAX_POINT];
	////static PPointColorUV v[MAX_POINT*2];
	//DWORD dwAlpha = (DWORD)(m_dataPtr->fAlpha * 255.0f);
	//if( dwAlpha > 255 )
	//	dwAlpha = 255;
	//DWORD dwRibbonColor = dwAlpha<<24|(color&0x00ffffff);
	//for(int i = 0; i < m_nEdgeCount-1; i++ )
	//{
	//	vertices[i*4] = m_edges[i].vPoint[0];
	//	colors[i*4] = dwRibbonColor;
	//	uvs[i*4].x = k*i;
	//	uvs[i*4].y = 1.0f;

	//	vertices[i*4+1] = m_edges[i].vPoint[1];
	//	colors[i*4+1] = dwRibbonColor;
	//	uvs[i*4+1].x = k*i;
	//	uvs[i*4+1].y = 0;

	//	vertices[i*4+2] = m_edges[i+1].vPoint[0];
	//	colors[i*4+2] = dwRibbonColor;
	//	uvs[i*4+2].x = k*i+k;
	//	uvs[i*4+2].y = 1.0f;

	//	vertices[i*4+3] = m_edges[i+1].vPoint[1];
	//	colors[i*4+3] = dwRibbonColor;
	//	uvs[i*4+3].x = k*i+k;
	//	uvs[i*4+3].y = 0;
	//}
	//if(m_dataPtr->texturePtr)
	//	m_dataPtr->texturePtr->SetTexture(0);
	//g_manager->SetAlphaMode(Phantom::AlphaMode_Blend, true);
	//GetRenderHelperPtr()->SetTextureStageState(0, Phantom::TSS_ALPHAOP, Phantom::TextureBlend_MODULATE);
	//GetRenderHelperPtr()->SetTextureStageState(0, Phantom::TSS_COLOROP, Phantom::TextureBlend_MODULATE);
	//GetRenderHelperPtr()->SetSrcBlend(m_dataPtr->srcBlend);
	//GetRenderHelperPtr()->SetDestBlend(m_dataPtr->destBlend);
	//GetRenderHelperPtr()->drawIndexed(Phantom::DRAW_TYPE_TRIANGLELIST, v, GetCWIndexBuffer(), (m_nEdgeCount-1)*4, (m_nEdgeCount-1)*2, sizeof(PPointColorUV), PPointColorUV::c_fvf);
	g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
	g_manager->SetBlendMode((BlendMode)m_dataPtr->srcBlend, (BlendMode)m_dataPtr->destBlend);
	if(!g_manager.SetRenderMode(RenderMode_PlaneShadow))
		return;
	Program* prog	=	g_manager.GetCurrProgram();
	if(m_dataPtr->texturePtr)
		prog->SetTexture(TextureIndex_0, m_dataPtr->texturePtr);
	prog->SetWorldMatrix(&matrix4x4::s_identity());
	g_manager->SetZWriteEnable(false);
	g_manager->SetCullMode(CullMode_Double);
	s_emitter->UpdateVertexBuffer(vertices, (m_nEdgeCount-1)*4);
	s_emitter->RenderIndexed(prog, (m_nEdgeCount-1)*2);
	//g_manager->DrawIndexed(prog, DrawVInfos((m_nEdgeCount-1)*4, vertices, colors, uvs), (unsigned short*)GetCWIndexBuffer(), (m_nEdgeCount-1)*2, DrawMode_Triangles);
	g_manager->SetZWriteEnable(true);

}
RibbonEmitterData::RibbonEmitterData(){
	bIsExport = 0;
	boneIndex2 = -1;
	texture[0] = 0;
	boneIndex = -1;
	memset(line, 0, sizeof(line));
	countPerSec = 1;
	countLifeSec = 0;
	maxLifeSec = 4;
	srcBlend = Phantom::BlendMode_SRCALPHA;
	destBlend = Phantom::BlendMode_ONE;
	fAlpha	=	1.0f;
}

//
char	MeshParts::LoadGroupData(CSafeFileHelperR& r, char bLodEnable, char bShadowEnable, int version, BOOL bUseVertexCache)
{CPUTimeT(MeshParts);
	destroy();
	//
	int head = _MakeFileHead('m','m','a','p');
	int headRead;
	r	>>	headRead;
	if(head != headRead)
	{
		return false;
	}
	m_geometryOriginal.m_groupPtr		=	this->m_geometryGroupData;
	if(!this->m_geometryOriginal.LoadGroupData( r ))
	{
		return false;
	}
	if(version < 0x00000039){
		char szModelName[128];
		if(version >= 0x00000022)
		{
			r.pStream->read(szModelName, sizeof(szModelName));
		}
		else
		{
			strcpy(szModelName, m_geometryOriginal.GetName());
		}
	}
	if(version < 0x00000020)
		m_geometryOriginal.m_nID	=	this->m_geometryGroupData->GetNewIdentity();//m_nGeometryID++;
	int geometryCount;
	r	>>	geometryCount;
	for(int igeom=0;igeom < geometryCount;igeom++)
	{
		SubMeshData* data		=	new SubMeshData();
		data->m_groupPtr			=	this->m_geometryGroupData;
		if(!data->LoadGroupData( r ))
		{
			return false;
		}
		data->m_boneLocal.boneID	=	m_geometryOriginal.m_boneLocal.boneID;
		if(version < 0x00000020)
			data->m_nID	=	this->m_geometryGroupData->GetNewIdentity();//m_nGeometryID++;
		m_modifyMap.push_back( data );
	}
	return true;
}
char		MeshParts::SaveGroupData(CSafeFileHelperW& w)
{CPUTimeT(MeshParts);
	int head = _MakeFileHead('m','m','a','p');
	w	<<	head;
	if(!this->m_geometryOriginal.SaveGroupData( w ))
	{
		return false;
	}
	w	<<	m_name;
	int geometryCount	=	(int)this->m_modifyMap.size();
	w	<<	geometryCount;
	for(int igeom=0;igeom < geometryCount;igeom++)
	{
		if(!m_modifyMap.at(igeom)->SaveGroupData( w ))
		{
			return false;
		}
	}
	return true;
}

MeshParts::MeshParts()
{
	//m_szModelName[0]	=	0;
	m_geometryGroupData	=	0;
}
SubMeshData*				MeshParts::findGeometry(const char* szName)
{CPUTimeT(MeshParts);
	if(m_geometryOriginal.m_name == szName)//.m_strName, szName) == 0)
		return &m_geometryOriginal;
	size_t count = m_modifyMap.size();
	for(size_t i=0;i<count;i++)
	{
		if(m_modifyMap.at(i)->m_name == szName)//stricmp(m_modifyMap.at(i)->m_strName, szName) == 0)
			return m_modifyMap.at(i);
	}
	return 0;
}
SubMeshData*				MeshParts::findGeometry(int geometryID)
{CPUTimeT(MeshParts);
	if(geometryID <= 0 || geometryID == m_geometryOriginal.m_nID)
		return &m_geometryOriginal;
	size_t count = m_modifyMap.size();
	for(size_t i=0;i<count;i++)
	{
		if(m_modifyMap.at(i)->m_nID	==	geometryID)
			return m_modifyMap.at(i);
	}
	return 0;
}

MeshParts::~MeshParts()
{
	destroy();
}

void	MeshParts::destroy()
{CPUTimeT(MeshParts);
	for(size_t i=0;i<m_modifyMap.size();i++)
		delete m_modifyMap.at(i);
	m_modifyMap.clear();
	m_geometryOriginal.destroy();
}

MeshData::MeshData()
{
	m_pakdir			=	0;
	m_bExistCollision	=	false;
	m_bExistHeight		=	false;
	m_isAlphaBlend		=	false;
	m_nID				=	1;
	m_bTurn180			=	false;
	m_fLodLength		=	5.0f;
	m_bCharacter		=	false;
	m_bShadowEnable		=	false;
	m_fShadowSize		=	.5f;
	m_shadowEpsilon		=	0.00001f;
	m_bLodEnable		=	false;
	m_bCanSelfShadow	=	true;
	this->m_bOptimized	=	false;
	m_fMaxVisibleLength	=	To3DValue(50.0f);
	m_nGeometryTypes	=	0xFFFFFFFF;
	m_bRenderToShadowMap	=	true;
	m_bEnableLODVisible	=	false;
	m_fBaseScale		=	1;
	m_originalMatrix.identity();
	m_bUseOriginal		=	false;
	m_fOriginalScale	=	1.0f;
	m_nDefaultActionID	=	0;
	m_bClipVisible		=	true;
	m_bExistRegion		=	false;
	m_idleHeight		=	0;
	m_bUseBoneIndex		=	0;
	//
	m_bShadowVisible	=	false;
	m_shadowOffset.setxy(0, 0);
	m_shadowTexture		=	"default_init_zip/shadow.png";
	for(int i=0;i<Phantom::ActionType_Count;i++){
		for(int j=0;j<Phantom::ActionType_Max;j++){
			m_actionTypeIDs[i][j] = 0;
		}
	}
	memset(m_actionTypeCounts, 0, sizeof(m_actionTypeCounts));
	this->m_baseMatrix.identity();
	m_meshType			=	MeshType_StaticMesh;//模型类型
	m_lineID			=	-1;//模型表格中的行ID
}

MeshData::~MeshData(void)
{
	safe_release(m_pakdir);
	m_shadowTexturePtr	=	0;
	for(int i=0;i<m_ribbons.size();i++)
	{
		safe_release(m_ribbons[i]->texturePtr);
		delete m_ribbons[i];
	}
	m_ribbons.clear();
	for(int i=0;i<m_geometrys.size();i++)
		delete m_geometrys[i];
	m_geometrys.clear();
	for(int i=0;i<m_polygons.size();i++)
		delete m_polygons[i];
	m_polygons.clear();
	for(size_t i=0;i<m_actionIO.size();i++)
		delete m_actionIO.at(i);
	m_actionIO.clear();
	m_actionMap.clear();
	this->m_boneNameMap.clear();
}
//
SubMeshPolygon*					MeshData::SearchPolygon(int id)
{CPUTimeT(MeshData);
	for(int i=0;i<m_polygons.size();i++)
		if(m_polygons[i]->m_id == id)
			return m_polygons[i];
	return 0;
}

SubMeshPolygon*					MeshData::CreatePolygon()
{CPUTimeNone();
	SubMeshPolygon* poly = new SubMeshPolygon();
	poly->m_id			=	this->GetNewIdentity();
	this->m_polygons.push_back(poly);
	return poly;
}
int								MeshData::GetActionIDByType(int actionType, int index)
{CPUTimeNone();
	if(actionType<0||actionType>=Phantom::ActionType_Count)
		return 0;
	if(index<0)index=0;
	if(m_actionTypeCounts[actionType]<=0)
		return 0;
	index %= m_actionTypeCounts[actionType];
	return this->m_actionTypeIDs[actionType][index];
}
VOID							MeshData::SetShadowVisible(BOOL b)
{CPUTimeNone();
	m_bShadowVisible = b;
	FileName	name	=	m_shadowTexture.t;
	SetShadowTexture(name);
}
VOID							MeshData::SetShadowTexture(const char* szTexture)
{CPUTimeNone();
	if(!m_bShadowVisible)return;
	if(szTexture && szTexture[0])
	{
		m_shadowTexture	=	szTexture;
		m_shadowTexture.setIsFile();
	}
	else
		m_shadowTexture	=	"default_init_zip/shadow.png";
	TexturePtr tex;
	if(this->LoadTexture(tex, m_shadowTexture.t))
		m_shadowTexturePtr	=	tex.GetPtr();
	else
		m_shadowTexturePtr	=	0;
}

BOOL							MeshData::IsLodEnableMaxVisible()	//LOD是否有效
{CPUTimeNone();
	return m_bEnableLODVisible;
}

VOID							MeshData::SetLodEnableMaxVisible(BOOL b)//设置LOD有效
{CPUTimeNone();
	this->m_bEnableLODVisible	=	b;
}

//
#define	GEOMETRY_VERSION	0x01010101

#pragma pack(push, 1)
struct GeometryID
{
	int								l,h,i;
	inline	bool	operator		==	(const GeometryID& id){return (id.h == h && id.l == l && id.i == i);}
};

#pragma pack(pop)

extern	void	computeNormals(char* pVertex, int vertexWidth/*顶点数据类型字节数*/, int vertexCount, const void* faces, int faceCount, int posOffset, int normalOffset);	//构造模型法线
char	MeshData::LoadGroupData(CSafeFileHelperR& r, ImportResult* result)
{CPUTimeT(MeshData);
	int head	=	_MakeFileHead('g','e','o','m');
	//
	int version = 0;
	r	>>	version;
	skip_r sr;
	if(version >= 0x00000043)
		sr.begin(r.pStream);
	if(version >= 0x00000038)
	{
		BOOL bTurn = false;
		r >> bTurn;
		m_bTurn180 = bTurn;
	}
	if(version >= 0x00000030 && version<0x00000039)
	{
		VersionWriter reads;
		reads.ReadAll(r.pStream);
	}
	if(version >= 0x00000032 && version<0x00000039)
	{
		MeshShaderType					shaderActive;
		BOOL							shaderRender[MeshShaderType_Count];//默认渲染列表
		Name							shaderName[MeshShaderType_Count];//默认渲染列表
		r >> shaderActive;
		r.pStream->read(shaderRender, sizeof(shaderRender));
		r.pStream->read(shaderName, sizeof(shaderName));
	}
	if(version >= 0x00000033&&version<0x00000039)
	{
		unsigned char auth[2048];
		int authLen = 0;
		BOOL bAuth = false;
		r >> bAuth >> authLen;
		r.pStream->read(auth, authLen);
		//if(!bAuth)
		//	return false;
	}
	r >> m_nID;
	if(version >= 0x00000041){
		r >> m_bOptimized;
	}
	if(version >= 0x00000042)
		r >> m_fBaseScale;
	if(version >= 0x00000040){
		//
		int polygons = 0;
		r >> polygons;
		m_polygons.resize(polygons);
		for(int i=0;i<polygons;i++)
		{
			m_polygons[i] = new SubMeshPolygon();
			m_polygons[i]->LoadData(r);
		}
	}
	//
	if(version<0x00000039){
		GeometryID temp;
		r	>> temp;
	}
	int geometryCount;
	r	>>	geometryCount	>>	m_region;
	if(version<0x00000039){
		int		alphaMode;
		int		nGeometryID;
		char	bIsSceneObject;
		r	>>	bIsSceneObject >> alphaMode >> nGeometryID;
	}
	r	>>	m_bLodEnable >> m_bShadowEnable;
	if(version >= 0x00000031)
	{
		r >> m_bShadowVisible >> m_shadowOffset >> m_shadowTexture >> m_fShadowSize;//默认default_init_zip\\shadow.dds
		this->SetShadowVisible(m_bShadowVisible);
	}
	if(version >= 0x00000025)
		r >> m_bClipVisible;
	r >> m_fLodLength;
	if(m_fLodLength < 0.01f)m_fLodLength = 0.01f;
	if(version >= 0x00000005)
		r >> m_bCanSelfShadow;
	if(version >= 0x00000008)
		r >> m_shadowEpsilon;
	if(version >= 0x00000009)
		r >> m_nGeometryTypes;
	if(version >= 0x00000010)
		r >> m_bRenderToShadowMap;
	r >> m_fMaxVisibleLength;
	if(version >= 0x00000012)
		r >> m_bEnableLODVisible;
	if(version >= 0x00000013&&version<0x00000039)
	{
		int nShaderMode;
		r >> nShaderMode;
	}
	if(version > 0x00000022)
		r >> m_nDefaultActionID;
	if(version >= 0x00000015)
		r >> m_fOriginalScale >> this->m_bUseOriginal;
	//matrixScaling(&m_originalMatrix, m_fOriginalScale, m_fOriginalScale, m_fOriginalScale);
	std::string str;
	m_aaboxV.setup(m_region.aabox._min, m_region.aabox._max);
	float3 vM = (m_region.aabox._max + m_region.aabox._min)*0.5f;
	float fS = (m_region.aabox._max - vM).R();
	m_region.sphere.setup(vM, fS );
	if(version <= 0x00000001)
	{
		r	>>	str;
	}
	if(version<0x00000039){
	int numBind;
	r	>>	numBind;
	}
	int	headRead;
	r	>>	headRead;
	if(head != headRead)
	{
		if(result)
			*result	=	ImportResult_HeadFailure;
		return false;
	}
	//
	if(version<0x00000039){
		PFileSeek temp;
		r.pStream->read(&temp, sizeof(PFileSeek));
	}
	int	actionCount;
	int	boneCount;
	r	>>	actionCount	>>	boneCount;
	if(version<0x00000039){
		for(int i=0;i<geometryCount;i++)
		{
			int head2 = _MakeFileHead('m','m','a','p');
			int headRead2;
			r	>>	headRead2;
			if(head2 != headRead2)
			{
				return false;
			}
			SubMeshData* data		=	new SubMeshData();
			data->m_groupPtr		=	this;
			if(!data->LoadGroupData( r ))
			{
				delete data;
				return false;
			}
			m_geometrys.push_back(data);
			if(version < 0x00000039){
				char szModelName[128];
				if(version >= 0x00000022)
				{
					r.pStream->read(szModelName, sizeof(szModelName));
				}
				else
				{
					strcpy(szModelName, data->GetName());
				}
			}
			if(version < 0x00000020)
				data->m_nID	=	this->GetNewIdentity();//m_nGeometryID++;
			int geometryCount2;
			r	>>	geometryCount2;
			for(int igeom=0;igeom < geometryCount2;igeom++)
			{
				SubMeshData* data		=	new SubMeshData();
				data->m_groupPtr		=	this;
				if(!data->LoadGroupData( r ))
					return false;
				data->m_boneLocal.boneID	=	data->m_boneLocal.boneID;
				if(version < 0x00000020)
					data->m_nID		=	GetNewIdentity();//m_nGeometryID++;
				m_geometrys.push_back( data );
			}
		}
	}
	else{
		for(int i=0;i<geometryCount;i++)
		{
			SubMeshData* data		=	new SubMeshData();
			data->m_groupPtr		=	this;
			if(!data->LoadGroupData( r ))
				return false;
			data->m_boneLocal.boneID	=	data->m_boneLocal.boneID;
			m_geometrys.push_back( data );
		}
	}
	m_idleHeight = m_region.aabox._max.z;
	for(int i=0;i<actionCount;i++)
	{
		MeshActionData* action = new MeshActionData();
		if(!action->LoadData(r))
			return false;
		action->initBoneName(this);
		m_actionIO.push_back( action );
	}
	if(boneCount)
	{
		std::string str;
		m_boneNameMap.resize(boneCount);
		for(int i=0;i<boneCount;i++)
		{
			r >> str;
			strcpy(m_boneNameMap.at(i).szName, str.c_str());
			m_boneNameMap[i].meshid = -1;
			if(version >= 0x00000037)
				r >> m_boneNameMap[i].meshid;
		}
	}
	//
	if(version > 0x00000033)
	{
		int emitter = 0;
		r >> emitter;
		for(int i=0;i<emitter;i++)
		{
			RibbonEmitterData2& d = *(new RibbonEmitterData2());
			RibbonEmitterData l;
			if(version >= 0x00000036)
			{
				int len = 0;
				r >> len;
				r.pStream->read(&l, len);
			}
			else
			{
				RibbonEmitterDataOld lo;
				r >> lo;
				memcpy(&l, &lo, sizeof(RibbonEmitterDataOld));
			}
			memcpy(&d, &l, sizeof(RibbonEmitterData));
			m_ribbons.push_back(&d);
		}
	}
	if(version > 0x00000034)
	{
		int particle = 0;
		r >> particle;
		for(int i=0;i<particle;i++)
		{
			ParticleEmitterData2& d = *(new ParticleEmitterData2());
			ParticleEmitterData l;
			r >> l;
			memcpy(&d, &l, sizeof(ParticleEmitterData));
			m_particles.push_back(&d);
		}
	}
	if(version>=0x00000045){
		r >> m_meshType;//模型类型
		r >> m_lineID;//模型表格中的行ID
	}
	if(version>=0x00000046)
		r >> m_idleHeight;
	if(version>=0x00000047)
		r >> m_bUseBoneIndex;
	else
		m_bUseBoneIndex = true;
	int numPoint = 0;
	if(version>=0x00000050){
		r >> numPoint;
		if(numPoint>0){
			m_points.resize(numPoint);
			for(int i=0;i<numPoint;i++){
				r >> m_points[i];
			}
		}
	}
	sr.end();

	this->RebuildActions(false);
	//
	m_bCharacter	=	(m_actionMap.size() > 0);
	if(result)
		*result	=	ImportResult_OK;
	this->RebuildBaseMatrix();
	for(int i=0;i<this->m_geometrys.size();i++){
		if(this->m_geometrys[i]->m_bIsCollision)
			m_bExistCollision = true;
		if(this->m_geometrys[i]->m_bIsHeight)
			m_bExistHeight = true;
	}
	m_bExistRegion = false;
	for(int i=0;i<m_actionMap.size();i++){
		if(m_actionMap[i]->m_bRegionOK)
			m_bExistRegion = true;
		//if(m_actionMap[i]->m_bRegionOK&&m_actionMap[i]->m_actionType==Phantom::ActionType_Idle){
		//	m_idleHeight = m_actionMap[i]->m_region.aabox._max.z;
		//}
	}
	if(m_idleHeight<0.1f)m_idleHeight=0.1f;
	return true;
}
VOID	MeshData::RebuildBaseMatrix()
{CPUTimeNone();
	matrixScaling(&m_baseMatrix, m_fBaseScale, m_fBaseScale, m_fBaseScale);
	if(m_bTurn180)
	{
		matrix4x4 temp;
		matrixRotationZ(&temp, PI);
		matrixMultiply(&m_baseMatrix, &m_baseMatrix, &temp);
	}
}
char	MeshData::SaveGroupData(CSafeFileHelperW& w, ImportResult* result)
{CPUTimeT(MeshData);
	int head	=	_MakeFileHead('g','e','o','m');
	//
	int version	=	0x00000050;
	w	<<	version;
	skip_w sw(w.pStream);//开始数据保护
	w	<<	m_bTurn180;
	w	<<	m_nID;
	w	<<	m_bOptimized;
	w	<<	m_fBaseScale;
	//
	int polygons = m_polygons.size();
	w << polygons;
	for(int i=0;i<polygons;i++)
		m_polygons[i]->SaveData(w);
	//
	int geometryCount = m_geometrys.size();
	w	<<	geometryCount	<<	m_region;
	w	<< m_bLodEnable << m_bShadowEnable;
	//
	w << m_bShadowVisible << m_shadowOffset << m_shadowTexture << m_fShadowSize;//默认default_init_zip\\shadow.dds
	//
	w	<< m_bClipVisible;
	w	<< m_fLodLength;
	w	<< m_bCanSelfShadow;
	w	<< m_shadowEpsilon;
	w	<< m_nGeometryTypes;
	w	<< m_bRenderToShadowMap;
	w	<< m_fMaxVisibleLength;
	w	<< m_bEnableLODVisible;
	w	<< m_nDefaultActionID;
	w	<< this->m_fOriginalScale << this->m_bUseOriginal;
	w	<<	head;
	int	actionCount				=	(int)m_actionIO.size();
	int	boneCount				=	(int)m_boneNameMap.size();
	w	<<	actionCount	<<	boneCount;
	for(int i=0;i<m_geometrys.size();i++)
	{
		if(!m_geometrys[i]->SaveGroupData( w ))
			return false;
	}
	for(int i=0;i<actionCount;i++)
	{
		if(!m_actionIO.at(i)->SaveData(w))
			return false;
	}
	if(boneCount)
	{
		for(int i=0;i<boneCount;i++)
		{
			Phantom::strlwr_t(m_boneNameMap.at(i).szName);
			w << m_boneNameMap.at(i).szName;
			w << m_boneNameMap.at(i).meshid;
		}
	}
	int emitter = (int)m_ribbons.size();
	w << emitter;
	for(int i=0;i<m_ribbons.size();i++){
		RibbonEmitterData& d = *m_ribbons[i];
		int len = sizeof(RibbonEmitterData);
		w << len;
		w.pStream->write(&d, sizeof(RibbonEmitterData));
	}
	int particle = (int)m_particles.size();
	w << particle;
	for(int i=0;i<m_particles.size();i++){
		Phantom::ParticleEmitterData& d = *this->m_particles[i];
		w << d;
	}
	w << m_meshType;//模型类型
	w << m_lineID;//模型表格中的行ID
	w << m_idleHeight;
	w << m_bUseBoneIndex;
	int numPoint = m_points.size();
	w << numPoint;
	for(int i=0;i<m_points.size();i++){
		w << m_points[i];
	}
	sw.end();//技术数据保护
	return true;
}

PakFileData*	LoadTexturePak(const char* szTextureFile, const char* szTexturePath)
{
	if(!szTextureFile||!szTextureFile[0])
		return 0;
	LFileName ret;
	if(fexist(szTextureFile, &ret))
		return LoadFileData(szTextureFile, false);
	FileName fileName, origDir;
	FileName path, dir, file, ext;
	file	=	szTextureFile;
	file.setIsFile();
	file.splitpath(&origDir, &fileName, &ext);
	//
	file		=	fileName.t;
	file		+=	".png";
	dir			=	origDir;
	dir			+=	file.t;
	if(fexist(dir.t, &ret))
		return LoadFileData(dir.t, false);
	//
	file		=	fileName.t;
	file		+=	".jpg";
	dir			=	origDir;
	dir			+=	file.t;
	if(fexist(dir.t, &ret))
		return LoadFileData(dir.t, false);
	//
	path = szTexturePath;
	path.setIsFile();
	path.splitpath(&origDir, 0, 0);
	//尝试本地目录
	file		=	fileName.t;
	file		+=	ext.t;
	dir			=	origDir;
	dir			+=	file.t;
	if(fexist(dir.t, &ret))
		return LoadFileData(dir.t, false);
	file		=	fileName.t;
	file		+=	".png";
	dir			=	origDir;
	dir			+=	file.t;
	if(fexist(dir.t, &ret))
		return LoadFileData(dir.t, false);
	//
	file		=	fileName.t;
	file		+=	".jpg";
	dir			=	origDir;
	dir			+=	file.t;
	if(fexist(dir.t, &ret))
		return LoadFileData(dir.t, false);
	return 0;
}
#ifdef WIN32_OS
BOOL	MeshData::LoadMixFile(int actionID, const char* szMixFile)
{
	for(size_t i=0;i<m_actionIO.size();i++)
	{
		if(m_actionIO[i]->m_actionID == actionID)
		{
			m_actionIO[i]->LoadMixFile(this, szMixFile);
			this->RebuildActions(true);
			return true;
		}
	}
	return false;
}
#endif
BOOL	MeshData::LoadTexture(TexturePtr& r, char* szTextureFile)
{
	TexturePtr ret;
	FileName fileName, szTexturePath;
	m_filename.splitpath(&szTexturePath, &fileName, 0);
	if(m_pakdir)
		return g_manager.LoadTexture(r, szTextureFile, true, 2, true, m_pakdir);
	return Phantom::LoadTexture(r, szTextureFile, szTexturePath, true, 0);
}
extern	BOOL	IsExistFile(CDynamicArray<PakFileData*>& files, const char* szFile);
extern	BOOL	GetAuthCode(GenuineGUID* r);
char				MeshData::ExportPak(const char* szFile)
{
	if(this->m_pakdir)return false;
	//GenuineGUID temp;
	//if(!GetAuthCode(&temp))return false;
	//int passlen = sizeof(Phantom::GenuineGUID);
	//unsigned char* pass = (unsigned char*)&temp;

	CDynamicArray<PakFileData*> files;
	PakFileData* data = LoadFileData(m_filename.t, true);
	if(!data)
		return false;
	files.push_back(data);
	//
	FileName fileName, szTexturePath;
	m_filename.splitpath(&szTexturePath, &fileName, 0);
	//
	if(!IsExistFile(files, m_shadowTexture.t)){
		data = LoadTexturePak(m_shadowTexture.t, szTexturePath);
		if(data)
		{
			data->bIsImage = true;
			files.push_back(data);
		}
	}
	for(int i=0;i<m_ribbons.size();i++){
		if(!IsExistFile(files, m_ribbons[i]->texture)){
			data = LoadTexturePak(m_ribbons[i]->texture, szTexturePath);
			if(data){
				data->bIsImage = true;
				files.push_back(data);
			}
		}
	}
	for(int i=0;i<m_particles.size();i++){
		if(!IsExistFile(files, m_particles[i]->szTexture)){
			data = LoadTexturePak(m_particles[i]->szTexture, szTexturePath);
			if(data)
			{
				data->bIsImage = true;
				files.push_back(data);
			}
		}
	}
	for(int i=0;i<m_geometrys.size();i++){
		for(int j=0;j<m_geometrys[i]->m_numMaterials;j++){
			MeshMaterial& mtrl = m_geometrys[i]->m_materials[j];
			for(int t=0;t<TEXTURE_MAX_COUNT;t++){
				if(!IsExistFile(files, mtrl.texture[t].t)){
					data = LoadTexturePak(mtrl.texture[t].t, szTexturePath);
					if(data)
					{
						data->bIsImage = true;
						files.push_back(data);
					}
				}
			}
			for(int t=0;t<DIFFUSE_TEXTURE_COUNT;t++){
				if(!IsExistFile(files, mtrl.diffuseTexture[t].t)){
					data = LoadTexturePak(mtrl.diffuseTexture[t].t, szTexturePath);
					if(data)
					{
						data->bIsImage = true;
						files.push_back(data);
					}
				}
			}
		}
	}
	char ret = PakDirectory::ExportPak(szFile, files);
	for(int i=0;i<files.size();i++)
		delete files[i];
	return ret;
}

Mesh*				MeshData::CreateRuntime()
{CPUTimeT(MeshData);
	Mesh* group = new Mesh(this);
	if(!group->CreateGeometry())
	{
		delete group;
		return 0;
	}
	return group;
}

float							MeshData::GetMaxVisibleLength()	//设置最长可见距离,超过这个距离将会自动隐藏，主要用于大场景中的快速过滤渲染物体时使用
{CPUTimeNone();
	return m_fMaxVisibleLength;
}

VOID							MeshData::SetMaxVisibleLength(float f)
{CPUTimeNone();
	m_fMaxVisibleLength	=	f;
}

int								MeshData::GetBoneCount()
{CPUTimeNone();
	return (int)m_boneNameMap.size();
}

void							MeshData::RemoveAction(int actionID)
{CPUTimeNone();
	for(int i=0;i<(int)m_actionIO.size();i++)
	{
		if(m_actionIO.at(i)->m_actionID == actionID)
		{
			safe_release(m_actionIO.at(i));
			m_actionIO.eraseByIndex(i);
			this->RebuildActions(true);
			return;
		}
		else if(m_actionIO[i]->RemoveAction(actionID))
		{
			RebuildActions(true);
			return;
		}
	}
}

const char*						MeshData::GetBoneNameFromIndex(int index)
{CPUTimeNone();
	int count	=	GetBoneCount();
	if(index < 0 || index >= count)
		return "";
	return m_boneNameMap.at(index).szName;
}

struct	FileExportTemp
{
	std::string			fileName;
	std::string			srcFile;
	std::string			destFile;
};
struct	TextureFile
{
	std::string			srcName;
	std::string			fileName;
	std::string			srcFile;
	std::string			destFile;
};

char	MeshData::LoadGroupData(const char* szFile, ImportResult* result)
{CPUTimeT(MeshData);
	InitMyFile(szFile, true, false);
	this->m_filename	=	szFile;
	m_filename.setIsFile();
	LogInfo("system->mesh_load(%s)", m_filename.t);
	//
	FileName dir, fileName, szTexturePath;
	m_filename.splitpath(&szTexturePath, &fileName, 0);
	//
	m_name	=	fileName.t;
	CSafeFileHelperR r;
	if(!r.open( m_filename, true ))
	{
		LogInfo("system->error: no mesh file found.(%s)", m_filename.t);
		return false;
	}
	/*打开ｐａｋ文件测试失败*/
	char ret = 0;
	if(PakDirectory::IsPakFile(r.pStream))//如果是压包类型则读取包内容
	{
		r.close();
		safe_release(m_pakdir);
		m_pakdir = new PakDirectory();
		CSafeFileHelperR temp;
		if(!m_pakdir->OpenPak(m_filename, temp))
		{
			if(result)
				*result = Phantom::ImportResult_FailureAuth;
			LogInfo("system->error: package password error.(%s)", m_filename.t);
			return false;
		}
		if(!this->LoadGroupData(temp, result))
		{
			LogInfo("system->error: package data error.(%s)", m_filename.t);
			return false;
		}
		m_pakdir->ClosePak();
		return true;
	}
	else{
		ret = LoadGroupData( r, result );
		r.close();
	}
#ifdef DIRECT_X_9//如果没有经过优化则，直接进行优化并且保存，如果没有创建过lod并且开启lod模式，直接创建lod
		if(!m_bOptimized){
			for(int i=0;i<m_geometrys.size();i++)
				m_geometrys[i]->m_polygons->OptimizeMesh(m_geometrys[i]->m_polygons->m_bLodEnable);
			m_bOptimized = true;
			SaveGroupData(szFile);
		}
#endif
	return ret;
}
void	MeshData::SetEmissiveAll(const color4& c)
{CPUTimeNone();
	for(int i=0;i<m_geometrys.size();i++){
		SubMeshData* mesh = m_geometrys[i];
		for(int m=0;m<mesh->m_numMaterials;m++){
			mesh->m_materials[m].emissive = c;
		}
	}
}
void	MeshData::ScaleAll(float fScale)
{CPUTimeNone();
	this->m_fBaseScale = 1.0f;
	for(int i=0;i<m_geometrys.size();i++)
		m_geometrys[i]->m_polygons->ScaleAll(fScale);
	for(int i=0;i<m_particles.size();i++){
		ParticleEmitterData2* data = m_particles[i];
		data->speed *= fScale;
		data->speedvar *= fScale;
		data->vCenter *= fScale;
		data->fPlaneWidth *= fScale;
		data->fPlaneHeight *= fScale;
		data->fParticleScaling[0] *= fScale;
		data->fParticleScaling[1] *= fScale;
		data->fParticleScaling[2] *= fScale;
	}
}
char	MeshData::SaveGroupData(const char* szFile, ImportResult* result)
{CPUTimeT(MeshData);
	if(fexist(szFile))
		removeFile(szFile);
	CSafeFileHelperW w;
	if(!w.create( szFile ))
		return false;
	if(SaveGroupData( w ))
		return true;
	return false;
}
int		MeshData::addBoneName(const char* szName)
{CPUTimeNone();
	if(!szName || !szName[0])return -1;
	for(size_t i=0;i<this->m_boneNameMap.size();i++)
	{
		if(stricmp(m_boneNameMap[i].szName, szName) == 0)
			return i;
	}
	BoneName n;
	strcpy(n.szName, szName);
	strlwr_t(n.szName);
	m_boneNameMap.push_back( n );
	n.meshid = -1;
	return (int)m_boneNameMap.size() - 1;
}

int		MeshData::findBoneIndex(const char* szName)
{CPUTimeT(MeshData);
	if(!szName || !szName[0])return -1;
	for(size_t i=0;i<this->m_boneNameMap.size();i++)
	{
		if(stricmp(m_boneNameMap[i].szName, szName) == 0)
			return (int)i;
	}
	return -1;
}

int		MeshData::findBoneIndexLike(const char* szName)
{CPUTimeT(MeshData);
	if(!szName || !szName[0])return -1;
	int n = findBoneIndex(szName);
	if(n>=0)return n;
	char buf[200];
	strcpy(buf, szName);
	strlwr_t(buf);
	for(size_t i=0;i<this->m_boneNameMap.size();i++)
	{
		if(strstr(m_boneNameMap[i].szName, szName))
			return (int)i;
	}
	return -1;
}

const char*	MeshData::GetBoneName(int id)
{CPUTimeNone();
	if(id < 0||id >= (int)m_boneNameMap.size())
		return "";
	return m_boneNameMap.at(id).szName;
}
MeshActionRange*				MeshData::findAction(const char* szActionName)
{CPUTimeT(MeshData);
	size_t actionCount = this->m_actionMap.size();
	for(size_t i=0;i<actionCount;i++)
	{
		if(m_actionMap[i]->m_name == szActionName)
			return m_actionMap.at(i);
	}
	return 0;
}
MeshActionRange*				MeshData::findAction(int actionID)
{CPUTimeT(MeshData);
	size_t actionCount = this->m_actionMap.size();
	for(size_t i=0;i<actionCount;i++)
	{
		if(m_actionMap[i]->m_actionID	==	actionID)
			return m_actionMap.at(i);
	}
	return 0;
}
VOID							MeshData::ResetActionByName(){CPUTimeNone();
	for(int i=0;i<this->m_actionMap.size();i++){
		MeshActionRange* range = m_actionMap[i];
		if(strstr(range->GetName(), "idle")||strstr(range->GetName(), "stand"))
			range->m_actionType = Phantom::ActionType_Idle;
		else if(strstr(range->GetName(), "walk")||strstr(range->GetName(), "move"))
		{
			range->m_actionType = Phantom::ActionType_Walk;
			if((int)range->m_fMoveOffset==0)
				range->m_fMoveOffset = this->m_region.sphere.radius*0.3f;
		}
		else if(strstr(range->GetName(), "run"))
		{
			range->m_actionType = Phantom::ActionType_Run;
			if((int)range->m_fMoveOffset==0)
				range->m_fMoveOffset = m_region.sphere.radius*0.6f;
		}
		else if(strstr(range->GetName(), "attack"))
			range->m_actionType = Phantom::ActionType_Attack;
		else if(strstr(range->GetName(), "dead")||strstr(range->GetName(), "die"))
			range->m_actionType = Phantom::ActionType_Die;
		else if(strstr(range->GetName(), "hurt")||strstr(range->GetName(), "hit"))
			range->m_actionType = Phantom::ActionType_Hit;
	}
}

VOID							Mesh::RecalcCollisionMesh(SubMesh* smesh)//如果是骨骼动画模型则可以按照这个模型为主模型来生产碰撞检测盒子关键帧
{CPUTimeT(Mesh);
	if(m_groupData->m_geometrys.size()==0)return;
	if(this->m_groupData->m_geometrys[0]->m_bUseWorld)
	{
		AABox& abox = m_groupData->m_region.aabox;
		for(int i=0;i<this->GetSubmeshCount();i++){
			SubMesh* mesh = this->GetSubmeshPtr(i);
			AABox box = mesh->m_geometryData->m_polygons->m_objectRegion.aabox;
			box._min.transform(box._min, mesh->m_worldLocal);
			box._max.transform(box._max, mesh->m_worldLocal);
			abox._min.minv(abox._min, box._min);
			abox._min.minv(abox._min, box._max);
			abox._max.maxv(abox._max, box._min);
			abox._max.maxv(abox._max, box._max);
		}
		float3 vM = (m_groupData->m_region.aabox._max + m_groupData->m_region.aabox._min)*0.5f;
		float fS = (m_groupData->m_region.aabox._max - vM).R();
		m_groupData->m_region.sphere.setup(vM, fS);
		m_groupData->m_aaboxV.setup(m_groupData->m_region.aabox._min, m_groupData->m_region.aabox._max);
		this->recalcObjectRegion();
	}
	int id = m_groupData->GetActionIDByType(Phantom::ActionType_Idle, 0);
	if(id<=0)
		m_groupData->ResetActionByName();
	if(m_groupData->m_actionMap.size()==0)
		return;
	for(int a=0;a<this->m_groupData->m_actionIO.size();a++)
	{
		MeshActionData* data = m_groupData->m_actionIO[a];
		data->m_keyBoxs.resize(data->GetKeyCount());
		this->m_actionDo.setGroup( data, 1 );
		for(int i=0;i<this->m_geometryCount;i++)
			setGeometryAction(i);
		int count = data->GetKeyCount();
		for(int k=0;k<count;k++){
			m_actionDo.SetKeyFrame(k, 0, 0);
			recalcBoneMatrix();
			AABox box;
			box._min.setxyz(100000,100000,100000);
			box._max.setxyz(-100000,-100000,-100000);
			bool bexist = false;
			for(int i=0;i<this->m_geometryCount;i++){
				if(smesh){
					if(smesh!=&m_geometryMap[i])
						continue;
				}
				bexist = true;
				AABox temp = m_geometryMap[i].RecalcIdleBox();
				box._min.minv(box._min, temp._min);
				box._max.maxv(box._max, temp._max);
			}
			if(!bexist)
				return;
			data->m_keyBoxs[k] = box;
		}
	}
}

BOOL							Mesh::RecalcIdleBox()//重新计算所有角色的盒子
{CPUTimeT(Mesh);
	int id = m_groupData->GetActionIDByType(Phantom::ActionType_Idle, 0);
	if(id<=0)
	{
		m_groupData->ResetActionByName();
		//id = m_groupData->GetActionIDByType(Phantom::ActionType_Idle, 0);
	}
	if(this->m_groupData->m_geometrys[0]->m_bUseWorld)
	{
		AABox& abox = m_groupData->m_region.aabox;
		for(int i=0;i<this->GetSubmeshCount();i++){
			SubMesh* mesh = this->GetSubmeshPtr(i);
			AABox box = mesh->m_geometryData->m_polygons->m_objectRegion.aabox;
			box._min.transform(box._min, mesh->m_worldLocal);
			box._max.transform(box._max, mesh->m_worldLocal);
			abox._min.minv(abox._min, box._min);
			abox._min.minv(abox._min, box._max);
			abox._max.maxv(abox._max, box._min);
			abox._max.maxv(abox._max, box._max);
		}
		float3 vM = (m_groupData->m_region.aabox._max + m_groupData->m_region.aabox._min)*0.5f;
		float fS = (m_groupData->m_region.aabox._max - vM).R();
		m_groupData->m_region.sphere.setup(vM, fS);
		m_groupData->m_aaboxV.setup(m_groupData->m_region.aabox._min, m_groupData->m_region.aabox._max);
		this->recalcObjectRegion();
	}
	if(m_groupData->m_actionMap.size()==0)
		return false;
	for(int a=0;a<this->m_groupData->m_actionIO.size();a++)
	{
		MeshActionData* data = m_groupData->m_actionIO[a];
		data->m_keyBoxs.resize(data->GetKeyCount());
		this->m_actionDo.setGroup( data, 1 );
		for(int i=0;i<this->m_geometryCount;i++)
			setGeometryAction(i);
		int count = data->GetKeyCount();
		for(int k=0;k<count;k++){
			m_actionDo.SetKeyFrame(k, 0, 0);
			recalcBoneMatrix();
			AABox box;
			box._min.setxyz(100000,100000,100000);
			box._max.setxyz(-100000,-100000,-100000);
			for(int i=0;i<this->m_geometryCount;i++){
				AABox temp = this->m_geometryMap[i].RecalcIdleBox();
				box._min.minv(box._min, temp._min);
				box._max.maxv(box._max, temp._max);
			}
			data->m_keyBoxs[k] = box;
		}
	}
	for(int a=0;a<this->m_groupData->m_actionMap.size();a++){
		MeshActionRange* range = m_groupData->m_actionMap[a];
		int end = range->m_endFrame;
		if(end>range->m_actionPtr->GetKeyCount())end = range->m_actionPtr->GetKeyCount();
		AABox box;
		box._min.setxyz(100000,100000,100000);
		box._max.setxyz(-100000,-100000,-100000);
		for(int k=range->m_startFrame;k<end;k++){
			AABox& temp = range->m_actionPtr->m_keyBoxs[k];
			box._min.minv(box._min, temp._min);
			box._max.maxv(box._max, temp._max);
		}
		range->m_bRegionOK = true;
		range->m_region.aabox = box;
		float3 vM = (range->m_region.aabox._max + range->m_region.aabox._min)*0.5f;
		float fS = (range->m_region.aabox._max - vM).R();
		range->m_region.sphere.setup(vM, fS);
		range->m_aaboxVertex.setup(box._min, box._max);
		if(a==0){
			this->m_groupData->m_region = range->m_region;
			m_groupData->m_aaboxV.setup(range->m_region.aabox._min, range->m_region.aabox._max);
		}
	}
	for(int i=0;i<m_groupData->m_actionMap.size();i++){
		if(m_groupData->m_actionMap[i]->m_bRegionOK&&m_groupData->m_actionMap[i]->m_actionType==Phantom::ActionType_Idle){
			m_groupData->m_idleHeight = m_groupData->m_actionMap[i]->m_region.aabox._max.z;
			break;
		}
	}
	return true;
}
BOOL							Mesh::RemoveBoneIfOne()//如果只有一个关键帧则去掉所有动作
{CPUTimeT(Mesh);
	if(this->m_groupData->m_actionMap.size()==1&&m_groupData->m_actionMap[0]->GetKeyCount()==1){
		for(int i=0;i<this->m_geometryCount;i++){
			this->m_geometryMap[i].RemoveBoneIfOne();
		}
		this->GetDataPtr()->m_fBaseScale = 1.0f;
		AABox box;
		box._min.setxyz(100000,100000,100000);
		box._max.setxyz(-100000,-100000,-100000);
		for(int i=0;i<this->m_geometryCount;i++){
			AABox& temp = this->m_geometryMap[i].m_geometryData->m_polygons->m_objectRegion.aabox;
			box._min.minv(box._min, temp._min);
			box._max.maxv(box._max, temp._max);
		}
		GetDataPtr()->m_region.aabox = box;
		GetDataPtr()->m_aaboxV.setup(GetDataPtr()->m_region.aabox._min, GetDataPtr()->m_region.aabox._max);
		float3 vM = (GetDataPtr()->m_region.aabox._max + GetDataPtr()->m_region.aabox._min)*0.5f;
		float fS = (GetDataPtr()->m_region.aabox._max - vM).R();
		GetDataPtr()->m_region.sphere.setup(vM, fS);
		for(size_t i=0;i<m_groupData->m_actionIO.size();i++)
			delete m_groupData->m_actionIO.at(i);
		m_groupData->m_actionIO.clear();
		m_groupData->m_actionMap.clear();
		return true;
	}
	return false;
}

int								MeshData::findActionID(const char* szActionName)
{CPUTimeT(MeshData);
	size_t actionCount = this->m_actionMap.size();
	for(size_t i=0;i<actionCount;i++)
	{
		if(m_actionMap[i]->m_name == szActionName)
			return m_actionMap.at(i)->m_actionID;
	}
	return -1;
}
//
//int								MeshData::GetSubmeshCount()
//{
//	return this->m_geometryCount;
//}
//
//const char*						MeshData::GetPartName(int geometryIndex)
//{
//	if(!this->m_geometryMap)return "";
//	if(geometryIndex<0 || geometryIndex>=m_geometryCount)return "";
//	return m_geometryMap[geometryIndex].m_szModelName;//.m_geometryOriginal.GetName();
//}
//
//VOID							MeshData::SetPartName(int geometryIndex, const char* szName)
//{
//	if(!this->m_geometryMap)return;
//	if(geometryIndex<0 || geometryIndex>=m_geometryCount)return;
//	strncpy(m_geometryMap[geometryIndex].m_szModelName, szName, 127);//.m_geometryOriginal.GetName();
//}
//
//int								MeshData::GetPartIndex(const char* szName)
//{
//	if(!this->m_geometryMap)return -1;
//	for(int i=0;i<m_geometryCount;i++)
//	{
//		if(m_geometryMap[i].m_name == szName)
//			return i;
//	}
//	return -1;
//}
//
void						MeshData::RemoveMesh(int index)
{CPUTimeNone();
	if(index<0 || index>=m_geometrys.size())return;
	SubMeshPolygon* poly = m_geometrys[index]->m_polygons;
	delete m_geometrys[index];
	m_geometrys.eraseByIndex(index);
	for(int i=0;i<this->m_polygons.size();i++){
		if(poly == m_polygons[i]){
			delete m_polygons[i];
			m_polygons.eraseByIndex(i);
			break;
		}
	}
}
SubMeshData*				MeshData::GetSubmeshPtr(int geometryIndex)
{CPUTimeNone();
	if(geometryIndex<0 || geometryIndex>=m_geometrys.size())return 0;
	return m_geometrys[geometryIndex];
}

//SubMeshData*				MeshData::GetPartEquipDataPtr(int equipID)//根据装备ID查找的可换装模型(int geometryIndex, int equipID)
//{
//	if(!this->m_geometryMap)return 0;
//	if(equipID < 0)
//		return 0;
//	for(int m=0;m<this->m_geometryCount;m++)
//	{
//		if(m_geometryMap[m].m_geometryOriginal.m_nID == equipID)
//			return &m_geometryMap[m].m_geometryOriginal;
//		for(size_t i=0;i<m_geometryMap[m].m_modifyMap.size();i++)
//		{
//			if(m_geometryMap[m].m_modifyMap.at(i)->m_nID == equipID)
//				return m_geometryMap[m].m_modifyMap.at(i);
//		}
//	}
//	return 0;
//}
//
//int								MeshData::GetPartEquipCount(int geometryIndex)
//{
//	if(!this->m_geometryMap)return 0;
//	if(geometryIndex<0 || geometryIndex>=m_geometryCount)return 0;
//	return (int)m_geometryMap[geometryIndex].m_modifyMap.size() + 1;
//}
//
//int								MeshData::GetPartEquipID(int geometryIndex, int index)
//{
//	if(!this->m_geometryMap)return 0;
//	if(geometryIndex<0 || geometryIndex>=m_geometryCount)return 0;
//	if(index < 0||index > (int)m_geometryMap[geometryIndex].m_modifyMap.size())return 0;
//	if(index == 0)
//		return m_geometryMap[geometryIndex].m_geometryOriginal.m_nID;
//	return m_geometryMap[geometryIndex].m_modifyMap.at(index - 1)->m_nID;
//}

int								MeshData::GetActionCount()
{CPUTimeNone();
	return m_actionMap.size();
}
int								MeshData::GetActionID(int index)
{CPUTimeNone();
	if(index<0||index>=m_actionMap.size())
		return 0;
	return m_actionMap.at(index)->m_actionID;
}

MeshActionRange*				MeshData::GetActionPtr(PtrID& id)	//返回动作指针
{CPUTimeNone();
	if(id.id == 0)
		return 0;
	if(id.index < m_actionMap.size()&&m_actionMap[id.index]->m_actionID == id.id)
		return m_actionMap[id.index];
	for(unsigned int i=0;i<m_actionMap.size();i++)
	{
		if(m_actionMap.at(i)->m_actionID == id.id)
		{
			id.index	=	i;
			id.id = m_actionMap.at(i)->m_actionID;
			return m_actionMap[id.index];
		}
	}
	id.id = 0;
	return 0;
}

MeshActionRange*			MeshData::GetActionPtr(int id)	//返回动作指针
{CPUTimeT(MeshData);
	for(size_t i=0;i<m_actionMap.size();i++)
	{
		if(m_actionMap.at(i)->m_actionID == id)
			return m_actionMap.at(i);
	}
	return 0;
}

VOID						MeshData::RebuildActions(BOOL bResetID)
{CPUTimeNone();
	this->m_actionMap.clear();
	int	actionID	=	1;
	for(int i=0;i<m_actionIO.size();i++)
	{
		if(bResetID)
			m_actionIO[i]->m_actionID	=	(actionID++);
		m_actionMap.push_back(m_actionIO[i]);
		for(int j=0;j<m_actionIO[i]->m_actions.size();j++)
		{
			if(bResetID)
				m_actionIO[i]->m_actions[j]->m_actionID	=	(actionID++);
			m_actionMap.push_back(m_actionIO[i]->m_actions[j]);
		}
	}
	memset(m_actionTypeCounts, 0, sizeof(m_actionTypeCounts));
	for(int i=0;i<m_actionMap.size();i++)
	{
		int t = m_actionMap[i]->m_actionType;
		if(t<0||t>=Phantom::ActionType_Count)
			continue;
		int id = m_actionMap[i]->GetActionID();
		if(m_actionTypeCounts[t]==0){
			assert(id>0&&id<30000);
			for(int j=0;j<Phantom::ActionType_Max;j++){
				m_actionTypeIDs[t][j] = id;
			}
			m_actionTypeCounts[t]++;
		}
		else if(m_actionTypeCounts[t]<Phantom::ActionType_Max){
			m_actionTypeIDs[t][m_actionTypeCounts[t]] = id;
			m_actionTypeCounts[t]++;
		}
	}
}

MeshActionRange*			MeshData::GetActionPtr(const char* szName)	//返回动作指针, 根据名称
{CPUTimeT(MeshData);
	for(size_t i=0;i<m_actionMap.size();i++)
	{
		if(m_actionMap.at(i)->m_name == szName)
			return m_actionMap.at(i);
	}
	return 0;
}
void		Mesh::SetPosition(const float3& _v)				//设置位置
{CPUTimeNone();
	this->m_world.SetPosition(_v);
	recalcObjectRegion();
}
void		Mesh::onChangeWorld()
{
	if(this->m_groupData->UseBaseMatrix())
		matrixMultiply(&m_world, &m_groupData->m_baseMatrix, &m_world);
	recalcObjectRegion();
}
void		Mesh::SetWorldMatrix(const matrix4x4& m, BOOL bResetSavePos)
{CPUTimeNone();
	m_world		=	m;
	onChangeWorld();
	if(bResetSavePos)
		this->m_savedMovePos = this->GetPosition();
}

void		Mesh::SetYawPitchRoll(const float3& v)
{CPUTimeNone();
	this->m_rotation	=	v;
	this->transformWorld();
}

void		Mesh::transformWorld()
{CPUTimeT(Mesh);
	static matrix4x4	matRot, matScale;
	float3 temp = m_world.GetPosition();
	Phantom::matrixRotationYawPitchRoll(&matRot, this->m_rotation.x, this->m_rotation.y, 0);
	Phantom::matrixRotationZ(&matScale, this->m_rotation.z);
	matrixMultiply(&matRot, &matRot, &matScale);
	if(m_groupData->m_bUseOriginal)
		Phantom::matrixScaling(&matScale, m_scale.x * this->m_groupData->m_fOriginalScale, m_scale.y * this->m_groupData->m_fOriginalScale, m_scale.z * this->m_groupData->m_fOriginalScale);
	else
		Phantom::matrixScaling(&matScale, m_scale.x , m_scale.y , m_scale.z );
	matrixMultiply(&m_world, &matScale, &matRot);
	m_world.SetPosition(temp);
	if(this->m_groupData->UseBaseMatrix())
		matrixMultiply(&m_world, &m_groupData->m_baseMatrix, &m_world);
	recalcObjectRegion();
}
//char		Mesh::ChangePartEquip(int geometryIndex, const char* szName)
//{
//	if(geometryIndex<0||geometryIndex>=this->m_geometryCount)
//		return false;
//	if(!this->m_groupData->m_geometryMap)return false;
//	SubMeshData* data	=	m_groupData->m_geometryMap[geometryIndex].findGeometry(szName);
//	if(!data)return false;
//	if(m_geometryMap[geometryIndex] && m_geometryMap[geometryIndex]->m_geometryData == data)
//		return true;
//	this->m_geometryMap[geometryIndex]->ResetData(data);
//	setGeometryAction(geometryIndex);
//	initRender();
//	return true;
//}
//char		Mesh::ChangePartEquip(int geometryIndex, int geometryID)
//{
//	if(geometryIndex<0||geometryIndex>=this->m_geometryCount)
//		return false;
//	if(!this->m_groupData->m_geometryMap)return false;
//	SubMeshData* data	=	m_groupData->m_geometryMap[geometryIndex].findGeometry(geometryID);
//	if(!data)return false;
//	if(m_geometryMap[geometryIndex] && m_geometryMap[geometryIndex]->m_geometryData == data)
//		return true;
//	this->m_geometryMap[geometryIndex]->ResetData(data);
//	setGeometryAction(geometryIndex);
//	initRender();
//	return true;
//}
//
//char		Mesh::ChangePartEquip(int equipID)
//{
//	if(equipID <= 0)
//		return false;
//	if(!this->m_groupData->m_geometryMap)return false;
//	SubMeshData* data	=	0;
//	int geometryIndex		=	-1;
//	for(int m=0;m<m_groupData->m_geometryCount;m++)
//	{
//		if(m_groupData->m_geometryMap[m].m_geometryOriginal.m_nID == equipID)
//		{
//			data	=	&m_groupData->m_geometryMap[m].m_geometryOriginal;
//			geometryIndex	=	m;
//			break;
//		}
//		size_t count = m_groupData->m_geometryMap[m].m_modifyMap.size();
//		for(size_t i=0;i<count;i++)
//		{
//			if(m_groupData->m_geometryMap[m].m_modifyMap.at(i)->m_nID == equipID)
//			{
//				data	=	m_groupData->m_geometryMap[m].m_modifyMap.at(i);
//				geometryIndex	=	m;
//				break;
//			}
//		}
//		if(data)
//			break;
//	}
//	if(!data)return false;
//	if(m_geometryMap[geometryIndex] && m_geometryMap[geometryIndex]->m_geometryData == data)
//		return true;
//	return ChangePartEquip(geometryIndex, equipID);
//	//SubMesh* geom		=	new SubMesh(data, this, geometryIndex);
//	//delete this->m_geometryMap[geometryIndex];
//	//m_geometryMap[geometryIndex]	=	geom;
//	//setGeometryAction(geometryIndex);
//	//initRender();
//	//geom->m_bVisible		=	data->m_bVisible;
//	//return true;
//}
//
//SubMeshData*				Mesh::GetPartEquipPtr(int geometryIndex, int index)//根据装备ID查找的可换装模型
//{
//	if(index == 0)
//		return &m_groupData->m_geometryMap[geometryIndex].m_geometryOriginal;
//	if(geometryIndex<0||geometryIndex>=this->m_geometryCount)
//		return 0;
//	if(!this->m_groupData->m_geometryMap)return 0;
//	if(index < 1 || index > (int)m_groupData->m_geometryMap[geometryIndex].m_modifyMap.size())
//		return 0;
//	return m_groupData->m_geometryMap[geometryIndex].m_modifyMap.at(index - 1);
//}
//
//int								Mesh::GetPartEquipCount(int geometryIndex)
//{
//	if(geometryIndex<0||geometryIndex>=this->m_geometryCount)
//		return 0;
//	if(!this->m_groupData->m_geometryMap)return false;
//	return (int)m_groupData->m_geometryMap[geometryIndex].m_modifyMap.size() + 1;
//}
//
//int								Mesh::GetEquipIDByName(const char* szName)	//根据名称查找装备
//{
//	for(int i=0;i<m_geometryCount;i++)
//	{
//		for(int j=0;j<m_groupData->m_geometryMap[i].m_modifyMap.size();j++)
//		{
//			if(m_groupData->m_geometryMap[i].m_modifyMap[j])
//			{
//				if(m_groupData->m_geometryMap[i].m_modifyMap[j]->m_name == szName)//stricmp(, szName) == 0)
//					return m_groupData->m_geometryMap[i].m_modifyMap[j]->m_nID;
//			}
//		}
//	}
//	return 0;
//}
//
//int								Mesh::GetPartEquipID(int geometryIndex, int skinIndex)
//{
//	if(skinIndex == 0)
//		return 0;
//	if(geometryIndex<0||geometryIndex>=this->m_geometryCount)
//		return 0;
//	if(!this->m_groupData->m_geometryMap)return false;
//	if(skinIndex < 1 || skinIndex > (int)m_groupData->m_geometryMap[geometryIndex].m_modifyMap.size())
//		return 0;
//	return m_groupData->m_geometryMap[geometryIndex].m_modifyMap.at(skinIndex - 1)->m_nID;
//}
//
//int			Mesh::GetEquipIDByType(int geometryIndex, int skinType)	//根据装备类型，查找对应的装备ID
//{
//	if(geometryIndex<0||geometryIndex>=this->m_geometryCount)
//		return -1;
//	if(!this->m_groupData->m_geometryMap)return false;
//	for(size_t i=0;i<m_groupData->m_geometryMap[geometryIndex].m_modifyMap.size();i++)
//	{
//		if(m_groupData->m_geometryMap[geometryIndex].m_modifyMap.at(i)->m_geometryType == skinType)
//			return m_groupData->m_geometryMap[geometryIndex].m_modifyMap.at(i)->m_nID;
//	}
//	return -1;
//}

void		Mesh::SetScale(const float3& v)
{CPUTimeNone();
	this->m_scale	=	v;
	this->transformWorld();
}

void		Mesh::destroy()
{CPUTimeNone();
	this->m_actionDo.setGroup(NULL, false);
	if(m_geometryMap){
		delete[] m_geometryMap;
		m_geometryMap = 0;
	}
	m_geometryMap		=	0;
}

Mesh::Mesh(MeshData* groupData)
{
	g_manager.PushMesh(this);
	m_lightPtr						=	0;
	m_bFollowCamera					=	0;
	m_bUseSystemProgram				=	true;
	m_bEnabled						=	true;
	m_ribbons						=	0;
	m_particles						=	0;
	m_nPlayType						=	0;
	m_diffuseIndex					=	0;
	m_bGeometryVisible				=	true;
	m_nState						=	0;
	m_bRRRegion						=	true;
	m_fMeshIndex					=	0;
	m_bLoopEndMode					=	false;
	m_bMoveDir	= 0;
	m_fMoveLength = 0;;//移动距离
	m_bMoveMode = 0;//移动模式，在移动模式中移动完毕所有的距离之后会自动停止
	m_bAutoMove = 1;
	m_savedMovePos.setxyz(0,0,0);
	if(groupData->m_ribbons.size() > 0)
	{
		m_ribbons = new RibbonEmitter[groupData->m_ribbons.size()];
		for(int i=0;i<groupData->m_ribbons.size();i++)
			m_ribbons[i].Init(groupData->m_ribbons[i], groupData);
	}
	if(groupData->m_particles.size() > 0)
	{
		m_particles = new CParticleEmitter[groupData->m_particles.size()];
		for(int i=0;i<groupData->m_particles.size();i++)
		{
			m_particles[i].Init(groupData->m_particles[i], groupData);
		}
	}
	m_renderR.m_groupPtr			=	this;
	m_nCurrentLodLevel				=	-1;
	m_bObjectRegion					=	true;
	m_bHasNormalMap					=	false;
	this->m_geometryMap				=	0;
	this->m_groupData				=	groupData;
	this->m_geometryCount			=	m_groupData->GetSubmeshCount();
	m_objectRegion					=	m_groupData->m_region;
	//
	m_fSpeed						=	1.0f;
	m_fSetFrameIndex				=	0;
	m_nOnceLodLevel					=	0;
	m_world.identity();
	//
	m_scale.setxyz(1,1,1);
	m_rotation.setxyz(0,0,0);
	//
	m_bVisible						=	true;
	m_bShadowVisible				=	false;
	this->m_bPlay					=	true;
	this->m_bPause					=	false;
	this->m_actionDo.m_groupPtr		=	this;
	m_callback						=	0;
	m_defaultActionID				=	groupData->GetDefaultActionID();
	m_bDisableAutoKey				=	false;
	this->m_actionDo.initializeAction( (int)groupData->m_boneNameMap.size() + 1);
	//
	//memset_s(m_boneParentID, 0, sizeof(m_boneParentID));
	//
	m_fAlphaValue					=	1;
	//m_normals.m_groupPtr			=	this;
	//m_alphas.m_groupPtr				=	this;
	//m_supports.m_groupPtr			=	this;
	//m_bindMatrix.setGrow(0);
	//m_bindRefresh.setGrow(0);
	m_drawMgrs.setGrow(0);
	m_binds.setGrow(0);
	//m_bindRefresh.setGrow(0);
}
VOID							Mesh::ClearChilds(){CPUTimeNone();
	for(int i=0;i<m_childs.size();i++){
		Mesh* old = g_manager.SearchMesh(m_childs[i]);
		if(old)
			old->m_parentID.id = 0;
	}
	m_childs.clear();
}
VOID							Mesh::SetParent(Mesh* obj)
{CPUTimeNone();
	Mesh* old = g_manager.SearchMesh(m_parentID);
	if(old)
		old->RemoveChild(this);
	if(obj)
	{
		m_parentID = obj->m_gameid;
		obj->AddChild(this);
	}
	else
		m_parentID.id = 0;
}
Mesh*							Mesh::GetParent()
{CPUTimeNone();
	return g_manager.SearchMesh(m_parentID);
}
VOID							Mesh::AddChild(Mesh* obj)
{CPUTimeNone();
	for(int i=0;i<m_childs.size();i++){
		if(m_childs[i].id==obj->m_gameid.id)
			return;
	}
	obj->SetPosition(this->m_world.GetPosition());
	m_childs.push_back(obj->m_gameid);
}
VOID							Mesh::RemoveChild(Mesh* obj)
{CPUTimeNone();
	if(!obj)return;
	for(int i=0;i<m_childs.size();i++){
		if(m_childs[i].id==obj->m_gameid.id){
			m_childs.eraseByIndex(i);
			break;
		}
	}
}
const color4&					Mesh::GetDiffuseColor()
{CPUTimeNone();
	return m_diffuse;
}
const color4&					Mesh::GetSpecularColor()
{CPUTimeNone();
	return m_diffuse;
}
const color4&					Mesh::GetEmissiveColor()
{CPUTimeNone();
	return m_emissive;
}
void							Mesh::SetAlpha(float fAlpha)
{CPUTimeNone();
	m_fAlphaValue = fAlpha;
	for(int geom=0;geom<m_geometryCount;geom++)
		m_geometryMap[geom].SetAlphaValue(fAlpha);
}
void							Mesh::SetDiffuseColor(const color4& c)
{CPUTimeNone();
	this->m_diffuse = c;
	for(int geom=0;geom<m_geometryCount;geom++)
		m_geometryMap[geom].m_diffusePtr=&m_diffuse;
}
void							Mesh::SetSpecularColor(const color4& c)
{CPUTimeNone();
}
void							Mesh::SetEmissiveColor(const color4& c)
{CPUTimeNone();
	m_emissive = c;
	for(int geom=0;geom<m_geometryCount;geom++)
		m_geometryMap[geom].m_emissivePtr = &m_emissive;
}
VOID							Mesh::ChangeDiffuse(int index)
{CPUTimeNone();
	m_diffuseIndex = index;
	if(m_diffuseIndex<0)m_diffuseIndex=0;
	if(m_diffuseIndex>DIFFUSE_TEXTURE_COUNT)
		m_diffuseIndex = DIFFUSE_TEXTURE_COUNT;
}//切换默认材质
void							Mesh::SetDestBlendMode(int c)
{CPUTimeNone();
	m_nDestBlend	=	c;
}
void							Mesh::SetSrcBlendMode(int c)
{CPUTimeNone();
	m_nSrcBlend		=	c;
}
//
//void							Mesh::SetPartAlphaMode(int geometryIndex, int alphaMode)	//如果index < 0则设置所有的模型的alpha模式
//{
//	if(!m_geometryMap)return;
//	m_nAlphaMode = alphaMode;
//	if(geometryIndex < 0)
//	{
//		for(int geom=0;geom<m_geometryCount;geom++)
//		{
//			if(AlphaMode_Blend == alphaMode)
//				m_geometryMap[geom]->m_isAlphaBlend	=	true;
//			else
//				m_geometryMap[geom]->m_isAlphaBlend	=	false;
//			//
//			for(int i=0;i<m_geometryMap[geom]->m_materialCount;i++)
//				m_geometryMap[geom]->m_materialMap[i].alphaMode	=	alphaMode;
//		}
//	}
//	else if(geometryIndex < this->m_geometryCount)
//	{
//		if(AlphaMode_Blend == alphaMode)
//			m_geometryMap[geometryIndex]->m_isAlphaBlend	=	true;
//		else
//			m_geometryMap[geometryIndex]->m_isAlphaBlend	=	false;
//		for(int i=0;i<m_geometryMap[geometryIndex]->m_materialCount;i++)
//		{
//			m_geometryMap[geometryIndex]->m_materialMap[i].alphaMode	=	alphaMode;
//		}
//	}
//}
//
//void							Mesh::SetPartAlphaValue(int geometryIndex, float fAlphaValue)	//如果index < 0则设置所有的模型的alpha值
//{
//	if(!m_geometryMap)return;
//	m_fAlphaValue = fAlphaValue;
//	if(geometryIndex < 0)
//	{
//		for(int geom=0;geom<m_geometryCount;geom++)
//			for(int i=0;i<m_geometryMap[geom]->m_materialCount;i++)
//				m_geometryMap[geom]->m_materialMap[i].m_diffuseColor.a	=	fAlphaValue;
//	}
//	else if(geometryIndex < this->m_geometryCount)
//	{
//		for(int i=0;i<m_geometryMap[geometryIndex]->m_materialCount;i++)
//			m_geometryMap[geometryIndex]->m_materialMap[i].m_diffuseColor.a	=	fAlphaValue;
//	}
//}

const char*						Mesh::GetBoneName(int boneID)
{CPUTimeNone();
	return m_groupData->GetBoneName( boneID );
}

char	Mesh::SetActionInterval(int elapseActionID, int bLoop, int* elapseBoneIndexs, int elapseBoneCount)	//动作合成
{CPUTimeNone();
	return true;
}

char				Mesh::SetCurrentAction(const char* szName, int nLoop)	//设置当前动作
{CPUTimeNone();
	return SetCurrentAction(m_groupData->findActionID(szName), nLoop);
}

void				Mesh::SetAnimationKey(int nActionID, float fFrame, BOOL bDisableAutoKey)
{CPUTimeNone();
	m_bDisableAutoKey	=	bDisableAutoKey;
	m_fSetFrameIndex	=	fFrame;
	if(m_actionDo.IsInitialized() && m_bDisableAutoKey)
	{
		m_actionDo.SetKeyFrame(fFrame, 0, 0);//ai, numBone);
		recalcBoneMatrix();
	}
}

MeshActionRange*	Mesh::GetCurrentAction()
{CPUTime(Mesh);
	return m_actionDo.GetBoneGroup();
}
VOID	Mesh::SetParticleScale(float f)
{CPUTimeNone();
	if(this->m_particles){
		for(int i=0;i<this->m_groupData->m_particles.size();i++){
			m_particles[i].m_fScale = f;
		}
	}
}
VOID	Mesh::ResetRibbonParticle()
{CPUTimeNone();
	if(this->m_ribbons){
		for(int i=0;i<this->m_groupData->m_ribbons.size();i++){
			m_ribbons[i].m_nEdgeCount = 0;
		}
	}
	if(this->m_particles){
		for(int i=0;i<this->m_groupData->m_particles.size();i++){
			m_particles[i].Reset(true);
		}
	}
}

PObjectRegion&	Mesh::GetRegion()
{CPUTimeNone();
	MeshActionRange*	r = GetCurrentAction();
	if(!r||!r->m_bRegionOK)
		return m_groupData->m_region;
	return r->m_region;
}
PBoxVertices&	Mesh::GetBoxVertices()
{CPUTime(Mesh);
	MeshActionRange*	r = GetCurrentAction();
	if(!r||!r->m_bRegionOK)
		return m_groupData->m_aaboxV;
	return r->m_aaboxVertex;
}

int		Mesh::GetCurrentActionID(){CPUTimeNone();
	if(m_actionDo.GetBoneGroup())
		return m_actionDo.GetBoneGroup()->GetActionID();
	return 0;
}
char	Mesh::SetCurrentAction(int actionID, int nLoop)
{CPUTimeT(Mesh);
	this->m_fMeshIndex = 0;
	MeshActionRange* action		=	this->m_groupData->findAction(actionID);
	if(!action && actionID <= 0 && m_groupData->m_actionMap.size() > 0)
		action	=	m_groupData->m_actionMap.at(0);
	if(!action)return false;
	//
	m_nState	=	1;
	this->m_actionDo.setGroup( action, nLoop );
	m_fSetFrameIndex	=	0;
	m_actionDo.SetKeyFrame(this->m_fSetFrameIndex, 0, 0);
	for(int i=0;i<this->m_geometryCount;i++)
	{
		setGeometryAction(i);
		this->m_geometryMap[i].m_bShowOnAction = false;
		for(int j=0;j<m_geometryMap[i].m_geometryData->m_showOnActionCount;j++){
			if(m_geometryMap[i].m_geometryData->m_showOnAction[j]==action->m_actionType){
				this->m_geometryMap[i].m_bShowOnAction = true;
			}
		}
	}
	recalcBoneMatrix();
	m_savedMovePos = this->GetPosition();
	this->recalcObjectRegion();
	m_bLoopEndMode	=	false;
	return true;
}

void	Mesh::setGeometryAction(int geometryIdx)
{CPUTimeT(Mesh);
	if(this->m_actionDo.GetBoneGroup() == 0)return;
	SubMesh* geometry			=	&m_geometryMap[geometryIdx];
	if(geometry->m_geometryData->m_boneLocal.boneID >= 0)
		geometry->m_boneMatrix.localBoneIdx	=	this->m_actionDo.findBone( geometry->m_geometryData->m_boneLocal.boneID );
	else
		geometry->m_boneMatrix.localBoneIdx	=	-1;
	geometry->m_nMtrlFrameID = 0;
	if(geometry->m_geometryData->m_polygons->m_boneCount > 0)
	{
		if(!geometry->m_boneMatrix.boneMatrix)
		{
			for(int im = 0; im < geometry->m_geometryData->m_numMaterials; im++)
			{
				MeshMaterial& material	=	geometry->m_geometryData->m_materials[im];
				memset(material.programID, 0, sizeof(material.programID));
			}
			geometry->m_boneMatrix.reset(geometry->m_geometryData->m_polygons->m_boneCount);
			for(size_t iB = 0;iB<(size_t)geometry->m_geometryData->m_polygons->m_boneCount; iB++)
			{
				geometry->m_boneMatrix.boneIdx[iB] = 0;
				geometry->m_boneMatrix.boneMatrix[iB].identity();
			}
		}
		for(size_t iB = 0;iB<(size_t)geometry->m_geometryData->m_polygons->m_boneCount; iB++)
		{
			geometry->m_boneMatrix.boneIdx[iB]	=	this->m_actionDo.findBone(geometry->m_geometryData->m_polygons->m_boneMap[iB].boneID);
			if(geometry->m_boneMatrix.boneIdx[iB]<0)
			{
				geometry->m_boneMatrix.boneIdx[iB]	=	0;
			}
		}
	}
}

void	Mesh::recalcObjectRegion()
{
	m_bRRRegion = true;
}
void	Mesh::beforeObjectRegion()
{CPUTimeT(Mesh);
	if(!m_bRRRegion)return;
	m_bRRRegion = false;
	if(m_bObjectRegion)
	{
		m_objectRegion.aabox._min.setxyz(100000000.0f, 100000000.0f, 100000000.0f);
		m_objectRegion.aabox._max.setxyz(-100000000.0f, -100000000.0f, -100000000.0f);
		for(int i=0;i<8;i++)
		{
			float3 v;
			PBoxVertices& boxv = this->GetBoxVertices();
			boxv.v[i].transform(v, m_world);
			m_objectRegion.aabox._max.maxv(m_objectRegion.aabox._max, v);
			m_objectRegion.aabox._min.minv(m_objectRegion.aabox._min, v);
		}
		//
		float3 vM = (m_objectRegion.aabox._max + m_objectRegion.aabox._min)*0.5f;
		float fS = (m_objectRegion.aabox._max - vM).R();
		m_objectRegion.sphere.setup(vM, fS );
	}
	if(m_geometryMap){
		for(int i=0;i<this->m_geometryCount;i++)
		{
			SubMesh* geom	=	&m_geometryMap[i];
			if(geom->m_geometryData->m_bUseWorld)
			{
				matrixMultiply(&geom->m_worldLocal, &geom->m_geometryData->m_world, &m_world);
				geom->recalcObjectRegion();
			}
			else if(geom->m_boneMatrix.localBoneIdx >= 0 && geom->m_bExistMeshAnim)
			{
			}
		}
	}
	for(int i=0;i<this->m_childs.size();i++){
		Mesh* mesh = g_manager.SearchMesh(m_childs[i]);
		if(mesh)
			mesh->SetPosition(this->m_world.GetPosition());
		else{
			m_childs.eraseByIndex(i);
			i--;
		}
	}
}
Mesh::~Mesh()
{
	if(m_ribbons)
		delete[] m_ribbons;
	m_ribbons = 0;
	if(m_particles)
		delete[] m_particles;
	m_particles = 0;
	destroy();
	m_groupData	=	0;
	g_manager.PopMesh(this);
	for(int i=0;i<m_binds.size();i++)
	{
		m_binds[i].mesh->m_bObjectRegion = true;
		safe_release(m_binds[i].mesh);
	}
	for(int i=0;i<m_drawMgrs.size();i++)
	{
		safe_release(m_drawMgrs[i].mgr);
	}
}
BOOL							Mesh::BindMesh(const char* szBoneName, Mesh* mesh)
{CPUTimeNone();
	if(!mesh)return false;
	if(mesh->SearchBindMesh(this))
		return false;
	if(SearchBindMesh(mesh))
		return true;
	BindBoneMesh bm;
	bm.boneIndex = -1;
	if(szBoneName&&szBoneName[0])
		bm.boneIndex = this->GetBoneIDLike(szBoneName);
	bm.mesh = mesh;
	safe_addref(mesh);
	m_binds.push_back(bm);
	mesh->m_bObjectRegion = false;
	return true;
}
BOOL							Mesh::UnbindMesh(Mesh* mesh)
{CPUTimeNone();
	for(int i=0;i<m_binds.size();i++)
	{
		if(m_binds[i].mesh==mesh)
		{
			m_binds[i].mesh->m_bObjectRegion = true;
			safe_release(m_binds[i].mesh);
			m_binds.eraseByIndex(i, 1, true);
			return true;
		}
	}
	return false;
}
BOOL							Mesh::SearchBindMesh(Mesh* mesh)
{CPUTimeNone();
	for(int i=0;i<m_binds.size();i++)
		if(m_binds[i].mesh==mesh)
			return true;
	return false;
}
char							Mesh::CreateGeometry()
{CPUTimeT(Mesh);
	destroy();
	m_geometryMap				=	new SubMesh[ m_geometryCount ];
	for(int i=0;i<m_geometryCount;i++)
	{
		m_geometryMap[i].Create(m_groupData->m_geometrys[i], this);
		m_geometryMap[i].m_bVisible		=	m_groupData->m_geometrys[i]->m_bVisible;
	}
	this->SetActionType(Phantom::ActionType_Idle, 0, 0);//SetCurrentAction(-1, true);
	for(int i=0;i<this->m_geometryCount;i++)
	{
		SubMesh* geom	=	&m_geometryMap[i];
		if(geom->m_geometryData->m_bUseWorld)
			matrixMultiply(&geom->m_worldLocal, &geom->m_geometryData->m_world, &m_world);
		else if(geom->m_boneMatrix.localBoneIdx >= 0 && geom->m_bExistMeshAnim)
		{
		}
	}
	this->transformWorld();
	return true;
}

int								Mesh::GetBoneIDLike(const char* szName)		//返回骨骼索引,根据存在的字符串
{CPUTimeNone();
	int id	=	this->m_groupData->findBoneIndexLike( szName );
	if(id < 0)return -1;
	return id;//m_groupData->getBoneIDFromIndex(id);//this->m_actionDo.findBone(id);
}

int								Mesh::GetBoneIDFromName(const char* szName)
{CPUTimeNone();
	int id	=	this->m_groupData->findBoneIndex( szName );
	if(id < 0)return -1;
	return id;
}

//int								Mesh::GetBoneParentID(int boneIndex)	//返回父物体boneID
//{
//	if(boneIndex < 0)return 0;
//	int boneID	=	m_actionDo.findBone(boneIndex);
//	if(boneID < 0)return false;
//	return m_boneParentID[boneID];
//}

int								Mesh::GetBoneCount()	//返回骨骼数量
{CPUTimeNone();
	return this->m_groupData->GetBoneCount();
}
//
const matrix4x4*				Mesh::GetBoneMatrix(int boneID)	//返回骨骼矩阵指针,默认返回模型矩阵指针
{CPUTimeT(Mesh);
	if(boneID < 0)return 0;
	int actionBoneID	=	m_actionDo.findBone(boneID);
	if(actionBoneID < 0 )return 0;
	static matrix4x4 temp;
	matrixMultiply(&temp, m_actionDo.GetBoneMatrixByGlobalID(actionBoneID), &m_world);
	return &temp;
	//if(m_bindRefresh.size() == 0)
//		m_bindRefresh.SetArrayCount(m_groupData->GetBoneCount());
	//if(!m_bindRefresh[actionBoneID])
	{
		//m_bindRefresh[actionBoneID]	=	true;
		//if(m_bindMatrix.size() == 0 && m_groupData->GetBoneCount() > 0)
			//m_bindMatrix.SetArrayCount(m_groupData->GetBoneCount());
		//matrixMultiply(&m_bindMatrix[actionBoneID], m_actionDo.GetBoneMatrix(actionBoneID), &m_world);
	}
	//return &m_bindMatrix[actionBoneID];
}

BOOL							Mesh::GetBoneMatrix(int boneID, matrix4x4* pOut)
{CPUTimeT(Mesh);
	if(!pOut)return false;
	if(boneID < 0)
	{
		*pOut = m_world;
		return false;
	}
	int actionBoneID	=	m_actionDo.findBone(boneID);
	if(actionBoneID < 0 || actionBoneID >= m_groupData->GetBoneCount())
	{
		*pOut = m_world;
		return false;
	}
	matrixMultiply(pOut, m_actionDo.GetBoneMatrixByGlobalID(actionBoneID), &m_world);
	return true;
}

int							Mesh::GetLoop()
{CPUTimeNone();
	return false;//m_actionDo.GetLoop();
}
void							Mesh::SetLoop(int bLoop)
{CPUTimeNone();
}
float							Mesh::GetActionSpeed()
{CPUTimeNone();
	return this->m_fSpeed;//m_actionDo.GetActionSpeed();
}
void							Mesh::SetActionSpeed(float fSpeed)
{CPUTimeNone();
	m_fSpeed = fSpeed;
}
void							Mesh::SetActionTime(float fTime)
{CPUTimeNone();
	if(fTime<0.1f)fTime=0.1f;
	m_fSpeed = m_actionDo.GetTimeLength()/fTime;
}
void							Mesh::Play(char bPlay)
{CPUTimeNone();
	m_bPlay = bPlay;
	m_fSetFrameIndex = 0;
	if(!this->SetCurrentAction(m_defaultActionID, true))
		m_defaultActionID = 0;
}
void		Mesh::PlayAction(int keyBegin, int keyEnd, int nLoop)
{
	if(this->m_groupData->m_actionIO.size()==0)return;
	this->SetCurrentAction(m_groupData->m_actionIO.at(0)->GetActionID(),0);
	this->m_actionDo.m_data.keyBegin = keyBegin;
	this->m_actionDo.m_data.keyEnd = keyEnd;
	this->m_actionDo.m_data.keyLoop = nLoop;
}

void						Mesh::SetRotation(float f)
{
	float3 pos = m_world.GetPosition();
	matrixRotationZ(&m_world, f);
	this->m_rotation.z = f;
	m_world.SetPosition(pos);
	if(this->m_groupData->UseBaseMatrix())
		matrixMultiply(&m_world, &m_groupData->m_baseMatrix, &m_world);
	recalcObjectRegion();
}
void						Mesh::MoveDir(const float3& dir, int action)
{
	if(this->m_actionDo.GetBoneGroup()&&m_actionDo.GetBoneGroup()->m_actionType!=action)
		this->SetActionType(action, true, 0);
	this->m_bMoveMode = true;
	this->m_bMoveDir = true;
	m_targetMovePos.x = getRotateZ(dir);
	m_savedMovePos	=	GetPosition();
	float3 dir2 = GetCameraPtr()->GetTarget() - GetCameraPtr()->GetEye();
	dir2.z = 0;
	dir2.normalize();
	dir2.RotateZ(dir2, m_targetMovePos.x);
	SetRotation(getRotateZ(dir2) + PI);
}
void						Mesh::MoveDirStop()
{
	m_bMoveDir	=	false;
	m_bMoveMode	=	false;
	m_bLoopEndMode = false;
}

void						Mesh::MoveTo(const float3& pos, int action)//设置移动目标点
{
	float3 dir = pos - this->GetPosition();
	this->m_fMoveLength = dir.length();
	dir.normalize();
	SetRotation(getRotateZ(dir) + PI);
	this->SetActionType(action, true, 0);
	this->m_bMoveMode = true;
	this->m_bMoveDir = false;
	m_targetMovePos = pos;
}

void							Mesh::FrameMove()
{CPUTimeT(Mesh);
	if(!m_bPlay || m_bPause)
	{
		if(m_bPause){
			if(this->m_particles)
			{
				for(int i=0;i<this->m_groupData->m_particles.size();i++)
				{
					if((m_groupData->m_particles[i]->dwBlendMode & 0x1000) == 0)
						m_particles[i].Update((int)m_fSetFrameIndex, this);
				}
			}
		}
		return;
	}
	if(m_bFollowCamera){
		static matrix4x4 matV;
		static BOOL bInit = false;
		if(!bInit){
			matrix4x4 matRot,matZ;
			matrixRotationX(&matRot, PI*0.5);
			matrixRotationZ(&matZ, PI);
			matrixMultiply(&matV, &matZ, &matRot);
			bInit = true;
		}
		matrix4x4 temp;
		matrixMultiply(&temp, &matV, &GetCameraPtr()->m_matViewInverse);
		this->SetWorldMatrix(temp);//GetCameraPtr()->m_matViewInverse);
	}
	float fElapsedTime	=	g_manager.GetElapsedTime();
	m_fMeshIndex += fElapsedTime * m_fSpeed;
	if(m_actionDo.IsInitialized() && !m_bDisableAutoKey)
	{
		float fSpeed = m_actionDo.GetBoneGroup()->m_fBaseSpeed * this->m_fSpeed;
		if(!m_bPause)
			m_fSetFrameIndex	+=	fElapsedTime * fSpeed;
		MeshActionRange* action	=	m_actionDo.GetBoneGroup();
		if(!action)
			return;
		float fTimeCount	=	m_actionDo.m_data.keyEnd - m_actionDo.m_data.keyBegin;//action->GetKeyCount();//m_actionDo.getBoneKey(0)->m_boneKey->getNumKey();
		if(m_fSetFrameIndex>=fTimeCount&&m_nState == 1)
		{
			if(m_bAutoMove&&action->m_actionPtr->m_moves.size()>0&&!m_bMoveDir){
				float3 speedDir = action->m_actionPtr->m_moves[action->m_actionPtr->m_moves.size()-1];// * (fSpeed/fTimeCount);
				float fLength = speedDir.R();
				this->m_fMoveLength -= fLength;
				float3 pos;
				speedDir.transformNoMove(pos, this->m_world);
				m_savedMovePos += pos;
				this->SetPosition(m_savedMovePos);
			}
			if(this->m_actionDo.GetBoneGroup()->m_bLoopMode||m_actionDo.m_data.keyLoop>0){
				if(m_actionDo.m_data.keyLoop==1&&m_bLoopEndMode){
					m_actionDo.m_data.keyLoop--;
					m_fSetFrameIndex=fTimeCount - 0.1f;
					this->m_nState = 0;
					if(m_callback)
						m_callback->ActionPlayOver(this, m_actionDo.GetBoneGroup()->m_actionType, 0);// m_actionID.id, m_nPlayType);
				}
				else{
					m_actionDo.m_data.keyLoop--;
					m_fSetFrameIndex=fmodf(m_fSetFrameIndex, fTimeCount);
					for(int i=0;i<this->m_geometryCount;i++)
						this->m_geometryMap[i].m_nMtrlFrameID = 0;
				}
			}
			else{
				m_fSetFrameIndex=fTimeCount - 0.1f;
				this->m_nState = 0;
				if(m_callback)
					m_callback->ActionPlayOver(this, m_actionDo.GetBoneGroup()->m_actionType, m_actionDo.GetBoneGroup()->m_name.t);// m_actionID.id, m_nPlayType);
			}
			//if(m_bPause)
				//return;
		}
		if(m_bAutoMove&&m_nState==1){
			if(m_bMoveDir){
				switch(action->m_actionType){
				case Phantom::ActionType_Walk:
				case Phantom::ActionType_Walk2:
				case Phantom::ActionType_Walk3:
				case Phantom::ActionType_Run:
				case Phantom::ActionType_Run2:
				case Phantom::ActionType_Run3:
					{
						float fSpeed = action->m_fMoveOffset;
						if(action->m_actionPtr->m_moves.size()>0)
							fSpeed = action->m_actionPtr->m_moves[action->m_actionPtr->m_moves.size()-1].R();
						float fMove = fSpeed * g_manager.GetElapsedTime();
						float3 dir = GetCameraPtr()->GetTarget() - GetCameraPtr()->GetEye();
						dir.z = 0;
						dir.normalize();
						dir.RotateZ(dir, m_targetMovePos.x);
						m_savedMovePos += dir*fMove;
						//m_pos.z = GetGameMgr()->m_scenePtr->GetHeight(m_pos);
						this->SetPosition(m_savedMovePos);
					}
					break;
				}
			}
			else if(action->m_actionPtr->m_moves.size()>0){
				int kb = (int)m_fSetFrameIndex+m_actionDo.m_data.keyBegin;
				if(kb>=action->m_actionPtr->m_moves.size())
					kb = action->m_actionPtr->m_moves.size()-1;
				float3& sf = action->m_actionPtr->m_moves[kb];
				float fLength = sf.R();
				if(fLength >= m_fMoveLength){
					this->SetPosition(m_targetMovePos);
					if(!m_callback||(m_callback&&!m_callback->ActionMoveOver(this)))
						this->SetActionType(Phantom::ActionType_Idle, 0, 0);
					//if(m_bPause)
						//return;
				}
				else{
					float3 pos;
					float fLerp = fmodf(m_fSetFrameIndex, 1.0f);
					if((kb+1)<action->m_actionPtr->m_moves.size()){
						pos = sf*(1-fLerp)+action->m_actionPtr->m_moves[kb+1]*fLerp;
						pos.transformNoMove(pos, this->m_world);
					}
					else{
						sf.transformNoMove(pos, this->m_world);
					}
					this->SetPosition(m_savedMovePos + pos);
				}
			}
			else if(m_bMoveMode){//如果需要计算移动的话
				switch(action->m_actionType){
				case Phantom::ActionType_Walk:
				case Phantom::ActionType_Walk2:
				case Phantom::ActionType_Walk3:
				case Phantom::ActionType_Run:
				case Phantom::ActionType_Run2:
				case Phantom::ActionType_Run3:
					{
						float fLength = action->m_fMoveOffset*m_fMeshIndex;
						if(m_bMoveMode && fLength >= m_fMoveLength){
							this->SetPosition(m_targetMovePos);
							if(!m_callback||(m_callback&&!m_callback->ActionMoveOver(this)))
								this->SetActionType(Phantom::ActionType_Idle, 0, 0);
						}
						else{
							float3 dir = m_targetMovePos - m_savedMovePos;
							dir.normalize();
							this->SetPosition(m_savedMovePos + dir*fLength);
						}
					}
					break;
				}
			}
		}
		m_actionDo.SetKeyFrame(m_fSetFrameIndex, 0, 0);
		recalcBoneMatrix();
	}
	for(int i=0;i<this->m_geometryCount;i++)
	{
		SubMesh* geom	=	&m_geometryMap[i];
		geom->m_nMtrlFrameID++;
		if(!geom->IsVisibled() && geom->m_boneMatrix.localBoneIdx<0)
			continue;
		if(geom->m_boneMatrix.localBoneIdx >= 0 && geom->m_bExistMeshAnim)
		{
			const MeshBoneKey* key	=	this->m_actionDo.getBoneKey(geom->m_boneMatrix.localBoneIdx);
			if(key)
			{
				//matrix4x4 invW;
				//matrixInverse(&invW, 0, &geom->m_geometryData->m_world);
				//matrixMultiply(&geom->m_worldLocal, &invW, &key->m_matKey);
				if(geom->m_geometryData->m_bUseWorld){
					matrixMultiply(&geom->m_worldLocal, &geom->m_geometryData->m_world, &key->m_matKey);
					matrixMultiply(&geom->m_worldLocal, &geom->m_worldLocal, &m_world);
					geom->recalcObjectRegion();
				}
				else
					matrixMultiply(&geom->m_worldLocal, &key->m_matKey, &m_world);
			}
			else
			{
				if(geom->m_geometryData->m_bUseWorld){
					matrixMultiply(&geom->m_worldLocal, &geom->m_geometryData->m_world, &m_world);
					geom->recalcObjectRegion();
				}
				else
					geom->m_worldLocal	=	m_world;
			}
		}
	}
	for(int i=0;i<m_binds.size();i++)
	{
		if(m_binds[i].boneIndex>=0){
			int meshid = this->m_groupData->m_boneNameMap[m_binds[i].boneIndex].meshid;;
			if(meshid >= 0)
			{
				m_binds[i].mesh->m_world = this->m_geometryMap[meshid].m_worldLocal;
			}
			else
				matrixMultiply(&m_binds[i].mesh->m_world, m_actionDo.GetBoneMatrixByGlobalID(m_binds[i].boneIndex), &m_world);
		}
		else
			m_binds[i].mesh->SetWorldMatrix(this->m_world);
		m_binds[i].mesh->FrameMove();
	}
	if(this->m_ribbons&&this->m_actionDo.GetBoneGroup()&&this->m_actionDo.GetBoneGroup()->m_rabbinEnable)
	{
		for(int i=0;i<this->m_groupData->m_ribbons.size();i++)
			m_ribbons[i].Update((int)(g_manager.GetAppTime()*20.0f), this);
	}
	if(this->m_particles)
	{
		for(int i=0;i<this->m_groupData->m_particles.size();i++)
		{
			if((m_groupData->m_particles[i]->dwBlendMode & 0x1000) == 0)
				m_particles[i].Update((int)m_fSetFrameIndex, this);
		}
	}
}

void						Mesh::recalcBoneMatrix()
{CPUTimeT(Mesh);
	if(m_actionDo.IsInitialized())
	{
		for(int i=0;i<this->m_geometryCount;i++)
		{
			SubMesh*		geom		=	&this->m_geometryMap[i];
			SubMeshData*	geomData	=	geom->m_geometryData;
			//
			if(geom->m_boneMatrix.boneMatrix)
			{
				for(int iB = 0;iB<geomData->m_polygons->m_boneCount; iB++)
				{
					const matrix4x4* mat	=	m_actionDo.GetBoneMatrix(geom->m_boneMatrix.boneIdx[iB]);
					if(mat)
					{
						matrixMultiply(&geom->m_boneMatrix.boneMatrix[iB], &geomData->m_polygons->m_boneMap[iB].invOriginal, mat);
					}
				}
			}
		}
	}
}

const matrix4x4*				Mesh::GetSubmeshMatrix(int geometryIndex)
{CPUTimeNone();
	if(geometryIndex < 0 || geometryIndex >= this->m_geometryCount)
		return NULL;
	SubMesh* geom	=	&m_geometryMap[geometryIndex];
	if(geom->m_boneMatrix.localBoneIdx >= 0 && geom->m_bExistMeshAnim)
	{
		const MeshBoneKey* key	=	this->m_actionDo.getBoneKey(geom->m_boneMatrix.localBoneIdx);
		if(key)
		{
			matrixMultiply(&geom->m_worldLocal, &key->m_matKey, &m_world);
			return &geom->m_worldLocal;
		}
	}
	return &m_world;
}
//
//VOID							Mesh::SetActiveShader(MeshShaderType t)
//{
//	m_activeShader = t;
//	for(int i=0;i<m_geometryCount;i++)
//	{
//		m_geometryMap[i]->m_activeShader	=	t;
//	}
//}

void						MeshPlaneShadow::drawObject(const matrix4x4* matPtr)
{CPUTimeT(MeshPlaneShadow);
	float3	vertices[4];
	unsigned int	colors[4];
	float2	uvs[4];
	float3 uvPos	=	m_ptr->GetWorldMatrix().GetPosition();
	const PObjectRegion& region	=	m_ptr->GetObjectRegion();
	float fRadius	=	region.sphere.radius * m_ptr->GetDataPtr()->GetShadowSize();
	//
	const float2& uv = m_ptr->GetDataPtr()->GetShadowUVOffset();
	float uBegin	=	uv.x;
	float vBegin	=	uv.y;
	float uEnd		=	uv.x + 1.0f;
	float vEnd		=	uv.y + 1.0f;
	float fHeight	=	1.0f;
	float fUVSize2	=	fRadius * 1.5f;
	for(int i=0;i<4;i++)
		colors[i]	=	0x8fffffff;
	vertices[0].setxyz(-fUVSize2, -fUVSize2, 0.5);
	uvs[0].setxy(uBegin, vBegin);
	//
	vertices[1].setxyz(fUVSize2, -fUVSize2, 0.5);
	uvs[1].setxy(uEnd, vBegin);
	//
	vertices[2].setxyz(-fUVSize2, fUVSize2, 0.5);
	uvs[2].setxy(uBegin, vEnd);
	//
	vertices[3].setxyz(fUVSize2, fUVSize2, 0.5);
	uvs[3].setxy(uEnd, vEnd);
	if(!g_manager.SetRenderMode(RenderMode_PlaneShadow))
		return;
	Program* prog	=	g_manager.GetCurrProgram();
	prog->SetTexture(TextureIndex_0, m_ptr->GetDataPtr()->GetShadowTexturePtr());
	prog->SetWorldMatrix(&m_ptr->GetWorldMatrix());
	g_manager->SetAlphaMode(AlphaMode_Blend);
	g_manager->SetBlendMode(BlendMode_SRCALPHA, BlendMode_INVSRCALPHA);
	g_manager->SetZWriteEnable(false);
	g_manager->SetCullMode(CullMode_Double);
	Triangle indices[2];
	indices[0].set(0, 1, 2);
	indices[1].set(1, 3, 2);
	g_manager->DrawIndexed(prog, DrawVInfos(4, vertices, colors, uvs), (unsigned short*)indices, 6, DrawMode_Triangles);
	g_manager->SetZWriteEnable(true);
}
const matrix4x4&			MeshPlaneShadow::GetWorldMatrix()
{CPUTimeT(MeshPlaneShadow);
	return m_ptr->GetWorldMatrix();
}
void						Mesh::AutoPushRender(RenderBase* obj, float fScale)
{
	if(!this->m_geometryCount)return;
	beforeObjectRegion();
	const matrix4x4* matPtr = &obj->GetWorldMatrix();
	int	nMode	=	0;
	if(m_bObjectRegion){
		PSphereData& sphere = this->GetRegion().sphere;
		if(!GetCameraPtr()->IsSphereVisible(sphere.center + matPtr->GetPosition(), sphere.radius))
			return;
	}
	//if(m_groupData->m_isAlphaBlend)
	//{
	//	g_manager.PushAlphaObject(obj, true);
	//	return;
	//}
	int nOnceLodLevel = 0;
	float3 vDist	=	GetCameraPtr()->GetEye() - matPtr->GetPosition();
	float fDistance	=	vDist.R();
	if(m_bObjectRegion)
	{
		PSphereData& sphere = this->GetRegion().sphere;
		float fLodLength	=	m_groupData->m_fLodLength * m_objectRegion.sphere.radius;
		nOnceLodLevel	=	(int)((fDistance - sphere.radius) / fLodLength);
		if(nOnceLodLevel < 0)
			nOnceLodLevel	=	0;
		if(this->m_groupData->m_bEnableLODVisible && fDistance > (sphere.radius + g_manager->GetLODMinVisible()))
		{
			if(fDistance >= (sphere.radius + m_groupData->m_fMaxVisibleLength) * g_manager->GetLODViewDelta())
			{
				return;
			}
		}
	}
	else
	{
		float fLodLength	=	m_groupData->m_fLodLength * GetRegion().sphere.radius;
		nOnceLodLevel	=	(int)(fDistance/ fLodLength);
	}
	if(nOnceLodLevel < 0)nOnceLodLevel = 0;
	else if(nOnceLodLevel >= MAX_LOD_LEVEL_COUNT)nOnceLodLevel = MAX_LOD_LEVEL_COUNT - 1;
	//
	BOOL bBlend = false;
	Program*	reqWorld		=	0;
	for(int i=0;i<this->m_geometryCount;i++)
	{
		if(m_geometryMap[i].m_geometryData->m_bIsAlphaBlend || !m_geometryMap[i].m_geometryData->m_bZWriteEnable||((int)(m_geometryMap[i].m_fAlpha*1000)<1000))
		{
			bBlend = true;
		}
		else
			this->m_geometryMap[i].drawObject(matPtr, nOnceLodLevel);
	}
	if(bBlend)
		g_manager.PushAlphaObject(obj, true);
}
void						Mesh::RenderToShadowMap(RenderBase* obj)
{CPUTimeT(Mesh);
	if(!this->m_geometryCount)return;
	beforeObjectRegion();
	const matrix4x4* matPtr = &obj->GetWorldMatrix();
	int	nMode	=	0;
	if(m_bObjectRegion){
		PSphereData& sphere = this->GetRegion().sphere;
		if(!GetCameraPtr()->IsSphereVisible(sphere.center + matPtr->GetPosition(), sphere.radius))
			return;
	}
	int nOnceLodLevel = 0;
	float3 vDist	=	GetCameraPtr()->GetEye() - matPtr->GetPosition();
	float fDistance	=	vDist.R();
	if(m_bObjectRegion)
	{
		PSphereData& sphere = this->GetRegion().sphere;
		float fLodLength	=	m_groupData->m_fLodLength * m_objectRegion.sphere.radius;
		nOnceLodLevel	=	(int)((fDistance - sphere.radius) / fLodLength);
		if(nOnceLodLevel < 0)
			nOnceLodLevel	=	0;
	}
	else
	{
		float fLodLength	=	m_groupData->m_fLodLength * GetRegion().sphere.radius;
		nOnceLodLevel	=	(int)(fDistance/ fLodLength);
	}
	if(nOnceLodLevel < 0)nOnceLodLevel = 0;
	else if(nOnceLodLevel >= MAX_LOD_LEVEL_COUNT)nOnceLodLevel = MAX_LOD_LEVEL_COUNT - 1;
	//
	BOOL bBlend = false;
	Program*	reqWorld		=	0;
	for(int i=0;i<this->m_geometryCount;i++)
	{
		if(!(m_geometryMap[i].m_geometryData->m_bIsAlphaBlend || !m_geometryMap[i].m_geometryData->m_bZWriteEnable))
			this->m_geometryMap[i].drawToShadowMap(matPtr, nOnceLodLevel);
	}
}
void						Mesh::RenderAlphaOnly(RenderBase* obj, float fScale)
{CPUTimeT(Mesh);
	if(!this->m_geometryCount)return;
	beforeObjectRegion();
	const matrix4x4* matPtr = &obj->GetWorldMatrix();
	matrix4x4 temp;
	if(this->m_groupData->m_bTurn180)
	{
		matrixRotationZ(&temp, PI);
		matrixMultiply(&temp, &temp, matPtr);
		matPtr = &temp;
	}
	int	nMode	=	0;
	float3 vDist	=	GetCameraPtr()->GetEye() - matPtr->GetPosition();
	float fDistance	=	vDist.R();
	int nOnceLodLevel;
	if(m_bObjectRegion){
		PSphereData& sphere = this->GetRegion().sphere;
		float fLodLength	=	m_groupData->m_fLodLength * m_objectRegion.sphere.radius;
		nOnceLodLevel	=	(int)((fDistance - sphere.radius) / fLodLength);
		if(nOnceLodLevel < 0)
			nOnceLodLevel	=	0;
	}
	else
	{
		float fLodLength	=	m_groupData->m_fLodLength * GetRegion().sphere.radius;
		nOnceLodLevel	=	(int)(fDistance/ fLodLength);
	}
	if(nOnceLodLevel < 0)nOnceLodLevel = 0;
	else if(nOnceLodLevel >= MAX_LOD_LEVEL_COUNT)nOnceLodLevel = MAX_LOD_LEVEL_COUNT - 1;
	//
	Program*	reqWorld		=	0;
	for(int i=0;i<this->m_geometryCount;i++)
	{
		if(m_geometryMap[i].m_geometryData->m_bIsAlphaBlend || !m_geometryMap[i].m_geometryData->m_bZWriteEnable)
			this->m_geometryMap[i].drawObject(matPtr, nOnceLodLevel);
	}
}
void						Mesh::RenderToShadowMap()
{CPUTimeT(Mesh);
	if(!m_bVisible)return;
	beforeObjectRegion();
	int	nMode	=	0;
	float3 vDist	=	GetCameraPtr()->GetEye() - this->m_world.GetPosition();
	float fDistance	=	vDist.R();
	if(m_bObjectRegion){
		float3 dir = m_objectRegion.sphere.center - g_manager.m_lightPos;
		if((dir.R()-m_objectRegion.sphere.radius)>g_manager.m_shadowViewSize)
			return;
		//if(!GetCameraPtr()->IsSphereVisible(this->m_objectRegion.sphere.center, m_objectRegion.sphere.radius))
		//	return;
		float fLodLength	=	m_groupData->m_fLodLength * m_objectRegion.sphere.radius;
		m_nOnceLodLevel		=	(int)((fDistance - this->m_objectRegion.sphere.radius) / fLodLength);
	}
	else
	{
		float fLodLength	=	m_groupData->m_fLodLength * GetRegion().sphere.radius;
		m_nOnceLodLevel	=	(int)(fDistance/ fLodLength);
	}
	if(m_nOnceLodLevel < 0)m_nOnceLodLevel = 0;
	else if(m_nOnceLodLevel > MAX_LOD_LEVEL_COUNT)m_nOnceLodLevel = MAX_LOD_LEVEL_COUNT;
	//
	Program*	reqWorld		=	0;
	if(m_bGeometryVisible){
		for(int i=0;i<this->m_geometryCount;i++)
		{
			if(!(m_geometryMap[i].m_geometryData->m_bIsAlphaBlend || !m_geometryMap[i].m_geometryData->m_bZWriteEnable))
				this->m_geometryMap[i].drawToShadowMap();
		}
	}
}
void						Mesh::Render(BOOL bAutoPush)		//所有设置都有效并进行渲染(包括矩阵数据，透明设置等等)
{CPUTimeT(Mesh);
	if(!m_bVisible)return;
	beforeObjectRegion();
	if(m_renderIdx==g_manager.m_renderIdx)
	{
		if(!this->m_bGeometryVisible)
			return;
		Program*	reqWorld		=	0;
		for(int i=0;i<this->m_geometryCount;i++)
		{
			if(!m_geometryMap[i].IsVisibled())continue;
			if(bAutoPush && (m_geometryMap[i].m_geometryData->m_bIsAlphaBlend || !m_geometryMap[i].m_geometryData->m_bZWriteEnable||(int)(this->m_fAlphaValue*1000)<1000))
			{
			}
			else
				this->m_geometryMap[i].drawObject();
		}
		g_manager->SetCullMode(Phantom::CullMode_CW);
		for(int i=0;i<m_drawMgrs.size();i++)
		{
			if(m_drawMgrs[i].mgr->m_bVisible)
			{
				if(m_drawMgrs[i].boneIndex>=0)
				{
					matrix4x4 mtemp = *m_actionDo.GetBoneMatrixByGlobalID(m_drawMgrs[i].boneIndex);
					matrixMultiply(&mtemp, &mtemp, &m_world);
					m_drawMgrs[i].mgr->Render(mtemp);
				}
				else
				{
					m_drawMgrs[i].mgr->Render(this->m_world);
				}
			}
		}
		return;
	}
	m_renderIdx=g_manager.m_renderIdx;
	int	nMode	=	0;
	float3 vDist	=	GetCameraPtr()->GetEye() - this->m_world.GetPosition();
	float fDistance	=	vDist.R();
	if(m_bObjectRegion&&m_groupData->m_geometrys.size()>0&&!this->m_groupData->m_geometrys[0]->m_bUseWorld){
		if(!GetCameraPtr()->IsSphereVisible(this->m_objectRegion.sphere.center, m_objectRegion.sphere.radius))
			return;
		float fLodLength	=	m_groupData->m_fLodLength * m_objectRegion.sphere.radius;
		m_nOnceLodLevel		=	(int)((fDistance - this->m_objectRegion.sphere.radius) / fLodLength);
		if(this->m_groupData->m_bEnableLODVisible && fDistance > (this->m_objectRegion.sphere.radius + g_manager->GetLODMinVisible())){
			if(fDistance >= (this->m_objectRegion.sphere.radius + m_groupData->m_fMaxVisibleLength) * g_manager->GetLODViewDelta()){
				m_bLodVisible	=	false;//lod不可见，直接返回
				return;
			}
		}
	}
	else
	{
		float fLodLength	=	m_groupData->m_fLodLength * GetRegion().sphere.radius;
		m_nOnceLodLevel	=	(int)(fDistance/ fLodLength);
	}
	if(m_nOnceLodLevel < 0)m_nOnceLodLevel = 0;
	else if(m_nOnceLodLevel > MAX_LOD_LEVEL_COUNT)m_nOnceLodLevel = MAX_LOD_LEVEL_COUNT;
	//
	Program*	reqWorld		=	0;
	if(m_bGeometryVisible){
		for(int i=0;i<this->m_geometryCount;i++)
		{
			if(!m_geometryMap[i].IsVisibled())continue;
			if(bAutoPush && (m_geometryMap[i].m_geometryData->m_bIsAlphaBlend || !m_geometryMap[i].m_geometryData->m_bZWriteEnable)||(int)(this->m_fAlphaValue*1000)<1000)
			{
				g_manager.PushAlphaObject(&m_geometryMap[i], true);
			}
			else
				this->m_geometryMap[i].drawObject();
		}
		//m_falseShadow.m_ptr				=	this;
		//if(m_groupData->m_bShadowVisible && bAutoPush)
		//{
		//	g_manager.PushAlphaObject(&m_falseShadow);
		//}
	}
	if(this->m_ribbons || this->m_particles)
	{
		if(m_ribbons)
			m_ribbons->SetEnable(this->m_actionDo.GetBoneGroup() && m_actionDo.GetBoneGroup()->m_rabbinEnable);
		if(this->m_actionDo.GetBoneGroup() && m_actionDo.GetBoneGroup()->m_rabbinEnable || m_particles)
		{
			g_manager.PushAlphaObject( &this->m_renderR);
		}
	}
	if(m_bGeometryVisible){
		for(int i=0;i<m_binds.size();i++)
		{
			m_binds[i].mesh->Render(bAutoPush);
		}
		g_manager->SetCullMode(Phantom::CullMode_CW);
		for(int i=0;i<m_drawMgrs.size();i++)
		{
			if(m_drawMgrs[i].mgr->m_bVisible)
			{
				if(m_drawMgrs[i].boneIndex>=0)
				{
					matrix4x4 mtemp = *m_actionDo.GetBoneMatrixByGlobalID(m_drawMgrs[i].boneIndex);
					matrixMultiply(&mtemp, &mtemp, &m_world);
					m_drawMgrs[i].mgr->Render(mtemp);
				}
				else
				{
					m_drawMgrs[i].mgr->Render(this->m_world);
				}
			}
		}
	}
}
void					Mesh::drawObject(const matrix4x4* matPtr)
{CPUTimeT(Mesh);
	if(this->m_ribbons&&this->m_actionDo.GetBoneGroup()&&this->m_actionDo.GetBoneGroup()->m_rabbinEnable&&this->m_bGeometryVisible)
	{
		for(int i=0;i<this->m_groupData->m_ribbons.size();i++)
		{
			m_ribbons[i].Render(1);
		}
	}
	if(this->m_particles)
	{
		for(int i=0;i<this->m_groupData->m_particles.size();i++)
		{
			if(!m_groupData->m_particles[i]->IsAlphaTest())
				m_particles[i].Render(1);
		}
	}
}
void					MeshR::drawObject(const matrix4x4* matPtr)
{CPUTimeT(MeshR);
	if(m_groupPtr)
		m_groupPtr->drawObject(matPtr);
}
const matrix4x4&		MeshR::GetWorldMatrix()
{CPUTimeNone();
	if(m_groupPtr)
		return m_groupPtr->GetWorldMatrix();
	return matrix4x4::s_identity();
}

void						Mesh::RenderShadow()
{CPUTimeNone();
	//if(!this->m_groupData->m_bShadowEnable)
	//	return;
	////if(!m_bOnetimeVisible)return;
	//IVspsObject* vspsObject		=	GetRenderDevicePtr()->GetVspsObjectPtr(0);
	//vspsObject->GetVspsMode(0, VspsSubMode_Blend)->sh->SetWorldMatrix( m_world );
	//vspsObject->GetVspsMode(0, VspsSubMode_Normal)->sh->SetWorldMatrix( m_world );

	//r.Render->SetCullMode(Phantom::CullMode_CW);
	//matrix4x4	matLocalAni;
	//VSPSMode*	reqWorld		=	0;
	//for(int i=0;i<this->m_geometryCount;i++)
	//{
	//	matrix4x4* mat				=	this->m_geometryMap[i].m_boneMatrix.boneMatrix;
	//	VSPSMode*	mode		=	0;
	//	if(mat)
	//	{
	//		mode					=	vspsObject->GetVspsMode(VspsMode_Shadow, VspsSubMode_Blend);
	//		if(!mode || !mode->sh)
	//			return;
	//		mode->sh->SetBoneMatrixs(mat, (unsigned int)m_geometryMap[i].m_geometryData->m_boneCount);
	//	}
	//	else
	//	{
	//		mode					=	vspsObject->GetVspsMode(VspsMode_Shadow, VspsSubMode_Normal);
	//	}
	//	if(m_geometryMap[i].m_boneMatrix.localBoneIdx >= 0)
	//	{
	//		const matrix4x4* mat	=	this->m_actionDo.GetBoneMatrix(m_geometryMap[i].m_boneMatrix.localBoneIdx);
	//		if(mat)
	//			matrixMultiply(&matLocalAni, mat, &m_world);
	//		else
	//			matLocalAni			=	m_world;
	//		mode->sh->SetWorldMatrix(matLocalAni);
	//		m_geometryMap[i].RenderShadow( mode, r );
	//		reqWorld				=	mode;
	//	}
	//	else
	//	{
	//		if(reqWorld)
	//		{
	//			reqWorld->sh->SetWorldMatrix(m_world);
	//			reqWorld	=	0;
	//		}
	//		m_geometryMap[i].RenderShadow( mode, r );
	//	}
	//}
}

int								Mesh::PickIsMoveRegion(IntersectData* inter)
{CPUTimeT(Mesh);
	if(!m_bVisible || !m_bEnabled)
		return 0;
	int ret	=	0;
	for(int i=0;i<this->m_geometryCount;i++){
		if(this->m_geometryMap[i].m_geometryData->m_bIsDisable || this->m_geometryMap[i].m_geometryData->m_bIsHeight)
		{
			matrix4x4	matLocalAni;
			matrix4x4*	matWorld		=	&m_world;
			if(m_geometryMap[i].m_boneMatrix.localBoneIdx >= 0)
			{
				const matrix4x4* mat	=	this->m_actionDo.GetBoneMatrix(m_geometryMap[i].m_boneMatrix.localBoneIdx);
				if(mat)
				{
					matrixMultiply(&matLocalAni, mat, &m_world);
					matWorld				=	&matLocalAni;
				}
			}
			if(m_geometryMap[i].pickGeometry( inter, matWorld , this))
			{
				if(this->m_geometryMap[i].m_geometryData->m_bIsDisable)
					ret	= -1;
				else
					ret = 1;
			}
		}
	}
	return ret;
}

SubMesh*					Mesh::SearchSubmesh(const char* szMeshName)	//根据名字搜索渲染器
{CPUTimeT(Mesh);
	if(!m_geometryMap)
		return 0;
	for(int i=0;i<m_geometryCount;i++)
	{
		if(stricmp(m_geometryMap[i].GetName(), szMeshName) == 0)
			return &m_geometryMap[i];
	}
	return 0;
}

char							Mesh::GetHeight(IntersectData* inter, matrix4x4* matin, PObjectRegion& region)
{CPUTimeT(Mesh);
	if(!this->m_groupData->m_bExistHeight&&!m_groupData->m_bExistCollision)
		return false;
	if(!matin)
		matin = &m_world;
	const float3& orig = inter->GetOriginal();
	const float3& src = matin->GetPosition();
	float2 vR(src.x-orig.x,src.y-orig.y);
	if((vR.x*vR.x+vR.y*vR.y)>region.sphere.radius2x)
		return false;
	char bRet	=	false;
	for(int i=0;i<m_geometryCount;i++)
	{
		SubMesh* collision		=	&m_geometryMap[i];//.at(i);
		if(!(collision->m_geometryData->m_bIsHeight||collision->m_geometryData->m_bIsCollision))
			continue;
		matrix4x4*	matWorld		=	matin;
		if(collision->m_geometryData->m_bUseWorld)
		{
			matWorld				=	&collision->m_worldLocal;
			const float3& src = matWorld->GetPosition();
			float2 vR(src.x-orig.x,src.y-orig.y);
			if((vR.x*vR.x+vR.y*vR.y)>collision->m_sphere.radius2x)
				continue;
		}
		else if(collision->m_boneMatrix.localBoneIdx >= 0)
		{
			matWorld				=	&collision->m_worldLocal;
		}
		//
		if(collision->pickGeometry( inter, matWorld , this))
			bRet	=	true;//return true;
	}
	return bRet;
}
char							Mesh::GetHeight(IntersectData* inter)
{CPUTimeNone();
	if(!m_bVisible)
		return false;
	if(!this->m_bObjectRegion)
		return false;
	beforeObjectRegion();
	return GetHeight(inter, 0, this->m_objectRegion);
}

char							Mesh::IsPointDisableMove(const float3& pos, float* retHeight)
{CPUTimeT(Mesh);
	if(!m_bVisible)
		return false;
	for(int i=0;i<this->m_geometryCount;i++)
	{
		SubMesh* collision		=	&m_geometryMap[i];
		if(!collision->m_geometryData->m_bIsDisable)
			continue;
		//
		IntersectData* inter	=	GetCameraPtr()->GetIntersectPtr();
		inter->begin(float3(pos.x, pos.y, pos.z + 1.0f), float3(0, 0, -1), To3DValue(1000000.0f));

		matrix4x4	matLocalAni;
		matrix4x4*	matWorld		=	&m_world;
		if(collision->m_geometryData->m_bUseWorld)
		{
			matWorld				=	&collision->m_worldLocal;
			if(!GetCameraPtr()->IsSphereVisible(collision->m_sphere.center, collision->m_sphere.radius))
				continue;
		}
		else if(collision->m_boneMatrix.localBoneIdx >= 0)
		{
			matWorld				=	&matLocalAni;
		}
		if(collision->pickGeometry( inter, matWorld , this))
		{
			if(retHeight)
				*retHeight	=	inter->GetPickInfo().vPick.z;
			return true;
		}
	}
	return false;
}
BOOL							Mesh::GetCollisionBox(AABox& ret)
{CPUTimeT(Mesh);
	if(this->m_actionDo.IsInitialized())
	{
		MeshActionRange* c = GetCurrentAction();
		if(!c||c->m_actionPtr->m_keyBoxs.size()==0)
			return false;
		int nStartFrame		=	c->m_startFrame;
		int nEndFrame		=	c->m_endFrame;
		if(nEndFrame < 0)
			nEndFrame		=	c->GetKeyCount();
		int nKeyCount		=	(nEndFrame - nStartFrame);
		if(nKeyCount <= 0)
			return false;
		float fPlayTime			=	fmodf(m_fSetFrameIndex, (float)nKeyCount);
		if(fPlayTime < 0)
			fPlayTime		+=	(float)nKeyCount;
		int beginIndex	=	(int)fPlayTime;
		beginIndex += nStartFrame;
		beginIndex %= c->m_actionPtr->m_keyBoxs.size();//c->m_actionPtr->GetKeyCount();
		ret = c->m_actionPtr->m_keyBoxs[beginIndex];
		return true;
	}
	return false;
}
char							Mesh::PickRay(IntersectData* p, matrix4x4* matin, PObjectRegion& region, MeshPick mp)
{CPUTimeT(Mesh);
	if(mp==MeshPick_Height&&!this->GetDataPtr()->m_bExistHeight)
		return false;
	else if((mp==MeshPick_Collision&&!this->GetDataPtr()->m_bExistCollision)&&!this->m_actionDo.IsInitialized())
		return false;
	if(!matin)
		matin = &m_world;
	if(!ObjectInLinear(p, region.sphere, 0))
		return false;
	AABox& box = region.aabox;
	if(!IntersectBox(p->GetOriginal(), p->GetDir(), box._min, box._max, 0))
		return false;
	if(this->m_actionDo.IsInitialized())
	{
		MeshActionRange* c = GetCurrentAction();
		if(!c||c->m_actionPtr->m_keyBoxs.size()==0)
		{
			LogInfo("system->error:%s", utf8("请使用编辑器“重新计算动作盒子”"));
			return false;
		}
		int nStartFrame		=	c->m_startFrame;
		int nEndFrame		=	c->m_endFrame;
		if(nEndFrame < 0)
			nEndFrame		=	c->GetKeyCount();
		int nKeyCount		=	(nEndFrame - nStartFrame);
		if(nKeyCount <= 0)
			return false;
		float fPlayTime			=	fmodf(m_fSetFrameIndex, (float)nKeyCount);
		if(fPlayTime < 0)
			fPlayTime		+=	(float)nKeyCount;
		int beginIndex	=	(int)fPlayTime;
		beginIndex += nStartFrame;
		beginIndex %= c->m_actionPtr->GetKeyCount();
		AABox& box2 = c->m_actionPtr->m_keyBoxs[beginIndex];
		if(!IntersectBox(p->GetOriginal(), p->GetDir(), box2._min, box2._max, matin))
			return false;
		//return false;
	}
	BOOL bRet = false;
	for(size_t i=0;i<this->m_geometryCount;i++)
	{
		SubMesh* collision		=	&m_geometryMap[i];//m_collisionGeometrys.at(i);
		BOOL bCollisionBox = false;
		if(mp==MeshPick_Height){
			if(!collision->GetDataPtr()->m_bIsHeight)
				continue;
		}
		else if(mp==MeshPick_Collision){
			if(!collision->IsCollision())
				continue;
		}
		else
			bCollisionBox = true;
		matrix4x4*	matWorld	=	matin;
		if(collision->m_geometryData->m_bUseWorld)
		{
			matWorld				=	&collision->m_worldLocal;
			if(!ObjectInLinear(p, collision->m_sphere, 0))
				continue;
			if(mp&MeshPick_Box){
				AABox& box = collision->m_geometryData->m_polygons->m_objectRegion.aabox;
				if(!IntersectBox(p->GetOriginal(), p->GetDir(), box._min, box._max, matWorld))
					continue;
			}
			else if(mp&MeshPick_Mesh){
				if(collision->pickGeometry(p,matWorld,0))
				{
					p->m_info.name = collision->GetName();
					bRet = true;
					continue;
				}
			}
		}
		else if(collision->m_boneMatrix.localBoneIdx >= 0)
		{
			matWorld				=	&collision->m_worldLocal;
		}
		if(!bCollisionBox){
			if(collision->pickGeometry( p, matWorld , this))
			{
				p->m_info.name = collision->GetName();
				bRet = true;
				continue;
			}
		}
		else{
			if(mp&MeshPick_Box){
				AABox& box = collision->m_geometryData->m_polygons->m_objectRegion.aabox;
				if(IntersectBox(p->GetOriginal(), p->GetDir(), box._min, box._max, matWorld))
				{
					p->m_info.name = collision->GetName();
					bRet = true;
					continue;
				}
			}
			else{
				if(collision->pickGeometry(p,matWorld,0))
				{
					p->m_info.name = collision->GetName();
					bRet = true;
					continue;
				}
			}
		}
	}
	return bRet;
}
char							Mesh::PickRay(IntersectData* p, MeshPick mp)	//只点击有collision盒子的物体
{CPUTimeNone();
	if(!m_bVisible || !m_bEnabled)
		return false;
	beforeObjectRegion();
	return PickRay(p, &m_world, m_objectRegion, mp);
}
void							Mesh::Pause(char bPause)
{CPUTimeNone();
	m_bPause	=	bPause;
	if(!m_bPause)
		ResetRibbonParticle();
}

void					Mesh::ReplaceImage(const char* szImageSrc, const char* szImageReplace)
{CPUTimeT(Mesh);
	for(int i=0;i<m_geometryCount;i++)
		this->m_geometryMap[i].ReplaceImage(szImageSrc, szImageReplace);
}
char					Mesh::SetActionType(int type, int nLoop, int index)
{CPUTimeNone();
	m_nPlayType = index;
	int id = this->m_groupData->GetActionIDByType(type, index);
	this->m_fMeshIndex = 0;
	if(id==0)
		return false;
	return SetCurrentAction(id, nLoop);
}
VOID					Mesh::BindDraw(LPCSTR szBoneName, UIDrawMgr* mgr)
{CPUTimeNone();
	safe_addref(mgr);
	BindBoneDraw m;
	m.mgr = mgr;
	m.boneIndex = 
	m.boneIndex = -1;
	if(szBoneName&&szBoneName[0])
		m.boneIndex = this->GetBoneIDLike(szBoneName);
	m_drawMgrs.push_back(m);
}
RenderNode*				Mesh::Clone()
{CPUTimeNone();
	if(!this->m_groupData)
		return NULL;
	Mesh* group = new Mesh(m_groupData);
	if(!group->CreateGeometry())
	{
		delete group;
		return 0;
	}
	return group;
}

};
