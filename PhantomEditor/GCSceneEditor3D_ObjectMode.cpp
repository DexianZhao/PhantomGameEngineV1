//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   GCSceneEditor3D.cpp
	软件名称   :   幻影游戏引擎

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	
	Copyright 2009-2016 Zhao Dexian
	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "GCSceneEditor3D.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"
#include "AlignDialog3D.h"

SelectEditor::SelectEditor()
{
	m_downMatrix.identity();
	m_newMatrix.identity();
	m_downPosition.setxyz(0,0,0);
	m_downRotation.setxyz(0,0,0);
	m_downScale.setxyz(0,0,0);
	m_eventPtr	=	0;
	m_objectPtr	=	0;
	m_lightPtr	=	0;
	m_bLightTarget	=	0;
}
BOOL	SelectEditor::operator ==(const SelectEditor& ed)
{
	if(this->m_objectPtr){
		return (m_objectPtr == ed.m_objectPtr);
	}
	if(this->m_eventPtr)
		return (m_eventPtr == ed.m_eventPtr);
	if(m_lightPtr)
		return m_lightPtr==ed.m_lightPtr;
	return false;
}
VOID	SelectEditor::Init(Phantom::SceneLight* light, int bTarget)
{
	m_lightPtr		=	light;
	m_downMatrix.identity();
	if(bTarget){
		Phantom::float3 target = m_lightPtr->m_pos + m_lightPtr->m_dir * m_lightPtr->GetNear();
		m_downMatrix.SetPosition(target);
	}
	else
		m_downMatrix.SetPosition(m_lightPtr->m_pos);
	m_newMatrix		=	m_downMatrix;
	m_bLightTarget	=	bTarget;
}
VOID	SelectEditor::Init(Phantom::SceneObject* obj)
{
	m_objectPtr = obj;
	m_newMatrix	=	m_downMatrix	=	*m_objectPtr->GetWorldPtr();
}
VOID	SelectEditor::Init(Phantom::ScenePoint* evt)
{
	m_eventPtr = evt;
	m_downMatrix.identity();
	m_downMatrix.SetPosition(m_eventPtr->GetPosition());
	m_newMatrix	=	m_downMatrix;
}
BOOL	SelectEditor::GetAABox(Phantom::AABox& ret)
{
	if(m_objectPtr)
	{
		ret = *m_objectPtr->GetBox();
		return true;
	}
	Phantom::float3 pos;
	float range = 0;
	if(m_lightPtr){
		if(this->m_bLightTarget){
			pos = m_lightPtr->m_pos + m_lightPtr->m_dir * m_lightPtr->GetNear();
		}
		else{
			pos = m_lightPtr->m_pos;
		}
		Phantom::float3 lens = pos - Phantom::GetCameraPtr()->GetEye();
		range = clamp(lens.length()/To3DValue(10.0f), 1, 5);
	}
	else if(this->m_eventPtr){
		pos = m_eventPtr->GetPosition();
		range = m_eventPtr->GetRange();
	}
	else
		return false;
	ret._min = ret._max = pos;
	ret._min.x -= range;
	ret._min.y -= range;
	ret._min.z -= range;
	ret._max.x += range;
	ret._max.y += range;
	ret._max.z += range;
	return true;
}
BOOL	SelectEditor::IsEnabled()
{
	if(m_objectPtr)
		return m_objectPtr->GetEnabled();
	if(m_eventPtr)
		return m_eventPtr->GetEnabled();
	if(m_lightPtr)
		return m_lightPtr->m_bEnabled;
	return false;
}
BOOL	SelectEditor::IsVisible()
{
	if(m_objectPtr)
		return m_objectPtr->GetVisible();
	return true;
}
VOID	SelectEditor::SetEnabled(BOOL b)
{
	if(m_objectPtr)
		m_objectPtr->SetEnabled(b);
	if(m_eventPtr)
		m_eventPtr->SetEnabled(b);
	if(m_lightPtr)
	{
		m_lightPtr->m_bEnabled = b;
	}
}
VOID	SelectEditor::SetVisible(BOOL b)
{
	if(m_objectPtr)
		m_objectPtr->SetVisible(b);
}
float			SelectEditor::GetSize()
{
	if(m_objectPtr)
		return m_objectPtr->m_fScale;
	if(m_eventPtr)
		return m_eventPtr->m_range;
	if(m_lightPtr)
		return m_lightPtr->GetNear();
	return 0;
}
VOID			SelectEditor::SetSize(float f)
{
	if(m_objectPtr)
		m_objectPtr->SetScale(f);
	if(m_eventPtr)
		m_eventPtr->SetRange(f);
	if(m_lightPtr)
		m_lightPtr->SetFar(f);
}
INT							SelectEditor::GetID()
{
	if(this->m_objectPtr)
		return m_objectPtr->m_nContentID;
	if(this->m_eventPtr)
		return m_eventPtr->GetID();
	if(m_lightPtr)
		return m_lightPtr->GetID();
	return 0;
}
const Phantom::float3&		SelectEditor::GetPosition()
{
	if(m_objectPtr)
		return m_objectPtr->GetPosition();
	if(m_eventPtr)
		return m_eventPtr->GetPosition();
	if(m_lightPtr){
		if(m_bLightTarget){
			static Phantom::float3 pos;
			pos = m_lightPtr->m_pos + m_lightPtr->m_dir * m_lightPtr->GetNear();
			return pos;
		}
		else
			return m_lightPtr->m_pos;
	}
	static Phantom::float3 v(0,0,0);
	return v;
}
VOID						SelectEditor::SetScale(const Phantom::float3& s)
{
	if(m_objectPtr)
		m_objectPtr->SetScale((s.z+(s.y+s.x)/2.0f)/2.0f);
}
VOID						SelectEditor::SetRotation(const Phantom::float3& r)
{
	if(m_objectPtr)
		m_objectPtr->SetRotation(r.z);
}

VOID						SelectEditor::SetPosition(const Phantom::float3& v)
{
	if(m_objectPtr)
		m_objectPtr->SetPosition(v);
	if(m_eventPtr)
		m_eventPtr->SetPosition(v);
	if(m_lightPtr)
	{
		if(this->m_bLightTarget)
			m_lightPtr->m_pos = v;
		else{
			m_lightPtr->m_dir = v - m_lightPtr->m_pos;
			m_lightPtr->SetNear(m_lightPtr->m_dir.R());
			m_lightPtr->m_dir.normalize();
		}
	}
}

