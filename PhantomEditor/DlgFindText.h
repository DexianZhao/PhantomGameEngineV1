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

// DlgFindText 对话框

class DlgFindText : public CDialogEx
{
	DECLARE_DYNAMIC(DlgFindText)

public:
	DlgFindText(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgFindText();

// 对话框数据
	enum { IDD = IDD_FIND_TEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
