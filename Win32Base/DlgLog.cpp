/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// DlgLog.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgLog.h"
#include "afxdialogex.h"
#ifdef EDITOR_OS
#include "MainFrm.h"
#include "DlgSelectAppID.h"
#else
#include "PhantomManager.h"
#endif
#
// CDlgLog 对话框
LogChannel::LogChannel()
{
	count		=	0;
	addcount	=	0;
}
LogChannel::~LogChannel()
{
	count		=	0;
	addcount	=	0;
	for(int i=0;i<logs.size();i++)
	{
		delete logs[i];
	}
}

IMPLEMENT_DYNAMIC(CDlgLog, CDialogEx)

CDlgLog::CDlgLog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLog::IDD, pParent)
{
}

CDlgLog::~CDlgLog()
{
}

void CDlgLog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//	DDX_Control(pDX, IDC_LOGTab, m_logTbl);
	DDX_Control(pDX, IDC_LOG_INFO, m_edLogInfo);
	DDX_Control(pDX, IDC_CLEAR_LOGs, m_btClear);
	DDX_Control(pDX, IDC_RESET_ALL, m_btReset);
	DDX_Control(pDX, IDC_RESET_CLEAR, m_btClearData);
	DDX_Control(pDX, IDC_SYSTEM_MSG, m_chkSystemLog);
#ifdef EDITOR_OS
	DDX_Control(pDX, IDC_SET_APPID, m_btAppID);
#endif
}


BEGIN_MESSAGE_MAP(CDlgLog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgLog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLog::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CLEAR_LOGs, &CDlgLog::OnBnClickedClearLogs)
	ON_BN_CLICKED(IDC_RESET_ALL, &CDlgLog::OnBnClickedResetAll)
	ON_BN_CLICKED(IDC_RESET_CLEAR, &CDlgLog::OnBnClickedResetClear)
	ON_LBN_SELCHANGE(IDC_LB_INFO, &CDlgLog::OnLbnSelchangeLbInfo)
	ON_NOTIFY(TCN_SELCHANGE, IDC_LOGTab, &CDlgLog::OnTcnSelchangeLogtab)
#ifdef EDITOR_OS
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SET_APPID, &CDlgLog::OnBnClickedSetAppid)
#endif
	ON_WM_MOVING()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_SYSTEM_MSG, &CDlgLog::OnBnClickedSystemMsg)
	ON_WM_MOUSEHWHEEL()
END_MESSAGE_MAP()


// CDlgLog 消息处理程序


void CDlgLog::OnBnClickedOk()
{
}

void CDlgLog::OnBnClickedCancel()
{
#ifdef EDITOR_OS
	this->ShowWindow(SW_HIDE);
	//OnOK();
	//KillTimer(1);
	//GetGameService()->StopServer();
#endif
}
extern BOOL g_bIsInit;
CRect LoadRect(const char* szKey, CRect& rcDefault)
{
	return CRect(GetProfileIntA(szKey, "left", rcDefault.left),
		GetProfileIntA(szKey, "top", rcDefault.top),
		GetProfileIntA(szKey, "right", rcDefault.right),
		GetProfileIntA(szKey, "bottom", rcDefault.bottom));

}
void SaveRect(CDialog* dlg, const char* szKey){
	CRect rc;
	dlg->GetWindowRect(&rc);
	char buf[128];
	sprintf(buf, "%d", rc.left);
	::WriteProfileStringA(szKey, "left", buf);
	sprintf(buf, "%d", rc.top);
	::WriteProfileStringA(szKey, "top", buf);
	sprintf(buf, "%d", rc.right);
	::WriteProfileStringA(szKey, "right", buf);
	sprintf(buf, "%d", rc.bottom);
	::WriteProfileStringA(szKey, "bottom", buf);
}
VOID CDlgLog::OnResize()
{
	if(m_logInfo.m_hWnd != 0)
	{
		CRect rc;
		GetClientRect(&rc);
		int cx = rc.Width();
		int cy = rc.Height();
		//m_logTbl.MoveWindow(0, 0, cx - 200, 20);
		m_btClear.MoveWindow(20, 0, 80, 20);
		m_btReset.MoveWindow(110, 0, 80, 20);
		m_btClearData.MoveWindow(200, 0, 80, 20);
		m_chkSystemLog.MoveWindow(290, 0, 100, 20);
		m_logInfo.MoveWindow(0, 20, cx, cy-150-20);
		m_edLogInfo.MoveWindow(0, cy-150, cx, 150);
#ifdef EDITOR_OS
		m_btAppID.MoveWindow(400, 0, 80, 20);
#endif
		if(g_bIsInit)
			SaveRect(this, "logdlg");
		this->Invalidate();
	}
}
void CDlgLog::OnSize(UINT nType, int cx, int cy)
{
	OnResize();
	CDialogEx::OnSize(nType, cx, cy);
}

