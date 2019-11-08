// DlgSceneObject.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgSceneObject.h"
#include "afxdialogex.h"
#include "GCRenderSystem.h"
#include "MainFrm.h"
#include "GCViewManager.h"
#include "GCBitmapMgr.h"
#include "CreateFileDlg.h"
#include "GCViewUIEditor.h"
#include "UIPngMaker.h"
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "DlgSceneObject.h"
#include "IDESocket.h"
#include "GCSceneEditor3D.h"
using namespace PhantomV1;
// DlgSceneObject 对话框

IMPLEMENT_DYNAMIC(DlgSceneObject, CDialogEx)

DlgSceneObject::DlgSceneObject(CWnd* pParent /*=NULL*/)
	: CDialogEx(DlgSceneObject::IDD, pParent)
{

}

DlgSceneObject::~DlgSceneObject()
{
}

VOID	DlgSceneObject::RefreshSceneUnits()
{
	int sels[512];
	int count	=	m_lbSceneUnits.GetSelItems(512, sels);
	int selids[512];
	for(int i=0;i<count;i++){
		selids[i] = m_lbSceneUnits.GetItemData(sels[i]);
	}
	m_lbSceneUnits.ResetContent();
	//for(int i=0;i<m_editors->m_scene3D->m_sceneUnits.size();i++)
	//{
	//	int index = m_lbSceneUnits.AddString(Wutf8(m_editors->m_scene3D->m_sceneUnits[i]->GetName()));
	//	m_lbSceneUnits.SetItemData(index, m_editors->m_scene3D->m_sceneUnits[i]->GetID());
	//	for(int j=0;j<count;j++)
	//	{
	//		if(m_editors->m_scene3D->m_sceneUnits[i]->GetID() == selids[j]){
	//			m_lbSceneUnits.SetSel(index);
	//			break;
	//		}
	//	}
	//}
}
VOID	DlgSceneObject::RefershRoles()
{
	int sels[512];
	int count	=	m_lbRoles.GetSelItems(512, sels);
	int selids[512];
	for(int i=0;i<count;i++){
		selids[i] = m_lbRoles.GetItemData(sels[i]);
	}
	m_lbRoles.ResetContent();
	//for(int i=0;i<m_editors->m_scene3D->m_sceneRoles.size();i++)
	//{
	//	int index = m_lbRoles.AddString(Wutf8(m_editors->m_scene3D->m_sceneRoles[i]->GetName()));
	//	m_lbRoles.SetItemData(index, m_editors->m_scene3D->m_sceneRoles[i]->GetID());
	//	for(int j=0;j<count;j++)
	//	{
	//		if(m_editors->m_scene3D->m_sceneRoles[i]->GetID() == selids[j]){
	//			m_lbRoles.SetSel(index);
	//			break;
	//		}
	//	}
	//}
}
VOID	DlgSceneObject::RefreshEvents()
{
	int sels[512];
	int count	=	m_lbEvents.GetSelItems(512, sels);
	int selids[512];
	for(int i=0;i<count;i++){
		selids[i] = m_lbEvents.GetItemData(sels[i]);
	}
	m_lbEvents.ResetContent();
	for(int i=0;i<m_editors->m_scene3D->GetActiveEnv()->m_points.size();i++)
	{
		int index = m_lbEvents.AddString(Wutf8(m_editors->m_scene3D->GetActiveEnv()->m_points[i]->m_name.t));
		m_lbEvents.SetItemData(index, m_editors->m_scene3D->GetActiveEnv()->m_points[i]->GetID());
		for(int j=0;j<count;j++)
		{
			if(m_editors->m_scene3D->GetActiveEnv()->m_points[i]->GetID() == selids[j]){
				m_lbEvents.SetSel(index);
				break;
			}
		}
	}
}

