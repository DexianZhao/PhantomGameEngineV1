// PngMakerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PngMakerDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "GCLanguage.h"
#include "GCViewManager.h"

// CFileFilterDlg 对话框

IMPLEMENT_DYNAMIC(CFileFilterDlg, CDialogEx)

CFileFilterDlg::CFileFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileFilterDlg::IDD, pParent)
{

}

CFileFilterDlg::~CFileFilterDlg()
{
}

void CFileFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_EXPORT_LIST, m_lbFilePath);
	DDX_Control(pDX, IDC_EXPORT_PATHs, m_edFilePath);
	DDX_Control(pDX, IDC_CREATE_DIR, m_chkDir);
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFileFilterDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CFileFilterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFileFilterDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_EXPORT_PATHs, &CFileFilterDlg::OnCbnSelchangeExportPaths)
	ON_BN_CLICKED(IDC_CREATE_DIR, &CFileFilterDlg::OnBnClickedCreateDir)
END_MESSAGE_MAP()


// CFileFilterDlg 消息处理程序

extern	VOID			ImportFile(CDynamicArray<ZipFileName>& rets, const char* szFile);

void CFileFilterDlg::OnBnClickedOk()
{
	CString str;
	this->m_edFilePath.GetWindowText(str);
	if(str.GetLength() == 0)
	{
		MessageBox(Language(L"写入输出目录"));
		return;
	}
	BOOL bFind = false;
	for(int i=0;i<m_edFilePath.GetCount();i++)
	{
		CString str2;
		m_edFilePath.GetLBText(i, str2);
		if(str2 == str)
		{
			m_edFilePath.SetItemData(i, m_chkDir.GetCheck());
			wchar_t buf[128];
			wchar_t val[128];
			_swprintf(buf, L"expd_%d", i);
			_swprintf(val, L"%d", m_chkDir.GetCheck());
			::WriteProfileStringW(L"Phantom", buf, val);
			bFind = true;
			break;
		}
	}
	if(!bFind)
	{
		m_edFilePath.SetItemData(m_edFilePath.AddString(str), m_chkDir.GetCheck());
		for(int i=0;i<m_edFilePath.GetCount();i++)
		{
			if(i >= 10)break;
			CString str2;
			m_edFilePath.GetLBText(i, str2);
			wchar_t buf[128];
			_swprintf(buf, L"exp_%d", i);
			::WriteProfileStringW(L"Phantom", buf, str2);
			_swprintf(buf, L"expd_%d", i);
			wchar_t val[128];
			_swprintf(val, L"%d", m_edFilePath.GetItemData(i));
			::WriteProfileStringW(L"Phantom", buf, val);
		}
		
	}
	if(!GetViewManager() || !GetViewManager()->GetActiveView())
		return;
	const wchar_t* szFileName = GetViewManager()->GetActiveView()->GetFileName();
	wchar_t szFile[256], szExt[128];
	wchar_t temp[128];
	_wsplitpath(szFileName, 0, 0, szFile, szExt);
	wcscpy(temp, szFile);
	wcscat(szFile, szExt);
	A a(szExt);
	strlwr(a.buf);
	if(m_extFiles.find(a.buf) == m_extFiles.end())
		return;
	str += L"/";
	char szPath[512];
	strcpy(szPath, A(str));
	//strcat(szPath, m_extFiles[a.buf].c_str());
	strcat(szPath, "/");
	CDynamicArray<ZipFileName> rets;
	ImportFile(rets, A(szFileName));
	SetProjectPath();
	BOOL bDir = m_chkDir.GetCheck();
	if(bDir)
	{
		strcat(szPath, A(temp));
		strcat(szPath, "/");
	}
	for(int i=0;i<rets.size();i++)
	{
		CString file;
		file = rets[i].t;
		if(fexist(0, A(file)))
		{
			_wsplitpath(file, 0, 0, szFile, szExt);
			wcscat(szFile, szExt);
			wchar_t dest[256];
			wcscpy(dest, W(szPath));
			wcscat(dest, szFile);
			createFolders(A(dest));
			CopyFileW(file, dest, false);
		}
	}
	MessageBox(Language(L"输出成功", L"输出成功!"), Language(L"提示!", L"提示!"));
}


void CFileFilterDlg::OnBnClickedCancel()
{
}


BOOL CFileFilterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_extFiles[".rpgmesh"] = "meshs";
	m_extFiles[".rpgeffect"] = "effects";
	m_extFiles[".rpgui"] = "uis";
	m_extFiles[".rpgunit"] = "3ds";
	m_extFiles[".rpgcur"] = "curs";
	m_lbFilePath.AddString(L"meshs");
	m_lbFilePath.AddString(L"effects");
	m_lbFilePath.AddString(L"uis");
	m_lbFilePath.AddString(L"3ds");
	m_lbFilePath.AddString(L"curs");
	// TODO:  在此添加额外的初始化
	for(int i=0;i<10;i++)
	{
		wchar_t buf[128];
		wchar_t ret[256];
		_swprintf(buf, L"exp_%d", i);
		DWORD d = ::GetProfileStringW(L"Phantom", buf, L"", ret, 256);
		ret[d] = 0;
		_swprintf(buf, L"expd_%d", i);
		wchar_t val[128];
		d = ::GetProfileStringW(L"Phantom", buf, L"0", val, 128);
		val[d] = 0;
		if(ret[0])
		{
			m_edFilePath.SetItemData(m_edFilePath.AddString(ret), _wtoi(val));
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CFileFilterDlg::OnCbnSelchangeExportPaths()
{
	if(m_edFilePath.GetCurSel() < 0)return;
	this->m_chkDir.SetCheck(m_edFilePath.GetItemData(m_edFilePath.GetCurSel()));
}


void CFileFilterDlg::OnBnClickedCreateDir()
{
	if(m_edFilePath.GetCurSel() < 0)return;
	wchar_t buf[128];
	wchar_t val[128];
	_swprintf(buf, L"expd_%d", m_edFilePath.GetCurSel());
	_swprintf(val, L"%d", this->m_chkDir.GetCheck());
	::WriteProfileStringW(L"Phantom", buf, val);
	m_edFilePath.SetItemData(m_edFilePath.GetCurSel(), this->m_chkDir.GetCheck());
}
