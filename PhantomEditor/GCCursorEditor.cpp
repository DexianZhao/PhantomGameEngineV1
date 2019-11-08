//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   GCCursorEditor.cpp
	软件名称   :   幻影游戏引擎

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	
	Copyright 2009-2016 Zhao Dexian
	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////



#include "StdAfx.h"
#include "GCCursorEditor.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"

class GCViewCursorEditor;

struct	CursorCopyData
{
	TreeItemType		type;
	GCViewCursorEditor*	editor;
	CursorCopyData()
	{
		editor	=	0;
	}
};

std::vector<CursorCopyData>		g_copyCursors;

VOID		SetCursorCopy(GCViewCursorEditor* e, std::vector<TreeItemType*>* vs)
{
	g_copyCursors.clear();
	for(int i=0;i<vs->size();i++)
	{
		CursorCopyData ud;
		ud.editor	=	e;
		ud.type		=	*vs->at(i);
		g_copyCursors.push_back(ud);
	}
}

BOOL		IsCanPasteCursor(int nType)
{
	for(int i=0;i<g_copyCursors.size();i++)
	{
		if(g_copyCursors[i].type.type == nType)
		{
			return true;
		}
	}
	return false;
}

GCViewCursorEditor::GCViewCursorEditor(CPhantomGCDoc* docPtr)
	:GCViewBase(docPtr, Language(L"光标编辑器"), IDB_UI_VIEW_BMP)
{
	this->m_cursorPtr	=	0;
}

GCViewCursorEditor::~GCViewCursorEditor(void)
{
	CloseFile();
}

HICON		GCViewCursorEditor::GetViewerIcon()
{
	static HICON	hIcon	=	AfxGetApp()->LoadIcon(IDI_UI_CURSOR_ICON);
	return hIcon;
}


VOID		GCViewCursorEditor::LoopAllCursors(ProCursors proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRename, BOOL bDefault)
{
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			selectTypes[hItem]	=	type;
		}
	}
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == CURSOR_CURSOR)
		{
			ICursorRender* cursor	=	m_cursorPtr->GetCursorByID(type->p1);
			if(proc)
				(*proc)(cursor, nParameter, nParameter2);
			SetModifyed(true);
			if(bDefault)
				m_cursorPtr->SetDefaultCursor(cursor);
			if(bRename)
				m_treeView.SetItemText(it->first, W(cursor->GetCursorName()));
		}
	}
}

VOID		ProcCursor_Speed(ICursorRender* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetPlaySpeed(*(float*)nParameter);
}

VOID		ProcCursor_File(ICursorRender* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetTextureFile((const char*)nParameter);
	data->Play();
}

VOID		ProcCursor_FileClear(ICursorRender* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetTextureFile(0);
}

VOID		ProcCursor_Name(ICursorRender* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetCursorName((const char*)nParameter);
}

VOID		ProcCursor_CenterX(ICursorRender* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetCenterPos(PPixel(nParameter, data->GetCenterPos().y));
}
VOID		ProcCursor_CenterY(ICursorRender* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetCenterPos(PPixel(data->GetCenterPos().x, nParameter));
}
VOID		ProcCursor_UVMapW(ICursorRender* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetUVMapW(nParameter);
}
VOID		ProcCursor_UVMapH(ICursorRender* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetUVMapH(nParameter);
}
VOID		ProcCursor_UVMapBegin(ICursorRender* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetUVMapBegin(nParameter);
}
VOID		ProcCursor_UVMapCount(ICursorRender* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetUVMapCount(nParameter);
}

