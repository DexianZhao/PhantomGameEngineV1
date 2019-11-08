/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "resource.h"

// CDlgSelectAppID 对话框

class CDlgSelectAppID : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSelectAppID)

public:
	CDlgSelectAppID(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSelectAppID();

// 对话框数据
	enum { IDD = IDD_SELECT_APPID };
	CComboBox		m_cbAppIDs;
	CButton			m_btOK;
	CString			m_strAppID;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
