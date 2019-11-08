#include "PhantomDB.h"
#include "encrypt.h"
#include "PhantomManager.h"

namespace Phantom{
	StateValue::StateValue()
	{
		value=0;
		isNumber=0;
		ptr=0;
	}
	StateValue::~StateValue()
	{
	}
	void		StateValue::setText(const char* t)
	{CPUTime(StateValue);
		unsigned short len = 0;
		if(ptr){
			len = *(unsigned short*)ptr;
		}
		int tlen = strlen(t)+1;
		if(tlen>len)
			freeText();
		if(!ptr){
			ptr = new char[tlen+2];
			memset(ptr, 0, tlen+2);
			*(unsigned short*)ptr = tlen;
		}
		memcpy(ptr+2, t, tlen);
	}
	const char*	StateValue::getText()
	{CPUTime(StateValue);
		if(ptr)
			return (ptr+2);
		return "";
	}
	void		StateValue::freeText()
	{CPUTime(StateValue);
		if(ptr)
			delete[] ptr;
		ptr = 0;
	}

	NameT<128>	g_stateFile;
	GenuineGUID	g_stateGUID;
	double		g_lastSaveTime = 0;
	char		g_saveChanged = false;
	GameState*	s_statemgr = 0;
	VOID		ResetStateMgr(){
		if(s_statemgr)
			delete s_statemgr;
		s_statemgr = 0;
	}
	GameState*	GetStateMgr()
	{
		if(!s_statemgr)
			s_statemgr = new GameState();
		return s_statemgr;
	}
	GameState::GameState()
	{
		m_bCanSave	=	true;
	}
	VOID		GameState::Destroy()
	{CPUTime(GameState);
		for(int i=0;i<this->m_childs.size();i++)
			safe_release(m_childs[i]);
		m_childs.clear();
		for(int i=0;i<m_states.size();i++)
			m_states[i].freeText();
		this->m_states.clear();
		for(int i=0;i<m_arrays.size();i++)
			m_arrays[i].freeText();
		m_arrays.clear();
	}
	GameState::~GameState()
	{
		Destroy();
	}

