#include "LuaManager.h"
#include "PhantomDB.h"
#include "MyInclude.h"
#include "PhantomGame.h"
extern VOID			FromLuaMessage(LPCSTR szName/*消息名称*/, LPCSTR szParams/*消息参数1*/, LPCSTR szParams2/*消息参数2*/);
#ifdef WIN32_OS
const char* utf8(const char* szIn){
	static Phantom::CDynamicArray<char> temps[6];
	static int usage = 0;
	usage++;
	usage %= 6;
	int len = strlen(szIn);
	temps[usage].SetArrayCount(len*3);
	wchar_t wbuf[1024];
	len = ::MultiByteToWideChar(CP_ACP, 0, szIn, len, wbuf, 1024);
	wbuf[len] = 0;
	len = ::WideCharToMultiByte(CP_UTF8, 0, wbuf, len, temps[usage].GetPtr(), len*3, 0, 0);
	temps[usage].SetArrayCount(len);
	return temps[usage].GetPtr();
}
#endif

char	g_currLuaFile[128] = {0};
const char* GetCurrentLuaFile()
{
	return g_currLuaFile;
}
char	g_currentFileIndex = -1;
void	SetCurrentLuaFileIndex(int n){g_currentFileIndex=n;}
extern "C"
{
	void	my_print(const char* szText)
	{
		if(szText[1] != 0)
		{
			if(g_currentFileIndex>=0){
				LogInfo("code->[%s] in file(%s)", szText, Phantom::g_luamanager.GetFileName(g_currentFileIndex));
			}
			else{
				LogInfo("code->[%s]", szText);
			}
		}
	}
	void	my_print_error(const char* szText)
	{
		if(szText[1] != 0)
			LogInfo("error->[%s] in file(%s)", szText, g_currLuaFile);
	}
};

static void call_stack_info(lua_State* L, int n){
	lua_Debug ar;
	if(lua_getstack(L, n, &ar) == 1){
		lua_getinfo(L, "nSlu", &ar);
		const char* indent;
		if(n == 0){
			indent = "->";
			LogInfo("[DEBUG] |/t[[call stack]]");
		}
		else{
			indent = "  ";
		}
		if(ar.name)
		{
			LogInfo("[DEBUG] |%s [%s : line %d] : %s()", indent,ar.short_src ,ar.currentline, ar.name);
		}
		else
		{
			LogInfo("[DEBUG] |%s [%s : line %d] : unkown", indent,ar.short_src,ar.currentline);
		}
		int index = 0;
		do{
			indent = lua_getlocal (L, &ar, ++index);
			if (indent){
				switch(lua_type(L, -1)){
				case LUA_TNIL:
					LogInfo( "[VAR] |/t/t/t/t/t|-%s:%s", lua_typename(L, lua_type(L, -1)),indent);
					break;
				case LUA_TBOOLEAN:
					LogInfo( "[VAR] |/t/t/t/t/t|-%s:%s    %s", lua_typename(L, lua_type(L, -1)),indent, lua_toboolean(L, -1)?"true":"false");
					break;
				case LUA_TLIGHTUSERDATA:
					LogInfo( "[VAR] |/t/t/t/t/t|-%s:%s    0x%08p", lua_typename(L, lua_type(L, -1)),indent, lua_topointer(L, -1));
					break;
				case LUA_TNUMBER:
					LogInfo( "[VAR] |/t/t/t/t/t|-%s:%s    %f", lua_typename(L, lua_type(L, -1)),indent, lua_tonumber(L, -1));
					break;
				case LUA_TSTRING:
					LogInfo( "[VAR] |/t/t/t/t/t|-%s:%s    %s", lua_typename(L, lua_type(L, -1)),indent, lua_tostring(L, -1));                    
					break;
				case LUA_TTABLE:
					LogInfo( "[VAR] |/t/t/t/t/t|-%s:%s    0x%08p", lua_typename(L, lua_type(L, -1)),indent, lua_topointer(L, -1));
					break;
				case LUA_TFUNCTION:
					LogInfo( "[VAR] |/t/t/t/t/t|-%s:%s()    0x%08p", lua_typename(L, lua_type(L, -1)),indent, lua_topointer(L, -1));
					break;
				case LUA_TUSERDATA:
					LogInfo( "[VAR] |/t/t/t/t/t|-%s:%s    0x%08p", lua_typename(L, lua_type(L, -1)),indent, lua_topointer(L, -1));
					break;
				case LUA_TTHREAD:
					LogInfo( "[VAR] |/t/t/t/t/t|-%s:%s", lua_typename(L, lua_type(L, -1)),indent);
					break;
				}
				lua_settop(L, -2);
			}
		} while (indent);
		call_stack_info(L, n+1);
	}
}



