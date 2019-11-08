/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// ImportEquipDLG.cpp : 实现文件
//

#include "stdafx.h"
#include "phantom-gc.h"
#include "ImportEquipDLG.h"
#include "afxdialogex.h"


// CImportEquipDLG 对话框

IMPLEMENT_DYNAMIC(CImportEquipDLG, CDialogEx)

CImportEquipDLG::CImportEquipDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImportEquipDLG::IDD, pParent)
{

}

CImportEquipDLG::~CImportEquipDLG()
{
}

void CImportEquipDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OBJECT_LIST, m_lbObjects);
	DDX_Control(pDX, IDC_PART_LIST, m_lbParts);
	DDX_Control(pDX, IDC_PART_OBJECTS, m_lbPartObjects);
}


BEGIN_MESSAGE_MAP(CImportEquipDLG, CDialogEx)
	ON_BN_CLICKED(IDC_ADD_IMPORT, &CImportEquipDLG::OnBnClickedAddImport)
	ON_BN_CLICKED(IDC_REMOVE_IMPORT, &CImportEquipDLG::OnBnClickedRemoveImport)
	ON_BN_CLICKED(IDOK, &CImportEquipDLG::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_PART_LIST, &CImportEquipDLG::OnLbnSelchangePartList)
END_MESSAGE_MAP()


// CImportEquipDLG 消息处理程序

//VOID		CImportEquipDLG::SetInitialize(IGeometryGroupData* groupPtr, CDynamicArray<const char*>& objects)
//{
//	this->m_geometryRender	=	groupPtr;
//	m_objects.clear();
//	for(int i=0;i<objects.size();i++)
//	{
//		ImportPartEquipObject ep;
//		ep.objectIndex	=	i;
//		ep.partIndex	=	-1;
//		ep.name			=	objects[i];
//		m_objects.push_back(ep);
//	}
//}


BOOL CImportEquipDLG::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ShowWindow(SW_SHOW);
	RebuildObjectList();
	//for(int i=0;i<m_geometryRender->GetPartsCount();i++)
	//{
	//	int index	=	this->m_lbParts.AddString(W(m_geometryRender->GetPartName(i)));
	//	m_lbParts.SetItemData(index, i);
	//}
	//m_lbParts.SetCurSel(0);
	return TRUE;
}

VOID	CImportEquipDLG::RebuildObjectList()
{
	m_lbObjects.ResetContent();
	for(int i=0;i<m_objects.size();i++)
	{
		if(m_objects[i].partIndex >= 0)
			continue;
		int index	=	m_lbObjects.AddString(W(m_objects[i].name.c_str()));
		m_lbObjects.SetItemData(index, i);
	}
}

VOID CImportEquipDLG::RebuildPartObjects()
{
	if(m_lbParts.GetCurSel() < 0)
		return;
	int partIndex	=	m_lbParts.GetItemData(m_lbParts.GetCurSel());
	this->m_lbPartObjects.ResetContent();
	for(int i=0;i<m_objects.size();i++)
	{
		if(m_objects[i].partIndex != partIndex)
			continue;
		int index	=	m_lbPartObjects.AddString(W(m_objects[i].name.c_str()));
		m_lbPartObjects.SetItemData(index, i);
	}
}

void CImportEquipDLG::OnBnClickedAddImport()
{
	if(m_lbParts.GetCurSel() < 0)
		return;
	int partIndex	=	m_lbParts.GetItemData(m_lbParts.GetCurSel());
	//
	int sels[10000];
	int count	=	this->m_lbObjects.GetSelItems(10000, sels);
	std::vector<int> selects;
	for(int i=0;i<count;i++)
	{
		int index	=	m_lbObjects.GetItemData(sels[i]);
		m_objects[index].partIndex	=	partIndex;
		selects.push_back(index);
	}
	RebuildObjectList();
	RebuildPartObjects();
	for(int i=0;i<selects.size();i++)
	{
		for(int j=0;j<m_lbPartObjects.GetCount();j++)
		{
			if(m_lbPartObjects.GetItemData(j) == selects[i])
			{
				m_lbPartObjects.SetSel(j, true);
				break;
			}
		}
	}
}


void CImportEquipDLG::OnBnClickedRemoveImport()
{
	int sels[10000];
	int count	=	this->m_lbPartObjects.GetSelItems(10000, sels);
	std::vector<int> selects;
	for(int i=0;i<count;i++)
	{
		int index	=	m_lbPartObjects.GetItemData(sels[i]);
		m_objects[index].partIndex	=	-1;
		selects.push_back(index);
	}
	RebuildObjectList();
	RebuildPartObjects();
	for(int i=0;i<selects.size();i++)
	{
		for(int j=0;j<m_lbObjects.GetCount();j++)
		{
			if(m_lbObjects.GetItemData(j) == selects[i])
			{
				m_lbObjects.SetSel(j, true);
				break;
			}
		}
	}
}


void CImportEquipDLG::OnBnClickedOk()
{
	//m_returns.clear();
	for(int i=0;i<m_objects.size();i++)
	{
		if(m_objects[i].partIndex >= 0)
		{
			//ImportEquip imp;
			//imp.objectIndex	=	i;
			//imp.partIndex	=	m_objects[i].partIndex;
			//m_returns.push_back(imp);
		}
	}
	CDialogEx::OnOK();
}


void CImportEquipDLG::OnLbnSelchangePartList()
{
	RebuildPartObjects();
}
