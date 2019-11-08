#include "PhantomScene3D.h"
#include "PhantomManager.h"
#include "APILua.h"
#include "LuaManager.h"

namespace Phantom{

	VOID				GetPelvis(float3& ret, Mesh* m, int boneid, float fBody)
	{
		const matrix4x4* mat = 0;
		if(boneid >= 0)
			mat = m->GetBoneMatrix(boneid);
		float fHeightAdd = 0;
		if(!mat)
		{
			mat = &m->GetWorldMatrix();
			fHeightAdd = m->GetObjectRegion().aabox._max.z * fBody;
		}
		ret = mat->GetPosition();
		ret.z += fHeightAdd;
	}
	ScenePoint::ScenePoint(Scene3D* scenePtr){
		m_scenePtr	=	scenePtr;
		m_id	=	m_scenePtr->GetNewIdentity();
		m_pos.setxyz(0,0,0);
		m_range	=	To3DValue(.2f);
		m_eventid	=	0;//表格中的事件ID
		m_name	=	"event";
		m_bEnabled	=	true;//是否有效
	}
	ScenePoint::~ScenePoint(){
	}
	VOID				ScenePoint::SetPosition(const float3& v)
	{CPUTimeNone();
	m_pos	=	v;
	}
	VOID				ScenePoint::SetRange(float f)
	{CPUTimeNone();
	m_range	=	f;
	}
	ScenePoint*			ScenePoint::Clone()
	{CPUTimeNone();
	ScenePoint* evt	=	m_scenePtr->GetActiveEnv()->InsertPoint(m_pos);
	evt->m_pos		=	m_pos;
	evt->m_range	=	m_range;
	evt->m_eventid	=	m_eventid;
	evt->m_bEnabled	=	m_bEnabled;//是否有效
	sprintf(evt->m_name.t, "%s%d", evt->m_name.t, evt->m_id);
	return evt;
	}
	BOOL				ScenePoint::Save(CSafeFileHelperW& w)
	{CPUTimeNone();
	short version = 1;
	w << version;
	skip_w sw(w.pStream);
	w << m_id << m_pos << m_range << m_eventid << m_bEnabled;
	w.str(m_name);
	sw.end();
	return true;
	}
	BOOL				ScenePoint::Load(CSafeFileHelperR& r)
	{CPUTimeNone();
	short version = 1;
	r >> version;
	skip_r sr;
	sr.begin(r.pStream);
	r >> m_id >> m_pos >> m_range >> m_eventid >> m_bEnabled;
	r.str(m_name);
	sr.end();
	return true;
	}
	SceneObject::SceneObject(Scene3D* s)
	{
		m_diffuseIndex			=	0;
		m_fileNameID			=	-1;
		this->m_pCurrentRegion	=	0;
		this->m_pNext			=	0;
		m_scenePtr		=	s;
		m_bVisible				=	true;
		m_bEnabled				=	true;
		m_bPause				=	false;
		m_binds.setGrow(0);
		m_nextActions.setGrow(0);
		m_fSpeed				=	1;
		m_fScale				=	1.0f;
		m_fRotationZ			=	0.0f;
		m_bloodColor			=	0xffff0000;
		m_bAttackReback			=	false;
		m_nState				=	0;
		m_nSaveState			=	0;
		m_bCustomName			=	false;
		m_bMeshVisible			=	true;
		m_bAutoHeight			=	true;
		m_wayPointIndex = 0;
		m_wayPointLength = 0;
		m_movedir.setxy(1,1);
		m_bShowBlood	=	false;
		m_bShowName		=	false;
		m_fBloodProgress	=	1;//血量
		m_nBipBoneIndex	=	-1;
		m_srb.m_obj = this;
		m_bloodOffset.Set(50,0);
		m_nClass		=	0;
		m_fFly			=	0;
		m_nActionType	=	0;
		m_nActionIndex	=	0;
		m_nSaveActionType = 0;
		m_nSaveActionIndex = 0;
#ifdef EDITOR_OS
		m_worldMat.identity();
#endif
		m_fRange		=	To3DValue(1.0f);
		m_fAtkRange		=	m_fRange+1.0f;
		m_saveMat.identity();
		m_saveRot = 0;
		m_saveVisible	=	true;
		m_gameID.sceneid.id = s->GetNewIdentity();
		m_nContentID			=	m_gameID.sceneid.id;
	}
	int					SceneObject::GetType()
	{CPUTimeNone();
	if(m_meshPtr)
		return m_meshPtr->GetDataPtr()->m_meshType;
	return MeshData::MeshType_StaticMesh;
	}
	SceneObject::~SceneObject(void)
	{
		m_meshPtr	=	0;
		//m_unitPtr	=	0;
		DetachManager();
	}
	VOID	SceneObject::SetVisible(BOOL b)
	{CPUTimeNone();
	m_bVisible = b;
#ifdef EDITOR_OS
	m_scenePtr->OnChangeWorld(this);
#endif
	}

