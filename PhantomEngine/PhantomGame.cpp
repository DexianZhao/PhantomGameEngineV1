#include "PhantomGame.h"
#include "PhantomManager.h"
#include "APILua.h"
#include "LuaManager.h"
#include "PhantomScene3DUnits.h"

namespace Phantom{
	GamePower::GamePower(){
		m_powerID = m_enemyID = 0;
		m_data = 0;
		m_enemeyPower=0;
	}
	GameMgr::GameMgr(){
		m_scenePtr	=	0;
		m_id		=	1;
		m_bCreated	=	false;
		m_bInitLua	=	false;
		m_sceneLine	=	0;
		m_leader	=	0;
		m_leaderTable	=	0;
		m_leaderLine	=	0;
		m_reloadCount	=	0;
		m_plotid		=	0;
		m_bPlotEnable	=	false;
		m_bPlotInit		=	false;
		m_cameraFollow	=	false;
		m_loadCount		=	0;
	}
	GameMgr::~GameMgr(){
		DestroyScene();
		for(int i=0;i<m_allocs.size();i++)
			delete m_allocs[i];
	}
	VOID			GameMgr::GotoPlot(int plotid)//-1下一个，-2结束
	{CPUTimeNone();
		if(!m_bPlotEnable)return;
		GameSceneID temp;
		if(plotid==-2){
			send(GameMessage::ToGame_PlotFinish, temp, 0, 0);
			return;
		}
		if(plotid==-1)
			m_plotid++;
		else
			m_plotid = plotid;
		send(GameMessage::ToGame_PlotNext, temp, &m_plotid, sizeof(short));
	}
	GameMessage*	GameMgr::alloc()
	{
		int oldSize = m_frees.size();
		if(oldSize>0){
			GameMessage* msg = m_frees[oldSize-1];
			m_frees.SetArrayCount(oldSize-1);
			return msg;
		}
		GameMessage* msg = new GameMessage();
		m_allocs.push_back(msg);
		return msg;
	}
	void	GameMgr::recycle(GameMessage* msg)//回收数据块
	{
		if(msg->ptr)
			delete[] msg->ptr;
		memset(msg, 0, sizeof(GameMessage));
		//for(int i=0;i<m_frees.size();i++)
		//{
		//	assert(m_frees[i]!=msg);
		//}
		m_frees.push_back(msg);
	}
	VOID	GameMgr::SetPlotEnable(BOOL b)
	{CPUTimeNone();
		m_bPlotEnable = b;
		if(m_bPlotEnable){
			if(!m_bPlotInit){
				m_bPlotInit = true;
				m_plotid = 1;
				//GameSceneID temp;
				//send(GameMessage::ToGame_PlotNext, temp, &m_plotid, sizeof(short));
			}
		}
	}
	VOID	GameMgr::AddLoadTexture(Texture* tex)
	{
		if(!tex)return;
		safe_addref(tex);
		m_loadlist.push_back(tex);
	}
	void	GameMgr::OnLoadFrameMove(){
		if(m_loadlist.size()==0)return;
		Texture* tex = m_loadlist[0];
		m_loadlist.eraseByIndex(0);
		tex->LoadTextureData(false);
		tex->OnLoadDevice();
		safe_release(tex);
		//
		if(m_funcLoading.t[0]){
			lua_Map3D_s mgr;
			SetIPtr(&mgr.m_data, m_scenePtr, Scene3D, IMap3DPtr);
			lua_tinker::call<void>(g_luamanager.GetLuaState(), m_funcLoading.t, mgr, (float)(m_loadCount-m_loadlist.size())/(float)m_loadCount, m_loadlist.size()==0);
		}
	}
	int		GameMgr::AddFileIndex(const FileName& f)
	{CPUTimeNone();
		for(int i=0;i<m_filelist.size();i++)
			if(m_filelist[i]==f.t)
				return i;
		int ret = m_filelist.size();
		m_filelist.push_back(f);
		return ret;
	}
	BOOL	GameMgr::CreateScene(int sceneID, LPCSTR szLuaFunc)//创建游戏系统
	{CPUTimeT(GameMgr);
		if(!szLuaFunc)return 0; 
		DestroyScene();
		m_bCreated	=	false;
		if(!ResetAllDatas(sceneID))
			return false;
		if(szLuaFunc)
			m_funcLoading = szLuaFunc;
		else
			m_funcLoading.t[0] = 0;
#ifndef EDITOR_OS
		if(sceneID<=0){
			m_bCreated = true;
			//if(m_funcLoading.t[0]){
			//	lua_Map3D_s mgr;
			//	SetIPtr(&mgr.m_data, m_scenePtr, Scene3D, IMap3DPtr);
			//	lua_tinker::call<void>(g_luamanager.GetLuaState(), m_funcLoading.t, scene, 1.0f, true);
			//}
			return true;
		}
		if(szLuaFunc[0])
			g_manager.m_bUseThread = true;
		m_bCreated = LoadScene(sceneID);
		if(szLuaFunc[0])
			g_manager.m_bUseThread = false;
		if(!m_bCreated)
		{
			for(int i=0;i<m_loadlist.size();i++){
				safe_release(m_loadlist[i]);
			}
			m_loadlist.clear();
			return false;
		}
		//使用单线程载入并读取进度
		this->m_loadCount = m_loadlist.size();
		RebuildInScenes();
		if(this->m_tblSkill){//文件名字转换成文件索引
			for(int c=0;c<m_tblSkill->m_numCol;c++){
				if(strstr(m_tblSkill->m_cols[c].name.t, "mesh")&&m_tblSkill->m_cols[c].type==ConfigHeader::type_text)//如果存在mesh类
				{
					m_tblSkill->m_cols[c].type = ConfigHeader::type_int;
					for(int l=0;l<m_tblSkill->m_numLine;l++){
						ConfigLine* line = m_tblSkill->GetLine(l);
						line->m_data[c].ival = -1;
						if(line->m_data[c].getText()){
							FileName f = line->m_data[c].getText();
							f.setIsFile();
							line->m_data[c].freeText();
							line->m_data[c].ival = AddFileIndex(f);
						}
					}
				}
			}
		}
		if(m_sceneLine&&m_sceneLine->GetScriptPtr()){
			ScriptBase* sc = m_sceneLine->GetScriptPtr();
			if(sc->IsExist(SceneEventID_Open)){
				lua_Map3D_s mgr;
				SetIPtr(&mgr.m_data, this->m_scenePtr, Scene3D, IMap3DPtr);
				char name[128];
				sc->GetEventName(name, 0, SceneEventID_Open, true);
				lua_tinker::call<void>(g_luamanager.GetLuaState(), name, mgr);
			}
		}
		return true;
#else
		return true;
#endif
	}
#define	EVENT_BORN_ID	1//出生点ID
#define	LEADER_ROLE_ID	1//主角roleID
	BOOL	GameMgr::CreateLeader()//创建主角数据
	{CPUTimeT(GameMgr);
		if(!m_leaderLine||!m_scenePtr||!m_tblRoles)return false;
		ConfigLine* line = this->m_tblRoles->GetLineFromID(LEADER_ROLE_ID);//创建主角,ID是1
		if(!line){
			return false;
		}
		int meshCol = m_tblRoles->SearchCol("mesh");
		if(meshCol<0){
			LogInfo("system->error:%s", utf8("找不到主模型[mesh]列文件在lead_table.txt"));
			return false;
		}
		SceneObject* obj = this->m_scenePtr->InsertObject(line->GetTextVal(meshCol), EVENT_BORN_ID);//1是出生点,固定
		if(!obj)
		{
			ScenePoint* point = this->m_scenePtr->GetActiveEnv()->GetPointByEventID(EVENT_BORN_ID);
			if(!point){
				LogInfo("system->error:%s", utf8("创建不了主模型，场景内找不到出生点"));
			}
			else{
				LogInfo("system->error:%s", utf8("载入不了主模型[mesh]列文件在lead_table.txt"));
			}
			return false;
		}
		obj->SetEnabled(false);
		obj->m_bShowBlood = true;
		this->m_leader = new GameLeader(this, line);
		m_leader->Setup(obj);
		m_objects.push_back(m_leader);
		AddToPower(m_leader);
		obj->ResetSaveEnv();
		return true;
	}
	VOID	GameMgr::OnCheckEnemyIn()
	{CPUTimeT(GameMgr);
		for(int p=0;p<m_powerList.size();p++){
			GamePower* power = m_powerList[p];
			GamePower* enemyPower = power->m_enemeyPower;
			if(!enemyPower)//如果没有敌对势力则
				continue;
			if(power->m_enemyID<power->m_powerID){
				if(enemyPower->m_enemyID==power->m_powerID){
					continue;
				}
			}
			for(int m=0;m<power->m_objects.size();m++){//正方势力
				GameObject* src = power->m_objects[m];
				if(src->m_bIsDead || src->getTarget()||!src->m_visible)//如果已经有目标了则不再进行计算
					continue;
				for(int e=0;e<enemyPower->m_objects.size();e++){//敌对势力
					GameObject* enemy = enemyPower->m_objects[e];
					if(enemy->m_bIsDead||!enemy->m_visible)
						continue;
					float3 dest = enemy->GetPosition() - src->GetPosition();
					float length = dest.length();
					if(length<src->m_atkrange){//如果敌人进入可攻击范围
						src->SetEnemyIn(enemy);
						break;
					}
				}
			}
		}
	}
	VOID	GameMgr::AddToPower(GameObject* obj)
	{CPUTimeNone();
		for(int i=0;i<m_powerList.size();i++){
			if(m_powerList[i]->m_powerID==obj->m_powerID)
			{
				m_powerList[i]->m_objects.push_back(obj);
				break;
			}
		}
	}
	VOID	GameMgr::RemoveFromPower(GameObject* obj)
	{CPUTimeNone();
		for(int i=0;i<m_powerList.size();i++){
			if(m_powerList[i]->m_powerID==obj->m_powerID)
			{
				for(int j=0;j<m_powerList[i]->m_objects.size();j++){
					if(m_powerList[i]->m_objects[j]==obj){
						m_powerList[i]->m_objects.eraseByIndex(j);
						break;
					}
				}
				break;
			}
		}
	}
	VOID	GameMgr::DestroyScene()
	{CPUTimeT(GameMgr);
		m_bPlotInit	=	false;
		this->m_bPlotEnable = false;
		this->m_plotid = 0;
		m_sceneLine	=	0;
		m_leader	=	0;//删除主角控制器
		safe_delete(m_leaderTable);
		for(int i=0;i<m_loadlist.size();i++)
		{
			safe_release(m_loadlist[i]);
		}
		m_loadlist.clear();
		m_leaderLine	=	0;
		this->m_frees	=	this->m_allocs;
		m_recvs.SetArrayCount(0);
		m_tempRecvs.SetArrayCount(0);
		safe_delete_dyn(m_objects);
		safe_delete_dyn(m_powerList);
		safe_delete_dyn(m_scripts);
		this->m_inobjects.clear();
		m_scenePtr		=	0;
		this->m_bInitLua = false;
		Destroy();
	}
	VOID	GameMgr::send(GameMessage::MID messageID, const GameSceneID& id, const void* buff, unsigned short length)
	{CPUTimeNone();
		GameMessage* msg = alloc();
		msg->mid = messageID;
		msg->uid = id;
		msg->length = length;
		if(length>MessageBufferLength){
			msg->ptr = new unsigned char[length];
			memcpy(msg->ptr, buff,length);
		}
		else{
			memcpy(msg->data, buff, length);
		}
		m_recvs.push_back(msg);
	}
	GameMgr*		GetGameMgr(){
		static GameMgr mgr;
		return &mgr;
	}
	GameObject*		GameMgr::Search(GameID& id, BOOL* bReset)
	{CPUTimeNone();
		if(bReset)
			*bReset = false;
		if(id.id==0)return 0;
		if(id.index<this->m_objects.size()&&m_objects[id.index]->m_id.gameid.id==id.id)
			return m_objects[id.index];
		CPUTimeT(GameMgr);
		for(int i=0;i<m_objects.size();i++){
			if(m_objects[i]->m_id.gameid.id==id.id){
				id.id = m_objects[i]->m_id.gameid.id;
				id.index = i;
				return m_objects[i];
			}
		}
		if(bReset)
			*bReset = true;
		id.id=0;
		return 0;
	}
	VOID			GameMgr::DeleteObject(GameObject* obj){CPUTimeT(GameMgr);
		for(int i=0;i<m_objects.size();i++){
			if(m_objects[i]==obj){
				RemoveFromPower(obj);
				delete obj;
				m_objects.eraseByIndex(i);
				break;
			}
		}
		for(int i=0;i<m_objects.size();i++){
			m_objects[i]->m_id.gameid.index=i;
		}
		RebuildInScenes();
	}
	GameObject*		GameMgr::CreateObject(int lineid)//创建角色，从roles表格中的行id
	{CPUTimeT(GameMgr);
		if(!m_tblRoles)return 0;
		ConfigLine* line = m_tblRoles->GetLineFromID(lineid);
		if(!line)return 0;
		GameObject* obj = new GameObject(this, line);
		obj->m_id.gameid.index = m_objects.size();
		m_objects.push_back(obj);
		AddToPower(obj);
		return obj;
	}
	VOID			GameMgr::RebuildInScenes()//重新构造在场对象列表
	{CPUTimeT(GameMgr);
		m_inobjects.SetArrayCount(m_objects.size());
		int count = 0;
		for(int i=0;i<this->m_objects.size();i++){
			GameObject*obj = m_objects[i];
			if(obj->IsLeaderObject()||obj->m_bIsDead||!obj->m_visible)
				continue;
			m_inobjects[count++] = obj;
		}
		m_inobjects.SetArrayCount(count);
	}
	VOID			GameMgr::ResetAllObjects()
	{CPUTimeT(GameMgr);
		if(!m_scenePtr)return;
		for(int i=0;i<this->m_objects.size();i++){
			GameObject* obj = m_objects[i];
			SceneObject* sobj = m_scenePtr->SearchObject(obj->m_id.sceneid);
			if(!sobj)continue;
			obj->ResetAll();
			sobj->ResetAll(obj->m_bornPos, obj->m_bornRot);
		}
		RebuildInScenes();
	}

