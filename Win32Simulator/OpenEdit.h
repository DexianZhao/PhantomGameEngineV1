#pragma once

#include "resource.h"
#include "afxwin.h"

// COpenEdit 对话框

class COpenEdit : public CDialogEx
{
	DECLARE_DYNAMIC(COpenEdit)

public:
	COpenEdit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COpenEdit();

// 对话框数据
	enum { IDD = IDD_OPEN_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CEdit m_ed;
};
