/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// CreateFileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "CreateFileDlg.h"
#include "afxdialogex.h"
#include "GCViewBase.h"
#include "SceneCreateDlg.h"

// CCreateFileDlg 对话框

IMPLEMENT_DYNAMIC(CCreateFileDlg, CDialogBase)

CCreateFileDlg::CCreateFileDlg(CWnd* pParent /*=NULL*/)
	: CDialogBase(CCreateFileDlg::IDD, pParent)
{

}

CCreateFileDlg::~CCreateFileDlg()
{
}

void CCreateFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CREATE_UI, m_chkUIFile);
	DDX_Control(pDX, IDC_CREATE_UNIT2D, m_chkUnit2D);
	DDX_Control(pDX, IDC_CREATE_CURSORS, m_chkCursors);
	DDX_Control(pDX, IDC_CREATE_SCENE2D, m_chkScene2D);
	DDX_Control(pDX, IDC_CREATE_GAME_PROJECT, m_chkGameProject);
	DDX_Control(pDX, IDC_CREATE_SCENE3D, m_chkScene3D);
	DDX_Control(pDX, IDC_CREATE_UNIT3D, m_chkUnit3D);
}


BEGIN_MESSAGE_MAP(CCreateFileDlg, CDialogBase)
	ON_BN_CLICKED(IDOK, &CCreateFileDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CREATE_UI, &CCreateFileDlg::OnBnClickedCreateUi)
	ON_BN_DOUBLECLICKED(IDC_CREATE_UI, &CCreateFileDlg::OnBnDoubleclickedCreateUi)
	ON_BN_CLICKED(IDC_CREATE_UNIT2D, &CCreateFileDlg::OnBnClickedCreateUnit2d)
	ON_BN_CLICKED(IDC_CREATE_CURSORS, &CCreateFileDlg::OnBnClickedCreateCursors)
	ON_BN_CLICKED(IDC_CREATE_SCENE2D, &CCreateFileDlg::OnBnClickedCreateScene2d)
	ON_BN_CLICKED(IDC_CREATE_GAME_PROJECT, &CCreateFileDlg::OnBnClickedCreateGameProject)
	ON_BN_CLICKED(IDC_CREATE_SCENE3D, &CCreateFileDlg::OnBnClickedCreateScene3d)
	ON_BN_CLICKED(IDC_CREATE_UNIT3D, &CCreateFileDlg::OnBnClickedCreateUnit3d)
	ON_BN_DOUBLECLICKED(IDC_CREATE_UNIT3D, &CCreateFileDlg::OnBnDoubleclickedCreateUnit3d)
	ON_BN_DOUBLECLICKED(IDC_CREATE_SCENE3D, &CCreateFileDlg::OnBnDoubleclickedCreateScene3d)
END_MESSAGE_MAP()


// CCreateFileDlg 消息处理程序


