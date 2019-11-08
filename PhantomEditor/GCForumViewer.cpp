#include "StdAfx.h"
#include "GCForumViewer.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"
#include "phantom-gcView.h"

GCForumViewer::GCForumViewer(CPhantomGCDoc* docPtr)
	:GCViewBase(docPtr, Language(L"主页"), IDB_UI_VIEW_BMP)
{
}

GCForumViewer::~GCForumViewer(void)
{
}

HICON		GCForumViewer::GetViewerIcon()
{
	static HICON	hIcon	=	AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	return hIcon;
}

void		GCForumViewer::OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp)
{
	__super::OnPropertyChanged(wnd, pProp);
}

VOID		GCForumViewer::OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem)	//更新选择树形控件
{
	if(treePtr == &m_treeView)
	{
		TreeItemType* type	=	this->GetSelectedItem();
		if(!type)
		{
			SetCurrentProperties(&m_viewerProps, L"主页属性", NULL);
			return;
		}
		switch(type->type)
		{
		case TEXTURE_MAIN:
			{
				SetCurrentProperties(NULL, NULL, NULL);
			}
			break;
		default:
			SetCurrentProperties(NULL, NULL, NULL);
			break;
		}
		return;
	}
	SetCurrentProperties(NULL, NULL, NULL);
}

VOID		GCForumViewer::GetNewFileName(wchar_t* szFile)
{
	wcscpy(szFile, Language(L"正版技术支持"));
}

std::wstring	g_openHttpFile;
VOID	SetOpenHttpFile(const wchar_t* szUrl)
{
	g_openHttpFile	=	szUrl;
}

BOOL		GCForumViewer::OpenFile(const wchar_t* szFile)	//打开文件,如果失败则会自动删除
{
	if(!__super::OpenFile(0))
		return false;
	this->m_treeView.InsertItem(Language(L"TextureViewer", Language(L"正版用户支持")), 25, 25);
	RebuildTrees();
	InitViewerProps();
	this->GetActiveView()->m_wndViewer	=	&m_viewerHttp;
	m_viewerHttp.Create(m_viewerHttp.IDD, GetActiveView());
	m_viewerHttp.ShowWindow(SW_SHOW);
	m_viewerHttp.Navigate(g_openHttpFile.c_str());
	m_viewerHttp.m_pBrowserApp->put_Silent(VARIANT_TRUE);
	CRect rc;
	m_viewerHttp.SetHostFlags(DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_FLAT_SCROLLBAR);
	this->GetActiveView()->GetWindowRect(&rc);
	m_viewerHttp.MoveWindow(0, 0, rc.Width(), rc.Height());
	GetRenderDevicePtr()->ShowWindow(false);
	return true;
}

VOID		GCForumViewer::RebuildTrees()
{
	this->ClearTreeItem();
	m_treeView.m_callback	=	this;
}

VOID		GCForumViewer::BuildAll()
{
}

VOID		GCForumViewer::OnActive()		//被激活
{
	GetRenderDevicePtr()->ShowWindow(false);
	__super::OnActive();
	GetRenderDevicePtr()->ShowWindow(false);
}

VOID		GCForumViewer::OnDeActive()		//失去激活
{
	GetRenderDevicePtr()->ShowWindow(true);
	__super::OnDeActive();
}

void		GCForumViewer::OnFrameMove(const RenderParameter& r)	//每次更新
{
}

void		GCForumViewer::OnPushRenderObjects(const RenderParameter& r)	//渲染
{
}

BOOL		GCForumViewer::SaveFile(BOOL bSaveAs)	//保存文件
{
	return __super::SaveFile(bSaveAs);
}

void		GCForumViewer::OnRenderingEnd(const RenderParameter& r)
{
}
//渲染
void		GCForumViewer::OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI)
{
	switch(nMenuID)
	{
	default:
		pCmdUI->Enable(false);
		break;
	}
}


void		GCForumViewer::OnCommandUI(INT nMenuID)
{
}
