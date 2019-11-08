/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/

#include "stdafx.h"
#include "mainfrm.h"
#include "FileView.h"
#include "Resource.h"
#include "phantom-gc.h"
#include "GCViewManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView*		g_pFileView	=	0;
CFileView*		GetFileViewPtr()
{
	return g_pFileView;
}

CFileView::CFileView()
{
	g_pFileView	=	this;
}

CFileView::~CFileView()
{
}

#define	ON_REG_COMMAND(id)		ON_UPDATE_COMMAND_UI(id, &CFileView::OnUpdateMenuCommand)\
	ON_COMMAND(id, &CFileView::OnMenuCommand)


BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	//
	ON_REG_COMMAND(IDM_ALL_MATERIAL)
	ON_REG_COMMAND(IDM_ALL_SUBGEOMETRY)
	ON_REG_COMMAND(IDM_ALL_PARTS)
	ON_REG_COMMAND(IDM_ALL_ACTIONS)
	ON_REG_COMMAND(IDM_ATTACH_EQUIPS)
	ON_REG_COMMAND(IDM_REMOVE_EQUIPS)
	ON_REG_COMMAND(IDM_ATTACH_ACTIONS)
	ON_REG_COMMAND(IDM_REMOVE_ACTIONS)
	ON_REG_COMMAND(ID_GEOMETRY_INSERTSHADER)
	ON_REG_COMMAND(ID_GEOM_ACTION_FROM_MIX)
	ON_REG_COMMAND(ID_GEOMETRY_NEW_KNIFE)
	ON_REG_COMMAND(ID_GEOMETRY_NEWPARTICLE)
	//
	ON_REG_COMMAND(ID_UI_INSERTANIMATION)
	ON_REG_COMMAND(ID_INSERT_UIGROUP)
	ON_REG_COMMAND(ID_SEL_ALL_ELEMENT)
	ON_REG_COMMAND(ID_UI_HIDE_CONTROLE)
	ON_REG_COMMAND(ID_UI_INSERTBODY)
	ON_REG_COMMAND(ID_UI_INSERTJOINT)
	ON_REG_COMMAND(ID_UI_INSERTSHAPE)
	//
	ON_REG_COMMAND(ID_UI_INSERTUVANIMATION)
	ON_REG_COMMAND(ID_UI_REMOVECONTROL)
	ON_REG_COMMAND(ID_UI_REMOVEANIMATION)
	ON_REG_COMMAND(ID_UI_REMOVEUVANIMATION)
	ON_REG_COMMAND(ID_UI_CLONEUVMAP)
	ON_REG_COMMAND(ID_UI_CLONETRACK)
	ON_REG_COMMAND(ID_UI_COPY)
	ON_REG_COMMAND(ID_UI_PASTE)
	ON_REG_COMMAND(ID_AUTO_GENERIC_UVMAP)
	//
	ON_REG_COMMAND(ID_UNIT2D_INSERTEQUIP)
	ON_REG_COMMAND(ID_UNIT2D_REMOVEEQUIP)
	ON_REG_COMMAND(ID_UNIT2D_INSERTANIMATION)
	ON_REG_COMMAND(ID_UNIT2D_REMOVEANIMATION)
	//
	ON_REG_COMMAND(ID_CURSOR_INSERTCURSOR)
	ON_REG_COMMAND(ID_CURSOR_DELETECURSOR)
	//
	ON_REG_COMMAND(ID_TERRAIN_CREATETERRAIN)
	ON_REG_COMMAND(ID_TERRAIN_REMOVETERRAIN)
	ON_REG_COMMAND(ID_OBJECT_INSERTOBJECTS)
	ON_REG_COMMAND(ID_OBJECT_REMOVEOBJECT)
	ON_REG_COMMAND(ID_UNIT_INSERTUNITS)
	ON_REG_COMMAND(ID_UNIT_REMOVEUNITS)
	//
	ON_REG_COMMAND(ID_PROJECT_IMPORTFILE)
	ON_REG_COMMAND(ID_PROJECT_REFRESHFILES)
	//
	ON_REG_COMMAND(ID_PLOT_INSERTCHAPTER)
	ON_REG_COMMAND(ID_PLOT_INSERTDRAMA)
	ON_REG_COMMAND(ID_PLOT_INSERTMODULE)
	ON_REG_COMMAND(ID_PLOT_INSERTACTION)
	ON_REG_COMMAND(ID_PLOT_INSERTELEMENT)
	ON_REG_COMMAND(ID_PLOTACTION_INSERTCALL)
	ON_REG_COMMAND(ID_PLOTRIGHT_INSERTEVENT)
	ON_REG_COMMAND(ID_PLOTRIGHT_REFRESH_ELEMENT)
	//
	ON_REG_COMMAND(ID_SCENE3D_INSERTSKY)
	ON_REG_COMMAND(ID_SCENE3D_SHADOW)
	ON_REG_COMMAND(ID_SCREEN_HDR)
	ON_REG_COMMAND(ID_SCENE3D_INSERTFOG)
	ON_REG_COMMAND(ID_SCENE3D_INSERTCAMERA)
	ON_REG_COMMAND(ID_SCENE3D_INSERT_WATER)
	ON_REG_COMMAND(ID_SCENE3D_INSERTGRASS)
	ON_REG_COMMAND(ID_SCENE3D_INSERTOBJECTS)
	ON_REG_COMMAND(ID_SCENE3D_OPENFILE)
	ON_REG_COMMAND(ID_SCENE3D_REFRESHOBJs)
	ON_REG_COMMAND(ID_SCENE3D_ENV)
	ON_REG_COMMAND(ID_ADD_MOVE_POINT)
	//
	ON_REG_COMMAND(ID_EFFECT_ADDPARTICLE)
	ON_REG_COMMAND(ID_EFFECT_ADDGEOMETRY)
	ON_REG_COMMAND(ID_EFFECT_ADDSOUND)
	ON_REG_COMMAND(ID_EFFECT_INSERTACTION)
	ON_REG_COMMAND(ID_EFFECT_INSERT_As)
	ON_REG_COMMAND(ID_EFFECT_ADDUNITCONTROL)
	ON_REG_COMMAND(ID_EFFECT_CHILD)
	//
	ON_REG_COMMAND(ID_SCENE3D_INSERTANIMATION)
	ON_REG_COMMAND(ID_SCENE3D_SCAUNIT3D)
	ON_REG_COMMAND(ID_SCENE3D_SCAGEOMETRY)
	ON_REG_COMMAND(ID_SCENE3D_SCACAMERA)
	ON_REG_COMMAND(ID_SCENE3D_SCASKY)
	ON_REG_COMMAND(ID_SCENE3D_SCAFOG)
	ON_REG_COMMAND(ID_SCENE3D_SCAWATER)
	ON_REG_COMMAND(ID_SCENE3D_INSERTSCATRACK)
	ON_REG_COMMAND(ID_SCENE3D_BINDSELECT)
	ON_REG_COMMAND(ID_SCENE3D_SCAGRASS)
	//
	ON_REG_COMMAND(ID_LOGIC_INSERTLINE)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	//if (!m_wndGeometryView.Create(dwViewStyle, rectDummy, this, 4))
	//{
	//	TRACE0("未能创建文件视图\n");
	//	return -1;      // 未能创建
	//}
	//SetSelect(&m_wndGeometryView);
	AdjustLayout();
	return 0;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if(!GetViewManager()->GetActiveView())
		return;
	CTreeCtrl* pWndTree = GetViewManager()->GetActiveView()->GetTreeView();
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	if(GetViewManager()->GetActiveView())
	{
		INT nID	=	GetViewManager()->GetActiveView()->GetPopupMenuID();
		if(nID)
		{
			CMenu mnu;
			mnu.LoadMenuW(IDR_POPUP_VIEW_GEOMETRY_EDITOR);
			theApp.GetContextMenuManager()->ShowPopupMenu(mnu.GetSubMenu(nID - 1)->m_hMenu, point.x, point.y, this, TRUE);
		}
	}
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}
	CRect rectClient;
	GetClientRect(rectClient);
	int cyTlb = 0;
	for(int i=0;i<GetViewManager()->GetViewCount();i++)
	{
		if(GetViewManager()->GetView(i)->GetTreeView())
			GetViewManager()->GetView(i)->GetTreeView()->SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetTreeView())
	{
		CRect rectTree;
		GetViewManager()->GetActiveView()->GetTreeView()->GetWindowRect(rectTree);
		ScreenToClient(rectTree);
		rectTree.InflateRect(1, 1);
		dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
	}
	else
	{
		CRect rc;
		GetClientRect(&rc);
		dc.FillRect(&rc, &CBrush(RGB(255,255,255)));
	}
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetTreeView())
		GetViewManager()->GetActiveView()->GetTreeView()->SetFocus();
}