void		GCViewCursorEditor::OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp)
{
	__super::OnPropertyChanged(wnd, pProp);
	if(wnd == &m_cursorProp)
	{
		if(pProp == m_cursorPlaySpeed)
		{
			LoopAllCursors(ProcCursor_Speed, (LPARAM)GetPropFloatPtr(this->m_cursorPlaySpeed), 0);
		}
		else if(pProp == m_cursorFileSet)
		{
			std::wstring str;
			GetPropFile(m_cursorFileSet, str);
			LoopAllCursors(ProcCursor_File, (LPARAM)A(str.c_str()).buf, TRUE);
		}
		else if(pProp == m_cursorFileClear)
		{
			LoopAllCursors(ProcCursor_FileClear);
		}
		else if(pProp == m_cursorName)
		{
			std::string str;
			GetPropText(m_cursorName, str);
			LoopAllCursors(ProcCursor_Name, (LPARAM)str.c_str(), 0, true);
		}
		else if(pProp == m_cursorIsDefault)
		{
			LoopAllCursors(0, 0, 0, 0, true);
		}
		else if(pProp == m_cursorCenterX)
		{
			LoopAllCursors(ProcCursor_CenterX, (LPARAM)GetPropInt(this->m_cursorCenterX), 0);
		}
		else if(pProp == m_cursorCenterY)
		{
			LoopAllCursors(ProcCursor_CenterY, (LPARAM)GetPropInt(this->m_cursorCenterY), 0);
		}
		else if(pProp == m_cursorUVMapW)
		{
			LoopAllCursors(ProcCursor_UVMapW, (LPARAM)GetPropInt(this->m_cursorUVMapW), 0);
		}
		else if(pProp == m_cursorUVMapH)
		{
			LoopAllCursors(ProcCursor_UVMapH, (LPARAM)GetPropInt(this->m_cursorUVMapH), 0);
		}
		else if(pProp == m_cursorUVMapBegin)
		{
			LoopAllCursors(ProcCursor_UVMapBegin, (LPARAM)GetPropInt(this->m_cursorUVMapBegin), 0);
		}
		else if(pProp == m_cursorUVMapCount)
		{
			LoopAllCursors(ProcCursor_UVMapCount, (LPARAM)GetPropInt(this->m_cursorUVMapCount), 0);
		}
	}
	else if(wnd == &m_managerProp)
	{
		if(pProp == m_managerHardware)
		{
			m_cursorPtr->SetHardwardCursor(GetPropBool(m_managerHardware));
			this->SetModifyed();
		}
	}
}

