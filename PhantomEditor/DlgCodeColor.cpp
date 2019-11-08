/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// DlgCodeColor.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgCodeColor.h"
#include "afxdialogex.h"
#include "resource.h"
#include "CodeEditor.h"
#include "GCMFCHelper.h"
#include "GCLanguage.h"

// CDlgCodeColor 对话框

IMPLEMENT_DYNAMIC(CDlgCodeColor, CDialogEx)

CDlgCodeColor::CDlgCodeColor(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCodeColor::IDD, pParent)
{

}

CDlgCodeColor::~CDlgCodeColor()
{
}

void CDlgCodeColor::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST2, m_colorlist);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON1, m_browseColor);
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCodeColor, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST2, &CDlgCodeColor::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON1, &CDlgCodeColor::OnBnClickedMfccolorbutton1)
	ON_BN_CLICKED(IDOK, &CDlgCodeColor::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgCodeColor 消息处理程序


void CDlgCodeColor::OnLbnSelchangeList2()
{
	int sel = this->m_colorlist.GetCurSel();
	int index = m_colorlist.GetItemData(sel);
	this->m_browseColor.SetColor(CodeEditor::m_maskColor[index].c);
}

extern VOID	RefreshAllColors();
void CDlgCodeColor::OnBnClickedMfccolorbutton1()
{
	int sel = this->m_colorlist.GetCurSel();
	int index = m_colorlist.GetItemData(sel);
	CodeEditor::m_maskColor[index].c = this->m_browseColor.GetColor();
	RefreshAllColors();
}


BOOL CDlgCodeColor::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"关键字")), MaskText::MaskType_Keywords);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"符号")), MaskText::MaskType_Token);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"变量")), MaskText::MaskType_Variant);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"方法")), MaskText::MaskType_Method);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"文本")), MaskText::MaskType_Text);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"注释")), MaskText::MaskType_Comment);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"枚举")), MaskText::MaskType_Enum);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"数字")), MaskText::MaskType_Number);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"幻影API函数")), MaskText::MaskType_PhantomAPI);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"lua的API函数")), MaskText::MaskType_LuaAPI);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"正常文本颜色")), MaskText::MaskType_Normal);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"背景颜色")), MaskText::MaskType_BackColor);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"行文本颜色")), MaskText::MaskType_LineText);
	this->m_colorlist.SetItemData(m_colorlist.AddString(Language(L"行背景颜色")), MaskText::MaskType_LineBack);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgCodeColor::OnBnClickedOk()
{
	for(int i=0;i<MaskText::MaskType_Count;i++){
		MaskText::Write(i, CodeEditor::m_maskColor[i].c);
	}
	CDialogEx::OnOK();
}
