//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UNIT3DELEMENT_H__
#define __PHANTOM_UNIT3DELEMENT_H__
#include "PhantomMesh.h"
#include "PhantomFloatAnimation.h"
#include "PhantomMusic2D.h"

#define	MAX_EMISSION_BIND_POINTS		8
#define	MAX_EMISSION_PARTICLEs			500
#define	TRACK_SECOND_LENGTH				20.0f
//#define	MAX_ELEMENT_MESH				5
//#define	MAX_UNIT3D_CHILD_MESH			5	//最多10个子模型
#define	MAX_FLOAT_PARAMETER				2
#define	MAX_PARTICLE_SHOW_TYPE		32

namespace Phantom{

	enum ElementType{
		ElementType_Mesh	=	0,
		ElementType_Emission	=	1,
		ElementType_Sound		=	2,
	};

	/*ENUM*/enum EmissionType/*喷射类型*/
	{
		EmissionType_Matrix/*喷射类型_使用矩阵控制*/	=	0,		//跟随世界矩阵进行变换
		EmissionType_Self/*喷射类型_发射后脱离控制*/	=	1,		//始终计算位置（每次更新都重新计算）
		//	EmissionType_BindPoint/*喷射类型_绑定点之间形成一条线*/	=	2,		//始终计算位置（每次更新都重新计算）
	};

	enum MeshKeyIndex{
		MeshKeyIndex_ColorR	=	0,
		MeshKeyIndex_ColorG,
		MeshKeyIndex_ColorB,
		MeshKeyIndex_Alpha,
		MeshKeyIndex_ScaleFull,
		MeshKeyIndex_PositionX,
		MeshKeyIndex_PositionY,
		MeshKeyIndex_PositionZ,
		//转动一组
		MeshKeyIndex_RotationOffset,
		MeshKeyIndex_RotationX,
		MeshKeyIndex_RotationZ,
		MeshKeyIndex_ScaleX,
		MeshKeyIndex_ScaleY,
		MeshKeyIndex_ScaleZ,
		//往下加
		MeshKeyIndex_Count,
	};
	enum EmissionKeyIndex{
	};

