#include "GameManager.h"
#ifdef STOCK
#include "MyStockState.h"
#else
#include "MyGameState.h"
#endif
#include "LuaManager.h"
#include "PhantomDB.h"
int		GetMemoryFree();//返回系统空闲内存

#ifdef IPHONE_OS
#include <mach/mach.h>
int GetMemoryFree()
{
    vm_statistics_data_t vmStats;
    mach_msg_type_number_t infoCount = HOST_VM_INFO_COUNT;
    kern_return_t kernReturn = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmStats, &infoCount);
    if(kernReturn == KERN_SUCCESS)
        return vmStats.free_count*vm_page_size;
    return 1024*1024*128;
}
#endif

int GetMaxMemoryUsage(int s)
{
	int sm = Texture::s_maxImageBuffer;
	if(s == 0)
		return sm;
	if(s == 1){
		GetStatePtr()->maxImageSize = getmax(GetStatePtr()->maxImageSize, sm);
		return GetStatePtr()->maxImageSize;
	}
	return 0;
}
GameRuntimeState*		GetStatePtr()
{
	static GameRuntimeState s;
	return &s;
}
#ifdef IPHONE_OS
VOID			FromLuaMessage(LPCSTR szName/*消息名称*/, LPCSTR szParams/*消息参数1*/, LPCSTR szParams2/*消息参数2*/)
{
}
#endif

#ifdef WIN32_OS//只有在windows系统中才会输出测试数据
VOID			FromLuaMessage(LPCSTR szName/*消息名称*/, LPCSTR szParams/*消息参数1*/, LPCSTR szParams2/*消息参数2*/)
{
}
VOID					SaveRuntimeState()
{
	char head[4];
	int version = 1;
	FILE* f = fopen("runtime", "wb");
	if(!f)return;
	head[0] = 'r';
	head[1] = 'u';
	head[2] = 'n';
	head[3] = 't';
	fwrite(head, 4, 1, f);
	fwrite(&version, sizeof(int), 1, f);


	char szExePath[1024];
	int len = ::WideCharToMultiByte(CP_ACP, 0, _wpgmptr, -1, szExePath, 1024, "", 0);
	szExePath[len] = 0;
	char wdrive[256], wdir[256], szIni[256];
	_splitpath(szExePath, wdrive, wdir, 0, 0);
	strcat(wdrive, wdir);
	strcpy(szIni, wdrive);
	strcat(szIni, "Phantom.ini");
	char szWidth[128], szHeight[128];
	char file[128];
	GetPrivateProfileStringA("active", "name", "", file, 128, szIni);
	::GetPrivateProfileStringA(file, "game_width", "960", szWidth, 128, szIni);
	::GetPrivateProfileStringA(file, "game_height", "640", szHeight, 128, szIni);
	GetStatePtr()->width = atoi(szWidth);
	GetStatePtr()->height = atoi(szHeight);
	GetStatePtr()->maxImageSize = getmax(Texture::s_maxImageBuffer, GetStatePtr()->maxImageSize);
	fwrite(GetStatePtr(), sizeof(GameRuntimeState), 1, f);
	fclose(f);
}
#endif
BOOL					LoadRuntimeState()
{
	Phantom::StreamBase* f = open_s("runtime");
	if(!f)
		return false;
	char head[4];
	int version;
	f->read(head, 4);
	if(!(head[0]=='r'&&head[1]=='u'&&head[2]=='n'&&head[3]=='t'))
	{
		close_s(&f);
		return false;
	}
	f->read(&version, sizeof(int));
	f->read(GetStatePtr(), sizeof(GameRuntimeState));
	close_s(&f);
	return true;
}

static	CGameManager*			g_gameManager;

extern "C"// in objective c
{
    void DestroyAllSDK()
    {
        g_manager.SetActiveSDK(0);
    }
    void CreateMainSDK()
    {
		LogInfo("CreateMainSDK");
        g_manager.SetActiveSDK(GetGameManager());
    }
};
VOID	ResetAllMain()
{
	LogInfo("Reset All");
	GetGameManager()->ResetAll();
}
CGameManager::CGameManager()
{
	g_gameManager		=	this;
	m_bIsVIPUser		=	TRUE;
	m_bSoundEnable	=	true;
	m_bMusicEnable	=	true;
	m_nextState		=	0;
	m_bLanguageChanged	=	false;
	m_defaultLua		=	"sc/main.lua";
}

