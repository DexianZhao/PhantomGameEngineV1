// DlgLog.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgLog.h"
#include "afxdialogex.h"


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
	DDX_Control(pDX, IDC_LB_INFO, m_lbInfo);
	DDX_Control(pDX, IDC_LOGTab, m_logTbl);
	DDX_Control(pDX, IDC_LOG_INFO, m_edLogInfo);
}


BEGIN_MESSAGE_MAP(CDlgLog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgLog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLog::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CLEAR_LOGs, &CDlgLog::OnBnClickedClearLogs)
	ON_BN_CLICKED(IDC_RESET_ALL, &CDlgLog::OnBnClickedResetAll)
	ON_LBN_SELCHANGE(IDC_LB_INFO, &CDlgLog::OnLbnSelchangeLbInfo)
	ON_NOTIFY(TCN_SELCHANGE, IDC_LOGTab, &CDlgLog::OnTcnSelchangeLogtab)
	ON_BN_CLICKED(IDC_RESET_ALL2, &CDlgLog::OnBnClickedResetAll2)
	ON_BN_CLICKED(IDC_RESET_ALL3, &CDlgLog::OnBnClickedResetAll3)
END_MESSAGE_MAP()


// CDlgLog 消息处理程序


void CDlgLog::OnBnClickedOk()
{
}


void CDlgLog::OnBnClickedCancel()
{
}


void CDlgLog::OnSize(UINT nType, int cx, int cy)
{
	if(m_logTbl.m_hWnd != 0)
	{
		m_logTbl.MoveWindow(0, 0, cx - 300, 20);
		m_lbInfo.MoveWindow(0, 20, cx-300, cy-20);
		m_edLogInfo.MoveWindow(cx-300, 20, 300, cy-20);
	}
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}

inline	BOOL	intext(const char* t, const char* find)
{
	int len = strlen(find);
	return (strnicmp(t, find, len) == 0);
}
VOID CDlgLog::AddLog(const char* szText)
{
	if(m_hWnd == 0)
		return;
	if(intext(szText, "success:"))
		return;
	int channel = GetChannel(szText);
	if(channel < 0)
		return;
	std::string* str = new std::string();
	*str = (szText+strlen(m_channels[channel]->name)+1);
	m_channels[channel]->logs.push_back(str);
	if(this->m_logTbl.GetCurSel() == channel)
	{
		m_lbInfo.AddString(szText);
		int nMin=0,nMax=0;
		if(::GetFocus() != m_lbInfo.m_hWnd){
			m_lbInfo.GetScrollRange(SB_VERT, &nMin, &nMax);
			m_lbInfo.SetScrollPos(SB_VERT, nMax + 1);
			m_lbInfo.SetCurSel(m_lbInfo.GetCount()-1);
		}
	}
}

int		CDlgLog::GetChannel(const char* szLogFirst)
{
	char buf[128];
	int index = 0;
	bool bFind = false;
	for(int i=0;i<10;i++)
	{
		buf[i] = szLogFirst[i];
		if(szLogFirst[i] == ':')
		{
			bFind = true;
			buf[i] = 0;
			break;
		}
		else if(szLogFirst[i] == '\r' || szLogFirst[i] == '\n' || szLogFirst[i] == ' ')
			return -1;
	}
	if(!bFind)return -1;
	for(int i=0;i<this->m_channels.size();i++)
	{
		if(stricmp(m_channels[i]->name, buf) == 0)
		{
			return i;
		}
	}
	LogChannel* c = new LogChannel();
	m_channels.push_back(c);
	strcpy(c->name, buf);
	m_logTbl.InsertItem(m_logTbl.GetItemCount(), c->name);
	return ((int)m_channels.size()-1);
}

BOOL CDlgLog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
extern	VOID	CreateClient();
void CDlgLog::OnBnClickedClearLogs()
{
	for(int i=0;i<m_channels.size();i++)
	{
		for(int j=0;j<m_channels[i]->logs.size();j++)
		{
			delete m_channels[i]->logs[j];
		}
		m_channels[i]->logs.clear();
	}
	this->m_lbInfo.ResetContent();
}

extern VOID	ResetAll();
void CDlgLog::OnBnClickedResetAll()
{
	for(int i=0;i<m_channels.size();i++)
	{
		delete m_channels[i];
	}
	m_channels.clear();
	OnBnClickedClearLogs();
	this->m_logTbl.DeleteAllItems();
	ResetAll();
}


void CDlgLog::OnLbnSelchangeLbInfo()
{
	if(this->m_lbInfo.GetCurSel() < 0)return;
	this->m_edLogInfo.SetWindowTextA(m_channels[m_logTbl.GetCurSel()]->logs[m_lbInfo.GetCurSel()]->c_str());
}


void CDlgLog::OnTcnSelchangeLogtab(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	if(m_logTbl.GetCurSel() < 0)return;
	this->m_lbInfo.ResetContent();
	for(int i=0;i<m_channels[m_logTbl.GetCurSel()]->logs.size();i++)
	{
		m_lbInfo.AddString(m_channels[m_logTbl.GetCurSel()]->logs[i]->c_str());
	}
	int nMin = 0, nMax = 0;
	m_lbInfo.GetScrollRange(SB_VERT, &nMin, &nMax);
	m_lbInfo.SetScrollPos(SB_VERT, nMax + 1);
	m_lbInfo.SetCurSel(m_lbInfo.GetCount()-1);
	
}

extern	VOID	CallOnTest();
void CDlgLog::OnBnClickedResetAll2()
{
	CallOnTest();
//	CreateClient();
}

//extern	VOID	ActiveGameMode(BOOL b);
void CDlgLog::OnBnClickedResetAll3()
{
	//ActiveGameMode(false);
}
