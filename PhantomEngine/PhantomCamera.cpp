//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomCamera.h"
#include "PhantomManager.h"
#include "PhantomMath.h"
#include "LuaManager.h"

#define	ISCameraEnable(name)		m_cameraData.IsEnable(CameraEnable_##name)
#define	SetCameraEnable(name, b)	m_cameraData.SetEnable(CameraEnable_##name, b)

namespace Phantom{

	OrientationEKF::OrientationEKF(){
		m_dUpdateTime	=	0;
		memset(this, 0, sizeof(OrientationEKF));
		gyroFilterValid = true;
		reset();
	}
	void OrientationEKF::reset()
	{
		sensorTimeStampGyro = 0L;
		sensorTimeStampAcc = 0L;
		sensorTimeStampMag = 0L;
		so3SensorFromWorld.identity();
		so3LastMotion.identity();
		double initialSigmaP = 5.0;
		mP.clear();
		mP.setval(25.0);

		double initialSigmaQ = 1.0;
		mQ.clear();
		mQ.setval(1.0);
		double initialSigmaR = 0.25;
		mR.clear();
		mR.setval(0.0625);

		double initialSigmaRaccel = 0.75;
		mRaccel.clear();
		mRaccel.setval(0.5625);

		mS.clear();
		mH.clear();
		mK.clear();
		mNu.clear();
		mz.clear();
		mh.clear();
		mu.clear();
		mx.clear();

		down = double3(0.0, 0.0, 9.810000000000001);
		north = double3(0.0, 1.0, 0.0);
	}
	void OrientationEKF::sO3FromTwoVec(const double3& a, const double3& b, matrix3x3d& result){
		double3 temp31;
		double3 sO3FromTwoVecN;
		double3 sO3FromTwoVecA;
		double3 sO3FromTwoVecB;
		double3 sO3FromTwoVecRotationAxis;
		matrix3x3d sO3FromTwoVec33R1;
		matrix3x3d sO3FromTwoVec33R2;
		sO3FromTwoVecN = a.cross(b);
		if (sO3FromTwoVecN.length() == 0.0)
		{
			double dot = a.dot(b);
			if (dot >= 0.0) {
				result.identity();
			} else {
				sO3FromTwoVecRotationAxis = a.ortho();
				rotationPiAboutAxis(sO3FromTwoVecRotationAxis, result);
			}
			return;
		}

		sO3FromTwoVecA = a;
		sO3FromTwoVecB = b;
		sO3FromTwoVecN.normalize();
		sO3FromTwoVecA.normalize();
		sO3FromTwoVecB.normalize();
		sO3FromTwoVec33R1.setcol(0, sO3FromTwoVecA);
		sO3FromTwoVec33R1.setcol(1, sO3FromTwoVecN);
		temp31 = sO3FromTwoVecN.cross(sO3FromTwoVecA);
		sO3FromTwoVec33R1.setcol(2, temp31);

		sO3FromTwoVec33R2.setcol(0, sO3FromTwoVecB);
		sO3FromTwoVec33R2.setcol(1, sO3FromTwoVecN);
		temp31 = sO3FromTwoVecN.cross(sO3FromTwoVecB);
		sO3FromTwoVec33R2.setcol(2, temp31);
		sO3FromTwoVec33R1.transpose();
		result = sO3FromTwoVec33R2 * sO3FromTwoVec33R1;
	}
	void OrientationEKF::rotationPiAboutAxis(const double3& v, matrix3x3d& result){
		double3 rotationPiAboutAxisTemp;
		rotationPiAboutAxisTemp = v;
		rotationPiAboutAxisTemp *= (3.141592653589793 / rotationPiAboutAxisTemp.length());
		double invTheta = 0.3183098861837907;
		double kA = 0.0;
		double kB = 0.2026423672846756;
		rodriguesSo3Exp(rotationPiAboutAxisTemp, kA, kB, result);
	}
	void OrientationEKF::sO3FromMu(const double3& w, matrix3x3d& result) const
	{
		//LogVec3("s03w", w);
		double thetaSq = w.dot(w);
		double theta = sqrt(thetaSq);
		double kA, kB;
		if (thetaSq < 1.0E-08) {
			kA = 1.0 - 0.16666667163372 * thetaSq;
			kB = 0.5;
		}
		else
		{
			if (thetaSq < 1.0E-06) {
				kB = 0.5 - 0.0416666679084301 * thetaSq;
				kA = 1.0 - thetaSq * 0.16666667163372 * (1.0 - 0.16666667163372 * thetaSq);
			}
			else
			{
				double invTheta = 1.0 / theta;
				kA = sin(theta) * invTheta;
				kB = (1.0 - cos(theta)) * (invTheta * invTheta);
			}
		}
		rodriguesSo3Exp(w, kA, kB, result);
	}
	void OrientationEKF::muFromSO3(const matrix3x3d& so3, double3& result)
	{
		double cosAngle = (so3.mm[0][0] + so3.mm[1][1] + so3.mm[2][2] - 1.0) * 0.5;
		result = double3( (so3.mm[2][1] - so3.mm[1][2]) / 2.0, (so3.mm[0][2] - so3.mm[2][0]) / 2.0, (so3.mm[1][0] - so3.mm[0][1]) / 2.0);
		double sinAngleAbs = result.length();
		if (cosAngle > 0.7071067811865476)
		{
			if (sinAngleAbs > 0.0)
				result *= asin(sinAngleAbs) / sinAngleAbs;
		}
		else if (cosAngle > -0.7071067811865476)
		{
			double angle = acos(cosAngle);
			result *= angle / sinAngleAbs;
		}
		else
		{
			double angle = 3.141592653589793 - asin(sinAngleAbs);
			double d0 = so3.mm[0][0] - cosAngle;
			double d1 = so3.mm[1][1] - cosAngle;
			double d2 = so3.mm[2][2] - cosAngle;
			double3 r2;
			if ((d0 * d0 > d1 * d1) && (d0 * d0 > d2 * d2))
			{
				r2 = double3(d0, (so3.mm[1][0] + so3.mm[0][1]) / 2.0, (so3.mm[0][2] + so3.mm[2][0]) / 2.0);
			}
			else if (d1 * d1 > d2 * d2)
			{
				r2 = double3((so3.mm[1][0] + so3.mm[0][1]) / 2.0, d1, (so3.mm[2][1] + so3.mm[1][2]) / 2.0);
			}
			else
			{
				r2 = double3((so3.mm[0][2] + so3.mm[2][0]) / 2.0, (so3.mm[2][1] + so3.mm[1][2]) / 2.0, d2);
			}
			if (r2.dot(result) < 0.0){
				r2 *= -1.0;
			}
			r2.normalize();
			r2 *= angle;
			result = r2;
		}
	}
	void OrientationEKF::rodriguesSo3Exp(const double3& w, double kA, double kB, matrix3x3d& result)const{
		double wx2 = w.x * w.x;
		double wy2 = w.y * w.y;
		double wz2 = w.z * w.z;

		result.mm[0][0] = 1.0 - kB * (wy2 + wz2);
		result.mm[1][1] = 1.0 - kB * (wx2 + wz2);
		result.mm[2][2] = 1.0 - kB * (wx2 + wy2);

		double a, b;
		a = kA * w.z;
		b = kB * (w.x * w.y);
		result.mm[0][1] = b - a;
		result.mm[1][0] = b + a;

		a = kA * w.y;
		b = kB * (w.x * w.z);
		result.mm[0][2] = b + a;
		result.mm[2][0] = b - a;

		a = kA * w.x;
		b = kB * (w.y * w.z);
		result.mm[1][2] = b - a;
		result.mm[2][1] = b + a;
	}
	void OrientationEKF::generatorField(int i, const matrix3x3d& pos, matrix3x3d& result)
	{
		result.mm[i][0] = 0.0;
		result.mm[(i + 1) % 3][0] = -pos.mm[(i + 2) % 3][0];
		result.mm[(i + 2) % 3][0] = pos.mm[(i + 1) % 3][0];
	}
	BOOL OrientationEKF::isReady()
	{
		return sensorTimeStampAcc != 0;
	}
	matrix4x4 OrientationEKF::getPredictedGLMatrix(double secondsAfterLastGyroEvent) const
	{
		double dT = secondsAfterLastGyroEvent;
		double3 pmu;
		pmu = double3(lastGyro[0] * -dT, lastGyro[1] * -dT, lastGyro[2] * -dT);
		matrix3x3d so3PredictedMotion;
		sO3FromMu(pmu, so3PredictedMotion);
		matrix3x3d so3PredictedState;
		so3PredictedState = so3PredictedMotion * so3SensorFromWorld;
		//LogMat("GM1", so3PredictedMotion);
		//LogMat("GM2", so3PredictedState);
		return glMatrixFromSo3(so3PredictedState);
	}

	matrix4x4 OrientationEKF::glMatrixFromSo3(const matrix3x3d& s) const
	{
		matrix4x4 ret;
		for (int r = 0; r < 3; r++)
			for (int c = 0; c < 3; c++)
			{
				ret.m[c][r] = s.mm[r][c];
			}
			ret.f[3] = ret.f[7] = ret.f[11] = 0.0;
			ret.f[12]=ret.f[13]=ret.f[14]=0;
			ret.f[15] = 1.0;
			return ret;
	}
	void OrientationEKF::processGyro(float* gyro, long sensorTimeStamp)
	{
		float kTimeThreshold = 0.04f;
		float kdTdefault = 0.01f;
		if (sensorTimeStampGyro != 0L) {
			float dT = (float)(sensorTimeStamp - sensorTimeStampGyro) * 1.0E-09f;
			if (dT > 0.04f)
				dT = gyroFilterValid ? filteredGyroTimestep : 0.01f;
			else {
				filterGyroTimestep(dT);
			}
			mu = double3(gyro[0] * -dT, gyro[1] * -dT, gyro[2] * -dT);
			sO3FromMu(mu, so3LastMotion);
			so3SensorFromWorld = so3LastMotion * so3SensorFromWorld;
			updateCovariancesAfterMotion();
			matrix3x3d temp = mQ;
			temp *= dT * dT;
			mP += temp;
		}
		sensorTimeStampGyro = sensorTimeStamp;
		lastGyro[0] = gyro[0];
		lastGyro[1] = gyro[1];
		lastGyro[2] = gyro[2];
	}

	void OrientationEKF::processAcc(float* acc, long sensorTimeStamp)
	{
		mz = double3(acc[0], acc[1], acc[2]);

		if (sensorTimeStampAcc != 0L) {
			accObservationFunctionForNumericalJacobian(so3SensorFromWorld, mNu);
			double eps = 1.0E-07;
			for (int dof = 0; dof < 3; dof++) {
				double3 delta;
				delta.setComponent(dof, eps);
				matrix3x3d tempM1;
				sO3FromMu(delta, tempM1);
				matrix3x3d tempM2 = tempM1 * so3SensorFromWorld;
				double3 tempV1;
				accObservationFunctionForNumericalJacobian(tempM2, tempV1);
				double3 tempV2 = mNu - tempV1;
				tempV2 *= (1.0 / eps);
				mH.setcol(dof, tempV2);
			}
			matrix3x3d tempM3,tempM4,tempM5;
			//
			mH.transpose(tempM3);
			tempM4 = mP * tempM3;
			tempM5 = mH * tempM4;
			mS = tempM5 + mRaccel;
			mS.invert(tempM3);
			mH.transpose(tempM4);
			tempM5 = tempM4 * tempM3;
			mK = mP * tempM5;
			mx = mK * mNu;
			tempM3 = mK * mH;
			tempM4.identity();
			tempM4 -= tempM3;
			tempM3 = tempM4 * mP;
			mP = tempM3;
			sO3FromMu(mx, so3LastMotion);
			so3SensorFromWorld = so3LastMotion * so3SensorFromWorld;
			updateCovariancesAfterMotion();
		}
		else
		{
			sO3FromTwoVec(down, mz, so3SensorFromWorld);
		}
		sensorTimeStampAcc = sensorTimeStamp;
	}

	void OrientationEKF::filterGyroTimestep(float timeStep)
	{
		float kFilterCoeff = 0.95f;
		float kMinSamples = 10.0f;
		if (!timestepFilterInit) {
			filteredGyroTimestep = timeStep;
			numGyroTimestepSamples = 1;
			timestepFilterInit = true;
		}
		else {
			filteredGyroTimestep = (0.95f * filteredGyroTimestep + 0.05000001f * timeStep);

			if (++numGyroTimestepSamples > 10.0f)
				gyroFilterValid = true;
		}
	}

	void OrientationEKF::updateCovariancesAfterMotion()
	{
		matrix3x3d tempM1,tempM2;
		so3LastMotion.transpose(tempM1);
		tempM2 = mP * tempM1;
		mP = so3LastMotion * tempM2;
		so3LastMotion.identity();
	}