const char*		SelectEditor::GetName()
{
	if(m_objectPtr)
		return m_objectPtr->GetName();
	if(m_eventPtr)
		return m_eventPtr->m_name.t;
	if(m_lightPtr)
		return m_lightPtr->m_name.t;
	return "";
}
BOOL			SelectEditor::IsCustomName()
{
	if(m_objectPtr)
		return m_objectPtr->IsCustomName();
	return true;
}
VOID			SelectEditor::SetName(const char* s, BOOL bCustom)
{
	if(m_objectPtr)
		m_objectPtr->SetName(s, bCustom);
	else if(m_eventPtr)
		m_eventPtr->m_name = s;
	else if(m_lightPtr)
		m_lightPtr->m_name = s;
}

BOOL	SelectEditor::CanMove()
{
	if(m_objectPtr)
		return true;
	if(m_eventPtr)
		return true;
	if(m_lightPtr)
		return true;
	return false;
}
BOOL	SelectEditor::CanRotation()
{
	if(m_objectPtr)
		return true;
	return false;
}
BOOL	SelectEditor::CanScale()
{
	if(m_objectPtr)
		return true;
	return false;
}
BOOL	SelectEditor::Clone(SelectEditor& ret)
{
	if(m_objectPtr){
		ret.Init(m_objectPtr->Clone());
	}
	else if(m_eventPtr){
		ret.Init(m_eventPtr->Clone());
	}
	else if(m_lightPtr){
		if(!m_bLightTarget)
			ret.Init(m_lightPtr->Clone(), false);
	}
	else
		return false;
	return true;
}
VOID	SelectEditor::BeginEditor(AxisOperatorType ot)
{
	if(m_objectPtr){
		m_downMatrix	=	*m_objectPtr->GetWorldPtr();
		m_downPosition	=	m_objectPtr->GetPosition();
		m_downRotation.setxyz(0, 0, m_objectPtr->m_fRotationZ);
		m_downScale.setxyz(m_objectPtr->m_fScale, m_objectPtr->m_fScale, m_objectPtr->m_fScale);
		return;
	}
	Phantom::float3 pos(0,0,0);
	if(m_lightPtr)
	{
		pos = m_lightPtr->m_pos;
		if(this->m_bLightTarget)
			pos = m_lightPtr->m_pos + m_lightPtr->m_dir * m_lightPtr->GetNear();
	}
	else if(m_eventPtr)
		pos = m_eventPtr->GetPosition();
	m_downMatrix.identity();
	m_downMatrix.SetPosition(pos);
	m_downPosition	=	pos;
	m_downRotation.setxyz(0, 0, 0);
	m_downScale.setxyz(1,1,1);
	m_newMatrix		=	m_downMatrix;
}

VOID	SelectEditor::EndEditor(AxisOperatorType ot, const Phantom::float3& val)
{
	if(m_objectPtr)
	{
		//GetAlignValue3D(temp, (int)fGrid/4, m_scene3D);
		//pObjectPtr->SetWorld( &temp );
		if(ot == AxisOT_Move)
			m_objectPtr->SetPosition(m_downPosition + val);
		else if(ot == AxisOT_Scale)
		{
			m_downScale += Phantom::float3(val.x-1,val.y-1,val.z-1);
			m_objectPtr->SetScale((m_downScale.z+(m_downScale.y+m_downScale.x)/2.0f)/2.0f);
		}
		else if(ot == AxisOT_Rotation)
		{
			m_downRotation += val;
			m_objectPtr->SetRotation(m_downRotation.z);
		}
	}
	else if(m_eventPtr){
		if(ot == AxisOT_Move)
			m_eventPtr->SetPosition(m_downPosition + val);
	}
	else if(m_lightPtr){
		if(ot == AxisOT_Move)
		{
			if(this->m_bLightTarget){
				m_lightPtr->m_dir = (m_downPosition + val) - m_lightPtr->m_pos;
				m_lightPtr->SetNear(m_lightPtr->m_dir.R());
				m_lightPtr->m_dir.normalize();
			}
			else
				m_lightPtr->m_pos = m_downPosition + val;
		}
	}
}
VOID	SelectEditor::OnEditor(AxisOperatorType ot, const Phantom::float3& val)
{
	//Phantom::matrix4x4 temp = it->newMatrix;
	//GetAlignValue3D(temp, (int)fGrid/4, m_scene3D);
	if(m_objectPtr){
		if(ot == AxisOT_Move)
			m_objectPtr->SetPosition(m_downPosition + val);
		else if(ot == AxisOT_Scale)
		{
			Phantom::float3 scale = m_downScale + Phantom::float3(val.x-1,val.y-1,val.z-1);
			m_objectPtr->SetScale((scale.z+(scale.y+scale.x)/2.0f)/2.0f);
		}
		else if(ot == AxisOT_Rotation)
		{
			Phantom::float3 rot = m_downRotation + val;
			m_objectPtr->SetRotation(rot.z);
		}
	}
	else if(m_eventPtr){
		if(ot == AxisOT_Move)
			m_eventPtr->SetPosition(m_downPosition + val);
	}
	else if(m_lightPtr){
		if(ot == AxisOT_Move)
		{
			if(this->m_bLightTarget){
				m_lightPtr->m_dir = (m_downPosition + val) - m_lightPtr->m_pos;
				m_lightPtr->SetNear(m_lightPtr->m_dir.R());
				m_lightPtr->m_dir.normalize();
			}
			else
				m_lightPtr->m_pos = m_downPosition + val;
		}
	}
}

BOOL	SelectEditor::GetObjectBox(Phantom::AABox& ret, Phantom::float3& off, Phantom::matrix4x4* newMat){
	if(this->m_objectPtr){
		Phantom::AABox boxt = m_objectPtr->GetMeshPtr()->GetDataPtr()->m_region.aabox;//
		Phantom::PBoxVertices v;
		v.setup(boxt._min, boxt._max);
		Phantom::matrix4x4 mat = m_objectPtr->GetMeshPtr()->GetWorldMatrix();
		if(newMat)mat = *newMat;
		for(int i=0;i<8;i++)
		{
			v.v[i].transform(v.v[i], mat);
		}
		Phantom::AABox box;
		box._min.setxyz(10000000, 10000000, 10000000);
		box._max.setxyz(-10000000, -10000000, -10000000);
		for(int i=0;i<8;i++)
		{
			box._min.x = min(v.v[i].x, box._min.x);
			box._min.y = min(v.v[i].y, box._min.y);
			box._min.z = min(v.v[i].z, box._min.z);
			box._max.x = max(v.v[i].x, box._max.x);
			box._max.y = max(v.v[i].y, box._max.y);
			box._max.z = max(v.v[i].z, box._max.z);
		}
		ret = box;
		off = mat.GetPosition() - ret._min;
		return true;
	}
	Phantom::float3 pos;
	float range = 0;
	if(m_lightPtr){
		if(this->m_bLightTarget)
			pos = m_lightPtr->m_pos + m_lightPtr->m_dir * m_lightPtr->GetNear();
		else
			pos = m_lightPtr->m_pos;
		Phantom::float3 lens = pos - Phantom::GetCameraPtr()->GetEye();
		range = clamp(lens.length()/To3DValue(10.0f), 1, 5);
	}
	else if(this->m_eventPtr){
		pos = m_eventPtr->GetPosition();
		range = m_eventPtr->GetRange();
	}
	Phantom::AABox box;
	box._min = box._max = pos;
	box._min.x	-=	range;
	box._min.y	-=	range;
	box._min.z	-=	range;
	box._max.x	+=	range;
	box._max.y	+=	range;
	box._max.z	+=	range;
	Phantom::matrix4x4 mat;
	mat.identity();
	mat.SetPosition(pos);
	if(newMat)mat = *newMat;
	ret = box;
	off = mat.GetPosition() - ret._min;
	return true;
}

