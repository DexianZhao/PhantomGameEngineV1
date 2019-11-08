/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/

// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "PropertiesDialogWnd.h"
#include "PropertiesRuntimeWnd.h"
#include "PropertiesUIWnd.h"
#include "AlignDialog.h"
#include "ImageListViewDLG.h"
#include "ViewImageManager.h"
#include "AlignDialog3D.h"
#include "../Win32base/DlgLog.h"
#include "DlgResShop.h"
#include "DlgFindReplace.h"
class GCViewManager;
class GCBitmapMgr;


#define	WM_DDE_FILEOPEN		(WM_USER + 168)

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

	virtual BOOL OnMenuButtonToolHitTest(CMFCToolBarButton* /*pButton*/, TOOLINFO* /*pTI*/);
	virtual BOOL GetToolbarButtonToolTipText(CMFCToolBarButton* /*pButton*/, CString& /*strTTText*/);

	CAlignDialog		m_alignDlg;
	CAlignDialog3D		m_alignDlg3D;
	CDlgFindReplace		m_dlgFind;
// 特性
public:
	GCViewManager*	m_pViewManager;
	GCBitmapMgr*	m_pBitmapManager;
// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

	CPropertiesUIWnd	m_uiProperties;

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCToolBar       m_wndToolBarUI;
	CMFCToolBar       m_wndToolBarScene;
	CMFCToolBar       m_wndToolBarAlign;
	CMFCToolBar       m_wndToolBarProj;
	CMFCToolBar       m_wndToolBarScene3D;
	CMFCToolBar       m_wndToolBarReport;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CFileView         m_wndFileView;
	CFileBrowser      m_wndFileBrowser;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	CPropertiesDialogWnd	m_wndDlg2;
	CPropertiesDialogWnd	m_wndPropertiesDialog;
	bool				m_bShowMessage;
	CDlgLog				m_dlgLog;
// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	virtual BOOL OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bTabDrop){return true;}
	afx_msg void OnDestroy();
	virtual void ActivateFrame(int nCmdShow = -1);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileNew();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	//
	afx_msg void OnUpdateInsertFormatstatic(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertImeedit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertFormatedit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertCheckbox(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertCombobox(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertSlider(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertListbox(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertScrollbar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertProgress(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertListctrl(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertButton(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertStatic(CCmdUI *pCmdUI);
	//
	afx_msg void OnInsertFormatstatic();
	afx_msg void OnInsertImeedit();
	afx_msg void OnInsertFormatedit();
	afx_msg void OnInsertCheckbox();
	afx_msg void OnInsertCombobox();
	afx_msg void OnInsertSlider();
	afx_msg void OnInsertListbox();
	afx_msg void OnInsertScrollbar();
	afx_msg void OnInsertProgress();
	afx_msg void OnInsertListctrl();
	afx_msg void OnInsertButton();
	afx_msg void OnInsertStatic();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRefreshFiles();
	afx_msg void OnUiPngMaker();
	afx_msg void OnExportUi();
	afx_msg void OnSetProject();
	afx_msg void OnUserInfo();
	afx_msg void OnUpdateShowScript(CCmdUI *pCmdUI);
	afx_msg void OnShowScript();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnUpdateBuyResource(CCmdUI *pCmdUI);
	afx_msg void OnBuyResource();
	afx_msg void OnConvertAllFbx();
};

CMainFrame*	GetMainFrame();

VOID		UpdateRobbinNow();
