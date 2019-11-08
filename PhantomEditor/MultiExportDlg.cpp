/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// MultiExportDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "MultiExportDlg.h"
#include "afxdialogex.h"
#include "GCLanguage.h"
#include "ViewImageManager.h"

// CMultiExportDlg 对话框

IMPLEMENT_DYNAMIC(CMultiExportDlg, CDialogEx)

CMultiExportDlg::CMultiExportDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMultiExportDlg::IDD, pParent)
{

}

CMultiExportDlg::~CMultiExportDlg()
{
}

void CMultiExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SRC_PATH, m_edSrcPath);
	DDX_Control(pDX, IDC_DEST_PATH, m_edExportPath);
	DDX_Control(pDX, IDC_AUTO_RENAME, m_chkTextureRename);
	DDX_Control(pDX, IDC_AUTO_RENAME2, m_chkMeshRename);
}


BEGIN_MESSAGE_MAP(CMultiExportDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMultiExportDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMultiExportDlg 消息处理程序

extern	bool	LoadAllMeshs(std::vector<std::string>& files, const char* srcPath, const char* destPath, BOOL bRandomMesh, BOOL bRandomTexture);

void CMultiExportDlg::OnBnClickedOk()
{
	//std::string str		=	".rpgExp";
	//CString strPath;
	//CString strDest;
	//this->m_edSrcPath.GetWindowTextW(strPath);
	//this->m_edExportPath.GetWindowTextW(strDest);
	//strPath				+=	L"\\";
	//strDest				+=	L"\\";
	//PFileList* files	=	::enum_files(A(strPath), 100, &str, 1);
	//if(!files)
	//{
	//	MessageBox(Language(L"找不到rpgExp文件"));
	//	return;
	//}
	//if(MessageBox(Language(L"确实要批量转换吗？"), 0, MB_YESNO) == IDYES)
	//{
	//	IGeometryGroupManager* pGeom	=	CreateGeometryManager();
	//	//
	//	std::vector<std::string> fileS;
	//	for(int i=0;i<files->vFiles.size();i++)
	//	{
	//		char szPath[512], szDir[512], szFile[128];
	//		_splitpath(files->vFiles[i].szRealName, szPath, szDir, szFile, 0);
	//		strcat(szPath, szDir);
	//		strcat(szPath, szFile);
	//		strcat(szPath, ".rpgmesh");
	//		bool bConvert	=	false;
	//		try{
	//			bConvert	=	pGeom->ConvertFromFile(files->vFiles[i].szRealName, szPath);
	//		}
	//		catch(...)
	//		{
	//			bConvert	=	false;
	//		}
	//		fileS.push_back(files->vFiles[i].szRealName);
	//	}
	//	safe_release(pGeom);
	//	//LoadAllMeshs(fileS, A(strPath), A(strDest), this->m_chkMeshRename.GetCheck(), m_chkTextureRename.GetCheck());
	//	MessageBox(Language(L"批量转换成功？"));
	//}
	////
	//::destroyFileList(files);
	////
	//CDialogEx::OnOK();
}
