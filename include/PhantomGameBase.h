//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   PhantomGame.h
软件名称   :   幻影游戏引擎

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___PHANTOMGAMEBASE__H___
#define ___PHANTOMGAMEBASE__H___

#include "PhantomBase.h"
#include "PhantomDB.h"

#define	MaxEvent_Count	20

namespace Phantom{

#pragma pack(push,1)
	template <int Tindex> class lua_GameTable;
	class	GameTableBase
	{
	public:
		GameID			m_id;
		bool			isnull(){return (m_id.id==0);}
		//
		GameTableBase(){}
		int				GetID();
		VOID			SetID(int id);
		const char*		GetName();
		VOID			SetName(const char* name);
		//
		template <typename Tclass>	Tclass	GetT(int id);
		template <typename Tclass>	void	SetT(Tclass val, int id);
		template <int Tother>	lua_GameTable<Tother>	GetClass(int id);
		template <int Tother>	 void				SetClass(const lua_GameTable<Tother>& other, int id);
		//
		template <int Tindex>					int				T_Count();
		template <int Tindex, typename Tclass>	Tclass			T_Get(int id);
		template <int Tindex, typename Tclass>	void			T_Set(int id, Tclass val);
		template <int Tindex,int Tother>	lua_GameTable<Tother>	T_GetClass(int id);
		template <int Tindex,int Tother>	 void				T_SetClass(int id, const lua_GameTable<Tother>& other);
		template <typename Tclass>
		Tclass			T_GetVal(int index, int Tindex);
		template <typename Tclass>
		void			T_SetVal(Tclass val, int index, int Tindex);
		int				T_ArrayCount(int Tindex);
		ConfigLine*		GetClassLine(int index, int Tindex);
		VOID			SetClassLine(int index, int Tindex, const GameID& other);
	};
#pragma pack(pop)
template <int Tindex>
	class lua_GameTable: public GameTableBase{
	public:
		lua_GameTable(int _id);
		lua_GameTable(const GameID& id){m_id = id;}
		lua_GameTable(){}
		~lua_GameTable(){}
		static const int Index = Tindex;
		static Config* s_config;
		static void	RegisterLua(Config* cfg);
	};

	class	ScriptBase{
	public:
		enum eScript{
			eScript_AI	=	1,//
			eScript_Skill,
			eScript_Scene,
			eScript_Plot,//剧情系统脚本
		};
		ScriptBase(){memset(m_func, 0, sizeof(m_func));m_fileIndex=-1;}
		virtual ~ScriptBase(){}
		BOOL				IsExist(int id){if(id<0||id>=MaxEvent_Count)return false; return m_func[id];}//事件是否存在
		virtual	eScript		GetScriptType() = 0;
		static ScriptBase*	CreateNew(eScript s);
	public:
		FileName			m_file;//脚本文件
		NameT<32>			m_name;//脚本开头名字
		char				m_func[MaxEvent_Count];
		int					m_fileIndex;
	public:
		virtual	BOOL		GetEventName(char* retFunc, char* retFull, int id, BOOL bName) = 0;//根据场景名字进行修订
		virtual	int			GetEventCount() = 0;
	};
	//AI事件类
	class	AIScript: public ScriptBase{
	public:
		virtual	eScript		GetScriptType(){return eScript_AI;}
		virtual	BOOL		GetEventName(char* retFunc, char* retFull, int id, BOOL bName);//根据场景名字进行修订
		virtual	int			GetEventCount();
	};
	//游戏技能类,脚本+表格驱动
	class	SkillScript: public ScriptBase{
	public:
		virtual	eScript		GetScriptType(){return eScript_Skill;}
	public:
		virtual	BOOL		GetEventName(char* retFunc, char* retFull, int id, BOOL bName);//根据场景名字进行修订
		virtual	int			GetEventCount();
	};

	//游戏任务类,脚本+表格驱动
	class	MissionScript{
	public:
	};

	//游戏特效类,脚本+表格驱动
	class	EffectScript{
	public:
	};
	class	SceneScript: public ScriptBase
	{
	public:
		SceneScript(){}
		virtual ~SceneScript(){}
		virtual	eScript		GetScriptType(){return eScript_Scene;}
	public:
		virtual	BOOL		GetEventName(char* retFunc, char* retFull, int id, BOOL bName);//根据场景名字进行修订
		virtual	int			GetEventCount();
	};
};

#endif