int break_point(lua_State *L){
	LogInfo(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
	call_stack_info(L, 0);
	LogInfo("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	return 0;
}

#include "APILua.h"
namespace Phantom{

	extern	int initPhantomEnum(lua_State* L);
extern	int initPhantomMath(lua_State* L);

CLuaFileMgr		g_luamanager;

LuaAction::LuaAction()
{
	g_luamanager.Push(this);
	m_bEnabled	=	true;
	m_currentIndex = 0;
	m_dTime = 0;
	m_dLastTime	=	0;
}
LuaAction::~LuaAction()
{
	g_luamanager.Pop(this);
}
VOID	LuaAction::AddNext(const float4& end, float fTimeLength, int loopCount)
{
	ActionVal v;
	v.next = end;
	v.dTime = fTimeLength;
	v.loop = loopCount;
	v.currentloop = loopCount;
	m_vals.push_back(&v, 1);
}
VOID	LuaAction::OnFrameMove(double dElapsedTime)
{
	if(m_vals.size() <= 1)return;
	m_dTime += dElapsedTime;
	ActionVal& val = m_vals[m_currentIndex];
	if(m_dTime >= val.dTime)
	{
		val.currentloop ++;
		if(val.currentloop >= val.loop)
		{
			m_currentIndex = (m_currentIndex+1)%m_vals.size();
			m_vals[m_currentIndex].currentloop = 0;
		}
		m_dTime -= val.dTime;
	}
	if(m_luaFunc.t[0])
	{
		int val2 = (m_currentIndex+1)%m_vals.size();
		float4& from = this->m_vals[m_currentIndex].next;
		float4& to = this->m_vals[val2].next;
		float fLerp = m_dTime / val.dTime;
		lua_float4 f;
		f = (to-from)*fLerp + from;
		lua_tinker::call<void>(g_luamanager.GetLuaState(), m_luaFunc.t, f);
	}
}
CLuaFile::CLuaFile()
{
	isExcuted = false;
}
CLuaFile::~CLuaFile()
{
}

BOOL		CLuaFile::Load(const char* szText, FileName& f)//从文件载入
{
	this->m_file = f;
	int len = strlen(szText);
	char* buffer = new char[len+1];
	strcpy(buffer, szText);
	buffer[len] = 0;
	int i=0;
	int index = 0;
	bool begin = false;
	while(i<len)
	{
		if(buffer[i] == '\r')
		{
			i++;
			continue;
		}
		if(buffer[i] == ' '||buffer[i] == '\t')
		{
			if(begin)
			{
				i++;
				continue;
			}
			begin = true;
		}
		else
			begin = false;
		if(buffer[i] == '\t')
			buffer[i] = ' ';
		buffer[index++] = buffer[i++];
	}
	buffer[index++] = 0;
	BOOL ret;
	ret = LoadText(buffer, len);
	delete buffer;
	return ret;
}
BOOL		CLuaFile::Load(FileName& f)//从文件载入
{
	this->m_file = f;
	StreamBase* b = open_s(f);
	if(!b)
	{
		LogInfo("system->file not exist(%s)", m_file.t);
		return false;
	}
	BOOL ret = true;
	if(b->size() > 0){
		unsigned char head[3];
		b->read(head, 3);
		if(!(head[0] == 0xef && head[1] == 0xbb && head[2] == 0xbf))
		{
#ifdef WIN32_OS
			LogInfo("system->error:(%s)%s. lua file must be a utf8 char code.",m_file.t, utf8("lua 文件格式必须得是有BOM的utf8格式编码,将被强制转换成utf8格式"));
			b->seek(0);
			int len = b->size();
			char* buffer = new char[len+1];
			b->read(buffer, len);
			buffer[len] = 0;
			CDynamicArray<char> buf;
			Phantom::AsciiToUtf8(buf, buffer);
			SaveUtf8(f, buf.GetPtr());
			delete[] buffer;
#else
			LogInfo("system->error:(%s)%s. lua file must be a utf8 char code.",m_file.t, utf8("lua 文件格式必须得是有BOM的utf8格式编码"));
#endif
			close_s(&b);
			return false;
		}
		int len = b->size()-3;
		char* buffer = new char[len+1];
		b->read(buffer, len);
		buffer[len] = 0;
		//{
		//	char bf[512];
		//	FileName temp = f.t;
		//	FileName fff;
		//	temp.splitpath(0, &fff, 0);
		//	sprintf(bf, "/storage/emulated/0/A_%s.txt", fff.t);
		//	LogInfo("PATH_A:%s,%d/%d", bf,strlen(buffer), len);
		//	FILE* faa = fopen(bf, "wb");
		//	fwrite(buffer, strlen(buffer), 1, faa);
		//	fclose(faa);
		//}
		//int len = strlen(buffer);
		int i=0;
		int index = 0;
		bool begin = false;
		while(i<len)
		{
			if(buffer[i] == '\r')
			{
				i++;
				continue;
			}
			if(buffer[i] == ' '||buffer[i] == '\t')
			{
				if(begin)
				{
					i++;
					continue;
				}
				begin = true;
			}
			else
				begin = false;
			if(buffer[i] == '\t')
				buffer[i] = ' ';
			buffer[index++] = buffer[i++];
		}
		buffer[index++] = 0;
		//{
		//	char bf[512];
		//	FileName temp = f.t;
		//	FileName fff;
		//	temp.splitpath(0, &fff, 0);
		//	sprintf(bf, "/storage/emulated/0/B_%s.txt", fff.t);
		//	LogInfo("PATH_B:%s,%d/%d", bf,strlen(buffer), index);
		//	FILE* faa = fopen(bf, "wb");
		//	fwrite(buffer, strlen(buffer), 1, faa);
		//	fclose(faa);
		//}
		ret = LoadText(buffer, len);
		delete buffer;
	}
	//if(ret)
	//{
	//	FileName temp = m_file;
	//	temp += ".lua";
	//	FILE* f = fopen(temp,"wb");
	//	fwrite(m_script.c_str(), m_script.size(), 1, f);
	//	fclose(f);
	//}
	close_s(&b);
	return ret;
}
inline	BOOL	IsCodeSec(char c){
	switch(c)
	{
	case ' ':
	case '\n':
	case ';':
	case '(':
	case ')':
	case '[':
	case ']':
	case '&':
	case '*':
	case '-':
	case '+':
	case '#':
	case '$':
	case '@':
	case '!':
	case '`':
	case '~':
	case '?':
	case '/':
	case '\\':
	case '.':
	case ',':
	case '<':
	case '>':
		return true;
	}
	return false;
}

BOOL		CLuaFile::LoadText(const char* szText, int len)//从文本载入
{
	const char*	_include = "#include";
	int _includelen = strlen(_include);
	const char*	_define = "#define";
	int _definelen = strlen(_define);
	char ret[10240];
	CDynamicArray<char> buffer2;
	int lastLen = len + 10;
	buffer2.SetArrayCount(len + 10);
	char* buffer = buffer2.GetPtr();
	int index = 0;
	bool nSpace = false;
	for(int i=0;i<len;i++)
	{
		char ch1 = szText[i];
		char ch2 = szText[i + 1];
		char ch3 = szText[i + 2];
		char ch4 = szText[i + 3];
		if(ch1 == '\r')
			continue;
		if(ch1=='#')
		{
			if(strnicmp(szText + i, _include, _includelen) == 0)//include
			{
				int nr = 0;
				int bFirst = 0;
				i += _includelen;
				while(i<len)
				{
					if(szText[i] == '\n')
						break;
					if(bFirst == 0)
					{
						if(szText[i] == ' ' || szText[i] == '\t')
						{
							i++;
							continue;
						}
						if(szText[i] == '\"' ||szText[i] == '\'')
							bFirst = 1;
					}
					else
					{
						if(szText[i] == '\"' || szText[i] == '\'')
							bFirst = 2;
						else if(bFirst == 1)
							ret[nr++] = szText[i];
					}
					i++;
				}
				ret[nr++] = 0;
				//this->m_includes.push_back(ret);
				FileName dir = "sc/";//脚本基础目录
				//m_file.splitpath(&dir, 0, 0);
				dir += ret;
				g_luamanager.Load(dir);
				buffer[index++] = '\n';
				continue;
			}
			else if(strnicmp(szText + i, _define, _definelen) == 0)//define
			{
				int nr = 0;
				int _n = 0;
				int bFirst = 0;
				char name[128];
				i += _definelen + 1;
				while(i<len)
				{
					if(szText[i] == '\n')
					{
						buffer[index++] = '\n';
						ret[nr++] = '\n';
						if(szText[i-1] == '\\' || (szText[i-1] == '_' && szText[i-2] == ' '))
						{
							if(nr >= 2)
								ret[nr-2] = ' ';
							i++;
							continue;
						}
						break;
					}
					if(bFirst == 0){
						if(IsCodeSec(szText[i]))
						{
							bFirst++;
						}
						if(szText[i] != ' ')
							name[_n++] = szText[i];
					}
					else
						ret[nr++] = szText[i];
					i++;
				}
				name[_n++] = 0;
				ret[nr++] = 0;
				g_luamanager.AddMacroText(name, ret);
				continue;
			}
		}
		else if(ch1 == '/' && ch2 == '/')//如果是注释符号则自动过滤
		{
			while(i<len)
			{
				if(szText[i] == '\n')
					break;
				i++;
			}
			buffer[index++] = '\n';
			continue;
		}
		else if(ch1 == '/' && ch2 == '*')//如果是注释符号则自动过滤
		{
			while(i<len)
			{
				if(szText[i] == '*' && szText[i+1] == '/')
				{
					i++;
					break;
				}
				if(szText[i] == '\n')
					buffer[index++] = '\n';
				i++;
			}
			continue;
		}
		else if(ch1 == '\'' || ch1 == '\"')//如果是开引号则全部添加
		{
			int endc = '\'';
			if(ch1 == '\"')
				endc = '\"';
			while(i<len)
			{
				if(szText[i] == endc)
				{
					buffer[index++] = szText[i];
					break;
				}
				if(szText[i] == '\r')
					continue;
				buffer[index++] = szText[i];
				i++;
			}
			continue;
		}
		else
		{
			//bool newline = false;
			//if(index > 0 && buffer[index-1] == '\n')
			//	newline = true;
			//if(IsCodeSec(ch1) || newline)
			{
				char codeseg[256];
				int nseg = 0;
				int bi = i;
				//if(newline)
				//	bi = i;
				while(bi<len)
				{
					if(IsCodeSec(szText[bi]))
					{
						codeseg[nseg] = 0;
						break;
					}
					codeseg[nseg++] = szText[bi];
					bi++;
				}
				bool bMacro = false;
				if(codeseg[0] != 0){
					for(int j=0;j<g_luamanager.m_defines.size();j++)
					{
						if(g_luamanager.m_defines[j]->name == codeseg)
						{
							i += nseg;
							bMacro = true;
							lastLen += g_luamanager.m_defines[j]->text.size();
							buffer2.SetArrayCount(lastLen);
							buffer = buffer2.GetPtr();
							int tsize = g_luamanager.m_defines[j]->text.size();
							const char* tmacro = g_luamanager.m_defines[j]->text.c_str();
							for(int k=0;k<tsize;k++)
								buffer[index++] = tmacro[k];
							if(i < len)
								buffer[index++] = szText[i];
							break;
						}
					}
					if(bMacro)
						continue;
					else
					{
						for(int j=0;j<nseg;j++)
							buffer[index++] = codeseg[j];
						i += nseg-1;
						continue;
					}
				}
			}
			buffer[index++] = ch1;
		}
	}
	buffer[index++] = 0;
	m_script = buffer2.GetPtr();
	return true;
}

CLuaFileMgr::CLuaFileMgr()
{
	m_luaState	=	0;
	this->m_callback	=	0;
	m_timeID = 1;
	m_currentTime = 0;
	m_bOneDown = false;
	m_bDoubleDown = false;
	g_luamanager.m_http.SetCallback(this);
	m_luaJson	=	0;
	m_lastTouchIndex	=	-1;
	m_memvals = new GameState();
	m_memvals->m_bCanSave=false;
	m_dblDownLength	=	0;
}
CLuaFileMgr::~CLuaFileMgr()
{
	if(m_memvals)
		delete m_memvals;
	m_memvals = 0;
	safe_release(m_callback);
	Destroy();
}

int			CLuaFileMgr::GetFileIndex(const char* file)
{
	for(int i=0;i<m_files.size();i++){
		if(m_files[i]->m_file==file)
			return i;
	}
	return -1;
}
const char*	CLuaFileMgr::GetFileName(int index)
{
	if(index<0||index>=m_files.size())
		return "";
	return m_files[index]->m_file.t;
}

CLuaFile*	CLuaFileMgr::Load(const char* szFile)//如果文件存在则直接返回true，不会重复载入同一个文件
{
	InitMyFile(szFile, true, true);
	FileName f = szFile;
	f.setIsFile();
	for(int i=0;i<m_files.size();i++)
	{
		if(m_files[i]->m_file == f)
			return m_files[i];
	}
	CLuaFile* lf = new CLuaFile();
	if(!lf->Load(f))
	{
		delete lf;
		return 0;
	}
	m_files.push_back(lf);
	m_runlist.push_back(lf);
	return lf;
}

/*API*/LPCSTR		WINAPI	Global_callt(LPCSTR szText)
{
	if(!g_luamanager.m_callback)
		return "error";
	NameT<128>	text[16];//最大16个参数
	Text		longtext[16];
	LPCSTR		params[16];
	memset(params, 0, sizeof(params));
	Name		methods;
	int len = strlen(szText);
	int begin = 0;
	int nseek = 0;
	char buf[10240];
	int index = 0;
	for(int i=0;i<len;i++)
	{
		if(szText[i] == '(')
		{
			methods.t[index++] = 0;
			index = 0;
			nseek++;
		}
		else if(szText[i] == ',' || szText[i] == ')')
		{
			if(nseek >= 16)
				return "error";
			buf[index++] = 0;
			if(index < 128)
			{
				text[nseek-1] = buf;
				params[nseek-1] = text[nseek-1];
			}
			else
			{
				longtext[nseek-1] = buf;
				params[nseek-1] = longtext[nseek-1];
			}
			index = 0;
			nseek++;
		}
		else if(nseek == 0)
		{
			if(szText[i]==' ' || szText[i]=='\t'  || szText[i]=='\r' || szText[i]=='\n')
				continue;
			if(index >= methods.Length)
				return "error";
			methods.t[index++] = szText[i];
		}
		else{
			if(szText[i] == '\'' || szText[i] == '\"')
			{
				i++;
				while(i<len)
				{
					if(szText[i] == '\'' || szText[i] == '\"')
						break;
					buf[index++] = szText[i];
					i++;
				}
			}
			else
			{
				if(szText[i]==' ' || szText[i]=='\t'  || szText[i]=='\r' || szText[i]=='\n')
					continue;
				buf[index++] = szText[i];
			}
		}
	}
	return g_luamanager.m_callback->OnCall(methods, params, nseek-1);
}

VOID		CLuaFileMgr::SetCallback(CLuaCallback* cb)
{
	safe_addref(cb);
	safe_release(m_callback);
}

VOID		CLuaFileMgr::AddMacroText(const char* szName, const char* szText)//添加宏
{
	LuaDefine* d = new LuaDefine();
	d->name = szName;
	d->text = szText;
	m_defines.push_back(d);
}
extern		VOID		reloadState();
extern	VOID		SaveStateNow();
VOID		CLuaFileMgr::Destroy()//重置所有的lua设定,可以重新调用Run
{
	if(this->m_files.size() > 0)
		SaveStateNow();
	g_luamanager.m_luaSend.Reset();
	g_manager.ReleaseAllTextureState();
	g_manager.m_textItems.clear();
	GetCameraPtr()->ResetAll();
	GetGameMgr()->DestroyScene();
    //DestroyAllMusic();
	//g_manager.StopAllMusic();
	m_funcRender.t[0] = 0;
	this->m_downFunc.t[0] = 0;
	this->m_upFunc.t[0] = 0;
	this->m_moveFunc.t[0] = 0;
	m_doubleUpFunc.t[0] = 0;
	m_doubleDownFunc.t[0] = 0;
	m_doubleMoveFunc.t[0] = 0;
	if(m_luaState)
		lua_close(m_luaState);
	m_luaState	=	0;
	for(int i=0;i<m_defines.size();i++)
		delete m_defines[i];
	m_defines.clear();
	for(int i=0;i<m_files.size();i++)
	{
		if(m_files[i])
			delete m_files[i];
	}
	m_files.clear();
	m_runlist.clear();
	//for(int i=0;i<m_vals.size();i++)
	//{
	//	if(m_vals[i].tVal.size() > 0)
	//		m_vals[i].tVal.clear();
	//}
	//m_vals.clear();
	for(int i=0;i<m_creats.size();i++)
	{
		if(m_creats[i].fileIndex == -1)
		{
			Base* autoPtr	=	GetBasePtr(m_creats[i].id);
			safe_release(autoPtr);
		}
		else
		{
			MultiMusic* mediaPtr	=	g_manager.GetMusicFilePtrByIndex(m_creats[i].fileIndex);
			if(mediaPtr)
			{
				PtrID id;
				id.id = m_creats[i].id.id;
				id.index = m_creats[i].id.index;
				mediaPtr->Stop(id);
			}
		}
	}
	m_timeID = 1;
	m_times.clear();
}

VOID		LuaTimeData::Init(LuaTimeData::TimeType _type, int _id, const char* _name, int millisecond, int nMaxInterval, const char* _params, unsigned char _idtype, unsigned char _idt, int _idptr, int _ididx)
{
	dTime = (double)millisecond / 1000.0f;
	this->id_type = _idtype;
	this->id_t = _idt;
	this->id_ptr = _idptr;
	this->id_idx = _ididx;
	this->type = _type;
	this->dInitTime = dTime;
	this->id = _id;
	this->name = _name;
	this->dMaxInterval = (double)nMaxInterval/1000.0f;
	this->dMaxIntervalSave = this->dMaxInterval;
	this->dCurrentTime = 0;
	if(_params)
		this->params = _params;
	else
		this->params.t[0] = 0;
	this->pause = false;
}

VOID				CLuaFileMgr::Push(LuaAction*a)
{
	for(int i=0;i<m_actions.size();i++)
		if(m_actions[i] == 0)
		{
			m_actions[i] = a;
			return;
		}
	m_actions.push_back(a);
}
VOID				CLuaFileMgr::Pop(LuaAction*a)
{
	for(int i=0;i<m_actions.size();i++)
		if(m_actions[i]==a)
		{
			m_actions[i] = 0;
			break;
		}
}
VOID	CLuaFileMgr::BreakTime()
{
	if(m_currentTime>=0&&m_currentTime<m_times.size())
		m_times[m_currentTime].id = 0;
	m_currentTime = -1;
}
VOID	CLuaFileMgr::PauseTime(BOOL bPause)
{
	for(int i=0;i<m_times.size();i++)
	{
		m_times[i].pause = bPause;
	}
}
VOID	CLuaFileMgr::OnRender(float dElapsedTime)
{
	if(m_funcRender.t[0])
	{
		lua_tinker::call<void>(this->m_luaState, m_funcRender.t, dElapsedTime);
	}
}
extern	VOID	AutoSaveNow();
VOID	CLuaFileMgr::OnFrameMove(double dElapsedTime, double dRealElapsedTime)
{
	AutoSaveNow();
	GetGameMgr()->OnFrameMove(dElapsedTime);
	if(!m_luaState)
		return;
	m_http.DoMessageQueue();
	for(int i=0;i<m_actions.size();i++)
	{
		if(!m_actions[i])continue;
		if(m_actions[i]->m_bEnabled)
		{
			m_actions[i]->m_dLastTime -=dElapsedTime;
			if(m_actions[i]->m_dLastTime < 0)
			{
				m_actions[i]->m_dLastTime = 0.05;//每隔50毫秒引发一次
				m_actions[i]->OnFrameMove(0.05);
			}
		}
	}
	m_currentTime = -1;
	lua_GameObject_s tobj;
	for(int i=0;i<m_times.size();i++)
	{
		LuaTimeData& d = m_times[i];
		if(d.id <= 0)
			continue;
		m_currentTime = i;
		double dTemp = dElapsedTime;
		if(d.pause)
			dTemp = dElapsedTime;
		else
			dTemp = dRealElapsedTime;
		d.dTime -= dTemp;
		if(d.type==LuaTimeData::TT_FrameMove){
			d.dCurrentTime += dTemp;
			d.dMaxInterval -= dTemp;
			bool bend = false;
			if(d.dMaxInterval<=0)
			{
				bend = true;
				d.id = 0;
			}
			if(d.dCurrentTime>=d.dMaxIntervalSave)
				d.dCurrentTime=d.dMaxIntervalSave;
			switch(d.id_type){
			case LuaTimeData::IDT_GameID:
				{
					tobj.m_data.id = d.id_idx;// = d.gameid;
					tobj.m_data.index = d.id_ptr;
					if(d.params.t[0])
						lua_tinker::call<void>(this->m_luaState, d.name.t, tobj, d.dCurrentTime, bend, d.params.t, d.dMaxIntervalSave);
					else
						lua_tinker::call<void>(this->m_luaState, d.name.t, tobj, d.dCurrentTime, bend, d.dMaxIntervalSave);
				}
				break;
			case LuaTimeData::IDT_Control:
				{
					lua_UIControl_s temp;
					temp.m_data.t = d.id_t;
					temp.m_data.idx = d.id_idx;
					temp.m_data.ptr = d.id_ptr;
					if(d.params.t[0])
						lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.dCurrentTime, bend, d.params.t, d.dMaxIntervalSave);
					else
						lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.dCurrentTime, bend, d.dMaxIntervalSave);
				}
				break;
			case LuaTimeData::IDT_Group:
				{
					lua_UIGroup_s temp;
					temp.m_data.t = d.id_t;
					temp.m_data.idx = d.id_idx;
					temp.m_data.ptr = d.id_ptr;
					if(d.params.t[0])
						lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.dCurrentTime, bend, d.params.t, d.dMaxIntervalSave);
					else
						lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.dCurrentTime, bend, d.dMaxIntervalSave);
				}
				break;
			case LuaTimeData::IDT_Dialog:
				{
					lua_UIDialog_s temp;
					temp.m_data.t = d.id_t;
					temp.m_data.idx = d.id_idx;
					temp.m_data.ptr = d.id_ptr;
					if(d.params.t[0])
						lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.dCurrentTime, bend, d.params.t, d.dMaxIntervalSave);
					else
						lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.dCurrentTime, bend, d.dMaxIntervalSave);
				}
				break;
			case LuaTimeData::IDT_Global:
				{
					if(d.params.t[0])
						lua_tinker::call<void>(this->m_luaState, d.name.t, d.dCurrentTime, bend, d.params.t, d.dMaxIntervalSave);
					else
						lua_tinker::call<void>(this->m_luaState, d.name.t, d.dCurrentTime, bend, d.dMaxIntervalSave);
				}
				break;
			}
			//if(d.isgameid){
			//	GameObject* obj = GetGameMgr()->Search(d.gameid);
			//	if(obj)
			//	{
			//		tobj.m_data = d.gameid;
			//		lua_tinker::call<void>(this->m_luaState, d.name.t, d.dCurrentTime, bend, tobj, d.dMaxIntervalSave);
			//	}
			//	else
			//		d.id = 0;
			//}
			//else{
			//	if(d.params.t[0])
			//		lua_tinker::call<void>(this->m_luaState, d.name.t, d.dCurrentTime, bend, d.params.t, d.dMaxIntervalSave);
			//	else
			//		lua_tinker::call<void>(this->m_luaState, d.name.t, d.dCurrentTime, bend, d.dMaxIntervalSave);
			//}
			if(lua_tinker::g_luaTnikerCallError)
				d.id = 0;
		}
		if(d.dTime <= 0)
		{
			if(d.type == LuaTimeData::TT_Timeout)
			{
				d.id = 0;
				switch(d.id_type){
				case LuaTimeData::IDT_GameID:
					{
						tobj.m_data.id = d.id_idx;// = d.gameid;
						tobj.m_data.index = d.id_ptr;
						if(d.params.t[0])
							lua_tinker::call<void>(this->m_luaState, d.name.t, tobj, d.params.t);
						else
							lua_tinker::call<void>(this->m_luaState, d.name.t, tobj);
					}
					break;
				case LuaTimeData::IDT_Control:
					{
						lua_UIControl_s temp;
						temp.m_data.t = d.id_t;
						temp.m_data.idx = d.id_idx;
						temp.m_data.ptr = d.id_ptr;
						if(d.params.t[0])
							lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.params.t);
						else
							lua_tinker::call<void>(this->m_luaState, d.name.t, temp);
					}
					break;
				case LuaTimeData::IDT_Group:
					{
						lua_UIGroup_s temp;
						temp.m_data.t = d.id_t;
						temp.m_data.idx = d.id_idx;
						temp.m_data.ptr = d.id_ptr;
						if(d.params.t[0])
							lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.params.t);
						else
							lua_tinker::call<void>(this->m_luaState, d.name.t, temp);
					}
					break;
				case LuaTimeData::IDT_Dialog:
					{
						lua_UIDialog_s temp;
						temp.m_data.t = d.id_t;
						temp.m_data.idx = d.id_idx;
						temp.m_data.ptr = d.id_ptr;
						if(d.params.t[0])
							lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.params.t);
						else
							lua_tinker::call<void>(this->m_luaState, d.name.t, temp);
					}
					break;
				case LuaTimeData::IDT_Global:
					{

						if(d.params.t[0])
							lua_tinker::call<void>(this->m_luaState, d.name.t, d.params.t);
						else
							lua_tinker::call<void>(this->m_luaState, d.name.t);
					}
					break;
				}
				continue;
			}
			else if(d.type==LuaTimeData::TT_Interval)
			{
				d.dCurrentTime += d.dInitTime;
				d.dTime += d.dInitTime;
				d.dMaxInterval -= d.dInitTime;
				bool bend = false;
				if(d.dMaxInterval<=0)
				{
					d.id = 0;
					bend = true;
				}
				if(d.dCurrentTime>=d.dMaxIntervalSave)
					d.dCurrentTime=d.dMaxIntervalSave;
				switch(d.id_type){
				case LuaTimeData::IDT_GameID:
					{
						tobj.m_data.id = d.id_idx;// = d.gameid;
						tobj.m_data.index = d.id_ptr;
						if(d.params.t[0])
							lua_tinker::call<void>(this->m_luaState, d.name.t, tobj, d.dCurrentTime, bend, d.params.t, d.dMaxIntervalSave);
						else
							lua_tinker::call<void>(this->m_luaState, d.name.t, tobj, d.dCurrentTime, bend, d.dMaxIntervalSave);
					}
					break;
				case LuaTimeData::IDT_Control:
					{
						lua_UIControl_s temp;
						temp.m_data.t = d.id_t;
						temp.m_data.idx = d.id_idx;
						temp.m_data.ptr = d.id_ptr;
						if(d.params.t[0])
							lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.dCurrentTime, bend, d.params.t, d.dMaxIntervalSave);
						else
							lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.dCurrentTime, bend, d.dMaxIntervalSave);
					}
					break;
				case LuaTimeData::IDT_Group:
					{
						lua_UIGroup_s temp;
						temp.m_data.t = d.id_t;
						temp.m_data.idx = d.id_idx;
						temp.m_data.ptr = d.id_ptr;
						if(d.params.t[0])
							lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.dCurrentTime, bend, d.params.t, d.dMaxIntervalSave);
						else
							lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.dCurrentTime, bend, d.dMaxIntervalSave);
					}
					break;
				case LuaTimeData::IDT_Dialog:
					{
						lua_UIDialog_s temp;
						temp.m_data.t = d.id_t;
						temp.m_data.idx = d.id_idx;
						temp.m_data.ptr = d.id_ptr;
						if(d.params.t[0])
							lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.dCurrentTime, bend, d.params.t, d.dMaxIntervalSave);
						else
							lua_tinker::call<void>(this->m_luaState, d.name.t, temp, d.dCurrentTime, bend, d.dMaxIntervalSave);
					}
					break;
				case LuaTimeData::IDT_Global:
					{
						if(d.params.t[0])
							lua_tinker::call<void>(this->m_luaState, d.name.t, d.dCurrentTime, bend, d.params.t, d.dMaxIntervalSave);
						else
							lua_tinker::call<void>(this->m_luaState, d.name.t, d.dCurrentTime, bend, d.dMaxIntervalSave);
					}
					break;
				}
				if(lua_tinker::g_luaTnikerCallError)
					d.id = 0;
			}
		}
	}
	RenderNodeManager::FrameMove();
}

