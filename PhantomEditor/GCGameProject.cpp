#include "StdAfx.h"
#include "resource.h"
#include "GCGameProject.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"
#include "SetElementTextureDlg.h"
#include "ChildFrm.h"
#include "phantom-gc.h"
#include "phantom-gcView.h"
#include "GCViewManager.h"

extern	GCGameProject*	g_gameProject;

HICON		GCGameProject::GetViewerIcon()
{
	static HICON	hIcon	=	AfxGetApp()->LoadIcon(IDI_GAMEPROJ_ICON);
	return hIcon;
}

VOID		GCGameProject::SetGameRename(const wchar_t* szName)
{
	m_gameDirector.SetName(szName);
	//
	//FileNameW oldFile	=	this->m_strFile.c_str();
	//FileNameW newFile	=	this->m_gamePath;
	//newFile				+=	szName;
	//newFile				+=	L".rpgProj";
	//::MoveFileW(oldFile, newFile);
	//m_strFile			=	newFile;
	//
	this->SetModifyed();
	this->UpdateMainTitle();
	this->m_treeView.SetItemText(this->m_hViewItem, szName);
}


int CALLBACK	OnSortFileCB(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	GCGameProject* proj		=	(GCGameProject*)lParamSort;
	//
	TreeItemType* pItem1	=	(TreeItemType*)lParam1;
	TreeItemType* pItem2	=	(TreeItemType*)lParam2;
	return wcsicmp(pItem1->path, pItem2->path);
}

