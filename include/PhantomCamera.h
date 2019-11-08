//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_CAMERA_H__
#define	__PHANTOM_CAMERA_H__
#include "PhantomBase.h"

namespace Phantom{

	struct OrientationEKF
	{
		matrix3x3d		so3SensorFromWorld,so3LastMotion,mP,mQ,mR,mRaccel,mS,mH,mK;
		double3			mNu,mz,mh,mu,mx,down,north;
		long			sensorTimeStampGyro;
		long			sensorTimeStampAcc;
		long			sensorTimeStampMag;
		float			lastGyro[3];
		float			filteredGyroTimestep;
		BOOL			timestepFilterInit;
		int				numGyroTimestepSamples;
		BOOL			gyroFilterValid;
		double			m_dUpdateTime;
		OrientationEKF();
		void reset();
		void sO3FromTwoVec(const double3& a, const double3& b, matrix3x3d& result);
		void rotationPiAboutAxis(const double3& v, matrix3x3d& result);
		void sO3FromMu(const double3& w, matrix3x3d& result) const;
		void muFromSO3(const matrix3x3d& so3, double3& result);
		void rodriguesSo3Exp(const double3& w, double kA, double kB, matrix3x3d& result)const;
		void generatorField(int i, const matrix3x3d& pos, matrix3x3d& result);
		BOOL	isReady();
		matrix4x4 getPredictedGLMatrix(double secondsAfterLastGyroEvent) const;
		matrix4x4 glMatrixFromSo3(const matrix3x3d& s) const;
		void processGyro(float* gyro, long sensorTimeStamp);
		void processAcc(float* acc, long sensorTimeStamp);
		void filterGyroTimestep(float timeStep);
		void updateCovariancesAfterMotion();
		void accObservationFunctionForNumericalJacobian(const matrix3x3d& so3SensorFromWorldPred, double3& result);
		void magObservationFunctionForNumericalJacobian(const matrix3x3d& so3SensorFromWorldPred, double3& result);
		void arrayAssign(matrix4x4& data, matrix3x3d& m);
	};

	#define	MAX_DIRECTION					1.0f//10000000.0f
	#define	DIST_MAX						10000000.0f
#define	MAX_MOVE_SPEED				2//保存最近三个数据
	class Camera;
	struct PickInfoData//点击后的数据返回
	{
		float			fDist;
		float3			vPick;
		float3			vFaceNormal;
		float			fU;
		float			fV;
		int				iObjectType;
		Name			name;
		const char*		strType;
		void*			pData;
		PickInfoData(){}
		PickInfoData(float _d, const float3& _Pick, float _fU, float _fV, const char* _type, void* _data, int iObjT, const float3* pNormal = 0)
		{
			if(pNormal)
				vFaceNormal	=	*pNormal;
			else
				vFaceNormal.setxyz(0,0,0);
			fDist	= _d; strType = _type;pData = _data; iObjectType = iObjT;
			vPick	= _Pick;
			fU		= _fU;
			fV		= _fV;
		}
	};
	/*STRUCT*/struct	PickPoint/*碰撞数据*/
	{
		float3		pos;
		float3		normal;
		float		t;
		/*END_STRUCT*/
		PickPoint(){pos.setxyz(0,0,0); normal.setxyz(0,0,0); t=0;}
	};
	class IntersectData
	{
	public:
		BOOL					begin(const matrix4x4& view, const matrix4x4& proj, int x, int y, const float3& target, const float3& eye, float fMaxDistance, BOOL bIsFPSMode, BOOL bOrtho = false);
		BOOL					begin(Camera* _camera, int x, int y);
		BOOL					begin(const float3& vOrig, const float3& vDir, float fMaxDistance);
		//
		BOOL					IsPicked();
		BOOL					PushPickInfo(const PickInfoData& _info);
		const PickInfoData&		GetPickInfo(){return m_info;}
		float					GetMaxWheelDistance(){return m_fMaxDistance;}
		//
		const float3&			GetOriginal(){return m_vOrig;}
		const float3&			GetDir(){return m_vDir;}
		const PDirection3D&		GetDirection(){return m_lineRay;}
	public:
		PickInfoData			m_info;
		//
		IntersectData(void);
		~IntersectData(void);
	protected:
		float3					m_vOrig;
		float3					m_vDir;
		Pixel					m_lastPixel;
		//
		PDirection3D			m_lineRay;
		BOOL					m_picked;
		//
		float					m_fMaxDistance;
		//
		void					reset_pick();
	};

