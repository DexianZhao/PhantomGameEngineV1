//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   Effect.cpp
软件名称   :   幻影游戏引擎

设计编写者 :   赵德贤(英文:ZhaoDexian)
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "PhantomEffect.h"
namespace Phantom{
void	Mul(matrix4x4& out2, const matrix4x4& m1, const matrix4x4& m2);
void	Mul(matrix4x4& out2, const matrix4x4& m2);
void	matrixTranslate(matrix4x4& mat, const float3& v);
float3 TransVector(matrix4x4& mat, const float3& v);
void RotateX(matrix4x4& mat, const float rads);
void RotateY(matrix4x4& mat, const float rads);
void RotateZ(matrix4x4& mat, const float rads);
void Scale(matrix4x4& mat, const float a, const float b, const float c);
void Translate(matrix4x4& mat, const float3& v);
float3 TransVector(matrix4x4& mat, const float3& v);
float3 &  RotateX(float3& v, float rad);
float3 &  RotateY(float3& v, float rad);
float3 &  RotateZ(float3& v, float rad);
float Mag(const float3& v);
matrix4x4 DirToMatrix(float3 sDir);
BOOL BoundPointSphere(float3& sPosDot, float fDistance, float3& sPosA, float3& sPosB);


enum EStart
{
	EPOINT_EF, ELINE_EF, EPLANE_EF, ESPACE_EF
};

enum EMove
{
	EEXPANSION, ECONCENTRATION, ERANDOM, EUSER
};

enum ECreate
{
	ETIME_CREATE, EONECREATE, ECONTINUE //EFLAG, EDISTANCE, 
};

enum EScale
{
	ESCALEALL, ESCALEX, ESCALEY, ESCALEZ
};

enum EBlend
{
	EBLEND_NORMAL, EBLEND_ALPHAKEY, EBLEND_ALPHA, EBLEND_ALPHA_ADD, EBLEND_MODULATE, EBLEND_FORCE_ADD, EBLEND_FORCE_SUBTRACT
};


AotmicData::AotmicData()
{
	m_pArrayVer	=	0;
	m_pArrayUV	=	0;
	m_pArrayColor	=	0;
	m_sMatExpansion = m_sMat = m_sMatPrev = m_sMatStart = matrix4x4::s_identity();
	m_sRotate = m_sScale = m_sPos = m_sExpansionPos = m_sExpansionDir = float3(0, 0, 0);
	m_fExpansionSpeed = m_fPersentage = m_fLivedTime = m_fAlpha = 0;
	m_nNowTexture = 0;
	m_fNowFrame = 0;
}

static float* s_pColor;

// Color
static float s_fColorAdd[16] = 
{
	1.0f, 1.0f,	1.0f, 1.0f,		// V0
	1.0f, 1.0f, 1.0f, 1.0f,		// V1
	1.0f, 1.0f, 1.0f, 1.0f,		// V2
	1.0f, 1.0f, 1.0f, 1.0f		// V3
};

// NoColor
static float s_fAlphaOnly[16] = 
{
	1.0f, 1.0f,	1.0f, 1.0f,		// V0
	1.0f, 1.0f, 1.0f, 1.0f,		// V1
	1.0f, 1.0f, 1.0f, 1.0f,		// V2
	1.0f, 1.0f, 1.0f, 1.0f		// V3
};

static float s_fVer[12]	= 
{
	-10.0f, -10.0f, 0.0f,		// V0
	+10.0f, -10.0f, 0.0f,		// V1
	+10.0f, +10.0f, 0.0f,		// V2
	-10.0f, +10.0f, 0.0f		// V3
};

#define SETCOLOR(a)		{ s_fColorAdd[3] = a; s_fColorAdd[7] = a; s_fColorAdd[11] = a; s_fColorAdd[15] = a; }

static float	s_fPlusMoveX;
static float	s_fPlusMoveY;

static float	s_fCos;
static float	s_fSin;

static float	s_fScale;

static float	s_fAlpha;

static int		s_nOldTexID;

matrix4x4 DirToMatrix(float3 sDir)
{
	matrix4x4		sMatDir;
	matrix4x4		sMatDirX;
	matrix4x4		sMatDirY;
	float		fRadianY;
	float		fRadianX;

	sDir.normalize();
	fRadianY = acosf(sDir.y)+PI/2.0;
	fRadianX = asinf(sDir.z);
	if( sDir.x < 0.0f )		fRadianY = -fRadianY;
//	if( sDir.y < 0.0f )		fRadianX = -fRadianX;
	RotateZ(sMatDirY,-fRadianY );
	RotateX(sMatDirX,fRadianX );
	sMatDir = sMatDirY * sMatDirX;
	return sMatDir;
}

BOOL BoundPointSphere(float3& sPosDot, float fDistance, float3& sPosA, float3& sPosB)
{
	EPlane		sPlane;
	float3		sDir = sPosB - sPosA;
	float3		sPosCross;
	float		fMultPlaneB;
	float		fRate;


	sDir.normalize();

	sPlane.a	= sDir.x;
	sPlane.b	= sDir.y;
	sPlane.c	= sDir.z;
	sPlane.d	= -sDir .dot(sPosDot);

	fMultPlaneB	= sPlane.MultiPlanef(sPosB);

	if( fMultPlaneB >= 0.0f )
	{
		fRate	=	(fMultPlaneB) / (
					sPlane.a * (sPosB.x - sPosA.x) + 
					sPlane.b * (sPosB.y - sPosA.y) + 
					sPlane.c * (sPosB.z - sPosA.z) );

		sPosCross.x	= (sPosA.x * fRate) + sPosB.x * (1 - fRate);
		sPosCross.y	= (sPosA.y * fRate) + sPosB.y * (1 - fRate);
		sPosCross.z	= (sPosA.z * fRate) + sPosB.z * (1 - fRate);

		sPosCross -= sPosDot;

		if( Mag(sPosCross) < fDistance )
		{
			return TRUE;
		}
	}

	return FALSE;

}

void EffectLine::Default()
{CPUTime(EffectLine);
	memset(m_szLineName, 0, 32);
	memset(m_szTextureName, 0, 32);

	m_bLighting			= FALSE;
	m_fWidthMax			= 5.0f;
	m_fWidthMin			= 3.0f;
	m_nNumSegmentLarge	= 5;
	m_nNumSegmentSmall	= 10;
	m_fTensionLargeMax	= 60.0f;
	m_fTensionLargeMin	= 0.0f;
	m_fTensionSmallMax	= 20.0f;
	m_fTensionSmallMin	= 0.0f;
	
}

void EffectLine::Open(char* szFileName)
{CPUTime(EffectLine);
	CFileHelperR r;
	if(!r.open(szFileName))
		return;
	r.pStream->read(this, sizeof(EffectLine));
}

void EffectLine::Save(char *szFileName)
{CPUTime(EffectLine);
	CFileHelperW w;
	if(!w.create(szFileName))
		return;
	w.pStream->write(this, sizeof(EffectLine));
}

void	Mul(matrix4x4& out2, const matrix4x4& m1, const matrix4x4& m2){
	float* m = (float*)m1.m;
	float* in2 = (float*)m2.m;
	matrix4x4 o;
	float* out = (float*)o.m;
	out[0] = m[0]*in2[0] + m[4]*in2[1] + m[8]*in2[2] + m[12]*in2[3];
	out[1] = m[1]*in2[0] + m[5]*in2[1] + m[9]*in2[2] + m[13]*in2[3];
	out[2] = m[2]*in2[0] + m[6]*in2[1] + m[10]*in2[2] + m[14]*in2[3];
	out[3] = m[3]*in2[0] + m[7]*in2[1] + m[11]*in2[2] + m[15]*in2[3];
	out[4] = m[0]*in2[4] + m[4]*in2[5] + m[8]*in2[6] + m[12]*in2[7];
	out[5] = m[1]*in2[4] + m[5]*in2[5] + m[9]*in2[6] + m[13]*in2[7];
	out[6] = m[2]*in2[4] + m[6]*in2[5] + m[10]*in2[6] + m[14]*in2[7];
	out[7] = m[3]*in2[4] + m[7]*in2[5] + m[11]*in2[6] + m[15]*in2[7];
	out[8] = m[0]*in2[8] + m[4]*in2[9] + m[8]*in2[10] + m[12]*in2[11];
	out[9] = m[1]*in2[8] + m[5]*in2[9] + m[9]*in2[10] + m[13]*in2[11];
	out[10] = m[2]*in2[8] + m[6]*in2[9] + m[10]*in2[10] + m[14]*in2[11];
	out[11] = m[3]*in2[8] + m[7]*in2[9] + m[11]*in2[10] + m[15]*in2[11];
	out[12] = m[0]*in2[12] + m[4]*in2[13] + m[8]*in2[14] + m[12]*in2[15];
	out[13] = m[1]*in2[12] + m[5]*in2[13] + m[9]*in2[14] + m[13]*in2[15];
	out[14] = m[2]*in2[12] + m[6]*in2[13] + m[10]*in2[14] + m[14]*in2[15];
	out[15] = m[3]*in2[12] + m[7]*in2[13] + m[11]*in2[14] + m[15]*in2[15];
	memcpy(out2.m, out, sizeof(o.m));
}
//
void RotateX(matrix4x4& mat, const float rads)
{
	float	cosine, sine;

	cosine = (float) cos(rads);
	sine = (float) sin(rads);
    mat.identity();
    mat.m[1][1] = cosine;
	mat.m[2][2] = cosine;
	mat.m[1][2] = -sine;
	mat.m[2][1] = sine;
} 

void RotateY(matrix4x4& mat, const float rads)
{
	float	cosine, sine;

	cosine = (float) cos(rads);
	sine = (float) sin(rads);
    mat.identity();
    mat.m[0][0] = cosine;
	mat.m[2][2] = cosine;
	mat.m[0][2] = sine;
	mat.m[2][0] = -sine;
} 

void RotateZ(matrix4x4& mat, const float rads)
{
	float	cosine, sine;

	cosine = (float) cos(rads);
	sine = (float) sin(rads);
	mat.identity();
    mat.m[0][0] = cosine;
	mat.m[1][1] = cosine;
	mat.m[0][1] = -sine;
	mat.m[1][0] = sine;
} 
//void	Mul(matrix4x4& out2, const matrix4x4& m1, const matrix4x4& m2){
//	float* m = (float*)m1.m;
//	float* in2 = (float*)m2.m;
//	matrix4x4 o;
//	float* out = (float*)o.m;
//	out[0] = m[0]*in2[0] + m[4]*in2[1] + m[8]*in2[2] + m[12]*in2[3];
//	out[1] = m[1]*in2[0] + m[5]*in2[1] + m[9]*in2[2] + m[13]*in2[3];
//	out[2] = m[2]*in2[0] + m[6]*in2[1] + m[10]*in2[2] + m[14]*in2[3];
//	out[3] = m[3]*in2[0] + m[7]*in2[1] + m[11]*in2[2] + m[15]*in2[3];
//	out[4] = m[0]*in2[4] + m[4]*in2[5] + m[8]*in2[6] + m[12]*in2[7];
//	out[5] = m[1]*in2[4] + m[5]*in2[5] + m[9]*in2[6] + m[13]*in2[7];
//	out[6] = m[2]*in2[4] + m[6]*in2[5] + m[10]*in2[6] + m[14]*in2[7];
//	out[7] = m[3]*in2[4] + m[7]*in2[5] + m[11]*in2[6] + m[15]*in2[7];
//	out[8] = m[0]*in2[8] + m[4]*in2[9] + m[8]*in2[10] + m[12]*in2[11];
//	out[9] = m[1]*in2[8] + m[5]*in2[9] + m[9]*in2[10] + m[13]*in2[11];
//	out[10] = m[2]*in2[8] + m[6]*in2[9] + m[10]*in2[10] + m[14]*in2[11];
//	out[11] = m[3]*in2[8] + m[7]*in2[9] + m[11]*in2[10] + m[15]*in2[11];
//	out[12] = m[0]*in2[12] + m[4]*in2[13] + m[8]*in2[14] + m[12]*in2[15];
//	out[13] = m[1]*in2[12] + m[5]*in2[13] + m[9]*in2[14] + m[13]*in2[15];
//	out[14] = m[2]*in2[12] + m[6]*in2[13] + m[10]*in2[14] + m[14]*in2[15];
//	out[15] = m[3]*in2[12] + m[7]*in2[13] + m[11]*in2[14] + m[15]*in2[15];
//	memcpy(out2.m, out, sizeof(o.m));
//}
void	Mul(matrix4x4& out2, const matrix4x4& m2)
{
	Mul(out2, out2, m2);
}
void	matrixTranslate(matrix4x4& mat, const float3& v){
	mat.identity();
	mat.SetPosition(v);
}

float3 &  RotateX(float3& v, float rad)
{
	float _x = v.y, _y = v.z;
	float vsin = sin(rad), vcos = cos(rad);
	
	v.y = + _x*vcos - _y*vsin;
	v.z = + _x*vsin + _y*vcos;

	return v;
}

float3 &  RotateY(float3& v, float rad)
{
	float _x = v.x, _y = v.z;
	float vsin = sin(rad), vcos = cos(rad);

	v.x = + _x*vcos + _y*vsin;
	v.z = - _x*vsin + _y*vcos;

	return v;
}
float Mag(const float3& v)
{
	return (float) sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

float3 &  RotateZ(float3& v, float rad)
{
	float _x = v.x, _y = v.y;
	float vsin = sin(rad), vcos = cos(rad);

	v.x = + _x*vcos - _y*vsin;
	v.y = + _x*vsin + _y*vcos;

	return v;
}



float3 TransVector(matrix4x4& mat, const float3& v)
{
	float hvec[4];
	
	hvec[0] = v.x*mat.m[0][0] + v.y*mat.m[1][0] + v.z*mat.m[2][0] + mat.m[3][0];
	hvec[1] = v.x*mat.m[0][1] + v.y*mat.m[1][1] + v.z*mat.m[2][1] + mat.m[3][1];
	hvec[2] = v.x*mat.m[0][2] + v.y*mat.m[1][2] + v.z*mat.m[2][2] + mat.m[3][2];
	hvec[3] = v.x*mat.m[0][3] + v.y*mat.m[1][3] + v.z*mat.m[2][3] + mat.m[3][3];

	float dv = 1.0f/hvec[3];
	float3 ret(hvec[0]*dv, hvec[1]*dv, hvec[2]*dv);

	return ret;
} 

void Translate(matrix4x4& mat, const float dx, const float dy, const float dz)
{
	mat.identity();
	mat.m[3][0] = dx;
	mat.m[3][1] = dy;
	mat.m[3][2] = dz;
} 

void Translate(matrix4x4& mat, const float3& v)
{
	mat.identity();
	mat.m[3][0] = v.x;
	mat.m[3][1] = v.y;
	mat.m[3][2] = v.z;
}

void Scale(matrix4x4& mat, const float a, const float b, const float c)
{
	mat.identity();
	mat.m[0][0] = a;
	mat.m[1][1] = b;
	mat.m[2][2] = c;
} 

float3 EPlane::SetPlane(float3& sStartPos, float3& sEndPos)
{CPUTime(EPlane);
	float3 sDir;

	sDir= sEndPos - sStartPos;

	if( !Mag(sDir) )
	{
		sDir.normalize();

		a = 0.0f;
		b = 0.0f;
		c = 0.0f;
		d = 0.0f;

		return sDir;
	}
	else
	{
		sDir.normalize();

		a = sDir.x;
		b = sDir.y;
		c = sDir.z;
		d = -sDir.dot(sEndPos);

		return sDir;
	}
	
}

bool EPlane::MultiPlane(float3& sPos)
{CPUTime(EPlane);
	return  ( sPos.x*a + sPos.y*b + sPos.z*c + d >= 0.0f );

}

float EPlane::MultiPlanef(float3& sPos)
{CPUTime(EPlane);
	return  sPos.x*a + sPos.y*b + sPos.z*c + d;

}
void AtomicBase::_SetColor(float3 rgb, float a, BOOL bColor)
{CPUTime(AtomicBase);
	if( bColor )
	{
		s_fColorAdd[0] = rgb.x;		s_fColorAdd[1] = rgb.y;		s_fColorAdd[2] = rgb.z;		s_fColorAdd[3] = a;
		s_fColorAdd[4] = rgb.x;		s_fColorAdd[5] = rgb.y;		s_fColorAdd[6] = rgb.z;		s_fColorAdd[7] = a;
		s_fColorAdd[8] = rgb.x;		s_fColorAdd[9] = rgb.y;		s_fColorAdd[10] = rgb.z;	s_fColorAdd[11] = a;
		s_fColorAdd[12] = rgb.x;	s_fColorAdd[13] = rgb.y;	s_fColorAdd[14] = rgb.z;	s_fColorAdd[15] = a;

		s_pColor = s_fColorAdd;
	}
	else
	{
		s_fAlphaOnly[3] = a;
		s_fAlphaOnly[7] = a;
		s_fAlphaOnly[11] = a;
		s_fAlphaOnly[15] = a;

		s_pColor = s_fAlphaOnly;
	}

}




static float3		s_sVer0 = float3(0.0f, 0.0f, 10.0f);
static float3		s_sVer1 = float3(0.0f, 0.0f, -10.0f);
static float3		s_sPosStart;
static float3		s_sPosEnd;



void AtomicLine::Init(char* szFileName)
{CPUTime(AtomicLine);
	m_line.Open(szFileName);
	FileName file;
	if(m_textureFile.t[0])
		m_textureFile.splitpath(0, &file, 0);
	else
	{
		FileName temp = m_line.m_szTextureName;
		temp.setIsFile();
		temp.splitpath(0, &file, 0);
	}
	strcat(file.t, ".tga");
	TexturePtr tex;
	if(LoadTexture(tex, file.t, szFileName, true))
	{
		m_pTex = tex.GetPtr();
		safe_addref(m_pTex);
	}
}

void AtomicLine::Release()
{CPUTime(AtomicLine);
	safe_release(m_pTex);
}

AotmicData* AtomicLine::CreateAtomic(SProperty* pPro)
{CPUTime(AtomicLine);
	AotmicData* pAtomic = new AotmicData();
	if( !m_line.m_bLighting )
	{
		// Create
		pAtomic->m_pArrayVer	= new float3[2];
		pAtomic->m_pArrayUV		= new EffectUV[2];
		pAtomic->m_pArrayUV[0].Set(0.0f, 1.0f);
		pAtomic->m_pArrayUV[1].Set(1.0f, 1.0f);
	}
	else
	{
		pAtomic->m_pArrayVer	= NULL;
		pAtomic->m_pArrayUV		= NULL;
		pAtomic->m_pArrayColor	= NULL;
	}

	return pAtomic;

}

void AtomicLine::Update(EffectNode* pNode)
{CPUTime(AtomicLine);
	CDynamicArray<AotmicData*>&		vAtomic = pNode->m_vNodeActive;
	SProperty*			pPro	= &pNode->m_pPro;
	AotmicData*			pAtomic;
	int					nNumAtomic = vAtomic.size();
	// Multiply Vertex
	if( !m_line.m_bLighting )
	{
		float	fPlusV	= g_manager.GetElapsedTime() / pPro->fLifeTime;
		for(int i=0;i<vAtomic.size();i++)//it = it_start; it != it_end; ++it)
		{
			pAtomic	= vAtomic[i];//(*it);
			pAtomic->m_pArrayUV[0].v	-= fPlusV;
			pAtomic->m_pArrayUV[1].v	-= fPlusV;
			pAtomic->m_pArrayVer[0]		= TransVector(pAtomic->m_sMat,s_sVer0);
			pAtomic->m_pArrayVer[1]		= TransVector(pAtomic->m_sMat,s_sVer1);
		}
	}
	else
	{
		// Find InheritEndPos
		EffectNode*	pNodeParent = pNode;
		bool	bInheritEndPos = false;
		while( pNodeParent->m_pNodeParent )
		{
			pNodeParent = pNodeParent->m_pNodeParent;
			if( pNodeParent->m_pPro.bInheritEndPos )
			{
				bInheritEndPos	= true;
				break;
			}
		}

		if( bInheritEndPos )
		{
			if( pNodeParent->m_pPro.bDirEqualNodeDir )
			{
				float3		sDir;
				float		fDistance;

				s_sPosStart	= pNodeParent->m_sPosStart;
				s_sPosEnd	= pNodeParent->m_sPosEnd;

				sDir		= s_sPosEnd - s_sPosStart;
				fDistance	= Mag(sDir);
				s_sPosEnd	= s_sPosStart + float3(0.0f, 0.0f, fDistance);
			}
			else
			{
				s_sPosStart	= pNodeParent->m_sPosStart;
				s_sPosEnd	= pNodeParent->m_sPosEnd;
			}
		}
		else
		{
			s_sPosStart		= pNode->m_sPosStart;
			s_sPosEnd		= pNode->m_sPosEnd;
		}

		if( pNode->m_pPro.bStartPos )		s_sPosStart	+= pNode->m_pPro.sStartPos;
		if( pNode->m_pPro.bEndPos )		s_sPosEnd	+= pNode->m_pPro.sEndPos;


		int nNumArray = (m_line.m_nNumSegmentSmall + 1) * 4 * 2;

		for(int i=0;i<vAtomic.size();i++)//it = it_start; it != it_end; ++it)
		{
			pAtomic	= vAtomic[i];//(*it);

			if( pAtomic->m_pArrayVer == NULL )
			{
				_SetLightingAtomic(pAtomic, pPro);
				for(int i = 0; i < nNumArray; ++i)
				{
					pAtomic->m_pArrayVer[i]	= TransVector(pAtomic->m_sMat,pAtomic->m_pArrayVer[i]);
				}
			}
		}
	}

}
static CDynamicArray<unsigned int>	g_tempColors;
void DrawLineElement(Texture* tex, color4* colors,  float2* uvs, float3* verts, int count)
{
	if(!verts)
		return;
	Program* vsps = g_manager.GetCurrProgram();
	if(tex)
		vsps->SetTexture(Phantom::TextureIndex_0, tex);
	//
	g_tempColors.SetArrayCount(count);
	for( int i = 0; i < count; i++ )
	{
		g_tempColors[i] = colors[i];
	}
	if( count > 0 )
		g_manager->DrawNoIndexed(vsps, DrawVInfos(count, verts, g_tempColors.GetPtr(), uvs), count, Phantom::DrawMode_TriangleStrip);
}

void AtomicLine::Draw(CDynamicArray<AotmicData*>& vAtomic, EffectNode* node)
{CPUTime(AtomicLine);
	SProperty* pPro = &node->m_pPro;
	AotmicData*	pAtomic;
	float3*		pVer;
	EffectUV*		pUV;
	float3		sRGB(1.0f, 1.0f, 1.0f);
	float*		fRGB;
	if( pPro->bAtomicInstTwoSide )
		g_manager->SetCullMode(Phantom::CullMode_Double);
	_SetBlend(pPro->eAlphaBlend);
	if(!pPro->bAlphaZBuffer)
		g_manager->SetZWriteEnable(false);
	if( pPro->bAlphaColor )			fRGB	= pPro->sAlphaColor;
	else							fRGB	= sRGB;

	if( !m_line.m_bLighting )
	{
		static CDynamicArray<color4> colors;
		static CDynamicArray<float3> verts;
		static CDynamicArray<float2> uvs;
		colors.SetArrayCount(vAtomic.size() * 2);
		verts.SetArrayCount(vAtomic.size() * 2);
		uvs.SetArrayCount(vAtomic.size() * 2);
		int nIndex = 0;
		for(int i=0;i<vAtomic.size();i++)
		{
			pAtomic	= vAtomic[i];
			pVer	= pAtomic->m_pArrayVer;
			pUV		= pAtomic->m_pArrayUV;
			colors[nIndex].setRGBA(fRGB[0], fRGB[1], fRGB[2], pAtomic->m_fAlpha);
			uvs[nIndex].setxy(pUV[0].u, pUV[0].v);
			verts[nIndex] = pVer[0];
			nIndex++;
			colors[nIndex].setRGBA(fRGB[0], fRGB[1], fRGB[2], pAtomic->m_fAlpha);
			uvs[nIndex].setxy(pUV[1].u, pUV[1].v);
			verts[nIndex] = pVer[1];
			nIndex++;
		}
		DrawLineElement(this->m_pTex, colors.GetPtr(), uvs.GetPtr(), verts.GetPtr(), nIndex);
	}
	else
	{
		int nNumArray = (m_line.m_nNumSegmentSmall + 1) * 4 * 2;
	}
	if(!pPro->bAlphaZBuffer)
		g_manager->SetZWriteEnable(true);
}

const char* AtomicLine::GetFileName()
{CPUTime(AtomicLine);
	return m_line.m_szLineName;

}

void AtomicLine::DestroyAtomic(AotmicData* pAtomic)
{CPUTime(AtomicLine);
	if(!pAtomic)return;
	if(pAtomic->m_pArrayUV)
		delete [] pAtomic->m_pArrayUV;
	pAtomic->m_pArrayUV = 0;
	if(pAtomic->m_pArrayVer)
		delete [] pAtomic->m_pArrayVer;
	pAtomic->m_pArrayVer = 0;
	if(pAtomic->m_pArrayColor)
		delete[] pAtomic->m_pArrayColor;
	pAtomic->m_pArrayColor = 0;
	delete pAtomic;
}

inline void AtomicLine::_SetLightingAtomic(AotmicData *pAtomic, SProperty *pPro)
{
	float3		sDirX(1.0f, 0.0f, 0.0f);
	float3		sDirZ(0.0f, 0.0f, 1.0f);
	float3		sPosZero(0.0f, 0.0f, 0.0f);
//	float3		sDir = pPro->sEndPos - pPro->sStartPos;
	float3		sDir = s_sPosEnd - s_sPosStart;
	float		fDistance = Mag(sDir);
	int			nNumSegmentLarge = m_line.m_nNumSegmentLarge + 2;
	int			nNumSegmentSmall = m_line.m_nNumSegmentSmall + 2;
	int			nNumArray = (nNumSegmentSmall - 1) * 4 * 2;
	int i, j;

	// Create
	pAtomic->m_pArrayColor	= new color4 [nNumArray];
	pAtomic->m_pArrayUV		= new EffectUV [nNumArray];
	pAtomic->m_pArrayVer	= new float3 [nNumArray];

	// Matrix
	matrix4x4		sMat = DirToMatrix(sDir);
	// Large Persent
	float*		fSegmentLargePersent = new float [nNumSegmentLarge];
	float3*	pSegmentLarge		 = new float3 [nNumSegmentLarge];
	float		fNowPersent;
	float		fTempPersent;

	for(i = 0; i < nNumSegmentLarge; ++i)
	{
		fSegmentLargePersent[i] = fmod((float)rand(), fDistance);
	}
	fSegmentLargePersent[0]						= 0.0f;
	fSegmentLargePersent[nNumSegmentLarge - 1]	= fDistance;

	for(i = 1; i < nNumSegmentLarge - 1; ++i)
	{
		fNowPersent = fSegmentLargePersent[i];
		for(j = i; j < nNumSegmentLarge - 1; ++j)
		{
			if( fNowPersent > fSegmentLargePersent[j] )
			{
				fTempPersent			= fNowPersent;
				fNowPersent				= fSegmentLargePersent[j];
				fSegmentLargePersent[j]	= fTempPersent;
			}
		}
		fSegmentLargePersent[i] = fNowPersent;
	}

	// Large Tension
	float3*	pDirLarge = new float3 [nNumSegmentLarge];
	float		fTensionLargeGap = m_line.m_fTensionLargeMax - m_line.m_fTensionLargeMin;

	for(i = 0; i < nNumSegmentLarge; ++i)
	{
		pDirLarge[i] = sDirX;
		RotateZ(pDirLarge[i], (float)rand());
		pDirLarge[i] *= fmod((float)rand(), fTensionLargeGap) + m_line.m_fTensionLargeMin;
	}
	pDirLarge[0]					= sPosZero;
	pDirLarge[nNumSegmentLarge - 1]	= sPosZero;

	// Large Sum
	for(i = 0; i < nNumSegmentLarge; ++i)
	{
		pSegmentLarge[i] = sDirZ;
		pSegmentLarge[i] *= fSegmentLargePersent[i];
		pSegmentLarge[i] += pDirLarge[i];
	}


	// Small Persent 
	float*		fSegmentSmallPersent	= new float [nNumSegmentSmall];
	float3*	pSegmentSmall			= new float3 [nNumSegmentSmall];

	for(i = 0; i < nNumSegmentSmall; ++i)
	{
		fSegmentSmallPersent[i] = fmod((float)rand(), fDistance);
	}
	fSegmentSmallPersent[0]						= 0.0f;
	fSegmentSmallPersent[nNumSegmentSmall - 1]	= fDistance;

	for(i = 1; i < nNumSegmentSmall - 1; ++i)
	{
		fNowPersent = fSegmentSmallPersent[i];
		for(j = i; j < nNumSegmentSmall - 1; ++j)
		{
			if( fNowPersent > fSegmentSmallPersent[j] )
			{
				fTempPersent			= fNowPersent;
				fNowPersent				= fSegmentSmallPersent[j];
				fSegmentSmallPersent[j]	= fTempPersent;
			}
		}
		fSegmentSmallPersent[i] = fNowPersent;
	}

	// Small Tension
	float3*	pDirSmall = new float3 [nNumSegmentSmall];
	float		fTensionSmallGap = m_line.m_fTensionSmallMax - m_line.m_fTensionSmallMin;

	for(i = 0; i < nNumSegmentSmall; ++i)
	{
		pDirSmall[i] = sDirX;
		RotateZ(pDirSmall[i], (float)rand());
		pDirSmall[i] *= fmod((float)rand(), fTensionSmallGap) + m_line.m_fTensionSmallMin;
	}
	pDirSmall[0]					= sPosZero;
	pDirSmall[nNumSegmentSmall - 1]	= sPosZero;

	// Sum
	float3		sTempDir;
	float		fTempPersent2;
	for(i = 0; i < nNumSegmentSmall; ++i)
	{
		for(j = 0; j < nNumSegmentLarge; ++j)
		{
			if( fSegmentSmallPersent[i] < fSegmentLargePersent[j] )
			{
				fTempPersent	= fSegmentLargePersent[j] - fSegmentLargePersent[j - 1];
				fTempPersent2	= fSegmentSmallPersent[i] - fSegmentLargePersent[j - 1];
				sTempDir		= pSegmentLarge[j] - pSegmentLarge[j - 1];
				sTempDir		*= fTempPersent2 / fTempPersent;
				sTempDir		+= pSegmentLarge[j - 1];
				break;
			}
		}
		if( j == nNumSegmentLarge )		sTempDir = pSegmentLarge[j - 1];

		pSegmentSmall[i] = sTempDir;
		pSegmentSmall[i] += pDirSmall[i];
	}

	// Width
	float		fWidthGap = m_line.m_fWidthMax - m_line.m_fWidthMin;
	float		fWidth;
	float3*	pVer;
	EffectUV*		pUV;
	for(i = 0; i < nNumSegmentSmall - 1; ++i)
	{
		pVer	= &pAtomic->m_pArrayVer[i * 8];
		pUV		= &pAtomic->m_pArrayUV[i * 8];
		fWidth	= fmod((float)rand(), fWidthGap) + m_line.m_fWidthMin;

		pVer[0] = pSegmentSmall[i + 0];
		pVer[1] = pSegmentSmall[i + 0];
		pVer[2] = pSegmentSmall[i + 1];
		pVer[3] = pSegmentSmall[i + 1];
		pVer[4] = pSegmentSmall[i + 0];
		pVer[5] = pSegmentSmall[i + 0];
		pVer[6] = pSegmentSmall[i + 1];
		pVer[7] = pSegmentSmall[i + 1];

		pVer[0].x += fWidth;
		pVer[1].x -= fWidth;
		pVer[2].x -= fWidth;
		pVer[3].x += fWidth;
		pVer[4].y += fWidth;
		pVer[5].y -= fWidth;
		pVer[6].y -= fWidth;
		pVer[7].y += fWidth;

		pUV[0].u = 0.0f;
		pUV[0].v = 0.0f;
		pUV[1].u = 1.0f;
		pUV[1].v = 0.0f;
		pUV[2].u = 1.0f;
		pUV[2].v = 1.0f;
		pUV[3].u = 0.0f;
		pUV[3].v = 1.0f;
		pUV[4].u = 0.0f;
		pUV[4].v = 0.0f;
		pUV[5].u = 1.0f;
		pUV[5].v = 0.0f;
		pUV[6].u = 1.0f;
		pUV[6].v = 1.0f;
		pUV[7].u = 0.0f;
		pUV[7].v = 1.0f;
	}

	color4	sInit;
	if( pPro->bAlphaColor )		sInit = color4(pPro->sAlphaColor.x, pPro->sAlphaColor.y, pPro->sAlphaColor.z, 1.0f);
	else						sInit = color4(1.0f, 1.0f, 1.0f, 1.0f);

	for(i = 0; i < nNumArray; ++i)
	{
		pAtomic->m_pArrayColor[i]	= sInit;
	}

	// Trans Matrix
	for(i = 0; i < nNumArray; ++i)
	{
		pAtomic->m_pArrayVer[i] = TransVector(sMat, pAtomic->m_pArrayVer[i]);
	}



	delete [] fSegmentLargePersent;
	delete [] fSegmentSmallPersent;
	delete [] pSegmentLarge;
	delete [] pSegmentSmall;
	delete [] pDirLarge;
	delete [] pDirSmall;

}

static SProperty*	s_pPro		= NULL;
static AotmicData*		s_pAtomic	= NULL;
// End

void AtomicMesh::Init(char* szFileName)
{CPUTime(AtomicMesh);
	m_meshPtr = GetEffectMgr()->LoadMesh(szFileName);
	EffectMeshFrame*	pFrame;
	float3*			pSrcVer;
	EffectUV**			pSrcUV;
	pFrame			= m_meshPtr->m_pFrame;
	m_nNumVer		= pFrame->m_nFace * 3;
	pSrcUV			= (EffectUV**)pFrame->m_pUV;
	pSrcVer			= (float3*)pFrame->m_pVert;
	m_pArrayUV		= pSrcUV[0];
	m_pArrayVer		= new float3[m_nNumVer];
	//
	if(m_textureFile.t[0])
	{
		TexturePtr tex;
		if(LoadTexture(tex, m_textureFile.t, 0, true))
		{
			m_pTex = tex.GetPtr();
			safe_addref(m_pTex);
		}
	}
}

void AtomicMesh::Release()
{CPUTime(AtomicMesh);
	if(m_pArrayVer)
		delete [] m_pArrayVer;
	m_pArrayVer = 0;
}

void AtomicMesh::Update(EffectNode* pNode)
{CPUTime(AtomicMesh);
	CDynamicArray<AotmicData*>& vAtomic = pNode->m_vNodeActive;
	SProperty*			pPro	= &pNode->m_pPro;
	EffectMeshFrame*	pFrame = m_meshPtr->m_pFrame;
	AotmicData*	pAtomic;
	float3*	pArrayVer;
	float3*	pPrevVer;
	float3*	pNextVer;
	float fPrevRate;
	float fNextRate;
	float fNowFrame;
	float fNumFrame = (float)m_meshPtr->m_nFrame;
	int nPrevFrame;
	int nNextFrame;
	int nNumVer;
	if( pPro->bAtomicInstAnimation )
	{
		for(int i=0;i<vAtomic.size();i++)
		{
			pAtomic = vAtomic[i];
			pAtomic->m_fNowFrame += m_meshPtr->m_fAnimSpeed * g_manager.GetElapsedTime();
		}
	}

	// Color
	int		nNumTex = m_meshPtr->m_textures.size();
	int		nNumFace = m_nNumVer / 3;
	int*	pTexIndex = (int*)m_meshPtr->m_pFrame->m_pTexID[0];
	float	fTotalTime;

	s_fAlpha	= 1.0f;
	//Texture* pTex		= m_meshPtr->m_textures[0];
	for(int a=0;a<vAtomic.size();a++)
	{
		pAtomic		= vAtomic[a];
		fTotalTime	= pAtomic->m_fLivedTime;

		//if( pPro->bAtomicInstBmxBlend )			PrepareTransAlpha(pTex, fTotalTime);
		//else
		s_fAlpha = pAtomic->m_fAlpha;

		for(int i = 0; i < m_nNumVer; ++i)
		{
			pAtomic->m_pArrayColor[i].a = s_fAlpha;
		}
	}

	// UV
	if( pPro->bAtomicInstAnimation )
	{
		for(int i=0;i<vAtomic.size();i++)
		{
			pAtomic		= vAtomic[i];
			fTotalTime	= pAtomic->m_fLivedTime;

			//PrepareTransUV(pTex, fTotalTime);

			//for(i = 0; i < m_nNumVer; ++i)
			//{
			//	TransUV(&pAtomic->m_pArrayUV[i], pTex);
			//}
		}
	}


	// Vertex
	for(int a=0;a<vAtomic.size();a++)
	{
		pAtomic		= vAtomic[a];
		pArrayVer	= pAtomic->m_pArrayVer;

		fNowFrame	= pAtomic->m_fNowFrame;
		fPrevRate	= fmod(fNowFrame, 1.0f);
		fNextRate	= 1.0f - fPrevRate;
		nPrevFrame	= (int)fNowFrame % m_meshPtr->m_nFrame;
		nNextFrame	= (nPrevFrame + 1) % m_meshPtr->m_nFrame;
		pPrevVer	= (float3*)pFrame[nPrevFrame].m_pVert;
		pNextVer	= (float3*)pFrame[nNextFrame].m_pVert;
		nNumVer		= pFrame->m_nVert;

		for(int i = 0; i < nNumVer; ++i)
		{
			pArrayVer[i].x = pPrevVer[i].x*fNextRate + pNextVer[i].x*fPrevRate;
			pArrayVer[i].y = pPrevVer[i].y*fNextRate + pNextVer[i].y*fPrevRate;
			pArrayVer[i].z = pPrevVer[i].z*fNextRate + pNextVer[i].z*fPrevRate;
		}
	}

	// Multiply Vertex
	matrix4x4 mat,matTemp;
	matrixRotationZ(&mat, PI);
	for(int a=0;a<vAtomic.size();a++)
	{
		pAtomic		= vAtomic[a];
		pArrayVer	= pAtomic->m_pArrayVer;

		matrixMultiply(&matTemp, &mat, &pAtomic->m_sMat);
		for(int i = 0; i < nNumVer; ++i)
		{
			pArrayVer[i] = TransVector(matTemp, pArrayVer[i]);
		}
	}
}

void AtomicMesh::Draw(CDynamicArray<AotmicData*>& vAtomic, EffectNode* node)
{CPUTime(AtomicMesh);
	AotmicData*	pAtomic;
	float3*	pArrayVer;
	int*		pTexIndex	= (int*)m_meshPtr->m_pFrame->m_pTexID[0];
	int*		pVerIndex	= m_meshPtr->m_pFrame->m_pVertIndex;
	int			nNumFace	= m_meshPtr->m_pFrame->m_nFace;
	int			i;

	SProperty* pPro = &node->m_pPro;
	if( pPro->bAtomicInstTwoSide )
		g_manager->SetCullMode(Phantom::CullMode_Double);
	if(!pPro->bAlphaZBuffer)
		g_manager->SetZWriteEnable(false);
	_SetBlend(pPro->eAlphaBlend);
	Program* vsps = g_manager.GetCurrProgram();
	if(m_pTex)
		vsps->SetTexture(Phantom::TextureIndex_0, m_pTex);
	else if(m_meshPtr->m_textures.size() > 0)
		vsps->SetTexture(Phantom::TextureIndex_0, m_meshPtr->m_textures[0]);
	for(int a=0;a<vAtomic.size();a++)
	{
		pAtomic		= vAtomic[a];
		pArrayVer	= pAtomic->m_pArrayVer;
		s_pAtomic	= pAtomic;
		for(i = 0; i < m_nNumVer; ++i)
		{
			m_pArrayVer[i] = pArrayVer[ pVerIndex[i] ];
		}
		// Draw One MeshPtr
		g_tempColors.SetArrayCount(m_nNumVer);
		for(int j=0;j<m_nNumVer;j++)
			g_tempColors[j] = pAtomic->m_pArrayColor[j];
		g_manager->DrawNoIndexed(vsps, DrawVInfos(m_nNumVer, m_pArrayVer, g_tempColors.GetPtr(), (float2*)pAtomic->m_pArrayUV), m_nNumVer, Phantom::DrawMode_Triangles);
		//GetRenderHelperPtr()->drawNoIndexed(Phantom::DRAW_TYPE_TRIANGLELIST, verts.GetPtr(), m_nNumVer, sizeof(PEffectVertex), PEffectVertex::c_fvf);
	}
	if(!pPro->bAlphaZBuffer)
		g_manager->SetZWriteEnable(true);
}

const char* AtomicMesh::GetFileName()
{CPUTime(AtomicMesh);
	if(m_meshPtr)
		return  m_meshPtr->m_strFileName.c_str();
	return "";
}

AotmicData* AtomicMesh::CreateAtomic(SProperty* pPro)
{CPUTime(AtomicMesh);
	int nNumVer = m_meshPtr->m_pFrame->m_nVert;
	int i;

	AotmicData* pAtomic		= new AotmicData;
	pAtomic->m_pArrayVer	= new float3 [nNumVer];
	pAtomic->m_pArrayUV		= new EffectUV [m_nNumVer];
	pAtomic->m_pArrayColor	= new color4 [m_nNumVer];


	color4	sInit(1.0f, 1.0f, 1.0f, 1.0f);

	if( !pPro->bAtomicInstBmxBlend && pPro->bAlphaColor )	sInit = color4(pPro->sAlphaColor.x, pPro->sAlphaColor.y, pPro->sAlphaColor.z, 1.0f);
	else													sInit = color4(1.0f, 1.0f, 1.0f, 1.0f);

	for(i = 0; i < m_nNumVer; ++i)
	{
		pAtomic->m_pArrayUV[i]		= m_pArrayUV[i];
		pAtomic->m_pArrayColor[i]	= sInit;
	}

	return pAtomic;
}
void AtomicMesh::DestroyAtomic(AotmicData *pAtomic)
{CPUTime(AtomicMesh);
	delete [] pAtomic->m_pArrayColor;
	delete [] pAtomic->m_pArrayVer;
	delete [] pAtomic->m_pArrayUV;

	pAtomic->m_pArrayColor	= NULL;
	pAtomic->m_pArrayVer	= NULL;
	pAtomic->m_pArrayUV		= NULL;
	
	delete pAtomic;


}
// End


extern	const char*	GetEffectTexture();

void AtomicPlane::Init(char* szFileName)
{CPUTime(AtomicPlane);
	if(m_textureFile.t[0])
		szFileName = m_textureFile.t;
	TexturePtr tex;
	if(LoadTexture(tex, szFileName, 0, true))
	{
		m_pTex = tex.GetPtr();
		safe_addref(m_pTex);
	}
}

void AtomicPlane::Release()
{CPUTime(AtomicPlane);
	//delete m_pTex;
	safe_release(m_pTex);
}

AotmicData* AtomicPlane::CreateAtomic(SProperty* pPro)
{CPUTime(AtomicPlane);
//	AotmicData* pAtomic = _AtomicPacket()->AtomicNew();
	AotmicData* pAtomic = new AotmicData;
	pAtomic->m_pArrayVer = new float3[4];
	memcpy(pAtomic->m_pArrayVer, s_fVer, 48);

	return pAtomic;

}

void DrawElement(Texture* tex, color4* colors,  float2* uvs, float3* verts)
{
	if(!verts)
		return;
	unsigned int tempColors[4];
	for(int i=0;i<4;i++)
		tempColors[i] = colors[i];
	Program* vsps = g_manager.GetCurrProgram();
	if(tex)
		vsps->SetTexture(Phantom::TextureIndex_0, tex);
	WORD s_indices[6]={0,1,2,2,3,0};
	g_manager->DrawIndexed(vsps, DrawVInfos(4, verts, tempColors, uvs), s_indices, 6, Phantom::DrawMode_Triangles);
	//GetRenderHelperPtr()->drawIndexed(Phantom::DRAW_TYPE_TRIANGLELIST, temp, s_indices, 4, 2, sizeof(PEffectVertex), PEffectVertex::c_fvf);
}

static float s_fUV[8]	= 
{
	1.0f, 0.0f,					// V2
	1.0f, 1.0f,					// V3
	0.0f, 1.0f,					// V0
	0.0f, 0.0f					// V1
};
void AtomicPlane::Draw(CDynamicArray<AotmicData*>& vAtomic, EffectNode* node)
{CPUTime(AtomicPlane);
	AotmicData*	pAtomic;
	SProperty* pPro = &node->m_pPro;
	if( pPro->bAtomicInstTwoSide )
		g_manager->SetCullMode(Phantom::CullMode_Double);
	if(!pPro->bAlphaZBuffer)
		g_manager->SetZWriteEnable(false);
	if( pPro->bAtomicInstBmxBlend )
		_SetBlend(node->m_nBlendType);
	else
		_SetBlend(pPro->eAlphaBlend);

	//gTex::Reset();
	for(int i=0;i<vAtomic.size();i++)//it = it_start; it != it_end; ++it)
	{
		pAtomic	= vAtomic[i];//(*it);

//		m_pTex->ApplyFrame(pAtomic->m_nNowTexture);


		_SetColor(pPro->sAlphaColor, pAtomic->m_fAlpha, pPro->bAlphaColor);
		//if(node->m_pPro.bDirBillBoard || node->m_pPro.bDirChildBillBoard)
			//DrawElement(m_pTex, (color4*)s_pColor, (float2*)s_fUVB, pAtomic->m_pArrayVer);//, 4);
		//else
			DrawElement(m_pTex, (color4*)s_pColor, (float2*)s_fUV, pAtomic->m_pArrayVer);//, 4);
		//glColorPointer(4, GL_FLOAT, 0, s_pColor);
		//glTexCoordPointer(2, GL_FLOAT, 0, s_fUV);
		//glVertexPointer(3, GL_FLOAT, 0, pAtomic->m_pArrayVer);
		//glDrawArrays(GL_QUADS, 0, 4);
	}

	if(!pPro->bAlphaZBuffer)
		g_manager->SetZWriteEnable(true);
}

const char* AtomicPlane::GetFileName()
{CPUTime(AtomicPlane);
	return m_pTex->GetFileName();//m_strFileName.c_str();

}

void AtomicPlane::Update(EffectNode* pNode)
{CPUTime(AtomicPlane);
	CDynamicArray<AotmicData*>&		vAtomic = pNode->m_vNodeActive;
	SProperty*			pPro	= &pNode->m_pPro;
	AotmicData*			pAtomic;

	if( pPro->bAtomicInstAnimation )
	{
		for(int i=0;i<vAtomic.size();i++)//it = it_start; it != it_end; ++it)
		{
			pAtomic	= vAtomic[i];//(*it);

			if( pAtomic->m_fPersentage < 1.0f )
			{
				//pAtomic->m_nNowTexture = pAtomic->m_fPersentage * m_pTex->m_nFrame;
			}
			else
			{
				//pAtomic->m_nNowTexture = fmod(pAtomic->m_fPersentage, 1.0f) * m_pTex->m_nFrame;
			}
		}
	}

	static float3		s_sVer0 = float3(-10.0f, 0.0f, -10.0f );
	static float3		s_sVer1 = float3(-10.0f, 0.0f, +10.0f );
	static float3		s_sVer2 = float3(+10.0f, 0.0f, +10.0f );
	static float3		s_sVer3 = float3(+10.0f, 0.0f, -10.0f );
	//
	//static float3		b_sVer0 = float3( 0.0f, -10.0f, -10.0f);
	//static float3		b_sVer1 = float3( 0.0f, -10.0f, +10.0f);
	//static float3		b_sVer2 = float3( 0.0f, +10.0f, +10.0f);
	//static float3		b_sVer3 = float3( 0.0f, +10.0f, -10.0f);
	static float3		b_sVer0 = float3(  -10.0f, -10.0f , 0);
	static float3		b_sVer1 = float3(  -10.0f, +10.0f , 0);
	static float3		b_sVer2 = float3(  +10.0f, +10.0f , 0);
	static float3		b_sVer3 = float3(  +10.0f, -10.0f , 0);

	if(pNode->m_pPro.bDirBillBoard || pNode->m_pPro.bDirChildBillBoard)
	{
		for(int i=0;i<vAtomic.size();i++)//it = it_start; it != it_end; ++it)
		{
			pAtomic	= vAtomic[i];//(*it);
			pAtomic->m_pArrayVer[0] = TransVector(pAtomic->m_sMat, b_sVer0);
			pAtomic->m_pArrayVer[1] = TransVector(pAtomic->m_sMat, b_sVer1);
			pAtomic->m_pArrayVer[2] = TransVector(pAtomic->m_sMat, b_sVer2);
			pAtomic->m_pArrayVer[3] = TransVector(pAtomic->m_sMat, b_sVer3);
		}
	}
	else
	{
		// Multiply Vertex
		for(int i=0;i<vAtomic.size();i++)//it = it_start; it != it_end; ++it)
		{
			pAtomic	= vAtomic[i];//(*it);

			pAtomic->m_pArrayVer[0] = TransVector(pAtomic->m_sMat,s_sVer0);
			pAtomic->m_pArrayVer[1] = TransVector(pAtomic->m_sMat,s_sVer1);
			pAtomic->m_pArrayVer[2] = TransVector(pAtomic->m_sMat,s_sVer2);
			pAtomic->m_pArrayVer[3] = TransVector(pAtomic->m_sMat,s_sVer3);
		}
	}
}

void AtomicPlane::DestroyAtomic(AotmicData *pAtomic)
{CPUTime(AtomicPlane);
	delete [] pAtomic->m_pArrayVer;
	delete pAtomic;
}

SProperty::SProperty()
{

}

void SProperty::Default()
{CPUTime(SProperty);
	float3 sVecZero		= float3(0.0f, 0.0f, 0.0f);
	float3 sVecOne		= float3(1.0f, 1.0f, 1.0f);

	// Name
	memset(szNodeName, '\0', 32);

	// Ect
	memset(szAtomicName, '\0', 32);
	memset(szNextEffectName, '\0', 32);

	memset(szSoundName, '\0', 32);
	bSoundRepeat					= FALSE;

	bAttachMode						= FALSE;
	bAttachEndMode					= FALSE;


	// Create
	fDelayTime						= 0.0f;
	fCreateTime						= 0.02f;
	fLifeTime						= 0.5f;
	nNumNode						= 1;
	eCreate							= ETIME_CREATE;

	// StartMode
	eStart							= EPOINT_EF;
	bScalarRegular					= FALSE;
	fScalarMax						= 1.0f;
	fScalarMin						= 0.0f;
	bAngleRegular					= FALSE;
	fAngleMax						= 360.0f;
	fAngleMin						= 0.0f;
	bInheritMatrix					= TRUE;

	// MoveMode
	bMoveMode						= FALSE;
	eMove							= EEXPANSION;	
	fMoveSpeedMax					= 1.0f;
	fMoveSpeedMin					= 0.0f;
	bExpansionMatrix				= FALSE;

	// Pos
	bStartPos						= FALSE;
	sStartPos						= sVecZero;
	bEndPos							= FALSE;
	sEndPos							= sVecZero;
	bBoundCheck						= FALSE;
	bInheritEndPos					= FALSE;
	bPosRevers						= FALSE;
			
	// Straight
	bStraight						= FALSE;
	fStraightMoveSpeed				= 0.0f;
	fStraightAccelSpeed				= 0.0f;
	sStraightDir					= sVecZero;
			
	// Rotate			
	bRotate							= FALSE;
	fRotateRadius					= 0.0f;
	fRotateAngleSpeed				= 0.0f;
	sRotateDir						= sVecZero;

	// Gravity			
	bGravity						= FALSE;
	fGravityStart					= 0.0f;
	fGravitySpeed					= 0.0f;
	sGravityDir						= sVecZero;

	// Following
	bFollowing						= FALSE;
	fFollowingRangeStart			= 0.0f;
	fFollowingRangeAccel			= 0.0f;

	// Dir
	bDirStart						= FALSE;
	sDirStart						= sVecZero;
	bDirRotate						= FALSE;
	fDirRotateSpeedX				= 0.0f;
	fDirRotateSpeedY				= 0.0f;
	fDirRotateSpeedZ				= 0.0f;
	bDirBillBoard					= FALSE;
	bDirEqualNodeDir				= FALSE;

	// DirChild
	bDirChildStart					= FALSE;
	sDirChildStart					= sVecZero;
	bDirChildRotate					= FALSE;
	fDirChildRotateSpeedX			= 0.0f;
	fDirChildRotateSpeedY			= 0.0f;
	fDirChildRotateSpeedZ			= 0.0f;
	bDirChildRandomRotateX			= FALSE;
	bDirChildRandomRotateY			= FALSE;
	bDirChildRandomRotateZ			= FALSE;
	bDirChildBillBoard				= TRUE;
	bDirChildEqualNodeDir			= FALSE;

	// Scale
	bScale							= FALSE;
	bScaleBaseRegular				= FALSE;
	fScaleBaseRegularStart			= 1.0f;
	fScaleBaseRegularSpeed			= 0.0f;
	bScaleBaseSin					= FALSE;
	fScaleBaseSinStart				= 1.0f;
	fScaleBaseSinEnd				= 0.0f;
	fScaleBaseSinPeriodicTime		= 0.0f;

	bScaleAdditionX					= FALSE;
	bScaleAdditionY					= FALSE;
	bScaleAdditionZ					= FALSE;
	bScaleAdditionRegular			= FALSE;
	fScaleAdditionRegularStart		= 1.0f;
	fScaleAdditionRegularSpeed		= 0.0f;
	bScaleAdditionSin				= FALSE;
	fScaleAdditionSinStart			= 1.0f;
	fScaleAdditionSinEnd			= 0.0f;
	fScaleAdditionSinPeriodicTime	= 0.0f;

	// Alpha
	eAlphaBlend						= EBLEND_ALPHA_ADD;
	bAlphaZBuffer					= TRUE;
	bAlphaRegular					= FALSE;
	fAlphaRegularStart				= 1.0f;
	fAlphaRegularEnd				= 0.0f;
	bAlphaSin						= FALSE;
	fAlphaSinStart					= 1.0f;
	fAlphaSinEnd					= 0.0f;
	fAlphaSinPeriodicTime			= 0.0f;
	bAlphaColor						= FALSE;
	sAlphaColor						= sVecOne;

	// AtomicInst
	bAtomicInstAnimation			= FALSE;
	bAtomicInstBmxBlend				= FALSE;
	bAtomicInstTwoSide				= FALSE;
	bAtomicInstAniTime				= FALSE;
	fAtomicInstAniTime				= 0.0f;

	// DynamicLight
	bDynamicLight					= FALSE;
	fDynamicLightRange				= 0.0f;
	sDynamicLightColor				= sVecOne;

	// Vibration
	bVibration						= FALSE;
	fVibrationPower					= 0.0f;
	fVibrationRange					= 0.0f;
	fVibrationTime					= 0.0f;

}

void AtomicBase::_SetBlend(int nBlendType)
{CPUTime(AtomicBase);
	switch( nBlendType )
	{
	case 0:
		g_manager->SetAlphaMode(Phantom::AlphaMode_None);
		break;
	case 1: // ALPHA KEY BLEND
		g_manager->SetAlphaMode(Phantom::AlphaMode_Test);
		g_manager->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_INVSRCALPHA);
		break;
	case 2: // ALPHA BLEND
		g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		g_manager->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_INVSRCALPHA);
		break;
	case 3: // ALPHA BLEND ADD
		g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		g_manager->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_ONE);
		break;
	case 4: // MODULATE
		g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		g_manager->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_ONE);
		break;
	case 5: // FORCE BLEND ADD
		g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		g_manager->SetBlendMode(Phantom::BlendMode_ONE, Phantom::BlendMode_ONE);
		break;
	case 6: // FORCE BLEND SUBTRACT
		g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		g_manager->SetBlendMode(Phantom::BlendMode_ZERO, Phantom::BlendMode_SRCCOLOR);
		break;
	}
}



