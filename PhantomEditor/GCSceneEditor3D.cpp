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
#include "resource.h"
#include "GCSceneEditor3D.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"
#include "SceneCreateDlg.h"
struct	SceneCopyData3D
{
	TreeItemType		type;
	GCSceneEditor3D*	editor;
	SceneCopyData3D()
	{
		editor	=	0;
	}
};

std::vector<SceneCopyData3D>		g_copyScene3Ds;

VOID		SetSceneCopy3D(GCSceneEditor3D* e, std::vector<TreeItemType*>* vs)
{
	g_copyScene3Ds.clear();
	for(int i=0;i<vs->size();i++)
	{
		SceneCopyData3D ud;
		ud.editor	=	e;
		ud.type		=	*vs->at(i);
		g_copyScene3Ds.push_back(ud);
	}
}

BOOL		IsCanPasteScene3D(int nType)
{
	for(int i=0;i<g_copyScene3Ds.size();i++)
	{
		if(g_copyScene3Ds[i].type.type == nType)
		{
			return true;
		}
	}
	return false;
}

float BrushImage::GetHeight(float fX, float fY)
{
	if(fX < 0 || fY < 0 || fX > 1.0f || fY > 1.0f)
		return 0.0f;
	int x	=	(int)(fX * (float)width);
	int y	=	(int)(fY * (float)height);
	return (float)heightBuffer[x + y * width] / 255.0f;
}

float BrushImage::GetHeightPixel(INT nX, INT nY)
{
	if(nX < 0 || nY < 0 || nX >= width || nY >= height)return 0.0f;
	return (float)heightBuffer[nX + nY * width] / 255.0f;
}

VOID BrushImage::LoadBuffer()
{
	if(heightBuffer)return;
	CImage img;
	if(FAILED(img.Load(fileName)))
		return;
	unsigned char* bits	=	(unsigned char*)img.GetBits();
	assert(bits);
	int pitch		=	img.GetPitch();
	int bpp			=	1;
	if(img.GetBPP() == 8)
		bpp		=	1;
	else if(img.GetBPP() == 16)
		bpp		=	2;
	else if(img.GetBPP() == 24)
		bpp		=	3;
	else if(img.GetBPP() == 32)
		bpp		=	4;
	else
	{
		assert(false);
		return;
	}
	this->width		=	img.GetWidth();
	this->height	=	img.GetHeight();
	heightBuffer	=	(unsigned char*)malloc(sizeof(unsigned char) * width * height);
	for(int y=0;y<height;y++)
	for(int x=0;x<width;x++)
	{
		heightBuffer[x + y * width] = bits[y * pitch + x * bpp];
	}
	if(imageSmall.IsNull())
	{
		imageSmall.Create(128, 128, 24);
		HDC dcSmall =	imageSmall.GetDC();
		HDC dc		=	img.GetDC();
		::SetStretchBltMode(dcSmall, COLORONCOLOR);
		::SetStretchBltMode(dc, COLORONCOLOR);
		StretchBlt(dcSmall, 0, 0, 128, 128, dc, 0, 0, width, height, SRCCOPY);
		//
		imageSmall.ReleaseDC();
		img.ReleaseDC();
	}
}

VOID BrushImage::FreeBuffer()
{
	if(heightBuffer)
		free(heightBuffer);
	heightBuffer	=	0;
}

GCSceneEditor3D::GCSceneEditor3D(CPhantomGCDoc* docPtr)
	:GCViewBase(docPtr, Language(L"3D场景编辑"), IDB_SCENE_VIEW_BMP)
{
	//m_pointTexture		=	0;
	m_regionIndex		=	0;
	m_scene3D			=	0;
	m_nHeightPointCount	=	0;
	m_lightSelect		=	0;
	m_brushType			=	BrushType_Circle;
	m_editorMode		=	EditorMode_None;
	m_bUseBrushTexture	=	false;
	m_bBrush01Mode		=	false;
	m_brushPtr			=	0;
	m_bShowDisableMove	=	false;
	memset(m_savedData, 0, sizeof(m_savedData));
	memset(m_savedDataT, 0, sizeof(m_savedDataT));
	m_bShowObjectFile	=	false;
	m_bMouseMove		=	false;
	m_bDoEvent			=	false;
	m_objectEditMode	=	ObjectEditMode_None;
	m_fHeightRadius		=	GetConfigDouble("HeightRadius", To3DValue(1.0f));
	for(int i=1;i<EditorMode_Count;i++)
	{
		SceneEditorInfo& info	=	this->m_editorInfo[i];
		char buf[128];
		sprintf(buf, "heightInfo%d_var", i);
		info.fVar		=	GetConfigDouble(buf, To3DValue(1.0f));
		sprintf(buf, "heightInfo%d_normal", i);
		info.fNormalVar	=	GetConfigDouble(buf, (1.0f));
		sprintf(buf, "heightInfo%d_power", i);
		info.fPower		=	GetConfigDouble(buf, (1.0f));
		sprintf(buf, "heightInfo%d_enable", i);
		info.bEnable	=	GetConfigBool(buf, true);
		sprintf(buf, "heightInfo%d_min", i);
		info.fMinValue	=	GetConfigDouble(buf, 0.0f);
		sprintf(buf, "heightInfo%d_max", i);
		info.fMaxValue	=	GetConfigDouble(buf, 1.0f);
	}
	m_nTextureBlendChannel	=	0;
	m_fCalculateTime		=	0;
	m_wireframeColor.from(GetConfigInt("SceneWireframe", 0xff3f3f3f), 1.0f);
	//
	m_bCopyTextureData	=	false;
	memset(m_copyTextureData, 0, sizeof(m_copyTextureData));
	memset(m_vEditRegions, 0, sizeof(m_vEditRegions));
	m_nEditRegionCount	=	0;
	m_downPoint.SetPoint(0, 0);
	m_bLButtonDown		=	false;
	m_bBrushMode		=	GetConfigInt("SceneBrush", 0);
	m_axisCurrent		=	0;
	m_axisMoving.m_axisCallback		=	this;
	m_axisScaling.m_axisCallback	=	this;
	m_axisRotation.m_axisCallback	=	this;
	m_axisRotation.m_scene3D		=	this;
	//
	//m_character			=	0;
	//m_bShowCharacter	=	false;
	m_bIsPlaying		=	false;
	m_bIsPause			=	false;
	m_bLoopMode			=	false;
}

GCSceneEditor3D::~GCSceneEditor3D(void)
{
	for(int i=0;i<10000;i++)
	{
		if(m_savedData[i])
			delete m_savedData[i];
		m_savedData[i] = 0;
		if(m_savedDataT[i])
			delete m_savedDataT[i];
		m_savedDataT[i] = 0;
	}
	m_selects.clear();
	CloseFile();
}

BOOL		GCSceneEditor3D::IsTextureEditorMode()
{
	switch(m_editorMode)
	{
	case EditorMode_TextureAdd:
	case EditorMode_TextureSet:
	case EditorMode_TextureAvg:
	case EditorMode_TextureUVMoving:
	case EditorMode_TextureUVScaling:
	case EditorMode_TextureUVRotation:
	case EditorMode_HeightGridEnable:
	case EditorMode_RegionSet:
		return true;
	}
	return false;
}

VOID		GCSceneEditor3D::SetEditorMode(int mode)
{
	this->m_editorMode	=	(EditorMode)mode;
	//
	if(m_editorMode == 0)
		return;
	//
	if(m_editorMode == EditorMode_ObjectMove)
		m_axisCurrent	=	&m_axisMoving;
	else if(m_editorMode == EditorMode_ObjectScale)
		m_axisCurrent	=	&m_axisScaling;
	else if(m_editorMode == EditorMode_ObjectRotation)
		m_axisCurrent	=	&m_axisRotation;
	else
		m_axisCurrent	=	0;
	switch(m_editorMode)
	{
	case EditorMode_HeightAdd:
	case EditorMode_HeightAvg:
	case EditorMode_HeightAlign:
	case EditorMode_HeightSet:
	case EditorMode_HeightGridVisible:
	case EditorMode_HeightGridEnable:
	case EditorMode_RegionSet:
	case EditorMode_AddGrass:
	case EditorMode_RemoveGrass:
		SetCurrentProperties(&m_editorProperties, L"编辑属性", NULL);
		m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowMoveGrids, false);
		m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowRegionSet, false);
		break;
	case EditorMode_ObjectMove:
	case EditorMode_ObjectScale:
	case EditorMode_ObjectRotation:
		SetCurrentProperties(0, L"单位属性", NULL);
		m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowMoveGrids, false);
		m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowRegionSet, false);
		return;
	case EditorMode_LineEditorMode:
		SetCurrentProperties(0, L"线条属性", NULL);
		m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowMoveGrids, false);
		m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowRegionSet, false);
		return;
	case EditorMode_TextureAdd:
	case EditorMode_TextureSet:
	case EditorMode_TextureAvg:
	case EditorMode_TextureUVMoving:
	case EditorMode_TextureUVScaling:
	case EditorMode_TextureUVRotation:
		SetCurrentProperties(&m_editorProperties, L"编辑属性", &this->m_layerImages);
		break;
	}
	//
	m_editorProperties.EnableRender(false);
	SceneEditorInfo& info	=	m_editorInfo[m_editorMode];
	SetPropFloat(this->m_editorRadius, m_fHeightRadius);
	SetPropFloat(this->m_editorVar, info.fVar);
	SetPropFloat(this->m_editorNormalVar, info.fNormalVar);
	SetPropFloat(this->m_editorPower, info.fPower);
	SetPropBool(this->m_editorEnable, info.bEnable);
	//
	SetPropFloat(this->m_editorMinValue, info.fMinValue);
	SetPropFloat(this->m_editorMaxValue, info.fMaxValue);
	//
	ShowRegions();
	//
	m_editorMinValue->Show(false);
	m_editorMaxValue->Show(false);
	m_editorGrassSelect->Show(false);
	m_editorVar->SetName(Language(L"高度值"));
	switch(m_editorMode)
	{
	case EditorMode_HeightAdd:
		{
			m_editorVar->Show(true);
			m_editorNormalVar->Show(false);
			m_editorPower->Show(true);
			m_editorEnable->Show(false);
		}
		break;
	case EditorMode_HeightAvg:
		{
			m_editorVar->Show(false);
			m_editorNormalVar->Show(false);
			m_editorPower->Show(true);
			m_editorEnable->Show(false);
		}
		break;
	case EditorMode_HeightAlign:
		{
			m_editorVar->Show(true);
			m_editorNormalVar->Show(false);
			m_editorPower->Show(false);
			m_editorEnable->Show(false);
		}
		break;
	case EditorMode_HeightSet:
		{
			m_editorVar->Show(true);
			m_editorNormalVar->Show(false);
			m_editorPower->Show(true);
			m_editorEnable->Show(false);
		}
		break;
	case EditorMode_HeightGridVisible:
		{
			m_editorVar->Show(false);
			m_editorNormalVar->Show(false);
			m_editorPower->Show(false);
			m_editorEnable->Show(true);
		}
		break;
	case EditorMode_RegionSet:
		{
			m_editorVar->Show(false);
			m_editorNormalVar->Show(false);
			m_editorPower->Show(false);
			m_editorEnable->Show(true);
			m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowRegionSet, true);
			m_scene3D->RebuildRegionSets(this->m_regionIndex);
		}
		break;
	case EditorMode_HeightGridEnable:
		{
			m_editorVar->Show(false);
			m_editorNormalVar->Show(false);
			m_editorPower->Show(false);
			m_editorEnable->Show(true);
			m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowMoveGrids, true);
			m_scene3D->RebuildAllMoveGrids();
		}
		break;
	case EditorMode_AddGrass:
	case EditorMode_RemoveGrass:
		{
			m_editorVar->SetName(Language(L"刷草数量"));
			m_editorVar->Show(true);
			m_editorNormalVar->Show(false);
			m_editorPower->Show(false);
			m_editorEnable->Show(false);
			m_editorGrassSelect->Show(true);
		}
		break;
	case EditorMode_TextureAdd:
	case EditorMode_TextureSet:
	case EditorMode_TextureAvg:
		{
			m_editorVar->Show(false);
			m_editorNormalVar->Show(true);
			m_editorPower->Show(true);
			m_editorEnable->Show(false);
			m_editorMinValue->Show(true);
			m_editorMaxValue->Show(true);
			RefreshLayerImages();
		}
		break;
	case EditorMode_TextureUVMoving:
	case EditorMode_TextureUVScaling:
	case EditorMode_TextureUVRotation:
		{
			m_editorVar->Show(false);
			m_editorNormalVar->Show(false);
			m_editorPower->Show(false);
			m_editorEnable->Show(false);
		}
		break;
	}
	if(m_editorMode == EditorMode_TextureSet)
	{
		m_editorEnable->SetName(Language(L"是否随机"));
		m_editorEnable->Show(true);
	}
	else
		m_editorEnable->SetName(Language(L"开关"));
	m_editorProperties.EnableRender(true);
}

