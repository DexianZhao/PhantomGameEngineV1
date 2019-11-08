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



void	OnEditorAddHeight(GCSceneEditor3D* dlg, INT nGridX, INT nGridY, const Phantom::float3& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	float fElapsedTime				=	Phantom::g_manager.GetElapsedTime();
	//
	Phantom::GVertexData* attr		=	dlg->m_scene3D->GetVertexAttribute(nGridX, nGridY);
	if(!attr)
		return;
	//if(dlg->m_bUndoBuffer && dlg->m_fHeightRadius < MAX_UNDO_RANGE)
	//	dlg->AddUndoVertexData(nGridX, nGridY);
	Phantom::GVertexData		newAttr	=	*attr;
	float fValue				=	dlg->GetHeightAmount(dir, dlg->m_fHeightRadius, info.fPower) * info.fVar * fElapsedTime;
	if(GetSelectKeyboard())
		fValue					=	-fValue;
	float fHeight				=	attr->GetHeight() + fValue;
	newAttr.SetHeight(fHeight);
	dlg->m_scene3D->SetVertexAttribute(nGridX, nGridY, &newAttr);
	dlg->SetModifyed();
}

//
void	OnEditorSetHeight(GCSceneEditor3D* dlg, INT nGridX, INT nGridY, const Phantom::float3& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	float fElapsedTime			=	Phantom::g_manager.GetElapsedTime();
	Phantom::GVertexData* attr		=	dlg->m_scene3D->GetVertexAttribute(nGridX, nGridY);
	if(!attr)
		return;
	//if(dlg->m_bUndoBuffer && dlg->m_fHeightRadius < MAX_UNDO_RANGE)
	//	dlg->AddUndoVertexData(nGridX, nGridY);
	if(dlg->m_bUseBrushTexture)
	{
		Phantom::GVertexData val			=	*attr;
		val.SetHeight(info.fVar * dlg->GetHeightAmount(dir, dlg->m_fHeightRadius, info.fPower));
		dlg->m_scene3D->SetVertexAttribute( nGridX, nGridY, &val );
	}
	else
	{
		Phantom::GVertexData val			=	*attr;
		val.SetHeight(info.fVar);
		dlg->m_scene3D->SetVertexAttribute( nGridX, nGridY, &val );
	}
	dlg->SetModifyed();
}

void	OnEditorAddGrass(GCSceneEditor3D* dlg, INT nGridX, INT nGridY, const Phantom::float3& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	//if(dlg->m_selectGrassPtrID.id == 0)
	//	return;
	//float fElapsedTime		=	Phantom::g_manager.GetElapsedTime();
	//int count	=	(int)info.fVar + 1;
	//for(int i=0;i<count;i++)
	//	if(!dlg->m_scene3D->AddRandomGrass(nGridX, nGridY, dlg->m_selectGrassPtrID, 0))
	//		return;
}

void	OnEditorDelGrass(GCSceneEditor3D* dlg, INT nGridX, INT nGridY, const Phantom::float3& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	//float fElapsedTime		=	Phantom::g_manager.GetElapsedTime();
	////
	//Phantom::PtrID* tex	=	&dlg->m_selectGrassPtrID;
	//if(GetCopyKeyboard())
	//	tex	=	0;
	//int count	=	(int)info.fVar + 1;
	//for(int i=0;i<count;i++)
	//	if(!dlg->m_scene3D->RemoveOneGrass(nGridX, nGridY, tex))
	//		return;
}

void	OnEditorShowGrid(GCSceneEditor3D* dlg, INT nGridX, INT nGridY, const Phantom::float3& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	BOOL bEnable	=	info.bEnable;
	if(IsKeyDown(VK_CONTROL))
		bEnable		=	!bEnable;
	dlg->m_scene3D->SetGridAttrVisible(nGridX, nGridY, bEnable);
	dlg->SetModifyed();
}

void	OnEditorRegionSet(GCSceneEditor3D* dlg, INT nMX, INT nMY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	Phantom::GMoveData* pRuntime	=	dlg->m_scene3D->GetMoveAttribute(nMX, nMY);
	if(!pRuntime||dlg->m_regionIndex<0)return;
	int index = dlg->m_regionIndex;
	if(IsKeyDown(VK_CONTROL))
		index = 0;
	dlg->m_scene3D->SetGridRegionIndex(nMX, nMY, index);
	//dlg->m_scene3D->GetSceneRegion(nMX/dlg->m_scene3D->GetRegionWidthGrids()/MOVE_GRID_SIZE, nMY/dlg->m_scene3D->GetRegionWidthGrids()/MOVE_GRID_SIZE)->SetModifyed(true);
	dlg->SetModifyed();
}

//void	OnEditorMoveEnable(GCSceneEditor3D* dlg, INT nGridX, INT nGridY, const Phantom::float3& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
//{
//	float fElapsedTime		=	Phantom::g_manager.GetElapsedTime();
//	GridAttributeLevel* pRuntime	=	dlg->m_scene3D->GetBaseLevelAttribute(nGridX, nGridY);
//	if(!pRuntime)return;
//	BOOL bEnable	=	info.bEnable;
//	if(IsKeyDown(VK_CONTROL))
//		bEnable		=	!bEnable;
//	//if(dlg->m_bUndoBuffer && dlg->m_fHeightRadius < MAX_UNDO_RANGE)
//	//	dlg->AddUndoVertexData(nGridX, nGridY);
//	pRuntime->setMoveEnable(bEnable);
//	dlg->SetModifyed();
//}

void	OnEditorFromHeightMapSet(GCSceneEditor3D* dlg, INT nGridX, INT nGridY, const Phantom::float3& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	//if(dlg->m_imageBits.IsNull())return;
	//float fElapsedTime			=	Phantom::g_manager.GetElapsedTime();
	//Phantom::GVertexData* attr		=	dlg->m_scene3D->GetVertexAttribute(nGridX, nGridY);
	//if(!attr)
	//	return;
	////
	////if(dlg->m_bUndoBuffer && dlg->m_fHeightRadius < MAX_UNDO_RANGE)
	////	dlg->AddUndoVertexData(nGridX, nGridY);
	//Phantom::GVertexData val			=	*attr;
	////
	//double fWeight			=	0;
	//int count	=	0;
	//for(int x=-dlg->m_blendCount;x<=dlg->m_blendCount;x++)
	//{
	//	for(int y=-dlg->m_blendCount;y<=dlg->m_blendCount;y++)
	//	{
	//		float fF = 0;
	//		if(dlg->m_imageBits.GetPixelWeight(fF, nGridX + x, nGridY + y))
	//		{
	//			fWeight		+=	fF;
	//			count		++;
	//		}
	//	}
	//}
	//if(count > 0)
	//{
	//	val.SetHeight((float)(fWeight / (double)count) * info.fVar);
	//	dlg->m_scene3D->SetVertexAttribute( nGridX, nGridY, &val );
	//}
}

