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

class CAlignDialog3D : public CDialogBase
{
	DECLARE_DYNAMIC(CAlignDialog3D)

public:
	CAlignDialog3D(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAlignDialog3D();

// 对话框数据
	enum { IDD = IDD_ALIGN_DIALOG3D };
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
	CEdit m_edOffsetZ;
	CEdit m_edAlignMinX;
	CEdit m_edAlignMinY;
	CEdit m_edAlignMaxX;
	CEdit m_edAlignMaxY;
	CEdit m_edAlignMinZ;
	CEdit m_edAlignMaxZ;
	//
	CEdit m_edAlignSpaceX;
	CEdit m_edAlignSpaceY;
	CEdit m_edAlignSpaceZ;
	CEdit m_edAlignWidth;
	CEdit m_edAlignHeight;
	CEdit m_edAlignLength;
	//
	CEdit m_edGridWidth;
	CEdit m_edRCCount;
	CEdit m_edRCSpaceX;
	CEdit m_edRCSpaceY;
	CButton m_chkRCVertical;
	CButton m_chkLockRot;
	CEdit m_edRotAlign;
	CEdit m_edRCRename;
	afx_msg void OnBnClickedRcRealign();
	afx_msg void OnBnClickedRcRename();
	afx_msg void OnDeltaposSpin5(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnSetfocusOffsetY3();
	afx_msg void OnBnClickedCheck1();
};

float	Get3DGridSize();
VOID	Set3DGridSize(float f);
float	GetRotAlign();
BOOL	IsRotAlign();

CAlignDialog3D*	GetAlignDialog3D();
