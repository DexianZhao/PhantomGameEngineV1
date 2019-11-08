/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// SubGeometryEditorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "phantom-gc.h"
#include "SubGeometryEditorDlg.h"
#include "afxdialogex.h"
#include "GCViewGeometryEditor.h"

// CSubGeometryEditorDlg 对话框

IMPLEMENT_DYNAMIC(CSubGeometryEditorDlg, CDialogBase)

CSubGeometryEditorDlg::CSubGeometryEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialogBase(CSubGeometryEditorDlg::IDD, pParent)
{
	m_geomEditor		=	0;
	m_geomData			=	0;
	m_geomShader		=	0;
}

CSubGeometryEditorDlg::~CSubGeometryEditorDlg()
{
}

void CSubGeometryEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GEOM_VARIANT_LIST, m_cbShaderVariants);
	DDX_Control(pDX, IDC_GEOM_GEOMETRY_VARIANT, m_lbExistVariants);
	DDX_Control(pDX, IDC_GEOM_VALUE_X, m_edValueX);
	DDX_Control(pDX, IDC_GEOM_VALUE_Y, m_edValueY);
	DDX_Control(pDX, IDC_GEOM_VALUE_Z, m_edValueZ);
	DDX_Control(pDX, IDC_GEOM_VALUE_W, m_edValueW);
	DDX_Control(pDX, IDC_VALUE_RGB, m_colorValue);
}


BEGIN_MESSAGE_MAP(CSubGeometryEditorDlg, CDialogBase)
	ON_WM_CTLCOLOR()
	ON_LBN_SELCHANGE(IDC_GEOM_GEOMETRY_VARIANT, &CSubGeometryEditorDlg::OnLbnSelchangeGeomGeometryVariant)
	ON_BN_CLICKED(IDC_GEOM_ADD_VARIANT, &CSubGeometryEditorDlg::OnBnClickedGeomAddVariant)
	ON_BN_CLICKED(IDC_GEOM_DELETE_VARIANT, &CSubGeometryEditorDlg::OnBnClickedGeomDeleteVariant)
	ON_BN_CLICKED(IDC_GEOM_SET_VARIANT_VALUE, &CSubGeometryEditorDlg::OnBnClickedGeomSetVariantValue)
	ON_BN_CLICKED(IDC_VALUE_RGB, &CSubGeometryEditorDlg::OnBnClickedValueRgb)
END_MESSAGE_MAP()


// CSubGeometryEditorDlg 消息处理程序


HBRUSH CSubGeometryEditorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogBase::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

VOID	CSubGeometryEditorDlg::SetGeometryData(Phantom::SubMeshData*	data, Phantom::Mesh* render, Phantom::MeshShaderProject*	shader)
{
	this->m_geomData	=	data;
	this->m_geomRender	=	render;
	m_geomShader		=	shader;
	if(m_geomShader)
	{
		Phantom::MeshData* data	=	this->m_geomRender->GetDataPtr();
		//IVspsObject* obj	=	GetRenderDevicePtr()->GetVspsObjectPtr(data->GetShaderMode());
		//if(!obj)return;
		//IShader* vsps	=	obj->GetNormalShaderPtr();
		//
		int sel	=	m_lbExistVariants.GetItemData(m_lbExistVariants.GetCurSel());
		m_lbExistVariants.ResetContent();
		if(sel >= m_geomShader->GetVariants()->GetDataCount())
			sel = m_geomShader->GetVariants()->GetDataCount() - 1;
		//
		for(int i=0;i<m_geomShader->GetVariants()->GetDataCount();i++)
		{
			Phantom::PShaderValue* var	=	m_geomShader->GetVariants()->GetDataPtr(i);
			//
			int index	=	-1;
			//if(obj)
			{
				//for(unsigned int ivsps = 0;ivsps<vsps->GetVariantCount();ivsps++)
				//{
				//	const char* nameOut = 0;
				//	vsps->GetVariantHandle(ivsps, &nameOut);
				//	const char* sztype;
				//	if(!vsps->GetTextParameter(nameOut, "name", &sztype))
				//		continue;
				//	if(stricmp(nameOut, var->name.t) == 0)
				//	{
				//		static std::map<std::string, std::string> values;
				//		values[sztype]	=	var->name.t;
				//		index	=	m_lbExistVariants.AddString(W(sztype));
				//		m_lbExistVariants.SetItemData(index, i);
				//		break;
				//	}
				//}
			}
			if(index < 0)continue;
			if(m_lbExistVariants.GetItemData(index) == sel)
				m_lbExistVariants.SetCurSel(index);
		}
		//m_lbExistVariants.SetCurSel(sel);
	}
}



