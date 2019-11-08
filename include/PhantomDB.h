
#ifndef __PHANTOM_DB_H__
#define	__PHANTOM_DB_H__
#include "sqlite3.h"
#include "PhantomBase.h"
#include "PhantomFile.h"

namespace Phantom{
	class	Config;
	struct	ConfigHeader
	{
		Name		name;
		NameT<32>	type_s;
		Text		desc;//说明
		unsigned short	listcount;//如果是数组则有效，数组长度
		Config*		classPtr;//如果是class则需要这个项
		VOID		SetName(const char* n);
		VOID		ResetType();
		enum type_num{
			type_int	=	1,
			type_float	=	2,
			type_text	=	3,//
			type_class	=	4,//其他表中的某个行
			type_boolean=	5,
		};
		int		type;
		ConfigHeader(){
			classPtr = 0;
			type = 0;
			listcount = 0;
		}
		~ConfigHeader(){
			desc.clear();
		}
	};
#pragma pack(push,1)
	struct	ConfigVal{
		float		fval;
		int			ival;
		char*		text;//文本指针
		ConfigVal*	list;//如果是数组则旗下有count-1个数据
		//
		const char*	getText();
		void		setText(const char* t);
		void		freeText();
		void	operator = (const ConfigVal& v);
		ConfigVal(){fval = 0; ival = 0; text = 0; list = 0;}
		~ConfigVal();
	};
#pragma pack(pop)

	const char*			ReadText(CFileHelperR& r, char* buf);
	struct DBColumn
	{
		DBColumn(const char* szName){name = szName; index = -1;}
		Name		name;
		int			index;
	};

	class	Sqlite3
	{
	public:
		Sqlite3();
		virtual ~Sqlite3();
	public:
		BOOL			OpenDB(const char* szFile);		//打开数据库文件
		VOID			CloseDB();
		sqlite3_stmt*	Select(const char* szExec);	//选择一个数据库命令
		BOOL			MoveNext(sqlite3_stmt* stmt);//如果返回true表示可以继续循环
		INT				ReadInt(sqlite3_stmt* stmt, DBColumn& col);
		double			ReadDouble(sqlite3_stmt* stmt, DBColumn& col);
		const char*		ReadText(sqlite3_stmt* stmt, DBColumn& col);
		VOID			Execute(const char* szExec, BOOL bTransMode = TRUE/*如果Transmode则使用一次性传输操作*/);//执行并不返回数据
	protected:
		BOOL			InitColumn(sqlite3_stmt* stmt, DBColumn& col);//自动初始化列
		BOOL			ExecuteDB(const char* szExec);
	protected:
		sqlite3*	m_sqlite;
	};

