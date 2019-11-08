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
#include "AlignDialog3D.h"

// CAlignDialog3D 对话框

float	g_fRotAlign = 15.0f;
BOOL	g_bRotAlign = false;


CAlignDialog3D*	g_alignDialog	=	0;
CAlignDialog3D*	GetAlignDialog3D()
{
	return g_alignDialog;
}

float	g_3dGridSize;
float Get3DGridSize()
{
	return g_3dGridSize;
}
VOID	Set3DGridSize(float f)
{
	g_3dGridSize	=	f;
}

IMPLEMENT_DYNAMIC(CAlignDialog3D, CDialogBase)

CAlignDialog3D::CAlignDialog3D(CWnd* pParent /*=NULL*/)
	: CDialogBase(CAlignDialog3D::IDD, pParent)
{
	g_3dGridSize	=	To3DValue(1.0f);
	g_alignDialog	=	this;
}

CAlignDialog3D::~CAlignDialog3D()
{
	g_alignDialog	=	0;
}

void CAlignDialog3D::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OFFSET_X, m_edOffsetX);
	DDX_Control(pDX, IDC_OFFSET_Y, m_edOffsetY);
	DDX_Control(pDX, IDC_OFFSET_Y3, m_edOffsetZ);
	DDX_Control(pDX, IDC_ALIGN_LEFT, m_edAlignMinX);
	DDX_Control(pDX, IDC_ALIGN_TOP, m_edAlignMinY);
	DDX_Control(pDX, IDC_ALIGN_RIGHT, m_edAlignMaxX);
	DDX_Control(pDX, IDC_ALIGN_BOTTOM, m_edAlignMaxY);
	DDX_Control(pDX, IDC_ALIGN_TOP2, m_edAlignMinZ);
	DDX_Control(pDX, IDC_ALIGN_BOTTOM2, m_edAlignMaxZ);
	DDX_Control(pDX, IDC_ALIGN_WIDTH, m_edAlignWidth);
	DDX_Control(pDX, IDC_ALIGN_HEIGHT, m_edAlignHeight);
	DDX_Control(pDX, IDC_ALIGN_HEIGHT2, m_edAlignLength);
	DDX_Control(pDX, IDC_ALIGN_SPACE_X, m_edAlignSpaceX);
	DDX_Control(pDX, IDC_ALIGN_SPACE_Y, m_edAlignSpaceY);
	DDX_Control(pDX, IDC_ALIGN_SPACE_Y2, m_edAlignSpaceZ);
	DDX_Control(pDX, IDC_OFFSET_X2, m_edRotAlign);
	//
	DDX_Control(pDX, IDC_GRID_WIDTH, m_edGridWidth);
	DDX_Control(pDX, IDC_RC_ALIGN_COUNT, m_edRCCount);
	DDX_Control(pDX, IDC_RC_SPACE_X, m_edRCSpaceX);
	DDX_Control(pDX, IDC_RC_SPACE_Y, m_edRCSpaceY);
	DDX_Control(pDX, IDC_RC_IsVertical, m_chkRCVertical);
	DDX_Control(pDX, IDC_CHECK1, m_chkLockRot);
	DDX_Control(pDX, IDC_ALIGN_MATRIX_X2, m_edRCRename);
}