BOOL CFileView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD	wNotifyCode = HIWORD(wParam);
	WORD	wID			= LOWORD(wParam);
	switch(wNotifyCode)
	{
	case CN_COMMAND:
		{
			if(GetViewManager()->GetActiveView())
				GetViewManager()->GetActiveView()->OnCommandUI(wID);
		}
		break;
	}
	//
	return CDockablePane::OnCommand(wParam, lParam);
}


void CFileView::OnUpdateMenuCommand(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL);
	if(GetViewManager()->GetActiveView())
		GetViewManager()->GetActiveView()->OnUpdateUI(pCmdUI->m_nID, pCmdUI);
}

void CFileView::OnMenuCommand(){}




CFileBrowser*		g_pFileBrowser	=	0;
CFileBrowser*		GetFileBrowserPtr()
{
	return g_pFileBrowser;
}

CFileBrowser::CFileBrowser()
	:m_treeView(IDB_GAME_VIEWER)
{
	this->m_hViewItem = 0;
	g_pFileBrowser	=	this;
}

CFileBrowser::~CFileBrowser()
{
}

#define	ON_REG_COMMAND(id)		ON_UPDATE_COMMAND_UI(id, &CFileBrowser::OnUpdateMenuCommand)\
	ON_COMMAND(id, &CFileBrowser::OnMenuCommand)