INT		CLuaFileMgr::AddTimeout(int millisecond, const char* name, const char* params, unsigned char _idtype, unsigned char _idt, int _idptr, int _ididx)
{
	if(millisecond < 0)
		return 0;
	if(params&&params[0])
	{
		LogInfo("system->time:timeout2(%s,%d,%s)", name, millisecond,params);
	}
	else
	{
		LogInfo("system->time:timeout(%s,%d)", name, millisecond);
	}
	for(int i=0;i<m_times.size();i++)
	{
		if(m_times[i].id == 0)
		{
			//LogInfo("system->timeout(%s).start", name);
			m_times[i].Init(LuaTimeData::TT_Timeout, m_timeID++, name, millisecond, -1, params, _idtype,_idt,_idptr,_ididx);
			return m_times[i].id;
		}
	}
	LuaTimeData d;
	d.Init(LuaTimeData::TT_Timeout, m_timeID++, name, millisecond, -1, params, _idtype,_idt,_idptr,_ididx);
	m_times.push_back(&d);
	return d.id;
}

INT		CLuaFileMgr::AddFramemove(int nMaxInterval, const char* name, const char* params, unsigned char _idtype, unsigned char _idt, int _idptr, int _ididx)
{
	for(int i=0;i<m_times.size();i++)
	{
		if(m_times[i].type==LuaTimeData::TT_FrameMove&&m_times[i].id > 0 && m_times[i].name == name&&m_times[i].id_type == _idtype&&m_times[i].id_t == _idt&&m_times[i].id_ptr == _idptr&&m_times[i].id_idx == _ididx)
		{
			if(params)
				if(!(m_times[i].params == params))
					continue;
			m_times[i].id = 0;
			break;
		}
	}
	if(params&&params[0])
	{
		LogInfo("system->time:framemove2(%s,%d,%s)", name, nMaxInterval,params);
	}
	else
	{
		LogInfo("system->time:framemove(%s,%d)", name, nMaxInterval);
	}
	for(int i=0;i<m_times.size();i++)
	{
		if(m_times[i].id == 0)
		{
			//LogInfo("system->interval(%s).start", name);
			m_times[i].Init(LuaTimeData::TT_FrameMove, m_timeID++, name, 0, nMaxInterval, params, _idtype,_idt,_idptr,_ididx);
			return m_times[i].id;
		}
	}
	LuaTimeData d;
	d.Init(LuaTimeData::TT_FrameMove, m_timeID++, name, 0, nMaxInterval, params, _idtype,_idt,_idptr,_ididx);
	m_times.push_back(&d);
	return d.id;
}
INT		CLuaFileMgr::AddInterval(int millisecond, int nMaxInterval, const char* name, const char* params, unsigned char _idtype, unsigned char _idt, int _idptr, int _ididx)
{
	if(millisecond < 0)
		return 0;
	for(int i=0;i<m_times.size();i++)
	{
		if(m_times[i].type==LuaTimeData::TT_Interval&&m_times[i].id > 0 && m_times[i].name == name&&m_times[i].id_type == _idtype&&m_times[i].id_t == _idt&&m_times[i].id_ptr == _idptr&&m_times[i].id_idx == _ididx)
		{
			if(params)
				if(!(m_times[i].params == params))
					continue;
			m_times[i].id = 0;
			break;
		}
	}
	if(params&&params[0])
	{
		LogInfo("system->time:interval2(%s,%d,%d,%s)", name, millisecond, nMaxInterval,params);
	}
	else
	{
		LogInfo("system->time:interval(%s,%d,%d)", name, millisecond, nMaxInterval);
	}
	for(int i=0;i<m_times.size();i++)
	{
		if(m_times[i].id == 0)
		{
			//LogInfo("system->interval(%s).start", name);
			m_times[i].Init(LuaTimeData::TT_Interval, m_timeID++, name, millisecond, nMaxInterval, params, _idtype,_idt,_idptr,_ididx);
			return m_times[i].id;
		}
	}
	LuaTimeData d;
	//LogInfo("system->interval(%s).start", name);
	d.Init(LuaTimeData::TT_Interval, m_timeID++, name, millisecond, nMaxInterval, params, _idtype,_idt,_idptr,_ididx);
	m_times.push_back(&d);
	return d.id;
}
VOID		CLuaFileMgr::ClearTime(int id)
{
	for(int i=0;i<m_times.size();i++)
	{
		if(m_times[i].id == id)
		{
			m_times[i].id = 0;
		}
	}
}
VOID		CLuaFileMgr::ClearTime(const char* name, const char* params)
{
	for(int i=0;i<m_times.size();i++)
	{
		if(m_times[i].name == name)
		{
			if(params)
			{
				if(!(m_times[i].params==params))
					continue;
			}
			//LogInfo("system->cleartime(%s).fromuser", name);
			m_times[i].id = 0;
		}
	}
}