VOID		GCViewCursorEditor::OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem)	//更新选择树形控件
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
		case CURSOR_MAIN:
			{
				SetCurrentProperties(&m_managerProp, L"光标属性", NULL);
				SetPropBool(this->m_managerHardware, this->m_cursorPtr->IsHardwardCursor());
			}
			break;
		case CURSOR_CURSOR:
			{
				SetCurrentProperties(&m_cursorProp, L"光标属性", NULL);
				//
				ICursorRender* ani	=	m_cursorPtr->GetCursorByID(type->p1);
				m_cursorProp.EnableRender(false);
				if(!ani)
				{
					m_cursorProp.EnableWindow(FALSE);
					m_cursorProp.EnableRender(true);
					break;
				}
				//
				SetPropText(m_cursorName, ani->GetCursorName());
				SetPropBool(m_cursorIsDefault, ani == m_cursorPtr->GetDefaultCursor());
				SetPropFile(m_cursorFileSet, ani->GetTextureFile());
				SetPropFloat(m_cursorPlaySpeed, ani->GetPlaySpeed());
				//
				SetPropInt(m_cursorCenterX, ani->GetCenterPos().x);
				SetPropInt(m_cursorCenterY, ani->GetCenterPos().y);
				SetPropInt(m_cursorUVMapW, ani->GetUVMapW());
				SetPropInt(m_cursorUVMapH, ani->GetUVMapH());
				SetPropInt(m_cursorUVMapBegin, ani->GetUVMapBegin());
				SetPropInt(m_cursorUVMapCount, ani->GetUVMapCount());
				//
				m_cursorPtr->SetActiveCursor(ani);
				//
				m_cursorProp.EnableWindow(TRUE);
				m_cursorProp.EnableRender(true);
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
VOID		GCViewCursorEditor::CloseFile(){
	for(int i=0;i<g_copyCursors.size();i++)
	{
		if(g_copyCursors[i].editor == this)
		{
			g_copyCursors.erase(g_copyCursors.begin() + i);
			i--;
			break;
		}
	}
	safe_release(m_cursorPtr);
}
BOOL		GCViewCursorEditor::OpenFile(const wchar_t* szFile)	//打开文件,如果失败则会自动删除
{
	//
	this->m_strFile	=	szFile;
	if(!__super::OpenFile(szFile))
		return false;
	if(this->m_viewerProps.m_hWnd == 0)
		this->m_treeView.InsertItem(Language(L"CursorViewer", L"光标编辑器"), 25, 25);
	if(m_strFile.empty())
	{
		m_cursorPtr		=	CreateCursorManager();
		this->SetModifyed(true);
	}
	else
	{
		m_cursorPtr		=	LoadCursorManager(m_packagePtr, A(m_strFile.c_str()));
		if(!m_cursorPtr)
			return false;
	}
	//
	RebuildTrees();
	if(this->m_viewerProps.m_hWnd == 0){
	InitViewerProps();
	//
	InitCursorProp();
	}
	return true;
}

VOID		GCViewCursorEditor::RebuildTrees()
{
	if(m_hViewItem)
		m_treeView.DeleteItem(m_hViewItem);
	wchar_t wFileName[256], wExt[256];
	_wsplitpath(m_strFile.c_str(), 0, 0, wFileName, wExt);
	wcscat(wFileName, wExt);
	if(!wFileName[0])
		GetNewFileName(wFileName);
	m_hViewItem = m_treeView.InsertItem(wFileName, 30, 30);
	m_treeView.SetItemState(m_hViewItem, TVIS_BOLD, TVIS_BOLD);
	m_treeView.Expand(m_hViewItem, TVE_EXPAND);
	//
	this->ClearTreeItem();
	BuildAll();
	//
	m_treeView.m_callback	=	this;
}

VOID		GCViewCursorEditor::BuildAll()
{
	m_treeView.SelectItem(m_hViewItem);
	DeleteChildItems(m_hViewItem);
	m_treeView.SetItemData(m_hViewItem, AddTreeItem(CURSOR_MAIN));
	//
	RefreshAllCursors();
}

VOID		GCViewCursorEditor::RefreshAllCursors()
{
	m_treeView.SelectItem(m_hViewItem);
	DeleteChildItems(m_hViewItem);
	for(int i=0;i<m_cursorPtr->GetCursorCount();i++)
	{
		ICursorRender* e		=	m_cursorPtr->GetCursorPtr(i);
		HTREEITEM hCursor		=	m_treeView.InsertItem(W(e->GetCursorName()), 31, 31, m_hViewItem);
		m_treeView.SetItemData(hCursor, AddTreeItem(CURSOR_CURSOR, e->GetID()));
	}
}

VOID		GCViewCursorEditor::OnActive()		//被激活
{
	SetCursorManager(m_cursorPtr);
	__super::OnActive();
}

VOID		GCViewCursorEditor::OnDeActive()		//失去激活
{
	__super::OnDeActive();
}

void		GCViewCursorEditor::OnFrameMove(const RenderParameter& r)	//每次更新
{
	if(m_cursorPtr && m_cursorPtr->GetActiveCursor())
	{
		m_cursorPtr->GetActiveCursor()->OnFrameMove(r.fElapsedTime);
	}
}

void		GCViewCursorEditor::OnPushRenderObjects(const RenderParameter& r)	//渲染
{
}

BOOL		GCViewCursorEditor::SaveFile(BOOL bSaveAs)	//保存文件
{
	bool bNew	=	false;
	if(bSaveAs || m_strFile.size() == 0)
	{
		bNew	=	true;
		CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, formatw(L"%s|*.rpgCur||", Language(L"CursorFileType", L"光标文件") ), GetMainFrame());
		wchar_t buffer[1024];
		dlg.m_pOFN->lpstrInitialDir	=	AToW(GetProjectPath(), buffer, 1024);
		if(dlg.DoModal() != IDOK)
			return false;
		SetProjectPath();
		wchar_t szPath[256], szFile[256], szDir[256];
		_wsplitpath(dlg.GetPathName(), szPath, szDir, szFile, 0);
		wcscat(szPath, szDir);
		wcscat(szPath, szFile);
		wcscat(szPath, L".rpgCur");
		m_strFile	=	szPath;
	}
	if(m_cursorPtr || bSaveAs)
		m_cursorPtr->SaveCursors(A(m_strFile.c_str()));
	return __super::SaveFile(bSaveAs);
}

VOID		GCViewCursorEditor::GetNewFileName(wchar_t* szFile)
{
	wcscpy(szFile, Language(L"新Cursor"));
}