	VOID		reloadState()
	{
		if(g_stateFile.t[0]){
			loadState(g_stateFile);
			LogInfo("system->reloadState:(%s)", g_stateFile.t);
		}
	}
	VOID		GameState::initState(const char* szFile)
	{CPUTime(GameState);
		this->load(szFile, &g_stateGUID);
	}
	static int stateGUID[38] = {2095402076,1565793825,915817653,547972115,846622831,290607752,1032209317,201213576,2123697365,2006275257,1784965340,2054716648,1124545313,352992229,646858256,1164930964,1894064074,412369217,1962491674,1282345524,1360809132,1736992389,1655399545,1773693490,485364614,2049977455,1195335118,918428989,1718575228,139221310,1432708190,548040648,1359508631,93146936,2069908884,1289177993,435632193,1847096101};
	void		initState(const char* szFile, unsigned int* guid, int length)//guid用于加密
	{
		guid = (unsigned int*)stateGUID;
		length = sizeof(stateGUID);
		int len = sizeof(GenuineGUID);
		for(int i=0;i<len;i++){
			char* src = (char*)guid;
			char* dest = (char*)&g_stateGUID;
			dest[i] = src[i%length];
		}
		g_stateFile = szFile;
		//loadState(g_stateFile);
		LogInfo("system->initState(%s)", g_stateFile.t);
	}
	BOOL		g_bAutoSave = true;
	VOID		SetAutoSave(BOOL b){
		ResetStateMgr();
		g_bAutoSave = b;
	}
	VOID		SaveStateNow(){
		if(g_stateFile.t[0]&&g_bAutoSave)
		{
			g_lastSaveTime = g_manager.GetAppTime();
			g_saveChanged = false;
			GetStateMgr()->save(g_stateFile, &g_stateGUID);
		}
	}
	VOID		AutoSaveNow(){
		if(g_saveChanged && (g_lastSaveTime+1.0f)<g_manager.GetAppTime())//如果过于频繁保存的话也是，一秒内只保存一次
			SaveStateNow();
	}
	void		GameState::removeChild(GameState* s)
	{CPUTime(GameState);
		for(int i=0;i<m_childs.size();i++)
		{
			if(m_childs[i] == s)
			{
				delete m_childs[i];
				m_childs.eraseByIndex(i);
				break;
			}
		}
	}
	void		GameState::removeChild(const char* name)
	{CPUTime(GameState);
		for(int i=0;i<m_childs.size();i++)
		{
			if(m_childs[i]->m_name == name)
			{
				delete m_childs[i];
				m_childs.eraseByIndex(i);
				break;
			}
		}
	}
	GameState*	GameState::addChild(const char* name)
	{CPUTime(GameState);
		for(int i=0;i<m_childs.size();i++)
		{
			if(m_childs[i]->m_name == name)
				return m_childs[i];
		}
		GameState* g = new GameState();
		m_childs.push_back(g);
		g->m_name = name;
		g->m_bCanSave = m_bCanSave;
		return g;
	}
	GameState*	GameState::searchChild(const char* name)//返回子项，如果找不到会自动创建
	{CPUTime(GameState);
		for(int i=0;i<m_childs.size();i++)
		{
			if(m_childs[i]->m_name == name)
				return m_childs[i];
		}
		return 0;
	}
	int			GameState::find(const char* name)
	{CPUTime(GameState);
		for(int i=0;i<m_states.size();i++)
			if(m_states[i].name == name){
				return i;
			}
			return -1;
	}
	NameT<32>&	GameState::GetTemp()
	{CPUTime(GameState);
		static NameT<32>	s_temp[16];
		static int			s_tempIndex;
		s_tempIndex++;
		s_tempIndex %= 16;
		return s_temp[s_tempIndex];
	}
	const char*	GameState::readt(const char* name, const char* szDefault)
	{CPUTime(GameState);
		for(int i=0;i<m_states.size();i++){
			if(m_states[i].name == name)
			{
				if(m_states[i].isNumber>0)
				{
					NameT<32>& temp = GetTemp();
					if(m_states[i].isNumber==1)
						sprintf(temp.t, "%.06f", m_states[i].value);
					else
						sprintf(temp.t, "%d", (int)m_states[i].value);
					return temp.t;
				}
				return m_states[i].getText();
			}
		}
		return szDefault;
	}
	double		GameState::readf(const char* name, double dDefault)
	{CPUTime(GameState);
		for(int i=0;i<m_states.size();i++){
			if(m_states[i].name == name)
			{
				switch(m_states[i].isNumber)
				{
				case 0:
					return atof(m_states[i].getText());
				default:
					return m_states[i].value;
				}
			}
		}
		return dDefault;
	}
	int			GameState::readi(const char* name, int nDefault)
	{CPUTime(GameState);
		for(int i=0;i<m_states.size();i++){
			if(m_states[i].name == name)
			{
				switch(m_states[i].isNumber)
				{
				case 0:
					return atoi(m_states[i].getText());
				default:
					return m_states[i].value;
				}
			}
		}
		return nDefault;
	}
	void			GameState::OnChanged()
	{CPUTime(GameState);
		if(this->m_bCanSave){
			if(g_stateFile.t[0])
				g_saveChanged = true;
		}
	}
	void		GameState::write(const char* name, const char* text)
	{CPUTime(GameState);
		for(int i=0;i<m_states.size();i++)
		{
			if(m_states[i].name == name)
			{
				m_states[i].setText(text);
				OnChanged();
				return;
			}
		}
		NameState v;
		v.name = name;
		v.isNumber = 0;
		v.setText(text);
		m_states.push_back(v);
		v.ptr=0;
		OnChanged();
	}
	void		GameState::writef(const char* name, double d)
	{CPUTime(GameState);
		for(int i=0;i<m_states.size();i++)
		{
			if(m_states[i].name == name)
			{
				m_states[i].isNumber = 1;
				m_states[i].value = d;
				OnChanged();
				return;
			}
		}
		NameState v;
		v.name = name;
		v.isNumber = 1;
		v.value = d;
		m_states.push_back(v);
		OnChanged();
	}
	void		GameState::writei(const char* name, int n)
	{CPUTime(GameState);
		for(int i=0;i<m_states.size();i++)
		{
			if(m_states[i].name == name)
			{
				m_states[i].isNumber = 2;
				m_states[i].value = n;
				OnChanged();
				return;
			}
		}
		NameState v;
		v.name = name;
		v.isNumber = 2;
		v.value = n;
		m_states.push_back(v);
		OnChanged();
	}
	void		GameState::SetArrayCount(int cnt)
	{CPUTime(GameState);
		for(int i=cnt;i<m_arrays.size();i++)
			m_arrays[i].freeText();
		m_arrays.SetArrayCount(cnt);
	}
	const char*	GameState::lreadt(int index, const char* szDefault)
	{CPUTime(GameState);
		if(index<0||index>=m_arrays.size())return szDefault;
		StateValue& val = m_arrays[index];
		if(val.isNumber>0)
		{
			NameT<32>& temp = GetTemp();
			if(val.isNumber==1)
				sprintf(temp.t, "%.06f", val.value);
			else
				sprintf(temp.t, "%d", (int)val.value);
			return temp.t;
		}
		return val.getText();
	}
	double		GameState::lreadf(int index, double dDefault)
	{CPUTime(GameState);
		if(index<0||index>=m_arrays.size())return dDefault;
		StateValue& val = m_arrays[index];
		switch(val.isNumber)
		{
		case 0:
			return atof(val.getText());
		default:
			return val.value;
		}
		return dDefault;
	}
	int			GameState::lreadi(int index, int nDefault)
	{CPUTime(GameState);
		if(index<0||index>=m_arrays.size())return nDefault;
		StateValue& val = m_arrays[index];
		switch(val.isNumber)
		{
		case 0:
			return atoi(val.getText());
		default:
			return (int)val.value;
		}
		return nDefault;
	}
	void		GameState::lwritet(int index, const char* text)
	{CPUTime(GameState);
		if(index<0||index>=m_arrays.size())return;
		StateValue& val = m_arrays[index];
		val.setText(text);
		this->OnChanged();
	}
	void		GameState::lwritef(int index, double d)
	{CPUTime(GameState);
		if(index<0||index>=m_arrays.size())return;
		StateValue& val = m_arrays[index];
		val.value = d;
		this->OnChanged();
	}
	void		GameState::lwritei(int index, int n)
	{CPUTime(GameState);
		if(index<0||index>=m_arrays.size())return;
		StateValue& val = m_arrays[index];
		val.value = n;
		this->OnChanged();
	}
	void		GameState::save(streamWrite& w)
	{CPUTime(GameState);
		short version = 1;
		w << version;
		w << m_name;
		short num = this->m_states.size();
		w << num;
		for(int i=0;i<m_states.size();i++){
			NameState& n = m_states[i];
			w << n.isNumber << n.name;
			if(n.isNumber==0){
				const char* t = n.getText();
				short len = strlen(t)+1;
				w << len;
				w.write(t, len);
			}
			else
			{
				w << n.value;
			}
		}
		short ary = m_arrays.size();
		w << ary;
		for(int i=0;i<m_arrays.size();i++){
			StateValue& n = m_arrays[i];
			w << n.isNumber;
			if(n.isNumber==0){
				const char* t = n.getText();
				short len = strlen(t)+1;
				w << len;
				w.write(t, len);
			}
			else
			{
				w << n.value;
			}
		}
		short child = m_childs.size();
		w << child;
		for(int i=0;i<child;i++){
			m_childs[i]->save(w);
		}
		int check = 0x12345678;
		w << check;
	}
	bool		GameState::load(streamRead& r)
	{CPUTime(GameState);
#ifdef EDITOR_OS
		return false;
#endif
		Destroy();
		short version = 1;
		r >> version;
		r >> m_name;
		short num = 0;
		r >> num;
		char buf[512];
		if(num>0)
			m_states.SetArrayCount(num);
		for(int i=0;i<num;i++){
			NameState& n = m_states[i];
			r >> n.isNumber >> n.name;
			if(n.isNumber==0){
				short len = 0;
				r >> len;
				if(len<512){
					r.read(buf, len);
					n.setText(buf);
				}
				else{
					char* ch = new char[len];
					r.read(ch, len);
					delete[] ch;
					n.setText(ch);
				}
			}
			else
			{
				r >> n.value;
			}
		}
		short ary = 0;
		r >> ary;
		if(ary>0)
			m_arrays.SetArrayCount(ary);
		for(int i=0;i<m_arrays.size();i++){
			StateValue& n = m_arrays[i];
			r >> n.isNumber;
			if(n.isNumber==0){
				short len = 0;
				r >> len;
				if(len<512){
					r.read(buf, len);
					n.setText(buf);
				}
				else{
					char* ch = new char[len];
					r.read(ch, len);
					delete[] ch;
					n.setText(ch);
				}
			}
			else
			{
				r >> n.value;
			}
		}
		short child = 0;
		r >> child;
		for(int i=0;i<child;i++){
			GameState* ch = new GameState();
			ch->m_bCanSave=this->m_bCanSave;
			ch->load(r);
			m_childs.push_back(ch);
		}
		int check = 0;
		r >> check;
		assert(check==0x12345678);
		return true;
	}
	VOID	ThrowError()
	{
		assert(false);
		g_manager.ExitGame();
	}
	void		GameState::save(LPCSTR szFile, GenuineGUID* guid)
	{CPUTime(GameState);
#ifdef EDITOR_OS
		return;
#endif
		if(!m_bCanSave)
			return;
		CSafeFileHelperW w;
		if(!w.create(szFile))
		{
			LogError("SaveFileInDoc Error:%s", szFile);
			return;
		}
		int version = 1;
		w << version;
		int head = _MakeFileHead('s','t','a',0);
		w << head;
		streamWrite ww(1024*1024);
		save(ww);
		CDynamicArray<char> ary;
		if(ww.m_point>0){
			EncryptData(ary, (const char*)ww.m_ptr, ww.m_point);
		}
		int len = ary.size();
		w << len;
		if(len>0)
			w.pStream->write(ary.GetPtr(), ary.size());
	}
	bool		GameState::load(LPCSTR szFile, GenuineGUID* guid)
	{CPUTime(GameState);
		FileName f = szFile;
		f.setIsFile();
		CSafeFileHelperR r;
		if(!r.openInDoc(szFile)){
			return false;
		}
		int version = 1;
		r >> version;
		int head;
		r >> head;
		if(head!=_MakeFileHead('s','t','a',0))
			return false;
		int len;
		r >> len;
		bool bret = true;
		if(len>0){
			char* ch = new char[len];
			r.pStream->read(ch, len);
			int newlen = 0;
			char* ret = DecryptData(ch, len, newlen);
			if(ret){
				streamRead sr(ret, newlen);
				bret = load(sr);
			}
			else
				bret = false;
			delete[] ch;
		}
		return bret;
	}
	void		saveState(const char* szFile)
	{
#ifndef EDITOR_OS
		GetStateMgr()->save(szFile, &g_stateGUID);
#endif
	}
	bool		loadState(const char* szFile)
	{
#ifndef EDITOR_OS
		return GetStateMgr()->load(szFile, &g_stateGUID);
#else
		return true;
#endif
	}
	Sqlite3::Sqlite3()
	{
		m_sqlite = 0;
	}
	Sqlite3::~Sqlite3()
	{
		CloseDB();
	}

