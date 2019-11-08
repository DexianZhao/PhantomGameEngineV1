#include "MyStockState.h"
#include "GameManager.h"
#include "PhantomDB.h"
bool __cdecl IsExit(void){return false;}
void __cdecl Log(char const *,...){}
void __cdecl StartProgress(char const *){}
void __cdecl UpdateProgress(float,int,int){}

MyStockState* g_gameState = 0;
MyStockState::MyStockState()
{
	g_gameState = this;
	m_lastBuildTime	=	0;
	m_seltime		=	0;
}

MyStockState::~MyStockState(void)
{
	if(g_gameState == this)
		g_gameState = 0;
}
double g_beginAppTime	=	GetTickTime();
struct	cputime_out{
	double			m_timeFull;//所有时间
	unsigned int	m_beginCount;//总共调用次数
	double			m_timeMax;//峰值
	double			m_timeMin;//最低值
	Name			m_funcName;
	Name			m_fileName;
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

VOID			MyStockState::OnUIEvent(EventID id, UIDialog* dlg, UIControl* c, INT nParam)
{
	if(id == Phantom::EventID_Click)
	{
		if(m_btnSave==c){
#if defined(_CPUTIME)||defined(_CPUTIME_TOP)
			StreamBase* f = create_s("cputime.txt");
			char buf[1024];
			sprintf(buf, "File\tLine\tFullTime\tMinTime\tAvgTime\tMaxTime\tCalled\tChannel\tAppTimePercent\tFunction\tTimePercent\tCalled/s\n");
			f->write(buf, strlen(buf));
			CDynamicArray<cputime*>& times = cputime::s_tops;
			CDynamicArray<cputime_out> ctimes[cputime::cid_count];
			for(int ic=0;ic<cputime::cid_count;ic++){
				ctimes[ic].resize(times.size());
				for(int j=0;j<times.size();j++){
					cputime_out& cto = ctimes[ic].at(j);
					cputime& ct = *times[j];
					const char* szFunc = ct.m_funcName.t;
#ifdef WIN32_OS
					if(strnicmp(szFunc, "Phantom::", strlen("Phantom::"))==0)
						szFunc += strlen("Phantom::");
					if(strnicmp(szFunc, ct.m_className.t, strlen(ct.m_className.t))==0)
						szFunc += strlen(ct.m_className.t)+2;
#endif
					cto.m_beginCount = ct.m_beginCount[ic];
					cto.m_timeFull = ct.m_timeFull[ic];
					sprintf(cto.m_funcName.t, "%s::%s", ct.m_className.t, szFunc);
					cto.m_fileName = ct.m_fileName;
					cto.m_line = ct.m_line;
					cto.m_timeMax = ct.m_timeMax;
					cto.m_timeMin = ct.m_timeMin;
					if(cto.m_beginCount<1)cto.m_beginCount = 1;
				}
				qsort(ctimes[ic].GetPtr(), ctimes[ic].size(), sizeof(cputime_out), SortTimes);
				const char* szFile = 0;
				switch(ic){
				case cputime::cid_create:
					szFile = "app_create";
					break;
				case cputime::cid_framemove:
					szFile = "app_framemove";
					break;
				case cputime::cid_render:
					szFile = "app_render";
					break;
				case cputime::cid_input:
					szFile = "app_input";
					break;
				case cputime::cid_resize:
					szFile = "app_resize";
					break;
				case cputime::cid_resetdev:
					szFile = "app_resetdev";
					break;
				}
				if(!szFile)
					continue;
				for(int i=1;i<times.size();i++){
					cputime_out* ct = &ctimes[ic].at(i);
					ct->m_timePercent = (ct->m_timeFull / ctimes[ic].at(0).m_timeFull);
					ct->m_calls = (ct->m_beginCount / ctimes[ic].at(0).m_beginCount);
				}
				int numOut = 180;
				if(times.size()<numOut)
					numOut = times.size();
				double dTime = GetTickTime() - g_beginAppTime;
				for(int i=0;i<numOut;i++){
					cputime_out* ct = &ctimes[ic].at(i);
					sprintf(buf, "%s\t%d\t%.08f\t%.08f\t%.08f\t%.08f\t%d\t%s\t%.02f\t%s\t%.02f\t%d\n", 
						ct->m_fileName.t, ct->m_line, ct->m_timeFull, ct->m_timeMin, ct->m_timeFull / (double)ct->m_beginCount, ct->m_timeMax, ct->m_beginCount, szFile, ct->m_timeFull/dTime, ct->m_funcName.t, ct->m_timePercent, ct->m_calls);
					f->write(buf, strlen(buf));
				}
				//fclose(f);
			}
			close_s(&f);
			return;
#endif
		}
	}
}

BOOL	MyStockState::OnFrameMove(double dElapsedTime)
{
	m_lastBuildTime-=dElapsedTime;
	if(m_lastBuildTime<0){
		CDynamicArray<cputime*> times;
		if(m_seltime){
			times.push_back(m_seltime);
			for(int i=0;i<m_seltime->m_childs.size();i++)
				times.push_back(m_seltime->m_childs[i]);
		}
		else
			times = cputime::s_tops;
		if(times.size()>1){
			//qsort(times.GetPtr(), times.size(), sizeof(cputime*), SortTimes);
		}
		char buf[512];
		for(int i=0;i<MaxTimeCount;i++){
			m_timeList[i]->SetVisible(i<times.size());
			if(i<times.size()){
				cputime* ct = times.GetPtr()[i];
				if(m_seltime&&i==0)
					sprintf(buf, "...<-...");
				else
					sprintf(buf, "%s::%s[%.02f(%.02f<%.02f<%.02f) %d]", ct->m_className.t, ct->m_funcName.t, ct->m_timeFull[1], ct->m_timeMin, ct->m_timeFull[1] / (double)ct->m_beginCount[1], ct->m_timeMax, ct->m_beginCount[1]);
				m_timeList[i]->SetText(buf);
			}
		}
		//for(int i=0;i<MaxTimeCount;i++){
		//	m_timeList[i]->SetVisible(true);
		//	for(int j=0;j<120;j++)
		//		buf[j] = rand()%20+'a';
		//	buf[120] = 0;
		//	m_timeList[i]->SetText(buf);
		//}
		m_lastBuildTime = 1;
	}
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
	return true;
}
const char*	GetDeviceID(){//
	return "25233437E5B6434ABB7E59D0C3972892";
}


VOID			MyStockState::OnCreate()
{
	g_manager.SetDefaultUnicodeFont("default_init_zip/fonts/cn.fig");
	LogInfo("MyStockState::OnCreate");
	char szFile[256];
	strcpy(szFile, GetGameManager()->m_defaultLua);//"sc/main.lua");
	LoadStock("F:/Trend/Stock/000001.stock");
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
	GetDlgManager()->CreateDlg(m_cputime);
	int height = 13;//g_manager.GetBufferHeight()/(MaxTimeCount+1);
	for(int i=0;i<MaxTimeCount;i++){
		UIButton* temp = m_cputime->AddButton(true, "Time", 0, height/2+i*height, g_manager.GetBufferWidth()*2/3, height);
		temp->SetText("Time");
		m_timeList[i] = temp;
		m_timeList[i]->GetElementPtr(0)->SetFontID(GetDlgManager()->AddFont("Arial", 12, 1));
		//m_timeList[i]->SetVisible(false);
	}
	m_btnSave = m_cputime->AddButton(true, "Save", g_manager.GetBufferWidth()-100, 0, 100, 100);
	m_cputime->SetVisible(true);
}
//
VOID			MyStockState::OnDestroy()
{
	g_luamanager.Destroy();
}
VOID			MyStockState::OnRender(double dElapsedTime)
{
}
class CRender:public IRender
{
public:
	void	DrawRect(int x, int y, int w, int h, int color)
	{
		Rect rc(x,y,x+w,y+h);
		g_gameState->m_cputime->DrawRect2D(&rc, color, true);
	}
};
VOID			MyStockState::OnRenderUI(){
	Rect rc(0, 0, g_manager.GetBufferWidth(), g_manager.GetBufferHeight());
	m_cputime->DrawRect2D(&rc, 0xff3f3f3f, true);
	int bufferWidth = g_manager.GetBufferWidth();
	int bufferHeight = g_manager.GetBufferHeight();
	CRender temp;
	RenderStock(&temp, bufferWidth, bufferHeight);

	
}
VOID			MyStockState::OnInputEvent(Phantom::InputEventID evt, int x, int y, unsigned int touchIndex)
{
}

VOID			MyStockState::OnMessageBox(INT nUserID, MsgID id)
{
	//if(nUserID == STR_NoVIPUser)
	//{
	//	if(id == MsgID_Yes)
	//	{
	//		gotoBuyRemoveAD();
	//	}
	//}
}

VOID  MyStockState::OnBuyCallback(BOOL bFail)
{
    if(bFail)
    {
    }
}