void		GCViewCursorEditor::OnRenderingEnd(const RenderParameter& r)
{
	if(m_cursorPtr && m_cursorPtr->GetActiveCursor())
	{
		PPixel pos	=	m_cursorPtr->GetActiveCursor()->GetCenterPos();
		m_cursorPtr->GetActiveCursor()->Render(&PPixel(pos.x, pos.y));
		GetDlgResManager()->DrawCircle(pos, 3.0f, 0xffff0000, 12, true);
	}
}


//渲染
void		GCViewCursorEditor::OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI)
{
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			selectTypes[hItem]	=	type;
		}
	}
	switch(nMenuID)
	{
	case ID_CURSOR_INSERTCURSOR:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == CURSOR_MAIN);
		}
		break;
	case ID_CURSOR_DELETECURSOR:
		{
			pCmdUI->Enable(false);
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == CURSOR_CURSOR)
				{
					pCmdUI->Enable(true);
					break;
				}
			}
		}
		break;
	default:
		pCmdUI->Enable(false);
		break;
	}
}


void		GCViewCursorEditor::OnCommandUI(INT nMenuID)
{
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			selectTypes[hItem]	=	type;
		}
	}
	switch(nMenuID)
	{
	case ID_CURSOR_INSERTCURSOR:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(type && type->type == CURSOR_MAIN)
			{
				this->m_cursorPtr->AddCursor(0);
				this->RefreshAllCursors();
				this->OnChangeTreeSelect(&m_treeView, NULL);
				this->SetModifyed();
			}
		}
		break;
	case ID_CURSOR_DELETECURSOR:
		{
			if(MessageBox(this->GetViewHandle(), Language(L"是否真的要删除?"), Language(L"提示"), MB_YESNO) == IDYES)
			{
				for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
				{
					TreeItemType* type	=	it->second;
					if(!type)
						continue;
					if(type->type == CURSOR_CURSOR)
					{
						ICursorRender* cur	=	m_cursorPtr->GetCursorByID(type->p1);
						this->m_treeView.DeleteItem(it->first);
						m_cursorPtr->DelCursor(cur);
					}
				}
				m_treeView.ClearSelect();
				this->OnChangeTreeSelect(&m_treeView, NULL);
				this->SetModifyed();
			}
		}
		break;
	}
}

VOID		GCViewCursorEditor::InitCursorProp()
{
	InitProp(m_cursorProp);
	InitProp(m_managerProp);
	//
	CMFCPropertyGridProperty* group1			= new CMFCPropertyGridProperty(Language(L"基本属性"));
	group1->AddSubItem(m_cursorName				= AddPropText(Language(L"光标名称"), Language(L"Cursor_Name", L"设置光标的名称。")));
	group1->AddSubItem(m_cursorFileSet			= AddPropFile(Language(L"设置图片"), Language(L"Cursor_TextureSet", L"设置光标贴图文件。"), GetImageFilter(), true));
	group1->AddSubItem(m_cursorFileClear		= AddPropButton(Language(L"清空图片"), Language(L"Cursor_TextureClear", L"清空光标贴图文件。")));
	group1->AddSubItem(m_cursorIsDefault		= AddPropBool(Language(L"是否默认光标"), Language(L"Cursor_IsDefault", L"设置当前光标是否作为默认光标。")));
	//
	CMFCPropertyGridProperty* groupUV			= new CMFCPropertyGridProperty(Language(L"图片属性"));
	groupUV->AddSubItem(m_cursorUVMapW			= AddPropInt(Language(L"UV横向数量"), Language(L"Cursor_UVMapW", L"光标图片UV横向数量。"), 0, 1000));
	groupUV->AddSubItem(m_cursorUVMapH			= AddPropInt(Language(L"UV纵向数量"), Language(L"Cursor_UVMapH", L"光标图片UV纵向数量。"), 0, 1000));
	groupUV->AddSubItem(m_cursorUVMapBegin		= AddPropInt(Language(L"UV开始索引"), Language(L"Cursor_UVMapBegin", L"光标图片UV开始索引。"), 0, 10000));
	groupUV->AddSubItem(m_cursorUVMapCount		= AddPropInt(Language(L"UV播放数量"), Language(L"Cursor_UVMapCount", L"光标图片UV播放数量。"), 0, 10000));
	groupUV->AddSubItem(m_cursorPlaySpeed		= AddPropFloat(Language(L"播放速度"), Language(L"Cursor_PlaySpeed", L"光标序列播放速度。"), false, 0.001f, 10000.0f));
	//
	CMFCPropertyGridProperty* groupM			= new CMFCPropertyGridProperty(Language(L"指针属性"));
	groupM->AddSubItem(m_cursorCenterX			= AddPropInt(Language(L"中心点X"), Language(L"Cursor_CenterX", L"设置图片中指针位置X方向。"), 0, 8192));
	groupM->AddSubItem(m_cursorCenterY			= AddPropInt(Language(L"中心点Y"), Language(L"Cursor_CenterY", L"设置图片中指针位置Y方向。"), 0, 8192));
	//
	this->m_cursorProp.AddProperty(group1);
	this->m_cursorProp.AddProperty(groupUV);
	this->m_cursorProp.AddProperty(groupM);
	//
	CMFCPropertyGridProperty* groupManager		= new CMFCPropertyGridProperty(Language(L"基本属性"));
	groupManager->AddSubItem(m_managerHardware	= AddPropBool(Language(L"是否硬件指针"), Language(L"m_managerHardware", L"设置光标是否硬件指针。")));
	m_managerProp.AddProperty(groupManager);
}

