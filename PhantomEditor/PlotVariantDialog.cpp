// PlotVariantDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "PlotVariantDialog.h"
#include "afxdialogex.h"
#include "GCGameProject.h"


// CPlotVariantDialog 对话框

IMPLEMENT_DYNAMIC(CPlotVariantDialog, CDialogBase)

CPlotVariantDialog::CPlotVariantDialog(CWnd* pParent /*=NULL*/)
	: CDialogBase(CPlotVariantDialog::IDD, pParent)
{
	m_actionPtr	=	0;
	m_gamePtr	=	0;
}

CPlotVariantDialog::~CPlotVariantDialog()
{
}

void CPlotVariantDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ICON_LIST, m_lists);
	DDX_Control(pDX, IDC_INSERT_VARIANT, m_btVariant);
	DDX_Control(pDX, IDC_REMOVE_SELECT, m_btRemove);
}


BEGIN_MESSAGE_MAP(CPlotVariantDialog, CDialogBase)
	ON_BN_CLICKED(IDOK, &CPlotVariantDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPlotVariantDialog::OnBnClickedCancel)
	ON_LBN_SELCHANGE(IDC_ICON_LIST, &CPlotVariantDialog::OnLbnSelchangeIconList)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_INSERT_VARIANT, &CPlotVariantDialog::OnPlotcallInsertvariantset)
	ON_BN_CLICKED(IDC_REMOVE_SELECT, &CPlotVariantDialog::OnPlotcallRemove)
END_MESSAGE_MAP()

VOID CPlotVariantDialog::SetActionPtr(CPlotAction* act, CPlotVariant* ptr)
{
	this->m_actionPtr	=	act;
	int sel	=	m_lists.GetCurSel();
	this->m_lists.ResetContent();
	//
	for(int i=0;i<m_actionPtr->GetVariantCount();i++)
	{
		CPlotVariant* varPtr	=	m_actionPtr->GetVariantPtr(i);
		//
		std::wstring str;
		str	=	varPtr->m_name.strW();
		wchar_t buf[1024];
		str	+=	L"(";
		Text text	=	varPtr->m_value.getText();
		if(text.size() > 30)
		{
			text.texts.SetArrayCount(30);
			text.texts[27] = '.';
			text.texts[28] = '.';
			text.texts[29] = '.';
		}
		swprintf(buf, L"%d,%.01f,%s", varPtr->m_value.getInt(), varPtr->m_value.getDouble(), W(text).buf);
		str	+=	buf;
		swprintf(buf, L"[%d,%d,%d,%d]", varPtr->m_value.m_i4.left, varPtr->m_value.m_i4.top, varPtr->m_value.m_i4.right, varPtr->m_value.m_i4.bottom);
		str	+=	buf;
		swprintf(buf, L"[%.01f,%01f,%01f,%01f]", varPtr->m_value.m_f4.x, varPtr->m_value.m_f4.y, varPtr->m_value.m_f4.z, varPtr->m_value.m_f4.w);
		str	+=	buf;
		str	+=	L")";
		m_lists.AddString(str.c_str(), 0);
		if(ptr == m_actionPtr->GetVariantPtr(i))
			sel	=	i;
	}
	if(sel >= m_lists.GetCount())
		sel	=	m_lists.GetCount() - 1;
	m_lists.SetCurSel(sel);
}

void CPlotVariantDialog::OnLbnSelchangeIconList()
{
	CPlotVariant* var	=	m_actionPtr->GetVariantPtr(m_lists.GetCurSel());
	if(var)
	{
		m_gamePtr->SetPropertyVariant(m_actionPtr, var);
	}
}


void CPlotVariantDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogBase::OnSize(nType, cx, cy);
	CRect rc;
	GetClientRect(&rc);
	if(m_lists.m_hWnd)
	{
		const int height	=	16;
		m_lists.MoveWindow(&CRect(0, height, rc.right, rc.bottom));
	}
}


BOOL CPlotVariantDialog::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&CRect(0, 0, rc.right, 16), &CBrush(RGB(192,192,192)));
	return TRUE;
}


void CPlotVariantDialog::OnPlotcallInsertvariantset()
{
	if(!this->m_actionPtr)
		return;
	m_actionPtr->InsertVariant(L"variant");
	this->SetActionPtr(m_actionPtr);
	m_gamePtr->SetModifyed();
}


void CPlotVariantDialog::OnPlotcallRemove()
{
	if(m_gamePtr->IsGameRuntime())
	{
		MessageBox(Language(L"游戏运行模式下不能删除任何对象"), Language(L"提示"), MB_OK);
		return;
	}
	if(m_actionPtr)
	{
		m_actionPtr->RemoveVariant(m_lists.GetCurSel());
		this->SetActionPtr(m_actionPtr);
		m_gamePtr->SetModifyed();
	}
}


BOOL CPlotVariantDialog::OnInitDialog()
{
	CDialogBase::OnInitDialog();

	CRect rectDummy;
	GetClientRect(&rectDummy);
	m_lists.MoveWindow(&rectDummy);//.Create(rectDummy, this))
	m_lists.m_bShowIndex	=	true;
	m_lists.SetIconRes(IDB_PLOT_CALL_ICON);
	m_lists.SetFont(GetFont());
	//
	m_imageButton.LoadFromResource(AfxGetResourceHandle(), IDB_PROPERTIES_PLOTCALL);
	CPoint pos(10, 0);
	this->m_btVariant.SetImage(this->m_imageButton, 2, -1, 0, &pos);
	this->m_btRemove.SetImage(this->m_imageButton, 2, -1, 3, &pos);
	AddTool(m_btVariant, Language(L"插入一个变量"));
	AddTool(m_btRemove, Language(L"删除一个变量"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPlotVariantDialog::OnBnClickedOk()
{
}

void CPlotVariantDialog::OnBnClickedCancel()
{
}
