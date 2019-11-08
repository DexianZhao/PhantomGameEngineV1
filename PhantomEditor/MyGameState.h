/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "GameManager.h"
#include "PhantomMap2D.h"

class MyGameState: public GameBase
{
public:
	MyGameState();
	~MyGameState(void);
	virtual	VOID			OnCreate();
	virtual	VOID			OnDestroy();
	virtual	BOOL			OnFrameMove(double dElapsedTime);
	virtual	VOID			OnInputEvent(Phantom::InputEventID evt, int x, int y, unsigned int touchIndex);
	virtual	VOID			OnUIEvent(EventID id, UIDialog* dlg, UIControl* c, INT nParam);
	//
	virtual	BOOL			GotoBack(){return false;}//
    virtual VOID            OnBuyCallback(BOOL bFail);
	VOID					SetRenameVisible(BOOL b);
	virtual	VOID			OnMessageBox(INT nUserID, MsgID id);
	virtual	void			OnHttpCallback(){}
	virtual	VOID			OnRender(double dElapsedTime);
	virtual	VOID			OnRenderUI();
	FileName				m_nextLua;
public:
};

extern	MyGameState*		g_gameState;