	class CSceneShadowData: public Base
	{
	public:
		ImplementComValue();
	public:
		CDynamicArray<PosColorUV>				vertices;
		CDynamicArray<Triangle>					indices;
		int										x,y;
		int										vertexW, vertexH;
		//
		BOOL									bPushed;
		Texture*							texturePtr;
		CSceneShadowData(){x = y = 0; vertexW = vertexH = 0; bPushed = false; texturePtr = 0;}
		virtual ~CSceneShadowData(){vertices.clear(); indices.clear();}
	};
	class Unit3DActionData;
	class CElementMeshControl;
	class CElementEmission;
	class CElementMusic3D;
	class CElementMeshControl;
	class CActionElement
	{
	public:
		CActionElement(Unit3DActionData* effectPtr);
		virtual ~CActionElement();
	public:
		//
		virtual	CElementMeshControl*		GetElementMeshPtr(){return 0;}
		virtual	CElementEmission*			GetElementEmissionPtr(){return 0;}
		virtual	CElementMusic3D*			GetElementMusicPtr(){return 0;}
		//
		virtual	VOID						SetBeginTrackIndex(INT nFrame);	//设置开始显示的关键帧
		virtual	VOID						SetEndTrackIndex(INT nFrame);	//设置结束显示的关键帧
		virtual	INT							GetBeginTrackIndex();			//取得开始显示的关键帧
		virtual	INT							GetEndTrackIndex();				//取得结束显示的关键帧
		//
		int									GetID(){return m_id;}
		virtual	VOID						SetFileName(const char* szFile){}
		virtual	const char*					GetFileName(){return "";}
		//
		virtual	const char*					GetName();	//设置名称
		virtual	VOID						SetName(const char* szName);
		//
		virtual	ElementType					GetElementType() = 0;
		//
		virtual	INT							GetKeyCount(){return 0;}
		virtual FloatAnimation*				GetKeyPtr(int index){return 0;}
		virtual const char*					GetKeyName(int index){return "";}
		virtual	BOOL						IsEnabled(int index){return false;}
		virtual	float						GetKeyValue(int index, float fTime){return 0;}
		//
		virtual	VOID						SetVisible(BOOL b){m_bVisible = b;}
		virtual	BOOL						IsVisible(){return m_bVisible;}
		//
		virtual	VOID						CloneFrom(CActionElement* eff);
		//ImplementComValue();
		virtual	VOID						SetBindPoint(int equipID){m_bindPoint = equipID;}	//设置帮定点
		virtual	int							GetBindPoint(){return m_bindPoint;}
		//
		virtual	VOID						Load(CFileHelperR& r);
		virtual	VOID						Save(CFileHelperW& w);
		virtual	VOID						SetFrameCount(INT nFrameCount);
		virtual	int							GetFrameCount(){return m_frameCount;}
		virtual	VOID						InitKey(FloatAnimation& fani, BOOL bEnable, BOOL bVisible, float fMin, float fMax, float fDefault, BOOL bIsColor = false);
		virtual	VOID						SetTemplateVisible(BOOL b){m_bTemplateVisible = b; m_nChangeV++;}	//是否显示在模板
		virtual	BOOL						GetTemplateVisible(){return m_bTemplateVisible;}		//设置为模板动作
	public:
		//
		short								m_nChangeV;
		char								m_bTemplateVisible;
		int									m_id;
		NameT<32>							m_name;
		//FileName							m_fileName;
		short								m_trackBeginIndex, m_trackEndIndex;
		short								m_frameCount;
		char								m_bVisible;
		short								m_bindPoint;
		//
		Unit3DActionData*					m_actionPtr;
	};

	class CElementMeshControl: public CActionElement
	{
	public:
		CElementMeshControl(Unit3DActionData* effectPtr);
		~CElementMeshControl();
		virtual	VOID						Load(CFileHelperR& r);
		virtual	VOID						Save(CFileHelperW& w);
		virtual	ElementType					GetElementType(){return ElementType_Mesh;}
	public:
		CElementMeshControl*				GetElementMeshPtr(){return this;}
		int									GetActionID();
		VOID								SetActionID(int nID);
		int									GetKeyBeginIndex();
		int									GetKeyEndIndex();
		VOID								SetKeyBeginIndex(int n);
		VOID								SetKeyEndIndex(int n);
		//动作播放速度
		void								SetActionLoop(int loopCount);	//设置是否循环播放
		int									GetActionLoop();			//是否循环播放
		//
		VOID								SetActionSpeed(float fSpeed);
		float								GetActionSpeed();
		//
		VOID								CloneFrom(CActionElement* eff);
		//
		virtual	INT							GetKeyCount(){return MeshKeyIndex_Count;}
		virtual FloatAnimation*				GetKeyPtr(int index){if(index<0||index>=MeshKeyIndex_Count)return 0; return &m_keys[index];}
		virtual const char*					GetKeyName(int index);//返回中文
		virtual	BOOL						IsEnabled(int index);
		virtual	float						GetKeyValue(int index, float fTime);
	public:
		FloatAnimation						m_keys[MeshKeyIndex_Count];
		int									m_meshid;//模型数据id，如果是0则表示控制主模型
		CDynamicArrayS<short,0>				m_submeshs;//控制的所有子模型数量,只有在控制所有模型的时候动作才能被设置有效
		int									m_nActionID;
		short								m_nKeyBegin, m_nKeyEnd;
		short								m_nLoopCount;
		float								m_fActionSpeed;
	};