VOID		GCGameProject::RebuildSubDir(const wchar_t* szDir, HTREEITEM hDirectory)
{
	//
	this->DeleteChildItems(hDirectory);
	//
	wchar_t szNewPath[512];
	wcscpy(szNewPath, m_gamePath);
	wcscat(szNewPath, szDir);
	wchar_t ch	=	szNewPath[wcslen(szNewPath) - 1];
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
			if(win32.cFileName[0]!='.' && !(win32.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				if((win32.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
				{
					wcscpy(szNewPath, szDir);
					wcscat(szNewPath, win32.cFileName);
					wcscat(szNewPath, L"\\");
					HTREEITEM hItem	=	AddFileItem(win32.cFileName, hDirectory, szNewPath, FALSE);
					RebuildSubDir(szNewPath, hItem);
				}
				else
				{
					wcscpy(szNewPath, szDir);
					wcscat(szNewPath, win32.cFileName);
					wchar_t szExt[128];
					_wsplitpath(szNewPath, 0, 0, 0, szExt);
					//
					if(wcsicmp(szExt, L".rpgproj") == 0)
					{
					}
					else
						AddFileItem(win32.cFileName, hDirectory, szNewPath, TRUE);
				}
			}
			if(!::FindNextFile(hFile, &win32))
				break;
		}
	}
	FindClose(hFile);
	//
	//TVSORTCB cb;
	//cb.hParent		=	hDirectory;
	//cb.lParam		=	(LPARAM)this;
	//cb.lpfnCompare	=	OnSortFileCB;
	//m_treeView.SortChildrenCB(&cb);
}

INT			GetFileIcon(const wchar_t* szFile)
{
	wchar_t szExt[128];
	_wsplitpath(szFile, 0, 0, 0, szExt);
	if(wcsicmp(szExt, L".rpgmesh") == 0 || wcsicmp(szExt, L".geoms") == 0|| wcsicmp(szExt, L".pakmesh") == 0)
		return 1;
	else if(wcsicmp(szExt, L".rpg2D") == 0 || wcsicmp(szExt, L".z_rpg2D") == 0)
		return 11;
	else if(wcsicmp(szExt, L".rpg2Ds") == 0 || wcsicmp(szExt, L".z_rpg2Ds") == 0)
		return 10;
	else if(wcsicmp(szExt, L".rpgscene") == 0 || wcsicmp(szExt, L".z_rpg3Ds") == 0 || wcsicmp(szExt, L".rpg2ds") == 0)
		return 15;
	else if(wcsicmp(szExt, L".dds") == 0 || wcsicmp(szExt, L".tga") == 0 || wcsicmp(szExt, L".png") == 0 || wcsicmp(szExt, L".jpg") == 0 || wcsicmp(szExt, L".bmp") == 0)
		return 16;
	else if(wcsicmp(szExt, L".wav") == 0)
		return 17;
	else if(wcsicmp(szExt, L".ogg") == 0 || wcsicmp(szExt, L".mid") == 0 || wcsicmp(szExt, L".mpg") == 0)
		return 18;
	else if(wcsicmp(szExt, L".txt") == 0 || wcsicmp(szExt, L".z_rpgLogic") == 0)
		return 23;
	else if(wcsicmp(szExt, L".csv") == 0)
		return 21;
	else if(wcsicmp(szExt, L".rpgui") == 0 || wcsicmp(szExt, L".zui") == 0 || wcsicmp(szExt, L".pakui") == 0)
		return 24;
	else if(wcsicmp(szExt, L".rpgCur") == 0 || wcsicmp(szExt, L".cursors") == 0 || wcsicmp(szExt, L".z_rpgCur") == 0)
		return 25;
	return 26;
}

const wchar_t*			GetFileExt(const wchar_t* szFile)
{
	static wchar_t szExt[128];
	_wsplitpath(szFile, 0, 0, 0, szExt);
	if(wcsicmp(szExt, L".rpgmesh") == 0 || wcsicmp(szExt, L".geoms") == 0)
		return L".rpgmesh";
	else if(wcsicmp(szExt, L".rpgui") == 0 || wcsicmp(szExt, L".zui") == 0)
		return L".rpgui";
	else if(wcsicmp(szExt, L".rpgCur") == 0 || wcsicmp(szExt, L".cursors") == 0)
		return L".rpgCur";
	return szExt;
}

HTREEITEM	GCGameProject::AddFileItem(const wchar_t* szFileName, HTREEITEM hParent, const wchar_t* szDir, BOOL bIsFile)
{
	INT nIcon		=	22;
	INT nSelIcon	=	20;
	if(bIsFile)
	{
		nIcon		=	GetFileIcon(szDir);
		nSelIcon	=	nIcon;
	}
	HTREEITEM hItem	=	m_treeView.InsertItem(szFileName, nIcon, TreeImageNone, hParent);
	m_treeView.SetItemData(hItem, AddTreeItem(bIsFile ? GAMEPROJ_FILE : GAMEPROJ_FOLDER, 0, 0, 0, 0, 0, szDir));
	return hItem;
}

VOID		GCGameProject::GetNewFileName(wchar_t* szFile)
{
	wcscpy(szFile, Language(L"新项目工程"));
}

VOID		GCGameProject::ChangeGameSize()
{
	SetRenderWindowSize(m_gameDirector.GetScreenSize().cx, m_gameDirector.GetScreenSize().cy);
	GetViewManager()->RefreshRenderSize();
	this->GetActiveView()->Invalidate();
}

BOOL		GCGameProject::OpenFile(const wchar_t* szFile)	//打开文件,如果失败则会自动删除
{
	CloseFile();
	if(g_gameProject)
	{
		GetMainFrame()->MessageBox(Language(L"一次只能打开一个项目进行编辑"), Language(L"提示"));
		return false;
	}
	this->m_strFile	=	szFile;
	if(!__super::OpenFile(szFile))
		return false;
	m_treeView.SetFrameBitmap(IDB_PLOT_FRAMES, 8);
	//if(m_luaScript.m_hWnd == 0){
		this->m_treeView.InsertItem(Language(L"GameProjectViewer", L"项目工程编辑器"), 0, TreeImageNone);
	//}
	if(!m_gameDirector.Load(m_packagePtr, A(m_strFile.c_str())))
		return false;
	//
	//m_luaScript.m_gamePtr	=	this;
	m_gameDirector.SetDebugMode(TRUE);
	assert(GetActiveView() != NULL);
	//if(m_luaScript.m_hWnd == 0){
	m_callPlot.Create(m_callPlot.IDD, GetPropertiesDlgWindowPtr());
	//}
	m_callPlot.ShowWindow(SW_HIDE);
	m_callPlot.m_gamePtr			=	this;
	//if(m_luaScript.m_hWnd==0){
	m_dlgVariant.Create(m_dlgVariant.IDD, GetPropertiesDlgWindowPtr());
	//}
	m_dlgVariant.ShowWindow(SW_HIDE);
	m_dlgVariant.m_gamePtr			=	this;
	//
	//
	g_gameProject	=	this;
	wchar_t szDrive[256], szDir[256];
	_wsplitpath(szFile, szDrive, szDir, 0, 0);
	wcscat(szDrive, szDir);
	m_gamePath		=	szDrive;
	SetProjectPath(A(szDrive));
	//
	m_treeView.SetFrameTextColor(Frame_Chapter, RGB(255, 255, 255));
	m_treeView.SetFrameTextColor(Frame_Action, RGB(255, 255, 0));
	m_treeView.SetFrameTextColor(Frame_Event, RGB(0, 55, 255));
	m_treeView.SetFrameTextColor(Frame_Drama, RGB(0, 55, 155));
	m_treeView.SetFrameTextColor(Frame_Game, RGB(255, 122, 15));
	//m_treeView.SetFrameTextColor(Frame_Chapter, RGB(62, 122, 172));
	//m_treeView.SetFrameTextColor(Frame_Module, RGB(62, 122, 172));
	//m_treeView.SetFrameTextColor(Frame_Drama, RGB(178, 205, 125));
	//m_treeView.SetFrameTextColor(Frame_PlotCall, RGB(100, 100, 50));
	//m_treeView.SetFrameTextColor(Frame_Event, RGB(73, 161, 158));
	//m_treeView.SetFrameTextColor(Frame_Element, RGB(73, 161, 158));
	//m_treeView.SetFrameTextColor(Frame_Action, RGB(73, 161, 158));
	//
	//if(m_luaScript.m_hWnd == 0){
	//this->AddPropertieWindow(&this->m_luaScript);
	this->AddPropertieWindow(&this->m_callPlot);
	InitProp(m_gameProp);
	InitProp(m_callVarWnd);
	InitProp(m_callPropWnd);
	InitProp(m_variantProp);
	//
	InitProp(m_chapterProp);
	InitProp(m_dramaProp);
	InitProp(m_actionProp);
	InitProp(m_stateProp);
	InitProp(m_eventProp);
	m_eventProp.SetDescriptionRows(5);
	m_stateProp.SetDescriptionRows(5);
	//
	//m_luaScript.Create(m_luaScript.IDD, GetPropertiesDlgWindowPtr());//GetPropertiesWindow());
	//m_luaScript.ShowWindow(SW_HIDE);
	//
	InitViewerProps();
	InitGameProp();
	InitCallVariantSet();
	InitCallProperties();
	InitVariants();
	//
	InitChapterProp();
	InitDramaProp();
	InitActionProp();
	InitStateProp();
	InitEventProp();
	//}
	SetSelectNone();
	ChangeGameSize();
	RebuildTrees();
	return true;
}

VOID		GCGameProject::RebuildTrees()
{
	if(m_hViewItem)
		m_treeView.DeleteItem(m_hViewItem);
	m_hViewItem = m_treeView.InsertItem(m_gameDirector.GetName(), 19, 5);
	m_treeView.SetItemState(m_hViewItem, TVIS_BOLD, TVIS_BOLD);
	m_treeView.Expand(m_hViewItem, TVE_EXPAND);
	//
	this->ClearTreeItem();
	BuildAll();
	//
	m_treeView.m_callback	=	this;
}

VOID		GCGameProject::BuildAll()
{
	m_treeView.SelectItem(m_hViewItem);
	DeleteChildItems(m_hViewItem);
	//
	m_treeView.SetItemData(m_hViewItem, AddTreeItem(GAMEPROJ_MAIN));
	//
	HTREEITEM hDirector	=	m_treeView.InsertItem(Language(L"工程目录结构"), 22, TreeImageNone, m_hViewItem);
	m_treeView.SetItemData(hDirector, AddTreeItem(GAMEPROJ_MAIN, PlotType_Director));
	RebuildSubDir(L"", hDirector);
	//
	m_hViewChapters	=	m_treeView.InsertItem(Language(L"剧情系统"), 5, TreeImageNone, m_hViewItem);
	m_treeView.SetItemData(m_hViewChapters, AddTreeItem(GAMEPROJ_CHAPTER_LIST));
	RefreshChapters();
}

VOID		GCGameProject::RefreshProcess(CPlotProcess* process, HTREEITEM hItem)
{
	//
	if(process->GetEventCallCount() > 0)
	{
		HTREEITEM hItemEventList	=	m_treeView.InsertItem(Language(L"事件列表"), 4, TreeImageNone, hItem);//7, 3, hItem);
		m_treeView.SetItemData(hItemEventList, AddTreeItem(GAMEPROJ_EVENT_LIST, process->GetPlotType(), process->GetID()));
		RefreshEvents(process, hItemEventList);
	}
	//
	if(process->GetElementCount() > 0)
	{
		HTREEITEM hItemElementList	=	m_treeView.InsertItem(Language(L"对象列表"), 4, TreeImageNone, hItem);//4, 7, hItem);
		m_treeView.SetItemData(hItemElementList, AddTreeItem(GAMEPROJ_ELEMENT_LIST, process->GetPlotType(), process->GetID()));
		RefreshElements(process, hItemElementList);
	}
	//
	if(process->GetActionCount() > 0)
	{
		HTREEITEM hItemActionList	=	m_treeView.InsertItem(Language(L"动作列表"), 4, TreeImageNone, hItem);//8, 4, hItem);
		m_treeView.SetItemData(hItemActionList, AddTreeItem(GAMEPROJ_ACTION_LIST, process->GetPlotType(), process->GetID()));
		RefreshActions(process, hItemActionList);
	}
	//
	if(process->GetStateCount() > 0)
	{
		HTREEITEM hItemStateList	=	m_treeView.InsertItem(Language(L"调用列表"), 4, TreeImageNone, hItem);//8, 4, hItem);
		m_treeView.SetItemData(hItemStateList, AddTreeItem(GAMEPROJ_STATE_LIST, process->GetPlotType(), process->GetID()));
		RefreshStates(process, hItemStateList);
	}
}

const wchar_t*		GetPlotName(PlotType type, CPlotSystemBase* ch, wchar_t* wbufRet)
{
	static wchar_t staticBuf[256];
	wchar_t wbuf[256];
	//
	wchar_t szParent[256];
	szParent[0] = 0;
	//if(ch->GetParentPtr())
	//{
	//	switch(ch->GetPlotType())
	//	{
	//	case PlotType_Director:
	//	case PlotType_Chapter:
	//	case PlotType_Drama:
	//	case PlotType_Action:
	//		break;
	//	default:
	//		{
	//			GetPlotName(ch->GetParentPtr()->GetPlotType(), ch->GetParentPtr(), szParent);
	//		}
	//		break;
	//	}
	//}
	//
	if(!wbufRet)
		wbufRet	=	staticBuf;
	wbufRet[0]	=	0;
	//
	wcscpy(wbuf, ch->GetName());
	switch(type)
	{
	case PlotType_Chapter:
		{
			CChapter* chapter	=	static_cast<CChapter*>(ch);
			int index	=	0;
			for(int i=0;i<chapter->GetProjectPtr()->GetChapterCount();i++)
			{
				if(chapter->GetProjectPtr()->GetChapterPtr(i) == ch)
				{
					index	=	i;
					break;
				}
			}
			wchar_t buf[128];
			GetCNNumber(buf, index + 1);
			if(IsChiness())
				swprintf(wbuf, Language(L"第%s章："), buf);
			else
				wcscpy(wbuf, Language(L"章节："));
			wcscat(wbuf, chapter->GetName());
		}
		break;
	case PlotType_Drama:
		{
			CDrama* drama	=	static_cast<CDrama*>(ch);
			int index	=	0;
			for(int i=0;i<drama->GetChapterPtr()->GetDramaCount();i++)
			{
				if(drama->GetChapterPtr()->GetDramaPtr(i) == ch)
				{
					index	=	i;
					break;
				}
			}
			wchar_t buf[128];
			GetCNNumber(buf, index + 1);
			if(IsChiness())
				swprintf(wbuf, Language(L"第%s篇："), buf);
			else
				wcscpy(wbuf, Language(L"篇章："));
			wcscat(wbuf, drama->GetName());
		}
		break;
	case PlotType_Module:
		{
			CModule* module	=	static_cast<CModule*>(ch);
			wcscpy(wbuf, Language(L"模块："));
			wcscat(wbuf, module->GetName());
		}
		break;
	case PlotType_Action:
		{
			CPlotAction* action	=	static_cast<CPlotAction*>(ch);
			wcscpy(wbuf, Language(L"动作："));
			wcscat(wbuf, action->GetName());
		}
		break;
	}
	if(szParent[0])
		swprintf(wbufRet, L"%s.%s", szParent, wbuf);
	else
		wcscpy(wbufRet, wbuf);
	return wbufRet;
}

const wchar_t*		GetPlotName(CPlotPropertiesMap* ch, wchar_t* wbuf)
{
	return GetPlotName(ch->GetPlotType(), ch, wbuf);
}

VOID		GCGameProject::RefreshChapters()
{
	m_treeView.SelectItem(m_hViewChapters);
	DeleteChildItems(m_hViewChapters);
	//
	RefreshProcess(&m_gameDirector, m_hViewChapters);
	//
	for(int i=0;i<m_gameDirector.GetChapterCount();i++)
	{
		CChapter*	chapter	=	m_gameDirector.GetChapterPtr(i);
		HTREEITEM hItem	=	m_treeView.InsertItem(GetPlotName(chapter), 12, 0, m_hViewChapters);
		m_treeView.SetItemData(hItem, AddTreeItem(GAMEPROJ_CHAPTER, chapter->GetID()));
		RefreshChapter(chapter, hItem);
	}
	//
	for(int i=0;i<m_gameDirector.GetModuleCount();i++)
	{
		CModule*	module	=	m_gameDirector.GetModulePtr(i);
		HTREEITEM hItem	=	m_treeView.InsertItem(GetPlotName(module), 3, 2, m_hViewChapters);
		m_treeView.SetItemData(hItem, AddTreeItem(GAMEPROJ_MODULE, module->GetID()));
		RefreshChapter(module, hItem);
	}
}

VOID		GCGameProject::RefreshChapter(CChapter* chapter, HTREEITEM hItem)
{
	DeleteChildItems(hItem);
	//
	RefreshProcess(chapter, hItem);
	//
	for(int i=0;i<chapter->GetDramaCount();i++)
	{
		CDrama* drama	=	chapter->GetDramaPtr(i);
		HTREEITEM hItem2	=	m_treeView.InsertItem(GetPlotName(drama), 14, 1, hItem);
		m_treeView.SetItemData(hItem2, AddTreeItem(GAMEPROJ_DRAMA, drama->GetID()));
		RefreshDrama(drama, hItem2);
	}
}

VOID		GCGameProject::RefreshDrama(CDrama* drama, HTREEITEM hItem)
{
	DeleteChildItems(hItem);
	RefreshProcess(drama, hItem);
	//RefreshElements(drama, hItemElementList);
	//RefreshActions(drama, hItem, FALSE);
	//RefreshEvents(drama, hItemEventList, FALSE);
}

VOID		GCGameProject::RefreshElements(CPlotProcess* process, HTREEITEM hItem)
{
	if(process->GetElementCount() == 0)
	{
		GetTreeView()->RemoveSelect(hItem);
		GetTreeView()->DeleteItem(hItem);
		return;
	}
	DeleteChildItems(hItem);
	for(int i=0;i<process->GetElementCount();i++)
	{
		CPlotElementBase* element	=	process->GetElementPtr(i);
		int nIcon	=	9;
		switch(element->GetPlotType())
		{
		case PlotType_ElementUI:
			nIcon	=	24;
			break;
		case PlotType_Element2DScene:
			nIcon	=	26;
			break;
		case PlotType_ElementMusic:
			nIcon	=	18;
			break;
		case PlotType_Element3DScene:
			nIcon	=	15;
			break;
		case PlotType_Element3DMesh:
			nIcon	=	1;
			break;
		case PlotType_ElementData:
			nIcon	=	23;
			break;
		case PlotType_ElementCursor:
			nIcon	=	25;
			break;
		case PlotType_ElementCSV:
			nIcon	=	21;
			break;
		}
		HTREEITEM hItem2	=	m_treeView.InsertItem(element->GetName(), nIcon, 7, hItem);
		m_treeView.SetItemData(hItem2, AddTreeItem(GAMEPROJ_ELEMENT, process->GetID(), element->GetID()));
		RefreshElement(element, hItem2);
	}
}

VOID		GCGameProject::RefreshElement(CPlotElementBase* element, HTREEITEM hItem)
{
	DeleteChildItems(hItem);
	CDynamicArray<PlotObjectContext> rets;
	element->EnumObjectContexts(rets);
	int nIcon	=	30;
	//switch(element->GetPlotType())
	//{
	//case PlotType_ElementUI:
	//	nIcon	=	24;
	//	break;
	//case PlotType_Element2DScene:
	//	nIcon	=	26;
	//	break;
	//case PlotType_ElementMusic:
	//	nIcon	=	18;
	//	break;
	//case PlotType_Element3DScene:
	//	nIcon	=	15;
	//	break;
	//case PlotType_Element3DMesh:
	//	nIcon	=	1;
	//	break;
	//case PlotType_ElementData:
	//	nIcon	=	23;
	//	break;
	//case PlotType_ElementCursor:
	//	nIcon	=	25;
	//	break;
	//case PlotType_ElementCSV:
	//	nIcon	=	21;
	//	break;
	//}
	for(int i=0;i<rets.size();i++)
	{
		HTREEITEM hItem2	=	m_treeView.InsertItem(rets[i].name, nIcon, TreeImageNone, hItem);
		m_treeView.SetItemData(hItem2, AddTreeItem(GAMEPROJ_OBJECT, element->GetProcessPtr()->GetID(), element->GetID()));
	}
}

VOID		GCGameProject::RefreshActions(CPlotProcess* process, HTREEITEM hItem, BOOL bDeleteChilds)
{
	if(process->GetActionCount() == 0)
	{
		GetTreeView()->RemoveSelect(hItem);
		GetTreeView()->DeleteItem(hItem);
		return;
	}
	if(bDeleteChilds)
		DeleteChildItems(hItem);
	for(int i=0;i<process->GetActionCount();i++)
	{
		CPlotAction* action	=	process->GetActionPtr(i);
		HTREEITEM hItem2	=	m_treeView.InsertItem(GetPlotName(action), 9, 4, hItem);
		m_treeView.SetItemData(hItem2, AddTreeItem(GAMEPROJ_ACTION, process->GetID(), action->GetID()));
		RefreshAction(action, hItem2);
	}
}

VOID		GCGameProject::RefreshEvents(CPlotPropertiesMap* process, HTREEITEM hItem, BOOL bDeleteChilds)
{
	if(process->GetEventCallCount() == 0 && hItem)
	{
		GetTreeView()->RemoveSelect(hItem);
		GetTreeView()->DeleteItem(hItem);
		return;
	}
	if(!hItem)
	{
		hItem	=	FindTreeItem(this->m_hViewChapters, GAMEPROJ_EVENT_LIST, process->GetPlotType(), process->GetID());
		if(!hItem)
			return;
	}
	if(bDeleteChilds)
		DeleteChildItems(hItem);
	//
	std::map<int, int > eventMap;
	for(int i=0;i<process->GetEventCallCount();i++)
	{
		CPlotEventCall* pEvent	=	process->GetEventCallPtr(i);
		//wchar_t wbuf[128];
		//swprintf(wbuf, L"%s:%s", pEvent->GetName(), GetEventName(pEvent->GetEventID()));
		HTREEITEM hItem2	=	m_treeView.InsertItem(GetEventName(pEvent->GetEventID()), 6, TreeImageNone, hItem);
		m_treeView.SetItemData(hItem2, AddTreeItem(GAMEPROJ_EVENT, process->GetID(), pEvent->GetID(), pEvent->GetEventID()));
	}
}
//
//VOID		GCGameProject::RefreshEvent(CPlotPropertiesMap* process, HTREEITEM hItem, INT nEventID)
//{
//	DeleteChildItems(hItem);
//	for(int i=0;i<process->GetEventCallCount();i++)
//	{
//		CPlotEventCall* pEvent	=	process->GetEventCallPtr(i);
//		if(pEvent->GetEventID().eventID == nEventID)
//		{
//			HTREEITEM hItem2	=	m_treeView.InsertItem(pEvent->GetName(), 10, 10, hItem);
//			m_treeView.SetItemData(hItem2, AddTreeItem(GAMEPROJ_EVENT_STATE, process->GetID(), pEvent->GetID()));
//		}
//	}
//}

VOID		GCGameProject::RefreshAction(CPlotAction* action, HTREEITEM hItem)
{
	DeleteChildItems(hItem);
	if(action->GetEventCallCount() > 0)
	{
		HTREEITEM hItemEventList	=	m_treeView.InsertItem(Language(L"事件列表"), 4, TreeImageNone, hItem);//7, 3, hItem);
		m_treeView.SetItemData(hItemEventList, AddTreeItem(GAMEPROJ_EVENT_LIST, action->GetPlotType(), action->GetID()));
		RefreshEvents(action, hItemEventList);
	}
	if(action->GetStateCount() > 0)
	{
		HTREEITEM hItemStateList	=	m_treeView.InsertItem(Language(L"调用列表"), 4, TreeImageNone, hItem);//8, 4, hItem);
		m_treeView.SetItemData(hItemStateList, AddTreeItem(GAMEPROJ_STATE_LIST, action->GetPlotType(), action->GetID()));
		RefreshStates(action, hItemStateList);
	}
}

VOID		GCGameProject::RefreshStates(CPlotAction* action, HTREEITEM hItem, BOOL bDeleteChilds)
{
	if(action->GetStateCount() == 0 && hItem)
	{
		GetTreeView()->RemoveSelect(hItem);
		GetTreeView()->DeleteItem(hItem);
		return;
	}
	DeleteChildItems(hItem);
	for(int i=0;i<action->GetStateCount();i++)
	{
		CPlotState* state	=	action->GetStatePtr(i);
		HTREEITEM hItem2	=	m_treeView.InsertItem(state->GetName(), 10, TreeImageNone, hItem);
		m_treeView.SetItemData(hItem2, AddTreeItem(GAMEPROJ_STATE, action->GetID(), state->GetID()));
	}
}

VOID		GCGameProject::OnActive()		//被激活
{
	__super::OnActive();
}

VOID		GCGameProject::OnDeActive()		//失去激活
{
	safe_delete(this->m_projectRun);
	__super::OnDeActive();
}

BOOL		GCGameProject::SaveFile(BOOL bSaveAs)	//保存文件
{
	bool bNew	=	false;
	if(bSaveAs || m_strFile.size() == 0)
	{
		bNew	=	true;
		CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, formatw(L"%s|*.rpgProj||", Language(L"ProjectFileType", L"项目文件") ), GetMainFrame());
		wchar_t buffer[1024];
		dlg.m_pOFN->lpstrInitialDir	=	AToW(GetProjectPath(), buffer, 1024);
		if(dlg.DoModal() != IDOK)
			return false;
		SetProjectPath();
		wchar_t szPath[256], szFile[256], szDir[256];
		_wsplitpath(dlg.GetPathName(), szPath, szDir, szFile, 0);
		wcscat(szPath, szDir);
		wcscat(szPath, szFile);
		wcscat(szPath, L".rpgProj");
		m_strFile	=	szPath;
		wcscat(szFile, L".rpgProj");
		this->m_treeView.SetItemText(this->m_hViewItem, szFile);
	}
	if(IsModifyed() || bSaveAs)
		m_gameDirector.Save(A(this->m_strFile.c_str()));
	return __super::SaveFile(bSaveAs);
}