	struct	PlaneVisible//可见面检测数据
	{
		BOOL	bVisible[6];
		enum{
			plane_left = 0,
			plane_right = 1,
			plane_near = 2,
			plane_far = 3,
			//
			plane_top = 4,
			plane_bottom = 5,
		};
	};

	/*ENUM*/enum MouseButton/*鼠标按钮类型*/
	{
		MouseButton_None	/*鼠标按钮类型_无*/=	0,
		MouseButton_Left	/*鼠标按钮类型_左键*/=	1,
		MouseButton_Right	/*鼠标按钮类型_右键*/=	2,
		MouseButton_Middle	/*鼠标按钮类型_中键*/=	3,
	};

	/*ENUM*/enum CameraEnable/*镜头开关*/
	{
		CameraEnable_XRotation				/*镜头开关XRotation*/	=	0x00000001,	//bIsXRotEnable
		CameraEnable_YRotation				/*镜头开关YRotation*/	=	0x00000002,	//bIsYRotEnable
		CameraEnable_FPSMode				/*镜头开关FPSMode*/	=	0x00000004,	//bIsFPSMode
		CameraEnable_OrthoMode				/*镜头开关OrthoMode*/	=	0x00000008,	//bIsOrtho
		CameraEnable_CursorClipInRotMove	/*镜头开关CursorClipInRotMove*/	=	0x00000010,	//当转动的时候是否进行自动复位操作,如果存在则会自动隐藏鼠标
		CameraEnable_EnableMove				/*镜头开关EnableMove*/	=	0x00000020,	//bEnableMove
		CameraEnable_ReturnXRot				/*镜头开关ReturnXRot*/	=	0x00000040,	//bIsReturnXRot
		CameraEnable_ReturnYRot				/*镜头开关ReturnYRot*/	=	0x00000080,	//bIsReturnYRot
		CameraEnable_ReturnZRot				/*镜头开关ReturnZRot*/	=	0x00000100,	//bIsReturnZRot
		CameraEnable_RotationSpeedEnable	/*镜头开关RotationSpeedEnable*/	=	0x00000200,	//bIsRotationSpeedEnable
		CameraEnable_CenterCross			/*镜头开关CenterCross*/	=	0x00000400,	//是否在中间显示鼠标，而不自动跟着鼠标走，也就是FPS的准心
		CameraEnable_EnableDistance			/*镜头开关EnableDistance*/	=	0x00000800,	//bEnableDistance
		CameraEnable_LeftHand				/*镜头开关LeftHand*/	=	0x00001000,	//bIsLeftHand
		CameraEnable_ShiftMoveMul			/*镜头开关ShiftMoveMul*/	=	0x00002000,	//Shift加速移动
		CameraEnable_MoveHeightIsZ			/*镜头开关MoveHeightIsZ*/	=	0x00004000,	//移动Y值在3D中是Z轴
		CameraEnable_FixedZAxis				/*镜头开关Z轴固定*/		=	0x00008000,	//
	};

	/*STRUCT*/struct	CameraData/*镜头数据*/
	{
		int												flags/*常数_镜头开关*/;
		MouseButton										rotButton;	//转动按钮
		MouseButton										moveButton;	//移动按钮
		float3											vRotate/*转动率*/;
		float											fRotationSpeedMask;
		float3											vUp;
		float3											vTarget/*目标点*/;
		float3 											vEye/*视点*/;
		float											fFov/*广角*/;
		int												nWidth;
		int												nHeight;
		float											fZNear;
		float											fZFar;
		float											fMinDistance/*视点最近距离目标点*/;
		float											fMaxDistance/*视点最远距离目标点*/;
		float											fMoveSpeed/*移动速度*/;
	};/*END_STRUCT*/

