#pragma once
#include "MyWebBrowser.h"
#include "afxwin.h"
#include "afxcmn.h"

// CDlgWebView 对话框

class CDlgWebView : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWebView)

public:
	CDlgWebView(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgWebView();

// 对话框数据
	enum { IDD = IDD_WEB_VIEW };

	CMyWebBrowser	m_web;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	CComboBox m_cbUrl;
	CButton m_btOpen;
	CSpinButtonCtrl m_spBackForward;
	afx_msg void OnBnClickedOpen();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