	void OrientationEKF::accObservationFunctionForNumericalJacobian(const matrix3x3d& so3SensorFromWorldPred, double3& result)
	{
		matrix3x3d tempM;
		mh = so3SensorFromWorldPred * down;
		sO3FromTwoVec(mh, mz, tempM);
		muFromSO3(tempM, result);
	}

	void OrientationEKF::magObservationFunctionForNumericalJacobian(const matrix3x3d& so3SensorFromWorldPred, double3& result)
	{
		matrix3x3d tempM;
		mh = so3SensorFromWorldPred * north;
		sO3FromTwoVec(mh, mz, tempM);
		muFromSO3(tempM, result);
	}
	void OrientationEKF::arrayAssign(matrix4x4& data, matrix3x3d& m)
	{
		m = matrix3x3d(data.m[0][0], data.m[0][1], data.m[0][2], data.m[1][0], data.m[1][1], data.m[1][2], data.m[2][0], data.m[2][1], data.m[2][2]);
	}
	void	Camera::processAcc(float x,float y,float z,long timestamp){
		if(g_manager.IsVR3DViewer()||g_manager.IsVR3DSingleEye()){
			float d[3];
			d[0]=x;
			d[1]=y;
			d[2]=z;
			m_ekf.processAcc(d, timestamp);
		}
	}
	void	Camera::processGyro(float x,float y,float z,long timestamp)
	{
		if(g_manager.IsVR3DViewer()||g_manager.IsVR3DSingleEye()){
			m_ekf.m_dUpdateTime = Phantom::GetTickTime();
			float d[3];
			d[0]=x;
			d[1]=y;
			d[2]=z;
			m_ekf.processGyro(d, timestamp);
		}
	}
	void	Camera::processEye(){
		if(g_manager.IsVR3DViewer()||g_manager.IsVR3DSingleEye()){
			double secondsSinceLastGyroEvent = (GetTickTime() - m_ekf.m_dUpdateTime) * 1.E-09;
			double secondsToPredictForward = secondsSinceLastGyroEvent + 0.03333333333333333;
			matrix4x4 ret = m_ekf.getPredictedGLMatrix(secondsToPredictForward);
			Set3DVRViewMatrix(ret);
		}
	}
	IntersectData::IntersectData(void)
	{
		memset(&this->m_lineRay, 0, sizeof(PDirection3D));
		this->m_vOrig.setxyz(0,0,0);
		this->m_vDir.setxyz(0,0,0);
		this->m_picked = 0;
		memset(&m_info,0,sizeof(PickInfoData));
		m_fMaxDistance = 0;
		m_lastPixel.Set(-1,-1);
	}

	IntersectData::~IntersectData(void)
	{
	}

	BOOL	IntersectData::begin(const float3& vOrig, const float3& vDir, float fMaxDistance)
	{CPUTime(IntersectData);
		m_lastPixel.Set(-1,-1);
		this->m_vOrig = vOrig;
		this->m_vDir = vDir;
		this->m_fMaxDistance = fMaxDistance;
		m_vDir.normalize();
		reset_pick();
		return true;
	}

	BOOL	IntersectData::begin(Camera* _camera,int x,int y)
	{CPUTime(IntersectData);
		return begin(_camera->GetViewMatrix(), _camera->GetProjMatrix(), x, y, _camera->GetTarget(), _camera->GetEye(), 1000000.0f, true, !_camera->IsPerspectiveProjection());
	}
	BOOL	IntersectData::begin(const matrix4x4& view, const matrix4x4& proj, int x, int y, const float3& target, const float3& eye, float fMaxDistance,
		BOOL bIsFPSMode, BOOL bOrtho)
	{CPUTime(IntersectData);
		m_lastPixel.Set(x,y);
		this->m_fMaxDistance = fMaxDistance;
		float fWidth	= (float)g_manager.GetBufferWidth();
		float fHeight	= (float)g_manager.GetBufferHeight();//
		float sx 		= (float)x;//fWidth;
		float sy 		= (float)y;//fHeight;
		//
		const matrix4x4& matProj = proj;
		const matrix4x4& matView = view;
		float3 v;
		if(!bIsFPSMode)
		{
			v.x =  ( ( ( 2.0f * sx ) / fWidth) - 1 ) / matProj._11;
			v.y = -( ( ( 2.0f * sy ) / fHeight) - 1 ) / matProj._22;
			v.z =  1.0f;
		}
		else
		{
			v.x =  ( sx / fWidth-0.5f )*2.0f / matProj._11;
			v.y =	 ( 0.5f-sy / fHeight )*2.0f / matProj._22;
			v.z =  -1.0f;
		}
		matrix4x4	matViewInverse;
		matrixInverse(&matViewInverse, 0, &matView);
		matrix4x4&	m = matViewInverse;
		if(bOrtho)
		{
			float3 v2;
			v2.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
			v2.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
			v2.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
			m_vOrig.x = v2.x+m._41;
			m_vOrig.y = v2.y+m._42;
			m_vOrig.z = v2.z+m._43;
			m_vDir.x=target.x-eye.x;//m._41;
			m_vDir.y=target.y-eye.y;//m._42;
			m_vDir.z=target.z-eye.z;//m._43;
		}
		else
		{
			m_vDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
			m_vDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
			m_vDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
			m_vOrig.x = m._41;
			m_vOrig.y = m._42;
			m_vOrig.z = m._43;
		}
		m_vDir.normalize();
		reset_pick();
		return true;
	}

	void			IntersectData::reset_pick()
	{CPUTime(IntersectData);
		memset(&m_info,0,sizeof(PickInfoData));
		m_info.fDist = m_fMaxDistance;
		m_lineRay.setup(m_vOrig,m_vDir);
	}

	BOOL			IntersectData::PushPickInfo(const PickInfoData& _info)
	{
		if(_info.fDist < m_info.fDist)
		{
			m_info = _info;
			return true;
		}
		return false;
	}

	BOOL			IntersectData::IsPicked()
	{CPUTime(IntersectData);
		return (this->m_info.pData!=NULL);
	}
	CamLineIntersect::CamLineIntersect()
	{
		m_vDir.identity();
		m_vFrom.identity();
		m_vTo.identity();
		m_distRate=-DIST_MAX;
		m_fradius=m_fdirMul=0.0f;
		m_bDirection=false;
	}
	BOOL CamLineIntersect::ExtendXYInz(float3& vOut,float z)
	{CPUTime(CamLineIntersect);
		if(fabsf(m_vDir.z)<0.0001)
			return false;
		float yr=(z-m_vFrom.z)/m_vDir.z;
		vOut.x=m_vFrom.x+yr*m_vDir.x;
		vOut.y=m_vFrom.y+yr*m_vDir.y;
		vOut.z=z;
		return true;
	}
	void CamLineIntersect::SetDirectionLength(float distance)
	{CPUTime(CamLineIntersect);
		float r=sqrtf(m_vDir.x*m_vDir.x+m_vDir.y*m_vDir.y+m_vDir.z*m_vDir.z);
		m_vDir.x=(m_vDir.x/r)*distance;
		m_vDir.y=(m_vDir.y/r)*distance;
		m_vDir.z=(m_vDir.z/r)*distance;
		m_vTo=m_vFrom+m_vDir;
		BuildParam();
	}
	void CamLineIntersect::GetPickPosition(float3& vPickPos)
	{CPUTime(CamLineIntersect);
		vPickPos.x=m_vDir.x/MAX_DIRECTION*m_distRate+m_vFrom.x;
		vPickPos.y=m_vDir.y/MAX_DIRECTION*m_distRate+m_vFrom.y;
		vPickPos.z=m_vDir.z/MAX_DIRECTION*m_distRate+m_vFrom.z;
		if(vPickPos.x>DIST_MAX)vPickPos.x=0.0f;
		if(vPickPos.y>DIST_MAX)vPickPos.y=0.0f;
		if(vPickPos.z>DIST_MAX)vPickPos.z=0.0f;
	}
	void	CamLineIntersect::BuildParam()
	{CPUTimeNone();
		m_distRate=DIST_MAX;
		m_fradius=m_vDir.R();
		m_fdirMul=(m_vDir.x*m_vDir.x+m_vDir.y*m_vDir.y+m_vDir.z*m_vDir.z);
	}
	void	CamLineIntersect::Initialize(float3& vFrom,float3& pvTo,float fDirMultiply,BOOL bDirection)
	{CPUTime(CamLineIntersect);
		pvTo.subv(m_vDir,vFrom);
		m_vFrom=vFrom;
		m_vDir.mulf(m_vDir,fDirMultiply);
		m_vFrom.addv(m_vTo,m_vDir);
		//
		m_bDirection=bDirection;
		//
		this->BuildParam();
	}
	BOOL	CamLineIntersect::IntersectSphere(float3& mdPos,float fRadius)
	{CPUTime(CamLineIntersect);
		CamLineIntersect& s=*this;
		float3	b;
		b.x=mdPos.x-s.m_vFrom.x;	b.y=mdPos.y-s.m_vFrom.y;	b.z=mdPos.z-s.m_vFrom.z;
		float bmul=sqrtf(b.x*b.x+b.y*b.y+b.z*b.z);
		float dotAB=(s.m_vDir.x*b.x+s.m_vDir.y*b.y+s.m_vDir.z*b.z);
		if(dotAB<=0.0f)
		{
			if(bmul<fRadius)
				return true;
			else
				return false;
		}
		if(bmul>s.m_fradius)
		{
			float c=sqrtf((bmul*bmul+s.m_fradius*s.m_fradius)-2*dotAB);
			if(c<fRadius)
				return true;
			else
				return false;
		}
		float cosAB=dotAB/(bmul*s.m_fradius);
		float h=bmul*sqrtf(1.0f-cosAB*cosAB);
		if(h<fRadius)
			return true;
		return false;
	}
	BOOL	CamLineIntersect::IntersectSphereDirection(float3& mdPos,float fRadius)
	{CPUTime(CamLineIntersect);
		CamLineIntersect& s=*this;
		float3	b;
		b.x=mdPos.x-s.m_vFrom.x;	b.y=mdPos.y-s.m_vFrom.y;	b.z=mdPos.z-s.m_vFrom.z;
		float bmul=sqrtf(b.x*b.x+b.y*b.y+b.z*b.z);
		float dotAB=(s.m_vDir.x*b.x+s.m_vDir.y*b.y+s.m_vDir.z*b.z);
		if(dotAB<=0.0f)
		{
			if(bmul<fRadius)
				return true;
			else
				return false;
		}
		float cosAB=dotAB/(bmul*s.m_fradius);
		float h=bmul*sqrtf(1.0f-cosAB*cosAB);
		if(h<fRadius)
			return true;
		return false;
	}
	float	CamLineIntersect::GetDistanceFromLineDirection(float3& mdPos)
	{CPUTime(CamLineIntersect);
		CamLineIntersect& s=*this;
		float3	b;
		b.x=mdPos.x-s.m_vFrom.x;	b.y=mdPos.y-s.m_vFrom.y;	b.z=mdPos.z-s.m_vFrom.z;
		float bmul=sqrtf(b.x*b.x+b.y*b.y+b.z*b.z);
		float dotAB=(s.m_vDir.x*b.x+s.m_vDir.y*b.y+s.m_vDir.z*b.z);
		float cosAB=dotAB/(bmul*s.m_fradius);
		float h=bmul*sqrtf(1.0f-cosAB*cosAB);
		return fabsf(h)/2.0f;
	}
	float	CamLineIntersect::GetDistanceFromLine(float3& mdPos)
	{CPUTime(CamLineIntersect);
		CamLineIntersect& s=*this;
		float3	b;
		b.x=mdPos.x-s.m_vFrom.x;	b.y=mdPos.y-s.m_vFrom.y;	b.z=mdPos.z-s.m_vFrom.z;
		float bmul=sqrtf(b.x*b.x+b.y*b.y+b.z*b.z);
		float dotAB=(s.m_vDir.x*b.x+s.m_vDir.y*b.y+s.m_vDir.z*b.z);
		if(dotAB<=0.0f)
		{
			return bmul;
		}
		if(bmul>s.m_fradius)
		{
			return sqrtf((bmul*bmul+s.m_fradius*s.m_fradius)-2.0f*dotAB);
		}
		float cosAB=dotAB/(bmul*s.m_fradius);
		float h=bmul*sqrtf(1.0f-cosAB*cosAB);
		return fabsf(h)/2.0f;
	}

