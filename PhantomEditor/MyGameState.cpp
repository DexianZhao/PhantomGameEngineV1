/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "MyGameState.h"
#include "GameManager.h"
#include "PhantomDB.h"

extern VOID	OnViewFrameMove();
extern VOID	OnViewRender();
extern VOID	OnViewRenderUI();

MyGameState* g_gameState = 0;
MyGameState::MyGameState()
{
	g_gameState = this;
}

MyGameState::~MyGameState(void)
{
	if(g_gameState == this)
		g_gameState = 0;
}

BOOL	MyGameState::OnFrameMove(double dElapsedTime)
{
	if(m_nextLua.t[0])
	{
		//只有在脚本中跳转的时候才会优化载入，其他时候必须重新载入，便于调试
		CDynamicArray<Texture*> textures;
		for(int i=0;i<g_manager.GetTextureCount();i++){
			Texture* tex = g_manager.GetTexturePtr(i);
			textures.push_back(tex);
			tex->AddRef();
		}
		ResetLua(m_nextLua.t);
		lua_tinker::call<void>(g_luamanager.GetLuaState(), "oninit");
		m_nextLua.t[0] = 0;
		//
		for(int i=0;i<textures.size();i++)
			safe_release(textures[i]);
	}
	OnViewFrameMove();
	return true;
}
const char*	GetDeviceID(){//
	return "25233437E5B6434ABB7E59D0C3972892";
}

VOID			MyGameState::OnCreate()
{
	LogInfo("MyGameState::OnCreate");
	g_manager.SetDefaultUnicodeFont("default_init_zip/fonts/cn.fig");
	char szFile[256];
	strcpy(szFile, GetGameManager()->m_defaultLua);//"sc/main.lua");
	//if(!fexist(szFile, 0)){
//		StreamBase* s = open_s("sc/config.txt");
//		char buf[258];
//		if(!s)
//			strcpy(szFile, GetGameManager()->m_defaultLua);//"sc/main.lua");
//		else
//		{
//			int len = s->read(buf, 256);
//			buf[len] = 0;
//			for(int i=0;i<len;i++)
//			{
//				if(buf[i]=='\r'||buf[i]=='\n')
//					buf[i] = 0;
//			}
//			sprintf(szFile, "sc/%s", buf);
//			close_s(&s);
//		}
//		LogInfo("MainLua(%s)", szFile);
	//}
	ResetSmallImage();
	ResetLua(szFile);
	lua_tinker::call<void>(g_luamanager.GetLuaState(), "oninit");
}
double g_beginAppTime	=	Phantom::GetTickTime();
struct	cputime_out{
	double			m_timeFull;//所有时间
	unsigned int	m_beginCount;//总共调用次数
	double			m_timeMax;//峰值
	double			m_timeMin;//最低值
	Phantom::Name			m_funcName;
	Phantom::Name			m_fileName;
	int				m_line;
	double			m_timePercent;
	int				m_calls;//
};