void CSubGeometryEditorDlg::OnLbnSelchangeGeomGeometryVariant()
{
	//if(m_lbExistVariants.GetCurSel() < 0 || !m_geomShader)return;
	//int index		=	m_lbExistVariants.GetItemData(m_lbExistVariants.GetCurSel());
	//const Phantom::PShaderValue*	v	=	m_geomShader->GetVariants()->GetDataPtr(index);
	//if(v)
	//{
	//	switch(v->type)
	//	{
	//	case VSPS_TYPE_FLOAT:
	//	case VSPS_TYPE_INT:
	//		{
	//			this->m_edValueX.ShowWindow(SW_SHOW);
	//			this->m_edValueY.ShowWindow(SW_HIDE);
	//			this->m_edValueZ.ShowWindow(SW_HIDE);
	//			this->m_edValueW.ShowWindow(SW_HIDE);
	//		}
	//		break;
	//	case VSPS_TYPE_VECTOR:
	//		{
	//			this->m_edValueX.ShowWindow(SW_SHOW);
	//			this->m_edValueY.ShowWindow(SW_SHOW);
	//			this->m_edValueZ.ShowWindow(SW_SHOW);
	//			this->m_edValueW.ShowWindow(SW_SHOW);
	//		}
	//		break;
	//	}
	//	char buf[200];
	//	sprintf(buf, "%.08f", v->value.x);
	//	m_edValueX.SetWindowText(W(buf));
	//	sprintf(buf, "%.08f", v->value.y);
	//	m_edValueY.SetWindowText(W(buf));
	//	sprintf(buf, "%.08f", v->value.z);
	//	m_edValueZ.SetWindowText(W(buf));
	//	sprintf(buf, "%.08f", v->value.w);
	//	m_edValueW.SetWindowText(W(buf));
	//	//IVspsObject* obj	=	GetRenderDevicePtr()->GetVspsObjectPtr(m_geomRender->GetDataPtr()->GetShaderMode());
	//	//if(obj)
	//	//{
	//	//	IShader* vsps	=	obj->GetNormalShaderPtr();
	//	//	unsigned int numVar = vsps->GetVariantCount();
	//	//	for(unsigned int iVar = 0; iVar < numVar; iVar ++)
	//	//	{
	//	//		const char* strName = 0;
	//	//		vsps->GetVariantHandle(iVar, &strName);
	//	//		if(!strName || stricmp(v->name.t, strName) != 0)continue;
	//	//		const char* name = 0;
	//	//		const char* type = 0;
	//	//		float fMin = 0;
	//	//		float fMax = 1;
	//	//		if(!vsps->GetTextParameter(strName, "name", &name))continue;
	//	//		if(!vsps->GetFloatParameter(strName, "min", &fMin))continue;
	//	//		if(!vsps->GetFloatParameter(strName, "max", &fMax))continue;
	//	//		this->m_edValueX.SetFloat(fMin, fMax, this, IDC_GEOM_SET_VARIANT_VALUE);
	//	//		this->m_edValueY.SetFloat(fMin, fMax, this, IDC_GEOM_SET_VARIANT_VALUE);
	//	//		this->m_edValueZ.SetFloat(fMin, fMax, this, IDC_GEOM_SET_VARIANT_VALUE);
	//	//		this->m_edValueW.SetFloat(fMin, fMax, this, IDC_GEOM_SET_VARIANT_VALUE);
	//	//		break;
	//	//	}
	//	//}
	//}
}