BEGIN_MESSAGE_MAP(CFileBrowser, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CFileBrowser::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_hViewItem = 0;
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	m_treeView.Create(dwViewStyle, rectDummy, this, 0);
	//m_filter.Create(m_filter.IDD, this);
	//m_filter.ShowWindow(SW_SHOW);
	//if (!m_wndGeometryView.Create(dwViewStyle, rectDummy, this, 4))
	//{
	//	TRACE0("未能创建文件视图\n");
	//	return -1;      // 未能创建
	//}
	//SetSelect(&m_wndGeometryView);
	AdjustLayout();
	return 0;
}

void CFileBrowser::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFileBrowser::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if(!GetViewManager()->GetActiveView())
		return;
	CTreeCtrl* pWndTree = GetViewManager()->GetActiveView()->GetTreeView();
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	//if(GetViewManager()->GetActiveView())
	//{
	//	INT nID	=	GetViewManager()->GetActiveView()->GetPopupMenuID();
	//	if(nID)
	//	{
	//		CMenu mnu;
	//		mnu.LoadMenuW(IDR_POPUP_VIEW_GEOMETRY_EDITOR);
	//		theApp.GetContextMenuManager()->ShowPopupMenu(mnu.GetSubMenu(nID - 1)->m_hMenu, point.x, point.y, this, TRUE);
	//	}
	//}
}

void CFileBrowser::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}
	CRect rectClient;
	GetClientRect(rectClient);
	int cyTlb = 0;
	if(m_treeView.m_hWnd)
	{
		CRect rcTree = rectClient;
		rcTree.bottom -= 200;
		m_treeView.SetWindowPos(NULL, rcTree.left + 1, rcTree.top + cyTlb + 1, rcTree.Width() - 2, rcTree.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
		rcTree = rectClient;
		rcTree.top = rcTree.bottom - 200;
		//m_filter.SetWindowPos(NULL, rcTree.left + 1, rcTree.top + cyTlb + 1, rcTree.Width() - 2, rcTree.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CFileBrowser::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	if(m_treeView.m_hWnd)
	{
		CRect rectTree;
		m_treeView.GetWindowRect(rectTree);
		ScreenToClient(rectTree);
		rectTree.InflateRect(1, 1);
		dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
	}
	else
	{
		CRect rc;
		GetClientRect(&rc);
		dc.FillRect(&rc, &CBrush(RGB(255,255,255)));
	}
}

void CFileBrowser::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	if(m_treeView.m_hWnd)
		m_treeView.SetFocus();
}

BOOL CFileBrowser::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD	wNotifyCode = HIWORD(wParam);
	WORD	wID			= LOWORD(wParam);
	switch(wNotifyCode)
	{
	case CN_COMMAND:
		{
			//if(GetViewManager()->GetActiveView())
			//	GetViewManager()->GetActiveView()->OnCommandUI(wID);

		}
		break;
	}
	//
	return CDockablePane::OnCommand(wParam, lParam);
}


void CFileBrowser::OnUpdateMenuCommand(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL);
	//if(GetViewManager()->GetActiveView())
	//	GetViewManager()->GetActiveView()->OnUpdateUI(pCmdUI->m_nID, pCmdUI);
}

