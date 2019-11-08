/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// DlgMixFPS.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgMixFPS.h"
#include "afxdialogex.h"


// CDlgMixFPS 对话框

IMPLEMENT_DYNAMIC(CDlgMixFPS, CDialogBase)

CDlgMixFPS::CDlgMixFPS(CWnd* pParent /*=NULL*/)
	: CDialogBase(CDlgMixFPS::IDD, pParent)
{

}

CDlgMixFPS::~CDlgMixFPS()
{
}

void CDlgMixFPS::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edFPS);
}


BEGIN_MESSAGE_MAP(CDlgMixFPS, CDialogBase)
	ON_BN_CLICKED(IDOK, &CDlgMixFPS::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgMixFPS 消息处理程序
namespace Phantom{
extern	VOID		SetMixFPS(INT nMixFPS);
};

void CDlgMixFPS::OnBnClickedOk()
{
	CString str;
	m_edFPS.GetWindowTextW(str);
	SetMixFPS(_wtoi(str));
	OnOK();
}


BOOL CDlgMixFPS::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	m_edFPS.SetWindowTextW(L"15");
	return TRUE;
}