inline	BOOL	intext(const char* t, const char* find)
{
	int len = strlen(find);
	return (strnicmp(t, find, len) == 0);
}
VOID CDlgLog::AddLog(int channel, const char* szText)
{
	if(m_hWnd == 0)
		return;
	if(intext(szText, "success:"))
		return;
	int len = strlen(szText);
	wchar_t* wbuf = new wchar_t[len+50];
	len = ::MultiByteToWideChar(CP_UTF8, 0, szText, len, wbuf, len+50);
	wbuf[len] = 0;
	m_logInfo.AddLog(wbuf, this->m_chkSystemLog.GetCheck());
	delete[] wbuf;
}
VOID CDlgLog::AddLog(const char* szText)
{
	AddLog(0, szText);
}

#ifdef EDITOR_OS
extern	const char*	GetDeviceIp();
VOID CDlgLog::StartRuntime()
{
	OnBnClickedClearLogs();
	this->KillTimer(1);
	this->SetTimer(1, 100, 0);
	this->ShowWindow(SW_SHOW);
	GetGameService()->ResetAll();
}
extern wchar_t *	GetProjectName();
#endif
BOOL CDlgLog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rc = LoadRect("logdlg", CRect(0, 0, 200, 500));
	if(rc.left<0)rc.left=0;
	if(rc.top<0)rc.top=0;
	if(rc.left>1024)rc.left=1024;
	if(rc.top>800)rc.top=800;
	if(rc.right>(rc.left+1024))rc.right=rc.left+1024;
	if(rc.bottom>(rc.top+800))rc.bottom=rc.top+800;
	m_logInfo.Create(L"",WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_VSCROLL, CRect(0, 0, 500, 500), this, 0);
	m_logInfo.m_edit = &m_edLogInfo;
	m_logInfo.SetFont(GetFont());
	m_chkSystemLog.SetCheck(true);
	MoveWindow(&rc);
	OnResize();
	InitLog();
#ifdef EDITOR_OS
	GetGameService()->StartServer();
	char buf[256];
	sprintf(buf, "log:system->%s", Autf8(Language(L"wait", L"等待手机连接...")).buf);
	GetMainFrame()->m_dlgLog.AddLog(buf);
	HICON hIcon;
	hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);
	CString str = AfxGetApp()->GetProfileStringW(L"AppID", GetProjectName());
	wchar_t tTitle[512];
	swprintf(tTitle, L"%s(AppID:%s)", Language(L"运行日志输出"), (const wchar_t*)str);
	SetWindowText(tTitle);
#endif
	m_chkSystemLog.SetCheck(AfxGetApp()->GetProfileIntW(L"AppDA", L"SystemLog", 1));
#ifndef EDITOR_OS
	m_btClearData.ShowWindow(SW_HIDE);
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CDlgLog::InitLog()
{
	//LogChannel* c = new LogChannel();
	//m_channels.push_back(c);
	//strcpy(c->name, "log");
	//m_logTbl.InsertItem(m_logTbl.GetItemCount(), L"LOG 日志输出");
}
extern	VOID	CreateClient();
void CDlgLog::OnBnClickedClearLogs()
{
	m_logInfo.Clear();
}

#ifdef EDITOR_OS
void CDlgLog::OnTimer(UINT_PTR nIDEvent)
{
	static BOOL bTime = false;
	if(!bTime)
	{
		bTime = true;
		Phantom::NetRecvMessage::DoMessageQueue();
		GetGameService()->OnFrameMove();
		CDialogEx::OnTimer(nIDEvent);
		bTime = false;
	}
}