VOID	OnSetGeometryAddShaderVar(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, Phantom::MeshShaderProject* shader, LPARAM nParameter, LPARAM nParameter2)
{
	//IVspsObject* obj	=	GetRenderDevicePtr()->GetVspsObjectPtr(data->GetShaderMode());
	//if(obj)
	//{
	//	IShader* vsps	=	obj->GetNormalShaderPtr();
	//	const char* nameOut = 0;
	//	vsps->GetVariantHandle(nParameter, &nameOut);
	//	if(!nameOut)
	//		return;
	//	int type;//		=	m_cbVariantList.GetItemData(m_cbVariantList.GetCurSel());
	//	int id	=	0;
	//	const char* sztype;
	//	if(!vsps->GetTextParameter(nameOut, "type", &sztype))
	//		return;
	//	const char* szID;
	//	if(vsps->GetTextParameter(nameOut, "id", &szID))
	//		id	=	atoi(szID);
	//	if(stricmp(sztype, "float") == 0)
	//	{
	//		type	=	VSPS_TYPE_FLOAT;
	//	}
	//	else if(stricmp(sztype, "check") == 0 || stricmp(sztype, "int") == 0)
	//	{
	//		type	=	VSPS_TYPE_INT;
	//	}
	//	else if(stricmp(sztype, "color") == 0 || stricmp(sztype, "Phantom::float4") == 0)
	//	{
	//		type	=	VSPS_TYPE_VECTOR;
	//	}
	//	else
	//		return;
	//	Phantom::PShaderValue* val	=	shader->GetVariants()->AddData(0);
	//	val->name	=	nameOut;
	//	val->type	=	type;
	//	val->id		=	id;
	//	//shader->AddShaderVariant(nameOut, type);
	//	_shader_var var;
	//	switch(type)
	//	{
	//	case VSPS_TYPE_FLOAT:
	//		{
	//			float f = 0;
	//			if(vsps->GetFloat(var, f, nameOut))
	//				val->value.x	=	f;
	//		}
	//		break;
	//	case VSPS_TYPE_INT:
	//		{
	//			int f = 0;
	//			if(vsps->GetInt(var, f, nameOut))
	//				val->value.x	=	f;
	//		}
	//		break;
	//	case VSPS_TYPE_VECTOR:
	//		{
	//			Phantom::float4 v(0,0,0,0);
	//			if(vsps->GetFloat4(var, v, nameOut))
	//				val->value	=	v;
	//		}
	//		break;
	//	}
	//}
}

VOID	OnSetGeometryRemoveVar(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, Phantom::MeshShaderProject* shader, LPARAM nParameter, LPARAM nParameter2)
{
	Phantom::PtrID id;
	id.id	=	nParameter;
	shader->GetVariants()->RemoveData(id);
	shader->SetVariantChanged();
}

