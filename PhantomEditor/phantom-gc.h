
// phantom-gc.h : phantom-gc 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


#define	AUTH_MODE	1	//仅对正版用户支持

// CPhantomGCApp:
// 有关此类的实现，请参阅 phantom-gc.cpp
//

class CPhantomGCApp : public CWinAppEx
{
public:
	CPhantomGCApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDataRecoveryHandler *GetDataRecoveryHandler(){return NULL;}
// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing

	BOOL			IsUpdateCmdUI();
	unsigned int	m_nLastTickTime;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	afx_msg void OnGotoForum();
	afx_msg void OnBuyProfessional();
	afx_msg void OnShowHelp();
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
};

extern CPhantomGCApp theApp;

extern	VOID		UpdateMainTitle();