BEGIN_MESSAGE_MAP(CAlignDialog3D, CDialogBase)
	ON_BN_CLICKED(IDOK, &CAlignDialog3D::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAlignDialog3D::OnBnClickedCancel)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CAlignDialog3D::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, &CAlignDialog3D::OnDeltaposSpin2)
	ON_EN_SETFOCUS(IDC_OFFSET_X, &CAlignDialog3D::OnEnSetfocusOffsetX)
	ON_EN_SETFOCUS(IDC_OFFSET_Y, &CAlignDialog3D::OnEnSetfocusOffsetY)
	ON_BN_CLICKED(IDC_RC_REALIGN, &CAlignDialog3D::OnBnClickedRcRealign)
	ON_BN_CLICKED(IDC_RC_RENAME, &CAlignDialog3D::OnBnClickedRcRename)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN5, &CAlignDialog3D::OnDeltaposSpin5)
	ON_EN_SETFOCUS(IDC_OFFSET_Y3, &CAlignDialog3D::OnEnSetfocusOffsetY3)
	ON_BN_CLICKED(IDC_CHECK1, &CAlignDialog3D::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CAlignDialog3D 消息处理程序

VOID	CAlignDialog3D::OnActiveView(GCViewBase* base)
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
	m_edOffsetZ.EnableWindow(bEnable);
	m_edAlignMinX.EnableWindow(bEnable);
	m_edAlignMinY.EnableWindow(bEnable);
	m_edAlignMaxX.EnableWindow(bEnable);
	m_edAlignMaxY.EnableWindow(bEnable);
	//
	m_edAlignMinZ.EnableWindow(bEnable);
	m_edAlignMaxZ.EnableWindow(bEnable);
	//
	m_edAlignSpaceX.EnableWindow(bEnable);
	m_edAlignSpaceY.EnableWindow(bEnable);
	m_edAlignSpaceZ.EnableWindow(bEnable);
	m_edAlignWidth.EnableWindow(bEnable);
	m_edAlignHeight.EnableWindow(bEnable);
	m_edAlignLength.EnableWindow(bEnable);
	m_edRotAlign.EnableWindow(bEnable);
	//
	m_edRCCount.EnableWindow(bEnable);
	m_edRCSpaceX.EnableWindow(bEnable);
	m_edRCSpaceY.EnableWindow(bEnable);
	m_chkRCVertical.EnableWindow(bEnable);
	m_chkLockRot.EnableWindow(bEnable);
	m_edRCRename.EnableWindow(bEnable);
	//
	m_edGridWidth.EnableWindow(bGridEnable);
}

BOOL	CAlignDialog3D::GetFocusEditor(CEdit* edit, AlignType align, BOOL bResetValue)
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

void CAlignDialog3D::OnBnClickedOk()
{
	if(GetFocusEditor(&m_edOffsetX, AlignType_OffsetX, TRUE))
		return;
	if(GetFocusEditor(&m_edOffsetY, AlignType_OffsetY, TRUE))
		return;
	if(GetFocusEditor(&m_edOffsetZ, AlignType_OffsetZ, TRUE))
		return;
	if(GetFocusEditor(&m_edAlignMinX, AlignType_Left))
		return;
	if(GetFocusEditor(&m_edAlignMinY, AlignType_Top))
		return;
	if(GetFocusEditor(&m_edAlignMaxX, AlignType_Right))
		return;
	if(GetFocusEditor(&m_edAlignMinZ, AlignType_MinZ))
		return;
	if(GetFocusEditor(&m_edAlignMaxZ, AlignType_MaxZ))
		return;
	if(GetFocusEditor(&m_edAlignMaxY, AlignType_Bottom))
		return;
	if(GetFocusEditor(&m_edAlignSpaceX, AlignType_SpaceX))
		return;
	if(GetFocusEditor(&m_edAlignSpaceZ, AlignType_SpaceZ))
		return;
	if(GetFocusEditor(&m_edAlignSpaceY, AlignType_SpaceY))
		return;
	if(GetFocusEditor(&m_edAlignWidth, AlignType_Width))
		return;
	if(GetFocusEditor(&m_edAlignHeight, AlignType_Height))
		return;
	if(GetFocusEditor(&m_edAlignLength, AlignType_Length))
		return;
	{
		BOOL bEnable	=	(this->m_edGridWidth.m_hWnd == ::GetFocus());
		if(bEnable)
		{
			float nValue		=	0;
			CString str;
			m_edGridWidth.GetWindowTextW(str);
			nValue		=	_wtof(str);
			nValue		=	clamp(nValue, 1, 500);
			g_3dGridSize	=	nValue;
			SetConfigDouble("UIGridWidth", nValue);
		}
	}
}

void CAlignDialog3D::OnBnClickedCancel()
{
	ShowWindow(SW_HIDE);
}


void CAlignDialog3D::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
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


void CAlignDialog3D::OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult)
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


void CAlignDialog3D::OnEnSetfocusOffsetX()
{
	m_edOffsetX.SetSel(0, -1);
}


void CAlignDialog3D::OnEnSetfocusOffsetY()
{
	m_edOffsetY.SetSel(0, -1);
}