void		GCViewCursorEditor::OnTreeViewKeyDown(INT nChar)
{
	if(nChar == VK_DELETE)
	{
		this->OnDeleteTree();
	}
}

BOOL		GCViewCursorEditor::IsCopyTree()
{
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type && type->type == this->CURSOR_CURSOR)
			return true;
	}
	return false;
}

BOOL		GCViewCursorEditor::IsPasteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(IsCanPasteCursor(type->type))
				return true;
		}
	}
	return false;
}

VOID		GCViewCursorEditor::OnDeleteTree()
{
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			selectTypes[hItem]	=	type;
		}
	}
	if(MessageBox(this->GetViewHandle(), Language(L"是否真的要删除?"), Language(L"提示"), MB_YESNO) == IDYES)
	{
		for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
		{
			TreeItemType* type	=	it->second;
			if(!type)
				continue;
			if(type->type == CURSOR_CURSOR)
			{
				ICursorRender* cur	=	m_cursorPtr->GetCursorByID(type->p1);
				this->m_treeView.DeleteItem(it->first);
				m_cursorPtr->DelCursor(cur);
			}
		}
		m_treeView.ClearSelect();
		this->OnChangeTreeSelect(&m_treeView, NULL);
		this->SetModifyed();
	}
}

BOOL		GCViewCursorEditor::IsDeleteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type && type->type == CURSOR_CURSOR)
			return true;
	}
	return false;
}

VOID				GCViewCursorEditor::OnCopyTree()
{
	DefineAuth_6();
	if(!CheckAuth_6(0))
		return;
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			selectTypes[hItem]	=	type;
		}
	}
	std::vector<TreeItemType*> types;
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == this->CURSOR_CURSOR)
			types.push_back(type);
	}
	SetCursorCopy(this, &types);
}

VOID				GCViewCursorEditor::OnPasteTree()
{
	DefineAuth_6();
	if(!CheckAuth_6(0))
	{
		ShowMainMessage(L"免费版：不支持拷贝粘贴.");
		return;
	}
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			selectTypes[hItem]	=	type;
		}
	}
	bool bPaste	=	false;
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(IsCanPasteCursor(type->type))
		{
			bPaste	=	true;
			break;
		}
	}
	if(!bPaste)
		return;
	try{
		for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
		{
			TreeItemType* type	=	it->second;
			if(!type)
				continue;
			switch(type->type)
			{
			case CURSOR_CURSOR:
				{
					for(int j=0;j<g_copyCursors.size();j++)
					{
						if(g_copyCursors[j].type.type != CURSOR_CURSOR)
							continue;
						ICursorRender* from	=	g_copyCursors[j].editor->m_cursorPtr->GetCursorByID(g_copyCursors[j].type.p1);
						ICursorRender* to	=	m_cursorPtr->GetCursorByID(type->p1);
						if(from && to)
							to->CloneFrom(from);
					}
					this->RefreshAllCursors();
					this->OnChangeTreeSelect(&m_treeView, NULL);
					this->SetModifyed();
				}
				break;
			}
		}
	}
	catch(...)
	{
		assert(false);
		g_copyCursors.clear();
	}
}
