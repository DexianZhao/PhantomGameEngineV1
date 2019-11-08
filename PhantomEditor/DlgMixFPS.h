/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once

#include "DialogBase.h"
#include "afxwin.h"

// CDlgMixFPS 对话框

class CDlgMixFPS : public CDialogBase
{
	DECLARE_DYNAMIC(CDlgMixFPS)

public:
	CDlgMixFPS(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMixFPS();

// 对话框数据
	enum { IDD = IDD_MIX_FPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edFPS;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