void GetExtension(char *&szExtension, char *szFileName)
{
	int		nPoint = '.';
	char*	szOffSet;

	szOffSet = (char*)strchr((LPCSTR)szFileName, nPoint);
	++szOffSet;

	szExtension = szOffSet;

}
EffectMeshFrame::EffectMeshFrame()	{
	m_pVert = NULL; 
	m_pVertIndex = NULL; 
	m_pFaceNorm = NULL;	
	m_pVertNorm = NULL;
	for (int i = 0; i < MAX_MULTITEX; i++)
	{
		m_pTexID[i] = NULL;
		m_pUV[i] = NULL;
	}
}

void EffectMeshFrame::Release()
{CPUTime(EffectMeshFrame);
	if (m_pVert) 
	{
		delete [] m_pVert;
		m_pVert = NULL;
	}
	if (m_pVertIndex) 
	{
		delete [] m_pVertIndex;
		m_pVertIndex = NULL;
	}
	if (m_pFaceNorm) 
	{
		delete [] m_pFaceNorm;
		m_pFaceNorm = NULL;
	}

	if (m_pVertNorm) 
	{
		delete [] m_pVertNorm;
		m_pVertNorm = NULL;
	}

	for (int i = 0; i < MAX_MULTITEX; i++)
	{
		if (m_pUV[i]) 
		{
			delete [] m_pUV[i];
			m_pUV[i] = NULL;
		}
		if (m_pTexID[i]) 
		{
			delete [] m_pTexID[i];
			m_pTexID[i] = NULL;
		}
	}
}