	class CElementMusic3D: public CActionElement
	{
	public:
		CElementMusic3D(Unit3DActionData* effectPtr);
		~CElementMusic3D();
		CElementMusic3D*					GetElementMusicPtr(){return this;}
		virtual	VOID						Load(CFileHelperR& r);
		virtual	VOID						Save(CFileHelperW& w);
		virtual	ElementType					GetElementType(){return ElementType_Sound;}
		//
		VOID								CloneFrom(CActionElement* eff);
	public:
		void								SetIncidence(float fDistance);//设置影响范围
		float								GetIncidence();	//返回影响范围
		float								GetWeight();			//设置声音权重
		void								SetWeight(float f);	//设置声音权重
		float								GetPlaySpeed();
		VOID								SetPlaySpeed(float f);
		void								SetSoundLoop(BOOL loopCount){m_bLoop = loopCount;}	//设置是否循环播放
		BOOL								GetSoundLoop(){return m_bLoop;}			//是否循环播放
	public:
		FileName							m_fileName;
		float								m_fIncidence;
		float								m_fWeight;
		float								m_fPlaySpeed;
		char								m_bLoop;
	};
	
	class CElementEmission: public CActionElement
	{
	public:
		CElementEmission(Unit3DActionData* effectPtr);
		~CElementEmission(void);
		CElementEmission*					GetElementEmissionPtr(){return this;}
		//
		char								GetRenderType(){return m_nRenderType;}
		VOID								SetRenderType(ParticleRenderType st){m_nRenderType = st;}
		VOID								SetFileName(const char* szFile);
		//VOID								SetMaxParticle(INT nCount){m_data.nCount = getmin(nCount, MAX_EMISSION_PARTICLEs);}	//一直可见的数量
		//INT									GetMaxParticle(){return m_nMaxParticle;}
		//VOID								SetCreationTime(float fTime){m_fCreationTime = getmax(fTime, 0.0f);}
		//float								GetCreationTime(){return m_fCreationTime;}
		//
		BOOL								IsControlCamera(){return m_bIsControlCamera;}			//是否控制摄像头晃动
		VOID								SetControlCamera(BOOL b){m_bIsControlCamera = b;}
		//
		char								GetTextureOP(){return m_nTextureOP;}
		VOID								SetTextureOP(Phantom::TextureBlend nOP){m_nTextureOP = nOP;}			//设置贴图运算方法
		//
		int									GetSrcBlend(){return m_data.srcBlend;}
		VOID								SetSrcBlend(Phantom::BlendMode b){m_data.srcBlend = b;}
		int									GetDestBlend(){return m_data.destBlend;}
		VOID								SetDestBlend(Phantom::BlendMode b){m_data.destBlend = b;}
		//
		VOID								SetLookAtNext(BOOL b){m_bLookAtNext = b;}	//模型方向指向下一个点
		BOOL								GetLookAtNext(){return m_bLookAtNext;}
		VOID								SetEmissionType(EmissionType nType){this->m_nEmissionType = nType;}	//设置控制类型
		char								GetEmissionType(){return this->m_nEmissionType;}
		//
		BOOL								IsUseSystemWind(){return m_bUseSystemWind;}	//是否使用系统风向
		VOID								SetUseSystemWind(BOOL b){m_bUseSystemWind = b;}	//是否使用系统风向
		//
		//INT									GetBindCount();	//骨骼绑定数量
		int									GetBindPoint(){return m_nBindPoint;}	//返回绑定ID
		VOID								SetBindPoint(int nBindPoint = -1){m_nBindPoint = nBindPoint;}
		//
		VOID								SetPartIndex(INT index){this->m_nEmissionPartIndex = index;}
		INT									GetPartIndex(){return this->m_nEmissionPartIndex;}
		//
		//FloatAnimation*						GetParticleKeyPtr(ParticleKey k);
		//int									GetParticleFrameCount();
		//VOID								SetParticleFrameCount(int n);
		//VOID								InitParticleKey(int index, const char* szName, BOOL bEnable, BOOL bVisible, float fMin, float fMax, float fDefault);
		////
	public:
		VOID								CloneFrom(CActionElement* eff);
		virtual	VOID						Load(CFileHelperR& r);
		virtual	VOID						Save(CFileHelperW& w);
		virtual	ElementType					GetElementType(){return ElementType_Emission;}
		//
	public:
		VOID								DestroyAll();
	public:
		//发射控制,控制整个粒子数量的发射参数,比如总共可以发射1000个粒子，那么循环到最后是第1000个粒子的发射参数
		ParticleEmitterData					m_data;
		FileName							m_fileName;
		char								m_bLookAtNext;	//正视下一个方向点,模型渲染方式下有效
		char								m_bUseSystemWind;	//系统风向对粒子有效
		char								m_bIsControlCamera;
		////
		char								m_nEmissionType;
		char								m_nRenderType;//
		char								m_nTextureOP;
		int									m_nBindPoint;
		AutoPtr<Texture>					m_texturePtr;
		INT									m_nEmissionPartIndex;			//是否在模型面上随机产生
	};

#define	SHADER_PARAM_COUNT				3
	struct	EmissionFrameMove
	{
		color4								color;
		float3								offset;
		float3								scale;
		char								bOffset[3];
		char								bColor[4];
		float								offsetSelf;
		float								rotationX, rotationZ;
		int									actionID;
		float								actionKeyIndex;
		float								shaderParam;//[SHADER_PARAM_COUNT];
		EmissionFrameMove()
		{
			bColor[0]=bColor[1]=bColor[2]=bColor[3] = false;
			offsetSelf	=	0;
			actionID = -1;
			actionKeyIndex = 0;
			color.setRGBA(0,0,0,1);
			offset.setxyz(0,0,0);
			scale.setxyz(1,1,1);
			rotationX	=	rotationZ	=	0;
			bOffset[0]	=	bOffset[1]	=	bOffset[2]	=	false;
			//for(int i=0;i<SHADER_PARAM_COUNT;i++)
			shaderParam	=	0;
		}
	};