	UIControl*	SceneObject::AddHit(int val, float fWaitTime, BOOL bText)
	{CPUTimeT(SceneObject);
		m_fHitWaitTime = fWaitTime;
		UIControl* ret = 0;
		if(bText)
			ret = this->m_scenePtr->AddHit(this, val);
		if(m_nState!=State_Hit){
			m_nSaveState	=	m_nState;
			m_nSaveActionType = m_nActionType;
			m_nSaveActionIndex = m_nActionIndex;
		}
		m_nState	=	State_Hit;
		this->SetAction(Phantom::ActionType_Hit, -1);
		return ret;
	}
	VOID	SceneObject::StopMove()
	{CPUTimeNone();
	m_nState	=	State_Standard;
	}
	VOID	SceneObject::OnSetAction(const char* actionName)
	{CPUTimeT(SceneObject);
	//if(m_unitPtr)
	//{
	//	Unit3DAction* action = m_unitPtr->SearchActionPtr(actionName);
	//	if(action)
	//	{
	//		m_unitPtr->SetActiveAction(action);
	//		OnBeginAction(action->m_actionPtr->m_actionType, action->GetName());
	//	}
	//	else
	//	{
	//		LogInfo("system->error:Map3DItem中找不到动画%s(%s)", this->m_name.t, actionName);
	//	}
	//}
	//else
	//{
	MeshActionRange* action = m_meshPtr->GetDataPtr()->findAction(actionName);
	if(action){
		m_meshPtr->SetCurrentAction(action->m_actionID, 1);
		OnBeginAction(action->m_actionType, action->m_name.t);
	}
	else
	{
		LogInfo("system->error:Map3DItem中找不到动画%s(%s)", this->m_name.t, actionName);
	}
	//}
	UpdateMoveSpeed();
	}
	VOID	SceneObject::OnSetAction(ActionType type, int index, int nLoop)
	{CPUTimeT(SceneObject);
	//if(m_unitPtr)
	//{
	//	if(m_unitPtr->SetActiveAction(type) && m_unitPtr->m_activePtr)
	//		OnBeginAction(type, m_unitPtr->m_activePtr->GetName());
	//	else if(type)
	//	{
	//		LogInfo("system->error:Map3DItem中找不到动画pushaction(%s)", GetActionTypeName(type));
	//	}
	//}
	//else
	//{
	char aindex = index>=0?index:(int)RandomF(0, Phantom::ActionType_Max);
	if(m_meshPtr->SetActionType(type, nLoop, aindex) && m_meshPtr->m_actionDo.GetBoneGroup())
		OnBeginAction(type, m_meshPtr->m_actionDo.GetBoneGroup()->GetName());
	else if(type)
	{
		//LogInfo("system->error:Map3DItem中找不到动画pushaction(%s)", GetActionTypeName(type));
	}
	m_meshPtr->SetActionSpeed(1);
	//}
	UpdateMoveSpeed();
	}
	BOOL	SceneObject::GetHeight(IntersectData* intersect){CPUTimeT(SceneObject);
	if(!this->m_bVisible)
		return false;
	switch(this->m_meshPtr->GetDataPtr()->m_meshType)
	{
	case MeshData::MeshType_SceneUnit:
		return m_meshPtr->GetHeight(intersect);
	}
	return FALSE;
	}
	VOID	SceneObject::SetAction(const char* actionName)
	{CPUTimeNone();
	m_nActionType = -1;
	m_nActionIndex = 0;
	OnSetAction(actionName);
	SetPause(false);
	m_nextActions.SetArrayCount(0);
	}
	const char*	SceneObject::GetActionName()
	{CPUTimeNone();
	//if(m_unitPtr)
	//{
	//	if(m_unitPtr->m_activePtr)
	//		m_unitPtr->m_activePtr->GetName();
	//}
	//else
	//{
	if(m_meshPtr->GetCurrentAction())
		return m_meshPtr->GetCurrentAction()->GetName();
	//}
	return "";
	}
	ActionType	SceneObject::GetActionType()
	{CPUTimeNone();
	//if(m_unitPtr)
	//{
	//	if(m_unitPtr->m_activePtr && m_unitPtr->m_activePtr->m_actionPtr)
	//		m_unitPtr->m_activePtr->m_actionPtr->m_actionType;
	//}
	//else
	//{
	if(m_meshPtr->GetCurrentAction())
		return (ActionType)m_meshPtr->GetCurrentAction()->m_actionType;
	//}
	return ActionType_None;
	}
	VOID	SceneObject::SetClassID(int cid)
	{CPUTimeNone();
	this->m_nClass = cid;
	this->m_scenePtr->m_classChanged = true;
	}
	VOID	SceneObject::SetActionBeginFunc(LPCSTR szLuaBegin)
	{CPUTimeNone();
	if(szLuaBegin)
		m_actionBeginFunc = szLuaBegin;
	else
		m_actionBeginFunc.t[0] = 0;
	}
	VOID	SceneObject::SetActionEndFunc(LPCSTR szLuaEnd)
	{CPUTimeNone();
	if(szLuaEnd)
		m_actionEndFunc = szLuaEnd;
	else
		m_actionEndFunc.t[0] = 0;
	}
	VOID	SceneObject::SetAttack(ActionType type, int index, float fAtkTime)
	{CPUTimeNone();
	m_bAttackReback	=	false;
	SetAction(type, m_nActionIndex);
	if(m_meshPtr)
		m_meshPtr->SetActionTime(fAtkTime);
	m_nState	=	this->State_Attack;
	}
	VOID	SceneObject::SetDie(ActionType type, int index)
	{CPUTimeNone();
	m_bAttackReback	=	false;
	SetAction(type, m_nActionIndex);
	m_nState	=	this->State_Die;
	this->m_bShowBlood = false;
	m_bEnabled	=	false;
	}
	VOID	SceneObject::SetAction(ActionType type, int index, int nLoop)
	{CPUTimeNone();
	m_nActionType = type;
	m_nActionIndex = index;
	OnSetAction(type, m_nActionIndex, nLoop);
	SetPause(false);
	m_nextActions.SetArrayCount(0);
	if(type!=ActionType_Hit){
		m_nSaveActionType = m_nActionType;
		m_nSaveActionIndex = m_nActionIndex;
	}
	}
	VOID	SceneObject::SetNextAction(const char* actionName)
	{CPUTimeNone();
	NextAction next;
	next.mode = 2;
	next.name = actionName;
	this->m_nextActions.push_back(next);
	}
	VOID	SceneObject::SetNextAction(ActionType type)
	{CPUTimeNone();
	NextAction next;
	next.mode = 1;
	next.type = type;
	this->m_nextActions.push_back(next);
	}
	VOID	SceneObject::SetFly(float f)
	{CPUTimeNone();
	m_fFly	=	f;
	float3 v = this->GetPosition();
	v.z = GetScenePtr()->GetHeight(v) + this->m_fFly;
	this->SetPosition(v);
	}
	Mesh*	SceneObject::GetMeshPtr()
	{CPUTimeNone();
	//if(m_unitPtr)
	//	return m_unitPtr->GetGeometryPtr();
	return m_meshPtr;
	}
	VOID				SceneObject::bind_event(int eventID, const char* szLuaFunc)
	{CPUTimeNone();
	for(int i=0;i<m_binds.size();i++)
	{
		if(m_binds[i].id == eventID)
		{
			m_binds[i].func = szLuaFunc;
			return;
		}
	}
	BindEvent m;
	m.id = eventID;
	m.func = szLuaFunc;
	m_binds.push_back(m);
	}
	BOOL				SceneObject::LoadObject(int fileNameID, Mesh* mesh)
	{CPUTimeT(SceneObject);
	m_fileNameID = fileNameID;
	SceneObjectFile* obj = this->m_scenePtr->GetObjectFile(m_fileNameID);
	if(!obj)
		return false;
	if(m_name.t[0] == 0){
		FileName f = obj->name.t;
		f.setIsFile();
		FileName n;
		f.splitpath(0, &n, 0);
		m_name = n.t;
	}
	if(mesh->GetDataPtr()->m_lineID){
		if(GetGameMgr()->m_tblRoles){
			ConfigLine* line = GetGameMgr()->m_tblRoles->GetLineFromID(mesh->GetDataPtr()->m_lineID);
			if(line){
				m_name = line->GetLineName();
			}
		}
	}
	m_meshPtr = mesh;
	m_meshPtr->m_callback = this;
	m_nBipBoneIndex	=	m_meshPtr->GetBoneIDFromName("bip01");
	m_meshPtr->ChangeDiffuse(m_diffuseIndex);
	ChangeMeshTransform();
	return true;
	}
	BOOL				SceneObject::LoadObject(int fileNameID)
	{CPUTimeT(SceneObject);
	m_fileNameID = fileNameID;
	SceneObjectFile* obj = this->m_scenePtr->GetObjectFile(m_fileNameID);
	if(!obj)
		return false;
	if(m_name.t[0] == 0){
		FileName f = obj->name.t;
		f.setIsFile();
		FileName n;
		f.splitpath(0, &n, 0);
		m_name = n.t;
	}
	if(!g_manager.LoadMesh(this->m_meshPtr, obj->name.t))
		return false;
	m_meshPtr->m_callback = this;
	m_nBipBoneIndex	=	m_meshPtr->GetBoneIDFromName("bip01");
	m_meshPtr->ChangeDiffuse(m_diffuseIndex);
	ChangeMeshTransform();
	return true;
	}
	VOID				SceneObject::SetSpeed(float f)
	{CPUTimeNone();
	this->m_fSpeed = f;
	}
	VOID				SceneObject::SetScale(float f)
	{CPUTimeNone();
	this->m_fScale = f;
	ChangeMeshTransform();
	UpdateMoveSpeed();
#ifdef EDITOR_OS
	this->m_scenePtr->OnChangeWorld(this);
#endif
	}
	/*VOID				SceneObject::AttackBy(Mesh* mesh, LPCSTR szLikeBone, const Pixel* from, float fBody)
	{
	if(m_nState==this->State_ByAttack)
	return;
	Mesh* temp = mesh->GetDataPtr()->CreateRuntime();
	m_byTargetMesh = temp;
	temp->Release();
	m_fTargetBody = fBody;
	if(m_byTargetMesh)
	{
	m_byTargetMesh->SetVisible(true);
	m_byTargetMesh->m_callback = this;
	m_nTargetBoneID = -1;
	if(szLikeBone&&szLikeBone[0])
	m_nTargetBoneID	=	GetMeshPtr()->GetBoneIDLike(szLikeBone);
	float3 ret;
	GetPelvis(ret, GetMeshPtr(), m_nTargetBoneID, m_fTargetBody);
	m_byTargetMesh->SetPosition(ret);
	m_byTargetMesh->SetCurrentAction(0,1);
	}
	if(from)
	{
	m_nSaveState	=	this->m_nState;
	this->ByAttack(from, -1);
	}
	}*/
	VOID				SceneObject::UpdateMoveSpeed()
	{CPUTimeNone();
	Mesh* meshPtr = GetMeshPtr();
	if(!meshPtr)return;
	if(meshPtr&&meshPtr->GetCurrentAction()&&meshPtr->GetCurrentAction()->m_fMoveOffset)
		m_fSpeed	=	meshPtr->GetCurrentAction()->m_fMoveOffset * m_fScale;
	else
		m_fSpeed	=	1;
	}
	VOID				SceneObject::SetBlood(float f)
	{CPUTimeNone();
	this->m_fBloodProgress = clamp(f, 0, 1);
	}
	VOID				SceneObject::SetRotation(float fZ)
	{CPUTimeNone();
	this->m_fRotationZ = fZ;
	if(GetMeshPtr())GetMeshPtr()->m_rotation.z = fZ;
	this->ChangeMeshTransform();
#ifdef EDITOR_OS
	this->m_scenePtr->OnChangeWorld(this);
#endif
	}
	SceneObject*		SceneObject::Clone()
	{CPUTimeNone();
	SceneObject* obj	=	0;
	//if(m_unitPtr)
	//	obj = m_scenePtr->InsertObject(m_unitPtr->GetDataPtr()->m_fileName.t, *this->GetPosition(), GetWorldPtr());
	//else 
	if(m_meshPtr)
		obj = m_scenePtr->InsertObject(m_meshPtr->GetDataPtr()->m_filename.t, this->GetPosition(), GetWorldPtr());
	if(!obj)
		return 0;
	obj->SetPause(this->m_bPause);
	obj->m_bloodOffset		=	this->m_bloodOffset;
	obj->m_binds			=	this->m_binds;
	obj->m_ways				=	this->m_ways;
	obj->m_name				=	this->m_name;
	obj->m_moveEndFunc		=	this->m_moveEndFunc;
	obj->m_actionBeginFunc	=	this->m_actionBeginFunc;
	obj->m_actionEndFunc	=	this->m_actionEndFunc;
	obj->m_nextActions		=	this->m_nextActions;
	obj->m_bShowBlood		=	this->m_bShowBlood;
	obj->m_bShowName		=	this->m_bShowName;
	obj->m_fBloodProgress	=	this->m_fBloodProgress;
	obj->m_nBipBoneIndex	=	this->m_nBipBoneIndex;
	obj->m_nState			=	this->m_nState;
	obj->m_fFly				=	this->m_fFly;
	obj->m_fSpeed			=	this->m_fSpeed;
	obj->m_fScale			=	this->m_fScale;
	obj->m_fRotationZ		=	this->m_fRotationZ;
	if(GetMeshPtr()){
		obj->GetMeshPtr()->m_rotation = GetMeshPtr()->m_rotation;
	}
	//obj->m_color			=	this->m_color;
	obj->m_wayPointIndex		=	this->m_wayPointIndex;
	obj->m_powerSide		=	this->m_powerSide;
	obj->m_wayPointLength		=	this->m_wayPointLength;
	obj->m_movedir			=	this->m_movedir;
	obj->m_nClass			=	this->m_nClass;
	//obj->m_type				=	this->m_type;
#ifdef EDITOR_OS
	obj->m_worldMat			=	this->m_worldMat;
#endif
	obj->m_bVisible			=	this->m_bVisible;
	obj->m_bEnabled			=	this->m_bEnabled;
	obj->m_fileNameID		=	this->m_fileNameID;
	obj->m_columnCount		=	m_columnCount;
	obj->m_saveMat			=	m_saveMat;
	obj->m_saveRot			=	m_saveRot;
	obj->m_saveVisible		=	m_saveVisible;
	obj->m_fRange			=	m_fRange;
	obj->m_fAtkRange		=	m_fAtkRange;
	if(m_lineDB&&m_columnCount>0){
		obj->m_lineDB = new SceneObjectDB[m_columnCount];
		for(int i=0;i<m_columnCount;i++){
			obj->m_lineDB[i] = this->m_lineDB[i];
			if(m_lineDB[i].textPtr)
			{
				int len = strlen(m_lineDB[i].textPtr);
				obj->m_lineDB[i].textPtr = new char[len+1];
				strcpy(obj->m_lineDB[i].textPtr, m_lineDB[i].textPtr);
			}
		}
	}
	obj->GetMeshPtr()->SetWorldMatrix(GetMeshPtr()->GetWorldMatrix());
	m_scenePtr->AddObjectType(obj);
	return obj;
	}
	VOID						SceneObject::SetParent(SceneObject* obj)
	{CPUTimeNone();
	SceneObject* old = m_scenePtr->SearchObject(m_parentID);
	if(old)
		old->RemoveChild(this);
	if(obj)
	{
		m_parentID = obj->m_gameID.sceneid;
		obj->AddChild(this);
	}
	else
		m_parentID.id = 0;
	}
	SceneObject*				SceneObject::GetParent()
	{CPUTimeNone();
	return m_scenePtr->SearchObject(m_parentID);
	}
	VOID						SceneObject::Reborn(const float3& pos)
	{CPUTimeNone();
	this->SetPosition(pos);
	this->m_bVisible = true;
	this->m_bEnabled = true;
	this->m_meshPtr->Pause(false);
	m_meshPtr->SetAlpha(1.0f);
	m_meshPtr->ClearChilds();
	}
	VOID						SceneObject::AddChild(SceneObject* obj)
	{CPUTimeNone();
	for(int i=0;i<m_childs.size();i++){
		if(m_childs[i].id==obj->m_gameID.sceneid.id)
			return;
	}
	m_childs.push_back(obj->m_gameID.sceneid);
	}
	VOID						SceneObject::RemoveChild(SceneObject* obj)
	{CPUTimeNone();
	if(!obj)return;
	for(int i=0;i<m_childs.size();i++){
		if(m_childs[i].id==obj->m_gameID.sceneid.id){
			m_childs.eraseByIndex(i);
			break;
		}
	}
	}