	BOOL			Sqlite3::OpenDB(const char* szFile)
	{CPUTime(Sqlite3);
		if(m_sqlite)
			CloseDB();
		int rc = sqlite3_open(szFile, &m_sqlite);
		if(rc != SQLITE_OK)
			return false;
		return true;
	}

	VOID			Sqlite3::CloseDB()
	{CPUTime(Sqlite3);
		if(m_sqlite)
			sqlite3_close(m_sqlite);
		m_sqlite = 0;
	}

	sqlite3_stmt*	Sqlite3::Select(const char* szExec)
	{CPUTime(Sqlite3);
		if(!m_sqlite)return 0;
		sqlite3_stmt* stmt = 0;
		sqlite3_prepare(m_sqlite, szExec, -1,&stmt,0);
		if(!stmt)
			return 0;
		return stmt;
	}

	BOOL			Sqlite3::MoveNext(sqlite3_stmt* stmt)
	{CPUTime(Sqlite3);
		if(!stmt || !this->m_sqlite)
			return false;
		int rc = sqlite3_step(stmt);
		if(rc == SQLITE_ROW)
			return true;
		sqlite3_finalize(stmt);
		return false;
	}

	INT				Sqlite3::ReadInt(sqlite3_stmt* stmt, DBColumn& col)
	{CPUTime(Sqlite3);
		if(!InitColumn(stmt, col))
			return 0;
		return sqlite3_column_int(stmt, col.index);
	}

	double			Sqlite3::ReadDouble(sqlite3_stmt* stmt, DBColumn& col)
	{CPUTime(Sqlite3);
		if(!InitColumn(stmt, col))
			return 0;
		return sqlite3_column_double(stmt, col.index);
	}

	const char*		Sqlite3::ReadText(sqlite3_stmt* stmt, DBColumn& col)
	{CPUTime(Sqlite3);
		if(!InitColumn(stmt, col))
			return 0;
		return (const char*)sqlite3_column_text(stmt, col.index);
	}
	BOOL			Sqlite3::ExecuteDB(const char* szExec)
	{CPUTime(Sqlite3);
		if(!m_sqlite)return false;
		char* error = 0;
		if(sqlite3_exec(m_sqlite, szExec, 0, 0, &error) != SQLITE_OK)
		{
			sqlite3_free(error);
			return false;
		}
		return true;
	}
	VOID			Sqlite3::Execute(const char* szExec, BOOL bTransMode)
	{CPUTime(Sqlite3);
		if(bTransMode)
			ExecuteDB("BEGIN TRANSACTION;");
		ExecuteDB(szExec);
		if(bTransMode)
			ExecuteDB("END TRANSACTION;");
	}

	BOOL			Sqlite3::InitColumn(sqlite3_stmt* stmt, DBColumn& col)//自动初始化列
	{CPUTime(Sqlite3);
		if(!m_sqlite)return false;
		if(col.index >= 0)
			return true;
		int columnCount	=	sqlite3_column_count(stmt);
		for(int i=0;i<columnCount;i++)
		{
			const char* name = sqlite3_column_name(stmt, i);
			if(col.name == name)
			{
				col.index = i;
				return true;
			}
		}
		return false;
	}

