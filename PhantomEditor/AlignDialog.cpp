/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/

//

#include "stdafx.h"
#include "AlignDialog.h"
#include "afxdialogex.h"
#include "GCViewUIEditor.h"
#include "GCViewManager.h"

// CAlignDialog 对话框

CAlignDialog*	g_alignDialog	=	0;
CAlignDialog*	GetAlignDialog()
{
	return g_alignDialog;
}

CPoint	g_uiGridSize;
CPoint	GetUIGridSize()
{
	return g_uiGridSize;
}
VOID	SetUIGridSize(const CPoint& p)
{
	g_uiGridSize	=	p;
}

IMPLEMENT_DYNAMIC(CAlignDialog, CDialogBase)

CAlignDialog::CAlignDialog(CWnd* pParent /*=NULL*/)
	: CDialogBase(CAlignDialog::IDD, pParent)
{
	g_uiGridSize.SetPoint(10, 10);
	g_alignDialog	=	this;
}

CAlignDialog::~CAlignDialog()
{
	g_alignDialog	=	0;
}

void CAlignDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OFFSET_X, m_edOffsetX);
	DDX_Control(pDX, IDC_OFFSET_Y, m_edOffsetY);
	DDX_Control(pDX, IDC_ALIGN_LEFT, m_edAlignLeft);
	DDX_Control(pDX, IDC_ALIGN_TOP, m_edAlignTop);
	DDX_Control(pDX, IDC_ALIGN_RIGHT, m_edAlignRight);
	DDX_Control(pDX, IDC_ALIGN_BOTTOM, m_edAlignBottom);
	DDX_Control(pDX, IDC_ALIGN_WIDTH, m_edAlignWidth);
	DDX_Control(pDX, IDC_ALIGN_HEIGHT, m_edAlignHeight);
	DDX_Control(pDX, IDC_ALIGN_SPACE_X, m_edAlignSpaceX);
	DDX_Control(pDX, IDC_ALIGN_SPACE_Y, m_edAlignSpaceY);
	//
	DDX_Control(pDX, IDC_GRID_WIDTH, m_edGridWidth);
	DDX_Control(pDX, IDC_GRID_HEIGHT, m_edGridHeight);
	DDX_Control(pDX, IDC_RC_ALIGN_COUNT, m_edRCCount);
	DDX_Control(pDX, IDC_RC_SPACE_X, m_edRCSpaceX);
	DDX_Control(pDX, IDC_RC_SPACE_Y, m_edRCSpaceY);
	DDX_Control(pDX, IDC_RC_IsVertical, m_chkRCVertical);
	DDX_Control(pDX, IDC_CENTERALIGN, m_chkCenter);
	DDX_Control(pDX, IDC_ALIGN_MATRIX_X2, m_edRCRename);
}