BOOL			GetFileInParentFolder(char* szPath, const char* szSourceFile, const char* szTextureFile)
{
	FILE* f = fopen(szTextureFile, "rb");
	if(f)
	{
		strcpy(szPath, szTextureFile);
		fclose(f);
		return true;
	}
	char szSourceDrive[256], szSourceDir[256], szSD[256];
	_splitpath(szSourceFile, szSourceDrive, szSourceDir, 0, 0);
	strcat(szSourceDrive, szSourceDir);
	strcpy(szSD, szSourceDrive);
	//
	char szFileName[128], szExt[128];
	_splitpath(szTextureFile, 0, 0, szFileName, szExt);
	strcat(szFileName, szExt);
	//
	strcpy(szSourceDrive, szSD);
	strcat(szSourceDrive, szFileName);
	//
	f	=	fopen(szSourceDrive, "rb");
	if(f)
	{
		fclose(f);
		strcpy(szPath, szSourceDrive);
		return true;
	}
	strcpy(szSourceDrive, szSD);
	strcat(szSourceDrive, szTextureFile);
	//
	f	=	fopen(szSourceDrive, "rb");
	if(f)
	{
		fclose(f);
		strcpy(szPath, szSourceDrive);
		return true;
	}
	int len	=	strlen(szSD);
	if(len > 5)
	{
		szSD[len - 1] = 0;
		//
		if(GetFileInParentFolder(szPath, szSD, szTextureFile))
		{
			return true;
		}
	}
	
	return false;
}