BOOL						GCSceneEditor3D::BeginEditor(Phantom::matrix4x4& matOut, AxisOperatorType ot)
{
	if(GetCopyKeyboard())
		m_objectEditMode	=	ObjectEditMode_Copy;
	if(this->m_editorMode == this->EditorMode_LineEditorMode)
	{
		return BeginEditorLine(matOut);
	}
	for(int i=0;i<m_selects.size();i++)
		m_selects[i].BeginEditor(ot);
	if(!RebuildObjectsCenter())
		return FALSE;
	//this->AddUndoRedoData(ObjectUndoData::UNDO_SET_MATRIX);
	matOut.identity();
	matOut.SetPosition( m_selectCenter );
	return TRUE;
}

BOOL						GCSceneEditor3D::PickTerrain(int x, int y, Phantom::float3& ret)
{
	Phantom::float3 orig, dir;
	Phantom::GetCameraPtr()->SetViewMatrix();
	Phantom::GetCameraPtr()->Get3DFrom2D(orig, dir, Phantom::Pixel(x, y));

	//	PIntersectData d;
	//	float dist = 100000.0f;
	//	memset(&d, 0, sizeof(d));
	//	d.t = &dist;
	//	d.Pick	=	&ret;

	//if(intersectTriangle(&d, orig, dir, Phantom::float3(0, 0, 0), Phantom::float3(1000000.0f, 0, 0), Phantom::float3(1000000, 1000000.0f, 0), 0, true))
	//{
	//	return true;
	//}
	//if(intersectTriangle(&d, orig, dir, Phantom::float3(0, 0, 0), Phantom::float3(1000000, 1000000.0f, 0), Phantom::float3(0, 1000000, 0), 0, true))
	//{
	//	return true;
	//}
	//dir	*=	100000.0f;
	//Phantom::PickPoint td;
	//if(!m_scene3D->PickLinear(orig, orig + dir, td))
	//	return false;
	//ret	=	td.pos;
	return false;//true;
}

BOOL	GetAlignValue3D(float fMin, float fMax, float fGrid, float& retMin)
{
	float min1 = fmodf(fMin, fGrid);
	float max1 = fmodf(fMax, fGrid);
	float min2 = fGrid - min1;
	float max2 = fGrid - min2;
	const float fAlignWidth = 3.0f;
	float fAlign = (fGrid / fAlignWidth);//3分之一的精度
	int nType = 0;
	float mX = 0;
	if(min1 < max1)
	{
		mX = min1;
		nType = 0;
	}
	else
	{
		mX = max1;
		nType = 1;
	}
	if(min2 < mX)
	{
		mX = min2;
		nType = 2;
	}
	if(max2 < mX)
	{
		mX = max2;
		nType = 3;
	}
	if(mX > fAlign)//如果没有达到可对齐的大小则不进行对齐
		return false;
	if(nType == 0)
		retMin = (int)(fMin / fGrid) * fGrid;
	if(nType == 2)
		retMin = ((int)(fMin / fGrid) + 1) * fGrid;// - (fMax - fMin);
	if(nType == 1)
		retMin = (int)(fMax / fGrid) * fGrid - (fMax - fMin);
	if(nType == 3)
		retMin = ((int)(fMax / fGrid) + 1) * fGrid - (fMax - fMin);
	return true;
}
VOID	GetAlignValue3D(Phantom::matrix4x4& temp, int size, Phantom::Scene3D* scene)
{
	temp._41 = (int)(temp._41 / (int)size)*(int)size;
	temp._42 = (int)(temp._42 / (int)size)*(int)size;
	float z = temp._43;
	scene->GetHeight(temp.GetPosition(), z);
	float fZ = (temp._43 - z);
	fZ = (int)(fZ / (int)size)*(int)size;
	temp._43 = z + fZ;
}
VOID						GCSceneEditor3D::OnEditor(const Phantom::matrix4x4& downMat, const Phantom::matrix4x4& newMat, AxisOperatorType ot, const Phantom::float3& newValOffset)
{
	if(this->m_editorMode == this->EditorMode_LineEditorMode)
	{
		OnEditorLine(downMat, newMat);
		return;
	}
	Phantom::matrix4x4 origMat	=	newMat;
	origMat.SetPosition(0,0,0);
	Phantom::float3 offset	=	newMat.GetPosition() - downMat.GetPosition();
	float fGrid = To3DValue(Get3DGridSize());
	if(fGrid < 1)fGrid = 1;
	BOOL bFirst = true;
	Phantom::float3 firstOffset;
	for(std::vector<SelectEditor>::iterator it = m_selects.begin(); it != m_selects.end();)
	{
		BOOL bControl	=	GetSelectKeyboard();
		if(bControl)
		{
			Phantom::matrix4x4 mat;
			matrixInverse(&mat, NULL, &downMat);
			matrixMultiply(&mat, &it->m_downMatrix, &mat);
			matrixMultiply(&it->m_newMatrix, &mat, &newMat);//origMat);
		}
		else
		{
			it->m_newMatrix	=	it->m_downMatrix;
			it->m_newMatrix.SetPosition(0,0,0);
			matrixMultiply(&it->m_newMatrix, &newMat, &it->m_newMatrix);
			it->m_newMatrix.SetPosition(  it->m_downMatrix.GetPosition() + offset );
		}
		if(m_editorMode == EditorMode_ObjectMove)
		{
			Phantom::AABox box;
			Phantom::float3 off;
			if(it->GetObjectBox(box, off, &it->m_newMatrix))
			{
				Phantom::float3 vmin = box._min;
				bFirst = false;
				it->m_newMatrix.SetPosition(vmin + off);//fGrid * (int)(pos.x / fGrid), fGrid * (int)(pos.y / fGrid), fGrid * (int)(pos.z / fGrid));
			}
		}
		if(m_objectEditMode != ObjectEditMode_Copy)
		{
			//Phantom::matrix4x4 temp = it->m_newMatrix;
			//GetAlignValue3D(temp, (int)fGrid/4, m_scene3D);
			if(it->m_objectPtr&&it->m_objectPtr->GetType()==Phantom::MeshData::MeshType_StaticMesh){
				it->m_objectPtr->SetWorld(&it->m_newMatrix);
			}
			else{
				it->OnEditor(ot, newValOffset);
				if(it->m_lightPtr)m_scene3D->m_lightChanged++;
				this->SetModifyed();
			}
			//if(ot == AxisOT_Move)
			//	it->objectPtr->SetPosition(it->downPosition + newValOffset);
			//else if(ot == AxisOT_Scale)
			//{
			//	Phantom::float3 scale = it->downScale + Phantom::float3(newValOffset.x-1,newValOffset.y-1,newValOffset.z-1);
			//	it->objectPtr->SetScale((scale.z+(scale.y+scale.x)/2.0f)/2.0f);
			//}
			//else if(ot == AxisOT_Rotation)
			//{
			//	Phantom::float3 rot = it->downRotation + newValOffset;
			//	it->objectPtr->SetRotation(rot.z);
			//}
			//it->objectPtr->SetWorld(&temp);
		}
		it++;
	}
}