VOID	OnSetGeometryVarValue(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, Phantom::MeshShaderProject* shader, LPARAM nParameter, LPARAM nParameter2)
{
	//const char* strName =	(const char*)nParameter;
	//Phantom::PtrID id;
	//id.id	=	nParameter;
	Phantom::PShaderValue* val	=	shader->GetVariants()->GetDataPtr(nParameter);
	if(!val)
		return;
	Phantom::float4*		value	=	(Phantom::float4*)nParameter2;
	//IVspsObject* obj	=	GetRenderDevicePtr()->GetVspsObjectPtr(data->GetShaderMode());
	//if(obj)
	//{
	//	IShader* vsps	=	obj->GetNormalShaderPtr();
	//	unsigned int numVar = vsps->GetVariantCount();
	//	for(unsigned int iVar = 0; iVar < numVar; iVar ++)
	//	{
	//		const char* szName = 0;
	//		vsps->GetVariantHandle(iVar, &szName);
	//		if(!szName || stricmp(szName, val->name.t) != 0)continue;
	//		const char* name = 0;
	//		const char* type = 0;
	//		if(!vsps->GetTextParameter(szName, "name", &name))continue;
	//		if(!vsps->GetTextParameter(szName, "type", &type))continue;
	//		val->value	=	*value;
	//		shader->SetVariantChanged();
	//		//if(stricmp(type, "float") == 0)
	//		//{
	//		//	val->value	=	*value;
	//		//	shader->SetShaderVariantFloat4(strName, *value);
	//		//}
	//		//else if(stricmp(type, "check") == 0 || stricmp(type, "int") == 0)
	//		//{
	//		//	shader->SetShaderVariantFloat4(strName, *value);
	//		//}
	//		//else if(stricmp(type, "Phantom::float4") == 0)
	//		//{
	//		//	shader->SetShaderVariantFloat4(strName, *value);
	//		//}
	//		//else if(stricmp(type, "color") == 0)
	//		//{
	//		//	shader->SetShaderVariantFloat4(strName, *value);
	//		//}
	//		break;
	//	}
	//}
}

void CSubGeometryEditorDlg::OnBnClickedGeomAddVariant()
{
	if(this->m_cbShaderVariants.GetCurSel() < 0 || !m_geomShader)return;
	int vspsIndex	=	(int)m_cbShaderVariants.GetItemData(m_cbShaderVariants.GetCurSel());
	//
	m_geomEditor->LoopAllGeometryShaders(OnSetGeometryAddShaderVar, vspsIndex);
	SetGeometryData(this->m_geomData, this->m_geomRender, this->m_geomShader);
}

void CSubGeometryEditorDlg::OnBnClickedGeomDeleteVariant()
{
	if(this->m_lbExistVariants.GetCurSel() < 0 || !m_geomShader)return;
	Phantom::PShaderValue* var	=	m_geomShader->GetVariants()->GetDataPtr(m_lbExistVariants.GetItemData(m_lbExistVariants.GetCurSel()));
	if(!var)
		return;
	//
	m_geomEditor->LoopAllGeometryShaders(OnSetGeometryRemoveVar, (LPARAM)var->id);
	SetGeometryData(this->m_geomData, this->m_geomRender, this->m_geomShader);
}


void CSubGeometryEditorDlg::OnBnClickedGeomSetVariantValue()
{
	if(this->m_lbExistVariants.GetCurSel() < 0 || !m_geomShader)return;
	//
	CString x,y,z,w;
	m_edValueX.GetWindowText(x);
	m_edValueY.GetWindowText(y);
	m_edValueZ.GetWindowText(z);
	m_edValueW.GetWindowText(w);
	Phantom::float4 val(_wtof(x),_wtof(y),_wtof(z),_wtof(w));
	int index	=	m_lbExistVariants.GetItemData(m_lbExistVariants.GetCurSel());
	Phantom::PShaderValue* var	=	m_geomShader->GetVariants()->GetDataPtr(index);
	if(!var)
		return;
	m_geomEditor->LoopAllGeometryShaders(OnSetGeometryVarValue, (LPARAM)index, false, (LPARAM)&val);
	Phantom::color4 c(val.x, val.y, val.z, val.w);
	m_colorValue.SetColor(c.getRGB());
}


void CSubGeometryEditorDlg::OnBnClickedValueRgb()
{
	Phantom::color4 c;
	c.from(m_colorValue.GetColor(), 1.0f);
	wchar_t buf[128];
	swprintf(buf, L"%.08f", c.r);
	m_edValueX.SetWindowTextW(buf);
	swprintf(buf, L"%.08f", c.g);
	m_edValueY.SetWindowTextW(buf);
	swprintf(buf, L"%.08f", c.b);
	m_edValueZ.SetWindowTextW(buf);
	OnBnClickedGeomSetVariantValue();
}