	char s_luaCodesymbol[] = {' ','\t','+','-','*','/','\"','\\','*','%','^','#','=','~','!','<','>','(',')','{','}','[',']',';',':',',','.','\r','\n','\'', '\"', '\r', '\n', 0};
	BOOL	isLuaSection(wchar_t c)
	{
		int len	=	sizeof(s_luaCodesymbol) / sizeof(wchar_t);
		for(int i=0;i<len;i++)
			if(c == s_luaCodesymbol[i])
				return true;
		return false;
	}
	ConfigLine*	ConfigLine::Clone(ConfigMgr* mgr)//克隆一个
	{CPUTime(ConfigLine);
		ConfigLine* l = new ConfigLine();
		l->m_id.id	=	mgr->GetNewIdentity();
		l->m_config	=	m_config;
		l->m_index	=	-1;
		l->m_data	=	new ConfigVal[m_config->m_numCol];
		for(int i=0;i<m_config->m_numCol;i++){
			l->m_data[i] = m_data[i];
		}
		return l;
	}
	ConfigLine::ConfigLine(){
		m_data		=	0;//行数据
		m_config	=	0;
		m_index		=	-1;
		m_script	=	0;
	}
	ConfigLine::~ConfigLine(){
		if(m_data)
			delete[] m_data;
	}
	BOOL			ConfigLine::SearchID(GameID& id, const char* name)
	{CPUTime(ConfigLine);
		if(id.id!=GetGameMgr()->GetReloadCount())
			id.index = 10000;
		if(id.index>10000)
			return false;
		if(id.index==10000)//如果是-1
		{
			id.id=GetGameMgr()->GetReloadCount();
			int index = SearchCol(name);
			if(index==-1)//如果没有找到这个行
			{
				LogInfo("system->error:(%s)%s(%s)",this->m_config->m_tableName.t, utf8("中找不到指定的列"), name);
				id.index = 10001;
				return false;
			}
			id.index = index;
		}
		return (id.index < m_config->m_numCol);
	}
	int				ConfigLine::GetIntVal(GameID& id, const char* name)
	{CPUTime(ConfigLine);
		if(!SearchID(id, name))
			return 0;
		return GetIntVal(id.index);
	}
	float			ConfigLine::GetDoubleVal(GameID& id, const char* name)
	{CPUTime(ConfigLine);
		if(!SearchID(id, name))
			return 0;
		return GetDoubleVal(id.index);
	}
	const char*		ConfigLine::GetTextVal(GameID& id, const char* name)
	{CPUTime(ConfigLine);
		if(!SearchID(id, name))
			return "";
		return GetTextVal(id.index);
	}
	ConfigLine*		ConfigLine::GetOtherLinePtr(GameID& id, const char* name)
	{CPUTime(ConfigLine);
		if(!SearchID(id, name))
			return 0;
		return GetOtherLinePtr(id.index);
	}

	VOID			ConfigLine::Setup(Config* c)
	{CPUTime(ConfigLine);
		m_config	=	c;
		if(m_data)
			delete[] m_data;
		m_data = new ConfigVal[c->m_numCol];
	}
	void			ConfigLine::SetIntVal(int col, int v, int index)
	{CPUTime(ConfigLine);
		if(col < 0 || col >= m_config->m_numCol || !m_data)
			return;
		ConfigHeader& head = m_config->m_cols[col];
		ConfigVal& val = m_data[col];
		if(val.list){
			if(index>0&&index<head.listcount)
				val = val.list[index-1];
			else if(index<0||index>=head.listcount)
				return;
		}
		switch(head.type)
		{
		case ConfigHeader::type_float:
			val.fval = v;
			break;
		case ConfigHeader::type_int:
		case ConfigHeader::type_boolean:
			val.ival = v;
			break;
		case ConfigHeader::type_class:
			SetOtherLineID(col, v);
			break;
		default:
			{
				char buf[256];
				sprintf(buf, "%d", v);
				val.setText(buf);
			}
			break;
		}
	}
	void			ConfigLine::SetDoubleVal(int col, float v, int index)
	{CPUTime(ConfigLine);
		if(col < 0 || col >= m_config->m_numCol || !m_data)
			return;
		ConfigHeader& head = m_config->m_cols[col];
		ConfigVal& val = m_data[col];
		if(val.list){
			if(index>0&&index<head.listcount)
				val = val.list[index-1];
			else if(index<0||index>=head.listcount)
				return;
		}
		switch(head.type)
		{
		case ConfigHeader::type_float:
			val.fval = v;
			break;
		case ConfigHeader::type_int:
		case ConfigHeader::type_boolean:
			val.ival = v;
			break;
		case ConfigHeader::type_class:
			SetOtherLineID(col, (int)v);
			break;
		default:
			{
				char buf[256];
				sprintf(buf, "%.03f", v);
				val.setText(buf);
			}
			break;
		}
	}
	void			ConfigLine::SetTextVal(int col, const char* text, int index)
	{CPUTime(ConfigLine);
		if(col < 0 || col >= m_config->m_numCol || !m_data||!text)
			return;
		ConfigHeader& head = m_config->m_cols[col];
		ConfigVal& val = m_data[col];
		if(val.list){
			if(index>0&&index<head.listcount)
				val = val.list[index-1];
			else if(index<0||index>=head.listcount)
				return;
		}
		switch(head.type)
		{
		case ConfigHeader::type_float:
			if(text)val.fval = atof(text);
			break;
		case ConfigHeader::type_int:
		case ConfigHeader::type_boolean:
			if(text)val.ival = atoi(text);
			break;
		case ConfigHeader::type_class:
			SetOtherLineID(col, atoi(text));
			break;
		default:
			val.setText(text);
			break;
		}
	}
	VOID			ConfigLine::SetOtherLineID(int col, int otherid, int index)
	{CPUTime(ConfigLine);
		if(col < 0 || col >= m_config->m_numCol || !m_data)
			return;
		ConfigHeader& head = m_config->m_cols[col];
		ConfigVal& val = m_data[col];
		if(val.list){
			if(index>0&&index<head.listcount)
				val = val.list[index-1];
			else if(index<0||index>=head.listcount)
				return;
		}
		if(head.type!=ConfigHeader::type_class || !head.classPtr)
			return;
		for(int i=0;i<head.classPtr->m_numLine;i++){
			if(head.classPtr->m_lines[i].GetLineID()==otherid)
			{
				val.ival = i;
				break;
			}
		}
	}
	int				ConfigLine::GetOtherLineID(int col, int index)//返回id
	{CPUTime(ConfigLine);
		if(col < 0 || col >= m_config->m_numCol || !m_data)
			return 0;
		ConfigHeader& head = m_config->m_cols[col];
		ConfigVal& val = m_data[col];
		if(index>0&&val.list&&index<head.listcount)
			val = val.list[index-1];
		if(head.type!=ConfigHeader::type_class || !head.classPtr||val.ival<0)
			return 0;
		return head.classPtr->GetLine(val.ival)->GetLineID();
	}
	VOID			ConfigLine::SetOtherLinePtr(int col, ConfigLine* line, int index)
	{CPUTime(ConfigLine);
		if(col < 0 || col >= m_config->m_numCol || !m_data)
			return;
		ConfigHeader& head = m_config->m_cols[col];
		ConfigVal& val = m_data[col];
		if(val.list){
			if(index>0&&index<head.listcount)
				val = val.list[index-1];
			else if(index<0||index>=head.listcount)
				return;
		}
		if(head.type!=ConfigHeader::type_class || !head.classPtr)
			return;
		if(line)
			val.ival = line->m_index;
		else
			val.ival = -1;
	}
	ConfigLine*		ConfigLine::GetOtherLinePtr(int col, int index)//如果是指向其他的类,返回其他类对应的行,没有则返回0
	{CPUTime(ConfigLine);
		if(col < 0 || col >= m_config->m_numCol || !m_data)
			return 0;
		ConfigHeader& head = m_config->m_cols[col];
		ConfigVal& val = m_data[col];
		if(index>0&&val.list&&index<head.listcount)
			val = val.list[index-1];
		if(head.type!=ConfigHeader::type_class || !head.classPtr || val.ival<0)
			return 0;
		return &head.classPtr->m_lines[val.ival];
	}
	int				ConfigLine::GetIntVal(int col, int index)
	{CPUTime(ConfigLine);
		if(col < 0 || col >= m_config->m_numCol || !m_data)
			return 0;
		ConfigHeader& head = m_config->m_cols[col];
		ConfigVal& val = m_data[col];
		if(index>0&&val.list&&index<head.listcount)
			val = val.list[index-1];
		switch(head.type)
		{
		case ConfigHeader::type_float:
			return (int)val.fval;
		case ConfigHeader::type_int:
		case ConfigHeader::type_boolean:
			return val.ival;
		case ConfigHeader::type_class:
			return GetOtherLineID(col);
		default:
			return atoi(val.getText());
		}
		return val.ival;
	}
	float			ConfigLine::GetDoubleVal(int col, int index)
	{CPUTime(ConfigLine);
		if(col < 0 || col >= m_config->m_numCol || !m_data)
			return 0;
		ConfigHeader& head = m_config->m_cols[col];
		ConfigVal& val = m_data[col];
		if(index>0&&val.list&&index<head.listcount)
			val = val.list[index-1];
		switch(head.type)
		{
		case ConfigHeader::type_float:
			return val.fval;
		case ConfigHeader::type_int:
		case ConfigHeader::type_boolean:
			return (float)val.ival;
		case ConfigHeader::type_class:
			return GetOtherLineID(col);
		default:
			return atof(val.getText());
		}
		return val.fval;
	}
	const char*		ConfigLine::GetTextVal(int col, int index)
	{CPUTime(ConfigLine);
		if(col < 0 || col >= m_config->m_numCol || !m_data)
			return "";
		static int tempIndex = 0;
		static NameT<16>	temp[16];
		ConfigHeader& head = m_config->m_cols[col];
		ConfigVal& val = m_data[col];
		if(index>0&&val.list&&index<head.listcount)
			val = val.list[index-1];
		switch(head.type)
		{
		case ConfigHeader::type_float:
			{
				NameT<16>& r = temp[tempIndex++];
				tempIndex %= 16;
				sprintf(r.t, "%.03f",val.fval);
				return r.t;
			}
			break;
		case ConfigHeader::type_int:
			{
				NameT<16>& r = temp[tempIndex++];
				tempIndex %= 16;
				sprintf(r.t, "%d",val.ival);
				return r.t;
			}
		case ConfigHeader::type_boolean:
			{
				if(val.ival!=0)
					return "true";
				return "false";
			}
		case ConfigHeader::type_class:
			{
				NameT<16>& r = temp[tempIndex++];
				tempIndex %= 16;
				sprintf(r.t, "%d",(int)GetOtherLineID(col));
				return r.t;
			}
		default:
			return val.getText();
		}
		return "";
	}
	int				ConfigLine::SearchCol(const char* columnName)
	{CPUTime(ConfigLine);
		for(int i=0;i<m_config->m_numCol;i++)
			if(m_config->m_cols[i].name == columnName)
				return i;
		return -1;
	}
	const char*		ConfigLine::GetTableName()
	{CPUTime(ConfigLine);
		return m_config->m_tableName;
	}
	const char*		ConfigLine::GetLineName()
	{CPUTime(ConfigLine);
		if(m_data)
			return m_data[1].getText();
		return "";
	}
	VOID			ConfigLine::SetLineName(const char* name)
	{CPUTime(ConfigLine);
		if(m_data)
			m_data[1].setText(name);
	}
	int				ConfigLine::GetLineID()
	{CPUTime(ConfigLine);
		if(m_data)
			return (int)m_data[0].ival;
		return 0;
	}
	VOID			ConfigLine::SetLineID(int id)
	{CPUTime(ConfigLine);
		if(m_data)
			m_data[0].ival = id;
	}