EffectMeshFrame::~EffectMeshFrame() 
{
	// by ks
	Release();
}


EffectMesh::EffectMesh()
{
	m_pFrame = NULL;
	m_nFrame = 0;

	memset(m_fRangeMin, 0, sizeof m_fRangeMin);
	memset(m_fRangeMax, 0, sizeof m_fRangeMax);
	memset(m_fRangeMid, 0, sizeof m_fRangeMid);


	snd_idx.m_n3didx = -1;
	snd_idx.m_nUserMark = -1;

}

EffectMesh::~EffectMesh()
{
	for(int i=0;i<m_textures.size();i++)
		safe_release(m_textures[i]);
	m_textures.clear();
	if( m_pFrame )
	{
		if(m_nFrame > 1)
			delete[] m_pFrame;
		else
			delete m_pFrame;
		m_pFrame = NULL;
	}
	m_nFrame = 0;
}

void	EffectMesh::SaveMorph(CFileHelperW&ar)
{CPUTime(EffectMesh);
	ar << this->m_nFrame;
	EffectMeshFrame* f = this->m_pFrame;
	ar << f->m_nFace;
	ar.pStream->write(f->m_pTexID[0], sizeof(int)*f->m_nFace);
	ar.pStream->write(f->m_pUV[0], sizeof(float)*f->m_nFace*3*2);
	for(int frame=0;frame<this->m_nFrame;frame++)
	{
		EffectMeshFrame* f = &this->m_pFrame[frame];
		ar << f->m_nFace;
		ar << f->m_nMultiTex;
		ar << f->m_nVert;
		ar.pStream->write(f->m_pVert, sizeof(float)*f->m_nVert*3);
		ar.pStream->write(f->m_pVertIndex, sizeof(int)*f->m_nFace*3);
		ar.pStream->write(f->m_pFaceNorm, sizeof(float)*f->m_nFace*3);
		ar.pStream->write(f->m_pVertNorm, sizeof(float)*f->m_nFace*3*3);
	}
}
void	EffectMesh::SaveMesh(CFileHelperW&ar)
{CPUTime(EffectMesh);
	ar << this->m_nFrame;
	for(int frame=0;frame<this->m_nFrame;frame++)
	{
		EffectMeshFrame* f = &this->m_pFrame[frame];
		ar << f->m_nVert;
		ar.pStream->write(f->m_pVert, sizeof(float)*f->m_nVert*3);
		ar << f->m_nFace;
		ar.pStream->write(f->m_pVertIndex, sizeof(int)*f->m_nFace*3);
		ar.pStream->write(f->m_pFaceNorm, sizeof(float)*f->m_nFace*3);
		ar.pStream->write(f->m_pVertNorm, sizeof(float)*f->m_nFace*3*3);
		ar << f->m_nMultiTex;
		for (int i = 0; i < f->m_nMultiTex; i++)
		{
			ar.pStream->write(f->m_pTexID[i], sizeof(int)*f->m_nFace);
			ar.pStream->write(f->m_pUV[i], sizeof(float)*f->m_nFace*3*2);
		}
	}
}
VOID	EffectMesh::Save(const char* szFile)
{CPUTime(EffectMesh);
	CFileHelperW ar;
	ar.create(szFile);
	int nTex = m_textureFiles.size();
	for(int i=0;i<nTex;i++)
	{
		ar << m_textureFiles[i].c_str();
	}
	ar.pStream->write(this->m_fRangeMin, sizeof(this->m_fRangeMin));
	ar.pStream->write(this->m_fRangeMax, sizeof(this->m_fRangeMax));
	ar.pStream->write(this->m_fRangeMid, sizeof(this->m_fRangeMid));
	ar << this->m_fRadius;
	ar << this->m_nType;
	ar << this->m_bUseTile;
	ar << this->m_fAnimSpeed;
	ar << this->m_bAutoHide;
	ar << this->m_fHideRange;
	ar << this->m_bCollideTest;
	ar << this->m_bZBuffer;
	ar << this->m_nBlendMode;
	ar << this->m_bVertLight;
	ar << this->m_bGroundStruct;
	ar << this->m_strSound;
	if (this->m_nType == 0)
		SaveMesh(ar);
	else
		SaveMorph(ar);
}

