#pragma once


// CBigTextEditor 对话框

class CBigTextEditor : public CDialogEx
{
	DECLARE_DYNAMIC(CBigTextEditor)

public:
	CBigTextEditor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBigTextEditor();

// 对话框数据
	enum { IDD = IDD_EDIT_BIG_TEXT };

	CString					m_editText;
	CEdit					m_richEdit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