CGameManager::~CGameManager()
{
}

VOID			CGameManager::OnInputEvent(Phantom::InputEventID evt, int x, int y, unsigned int touchIndex)
{
	if(m_stateActive)
		m_stateActive->OnInputEvent(evt, x, y, touchIndex);
}

VOID			CGameManager::OnActiveState(GameBase* s)
{
	AutoPtr<GameBase>	old;
	old			=	m_stateActive;
	m_stateActive	=	0;
	m_stateActive	=	s;
	if(old)
		old->OnDestroy();
	if(m_stateActive)
		m_stateActive->OnCreate();
	if(old && old->IsNewed())
		old	=	0;
	else
		old.t = 0;
}
VOID			CGameManager::OnRender(double dElapsedTime)
{
	if(m_stateActive)
		m_stateActive->OnRender(dElapsedTime);
}
VOID			CGameManager::OnRenderUI()
{
	if(m_stateActive)
		m_stateActive->OnRenderUI();
}
VOID			CGameManager::OnUIEvent(EventID id, UIDialog* dlg, UIControl* c, INT nParam)
{
	if(id == Phantom::EventID_Click && m_stateActive && m_dlgMsgBox == dlg)
	{
		if(this->m_nMsgBoxCallbackID == -1)
		{
			LPCSTR ret = "ok";
			if(c == m_msg_cancel)
				ret = "cancel";
			else if(c == m_msg_yes)
				ret = "yes";
			else if(c == m_msg_no)
				ret = "no";
			if(m_messageLuaCall.t[0])
				lua_tinker::call<void>(g_luamanager.GetLuaState(), m_messageLuaCall.t, ret);
		}
		else
		{
			if(c == m_msg_ok || c == m_msg_close)
				m_stateActive->OnMessageBox(this->m_nMsgBoxCallbackID, GameBase::MsgID_OK);
			else if(c == m_msg_cancel)
				m_stateActive->OnMessageBox(this->m_nMsgBoxCallbackID, GameBase::MsgID_Cancel);
			else if(c == m_msg_yes)
				m_stateActive->OnMessageBox(this->m_nMsgBoxCallbackID, GameBase::MsgID_Yes);
			else if(c == m_msg_no)
				m_stateActive->OnMessageBox(this->m_nMsgBoxCallbackID, GameBase::MsgID_No);
		}
		m_dlgMsgBox->SetVisible(false);
	}
	if(m_stateActive)
		m_stateActive->OnUIEvent(id, dlg, c, nParam);
}
VOID		ResetAll()
{
	GetGameManager()->ResetAll();
}
static int stateGUID[38] = {2095402076,1565793825,915817653,547972115,846622831,290607752,1032209317,201213576,2123697365,2006275257,1784965340,2054716648,1124545313,352992229,646858256,1164930964,1894064074,412369217,1962491674,1282345524,1360809132,1736992389,1655399545,1773693490,485364614,2049977455,1195335118,918428989,1718575228,139221310,1432708190,548040648,1359508631,93146936,2069908884,1289177993,435632193,1847096101};
VOID		SetStateGUID(int* n){
	for(int i=0;i<38;i++)
		stateGUID[i] = n[i];
}
VOID		CGameManager::ResetAll()
{
//#ifdef WIN32_OS
//	char szDir[256], szDrive[256];
//	GetCurrentDirectoryA(256, szDrive);
//	strcpy(szDir, szDrive);
//	strcat(szDrive, "document/");
//	CreateDirectoryA(szDrive,NULL);
//	SetDocumentDir(szDrive);
//	//::SetCurrentDirectoryA(szDir);
//#endif
	if(g_luamanager.GetLuaState())
		lua_tinker::call<void>(g_luamanager.GetLuaState(), "onexit");
	FreeAllMemoryFiles();
	this->m_dlgMsgBox = 0;
	//int stateGUID[] = {2095402076,1565793825,915817653,547972115,846622831,290607752,1032209317,201213576,2123697365,2006275257,1784965340,2054716648,1124545313,352992229,646858256,1164930964,1894064074,412369217,1962491674,1282345524,1360809132,1736992389,1655399545,1773693490,485364614,2049977455,1195335118,918428989,1718575228,139221310,1432708190,548040648,1359508631,93146936,2069908884,1289177993,435632193,1847096101};
#ifdef STOCK
	SetNextState(new MyStockState());
#else
	SetNextState(new MyGameState());
#endif
	g_manager.StopAllMusic();
}
VOID			CGameManager::OnCreate()
{
	initState("states", 0, 0);
	LogInfo("CGameManager::OnCreate");
	g_manager.SetSleepTime(0);
    g_manager.m_bShowFPS = false;
	g_manager.SetWavEnabled(m_bSoundEnable);
	g_manager.SetMp3Enabled(m_bMusicEnable);
	OnResize(g_manager.GetBufferWidth(), g_manager.GetBufferHeight());
	g_manager.SetSleepTime(0);
	//GetDlgManager()->SetCallback(this);
	this->LoadMsgBox("dlgs/cn/msgbox.rpgui");
	ResetAll();
	if(this->m_nextState)
	{
		OnActiveState(m_nextState);
		safe_release(m_nextState);
		m_nextState = 0;
	}
}

