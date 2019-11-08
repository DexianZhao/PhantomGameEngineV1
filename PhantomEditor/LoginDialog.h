#pragma once
#include "resource.h"
#include <string>
#include "DialogBase.h"
// CLoginDialog 对话框

class CLoginDialog : public CDialogBase
{
	DECLARE_DYNAMIC(CLoginDialog)

public:
	CLoginDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoginDialog();
	BOOL		m_bResetPassword;
	VOID		OK(){CDialogEx::OnOK();}
// 对话框数据
	enum { IDD = IDD_LOGINUSERs };
	std::string	m_strEmail,m_strPassword;
	CEdit		m_edEmail,m_edPassword;
	CButton		m_chkAutoLogin;
	VOID		ReloadLogin();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRegister();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeEdit3();
};
