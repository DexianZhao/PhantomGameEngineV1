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


INT	GetBlendAlphaChannel(INT nBlendChannel)
{
	switch(nBlendChannel)
	{
	case 0:
		return -1;
	case 1:
		return 0;
	case 2:
		return 1;
	case 3:
		return 2;
	case 4:
		return 3;
	case 5:
		return 4;
	case 6:
		return 5;
	case 7:
		return 6;
	case 8:
		return -1;
	}
	return -1;
}
//
//void	OnEditorRenderTexturePoint(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
//{
//	static Phantom::color4 c(1,1,0,1);
//	//if(dlg->m_editorMode == EDIT_WATER_ALPHA_MODE)
//		//dlg->m_scene3D->GetBlendValue(nBlendX, nBlendY, WATER_BLEND_CHANNEL, c.r);
//	//else
//	dlg->m_scene3D->GetBlendValue(nBlendX, nBlendY, GetBlendAlphaChannel(dlg->m_nTextureBlendChannel), c.r);
//	c.g	=	c.b	=	c.r;
//	c		=	c * dlg->GetHeightAmount(dir, dlg->m_fHeightRadius, info.fPower);
//	c.a		=	1.0f;
//	//
//	float fL	=	dlg->m_scene3D->GetGridSize() * 0.015f;
//	Phantom::float3 pos	=	gridMiddle;
//	dlg->m_scene3D->GetHeight(pos, pos.z);
//	GetRenderHelperPtr()->drawBillboard(pos, rect_f(-fL, fL * 32.0f, fL, 0.0f), c);
//}

void	OnEditorMoveEnable(GCSceneEditor3D* dlg, INT nMX, INT nMY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	Phantom::GMoveData* pRuntime	=	dlg->m_scene3D->GetMoveAttribute(nMX, nMY);
	if(!pRuntime)return;
	BOOL bEnable	=	true;
	if(IsKeyDown(VK_CONTROL))
		bEnable		=	false;
	dlg->m_scene3D->SetMoveGridEnable(nMX, nMY, bEnable);
	//dlg->m_scene3D->GetSceneRegion(nMX/dlg->m_scene3D->GetRegionWidthGrids()/MOVE_GRID_SIZE, nMY/dlg->m_scene3D->GetRegionWidthGrids()/MOVE_GRID_SIZE)->SetModifyed(true);
	dlg->SetModifyed();
}
void	OnEditorAddTexture(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	//if(dlg->m_scene3D->IsScene25d())
	//{
		dlg->m_scene3D->SetGridAttrIndex(nBlendX, nBlendY, dlg->m_nTextureBlendChannel);
		dlg->SetModifyed();
	//	return;
	//}
	//float fElapsedTime				=	Phantom::g_manager.GetElapsedTime();
	//float fBlend			=	0;
	////if(dlg->m_bUndoBuffer && dlg->m_fHeightRadius < MAX_UNDO_RANGE)
	////	dlg->AddUndoAlpha( nBlendX, nBlendY , fBlend );
	//float fAddValue			=	dlg->GetHeightAmount(dir, dlg->m_fHeightRadius, info.fPower) * info.fVar * fElapsedTime;
	//if(GetSelectKeyboard)
	//	fAddValue			=	-fAddValue;
	////
	////if(dlg->m_randomMode->GetCheckBoxPtr()->GetItemValue())
	////	fAddValue			*=	(0.5f - RandomF(info.fMinValue, info.fMaxValue));
	//if(dlg->m_scene3D->GetBlendValue(nBlendX, nBlendY, GetBlendAlphaChannel(dlg->m_nTextureBlendChannel), fBlend))
	//{
	//	fBlend					+=	fAddValue;
	//	fBlend					=	min(fBlend, info.fMaxValue);
	//	fBlend					=	max(info.fMinValue, fBlend);
	//	dlg->m_scene3D->SetBlendValue(nBlendX, nBlendY, GetBlendAlphaChannel(dlg->m_nTextureBlendChannel), fBlend);
	//}
	//for(int i=dlg->m_nTextureBlendChannel + 1;i<MAX_DETAIL_TEXTURE;i++)
	//{
	//	int cn	=	GetBlendAlphaChannel(i);
	//	float r = 0;
	//	Phantom::SceneBox* rg	=	dlg->m_scene3D->GetSceneRegionPtr(Phantom::float3(gridMiddle.x, gridMiddle.y, 0));
	//	if(rg && rg->GetLayerTextureID(i) > 0 && dlg->m_scene3D->GetBlendValue(nBlendX, nBlendY, cn, r))
	//	{
	//		r -= fAddValue;
	//		if(r < 0)r = 0;
	//		dlg->m_scene3D->SetBlendValue(nBlendX, nBlendY, cn, r);
	//	}
	//}
	//dlg->SetModifyed();
}

