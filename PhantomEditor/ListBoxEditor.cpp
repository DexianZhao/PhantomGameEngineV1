// ListBoxEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "ListBoxEditor.h"
#include "afxdialogex.h"


// CListBoxEditor 对话框

IMPLEMENT_DYNAMIC(CListBoxEditor, CDialogEx)

CListBoxEditor::CListBoxEditor(CWnd* pParent /*=NULL*/)
	: CDialogEx(CListBoxEditor::IDD, pParent)
{

}

CListBoxEditor::~CListBoxEditor()
{
}

void CListBoxEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LIST, m_listBox);
	DDX_Control(pDX, IDC_MAP_SELECT, m_mapBox);
}


BEGIN_MESSAGE_MAP(CListBoxEditor, CDialogEx)
	ON_BN_CLICKED(IDOK, &CListBoxEditor::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CListBoxEditor::OnBnClickedCancel)
END_MESSAGE_MAP()


// CListBoxEditor 消息处理程序


void CListBoxEditor::OnBnClickedOk()
{
	if(m_listBox.IsWindowVisible()){
		m_lists.clear();
		for(int i=0;i<m_listBox.GetCount();i++)
		{
			m_lists.push_back((LPCTSTR)m_listBox.GetItemText(i));
		}
	}
	if(m_mapBox.IsWindowVisible()){
		m_mapsel.clear();
		for(int i=0;i<m_mapBox.GetCount();i++)
		{
			if(m_mapBox.GetSel(i)){
				m_mapsel.push_back(m_mapBox.GetItemData(i));
			}
		}
	}
	CDialogEx::OnOK();
}


void CListBoxEditor::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


BOOL CListBoxEditor::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ShowWindow(SW_SHOW);
	m_listBox.ShowWindow(SW_HIDE);
	m_mapBox.ShowWindow(SW_HIDE);
	if(m_lists.size()>0){
		m_listBox.ShowWindow(SW_SHOW);
		for(int i=0;i<m_lists.size();i++)
			m_listBox.AddItem(m_lists[i].c_str());
	}
	if(m_maps.size()>0){
		m_mapBox.ShowWindow(SW_SHOW);
		for(std::map<int,std::wstring>::iterator it = m_maps.begin();it!=m_maps.end();it++){
			int index = m_mapBox.AddString(it->second.c_str());
			m_mapBox.SetItemData(index, it->first);
			for(int i=0;i<m_mapsel.size();i++)
			{
				if(m_mapsel[i] == it->first){
					m_mapBox.SetSel(index, true);
					break;
				}
			}
		}
	}
	return TRUE;
}
