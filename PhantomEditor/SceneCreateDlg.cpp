// SceneCreateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "SceneCreateDlg.h"
#include "afxdialogex.h"
#include "GCDefine.h"
#include "MainFrm.h"

// CSceneCreateDlg 对话框

IMPLEMENT_DYNAMIC(CSceneCreateDlg, CDialogBase)

CSceneCreateDlg::CSceneCreateDlg(CWnd* pParent /*=NULL*/)
	: CDialogBase(CSceneCreateDlg::IDD, pParent)
{

}

CSceneCreateDlg::~CSceneCreateDlg()
{
}

void CSceneCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID_SIZE, m_edGridSize);
	DDX_Control(pDX, IDC_MAP_SIZE_LEVEL, m_cbMapSizeLevel);
}


BEGIN_MESSAGE_MAP(CSceneCreateDlg, CDialogBase)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

// CSceneCreateDlg 消息处理程序

void CSceneCreateDlg::OnBnClickedOk()
{
	CString strGridSize, strMapSize;
	m_edGridSize.GetWindowText(strGridSize);
	m_nMapSizeLevel = m_cbMapSizeLevel.GetItemData(m_cbMapSizeLevel.GetCurSel());
	m_fGridSize		=	_wtof(strGridSize);
	if(m_fGridSize<0.1||m_fGridSize>1000.0f)
		return;
	OnOK();
}

void CSceneCreateDlg::OnBnClickedCancel()
{
	OnCancel();
}

BOOL CSceneCreateDlg::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	m_edGridSize.SetWindowText(L"3");
	m_cbMapSizeLevel.SetItemData(m_cbMapSizeLevel.AddString(L"32"), 32);
	m_cbMapSizeLevel.SetItemData(m_cbMapSizeLevel.AddString(L"64"), 64);
	m_cbMapSizeLevel.SetItemData(m_cbMapSizeLevel.AddString(L"128"), 128);
	m_cbMapSizeLevel.SetItemData(m_cbMapSizeLevel.AddString(L"256"), 256);
	m_cbMapSizeLevel.SetItemData(m_cbMapSizeLevel.AddString(L"512"), 512);
	m_cbMapSizeLevel.SetItemData(m_cbMapSizeLevel.AddString(L"1024"), 1024);
	m_cbMapSizeLevel.SetItemData(m_cbMapSizeLevel.AddString(L"2048"), 2048);
	m_cbMapSizeLevel.SetItemData(m_cbMapSizeLevel.AddString(L"4096"), 4096);
	m_cbMapSizeLevel.SetItemData(m_cbMapSizeLevel.AddString(L"8192"), 8192);
	m_cbMapSizeLevel.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
}