BOOL CCreateFileDlg::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	m_createType	=	-1;
	m_createImage.LoadFromResource(AfxGetResourceHandle(), IDR_NEW_FILES);
	int imageCount	=	7;
	m_chkUIFile.SetImage(m_createImage, 2, imageCount, 0, true);
	m_chkUIFile.m_bDrawTextInLeft	=	TRUE;
	m_chkUIFile.SetWindowTextW(Language(L"CreateNewUI", L"新建界面(.rpgui)文件"));
	//m_chkCursors.SetImage(m_createImage, 2, imageCount, 2, true);
	//m_chkCursors.m_bDrawTextInLeft	=	TRUE;
	//m_chkCursors.SetWindowTextW(Language(L"CreateNewCursor", L"新建光标集合对象(.rpgCur)文件"));
	m_chkCursors.ShowWindow(SW_HIDE);
	//m_chkUnit2D.SetImage(m_createImage, 2, imageCount, 1, true);
	//m_chkUnit2D.m_bDrawTextInLeft	=	TRUE;
	//m_chkUnit2D.SetWindowTextW(Language(L"CreateNewUnit2D", L"新建2D单位对象(.rpg2D)文件"));
	m_chkUnit2D.ShowWindow(SW_HIDE);
	//m_chkScene2D.SetImage(m_createImage, 2, imageCount, 3, true);
	//m_chkScene2D.m_bDrawTextInLeft	=	TRUE;
	//m_chkScene2D.SetWindowTextW(Language(L"CreateNew2DScene", L"新建2D场景对象(.rpg2Ds)文件"));
	m_chkScene2D.ShowWindow(SW_HIDE);
	//
	m_chkUnit3D.SetImage(m_createImage, 2, imageCount, 6, true);
	m_chkUnit3D.m_bDrawTextInLeft	=	TRUE;
	m_chkUnit3D.SetWindowTextW(Language(L"CreateNewUnit3D", L"新建3D单位对象(.rpgunit)文件"));
	m_chkUnit3D.ShowWindow(SW_HIDE);
	//
	m_chkScene3D.SetImage(m_createImage, 2, imageCount, 5, true);
	m_chkScene3D.m_bDrawTextInLeft	=	TRUE;
	m_chkScene3D.SetWindowTextW(Language(L"CreateNewScene3D", L"新建3D场景对象(.rpgscene)文件"));
	//
	//m_chkGameProject.SetImage(m_createImage, 2, imageCount, 4, true);
	//m_chkGameProject.m_bDrawTextInLeft	=	TRUE;
	//m_chkGameProject.SetWindowTextW(Language(L"CreateNewGameProj", L"新建游戏项目(.rpgProj)文件"));
	m_chkGameProject.ShowWindow(SW_HIDE);
	//
	CRect rcClient;
	GetClientRect(&rcClient);
	INT nTop	=	5;
	m_chkUIFile.MoveWindow(&CRect(rcClient.left + 5, nTop, rcClient.right - 5, nTop + 32));
	nTop		+=	32;
	//m_chkCursors.MoveWindow(&CRect(rcClient.left + 5, nTop, rcClient.right - 5, nTop + 32));
	//nTop		+=	32;
	//m_chkUnit2D.MoveWindow(&CRect(rcClient.left + 5, nTop, rcClient.right - 5, nTop + 32));
	//nTop		+=	32;
	//m_chkScene2D.MoveWindow(&CRect(rcClient.left + 5, nTop, rcClient.right - 5, nTop + 32));
	//nTop		+=	32;
	//m_chkUnit3D.MoveWindow(&CRect(rcClient.left + 5, nTop, rcClient.right - 5, nTop + 32));
	//nTop		+=	32;
	m_chkScene3D.MoveWindow(&CRect(rcClient.left + 5, nTop, rcClient.right - 5, nTop + 32));
	//nTop		+=	32;
	//m_chkGameProject.MoveWindow(&CRect(rcClient.left + 5, nTop, rcClient.right - 5, nTop + 32));
	return TRUE;
}


//创建多级目录
void			createFolders(const char* szSubFolders)
{
	char* strPath=(char*)szSubFolders;
	int len=(int)strlen(strPath);
	char strDir[512];
	for(int i=0;i<len;i++)
	{
		strDir[i]=strPath[i];
		if(strPath[i]!='\\' && strPath[i]!='/')
		{
			continue;
		}
		else
		{
			strDir[i]=0;
			if(!Phantom::fexist(strDir))
				if(!::CreateDirectoryA(strDir,NULL))
				{
//					logout("不能创建文件夹[%s] xx",strDir);
				}
				else
				{
//					logout("创建文件夹成功[%s] !!",strDir);
				}
				strDir[i]='\\';
		}
	}
}

VOID	CreateProjectDir(const char* szPath, const char* szDir)
{
	char szTemp[256];
	strcpy(szTemp, szPath);
	strcat(szTemp, szDir);
	createFolders(szTemp);
}

VOID	CreateGameProject(const char* szPath)					//在指定目录,创建一个游戏管理器
{
	Text text	=	szPath;
	//CreateProjectDir(text, "unit3d\\");
	//CreateProjectDir(text, "ui\\");
	//CreateProjectDir(text, "effect\\");
	//CreateProjectDir(text, "cursor\\");
	//CreateProjectDir(text, "sound\\");
	//CreateProjectDir(text, "music\\");
	//CreateProjectDir(text, "texture\\");
	//CreateProjectDir(text, "unit2d\\");
	//CreateProjectDir(text, "scene2d\\");
	//CreateProjectDir(text, "scene3d\\");
	//CreateProjectDir(text, "text\\");
	//CreateProjectDir(text, "scripts\\");
	//CreateProjectDir(text, "other\\");
}

