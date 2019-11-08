// DlgUIEventSet.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgUIEventSet.h"
#include "afxdialogex.h"
#include "DlgInsertEvent.h"

// CDlgUIEventSet 对话框

IMPLEMENT_DYNAMIC(CDlgUIEventSet, CDialogBase)

CDlgUIEventSet::CDlgUIEventSet(CWnd* pParent /*=NULL*/)
	: CDialogBase(CDlgUIEventSet::IDD, pParent)
{
	m_nEventID		=	0;
	m_nDialogID		=	0;
	m_nControlID	=	0;
	m_name[0]		=	0;
}

CDlgUIEventSet::~CDlgUIEventSet()
{
}

void CDlgUIEventSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ICON_LIST, m_lbEventLinks);
	DDX_Control(pDX, IDC_INSERT_EVENTCALL, m_btInsert);
	DDX_Control(pDX, IDC_REMOVE_SELECT, m_btRemove);
	DDX_Control(pDX, IDC_EVENT_LIST, m_lbEvent);
}


BEGIN_MESSAGE_MAP(CDlgUIEventSet, CDialogBase)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_INSERT_EVENTCALL, &CDlgUIEventSet::OnBnClickedInsertEventcall)
	ON_BN_CLICKED(IDC_REMOVE_SELECT, &CDlgUIEventSet::OnBnClickedRemoveSelect)
	ON_LBN_SELCHANGE(IDC_EVENT_LIST, &CDlgUIEventSet::OnLbnSelchangeEventList)
	ON_LBN_DBLCLK(IDC_EVENT_LIST, &CDlgUIEventSet::OnLbnDblclkIconList)
END_MESSAGE_MAP()


// CDlgUIEventSet 消息处理程序

BOOL CDlgUIEventSet::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	CRect rectDummy;
	GetClientRect(&rectDummy);
	m_lbEventLinks.MoveWindow(&CRect(rectDummy.left + 200, rectDummy.top, rectDummy.right, rectDummy.bottom));//.Create(rectDummy, this))
	m_lbEventLinks.m_bShowIndex	=	true;
	m_lbEventLinks.SetIconRes(IDB_PLOT_CALL_ICON);
	m_lbEventLinks.SetFont(GetFont());
	m_lbEvent.MoveWindow(&CRect(rectDummy.left, rectDummy.top, rectDummy.left + 200, rectDummy.bottom));//.Create(rectDummy, this))
	m_lbEvent.m_bShowIndex	=	true;
	m_lbEvent.SetIconRes(IDB_PLOT_CALL_ICON);
	m_lbEvent.SetFont(GetFont());
	m_imgButton.LoadFromResource(AfxGetResourceHandle(), IDB_PROPERTIES_PLOTCALL);
	CPoint pos(10, 0);
	this->m_btInsert.SetImage(this->m_imgButton, 2, -1, 5, &pos);
	this->m_btRemove.SetImage(this->m_imgButton, 2, -1, 3, &pos);
	AddTool(m_btInsert, Language(L"插入一个事件处理，必须要同时打开.rpgProj项目文件才可以起作用"));
	return TRUE;
}


VOID	CDlgUIEventSet::SetUIEvent(INT nEventID, INT nDialogID, INT nControlID, const wchar_t* szName)
{
	//m_nEventID		=	nEventID;
	//m_nDialogID		=	nDialogID;
	//m_nControlID	=	nControlID;
	//wcscpy(m_name, szName);
	////
	//m_lbEventLinks.ResetContent();
	//if(g_gameProject)
	//{
	//	CDynamicArray<CPlotPropertiesMap*> rets;
	//	g_gameProject->m_gameDirector.EnumAllPropertiesMap(rets);
	//	for(int c=0;c<rets.size();c++)
	//	{
	//		CPlotPropertiesMap* map	=	rets[c];
	//		for(int i=0;i<map->GetEventCallCount();i++)
	//		{
	//			CPlotEventCall* call	=	map->GetEventCallPtr(i);
	//			if(call)
	//			{
	//				if(call->GetEventID() == nEventID)
	//				{
	//					//if(call->GetEventParameter(0) == nDialogID && call->GetEventParameter(1) == nControlID)
	//					//{
	//					//	std::wstring str;
	//					//	str	=	map->GetProjectPtr()->GetName();
	//					//	if(map->GetChapterPtr())
	//					//	{
	//					//		str	+=	L"->";
	//					//		str	+=	map->GetChapterPtr()->GetName();
	//					//		if(map->GetDramaPtr())
	//					//		{
	//					//			str	+=	L"->";
	//					//			str	+=	map->GetDramaPtr()->GetName();
	//					//		}
	//					//	}
	//					//	if(map->GetPlotType() == PlotType_Action)
	//					//	{
	//					//		str	+=	L"->";
	//					//		str	+=	map->GetActionPtr()->GetName();
	//					//	}
	//					//	str	+=	L":";
	//					//	str	+=	call->GetName();
	//					//	this->m_lbEventLinks.AddString(str.c_str(), 4);
	//					//}
	//				}
	//			}
	//		}
	//	}
	//}
}