VOID		CLuaFileMgr::OnInputSize(Pixel centerPos, float fSize, int nBegin)
{
	if(m_doubleMoveFunc.t[0]){
		lua_Pixel p1(centerPos.x,centerPos.y);
		lua_tinker::call<void>(this->m_luaState, m_doubleMoveFunc.t, p1, nBegin, fSize);
	}
}
VOID		CLuaFileMgr::OnInputEvent(Phantom::InputEventID btn, int x, int y, unsigned int touchIndex)
{
	g_manager.SetUIDialogSize(true, g_manager.m_gameWidth, g_manager.m_gameHeight, 0, 0);
	g_manager.SetUIDialogSize(true, g_manager.m_gameWidth, g_manager.m_gameHeight, 0, 1);
	Pixel ret;
	g_manager.GetCursorPos2D(ret, 0);
	lua_Pixel p1(ret.x, ret.y);
	Pixel ret2;
	g_manager.GetCursorPos2D(ret2, 1);
	lua_Pixel p2(ret2.x, ret2.y);
	if(btn == Phantom::InputEventID_ButtonDown)
	{
		if(g_manager.IsCursorDown(0) && g_manager.IsCursorDown(1) && m_doubleMoveFunc.t[0])
		{
			Pixel v = ret2 - ret;
			m_dblDownLength = 0;
			if((v.x+v.y)!=0)
				m_dblDownLength = sqrtf(v.x*v.x+v.y*v.y);
			m_bDoubleDown = true;
			OnInputSize(Pixel((ret.x+ret2.x)/2, (ret.y+ret2.y)/2), 1.0f, 0);
		}
		else if(this->m_downFunc.t[0])
		{
			m_bOneDown = true;
			lua_tinker::call<void>(this->m_luaState, m_downFunc.t, p1);
			m_lastTouchIndex = touchIndex;
		}
	}
	else if(btn == Phantom::InputEventID_MouseMove)
	{
		if(g_manager.IsCursorDown(0)&&g_manager.IsCursorDown(1)&&m_doubleMoveFunc.t[0]&&m_bDoubleDown&&m_dblDownLength>0)
		{
			Pixel v = ret2 - ret;
			float dblDownLength = 0;
			if((v.x+v.y)!=0)
				dblDownLength = sqrtf(v.x*v.x+v.y*v.y);
			OnInputSize(Pixel((ret.x+ret2.x)/2, (ret.y+ret2.y)/2), dblDownLength/m_dblDownLength, 1);
		}
		else if(this->m_moveFunc.t[0] && touchIndex == m_lastTouchIndex)
			lua_tinker::call<void>(this->m_luaState, m_moveFunc.t, p1);
	}
	else if(btn == Phantom::InputEventID_ButtonUp)
	{
		if(m_bDoubleDown && m_doubleUpFunc.t[0])
		{
			//lua_tinker::call<void>(this->m_luaState, m_doubleUpFunc.t, p1, p2);
		}
		if(this->m_upFunc.t[0] && touchIndex == m_lastTouchIndex&&m_bOneDown)
			lua_tinker::call<void>(this->m_luaState, m_upFunc.t, p1);
		m_bDoubleDown = false;
		if(touchIndex == m_lastTouchIndex)
			m_bOneDown = false;
		m_lastTouchIndex = -1;
	}
}

