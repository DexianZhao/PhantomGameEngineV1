/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/

#ifndef __BW_GAME_MANAGER_H__
#define	__BW_GAME_MANAGER_H__
#include "PhantomHttp.h"
#include "PhantomLanguage.h"
#include "LuaManager.h"
using namespace Phantom;

#define	MINE_HTTP	"http://www.aixspace.com/"
#pragma pack(push, 1);
struct	GameRuntimeState
{
	int					maxImageSize;
	short				width,height;
	int					reserved[511];
	GameRuntimeState(){memset(this, 0, sizeof(GameRuntimeState));}
};
#pragma pack(pop);
GameRuntimeState*		GetStatePtr();
#ifdef WIN32_OS//只有在windows系统中才会输出测试数据
VOID					SaveRuntimeState();
#endif
BOOL					LoadRuntimeState();

class	GameBase: public SDKBase
{
public:
	enum MsgID{
		MsgID_Yes	=	0,
		MsgID_No,
		MsgID_OK,
		MsgID_Cancel,
	};
	virtual	void		OnHttpCallback(){}
    virtual void		OnBuyCallback(BOOL bFail){}
	virtual	VOID		OnMessageBox(INT nUserID, MsgID id){}	//
	virtual	BOOL		IsNewed(){return true;}//是否是创建的新类
	//
};

extern "C"// in objective c
{
    extern	void    showAd(int x, int y, int type, int bShow);
    extern	void    shopBuy(const char* buyName);
	extern	void    shopRestore(const char* buyName);
	extern	void	gotoComment();
	extern	void	openUrl(const char* url);
	extern	void	openEdit(const char* szDefault, int x, int y, int w, int h);	//打开一个编辑框并输入文本, 指定位置信息
	extern	void	closeEdit(char* retText); //写入了文本之后关闭编辑框，并且返回一个可用文本
    int     GetAdViewEnabled();
	void	setAdMode(int mode, const char** urls, const char** images, int count, int myadv_gailv);//设置我的广告条列表
};

class	CGameManager: public SDKBase
{
public:
	CGameManager();
	~CGameManager();
	VOID								EnableMusic(BOOL b);
	virtual	BOOL						GotoBack();//是否需要创建SDK?
public://messagebox
	enum MessageFlags{
		Message_OK	=	0,
		Message_YesNo,
		Message_OkCancel,
		Message_YesNoCancel,
	};
	enum{
	};
	VOID								ShowMessage(const char* szM, INT nCallbackID, MessageFlags uFlags = Message_OK, const char* szTitle = 0);
	VOID								ShowMessage(const char* szM, MessageFlags uFlags, const char* szTitle = 0, const char* szLuaCall = 0);
	VOID								ShowMessage(MessageFlags uFlags);
	VOID								ResetAll();
	//
public:
	VOID								LoadMsgBox(const char* szFile);//
	VOID								PlayMusic(const char* szFileName);//music目录内自动选择这个文件，写入比如a.mp3，自动搜索music/a.mp3
	VOID								SetNextState(GameBase* newPtr);	//添加下一个要切换的状态，一定要新建
	//
	virtual	const char*					GetGameName(){return "PhantomGame";}
	const char*							GetMineName(){return m_mineName;}
	VOID								GetCursorPos(Pixel& ret, int touchIndex, INT nW, INT nH);
public:
	AutoPtr<GameBase>					m_stateActive;
	FileName							m_url;
	Music2D								m_music;
	Name								m_mineName;
	Name								m_defaultLua;
protected:
	virtual	VOID						OnResize(int w, int h);
	virtual	BOOL						OnFrameMove(double dElapsedTime);
	virtual	VOID						OnRender(double dElapsedTime);
	virtual	VOID						OnUIEvent(EventID id, UIDialog* dlg, UIControl* c, INT nParam);
	virtual	VOID						OnInputEvent(Phantom::InputEventID evt, int x, int y, unsigned int touchIndex);
	virtual	VOID						OnCreate();
	virtual	VOID						OnDestroy();
	virtual	VOID						OnRenderUI();
	//
	virtual	VOID						OnActiveState(GameBase* s);
protected:
	NameT<4096>							m_messageLuaCall;
	BOOL								m_bSoundEnable;
	BOOL								m_bMusicEnable;
	FileName							m_currMusic;
protected:
	GameBase*							m_nextState;
	Dialog								m_dlgMsgBox;
	UIStatic*							m_msg_s_text;
	UIStatic*							m_msg_s_title;
	UIButton*							m_msg_cancel;
	UIButton*							m_msg_no;
	UIButton*							m_msg_ok;
	UIButton*							m_msg_close;
	UIButton*							m_msg_yes;
	UIStatic*							m_msg_black_bg;
	//
	BOOL								m_bLanguageChanged;//语言已经改变，要重新载入所有语言资源
	INT									m_nMsgBoxCallbackID;
public:
	virtual	VOID						OnBeforeDeviceInit(){}
};
VOID            ResetSmallImage();
extern	CGameManager*					GetGameManager();

struct	TextEnum{
	unsigned int color;
	char		cnText[512];
	char		enText[512];
	int			type;

};

VOID									UpdateTime(UIControl* c);//更新当前时间到这个控件
template <typename T>
inline	VOID	LoadDlg(T& t, const char* szName)
{
	t.destroy();
	t.Load(GetLanguagePtr()->GetDlgFile(szName));
	t->SetVisible(false);
	t->PlayAction(PhantomV1::ActionType_Idle);
}
VOID	ResetLua(const char* szInFile);//重新启动lua，并且使用入口文件


#endif