VOID			CGameManager::OnDestroy()
{
	g_luamanager.SetCallback(0);
	g_luamanager.Destroy();
	m_dlgMsgBox = 0;
	m_music.Destroy();
	this->OnActiveState(0);
	m_stateActive		=	0;
}

VOID			CGameManager::SetNextState(GameBase* func)	//添加一个新的状态
{
	m_nextState	=	func;
}
VOID			CGameManager::PlayMusic(LPCSTR szFileName)
{
	if(this->m_bMusicEnable)
	{
		char buf[128];
		sprintf(buf, "music/%s.mp3", szFileName);
		m_music.Load(buf);
		m_music.Play(100, m_music.GetVolume() * 0.9f, m_music.GetPan());
	}
}
VOID			CGameManager::LoadMsgBox(LPCSTR szFile)
{
	m_dlgMsgBox = 0;
	if(!GetDlgManager()->LoadDialog(m_dlgMsgBox, szFile))
		return;
	m_dlgMsgBox->SetVisible(false);
	m_msg_s_title	=	SearchControlUI<UIStatic>(m_dlgMsgBox, "title", UIControlType_STATIC);//s_text
	m_msg_s_text	=	SearchControlUI<UIStatic>(m_dlgMsgBox, "text", UIControlType_STATIC);//s_text
	m_msg_cancel	=	SearchControlUI<UIButton>(m_dlgMsgBox, "cancel", UIControlType_BUTTON);//cancel
	m_msg_no	=	SearchControlUI<UIButton>(m_dlgMsgBox, "no", UIControlType_BUTTON);//no
	m_msg_ok	=	SearchControlUI<UIButton>(m_dlgMsgBox, "ok", UIControlType_BUTTON);//ok
	m_msg_close	=	SearchControlUI<UIButton>(m_dlgMsgBox, "close", UIControlType_BUTTON);//ok
	m_msg_yes	=	SearchControlUI<UIButton>(m_dlgMsgBox, "yes", UIControlType_BUTTON);//yes
	m_msg_black_bg	=	SearchControlUI<UIStatic>(m_dlgMsgBox, "background", UIControlType_STATIC);//black_bg
}
VOID			CGameManager::OnResize(int w, int h)
{
//#ifdef ANDROID_OS
	g_manager.SetUIOffset(0, 0);
	g_manager.SetUIScale(g_manager.GetBufferWidth()/(float)g_manager.GetUIDialogSizeW(), g_manager.GetBufferHeight()/(float)g_manager.GetUIDialogSizeH());
//#endif
//	float fS = (float)g_manager.GetUIDialogSizeW() / (float)g_manager.GetUIDialogSizeH();
//	int newH = h - w / 5;//(float)w / 512.f * 768.f;
//	int nHeightSpace	=	h - newH;
//	if(nHeightSpace >= 50)
//	{
//		//g_bAlwaysAdmob = true;
//	}
//	g_manager.SetUIOffset(0, nHeightSpace);
//#else
//	int newH	=	h;
//#endif
	if(this->m_stateActive)
		this->m_stateActive->OnResize(w, h);
}