void		GCSceneEditor3D::OnFrameMove()	//每次更新
{
	Phantom::float3 pos	=	Phantom::GetCameraPtr()->GetTarget();
	float fSpeed = Phantom::g_manager.GetElapsedTime() * To3DValue(10.0f);
	if(HIBYTE(GetKeyState(VK_SHIFT)))
		fSpeed *= 0.2f;
	Phantom::float3 xdir,ydir;
	Phantom::GetCameraPtr()->GetCameraOffset(&xdir, &ydir);
	Phantom::float3 off(0,0,0);
	char bChanged = false;
	static char s_bControl = false;
	if(HIBYTE(GetKeyState('W'))){
		off = ydir * fSpeed;
		bChanged = true;
	}
	else if(HIBYTE(GetKeyState('S'))){
		off -= ydir * fSpeed;
		bChanged = true;
	}
	if(HIBYTE(GetKeyState('A'))){
		off -= xdir * fSpeed;
		bChanged = true;
	}
	else if(HIBYTE(GetKeyState('D'))){
		off += xdir * fSpeed;
		bChanged = true;
	}
	if(!bChanged)
		s_bControl = false;
	else if(HIBYTE(GetKeyState(VK_CONTROL)))
		s_bControl = true;
	if(bChanged&&!s_bControl&&::GetFocus()==GetDeviceWindow()){
		Phantom::GetCameraPtr()->Offset(off);
	}
	//IUnit3DRender* unit	=	m_character;
	//if(this->m_bShowCharacter)
	//{
	//	if(m_controlObject.id > 0)
	//	{
	//		SceneObjectData* obj	=	m_scene3D->GetObjectPtr(m_controlObject);
	//		if(obj && obj->iObjectPtr && obj->iObjectPtr->GetUnit3DPtr())
	//		{
	//			unit	=	obj->iObjectPtr->GetUnit3DPtr()->GetUnit3DRender();
	//			pos.z			=	obj->iObjectPtr->GetPosition()->z;
	//			Phantom::float3 length	=	*obj->iObjectPtr->GetPosition() - pos;
	//			length.z		=	0;
	//			obj->iObjectPtr->SetPosition(pos);
	//			pos		=	*obj->iObjectPtr->GetPosition();
	//			if(unit)
	//			{
	//				if(length.length() >= r.fElapsedTime)
	//				{
	//					if(unit->GetActiveActionType() == Unit3DActionType_Standard)
	//						unit->SetActiveAction(Unit3DActionType_Run);
	//				}
	//				else if(unit->GetActiveActionType() == Unit3DActionType_Run)
	//					unit->SetActiveAction(Unit3DActionType_Standard);
	//			}
	//			//
	//			Phantom::float3 target	=	Phantom::GetCameraPtr()->GetTarget();
	//			Phantom::float3 dir		=	Phantom::GetCameraPtr()->GetEye() - target;
	//			Phantom::float3 oldDir	=	dir;
	//			dir.z			=	0;
	//			dir.normalize();
	//			if(unit)
	//				unit->SetRotationZ(ToAngle(getRotateZ(dir)));
	//			//
	//			target			=	pos;
	//			target.z		=	pos.z + To3DValue(1.8f);
	//			if(!GetSelectKeyboard())
	//			{
	//				Phantom::GetCameraPtr()->SetTarget(target);
	//				Phantom::GetCameraPtr()->SetEye(target + oldDir);
	//			}
	//		}
	//	}
	//	else if(m_character)
	//	{
	//	pos.z			=	m_character->GetPosition()->z;
	//	Phantom::float3 length	=	*m_character->GetPosition() - pos;
	//	length.z		=	0;
	//	m_character->SetPosition(pos);
	//	pos		=	*m_character->GetPosition();
	//	if(length.length() >= r.fElapsedTime)
	//	{
	//		if(m_character->GetActiveActionType() == Unit3DActionType_Standard)
	//			m_character->SetActiveAction(Unit3DActionType_Run);
	//	}
	//	else if(m_character->GetActiveActionType() == Unit3DActionType_Run)
	//		m_character->SetActiveAction(Unit3DActionType_Standard);
	//	//
	//	Phantom::float3 target	=	Phantom::GetCameraPtr()->GetTarget();
	//	Phantom::float3 dir		=	Phantom::GetCameraPtr()->GetEye() - target;
	//	Phantom::float3 oldDir	=	dir;
	//	dir.z			=	0;
	//	dir.normalize();
	//	m_character->SetRotationZ(ToAngle(getRotateZ(dir)));
	//	//
	//	target			=	pos;
	//	target.z		=	pos.z + To3DValue(1.8f);
	//	if(!GetSelectKeyboard())
	//	{
	//		Phantom::GetCameraPtr()->SetTarget(target);
	//		Phantom::GetCameraPtr()->SetEye(target + oldDir);
	//	}
	//	//
	//	m_character->FrameMove();
	//	}
	//}
	//
	//this->m_scene3D->ShowWireFrame(IsShowUIGrid());
	//this->m_scene3D->SetWireframeColor(this->m_wireframeColor);
	//
	//TreeItemType* type	=	GetSelectedItem();
	//if(type && type->type == this->SCENE3D_SCA_TRACK && this->m_bIsPause)
	//{
	//	ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
	//	if(ani)
	//	{
	//		ISCA_Base* base	=	ani->GetScaManager().SearchDataPtr(type->p2);
	//		if(base)
	//		{
	//			ISCA_Track* track	=	base->GetTrackManager().SearchDataPtr(type->p3);
	//			float fTime			=	(float)track->GetBeginTrackIndex() / (float)TRACK_SECOND_LENGTH;
	//			fTime				+=	(this->m_trackDlg.GetPlayValue() / (track->GetFrameTimeLength() * TRACK_SECOND_LENGTH)) * (track->GetEndTrackIndex() - track->GetBeginTrackIndex()) / (float)TRACK_SECOND_LENGTH;
	//			ani->SetPlayTime(fTime);
	//		}
	//	}
	//}
	switch(m_editorMode)
	{
	case EditorMode_HeightAdd:
	case EditorMode_HeightAvg:
	case EditorMode_HeightAlign:
	case EditorMode_HeightSet:
	case EditorMode_HeightGridVisible:
	case EditorMode_HeightGridEnable:
	case EditorMode_HeightMapAdd:
	case EditorMode_HeightMapSet:
	case EditorMode_TextureAdd:
	case EditorMode_TextureSet:
	case EditorMode_TextureAvg:
	case EditorMode_TextureUVMoving:
	case EditorMode_TextureUVScaling:
	case EditorMode_TextureUVRotation:
	case EditorMode_AddGrass:
	case EditorMode_RemoveGrass:
	case EditorMode_RegionSet:
		OnFrameMoveHeight();
		break;
	case EditorMode_ObjectMove:
	case EditorMode_ObjectScale:
	case EditorMode_ObjectRotation:
		OnFrameMoveObject();
		break;
	case EditorMode_LineEditorMode:
		OnFrameMoveLine();
		break;
	}
}

void		GCSceneEditor3D::OnRenderingEnd()
{
	Phantom::g_manager->SetCullMode(Phantom::CullMode_Double);
	switch(m_editorMode)
	{
	case EditorMode_HeightAdd:
	case EditorMode_HeightAvg:
	case EditorMode_HeightAlign:
	case EditorMode_HeightSet:
	case EditorMode_HeightGridVisible:
	case EditorMode_HeightGridEnable:
	case EditorMode_RegionSet:
	case EditorMode_HeightMapAdd:
	case EditorMode_HeightMapSet:
	case EditorMode_TextureAdd:
	case EditorMode_TextureSet:
	case EditorMode_TextureAvg:
	case EditorMode_TextureUVMoving:
	case EditorMode_TextureUVScaling:
	case EditorMode_TextureUVRotation:
	case EditorMode_AddGrass:
	case EditorMode_RemoveGrass:
		break;
	case EditorMode_ObjectMove:
	case EditorMode_ObjectScale:
	case EditorMode_ObjectRotation:
		break;
	case EditorMode_LineEditorMode:
		break;
	}
	//if(m_bShowDisableMove)
	//	m_scene3D->RenderMoveEnable(r, Phantom::GetCameraPtr());
	//if(m_bShowObjectFile)
	//{
	//	for(INT i=0;i<m_scene3D->GetObjectCount();i++)
	//	{
	//		Phantom::SceneObject* obj	=	m_scene3D->GetObjectPtr(i)->iObjectPtr;
	//		if(!obj)
	//			continue;
	//		Phantom::float3 m = *obj->GetPosition();
	//		Phantom::Pixel p;
	//		Phantom::GetCameraPtr()->Get2DFrom3D(p, m);
	//		if(p.x > 0 && p.y > 0 && p.x < Phantom::g_manager.GetBufferWidth()
	//			&& p.y < Phantom::g_manager.GetBufferHeight())
	//		{
	//			wchar_t szUnit[256];
	//			szUnit[::MultiByteToWideChar(CP_ACP, 0, obj->m_(), -1, szUnit, 256)] = 0;
	//			GetDlgResManager()->DrawTextW(szUnit, &PRect(p.x, p.y, p.x + 300, p.y + 20), 0, Phantom::color4(0.0f, 0.0f, 0.0f, 1.0f), -1, 1, Phantom::color4(1,1,0, 1), DRAW_TEXT_TOP|DRAW_TEXT_LEFT);
	//		}
	//	}
	//}
	//static Phantom::float3 lastTarget = Phantom::GetCameraPtr()->GetTarget();
	//Phantom::PickPoint pick;
	//static Phantom::AABox pickBox, pickNormal;
	//static bool bPick = false;
	//if(this->m_scene3D->PickLinear(lastTarget, Phantom::GetCameraPtr()->GetTarget(), pick))
	//{
	//	bPick	=	true;
	//	Phantom::AABox box;
	//	pickBox._min	=	pick.pos + Phantom::float3(-1,-1,-1);
	//	pickBox._max	=	pick.pos + Phantom::float3(1,1,1);
	//	pick.normal.normalize();
	//	pick.normal	*=	To3DValue(1.0f);
	//	pickNormal._min	=	pick.pos + pick.normal + Phantom::float3(-1,-1,-1);
	//	pickNormal._max	=	pick.pos + pick.normal + Phantom::float3(1,1,1);
	//}
	//if(bPick)
	//{
	//	GetRenderHelperPtr()->drawBox(pickBox, 0xffff0000, true);//this->m_pointTexture
	//	GetRenderHelperPtr()->drawBox(pickNormal, 0xffaf3f00, true);//this->m_pointTexture
	//}
	//lastTarget	=	Phantom::GetCameraPtr()->GetTarget();
	//Phantom::AABox box;
	//box._min	=	Phantom::GetCameraPtr()->GetTarget() + Phantom::float3(-1,-1,-1);
	//box._max	=	Phantom::GetCameraPtr()->GetTarget() + Phantom::float3(1,1,1);
	//GetRenderHelperPtr()->drawBox(box, 0xff00fff0, true);//this->m_pointTexture
	//box._min	=	lastTarget + Phantom::float3(-0.2f,-0.2f,-0.2f);
	//box._max	=	lastTarget + Phantom::float3(0.2f,0.2f,0.2f);
	//GetRenderHelperPtr()->drawBox(box, 0xff00ff00, true);//this->m_pointTexture
	if(m_axisCurrent)
	{
		//Phantom::g_manager->SetEnableZ(false);
		m_axisCurrent->DrawAxis();
		//Phantom::g_manager->SetEnableZ(true);
	}
}
bool		GCSceneEditor3D::OnWindowProc(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam)
{
	switch(uMsg){
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		{
			Phantom::GetCameraPtr()->GetCameraConfig(m_currentCamera);
			Phantom::SceneCameraData* d = m_scene3D->GetCameraDataPtr()->SearchDataPtr(m_currentCamera.id);
			if(d)
				*d = m_currentCamera;
		}
		break;
	}
	//if(uMsg == WM_KEYDOWN)
	//{
	//	if(wParam == VK_SPACE)
	//	{
	//		if(this->m_bShowCharacter && this->m_character)
	//			m_character->SetActiveAction(Unit3DActionType_Jump);
	//	}
	//}
	switch(m_editorMode)
	{
	case EditorMode_HeightAdd:
	case EditorMode_HeightAvg:
	case EditorMode_HeightAlign:
	case EditorMode_HeightSet:
	case EditorMode_HeightGridVisible:
	case EditorMode_HeightGridEnable:
	case EditorMode_RegionSet:
	case EditorMode_HeightMapAdd:
	case EditorMode_HeightMapSet:
	case EditorMode_TextureAdd:
	case EditorMode_TextureSet:
	case EditorMode_TextureAvg:
	case EditorMode_TextureUVMoving:
	case EditorMode_TextureUVScaling:
	case EditorMode_TextureUVRotation:
	case EditorMode_AddGrass:
	case EditorMode_RemoveGrass:
		return OnWindowProcHeight(hWnd, uMsg, wParam, lParam);
	case EditorMode_ObjectMove:
	case EditorMode_ObjectScale:
	case EditorMode_ObjectRotation:
		return OnWindowProcObject(hWnd, uMsg, wParam, lParam);
	case EditorMode_LineEditorMode:
		return OnWindowProcLine(hWnd, uMsg, wParam, lParam);
	}
	return false;
}

HICON		GCSceneEditor3D::GetViewerIcon()
{
	static HICON	hIcon	=	AfxGetApp()->LoadIcon(IDI_SCENE3D_FILE_ICON);
	static HICON	hIconR	=	AfxGetApp()->LoadIcon(IDI_Z_SCENE3D_FILE_ICON);
	if(this->m_bIsReadOnly)
		return hIconR;
	return hIcon;
}

VOID		GCSceneEditor3D::OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem)	//更新选择树形控件
{
	if(treePtr == &m_treeView)
	{
		OnChangeTreeSelect();
		return;
	}
	SetCurrentProperties(NULL, NULL, NULL);
}

VOID		GCSceneEditor3D::CloseFile(){
	//safe_release(m_character);
	SetConfigDouble("HeightRadius", m_fHeightRadius);
	for(int i=1;i<EditorMode_Count;i++)
	{
		SceneEditorInfo& info	=	this->m_editorInfo[i];
		char buf[128];
		sprintf(buf, "heightInfo%d_var", i);
		SetConfigDouble(buf, info.fVar);
		sprintf(buf, "heightInfo%d_normal", i);
		SetConfigDouble(buf, info.fNormalVar);
		sprintf(buf, "heightInfo%d_power", i);
		SetConfigDouble(buf, info.fPower);
		sprintf(buf, "heightInfo%d_enable", i);
		SetConfigBool(buf, info.bEnable);
		//
		sprintf(buf, "heightInfo%d_min", i);
		SetConfigDouble(buf, info.fMinValue);
		sprintf(buf, "heightInfo%d_max", i);
		SetConfigDouble(buf, info.fMaxValue);
	}
	SetConfigInt("SceneWireframe", m_wireframeColor.getRGB());
	//
	for(int i=0;i<m_brushs.size();i++)
		delete m_brushs[i];
	//
	m_scene3D = 0;
	////if(GetProjectPtr()->GetScenePtr() == this->m_scene3D)
	////	GetProjectPtr()->SetScenePtr(0);

	//safe_release(m_scene3D);
	////safe_release(m_pointTexture);
}

