//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   ShadowMapInfoDialog.cpp
	软件名称   :   幻影游戏引擎

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	
	Copyright 2009-2016 Zhao Dexian
	
	()
	
	
	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


// ShadowMapInfoDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include ".\ShadowMapInfoDialog.h"

// CShadowMapInfoDialog 对话框

CShadowMapInfoDialog*	g_pShadowMapInfoDialog	=	0;
IMPLEMENT_DYNAMIC(CShadowMapInfoDialog, CDialogBase)
CShadowMapInfoDialog::CShadowMapInfoDialog(CWnd* pParent /*=NULL*/)
	: CDialogBase(CShadowMapInfoDialog::IDD, pParent)
{
	m_scene3D	=	0;
}

CShadowMapInfoDialog::~CShadowMapInfoDialog()
{
}

void CShadowMapInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHADOW_LEVEL_SELECT, m_cbShadowLevel);
	DDX_Control(pDX, IDC_TEXTURE_SIZE, m_cbTextureSize);
	DDX_Control(pDX, IDC_VIEW_SIZE, m_cbViewSize);
	DDX_Control(pDX, IDC_RENDER_TERRAIN_SHADOW, m_chkRenderTerrainShadow);
	DDX_Control(pDX, IDC_RENDER_OBJECT_SHADOW, m_chkRenderObjectShadow);
	DDX_Control(pDX, IDC_RENDER_GRASS_SHADOW, m_chkRenderGrassShadow);
	DDX_Control(pDX, IDC_MIN_DISTANCE_VISIBLE, m_edMinVisibleDistance);
	DDX_Control(pDX, IDC_GRASS_AFFECT_SHADOW, m_chkGrassAffectShadow);
	DDX_Control(pDX, IDC_SHADOW_TO_TERRAIN, m_chkShadowToTerrain);
	DDX_Control(pDX, IDC_SINGLE_SHADOWMAP_ONLY, m_chkSingleShadowMapOnly);
	DDX_Control(pDX, IDC_OBJECT_AFFECT_BY_SHADOW, m_chkObjectAffectByShadow);
	DDX_Control(pDX, IDC_WATER_AFFECT_BY_SHADOW, m_chkWaterAffectBy);
	DDX_Control(pDX, IDC_ToSceneEpsilon, m_edToSceneEpsilon);
	DDX_Control(pDX, IDC_ToShadowMapEpsilon, m_edToShadowMapEpsilon);
	DDX_Control(pDX, IDC_ShadowDensity, m_edShadowDensity);
	DDX_Control(pDX, IDC_SoftShadowAmount, m_edSoftShadowAmount);
	DDX_Control(pDX, IDC_SOFT_SHADOW_BLENDS, m_cbSoftShadowBlends);
	DDX_Control(pDX, IDC_SINGLE_SHADOWMAP_ONLY2, m_btShadowEnable);
	DDX_Control(pDX, IDC_ObjectToShadowMap, m_edObjectToShadowMap);
}


BEGIN_MESSAGE_MAP(CShadowMapInfoDialog, CDialogBase)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_SET_MIN_DISTANCE, OnBnClickedSetMinDistance)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_TEXTURE_SIZE, OnCbnSelchangeTextureSize)
	ON_BN_CLICKED(IDC_RENDER_TERRAIN_SHADOW, OnBnClickedRenderTerrainShadow)
	ON_BN_CLICKED(IDC_RENDER_OBJECT_SHADOW, OnBnClickedRenderObjectShadow)
	ON_BN_CLICKED(IDC_RENDER_GRASS_SHADOW, OnBnClickedRenderGrassShadow)
	ON_CBN_SELCHANGE(IDC_SHADOW_LEVEL_SELECT, OnCbnSelchangeShadowLevelSelect)
	ON_BN_CLICKED(IDC_SET_VIEW_SIZE, OnBnClickedSetViewSize)
	ON_BN_CLICKED(IDC_GRASS_AFFECT_SHADOW, OnBnClickedGrassAffectShadow)
	ON_BN_CLICKED(IDC_SHADOW_TO_TERRAIN, OnBnClickedShadowToTerrain)
	ON_BN_CLICKED(IDC_SINGLE_SHADOWMAP_ONLY, OnBnClickedSingleShadowmapOnly)
	ON_BN_CLICKED(IDC_OBJECT_AFFECT_BY_SHADOW, OnBnClickedObjectAffectByShadow)
	ON_BN_CLICKED(IDC_WATER_AFFECT_BY_SHADOW, OnBnClickedWaterAffectByShadow)
	ON_BN_CLICKED(IDC_SetToSceneEpsilon, OnBnClickedSettosceneepsilon)
	ON_BN_CLICKED(IDC_SetToShadowMapEpsilon, OnBnClickedSettoshadowmapepsilon)
	ON_BN_CLICKED(IDC_SetShadowDensity, OnBnClickedSetshadowdensity)
	ON_BN_CLICKED(IDC_SetSoftShadowAmount, OnBnClickedSetsoftshadowamount)
	ON_CBN_SELCHANGE(IDC_SOFT_SHADOW_BLENDS, OnCbnSelchangeSoftShadowBlends)
	ON_BN_CLICKED(IDC_SINGLE_SHADOWMAP_ONLY2, OnBnClickedSingleShadowmapOnly2)
	ON_BN_CLICKED(IDC_SetObjectToShadowMap, OnBnClickedSetobjecttoshadowmap)
