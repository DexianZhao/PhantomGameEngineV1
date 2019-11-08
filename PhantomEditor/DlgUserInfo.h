#pragma once
#include "resource.h"
#include "DialogBase.h"

// CDlgUserInfo 对话框

class CDlgUserInfo : public CDialogBase
{
	DECLARE_DYNAMIC(CDlgUserInfo)

public:
	CDlgUserInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgUserInfo();

// 对话框数据
	enum { IDD = IDC_USER_INFO };
	CEdit	m_edPPoint,m_edLevel,m_edVipEnd;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChangeaccount();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBuyToUrl();
	afx_msg void OnBnClickedBuyToUrl2();
	afx_msg void OnBnClickedCancel();
};