void	OnEditorAverage(GCSceneEditor3D* dlg, INT nGridX, INT nGridY, const Phantom::float3& gridMiddle, const Phantom::float3& dir, Phantom::GVertexData& valRet, SceneEditorInfo& info)
{
	float fElapsedTime			=	Phantom::g_manager.GetElapsedTime();
	Phantom::GVertexData* attr		=	dlg->m_scene3D->GetVertexAttribute(nGridX, nGridY);
	if(!attr)
		return;
	//if(dlg->m_bUndoBuffer && dlg->m_fHeightRadius < MAX_UNDO_RANGE)
	//	dlg->AddUndoVertexData(nGridX, nGridY);
	valRet						=	*attr;
	double fHeight				=	0;
	int	  nCount				=	0;
	for(int x=-3;x<=3;x++)
	{
		for(int y=-3;y<=3;y++)
		{
			Phantom::GVertexData* val	=	dlg->m_scene3D->GetVertexAttribute(nGridX + x, nGridY + y);
			if(val)
			{
				fHeight		+=	(val->GetHeight() - valRet.GetHeight()) * fElapsedTime * 10.0f;
				nCount++;
			}
		}
	}
	if(nCount > 0)
	{
		float fNewHeight	=	(fHeight / (float)nCount);
		if(IsKeyDown(VK_CONTROL))
			valRet.SetHeight(valRet.GetHeight() - fNewHeight * (1.0f - dir.R() / dlg->m_fHeightRadius) * info.fPower);
		else
			valRet.SetHeight(valRet.GetHeight() + fNewHeight * (1.0f - dir.R() / dlg->m_fHeightRadius) * info.fPower);
	}
	dlg->SetModifyed();
}

void	OnEditorAverageAlign(GCSceneEditor3D* dlg, INT nGridX, INT nGridY, const Phantom::float3& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	float fElapsedTime			=	Phantom::g_manager.GetElapsedTime();
	Phantom::GVertexData* attr		=	dlg->m_scene3D->GetVertexAttribute(nGridX, nGridY);
	if(!attr || info.fVar < 1.0f)
		return;
	//if(dlg->m_bUndoBuffer && dlg->m_fHeightRadius < MAX_UNDO_RANGE)
	//	dlg->AddUndoVertexData(nGridX, nGridY);
	Phantom::GVertexData newAttr		=	*attr;
	float fHeight				=	newAttr.GetHeight();
	int nMul					=	(int)fHeight / (int)info.fVar;
	newAttr.SetHeight(info.fVar * (float)nMul);
	dlg->m_scene3D->SetVertexAttribute(nGridX, nGridY, &newAttr);
	dlg->SetModifyed();
}

BOOL	GCSceneEditor3D::GetHeightPoint(int nGridX, int nGridY, Phantom::float3& ret)
{
	switch(m_editorMode)
	{
	case EditorMode_HeightAdd:
	case EditorMode_HeightAvg:
	case EditorMode_HeightAlign:
	case EditorMode_HeightSet:
	case EditorMode_HeightMapAdd:
	case EditorMode_HeightMapSet:
	case EditorMode_AddGrass:
	case EditorMode_RemoveGrass:
		return m_scene3D->GetVertexPosition(nGridX, nGridY, ret);
	case EditorMode_HeightGridVisible:
	case EditorMode_HeightGridEnable:
	case EditorMode_RegionSet:
	case EditorMode_TextureSet:
		return m_scene3D->GetGridMiddle(nGridX, nGridY, ret);
	}
	return FALSE;
}

VOID						GCSceneEditor3D::LoopHeightGridsSaved(ProTerrainGridsSaved proc)
{
	this->m_nHeightPointCount	=	0;
	int range	=	(INT)(this->m_fHeightRadius / m_scene3D->GetGridSize()) + 1;
	int middleX	=	(int)(m_vHeightCenter.pos.x /  m_scene3D->GetGridSize());
	int middleY	=	(int)(m_vHeightCenter.pos.y /  m_scene3D->GetGridSize());
	//
	int					saveCount	=	0;
	GridSaveValue * savePtr	=	(GridSaveValue*)malloc(range * range * 3 * 3 * sizeof(GridSaveValue));
	for(int x = -range;x <= range;x++)
	{
		for(int y=-range;y<=range;y++)
		{
			int xGrid	=	x + middleX;
			int yGrid	=	y + middleY;
			//
			Phantom::float3 gridPos;
			if(GetHeightPoint(xGrid, yGrid, gridPos))
			{
				Phantom::float3 dir	=	gridPos - m_vHeightCenter.pos;
				dir.z		=	0;
				float dR	=	dir.R();
				if(this->m_bBrushMode == 0)
				{
					if(dR >= this->m_fHeightRadius)
						continue;
				}
				else
				{
					if(dir.x > m_fHeightRadius || dir.x < -m_fHeightRadius || dir.y > m_fHeightRadius || dir.y < -m_fHeightRadius)
						continue;
				}
				{
					for(int xx=-1;xx<=1;xx++)
					{
						for(int yy=-1;yy<=1;yy++)
						{
							int xRegion	=	(xGrid + xx) / this->m_scene3D->GetRegionWidthGrids();
							int yRegion	=	(yGrid + yy) / m_scene3D->GetRegionWidthGrids();
							bool bfind	=	false;
							for(int i=0;i<this->m_nHeightPointCount;i++)
							{
								if(m_vRefreshHeightPoints[i].x == xRegion && m_vRefreshHeightPoints[i].y == yRegion)
								{
									bfind	=	true;
									break;
								}
							}
							if(!bfind)
							{
								Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion( xRegion, yRegion );
								if(pRegion)
								{
									m_vRefreshHeightPoints[m_nHeightPointCount].x	=	xRegion;
									m_vRefreshHeightPoints[m_nHeightPointCount].y	=	yRegion;
									m_nHeightPointCount	++;
									pRegion->SetModifyed(true);
									SetModifyed();
								}
							}
							bfind	=	false;
							for(int i=0;i<this->m_nEditRegionCount;i++)
							{
								if(m_vEditRegions[i].x == xRegion && m_vEditRegions[i].y == yRegion)
								{
									bfind	=	true;
									break;
								}
							}
							if(!bfind)
							{
								Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion( xRegion, yRegion );
								if(pRegion)
								{
									m_vEditRegions[m_nEditRegionCount].x	=	xRegion;
									m_vEditRegions[m_nEditRegionCount].y	=	yRegion;
									if(!m_savedData[m_nEditRegionCount])
										m_savedData[m_nEditRegionCount] = new SaveRegionV();
									SaveRegionD(*m_savedData[m_nEditRegionCount], pRegion);
									m_nEditRegionCount	++;
								}
							}
						}
					}
					savePtr[saveCount].gridX = xGrid;
					savePtr[saveCount].gridY = yGrid;
					(*proc)(this, xGrid, yGrid, gridPos, dir, savePtr[saveCount].val, this->m_editorInfo[this->m_editorMode]);
					saveCount++;
					//
				}
			}
			//
		}
	}
	if(savePtr)
	{
		for(int i=0;i<saveCount;i++)
		{
			Phantom::GVertexData* val			=	m_scene3D->GetVertexAttribute(savePtr[i].gridX, savePtr[i].gridY);
			if(val)
			{
				Phantom::GVertexData v	=	*val;
				v.SetHeight(savePtr[i].val.GetHeight());
				m_scene3D->SetVertexAttribute(savePtr[i].gridX, savePtr[i].gridY, &v);
			}
		}
		free(savePtr);
	}
}

