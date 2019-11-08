// DlgBindBar.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgBindBar.h"
#include "afxdialogex.h"
#include "TlHelp32.h"


// CDlgBindBar 对话框

IMPLEMENT_DYNAMIC(CDlgBindBar, CDialogEx)

CDlgBindBar::CDlgBindBar(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgBindBar::IDD, pParent)
{

}

CDlgBindBar::~CDlgBindBar()
{
}

void CDlgBindBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgBindBar, CDialogEx)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CDlgBindBar::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgBindBar::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgBindBar 消息处理程序


void CDlgBindBar::OnDestroy()
{
	if(m_hWndBind)
	{
		::SetParent(m_hWndBind, 0);
		LONG style = ::GetWindowLongA(m_hWndBind, GWL_STYLE);
		style |= (WS_SIZEBOX | WS_CAPTION | WS_SYSMENU);
		::SetWindowLongA(m_hWndBind, GWL_STYLE, style);
		m_hWndBind = 0;
	}
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}
HWND	g_hWndNotepad = 0;

//BOOL WINAPI MyEnumProc(HWND hWnd, LPARAM)
//{
//	char buf[256];
//	GetWindowText(hWnd, buf, 256);
//	strlwr(buf);
//	if(strstr(buf, "notepad"))
//	{
//		g_hWndNotepad = hWnd;
//		ShowWindow(g_hWndNotepad, SW_SHOW);
//		return false;
//	}
//	return true;
//}

BOOL CDlgBindBar::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_hWndBind = 0;
	//::EnumChildWindows(0, MyEnumProc, 0);
	m_hWndBind = g_hWndNotepad;
	if(m_hWndBind)
	{
		//::SetParent(m_hWndBind, m_hWnd);
		CRect rc;
		GetClientRect(&rc);
		ClientToScreen(&rc);
		//::MoveWindow(m_hWndBind, rc.left, rc.top, rc.Width(), rc.Height(), true);
		//LONG style = ::GetWindowLongA(m_hWndBind, GWL_STYLE);
		//style ^= (WS_CAPTION | WS_SYSMENU);
		//::SetWindowLongA(m_hWndBind, GWL_STYLE, style);
	}
	else
		this->ShowWindow(SW_HIDE);
	return TRUE;
}


void CDlgBindBar::OnBnClickedOk()
{
}


void CDlgBindBar::OnBnClickedCancel()
{
}



void CDlgBindBar::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if(m_hWndBind)
	{
		CRect rc;
		GetClientRect(&rc);
		ClientToScreen(&rc);
		//::MoveWindow(m_hWndBind, rc.left, rc.top, rc.Width(), rc.Height(), true);
		static BOOL bFirst = false;
		if(bFirst){
		char buf[128];
		sprintf(buf, "%d", rc.Height());
		::WriteProfileStringA("DWZB", "bindHeight", buf);
		}
		bFirst = true;
	}
}


void CDlgBindBar::OnPaint()
{
	CPaintDC dc(this);
}


BOOL CDlgBindBar::OnEraseBkgnd(CDC* pDC)
{
	return false;
}
