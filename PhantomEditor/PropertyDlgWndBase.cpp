/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// PropertyDlgWndBase.cpp : 实现文件
//

#include "stdafx.h"
#include "phantom-gc.h"
#include "PropertyDlgWndBase.h"
#include "afxdialogex.h"


// CPropertyDlgWndBase 对话框

IMPLEMENT_DYNAMIC(CPropertyDlgWndBase, CDialogEx)

CPropertyDlgWndBase::CPropertyDlgWndBase(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPropertyDlgWndBase::IDD, pParent)
{

}

CPropertyDlgWndBase::~CPropertyDlgWndBase()
{
}

void CPropertyDlgWndBase::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertyDlgWndBase, CDialogEx)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CPropertyDlgWndBase 消息处理程序


void CPropertyDlgWndBase::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnRButtonDown(nFlags, point);
}


void CPropertyDlgWndBase::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnRButtonUp(nFlags, point);
}
