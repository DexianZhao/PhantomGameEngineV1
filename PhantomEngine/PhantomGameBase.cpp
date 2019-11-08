#include "PhantomGameBase.h"
#include "PhantomGame.h"
#include "PhantomManager.h"
#include "APILua.h"
#include "LuaManager.h"
extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}
#include "lua/lua_tinker.h"
using namespace lua_tinker;
namespace Phantom{
#define CASE_TCLASS_DEFINE(tindex)	case tindex:class_property_ptr< Tclass >(L, col->name.t, &Tclass::template GetClass<tindex>, &Tclass::template SetClass<tindex>, i);break

#define CASE_TCLASS_INDEX_FUN(tindex,getfn,setfn,tclass)	case tindex: \
	class_def< Tclass >(L, col->name.t, &Tclass::template getfn<tindex,tclass>);\
	class_def< Tclass >(L, buf, &Tclass::template setfn<tindex,tclass>);break

#define CASE_TCLASS_INDEX_CLASS(tii, too)	case tii: \
	class_def< Tclass >(L, col->name.t, &Tclass::template T_GetClass<tii, too>);\
	class_def< Tclass >(L, buf, &Tclass::template T_SetClass<tii, too>);break

#define CASE_TCLASS_INDEX_DEFINE(tindex, tother)	case tother: \
	switch(tindex){ \
	CASE_TCLASS_INDEX_CLASS(0,tother); \
	CASE_TCLASS_INDEX_CLASS(1,tother); \
	CASE_TCLASS_INDEX_CLASS(2,tother); \
	CASE_TCLASS_INDEX_CLASS(3,tother); \
	CASE_TCLASS_INDEX_CLASS(4,tother); \
	CASE_TCLASS_INDEX_CLASS(5,tother); \
	CASE_TCLASS_INDEX_CLASS(6,tother); \
	CASE_TCLASS_INDEX_CLASS(7,tother); \
	CASE_TCLASS_INDEX_CLASS(8,tother); \
	CASE_TCLASS_INDEX_CLASS(9,tother); \
};break

	template <class Tclass>
	void RegisterLuaClass(Config* cfg, int beginCol){
		if(cfg->GetNumCol()==0)return;
		lua_State* L = g_luamanager.GetLuaState();
		for(int i=beginCol;i<cfg->GetNumCol();i++){
			ConfigHeader* col = cfg->GetCol(i);
			if(col->listcount>1)
				continue;
			switch(col->type){
			case ConfigHeader::type_float:
				class_property< Tclass >(L, col->name.t, &Tclass::template GetT<float>, &Tclass::template SetT<float>, i);
				break;
			case ConfigHeader::type_int:
				class_property< Tclass >(L, col->name.t, &Tclass::template GetT<int>, &Tclass::template SetT<int>, i);
				break;
			case ConfigHeader::type_boolean:
				class_property< Tclass >(L, col->name.t, &Tclass::template GetT<bool>, &Tclass::template SetT<bool>, i);
				break;
			case ConfigHeader::type_class:
				{
					switch(col->classPtr->GetConfigIndex()){
						CASE_TCLASS_DEFINE(0);
						CASE_TCLASS_DEFINE(1);
						CASE_TCLASS_DEFINE(2);
						CASE_TCLASS_DEFINE(3);
						CASE_TCLASS_DEFINE(4);
						CASE_TCLASS_DEFINE(5);
						CASE_TCLASS_DEFINE(6);
						CASE_TCLASS_DEFINE(7);
						CASE_TCLASS_DEFINE(8);
						CASE_TCLASS_DEFINE(9);
						CASE_TCLASS_DEFINE(10);
						CASE_TCLASS_DEFINE(11);
						CASE_TCLASS_DEFINE(12);
						CASE_TCLASS_DEFINE(13);
						CASE_TCLASS_DEFINE(14);
						CASE_TCLASS_DEFINE(15);
						CASE_TCLASS_DEFINE(16);
						CASE_TCLASS_DEFINE(17);
						CASE_TCLASS_DEFINE(18);
						CASE_TCLASS_DEFINE(19);
					}
				}
				break;
			default:
				class_property< Tclass >(L, col->name.t, &Tclass::template GetT<const char*>, &Tclass::template SetT<const char*>, i);
				break;
			}
		}
		char buf[128];
		for(int i=0;i<cfg->GetArrayCount();i++){
			int c = cfg->GetArrayCol(i);
			ConfigHeader* col = cfg->GetCol(c);
			assert(col->listcount>1);
			if(col->listcount<=1)//如果是数组则注册为函数
				continue;
			sprintf(buf, "%scount", col->name.t);
			switch(i){
			case 0:class_def< Tclass >(L, buf, &Tclass::template T_Count<0>);break;
			case 1:class_def< Tclass >(L, buf, &Tclass::template T_Count<1>);break;
			case 2:class_def< Tclass >(L, buf, &Tclass::template T_Count<2>);break;
			case 3:class_def< Tclass >(L, buf, &Tclass::template T_Count<3>);break;
			case 4:class_def< Tclass >(L, buf, &Tclass::template T_Count<4>);break;
			case 5:class_def< Tclass >(L, buf, &Tclass::template T_Count<5>);break;
			case 6:class_def< Tclass >(L, buf, &Tclass::template T_Count<6>);break;
			case 7:class_def< Tclass >(L, buf, &Tclass::template T_Count<7>);break;
			case 8:class_def< Tclass >(L, buf, &Tclass::template T_Count<8>);break;
			case 9:class_def< Tclass >(L, buf, &Tclass::template T_Count<9>);break;
			}
			sprintf(buf, "set%s", col->name.t);
			switch(col->type){
			case ConfigHeader::type_float:
				switch(i){
					CASE_TCLASS_INDEX_FUN(0, T_Get,T_Set,float);
					CASE_TCLASS_INDEX_FUN(1, T_Get,T_Set,float);
					CASE_TCLASS_INDEX_FUN(2, T_Get,T_Set,float);
					CASE_TCLASS_INDEX_FUN(3, T_Get,T_Set,float);
					CASE_TCLASS_INDEX_FUN(4, T_Get,T_Set,float);
					CASE_TCLASS_INDEX_FUN(5, T_Get,T_Set,float);
					CASE_TCLASS_INDEX_FUN(6, T_Get,T_Set,float);
					CASE_TCLASS_INDEX_FUN(7, T_Get,T_Set,float);
					CASE_TCLASS_INDEX_FUN(8, T_Get,T_Set,float);
					CASE_TCLASS_INDEX_FUN(9, T_Get,T_Set,float);
				};
				break;
			case ConfigHeader::type_int:
				switch(i){
					CASE_TCLASS_INDEX_FUN(0, T_Get,T_Set,int);
					CASE_TCLASS_INDEX_FUN(1, T_Get,T_Set,int);
					CASE_TCLASS_INDEX_FUN(2, T_Get,T_Set,int);
					CASE_TCLASS_INDEX_FUN(3, T_Get,T_Set,int);
					CASE_TCLASS_INDEX_FUN(4, T_Get,T_Set,int);
					CASE_TCLASS_INDEX_FUN(5, T_Get,T_Set,int);
					CASE_TCLASS_INDEX_FUN(6, T_Get,T_Set,int);
					CASE_TCLASS_INDEX_FUN(7, T_Get,T_Set,int);
					CASE_TCLASS_INDEX_FUN(8, T_Get,T_Set,int);
					CASE_TCLASS_INDEX_FUN(9, T_Get,T_Set,int);
				};
				break;
			case ConfigHeader::type_boolean:
				switch(i){
					CASE_TCLASS_INDEX_FUN(0, T_Get,T_Set,bool);
					CASE_TCLASS_INDEX_FUN(1, T_Get,T_Set,bool);
					CASE_TCLASS_INDEX_FUN(2, T_Get,T_Set,bool);
					CASE_TCLASS_INDEX_FUN(3, T_Get,T_Set,bool);
					CASE_TCLASS_INDEX_FUN(4, T_Get,T_Set,bool);
					CASE_TCLASS_INDEX_FUN(5, T_Get,T_Set,bool);
					CASE_TCLASS_INDEX_FUN(6, T_Get,T_Set,bool);
					CASE_TCLASS_INDEX_FUN(7, T_Get,T_Set,bool);
					CASE_TCLASS_INDEX_FUN(8, T_Get,T_Set,bool);
					CASE_TCLASS_INDEX_FUN(9, T_Get,T_Set,bool);
				};
				break;
			case ConfigHeader::type_class:
				{
					switch(col->classPtr->GetConfigIndex()){
						CASE_TCLASS_INDEX_DEFINE(i, 0);
						CASE_TCLASS_INDEX_DEFINE(i, 1);
						CASE_TCLASS_INDEX_DEFINE(i, 2);
						CASE_TCLASS_INDEX_DEFINE(i, 3);
						CASE_TCLASS_INDEX_DEFINE(i, 4);
						CASE_TCLASS_INDEX_DEFINE(i, 5);
						CASE_TCLASS_INDEX_DEFINE(i, 6);
						CASE_TCLASS_INDEX_DEFINE(i, 7);
						CASE_TCLASS_INDEX_DEFINE(i, 8);
						CASE_TCLASS_INDEX_DEFINE(i, 9);
						CASE_TCLASS_INDEX_DEFINE(i, 10);
						CASE_TCLASS_INDEX_DEFINE(i, 11);
						CASE_TCLASS_INDEX_DEFINE(i, 12);
						CASE_TCLASS_INDEX_DEFINE(i, 13);
						CASE_TCLASS_INDEX_DEFINE(i, 14);
						CASE_TCLASS_INDEX_DEFINE(i, 15);
						CASE_TCLASS_INDEX_DEFINE(i, 16);
						CASE_TCLASS_INDEX_DEFINE(i, 17);
						CASE_TCLASS_INDEX_DEFINE(i, 18);
						CASE_TCLASS_INDEX_DEFINE(i, 19);
					}
				}
				break;
			default:
				switch(i){
					CASE_TCLASS_INDEX_FUN(0, T_Get,T_Set, const char*);
					CASE_TCLASS_INDEX_FUN(1, T_Get,T_Set, const char*);
					CASE_TCLASS_INDEX_FUN(2, T_Get,T_Set, const char*);
					CASE_TCLASS_INDEX_FUN(3, T_Get,T_Set, const char*);
					CASE_TCLASS_INDEX_FUN(4, T_Get,T_Set, const char*);
					CASE_TCLASS_INDEX_FUN(5, T_Get,T_Set, const char*);
					CASE_TCLASS_INDEX_FUN(6, T_Get,T_Set, const char*);
					CASE_TCLASS_INDEX_FUN(7, T_Get,T_Set, const char*);
					CASE_TCLASS_INDEX_FUN(8, T_Get,T_Set, const char*);
					CASE_TCLASS_INDEX_FUN(9, T_Get,T_Set, const char*);
				};
				break;
			}
		}
	}
	int				GameTableBase::GetID()
	{CPUTime(GameTableBase);
		ConfigLine* line = GetGameMgr()->SearchLine(m_id);
		if(!line)return 0;
		return line->GetLineID();
	}
	VOID			GameTableBase::SetID(int id)
	{CPUTime(GameTableBase);
		ConfigLine* line = GetGameMgr()->SearchLine(m_id);
		if(!line)return;
		line = line->GetConfigPtr()->GetLineFromID(id);
		if(!line)return;
		m_id = line->GetID();
	}
	const char*		GameTableBase::GetName()
	{CPUTime(GameTableBase);
		ConfigLine* line = GetGameMgr()->SearchLine(m_id);
		if(!line)return "";
		return line->GetLineName();
	}
	VOID			GameTableBase::SetName(const char* name)
	{CPUTime(GameTableBase);
		ConfigLine* line = GetGameMgr()->SearchLine(m_id);
		if(!line)return;
		for(int i=0;i<line->GetConfigPtr()->GetLineCount();i++){
			ConfigLine* l = line->GetConfigPtr()->GetLine(i);
			if(stricmp(l->GetLineName(), name)==0){
				m_id = l->GetID();
				break;
			}
		}
	}