int		WINAPI	Global_round(float d){
	float d2 = fmodf(d, 1);
	if(d2 < 0.5f)
		return (int)d;
	return ((int)d+1);
}
int		WINAPI	Global_abs(double n){if(n>0)return n;return -n;}
int		WINAPI	Global_symbol(double n){if(n>0)return 1;return -1;}
int		WINAPI	Global_atoi(const char* v){if(!v)return 0;return atoi(v);}
int		WINAPI	Global_int(double d){return (int)d;}
/*API*/BOOL			WINAPI	Global_isstr/*全局_A文本中是否存在这段B文本*/(LPCSTR a, LPCSTR b)
{
	if(!a||!b)return false;
	return (strstr(a,b)!=0);
}
/*API*/float		WINAPI	Global_rand/*全局_取得随机数*/(float fmin,float fmax)
{
	return RandomF(fmin, fmax);
}
BOOL	WINAPI	Global_infile(LPCSTR szFile){if(!szFile)return false; return (g_luamanager.m_mainName==szFile);}
/*API*/BOOL			WINAPI	Global_insystem/*全局_是否在某个系统中*/(LPCSTR szSystem)//是否在某个系统中，参数"windows","ios","android"
{
	if(stricmp(szSystem, "ios") == 0)
		return (g_manager.GetOSType() == OSType_IPhone||g_manager.GetOSType() == OSType_IPad);
	if(stricmp(szSystem, "android") == 0)
		return (g_manager.GetOSType() == OSType_Android);
	if(stricmp(szSystem, "windows") == 0)
		return (g_manager.GetOSType() == OSType_Windows);
	return false;
}