	BOOL	CamLineIntersect::ObjectInDirectionPick(float3& mdPos,float fRadius)
	{CPUTime(CamLineIntersect);
		CamLineIntersect& s=*this;
		float3	b=mdPos-s.m_vFrom;
		float fR=b.R();
		if(fR<fRadius)
			return true;
		float fdot=b.dot(s.m_vDir);
		//解(a*b*sin(acosf(A*B)))/b=h
		if(fdot<0.0f)
			return false;
		float requestH=(s.m_fradius/fR)*fRadius;
		//float h=s.m_fradius*sinf(acosf(fdot/(fR*s.m_fradius)));
		float h=s.m_fradius*sinf(acosf(fdot/(fR*s.m_fradius)));
		return requestH>=h;//(h<=fRadius);
	}
	BOOL	CamLineIntersect::ObjectInLinePick(float3& mdPos,float fRadius)
	{CPUTime(CamLineIntersect);
		CamLineIntersect& s=*this;
		float3	b=mdPos-s.m_vFrom;
		float fR=b.R();
		if(fR<fRadius)
			return true;
		if((fR-fRadius)>s.m_fradius)
			return false;
		float fdot=b.dot(s.m_vDir);
		//解(a*b*sin(acosf(A*B)))/b=h
		if(fdot<0.0f)
			return false;
		float h=s.m_fradius*sinf(acosf(fdot/(fR*s.m_fradius)));
		return (h<=fRadius);
	}

	BOOL	CamOrthoRT::objInOrtho(const float3& p,float fObjRadius,float* pResultDistance)
	{CPUTime(CamOrthoRT);
		float3	b;
		//(p-(v0+m*vDir))*vDir=0  ->  m=((p-v0)*vDir)/(vDir*vDir)  ->radius=(p-(v0+m*vDir)).R()=(p-v0-m*vDir).radius;
		//()-()-||?
		if(pResultDistance)
			*pResultDistance = b.R();
		//
		p.subv(b,cameraMiddle.getLineFrom());
		float3& vDir=cameraMiddle.getLineDir();
		float m=(b.x*vDir.x+b.y*vDir.y+b.z*vDir.z)/fDirMul;
		b.x-=m*vDir.x;
		b.y-=m*vDir.y;
		b.z-=m*vDir.z;
		float radius=b.R()-fObjRadius;
		if(radius<=fRadius)
			return ((m>0.0f)||((m+fObjRadius/cameraMiddle.getLineRadius())>0.0f));
		return false;
	}

	BOOL	CamPerspectiveRT::objInPerspective(const float3& mdPos,float fRadius,float fMaxDistance,float* pResultDistance)
	{CPUTime(CamPerspectiveRT);
		float3	b=mdPos-cameraMiddle.getLineFrom();
		float fR=b.R();
		if(pResultDistance)
			*pResultDistance = fR;
		//
		if((fR-fRadius)<=cameraMiddle.getLineRadius())
			return true;
		if((fR-fRadius)>fMaxDistance)
			return false;
		float fmdR=b.dot(cameraMiddle.getLineDir());
		if(fmdR<0.0f)
			return false;
		float fDistRadius=acosf(fmdR/(fR*cameraMiddle.getLineRadius()))-atanf(fRadius/fR);
		return (fDistRadius<=fovy);
	}


	BOOL	Camera::Get2DFrom3D(Pixel& _out,const float3& vPos) const
	{CPUTime(Camera);
		float3 p;
		vPos.transform(p, this->m_matView);
		//
#ifdef ANDROID_OS
		int w = g_manager.GetDeviceWidth();//.m_gameWidth;
		int h = g_manager.GetDeviceHeight();//.m_gameHeight;
#else
		int w = g_manager.m_gameWidth;
		int h = g_manager.m_gameHeight;
#endif
		if(ISCameraEnable(LeftHand))
		{
			if(p.z<0.001f)
				return false;
			float finv = 1.0f/p.z;
			p.x *= finv;
			p.y *= finv;
			//
			_out.x = (int)(((p.x*m_matProjection._11 + 1.0f) * (float)w)/2.0f);
			_out.y = (int)((((-p.y)*m_matProjection._22 + 1.0f) * (float)h)/2.0f);
			//
		}
		else
		{
			if(p.z>-0.001f)
				return false;
			float finv = 1.0f/-p.z;// / m_cameraData.fFov;
			p.x *= finv;
			p.y *= finv;
			//
			_out.x = (int)((p.x*m_matProjection._11/2.0f+0.5f)*(float)w);
			_out.y = (int)((0.5f - p.y*m_matProjection._22/2.0f)*(float)h);
			//
		}
		if(_out.x<-w||_out.y<-h)
			return false;
		if(_out.x>w*2||_out.y>h*2)
			return false;
//#ifdef ANDROID_OS
//		_out.x*=2;
//		_out.y*=2;
//#endif
		return true;
	}

	void	Camera::Get3DFrom2D(float3& _orig,float3& _dir,const Pixel& _in) const
	{CPUTime(Camera);
		float3 v;
#ifdef ANDROID_OS
		int w = g_manager.GetDeviceWidth();//.m_gameWidth;
		int h = g_manager.GetDeviceHeight();//.m_gameHeight;
#else
		int w = g_manager.m_gameWidth;
		int h = g_manager.m_gameHeight;
#endif
		if(ISCameraEnable(LeftHand))
		{
			v.x =  ( ( ( 2.0f * (float)_in.x ) / (float)w) - 1 ) / m_matProjection._11;
			v.y = -( ( ( 2.0f * (float)_in.y ) / (float)h) - 1 ) / m_matProjection._22;
			v.z =  1.0f;
		}
		else
		{
			v.x =  ( (float)_in.x / (float)w-0.5f )*2.0f / m_matProjection._11;
			v.y =	 ( 0.5f-(float)_in.y / (float)h )*2.0f / m_matProjection._22;
			v.z =  -1.0f;
		}

		const matrix4x4& m=m_matViewInverse;
		if(ISCameraEnable(OrthoMode))
		{
			float3 v2;
			v2.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
			v2.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
			v2.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
			_orig.x = v2.x+m._41;
			_orig.y = v2.y+m._42;
			_orig.z = v2.z+m._43;
			_dir.x=m_cameraData.vTarget.x-m_cameraData.vEye.x;
			_dir.y=m_cameraData.vTarget.y-m_cameraData.vEye.y;
			_dir.z=m_cameraData.vTarget.z-m_cameraData.vEye.z;
		}
		else
		{
			_dir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
			_dir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
			_dir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
			_orig.x = m._41;
			_orig.y = m._42;
			_orig.z = m._43;
		}
		_dir.normalv(_dir);
	}
	//
	//
	BOOL	Camera::CheckObjectVisible(const float3& vPosition,float fRadius,float fMaxDistance,float* pResultDistance)
	{CPUTime(Camera);
		if(ISCameraEnable(OrthoMode))
			return m_vOrtho.objInOrtho(vPosition,fRadius,pResultDistance);
		return m_vPerspective.objInPerspective(vPosition,fRadius,fMaxDistance,pResultDistance);
	}



	void	Camera::getLineIn2DPixel_P(CamLineIntersect& vParam,int sx,int sy)
	{CPUTime(Camera);
		float3 v;
		//
		if(ISCameraEnable(LeftHand))
		{
			v.x =  ( ( ( 2.0f * (float)sx ) / (float)m_cameraData.nWidth) - 1 ) / m_matProjection._11;
			v.y = -( ( ( 2.0f * (float)sy ) / (float)m_cameraData.nHeight) - 1 ) / m_matProjection._22;
			v.z =  1.0f;
		}
		else
		{
			v.x =  ( (float)sx / (float)m_cameraData.nWidth-0.5f )*2.0f / m_matProjection._11;
			v.y =	 ( 0.5f-(float)sy / (float)m_cameraData.nHeight )*2.0f / m_matProjection._22;
			v.z =  -1.0f;
		}
		//
		matrix4x4& m	=	m_matViewInverse;
		float3 vOrig,vDir;
		if(ISCameraEnable(OrthoMode))
		{
			float3 v2;
			v2.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
			v2.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
			v2.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
			vOrig.x = v2.x+m._41;
			vOrig.y = v2.y+m._42;
			vOrig.z = v2.z+m._43;
			vDir.x=m_cameraData.vTarget.x-m_cameraData.vEye.x;//m._41;
			vDir.y=m_cameraData.vTarget.y-m_cameraData.vEye.y;//m._42;
			vDir.z=m_cameraData.vTarget.z-m_cameraData.vEye.z;//m._43;
		}
		else
		{
			vDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
			vDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
			vDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
			vOrig.x = m._41;
			vOrig.y = m._42;
			vOrig.z = m._43;
		}
		float3 vOD = vOrig + vDir;
		vParam.Initialize(vOrig, vOD, 1.0f,true);
	}


	void	Camera::getLineIn2DPixel()
	{CPUTime(Camera);
		if(ISCameraEnable(OrthoMode))
		{
			float3 v0,vDir;
			getLineIn2DPixel_P(m_vOrtho.cameraMiddle,m_cameraData.nWidth/2,m_cameraData.nHeight/2);
			//
			CamLineIntersect vLine;
			this->getLineIn2DPixel_P(vLine,0,0);
			//
			float3& cvDir=m_vOrtho.cameraMiddle.getLineDir();
			cvDir.mulf(cvDir,100.0f/cvDir.R());
			m_vOrtho.fDirMul=(cvDir.x*cvDir.x+cvDir.y*cvDir.y+cvDir.z*cvDir.z);
			//m_vOrtho.cameraMiddle.Initialize(1.0f,false);
			m_vOrtho.cameraMiddle.getLineFrom().subv(vLine.getLineDir(),vLine.getLineFrom());
			m_vOrtho.fRadius=vLine.getLineDir().R();
		}
		else
		{
			this->getLineIn2DPixel_P(m_vPerspective.cameraMiddle,m_cameraData.nWidth/2,m_cameraData.nHeight/2);
			//
			float fovy=this->m_cameraData.fFov/2.0f*(sqrtf((float)(m_cameraData.nWidth*m_cameraData.nWidth+m_cameraData.nHeight*m_cameraData.nHeight))/(float)m_cameraData.nHeight);
			m_vPerspective.fovy=fovy;
			m_vPerspective.tanfovy=tanf(fovy)*1.3f;
		}
	}


	void	Camera::SetOrthoProjection(int w,int h,float znear,float zfar)
	{CPUTime(Camera);
		matrixOrthoRH(&m_matProjection, (float)w, (float)h, znear, zfar);
		//
		this->m_cameraData.nWidth	=(int)((float)w * m_cameraData.fFov);
		this->m_cameraData.nHeight	=(int)((float)h * m_cameraData.fFov);
		this->m_cameraData.fZNear	=znear;
		this->m_cameraData.fZFar	=zfar;
		SetCameraEnable(OrthoMode, true);
		//
		this->getLineIn2DPixel();
		onChangeView();
	}

	void	Camera::SetPerspectiveProjection(float fov,int w,int h,float znear,float zfar)
	{CPUTime(Camera);
#ifdef WIN32_OS
        matrixPerspectiveFovRH(& m_matProjection, fov, (float)w/(float)h, znear, zfar);
#else
		matrixPerspectiveFovRH(& m_matProjection, fov*0.5f, (float)w/(float)h, znear, zfar);
#endif
		SetCameraEnable(OrthoMode, false);
		this->m_cameraData.nWidth			=w;
		this->m_cameraData.nHeight			=h;
		this->m_cameraData.fFov				=fov;
		this->m_cameraData.fZFar			=zfar;
		this->m_cameraData.fZNear			=znear;
		this->m_fAspect			=(float)w/(float)h;
		this->getLineIn2DPixel();
		onChangeView();
	}


	void	Camera::SetDitherOffset(const float3& offset, float fMaxOffsetTime)	//是否进行抖动，fMaxOffsetTime表示抖动的时间
	{CPUTime(Camera);
		this->m_ditherOffset		=	offset;
		this->m_fMaxDitherOffset	=	fMaxOffsetTime;
	}

