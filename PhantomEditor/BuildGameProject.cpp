// ReportDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "BuildGameProject.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CBuildGameProject 对话框

IMPLEMENT_DYNAMIC(CBuildGameProject, CDialogEx)

CBuildGameProject::CBuildGameProject(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBuildGameProject::IDD, pParent)
{
	m_projPtr	=	0;
}

CBuildGameProject::~CBuildGameProject()
{
}

void CBuildGameProject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESOURCE_CODE, m_edAuthCode);
	DDX_Control(pDX, IDC_REPORT_NAME, m_edUserName);
	DDX_Control(pDX, IDC_REPORT_URL, m_edUserURL);
	DDX_Control(pDX, IDC_RESOURCE_DESC, m_edResourceDesc);
	DDX_Control(pDX, IDC_AUTH_ENABLE, m_chkAuthEnable);
	DDX_Control(pDX, IDC_FILE_LIST, m_lbFileList);
	DDX_Control(pDX, IDC_PROGRESS_EXPORT, m_progExport);
	DDX_Control(pDX, IDC_RESOURCE_C, m_edResourceCode);
	DDX_Control(pDX, IDC_GAME_NAME, m_edGameName);
	DDX_Control(pDX, IDC_GAME_ICONs, m_btGameIcon);
	DDX_Control(pDX, IDC_COMPRESS2, m_chkCompress);
}


BEGIN_MESSAGE_MAP(CBuildGameProject, CDialogEx)
	ON_BN_CLICKED(IDOK, &CBuildGameProject::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RESET_GUID, &CBuildGameProject::OnBnClickedResetGuid)
	ON_BN_CLICKED(IDOK2, &CBuildGameProject::OnBnClickedOk2)
	ON_BN_CLICKED(IDC_AUTH_ENABLE, &CBuildGameProject::OnBnClickedAuthEnable)
	ON_BN_CLICKED(IDC_SET_ICON, &CBuildGameProject::OnBnClickedSetIcon)
	ON_BN_CLICKED(IDCANCEL2, &CBuildGameProject::OnBnClickedCancel2)
END_MESSAGE_MAP()


// CBuildGameProject 消息处理程序


int		GetSmallTextureFormat(int fmt)
{
	switch(fmt)
	{
	case Enum::FMT_A8R8G8B8:
		return Enum::FMT_A4R4G4B4;
	case Enum::FMT_R8G8B8:
		return Enum::FMT_R5G6B5;
	case Enum::FMT_X8R8G8B8:
		return Enum::FMT_X4R4G4B4;
	}
	return fmt;
}

BufferPointer*	ConvertSmall(const char* szTexture, std::string& sm)
{
	char szDrive[256], szDir[256], szFile[256], szExt[256];
	_splitpath(szTexture, szDrive, szDir, szFile, szExt);
	if(
		stricmp(szExt, ".png") == 0
		||stricmp(szExt, ".bmp") == 0
		||stricmp(szExt, ".dds") == 0
		||stricmp(szExt, ".jpg") == 0
		||stricmp(szExt, ".jpeg") == 0
		)
	{
	}
	else
		return 0;
	strcat(szDrive, szDir);
	strcat(szDrive, szFile);
	strcat(szDrive, "_s_f_img");
	strcat(szDrive, szExt);
	ITexture* tex	=	GetRenderDevicePtr()->CreateEmptyTexture(0);
	if(!tex->LoadTexture(0, szTexture))
	{
		safe_release(tex);
		return 0;
	}
	ITexture* smallTex	=	GetRenderDevicePtr()->CreateEmptyTexture(0);
	int smallWidth	=	tex->GetWidth() / SMALL_DIV_VALUE;
	int smallHeight	=	tex->GetHeight() / SMALL_DIV_VALUE;
	if(smallWidth < 16 || smallWidth < 16)
		return 0;
	if(!smallTex->Create2D(smallWidth, smallHeight, Enum::FMT_DXT2, Enum::USAGE_DYNAMIC, Enum::POOL_DEFAULT, 1))
	{
		safe_release(tex);
		safe_release(smallTex);
		return 0;
	}
	//缩小图片之后，在载入的时候一定要放大，然后载入原来大小的高清图片的时候一定要重新自动载入并且释放小图
	if(!tex->bitblt(smallTex, PRect(0, 0, smallWidth, smallHeight), PRect(0, 0, tex->GetWidth(), tex->GetHeight())))
	{
		safe_release(tex);
		safe_release(smallTex);
		return 0;
	}
	BufferPointer* buffer	=	smallTex->SaveTextureTo();
	//
	//smallTex->SaveTextureTo(szDrive);
	//
	//destroyFileData(buffer);
	safe_release(tex);
	safe_release(smallTex);
	sm	=	szDrive;
	return buffer;
}