END_MESSAGE_MAP()


// CShadowMapInfoDialog 消息处理程序

void CShadowMapInfoDialog::OnBnClickedOk()
{
}

void CShadowMapInfoDialog::OnBnClickedCancel()
{
	OnCancel();
}

BOOL CShadowMapInfoDialog::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	for(int i=0;i<MAX_SHADOWMAP_LEVEL;i++)
	{
		wchar_t buf[128];
		swprintf(buf, L"Level-%d", i);
		this->m_cbShadowLevel.SetItemData(m_cbShadowLevel.AddString(buf), i);
	}
	this->m_cbTextureSize.SetItemData(m_cbTextureSize.AddString(Language(L"512 x 512")), 512);
	this->m_cbTextureSize.SetItemData(m_cbTextureSize.AddString(Language(L"1024 x 1024，一般")), 1024);
	this->m_cbTextureSize.SetItemData(m_cbTextureSize.AddString(Language(L"2048 x 2048")), 2048);
	this->m_cbTextureSize.SetItemData(m_cbTextureSize.AddString(Language(L"4096 x 4096，要求配置高。")), 4096);
	this->m_cbViewSize.SetWindowText(L"1.0f");
	AddTool(m_cbShadowLevel, Language(L"选择阴影层级，目前仅提供两层贴图，如果想得到更高的清晰度，请使用2048x2048贴图，不过相应的也会比较消耗渲染开销。"));
	AddTool(m_cbTextureSize, Language(L"设置阴影图大小，值越大越消耗渲染开销"));
	AddTool(m_cbViewSize, Language(L"视口大小比例，主要是设置每个层的阴影所涉及的范围"));
	AddTool(m_chkRenderTerrainShadow, Language(L"是否渲染地形的阴影。"));
	//
	AddTool(m_chkRenderObjectShadow, Language(L"是否渲染物体的阴影"));
	AddTool(m_chkRenderGrassShadow, Language(L"是否渲染草地阴影"));
	AddTool(m_edMinVisibleDistance, Language(L"设置最小可见范围，在这个范围内的物体都会显示"));
	AddTool(m_chkGrassAffectShadow, Language(L"草地是否受到阴影的影响。"));
	AddTool(m_chkShadowToTerrain, Language(L"地形是否受到阴影的影响。"));
	AddTool(m_chkSingleShadowMapOnly, Language(L"是否只使用单张阴影贴图，如果使用单张则比较节省渲染开销，可以在2.5D视角中使用。"));
	AddTool(m_chkObjectAffectByShadow, Language(L"物体是否受到阴影影响，需要在模型属性编辑其中,单独指定的模型才会受阴影影响。"));
	AddTool(m_chkWaterAffectBy, Language(L"水面是否受到阴影影响。"));
	AddTool(m_edToSceneEpsilon, Language(L"当阴影渲染到场景的时候的精度，此精度必须得大于0，而且草地和水面、地形都受此参数影响。"));
	AddTool(m_edToShadowMapEpsilon, Language(L"当场景渲染到阴影贴图时候的精度。"));
	AddTool(m_edShadowDensity, Language(L"阴影浓度，0~1之间。"));
	AddTool(m_btShadowEnable, Language(L"阴影开关。"));
	wchar_t buf[128];
	for(int i=0;i<=5;i++)
		m_cbSoftShadowBlends.SetItemData(m_cbSoftShadowBlends.AddString(_itow(i, buf, 10)), i);
	return TRUE;
}

void CShadowMapInfoDialog::OnBnClickedSetMinDistance()
{
	if(!m_scene3D)return;
	CString strN;
	this->m_edMinVisibleDistance.GetWindowText(strN);
	m_scene3D->SetShadowMapNVD(To3DValue(_wtof(strN)));
}

