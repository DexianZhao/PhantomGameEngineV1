//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   PhantomGameObject.h
软件名称   :   幻影游戏引擎

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___PHANTOMGAMEOBJECT__H___
#define ___PHANTOMGAMEOBJECT__H___
#include "PhantomAStarPath.h"
#include "PhantomGameBase.h"
#include "PhantomDB.h"

namespace Phantom{
	class Scene3D;

#define MessageBufferLength		128
#define	MAX_ENEMY_INs			5//最多同时5个角色进行可攻击范围计算
#pragma pack(push,1)
	struct	SceneID
	{
		unsigned int	 id;
		unsigned short	 index;
		inline	BOOL	operator == (const SceneID& g){return (id==g.id&&index==g.index);}
		SceneID(){id=index=0;}
	};
	struct	GameBornData{//游戏角色出生时数据
		SceneID			sceneid;//角色场景对象ID
		unsigned short	lineID;//roles.txt配置文件里面的行ID
		float3			pos;//出生点
		float			rot;//出生转动率
		float			range;//范围
		float			atkrange;
		float			walkSpeed,runSpeed;
		float			height;
	};
	struct	GameSceneID{
		GameID			gameid;
		SceneID			sceneid;
		char			scene;//场景索引,0永远都是指向主场景
		GameSceneID(){scene=0;}
	};
	struct	GameMessage{
		enum MID{
			ToScene_PosRot			=	1,//pos+rot,发送移动数据,移动数据肯定附带转动数据
			ToScene_Rot				=	2,//发送转动数据
			ToScene_Action			=	3,//发送最新动作
			ToScene_Attack			=	4,//发送攻击动作
			ToScene_Hit				=	5,//造成伤害
			ToScene_BloodProgress	=	6,//更新血条信息
			ToScene_Die				=	7,//角色死亡
			ToScene_Skill			=	8,//播放技能
			ToScene_ResetID			=	9,//重新复活等待，会重置id
			ToScene_Reborn			=	10,//重新复活计算
			ToScene_Max				=	999,//发送到场景最大消息数量
			//
			ToGame_Born			=	1000,//初始化游戏角色对象
			ToGame_ClickObject	=	1001,//点击对象
			ToGame_ClickTerrain	=	1002,//电机地形
			ToGame_AttackOver	=	1003,//攻击结束
			ToGame_AttackHit	=	1004,//攻击时可造成伤害点
			ToGame_DieOver		=	1005,//死亡结束
			ToGame_ResetID		=	1006,//重置id
			ToGame_InitScene	=	1007,//初始化场景
			ToGame_InitPlot		=	1008,//初始化剧情
			ToGame_PlotNext		=	1009,//进入下一个剧情
			ToGame_PlotFinish	=	1010,//结束剧情
			//
		};
		GameMessage();
		~GameMessage();
		unsigned short				mid;//message id
		GameSceneID					uid;//角色数据
		unsigned short				length;
		unsigned char				data[MessageBufferLength];//基础数据长度
		unsigned char*				ptr;//分配更多的数据
		inline	unsigned char*		GetDataPtr(){
			if(ptr)return ptr;
			return data;
		}
	};
#pragma pack(pop)
	
	enum SceneEventID{
		SceneEventID_Init	=	0,//初始化成功
		SceneEventID_Open,//打开场景时被调用
		SceneEventID_Click,//点击对象成功
		SceneEventID_ClickTerrain,//左键点击地面成功
		SceneEventID_ClickTerrain2,//右键点击地面成功
		SceneEventID_IdleOver,//每个角色静止结束之后调用
		SceneEventID_MoveOver,//每个角色移动结束之后
		SceneEventID_Next,//进入下一个剧情
		SceneEventID_Finish,//结束剧情
		SceneEventID_EnterRegion,//进入某个区域
		SceneEventID_Count,
	};
	enum AIEvent{
		AIEvent_OnBorn	=	0,//初始化一个GameObject的AI
		AIEvent_OnIdleOver,//一个GameObject空闲
		AIEvent_OnAttackOver,//一个GameObject攻击
		AIEvent_OnMoveOver,//一个GameObject移动完毕
		AIEvent_OnDieOver,//一个GameObject死亡
		AIEvent_OnByAttack,//被攻击时候
		AIEvent_OnEnemyIn,//进入警戒区域
		AIEvent_OnTargetMissing,//丢失目标对象
		AIEvent_OnKillEnemy,//打死目标
		AIEvent_OnNextBegin,//下一个动作开始时调用
		AIEvent_OnActionEvent,
		AIEvent_OnActionOver,//一个动作结束之后
		AIEvent_Count,
	};
	enum SkillEvent{
		SkillEvent_Hit = 0,//技能裁决
		//SkillEvent_Hit,//技能裁决,对目标点
		SkillEvent_Count,//技能事件数量
	};
	enum PlotEvent{