	class	Unit3DAction;
	class	CElementRuntime
	{
	public:
		CElementRuntime(CActionElement* e, Unit3DAction* a);
		virtual	~CElementRuntime();
	public://
		virtual	VOID						SetVisible(BOOL b){m_bVisible = b;}
		BOOL								IsVisible(){return m_bVisible;}
		INT									GetID(){return m_elementPtr->GetID();}
		const char*							GetName(){return m_elementPtr->m_name;}
		//
		virtual	INT							GetShowMeshCount(){return 0;}
		virtual	INT							GetShowMeshIndex(INT nIndex){return -1;}
		virtual	VOID						SetShowMeshIndex(INT nIndex, INT nPartIndex){}
		virtual	Mesh*						GetMeshPtr(){return 0;}	//模型指针
	public:
		//重新计算阶段
		virtual	BOOL						BeforeFrameMove(EmissionFrameMove& fm, Mesh* basePtr, float fTrackIndex);
		//frameMove阶段，只有m_bRender有效时才会自动被调用
		virtual	VOID						FrameMove(Mesh* basePtr, const matrix4x4& actionMat);
		virtual	VOID						OnRender();
		//virtual	VOID						PushToRenderEmission(){}
		virtual	VOID						BeginAction();
		virtual	VOID						StopAction();
		//
		virtual	VOID						OnStartElement(){}
		virtual	VOID						OnStopElement(){}
		//
		inline	BOOL						IsEnabled(int k){return m_elementPtr->IsEnabled(k);}
		inline	float						GetValue(int k){return m_elementPtr->GetKeyValue(k, m_fFrameIndex);}
		inline	float						GetValueFrame(int k, float fFrameIndex){return m_elementPtr->GetKeyValue(k, fFrameIndex);}
		//
		virtual	BOOL						GetKeyMatrix(matrix4x4& ret, const matrix4x4* parent, float fFrameIndex, float3* scaleRet = 0);
		//
		VOID								CheckChange();
	protected:
		virtual	VOID						OnFrameMove(const matrix4x4& worldMat){}
		virtual	BOOL						IsRequestMatrix(){return true;}	//不需要构造矩阵数据
	public:
		Unit3DAction*						m_actionPtr;
		CActionElement*						m_elementPtr;
		char								m_bVisible;
		char								m_bRender;
		//matrix4x4							m_worldMat;
		float								m_fFrameIndex;
		float								m_fLerp;
		short								m_nChangeV;
		char								m_bTemplateVisible;
		char								m_bPlaying;
	};

