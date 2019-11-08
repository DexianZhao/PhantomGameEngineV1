/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// PropertiesUIWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "PropertiesUIWnd.h"


// CPropertiesUIWnd

IMPLEMENT_DYNAMIC(CPropertiesUIWnd, CDockablePane)

CPropertiesUIWnd::CPropertiesUIWnd()
{

}

CPropertiesUIWnd::~CPropertiesUIWnd()
{
}


BEGIN_MESSAGE_MAP(CPropertiesUIWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CPropertiesUIWnd 消息处理程序



VOID	CPropertiesUIWnd::SetEditorMode(FileViewerType type)
{
	//if(type == FileViewerType_UI)
	//{
	//	m_dlgUI.ShowWindow(SW_SHOW);
	//	m_dlgUI.EnableWindow(true);
	//	m_sceneEditor.ShowWindow(SW_HIDE);
	//	SetWindowText(Language(L"UI编辑功能"));
	//	HICON hUIProperties	= (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_UI_FILE_ICON), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//	SetIcon(hUIProperties, FALSE);
	//	this->GetParent()->Invalidate();
	//}
	//else if(type == FileViewerType_Scene)
	//{
	//	m_dlgUI.ShowWindow(SW_HIDE);
	//	m_sceneEditor.ShowWindow(SW_SHOW);
	//	m_sceneEditor.EnableWindow(true);
	//	SetWindowText(Language(L"场景编辑功能"));
	//	HICON hUIProperties	= (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_SCENE2D_ICON), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//	SetIcon(hUIProperties, FALSE);
	//	this->GetParent()->Invalidate();
	//}
	//else
	//{
	//	m_dlgUI.EnableWindow(false);
	//	m_sceneEditor.EnableWindow(false);
	//}
}

int CPropertiesUIWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	//m_dlgUI.Create(m_dlgUI.IDD, this);
	//m_dlgUI.ShowWindow(SW_SHOW);
	//m_dlgUI.EnableWindow(false);
	////
	//m_sceneEditor.Create(m_sceneEditor.IDD, this);
	//m_sceneEditor.ShowWindow(SW_HIDE);
	//m_sceneEditor.EnableWindow(false);
	return 0;
}


void CPropertiesUIWnd::OnSize(UINT nType, int cx, int cy)
{
	//if(m_dlgUI.m_hWnd)
	//{
	//	CRect rc;
	//	GetClientRect(&rc);
	//	m_dlgUI.MoveWindow(&rc);
	//}
	//if(m_sceneEditor.m_hWnd)
	//{
	//	CRect rc;
	//	GetClientRect(&rc);
	//	m_sceneEditor.MoveWindow(&rc);
	//}
	CDockablePane::OnSize(nType, cx, cy);
}