void CFileBrowser::OnMenuCommand(){}
void CFileBrowser::OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem)	//更新选择树形控件
{
	std::map<HTREEITEM, std::wstring>::iterator it = m_files.find(hAddItem);
	if(it != m_files.end())
	{
		const wchar_t* szFile = it->second.c_str();
		int numView = GetViewManager()->GetViewCount();
		for(int i=0;i<numView;i++)
		{
			if(GetViewManager()->GetView(i)->IsModifyed())
			{
				if(MessageBox(Language(L"是否保存？", L"当前有文件被修改，是否保存？ 保存之后才能打开这个文件"), L"消息框", MB_YESNO) == IDNO)
					return;
			}
			if(wcsicmp(GetViewManager()->GetView(i)->GetFileName(), szFile) == 0)
				return;
			wchar_t szExt[128];
			_wsplitpath(szFile, 0, 0, 0, szExt);
			wchar_t szExt2[128];
			_wsplitpath(GetViewManager()->GetView(i)->GetFileName(), 0, 0, 0, szExt2);
			if(wcsicmp(szExt2, szExt) == 0)
			{
				GetViewManager()->GetView(i)->OpenFile(szFile);
				return;
			}
		}
		AfxGetApp()->OpenDocumentFile(szFile);
		UpdateMainTitle();
	}
	
	
}
VOID CFileBrowser::Refresh(BOOL bRebuild)
{
	if(bRebuild)
	{
		m_treeView.DeleteItem(m_hViewItem);
		m_hViewItem = 0;
	}
	else{
	if(!this->IsWindowVisible() || m_hViewItem)
		return;
	}
	if(m_hViewItem)
		m_treeView.DeleteItem(m_hViewItem);
	m_treeView.m_callback = this;
	m_hViewItem = m_treeView.InsertItem(L"文件列表", 19, 5);
	m_treeView.SetItemState(m_hViewItem, TVIS_BOLD, TVIS_BOLD);
	m_treeView.Expand(m_hViewItem, TVE_EXPAND);
	//
	wchar_t szDrive[256], szDir[256];
	//_wsplitpath(_wpgmptr, szDrive, szDir, 0, 0);
	//wcscat(szDrive, szDir);
	wcscpy(szDrive, W(GetProjectPath()).buf);
	m_files.clear();
	RebuildSubDir(m_hViewItem, L"", szDrive);
}

extern int			GetIconFromExt(const char* szExt);

VOID CFileBrowser::RebuildSubDir(HTREEITEM hItem, const wchar_t* szDir, const wchar_t* szPath)
{
	wchar_t szNewPath[512];
	wcscpy(szNewPath, szPath);
	wcscat(szNewPath, szDir);
	wchar_t	ch	=	szNewPath[wcslen(szNewPath) - 1];
	if(ch != L'\\' && ch != L'/')
		wcscat(szNewPath, L"\\");
	wcscat(szNewPath, L"*.*");
	WIN32_FIND_DATA win32;
	memset(&win32, 0, sizeof(win32));
	HANDLE hFile	=	::FindFirstFile(szNewPath, &win32);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		while(true)
		{
			if(win32.cFileName[0] != L'.' && !(win32.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				if((win32.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
				{
					wcscpy(szNewPath, szDir);
					wcscat(szNewPath, win32.cFileName);
					wcscat(szNewPath, L"\\");
					HTREEITEM hItemChild	=	m_treeView.InsertItem(win32.cFileName, 20, 20, hItem);
					RebuildSubDir(hItemChild, szNewPath, szPath);
				}
				else
				{
					wcscpy(szNewPath, szDir);
					wcscat(szNewPath, win32.cFileName);
					wchar_t szExt[128];
					_wsplitpath(szNewPath, 0, 0, 0, szExt);
					//
					//if(GetIconFromExt(A(szExt).buf) >= 0)//wcsicmp(szExt, L".rpgmesh") == 0 || wcsicmp(szExt, L".rpgunit") == 0 || wcsicmp(szExt, L".z_rpg3D") == 0 || wcsicmp(szExt, L".geoms") == 0 || wcsicmp(szExt, L".z_rpgMesh") == 0)
					{
						FileName fname	=	A(szNewPath).buf;
						fname.setIsFile();
						//
						int iconIndex		=	GetIconFromExt(A(szExt));//rpgCur;*.rpgmesh;*rpg3d;*.geoms;*.rpgExp;*.obj;*.zui;*.rpgui;logic_system.txt;*.rpgProj;*.rpg2d;*.rpg2ds;*.rpgscene;*.ogg;*.mp3;*.wav;*.mid;*.rpgEffect
						if(iconIndex < 0)iconIndex = 11;
						if(wcsicmp(L".rpgmesh", szExt) == 0 || wcsicmp(L".rpgCur", szExt) == 0 ||wcsicmp(L".rpgunit", szExt) == 0||wcsicmp(L".rpgui", szExt) == 0 || wcsicmp(L".rpgEffect", szExt) == 0)//||wcsicmp(L".ogg", szExt) == 0||wcsicmp(L".wav", szExt) == 0||wcsicmp(L".mp3", szExt) == 0){
						{
							HTREEITEM hData		=	m_treeView.InsertItem(win32.cFileName, iconIndex, iconIndex, hItem);
							m_treeView.SetItemData(hData, 0);
							m_files[hData] = szNewPath;
						}
					}
				}
			}
			if(!::FindNextFile(hFile, &win32))
				break;
		}
	}
	FindClose(hFile);
	//
}