VOID		GCSceneEditor3D::GetNewFileName(wchar_t* szFile)
{
	wcscpy(szFile, Language(L"新场景"));
}
BOOL		GCSceneEditor3D::OpenFile(const wchar_t* szFile)	//打开文件,如果失败则会自动删除
{
	this->m_strFile	=	szFile;
	if(!__super::OpenFile(szFile))
		return false;
	if(m_strFile.size()==0){
		Phantom::Scene3DPtr scene;
		CSceneCreateDlg dlg(GetMainFrame());
		if(dlg.DoModal()!=IDOK)
			return false;
		if(!Phantom::g_manager.CreateScene3D(scene, To3DValue(dlg.m_fGridSize), dlg.m_nMapSizeLevel))
		{
			GetMainFrame()->MessageBox(Language(L"无法创建新场景"));
			return false;
		}
		this->m_bShowUIGrid = true;
		m_scene3D = scene;
	}
	else{
		wchar_t ext[256];
		_wsplitpath(m_strFile.c_str(), 0, 0, 0, ext);
		Phantom::Scene3DPtr scene;
		Phantom::NameT<512> filename = A(m_strFile.c_str()).buf;
		filename.setIsFile();
		const char* file = filename.t;
		FileName path = GetProjectPath();
		path.setIsFile();
		if(strnicmp(filename.t, path.t, path.size())==0){
			file += strlen(GetProjectPath());
		}
		if(!Phantom::g_manager.LoadScene3D(scene, file))
		{
			GetMainFrame()->MessageBox(Language(L"无法打开场景文件"));
			return false;
		}
		m_scene3D = scene;
	}
	m_scene3D->ActiveEnv(0);
	//m_scene3D->SetEditorMode(TRUE);
	//m_scene3D->SetScreenEffectPtr(GetProjectPtr()->m_screenEffectPtr);
	this->m_treeView.InsertItem(Language(L"SceneViewer", L"3D场景编辑器"), 0, 0);
	wchar_t dir[512];
	GetCurrentDirectory(512, dir);
	m_dlgObjects.Create(m_dlgObjects.IDD, GetMainFrame());
	m_dlgObjects.ShowWindow(SW_HIDE);
	m_dlgObjects.m_editors = this;
	//
	//std::wstring script = m_strFile.c_str();
	//script += L".lua";
	//m_editor = GetLuaEditor()->Open(A(script.c_str()));
	//GetLuaEditor()->Active(m_editor);
	//m_editor->m_callback = &m_editorCB;
	//wchar_t wFile[128];
	//_wsplitpath(m_strFile.c_str(), 0, 0, wFile, 0);
	//for(int i=0;i<Phantom::GetSceneEventCount();i++){
	//	char name[128];
	//	char fulls[256];
	//	Phantom::GetSceneEventName(name, fulls, i, A(wFile));
	//	m_editor->GetCodeMgr().AddEvent(Wutf8(name), Wutf8(fulls), EventType_SceneEvent, i);
	//}
	//m_editor->AnalyseAllCodes();
	//for(int i=0;i<100;i++)
	//{
	//	wchar_t buf[128];
	//	swprintf(buf, L"onevent_%d", i+1);
	//	wchar_t param[128];
	//	swprintf(param, L"a,b,c");
	//	m_dlgEditor.m_editor.GetCodeMgr().AddEvent(buf, param);
	//}

	//std::string strCharacter;
	//GetConfigText("SceneCharacter", strCharacter, "meshs\\man\\man.rpgunit");
	//m_character		=	LoadUnit3D(0, strCharacter.c_str());
	//if(m_character)
	//{
	//	m_character->SetScenePtr(m_scene3D);
	//	m_character->SetName("character test");
	//	m_character->SetNameColor(Phantom::color4(0,1,0,1));
	//	m_character->SetBloodColor(Phantom::color4(0, 1, 0, .5));
	//}
	//
	//m_smallMapInfo.Create(m_smallMapInfo.IDD);
	//m_smallMapInfo.ShowWindow(SW_HIDE);
	//m_shadowMapInfo.Create(m_shadowMapInfo.IDD);
	//m_renderBufferInfo.Create(m_renderBufferInfo.IDD);
	//m_renderBufferInfo.ShowWindow(SW_HIDE);
	//m_smallMapInfo.m_scene3D		=	m_scene3D;
	//m_shadowMapInfo.m_scene3D		=	m_scene3D;
	//m_renderBufferInfo.m_scene3D	=	m_scene3D;
	//m_smallMapInfo.OnChangeScene();
	//m_shadowMapInfo.OnChangeScene();
	//m_renderBufferInfo.OnChangeScene();
	//
	Phantom::Camera* camera = Phantom::GetCameraPtr();
	//
	m_layerImages.Create(m_layerImages.IDD, GetPropertiesDlgWindowPtr());
	m_layerImages.ShowWindow(SW_HIDE);
	m_layerImages.m_pIILCallback	=	this;
	m_layerImages.m_imageListView.m_imageManager.reloadImages();
	//
	////m_pointTexture	=	Phantom::LoadTexture(0, "default_init_zip\\point.dds", 0);
	//Phantom::GetCameraPtr()->SetViewMatrix(&Phantom::float3(500,100,100), &Phantom::float3(0,0,0));
	//Phantom::GetCameraPtr()->SetMoveSpeed(10.0f);
	//camera->SetMaxWheelDistance(To3DValue(10000.0f));
	//
	m_currentCamera	=	*m_scene3D->GetCameraDataPtr()->GetDataPtr(0);
	//m_currentCamera.name	=	"current";
	//
	camera->SetCameraConfig(m_currentCamera);//m_scene3D->GetSavedCameraData());
	//
	m_layerImages.SetImageName(0, A(Language(L"细节纹理-1")), true, A(Language(L"细节纹理-1")),true);
	m_layerImages.SetImageName(1, A(Language(L"高亮纹理-1")), true, A(Language(L"高亮纹理-1")),true);
	m_layerImages.SetImageName(2, A(Language(L"细节纹理-2")), true, A(Language(L"细节纹理-2")),true);
	m_layerImages.SetImageName(3, A(Language(L"高亮纹理-2")), true, A(Language(L"高亮纹理-2")),true);
	m_layerImages.SetImageName(4, A(Language(L"细节纹理-3")), true, A(Language(L"细节纹理-3")),true);
	m_layerImages.SetImageName(5, A(Language(L"高亮纹理-3")), true, A(Language(L"高亮纹理-3")),true);
	m_layerImages.SetImageName(6, A(Language(L"细节纹理-4")), true, A(Language(L"细节纹理-4")),true);
	m_layerImages.SetImageName(7, A(Language(L"高亮纹理-4")), true, A(Language(L"高亮纹理-4")),true);
	m_layerImages.SetImageName(8, A(Language(L"框架纹理-9")), true, A(Language(L"框架纹理-9")),true);
	//
	//m_trackCtrl.Create(m_trackCtrl.IDD, GetPropertiesDlgWindowPtr());
	//m_trackCtrl.ShowWindow(SW_HIDE);
	//m_trackCtrl.SetCallback(this);
	////
	//m_trackDlg.Create(m_trackDlg.IDD, GetPropertiesDlgWindowPtr());
	//m_trackDlg.ShowWindow(SW_HIDE);
	//m_trackDlg.m_baseView	=	this;
	////
	//for(int i=1;i<12;i++)
	//	m_trackDlg.AddEditor(L"", i, 0, 1, false);
	//
	this->InitProp(m_sceneProperties);
	//this->InitProp(m_skyProperties);
	//this->InitProp(m_flareProperties);
	this->InitProp(m_fogProperties);
	this->InitProp(m_cameraProperties);
	this->InitProp(m_lightProperties);
	this->InitProp(m_objectProperties);
	this->InitProp(m_unitProperties);
	this->InitProp(m_textureProperties);
	//this->InitProp(m_waterProperties);
	this->InitProp(m_editorProperties);
	//this->InitProp(m_grassProperties);
	this->InitProp(m_envProperties);
	this->InitProp(m_regionProperties);
	//
	//this->InitProp(m_animationProperties);
	//this->InitProp(m_scaProperties);
	//this->InitProp(m_scaTrackProperties);
	//this->InitProp(m_smallMapProperties);
	//this->InitProp(m_advancedProperties);
	//
	RebuildTrees();
	InitViewerProps();
	InitSceneProperties();
	InitSkyProperties();
	InitFogProperties();
	InitCameraProperties();
	InitObjectProperties();
	InitUnitProperties();
	InitTextureProperties();
	InitLightProperties();
	InitEditorProperties();
	InitWaterProperties();
	InitGrassProperties();
	InitAnimationProperties();
	InitScaProperties();
	InitScaTrackProperties();
	InitSmallMapProperties();
	InitEnvProperties();
	return true;
}

INT				GCSceneEditor3D::GetKeyFrameCount()
{
	//TreeItemType* type	=	this->GetSelectedItem();
	//if(!type)
	//	return 0;
	//switch(type->type)
	//{
	//case SCENE3D_ANIMATION:
	//case SCENE3D_SCA:
	//case SCENE3D_SCA_TRACK:
	//	{
	//		ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
	//		if(ani)
	//			return (int)(ani->GetTrackTimeLength() * (float)TRACK_SECOND_LENGTH);
	//	}
	//	break;
	//}
	return 0;
}
BOOL			GCSceneEditor3D::OnTrackChange(INT id, INT nBeginFrame, INT nEndFrame)	//如果返回TRUE表示可以被改变
{
	TreeItemType* type	=	this->GetSelectedItem();
	if(!type)
		return false;
	switch(type->type)
	{
	case SCENE3D_ANIMATION:
	case SCENE3D_SCA:
	case SCENE3D_SCA_TRACK:
		{
			//ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
			//for(int i=0;i<ani->GetScaManager().GetDataCount();i++)
			//{
			//	ISCA_Base* base	=	ani->GetScaManager().GetDataPtr(i);
			//	if(base)
			//	{
			//		ISCA_Track* t	=	base->GetTrackManager().SearchDataPtr(id);
			//		if(t)
			//		{
			//			t->SetBeginTrackIndex(nBeginFrame);
			//			t->SetEndTrackIndex(nEndFrame);
			//			this->SetModifyed();
			//			return true;
			//		}
			//	}
			//}
		}
		break;
	}
	return false;
}

VOID			GCSceneEditor3D::EnumTracks(std::vector<TrackItem>& rets)
{
	//TreeItemType* type	=	this->GetSelectedItem();
	//if(!type)
	//	return;
	//switch(type->type)
	//{
	//case SCENE3D_ANIMATION:
	//case SCENE3D_SCA:
	//case SCENE3D_SCA_TRACK:
	//	{
	//		ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
	//		for(int i=0;i<ani->GetScaManager().GetDataCount();i++)
	//		{
	//			ISCA_Base* base	=	ani->GetScaManager().GetDataPtr(i);
	//			if(base)
	//			{
	//				for(int j=0;j<base->GetTrackManager().GetDataCount();j++)
	//				{
	//					ISCA_Track* t	=	base->GetTrackManager().GetDataPtr(j);
	//					if(t)
	//					{
	//						TrackItem track;
	//						memset(&track, 0, sizeof(TrackItem));
	//						track.beginFrame	=	t->GetBeginTrackIndex();
	//						track.endFrame		=	t->GetEndTrackIndex();
	//						track.c				=	RGB(255, 0, 0);
	//						track.id			=	t->GetID();
	//						switch(type->type)
	//						{
	//						case SCENE3D_ANIMATION:
	//							if(type->p1 == ani->GetID())
	//								track.c	=	RGB(0, 255, 0);
	//							else
	//								continue;
	//							break;
	//						case SCENE3D_SCA:
	//							if(type->p2 == base->GetID())
	//								track.c	=	RGB(0, 255, 0);
	//							else
	//								continue;
	//							break;
	//						case SCENE3D_SCA_TRACK:
	//							if(type->p3 == t->GetID())
	//								track.c	=	RGB(0, 255, 0);
	//							else
	//								continue;
	//							break;
	//						}
	//						track.name			=	W(t->GetName());
	//						rets.push_back(track);
	//					}
	//				}
	//			}
	//		}
	//	}
	//	break;
	//}
}

VOID		GCSceneEditor3D::RebuildTrees()
{
	if(m_hViewItem)
		m_treeView.DeleteItem(m_hViewItem);
	wchar_t wFileName[256], wExt[256];
	_wsplitpath(m_strFile.c_str(), 0, 0, wFileName, wExt);
	wcscat(wFileName, wExt);
	if(!wFileName[0])
		GetNewFileName(wFileName);
	m_hViewItem = m_treeView.InsertItem(wFileName, 15, 15);
	m_treeView.SetItemState(m_hViewItem, TVIS_BOLD, TVIS_BOLD);
	m_treeView.Expand(m_hViewItem, TVE_EXPAND);
	//
	this->ClearTreeItem();
	BuildAll();
	//
	m_treeView.m_callback	=	this;
}

VOID		GCSceneEditor3D::BuildAll()
{
	m_treeView.SetItemData(m_hViewItem, AddTreeItem(SCENE3D_MAIN));
	m_treeView.SelectItem(m_hViewItem);
	DeleteChildItems(m_hViewItem);
	int nShadow		=	SCENE3D_EFFECT_PROFESSIONAL;
	int nSmallMap	=	SCENE3D_EFFECT_PROFESSIONAL;
	int nAdvanced	=	SCENE3D_EFFECT_PROFESSIONAL;
	nShadow		=	SCENE3D_EFFECT_SHADOW_LIST;
	nSmallMap	=	SCENE3D_EFFECT_SMALLMAP;
	nAdvanced	=	SCENE3D_EFFECT_ADVANCED_LIST;
	//
	{
		HTREEITEM hAffect	=	m_treeView.InsertItem(Language(L"效果设置"), 36, 36, m_hViewItem);
		m_treeView.SetItemState(hAffect, TVIS_BOLD, TVIS_BOLD);
		m_treeView.SetItemData(hAffect, AddTreeItem(SCENE3D_EFFECT));
		//
		m_hShadowItem	=	m_treeView.InsertItem(Language(L"阴影列表"), 37, 37, hAffect);
		m_treeView.SetItemState(m_hShadowItem, TVIS_BOLD, TVIS_BOLD);
		m_treeView.SetItemData(m_hShadowItem, AddTreeItem(nShadow));
		RefreshShadows();
		//
		m_hAdvancedItem	=	m_treeView.InsertItem(Language(L"全屏幕影响器"), 39, 39, hAffect);
		m_treeView.SetItemState(m_hAdvancedItem, TVIS_BOLD, TVIS_BOLD);
		m_treeView.SetItemData(m_hAdvancedItem, AddTreeItem(nAdvanced));
		RefreshAdvanced();
		//
		HTREEITEM hSmallMap	=	m_treeView.InsertItem(Language(L"小地图设置"), 38, 38, hAffect);
		m_treeView.SetItemState(hSmallMap, TVIS_BOLD, TVIS_BOLD);
		m_treeView.SetItemData(hSmallMap, AddTreeItem(nSmallMap));
	}
	//
	//HTREEITEM hSkyList	=	m_treeView.InsertItem(Language(L"天空体列表"), 3, 3, m_hViewItem);
	//m_treeView.SetItemState(hSkyList, TVIS_BOLD, TVIS_BOLD);
	//m_treeView.SetItemData(hSkyList, AddTreeItem(SCENE3D_SKY_LIST));
	//RefreshSkys(hSkyList);
	//
	//HTREEITEM hFog	=	m_treeView.InsertItem(Language(L"雾列表"), 7, 7, m_hViewItem);
	//m_treeView.SetItemState(hFog, TVIS_BOLD, TVIS_BOLD);
	//m_treeView.SetItemData(hFog, AddTreeItem(SCENE3D_FOG_LIST));
	//RefreshFogs(hFog);
	//
	wchar_t wbuf[128];
	swprintf(wbuf, L"%s[%s]", Language(L"环境列表"), Wutf8(m_scene3D->m_defaultEnv.m_name.t).buf);
	m_hEnv	=	m_treeView.InsertItem(wbuf, 41, 41, m_hViewItem);
	m_treeView.SetItemState(m_hEnv, TVIS_BOLD, TVIS_BOLD);
	m_treeView.SetItemData(m_hEnv, AddTreeItem(SCENE3D_ENV_LIST));
	RefreshEnvs();
	swprintf(wbuf, L"%s[%s]", Language(L"摄像机列表"), Wutf8(m_currentCamera.name.t).buf);
	m_hCameraList	=	m_treeView.InsertItem(wbuf, 14, 14, m_hViewItem);
	m_treeView.SetItemState(m_hCameraList, TVIS_BOLD, TVIS_BOLD);
	m_treeView.SetItemData(m_hCameraList, AddTreeItem(SCENE3D_CAMERA_LIST));
	RefreshCameras();
	m_hRegionList	=	m_treeView.InsertItem(Language(L"游戏地区列表"), 38, 38, m_hViewItem);
	m_treeView.SetItemState(m_hRegionList, TVIS_BOLD, TVIS_BOLD);
	m_treeView.SetItemData(m_hRegionList, AddTreeItem(SCENE3D_REGION_LIST));
	RefreshRegions(0);
	//
	//
	m_hLightList	=	m_treeView.InsertItem(Language(L"灯光列表"), 4, 4, m_hViewItem);
	m_treeView.SetItemState(m_hLightList, TVIS_BOLD, TVIS_BOLD);
	m_treeView.SetItemData(m_hLightList, AddTreeItem(SCENE3D_LIGHT_LIST));
	RefreshLights();
	//
	HTREEITEM hWaterList	=	m_treeView.InsertItem(Language(L"水面列表"), 10, 10, m_hViewItem);
	m_treeView.SetItemState(hWaterList, TVIS_BOLD, TVIS_BOLD);
	m_treeView.SetItemData(hWaterList, AddTreeItem(SCENE3D_WATER_LIST));
	RefreshWaters(hWaterList);
	//
	//m_hGrassList	=	m_treeView.InsertItem(Language(L"草地列表"), 23, 23, m_hViewItem);
	//m_treeView.SetItemState(m_hGrassList, TVIS_BOLD, TVIS_BOLD);
	//m_treeView.SetItemData(m_hGrassList, AddTreeItem(SCENE3D_GRASS_LIST));
	//RefreshGrasss(m_hGrassList);
	//
	//HTREEITEM hTextureList	=	m_treeView.InsertItem(Language(L"地形材质列表"), 12, 12, m_hViewItem);
	//m_treeView.SetItemState(hTextureList, TVIS_BOLD, TVIS_BOLD);
	//m_treeView.SetItemData(hTextureList, AddTreeItem(SCENE3D_TEXTURE_LIST));
	//RefreshTextures(hTextureList);
	//
	m_hObjectList	=	m_treeView.InsertItem(Language(L"文件列表"), 11, 11, m_hViewItem);
	m_treeView.SetItemState(m_hObjectList, TVIS_BOLD, TVIS_BOLD);
	m_treeView.SetItemData(m_hObjectList, AddTreeItem(SCENE3D_OBJECT_LIST));
	RefreshObjects();
	//
	m_hEventList	=	m_treeView.InsertItem(Language(L"移动点列表"), 2, 2, m_hViewItem);
	m_treeView.SetItemState(m_hEventList, TVIS_BOLD, TVIS_BOLD);
	m_treeView.SetItemData(m_hEventList, AddTreeItem(SCENE3D_POINT_LIST));
	RebuildPoints();
	//
	//m_hUnits	=	m_treeView.InsertItem(Language(L"对象列表"), 2, 2, m_hViewItem);
	//m_treeView.SetItemState(m_hUnits, TVIS_BOLD, TVIS_BOLD);
	//m_treeView.SetItemData(m_hUnits, AddTreeItem(SCENE3D_UNIT_LIST));
	//this->RebuildUnits();
	//
	//HTREEITEM hAnimationList	=	m_treeView.InsertItem(Language(L"动画列表"), 8, 8, m_hViewItem);
	//m_treeView.SetItemState(hAnimationList, TVIS_BOLD, TVIS_BOLD);
	//m_treeView.SetItemData(hAnimationList, AddTreeItem(SCENE3D_ANIMATION_LIST));
	//RefreshAnimationList(hAnimationList);
	//
	//HTREEITEM hUnitList	=	m_treeView.InsertItem(Language(L"单位列表"), 2, 2, m_hViewItem);
	//m_treeView.SetItemState(hUnitList, TVIS_BOLD, TVIS_BOLD);
	//m_treeView.SetItemData(hUnitList, AddTreeItem(SCENE3D_UNIT_LIST));
	//RefreshUnits(hUnitList);
}

