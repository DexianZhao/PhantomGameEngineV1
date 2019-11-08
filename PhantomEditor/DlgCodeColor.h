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

// CDlgCodeColor 对话框

class CDlgCodeColor : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCodeColor)

public:
	CDlgCodeColor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCodeColor();

// 对话框数据
	enum { IDD = IDD_COLOR_SET };
	CListBox			m_colorlist;
	CMFCColorButton		m_browseColor;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnBnClickedMfccolorbutton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
