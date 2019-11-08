#include "PhantomGameObject.h"
#include "PhantomManager.h"
#include "APILua.h"
#include "LuaManager.h"
#include "PhantomScene3DUnits.h"
extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}
#include "lua/lua_tinker.h"
using namespace lua_tinker;
namespace Phantom{
	GameID			GameObject::s_method;
	GameID			GameObject::s_actionindex;
	GameID			GameObject::s_toself;
	GameID			GameObject::s_health;
	GameID			GameObject::s_atk_speed;
	GameID			GameObject::s_hitmesh;
	GameObject::GameObject(GameMgr* mgr, ConfigLine* from, BOOL bCopy)
	{
		m_bIsDataCopy		=	bCopy;
		m_id.gameid.id		=	mgr->GetNewIdentity();
		if(m_bIsDataCopy)
			m_data		=	from->Clone(mgr);
		else
			m_data		=	from;
		m_regionIndex	=	-1;
		m_range		=	0;
		m_atkrange	=	0;
		m_movedir	=	0;
		m_bHitText	=	true;
		m_bMoveDir	=	false;
		m_state		=	State_Born;//AI状态
		m_bornPos.setxyz(0,0,0);//出生点位置
		//m_pos.setxyz(0,0,0);//当前位置
		m_target.setxyz(0,0,0);//目标位置
		m_fWalkSpeed	=	m_fRunSpeed = To3DValue(1);
		//m_moveSpeed		=	To3DValue(1);//移动速度
		m_idleTime		=	0;//本次休闲时间
		m_reviveTime	=	15;//复活时间，如果没有复活则删除指针
		m_skillRange	=	To3DValue(1);
		m_bTargetOut	=	false;
		m_bIsDead		=	false;
		m_fActionTime = 0;
		m_bMeshHeight	=	false;
		m_nActionType = 0;
		m_nextIndex = 0;//下一个动作ID
		m_nextLoopIndex	=	-1;
		m_fHeight		=	0;
		m_visible		=	true;
		m_bUseFindway	=	false;
		m_bRunMode		=	0;
		m_atkTime		=	0;
		m_fPauseTime	=	0;
		m_bAttackHit	=	true;
		m_bTargetMissing	=	false;
		m_nextPause		=	false;
		m_content		=	0;
		static GameID physicalid;
		static GameID atkrange;
		static GameID powerid;
		m_physicalPtr	=	from->GetOtherLinePtr(physicalid, "physicalid");
		ConfigLine* power = from->GetOtherLinePtr(powerid, "powerid");
		//
		m_bAttackOver	=	true;
		m_skillPtr		=	0;
		setSkill(0);
		m_currentAction	=	-1;
		m_currentActionIndex	=	0;
//		m_rotation		=	0;
		m_aiPtr			=	0;
		m_powerID		=	0;//势力ID
		m_bornRot		=	0;
		m_moveType		=	MoveTo_Pos;
		int aiid		=	from->SearchCol("aiid");
		m_aiPtr			=	from->GetOtherLinePtr(aiid);
		if(power)
			m_powerID = power->GetLineID();
		memset(m_enemyIns, 0, sizeof(m_enemyIns));
		m_enemyInCount	=	0;
		memset(m_enemyAtks, 0, sizeof(m_enemyAtks));
		m_enemyAtkCount	=	0;
	}
	GameObject::~GameObject(){
		if(m_bIsDataCopy)
			safe_delete(m_data);
	}
	BOOL			GameObject::AddEnemeyATK(GameObject* obj)//增加一个攻击我的敌人
	{CPUTimeT(GameObject);
		for(int i=0;i<m_enemyAtkCount;i++){
			if(m_enemyAtks[i].id==obj->m_id.gameid.id)
				return false;
		}
		if(m_enemyAtkCount>=MAX_ENEMY_INs)
			return false;
		m_enemyAtks[m_enemyAtkCount] = obj->m_id.gameid;
		m_enemyAtkCount++;
		raiseAIEvent(Phantom::AIEvent_OnEnemyIn, obj);
		return true;
	}
	BOOL			GameObject::SetEnemyIn(GameObject* obj)//如果是新进入的则返回true
	{CPUTimeT(GameObject);
		for(int i=0;i<m_enemyInCount;i++){
			if(m_enemyIns[i].id==obj->m_id.gameid.id)
				return false;
		}
		if(m_enemyInCount>=MAX_ENEMY_INs)
			return false;
		m_enemyIns[m_enemyInCount] = obj->m_id.gameid;
		m_enemyInCount++;
		raiseAIEvent(Phantom::AIEvent_OnEnemyIn, obj);
		return true;
	}
	void			GameObject::setSkill(int skillid)//设置技能表行
	{CPUTimeNone();
		if(skillid==0)
			m_skillPtr = m_physicalPtr;
		else
		{
			ConfigLine* l = GetGameMgr()->m_tblSkill->GetLineFromID(skillid);
			if(!l)return;
			m_skillPtr = l;
		}
		if(!m_skillPtr)return;
		static GameID s_skillRange;
		m_skillRange = To3DValue(m_skillPtr->GetDoubleVal(s_skillRange, "range"));
	}
	void			GameObject::setAction(char actiontype, char actionindex)//动作类型和类型索引,比如一个类型idle动作有三个不同的表现方式,actionindex就是0到2
	{CPUTimeNone();
		if(m_currentAction==actiontype&&m_currentActionIndex==actionindex)
			return;
		m_currentAction = actiontype;
		m_currentActionIndex = actionindex;
		char ptr[MessageBufferLength];
		streamWrite w(ptr, MessageBufferLength);
		//
		w << m_currentAction << m_currentActionIndex;
		GetGameMgr()->send(GameMessage::ToScene_Action, this->m_id, w.buffer(), w.size());
	}
	SceneObject*	GameObject::GetObject()
	{
		return GetGameMgr()->GetScenePtr()->SearchObject(this->m_id.sceneid);
	}
	VOID			GameObject::Setup(const GameBornData& born)//初始化数据
	{CPUTimeT(GameObject);
		this->m_bornPos = born.pos;
		this->m_id.sceneid = born.sceneid;
		m_range		=	born.range;
		m_atkrange	=	born.atkrange;
//		m_rotation	=	born.rot;
		m_bornRot	=	born.rot;
		m_fWalkSpeed	=	born.walkSpeed;
		m_fRunSpeed		=	born.runSpeed;
		m_fHeight		=	born.height;
	}
	void			GameObject::onDieOver()
	{CPUTimeNone();
		this->raiseAIEvent(Phantom::AIEvent_OnDieOver);
	}
	BOOL			GameObject::SelectNextEnemy()
	{CPUTimeNone();
		while(m_enemyAtkCount>0)
		{
			m_enemyAtkCount--;
			GameObject* obj = GetGameMgr()->Search(this->m_enemyAtks[m_enemyAtkCount]);
			if(obj){
				this->setTarget(obj);
				return true;
			}
		}
		while(m_enemyInCount>0)
		{
			m_enemyInCount--;
			GameObject* obj = GetGameMgr()->Search(this->m_enemyIns[m_enemyInCount]);
			if(obj){
				this->setTarget(obj);
				return true;
			}
		}
		return false;
	}
	void			GameObject::movedir_enable(BOOL b)
	{CPUTimeNone();
		if(m_state == GameObject::State_MoveDir&&!b)
		{
			this->raiseAIEvent(Phantom::AIEvent_OnMoveOver);
		}
		m_bMoveDir = 0;
		if(b)
		{
			m_state = GameObject::State_MoveDir;
			m_movedir = 0;
		}
		SceneObject* obj = GetObject();
		if(obj)
		{
			obj->GetMeshPtr()->MoveDirStop();
		}
	}
	void			GameObject::movedir(const float3& dir)
	{CPUTimeNone();
		float r = dir.R();
		if(r<0.01f)
		{
			m_bMoveDir = 0;
			m_movedir = 0;
			return;
		}
		m_bMoveDir = true;
		m_movedir = getRotateZ(dir);
		//float3 dir2 = GetCameraPtr()->GetTarget() - GetCameraPtr()->GetEye();
		//dir2.z = 0;
		//dir2.normalize();
		//dir2.RotateZ(dir2, m_movedir);
		SceneObject* obj = GetObject();
		if(obj)
		{
			//obj->SetRotation(getRotateZ(dir2) + PI);
			obj->GetMeshPtr()->MoveDir(dir, m_currentAction=(this->m_bRunMode?Phantom::ActionType_Run:Phantom::ActionType_Walk));
		}
//		m_rotation = getRotateZ(dir2) + PI;
	}
	VOID			GameObject::onKillObject(GameObject* obj)
	{CPUTimeNone();
		this->raiseAIEvent(Phantom::AIEvent_OnKillEnemy, obj);
	}
	void			GameObject::die(/*GameObject* enemy*/)//死亡
	{CPUTimeNone();
		this->cancelAttack();
		if(!m_bIsDead){
			this->m_state = State_Die;
			m_bIsDead	=	true;
			m_currentAction = Phantom::ActionType_Die;
			m_currentActionIndex = -1;
			GetGameMgr()->RebuildInScenes();
			m_events.resize(0);
			char ptr[MessageBufferLength];
			streamWrite w(ptr, MessageBufferLength);
			w << m_currentAction << m_currentActionIndex;
			GetGameMgr()->send(GameMessage::ToScene_Die, this->m_id, w.buffer(), w.size());
		}
		this->clearNext();
		//if(enemy){
		//	enemy->onKillObject(this);
		//}
	}
	bool			GameObject::isDead(){CPUTimeNone();
		return (m_bIsDead>0);
	}
	float			GameObject::GetRotation()
	{
		SceneObject* obj = GetObject();
		if(obj)
			return obj->m_fRotationZ;
		return 0;
	}
	void			GameObject::SetRotation(float f)
	{
		SceneObject* obj = GetObject();
		if(obj)
			obj->SetRotation(f);
	}
	bool			GameObject::moveto(const float3& target)//移动到目标点
	{CPUTimeNone();
		if(!GetGameMgr()->m_scenePtr->IsMoveGridEnable(target))//判断目标点是否可以进入
		{
			idle(1.0);
			return false;
		}
		m_target = target;
		this->m_state = State_Move;
		m_moveType = MoveTo_Pos;
		this->cancelAttack();
		SceneObject* obj = GetObject();
		if(obj)
			obj->GetMeshPtr()->MoveTo(target, m_currentAction=(this->m_bRunMode?Phantom::ActionType_Run:Phantom::ActionType_Walk));
		return true;
	}
	void			GameObject::SetPosition(const float3& pos)
	{
		SceneObject* obj = GetObject();
		if(obj)
			obj->SetPosition(pos);
	}
	float3			GameObject::GetPosition()
	{
		SceneObject* obj = GetObject();
		if(obj)
			return obj->GetPosition();
		return float3(0,0,0);
	}
	bool			GameObject::movetoTarget()//移动到目标附近
	{CPUTimeNone();
		GameObject* obj = getTarget();
		if(!obj)return false;
		bool ret = moveto(obj->GetPosition());
		m_moveType = MoveTo_Target;
		return ret;
	}
	void			GameObject::setTarget(GameObject* obj)//设置目标对象
	{CPUTimeNone();
		if(!obj||obj==this){
			m_targetID.id=m_targetID.index=0;
			return;
		}
		if(this->m_bIsDead)return;
		this->m_targetID	= obj->m_id.gameid;
		m_bTargetOut		=	false;
		m_bTargetMissing	=	false;
		this->cancelAttack();
	}
	GameObject*		GameObject::getTarget()//获得目标对象
	{CPUTimeNone();
		BOOL bReset = false;
		GameObject* obj = GetGameMgr()->Search(m_targetID, &bReset);
		if(bReset)//如果目标被删除了或者目标死亡
		{
			onTargetMissing();
		}
		return obj;
	}
	void			GameObject::movetoBorn()//移动到出生点
	{CPUTimeNone();
		this->moveto(this->m_bornPos);
		this->m_moveType = MoveTo_Born;
	}
	void			GameObject::lookat(const float3& pos){CPUTimeNone();
		float3 dir = pos - GetPosition();
		//m_rotation = getRotateZ(dir) + PI;
		SceneObject* obj = GetObject();
		if(obj)
			obj->SetRotation(getRotateZ(dir) + PI);
		//GetGameMgr()->send(GameMessage::ToScene_Rot, this->m_id, &m_rotation, sizeof(float));
	}
	void			GameObject::lookatTarget()
	{CPUTimeNone();
		GameObject* target = getTarget();
		if(!target)return;
		lookat(target->GetPosition());
	}
	bool			GameObject::attack(short actionindex)//攻击动作类型自动选择，如果攻击成功返回true
	{CPUTimeNone();
		this->onAttackOver();
		m_atkTime	=	m_data->GetDoubleVal(s_atk_speed, "atktime");
		m_bAttackHit	=	false;
		m_bAttackOver	=	false;
		this->lookatTarget();
		sendAttack(actionindex);
		return true;
	}
	void			GameObject::sendSkill(unsigned short meshfileindex, GameObject* target, float fContinueTime)
	{CPUTimeNone();
		if(!target)return;
		if(!GetGameMgr()->GetScenePtr())return;
		GetGameMgr()->GetScenePtr()->StartSkill(m_id, target->m_id, float3(0,0,0), SceneGameSkill::SGS_ToTarget, meshfileindex, fContinueTime);

		//char ptr[MessageBufferLength];
		//streamWrite w(ptr, MessageBufferLength);
		//GameSceneID targetid;
		//targetid = target->m_id;
		//float3 tpos(0,0,0);
		//char toTarget = SceneGameSkill::SGS_ToTarget;
		//w << meshfileindex << targetid << tpos << toTarget << fContinueTime;
		//GetGameMgr()->send(GameMessage::ToScene_Skill, this->m_id, w.buffer(), w.size());
	}
	void			GameObject::sendSkill(unsigned short meshfileindex, float fContinueTime)
	{CPUTimeNone();
		if(!GetGameMgr()->GetScenePtr())return;
		GetGameMgr()->GetScenePtr()->StartSkill(m_id, m_id, float3(0,0,0), SceneGameSkill::SGS_InSource, meshfileindex, fContinueTime);
		//char ptr[MessageBufferLength];
		//streamWrite w(ptr, MessageBufferLength);
		//GameSceneID targetid;
		//targetid = m_id;
		//float3 tpos(0,0,0);
		//char toTarget = SceneGameSkill::SGS_InSource;
		//w << meshfileindex << targetid << tpos << toTarget << fContinueTime;
		//GetGameMgr()->send(GameMessage::ToScene_Skill, this->m_id, w.buffer(), w.size());
	}
	void			GameObject::sendSkill(unsigned short meshfileindex, const float3& pos, float fContinueTime)
	{CPUTimeNone();
		if(!GetGameMgr()->GetScenePtr())return;
		GetGameMgr()->GetScenePtr()->StartSkill(m_id, m_id, pos, SceneGameSkill::SGS_InPoint, meshfileindex, fContinueTime);

		//char ptr[MessageBufferLength];
		//streamWrite w(ptr, MessageBufferLength);
		//GameSceneID targetid;
		//float3 tpos = pos;
		//char toTarget = SceneGameSkill::SGS_InPoint;
		//w << meshfileindex << targetid << tpos << toTarget << fContinueTime;
		//GetGameMgr()->send(GameMessage::ToScene_Skill, this->m_id, w.buffer(), w.size());
	}
	void			GameObject::cancelAttack()
	{CPUTimeNone();
		m_atkTime = 0;
		m_bAttackHit	=	true;
		m_bAttackOver	=	true;
	}
	void			GameObject::sendAttack(short actionindex)//攻击动作类型自动选择，如果攻击成功返回true
	{CPUTimeNone();
		this->m_state = GameObject::State_None;
		m_currentAction = Phantom::ActionType_Attack;
		m_currentActionIndex = actionindex;
		char ptr[MessageBufferLength];
		streamWrite w(ptr, MessageBufferLength);
		//
		w << m_currentAction << m_currentActionIndex << m_atkTime;
		GetGameMgr()->send(GameMessage::ToScene_Attack, this->m_id, w.buffer(), w.size());
	}
	bool			GameObject::doskillTarget(short skillid)//使用技能攻击目标,skillid==0则表示使用默认的物理攻击
	{CPUTimeNone();
		this->cancelAttack();
		//if(m_atkTime>0.001f)return false;
		GameObject* obj = getTarget();
		if(!obj)
			return false;
		setSkill(skillid);
		if(m_skillPtr)
			attack(m_skillPtr->GetIntVal(s_actionindex, "actionindex"));
		//m_moveType=MoveTo_TargetSkill;
		//m_state = GameObject::State_Move;
		return true;
	}
	bool			GameObject::doskill(short skillid, const float3& pos)//使用技能攻击
	{CPUTimeNone();
		this->cancelAttack();
		//if(m_atkTime>0.001f)return false;
		setSkill(skillid);
		if(m_skillPtr)
			attack(m_skillPtr->GetIntVal(s_actionindex, "actionindex"));
//		m_target	=	pos;
//		m_state		=	GameObject::State_Skill;
		return true;
	}
	void			GameObject::idle(float fTime)//休闲动作自动选择
	{CPUTimeNone();
		this->cancelAttack();
		this->m_state = GameObject::State_Idle;
		this->m_idleTime = fTime;
	}
	GameObject*		GameObject::Clone()
	{
		SceneObject* sobj = GetGameMgr()->GetScenePtr()->SearchObject(GetID().sceneid);
		SceneObject* dobj = GetGameMgr()->GetScenePtr()->CloneAndBind(sobj);
		GameObject* obj = GetGameMgr()->Search(dobj->m_gameID.gameid);
		if(!obj)return 0;
		//obj->m_content = m_content;
		//obj->m_visible = m_visible;
		//obj->m_fHeight = m_fHeight;
		//obj->m_bMeshHeight = m_bMeshHeight;
		//obj->m_bUseFindway = m_bUseFindway;
		//obj->m_skillRange = m_skillRange;
		//obj->m_movedir = m_movedir;
		//obj->m_bMoveDir = m_bMoveDir;
		////
		//obj->m_powerID = m_powerID;
		//obj->m_fPauseTime = m_fPauseTime;
		//obj->m_fWalkSpeed = m_fWalkSpeed;
		//obj->m_fRunSpeed = m_fRunSpeed;
		//obj->m_state = m_state;
		////
		//obj->m_nextLoop = m_nextLoop;
		//obj->m_nextIndex = m_nextIndex;
		//obj->m_moveType = m_moveType;
		//obj->m_currentAction = m_currentAction;
		//obj->m_currentActionIndex = m_currentActionIndex;
		//obj->m_rotation = m_rotation;
		//obj->m_atkrange = m_atkrange;
		//obj->m_bornPos = m_bornPos;
		//obj->m_pos = m_pos;
		//obj->m_bornRot = m_bornRot;
		//obj->m_target = m_target;
		//obj->m_idleTime = m_idleTime;
		//obj->m_targetID = m_targetID;
		//obj->m_bTargetMissing = m_bTargetMissing;
		//obj->m_bTargetOut = m_bTargetOut;
		//obj->m_bIsDead = m_bIsDead;
		//obj->m_atkTime = m_atkTime;
		//obj->m_bAttackHit = m_bAttackHit;
		//obj->m_bAttackOver = m_bAttackOver;
		//obj->m_bRunMode = m_bRunMode;
		//obj->m_fActionTime = m_fActionTime;
		//obj->m_nActionType = m_nActionType;
		//obj->m_regionIndex = m_regionIndex;
		//obj->m_reviveTime = m_reviveTime;
		return obj;
	}
	UIControl*			GameObject::onhit(GameObject* enemy, int val, float fPlayTime)//被击动作自动选择
	{CPUTimeNone();
		//this->m_state = GameObject::State_Hit;
		if(enemy){
			this->raiseAIEvent(Phantom::AIEvent_OnByAttack, enemy);
		}
		//char buf[128];
		//streamWrite w(buf, 128);
		//w << val << fPlayTime;
		//GetGameMgr()->send(GameMessage::ToScene_Hit, this->m_id, w.buffer(), w.size());
		//this->AddEnemeyATK(enemy);
		//if(!enemy->m_skillPtr)
		//	return;
		if(m_skillPtr){
			int mindex = enemy->m_skillPtr->GetIntVal(s_hitmesh, "hitmesh");
			if(mindex>=0)
				this->sendSkill(mindex);
		}
		SceneObject* obj = this->GetObject();
		if(obj&&m_bHitText)
			return obj->AddHit(val, 0, this->m_bHitText);
		return 0;
	}
	void			GameObject::updateBloodProgress(float fProgress)
	{CPUTimeNone();
		GetGameMgr()->send(GameMessage::ToScene_BloodProgress, this->m_id, &fProgress, sizeof(float));
	}
	void			GameObject::onAttackHit()
	{CPUTimeNone();
		if(m_bAttackHit)return;
		m_bAttackHit	=	true;
		if(!this->m_skillPtr)
			return;//如果当前没有技能则返回false
		int method = m_skillPtr->GetIntVal(s_method, "method");
		switch(method){
		case SkillMethod_Now:
			{//如果是立即引发模式
				raiseSkillEvent(Phantom::SkillEvent_Hit);
			}
			break;
		}
	}
	float			GameObject::readval(const char* name)//读取整数
	{CPUTimeNone();
		for(int i=0;i<m_vals.size();i++){
			if(m_vals[i].name==name)
				return m_vals[i].val;
		}
		return 0;
	}
	void			GameObject::writeval(const char* name, float val)
	{CPUTimeNone();
		for(int i=0;i<m_vals.size();i++){
			if(m_vals[i].name==name)
			{
				m_vals[i].val = val;
				return;
			}
		}
		DataVal v;
		v.name = name;
		v.val = val;
		m_vals.push_back(v);
	}
	void			GameObject::pause(float fSecond)//暂停时间
	{CPUTimeNone();
		this->m_fPauseTime = fSecond;
	}
	void			GameObject::onTargetMissing(){CPUTimeNone();
		if(m_bTargetMissing)return;
		m_bTargetMissing = true;
		m_targetID.id=0;
		m_targetID.index=0;
		this->raiseAIEvent(Phantom::AIEvent_OnTargetMissing);
	}
	bool			GameObject::movetoE(const char* name)//移动到目标点
	{CPUTimeNone();
		ScenePoint* pos = GetGameMgr()->GetScenePtr()->GetActiveEnv()->GetPointByName(name);
		if(!pos)return false;
		return moveto(pos->GetPosition());
	}
	bool			GameObject::nexttoE(const char* name)//移动到目标点
	{CPUTimeNone();
		ScenePoint* pos = GetGameMgr()->GetScenePtr()->GetActiveEnv()->GetPointByName(name);
		if(!pos)return false;
		return nextto(pos->GetPosition());
	}
	bool			GameObject::nextto(const float3& pos)//移动到目标点
	{CPUTimeNone();
		NextData data;
		memset(&data, 0, sizeof(NextData));
		data.fx = pos.x;
		data.fy = pos.y;
		data.fz = pos.z;
		data.type = NextData::Type_Move;
		m_nexts.push_back(data);
		return true;
	}
	bool			GameObject::nextskill_t(int skillid)//移动到目标点
	{CPUTimeNone();
		NextData data;
		memset(&data, 0, sizeof(NextData));
		data.type = NextData::Type_Skill;
		data.ix = skillid;
		m_nexts.push_back(data);
		return true;
	}
	bool			GameObject::nextidle(float fTime)
	{CPUTimeNone();
		NextData data;
		memset(&data, 0, sizeof(NextData));
		data.fx = fTime;
		data.type = NextData::Type_Idle;
		m_nexts.push_back(data);
		return true;
	}
	bool			GameObject::nextaction(int act, float fTime)
	{CPUTimeNone();
		NextData data;
		memset(&data, 0, sizeof(NextData));
		data.ix = act;
		data.fy = fTime;
		data.type = NextData::Type_Action;
		m_nexts.push_back(data);
		return true;
	}
	void			GameObject::NextPause(BOOL b)
	{
		m_nextPause = b;
		if(!m_nextPause)
			onNext();
	}
	bool			GameObject::onNext()
	{CPUTimeT(GameObject);
		if(!m_nextPause&&m_nexts.size()>0){
			int nrnd = 0;
			while(m_nexts.size()>0){
				//LogInfo("Play:%d/%d,%d", m_nextIndex,m_nexts.size(), m_nextLoopIndex);
				if(m_nextIndex>=m_nexts.size()){
					m_nextIndex = 0;
					m_nexts.clear();
					return false;
				}
				//m_nextIndex %= m_nexts.size();
				NextData data = m_nexts[m_nextIndex];
				if(data.type==NextData::Type_Loop)
				{
					if(m_nextLoopIndex<0)
						m_nextLoopIndex = data.ix;
					else
						m_nextLoopIndex++;
					if(m_nextLoopIndex>data.iy){
						data.iz--;
						m_nextLoopIndex = data.ix;
					}
					if(data.iz<=0||(m_nextLoopIndex<0||m_nextLoopIndex>=m_nexts.size()))
					{
						m_nextIndex++;
						if(m_nextIndex>=m_nexts.size()){
							m_nextIndex = 0;
							m_nexts.clear();
							return false;
						}
						data = m_nexts[m_nextIndex];
					}
					else{
						data = m_nexts[m_nextLoopIndex];
					}
				}
				else
				{
					m_nextIndex++;
					m_nextLoopIndex = -1;
				}
				if(RandomF(0,1)<data.rnd)
				{
					nrnd++;
					if(nrnd>=20)
						return false;
					continue;
				}
				if(data.id>0)
					raiseAIEvent(Phantom::AIEvent_OnNextBegin, 0, data.id);
				switch(data.type){
				case NextData::Type_Move:
					{
						if(moveto(float3(data.fx,data.fy,data.fz)))
							return true;
					}
					break;
				case NextData::Type_Skill:
					{
						if(this->doskillTarget((int)data.ix))
							return true;
					}
					break;
				case NextData::Type_LookatObj:
					{
						GameID temp;
						temp.id = (int)data.ix;
						temp.index = (int)data.iy;
						GameObject* objs = GetGameMgr()->Search(temp);
						if(objs)
							lookat(objs->GetPosition());
						continue;
					}
					break;
				case NextData::Type_LookatPos:
					{
						lookat(float3(data.fx,data.fy,data.fz));
						continue;
					}
					break;
				case NextData::Type_Idle:
					{
						idle(data.fx);
						return true;
					}
					break;
				case NextData::Type_Action:
					{
						PlayAction(data.ix, data.fy);
						return true;
					}
					break;
				case NextData::Type_Action2:
					{
						PlayAction2(data.ix, data.iy);
						return true;
					}
					break;
				}
			}
		}
		return false;
	}
	void			GameObject::PlayAction(int act, float fTime)
	{
		this->m_nActionType=(int)act;
		this->m_fActionTime = fTime;
		this->m_state=this->State_Action;
		this->m_currentAction = -1;
		this->m_currentActionIndex = -1;
		SceneObject* obj = GetObject();
		if(obj)
		{
			obj->SetAction((ActionType)act, 0, -1);//setAction(m_nActionType, 0);
			obj->GetMeshPtr()->MoveDirStop();
		}
	}
	void			GameObject::PlayAction2(int act, int loop)
	{
		this->m_nActionType=(int)act;
		this->m_state=this->State_Action2;
		this->m_currentAction = -1;
		this->m_currentActionIndex = -1;
		SceneObject* obj = GetObject();
		if(obj)
		{
			obj->SetAction((ActionType)act, 0, loop);
			obj->GetMeshPtr()->MoveDirStop();
			obj->GetMeshPtr()->m_bLoopEndMode = true;
		}
	}