int			GetSceneObjectIcon(INT type)
{
	switch(type)
	{
	case 0:
		return 11;
	case 1:
		return 1;
	}
	return 2;
}

VOID				GCSceneEditor3D::AddUnitProp(Phantom::SceneObject* r)
{
	//HTREEITEM hItem	=	m_treeView.GetChildItem(m_hUnits);
	//while(hItem)
	//{
	//	TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
	//	if(type && type->id == r->GetID())
	//	{
	//		this->m_treeView.AddSelect(hItem);
	//	}
	//	hItem	=	m_treeView.GetNextSiblingItem(hItem);
	//}
}
VOID				GCSceneEditor3D::RebuildUnits()
{
	//m_treeView.SelectItem(m_hUnits);
	//DeleteChildItems(m_hUnits);
	//for(int i=0;i<m_scene3D->GetObjectCount();i++)//GetRuntimeObjectCount();i++)
	//{
	//	Phantom::SceneObject* r	=	m_scene3D->GetObjectPtr(i);
	//	if(r->GetMeshPtr()&&r->GetMeshPtr()->GetDataPtr()->m_meshType!=Phantom::MeshData::MeshType_StaticMesh){
	//		HTREEITEM hData		=	m_treeView.InsertItem(Wutf8(r->GetName()), GetSceneObjectIcon(0), GetSceneObjectIcon(0), m_hUnits);
	//		m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_UNIT, r->GetID()));
	//	}
	//}
}
VOID				GCSceneEditor3D::RebuildPoints()
{
	m_treeView.SelectItem(m_hEventList);
	DeleteChildItems(m_hEventList);
	for(int i=0;i<m_scene3D->GetActiveEnv()->m_points.size();i++)//GetRuntimeObjectCount();i++)
	{
		Phantom::ScenePoint* r	=	m_scene3D->GetActiveEnv()->m_points[i];
		HTREEITEM hEvent		=	m_treeView.InsertItem(Wutf8(r->m_name.t), GetSceneObjectIcon(0), GetSceneObjectIcon(0), m_hEventList);
		m_treeView.SetItemData(hEvent, AddTreeItem(SCENE3D_POINT, r->GetID()));
	}}
VOID				GCSceneEditor3D::RefreshUnits()
{

}
VOID				GCSceneEditor3D::SaveRegionD(SaveRegionV& v, Phantom::SceneBox* r)
{
	for(int x=0;x<REGION_VERTEX_COUNT_25d;x++){
		for(int y=0;y<REGION_VERTEX_COUNT_25d;y++){
			v.data[x+y*REGION_VERTEX_COUNT_25d] = *r->GetVertexAttributePtr(x, y);
		}
	}
}
VOID				GCSceneEditor3D::SaveRegionTs(SaveRegionT& v, Phantom::SceneBox* r)
{
	for(int x=0;x<REGION_VERTEX_COUNT_25d;x++){
		for(int y=0;y<REGION_VERTEX_COUNT_25d;y++){
			v.data[x+y*REGION_VERTEX_COUNT_25d] = r->m_attrs[x+y*REGION_VERTEX_COUNT_25d];
		}
	}
}
VOID				GCSceneEditor3D::RemoveUnits(INT nID)
{
	//HTREEITEM hItem	=	m_treeView.GetChildItem(m_hUnits);
	//while(hItem)
	//{
	//	TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
	//	if(type && type->id == nID)
	//	{
	//		m_treeView.DeleteItem(hItem);
	//		return;
	//	}
	//	hItem	=	m_treeView.GetNextSiblingItem(hItem);
	//}
}

VOID		GCSceneEditor3D::RefreshGrasss(HTREEITEM hItem)
{
	//m_treeView.SelectItem(hItem);
	//DeleteChildItems(hItem);
	//for(int i=0;i<m_scene3D->GetGrassDataPtr()->GetDataCount();i++)
	//{
	//	ISceneGrassTexture*	data	=	m_scene3D->GetGrassDataPtr()->GetDataPtr(i);
	//	HTREEITEM hData		=	m_treeView.InsertItem(W(data->GetName()), 23, 23, hItem);
	//	m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_GRASS, data->GetID()));
	//}
}

VOID		GCSceneEditor3D::RefreshShadows()
{
	//RequestAuth();
	//DefineAuth_3();
	//if(!CheckAuth_3(0))
	//	return;
	//m_treeView.SelectItem(m_hShadowItem);
	//DeleteChildItems(m_hShadowItem);
	//for(int i=0;i<m_scene3D->GetShadowDataPtr()->GetDataCount();i++)
	//{
	//	SceneShadowData*	data	=	m_scene3D->GetShadowDataPtr()->GetDataPtr(i);
	//	HTREEITEM hData		=	m_treeView.InsertItem(W(data->name.t), 37, 37, m_hShadowItem);
	//	m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_EFFECT_SHADOW, data->id));
	//}

}

VOID		GCSceneEditor3D::RefreshAdvanced()
{
	//RequestAuth();
	//DefineAuth_3();
	//if(!CheckAuth_3(0))
	//	return;
	//m_treeView.SelectItem(m_hAdvancedItem);
	//DeleteChildItems(m_hAdvancedItem);
	//for(int i=0;i<m_scene3D->GetAdvancedEffects().GetDataCount();i++)
	//{
	//	IAdvancedEffect*	data	=	m_scene3D->GetAdvancedEffects().GetDataPtr(i);
	//	HTREEITEM hData		=	m_treeView.InsertItem(W(data->GetName()), 39, 39, m_hAdvancedItem);
	//	m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_EFFECT_ADVANCED, data->GetID()));
	//}
}

VOID		GCSceneEditor3D::RefreshSkys(HTREEITEM hItem)
{
	m_treeView.SelectItem(hItem);
	DeleteChildItems(hItem);
	for(int i=0;i<m_scene3D->m_skyDataPtr.GetDataCount();i++)
	{
		Phantom::SceneSkyData*	data	=	m_scene3D->m_skyDataPtr.GetDataPtr(i);
		HTREEITEM hData		=	m_treeView.InsertItem(Wutf8(data->name.t), 3, 3, hItem);
		m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_SKY, data->id));
		RefreshFlares(data, hData);
	}
}
VOID		GCSceneEditor3D::RefreshFlares(Phantom::SceneSkyData* sky, HTREEITEM hItem)
{
	m_treeView.SelectItem(hItem);
	DeleteChildItems(hItem);
	for(int i=0;i<sky->flareCount;i++)
	{
		wchar_t buf[128];
		swprintf(buf, L"flare%d", i + 1);
		HTREEITEM hData		=	m_treeView.InsertItem(buf, 3, 3, hItem);
		m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_FLARE, sky->id, i));
	}
}
VOID		GCSceneEditor3D::RefreshTextures(HTREEITEM hItem)
{
	//for(int i=0;i<this->m_scene3D->GetTextureCount();i++)
	{
		//this->m_scene3D->GetTextureRuntime(i)->
	}
}

struct	ObjectFileNameSort
{
	const char*		szText;
	char			szExt[32];
	int				id;
	int				iconIndex;
};
int			OnSortObjectFile(const void* v1, const void* v2)
{
	ObjectFileNameSort* f1	=	(ObjectFileNameSort*)v1;
	ObjectFileNameSort* f2	=	(ObjectFileNameSort*)v2;
	return stricmp(f1->szExt, f2->szExt);
}

int			GetIconFromExt(const char* szExt)
{
	if(stricmp(szExt, ".rpgmesh") == 0 || stricmp(szExt, ".geoms") == 0 || stricmp(szExt, ".pakmesh") == 0)
		return 1;
	else if(stricmp(szExt, ".mp3") == 0 || stricmp(szExt, ".ogg") == 0 || stricmp(szExt, ".wav") == 0 || stricmp(szExt, ".wmv") == 0 || stricmp(szExt, ".mid") == 0)
		return 18;
	if(stricmp(szExt, ".rpgunit") == 0 || stricmp(szExt, ".z_rpg3D") == 0)
		return 11;
	return -1;
}