	Camera::Camera()
	{
		m_cameraViewIdx	=	0;
		m_bPlay		=	false;
		m_bEnabled	=	true;
		this->m_ditherOffset.setxyz(0, 0, 0);
		this->m_fMaxDitherOffset	=	0;
		memset(&m_cameraData, 0, sizeof(m_cameraData));
		SetCameraEnable(LeftHand, false);//右手系
		SetCameraEnable(OrthoMode, false);
		SetCameraEnable(RotationSpeedEnable, false);
		m_cameraData.fRotationSpeedMask	=	1.0f;
		m_funcParam	=	0;
		m_cameraData.nWidth = 1;
		m_cameraData.nHeight = 1;
		m_cameraData.fFov = PI / 4.0f;
		m_fAspect = 1;
		m_rayCamera = true;
		m_fRotationVRX	=	0;
		m_cameraData.fZNear = 1;
		m_cameraData.fZFar = 100000.0f;
		m_currPlayX=m_currPlayY=0;
		m_bRefreshViewRot	=	false;
		m_fSavedMulDistance	=	0;
		m_bOffsetEnable		=	false;
		SetCameraEnable(CursorClipInRotMove, false);
		m_matProjection.identity();
		m_matView.identity();
		m_matViewInverse.identity();
		m_fOffsetSpeed	=	10.0f;
		m_fOffsetDown	=	1.0f;
		m_cameraData.vEye.identity();
		m_cameraData.vTarget.identity();
		m_cameraData.vUp.setxyz(0,0,1);
		SetCameraEnable(EnableMove, true);
		SetCameraEnable(EnableDistance, true);
		m_cameraData.fMinDistance = 1;
		m_cameraData.fMaxDistance = 1000.0f;
		ClearMoveSpeed();
		SetCameraEnable(ReturnXRot, false);
		SetCameraEnable(ReturnYRot, false);
		SetCameraEnable(ReturnZRot, false);
		SetCameraEnable(XRotation, true);
		SetCameraEnable(YRotation, true);
		m_bVRFrontSet = false;
		m_vrFrontSet.setxyz(0,0,0);
		m_vrFrontMat.identity();
		m_fVRFirst = 0;
		m_bVRFirst = true;
        for(int i=0;i<MAX_TOUCH_COUNT;i++)
        {
            m_lastPosition[i].identity();
            m_lastDownPosition[i].identity();
            m_bLButtonDown[i]	=	false;
        }
		m_bRButtonDown	=	false;
		m_bMButtonDown	=	false;
		SetCameraEnable(FPSMode, false);
		SetCameraEnable(MoveHeightIsZ, false);
		//
		m_cameraData.vRotate.identity();
		m_vLookatDest.identity();
		m_bReturnRotate = true;
		this->m_matViewVR.identity();

		m_fSaveDistance = 1;
		SetCameraEnable(ShiftMoveMul, true);

		m_cameraData.fMoveSpeed = To3DValue(10.0f);

		this->m_clipMin.setxyz(-100000000, -100000000, -100000000);
		this->m_clipMax.setxyz(100000000, 100000000, 100000000);
		//
		float3 vEye(1,0,0);
		float3 vAt(0,0,0);
		float3 vUp(0,0,1);
		m_vrRotator.setxyz(0,0,0);
		this->SetViewMatrix(&vEye, &vAt, &vUp);//&float3(1,0,0), &float3(0,0,0),&float3(0,0,1));
		//
		this->rebuildViewRot();
		this->ViewMoveDistance(10.0f,false);
		//
		m_fFrustumMul	=	1.2f;
		m_fMaxLodDistance	=	To3DValue(100.0f);
		m_cameraData.rotButton		=	MouseButton_Left;
		m_cameraData.moveButton		=	MouseButton_Middle;
		m_bArea						=	0;
		m_area.Set(0,0,0,0);
		m_fOffsetEye = 1;
		m_nVRSmooth	=	0;
	}

	Camera::~Camera(void)
	{
	}

	VOID	Camera::SetArea(const Rect* rc)
	{CPUTime(Camera);
		if(!rc || (rc->left == rc->right && rc->left==0) || (rc->top == rc->bottom&&rc->top==0))
		{
			m_bArea = false;
			return;
		}
		//m_bArea = true;
		m_area.Set(getmin(rc->left, rc->right), getmin(rc->top, rc->bottom), getmax(rc->left, rc->right), getmax(rc->top, rc->bottom));
	}
	VOID	Camera::ClearMoveSpeed()
	{CPUTime(Camera);
		m_bMoveMode	=	false;
		m_fLastMoveTime = g_manager.GetAppTime();
		m_fMoveFirst.setxyz(0,0,0);
		m_lastMovePos = m_cameraData.vTarget;
		memset(m_lastMoveOffset, 0, sizeof(m_lastMoveOffset));
		memset(m_lastMoveOffsetTime, 0, sizeof(m_lastMoveOffsetTime));
		m_lastMoveOffsetCnt	=	0;
		AddMoveSpeed();
	}
	VOID	Camera::AddMoveSpeed()//移动距离和时间
	{CPUTime(Camera);
		m_fLastMoveTime = g_manager.GetAppTime();
		int index = m_lastMoveOffsetCnt % MAX_MOVE_SPEED;
		m_lastMoveOffset[index] = m_cameraData.vTarget;
		m_lastMoveOffsetTime[index] = g_manager.GetAppTime();
		m_lastMoveOffsetCnt++;
	}
	VOID	Camera::CheckMoveMode()//移动距离和时间
	{CPUTime(Camera);
		int begin = m_lastMoveOffsetCnt-MAX_MOVE_SPEED-1;
		if(begin<0)begin=0;
		begin %= MAX_MOVE_SPEED;
		double dTimeDiff = g_manager.GetAppTime()-m_lastMoveOffsetTime[begin];
		if(dTimeDiff >= 0.2f)
			dTimeDiff = 0.2f;
		m_fMoveFirst = m_cameraData.vTarget - m_lastMoveOffset[begin];
		m_fOffsetSpeed = m_fMoveFirst.length() * (0.2f-dTimeDiff)*5.0f;
		if(m_fOffsetSpeed<=1)
			return;
		m_fMoveFirst.normalize();
		m_bMoveMode = true;
	}
	VOID	Camera::OnMoveMode()//移动计算
	{CPUTime(Camera);
		if(!m_bOffsetEnable)return;
		if(!m_bMoveMode)
		{
#ifdef EDITOR_OS
			if(m_bLButtonDown[0]||m_bLButtonDown[1]||m_bLButtonDown[2])
#else
			if(m_bLButtonDown[0])
#endif
			{
				double dTime = g_manager.GetAppTime() - m_fLastMoveTime;
				if(dTime>0.1f){
					AddMoveSpeed();
				}
			}
		}
		else{
			float fElapsedTime = g_manager.GetElapsedTime();
			float3 save = m_cameraData.vTarget;
			float3 dir = m_cameraData.vEye - m_cameraData.vTarget;
			m_fOffsetSpeed	-=	m_fOffsetSpeed*(m_fOffsetDown*0.1f);
			if(m_fOffsetSpeed<0){
				m_bMoveMode = false;
				return;
			}
			save += m_fMoveFirst*m_fOffsetSpeed;
			m_cameraData.vTarget = save;
			m_cameraData.vEye = save + dir;
			this->SetViewMatrix(0,0,0);
		}
	}
	VOID	Camera::OnInputEvent(Phantom::InputEventID evt, int x, int y, unsigned int touchIndex)
	{CPUTime(Camera);
		if(!m_bEnabled)return;
		bool bRotation		=	(ISCameraEnable(YRotation) && ISCameraEnable(XRotation));
		bool bSet			=	false;
		if(evt == Phantom::InputEventID_ButtonDown)
		{
            m_lastDownPosition[touchIndex].Set(x, y);
			m_lastPosition[touchIndex].Set(x, y);
			m_bLButtonDown[touchIndex]	=	true;
			m_fSavedMulDistance	=	this->GetDistance();
#ifndef EDITOR_OS
			if(touchIndex==0)
#endif
				ClearMoveSpeed();
		}
		else if(evt == Phantom::InputEventID_ButtonUp)
		{
			m_bLButtonDown[touchIndex]	=	false;
#ifndef EDITOR_OS
			if(touchIndex==0)
#endif
				CheckMoveMode();
		}
		else if(evt == Phantom::InputEventID_MouseMove)
		{
			float fX=(float)(m_lastPosition[touchIndex].x - x);
			float fY=(float)(m_lastPosition[touchIndex].y - y);
#ifndef EDITOR_OS
			if(bRotation){
				if(m_bLButtonDown[0])
					RotationCamera2D(-fX, fY);
			}
			if(ISCameraEnable(EnableMove)){
				if(m_bLButtonDown[0])
					MoveCamera2D(fX, fY);
			}
#else
			if(bRotation){
				if(m_bLButtonDown[0] && this->m_cameraData.rotButton == Phantom::MouseButton_Left)//!m_bLButtonDown[1])
					RotationCamera2D(-fX, fY);
				else if(m_bLButtonDown[1] && this->m_cameraData.rotButton == Phantom::MouseButton_Right)//!m_bLButtonDown[1])
					RotationCamera2D(-fX, fY);
				else if(m_bLButtonDown[2] && this->m_cameraData.rotButton == Phantom::MouseButton_Middle)//!m_bLButtonDown[1])
					RotationCamera2D(-fX, fY);
			}
			if(ISCameraEnable(EnableMove)){
				if(m_bLButtonDown[0] && this->m_cameraData.moveButton == Phantom::MouseButton_Left)//!m_bLButtonDown[1])
					MoveCamera2D(fX, fY);
				else if(m_bLButtonDown[1] && this->m_cameraData.moveButton == Phantom::MouseButton_Right)//!m_bLButtonDown[1])
					MoveCamera2D(fX, fY);
				else if(m_bLButtonDown[2] && this->m_cameraData.moveButton == Phantom::MouseButton_Middle)//!m_bLButtonDown[1])
					MoveCamera2D(fX, fY);
				//if(touchIndex==0)
					//AddMoveSpeed();
			}
#endif
  			m_lastPosition[touchIndex].x	=	x;
			m_lastPosition[touchIndex].y	=	y;
     //       if(m_bLButtonDown[0] && m_bLButtonDown[1])
     //       {
     //           float2 b0(m_lastDownPosition[0].x, m_lastDownPosition[0].y);
     //           float2 b1(m_lastDownPosition[1].x, m_lastDownPosition[1].y);
     //           float2 e0(m_lastPosition[0].x, m_lastPosition[0].y);
     //           float2 e1(m_lastPosition[1].x, m_lastPosition[1].y);
     //           float2 bd = b1 - b0;
     //           float2 ed = e1 - e0;
     //           if((bd.x > 0 || bd.x < 0) && (bd.y > 0 || bd.y < 0))
     //           {
     //               float fScale = ed.length() - bd.length();
     //               float fMul			=	fScale / 100.0;
					//float fNewDist		=	m_fSavedMulDistance + m_fSavedMulDistance * fMul;
     //               //float fDistance		=	(m_cameraData.fMaxDistance  - m_cameraData.fMinDistance) * 0.033f * fMul;//this->GetViewMoveDistance(fMul);
     //               float fDistance			=	fNewDist - m_fSaveDistance;//clamp(fDistance, -10.0f, 10.0f);
     //               ViewMoveDistance(fDistance, false);//
     //           }
     //       }
		}
		else if(evt == Phantom::InputEventID_Wheel)
		{
			float fMul			=	1.0f;
			if(x >= 0)
				fMul			=	-1.0f;
			float fDistance		=	(m_cameraData.fMaxDistance  - m_cameraData.fMinDistance) * 0.033f * fMul;//this->GetViewMoveDistance(fMul);
			fDistance			=	clamp(fDistance, -10.0f, 10.0f);
			ViewMoveDistance(fDistance, false);//
		}
	}
	void	Camera::SetRotationSpeed(BOOL bEnable, float fMask)
	{CPUTime(Camera);
		SetCameraEnable(RotationSpeedEnable, bEnable);
		this->m_cameraData.fRotationSpeedMask		= fMask;
	}

	void	Camera::SetRightHandArrow(BOOL br)
	{CPUTime(Camera);
		SetCameraEnable(LeftHand, !br);
		SetViewMatrix();
		//
	}