unsigned char g_sceneObjectText[] = {0x45,0xc6,0x22,0xc4,0xbc,0x14,0xe1,0x08,0x27,0x94,0xd3,0xf6,0xb1,0xfe,0xb3,0x5c,0x72,0x3f,0xc3,0x5f,0x1e,0x15,0xd2,0x38,0xe1,0x9f,0x6c,0x45,0x82,0xcf,0x85,0xbf,0x75};
unsigned char g_sceneObjectTextPass[] = {0x9e,0x3c,0x8d,0x0d,0x14,0xea,0x48,0xae,0xe9,0x37,0x0f,0x32,0x65,0x0b,0x17,0x97,0xbd,0xc9,0x6d,0xa7,0x37,0x3d,0xfa,0x98,0x0f,0x34,0xd5,0xe3,0x2a,0x61,0x49,0x68};

VOID						ShowSceneMaxAdd()
{
}
VOID						GCSceneEditor3D::EndEditor(const Phantom::matrix4x4& downMat, AxisOperatorType ot, const Phantom::float3& newValOffset)
{
	float fGrid = To3DValue(Get3DGridSize());
	if(fGrid < 1)fGrid = 1;
	if(this->m_editorMode == this->EditorMode_LineEditorMode)
	{
		EndEditorLine(downMat);
		m_objectEditMode	=	ObjectEditMode_None;
		return;
	}
	if(m_objectEditMode == ObjectEditMode_Copy)
	{
		if(GetCopyKeyboard())
		{
			//if(m_scene3D->GetObjectCount() >= MAX_SCENE_OBJECT)
			//{
			//	ShowSceneMaxAdd();
			//	return;
			//}
			std::vector<SelectEditor> newSelects;
			for(int i=0;i<m_selects.size();i++)
			{
				SelectEditor ed;
				if(!m_selects[i].Clone(ed))
					continue;
				Phantom::float3 pos = m_selects[i].GetPosition();
				Phantom::float3 rot = m_selects[i].GetRotation();
				Phantom::float3 scale = m_selects[i].GetScale();
				if(ot == AxisOT_Move)
					pos += newValOffset;
				else if(ot == AxisOT_Scale)
					scale += Phantom::float3(newValOffset.x-1,newValOffset.y-1,newValOffset.z-1);
				else if(ot == AxisOT_Rotation)
					rot += newValOffset;
				ed.SetPosition(pos);//SetWorld(&it->newMatrix );
				ed.SetScale(scale);
				ed.SetRotation(rot);
				newSelects.push_back( ed );
				if(ed.m_lightPtr)m_scene3D->m_lightChanged++;
			}
			this->m_dlgObjects.RefreshSceneUnits();
			this->m_dlgObjects.RefershRoles();
			this->RefreshLights();
			OnSelectObject(newSelects);
		}
	}
	else
	{
		this->BeginUndo();
		SelectEditor ed;
		for(int i=0;i<m_selects.size();i++)
		{
			ed = m_selects[i];
			if(ed.m_objectPtr&&ed.m_objectPtr->GetType()==Phantom::MeshData::MeshType_StaticMesh){
			}
			else
			{
			}
			UndoPtr uptr;
			uptr.buff = AddUndoRedo(GCSceneEditor3D::SCENE3D_UNIT_LIST, (ProcUndoRedo)0);
			uptr.buff->m_bEnabled=true;
			uptr.urMode = UndoRedoBuffer::URMode_Add;
			uptr.buff->m_id		=	0;
			uptr.buff->m_id3 = this->m_scene3D->GetActiveEnv()->GetID();
			SSData vold,vnew;
			if(m_selects[i].m_objectPtr)
			{
				uptr.buff->m_id		=	1;
				uptr.buff->m_id2	=	m_selects[i].m_objectPtr->GetID();
			}
			else if(m_selects[i].m_eventPtr){
				uptr.buff->m_id		=	2;
				uptr.buff->m_id2	=	m_selects[i].m_eventPtr->GetID();
			}
			else if(m_selects[i].m_lightPtr){
				uptr.buff->m_id		=	3;
				uptr.buff->m_id2	=	m_selects[i].m_lightPtr->GetID();
				uptr.buff->m_id4	=	m_selects[i].m_bLightTarget;
			}
			else
				continue;
			vold.m_downPosition = ed.m_downPosition;
			vold.m_downRotation = ed.m_downRotation;
			vold.m_downScale = ed.m_downScale;
			ed.EndEditor(ot, newValOffset);
			vnew.m_downPosition = ed.GetPosition();
			vnew.m_downRotation = ed.GetRotation();
			vnew.m_downScale = ed.GetScale();
			(*uptr.buff) << vold << vnew;
		}
		if(m_selects.size()>0)
			OnSelectObject(ed);
		if(ed.m_lightPtr)m_scene3D->m_lightChanged++;
		this->RebuildObjectsCenter();
	}
	m_objectEditMode	=	ObjectEditMode_None;
	//m_scene3D->RebuildSceneRegionObjects();
}