	void				SceneObject::ChangeMeshTransform()
	{CPUTimeT(SceneObject);
	float3 oldpos = GetMeshPtr()->GetPosition();//m_worldMat.GetPosition();
	matrix4x4 temp;
	if((int)(m_fScale*1000) != 1000)
	{
		matrix4x4 matR;
		matrixScaling(&temp, this->m_fScale, this->m_fScale, this->m_fScale);
		matrixRotationZ(&matR, this->GetMeshPtr()->m_rotation.z);
		matrixMultiply(&temp, &temp, &matR);
	}
	else
	{
		matrixRotationZ(&temp, this->GetMeshPtr()->m_rotation.z);
	}
	if(m_meshPtr&&m_meshPtr->GetDataPtr()->m_meshType!=MeshData::MeshType_StaticMesh && this->m_bAutoHeight){
		oldpos.z += 5.0f;
		oldpos.z = m_scenePtr->GetHeight(oldpos) + m_fFly;
	}
	temp.SetPosition(oldpos);
#ifdef EDITOR_OS
	m_worldMat = temp;
#endif
	if(m_meshPtr)
		this->m_meshPtr->SetWorldMatrix(temp, false);
	for(int i=0;i<m_childs.size();i++){
		SceneObject* c = m_scenePtr->SearchObject(m_childs[i]);
		if(!c){
			i--;
			m_childs.eraseByIndex(i);
			continue;
		}
		c->SetWorld(&temp);//m_worldMat);
	}
	}
	//VOID				SceneObject::SetColor(const color4& c)
	//{
	//	this->m_color = c;
	//	if(m_color.a<0.99)
	//	{
	//	}
	//	else
	//	{
	//	}
	//}
	VOID				SceneObject::LookatTo(const Pixel* s)
	{CPUTimeNone();
	if(s){
		const matrix4x4& mat = GetMeshPtr()->GetWorldMatrix();
		Pixel dir(s->x - mat.GetPosition().x, s->y - mat.GetPosition().y);
		if(dir.x!=0||dir.y!=0){
			float3 v(dir.x,dir.y,0);
			this->m_fRotationZ = getRotateZ(v) + PI;
			if(GetMeshPtr())GetMeshPtr()->m_rotation.z = m_fRotationZ;
			this->ChangeMeshTransform();
		}
	}
	}
	VOID					SceneObject::OnBeginAction(int actiontype, const char* actionname)
	{CPUTimeNone();
	if(m_actionBeginFunc.t[0])
	{
		lua_Map3DItem_s mgr;
		SetIPtr(&mgr.m_data, this, SceneObject, IMap3DItemPtr);
		lua_tinker::call<void>(g_luamanager.GetLuaState(), m_actionBeginFunc.t, mgr, actiontype, actionname);
	}
	}
	VOID					SceneObject::CheckNextAction()
	{CPUTimeNone();
	if(m_nextActions.size()>0)
	{
		NextAction next = m_nextActions[0];
		m_nextActions.eraseByIndex(0);
		switch(next.mode)
		{
		case 1:
			if(next.type == 0)
			{
				SetPause(true);
			}
			else
			{
				OnSetAction((ActionType)next.type);
			}
			break;
		case 2:
			if(next.name.t[0] == 0)
			{
				SetPause(true);
			}
			else
			{
				OnSetAction(next.name.t);
			}
			break;
		}
	}
	else if(m_nState==State_Die){
		GetGameMgr()->send(GameMessage::ToGame_DieOver, this->m_gameID, 0, 0);
		m_nState = State_Standard;
		this->m_meshPtr->Pause(true);//暂停模型的所有动作
	}
	else if(m_nState==State_Attack)
	{
		if(!m_bAttackReback){
			GetGameMgr()->send(GameMessage::ToGame_AttackHit, this->m_gameID, 0, 0);
			m_bAttackReback = true;
		}
		GetGameMgr()->send(GameMessage::ToGame_AttackOver, this->m_gameID, 0, 0);
		m_nState = State_Standard;
		//OnSetAction(Phantom::ActionType_Idle, 0);
	}
	else if(m_nActionIndex<0&&m_nActionType>=0){
		OnSetAction((ActionType)m_nActionType, m_nActionIndex);
	}
	}
	VOID					SceneObject::ActionPlayOver(Unit3D* m, int actiontype, const char* actionname)
	{CPUTimeT(SceneObject);
	if(m_actionEndFunc.t[0])
	{
		lua_Map3DItem_s mgr;
		SetIPtr(&mgr.m_data, this, SceneObject, IMap3DItemPtr);
		lua_tinker::call<void>(g_luamanager.GetLuaState(), m_actionEndFunc.t, mgr, actiontype, actionname);
	}
	CheckNextAction();
	}
	VOID					SceneObject::ActionAttackBy(Mesh* m, int actionID, int playtype)
	{CPUTimeT(SceneObject);
		if(!m_bAttackReback){
			m_bAttackReback = true;
			GameObject* obj = GetGameMgr()->Search(this->m_gameID.gameid);
			if(obj)
				obj->onAttackHit();//造成伤害裁决,按照技能方法进行不同的裁决
			//GetGameMgr()->send(GameMessage::ToGame_AttackHit, this->m_gameID, 0, 0);
		}
	}
	VOID					SceneObject::ActionEventRaise(Mesh* m, int actiontype)
	{
		GameObject* obj = GetGameMgr()->Search(this->m_gameID.gameid);
		if(obj)
			obj->onActionEvent(actiontype);
	}
	VOID					SceneObject::ActionPlayOver(Mesh* m, int actiontype, const char* actionname)
	{CPUTimeT(SceneObject);
	if(State_Hit==m_nState){
		m_nActionType = m_nSaveActionType;
		m_nActionIndex = m_nSaveActionIndex;
		OnSetAction((ActionType)m_nSaveActionType, m_nSaveActionIndex);
		m_nState = m_nSaveState;
	}
	if(m_actionEndFunc.t[0])
	{
		lua_Map3DItem_s mgr;
		SetIPtr(&mgr.m_data, this, SceneObject, IMap3DItemPtr);
		lua_tinker::call<void>(g_luamanager.GetLuaState(), m_actionEndFunc.t, mgr, actiontype, actionname);
	}
	CheckNextAction();
	if(actionname==0){
		GameObject* obj = GetGameMgr()->Search(this->m_gameID.gameid);
		if(obj)
		{
			if(obj->onNext())
				return;
			obj->idle(0.0f);
			obj->raiseAIEvent(Phantom::AIEvent_OnActionOver, 0, actiontype);
		}
	}
	//if(m == this->GetMeshPtr()){
	/*if(playtype<0||playtype>=State_Count)
	return 0;
	if(playtype == State_ByAttack)
	{
	this->SetState(m_nSaveState);
	return 1;
	}
	if(m_stateFunc[playtype])
	{
	lua_Map3DItem_s mgr;
	SetIPtr(&mgr.m_data, this, SceneObject, IMap3DItemPtr);
	lua_tinker::call<void>(g_luamanager.GetLuaState(), m_stateFunc[playtype]->t, 3, mgr);
	return 1;
	}*/
	//}
	//else if(m == m_byTargetMesh)
	//{
	//	m_byTargetMesh->SetVisible(false);
	//}
	}
	//VOID				SceneObject::SetStateFunc(int nState, const char* szFunc)
	//{
	//	if(!m_stateFunc[nState])
	//		m_stateFunc[nState] = new NameT<32>();
	//	if(szFunc&&szFunc[0])
	//		*this->m_stateFunc[nState] = szFunc;
	//	else
	//		*this->m_stateFunc[nState] = 0;
	//}
	VOID				SceneObject::MoveDir(const Pixel& dir)
	{CPUTimeNone();
	if(dir.x==0&&dir.y==0)
		return;
	this->m_fRotationZ = getRotateZ(float3(dir.x, -dir.y, 0));
	if(GetMeshPtr())GetMeshPtr()->m_rotation.z = m_fRotationZ;
	m_movedir.setxy(-dir.x, dir.y);
	m_movedir.normalize();
	if(m_nState != State_MoveDir)
	{
		m_nState	=	State_MoveDir;
		//SetState(State_MoveDir);
	}
	}
	BOOL				SceneObject::MoveTo(const Pixel& s, const char* szLuaFunc)
	{CPUTimeT(SceneObject);
	if(szLuaFunc)
		m_moveEndFunc = szLuaFunc;
	else
		m_moveEndFunc.t[0] = 0;
	m_wayPointIndex = 0;
	m_wayPointLength = 0;
	float3 to(s.x, s.y, 0);
	const matrix4x4& mat = GetMeshPtr()->GetWorldMatrix();
	float3 pos = mat.GetPosition();
	//if(m_unitPtr)
	//	pos = m_unitPtr->GetWorldMatrix().GetPosition();
	//else
	//	pos = m_meshPtr->GetWorldMatrix().GetPosition();
	if(m_scenePtr->IsEnabled(Scene3D::Enable_MoveGridsData))
	{
		if(!this->m_scenePtr->FindPath(this->m_ways, pos, to))
			return false;
	}
	else
	{
		m_ways.SetArrayCount(2);
		m_ways[0].pos = float2(pos.x, pos.y);
		m_ways[1].pos = float2(to.x, to.y);
	}
	for(int i=0;i<m_ways.size()-1;i++)
	{
		m_ways[i].dir = m_ways[i+1].pos - m_ways[i].pos;
		m_ways[i].length = m_ways[i].dir.length();
		if(m_ways[i].length > 0.01f)
		{
			float3 v(m_ways[i].dir.x,m_ways[i].dir.y,0);
			m_ways[i].fAngle	=	getRotateZ(v) + PI;
		}
		m_ways[i].dir.normalize();
	}
	m_nState	=	State_MoveTo;
	return true;
	}