	void	Camera::SetViewMatrix(const float3* pEye,const float3* pTarget,const float3* pUp)
	{CPUTime(Camera);
		m_bPlay = false;
		if(pEye)
			this->m_cameraData.vEye	=	*pEye;
		if(pTarget)
			this->m_cameraData.vTarget	=	*pTarget;
		if(pUp)
			this->m_cameraData.vUp		=	*pUp;
		//
		if(m_bArea){
			float3 dir = m_cameraData.vEye - m_cameraData.vTarget;
			if(m_cameraData.vTarget.x < m_area.left)
				m_cameraData.vTarget.x = m_area.left;
			else if(m_cameraData.vTarget.x > m_area.right)
				m_cameraData.vTarget.x = m_area.right;
			if(m_cameraData.vTarget.y < m_area.top)
				m_cameraData.vTarget.y = m_area.top;
			else if(m_cameraData.vTarget.y > m_area.bottom)
				m_cameraData.vTarget.y = m_area.bottom;
			m_cameraData.vEye = m_cameraData.vTarget + dir;
		}
		float3 vEye	= m_cameraData.vEye + m_ditherOffset;
		float3 vTarget = m_cameraData.vTarget + m_ditherOffset;
		matrixLookAtRH(& m_matView, & vEye, & vTarget, & m_cameraData.vUp);
		//
		onChangeView();
	}
	void	Camera::SetViewMatrixOnce(const float3& pEye,const float3& pTarget)
	{CPUTime(Camera);
		matrixLookAtRH(&m_matView, &pEye, &pTarget, & m_cameraData.vUp);
		onChangeView();
	}
	void	Camera::onChangeView()
	{CPUTime(Camera);
		m_cameraViewIdx++;
		matrixMultiply(&m_viewProjection, &m_matView, &m_matProjection);
		matrixInverse(& m_matViewInverse, NULL, & m_matView);
		this->getLineIn2DPixel();
		m_fSaveDistance = (m_cameraData.vEye - m_cameraData.vTarget).R();
		setupFrustum();
		//
		static matrix4x4 screenMat = matrix4x4(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f,  0.0f, 0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f);
		matrixMultiply(&m_screenViewProjMat, &m_viewProjection, &screenMat);
		matrixInverse( &m_matBoards[MatrixBoard_CameraBoard], 0, &m_matView );
		m_matBoards[MatrixBoard_CameraBoard].SetPosition(0, 0, 0);
		m_matBoards[MatrixBoard_ZBoard].identity();
		float3 dir = this->m_cameraData.vTarget - this->m_cameraData.vEye;
		if( dir.y > 0.0f )
			matrixRotationZ( &m_matBoards[MatrixBoard_Billbard], -atanf(dir.x/dir.y)-PI/2.0f);
		else
			matrixRotationZ( &m_matBoards[MatrixBoard_Billbard], PI/2.0f-atanf(dir.x/dir.y));
		m_matBoards[MatrixBoard_Billbard].SetPosition( 0, 0, 0 );
	}
	CDynamicArray<matrix4x4>	g_matProcess;
	matrix4x4	g_saveSmooth;
	void	SaveVRSmooth()
	{
		g_saveSmooth = GetCameraPtr()->m_matViewVR;
	}
	VOID	Camera::SetVRFront(const float3& dir)
	{
		this->m_bVRFrontSet = true;
		this->m_vrFrontSet = dir;
		m_vrFrontSet.normalize();
		LogInfo("VR_Front:(%.03f,%.03f,%.03f)", m_vrFrontSet.x, m_vrFrontSet.y, m_vrFrontSet.z);
	}
	VOID	Camera::Set3DVRViewMatrix(const matrix4x4& mat)//3D VR View matrix
	{CPUTime(Camera);
	if(g_manager.IsVR3DViewer()||g_manager.IsVR3DSingleEye()){
		if(m_nVRSmooth==0){
			this->m_matViewVR = mat;
			Set3DVRViewMatrix();
			return;
		}
		//matrixLerp(&m_matViewVR, &g_saveSmooth, &mat, 1.0f - clamp((float)m_nVRSmooth/100.0f, 0, 1));
		if(m_nVRSmooth<=1){
			this->m_matViewVR = mat;
			return;
		}
		if(m_nVRSmooth>50)m_nVRSmooth = 50;
		static int num = 0;
		if(g_matProcess.size()<m_nVRSmooth)
			g_matProcess.push_back(mat);
		else
			g_matProcess[num%m_nVRSmooth] = mat;
		num++;
		//
		matrix4x4 matV;
		memset(&matV, 0, sizeof(matrix4x4));
		float fInv = 1.0f / (float)g_matProcess.size();
		for(int i=0;i<g_matProcess.size();i++){
			matV += g_matProcess[i] * fInv;
		}
		m_matViewVR = matV;
		Set3DVRViewMatrix();
	}
	}
	VOID	Camera::ResetAll()
	{
		m_bVRFirst = true;
	}
	float InvSqrt( float F )
	{
		return 1.0f / sqrtf( F );
	}
	struct rotator{
		float Pitch,Yaw,Roll;
	};
	
	float Atan2(float Y, float X)
	{
		//return atan2f(Y,X);
		// atan2f occasionally returns NaN with perfectly valid input (possibly due to a compiler or library bug).
		// We are replacing it with a minimax approximation with a max relative error of 7.15255737e-007 compared to the C library function.
		// On PC this has been measured to be 2x faster than the std C version.

		const float absX = fabsf(X);
		const float absY = fabsf(Y);
		const bool yAbsBigger = (absY > absX);
		float t0 = yAbsBigger ? absY : absX; // Max(absY, absX)
		float t1 = yAbsBigger ? absX : absY; // Min(absX, absY)

		if (t0 == 0.f)
			return 0.f;

		float t3 = t1 / t0;
		float t4 = t3 * t3;

		static const float c[7] = {
			+7.2128853633444123e-03f,
			-3.5059680836411644e-02f,
			+8.1675882859940430e-02f,
			-1.3374657325451267e-01f,
			+1.9856563505717162e-01f,
			-3.3324998579202170e-01f,
			+1.0f
		};

		t0 = c[0];
		t0 = t0 * t4 + c[1];
		t0 = t0 * t4 + c[2];
		t0 = t0 * t4 + c[3];
		t0 = t0 * t4 + c[4];
		t0 = t0 * t4 + c[5];
		t0 = t0 * t4 + c[6];
		t3 = t0 * t3;

		t3 = yAbsBigger ? (0.5f * PI) - t3 : t3;
		t3 = (X < 0.0f) ? PI - t3 : t3;
		t3 = (Y < 0.0f) ? -t3 : t3;

		return t3;
	}
	template< class T > 
	T Square( const T A )
	{
		return A*A;
	}
	int TruncToInt(float F)
	{
		return (int)F;
	}
	float TruncToFloat(float F)
	{
		return (float)TruncToInt(F);
	}
	#define FASTASIN_HALF_PI (1.5707963050f)
	float FastAsin(float Value)
	{
		// Clamp input to [-1,1].
		bool nonnegative = (Value >= 0.0f);
		float x = fabsf(Value);
		float omx = 1.0f - x;
		if (omx < 0.0f)
		{
			omx = 0.0f;
		}
		float root = sqrtf(omx);
		// 7-degree minimax approximation
		float result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + FASTASIN_HALF_PI;
		result *= root;  // acos(|x|)
		// acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
		return (nonnegative ? FASTASIN_HALF_PI - result : result - FASTASIN_HALF_PI);
	}
	float Fmod(float X, float Y)
	{
		if (fabsf(Y) <= 1.e-8f)
		{
			return 0.f;
		}
		const float Quotient = TruncToFloat(X / Y);
		float IntPortion = Y * Quotient;

		// Rounding and imprecision could cause IntPortion to exceed X and cause the result to be outside the expected range.
		// For example Fmod(55.8, 9.3) would result in a very small negative value!
		if (fabsf(IntPortion) > fabsf(X))
		{
			IntPortion = X;
		}

		const float Result = X - IntPortion;
		return Result;
	}
	float ClampAxis( float Angle )
	{
		// returns Angle in the range (-360,360)
		Angle = Fmod(Angle, 360.f);

		if (Angle < 0.f)
		{
			// shift to [0,360) range
			Angle += 360.f;
		}

		return Angle;
	}

	float NormalizeAxis( float Angle )
	{
		// returns Angle in the range [0,360)
		Angle = ClampAxis(Angle);

		if (Angle > 180.f)
		{
			// shift to (-180,180]
			Angle -= 360.f;
		}

		return Angle;
	}

	rotator QuatToRotator(quaternion& qtn)
	{
		float X = qtn.x;
		float Y = qtn.y;
		float Z = qtn.z;
		float W = qtn.w;
		const float SingularityTest = Z*X-W*Y;
		const float YawY = 2.f*(W*Z+X*Y);
		const float YawX = (1.f-2.f*(Square(Y) + Square(Z)));
		const float SINGULARITY_THRESHOLD = 0.4999995f;
		const float RAD_TO_DEG = (180.f)/PI;
		rotator RotatorFromQuat;

		if (SingularityTest < -SINGULARITY_THRESHOLD)
		{
			RotatorFromQuat.Pitch = -90.f;
			RotatorFromQuat.Yaw = Atan2(YawY, YawX) * RAD_TO_DEG;
			RotatorFromQuat.Roll = NormalizeAxis(-RotatorFromQuat.Yaw - (2.f * Atan2(X, W) * RAD_TO_DEG));
		}
		else if (SingularityTest > SINGULARITY_THRESHOLD)
		{
			RotatorFromQuat.Pitch = 90.f;
			RotatorFromQuat.Yaw = Atan2(YawY, YawX) * RAD_TO_DEG;
			RotatorFromQuat.Roll = NormalizeAxis(RotatorFromQuat.Yaw - (2.f * Atan2(X, W) * RAD_TO_DEG));
		}
		else
		{
			RotatorFromQuat.Pitch = FastAsin(2.f*(SingularityTest)) * RAD_TO_DEG;
			RotatorFromQuat.Yaw = Atan2(YawY, YawX) * RAD_TO_DEG;
			RotatorFromQuat.Roll = Atan2(-2.f*(W*X+Y*Z), (1.f-2.f*(Square(X) + Square(Y)))) * RAD_TO_DEG;
		}

		return RotatorFromQuat;
	}
	quaternion	MatrixToQuat(matrix4x4& mat){

		//const MeReal *const t = (MeReal *) tm;
		float	s;

		// Check diagonal (trace)
		const float tr = mat.m[0][0] + mat.m[1][1] + mat.m[2][2];
		quaternion qtn;
		if (tr > 0.0f) 
		{
			float InvS = InvSqrt(tr + 1.f);
			qtn.w = 0.5f * (1.f / InvS);
			s = 0.5f * InvS;

			qtn.x = (mat.m[1][2] - mat.m[2][1]) * s;
			qtn.y = (mat.m[2][0] - mat.m[0][2]) * s;
			qtn.z = (mat.m[0][1] - mat.m[1][0]) * s;
		} 
		else 
		{
			// diagonal is negative
			int i = 0;

			if (mat.m[1][1] > mat.m[0][0])
				i = 1;

			if (mat.m[2][2] > mat.m[i][i])
				i = 2;

			static const int nxt[3] = { 1, 2, 0 };
			const int j = nxt[i];
			const int k = nxt[j];

			s = mat.m[i][i] - mat.m[j][j] - mat.m[k][k] + 1.0f;

			float InvS = InvSqrt(s);

			float qt[4];
			qt[i] = 0.5f * (1.f / InvS);

			s = 0.5f * InvS;

			qt[3] = (mat.m[j][k] - mat.m[k][j]) * s;
			qt[j] = (mat.m[i][j] + mat.m[j][i]) * s;
			qt[k] = (mat.m[i][k] + mat.m[k][i]) * s;

			qtn.x = qt[0];
			qtn.y = qt[1];
			qtn.z = qt[2];
			qtn.w = qt[3];

			//DiagnosticCheckNaN();
		}
		return qtn;
	}
	void	Camera::Set3DVRViewMatrix()
	{
		matrix4x4 pos;
		matrixTranslation(&pos, this->m_cameraData.vEye.x, this->m_cameraData.vEye.y, this->m_cameraData.vEye.z);
		if(m_bVRFirst){
			float3 vdir(0,1,0);
			vdir.transformNoMove(vdir, m_matViewVR);
			m_fVRFirst = getRotateZ(vdir);
			m_bVRFirst = false;
		}
		if(m_bVRFrontSet){
			float fFrontZ = getRotateZ(this->m_vrFrontSet);
			matrixRotationZ(&m_vrFrontMat, m_fVRFirst-fFrontZ+PI);
			m_bVRFrontSet = false;
		}
		if((int)(m_fRotationVRX*1000)!=0){
			matrix4x4 matX;
			matrixRotationY(&matX, m_fRotationVRX);
			matrixMultiply(&pos, &matX, &pos);//VR旋转测试功能
		}
		matrixMultiply(&pos, &m_vrFrontMat, &pos);//VR旋转测试功能
		matrixInverse(&pos, 0, &pos);
		matrixMultiply(&m_matView, &pos, &m_matViewVR);
		//m_matView是最终计算出来的正前方VR值
		onChangeView();
		quaternion qtn = MatrixToQuat(m_matViewInverse);
		rotator rot = QuatToRotator(qtn);
		m_vrRotator.x = rot.Pitch;
		m_vrRotator.y = rot.Yaw;
		m_vrRotator.z = rot.Roll;
		//float3 vdir2(0,1,0);
		//vdir2.transformNoMove(vdir2, m_matViewVR);
		//float rotateZ = getRotateZ(vdir2);
		//LogInfo("Rotator:(%.0f,%.0f,%.0f)(%.0f)", rot.Yaw, rot.Pitch, rot.Roll, rotateZ/PI*180.0f);
	}
	void	Camera::Set3DVRViewMatrixEye(BOOL bLeftEye)//设置每个眼睛的矩阵
	{CPUTime(Camera);
		if(g_manager.IsVR3DViewer())
		{
			matrix4x4 pos;
			matrixTranslation(&pos, this->m_cameraData.vEye.x, this->m_cameraData.vEye.y, this->m_cameraData.vEye.z);
			matrixInverse(&pos, 0, &pos);
			matrixMultiply(&m_matView, &pos, &m_matViewVR);
			if(bLeftEye)
			{
				matrix4x4 posy,posz;
				matrixTranslation(&posy, 0, 1, 0);
				matrixMultiply(&posy, &m_matViewVR, &posy);
				matrixTranslation(&posz, 0, 0, 1);
				matrixMultiply(&posz, &m_matViewVR, &posz);
				float3 v = posy.GetPosition().cross(posz.GetPosition());
				m_matView._41+=v.x*m_fOffsetEye;
				m_matView._42+=v.y*m_fOffsetEye;
				m_matView._43+=v.z*m_fOffsetEye;
			}
			else
			{
				matrix4x4 posy,posz;
				matrixTranslation(&posy, 0, 1, 0);
				matrixMultiply(&posy, &m_matViewVR, &posy);
				matrixTranslation(&posz, 0, 0, 1);
				matrixMultiply(&posz, &m_matViewVR, &posz);
				float3 v = posy.GetPosition().cross(posz.GetPosition());
				m_matView._41-=v.x*m_fOffsetEye;
				m_matView._42-=v.y*m_fOffsetEye;
				m_matView._43-=v.z*m_fOffsetEye;
			}
			onChangeView();
		}
	}
	void	Camera::SetViewMatrix(const matrix4x4& view)
	{CPUTime(Camera);
		m_matView	=	view;
		onChangeView();
		//matrixInverse(& m_matViewInverse, NULL, & m_matView);
	}

