#include "StdAfx.h"
#include "GCViewTexture.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"

GCViewTextureEditor::GCViewTextureEditor(CPhantomGCDoc* docPtr)
	:GCViewBase(docPtr, Language(L"图片察看"), IDB_UI_VIEW_BMP)
{
	this->m_texturePtr	=	0;
}

GCViewTextureEditor::~GCViewTextureEditor(void)
{
	CloseFile();
}

HICON		GCViewTextureEditor::GetViewerIcon()
{
	static HICON	hIcon	=	AfxGetApp()->LoadIcon(IDI_TEXTURE_FILE_ICON);
	return hIcon;
}

void		GCViewTextureEditor::OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp)
{
	__super::OnPropertyChanged(wnd, pProp);
}

VOID		GCViewTextureEditor::OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem)	//更新选择树形控件
{
	if(treePtr == &m_treeView)
	{
		TreeItemType* type	=	this->GetSelectedItem();
		if(!type)
		{
			SetCurrentProperties(&m_viewerProps, L"文件属性", NULL);
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
VOID		GCViewTextureEditor::CloseFile(){
	safe_release(m_texturePtr);
}
BOOL		GCViewTextureEditor::OpenFile(const wchar_t* szFile)	//打开文件,如果失败则会自动删除
{
	this->m_strFile	=	szFile;
	if(!__super::OpenFile(szFile))
		return false;
	if(m_viewerProps.m_hWnd == 0)
		this->m_treeView.InsertItem(Language(L"TextureViewer", L"图片察看器"), 25, 25);
	wchar_t dir[512];
	GetCurrentDirectory(512, dir);
	m_texturePtr	=	LoadTexture(0, A(m_strFile.c_str()), A(dir));
	if(!m_texturePtr)
		return false;
	//
	RebuildTrees();
	if(m_viewerProps.m_hWnd == 0)
		InitViewerProps();
	return true;
}

VOID		GCViewTextureEditor::RebuildTrees()
{
	if(m_hViewItem)
		m_treeView.DeleteItem(m_hViewItem);
	wchar_t wFileName[256], wExt[256];
	_wsplitpath(m_strFile.c_str(), 0, 0, wFileName, wExt);
	wcscat(wFileName, wExt);
	m_hViewItem = m_treeView.InsertItem(wFileName, 18, 18);
	m_treeView.SetItemState(m_hViewItem, TVIS_BOLD, TVIS_BOLD);
	m_treeView.Expand(m_hViewItem, TVE_EXPAND);
	//
	this->ClearTreeItem();
	BuildAll();
	//
	m_treeView.m_callback	=	this;
}

VOID		GCViewTextureEditor::BuildAll()
{
	m_treeView.SelectItem(m_hViewItem);
	DeleteChildItems(m_hViewItem);
	m_treeView.SetItemData(m_hViewItem, AddTreeItem(TEXTURE_MAIN));
}

VOID		GCViewTextureEditor::OnActive()		//被激活
{
	__super::OnActive();
}

VOID		GCViewTextureEditor::OnDeActive()		//失去激活
{
	__super::OnDeActive();
}

void		GCViewTextureEditor::OnFrameMove()	//每次更新
{
}

void		GCViewTextureEditor::OnPushRenderObjects()	//渲染
{
}

BOOL		GCViewTextureEditor::SaveFile(BOOL bSaveAs)	//保存文件
{
	bool bNew	=	false;
	if(bSaveAs || m_strFile.size() == 0)
	{
		bNew	=	true;
		CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, formatw(L"%s|*.dds||", Language(L"TextureFileType", L"图片文件") ), GetMainFrame());
		wchar_t buffer[1024];
		dlg.m_pOFN->lpstrInitialDir	=	AToW(GetProjectPath(), buffer, 1024);
		if(dlg.DoModal() != IDOK)
			return false;
		SetProjectPath();
		wchar_t szPath[256], szFile[256], szDir[256];
		_wsplitpath(dlg.GetPathName(), szPath, szDir, szFile, 0);
		wcscat(szPath, szDir);
		wcscat(szPath, szFile);
		wcscat(szPath, L".dds");
		m_strFile	=	szPath;
	}
	if(m_texturePtr)
		m_texturePtr->SaveTextureTo(A(m_strFile.c_str()));
	return __super::SaveFile(bSaveAs);
}

void		GCViewTextureEditor::OnRenderingEnd()
{
	//Phantom::g_manager.DrawRect2D(
//	if(m_texturePtr)
//		GetDlgResManager()->DrawTexture(m_texturePtr, NULL, NULL, false, 0xffffffff);
}
//渲染
void		GCViewTextureEditor::OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI)
{
	switch(nMenuID)
	{
	default:
		pCmdUI->Enable(false);
		break;
	}
}


void		GCViewTextureEditor::OnCommandUI(INT nMenuID)
{
}