void	EffectMesh::LoadMorph(CFileHelperR& ar)
{CPUTime(EffectMesh);
	ar >> this->m_nFrame;
	m_pFrame = new EffectMeshFrame[this->m_nFrame];
	EffectMeshFrame* f = this->m_pFrame;
	ar >> f->m_nFace;
	f->m_pTexID[0] = new int [f->m_nFace];
	f->m_pUV[0] = new float [f->m_nFace*3*2];
	ar.pStream->read(f->m_pTexID[0], sizeof(int)*f->m_nFace);
	ar.pStream->read(f->m_pUV[0], sizeof(float)*f->m_nFace*3*2);
	for(int frame=0;frame<this->m_nFrame;frame++)
	{
		EffectMeshFrame* f = &this->m_pFrame[frame];
		ar >> f->m_nFace;
		ar >> f->m_nMultiTex;
		ar >> f->m_nVert;
		f->m_pVert = new float [f->m_nVert*3];
		f->m_pVertIndex = new int [f->m_nFace*3];
		f->m_pFaceNorm = new float [f->m_nFace*3];
		f->m_pVertNorm = new float [f->m_nFace*3*3];
		//
		ar.pStream->read(f->m_pVert, sizeof(float)*f->m_nVert*3);
		ar.pStream->read(f->m_pVertIndex, sizeof(int)*f->m_nFace*3);
		ar.pStream->read(f->m_pFaceNorm, sizeof(float)*f->m_nFace*3);
		ar.pStream->read(f->m_pVertNorm, sizeof(float)*f->m_nFace*3*3);
	}
}
void	EffectMesh::LoadMesh(CFileHelperR&ar)
{CPUTime(EffectMesh);
	//ar >> this->m_nFrame;
	int nVersion = 0;
	ar >> nVersion;
	this->m_nFrame = 1;
	m_pFrame = new EffectMeshFrame;// [m_nFrame];
	for(int frame=0;frame<this->m_nFrame;frame++)
	{
		EffectMeshFrame* f = &this->m_pFrame[frame];
		ar >> f->m_nVert;

		f->m_pVert = new float [f->m_nVert*3];

		ar.pStream->read(f->m_pVert, sizeof(float)*f->m_nVert*3);
		ar >> f->m_nFace;
		//
		f->m_pVertIndex = new int [f->m_nFace*3];
		f->m_pFaceNorm = new float [f->m_nFace*3];
		f->m_pVertNorm = new float [f->m_nFace*3*3];
		//
		ar.pStream->read(f->m_pVertIndex, sizeof(int)*f->m_nFace*3);
		ar.pStream->read(f->m_pFaceNorm, sizeof(float)*f->m_nFace*3);
		ar.pStream->read(f->m_pVertNorm, sizeof(float)*f->m_nFace*3*3);
		ar >> f->m_nMultiTex;
		for (int i = 0; i < f->m_nMultiTex; i++)
		{
			f->m_pTexID[i] = new int [f->m_nFace];
			f->m_pUV[i] = new float [f->m_nFace*3*2];
			//
			ar.pStream->read(f->m_pTexID[i], sizeof(int)*f->m_nFace);
			ar.pStream->read(f->m_pUV[i], sizeof(float)*f->m_nFace*3*2);
		}
	}
}
BOOL	EffectMesh::Load(const char* szFile, const char* szTexturePath)
{CPUTime(EffectMesh);
	CFileHelperR ar;
	if(!ar.open(szFile))return false;
	int nVersion = 1;
	ar >> nVersion;
	char head[4];
	ar.pStream->read(head, sizeof(head));
	int nTex;
	ar >> nTex;
	if(nTex > 0)
	{
		m_textureFiles.resize(nTex);
	}
	for(int i=0;i<nTex;i++)
	{
		ar >> m_textureFiles[i];
		//
		char szTemp[256];
		strcpy(szTemp, szTexturePath);
		strcat(szTemp, m_textureFiles[i].c_str());
		TexturePtr tex;
		if(LoadTexture(tex, szTemp, szTexturePath, true))
		{
			Texture* tex2 = tex.GetPtr();
			safe_addref(tex2);
			if(tex2)
				m_textures.push_back(tex2);
		}
	}
	ar.pStream->read(this->m_fRangeMin, sizeof(this->m_fRangeMin));
	ar.pStream->read(this->m_fRangeMax, sizeof(this->m_fRangeMax));
	ar.pStream->read(this->m_fRangeMid, sizeof(this->m_fRangeMid));
	ar >> this->m_fRadius;
	ar >> this->m_nType;
	ar >> this->m_bUseTile;
	ar >> this->m_fAnimSpeed;
	ar >> this->m_bAutoHide;
	ar >> this->m_fHideRange;
	ar >> this->m_bCollideTest;
	ar >> this->m_bZBuffer;
	ar >> this->m_nBlendMode;
	ar >> this->m_bVertLight;
	ar >> this->m_bGroundStruct;
	ar >> this->m_strSound;
	if (this->m_nType == 0)
		LoadMesh(ar);
	else
		LoadMorph(ar);
	return true;
}