/*API*/void	WINAPI	Global_jbind(int id, LPCSTR szFunc)
{
	if(id < 0)return;
	for(int i=0;i<g_luamanager.m_luaBindIDs.size();i++)
	{
		if(g_luamanager.m_luaBindIDs[i].id == id)
		{
			g_luamanager.m_luaBindIDs[i].name = szFunc;
			return;
		}
	}
	CLuaFileMgr::LuaBindName lb;
	lb.id = id;
	lb.name = szFunc;
	g_luamanager.m_luaBindIDs.push_back(lb);
}
LPCSTR	WINAPI	Global_jget(LPCSTR szName)
{
	if(!g_luamanager.m_luaJson)
		return "";
	return g_luamanager.m_luaJson->operator[](szName);
}
int	WINAPI	Global_jgeti(LPCSTR szName)
{
	if(!g_luamanager.m_luaJson)
		return 0;
	return g_luamanager.m_luaJson->operator[](szName);
}
float	WINAPI	Global_jgetf(LPCSTR szName)
{
	if(!g_luamanager.m_luaJson)
		return 0;
	return atof(g_luamanager.m_luaJson->operator[](szName));
}
VOID	WINAPI	Global_jset(LPCSTR szName, LPCSTR szValue)
{
	g_luamanager.m_luaSend[szName] = szValue;
}
VOID	WINAPI	Global_jseti(LPCSTR szName, int nValue)
{
	g_luamanager.m_luaSend[szName] = nValue;
}
VOID	WINAPI	Global_jsetf(LPCSTR szName, float fValue)
{
	sprintf(g_luamanager.m_luaSend[szName].svalue.t, "%.03f", fValue);
}

VOID	WINAPI	Global_jclear(LPCSTR szName, float fValue)
{
	g_luamanager.m_luaSend.Reset();
}
void			WINAPI	Global_jinit/*全局_初始化json设置http地址*/(LPCSTR szHttpInPhpFile)
{
	g_luamanager.m_http.Initialize(szHttpInPhpFile);
	g_luamanager.m_http.SetUseThread(true);
}
VOID	WINAPI	Global_jrequest()
{
	g_luamanager.m_http.call(g_luamanager.m_luaSend);
	g_luamanager.m_luaSend.Reset();
}
//*API*/VOID			WINAPI	Global_payment/*全局_购买产品*/(LPCSTR szBuyName/*购买名称*/, LPCSTR szFunc/*购买反馈函数*/);//反馈有一个参数,1表示成功,0表示失败,失败情况可查看调试输出,事件[szFunc(szBuyName,val)--(szBuyName=string,val=int)val是0或1,1表示成功]
//*API*/VOID			WINAPI	Global_payment_restore/*全局_购买恢复*/(LPCSTR szBuyName/*购买名称*/, LPCSTR szFunc/*恢复反馈函数*/);//反馈有一个参数,1表示成功,0表示失败,失败情况可查看调试输出,事件[szFunc(szBuyName,val)--(szBuyName=string,val=int)val是0或1,1表示成功]

extern "C"{
    extern	void    showAd(int x, int y, int type, int bShow);
    extern	void    shopBuy(const char* buyName);
	extern	void    shopRestore(const char* buyName);
	extern	void	gotoComment();
	extern	void	openUrl(const char* url);
	const char*		call_lua(const char* type, const char* params, const char* params2)//调用lua内部的函数oncall
	{
        if(g_luamanager.GetLuaState())
            lua_tinker::call<void>(g_luamanager.GetLuaState(), "onmessage", type, params, params2);
		return 0;
	}
	extern	void	onVibrator(float fSecond);
	extern	void	openRank(const char* rank);
	extern	void	setRankScore(const char* rank, int value);
	extern	void	setAchievement(const char* achievement, int persent);
	extern	const char*	onAlert(const char* message, const char* title, const char* type);
	int		OnAndroidBack()
	{
        if(g_luamanager.GetLuaState())
		{
            int ret = lua_tinker::call<int>(g_luamanager.GetLuaState(), "onback");
			return ret;
		}
		return 0;
	}
};
/*API*/VOID			WINAPI	System_vibrator/*全局_振动器*/(float fSecond/*秒*/)
{
	onVibrator(fSecond);
}
/*API*/LPCSTR		WINAPI	Global_alert/*全局_消息框*/(LPCSTR szMessage, LPCSTR szTitle, LPCSTR szType)
{
	if(!szTitle || !szTitle[0])szTitle = "message";
	if(!szType || !szType[0])szType = "ok";
	return onAlert(szMessage, szTitle, szType);
}

BOOL g_bShowBanner = false;
Pixel g_bannerPos(0, 0);
/*API*/VOID			WINAPI	Global_ad_show/*全局_显示广告条*/(INT nType/*类型*/, BOOL bShow/*显示*/)
{
	if(nType == 0)
		g_bShowBanner = bShow;
	showAd(g_bannerPos.x, g_bannerPos.y, nType, bShow);
}
/*API*/VOID			WINAPI	Global_ad_setpos/*全局_设置显示位置*/(INT x/*x*/, INT y/*y*/)
{
	g_bannerPos.x = x;
	g_bannerPos.y = y;
	if(g_bShowBanner)
		showAd(g_bannerPos.x, g_bannerPos.y, 0, g_bShowBanner);
}
VOID				WINAPI	Global_openurl(LPCSTR url){
	openUrl(url);
}
VOID				WINAPI	Global_gotocomment()
{
	gotoComment();
}
/*API*/VOID			WINAPI	Global_openrank/*全局_显示排行榜*/(LPCSTR rank)
{
	openRank(rank);
}
/*API*/VOID			WINAPI	Global_rankscore/*全局_设置排行榜值*/(LPCSTR rank, int nValue)
{
	setRankScore(rank, nValue);
}
/*API*/VOID			WINAPI	Global_achievement/*全局_设置取得成就*/(LPCSTR achievement, int persent)
{
	setAchievement(achievement, persent);
}
struct RandItem{
	int		val;
	int		weight;//权重，权重越高则越有可能排到最前面
};
CDynamicArray<RandItem>	g_randomTables;
VOID	WINAPI	Global_random_init(int number)
{
	g_randomTables.SetArrayCount(number);
	for(int i=0;i<g_randomTables.size();i++)
	{
		g_randomTables[i].val = i;
		g_randomTables[i].weight = 1;
	}
}
VOID	WINAPI	Global_random_weight(int index, int weight)
{
	if(index<0||index>=g_randomTables.size())return;
	g_randomTables[index].weight = weight;
}
VOID	WINAPI	Global_random_do()
{
	CDynamicArray<RandItem> randoms = g_randomTables;
	int maxnum = 0;
	for(int i=0;i<randoms.size();i++)
		maxnum += randoms[i].weight;
	int number = randoms.size();
	for(int i=0;i<number;i++)
	{
		int index = 0;
		int inum = (int)(RandomF(0, 0.999999f) * maxnum);
		for(int j=0;j<randoms.size();j++)
		{
			if(inum<randoms[j].weight)
			{
				index = j;
				break;
			}
			inum -= randoms[j].weight;
		}
		maxnum -= randoms[index].weight;
		g_randomTables[i] = randoms[index];
		randoms.eraseByIndex(index, 1, true);
	}
}
VOID	WINAPI	Global_random_table(int number)
{
	CDynamicArray<RandItem> randoms;
	randoms.SetArrayCount(number);
	for(int i=0;i<randoms.size();i++)
	{
		randoms[i].val = i;
		randoms[i].weight = 1;
	}
	g_randomTables.SetArrayCount(number);
	for(int i=0;i<number;i++)
	{
		int index = (int)(RandomF(0, 0.999999f) * (float)randoms.size());
		g_randomTables[i] = randoms[index];
		randoms.eraseByIndex(index, 1, true);
	}
}

int		WINAPI	Global_random_get(int b)
{
	if(b<0|| b>=g_randomTables.size())
		return 0;
	return g_randomTables[b].val;
}

/*API*/VOID			WINAPI	Global_sendmessage/*全局_向核心底层发送消息*/(LPCSTR szName/*消息名称*/, LPCSTR szParams/*消息参数1*/, LPCSTR szParams2/*消息参数2*/)
{
	if(stricmp(szName, "buy")==0){
		shopBuy(szName);
	}
	else if(stricmp(szName, "restore")==0){
		shopRestore(szName);
	}
	else{
		FromLuaMessage(szName, szParams, szParams2);
	}
//#ifdef WIN32_OS
//	lua_tinker::call<void>(g_luamanager.GetLuaState(), "onmessage", szName, "1", "");
//#endif
}

