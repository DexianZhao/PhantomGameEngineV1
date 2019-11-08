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
#include "DlgPlotEventSel.h"

class GCGameProject;

struct	Game2DCopyData
{
	TreeItemType		type;
	GCGameProject*	editor;
	Game2DCopyData()
	{
		editor	=	0;
	}
};

std::vector<Game2DCopyData>		g_copyGame2Ds;

VOID		SetGame2DCopy(GCGameProject* e, std::vector<TreeItemType*>* vs)
{
	g_copyGame2Ds.clear();
	for(int i=0;i<vs->size();i++)
	{
		Game2DCopyData ud;
		ud.editor	=	e;
		ud.type		=	*vs->at(i);
		g_copyGame2Ds.push_back(ud);
	}
}

BOOL		IsCanPasteGame2D(int nType)
{
	for(int i=0;i<g_copyGame2Ds.size();i++)
	{
		if(g_copyGame2Ds[i].type.type == nType)
		{
			return true;
		}
	}
	return false;
}

GCGameProject*	g_gameProject	=	0;

GCGameProject::GCGameProject(CPhantomGCDoc* docPtr)
	:GCViewBase(docPtr, Language(L"项目工程"), IDB_GAME_VIEWER)
{
	m_downPoint.SetPoint(0, 0);
	m_bLButtonDown	=	false;
	m_projectRun	=	0;
}

extern	VOID	SetExecutePath();

VOID	GCGameProject::CloseFile(){
	if(m_projectRun)
		delete m_projectRun;
	m_projectRun = 0;
	if(g_gameProject == this)
		g_gameProject	=	0;
	SetExecutePath();
	for(int i=0;i<g_copyGame2Ds.size();i++)
	{
		if(g_copyGame2Ds[i].editor == this)
		{
			g_copyGame2Ds.erase(g_copyGame2Ds.begin() + i);
			i--;
			break;
		}
	}
}
GCGameProject::~GCGameProject(void)
{
	CloseFile();
}

//渲染
void		GCGameProject::OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI)
{
	bool bExistFolder		=	false;
	bool bExistChapterList		=	false;
	bool bExistDrama		=	false;
	bool bExistChapter		=	false;
	bool bExistActionList	=	false;
	bool bExistElementList	=	false;
	bool bExistAction		=	false;
	bool bExistState		=	false;
	bool bExistStateList	=	false;
	bool bExistModule		=	false;
	bool bExistEventList	=	false;
	bool bExistEvent		=	false;
	bool bExistGame			=	false;
	bool bExistElement		=	false;
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			selectTypes[hItem]	=	type;
			if(type->type == GAMEPROJ_FOLDER)
				bExistFolder	=	true;
			else if(type->type == GAMEPROJ_MAIN)
				bExistGame	=	true;
			else if(type->type == GAMEPROJ_CHAPTER_LIST)
				bExistChapterList	=	true;
			else if(type->type == GAMEPROJ_ACTION_LIST)
				bExistActionList	=	true;
			else if(type->type == GAMEPROJ_ELEMENT_LIST)
				bExistElementList	=	true;
			else if(type->type == GAMEPROJ_ELEMENT)
				bExistElement		=	true;
			else if(type->type == GAMEPROJ_EVENT_LIST)
				bExistEventList	=	true;
			else if(type->type == GAMEPROJ_CHAPTER)
				bExistChapter	=	true;
			else if(type->type == GAMEPROJ_MODULE)
				bExistModule	=	true;
			else if(type->type == GAMEPROJ_DRAMA)
				bExistDrama	=	true;
			else if(type->type == GAMEPROJ_ACTION)
				bExistAction				=	true;
			else if(type->type == GAMEPROJ_EVENT)
				bExistEvent		=	true;
			else if(type->type == GAMEPROJ_STATE_LIST)
				bExistStateList				=	true;
			else if(type->type == GAMEPROJ_STATE)
				bExistState				=	true;
		}
	}
	switch(nMenuID)
	{
	case ID_PROJECT_IMPORTFILE:
		{
			pCmdUI->Enable(bExistFolder);
		}
		break;
	case ID_PROJECT_REFRESHFILES:
		{
			pCmdUI->Enable(bExistFolder);
		}
		break;
	case ID_PLOT_INSERTCHAPTER:
		pCmdUI->Enable(bExistChapterList);
		break;
	case ID_PLOT_INSERTDRAMA:
		pCmdUI->Enable(bExistChapter || bExistModule);
		break;
	case ID_PLOT_INSERTMODULE:
		pCmdUI->Enable(bExistChapterList);
		break;
	case ID_PLOT_INSERTACTION:
		pCmdUI->Enable(bExistDrama || bExistActionList || bExistChapter || bExistModule || bExistGame);
		break;
	case ID_PLOT_INSERTELEMENT:
		pCmdUI->Enable(bExistElementList || bExistDrama || bExistChapter || bExistModule || bExistGame);
		break;
	case ID_PLOTACTION_INSERTCALL:
		pCmdUI->Enable(bExistAction || bExistDrama || bExistChapter || bExistModule || bExistGame || bExistStateList);
		break;
	case ID_PLOTRIGHT_REFRESH_ELEMENT:
		pCmdUI->Enable(bExistElement);
		break;
	case ID_PLOTRIGHT_INSERTEVENT:
		pCmdUI->Enable(bExistEventList || bExistDrama || bExistChapter || bExistModule || bExistGame);
		break;
	default:
		pCmdUI->Enable(false);
		break;
	}
}