	VOID			GameMgr::Reborn(GameObject* obj, float fRebornTime)//重新复活等待
	{CPUTimeT(GameMgr);
		obj->setReborn(fRebornTime);
		this->send(GameMessage::ToScene_ResetID, obj->m_id, 0, 0);
		RebuildInScenes();
	}
	VOID			GameMgr::OnFrameMove(double dElapsedTime)//计算所有游戏对象
    {CPUTimeT(GameMgr);
		OnLoadFrameMove();
		if(!this->m_bCreated)
			return;
		//do game recvs
		m_tempRecvs = m_recvs;
		m_recvs.SetArrayCount(0);
		for(int i=0;i<m_tempRecvs.size();i++)
		{
			onMessage(m_tempRecvs[i]);
			recycle(m_tempRecvs[i]);
		}
		//
		if(this->m_leader)
			m_leader->frameMove(dElapsedTime);
		for(int i=0;i<m_objects.size();i++){//
			if(m_objects[i]!=m_leader)
			{
				if(!m_objects[i]->m_visible)
					continue;
				m_objects[i]->frameMove(dElapsedTime);
			}
		}
		//do scene recvs
		m_tempRecvs = m_recvs;
		m_recvs.SetArrayCount(0);
		for(int i=0;i<m_tempRecvs.size();i++)
		{
			onMessage(m_tempRecvs[i]);
			recycle(m_tempRecvs[i]);
		}
		m_tempRecvs.SetArrayCount(0);
		OnCheckEnemyIn();
		//
		if(m_cameraFollow){
			GameObject* cameraFollow = Search(m_cameraTarget);
			if(cameraFollow){
				float3 vEye = GetCameraPtr()->GetEye();
				float3 vTarget = GetCameraPtr()->GetTarget();
				float3 vDir = vEye - vTarget;
				vTarget = cameraFollow->GetPosition();
				vEye = vTarget + vDir;
				GetCameraPtr()->SetViewMatrix(&vEye, &vTarget);
			}
		}
	}
	void			GameMgr::onMessage(GameMessage* msg)//处理游戏消息接收
    {CPUTimeT(GameMgr);
		if(msg->mid<GameMessage::ToScene_Max){
			if(m_scenePtr)
				m_scenePtr->OnMessage(msg);
			return;
		}
		switch(msg->mid){
		case GameMessage::ToGame_InitScene:
			{
				if(m_sceneLine&&m_sceneLine->GetScriptPtr()){
					ScriptBase* sc = m_sceneLine->GetScriptPtr();
					if(sc->IsExist(SceneEventID_Init)){
						lua_Map3D_s mgr;
						SetIPtr(&mgr.m_data, this->m_scenePtr, Scene3D, IMap3DPtr);
						char name[128];
						sc->GetEventName(name, 0, SceneEventID_Init, true);
						lua_tinker::call<void>(g_luamanager.GetLuaState(), name, mgr);
					}
				}
			}
			break;
		case GameMessage::ToGame_PlotNext:
			{
				if(!m_sceneLine)break;
				int pid = *(short*)msg->GetDataPtr();
				ScriptBase* sc = m_sceneLine->GetScriptPtr();
				if(sc->IsExist(SceneEventID_Next)){
					char name[128];
					sc->GetEventName(name, 0, SceneEventID_Next, true);
					lua_Map3D_s mgr;
					SetIPtr(&mgr.m_data, this->m_scenePtr, Scene3D, IMap3DPtr);
					SetCurrentLuaFileIndex(sc->m_fileIndex);
					lua_tinker::call<void>(g_luamanager.GetLuaState(), name, mgr, pid);
					SetCurrentLuaFileIndex(-1);
				}
			}
			break;
		case GameMessage::ToGame_PlotFinish:
			{
				if(!m_sceneLine)break;
				ScriptBase* sc = m_sceneLine->GetScriptPtr();
				if(sc->IsExist(SceneEventID_Finish)){
					char name[128];
					sc->GetEventName(name, 0, SceneEventID_Finish, true);
					lua_Map3D_s mgr;
					SetIPtr(&mgr.m_data, this->m_scenePtr, Scene3D, IMap3DPtr);
					SetCurrentLuaFileIndex(sc->m_fileIndex);
					lua_tinker::call<void>(g_luamanager.GetLuaState(), name, mgr);
					SetCurrentLuaFileIndex(-1);
					this->m_bPlotEnable=false;
				}
			}
			break;
		case GameMessage::ToGame_Born:
			{
				GameBornData* data = (GameBornData*)msg->GetDataPtr();
				GameObject* obj = this->CreateObject(data->lineID);
				if(!obj){
					LogInfo("system->error:(id=%d)%s", data->lineID, utf8("不能初始化GameObject对象,在roles.txt中找不到对应的行ID"));
					break;
				}
				obj->Setup(*data);
				SceneObject* sobj = this->m_scenePtr->SearchObject(msg->uid.sceneid);
				if(sobj){
					sobj->m_gameID.gameid = obj->m_id.gameid;
				}
			}
			break;
		case GameMessage::ToGame_ResetID:
			{
				GameObject* obj = this->Search(msg->uid.gameid);
				if(obj)
					obj->m_id.sceneid = msg->uid.sceneid;
			}
			break;
		case GameMessage::ToGame_ClickObject:
			{
				if(m_sceneLine&&m_sceneLine->GetScriptPtr()){
					ScriptBase* sc = m_sceneLine->GetScriptPtr();
					if(sc->IsExist(SceneEventID_Click)){
						GameSceneID* data = (GameSceneID*)msg->GetDataPtr();
						lua_Map3D_s mgr;
						SetIPtr(&mgr.m_data, this->m_scenePtr, Scene3D, IMap3DPtr);
						lua_GameObject_s obj;
						obj.m_data = data->gameid;
						char name[128];
						sc->GetEventName(name, 0, SceneEventID_Click, true);
						lua_tinker::call<void>(g_luamanager.GetLuaState(), name, mgr, obj);
					}
				}
			}
			break;
		case GameMessage::ToGame_ClickTerrain:
			{
				if(m_sceneLine&&m_sceneLine->GetScriptPtr()){
					ScriptBase* sc = m_sceneLine->GetScriptPtr();
					if(sc->IsExist(SceneEventID_ClickTerrain)){
						float3* data = (float3*)msg->GetDataPtr();
						lua_Map3D_s mgr;
						SetIPtr(&mgr.m_data, this->m_scenePtr, Scene3D, IMap3DPtr);
						lua_float3 pos;
						pos.setup(data->x, data->y, data->z);
						char name[128];
						sc->GetEventName(name, 0, SceneEventID_ClickTerrain, true);
						lua_tinker::call<void>(g_luamanager.GetLuaState(), name, mgr, pos);
					}
				}
			}
			break;
		case GameMessage::ToGame_AttackOver:
			{
				GameObject* obj = this->Search(msg->uid.gameid);
				if(obj){
					obj->onAttackOver();
				}
			}
			break;
		case GameMessage::ToGame_AttackHit:
			{
				GameObject* obj = this->Search(msg->uid.gameid);
				if(obj){
					obj->onAttackHit();//造成伤害裁决,按照技能方法进行不同的裁决
				}
			}
			break;
		case GameMessage::ToGame_DieOver:
			{
				GameObject* obj = this->Search(msg->uid.gameid);
				if(obj){
					obj->onDieOver();
				}
			}
			break;
		}
	}
	BOOL			GameMgr::LoadScene(int nLineID)//从表格进入场景
	{CPUTimeT(GameMgr);
		if(!m_tblScene)return false;
		ConfigLine* line = this->m_tblScene->GetLineFromID(nLineID);
		if(!line){
			LogInfo("system->error:(%d)%s", nLineID, utf8("在地区表中找不到这个行id"));
			return false;
		}
		Scene3DPtr s;
		int col = line->SearchCol("file");
		if(col<0){
			LogInfo("system->error:%s", utf8("在地区表中找不到file列"));
			return false;
		}
		FileName file = line->GetTextVal(col);
		FileName ext;
		file.splitpath(0, 0, &ext);
		if(!(ext==".rpgscene")){
			LogInfo("system->error:(%s)%s", line->GetTextVal(col), utf8("地形不是rpgscene文件"));
			return false;
		}
		if(!g_manager.LoadScene3D(s, file.t))
		{
			LogInfo("system->error:(%s)%s", line->GetTextVal(col), utf8("找不到地形文件"));
			return false;
		}
		m_sceneLine	=	line;
		this->m_scenePtr = s;
		if(!CreateLeader())
			return false;
		m_scenePtr->BindAllObjects();//载入成功之后绑定所有角色
		m_scenePtr->m_bShowBlood = true;
		GameSceneID temp;
		send(GameMessage::ToGame_InitScene, temp, 0, 0);
		return true;
	}
	VOID			ReplaceFunction(CDynamicArray<char>& ret, const char* szText, const char* szFinds, const char* szReplace, CDynamicArray<Name>& funcs){
		int nLen = strlen(szText);
		int nFind = strlen(szFinds);
		int nReplace = strlen(szReplace);
		const char* szFunction = "function";
		int nFunction = strlen(szFunction);
		const char* first = strstr(szText, szFunction);
		const char* last = szText;
		while(first){
			first += nFunction;
			for(int i=0;i<10;i++){
				if(first[i]=='\t'||first[i]==' ')
					first++;
				else if(first[i]==0)
					break;
				else
					break;
			}
			if(strnicmp(first, szFinds, nFind)==0){
				for(int k=0;k<64;k++){
					if(first[k]==' '||first[k]=='\t'||first[k]=='(')
					{
						Name n;
						strncpy(n.t, first, k);
						n.t[k] = 0;
						funcs.push_back(n);
						break;
					}
				}
				int len = first - last;
				int oldLen = ret.size();
				ret.SetArrayCount(ret.size()+len+nReplace, 0);
				memcpy(ret.GetPtr()+oldLen, last, len);
				memcpy(ret.GetPtr()+oldLen+len, szReplace, nReplace);
				first = first + nFind;
				last = first;
				first = strstr(last, szFunction);
			}
			else
				first = strstr(first, szFunction);
		}
		first = szText + nLen;
		int len = first - last;
		int oldLen = ret.size();
		ret.SetArrayCount(ret.size()+len, 0);
		memcpy(ret.GetPtr()+oldLen, last, len);
	}
	BOOL			GameMgr::ResetAllDatas(int sceneID)//载入所有脚本
	{CPUTimeT(GameMgr);
		m_reloadCount++;
		if(!ResearchAllFiles())//如果没有初始化表格则返回，游戏不能继续
			return false;
		assert(m_leaderTable==0);
		if(m_tblLeader&&m_tblLeader->GetLineCount()>0){
			int itype = m_tblLeader->SearchCol("type");
			int ival = m_tblLeader->SearchCol("val");
			int idesc = m_tblLeader->SearchCol("desc");
			assert(itype>=0&&ival>=0&&idesc>=0);
			if(itype>=0&&ival>=0&&idesc>=0){
				m_leaderTable = new Config();
				m_leaderTable->m_numCol = m_tblLeader->GetLineCount();
				const int numLine = 1;//主角只有自己的一个行
				m_leaderTable->m_numLine = numLine;
				m_leaderTable->m_tableName = "LeaderData";
				m_leaderTable->m_cols = new ConfigHeader[m_leaderTable->m_numCol];
				m_leaderTable->m_lines = new ConfigLine[numLine];
				m_leaderLine = &m_leaderTable->m_lines[0];
				m_leaderLine->m_config = m_leaderTable;
				m_leaderLine->m_data = new ConfigVal[m_leaderTable->m_numCol];
				for(int i=0;i<m_tblLeader->GetLineCount();i++){
					ConfigLine* tempLine = m_tblLeader->GetLine(i);
					ConfigHeader& col = m_leaderTable->m_cols[i];
					col.SetName(tempLine->GetLineName());
					col.desc	= tempLine->GetTextVal(idesc);
					col.type_s	= tempLine->GetTextVal(itype);
					col.ResetType();
					ConfigVal& val = m_leaderLine->m_data[i];
					const char* szLine = tempLine->GetTextVal(ival);
					switch(col.type){
					case ConfigHeader::type_float:
						val.fval = atof(szLine);
						break;
					case ConfigHeader::type_int:
						val.ival = atoi(szLine);
						break;
					case ConfigHeader::type_boolean:
						if(stricmp(szLine, "true") == 0)
							val.ival = 1;
						else if(stricmp(szLine, "false") == 0)
							val.ival = 0;
						else
							val.ival = atoi(szLine);
						break;
					case ConfigHeader::type_class:
						{
							for(int m=0;m<m_configs.size();m++){
								if(col.type_s == m_configs[m]->m_tableName)
								{
									col.classPtr = m_configs[m];
									break;
								}
							}
							if(!col.classPtr)
							{
								LogInfo("system->error: can't find class(%s) of column in table(lead_table.txt)", col.type_s.t);
								col.type = ConfigHeader::type_int;
								val.ival = atoi(szLine);
							}
							else
							{
								val.ival = -1;
								if(szLine[0]){
									BOOL bFind = false;
									for(int k=0;k<col.classPtr->m_numLine;k++){
										if(stricmp(col.classPtr->GetLine(k)->GetLineName(), szLine)==0)//找到了
										{
											bFind = true;
											val.ival = k;//col.classPtr->GetLine(k)->GetID();
											break;
										}
									}
									if(!bFind){
										LogInfo("system->error: can't find name(%s) at class(%s) of column in table(lead_table)", szLine, col.type_s.t);
									}
								}
							}
						}
						break;
					default:
						{
							val.setText(szLine);
						}
						break;
					}
					if(col.listcount>1){
						val.list = new ConfigVal[col.listcount-1];
						for(int i=0;i<(col.listcount-1);i++){
							val.list[i].ival = val.ival;
							val.list[i].fval = val.fval;
							if(val.text)
								val.list[i].setText(val.text);
						}
					}
				}
				m_leaderTable->RebuildListIndexs();
			}
			else{
				LogInfo("system->error:%s", utf8("不能初始化主角数据，不存在type,val,desc中的任意一个"));
			}
		}
		Config* cfgPower = this->SearchConfig("power_table");
		if(cfgPower){
			int enemyID = cfgPower->SearchCol("enemyid");
			for(int i=0;i<cfgPower->GetLineCount();i++){
				ConfigLine* line = cfgPower->GetLine(i);
				GamePower* power = new GamePower();
				m_powerList.push_back(power);
				power->m_data = line;
				power->m_powerID = line->GetLineID();
				if(enemyID>=0){
					ConfigLine* lineEnemy = line->GetOtherLinePtr(enemyID);
					if(lineEnemy){
						power->m_enemyID = lineEnemy->GetLineID();
					}
				}
			}
			for(int i=0;i<m_powerList.size();i++){
				GamePower* src = m_powerList[i];
				for(int j=0;j<m_powerList.size();j++){
					if(src->m_enemyID == m_powerList[j]->m_powerID){
						src->m_enemeyPower = m_powerList[j];
						break;
					}
				}
			}
		}
		if(!this->m_bInitLua){
			this->m_bInitLua = true;
			//注册所有lua类和运行所有lua脚本
			RegisterAllLuaClass();
			AddScripts(ScriptBase::eScript_AI, m_scripts, "ai_table","onai_");
			AddScripts(ScriptBase::eScript_Scene, m_scripts, "scene_table", "onscene_", sceneID);
			AddScripts(ScriptBase::eScript_Skill, m_scripts, "skill_table", "onskill_");
			//AddScripts(ScriptBase::eScript_Plot, m_scripts, "plot_table", "onplot_", plotID);
		}
		return true;
	}
	bool	GameMgr::AddScripts(ScriptBase::eScript type, CDynamicArray<ScriptBase*>& sc, const char* config, const char* func, int loadID)
	{CPUTimeT(GameMgr);
		Config* cfg = this->SearchConfig(config);
		if(!cfg)
		{
			LogInfo("system->error:找不到脚本表格%s", config);
			return false;
		}
		int file = cfg->SearchCol("script");
		int namecol = cfg->SearchCol("name");
		if(namecol<0||file<0)
		{
			LogInfo("system->error:找不到脚本表格%s中的script和name项", config);
			return false;
		}
		CDynamicArray<ScriptBase*> scload;
		int numAI = cfg->GetLineCount();
		for(int i=0;i<numAI;i++){
			FileName f = cfg->GetLine(i)->GetTextVal(file);
			f.setIsFile();
			int index = -1;
			for(int j=0;j<sc.size();j++){
				if(sc[j]->m_file == f){
					index = j;
					break;
				}
			}
			if(index < 0)
			{
				if(loadID<0||loadID == cfg->GetLine(i)->GetLineID()){
					ScriptBase* ai = ScriptBase::CreateNew(type);
					FileName name;
					f.splitpath(0, &name, 0);
					ai->m_name = name.t;
					ai->m_file = f;
					index = sc.size();
					sc.push_back(ai);
					scload.push_back(ai);
				}
			}
			if(index >= 0){
				cfg->GetLine(i)->m_script = sc[index];
			}
		}
		for(int i=0;i<scload.size();i++){
			ScriptBase* ai = scload[i];
			FileName f = ai->m_file;
			CDynamicArray<char> scripts;
			if(!LoadUtf8(f.t, scripts))
			{
				LogInfo("system->error:(%s)%s", f.t, utf8("AI表格中这个文件不存在或无法读取"));
				return false;
			}
			CDynamicArray<char> scriptRep;
			CDynamicArray<char>* pscript = &scripts;
			char buf[128];
			const char* colname = ai->m_name.t;
			sprintf(buf, "%s_%s", colname, func);
			CDynamicArray<Name> funcs;
			ReplaceFunction(scriptRep, scripts.GetPtr(), func, buf, funcs);
			if(loadID<0){
				pscript = &scriptRep;
			}
			else{
				pscript = &scripts;
			}
			if(pscript->size()>0&&!g_luamanager.loadAndExecute(pscript->GetPtr(), f.t))
			{
				LogInfo("system->error:(%s)%s", f.t, utf8("AI表格中这个文件不存在或无法读取"));
				return false;
			}
			ai->m_fileIndex = g_luamanager.GetFileIndex(f.t);
			for(int j=0;j<ai->GetEventCount();j++){
				char func[256];
				ai->GetEventName(func, 0, j, 0);
				for(int k=0;k<funcs.size();k++){
					if(funcs[k]==func){
						ai->m_func[j] = true;
						break;
					}
				}
			}
		}
		return true;
	}
	/*API*/BOOL			WINAPI	GameMgr_create/*游戏_创建游戏系统数据*/(int sceneID, LPCSTR szLuaFunc)
	{
		return GetGameMgr()->CreateScene(sceneID, szLuaFunc);
	}
	/*API*/VOID			WINAPI	GameMgr_scene/*游戏_返回场景*/(IMap3DPtr* _return/*返回*/)
	{
		Scene3D* s = GetGameMgr()->GetScenePtr();
		if(!s)return;
		SetIPtr(_return, s, Scene3D, IMap3DPtr);
	}
	/*API*/INT			WINAPI	GameMgr_objectcount/*游戏_返回对象数量*/()
	{
		return GetGameMgr()->m_inobjects.size();
	}
	/*API*/VOID			WINAPI	GameMgr_objectat/*游戏_返回对象*/(int index, IGameObjectPtr* _return/*返回*/)
	{
		if(index<0||index>=GetGameMgr()->m_inobjects.size())
			return;
		GameObject* obj = GetGameMgr()->m_inobjects[index];
		*_return = obj->GetID().gameid;
	}
	/*API*/VOID			WINAPI	GameMgr_searchby_sceneid/*游戏_返回场景对象ID搜索游戏对象*/(INT sceneID/*ID*/, IGameObjectPtr* _return/*返回*/)
	{
		if(!GetGameMgr()->GetScenePtr())return;
		int count = GetGameMgr()->m_objects.size();
		for(int i=0;i<count;i++){
			GameObject* obj = GetGameMgr()->m_objects[i];
			SceneObject* sobj = GetGameMgr()->GetScenePtr()->SearchObject(obj->GetID().sceneid);
			if(sobj&&sobj->m_nContentID==sceneID)
			{
				*_return = obj->GetID().gameid;
				break;
			}
		}
	}
	/*API*/VOID			WINAPI	GameMgr_searchby/*游戏_返回对象*/(LPCSTR szName/*名字*/, IGameObjectPtr* _return/*返回*/)
	{
		if(!szName||!szName[0])return;
		if(!GetGameMgr()->GetScenePtr())return;
		int count = GetGameMgr()->m_objects.size();
		for(int i=0;i<count;i++){
			GameObject* obj = GetGameMgr()->m_objects[i];
			SceneObject* sobj = GetGameMgr()->GetScenePtr()->SearchObject(obj->GetID().sceneid);
			if(sobj&&stricmp(sobj->GetName(),szName)==0)
			{
				*_return = obj->GetID().gameid;
				break;
			}
		}
	}
	/*API*/VOID			WINAPI	GameMgr_search/*游戏_返回对象根据场景对象ID*/(INT nID/*ID*/, IGameObjectPtr* _return/*返回*/)
	{
		if(!GetGameMgr()->GetScenePtr())return;
		SceneObject* obj = GetGameMgr()->GetScenePtr()->GetObjectByID(nID);
		if(!obj)return;
		GameObject* gameobj = GetGameMgr()->Search(obj->m_gameID.gameid);
		if(gameobj)
			*_return = gameobj->GetID().gameid;
	}
	/*API*/VOID			WINAPI	GameMgr_plotnext/*游戏_进入下一个剧情脚本*/()
	{
		GetGameMgr()->GotoPlot(-1);
	}
	/*API*/VOID			WINAPI	GameMgr_plotto/*游戏_进入下一个剧情脚本*/(int id/*剧情id*/)
	{
		GetGameMgr()->GotoPlot(id);
	}
	/*API*/VOID			WINAPI	GameMgr_plotfinish/*游戏_结束剧情系统*/()
	{
		GetGameMgr()->GotoPlot(-2);
	}
	/*API*/LPCSTR		WINAPI	GameMgr_loadtextby/*游戏_返回文本表中的文本*/(LPCSTR szName/*名字*/, LPCSTR szCol/*列*/)
	{
		if(!GetGameMgr()->m_tblText)
			return "";
		int col = GetGameMgr()->m_colTextDesc;
		if(szCol&&szCol[0])
			col = GetGameMgr()->m_tblText->SearchCol(szCol);
		if(col<0)
			col = GetGameMgr()->m_colTextDesc;
		int count = GetGameMgr()->m_tblText->GetLineCount();
		for(int i=0;i<count;i++){
			if(stricmp(GetGameMgr()->m_tblText->GetLine(i)->GetLineName(), szName)==0)
				return GetGameMgr()->m_tblText->GetLine(i)->GetTextVal(col);
		}
		return "";
	}
	/*API*/LPCSTR		WINAPI	GameMgr_loadtext/*游戏_返回文本表中的文本根据ID*/(INT nID/*ID*/, LPCSTR szCol/*列*/)
	{
		if(!GetGameMgr()->m_tblText)
			return "";
		int col = GetGameMgr()->m_colTextDesc;
		if(szCol&&szCol[0])
			col = GetGameMgr()->m_tblText->SearchCol(szCol);
		if(col<0)
			col = GetGameMgr()->m_colTextDesc;
		ConfigLine* line = GetGameMgr()->m_tblText->GetLineFromID(nID);
		if(!line)return "";
		return line->GetTextVal(col);
	}