VOID			CGameManager::GetCursorPos(Pixel& ret, int touchIndex, INT nW, INT nH)
{
	int w = g_manager.GetBufferWidth();
	int h = g_manager.GetBufferHeight();
	Pixel c = g_manager.GetCursorPos(touchIndex);
	ret.Set((float)c.x / ((float)w / (float)nW), (float)c.y / ((float)h / (float)nH));
}
//VOID			CGameManager::GetGameScale(float2& scale, INT nW, INT nH)
//{
//	int w = g_manager.GetBufferWidth();
//	int h = g_manager.GetBufferHeight();
//	scale.setxy((float)w / (float)nW, (float)h / (float)nH);
//}
VOID			CGameManager::ShowMessage(MessageFlags uFlags)
{
	m_msg_ok->SetVisible(false);
	m_msg_yes->SetVisible(false);
	m_msg_no->SetVisible(false);
	m_msg_cancel->SetVisible(false);
	m_msg_close->SetVisible(false);
	//
	switch(uFlags)
	{
	case Message_OK:
		m_msg_close->SetVisible(true);
		break;
	case Message_YesNo:
		m_msg_yes->SetVisible(true);
		m_msg_no->SetVisible(true);
		break;
	case Message_OkCancel:
		m_msg_ok->SetVisible(true);
		m_msg_cancel->SetVisible(true);
		break;
	case Message_YesNoCancel:
		m_msg_yes->SetVisible(true);
		m_msg_no->SetVisible(true);
		m_msg_cancel->SetVisible(true);
		break;
	}
	m_dlgMsgBox->DoModal(true);
}
VOID			CGameManager::ShowMessage(LPCSTR szM, MessageFlags uFlags, LPCSTR szTitle, LPCSTR szLuaCall)
{
	if(szLuaCall)
		m_messageLuaCall = szLuaCall;
	else
		m_messageLuaCall.t[0] = 0;
	ShowMessage(szM, -1, uFlags, szTitle);
}
VOID			CGameManager::ShowMessage(LPCSTR szM, INT nCallbackID, MessageFlags uFlags, LPCSTR szTitle)
{
	m_nMsgBoxCallbackID	=	nCallbackID;
	if(!m_msg_s_text)
		return;
	if(szTitle==0)
	{
		szTitle = Lan("Message", "提示信息!");
	}
	m_msg_s_text->SetText(szM);
	m_msg_s_title->SetText(szTitle);
	ShowMessage(uFlags);
}
VOID			CGameManager::EnableMusic(BOOL b)
{
	if(b)
	{
		if(!m_music.IsPaused())
			m_music.Play();
		else
			m_music.Resume();
	}
	else
		m_music.Stop();
}
VOID			CGameManager::SetVIP(BOOL b)
{
	this->m_bIsVIPUser	=	b;
}

BOOL			CGameManager::GotoBack()//是否需要创建SDK?
{
	if(this->m_stateActive)
		return m_stateActive->GotoBack();
	return false;
}

char*			BuildArray(char* buf, int* n, int count)
{
	buf[0] = 0;
	char temp[128];
	for(int i=0;i<count;i++)
	{
		sprintf(temp, "%d", n[i]);
		strcat(buf, temp);
		if(i < (count-1))
			strcat(buf, ",");
	}
	return buf;
}

BOOL			CGameManager::OnFrameMove(double dElapsedTime)
{
	//if(m_bLanguageChanged)
	//{
	//	int nCount = GetDlgManager()->GetDlgBaseCount();
	//	for(int i=0;i<nCount;i++)
	//	{
	//		if(GetDlgManager()->GetDlgBase(i)->IsInit())
	//		{
	//			FileName file;
	//			GetDlgManager()->GetDlgBase(i)->dlg->m_strFile.splitpath(0, &file, 0);
	//			GetDlgManager()->GetDlgBase(i)->Load(GetLanguagePtr()->GetDlgFile(file.t));
	//		}
	//	}
	//	m_bLanguageChanged	=	false;
	//}
	if(this->m_nextState)
	{
		OnActiveState(m_nextState);
		safe_release(m_nextState);
		m_nextState = 0;
	}
	if(m_stateActive)
		m_stateActive->OnFrameMove(dElapsedTime);
	return true;
}