void		GCGameProject::OnCommandUI(INT nMenuID)
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
	case ID_PROJECT_IMPORTFILE:
		{
			CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT, GetPhantomFilters(), CWnd::FromHandle(this->GetViewHandle()));
			W init(GetProjectPath());
			dlg.m_pOFN->lpstrInitialDir	=	init;
			if(dlg.DoModal() != IDOK)
				return;
			SetProjectPath();
			this->m_treeView.ClearSelect();
			POSITION pos	=	dlg.GetStartPosition();
			while(pos)
			{
				ImportFile(A((const wchar_t*)dlg.GetNextPathName(pos)));
			}
			BuildAll();
		}
		break;
	case ID_PROJECT_REFRESHFILES:
		{
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin(); it != selectTypes.end(); it++)
			{
				if(it->second->type == this->GAMEPROJ_FOLDER)
					this->RebuildSubDir(it->second->path, it->first);
			}
		}
		break;
	case ID_PLOT_INSERTCHAPTER:
		{
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin(); it != selectTypes.end(); it++)
			{
				if(it->second->type == GAMEPROJ_CHAPTER_LIST)
				{
					if(m_gameDirector.GetChapterCount() >= MAX_FREE_CHAPTER_COUNT)
					{
						unsigned char text[] = {0xcf,0x7d,0x52,0x96,0x46,0xff,0xbe,0xff,0x08,0xce,0xab,0xd0,0x2f,0x28,0xca,0xd6,0x19,0xec,0x23,0x91,0x1f,0x8e,0x5d,0xb2,0x07,0x41,0x95,0x21,0xb5,0x00,0x29,0x06,0xc0,0x72,0x59,0x94,0xc3,0xa1,0xf9,0x94,0x1c,0xa3,0xd2};
						unsigned char password[] = {0x14,0x87,0xfd,0x5f,0xee,0x01,0x17,0x59,0xc6,0x06,0x10,0x64,0xe5,0x8b,0x6a,0x38,0xbf,0x13,0xfc,0x60,0xc8,0x23,0x88,0x19,0xc8,0xb7,0x3b,0xd9,0x7b,0xa3,0xf5,0xc2};
						wchar_t ret[512];
						GetCryptText(ret, text, sizeof(text), password, sizeof(password));
						ShowMainMessage(ret);
						break;
					}
					this->m_gameDirector.AddChapter();
					this->RefreshChapters();
					this->SetModifyed();
					break;
				}
			}
		}
		break;
	case ID_PLOT_INSERTDRAMA:
		{
			std::map<CChapter*, HTREEITEM> inserts;
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin(); it != selectTypes.end(); it++)
			{
				if(it->second->type == GAMEPROJ_CHAPTER)// || it->second->type == GAMEPROJ_MODULE)
				{
					CChapter* chapter	=	m_gameDirector.GetChapterPtrByID(it->second->p1);
					if(chapter)
					{
						if(chapter->GetDramaCount() >= MAX_FREE_DRAMA_COUNT)
						{
							unsigned char text[] = {0x32,0xe1,0xbf,0xeb,0xa1,0x5e,0x5f,0x08,0x84,0xc0,0x71,0x27,0x4a,0x88,0x71,0x33,0x74,0xda,0xd7,0xe8,0xea,0xf4,0x33,0x3a,0x62,0x4a,0xf4,0x7f,0x71,0xc7,0xf6,0x03,0x4c,0xd9,0xdf,0xd4,0xa7,0x58,0x38,0x0d,0x96,0xcc,0x1e,0x66,0x24,0xe0,0xfa,0x7d,0x3c,0xfb,0xba,0xd4,0xe7};
							unsigned char password[] = {0xe9,0x1b,0x10,0x22,0x09,0xa0,0xf6,0xae,0x4a,0x08,0xca,0x93,0x80,0x2b,0xd1,0xdd,0xd2,0x25,0x08,0x19,0x3d,0x59,0xe6,0x91,0xcf,0x96,0x2f,0xd8,0xd1,0x29,0x3b,0xd9};
							wchar_t ret[512];
							GetCryptText(ret, text, sizeof(text), password, sizeof(password));
							ShowMainMessage(ret);
							break;
						}
						chapter->AddDrama();
						inserts[chapter]	=	it->first;
					}
				}
				else if(it->second->type == GAMEPROJ_MODULE)
				{
					CModule* chapter	=	m_gameDirector.GetModulePtrByID(it->second->p1);
					if(chapter)
					{
						if(chapter->GetDramaCount() >= MAX_FREE_DRAMA_COUNT)
						{
							unsigned char text[] = {0x32,0xe1,0xbf,0xeb,0xa1,0x5e,0x5f,0x08,0x84,0xc0,0x71,0x27,0x4a,0x88,0x71,0x33,0x74,0xda,0xd7,0xe8,0xea,0xf4,0x33,0x3a,0x62,0x4a,0xf4,0x7f,0x71,0xc7,0xf6,0x03,0x4c,0xd9,0xdf,0xd4,0xa7,0x58,0x38,0x0d,0x96,0xcc,0x1e,0x66,0x24,0xe0,0xfa,0x7d,0x3c,0xfb,0xba,0xd4,0xe7};
							unsigned char password[] = {0xe9,0x1b,0x10,0x22,0x09,0xa0,0xf6,0xae,0x4a,0x08,0xca,0x93,0x80,0x2b,0xd1,0xdd,0xd2,0x25,0x08,0x19,0x3d,0x59,0xe6,0x91,0xcf,0x96,0x2f,0xd8,0xd1,0x29,0x3b,0xd9};
							wchar_t ret[512];
							GetCryptText(ret, text, sizeof(text), password, sizeof(password));
							ShowMainMessage(ret);
							break;
						}
						chapter->AddDrama();
						inserts[chapter]	=	it->first;
					}
				}
			}
			for(std::map<CChapter*, HTREEITEM>::iterator it = inserts.begin(); it != inserts.end(); it++)
			{
				this->RefreshChapter(it->first, it->second);
			}
			if(inserts.size() > 0)
				SetModifyed();
		}
		break;
	case ID_PLOT_INSERTMODULE:
		{
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin(); it != selectTypes.end(); it++)
			{
				if(it->second->type == GAMEPROJ_CHAPTER_LIST)
				{
					this->m_gameDirector.AddModule();
					this->RefreshChapters();
					this->SetModifyed();
					break;
				}
			}
		}
		break;
	case ID_PLOTRIGHT_REFRESH_ELEMENT:
		{
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin(); it != selectTypes.end(); it++)
			{
				if(it->second->type == GAMEPROJ_ELEMENT)
				{
					CPlotProcess* process	=	m_gameDirector.SearchProcess(it->second->p1);
					if(process)
					{
						CPlotElementBase* base	=	process->GetElementByID(it->second->p2);
						if(base)
						{
							base->RefreshParameters();
							this->RefreshElement(base, it->first);
							this->SetModifyed();
						}
					}
				}
			}
		}
		break;
	case ID_PLOT_INSERTACTION:
		{
			std::map<CPlotProcess*, HTREEITEM> inserts;
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin(); it != selectTypes.end(); it++)
			{
				CPlotProcess* proc		=	0;
				if(it->second->type == GAMEPROJ_ACTION_LIST)
				{
					if(it->second->p1 == PlotType_Chapter || it->second->p1 == PlotType_Module)
					{
						proc	=	m_gameDirector.SearchProcess(it->second->p2);
						if(proc->GetActionCount() >= MAX_FREE_PLOT_ACTION)
						{
							unsigned char text[] = {0x60,0xf4,0x8f,0x02,0x00,0xe7,0x34,0xbc,0xcd,0xc1,0x03,0xbe,0xd8,0x9c,0x4c,0x67,0xdd,0x38,0x07,0x67,0x4b,0xc4,0xa0,0x11,0x80,0x11,0xdc,0x8d,0x62,0xdd,0x5f,0x2c,0x1e,0xcc,0x82,0x1e,0x76,0xab,0x50,0xc0,0xcc,0xff,0x16,0xf2,0xdc,0x9c,0x30,0x4d,0xaf,0x32,0x7c,0x5d,0xb1,0xc9,0x9b,0x14,0x9a,0x02,0xe8};
							unsigned char password[] = {0xbb,0x0e,0x20,0xcb,0xa8,0x19,0x9d,0x1a,0x03,0x09,0xb8,0x0a,0x12,0x3f,0xec,0x89,0x7b,0xc7,0xd8,0x96,0x9c,0x69,0x75,0xba,0x2d,0xcd,0x07,0x2a,0xc2,0x33,0x92,0xf6};
							wchar_t ret[512];
							GetCryptText(ret, text, sizeof(text), password, sizeof(password));
							ShowMainMessage(ret);
							break;
						}
						inserts[proc]	=	it->first;
						proc->AddAction();
						this->RefreshActions(proc, it->first, true);
					}
					else if(it->second->p1 == PlotType_Director)
					{
						proc	=	&m_gameDirector;
						if(proc->GetActionCount() >= MAX_FREE_PLOT_ACTION)
						{
							unsigned char text[] = {0x60,0xf4,0x8f,0x02,0x00,0xe7,0x34,0xbc,0xcd,0xc1,0x03,0xbe,0xd8,0x9c,0x4c,0x67,0xdd,0x38,0x07,0x67,0x4b,0xc4,0xa0,0x11,0x80,0x11,0xdc,0x8d,0x62,0xdd,0x5f,0x2c,0x1e,0xcc,0x82,0x1e,0x76,0xab,0x50,0xc0,0xcc,0xff,0x16,0xf2,0xdc,0x9c,0x30,0x4d,0xaf,0x32,0x7c,0x5d,0xb1,0xc9,0x9b,0x14,0x9a,0x02,0xe8};
							unsigned char password[] = {0xbb,0x0e,0x20,0xcb,0xa8,0x19,0x9d,0x1a,0x03,0x09,0xb8,0x0a,0x12,0x3f,0xec,0x89,0x7b,0xc7,0xd8,0x96,0x9c,0x69,0x75,0xba,0x2d,0xcd,0x07,0x2a,0xc2,0x33,0x92,0xf6};
							wchar_t ret[512];
							GetCryptText(ret, text, sizeof(text), password, sizeof(password));
							ShowMainMessage(ret);
							break;
						}
						inserts[proc]	=	it->first;
						proc->AddAction();
						this->RefreshActions(proc, it->first, true);
					}
					else if(it->second->p1 == PlotType_Drama)
					{
						proc	=	m_gameDirector.SearchProcess(it->second->p2);
						if(proc->GetActionCount() >= MAX_FREE_PLOT_ACTION)
						{
							unsigned char text[] = {0x60,0xf4,0x8f,0x02,0x00,0xe7,0x34,0xbc,0xcd,0xc1,0x03,0xbe,0xd8,0x9c,0x4c,0x67,0xdd,0x38,0x07,0x67,0x4b,0xc4,0xa0,0x11,0x80,0x11,0xdc,0x8d,0x62,0xdd,0x5f,0x2c,0x1e,0xcc,0x82,0x1e,0x76,0xab,0x50,0xc0,0xcc,0xff,0x16,0xf2,0xdc,0x9c,0x30,0x4d,0xaf,0x32,0x7c,0x5d,0xb1,0xc9,0x9b,0x14,0x9a,0x02,0xe8};
							unsigned char password[] = {0xbb,0x0e,0x20,0xcb,0xa8,0x19,0x9d,0x1a,0x03,0x09,0xb8,0x0a,0x12,0x3f,0xec,0x89,0x7b,0xc7,0xd8,0x96,0x9c,0x69,0x75,0xba,0x2d,0xcd,0x07,0x2a,0xc2,0x33,0x92,0xf6};
							wchar_t ret[512];
							GetCryptText(ret, text, sizeof(text), password, sizeof(password));
							ShowMainMessage(ret);
							break;
						}
						inserts[proc]	=	it->first;
						proc->AddAction();
						RefreshActions(static_cast<CDrama*>(proc), it->first);
					}
				}
				else if(it->second->type == GAMEPROJ_MAIN)
				{
					if(m_gameDirector.GetActionCount() >= MAX_FREE_PLOT_ACTION)
					{
						unsigned char text[] = {0x60,0xf4,0x8f,0x02,0x00,0xe7,0x34,0xbc,0xcd,0xc1,0x03,0xbe,0xd8,0x9c,0x4c,0x67,0xdd,0x38,0x07,0x67,0x4b,0xc4,0xa0,0x11,0x80,0x11,0xdc,0x8d,0x62,0xdd,0x5f,0x2c,0x1e,0xcc,0x82,0x1e,0x76,0xab,0x50,0xc0,0xcc,0xff,0x16,0xf2,0xdc,0x9c,0x30,0x4d,0xaf,0x32,0x7c,0x5d,0xb1,0xc9,0x9b,0x14,0x9a,0x02,0xe8};
						unsigned char password[] = {0xbb,0x0e,0x20,0xcb,0xa8,0x19,0x9d,0x1a,0x03,0x09,0xb8,0x0a,0x12,0x3f,0xec,0x89,0x7b,0xc7,0xd8,0x96,0x9c,0x69,0x75,0xba,0x2d,0xcd,0x07,0x2a,0xc2,0x33,0x92,0xf6};
						wchar_t ret[512];
						GetCryptText(ret, text, sizeof(text), password, sizeof(password));
						ShowMainMessage(ret);
						break;
					}
					m_gameDirector.AddAction();
					this->RefreshChapters();
					this->SetModifyed();
				}
				else if(it->second->type == GAMEPROJ_CHAPTER)
				{
					CChapter* chapter	=	m_gameDirector.GetChapterPtrByID(it->second->p1);
					if(chapter->GetActionCount() >= MAX_FREE_PLOT_ACTION)
					{
						unsigned char text[] = {0x60,0xf4,0x8f,0x02,0x00,0xe7,0x34,0xbc,0xcd,0xc1,0x03,0xbe,0xd8,0x9c,0x4c,0x67,0xdd,0x38,0x07,0x67,0x4b,0xc4,0xa0,0x11,0x80,0x11,0xdc,0x8d,0x62,0xdd,0x5f,0x2c,0x1e,0xcc,0x82,0x1e,0x76,0xab,0x50,0xc0,0xcc,0xff,0x16,0xf2,0xdc,0x9c,0x30,0x4d,0xaf,0x32,0x7c,0x5d,0xb1,0xc9,0x9b,0x14,0x9a,0x02,0xe8};
						unsigned char password[] = {0xbb,0x0e,0x20,0xcb,0xa8,0x19,0x9d,0x1a,0x03,0x09,0xb8,0x0a,0x12,0x3f,0xec,0x89,0x7b,0xc7,0xd8,0x96,0x9c,0x69,0x75,0xba,0x2d,0xcd,0x07,0x2a,0xc2,0x33,0x92,0xf6};
						wchar_t ret[512];
						GetCryptText(ret, text, sizeof(text), password, sizeof(password));
						ShowMainMessage(ret);
						break;
					}
					chapter->AddAction();
					this->RefreshChapter(chapter, it->first);
					this->SetModifyed();
				}
				else if(it->second->type == GAMEPROJ_MODULE)
				{
					CModule* chapter	=	m_gameDirector.GetModulePtrByID(it->second->p1);
					chapter->AddAction();
					this->RefreshChapter(chapter, it->first);
					this->SetModifyed();
				}
				else if(it->second->type == GAMEPROJ_DRAMA)
				{
					CDrama* drama	=	m_gameDirector.SearchDrama(it->second->p1);
					if(drama->GetActionCount() >= MAX_FREE_PLOT_ACTION)
					{
						unsigned char text[] = {0x60,0xf4,0x8f,0x02,0x00,0xe7,0x34,0xbc,0xcd,0xc1,0x03,0xbe,0xd8,0x9c,0x4c,0x67,0xdd,0x38,0x07,0x67,0x4b,0xc4,0xa0,0x11,0x80,0x11,0xdc,0x8d,0x62,0xdd,0x5f,0x2c,0x1e,0xcc,0x82,0x1e,0x76,0xab,0x50,0xc0,0xcc,0xff,0x16,0xf2,0xdc,0x9c,0x30,0x4d,0xaf,0x32,0x7c,0x5d,0xb1,0xc9,0x9b,0x14,0x9a,0x02,0xe8};
						unsigned char password[] = {0xbb,0x0e,0x20,0xcb,0xa8,0x19,0x9d,0x1a,0x03,0x09,0xb8,0x0a,0x12,0x3f,0xec,0x89,0x7b,0xc7,0xd8,0x96,0x9c,0x69,0x75,0xba,0x2d,0xcd,0x07,0x2a,0xc2,0x33,0x92,0xf6};
						wchar_t ret[512];
						GetCryptText(ret, text, sizeof(text), password, sizeof(password));
						ShowMainMessage(ret);
						break;
					}
					drama->AddAction();
					this->RefreshDrama(drama, it->first);
					this->SetModifyed();
				}
			}
			for(std::map<CPlotProcess*, HTREEITEM>::iterator it = inserts.begin(); it != inserts.end(); it++)
			{
				
			}
			if(inserts.size() > 0)
				SetModifyed();
		}
		break;
	case ID_PLOT_INSERTELEMENT:
		{
			CFileDialog dlg(true, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT, GetPhantomFilters(), this->GetActiveView());
			W init(GetProjectPath());
			dlg.m_pOFN->lpstrInitialDir	=	init;
			if(dlg.DoModal() != IDOK)
				return;
			SetProjectPath();
			std::vector<std::wstring> files;
			POSITION pos	=	dlg.GetStartPosition();
			while(pos)
			{
				std::wstring ret;
				this->GetSortFile(ret, (const wchar_t*)dlg.GetNextPathName(pos));
				files.push_back(ret.c_str());
			}
			//
			//std::map<CPlotProcess*, HTREEITEM> inserts;
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin(); it != selectTypes.end(); it++)
			{
				CPlotProcess* proc		=	0;
				if(it->second->type == GAMEPROJ_ELEMENT_LIST)
				{
					if(it->second->p1 == PlotType_Chapter || it->second->p1 == PlotType_Module)
					{
						proc	=	m_gameDirector.SearchProcess(it->second->p2);
					}
					else if(it->second->p1 == PlotType_Director)
					{
						proc	=	&m_gameDirector;
					}
					else if(it->second->p1 == PlotType_Drama)
					{
						proc	=	m_gameDirector.SearchProcess(it->second->p2);
					}
					if(proc)
					{
						for(int i=0;i<files.size();i++)
						{
							if(proc->GetElementCount() >= MAX_FREE_PLOT_OBJECT)
							{
								unsigned char text[] = {0xdb,0xb9,0x04,0x27,0x88,0x64,0x52,0x0c,0x8b,0x9a,0x89,0xec,0x19,0x4b,0x27,0x1e,0xbe,0xf5,0xfc,0xb8,0x80,0x77,0x5a,0x8f,0x7e,0x1c,0x34,0xcd,0xde,0x7a,0x5d,0x39,0xa5,0x81,0x09,0x3b,0xfe,0x28,0x36,0x70,0x8a,0xa4,0x9c,0xa0,0x1d,0x4b,0x5b,0x34,0xcc,0xff,0x87,0x82,0x7c,0x7a,0x61,0xf2,0x0f,0x64,0x09,0xc4,0xb2,0x43,0x7b};
								unsigned char password[] = {0x00,0x43,0xab,0xee,0x20,0x9a,0xfb,0xaa,0x45,0x52,0x32,0x58,0xd3,0xe8,0x87,0xf0,0x18,0x0a,0x23,0x49,0x57,0xda,0x8f,0x24,0xd3,0xc0,0xef,0x6a,0x7e,0x94,0x90,0xe3};
								wchar_t ret[512];
								GetCryptText(ret, text, sizeof(text), password, sizeof(password));
								ShowMainMessage(ret);
								break;
							}
							CPlotElementBase* base	=	proc->AddElement(A(files[i].c_str()));
							if(base)
								base->RefreshParameters();
						}
						this->RefreshElements(proc, it->first);
						SetModifyed();
					}
				}
				else if(it->second->type == GAMEPROJ_MAIN)
				{
					for(int i=0;i<files.size();i++)
					{
						if(m_gameDirector.GetElementCount() >= MAX_FREE_PLOT_OBJECT)
						{
							unsigned char text[] = {0xdb,0xb9,0x04,0x27,0x88,0x64,0x52,0x0c,0x8b,0x9a,0x89,0xec,0x19,0x4b,0x27,0x1e,0xbe,0xf5,0xfc,0xb8,0x80,0x77,0x5a,0x8f,0x7e,0x1c,0x34,0xcd,0xde,0x7a,0x5d,0x39,0xa5,0x81,0x09,0x3b,0xfe,0x28,0x36,0x70,0x8a,0xa4,0x9c,0xa0,0x1d,0x4b,0x5b,0x34,0xcc,0xff,0x87,0x82,0x7c,0x7a,0x61,0xf2,0x0f,0x64,0x09,0xc4,0xb2,0x43,0x7b};
							unsigned char password[] = {0x00,0x43,0xab,0xee,0x20,0x9a,0xfb,0xaa,0x45,0x52,0x32,0x58,0xd3,0xe8,0x87,0xf0,0x18,0x0a,0x23,0x49,0x57,0xda,0x8f,0x24,0xd3,0xc0,0xef,0x6a,0x7e,0x94,0x90,0xe3};
							wchar_t ret[512];
							GetCryptText(ret, text, sizeof(text), password, sizeof(password));
							ShowMainMessage(ret);
							break;
						}
						CPlotElementBase* base	=	m_gameDirector.AddElement(A(files[i].c_str()));
						if(base)
							base->RefreshParameters();
					}
					this->RefreshChapters();
					SetModifyed();
				}
				else if(it->second->type == GAMEPROJ_CHAPTER)
				{
					CChapter* chapter	=	m_gameDirector.GetChapterPtrByID(it->second->p1);
					for(int i=0;i<files.size();i++)
					{
						if(chapter->GetElementCount() >= MAX_FREE_PLOT_OBJECT)
						{
							unsigned char text[] = {0xdb,0xb9,0x04,0x27,0x88,0x64,0x52,0x0c,0x8b,0x9a,0x89,0xec,0x19,0x4b,0x27,0x1e,0xbe,0xf5,0xfc,0xb8,0x80,0x77,0x5a,0x8f,0x7e,0x1c,0x34,0xcd,0xde,0x7a,0x5d,0x39,0xa5,0x81,0x09,0x3b,0xfe,0x28,0x36,0x70,0x8a,0xa4,0x9c,0xa0,0x1d,0x4b,0x5b,0x34,0xcc,0xff,0x87,0x82,0x7c,0x7a,0x61,0xf2,0x0f,0x64,0x09,0xc4,0xb2,0x43,0x7b};
							unsigned char password[] = {0x00,0x43,0xab,0xee,0x20,0x9a,0xfb,0xaa,0x45,0x52,0x32,0x58,0xd3,0xe8,0x87,0xf0,0x18,0x0a,0x23,0x49,0x57,0xda,0x8f,0x24,0xd3,0xc0,0xef,0x6a,0x7e,0x94,0x90,0xe3};
							wchar_t ret[512];
							GetCryptText(ret, text, sizeof(text), password, sizeof(password));
							ShowMainMessage(ret);
							break;
						}
						CPlotElementBase* base = chapter->AddElement(A(files[i].c_str()));
						if(base)
							base->RefreshParameters();
					}
					this->RefreshChapter(chapter, it->first);
					this->SetModifyed();
				}
				else if(it->second->type == GAMEPROJ_MODULE)
				{
					CModule* chapter	=	m_gameDirector.GetModulePtrByID(it->second->p1);
					for(int i=0;i<files.size();i++)
					{
						if(chapter->GetElementCount() >= MAX_FREE_PLOT_OBJECT)
						{
							unsigned char text[] = {0xdb,0xb9,0x04,0x27,0x88,0x64,0x52,0x0c,0x8b,0x9a,0x89,0xec,0x19,0x4b,0x27,0x1e,0xbe,0xf5,0xfc,0xb8,0x80,0x77,0x5a,0x8f,0x7e,0x1c,0x34,0xcd,0xde,0x7a,0x5d,0x39,0xa5,0x81,0x09,0x3b,0xfe,0x28,0x36,0x70,0x8a,0xa4,0x9c,0xa0,0x1d,0x4b,0x5b,0x34,0xcc,0xff,0x87,0x82,0x7c,0x7a,0x61,0xf2,0x0f,0x64,0x09,0xc4,0xb2,0x43,0x7b};
							unsigned char password[] = {0x00,0x43,0xab,0xee,0x20,0x9a,0xfb,0xaa,0x45,0x52,0x32,0x58,0xd3,0xe8,0x87,0xf0,0x18,0x0a,0x23,0x49,0x57,0xda,0x8f,0x24,0xd3,0xc0,0xef,0x6a,0x7e,0x94,0x90,0xe3};
							wchar_t ret[512];
							GetCryptText(ret, text, sizeof(text), password, sizeof(password));
							ShowMainMessage(ret);
							break;
						}
						CPlotElementBase* base = chapter->AddElement(A(files[i].c_str()));
						if(base)
							base->RefreshParameters();
					}
					this->RefreshChapter(chapter, it->first);
					this->SetModifyed();
				}
				else if(it->second->type == GAMEPROJ_DRAMA)
				{
					CDrama* drama	=	m_gameDirector.SearchDrama(it->second->p1);
					for(int i=0;i<files.size();i++)
					{
						if(drama->GetElementCount() >= MAX_FREE_PLOT_OBJECT)
						{
							unsigned char text[] = {0xdb,0xb9,0x04,0x27,0x88,0x64,0x52,0x0c,0x8b,0x9a,0x89,0xec,0x19,0x4b,0x27,0x1e,0xbe,0xf5,0xfc,0xb8,0x80,0x77,0x5a,0x8f,0x7e,0x1c,0x34,0xcd,0xde,0x7a,0x5d,0x39,0xa5,0x81,0x09,0x3b,0xfe,0x28,0x36,0x70,0x8a,0xa4,0x9c,0xa0,0x1d,0x4b,0x5b,0x34,0xcc,0xff,0x87,0x82,0x7c,0x7a,0x61,0xf2,0x0f,0x64,0x09,0xc4,0xb2,0x43,0x7b};
							unsigned char password[] = {0x00,0x43,0xab,0xee,0x20,0x9a,0xfb,0xaa,0x45,0x52,0x32,0x58,0xd3,0xe8,0x87,0xf0,0x18,0x0a,0x23,0x49,0x57,0xda,0x8f,0x24,0xd3,0xc0,0xef,0x6a,0x7e,0x94,0x90,0xe3};
							wchar_t ret[512];
							GetCryptText(ret, text, sizeof(text), password, sizeof(password));
							ShowMainMessage(ret);
							break;
						}
						CPlotElementBase* base = drama->AddElement(A(files[i].c_str()));
						if(base)
							base->RefreshParameters();
					}
					this->RefreshDrama(drama, it->first);
					this->SetModifyed();
				}
			}
		}
		break;
	case ID_PLOTACTION_INSERTCALL:
		{
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin(); it != selectTypes.end(); it++)
			{
				if(it->second->type == GAMEPROJ_STATE_LIST)
				{
					CPlotAction* action	=	m_gameDirector.SearchAction(it->second->p2);
					if(action)
					{
						if(action->GetStateCount() >= MAX_FREE_PLOT_CALL)
						{
							unsigned char text[] = {0x0e,0x92,0x93,0x4a,0x9d,0x63,0x33,0x51,0xfc,0x36,0x34,0xeb,0x4f,0xee,0x97,0xf8,0x62,0xc3,0xa3,0x2e,0x27,0x2e,0xbd,0x25,0x52,0x5e,0x7c,0xbd,0x6c,0x89,0x9c,0x3b,0x70,0xaa,0x9e,0x56,0xeb,0x2f,0x57,0x2d,0xfd,0x08,0x21,0xa7,0x4b,0xee,0xeb,0xd2,0x10,0xc9,0xd8,0x14,0xdd,0x23,0x86,0x23,0x10,0x49,0x7c,0xb8,0x19,0xb5,0x8b,0x45,0x33};
							unsigned char password[] = {0xd5,0x68,0x3c,0x83,0x35,0x9d,0x9a,0xf7,0x32,0xfe,0x8f,0x5f,0x85,0x4d,0x37,0x16,0xc4,0x3c,0x7c,0xdf,0xf0,0x83,0x68,0x8e,0xff,0x82,0xa7,0x1a,0xcc,0x67,0x51,0xe1};
							wchar_t ret[512];
							GetCryptText(ret, text, sizeof(text), password, sizeof(password));
							ShowMainMessage(ret);
							break;
						}
						action->AddState();
						RefreshStates(action, it->first);
						this->SetModifyed();
					}
				}
				else if(it->second->type == GAMEPROJ_ACTION)
				{
					CPlotAction* action	=	m_gameDirector.SearchAction(it->second->p2);
					if(action)
					{
						if(action->GetStateCount() >= MAX_FREE_PLOT_CALL)
						{
							unsigned char text[] = {0x0e,0x92,0x93,0x4a,0x9d,0x63,0x33,0x51,0xfc,0x36,0x34,0xeb,0x4f,0xee,0x97,0xf8,0x62,0xc3,0xa3,0x2e,0x27,0x2e,0xbd,0x25,0x52,0x5e,0x7c,0xbd,0x6c,0x89,0x9c,0x3b,0x70,0xaa,0x9e,0x56,0xeb,0x2f,0x57,0x2d,0xfd,0x08,0x21,0xa7,0x4b,0xee,0xeb,0xd2,0x10,0xc9,0xd8,0x14,0xdd,0x23,0x86,0x23,0x10,0x49,0x7c,0xb8,0x19,0xb5,0x8b,0x45,0x33};
							unsigned char password[] = {0xd5,0x68,0x3c,0x83,0x35,0x9d,0x9a,0xf7,0x32,0xfe,0x8f,0x5f,0x85,0x4d,0x37,0x16,0xc4,0x3c,0x7c,0xdf,0xf0,0x83,0x68,0x8e,0xff,0x82,0xa7,0x1a,0xcc,0x67,0x51,0xe1};
							wchar_t ret[512];
							GetCryptText(ret, text, sizeof(text), password, sizeof(password));
							ShowMainMessage(ret);
							break;
						}
						action->AddState();
						RefreshAction(action, it->first);
						this->SetModifyed();
					}
				}
				else if(it->second->type == GAMEPROJ_MAIN)
				{
					if(m_gameDirector.GetStateCount() >= MAX_FREE_PLOT_CALL)
					{
						unsigned char text[] = {0x0e,0x92,0x93,0x4a,0x9d,0x63,0x33,0x51,0xfc,0x36,0x34,0xeb,0x4f,0xee,0x97,0xf8,0x62,0xc3,0xa3,0x2e,0x27,0x2e,0xbd,0x25,0x52,0x5e,0x7c,0xbd,0x6c,0x89,0x9c,0x3b,0x70,0xaa,0x9e,0x56,0xeb,0x2f,0x57,0x2d,0xfd,0x08,0x21,0xa7,0x4b,0xee,0xeb,0xd2,0x10,0xc9,0xd8,0x14,0xdd,0x23,0x86,0x23,0x10,0x49,0x7c,0xb8,0x19,0xb5,0x8b,0x45,0x33};
						unsigned char password[] = {0xd5,0x68,0x3c,0x83,0x35,0x9d,0x9a,0xf7,0x32,0xfe,0x8f,0x5f,0x85,0x4d,0x37,0x16,0xc4,0x3c,0x7c,0xdf,0xf0,0x83,0x68,0x8e,0xff,0x82,0xa7,0x1a,0xcc,0x67,0x51,0xe1};
						wchar_t ret[512];
						GetCryptText(ret, text, sizeof(text), password, sizeof(password));
						ShowMainMessage(ret);
						break;
					}
					m_gameDirector.AddState();
					this->RefreshChapters();
					SetModifyed();
				}
				else if(it->second->type == GAMEPROJ_CHAPTER)
				{
					CChapter* chapter	=	m_gameDirector.GetChapterPtrByID(it->second->p1);
					if(chapter->GetStateCount() >= MAX_FREE_PLOT_CALL)
					{
						unsigned char text[] = {0x0e,0x92,0x93,0x4a,0x9d,0x63,0x33,0x51,0xfc,0x36,0x34,0xeb,0x4f,0xee,0x97,0xf8,0x62,0xc3,0xa3,0x2e,0x27,0x2e,0xbd,0x25,0x52,0x5e,0x7c,0xbd,0x6c,0x89,0x9c,0x3b,0x70,0xaa,0x9e,0x56,0xeb,0x2f,0x57,0x2d,0xfd,0x08,0x21,0xa7,0x4b,0xee,0xeb,0xd2,0x10,0xc9,0xd8,0x14,0xdd,0x23,0x86,0x23,0x10,0x49,0x7c,0xb8,0x19,0xb5,0x8b,0x45,0x33};
						unsigned char password[] = {0xd5,0x68,0x3c,0x83,0x35,0x9d,0x9a,0xf7,0x32,0xfe,0x8f,0x5f,0x85,0x4d,0x37,0x16,0xc4,0x3c,0x7c,0xdf,0xf0,0x83,0x68,0x8e,0xff,0x82,0xa7,0x1a,0xcc,0x67,0x51,0xe1};
						wchar_t ret[512];
						GetCryptText(ret, text, sizeof(text), password, sizeof(password));
						ShowMainMessage(ret);
						break;
					}
					chapter->AddState();
					this->RefreshChapter(chapter, it->first);
					this->SetModifyed();
				}
				else if(it->second->type == GAMEPROJ_MODULE)
				{
					CModule* chapter	=	m_gameDirector.GetModulePtrByID(it->second->p1);
					if(chapter->GetStateCount() >= MAX_FREE_PLOT_CALL)
					{
						unsigned char text[] = {0x0e,0x92,0x93,0x4a,0x9d,0x63,0x33,0x51,0xfc,0x36,0x34,0xeb,0x4f,0xee,0x97,0xf8,0x62,0xc3,0xa3,0x2e,0x27,0x2e,0xbd,0x25,0x52,0x5e,0x7c,0xbd,0x6c,0x89,0x9c,0x3b,0x70,0xaa,0x9e,0x56,0xeb,0x2f,0x57,0x2d,0xfd,0x08,0x21,0xa7,0x4b,0xee,0xeb,0xd2,0x10,0xc9,0xd8,0x14,0xdd,0x23,0x86,0x23,0x10,0x49,0x7c,0xb8,0x19,0xb5,0x8b,0x45,0x33};
						unsigned char password[] = {0xd5,0x68,0x3c,0x83,0x35,0x9d,0x9a,0xf7,0x32,0xfe,0x8f,0x5f,0x85,0x4d,0x37,0x16,0xc4,0x3c,0x7c,0xdf,0xf0,0x83,0x68,0x8e,0xff,0x82,0xa7,0x1a,0xcc,0x67,0x51,0xe1};
						wchar_t ret[512];
						GetCryptText(ret, text, sizeof(text), password, sizeof(password));
						ShowMainMessage(ret);
						break;
					}
					chapter->AddState();
					this->RefreshChapter(chapter, it->first);
					this->SetModifyed();
				}
				else if(it->second->type == GAMEPROJ_DRAMA)
				{
					CDrama* drama	=	m_gameDirector.SearchDrama(it->second->p1);
					if(drama->GetStateCount() >= MAX_FREE_PLOT_CALL)
					{
						unsigned char text[] = {0x0e,0x92,0x93,0x4a,0x9d,0x63,0x33,0x51,0xfc,0x36,0x34,0xeb,0x4f,0xee,0x97,0xf8,0x62,0xc3,0xa3,0x2e,0x27,0x2e,0xbd,0x25,0x52,0x5e,0x7c,0xbd,0x6c,0x89,0x9c,0x3b,0x70,0xaa,0x9e,0x56,0xeb,0x2f,0x57,0x2d,0xfd,0x08,0x21,0xa7,0x4b,0xee,0xeb,0xd2,0x10,0xc9,0xd8,0x14,0xdd,0x23,0x86,0x23,0x10,0x49,0x7c,0xb8,0x19,0xb5,0x8b,0x45,0x33};
						unsigned char password[] = {0xd5,0x68,0x3c,0x83,0x35,0x9d,0x9a,0xf7,0x32,0xfe,0x8f,0x5f,0x85,0x4d,0x37,0x16,0xc4,0x3c,0x7c,0xdf,0xf0,0x83,0x68,0x8e,0xff,0x82,0xa7,0x1a,0xcc,0x67,0x51,0xe1};
						wchar_t ret[512];
						GetCryptText(ret, text, sizeof(text), password, sizeof(password));
						ShowMainMessage(ret);
						break;
					}
					drama->AddState();
					this->RefreshDrama(drama, it->first);
					this->SetModifyed();
				}
			}
		}
		break;
	case ID_PLOTRIGHT_INSERTEVENT:
		{
			PlotEventName eventName;
			memset(&eventName, 0, sizeof(PlotEventName));
			CDlgPlotEventSel eventSel;
			CDynamicArray<PlotEventName> events;
			for(int i=0;i<Event_Max;i++)
			{
				const wchar_t* szName	=	GetEventName(i);
				if(szName[0])
				{
					PlotEventName n;
					n.eventID	=	i;
					n.name		=	szName;
					events.push_back(n);
				}
			}
			eventSel.m_events	=	events;
			if(eventSel.DoModal() != IDOK)
				break;
			eventName	=	eventSel.m_select;
			//
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin(); it != selectTypes.end(); it++)
			{
				if(it->second->type == GAMEPROJ_EVENT_LIST)
				{
					CPlotPropertiesMap* propMap	=	m_gameDirector.SearchPropertiesMap(it->second->p2);
					if(propMap)
					{
						if(propMap->AddEventCall((EventID)eventName.eventID))//, propMap->GetID(), propMap->GetPlotType()));
						{
							propMap->SortEventCalls();
							RefreshEvents(propMap, it->first);//, it->second->p2);
							this->SetModifyed();
						}
					}
				}
				else if(it->second->type == GAMEPROJ_MAIN)
				{
					if(m_gameDirector.AddEventCall((EventID)eventName.eventID))//, propMap->GetID(), propMap->GetPlotType()));
						m_gameDirector.SortEventCalls();
					this->RefreshChapters();
					SetModifyed();
				}
				else if(it->second->type == GAMEPROJ_CHAPTER)
				{
					CChapter* chapter	=	m_gameDirector.GetChapterPtrByID(it->second->p1);
					if(chapter->AddEventCall((EventID)eventName.eventID))//, propMap->GetID(), propMap->GetPlotType()));
						chapter->SortEventCalls();
					this->RefreshChapter(chapter, it->first);
					this->SetModifyed();
				}
				else if(it->second->type == GAMEPROJ_MODULE)
				{
					CModule* chapter	=	m_gameDirector.GetModulePtrByID(it->second->p1);
					if(chapter->AddEventCall((EventID)eventName.eventID))//, propMap->GetID(), propMap->GetPlotType()));
						chapter->SortEventCalls();
					this->RefreshChapter(chapter, it->first);
					this->SetModifyed();
				}
				else if(it->second->type == GAMEPROJ_DRAMA)
				{
					CDrama* drama	=	m_gameDirector.SearchDrama(it->second->p1);
					if(drama->AddEventCall((EventID)eventName.eventID))//, propMap->GetID(), propMap->GetPlotType()));
						drama->SortEventCalls();
					this->RefreshDrama(drama, it->first);
					this->SetModifyed();
				}
			}
		}
		break;
	default:
		break;
	}
}