BEGIN_MESSAGE_MAP(CAlignDialog, CDialogBase)
	ON_BN_CLICKED(IDOK, &CAlignDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAlignDialog::OnBnClickedCancel)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CAlignDialog::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, &CAlignDialog::OnDeltaposSpin2)
	ON_EN_SETFOCUS(IDC_OFFSET_X, &CAlignDialog::OnEnSetfocusOffsetX)
	ON_EN_SETFOCUS(IDC_OFFSET_Y, &CAlignDialog::OnEnSetfocusOffsetY)
	ON_BN_CLICKED(IDC_RC_REALIGN, &CAlignDialog::OnBnClickedRcRealign)
	ON_BN_CLICKED(IDC_RC_RENAME, &CAlignDialog::OnBnClickedRcRename)
	ON_BN_CLICKED(IDC_CHECK1, &CAlignDialog::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CAlignDialog 消息处理程序

VOID	CAlignDialog::OnActiveView(GCViewBase* base)
{
	BOOL bEnable		=	false;
	BOOL bGridEnable	=	false;
	if(base)
	{
		if(base->GetViewerType() == FileViewerType_UI || base->GetViewerType() == FileViewerType_Scene)
		{
			bEnable			=	true;
			if(base->GetViewerType() == FileViewerType_UI)
				bGridEnable	=	true;
		}
	}
	//
	m_edOffsetX.EnableWindow(bEnable);
	m_edOffsetY.EnableWindow(bEnable);
	m_edAlignLeft.EnableWindow(bEnable);
	m_edAlignTop.EnableWindow(bEnable);
	m_edAlignRight.EnableWindow(bEnable);
	m_edAlignBottom.EnableWindow(bEnable);
	//
	m_edAlignSpaceX.EnableWindow(bEnable);
	m_edAlignSpaceY.EnableWindow(bEnable);
	m_edAlignWidth.EnableWindow(bEnable);
	m_edAlignHeight.EnableWindow(bEnable);
	//
	m_edRCCount.EnableWindow(bEnable);
	m_edRCSpaceX.EnableWindow(bEnable);
	m_edRCSpaceY.EnableWindow(bEnable);
	m_chkRCVertical.EnableWindow(bEnable);
	m_edRCRename.EnableWindow(bEnable);
	m_chkCenter.EnableWindow(bEnable);
	if(base->GetViewerType() == FileViewerType_UI)
	{
		//m_chkCenter.EnableWindow(static_cast<GCViewUIEditor*>(base)->m_bCenterAlign);
	}
	//
	m_edGridWidth.EnableWindow(bGridEnable);
	m_edGridHeight.EnableWindow(bGridEnable);
}

BOOL	CAlignDialog::GetFocusEditor(CEdit* edit, AlignType align, BOOL bResetValue)
{
	BOOL bEnable	=	(edit->m_hWnd == ::GetFocus());
	if(!bEnable)
		return false;
	INT nValue		=	0;
	CString str;
	edit->GetWindowTextW(str);
	nValue		=	_wtoi(str);
	if(GetViewManager()->GetActiveView())
		GetViewManager()->GetActiveView()->AlignObjects(align, nValue);
	if(bResetValue)
		edit->SetWindowTextW(L"0");
	edit->SetSel(0, -1);
	return bEnable;
}

void CAlignDialog::OnBnClickedOk()
{
	if(GetFocusEditor(&m_edOffsetX, AlignType_OffsetX, TRUE))
		return;
	if(GetFocusEditor(&m_edOffsetY, AlignType_OffsetY, TRUE))
		return;
	if(GetFocusEditor(&m_edAlignLeft, AlignType_Left))
		return;
	if(GetFocusEditor(&m_edAlignTop, AlignType_Top))
		return;
	if(GetFocusEditor(&m_edAlignRight, AlignType_Right))
		return;
	if(GetFocusEditor(&m_edAlignBottom, AlignType_Bottom))
		return;
	if(GetFocusEditor(&m_edAlignSpaceX, AlignType_SpaceX))
		return;
	if(GetFocusEditor(&m_edAlignSpaceY, AlignType_SpaceY))
		return;
	if(GetFocusEditor(&m_edAlignWidth, AlignType_Width))
		return;
	if(GetFocusEditor(&m_edAlignHeight, AlignType_Height))
		return;
	{
		BOOL bEnable	=	(this->m_edGridWidth.m_hWnd == ::GetFocus());
		if(bEnable)
		{
			INT nValue		=	0;
			CString str;
			m_edGridWidth.GetWindowTextW(str);
			nValue		=	_wtoi(str);
			nValue		=	clamp(nValue, 1, 500);
			g_uiGridSize.x	=	nValue;
			SetConfigInt("UIGridWidth", nValue);
		}
	}
	{
		BOOL bEnable	=	(this->m_edGridHeight.m_hWnd == ::GetFocus());
		if(bEnable)
		{
			INT nValue		=	0;
			CString str;
			m_edGridHeight.GetWindowTextW(str);
			nValue		=	_wtoi(str);
			nValue		=	clamp(nValue, 1, 500);
			g_uiGridSize.y	=	nValue;
			SetConfigInt("UIGridHeight", nValue);
		}
	}
}

void CAlignDialog::OnBnClickedCancel()
{
	ShowWindow(SW_HIDE);
}


void CAlignDialog::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if(pNMUpDown->iDelta > 0)
	{
		this->m_edOffsetX.SetWindowText(L"1");
	}
	else
	{
		this->m_edOffsetX.SetWindowText(L"-1");
	}
	m_edOffsetX.SetFocus();
	OnBnClickedOk();
	*pResult = 0;
}


void CAlignDialog::OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if(pNMUpDown->iDelta > 0)
	{
		this->m_edOffsetY.SetWindowText(L"1");
	}
	else
	{
		this->m_edOffsetY.SetWindowText(L"-1");
	}
	m_edOffsetY.SetFocus();
	OnBnClickedOk();
	*pResult = 0;
}


void CAlignDialog::OnEnSetfocusOffsetX()
{
	m_edOffsetX.SetSel(0, -1);
}


void CAlignDialog::OnEnSetfocusOffsetY()
{
	m_edOffsetY.SetSel(0, -1);
}