	template <typename Tclass>	Tclass	GameTableBase::GetT(int id)
	{CPUTime(GameTableBase);
		ConfigLine* line = GetGameMgr()->SearchLine(m_id);
		if(!line)return 0;
		Tclass temp;
		return line->GetVal(id, 0, temp);
	}
	template <typename Tclass>	void	GameTableBase::SetT(Tclass val, int id)
	{CPUTime(GameTableBase);
		ConfigLine* line = GetGameMgr()->SearchLine(m_id);
		if(!line)return;
		return line->SetVal(id, 0, val);
	}
	template <int Tother> lua_GameTable<Tother>	GameTableBase::GetClass(int id)
	{CPUTime(GameTableBase);
		return lua_GameTable<Tother>(GetGameMgr()->GetLineOther(m_id, id, 0));
	}
	template <int Tother> void		GameTableBase::SetClass(const lua_GameTable<Tother>& other, int id)
	{CPUTime(GameTableBase);
		GetGameMgr()->SetLineOtherC(m_id, id, other.m_id, 0);
	}
	template <typename Tclass>
	Tclass		GameTableBase::T_GetVal(int index, int Tindex)
	{CPUTime(GameTableBase);
		ConfigLine* line = GetGameMgr()->SearchLine(m_id);
		if(!line)return 0;
		int col = line->GetConfigPtr()->GetArrayCol(Tindex);
		if(col <= 0)return 0;
		Tclass temp;
		return line->GetVal(col, index, temp);
	}
	template <typename Tclass>
	void			GameTableBase::T_SetVal(Tclass val, int index, int Tindex)
	{CPUTime(GameTableBase);
		ConfigLine* line = GetGameMgr()->SearchLine(m_id);
		if(!line)return;
		int col = line->GetConfigPtr()->GetArrayCol(Tindex);
		if(col <= 0)return;
		line->SetVal(col, index, val);
	}
	int				GameTableBase::T_ArrayCount(int Tindex)
	{CPUTime(GameTableBase);
		ConfigLine* line = GetGameMgr()->SearchLine(m_id);
		if(!line)return 0;
		int col = line->GetConfigPtr()->GetArrayCol(Tindex);
		if(col <= 0)return 0;
		return line->GetConfigPtr()->GetCol(col)->listcount;
	}