float3	GetMinePosition(){
	return float3(0, 0, 0);
}

EffectNode::EffectNode()
{
	m_pNodeParent = NULL;
	m_pAtomicInst = NULL; 
	m_pSkill = NULL;
	m_pPro.Default();
	m_eff_idx.m_n3didx = -1;
	m_eff_idx.m_nUserMark = -1;
	m_nBlendType = m_nScaleFunc = m_nAlphaFunc = 0;
	m_dwProp = 0;
	m_nID = 0;
	m_fMoveX = m_fMoveY = m_fRotAng = 0;
	m_fFrameSpeed = 0;
	memset(m_fScaleArg, 0, sizeof(m_fScaleArg));
	memset(m_fAlphaArg, 0, sizeof(m_fAlphaArg));
}

EffectNode::~EffectNode()
{
	Release();
}

void EffectNode::Update()
{CPUTime(EffectNode);
	if( !m_bActive )
	{
		return;
	}
	if( m_fDelayTime > 0.0f )
	{
		m_fDelayTime -= g_manager.GetElapsedTime();
		return;
	}

	Update_Prev();

	if(m_vNodeInst.size() != 0)// !m_vNodeInst.empty() )
	{
		UpdateKill();
		UpdateCreate();
		UpdateTrans();
		EffectNode*	pNode;

		// Update
		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pNode = (EffectNode*)m_vNodeActive[i];//(*it);
			pNode->Update();
		}

		// Kill
		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pNode = (EffectNode*)m_vNodeActive[i];//(*it);
			if( !pNode->m_bLife && pNode->m_fDelayTime <= 0.0f && pNode->m_vNodeActive.size() == 0)//.empty() )
			{
				pNode->ReleaseActive();
				delete pNode;
				m_vNodeActive.eraseByIndex(i);
				i--;
				//m_vNodeActive.erase(it--);
			}
		}
	}
	else
	{
		UpdateKill_At();
		UpdateCreate_At();
		UpdateTrans();
	}
}

void EffectNode::Draw()
{CPUTime(EffectNode);
	if( !m_bActive )				return;
	if( m_fDelayTime > 0.0f )		return;

	if( m_vNodeInst.size() != 0 )
	{
		EffectNode*	pNode;
		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pNode = (EffectNode*)m_vNodeActive[i];//(*it);
			pNode->Draw();
		}
	}
	else if(m_pAtomicInst)
	{
		m_pAtomicInst->Draw(m_vNodeActive, this);
	}
}

void EffectNode::Save(CFileHelperW& w)
{CPUTime(EffectNode);
}

void EffectNode::Load(CFileHelperR& r)
{CPUTime(EffectNode);
	m_nID = GetEffectMgr()->CreateNewID();
	EffectNode*		pNode = NULL;
	int			nNumNode;
	int version = 1;
	r >> version;
	r >> m_nBlendType >> m_nScaleFunc >> m_nAlphaFunc;
	r >> m_dwProp;
	r >> m_fMoveX >> m_fMoveY >> m_fRotAng;
	r.pStream->read(m_fScaleArg, sizeof(m_fScaleArg));
	r.pStream->read(m_fAlphaArg, sizeof(m_fAlphaArg));
	r >> m_fFrameSpeed;
	r >> m_pPro;
	r >> nNumNode;
	if(version > 2)
		r >> m_textureFile;
#ifndef _TOOLMODE
	//m_eff_idx.m_nUserMark = CTSndScript::GetInterface()->SSXStringToIndex( m_pPro.szSoundName );
#endif
	if( nNumNode )
	{
		for(int i = 0; i < nNumNode; ++i)
		{
			pNode = new EffectNode;
			pNode->Load(r);
			pNode->m_pNodeParent = this;
			m_vNodeInst.push_back(pNode);
		}
	}
	else
	{
		int nAtomicInstIndex;
		nAtomicInstIndex	= GetEffectMgr()->AddAtomicInst(m_pPro.szAtomicName, m_textureFile);
		m_pAtomicInst		= GetEffectMgr()->GetAtomicInst(nAtomicInstIndex);
	}
}

void EffectNode::InitAtomicInst(AtomicBase *pInst)
{CPUTime(EffectNode);
	if( pInst == NULL )
	{
		m_pAtomicInst = NULL;
		m_pPro.szAtomicName[0] = '\0';
	}
	else
	{
		m_pAtomicInst = pInst;
		strncpy(m_pPro.szAtomicName, pInst->GetFileName(), 32);
	}

}

void EffectNode::InitProperty()
{CPUTime(EffectNode);
}

void EffectNode::ReleaseProperty()
{CPUTime(EffectNode);
}

void EffectNode::Release()
{CPUTime(EffectNode);
	ReleaseActive();
	ReleaseInst();

}

void EffectNode::ReleaseInst()
{CPUTime(EffectNode);
	EffectNode*	pNode;

	for(int i=0;i<m_vNodeInst.size();i++)//it = it_start; it != it_end; ++it)
	{
		pNode = (EffectNode*)m_vNodeInst[i];//(*it);
		pNode->ReleaseInst();
		delete pNode;
	}
	m_vNodeInst.clear();
	ReleaseProperty();

}

void EffectNode::ReleaseActive()
{CPUTime(EffectNode);
	// Sound
	StopSound();
	if( m_vNodeInst.size() != 0)//.empty() )
	{
		EffectNode*	pNode;
		for(int i=0;i<m_vNodeInst.size();i++)//it = it_start; it != it_end; ++it)
		{
			pNode = (EffectNode*)m_vNodeInst[i];//(*it);
			pNode->ReleaseActive();
			delete pNode;
		}
		m_vNodeInst.clear();
		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pNode = (EffectNode*)m_vNodeActive[i];//(*it);
			pNode->ReleaseActive();
			delete pNode;
		}
		m_vNodeActive.clear();

		m_pAtomicInst	= NULL;
//		safe_delete(m_pSkill);
	}
	else
	{
		AotmicData*	pAtomic;
		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pAtomic = m_vNodeActive[i];//(*it);
			m_pAtomicInst->DestroyAtomic(pAtomic);
		}
		m_vNodeActive.clear();
		m_pAtomicInst	= NULL;
	}
}

EffectNode* EffectNode::GetNodeInst(DWORD dwNodeIndex)
{CPUTime(EffectNode);
	EffectNode*	pNode;
	if( m_dwNodeIndex == dwNodeIndex )
	{
		return this;
	}

	for(int i=0;i<m_vNodeInst.size();i++)//it = it_start; it != it_end; ++it)
	{
		pNode = ((EffectNode*)m_vNodeInst[i])->GetNodeInst(dwNodeIndex);

		if( pNode )
		{
			return pNode;
		}
	}

	return NULL;

}

void EffectNode::UpdateKill()
{CPUTime(EffectNode);
	EffectNode*		pNode;
	float		fPlusTime		= g_manager.GetElapsedTime();
	float		fPlusPersent	= g_manager.GetElapsedTime() / m_pPro.fLifeTime;
	for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
	{
		pNode					= (EffectNode*)m_vNodeActive[i];//(*it);
		pNode->m_fLivedTime		+= fPlusTime;
		pNode->m_fPersentage	+= fPlusPersent;
	}
	// Kill
	if( !m_bNoKill )	// Time, OneCreate
	{
		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pNode = (EffectNode*)m_vNodeActive[i];//(*it);
			if( pNode->m_bLife && pNode->m_fPersentage > 1.0f )
			{
				pNode->m_bLife		= false;

				// NextEffect
				if( pNode->m_pPro.szNextEffectName[0] )
				{
//					GetEffectMgr()->AddEffect(pNode->m_nNextEffect, 1, pNode->m_sMat);
					float3		sPosStart	= TransVector(pNode->m_sMat,float3(0.0f, 0.0f, 0.0f));
					float3		sPosEnd		= TransVector(pNode->m_sMat,float3(0.0f, 0.0f, 1.0f));

					GetEffectMgr()->PlayEffect(pNode->m_pPro.szNextEffectName, sPosStart, sPosEnd);
				}

				// 距埃篮 窍靛 内爹牢啊?...  铝..
				if( m_pSkill )
				{
					//m_pSkill->End();
					m_pSkill = NULL;
				}
				else if( m_pNodeParent && m_pNodeParent->m_pSkill )
				{
					//m_pNodeParent->m_pSkill->End();
					m_pNodeParent->m_pSkill = NULL;
				}
			}
		}
	}
	else if( !m_bLife )	// Continue
	{
		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pNode = (EffectNode*)m_vNodeActive[i];//(*it);

			if( pNode->m_bLife )
			{
				pNode->m_bLife = false;

				// NextEffect
				if( pNode->m_pPro.szNextEffectName[0] )
				{
					float3		sPosStart	= TransVector(pNode->m_sMat,float3(0.0f, 0.0f, 0.0f));
					float3		sPosEnd		= TransVector(pNode->m_sMat,float3(0.0f, 0.0f, 1.0f));

					GetEffectMgr()->PlayEffect(pNode->m_pPro.szNextEffectName, sPosStart, sPosEnd);
				}
				if( pNode->m_pSkill )
				{
					//pNode->m_pSkill->End();
					pNode->m_pSkill = NULL;
				}
			}
		}
	}

}

void EffectNode::UpdateCreate()
{CPUTime(EffectNode);
	if( m_bNoCreate )	return;
	if( m_pPro.eCreate == EONECREATE || m_pPro.eCreate == ECONTINUE )
	{
		m_bNoCreate = true;
	}

	float	fStartGap	= m_pPro.fCreateTime - m_fCreateTime;

	m_fCreateTime	+= g_manager.GetElapsedTime();
	if( m_bLife && m_fCreateTime > m_pPro.fCreateTime )
	{
		EffectNode*		pNodeNew;
		EffectNode*		pNodeSrc;
		matrix4x4		sMatExpansion;
		float3		sExpansionPos;
		float3		sExpansionDir;
		float		fExpansionSpeed;
		float		fRadianX;
		float		fRadianY;
		int			nNumNode	= m_pPro.nNumNode;

		float	fStartLimit	= g_manager.GetElapsedTime() - m_pPro.fLifeTime;
		float f;
		for(f = fStartGap; f < g_manager.GetElapsedTime(); f += m_pPro.fCreateTime)
		{
			if( f > fStartLimit )
			{
				for(int i = 0; i < nNumNode; ++i)
				{
					sExpansionPos	= _CreatePosStart(i, fRadianX, fRadianY);
					sExpansionDir	= _CreatePosMove(sExpansionPos, fRadianX, fRadianY);
					fExpansionSpeed	= _CreateMoveSpeed();
					if( m_pPro.bExpansionMatrix )
					{
						sMatExpansion	= _CreatePosMatrix(fRadianX, fRadianY);
					}

					for(int i=0;i<m_vNodeInst.size();i++)//it = it_start; it != it_end; ++it)
					{
						pNodeNew	= new EffectNode();
						pNodeSrc	= (EffectNode*)m_vNodeInst[i];//(*it);

						pNodeNew->m_pNodeParent			= this;
						pNodeNew->m_fLivedTime			= f;
						pNodeNew->m_fPersentage			= f / m_pPro.fLifeTime;
						pNodeNew->m_sExpansionPos		= sExpansionPos;
						pNodeNew->m_sExpansionDir		= sExpansionDir;
						pNodeNew->m_fExpansionSpeed		= fExpansionSpeed;
						if( m_pPro.bExpansionMatrix )
						{
							pNodeNew->m_sMatExpansion	= sMatExpansion;
						}

						pNodeNew->_InitActive(pNodeSrc, m_sMat);
						pNodeNew->UpdateVibration();	// 睬荐促... ば ば
						m_vNodeActive.push_back(pNodeNew);
					}
				}
				if( m_bNoCreate )	break;
			}
		}
		m_fCreateTime = m_pPro.fCreateTime - (f - g_manager.GetElapsedTime());
	}

	if( m_pPro.eCreate == EONECREATE )
	{
		m_bLife = false;
	}

}

void EffectNode::UpdateTrans()
{CPUTime(EffectNode);
	AotmicData*	pAtomic;

	// Sum Pos
	{
		// Pos Expansion Start
		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pAtomic = m_vNodeActive[i];//(*it);
			pAtomic->m_sPos = pAtomic->m_sExpansionPos;		// 咯扁 吝夸.. 弊成 措涝茄促..
		}

		// Pos Expansion Move
		if( m_pPro.bMoveMode )
		{
			for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
			{
				pAtomic = m_vNodeActive[i];//(*it);
				pAtomic->m_sPos += pAtomic->m_sExpansionDir * (pAtomic->m_fExpansionSpeed * pAtomic->m_fLivedTime);
			}
		}

		// Pos Gravity
		if( m_pPro.bGravity )
		{
			float3		sGravityDir;
			sGravityDir = float3(0.0f, 0.0f, -1.0f);
			RotateX(sGravityDir,Radians(m_pPro.sGravityDir.x) );
			RotateZ(sGravityDir, Radians(m_pPro.sGravityDir.z) );

			for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
			{
				pAtomic = m_vNodeActive[i];//(*it);
				pAtomic->m_sPos += sGravityDir * ((m_pPro.fGravityStart + m_pPro.fGravitySpeed * pAtomic->m_fLivedTime) * pAtomic->m_fLivedTime);
			}
		}
	}

	// Dir
	{
		// Rot
		if( m_pPro.bDirChildRotate )
		{
			bool bRotX = ( ( m_pPro.fDirChildRotateSpeedX > 0.0f || m_pPro.fDirChildRotateSpeedX < 0.0f ) ? true : false );
			bool bRotY = ( ( m_pPro.fDirChildRotateSpeedY > 0.0f || m_pPro.fDirChildRotateSpeedY < 0.0f ) ? true : false );
			bool bRotZ = ( ( m_pPro.fDirChildRotateSpeedZ > 0.0f || m_pPro.fDirChildRotateSpeedZ < 0.0f ) ? true : false );

			for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
			{
				pAtomic = m_vNodeActive[i];//(*it);

				if( m_pPro.bDirChildRandomRotateX )	pAtomic->m_sRotate.x = (float)rand();
				else if( bRotX )						pAtomic->m_sRotate.x = m_pPro.fDirChildRotateSpeedX * pAtomic->m_fLivedTime;
				if( m_pPro.bDirChildRandomRotateY )	pAtomic->m_sRotate.y = (float)rand();
				else if( bRotY )						pAtomic->m_sRotate.y = m_pPro.fDirChildRotateSpeedY * pAtomic->m_fLivedTime;
				if( m_pPro.bDirChildRandomRotateZ )	pAtomic->m_sRotate.z = (float)rand();
				else if( bRotZ )						pAtomic->m_sRotate.z = m_pPro.fDirChildRotateSpeedZ * pAtomic->m_fLivedTime;
			}
		}
	}

	// Scale
	if( m_pPro.bScale )
	{
		float	fLifeTime;
		float	fBaseRegular;
		float	fBaseSin;
		float	fAdditionRegular;
		float	fAdditionSin;
		float	fScaleRateHalfBase		= (m_pPro.fScaleBaseSinStart - m_pPro.fScaleBaseSinEnd) * 0.5f;
		float	fScaleRateHalfAddition	= (m_pPro.fScaleAdditionSinStart - m_pPro.fScaleAdditionSinEnd) * 0.5f;
		float	fBaseTimeRate_X_Phi		= (m_pPro.fLifeTime / m_pPro.fScaleBaseSinPeriodicTime * PI);
		float	fAdditionTimeRate_X_Phi	= (m_pPro.fLifeTime / m_pPro.fScaleAdditionSinPeriodicTime * PI);

		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pAtomic = m_vNodeActive[i];//(*it);

			fLifeTime			= pAtomic->m_fLivedTime;
			fBaseRegular		= 1.0f;
			fBaseSin			= 1.0f;
			fAdditionRegular	= 1.0f;
			fAdditionSin		= 1.0f;

			if( m_pPro.bScaleBaseRegular )
			{
				fBaseRegular = m_pPro.fScaleBaseRegularStart + m_pPro.fScaleBaseRegularSpeed * fLifeTime;
			}
			if( m_pPro.bScaleBaseSin )
			{
				fBaseSin = (cos(pAtomic->m_fPersentage * fBaseTimeRate_X_Phi) + 1.0f) * fScaleRateHalfBase + m_pPro.fScaleBaseSinEnd;
			}
			if( m_pPro.bScaleAdditionRegular )
			{
				fAdditionRegular = m_pPro.fScaleAdditionRegularStart + m_pPro.fScaleAdditionRegularSpeed * fLifeTime;
			}
			if( m_pPro.bScaleAdditionSin )
			{
				fAdditionSin = (cos(pAtomic->m_fPersentage * fAdditionTimeRate_X_Phi) + 1.0f) * fScaleRateHalfAddition 
					+ m_pPro.fScaleAdditionSinEnd;
			}

			pAtomic->m_sScale.x = fBaseRegular;
			pAtomic->m_sScale.y = fBaseRegular;
			pAtomic->m_sScale.z = fBaseRegular;

			if( m_pPro.bScaleBaseSin )
			{
				pAtomic->m_sScale *= fBaseSin;
			}
			if( m_pPro.bScaleAdditionX )
			{
				pAtomic->m_sScale.x *= fAdditionRegular;
				pAtomic->m_sScale.x *= fAdditionSin;
			}
			if( m_pPro.bScaleAdditionY )
			{
				pAtomic->m_sScale.y *= fAdditionRegular;
				pAtomic->m_sScale.y *= fAdditionSin;
			}
			if( m_pPro.bScaleAdditionZ )
			{
				pAtomic->m_sScale.z *= fAdditionRegular;
				pAtomic->m_sScale.z *= fAdditionSin;
			}
		}
	}




	// Matrix
	TransMatrix();

	// Update LifeNode
	if( m_vNodeInst.size() == 0 )//empty() )
	{
		// Alpha
		if( m_pPro.bAlphaRegular )
		{
			float fGapAlpha = m_pPro.fAlphaRegularEnd - m_pPro.fAlphaRegularStart;

			for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
			{
				pAtomic = m_vNodeActive[i];//(*it);
				pAtomic->m_fAlpha = fGapAlpha * pAtomic->m_fPersentage + m_pPro.fAlphaRegularStart;
			}
		}
		if( m_pPro.bAlphaSin )
		{
			float	fAlphaRateHalf	= (m_pPro.fAlphaSinStart - m_pPro.fAlphaSinEnd) * 0.5f;
			float	fTimeRate_X_Phi = (m_pPro.fLifeTime / m_pPro.fAlphaSinPeriodicTime * PI);
			
			for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
			{
				pAtomic = m_vNodeActive[i];//(*it);
				pAtomic->m_fAlpha = (cos(pAtomic->m_fPersentage * fTimeRate_X_Phi + PI) + 1.0f) * fAlphaRateHalf + m_pPro.fAlphaSinEnd;
			}
		}

		// UpdateVertex, AtomicAni
		if(m_pAtomicInst)
			m_pAtomicInst->Update(this);
	}

}

