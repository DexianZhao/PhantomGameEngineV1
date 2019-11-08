
// BaiWanGameDlg.h : 头文件
//

#pragma once

#include "OpenEdit.h"
#include "DlgLog.h"
#include "DlgBindBar.h"


// CBaiWanGameDlg 对话框
class CBaiWanGameDlg : public CDialogEx
{
// 构造
public:
	CBaiWanGameDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BAIWANGAME_DIALOG };

	COpenEdit	m_edit;
	CDlgBindBar	m_bind;
	CDlgLog		m_log;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持



// 实现
protected:
	HICON m_hIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnMove(int x, int y);
};
