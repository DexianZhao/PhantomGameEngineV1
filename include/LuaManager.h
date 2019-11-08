#ifndef __LUA_MANAGER_H__
#define	__LUA_MANAGER_H__
#include "PhantomManager.h"
#include "PhantomEffect.h"
#include "PhantomAction.h"
#include "PhantomHttp.h"

extern "C"
{
	#include "lua/lua.h"
	#include "lua/lualib.h"
	#include "lua/lauxlib.h"
};
#include "lua/lua_tinker.h"

namespace Phantom{


class GameState;
class CLuaFile;
class CLuaFile
{
public:
	CLuaFile();
	~CLuaFile();
	BOOL					Load(FileName& f);//从文件载入
	BOOL					Load(const char* szText, FileName& f);//从文件载入
	BOOL					LoadText(const char* szText, int len);//从文本载入
	char					isExcuted;
protected:
	friend class			CLuaFileMgr;
	std::string				m_script;
	FileName				m_file;
};

struct LuaDefine
{
	Name					name;
	std::string				text;
};

class	CLuaCallback: public Base
{
public:
	ImplementComValue();
	virtual	const char*		OnCall(Name& methods, LPCSTR* params, int numParams) = 0;//从lua内部调用自定义函数，这个函数中全部都是字符串参数
};

struct	LuaCreat
{
	int					fileIndex;
	PtrID				id;
};
struct	LuaTimeData
{
	enum TimeType{
		TT_Timeout	=	0,
		TT_Interval =	1,
		TT_FrameMove=	2,
	};
	char				type;
	int					id;
	char				pause;
	Name				name;
	NameT<32>			params;
	double				dTime;
	double				dInitTime;//
	double				dMaxInterval;
	double				dMaxIntervalSave;
	double				dCurrentTime;
	//
	unsigned char		id_type;
	unsigned char		id_t;
	INT					id_ptr;
	INT					id_idx;
	enum{
		IDT_Global		=	0,//无任何对象的全局
		IDT_GameID		=	1,//游戏对象ID,GameObject
		IDT_Control		=	2,
		IDT_Group		=	3,
		IDT_Dialog		=	4,
	};
	VOID				Init(TimeType _type, int _id, const char* _name, int millisecond, int nMaxInterval = -1, const char* params = 0, unsigned char _idtype = 0, unsigned char _idt = 0, int _idptr = 0, int _ididx = 0);
};

class LuaAction;
struct ActionVal
{
	float4				next;
	double				dTime;
	int					loop;
	int					currentloop;
};
class LuaAction: public Base
{
public:
	LuaAction();
	~LuaAction();
	VOID				OnFrameMove(double dElapsedTime);
	VOID				AddNext(const float4& end, float fTimeLength, int loopCount);
	BOOL				m_bEnabled;
	double				m_dLastTime;
public:
	CDynamicArray<ActionVal>	m_vals;
	int					m_currentIndex;
	double				m_dTime;
	Name				m_luaFunc;
};
class CLuaState{
public:
	lua_State*			m_luaState;
};

class CLuaFileMgr: public RenderNodeManager
{
public:
	CLuaFileMgr();
	~CLuaFileMgr();
	VOID				Destroy();//重置所有的lua设定,可以重新调用Create
	BOOL				Create(BOOL bUsePhantomSystem, BOOL bDebugMode);	//开始运行脚本系统，不会重新初始化
	BOOL				callFile(const char* szMainFile);	//运行某个文件
	BOOL				call(const char* szLuaCode, const char* szFile);	//调用代码
	VOID				SetCallback(CLuaCallback* cb);
	inline	lua_State*	GetLuaState(){return m_luaState;}
	//
	int					GetFileIndex(const char* file);
	const char*			GetFileName(int index);
	//
	BOOL				loadAndExecute(const char* szLuaCode, const char* szFile);	//添加脚本代码,文件名可以不是脚本
	//
	VOID				OnJsonAnalyse(JsonObject& obj);
	VOID				OnJsonAnalyseFail(const char* type, const char* szFailText);
	VOID				Push(const PtrID& id);
	VOID				Push(const SoundID& id);
	VOID				Pop(const PtrID& id);
	VOID				Pop(const SoundID& id);
	VOID				Push(LuaAction*a);
	VOID				Pop(LuaAction*a);
	//
	INT					AddTimeout(int millisecond, const char* name, const char* params = 0, unsigned char _idtype = 0, unsigned char _idt = 0, int _idptr = 0, int _ididx = 0);
	INT					AddInterval(int millisecond, int nMaxMillisecond, const char* name, const char* params = 0, unsigned char _idtype = 0, unsigned char _idt = 0, int _idptr = 0, int _ididx = 0);
	INT					AddFramemove(int nMaxMillisecond, const char* name, const char* params = 0, unsigned char _idtype = 0, unsigned char _idt = 0, int _idptr = 0, int _ididx = 0);
	VOID				ClearTime(const char* name, const char* params);
	VOID				ClearTime(int id);
	VOID				ClearTimeAll(){m_times.SetArrayCount(0);}
	VOID				BreakTime();
	VOID				PauseTime(BOOL bPause);
	//
	VOID				OnFrameMove(double dElapsedTime, double dRealElapsedTime);
	VOID				OnRender(float dElapsedTime);
	//鼠标事件
	VOID				OnInputEvent(Phantom::InputEventID btn, int x, int y, unsigned int touchIndex);
	VOID				OnInputSize(Pixel centerPos, float fSize, int nBegin);
	Name				m_downFunc;
	Name				m_upFunc;
	Name				m_moveFunc;
	Name				m_mainName;
	Name				m_doubleDownFunc;
	Name				m_doubleMoveFunc;
	Name				m_doubleUpFunc;
	BOOL				m_bDoubleDown;
	BOOL				m_bOneDown;
	float				m_dblDownLength;
	CDynamicArray<LuaAction*>	m_actions;
	CLuaCallback*		m_callback;

	HttpRequest			m_http;
	struct LuaBindName{
		int				id;
		Name			name;
	};
	CDynamicArray<LuaBindName>	m_luaBindIDs;
	JsonObject*			m_luaJson;
	JsonObject			m_luaSend;
	int					m_lastTouchIndex;
	GameState*			m_memvals;
	Name				m_funcRender;
protected:
	int					m_currentTime;
	lua_State*			m_luaState;
	friend class CLuaFile;
	VOID				AddMacroText(const char* szName, const char* szText);	//添加宏代码
	CLuaFile*			Load(const char* szFile);//如果文件存在则直接返回true，不会重复载入同一个文件
	CDynamicArray<LuaDefine*>	m_defines;//lua宏
	CDynamicArray<CLuaFile*>	m_files;//lua文件列表
	CDynamicArray<CLuaFile*>	m_runlist;//运行列表
	CDynamicArray<LuaCreat>		m_creats;
	CDynamicArray<LuaTimeData>	m_times;
	int							m_timeID;
};
extern	CLuaFileMgr		g_luamanager;
}
extern	void	SetCurrentLuaFileIndex(int n);

#endif