BOOL CDlgUIEventSet::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&CRect(0, 0, rc.right, 16), &CBrush(RGB(192,192,192)));
	return TRUE;
}


void CDlgUIEventSet::OnSize(UINT nType, int cx, int cy)
{
	CDialogBase::OnSize(nType, cx, cy);
	CRect rc;
	GetClientRect(&rc);
	if(m_lbEventLinks.m_hWnd)
	{
		const int height	=	16;
		m_lbEventLinks.MoveWindow(&CRect(200, height, rc.right, rc.bottom));
		m_lbEvent.MoveWindow(&CRect(0, height, 200, rc.bottom));
	}
}


void CDlgUIEventSet::OnBnClickedInsertEventcall()
{
	//if(!g_gameProject)
	//{
	//	MessageBox(Language(L"必须要打开一个.rpgProj文件才能使用此项功能"), Language(L"提示"), MB_OK);
	//	return;
	//}
	//CDlgInsertEvent evt(this);
	//evt.m_nEventID		=	this->m_nEventID;
	//evt.m_nParameter[0]	=	this->m_nDialogID;
	//evt.m_nParameter[1]	=	this->m_nControlID;
	//wcscpy(evt.m_name, this->m_name);
	//if(evt.DoModal() != IDOK)
	//	return;
	//this->SetUIEvent(this->m_nEventID, this->m_nDialogID, this->m_nControlID, this->m_name);
}

void CDlgUIEventSet::OnBnClickedRemoveSelect()
{
}


void CDlgUIEventSet::OnLbnSelchangeEventList()
{
	int sel	=	m_lbEvent.GetCurSel();
	if(sel < 0)
		return;
	INT nID	=	m_lbEvent.GetItemData(sel);
	for(int i=0;i<m_controlPtr->m_events.size();i++)
		if(nID == m_controlPtr->m_events[i].id)//GetEventPtr(i)->GetID())
		{
			wchar_t name[128];
			char szFile[128], szExt[128];
			_splitpath(m_controlPtr->GetDialogPtr()->m_strFile.t, 0, 0, szFile, szExt);
			strcat(szFile, szExt);
			//
			swprintf(name, L"%s[%s]", m_controlPtr->GetName(), W(szFile));
			SetUIEvent(m_controlPtr->m_events[i].id, m_controlPtr->GetDialogPtr()->GetID(), m_controlPtr->GetID(), name);
			break;
		}
}

VOID CDlgUIEventSet::SetControlPtr(Phantom::UIControl* c)
{
	//int sel	=	m_lbEvent.GetCurSel();
	//m_lbEvent.ResetContent();
	//m_controlPtr	=	c;
	//for(int i=0;i<m_controlPtr->GetEventCount();i++)
	//{
	//	IEvent* evt	=	m_controlPtr->GetEventPtr(i);
	//	int index	=	m_lbEvent.AddString(evt->GetName(), 4);
	//	m_lbEvent.SetItemData(index, evt->GetID());
	//}
	//if(sel < 0)
	//	sel	=	0;
	//if(sel < m_lbEvent.GetCount())
	//{
	//	m_lbEvent.SetCurSel(sel);
	//	OnLbnSelchangeEventList();
	//}
}


void CDlgUIEventSet::OnLbnDblclkIconList()
{
	OnBnClickedInsertEventcall();
}
