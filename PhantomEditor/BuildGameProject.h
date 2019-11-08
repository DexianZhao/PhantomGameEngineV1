#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "PlotSystem.h"
#include "SqliteDB.h"
#include "ImageButton.h"
#include "GameDirector.h"
#include "GCGameProject.h"

// CBuildGameProject 对话框

class CBuildGameProject : public CDialogEx, public ISystemProgress
{
	DECLARE_DYNAMIC(CBuildGameProject)

public:
	CBuildGameProject(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBuildGameProject();

// 对话框数据
	enum { IDD = IDD_REPORT_RUNTIME };

	virtual	VOID	OnBegin(INT nMax);	//开始进度
	virtual	VOID	OnProgress();//进度进行中+1
	virtual	VOID	OnEnd();//结束进度
	INT				m_nCurrentProgress;
	INT				m_nMaxProgress;
	//
	CString			m_strFileExport;
	CString			m_strGameName;
	CString			m_strGameIcon;
	//
	GCGameProject*	m_projPtr;
	VOID			LoadGameIcon();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edGameName;
	CEdit m_edResourceCode;
	CEdit m_edAuthCode;
	CEdit m_edUserName;
	CEdit m_edUserURL;
	CEdit m_edResourceDesc;
	CButton m_chkAuthEnable;
	afx_msg void OnBnClickedOk();
	CListBox m_lbFileList;
	CProgressCtrl		m_progExport;
	CString				m_strBuild;
	CImageButton		m_btGameIcon;
	CImage				m_imgGameIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedResetGuid();
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedAuthEnable();
	afx_msg void OnBnClickedSetIcon();
	afx_msg void OnBnClickedCancel2();
	CButton m_chkCompress;
};