	class	CElementMeshRuntime: public CElementRuntime
	{
	public:
		CElementMeshRuntime(CElementMeshControl* act, Unit3DAction* a);
		~CElementMeshRuntime();
		virtual	BOOL						BeforeFrameMove(EmissionFrameMove& fm, Mesh* basePtr, float fTrackIndex);
		CElementMeshControl*				GetActionPtr(){return static_cast<CElementMeshControl*>(m_elementPtr);}
		virtual	BOOL						GetKeyMatrix(matrix4x4& ret, const matrix4x4* parent, float fFrameIndex, float3* scaleRet = 0);
	public:
		virtual	BOOL						IsRequestMatrix(){return false;}	//不需要构造矩阵数据
		PtrID								m_actionID;
	};

	class	CElementMusic3DRuntime: public CElementRuntime
	{
	public:
		CElementMusic3DRuntime(CElementMusic3D* m, Unit3DAction* a);
		~CElementMusic3DRuntime();
		CElementMusic3D*					GetMusicPtr(){return static_cast<CElementMusic3D*>(m_elementPtr);}
		virtual	VOID						OnFrameMove(const matrix4x4& worldMat);
		virtual	BOOL						BeforeFrameMove(EmissionFrameMove& fm, Mesh* basePtr, float fTrackIndex);
		virtual	VOID						OnStartElement();
		virtual	VOID						OnStopElement();
	public:
		//Music2D						m_musicPtr;
        int                             m_fileIndex;
	};

	class CElementEmissionRuntime: public CElementRuntime
	{
	public:
		CElementEmissionRuntime(CElementEmission* e, Unit3DAction* a);
		virtual	~CElementEmissionRuntime();
		CElementEmission*			GetEmissionPtr();
		//
		//
		//inline	BOOL				IsEnableEmit(CElementEmission* emit, int k){return emit->m_particleKeys[k].IsEnabled();}
		//inline	float				GetValue(CElementEmissionOld* emit, int k, float fFrameIndex){return emit->m_particleKeys[k].GetFrameValue(fFrameIndex);}
	public://virtual
		virtual	VOID				StopAction(){}//m_trackLight.Reset();}
		virtual	VOID				BeginAction();
		VOID						GetKeyMatrixEmission(matrix4x4& ret, const matrix4x4* parent, float fFrameIndex, float3* scaleRet = 0);
		VOID						FrameMove(Mesh* basePtr, const matrix4x4& actionMat);
		virtual	VOID				OnStartElement();
		virtual	VOID				OnStopElement();
	public:
		VOID						RenderShadow(CSceneShadowData* shadowPtr, float fHeight/*向上稍微抬起*/, EmissionParticle* pLocal, float uBegin, float vBegin, float uEnd, float vEnd);	//渲染贴地阴影
		//
		BOOL						AddParticleOne(matrix4x4& world, Mesh* basePtr, float3& scaleBase, float fSingleFrameIndex);
		VOID						FreeParticle(int nIndex);
	public://particle
		//
		BOOL						StartEmit();
		VOID						StopEmit();
	protected://IRenderBase
		void						drawObject();
		//const matrix4x4&			GetWorldMatrix(){return m_worldMat;}
	};


};

#endif
