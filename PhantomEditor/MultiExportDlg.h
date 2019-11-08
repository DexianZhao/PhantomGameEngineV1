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

// CMultiExportDlg 对话框

class CMultiExportDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMultiExportDlg)

public:
	CMultiExportDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMultiExportDlg();

// 对话框数据
	enum { IDD = IDD_MULTIEXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit m_edSrcPath;
	CEdit m_edExportPath;
	CButton m_chkTextureRename;
	CButton m_chkMeshRename;
};