	ConfigLine*		GameTableBase::GetClassLine(int index, int Tindex){CPUTime(GameTableBase);
		ConfigLine* line = GetGameMgr()->SearchLine(m_id);
		if(!line)return 0;
		int col = line->GetConfigPtr()->GetArrayCol(Tindex);
		if(col <= 0)return 0;
		return line->GetOtherLinePtr(col, index);
	}
	VOID			GameTableBase::SetClassLine(int index, int Tindex, const GameID& other){CPUTime(GameTableBase);
		ConfigLine* line = GetGameMgr()->SearchLine(m_id);
		if(!line)return;
		int col = line->GetConfigPtr()->GetArrayCol(Tindex);
		if(col <= 0)return;
		line->SetOtherLinePtr(col, GetGameMgr()->SearchLineC(other), index);
	}
	template <int Tindex> int								GameTableBase::T_Count(){CPUTime(GameTableBase);return T_ArrayCount(Tindex);}
	template <int Tindex, typename Tclass>	Tclass			GameTableBase::T_Get(int id){
		return T_GetVal<Tclass>(id, Tindex);
	}
	template <int Tindex, typename Tclass>	void			GameTableBase::T_Set(int id, Tclass val){
		T_SetVal<Tclass>(val, id, Tindex);
	}
	template <int Tindex,int Tother>	lua_GameTable<Tother>	GameTableBase::T_GetClass(int index)
	{
		ConfigLine* temp = GetClassLine(index, Tindex);
		lua_GameTable<Tother> other;
		if(temp)
			other.m_id = temp->GetID();
		return other;
	}
	template <int Tindex,int Tother>	 void				GameTableBase::T_SetClass(int index, const lua_GameTable<Tother>& other){
		SetClassLine(index, Tindex, other.m_id);
	}
	template <int Tindex> lua_GameTable<Tindex>::lua_GameTable(int _id){CPUTime(lua_GameTable<Tindex>);
		m_id.id = 0;
		if(s_config)
		{
			int count = s_config->GetLineCount();
			for(int i=0;i<count;i++){
				if(s_config->GetLine(i)->GetLineID()==_id){
					m_id = s_config->GetLine(i)->GetID();
					break;
				}
			}
		}
	}
	template <int Tindex> void	lua_GameTable<Tindex>::RegisterLua(Config* cfg){CPUTime(lua_GameTable<Tindex>);
		s_config = cfg;
		if(cfg->GetNumCol()<2)return;
		lua_State* L = g_luamanager.GetLuaState();
		class_add< lua_GameTable<Tindex> >(L, cfg->GetTableName());
		class_con< lua_GameTable<Tindex> >(L, constructor< lua_GameTable<Tindex> >);
		class_con< lua_GameTable<Tindex> >(L, constructor< lua_GameTable<Tindex>, int >);
		class_def< lua_GameTable<Tindex> >(L, "isnull", &lua_GameTable<Tindex>::isnull);
		ConfigHeader* idcol = cfg->GetCol(0);
		ConfigHeader* namecol = cfg->GetCol(1);
		class_property< lua_GameTable<Tindex> >(L, idcol->name.t, &lua_GameTable<Tindex>::GetID, &lua_GameTable<Tindex>::SetID);
		class_property< lua_GameTable<Tindex> >(L, namecol->name.t, &lua_GameTable<Tindex>::GetName, &lua_GameTable<Tindex>::SetName);
		RegisterLuaClass< lua_GameTable<Tindex> >(cfg, 2);
	}
	template <int Tindex> Config* lua_GameTable<Tindex>::s_config = 0;

	Config* lua_GameObject_s::s_config = 0;
	void lua_GameObject_s::registerLua(){CPUTime(lua_GameObject_s);
		Config* cfg = GetGameMgr()->GetRoleTable();
		if(!cfg)return;
		s_config = cfg;
		lua_State* L = g_luamanager.GetLuaState();
		class_con<lua_GameObject_s>(L, constructor< lua_GameObject_s, int >);
		class_property< lua_GameObject_s >(L, "gameid", &lua_GameObject_s::GetID, &lua_GameObject_s::SetID);
		RegisterLuaClass< lua_GameObject_s >(cfg, 0);
	}

#define CASE_GAME_TABLE(tindex)	case tindex:lua_GameTable<tindex>::RegisterLua(m_configLua[index]);break
	VOID		ConfigMgr::RegisterAllLuaClass()//注册所有lua变量列表,每次初始化脚本都需要调用一次
	{CPUTime(ConfigMgr);
		for(int index=0;index<m_configLua.size();index++){
			switch(index){
				CASE_GAME_TABLE(0);
				CASE_GAME_TABLE(1);
				CASE_GAME_TABLE(2);
				CASE_GAME_TABLE(3);
				CASE_GAME_TABLE(4);
				CASE_GAME_TABLE(5);
				CASE_GAME_TABLE(6);
				CASE_GAME_TABLE(7);
				CASE_GAME_TABLE(8);
				CASE_GAME_TABLE(9);
				CASE_GAME_TABLE(10);
				CASE_GAME_TABLE(11);
				CASE_GAME_TABLE(12);
				CASE_GAME_TABLE(13);
				CASE_GAME_TABLE(14);
				CASE_GAME_TABLE(15);
				CASE_GAME_TABLE(16);
				CASE_GAME_TABLE(17);
				CASE_GAME_TABLE(18);
				CASE_GAME_TABLE(19);
			}
		}
		lua_GameObject_s::registerLua();
		lua_GameMgr::registerLua();
	}
	int		lua_GameObject_s::GetID()
	{CPUTime(lua_GameObject_s);
		return m_data.id;
	}
	void	lua_GameObject_s::SetID(int id)
	{CPUTime(lua_GameObject_s);
		m_data.id = id;
	}
	int		lua_GameObject_s::getcontext()
	{CPUTime(lua_GameObject_s);
		GameObject* obj = GetGameMgr()->Search(m_data);
		if(!obj)return 0;
		return obj->m_content;
	}
	void	lua_GameObject_s::setcontext(int id)
	{CPUTime(lua_GameObject_s);
		GameObject* obj = GetGameMgr()->Search(m_data);
		if(!obj)return;
		obj->m_content = id;
	}
	template <int Tindex>					int		lua_GameObject_s::T_Count()
	{CPUTime(lua_GameObject_s);
		GameObject* obj = GetGameMgr()->Search(m_data);
		if(!obj)return 0;
		ConfigLine* line = obj->GetData();
		if(!line)return 0;
		int col = line->GetConfigPtr()->GetArrayCol(Tindex);
		if(col <= 0)return 0;
		return line->GetConfigPtr()->GetCol(col)->listcount;
	}
	template <int Tindex, typename Tclass>	Tclass	lua_GameObject_s::T_Get(int index)
	{CPUTime(lua_GameObject_s);
		GameObject* obj = GetGameMgr()->Search(m_data);
		if(!obj)return 0;
		ConfigLine* line = obj->GetData();
		if(!line)return 0;
		int col = line->GetConfigPtr()->GetArrayCol(Tindex);
		if(col <= 0)return 0;
		Tclass temp;
		return line->GetVal(col, index, temp);
	}
	template <int Tindex, typename Tclass>	void	lua_GameObject_s::T_Set(int index, Tclass val)
	{CPUTime(lua_GameObject_s);
		GameObject* obj = GetGameMgr()->Search(m_data);
		if(!obj)return;
		ConfigLine* line = obj->GetData();
		if(!line)return;
		int col = line->GetConfigPtr()->GetArrayCol(Tindex);
		if(col <= 0)return;
		line->SetVal(col, index, val);
	}
	template <int Tindex,int Tother>	lua_GameTable<Tother>	lua_GameObject_s::T_GetClass(int index)
	{CPUTime(lua_GameObject_s);
		lua_GameTable<Tother> other;
		GameObject* obj = GetGameMgr()->Search(m_data);
		if(obj&&obj->GetData()){
			ConfigLine* line = obj->GetData();
			int col = line->GetConfigPtr()->GetArrayCol(Tindex);
			if(col <= 0)
				return other;
			ConfigLine* l = line->GetOtherLinePtr(col, index);
			if(l)
				other.m_id = l->GetID();
		}
		return other;
	}
	template <int Tindex,int Tother>	 void				lua_GameObject_s::T_SetClass(int index, const lua_GameTable<Tother>& other)
	{CPUTime(lua_GameObject_s);
		GameObject* obj = GetGameMgr()->Search(m_data);
		if(obj&&obj->GetData()){
			ConfigLine* line = obj->GetData();
			int col = line->GetConfigPtr()->GetArrayCol(Tindex);
			if(col <= 0)
				return;
			line->SetOtherLinePtr(col, GetGameMgr()->SearchLineC(other.m_id), index);
		}
	}
	template <typename Tclass>	Tclass	lua_GameObject_s::GetT(int id)
	{CPUTime(lua_GameObject_s);
		GameObject* obj = GetGameMgr()->Search(m_data);
		if(!obj)return 0;
		ConfigLine* line = obj->GetData();
		if(!line)return 0;
		Tclass temp;
		return line->GetVal(id, 0, temp);
	}
	template <typename Tclass>	void	lua_GameObject_s::SetT(Tclass val, int id)
	{CPUTime(lua_GameObject_s);
		GameObject* obj = GetGameMgr()->Search(m_data);
		if(!obj)return;
		ConfigLine* line = obj->GetData();
		if(!line)return;
		line->SetVal(id, 0, val);
	}
	template <int Tother> lua_GameTable<Tother>	lua_GameObject_s::GetClass(int id){CPUTime(lua_GameObject_s);
		GameObject* obj = GetGameMgr()->Search(m_data);
		lua_GameTable<Tother> other;
		if(obj){
			ConfigLine* line = obj->GetData();
			if(line){
				ConfigLine* otherLine = line->GetOtherLinePtr(id);
				if(otherLine)
					other.m_id = otherLine->GetID();
			}
		}
		return other;
	}
	template <int Tother> void		lua_GameObject_s::SetClass(const lua_GameTable<Tother>& other, int id){CPUTime(lua_GameObject_s);
		if(other.m_id.id<=0)return;
		GameObject* obj = GetGameMgr()->Search(m_data);
		if(!obj)return;
		ConfigLine* line = obj->GetData();
		if(!line)return;
		line->SetOtherLinePtr(id, GetGameMgr()->SearchLineC(other.m_id));
	}