	VOID	ConfigHeader::SetName(const char* n)
	{CPUTime(ConfigHeader);
		name = n;
		int len = name.size();
		for(int i=0;i<len;i++){
			if(name[i]=='[')//如果是数组
			{
				name.t[i] = 0;
				listcount = atoi(name.t+(i+1));
				break;
			}
			if(isLuaSection(name.t[i]))
				name.t[i] = '_';
		}
	}

	const char*	ConfigVal::getText()
	{CPUTime(ConfigVal);
		if(text)
			return text;
		return "";
	}
	void		ConfigVal::freeText()
	{CPUTime(ConfigVal);
		if(text)
			free(text);
		text = 0;
		fval=0;
		ival=0;
	}

	void	ConfigVal::setText(const char* t)
	{CPUTime(ConfigVal);
		if(!t)
		{
			freeText();
			return;
		}
		int len = strlen(t)+1;
		if(len==0)
		{
			freeText();
			return;
		}
		int oldLen = ival;
		//if(text)
		//	oldLen = *(int*)text;
		if(len>oldLen){
			oldLen = len;
			freeText();
			text = (char*)malloc(oldLen);
			ival = oldLen;
		}
		memcpy(text, t, len);
	}
	void	ConfigVal::operator = (const ConfigVal& v)
	{
		ival = v.ival;
		fval = v.fval;
		if(v.text)
		{
			ival = 0;
			fval = 0;
			this->setText(v.text);
		}
		assert(list==0);
	}
	ConfigVal::~ConfigVal()
	{
		if(text)
			free(text);
		if(list)
			delete[] list;
	}
	Config::Config()
	{
		m_cols = 0;//表头数据
		m_lines = 0;//行数据
		m_numLine = 0;
		m_numCol = 0;
		m_arrayIndex = 0;//数组索引列表
		m_arrayCount	= 0;//数组数量，一个表格中的数组变量不能超过20个
	}
	Config::~Config()
	{
		if(m_cols)
			delete[] m_cols;
		if(m_lines)
			delete[] m_lines;
		if(m_arrayIndex)
			delete[] m_arrayIndex;
	}
	int					Config::GetLineCount()
	{CPUTime(Config);
		return this->m_numLine;
	}
	ConfigLine*			Config::GetLine(int line)
	{CPUTime(Config);
		if(line<0||line>m_numLine)
			return 0;
		return &this->m_lines[line];
	}
	int					Config::SearchCol(const char* columnName)
	{CPUTime(Config);
		for(int i=0;i<m_numCol;i++)
			if(m_cols[i].name == columnName)
				return i;
		return -1;
	}
	BOOL				Config::IsExistLine(int id)
	{CPUTime(Config);
		for(int i=0;i<m_numLine;i++)
		{
			if((int)m_lines[i].GetLineID() == id)
				return true;
		}
		return false;
	}
	ConfigLine*			Config::GetLineFromID(int id)
	{CPUTime(Config);
		for(int i=0;i<m_numLine;i++)
		{
			if((int)m_lines[i].GetLineID() == id)
				return &m_lines[i];
		}
		return 0;
	}

