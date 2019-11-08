/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// DialogBase.cpp : 实现文件
//

#include "stdafx.h"
#include "DialogBase.h"
#include "afxdialogex.h"
#include "GCViewManager.h"


// CDialogBase 对话框

IMPLEMENT_DYNAMIC(CDialogBase, CDialogEx)

CDialogBase::CDialogBase(UINT nID, CWnd* pParent /*=NULL*/)
	: CDialogEx(nID, pParent)
{

}

CDialogBase::~CDialogBase()
{
}

void CDialogBase::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogBase, CDialogEx)
END_MESSAGE_MAP()


// CDialogBase 消息处理程序

BOOL	CDialogBase::OnInitDialog()
{
	if(!__super::OnInitDialog())
		return false;
	if(GetLanguagesPtr())
		GetLanguagesPtr()->SetChildsText(m_hWnd);
	InitTool(m_toolTip, this);
	ShowWindow(SW_SHOW);
	return true;
}

VOID	CDialogBase::AddTool(CWnd& wnd, const wchar_t* szText)
{
	::AddTool(m_toolTip, wnd, szText);
}

BOOL	CDialogBase::PreTranslateMessage(MSG* pMsg)
{
	m_toolTip.RelayEvent( pMsg );
	return __super::PreTranslateMessage(pMsg);
}
