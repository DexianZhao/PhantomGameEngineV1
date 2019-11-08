#pragma once
#include "GameManager.h"
#include "PhantomMap2D.h"
#include "StockInterface.h"

#define	MaxTimeCount		50
class MyStockState: public GameBase
{
public:
	MyStockState();
	~MyStockState(void);
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
	Dialog					m_cputime;
	UIControl*				m_timeList[MaxTimeCount];
	UIControl*				m_btnSave;
	float					m_lastBuildTime;
	cputime*				m_seltime;
public:
};

extern	MyStockState*		g_gameState;