VOID						GCSceneEditor3D::AddSelectObject(std::vector<SelectEditor>& objects, BOOL bSelTrue)
{
	BOOL bShift			=	GetCopyKeyboard();
	BOOL bControl		=	GetSelectKeyboard();
	BOOL bAlt			=	HIBYTE(GetKeyState(VK_MENU));
	if(bControl)
	{
		for(size_t i=0;i<m_selects.size();i++)
		{
			bool bfind	=	false;
			for(size_t j=0;j<objects.size();j++)
			{
				if(m_selects.at(i) == objects.at(j))
				{
					bfind	=	true;
					break;
				}
			}
			if(bfind)
				continue;
			objects.push_back( m_selects.at(i) );
		}
	}
	else if(bAlt)
	{
		std::vector<SelectEditor> oldObjects;
		for(size_t i=0;i<m_selects.size();i++)
		{
			oldObjects.push_back( m_selects.at(i) );
		}
		for(size_t j=0;j<objects.size();j++)
		{
			for(size_t i=0;i<oldObjects.size();i++)
			{
				if(oldObjects.at(i) == objects.at(j))
				{
					oldObjects.erase(oldObjects.begin() + i);
					break;
				}
			}
		}
		objects	=	oldObjects;
	}
	OnSelectObject(objects, bSelTrue);
}

bool						GCSceneEditor3D::OnWindowProcObject(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam)
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(GetDeviceWindow(), &pos);
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		{
			SetCapture(hWnd);
			m_bLButtonDown		=	true;
			m_objectEditMode	=	ObjectEditMode_None;
			m_downPoint			=	pos;
			if(m_axisCurrent && m_axisCurrent->OnLButtonDown(pos))
				return true;
			m_objectEditMode	=	ObjectEditMode_Select;
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			m_bLButtonDown		=	true;
			m_objectEditMode	=	ObjectEditMode_GetHeight;
			m_downPoint			=	pos;
			if(GetCopyKeyboard())
			{
				for(int i=0;i<m_selects.size();i++)
				{
					if(m_selects[i].GetObjectPtr())
					{
						Phantom::SceneObjectFile* file	=	m_scene3D->m_objectFilePtr.SearchDataPtr(m_selects[i].GetObjectPtr()->m_fileNameID);
						if(file)
							AfxGetApp()->OpenDocumentFile(W(file->name.t));
					}
				}
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			ReleaseCapture();
			if(!m_bLButtonDown)
			{
				m_objectEditMode	=	ObjectEditMode_None;
				return false;
			}
			//
			if(m_axisCurrent && m_axisCurrent->OnLButtonUp(pos))
			{
				m_objectEditMode	=	ObjectEditMode_None;
				this->SetModifyed();
				return true;
			}
			//
			switch(m_objectEditMode)
			{
			case ObjectEditMode_Select:
				{
					Phantom::CDynamicArray<Phantom::SceneObject*> objs;
					BOOL bPick = false;
					if(pos.x==m_downPoint.x&&pos.y==m_downPoint.y)
						bPick = true;
					Phantom::Rect rcSelect(min(pos.x, m_downPoint.x), min(pos.y, m_downPoint.y), max(pos.x, m_downPoint.x), max(pos.y, m_downPoint.y));
					if(!bPick)
						m_scene3D->SelectObjects(objs, rcSelect);
					//
					Phantom::SceneObject* bselOne = 0;
					std::vector<SelectEditor> objects;
					Phantom::IntersectData* intersect	=	Phantom::GetCameraPtr()->BeginIntersect(pos.x, pos.y);
					if(bPick)
					{
						Phantom::SceneObject* obj = m_scene3D->Intersect(intersect, false);
						if(obj)
							objs.push_back(obj);
					}
					//
					for(size_t i=0;i<objs.size();i++)
					{
						SelectEditor s;
						s.Init(objs[i]);
						objects.push_back( s );
					}
					if(bPick){
						Phantom::SceneEnv* env = m_scene3D->GetActiveEnv();
						for(int i=0;i<env->m_points.size();i++){
							Phantom::AABox box;
							box._min = box._max = env->m_points[i]->GetPosition();
							box._min -= env->m_points[i]->GetRange();
							box._max += env->m_points[i]->GetRange();
							if(Phantom::IntersectBox(intersect->GetOriginal(), intersect->GetDir(), box._min, box._max, 0))
							{
								SelectEditor s;
								s.Init(env->m_points[i]);
								objects.push_back( s );
								break;
							}
						}
						for(int i=0;i<env->m_lights.size();i++){
							Phantom::AABox box;
							box._min = box._max = env->m_lights[i]->m_pos;
							Phantom::float3 lens = env->m_lights[i]->m_pos - Phantom::GetCameraPtr()->GetEye();
							float fSize = clamp(lens.length()/To3DValue(10.0f), 1, 5);
							box._min -= fSize;
							box._max += fSize;
							if(Phantom::IntersectBox(intersect->GetOriginal(), intersect->GetDir(), box._min, box._max, 0))
							{
								SelectEditor s;
								s.Init(env->m_lights[i], 0);
								objects.push_back( s );
								break;
							}
							Phantom::float3 target = env->m_lights[i]->m_pos + env->m_lights[i]->m_dir * env->m_lights[i]->GetNear();
							box._min = box._max = target;
							lens = target - Phantom::GetCameraPtr()->GetEye();
							fSize = clamp(lens.length()/To3DValue(10.0f), 1, 5);
							box._min -= fSize;
							box._max += fSize;
							if(Phantom::IntersectBox(intersect->GetOriginal(), intersect->GetDir(), box._min, box._max, 0))
							{
								SelectEditor s;
								s.Init(env->m_lights[i], 1);
								objects.push_back( s );
								break;
							}
						}
						if(objects.size()>0){
							static int selectIndex	=	0;
							static Phantom::Pixel lastPixel(0,0);
							if(lastPixel.x != pos.x || lastPixel.y != pos.y)
								selectIndex			=	0;
							else
								selectIndex			++;
							std::vector<SelectEditor> temp;
							temp = objects;
							objects.clear();
							objects.push_back(temp[selectIndex%temp.size()]);
						}
					}
					else{
						Phantom::CDynamicArray<Phantom::ScenePoint*> evts;
						m_scene3D->SelectEvents(evts, rcSelect);
						for(size_t i=0;i<evts.size();i++)
						{
							SelectEditor s;
							s.Init(evts[i]);
							objects.push_back( s );
						}
						Phantom::CDynamicArray<Phantom::SceneLight*> lights;
						Phantom::SceneEnv* env = m_scene3D->GetActiveEnv();
						for(int i=0;i<env->m_lights.size();i++){
							Phantom::Pixel pos;
							if(Phantom::GetCameraPtr()->Get2DFrom3D(pos, env->m_lights[i]->m_pos)){
								if(rcSelect.isPtIn(pos))
									lights.push_back(env->m_lights[i]);
							}
							Phantom::float3 target = env->m_lights[i]->m_pos + env->m_lights[i]->m_dir * env->m_lights[i]->GetNear();
							if(Phantom::GetCameraPtr()->Get2DFrom3D(pos, target)){
								if(rcSelect.isPtIn(pos))
									lights.push_back(env->m_lights[i]);
							}
						}
						for(size_t i=0;i<lights.size();i++)
						{
							SelectEditor s;
							s.Init(lights[i], 0);
							objects.push_back( s );
							s.Init(lights[i], 1);
							objects.push_back( s );
						}
					}
					AddSelectObject(objects);
					if(bselOne && m_selects.size() > 1)
					{
						SelectEditor save = m_selects[0];
						for(int i=0;i<m_selects.size();i++)
						{
							if(this->m_selects[i].GetObjectPtr() == bselOne)
							{
								this->m_selects[0] = m_selects[i];
								m_selects[i] = save;
								break;
							}
						}
					}
					if(objects.size()>0)
						this->OnSelectObject(objects[0]);
				}
				break;
			case ObjectEditMode_GetHeight:
				{
					if(this->m_axisCurrent == &this->m_axisMoving)
					{
						for(int i=0;i<m_selects.size();i++)
						{
							Phantom::float3 pos	=	m_selects[i].GetPosition();
							if(m_scene3D->GetHeight(pos, pos.z, 0))
								m_selects[i].SetPosition(pos);
						}
					}
					else if(this->m_axisCurrent == &this->m_axisRotation)
					{
						for(int i=0;i<m_selects.size();i++)
						{
							m_selects[i].SetRotation(Phantom::float3(0,0,0));
							//Phantom::SceneObject* pObjectPtr	=	it->GetObjectPtr();
							//if(pObjectPtr)
							//	pObjectPtr->SetRotation(0);
							//	Phantom::float3 pos	=	pObjectPtr->GetWorldPtr()->GetPosition();
							//	Phantom::matrix4x4 xmat	=	*pObjectPtr->GetWorldPtr();
							//	Phantom::float3 scale;
							//	Phantom::getMatrixScale(scale, xmat);
							//	Phantom::matrixScaling(&xmat, scale.x, scale.y, scale.z);
							//	xmat.SetPosition(pos);
							//	pObjectPtr->SetWorld(&xmat);
							//}
						}
					}
					else if(this->m_axisCurrent == &this->m_axisScaling)
					{
						for(int i=0;i<m_selects.size();i++)//std::vector<SelectEditor>::iterator it = m_selectObjects.begin(); it != m_selectObjects.end();it++)
						{
							Phantom::SceneObject* pObjectPtr	=	m_selects[i].GetObjectPtr();
							if(pObjectPtr)
							{
								Phantom::float3 pos	=	pObjectPtr->GetWorldPtr()->GetPosition();
								Phantom::matrix4x4 xmat	=	*pObjectPtr->GetWorldPtr();
								Phantom::quaternion qtn;
								Phantom::getMatrixQuaternion(qtn, xmat);
								Phantom::matrixRotationQuaternion(&xmat, &qtn);
								xmat.SetPosition(pos);
								pObjectPtr->SetWorld(&xmat);
							}
						}
					}
					if(m_selects.size() > 0)
						this->SetModifyed();
				}
				break;
			}
			m_objectEditMode	=	ObjectEditMode_None;
		}
		break;
	case WM_MOUSEMOVE:
		{
			if(m_axisCurrent)
				if(m_axisCurrent->OnMouseMove(pos))
					return true;
		}
		break;
	case WM_KEYDOWN:
		{
			if(wParam == VK_DELETE)
			{
				if(MessageBox(this->GetViewHandle(), Language(L"是否真的要删除?"), Language(L"提示"), MB_YESNO) == IDYES)
				{
					for(int i=0;i<this->m_selects.size();i++)
					{
						if(m_selects[i].GetObjectPtr())
							m_scene3D->RemoveObject(m_selects[i].GetObjectPtr());
						else if(m_selects[i].GetEventPtr())
							m_scene3D->GetActiveEnv()->RemovePoint(m_selects[i].GetEventPtr());
						else if(m_selects[i].m_lightPtr)
							m_scene3D->GetActiveEnv()->RemoveLight(m_selects[i].m_lightPtr->GetID());
					}
					m_scene3D->m_lightChanged++;
					this->SetModifyed();
					m_selects.clear();
					std::vector<SelectEditor> objects;
					this->RebuildPoints();
					this->RefreshLights();
					this->OnSelectObject(objects);
				}
			}
		}
		break;
	}
	return false;
}