VOID				UpdateTime(UIControl* c)//更新当前时间到这个控件
{
	int hour, minute, second;
	GetTime(hour, minute, second);
	static std::map<UIControl*, int> minutes;
	int minuteS = minutes[c];
	if(minuteS != minute)
	{
		minutes[c] = minute;
		char buf[128];
		sprintf(buf, "%02d:%02d", hour,minute);
		c->SetText(buf);
	}
}




BOOL	find(Name* names, int count, LPCSTR findtext)
{
	for(int i=0;i<count;i++)
		if(names[i] == findtext)
			return true;
	return 0;
}
int		split(LPCSTR szIn, char symbol, Name* names, int maxNum)
{
	int len = strlen(szIn);
	int begin = 0;
	int count = 0;
	Name* n = &names[count];
	for(int i=0;i<=len;i++)
	{
		if(szIn[i] == symbol || i==len)
		{
			n->t[begin++] = 0;
			count++;
			if(count >= maxNum)
				return count;
			n = &names[count];
		}
		else
		{
			n->t[begin++] = szIn[i];
		}
	}
	return count;
}

//VOID	LoadConfig(LPCSTR szBin)
//{
//	GetConfigMgr()->Destroy();
//	GetConfigMgr()->Load(szBin);
//}
VOID	ResetLua(LPCSTR szInFile)
{
#ifdef WIN32_OS
	SaveRuntimeState();
#endif
	if(g_luamanager.GetLuaState())
		lua_tinker::call<void>(g_luamanager.GetLuaState(), "onexit");
	g_luamanager.Destroy();
	g_manager.FramePause(false);
	g_manager.SetFrameSpeed(1);
	reloadState();
	g_luamanager.Create(true, true);
	//luabind
	//lua_tinker::def(g_luamanager.GetLuaState(), "loadconfig", LoadConfig);
	//
	g_luamanager.callFile(szInFile);
}
VOID            ResetSmallImage(){
	LoadRuntimeState();
	if(GetStatePtr()->width == 0)
		GetStatePtr()->width = 960;
	if(GetStatePtr()->height == 0)
		GetStatePtr()->height = 640;
	g_manager.SetGameSize(GetStatePtr()->width, GetStatePtr()->height);
    Texture::s_bSmallImage = false;
#ifndef WIN32_OS
    int nFree = GetMemoryFree();
    int nMax =(GetStatePtr()->maxImageSize+1024*1024*20);
	if(nFree < nMax)//保留20兆内存之外的额外内存如果不够就是用小图片
	{
		Texture::s_bSmallImage = true;
	}
#endif
}
VOID			OnBeforeDeviceInit()
{
	GetGameManager()->OnBeforeDeviceInit();
}



namespace Phantom{
#ifdef ANDROID_OS
#define	WINAPI
#endif
#ifdef IPHONE_OS
#define	WINAPI
#endif
	/*API*/void	WINAPI	Global_msgbox(LPCSTR szTitle, LPCSTR szMsg, LPCSTR szFlags, LPCSTR script)
	{
		Name name[16];
		int count = split(szFlags, ',', name, 16);
		int flags = 0;
		if(find(name, 16, "ok"))
			flags |= GetGameManager()->Message_OK;
		else if(find(name, 16, "cancel"))
			flags |= GetGameManager()->Message_OkCancel;
		else if(find(name, 16, "yesno"))
			flags |= GetGameManager()->Message_YesNo;
		GetGameManager()->ShowMessage(szMsg, (CGameManager::MessageFlags)flags, szTitle, script);
	}

	VOID	WINAPI	Global_goto(LPCSTR szLuaFile)
	{
		if(g_gameState)
		{
			sprintf(g_gameState->m_nextLua.t, "sc/%s.lua", szLuaFile);
		}
	}

};