VOID						GCSceneEditor3D::LoopHeightGrids(ProTerrainGrids proc, BOOL bIsRender)
{
	m_nHeightPointCount	=	0;
	int range	=	(INT)(m_fHeightRadius / m_scene3D->GetGridSize()) + 1;
	int middleX	=	(int)(m_vHeightCenter.pos.x /  m_scene3D->GetGridSize());
	int middleY	=	(int)(m_vHeightCenter.pos.y /  m_scene3D->GetGridSize());
	for(int x = -range;x <= range;x++)
	{
		for(int y=-range;y<=range;y++)
		{
			int xGrid	=	x + middleX;
			int yGrid	=	y + middleY;
			//
			Phantom::float3 gridPos;
			if(GetHeightPoint(xGrid, yGrid, gridPos))
			{
				Phantom::float3 dir	=	gridPos - m_vHeightCenter.pos;
				dir.z		=	0;
				float dR	=	dir.R();
				if(this->m_bBrushMode == 0)
				{
					if(dR >= this->m_fHeightRadius)
						continue;
				}
				else
				{
					if(dir.x > m_fHeightRadius || dir.x < -m_fHeightRadius || dir.y > m_fHeightRadius || dir.y < -m_fHeightRadius)
						continue;
				}
				if(!bIsRender)
				{
					for(int xx=-1;xx<=1;xx++)
					{
						for(int yy=-1;yy<=1;yy++)
						{
							int xRegion	=	(xGrid + xx) / m_scene3D->GetRegionWidthGrids();
							int yRegion	=	(yGrid + yy) / m_scene3D->GetRegionWidthGrids();
							if(xRegion>=m_scene3D->GetMaxRegionWidth())
								xRegion = m_scene3D->GetMaxRegionWidth() - 1;
							if(yRegion>=m_scene3D->GetMaxRegionWidth())
								yRegion = m_scene3D->GetMaxRegionWidth() - 1;
							bool bfind	=	false;
							for(int i=0;i<this->m_nHeightPointCount;i++)
							{
								if(m_vRefreshHeightPoints[i].x == xRegion && m_vRefreshHeightPoints[i].y == yRegion)
								{
									bfind	=	true;
									break;
								}
							}
							if(!bfind)
							{
								Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion( xRegion, yRegion );
								if(pRegion)
								{
									m_vRefreshHeightPoints[m_nHeightPointCount].x	=	xRegion;
									m_vRefreshHeightPoints[m_nHeightPointCount].y	=	yRegion;
									m_nHeightPointCount	++;
									pRegion->SetModifyed(true);
									SetModifyed();
								}
							}
							//
							bfind	=	false;
							for(int i=0;i<this->m_nEditRegionCount;i++)
							{
								if(m_vEditRegions[i].x == xRegion && m_vEditRegions[i].y == yRegion)
								{
									bfind	=	true;
									break;
								}
							}
							if(!bfind)
							{
								Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion( xRegion, yRegion );
								if(pRegion)
								{
									m_vEditRegions[m_nEditRegionCount].x	=	xRegion;
									m_vEditRegions[m_nEditRegionCount].y	=	yRegion;
									if(!m_savedData[m_nEditRegionCount])
										m_savedData[m_nEditRegionCount] = new SaveRegionV();
									SaveRegionD(*m_savedData[m_nEditRegionCount], pRegion);
									m_nEditRegionCount	++;
								}
							}
						}
					}
				}
				(*proc)(this, xGrid, yGrid, gridPos, dir, this->m_editorInfo[this->m_editorMode]);
			}
			//
		}
	}
}

VOID						GCSceneEditor3D::ShowRegionTextures()
{
	//if(m_selectRegions.size() == 0)
	//{
	//	m_textureGroup->Show(false);
	//	return;
	//}
	//Phantom::Pixel& last	=	m_selectRegions[m_selectRegions.size() - 1];
	//Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion(last.x, last.y);
	//if(pRegion)
	//{
	//	int texID	=	pRegion->GetLayerTextureID(this->m_nTextureBlendChannel);
	//	Phantom::SceneTextureRuntime* tex	=	this->m_scene3D->GetTextureRuntime(texID);
	//	if(tex)
	//	{
	//		this->m_textureGroup->Show(true);
	//		//SetPropFloat(this->m_textureHighlight, tex->pRegion->GetLayerHighlight(this->m_nTextureBlendChannel));
	//		SetPropFloat(this->m_textureMipmapBias, tex->mipMapLodBias);//pRegion->GetMipmapBias(this->m_nTextureBlendChannel));
	//		//SetPropFloat(this->m_texturePositionX, pRegion->GetUVPositionX(m_nTextureBlendChannel));
	//		//SetPropFloat(this->m_texturePositionY, pRegion->GetUVPositionY(m_nTextureBlendChannel));
	//		SetPropFloat(this->m_textureScalingX, tex->uvTile.x);//pRegion->GetUVScaleX(0));
	//		//SetPropFloat(this->m_textureScalingY, pRegion->GetUVScaleY(0));
	//		//SetPropFloat(this->m_textureRotationZ, pRegion->GetUVRotation(0));
	//	}
	//	else
	//		this->m_textureGroup->Show(false);
	//}
	//else
	//	this->m_textureGroup->Show(false);
}