	/*API*/BOOL			WINAPI	GameObject_moveto/*游戏对象_移动到目标点*/(IGameObjectPtr* _self/*游戏对象*/, const float3* pos/*位置*/)
	{CPUTime(Global);
		if(!pos)return false;
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		obj->NextPause(true);
		if(obj->m_bUseFindway&&GetGameMgr()->GetScenePtr()->IsEnabled(Scene3D::Enable_MoveGridsData)){
			CDynamicArrayS<WayPoint2D> temp;
			if(!GetGameMgr()->GetScenePtr()->FindPath(temp, obj->GetPosition(), *pos)){
				if(!GetGameMgr()->GetScenePtr()->IsMoveGridEnable(obj->GetPosition()))//如果出不来了则直接进入目标点
					return obj->moveto(*pos);
				return false;
			}
			obj->clearNext();
			obj->moveto(float3(temp[0].pos.x, temp[0].pos.y, pos->z));
			for(int i=1;i<temp.size();i++){
				obj->nextto(float3(temp[i].pos.x, temp[i].pos.y, pos->z));
			}
			obj->NextPause(false);
			return true;
		}
		return obj->moveto(*pos);
	}
	/*API*/BOOL			WINAPI	GameObject_moveto_t/*游戏对象_移动到目标对象*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		obj->NextPause(true);
		return obj->movetoTarget();
	}
	/*API*/VOID			WINAPI	GameObject_moveto_b/*游戏对象_移动到出生点*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->NextPause(true);
		obj->movetoBorn();
	}
	/*API*/BOOL			WINAPI	GameObject_attack/*游戏对象_攻击*/(IGameObjectPtr* _self/*游戏对象*/, int actionindex/*攻击动作组的索引*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		obj->NextPause(true);
		return obj->attack(actionindex);
	}
	/*API*/VOID			WINAPI	GameObject_lookat/*游戏对象_面向目标*/(IGameObjectPtr* _self/*游戏对象*/, const float3* target/*位置*/)
	{
		if(!target)return;
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->lookat(*target);
	}
	/*API*/VOID			WINAPI	GameObject_lookat_t/*游戏对象_面向目标对象*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->lookatTarget();
	}

	/*API*/VOID			WINAPI	GameObject_Getbornpos/*游戏对象_取得出生点坐标*/(IGameObjectPtr* _self/*游戏对象*/, float3* _return/*位置*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		*_return = obj->m_bornPos;
	}
	/*API*/VOID			WINAPI	GameObject_Setbornpos/*游戏对象_设置出生点坐标*/(IGameObjectPtr* _self/*游戏对象*/, const float3* target/*位置*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->m_bornPos = *target;
	}
	/*API*/VOID			WINAPI	GameObject_Getpos/*游戏对象_取得坐标*/(IGameObjectPtr* _self/*游戏对象*/, float3* _return/*位置*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		*_return = obj->GetPosition();
	}
	/*API*/VOID			WINAPI	GameObject_Setpos/*游戏对象_设置坐标*/(IGameObjectPtr* _self/*游戏对象*/, const float3* pos/*位置*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->SetPosition(*pos);
	}

	/*API*/VOID			WINAPI	GameObject_idle/*游戏对象_休闲动作*/(IGameObjectPtr* _self/*游戏对象*/, float fTime/*休闲动作持续时间*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->idle(fTime);
	}
	/*API*/BOOL			WINAPI	GameObject_doskill_r/*游戏对象_使用技能攻击目标点群体*/(IGameObjectPtr* _self/*游戏对象*/, int skillid/*技能表中的技能ID*/, const float3* target/*技能施放目标点*/)
	{
		if(!target)return false;
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		obj->NextPause(true);
		return obj->doskill(skillid, *target);
	}
	/*API*/BOOL			WINAPI	GameObject_doskill_t/*游戏对象_使用技能攻击当前目标*/(IGameObjectPtr* _self/*游戏对象*/, int skillid/*技能表中的技能ID*/)
	{CPUTime(Global);
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		obj->NextPause(true);
		GameObject* target = obj->getTarget();
		if(!target)return false;
		float3 dir = target->GetPosition() - obj->GetPosition();
		if(dir.R()<=obj->m_skillRange)
			return obj->doskillTarget(skillid);;
		if(obj->m_bUseFindway&&GetGameMgr()->GetScenePtr()->IsEnabled(Scene3D::Enable_MoveGridsData)){
			CDynamicArrayS<WayPoint2D> temp;
			if(!GetGameMgr()->GetScenePtr()->FindPath(temp, obj->GetPosition(), target->GetPosition())){
				if(!GetGameMgr()->GetScenePtr()->IsMoveGridEnable(obj->GetPosition()))//如果出不来了则直接进入目标点
					return obj->doskillTarget(skillid);
				obj->idle(0);
				return false;
			}
			if(temp.size()>2){
				float tz = target->GetPosition().z;
				obj->moveto(float3(temp[1].pos.x, temp[1].pos.y, tz));
				for(int i=2;i<temp.size()-1;i++){
					obj->nextto(float3(temp[i].pos.x, temp[i].pos.y, tz));
				}
				obj->nextskill_t(skillid);
			}
			else
				return obj->doskillTarget(skillid);
			return true;
		}
		if(!obj->doskillTarget(skillid))
		{
			return false;
		}
		return true;
	}

	/*API*/VOID			WINAPI	GameObject_clone/*游戏对象_克隆对象*/(IGameObjectPtr* _self/*游戏对象*/, IGameObjectPtr* _return/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		GameObject* cobj = obj->Clone();
		*_return = cobj->GetID().gameid;
	}

	/*API*/VOID			WINAPI	GameObject_onhit/*游戏对象_被击动作*/(IGameObjectPtr* _self/*游戏对象*/, IGameObjectPtr* enemy/*敌人*/, int val/*造成伤害的值*/, float fPlayTime)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->onhit(GetGameMgr()->Search(*enemy), val, fPlayTime);
	}
	/*API*/VOID			WINAPI	GameObject_onhit2/*游戏对象_被击动作返回控件指针*/(IGameObjectPtr* _self/*游戏对象*/, IGameObjectPtr* enemy/*敌人*/, int val/*造成伤害的值*/, float fPlayTime/*播放动作最长时间*/, IControlPtr* _return)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		UIControl* c = obj->onhit(GetGameMgr()->Search(*enemy), val, fPlayTime);
		if(c){
			SetIPtr(_return, &c->m_this, PhantomUIControlBase, IControlPtr);
		}
	}
	/*API*/BOOL			WINAPI	GameObject_Gethit_text/*游戏对象_取得是否显示被击文字*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		return obj->m_bHitText;
	}
	/*API*/VOID			WINAPI	GameObject_Sethit_text/*游戏对象_设置是否显示被击文字*/(IGameObjectPtr* _self/*游戏对象*/, BOOL bText/*显示被击文字*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->m_bHitText = bText;
	}

	/*API*/VOID			WINAPI	GameObject_pause_t/*游戏对象_暂停一段时间*/(IGameObjectPtr* _self/*游戏对象*/, float fSecond/*秒*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->pause(fSecond);
	}
	/*API*/float		WINAPI	GameObject_height/*游戏对象_取得角色高度*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return 0;
		return obj->m_fHeight;
	}
	/*API*/BOOL			WINAPI	GameObject_Getmeshheight/*游戏对象_取得计算取得模型碰撞盒高度*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return 0;
		return obj->m_bMeshHeight;
	}
	/*API*/VOID			WINAPI	GameObject_Setmeshheight/*游戏对象_设置计算取得模型碰撞盒高度*/(IGameObjectPtr* _self/*游戏对象*/, BOOL bEnable/*是否有效*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->m_bMeshHeight = bEnable;
	}

	/*API*/VOID			WINAPI	GameObject_update_blood/*游戏对象_更新场景对象血条信息*/(IGameObjectPtr* _self/*游戏对象*/, float fProgress/*血条显示进度*/)//更新血条信息,通常是当前生命值/最大生命值(0到1之间)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->updateBloodProgress(fProgress);
	}
	/*API*/void			WINAPI	GameObject_next_setrandom/*游戏对象_设置某个ID的随机值*/(IGameObjectPtr* _self/*游戏对象*/, int index/*索引*/, float fRandom/*发生概率*/)//发生概率在0到100之间，值越大发生概率越高
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		if(index>=0&&index<obj->m_nexts.size())
			obj->m_nexts[index].rnd = 1.0f - clamp(fRandom*0.01f, 0, 1);
	}
	/*API*/void			WINAPI	GameObject_next_setid/*游戏对象_设置某个索引Next的ID*/(IGameObjectPtr* _self/*游戏对象*/, int index/*索引*/, int nid/*NID*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		if(index>=0&&index<obj->m_nexts.size())
			obj->m_nexts[index].id = nid;
	}
	/*API*/BOOL			WINAPI	GameObject_nextenemy/*游戏对象_选择下一个敌人*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		return obj->SelectNextEnemy();
	}
	/*API*/VOID			WINAPI	GameObject_die/*游戏对象_死亡*/(IGameObjectPtr* _self/*游戏对象*/)//, IGameObjectPtr* enemy/*敌人*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->NextPause(true);
		obj->die();//GetGameMgr()->Search(*enemy));
	}
	/*API*/float		WINAPI	GameObject_Getaffrange/*游戏对象_取得影响范围变量*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return 0;
		return obj->m_range;
	}
	/*API*/VOID			WINAPI	GameObject_Setaffrange/*游戏对象_设置影响范围变量*/(IGameObjectPtr* _self/*游戏对象*/, float affRange/*位置*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->m_range = affRange;
	}
	/*API*/float		WINAPI	GameObject_Getatkrange/*游戏对象_取得攻击范围变量*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return 0;
		return obj->m_atkrange;
	}
	/*API*/VOID			WINAPI	GameObject_Setatkrange/*游戏对象_设置攻击范围变量*/(IGameObjectPtr* _self/*游戏对象*/, float atkRange/*位置*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->m_atkrange = atkRange;
	}
	/*API*/BOOL			WINAPI	GameObject_Getmoverun/*游戏对象_取得是否跑步*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		return obj->m_bRunMode;
	}
	/*API*/VOID			WINAPI	GameObject_Setmoverun/*游戏对象_设置是否跑步*/(IGameObjectPtr* _self/*游戏对象*/, BOOL bRun/*是否跑步*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->m_bRunMode = bRun;
	}
	/*API*/BOOL			WINAPI	GameObject_Getsearchway/*游戏对象_取得是否进行寻路*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		return obj->m_bUseFindway;
	}
	/*API*/VOID			WINAPI	GameObject_Setsearchway/*游戏对象_设置是否进行寻路*/(IGameObjectPtr* _self/*游戏对象*/, BOOL bEnable/*是否有效*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->m_bUseFindway = bEnable;
	}
	/*API*/BOOL			WINAPI	GameObject_Getmovedir_enable/*游戏对象_取得是否自动按照方向移动*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		return (obj->GetState()==obj->State_MoveDir);
	}
	/*API*/VOID			WINAPI	GameObject_Setmovedir_enable/*游戏对象_设置是否自动按照方向移动*/(IGameObjectPtr* _self/*游戏对象*/, BOOL bRun/*是否跑步*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->movedir_enable(bRun);
	}
	/*API*/VOID			WINAPI	GameObject_movedir/*游戏对象_设置自动方向移动*/(IGameObjectPtr* _self/*游戏对象*/, const float3* dir/*方向*/)
	{
		if(!dir)return;
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->movedir(*dir);
	}
	/*API*/float		WINAPI	GameObject_Getrot/*游戏对象_取得转动率变量*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return 0;
		return Degress(obj->GetRotation());
	}
	/*API*/VOID			WINAPI	GameObject_Setrot/*游戏对象_设置转动率变量*/(IGameObjectPtr* _self/*游戏对象*/, float fRot/*转动率*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->setRot(Radians(obj->GetRotation()));
	}
	/*API*/VOID				WINAPI	GameObject_sceneobj/*游戏对象_返回场景对象类*/(IGameObjectPtr* _self/*游戏对象*/, IMap3DItemPtr* _return/*返回3D场景对象*/)//从这里返回的场景对象不要直接进行游戏内容和逻辑还有位置设置等操作，将会无效
	{
		if(!GetGameMgr()->GetScenePtr())return;
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		SceneObject* sobj = GetGameMgr()->GetScenePtr()->SearchObject(obj->GetID().sceneid);
		SetIPtr(_return,sobj,SceneObject,IMap3DItemPtr);
	}
	/*API*/float			WINAPI	GameObject_read/*游戏对象_读取一个整数变量*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szName/*变量名*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return 0;
		return obj->readval(szName);
	}
	/*API*/VOID			WINAPI	GameObject_write/*游戏对象_写入一个整数变量*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szName/*变量名*/, float fValue/*值*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->writeval(szName, fValue);
	}

	/*API*/VOID			WINAPI	GameObject_Gettarget/*游戏对象_取得目标对象*/(IGameObjectPtr* _self/*游戏对象*/, IGameObjectPtr* _return/*游戏对象*/)
	{
		_return->id=_return->index=0;
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		if(obj->getTarget()){
			*_return = obj->getTarget()->GetID().gameid;
		}
	}
	/*API*/VOID			WINAPI	GameObject_Settarget/*游戏对象_设置目标对象*/(IGameObjectPtr* _self/*游戏对象*/, const IGameObjectPtr* target/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		assert(sizeof(GameID)==sizeof(IGameObjectPtr));
		GameID t = (GameID&)*target;
		obj->setTarget(GetGameMgr()->Search(t));
	}

	/*API*/VOID			WINAPI	GameObject_bind_idle_over/*游戏对象_绑定_idle事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->AddEvent(Phantom::AIEvent_OnIdleOver, szLuaFunc);
	}
	/*API*/VOID			WINAPI	GameObject_bind_move_over/*游戏对象_绑定_move事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->AddEvent(Phantom::AIEvent_OnMoveOver, szLuaFunc);
	}
	/*API*/VOID			WINAPI	GameObject_bind_die_over/*游戏对象_绑定_die事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->AddEvent(Phantom::AIEvent_OnDieOver, szLuaFunc);
	}
	/*API*/VOID			WINAPI	GameObject_bind_attack_over/*游戏对象_绑定_attack事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->AddEvent(Phantom::AIEvent_OnAttackOver, szLuaFunc);
	}
	/*API*/VOID			WINAPI	GameObject_bind_action_over/*游戏对象_绑定_action事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->AddEvent(Phantom::AIEvent_OnActionOver, szLuaFunc);
	}
	/*API*/VOID			WINAPI	GameObject_bind_hit_over/*游戏对象_绑定_hit事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->AddEvent(Phantom::AIEvent_OnByAttack, szLuaFunc);
	}
	/*API*/VOID			WINAPI	GameObject_bind_nextbegin/*游戏对象_绑定_nextbegin事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->AddEvent(Phantom::AIEvent_OnNextBegin, szLuaFunc);
	}
	/*API*/VOID			WINAPI	GameObject_bind_action_event/*游戏对象_绑定_actionevent事件*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->AddEvent(Phantom::AIEvent_OnActionEvent, szLuaFunc);
	}

	/*API*/int			WINAPI	GameObject_roleid/*游戏对象_角色表对应ID*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj||!obj->GetData())return 0;
		return obj->GetData()->GetLineID();
	}
	/*API*/int			WINAPI	GameObject_sceneid/*游戏对象_场景编辑器对应场景对象ID*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return 0;
		return obj->GetObject()->m_nContentID;
	}
	/*API*/BOOL			WINAPI	GameObject_moveto_e/*游戏对象_移动到事件点*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szEvent/*事件点*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		obj->NextPause(true);
		return obj->movetoE(szEvent);
	}
	/*API*/int			WINAPI	GameObject_nextto_e/*游戏对象_下一个移动到事件点*/(IGameObjectPtr* _self/*游戏对象*/, LPCSTR szEvent/*事件点*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return -1;
		obj->m_nextPause = false;
		if(obj->nexttoE(szEvent))
			return obj->m_nexts.size()-1;
		return -1;
	}
	/*API*/int			WINAPI	GameObject_nextto/*游戏对象_下一个移动到点*/(IGameObjectPtr* _self/*游戏对象*/, const float3* pos/*位置*/)
	{
		if(!pos)return -1;
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return -1;
		obj->m_nextPause = false;
		if(obj->nextto(*pos))
			return obj->m_nexts.size()-1;
		return -1;
	}
	/*API*/int			WINAPI	GameObject_nextidle/*游戏对象_下一个静止*/(IGameObjectPtr* _self/*游戏对象*/, float fTime/*时间*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return -1;
		obj->m_nextPause = false;
		if(obj->nextidle(fTime))
			return obj->m_nexts.size()-1;
		return -1;
	}
	/*API*/int			WINAPI	GameObject_nextaction2/*游戏对象_下一个动作*/(IGameObjectPtr* _self/*游戏对象*/, ActionType type/*动作*/, int nLoop)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		obj->m_nextPause = false;
		GameObject::NextData data;
		memset(&data, 0, sizeof(GameObject::NextData));
		data.ix = type;
		data.iy = nLoop;
		data.type = GameObject::NextData::Type_Action2;
		obj->m_nexts.push_back(data);
		return obj->m_nexts.size()-1;
	}
	/*API*/BOOL			WINAPI	GameObject_nextaction/*游戏对象_下一个动作*/(IGameObjectPtr* _self/*游戏对象*/, ActionType type/*动作*/, float fTime/*时间*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return -1;
		obj->m_nextPause = false;
		if(!obj->nextaction(type, fTime))
			return -1;
		return obj->m_nexts.size()-1;
	}
	/*API*/void			WINAPI	GameObject_action/*游戏对象_动作*/(IGameObjectPtr* _self/*游戏对象*/, ActionType type/*动作*/, float fTime/*时间*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->PlayAction(type, fTime);
	}
	/*API*/void			WINAPI	GameObject_action2/*游戏对象_动作2*/(IGameObjectPtr* _self/*游戏对象*/, ActionType type/*动作*/, int nLoop)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->PlayAction2(type, nLoop);
	}

	/*API*/int			WINAPI	GameObject_nextloop/*游戏对象_循环所有设定*/(IGameObjectPtr* _self/*游戏对象*/, int push_begin/*push_系列返回的索引值*/, int push_end/*push_系列返回的索引值结束*/, int push_loop/*循环多少次这个动作*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return -1;
		GameObject::NextData data;
		memset(&data, 0, sizeof(GameObject::NextData));
		data.type = GameObject::NextData::Type_Loop;
		data.ix = push_begin;
		data.iy = push_end;
		data.iz = push_loop;
		obj->m_nexts.push_back(data);
		obj->m_nextPause = false;
		return obj->m_nexts.size()-1;
	}
	/*API*/void			WINAPI	GameObject_nextclear/*游戏对象_下一个播放*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->clearNext();
	}
	/*API*/void			WINAPI	GameObject_Setnextpause/*游戏对象_暂停播放*/(IGameObjectPtr* _self/*游戏对象*/, BOOL bPause/*暂定与否*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return;
		obj->NextPause(bPause);
	}
	/*API*/BOOL			WINAPI	GameObject_Getnextpause/*游戏对象_暂停播放*/(IGameObjectPtr* _self/*游戏对象*/)
	{
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return false;
		return obj->m_nextPause;
	}

	/*API*/int			WINAPI	GameObject_nextlookat/*游戏对象_循环所有设定*/(IGameObjectPtr* _self/*游戏对象*/, const float3* pos/*目标点*/)
	{
		if(!pos)return -1;
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return -1;
		GameObject::NextData data;
		memset(&data, 0, sizeof(GameObject::NextData));
		data.type = GameObject::NextData::Type_LookatPos;
		data.fx = pos->x;
		data.fy = pos->y;
		data.fz = pos->z;
		obj->m_nexts.push_back(data);
		obj->m_nextPause = false;
		return obj->m_nexts.size()-1;
	}
	/*API*/int			WINAPI	GameObject_nextlookat_t/*游戏对象_循环所有设定*/(IGameObjectPtr* _self/*游戏对象*/, IGameObjectPtr* t/*目标对象*/)
	{
		if(!t)return -1;
		GameObject* obj = GetGameMgr()->Search(*_self);
		if(!obj)return -1;
		GameObject::NextData data;
		memset(&data, 0, sizeof(GameObject::NextData));
		data.type = GameObject::NextData::Type_LookatObj;
		data.ix = (int)t->id;
		data.iy = (int)t->index;
		obj->m_nexts.push_back(data);
		obj->m_nextPause = false;
		return obj->m_nexts.size()-1;
	}


	template <typename Tclass>	Tclass	lua_GameMgr::GetT(int id)
	{CPUTime(lua_GameMgr);
		if(!GetGameMgr()->GetLeaderLine())return 0;
		Tclass temp;
		return GetGameMgr()->GetLeaderLine()->GetVal(id, 0, temp);
	}
	template <typename Tclass>	void	lua_GameMgr::SetT(Tclass val, int id)
	{CPUTime(lua_GameMgr);
		if(!GetGameMgr()->GetLeaderLine())return;
		GetGameMgr()->GetLeaderLine()->SetVal(id, 0, val);
	}

	template <int Tother> lua_GameTable<Tother>	lua_GameMgr::GetClass(int id){CPUTime(lua_GameMgr);
		lua_GameTable<Tother> other;
		if(GetGameMgr()->GetLeaderLine()){
			ConfigLine* otherLine = GetGameMgr()->GetLeaderLine()->GetOtherLinePtr(id);
			if(otherLine)
				other.m_id = otherLine->GetID();
		}
		return other;
	}
	template <int Tother> void		lua_GameMgr::SetClass(const lua_GameTable<Tother>& other, int id){CPUTime(lua_GameMgr);
		if(other.m_id.id<=0)return;
		if(!GetGameMgr()->GetLeaderLine())return;
		GetGameMgr()->GetLeaderLine()->SetOtherLinePtr(id, GetGameMgr()->SearchLineC(other.m_id));
	}

	template <typename Tclass>
	Tclass	T_GetVal(int index, int Tindex){
		if(!GetGameMgr()->GetLeaderLine())return 0;
		int col = GetGameMgr()->GetLeaderTable()->GetArrayCol(Tindex);
		if(col <= 0)return 0;
		Tclass temp;
		return GetGameMgr()->GetLeaderLine()->GetVal(col, index, temp);
	}
	template <typename Tclass>
	void	T_SetVal(Tclass f, int index, int Tindex){
		if(!GetGameMgr()->GetLeaderLine())return;
		int col = GetGameMgr()->GetLeaderTable()->GetArrayCol(Tindex);
		if(col <= 0)return;
		GetGameMgr()->GetLeaderLine()->SetVal(col, index, f);
	}
	int	T_ArrayCount(int Tindex){
		if(!GetGameMgr()->GetLeaderLine())return 0;
		int col = GetGameMgr()->GetLeaderTable()->GetArrayCol(Tindex);
		if(col <= 0)return 0;
		return GetGameMgr()->GetLeaderTable()->GetCol(col)->listcount;
	}
	template <int Tindex> int						lua_GameMgr::T_Count(){CPUTime(lua_GameMgr);return T_ArrayCount(Tindex);}
	template <int Tindex, typename Tclass>	Tclass	lua_GameMgr::T_Get(int id)
	{CPUTime(lua_GameMgr);
		return T_GetVal<Tclass>(id, Tindex);
	}
	template <int Tindex, typename Tclass>	void	lua_GameMgr::T_Set(int id, Tclass val)
	{CPUTime(lua_GameMgr);
		T_SetVal<Tclass>(val, id, Tindex);
	}
	ConfigLine*	GetClassLine(int index, int Tindex){
		if(!GetGameMgr()->GetLeaderLine())return 0;
		int col = GetGameMgr()->GetLeaderTable()->GetArrayCol(Tindex);
		if(col <= 0)return 0;
		return GetGameMgr()->GetLeaderLine()->GetOtherLinePtr(col, index);
	}
	template <int Tindex,int Tother>	lua_GameTable<Tother>	lua_GameMgr::T_GetClass(int index)
	{CPUTime(lua_GameMgr);
		ConfigLine* temp = GetClassLine(index, Tindex);
		lua_GameTable<Tother> other;
		if(temp)
			other.m_id = temp->GetID();
		return other;
	}
	VOID	SetClassLine(int index, int Tindex, const GameID& other){
		if(other.id==0)return;
		if(!GetGameMgr()->GetLeaderLine())return;
		int col = GetGameMgr()->GetLeaderTable()->GetArrayCol(Tindex);
		if(col <= 0)return;
		GetGameMgr()->GetLeaderLine()->SetOtherLinePtr(col, GetGameMgr()->SearchLineC(other), index);
	}
	template <int Tindex,int Tother>	 void				lua_GameMgr::T_SetClass(int index, const lua_GameTable<Tother>& other){CPUTime(lua_GameMgr);
		SetClassLine(index, Tindex, other.m_id);
	}
	void lua_GameMgr::registerLua()
	{CPUTime(lua_GameMgr);
		Config* cfg = GetGameMgr()->GetLeaderTable();
		if(!cfg)return;
		RegisterLuaClass< lua_GameMgr >(cfg, 0);
	}

	int				SceneScript::GetEventCount(){CPUTime(SceneScript);return SceneEventID_Count;}
	BOOL			SceneScript::GetEventName(char* retFunc, char* retFull, int id, BOOL bName)//根据场景名字进行修订
	{CPUTime(SceneScript);
		const char* name = 0;
		if(bName)name = m_name.t;
		char retname[128];
		if(!retFunc)retFunc = retname;
		switch(id){
		case SceneEventID_Init:
			//if(name)
			//	sprintf(retFunc, "%s_onscene_init", name);
			//else
				sprintf(retFunc, "onscene_init");
			if(retFull)
				sprintf(retFull, "function %s(scene)--(scene=Map3D)%s", retFunc, utf8("当初始化场景成功后被调用"));
			return true;
		case SceneEventID_Open:
			//if(name)
			//	sprintf(retFunc, "%s_onscene_open", name);
			//else
				sprintf(retFunc, "onscene_open");
			if(retFull)
				sprintf(retFull, "function %s(scene)--(scene=Map3D)%s", retFunc, utf8("当打开场景时被调用"));
			return true;
		case SceneEventID_Click:
			//if(name)
			//	sprintf(retFunc, "%s_onscene_click", name);
			//else
				sprintf(retFunc, "onscene_click");
			if(retFull)
				sprintf(retFull, "function %s(scene,sel)--(scene=Map3D,sel=GameObject)%s", retFunc, utf8("当单击场景时被调用，如果有对象被点击则返回到sel游戏对象，否则sel=nil"));
			return true;
		case SceneEventID_ClickTerrain:
			//if(name)
			//	sprintf(retFunc, "%s_onscene_terrain", name);
			//else
				sprintf(retFunc, "onscene_terrain");
			if(retFull)
				sprintf(retFull, "function %s(scene,pos)--(scene=Map3D,pos=float3)%s", retFunc, utf8("当单击场景时被调用，如果有仅地形被点击则返回这个消息"));
			return true;
		case SceneEventID_ClickTerrain2:
			//if(name)
			//	sprintf(retFunc, "%s_onscene_terrain2", name);
			//else
				sprintf(retFunc, "onscene_terrain2");
			if(retFull)
				sprintf(retFull, "function %s(scene,pos)--(scene=Map3D,pos=float3)%s", retFunc, utf8("当第二个手指单击场景时被调用，如果有仅地形被点击则返回这个消息"));
			return true;
		case SceneEventID_IdleOver:
			//if(name)
			//	sprintf(retFunc, "%s_onscene_idleover", name);
			//else
				sprintf(retFunc, "onscene_idleover");
			if(retFull)
				sprintf(retFull, "function %s(scene,obj)--(scene=Map3D,obj=GameObject)%s", retFunc, utf8("剧情内对象休闲时间结束时调用"));
			return true;
		case SceneEventID_MoveOver:
			//if(name)
			//	sprintf(retFunc, "%s_onscene_moveover", name);
			//else
				sprintf(retFunc, "onscene_moveover");
			if(retFull)
				sprintf(retFull, "function %s(scene,obj)--(scene=Map3D,obj=GameObject)%s", retFunc, utf8("剧情内对象移动结束时调用"));
			return true;
		case SceneEventID_Next:
			//if(name)
			//	sprintf(retFunc, "%s_onscene_next", name);
			//else
				sprintf(retFunc, "onscene_next");
			if(retFull)
				sprintf(retFull, "function %s(scene,id)--(scene=Map3D,id=int)%s", retFunc, utf8("进入下一个剧情，按照id来区分,初始化完毕之后会调用id为1的脚本"));
			return true;
		case SceneEventID_Finish:
			sprintf(retFunc, "onscene_finish");
			if(retFull)
				sprintf(retFull, "function %s(scene)--(scene=Map3D)%s", retFunc, utf8("结束整个剧情"));
			return true;
		case SceneEventID_EnterRegion:
				sprintf(retFunc, "onscene_region");
			if(retFull)
				sprintf(retFull, "function %s(scene,region)--(scene=Map3D,region=region_table)%s", retFunc, utf8("游戏主角进入某个地区的时候被调用"));
			return true;
		}
		return false;
	}

	BOOL			AIScript::GetEventName(char* retFunc, char* retFull, int id, BOOL bName)//根据场景名字进行修订
	{CPUTime(AIScript);
		const char* name = 0;
		if(bName)name = m_name.t;
		char retname[128];
		if(!retFunc)retFunc = retname;
		switch(id){
		case AIEvent_OnBorn:
			if(name)
				sprintf(retFunc, "%s_onai_born", name);
			else
				sprintf(retFunc, "onai_born");
			if(retFull)
				sprintf(retFull, "function %s(ai,obj)--(ai=ai_table,obj=GameObject)%s", retFunc, utf8("游戏对象被创建时调用"));
			return true;
		case AIEvent_OnIdleOver:
			if(name)
				sprintf(retFunc, "%s_onai_idleover", name);
			else
				sprintf(retFunc, "onai_idleover");
			if(retFull)
				sprintf(retFull, "function %s(ai,obj)--(ai=ai_table,obj=GameObject)%s", retFunc, utf8("游戏对象休闲时间结束时调用"));
			return true;
		case AIEvent_OnAttackOver:
			if(name)
				sprintf(retFunc, "%s_onai_attackover", name);
			else
				sprintf(retFunc, "onai_attackover");
			if(retFull)
				sprintf(retFull, "function %s(ai,obj)--(ai=ai_table,obj=GameObject)%s", retFunc, utf8("游戏对象攻击结束时调用"));
			return true;
		case AIEvent_OnMoveOver:
			if(name)
				sprintf(retFunc, "%s_onai_moveover", name);
			else
				sprintf(retFunc, "onai_moveover");
			if(retFull)
				sprintf(retFull, "function %s(ai,obj)--(ai=ai_table,obj=GameObject)%s", retFunc, utf8("游戏对象移动结束时调用"));
			return true;
		case AIEvent_OnDieOver:
			if(name)
				sprintf(retFunc, "%s_onai_dieover", name);
			else
				sprintf(retFunc, "onai_dieover");
			if(retFull)
				sprintf(retFull, "function %s(ai,obj)--(ai=ai_table,obj=GameObject)%s", retFunc, utf8("游戏对象死亡结束时调用"));
			return true;
		case AIEvent_OnByAttack:
			if(name)
				sprintf(retFunc, "%s_onai_byattack", name);
			else
				sprintf(retFunc, "onai_byattack");
			if(retFull)
				sprintf(retFull, "function %s(ai,obj,target)--(ai=ai_table,obj=GameObject,target=GameObject)%s", retFunc, utf8("游戏对象被击的时候被调用"));
			return true;
		case AIEvent_OnEnemyIn:
			if(name)
				sprintf(retFunc, "%s_onai_enemyin", name);
			else
				sprintf(retFunc, "onai_enemyin");
			if(retFull)
				sprintf(retFull, "function %s(ai,obj,target)--(ai=ai_table,obj=GameObject,target=GameObject)%s", retFunc, utf8("敌人进入主动攻击范围的时候被调用,也就是怪物可攻击的距离"));
			return true;
		case AIEvent_OnTargetMissing:
			if(name)
				sprintf(retFunc, "%s_onai_targetMissing", name);
			else
				sprintf(retFunc, "onai_targetMissing");
			if(retFull)
				sprintf(retFull, "function %s(ai,obj)--(ai=ai_table,obj=GameObject)%s", retFunc, utf8("当前目标敌人超出最大可攻击或者目标丢失的时候被调用"));
			return true;
		case AIEvent_OnKillEnemy:
			if(name)
				sprintf(retFunc, "%s_onai_killEnemy", name);
			else
				sprintf(retFunc, "onai_killEnemy");
			if(retFull)
				sprintf(retFull, "function %s(ai,obj)--(ai=ai_table,obj=GameObject)%s", retFunc, utf8("游戏对象打死目标敌人的时候被调用"));
			return true;
		case AIEvent_OnNextBegin:
			if(name)
				sprintf(retFunc, "%s_onai_next", name);
			else
				sprintf(retFunc, "onai_next");
			if(retFull)
				sprintf(retFull, "function %s(ai,obj,nid)--(ai=ai_table,obj=GameObject,nid=int)%s", retFunc, utf8("游戏对象下一个动作执行前被调用,nid是之前设置好的自定义ID,next_setid"));
			return true;
		case AIEvent_OnActionEvent:
			if(name)
				sprintf(retFunc, "%s_onai_event", name);
			else
				sprintf(retFunc, "onai_event");
			if(retFull)
				sprintf(retFull, "function %s(ai,obj,atype)--(ai=ai_table,obj=GameObject,atype=ActionType)%s", retFunc, utf8("游戏对象的动作事件触发时被调用"));
			return true;
		case AIEvent_OnActionOver:
			if(name)
				sprintf(retFunc, "%s_onai_actionover", name);
			else
				sprintf(retFunc, "onai_actionover");
			if(retFull)
				sprintf(retFull, "function %s(ai,obj,atype)--(ai=ai_table,obj=GameObject,atype=ActionType)%s", retFunc, utf8("游戏对象的动作结束事件触发时被调用"));
			return true;
		}
		return false;
	}
	int				AIScript::GetEventCount()
	{CPUTime(AIScript);
		return AIEvent_Count;
	}
	BOOL		SkillScript::GetEventName(char* retFunc, char* retFull, int id, BOOL bName)//根据场景名字进行修订
	{CPUTime(SkillScript);
		const char* name = 0;
		if(bName)name = m_name.t;
		char retname[128];
		if(!retFunc)retFunc = retname;
		switch(id){
		case SkillEvent_Hit:
			if(name)
				sprintf(retFunc, "%s_onskill_hit", name);
			else
				sprintf(retFunc, "onskill_hit");
			if(retFull)
				sprintf(retFull, "function %s(skill,obj)--(skill=skill_table,obj=GameObject)%s", retFunc, utf8("对目标使用某个技能成功时被调用,裁决伤害"));
			return true;
		//case SkillEvent_DoSkillPos:
		//	if(name)
		//		sprintf(retFunc, "%s_onskill_range", name);
		//	else
		//		sprintf(retFunc, "onskill_range");
		//	if(retFull)
		//		sprintf(retFull, "function %s(skill,obj,pos)--(skill=skill_table,obj=GameObject,pos=float3)%s", retFunc, utf8("对目标店使用某个技能时被调用"));
		//	return true;
		//case SkillEvent_DoHit:
		//	if(name)
		//		sprintf(retFunc, "%s_onskill_hit", name);
		//	else
		//		sprintf(retFunc, "onskill_hit");
		//	if(retFull)
		//		sprintf(retFull, "function %s(skill,obj,target)--(skill=skill_table,obj=GameObject,target=GameObject)%s", retFunc, utf8("对一个目标进行伤害裁决,如果是群体技能则会多次被调用"));
		//	return true;
		}
		return false;
	}
	int			SkillScript::GetEventCount()
	{CPUTime(SkillScript);
		return SkillEvent_Count;
	}
	ScriptBase*	ScriptBase::CreateNew(ScriptBase::eScript s)
	{CPUTime(ScriptBase);
		switch(s){
		case eScript_AI:
			return new AIScript();
		case eScript_Skill:
			return new SkillScript();
		case eScript_Scene:
			return new SceneScript();
		}
		assert(false);
		return 0;
	}

};