std::vector<BufferPointer*>	g_smallFiles;

extern	IFilePackage*				g_filePackage;
extern	IPackageFile*				g_fileMainPtr;
extern	AuthGUID					g_fileAuthGUID;
extern	BOOL						g_bFileAtRootDir;
extern	CString						g_filePackageSave;
extern	PackageInfo					g_fileExportInfo;
extern	BOOL	GetAuthGUID(AuthGUID* guidRet);
DWORD	WINAPI	ThreadSaveGameProgress(LPVOID lpVoid)
{
	CBuildGameProject* dlg	=	(CBuildGameProject*)lpVoid;
	//SavePackage(const char* szFile, IPackageFile* pMainFile, BOOL bAtRootDir, const PackageInfo& info, const GUID* pAuthGUID = 0, ISystemProgress
	GetAuthGUID(&g_fileAuthGUID);
	g_filePackage->SavePackage(A(g_filePackageSave),  g_fileMainPtr, false, g_fileExportInfo, &g_fileAuthGUID, dlg);
	safe_release(g_filePackage);
	wchar_t buf[256];
	swprintf(buf, L"%s\n%s", Language(L"生成成功游戏: 文件保存到"), (const wchar_t*)g_filePackageSave);
	dlg->MessageBox(buf, Language(L"生成成功游戏"), MB_OK);
	g_smallFiles.clear();
	return 0;
}

const wchar_t*			GetFileExt(const wchar_t* szFile);