VOID						GCSceneEditor3D::ShowRegions()
{
	//if(m_selectRegions.size() == 0)
	//{
	//	this->m_regionGroup->Show(false);
	//	this->m_textureGroup->Show(false);
	//	return;
	//}
	//Phantom::Pixel& last	=	m_selectRegions[m_selectRegions.size() - 1];
	//Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion(last.x, last.y);
	//if(pRegion)
	//{
	//	this->m_regionGroup->Show(true);
	//	SetPropBool(this->m_regionVisible, pRegion->m_bVisible);
	//	//m_regionWater->RemoveAllOptions();
	//	//m_regionWater->AddOption(L"");
	//	//wchar_t buf[128];
	//	//for(int i=0;i<m_scene3D->GetWaterDataPtr()->GetDataCount();i++)
	//	//{
	//	//	swprintf(buf, L"%s[%d]", W(m_scene3D->GetWaterDataPtr()->GetDataPtr(i)->name.t).buf, m_scene3D->GetWaterDataPtr()->GetDataPtr(i)->id);
	//	//	m_regionWater->AddOption(buf);
	//	//}
	//	//const SceneWaterData* water	=	pRegion->GetWaterData();
	//	//if(water)
	//	//{
	//	//	swprintf(buf, L"%s[%d]", W(water->name.t).buf, water->id);
	//	//	SetPropText(m_regionWater, buf);
	//	//}
	//	//else
	//	//	SetPropText(m_regionWater, L"");
	//	//
	//	ShowRegionTextures();
	//}
	//else
	//{
	//	this->m_regionGroup->Show(false);
	//	this->m_textureGroup->Show(false);
	//}
}

VOID						GCSceneEditor3D::OnSelectLayer(INT nLayer)
{
	m_nTextureBlendChannel			=	nLayer;
	ShowRegions();
}

void						GCSceneEditor3D::OnSelectImage(ImageItem* pItem, INT nParameter)
{
	this->BeginUndo();
	FileName fname = "";
	FileName vold = "";
	if(m_scene3D->GetLayerTexture(nParameter))
		vold = m_scene3D->GetLayerTexture(nParameter)->GetFileName().t;
	if(!(pItem->filePath == "Empty"))
		fname = pItem->filePath;
	else
		fname = "";
	m_scene3D->SetLayerTexture(nParameter, fname);
	UndoRedoBuffer* buff = AddUndoRedo(SCENE3D_SELECT_LAYER, (ProcUndoRedo)0);
	buff->m_bEnabled = true;
	buff->m_id	=	nParameter;
	(*buff) << vold << fname;
	//
	RefreshLayerImages();
}

VOID	GCSceneEditor3D::RefreshLayerImages()
{
	for(int i=0;i<MAX_LAYER_TEXTURE;i++)
	{
		Phantom::SceneTextureRuntime* pRuntime	=	m_scene3D->GetLayerTextureData(i);
		if(pRuntime && pRuntime->fileName.t[0])
			this->m_layerImages.SetImageFile(i, pRuntime->fileName, pRuntime->bMoveEnable);
		else
			m_layerImages.SetImageFile(i, "Empty", true);
	}
	OnSelectLayer(m_nTextureBlendChannel);
	m_layerImages.Invalidate();
}

VOID						GCSceneEditor3D::AutoAddRegion()
{
	//int regionX			=	(int)(m_vHeightCenter.pos.x /  m_scene3D->GetRegionSize());
	//int regionY			=	(int)(m_vHeightCenter.pos.y /  m_scene3D->GetRegionSize());
	//Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion(regionX, regionY);
	////Phantom::Camera* c		=	Phantom::GetCameraPtr();
	////Phantom::Pixel pixel	=	c->getCursorPos();
	////Phantom::float3 orig, dir;
	////c->Get3DFrom2D(orig, dir, pixel);
	////Phantom::SceneBox* pRegion	=	m_scene3D->IntersectSceneRegionPtr(orig, dir);
	//////
	//if(!pRegion)
	//	return;
	////if(!pRegion->IsCreated())
	////{
	////	pRegion->Create();
	////	return;
	////}
	//Phantom::Pixel pos(pRegion->GetRegionIndexX(), pRegion->GetRegionIndexY());
	//BOOL bControl		=	GetSelectKeyboard();
	//if(bControl)
	//{
	//	bool find	=	false;
	//	for(int i=0;i<m_selectRegions.size();i++)
	//	{
	//		if(m_selectRegions[i].x == pos.x && m_selectRegions[i].y == pos.y)
	//		{
	//			m_selectRegions.erase(m_selectRegions.begin() + i);
	//			find	=	true;
	//			break;
	//		}
	//	}
	//	if(!find)
	//	{
	//		this->m_selectRegions.push_back(pos);
	//	}
	//}
	//else
	//{
	//	if(m_selectRegions.size() == 1 && m_selectRegions[0].x == pos.x && m_selectRegions[0].y == pos.y)
	//	{
	//	}
	//	else
	//	{
	//		m_selectRegions.clear();
	//		this->m_selectRegions.push_back(pos);
	//	}
	//}
	//if(IsTextureEditorMode())
	//	RefreshLayerImages();
	//if(m_editorMode > 0)
	//	this->SetEditorMode(this->m_editorMode);
}