BOOL CAlignDialog3D::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	//
	AddTool(m_edOffsetX, Language(L"UI_m_edOffsetX", L"设置左右偏移量"));
	AddTool(m_edOffsetY, Language(L"UI_m_edOffsetY", L"设置上下偏移量"));
	AddTool(m_edAlignMinX, Language(L"3D_m_edAlignLeft", L"设置最小 X 对齐"));
	AddTool(m_edAlignMinY, Language(L"3D_m_edAlignTop", L"设置最小 Y 对齐"));
	AddTool(m_edAlignMaxX, Language(L"3D_m_edAlignRight", L"设置最大 X 对齐"));
	AddTool(m_edAlignMaxY, Language(L"3D_m_edAlignBottom", L"设置最大 Y 对齐"));
	AddTool(m_edAlignMinZ, Language(L"3D_m_edAlignMinZ", L"设置最小 Z 对齐"));
	AddTool(m_edAlignMaxZ, Language(L"3D_m_edAlignMaxZ", L"设置最大 Z 对齐"));
	AddTool(m_edAlignSpaceX, Language(L"UI_m_edAlignSpaceX", L"设置选择控件之间的左右间距"));
	AddTool(m_edAlignSpaceY, Language(L"UI_m_edAlignSpaceY", L"设置选择控件之间的上下间距"));
	AddTool(m_edAlignWidth, Language(L"3D_m_edAlignWidth", L"设置统一 X 缩放"));
	AddTool(m_edAlignHeight, Language(L"3D_m_edAlignHeight", L"设置统一 Y 缩放"));
	AddTool(m_edAlignLength, Language(L"3D_m_edAlignLength", L"设置统一 Z 缩放"));
	AddTool(m_edGridWidth, Language(L"UI_m_edGridWidth", L"设置对齐格子宽度(米)"));
	//
	AddTool(m_edRCCount, Language(L"UI_m_edRCCount", L"行列排序中使用的，横向[纵向]最大数量"));
	AddTool(m_edRCSpaceX, Language(L"UI_m_edRCSpaceX", L"行列排序中使用的，横向间隔"));
	AddTool(m_edRCSpaceY, Language(L"UI_m_edRCSpaceY", L"行列排序中使用的，纵向间隔"));
	AddTool(m_chkRCVertical, Language(L"UI_m_chkRCVertical", L"行列排序中使用的，是否先从纵向开始排序"));
	AddTool(m_chkLockRot, Language(L"3d_m_chkLockRot", L"是否固定转动幅度"));
	AddTool(m_edRCRename, Language(L"UI_m_edRCRename", L"行列排序中使用的，重命名所有对象"));
	//
	m_edOffsetX.SetWindowTextW(L"0");
	m_edOffsetY.SetWindowTextW(L"0");
	m_edOffsetZ.SetWindowTextW(L"0");
	m_edAlignMinX.SetWindowTextW(L"0");
	m_edAlignMinY.SetWindowTextW(L"0");
	m_edAlignMinZ.SetWindowTextW(L"0");
	//
	m_edRCCount.SetWindowTextW(L"10");
	m_edRCSpaceX.SetWindowTextW(L"0");
	m_edRCSpaceY.SetWindowTextW(L"0");
	m_edRCRename.SetWindowTextW(L"RC");
	//
	wchar_t buf[128];
	swprintf(buf, L"%.01f", g_3dGridSize = GetConfigDouble("UIGridWidth", 1));
	this->m_edGridWidth.SetWindowTextW(buf);
	//
	g_bRotAlign = GetConfigBool("b_rotAlign", 1);
	g_fRotAlign = GetConfigDouble("f_rotAlign", 15.0f);
	this->m_chkLockRot.SetCheck(g_bRotAlign);
	swprintf(buf, L"%.01f", g_fRotAlign);
	this->m_edRotAlign.SetWindowTextW(buf);
	return TRUE;
}


void CAlignDialog3D::OnBnClickedRcRealign()
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


void CAlignDialog3D::OnBnClickedRcRename()
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


void CAlignDialog3D::OnDeltaposSpin5(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if(pNMUpDown->iDelta > 0)
	{
		this->m_edOffsetZ.SetWindowText(L"1");
	}
	else
	{
		this->m_edOffsetZ.SetWindowText(L"-1");
	}
	m_edOffsetZ.SetFocus();
	OnBnClickedOk();
	*pResult = 0;
}


void CAlignDialog3D::OnEnSetfocusOffsetY3()
{
	m_edOffsetZ.SetSel(0, -1);
}

float	GetRotAlign()
{
	return g_fRotAlign;
}
BOOL	IsRotAlign()
{
	return g_bRotAlign;
}

void CAlignDialog3D::OnBnClickedCheck1()
{
	g_bRotAlign = this->m_chkLockRot.GetCheck();
	CString s;
	m_edRotAlign.GetWindowTextW(s);
	g_fRotAlign	=	_wtof(s);
	SetConfigBool("b_rotAlign", g_bRotAlign);
	SetConfigDouble("f_rotAlign", g_fRotAlign);
}