	struct	ID
	{
		unsigned int id;
		int index;
		ID(){id = 0; index = -1;}
	};
	template <typename type>
	class DBManager
	{
	public:
		DBManager(){}
		~DBManager(){}
		inline	int			size(){return m_list.size();}
		inline	type&		operator[](int index){return m_list[index];}
		type*				operator[](ID& id)
		{
			if(id.index == -2)//如果从来找不到
				return 0;
			if(id.index >= 0 && id.index < m_list.size() && id.id == m_list[id.index].id)
				return &m_list[id.index];
			for(int i=0;i<m_list.size();i++)
			{
				if(m_list[i].id == id.id)
				{
					id.index = i;
					return &m_list[i];
				}
			}
			id.index = -2;
			return 0;
		}
		type*				Find(unsigned int id)//根据ID查找
		{
			for(int i=0;i<m_list.size();i++)
			{
				if(m_list[i].id == id)
					return &m_list[i];
			}
			return 0;
		}
		unsigned int		Insert(type& t)//新建一个并返回ID
		{
			int idMax = 0;
			for(int i=0;i<m_list.size();i++)
			{
				idMax = getmax(idMax, m_list[i].id);
			}
			t.id = idMax;
			m_list.push_back(&t);
			//OnInsert(t);
			return idMax;
		}
		BOOL				Remove(unsigned int id)//新建一个并返回ID
		{
			for(int i=0;i<m_list.size();i++)
			{
				if(m_list[i].id == id)
				{
					m_list.eraseByIndex(i);
					//OnRemove(id);
					return true;
				}
			}
			return false;
		}
	//protected:
	//	virtual	void		OnInsert(type& t) = 0;
	//	virtual	void		OnRemove(unsigned int id) = 0;
		CDynamicArray<type>	m_list;
	};
#pragma pack(push, 1)
	struct	StateValue
	{
		char			isNumber;//
		double			value;
		char*			ptr;
		void			setText(const char* t);
		const char*		getText();
		void			freeText();
		//
		StateValue();
		~StateValue();
	};
	struct	NameState: StateValue
	{
		NameT<31>		name;//最多15个中文文本,最后一个是0
	};
#pragma pack(pop)
	struct	GenuineGUID;
	class GameState: public Base
	{
	public:
		GameState();
		~GameState();
		const char*					readt(const char* name, const char* szDefault = "");
		double						readf(const char* name, double dDefault = 0);
		int							readi(const char* name, int nDefault = 0);
		void						write(const char* name, const char* text);
		void						writef(const char* name, double d);
		void						writei(const char* name, int n);
		//
		int							find(const char* name);
		//
		const char*					lreadt(int index, const char* szDefault = "");
		double						lreadf(int index, double dDefault = 0);
		int							lreadi(int index, int nDefault = 0);
		void						lwritet(int index, const char* text);
		void						lwritef(int index, double d);
		void						lwritei(int index, int n);
		void						SetArrayCount(int cnt);
		//
		GameState*					addChild(const char* name);//返回子项，如果找不到会自动创建
		GameState*					searchChild(const char* name);//返回子项
		void						removeChild(GameState* s);
		void						removeChild(const char* name);
		VOID						Destroy();
		VOID						initState(const char* szFile);
		void						OnChanged();
		//
		void						save(streamWrite& w);
		bool						load(streamRead& r);
		void						save(LPCSTR szFile, GenuineGUID* guid);
		bool						load(LPCSTR szFile, GenuineGUID* guid);
	public:
		char							m_bCanSave;//是否可以被保存
		NameT<32>						m_name;
		CDynamicArrayS<GameState*,5>	m_childs;//子项
		CDynamicArrayS<NameState,5>		m_states;
		CDynamicArrayS<StateValue,1>	m_arrays;
		//
		static NameT<32>&				GetTemp();
	};
	VOID							reloadState();
    VOID							saveStateNow();
	GameState*						GetStateMgr();//
	void							initState(const char* szFile, unsigned int* guid, int length);//guid用于加密
	void							saveState(const char* szFile);//guid用于加密
	bool							loadState(const char* szFile);
	void							EncryptData(CDynamicArray<char>& rets, const char* ptr, int length);
	char*							DecryptData(char* inptr, int length, int& realsize);
	//

	class	ConfigMgr;
	class	Config;
	class	ScriptBase;
	class	ConfigLine{
	public:
		ConfigLine();
		virtual ~ConfigLine();
		//如果index>0则从数组中搜索，如果不存在数组则从第一个数据获得
		int						GetIntVal(int col, int index = 0);
		void					SetIntVal(int col, int val, int index = 0);
		float					GetDoubleVal(int col, int index = 0);
		void					SetDoubleVal(int col, float val, int index = 0);
		const char*				GetTextVal(int col, int index = 0);
		void					SetTextVal(int col, const char* text, int index = 0);

		int						GetIntVal(GameID& id, const char* name);
		float					GetDoubleVal(GameID& id, const char* name);
		const char*				GetTextVal(GameID& id, const char* name);
		ConfigLine*				GetOtherLinePtr(GameID& id, const char* name);//如果是指向其他的类,返回其他类对应的行,没有则返回0
		BOOL					SearchID(GameID& id, const char* name);

