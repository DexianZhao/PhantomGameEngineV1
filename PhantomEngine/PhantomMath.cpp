//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomMath.h"
#include "PhantomBase.h"
#include "PhantomCamera.h"
	extern "C"{
		float* 			matrixMultiply2( float *pOut, const float *pM1, const float *pM2 );// Matrix multiplication.  The result represents the transformation M2// followed by the transformation M1.  (Out = M1 * M2)
		float* 			matrixInverse2( float *pOut, float *pDeterminant, const float *pM );// Calculate inverse of matrix.  Inversion my fail, in which case NULL will// be returned.  The determinant of pM is also returned it pfDeterminant// is non-NULL.
	};
namespace Phantom{
	float vec3Length( const float3 *pV )
	{
		return sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);
	}

	float vec3LengthSq( const float3 *pV )
	{
		return pV->x * pV->x + pV->y * pV->y + pV->z * pV->z;
	}

	float vec3Dot( const float3 *pV1, const float3 *pV2 )
	{
		return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;
	}

	float3* vec3Cross( float3 *pOut, const float3 *pV1, const float3 *pV2 )
	{
		float3 v;
		v.x = pV1->y * pV2->z - pV1->z * pV2->y;
		v.y = pV1->z * pV2->x - pV1->x * pV2->z;
		v.z = pV1->x * pV2->y - pV1->y * pV2->x;

		*pOut = v;
		return pOut;
	}

	float3* vec3Add( float3 *pOut, const float3 *pV1, const float3 *pV2 )
	{
		pOut->x = pV1->x + pV2->x;
		pOut->y = pV1->y + pV2->y;
		pOut->z = pV1->z + pV2->z;
		return pOut;
	}

	float3* vec3Subtract( float3 *pOut, const float3 *pV1, const float3 *pV2 )
	{
		pOut->x = pV1->x - pV2->x;
		pOut->y = pV1->y - pV2->y;
		pOut->z = pV1->z - pV2->z;
		return pOut;
	}

	float3* vec3Minimize( float3 *pOut, const float3 *pV1, const float3 *pV2 )
	{
		pOut->x = pV1->x < pV2->x ? pV1->x : pV2->x;
		pOut->y = pV1->y < pV2->y ? pV1->y : pV2->y;
		pOut->z = pV1->z < pV2->z ? pV1->z : pV2->z;
		return pOut;
	}

	float3* vec3Maximize( float3 *pOut, const float3 *pV1, const float3 *pV2 )
	{
		pOut->x = pV1->x > pV2->x ? pV1->x : pV2->x;
		pOut->y = pV1->y > pV2->y ? pV1->y : pV2->y;
		pOut->z = pV1->z > pV2->z ? pV1->z : pV2->z;
		return pOut;
	}

	float3* vec3Scale( float3 *pOut, const float3 *pV, float s)
	{
		pOut->x = pV->x * s;
		pOut->y = pV->y * s;
		pOut->z = pV->z * s;
		return pOut;
	}

	float3* vec3Lerp( float3 *pOut, const float3 *pV1, const float3 *pV2,float s )
	{
		pOut->x = pV1->x + s * (pV2->x - pV1->x);
		pOut->y = pV1->y + s * (pV2->y - pV1->y);
		pOut->z = pV1->z + s * (pV2->z - pV1->z);
		return pOut;
	}


	void			qtnFromDirection(quaternion& qtn,float3& vFrom, float3 &vTo)
	{
		float3 vPart;
		float fDot = vFrom.dot(vTo);
		vFrom.cross(vPart,vTo);
		qtn.setXYZW(vPart.x, vPart.y, vPart.z, fDot);
	}
	float		planeDot( const Plane *pP, const float4 *pV)
	{
		return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d * pV->w;
	}
	float		planeDotCoord( const Plane *pP, const float3 *pV)
	{
		return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d;
	}

	float		planeDotNormal( const Plane *pP, const float3 *pV)
	{
		return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z;
	}

	Plane*		planeScale(Plane *pOut, const Plane *pP, float s)
	{
		pOut->a = pP->a * s;
		pOut->b = pP->b * s;
		pOut->c = pP->c * s;
		pOut->d = pP->d * s;
		return pOut;
	}
	Plane*				planeFromPointNormal( Plane *pOut, const float3 *pPoint, const float3 *pNormal)
	{
		pOut->a = pNormal->x;
		pOut->b = pNormal->y;
		pOut->c = pNormal->z;
		pOut->d = -pPoint->dot(*pNormal);
		return pOut;
	}
	Plane::Plane( const float3& p1, const float3& p2, const float3& p3 )
	{
		float3 m = p2-p1;
		float3 n = p3-p1;
		float3 normal = m.cross(n);
		normal.normalize();
		a	=	normal.x;
		b	=	normal.y;
		c	=	normal.z;
		d = -p1.dot(normal);
	}
	color4			color4::s_color(1,1,1,1);
	color4*  colorAdjustContrast(color4 *pout, const color4 *pc, float s)
	{
		pout->r = 0.5f + s * (pc->r - 0.5f);
		pout->g = 0.5f + s * (pc->g - 0.5f);
		pout->b = 0.5f + s * (pc->b - 0.5f);
		pout->a = pc->a;
		return pout;
	}

	color4*  colorAdjustSaturation(color4 *pout, const color4 *pc, float s)
	{
		float grey;

		grey = pc->r * 0.2125f + pc->g * 0.7154f + pc->b * 0.0721f;
		pout->r = grey + s * (pc->r - grey);
		pout->g = grey + s * (pc->g - grey);
		pout->b = grey + s * (pc->b - grey);
		pout->a = pc->a;
		return pout;
	}



	matrix4x4&			matrix4x4::operator	=	(const matrix4x4& v)
	{
		memcpy_s(this,&v,sizeof(matrix4x4));
		return *this;
	}
	//将弧度限制范围在 -PI ~ PI 之间
	float					clampDegress(float fDegress)
	{
		fDegress	=	fmodf(fDegress, PI_X2);
		if(fDegress <= PI && fDegress >= -PI)
			return fDegress;
		if(fDegress > PI)
			fDegress	=	fDegress - PI_X2;
		else if(fDegress < -PI)
			fDegress	=	fDegress + PI_X2;
		return fDegress;
	}

	void	IntersectPlane(const float3& point, const float3& normal, const float3& from, const float3& to, float3& pickPoint)
	{
		float3 pv	=	-(from - point) * normal;
		float3 nn	=	normal*normal;
		float fNN	=	nn.length();
		float pNN	=	pv.length();
		if(fNN < 0.001f)return;
		float t		=	pNN / fNN;
		pickPoint	=	from + normal * t;
	}

	matrix4x4*	matrixMultiply(matrix4x4 *pOut, const matrix4x4 *pM1, const matrix4x4 *pM2)
	{CPUTime("matrix4x4");
#ifdef WIN32_OS
		return (matrix4x4*)matrixMultiply2((float*)pOut, (float*)pM1, (float*)pM2);
//#elif defined(__ARM_NEON__) && !defined(ANDROID_OS)
//        return (matrix4x4*)matrixMultiply2((float*)pOut, (float*)pM1, (float*)pM2);
#else
		const matrix4x4& mt1	=	*pM1;
		const matrix4x4& mt2	=	*pM2;
		*pOut	=	matrix4x4(
			mt1._11 * mt2._11 + mt1._12 * mt2._21 + mt1._13 * mt2._31 + mt1._14 * mt2._41,
			mt1._11 * mt2._12 + mt1._12 * mt2._22 + mt1._13 * mt2._32 + mt1._14 * mt2._42,
			mt1._11 * mt2._13 + mt1._12 * mt2._23 + mt1._13 * mt2._33 + mt1._14 * mt2._43,
			mt1._11 * mt2._14 + mt1._12 * mt2._24 + mt1._13 * mt2._34 + mt1._14 * mt2._44,
			mt1._21 * mt2._11 + mt1._22 * mt2._21 + mt1._23 * mt2._31 + mt1._24 * mt2._41,
			mt1._21 * mt2._12 + mt1._22 * mt2._22 + mt1._23 * mt2._32 + mt1._24 * mt2._42,
			mt1._21 * mt2._13 + mt1._22 * mt2._23 + mt1._23 * mt2._33 + mt1._24 * mt2._43,
			mt1._21 * mt2._14 + mt1._22 * mt2._24 + mt1._23 * mt2._34 + mt1._24 * mt2._44,
			mt1._31 * mt2._11 + mt1._32 * mt2._21 + mt1._33 * mt2._31 + mt1._34 * mt2._41,
			mt1._31 * mt2._12 + mt1._32 * mt2._22 + mt1._33 * mt2._32 + mt1._34 * mt2._42,
			mt1._31 * mt2._13 + mt1._32 * mt2._23 + mt1._33 * mt2._33 + mt1._34 * mt2._43,
			mt1._31 * mt2._14 + mt1._32 * mt2._24 + mt1._33 * mt2._34 + mt1._34 * mt2._44,
			mt1._41 * mt2._11 + mt1._42 * mt2._21 + mt1._43 * mt2._31 + mt1._44 * mt2._41,
			mt1._41 * mt2._12 + mt1._42 * mt2._22 + mt1._43 * mt2._32 + mt1._44 * mt2._42,
			mt1._41 * mt2._13 + mt1._42 * mt2._23 + mt1._43 * mt2._33 + mt1._44 * mt2._43,
			mt1._41 * mt2._14 + mt1._42 * mt2._24 + mt1._43 * mt2._34 + mt1._44 * mt2._44 );
#endif
		return pOut;
	}
	matrix4x4* matrixLookAtRH( matrix4x4 *pOut, const float3 *pEye, const float3 *pAt,const float3 *pUp )
	{CPUTime("matrix4x4");
		float3 eye = *pEye;
		float3 at = *pAt;
		float3 up = *pUp;
		matrix4x4& r	=	*pOut;
		float3   uAxis, vAxis, nAxis;
		nAxis = -(at - eye);
		nAxis.normalize();
		vAxis = up - nAxis * up.dot(nAxis);
		if(vAxis.length() < 1e-5f)
		{
			vAxis.x =      - nAxis.y * nAxis.x;
			vAxis.y = 1.0f - nAxis.y * nAxis.y;
			vAxis.z =      - nAxis.y * nAxis.z;
			if(vAxis.length() < 1e-5f ) {
				vAxis.x =      - nAxis.z * nAxis.x;
				vAxis.y =      - nAxis.z * nAxis.y;
				vAxis.z = 1.0f - nAxis.z * nAxis.z;
				if (vAxis.length() < 1e-5f)
					return pOut;
			}
		}
		vAxis.normalize();
		uAxis = vAxis.cross( nAxis );
		r._11 = uAxis.x;	r._12 = vAxis.x;	r._13 = nAxis.x;	r._14 = 0.0f;
		r._21 = uAxis.y;	r._22 = vAxis.y;	r._23 = nAxis.y;	r._24 = 0.0f;
		r._31 = uAxis.z;	r._32 = vAxis.z;	r._33 = nAxis.z;	r._34 = 0.0f;
		r._41 = -eye.dot(uAxis);
		r._42 = -eye.dot(vAxis);
		r._43 = -eye.dot(nAxis);
		r._44 = 1.0f;
		return pOut;
	}
	matrix4x4* matrixPerspectiveFovRH( matrix4x4 *pOut, float fovy, float Aspect, float zn, float zf )
	{CPUTime("matrix4x4");
		static float fV	=	0.5f;//0.5f;
		matrix4x4& r	=	*pOut;
		float cotFovY = 1.0f/tan(fovy * fV);
		float scalex = cotFovY/ Aspect;
		float scaley = cotFovY;
		r._11 = scalex;	r._12 = 0;		r._13 = 0;				r._14 = 0;
		r._21 = 0;		r._22 = scaley;	r._23 = 0;				r._24 = 0;
		r._31 = 0;		r._32 = 0;		r._33 = zf/(zn-zf);		r._34 = -1.0f;
		r._41 = 0;		r._42 = 0;		r._43 = zn * zf/(zn-zf);r._44 = 0;
		return pOut;
	}

	float				CalculateDeterminant3( float _11, float _12, float _13, float _21, float _22, float _23, float _31, float _32, float _33)
	{
		return _11 * (_22 * _33 - _23 * _32) - _12 * (_21 * _33 - _23 * _31) + _13 * (_21 * _32 - _22 * _31);
	}
	float				Determinant(const matrix4x4& m)
	{
		return m._11 * CalculateDeterminant3(m._22, m._32, m._42, m._23, m._33, m._43, m._24, m._34, m._44)
			- m._12 * CalculateDeterminant3(m._21,  m._31, m._41, m._23, m._33, m._43, m._24, m._34, m._44)
			+ m._13 * CalculateDeterminant3(m._21,  m._31, m._41, m._22, m._32, m._42, m._24, m._34, m._44)
			- m._14 * CalculateDeterminant3(m._21,  m._31, m._41, m._22, m._32, m._42, m._23, m._33, m._43);
	}
	matrix4x4*	matrixScaling( matrix4x4 *pOut, float x, float y, float z )
	{CPUTime("matrix4x4");
		pOut->m[0][1] = pOut->m[0][2] =pOut->m[0][3] = pOut->m[1][0] = pOut->m[1][2] =pOut->m[1][3] = pOut->m[2][0] = pOut->m[2][1] =pOut->m[2][3] = pOut->m[3][0] = pOut->m[3][1] =pOut->m[3][2] = 0.0f;
		pOut->m[0][0] = x;
		pOut->m[1][1] = y;
		pOut->m[2][2] = z;
		pOut->m[3][3] = 1.0f;
		return pOut;
	}
	matrix4x4*	matrixInverse( matrix4x4 *pOut, float *pDeterminant, const matrix4x4 *pM )
	{CPUTime("matrix4x4");
#ifdef WIN32_OS
		return (matrix4x4*)matrixInverse2((float*)pOut, (float*)pDeterminant, (float*)pM);
#else
		float det;
		float a1, b1, c1, d1;
		float a2, b2, c2, d2;
		float a3, b3, c3, d3;
		float a4, b4, c4, d4;
		if (pDeterminant == NULL) 
			pDeterminant = &det;
		*pDeterminant = Determinant(*pM);
		if (fabsf(*pDeterminant) < 1e-5f) 
			return pOut;
		det = 1.0f / (*pDeterminant);
		a1 = pM->_11; b1 = pM->_12; c1 = pM->_13; d1 = pM->_14;
		a2 = pM->_21; b2 = pM->_22; c2 = pM->_23; d2 = pM->_24;
		a3 = pM->_31; b3 = pM->_32; c3 = pM->_33; d3 = pM->_34;
		a4 = pM->_41; b4 = pM->_42; c4 = pM->_43; d4 = pM->_44;
		*pOut	=	*pM;

		pOut->_11 =  CalculateDeterminant3(b2, b3, b4, c2, c3, c4, d2, d3, d4) * det;
		pOut->_21 = -CalculateDeterminant3(a2, a3, a4, c2, c3, c4, d2, d3, d4) * det;
		pOut->_31 =  CalculateDeterminant3(a2, a3, a4, b2, b3, b4, d2, d3, d4) * det;
		pOut->_41 = -CalculateDeterminant3(a2, a3, a4, b2, b3, b4, c2, c3, c4) * det;

		pOut->_12 = -CalculateDeterminant3(b1, b3, b4, c1, c3, c4, d1, d3, d4) * det;
		pOut->_22 =  CalculateDeterminant3(a1, a3, a4, c1, c3, c4, d1, d3, d4) * det;
		pOut->_32 = -CalculateDeterminant3(a1, a3, a4, b1, b3, b4, d1, d3, d4) * det;
		pOut->_42 =  CalculateDeterminant3(a1, a3, a4, b1, b3, b4, c1, c3, c4) * det;

		pOut->_13 =  CalculateDeterminant3(b1, b2, b4, c1, c2, c4, d1, d2, d4) * det;
		pOut->_23 = -CalculateDeterminant3(a1, a2, a4, c1, c2, c4, d1, d2, d4) * det;
		pOut->_33 =  CalculateDeterminant3(a1, a2, a4, b1, b2, b4, d1, d2, d4) * det;
		pOut->_43 = -CalculateDeterminant3(a1, a2, a4, b1, b2, b4, c1, c2, c4) * det;

		pOut->_14 = -CalculateDeterminant3(b1, b2, b3, c1, c2, c3, d1, d2, d3) * det;
		pOut->_24 =  CalculateDeterminant3(a1, a2, a3, c1, c2, c3, d1, d2, d3) * det;
		pOut->_34 = -CalculateDeterminant3(a1, a2, a3, b1, b2, b3, d1, d2, d3) * det;
		pOut->_44 =  CalculateDeterminant3(a1, a2, a3, b1, b2, b3, c1, c2, c3) * det;
#endif
		return pOut;
	}
	matrix4x4*	matrixOrthoRH(matrix4x4* m, float w,float h,float zn,float zf)
	{CPUTime("matrix4x4");
		float nfRatio;
		if (fabsf(w) < 1e-2f) return m;
		if (fabsf(h) < 1e-2f) return m;
		nfRatio = zf - zn;
		if (fabsf(nfRatio) < 1e-2f) return m;
		nfRatio = 1.0f / nfRatio;
		m->_11 = 2.0f / w;
		m->_22 = 2.0f / h;
		m->_33 = -nfRatio;
		m->_43 = -nfRatio * zn;
		m->_44 = 1.0f;
		m->_12 = 0.0f; m->_13 = 0.0f; m->_14 = 0.0f;
		m->_21 = 0.0f; m->_23 = 0.0f; m->_24 = 0.0f;
		m->_31 = 0.0f; m->_32 = 0.0f; m->_34 = 0.0f;
		m->_41 = 0.0f; m->_42 = 0.0f;
		return m;
	}
	matrix4x4*	matrixRotationX(matrix4x4* m, float f)
	{CPUTime("matrix4x4");
		float s = (float)sin(f);	
		float c = (float)cos(f);	
		m->_11 = 1;		m->_12 = 0;		m->_13 = 0;		m->_14 = 0;
		m->_21 = 0;		m->_22 = c;		m->_23 = -s;	m->_24 = 0;	
		m->_31 = 0;		m->_32	= s;	m->_33	= c;	m->_34 = 0;
		m->_41	= 0;	m->_42 = 0;		m->_43 = 0;		m->_44 = 1;
		return m;
	}
	matrix4x4*	matrixRotationY(matrix4x4* m, float f)
	{CPUTime("matrix4x4");
		float s = (float)sin(f);	
		float c = (float)cos(f);	
		m->_11 = c;		m->_12 = 0;		m->_13 = s;		m->_14 = 0;
		m->_21 = 0;		m->_22 = 1;		m->_23 = 0;		m->_24 = 0;	
		m->_31 = -s;	m->_32	= 0;	m->_33	= c;	m->_34 = 0;
		m->_41	= 0;	m->_42 = 0;		m->_43 = 0;		m->_44 = 1;
		return m;
	}
	matrix4x4*	matrixRotationZ(matrix4x4* m, float f)
	{CPUTime("matrix4x4");
		float s = (float)sin(-f);
		float c = (float)cos(-f);
		m->_11 = c;		m->_12 = -s;	m->_13 = 0;		m->_14 = 0;
		m->_21 = s;		m->_22 = c;		m->_23 = 0;		m->_24 = 0;	
		m->_31 = 0;		m->_32	= 0;	m->_33	= 1;	m->_34 = 0;
		m->_41	= 0;	m->_42 = 0;		m->_43 = 0;		m->_44 = 1;
		return m;
	}

	matrix4x4::matrix4x4(){/*identity();*/}
	matrix4x4::matrix4x4( const float* pf )
	{
		memcpy(&_11, pf, sizeof(matrix4x4));
	}
	matrix4x4::matrix4x4( const matrix4x4& mat )
	{
		memcpy(&_11, &mat, sizeof(matrix4x4));
	}
	matrix4x4::matrix4x4( float f11, float f12, float f13, float f14,
		float f21, float f22, float f23, float f24,
		float f31, float f32, float f33, float f34,
		float f41, float f42, float f43, float f44 )
	{
		_11 = f11; _12 = f12; _13 = f13; _14 = f14;
		_21 = f21; _22 = f22; _23 = f23; _24 = f24;
		_31 = f31; _32 = f32; _33 = f33; _34 = f34;
		_41 = f41; _42 = f42; _43 = f43; _44 = f44;
	}
	// casting operators
	matrix4x4::operator float* ()
	{
		return (float *) &_11;
	}
	matrix4x4::operator const float* () const
	{
		return (const float *) &_11;
	}
	// assignment operators
	matrix4x4&	matrix4x4::operator *= ( const matrix4x4& mat )
	{
		matrixMultiply(this,this,&mat);
		return *this;
	}
	matrix4x4&	matrix4x4::operator += ( const matrix4x4& mat )
	{CPUTime("matrix4x4");
		_11 += mat._11; _12 += mat._12; _13 += mat._13; _14 += mat._14;
		_21 += mat._21; _22 += mat._22; _23 += mat._23; _24 += mat._24;
		_31 += mat._31; _32 += mat._32; _33 += mat._33; _34 += mat._34;
		_41 += mat._41; _42 += mat._42; _43 += mat._43; _44 += mat._44;
		return *this;
	}
	matrix4x4&	matrix4x4::operator -= ( const matrix4x4& mat )
	{CPUTime("matrix4x4");
		_11 -= mat._11; _12 -= mat._12; _13 -= mat._13; _14 -= mat._14;
		_21 -= mat._21; _22 -= mat._22; _23 -= mat._23; _24 -= mat._24;
		_31 -= mat._31; _32 -= mat._32; _33 -= mat._33; _34 -= mat._34;
		_41 -= mat._41; _42 -= mat._42; _43 -= mat._43; _44 -= mat._44;
		return *this;
	}
	matrix4x4&	matrix4x4::operator *= ( float f )
	{CPUTime("matrix4x4");
		_11 *= f; _12 *= f; _13 *= f; _14 *= f;
		_21 *= f; _22 *= f; _23 *= f; _24 *= f;
		_31 *= f; _32 *= f; _33 *= f; _34 *= f;
		_41 *= f; _42 *= f; _43 *= f; _44 *= f;
		return *this;
	}
	matrix4x4&	matrix4x4::operator /= ( float f )
	{CPUTime("matrix4x4");
		float fInv = 1.0f / f;
		_11 *= fInv; _12 *= fInv; _13 *= fInv; _14 *= fInv;
		_21 *= fInv; _22 *= fInv; _23 *= fInv; _24 *= fInv;
		_31 *= fInv; _32 *= fInv; _33 *= fInv; _34 *= fInv;
		_41 *= fInv; _42 *= fInv; _43 *= fInv; _44 *= fInv;
		return *this;
	}
	// unary operators
	matrix4x4	matrix4x4::operator + () const
	{
		return *this;
	}
	matrix4x4	matrix4x4::operator - () const
	{
		return matrix4x4(-_11, -_12, -_13, -_14,
			-_21, -_22, -_23, -_24,
			-_31, -_32, -_33, -_34,
			-_41, -_42, -_43, -_44);
	}
	// binary operators
	matrix4x4	matrix4x4::operator * ( const matrix4x4& mat ) const
	{
		matrix4x4 matT;
		matrixMultiply(&matT, this, &mat);
		return matT;
	}
	matrix4x4 			matrix4x4::operator / ( const matrix4x4& mat) const
	{
		matrix4x4 matT = mat;
		matT.Invert(0);
		matrixMultiply(&matT, this, &matT);
		return matT;
	}
	bool				matrix4x4::operator == ( const matrix4x4& mat) const
	{
		return (
			_11 == mat._11 && _12 == mat._12 && _13 == mat._13 && _14 == mat._14,
			_21 == mat._21 && _22 == mat._22 && _23 == mat._23 && _24 == mat._24,
			_31 == mat._31 && _32 == mat._32 && _33 == mat._33 && _34 == mat._34,
			_41 == mat._41 && _42 == mat._42 && _43 == mat._43 && _44 == mat._44);
	}

	matrix4x4	matrix4x4::operator + ( const matrix4x4& mat ) const
	{
		return matrix4x4(_11 + mat._11, _12 + mat._12, _13 + mat._13, _14 + mat._14,
			_21 + mat._21, _22 + mat._22, _23 + mat._23, _24 + mat._24,
			_31 + mat._31, _32 + mat._32, _33 + mat._33, _34 + mat._34,
			_41 + mat._41, _42 + mat._42, _43 + mat._43, _44 + mat._44);
	}
	matrix4x4	matrix4x4::operator - ( const matrix4x4& mat ) const
	{
		return matrix4x4(_11 - mat._11, _12 - mat._12, _13 - mat._13, _14 - mat._14,
			_21 - mat._21, _22 - mat._22, _23 - mat._23, _24 - mat._24,
			_31 - mat._31, _32 - mat._32, _33 - mat._33, _34 - mat._34,
			_41 - mat._41, _42 - mat._42, _43 - mat._43, _44 - mat._44);
	}
	matrix4x4	matrix4x4::operator * ( float f ) const
	{
		return matrix4x4(_11 * f, _12 * f, _13 * f, _14 * f,
			_21 * f, _22 * f, _23 * f, _24 * f,
			_31 * f, _32 * f, _33 * f, _34 * f,
			_41 * f, _42 * f, _43 * f, _44 * f);
	}
	matrix4x4	matrix4x4::operator / ( float f ) const
	{
		float fInv = 1.0f / f;
		return matrix4x4(_11 * fInv, _12 * fInv, _13 * fInv, _14 * fInv,
			_21 * fInv, _22 * fInv, _23 * fInv, _24 * fInv,
			_31 * fInv, _32 * fInv, _33 * fInv, _34 * fInv,
			_41 * fInv, _42 * fInv, _43 * fInv, _44 * fInv);
	}
	matrix4x4	operator * ( float f, const matrix4x4& mat )
	{
		return matrix4x4(f * mat._11, f * mat._12, f * mat._13, f * mat._14,
			f * mat._21, f * mat._22, f * mat._23, f * mat._24,
			f * mat._31, f * mat._32, f * mat._33, f * mat._34,
			f * mat._41, f * mat._42, f * mat._43, f * mat._44);
	}
	void	matrix4x4::identity()
	{
		m[0][1] = m[0][2] =m[0][3] =
			m[1][0] = m[1][2] =m[1][3] =
			m[2][0] = m[2][1] =m[2][3] =
			m[3][0] = m[3][1] =m[3][2] = 0.0f;

		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
	}

	bool	matrix4x4::isIdentity()
	{
		return m[0][0] == 1.0f && m[0][1] == 0.0f && m[0][2] == 0.0f && m[0][3] == 0.0f &&
			m[1][0] == 0.0f && m[1][1] == 1.0f && m[1][2] == 0.0f && m[1][3] == 0.0f &&
			m[2][0] == 0.0f && m[2][1] == 0.0f && m[2][2] == 1.0f && m[2][3] == 0.0f &&
			m[3][0] == 0.0f && m[3][1] == 0.0f && m[3][2] == 0.0f && m[3][3] == 1.0f;
	}

	void	matrix4x4::PerspectiveFovRH(float fovy, float Aspect, float zn, float zf)
	{
		matrixPerspectiveFovRH(this, fovy, Aspect, zn, zf);
	}
	void	matrix4x4::OrthoRH(float w,float h,float zn,float zf)
	{
		matrixOrthoRH(this, w, h, zn , zf);
	}

	void	matrix4x4::LookAtRH(const float3& eye,const float3& at,const float3& up)
	{
		matrixLookAtRH(this, &eye, &at, &up);
	}
	void				matrix4x4::RotationX(float f)
	{
		matrixRotationX(this, f);
	}
	void				matrix4x4::RotationY(float f)
	{
		matrixRotationY(this, f);
	}
	void				matrix4x4::RotationZ(float f)
	{
		matrixRotationZ(this, f);
	}

	void				matrix4x4::Invert( float * pDeterminant )
	{
		matrixInverse(this, pDeterminant, this);
	}
	float				getRotateZ(const float3& _dir)
	{
		float fr = sqrtf(_dir.y*_dir.y + _dir.x*_dir.x);
		if(fr<0.00001f)
			return 0.0f;
		return acosf(-_dir.y/fr)*(asinf(_dir.x/fr) > 0.0f?1.0f:-1.0f);
	}


	float3&	TransformRot(float3& dest, float3& v, matrix4x4& matWorld)
	{CPUTime("matrix4x4");
		float vx=v.x,vy=v.y,vz=v.z;
		dest.x=vx*matWorld._11+vy*matWorld._21+vz*matWorld._31;//+matWorld._41;
		dest.y=vx*matWorld._12+vy*matWorld._22+vz*matWorld._32;//+matWorld._42;
		dest.z=vx*matWorld._13+vy*matWorld._23+vz*matWorld._33;//+matWorld._43;
		return dest;
	}
	float			GetVec3Radius(float3& v){return sqrtf(v.x*v.x+v.y*v.y+v.z*v.z);}

	void	getRSP(quaternion& outQtn,float3& outScale,float3& outPosition,const matrix4x4& matIn_)
	{
		matrix4x4 matIn=(matrix4x4&)matIn_;
		matIn._41=matIn._42=matIn._43=0;
		//
		float3 x,y,z,nx,ny,nz,scale;
		x.x=x.y=x.z=0;
		y.x=y.y=y.z=0;
		z.x=z.y=z.z=0;
		x.x=y.y=z.z=1.0f;
		//

		TransformRot(nx,x,matIn);
		TransformRot(ny,y,matIn);
		TransformRot(nz,z,matIn);
		//
		scale.x=GetVec3Radius(nx);
		scale.y=GetVec3Radius(ny);
		scale.z=GetVec3Radius(nz);
		//
		matrix4x4 matScale,matRot,mm;
		matrixScaling(&mm,scale.x,scale.y,scale.z);
		matrixInverse(&matScale,0,&mm);
		matrixMultiply(&matRot,&matScale,&matIn);
		//
		//matRot=matIn;
		quaternion newQtn_;
		qtnRotationMatrix(&newQtn_,&matRot);
		//
		outQtn=(quaternion&)newQtn_;
		outScale=(float3&)scale;
		outPosition.x=matIn_._41;
		outPosition.y=matIn_._42;
		outPosition.z=matIn_._43;
	}

	void	getMatrixQuaternion(quaternion& _out,const matrix4x4& _matIn)
	{CPUTime("matrix4x4");
		matrix4x4 matIn=(matrix4x4&)_matIn;
		matIn._41=matIn._42=matIn._43=0;
		//
		float3 x,y,z,nx,ny,nz,scale;
		x.x=x.y=x.z=0;
		y.x=y.y=y.z=0;
		z.x=z.y=z.z=0;
		x.x=y.y=z.z=1.0f;
		//

		TransformRot(nx,x,matIn);
		TransformRot(ny,y,matIn);
		TransformRot(nz,z,matIn);
		//
		scale.x=GetVec3Radius(nx);
		scale.y=GetVec3Radius(ny);
		scale.z=GetVec3Radius(nz);
		//
		matrix4x4 matScale,matRot,mm;
		matrixScaling(&mm,scale.x,scale.y,scale.z);
		matrixInverse(&matScale,0,&mm);
		matrixMultiply(&matRot,&matScale,&matIn);
		//
		//matRot=matIn;
		quaternion newQtn_;
		qtnRotationMatrix(&newQtn_,&matRot);
		//
		_out=(quaternion&)newQtn_;
	}

	void	getMatrixScale(float3& outScale,const matrix4x4& _matIn)
	{CPUTime("matrix4x4");
		matrix4x4 matIn=(matrix4x4&)_matIn;
		matIn._41=matIn._42=matIn._43=0;
		//
		float3 x,y,z,nx,ny,nz,scale;
		x.x=x.y=x.z=0;
		y.x=y.y=y.z=0;
		z.x=z.y=z.z=0;
		x.x=y.y=z.z=1.0f;
		//
		TransformRot(nx,x,matIn);
		TransformRot(ny,y,matIn);
		TransformRot(nz,z,matIn);
		//
		scale.x=GetVec3Radius(nx);
		scale.y=GetVec3Radius(ny);
		scale.z=GetVec3Radius(nz);
		outScale = (float3&)scale;
	}

	matrix4x4*			matrixLerp(matrix4x4* pOut, const matrix4x4* pFrom, const matrix4x4* pTo, float fLerp)
	{CPUTime("matrix4x4");
		//matrix4x4 mat;
		float fFrom = (1.0f - fLerp);
		float fTo = fLerp;
		//
		float* pfOut = (*pOut);
		float* pfFrom = (float*)pFrom;
		float* pfTo = (float*)pTo;
		//
		for(int i=0;i<16;i++)
		{
			pfOut[i] = pfFrom[i]*fFrom + pfTo[i]*fLerp;
		}
		//
		return pOut;
	}

	struct static_matrix
	{
		matrix4x4		scale;
		matrix4x4		rot;
		//
		static_matrix(){scale.identity();}
	};
	matrix4x4*			matrixTransform(matrix4x4* bzOut, const float3* pos, const quaternion* rot, const float3* scale)	//转变,根据参数
	{CPUTime("matrix4x4");
		static static_matrix	s_mat;
		matrix4x4& matScale	= s_mat.scale;
		matrix4x4& matRot		= s_mat.rot;
		//
		if(rot)
		{
			matrixRotationQuaternion(&matRot,rot);
			if(!scale)
				*bzOut = matRot;
		}
		if(scale)
		{
			matScale._11 = scale->x;
			matScale._22 = scale->y;
			matScale._33 = scale->z;
			if(rot)
				matrixMultiply((matrix4x4*)bzOut,&matScale,&matRot);
			else
				*bzOut = matScale;
		}
		//
		if(pos)
		{
			bzOut->_41 = pos->x;
			bzOut->_42 = pos->y;
			bzOut->_43 = pos->z;
		}
		return bzOut;
	}

	BOOL			ObjectInLinear( IntersectData* intersect, const PSphereData& sphere , const matrix4x4* world)
	{CPUTime("matrix4x4");
		float3 s = sphere.center;
		if(world)
		{
			s.x += world->_41;
			s.y += world->_42;
			s.z += world->_43;
		}
		float fMaxDistance	=	intersect->GetPickInfo().fDist;
		float3 dd			=	intersect->GetOriginal() - sphere.center;
		if(dd.length() > (fMaxDistance + sphere.radius))
			return false;
		//
		if(intersect->GetDirection().isSpherePick( s, sphere.radius2x ))
			return true;
		return false;
	}

	BOOL			ObjectInLinear( IntersectData* intersect, const PObjectRegion& obj , const matrix4x4* world)
	{CPUTime("matrix4x4");
		float3 s = obj.sphere.center;
		if(world)
		{
			s.x += world->_41;
			s.y += world->_42;
			s.z += world->_43;
		}
		float fMaxDistance	=	intersect->GetPickInfo().fDist;
		float3 dd			=	intersect->GetOriginal() - obj.sphere.center;
		if(dd.length() > (fMaxDistance + obj.sphere.radius))
			return false;
		//
		if(intersect->GetDirection().isSpherePick( s, obj.sphere.radius2x ))
			return true;
		return false;
	}
	float3* vec3Transform( float3 *pDest, const float3 *pV, const matrix4x4 *pM )
	{
		float vx=pV->x,vy=pV->y,vz=pV->z;
		const matrix4x4& _m = *pM;
		pDest->x	=	vx	*	_m._11	+	vy	*	_m._21	+	vz	*	_m._31	+	_m._41;
		pDest->y	=	vx	*	_m._12	+	vy	*	_m._22	+	vz	*	_m._32	+	_m._42;
		pDest->z	=	vx	*	_m._13	+	vy	*	_m._23	+	vz	*	_m._33	+	_m._43;
		return pDest;
	}
	BOOL		IntersectTriangle(const Pixel& cursor, const Pixel& pv0, const Pixel& pv1, const Pixel& pv2)
	{CPUTime("matrix4x4");
		float3		vOrig(cursor.x, cursor.y, -1);
		float3		vDir(0, 0, 1);
		float3		v0(pv0.x,pv0.y, 0);
		float3		v1(pv1.x,pv1.y, 0);
		float3		v2(pv2.x,pv2.y, 0);
		float det, u, fv,t;
		//
		BOOL bNoPick = false;
		float3 edge1, edge2;
		{
			edge1	=	v1 - v0;
			edge2	=	v2 - v0;
			float3 pvec;
			vec3Cross(&pvec, &vDir, &edge2);
			det = vec3Dot(&edge1,&pvec);
			if( det < 0.0001f ){
				bNoPick = true;
			}
			else
			{
				//
				float3 tvec = vOrig-v0;
				u = vec3Dot(&tvec, &pvec);
				if( u < 0.0f || u > det ){
					bNoPick = true;
				}
				else
				{
					//
					float3 qvec;
					vec3Cross(&qvec, &tvec, &edge1);
					fv = vec3Dot(&vDir, &qvec);
					if( fv < 0.0f || u + fv > det ){
						bNoPick = true;
					}
					else
					{
						//
						t = vec3Dot(&edge2, &qvec)/det;
						if(/*t>oldt||*/t<0.0f)
						{
							bNoPick = true;
						}
					}
				}
			}
		}
		if(bNoPick)
			return false;
		return true;
	}
	BOOL			IntersectTriangle(PIntersectData* pOutPick, const float3& vOrig, const float3& vDir, const float3& v0, const float3& v1, const float3& v2, const matrix4x4* pWorld, bool bDoubleSide)
	{CPUTime("matrix4x4");
		const float3*	pPickRayOrig;
		const float3*	pPickRayDir;
		//
		float3		_vPickRayOrig;
		float3		_vPickRayDir;
		//
		if(pWorld)
		{
			matrix4x4		matInvert;
			float3 _to		= vDir + vOrig;
			//
			matrixInverse(&matInvert, 0, pWorld);
			//
			vec3Transform(&_vPickRayOrig, &vOrig, &matInvert);
			vec3Transform(&_to, &_to, &matInvert);
			//
			_vPickRayDir.x 	= _to.x - _vPickRayOrig.x;
			_vPickRayDir.y 	= _to.y - _vPickRayOrig.y;
			_vPickRayDir.z 	= _to.z - _vPickRayOrig.z;
			//
			pPickRayOrig	= &_vPickRayOrig;
			pPickRayDir		= &_vPickRayDir;
		}
		else
		{
			pPickRayOrig	= &vOrig;
			pPickRayDir		= &vDir;
		}
		float3 edge1	=	v1 - v0;
		float3 edge2	=	v2 - v0;
		float3 pvec;
		vec3Cross(&pvec, pPickRayDir, &edge2);
		float det = vec3Dot(&edge1,&pvec);
		if( det < 0.0001f && !bDoubleSide ) return false;
		//
		float3 tvec = (*pPickRayOrig)-v0;
		float u = vec3Dot(&tvec, &pvec);
		if( u < 0.0f || u > det ) return false;
		//
		float3 qvec;
		vec3Cross(&qvec, &tvec, &edge1);
		float fv = vec3Dot(pPickRayDir, &qvec);
		if( fv < 0.0f || u + fv > det ) return false;
		//
		float t = vec3Dot(&edge2, &qvec)/det;
		if(/*t>oldt||*/t<0.0f) return false;
		//
		if(pOutPick)
		{
			if(pOutPick->t)
			{
				if(*pOutPick->t < t)
					return false;
				*pOutPick->t = t;
			}
			if(pOutPick->Pick)
			{
				*pOutPick->Pick = vOrig + vDir*t;
			}
			if(pOutPick->normal)
			{
				*pOutPick->normal	=	edge1.cross(edge2);
			}
		}
	//	//
	//	float det, u, fv,t;
	//	//
	//	BOOL bNoPick = false;
	//	float3 edge1, edge2;
	//	{
	//		edge1	=	v1 - v0;
	//		edge2	=	v2 - v0;
	//		float3 pvec;
	//		vec3Cross(&pvec, pPickRayDir, &edge2);
	//		det = vec3Dot(&edge1,&pvec);
	//		if( det < 0.0001f ){
	//			bNoPick = true;
	//		}
	//		else
	//		{
	//			//
	//			float3 tvec = (*pPickRayOrig)-v0;
	//			u = vec3Dot(&tvec, &pvec);
	//			if( u < 0.0f || u > det ){
	//				bNoPick = true;
	//			}
	//			else
	//			{
	//				//
	//				float3 qvec;
	//				vec3Cross(&qvec, &tvec, &edge1);
	//				fv = vec3Dot(pPickRayDir, &qvec);
	//				if( fv < 0.0f || u + fv > det ){
	//					bNoPick = true;
	//				}
	//				else
	//				{
	//					//
	//					t = vec3Dot(&edge2, &qvec)/det;
	//					if(/*t>oldt||*/t<0.0f)
	//					{
	//						bNoPick = true;
	//					}
	//				}
	//			}
	//		}
	//	}
	//	if(bNoPick && !bDoubleSide)
	//		return false;
	//	if(bDoubleSide)
	//	{
	//		edge1	=	v1 - v2;
	//		edge2	=	v0 - v2;
	//		float3 pvec;
	//		vec3Cross(&pvec, pPickRayDir, &edge2);
	//		det = vec3Dot(&edge1,&pvec);
	//		if( det < 0.0001f ){
	//			bNoPick = true;
	//		}
	//		else
	//		{
	//			//
	//			float3 tvec = (*pPickRayOrig)-v2;
	//			u = vec3Dot(&tvec, &pvec);
	//			if( u < 0.0f || u > det ){
	//				bNoPick = true;
	//			}
	//			else
	//			{
	//				//
	//				float3 qvec;
	//				vec3Cross(&qvec, &tvec, &edge1);
	//				fv = vec3Dot(pPickRayDir, &qvec);
	//				if( fv < 0.0f || u + fv > det ){
	//					bNoPick = true;
	//				}
	//				else
	//				{
	//					//
	//					t = vec3Dot(&edge2, &qvec)/det;
	//					if(/*t>oldt||*/t<0.0f)
	//					{
	//						bNoPick = true;
	//					}
	//				}
	//			}
	//		}
	//	}
	//	if(bNoPick)
	//		return false;
	//	//
	//	if(pOutPick)
	//	{
	//		if(pOutPick->t)
	//		{
	//			if(*pOutPick->t < t)
	//				return false;
	//			*pOutPick->t = t;
	//		}
	//		if(pOutPick->Pick)
	//		{
	//			*pOutPick->Pick = vOrig + vDir*t;
	//		}
	//		if(pOutPick->normal)
	//		{
	//			*pOutPick->normal	=	edge1.cross(edge2);
	//		}
	//	}
		return true;
	}

	BOOL	IntersectTriangleList(PIntersectData* pOutPick, const float3& vOrig, const float3& vDir, const void* _verts, const void* pIndices, int numFace, const matrix4x4* pWorld, int strideSize, int cull)
	{CPUTime("matrix4x4");
		float3 vPickRayOrig;
		float3 vPickRayDir;
		//
		if(pWorld)
		{
			matrix4x4		matInvert;
			float3 _to		=  vDir + vOrig;
			matrixInverse(&matInvert, 0, pWorld);
			//
			vOrig.transform(vPickRayOrig, matInvert);
			_to.transform(_to, matInvert);
			//
			vPickRayDir.x = _to.x - vPickRayOrig.x;
			vPickRayDir.y = _to.y - vPickRayOrig.y;
			vPickRayDir.z = _to.z - vPickRayOrig.z;
		}
		else
		{
			vPickRayOrig = vOrig;
			vPickRayDir = vDir;
		}
		//
		char* v			= (char*)_verts;
		float oldt		= 100000000.0f;
		float fU,fV;
		int dwFaceID	= -1;
		//
		BOOL bInvert	=	false;
		Triangle* faces = (Triangle*)pIndices;
		//
		BOOL bB = (cull==Phantom::CullMode_CCW)||(cull==Phantom::CullMode_Double);
		BOOL bA = (cull==Phantom::CullMode_CW)||(cull==Phantom::CullMode_Double);
		float3 fedge1,fedge2,fv0;
		for(int i=0;i<numFace;i++)
		{
			if(bA)
			{
				float3& v0	=	*(float3*)(v + faces[i].v1*strideSize);
				float3& v1	=	*(float3*)(v + faces[i].v2*strideSize);
				float3& v2	=	*(float3*)(v + faces[i].v3*strideSize);
				float3 edge1	=	v1-v0;
				float3 edge2	=	v2-v0;
				float3 pvec;

				vec3Cross(&pvec, &vPickRayDir, &edge2);
				float det		=	vec3Dot(&edge1, &pvec);
				if( !(det < 0.0001f ))
				{
					float3 tvec	=	vPickRayOrig - v0;
					float u			=	vec3Dot(&tvec, &pvec);
					if(!( u < 0.0f || u > det ))
					{
						float3 qvec;
						vec3Cross(&qvec, &tvec, &edge1);
						float fv		=	vec3Dot(&vPickRayDir, &qvec);//vPickRayDir.dot(qvec);
						if(!( fv < 0.0f || u + fv > det ))
						{

							//
							float fInvDet	=	1.0f / det;
							float t	=	vec3Dot(&edge2, &qvec) * fInvDet;//edge2.dot(qvec)/det;
							if(!(t>oldt||t<0.0f))
							{
								//
								fU	=	u * fInvDet;
								fV	=	fv * fInvDet;
								oldt = t;
								bInvert	=	false;
								dwFaceID = i;
								fedge1 = edge1;
								fedge2 = edge2;
								fv0 = v0;
							}
						}
					}
				}
			}
			if(bB)
			{
				float3& v0	=	*(float3*)(v + faces[i].v1*strideSize);
				float3& v2	=	*(float3*)(v + faces[i].v2*strideSize);
				float3& v1	=	*(float3*)(v + faces[i].v3*strideSize);
				float3 edge1	=	v1-v0;
				float3 edge2	=	v2-v0;
				float3 pvec;

				vec3Cross(&pvec, &vPickRayDir, &edge2);
				float det		=	vec3Dot(&edge1, &pvec);
				if( det < 0.0001f )
					continue;
				//
				float3 tvec	=	vPickRayOrig - v0;
				float u			=	vec3Dot(&tvec, &pvec);
				if( u < 0.0f || u > det )continue;
				//
				float3 qvec;
				vec3Cross(&qvec, &tvec, &edge1);
				float fv		=	vec3Dot(&vPickRayDir, &qvec);//vPickRayDir.dot(qvec);
				if( fv < 0.0f || u + fv > det )continue;
				//
				float fInvDet	=	1.0f / det;
				float t	=	vec3Dot(&edge2, &qvec) * fInvDet;//edge2.dot(qvec)/det;
				if(t>oldt||t<0.0f)continue;
				//
				fU	=	u * fInvDet;
				fV	=	fv * fInvDet;
				oldt = t;
				bInvert	=	true;
				dwFaceID = i;
				fedge1 = edge1;
				fedge2 = edge2;
				fv0 = v0;
			}
		}
		//
		if(dwFaceID == -1)
			return false;
		if(pOutPick)
		{
			float3 Pick;
			if(pWorld)
			{
				Pick			=	vPickRayOrig + vPickRayDir * oldt;
				Pick.transform(Pick, *pWorld);
				oldt			=	(vOrig - Pick).R();
			}
			//
			if(pOutPick->t)
			{
				if(*pOutPick->t < oldt)
					return false;
				*pOutPick->t = oldt;
				*pOutPick->u = fU;
				*pOutPick->v = fV;
			}
			pOutPick->faceID	=	dwFaceID;
			if(pOutPick->normal)
			{
				fedge1.cross(*pOutPick->normal, fedge2);
				if(pWorld)
				{
					float3 v1 = fv0 + *pOutPick->normal;
					fv0.transform(fv0, *pWorld);
					v1.transform(v1, *pWorld);
					*pOutPick->normal = v1 - fv0;
					pOutPick->normal->normalize();
				}
			}
			if(pOutPick->Pick)
			{
				if(pWorld)
					*pOutPick->Pick	=	Pick;
				else
					*pOutPick->Pick = vOrig + vDir * oldt;
			}
		}
		return true;
	}

	float	LineCrossPoint(float3* pOutCross, const float3& orig, const float3& dir, const float3& point)
	{CPUTime("matrix4x4");
		float3 QP = point - orig;
		float t0 = dir.dot(QP);
		if(t0<0)//如果物体在线段后方则返回
		{
			return false;
		}
		float3 r2x(QP.x - t0*dir.x,QP.y - t0*dir.y,QP.z - t0*dir.z);// = QP - t0*ldir;
		float fRet = r2x.dot(r2x);
		if(pOutCross)
		{
			float ft = t0 / dir.dot(dir);
			*pOutCross = orig + dir*ft;
		}
		return fRet;
	}

	float				LineInPointY(float2* vOutCross, const float2& orig, const float2& dir, const float2& point)
	{CPUTime("matrix4x4");
		//向量方向是上下
		if(dir.x<0.00001f && dir.x>=-0.00001f)
		{
			float2 vP(orig.x, point.y);
			if(vOutCross)*vOutCross = vP;
			return 0;//sqrtf((orig.x - point.x)*(orig.x - point.x));
		}
		else if(dir.y<0.00001f && dir.y>=-0.00001f)
		{
			float2 vP(point.x, orig.y);
			if(vOutCross)*vOutCross = vP;
			return sqrtf((point.y - orig.y)*(point.y - orig.y));
		}
		float fD = dir.y / dir.x;
		float2 vP(point.x, fD * (point.x - orig.x) + orig.y);
		if(vOutCross)*vOutCross = vP;
		return (vP.y - point.y);
	}

	float				LineInPointX(float2* vOutCross, const float2& orig, const float2& dir, const float2& point)
	{CPUTime("matrix4x4");
		//向量方向是上下
		if(dir.x<0.00001f && dir.x>=-0.00001f)
		{
			float2 vP(orig.x, point.y);
			if(vOutCross)*vOutCross = vP;
			return sqrtf((orig.x - point.x)*(orig.x - point.x));
		}
		else if(dir.y<0.00001f && dir.y>=-0.00001f)
		{
			float2 vP(point.x, orig.y);
			if(vOutCross)*vOutCross = vP;
			return 0;
		}
		float fD = dir.x / dir.y;
		float2 vP(orig.x + fD * (point.y - orig.y), point.y);
		if(vOutCross)*vOutCross = vP;
		return (vP.x - point.x);
	}
	BOOL	IntersectBox(IntersectData* p, const float3& vMin, const float3& vMax, const matrix4x4* pWorld)
	{CPUTime("matrix4x4");
		//return intersectBox(p->GetOriginal(), p->GetDir(), vMin, vMax, pWorld, pPickRet);
		static PBoxVertices vert;
		vert.setup( vMin, vMax );

		PIntersectData d;
		float3 vPick;
		float u,v;
		d.Pick	= &vPick;
		d.u		= &u;
		d.v		= &v;
		float f = p->GetPickInfo().fDist;
		d.t = &f;
		static	unsigned short	s_aaboxIndices[36] = {4,0,3, 3,7,4, 1,5,6, 6,2,1, 1,0,4, 4,5,1, 3,2,6, 6,7,3, 0,1,2, 2,3,0, 6,5,4, 4,7,6};
		//
		if(IntersectTriangleList(&d, p->GetOriginal(), p->GetDir(), vert.v, s_aaboxIndices, 12, pWorld, sizeof(float3), Phantom::CullMode_Double))
		{
			return p->PushPickInfo(PickInfoData(f, vPick, u, v,  "box", (void*)0, 0));
		}
		return false;
	}

	BOOL	PickBoxLinear(const float2& orig, const float2& dir, const float2& vMin, const float2& vMax, float2* retPick)
	{CPUTime("matrix4x4");
		float2 pickRet[5];
		int pickCount = 0;
		if(dir.y <= 0.0001f && dir.y >= -0.0001f)
		{
			if(orig.x < vMin.x || orig.x > vMax.y)
				return false;
		}
		else
		{
			float dy = dir.x / dir.y;
			float x1 = (vMin.y - orig.y) * dy + orig.x;
			float x2 = (vMax.y - orig.y) * dy + orig.x;
			float fX1	=	x1 - vMin.x;
			float fX2	=	x2 - vMin.x;
			float fWidth	=	(vMax.x - vMin.x);
			if(fX1 >= 0.0f && fX1 <= fWidth)
				pickRet[pickCount++].setxy( x1, vMin.y );
			if(fX2 >= 0.0f && fX2 <= fWidth)
				pickRet[pickCount++].setxy( x2, vMax.y );
		}
		//
		if(dir.x <= 0.0001f && dir.x >= -0.0001f)
		{
			if(orig.y < vMin.y || orig.y > vMax.y)
				return false;
		}
		else if(pickCount < 2)
		{
			float dx = dir.y / dir.x;
			float y1 = (vMin.x - orig.x) * dx + orig.y;
			float y2 = (vMax.x - orig.x) * dx + orig.y;
			float fY1	=	y1 - vMin.y;
			float fY2	=	y2 - vMin.y;
			float fHeight	=	(vMax.y - vMin.y);
			if(fY1 >= 0.0f && fY1 <= fHeight)
				pickRet[pickCount++].setxy( vMin.x , y1 );
			if(fY2 >= 0.0f && fY2 <= fHeight)
				pickRet[pickCount++].setxy( vMax.x , y2 );
		}
		assert(pickCount <= 2);
		if(pickCount == 1)
			*retPick	=	pickRet[0];
		else
		{
			float x1	=	pickRet[0].x - orig.x;
			float x2	=	pickRet[1].x - orig.x;
			float y1	=	pickRet[0].y - orig.y;
			float y2	=	pickRet[1].y - orig.y;
			//
			float fR1	=	x1 * x1 + y1 * y1;
			float fR2	=	x2 * x2 + y2 * y2;
			if(fR1 < fR2)
				*retPick	=	pickRet[0];
			else
				*retPick	=	pickRet[1];
		}
		return true;
	}

	BOOL				IntersectBox(const float3& vOrig, const float3& vDir, const float3& vMin, const float3& vMax, const matrix4x4* pWorld)
	{CPUTime("matrix4x4");
		const float3*	pPickRayOrig;
		const float3*	pPickRayDir;
		//
		float3		_vPickRayOrig;
		float3		_vPickRayDir;
		//
		if(pWorld)
		{
			matrix4x4		matInvert;
			float3 _to		= vDir + vOrig;
			//
			matrixInverse(&matInvert, 0, pWorld);
			//
			vec3Transform(&_vPickRayOrig, &vOrig, &matInvert);
			vec3Transform(&_to, &_to, &matInvert);
			//
			_vPickRayDir.x 	= _to.x - _vPickRayOrig.x;
			_vPickRayDir.y 	= _to.y - _vPickRayOrig.y;
			_vPickRayDir.z 	= _to.z - _vPickRayOrig.z;
			//
			pPickRayOrig	= &_vPickRayOrig;
			pPickRayDir		= &_vPickRayDir;
		}
		else
		{
			pPickRayOrig	= &vOrig;
			pPickRayDir		= &vDir;
		}
		//
		float fHeight = vMax.y - vMin.y;
		float fWidth = vMax.x - vMin.x;
		float fLength = vMax.z - vMin.z;
		//先取得与两点最近的线条上的点
		{
			float2 orig2(pPickRayOrig->x, pPickRayOrig->y);
			float2 dir2(pPickRayDir->x, pPickRayDir->y);
			float2 min2(vMin.x, vMin.y);
			float2 max2(vMax.x, vMax.y);
			//
			{
				float fL1 = LineInPointY(0, orig2, dir2, min2);
				float fL2 = LineInPointY(0, orig2, dir2, max2);
				if(fL1 < 0 && fL2 < -fHeight || fL1>fHeight && fL2 > 0)
					return false;
			}
			{
				float fL1 = LineInPointX(0, orig2, dir2, min2);
				float fL2 = LineInPointX(0, orig2, dir2, max2);
				if(fL1 < 0 && fL2 < -fWidth || fL1>fWidth && fL2 > 0)
					return false;
			}
		}
		{
			float2 orig2(pPickRayOrig->x, pPickRayOrig->z);
			float2 dir2(pPickRayDir->x, pPickRayDir->z);
			float2 min2(vMin.x, vMin.z);
			float2 max2(vMax.x, vMax.z);
			//
			{
				float fL1 = LineInPointY(0, orig2, dir2, min2);
				float fL2 = LineInPointY(0, orig2, dir2, max2);
				if(fL1 < 0 && fL2 < -fLength || fL1>fLength && fL2 > 0)
					return false;
			}
			{
				float fL1 = LineInPointX(0, orig2, dir2, min2);
				float fL2 = LineInPointX(0, orig2, dir2, max2);
				if(fL1 < 0 && fL2 < -fWidth || fL1>fWidth && fL2 > 0)
					return false;
			}
		}
		{
			float2 orig2(pPickRayOrig->z, pPickRayOrig->y);
			float2 dir2(pPickRayDir->z, pPickRayDir->y);
			float2 min2(vMin.z, vMin.y);
			float2 max2(vMax.z, vMax.y);
			//
			{
				float fL1 = LineInPointY(0, orig2, dir2, min2);
				float fL2 = LineInPointY(0, orig2, dir2, max2);
				if(fL1 < 0 && fL2 < -fHeight || fL1>fHeight && fL2 > 0)
					return false;
			}
			{
				float fL1 = LineInPointX(0, orig2, dir2, min2);
				float fL2 = LineInPointX(0, orig2, dir2, max2);
				if(fL1 < 0 && fL2 < -fLength || fL1>fLength && fL2 > 0)
					return false;
			}
		}
		return true;
	}
	//
	BOOL	IsSphereInAABox(const AABox& parent, const PSphereData& child)
	{CPUTime("matrix4x4");
		if(parent._min.x > (child.center.x + child.radius))
			return false;
		if(parent._min.y > (child.center.y + child.radius))
			return false;
		if(parent._min.z > (child.center.z + child.radius))
			return false;

		if(parent._max.x < (child.center.x - child.radius))
			return false;
		if(parent._max.y < (child.center.y - child.radius))
			return false;
		if(parent._max.z < (child.center.z - child.radius))
			return false;
		return true;
	}
	//

	BOOL	IsPointInAABox(const AABox& parent, const float3& child)
	{CPUTime("matrix4x4");
		if(parent._min.x > child.x)
			return false;
		if(parent._min.y > child.y)
			return false;
		if(parent._min.z > child.z)
			return false;

		if(parent._max.x < child.x)
			return false;
		if(parent._max.y < child.y)
			return false;
		if(parent._max.z < child.z)
			return false;
		return true;
	}
	BOOL	IsFullClipAABox(const AABox& parent, const AABox& child)
	{
		if(parent._min.x <= child._min.x && parent._min.y <= child._min.y && parent._min.z <= child._min.z
			&& parent._max.x >= child._max.x && parent._max.y >= child._max.y && parent._max.z >= child._max.z)
			return true;
		return false;
	}

	BOOL	IsAABoxInAABox(const AABox& parent, const AABox& child)
	{CPUTime("matrix4x4");
		if(parent._min.x >= child._max.x)
			return false;
		if(parent._min.y >= child._max.y)
			return false;
		if(parent._min.z >= child._max.z)
			return false;

		if(parent._max.x < child._min.x)
			return false;
		if(parent._max.y < child._min.y)
			return false;
		if(parent._max.z < child._min.z)
			return false;
		return true;
	}
	//float2
	float2*	vec2BaryCentric(float2 *pOut, const float2 *pV1, const float2 *pV2, const float2 *pV3, float f, float g)
	{
		pOut->x = (1.0f-f-g) * (pV1->x) + f * (pV2->x) + g * (pV3->x);
		pOut->y = (1.0f-f-g) * (pV1->y) + f * (pV2->y) + g * (pV3->y);
		return pOut;
	}
	float2*  vec2CatmullRom(float2 *pOut, const float2 *pV0, const float2 *pV1, const float2 *pV2, const float2 *pV3, float s)
	{
		pOut->x = 0.5f * (2.0f * pV1->x + (pV2->x - pV0->x) *s + (2.0f *pV0->x - 5.0f * pV1->x + 4.0f * pV2->x - pV3->x) * s * s + (pV3->x -3.0f * pV2->x + 3.0f * pV1->x - pV0->x) * s * s * s);
		pOut->y = 0.5f * (2.0f * pV1->y + (pV2->y - pV0->y) *s + (2.0f *pV0->y - 5.0f * pV1->y + 4.0f * pV2->y - pV3->y) * s * s + (pV3->y -3.0f * pV2->y + 3.0f * pV1->y - pV0->y) * s * s * s);
		return pOut;
	}
	float2*  vec2Hermite(float2 *pOut, const float2 *pV1, const float2 *pT1, const float2 *pV2, const float2 *pT2, float s)
	{
		float h1, h2, h3, h4;
		h1 = 2.0f * s * s * s - 3.0f * s * s + 1.0f;
		h2 = s * s * s - 2.0f * s * s + s;
		h3 = -2.0f * s * s * s + 3.0f * s * s;
		h4 = s * s * s - s * s;
		pOut->x = h1 * (pV1->x) + h2 * (pT1->x) + h3 * (pV2->x) + h4 * (pT2->x);
		pOut->y = h1 * (pV1->y) + h2 * (pT1->y) + h3 * (pV2->y) + h4 * (pT2->y);
		return pOut;
	}
	float2*  vec2Normalize(float2 *pOut, const float2 *pV)
	{
		float2 o;
		float norm;
		norm = vec2Length(pV);
		if ( !norm )
		{
			o.x = 0.0f;
			o.y = 0.0f;
		}
		else
		{
			o.x = pV->x / norm;
			o.y = pV->y / norm;
		}
		*pOut=o;
		return pOut;
	}
	float4*  vec2Transform(float4 *pOut, const float2 *pV, const matrix4x4 *pMat)
	{
		pOut->x = pMat->m[0][0] * pV->x + pMat->m[1][0] * pV->y  + pMat->m[3][0];
		pOut->y = pMat->m[0][1] * pV->x + pMat->m[1][1] * pV->y  + pMat->m[3][1];
		pOut->z = pMat->m[0][2] * pV->x + pMat->m[1][2] * pV->y  + pMat->m[3][2];
		pOut->w = pMat->m[0][3] * pV->x + pMat->m[1][3] * pV->y  + pMat->m[3][3];
		return pOut;
	}
	float4*  vec2TransformArray(float4* pOut, unsigned int outstride, const float2* pIn, unsigned int instride, const matrix4x4* pMatrix, unsigned int elements)
	{
		unsigned int i;

		for (i = 0; i < elements; ++i)
		{
			vec2Transform((float4*)((char*)pOut + outstride * i),(const float2*)((const char*)pIn + instride * i),pMatrix);
		}
		return pOut;
	}

	float2*  vec2TransformCoord(float2 *pOut, const float2 *pV, const matrix4x4 *pMat)
	{
		float2 v;
		float norm;
		v = *pV;
		norm = pMat->m[0][3] * pV->x + pMat->m[1][3] * pV->y + pMat->m[3][3];
		pOut->x = (pMat->m[0][0] * v.x + pMat->m[1][0] * v.y + pMat->m[3][0]) / norm;
		pOut->y = (pMat->m[0][1] * v.x + pMat->m[1][1] * v.y + pMat->m[3][1]) / norm;
		return pOut;
	}

	float2*  vec2TransformCoordArray(float2* pOut, unsigned int outstride, const float2* pIn, unsigned int instride, const matrix4x4* pMatrix, unsigned int elements)
	{
		for(unsigned int i = 0; i < elements; ++i)
		{
			vec2TransformCoord((float2*)((char*)pOut + outstride * i),(const float2*)((const char*)pIn + instride * i),pMatrix);
		}
		return pOut;
	}

	float2*  vec2TransformNormal(float2 *pOut, const float2 *pV, const matrix4x4 *pMat)
	{
		const float2 v = *pV;
		pOut->x = pMat->m[0][0] * v.x + pMat->m[1][0] * v.y;
		pOut->y = pMat->m[0][1] * v.x + pMat->m[1][1] * v.y;
		return pOut;
	}

	float2*  vec2TransformNormalArray(float2* pOut, unsigned int outstride, const float2 *pIn, unsigned int instride, const matrix4x4 *pMatrix, unsigned int elements)
	{
		for(unsigned int i = 0; i < elements; ++i)
		{
			vec2TransformNormal((float2*)((char*)pOut + outstride * i),(const float2*)((const char*)pIn + instride * i),pMatrix);
		}
		return pOut;
	}
	//float3

	float3*  vec3BaryCentric(float3 *pOut, const float3 *pV1, const float3 *pV2, const float3 *pV3, float f, float g)
	{
		pOut->x = (1.0f-f-g) * (pV1->x) + f * (pV2->x) + g * (pV3->x);
		pOut->y = (1.0f-f-g) * (pV1->y) + f * (pV2->y) + g * (pV3->y);
		pOut->z = (1.0f-f-g) * (pV1->z) + f * (pV2->z) + g * (pV3->z);
		return pOut;
	}

	float3*  vec3CatmullRom( float3 *pOut, const float3 *pV0, const float3 *pV1, const float3 *pV2, const float3 *pV3, float s)
	{
		pOut->x = 0.5f * (2.0f * pV1->x + (pV2->x - pV0->x) *s + (2.0f *pV0->x - 5.0f * pV1->x + 4.0f * pV2->x - pV3->x) * s * s + (pV3->x -3.0f * pV2->x + 3.0f * pV1->x - pV0->x) * s * s * s);
		pOut->y = 0.5f * (2.0f * pV1->y + (pV2->y - pV0->y) *s + (2.0f *pV0->y - 5.0f * pV1->y + 4.0f * pV2->y - pV3->y) * s * s + (pV3->y -3.0f * pV2->y + 3.0f * pV1->y - pV0->y) * s * s * s);
		pOut->z = 0.5f * (2.0f * pV1->z + (pV2->z - pV0->z) *s + (2.0f *pV0->z - 5.0f * pV1->z + 4.0f * pV2->z - pV3->z) * s * s + (pV3->z -3.0f * pV2->z + 3.0f * pV1->z - pV0->z) * s * s * s);
		return pOut;
	}

	float3*  vec3Hermite(float3 *pOut, const float3 *pV1, const float3 *pT1, const float3 *pV2, const float3 *pT2, float s)
	{
		float h1, h2, h3, h4;

		h1 = 2.0f * s * s * s - 3.0f * s * s + 1.0f;
		h2 = s * s * s - 2.0f * s * s + s;
		h3 = -2.0f * s * s * s + 3.0f * s * s;
		h4 = s * s * s - s * s;

		pOut->x = h1 * (pV1->x) + h2 * (pT1->x) + h3 * (pV2->x) + h4 * (pT2->x);
		pOut->y = h1 * (pV1->y) + h2 * (pT1->y) + h3 * (pV2->y) + h4 * (pT2->y);
		pOut->z = h1 * (pV1->z) + h2 * (pT1->z) + h3 * (pV2->z) + h4 * (pT2->z);
		return pOut;
	}

	float3*  vec3Normalize(float3 *pOut, const float3 *pV)
	{
		float3 o;
		float norm;

		norm = pV->length();
		if ( !norm )
		{
			o.x = 0.0f;
			o.y = 0.0f;
			o.z = 0.0f;
		}
		else
		{
			o.x = pV->x / norm;
			o.y = pV->y / norm;
			o.z = pV->z / norm;
		}
		*pOut = o;
		return pOut;
	}

	float4*  vec3Transform(float4 *pOut, const float3 *pV, const matrix4x4 *pMat)
	{
		pOut->x = pMat->m[0][0] * pV->x + pMat->m[1][0] * pV->y + pMat->m[2][0] * pV->z + pMat->m[3][0];
		pOut->y = pMat->m[0][1] * pV->x + pMat->m[1][1] * pV->y + pMat->m[2][1] * pV->z + pMat->m[3][1];
		pOut->z = pMat->m[0][2] * pV->x + pMat->m[1][2] * pV->y + pMat->m[2][2] * pV->z + pMat->m[3][2];
		pOut->w = pMat->m[0][3] * pV->x + pMat->m[1][3] * pV->y + pMat->m[2][3] * pV->z + pMat->m[3][3];
		return pOut;
	}

	float4*  vec3TransformArray(float4* pOut, unsigned int outstride, const float3* pIn, unsigned int instride, const matrix4x4* pMatrix, unsigned int elements)
	{
		unsigned int i;

		for (i = 0; i < elements; ++i) {
			vec3Transform(
				(float4*)((char*)pOut + outstride * i),
				(const float3*)((const char*)pIn + instride * i),
				pMatrix);
		}
		return pOut;
	}

	float3*  vec3TransformCoord(float3 *pOut, const float3 *pV, const matrix4x4 *pMat)
	{
		float3 o;
		float norm;

		norm = pMat->m[0][3] * pV->x + pMat->m[1][3] * pV->y + pMat->m[2][3] *pV->z + pMat->m[3][3];

		o.x = (pMat->m[0][0] * pV->x + pMat->m[1][0] * pV->y + pMat->m[2][0] * pV->z + pMat->m[3][0]) / norm;
		o.y = (pMat->m[0][1] * pV->x + pMat->m[1][1] * pV->y + pMat->m[2][1] * pV->z + pMat->m[3][1]) / norm;
		o.z = (pMat->m[0][2] * pV->x + pMat->m[1][2] * pV->y + pMat->m[2][2] * pV->z + pMat->m[3][2]) / norm;

		*pOut = o;

		return pOut;
	}

	float3*  vec3TransformCoordArray(float3* pOut, unsigned int outstride, const float3* pIn, unsigned int instride, const matrix4x4* pMatrix, unsigned int elements)
	{
		unsigned int i;

		for (i = 0; i < elements; ++i) {
			vec3TransformCoord(
				(float3*)((char*)pOut + outstride * i),
				(const float3*)((const char*)pIn + instride * i),
				pMatrix);
		}
		return pOut;
	}

	float3*  vec3TransformNormal(float3 *pOut, const float3 *pV, const matrix4x4 *pMat)
	{
		const float3 v = *pV;
		pOut->x = pMat->m[0][0] * v.x + pMat->m[1][0] * v.y + pMat->m[2][0] * v.z;
		pOut->y = pMat->m[0][1] * v.x + pMat->m[1][1] * v.y + pMat->m[2][1] * v.z;
		pOut->z = pMat->m[0][2] * v.x + pMat->m[1][2] * v.y + pMat->m[2][2] * v.z;
		return pOut;

	}

	float3*  vec3TransformNormalArray(float3* pOut, unsigned int outstride, const float3* pIn, unsigned int instride, const matrix4x4* pMatrix, unsigned int elements)
	{
		unsigned int i;

		for (i = 0; i < elements; ++i) {
			vec3TransformNormal(
				(float3*)((char*)pOut + outstride * i),
				(const float3*)((const char*)pIn + instride * i),
				pMatrix);
		}
		return pOut;
	}


	float4*  vec4BaryCentric(float4 *pOut, const float4 *pV1, const float4 *pV2, const float4 *pV3, float f, float g)
	{
		pOut->x = (1.0f-f-g) * (pV1->x) + f * (pV2->x) + g * (pV3->x);
		pOut->y = (1.0f-f-g) * (pV1->y) + f * (pV2->y) + g * (pV3->y);
		pOut->z = (1.0f-f-g) * (pV1->z) + f * (pV2->z) + g * (pV3->z);
		pOut->w = (1.0f-f-g) * (pV1->w) + f * (pV2->w) + g * (pV3->w);
		return pOut;
	}

	float4*  vec4CatmullRom(float4 *pOut, const float4 *pV0, const float4 *pV1, const float4 *pV2, const float4 *pV3, float s)
	{
		pOut->x = 0.5f * (2.0f * pV1->x + (pV2->x - pV0->x) *s + (2.0f *pV0->x - 5.0f * pV1->x + 4.0f * pV2->x - pV3->x) * s * s + (pV3->x -3.0f * pV2->x + 3.0f * pV1->x - pV0->x) * s * s * s);
		pOut->y = 0.5f * (2.0f * pV1->y + (pV2->y - pV0->y) *s + (2.0f *pV0->y - 5.0f * pV1->y + 4.0f * pV2->y - pV3->y) * s * s + (pV3->y -3.0f * pV2->y + 3.0f * pV1->y - pV0->y) * s * s * s);
		pOut->z = 0.5f * (2.0f * pV1->z + (pV2->z - pV0->z) *s + (2.0f *pV0->z - 5.0f * pV1->z + 4.0f * pV2->z - pV3->z) * s * s + (pV3->z -3.0f * pV2->z + 3.0f * pV1->z - pV0->z) * s * s * s);
		pOut->w = 0.5f * (2.0f * pV1->w + (pV2->w - pV0->w) *s + (2.0f *pV0->w - 5.0f * pV1->w + 4.0f * pV2->w - pV3->w) * s * s + (pV3->w -3.0f * pV2->w + 3.0f * pV1->w - pV0->w) * s * s * s);
		return pOut;
	}

	float4*  vec4Cross(float4 *pOut, const float4 *pV1, const float4 *pV2, const float4 *pV3)
	{
		float4 o;
		o.x = pV1->y * (pV2->z * pV3->w - pV3->z * pV2->w) - pV1->z * (pV2->y * pV3->w - pV3->y * pV2->w) + pV1->w * (pV2->y * pV3->z - pV2->z *pV3->y);
		o.y = -(pV1->x * (pV2->z * pV3->w - pV3->z * pV2->w) - pV1->z * (pV2->x * pV3->w - pV3->x * pV2->w) + pV1->w * (pV2->x * pV3->z - pV3->x * pV2->z));
		o.z = pV1->x * (pV2->y * pV3->w - pV3->y * pV2->w) - pV1->y * (pV2->x *pV3->w - pV3->x * pV2->w) + pV1->w * (pV2->x * pV3->y - pV3->x * pV2->y);
		o.w = -(pV1->x * (pV2->y * pV3->z - pV3->y * pV2->z) - pV1->y * (pV2->x * pV3->z - pV3->x *pV2->z) + pV1->z * (pV2->x * pV3->y - pV3->x * pV2->y));
		*pOut = o;
		return pOut;
	}

	float4*  vec4Hermite(float4 *pOut, const float4 *pV1, const float4 *pT1, const float4 *pV2, const float4 *pT2, float s)
	{
		float h1, h2, h3, h4;

		h1 = 2.0f * s * s * s - 3.0f * s * s + 1.0f;
		h2 = s * s * s - 2.0f * s * s + s;
		h3 = -2.0f * s * s * s + 3.0f * s * s;
		h4 = s * s * s - s * s;

		pOut->x = h1 * (pV1->x) + h2 * (pT1->x) + h3 * (pV2->x) + h4 * (pT2->x);
		pOut->y = h1 * (pV1->y) + h2 * (pT1->y) + h3 * (pV2->y) + h4 * (pT2->y);
		pOut->z = h1 * (pV1->z) + h2 * (pT1->z) + h3 * (pV2->z) + h4 * (pT2->z);
		pOut->w = h1 * (pV1->w) + h2 * (pT1->w) + h3 * (pV2->w) + h4 * (pT2->w);
		return pOut;
	}

	float4*  vec4Normalize(float4 *pOut, const float4 *pV)
	{
		float4 o;
		float norm;

		norm = pV->length();

		o.x = pV->x / norm;
		o.y = pV->y / norm;
		o.z = pV->z / norm;
		o.w = pV->w / norm;

		*pOut = o;
		return pOut;
	}

	float4*  vec4Transform(float4 *pOut, const float4 *pV, const matrix4x4 *pMat)
	{
		float4 o;
		o.x = pMat->m[0][0] * pV->x + pMat->m[1][0] * pV->y + pMat->m[2][0] * pV->z + pMat->m[3][0] * pV->w;
		o.y = pMat->m[0][1] * pV->x + pMat->m[1][1] * pV->y + pMat->m[2][1] * pV->z + pMat->m[3][1] * pV->w;
		o.z = pMat->m[0][2] * pV->x + pMat->m[1][2] * pV->y + pMat->m[2][2] * pV->z + pMat->m[3][2] * pV->w;
		o.w = pMat->m[0][3] * pV->x + pMat->m[1][3] * pV->y + pMat->m[2][3] * pV->z + pMat->m[3][3] * pV->w;
		*pOut = o;
		return pOut;
	}

	float4*  vec4TransformArray(float4* pOut, unsigned int outstride, const float4* pIn, unsigned int instride, const matrix4x4* pMatrix, unsigned int elements)
	{
		unsigned int i;

		for (i = 0; i < elements; ++i)
		{
			vec4Transform((float4*)((char*)pOut + outstride * i),(const float4*)((const char*)pIn + instride * i),pMatrix);
		}
		return pOut;
	}


	quaternion*  qtnBaryCentric(quaternion *pOut, const quaternion *pQ1, const quaternion *pQ2, const quaternion *pQ3, float f, float g)
	{
		quaternion temp1, temp2;
		qtnSlerp(pOut, qtnSlerp(&temp1, pQ1, pQ2, f + g), qtnSlerp(&temp2, pQ1, pQ3, f+g), g / (f + g));
		return pOut;
	}

	quaternion*  qtnExp(quaternion *pOut, const quaternion *pQtn)
	{
		float norm;

		norm = sqrt(pQtn->x * pQtn->x + pQtn->y * pQtn->y + pQtn->z * pQtn->z);
		if (norm )
		{
			pOut->x = sin(norm) * pQtn->x / norm;
			pOut->y = sin(norm) * pQtn->y / norm;
			pOut->z = sin(norm) * pQtn->z / norm;
			pOut->w = cos(norm);
		}
		else
		{
			pOut->x = 0.0f;
			pOut->y = 0.0f;
			pOut->z = 0.0f;
			pOut->w = 1.0f;
		}
		return pOut;
	}

	quaternion*  qtnInverse(quaternion *pOut, const quaternion *pQtn)
	{
		quaternion o;
		float norm;

		norm = pQtn->lengthSq();

		o.x = -pQtn->x / norm;
		o.y = -pQtn->y / norm;
		o.z = -pQtn->z / norm;
		o.w = pQtn->w / norm;

		*pOut =o;
		return pOut;
	}

	quaternion*  qtnLn(quaternion *pOut, const quaternion *pQtn)
	{
		float norm, normvec, theta;

		norm = pQtn->lengthSq();
		if ( norm > 1.0001f )
		{
			pOut->x = pQtn->x;
			pOut->y = pQtn->y;
			pOut->z = pQtn->z;
			pOut->w = 0.0f;
		}
		else if( norm > 0.99999f)
		{
			normvec = sqrt( pQtn->x * pQtn->x + pQtn->y * pQtn->y + pQtn->z * pQtn->z );
			theta = atan2(normvec, pQtn->w) / normvec;
			pOut->x = theta * pQtn->x;
			pOut->y = theta * pQtn->y;
			pOut->z = theta * pQtn->z;
			pOut->w = 0.0f;
		}
		else
			memset(pOut, 0, sizeof(quaternion));
		return pOut;
	}

	quaternion*  qtnMultiply(quaternion *pOut, const quaternion *pQ1, const quaternion *pQ2)
	{
		quaternion o;
		o.x = pQ2->w * pQ1->x + pQ2->x * pQ1->w + pQ2->y * pQ1->z - pQ2->z * pQ1->y;
		o.y = pQ2->w * pQ1->y - pQ2->x * pQ1->z + pQ2->y * pQ1->w + pQ2->z * pQ1->x;
		o.z = pQ2->w * pQ1->z + pQ2->x * pQ1->y - pQ2->y * pQ1->x + pQ2->z * pQ1->w;
		o.w = pQ2->w * pQ1->w - pQ2->x * pQ1->x - pQ2->y * pQ1->y - pQ2->z * pQ1->z;
		*pOut = o;
		return pOut;
	}

	quaternion*  qtnNormalize(quaternion *pOut, const quaternion *pQtn)
	{
		quaternion o;
		float norm;

		norm = pQtn->length();

		o.x = pQtn->x / norm;
		o.y = pQtn->y / norm;
		o.z = pQtn->z / norm;
		o.w = pQtn->w / norm;

		*pOut=o;

		return pOut;
	}

	quaternion*  qtnRotationAxis(quaternion *pOut, const float3 *pV, float angle)
	{
		float3 temp;

		vec3Normalize(&temp, pV);
		pOut->x = sin( angle / 2.0f ) * temp.x;
		pOut->y = sin( angle / 2.0f ) * temp.y;
		pOut->z = sin( angle / 2.0f ) * temp.z;
		pOut->w = cos( angle / 2.0f );
		return pOut;
	}

	quaternion*  qtnRotationMatrix(quaternion *pOut, const matrix4x4 *pMat)
	{
		int i, maxi;
		float maxdiag, S, trace;

		trace = pMat->m[0][0] + pMat->m[1][1] + pMat->m[2][2] + 1.0f;
		if ( trace > 1.0f)
		{
			pOut->x = ( pMat->m[1][2] - pMat->m[2][1] ) / ( 2.0f * sqrt(trace) );
			pOut->y = ( pMat->m[2][0] - pMat->m[0][2] ) / ( 2.0f * sqrt(trace) );
			pOut->z = ( pMat->m[0][1] - pMat->m[1][0] ) / ( 2.0f * sqrt(trace) );
			pOut->w = sqrt(trace) / 2.0f;
			return pOut;
		}
		maxi = 0;
		maxdiag = pMat->m[0][0];
		for (i=1; i<3; i++)
		{
			if ( pMat->m[i][i] > maxdiag )
			{
				maxi = i;
				maxdiag = pMat->m[i][i];
			}
		}
		switch( maxi )
		{
		case 0:
			S = 2.0f * sqrt(1.0f + pMat->m[0][0] - pMat->m[1][1] - pMat->m[2][2]);
			pOut->x = 0.25f * S;
			pOut->y = ( pMat->m[0][1] + pMat->m[1][0] ) / S;
			pOut->z = ( pMat->m[0][2] + pMat->m[2][0] ) / S;
			pOut->w = ( pMat->m[1][2] - pMat->m[2][1] ) / S;
			break;
		case 1:
			S = 2.0f * sqrt(1.0f + pMat->m[1][1] - pMat->m[0][0] - pMat->m[2][2]);
			pOut->x = ( pMat->m[0][1] + pMat->m[1][0] ) / S;
			pOut->y = 0.25f * S;
			pOut->z = ( pMat->m[1][2] + pMat->m[2][1] ) / S;
			pOut->w = ( pMat->m[2][0] - pMat->m[0][2] ) / S;
			break;
		case 2:
			S = 2.0f * sqrt(1.0f + pMat->m[2][2] - pMat->m[0][0] - pMat->m[1][1]);
			pOut->x = ( pMat->m[0][2] + pMat->m[2][0] ) / S;
			pOut->y = ( pMat->m[1][2] + pMat->m[2][1] ) / S;
			pOut->z = 0.25f * S;
			pOut->w = ( pMat->m[0][1] - pMat->m[1][0] ) / S;
			break;
		}
		return pOut;
	}

	quaternion*  qtnRotationYawPitchRoll(quaternion *pOut, float fYaw, float fPitch, float fRoll)
	{
		pOut->x = sin( fYaw / 2.0f) * cos(fPitch / 2.0f) * sin(fRoll / 2.0f) + cos(fYaw / 2.0f) * sin(fPitch / 2.0f) * cos(fRoll / 2.0f);
		pOut->y = sin( fYaw / 2.0f) * cos(fPitch / 2.0f) * cos(fRoll / 2.0f) - cos(fYaw / 2.0f) * sin(fPitch / 2.0f) * sin(fRoll / 2.0f);
		pOut->z = cos(fYaw / 2.0f) * cos(fPitch / 2.0f) * sin(fRoll / 2.0f) - sin( fYaw / 2.0f) * sin(fPitch / 2.0f) * cos(fRoll / 2.0f);
		pOut->w = cos( fYaw / 2.0f) * cos(fPitch / 2.0f) * cos(fRoll / 2.0f) + sin(fYaw / 2.0f) * sin(fPitch / 2.0f) * sin(fRoll / 2.0f);
		return pOut;
	}

	quaternion*  qtnSlerp(quaternion *pOut, const quaternion *pQ1, const quaternion *pQ2, float t)
	{
		float dot, epsilon, temp, theta, u;

		epsilon = 1.0f;
		temp = 1.0f - t;
		u = t;
		dot = pQ1->dot(*pQ2);
		if ( dot < 0.0f )
		{
			epsilon = -1.0f;
			dot = -dot;
		}
		if( 1.0f - dot > 0.001f )
		{
			theta = acos(dot);
			temp  = sin(theta * temp) / sin(theta);
			u = sin(theta * u) / sin(theta);
		}
		pOut->x = temp * pQ1->x + epsilon * u * pQ2->x;
		pOut->y = temp * pQ1->y + epsilon * u * pQ2->y;
		pOut->z = temp * pQ1->z + epsilon * u * pQ2->z;
		pOut->w = temp * pQ1->w + epsilon * u * pQ2->w;
		return pOut;
	}

	quaternion*  qtnSquad(quaternion *pOut, const quaternion *pQ1, const quaternion *pQ2, const quaternion *pQ3, const quaternion *pQ4, float t)
	{
		quaternion temp1, temp2;

		qtnSlerp(pOut, qtnSlerp(&temp1, pQ1, pQ4, t), qtnSlerp(&temp2, pQ2, pQ3, t), 2.0f * t * (1.0f - t));
		return pOut;
	}

	void  qtnToAxisAngle(const quaternion *pQtn, float3 *paxis, float *pangle)
	{
		paxis->x = pQtn->x;
		paxis->y = pQtn->y;
		paxis->z = pQtn->z;
		*pangle = 2.0f * acos(pQtn->w);
	}

	Plane*  planeFromPoints(Plane *pOut, const float3 *pV1, const float3 *pV2, const float3 *pV3)
	{
		float3 edge1, edge2, normal, np;

		edge1.x = 0.0f; edge1.y = 0.0f; edge1.z = 0.0f;
		edge2.x = 0.0f; edge2.y = 0.0f; edge2.z = 0.0f;
		vec3Subtract(&edge1, pV2, pV1);
		vec3Subtract(&edge2, pV3, pV1);
		vec3Cross(&normal, &edge1, &edge2);
		vec3Normalize(&np, &normal);
		planeFromPointNormal(pOut, pV1, &np);
		return pOut;
	}

	float3*  planeIntersectLine(float3 *pOut, const Plane *pp, const float3 *pV1, const float3 *pV2)
	{
		float3 direction, normal;
		float dot, temp;

		normal.x = pp->a;
		normal.y = pp->b;
		normal.z = pp->c;
		direction.x = pV2->x - pV1->x;
		direction.y = pV2->y - pV1->y;
		direction.z = pV2->z - pV1->z;
		dot = vec3Dot(&normal, &direction);
		if ( !dot ) return 0;
		temp = ( pp->d + vec3Dot(&normal, pV1) ) / dot;
		pOut->x = pV1->x - temp * direction.x;
		pOut->y = pV1->y - temp * direction.y;
		pOut->z = pV1->z - temp * direction.z;
		return pOut;
	}

	Plane*  planeNormalize(Plane *pOut, const Plane *pp)
	{
		Plane o;
		float norm;

		norm = sqrt(pp->a * pp->a + pp->b * pp->b + pp->c * pp->c);
		if ( norm )
		{
			o.a = pp->a / norm;
			o.b = pp->b / norm;
			o.c = pp->c / norm;
			o.d = pp->d / norm;
		}
		else
		{
			o.a = 0.0f;
			o.b = 0.0f;
			o.c = 0.0f;
			o.d = 0.0f;
		}
		*pOut = o;
		return pOut;
	}

	Plane*  planeTransform(Plane *pOut, const Plane *pplane, const matrix4x4 *pMat)
	{
		const Plane plane = *pplane;
		pOut->a = pMat->m[0][0] * plane.a + pMat->m[1][0] * plane.b + pMat->m[2][0] * plane.c + pMat->m[3][0] * plane.d;
		pOut->b = pMat->m[0][1] * plane.a + pMat->m[1][1] * plane.b + pMat->m[2][1] * plane.c + pMat->m[3][1] * plane.d;
		pOut->c = pMat->m[0][2] * plane.a + pMat->m[1][2] * plane.b + pMat->m[2][2] * plane.c + pMat->m[3][2] * plane.d;
		pOut->d = pMat->m[0][3] * plane.a + pMat->m[1][3] * plane.b + pMat->m[2][3] * plane.c + pMat->m[3][3] * plane.d;
		return pOut;
	}

	Plane*  planeTransformArray(Plane* pOut, unsigned int outstride, const Plane* pIn, unsigned int instride, const matrix4x4* pMatrix, unsigned int elements)
	{
		unsigned int i;

		for (i = 0; i < elements; ++i) {
			planeTransform(
				(Plane*)((char*)pOut + outstride * i),
				(const Plane*)((const char*)pIn + instride * i),
				pMatrix);
		}
		return pOut;
	}




	matrix4x4*  matrixAffineTransformation(matrix4x4 *pOut, float scaling, const float3 *rotationcenter, const quaternion *rotation, const float3 *translation)
	{
		matrix4x4 m1, m2, m3, m4, m5;

		matrixScaling(&m1, scaling, scaling, scaling);

		if ( !rotationcenter )
		{
			matrixIdentity(&m2);
			matrixIdentity(&m4);
		}
		else
		{
			matrixTranslation(&m2, -rotationcenter->x, -rotationcenter->y, -rotationcenter->z);
			matrixTranslation(&m4, rotationcenter->x, rotationcenter->y, rotationcenter->z);
		}

		if ( !rotation ) matrixIdentity(&m3);
		else matrixRotationQuaternion(&m3, rotation);

		if ( !translation ) matrixIdentity(&m5);
		else matrixTranslation(&m5, translation->x, translation->y, translation->z);

		matrixMultiply(&m1, &m1, &m2);
		matrixMultiply(&m1, &m1, &m3);
		matrixMultiply(&m1, &m1, &m4);
		matrixMultiply(pOut, &m1, &m5);
		return pOut;
	}

	matrix4x4*  matrixAffineTransformation2D(matrix4x4 *pOut, float scaling, const float2 *protationcenter, float rotation, const float2 *ptranslation)
	{
		matrix4x4 m1, m2, m3, m4, m5;
		quaternion rot;
		float3 rot_center, trans;

		rot.w=cos(rotation/2.0f);
		rot.x=0.0f;
		rot.y=0.0f;
		rot.z=sin(rotation/2.0f);

		if ( protationcenter )
		{
			rot_center.x=protationcenter->x;
			rot_center.y=protationcenter->y;
			rot_center.z=0.0f;
		}
		else
		{
			rot_center.x=0.0f;
			rot_center.y=0.0f;
			rot_center.z=0.0f;
		}

		if ( ptranslation )
		{
			trans.x=ptranslation->x;
			trans.y=ptranslation->y;
			trans.z=0.0f;
		}
		else
		{
			trans.x=0.0f;
			trans.y=0.0f;
			trans.z=0.0f;
		}

		matrixScaling(&m1, scaling, scaling, 1.0f);
		matrixTranslation(&m2, -rot_center.x, -rot_center.y, -rot_center.z);
		matrixTranslation(&m4, rot_center.x, rot_center.y, rot_center.z);
		matrixRotationQuaternion(&m3, &rot);
		matrixTranslation(&m5, trans.x, trans.y, trans.z);

		matrixMultiply(&m1, &m1, &m2);
		matrixMultiply(&m1, &m1, &m3);
		matrixMultiply(&m1, &m1, &m4);
		matrixMultiply(pOut, &m1, &m5);

		return pOut;
	}

	BOOL  matrixDecompose(float3 *poutscale, quaternion *poutrotation, float3 *pouttranslation, const matrix4x4 *pMat)
	{
		matrix4x4 normalized;
		float3 vec;

		/*Compute the scaling part.*/
		vec.x=pMat->m[0][0];
		vec.y=pMat->m[0][1];
		vec.z=pMat->m[0][2];
		poutscale->x=vec3Length(&vec);

		vec.x=pMat->m[1][0];
		vec.y=pMat->m[1][1];
		vec.z=pMat->m[1][2];
		poutscale->y=vec3Length(&vec);

		vec.x=pMat->m[2][0];
		vec.y=pMat->m[2][1];
		vec.z=pMat->m[2][2];
		poutscale->z=vec3Length(&vec);

		/*Compute the translation part.*/
		pouttranslation->x=pMat->m[3][0];
		pouttranslation->y=pMat->m[3][1];
		pouttranslation->z=pMat->m[3][2];

		/*Let's calculate the rotation now*/
		if ( (poutscale->x == 0.0f) || (poutscale->y == 0.0f) || (poutscale->z == 0.0f) ) return false;

		normalized.m[0][0]=pMat->m[0][0]/poutscale->x;
		normalized.m[0][1]=pMat->m[0][1]/poutscale->x;
		normalized.m[0][2]=pMat->m[0][2]/poutscale->x;
		normalized.m[1][0]=pMat->m[1][0]/poutscale->y;
		normalized.m[1][1]=pMat->m[1][1]/poutscale->y;
		normalized.m[1][2]=pMat->m[1][2]/poutscale->y;
		normalized.m[2][0]=pMat->m[2][0]/poutscale->z;
		normalized.m[2][1]=pMat->m[2][1]/poutscale->z;
		normalized.m[2][2]=pMat->m[2][2]/poutscale->z;

		qtnRotationMatrix(poutrotation,&normalized);
		return true;
	}

	matrix4x4*  matrixLookAtLH(matrix4x4 *pOut, const float3 *peye, const float3 *pat, const float3 *pup)
	{CPUTime("matrix4x4");
		float3 right, rightn, up, upn, vec, vec2;

		vec3Subtract(&vec2, pat, peye);
		vec3Normalize(&vec, &vec2);
		vec3Cross(&right, pup, &vec);
		vec3Cross(&up, &vec, &right);
		vec3Normalize(&rightn, &right);
		vec3Normalize(&upn, &up);
		pOut->m[0][0] = rightn.x;
		pOut->m[1][0] = rightn.y;
		pOut->m[2][0] = rightn.z;
		pOut->m[3][0] = -vec3Dot(&rightn,peye);
		pOut->m[0][1] = upn.x;
		pOut->m[1][1] = upn.y;
		pOut->m[2][1] = upn.z;
		pOut->m[3][1] = -vec3Dot(&upn, peye);
		pOut->m[0][2] = vec.x;
		pOut->m[1][2] = vec.y;
		pOut->m[2][2] = vec.z;
		pOut->m[3][2] = -vec3Dot(&vec, peye);
		pOut->m[0][3] = 0.0f;
		pOut->m[1][3] = 0.0f;
		pOut->m[2][3] = 0.0f;
		pOut->m[3][3] = 1.0f;
		return pOut;
	}

	matrix4x4*  matrixMultiplyTranspose(matrix4x4 *pOut, const matrix4x4 *pm1, const matrix4x4 *pm2)
	{
		matrixMultiply(pOut, pm1, pm2);
		matrixTranspose(pOut, pOut);
		return pOut;
	}

	matrix4x4*  matrixOrthoLH(matrix4x4 *pOut, float w, float h, float zn, float zf)
	{CPUTime("matrix4x4");
		matrixIdentity(pOut);
		pOut->m[0][0] = 2.0f / w;
		pOut->m[1][1] = 2.0f / h;
		pOut->m[2][2] = 1.0f / (zf - zn);
		pOut->m[3][2] = zn / (zn - zf);
		return pOut;
	}

	matrix4x4*  matrixOrthoOffCenterLH(matrix4x4 *pOut, float l, float r, float b, float t, float zn, float zf)
	{
		matrixIdentity(pOut);
		pOut->m[0][0] = 2.0f / (r - l);
		pOut->m[1][1] = 2.0f / (t - b);
		pOut->m[2][2] = 1.0f / (zf -zn);
		pOut->m[3][0] = -1.0f -2.0f *l / (r - l);
		pOut->m[3][1] = 1.0f + 2.0f * t / (b - t);
		pOut->m[3][2] = zn / (zn -zf);
		return pOut;
	}

	matrix4x4*  matrixOrthoOffCenterRH(matrix4x4 *pOut, float l, float r, float b, float t, float zn, float zf)
	{
		matrixIdentity(pOut);
		pOut->m[0][0] = 2.0f / (r - l);
		pOut->m[1][1] = 2.0f / (t - b);
		pOut->m[2][2] = 1.0f / (zn -zf);
		pOut->m[3][0] = -1.0f -2.0f *l / (r - l);
		pOut->m[3][1] = 1.0f + 2.0f * t / (b - t);
		pOut->m[3][2] = zn / (zn -zf);
		return pOut;
	}

	matrix4x4*  matrixPerspectiveFovLH(matrix4x4 *pOut, float fovy, float aspect, float zn, float zf)
	{
		matrixIdentity(pOut);
		pOut->m[0][0] = 1.0f / (aspect * tan(fovy/2.0f));
		pOut->m[1][1] = 1.0f / tan(fovy/2.0f);
		pOut->m[2][2] = zf / (zf - zn);
		pOut->m[2][3] = 1.0f;
		pOut->m[3][2] = (zf * zn) / (zn - zf);
		pOut->m[3][3] = 0.0f;
		return pOut;
	}

	matrix4x4*  matrixPerspectiveLH(matrix4x4 *pOut, float w, float h, float zn, float zf)
	{
		matrixIdentity(pOut);
		pOut->m[0][0] = 2.0f * zn / w;
		pOut->m[1][1] = 2.0f * zn / h;
		pOut->m[2][2] = zf / (zf - zn);
		pOut->m[3][2] = (zn * zf) / (zn - zf);
		pOut->m[2][3] = 1.0f;
		pOut->m[3][3] = 0.0f;
		return pOut;
	}

	matrix4x4*  matrixPerspectiveOffCenterLH(matrix4x4 *pOut, float l, float r, float b, float t, float zn, float zf)
	{
		matrixIdentity(pOut);
		pOut->m[0][0] = 2.0f * zn / (r - l);
		pOut->m[1][1] = -2.0f * zn / (b - t);
		pOut->m[2][0] = -1.0f - 2.0f * l / (r - l);
		pOut->m[2][1] = 1.0f + 2.0f * t / (b - t);
		pOut->m[2][2] = - zf / (zn - zf);
		pOut->m[3][2] = (zn * zf) / (zn -zf);
		pOut->m[2][3] = 1.0f;
		pOut->m[3][3] = 0.0f;
		return pOut;
	}

	matrix4x4*  matrixPerspectiveOffCenterRH(matrix4x4 *pOut, float l, float r, float b, float t, float zn, float zf)
	{
		matrixIdentity(pOut);
		pOut->m[0][0] = 2.0f * zn / (r - l);
		pOut->m[1][1] = -2.0f * zn / (b - t);
		pOut->m[2][0] = 1.0f + 2.0f * l / (r - l);
		pOut->m[2][1] = -1.0f -2.0f * t / (b - t);
		pOut->m[2][2] = zf / (zn - zf);
		pOut->m[3][2] = (zn * zf) / (zn -zf);
		pOut->m[2][3] = -1.0f;
		pOut->m[3][3] = 0.0f;
		return pOut;
	}

	matrix4x4*  matrixPerspectiveRH(matrix4x4 *pOut, float w, float h, float zn, float zf)
	{
		matrixIdentity(pOut);
		pOut->m[0][0] = 2.0f * zn / w;
		pOut->m[1][1] = 2.0f * zn / h;
		pOut->m[2][2] = zf / (zn - zf);
		pOut->m[3][2] = (zn * zf) / (zn - zf);
		pOut->m[2][3] = -1.0f;
		pOut->m[3][3] = 0.0f;
		return pOut;
	}

	matrix4x4*  matrixReflect(matrix4x4 *pOut, const Plane *pplane)
	{
		Plane np;

		planeNormalize(&np, pplane);
		matrixIdentity(pOut);
		pOut->m[0][0] = 1.0f - 2.0f * np.a * np.a;
		pOut->m[0][1] = -2.0f * np.a * np.b;
		pOut->m[0][2] = -2.0f * np.a * np.c;
		pOut->m[1][0] = -2.0f * np.a * np.b;
		pOut->m[1][1] = 1.0f - 2.0f * np.b * np.b;
		pOut->m[1][2] = -2.0f * np.b * np.c;
		pOut->m[2][0] = -2.0f * np.c * np.a;
		pOut->m[2][1] = -2.0f * np.c * np.b;
		pOut->m[2][2] = 1.0f - 2.0f * np.c * np.c;
		pOut->m[3][0] = -2.0f * np.d * np.a;
		pOut->m[3][1] = -2.0f * np.d * np.b;
		pOut->m[3][2] = -2.0f * np.d * np.c;
		return pOut;
	}

	matrix4x4*  matrixRotationAxis(matrix4x4 *pOut, const float3 *pV, float angle)
	{
		float3 v;

		vec3Normalize(&v,pV);
		matrixIdentity(pOut);
		pOut->m[0][0] = (1.0f - cos(angle)) * v.x * v.x + cos(angle);
		pOut->m[1][0] = (1.0f - cos(angle)) * v.x * v.y - sin(angle) * v.z;
		pOut->m[2][0] = (1.0f - cos(angle)) * v.x * v.z + sin(angle) * v.y;
		pOut->m[0][1] = (1.0f - cos(angle)) * v.y * v.x + sin(angle) * v.z;
		pOut->m[1][1] = (1.0f - cos(angle)) * v.y * v.y + cos(angle);
		pOut->m[2][1] = (1.0f - cos(angle)) * v.y * v.z - sin(angle) * v.x;
		pOut->m[0][2] = (1.0f - cos(angle)) * v.z * v.x - sin(angle) * v.y;
		pOut->m[1][2] = (1.0f - cos(angle)) * v.z * v.y + sin(angle) * v.x;
		pOut->m[2][2] = (1.0f - cos(angle)) * v.z * v.z + cos(angle);
		return pOut;
	}

	matrix4x4*  matrixRotationQuaternion(matrix4x4 *pOut, const quaternion *pq)
	{
		matrixIdentity(pOut);
		pOut->m[0][0] = 1.0f - 2.0f * (pq->y * pq->y + pq->z * pq->z);
		pOut->m[0][1] = 2.0f * (pq->x *pq->y + pq->z * pq->w);
		pOut->m[0][2] = 2.0f * (pq->x * pq->z - pq->y * pq->w);
		pOut->m[1][0] = 2.0f * (pq->x * pq->y - pq->z * pq->w);
		pOut->m[1][1] = 1.0f - 2.0f * (pq->x * pq->x + pq->z * pq->z);
		pOut->m[1][2] = 2.0f * (pq->y *pq->z + pq->x *pq->w);
		pOut->m[2][0] = 2.0f * (pq->x * pq->z + pq->y * pq->w);
		pOut->m[2][1] = 2.0f * (pq->y *pq->z - pq->x *pq->w);
		pOut->m[2][2] = 1.0f - 2.0f * (pq->x * pq->x + pq->y * pq->y);
		return pOut;
	}
	
	matrix4x4*  matrixRotationYawPitchRoll(matrix4x4 *pOut, float fYaw, float fPitch, float fRoll)
	{
		matrix4x4 m;

		matrixIdentity(pOut);
		matrixRotationZ(&m, fRoll);
		matrixMultiply(pOut, pOut, &m);
		matrixRotationX(&m, fPitch);
		matrixMultiply(pOut, pOut, &m);
		matrixRotationY(&m, fYaw);
		matrixMultiply(pOut, pOut, &m);
		return pOut;
	}
	matrix4x4*  matrixShadow(matrix4x4 *pOut, const float4 *plight, const Plane *pplane)
	{
		Plane np;
		float dot;

		planeNormalize(&np, pplane);
		dot = planeDot(&np, plight);
		pOut->m[0][0] = dot - np.a * plight->x;
		pOut->m[0][1] = -np.a * plight->y;
		pOut->m[0][2] = -np.a * plight->z;
		pOut->m[0][3] = -np.a * plight->w;
		pOut->m[1][0] = -np.b * plight->x;
		pOut->m[1][1] = dot - np.b * plight->y;
		pOut->m[1][2] = -np.b * plight->z;
		pOut->m[1][3] = -np.b * plight->w;
		pOut->m[2][0] = -np.c * plight->x;
		pOut->m[2][1] = -np.c * plight->y;
		pOut->m[2][2] = dot - np.c * plight->z;
		pOut->m[2][3] = -np.c * plight->w;
		pOut->m[3][0] = -np.d * plight->x;
		pOut->m[3][1] = -np.d * plight->y;
		pOut->m[3][2] = -np.d * plight->z;
		pOut->m[3][3] = dot - np.d * plight->w;
		return pOut;
	}

	matrix4x4*  matrixTransformation(matrix4x4 *pOut, const float3 *pscalingcenter, const quaternion *pscalingrotation, const float3 *pscaling, const float3 *protationcenter, const quaternion *protation, const float3 *ptranslation)
	{
		matrix4x4 m1, m2, m3, m4, m5, m6, m7;
		quaternion prc;
		float3 psc, pt;

		if ( !pscalingcenter )
		{
			psc.x = 0.0f;
			psc.y = 0.0f;
			psc.z = 0.0f;
		}
		else
		{
			psc.x = pscalingcenter->x;
			psc.y = pscalingcenter->y;
			psc.z = pscalingcenter->z;
		}

		if ( !protationcenter )
		{
			prc.x = 0.0f;
			prc.y = 0.0f;
			prc.z = 0.0f;
		}
		else
		{
			prc.x = protationcenter->x;
			prc.y = protationcenter->y;
			prc.z = protationcenter->z;
		}

		if ( !ptranslation )
		{
			pt.x = 0.0f;
			pt.y = 0.0f;
			pt.z = 0.0f;
		}
		else
		{
			pt.x = ptranslation->x;
			pt.y = ptranslation->y;
			pt.z = ptranslation->z;
		}

		matrixTranslation(&m1, -psc.x, -psc.y, -psc.z);

		if ( !pscalingrotation )
		{
			matrixIdentity(&m2);
			matrixIdentity(&m4);
		}
		else
		{
			matrixRotationQuaternion(&m4, pscalingrotation);
			matrixInverse(&m2, 0, &m4);
		}

		if ( !pscaling )
			matrixIdentity(&m3);
		else
			matrixScaling(&m3, pscaling->x, pscaling->y, pscaling->z);

		if ( !protation )
			matrixIdentity(&m6);
		else
			matrixRotationQuaternion(&m6, protation);

		matrixTranslation(&m5, psc.x - prc.x,  psc.y - prc.y,  psc.z - prc.z);
		matrixTranslation(&m7, prc.x + pt.x, prc.y + pt.y, prc.z + pt.z);
		matrixMultiply(&m1, &m1, &m2);
		matrixMultiply(&m1, &m1, &m3);
		matrixMultiply(&m1, &m1, &m4);
		matrixMultiply(&m1, &m1, &m5);
		matrixMultiply(&m1, &m1, &m6);
		matrixMultiply(pOut, &m1, &m7);
		return pOut;
	}
	matrix4x4*  matrixTransformation2D(matrix4x4 *pOut, const float2 *pscalingcenter, float scalingrotation, const float2 *pscaling, const float2 *protationcenter, float rotation, const float2 *ptranslation)
	{
		quaternion rot, sca_rot;
		float3 rot_center, sca, sca_center, trans;

		if ( pscalingcenter )
		{
			sca_center.x=pscalingcenter->x;
			sca_center.y=pscalingcenter->y;
			sca_center.z=0.0f;
		}
		else
		{
			sca_center.x=0.0f;
			sca_center.y=0.0f;
			sca_center.z=0.0f;
		}

		if ( pscaling )
		{
			sca.x=pscaling->x;
			sca.y=pscaling->y;
			sca.z=1.0f;
		}
		else
		{
			sca.x=1.0f;
			sca.y=1.0f;
			sca.z=1.0f;
		}

		if ( protationcenter )
		{
			rot_center.x=protationcenter->x;
			rot_center.y=protationcenter->y;
			rot_center.z=0.0f;
		}
		else
		{
			rot_center.x=0.0f;
			rot_center.y=0.0f;
			rot_center.z=0.0f;
		}

		if ( ptranslation )
		{
			trans.x=ptranslation->x;
			trans.y=ptranslation->y;
			trans.z=0.0f;
		}
		else
		{
			trans.x=0.0f;
			trans.y=0.0f;
			trans.z=0.0f;
		}

		rot.w=cos(rotation/2.0f);
		rot.x=0.0f;
		rot.y=0.0f;
		rot.z=sin(rotation/2.0f);

		sca_rot.w=cos(scalingrotation/2.0f);
		sca_rot.x=0.0f;
		sca_rot.y=0.0f;
		sca_rot.z=sin(scalingrotation/2.0f);

		matrixTransformation(pOut, &sca_center, &sca_rot, &sca, &rot_center, &rot, &trans);

		return pOut;
	}

	matrix4x4*  matrixTranslation(matrix4x4 *pOut, float x, float y, float z)
	{
		matrixIdentity(pOut);
		pOut->m[3][0] = x;
		pOut->m[3][1] = y;
		pOut->m[3][2] = z;
		return pOut;
	}

	matrix4x4*  matrixTranspose(matrix4x4 *pOut, const matrix4x4 *pMat)
	{
		const matrix4x4 m = *pMat;
		int i,j;

		for (i=0; i<4; i++)
			for (j=0; j<4; j++) pOut->m[i][j] = m.m[j][i];

		return pOut;
	}
	//将弧度限制范围在 -bz_pi ~ bz_pi 之间
	float					clampAngle(float fAngle)
	{
		fAngle	=	fmodf(fAngle, PI * 2.0f);
		if(fAngle <= PI && fAngle >= -PI)
			return fAngle;
		if(fAngle > PI)
			fAngle	=	fAngle - PI * 2.0f;
		else if(fAngle < -PI)
			fAngle	=	fAngle + PI * 2.0f;
		return fAngle;
	}
};