	const AABox*		SceneObject::GetBox()
	{CPUTimeNone();
	return &GetMeshPtr()->GetObjectRegion().aabox;
	}
	VOID				SceneObject::ResetAll(const float3& pos, float rot)
	{CPUTimeNone();
	this->Change(pos,rot);
	this->m_bVisible = true;
	this->m_bEnabled = true;
	this->m_meshPtr->Pause(false);
	m_meshPtr->SetAlpha(1.0f);
	m_meshPtr->ClearChilds();
	}
	float				SceneObject::GetObjectHeight()
	{CPUTimeNone();
	return (m_meshPtr->GetDataPtr()->m_idleHeight*m_meshPtr->GetDataPtr()->m_fBaseScale*this->m_fScale);//*1.15;
	}
	extern void	AddUVScaleIndex();
	VOID				SceneObject::RenderBlood()
	{CPUTimeT(SceneObject);
	if(!m_bMeshVisible)return;
	float3 pos;
	float fScale;
	Mesh* meshPtr = GetMeshPtr();
	if(m_scenePtr->m_bShowBlood || this->m_bShowBlood || this->m_bShowName||m_scenePtr->m_bHitDisplay)
	{
		//if(m_nBipBoneIndex>=0)
		//{
		//	matrix4x4 mtemp = *meshPtr->m_actionDo.GetBoneMatrix(m_nBipBoneIndex);
		//	matrixMultiply(&mtemp, &mtemp, &m_worldMat);//meshPtr->GetWorldMatrix());
		//	pos = mtemp.GetPosition();
		//	pos.z += meshPtr->GetObjectRegion().aabox._max.z*1.15-(pos.z-m_worldMat._43);//meshPtr->GetWorldMatrix()._43);
		//}
		//else
		//{
		const matrix4x4& mat = GetMeshPtr()->GetWorldMatrix();
		pos = mat.GetPosition();// meshPtr->GetWorldMatrix().GetPosition();
		pos.z += GetObjectHeight()*1.15f;//meshPtr->GetObjectRegion().aabox._max.z*1.15;
		//}
		float3 len = pos - GetCameraPtr()->GetEye();
		fScale = clamp(1 - len.length()/800.0f, 0.05f, 1.0f)*m_scenePtr->m_fBloodSize;
		if(fScale < 0.1f)
			return;
	}
	else
		return;
	Pixel out;
	if(GetCameraPtr()->Get2DFrom3D(out, pos))
	{
		//if(m_scenePtr->m_bloodProgress){
		//g_manager.SetUIDialogSize(m_scenePtr->m_bloodProgress->m_nAlignMode==0, m_scenePtr->m_bloodProgress->m_dialogPtr->m_screenSize.cx, m_scenePtr->m_bloodProgress->m_dialogPtr->m_screenSize.cy, 0);
		//AddUVScaleIndex();
		//m_scenePtr->m_bloodProgress->m_dialogPtr->UpdateDialogSize(m_scenePtr->m_bloodProgress);
		//LogInfo("Screen(%.03f,%.03f,%d,%d,%d,%d,%d,%d,%d)", g_manager.GetUIScale().x, g_manager.GetUIScale().y, g_manager.GetBufferWidth(), g_manager.GetBufferHeight(), g_manager.GetDeviceWidth(), g_manager.GetDeviceHeight(),
		//m_scenePtr->m_bloodProgress->m_dialogPtr->m_screenSize.cx, m_scenePtr->m_bloodProgress->m_dialogPtr->m_screenSize.cy, m_scenePtr->m_bloodProgress->m_nAlignMode);
		//}
		float fSW = 1;
		if(m_scenePtr->m_bloodProgress)
			fSW = (float)g_manager.GetBufferWidth() / (float)m_scenePtr->m_bloodProgress->m_dialogPtr->m_screenSize.cx;
		float fSH = 1;
		if(m_scenePtr->m_bloodProgress)
			fSH = (float)g_manager.GetBufferHeight() / (float)m_scenePtr->m_bloodProgress->m_dialogPtr->m_screenSize.cy;
		if(m_scenePtr->m_bShowBlood&&this->m_bShowBlood)
		{
			if(this->m_scenePtr->m_bloodProgress){
				Rect old = m_scenePtr->m_bloodProgress->m_windowRect;
				float w = (float)old.GetWidth()/2.0f * fSW;
				float h = (float)old.GetHeight()/2.0f * fSH;
				Rect newRect(-(float)w*fScale+out.x, -(float)h*fScale+out.y, (float)w*fScale+out.x, (float)h*fScale+out.y);
				m_scenePtr->m_bloodProgress->DrawProgress(newRect, m_fBloodProgress, this->m_bloodColor);
			}
			else{
				m_scenePtr->m_bloodDraw.SetProgress(this->m_fBloodProgress);
				this->m_scenePtr->m_bloodDraw.Render3D(&pos, UIDrawMgr::DrawMgrMode_Progress, 0, fScale, &m_bloodOffset);
			}
		}
		if(m_scenePtr->m_bHitDisplay){
			for(int i=0;i<m_scenePtr->m_hits.size();i++){
				if(m_scenePtr->m_hits[i].id.id!=m_gameID.sceneid.id)
					continue;
				UIControl* hit = m_scenePtr->m_hits[i].hit;
				Rect old = hit->m_saveWindowRect;
				float w = 200.0f*fSW;//old.GetWidth()/2;
				float h = (float)old.GetHeight()/2.0f*fSH;
				Rect newRect(-(float)w*fScale+out.x, -(float)h*fScale+out.y, (float)w*fScale+out.x, (float)h*fScale+out.y);
				hit->FrameMove(g_manager.GetElapsedTime(), &newRect, true);
				if(hit->m_playdata.m_bIsPlayOver||!hit->m_trackPtr)
				{
					m_scenePtr->m_hits[i].id.id = 0;
				}
				else
					hit->Render(g_manager.GetElapsedTime());
			}
		}
	}
	//if(this->m_bShowName)
	//{
	//	fScale *= 0.9;
	//	Pixel temp = m_bloodOffset;
	//	temp.y -= 30*fScale;
	//	this->m_scenePtr->m_bloodDraw.Render3D(&pos, UIDrawMgr::DrawMgrMode_Text, this->m_name.t, fScale, &temp);
	//}
	}
	VOID				SceneObject::RenderToShadowMap()
	{CPUTimeT(SceneObject);
	if(!this->m_bVisible)
		return;
	if(m_bMeshVisible&&m_meshPtr.t)
		m_meshPtr->RenderToShadowMap();
	}
	VOID				SceneObject::Render()
	{CPUTimeT(SceneObject);
	if(!this->m_bVisible)
		return;
	if(m_bMeshVisible){
		if(m_meshPtr.t)
		{
			m_meshPtr->m_lightPtr = &this->m_light;
			m_meshPtr->Render();
			m_meshPtr->m_lightPtr = 0;
		}
		if(m_scenePtr->m_bShowBlood||this->m_bShowBlood)// || this->m_bShowName)
		{
			const matrix4x4& mat = GetMeshPtr()->GetWorldMatrix();
			g_manager.Push3DDrawMgr(&m_srb, mat.GetPosition());//m_meshPtr->GetWorldMatrix().GetPosition());
		}
	}
	//if(m_byTargetMesh)
	//	m_byTargetMesh->Render();
	}
	VOID				SceneObject::SetPosition(const Pixel& s)
	{CPUTimeNone();
	float3 v(s.x, s.y, this->GetPosition().z);
	v.z = GetScenePtr()->GetHeight(v) + this->m_fFly;
	this->SetPosition(v);
	}
	VOID				SceneObject::FrameMove()
	{CPUTimeT(SceneObject);
	if(!this->m_bVisible)
		return;
	double dElapsedTime = g_manager.GetElapsedTime();
	if(this->m_bPause)
		dElapsedTime = 0;
	//{
	//	switch(m_nState)
	//	{
	//	case State_Hit:
	//		{
	//			m_fHitWaitTime-=dElapsedTime;
	//			if(m_fHitWaitTime<0){
	//				m_nActionType = m_nSaveActionType;
	//				m_nActionIndex = m_nSaveActionIndex;
	//				OnSetAction((ActionType)m_nSaveActionType, m_nSaveActionIndex);
	//				m_nState = m_nSaveState;
	//			}
	//		}
	//		break;
	//	case State_MoveTo:
	//		{
	//			if(m_ways.size() > 0)
	//			{
	//				WayPoint2D& w2d = m_ways[m_wayPointIndex];
	//				m_wayPointLength += dElapsedTime * this->m_fSpeed;
	//				float2 pos;
	//				float rot = 0;
	//				if(this->m_wayPointLength >= w2d.length)
	//				{
	//					m_wayPointLength -= w2d.length;
	//					m_wayPointIndex++;
	//					pos = m_ways[m_wayPointIndex].pos;
	//					rot = m_ways[m_wayPointIndex].fAngle;
	//				}
	//				else
	//				{
	//					pos = w2d.pos + w2d.dir*this->m_wayPointLength;
	//					rot = w2d.fAngle;
	//					this->m_fRotationZ = rot;
	//				}
	//				const matrix4x4& mat = GetMeshPtr()->GetWorldMatrix();
	//				mat.SetPosition(pos.x, pos.y, mat._43);
	//				ChangeMeshTransform();
	//				this->OnChangeWorld();
	//				if(m_wayPointIndex>=(m_ways.size()-1))//移动结束
	//				{
	//					m_ways.SetArrayCount(0);
	//					if(m_moveEndFunc.t[0])
	//					{
	//						lua_Map3DItem_s mgr;
	//						SetIPtr(&mgr.m_data, this, SceneObject, IMap3DItemPtr);
	//						lua_tinker::call<void>(g_luamanager.GetLuaState(), m_moveEndFunc.t, mgr);
	//					}
	//				}
	//			}
	//		}
	//		break;
	//	case State_MoveDir:
	//		{
	//			float fMove = dElapsedTime * this->m_fSpeed;
	//			float3 pos = m_worldMat.GetPosition();
	//			pos.x += m_movedir.x*fMove;
	//			pos.y += m_movedir.y*fMove;
	//			pos.z = m_worldMat._43;//this->m_scenePtr->GetHeight(pos) + m_fFly;
	//			m_worldMat.SetPosition(pos);
	//			ChangeMeshTransform();
	//			this->OnChangeWorld();
	//		}
	//		break;
	//	}
	if(!m_bPause){
		this->m_bMeshVisible=true;
		if(g_manager.IsFogEnable()){
			const matrix4x4& mat = GetMeshPtr()->GetWorldMatrix();
			float3 vPos = mat.GetPosition() - g_manager.GetFogPos();
			if(g_manager.GetFogMode()>=FogMode_Fog2D)
				vPos.z = 0;
			if((vPos.R()-m_meshPtr->GetObjectRegion().sphere.radius)>g_manager.GetFogFar())
			{
				m_bMeshVisible=false;
			}
		}
		if(m_bMeshVisible)
		{
			if(m_lightChanged!=m_scenePtr->m_lightChanged){
				m_lightChanged = m_scenePtr->m_lightChanged;
				m_scenePtr->RebuildLight(m_light, m_meshPtr->GetObjectRegion().sphere.center, this->m_meshPtr->GetObjectRegion().sphere.radius);
			}
			m_meshPtr->FrameMove();
		}
	}
	//if(m_byTargetMesh && m_byTargetMesh->IsVisible())
	//{
	//	float3 pos;
	//	GetPelvis(pos, GetMeshPtr(), this->m_nTargetBoneID, this->m_fTargetBody);
	//	m_byTargetMesh->SetPosition(pos);
	//	m_byTargetMesh->FrameMove();
	//}
	//}
	}
	VOID				SceneObject::SetPause(BOOL b)
	{CPUTimeNone();
	m_bPause = b;
	if(m_meshPtr)
		m_meshPtr->Pause(b);
	}
	const matrix4x4*	SceneObject::GetWorldPtr()
	{CPUTimeNone();
	return &GetMeshPtr()->GetWorldMatrix();;
	}
	VOID				SceneObject::ResetSaveEnv(){CPUTimeNone();
	m_saveRot = m_fRotationZ;
	m_saveVisible = m_bVisible;
#ifdef EDITOR_OS
	m_saveMat	=	m_worldMat;
#else
	m_saveMat.identity();
	m_saveMat.SetPosition(GetMeshPtr()->GetWorldMatrix().GetPosition());
#endif
	}
	VOID				SceneObject::ResetEnv(SavePosRot* spr)//如果spr==0则设置成游戏模式
	{CPUTimeNone();
	if(spr){
		m_fRotationZ = spr->rot;
		m_bVisible = spr->visible;
		SetWorld(&spr->mat);
#ifndef EDITOR_OS
		GameObject* obj = GetGameMgr()->Search(this->m_gameID.gameid);
		if(obj){
			obj->m_bornPos=/*obj->m_pos = */spr->mat.GetPosition();
			obj->m_bornRot=/*obj->m_rotation = */spr->rot;
		}
#endif
	}
	else{
		m_fRotationZ = m_saveRot;
		m_bVisible = m_saveVisible;
		SetWorld(&m_saveMat);
#ifndef EDITOR_OS
		GameObject* obj = GetGameMgr()->Search(this->m_gameID.gameid);
		if(obj){
			obj->m_bornPos=/*obj->m_pos = */m_saveMat.GetPosition();
			obj->m_bornRot=/*obj->m_rotation = */m_saveRot;
		}
#endif
	}

	}
	VOID				SceneObject::SetWorld(const matrix4x4* mat)
	{CPUTimeNone();
	if(!GetMeshPtr())return;
	if(mat)
	{
		GetMeshPtr()->SetWorldMatrix(*mat, false);
#ifdef EDITOR_OS
		m_worldMat = *mat;
#endif
		//if(m_unitPtr)
		//	m_unitPtr->SetWorldMatrix(*mat);
		//else 
		//if(m_meshPtr.t)
		//m_meshPtr->SetWorldMatrix(*mat);
		//else
		//return;
	}
	this->OnChangeWorld();
	}