BOOL CAlignDialog::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	//
	AddTool(m_edOffsetX, Language(L"UI_m_edOffsetX", L"设置左右偏移量"));
	AddTool(m_edOffsetY, Language(L"UI_m_edOffsetY", L"设置上下偏移量"));
	AddTool(m_edAlignLeft, Language(L"UI_m_edAlignLeft", L"设置左边对齐"));
	AddTool(m_edAlignTop, Language(L"UI_m_edAlignTop", L"设置上边对齐"));
	AddTool(m_edAlignRight, Language(L"UI_m_edAlignRight", L"设置右边对齐"));
	AddTool(m_edAlignBottom, Language(L"UI_m_edAlignBottom", L"设置下边对齐"));
	AddTool(m_edAlignSpaceX, Language(L"UI_m_edAlignSpaceX", L"设置选择控件之间的左右间距"));
	AddTool(m_edAlignSpaceY, Language(L"UI_m_edAlignSpaceY", L"设置选择控件之间的上下间距"));
	AddTool(m_edAlignWidth, Language(L"UI_m_edAlignWidth", L"设置统一宽度"));
	AddTool(m_edAlignHeight, Language(L"UI_m_edAlignHeight", L"设置统一高度"));
	AddTool(m_edGridWidth, Language(L"UI_m_edGridWidth", L"设置对齐格子宽度"));
	AddTool(m_edGridHeight, Language(L"UI_m_edGridHeight", L"设置对齐格子高度"));
	//
	AddTool(m_edRCCount, Language(L"UI_m_edRCCount", L"行列排序中使用的，横向[纵向]最大数量"));
	AddTool(m_edRCSpaceX, Language(L"UI_m_edRCSpaceX", L"行列排序中使用的，横向间隔"));
	AddTool(m_edRCSpaceY, Language(L"UI_m_edRCSpaceY", L"行列排序中使用的，纵向间隔"));
	AddTool(m_chkRCVertical, Language(L"UI_m_chkRCVertical", L"行列排序中使用的，是否先从纵向开始排序"));
	AddTool(m_chkCenter, Language(L"m_chkCenter", L"是否以格子中心点和控件中心位置对齐"));
	AddTool(m_edRCRename, Language(L"UI_m_edRCRename", L"行列排序中使用的，重命名所有对象"));
	//
	m_edOffsetX.SetWindowTextW(L"0");
	m_edOffsetY.SetWindowTextW(L"0");
	m_edAlignLeft.SetWindowTextW(L"0");
	m_edAlignTop.SetWindowTextW(L"0");
	//
	m_edRCCount.SetWindowTextW(L"10");
	m_edRCSpaceX.SetWindowTextW(L"0");
	m_edRCSpaceY.SetWindowTextW(L"0");
	m_edRCRename.SetWindowTextW(L"RC");
	//
	wchar_t buf[128];
	swprintf(buf, L"%d", g_uiGridSize.x = GetConfigInt("UIGridWidth", 10));
	this->m_edGridWidth.SetWindowTextW(buf);
	swprintf(buf, L"%d", g_uiGridSize.y = GetConfigInt("UIGridHeight", 10));
	this->m_edGridHeight.SetWindowTextW(buf);
	//ShowUIGrid(GetConfigInt("UIGridVisible", 1));
	//
	return TRUE;
}


void CAlignDialog::OnBnClickedRcRealign()
{
	CString rcCount;
	CString rcSpaceX;
	CString rcSpaceY;
	CString rcName;
	m_edRCCount.GetWindowTextW(rcCount);
	m_edRCSpaceX.GetWindowTextW(rcSpaceX);
	m_edRCSpaceY.GetWindowTextW(rcSpaceY);
	m_edRCRename.GetWindowTextW(rcName);
	if(GetViewManager()->GetActiveView())
		GetViewManager()->GetActiveView()->AlignRCObjects(Phantom::Size(_wtoi(rcSpaceX), _wtoi(rcSpaceY)), _wtoi(rcCount), m_chkRCVertical.GetCheck(), false);
}


void CAlignDialog::OnBnClickedRcRename()
{
	CString rcCount;
	CString rcSpaceX;
	CString rcSpaceY;
	CString rcName;
	m_edRCCount.GetWindowTextW(rcCount);
	m_edRCSpaceX.GetWindowTextW(rcSpaceX);
	m_edRCSpaceY.GetWindowTextW(rcSpaceY);
	m_edRCRename.GetWindowTextW(rcName);
	if(GetViewManager()->GetActiveView())
		GetViewManager()->GetActiveView()->AlignRCObjects(Phantom::Size(_wtoi(rcSpaceX), _wtoi(rcSpaceY)), _wtoi(rcCount), m_chkRCVertical.GetCheck(), true, rcName);
}


void CAlignDialog::OnBnClickedCheck1()
{
	if(GetViewManager()->GetActiveView())
		GetViewManager()->GetActiveView()->AlignObjects(AlignType_CenterAlign, this->m_chkCenter.GetCheck());
}
