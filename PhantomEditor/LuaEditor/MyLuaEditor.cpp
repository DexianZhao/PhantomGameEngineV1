/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// MyLuaEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "MyLuaEditor.h"
#include "afxdialogex.h"
#ifdef EDITOR_OS
#include "PhantomDB.h"
#include "PhantomGame.h"
#include "GCViewBase.h"
#include "MainFrm.h"
#endif

// CMyLuaEditor 对话框

IMPLEMENT_DYNAMIC(CMyLuaEditor, CDialogEx)

CMyLuaEditor::CMyLuaEditor(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyLuaEditor::IDD, pParent)
{
	m_current = 0;
}

CMyLuaEditor::~CMyLuaEditor()
{
	if(m_current)
		delete m_current;
}

void CMyLuaEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMyLuaEditor, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMyLuaEditor::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMyLuaEditor::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// CMyLuaEditor 消息处理程序

BOOL CMyLuaEditor::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_bIsFirst = true;
	SetTimer(1, 1, 0);
	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);
	m_tooltip.SetMaxTipWidth(500);
	m_tooltip.SetDelayTime(10000, 0);
	this->RecalcRects();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

VOID		CMyLuaEditor::InvalidateDlg()
{
	this->Invalidate();
}
VOID		CMyLuaEditor::UpdateTipText(const wchar_t* str, CWnd* wnd)
{
	m_tooltip.UpdateTipText(str, wnd);
}
VOID		CMyLuaEditor::OffsetToolRect(int x, int y)
{
	CRect rc;
	m_tooltip.GetWindowRect(&rc);
	rc.OffsetRect(x, y);
	m_tooltip.MoveWindow(&rc);
}

CodeEditor*	CMyLuaEditor::Open(const char* szFile)
{
	if(m_current)
		delete m_current;
	m_current = new CodeEditor();
	NameT<512> temp=szFile;
	temp.setIsFile(); 
	CRect rc;
	GetClientRect(&rc);
	m_current->Setup(this, rc, GetFont(), IDD_TOOL_TIP, IDB_PLOT_LUA_ICON);
	m_current->m_dlgEditor = this;
	m_current->ShowWindow(SW_SHOW);
	m_current->SetInitFile(szFile);
	m_current->EnableToolTips(TRUE);
	char file[128];
	_splitpath(szFile, 0, 0, file, 0);
	m_current->m_name = W(file);
	m_tooltip.AddTool(m_current, L"");
	m_current->m_fileName=temp;
	FileName fpath = m_current->m_fileName.t + strlen(GetProjectPath());
	m_current->GetCodeMgr().AddEvent(L"oninit", Language(L"function oninit()--文件入口函数"), 0, 10001);
	m_current->GetCodeMgr().AddEvent(L"onexit", Language(L"function onexit()--文件被跳转之前被调用"), 0, 10002);
	m_current->GetCodeMgr().AddEvent(L"onmessage", Language(L"function onmessage(type,params,params2)--(type=string,params=string,param2=string)外部调用处理接口,特定情况下会使用,外部实用sendmessage调用"), 0, 10003);
	m_current->GetCodeMgr().AddEvent(L"onback", Language(L"function onback()--安卓系统中后退键按下式被调用,返回1表示由内部处理了"), 0, 10004);
	m_current->GetCodeMgr().AddEvent(L"ondownload", Language(L"function ondownload(file,result)--(file=string,result=string)文件下载之后被调用,result=fail表示失败,ok表示成功,file返回路径"), 0, 10005);
	fpath.setIsFile();
	char aiName[128];
	if(InitEvent(aiName, "ai_table", fpath))
	{
		Phantom::AIScript temp;
		for(int i=0;i<temp.GetEventCount();i++){
			char name[512],full[512];
			if(temp.GetEventName(name, full, i, 0))
				m_current->GetCodeMgr().AddEvent(Wutf8(name), Wutf8(full), 0, i);
		}
	}
	else if(InitEvent(aiName, "scene_table", fpath))
	{
		Phantom::SceneScript temp;
		for(int i=0;i<temp.GetEventCount();i++){
			char name[512],full[512];
			if(temp.GetEventName(name, full, i, 0))
				m_current->GetCodeMgr().AddEvent(Wutf8(name), Wutf8(full), 0, i);
		}
	}
	else if(InitEvent(aiName, "skill_table", fpath))
	{
		Phantom::SkillScript temp;
		for(int i=0;i<temp.GetEventCount();i++){
			char name[512],full[512];
			if(temp.GetEventName(name, full, i, 0))
				m_current->GetCodeMgr().AddEvent(Wutf8(name), Wutf8(full), 0, i);
		}
	}
	RecalcRects();
	this->Invalidate();
	return m_current;
}
BOOL		CMyLuaEditor::InitEvent(char* retAIName, const char* tableName, FileName& fpath)//是否存在这个脚本
{
	Phantom::Config* ai = Phantom::GetGameMgr()->SearchConfig(tableName);
	retAIName[0] = 0;
	if(ai){
		int nfile = ai->SearchCol("script");
		if(nfile>0){
			for(int i=0;i<ai->GetLineCount();i++){
				FileName f = ai->GetLine(i)->GetTextVal(nfile);
				f.setIsFile();
				if(fpath==f){
					strcpy(retAIName, ai->GetLine(i)->GetLineName());
					return true;
				}
			}
		}
	}
	return false;
}
VOID		CMyLuaEditor::SetModifyed(CodeEditor* e, BOOL b)
{
}
#define	LINE_WIDTH	30
#define EDITOR_TOP	25
VOID		CMyLuaEditor::RecalcRects()
{
	if(m_current&&m_current->m_hWnd){
		CRect rc,dlgRc;
		GetClientRect(&dlgRc);
		rc = dlgRc;
		CRect rcLeft,rcRight;
		rcLeft = rc;
		rcRight = rc;
		int w = rc.Width();
		rcLeft.right = w/2-1;
		rcRight.left = w/2;
		rcLeft.bottom=rcRight.bottom = 200;
		m_current->m_cbEvents.MoveWindow(&rcLeft);
		m_current->m_cbMethods.MoveWindow(&rcRight);
		rc.top	+= EDITOR_TOP;
		rc.left += LINE_WIDTH + 2;
		m_current->MoveWindow(&rc);
	}
}
void CMyLuaEditor::OnBnClickedOk()
{
}