VOID CShadowMapInfoDialog::OnChangeScene()
{
	if(m_scene3D)
	{
		for(int i=0;i<m_cbTextureSize.GetCount();i++)
		{
			int tsize	=	m_cbTextureSize.GetItemData(i);
			if(tsize == m_scene3D->GetShadowMapTextureSize())
			{
				m_cbTextureSize.SetCurSel(i);
				break;
			}
		}
		//
		wchar_t buf[128];
		swprintf(buf, L"%.01f", ToMeter(m_scene3D->GetShadowMapNVD()));
		m_edMinVisibleDistance.SetWindowText(buf);
		this->m_chkRenderTerrainShadow.SetCheck((m_scene3D->GetShadowRenderTypes() & SHADOW_RENDER_TERRAINS) > 0);
		this->m_chkRenderObjectShadow.SetCheck((m_scene3D->GetShadowRenderTypes() & SHADOW_RENDER_OBJECTS) > 0);
		this->m_chkRenderGrassShadow.SetCheck((m_scene3D->GetShadowRenderTypes() & SHADOW_RENDER_GRASS) > 0);
		this->m_chkGrassAffectShadow.SetCheck((m_scene3D->GetShadowRenderTypes() & SHADOW_RENDER_GRASS_A_SHADOW) > 0);
		this->m_chkShadowToTerrain.SetCheck((m_scene3D->GetShadowRenderTypes() & SHADOW_RENDER_TERRAIN_A_SHADOW) > 0);
		m_chkSingleShadowMapOnly.SetCheck(m_scene3D->IsSingleShadowMap());
		m_chkObjectAffectByShadow.SetCheck((m_scene3D->GetShadowRenderTypes() & SHADOW_RENDER_OBJECT_A_SHADOW) > 0);
		m_chkWaterAffectBy.SetCheck((m_scene3D->GetShadowRenderTypes() & SHADOW_RENDER_WATER_A_SHADOW) > 0);
		m_btShadowEnable.SetCheck(m_scene3D->IsShowShadow());
		//
		swprintf(buf, L"%.08f", m_scene3D->GetShadowMapSceneEpsilon());
		this->m_edToSceneEpsilon.SetWindowText(buf);
		//
		swprintf(buf, L"%.08f", m_scene3D->GetSceneToShadowMapEpsilon());
		this->m_edToShadowMapEpsilon.SetWindowText(buf);
		//
		swprintf(buf, L"%.03f", m_scene3D->GetShadowDensity());
		this->m_edShadowDensity.SetWindowText(buf);
		//
		swprintf(buf, L"%.03f", m_scene3D->GetShadowMapObjectEpsilon());
		this->m_edObjectToShadowMap.SetWindowText(buf);
		//
		swprintf(buf, L"%.03f", m_scene3D->GetSoftShadowAmount());
		this->m_edSoftShadowAmount.SetWindowText(buf);
		for(int i=0;i<m_cbSoftShadowBlends.GetCount();i++)
			if(m_cbSoftShadowBlends.GetItemData(i) == m_scene3D->GetSoftShadowBlendCount())
				m_cbSoftShadowBlends.SetCurSel(i);
	}
}

void CShadowMapInfoDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogBase::OnShowWindow(bShow, nStatus);
}

void CShadowMapInfoDialog::OnCbnSelchangeTextureSize()
{
	if(!m_scene3D)return;
	if(this->m_cbTextureSize.GetCurSel() < 0)return;
	m_scene3D->SetShadowMapTextureSize(m_cbTextureSize.GetItemData(m_cbTextureSize.GetCurSel()));
	m_scene3D->CommitShadowModes();
}

void CShadowMapInfoDialog::OnBnClickedRenderTerrainShadow()
{
	if(!m_scene3D)return;
	INT nType = 0;
	if(this->m_chkRenderTerrainShadow.GetCheck())nType	|=	SHADOW_RENDER_TERRAINS;
	if(this->m_chkRenderObjectShadow.GetCheck())nType	|=	SHADOW_RENDER_OBJECTS;
	if(this->m_chkRenderGrassShadow.GetCheck())nType	|=	SHADOW_RENDER_GRASS;
	if(this->m_chkGrassAffectShadow.GetCheck())nType	|=	SHADOW_RENDER_GRASS_A_SHADOW;
	if(this->m_chkShadowToTerrain.GetCheck())nType		|=	SHADOW_RENDER_TERRAIN_A_SHADOW;
	if(this->m_chkObjectAffectByShadow.GetCheck())nType	|=	SHADOW_RENDER_OBJECT_A_SHADOW;
	if(this->m_chkWaterAffectBy.GetCheck())nType		|=	SHADOW_RENDER_WATER_A_SHADOW;
	m_scene3D->SetShadowRenderTypes(nType);
}