	const char*			Config::GetTableName()
	{CPUTime(Config);
		return this->m_tableName;
	}
	VOID				ConfigHeader::ResetType()
	{CPUTime(ConfigHeader);
		if(type_s=="float")
			type = type_float;
		else if(type_s=="text")
			type = type_text;
		else if(type_s=="int")
			type = type_int;
		else if(type_s=="bool")
			type = type_boolean;
		else
			type = type_class;//返回类
	}
	int					Config::GetArrayCol(int listIndex)//返回数组索引中的列索引
	{CPUTime(Config);
		if(listIndex<0||listIndex>=m_arrayCount)return -1;
		return m_arrayIndex[listIndex];
	}
	VOID				Config::RebuildListIndexs()
	{CPUTime(Config);
		m_arrayCount = 0;
		if(!m_arrayIndex)
			m_arrayIndex = new unsigned short[m_numCol];
		for(int i=0;i<m_numCol;i++){
			if(m_cols[i].listcount>1)//如果数组数量超过1个
			{
				if(m_arrayCount>10){
					LogInfo("system->error:(%s::%s)%s", m_tableName.t, m_cols[i].name.t, utf8("一个表格中不能超过10个数组"));
					break;
				}
				m_arrayIndex[m_arrayCount++] = i;
			}
		}
	}
	BOOL				Config::Load(ConfigMgr* mgr, const char* szFileName)
	{CPUTime(Config);
		if(m_cols)
			delete[] m_cols;
		if(m_lines)
			delete[] m_lines;
		if(m_arrayIndex)
			delete[] m_arrayIndex;
		m_arrayIndex = 0;
		m_arrayCount = 0;
		m_cols = 0;
		m_lines = 0;
		FileName f = szFileName;
		f.setIsFile();
		FileName ff;
		f.splitpath(0, &ff, 0);
		this->m_tableName = ff.t;
		TableTxt txt;
		if(!txt.Load(szFileName))// ||  || stricmp(txt.getHead(0),"id")!=0 || stricmp(txt.getHead(1),"name")!=0)
		{
			LogInfo("system->error:%s(%s)", utf8("不能读取文件"), szFileName);
			return false;
		}
		if(txt.getNumLine()<2){
			LogInfo("system->error:%s(%s)", utf8("行中没有说明行和类型行"), szFileName);
			return false;
		}
		if(stricmp(txt.getHead(0),"id")!=0 || stricmp(txt.getHead(1),"name")!=0){
			LogInfo("system->error:%s(%s)", utf8("第一列必须得是id，第二列必须得是name"), szFileName);
			return false;
		}
		m_id	=	mgr->m_id++;
		int cols = txt.getNumHead();
		m_cols = new ConfigHeader[cols];
		for(int i=0;i<cols;i++)
		{
			m_cols[i].SetName(txt.getHead(i));
			m_cols[i].desc		= txt.getLineItemI(0, i);
			m_cols[i].type_s	= txt.getLineItemI(1, i);
			m_cols[i].ResetType();
		}
		int lines = txt.getNumLine() - 2;//抛去前两行，一个是中文名，一个是类型
		m_numCol	= cols;
		m_numLine	= lines;
		if(lines==0)
			return true;
		m_lines = new ConfigLine[m_numLine];
		for(int l=0;l<m_numLine;l++){
			m_lines[l].Setup(this);
			m_lines[l].m_index = l;
		}
		for(int c=0;c<m_numCol;c++){
			for(int l=0;l<m_numLine;l++){
				const char* szLine = txt.getLineItemI(l+2, c);
				ConfigVal& val = m_lines[l].m_data[c];
				switch(m_cols[c].type){
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
					val.ival = -1;
					val.setText(szLine);
					break;
				default:
					{
						FileName temp;
						if(this->m_cols[c].name=="file")
						{
							temp = szLine;
							temp.setIsFile();
							szLine = temp;
						}
						val.setText(szLine);
					}
					break;
				}
			}
		}
		RebuildListIndexs();
		return true;
	}

	ConfigMgr::ConfigMgr()
	{
		m_id	=	1;
		m_tblRoles	=	0;
		m_tblAI = 0;//AI表格
		m_tblScene = 0;//场景列表
		m_tblCity = 0;//城市列表
		m_tblMission = 0;//任务列表
		m_tblSkill = 0;//技能列表
		m_tblLeader = 0;//主角属性列表
		m_tblText = 0;
		m_tblSceneUnit = 0;
		m_colTextDesc = 2;
	}
	ConfigMgr::~ConfigMgr()
	{
		for(int i=0;i<m_configs.size();i++)
			delete m_configs[i];
		m_configs.clear();
	}
	VOID				ConfigMgr::OnDeleteLine(ConfigLine* l)
	{CPUTime(ConfigMgr);
		for(int i=0;i<m_lines.size();i++){
			if(m_lines[i]==l)
			{
				m_lines.eraseByIndex(i);
				break;
			}
		}
	}
	VOID				ConfigMgr::Destroy()
	{CPUTime(ConfigMgr);
		m_lines.clear();
		for(int i=0;i<m_configs.size();i++)
			delete m_configs[i];
		m_tblRoles	=	0;
		m_tblAI = 0;//AI表格
		m_tblScene = 0;//场景列表
		m_tblCity = 0;//城市列表
		m_tblMission = 0;//任务列表
		m_tblSkill = 0;//技能列表
		m_tblLeader = 0;
		m_tblText = 0;
		m_tblSceneUnit = 0;
		m_colTextDesc = 2;
		m_configs.clear();
	}
	Config*				ConfigMgr::GetConfigPtr(PtrID& id, const char* szTableName)
	{CPUTime(ConfigMgr);
		if(id.id==0)return 0;
		if(id.index<m_configs.size()&&m_configs[id.index]->m_id==id.id)
			return m_configs[id.index];
		for(int i=0;i<m_configs.size();i++)
		{
			if(m_configs[i]->m_tableName == szTableName)
			{
				id.index = i;
				id.id = m_configs[i]->m_id;
				return m_configs[i];
			}
		}
		id.id = 0;
		return 0;
	}
	Config*				ConfigMgr::SearchConfig(const char* szTableName)//搜索表格
	{CPUTime(ConfigMgr);
		for(int i=0;i<m_configs.size();i++)
			if(m_configs[i]->m_tableName == szTableName)
				return m_configs[i];
		return 0;
	}

