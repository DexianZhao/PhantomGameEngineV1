#pragma once
#include "resource.h"

// CDlgResDetail 对话框

class CDlgResDetail : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgResDetail)

public:
	CDlgResDetail(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgResDetail();
	CString	m_strDetail;
	CEdit	m_edDetail;
// 对话框数据
	enum { IDD = IDD_RESOURCE_DETAIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