VOID		GCSceneEditor3D::RebuildSubDir(HTREEITEM hItem, const wchar_t* szDir, const wchar_t* szPath)
{
	wchar_t szNewPath[512];
	wcscpy(szNewPath, szPath);
	wcscat(szNewPath, szDir);
	wchar_t	ch	=	szNewPath[wcslen(szNewPath) - 1];
	if(ch != L'\\' && ch != L'/')
		wcscat(szNewPath, L"\\");
	wcscat(szNewPath, L"*.*");
	WIN32_FIND_DATA win32;
	memset(&win32, 0, sizeof(win32));
	HANDLE hFile	=	::FindFirstFile(szNewPath, &win32);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		while(true)
		{
			if(win32.cFileName[0] != L'.' && !(win32.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				if((win32.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
				{
					wcscpy(szNewPath, szDir);
					wcscat(szNewPath, win32.cFileName);
					wcscat(szNewPath, L"\\");
					HTREEITEM hItemChild	=	m_treeView.InsertItem(win32.cFileName, 20, 20, hItem);
					RebuildSubDir(hItemChild, szNewPath, szPath);
				}
				else
				{
					wcscpy(szNewPath, szDir);
					wcscat(szNewPath, win32.cFileName);
					wchar_t szExt[128];
					_wsplitpath(szNewPath, 0, 0, 0, szExt);
					//
					if(GetIconFromExt(A(szExt).buf) >= 0)//wcsicmp(szExt, L".rpgmesh") == 0 || wcsicmp(szExt, L".rpgunit") == 0 || wcsicmp(szExt, L".z_rpg3D") == 0 || wcsicmp(szExt, L".geoms") == 0 || wcsicmp(szExt, L".z_rpgMesh") == 0)
					{
						Phantom::FileName fname	=	A(szNewPath).buf;
						fname.setIsFile();
						//
						Phantom::SceneObjectFile* obj	=	m_scene3D->m_objectFilePtr.SearchDataByName(fname.t);
						if(!obj)
						{
							obj	=	m_scene3D->m_objectFilePtr.AddData(0);
							obj->name	=	fname.t;
							this->SetModifyed();
						}
						int iconIndex		=	GetIconFromExt(A(szExt));
						HTREEITEM hData		=	m_treeView.InsertItem(win32.cFileName, iconIndex, iconIndex, hItem);
						m_treeView.SetItemData(hData, (DWORD_PTR)AddTreeItem(SCENE3D_OBJECT, obj->id));
					}
				}
			}
			if(!::FindNextFile(hFile, &win32))
				break;
		}
	}
	FindClose(hFile);
	//
}

VOID		AddChildFile(CTreeCtrl& tree, HTREEITEM hItem, const wchar_t* szName, int iconIndex, int data)
{
	wchar_t first[512];
	first[0] = 0;
	int index	=	0;
	while(true)
	{
		if(*szName == 0)
		{
			first[index++] = 0;
			//
			HTREEITEM hData		=	tree.InsertItem(first, iconIndex, iconIndex, hItem);//files[i].iconIndex, files[i].iconIndex, hItem);
			tree.SetItemData(hData, data);
			//
			if(*szName == 0)
				return;
		}
		else if(*szName == L'\\' || *szName == L'/')
		{
			first[index++] = 0;
			HTREEITEM hChild	=	tree.GetChildItem(hItem);
			HTREEITEM hFind		=	0;
			while(hChild)
			{
				CString str	=	tree.GetItemText(hChild);
				if(wcsicmp(str, first) == 0)
				{
					hFind		=	hChild;
					break;
				}
				hChild			=	tree.GetNextSiblingItem(hChild);
			}
			if(!hFind)
				hFind	=	tree.InsertItem(first, 20, 20, hItem);//files[i].iconIndex, files[i].iconIndex, hItem);
			hItem	=	hFind;
			index		=	0;
			first[0]	=	0;
			szName++;
		}
		else
		{
			first[index++]	=	*szName;
			szName++;
		}
	}
}

VOID		GCSceneEditor3D::RefreshObjects()
{
	m_treeView.SelectItem(m_hObjectList);
	DeleteChildItems(m_hObjectList);
	//
	RebuildSubDir(m_hObjectList, L"", W(GetProjectPath()));
	//std::vector<ObjectFileNameSort>	files;
	////
	//for(int i=0;i<m_scene3D->m_objectFilePtr.GetDataCount();i++)
	//{
	//	Phantom::SceneObjectFile* file	=	m_scene3D->m_objectFilePtr.GetDataPtr(i);
	//	ObjectFileNameSort fs;
	//	fs.id		=	file->id;
	//	fs.szText	=	file->name.t;
	//	_splitpath(fs.szText, 0, 0, 0, fs.szExt);
	//	fs.iconIndex	=	GetIconFromExt(fs.szExt);
	//	files.push_back(fs);
	//}
	//if(files.size() > 1)
	//	qsort(&files.at(0), files.size(), sizeof(ObjectFileNameSort), OnSortObjectFile);
	//for(int i=0;i<files.size();i++)
	//{
	//	wchar_t wbuf[256];
	//	wcscpy(wbuf, W(files[i].szText));
	//	int len	=	wcslen(wbuf);
	//	if(len > 32)
	//	{
	//		wchar_t szFile[128], szExt[128];
	//		_wsplitpath(wbuf, 0, 0, szFile, szExt);
	//		wcscat(szFile, szExt);
	//		for(int j=0;j<len;j++)
	//		{
	//			if(wbuf[j] == L'\\')
	//			{
	//				wbuf[j + 1] = L'.';
	//				wbuf[j + 2] = L'.';
	//				wbuf[j + 3] = L'.';
	//				wbuf[j + 4] = L'\\';
	//				wbuf[j + 5] = 0;
	//				wcscat(wbuf, szFile);
	//				break;
	//			}
	//		}
	//	}
	//	AddChildFile(m_treeView, m_hObjectList, wbuf, files[i].iconIndex, AddTreeItem(SCENE3D_OBJECT, files[i].id));
	//	//HTREEITEM hData		=	m_treeView.InsertItem(wbuf, files[i].iconIndex, files[i].iconIndex, hItem);
	//	//m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_OBJECT, files[i].id));
	//}
}

VOID		GCSceneEditor3D::RefreshAnimationList(HTREEITEM hItem)
{
	//m_treeView.SelectItem(hItem);
	//DeleteChildItems(hItem);
	//for(int i=0;i<m_scene3D->GetAnimationManager().GetDataCount();i++)
	//{
	//	ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().GetDataPtr(i);
	//	HTREEITEM hAnimation		=	m_treeView.InsertItem(W(ani->GetName()), 9, 9, hItem);
	//	m_treeView.SetItemData(hAnimation, AddTreeItem(SCENE3D_ANIMATION, ani->GetID()));
	//	RefreshAnimation(ani, hAnimation);
	//}
}

VOID		GCSceneEditor3D::RefreshUnits(HTREEITEM hItem)
{
}
VOID		GCSceneEditor3D::RefreshEnvs()
{
	m_treeView.SelectItem(m_hEnv);
	DeleteChildItems(m_hEnv);
	HTREEITEM hData		=	m_treeView.InsertItem(Language(L"游戏环境"), 40, 40, m_hEnv);
	m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_ENV, 0));
	for(int i=0;i<m_scene3D->m_sceneEnvs.size();i++)
	{
		Phantom::SceneEnv* env = m_scene3D->m_sceneEnvs[i];
		HTREEITEM hData		=	m_treeView.InsertItem(Wutf8(env->m_name.t), 40, 40, m_hEnv);
		m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_ENV, env->GetID()));
	}
}
VOID		GCSceneEditor3D::RefreshFogs(HTREEITEM hItem)
{
	//m_treeView.SelectItem(hItem);
	//DeleteChildItems(hItem);
	//for(int i=0;i<m_scene3D->m_fogDataPtr.GetDataCount();i++)
	//{
	//	Phantom::SceneFogData*	data	=	m_scene3D->m_fogDataPtr.GetDataPtr(i);
	//	HTREEITEM hData		=	m_treeView.InsertItem(Wutf8(data->name.t), 7, 7, hItem);
	//	m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_FOG, data->id));
	//}
}
VOID		GCSceneEditor3D::OnUndoRedo(UndoRedoBuffer* buffer, int ur_mode)
{
	UndoPtr uptr;
	uptr.buff = buffer;
	uptr.urMode = ur_mode;
	switch(buffer->type){
	case SCENE3D_ENV:
		{
			ProSceneEnv proc = (ProSceneEnv)buffer->proc;
			Phantom::SceneEnv* env = 0;
			if(buffer->m_id==0)
				env = &m_scene3D->m_defaultEnv;
			else
				env = m_scene3D->GetEnvByID(buffer->m_id);
			if(proc&&env)
				(*proc)(m_scene3D, env, 0, 0, &uptr);
		}
		break;
	case SCENE3D_LIGHT:
		{
			ProSceneLight proc = (ProSceneLight)buffer->proc;
			Phantom::SceneLight* light = 0;
			if(buffer->m_id==0)
				light = &m_scene3D->GetActiveEnv()->m_defaultLight;
			else
			{
				Phantom::GameID id;
				id.id = buffer->m_id;
				light = m_scene3D->GetActiveEnv()->GetLightPtr(id);
			}
			if(proc&&light)
				(*proc)(m_scene3D, light, 0, 0, &uptr);
		}
		break;
	case SCENE3D_REGION:
		{
			ProSceneRegion proc = (ProSceneRegion)buffer->proc;
			Phantom::SceneRegionData* data = m_scene3D->GetRegionPtr(buffer->m_id);
			if(proc&&data)
				(*proc)(m_scene3D, data, 0, 0, &uptr);
		}
		break;
	case SCENE3D_CAMERA:
		{
			ProSceneCamera proc = (ProSceneCamera)buffer->proc;
			Phantom::PtrID id;
			id.id	=	buffer->m_id;
			Phantom::SceneCameraData* camera	=	m_scene3D->GetCameraDataPtr()->GetDataPtr(id);
			if(proc&&camera)
				(*proc)(m_scene3D, camera, 0, 0, &uptr);
		}
		break;
	case SCENE3D_UNIT:
		{
			ProSceneUnit proc = (ProSceneUnit)buffer->proc;
			Phantom::SceneEnv* env = m_scene3D->GetEnvByID(buffer->m_id3);
			SelectEditor se;
			if(buffer->m_id==1)
				se.Init(m_scene3D->GetObjectByID(buffer->m_id2));
			else if(buffer->m_id==2)
				se.Init(env->GetPointByID(buffer->m_id2));
			else if(buffer->m_id==3)
				se.Init(env->GetLightPtr(Phantom::GameID(-1, buffer->m_id2)), buffer->m_id4);
			if(proc)
				(*proc)(m_scene3D, se, 0, 0, &uptr);
		}
		break;
	case SCENE3D_UNIT_LIST:
		{
			Phantom::SceneEnv* env = m_scene3D->GetEnvByID(buffer->m_id3);
			SelectEditor se;
			if(buffer->m_id==1)
				se.Init(m_scene3D->GetObjectByID(buffer->m_id2));
			else if(buffer->m_id==2)
				se.Init(env->GetPointByID(buffer->m_id2));
			else if(buffer->m_id==3)
				se.Init(env->GetLightPtr(Phantom::GameID(-1, buffer->m_id2)), buffer->m_id4);
			SSData vold,vnew;
			(*buffer) >> vold >> vnew;
			if(ur_mode==UndoRedoBuffer::URMode_Undo){
				se.SetPosition(vold.m_downPosition);
				se.SetRotation(vold.m_downRotation);
				se.SetScale(vold.m_downScale);
			}
			else{
				se.SetPosition(vnew.m_downPosition);
				se.SetRotation(vnew.m_downRotation);
				se.SetScale(vnew.m_downScale);
			}
		}
		break;
	case SCENE3D_HEIGHT_SET:
		{
			Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion(buffer->m_id, buffer->m_id2);
			if(pRegion){
				SaveRegionV* vold = new SaveRegionV();
				SaveRegionV* vnew = new SaveRegionV();
				(*buffer) >> (*vold) >> (*vnew);
				for(int x=0;x<REGION_VERTEX_COUNT_25d;x++){
					for(int y=0;y<REGION_VERTEX_COUNT_25d;y++){
						if(ur_mode==UndoRedoBuffer::URMode_Undo){
							pRegion->SetVertexAttributeValue(x,y,&vold->data[x+y*REGION_VERTEX_COUNT_25d]);
						}
						else if(ur_mode==UndoRedoBuffer::URMode_Redo){
							pRegion->SetVertexAttributeValue(x,y,&vnew->data[x+y*REGION_VERTEX_COUNT_25d]);
						}
					}
				}
				delete vold;
				delete vnew;
				m_scene3D->RebuildRegionFaces(buffer->m_id, buffer->m_id2, true);
			}
		}
		break;
	case SCENE3D_TEXTURE_SET:
		{
			Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion(buffer->m_id, buffer->m_id2);
			if(pRegion){
				SaveRegionT* vold = new SaveRegionT();
				SaveRegionT* vnew = new SaveRegionT();
				(*buffer) >> (*vold) >> (*vnew);
				for(int x=0;x<REGION_VERTEX_COUNT_25d;x++){
					for(int y=0;y<REGION_VERTEX_COUNT_25d;y++){
						if(ur_mode==UndoRedoBuffer::URMode_Undo){
							pRegion->m_attrs[x+y*REGION_VERTEX_COUNT_25d] = vold->data[x+y*REGION_VERTEX_COUNT_25d];
						}
						else if(ur_mode==UndoRedoBuffer::URMode_Redo){
							pRegion->m_attrs[x+y*REGION_VERTEX_COUNT_25d] = vnew->data[x+y*REGION_VERTEX_COUNT_25d];
						}
					}
				}
				delete vold;
				delete vnew;
				m_scene3D->RebuildRegionFaces(buffer->m_id, buffer->m_id2, true);
			}
		}
		break;
	case SCENE3D_SELECT_LAYER:
		{
			FileName vold, vnew;
			(*buffer) >> vold >> vnew;
			if(ur_mode==UndoRedoBuffer::URMode_Undo)
				m_scene3D->SetLayerTexture(buffer->m_id, vold);
			else
				m_scene3D->SetLayerTexture(buffer->m_id, vnew);
			RefreshLayerImages();
		}
		break;
	}
	this->SetModifyed();
}
VOID		GCSceneEditor3D::RefreshRegions(HTREEITEM h)
{
	m_treeView.SelectItem(m_hRegionList);
	DeleteChildItems(m_hRegionList);
	for(int i=0;i<MAX_REGION_COUNT;i++)
	{
		HTREEITEM hData		=	m_treeView.InsertItem(Wutf8(m_scene3D->GetRegionName(i)), 7, 7, m_hRegionList);
		m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_REGION, i));
	}
}
VOID		GCSceneEditor3D::RefreshCameras()
{
	m_treeView.SelectItem(m_hCameraList);
	DeleteChildItems(m_hCameraList);
	for(int i=0;i<m_scene3D->GetCameraDataPtr()->GetDataCount();i++)
	{
		const Phantom::SceneCameraData*	data	=	m_scene3D->GetCameraDataPtr()->GetDataPtr(i);
		HTREEITEM hData		=	m_treeView.InsertItem(Wutf8(data->name.t), 14, 14, m_hCameraList);
		m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_CAMERA, data->id));
	}
}

VOID		GCSceneEditor3D::RefreshLights()
{
	m_treeView.SelectItem(m_hLightList);
	DeleteChildItems(m_hLightList);
	HTREEITEM hData		=	m_treeView.InsertItem(Language(L"全局光"), 4, 4, m_hLightList);
	m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_LIGHT, 0));
	Phantom::SceneEnv* env = m_scene3D->GetActiveEnv();
	for(int i=1;i<env->m_lights.size();i++)
	{
		Phantom::SceneLight*	data	=	env->m_lights[i];
		HTREEITEM hData		=	m_treeView.InsertItem(Wutf8(data->m_name.t), 4, 4, m_hLightList);
		m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_LIGHT, data->GetID()));
	}

}
VOID		GCSceneEditor3D::RefreshWaters(HTREEITEM hItem)
{
	//m_treeView.SelectItem(hItem);
	//DeleteChildItems(hItem);
	//for(int i=0;i<m_scene3D->GetWaterDataPtr()->GetDataCount();i++)
	//{
	//	const SceneWaterData*	data	=	m_scene3D->GetWaterDataPtr()->GetDataPtr(i);
	//	HTREEITEM hData		=	m_treeView.InsertItem(W(data->name.t), 10, 10, hItem);
	//	m_treeView.SetItemData(hData, AddTreeItem(SCENE3D_WATER, data->id));
	//}
}

VOID		GCSceneEditor3D::OnActive()		//被激活
{
	m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowScene, true);
	UsePhantom3D(true);
	__super::OnActive();
	Phantom::GetCameraPtr()->SetCameraConfig(m_currentCamera);
}

VOID		GCSceneEditor3D::OnDeActive()		//失去激活
{
	m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowScene, false);
	UsePhantom3D(false);
	Phantom::GetCameraPtr()->GetCameraConfig(m_currentCamera);
	__super::OnDeActive();
}

void		GCSceneEditor3D::OnPushRenderObjects()	//渲染
{
	//if(this->m_bShowCharacter && this->m_character)
	//	m_character->PushToRenderList();
}

BOOL		GCSceneEditor3D::SaveFile(BOOL bSaveAs)	//保存文件
{
	bool bNew	=	false;
	if(bSaveAs || m_strFile.size() == 0)
	{
		bNew	=	true;
		CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, formatw(L"%s|*.rpgscene||", Language(L"TypeScene", L"场景文件") ), GetMainFrame());
		wchar_t buffer[1024];
		dlg.m_pOFN->lpstrInitialDir	=	AToW(GetProjectPath(), buffer, 1024);
		if(dlg.DoModal() != IDOK)
			return false;
		wchar_t szPath[256], szFile[256], szDir[256];
		_wsplitpath(dlg.GetPathName(), szPath, szDir, szFile, 0);
		wcscat(szPath, szDir);
		wcscat(szPath, szFile);
		wcscat(szPath, L".rpgscene");
		m_strFile	=	szPath;
	}
	//Phantom::GetCameraPtr()->GetCameraConfig(m_currentCamera);
	if(m_scene3D)
	{
		m_scene3D->SaveScene(A(m_strFile.c_str()));//(A(m_strFile.c_str()));
	}
	//
	return __super::SaveFile(bSaveAs);
}

//渲染
void		GCSceneEditor3D::OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI)
{
	switch(nMenuID)
	{
	case ID_SCENE3D_INSERTSKY:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			pCmdUI->Enable(type&&type->type==SCENE3D_MAIN);
			pCmdUI->SetText(m_scene3D->IsEnabled(Phantom::Scene3D::Enable_MoveGridsData)?Language(L"删除寻路数据"):Language(L"创建寻路数据"));
		}
		break;
	case ID_SCENE3D_SHADOW:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			pCmdUI->Enable(type&&type->type==SCENE3D_MAIN);
			pCmdUI->SetText(m_scene3D->HasTerrainData()?Language(L"删除地形"):Language(L"创建地形"));
		}
		break;
	case ID_SCREEN_HDR:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_EFFECT_ADVANCED_LIST);
		}
		break;
	case ID_SCENE3D_INSERTFOG:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_LIGHT_LIST);
		}
		break;
	case ID_SCENE3D_INSERTCAMERA:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_CAMERA_LIST);
		}
		break;
	case ID_SCENE3D_INSERT_WATER:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_WATER_LIST);
		}
		break;
	case ID_SCENE3D_INSERTGRASS:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_GRASS_LIST);
		}
		break;
	case ID_SCENE3D_ENV:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_ENV_LIST);
		}
		break;
	case ID_ADD_MOVE_POINT:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_POINT_LIST);
		}
		break;
	case ID_SCENE3D_INSERTOBJECTS:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_OBJECT_LIST);
		}
		break;
	case ID_SCENE3D_REFRESHOBJs:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_UNIT_LIST);
		}
		break;
	case ID_SCENE3D_OPENFILE:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_OBJECT);
		}
		break;
	case ID_SCENE3D_INSERTANIMATION:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_ANIMATION_LIST);
		}
		break;
	case ID_SCENE3D_SCAUNIT3D:
	case ID_SCENE3D_SCAGEOMETRY:
	case ID_SCENE3D_SCACAMERA:
	case ID_SCENE3D_SCASKY:
	case ID_SCENE3D_SCAFOG:
	case ID_SCENE3D_SCAWATER:
	case ID_SCENE3D_SCAGRASS:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_ANIMATION);
		}
		break;
	case ID_SCENE3D_INSERTSCATRACK:
	case ID_SCENE3D_BINDSELECT:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == SCENE3D_SCA);
		}
		break;
	default:
		pCmdUI->Enable(false);
		break;
	}
}