void DlgSceneObject::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_SCENE_UNITS, m_lbSceneUnits);
	DDX_Control(pDX, IDC_ROLES, m_lbRoles);
	DDX_Control(pDX, IDC_EVENTS, m_lbEvents);
	DDX_Control(pDX, IDC_CHK_SCENEUNITS, m_chkSceneUnits);
	DDX_Control(pDX, IDC_CHK_ROLES, m_chkRoles);
	DDX_Control(pDX, IDC_CHK_EVENTS, m_chkEvents);
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgSceneObject, CDialogEx)
	ON_BN_CLICKED(IDC_CHK_SCENEUNITS, &DlgSceneObject::OnBnClickedChkSceneunits)
	ON_BN_CLICKED(IDC_CHK_ROLES, &DlgSceneObject::OnBnClickedChkRoles)
	ON_BN_CLICKED(IDC_CHK_EVENTS, &DlgSceneObject::OnBnClickedChkEvents)
	ON_LBN_SELCHANGE(IDC_SCENE_UNITS, &DlgSceneObject::OnLbnSelchangeSceneUnits)
	ON_LBN_SELCHANGE(IDC_ROLES, &DlgSceneObject::OnLbnSelchangeRoles)
	ON_LBN_SELCHANGE(IDC_EVENTS, &DlgSceneObject::OnLbnSelchangeEvents)
END_MESSAGE_MAP()

BOOL DlgSceneObject::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rc;
	GetClientRect(&rc);
	m_lbSceneUnits.MoveWindow(2, 30, rc.right-4, rc.bottom-70);
	m_lbRoles.MoveWindow(2, 30, rc.right-4, rc.bottom-70);
	m_lbEvents.MoveWindow(2, 30, rc.right-4, rc.bottom-70);
	m_chkSceneUnits.SetCheck(false);
	m_chkRoles.SetCheck(false);
	m_chkEvents.SetCheck(false);
	return TRUE;
}
VOID DlgSceneObject::Active(CButton* b)
{
	m_lbSceneUnits.ShowWindow(SW_HIDE);
	m_lbRoles.ShowWindow(SW_HIDE);
	m_lbEvents.ShowWindow(SW_HIDE);
	m_chkSceneUnits.SetCheck(false);
	m_chkRoles.SetCheck(false);
	m_chkEvents.SetCheck(false);
	if(&m_chkSceneUnits==b){
		m_chkSceneUnits.SetCheck(true);
		m_lbSceneUnits.ShowWindow(SW_SHOW);
	}
	else if(&m_chkRoles==b){
		m_chkRoles.SetCheck(true);
		m_lbRoles.ShowWindow(SW_SHOW);
	}
	else if(&m_chkEvents==b){
		m_chkEvents.SetCheck(true);
		m_lbEvents.ShowWindow(SW_SHOW);
	}
}

void DlgSceneObject::OnBnClickedChkSceneunits(){
	Active(&m_chkSceneUnits);
}
void DlgSceneObject::OnBnClickedChkRoles(){
	Active(&m_chkRoles);
}
void DlgSceneObject::OnBnClickedChkEvents(){
	Active(&m_chkEvents);
}

void DlgSceneObject::OnLbnSelchangeSceneUnits()
{
	int sels[512];
	int count	=	m_lbSceneUnits.GetSelItems(512, sels);
	std::vector<SelectEditor> eds;
	for(int i=0;i<count;i++){
		int id = m_lbSceneUnits.GetItemData(sels[i]);
		SelectEditor ed;
		ed.Init(m_editors->m_scene3D->GetObjectByID(id));
		eds.push_back(ed);
	}
	m_editors->OnSelectObject(eds);
}
void DlgSceneObject::OnLbnSelchangeRoles()
{
	int sels[512];
	int count	=	m_lbSceneUnits.GetSelItems(512, sels);
	std::vector<SelectEditor> eds;
	for(int i=0;i<count;i++){
		int id = m_lbSceneUnits.GetItemData(sels[i]);
		SelectEditor ed;
		ed.Init(m_editors->m_scene3D->GetObjectByID(id));
		eds.push_back(ed);
	}
	m_editors->OnSelectObject(eds);
}
void DlgSceneObject::OnLbnSelchangeEvents()
{
	int sels[512];
	int count	=	m_lbSceneUnits.GetSelItems(512, sels);
	std::vector<SelectEditor> eds;
	for(int i=0;i<count;i++){
		int id = m_lbSceneUnits.GetItemData(sels[i]);
		SelectEditor ed;
		ed.Init(m_editors->m_scene3D->GetActiveEnv()->GetPointByID(id));
		eds.push_back(ed);
	}
	m_editors->OnSelectObject(eds);
}
