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
#include "ImageButton.h"
#include "ImageCheck.h"
#include "GCMFCHelper.h"
#include "DialogBase.h"

class GCViewBase;

class CAlignDialog : public CDialogBase
{
	DECLARE_DYNAMIC(CAlignDialog)

public:
	CAlignDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAlignDialog();

// 对话框数据
	enum { IDD = IDD_ALIGN_DIALOG };
	//
	VOID	OnActiveView(GCViewBase* base);
	//
	VOID	SetEditorFlags(int type);
	//
	BOOL	GetFocusEditor(CEdit* edit, AlignType align, BOOL bResetValue = FALSE);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnSetfocusOffsetX();
	afx_msg void OnEnSetfocusOffsetY();
	virtual BOOL OnInitDialog();
	//
	CEdit m_edOffsetX;
	CEdit m_edOffsetY;
	CEdit m_edAlignLeft;
	CEdit m_edAlignTop;
	CEdit m_edAlignRight;
	CEdit m_edAlignBottom;
	//
	CEdit m_edAlignSpaceX;
	CEdit m_edAlignSpaceY;
	CEdit m_edAlignWidth;
	CEdit m_edAlignHeight;
	//
	CEdit m_edGridWidth;
	CEdit m_edGridHeight;
	CEdit m_edRCCount;
	CEdit m_edRCSpaceX;
	CEdit m_edRCSpaceY;
	CButton m_chkRCVertical;
	CEdit m_edRCRename;
	CButton m_chkCenter;
	afx_msg void OnBnClickedRcRealign();
	afx_msg void OnBnClickedRcRename();
	afx_msg void OnBnClickedCheck1();
};

CPoint	GetUIGridSize();
VOID	SetUIGridSize(const CPoint& p);

CAlignDialog*	GetAlignDialog();
