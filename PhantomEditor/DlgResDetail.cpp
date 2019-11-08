// DlgResDetail.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgResDetail.h"
#include "afxdialogex.h"


// CDlgResDetail 对话框

IMPLEMENT_DYNAMIC(CDlgResDetail, CDialogEx)

CDlgResDetail::CDlgResDetail(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgResDetail::IDD, pParent)
{

}

CDlgResDetail::~CDlgResDetail()
{
}

void CDlgResDetail::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_DESC, m_edDetail);
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgResDetail, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgResDetail::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgResDetail 消息处理程序


BOOL CDlgResDetail::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_edDetail.SetWindowTextW(m_strDetail);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgResDetail::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