void CCreateFileDlg::OnBnClickedOk()
{
	if(this->m_chkUIFile.GetCheck())
		m_createType	=	FileViewerType_UI;
	else if(this->m_chkUnit2D.GetCheck())
		m_createType	=	FileViewerType_Unit2D;
	else if(this->m_chkCursors.GetCheck())
		m_createType	=	FileViewerType_CursorEditor;
	else if(this->m_chkScene2D.GetCheck())
		m_createType	=	FileViewerType_Scene;
	else if(this->m_chkUnit3D.GetCheck())
		m_createType	=	FileViewerType_Unit3D;
	else if(this->m_chkGameProject.GetCheck())
	{
		//if (afxShellManager == NULL)
		//{
		//	CWinAppEx* pApp = DYNAMIC_DOWNCAST(CWinAppEx, AfxGetApp());
		//	if (pApp != NULL)
		//	{
		//		pApp->InitShellManager();
		//	}
		//}
		//BOOL bUpdate = false;
		//CString strPath;
		//if (afxShellManager == NULL)
		//{
		//	ASSERT(FALSE);
		//}
		//else
		//{
		//	bUpdate = afxShellManager->BrowseForFolder(strPath, this, W(GetProjectPath()), Language(L"选择要保存的项目工程目录"), BIF_RETURNONLYFSDIRS|BIF_USENEWUI );
		//}
		//if(bUpdate)
		//{
		//	strPath			+=	L"\\";
		//	GUID guid		=	GUID_NULL;
		//	::CoCreateGuid(&guid);
		//	CProject gameDirector;
		//	gameDirector.SetGUID(guid);
		//	CreateGameProject(A(strPath));
		//	strPath			+=	gameDirector.GetName();
		//	strPath			+=	L".rpgProj";
		//	gameDirector.Save(A(strPath));
		//	m_createType	=	FileViewerType_GameProject;
		//	m_strGameProj	=	strPath;
		//}

	}
	else if(this->m_chkScene3D.GetCheck())
	{
		//CSceneCreateDlg dlg(this);
		//if(dlg.DoModal() != IDOK)
		//	return;
		//
		//m_strScene3D	=	dlg.m_strSaveFile;
		m_createType	=	FileViewerType_GCSceneEditor3D;
	}
	else
	{
		CDialogBase::OnCancel();
		return;
	}
	CDialogBase::OnOK();
}

VOID	CCreateFileDlg::SetCheck(CImageCheck* c)
{
	m_chkUIFile.SetCheck(c == &m_chkUIFile);
	m_chkUnit2D.SetCheck(c == &m_chkUnit2D);
	m_chkCursors.SetCheck(c == &m_chkCursors);
	m_chkScene2D.SetCheck(c == &m_chkScene2D);
	m_chkGameProject.SetCheck(c == &m_chkGameProject);
	m_chkScene3D.SetCheck(c == &m_chkScene3D);
	m_chkUnit3D.SetCheck(c == &m_chkUnit3D);
}

void CCreateFileDlg::OnBnClickedCreateUi()
{
	SetCheck(&m_chkUIFile);
}
void CCreateFileDlg::OnBnDoubleclickedCreateUi()
{
	OnBnClickedOk();
}
void CCreateFileDlg::OnBnClickedCreateUnit2d()
{
	SetCheck(&m_chkUnit2D);
}
void CCreateFileDlg::OnBnClickedCreateCursors()
{
	SetCheck(&m_chkCursors);
}
void CCreateFileDlg::OnBnClickedCreateScene2d()
{
	SetCheck(&m_chkScene2D);
}
void CCreateFileDlg::OnBnClickedCreateGameProject()
{
	SetCheck(&m_chkGameProject);
}
void CCreateFileDlg::OnBnClickedCreateScene3d()
{
	SetCheck(&m_chkScene3D);
}

void CCreateFileDlg::OnBnClickedCreateUnit3d()
{
	SetCheck(&m_chkUnit3D);
}


void CCreateFileDlg::OnBnDoubleclickedCreateUnit3d()
{
	OnBnClickedOk();
}


void CCreateFileDlg::OnBnDoubleclickedCreateScene3d()
{
	OnBnClickedOk();
}