	struct	CameraDataLocal: public CameraData
	{
		inline	BOOL	IsEnable(CameraEnable n) const {return ((flags & n) == n);}
		inline	void	SetEnable(CameraEnable n, BOOL b)
		{
			if(b)
				flags	|=	n;
			else if(flags & n)
				flags	^=	(flags & n);
		}
		char											reserved[32];
		CameraDataLocal(){memset(this,0,sizeof(CameraDataLocal));}
	};

	class	CamLineIntersect
	{
	protected:
		BOOL						m_bDirection;
		float3						m_vDir,m_vFrom,m_vTo;
		//
		float						m_fradius;
		float						m_fdirMul;//|vDir*vDir|;
		float						m_distRate;//,u,v;//距离 目标点与射线源点的距离.
		//
		void						BuildParam();
	public:
		CamLineIntersect();
		//
		inline	float3&				getLineDir(){return m_vDir;}
		inline	float3&				getLineFrom(){return m_vFrom;}
		inline	float3&				getLineTo(){return m_vTo;}
		//
		inline	float				getLineRadius(){return m_fradius;}
		inline	float				getLineDirMultiply(){return m_fdirMul;}
		inline	float				GetDistance(){return m_distRate;}
		//
		inline	void				setLineRadius(float fRadius){m_fradius=fRadius;}
		inline	void				setLineDirMultiply(float fMUL){m_fdirMul=fMUL;}
		inline	void				SetDistance(float fDist){m_distRate=fDist;}
		//
		inline	BOOL				IsDirection(){return m_bDirection;}
		inline	void				SetDirection(BOOL bDirection){m_bDirection=bDirection;}
		BOOL						ExtendXYInz(float3& vOut,float z);//扩展 XY 以 z 
		CamLineIntersect& operator =(CamLineIntersect& i){m_fdirMul=i.m_fdirMul;m_vTo=i.m_vTo;m_distRate=i.m_distRate;m_vFrom=i.m_vFrom;m_vDir=i.m_vDir;m_fradius=i.m_fradius;m_bDirection=i.m_bDirection;return *this;}
		//
		float						GetPickDistance(){float r=sqrtf(m_vDir.x*m_vDir.x+m_vDir.y*m_vDir.y+m_vDir.z*m_vDir.z);return r*m_distRate;}
		void						SetDirectionLength(float distance);
		void						GetPickPosition(float3& vPickPosition);//得到点击位置
		void						Initialize(float3& vFrom,float3& pvTo,float fDirMultiply=1.0f,BOOL bDirection=false);
		BOOL						IntersectSphere(float3& mdPos,float fRadius);
		BOOL						IntersectSphereDirection(float3& mdPos,float fRadius);
		BOOL						ObjectInDirectionPick(float3& mdPos,float fRadius);
		BOOL						ObjectInLinePick(float3& mdPos,float fRadius);
		float						GetDistanceFromLineDirection(float3& mdPos);
		float						GetDistanceFromLine(float3& mdPos);
	};

	////////////////////////////////////////////////////////////////////////////
	//TODO: CamPerspectiveRT	说明:
	//class	:	CamPerspectiveRT
	//日期:2006-12-30 19:09:48
	////////////////////////////////////////////////////////////////////////////
	struct	CamPerspectiveRT
	{
		CamPerspectiveRT()	{	fovy=tanfovy=0.0f;}
		//
		BOOL					objInPerspective(const float3& p,float fObjRadius,float fMaxDistance,float* pResultDistance);
		//
		CamLineIntersect		cameraMiddle;
		float					fovy,tanfovy;
		//
	};

	////////////////////////////////////////////////////////////////////////////
	//TODO: CamOrthoRT	说明:
	//class	:	CamOrthoRT
	//日期:2006-12-30 19:09:43
	////////////////////////////////////////////////////////////////////////////
	struct	CamOrthoRT
	{
		CamOrthoRT()		{	fRadius=fDirMul=0.0f;}
		//
		BOOL					objInOrtho(const float3& p,float fObjRadius,float* pResultDistance);
		//
		CamLineIntersect		cameraMiddle;
		float					fRadius;//fRadius=1/ortho._11,和-1/ortho._22 的半径.
		float					fDirMul;//|vDir*vDir|
	};
	//