void EffectNode::TransMatrix()
{CPUTime(EffectNode);
	AotmicData*	pAtomic;

	// Pos
	{
//		matrix4x4		sMatPos;
		if( m_pPro.bDirChildBillBoard )
		{
//			float3		sPosZero(0.0f, 0.0f, 0.0f);
			for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
			{
				pAtomic	= m_vNodeActive[i];//(*it);
//				pAtomic->m_sPos += TransVector(pAtomic->m_sMatPrev,sPosZero);
				pAtomic->m_sPos = TransVector(pAtomic->m_sMatPrev,pAtomic->m_sPos);
				Translate(pAtomic->m_sMat,pAtomic->m_sPos);
			}
		}
		else
		{
			matrix4x4		sMatPos;
			for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
			{
				pAtomic	= m_vNodeActive[i];//(*it);
				Translate(sMatPos,pAtomic->m_sPos);
				Mul(pAtomic->m_sMat,pAtomic->m_sMatPrev, sMatPos);	// 咯扁俊辑 谅钎拌啊 惑加等促.
			}
		}
	}

	// Dir
	{
		matrix4x4		sMatDir;
		matrix4x4		sMatDirX;
		matrix4x4		sMatDirY;
		matrix4x4		sMatDirZ;
		matrix4x4		sMatRot;
		BOOL		bRotX = FALSE;
		BOOL		bRotY = FALSE;
		BOOL		bRotZ = FALSE;

		sMatDir.identity();
		if( m_pPro.bDirChildBillBoard )
		{
//#ifdef _TOOLMODE
//			RotateY(sMatDirY,Radians(_Camera()->GetAngleY()) );
//			RotateX(sMatDirX,Radians(_Camera()->GetAngleX()) );
//#else
//
//#ifdef DEF_FREECAM
//			gDrawMap*	pDrawMap = gDrawMap::GetInterface();
//
//			RotateY(sMatDirY,-pDrawMap->s_fCamRotY );
//			RotateX(sMatDirX,-pDrawMap->s_fCamRotX );
//#else
//			RotateZ(sMatDirY,Radians(45.0f) );
//			RotateX(sMatDirX,Radians(45.0f) );
//#endif // DEF_FREECAM
//#endif
//			RotateY(sMatDirY,Radians(_Camera()->GetAngleY()) );
//			RotateX(sMatDirX,Radians(_Camera()->GetAngleX()) );
//			Mul(sMatDir,sMatDirY, sMatDirX);
			sMatDir = * GetCameraPtr()->GetBoardWorld(MatrixBoard_CameraBoard);
		}
		if( m_pPro.bDirChildStart )
		{
			RotateY(sMatDirY,Radians(-m_pPro.sDirChildStart.y) );
			RotateX(sMatDirX,Radians(-m_pPro.sDirChildStart.x) );
			RotateZ(sMatDirZ,Radians(-m_pPro.sDirChildStart.z) );

			Mul(sMatDir,sMatDirY, sMatDirX);
			Mul(sMatDir,sMatDirZ);
		}

		// Rot
		if( m_pPro.bDirChildRotate )
		{
			bRotX = ( ( m_pPro.fDirChildRotateSpeedX > 0.0f || m_pPro.fDirChildRotateSpeedX < 0.0f ) ? true : false );
			bRotY = ( ( m_pPro.fDirChildRotateSpeedY > 0.0f || m_pPro.fDirChildRotateSpeedY < 0.0f ) ? true : false );
			bRotZ = ( ( m_pPro.fDirChildRotateSpeedZ > 0.0f || m_pPro.fDirChildRotateSpeedZ < 0.0f ) ? true : false );
			bRotX |= m_pPro.bDirChildRandomRotateX;
			bRotY |= m_pPro.bDirChildRandomRotateY;
			bRotZ |= m_pPro.bDirChildRandomRotateZ;
		}

		// Expansion Dir
		if( m_pPro.bExpansionMatrix )
		{
			for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
			{
				pAtomic	= m_vNodeActive[i];//(*it);
				Mul(pAtomic->m_sMat,pAtomic->m_sMatExpansion);
			}
		}

		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pAtomic	= m_vNodeActive[i];//(*it);

			Mul(pAtomic->m_sMat,sMatDir);

			if( bRotY )
			{
				RotateY(sMatRot,Radians(pAtomic->m_sRotate.y) );
				Mul(pAtomic->m_sMat,sMatRot);
			}
			if( bRotX )
			{
				RotateX(sMatRot,Radians(pAtomic->m_sRotate.x) );
				Mul(pAtomic->m_sMat,sMatRot);
			}
			if( bRotZ )
			{
				RotateZ(sMatRot,Radians(pAtomic->m_sRotate.z) );
				Mul(pAtomic->m_sMat,sMatRot);
			}
		}
	}

	// Scale
	{
		if( m_pPro.bScale )
		{
			matrix4x4		sMatScale;

			for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
			{
				pAtomic	= m_vNodeActive[i];//(*it);
				Scale(sMatScale,pAtomic->m_sScale.x, pAtomic->m_sScale.y, pAtomic->m_sScale.z);
				Mul(pAtomic->m_sMat,sMatScale);
			}
		}
	}


}


void EffectNode::UpdateKill_At()
{CPUTime(EffectNode);
	AotmicData*	pAtomic;
	float		fPlusTime		= g_manager.GetElapsedTime();
	float		fPlusPersent	= g_manager.GetElapsedTime() / m_pPro.fLifeTime;
	// Persent Update
	for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
	{
		pAtomic					= m_vNodeActive[i];//(*it);
		pAtomic->m_fLivedTime	+= fPlusTime;
		pAtomic->m_fPersentage	+= fPlusPersent;
	}

	// Kill
	if( !m_bNoKill )
	{
		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pAtomic	= m_vNodeActive[i];//(*it);

			if( pAtomic->m_fPersentage > 1.0f )
			{
				m_pAtomicInst->DestroyAtomic(pAtomic);
				m_vNodeActive.eraseByIndex(i);
				i--;
				//m_vNodeActive.erase(it--);
			}
		}
	}
	else if( !m_bLife )
	{
		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pAtomic	= m_vNodeActive[i];//(*it);

			m_pAtomicInst->DestroyAtomic(pAtomic);
			//m_vNodeActive.erase(it--);
		}
		m_vNodeActive.clear();
	}

}

void EffectNode::UpdateCreate_At()
{CPUTime(EffectNode);
	if( m_bNoCreate )	return;
	if( m_pPro.eCreate == EONECREATE || m_pPro.eCreate == ECONTINUE )
	{
		m_bNoCreate = true;
	}

	float	fStartGap	= m_pPro.fCreateTime - m_fCreateTime;

#ifdef _TOOLMODE
	if( !m_pAtomicInst )
	{
		MessageBox(NULL, STR_2010, MB_OK);
		GetEffectMgr()->ActiveEffect(0, FALSE);
		GetEffectMgr()->KillEffect(0);
		return;
	}
#endif

	m_fCreateTime	+= g_manager.GetElapsedTime();
	if( m_bLife && m_fCreateTime > m_pPro.fCreateTime )
	{
		AotmicData*	pAtomic;
		int			nNumAtomic = m_pPro.nNumNode;
		float		fRadianX;
		float		fRadianY;



		float	fStartLimit	= g_manager.GetElapsedTime() - m_pPro.fLifeTime;
		float f;
		for(f = fStartGap; f < g_manager.GetElapsedTime(); f += m_pPro.fCreateTime)
		{
			if( f > fStartLimit )
			{
				for(int i = 0; i < nNumAtomic; ++i)
				{
					if(!m_pAtomicInst)break;;
					pAtomic		= m_pAtomicInst->CreateAtomic(&m_pPro);
					if(!pAtomic)break;
					pAtomic->m_fLivedTime				= f;
					pAtomic->m_fPersentage				= f / m_pPro.fLifeTime;
					pAtomic->m_fNowFrame				= 0.0;
					pAtomic->m_nNowTexture				= 0;
					pAtomic->m_sExpansionPos			= _CreatePosStart(i, fRadianX, fRadianY);
					pAtomic->m_sExpansionDir			= _CreatePosMove(pAtomic->m_sExpansionPos, fRadianX, fRadianY);
					pAtomic->m_fExpansionSpeed			= _CreateMoveSpeed();
					if( m_pPro.bExpansionMatrix )
					{
						pAtomic->m_sMatExpansion		= _CreatePosMatrix(fRadianX, fRadianY);
					}
					pAtomic->m_fAlpha					= 1.0f;
					
					pAtomic->m_sMatPrev					= m_sMat;

					m_vNodeActive.push_back(pAtomic);
				}
				if( m_bNoCreate )	break;
			}
		}
		m_fCreateTime = m_pPro.fCreateTime - (f - g_manager.GetElapsedTime());
	}

	if( m_pPro.eCreate == EONECREATE )
	{
		m_bLife = false;
	}

}
void EffectNode::Update_Prev()
{CPUTime(EffectNode);
	// Attach Inst
	if( m_dwAttachEndID )
	{
		float3		sPosStart;
		m_bLife		= false;
		//if( !pInst )				m_bLife		= false;
		//else						m_sPosStart	= float3(pInst->m_fPos);
		m_sPosStart = GetMinePosition();
		sPosStart	= m_sPosStart;

		if( m_pPro.bStartPos )		sPosStart += m_pPro.sStartPos;

		Translate(m_sMatStart,sPosStart);

		if( m_pPro.bInheritEndPos )	// 咯扁啊 嘎唱?
		{
			EffectNode	*pNode;

			for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
			{
				pNode = (EffectNode*)m_vNodeActive[i];//(*it);
				Translate(pNode->m_sMatStart,sPosStart);
			}
		}

	}

	// Matrix Start
	Mul(m_sMat, m_sMatStart);

	// EqualNodeDir
	if( m_pPro.bDirEqualNodeDir && !m_pPro.bFollowing )
	{
		matrix4x4		sMatDirEqual;

		m_sDir = m_sPosEnd - m_sPosStart;
		m_sDir.normalize();

		sMatDirEqual	= DirToMatrix(m_sDir);
		Mul(m_sMat, sMatDirEqual);
	}

	// Dir
	{
		matrix4x4		sMatPos;
		matrix4x4		sMatRot;
		matrix4x4		sMatDir;
		matrix4x4		sMatDirX;
		matrix4x4		sMatDirY;
		matrix4x4		sMatDirZ;

		if( m_pPro.bDirBillBoard )
		{
//			float3		sPosZero(0.0f, 0.0f, 0.0f);
//
//			sPosZero = TransVector(m_sMat,sPosZero);
//			Translate(m_sMat,sPosZero);
//#ifdef _TOOLMODE
//			RotateY(sMatDirY,Radians(_Camera()->GetAngleY()) );
//			RotateX(sMatDirX,Radians(_Camera()->GetAngleX()) );
//#else
//#ifdef DEF_FREECAM
//			gDrawMap*	pDrawMap = gDrawMap::GetInterface();
//
//			RotateY(sMatDirY,-pDrawMap->s_fCamRotY );
//			RotateX(sMatDirX,-pDrawMap->s_fCamRotX );
//#else
//			RotateZ(sMatDirY,Radians(45.0f) );
//			RotateX(sMatDirX,Radians(45.0f) );
//#endif // DEF_FREECAM
//#endif
//			Mul(sMatDir,sMatDirY, sMatDirX);
			sMatDir = * GetCameraPtr()->GetBoardWorld(MatrixBoard_CameraBoard);
			Mul(m_sMat, sMatDir);
		}
		if( m_pPro.bDirStart )
		{
			RotateX(sMatDirX,Radians(m_pPro.sDirStart.x) );
			RotateY(sMatDirY,Radians(m_pPro.sDirStart.y) );
			RotateZ(sMatDirZ,Radians(m_pPro.sDirStart.z) );

			Mul(sMatPos,sMatDirY, sMatDirX);
			Mul(sMatPos,sMatDirZ);
			Mul(m_sMat, sMatPos);
		}
		if( m_pPro.bDirRotate )
		{
			RotateX(sMatDirX,Radians(m_pPro.fDirRotateSpeedX * m_fLivedTime) );
			RotateY(sMatDirY,Radians(m_pPro.fDirRotateSpeedY * m_fLivedTime) );
			RotateZ(sMatDirZ,Radians(m_pPro.fDirRotateSpeedZ * m_fLivedTime) );
			Mul(sMatRot,sMatDirY, sMatDirX);
			Mul(sMatRot,sMatDirZ);
			Mul(m_sMat, sMatRot);
		}
	}

	// Following && EqualNodeDir 捞巴阑 林狼秦具 且巴捞促~~.
	if( m_pPro.bFollowing )
	{
		if( m_dwFollowingEndID )
		{
			//gCharInst*		pInst		= gCharCtrl::GetInterface()->GetInst(m_dwFollowingEndID);

			//if( !pInst )	m_bLife		= false;
			//else			m_sPosEnd	= float3(pInst->m_fPos);
			m_sPosEnd = GetMinePosition();
			m_bLife		= false;
			m_sPosEnd += m_pPro.sEndPos;	// 褥.. 咯扁俊 初栏搁 救蹬绰单..
		}

		float		fMovePlus;
		float3		sDirTarget		= m_sPosEnd - m_sPosWorld;
		float3		sDirEffect		= m_sDir;
		float3		sPosEffect;
		float3		sPosMovePlus	= float3(0.0f, 0.0f, 1.0f);
		float3		sPosMoved		= m_sPosWorld - m_sPosStart;
		matrix4x4		sMatPos;
		matrix4x4		sMatDir;
		matrix4x4		sMatPosMovePlus;


		// 捞悼
		fMovePlus			= (m_pPro.fStraightMoveSpeed + m_pPro.fStraightAccelSpeed * m_fLivedTime) * g_manager.GetElapsedTime();
		m_fFollowingRange	= m_pPro.fFollowingRangeStart + m_pPro.fFollowingRangeAccel * m_fLivedTime;

		if( m_fFollowingRange > 7.0f )		m_fFollowingRange = 7.0f;
		
		sDirTarget.normalize();
		sDirTarget	*= m_fFollowingRange;
		sDirEffect	/= m_fFollowingRange;
		sDirEffect	= sDirTarget + sDirEffect;
		sDirEffect.normalize();
		m_sDir	= sDirEffect;

		sPosMovePlus *= fMovePlus;

		Translate(sMatPosMovePlus,sPosMovePlus);
		Translate(sMatPos,sPosMoved);
		sMatDir	= DirToMatrix(sDirEffect);
		Mul(m_sMat, sMatPos);
		Mul(m_sMat, sMatDir);
		Mul(m_sMat, sMatPosMovePlus);
	}

	// Straight 咯扁啊 亮阑鳖?
	if( m_pPro.bStraight && !m_pPro.bFollowing )
	{
		matrix4x4		sMatStraight;
		float3		sDirStraight;
		float		fDistance;

		fDistance		= (m_pPro.fStraightMoveSpeed + m_pPro.fStraightAccelSpeed * m_fLivedTime) * m_fLivedTime;
		sDirStraight	= m_pPro.sStraightDir * fDistance;

		Translate(sMatStraight,sDirStraight);
		Mul(m_sMat, sMatStraight);
	}

	// Rotate
	if( m_pPro.bRotate )
	{
		matrix4x4		sMatDir;
		matrix4x4		sMatDirX;
		matrix4x4		sMatDirY;
		matrix4x4		sMatRotateZ;
		matrix4x4		sMatRadius;
//		matrix4x4		sMatMoveDir;
		float3		sRotateRadius(0.0f, 1.0f, 0.0f);

		RotateY(sMatDirY,Radians(m_pPro.sRotateDir.y) );
		RotateX(sMatDirX,Radians(m_pPro.sRotateDir.x) );
		Mul(sMatDir,sMatDirY, sMatDirX);

		RotateZ(sMatRotateZ,Radians(m_pPro.fRotateAngleSpeed * m_fLivedTime) );
		sRotateRadius	*= m_pPro.fRotateRadius;
		Translate(sMatRadius,sRotateRadius);
//		sMatMoveDir.RotateY( Radians(-90.0f) );
		Mul(m_sMat, sMatDir);
		Mul(m_sMat, sMatRotateZ);
		Mul(m_sMat, sMatRadius);
	}
	AotmicData*	pAtomic;

	// World Pos
	m_sPosWorldPrev = m_sPosWorld;
	m_sPosWorld		= TransVector(m_sMat,float3(0.0f, 0.0f, 0.0f));

	// Check Bound
	if( m_bLife && m_pPro.bBoundCheck )
	{
		if( m_pPro.bFollowing )
		{
			if( BoundPointSphere(m_sPosEnd, 50.0f, m_sPosWorldPrev, m_sPosWorld) )
			{
				m_bLife = false;

				// NextEffect
				if( m_pPro.szNextEffectName[0] )
				{
					float3		sPosStart	= m_sPosWorld;
					float3		sPosEnd		= m_sPosWorld;
					GetEffectMgr()->PlayEffect(m_pPro.szNextEffectName, sPosStart, sPosEnd);
				}
				if( m_pSkill )
				{
					//m_pSkill->End();
					m_pSkill = NULL;
				}
			}
		}
		else
		{
			if( m_sPlane.MultiPlane(m_sPosWorld) )
			{
				m_bLife = false;

				// NextEffect
				if( m_pPro.szNextEffectName[0] )
				{
					float3		sPosStart	= m_sPosWorld;
					float3		sPosEnd		= m_sPosWorld;
					GetEffectMgr()->PlayEffect(m_pPro.szNextEffectName, sPosStart, sPosEnd);
				}
				if( m_pSkill )
				{
					//m_pSkill->End();
					m_pSkill = NULL;
				}
			}
		}
	}

	// Inherit Matrix
	if( !m_pPro.bInheritMatrix )
	{
		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pAtomic = m_vNodeActive[i];//(*it);
			pAtomic->m_sMatPrev = m_sMat;
		}
	}

	// Inherit EndPos
