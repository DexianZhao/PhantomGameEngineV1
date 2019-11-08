/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "afxcmn.h"
#include "afxwin.h"
class CodeEditor;

// CDlgFindReplace 对话框

class CDlgFindReplace : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFindReplace)

public:
	CDlgFindReplace(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgFindReplace();

// 对话框数据
	enum { IDD = 0 };

	CodeEditor*	m_editor;
	VOID	ShowFind(const wchar_t* wt, BOOL bReplace);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit	m_edFindText;
	CEdit	m_edReplaceText;
	CButton m_chkReplace;
	CButton m_btReplace;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedReplace();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnBnClickedChkReplace();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