void	OnEditorSetTexture(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	//if(dlg->m_scene3D->IsScene25d())
	//{
		Phantom::color4 c = dlg->m_editorInfo[dlg->m_editorMode].color;
		if(GetSelectKeyboard)
		{
			c.r = 1 - c.r;
			c.g = 1 - c.g;
			c.b = 1 - c.b;
		}
		dlg->m_scene3D->SetGridAttrColor(nBlendX, nBlendY, c);
		dlg->SetModifyed();
	//	return;
	//}
	//float fElapsedTime				=	Phantom::g_manager.GetElapsedTime();
	//float fBlend			=	0;
	////if(dlg->m_bUndoBuffer && dlg->m_fHeightRadius < MAX_UNDO_RANGE)
	////	dlg->AddUndoAlpha( nBlendX, nBlendY , fBlend );
	////
	//if(dlg->m_bUseBrushTexture)
	//{
	//	if(dlg->m_scene3D->GetBlendValue(nBlendX, nBlendY, GetBlendAlphaChannel(dlg->m_nTextureBlendChannel), fBlend))
	//	{
	//		fBlend					=	info.fVar * dlg->GetHeightAmount(dir, dlg->m_fHeightRadius, info.fPower);
	//		if(info.bEnable)
	//			fBlend			=	RandomF(info.fMinValue, info.fMaxValue);
	//		fBlend					=	min(fBlend, info.fMaxValue);
	//		fBlend					=	max(info.fMinValue, fBlend);
	//		dlg->m_scene3D->SetBlendValue(nBlendX, nBlendY, GetBlendAlphaChannel(dlg->m_nTextureBlendChannel), fBlend);
	//	}
	//}
	//else
	//{
	//	if(dlg->m_scene3D->GetBlendValue(nBlendX, nBlendY, GetBlendAlphaChannel(dlg->m_nTextureBlendChannel), fBlend))
	//	{
	//		fBlend					=	info.fVar;
	//		if(info.bEnable)
	//			fBlend			=	RandomF(info.fMinValue, info.fMaxValue);
	//		fBlend					=	min(fBlend, info.fMaxValue);
	//		fBlend					=	max(info.fMinValue, fBlend);
	//		dlg->m_scene3D->SetBlendValue(nBlendX, nBlendY, GetBlendAlphaChannel(dlg->m_nTextureBlendChannel), fBlend);
	//	}
	//}
	//for(int i=dlg->m_nTextureBlendChannel + 1;i<MAX_DETAIL_TEXTURE;i++)
	//{
	//	int cn	=	GetBlendAlphaChannel(i);
	//	dlg->m_scene3D->SetBlendValue(nBlendX, nBlendY, cn, 0);
	//}
	//dlg->SetModifyed();
}