		PlotEvent_Count,
	};
	class	GameMgr;
	class	SceneObject;
	//游戏对象类,删除对象流程是死亡状态多久之后自动删除
	class	GameObject{
	public://enums
		enum{
			State_Born	=	0,//没有初始化
			State_None,//没有任何状态
			State_Idle,//空闲状态
			State_Skill,//技能状态
			State_Move,//移动状态
			State_Hit,//被击状态
			State_Die,//死亡状态
			State_Reborn,//等待重新刷新复活
			State_Action,//播放动作
			State_MoveDir,//按照方向移动
			State_Action2,//播放动作
			State_Count,
		};
		enum{
			SkillMethod_Now = 0,//立即模式
			SkillMethod_Fly = 1,//飞行模式
			SkillMethod_Group = 2,//群体模式
		};
	public:
		void					die();//GameObject* enemy);//死亡
		bool					moveto(const float3& target);//移动到目标点
		bool					movetoTarget();//移动到目标附近
		bool					movetoE(const char* name);//移动到目标点
		bool					nexttoE(const char* name);//移动到目标点
		bool					nextto(const float3& pos);//移动到目标点
		bool					nextskill_t(int skillid);//移动到目标点
		bool					nextidle(float fTime);
		bool					nextaction(int act, float fTime);
		void					setTarget(GameObject* obj);//设置目标对象
		GameObject*				getTarget();//获得目标对象
		virtual	void			movetoBorn();//移动到出生点
		bool					attack(short actionindex);//攻击动作类型自动选择，如果攻击成功返回true
		void					lookat(const float3& pos);
		void					lookatTarget();
		//
		void					cancelAttack();
		//
		bool					doskill(short skillid, const float3& pos);//使用技能攻击,skillid==0则表示使用默认的物理攻击
		bool					doskillTarget(short skillid);//使用技能攻击目标,skillid==0则表示使用默认的物理攻击
		void					idle(float fTime);//休闲动作自动选择
		UIControl*				onhit(GameObject* enemy, int val, float fPlayTime);//被击动作自动选择
		float					readval(const char* name);//读取整数
		void					writeval(const char* name, float val);//写入整数
		bool					isDead();
		void					setReborn(float fRebornTime);
		//
		bool					raiseSkillEvent(SkillEvent evt);
		bool					raiseAIEvent(AIEvent evt, GameObject* target = 0, int param1 = 0);//引发一个事件，并且参数按照自定义方式压入
		void					frameMove(double dElapsedTime);//AI计算，时间间隔
		void					onAttackHit();//攻击被击响应
		void					onAttackOver();//攻击结束
		void					onDieOver();//死亡结束
		void					onActionEvent(int type);//
		void					onTargetMissing();
		VOID					onKillObject(GameObject* obj);
		inline	ConfigLine*		GetData(){return m_data;}
		const char*				GetName();
		//
		inline	GameSceneID&	GetID(){return m_id;}
		VOID					Setup(const GameBornData& born);//初始化数据
		virtual	BOOL			IsLeaderObject(){return false;}
		//
		void					setAction(char actiontype, char actionindex);//动作类型和类型索引,比如一个类型idle动作有三个不同的表现方式,actionindex就是0到2
		void					PlayAction(int act, float fTime);
		void					PlayAction2(int act, int loop);
		void					setSkill(int skillid);//设置技能表行
		BOOL					SetEnemyIn(GameObject* obj);//如果是新进入的则返回true
		BOOL					AddEnemeyATK(GameObject* obj);//增加一个攻击我的敌人
		BOOL					SelectNextEnemy();
		void					updateBloodProgress(float fProgress);//发送最新生命值并且计算是否死亡
		void					pause(float fSecond);//暂停时间
		void					AddEvent(int eventid, const char* func);
		//
		void					sendSkill(unsigned short meshfileindex, GameObject* target, float fContinueTime = 0/*持续时间*/);//发送技能引发消息
		void					sendSkill(unsigned short meshfileindex, float fContinueTime = 0/*持续时间*/);//发送技能引发消息
		void					sendSkill(unsigned short meshfileindex, const float3& pos, float fContinueTime = 0/*持续时间*/);//发送技能引发消息
		void					sendAttack(short actionindex);//攻击动作类型自动选择，如果攻击成功返回true
		//
		void					movedir_enable(BOOL b);
		void					movedir(const float3& dir);
		inline	int				GetState(){return this->m_state;}
		VOID					SetPosition(const float3& pos);
		void					setRot(float rot);
		void					ResetAll();
		bool					onNext();
		void					clearNext();
		static GameID			s_method,s_actionindex,s_toself,s_health,s_atk_speed,s_hitmesh;
		int						m_content;
		char					m_visible;
		float					m_fHeight;
		char					m_bMeshHeight;//是否启动模型高度取得，不能所有的角色都是用这个，否则效率开销很大，所以一般只需要设置主角的高度从模型高度取得就可以了
		char					m_bUseFindway;//是否使用寻路系统
		char					m_bHitText;
		ConfigLine*				m_skillPtr;//当前技能
		float					m_skillRange;//当前选择的技能或者普通攻击的可攻击距离
		float					m_movedir;//移动方向
		char					m_bMoveDir;
		GameObject*				Clone();//克隆一个一模一样的对象
		SceneObject*			GetObject();
		//void					SetPosition(const float3& pos);
		float3					GetPosition();
		float					GetRotation();
		void					SetRotation(float f);
		void					onMoveOver();//移动完毕
	protected:
		GameObject(GameMgr* mgr, ConfigLine* from, BOOL bCopy = true);//复制一行数据
		GameObject();
		virtual ~GameObject();
		GameSceneID				m_id;
		ConfigLine*				m_data;//数据行记录
		ConfigLine*				m_aiPtr;
		ConfigLine*				m_physicalPtr;//当前
		char					m_powerID;//势力ID
		GameID					m_enemyIns[MAX_ENEMY_INs];////最多同时MAX_ENEMY_INs个角色进行可攻击范围计算
		char					m_enemyInCount;//当前敌人进入数量
		GameID					m_enemyAtks[MAX_ENEMY_INs];////攻击我的敌人列表
		char					m_enemyAtkCount;//当前攻击我的敌人数量
		float					m_fPauseTime;//暂停时间
		float					m_fWalkSpeed,m_fRunSpeed;
		//
		char					m_bIsDataCopy;//是否复制了数据
		struct	DataVal{
			NameT<16>			name;
			float				val;
		};
		CDynamicArrayS<DataVal, 0>	m_vals;//自定义变量列表
		struct	EventVal{
			char				eventid;
			Name				func;
		};
		CDynamicArrayS<EventVal, 0>	m_events;//事件绑定列表
		char					m_state;//AI状态
		//
		friend class			GameMgr;
	public:
		enum{
			MoveTo_Pos = 0,
			MoveTo_Target,
			MoveTo_TargetSkill,
			MoveTo_Born,//返回出生点
		};
		struct NextData{
			enum{
				Type_Move = 0,
				Type_Idle,
				Type_Action,
				Type_LookatObj,
				Type_LookatPos,
				Type_Skill,
				Type_Loop,
				Type_Action2,
			};
			char				type;
			union{
				struct{
					float		fx,fy,fz;
				};
				struct{
					int			ix,iy,iz;
				};
			};
			short				id;
			float				rnd;
		};
		CDynamicArrayS<NextData,0>	m_nexts;
		void					NextPause(BOOL b);
		char					m_nextPause;
		short					m_nextIndex;//下一个动作ID
		short					m_nextLoopIndex;
		char					m_moveType;//移动类型
		char					m_currentAction,m_currentActionIndex;//动作相关
		//float					m_rotation;//转动率
		float					m_range;//影响范围
		float					m_atkrange;//攻击范围
		//位置信息
		float3					m_bornPos;//出生点位置
		//float3					m_pos;//当前位置
		float					m_bornRot;
		//move state
		float3					m_target;//目标位置
		//idle state
		float					m_idleTime;//本次休闲时间
		//attack state
		GameID					m_targetID;//目标指向类
		char					m_bTargetMissing;//目标是否已经死亡了
		char					m_bTargetOut;//目标是否已经离开过
		char					m_bIsDead;//是否已经死亡
		float					m_atkTime;//攻击时间间隔
		char					m_bAttackHit;//是否反馈过被击消息
		char					m_bAttackOver;//是否反馈过被击消息
		char					m_bRunMode;
		float					m_fActionTime;
		char					m_nActionType;
		char					m_regionIndex;//区域索引
		//die state
		float					m_reviveTime;//复活时间，如果没有复活则删除指针
	};

	class	GameLeader: public GameObject
	{
	public:
		VOID					Setup(SceneObject* obj);//初始化数据
		virtual	void			movetoBorn();//移动到出生点
		void					frameMove(double dElapsedTime);//AI计算，时间间隔
		virtual	BOOL			IsLeaderObject(){return true;}
	protected:
		GameLeader(GameMgr* mgr, ConfigLine* from);
		virtual ~GameLeader();
		friend class			GameMgr;
	};

};

#endif