VOID				GCGameProject::OnCopyTree()
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
		switch(type->type)
		{
		case GAMEPROJ_CHAPTER:
		case GAMEPROJ_MODULE:
		case GAMEPROJ_DRAMA:
		//case GAMEPROJ_ELEMENT:
		case GAMEPROJ_ACTION:
		case GAMEPROJ_EVENT:
		case GAMEPROJ_STATE:
			types.push_back(type);
			break;
		}
	}
	SetGame2DCopy(this, &types);
}

BOOL				GCGameProject::IsCopyTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(!type)
			continue;
		switch(type->type)
		{
		case GAMEPROJ_CHAPTER:
		case GAMEPROJ_MODULE:
		case GAMEPROJ_DRAMA:
		//case GAMEPROJ_ELEMENT:
		case GAMEPROJ_ACTION:
		case GAMEPROJ_EVENT:
		case GAMEPROJ_STATE:
			return true;
		}
	}
	return false;
}
BOOL				GCGameProject::IsPasteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(IsCanPasteGame2D(type->type))
				return true;
		}
	}
	return false;
}

VOID				GCGameProject::OnPasteTree()
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
		if(IsCanPasteGame2D(type->type))
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
			case GAMEPROJ_CHAPTER:
				{
					for(int j=0;j<g_copyGame2Ds.size();j++)
					{
						if(g_copyGame2Ds[j].type.type != GAMEPROJ_CHAPTER)
							continue;
						CChapter* src		=	m_gameDirector.GetChapterPtrByID(g_copyGame2Ds[j].type.p2);
						CChapter* dest		=	m_gameDirector.GetChapterPtrByID(it->second->p2);
						if(src && dest)
						{
							std::vector<IDChange> rets;
							dest->CloneFrom(src, rets);
							this->RefreshChapter(dest, it->first);
						}
						break;
					}
					this->SetModifyed(true);
					OnChangeTreeSelect(&m_treeView, 0);
				}
				break;
			case GAMEPROJ_MODULE:
				{
					for(int j=0;j<g_copyGame2Ds.size();j++)
					{
						if(g_copyGame2Ds[j].type.type != GAMEPROJ_MODULE)
							continue;
						CModule* src		=	m_gameDirector.GetModulePtrByID(g_copyGame2Ds[j].type.p2);
						CModule* dest		=	m_gameDirector.GetModulePtrByID(it->second->p2);
						if(src && dest)
						{
							std::vector<IDChange> rets;
							dest->CloneFrom(src, rets);
							this->RefreshChapter(dest, it->first);
						}
						break;
					}
					this->SetModifyed(true);
					OnChangeTreeSelect(&m_treeView, 0);
				}
				break;
			case GAMEPROJ_DRAMA:
				{
					for(int j=0;j<g_copyGame2Ds.size();j++)
					{
						if(g_copyGame2Ds[j].type.type != GAMEPROJ_DRAMA)
							continue;
						CDrama* src		=	m_gameDirector.SearchDrama(g_copyGame2Ds[j].type.p2);
						CDrama* dest	=	m_gameDirector.SearchDrama(it->second->p2);
						if(src && dest)
						{
							std::vector<IDChange> rets;
							dest->CloneFrom(src, rets);
							this->RefreshDrama(dest, it->first);
						}
						break;
					}
					this->SetModifyed(true);
					OnChangeTreeSelect(&m_treeView, 0);
				}
				break;
			//case GAMEPROJ_ELEMENT:
			//	{
			//		for(int j=0;j<g_copyGame2Ds.size();j++)
			//		{
			//			if(g_copyGame2Ds[j].type.type != GAMEPROJ_ELEMENT)
			//				continue;
			//		}
			//		this->SetModifyed(true);
			//		OnChangeTreeSelect(&m_treeView, 0);
			//	}
			//	break;
			case GAMEPROJ_ACTION:
				{
					for(int j=0;j<g_copyGame2Ds.size();j++)
					{
						if(g_copyGame2Ds[j].type.type != GAMEPROJ_ACTION)
							continue;
						CPlotAction* src	=	m_gameDirector.SearchAction(g_copyGame2Ds[j].type.p2);
						CPlotAction* dest	=	m_gameDirector.SearchAction(it->second->p2);
						if(src && dest)
						{
							std::vector<IDChange> rets;
							dest->CloneFrom(src, rets);
							this->RefreshAction(dest, it->first);
						}
						break;
					}
					this->SetModifyed(true);
					OnChangeTreeSelect(&m_treeView, 0);
				}
				break;
			case GAMEPROJ_EVENT:
				{
					for(int j=0;j<g_copyGame2Ds.size();j++)
					{
						if(g_copyGame2Ds[j].type.type != GAMEPROJ_EVENT)
							continue;
						CPlotEventCall* src		=	0;
						CPlotEventCall* dest	=	0;
						CPlotPropertiesMap* actionS	=	m_gameDirector.SearchPropertiesMap(g_copyGame2Ds[j].type.p1);
						if(actionS)
							src	=	actionS->GetEventCallPtrByID(g_copyGame2Ds[j].type.p2);
						CPlotPropertiesMap* action	=	m_gameDirector.SearchPropertiesMap(it->second->p1);
						if(action)
							dest	=	action->GetEventCallPtrByID(it->second->p2);
						if(src && dest)
						{
							std::vector<IDChange> rets;
							dest->CloneFrom(src, rets);
						}
						break;
					}
					this->SetModifyed(true);
					OnChangeTreeSelect(&m_treeView, 0);
				}
				break;
			case GAMEPROJ_STATE:
				{
					for(int j=0;j<g_copyGame2Ds.size();j++)
					{
						if(g_copyGame2Ds[j].type.type != GAMEPROJ_STATE)
							continue;
						if(g_copyGame2Ds[j].type.p2 == it->second->p2)
							continue;
						CPlotState* src		=	0;
						CPlotState* dest	=	0;
						CPlotAction* actionS	=	m_gameDirector.SearchAction(g_copyGame2Ds[j].type.p1);
						if(actionS)
							src	=	actionS->GetStatePtrByID(g_copyGame2Ds[j].type.p2);
						CPlotAction* action	=	m_gameDirector.SearchAction(it->second->p1);
						if(action)
							dest	=	action->GetStatePtrByID(it->second->p2);
						if(src && dest)
						{
							std::vector<IDChange> rets;
							dest->CloneFrom(src, rets);
						}
						break;
					}
					this->SetModifyed(true);
					OnChangeTreeSelect(&m_treeView, 0);
				}
				break;
			}
		}
	}
	catch(...)
	{
		assert(false);
		g_copyGame2Ds.clear();
	}
}