VOID						GCSceneEditor3D::OnSelectObject(std::vector<SelectEditor>& objects, BOOL bSelTrue)
{
	std::map<int, int> selTrees;
	this->m_selects.clear();
	for(size_t i=0;i<objects.size();i++)
	{
		bool bfind	=	false;
		for(int j=0;j<m_selects.size();j++)
		{
			if(m_selects.at(j) == objects.at(i))
			{
				bfind	=	true;
				break;
			}
		}
		if(bfind)
			continue;
		m_selects.push_back(objects.at(i));
		if(objects.at(i).GetObjectPtr()){
			selTrees[objects.at(i).GetObjectPtr()->m_fileNameID]	=	1;
		}
	}
	if(!this->RebuildObjectsCenter())
		return;
	//for(size_t i=0;i<m_selectObjects.size();i++)
		//m_selectObjects.at(i).objectPtr->->SaveTransform();
	//
	if(m_editorMode == EditorMode_ObjectMove)
		m_axisCurrent	=	&m_axisMoving;
	else if(m_editorMode == EditorMode_ObjectScale)
		m_axisCurrent	=	&m_axisScaling;
	else if(m_editorMode == EditorMode_ObjectRotation)
		m_axisCurrent	=	&m_axisRotation;
	else
		m_axisCurrent	=	0;
	this->m_treeView.ClearSelect();
	//if(m_selects.size() > 0)
	//{
	//	Phantom::SceneObject* obj = m_selects[m_selects.size() - 1].GetObjectPtr();
	//	Phantom::AABox box;
	//	Phantom::float3 off;
	//	if(!GetObjectBox(obj, box, off))//if(!obj)return 0;
	//		return;
	//	wchar_t buf[128];
	//	swprintf(buf, L"%.03f", box._min.x);
	//	GetAlignDialog3D()->m_edAlignMinX.SetWindowTextW(buf);
	//	swprintf(buf, L"%.03f", box._min.y);
	//	GetAlignDialog3D()->m_edAlignMinY.SetWindowTextW(buf);
	//	swprintf(buf, L"%.03f", box._min.z);
	//	GetAlignDialog3D()->m_edAlignMinZ.SetWindowTextW(buf);
	//	//
	//	swprintf(buf, L"%.03f", box._max.x);
	//	GetAlignDialog3D()->m_edAlignMaxX.SetWindowTextW(buf);
	//	//
	//	swprintf(buf, L"%.03f", box._max.y);
	//	GetAlignDialog3D()->m_edAlignMaxY.SetWindowTextW(buf);
	//	//
	//	swprintf(buf, L"%.03f", box._max.z);
	//	GetAlignDialog3D()->m_edAlignMaxZ.SetWindowTextW(buf);
	//	//
	//	swprintf(buf, L"%.03f", box._max.x - box._min.x);//rc.GetWidth());
	//	GetAlignDialog3D()->m_edAlignWidth.SetWindowTextW(buf);
	//	//
	//	swprintf(buf, L"%.03f", box._max.y - box._min.y);//rc.GetHeight());
	//	GetAlignDialog3D()->m_edAlignHeight.SetWindowTextW(buf);
	//	//
	//	swprintf(buf, L"%.03f", box._max.z - box._min.z);//rc.GetHeight());
	//	GetAlignDialog3D()->m_edAlignLength.SetWindowTextW(buf);
	//	//
	//	float spaceX	=	0;
	//	float spaceY	=	0;
	//	float spaceZ	=	0;
	//	float maxX	=	-10000000;
	//	float maxY	=	-10000000;
	//	float minX	=	10000000;
	//	float minY	=	10000000;
	//	float maxZ	=	-10000000;
	//	float minZ	=	10000000;
	//	for(int i=0;i<m_selectObjects.size();i++)
	//	{
	//		Phantom::SceneObject* obj = m_selectObjects[i].objectPtr;
	//		if(!obj)continue;
	//		Phantom::AABox box = *obj->GetBox();
	//		spaceX	+=	box._max.x - box._min.x;
	//		spaceY	+=	box._max.y - box._min.y;
	//		spaceZ	+=	box._max.z - box._min.z;
	//		maxX	=	max(maxX, box._max.x);
	//		minX	=	min(minX, box._min.x);
	//		maxY	=	max(maxY, box._max.y);
	//		minY	=	min(minY, box._min.y);
	//		maxZ	=	max(maxZ, box._max.z);
	//		minZ	=	min(minZ, box._min.z);
	//	}
	//	//
	//	swprintf(buf, L"%.03f", (maxX - minX - spaceX) / (float)m_selectObjects.size());
	//	GetAlignDialog3D()->m_edAlignSpaceX.SetWindowTextW(buf);
	//	//
	//	swprintf(buf, L"%.03f", (maxY - minY - spaceY) / (float)m_selectObjects.size());
	//	GetAlignDialog3D()->m_edAlignSpaceY.SetWindowTextW(buf);
	//	//
	//	swprintf(buf, L"%.03f", (maxZ - minZ - spaceZ) / (float)m_selectObjects.size());
	//	GetAlignDialog3D()->m_edAlignSpaceZ.SetWindowTextW(buf);
	//	//
	//}

	//}
}