	VOID				SceneObject::DetachManager()
	{CPUTimeNone();
	if(this->m_pCurrentRegion)
		this->m_pCurrentRegion->RemoveSceneNode(this);
	m_pCurrentRegion	=	0;
	}
	VOID						SceneObject::Change(const float3& pos, float fRot)
	{CPUTimeNone();
	this->m_fRotationZ = fRot;
	if(GetMeshPtr())GetMeshPtr()->m_rotation.z = m_fRotationZ;
	GetMeshPtr()->SetPosition(pos);
#ifdef EDITOR_OS
	m_worldMat.SetPosition(pos);
#endif
	ChangeMeshTransform();
	OnChangeWorld();
	}
	VOID						SceneObject::SetPosition(const float3& v)	//设置物体位置
	{CPUTimeNone();
	if(!GetWorldPtr())return;
	GetMeshPtr()->SetPosition(v);
#ifdef EDITOR_OS
	m_worldMat.SetPosition(v);
#endif
	ChangeMeshTransform();
	OnChangeWorld();
	}

	VOID						SceneObject::EnableSelect(bool bEnable, unsigned int color)	//设置为选择项，并同时设置选择发光颜色
	{CPUTimeNone();
	}

	BOOL						SceneObject::ActionMoveOver(Mesh* m)
	{
		GameObject* obj = GetGameMgr()->Search(this->m_gameID.gameid);
		if(obj)
			obj->onMoveOver();
		return true;
	}
	BOOL						SceneObject::Intersect(IntersectData* intersect, BOOL bCollisionOnly)
	{CPUTimeNone();
	if(!this->m_bEnabled || !this->m_bVisible)
		return false;
	//if(m_binds.size()==0)return false;
	return GetMeshPtr()->PickRay(intersect,bCollisionOnly?Mesh::MeshPick_Collision:Mesh::MeshPick_Full);
	}