//std::map<std::string, std::string> g_shopfunc;
//std::map<std::string, std::string> g_shopfuncR;
//
///*API*/VOID			WINAPI	Global_payment/*全局_购买*/(LPCSTR szBuyName/*购买名称*/, LPCSTR szFunc/*购买反馈函数*/)//反馈有一个参数,1表示成功,0表示失败,失败情况可查看调试输出
//{
//	if(szFunc)
//		g_shopfunc[szBuyName] = szFunc;
//	shopBuy(szBuyName);
//#ifdef WIN32_OS
//	if(szFunc)
//		lua_tinker::call<void>(g_luamanager.GetLuaState(), szFunc, szBuyName, 1);
//#endif
//}
///*API*/VOID			WINAPI	Global_payment_restore/*全局_购买恢复*/(LPCSTR szBuyName/*购买名称*/, LPCSTR szFunc/*恢复反馈函数*/)
//{
//	if(szFunc)
//		g_shopfuncR[szBuyName] = szFunc;
//	shopRestore(szBuyName);
//#ifdef WIN32_OS
//	if(szFunc)
//		lua_tinker::call<void>(g_luamanager.GetLuaState(), szFunc, szBuyName, 1);
//#endif
//}


/*API*/VOID			WINAPI	Global_c_add/*全局_统计计数器加1*/(LPCSTR szName/*计数器*/)
{
	int nVal = GetStateMgr()->readi(szName);
	GetStateMgr()->writei(szName, nVal+1);
}
/*API*/VOID			WINAPI	Global_c_set/*全局_统计计数器设置*/(LPCSTR szName/*计数器*/, INT nValue/*值*/)
{
	GetStateMgr()->writei(szName, nValue);
}
/*API*/INT			WINAPI	Global_c_get/*全局_统计计数器返回*/(LPCSTR szName/*计数器*/)
{
	return GetStateMgr()->readi(szName);
}

int luaListVars (lua_State *L, int level) {
	lua_Debug ar;
	int i;
	const char *name;
	if (lua_getstack(L, level, &ar) == 0)
		return 0;  /* failure: no such level in the stack */
	i = 1;
	while ((name = lua_getlocal(L, &ar, i++)) != NULL) {
		LogInfo("local %d %s/n", i-1, name);
		lua_pop(L, 1);  /* remove variable value */
	}
	lua_getinfo(L, "f", &ar);  /* retrieves function */
	i = 1;
	while ((name = lua_getupvalue(L, -1, i++)) != NULL) {
		LogInfo("upvalue %d %s/n", i-1, name);
		lua_pop(L, 1);  /* remove upvalue value */
	}
	return 1;
}
void MyLuaHook(lua_State *L, lua_Debug *ar)
{
	//lua_Debug arinfo;
	//lua_pushstring(L, "f");
	//lua_gettable(L, LUA_GLOBALSINDEX);  /* get global `f' */
	//lua_getinfo(L, ">S", &arinfo);
	//LogInfo("%d/n", arinfo.linedefined);
	//int index = 0;
	//while(luaListVars(L, index)>0){
	//	index++;
	//}
	//if(ar->currentline==47)
		//lua_yield(L, 0);
}
streamRead		g_socketRead;
streamWrite		g_socketWrite;
Name			g_fnLink;
Name			g_fnDislink;
Name			g_fnMessage;
VOID	OnLuaLink(DWORD linkid)//某个客户端连接时被调用
{
	if(g_fnLink.t[0])
		lua_tinker::call<void>(g_luamanager.GetLuaState(), g_fnLink.t, linkid, true);
}
VOID	OnLuaDislink(DWORD linkid)//某个客户端断开连接时被调用
{
	if(g_fnDislink.t[0])
		lua_tinker::call<void>(g_luamanager.GetLuaState(), g_fnDislink.t, linkid);
}
VOID	OnLuaMessage(DWORD linkid, WORD wID, const char* dataPtr, unsigned int wSize)//某个客户端处理消息时被调用
{
	if(wID == MsgID_cs_ServerInfo)
	{
		Send(linkid, wID, (const char*)&g_server->m_info, sizeof(ServerInfo));
		return;
	}
	g_socketRead.m_ptr = dataPtr;
	g_socketRead.m_point = 0;
	g_socketRead.m_length = wSize;
	if(g_fnMessage.t[0])
		lua_tinker::call<void>(g_luamanager.GetLuaState(), g_fnMessage.t, linkid, wID);
}
Name	g_fnSearch;
VOID	OnLuaSearchComplete(const char* ip)
{
	if(ip){//count>0){
		//for(int i=0;i<count;i++)
		{
			if(g_fnSearch.t[0])
			{
				//char buf[256];
				//sprintf(buf, "%s,%s,%s,%d", servers[i].name.t, servers[i].ip.t, servers[i].state.t, servers[i].players);
				lua_tinker::call<void>(g_luamanager.GetLuaState(), g_fnSearch.t, ip);
			}
		}
	}
	else
	{
		if(g_fnSearch.t[0])
			lua_tinker::call<void>(g_luamanager.GetLuaState(), g_fnSearch.t, "");
	}
}
VOID	OnLuaDownloadComplete(const char* requestText, const char* indocFile, BOOL bError, char* buffer, int length)
{
	lua_tinker::call<void>(g_luamanager.GetLuaState(), "ondownload", indocFile, bError?"fail":"ok");
}
/*API*/VOID			WINAPI	Global_download/*全局从网络下载文件*/(LPCSTR httpRequest/*请求信息*/, LPCSTR inDocFile/*文档中的文件相对路径*/)
{
	downloadFile(httpRequest, inDocFile, OnLuaDownloadComplete, true, false);
}
/*API*/void			WINAPI	Global_net_server_max_links/*全局_net设置服务器最大可连接客户端数量*/(INT nMaxLinks)
{
	if(g_server)
		g_server->SetMaxLinks(nMaxLinks);
}
/*API*/void			WINAPI	Global_net_onlink/*全局_netOnLink*/(LPCSTR szFunc/*连接时触发*/)
{
	if(!szFunc)szFunc = "";
	g_fnLink = szFunc;
}
/*API*/void			WINAPI	Global_net_ondislink/*全局_netOnDislink*/(LPCSTR szFunc/*断开连接时触发*/)
{
	if(!szFunc)szFunc = "";
	g_fnDislink = szFunc;
}
/*API*/void			WINAPI	Global_net_onmessage/*全局_netOnMessage*/(LPCSTR szFunc/*消息接收时触发*/)
{
	if(!szFunc)szFunc = "";
	g_fnMessage = szFunc;
}

/*API*/void			WINAPI	Global_net_init/*全局_net初始化*/(int dwGUID)
{
	SetupSocket(OnLuaLink, OnLuaDislink, OnLuaMessage, dwGUID);
	g_socketWrite.buffers.SetArrayCount(SOCKET_PACKAGE_MAX);
	g_socketWrite.m_length	=	g_socketWrite.buffers.size();
	g_socketWrite.m_ptr		=	g_socketWrite.buffers.GetPtr();
}
/*API*/BOOL			WINAPI	Global_net_server/*全局_net启动服务器*/(LPCSTR name, int port)
{
	if(port == 0)return false;
	return StartServer(name, port);
}
/*API*/void			WINAPI	Global_net_search/*全局_net搜索服务器*/(LPCSTR _onsearch, int port)
{
	if(port == 0)return;
	g_fnSearch = _onsearch;
	SearchServers(OnLuaSearchComplete, port);
}
/*API*/void			WINAPI	Global_net_server_state/*全局_net设置服务器状态*/(LPCSTR state)
{
	if(g_server)
		g_server->SetState(state);
}
BOOL	g_bIsConnectedSearch = false;
int		g_nConnectedSearchPort = 0;
VOID	OnLuaSearchComplete2(const char* ip)
{
	if(ip&&!g_bIsConnectedSearch){
		StartClient(ip, g_nConnectedSearchPort, OnLuaLink, OnLuaDislink, OnLuaMessage, 0);
	}
}
/*API*/int			WINAPI	Global_net_connect/*全局_net启动客户端*/(LPCSTR szIP, int port)
{
	//if(g_bIsConnectedSearch)return;
	//if(!szIP||!szIP[0]){
	//	g_bIsConnectedSearch = false;
	//	g_nConnectedSearchPort = port;
	//	SearchServers(OnLuaSearchComplete2, port);
	//	return;
	//}
	return StartClient(szIP, port, OnLuaLink, OnLuaDislink, OnLuaMessage, 0);
}
/*API*/void			WINAPI	Global_net_close/*全局_net关闭连接*/(INT linkid)
{
	CloseLink(linkid);
}
/*API*/void			WINAPI	Global_net_stopall/*全局_net关闭所有服务器和客户端*/()
{
	StopAllSocket();
}
/*API*/LPCSTR		WINAPI	Global_net_get/*全局_net读取文本*/()
{
	char ch = 0;
	g_socketRead >> ch;
	if(ch != 1)
	{
		LogError("错误，sget在非文本类型时读取文本，请仔细检查代码");
		return "";
	}
	WORD wLength = 0;
	g_socketRead >> wLength;
	if(wLength <= 0)
		return "";
	static CDynamicArray<char> str;
	str.SetArrayCount(wLength);
	g_socketRead.read(str.GetPtr(), wLength);
	return str.GetPtr();
}
/*API*/int			WINAPI	Global_net_geti/*全局_net读取整数*/()
{
	char ch = 0;
	g_socketRead >> ch;
	if(ch != 2)
	{
		LogError("错误，sgeti在非整数类型时读取整数，请仔细检查代码");
		return 0;
	}
	int val = 0;
	g_socketRead >> val;
	return val;
}
/*API*/float		WINAPI	Global_net_getf/*全局_net读取浮点*/()
{
	char ch = 0;
	g_socketRead >> ch;
	if(ch != 3)
	{
		LogError("错误，sgetf在非浮点类型时读取浮点，请仔细检查代码");
		return 0;
	}
	float f = 0;
	g_socketRead >> f;
	return f;
}
/*API*/VOID			WINAPI	Global_net_begin/*全局_net清除缓存数据*/()
{
	g_socketWrite.m_point = 0;
}
/*API*/VOID			WINAPI	Global_net_set/*全局_net写入文本*/(LPCSTR szValue)
{
	WORD wLength = 0;
	if(szValue)
	{
		char ch = 1;
		g_socketWrite << ch;
		wLength = strlen(szValue)+1;
		g_socketWrite << wLength;
		g_socketWrite.write(szValue, wLength);
	}
}
/*API*/VOID			WINAPI	Global_net_seti/*全局_net写入整数*/(int nValue)
{
	char ch = 2;
	g_socketWrite << ch;
	g_socketWrite << nValue;
}
/*API*/VOID			WINAPI	Global_net_setf/*全局_net写入浮点*/(float fValue)
{
	char ch = 3;
	g_socketWrite << ch;
	g_socketWrite << fValue;
}
/*API*/VOID			WINAPI	Global_net_send/*全局_net发送数据*/(INT linkid, short msgid)
{
	Send(linkid, msgid, g_socketWrite.m_point > 0 ? g_socketWrite.m_ptr : 0, g_socketWrite.m_point);
	g_socketWrite.m_point = 0;
}