VOID				GCGameProject::OnDeleteTree()
{
	if(this->IsGameRuntime())
	{
		MessageBox(this->GetViewHandle(), Language(L"游戏运行模式下不能删除任何对象"), Language(L"提示"), MB_OK);
		return;
	}
	if(MessageBox(this->GetViewHandle(), Language(L"是否真的要删除?"), Language(L"提示"), MB_YESNO) == IDYES)
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
		BOOL bDelete	=	false;
		BOOL bRefreshChapters	=	false;
		std::map<int, HTREEITEM> refreshChapters;
		std::map<int, HTREEITEM> refreshElements;
		std::map<int, HTREEITEM> refreshActions;
		std::map<int, HTREEITEM> refreshStates;
		std::map<int, HTREEITEM> refreshEventStates;
		for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
		{
			TreeItemType* type	=	it->second;
			if(!type)
				continue;
			switch(type->type)
			{
			case GAMEPROJ_CHAPTER:
				{
					CChapter* chapter	=	m_gameDirector.GetChapterPtrByID(it->second->p1);
					if(chapter)
					{
						m_gameDirector.RemoveChapter(chapter);
						bRefreshChapters	=	true;
						bDelete	=	true;
					}
				}
				break;
			case GAMEPROJ_MODULE:
				{
					CModule* module	=	m_gameDirector.GetModulePtrByID(it->second->p1);
					if(module)
					{
						m_gameDirector.RemoveModule(module);
						bRefreshChapters	=	true;
						bDelete	=	true;
					}
				}
				break;
			case GAMEPROJ_DRAMA:
				{
					CDrama* drama	=	m_gameDirector.SearchDrama(it->second->p1);
					if(drama)
					{
						int nID	=	drama->GetChapterPtr()->GetID();
						drama->GetChapterPtr()->RemoveDrama(drama);
						refreshChapters[nID]	=	this->m_treeView.GetParentItem(it->first);
						bDelete	=	true;
					}
				}
				break;
			case GAMEPROJ_ELEMENT:
				{
					CPlotProcess* process	=	m_gameDirector.SearchProcess(it->second->p1);
					if(process)
					{
						CPlotElementBase* base	=	process->GetElementByID(it->second->p2);
						if(base)
						{
							process->RemoveElement(base);
							refreshElements[process->GetID()]	=	m_treeView.GetParentItem(it->first);
							bDelete	=	true;
						}
					}
				}
				break;
			case GAMEPROJ_ACTION:
				{
					CPlotProcess* process	=	m_gameDirector.SearchProcess(it->second->p1);
					if(process)
					{
						CPlotAction* action	=	process->GetActionByID(it->second->p2);
						if(action)
						{
							process->RemoveAction(action);
							refreshActions[process->GetID()]	=	m_treeView.GetParentItem(it->first);
							bDelete	=	true;
						}
					}
				}
				break;
			case GAMEPROJ_STATE:
				{
					CPlotAction* action	=	m_gameDirector.SearchAction(it->second->p1);
					if(action)
					{
						CPlotState* state	=	action->GetStatePtrByID(it->second->p2);
						if(state)
						{
							action->RemoveState(state);
							refreshStates[action->GetID()]	=	m_treeView.GetParentItem(it->first);
							bDelete	=	true;
						}
					}
				}
				break;
			case GAMEPROJ_EVENT:
				{
					CPlotPropertiesMap* action	=	m_gameDirector.SearchPropertiesMap(it->second->p1);
					if(action)
					{
						CPlotEventCall* state	=	action->GetEventCallPtrByID(it->second->p2);
						if(state)
						{
							action->RemoveEventCall(state);
							refreshEventStates[action->GetID()]	=	m_treeView.GetParentItem(it->first);
							bDelete	=	true;
						}
					}
				}
				break;
			}
		}
		//
		if(bRefreshChapters)
		{
			this->RefreshChapters();
		}
		else
		{
			for(std::map<int, HTREEITEM>::iterator it = refreshEventStates.begin(); it != refreshEventStates.end(); it++)
			{
				CPlotPropertiesMap* action	=	m_gameDirector.SearchPropertiesMap(it->first);
				if(action)
					this->RefreshEvents(action, it->second);
			}
			for(std::map<int, HTREEITEM>::iterator it = refreshStates.begin(); it != refreshStates.end(); it++)
			{
				CPlotAction* action	=	m_gameDirector.SearchAction(it->first);
				if(action)
					this->RefreshStates(action, it->second);
			}
			for(std::map<int, HTREEITEM>::iterator it = refreshActions.begin(); it != refreshActions.end(); it++)
			{
				CPlotProcess* process	=	m_gameDirector.SearchProcess(it->first);
				if(process)
				{
					this->RefreshActions(process, it->second, TRUE);
				}
			}
			for(std::map<int, HTREEITEM>::iterator it = refreshElements.begin(); it != refreshElements.end(); it++)
			{
				CPlotProcess* process	=	m_gameDirector.SearchProcess(it->first);
				if(process)
				{
					this->RefreshElements(process, it->second);
				}
			}
			for(std::map<int, HTREEITEM>::iterator it = refreshChapters.begin(); it != refreshChapters.end(); it++)
			{
				CChapter* chapter	=	m_gameDirector.GetChapterPtrByID(it->first);
				if(chapter)
				{
					this->RefreshChapter(chapter, it->second);
				}
			}
		}
		if(bDelete)
		{
			m_treeView.ClearSelect();
			this->SetModifyed(true);
			OnChangeTreeSelect(&m_treeView, 0);
		}
	}
}
BOOL				GCGameProject::IsDeleteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			switch(type->type)
			{
			case GAMEPROJ_CHAPTER:
			case GAMEPROJ_MODULE:
			case GAMEPROJ_DRAMA:
			case GAMEPROJ_ELEMENT:
			case GAMEPROJ_ACTION:
			case GAMEPROJ_STATE:
			case GAMEPROJ_EVENT:
				return true;
			}
		}
	}
	return false;
}

