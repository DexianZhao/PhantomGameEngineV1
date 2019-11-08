/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// DlgFindText.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgFindText.h"
#include "afxdialogex.h"


// DlgFindText 对话框

IMPLEMENT_DYNAMIC(DlgFindText, CDialogEx)

DlgFindText::DlgFindText(CWnd* pParent /*=NULL*/)
	: CDialogEx(DlgFindText::IDD, pParent)
{

}

DlgFindText::~DlgFindText()
{
}

void DlgFindText::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgFindText, CDialogEx)
END_MESSAGE_MAP()


// DlgFindText 消息处理程序