VOID		CLuaFileMgr::OnJsonAnalyse(JsonObject& obj)
{
	if(!obj.exist("id"))
	{
		LogInfo("json:not exist id");
		return;
	}
	m_luaJson = &obj;
	int id = obj["id"];
	for(int i=0;i<this->m_luaBindIDs.size();i++)
	{
		if(m_luaBindIDs[i].id == id)
		{
			if(m_luaBindIDs[i].name.t[0])
				lua_tinker::call<void>(GetLuaState(), m_luaBindIDs[i].name.t);
			m_luaJson = 0;
			return;
		}
	}
	m_luaJson = 0;
}
VOID		CLuaFileMgr::OnJsonAnalyseFail(const char* type, const char* szFailText)
{
	LogInfo("json:%s", szFailText);
}

BOOL		CLuaFileMgr::Create(BOOL bUsePhantomSystem, BOOL bDebugMode)	//开始运行脚本系统，会重新初始化所有lua脚本，也就是重置所有代码
{
	if(m_luaState)
		return false;
	m_funcRender.t[0] = 0;
	if(m_luaState)
		lua_close(m_luaState);
	m_luaState	=	lua_open();
	luaopen_base(m_luaState);
	if(bDebugMode)
	{
		//luaopen_debug(m_luaState);
		//lua_sethook(m_luaState, MyLuaHook, LUA_MASKLINE, 1);
	}
	luaopen_table(m_luaState);
	luaopen_string(m_luaState);
	luaopen_math(m_luaState);
	if(bUsePhantomSystem){
		initPhantomEnum(m_luaState);
		initPhantomMath(m_luaState);
	}
	//lua_tinker::def(m_luaState, "callt", CLuaFileMgr::CallMyFunc);
	//lua_tinker::def(m_luaState, "dtoi", DtoI);
	//lua_tinker::def(m_luaState, "int", DtoI);
	//lua_tinker::def(m_luaState, "abs", ABS);
	//lua_tinker::def(m_luaState, "atoi", CTOI);
	//lua_tinker::def(m_luaState, "symbol", Symbol);
	//lua_tinker::def(m_luaState, "round", Round);
	//lua_tinker::def(m_luaState, "infile", InMainFile);
	//lua_tinker::def(m_luaState, "utf8", utf8);
	return true;
}
/*API*/BOOL			WINAPI	Global_include/*全局_载入脚本文件*/(LPCSTR szFile/*脚本文件*/)
{
	char buf[256];
	sprintf(buf, "sc/%s", szFile);
	CDynamicArray<char> str;
	if(!LoadUtf8(buf, str))
		return false;
	return g_luamanager.loadAndExecute(str.GetPtr(), buf);
}
BOOL		CLuaFileMgr::loadAndExecute(const char* szLuaCode, const char* szFile)	//添加脚本代码,文件名可以不是脚本
{
	if(!m_luaState)
		return false;
	FileName f = szFile;
	f.setIsFile();
	for(int i=0;i<m_files.size();i++)
	{
		if(m_files[i]->m_file == f)
			return true;
	}
	CLuaFile* main = new CLuaFile();
	if(!main->Load(szLuaCode, f))
	{
		delete main;
		return 0;
	}
	m_files.push_back(main);
	m_runlist.push_back(main);
	LogInfo("system->:Run(%s)", szFile);
	for(int i=0;i<m_runlist.size();i++)
	{
		if(!m_runlist[i]->isExcuted){
			m_runlist[i]->isExcuted = true;
			LogInfo("system->import(%s)", m_runlist[i]->m_file.t);
			call(m_runlist[i]->m_script.c_str(), m_runlist[i]->m_file.t);
		}
	}
	return true;
}
BOOL		CLuaFileMgr::callFile(const char* szMainFile)
{
	if(!m_luaState || !szMainFile)
		return false;
	for(int i=0;i<m_defines.size();i++)
		delete m_defines[i];
	m_defines.clear();
	for(int i=0;i<m_files.size();i++)
		delete m_files[i];
	m_files.clear();
	m_runlist.clear();
	//
	CLuaFile* main = Load(szMainFile);
	if(!main)
		return false;
	LogInfo("system->:Run(%s)", szMainFile);
	FileName f = szMainFile;
	f.setIsFile();
	FileName file;
	f.splitpath(0, &file, 0);
	m_mainName = file.t;
	for(int i=0;i<m_runlist.size();i++)
	{
		if(!m_runlist[i]->isExcuted){
			m_runlist[i]->isExcuted = true;
			LogInfo("system->import(%s)", m_runlist[i]->m_file.t);
			call(m_runlist[i]->m_script.c_str(), m_runlist[i]->m_file.t);
		}
	}
	lua_pushstring(m_luaState, "dbg");
	lua_pushcfunction(m_luaState, break_point);
	lua_settable(m_luaState, LUA_GLOBALSINDEX);
	return true;
}

BOOL		CLuaFileMgr::call(const char* szLuaCode, const char* szFile)
{
	if(!m_luaState)return false;
	strcpy(g_currLuaFile, szFile);
	if(luaL_loadstring(m_luaState, szLuaCode) != 0)
	{
		const char* error = lua_tostring(m_luaState, -1);
		LogInfo("error->[%s] in file(%s)", error, szFile);//, szLuaCode);
		return false;
	}
	if(lua_pcall(m_luaState, 0, LUA_MULTRET, 0) != 0)
	{
		const char* error = lua_tostring(m_luaState, -1);
		LogInfo("error->call(%s) in file(%s)", error, szFile);
		return false;
	}
	return true;
}

VOID		CLuaFileMgr::Push(const PtrID& id)
{
	LuaCreat l;
	l.id = id;
	l.fileIndex = -1;
	m_creats.push_back(l);
}
VOID		CLuaFileMgr::Push(const SoundID& id)
{
	LuaCreat l;
	l.id.id = id.rid;
	l.id.index = id.index;
	l.fileIndex = id.fileIndex;
	m_creats.push_back(l);
}
VOID		CLuaFileMgr::Pop(const PtrID& id)
{
	for(int i=0;i<m_creats.size();i++)
	{
		if(m_creats[i].id.id == id.id)
		{
			m_creats.eraseByIndex(i);
			return;
		}
	}
}
VOID		CLuaFileMgr::Pop(const SoundID& id)
{
	for(int i=0;i<m_creats.size();i++)
	{
		LuaCreat& c = m_creats[i];
		if(c.fileIndex == id.fileIndex && c.id.id == id.rid)
		{
			m_creats.eraseByIndex(i);
			return;
		}
	}
}

};

VOID	CallOnTest()
{
	lua_tinker::call<void>(g_luamanager.GetLuaState(), "ontest");
}