int		SortTimes(const void* v1, const void* v2){
	cputime_out* t1 = (cputime_out*)v1;
	cputime_out* t2 = (cputime_out*)v2;
	if(t1->m_timeFull > t2->m_timeFull)
		return -1;
	if(t1->m_timeFull < t2->m_timeFull)
		return 1;
	return 0;
}
VOID			MyGameState::OnDestroy()
{
//
//			Phantom::StreamBase* f = Phantom::create_s("cputime.txt");
//			char buf[1024];
//			sprintf(buf, "File\tLine\tFullTime\tMinTime\tAvgTime\tMaxTime\tCalled\tChannel\tAppTimePercent\tFunction\tTimePercent\tCalled/s\n");
//			f->write(buf, strlen(buf));
//			Phantom::CDynamicArray<Phantom::cputime*>& times = Phantom::cputime::s_tops;
//			Phantom::CDynamicArray<cputime_out> ctimes[Phantom::cputime::cid_count];
//			for(int ic=0;ic<Phantom::cputime::cid_count;ic++){
//				ctimes[ic].resize(times.size());
//				for(int j=0;j<times.size();j++){
//					cputime_out& cto = ctimes[ic].at(j);
//					Phantom::cputime& ct = *times[j];
//					const char* szFunc = ct.m_funcName.t;
//#ifdef WIN32_OS
//					if(strnicmp(szFunc, "Phantom::", strlen("Phantom::"))==0)
//						szFunc += strlen("Phantom::");
//					if(strnicmp(szFunc, ct.m_className.t, strlen(ct.m_className.t))==0)
//						szFunc += strlen(ct.m_className.t)+2;
//#endif
//					cto.m_beginCount = ct.m_beginCount[ic];
//					cto.m_timeFull = ct.m_timeFull[ic];
//					sprintf(cto.m_funcName.t, "%s::%s", ct.m_className.t, szFunc);
//					cto.m_fileName = ct.m_fileName;
//					cto.m_line = ct.m_line;
//					cto.m_timeMax = ct.m_timeMax;
//					cto.m_timeMin = ct.m_timeMin;
//					if(cto.m_beginCount<1)cto.m_beginCount = 1;
//				}
//				qsort(ctimes[ic].GetPtr(), ctimes[ic].size(), sizeof(cputime_out), SortTimes);
//				const char* szFile = 0;
//				switch(ic){
//				case Phantom::cputime::cid_create:
//					szFile = "app_create";
//					break;
//				case Phantom::cputime::cid_framemove:
//					szFile = "app_framemove";
//					break;
//				case Phantom::cputime::cid_render:
//					szFile = "app_render";
//					break;
//				case Phantom::cputime::cid_input:
//					szFile = "app_input";
//					break;
//				case Phantom::cputime::cid_resize:
//					szFile = "app_resize";
//					break;
//				case Phantom::cputime::cid_resetdev:
//					szFile = "app_resetdev";
//					break;
//				}
//				if(!szFile)
//					continue;
//				for(int i=1;i<times.size();i++){
//					cputime_out* ct = &ctimes[ic].at(i);
//					ct->m_timePercent = (ct->m_timeFull / ctimes[ic].at(0).m_timeFull);
//					ct->m_calls = (ct->m_beginCount / ctimes[ic].at(0).m_beginCount);
//				}
//				int numOut = 180;
//				if(times.size()<numOut)
//					numOut = times.size();
//				double dTime = Phantom::GetTickTime() - g_beginAppTime;
//				for(int i=0;i<numOut;i++){
//					cputime_out* ct = &ctimes[ic].at(i);
//					sprintf(buf, "%s\t%d\t%.08f\t%.08f\t%.08f\t%.08f\t%d\t%s\t%.02f\t%s\t%.02f\t%d\n", 
//						ct->m_fileName.t, ct->m_line, ct->m_timeFull, ct->m_timeMin, ct->m_timeFull / (double)ct->m_beginCount, ct->m_timeMax, ct->m_beginCount, szFile, ct->m_timeFull/dTime, ct->m_funcName.t, ct->m_timePercent, ct->m_calls);
//					f->write(buf, strlen(buf));
//				}
//				//fclose(f);
//			}
//			Phantom::close_s(&f);

	g_luamanager.Destroy();
}
VOID			MyGameState::OnRender(double dElapsedTime)
{
	OnViewRender();
}
VOID			MyGameState::OnRenderUI()
{
	OnViewRenderUI();
}
VOID			MyGameState::OnInputEvent(Phantom::InputEventID evt, int x, int y, unsigned int touchIndex)
{
}

VOID			MyGameState::OnUIEvent(EventID id, UIDialog* dlg, UIControl* c, INT nParam)
{
	if(id == Phantom::EventID_Click)
	{
	}
}

VOID			MyGameState::OnMessageBox(INT nUserID, MsgID id)
{
	//if(nUserID == STR_NoVIPUser)
	//{
	//	if(id == MsgID_Yes)
	//	{
	//		gotoBuyRemoveAD();
	//	}
	//}
}

VOID  MyGameState::OnBuyCallback(BOOL bFail)
{
    if(bFail)
    {
    }
}
