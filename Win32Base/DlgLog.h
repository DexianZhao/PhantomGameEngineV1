/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"
#include <vector>
#include <string>
#ifdef EDITOR_OS
#include "GCGameService.h"
#include "GCStateDefault.h"
#endif

extern	CRect	LoadRect(const char* szKey, CRect& rcDefault);
extern	void	SaveRect(CDialog* dlg, const char* szKey);

// CDlgLog 对话框
struct	LogChannel
{
	int							count;
	int							addcount;
	char						name[32];
	std::vector<std::wstring*>	logs;//
	LogChannel();
	~LogChannel();
};

struct LogData{
	int			type;
	wchar_t		time[64];
	wchar_t		text[256];
	CRect		rect;
	enum{
		Log_System = 0,
		Log_Warning,
		Log_Error,
		Log_Code,
		Log_Highlight,
	};
};
// CMyLogout

class CMyLogout : public CButton
{
	DECLARE_DYNAMIC(CMyLogout)

public:
	CMyLogout();
	virtual ~CMyLogout();
	VOID				RenderLine(CDC& dc, const CRect& rc);
	VOID				AddLog(const wchar_t* text, BOOL bSystem);
	CImage				m_tempDC;
	std::vector<LogData>		m_logs;
	int							m_select;
	CEdit*				m_edit;
	VOID				Clear();
protected:
	DECLARE_MESSAGE_MAP()
public:
	void DrawItem(LPDRAWITEMSTRUCT);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};



class CDlgLog : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLog)

public:
	CDlgLog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLog();

	VOID		AddLog(const char* szText);
	VOID		AddLog(int channel, const char* szText);
	enum { IDD = IDD_LOG_DIALOG };
#ifdef EDITOR_OS
	VOID		StartRuntime();
	VOID		ResetServer();
#endif	
	VOID		OnResize();
	VOID		InitLog();
	//
	CMyLogout	m_logInfo;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton		m_btClear,m_btReset,m_btClearData;
	afx_msg void OnBnClickedClearLogs();
	afx_msg void OnBnClickedResetAll();
	afx_msg void OnBnClickedResetClear();
	CEdit m_edLogInfo;
	afx_msg void OnLbnSelchangeLbInfo();
	afx_msg void OnTcnSelchangeLogtab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnMove(int x, int y);
#ifdef EDITOR_OS
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedSetAppid();
	CButton		m_btAppID;
#endif
	CButton m_chkSystemLog;
	afx_msg void OnBnClickedSystemMsg();
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
};
