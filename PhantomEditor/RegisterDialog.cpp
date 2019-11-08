// RegisterDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "RegisterDialog.h"
#include "afxdialogex.h"
#include "GCMFCHelper.h"
#include "IDEHttp.h"
#include "GCLanguage.h"

using namespace PhantomV1;

// CRegisterDialog 对话框

CRegisterDialog*	g_registerDlg = 0;

IMPLEMENT_DYNAMIC(CRegisterDialog, CDialogBase)

CRegisterDialog::CRegisterDialog(CWnd* pParent /*=NULL*/)
	: CDialogBase(CRegisterDialog::IDD, pParent)
{
	g_registerDlg	=	this;
}

CRegisterDialog::~CRegisterDialog()
{
}

void CRegisterDialog::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_EDIT1, m_edEmail);
	DDX_Control(pDX, IDC_EDIT3, m_edPassword);
	DDX_Control(pDX, IDC_EDIT4, m_edRepassword);
	//m_edEmail,m_edPassword,m_edRepassword,m_edNickname
	CDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegisterDialog, CDialogBase)
	ON_BN_CLICKED(IDOK, &CRegisterDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRegisterDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// CRegisterDialog 消息处理程序
extern VOID		ReloadLogin();
void WINAPI OnRegisterCallback(Phantom::Name& file, IDCJsonObject& obj, const char* errorText, void* param)//返回函数
{
	if(errorText){
		g_registerDlg->MessageBox(W(errorText), Language(L"产生错误"), MB_OK|MB_ICONERROR);
		return;
	}
	int result = obj["result"];
	if(result == 1){
		g_registerDlg->MessageBox(Language(L"激活链接已经发送到你的邮箱\n成功激活邮件之后您就可以正式成为\"幻影开发者\"用户\n当你成为\"幻影开发者\"之后可以免费使用幻影开发游戏了！"), Language(L"恭喜你成功注册"), MB_OK);
		CString strEmail;
		g_registerDlg->m_edEmail.GetWindowText(strEmail);
		WriteRegValue("email", A(g_registerDlg->m_strEmail).buf);
		WriteRegValue("password", A(g_registerDlg->m_strPassword).buf);
		g_registerDlg->OK();
		ReloadLogin();
	}
	else
	{
		MessageResult(g_registerDlg->m_hWnd, result);
	}
}

void CRegisterDialog::OnBnClickedOk()
{
	CString strEmail,strPassword,strRepassword;
	m_edEmail.GetWindowText(strEmail);
	m_edPassword.GetWindowText(strPassword);
	m_edRepassword.GetWindowText(strRepassword);
	BOOL bAscii = 1;
	for(int i=0;i<strEmail.GetLength();i++)
	{
		wchar_t t = strEmail[i];
		if(!((t>='0'&&t<='9')||(t>='a'&&t<='z')||(t>='A'&&t<='Z')||t=='@'||t=='.'))
		{
			MessageBox(Language(L"邮箱地址错误，只能使用英文和数字"));
			return;
		}
	}
	if(strEmail.GetLength()<5 || wcsstr(strEmail, L"@") == 0 || wcsstr(strEmail, L".") == 0){
		MessageBox(Language(L"邮箱地址错误"));
		return;
	}
	if(wcsicmp(strPassword,strRepassword) != 0)
	{
		MessageBox(Language(L"密码不一致"));
		return;
	}
	if(strPassword.GetLength()<6)
	{
		MessageBox(Language(L"密码最少6个字"));
		return;
	}
	IDCJsonObject obj;
	obj["email"] = A(strEmail).buf;
	std::string m5;
	md5(A(strPassword).buf, m5);
	obj["password"] = m5.c_str();
	m_strEmail = strEmail;
	m_strPassword = m5.c_str();
	Request("pvd_register", obj, OnRegisterCallback, false);
	//CDialogBase::OnOK();
}


void CRegisterDialog::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogBase::OnCancel();
}