BOOL	GCSceneEditor3D::RebuildObjectsCenter()
{
	double dX	=	0;
	double dY	=	0;
	double dZ	=	0;
	for(int i=0;i<m_selects.size();i++)//std::vector<SelectEditor>::iterator it = m_selectObjects.begin(); it != m_selectObjects.end();)
	{
		dX		+=	m_selects[i].m_downMatrix._41;
		dY		+=	m_selects[i].m_downMatrix._42;
		dZ		+=	m_selects[i].m_downMatrix._43;
	}
	if(m_selects.size() == 0)
		return FALSE;
	double dV	=	(1.0f / (double)m_selects.size());
	m_selectCenter.setxyz((float)(dX * dV), (float)(dY * dV), (float)(dZ * dV));
	//
	Phantom::matrix4x4 mat;
	mat.identity();
	mat.SetPosition( m_selectCenter );
	if(m_selects.size() == 1)
		mat.SetPosition(m_selects.at(0).GetPosition());
	//
	m_axisMoving.m_viewWorld		=	mat;
	m_axisRotation.m_viewWorld		=	mat;
	m_axisScaling.m_viewWorld		=	mat;
	return TRUE;
}

VOID						GCSceneEditor3D::OnRenderingObject()
{

	Phantom::matrix4x4 mat;
	mat.identity();
	mat.SetPosition( this->m_selectCenter );
	if(this->m_selects.size() > 0)
	{
		//
		Phantom::AABox box;
		box._max.setxyz(To3DValue(.5f), To3DValue(.5f), To3DValue(1.0f));
		box._min.setxyz(-To3DValue(.5f), -To3DValue(.5f), 0.0f);
		//
		Phantom::matrix4x4 mat;
		mat.identity();
		Phantom::g_manager.SetWorldMatrix(&mat);
		int index = 0;
		for(std::vector<SelectEditor>::iterator it = m_selects.begin(); it != m_selects.end();)
		{
			if(m_objectEditMode == ObjectEditMode_Copy)
			{
				Phantom::g_manager.SetWorldMatrix(&it->m_newMatrix);
				Phantom::g_manager.DrawBox(box, 0xffff0000, true);
			}
			else
			{
				Phantom::AABox box;
				if(it->GetAABox(box)){
					if(index == 0)
						Phantom::g_manager.DrawBox(box, 0xff00ff00, false);
					else
						Phantom::g_manager.DrawBox(box, 0xffff0000, false);
					index++;
				}
			}
			//
			it++;
		}
	}
	else
		m_axisCurrent			=	NULL;
	if(m_objectEditMode == ObjectEditMode_Select)
	{
		CPoint							pos;
		GetCursorPos(&pos);
		ScreenToClient(GetDeviceWindow(), &pos);
		Phantom::g_manager.DrawRect2D(Phantom::Rect(min(pos.x, m_downPoint.x), min(pos.y, m_downPoint.y), max(pos.x, m_downPoint.x), max(pos.y, m_downPoint.y)), Phantom::color4(0xffff0000), false);
	}
	//Phantom::g_manager->Clear(Phantom::CLEAR_ZBUFFER);

}

VOID						GCSceneEditor3D::OnFrameMoveObject()
{
}