VOID			CopyImportFile(const char* sourceFile, const char* szFile, const char* szDir)
{
	char szSourceDrive[256], szSourceDir[256];
	_splitpath(sourceFile, szSourceDrive, szSourceDir, 0, 0);
	strcat(szSourceDrive, szSourceDir);
	//
	char szFileName[128], szExt[128];
	_splitpath(szFile, 0, 0, szFileName, szExt);
	//
	char szDExt[128];
	strcpy(szDExt, szExt);
	if(stricmp(szDExt, ".geoms") == 0)
		strcpy(szDExt, ".rpgmesh");
	else if(stricmp(szDExt, ".zui") == 0)
		strcpy(szDExt, ".rpgui");
	else if(stricmp(szDExt, ".cursors") == 0)
		strcpy(szDExt, ".rpgCur");
	char szDestPath[256];
	strcpy(szDestPath, szDir);
	strcat(szDestPath, szFileName);
	strcat(szDestPath, szDExt);
	//
	strcat(szFileName, szExt);

	if(GetFileInParentFolder(szSourceDrive, sourceFile, szFile))
	{
		CopyFileA(szSourceDrive, szDestPath, FALSE);
	}
}

BOOL			GCGameProject::ImportFile(const char* szFile)//ImportFileType type
{
	CDynamicArray<ZipFileName> files;
	::ImportFile(files, szFile);
	const char* szPath	=	GetProjectFileDir(szFile);
	Text	t	=	A(m_gamePath);
	t			+=	szPath;
	for(int i=0;i<files.size();i++)
		CopyImportFile(szFile, files[i].t, t);
	return true;
}