void	OnEditorBrightnessTexture(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	//float fElapsedTime				=	Phantom::g_manager.GetElapsedTime();
	//float fBlend			=	0;
	//if(!dlg->m_scene3D->GetBlendValue(nBlendX, nBlendY, MAX_LAYER_TEXTURE, fBlend))
	//	return;
	////if(dlg->m_bUndoBuffer && dlg->m_fHeightRadius < MAX_UNDO_RANGE)
	////	dlg->AddUndoAlpha( nBlendX, nBlendY , fBlend );
	////
	//float fAddValue			=	dlg->GetHeightAmount(dir, dlg->m_fHeightRadius, info.fPower) * info.fVar * fElapsedTime;
	//fBlend					+=	fAddValue;
	//fBlend					=	min(fBlend, info.fMaxValue);
	//fBlend					=	max(info.fMinValue, fBlend);
	//dlg->m_scene3D->SetBlendValue(nBlendX, nBlendY, MAX_LAYER_TEXTURE, fBlend);
	//dlg->SetModifyed();
}

void	OnEditorWaterAlphaTexture(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info)
{
	//float fElapsedTime				=	Phantom::g_manager.GetElapsedTime();
	//float fBlend			=	0;
	//if(!dlg->m_scene3D->GetBlendValue(nBlendX, nBlendY, WATER_BLEND_CHANNEL, fBlend))
	//	return;
	////if(dlg->m_bUndoBuffer && dlg->m_fHeightRadius < MAX_UNDO_RANGE)
	////	dlg->AddUndoAlpha( nBlendX, nBlendY , fBlend );
	////
	//float fAddValue			=	dlg->GetHeightAmount(dir, dlg->m_fHeightRadius, info.fPower) * info.fVar * fElapsedTime;
	//fBlend					+=	fAddValue;
	//fBlend					=	min(fBlend, info.fMaxValue);
	//fBlend					=	max(info.fMinValue, fBlend);

	//dlg->m_scene3D->SetBlendValue(nBlendX, nBlendY, WATER_BLEND_CHANNEL, fBlend);
	//dlg->SetModifyed();
}

void	OnEditorAvgTexture(GCSceneEditor3D* dlg, INT nBlendX, INT nBlendY, const Phantom::float2& gridMiddle, const Phantom::float3& dir, SceneEditorInfo& info, float& valRet)
{
	//if(dlg->m_scene3D->IsScene25d())
	//{
		Phantom::color4 c = dlg->m_scene3D->GetGridAttrColor(nBlendX, nBlendY);
		if(!GetSelectKeyboard)
		{
			c.r *= 1.1f;
			c.g *= 1.1f;
			c.b *= 1.1f;
		}
		else
		{
			c.r *= 0.9f;
			c.g *= 0.9f;
			c.b *= 0.9f;
		}
		dlg->m_scene3D->SetGridAttrColor(nBlendX, nBlendY, c);
		dlg->SetModifyed();
	//	return;
	//}
	//float fElapsedTime		=	Phantom::g_manager.GetElapsedTime();
	//float fBlend			=	0;
	//if(!dlg->m_scene3D->GetBlendValue(nBlendX, nBlendY, GetBlendAlphaChannel(dlg->m_nTextureBlendChannel), fBlend))
	//	return;
	//valRet					=	fBlend;
	////if(dlg->m_bUndoBuffer && dlg->m_fHeightRadius < MAX_UNDO_RANGE)
	////	dlg->AddUndoAlpha( nBlendX, nBlendY , fBlend );
	//double r				=	0;
	//int	  nCount			=	0;
	//for(int x=-1;x<=1;x++)
	//{
	//	for(int y=-1;y<=1;y++)
	//	{
	//		float fB;
	//		if(dlg->m_scene3D->GetBlendValue(nBlendX, nBlendY, GetBlendAlphaChannel(dlg->m_nTextureBlendChannel), fB))
	//		{
	//			r			+=	fB * fElapsedTime * 10.0f;
	//			nCount++;
	//		}
	//	}
	//}
	//if(nCount > 0)
	//{
	//	float fNewHeight	=	(r / (float)nCount);
	//	if(IsKeyDown(VK_CONTROL))
	//		valRet				=	fBlend - fNewHeight * dlg->GetHeightAmount(dir, dlg->m_fHeightRadius, info.fPower);
	//	else
	//		valRet				=	fBlend + fNewHeight * dlg->GetHeightAmount(dir, dlg->m_fHeightRadius, info.fPower);
	//	valRet					=	min(valRet, info.fMaxValue);
	//	valRet					=	max(info.fMinValue, valRet);
	//}
	//dlg->SetModifyed();
}