bool						GCSceneEditor3D::OnWindowProcHeight(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam)
{
	static POINT s_downPoint = {0,0};
	switch(uMsg)
	{
	case WM_LBUTTONUP:
		{
			POINT pos;
			GetCursorPos(&pos);
			m_bDoEvent		=	false;
			m_bMouseMove	=	false;
			m_fCalculateTime	=	0;
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
				{
					for(int i=0;i<m_nHeightPointCount;i++)
					{
						m_scene3D->RebuildRegionFaces(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y, true);
						if(m_editorMode==EditorMode_HeightGridEnable)
							m_scene3D->RebuildMoveGrids(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
						if(m_editorMode==EditorMode_RegionSet)
							m_scene3D->RebuildRegionSets(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y, this->m_regionIndex);
						//Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
					}
					//m_vRefreshHeightPoints
					BOOL bSet	=	false;
					this->BeginUndo();
					for(int i=0;i<m_nEditRegionCount;i++)
					{
						Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion(m_vEditRegions[i].x, m_vEditRegions[i].y);
						if(m_savedData[i]){
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
								{
									UndoRedoBuffer* buff = AddUndoRedo(SCENE3D_HEIGHT_SET, (ProcUndoRedo)0);
									buff->m_bEnabled = true;
									buff->m_id	=	m_vEditRegions[i].x;
									buff->m_id2	=	m_vEditRegions[i].y;
									SaveRegionV* temp = new SaveRegionV();
									this->SaveRegionD(*temp, pRegion);
									(*buff) << (*m_savedData[i]) << (*temp);
								}
								break;
							}
						}
						if(m_savedDataT[i]){
							switch(m_editorMode)
							{
							case EditorMode_TextureAdd:
							case EditorMode_TextureSet:
							case EditorMode_TextureAvg:
							case EditorMode_TextureUVMoving:
							case EditorMode_TextureUVScaling:
							case EditorMode_TextureUVRotation:
								{
									UndoRedoBuffer* buff = AddUndoRedo(SCENE3D_TEXTURE_SET, (ProcUndoRedo)0);
									buff->m_bEnabled = true;
									buff->m_id	=	m_vEditRegions[i].x;
									buff->m_id2	=	m_vEditRegions[i].y;
									SaveRegionT* temp = new SaveRegionT();
									this->SaveRegionTs(*temp, pRegion);
									(*buff) << (*m_savedDataT[i]) << (*temp);
								}
								break;
							};
						}
						//if(pRegion && pRegion->GetWaterData())
						{
							//m_scene3D->RecalcWaterHeightAlpha(m_vEditRegions[i].x, m_vEditRegions[i].y);
							bSet	=	true;
						}
						m_scene3D->RecalcRegionGrassHeight(m_vEditRegions[i].x, m_vEditRegions[i].y);
					}
					//if(bSet)
					//	m_scene3D->RefreshBlendDatas();
				}
				break;
			}
			m_nEditRegionCount	=	0;
			if((s_downPoint.x == pos.x && s_downPoint.y == pos.y) && !this->m_editorProperties.IsWindowVisible() && m_editorMode > 0)
			{
				this->SetEditorMode(this->m_editorMode);
			}
			return true;
		}
		break;
	case WM_MBUTTONDOWN:
		{
			AutoAddRegion();
		}
		break;
	case WM_LBUTTONDOWN:
		{
			GetCursorPos(&s_downPoint);
			//
			m_nEditRegionCount	=	0;
			m_fCalculateTime	=	0;
			m_bDoEvent		=	false;
			m_bMouseMove	=	false;
			//AutoAddRegion();
			//if(this->m_editorMode == EditorMode_AddGrass || this->m_editorMode == EditorMode_RemoveGrass)
			//{
			//	ISceneGrassTexture* grass	=	this->m_scene3D->GetGrassDataPtr()->GetDataPtr(this->m_selectGrassPtrID);
			//	if(!grass)
			//	{
			//		GetMainFrame()->MessageBox(Language(L"需要选择一个草之后才能操作"));
			//		return false;
			//	}
			//}
			m_bMouseMove		=	true;
			this->m_bDoEvent	=	true;
			//
			break;
		}
		break;
	case WM_RBUTTONUP:
		{
		}
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		{
			//if(m_fHeightRadius < MAX_UNDO_RANGE)
				//AddUndoRedoMode(PointEditorUndoRedo::HEMODE_POINT_BUFFER);
		}
		break;
	case WM_MOUSEMOVE:
		m_bMouseMove		=	true;
		return m_bDoEvent;
	case WM_MOUSEWHEEL:
		if(IsKeyDown(VK_CONTROL))
		{
			int zDelta = (short) HIWORD(wParam) / WHEEL_DELTA;
			if(zDelta > 0)
			{
				if(m_fHeightRadius < To3DValue(35.0f))
					this->m_fHeightRadius	*= 1.1f;
				SetPropFloat(m_editorRadius, m_fHeightRadius);
			}
			else
			{
				if(m_fHeightRadius > To3DValue(0.3f))
					this->m_fHeightRadius	*= 0.9f;
				SetPropFloat(m_editorRadius, m_fHeightRadius);
			}
			return true;
		}
		break;
	}
	return false;
}

BOOL	GCSceneEditor3D::IsDoEvent()
{
	return (::GetFocus() == GetDeviceWindow() && m_bDoEvent && HIBYTE(GetKeyState(VK_LBUTTON)));
}



extern	void	OnEditorMoveEnable(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info);
extern	void	OnEditorAddTexture(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info);
extern	void	OnEditorSetTexture(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info);
extern	void	OnEditorBrightnessTexture(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info);
extern	void	OnEditorWaterAlphaTexture(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info);
extern	void	OnEditorAvgTexture(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info, float& valRet);

