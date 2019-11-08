// LoginDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "LoginDialog.h"
#include "afxdialogex.h"
#include "RegisterDialog.h"
#include "GCLanguage.h"
#include "GCMFCHelper.h"
#include "IDEHttp.h"
#include "MainFrm.h"

// CLoginDialog 对话框
extern	BOOL	IsLoginOK();
extern	VOID	SetLoginOK(BOOL b);
extern	VOID	CheckRequestIP();
BOOL	CheckLogin()
{
	if(!IsLoginOK()){
		CheckRequestIP();
		//if(GetMainFrame()->MessageBoxW(Language(L"您当前还没有登录\n是否现在就去登陆？"), 0, MB_YESNO)==IDYES)
		//{
			CLoginDialog dlg;
			if(dlg.DoModal()!=IDOK)
				return true;
		//}
		//else
		//	return false;
	}
	return true;
}

CLoginDialog*	g_loginDlg = 0;
IMPLEMENT_DYNAMIC(CLoginDialog, CDialogBase)

CLoginDialog::CLoginDialog(CWnd* pParent /*=NULL*/)
	: CDialogBase(CLoginDialog::IDD, pParent)
{
	g_loginDlg = this;
	m_bResetPassword = false;
}

CLoginDialog::~CLoginDialog()
{
}


VOID		ReloadLogin()
{
	g_loginDlg->ReloadLogin();
	
}

VOID CLoginDialog::ReloadLogin()
{
	char email[128],password[128];
	ReadRegValue("email", email);
	ReadRegValue("password", password);
	m_edEmail.SetWindowTextW(W(email).buf);
	m_edPassword.SetWindowTextW(W(password).buf);
}

void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_USERNAME_s, m_edEmail);
	DDX_Control(pDX, IDC_PASSWORD_s, m_edPassword);
	DDX_Control(pDX, IDC_AUTO_LOGIN_s, m_chkAutoLogin);
	CDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoginDialog, CDialogBase)
	ON_BN_CLICKED(IDOK, &CLoginDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLoginDialog::OnBnClickedCancel)
	ON_BN_CLICKED(IDREGISTER, &CLoginDialog::OnBnClickedRegister)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT3, &CLoginDialog::OnEnChangeEdit3)
END_MESSAGE_MAP()


// CLoginDialog 消息处理程序
extern	VOID	SetIsVIP(BOOL b);
extern	VOID	SetMainTitle(const char* s);
extern	VOID	SetUserID(int id);
void WINAPI OnLoginCallback(Name& file, IDCJsonObject& obj, const char* errorText, void* param)//返回函数
{
	if(errorText){
		g_loginDlg->MessageBox(W(errorText), Language(L"产生错误"), MB_OK|MB_ICONERROR);
		g_loginDlg->GetDlgItem(IDREGISTER)->EnableWindow(TRUE);
		g_loginDlg->GetDlgItem(IDOK)->EnableWindow(TRUE);
		g_loginDlg->GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
		return;
	}
	int result = obj["result"];
	if(result == 1){
		//写入最后一次登录的账号密码
		int nVip = obj["vip"];
		SetIsVIP(nVip);
		SetHttpKey(obj["key"]);
		SetUserID(obj["uid"]);
		if(nVip)
			SetVipCode(obj["code"]);
		WriteRegValue("email", g_loginDlg->m_strEmail.c_str());
		WriteRegValue("password", g_loginDlg->m_strPassword.c_str());
		char buf[128];
		sprintf(buf, "%d", g_loginDlg->m_chkAutoLogin.GetCheck());
		WriteRegValue("autologin", buf);
		const char* title = obj["title"];
		if(title){
			SetMainTitle(title);
		}
		g_loginDlg->OK();
		SetLoginOK(true);
	}
	else
	{
		MessageResult(g_loginDlg->m_hWnd, result);
		g_loginDlg->GetDlgItem(IDREGISTER)->EnableWindow(TRUE);
		g_loginDlg->GetDlgItem(IDOK)->EnableWindow(TRUE);
		g_loginDlg->GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	}
}
void CLoginDialog::OnBnClickedOk()
{
	CString strEmail,strPassword;
	m_edEmail.GetWindowText(strEmail);
	m_edPassword.GetWindowText(strPassword);
	BOOL bAscii = 1;
	for(int i=0;i<strEmail.GetLength();i++)
	{
		wchar_t t = strEmail[i];
		if(!((t>='0'&&t<='9')||(t>='a'&&t<='z')||(t>='A'&&t<='Z')||t=='@'||t=='.'))
		{
			MessageBox(Language(L"邮箱地址错误，只能使用英文和数字"));
			GetDlgItem(IDREGISTER)->EnableWindow(true);
			GetDlgItem(IDOK)->EnableWindow(true);
			GetDlgItem(IDCANCEL)->EnableWindow(true);
			return;
		}
	}
	if(strEmail.GetLength()<5 || wcsstr(strEmail, L"@") == 0 || wcsstr(strEmail, L".") == 0){
		MessageBox(Language(L"邮箱地址错误"));
		GetDlgItem(IDREGISTER)->EnableWindow(true);
		GetDlgItem(IDOK)->EnableWindow(true);
		GetDlgItem(IDCANCEL)->EnableWindow(true);
		return;
	}
	if(strPassword.GetLength()<6)
	{
		MessageBox(Language(L"密码最少6个字"));
		GetDlgItem(IDREGISTER)->EnableWindow(true);
		GetDlgItem(IDOK)->EnableWindow(true);
		GetDlgItem(IDCANCEL)->EnableWindow(true);
		return;
	}
	IDCJsonObject obj;
	m_strEmail = A(strEmail).buf;
	obj["email"] = m_strEmail.c_str();
	char password[128];
	ReadRegValue("password", password);
	if(stricmp(password, A(strPassword).buf) != 0){//如果密码是重新输入的则
		std::string m5;
		md5(A(strPassword).buf, m5);
		m_strPassword = m5.c_str();
	}
	else
	{
		m_strPassword = A(strPassword).buf;
	}
	obj["password"] = m_strPassword.c_str();
	SetLoginOK(false);
	Request("pvd_login", obj, OnLoginCallback, false);
	//CDialogBase::OnOK();
}


void CLoginDialog::OnBnClickedCancel()
{
	OnCancel();
	//PostQuitMessage(0);
}


void CLoginDialog::OnBnClickedRegister()
{
	CRegisterDialog dlg(this);
	dlg.DoModal();
}


BOOL CLoginDialog::OnInitDialog()
{
	CDialogBase::OnInitDialog();

	char autologin[128];
	ReadRegValue("autologin", autologin);
	m_chkAutoLogin.SetCheck(atoi(autologin));
	ReloadLogin();
	HICON hIcon;
	hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);
	return TRUE;
}


void CLoginDialog::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1){
		static BOOL bLogin = false;
		if(!bLogin){
			bLogin = true;
			GetDlgItem(IDREGISTER)->EnableWindow(FALSE);
			GetDlgItem(IDOK)->EnableWindow(FALSE);
			//GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
			OnBnClickedOk();
		}
	}

	CDialogBase::OnTimer(nIDEvent);
}


void CLoginDialog::OnEnChangeEdit3()
{
	m_bResetPassword = true;
}