VOID	GCSceneEditor3D::LoopTextureSavedRange(ProTerrainTextureGridsSaved proc)
{
	if(m_nTextureBlendChannel < 1)return;
	//int range	=	(INT)(this->m_fHeightRadius / m_scene3D->GetBlendSize()) + 1;
	//int middleX	=	(int)(m_vHeightCenter.pos.x /  m_scene3D->GetBlendSize());
	//int middleY	=	(int)(m_vHeightCenter.pos.y /  m_scene3D->GetBlendSize());
	//
	//m_nHeightPointCount	=	0;
	//int					saveCount	=	0;
	//GridSaveValue * savePtr	=	(GridSaveValue*)malloc(range * range * 3 * 3 * sizeof(GridSaveValue));
	//for(int x = -range;x <= range;x++)
	//{
	//	for(int y=-range;y<=range;y++)
	//	{
	//		int xBlend	=	x + middleX;
	//		int yBlend	=	y + middleY;
	//		//
	//		Phantom::float2 gridPos;
	//		Phantom::float3 gridPos3;
	//		if(m_scene3D->GetBlendVertex(xBlend, yBlend, gridPos))
	//		{
	//			Phantom::float3 dir	=	Phantom::float3(gridPos.x, gridPos.y, 0) - m_vHeightCenter.pos;
	//			dir.z		=	0;
	//			float dR	=	dir.R();
	//			if(this->m_bBrushMode == 0)
	//			{
	//				if(dR >= this->m_fHeightRadius)
	//					continue;
	//			}
	//			else
	//			{
	//				if(dir.x > m_fHeightRadius || dir.x < -m_fHeightRadius || dir.y > m_fHeightRadius || dir.y < -m_fHeightRadius)
	//					continue;
	//			}
	//			{
	//				for(int xx=-1;xx<=1;xx++)
	//				{
	//					for(int yy=-1;yy<=1;yy++)
	//					{
	//						int xRegion	=	(xBlend + xx);// / m_scene3D->GetRegionBlendSize();
	//						int yRegion	=	(yBlend + yy);// / m_scene3D->GetRegionBlendSize();
	//						bool bfind	=	false;
	//						for(int i=0;i<this->m_nHeightPointCount;i++)
	//						{
	//							if(m_vRefreshHeightPoints[i].x == xRegion && m_vRefreshHeightPoints[i].y == yRegion)
	//							{
	//								bfind	=	true;
	//								break;
	//							}
	//						}
	//						if(!bfind)
	//						{
	//							Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion( xRegion, yRegion );
	//							if(pRegion)
	//							{
	//								m_vRefreshHeightPoints[m_nHeightPointCount].x	=	xRegion;
	//								m_vRefreshHeightPoints[m_nHeightPointCount].y	=	yRegion;
	//								m_nHeightPointCount	++;
	//								pRegion->SetModifyed(true);
	//							}
	//						}
	//						bfind	=	false;
	//						for(int i=0;i<this->m_nEditRegionCount;i++)
	//						{
	//							if(m_vEditRegions[i].x == xRegion && m_vEditRegions[i].y == yRegion)
	//							{
	//								bfind	=	true;
	//								break;
	//							}
	//						}
	//						if(!bfind)
	//						{
	//							Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion( xRegion, yRegion );
	//							if(pRegion)
	//							{
	//								m_vEditRegions[m_nEditRegionCount].x	=	xRegion;
	//								m_vEditRegions[m_nEditRegionCount].y	=	yRegion;
	//								m_nEditRegionCount	++;
	//							}
	//						}
	//					}
	//				}
	//				savePtr[saveCount].gridX = xBlend;
	//				savePtr[saveCount].gridY = yBlend;
	//				savePtr[saveCount].f	=	0;
	//				(*proc)(this, xBlend, yBlend, gridPos, dir, this->m_editorInfo[this->m_editorMode], savePtr[saveCount].f);
	//				saveCount++;
	//				//
	//			}
	//		}
	//		//
	//	}
	//}
	//if(savePtr)
	//{
	//	//for(int i=0;i<saveCount;i++)
	//	//{
	//	//	m_scene3D->SetBlendValue(savePtr[i].gridX, savePtr[i].gridY, GetBlendAlphaChannel(m_nTextureBlendChannel), savePtr[i].f);
	//	//}
	//	free(savePtr);
	//}
}
BOOL	IsRectCross(const Phantom::Rect& rc1, const Phantom::Rect& rc2)
{
	if(rc1.left > rc2.right || rc1.right < rc2.left || rc1.top > rc2.bottom || rc1.bottom < rc2.top)
		return false;
	return true;
}
VOID	GCSceneEditor3D::LoopMoveEnableRange(ProTerrainTextureGrids proc)
{
	if(!m_scene3D->IsEnabled(Phantom::Scene3D::Enable_MoveGridsData))return;
	float fGridS = m_scene3D->GetMoveGridSize()/2.0;
	if(m_editorMode == EditorMode_HeightGridEnable)
		fGridS = 0;
	if(m_editorMode == EditorMode_RegionSet)
		fGridS = 0;
	int nCount = (int)(m_fHeightRadius / m_scene3D->GetMoveGridSize())-1;
	if(nCount<0)nCount = 0;
	int gx = (int)((this->m_vHeightCenter.pos.x + fGridS) / m_scene3D->GetMoveGridSize()) - nCount/2;
	int gy = (int)((this->m_vHeightCenter.pos.y + fGridS) / m_scene3D->GetMoveGridSize()) - nCount/2;
	for(int x=0;x<=nCount;x++)
	{
		for(int y=0;y<=nCount;y++)
		{
			int xBlend = x + gx-1;
			int yBlend = y + gy-1;
			if(m_editorMode == EditorMode_HeightGridEnable||m_editorMode == EditorMode_RegionSet)
			{
				if((nCount%2)==0)
				{
					xBlend++;
					yBlend++;
				}
			}
			int xRegion	=	xBlend / m_scene3D->GetRegionWidthMoveGrids();
			int yRegion	=	yBlend / m_scene3D->GetRegionWidthMoveGrids();
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
				}
			}
			(*proc)(this, xBlend, yBlend, Phantom::float2(0,0), Phantom::float3(0,0,0), this->m_editorInfo[this->m_editorMode]);
		}
	}
}
VOID	GCSceneEditor3D::LoopTextureRange(ProTerrainTextureGrids proc)
{
	if(m_nTextureBlendChannel < 0)return;
	m_nHeightPointCount	=	0;
	//int range	=	(INT)(this->m_fHeightRadius		/	m_scene3D->GetBlendSize()) + 1;
	//int middleX	=	(int)(m_vHeightCenter.pos.x	/	m_scene3D->GetBlendSize());
	//int middleY	=	(int)(m_vHeightCenter.pos.y	/	m_scene3D->GetBlendSize());
	//for(int x = -range;x <= range;x++)
	//{
	//	for(int y=-range;y<=range;y++)
	//	{
	//		int xBlend	=	x + middleX;
	//		int yBlend	=	y + middleY;
	//		//
	//		Phantom::float2 gridPos;
	//		Phantom::float3 gridPos3;
	//		if(m_scene3D->GetBlendVertex(xBlend, yBlend, gridPos))
	//		{
	//			Phantom::float3 dir	=	Phantom::float3(gridPos.x, gridPos.y, 0) - m_vHeightCenter.pos;
	//			dir.z		=	0;
	//			float dR	=	dir.R();
	//			if(this->m_bBrushMode == 0)
	//			{
	//				if(dR >= this->m_fHeightRadius)
	//					continue;
	//			}
	//			else
	//			{
	//				if(dir.x > m_fHeightRadius || dir.x < -m_fHeightRadius || dir.y > m_fHeightRadius || dir.y < -m_fHeightRadius)
	//					continue;
	//			}
	//			{
	//				for(int xx=-1;xx<=1;xx++)
	//				{
	//					for(int yy=-1;yy<=1;yy++)
	//					{
	//						int xRegion	=	(xBlend + xx);// / m_scene3D->GetRegionBlendSize();
	//						int yRegion	=	(yBlend + yy);// / m_scene3D->GetRegionBlendSize();
	//						bool bfind	=	false;
	//						for(int i=0;i<this->m_nHeightPointCount;i++)
	//						{
	//							if(m_vRefreshHeightPoints[i].x == xRegion && m_vRefreshHeightPoints[i].y == yRegion)
	//							{
	//								bfind	=	true;
	//								break;
	//							}
	//						}
	//						if(!bfind)
	//						{
	//							Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion( xRegion, yRegion );
	//							if(pRegion)
	//							{
	//								m_vRefreshHeightPoints[m_nHeightPointCount].x	=	xRegion;
	//								m_vRefreshHeightPoints[m_nHeightPointCount].y	=	yRegion;
	//								m_nHeightPointCount	++;
	//								pRegion->SetModifyed(true);
	//							}
	//						}
	//						bfind	=	false;
	//						for(int i=0;i<this->m_nEditRegionCount;i++)
	//						{
	//							if(m_vEditRegions[i].x == xRegion && m_vEditRegions[i].y == yRegion)
	//							{
	//								bfind	=	true;
	//								break;
	//							}
	//						}
	//						if(!bfind)
	//						{
	//							Phantom::SceneBox* pRegion	=	m_scene3D->GetSceneRegion( xRegion, yRegion );
	//							if(pRegion)
	//							{
	//								m_vEditRegions[m_nEditRegionCount].x	=	xRegion;
	//								m_vEditRegions[m_nEditRegionCount].y	=	yRegion;
	//								m_nEditRegionCount	++;
	//							}
	//						}
	//					}
	//				}
	//				(*proc)(this, xBlend, yBlend, gridPos, dir, this->m_editorInfo[this->m_editorMode]);
	//			}
	//		}
	//	}
	//}
	//if(m_scene3D->IsScene25d())
	//{
		float fGridS = m_scene3D->GetGridSize()/2.0;
		if(m_editorMode == EditorMode_HeightGridEnable)
			fGridS = 0;
		if(m_editorMode == EditorMode_RegionSet)
			fGridS = 0;
		int nCount = (int)(m_fHeightRadius / m_scene3D->GetGridSize())-1;
		if(nCount<0)nCount = 0;
		int gx = (int)((this->m_vHeightCenter.pos.x + fGridS) / m_scene3D->GetGridSize()) - nCount/2;
		int gy = (int)((this->m_vHeightCenter.pos.y + fGridS) / m_scene3D->GetGridSize()) - nCount/2;
		for(int x=0;x<=nCount;x++)
		{
			for(int y=0;y<=nCount;y++)
			{
				int xBlend = x + gx-1;
				int yBlend = y + gy-1;
				if(m_editorMode == EditorMode_HeightGridEnable||m_editorMode == EditorMode_RegionSet)
				{
					if((nCount%2)==0)
					{
						xBlend++;
						yBlend++;
					}
				}
				for(int xx=-1;xx<=1;xx++)
				{
					for(int yy=-1;yy<=1;yy++)
					{
						int xRegion	=	(xBlend + xx) / m_scene3D->GetRegionWidthGrids();
						int yRegion	=	(yBlend + yy) / m_scene3D->GetRegionWidthGrids();
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

								if(!m_savedDataT[m_nEditRegionCount])
									m_savedDataT[m_nEditRegionCount] = new SaveRegionT();
								SaveRegionTs(*m_savedDataT[m_nEditRegionCount], pRegion);

								m_nEditRegionCount	++;
							}
						}
					}
				}
				(*proc)(this, xBlend, yBlend, Phantom::float2(0,0), Phantom::float3(0,0,0), this->m_editorInfo[this->m_editorMode]);
			}
		}
	//}
}