void CMyLuaEditor::OnBnClickedCancel()
{
}

void CMyLuaEditor::OnSize(UINT nType, int cx, int cy)
{
	RecalcRects();
	if(!m_image.IsNull())
		m_image.Destroy();
	CDialogEx::OnSize(nType, cx, cy);
}
VOID CMyLuaEditor::RedrawLines()
{
	CRect rc;
	GetClientRect(&rc);
	rc.right = LINE_WIDTH;
	rc.top += EDITOR_TOP;
	this->InvalidateRect(&rc);
}
void CMyLuaEditor::OnPaint()
{
	CPaintDC paintdc(this);
	CRect rc;
	GetClientRect(&rc);
	if(m_image.IsNull())
		m_image.Create(rc.Width(), rc.Height(), 24);
	CDC& dc = *CDC::FromHandle(m_image.GetDC());
	if(m_current){
		dc.FillRect(&rc, &CBrush(RGB(20,20,20)));
		int begin = m_current->GetFirstVisibleLine();
		int count = m_current->GetLineCount();
		CFont* oldFont = dc.SelectObject(GetFont());
		CRect rcL(0, 0, LINE_WIDTH, rc.bottom);
		rcL.InflateRect(-1,-1);
		dc.FillRect(&rcL, &CBrush(m_current->m_maskColor[MaskText::MaskType_LineBack].c));
		dc.SetBkMode(TRANSPARENT);
		if(m_current&&m_current->m_hWnd){
			CRect rcEdit;
			m_current->GetWindowRect(&rcEdit);
			ScreenToClient(&rcEdit);
			CPoint caret = m_current->GetCaretPos();
			long b,e;
			m_current->GetSel(b,e);
			int selline = m_current->LineFromChar(b);
			for(int i=begin;i<count;i++)
			{
				if(i == selline)
					dc.SetTextColor(RGB(255,50,50));
				else
					dc.SetTextColor(m_current->m_maskColor[MaskText::MaskType_LineText].c);
				CPoint ptbegin = m_current->PosFromChar(m_current->LineIndex(i));
				CPoint ptend(0, rc.bottom);
				if(i<(count-1))
					ptend = m_current->PosFromChar(m_current->LineIndex(i+1));
				else if(i>0)
					ptend.y = ptbegin.y-m_current->PosFromChar(m_current->LineIndex(i-1)).y+ptbegin.y;
				CRect rcLine(0, ptbegin.y+rcEdit.top, 0+LINE_WIDTH-3, ptend.y+rcEdit.top);
				if(rcLine.top>=rc.bottom)
					break;
				wchar_t buf[128];
				swprintf(buf, L"%d", i+1);
				if(i == selline)
					dc.FillRect(&rcLine, &CBrush(RGB(100,100,100)));
				dc.DrawText(buf, wcslen(buf), &rcLine, DT_RIGHT|DT_SINGLELINE|DT_VCENTER);
			}
		}
		if(oldFont)
			dc.SelectObject(oldFont);
	}
	paintdc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, 0, 0, SRCCOPY);
	m_image.ReleaseDC();
}

VOID CMyLuaEditor::OnViewSize(CRect* rc)
{
	GetMainFrame()->ScreenToClient(rc);
	MoveWindow(rc);
}

BOOL CMyLuaEditor::OnEraseBkgnd(CDC* pDC)
{
	return true;//return CDialogEx::OnEraseBkgnd(pDC);
}
void CMyLuaEditor::OnTimer(UINT_PTR nIDEvent)
{
	if(this->IsWindowVisible()&&m_current){
		int firstLine = m_current->GetFirstVisibleLine();
		if(m_current->m_firstLine != firstLine){
			this->RedrawLines();
			m_current->m_firstLine = firstLine;
		}
	}
	if(m_bIsFirst&&m_current){
		m_bIsFirst = false;
	}
	CDialogEx::OnTimer(nIDEvent);
}


BOOL CMyLuaEditor::PreTranslateMessage(MSG* pMsg)
{
	if(m_tooltip.m_hWnd)
		m_tooltip.RelayEvent(pMsg);
	BOOL bRet = CDialogEx::PreTranslateMessage(pMsg);
	if(pMsg->message==WM_MOUSEWHEEL)
		RedrawLines();
	return bRet;
}


LRESULT CMyLuaEditor::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

	return __super::WindowProc(message, wParam, lParam);
}


void CMyLuaEditor::OnSetFocus(CWnd* pOldWnd)
{
	if(this->m_current)
		m_current->SetFocus();
	else
		__super::OnSetFocus(pOldWnd);
}
