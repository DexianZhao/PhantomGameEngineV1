/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// DlgFindReplace.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgFindReplace.h"
#include "afxdialogex.h"
#include "CodeEditor.h"
#include "resource.h"
// CDlgFindReplace 对话框

IMPLEMENT_DYNAMIC(CDlgFindReplace, CDialogEx)

CDlgFindReplace::CDlgFindReplace(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgFindReplace::IDD, pParent)
{
	m_editor = 0;
}

CDlgFindReplace::~CDlgFindReplace()
{
}

void CDlgFindReplace::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FIND_TEXT, m_edFindText);
	DDX_Control(pDX, IDC_REPLACE_TEXT, m_edReplaceText);
	DDX_Control(pDX, IDC_CHK_REPLACE, m_chkReplace);
	DDX_Control(pDX, IDREPLACE, m_btReplace);
}


BEGIN_MESSAGE_MAP(CDlgFindReplace, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgFindReplace::OnBnClickedOk)
	ON_BN_CLICKED(IDREPLACE, &CDlgFindReplace::OnBnClickedReplace)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_CHK_REPLACE, &CDlgFindReplace::OnBnClickedChkReplace)
END_MESSAGE_MAP()


// CDlgFindReplace 消息处理程序

VOID	CDlgFindReplace::ShowFind(const wchar_t* wt, BOOL bReplace)
{
	this->ShowWindow(SW_SHOW);
	this->m_edReplaceText.EnableWindow(bReplace);
	this->m_btReplace.EnableWindow(bReplace);
	this->m_chkReplace.SetCheck(bReplace);
	if(wt&&wcslen(wt)>0)
		this->m_edFindText.SetWindowTextW(wt);
	this->m_edFindText.SetFocus();
	m_edFindText.SetSel(0,-1);
}

void CDlgFindReplace::OnBnClickedOk()
{
	CString strFind;
	m_edFindText.GetWindowTextW(strFind);
	if(strFind.GetLength()==0)
	{
		MessageBox(L"写入要查找的文本");
		return;
	}
	long b,e;
	m_editor->GetSel(b,e);
	CString str;
	int tlen = m_editor->GetTextLength();
	this->m_editor->GetTextRange(0, tlen, str);
	const wchar_t* orig = str;
	const wchar_t* find = strFind;
	int findlen = strFind.GetLength();
	int origlen = str.GetLength();
	for(int i=0;i<origlen;i++)
	{
		int begin = i+e;
		if(begin>=origlen)
			begin-=origlen;
		if((begin+findlen)>=origlen)
			continue;
		if(orig[begin] == find[0]){
			if(wcsnicmp(orig+begin,find, findlen)==0)
			{
				m_editor->SetSel(begin,findlen+begin);
				m_editor->SetFocus();
				return;
			}
		}
	}
	MessageBox(L"找不到更多了");
	m_editor->SetFocus();
}


void CDlgFindReplace::OnBnClickedReplace()
{
	CString strFind;
	m_edFindText.GetWindowTextW(strFind);
	if(strFind.GetLength()==0)
	{
		MessageBox(L"写入要查找的文本");
		return;
	}
	CString strReplace;
	m_edReplaceText.GetWindowTextW(strReplace);
	long b,e;
	m_editor->GetSel(b,e);
	CString str;
	int tlen = m_editor->GetTextLength();
	this->m_editor->GetTextRange(b, tlen, str);
	const wchar_t* orig = str;
	const wchar_t* find = strFind;
	int findlen = strFind.GetLength();
	int origlen = str.GetLength();
	for(int i=0;i<(origlen - findlen);i++)
	{
		if(orig[i] == find[0]){
			if(wcsnicmp(orig+i,find, findlen)==0)
			{
				m_editor->SetSel(b+i,findlen+b+i);
				if(i==0){
					m_editor->ReplaceTextW(strReplace);
				}
				m_editor->SetFocus();
				return;
			}
		}
	}
	MessageBox(L"找不到更多了");
	m_editor->SetFocus();
}


void CDlgFindReplace::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 在此处添加消息处理程序代码
}


void CDlgFindReplace::OnBnClickedChkReplace()
{
	this->m_edReplaceText.EnableWindow(m_chkReplace.GetCheck());
	this->m_btReplace.EnableWindow(m_chkReplace.GetCheck());
}


BOOL CDlgFindReplace::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN||pMsg->message == WM_KEYUP){
		if(pMsg->wParam == L'F'&&HIBYTE(GetKeyState(VK_CONTROL)))
		{
			if(pMsg->message == WM_KEYUP)
				this->OnBnClickedOk();
			return true;
		}
		if(pMsg->wParam == L'R'&&HIBYTE(GetKeyState(VK_CONTROL)))
		{
			if(pMsg->message == WM_KEYUP)
				this->OnBnClickedChkReplace();
			return true;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