	class Camera;
	class CamFrustumPlane
	{
	protected:
		friend	class	Camera;
		float3			p[4];
		unsigned int	color;//
		//
		float3			n;
		float			d;
	public:
		//
		enum point_side
		{
			NO_SIDE,
			POSITIVE_SIDE,
			NEGATIVE_SIDE
		};
		//
		void		setup(float3& v1,float3& v2,float3& v3,float3& v4,unsigned int _color);
		//void		draw_plane();
		point_side	get_side(float3& p);
	};

	struct	CameraAni{
		char	type;
		enum{
			CAT_Move = 0,
			CAT_Rot,
			CAT_Dist,
			CAT_Dither,
		};
		float4	v;//最新设定
	};
	////////////////////////////////////////////////////////////////////////////
	//TODO: Camera	说明:
	//class	:	Camera
	//日期:2006-12-30 19:09:36
	////////////////////////////////////////////////////////////////////////////
	class	Camera
	{
	public:
		Camera(void);
		~Camera(void);
	public:
		//
		BOOL											Get2DFrom3D(Pixel& _out,const float3& vPos) const ;	//根据3D坐标转换成windows的xy坐标
		void											Get3DFrom2D(float3& _orig,float3& _dir, const Pixel& _in) const ;	//根据windows的xy坐标转换成3D射线和，原点
		//
		BOOL											CheckObjectVisible(const float3& vPosition,float fRadius,float fMaxDistance,float* pResultDistance);	//物体是否在镜头线内
		//
		void											SetOrthoProjection(int w,int h,float znear,float zfar);	//设置为2D视图
		void											SetPerspectiveProjection(float fov,int w,int h,float znear,float zfar);	//设置为3D视图
		//
		BOOL											IsPerspectiveProjection();	//是否是3D视图
		BOOL											IsOrthoProjection();			//是否是2D视图
		//
		void											SetRightHandArrow(BOOL b);	//是否是右手系
		//
		void											SetViewMatrix(const float3* pEye = 0,const float3* pTarget = 0,const float3* pUp = 0);	//设置观察矩阵
		void											SetViewMatrix(const matrix4x4& view);
		void											SetProjectionMatrix(const matrix4x4& proj);
		void											SetViewMatrixOnce(const float3& pEye,const float3& pTarget);
		//
		void											Set3DVRViewMatrixEye(BOOL bLeftEye);//设置每个眼睛的矩阵
		void											Set3DVRViewMatrix();
		float											m_fOffsetEye;//两眼间距，根据头大小视觉效果不一样
		int												m_nVRSmooth;
		VOID											Set3DVRViewMatrix(const matrix4x4& mat);//3D VR View matrix
		const matrix4x4*								Get3DVRViewMatrix(){return &m_matViewVR;}
		VOID											recalcRot();
		//
		const matrix4x4*								GetBoardWorld(int nBoard);	//返回MatrixBoard_Billbard，。。。
		void											SetRotationXYEnable(BOOL bx,BOOL by);	//设置左右和上下是否可以旋转
		//
		void											SetCameraRotate(const float3& vrot,BOOL bret = false);	//设置转动参数
		void											SetCameraRotateEnable(BOOL bx,BOOL by,BOOL bz);	//设置是否可以返回转动
		//
		void											SetUpper(const float3& vUp);
		void											SetTarget(const float3& vNewTargetPos);
		void											SetEye(const float3& eye);
		//
		inline	float									GetFov(){return this->m_cameraData.fFov;}
		inline	int										GetWidth(){return this->m_cameraData.nWidth;}
		inline	int										GetHeight(){return this->m_cameraData.nHeight;}
		inline	float									GetZNearest(){return this->m_cameraData.fZNear;}
		inline	float									GetZFar(){return this->m_cameraData.fZFar;}

