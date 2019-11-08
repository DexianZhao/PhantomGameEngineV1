// DlgInsertEvent.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgInsertEvent.h"
#include "afxdialogex.h"
#include "GCGameProject.h"

// CDlgInsertEvent 对话框

extern	GCGameProject*	g_gameProject;

IMPLEMENT_DYNAMIC(CDlgInsertEvent, CDialogBase)

CDlgInsertEvent::CDlgInsertEvent(CWnd* pParent /*=NULL*/)
	: CDialogBase(CDlgInsertEvent::IDD, pParent)
{
	memset(m_nParameter, 0, sizeof(m_nParameter));
	m_nEventID	=	0;
	m_name[0]	=	0;
}

CDlgInsertEvent::~CDlgInsertEvent()
{
}

void CDlgInsertEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GAME_PROJ, m_treeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgInsertEvent, CDialogBase)
	ON_BN_CLICKED(IDOK, &CDlgInsertEvent::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgInsertEvent::OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_GAME_PROJ, &CDlgInsertEvent::OnTvnSelchangedGameProj)
END_MESSAGE_MAP()


// CDlgInsertEvent 消息处理程序

VOID CDlgInsertEvent::Init(INT nEventID, INT nParam1, INT nParam2, INT nParam3, INT nParam4, INT nParam5)
{
	this->m_nEventID		=	nEventID;
	this->m_nParameter[0]	=	nParam1;
	this->m_nParameter[1]	=	nParam2;
	this->m_nParameter[2]	=	nParam3;
	this->m_nParameter[3]	=	nParam4;
	this->m_nParameter[4]	=	nParam5;
}

void CDlgInsertEvent::OnBnClickedOk()
{
	HTREEITEM hItem	=	m_treeCtrl.GetSelectedItem();
	if(hItem)
	{
		INT nItem	=	m_treeCtrl.GetItemData(hItem);
		if(nItem != 0)
		{
			CPlotAction* action	=	g_gameProject->m_gameDirector.SearchAction(nItem);
			if(action)
			{
				CPlotEventCall* call	=	action->AddEventCall((EventID)this->m_nEventID);
				if(call)
				{
					call->SetName(this->m_name);
					//for(int i=0;i<MAX_EVENT_PARAM_COUNT;i++)
						//call->SetEventParameter(i, this->m_nParameter[i]);
					//
					g_gameProject->SetModifyed();
					g_gameProject->RefreshEvents(action, 0);
				}
			}
		}
	}
	CDialogBase::OnOK();
}


void CDlgInsertEvent::OnBnClickedCancel()
{
	CDialogBase::OnCancel();
}


