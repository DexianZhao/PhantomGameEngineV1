//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   Scene3DUnits.h
软件名称   :   幻影游戏引擎

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___PHANTOMSCENE3DUNITS__H___
#define ___PHANTOMSCENE3DUNITS__H___

#include "PhantomBase.h"
#include "PhantomUnit3D.h"
#include "PhantomCamera.h"
#include "PhantomUIControl.h"
#include "PhantomSceneDefine3D.h"
#include "PhantomAStarPath.h"
#include "PhantomGame.h"
#include "PhantomUIManager.h"

namespace Phantom{
	class	QuadBox;
	class	Scene3D;
	#define	MAX_CLASS_IDs			100

#pragma pack(push,1)
	struct	SavePosRot{
		unsigned int			id;//对应的场景对象的ID
		float					rot;
		matrix4x4				mat;
		char					visible;
	};
#pragma pack(pop)

	struct BindEvent
	{
		int						id;
		NameT<32>				func;
	};

	class	SceneObject;
	class SceneRenderBlood: public UISortBase
	{
	public:
		virtual	VOID			Render3D();
		SceneObject*	m_obj;
	};

	class SceneObject: public MeshCallback
	{
	public:
		ImplementComValue();
		virtual	const AABox*		GetBox();
		virtual	VOID				Render();
		virtual	VOID				FrameMove();
		virtual	VOID				RenderToShadowMap();
		virtual	const matrix4x4*	GetWorldPtr();
		virtual	VOID				SetWorld(const matrix4x4* mat);
		virtual	int					GetType();
		//
		virtual	VOID				SetPosition(const float3& v);	//设置物体位置
		virtual	const float3&		GetPosition(){return GetMeshPtr()->GetPosition();}//{return m_worldMat.GetPosition();}					//取得物体位置
		virtual	VOID				EnableSelect(bool bEnable, unsigned int data = 0);			//设置为选择项，并同时设置选择发光颜色
		virtual	BOOL				Intersect(IntersectData* intersect, BOOL bCollisionOnly);
		virtual	BOOL				GetHeight(IntersectData* intersect);
		VOID						DetachManager();
		VOID						RenderBlood();
		//
		virtual	BOOL				Save(CSafeFileHelperW& w);
		virtual	BOOL				Load(CSafeFileHelperR& r, BOOL bLoadMesh = true);
		//
		BOOL						IsAllowSelect(INT nSelectType);	//是否可以选择此项
		BOOL						GetVisible(){return m_bVisible;}
		VOID						SetVisible(BOOL b);
		BOOL						GetEnabled(){return m_bEnabled;}
		VOID						SetEnabled(BOOL b){m_bEnabled = b;}	//设置显示与否
		const char*					GetName();
		VOID						SetName(const char* szName, BOOL bCustom = true){m_name = szName;this->m_bCustomName=bCustom;}
		inline	BOOL				IsCustomName(){return m_bCustomName;}
		virtual	VOID				SetScenePtr(Scene3D* s){m_scenePtr = s;}
		//
		VOID						bind_event(int eventID, const char* szLuaFunc);
		virtual	VOID				ActionPlayOver(Mesh* m, int actiontype, const char* actionname);
		virtual	VOID				ActionPlayOver(Unit3D* m, int actiontype, const char* actionname);
		virtual	BOOL				ActionMoveOver(Mesh* m);
		virtual	VOID				ActionAttackBy(Mesh* m, int actionID, int playtype);
		virtual	VOID				ActionEventRaise(Mesh* m, int actiontype);
		VOID						CheckNextAction();
		VOID						ResetAll(const float3& pos, float rot);
		//
		Mesh*						GetMeshPtr();
		Scene3D*					GetScenePtr(){return m_scenePtr;}
		SceneObject*				Clone();
		VOID						SetClassID(int cid);
		inline	INT					GetID(){return m_gameID.sceneid.id;}
		VOID						SetPause(BOOL b);
		VOID						UpdateMoveSpeed();
		VOID						SetSpeed(float f);
		VOID						SetScale(float f);
		VOID						SetRotation(float fZ);
		//VOID						SetColor(const color4& c);
		VOID						SetPosition(const Pixel& s);
		BOOL						MoveTo(const Pixel& s, const char* szLuaFunc);
		VOID						LookatTo(const Pixel* s);
		VOID						MoveDir(const Pixel& dir);
		VOID						SetBlood(float f);
		VOID						StopMove();
		VOID						SetAction(const char* actionName);
		VOID						SetAction(ActionType type, int index = 0, int nLoop=-1);
		VOID						SetAttack(ActionType type, int index = 0, float fAtkTime = 3.0f);
		VOID						SetDie(ActionType type, int index = 0);
		VOID						SetActionBeginFunc(LPCSTR szLuaBegin);
		VOID						Change(const float3& pos, float fRot);
		VOID						SetActionEndFunc(LPCSTR szLuaEnd);
		const char*					GetActionName();
		ActionType					GetActionType();
		VOID						SetNextAction(const char* actionName);
		VOID						SetNextAction(ActionType type);
		VOID						SetTargetPos(const float3& pos);
		VOID						OnSetAction(const char* actionName);
		VOID						OnSetAction(ActionType type, int index = 0, int nLoop=-1);
		VOID						OnBeginAction(int actiontype, const char* actionname);
		enum{
			State_Standard = 0,
			State_MoveTo,
			State_MoveDir,
			State_Attack,
			State_ByAttack,
			State_Die,
			State_Hit,
			State_Fly,
			State_Jump,
			State_Count,
		};
		VOID						SetFly(float f);
		//
		inline	int					GetDiffuseIndex(){return m_diffuseIndex;}
		VOID						ChangeDiffuse(int index);
		//
		UIControl*					AddHit(int val, float fWaitTime, BOOL bText/*是否头顶上显示文字*/);
		VOID						Reborn(const float3& pos);
		VOID						ResetEnv(SavePosRot* spr=0);//如果spr==0则设置成游戏模式
		VOID						ResetSaveEnv();//如果spr==0则设置成游戏模式
	protected:
		void						OnChangeWorld();
		void						ChangeMeshTransform();
	protected:
		SceneObject(Scene3D* s);
		virtual	~SceneObject(void);
		BOOL						LoadObject(int fileNameID);
		BOOL						LoadObject(int fileNameID, Mesh* mesh);
	public://动态
		//父对象
		VOID						SetParent(SceneObject* obj);
		SceneObject*				GetParent();
		VOID						AddChild(SceneObject* obj);
		VOID						RemoveChild(SceneObject* obj);
		SceneID						m_parentID;//父对象ID
		CDynamicArrayS<SceneID,0>	m_childs;//子场景数据
		//
		LightAffectData			m_light;
		//
		SceneRenderBlood			m_srb;
		CDynamicArrayS<BindEvent,0>		m_binds;
		CDynamicArrayS<WayPoint2D>	m_ways;
		char						m_bPause;
		NameT<32>					m_moveEndFunc;
		NameT<32>					m_actionBeginFunc;
		NameT<32>					m_actionEndFunc;
		CDynamicArrayS<NextAction,0>	m_nextActions;
		float						m_fBloodProgress;//血量
		float						m_fRange;//范围
		float						m_fAtkRange;//攻击范围,红色
		unsigned int				m_bloodColor;
		short						m_nBipBoneIndex;
		char						m_nState;
		float						m_fFly;//从模型固定值读取
		short						m_wayPointIndex;
		float						m_wayPointLength;
		float2						m_movedir;//当前移动方向
		PtrID						m_configID;//配置文档信息
		char						m_nActionIndex;//动作索引
		char						m_nActionType;//动作索引
		char						m_bAttackReback;//是否反馈了攻击动作了
		char						m_nSaveActionIndex;//动作索引保存
		char						m_nSaveActionType;//动作索引保存
		char						m_nSaveState;
		float						m_fHitWaitTime;
		char						m_bMeshVisible;
		char						m_bAutoHeight;
		unsigned int				m_lightChanged;
		float						GetObjectHeight();
	protected:
		AutoPtr<Mesh>				m_meshPtr;
		QuadBox*					m_pCurrentRegion;
		SceneObject*				m_pNext;
		Scene3D*					m_scenePtr;
		friend class				QuadBox;
		friend class				SceneEnv;
		friend class				Scene3D;
	public://需要保存
		GameSceneID					m_gameID;
		//GameID						m_sceneID;
		Pixel						m_bloodOffset;
		char						m_bShowBlood,m_bShowName;
		float						m_fSpeed;
		float						m_fScale;//场景角色的对象位置，缩放和转动都设置这两个值
		float						m_fRotationZ;//场景角色的对象位置，缩放和转动都设置这两个值
		//color4						m_color;//以后将作为所属方颜色区别
		char						m_powerSide;//所属方ID
		short						m_nClass;//类型ID
		short						m_fileNameID;//文件索引ID
		unsigned int				m_nContentID;
		//
		matrix4x4					m_saveMat;
		float						m_saveRot;
		char						m_saveVisible;
	protected:
		NameT<32>					m_name;
		char						m_bCustomName;//是否独立取名字了
		//int							m_id;//唯一标识符
		char						m_bVisible;
		char						m_bEnabled;
#ifdef EDITOR_OS
		matrix4x4					m_worldMat;//场景对象矩阵
#endif
		//
		char						m_diffuseIndex;//材质方案
		short						m_columnCount;
		SceneObjectDB*				m_lineDB;//自定义数据
	};
	