VOID						GCSceneEditor3D::OnFrameMoveHeight()
{
	SceneEditorInfo& info	=	this->m_editorInfo[this->m_editorMode];
	m_scene3D->SetNormalBuildVar(info.fNormalVar);
	Phantom::Camera* c		=	Phantom::GetCameraPtr();
	Phantom::Pixel pixel;
	::GetCursorPos((LPPOINT)&pixel);
	ScreenToClient((HWND)GetDeviceWindow(), (LPPOINT)&pixel);

	Phantom::float3 orig, dir;
	c->Get3DFrom2D(orig, dir, pixel);
	Phantom::PickPoint data;
	m_nTextureBlendChannel	=	this->m_layerImages.m_imageViewSelect;
	if(!m_scene3D->IntersectTerrain(orig, dir, data))
	{
		data.pos = orig - dir * (orig.z/dir.z);
		data.normal.setxyz(0,0,1);
	}
	{
		m_vHeightCenter	=	data;
		if(IsDoEvent())
		{
			m_fCalculateTime	-=	Phantom::g_manager.GetElapsedTime();
			if(this->IsTextureEditorMode())
			{
				if(m_fCalculateTime >= 0.0f)
					return;
				m_fCalculateTime	=	0.05f;
			}
			switch(m_editorMode)
			{
			case EditorMode_HeightAdd:
				{
					LoopHeightGrids(OnEditorAddHeight);
					for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RefreshEditVertices(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
					for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RebuildRegionFaces(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
				}
				break;
			case EditorMode_HeightAvg:
				{
					LoopHeightGridsSaved(OnEditorAverage);
					for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RefreshEditVertices(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
					for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RebuildRegionFaces(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
				}
				break;
			case EditorMode_HeightAlign:
				{
					LoopHeightGrids(OnEditorAverageAlign);
					for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RefreshEditVertices(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
					for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RebuildRegionFaces(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
				}
				break;
			case EditorMode_HeightSet:
				{
					if(m_bMouseMove)
					{
						LoopHeightGrids(OnEditorSetHeight);
						for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RefreshEditVertices(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
						for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RebuildRegionFaces(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
						m_bMouseMove	=	false;
					}
				}
				break;
			case EditorMode_HeightGridVisible:
				{
					LoopHeightGrids(OnEditorShowGrid);
					for(int i=0;i<this->m_nHeightPointCount;i++)
						m_scene3D->RebuildRegionFaces(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
				}
				break;
			case EditorMode_AddGrass:
				{
					if(m_fCalculateTime >= 0.0f)
						return;
					if(GetSelectKeyboard())
						LoopHeightGrids(OnEditorDelGrass);
					else
						LoopHeightGrids(OnEditorAddGrass);
					m_fCalculateTime	=	0.1f;
				}
				break;
			case EditorMode_RemoveGrass:
				{
					if(m_fCalculateTime >= 0.0f)
						return;
					if(GetSelectKeyboard())
						LoopHeightGrids(OnEditorAddGrass);
					else
						LoopHeightGrids(OnEditorDelGrass);
					m_fCalculateTime	=	0.1f;
				}
				break;

			//case HEIGHT_MODE_ADD_GRASS:
			//	{
			//		if(!bGrass)break;
			//		this->m_grassColor.from(m_property.GetColor("GrassColor"), 1.0f);
			//		GrassTextureInfo* tex	=	m_scene3D->GetGrassTexturePtr(m_textureIdx);
			//		if(tex)
			//			m_scene3D->AddGrass(m_vHeightCenter.pos.x, m_vHeightCenter.pos.y, tex, m_grassColor);
			//		for(int i=0;i<this->m_nHeightPointCount;i++)
			//			if(m_scene3D->GetSceneRegionPtr(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y))
			//				m_scene3D->GetSceneRegionPtr(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y)->SetModifyed(true);
			//	}
			//	break;
			//case HEIGHT_MODE_SET_GRASS_TEX:
			//	{
			//		if(!bGrass)break;
			//		LoopHeightGrids(OnEditorSetGrassTexture);
			//		for(int i=0;i<this->m_nHeightPointCount;i++)
			//			if(m_scene3D->GetSceneRegionPtr(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y))
			//				m_scene3D->GetSceneRegionPtr(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y)->SetModifyed(true);
			//	}
			//	break;
			//case HEIGHT_MODE_SET_GRASS_COLOR:
			//	{
			//		if(!bGrass)break;
			//		this->m_grassColor.from(m_property.GetColor("GrassColor"), 1.0f);
			//		LoopHeightGrids(OnEditorSetGrassColor);
			//		for(int i=0;i<this->m_nHeightPointCount;i++)
			//			if(m_scene3D->GetSceneRegionPtr(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y))
			//				m_scene3D->GetSceneRegionPtr(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y)->SetModifyed(true);
			//	}
			//	break;
			//case HEIGHT_MODE_SET_GRASS_HEIGHT:
			//	{
			//		if(!bGrass)break;
			//		LoopHeightGrids(OnEditorSetGrassHeight);
			//		for(int i=0;i<this->m_nHeightPointCount;i++)
			//			if(m_scene3D->GetSceneRegionPtr(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y))
			//				m_scene3D->GetSceneRegionPtr(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y)->SetModifyed(true);
			//	}
			//	break;
			//case HEIGHT_MODE_DEL_ALL_GRASS:
			//	{
			//		if(!bGrass)break;
			//		LoopHeightGrids(OnEditorDelAllGrass);
			//		for(int i=0;i<this->m_nHeightPointCount;i++)
			//			if(m_scene3D->GetSceneRegionPtr(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y))
			//				m_scene3D->GetSceneRegionPtr(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y)->SetModifyed(true);
			//	}
			//	break;
			case EditorMode_HeightMapSet:
				{
					//if(!m_imageBits.IsNull())
					//{
					//	LoopHeightGrids(OnEditorFromHeightMapSet);
					//	for(int i=0;i<this->m_nHeightPointCount;i++)
					//		m_scene3D->RefreshEditVertices(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
					//	for(int i=0;i<this->m_nHeightPointCount;i++)
					//		m_scene3D->RebuildRegionFaces(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
					//}
				}
				break;
				//
			case EditorMode_HeightGridEnable:
				{
					LoopMoveEnableRange(OnEditorMoveEnable);
					//if(m_scene3D->IsScene25d())
						for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RebuildMoveGrids(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
				}
				break;
			case EditorMode_RegionSet:
				{
					LoopMoveEnableRange(OnEditorRegionSet);
					//if(m_scene3D->IsScene25d())
						for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RebuildRegionSets(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y, m_regionIndex);
				}
				break;
			case EditorMode_TextureAdd:
				{
					LoopTextureRange(OnEditorAddTexture);
					//if(m_scene3D->IsScene25d())
						for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RebuildRegionFaces(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
					//else
					//	m_scene3D->RefreshBlendDatas();
				}
				break;
			case EditorMode_TextureSet:
				{
					LoopTextureRange(OnEditorSetTexture);
					//if(m_scene3D->IsScene25d())
						for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RebuildRegionFaces(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
					//else
					//	m_scene3D->RefreshBlendDatas();
				}
				break;
			case EditorMode_TextureAvg:
				{
					LoopTextureSavedRange(OnEditorAvgTexture);
					//if(m_scene3D->IsScene25d())
						for(int i=0;i<this->m_nHeightPointCount;i++)m_scene3D->RebuildRegionFaces(m_vRefreshHeightPoints[i].x, m_vRefreshHeightPoints[i].y);
					//else
					//	m_scene3D->RefreshBlendDatas();
				}
				break;
			//case EDIT_BRIGHTNESS_MODE:
			//	{
			//		LoopTextureRange(OnEditorBrightnessTexture);
			//		m_scene3D->RefreshBlendDatas();
			//	}
			//	break;
			//case EDIT_WATER_ALPHA_MODE:
			//	{
			//		LoopTextureRange(OnEditorWaterAlphaTexture);
			//		m_scene3D->RefreshBlendDatas();
			//	}
			//	break;
			}
		}
	}
}


void	OnEditorRenderHeightPoint(GCSceneEditor3D* dlg, INT nGridX, INT nGridY, const Phantom::float3& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	//static Phantom::color4 c(1,1,0,1);
	//Phantom::color4 cc	=	c;
	//cc.r *= dlg->GetHeightAmount(dir, dlg->m_fHeightRadius, info.fPower);
	//cc.a		=	1.0f;
	//float fL	=	dlg->m_scene3D->GetGridSize() * 0.015f;
	//Phantom::float3 pos	=	gridMiddle;
	//dlg->m_scene3D->GetHeight(pos, pos.z);
	//GetRenderHelperPtr()->drawBillboard(pos, rect_f(-fL, fL * 32.0f, fL, 0.0f), cc);
}

//extern	void	OnEditorRenderTexturePoint(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info);

VOID	GCSceneEditor3D::RenderCircle(const Phantom::float3& vCenter, float fSize, unsigned int color)
{
	static int smooth = 0;
	static Phantom::float3 vertices[70];
	unsigned int colors[70];
	if(smooth==0){
		smooth = 64;
		vertices[0].setxyz(0,0,0);
		for(int i=0;i<=smooth;i++)
		{
			float fR			=	(float)i / (float)smooth * PI * 2.0f;
			Phantom::float3 pos(cosf(fR), sinf(fR), 0);
			//m_scene3D->GetHeight(pos, pos.z);
			pos.z += 0.1f;
			vertices[i+1]		=	pos;
		}
	}
	Phantom::matrix4x4 mat;
	Phantom::matrixScaling(&mat, fSize, fSize, 1);
	mat.SetPosition(vCenter);
	Phantom::g_manager.SetRenderMode(Phantom::RenderMode_Color);
	Phantom::g_manager.SetWorldMatrix(&mat);
	Phantom::g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
	for(int i=0;i<=(smooth+1);i++)
		colors[i] = color;
	//Phantom::g_manager.GetCurrProgram()->SetDiffuse(Phantom::color4(color));
	Phantom::g_manager->DrawNoIndexed(Phantom::g_manager.GetCurrProgram(), Phantom::DrawVInfos(smooth + 2, vertices, colors), smooth + 2, Phantom::DrawMode_TriangleFan);

}
VOID	GCSceneEditor3D::DrawCircle(const Phantom::float3& vCenter, float fRadius, unsigned int color, int smooth)
{
	Phantom::float3 vertices[80];
	unsigned int colors[80];
	assert(smooth < 80);
	for(int i=0;i<=smooth;i++)
	{
		float fR			=	(float)i / (float)smooth * PI * 2.0f;
		Phantom::float3 pos(cosf(fR) * fRadius + vCenter.x, sinf(fR) * fRadius + vCenter.y, vCenter.z);
		//m_scene3D->GetHeight(pos, pos.z);
		pos.z += 0.1f;
		vertices[i]		=	pos;
		colors[i]		=	color;
	}
	Phantom::g_manager.SetRenderMode(Phantom::RenderMode_Color);
	Phantom::g_manager->DrawNoIndexed(Phantom::g_manager.GetCurrProgram(), Phantom::DrawVInfos(smooth + 1, vertices, colors), smooth + 1, Phantom::DrawMode_LineStrip);
}
VOID	GCSceneEditor3D::DrawOmni(const Phantom::float3& pos, unsigned int color, float fSize)
{
	Phantom::float3 vertices[15];
	unsigned int colors[15];
	for(int i=0;i<15;i++)
	{
		colors[i] = color;
		vertices[i] = pos;
	}
	Phantom::float3 vx(fSize,0,0),vy(0,fSize,0),vz(0,0,fSize);
	int count = 0;
	vertices[count++] += vz;
	vertices[count++] += vx;
	vertices[count++] -= vz;
	vertices[count++] -= vx;
	vertices[count++] += vy;
	vertices[count++] += vx;
	vertices[count++] -= vy;
	vertices[count++] -= vx;
	vertices[count++] += vz;
	vertices[count++] -= vy;
	vertices[count++] -= vz;
	vertices[count++] += vy;
	vertices[count++] += vz;
	Phantom::g_manager.SetRenderMode(Phantom::RenderMode_Color);
	Phantom::g_manager->DrawNoIndexed(Phantom::g_manager.GetCurrProgram(), Phantom::DrawVInfos(count, vertices, colors), count, Phantom::DrawMode_LineStrip);
}
VOID	GCSceneEditor3D::RenderRect(const Phantom::float3& vCenter, float fRadius, unsigned int color, int smooth)
{
	Phantom::float3 vertices[256];
	unsigned int colors[256];
	float fRadius2 = fRadius / (float)(smooth);
	for(int i=-smooth;i<=smooth;i++)
	{
		Phantom::float3 pos((float)i * fRadius2 + vCenter.x, fRadius + vCenter.y, 0);
		m_scene3D->GetHeight(pos, pos.z);
		pos.z += 0.1f;
		vertices[i+smooth]		=	pos;
		colors[i+smooth]		=	color;
	}
	Phantom::g_manager.SetRenderMode(Phantom::RenderMode_Color);
	Phantom::g_manager->DrawNoIndexed(Phantom::g_manager.GetCurrProgram(), Phantom::DrawVInfos(smooth*2 + 1, vertices, colors), smooth*2 + 1, Phantom::DrawMode_LineStrip);
	//r.Render->drawNoIndexed(Enum::DRAW_TYPE_LINESTRIP, vertices, smooth*2 + 1, sizeof(PPointDiffuse), PPointDiffuse::c_fvf);
	for(int i=-smooth;i<=smooth;i++)
	{
		Phantom::float3 pos((float)i * fRadius2 + vCenter.x, vCenter.y-fRadius, 0);
		m_scene3D->GetHeight(pos, pos.z);
		pos.z += 0.1f;
		vertices[i+smooth]		=	pos;
		colors[i+smooth]		=	color;
	}
	Phantom::g_manager->DrawNoIndexed(Phantom::g_manager.GetCurrProgram(), Phantom::DrawVInfos(smooth*2 + 1, vertices, colors), smooth*2 + 1, Phantom::DrawMode_LineStrip);
	//r.Render->drawNoIndexed(Enum::DRAW_TYPE_LINESTRIP, vertices, smooth*2 + 1, sizeof(PPointDiffuse), PPointDiffuse::c_fvf);
	for(int i=-smooth;i<=smooth;i++)
	{
		Phantom::float3 pos(fRadius + vCenter.x, vCenter.y+(float)i*fRadius2, 0);
		m_scene3D->GetHeight(pos, pos.z);
		pos.z += 0.1f;
		vertices[i+smooth]		=	pos;
		colors[i+smooth]		=	color;
	}
	Phantom::g_manager->DrawNoIndexed(Phantom::g_manager.GetCurrProgram(), Phantom::DrawVInfos(smooth*2 + 1, vertices, colors), smooth*2 + 1, Phantom::DrawMode_LineStrip);
	//r.Render->drawNoIndexed(Enum::DRAW_TYPE_LINESTRIP, vertices, smooth*2 + 1, sizeof(PPointDiffuse), PPointDiffuse::c_fvf);
	for(int i=-smooth;i<=smooth;i++)
	{
		Phantom::float3 pos(vCenter.x - fRadius, vCenter.y+fRadius2*(float)i, 0);
		m_scene3D->GetHeight(pos, pos.z);
		pos.z += 0.1f;
		vertices[i+smooth]	=	pos;
		colors[i+smooth]	=	color;
	}
	Phantom::g_manager->DrawNoIndexed(Phantom::g_manager.GetCurrProgram(), Phantom::DrawVInfos(smooth*2 + 1, vertices, colors), smooth*2 + 1, Phantom::DrawMode_LineStrip);
	//r.Render->drawNoIndexed(Enum::DRAW_TYPE_LINESTRIP, vertices, smooth*2 + 1, sizeof(PPointDiffuse), PPointDiffuse::c_fvf);
}
VOID	GCSceneEditor3D::RenderLine(const Phantom::float3& from, const Phantom::float3& to, unsigned int color, float fGridSize2)
{
	Phantom::float3 vertices[2];
	unsigned int colors[2];
	vertices[0]		=	from;
	colors[0]		=	color;
	vertices[1]		=	to;
	colors[1]		=	color;
	for(int i=0;i<2;i++)
	{
		vertices[i].z += 1.0f;
		vertices[i].x -= fGridSize2;
		vertices[i].y -= fGridSize2;
		m_scene3D->GetHeight(vertices[i], vertices[i].z);
		vertices[i].z += 0.1f;
	}
	Phantom::g_manager.SetRenderMode(Phantom::RenderMode_Color);
	Phantom::g_manager->DrawNoIndexed(Phantom::g_manager.GetCurrProgram(), Phantom::DrawVInfos(2, vertices, colors), 2, Phantom::DrawMode_Lines);
	//r.Render->drawNoIndexed(Enum::DRAW_TYPE_LINELIST, vertices, 2, sizeof(PPointDiffuse), PPointDiffuse::c_fvf);
}
VOID						GCSceneEditor3D::OnRenderingHeight()
{
	Phantom::matrix4x4 mat;
	mat.identity();
	Phantom::g_manager.SetWorldMatrix(&mat);
	Phantom::g_manager->SetAlphaMode(Phantom::AlphaMode_None);
	//Phantom::g_manager->SetZWriteEnable(false);
	//for(int i=0;i<this->m_selectRegions.size();i++)
	//{
	//	Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion(m_selectRegions[i].x, m_selectRegions[i].y);
	//	if(pRegion)
	//	{
	//		Phantom::AABox box		=	pRegion->GetRegionBox();
	//		box._max.z				+=	To3DValue(100.0f);
	//		Phantom::g_manager->SetCullMode(Phantom::CullMode_Double);
	//		unsigned int colors[32];
	//		for(int k=0;k<32;k++)
	//			colors[k] = 0x3fffff00;
	//		Phantom::g_manager.DrawBox(box, colors);
	//		Phantom::g_manager->SetCullMode(Phantom::CullMode_CCW);
	//	}
	//}
	//
	if(/*this->m_scene3D->IsScene25d() &&*/IsTextureEditorMode())
	{
		float fGridS = m_scene3D->GetGridSize()/2.0;
		if(m_editorMode == EditorMode_HeightGridEnable)
			fGridS = 0;
		else if(m_editorMode == EditorMode_RegionSet)
			fGridS = 0;
		else if (m_editorMode == EditorMode_TextureSet)
			fGridS = -m_scene3D->GetGridSize();
		int nCount = (int)(m_fHeightRadius / m_scene3D->GetGridSize());
		int gx = (int)((this->m_vHeightCenter.pos.x + fGridS) / m_scene3D->GetGridSize()) - nCount/2;
		int gy = (int)((this->m_vHeightCenter.pos.y + fGridS) / m_scene3D->GetGridSize()) - nCount/2;
		for(int x=0;x<nCount;x++)
		{
			Phantom::float3 from,to;
			m_scene3D->GetVertexPosition(gx+x,gy, from);
			m_scene3D->GetVertexPosition(gx+x+1,gy, to);
			RenderLine(from, to, 0xffffff00, fGridS);
			m_scene3D->GetVertexPosition(gx+x,gy+nCount, from);
			m_scene3D->GetVertexPosition(gx+x+1,gy+nCount, to);
			RenderLine(from, to, 0xffffff00, fGridS);
		}
		for(int y=0;y<nCount;y++)
		{
			Phantom::float3 from,to;
			m_scene3D->GetVertexPosition(gx,gy+y, from);
			m_scene3D->GetVertexPosition(gx,gy+y+1, to);
			RenderLine(from, to, 0xffffff00, fGridS);
			m_scene3D->GetVertexPosition(gx+nCount,gy+y, from);
			m_scene3D->GetVertexPosition(gx+nCount,gy+y+1, to);
			RenderLine(from, to, 0xffffff00, fGridS);
		}
	}
	else
	{
		if(this->m_bBrushMode == 0){
			const int maxCircleLevel	=	5;
			for(int i=0;i<=maxCircleLevel;i++)
			{
				Phantom::color4 c(0,1,0,1);
				Phantom::color4 cd(0.3f, .3, .0, .3);
				c.lerp(c, cd, (float)i / (float)maxCircleLevel);
				DrawCircle(this->m_vHeightCenter.pos, this->m_fHeightRadius * (float)(i+1) / (float)maxCircleLevel, c);
			}
		}
		else if(this->m_bBrushMode == 1){
			const int maxCircleLevel	=	5;
			for(int i=0;i<=maxCircleLevel;i++)
			{
				Phantom::color4 c(0,1,0,1);
				Phantom::color4 cd(0.3f, .3, .0, .3);
				c.lerp(c, cd, (float)i / (float)maxCircleLevel);
				RenderRect(this->m_vHeightCenter.pos, this->m_fHeightRadius * (float)(i+1) / (float)maxCircleLevel, c);
			}
		}
		//LoopTextureGrids(OnEditorRenderTexturePoint);
	}
	//Phantom::g_manager->SetZWriteEnable(true);
	Phantom::g_manager->SetAlphaMode(Phantom::AlphaMode_None);
}