	void	Camera::SetProjectionMatrix(const matrix4x4& proj)
	{CPUTime(Camera);
		m_matProjection	=	proj;
		onChangeView();
	}

	BOOL	Camera::IsPerspectiveProjection()
	{CPUTime(Camera);
		return (!ISCameraEnable(OrthoMode));
	}

	BOOL	Camera::IsOrthoProjection()
	{CPUTime(Camera);
		return ISCameraEnable(OrthoMode);
	}

	float	Camera::GetDistance()
	{CPUTime(Camera);
		float3 vDir = m_cameraData.vEye - m_cameraData.vTarget;
		return vDir.R();
	}

	void	Camera::SetDistance(float f)
	{CPUTime(Camera);
		if(f<=0)return;
		float3 vDir = m_cameraData.vEye - m_cameraData.vTarget;
		float fR = vDir.R();
		if(fR <= 0)vDir.setxyz(0,0,1);
		if(f > m_cameraData.fMaxDistance)f = m_cameraData.fMaxDistance;
		if(f < m_cameraData.fMinDistance)f = m_cameraData.fMinDistance;
		float3 vEye = vDir * (f / vDir.R()) + m_cameraData.vTarget;
		m_cameraData.vEye	=	vEye;
		//if(ISCameraEnable(FPSMode))
		//{
		//	float3 vDir	=	m_cameraData.vTarget - m_cameraData.vEye;
		//	vDir.normalize();
		//	m_cameraData.vTarget = (m_cameraData.vEye + vDir);
		//}
		SetViewMatrix();
	}


	float	Camera::GetViewMoveDistance(float fMul, float fStep)
	{CPUTime(Camera);
		float3 vDir = m_cameraData.vEye - m_cameraData.vTarget;
		float fR = vDir.R();
		return fR;//
		////
		//float fDistance = ((m_cameraData.fMaxDistance  - m_cameraData.fMinDistance) * 0.033f * fMul);// * fStep;(fR - this->m_cameraData.fMinDistance);
		//if(fDistance < m_cameraData.fMinDistance)
		//	fDistance	=	m_cameraData.fMinDistance;
		//else if(fDistance > m_cameraData.fMaxDistance)
		//	fDistance	=	m_cameraData.fMaxDistance;
		//return fDistance;// * fMul;
	}

	void	Camera::SetRotationXYEnable(BOOL bx,BOOL by)
	{CPUTime(Camera);
		SetCameraEnable(XRotation, bx);
		SetCameraEnable(YRotation, by);
	}

	void	Camera::returnSaveDistance(float fMul)
	{CPUTime(Camera);
		if(ISCameraEnable(FPSMode))
			return;
		float3 vDir = m_cameraData.vEye - m_cameraData.vTarget;
		float fR = vDir.R();
		float fNewR = this->m_fSaveDistance;
		float fdr = fNewR/fR;
		if(fdr > (fMul + 0.05f))
			fdr = fMul;
		float3 vEye = m_cameraData.vTarget + vDir*fdr;
		this->SetViewMatrix(&vEye);
	}

	void	Camera::ViewMoveDistance(float fDistanceAdd, BOOL bMoveTarget)
	{CPUTime(Camera);
		if(!this->ISCameraEnable(EnableDistance))return;
		//
		float3 vDir = m_cameraData.vEye-m_cameraData.vTarget;
		float3 vEye;
		float3 vTarget;
		//if(ISCameraEnable(FPSMode))
		//{
		//	vDir.normalize();
		//	vDir		*=	fDistanceAdd;
		//	vEye		=	m_cameraData.vEye + vDir;
		//	vTarget		=	m_cameraData.vTarget + vDir;
		//}
		//else
		{
			float fR = m_fSaveDistance;//vDir.R();
			float fNewR = fR+fDistanceAdd;
			if(fNewR <= this->m_cameraData.fMinDistance)
				fNewR	=	this->m_cameraData.fMinDistance;
			else if(fNewR>this->m_cameraData.fMaxDistance)
				fNewR	=	m_cameraData.fMaxDistance;
			m_fSaveDistance =	fNewR;
			vDir		*=	(fNewR/fR);
			//
			vEye		=	m_cameraData.vTarget + vDir;
			vTarget		=	m_cameraData.vTarget;
		}
		m_cameraData.vEye		=	vEye;
		m_cameraData.vTarget	=	vTarget;
		//if(ISCameraEnable(FPSMode))
		//{
		//	float3 vDir	=	m_cameraData.vTarget - m_cameraData.vEye;
		//	vDir.normalize();
		//	m_cameraData.vTarget = (m_cameraData.vEye + vDir);
		//}
		this->SetViewMatrix();
	}

	void	Camera::rebuildViewRot()
	{CPUTime(Camera);
		if(this->ISCameraEnable(XRotation) || ISCameraEnable(YRotation))
		{
			float3 vDir;
			if(ISCameraEnable(FPSMode))
				vDir = m_cameraData.vTarget - m_cameraData.vEye;
			else
				vDir = m_cameraData.vEye-m_cameraData.vTarget;
			float fR = vDir.R();
			float3 vY(0.0f,-fR,0.0f);

			if(!this->ISCameraEnable(LeftHand))
			{
				if(this->m_cameraData.vUp.y>0.01f)
				{
					vY.RotateX(vY,-m_cameraData.vRotate.x);
					vY.RotateY(vY,-m_cameraData.vRotate.z);
				}
				else if(this->m_cameraData.vUp.z>0.1f)
				{
					vY.RotateX(vY,-m_cameraData.vRotate.x);
					vY.RotateZ(vY,m_cameraData.vRotate.z);
				}
			}
			else
			{
				if(this->m_cameraData.vUp.y>0.01f)
				{
					vY.setxyz(0,0,fR);
					vY.RotateX(vY,-m_cameraData.vRotate.x);
					vY.RotateY(vY,-m_cameraData.vRotate.z);
				}
				else if(this->m_cameraData.vUp.z>0.1f)
				{
					vY.RotateX(vY,-m_cameraData.vRotate.x);
					vY.RotateZ(vY,-m_cameraData.vRotate.z);
				}
			}
			if(ISCameraEnable(FPSMode))
			{
				float3 vTarget = m_cameraData.vEye + vY;
				this->SetViewMatrix(0, &vTarget);
			}
			else
			{
				float3 vEye = m_cameraData.vTarget + vY;
				this->SetViewMatrix(&vEye);
			}
		}
	}

	void	Camera::returnSaveRot()
	{CPUTime(Camera);
		if(!m_bReturnRotate || true)
			return;
		float3 v = m_vLookatDest-this->m_cameraData.vRotate;
		if(!this->ISCameraEnable(ReturnXRot) || !this->ISCameraEnable(ReturnYRot) || !this->ISCameraEnable(ReturnZRot))
			return;
		if(!this->ISCameraEnable(ReturnXRot))
			v.x=0;
		if(!this->ISCameraEnable(ReturnYRot))
			v.y=0;
		if(!this->ISCameraEnable(ReturnZRot))
			v.z=0;
		//
		//v=v*0.99999f;
		float fr=v.R();
		float3 vDest;
		float fd = PI/100.0f;
		if(fr<fd)
		{
			m_bReturnRotate = false;
			vDest=this->m_vLookatDest;
		}
		else
		{
			fr=v.R();
			v*=(fd/fr);
			vDest=this->m_cameraData.vRotate+v;
		}
		if(this->ISCameraEnable(ReturnXRot))
			m_cameraData.vRotate.x=vDest.x;
		if(this->ISCameraEnable(ReturnYRot))
			m_cameraData.vRotate.y=vDest.y;
		if(this->ISCameraEnable(ReturnZRot))
			m_cameraData.vRotate.z=vDest.z;
		else
		{
			return;
		}
		//
		this->rebuildViewRot();
	}
	BOOL	Camera::IsFPSMode()
	{CPUTime(Camera);
		return ISCameraEnable(FPSMode);
	}
	VOID	Camera::SetFPSMode(BOOL b)
	{CPUTime(Camera);
		SetCameraEnable(FPSMode, b);
	}
	VOID	Camera::OnPlayNext()
	{CPUTime(Camera);
		if(this->m_nexts.size()==0){
			m_bPlay = false;
			if(m_funcPlayOver.t[0]){
				lua_tinker::call<void>(g_luamanager.GetLuaState(), m_funcPlayOver.t, m_funcParam);
			}
			return;
		}
		m_currPlay = m_nexts[0];
		m_nexts.eraseByIndex(0);
		m_currPlayX=m_currPlayY=0;
	}
	VOID	Camera::Stop()
	{CPUTime(Camera);
		m_nexts.clear();
		m_bPlay = false;
		m_ditherOffset.setxyz(0, 0, 0);
	}
	VOID	Camera::Play()
	{CPUTime(Camera);
		m_bMoveMode = false;
		if(m_nexts.size()==0)return;
		this->m_bPlay = true;
		OnPlayNext();
	}
	BOOL	ToNear(float& inout, float fTarget, float fOffset){
		BOOL bOver = false;
		if(fTarget>inout){
			float fD = fTarget - inout;
			if(fD<fOffset){
				bOver = true;
			}
			else{
				inout = inout + fOffset;
			}
		}
		else{
			float fD = inout - fTarget;
			if(fD<fOffset){
				bOver = true;
			}
			else{
				inout = inout - fOffset;
			}
		}
		return bOver;
	}
	void	Camera::OnFrameMove(float fElapsedTime)
	{CPUTime(Camera);
		if(m_bPlay){//如果播放了动作
			BOOL bOver = false;
			switch(m_currPlay.type){
			case CameraAni::CAT_Move:
				{
					float3 pos(m_currPlay.v.x, m_currPlay.v.y, m_currPlay.v.z);
					float3 dir = pos - m_cameraData.vTarget;
					float fMove = fElapsedTime * m_currPlay.v.w;
					float fLength = dir.length();
					float3 olddir = m_cameraData.vEye - m_cameraData.vTarget;
					if(fLength<fMove){
						bOver = true;
						m_cameraData.vTarget = pos;
					}
					else{
						m_cameraData.vTarget = m_cameraData.vTarget + dir*(fMove/fLength);
					}
					m_cameraData.vEye = m_cameraData.vTarget + olddir;
					this->SetViewMatrix();
				}
				break;
			case CameraAni::CAT_Dist:
				{
					float fDistance = m_currPlay.v.x;
					float fMove = fElapsedTime * m_currPlay.v.w;
					float3 dir = m_cameraData.vEye - m_cameraData.vTarget;
					float fLength = dir.length();
					dir.normalize();
					bOver = ToNear(fLength, fDistance, fMove);
					m_cameraData.vEye = m_cameraData.vTarget + dir*fLength;
					this->SetViewMatrix();
				}
				break;
			case CameraAni::CAT_Dither:
				{
					m_currPlay.v.w -= fElapsedTime;
					if(m_currPlay.v.w<=0){
						bOver = true;
						m_ditherOffset.setxyz(0, 0, 0);
					}
					else{
						m_ditherOffset.setxyz(RandomF(-m_currPlay.v.x,m_currPlay.v.x),RandomF(-m_currPlay.v.x,m_currPlay.v.x),0);
					}
					this->SetViewMatrix();
				}
				break;
			case CameraAni::CAT_Rot:
				{
					float2 rot(m_currPlay.v.x,m_currPlay.v.y);
					float fRotSpeed = fElapsedTime * m_currPlay.v.w;
					if(!m_currPlayX)
						m_currPlayX = ToNear(m_cameraData.vRotate.z, m_currPlay.v.x, fRotSpeed);
					if(!m_currPlayY)
						m_currPlayY = ToNear(m_cameraData.vRotate.y, m_currPlay.v.y, fRotSpeed);
					if(m_currPlayX&&m_currPlayY)
						bOver = true;
					SetRotation(float2(m_cameraData.vRotate.x,m_cameraData.vRotate.z));
				}
				break;
			default:
				{
					Stop();
					break;
				}
			}
			m_bPlay = true;
			if(bOver)
				this->OnPlayNext();
		}
		else{
			returnSaveRot();
			if(m_fMaxDitherOffset > 0.0f)
			{
				m_fMaxDitherOffset	-=	fElapsedTime;
				if(m_fMaxDitherOffset <= 0.0f)
					m_ditherOffset.setxyz(0, 0, 0);
				else
					m_ditherOffset	*=	m_fMaxDitherOffset;
				SetViewMatrix();
			}
			if(!this->ISCameraEnable(EnableMove))
				return;
			//
			float vDist	=	To3DValue(1.0f) * fElapsedTime;
			float3 vMove(0,0,0);
			//
			float3 vOffset(0,0,0);
			float3 vDir = this->m_cameraData.vTarget - this->m_cameraData.vEye;
			if(ISCameraEnable(FixedZAxis))
				vDir.z	=	0.0f;
			this->OnMoveMode();
		}
#ifndef EDITOR_OS
		if(m_rayCamera&&GetGameMgr()->GetScenePtr()){
			float3 dir = m_cameraData.vEye - m_cameraData.vTarget;
			float fLength = dir.length();
			dir.normalize();
			this->m_intersect.begin(m_cameraData.vTarget, dir, fLength);
			if(GetGameMgr()->GetScenePtr()->IntersectStatic(&m_intersect)){
				float3 vEye = m_cameraData.vTarget + dir * m_intersect.GetPickInfo().fDist;
				float3 vOldEye = m_cameraData.vEye;
				SetViewMatrix(&vEye);
				m_cameraData.vEye = vOldEye;
			}
		}
#endif
		//if(ISCameraEnable(FPSMode))
		//{
		//	vDir.normalize();
		//	vDir	*=	To3DValue(1.0f);
		//}
	}
	void	Camera::SetRotation(const float2& vrot)
	{CPUTime(Camera);
		m_cameraData.vRotate.x=vrot.x;
		m_cameraData.vRotate.z=vrot.y;
		float3 vDir = m_cameraData.vEye-m_cameraData.vTarget;
		float fR = vDir.R();
		float3 vY(0.0f,-fR,0.0f);
		if(!this->ISCameraEnable(LeftHand))
		{
			vY.RotateX(vY,-m_cameraData.vRotate.x);
			vY.RotateZ(vY,m_cameraData.vRotate.z);
		}
		else
		{
			vY.RotateX(vY,-m_cameraData.vRotate.x);
			vY.RotateZ(vY,-m_cameraData.vRotate.z);
		}
		float3 vEye = m_cameraData.vTarget + vY;
		this->SetViewMatrix(&vEye);
	}
	VOID	Camera::GetCameraOffset(float3* xdirRet, float3* ydirRet, BOOL bFixedZAxis)
	{CPUTime(Camera);
		float3 vDir = this->m_cameraData.vTarget - this->m_cameraData.vEye;
		if(bFixedZAxis)
			vDir.z	=	0.0f;
		vDir.normalize();
		if(ydirRet)
			*ydirRet = vDir;
		vDir=vDir.cross(float3(0,0,1));
		if(xdirRet)
			*xdirRet = vDir;
	}
	VOID	Camera::Offset(const float3& off)
	{CPUTime(Camera);
		m_cameraData.vTarget+=off;
		m_cameraData.vEye+=off;
		this->SetViewMatrix();
	}