VOID				GCGameProject::BuildFileViewText()
{
	TextW	t;
	t	=	m_gameDirector.GetName();
	if(IsModifyed())
		t	+=	L"*";
	this->m_strFileView	=	t;
	if(m_childFrame)
	{
		m_childFrame->SendMessage(WM_SETTEXT, 0, (LPARAM)t.str());
	}
}

void				GCGameProject::OnDoubleClick(CMTreeCtrl* treePtr, HTREEITEM hItem)	//更新选择树形控件
{
	TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
	if(!type)
		return;
	if(type->type == this->GAMEPROJ_FILE)
	{
		AfxGetApp()->OpenDocumentFile(type->path);
	}
	else if(type->type == GAMEPROJ_ELEMENT)
	{
		CPlotProcess* process	=	m_gameDirector.SearchProcess(type->p1);
		if(process)
		{
			CPlotElementBase* base	=	process->GetElementByID(type->p2);
			if(base)
			{
				AfxGetApp()->OpenDocumentFile(W(base->GetFileName()));
			}
		}
	}
}

VOID				GCGameProject::SetGameRuntime(BOOL bRuntime)
{
	if(bRuntime)
	{
		if(!m_projectRun)
		{
			GetMainFrame()->m_wndOutput.ClearMessage();
			m_projectRun	=	new CProjectRun(&m_gameDirector, 0);
		}
	}
	else
	{
		safe_delete(m_projectRun);
	}
}

BOOL				GCGameProject::IsGameRuntime()
{
	return (m_projectRun != NULL);
}

VOID				GCGameProject::EnumAllEvents(CDynamicArray<PlotEventName>& rets)
{
}

//IBigScene3D*		GCGameProject::GetStateScene()
//{
//	if(m_projectRun)
//		return m_projectRun->GetScenePtr();
//	return 0;
//}

VOID		GCGameProject::PlayAnimation()
{
	this->SetGameRuntime(true);
}
VOID		GCGameProject::StopAnimation()
{
	this->SetGameRuntime(false);
}
VOID		GCGameProject::PauseAnimation(BOOL bPause)
{
}
VOID		GCGameProject::SetAnimationLoop(BOOL bLoop)
{
}
BOOL		GCGameProject::IsAnimationCanPause()
{
	return false;
}
BOOL		GCGameProject::IsAnimationCanPlayStop()
{
	return true;
}
BOOL		GCGameProject::IsAnimationCanLoop()
{
	return false;
}
BOOL		GCGameProject::IsAnimationPlaying()
{
	return this->IsGameRuntime();
}
BOOL		GCGameProject::IsAnimationPause()
{
	return false;
}
BOOL		GCGameProject::IsAnimationLoop()
{
	return false;
}
