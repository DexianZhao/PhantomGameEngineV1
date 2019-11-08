/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "afxwin.h"
#include "resource.h"

// CGameScreenSize 对话框

class CGameScreenSize : public CDialogEx
{
	DECLARE_DYNAMIC(CGameScreenSize)

public:
	CGameScreenSize(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameScreenSize();

// 对话框数据
	enum { IDD = IDD_GAME_SCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edScreenX;
	CEdit m_edScreenY;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