	void	enum_files(const char* path,CDynamicArray<FileName>& _out, int subFolderLevel, NameT<16>* szFindType,int numFindTypes)
	{
		int len=(int)strlen(path);
		if(!path||len==0)
			return;
		char szPath[512],szFind[512];
		strcpy(szPath,path);
		if(szPath[len-1]!='\\'&&szPath[len-1]!='/')
			strcat(szPath,"\\");
		strcpy(szFind,szPath);
		strcat(szFind,"*.*");
#ifdef WIN32_OS
		WIN32_FIND_DATAA wsa;
		HANDLE hFind=FindFirstFileA(szFind,&wsa);
		if(hFind==INVALID_HANDLE_VALUE)
			return;
		bool bFindAll = (szFindType==NULL);
		while(true)
		{
			//
			if(wsa.cFileName[0]!='.')
			{
				if((wsa.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
				{
					if(subFolderLevel > 0)
					{
						char szNewPath[512];
						strcpy(szNewPath,szPath);
						strcat(szNewPath,wsa.cFileName);
						strcat(szNewPath,"\\");
						//
						enum_files(szNewPath,_out,subFolderLevel - 1,szFindType,numFindTypes);
					}
				}
				else if(! (wsa.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
				{
					std::string sp=szPath;
					sp+=wsa.cFileName;
					if(!bFindAll)
					{
						char _ext[255];
						_splitpath(sp.c_str(),0,0,0,_ext);
						for(int i=0;i<numFindTypes;i++)
						{
							if(stricmp(_ext,szFindType[i].t)==0)
							{
								_out.push_back(FileName(sp.c_str()));
								break;
							}
						}
					}
					else
						_out.push_back(FileName(sp.c_str()));
				}
			}
			//
			if(!FindNextFileA(hFind,&wsa))
				break;
		}
		FindClose(hFind);
#endif
	}

	BOOL				ConfigMgr::ResearchAllFiles()//搜索文件中的所有表格并重新载入
	{CPUTime(ConfigMgr);
		m_id = 1;//reset ID
		CDynamicArray<FileName> rets;
		{
			//NameT<16> txt = ".txt";
			//enum_files("data/", rets, 1, &txt, 1);
			TableTxt filelist;
			if(!filelist.Load("data/filelist.txt"))
			{
				LogInfo("system->error:%s(data/filelist.txt)", utf8("找不到文件列表"));
				return false;
			}
			for(int i=0;i<filelist.GetNumofLine();i++){
				const char* file = filelist.read(i, "file");
				if(file&&file[0])
				{
					FileName temp;
					sprintf(temp.t, "data/%s.txt", file);
					rets.push_back(temp);
				}
			}
		}
		Destroy();
		if(rets.size()==0)
			return false;
		int numConfig = rets.size();
		CDynamicArray<Config*>		configs;
		for(int i=0;i<numConfig;i++)
		{
			Config* cfg = new Config();
			if(!cfg->Load(this, rets[i].t))
			{
				delete cfg;
				continue;
			}
			cfg->m_id = m_id++;
			if(cfg->m_tableName=="role_table")//角色表不进入自动表格列表，角色表单独创建
				this->m_tblRoles = cfg;
			else if(cfg->m_tableName=="ai_table")//角色表不进入自动表格列表，角色表单独创建
				this->m_tblAI = cfg;
			else if(cfg->m_tableName=="scene_table")//角色表不进入自动表格列表，角色表单独创建
				this->m_tblScene = cfg;
			else if(cfg->m_tableName=="region_table")//角色表不进入自动表格列表，角色表单独创建
				this->m_tblCity = cfg;
			else if(cfg->m_tableName=="mission_table")//角色表不进入自动表格列表，角色表单独创建
				this->m_tblMission = cfg;
			else if(cfg->m_tableName=="skill_table")//角色表不进入自动表格列表，角色表单独创建
				this->m_tblSkill = cfg;
			else if(cfg->m_tableName=="lead_table")
				this->m_tblLeader = cfg;
			else if(cfg->m_tableName=="sceneunit_table")
				this->m_tblSceneUnit = cfg;
			else if(cfg->m_tableName=="text_list")
				this->m_tblText = cfg;
			else
				configs.push_back(cfg);
			m_configs.push_back(cfg);
		}
		for(int i=0;i<m_configs.size();i++){
			Config* src = m_configs[i];
			src->m_index = i;
			for(int j=0;j<src->m_numCol;j++){
				ConfigHeader& col = src->m_cols[j];
				if(col.type==ConfigHeader::type_class){
					for(int m=0;m<m_configs.size();m++){
						if(col.type_s == m_configs[m]->m_tableName)
						{
							col.classPtr = m_configs[m];
							break;
						}
					}
					if(!col.classPtr)
					{
						LogInfo("system->error: can't find class(%s) of column in table(%s)", col.type_s.t, src->m_tableName.t);
						col.type = ConfigHeader::type_text;
					}
					else
					{
						for(int line=0;line<src->m_numLine;line++){
							if(col.classPtr){
								ConfigVal* val = &src->GetLine(line)->m_data[j];
								val->ival = -1;
								if(val->text){
									char text[256];
									strcpy(text, val->getText());
									val->freeText();
									val->ival = -1;
									for(int k=0;k<col.classPtr->m_numLine;k++){
										if(stricmp(col.classPtr->GetLine(k)->GetLineName(), text)==0)//找到了
										{
											val->ival = k;//col.classPtr->GetLine(k)->GetID();
											break;
										}
									}
								}
								if(val->text){
									LogInfo("system->error: can't find name(%s) at class(%s) of column in table(%s) at line(%d)", val->text, col.type_s.t, src->m_tableName.t, line);
									val->freeText();
								}
							}
						}
					}
				}
			}
		}
		for(int i=0;i<m_configs.size();i++){
			for(int j=0;j<m_configs[i]->m_numLine;j++){
				ConfigLine* line = m_configs[i]->GetLine(j);
				line->m_id.index = m_lines.size();
				line->m_id.id = m_id++;
				m_lines.push_back(line);
			}
		}
		m_configLua.SetArrayCount(0);
		//assert(m_tblRoles!=0);
		//assert(m_tblAI!=0);
		//assert(m_tblScene!=0);
		//assert(m_tblCity!=0);
		//assert(m_tblMission!=0);
		//assert(m_tblSkill!=0);
		//assert(m_tblLeader!=0);
		//assert(m_tblSceneUnit!=0);
		//if(m_tblRoles==0){
		//	LogInfo("system->error:%s", utf8("找不到角色表，不能继续使用游戏系统"));
		//	return false;
		//}
		//if(m_tblAI==0){
		//	LogInfo("system->error:%s", utf8("找不到AI表，不能继续使用游戏系统"));
		//	return false;
		//}
		//if(m_tblScene==0){
		//	LogInfo("system->error:%s", utf8("找不到地区表，不能继续使用游戏系统"));
		//	return false;
		//}
		//if(m_tblCity==0){
		//	LogInfo("system->error:%s", utf8("找不到城市表，不能继续使用游戏系统"));
		//	return false;
		//}
		//if(m_tblMission==0){
		//	LogInfo("system->error:%s", utf8("找不到任务表，不能继续使用游戏系统"));
		//	return false;
		//}
		//if(m_tblSkill==0){
		//	LogInfo("system->error:%s", utf8("找不到技能表，不能继续使用游戏系统"));
		//	return false;
		//}
		//if(m_tblLeader==0){
		//	LogInfo("system->error:%s", utf8("找不到主角表，不能继续使用游戏系统"));
		//	return false;
		//}
		//if(m_tblSceneUnit==0){
		//	LogInfo("system->error:%s", utf8("找不到场景单位表，不能继续使用游戏系统"));
		//	return false;
		//}
		if(m_tblAI)
			m_configLua.push_back(m_tblAI);
		if(m_tblScene)
			m_configLua.push_back(m_tblScene);
		if(m_tblCity)
			m_configLua.push_back(m_tblCity);
		if(m_tblMission)
			m_configLua.push_back(m_tblMission);
		if(m_tblSkill)
			m_configLua.push_back(m_tblSkill);
		if(m_tblSceneUnit)
			m_configLua.push_back(m_tblSceneUnit);
		for(int i=0;i<configs.size();i++){
			if(m_configLua.size()>MAX_CONFIG_FILE){
				LogInfo("system->error: %s", utf8("(除了role_table.txt和lead_table.txt之外,最多额外能载入20个配置文件"));
				break;
			}
			if(strstr(configs[i]->GetTableName(), "_table"))
				m_configLua.push_back(configs[i]);
		}
		for(int i=0;i<m_configLua.size();i++){
			m_configLua[i]->m_index = i;
		}
		if(m_tblText)
		{
			m_colTextDesc = m_tblText->SearchCol("desc");
			int count = m_tblText->GetLineCount();
			for(int i=0;i<count;i++){
				char* ch = (char*)m_tblText->GetLine(i)->GetTextVal(m_colTextDesc);
				if(ch[0]){
					int k=strlen(ch);
					for(int j=0;j<k;j++){
						if(ch[j]=='\\'&&ch[j+1]=='n'){
							ch[j] = '\r';
							ch[j+1] = '\n';
						}
					}
				}
			}
		}
		return true;
	}
	ConfigLine*			ConfigMgr::SearchLineC(const GameID& id)
	{CPUTime(ConfigMgr);
		if(id.id==0)return 0;
		if(id.index<m_lines.size()&&m_lines[id.index]->m_id.id==id.id)
			return m_lines[id.index];
		for(int i=0;i<m_lines.size();i++){
			if(m_lines[i]->m_id.id == id.id)
			{
				//id.index = i;
				//id.id = m_lines[i]->m_id.id;
				return m_lines[i];
			}
		}
		//id.id = 0;
		return 0;
	}
	ConfigLine*			ConfigMgr::SearchLine(GameID& id)
	{CPUTime(ConfigMgr);
		if(id.id==0)return 0;
		if(id.index<m_lines.size()&&m_lines[id.index]->m_id.id==id.id)
			return m_lines[id.index];
		for(int i=0;i<m_lines.size();i++){
			if(m_lines[i]->m_id.id == id.id)
			{
				id.id = m_lines[i]->m_id.id;
				id.index = i;
				return m_lines[i];
			}
		}
		id.id = 0;
		return 0;
	}
	GameID				ConfigMgr::GetLineOther(GameID& id, int col, int index)//返回这个行的指向行
	{CPUTime(ConfigMgr);
		GameID gid;
		ConfigLine* line = SearchLine(id);
		if(!line)return gid;
		ConfigLine* other = line->GetOtherLinePtr(col, index);
		if(other)
			gid = other->GetID();
		return gid;
	}
	VOID				ConfigMgr::SetLineOtherC(GameID& id, int col, const GameID& otherID, int index)
	{CPUTime(ConfigMgr);
		if(otherID.id==0)return;
		ConfigLine* line = SearchLine(id);
		if(!line)return;
		line->SetOtherLinePtr(col, SearchLineC(otherID), index);
	}
	VOID				ConfigMgr::SetLineOther(GameID& id, int col, GameID& otherID, int index)
	{CPUTime(ConfigMgr);
		if(otherID.id==0)return;
		ConfigLine* line = SearchLine(id);
		if(!line)return;
		line->SetOtherLinePtr(col, SearchLine(otherID), index);
	}
	const char*			ReadText(CFileHelperR& r, char* buf)
	{
		int len = 0;
		r >> len;
		if(len >= 2048)
			return "";
		r.read(buf, len);
		buf[len] = 0;
		return buf;
	}

	void		EncryptData(CDynamicArray<char>& rets, const char* ptr, int length)
	{
		RandomVal rnd, rndSave;
		EncryptRandomVal(rndSave, rnd, g_stateGUID);
		rets.SetArrayCount(sizeof(RandomVal));
		int begin = 0;
		memcpy(rets.GetPtr()+begin, &rndSave, sizeof(RandomVal));
		begin += sizeof(RandomVal);
		EncryptVal nums;
		rets.SetArrayCount(begin + length + sizeof(EncryptVal));
		memcpy(rets.GetPtr() + begin + sizeof(EncryptVal), ptr, length);
		EncryptData(rets.GetPtr()+begin + sizeof(EncryptVal), length, rnd, nums);
		memcpy(rets.GetPtr() + begin, &nums, sizeof(EncryptVal));
	}
	char*		DecryptData(char* inptr, int length, int& realsize)
	{
		RandomVal rnd;
		int begin = 0;
		memcpy(&rnd, inptr, sizeof(RandomVal));
		if(!DecryptRandomVal(rnd, g_stateGUID))
			return 0;
		begin += sizeof(RandomVal);
		EncryptVal chk;
		memcpy(&chk, inptr + begin, sizeof(EncryptVal));
		begin += sizeof(EncryptVal);
		if(!DecryptData(inptr + begin, length - begin, rnd, chk))
			return 0;
		realsize = length - begin;
		return inptr + begin;
	}
};