		ConfigLine*				GetOtherLinePtr(int col, int index = 0);//如果是指向其他的类,返回其他类对应的行,没有则返回0
		VOID					SetOtherLinePtr(int col, ConfigLine* line, int index = 0);
		VOID					SetOtherLineID(int col, int otherid, int index = 0);
		int						GetOtherLineID(int col, int index = 0);//返回id
		//
		inline	bool			GetVal(int col, int index, bool& b){return (GetIntVal(col, index)!=0);}
		inline	int				GetVal(int col, int index, int& b){return GetIntVal(col, index);}
		inline	float			GetVal(int col, int index, float& b){return (float)GetDoubleVal(col, index);}
		inline	const char*		GetVal(int col, int index, const char*& b){return GetTextVal(col, index);}
		inline	ConfigLine*		GetVal(int col, int index, ConfigLine*& b){return GetOtherLinePtr(col, index);}
		inline	void			SetVal(int col, int index, bool& b){SetIntVal(col, b?1:0, index);}
		inline	void			SetVal(int col, int index, int& b){SetIntVal(col, b, index);}
		inline	void			SetVal(int col, int index, float& b){SetDoubleVal(col, b, index);}
		inline	void			SetVal(int col, int index, const char*& b){SetTextVal(col, b, index);}
		inline	void			SetVal(int col, int index, ConfigLine*& b){SetOtherLinePtr(col, b, index);}
		//
		VOID					Setup(Config* c);
		int						SearchCol(const char* columnName);
		const char*				GetTableName();
		const char*				GetLineName();
		VOID					SetLineName(const char* name);
		int						GetLineID();
		VOID					SetLineID(int id);
		inline	const GameID&	GetID(){return m_id;}
		Config*					GetConfigPtr(){return m_config;}
		ConfigLine*				Clone(ConfigMgr* mgr);//克隆一个
		inline	int				GetLineIndex(){return m_index;}
		inline	ScriptBase*		GetScriptPtr(){return m_script;}
	protected:
		friend class			Config;
		friend class			ConfigMgr;
		friend class			GameMgr;
		GameID					m_id;
		int						m_index;
		ConfigVal*				m_data;//行数据
		Config*					m_config;
		ScriptBase*				m_script;//对应的脚本指针,如果存在脚本的话
	};
	class	Config
	{
	public:
		Config();
		~Config();
		int						GetLineCount();
		ConfigLine*				GetLine(int lineIndex);
		int						SearchCol(const char* columnName);
		const char*				GetTableName();
		BOOL					IsExistLine(int id);//是否存在这个ID
		ConfigLine*				GetLineFromID(int id);//返回lineindex
		inline	int				GetNumCol(){return m_numCol;}
		inline	ConfigHeader*	GetCol(int col){if(col<0||col>=m_numCol)return 0;return &m_cols[col];}
		inline	int				GetConfigIndex(){return m_index;}
		Name					m_tableName;//表格名字
		//
		VOID					RebuildListIndexs();
		int						GetArrayCol(int listIndex);//返回数组索引中的列索引
		int						GetArrayCount(){return m_arrayCount;}
	protected://第一个肯定是id,第二个肯定是name
		friend class			ConfigMgr;
		friend class			ConfigLine;
		friend class			GameMgr;
		BOOL					Load(ConfigMgr* mgr, const char* szFileName);
		int						m_id;
		int						m_index;
		ConfigHeader*			m_cols;//表头数据
		unsigned short*			m_arrayIndex;//数组索引列表
		unsigned short			m_arrayCount;//数组数量，一个表格中的数组变量不能超过10个
		ConfigLine*				m_lines;//行数据,i,f
		int						m_numLine,m_numCol;
	};
#define	MAX_CONFIG_FILE			20//最多支持20个配置文件+一个role_table
	class	ConfigMgr
	{
	public:
		ConfigMgr();
		~ConfigMgr();
		Config*						SearchConfig(const char* szTableName);//搜索表格
		Config*						GetConfigPtr(PtrID& id, const char* szTableName);
		BOOL						ResearchAllFiles();//搜索文件中的所有表格并重新载入
		VOID						Destroy();
		//
		VOID						RegisterAllLuaClass();//注册所有lua变量列表,每次初始化脚本都需要调用一次,最多支持52个表格的定义和lua注册
		ConfigLine*					SearchLine(GameID& id);
		ConfigLine*					SearchLineC(const GameID& id);
		//
		GameID						GetLineOther(GameID& id, int col, int index = 0);//返回这个行的指向行
		VOID						SetLineOther(GameID& id, int col, GameID& otherID, int index = 0);
		VOID						SetLineOtherC(GameID& id, int col, const GameID& otherID, int index = 0);
		//
		inline Config*				GetRoleTable(){return m_tblRoles;}
		inline	int					GetNewIdentity(){return m_id++;}
		VOID						OnDeleteLine(ConfigLine* l);
	public:
		CDynamicArray<Config*>		m_configLua;
		enum{
			Table_AI = 0,
			Table_Scene = 1,
			Table_City = 2,
			Table_Mission = 3,
			Table_Skill = 4,
		};
	protected:
		CDynamicArray<Config*>		m_configs;//配置文件列表，没有初始化
		CDynamicArray<ConfigLine*>	m_lines;//所有行数据,包括固定行和动态行
		//
		friend class				Config;
		int							m_id;
	public:
		Config*						m_tblRoles;//角色表格
		Config*						m_tblAI;//AI表格
		Config*						m_tblScene;//场景列表
		Config*						m_tblCity;//城市列表
		Config*						m_tblMission;//任务列表
		Config*						m_tblSkill;//技能列表
		Config*						m_tblLeader;//主角设定表格
		Config*						m_tblText;//文本列表
		Config*						m_tblSceneUnit;//场景单位
		char						m_colTextDesc;//文本列表说明
	};
	void						enum_files(const char* path,CDynamicArray<FileName>& _out, int subFolderLevel, NameT<16>* szFindType,int numFindTypes);
};

#endif