void CBuildGameProject::OnBnClickedOk()
{
	wchar_t szPath[256], szDir[256], szFile[256], szExt[32];
	_wsplitpath(m_strFileExport, szPath, szDir, szFile, szExt);
	wcscat(szPath, szDir);
	wcscat(szPath, szFile);
	wcscat(szPath, L".rpg");
	m_strBuild			=	szPath;
	////
	g_filePackageSave	=	A(m_strBuild).buf;
	g_filePackage		=	createFilePackage();
	g_fileMainPtr		=	0;
	memset(&g_fileExportInfo, 0, sizeof(g_fileExportInfo));
	for(int i=0;i<g_smallFiles.size();i++)
		destroyFileData(g_smallFiles[i]);
	g_smallFiles.clear();
	//
	CString strUserName;
	CString strUrl;
	CString strDesc;
	CString strGUID;
	CString strResGUID;
	this->m_edUserName.GetWindowTextW(strUserName);
	this->m_edUserURL.GetWindowTextW(strUrl);
	this->m_edResourceDesc.GetWindowTextW(strDesc);
	this->m_edAuthCode.GetWindowTextW(strGUID);
	this->m_edResourceCode.GetWindowTextW(strResGUID);
	//
	{
		char buf[256];
		strcpy(buf, A(strGUID).buf);
		std::vector<unsigned char> ret;
		HexEncode(ret, buf, 0);
		memcpy(&g_fileAuthGUID, &ret.at(0), sizeof(GUID));
	}
	{
		char buf[256];
		strcpy(buf, A(strResGUID).buf);
		std::vector<unsigned char> ret;
		HexEncode(ret, buf, 0);
		//memcpy(&g_fileExportInfo.header.resourceCode, &ret.at(0), sizeof(GUID));
	}
	//
	g_fileExportInfo.header.name = A(strUserName);
	g_fileExportInfo.header.url = A(strUrl);
	g_fileExportInfo.header.description = A(strDesc);
	if(this->m_chkAuthEnable.GetCheck())
		g_fileExportInfo.header.flags	|=	g_fileExportInfo.header.Package_RequestAuth;
	if(!m_chkCompress.GetCheck())
		g_fileExportInfo.header.flags	|=	g_fileExportInfo.header.Package_NoCompress;
	//
	SetProjectPath();
	//
	if(m_strGameIcon.GetLength() > 0)
		g_filePackage->insert(A(m_strGameIcon));
	for(int i=0;i<m_lbFileList.GetCount();i++)
	{
		CString file;
		m_lbFileList.GetText(i, file);
		A a(file);

		std::string sm;
		BufferPointer* buffer	=	ConvertSmall(a, sm);
		IPackageFile* filePtr	=	g_filePackage->insert(a);
		if(buffer)
		{
			IPackageFile* pFile		=	g_filePackage->insert(sm.c_str());
			g_smallFiles.push_back(buffer);
			if(pFile)
				pFile->SetFileBuffer(buffer);
		}
		if(wcsicmp(this->m_strFileExport, file) == 0)
			g_fileMainPtr	=	filePtr;
	}
	if(g_fileMainPtr == NULL)
	{
		MessageBox(Language(L"请设置主文件"), 0, MB_OK);
		return;
	}
	HANDLE hThread	=	CreateThread(0, 0, ThreadSaveGameProgress, (LPVOID)this, 0, 0);
	CloseHandle(hThread);
	//
	CSqliteTableExe* table	=	GetReportTable();
	if(table)
	{
		CSqliteLine w(table);
		w.SetText("file", A(m_strFileExport));
		w.SetText("url", A(strUrl));
		w.SetText("description", A(strDesc));
		w.SetText("guid", A(strResGUID));
		w.SetText("auth_code", A(strGUID));
		w.SetText("auth_enable", this->m_chkAuthEnable.GetCheck() ? "1" : "0");
		w.SetText("game_icon", A(m_strGameIcon).buf);
		w.UpdateToDB(true, "file");
	}
}

VOID	CBuildGameProject::OnBegin(INT nMax)	//开始进度
{
	m_nCurrentProgress	=	0;
	m_nMaxProgress		=	nMax;
	if(m_nMaxProgress <= 0)
		return;
	m_progExport.SetPos(0);
	m_progExport.ShowWindow(SW_SHOW);
	m_progExport.SetRange(0, 10000);
}

VOID	CBuildGameProject::OnProgress()//进度进行中+1
{
	if(m_nMaxProgress <= 0)
		return;
	m_nCurrentProgress++;
	m_progExport.SetPos((int)((float)m_nCurrentProgress / (float) m_nMaxProgress * 10000.0f));
}

VOID	CBuildGameProject::OnEnd()//结束进度
{
	m_progExport.ShowWindow(SW_HIDE);
}