void		GCSceneEditor3D::OnCommandUI(INT nMenuID)
{
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			switch(nMenuID)
			{
			case ID_SCENE3D_INSERTSKY:
				{
					if(!this->m_scene3D->IsEnabled(Phantom::Scene3D::Enable_MoveGridsData)){
						m_scene3D->CreateMoveGrids();
						m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowMoveGrids, true);
					}
					else{
						if(GetMainFrame()->MessageBox(Language(L"DelTerrain", L"删除寻路数据将会失去所有已经编辑的数据并且不能恢复，是否要删除？"), Language(L"提示"), MB_YESNO)==IDYES)
							m_scene3D->DeleteMoveGrids();
					}
					//if(type->type == SCENE3D_SKY_LIST)
					//{
					//	if(m_scene3D->m_skyDataPtr.GetDataCount() >= MAX_SCENE_SKY)
					//	{
					//		unsigned char text[] = {0x4f,0xd6,0xb2,0x37,0x88,0x68,0xaa,0xc6,0x9d,0x90,0x56,0xbf,0x36,0xf7,0x55,0x8b,0xca,0x33,0x29,0xf9,0x81,0xd8,0x2a,0x79,0xc7,0xf3,0x44,0x29,0xdb,0x09,0x97,0x25,0x7a,0xf8,0xe9,0x59,0xed,0x39,0xa6,0xc8,0xf3};
					//		unsigned char password[] = {0x94,0x2c,0x1d,0xfe,0x20,0x96,0x03,0x60,0x53,0x58,0x9c,0x1c,0x96,0x19,0xfe,0x32,0x6c,0x9b,0xe7,0x5a,0x5d,0x1c,0xfe,0x8c,0x63,0x38,0x8b,0xdf,0x75,0xf1,0xba,0x85};
					//		wchar_t ret[512];
					//		GetCryptText(ret, text, sizeof(text), password, sizeof(password));
					//		ShowMainMessage(ret);
					//		break;
					//	}
					//	Phantom::SceneSkyData* sky	=	this->m_scene3D->m_skyDataPtr.AddData(0);
					//	sky->bEnable	=	true;
					//	sky->file		=	"default_init_zip/sky.dds";
					//	sky->fAngleZ	=	ToDegress(15.0f);
					//	sky->fAngleX	=	ToDegress(15.0f);
					//	sky->skyColor	=	Phantom::color4(1,1,1,1);
					//	this->RefreshSkys(hItem);
					//	this->OnChangeTreeSelect(&m_treeView, NULL);
					//	this->SetModifyed();
					//}
				}
				break;
			case ID_SCENE3D_SHADOW:
				{
					if(!this->m_scene3D->HasTerrainData()){
						m_scene3D->CreateTerrain();
						m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowTerrain, true);
					}
					else{
						if(GetMainFrame()->MessageBox(Language(L"DelTerrain", L"删除地形将会失去所有已经编辑的数据并且不能恢复，是否要删除？"), Language(L"提示"), MB_YESNO)==IDYES)
							m_scene3D->DeleteTerrain();
					}
					//if(type->type == SCENE3D_EFFECT_SHADOW_LIST)
					//{
					//	DefineAuth_3();
					//	if(!CheckAuth_3(0))
					//	{
					//		unsigned char text[] = {0x4f,0xd6,0xb2,0x37,0x88,0x68,0xaa,0xc6,0x9d,0x90,0x56,0xbf,0x36,0xf7,0x55,0x8b,0xca,0x33,0x29,0xf9,0x81,0xd8,0x2a,0x79,0xc7,0xf3,0x44,0x29,0xdb,0x09,0x97,0x25,0x7a,0xf8,0xe9,0x59,0xed,0x39,0xa6,0xc8,0xf3};
					//		unsigned char password[] = {0x94,0x2c,0x1d,0xfe,0x20,0x96,0x03,0x60,0x53,0x58,0x9c,0x1c,0x96,0x19,0xfe,0x32,0x6c,0x9b,0xe7,0x5a,0x5d,0x1c,0xfe,0x8c,0x63,0x38,0x8b,0xdf,0x75,0xf1,0xba,0x85};
					//		wchar_t ret[512];
					//		GetCryptText(ret, text, sizeof(text), password, sizeof(password));
					//		ShowMainMessage(ret);
					//		break;
					//	}
					//	SceneShadowData* shadow	=	this->m_scene3D->GetShadowDataPtr()->AddData(0);
					//	shadow->bSingleMap	=	true;
					//	shadow->bEnable		=	false;
					//	shadow->fDensity	=	0.8f;
					//	shadow->fEpsilon	=	0.000008f;
					//	shadow->flags		=	SHADOW_RENDER_OBJECTS | SHADOW_RENDER_TERRAIN_A_SHADOW;
					//	shadow->fMinVisible	=	To3DValue(100.0f);
					//	shadow->textureSize	=	1024;
					//	shadow->fProportion[0]	=	0.1f;
					//	shadow->fProportion[1]	=	0.3f;
					//	shadow->fProportion[2]	=	0.8f;
					//	this->RefreshShadows();
					//	this->OnChangeTreeSelect(&m_treeView, NULL);
					//	this->SetModifyed();
					//}
				}
				break;
			case ID_SCREEN_HDR:
				{
					if(type->type == SCENE3D_EFFECT_ADVANCED_LIST)
					{
						//IAdvancedEffect* eff	=	m_scene3D->GetAdvancedEffects().AddData(0, AdvancedEffect_HDR);
						//this->RefreshAdvanced();
						//this->OnChangeTreeSelect(&m_treeView, NULL);
						//this->SetModifyed();
					}
				}
				break;
			case ID_SCENE3D_INSERTFOG:
				{
					if(type->type == SCENE3D_LIGHT_LIST)
					{
						Phantom::Camera* c		=	Phantom::GetCameraPtr();
						RECT rc;
						GetClientRect(GetDeviceWindow(), &rc);
						Phantom::Pixel pixel(rc.left + (rc.right - rc.left)/2, rc.top + (rc.bottom - rc.top) / 2);
						Phantom::float3 orig, dir;
						c->Get3DFrom2D(orig, dir, pixel);
						Phantom::PickPoint data;
						if(!m_scene3D->HasTerrainData()){
							orig.x -= dir.x*(orig.z/dir.z);
							orig.y -= dir.y*(orig.z/dir.z);
							orig.z = 0;
							data.pos = orig;
						}
						else if(m_scene3D->IntersectTerrain(orig, dir, data))
						{
						}
						else
							break;
						Phantom::SceneLight* light = m_scene3D->GetActiveEnv()->CreateLight();
						light->m_lightType = Phantom::SceneLight::LightType_Omni;
						light->m_bEnabled = true;
						light->m_pos = data.pos;
						light->m_pos.z += To3DValue(1.0f);
						sprintf(light->m_name.t, "light%d", light->GetID());
						this->RefreshLights();
						this->SetModifyed();
					}
				}
				break;
			case ID_SCENE3D_INSERTCAMERA:
				{
					if(type->type == SCENE3D_CAMERA_LIST)
					{
						Phantom::SceneCameraData* camera	=	this->m_scene3D->GetCameraDataPtr()->AddData(0);
						if(camera)
							Phantom::GetCameraPtr()->GetCameraConfig(*camera);
						//
						this->RefreshCameras();
						this->OnChangeTreeSelect(&m_treeView, NULL);
						this->SetModifyed();
					}
				}
				break;
			case ID_SCENE3D_INSERT_WATER:
				{
					//if(type->type == SCENE3D_WATER_LIST)
					//{
					//	if(m_scene3D->GetWaterDataPtr()->GetDataCount() >= MAX_SCENE_WATER)
					//	{
					//		unsigned char text[] = {0x3c,0xab,0x57,0x37,0x6d,0x69,0x19,0xbf,0xa3,0x80,0xc2,0xb1,0x64,0x23,0x3f,0x13,0x71,0xce,0x99,0xc8,0x6b,0x6f,0xa0,0xce,0x76,0xcd,0xec,0x24,0x6a,0xf7,0xcb,0x09,0x09,0x82,0x4e,0x25,0x3b,0x38,0x15,0xb1,0xcd};
					//		unsigned char password[] = {0xe7,0x51,0xf8,0xfe,0xc5,0x97,0xb0,0x19,0x6d,0x48,0x08,0x12,0xc4,0xcd,0x94,0xaa,0xd7,0x66,0x57,0x6b,0xb7,0xab,0x74,0x3b,0xd2,0x06,0x23,0xd2,0xc4,0x0f,0xe6,0xa9};
					//		wchar_t ret[512];
					//		GetCryptText(ret, text, sizeof(text), password, sizeof(password));
					//		ShowMainMessage(ret);
					//		break;
					//	}
					//	SceneWaterData* water	=	this->m_scene3D->GetWaterDataPtr()->AddData(0);
					//	water->SetWaterColor(0xffa6caf0);
					//	water->SetWaterFadeBias(3.0f);
					//	water->SetWaterHeight(To3DValue(-1.0f));
					//	water->SetWaterWaveX(0.3f);
					//	water->SetWaterWaveY(0.3f);
					//	water->SetWaterWaveX_1(0.5f);
					//	water->SetWaterWaveY_1(0.5f);
					//	water->SetWaterSpeed_1(10.f);
					//	water->SetWaterSpeed_2(-5.f);
					//	water->SetWaterRotation_1(60.0f);
					//	water->SetWaterRotation_2(90.0f);
					//	water->SetWaterFadeExponent(1.3f);
					//	water->SetWaterReflectBump(0.2f);
					//	water->SetWaterRefractBump(0.2f);
					//	water->SetWaterReflectWeight(1.0f);
					//	water->SetWaterRefractWeight(0.5f);
					//	water->m_bWaterVisible		=	true;
					//	//water->m_nWaterTextureID	=	m_scene3D->AddTexture("scene3d\\water_waves.dds");
					//	//
					//	this->RefreshWaters(hItem);
					//	this->OnChangeTreeSelect(&m_treeView, NULL);
					//	this->SetModifyed();
					//}
				}
				break;
			//case ID_SCENE3D_INSERTGRASS:
			//	{
			//		if(type->type == SCENE3D_GRASS_LIST)
			//		{
			//			if(m_scene3D->GetGrassDataPtr()->GetDataCount() >= MAX_SCENE_GRASS)
			//			{
			//				unsigned char text[] = {0x67,0x30,0x3c,0xd3,0xd8,0x2e,0xaf,0x92,0x55,0x3d,0x5d,0xc8,0xa3,0xea,0xd0,0xba,0x48,0xe1,0x79,0x0d,0x28,0x89,0xe6,0x06,0x20,0x4d,0x51,0x9b,0xe9,0x90,0x34,0xd6,0x52,0x60,0x56,0xb5,0xd5,0x78,0xa6};
			//				unsigned char password[] = {0xbc,0xca,0x93,0x1a,0x70,0xd0,0x06,0x34,0x9b,0xf5,0x97,0x6b,0x03,0x04,0x7b,0x03,0xee,0x49,0xb7,0xae,0xf4,0x4d,0x32,0xf3,0x84,0x86,0x9e,0x6d,0x47,0x68,0x19,0x76};
			//				wchar_t ret[512];
			//				GetCryptText(ret, text, sizeof(text), password, sizeof(password));
			//				ShowMainMessage(ret);
			//				break;
			//			}
			//			ISceneGrassTexture* grass	=	this->m_scene3D->GetGrassDataPtr()->AddData(0);
			//			grass->SetTextureFile("default_init_zip\\grass.dds");
			//			grass->SetTextureCenterY(120);
			//			this->RefreshGrasss(m_hGrassList);
			//			this->OnChangeTreeSelect(&m_treeView, NULL);
			//			this->SetModifyed();
			//		}
			//	}
			//	break;
			case ID_SCENE3D_ENV:
				{
					m_scene3D->CreateEnv();
					this->RefreshEnvs();
				}
				break;
			case ID_ADD_MOVE_POINT:
				{
					Phantom::Camera* c		=	Phantom::GetCameraPtr();
					RECT rc;
					GetClientRect(GetDeviceWindow(), &rc);
					Phantom::Pixel pixel(rc.left + (rc.right - rc.left)/2, rc.top + (rc.bottom - rc.top) / 2);
					Phantom::float3 orig, dir;
					c->Get3DFrom2D(orig, dir, pixel);
					Phantom::PickPoint data;
					if(!m_scene3D->HasTerrainData()){
						orig.x -= dir.x*(orig.z/dir.z);
						orig.y -= dir.y*(orig.z/dir.z);
						orig.z = 0;
						data.pos = orig;
					}
					else if(m_scene3D->IntersectTerrain(orig, dir, data))
					{
					}
					else
						return;
					SetModifyed();
					m_scene3D->GetActiveEnv()->InsertPoint(data.pos);
					m_dlgObjects.RefreshEvents();
					RebuildPoints();
					switch(this->m_editorMode)
					{
					case EditorMode_ObjectMove:
					case EditorMode_ObjectScale:
					case EditorMode_ObjectRotation:
						break;
					default:
						SetEditorMode(EditorMode_ObjectMove);
						break;
					}
				}
				break;
			case ID_SCENE3D_OPENFILE:
				{
					Phantom::SceneObjectFile* obj	=	m_scene3D->m_objectFilePtr.SearchDataPtr(type->p1);
					if(obj)
						AfxGetApp()->OpenDocumentFile(W(obj->name.t));
				}
				break;
			case ID_SCENE3D_REFRESHOBJs:
				this->RefreshUnits();
				break;
			case ID_SCENE3D_INSERTOBJECTS:
				{
					RefreshObjects();
					//CFileDialog dlg(TRUE, 0, 0, OFN_EXPLORER|OFN_ENABLESIZING|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT, GetScene3DObjectFilter(), CWnd::FromHandle(this->GetViewHandle()));
					//W init(GetProjectPath());
					//dlg.m_pOFN->lpstrInitialDir	=	init;
					//std::wstring temp;
					//temp.resize(1024000);
					//dlg.m_pOFN->lpstrFile	=	(wchar_t*)temp.c_str();
					//dlg.m_pOFN->lpstrFile[0]	=	0;
					//dlg.m_pOFN->nMaxFile	=	temp.size();
					//if(dlg.DoModal() != IDOK)
					//	return;
					//SetProjectPath();
					//POSITION pos	=	dlg.GetStartPosition();
					//while(pos)
					//{
					//	std::wstring file;
					//	this->GetSortFile(file, dlg.GetNextPathName(pos));
					//	FileName fname	=	A(file.c_str()).buf;
					//	fname.setIsFile();
					//	//
					//	Phantom::SceneObjectFile* obj	=	m_scene3D->m_objectFilePtr.SearchDataByName(fname.t);
					//	if(!obj)
					//	{
					//		obj	=	m_scene3D->m_objectFilePtr.AddData(0);
					//		obj->name	=	fname.t;
					//	}
					//}
					//this->RefreshObjects();
					//this->SetModifyed();
				}
				break;
			//case ID_SCENE3D_INSERTANIMATION:
			//	{
			//		if(type->type == SCENE3D_ANIMATION_LIST)
			//		{
			//			ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().AddData(0);
			//			ani->SetTrackTimeLength(10.0f);
			//			this->RefreshAnimationList(hItem);
			//			this->OnChangeTreeSelect(&m_treeView, NULL);
			//			this->SetModifyed();
			//		}
			//	}
			//	break;
			//case ID_SCENE3D_BINDSELECT:
			//	{
			//		if(type->type == SCENE3D_SCA_TRACK)
			//		{
			//			ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
			//			ISCA_Base* base		=	ani->GetScaManager().SearchDataPtr(type->p2);
			//			ISCA_Track* track	=	base ? base->GetTrackManager().SearchDataPtr(type->p3) : 0;
			//			if(track && m_selectObjects.size() > 0)//pSceneObjectPtr->iObjectPtr)
			//			{
			//				if(m_selectObjects.at(i).pSceneObjectPtr)
			//				{
			//					Phantom::PtrID id;
			//					id.id	=	m_selectObjects.at(i).pSceneObjectPtr->id;
			//					track->SetUnitID(id);
			//					this->OnChangeTreeSelect(&m_treeView, NULL);
			//					this->SetModifyed();
			//				}
			//			}
			//			else if(m_selectObjects.size() == 0)
			//			{
			//				GetMainFrame()->MessageBox(Language(L"请选择一个对象,模型或者3D对象"));
			//			}
			//		}
			//	}
			//	break;
			case ID_SCENE3D_SCAUNIT3D:
			case ID_SCENE3D_SCAGEOMETRY:
			case ID_SCENE3D_SCACAMERA:
			case ID_SCENE3D_SCASKY:
			case ID_SCENE3D_SCAFOG:
			case ID_SCENE3D_SCAWATER:
			case ID_SCENE3D_SCAGRASS:
				{
					//if(type->type == SCENE3D_ANIMATION)
					//{
					//	ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
					//	if(!ani)
					//		break;
					//	int scaType	=	0;
					//	switch(nMenuID)
					//	{
					//	case ID_SCENE3D_SCAUNIT3D:
					//		scaType	=	SCA_Type_Unit3D;
					//		break;
					//	case ID_SCENE3D_SCAGEOMETRY:
					//		scaType	=	SCA_Type_Geometry;
					//		break;
					//	case ID_SCENE3D_SCACAMERA:
					//		scaType	=	SCA_Type_Camera;
					//		break;
					//	case ID_SCENE3D_SCASKY:
					//		scaType	=	SCA_Type_Sky;
					//		break;
					//	case ID_SCENE3D_SCAFOG:
					//		scaType	=	SCA_Type_Fog;
					//		break;
					//	case ID_SCENE3D_SCAWATER:
					//		scaType	=	SCA_Type_Water;
					//		break;
					//	case ID_SCENE3D_SCAGRASS:
					//		scaType	=	SCA_Type_Grass;
					//		break;
					//	default:
					//		break;
					//	}
					//	if(scaType > 0)
					//	{
					//		ISCA_Base* base	=	ani->GetScaManager().AddData(0, scaType);
					//		this->RefreshAnimation(ani, hItem);
					//		this->OnChangeTreeSelect(&m_treeView, NULL);
					//		this->SetModifyed();
					//	}
					//}
				}
				break;
			//case ID_SCENE3D_INSERTSCATRACK:
			//	{
			//		if(type->type == SCENE3D_SCA)
			//		{
			//			ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
			//			if(!ani)
			//				break;
			//			ISCA_Base* base	=	ani->GetScaManager().SearchDataPtr(type->p2);
			//			if(!base)
			//				break;
			//			ISCA_Track* track	=	base->GetTrackManager().AddData(0, base->GetType());
			//			track->SetFrameTimeLength(10.0f);
			//			this->RefreshSCA(ani, base, hItem);
			//			this->OnChangeTreeSelect(&m_treeView, NULL);
			//			this->SetModifyed();
			//		}
			//	}
			//	break;
			}
		}
	}
}

