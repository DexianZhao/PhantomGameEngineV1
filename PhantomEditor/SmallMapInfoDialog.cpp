// SmallMapInfoDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include ".\SmallMapInfoDialog.h"

CSmallMapInfoDialog*	g_pSmallMapInfo = 0;
CSmallMapInfoDialog*	GetSmallMapInfoDlg()
{
	return g_pSmallMapInfo;
}

// CSmallMapInfoDialog 对话框

IMPLEMENT_DYNAMIC(CSmallMapInfoDialog, CDialogBase)
CSmallMapInfoDialog::CSmallMapInfoDialog(CWnd* pParent /*=NULL*/)
	: CDialogBase(CSmallMapInfoDialog::IDD, pParent)
{
	g_pSmallMapInfo	=	this;
	m_scene3D		=	0;
}

CSmallMapInfoDialog::~CSmallMapInfoDialog()
{
}

void CSmallMapInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SMALL_MAP_ENABLE, m_chkSmallMapEnable);
	DDX_Control(pDX, IDC_SMALL_MAP_RENDER_OBJECTS, m_chkSmallMapRenderObjects);
	DDX_Control(pDX, IDC_OBJECT_RENDERS, m_cbIncludes);
	DDX_Control(pDX, IDC_OBJECT_RENDER_MAP, m_lbIncludes);
	DDX_Control(pDX, IDC_ADD_INCLUDE, m_btAddInclude);
	DDX_Control(pDX, IDC_REMOVE_INCLUDE, m_btRemoveInclude);
	DDX_Control(pDX, IDC_SMALL_MAP_PLANE_DISTANCE, m_edSmalMapPlaneDistance);
	DDX_Control(pDX, IDC_SMALL_MAP_REBUILD_INTERVAL, m_edSmallMapRebuildInterval);
}


BEGIN_MESSAGE_MAP(CSmallMapInfoDialog, CDialogBase)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_ADD_INCLUDE, OnBnClickedAddInclude)
	ON_BN_CLICKED(IDC_REMOVE_INCLUDE, OnBnClickedRemoveInclude)
	ON_BN_CLICKED(IDC_ALL_ALL, OnBnClickedAllAll)
	ON_BN_CLICKED(IDC_SMALL_MAP_ENABLE, OnBnClickedSmallMapEnable)
	ON_BN_CLICKED(IDC_SMALL_MAP_RENDER_OBJECTS, OnBnClickedSmallMapRenderObjects)
	ON_BN_CLICKED(IDC_SET_MPD, OnBnClickedSetMpd)
	ON_BN_CLICKED(IDC_SET_MRI, OnBnClickedSetMri)
END_MESSAGE_MAP()


// CSmallMapInfoDialog 消息处理程序

BOOL CSmallMapInfoDialog::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	{
		ITableReader* table = LoadTableFile(0, "./geometry_types.editor");
		if(!table)return false;
		for(int i=0;i<table->getNumLine();i++)
		{
			const char* id		=	table->getLineItem(i, "value");
			const char* name	=	table->getLineItem(i, "name");
			this->m_geometryTypes[atoi(id)] = name;
			m_cbIncludes.SetItemData(m_cbIncludes.AddString(W(name)), atoi(id));
		}
		table->Release();
	}
	//
	AddTool(m_chkSmallMapEnable, Language(L"开关小地图动态创建模式"));
	AddTool(m_chkSmallMapRenderObjects, Language(L"是否渲染所有的物体，包括角色模型、NPC模型等等，如果使用此项则比较耗效率"));
	AddTool(m_cbIncludes, Language(L"小地图渲染时候的，模型类型，在geometry_types.editor中定义"));
	AddTool(m_lbIncludes, Language(L"当前参与小地图渲染的模型类型"));
	AddTool(m_btAddInclude, Language(L"添加一个模型类型支持"));
	AddTool(m_btRemoveInclude, Language(L"删除一个模型类型支持"));
	AddTool(m_edSmalMapPlaneDistance, Language(L"在编辑器里面预设一个小地图大小,而在运行时可以调整这个参数, 对小地图的显示大小进行调整."));
	AddTool(m_edSmallMapRebuildInterval, Language(L"小地图重新构造时间间隔"));
	return TRUE;
}