		void											SetFov(float f);
		void											SetWidth(int n);
		void											SetHeight(int n);
		void											SetZNearest(float f);
		void											SetZFar(float f);
		const Rect&										GetArea(){return m_area;}
		VOID											SetArea(const Rect* rc);
		BOOL											m_bArea;
		Rect											m_area;
		//
		inline	const float3&							GetUpper(){return m_cameraData.vUp;}
		inline	const float3&							GetTarget(){return m_cameraData.vTarget;}
		inline	const float3&							GetEye(){return m_cameraData.vEye;}
		inline	const float3&							GetCameraRotate(){return m_cameraData.vRotate;}
		//
		void											SetDitherOffset(const float3& offset, float fMaxOffsetTime = 1.0f);	//是否进行抖动，fMaxOffsetTime表示抖动的时间
		//
		inline	void									SetMinWheelDistance(float f){m_cameraData.fMinDistance = f;}
		inline	float									GetMinWheelDistance(){return m_cameraData.fMinDistance;}
		inline	void									SetMaxWheelDistance(float f){m_cameraData.fMaxDistance = f;}
		inline	float									GetMaxWheelDistance(){return m_cameraData.fMaxDistance;}
		float											GetDistance();
		void											SetDistance(float f);
		//
		void											SetMoveSpeed(float f){this->m_cameraData.fMoveSpeed = f;}
		float											GetMoveSpeed(){return m_cameraData.fMoveSpeed;}
		//
		BOOL											IsPointVisible(const float3& p);
		BOOL											IsSphereVisible(const float3& p,float radius);
		BOOL											IsAABoxVisible(const AABox& vbox);
		BOOL											IsAABoxVisible(const PBoxVertices& vbox, PlaneVisible* _out = 0);
		BOOL											IsPlaneVisible(const float3& p0, const float3& p1, const float3& p2, const float3& p3, BOOL bCCW = true);
		//
		BOOL											IsAABoxVisibleLRNF(const PBoxVertices& vbox);//左右远近
		BOOL											IsAABoxVisibleTB(const PBoxVertices& vbox);//上下（因为在前面的box太小的时候总是无法延伸到最后,在渲染得时候被调用
		//
		void											SetRotationSpeed(BOOL bEnable, float fMask = 1.0f);
		//
		BOOL											GetCameraConfig(CameraDataLocal& _out);
		BOOL											SetCameraConfig( const CameraDataLocal& _in);
		//
		void											SetMaxLodDistance(float fMaxDistance){m_fMaxLodDistance = fMaxDistance;}
		float											GetMaxLodDistance(){return m_fMaxLodDistance;}
		BOOL											IsFPSMode();
		VOID											SetFPSMode(BOOL b);
		//
		void											SetClipCamera(const float3& vMin, const float3& vMax);
		void											GetClipCamera(float3& vMin, float3& vMax);
		IntersectData*									BeginIntersect(int x = -1, int y = -1);
		IntersectData*									GetIntersectPtr(){return &m_intersect;}
		inline	matrix4x4&								GetViewMatrix(){return m_matView;}
		inline	matrix4x4&								GetProjMatrix(){return m_matProjection;}
		inline	matrix4x4&								GetViewProjMatrix(){return m_viewProjection;}
		inline	matrix4x4&								GetScreenViewProjMat(){return m_screenViewProjMat;}
		//
		void											SetVRFront(const float3& dir);
		char											m_bVRFrontSet;
		float3											m_vrFrontSet;
		float3											m_vrRotator;//VR 转动率返回
		matrix4x4										m_vrFrontMat;
		char											m_bVRFirst;
		float											m_fVRFirst;
		float											m_fRotationVRX;
		VOID											ResetAll();
		//
		BOOL											m_bEnabled;
		unsigned int									m_cameraViewIdx;
		void											onChangeView();
		VOID											GetCameraOffset(float3* xdirRet, float3* ydirRet, BOOL bFixedZAxis = false);//返回移动方向,xdir,ydir,zdir
		VOID											Offset(const float3& off);//位移镜头
		void											SetRotation(const float2& vrot);
		//
		void											processAcc(float x,float y,float z,long timestamp);
		void											processGyro(float x,float y,float z,long timestamp);
		void											processEye();
		OrientationEKF									m_ekf;
	public:
		void											setup(matrix4x4& view,float fov,float fZNear,float fZFar,int wndWidth,int wndHeight);
		void											setupFrustum();
		virtual	VOID									OnInputEvent(Phantom::InputEventID evt, int x, int y, unsigned int touchIndex);
		virtual	void									OnFrameMove(float fElapsedTime);	//每次更新
		BOOL											MoveCamera2D(float offsetX, float offsetY);
		BOOL											RotationCamera2D(float offsetX, float offsetY);
		CameraDataLocal&										GetCameraData(){return m_cameraData;}
		//
		VOID											Play();
		VOID											Stop();
		VOID											OnPlayNext();
		CDynamicArray<CameraAni>						m_nexts;
		Name											m_funcPlayOver;
		int												m_funcParam;
		char											m_rayCamera;
		float											m_fOffsetSpeed,m_fOffsetDown;
		char											m_bOffsetEnable;
		//
		float3											m_lastMovePos;
		double											m_fLastMoveTime;//最后一次按下的时间
		char											m_bMoveMode;
		float3											m_fMoveFirst;
		VOID											ClearMoveSpeed();
		float3											m_lastMoveOffset[MAX_MOVE_SPEED];
		double											m_lastMoveOffsetTime[MAX_MOVE_SPEED];
		char											m_lastMoveOffsetCnt;
		VOID											AddMoveSpeed();//移动距离和时间
		VOID											CheckMoveMode();//移动距离和时间
		VOID											OnMoveMode();//移动计算
	protected:
		BOOL											m_bPlay;
		CameraAni										m_currPlay;
		char											m_currPlayX,m_currPlayY;
	protected:
		//
		enum{
			pad_lt = 0,
			pad_lb = 1,
			pad_rt = 2,
			pad_rb = 3,
			//
		};
		matrix4x4										m_matBoards[BOARD_COUNT];
		float3											m_near[4];
		float3											m_far[4];
		CamFrustumPlane									m_plane[6];
	protected:
		void											getLineIn2DPixel();	//构建鼠标点击参数
	protected:
		void											getLineIn2DPixel_P(CamLineIntersect& vParam,int sx,int sy);
		float											GetViewMoveDistance(float fMul = 1.0f, float fStep = 20.0f);
		void											ViewMoveDistance(float fDistanceAdd, BOOL bMoveTarget);
		void											returnSaveDistance(float fMul = 1.05f);
		void											rebuildViewRot();
		void											returnSaveRot();
	public:///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		float3											GetFpsTarget();
		//
		CamPerspectiveRT								m_vPerspective;
		CamOrthoRT										m_vOrtho;
		CameraDataLocal										m_cameraData;
		//
		matrix4x4										m_matViewVR;
		//
		float											m_fMaxLodDistance;
		float											m_fAspect;
		float											m_fFrustumMul;
		matrix4x4										m_matProjection;
		matrix4x4										m_matView;
		matrix4x4										m_viewProjection;
		matrix4x4										m_screenViewProjMat;
		matrix4x4										m_matViewInverse;
		float3											m_vViewScale;
		IntersectData									m_intersect;
		float3											m_ditherOffset;
		float											m_fMaxDitherOffset;
		float3											m_clipMin, m_clipMax;
		BOOL											m_bRefreshViewRot;
		//
		float3											m_vLookatDest;
		BOOL											m_bReturnRotate;
		//
		Pixel											m_lastPosition[MAX_TOUCH_COUNT];
        Pixel											m_lastDownPosition[MAX_TOUCH_COUNT];
		BOOL											m_bLButtonDown[MAX_TOUCH_COUNT];
		BOOL											m_bRButtonDown;
		BOOL											m_bMButtonDown;
		//
		float											m_fSaveDistance;
		float											m_fSavedMulDistance;
	};
	class PixelLinear
	{
	public:
		PixelLinear(void);
		~PixelLinear(void);
	public:
		//
		const Pixel&				GetLinePoint(unsigned int index){return m_posBuffer[index];}
		unsigned int				GetNumPoint(){return m_numPick;}
		//
		unsigned int				Pick(float gridW, float gridH, float x, float y, float xto, float yto);
	protected:
		inline	BOOL				Push(int x,int y){if(m_numPick>=2056)return false;m_posBuffer[m_numPick].x = x;m_posBuffer[m_numPick].y = y;m_numPick++;return (m_numPick<2056);}
	protected:
		//
		Pixel						m_posBuffer[2056];
		unsigned int				m_numPick;
	};
};

#endif