void		GCSceneEditor3D::OnTreeViewKeyDown(INT nChar)
{
	if(nChar == VK_DELETE)
	{
		this->OnDeleteTree();
	}
}

BOOL		GCSceneEditor3D::IsCopyTree()
{
	//if(this->m_editorMode && ::GetFocus() == GetDeviceWindow())
	//{
	//	if(this->IsTextureEditorMode())
	//		return (this->m_selectRegions.size() > 0);
	//}
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			switch(type->type)
			{
			case SCENE3D_FOG:
			case SCENE3D_CAMERA:
			case SCENE3D_SKY:
			case SCENE3D_EFFECT_SHADOW:
			case SCENE3D_WATER:
			case SCENE3D_GRASS:
			case SCENE3D_ANIMATION:
			case SCENE3D_SCA:
			case SCENE3D_SCA_TRACK:
				return true;
			}
		}
	}
	return false;
}

BOOL		GCSceneEditor3D::IsPasteTree()
{
	if(this->m_editorMode && ::GetFocus() == GetDeviceWindow())
	{
		if(this->IsTextureEditorMode())
			return m_bCopyTextureData;
	}
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(IsCanPasteScene3D(type->type))
				return true;
		}
	}
	return false;
}

VOID		GCSceneEditor3D::OnDeleteTree()
{
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			selectTypes[hItem]	=	type;
		}
	}
	//
	BOOL bUnits	=	false;
	BOOL bEvents = false;
	if(MessageBox(this->GetViewHandle(), Language(L"是否真的要删除?"), Language(L"提示"), MB_YESNO) == IDYES)
	{
		bool bRefreshGrass	=	false;
		for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
		{
			TreeItemType* type	=	it->second;
			if(!type)
				continue;
			Phantom::PtrID id;
			id.id	=	type->p1;
			//
			switch(type->type)
			{
			case SCENE3D_LIGHT:
				{
					if(type->p1>0){
						m_scene3D->GetActiveEnv()->RemoveLight(type->p1);
						m_treeView.DeleteItem(it->first);
						m_scene3D->m_lightChanged++;
					}
				}
				break;
			case SCENE3D_ENV:
				{
					m_scene3D->RemoveEnv(type->p1);
					m_treeView.DeleteItem(it->first);
				}
				break;
			case SCENE3D_CAMERA:
				//Phantom::GetCameraPtr()->SetCameraConfig(m_currentCamera);
				if(m_scene3D->GetCameraDataPtr()->RemoveData(id))
					m_treeView.DeleteItem(it->first);
				break;
			case SCENE3D_SKY:
				if(m_scene3D->m_skyDataPtr.RemoveData(id))
					m_treeView.DeleteItem(it->first);
				break;
			case SCENE3D_POINT:
				{
					Phantom::ScenePoint* pos = m_scene3D->GetActiveEnv()->GetPointByID(type->p1);
					if(pos){
						bEvents = true;
						m_scene3D->GetActiveEnv()->RemovePoint(pos);
						m_treeView.DeleteItem(it->first);
					}
				}
				break;			//case SCENE3D_EFFECT_SHADOW:
			//	if(m_scene3D->GetShadowDataPtr()->RemoveData(id))
			//		m_treeView.DeleteItem(it->first);
			//	break;
			//case SCENE3D_EFFECT_ADVANCED:
			//	if(m_scene3D->GetAdvancedEffects().RemoveData(id))
			//		m_treeView.DeleteItem(it->first);
			//	break;
			//case SCENE3D_WATER:
			//	if(m_scene3D->GetWaterDataPtr()->RemoveData(id))
			//		m_treeView.DeleteItem(it->first);
			//	break;
			case SCENE3D_UNIT:
				{
					Phantom::SceneObject* obj = m_scene3D->GetObjectByID(type->p1);
					if(obj){
						bUnits = true;
						m_scene3D->RemoveObject(obj);
						m_treeView.DeleteItem(it->first);
					}
				}
				break;
			case SCENE3D_OBJECT:
				if(m_scene3D->m_objectFilePtr.RemoveData(id))
					m_treeView.DeleteItem(it->first);
				break;
			//case SCENE3D_GRASS:
			//	bRefreshGrass	=	true;
			//	if(m_scene3D->GetGrassDataPtr()->RemoveData(id))
			//		m_treeView.DeleteItem(it->first);
			//	break;
			//case SCENE3D_ANIMATION:
			//	{
			//		if(m_scene3D->GetAnimationManager().RemoveData(id))
			//		{
			//			m_selectLines.clear();
			//			m_treeView.DeleteItem(it->first);
			//		}
			//	}
			//	break;
			//case SCENE3D_SCA:
			//	{
			//		ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
			//		id.id	=	type->p2;
			//		if(ani && ani->GetScaManager().RemoveData(id))
			//		{
			//			m_selectLines.clear();
			//			m_treeView.DeleteItem(it->first);
			//		}
			//	}
			//	break;
			//case SCENE3D_SCA_TRACK:
			//	{
			//		ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
			//		if(ani)
			//		{
			//			ISCA_Base* base	=	ani->GetScaManager().SearchDataPtr(type->p2);
			//			id.id	=	type->p3;
			//			if(base && base->GetTrackManager().RemoveData(id))
			//			{
			//				m_selectLines.clear();
			//				m_treeView.DeleteItem(it->first);
			//			}
			//		}
			//	}
			//	break;
			}
		}
		//if(bRefreshGrass)
		//	this->RefreshGrasss(this->m_hGrassList);
		m_treeView.ClearSelect();
		this->OnChangeTreeSelect(&m_treeView, NULL);
		//if(bUnits)
		//	RebuildUnits();
		if(bEvents)
			this->RebuildPoints();
		this->SetModifyed();
	}
}

BOOL		GCSceneEditor3D::IsDeleteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			switch(type->type)
			{
			case SCENE3D_FOG:
			case SCENE3D_CAMERA:
			case SCENE3D_SKY:
			case SCENE3D_EFFECT_SHADOW:
			case SCENE3D_WATER:
			case SCENE3D_OBJECT:
			case SCENE3D_GRASS:
			case SCENE3D_ANIMATION:
			case SCENE3D_SCA:
			case SCENE3D_SCA_TRACK:
				return true;
			}
		}
	}
	return false;
}

VOID		GCSceneEditor3D::OnCopyTree()
{
	//if(this->m_editorMode && ::GetFocus() == GetDeviceWindow())
	//{
	//	if(this->IsTextureEditorMode())
	//	{
	//		if(this->m_selectRegions.size() == 0)
	//			return;
	//		Phantom::Pixel& last	=	m_selectRegions[m_selectRegions.size() - 1];
	//		Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion(last.x, last.y);
	//		if(pRegion)
	//		{
	//			this->m_bCopyTextureData	=	true;
	//			for(int i=0;i<MAX_LAYER_TEXTURE;i++)
	//			{
	//				m_copyTextureData[i].nTextureIDs		=	pRegion->m_layerTextureID[i];//GetLayerTextureID(i);
	//				//m_copyTextureData[i].fTextureMipMaps	=	pRegion->GetMipmapBias(i);
	//				//m_copyTextureData[i].uvPosition			=	Phantom::float2(pRegion->GetUVPositionX(i), pRegion->GetUVPositionY(i));
	//				//m_copyTextureData[i].uvScaling			=	Phantom::float2(pRegion->GetUVScaleX(i), pRegion->GetUVScaleY(i));
	//				//m_copyTextureData[i].uvRotation			=	pRegion->GetUVRotation(i);
	//			}
	//		}
	//		return;
	//	}
	//}
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			selectTypes[hItem]	=	type;
		}
	}
	std::vector<TreeItemType*> types;
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		switch(type->type)
		{
		case SCENE3D_FOG:
		case SCENE3D_CAMERA:
		case SCENE3D_SKY:
		case SCENE3D_EFFECT_SHADOW:
		case SCENE3D_WATER:
		case SCENE3D_GRASS:
		case SCENE3D_ANIMATION:
		case SCENE3D_SCA:
		case SCENE3D_SCA_TRACK:
			types.push_back(type);
			break;
		}
	}
	SetSceneCopy3D(this, &types);
}

VOID		GCSceneEditor3D::PasteAlls()
{
	//if(this->IsTextureEditorMode())
	//{
	//	if(this->m_selectRegions.size() == 0)
	//		return;
	//	for(int i=0;i<this->m_scene3D->GetSceneRegionCount();i++)
	//	{
	//		Phantom::SceneBox* r	=	m_scene3D->GetSceneRegionPtr(i);
	//		if(r)// && r->m_create)
	//		{
	//			for(int i=0;i<MAX_LAYER_TEXTURE;i++)
	//			{
	//				r->m_layerTextureID[i] = m_copyTextureData[i].nTextureIDs;
	//				//r->SetMipmapBias(i, m_copyTextureData[i].fTextureMipMaps);
	//				//r->SetUVPosition( i, Phantom::float2(0, 0) );//m_copyTextureData[i].uvPosition );
	//				//r->SetUVScale( i, m_copyTextureData[i].uvScaling );
	//				//r->SetUVRotation( i, m_copyTextureData[i].uvRotation );
	//			}
	//			this->RefreshLayerImages();
	//			this->SetModifyed();
	//		}
	//	}
	//	return;
	//}
}

VOID		GCSceneEditor3D::OnPasteTree()
{
	//if(this->m_editorMode && ::GetFocus() == GetDeviceWindow())
	//{
	//	if(this->IsTextureEditorMode())
	//	{
	//		if(this->m_selectRegions.size() == 0)
	//			return;
	//		for(int r=0;r<m_selectRegions.size();r++)
	//		{
	//			Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion(m_selectRegions[r].x, m_selectRegions[r].y);
	//			if(pRegion)
	//			{
	//				for(int i=0;i<MAX_LAYER_TEXTURE;i++)
	//				{
	//					pRegion->m_layerTextureID[i] = m_copyTextureData[i].nTextureIDs;
	//					//pRegion->SetMipmapBias(i, m_copyTextureData[i].fTextureMipMaps);
	//					//pRegion->SetUVPosition( i, Phantom::float2(0, 0) );//m_copyTextureData[i].uvPosition );
	//					//pRegion->SetUVScale( i, m_copyTextureData[i].uvScaling );
	//					//pRegion->SetUVRotation( i, m_copyTextureData[i].uvRotation );
	//				}
	//				this->RefreshLayerImages();
	//				this->SetModifyed();
	//			}
	//		}
	//		return;
	//	}
	//}
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			selectTypes[hItem]	=	type;
		}
	}
	bool bPaste	=	false;
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(IsCanPasteScene3D(type->type))
		{
			bPaste	=	true;
			break;
		}
	}
	if(!bPaste)
		return;
	try{
		for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
		{
			TreeItemType* type	=	it->second;
			if(!type)
				continue;
			for(int j=0;j<g_copyScene3Ds.size();j++)
			{
				Phantom::PtrID fromID, toID;
				fromID.id	=	g_copyScene3Ds[j].type.p1;
				toID.id		=	type->p1;
				//
				//if(g_copyScene3Ds[j].type.type == SCENE3D_FOG)
				//{
				//	Phantom::SceneFogData* from	=	g_copyScene3Ds[j].editor->m_scene3D->m_fogDataPtr.GetDataPtr(fromID);//m_cursorPtr->GetCursorByID(g_copyScene3Ds[j].type.p1);
				//	if(from)
				//		m_scene3D->m_fogDataPtr.SetData(toID, *from);
				//}
				//else 
				if(g_copyScene3Ds[j].type.type == SCENE3D_SKY)
				{
					Phantom::SceneSkyData* from	=	g_copyScene3Ds[j].editor->m_scene3D->m_skyDataPtr.GetDataPtr(fromID);//m_cursorPtr->GetCursorByID(g_copyScene3Ds[j].type.p1);
					if(from)
						m_scene3D->m_skyDataPtr.SetData(toID, *from);
				}
				//else if(g_copyScene3Ds[j].type.type == SCENE3D_EFFECT_SHADOW)
				//{
				//	SceneShadowData* from	=	g_copyScene3Ds[j].editor->m_scene3D->GetShadowDataPtr()->GetDataPtr(fromID);//m_cursorPtr->GetCursorByID(g_copyScene3Ds[j].type.p1);
				//	if(from)
				//		m_scene3D->GetShadowDataPtr()->SetData(toID, *from);
				//}
				//else if(g_copyScene3Ds[j].type.type == SCENE3D_EFFECT_ADVANCED)
				//{
				//	IAdvancedEffect* from	=	g_copyScene3Ds[j].editor->m_scene3D->GetAdvancedEffects().GetDataPtr(fromID);//m_cursorPtr->GetCursorByID(g_copyScene3Ds[j].type.p1);
				//	if(from)
				//		m_scene3D->GetAdvancedEffects().CloneData(toID, from);
				//}
				else if(g_copyScene3Ds[j].type.type == SCENE3D_CAMERA)
				{
					Phantom::SceneCameraData* from	=	g_copyScene3Ds[j].editor->m_scene3D->GetCameraDataPtr()->GetDataPtr(fromID);//m_cursorPtr->GetCursorByID(g_copyScene3Ds[j].type.p1);
					Phantom::SceneCameraData* to		=	m_scene3D->GetCameraDataPtr()->GetDataPtr(toID);
					if(from && to)
					{
						memcpy(to, from, sizeof(CameraData));
						Phantom::GetCameraPtr()->SetCameraConfig(*to);
					}
				}
				//else if(g_copyScene3Ds[j].type.type == SCENE3D_WATER)
				//{
				//	const SceneWaterData* from	=	g_copyScene3Ds[j].editor->m_scene3D->GetWaterDataPtr()->GetDataPtr(fromID);//m_cursorPtr->GetCursorByID(g_copyScene3Ds[j].type.p1);
				//	if(from)
				//		m_scene3D->GetWaterDataPtr()->SetData(toID, *from);
				//}
				//else if(g_copyScene3Ds[j].type.type == SCENE3D_GRASS)
				//{
				//	ISceneGrassTexture* from	=	g_copyScene3Ds[j].editor->m_scene3D->GetGrassDataPtr()->GetDataPtr(fromID);//m_cursorPtr->GetCursorByID(g_copyScene3Ds[j].type.p1);
				//	if(from)
				//		m_scene3D->GetGrassDataPtr()->CloneData(toID, from);
				//}
				//else if(g_copyScene3Ds[j].type.type == SCENE3D_ANIMATION && type->type == g_copyScene3Ds[j].type.type)
				//{
				//	ISCA_Animation* from	=	g_copyScene3Ds[j].editor->m_scene3D->GetAnimationManager().GetDataPtr(fromID);//m_cursorPtr->GetCursorByID(g_copyScene3Ds[j].type.p1);
				//	ISCA_Animation* to		=	m_scene3D->GetAnimationManager().GetDataPtr(toID);//m_cursorPtr->GetCursorByID(g_copyScene3Ds[j].type.p1);
				//	if(from && to)
				//	{
				//		to->CloneFrom(from);
				//		this->RefreshAnimation(to, it->first);
				//	}
				//}
				//else if(g_copyScene3Ds[j].type.type == SCENE3D_SCA && type->type == g_copyScene3Ds[j].type.type)
				//{
				//	fromID.id	=	g_copyScene3Ds[j].type.p2;
				//	toID.id		=	type->p2;
				//	ISCA_Animation* from	=	g_copyScene3Ds[j].editor->m_scene3D->GetAnimationManager().SearchDataPtr(g_copyScene3Ds[j].type.p1);
				//	ISCA_Animation* to		=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
				//	if(from && to)
				//	{
				//		ISCA_Base* base		=	from->GetScaManager().GetDataPtr(fromID);
				//		ISCA_Base* toBase	=	to->GetScaManager().GetDataPtr(toID);
				//		if(base && toBase)
				//		{
				//			toBase->CloneFrom(base);
				//			this->RefreshSCA(to, toBase, it->first);
				//		}
				//	}
				//}
				//else if(g_copyScene3Ds[j].type.type == SCENE3D_SCA_TRACK && type->type == g_copyScene3Ds[j].type.type)
				//{
				//	fromID.id	=	g_copyScene3Ds[j].type.p3;
				//	toID.id		=	type->p3;
				//	ISCA_Animation* from	=	g_copyScene3Ds[j].editor->m_scene3D->GetAnimationManager().SearchDataPtr(g_copyScene3Ds[j].type.p1);
				//	ISCA_Animation* to		=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
				//	if(from && to)
				//	{
				//		ISCA_Base* fromB	=	from->GetScaManager().SearchDataPtr(g_copyScene3Ds[j].type.p2);
				//		ISCA_Base* toB		=	to->GetScaManager().SearchDataPtr(type->p2);
				//		if(fromB && toB)
				//		{
				//			ISCA_Track* track	=	fromB->GetTrackManager().GetDataPtr(fromID);
				//			ISCA_Track* trackTo	=	toB->GetTrackManager().GetDataPtr(toID);
				//			if(track && trackTo)
				//				trackTo->CloneFrom(track);
				//		}
				//	}
				//}
			}
		}
		this->OnChangeTreeSelect(&m_treeView, NULL);
		this->SetModifyed();
	}
	catch(...)
	{
		assert(false);
		g_copyScene3Ds.clear();
	}
}