/*	if( !m_pPro.bInheritEndPos )
	{
		matrix4x4		sMatWorld	= m_sMatPrev * m_sMat;
		float3		sPosWorld	= sMatWorld.TransVector(m_pPro.sEndPos);
		for(it = it_start; it != it_end; ++it)
		{
			pAtomic = (*it);
			pAtomic->m_sPosEnd = s;
		}
	}
*/

	// Sound
	UpdateSound();


	// DynamicLight
	UpdateDynamicLight();

}

void EffectNode::SetNextEffectName(char *szNextEffect)
{CPUTime(EffectNode);
	strncpy(m_pPro.szNextEffectName, szNextEffect, 32);
}

void EffectNode::SetSoundName(char *szSound)
{CPUTime(EffectNode);

	strncpy(m_pPro.szSoundName, szSound, 32);

}

void EffectNode::PlaySound()
{CPUTime(EffectNode);
	// 捞惑窃... 促矫  		
	//if( m_eff_idx.m_nUserMark >= 0 ) return;
	if( m_eff_idx.m_nUserMark < 0 ) return;
	//gCharInst *mi = gCharMainPlayer::GetMainInst();
	//assert(mi);
		
	//float3 dv = float3(mi->m_fPos) - float3(m_sPosWorld.v);

	//float sd = dv*dv;
	//if (sd > 1600*1600)	return;
	
	//m_eff_idx.m_nUserMark = CTSndCtrl::GetInterface()->GetSoundUserMark(string(m_pPro.szSoundName));;
	//m_eff_idx.m_n3didx = CTSndCtrl::GetInterface()->Play( m_eff_idx.m_nUserMark, m_sPosWorld.v, m_pPro.bSoundRepeat );	

}

void EffectNode::UpdateSound()
{CPUTime(EffectNode);
	if( m_eff_idx.m_nUserMark < 0 ) return;
//	CTSndCtrl::GetInterface()->SetPos( m_eff_idx, m_sPosWorld.v );
}

void EffectNode::StopSound()
{CPUTime(EffectNode);

	if( m_pPro.bSoundRepeat && m_eff_idx.m_nUserMark >= 0 )
	{
	//	CTSndCtrl::GetInterface()->Stop( m_eff_idx );
		m_eff_idx.Release();
	}
}

void EffectNode::DecodNextEffect()
{CPUTime(EffectNode);
	EffectNode*	pNode;
	for(int i=0;i<m_vNodeInst.size();i++)//it = it_start; it != it_end; ++it)
	{
		pNode = (EffectNode*)m_vNodeInst[i];//(*it);
		pNode->DecodNextEffect();
	}

}

void EffectNode::UpdateDynamicLight()
{CPUTime(EffectNode);
//#ifndef	_TOOLMODE
//	// Dynamic Light
//	if( m_pPro.bDynamicLight && m_pLight )
//	{
//		gLightGen*	pLight		= gLightGen::GetInterface();
//		matrix4x4		sMatWorld	= m_sMatPrev * m_sMat;
//		float3		sPosWorld	= sMatWorld.TransVector(float3(0.0f, 0.0f, 0.0f));
//		BYTE		ucRGB[]		= 
//		{
//			(BYTE)m_pPro.sDynamicLightColor.x,
//			(BYTE)m_pPro.sDynamicLightColor.y, 
//			(BYTE)m_pPro.sDynamicLightColor.z
//		};
//
//		pLight->dyn_SetPos(m_pLight, sPosWorld.v);
//		pLight->dyn_SetRadius(m_pLight, m_pPro.fDynamicLightRange);
//		pLight->dyn_SetColor(m_pLight, ucRGB);
//
//		if( !m_bLife && m_pLight )	// 咯扁辑 懦~~
//		{
//			gLightGen::GetInterface()->dyn_Delete(m_pLight);
//			m_pLight = NULL;
//		}
//	}
//#endif // _TOOLMODE

}

void EffectNode::UpdateVibration()
{CPUTime(EffectNode);
	if( m_pPro.bVibration )
	{
		//_ViewVibration()->AddVibration(&m_pPro, m_sPosWorld);
	}
}



void EffectNode::_InitActive(EffectNode* pNode, matrix4x4& sMat)
{CPUTime(EffectNode);
	EffectNode*		pNodeNew;
	EffectNode*		pNodeSrc;

	m_pPro		= pNode->m_pPro;

	m_eff_idx = pNode->m_eff_idx;

	if( pNode->m_vNodeInst.size() == 0)//.empty() )
	{
		m_pAtomicInst = pNode->m_pAtomicInst;
	}

	// Value
	m_bActive				= true;
	m_bLife					= true;
	m_bNoCreate				= false;
	m_bNoKill				= ( m_pPro.eCreate == ECONTINUE ? true : false );
	m_fDelayTime			= m_pPro.fDelayTime;
	m_fCreateTime			= m_pPro.fCreateTime;
	m_fFollowingRange		= m_pPro.fFollowingRangeStart;
	m_dwFollowingEndID		= 0;
	m_dwAttachEndID			= 0;

	// Matrix
	{
		AotmicData::m_sMat.identity();
		AotmicData::m_sMatPrev	= sMat;		// 雷? m_sMatPrev俊 惑加?  m_sMatPrev = sMat * matrix4x4(m_pPrp->sStartPos)

		// StartPos
		float3		sPosStart;
		float3		sPosEnd;
		if( m_pNodeParent && m_pNodeParent->m_pPro.bInheritEndPos )
		{
			sPosStart	= pNode->m_pNodeParent->m_sPosStart;
			sPosEnd		= pNode->m_pNodeParent->m_sPosEnd;
		}
		else
		{
			sPosStart	= m_pPro.sStartPos;
			sPosEnd		= m_pPro.sEndPos;
		}
		if( m_pPro.bPosRevers )
		{
			float3		sTemp;
			sTemp		= sPosStart;
			sPosStart	= sPosEnd;
			sPosEnd		= sTemp;
		}

		if( m_pPro.bStartPos )
		{
//			AotmicData::Translate(m_sMatStart,sPosStart.x, sPosStart.y, sPosStart.z);
			Translate(m_sMatStart,m_pPro.sStartPos);
		}
		else
		{
			AotmicData::m_sMatStart.identity();
		}

		m_sPosStart	= sPosStart;
		m_sPosEnd	= sPosEnd;
		m_sPosWorld = TransVector(AotmicData::m_sMatPrev,sPosStart);
		m_sPosWorldPrev = m_sPosWorld;
	}
	for(int i=0;i<pNode->m_vNodeInst.size();i++)//it = it_start; it != it_end; ++it)
	{
		pNodeNew = new EffectNode;
		pNodeSrc = (EffectNode*)pNode->m_vNodeInst[i];//(*it);
		pNodeNew->m_pNodeParent = this;
		pNodeNew->_InitActive(pNodeSrc, AotmicData::m_sMat);
		m_vNodeInst.push_back(pNodeNew);
	}


	
	if( m_pPro.szSoundName[0] )
	{	
		//PlaySound();
	}
	

	// DynamicLight
//#ifndef _TOOLMODE
//	if( m_pPro.bDynamicLight )
//	{
//		matrix4x4	sMatWorld	= AotmicData::m_sMatPrev * AotmicData::m_sMatStart;
//		float3	sStartPos	= sMatWorld.TransVector(float3(0.0f, 0.0f, 0.0f));
//		BYTE	ucRGB[]		= 
//		{
//			(BYTE)m_pPro.sDynamicLightColor.x, 
//			(BYTE)m_pPro.sDynamicLightColor.y, 
//			(BYTE)m_pPro.sDynamicLightColor.z
//		};
//
//		m_pLight = gLightGen::GetInterface()->dyn_Create(ucRGB, m_pPro.fDynamicLightRange, sStartPos.v);
//	}
//	else
//	{
//		m_pLight = NULL;
//	}
//#endif

}

void EffectNode::_InitActive(EffectNode* pNode, const float3& sStartPos, const float3& sEndPos)
{CPUTime(EffectNode);
	EffectNode*		pNodeNew;
	EffectNode*		pNodeSrc;
	float3		sStartPos_	= sStartPos;
	float3		sEndPos_	= sEndPos;

	m_pPro		= pNode->m_pPro;

	m_eff_idx = pNode->m_eff_idx;

	if( pNode->m_vNodeInst.size() == 0)//.empty() )
	{
		m_pAtomicInst = pNode->m_pAtomicInst;
	}

	// Value
	m_bActive				= true;
	m_bLife					= true;
	m_bNoCreate				= false;
	m_bNoKill				= ( m_pPro.eCreate == ECONTINUE ? true : false );
	m_fDelayTime			= m_pPro.fDelayTime;
	m_fCreateTime			= m_pPro.fCreateTime;
	m_fLivedTime			= 0.0f;
	m_fFollowingRange		= m_pPro.fFollowingRangeStart;
	m_dwFollowingEndID		= 0;
	m_dwAttachEndID			= 0;


	// Matrix
	{
		AotmicData::m_sMat.identity();
		AotmicData::m_sMatPrev.identity();

		// StartPos
		if( m_pPro.bStartPos )
		{
			sStartPos_ += m_pPro.sStartPos;
		}
		// EndPos
		if( m_pPro.bEndPos )
		{
			sEndPos_ += m_pPro.sEndPos;
		}
		if( m_pPro.bPosRevers )
		{
			float3		sTemp;
			sTemp		= sStartPos_;
			sStartPos_	= sEndPos_;
			sEndPos_	= sTemp;
		}

		Translate(m_sMatStart,sStartPos_);
		m_sPosStart	= sStartPos_;
		m_sPosEnd	= sEndPos_;
		m_sPosWorld	= sStartPos_;		// 烙矫
		m_sPosWorldPrev = m_sPosWorld;

		m_sDir		= m_sPosEnd - m_sPosStart;
		m_sDir.normalize();
/*
		// EqualNodeDir
		if( m_pPro.bDirEqualNodeDir )
		{
			m_sDir = sEndPos_ - sStartPos_;
			
			AotmicData::m_sMatStart = AotmicData::m_sMatStart * DirToMatrix(m_sDir);
		}
//*/
	}
	

	// Plane
	m_sPlane.SetPlane(sStartPos_, sEndPos_);

	for(int i=0;i<pNode->m_vNodeInst.size();i++)//it = it_start; it != it_end; ++it)
	{
		pNodeNew = new EffectNode;
		pNodeSrc = (EffectNode*)pNode->m_vNodeInst[i];//(*it);
		pNodeNew->m_pNodeParent = this;
		pNodeNew->_InitActive(pNodeSrc, AotmicData::m_sMatStart);	// AotmicData::m_sMatStart 捞促 林狼.
		m_vNodeInst.push_back(pNodeNew);
	}


	if( m_pPro.szSoundName[0] )
	{
		//PlaySound();
	}
	

	// DynamicLight
	if( m_pPro.bDynamicLight )
	{
		BYTE	ucRGB[] = 
		{
			(BYTE)m_pPro.sDynamicLightColor.x, 
			(BYTE)m_pPro.sDynamicLightColor.y, 
			(BYTE)m_pPro.sDynamicLightColor.z
		};

		//m_pLight = gLightGen::GetInterface()->dyn_Create(ucRGB, m_pPro.fDynamicLightRange, sStartPos_.v);
	}
	else
	{
		m_pLight = NULL;
	}

}