	class SceneGameSkill: public MeshCallback
	{
	public:
		SceneGameSkill(Scene3D* s);
		virtual ~SceneGameSkill();
		enum SGS{
			SGS_None		=	0,
			SGS_InSource	=	1,//在我的身上播放，播放完毕之后不需要反馈消息,包括被击消息和持续技能
			SGS_ToTarget	=	2,//飞行到目标身上，到达目标之后反馈到达消息,如果途中目标死亡则直接反馈消息
			SGS_InPoint		=	3,//施放到某个点
		};
		BOOL						Setup(unsigned short fileid);//初始化技能类
		VOID						StartSkill(SceneObject* src, SceneObject* target, float fContinueTime);//SGS_ToTarget
		VOID						StartSkill(SceneObject* src, float fContinueTime);//SGS_InSource
		VOID						StartSkill(const float3& pos, float fContinueTime);//SGS_InPoint
	protected:
		VOID						OnFrameMove();
		VOID						OnRender();
		VOID						OnSkillStart(int type);
		VOID						Reset();
		//
		VOID						GetPos(matrix4x4& ret, SceneObject* m, int boneIndex, float fHeightScale);
		//
		VOID						OnFinish();
		BOOL						IsFree(){return (m_bFree&&m_fWaitFreeTime<=0);}
		virtual	VOID				ActionPlayOver(Mesh* m, int actiontype, const char* actionname);
		virtual	VOID				ActionAttackBy(Mesh* m, int actionID, int playtype);
		VOID						SetTarget(SceneObject* target, const char* szBoneLike);//目标对象,默认绑定pelvis
		VOID						SetSource(SceneObject* unit, const char* szBoneLike);//绑定发射对象
	protected:
		friend class				Scene3D;
		int							m_id;//最新使用的ID
		unsigned short				m_fileid;//技能表中的行ID
		GameSceneID					m_srcID;//
		GameSceneID					m_targetID;//第一个目标对象
		float						m_fSpeed;
		char						m_type;//技能类型
		short						m_sourceBoneID;
		short						m_targetBoneID;
		float						m_fHeightScale;
		float						m_fHeightScaleT;
		float						m_fWaitTime;
		float						m_fRotationZ;
		float						m_fWaitFreeTime;
		float						m_fContinueTime;//持续时间
		char						m_bContinueTime;
		float						m_fSourceScale;
		float3						m_sourcePos;//技能开始点
		float3						m_targetPos;
		float						m_readyTime;
		char						m_bFree;
		AutoPtr<Mesh>				m_mesh;
		Scene3D*					m_scenePtr;
	};

};

#endif
