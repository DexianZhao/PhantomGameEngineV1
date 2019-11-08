/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// GameScreenSize.cpp : 实现文件
//

#include "stdafx.h"
#include "GameScreenSize.h"
#include "afxdialogex.h"
#include "GCMFCHelper.h"
#include "GCRenderSystem.h"

// CGameScreenSize 对话框

IMPLEMENT_DYNAMIC(CGameScreenSize, CDialogEx)

CGameScreenSize::CGameScreenSize(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGameScreenSize::IDD, pParent)
{

}

CGameScreenSize::~CGameScreenSize()
{
}

void CGameScreenSize::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCREEN_X, m_edScreenX);
	DDX_Control(pDX, IDC_SCREEN_Y, m_edScreenY);
}


BEGIN_MESSAGE_MAP(CGameScreenSize, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGameScreenSize::OnBnClickedOk)
END_MESSAGE_MAP()


// CGameScreenSize 消息处理程序


void CGameScreenSize::OnBnClickedOk()
{
	wchar_t buf[128];
	m_edScreenX.GetWindowTextW(buf, 128);
	SetConfigInt("screenX", _wtoi(buf));
	m_edScreenY.GetWindowTextW(buf, 128);
	SetConfigInt("screenY", _wtoi(buf));
	SetRenderWindowSize(GetConfigInt("screenX", 800), GetConfigInt("screenY", 600));
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


BOOL CGameScreenSize::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ShowWindow(SW_SHOW);
	//
	char buf[128];
	sprintf(buf, "%d", GetConfigInt("screenX", 800));
	this->m_edScreenX.SetWindowTextW(W(buf));
	sprintf(buf, "%d", GetConfigInt("screenY", 600));
	this->m_edScreenY.SetWindowTextW(W(buf));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