	void			GameObject::onActionEvent(int type)
	{
		raiseAIEvent(Phantom::AIEvent_OnActionEvent, 0, type);
	}
	void			GameObject::onAttackOver()//攻击结束
	{CPUTimeNone();
		if(m_bAttackOver)return;
		m_bAttackOver = true;
		this->onAttackHit();
		m_atkTime = 0;
		if(!raiseAIEvent(Phantom::AIEvent_OnAttackOver, this->getTarget()))
			idle(RandomF(1,5));
	}
	void			GameObject::clearNext(){CPUTimeNone();
		m_nexts.clear();
		m_nextIndex = 0;//下一个动作ID
		m_nextLoopIndex	=	-1;
	}
	void			GameObject::setReborn(float fRebornTime)
	{CPUTimeNone();
		ResetAll();
		m_id.gameid.id = GetGameMgr()->GetNewIdentity();
		m_state = GameObject::State_Reborn;
		m_reviveTime = fRebornTime;
	}
	void			GameObject::setRot(float rot)
	{CPUTimeNone();
		SceneObject* obj = GetObject();
		if(obj)
			obj->SetRotation(rot);
		//this->m_rotation = rot;
		//float4 v;
		//v.setXYZW(m_pos.x, m_pos.y, m_pos.z, m_rotation);
		//GetGameMgr()->send(GameMessage::ToScene_PosRot, this->m_id, &v, sizeof(float4));
	}
	//VOID			GameObject::SetPosition(const float3& pos)
	//{CPUTimeNone();
	//	SceneObject* obj = GetObject();
	//	if(obj)
	//		obj->SetPosition(pos);
	//	//m_pos = pos;
	//	//float4 v;
	//	//v.setXYZW(m_pos.x, m_pos.y, m_pos.z, m_rotation);
	//	//GetGameMgr()->send(GameMessage::ToScene_PosRot, this->m_id, &v, sizeof(float4));
	//}
	void			GameObject::onMoveOver()//移动完毕
	{
		this->m_state = GameObject::State_None;
		if(m_moveType==MoveTo_Born){
			GetObject()->SetRotation(m_bornRot);
		}
		if(m_bMeshHeight)
		{
			float3 pos = GetObject()->GetPosition();
			pos.z = GetGameMgr()->m_scenePtr->GetHeight(pos);
			GetObject()->SetPosition(pos);
		}
		if(onNext())
			return;
		raiseAIEvent(Phantom::AIEvent_OnMoveOver);
	}
	void			GameObject::AddEvent(int eventid, const char* func)
	{
		for(int i=0;i<m_events.size();i++){
			if(m_events[i].eventid==eventid){
				m_events[i].func = func;
				return;
			}
		}
		EventVal e;
		e.eventid = eventid;
		e.func = func;
		m_events.push_back(e);
	}
	void			GameObject::frameMove(double dElapsedTime)//AI计算，时间间隔
	{CPUTimeT(GameObject);
		if(this->m_bIsDead){
			if(m_state == GameObject::State_Reborn){
				m_reviveTime -= dElapsedTime;
				if(m_reviveTime<=0){
					m_state		=	State_Born;
					this->m_bIsDead = false;
					GetGameMgr()->send(GameMessage::ToScene_Reborn, this->m_id, &m_bornPos, sizeof(float3));
					GetGameMgr()->RebuildInScenes();
				}
			}
			return;
		}
		m_fPauseTime -= dElapsedTime;
		if(m_fPauseTime>0)return;
		GameObject* target = getTarget();
		if(target&&target->m_bIsDead){//如果目标已经死亡
			this->onTargetMissing();
			target = 0;
		}
		if(m_atkTime>0){
			m_atkTime-=dElapsedTime;
			if(m_atkTime<=0){
				onAttackOver();
			}
		}
		bool bRecalcRegion = false;
		switch(this->m_state){
		case GameObject::State_Action2:
			{
				//setAction(m_nActionType, 0);
				//if(m_fActionTime<=0){
				//	if(onNext())
				//		break;
				//	m_state = GameObject::State_Idle;
				//}
			}
			break;
		case GameObject::State_Action:
			{
				this->m_fActionTime -= dElapsedTime;
				if(m_fActionTime<=0){
					if(onNext())
						break;
					m_state = GameObject::State_Idle;
					this->raiseAIEvent(Phantom::AIEvent_OnActionOver, 0, this->m_nActionType);
				}
			}
			break;
		case GameObject::State_Move:
			{
				//GetObject()->GetPosition();
				//float fMove = (m_bRunMode?m_fRunSpeed:m_fWalkSpeed) * dElapsedTime;
				//if(m_moveType==MoveTo_Target||m_moveType==MoveTo_TargetSkill)
				//{
				//	if(!target)
				//	{
				//		m_state = GameObject::State_None;
				//		onTargetMissing();
				//		break;
				//	}
				//	m_target	=	target->m_pos;
				//}
				//bRecalcRegion = true;
				////下一步是判断可攻击距离,然后攻击目标,先可以使用普通攻击,然后对象之间进行碰撞检测,不能互相叠加在一个地方
				//float3 dir = m_target - m_pos;
				//dir.z = 0;
				//float r = dir.R();
				//bool bOver = false;
				//if(r<fMove){//移动结束
				//	this->m_state = GameObject::State_None;
				//	bOver = true;
				//	m_pos = m_target;
				//}
				//else if(m_moveType==MoveTo_TargetSkill&&r<this->m_skillRange){
				//	this->m_state = GameObject::State_None;
				//	bOver = true;
				//}
				//else
				//{
				//	m_rotation = getRotateZ(dir) + PI;
				//	m_pos = m_target - dir*((r-fMove)/r);
				//}
				//if(bOver&&m_moveType==MoveTo_Born){
				//	m_rotation = this->m_bornRot;
				//}
				//if(m_bMeshHeight)
				//	GetGameMgr()->m_scenePtr->GetHeight(m_pos, m_pos.z);
				//else
				//	m_pos.z = GetGameMgr()->m_scenePtr->GetHeight(m_pos);
				//float4 v;
				//v.setXYZW(m_pos.x, m_pos.y, m_pos.z, m_rotation);
				//GetGameMgr()->send(GameMessage::ToScene_PosRot, this->m_id, &v, sizeof(float4));
				//setAction(m_bRunMode?Phantom::ActionType_Run:Phantom::ActionType_Walk, 0);
				//if(bOver){
				//	if(onNext())
				//		break;
				//	if(m_moveType==MoveTo_TargetSkill){
				//		if(m_skillPtr)
				//			attack(m_skillPtr->GetIntVal(s_actionindex, "actionindex"));
				//		break;
				//	}
				//	raiseAIEvent(Phantom::AIEvent_OnMoveOver);
				//}
		//	}
		//	break;
		//case GameObject::State_MoveDir:
		//	{
		//		float fMove = (m_bRunMode?m_fRunSpeed:m_fWalkSpeed) * dElapsedTime;
		//		if(!m_bMoveDir)
		//			break;
		//		float3 dir = GetCameraPtr()->GetTarget() - GetCameraPtr()->GetEye();
		//		dir.z = 0;
		//		dir.normalize();
		//		dir.RotateZ(dir, m_movedir);
		//		bRecalcRegion = true;
		//		//下一步是判断可攻击距离,然后攻击目标,先可以使用普通攻击,然后对象之间进行碰撞检测,不能互相叠加在一个地方
		//		m_rotation = getRotateZ(dir) + PI;
		//		m_pos += dir*fMove;
		//		if(m_bMeshHeight)
		//			GetGameMgr()->m_scenePtr->GetHeight(m_pos, m_pos.z);
		//		else
		//			m_pos.z = GetGameMgr()->m_scenePtr->GetHeight(m_pos);
		//		float4 v;
		//		v.setXYZW(m_pos.x, m_pos.y, m_pos.z, m_rotation);
		//		GetGameMgr()->send(GameMessage::ToScene_PosRot, this->m_id, &v, sizeof(float4));
		//		setAction(m_bRunMode?Phantom::ActionType_Run:Phantom::ActionType_Walk, 0);
			}
			break;
		case GameObject::State_Skill:
			{
				this->m_state = GameObject::State_None;
//				raiseSkillEvent(Phantom::SkillEvent_DoSkillPos);
			}
			break;
		case GameObject::State_Born:
			{
				this->m_state = GameObject::State_None;
				raiseAIEvent(Phantom::AIEvent_OnBorn);//
				bRecalcRegion = true;
			}
			break;
		case GameObject::State_Idle:
			{
				m_idleTime -= dElapsedTime;
				setAction(Phantom::ActionType_Idle, -1);
				if(m_idleTime<=0){
					if(onNext())
						break;
					this->m_state = GameObject::State_None;
					raiseAIEvent(Phantom::AIEvent_OnIdleOver);//
				}
			}
			break;
		case GameObject::State_Hit:
			{
			}
			break;
		case GameObject::State_Die:
			{
			}
			return;
		}
		if(bRecalcRegion&&IsLeaderObject()){
			int region = GetGameMgr()->GetScenePtr()->GetRegionIndex(GetObject()->GetPosition());
			if(m_regionIndex!=region)
			{
				m_regionIndex = region;
				SceneRegionData* rd = GetGameMgr()->GetScenePtr()->GetRegionPtr(this->m_regionIndex);
				ConfigLine* sceneLine = GetGameMgr()->m_sceneLine;
				if(sceneLine&&sceneLine->GetScriptPtr()&&rd){
					ScriptBase* sc = sceneLine->GetScriptPtr();
					ConfigLine* regionLine = GetGameMgr()->m_tblCity->GetLineFromID(rd->regionid);
					if(regionLine && sc->IsExist(SceneEventID_EnterRegion)){
						lua_Map3D_s mgr;
						SetIPtr(&mgr.m_data, GetGameMgr()->m_scenePtr, Scene3D, IMap3DPtr);
						lua_GameTable<ConfigMgr::Table_City> regionclass;
						regionclass.m_id = regionLine->GetID();
						char name[128];
						sc->GetEventName(name, 0, SceneEventID_EnterRegion, true);
						lua_tinker::call<void>(g_luamanager.GetLuaState(), name, mgr, regionclass);
					}
				}
			}
		}
		//if(target){
		//	float3 dir = target->m_pos - m_pos;
		//	float fRange = dir.length();
		//	if(!m_bTargetOut && fRange > this->m_atkRange*1.05){//超出可攻击距离1.05被的时候自动调用
		//		this->onTargetMissing();
		//		m_bTargetOut = true;
		//	}
		//	//else if(m_bTargetOut && fRange <this->m_atkRange){
		//	//	m_bTargetOut = false;
		//	//}
		//}
		for(int i=0;i<m_enemyInCount;i++)//敌人走出攻击范围裁决
		{
			GameObject* obj = GetGameMgr()->Search(m_enemyIns[i]);
			bool bRemove = false;
			if(!obj){
				bRemove = true;
			}
			else{
				float3 dir = obj->GetPosition() - GetPosition();//m_pos;
				float fRange = dir.length();
				if(fRange > this->m_atkrange*1.05){//超出可攻击距离1.2被的时候自动调用
					bRemove = true;
				}
			}
			if(bRemove){
				m_enemyInCount--;
				if(m_enemyInCount>i)
					m_enemyIns[i] = m_enemyIns[m_enemyInCount];
				i--;
			}
		}
	}
	bool			GameObject::raiseSkillEvent(SkillEvent evt)
	{CPUTimeT(GameObject);
		if(!m_skillPtr||!m_skillPtr->GetScriptPtr())return false;
		ScriptBase* sc = m_skillPtr->GetScriptPtr();
		if(!sc->IsExist(evt))//如果不存在这个事件则返回
			return false;
		char name[128];
		sc->GetEventName(name, 0, evt, true);
		lua_GameObject_s obj;
		obj.m_data = this->m_id.gameid;
		//
		lua_GameTable<ConfigMgr::Table_Skill> ais;
		ais.m_id = m_skillPtr->GetID();
		SetCurrentLuaFileIndex(sc->m_fileIndex);
		switch(evt){
		case SkillEvent_Hit:
			{
				lua_tinker::call<void>(g_luamanager.GetLuaState(), name, ais, obj);
			}
			break;
		//case SkillEvent_DoSkillPos:
		//	{
		//		lua_float3 pos;
		//		pos.setup(m_target.x, m_target.y, m_target.z);
		//		lua_tinker::call<void>(g_luamanager.GetLuaState(), name, ais, obj, pos);
		//	}
		//	break;
		}
		SetCurrentLuaFileIndex(-1);
		return true;
	}
	bool			GameObject::raiseAIEvent(AIEvent evt, GameObject* target, int param1)//引发一个事件，并且参数按照自定义方式压入
	{CPUTimeT(GameObject);
		if(GetGameMgr()->m_bPlotEnable&&GetGameMgr()->m_sceneLine){
			int pevt = -1;
			switch(evt){
			case AIEvent_OnIdleOver://一个GameObject空闲
				pevt = SceneEventID_IdleOver;
				break;
			case AIEvent_OnMoveOver://一个GameObject移动完毕
				pevt = SceneEventID_MoveOver;
				break;
			}
			ScriptBase* sc = GetGameMgr()->m_sceneLine->GetScriptPtr();
			if(pevt>=0 && sc->IsExist(pevt)){
				char name[128];
				sc->GetEventName(name, 0, pevt, true);
				lua_GameObject_s obj;
				obj.m_data = this->m_id.gameid;
				lua_Map3D_s mgr;
				SetIPtr(&mgr.m_data, GetGameMgr()->m_scenePtr, Scene3D, IMap3DPtr);
				SetCurrentLuaFileIndex(sc->m_fileIndex);
				switch(pevt){
				case SceneEventID_IdleOver://一个GameObject空闲
				case SceneEventID_MoveOver://一个GameObject移动完毕
					{
						lua_tinker::call<void>(g_luamanager.GetLuaState(), name, mgr, obj);
					}
					break;
				}
				SetCurrentLuaFileIndex(-1);
			}
		}
		lua_GameObject_s obj;
		obj.m_data = this->m_id.gameid;
		//
		for(int i=0;i<this->m_events.size();i++){
			if(m_events[i].eventid==evt){
				if(!m_events[i].func.t[0])//如果函数是空名字则返回
					break;
				const char* name = m_events[i].func.t;
				switch(evt){
				case AIEvent_OnIdleOver://一个GameObject空闲
				case AIEvent_OnMoveOver://一个GameObject移动完毕
				case AIEvent_OnDieOver://一个GameObject死亡
					{
						lua_tinker::call<void>(g_luamanager.GetLuaState(), name, obj);
					}
					break;
				case AIEvent_OnByAttack://被攻击时候
				case AIEvent_OnAttackOver:
					{
						lua_GameObject_s tobj;
						if(target)
							tobj.m_data = target->m_id.gameid;
						lua_tinker::call<void>(g_luamanager.GetLuaState(), name, obj, tobj);
					}
					break;
				case AIEvent_OnNextBegin:
				case AIEvent_OnActionOver:
				case AIEvent_OnActionEvent:
					{
						lua_tinker::call<void>(g_luamanager.GetLuaState(), name, obj, param1);
					}
					break;
				}
				return true;
			}
		}
		if(!m_aiPtr||!m_aiPtr->GetScriptPtr())return false;
		ScriptBase* sc = m_aiPtr->GetScriptPtr();
		if(!sc->IsExist(evt))//如果不存在这个事件则返回
			return false;
		char name[128];
		sc->GetEventName(name, 0, evt, true);
		lua_GameTable<ConfigMgr::Table_AI> ais;
		ais.m_id = m_aiPtr->GetID();
		SetCurrentLuaFileIndex(sc->m_fileIndex);
		switch(evt){
		case AIEvent_OnBorn://初始化一个GameObject的AI
		case AIEvent_OnIdleOver://一个GameObject空闲
		case AIEvent_OnAttackOver://一个GameObject攻击
		case AIEvent_OnMoveOver://一个GameObject移动完毕
		case AIEvent_OnDieOver://一个GameObject死亡
		case AIEvent_OnTargetMissing://我的目标走出攻击范围
			{
				lua_tinker::call<void>(g_luamanager.GetLuaState(), name, ais, obj);
			}
			break;
		case AIEvent_OnKillEnemy:
		case AIEvent_OnByAttack://被攻击时候
		case AIEvent_OnEnemyIn://进入警戒区域
			{
				lua_GameObject_s tobj;
				if(target)
					tobj.m_data = target->m_id.gameid;
				lua_tinker::call<void>(g_luamanager.GetLuaState(), name, ais, obj, tobj);
			}
			break;
		case AIEvent_OnNextBegin:
		case AIEvent_OnActionOver:
		case AIEvent_OnActionEvent:
			{
				lua_tinker::call<void>(g_luamanager.GetLuaState(), name, ais, obj, param1);
			}
			break;
		}
		SetCurrentLuaFileIndex(-1);
		return true;
	}
	void			GameObject::ResetAll()
	{CPUTimeT(GameObject);
		m_state		=	State_Born;//重置AI状态
		//m_pos		=	m_bornPos;
		m_bTargetOut	=	false;
		m_fActionTime = 0;
		m_nActionType = 0;
		m_nextIndex = 0;//下一个动作ID
		m_nextLoopIndex	=	-1;
		m_bRunMode		=	0;
		m_atkTime		=	0;
		m_fPauseTime	=	0;
		m_bAttackHit	=	true;
		m_bTargetMissing	=	true;
		m_bAttackOver	=	true;
		m_skillPtr		=	0;
		m_currentAction	=	-1;
		m_currentActionIndex	=	0;
//		m_rotation		=	m_bornRot;
		memset(m_enemyIns, 0, sizeof(m_enemyIns));
		m_enemyInCount	=	0;
		memset(m_enemyAtks, 0, sizeof(m_enemyAtks));
		m_enemyAtkCount	=	0;
	}
	const char*		GameObject::GetName()
	{CPUTimeNone();
		if(!GetGameMgr()->m_scenePtr)
			return "";
		SceneObject* obj = GetGameMgr()->m_scenePtr->SearchObject(this->m_id.sceneid);
		if(!obj)return "";
		return obj->GetName();
	}
	GameLeader::GameLeader(GameMgr* mgr, ConfigLine* from)
		:GameObject(mgr, from, true)
	{
	}
	GameLeader::~GameLeader()
	{
	}
	VOID		GameLeader::Setup(SceneObject* obj)//初始化数据
	{CPUTimeT(GameLeader);
		this->m_bornPos = obj->GetPosition();
		this->m_id.sceneid = obj->m_gameID.sceneid;
//		m_rotation	=	obj->m_fRotationZ;
		m_range		=	obj->m_fRange;
		m_atkrange	=	obj->m_fAtkRange;
		MeshData* md = obj->GetMeshPtr()->GetDataPtr();
		m_fHeight	=	obj->GetObjectHeight();
		for(int k=0;k<md->m_actionMap.size();k++){
			MeshActionRange* act = md->m_actionMap[k];
			if(act->m_actionType==Phantom::ActionType_Walk)
				m_fWalkSpeed = act->m_fMoveOffset;
			else if(act->m_actionType==Phantom::ActionType_Run)
				m_fRunSpeed = act->m_fMoveOffset;
		}
	}
	void		GameLeader::frameMove(double dElapsedTime)//AI计算，时间间隔
	{CPUTimeT(GameLeader);
		GameObject::frameMove(dElapsedTime);
	}
	void		GameLeader::movetoBorn()//移动到出生点
	{CPUTimeT(GameLeader);
		this->moveto(this->m_bornPos);
	}
	GameMessage::GameMessage()
	{
		memset(this, 0, sizeof(GameMessage));
	}
	GameMessage::~GameMessage()
	{
		if(ptr)
			delete[] ptr;
	}

};