	BOOL	Camera::MoveCamera2D(float offsetX, float offsetY)
	{CPUTime(Camera);
		if(this->ISCameraEnable(EnableMove))
		{
			float fDistance	=	this->m_cameraData.fMoveSpeed;
			//if(ISCameraEnable(FPSMode))
			//	fDistance	=	To3DValue(1.0f) * this->m_cameraData.fMoveSpeed;
			//
			float xMove=fDistance*offsetX/100.0f;
			float yMove=fDistance*offsetY/100.0f;
			matrix4x4 matOffset;
			matOffset.identity();
			if(ISCameraEnable(MoveHeightIsZ))
				matOffset.SetPosition(float3(-xMove,0, -yMove));
			else
				matOffset.SetPosition(float3(xMove,-yMove,0));
			matrixMultiply(&matOffset,&matOffset,&m_matViewInverse);
			float3 vDir=matOffset.GetPosition() - m_matViewInverse.GetPosition();
			if(ISCameraEnable(FixedZAxis))
				vDir.z	=	0.0f;
			this->m_cameraData.vTarget	+=	vDir;
			this->m_cameraData.vEye	+=	vDir;
			this->rebuildViewRot();
			this->SetViewMatrix();
			return true;
		}
		return false;
	}

	BOOL	Camera::RotationCamera2D(float offsetX, float offsetY)
	{CPUTime(Camera);
		if(this->IsFPSMode()){
			offsetY = -offsetY;
		}
		BOOL bSeted	=	false;
		float fRotateZ	=	offsetX	/	500.0f	*	PI;//每500个像素为一个半径转度.
		float fRotateX	=	offsetY	/	500.0f	*	PI;
		fRotateX *= m_cameraData.fRotationSpeedMask;
		fRotateZ *= m_cameraData.fRotationSpeedMask;
		if(ISCameraEnable(YRotation))
		{
			m_cameraData.vRotate.z+=fRotateZ;
			bSeted	=	true;
		}
		if(ISCameraEnable(XRotation))
		{
			const float abs_pi	=	PI / 2.1f;
			m_cameraData.vRotate.x	+=	fRotateX;
			if(m_cameraData.vRotate.x > abs_pi)
				m_cameraData.vRotate.x = abs_pi;
			else if(m_cameraData.vRotate.x < -abs_pi)
				m_cameraData.vRotate.x = -abs_pi;
			bSeted	=	true;
		}
		if(bSeted)
			this->rebuildViewRot();
		return bSeted;
	}

	void	Camera::SetCameraRotate(const float3& vrot,BOOL bret)
	{CPUTime(Camera);
		m_vLookatDest.setxyz(fmodf(vrot.x, PI_X2), fmodf(vrot.y, PI_X2), fmodf(vrot.z, PI_X2));
		m_cameraData.vRotate.x	=	fmodf(m_cameraData.vRotate.x, PI_X2);
		m_cameraData.vRotate.y	=	fmodf(m_cameraData.vRotate.y, PI_X2);
		m_cameraData.vRotate.z	=	fmodf(m_cameraData.vRotate.z, PI_X2);
		if(!bret)
			this->m_cameraData.vRotate = this->m_vLookatDest;
		else
		{
			float3 dir	=	m_vLookatDest - m_cameraData.vRotate;
			//
			if(dir.x > PI)
				m_vLookatDest.x		=	m_vLookatDest.x - PI_X2;
			else if( dir.x < - PI )
				m_vLookatDest.x		=	m_vLookatDest.x + PI_X2;
			//
			if(dir.y > PI)
				m_vLookatDest.y		=	m_vLookatDest.y - PI_X2;
			else if( dir.y < - PI )
				m_vLookatDest.y		=	m_vLookatDest.y + PI_X2;
			//
			if(dir.z > PI)
				m_vLookatDest.z		=	m_vLookatDest.z - PI_X2;
			else if( dir.z < - PI )
				m_vLookatDest.z		=	m_vLookatDest.z + PI_X2;
		}
		m_bReturnRotate=bret;
		this->rebuildViewRot();
	}
	//
	void	Camera::SetCameraRotateEnable(BOOL bx,BOOL by,BOOL bz)
	{CPUTime(Camera);
		SetCameraEnable(ReturnXRot,bx);
		SetCameraEnable(ReturnYRot,by);
		SetCameraEnable(ReturnZRot,bz);
	}
	//

	void	Camera::SetUpper(const float3& vUp)
	{CPUTime(Camera);
		SetViewMatrix(0, 0, &vUp);
	}
	
	void	Camera::SetFov(float f)
	{CPUTime(Camera);
		m_cameraData.fFov = f;
		if(ISCameraEnable(OrthoMode))
			this->SetOrthoProjection( m_cameraData.nWidth, m_cameraData.nHeight, m_cameraData.fZNear, m_cameraData.fZFar);
		else
			this->SetPerspectiveProjection( m_cameraData.fFov, m_cameraData.nWidth, m_cameraData.nHeight, m_cameraData.fZNear, m_cameraData.fZFar);
	}
	void	Camera::SetWidth(int n)
	{CPUTime(Camera);
		m_cameraData.nWidth = n;
		onChangeView();
	}
	void	Camera::SetHeight(int n)
	{CPUTime(Camera);
		m_cameraData.nHeight = n;
		onChangeView();
	}
	void	Camera::SetZNearest(float f)
	{CPUTime(Camera);
		m_cameraData.fZNear = f;
		onChangeView();
	}
	void	Camera::SetZFar(float f)
	{CPUTime(Camera);
		m_cameraData.fZFar = f;
		onChangeView();
	}
	float				getRotateX(const float3& _dir)
	{
		float fr = sqrtf(_dir.x*_dir.x + _dir.z*_dir.z + _dir.y*_dir.y);
		if(fr<0.00001f)
			return 0.0f;
		return asinf(-_dir.z/fr);//*(asinf(_dir.z/fr) > 0.0f?-1.0f:1.0f);
	}

	void	Camera::recalcRot(){
		float3 vDir;
		if(ISCameraEnable(FPSMode))
			vDir = m_cameraData.vTarget - m_cameraData.vEye;
		else
			vDir = m_cameraData.vEye-m_cameraData.vTarget;
		this->m_cameraData.vRotate.z = -getRotateZ(vDir);
		this->m_cameraData.vRotate.x = getRotateX(vDir);
	}
	void	Camera::SetTarget(const float3& vNewTargetPos)
	{CPUTime(Camera);
		float3 vDir = m_cameraData.vEye-vNewTargetPos;
		m_fSaveDistance = vDir.R();
		this->SetViewMatrix(&m_cameraData.vEye, &vNewTargetPos);
		recalcRot();
	}

	void	Camera::SetEye(const float3& eye)
	{CPUTime(Camera);
		float3 vDir = eye - m_cameraData.vTarget;
		m_fSaveDistance = vDir.R();
		this->SetViewMatrix(&eye);
		recalcRot();
	}
	const matrix4x4*	Camera::GetBoardWorld(int nBoard)	//返回MatrixBoard_Billbard，。。。
	{CPUTime(Camera);
		if(nBoard < 0 || nBoard >= BOARD_COUNT)return 0;
		return &m_matBoards[nBoard];
	}

	CamFrustumPlane::point_side CamFrustumPlane::get_side(float3& p)
	{CPUTime(CamFrustumPlane);
		float fdist = p.dot(n)+d;
		if ( fdist > 0.0 )
			return CamFrustumPlane::NEGATIVE_SIDE;

		if ( fdist < 0.0 )
			return CamFrustumPlane::POSITIVE_SIDE;

		return CamFrustumPlane::NO_SIDE;
	}
	void	CamFrustumPlane::setup(float3& v1,float3& v2,float3& v3,float3& v4,unsigned int _color)
	{CPUTime(CamFrustumPlane);
		p[0] = v1;
		p[1] = v2;
		p[2] = v3;
		p[3] = v4;

		color = _color;
		//
		float3 vv1 = p[0] - p[1];
		float3 vv2 = p[2] - p[1];
		n = vv1.cross(vv2);
		d = n.dot(p[1]);
		n.normalv(n);
		Plane plane(v2, v1, v3);
		n.x = plane.a;
		n.y = plane.b;
		n.z = plane.c;
		d = plane.d;
	}