VOID CDlgLog::ResetServer()
{
	//GetGameService()->StartServer();
	//m_authclient.Close();
	//m_authclient.Connect(GetDeviceIp(), SVR_PORT, 0x12345678);
	//m_authclient.Send(MsgID_ResetServer, 0, 0);
	//Sleep(200);
	CString str = AfxGetApp()->GetProfileStringW(L"AppID", GetProjectName());
	if(str.GetLength()==0){
		OnBnClickedSetAppid();
	}
}
extern VOID ReportFiles();
extern	VOID	SetForceDelete(BOOL b);
void CDlgLog::OnBnClickedResetAll()
{
	OnBnClickedClearLogs();
	SetForceDelete(FALSE);
	ReportFiles();
}
void CDlgLog::OnBnClickedResetClear()
{
	if(MessageBox(Language(L"确定要全部重新更新？"), 0, MB_YESNO)==IDNO)
		return;
	OnBnClickedClearLogs();
	SetForceDelete(TRUE);
	ReportFiles();
}
#else
void CDlgLog::OnBnClickedResetClear()
{
}

extern VOID	ResetAll();
void CDlgLog::OnBnClickedResetAll()
{
	OnBnClickedClearLogs();
	InitLog();
	ResetAll();
}
#endif

void CDlgLog::OnLbnSelchangeLbInfo()
{
	//if(this->m_lbInfo.GetCurSel() < 0)return;
	//this->m_edLogInfo.SetWindowTextW(m_channels[m_logTbl.GetCurSel()]->logs[m_lbInfo.GetCurSel()]->c_str());
}


void CDlgLog::OnTcnSelchangeLogtab(NMHDR *pNMHDR, LRESULT *pResult)
{
	//*pResult = 0;
	//if(m_logTbl.GetCurSel() < 0)return;
	//this->m_lbInfo.ResetContent();
	//for(int i=0;i<m_channels[m_logTbl.GetCurSel()]->logs.size();i++)
	//{
	//	m_lbInfo.AddString(m_channels[m_logTbl.GetCurSel()]->logs[i]->c_str());
	//}
	//int nMin = 0, nMax = 0;
	//m_lbInfo.GetScrollRange(SB_VERT, &nMin, &nMax);
	//m_lbInfo.SetScrollPos(SB_VERT, nMax + 1);
	//m_lbInfo.SetCurSel(m_lbInfo.GetCount()-1);
	
}
/*
extern	VOID	CallOnTest();
void CDlgLog::OnBnClickedResetAll2()
{
	CallOnTest();
//	CreateClient();
}
*/

void CDlgLog::OnMoving(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnMoving(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}


void CDlgLog::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
	if(g_bIsInit)
		SaveRect(this, "logdlg");
}










IMPLEMENT_DYNAMIC(CMyLogout, CButton)

CMyLogout::CMyLogout()
{
	m_edit	=	0;
	m_select = 0;
}

CMyLogout::~CMyLogout()
{
}