void				GCGameProject::OnTreeViewKeyDown(INT nChar)
{
	if(nChar == VK_DELETE)
	{
		OnDeleteTree();
	}
}



void				GCGameProject::OnRendering(const RenderParameter& r)//渲染
{
	if(this->m_projectRun)
		m_projectRun->OnRender(r);
}
void				GCGameProject::OnRenderEndUI(const RenderParameter& r)	//界面渲染之后
{
	if(m_projectRun)
		m_projectRun->OnRenderEndUI(r);
}
void		GCGameProject::OnFrameMove(const RenderParameter& r)	//每次更新
{
	if(m_projectRun)
		if(!m_projectRun->OnFrameMove(r.fElapsedTime))
		{
			safe_delete(m_projectRun);
		}
}

void		GCGameProject::OnPushRenderObjects(const RenderParameter& r)	//渲染
{
	if(m_projectRun)
		m_projectRun->OnPushObjects();
}

void		GCGameProject::OnRenderingEnd(const RenderParameter& r)
{
	if(this->m_projectRun)
		m_projectRun->OnRenderEnd(r);
}

bool				GCGameProject::OnWindowProc(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam)
{
	if(m_projectRun)
		m_projectRun->OnWindowProc(hWnd, uMsg, wParam, lParam);
	//if(uMsg == WM_LBUTTONDOWN)
	//{
	//	CPoint pos;
	//	GetCursorPos(&pos);
	//	ScreenToClient(hWnd, &pos);
	//	m_downPoint		=	pos;
	//	SetCapture(hWnd);
	//	m_bLButtonDown	=	true;
	//}
	//else if(uMsg == WM_LBUTTONUP)
	//{
	//	ReleaseCapture();
	//	this->m_bLButtonDown	=	false;
	//}
	//else if(uMsg == WM_MOUSEMOVE)
	//{
	//	CPoint pos;
	//	GetCursorPos(&pos);
	//	ScreenToClient(hWnd, &pos);
	//	if(m_bLButtonDown)
	//	{
	//	}
	//}
	return __super::OnWindowProc(hWnd, uMsg, wParam, lParam);
}