	void						SceneObject::OnChangeWorld()
	{CPUTimeNone();
#ifdef EDITOR_OS
	this->m_scenePtr->OnChangeWorld(this);
#endif
	m_lightChanged	=	0;
	if(this->m_pCurrentRegion)
	{
		this->m_pCurrentRegion->OnChangeSceneObject(this);
	}
	}
	VOID	SceneObject::ChangeDiffuse(int index)
	{CPUTimeNone();
	this->m_diffuseIndex = index;
	this->m_meshPtr->ChangeDiffuse(m_diffuseIndex);
	}
	BOOL	SceneObject::Save(CSafeFileHelperW& w)
	{CPUTimeT(SceneObject);
	short version = 8;
	w << version;
	skip_w sw(w.pStream);
	w << m_bloodOffset;
	w.str(m_name);
	w << m_bShowBlood << m_bShowName;
	w << m_fSpeed << m_fScale << m_fRotationZ << m_powerSide;
	int id = m_gameID.sceneid.id;
	matrix4x4 mat = GetMeshPtr()->GetWorldMatrix();
#ifdef EDITOR_OS
	mat = m_worldMat;
#endif
	w << m_nClass << m_fileNameID << id << m_bVisible << m_bEnabled << mat;//场景对象矩阵
	w << m_diffuseIndex;
	w << m_fRange;
	w << m_fAtkRange;
	w << m_saveMat << m_saveRot << m_saveVisible;
	w << m_bCustomName;
	w << m_bAutoHeight;
	w << m_nContentID;
	sw.end();
	return true;
	}
	const char*		SceneObject::GetName(){CPUTimeNone();
	if(this->m_bCustomName)
		return m_name;
	m_name.t[0] = 0;
	if(m_meshPtr->GetDataPtr()->m_meshType==MeshData::MeshType_Role&&GetGameMgr()->m_tblRoles){
		ConfigLine* line = GetGameMgr()->m_tblRoles->GetLineFromID(m_meshPtr->GetDataPtr()->m_lineID);
		if(line)
			m_name = line->GetLineName();
	}
	if(m_meshPtr->GetDataPtr()->m_meshType==MeshData::MeshType_SceneUnit&&GetGameMgr()->m_tblSceneUnit){
		ConfigLine* line = GetGameMgr()->m_tblSceneUnit->GetLineFromID(m_meshPtr->GetDataPtr()->m_lineID);
		if(line)
			m_name = line->GetLineName();
	}
#ifndef EDITOR_OS
	m_bCustomName = true;
#endif
	return m_name;
	}
	BOOL	SceneObject::Load(CSafeFileHelperR& r, BOOL bLoadMesh)
	{CPUTimeT(SceneObject);
	short version = 1;
	r >> version;
	skip_r sr;
	sr.begin(r.pStream);
	r >> m_bloodOffset;
	r.str(m_name);
	r >> m_bShowBlood >> m_bShowName;
	r >> m_fSpeed >> m_fScale >> m_fRotationZ >> m_powerSide;
	int id;
	matrix4x4 worldMat;
	r >> m_nClass >> m_fileNameID >> id >> m_bVisible >> m_bEnabled >> worldMat;//场景对象矩阵

	m_gameID.sceneid.id = id;
	if(version>=2)
		r >> m_diffuseIndex;
	if(version>=3)
		r >> m_fRange;
	if(version>=6)
		r >> m_fAtkRange;
	bool bInit = false;
	if(version>=4)
		r >> m_saveMat >> m_saveRot >> m_saveVisible;
	else
		bInit = true;
	if(version>=5)
		r >> m_bCustomName;
	if(version>=7)
		r >> m_bAutoHeight;
	if(version>=8)
		r >> m_nContentID;
	else
		m_nContentID = id;
	sr.end();
	if(bLoadMesh){
		if(!LoadObject(m_fileNameID))
			return false;
	}
#ifdef EDITOR_OS
	m_worldMat = worldMat;
#endif
	if(bInit){
		m_saveMat	=	worldMat;//GetMeshPtr()->GetWorldMatrix();
		m_saveRot	=	m_fRotationZ;
	}
	if(this->GetMeshPtr())
		this->GetMeshPtr()->SetWorldMatrix(worldMat);
	this->ResetEnv(0);
	return true;
	}

