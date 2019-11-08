
// BaiWanGame.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "MFCGame.h"
#include "MFCGameDlg.h"
#include "MyInclude.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBaiWanGameApp

BEGIN_MESSAGE_MAP(CBaiWanGameApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBaiWanGameApp 构造

CBaiWanGameApp::CBaiWanGameApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CBaiWanGameApp 对象

CBaiWanGameApp theApp;


// CBaiWanGameApp 初始化

BOOL CBaiWanGameApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2), &wsa);
	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	//SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CoInitialize(0);
	SetCharLocale("chs", 0);
	//
	LPSTR szCmd = ::GetCommandLineA();
	if(szCmd && szCmd[0])
	{
		char szExePath[1024];
		int len = ::WideCharToMultiByte(CP_ACP, 0, _wpgmptr, -1, szExePath, 1024, "", 0);
		szExePath[len] = 0;

		char dir[256], file[256];
		_splitpath(szExePath, dir, file, 0, 0);
		strcat(dir, file);
		char szIni[256];
		strcpy(szIni, dir);
		strcat(szIni, "Phantom.ini");
		SetCurrentDirectoryA(dir);
		char path[256];
		strcpy(path, szCmd);
		if(path[strlen(path)-1] =='\\')
			path[strlen(path)-1] = 0;
		_splitpath(path, 0, dir, file, 0);
		char temp[256];
		strcpy(temp, file);
		strcat(temp, "\\bin");
		if(::GetPrivateProfileStringA(file, "path", "", path, 256, szIni))
		{
			::WritePrivateProfileStringA("active", "name", file, szIni);
		}
		else
		{
			WIN32_FIND_DATAA w32;
			HANDLE h = ::FindFirstFileA(temp, &w32);
			if(h == INVALID_HANDLE_VALUE)
			{
			}
			else
			{
				::WritePrivateProfileStringA(file, "path", temp, szIni);
				::WritePrivateProfileStringA(file, "game_width", "960", szIni);
				::WritePrivateProfileStringA(file, "game_height", "640", szIni);
				::WritePrivateProfileStringA(file, "window_width", "960", szIni);
				::WritePrivateProfileStringA(file, "window_height", "640", szIni);
				::WritePrivateProfileStringA("active", "name", file, szIni);
				FindClose(h);
			}
		}
	}
	CBaiWanGameDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	OnAppDestroy();
	WSACleanup();
	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