//
//BOOL		SetObjectBox(Phantom::SceneObject* obj, Phantom::AABox& ret){
//	if(!obj->GetSceneGeometryPtr())
//		return false;
//	IGeometryGroupData* data = obj->GetSceneGeometryPtr()->GetGeometryGroup()->GetDataPtr();
//	Phantom::AABox box = data->GetObjectRegion().aabox;
//	PBoxVertices v;
//	v.setup(box._min, box._max);
//	Phantom::matrix4x4& mat = obj->GetSceneGeometryPtr()->GetGeometryGroup()->GetWorldMatrix();
//	for(int i=0;i<8;i++)
//	{
//		v.v[i].transform(v.v[i], mat);
//	}
//	Phantom::AABox box;
//	box._min.setxyz(10000000, 10000000, 10000000);
//	box._max.setxyz(-10000000, -10000000, -10000000);
//	for(int i=0;i<8;i++)
//	{
//		box._min.x = min(v.v[i].x, box._min.x);
//		box._min.y = min(v.v[i].y, box._min.y);
//		box._min.z = min(v.v[i].z, box._min.z);
//		box._max.x = max(v.v[i].x, box._max.x);
//		box._max.y = max(v.v[i].y, box._max.y);
//		box._max.z = max(v.v[i].z, box._max.z);
//	}
//	ret = box;
//	return true;
//}
VOID		GCSceneEditor3D::AlignObjects(AlignType type, int value)
{
	if(m_editorMode == EditorMode_ObjectMove || m_editorMode == EditorMode_ObjectScale || m_editorMode == EditorMode_ObjectRotation)
	{
	}
	else
		return;
	bool bSpace	=	false;
	int	begin	=	0;
	for(int i=(int)m_selects.size() - 1;i>=0;i--)
	{
		SelectEditor& data = m_selects[i];
		Phantom::AABox box;
		Phantom::float3 off;
		if(!data.GetObjectBox(box, off))
			continue;
		float width			=	box._max.x - box._min.x;
		float height		=	box._max.y - box._min.y;
		float length		=	box._max.z - box._min.z;
		switch(type)
		{
		case AlignType_Left:
			{
				box._min.x		=	value;
				box._max.x		=	box._min.x + width;
			}
			break;
		case AlignType_Top:
			{
				box._min.y		=	value;
				box._max.y		=	box._min.y + height;
			}
			break;
		case AlignType_Right:
			{
				box._max.x		=	value;
				box._min.x		=	box._max.x - width;
			}
			break;
		case AlignType_Bottom:
			{
				box._max.y		=	value;
				box._min.y		=	box._max.y - height;
			}
			break;
		case AlignType_MinZ:
			{
				box._min.z		=	value;
				box._max.z		=	box._min.z + height;
			}
			break;
		case AlignType_MaxZ:
			{
				box._max.z		=	value;
				box._min.z		=	box._max.z - height;
			}
			break;
		case AlignType_Width:
			{
				box._max.x	=	box._min.x + value;
			}
			break;
		case AlignType_Height:
			{
				box._max.y	=	box._min.y + value;
			}
			break;
		case AlignType_Length:
			{
				box._max.z	=	box._min.z + value;
			}
			break;
		case AlignType_OffsetX:
			{
				box._min.x += value;
				box._max.x += value;
			}
			break;
		case AlignType_OffsetY:
			{
				box._min.y += value;
				box._max.y += value;
			}
			break;
		case AlignType_OffsetZ:
			{
				box._min.z += value;
				box._max.z += value;
			}
			break;
		case AlignType_SpaceX:
			{
				if(!bSpace)
				{
					bSpace	=	true;
					begin	=	box._min.x;
				}
				else
				{
					box._min.x		=	begin;
					box._max.x	=	box._min.x + width;
				}
				begin	+=	width;
				begin	+=	value;
			}
			break;
		case AlignType_SpaceY:
			{
				if(!bSpace)
				{
					bSpace	=	true;
					begin	=	box._min.y;
				}
				else
				{
					box._min.y		=	begin;
					box._max.y	=	box._min.y + height;
				}
				begin	+=	height;
				begin	+=	value;
			}
			break;
		case AlignType_SpaceZ:
			{
				if(!bSpace)
				{
					bSpace	=	true;
					begin	=	box._min.z;
				}
				else
				{
					box._min.z		=	begin;
					box._max.z	=	box._min.z + length;
				}
				begin	+=	length;
				begin	+=	value;
			}
			break;
		}
		data.SetPosition(box._min + off);
		this->SetModifyed(true);
	}
}

BOOL		GCSceneEditor3D::IsAlignEnable()
{
	return (m_selects.size() > 1);
}

INT			GCSceneEditor3D::GetAlignValue(AlignType type)
{
	if(m_selects.size() > 0)
	{
		SelectEditor& ed = m_selects[m_selects.size() - 1];
		Phantom::AABox box;
		Phantom::float3 off;
		if(!ed.GetObjectBox(box, off))//if(!obj)return 0;
			return 0;
		//Phantom::AABox box = *obj->GetBox();
		switch(type)
		{
		case AlignType_Left:
			return box._min.x;
		case AlignType_Top:
			return box._min.y;
		case AlignType_Right:
			return box._max.x;
		case AlignType_Bottom:
			return box._max.y;
		case AlignType_MinZ:
			return box._min.z;
		case AlignType_MaxZ:
			return box._max.z;
		case AlignType_Width:
			return (box._max.x - box._min.x);
		case AlignType_Height:
			return (box._max.y - box._min.y);
		case AlignType_Length:
			return (box._max.z - box._min.z);
		case AlignType_SpaceX:
		case AlignType_SpaceY:
		case AlignType_SpaceZ:
		case AlignType_MatrixX:
		case AlignType_MatrixY:
			{
				float spaceX	=	0;
				float spaceY	=	0;
				float spaceZ	=	0;
				float maxX	=	-100000000;
				float maxY	=	-100000000;
				float minX	=	100000000;
				float minY	=	100000000;
				float maxZ	=	-100000000;
				float minZ	=	100000000;
				for(int i=0;i<m_selects.size();i++)
				{
					SelectEditor& ed = m_selects[i];
					Phantom::AABox temp;
					if(!ed.GetAABox(temp))
						continue;
					spaceX	+=	(temp._max.x - temp._min.x);//rc.GetWidth();
					spaceY	+=	(temp._max.y - temp._min.y);//rc.GetHeight();
					spaceZ	+=	(temp._max.z - temp._min.z);//rc.GetHeight();
					maxX	=	max(maxX, box._max.x);
					minX	=	min(minX, box._min.x);
					maxY	=	max(maxY, box._max.y);
					minY	=	min(minY, box._min.y);
					maxZ	=	max(maxZ, box._max.z);
					minZ	=	min(minZ, box._min.z);
				}
				if(type == AlignType_SpaceX || type == AlignType_MatrixX)
					return (maxX - minX - spaceX) / (float)m_selects.size();
				if(type == AlignType_SpaceY || type == AlignType_MatrixY)
					return (maxY - minY - spaceY) / (float)m_selects.size();
				if(type == AlignType_SpaceZ)
					return (maxZ - minZ - spaceZ) / (float)m_selects.size();
			}
			break;
		}
	}
	return -1;
}