float	GCSceneEditor3D::GetHeightAmount(const Phantom::float3& dir, float fEditRange, float fPower)
{
	float fRange	=	dir.R();
	fPower			=	max(fPower, 0.1f);
	if(this->m_bUseBrushTexture && m_brushPtr)
	{
		switch(this->m_brushType)
		{
		case BrushType_Circle:
			{
				float fR		=	fRange / fEditRange;
				if(fR > 0.0f)
					fR	=	pow(fR, fPower);
				if(this->m_bBrush01Mode)
					return (.5f - fR) * this->m_brushPtr->GetHeightPixel((int)((dir.x / fEditRange * 0.5f + 0.5f) * fEditRange * 2.0f), (int)((dir.y / fEditRange * 0.5f + 0.5f) * fEditRange * 2.0f));
				return (.5f - fR) * this->m_brushPtr->GetHeight(dir.x / fEditRange * 0.5f + 0.5f, dir.y / fEditRange * 0.5f + 0.5f);
			}
			break;
		case BrushType_Box:
			{
				float fX		=	(dir.x * dir.x / (fEditRange * fEditRange));
				float fY		=	(dir.y * dir.y / (fEditRange * fEditRange));
				if(fX > 0.0f)fX	=	pow(fX, fPower);
				if(fY > 0.0f)fY =	pow(fY, fPower);
				if(this->m_bBrush01Mode)
					return (.5f - max(fX , fY)) * this->m_brushPtr->GetHeightPixel((int)((dir.x / fEditRange * 0.5f + 0.5f) * fEditRange * 2.0f), (int)((dir.y / fEditRange * 0.5f + 0.5f) * fEditRange * 2.0f));
				return (.5f - max(fX , fY)) * this->m_brushPtr->GetHeight(dir.x / fEditRange * 0.5f + 0.5f, dir.y / fEditRange * 0.5f + 0.5f);
			}
			break;
		}
	}
	else
	{
		switch(this->m_brushType)
		{
		case BrushType_Circle:
			{
				float fR		=	fRange / fEditRange;
				if(fR > 0.0f)
					fR	=	pow(fR, fPower);
				return (1.0f - fR);
			}
			break;
		case BrushType_Box:
			{
				float fX		=	(dir.x * dir.x / (fEditRange * fEditRange));
				float fY		=	(dir.y * dir.y / (fEditRange * fEditRange));
				if(fX > 0.0f)fX	=	pow(fX, fPower);
				if(fY > 0.0f)fY =	pow(fY, fPower);
				return (1.0f - max(fX , fY));
			}
			break;
		}
	}
	return 0.0f;
}

extern	VOID	ShowSceneMaxAdd();
VOID		GCSceneEditor3D::InsertGeometry(const wchar_t* szFileName)
{
	const wchar_t* szF = szFileName + wcslen(W(GetProjectPath()).buf);
	if(!fexist(A(szF)))
		return;
	Phantom::SceneObjectFile* obj	=	m_scene3D->m_objectFilePtr.AddData(0);
	obj->name	=	A(szF).buf;
	m_scene3D->InsertObject(A(szF).buf, Phantom::GetCameraPtr()->GetTarget());
	this->m_dlgObjects.RefreshSceneUnits();
	this->m_dlgObjects.RefershRoles();
}
void		GCSceneEditor3D::OnDoubleClick(CMTreeCtrl* treePtr, HTREEITEM hItem)	//更新选择树形控件
{
	TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
	if(!type)
		return;
	if(type->type==SCENE3D_POINT_LIST){

	}
	else if(type->type == this->SCENE3D_OBJECT)
	{
		SetProjectPath();
		Phantom::SceneObjectFile* file	=	m_scene3D->m_objectFilePtr.SearchDataPtr(type->p1);
		if(!file)
			return;
		Phantom::Camera* c		=	Phantom::GetCameraPtr();
		RECT rc;
		GetClientRect(GetDeviceWindow(), &rc);
		Phantom::Pixel pixel(rc.left + (rc.right - rc.left)/2, rc.top + (rc.bottom - rc.top) / 2);
		Phantom::float3 orig, dir;
		c->Get3DFrom2D(orig, dir, pixel);
		Phantom::PickPoint data;
		if(!m_scene3D->HasTerrainData()){
			orig.x -= dir.x*(orig.z/dir.z);
			orig.y -= dir.y*(orig.z/dir.z);
			orig.z = 0;
			data.pos = orig;
		}
		else if(m_scene3D->IntersectTerrain(orig, dir, data))
		{
		}
		else
			return;
		m_scene3D->InsertObject(file->name.t, data.pos);
		SetModifyed();
		this->m_dlgObjects.RefreshSceneUnits();
		this->m_dlgObjects.RefershRoles();
		switch(this->m_editorMode)
		{
		case EditorMode_ObjectMove:
		case EditorMode_ObjectScale:
		case EditorMode_ObjectRotation:
			break;
		default:
			SetEditorMode(EditorMode_ObjectMove);
			break;
		}
		RefreshUnits();
	}
}

void		GCSceneEditor3D::SetViewCenter()
{
	switch(m_editorMode)
	{
	case EditorMode_HeightAdd:
	case EditorMode_HeightAvg:
	case EditorMode_HeightAlign:
	case EditorMode_HeightSet:
	case EditorMode_HeightGridVisible:
	case EditorMode_HeightGridEnable:
	case EditorMode_RegionSet:
	case EditorMode_HeightMapAdd:
	case EditorMode_HeightMapSet:
	case EditorMode_TextureAdd:
	case EditorMode_TextureSet:
	case EditorMode_TextureAvg:
	case EditorMode_TextureUVMoving:
	case EditorMode_TextureUVScaling:
	case EditorMode_TextureUVRotation:
	case EditorMode_AddGrass:
	case EditorMode_RemoveGrass:
		Phantom::GetCameraPtr()->SetTarget(this->m_vHeightCenter.pos);
		break;
	case EditorMode_ObjectMove:
	case EditorMode_ObjectScale:
	case EditorMode_ObjectRotation:
		Phantom::GetCameraPtr()->SetTarget(this->m_selectCenter);
		break;
	case EditorMode_LineEditorMode:
		Phantom::GetCameraPtr()->SetTarget(this->m_selectCenter);
		break;
	}
}


VOID		GCSceneEditor3D::PlayAnimation()
{
	this->SetPlay();
}
VOID		GCSceneEditor3D::StopAnimation()
{
	this->SetStop();
}
VOID		GCSceneEditor3D::PauseAnimation(BOOL bPause)
{
	this->SetPause(bPause);
}
VOID		GCSceneEditor3D::SetAnimationLoop(BOOL bLoop)
{
	this->SetLoopMode(bLoop);
}
BOOL		GCSceneEditor3D::IsAnimationCanPause()
{
	return true;
}
BOOL		GCSceneEditor3D::IsAnimationCanPlayStop()
{
	return true;
}
BOOL		GCSceneEditor3D::IsAnimationCanLoop()
{
	return true;
}
BOOL		GCSceneEditor3D::IsAnimationPlaying()
{
	return this->m_bIsPlaying;
}
BOOL		GCSceneEditor3D::IsAnimationPause()
{
	return this->m_bIsPause;
}
BOOL		GCSceneEditor3D::IsAnimationLoop()
{
	return this->m_bLoopMode;
}

void		GCSceneEditor3D::OnRendering()	//渲染
{
	if(IsShowUIGrid())
	{
		Phantom::g_manager.SetWorldMatrix(&Phantom::matrix4x4::s_identity());
		Phantom::g_manager->SetZWriteEnable(false);
		Phantom::g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		Phantom::g_manager->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_INVSRCALPHA);
		Phantom::float3 v = Phantom::GetCameraPtr()->GetTarget();
		float fGrid = To3DValue(Get3DGridSize());
		float fX = (int)(v.x / fGrid);
		float fY = (int)(v.y / fGrid);
		v.setxyz(fX * fGrid, fY * fGrid, 0);
		//GetRenderHelperPtr()->drawGrid(v, 0x3f3f3f3f, fGrid, 20, 20);
		int numW = 20;
		int numH = 20;
		Phantom::float3 _eye(this->m_scene3D->GetWorldSize()/2, this->m_scene3D->GetWorldSize()/2, 0);
		_eye.z = 0;
		//
		float fWidth2	=	m_scene3D->GetWorldSize()/2;//fGrid* (float)numW;
		float fHeight2	=	m_scene3D->GetWorldSize()/2;//fGrid* (float)numH;
		//
		Phantom::g_manager.DrawLine(	_eye + Phantom::float3(fWidth2,0.0f,0.0f),		_eye + Phantom::float3(0.0f,0.0f,0.0f),		&Phantom::color4(1.0f,0.0f,0.0f,1.0f));
		Phantom::g_manager.DrawLine(	_eye + Phantom::float3(0.0f,0.0f,0.0f),			_eye + Phantom::float3(-fWidth2,0.0f,0.0f),	&Phantom::color4(0.0f,1.0f,1.0f,0.5f));
		Phantom::g_manager.DrawLine(	_eye + Phantom::float3(0.0f,fHeight2,0.0f),		_eye + Phantom::float3(0.0f,0.0f,0.0f),		&Phantom::color4(0.0f,1.0f,0.0f,1.0f));
		Phantom::g_manager.DrawLine(	_eye + Phantom::float3(0.0f,0.0f,0.0f),			_eye + Phantom::float3(0.0f,-fHeight2,0.0f),	&Phantom::color4(1.0f,0.0f,1.0f,0.5f));
		Phantom::g_manager.DrawLine(	_eye + Phantom::float3(0.0f,0.0f,0.0f),			_eye + Phantom::float3(0.0,0.0f,min(fWidth2,fHeight2) * 0.25f),			&Phantom::color4(0.0f,0.0f,1.0f,1.0f));
		Phantom::g_manager.DrawLine(	_eye + Phantom::float3(0.0f,0.0f,0.0f),			_eye + Phantom::float3(0.0,0.0f,-min(fWidth2,fHeight2)* 0.25f),		&Phantom::color4(1.0f,1.0f,0.0f,0.5f));
		//
		fGrid = m_scene3D->GetWorldSize() / numW * 0.5;
		Phantom::color4 uiGridColor(0x3f3f3f3f);
		for(int x = -numW;x<=numW;x++)
		{
			if(x == 0)continue;
			float fX = (float)x * fGrid;
			Phantom::g_manager.DrawLine(Phantom::float3(fX, fHeight2, 0.0f) + _eye,	Phantom::float3(fX, -fHeight2, 0.0f) + _eye, &uiGridColor);
		}
		for(int y = -numH;y<=numH;y++)
		{
			if(y == 0)continue;
			float fY = (float)y * fGrid;
			Phantom::g_manager.DrawLine(Phantom::float3(fWidth2, fY, 0.0f) + _eye,	Phantom::float3(-fWidth2, fY, 0.0f) + _eye, &uiGridColor);
		}
		Phantom::g_manager->SetZWriteEnable(true);
	}
	Phantom::g_manager.SetWorldMatrix(&Phantom::matrix4x4::s_identity());
	if(m_scene3D->IsEnabled(Phantom::Scene3D::Enable_ShowObject)){
		unsigned int eventColor = 0x8822ff22;
		unsigned int atkColor = 0x88ff2222;
		for(int i=0;i<m_scene3D->GetActiveEnv()->m_points.size();i++)
		{
			Phantom::AABox box;
			box._min = box._max = m_scene3D->GetActiveEnv()->m_points[i]->GetPosition();
			box._min -= m_scene3D->GetActiveEnv()->m_points[i]->GetRange();
			box._max += m_scene3D->GetActiveEnv()->m_points[i]->GetRange();
			Phantom::g_manager.DrawBox(box, eventColor, false);
			//RenderBox(m_scene3D->m_points[i]->GetPosition(), m_scene3D->m_points[i]->GetRange(), eventColor);
		}
		for(int i=0;i<m_scene3D->GetObjectCount();i++)
		{
			Phantom::SceneObject* obj = m_scene3D->GetObjectPtr(i);
			if(!obj||!(obj->GetMeshPtr()->GetDataPtr()->m_meshType==Phantom::MeshData::MeshType_Role))
				continue;
			DrawCircle(obj->GetPosition(), obj->m_fRange, eventColor, 24);
			DrawCircle(obj->GetPosition(), obj->m_fAtkRange, atkColor, 24);
			//RenderBox(m_scene3D->m_points[i]->GetPosition(), m_scene3D->m_points[i]->GetRange(), eventColor);
		}
		Phantom::SceneEnv* env = m_scene3D->GetActiveEnv();
		for(int i=0;i<env->m_lights.size();i++){
			Phantom::SceneLight* light = env->m_lights[i];
			Phantom::float3 lens = light->m_pos - Phantom::GetCameraPtr()->GetEye();
			float fSize = clamp(lens.length()/To3DValue(10.0f), 1, 5);
			Phantom::color4 c(light->m_lightColor);
			DrawOmni(light->m_pos, c, fSize);
			if(light->m_lightType==Phantom::SceneLight::LightType_Direction||light->m_lightType==Phantom::SceneLight::LightType_Spot){
				Phantom::float3 target = light->m_pos + light->m_dir * light->GetNear();
				Phantom::g_manager.DrawLine(light->m_pos, target, &c);
				DrawOmni(target, c, fSize);
			}
		}
	}
	if(m_scene3D->m_bClipCamera){
		Phantom::AABox box;
		float center = m_scene3D->GetWorldSize()/2;
		float fmin = center - (center*m_scene3D->m_fClipCameraSize);
		float fmax = center + (center*m_scene3D->m_fClipCameraSize);
		Phantom::Rect rc(fmin,fmin,fmax,fmax);
		box._min.setxyz(rc.left, rc.top, 0);
		box._max.setxyz(rc.right, rc.bottom, 1);
		Phantom::g_manager.DrawBox(box, 0xffff0000, false);
	}
	Phantom::g_manager->SetCullMode(Phantom::CullMode_Double);
	switch(m_editorMode)
	{
	case EditorMode_HeightAdd:
	case EditorMode_HeightAvg:
	case EditorMode_HeightAlign:
	case EditorMode_HeightSet:
	case EditorMode_HeightGridVisible:
	case EditorMode_HeightGridEnable:
	case EditorMode_RegionSet:
	case EditorMode_HeightMapAdd:
	case EditorMode_HeightMapSet:
	case EditorMode_TextureAdd:
	case EditorMode_TextureSet:
	case EditorMode_TextureAvg:
	case EditorMode_TextureUVMoving:
	case EditorMode_TextureUVScaling:
	case EditorMode_TextureUVRotation:
	case EditorMode_AddGrass:
	case EditorMode_RemoveGrass:
		OnRenderingHeight();
		break;
	case EditorMode_ObjectMove:
	case EditorMode_ObjectScale:
	case EditorMode_ObjectRotation:
		OnRenderingObject();
		break;
	case EditorMode_LineEditorMode:
		OnRenderingLine();
		break;
	}

}