	BOOL	Camera::IsAABoxVisibleTB(const PBoxVertices& vbox)
	{CPUTime(Camera);
		for(int i=4;i<6;i++)
		{
			BOOL bPointOutFrustum = true;
			for(int ibox = 0;ibox<8;ibox++)
			{
				float fdist = vbox.v[ibox].dot(m_plane[i].n)+m_plane[i].d;
				if(fdist<0.0f)
				{
					bPointOutFrustum = false;
					break;
				}
			}
			if(bPointOutFrustum)//如果所有点在这个平面的外面则返回false
				return false;
		}
		return true;
	}
	//
	BOOL	Camera::IsAABoxVisibleLRNF(const PBoxVertices& vbox)
	{CPUTime(Camera);
		for(int i=0;i<4;i++)
		{
			BOOL bPointOutFrustum = true;
			for(int ibox = 0;ibox<8;ibox++)
			{
				float fdist = vbox.v[ibox].dot(m_plane[i].n)+m_plane[i].d;
				if(fdist<0.0f)
				{
					bPointOutFrustum = false;
					break;
				}
			}
			if(bPointOutFrustum)//如果所有点在这个平面的外面则返回false
				return false;
		}
		return true;
	}

	BOOL	Camera::IsPlaneVisible(const float3& p0, const float3& p1, const float3& p2, const float3& p3, BOOL bCCW)
	{CPUTime(Camera);
		const float3& eye = this->m_cameraData.vEye;
		float3 cdir = p0 - eye;
		float3 e1 = p1 - p0;
		float3 e2 = p2 - p0;
		float3 en = e2.cross(e1);
		if(cdir.dot(en) < 0)
		{
			if(bCCW)
				return false;
		}
		const float3* p[4] = {&p0, &p1, &p2, &p3};

		for(int i=0;i<6;i++)
		{
			BOOL bPointOutFrustum = true;
			for(int ibox = 0;ibox<4;ibox++)
			{
				float fdist = p[ibox]->dot(m_plane[i].n)+m_plane[i].d;
				if(fdist<0.0f)
				{
					bPointOutFrustum = false;
					break;
				}
			}
			if(bPointOutFrustum)//如果所有点在这个平面的外面则返回false
				return false;
		}
		return true;
	}

	BOOL	Camera::IsAABoxVisible(const PBoxVertices& vbox, PlaneVisible* _out)
	{CPUTime(Camera);
		if(_out)
		{
			BOOL bret = true;
			for(int i=0;i<6;i++)
			{
				BOOL bPointOutFrustum = true;
				for(int ibox = 0;ibox<8;ibox++)
				{
					float fdist = vbox.v[ibox].dot(m_plane[i].n)+m_plane[i].d;
					if(fdist<0.0f)
					{
						bPointOutFrustum = false;
						break;
					}
				}
				_out->bVisible[i] = !bPointOutFrustum;
				if(bPointOutFrustum)//如果所有点在这个平面的外面则返回false
					bret = false;
			}
			return bret;
		}
		else
		{
			for(int i=0;i<6;i++)
			{
				BOOL bPointOutFrustum = true;
				for(int ibox = 0;ibox<8;ibox++)
				{
					float fdist = vbox.v[ibox].dot(m_plane[i].n)+m_plane[i].d;
					if(fdist<0.0f)
					{
						bPointOutFrustum = false;
						break;
					}
				}
				if(bPointOutFrustum)//如果所有点在这个平面的外面则返回false
					return false;
			}
		}
		return true;
	}
	BOOL	Camera::IsAABoxVisible(const AABox& vbox)
	{CPUTime(Camera);
		static PBoxVertices _aabox;
		_aabox.setup(vbox._min, vbox._max);
		return IsAABoxVisible(_aabox);
	}

	BOOL	Camera::IsSphereVisible(const float3& p,float radius)
	{CPUTime(Camera);
		for (unsigned int i = 0; i < 6; i++)
		{
			if(p.dot(m_plane[i].n)+m_plane[i].d>=radius)
				return false;//pos.dot(planes[i].normal) + planes[i].offset <= -radius) return false;
		}
		return true;
	}
	BOOL	Camera::IsPointVisible(const float3& p)
	{CPUTime(Camera);
		for(int i=0;i<6;i++)
			if((p.dot(m_plane[i].n)+m_plane[i].d)>0)//m_plane[i].is_visible(p))
				return false;
		return true;
	}

	void	Camera::setupFrustum()
	{CPUTime(Camera);
		int w = this->m_cameraData.nWidth;
		if(g_manager.IsVR3DViewer())w*=2;
		setup(this->m_matView,this->m_cameraData.fFov, -fabsf(this->m_cameraData.fZNear),-fabsf(this->m_cameraData.fZFar), w, this->m_cameraData.nHeight);
	}

	void	Camera::setup(matrix4x4& view,float fov,float fZNear,float fZFar,int wndWidth,int wndHeight)
	{CPUTime(Camera);
		if(ISCameraEnable(OrthoMode))
		{
			float3 orig00, orig10, orig01, orig11, dir;
			this->Get3DFrom2D(m_near[pad_lt], dir, Pixel(0, 0));
			this->Get3DFrom2D(m_near[pad_rt], dir, Pixel(wndWidth, 0));
			this->Get3DFrom2D(m_near[pad_lb], dir, Pixel(0, wndHeight));
			this->Get3DFrom2D(m_near[pad_rb], dir, Pixel(wndWidth, wndHeight));
			//
			dir				*=	fZFar;
			m_far[pad_rt]	=	m_near[pad_rt] - dir;
			m_far[pad_rb]	=	m_near[pad_rb] - dir;
			m_far[pad_lt]	=	m_near[pad_lt] - dir;
			m_far[pad_lb]	=	m_near[pad_lb] - dir;
		}
		else
		{
			float fr = atanf(fov/2.0f);
			float ff = fov - (PI / 4.0f);
			fr *= (m_fFrustumMul + (ff > 0.0f ? ff / PI : 0.0f));
			float fAccept = (float)wndWidth/(float)wndHeight;
			float nx = fabsf(fZNear*fr*fAccept);
			float ny = fabsf(fZNear*fr);
			float nz = fZNear;
			float fx = fabsf(fZFar*fr*fAccept);
			float fy = fabsf(fZFar*fr);
			float fz = fZFar;
			//
			m_near[pad_rt].setxyz(nx,ny,nz);
			m_near[pad_rb].setxyz(nx,-ny,nz);
			m_near[pad_lt].setxyz(-nx,ny,nz);
			m_near[pad_lb].setxyz(-nx,-ny,nz);
			//
			m_far[pad_rt].setxyz(fx,fy,fz);
			m_far[pad_rb].setxyz(fx,-fy,fz);
			m_far[pad_lt].setxyz(-fx,fy,fz);
			m_far[pad_lb].setxyz(-fx,-fy,fz);
			if(!ISCameraEnable(OrthoMode))
			{
				matrix4x4& inv = this->m_matViewInverse;
				for(int i=0;i<4;i++)
				{
					m_near[i].transform(m_near[i],inv);
					m_far[i].transform(m_far[i],inv);
				}
			}
		}
		m_plane[PlaneVisible::plane_left].setup(m_near[pad_rt],m_far[pad_rt],m_far[pad_rb],m_near[pad_rb],color4(1,0,0,1));
		m_plane[PlaneVisible::plane_right].setup(m_near[pad_lb],m_far[pad_lb],m_far[pad_lt],m_near[pad_lt],color4(0,1,0,1));
		//
		m_plane[PlaneVisible::plane_top].setup(m_near[pad_lt],m_far[pad_lt],m_far[pad_rt],m_near[pad_rt],color4(1,0,0,1));
		m_plane[PlaneVisible::plane_bottom].setup(m_far[pad_lb],m_near[pad_lb],m_near[pad_rb],m_far[pad_rb],color4(0,1,0,1));
		//
		m_plane[PlaneVisible::plane_near].setup(m_near[pad_lt],m_near[pad_rt],m_near[pad_rb],m_near[pad_lb],color4(1,0,0,1));
		m_plane[PlaneVisible::plane_far].setup(m_far[pad_rt],m_far[pad_lt],m_far[pad_lb],m_far[pad_rb],color4(1,0,0,1));
		//
	}

	BOOL	Camera::GetCameraConfig(CameraDataLocal& _out)
	{CPUTime(Camera);
		_out	=	m_cameraData;
		return true;
	}

	BOOL	Camera::SetCameraConfig( const CameraDataLocal& _in)
	{CPUTime(Camera);
		this->m_cameraData		=	_in;
		m_cameraData.nWidth		=	g_manager.GetBufferWidth();
		m_cameraData.nHeight	=	g_manager.GetBufferHeight();
		m_cameraData.fFov		=	_in.fFov;// 0.75f;
		SetDistance(this->GetDistance());
		this->rebuildViewRot();
		if(ISCameraEnable(OrthoMode))
			this->SetOrthoProjection( m_cameraData.nWidth, m_cameraData.nHeight, m_cameraData.fZNear, m_cameraData.fZFar);
		else
			this->SetPerspectiveProjection( m_cameraData.fFov, m_cameraData.nWidth, m_cameraData.nHeight, m_cameraData.fZNear, m_cameraData.fZFar);
		return true;
	}

	void	Camera::SetClipCamera(const float3& vMin, const float3& vMax)
	{CPUTime(Camera);
		this->m_clipMax	=	vMax;
		this->m_clipMin	=	vMin;
	}

	void	Camera::GetClipCamera(float3& vMin, float3& vMax)
	{CPUTime(Camera);
		vMin	=	m_clipMin;
		vMax	=	m_clipMax;
	}

	IntersectData*				Camera::BeginIntersect(int x, int y)
	{CPUTime(Camera);
		if(!this->m_intersect.begin(this, x, y))// * (float)g_manager.GetUIScale().x, y * (float)g_manager.GetUIScale().y))
			return NULL;
		return &m_intersect;
	}


	PixelLinear::PixelLinear(void)
	{
		m_numPick = 0;
	}

	PixelLinear::~PixelLinear(void)
	{
	}

	unsigned int		PixelLinear::Pick(float gridW, float gridH, float x, float y, float xto, float yto)
	{CPUTime(PixelLinear);
		m_numPick = 0;
		int beginX = (int)(x/gridW);
		int beginY = (int)(y/gridH);
		int	endX = (int)(xto/gridW);
		int endY = (int)(yto/gridH);
		//
		//上下
		if(beginX == endX)
		{
			if(beginY<=endY)
			{
				for(int i = beginY;i<=endY;i++)
					if(!Push(beginX,i))
						return m_numPick;
			}
			else
			{
				for(int i = beginY;i>=endY;i--)
					if(!Push(beginX,i))
						return m_numPick;
			}
			return m_numPick;
		}
		//左右
		if(beginY == endY)
		{
			if(beginX<=endX)
			{
				for(int i = beginX;i<=endX;i++)
					if(!Push(i,beginY))
						return m_numPick;
			}
			else
			{
				for(int i = beginX;i>=endX;i--)
					if(!Push(i,beginY))
						return m_numPick;
			}
			return m_numPick;
		}
		//
		if(xto>x)
		{
			float ftangent = (yto - y)/(xto - x);
			////////
			{
				float yClipBegin = (float)beginY*gridH;
				int xClipBegin = beginX;
				float fYClipEnd = (float)endY*gridH;
				for(int i=beginX+1;i<=endX+1;i++)
				{
					float xClipEnd= (float)i * gridW;
					float yClipEnd = y + (xClipEnd - x)*ftangent;
					//
					if(i==endX+1)
						yClipEnd = fYClipEnd;
					////
					int yBegin = (int)(yClipBegin/gridH);
					int yEnd = (int)(yClipEnd/gridH);
					//
					if(yEnd>yBegin)
					{
						for(int iy = yBegin;iy<=yEnd;iy++)
							if(!Push(xClipBegin,iy))
								return m_numPick;
					}
					else
						for(int iy = yEnd;iy<=yBegin;iy++)
							if(!Push(xClipBegin,iy))
								return m_numPick;
					//
					yClipBegin = yClipEnd;
					xClipBegin = i;
					//
				}
				//
			}
		}
		else
		{
			float ftangent = (yto - y)/(xto - x);
			////////
			{
				float yClipBegin = (float)beginY*gridH;
				int xClipBegin = beginX;
				float fYClipEnd = (float)endY*gridH;
				for(int i=beginX;i>=endX;i--)
				{
					float xClipEnd= (float)i * gridW;
					float yClipEnd = y + (xClipEnd - x)*ftangent;
					//
					if(i==endX)
						yClipEnd = fYClipEnd;
					////
					int yBegin = (int)(yClipBegin/gridH);
					int yEnd = (int)(yClipEnd/gridH);
					//
					if(yEnd>yBegin)
					{
						for(int iy = yBegin;iy<=yEnd;iy++)
							if(!Push(i,iy))
								return m_numPick;
					}
					else
						for(int iy = yEnd;iy<=yBegin;iy++)
							if(!Push(i,iy))
								return m_numPick;
					//
					yClipBegin = yClipEnd;
					xClipBegin = i;
					//
				}
				//
			}
		}
		//
		return m_numPick;
	}

};