	/*API*/VOID			WINAPI	GameMgr_reborn/*游戏_重新刷怪*/(IGameObjectPtr* obj/*对象*/, float fRebornTime/*重新复活等待时间*/)
	{
		GetGameMgr()->Reborn(GetGameMgr()->Search(*obj), fRebornTime);
	}
	/*API*/VOID			WINAPI	GameMgr_leader/*游戏_返回主角对象*/(IGameObjectPtr* _return/*返回*/)
	{
		GameObject* obj = GetGameMgr()->GetLeaderPtr();
		if(!obj)return;
		*_return = obj->GetID().gameid;
	}
	/*API*/BOOL			WINAPI	GameMgr_Getcamerafollow/*游戏_返回镜头跟随对象是否有效*/()
	{
		return GetGameMgr()->m_cameraFollow;
	}
	/*API*/VOID			WINAPI	GameMgr_Setcamerafollow/*游戏_设置镜头跟随对象是否有效*/(BOOL bEnable/*有效*/)
	{
		GetGameMgr()->m_cameraFollow = bEnable;
	}

	/*API*/VOID			WINAPI	GameMgr_Getcameratarget/*游戏_返回镜头跟随对象*/(IGameObjectPtr* _return/*返回*/)
	{
		*_return = GetGameMgr()->m_cameraTarget;
	}
	/*API*/VOID			WINAPI	GameMgr_Setcameratarget/*游戏_设置镜头跟随对象*/(IGameObjectPtr* obj/*对象*/)
	{
		if(obj)
			GetGameMgr()->m_cameraTarget = *obj;
		else
			GetGameMgr()->m_cameraTarget.id = 0;
	}
	//时间函数列表
	/*API*/INT			WINAPI	GameMgr_timeout/*游戏_定时引发*/(IGameObjectPtr* obj/*对象*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/)
	{
		if(obj)
			return g_luamanager.AddTimeout(nMillisecond, szLuaFunc, 0, LuaTimeData::IDT_GameID, 0, obj->index, obj->id);
		return 0;
	}
	/*API*/INT			WINAPI	GameMgr_interval/*游戏_间隔引发*/(IGameObjectPtr* obj/*对象*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用次数*/)//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(obj,tm,endof)--(obj=GameObject,endof=bool,tm=float)tm是秒计数的小数,endof只有在计时器结束的时候被调用一次];返回TimeID
	{
		if(obj)
			return g_luamanager.AddInterval(nMillisecond, nMaxMillisecond, szLuaFunc, 0, LuaTimeData::IDT_GameID, 0, obj->index, obj->id);
		return 0;
	}
	/*API*/INT			WINAPI	GameMgr_framemove/*游戏_循环引发*/(IGameObjectPtr* obj/*对象*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/)//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(obj,tm,endof)--(obj=GameObject,endof=bool,tm=float)tm是秒计数的小数,endof只有在计时器结束的时候被调用一次];返回TimeID
	{
		if(obj)
			return g_luamanager.AddFramemove(nMaxMillisecond, szLuaFunc, 0, LuaTimeData::IDT_GameID, 0, obj->index, obj->id);
		return 0;
	}
	/*API*/BOOL			WINAPI	GameMgr_Getplotenable/*游戏_返回剧情是否有效*/()//剧情有效之后才会引发onscene_idleover,onscene_moveover,onscene_next,onscene_finish
	{
		return GetGameMgr()->m_bPlotEnable;
	}
	/*API*/VOID			WINAPI	GameMgr_Setplotenable/*游戏_设置剧情是否有效*/(BOOL bEnable/*设置*/)//剧情有效之后才会引发onscene_idleover,onscene_moveover,onscene_next,onscene_finish
	{
		GetGameMgr()->SetPlotEnable(bEnable);
	}
	/*API*/VOID			WINAPI	GameMgr_resetAll/*游戏_重置所有角色的位置信息血条，状态等*/()
	{
		GetGameMgr()->ResetAllObjects();
	}
	/*API*/INT			WINAPI	GameMgr_plotid/*游戏_当前剧情脚本ID*/()
	{
		return GetGameMgr()->m_plotid;
	}
	/*API*/LPCSTR		WINAPI	GameMgr_ray3d_getname/*游戏_返回碰撞检测之后的碰撞模型名字*/()
	{
		return GetCameraPtr()->GetIntersectPtr()->m_info.name.t;
	}
	/*API*/float		WINAPI	GameMgr_getheight_3d/*游戏_根据碰撞子模型取高度*/(const float3* pos/*位置*/)
	{
		if(!GetGameMgr()->GetScenePtr())return 0;
		return GetGameMgr()->GetScenePtr()->GetHeight(*pos);
	}
	/*API*/VOID			WINAPI	GameMgr_ray3d_point/*游戏_返回碰撞检测之后的值*/(float3* _return/*返回*/)
	{
		*_return = GetCameraPtr()->GetIntersectPtr()->m_info.vPick;
	}
	/*API*/VOID			WINAPI	GameMgr_ray3d_normal/*游戏_返回碰撞检测之后的法线*/(float3* _return/*返回*/)
	{
		*_return = GetCameraPtr()->GetIntersectPtr()->m_info.vFaceNormal;
	}
	/*API*/float		WINAPI	GameMgr_ray3d_length/*游戏_返回碰撞检测之后的距离*/()
	{
		return GetCameraPtr()->GetIntersectPtr()->m_info.fDist;
	}

	/*API*/VOID			WINAPI	GameMgr_ray3d/*游戏_碰撞检测_仅与碰撞子模型*/(const Pixel* pixel/*屏幕坐标位置*/, IMap3DItemPtr* _return/*返回*/)
	{
		if(!GetGameMgr()->GetScenePtr())return;
		IntersectData* data = GetCameraPtr()->BeginIntersect(pixel->x, pixel->y);
		SceneObject* obj = GetGameMgr()->GetScenePtr()->Intersect(data, true);
		if(!obj)
			return;
		SetIPtr(_return, obj, SceneObject, IMap3DItemPtr);
		//GameObject* gobj = GetGameMgr()->Search(obj->m_gameID.gameid);
		//if(gobj)
		//	*_return = gobj->GetID().gameid;
	}
	/*API*/VOID			WINAPI	GameMgr_ray3d_dir2/*游戏_碰撞检测_仅与碰撞子模型限制距离*/(const float3* orig/*原点*/, const float3* dir/*方向*/, float fMaxLength/*计算最大距离*/, IMap3DItemPtr* _return/*返回*/)
	{
		if(!GetGameMgr()->GetScenePtr())return;
		IntersectData* data = GetCameraPtr()->GetIntersectPtr();
		if(!data->begin(*orig, *dir, fMaxLength))return;
		SceneObject* obj = GetGameMgr()->GetScenePtr()->Intersect(data, true);
		if(!obj)
			return;
		//GameObject* gobj = GetGameMgr()->Search(obj->m_gameID.gameid);
		//if(gobj)
		//	*_return = gobj->GetID().gameid;
	}
	/*API*/VOID			WINAPI	GameMgr_ray3d_dir/*游戏_碰撞检测_仅与碰撞子模型*/(const float3* orig/*原点*/, const float3* dir/*方向*/, IMap3DItemPtr* _return/*返回*/)
	{
		if(!GetGameMgr()->GetScenePtr())return;
		IntersectData* data = GetCameraPtr()->GetIntersectPtr();
		if(!data->begin(*orig, *dir, 1000000.0f))return;
		SceneObject* obj = GetGameMgr()->GetScenePtr()->Intersect(data, true);
		if(!obj)
			return;
		//GameObject* gobj = GetGameMgr()->Search(obj->m_gameID.gameid);
		//if(gobj)
		//	*_return = gobj->GetID().gameid;
	}

};