void CShadowMapInfoDialog::OnBnClickedRenderObjectShadow()
{
	OnBnClickedRenderTerrainShadow();
}

void CShadowMapInfoDialog::OnBnClickedRenderGrassShadow()
{
	OnBnClickedRenderTerrainShadow();
}

void CShadowMapInfoDialog::OnCbnSelchangeShadowLevelSelect()
{
	if(!m_scene3D)return;
	if(this->m_cbShadowLevel.GetCurSel() < 0)return;
	wchar_t buf[128];
	swprintf(buf, L"%.05f", m_scene3D->GetShadowMapProportion(m_cbShadowLevel.GetItemData(m_cbShadowLevel.GetCurSel())));
	this->m_cbViewSize.SetWindowText(buf);
}

void CShadowMapInfoDialog::OnBnClickedSetViewSize()
{
	if(!m_scene3D)return;
	if(this->m_cbShadowLevel.GetCurSel() < 0)return;
	CString str;
	this->m_cbViewSize.GetWindowText(str);
	float f	=	_wtof(str);
	if(f > 2.0f || f < 0.01f)
	{
		MessageBox(Language(L"不能大于2或者小于0.01"));
		return;
	}
	m_scene3D->SetShadowMapProportion(m_cbShadowLevel.GetItemData(m_cbShadowLevel.GetCurSel()), f);
}

CShadowMapInfoDialog*	GetShadowMapInfoDialog()
{
	return g_pShadowMapInfoDialog;
}

void CShadowMapInfoDialog::OnBnClickedGrassAffectShadow()
{
	OnBnClickedRenderTerrainShadow();
}

void CShadowMapInfoDialog::OnBnClickedShadowToTerrain()
{
	OnBnClickedRenderTerrainShadow();
}

void CShadowMapInfoDialog::OnBnClickedSingleShadowmapOnly()
{
	if(!m_scene3D)return;
	m_scene3D->SetSingleShadowMap(m_chkSingleShadowMapOnly.GetCheck());
	m_scene3D->CommitShadowModes();
}

void CShadowMapInfoDialog::OnBnClickedObjectAffectByShadow()
{
	OnBnClickedRenderTerrainShadow();
}

void CShadowMapInfoDialog::OnBnClickedWaterAffectByShadow()
{
	OnBnClickedRenderTerrainShadow();
}

void CShadowMapInfoDialog::OnBnClickedSettosceneepsilon()
{
	if(!m_scene3D)return;
	CString str;
	this->m_edToSceneEpsilon.GetWindowText(str);
	m_scene3D->SetShadowMapSceneEpsilon(_wtof(str));
}

void CShadowMapInfoDialog::OnBnClickedSettoshadowmapepsilon()
{
	if(!m_scene3D)return;
	CString str;
	this->m_edToShadowMapEpsilon.GetWindowText(str);
	m_scene3D->SetSceneToShadowMapEpsilon(_wtof(str));
}

void CShadowMapInfoDialog::OnBnClickedSetshadowdensity()
{
	if(!m_scene3D)return;
	CString str;
	this->m_edShadowDensity.GetWindowText(str);
	m_scene3D->SetShadowDensity(_wtof(str));
}

void CShadowMapInfoDialog::OnBnClickedSetsoftshadowamount()
{
	if(!m_scene3D)return;
	CString str;
	this->m_edSoftShadowAmount.GetWindowText(str);
	float f = _wtof(str);
	f	=	clamp(f, -10.0f, 10.0f);
	m_scene3D->SetSoftShadowAmount(f);
}

void CShadowMapInfoDialog::OnCbnSelchangeSoftShadowBlends()
{
	if(!m_scene3D)return;
	if(m_cbSoftShadowBlends.GetCurSel() < 0)return;
	m_scene3D->SetSoftShadowBlendCount(m_cbSoftShadowBlends.GetItemData(m_cbSoftShadowBlends.GetCurSel()));
}

void CShadowMapInfoDialog::OnBnClickedSingleShadowmapOnly2()
{
	if(!m_scene3D)return;
	m_scene3D->SetShowShadow(m_btShadowEnable.GetCheck());
	m_scene3D->CommitShadowModes();
}

void CShadowMapInfoDialog::OnBnClickedSetobjecttoshadowmap()
{
	if(!m_scene3D)return;
	CString str;
	this->m_edObjectToShadowMap.GetWindowText(str);
	float f = _wtof(str);
	m_scene3D->SetShadowMapObjectEpsilon(f);
}
