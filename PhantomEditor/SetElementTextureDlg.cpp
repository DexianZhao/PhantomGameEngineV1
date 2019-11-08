/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// SetElementTextureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "SetElementTextureDlg.h"
#include "afxdialogex.h"


// CSetElementTextureDlg 对话框

IMPLEMENT_DYNAMIC(CSetElementTextureDlg, CDialogEx)

CSetElementTextureDlg::CSetElementTextureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetElementTextureDlg::IDD, pParent)
{

}

CSetElementTextureDlg::~CSetElementTextureDlg()
{
}

void CSetElementTextureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ELE_TEX, m_eleTex);
}


BEGIN_MESSAGE_MAP(CSetElementTextureDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSetElementTextureDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSetElementTextureDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

// CSetElementTextureDlg 消息处理程序

int	SortElements(const void* p1, const void* p2)
{
	ElementTexture* e1	=	(ElementTexture*)p1;
	ElementTexture* e2	=	(ElementTexture*)p2;
	return wcsicmp(e1->param.name.c_str(), e2->param.name.c_str());
}

int	SortTextures(const void* p1, const void* p2)
{
	std::wstring* e1	=	(std::wstring*)p1;
	std::wstring* e2	=	(std::wstring*)p2;
	return wcsicmp(e1->c_str(), e2->c_str());
}

VOID CSetElementTextureDlg::SetElements(std::vector<ElementIn>& elements, std::vector<std::wstring>& textures)
{
	this->m_textures			=	textures;
	for(int i=0;i<elements.size();i++)
	{
		ElementTexture ele;
		memset(&ele, 0, sizeof(ElementTexture));
		ele.param	=	elements[i];
		m_elements.push_back(ele);
	}
	qsort(&m_elements.at(0), m_elements.size(), sizeof(ElementTexture), SortElements);
	qsort(&m_textures.at(0), m_textures.size(), sizeof(std::wstring), SortTextures);
}

BOOL CSetElementTextureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ShowWindow(SW_SHOW);
	CMFCPropertyGridProperty* group1			= new CMFCPropertyGridProperty(L"SetTextures");
	for(int i=0;i<this->m_elements.size();i++)
	{
		const wchar_t* szTexture		=	m_textures.at(i % m_textures.size()).c_str();
		CMFCPropertyGridProperty* prop	=	new CMFCPropertyGridProperty(m_elements[i].param.name.c_str(), (_variant_t)szTexture, 0, 0, 0);
		m_elements[i].prop				=	prop;
		group1->AddSubItem(prop);
		prop->AllowEdit(false);
		for(int j=0;j<this->m_textures.size();j++)
		{
			prop->AddOption(this->m_textures[j].c_str());
		}
	}
	m_eleTex.AddProperty(group1);
	m_eleTex.SetNameWidth(150);
	this->ShowWindow(SW_SHOW);
	return TRUE;
}


void CSetElementTextureDlg::OnBnClickedOk()
{
	for(int i=0;i<m_elements.size();i++)
	{
		m_elements[i].str	=	(const wchar_t*)_bstr_t(m_elements[i].prop->GetValue());
	}
	CDialogEx::OnOK();
}


void CSetElementTextureDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}