BEGIN_MESSAGE_MAP(CMyLogout, CButton)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CMyLogout 消息处理程序
extern int GetMaxMemoryUsage(int);
#define	LINE_HEIGHT 15
BOOL	IsHead(const wchar_t* in, const wchar_t* t, const wchar_t** ret){
	int len = wcslen(t);
	if(wcsnicmp(in, t, len) == 0)
	{
		*ret = in;// + len;
		return true;
	}
	return false;
}
VOID CMyLogout::AddLog(const wchar_t* text, BOOL bSystem)
{
	LogData data;
	memset(&data, 0, sizeof(LogData));
	const wchar_t* inout = text;
	if(IsHead(text, L"system->:",&inout))
		data.type = LogData::Log_Highlight;
	else if(IsHead(text, L"system->error:",&inout) || IsHead(text, L"system->warning:",&inout) || IsHead(text, L"code->error:",&inout) || IsHead(text, L"error:",&inout) || IsHead(text, L"code error->",&inout) || IsHead(text, L"warning:",&inout) || IsHead(text, L"error->[call()",&inout))
		data.type = LogData::Log_Warning;
	else if(IsHead(text, L"error->",&inout))
		data.type = LogData::Log_Error;
	else if(IsHead(text, L"code->",&inout))
		data.type = LogData::Log_Code;
	else if(IsHead(text, L"system->",&inout))
		data.type = LogData::Log_System;
	if(!bSystem&&data.type==LogData::Log_System)
		return;
	wcsncpy(data.text, inout, 255);
	SYSTEMTIME time;
	GetLocalTime(&time);
	swprintf(data.time, L"%02d:%02d:%02d %.02f", time.wHour, time.wMinute, time.wSecond, (float)time.wMilliseconds / 1000);
	m_logs.push_back(data);
	CRect rc;
	GetClientRect(&rc);
	int nMax = rc.Height() / LINE_HEIGHT-1;
	if(nMax > m_logs.size())
		nMax = 0;
	else
		nMax = m_logs.size() - nMax;
	this->SetScrollRange(SB_VERT, 0, nMax, 0);
	if(::GetFocus()!=m_hWnd)
	{
		this->SetScrollPos(SB_VERT, nMax);
	}
	this->Invalidate();
}


void CMyLogout::OnPaint()
{
	CPaintDC pdc(this);
	CRect rc;
	GetClientRect(&rc);
	if(m_tempDC.IsNull())
		m_tempDC.Create(rc.Width(), rc.Height(), 24);
	CDC dc;
	dc.Attach(m_tempDC.GetDC());
	//
	CBrush br(RGB(15,15,15));
	dc.FillRect(&rc, &br);
	CFont* old = dc.SelectObject(this->GetFont());
	dc.SetBkMode(TRANSPARENT);
	{
		int curpos = GetScrollPos(SB_VERT);
		int index = 0;
		for(int i=curpos;i<m_logs.size();i++)
		{
			LogData& data = m_logs.at(i);
			data.rect.SetRect(0, (i-curpos)*LINE_HEIGHT, rc.Width(), (i-curpos+1)*LINE_HEIGHT);
			CRect temp = data.rect;
			CRect right = data.rect;
			dc.SetTextColor(RGB(88,88,88));
			right.left += 80;
			dc.DrawText(data.time, &temp, DT_LEFT|DT_SINGLELINE);
			switch(data.type)
			{
			case LogData::Log_System:
				dc.SetTextColor(RGB(100,100,100));
				break;
			case LogData::Log_Warning:
				dc.SetTextColor(RGB(155,155,55));
				break;
			case LogData::Log_Highlight:
				dc.SetTextColor(RGB(255,200,55));
				break;
			case LogData::Log_Error:
				dc.SetTextColor(RGB(200,25,25));
				break;
			case LogData::Log_Code:
				dc.SetTextColor(RGB(55,175,55));
				break;
			}
			if(i == m_select)
			{
				dc.DrawFocusRect(&data.rect);
			}
			dc.DrawText(data.text, &right, DT_LEFT|DT_SINGLELINE);
			if(data.rect.top > rc.bottom)
				break;
		}
	}
#ifndef EDITOR_OS
	CRect rcText = rc;
	rcText.left = rcText.right - 200;
	rcText.bottom = rcText.top + 200;
	CBrush br2(RGB(50,50,50));
	dc.FillRect(&rcText, &br2);
	dc.SetTextColor(RGB(255,255,0));
	rcText.bottom = rcText.top + 15;
	char buf[128];
	sprintf(buf, "current image memory: %.02fM", (float)GetMaxMemoryUsage(0) / (1024*1024));
	DrawTextA(dc.m_hDC, buf, -1, &rcText, DT_LEFT|DT_SINGLELINE);
	rcText.OffsetRect(0, 15);
	sprintf(buf, "max image memory: %.02fM", (float)GetMaxMemoryUsage(1) / (1024*1024));
	DrawTextA(dc.m_hDC, buf, -1, &rcText, DT_LEFT|DT_SINGLELINE);
	//Scene3D* s = GetCurrentScenePtr();
	//if(s){
	//	rcText.OffsetRect(0, 15);
	//	sprintf(buf, "max scene objects: %d", s->GetObjectCount());
	//	DrawTextA(dc.m_hDC, buf, -1, &rcText, DT_LEFT|DT_SINGLELINE);
	//	rcText.OffsetRect(0, 15);
	//	sprintf(buf, "max scene monsters: %d", s->m_monsters.size());
	//	DrawTextA(dc.m_hDC, buf, -1, &rcText, DT_LEFT|DT_SINGLELINE);
	//	rcText.OffsetRect(0, 15);
	//	sprintf(buf, "max scene npcs: %d", s->m_npcs.size());
	//	DrawTextA(dc.m_hDC, buf, -1, &rcText, DT_LEFT|DT_SINGLELINE);
	//	rcText.OffsetRect(0, 15);
	//	sprintf(buf, "max scene skills: %d", s->m_skills.size());
	//	DrawTextA(dc.m_hDC, buf, -1, &rcText, DT_LEFT|DT_SINGLELINE);
	//}
#endif
	dc.SelectObject(old);
	//
	BitBlt(pdc.m_hDC, 0, 0, m_tempDC.GetWidth(), m_tempDC.GetHeight(), dc.m_hDC, 0, 0, SRCCOPY);
	dc.Detach();
	m_tempDC.ReleaseDC();
}
VOID CMyLogout::Clear()
{
	m_logs.clear();
	this->Invalidate();
	SetScrollRange(SB_VERT, 0, 0, 0);
}
void CMyLogout::DrawItem(LPDRAWITEMSTRUCT)
{
}