BOOL CDlgInsertEvent::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	if(!g_gameProject)
		return false;
	HTREEITEM hItem	=	m_treeCtrl.InsertItem(g_gameProject->m_gameDirector.GetName());
	m_treeCtrl.SetItemData(hItem, g_gameProject->m_gameDirector.GetID());
	m_treeCtrl.Expand(hItem, TVE_EXPAND);
	//
	HTREEITEM hActionItem	=	m_treeCtrl.InsertItem(Language(L"动作列表"), hItem);
	m_treeCtrl.SetItemData(hActionItem, 0);
	for(int i=0;i<g_gameProject->m_gameDirector.GetActionCount();i++)
	{
		CPlotAction* act	=	g_gameProject->m_gameDirector.GetActionPtr(i);
		HTREEITEM hAction	=	m_treeCtrl.InsertItem(GetPlotName(act), hActionItem);
		m_treeCtrl.SetItemData(hAction, act->GetID());
	}
	m_treeCtrl.Expand(hActionItem, TVE_EXPAND);
	for(int c=0;c<g_gameProject->m_gameDirector.GetChapterCount();c++)
	{
		CChapter* ch		=	g_gameProject->m_gameDirector.GetChapterPtr(c);
		HTREEITEM hChapter	=	m_treeCtrl.InsertItem(GetPlotName(ch), hItem);
		m_treeCtrl.SetItemData(hChapter, ch->GetID());
		m_treeCtrl.Expand(hChapter, TVE_EXPAND);
		HTREEITEM hChapterActionItem	=	m_treeCtrl.InsertItem(Language(L"动作列表"), hChapter);
		m_treeCtrl.SetItemData(hChapterActionItem, 0);
		for(int a=0;a<ch->GetActionCount();a++)
		{
			CPlotAction* act	=	ch->GetActionPtr(a);
			HTREEITEM hAction	=	m_treeCtrl.InsertItem(GetPlotName(act), hChapterActionItem);
			m_treeCtrl.SetItemData(hAction, act->GetID());
		}
		m_treeCtrl.Expand(hChapterActionItem, TVE_EXPAND);
		for(int d=0;d<ch->GetDramaCount();d++)
		{
			CDrama* drama		=	ch->GetDramaPtr(d);
			HTREEITEM hDrama	=	m_treeCtrl.InsertItem(GetPlotName(drama), hChapter);
			m_treeCtrl.SetItemData(hDrama, drama->GetID());
			m_treeCtrl.Expand(hDrama, TVE_EXPAND);
			for(int a=0;a<drama->GetActionCount();a++)
			{
				CPlotAction* act	=	drama->GetActionPtr(a);
				HTREEITEM hAction	=	m_treeCtrl.InsertItem(GetPlotName(act), hDrama);
				m_treeCtrl.SetItemData(hAction, act->GetID());
			}
			m_treeCtrl.Expand(hDrama, TVE_EXPAND);
		}
		m_treeCtrl.Expand(hChapter, TVE_EXPAND);
	}
	for(int c=0;c<g_gameProject->m_gameDirector.GetModuleCount();c++)
	{
		CChapter* ch		=	g_gameProject->m_gameDirector.GetModulePtr(c);
		HTREEITEM hChapter	=	m_treeCtrl.InsertItem(GetPlotName(ch), hItem);
		m_treeCtrl.SetItemData(hChapter, ch->GetID());
		m_treeCtrl.Expand(hChapter, TVE_EXPAND);
		HTREEITEM hChapterActionItem	=	m_treeCtrl.InsertItem(Language(L"动作列表"), hChapter);
		m_treeCtrl.SetItemData(hChapterActionItem, 0);
		for(int a=0;a<ch->GetActionCount();a++)
		{
			CPlotAction* act	=	ch->GetActionPtr(a);
			HTREEITEM hAction	=	m_treeCtrl.InsertItem(GetPlotName(act), hChapterActionItem);
			m_treeCtrl.SetItemData(hAction, act->GetID());
		}
		m_treeCtrl.Expand(hChapterActionItem, TVE_EXPAND);
		for(int d=0;d<ch->GetDramaCount();d++)
		{
			CDrama* drama		=	ch->GetDramaPtr(d);
			HTREEITEM hDrama	=	m_treeCtrl.InsertItem(GetPlotName(drama), hChapter);
			m_treeCtrl.SetItemData(hDrama, drama->GetID());
			m_treeCtrl.Expand(hDrama, TVE_EXPAND);
			for(int a=0;a<drama->GetActionCount();a++)
			{
				CPlotAction* act	=	drama->GetActionPtr(a);
				HTREEITEM hAction	=	m_treeCtrl.InsertItem(GetPlotName(act), hDrama);
				m_treeCtrl.SetItemData(hAction, act->GetID());
			}
			m_treeCtrl.Expand(hDrama, TVE_EXPAND);
		}
		m_treeCtrl.Expand(hChapter, TVE_EXPAND);
	}
	m_treeCtrl.Expand(hItem, TVE_EXPAND);
	this->GetDlgItem(IDOK)->EnableWindow(FALSE);
	return TRUE;
}


void CDlgInsertEvent::OnTvnSelchangedGameProj(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem	=	m_treeCtrl.GetSelectedItem();
	if(hItem)
	{
		INT nItem	=	m_treeCtrl.GetItemData(hItem);
		if(nItem == 0)
			this->GetDlgItem(IDOK)->EnableWindow(FALSE);
		else
			this->GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
		this->GetDlgItem(IDOK)->EnableWindow(FALSE);
	*pResult = 0;
}