	SceneGameSkill::SceneGameSkill(Scene3D* s)
	{
		m_fileid		=	0;
		m_scenePtr		=	s;
		m_fSpeed		=	To3DValue(10);
		m_type			=	SGS_None;
		m_sourceBoneID	=	-1;
		m_targetBoneID	=	-1;
		m_sourcePos.setxyz(0,0,0);//技能开始点
		m_targetPos.setxyz(0,0,0);
		m_readyTime		=	0;
		m_bFree			=	true;
		m_fHeightScale	=	0;
		m_fHeightScaleT	=	0;
		m_fWaitTime		=	0;
		m_fWaitFreeTime =	0.1;
		m_fContinueTime	=	0;
		m_bContinueTime =	false;
		m_fSourceScale	=	1;
	}
	SceneGameSkill::~SceneGameSkill()
	{
		m_mesh = 0;
	}
	BOOL						SceneGameSkill::Setup(unsigned short fileid)//初始化技能类
	{CPUTimeT(SceneGameSkill);
	if(fileid>=GetGameMgr()->m_filelist.size())
		return false;
	if(!g_manager.LoadMesh(m_mesh, GetGameMgr()->m_filelist[fileid].t))
	{
		LogInfo("system->error:%s(%s)", utf8("技能找不到模型"), GetGameMgr()->m_filelist[fileid].t);
		return false;
	}
	return true;
	}
	VOID						SceneGameSkill::SetTarget(SceneObject* target, const char* szBoneLike)
	{CPUTimeT(SceneGameSkill);
	m_targetBoneID = -1;
	m_targetID = target->m_gameID;
	if(szBoneLike&&szBoneLike[0] && target->GetMeshPtr())
	{
		m_targetBoneID = target->GetMeshPtr()->GetBoneIDLike(szBoneLike);
		if(m_targetBoneID<0){
			LogInfo("system->error:target %s(%s)%s(%s)",utf8("场景对象"),target->GetName(),utf8("中找不到骨骼"),szBoneLike);
		}
	}
	}
	//
	VOID						SceneGameSkill::SetSource(SceneObject* unit, const char* szBoneLike)
	{CPUTimeT(SceneGameSkill);
	m_sourceBoneID = -1;
	m_srcID = unit->m_gameID;
	m_sourcePos = unit->GetPosition();
	if(!szBoneLike)
		szBoneLike = "head";
	if(szBoneLike&&szBoneLike[0] && unit->GetMeshPtr())
	{
		m_sourceBoneID = unit->GetMeshPtr()->GetBoneIDLike(szBoneLike);
		if(m_sourceBoneID<0){
			LogInfo("system->error:source %s(%s)%s(%s)",utf8("场景对象"),unit->GetName(),utf8("中找不到骨骼"),szBoneLike);
		}
	}
	}
	VOID						SceneGameSkill::Reset()
	{CPUTimeT(SceneGameSkill);
	m_srcID			=	GameSceneID();
	m_targetID		=	GameSceneID();
	m_fileid		=	0;
	m_type			=	SGS_None;
	m_sourceBoneID	=	-1;
	m_targetBoneID	=	-1;
	m_sourcePos.setxyz(0,0,0);//技能开始点
	m_targetPos.setxyz(0,0,0);
	m_fHeightScale	=	0.7f;
	m_fHeightScaleT	=	0.7f;
	m_readyTime		=	0;
	m_fWaitTime		=	0;
	m_fWaitFreeTime =	0.1;
	m_fContinueTime	=	0;
	m_bContinueTime =	false;
	m_fSourceScale	=	1;
	}
	VOID						SceneGameSkill::GetPos(matrix4x4& mat, SceneObject* m, int boneIndex, float fHeightScale)
	{CPUTimeT(SceneGameSkill);
	Mesh* mesh = m->GetMeshPtr();
	if(mesh){
		if(mesh->GetDataPtr()->m_bUseBoneIndex){
			if(!mesh->GetBoneMatrix(boneIndex, &mat)){
			}
		}
		else{
			mat = *m->GetWorldPtr();
			mat._43 += m->GetObjectHeight()*fHeightScale;
		}
		//if(mat._43<2.0f){
		//	const AABox& box = mesh->GetObjectRegion().aabox;
		//	mat._43 += (box._max.z-box._min.z)*fHeightScale;
		//}
	}
	else
	{
		mat = *m->GetWorldPtr();
	}
	}
	VOID						SceneGameSkill::StartSkill(SceneObject* src, SceneObject* target, float fContinueTime)
	{CPUTimeT(SceneGameSkill);
	Reset();
	this->SetSource(src, 0);
	this->SetTarget(target, 0);
	this->m_fContinueTime = fContinueTime;
	m_bContinueTime = this->m_fContinueTime>0.01f;
	matrix4x4 mat;
	GetPos(mat, src, m_sourceBoneID, m_fHeightScale);
	m_sourcePos = mat.GetPosition();
	GetPos(mat, target, m_targetBoneID, m_fHeightScaleT);
	m_targetPos = mat.GetPosition();
	OnSkillStart(SceneGameSkill::SGS_ToTarget);
	}
	VOID						SceneGameSkill::StartSkill(SceneObject* src, float fContinueTime)
	{CPUTimeT(SceneGameSkill);
	Reset();
	this->SetSource(src, 0);
	this->m_fContinueTime = fContinueTime;
	m_bContinueTime = this->m_fContinueTime>0.01f;
	matrix4x4 mat;
	GetPos(mat, src, m_sourceBoneID, m_fHeightScale);
	m_sourcePos = mat.GetPosition();
	OnSkillStart(SceneGameSkill::SGS_InSource);
	}
	VOID						SceneGameSkill::StartSkill(const float3& pos, float fContinueTime)
	{CPUTimeT(SceneGameSkill);
	Reset();
	this->m_fContinueTime = fContinueTime;
	m_bContinueTime = this->m_fContinueTime>0.01f;
	m_sourcePos	=	pos;
	OnSkillStart(SceneGameSkill::SGS_InPoint);
	}
	VOID						SceneGameSkill::OnSkillStart(int type)
	{CPUTimeT(SceneGameSkill);
	m_type	=	type;
	matrix4x4 mat;
	mat.identity();
	mat.SetPosition(m_sourcePos);
	m_mesh->m_callback = this;
	if((int)(m_fSourceScale*1000)!=1000)
	{
		matrix4x4 ms;
		matrixScaling(&ms, m_fSourceScale, m_fSourceScale, m_fSourceScale);
		matrixMultiply(&mat, &ms, &mat);
	}
	m_mesh->SetWorldMatrix(mat);
	m_mesh->SetCurrentAction(0, 1);
	m_mesh->m_bGeometryVisible = true;
	m_mesh->Pause(false);
	m_bFree = false;
	}
	VOID						SceneGameSkill::OnFinish()
	{CPUTimeT(SceneGameSkill);
	//if(this->m_finishFunc.t[0])
	//{
	//	lua_Map3DSkill_s mgr;
	//	SetIPtr(&mgr.m_data, this, SceneGameSkill, IMap3DSkillPtr);
	//	lua_tinker::call<void>(g_luamanager.GetLuaState(), m_finishFunc.t, mgr);
	//	m_finishFunc.t[0] = 0;
	//}
	m_mesh->m_bGeometryVisible = false;
	m_mesh->Pause(true);
	this->m_bFree		= true;
	Reset();
	}
	VOID						SceneGameSkill::OnFrameMove()
	{CPUTimeT(SceneGameSkill);
	if(m_bFree&&m_fWaitFreeTime<0)return;
	float fElapsedTime = g_manager.GetElapsedTime();
	if(m_bFree){
		m_fWaitFreeTime -= fElapsedTime;
		if(m_fWaitFreeTime<0)
			return;
	}
	else{
		if(m_fWaitTime>0)
		{
			m_fWaitTime -= fElapsedTime;
			return;
		}
		if(m_bContinueTime){
			m_fContinueTime -= fElapsedTime;
			if(m_fContinueTime<0){
				OnFinish();
				return;
			}
		}
		switch(this->m_type){
		case SceneGameSkill::SGS_ToTarget:
			{
				SceneObject* target = m_scenePtr->SearchObject(m_targetID.sceneid);
				if(target)
				{
					Mesh* mesh = target->GetMeshPtr();
					if(mesh)
					{
						matrix4x4 mat;
						GetPos(mat, target, m_targetBoneID, m_fHeightScaleT);
						m_targetPos = mat.GetPosition();
					}
				}
				float3 dir = m_sourcePos - m_targetPos;
				float fLength = dir.R();
				float fMove = fElapsedTime * this->m_fSpeed;
				if(fLength<fMove){
					matrix4x4 mat;
					matrixRotationZ(&mat, m_fRotationZ);
					mat.SetPosition(m_sourcePos=m_targetPos);
					m_mesh->SetWorldMatrix(mat);
					OnFinish();
				}
				else
				{
					m_sourcePos = dir * (1-fMove/fLength) + m_targetPos;
					m_fRotationZ = getRotateZ(dir) + PI;
					matrix4x4 mat;
					matrixRotationZ(&mat, m_fRotationZ);
					mat.SetPosition(m_sourcePos);
					if((int)(m_fSourceScale*1000)!=1000)
					{
						matrix4x4 ms;
						matrixScaling(&ms, m_fSourceScale, m_fSourceScale, m_fSourceScale);
						matrixMultiply(&mat, &ms, &mat);
					}
					m_mesh->SetWorldMatrix(mat);
				}
			}
			break;
		case SceneGameSkill::SGS_InPoint:
			{
			}
			break;
		case SceneGameSkill::SGS_InSource:
			{
				SceneObject* obj = m_scenePtr->SearchObject(m_srcID.sceneid);
				if(obj)
				{
					Mesh* mesh = obj->GetMeshPtr();
					if(mesh)
					{
						matrix4x4 mat;
						GetPos(mat, obj, m_sourceBoneID, m_fHeightScale);
						if((int)(m_fSourceScale*1000)!=1000)
						{
							matrix4x4 ms;
							matrixScaling(&ms, m_fSourceScale, m_fSourceScale, m_fSourceScale);
							matrixMultiply(&mat, &ms, &mat);
						}
						m_mesh->SetWorldMatrix(mat);
					}
				}
			}
			break;
		}
	}
	if(m_mesh)
	{
		m_mesh->FrameMove();
	}
	}
	VOID						SceneGameSkill::OnRender()
	{CPUTimeT(SceneGameSkill);
	if(m_bFree&&m_fWaitFreeTime<0)return;
	if(m_fWaitTime>0)return;
	if(m_mesh)
		m_mesh->Render();
	}
	VOID						SceneGameSkill::ActionPlayOver(Mesh* m, int actiontype, const char* actionname)
	{CPUTimeT(SceneGameSkill);
	switch(m_type){
	case SceneGameSkill::SGS_ToTarget:
		{
		}
		break;
	case SceneGameSkill::SGS_InPoint:
		{
			this->OnFinish();
		}
		break;
	case SceneGameSkill::SGS_InSource:
		{
			if(this->m_bContinueTime){//如果有持续时间才能被持续播放
				break;
			}
			this->OnFinish();
		}
		break;
	}
	}
	VOID						SceneGameSkill::ActionAttackBy(Mesh* m, int actionID, int playtype)
	{CPUTimeT(SceneGameSkill);
	}

};
//
//
//