void CMyLogout::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);
	if(!m_tempDC.IsNull())
		m_tempDC.Destroy();

	// TODO: 在此处添加消息处理程序代码
}


BOOL CMyLogout::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


void CMyLogout::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int minpos;
	int maxpos;

	GetScrollRange(SB_VERT, &minpos, &maxpos); 

	// Get the current position of scroll box.
	int curpos = GetScrollPos(SB_VERT);
	CRect rc;
	GetClientRect(&rc);
	int nMax = rc.Height() / LINE_HEIGHT;
	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		curpos = minpos;
		break;

	case SB_RIGHT:      // Scroll to far right.
		curpos = maxpos;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if (curpos > minpos)
			curpos--;
		break;

	case SB_LINERIGHT:   // Scroll right.
		if (curpos < maxpos)
			curpos++;
		break;


	case SB_PAGEUP:
		if (curpos > minpos)
			curpos	-=	nMax;
		break;

	case SB_PAGEDOWN:
		if (curpos < maxpos)
			curpos	+=	nMax;
		break;

	case SB_THUMBPOSITION:
		curpos = nPos;
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		curpos = nPos;     // position that the scroll box has been dragged to.
		break;
	}
	// Set the new position of the thumb (scroll box).
	SetScrollPos(SB_VERT, curpos);
	this->Invalidate();
	CButton::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CMyLogout::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	::SetFocus(m_hWnd);
	{
		int curpos = GetScrollPos(SB_VERT);
		int index = 0;
		for(int i=curpos;i<m_logs.size();i++)
		{
			LogData& data = m_logs.at(i);
			if(data.rect.PtInRect(point))
			{
				if(m_edit)
					m_edit->SetWindowTextW(data.text);
				m_select = i;
				this->Invalidate();
				break;
			}
		}
	}
	CButton::OnLButtonDown(nFlags, point);
}


void CDlgLog::OnBnClickedSystemMsg()
{
	AfxGetApp()->WriteProfileInt(L"AppDA", L"SystemLog", m_chkSystemLog.GetCheck());
}


void CDlgLog::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int minpos;
	int maxpos;
	GetScrollRange(SB_VERT, &minpos, &maxpos); 
	int curpos = GetScrollPos(SB_VERT);
	if(zDelta>0){
		if (curpos > minpos)
			curpos--;
	}
	else{
		if (curpos < maxpos)
			curpos++;
	}
	SetScrollPos(SB_VERT, curpos);
	this->Invalidate();

	CDialogEx::OnMouseHWheel(nFlags, zDelta, pt);
}

void CDlgLog::OnBnClickedSetAppid()
{
}