VOID CSmallMapInfoDialog::OnChangeScene()
{
	if(m_scene3D)
	{
		m_chkSmallMapEnable.SetCheck(m_scene3D->GetSmallMapEnable());
		m_chkSmallMapRenderObjects.SetCheck(m_scene3D->GetSmallMapRenderAllObjects());
		this->RebuildIncludes();
		wchar_t buf[128];
		swprintf(buf, L"%.03f", m_scene3D->GetSmallMapRebuildInterval());
		m_edSmallMapRebuildInterval.SetWindowText(buf);
		swprintf(buf, L"%d", m_scene3D->GetSmallMapOrthoWidth());
		m_edSmalMapPlaneDistance.SetWindowText(buf);
	}
}

void CSmallMapInfoDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogBase::OnShowWindow(bShow, nStatus);
}

void	CSmallMapInfoDialog::RebuildIncludes()
{
	if(!m_scene3D)
		return;
	this->m_lbIncludes.ResetContent();
	unsigned int nTypes	=	m_scene3D->GetSmallMapGeometryTypes();
	for(int i=0;i<32;i++)
	{
		unsigned int t = 1 << i;
		if((t & nTypes) > 0)
			m_lbIncludes.SetItemData(m_lbIncludes.AddString(W(m_geometryTypes[t].c_str())), t);
	}
}

void CSmallMapInfoDialog::OnBnClickedAddInclude()
{
	if(!m_scene3D)
		return;
	if(m_cbIncludes.GetCurSel() < 0)return;
	unsigned int nT	=	m_cbIncludes.GetItemData(m_cbIncludes.GetCurSel());
	m_scene3D->SetSmallMapGeometryTypes(m_scene3D->GetSmallMapGeometryTypes() | nT);
	RebuildIncludes();
}

void CSmallMapInfoDialog::OnBnClickedRemoveInclude()
{
	if(!m_scene3D)
		return;
	if(m_lbIncludes.GetCurSel() < 0)return;
	unsigned int nTypes	=	m_scene3D->GetSmallMapGeometryTypes();
	INT sels[128];
	int count = m_lbIncludes.GetSelItems(128, sels);
	for(int i=0;i<count;i++)
	{
		unsigned int type = m_lbIncludes.GetItemData(sels[i]);
		if((nTypes & type) > 0)
			nTypes ^= type;
	}
	m_scene3D->SetSmallMapGeometryTypes(nTypes);
	RebuildIncludes();
}

void CSmallMapInfoDialog::OnBnClickedAllAll()
{
	if(!m_scene3D)return;
	m_scene3D->SetSmallMapGeometryTypes(0xFFFFFFFF);
	RebuildIncludes();
}

void CSmallMapInfoDialog::OnBnClickedSmallMapEnable()
{
	if(!m_scene3D)return;
	m_scene3D->SetSmallMapEnable(this->m_chkSmallMapEnable.GetCheck());
}

void CSmallMapInfoDialog::OnBnClickedSmallMapRenderObjects()
{
	if(!m_scene3D)return;
	m_scene3D->SetSmallMapRenderAllObjects(this->m_chkSmallMapRenderObjects.GetCheck());
}

void CSmallMapInfoDialog::OnBnClickedSetMpd()
{
	if(!m_scene3D)return;
	CString str;
	this->m_edSmalMapPlaneDistance.GetWindowText(str);
	m_scene3D->SetSmallMapOrthoWidth(_wtoi(str));
}

void CSmallMapInfoDialog::OnBnClickedSetMri()
{
	if(!m_scene3D)return;
	CString str;
	this->m_edSmallMapRebuildInterval.GetWindowText(str);
	m_scene3D->SetSmallMapRebuildInterval(_wtof(str));
}