float3 EffectNode::_CreatePosStart(int nStartIndex, float& fRadianX, float& fRadianY)
{CPUTime(EffectNode);
//	if( m_pPro.eStart == EPOINT_EF )		return m_sPos;
	if( m_pPro.eStart == EPOINT_EF )		return float3(0.0f, 0.0f, 0.0f);

//	float3		sPos = m_sPos;
	float3		sPos(0.0f, 0.0f, 0.0f);
	float3		sLine(1.0f, 0.0f, 0.0f);
	float3		sDefault(0.0f, 0.0f, 1.0f);
	float		fScalarRate = m_pPro.fScalarMax - m_pPro.fScalarMin;
	float		fAngleRate	= m_pPro.fAngleMax - m_pPro.fAngleMin;
	float		fRand;
	float		fScalar;
	float		fRadian;


	if( m_pPro.bScalarRegular )
	{
		fScalar = m_pPro.fScalarMin + fScalarRate / (m_pPro.nNumNode - 1) * nStartIndex;
	}
	else
	{
		fScalar = m_pPro.fScalarMin + fmod((float)rand(), fScalarRate);
	}

	if( m_pPro.bAngleRegular )	
	{
		fRadian	= Radians( m_pPro.fAngleMin + fAngleRate / m_pPro.nNumNode * ((float)nStartIndex + 0.5f) );
	}
	else
	{
		fRand	= rand() % (int)fAngleRate;
		fRand	= (fAngleRate > 0.0f ? fRand : -fRand);
		fRadian = Radians( m_pPro.fAngleMin + fRand );
	}
	fRadianY = fRadian;


	switch( m_pPro.eStart )
	{
	case EPOINT_EF:
		{
		}
		break;
	case ELINE_EF:
		{
			if( m_pPro.bScalarRegular )
			{
				fScalar = fScalar * 2.0f - fScalarRate;
			}
			else
			{
				if( rand()%2 > 0 )	fScalar *= -1.0f;
			}
			sLine *= fScalar;
			sPos += sLine;
		}
		break;
	case EPLANE_EF:
		{
			fRadianX = 0.0f;

			RotateY(sDefault,fRadianY);
			sDefault *= fScalar;
			sPos += sDefault;
		}
		break;
	case ESPACE_EF:
		{
			fRadianX = (float)rand();

			RotateX(sDefault,fRadianX);
			RotateY(sDefault,fRadianY);
			sDefault *= fScalar;
			sPos += sDefault;
		}
		break;
	}

	return sPos;

}

EffectNode*		EffectNode::FindNode(int nID)
{CPUTime(EffectNode);
	if(nID == this->m_nID)
		return this;
	for(int i=0;i<this->m_vNodeInst.size();i++)
	{
		EffectNode* n = m_vNodeInst[i]->FindNode(nID);
		if(n)
			return n;
	}
	return 0;
}

float3 EffectNode::_CreatePosMove(float3& sPosStart, float& fRadianX, float& fRadianY)
{CPUTime(EffectNode);
	if( !m_pPro.bMoveMode )		return float3(0.0f, 0.0f, 0.0f);


	float3		sPos = sPosStart - m_sPos;
	float3		sDir(0.0f, 0.0f, 1.0f);

	switch( m_pPro.eMove )
	{
	case EEXPANSION:
		{
			sPos.normalize();
			sDir = sPos;
		}
		break;
	case ECONCENTRATION:
		{
			sPos.normalize();
			sPos *= -1.0f;
			sDir = sPos;

			fRadianX = -fRadianX;
			fRadianY += PI;
		}
		break;
	case ERANDOM:
		{
//			RotateY(sDir, fmod((float)rand(), PHIx2) );
//			RotateX(sDir, fmod((float)rand(), PHIx2) );
			fRadianX = (float)rand();
			fRadianY = (float)rand();
			RotateX(sDir, fRadianX );
			RotateY(sDir, fRadianY );
		}
		break;
	}

	return sDir;

}

float EffectNode::_CreateMoveSpeed()
{CPUTime(EffectNode);
	float	fMoveSpeedRate;
	float	fMoveSpeed;

	fMoveSpeedRate	= m_pPro.fMoveSpeedMax - m_pPro.fMoveSpeedMin;
	fMoveSpeed		= m_pPro.fMoveSpeedMin + fmod((float)rand(), fMoveSpeedRate);

	return fMoveSpeed;

}

matrix4x4 EffectNode::_CreatePosMatrix(float fRadianX, float fRadianY)
{CPUTime(EffectNode);
	matrix4x4		sMatX;
	matrix4x4		sMatY;

	RotateY(sMatY,-fRadianY);
	RotateX(sMatX,-fRadianX);

	return	sMatY * sMatX;
}

Effect::Effect()
{
	m_nID = 0;
	m_render.m_ptr = this;
}

Effect::~Effect()
{

}

void Effect::SaveFile(const char* szFileName)
{CPUTime(Effect);
}

void Effect::LoadFile(const char* szFileName)
{CPUTime(Effect);
	if(!GetEffectMgr())
		CreateEffectMgr();
	CFileHelperR r;
	if(!r.open(szFileName))
		return;
	FileName temp = szFileName;
	FileName szFile;
	temp.setIsFile();
	temp.splitpath(0, &szFile, 0);
	m_effectName = szFile.t;
	Phantom::strlwr_t(m_effectName.t);
	int				nVersion;
	int nRand = rand();
	r >> nVersion;
	r >> nRand;
	char head[4];
	r.pStream->read(head, sizeof(head));
	Load(r);
	m_pNodeParent = NULL;
	int nRandC = 0;
	r >> nRandC;
	assert(nRand == nRandC);
}


void Effect::Update_Prev()
{CPUTime(Effect);
	AotmicData::m_sMat.identity();
//	AotmicData::m_sMatPrev.identity();	// 嘎唱?
	EffectNode::Update_Prev();

}

void Effect::SetAttachPos(float3 &sPos)
{CPUTime(Effect);
	float3		sPosStart = sPos;
	if( m_pPro.bStartPos )
	{
		sPosStart += m_pPro.sStartPos;
	}
	Translate(m_sMatStart,sPosStart);

//	m_sPosStart = sPos;

	if( m_pPro.bInheritEndPos )
	{
		EffectNode	*pNode;

		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pNode = (EffectNode*)m_vNodeActive[i];//(*it);
			Translate(pNode->m_sMatStart,sPosStart);
//			pNode->m_sPosStart = sPos;
		}
	}

}
void Effect::drawObject(const matrix4x4* matPtr)
{CPUTime(Effect);
	if(!g_manager.SetRenderMode(RenderMode_PlaneShadow))
		return;
	g_manager.SetWorldMatrix(&matrix4x4::s_identity());
	Draw();
}
void Effect::SetAttachPos(matrix4x4 &sMat)
{CPUTime(Effect);
	AotmicData::m_sMatStart = sMat;
//	m_sPosStart = sPos;
	if( m_pPro.bStartPos )
	{
		matrix4x4		sMatStart;
		Translate(sMatStart, m_pPro.sStartPos);
		AotmicData::m_sMatStart = sMatStart * AotmicData::m_sMatStart;
	}

	if( m_pPro.bInheritEndPos )
	{
		EffectNode	*pNode;

		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pNode = (EffectNode*)m_vNodeActive[i];//(*it);
			pNode->m_sMatStart = AotmicData::m_sMatStart;
//			pNode->m_sPosStart = sPos;
		}
	}

}

void Effect::SetAttachPos(DWORD dwUniqueID)
{CPUTime(Effect);
	if( !m_pPro.bInheritEndPos )
	{
		m_dwAttachEndID = dwUniqueID;
	}

}

const matrix4x4&	EffectRender::GetWorldMatrix()
{CPUTime(EffectRender);
	return m_ptr->m_sMat;
}

void EffectRender::drawObject(const matrix4x4* matPtr)
{CPUTime(EffectRender);
	if(m_ptr)
		m_ptr->drawObject(matPtr);
}

void Effect::SetAttachEndPos(float3 &sPos)
{CPUTime(Effect);
//	m_sPosEnd = sPos;

	if( m_pPro.bInheritEndPos )
	{
		EffectNode	*pNode;

		for(int i=0;i<m_vNodeActive.size();i++)//it = it_start; it != it_end; ++it)
		{
			pNode = (EffectNode*)m_vNodeActive[i];//(*it);
//			pNode->m_sPosEnd = sPos;
		}
	}
}

void Effect::SetFollowingPos(float3 &sPos)
{CPUTime(Effect);
	if( !m_pPro.bInheritEndPos )
	{
		m_sPosEnd = sPos;
	}

}

void Effect::SetFollowingPos(DWORD dwUniqueID)
{CPUTime(Effect);
	if( !m_pPro.bInheritEndPos )
	{
		m_dwFollowingEndID = dwUniqueID;
	}

}

EffectManager* g_effectMgr = NULL;
VOID				CreateEffectMgr()
{
	if(!g_effectMgr)
		g_effectMgr = new EffectManager();
}
EffectManager*		GetEffectMgr()
{
	if(!g_effectMgr)
		g_effectMgr = new EffectManager();
	return g_effectMgr;
}
VOID				ReleaseEffectMgr()
{
	if(g_effectMgr)
		delete g_effectMgr;
	g_effectMgr = 0;
}

EffectManager::EffectManager()
{
	m_nCreateID = 1;
}

VOID		EffectManager::ClearEffect()
{CPUTime(EffectManager);
	for(int i=0;i<this->m_effectRuntimes.size();i++)
	{
		Effect* effect = m_effectRuntimes[i];
		if(!effect)continue;
		delete m_effectRuntimes[i];
		m_effectRuntimes[i] = 0;
	}
	m_effectRuntimes.clear();
	int nNumInst = m_vAtomicInst.size();
	for(int i = 0; i < nNumInst; ++i)
	{
		m_vAtomicInst[i]->Release();
		delete m_vAtomicInst[i];
	}
	m_vAtomicInst.clear();
	for(int i=0;i<m_meshs.size();i++)
		delete m_meshs[i];
	m_meshs.clear();
}
EffectManager::~EffectManager()
{
	ClearEffect();
}

int EffectManager::IsAtomicInst(char *szFileName)
{CPUTime(EffectManager);
	AtomicBase*	pInst;
	int nNumAtomicInst = m_vAtomicInst.size();

	for(int i = 0; i < nNumAtomicInst; ++i)
	{
		pInst = m_vAtomicInst[i];
		if( !strcmp(szFileName, pInst->GetFileName()) )	return i;
	}

	return -1;

}

void EffectManager::Render()
{CPUTime(EffectManager);
	for(int i=0;i<this->m_effectRuntimes.size();i++)
	{
		Effect* effect = m_effectRuntimes[i];
		if(!effect)continue;
		g_manager.PushAlphaObject(&effect->m_render);
	}
}
void EffectManager::FrameMove()
{CPUTime(EffectManager);
	for(int i=0;i<this->m_effectRuntimes.size();i++)
	{
		Effect* effect = m_effectRuntimes[i];
		if(!effect)continue;
		effect->m_fLivedTime	+= g_manager.GetElapsedTime();
		effect->Update();
		if( !effect->m_bLife && effect->m_fDelayTime <= 0.0f && effect->m_vNodeActive.size() == 0)
		{
			delete m_effectRuntimes[i];
			m_effectRuntimes[i] = 0;
		}
	}
}
Effect* EffectManager::LoadEffect(const char* szEffectFile)//预载入一个特效
{CPUTime(EffectManager);
	for(int i=0;i<m_effectLoads.size();i++)
	{
		if(m_effectLoads[i]->m_fileName == szEffectFile)
			return m_effectLoads[i];
	}
	Effect* effect = new Effect();
	effect->LoadFile(szEffectFile);
	m_effectLoads.push_back(effect);
	effect->m_fileName = szEffectFile;
	return effect;
}
VOID		EffectManager::StopEffect(EffectID& id)
{CPUTime(EffectManager);
	for(int i=0;i<m_effectRuntimes.size();i++)
	{
		if(!m_effectRuntimes[i])continue;
		if(m_effectRuntimes[i]->m_nID == id.wMask)
		{
			delete m_effectRuntimes[i];
			m_effectRuntimes[i] = 0;
			break;
		}
	}
}
EffectID	EffectManager::PlayEffect(const char* szEffectName, const float3& sStartPos, const float3& sEndPos)
{CPUTime(EffectManager);
	if(!szEffectName)
		return EffectID();
	Effect* pEffectPtr = 0;
	for(int i=0;i<m_effectLoads.size();i++)
	{
		if(m_effectLoads[i]->m_effectName == szEffectName)
		{
			pEffectPtr = m_effectLoads[i];
			break;
		}
	}
	if(!pEffectPtr)
		return EffectID();
	Effect* effect = new Effect();
	effect->m_pNodeParent = NULL;
	effect->_InitActive(pEffectPtr, sStartPos, sEndPos);
	int nIndex = m_effectRuntimes.size();
	for(int i=0;i<m_effectRuntimes.size();i++)
	{
		if(m_effectRuntimes[i] == 0)
		{
			m_effectRuntimes[i] = effect;
			nIndex = i;
			break;
		}
	}
	if(nIndex == m_effectRuntimes.size())
		m_effectRuntimes.push_back(effect);
	effect->m_nID = this->m_nCreateID++;
	return EffectID(nIndex, effect->m_nID);
}

Effect*	EffectManager::GetEffectPtr(EffectID& id)
{CPUTime(EffectManager);
	if(id.wIndex < 0)return 0;
	if(id.wIndex < this->m_effectRuntimes.size() && this->m_effectRuntimes[id.wIndex]->m_nID == id.wMask)
		return this->m_effectRuntimes[id.wIndex];
	for(int i=0;i<m_effectRuntimes.size();i++)
	{
		if(m_effectRuntimes[i]->m_nID == id.wMask)
		{
			id.wIndex = i;
			return m_effectRuntimes[i];
		}
	}
	id.wIndex = -1;
	return 0;
}

void EffectManager::SetAttachPos(EffectID& id, float3& sPos)
{CPUTime(EffectManager);
	Effect*	pEffect = GetEffectPtr(id);
	if( pEffect )	return;
	pEffect->SetAttachPos(sPos);

}

void EffectManager::SetAttachPos(EffectID& id, matrix4x4& sMat)
{CPUTime(EffectManager);
	Effect*	pEffect = GetEffectPtr(id);
	if( pEffect )	return;
	pEffect->SetAttachPos(sMat);
}

int EffectManager::AddAtomicInst(char* szFileName, const char* textureFile)
{CPUTime(EffectManager);
	AtomicBase*	pInst;
	char*			szExtension;
	int				nIndex	= IsAtomicInst(szFileName);
	const char* szPath = "";
	if( nIndex == -1 )
	{
		GetExtension(szExtension, szFileName);
		if( szExtension == (char*)1 )		return -1;

		if( stricmp(szExtension, "tga") == 0 || stricmp(szExtension, "jpg") == 0)
		{
			pInst	= new AtomicPlane(textureFile);
		}
		else if( stricmp(szExtension, "rpgFrame")==0)
		{
			pInst	= new AtomicMesh(textureFile);
		}
		else if( stricmp(szExtension, "rpgLine")==0)
		{
			pInst	= new AtomicLine(textureFile);
		}
		else
		{
			return -1;
		}
		char szFile[256];
		strcpy(szFile, "effect/");
		strcat(szFile, szFileName);
		pInst->Init(szFile);
		m_vAtomicInst.push_back(pInst);

		//_EffectManager()->PopFolder();
		return m_vAtomicInst.size() - 1;
	}
	else
	{
		return nIndex;
	}

}

AtomicBase* EffectManager::GetAtomicInst(int nIndex)
{CPUTime(EffectManager);
	if( nIndex == -1 )		return NULL;
	else					return m_vAtomicInst[nIndex];

}

void EffectManager::SetAttachEndPos(EffectID& id, float3& sPos)
{CPUTime(EffectManager);
	Effect*	pEffect = GetEffectPtr(id);
	if( !pEffect )	return;
	pEffect->SetAttachEndPos(sPos);
}

void EffectManager::SetFollowingPos(EffectID& id, float3& sPos)
{CPUTime(EffectManager);
	Effect*	pEffect = GetEffectPtr(id);
	if( !pEffect )	return;
	pEffect->SetFollowingPos(sPos);
}
EffectMesh*	EffectManager::LoadMesh(const char* szMeshFile)	//自动载入模型文件
{CPUTime(EffectManager);
	for(int i=0;i<m_meshs.size();i++)
	{
		if(stricmp(m_meshs[i]->m_strFileName.c_str(), szMeshFile) == 0)
			return m_meshs[i];
	}
	EffectMesh* pMesh = new EffectMesh();
	FileName temp;
	temp = szMeshFile;
	temp.setIsFile();
	FileName dir;
	temp.splitpath(&dir, 0, 0);
	if(!pMesh->Load(temp.t, dir.t))
	{
		delete pMesh;
		return 0;
	}
	m_meshs.push_back(pMesh);
	return pMesh;
}

};
