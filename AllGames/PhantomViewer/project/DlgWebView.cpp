// DlgWebView.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgWebView.h"
#include "afxdialogex.h"
extern "C"
{
	void SetBuyADOK(BOOL bFail)
	{
	}
};

// CDlgWebView 对话框

IMPLEMENT_DYNAMIC(CDlgWebView, CDialogEx)

CDlgWebView::CDlgWebView(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgWebView::IDD, pParent)
{

}

CDlgWebView::~CDlgWebView()
{
}

void CDlgWebView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_URL, m_cbUrl);
	DDX_Control(pDX, IDC_OPEN, m_btOpen);
	DDX_Control(pDX, IDC_BACK_FORWARD, m_spBackForward);
}


BEGIN_MESSAGE_MAP(CDlgWebView, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgWebView::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgWebView::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_OPEN, &CDlgWebView::OnBnClickedOpen)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgWebView 消息处理程序


void CDlgWebView::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CDlgWebView::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}

extern	INT			GetBrowserUrlCount();
extern	const char* GetBrowserUrl(int index);
extern	BOOL		AddBrowserUrl(const char* url);
BOOL CDlgWebView::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_web.Create(m_web.IDD, this);
	m_web.ShowWindow(SW_SHOW);
	// TODO:  在此添加额外的初始化
	int num = GetBrowserUrlCount();
	for(int i=0;i<num;i++)
	{
		m_cbUrl.AddString(GetBrowserUrl(i));
	}
	CRect rc;
	GetClientRect(&rc);
	m_web.MoveWindow(0, 20, rc.Width(), rc.Height()-20);
	m_web.ShowWindow(SW_HIDE);
	m_web.Navigate("http://www.51duli.com");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgWebView::OnBnClickedOpen()
{
	char buf[1024];
	CString url;
	this->m_cbUrl.GetWindowTextA(buf, 1024);
	if(strnicmp(buf, "http://", 7)!=0)
	{
		url = "http://";
		url += buf;
	}
	else
		url = buf;
	if(AddBrowserUrl(url))
		m_cbUrl.AddString(url);
	m_web.Navigate(url);
}


void CDlgWebView::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if(m_web.m_hWnd)
	{
		m_web.MoveWindow(0, 20, cx, cy-20);
	}
}