BOOL CBuildGameProject::OnInitDialog()
{
	__super::OnInitDialog();
	if(MessageBox(Language(L"必须保存项目文件才能发布游戏，是否保存？"), Language(L"保存提示"), MB_YESNO) != IDYES)
		return false;
	CDynamicArray<ZipFileName> files;
	CDynamicArray<CPlotPropertiesMap*> enums;
	m_projPtr->m_gameDirector.EnumAllPropertiesMap(enums);
	for(int i=0;i<enums.size();i++)
	{
		enums[i]->EnumIncludeFiles(files);
	}
	for(int i=0;i<files.size();i++)
	{
		this->m_lbFileList.AddString(W(files[i].t));
	}
	m_projPtr->SetModifyed(true);
	m_projPtr->SaveFile(FALSE);
	this->m_edUserName.SetLimitText(32);
	this->m_edUserURL.SetLimitText(32);
	this->m_edResourceDesc.SetLimitText(128);

	wchar_t buf[512];
	::GetProfileStringW(L"report", L"user", L"幻影用户", buf, 512);
	this->m_edUserName.SetWindowTextW(buf);
	//
	CSqliteTableExe* table	=	GetReportTable();
	std::string resourceCode;
	std::string authCode;
	std::string	gameIcon;
	if(table)
	{
		CSqliteLine r(table);
		r.SetText("file", A(m_strFileExport).buf);
		if(r.LoadFromDB("select * from report where file = '%s';", A(m_strFileExport).buf))
		{
			std::string temp;
			if(r.GetText(temp, "url"))
				m_edUserURL.SetWindowTextW(W(temp.c_str()));
			if(r.GetText(temp, "description"))
				this->m_edResourceDesc.SetWindowTextW(W(temp.c_str()));
			if(r.GetText(temp, "auth_enable"))
				this->m_chkAuthEnable.SetCheck(atoi(temp.c_str()));
			if(r.GetText(temp, "game_icon"))
				gameIcon	=	temp.c_str();
			r.GetText(resourceCode, "guid");
			r.GetText(authCode, "auth_code");
		}
		else
		{
			this->m_edUserURL.SetWindowTextW(L"http://www.aixspace.com");
			this->m_edResourceDesc.SetWindowTextW(L"说明");
			this->m_chkAuthEnable.SetCheck(0);
			//
			gameIcon		=	"";
			{
				GUID guid;
				CoCreateGuid(&guid);
				std::vector<unsigned char> hex;
				unsigned char* hexptr = (unsigned char*)&guid;
				for(int i=0;i<sizeof(GUID);i++)
					hex.push_back(hexptr[i]);
				HexDecode(resourceCode, hex, 0);
			}
			{
				GUID guid;
				CoCreateGuid(&guid);
				std::vector<unsigned char> hex;
				unsigned char* hexptr = (unsigned char*)&guid;
				for(int i=0;i<sizeof(GUID);i++)
					hex.push_back(hexptr[i]);
				HexDecode(authCode, hex, 0);
			}
		}
	}
	//
	LoadGameIcon();
	this->m_edGameName.SetWindowTextW(m_strGameName);
	this->m_edResourceCode.SetWindowTextW(W(resourceCode.c_str()));
	this->m_edAuthCode.SetWindowTextW(W(authCode.c_str()));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBuildGameProject::OnBnClickedResetGuid()
{
}

VOID CBuildGameProject::LoadGameIcon()
{
	if(!this->m_imgGameIcon.IsNull())
		m_imgGameIcon.Destroy();
	if(m_strGameIcon.GetLength() <= 0)
	{
		LoadResImage(m_imgGameIcon, MAKEINTRESOURCE(IDR_GAME_ICON));
	}
	else
	{
		m_imgGameIcon.Load(m_strGameIcon);
	}
	m_btGameIcon.SetImage(m_imgGameIcon, 1, 1, 0);
	this->m_btGameIcon.Invalidate();
}

void CBuildGameProject::OnBnClickedOk2()
{
	if(this->m_strBuild.GetLength() > 0)
	{
		wchar_t wbuf[512];
		swprintf(wbuf, L"/select,%s", (const wchar_t*)m_strBuild);
		ShellExecute(NULL, _T("open"), _T("Explorer.exe"), wbuf, NULL, SW_SHOWDEFAULT);
	}
}


void CBuildGameProject::OnBnClickedAuthEnable()
{
	//if(this->m_chkAuthEnable.GetCheck())
	//	OnBnClickedResetGuid();
}


void CBuildGameProject::OnBnClickedSetIcon()
{
	CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"Game Icon(*.jpg)|*.jpg|*.*|*.*", this);
	if(dlg.DoModal() != IDOK)
		return;
	SetProjectPath();
	this->m_strGameIcon	=	dlg.GetPathName();
	this->LoadGameIcon();
}


void CBuildGameProject::OnBnClickedCancel2()
{
	std::vector<std::wstring> lists;
	for(int i=0;i<m_lbFileList.GetCount();i++)
	{
		if(m_lbFileList.GetSel(i))
		{
		}
		else
		{
			CString str;
			m_lbFileList.GetText(i, str);
			lists.push_back((const wchar_t*)str);
		}
	}
	m_lbFileList.ResetContent();
	for(int i=0;i<lists.size();i++)
	{
		m_lbFileList.AddString(lists[i].c_str());
	}
	m_lbFileList.SetCurSel(0);
}
