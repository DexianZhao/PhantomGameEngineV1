/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// DlgGameEditBox.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgGameEditBox.h"
#include "DlgEditBox.h"

// DlgGameEditBox

IMPLEMENT_DYNAMIC(DlgGameEditBox, CEdit)

DlgGameEditBox::DlgGameEditBox()
{
	m_dlgEdit = 0;
}

DlgGameEditBox::~DlgGameEditBox()
{
}


BEGIN_MESSAGE_MAP(DlgGameEditBox, CEdit)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// DlgGameEditBox 消息处理程序




void DlgGameEditBox::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	m_dlgEdit->OnBnClickedOk();
	// TODO: 在此处添加消息处理程序代码
}
