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

// CPropertyDlgWndBase 对话框

class CPropertyDlgWndBase : public CDialogEx
{
	DECLARE_DYNAMIC(CPropertyDlgWndBase)

public:
	CPropertyDlgWndBase(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPropertyDlgWndBase();

// 对话框数据
	enum { IDD = IDD_PROPERTY_DLG_WND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};
