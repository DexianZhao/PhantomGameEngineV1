#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"
#include <vector>
#include <string>

// CDlgLog 对话框
struct	LogChannel
{
	int							count;
	int							addcount;
	char						name[32];
	std::vector<std::string*>	logs;//
	LogChannel();
	~LogChannel();
};

class CDlgLog : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLog)

public:
	CDlgLog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLog();

	VOID		AddLog(const char* szText);
	std::vector<LogChannel*>	m_channels;
	enum { IDD = IDD_LOG_DIALOG };
	int			GetChannel(const char* szLogFirst);//
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CListBox m_lbInfo;
	CTabCtrl m_logTbl;
	afx_msg void OnBnClickedClearLogs();
	afx_msg void OnBnClickedResetAll();
	CEdit m_edLogInfo;
	afx_msg void OnLbnSelchangeLbInfo();
	afx_msg void OnTcnSelchangeLogtab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedResetAll2();
	afx_msg void OnBnClickedResetAll3();
};
