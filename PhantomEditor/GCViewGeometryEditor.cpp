/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "GCViewGeometryEditor.h"
#include "MainFrm.h"
#include "resource.h"
#include "PropertiesWnd.h"
#include "PropertiesDialogWnd.h"
#include "ImportEquipDLG.h"
#include "GCViewManager.h"
#include "DlgMixFPS.h"
#include "PhantomDB.h"
GCViewGeometryEditor::GCViewGeometryEditor(CPhantomGCDoc* docPtr)
	:GCViewBase(docPtr, Language(L"模型编辑工具"), IDB_GEOMETRY_VIEW_24)
{
	m_nLodViewIndex	=	-1;
	m_pGeometryPtr	=	0;
	m_bPartMode		=	false;
	m_bAutoMoveMode	=	false;
	m_moveDir.setxyz(1,1,1);
	m_moveOrig.setxyz(0,0,0);
	m_bMoveDir		=	false;
	m_fMoveLength	=	0;
	m_fMoveTime		=	0;
	m_bShowBox		=	true;
}

Phantom::RibbonEmitterData2 g_copyRibbon;
Phantom::ParticleEmitterData2 g_copyParticle;
VOID	CopyParticle(Phantom::MeshData* dt, int index)
{
	if(index<0||index>=dt->m_particles.size())
		return;
	g_copyParticle = *dt->m_particles[index];
	g_copyParticle.texturePtr = 0;
}
VOID	CopyRibbon(Phantom::MeshData* dt, int index)
{
	if(index<0||index>=dt->m_ribbons.size())
		return;
	g_copyRibbon = *dt->m_ribbons[index];
	g_copyRibbon.texturePtr = 0;
}
VOID	PasteParticle(Phantom::MeshData* dt, int index)
{
	if(index<0||index>=dt->m_particles.size())
		return;
	*dt->m_particles[index] = g_copyParticle;
}
VOID	PasteRibbon(Phantom::MeshData* dt, int index)
{
	if(index<0||index>=dt->m_ribbons.size())
		return;
	*dt->m_ribbons[index] = g_copyRibbon;
}

GCViewGeometryEditor::~GCViewGeometryEditor(void)
{
	CloseFile();
}

HICON	GCViewGeometryEditor::GetViewerIcon()
{
	static HICON	hIcon	=	AfxGetApp()->LoadIcon(IDI_GEOMETRY_FILE_ICON);
	static HICON	hIconR	=	AfxGetApp()->LoadIcon(IDI_Z_MESH_FILE_ICON);
	if(this->m_bIsReadOnly)
		return hIconR;
	return hIcon;
}

VOID	GCViewGeometryEditor::LoopAllParticle(ProcGeometryParticle proc, LPARAM nParameter, BOOL bRename, LPARAM nParameter2)
{
	std::map<Phantom::SubMeshData*, int> equips;
	std::map<Phantom::SubMesh*, int> parts;
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
	Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == GEOMETRY_PARTICLE_BIND)
		{
			Phantom::ParticleEmitterData2* d = data->m_particles[type->p1];//GetParticlePtr(type->p1);
			(*proc)(data, d, nParameter, nParameter2);
		}
	}
	this->SetModifyed(TRUE);
}
VOID	GCViewGeometryEditor::LoopAllRibbon(ProcGeometryRibbon proc, LPARAM nParameter, BOOL bRename, LPARAM nParameter2)
{
	std::map<Phantom::SubMeshData*, int> equips;
	std::map<Phantom::SubMesh*, int> parts;
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
	Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == GEOMETRY_RIBBON_BIND)
		{
			(*proc)(data, type->p1, nParameter, nParameter2);
		}
	}
	this->SetModifyed(TRUE);
}

VOID	GCViewGeometryEditor::LoopAllGeometryShaders(ProcGeometryShader proc, LPARAM nParameter, BOOL bRename, LPARAM nParameter2)
{
	std::map<Phantom::SubMeshData*, int> equips;
	std::map<Phantom::SubMesh*, int> parts;
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
	Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		//if(type->type == GEOMETRY_SHADER)
		//{
		//	Phantom::SubMeshData* equipPtr		=	data->GetPartEquipDataPtr(type->p1, type->p2);
		//	if(!equipPtr)
		//		continue;
		//	Phantom::MeshShaderProject* shader	=	equipPtr->GetShaderPtr((Phantom::MeshShaderType)type->p3);
		//	if(!shader)
		//		continue;
		//	Phantom::SubMesh* pRender	=	m_pGeometryPtr->GetSubmeshPtr(type->p1);
		//	parts[pRender]	=	0;
		//	(*proc)(data, equipPtr, pRender, shader, nParameter, nParameter2);
		//	//
		//	equips[equipPtr]	=	0;
		//	if(bRename)
		//	{
		//		m_treeView.SetItemText(it->first, Wutf8(shader->GetName()));
		//		m_subGeometryEditor.SetGeometryData(equipPtr, m_pGeometryPtr, shader);
		//	}
		//	SetModifyed();
		//}
	}
}

VOID	GCViewGeometryEditor::LoopAllGeometryMaterials(ProcGeometryMaterial proc, LPARAM nParameter, BOOL bReloadTexture, LPARAM nParameter2)
{
	std::map<Phantom::SubMeshData*, int> equips;
	std::map<Phantom::SubMesh*, int> parts;
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
	Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == GEOMETRY_MATERIAL)
		{
			Phantom::SubMeshData* equipPtr	=	data->GetSubmeshPtr(type->p1);
			Phantom::SubMesh* pRender		=	m_pGeometryPtr->GetSubmeshPtr(type->p1);
			parts[pRender]	=	0;
			(*proc)(data, equipPtr, pRender, type->p3, nParameter, nParameter2);
			equips[equipPtr]	=	0;
		}
		else if(type->type == GEOMETRY_MODELS)
		{
			Phantom::SubMeshData* equipPtr	=	data->GetSubmeshPtr(type->p1);
			Phantom::SubMesh* pRender		=	m_pGeometryPtr->GetSubmeshPtr(type->p1);
			parts[pRender]	=	0;
			(*proc)(data, equipPtr, pRender, 0, nParameter, nParameter2);
			equips[equipPtr]	=	0;
		}
	}
	for(std::map<Phantom::SubMeshData*, int>::iterator it = equips.begin();it != equips.end(); it++)
	{
		if(bReloadTexture)
			it->first->ReloadAllTextures();
		it->first->SortAlphaBlendGeometrys();
		this->SetModifyed(TRUE);
	}
	//for(std::map<Phantom::SubMesh*, int>::iterator it = parts.begin();it != parts.end(); it++)
	//{
	//	it->first->ResetAllTextures();
	//}
}

VOID	OnSetMtrlCullMode(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	//if(render && render->GetMaterialPtr(nMaterialIndex))
	//	render->GetMaterialPtr(nMaterialIndex)->SetCullMode(nParameter);
	if(geoms->GetMaterialCount()>0)
		geoms->GetMaterialPtr(nMaterialIndex)->cullMode	=	nParameter;
}

VOID	OnSetMtrlAlphaMode(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	//if(render && render->GetMaterialPtr(nMaterialIndex))
	//	render->GetMaterialPtr(nMaterialIndex)->SetAlphaMode(nParameter);
	if(geoms->GetMaterialCount()>0)
		geoms->GetMaterialPtr(nMaterialIndex)->alphaMode	=	nParameter;
}

VOID	OnSetMtrlLodEnable(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->GetMaterialCount()>0)
		geoms->GetMaterialPtr(nMaterialIndex)->lodEnable	=	nParameter;
}


VOID	OnSetMtrlTextureChange(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
//	geoms->GetMaterialPtr(nMaterialIndex)->bTextureChange	=	nParameter;
}


VOID	OnSetMtrlColorChange(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
//	geoms->GetMaterialPtr(nMaterialIndex)->bColorChange	=	nParameter;
}

VOID	OnSetMtrlVisible(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->GetMaterialCount()==0)return;
	if(render && render->GetMaterialPtr(nMaterialIndex))
		render->GetMaterialPtr(nMaterialIndex)->SetVisible(nParameter);
	geoms->GetMaterialPtr(nMaterialIndex)->visible	=	nParameter;
}

VOID	OnSetMtrlVisibleInShadowMap(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->GetMaterialCount()==0)return;
	geoms->GetMaterialPtr(nMaterialIndex)->visibleInShadowMap	=	nParameter;
}

VOID	OnSetMtrlSrcBlend(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->GetMaterialCount()==0)return;
	geoms->GetMaterialPtr(nMaterialIndex)->srcBlend	=	nParameter;
}

VOID	OnSetMtrlDestBlend(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->GetMaterialCount()==0)return;
	geoms->GetMaterialPtr(nMaterialIndex)->destBlend	=	nParameter;
}

VOID	OnSetMtrlAnimationSpeed(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	//geoms->GetMaterialPtr(nMaterialIndex)->animationSpeed	=	*(float*)nParameter;
}

VOID	OnSetMtrlDiffuseColor(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->GetMaterialCount()==0)return;
	Phantom::color4 c;
	c.from((unsigned int)nParameter, geoms->GetMaterialPtr(nMaterialIndex)->diffuse.a);
	geoms->GetMaterialPtr(nMaterialIndex)->diffuse	=	c;
}
VOID	OnSetMtrlDiffuseColors(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->GetMaterialCount()==0)return;
	Phantom::color4 c;
	c.from((unsigned int)nParameter, geoms->GetMaterialPtr(nMaterialIndex)->diffuseColor[nParameter2].a);
	geoms->GetMaterialPtr(nMaterialIndex)->diffuseColor[nParameter2]	=	c;
}

VOID	OnSetMtrlTransparent(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->GetMaterialCount()==0)return;
	Phantom::color4 c;
	c	=	geoms->GetMaterialPtr(nMaterialIndex)->diffuse;
	c.a	=	*(float*)nParameter;
	geoms->GetMaterialPtr(nMaterialIndex)->diffuse	=	c;
}

VOID	OnSetMtrlEmissiveColor(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->GetMaterialCount()==0)return;
	Phantom::color4 c;
	c.from((unsigned int)nParameter, 1.0f);
	geoms->GetMaterialPtr(nMaterialIndex)->emissive	=	c;
}

VOID	OnSetMtrlSpecularColor(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->GetMaterialCount()==0)return;
	Phantom::color4 c;
	c.from((unsigned int)nParameter, 1.0f);
	geoms->GetMaterialPtr(nMaterialIndex)->specular =	c;
}
VOID	OnSetMtrlDiffuseFile(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	const char* szFile	=	(const char*)nParameter;
	geoms->GetMaterialPtr(nMaterialIndex)->diffuseTexture[nParameter2] = szFile;
	strlwr(geoms->GetMaterialPtr(nMaterialIndex)->diffuseTexture[nParameter2].t);
}
VOID	OnSetMtrlTextureFile(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->GetMaterialCount()==0)return;
	const char* szFile	=	(const char*)nParameter;
	geoms->GetMaterialPtr(nMaterialIndex)->texture[nParameter2] = szFile;
	strlwr(geoms->GetMaterialPtr(nMaterialIndex)->texture[nParameter2].t);
}
VOID	OnSetMtrlTextureClear(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, INT nMaterialIndex, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->GetMaterialCount()==0)return;
	const char* szFile	=	(const char*)nParameter;
	for(int i=0;i<Phantom::TEXTURE_MAX_COUNT;i++){
		geoms->GetMaterialPtr(nMaterialIndex)->texture[i] = "";
	}
	for(int i=0;i<DIFFUSE_TEXTURE_COUNT;i++)
	{
		geoms->GetMaterialPtr(nMaterialIndex)->diffuseTexture[i] = "";
	}
	//if(tex)
	//	tex->Release();
	//geoms->ReloadAllTextures();
}

VOID	GCViewGeometryEditor::LoopAllActions(ProcGeometryAction proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRefreshTree)
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
	Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == GEOMETRY_ACTION)
		{
			Phantom::MeshActionRange* act	=	data->GetActionPtr(type->p1);
			//Phantom::MeshMaterialAttribute* mtrl	=	equipPtr->GetMaterialPtr(type->p3);
			(*proc)(data, type->p1, nParameter, nParameter2);
			//
			this->SetModifyed(TRUE);
			if(m_pGeometryPtr->GetCurrentActionID() == type->p1)
			{
				m_pGeometryPtr->SetCurrentAction(type->p1, 0xf0000000);
			}
			if(bRefreshTree)
				m_treeView.SetItemText(it->first, Wutf8(act->GetName()));
		}
	}
}

VOID	OnSetActionBeginKey(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	Phantom::MeshActionRange* act	=	data->GetActionPtr(nActionID);
	act->SetLoop(nParameter, act->GetLoopEnd());
}

VOID	OnSetActionEndKey(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	Phantom::MeshActionRange* act	=	data->GetActionPtr(nActionID);
	act->SetLoop(act->GetLoopBegin(), nParameter);
}
VOID	OnSetActionLoopMode(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	Phantom::MeshActionRange* act	=	data->GetActionPtr(nActionID);
	act->m_bLoopMode = nParameter;//SetActionLoopMode(act, nParameter);
}
VOID	OnSetActionRibbon(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	Phantom::MeshActionRange* act	=	data->GetActionPtr(nActionID);
	act->m_rabbinEnable = nParameter;//SetActionRibbon(act, nParameter);
}
VOID	OnSetActionSpeed(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	Phantom::MeshActionRange* act	=	data->GetActionPtr(nActionID);
	act->SetActionSpeed(*(float*)nParameter);
}
VOID	OnSetActionSwitchSpeed(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	//Phantom::MeshActionRange* act	=	data->GetActionPtr(nActionID);
	//act->SetRotationZ(*(float*)nParameter);
}

VOID	OnSetActionSetDefault(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetDefaultActionID(nActionID);
}


VOID	OnSetActionHit1(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	data->GetActionPtr(nActionID)->m_toHitKeyID = nParameter;
}
VOID	OnSetActionMoveOffset(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	data->GetActionPtr(nActionID)->m_fMoveOffset = (*(float*)nParameter);
}

VOID	OnSetActionSetSndFile(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	data->GetActionPtr(nActionID)->m_sndfile = (const char*)nParameter;
	data->GetActionPtr(nActionID)->m_nWaveID = g_manager.GetMusicFileIndex(data->GetActionPtr(nActionID)->m_sndfile.t);
}
VOID	OnSetActionSetSndClear(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	data->GetActionPtr(nActionID)->m_sndfile.t[0] = 0;
	data->GetActionPtr(nActionID)->m_nWaveID = -1;
}
VOID	OnSetActionSetSndKey(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	data->GetActionPtr(nActionID)->m_nSndKeyIndex[0] = nParameter;
}
VOID	OnSetActionSetSndKey2(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	data->GetActionPtr(nActionID)->m_nSndKeyIndex[1] = nParameter;
}
VOID	OnSetActionSetActionType(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	Phantom::MeshActionRange* act	=	data->GetActionPtr(nActionID);
	act->m_actionType = nParameter;
}

VOID	OnSetActionSetName(Phantom::MeshData* data, INT nActionID, LPARAM nParameter, LPARAM nParameter2)
{
	Phantom::MeshActionRange* act	=	data->GetActionPtr(nActionID);
	act->SetName((const char*)nParameter);
}

VOID	GCViewGeometryEditor::LoopAllParts(ProcPartType proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRefreshTree)
{
	std::map<HTREEITEM,TreeItemType*> refreshs;
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
	Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == GEMOETRY_PART_TYPE)
		{
			(*proc)(data, type->p1, nParameter, nParameter2 );
			//
			if(bRefreshTree)
				refreshs[it->first] = it->second;
			this->SetModifyed(TRUE);
		}
	}
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = refreshs.begin();it != refreshs.end(); it++)
	{
		TreeItemType* type	=	it->second;
		this->m_treeView.SetItemText(it->first, Wutf8(data->GetSubmeshPtr(type->p1)->GetName()));
	}
}

VOID	OnSetPartName(Phantom::MeshData* data, INT nPartIndex, LPARAM nParameter, LPARAM nParameter2)
{
	data->GetSubmeshPtr(nPartIndex)->SetName((const char*)nParameter);
}

VOID	GCViewGeometryEditor::LoopAllGeometrys(ProcSubGeometryData proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRefreshTree)
{
	std::map<HTREEITEM,TreeItemType*> refreshs;
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
	Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == GEOMETRY_MODELS)
		{
			Phantom::SubMeshData* equipPtr		=	data->GetSubmeshPtr(type->p1);
			//Phantom::MeshMaterialAttribute* mtrl	=	equipPtr->GetMaterialPtr(type->p3);
			(*proc)(data, equipPtr, m_pGeometryPtr->GetSubmeshPtr(type->p1), nParameter, nParameter2 );
			//
			if(bRefreshTree)
				refreshs[it->first] = it->second;
			this->SetModifyed(TRUE);
		}
	}
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = refreshs.begin();it != refreshs.end(); it++)
	{
		TreeItemType* type	=	it->second;
		Phantom::SubMeshData* equipPtr		=	data->GetSubmeshPtr(type->p1);
		this->m_treeView.SetItemText(it->first, Wutf8(equipPtr->GetName()));
	}
}

VOID	OnSetGeometryName(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	geoms->SetName((const char*)nParameter);
}

VOID	OnSetGeometryVisible(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	geoms->SetVisible(nParameter);
	if(render)
		render->SetVisible(nParameter);
}

VOID	OnSetGeometryZWrite(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	geoms->SetZWriteEnable(nParameter);
}

VOID	OnSetGeometryToShadow(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	geoms->m_bToShadowMap = nParameter;
}

VOID	OnSetGeometryWithShadow(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	geoms->m_bEnableShadow = nParameter;
}

VOID	OnSetGeometryIsCollision(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	geoms->m_bIsCollision = nParameter;
}
VOID	OnSetGeometryUserColor(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	//geoms->SetHasVertexColor(nParameter);
}

VOID	OnSetGeometrySelfAni(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	geoms->SetExistMeshAnim(nParameter);
}

VOID	OnSetGeometryGenTangent(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	//geoms->GenericTangentBinormal(nParameter);
}
VOID	OnSetGeometryActionShow(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	geoms->m_showOnAction[nParameter] = nParameter2;
	geoms->m_showOnActionCount = nParameter+1;
}
//
//VOID	OnSetGeometryMinFilter(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
//{
//	geoms->SetSamplerMinFilter(nParameter);
//}
//
//VOID	OnSetGeometryMagFilter(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
//{
//	geoms->SetSamplerMagFilter(nParameter);
//}
//
//VOID	OnSetGeometryMaxAnisotropy(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
//{
//	geoms->SetSamplerMaxAnisotropy(nParameter);
//}
//
//VOID	OnSetGeometryMipmapBias(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
//{
//	geoms->SetMipmapLodBias(*(float*)nParameter);
//}

VOID	GeometryShader_ForceAlpha(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, Phantom::MeshShaderProject* shader, LPARAM nParameter, LPARAM nParameter2)
{
	shader->SetForceAlphaBlend(nParameter);
}

VOID	GeometryShader_SrcBlend(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, Phantom::MeshShaderProject* shader, LPARAM nParameter, LPARAM nParameter2)
{
	shader->SetSrcBlend((Phantom::BlendMode)nParameter);
}

VOID	GeometryShader_DestBlend(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, Phantom::MeshShaderProject* shader, LPARAM nParameter, LPARAM nParameter2)
{
	shader->SetDestBlend((Phantom::BlendMode)nParameter);
}

VOID	GeometryShader_CullMode(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, Phantom::MeshShaderProject* shader, LPARAM nParameter, LPARAM nParameter2)
{
	shader->SetCullMode((Phantom::CullMode)nParameter);
}

struct VspsVarData
{
	std::string			name;
	int					id;
	int					type;
	Phantom::float4		value;
};

VOID	GeometryShader_Technique(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, Phantom::MeshShaderProject* shader, LPARAM nParameter, LPARAM nParameter2)
{
	//shader->SetTehcnique((const char*)nParameter);
	//shader->SetName((const char*)nParameter2);
	////
	//const char* str = (const char*)nParameter;
	////
	//std::vector<VspsVarData>	vsps_variants;
	//IVspsObject* obj	=	GetRenderDevicePtr()->GetVspsObjectPtr(data->GetShaderMode());
	//if(!obj)
	//	return;
	//IShader* vsps		=	obj->GetNormalShaderPtr();
	//if(vsps)
	//{
	//	for(int i=1;i<=10;i++)
	//	{
	//		_shader_var var;
	//		char buf[128];
	//		sprintf(buf, "var_%d", i);
	//		const char* var_name;
	//		if(!vsps->GetTextParameter(var, str, buf, &var_name))
	//			continue;
	//		if(vsps->FindVariant(var_name) != NULL)
	//		{
	//			int type;
	//			const char* sztype;
	//			if(!vsps->GetTextParameter(var_name, "type", &sztype))
	//				continue;
	//			const char* szid;
	//			if(!vsps->GetTextParameter(var_name, "id", &szid))
	//				szid	=	"0";
	//			if(stricmp(sztype, "float") == 0)
	//				type	=	VSPS_TYPE_FLOAT;
	//			else if(stricmp(sztype, "check") == 0 || stricmp(sztype, "int") == 0)
	//				type	=	VSPS_TYPE_INT;
	//			else if(stricmp(sztype, "color") == 0 || stricmp(sztype, "Phantom::float4") == 0)
	//				type	=	VSPS_TYPE_VECTOR;
	//			else
	//				continue;
	//			//
	//			var.reset_handle();
	//			Phantom::float4 value(0,0,0,0);
	//			switch(type)
	//			{
	//			case VSPS_TYPE_FLOAT:
	//				{
	//					float f = 0;
	//					if(vsps->GetFloat(var, f, var_name))
	//						value	=	Phantom::float4(f, 0, 0, 0);
	//				}
	//				break;
	//			case VSPS_TYPE_INT:
	//				{
	//					int f = 0;
	//					if(vsps->GetInt(var, f, var_name))
	//						value	=	Phantom::float4(f, 0, 0, 0);
	//				}
	//				break;
	//			case VSPS_TYPE_VECTOR:
	//				{
	//					Phantom::float4 v(0,0,0,0);
	//					if(vsps->GetFloat4(var, v, var_name))
	//						value	=	v;
	//				}
	//				break;
	//			default:
	//				continue;
	//			}
	//			//
	//			VspsVarData varData;
	//			varData.name	=	var_name;
	//			varData.type	=	type;
	//			varData.value	=	value;
	//			varData.id		=	atoi(szid);
	//			vsps_variants.push_back( varData );
	//		}
	//	}
	//}
	//std::vector<std::string> vars;
	//for(int i=0;i<shader->GetVariants()->GetDataCount();i++)
	//	shader->GetVariants()->ClearDatas();
	////
	//for(size_t i=0;i<vsps_variants.size();i++)
	//{
	//	Phantom::PShaderValue* val	=	shader->GetVariants()->AddData(0);
	//	val->id		=	vsps_variants.at(i).id;
	//	val->name	=	vsps_variants.at(i).name.c_str();
	//	val->type	=	vsps_variants.at(i).type;
	//	val->value	=	vsps_variants.at(i).value;
	//}
}

VOID	OnSetGeometryGenLOD(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	if(geoms->m_bExistShadowMap){
		GetMainFrame()->MessageBox(Language(L"ShadowExistNoLOD", L"存在阴影贴图的模型不能创建lod"));
	}
	else
		geoms->m_polygons->GenericLodMesh(nParameter);
}

VOID	OnSetGeometryLevel(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	for(int i=0;i<MAX_LOD_LEVEL_COUNT;i++)
	{
		int face	=	geoms->m_polygons->GetLodLevelFaceCount(i);
		wchar_t buf[128];
		swprintf(buf, L"%d tri.", face);
		if(wcsicmp(buf, (const wchar_t*)nParameter2) == 0)
		{
			geoms->m_polygons->SetLevelLodIndex(nParameter, i);
			return;
		}
	}
	geoms->m_polygons->SetLevelLodIndex(nParameter, nParameter);
}

VOID	OnSetGeometryHighlight(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
//	geoms->SetEffectSupport(EffectSupport_Highlight, nParameter);
//	render->SetEffectSupport(EffectSupport_Highlight, nParameter);
}

VOID	OnSetGeometryInvertFace(Phantom::MeshData* data, Phantom::SubMeshData* geoms, Phantom::SubMesh* render, LPARAM nParameter, LPARAM nParameter2)
{
	//geoms->InvertFaces();
}

VOID	OnRibbonAlpha(Phantom::MeshData* data, int index, LPARAM nParameter, LPARAM nParameter2)
{
	data->m_ribbons[index]->fAlpha = *(float*)nParameter;
}
VOID	OnRibbonBindBone(Phantom::MeshData* data, int index, LPARAM nParameter, LPARAM nParameter2)
{
	data->m_ribbons[index]->boneIndex = nParameter;
}
VOID	OnRibbonBindBone2(Phantom::MeshData* data, int index, LPARAM nParameter, LPARAM nParameter2)
{
	data->m_ribbons[index]->boneIndex2 = nParameter;
}
VOID	OnRibbonDestBlend(Phantom::MeshData* data, int index, LPARAM nParameter, LPARAM nParameter2)
{
	data->m_ribbons[index]->destBlend = nParameter;
}
VOID	OnRibbonLiveSecond(Phantom::MeshData* data, int index, LPARAM nParameter, LPARAM nParameter2)
{
	Phantom::RibbonEmitterData2* d = data->m_ribbons[index];
	d->countLifeSec = max(nParameter, 1);
	d->maxLifeSec = d->countPerSec * d->countLifeSec;
	if( d->maxLifeSec > MAX_RIBBON_EDGE )
		d->maxLifeSec= MAX_RIBBON_EDGE;
}
VOID	OnRibbonSrcBlend(Phantom::MeshData* data, int index, LPARAM nParameter, LPARAM nParameter2)
{
	Phantom::RibbonEmitterData2* d = data->m_ribbons[index];
	d->srcBlend = nParameter;
}
VOID	OnRibbonTexture(Phantom::MeshData* data, int index, LPARAM nParameter, LPARAM nParameter2)
{
	Phantom::RibbonEmitterData2* d = data->m_ribbons[index];
	memset(d->texture, 0, sizeof(d->texture));
	strncpy(d->texture, (const char*)nParameter, 128);
	if(d->texture[0])
	{
		Phantom::TexturePtr tex;
		if(Phantom::LoadTexture(tex, d->texture, 0, true)){
			Phantom::Texture* old = d->texturePtr;
			d->texturePtr	=	tex.GetPtr();
			safe_addref(d->texturePtr);
			safe_release(old);
		}
		//d->texturePtr = LoadTexture(0, static_cast<bz_geometryGroupData*>(dt)->m_ribbons[index]->texture, static_cast<bz_geometryGroupData*>(dt)->GetFileName());
	}
}

VOID	OnParticleAlpha(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->nAlpha[nParameter2] = (int)(*(float*)nParameter * 255.0f);
}
VOID	OnParticleMiddleTime(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->fDecayTime = *(float*)nParameter;
}
VOID	OnParticlePivot(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->vCenter.operator float *()[nParameter2] = *(float*)nParameter;
}
VOID	OnParticlePlaneNormal(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->vPlaneNormal.operator float *()[nParameter2] = *(float*)nParameter;
}
VOID	OnParticleRow(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->nUVRow = nParameter;
}
VOID	OnParticleScaling(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->fParticleScaling[nParameter2] = *(float*)nParameter;;
}
VOID	OnParticleSpeed(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->speed = *(float*)nParameter;
}
VOID	OnParticleSrcBlend(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->srcBlend = nParameter;
}
VOID	OnParticleTailLength(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->destBlend = nParameter;
}
VOID	OnParticleTexture(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	safe_release(d->texturePtr);
	strcpy(d->szTexture, (const char*)nParameter);
}
VOID	OnParticleUVAnimFps(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->dwUVAnimFps = nParameter;
}
VOID	OnParticleVariation(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->speedvar = *(float*)nParameter;
}
VOID	OnParticleWidth(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->fPlaneWidth	= *(float*)nParameter;
}

VOID	OnParticleBindBone(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->nBindBoneID = nParameter;
}
VOID	OnParticleCol(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->nUVCol = nParameter;
}
VOID	OnParticleColor(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	Phantom::color4 c(nParameter);
	d->nSegmentColor[nParameter2][0] = c.r * 255.0f;
	d->nSegmentColor[nParameter2][1] = c.g * 255.0f;
	d->nSegmentColor[nParameter2][2] = c.b * 255.0f;
}
VOID	OnParticleConeAngle(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->fConeAngle = *(float*)nParameter;
}
VOID	OnParticleDecayUVAnim(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->dwDecayUVAnim[nParameter2] = nParameter;
}
VOID	OnParticleEmissionRate(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->fTimeLength = *(float*)nParameter;
}
VOID	OnParticleEnable(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	if(nParameter)
	{
		if(d->dwBlendMode & 0x1000)
			d->dwBlendMode ^= 0x1000;
	}
	else
		d->dwBlendMode |= 0x1000;
}

VOID	OnParticleDestBlend(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->destBlend = nParameter;
}
VOID	OnParticleHeight(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->fPlaneHeight = *(float*)nParameter;
}
VOID	OnParticleLifeSpan(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->fLifeSpan = *(float*)nParameter;
}
VOID	OnParticleLifeSpanUVAnim(Phantom::MeshData* data, Phantom::ParticleEmitterData2* d, LPARAM nParameter, LPARAM nParameter2)
{
	d->dwLifeSpanUVAnim[nParameter2] = nParameter;
}
VOID		GCViewGeometryEditor::OnMaterialChange(const CMFCPropertyGridProperty* pProp)
{
		if(pProp == m_propMtrlCullMode)
		{
			LoopAllGeometryMaterials(OnSetMtrlCullMode, GetPropOption(m_propMtrlCullMode, g_typeCull), false);
		}
		else if(pProp == m_propMtrlAlphaMode)
		{
			LoopAllGeometryMaterials(OnSetMtrlAlphaMode, GetPropOption(m_propMtrlAlphaMode, g_typeAlpha), false);
			EnableDisableAll();
		}
		else if(pProp == m_propMtrlTransparent)
		{
			LoopAllGeometryMaterials(OnSetMtrlTransparent, (LPARAM)GetPropFloatPtr(m_propMtrlTransparent), false);
		}
		else if(pProp == m_propMtrlLodEnable)
		{
			LoopAllGeometryMaterials(OnSetMtrlLodEnable, GetPropBool(m_propMtrlLodEnable), false);
		}
		else if(pProp == m_propMtrlVisible)
		{
			LoopAllGeometryMaterials(OnSetMtrlVisible, GetPropBool(m_propMtrlVisible), false);
		}
		else if(pProp == m_propMtrlVisibleInShadowMap)
		{
			LoopAllGeometryMaterials(OnSetMtrlVisibleInShadowMap, GetPropBool(m_propMtrlVisibleInShadowMap), false);
		}
		else if(pProp == m_propMtrlSrcBlend)
		{
			LoopAllGeometryMaterials(OnSetMtrlSrcBlend, GetPropOption(m_propMtrlSrcBlend, g_typeBlend), false);
		}
		else if(pProp == m_propMtrlDestBlend)
		{
			LoopAllGeometryMaterials(OnSetMtrlDestBlend, GetPropOption(m_propMtrlDestBlend, g_typeBlend), false);
		}
		//else if(pProp == m_propMtrlAnimationSpeed)
		//{
		//	float f = GetPropFloat(m_propMtrlAnimationSpeed);
		//	LoopAllGeometryMaterials(OnSetMtrlAnimationSpeed, (LPARAM)&f, false);
		//}
		//else if(pProp == m_propMtrlTextureChange)
		//{
		//	LoopAllGeometryMaterials(OnSetMtrlTextureChange, GetPropBool(m_propMtrlTextureChange), false);
		//}
		//else if(pProp == m_propMtrlColorChange)
		//{
		//	LoopAllGeometryMaterials(OnSetMtrlColorChange, GetPropBool(m_propMtrlColorChange), false);
		//}
		else if(pProp == m_propMtrlDiffuseColor)
		{
			LoopAllGeometryMaterials(OnSetMtrlDiffuseColor, GetPropColor(m_propMtrlDiffuseColor), false);
		}
		else if(pProp == m_propMtrlEmissiveColor)
		{
			LoopAllGeometryMaterials(OnSetMtrlEmissiveColor, GetPropColor(m_propMtrlEmissiveColor), false);
		}
		//else if(pProp == m_propMtrlSpecularColor)
		//{
		//	LoopAllGeometryMaterials(OnSetMtrlSpecularColor, GetPropColor(m_propMtrlSpecularColor), false);
		//}
		else
		{
			for(int i=0;i<Phantom::TEXTURE_MAX_COUNT;i++)
			{
				if(pProp == m_propMtrlTextureFile[i])
				{
					std::wstring str;
					GetPropFile(m_propMtrlTextureFile[i], str, true);
					{
						wchar_t geomPath[256], geomDir[256];
						wchar_t newPath[256], newDir[256];
						const wchar_t*	strText	=	str.c_str();
						if(m_strFile.size() > 0)
						{
						}
						A szFileA(strText);
						if(strlen(szFileA) >= 128)
						{
							MessageBox(this->m_modelProps.m_hWnd, Language(L"文件路径不能超过128个字节,请重新选择短路径."), L"Error", MB_OK);
						}
						else
						{
							Phantom::FileName ftemp = szFileA.buf;
							ftemp.setIsFile();
							LoopAllGeometryMaterials(OnSetMtrlTextureFile, (LPARAM)(const char*)ftemp.t, true, i);
						}
					}
				}
			}
			for(int i=0;i<DIFFUSE_TEXTURE_COUNT;i++)
			{
				if(pProp == m_propMtrlDiffuseColors[i])
				{
					LoopAllGeometryMaterials(OnSetMtrlDiffuseColors, GetPropColor(m_propMtrlDiffuseColors[i]), false, i);
					break;
				}
				if(pProp == m_propMtrlDiffuseFile[i])
				{
					std::wstring str;
					GetPropFile(m_propMtrlDiffuseFile[i], str, true);
					{
						wchar_t geomPath[256], geomDir[256];
						wchar_t newPath[256], newDir[256];
						const wchar_t*	strText	=	str.c_str();
						if(m_strFile.size() > 0)
						{
						}
						A szFileA(strText);
						if(strlen(szFileA) >= 128)
						{
							MessageBox(this->m_modelProps.m_hWnd, Language(L"文件路径不能超过128个字节,请重新选择短路径."), L"Error", MB_OK);
						}
						else
						{
							Phantom::FileName ftemp = szFileA.buf;
							ftemp.setIsFile();
							LoopAllGeometryMaterials(OnSetMtrlDiffuseFile, (LPARAM)(const char*)ftemp.t, true, i);
						}
						this->m_pGeometryPtr->ChangeDiffuse(i+1);
						break;
					}
				}
			}
			if(pProp == m_propMtrlTextureClear)
			{
				LoopAllGeometryMaterials(OnSetMtrlTextureClear, (LPARAM)0, true, 0);
				for(int i=0;i<Phantom::TEXTURE_MAX_COUNT;i++)
				{
					const char* szFile	=	"";
					SetPropText(m_propMtrlTextureFile[i], L"");
				}
			}
		}
}
void		GCViewGeometryEditor::OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp)
{
	__super::OnPropertyChanged(wnd, pProp);
	if(&m_modelProps == wnd)
	{
		if(pProp == m_propModelName)
		{
			std::wstring name;
			GetPropText(m_propModelName, name);
			this->LoopAllGeometrys(OnSetGeometryName, (LPARAM)Autf8(name.c_str()).buf, 0, true);
			Phantom::MeshData* data = this->m_pGeometryPtr->GetDataPtr();
			for(int i=0;i<data->m_geometrys.size();i++)
				data->m_geometrys[i]->CheckTypeFromName();
		}
		else if(pProp == m_propModelVisible)
		{
			this->LoopAllGeometrys(OnSetGeometryVisible, GetPropBool(m_propModelVisible));
		}
		else if(pProp == m_propModelZWrite)
		{
			this->LoopAllGeometrys(OnSetGeometryZWrite, GetPropBool(m_propModelZWrite));
		}
		else if(pProp == m_propModelToShadow)
		{
			this->LoopAllGeometrys(OnSetGeometryToShadow, GetPropBool(m_propModelToShadow));
		}
		else if(pProp == m_propModelWithShadow)
		{
			this->LoopAllGeometrys(OnSetGeometryWithShadow, GetPropBool(m_propModelWithShadow));
		}
		else if(pProp == m_propModelIsCollision)
		{
			this->LoopAllGeometrys(OnSetGeometryIsCollision, GetPropBool(m_propModelIsCollision));
		}
		/*else if(pProp == m_propModelUseColor)
		{
			this->LoopAllGeometrys(OnSetGeometryUserColor, GetPropBool(m_propModelUseColor));
		}*/
		//else if(pProp == m_propModelSelfAni)
		//{
		//	this->LoopAllGeometrys(OnSetGeometrySelfAni, GetPropBool(m_propModelSelfAni));
		//}
		else if(pProp == m_propModelGenTangent)
		{
			this->LoopAllGeometrys(OnSetGeometryGenTangent, GetPropBool(m_propModelGenTangent));
		}
		/*else if(pProp == m_propModelMinFilter)
		{
			this->LoopAllGeometrys(OnSetGeometryMinFilter, GetPropOption(m_propModelMinFilter, g_typeFilter));
		}
		else if(pProp == m_propModelMagFilter)
		{
			this->LoopAllGeometrys(OnSetGeometryMagFilter, GetPropOption(m_propModelMagFilter, g_typeFilter));
		}
		else if(pProp == m_propModelMaxAnisotropy)
		{
			this->LoopAllGeometrys(OnSetGeometryMaxAnisotropy, GetPropInt(m_propModelMaxAnisotropy));
		}*/
		/*else if(pProp == m_propModelMipmapBias)
		{
			this->LoopAllGeometrys(OnSetGeometryMipmapBias, (LPARAM)GetPropFloatPtr(m_propModelMipmapBias));
		}*/
		else if(pProp == m_propModelGenLOD)
		{
			BOOL b	=	GetPropBool(m_propModelGenLOD);
			this->LoopAllGeometrys(OnSetGeometryGenLOD, b);
			for(int i=0;i<MAX_LOD_LEVEL_COUNT;i++)
				this->m_propModelLODLevel[i]->Enable(b);
			this->RebuildLODs();
		}
		else if(pProp == m_propModelSetCollision)
		{
			for(int i=0;i<this->m_treeView.GetSelectCount();i++)
			{
				HTREEITEM hItem		=	m_treeView.GetSelect(i);
				TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
				if(type&&type->type == GEOMETRY_MODELS)
				{
					Phantom::SubMesh* mesh = m_pGeometryPtr->GetSubmeshPtr(type->p1);
					m_pGeometryPtr->RecalcCollisionMesh(mesh);
					this->SetModifyed(TRUE);
					break;
				}
			}
		}
		//else if(pProp == m_propModelHighlight)
		//{
		//	this->LoopAllGeometrys(OnSetGeometryHighlight, GetPropBool(m_propModelHighlight));
		//}
		//else if(pProp == m_propModelInvertFace)
		//{
		//	this->LoopAllGeometrys(OnSetGeometryInvertFace, 0);
		//}
		else if(pProp == m_propModelLodVisible)
		{
			this->m_pGeometryPtr->SetCurrentLodLevel(GetPropOption(m_propModelLodVisible, g_typeLodLevel));
		}
		else
		{
			for(int i=0;i<MAX_LOD_LEVEL_COUNT;i++)
			{
				if(pProp == this->m_propModelLODLevel[i])
				{
					//std::wstring str;
					//GetPropText(this->m_propModelLODLevel[i], str);
					//this->LoopAllGeometrys(OnSetGeometryLevel, i, (LPARAM)str.c_str());
					break;
				}
			}
			for(int i=0;i<MAX_SHOW_ON_ACTION;i++){
				if(pProp==m_propModelOnAction[i]){
					LoopAllGeometrys(OnSetGeometryActionShow, i, GetPropOption(m_propModelOnAction[i], g_typeUnitActionType));
					break;
				}
			}
		}
		OnMaterialChange(pProp);
	}
	else if(&this->m_ribbonEffect == wnd)
	{
		if(pProp == m_ribbonLiveSecond)
		{
			this->LoopAllRibbon(OnRibbonLiveSecond, GetPropInt(m_ribbonLiveSecond));
		}
		else if(pProp == m_ribbonTexture)
		{
			std::wstring str;
			GetPropFile(m_ribbonTexture, str, true);
			A a(str.c_str());
			this->LoopAllRibbon(OnRibbonTexture, (LPARAM)(const char*)a);
			this->ReinitGeometry();
		}
		else if(pProp == this->m_ribbonBindBone)
		{
			std::wstring str;
			GetPropText(m_ribbonBindBone, str);
			Autf8 a(str.c_str());
			for(int i=0;i<this->m_pGeometryPtr->GetDataPtr()->GetBoneCount();i++)
			{
				char buf[128];
				sprintf(buf, "%s.%d", m_pGeometryPtr->GetDataPtr()->GetBoneNameFromIndex(i), i+1);
				if(stricmp(buf, a.buf) == 0)
				{
					this->LoopAllRibbon(OnRibbonBindBone, (LPARAM)i);
					break;
				}
			}
		}
		else if(pProp == this->m_ribbonBindBone2)
		{
			std::wstring str;
			GetPropText(m_ribbonBindBone2, str);
			int nBone = -1;
			Autf8 a(str.c_str());
			for(int i=0;i<this->m_pGeometryPtr->GetDataPtr()->GetBoneCount();i++)
			{
				char buf[128];
				sprintf(buf, "%s.%d", m_pGeometryPtr->GetDataPtr()->GetBoneNameFromIndex(i), i+1);
				if(stricmp(buf, a.buf) == 0)
				{
					nBone = i;
					break;
				}
			}
			this->LoopAllRibbon(OnRibbonBindBone2, (LPARAM)nBone);
		}
		else if(pProp == m_ribbonSrcBlend)
		{
			this->LoopAllRibbon(OnRibbonSrcBlend, GetPropOption(m_ribbonSrcBlend, g_typeBlend));
		}
		else if(pProp == m_ribbonDestBlend)
		{
			this->LoopAllRibbon(OnRibbonDestBlend, GetPropOption(m_ribbonDestBlend, g_typeBlend));
		}
		else if(pProp == m_ribbonAlpha)
		{
			this->LoopAllRibbon(OnRibbonAlpha, (LPARAM)GetPropFloatPtr(m_ribbonAlpha));
		}
	}
	else if(&this->m_particleProps == wnd)
	{
		if(pProp == m_particleTexture)
		{
			std::wstring str;
			GetPropFile(m_particleTexture, str, true);
			A a(str.c_str());
			this->LoopAllParticle(OnParticleTexture, (LPARAM)(const char*)a);
			ReinitGeometry();
		}
		else if(pProp == this->m_particleBindBone)
		{
			std::wstring str;
			GetPropText(m_particleBindBone, str);
			Autf8 a(str.c_str());
			for(int i=0;i<this->m_pGeometryPtr->GetDataPtr()->GetBoneCount();i++)
			{
				char buf[128];
				sprintf(buf, "%s.%d", m_pGeometryPtr->GetDataPtr()->GetBoneNameFromIndex(i), i+1);
				if(stricmp(buf, a.buf) == 0)
				{
					this->LoopAllParticle(OnParticleBindBone, (LPARAM)i);
					break;
				}
			}
		}
		else if(pProp == m_particleSpeed)LoopAllParticle(OnParticleSpeed, (LPARAM)GetPropFloatPtr(m_particleSpeed));
		else if(pProp == m_particleVariation)LoopAllParticle(OnParticleVariation, (LPARAM)GetPropFloatPtr(m_particleVariation));
		else if(pProp == m_particleConeAngle)LoopAllParticle(OnParticleConeAngle, (LPARAM)GetPropFloatPtr(m_particleConeAngle));
		else if(pProp == m_particleLifeSpan)LoopAllParticle(OnParticleLifeSpan, (LPARAM)GetPropFloatPtr(m_particleLifeSpan));
		else if(pProp == m_particleEmissionRate)LoopAllParticle(OnParticleEmissionRate, (LPARAM)GetPropFloatPtr(m_particleEmissionRate));
		else if(pProp == m_particleWidth)LoopAllParticle(OnParticleWidth, (LPARAM)GetPropFloatPtr(m_particleWidth));
		else if(pProp == m_particleHeight)LoopAllParticle(OnParticleHeight, (LPARAM)GetPropFloatPtr(m_particleHeight));
		else if(pProp == m_particleTailLength)LoopAllParticle(OnParticleTailLength, (LPARAM)GetPropFloatPtr(m_particleTailLength));
		else if(pProp == m_particleMiddleTime)LoopAllParticle(OnParticleMiddleTime, (LPARAM)GetPropFloatPtr(m_particleMiddleTime));
		else if(pProp == m_particleAlpha[0])LoopAllParticle(OnParticleAlpha, (LPARAM)GetPropFloatPtr(m_particleAlpha[0]), 0, 0);
		else if(pProp == m_particleAlpha[1])LoopAllParticle(OnParticleAlpha, (LPARAM)GetPropFloatPtr(m_particleAlpha[1]), 0, 1);
		else if(pProp == m_particleAlpha[2])LoopAllParticle(OnParticleAlpha, (LPARAM)GetPropFloatPtr(m_particleAlpha[2]), 0, 2);
		//
		else if(pProp == m_particleRow)LoopAllParticle(OnParticleRow, (LPARAM)GetPropInt(m_particleRow));
		else if(pProp == m_particleCol)LoopAllParticle(OnParticleCol, (LPARAM)GetPropInt(m_particleCol));
		else if(pProp == m_particleUVAnimFps)LoopAllParticle(OnParticleUVAnimFps, (LPARAM)GetPropInt(m_particleUVAnimFps));
		//
		else if(pProp == m_particleScaling[0])LoopAllParticle(OnParticleScaling, (LPARAM)GetPropFloatPtr(m_particleScaling[0]), 0, 0);
		else if(pProp == m_particleScaling[1])LoopAllParticle(OnParticleScaling, (LPARAM)GetPropFloatPtr(m_particleScaling[1]), 0, 1);
		else if(pProp == m_particleScaling[2])LoopAllParticle(OnParticleScaling, (LPARAM)GetPropFloatPtr(m_particleScaling[2]), 0, 2);
		else if(pProp == m_particleColor[0])LoopAllParticle(OnParticleColor, (LPARAM)GetPropColor(m_particleColor[0]), 0, 0);
		else if(pProp == m_particleColor[1])LoopAllParticle(OnParticleColor, (LPARAM)GetPropColor(m_particleColor[1]), 0, 1);
		else if(pProp == m_particleColor[2])LoopAllParticle(OnParticleColor, (LPARAM)GetPropColor(m_particleColor[2]), 0, 2);
		else if(pProp == m_particleLifeSpanUVAnim[0])LoopAllParticle(OnParticleLifeSpanUVAnim, (LPARAM)GetPropInt(m_particleLifeSpanUVAnim[0]), 0, 0);
		else if(pProp == m_particleLifeSpanUVAnim[1])LoopAllParticle(OnParticleLifeSpanUVAnim, (LPARAM)GetPropInt(m_particleLifeSpanUVAnim[1]), 0, 1);
		else if(pProp == m_particleLifeSpanUVAnim[2])LoopAllParticle(OnParticleLifeSpanUVAnim, (LPARAM)GetPropInt(m_particleLifeSpanUVAnim[2]), 0, 2);
		else if(pProp == m_particleDecayUVAnim[0])LoopAllParticle(OnParticleDecayUVAnim, (LPARAM)GetPropInt(m_particleDecayUVAnim[0]), 0, 0);
		else if(pProp == m_particleDecayUVAnim[1])LoopAllParticle(OnParticleDecayUVAnim, (LPARAM)GetPropInt(m_particleDecayUVAnim[1]), 0, 1);
		else if(pProp == m_particleDecayUVAnim[2])LoopAllParticle(OnParticleDecayUVAnim, (LPARAM)GetPropInt(m_particleDecayUVAnim[2]), 0, 2);

		else if(pProp == m_particlePivot[0])LoopAllParticle(OnParticlePivot, (LPARAM)GetPropFloatPtr(m_particlePivot[0]), 0, 0);
		else if(pProp == m_particlePivot[1])LoopAllParticle(OnParticlePivot, (LPARAM)GetPropFloatPtr(m_particlePivot[1]), 0, 1);
		else if(pProp == m_particlePivot[2])LoopAllParticle(OnParticlePivot, (LPARAM)GetPropFloatPtr(m_particlePivot[2]), 0, 2);
		else if(pProp == m_particlePlaneNormal[0])LoopAllParticle(OnParticlePlaneNormal, (LPARAM)GetPropFloatPtr(m_particlePlaneNormal[0]), 0, 0);
		else if(pProp == m_particlePlaneNormal[1])LoopAllParticle(OnParticlePlaneNormal, (LPARAM)GetPropFloatPtr(m_particlePlaneNormal[1]), 0, 1);
		else if(pProp == m_particlePlaneNormal[2])LoopAllParticle(OnParticlePlaneNormal, (LPARAM)GetPropFloatPtr(m_particlePlaneNormal[2]), 0, 2);

		else if(pProp == m_particleSrcBlend)LoopAllParticle(OnParticleSrcBlend, GetPropOption(m_particleSrcBlend, g_typeBlend));
		else if(pProp == m_particleDestBlend)LoopAllParticle(OnParticleDestBlend, GetPropOption(m_particleDestBlend, g_typeBlend));
		else if(pProp == m_particleEnable)LoopAllParticle(OnParticleEnable, GetPropBool(m_particleEnable));
	}
	//else if(&m_shaderProps == wnd)
	//{
	//	if(pProp == m_shaderTechnique)
	//	{
	//		std::wstring str;
	//		GetPropText(m_shaderTechnique, str);
	//		const char* szTech	=	this->GetShaderTechnique(str.c_str());
	//		this->LoopAllGeometryShaders(GeometryShader_Technique, (LPARAM)szTech, true, (LPARAM)A(str.c_str()).buf);
	//		this->RebuildShaderVars();
	//		//
	//		IVspsObject* obj	=	GetRenderDevicePtr()->GetVspsObjectPtr(this->m_pGeometryPtr->GetDataPtr()->GetShaderMode());
	//		if(obj)
	//		{
	//			IShader* vsps	=	obj->GetNormalShaderPtr();
	//			_shader_var tech;
	//			const char* desc = 0;
	//			if(vsps->GetTextParameter(tech, szTech, "desc", &desc))
	//			{
	//				m_shaderTechnique->SetDescription((const wchar_t*)W(desc));
	//			}
	//			else
	//				m_shaderTechnique->SetDescription((const wchar_t*)W(szTech));
	//		}
	//		//需要添加每个方案的变量列表
	//	}
	//	else if(pProp == m_shaderForceAlpha)
	//	{
	//		BOOL b	=	GetPropBool(m_shaderForceAlpha);
	//		LoopAllGeometryShaders(GeometryShader_ForceAlpha, b);
	//		m_shaderSrcBlend->Enable(b);
	//		m_shaderDestBlend->Enable(b);
	//		m_shaderCullMode->Enable(b);
	//	}
	//	else if(pProp == m_shaderSrcBlend)
	//	{
	//		LoopAllGeometryShaders(GeometryShader_SrcBlend, GetPropOption(m_shaderSrcBlend, g_typeBlend));
	//	}
	//	else if(pProp == m_shaderDestBlend)
	//	{
	//		LoopAllGeometryShaders(GeometryShader_DestBlend, GetPropOption(m_shaderDestBlend, g_typeBlend));
	//	}
	//	else if(pProp == m_shaderCullMode)
	//	{
	//		LoopAllGeometryShaders(GeometryShader_CullMode, GetPropOption(m_shaderCullMode, g_typeCull));
	//	}
	//}
	else if(&this->m_partListProp == wnd)
	{
		if(pProp == m_propPartName)
		{
			std::wstring str;
			GetPropText(m_propPartName, str);
			LoopAllParts(OnSetPartName, (LPARAM)Autf8(str.c_str()).buf, 0, true);
		}
	}
	else if(&m_groupProps == wnd)
	{
		if(pProp == m_propGroupRenderToShadow)
		{
			this->m_pGeometryPtr->GetDataPtr()->SetRenderToShadowMap(GetPropBool(m_propGroupRenderToShadow));
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupSetCollision)
		{
			m_pGeometryPtr->RecalcCollisionMesh(0);
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupByShadowMap)
		{
			this->m_pGeometryPtr->GetDataPtr()->SetAffectByShadowMap(GetPropBool(m_propGroupByShadowMap));
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupDiffuse)
		{
			this->m_pGeometryPtr->ChangeDiffuse(GetPropOption(m_propGroupDiffuse, g_typeGroupDiffuse));
		}
		else if(pProp == m_propGroupShadowEpsilon)
		{
			this->m_pGeometryPtr->GetDataPtr()->SetShadowEpsilon(GetPropFloat(m_propGroupShadowEpsilon));
			this->SetModifyed(true);
		}
		//else if(pProp == m_propGroupShaderMode)
		//{
		//	std::wstring str;
		//	GetPropText(m_propGroupShaderMode, str);
		//	this->m_pGeometryPtr->GetDataPtr()->SetShaderMode(GetShaderMode(str.c_str()));
		//	this->SetModifyed(true);
		//	this->RebuildVSPS();
		//}
		else if(pProp == m_propGroupLodRangeLength)
		{
			this->m_pGeometryPtr->GetDataPtr()->SetLodRangeLength(GetPropFloat(m_propGroupLodRangeLength));
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupLodMaxVisible)
		{
			this->m_pGeometryPtr->GetDataPtr()->SetMaxVisibleLength(GetPropFloat(m_propGroupLodMaxVisible));
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupClipVisible)
		{
			this->m_pGeometryPtr->GetDataPtr()->SetClipVisible(GetPropBool(m_propGroupClipVisible));
			this->SetModifyed(true);
		}
		else if(pProp == m_groupShowSelect)
		{
//			this->m_pGeometryPtr->SetSelect(GetPropBool(m_groupShowSelect));
		}
		//else if(pProp == m_groupPartsMode)
		//{
		//	m_bPartMode	=	GetPropBool(m_groupPartsMode);
		//	this->BuildGeometrys();
		//}
		//else if(pProp == m_groupActiveShader)
		//{
		//	Phantom::MeshShaderType type	=	(Phantom::MeshShaderType)GetPropOption(m_groupActiveShader, g_typeShaderType);
		//	this->m_pGeometryPtr->GetDataPtr()->SetActiveShader(type);
		//	this->m_pGeometryPtr->SetActiveShader(type);
		//	this->SetModifyed(true);
		//}
		//else if(pProp == m_groupShaderCount)
		//{
		//	this->m_pGeometryPtr->GetDataPtr()->SetShaderCount(GetPropInt(m_groupShaderCount));
		//	this->BuildGeometrys();
		//}
		else if(pProp == m_propGroupShadowVisible)
		{
			BOOL b	=	GetPropBool(m_propGroupShadowVisible);
			this->m_pGeometryPtr->GetDataPtr()->SetShadowVisible(b);
			m_propGroupShadowUVOffsetX->Enable(b);
			m_propGroupShadowUVOffsetY->Enable(b);
			m_propGroupShadowSize->Enable(b);
			m_propGroupShadowTexture->Enable(b);
			m_propGroupShadowTextureClear->Enable(b);
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupShadowUVOffsetX)
		{
			Phantom::float2 uv	=	m_pGeometryPtr->GetDataPtr()->GetShadowUVOffset();
			uv.x		=	GetPropFloat(m_propGroupShadowUVOffsetX);
			this->m_pGeometryPtr->GetDataPtr()->SetShadowUVOffset(uv);
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupShadowUVOffsetY)
		{
			Phantom::float2 uv	=	m_pGeometryPtr->GetDataPtr()->GetShadowUVOffset();
			uv.y		=	GetPropFloat(m_propGroupShadowUVOffsetY);
			this->m_pGeometryPtr->GetDataPtr()->SetShadowUVOffset(uv);
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupShadowSize)
		{
			this->m_pGeometryPtr->GetDataPtr()->SetShadowSize(GetPropFloat(m_propGroupShadowSize));
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupShadowTexture)
		{
			std::wstring str;
			GetPropFile(m_propGroupShadowTexture, str, true);
			this->m_pGeometryPtr->GetDataPtr()->SetShadowTexture(A(str.c_str()));
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupShadowTextureClear)
		{
			this->m_pGeometryPtr->GetDataPtr()->SetShadowTexture("");
			this->SetModifyed(true);
		}
		//PropertyFilePtr				;
		else if(pProp == m_propGroupLodMaxEnable)
		{
			this->m_pGeometryPtr->GetDataPtr()->SetLodEnableMaxVisible(GetPropBool(m_propGroupLodMaxEnable));
			this->SetModifyed(true);
		}
		else if(pProp == m_propModelInvertTurn)
		{
			m_pGeometryPtr->GetDataPtr()->m_bTurn180 = (GetPropBool(m_propModelInvertTurn));
			m_pGeometryPtr->GetDataPtr()->RebuildBaseMatrix();
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupHeight)
		{
			m_pGeometryPtr->GetDataPtr()->m_fBaseScale = GetPropFloat(m_propGroupHeight)/m_pGeometryPtr->GetDataPtr()->m_idleHeight;
			m_pGeometryPtr->GetDataPtr()->RebuildBaseMatrix();
			m_pGeometryPtr->SetWorldMatrix(m_pGeometryPtr->GetWorldMatrix());
			//SetPropFloat(m_propGroupHeight, (m_pGeometryPtr->GetDataPtr()->m_fBaseScale*m_pGeometryPtr->GetDataPtr()->m_idleHeight));
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupHeightBase)
		{
			m_pGeometryPtr->GetDataPtr()->m_idleHeight = GetPropFloat(m_propGroupHeightBase);
			if(m_pGeometryPtr->GetDataPtr()->m_idleHeight<0.1f)m_pGeometryPtr->GetDataPtr()->m_idleHeight=0.1f;
			m_pGeometryPtr->GetDataPtr()->m_fBaseScale = GetPropFloat(m_propGroupHeight)/m_pGeometryPtr->GetDataPtr()->m_idleHeight;
			m_pGeometryPtr->GetDataPtr()->RebuildBaseMatrix();
			m_pGeometryPtr->SetWorldMatrix(m_pGeometryPtr->GetWorldMatrix());
			//SetPropFloat(m_propGroupHeight, (m_pGeometryPtr->GetDataPtr()->m_fBaseScale*m_pGeometryPtr->GetDataPtr()->m_idleHeight));
			this->SetModifyed(true);
		}
		else if(pProp == m_propGroupType){
			m_pGeometryPtr->GetDataPtr()->m_meshType = GetPropOption(m_propGroupType, g_typeGroupType);
			this->BuildTableLine();
			this->SetModifyed(true);
		}
		else if(pProp == this->m_propGroupTableLine){
			Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
			const char* szName = 0;
			if(dataPtr->m_meshType==Phantom::MeshData::MeshType_SceneUnit)
				szName = "sceneunit_table";
			else if(dataPtr->m_meshType==Phantom::MeshData::MeshType_Role)
				szName = "role_table";
			if(szName)
			{
				Phantom::PtrID id(1,1);
				Phantom::Config* su = Phantom::GetGameMgr()->GetConfigPtr(id, szName);
				if(!su)return;
				std::wstring wstr;
				GetPropText(m_propGroupTableLine, wstr);
				Autf8 a(wstr.c_str());
				for(int i=0;i<su->GetLineCount();i++)
				{
					char buf[256];
					sprintf(buf, "%s", su->GetLine(i)->GetLineName());
					if(stricmp(buf, a.buf)==0){
						dataPtr->m_lineID = su->GetLine(i)->GetLineID();
						break;
					}
				}
			}
			m_pGeometryPtr->GetDataPtr()->m_meshType = GetPropOption(m_propGroupType, g_typeGroupType);
			this->SetModifyed(true);
		}
		
	}
	else if(&m_actionProps == wnd)
	{
		if(pProp == m_propActionBeginKey)
		{
			this->LoopAllActions(OnSetActionBeginKey, GetPropInt(m_propActionBeginKey));
		}
		else if(pProp == m_propActionEndKey)
		{
			this->LoopAllActions(OnSetActionEndKey, GetPropInt(m_propActionEndKey));
		}
		else if(pProp == m_propActionRibbon)
		{
			this->LoopAllActions(OnSetActionRibbon, GetPropBool(m_propActionRibbon));
		}
		else if(pProp == m_propActionLoopMode)
		{
			this->LoopAllActions(OnSetActionLoopMode, GetPropBool(m_propActionLoopMode));
		}
		else if(pProp == m_propActionSpeed)
		{
			this->LoopAllActions(OnSetActionSpeed, (LPARAM)GetPropFloatPtr(m_propActionSpeed));
			this->m_pGeometryPtr->SetActionSpeed(1.0f);//GetPropFloat(m_propActionSpeed));
		}
		//else if(pProp == m_propActionRotation)
		//{
		//	this->LoopAllActions(OnSetActionSwitchSpeed, (LPARAM)GetPropFloatPtr(m_propActionRotation));
		//	SetPropFloat(m_propActionRotation, 0.0f);
		//}
		else if(pProp == m_propActionSetDefault)
		{
			this->LoopAllActions(OnSetActionSetDefault, (LPARAM)GetPropBool(m_propActionSetDefault));
		}
		else if(pProp == m_propActionSndFile)
		{
			std::wstring str;
			GetPropFile(m_propActionSndFile, str);
			this->LoopAllActions(OnSetActionSetSndFile, (LPARAM)Autf8(str.c_str()).buf);
		}
		else if(pProp == m_propActionSndClear)
		{
			this->LoopAllActions(OnSetActionSetSndClear, 0);
		}

		else if(pProp == m_propActionHit1)
		{
			this->LoopAllActions(OnSetActionHit1, (LPARAM)GetPropInt(m_propActionHit1));
		}
		else if(pProp == m_propActionMoveOffset)
		{
			this->LoopAllActions(OnSetActionMoveOffset, (LPARAM)GetPropFloatPtr(m_propActionMoveOffset));
		}
		else if(pProp == m_propActionSndPlayKey)
		{
			this->LoopAllActions(OnSetActionSetSndKey, (LPARAM)GetPropInt(m_propActionSndPlayKey));
		}
		else if(pProp == m_propActionSndPlayKey2)
		{
			this->LoopAllActions(OnSetActionSetSndKey2, (LPARAM)GetPropInt(m_propActionSndPlayKey2));
		}
		else if(pProp == m_propActionType)
		{
			LoopAllActions(OnSetActionSetActionType, GetPropOption(m_propActionType, g_typeUnitActionType));
		}
		else if(pProp == m_propActionName)
		{
			std::wstring str;
			GetPropText(m_propActionName, str);
			this->LoopAllActions(OnSetActionSetName, (LPARAM)Autf8(str.c_str()).buf, 0, true);
		}
	}
	else if(&m_materialProps == wnd)
	{
		OnMaterialChange(pProp);
	}
}

VOID		GCViewGeometryEditor::EnableDisableAll()
{
	bool bEnableAlpha	=	(GetPropOption(m_propMtrlAlphaMode, g_typeAlpha) == Phantom::AlphaMode_Blend);
	this->m_propMtrlSrcBlend->Enable(bEnableAlpha);
	this->m_propMtrlDestBlend->Enable(bEnableAlpha);
	this->m_propMtrlTransparent->Enable(bEnableAlpha);
}
BOOL	g_bCopyRibbon = false;
BOOL	g_bCopyParticle = false;
VOID		GCViewGeometryEditor::BuildTableLine()
{
	Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
	m_propGroupTableLine->Enable(dataPtr->m_meshType>0);
	if(dataPtr->m_meshType==0)
		return;
	m_propGroupTableLine->RemoveAllOptions();
	const char* szName = 0;
	if(dataPtr->m_meshType==Phantom::MeshData::MeshType_SceneUnit)
		szName = "sceneunit_table";
	else if(dataPtr->m_meshType==Phantom::MeshData::MeshType_Role)
		szName = "role_table";
	if(szName)
	{
		Phantom::PtrID id(1,1);
		Phantom::Config* su = Phantom::GetGameMgr()->GetConfigPtr(id, szName);
		if(!su)return;
		for(int i=0;i<su->GetLineCount();i++)
		{
			char buf[256];
			sprintf(buf, "%s", su->GetLine(i)->GetLineName());
			m_propGroupTableLine->AddOption(Wutf8(buf).buf);
			if(su->GetLine(i)->GetLineID() == dataPtr->m_lineID)
				SetPropText(m_propGroupTableLine, Wutf8(buf).buf);
		}
	}
}
VOID		GCViewGeometryEditor::OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem)	//更新选择树形控件
{
	if(treePtr == &m_treeView)
	{
		TreeItemType* type	=	this->GetSelectedItem();
		if(!type)
		{
			SetCurrentProperties(&m_viewerProps, L"文件属性", NULL);
			return;
		}
		switch(type->type)
		{
		case GEMOETRY_PART_TYPE:
			{
				m_partListProp.EnableRender(false);
				if(m_pGeometryPtr->GetDataPtr()->m_pakdir!=0)m_partListProp.DisableEditor();
				SetCurrentProperties(&m_partListProp, Language(L"部件组属性"), NULL);
				Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
				SetPropText(this->m_propPartName, Wutf8(dataPtr->GetSubmeshPtr(type->p1)->GetName()));
				m_partListProp.EnableRender(true);
			}
			break;
		case GEOMETRY_MODEL_LIST:
			{
				m_groupProps.EnableRender(false);
				SetCurrentProperties(&this->m_groupProps, Language(L"模型组属性"), NULL);
				if(m_pGeometryPtr->GetDataPtr()->m_pakdir!=0)m_groupProps.DisableEditor();
				Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
				//
				SetPropBool(m_propGroupRenderToShadow, dataPtr->IsRenderToShadowMap());
				SetPropOption(m_propGroupDiffuse, g_typeGroupDiffuse, m_pGeometryPtr->GetDiffuseIndex());
				SetPropBool(m_propGroupByShadowMap, dataPtr->IsAffectByShadowMap());
				SetPropFloat(m_propGroupShadowEpsilon, dataPtr->GetShadowEpsilon());
				//
				SetPropFloat(m_propGroupLodRangeLength, dataPtr->GetLodRangeLength());
				SetPropBool(m_propGroupLodMaxEnable, dataPtr->IsLodEnableMaxVisible());
				SetPropFloat(m_propGroupLodMaxVisible, dataPtr->GetMaxVisibleLength());
				SetPropBool(m_propGroupClipVisible, dataPtr->IsClipVisible());
				//SetPropOption(m_groupActiveShader, g_typeShaderType, (Phantom::MeshShaderType)dataPtr->GetActiveShader());
				SetPropBool(m_propGroupShadowVisible, dataPtr->IsShadowVisible());
				//SetPropBool(m_groupPartsMode, m_bPartMode);
				//SetPropInt(m_groupShaderCount, dataPtr->GetShaderCount());
				SetPropFloat(m_propGroupShadowUVOffsetX, dataPtr->GetShadowUVOffset().x);
				SetPropFloat(m_propGroupShadowUVOffsetY, dataPtr->GetShadowUVOffset().y);
				SetPropFloat(m_propGroupShadowSize, dataPtr->GetShadowSize());
				SetPropFile(m_propGroupShadowTexture, dataPtr->GetShadowTexture());
				SetPropBool(m_propModelInvertTurn, dataPtr->m_bTurn180);
				//SetPropFloat(m_propGroupScale, dataPtr->m_fBaseScale);
				//m_propGroupSetCollision->Enable(m_pGeometryPtr->GetDataPtr()->GetActionCount()>0);
				SetPropFloat(m_propGroupHeight, (m_pGeometryPtr->GetDataPtr()->m_fBaseScale*m_pGeometryPtr->GetDataPtr()->m_idleHeight));
				SetPropFloat(m_propGroupHeightBase, m_pGeometryPtr->GetDataPtr()->m_idleHeight);
				//
				SetPropOption(this->m_propGroupType, g_typeGroupType, dataPtr->m_meshType);
				BuildTableLine();
				//
				BOOL b	=	dataPtr->IsShadowVisible();
				m_propGroupShadowUVOffsetX->Enable(b);
				m_propGroupShadowUVOffsetY->Enable(b);
				m_propGroupShadowSize->Enable(b);
				m_propGroupShadowTexture->Enable(b);
				m_propGroupShadowTextureClear->Enable(b);
				//SetPropText(m_propGroupShaderMode, GetShaderMode(dataPtr->GetShaderMode()));
				m_groupProps.EnableRender(true);
			}
			break;
		case GEOMETRY_ACTION_LIST:
			{
				SetCurrentProperties(NULL, Language(L"动画列表"), NULL);
			}
			break;
		case GEOMETRY_BIND_LIST:
			{
				SetCurrentProperties(NULL, Language(L"绑定列表"), NULL);
			}
			break;
		case GEOMETRY_PARTICLE_BIND:
			{
				m_particleProps.EnableRender(false);
				SetCurrentProperties(&this->m_particleProps, Language(L"粒子特效"), NULL);
				if(m_pGeometryPtr->GetDataPtr()->m_pakdir!=0)m_particleProps.DisableEditor();
				//
				Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
				Phantom::ParticleEmitterData2* part = dataPtr->m_particles[type->p1];//GetParticlePtr(type->p1);
				//
				SetPropFloat(this->m_particleSpeed, part->speed);
				SetPropFloat(this->m_particleVariation, part->speedvar);
				SetPropFloat(this->m_particleConeAngle, part->fConeAngle);
				SetPropFloat(this->m_particleLifeSpan, part->fLifeSpan);
				SetPropFloat(this->m_particleEmissionRate, part->fTimeLength);
				SetPropFloat(this->m_particleWidth, part->fPlaneWidth);
				SetPropFloat(this->m_particleHeight, part->fPlaneHeight);
				SetPropFloat(this->m_particleTailLength, part->fTailLength);
				SetPropFloat(this->m_particleMiddleTime, part->fDecayTime);
				//
				SetPropInt(this->m_particleRow, part->nUVRow);
				SetPropInt(this->m_particleCol, part->nUVCol);
				//
				SetPropInt(this->m_particleUVAnimFps, part->dwUVAnimFps);
				SetPropOption(m_particleSrcBlend, g_typeBlend, part->srcBlend);
				SetPropOption(m_particleDestBlend, g_typeBlend, part->destBlend);
				SetPropBool(m_particleEnable, (part->dwBlendMode & 0x1000) == 0);
				
				m_particleBindBone->RemoveAllOptions();
				char buf[128];
				for(int i=0;i<dataPtr->GetBoneCount();i++)
				{
					sprintf(buf, "%s.%d", dataPtr->GetBoneNameFromIndex(i), i + 1);
					m_particleBindBone->AddOption(Wutf8(buf));
				}
				int bid = part->nBindBoneID;
				sprintf(buf, "%s.%d", dataPtr->GetBoneNameFromIndex(bid), bid + 1);
				SetPropText(m_particleBindBone, Wutf8(buf).buf);
				//
				SetPropFile(m_particleTexture, part->szTexture);
				//
				SetPropColor(m_particleColor[0], RGB(part->nSegmentColor[0][0], part->nSegmentColor[0][1], part->nSegmentColor[0][2]));
				SetPropColor(m_particleColor[1], RGB(part->nSegmentColor[1][0], part->nSegmentColor[1][1], part->nSegmentColor[1][2]));
				SetPropColor(m_particleColor[2], RGB(part->nSegmentColor[2][0], part->nSegmentColor[2][1], part->nSegmentColor[2][2]));
				//
				SetPropFloat(m_particleAlpha[0], part->nAlpha[0] / 255.0f);
				SetPropFloat(m_particleAlpha[1], part->nAlpha[1] / 255.0f);
				SetPropFloat(m_particleAlpha[2], part->nAlpha[2] / 255.0f);
				//
				SetPropFloat(m_particleScaling[0], part->fParticleScaling[0]);
				SetPropFloat(m_particleScaling[1], part->fParticleScaling[1]);
				SetPropFloat(m_particleScaling[2], part->fParticleScaling[2]);
				//
				SetPropInt(m_particleLifeSpanUVAnim[0], part->dwLifeSpanUVAnim[0]);
				SetPropInt(m_particleLifeSpanUVAnim[1], part->dwLifeSpanUVAnim[1]);
				SetPropInt(m_particleLifeSpanUVAnim[2], part->dwLifeSpanUVAnim[2]);
				//
				SetPropInt(m_particleDecayUVAnim[0], part->dwDecayUVAnim[0]);
				SetPropInt(m_particleDecayUVAnim[1], part->dwDecayUVAnim[1]);
				SetPropInt(m_particleDecayUVAnim[2], part->dwDecayUVAnim[2]);
				//
				SetPropFloat(m_particlePivot[0], part->vCenter.x);
				SetPropFloat(m_particlePivot[1], part->vCenter.y);
				SetPropFloat(m_particlePivot[2], part->vCenter.z);
				//
				SetPropFloat(m_particlePlaneNormal[0], part->vPlaneNormal.x);
				SetPropFloat(m_particlePlaneNormal[1], part->vPlaneNormal.y);
				SetPropFloat(m_particlePlaneNormal[2], part->vPlaneNormal.z);
				//
				m_particleProps.EnableRender(true);
			}
			break;
		case GEOMETRY_RIBBON_BIND:
			{
				m_ribbonEffect.EnableRender(false);
				SetCurrentProperties(&this->m_ribbonEffect, Language(L"刀光特效"), NULL);
				if(m_pGeometryPtr->GetDataPtr()->m_pakdir!=0)m_ribbonEffect.DisableEditor();
				//
				Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
				SetPropInt(this->m_ribbonLiveSecond, dataPtr->m_ribbons[type->p1]->countLifeSec);//GetRibbonLiveSecond(dataPtr, type->p1));
				SetPropOption(m_ribbonSrcBlend, g_typeBlend, dataPtr->m_ribbons[type->p1]->srcBlend);//GetRibbonSrcBlend(dataPtr, type->p1));
				SetPropOption(m_ribbonDestBlend, g_typeBlend, dataPtr->m_ribbons[type->p1]->destBlend);//GetRibbonDestBlend(dataPtr, type->p1));
				SetPropFloat(this->m_ribbonAlpha, dataPtr->m_ribbons[type->p1]->fAlpha);//GetRibbonAlpha(dataPtr, type->p1));
				this->m_ribbonBindBone->RemoveAllOptions();
				char buf[128];
				for(int i=0;i<dataPtr->GetBoneCount();i++)
				{
					sprintf(buf, "%s.%d", dataPtr->GetBoneNameFromIndex(i), i + 1);
					m_ribbonBindBone->AddOption(Wutf8(buf));
				}
				int bid = dataPtr->m_ribbons[type->p1]->boneIndex;//GetRibbonBoneIndex(dataPtr, type->p1);
				sprintf(buf, "%s.%d", dataPtr->GetBoneNameFromIndex(bid), bid + 1);
				SetPropText(m_ribbonBindBone, Wutf8(buf).buf);

				this->m_ribbonBindBone2->RemoveAllOptions();
				m_ribbonBindBone2->AddOption(L"n/a");
				for(int i=0;i<dataPtr->GetBoneCount();i++)
				{
					sprintf(buf, "%s.%d", dataPtr->GetBoneNameFromIndex(i), i + 1);
					m_ribbonBindBone2->AddOption(Wutf8(buf));
				}
				bid = dataPtr->m_ribbons[type->p1]->boneIndex2;//GetRibbonBoneIndex2(dataPtr, type->p1);
				if(bid >= 0)
					sprintf(buf, "%s.%d", dataPtr->GetBoneNameFromIndex(bid), bid + 1);
				else
					strcpy(buf, "n/a");
				SetPropText(m_ribbonBindBone2, Wutf8(buf).buf);

				SetPropFile(m_ribbonTexture, dataPtr->m_ribbons[type->p1]->texture);//GetRibbonTexture(dataPtr, type->p1));
				//
				m_ribbonEffect.EnableRender(true);

			}
			break;
		//case GEOMETRY_SHADER:
		//	{
		//		m_shaderProps.EnableRender(false);
		//		SetCurrentProperties(&this->m_shaderProps, Language(L"渲染方案属性"), &m_subGeometryEditor);
		//		//
		//		Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
		//		Phantom::SubMeshData* equipPtr	=	dataPtr->GetPartEquipDataPtr(type->p1, type->p2);
		//		Phantom::MeshShaderProject* shader	=	equipPtr->GetShaderPtr((Phantom::MeshShaderType)type->p3);
		//		//
		//		Phantom::SubMesh* r		=	m_pGeometryPtr->GetSubmeshPtr(type->p1);
		//		if(r)
		//		{
		//			//if(type->p3 == GeometryShaderType_Select && m_pGeometryPtr->IsSelect())
		//			//{
		//			//}
		//			//else
		//				r->SetActiveShader((Phantom::MeshShaderType)type->p3);
		//		}
		//		if(shader)
		//			SetPropText(m_shaderTechnique, m_vspsTechniques[shader->GetTehcnique()].c_str());
		//		SetPropBool(m_shaderForceAlpha, shader->IsForceAlphaBlend());
		//		SetPropOption(m_shaderSrcBlend, g_typeBlend, shader->GetSrcBlend());
		//		SetPropOption(m_shaderDestBlend, g_typeBlend, shader->GetDestBlend());
		//		SetPropOption(m_shaderCullMode, g_typeCull, shader->GetCullMode());
		//		m_shaderSrcBlend->Enable(shader->IsForceAlphaBlend());
		//		m_shaderDestBlend->Enable(shader->IsForceAlphaBlend());
		//		m_shaderCullMode->Enable(shader->IsForceAlphaBlend());
		//		//
		//		IVspsObject* obj	=	GetRenderDevicePtr()->GetVspsObjectPtr(dataPtr->GetShaderMode());
		//		if(obj && shader)
		//		{
		//			IShader* vsps	=	obj->GetNormalShaderPtr();
		//			_shader_var tech;
		//			const char* desc = 0;
		//			if(vsps->GetTextParameter(tech, shader->GetTehcnique(), "desc", &desc))
		//			{
		//				m_shaderTechnique->SetDescription((const wchar_t*)W(desc));
		//			}
		//			else
		//				m_shaderTechnique->SetDescription((const wchar_t*)W(shader->GetTehcnique()));
		//		}
		//		m_subGeometryEditor.SetGeometryData(equipPtr, m_pGeometryPtr, shader);
		//		//
		//		m_shaderProps.EnableRender(true);
		//	}
		//	break;
		case GEOMETRY_MODELS:
			{
				m_modelProps.EnableRender(false);
				SetCurrentProperties(&this->m_modelProps, Language(L"子模型属性"), 0);
				if(m_pGeometryPtr->GetDataPtr()->m_pakdir!=0)m_modelProps.DisableEditor();
				//
				Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
				Phantom::SubMeshData* equipPtr	=	dataPtr->GetSubmeshPtr(type->p1);//, nEquipID);
				SetPropText(m_propModelName, Wutf8(equipPtr->GetName()).buf);
				SetPropInt(m_propModelID, equipPtr->GetID());
				SetPropBool(m_propModelVisible, equipPtr->IsVisible());
				SetPropBool(m_propModelZWrite, equipPtr->IsZWriteEnable());
				SetPropBool(m_propModelToShadow, equipPtr->m_bToShadowMap);
				SetPropBool(m_propModelWithShadow, equipPtr->m_bEnableShadow);
				SetPropBool(m_propModelIsCollision, equipPtr->m_bIsCollision);
				//SetPropBool(m_propModelUseColor, equipPtr->HasVertexColor());
				//SetPropBool(m_propModelSelfAni, equipPtr->IsSelfAnimationMesh());
				//SetPropBool(m_propModelGenTangent, equipPtr->IsHasTangentBinormal());
				//SetPropBool(m_propModelHighlight, equipPtr->IsEffectSupport(EffectSupport_Highlight));
				//
				//SetPropOption(m_propModelMinFilter, g_typeFilter, equipPtr->GetSamplerMinFilter());
				//SetPropOption(m_propModelMagFilter, g_typeFilter, equipPtr->GetSamplerMagFilter());
				//SetPropInt(m_propModelMaxAnisotropy, equipPtr->GetSamplerMaxAnisotropy());
				SetPropInt(m_propModelVertexCount, equipPtr->m_polygons->GetVertexCount());
				//SetPropFloat(m_propModelMipmapBias, equipPtr->GetMipmapLodBias());
				//
				m_propModelSetCollision->Enable(this->m_pGeometryPtr->GetDataPtr()->GetActionCount()>0);
				SetPropBool(m_propModelGenLOD, equipPtr->m_polygons->IsLodExists());
				for(int i=0;i<MAX_LOD_LEVEL_COUNT;i++)
					this->m_propModelLODLevel[i]->Enable(equipPtr->m_polygons->IsLodExists());
				for(int i=0;i<MAX_SHOW_ON_ACTION;i++){
					SetPropOption(m_propModelOnAction[i], g_typeUnitActionType, equipPtr->m_showOnAction[i]);
				}
				//
				if(equipPtr->m_polygons->IsLodExists())
				{
					wchar_t buf[128];
					for(int i=0;i<MAX_LOD_LEVEL_COUNT;i++)
					{
						//m_propModelLODLevel[i]->RemoveAllOptions();
						//for(int j=0;j<equipPtr->m_polygons->GetLodLevelCount();j++)
						//{
							//int face	=	equipPtr->m_polygons->GetLodLevelFaceCount(i);
							//swprintf(buf, L"%d tri.", face);
							//SetPropText(m_propModelLODLevel[i], buf);
							//m_propModelLODLevel[i]->AddOption(buf);
						//}
						swprintf(buf, L"%d %s", equipPtr->m_polygons->GetLodLevelFaceCount(equipPtr->m_polygons->GetLevelLodIndex(i)), Language(L"triangle",L"面"));
						SetPropText(this->m_propModelLODLevel[i], buf);
					}
				}

				SetPropInt(m_propModelFaceCount, equipPtr->m_polygons->GetFaceCount());
				SetPropBool(m_propModel32BitMesh, equipPtr->m_polygons->Is32BitMesh());
				SetPropBool(m_propModelIsBlend, equipPtr->m_polygons->IsBlendBoneMesh());
				//
				SetPropOption(m_propModelLodVisible, g_typeLodLevel, m_nLodViewIndex);
				ResetMaterialProps(type->p1, 0);
				m_modelProps.EnableRender(true);
			}
			break;
		case GEOMETRY_MATERIAL:
			{
				m_materialProps.EnableRender(false);
				SetCurrentProperties(&this->m_materialProps, Language(L"材质属性"), NULL);
				if(m_pGeometryPtr->GetDataPtr()->m_pakdir!=0)m_materialProps.DisableEditor();
				//
				ResetMaterialProps(type->p1, type->p3);
				m_materialProps.EnableRender(true);
			}
			break;
		case GEOMETRY_ACTION:
			{
				m_actionProps.EnableRender(false);
				SetCurrentProperties(&this->m_actionProps, Language(L"动画属性"), NULL);
				if(m_pGeometryPtr->GetDataPtr()->m_pakdir!=0)m_actionProps.DisableEditor();
				//
				Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
				Phantom::MeshActionRange* act	=	dataPtr->GetActionPtr(type->p1);
				m_pGeometryPtr->SetCurrentAction(type->p1, 1000);
				m_pGeometryPtr->SetWorldMatrix(matrix4x4::s_identity());
				SetPropBool(m_propActionSetDefault, dataPtr->GetDefaultActionID() == type->p1);
				SetPropText(m_propActionSndFile, act->m_sndfile.t);
				SetPropText(m_propActionName, Wutf8(act->GetName()).buf);
				SetPropInt(m_propActionID, type->p1);
				SetPropInt(m_propActionKeyCount, act->GetKeyCount());
				SetPropInt(m_propActionBeginKey, act->GetLoopBegin());
				SetPropInt(this->m_propActionSndPlayKey, act->m_nSndKeyIndex[0]);
				SetPropInt(this->m_propActionSndPlayKey2, act->m_nSndKeyIndex[1]);

				SetPropInt(this->m_propActionHit1, act->m_toHitKeyID);
				SetPropFloat(this->m_propActionMoveOffset, act->m_fMoveOffset);

				SetPropInt(m_propActionEndKey, act->GetLoopEnd());
				SetPropFloat(m_propActionSpeed, act->GetActionSpeed());
				//SetPropFloat(m_propActionRotation, act->GetNextLerpTime());
				SetPropOption(m_propActionType, g_typeUnitActionType, act->m_actionType);
				SetPropBool(m_propActionRibbon, act->m_rabbinEnable);//GetActionRibbon(act));
				SetPropBool(m_propActionLoopMode, act->m_bLoopMode);//GetActionLoopMode(act));
				m_actionProps.EnableRender(true);
			}
			break;
		case GEOMETRY_BIND:
			{
				SetCurrentProperties(NULL, Language(L"绑定列表"), NULL);
			}
			break;
		default:
			SetCurrentProperties(NULL, NULL, NULL);
			break;
		}
		if(IsShowSelectOnly())
		{
			for(int i=0;i<m_pGeometryPtr->GetSubmeshCount();i++)
			{
				for(int j=0;j<m_pGeometryPtr->GetSubmeshPtr(i)->GetMaterialCount();j++)
				{
					m_pGeometryPtr->GetSubmeshPtr(i)->GetMaterialPtr(j)->SetVisible(false);
				}
			}
			for(int i=0;i<treePtr->GetSelectCount();i++)
			{
				HTREEITEM hItem		=	treePtr->GetSelect(i);
				TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
				if(type)
				{
					if(IsShowSelectOnly())
					{
						switch(type->type)
						{
						case GEMOETRY_PART_TYPE:
						case GEOMETRY_MODELS:
							{
								Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
								for(int m=0;m<m_pGeometryPtr->GetSubmeshPtr(type->p1)->GetMaterialCount();m++)
									m_pGeometryPtr->GetSubmeshPtr(type->p1)->GetMaterialPtr(m)->SetVisible(true);
							}
							break;
						case GEOMETRY_MODEL_LIST:
							{
								for(int k=0;k<m_pGeometryPtr->GetSubmeshCount();k++)
								{
									for(int j=0;j<m_pGeometryPtr->GetSubmeshPtr(k)->GetMaterialCount();j++)
									{
										m_pGeometryPtr->GetSubmeshPtr(k)->GetMaterialPtr(j)->SetVisible(true);
									}
								}
							}
							break;
						case GEOMETRY_MATERIAL:
							{
								m_pGeometryPtr->GetSubmeshPtr(type->p1)->GetMaterialPtr(type->p3)->SetVisible(true);
							}
							break;
						}
					}
				}
			}
		}
		else
		{
			//for(int i=0;i<m_pGeometryPtr->GetSubmeshCount();i++)
			//{
			//	for(int j=0;j<m_pGeometryPtr->GetSubmeshPtr(i)->GetMaterialCount();j++)
			//	{
			//		m_pGeometryPtr->GetSubmeshPtr(i)->GetMaterialPtr(j)->SetVisible(true);
			//	}
			//}
		}
		return;
	}
	SetCurrentProperties(NULL, NULL, NULL);
}
VOID				GCViewGeometryEditor::ResetMaterialProps(int submeshIndex, int mtrlIndex)
{
	Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
	Phantom::SubMeshData* equipPtr		=	dataPtr->GetSubmeshPtr(submeshIndex);//, nEquipID);
	Phantom::MeshMaterialAttribute* mtrl	=	equipPtr->GetMaterialPtr(mtrlIndex);//type->p3);
	if(!mtrl)return;
	//
	SetPropOption(m_propMtrlCullMode, g_typeCull, mtrl->cullMode);
	SetPropOption(m_propMtrlAlphaMode, g_typeAlpha, mtrl->alphaMode);
	m_propMtrlLodEnable->Enable(equipPtr->m_polygons->IsLodExists());
	SetPropBool(m_propMtrlVisible, mtrl->visible);
	SetPropBool(m_propMtrlLodEnable, mtrl->lodEnable);
	SetPropBool(m_propMtrlVisibleInShadowMap, mtrl->visibleInShadowMap);
	SetPropOption(m_propMtrlSrcBlend, g_typeBlend, mtrl->srcBlend);
	SetPropOption(m_propMtrlDestBlend, g_typeBlend, mtrl->destBlend);
	//SetPropInt(m_propMtrlFaceCount, mtrl->faceCount);
	//SetPropFloat(m_propMtrlAnimationSpeed, mtrl->animationSpeed);
	SetPropColor(m_propMtrlDiffuseColor, mtrl->diffuse.getRGB());
	SetPropFloat(m_propMtrlTransparent, mtrl->diffuse.a);
	SetPropColor(m_propMtrlEmissiveColor, mtrl->emissive.getRGB());
	//SetPropColor(m_propMtrlSpecularColor, mtrl->specularColor.getRGB());
	//SetPropBool(m_propMtrlTextureChange, mtrl->bTextureChange);
	//SetPropBool(m_propMtrlColorChange, mtrl->bColorChange);
	for(int i=0;i<Phantom::TEXTURE_MAX_COUNT;i++)
	{
		SetPropText(m_propMtrlTextureFile[i], mtrl->texture[i].t);
	}
	for(int i=0;i<DIFFUSE_TEXTURE_COUNT;i++)
	{
		SetPropText(m_propMtrlDiffuseFile[i], mtrl->diffuseTexture[i].t);
		SetPropColor(m_propMtrlDiffuseColors[i], mtrl->diffuseColor[i].getRGB());
	}
	EnableDisableAll();
}
const wchar_t*		GCViewGeometryEditor::GetShaderMode(int type)
{
	for(std::map<std::wstring, int>::iterator it = m_vspsModes.begin(); it != m_vspsModes.end(); it++)
	{
		if(it->second == type)
			return it->first.c_str();
	}
	return L"";
}

void				GCViewGeometryEditor::GetShaderMode(std::vector<std::wstring>& values)
{
	//Phantom::MeshData* data	=	m_pGeometryPtr->GetDataPtr();
	//IVspsObject* vspsObject	=	GetRenderDevicePtr()->GetVspsObjectPtr(data->GetShaderMode());
	//m_vspsModes.clear();
	//for(int i=0;i<GetRenderDevicePtr()->GetShaderObjectMap()->GetObjectCount();i++)
	//{
	//	Wutf8 w(GetRenderDevicePtr()->GetVspsObjectPtr(i)->GetModeName());
	//	values.push_back(w.buf);
	//	m_vspsModes[w.buf]	=	i;
	//}
}

int					GCViewGeometryEditor::GetShaderMode(const wchar_t* szText)
{
	for(std::map<std::wstring, int>::iterator it = m_vspsModes.begin(); it != m_vspsModes.end(); it++)
	{
		if(wcsicmp(it->first.c_str(), szText) == 0)
			return it->second;
	}
	return 0;
}
//
const wchar_t*		GCViewGeometryEditor::GetShaderTechnique(const char* type)
{
	for(std::map<std::string, std::wstring>::iterator it = m_vspsTechniques.begin(); it != this->m_vspsTechniques.end(); it++)
	{
		if(stricmp(it->first.c_str(), type) == 0)
			return it->second.c_str();
	}
	return L"";
}
void				GCViewGeometryEditor::GetShaderTechnique(std::vector<std::wstring>& values)
{
	//Phantom::MeshData* data	=	m_pGeometryPtr->GetDataPtr();
	//IVspsObject* vspsObject	=	GetRenderDevicePtr()->GetVspsObjectPtr(data->GetShaderMode());
	//IShader*		mode	=	vspsObject->GetNormalShaderPtr();
	//m_vspsTechniques.clear();
	//for(unsigned int i=0;i<mode->GetTechniqueCount();i++)
	//{
	//	const char* name = 0;
	//	mode->GetTechnique(i, &name);
	//	const char* szName;
	//	_shader_var var;
	//	if(mode->GetTextParameter(var, name, "techniqueName", &szName))
	//	{
	//		char szBuffer[512];
	//		strcpy(szBuffer, name);
	//		strlwr(szBuffer);
	//		Wutf8 w(szName);
	//		m_vspsTechniques[name]	=	w.buf;
	//		values.push_back(w.buf);
	//	}
	//}
	//m_subGeometryEditor.m_cbShaderVariants.ResetContent();
	//unsigned int numVar = mode->GetVariantCount();
	//for(unsigned int iVar = 0; iVar < numVar; iVar ++)
	//{
	//	const char* szName = 0;
	//	mode->GetVariantHandle(iVar, &szName);
	//	if(!szName)continue;
	//	const char* name = 0;
	//	const char* type = 0;
	//	if(!mode->GetTextParameter(szName, "name", &name))continue;
	//	int index	=	m_subGeometryEditor.m_cbShaderVariants.AddString( Wutf8(name) );
	//	//
	//	m_subGeometryEditor.m_cbShaderVariants.SetItemData(index, iVar);
	//}
}

const char*			GCViewGeometryEditor::GetShaderTechnique(const wchar_t* szText)
{
	for(std::map<std::string, std::wstring>::iterator it = m_vspsTechniques.begin(); it != this->m_vspsTechniques.end(); it++)
	{
		if(wcsicmp(it->second.c_str(), szText) == 0)
			return it->first.c_str();
	}
	return "tech_default";
}
VOID		GCViewGeometryEditor::CloseFile(){
	m_pGeometryPtr = 0;
}
BOOL		GCViewGeometryEditor::OpenFile(const wchar_t* szFile)	//打开文件,如果失败则会自动删除
{
	this->m_strFile	=	szFile;
	if(!__super::OpenFile(szFile))
		return false;
	//ImportResult result;
	//m_pGeometryPtr	=	LoadGeometry(m_packagePtr, A(m_strFile.c_str()), false, &result);
	Phantom::NameT<256> temp,proj;
	temp = A(szFile).buf;
	temp.setIsFile();
	const char* tempFile = temp.t;
	proj = GetProjectPath();
	proj.setIsFile();
	if(strnicmp(temp.t, proj.t, proj.size()) == 0)
		tempFile = temp.t + proj.size();
	Phantom::ImportResult result;
	if(!Phantom::g_manager.LoadMesh(m_pGeometryPtr, tempFile, &result))
	{
		if(result == Phantom::ImportResult_FailureAuth)
		{
			GetMainFrame()->MessageBox(Language(L"你目前还没有获得这个资源的授权,无法使用\n这个资源必须得获得授权才能使用!"), Language(L"提示"), MB_OK);
			return false;
		}
		//OutputTextError(L"%s", (const wchar_t*)Language(L"OpenGeometryFailure", L"打开模型文件失败，可能是数据错误!"));//, (const wchar_t*)strResult);
		return FALSE;
	}
	//OutputText(L"%s[%s]", (const wchar_t*)Language(L"OpenGeometryOK", L"打开模型文件成功!"), (const wchar_t*)m_strFile.c_str());
	SetModifyed(false);
	m_pGeometryPtr->SetPosition(Phantom::float3(0, 0, 0));
	Phantom::AABox box	=	m_pGeometryPtr->GetObjectRegion().aabox;
	Phantom::float3 vec	=	box._max - box._min;
	float r		=	vec.R();
	Phantom::GetCameraPtr()->GetCameraConfig(m_cameraSave);
	m_cameraSave.vEye.setxyz(r,r,r);
	this->m_bShowUIGrid = true;
	//
	if(m_subGeometryEditor.m_hWnd == 0)
		this->m_treeView.InsertItem(Language(L"GeometryEditorTree", L"模型编辑工具"), 0, 0);
	RebuildTrees();
	//
	if(this->m_pGeometryPtr->GetDataPtr()->m_actionIO.size()>0&&!m_pGeometryPtr->GetDataPtr()->m_bExistRegion){
		m_pGeometryPtr->RecalcIdleBox();
		this->SetModifyed(true);
	}
	m_pGeometryPtr->FrameMove();
	m_cameraSave.vTarget	=	Phantom::float3(0, 0, 0);
	float fSize	=	m_pGeometryPtr->GetObjectRegion().sphere.radius * 3.0f;
	m_cameraSave.vEye		=	Phantom::float3(fSize, fSize, fSize);
	m_cameraSave.SetEnable(Phantom::CameraEnable_FPSMode, false);
	m_cameraSave.SetEnable(Phantom::CameraEnable_EnableDistance, true);
	m_cameraSave.SetEnable(Phantom::CameraEnable_EnableMove,true);
	m_cameraSave.SetEnable(Phantom::CameraEnable_XRotation,true);
	m_cameraSave.SetEnable(Phantom::CameraEnable_YRotation,true);
	m_cameraSave.SetEnable(Phantom::CameraEnable_MoveHeightIsZ, false);
	m_cameraSave.fMoveSpeed	=	10.0f;
	m_cameraSave.fMaxDistance	=	1000000.0f;
	//
	if(m_subGeometryEditor.m_hWnd == 0){
		InitPropWnd(&this->m_subGeometryEditor, this->m_subGeometryEditor.IDD);
		m_subGeometryEditor.m_geomEditor	=	this;
		InitGroupProp();
		InitModelProp();
		InitShaderProp();
		InitMaterialProp();
		InitActionProp();
		InitPartProp();
		InitViewerProps();
	}
	//
	this->RebuildVSPS();
	return true;
}

VOID	GCViewGeometryEditor::RebuildLODs()
{
	//TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(m_treeView.GetSelectedItem()));
	//if(type && type->type == this->GEOMETRY_MODELS)
	//{
	//	Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
	//	int nEquipID	=	dataPtr->GetPartEquipID(type->p1, type->p2);
	//	Phantom::SubMeshData* equipPtr	=	dataPtr->GetPartEquipDataPtr(type->p1, nEquipID);
	//	//
	//}
}

VOID	GCViewGeometryEditor::RebuildShaderVars()
{
	RebuildLODs();
}

void	GCViewGeometryEditor::RebuildVSPS()
{
	//std::vector<std::wstring> types;
	//GetShaderTechnique(types);
	//m_shaderTechnique->RemoveAllOptions();
	//AddOptions(m_shaderTechnique, types);
}

VOID	GCViewGeometryEditor::InitPartProp()
{
	InitProp(m_partListProp);
	//
	CMFCPropertyGridProperty* pBase					= new CMFCPropertyGridProperty(Language(L"基本属性"));
	pBase->AddSubItem(m_propPartName			= AddPropText(Language(L"名称"), Language(L"名称_说明", L"设置装备组名称"), false, 120));
	//
	m_partListProp.AddProperty(pBase);
}

VOID	GCViewGeometryEditor::InitActionProp()	//动作属性
{
	InitProp(m_actionProps);
	//
	CMFCPropertyGridProperty* pBase					= new CMFCPropertyGridProperty(Language(L"基本属性"));
	pBase->AddSubItem(m_propActionName				= AddPropText(Language(L"名称"), Language(L"Geometry_AniName", L"动画名称"), false, 120));
	pBase->AddSubItem(m_propActionID				= AddPropInt(L"ID", Language(L"Geometry_AniID", L"动画ID")));
	m_propActionID->AllowEdit(false);
	pBase->AddSubItem(m_propActionSetDefault		= AddPropBool(Language(L"默认动画"), Language(L"Geometry_DefaultAni", L"设置成默认动画")));
	pBase->AddSubItem(m_propActionType				= AddPropOption(Language(L"动作类型"), Language(L"m_propActionType", L"设置动作类型。"), g_typeUnitActionType));
	pBase->AddSubItem(m_propActionKeyCount			= AddPropInt(Language(L"关键帧数量"), Language(L"Geometry_AniKeyCount", L"关键帧数量")));
	m_propActionKeyCount->AllowEdit(false);
	pBase->AddSubItem(m_propActionBeginKey			= AddPropInt(Language(L"循环-开始帧"), Language(L"Geometry_AniBeginKey", L"设置动画循环的开始帧")));
	pBase->AddSubItem(m_propActionEndKey			= AddPropInt(Language(L"循环-结束帧"), Language(L"Geometry_AniEndKey", L"设置动画循环的结束帧")));
	pBase->AddSubItem(m_propActionSpeed				= AddPropFloat(Language(L"播放速度"), Language(L"Geometry_AniPlaySpeed", L"设置动画的播放速度")));
	//pBase->AddSubItem(m_propActionRotation			= AddPropFloat(Language(L"增加转动方向"), Language(L"m_propActionRotation", L"设置动画的切换速度"), FloatType_Angle, -360.0f, 360.0f));
	pBase->AddSubItem(m_propActionRibbon			= AddPropBool(Language(L"刀光启用"), Language(L"m_propActionRibbon", L"设置刀光启用")));
	pBase->AddSubItem(m_propActionLoopMode			= AddPropBool(Language(L"循环动作"), Language(L"m_propActionLoopMode", L"设置循环动作")));
	pBase->AddSubItem(m_propActionSndFile			= AddPropFile(Language(L"音效文件"), Language(L"m_propActionSndFile", L"设置音效文件，并在攻击触发帧自动播放"), L"WAV文件(*.wav)|*.wav||"));
	pBase->AddSubItem(m_propActionSndClear			= AddPropButton(Language(L"去掉音效文件"), Language(L"m_propActionSndClear", L"去掉音效文件")));
	pBase->AddSubItem(m_propActionSndPlayKey		= AddPropInt(Language(L"事件触发帧"), Language(L"m_propActionSndFile", L"设置音效播放帧,-1不播放")));
	pBase->AddSubItem(m_propActionSndPlayKey2		= AddPropInt(Language(L"事件触发帧2"), Language(L"m_propActionSndFile2", L"设置音效播放帧2,-1不播放")));
	pBase->AddSubItem(m_propActionHit1				= AddPropInt(Language(L"被击触发帧"), Language(L"m_propActionHit1", L"被击触发帧,-1不触发")));
	pBase->AddSubItem(m_propActionMoveOffset		= AddPropFloat(Language(L"移动速度"), Language(L"m_propActionMoveOffset", L"移动速度"), false, -10000.0f, 10000.0f, 1.0f));
	m_actionProps.AddProperty(pBase);
}

VOID	GCViewGeometryEditor::InitMaterialProp()
{
	InitProp(m_materialProps);

	CMFCPropertyGridProperty* pBase				= new CMFCPropertyGridProperty(Language(L"基本属性"));
	//
	pBase->AddSubItem(m_propMtrlCullMode	= AddPropOption(Language(L"渲染面选择"), Language(L"Geometry_Cull", L"渲染A面还是B面还是都渲染"), g_typeCull));
	//
	pBase->AddSubItem(m_propMtrlAlphaMode	= AddPropOption(Language(L"透明模式"), Language(L"Geometry_AlphaMode", L"设置透明通道使用类型"), g_typeAlpha));
	//
	pBase->AddSubItem(m_propMtrlLodEnable			= AddPropBool(Language(L"LOD有效(针对材质)"), Language(L"Geometry_LodEnable", L"此项是否使用LOD模型，因为一些纹理模型中会出现错误现象")));
	pBase->AddSubItem(m_propMtrlVisible				= AddPropBool(Language(L"显示"), Language(L"Geometry_TextureGeomV", L"设置材质对应模型是否显示")));
	pBase->AddSubItem(m_propMtrlVisibleInShadowMap	= AddPropBool(Language(L"渲染到阴影图"), Language(L"Geometry_RenderToShadowMap", L"是否在shadowMap中进行显示,是否渲染到阴影图")));
	pBase->AddSubItem(m_propMtrlSrcBlend	= AddPropOption(Language(L"源混合方式"), Language(L"Geometry_SrcBlend", L"设置材质源混合方式"), g_typeBlend));
	pBase->AddSubItem(m_propMtrlDestBlend	= AddPropOption(Language(L"目标混合方式"), Language(L"Geometry_DestBlend", L"设置材质目标混合方式"), g_typeBlend));
	pBase->AddSubItem(m_propMtrlTransparent		= AddPropFloat(Language(L"透明值"), Language(L"Geometry_AlhpaValue", L"材质的透明值设置"), false, 0, 1, 0.1f));
	CMFCPropertyGridProperty* pDiffuse	= new CMFCPropertyGridProperty(Language(L"默认材质属性"));
	pDiffuse->AddSubItem(m_propMtrlTextureFile[Phantom::TEX_DIFFUSE]	= AddPropFile(Language(L"默认贴图"), Language(L"Geometry_DiffuseTexture", L"设置漫射光贴图"), GetImageFilter()));
	pDiffuse->AddSubItem(m_propMtrlDiffuseColor	= AddPropColor(Language(L"默认颜色"), Language(L"Geometry_DiffuseColor", L"慢反射光颜色设置")));
	for(int i=0;i<DIFFUSE_TEXTURE_COUNT;i++){
		wchar_t buf[522];
		swprintf(buf, L"%s %d", Language(L"默认贴图"), i+1);
		pDiffuse->AddSubItem(m_propMtrlDiffuseFile[i]	= AddPropFile(buf, Language(L"Geometry_DiffuseTexture", L"设置漫射光贴图"), GetImageFilter()));
		swprintf(buf, L"%s %d", Language(L"默认颜色"), i+1);
		pDiffuse->AddSubItem(m_propMtrlDiffuseColors[i]	= AddPropColor(buf, Language(L"Geometry_DiffuseColor", L"慢反射光颜色设置")));
		
	}

	CMFCPropertyGridProperty* pTexture	= new CMFCPropertyGridProperty(Language(L"其他材质属性"));
	pTexture->AddSubItem(m_propMtrlTextureFile[Phantom::TEX_SELF_ILLUMINATION]	= AddPropFile(Language(L"[自发光]贴图"), Language(L"Geometry_EmissiveTexture", L"设置自发光贴图"), GetImageFilter()));
	pTexture->AddSubItem(m_propMtrlEmissiveColor	= AddPropColor(Language(L"自发光色"), Language(L"Geometry_EmissiveColor", L"自发光颜色设置")));
	pTexture->AddSubItem(m_propMtrlTextureFile[Phantom::TEX_SPECULAR_MAP]	= AddPropFile(Language(L"[RGB高光]贴图"), Language(L"Geometry_SpecularTexture", L"设置RGB高光贴图"), GetImageFilter()));
	pTexture->AddSubItem(m_propMtrlTextureFile[Phantom::TEX_NORMAL_MAP]	= AddPropFile(Language(L"[法线]贴图"), Language(L"Geometry_NormalMap", L"设置法线贴图"), GetImageFilter()));
	pTexture->AddSubItem(m_propMtrlTextureFile[Phantom::TEX_REFLECTION]	= AddPropFile(Language(L"[反射]贴图"), Language(L"Geometry_ReflectMap", L"设置反射贴图"), GetImageFilter()));
	pTexture->AddSubItem(m_propMtrlTextureFile[Phantom::TEX_SHADOWMAP]	= AddPropFile(Language(L"[阴影]贴图"), Language(L"Geometry_ShadowMap", L"设置阴影贴图"), GetImageFilter()));
	wchar_t szDrive[256], szDir[256];
	_wsplitpath(this->m_strFile.c_str(), szDrive, szDir, 0, 0);
	wcscat(szDrive, szDir);
	for(int i=0;i<Phantom::TEXTURE_MAX_COUNT;i++)
	{
		m_propMtrlTextureFile[i]->AllowEdit(false);
		m_propMtrlTextureFile[i]->SetInitDir(szDrive);
	}
	//
	CMFCPropertyGridProperty* pTextureClear	= new CMFCPropertyGridProperty(Language(L"材质清空"));
	pTextureClear->AddSubItem(m_propMtrlTextureClear	= AddPropButton(Language(L"清空所有贴图"), Language(L"Clear_Geometry_DiffuseTexture", L"清空所有贴图")));
	//
	//
	m_materialProps.AddProperty(pBase);
	m_materialProps.AddProperty(pDiffuse);
	m_materialProps.AddProperty(pTexture);
	m_materialProps.AddProperty(pTextureClear);
	//m_modelProps.AddProperty(pBase);
	//m_modelProps.AddProperty(pDiffuse);
	//m_modelProps.AddProperty(pTexture);
	//m_modelProps.AddProperty(pTextureClear);
}

VOID	GCViewGeometryEditor::InitGroupProp()
{
	InitProp(m_groupProps);
	CMFCPropertyGridProperty* pGroup1				= new CMFCPropertyGridProperty(Language(L"基本属性"));
	//
	//{
	//	pGroup1->AddSubItem(m_propGroupShaderMode	= AddPropText(Language(L"着色器模式"), Language(L"Geometry_ShaderMode", L"设置着色器模式，在vspsMode.t中定义")));
	//	//
	//	std::vector<std::wstring> types;
	//	GetShaderMode(types);
	//	AddOptions(m_propGroupShaderMode, types);
	//	m_propGroupShaderMode->AllowEdit(FALSE);
	//}
	pGroup1->AddSubItem(m_propGroupClipVisible	= AddPropBool(Language(L"是否进行裁剪"), Language(L"m_propGroupClipVisible", L"设置模型是否进行裁剪，裁剪之后的模型可以大大增加渲染效率。")));
	//pGroup1->AddSubItem(m_groupActiveShader	= AddPropOption(Language(L"激活渲染方案"), Language(L"m_groupActiveShader", L"设置激活着的渲染方案。"), g_typeShaderType));
	//pGroup1->AddSubItem(m_groupPartsMode	= AddPropBool(Language(L"切换装备模式"), Language(L"m_groupPartsMode", L"设置切换装备模式。")));
	//pGroup1->AddSubItem(m_groupShaderCount	= AddPropInt(Language(L"渲染方案数量"), Language(L"m_groupShaderCount", L"设置激活着的渲染方案数量。"), 1, GeometryShaderType_Count));
	//pGroup1->AddSubItem(m_groupShowSelect	=	AddPropBool(Language(L"选择状态"), Language(L"m_groupShowSelect", L"设置当前显示选择状态")));
	pGroup1->AddSubItem(m_propModelInvertTurn	=	AddPropBool(Language(L"角色方向90度旋转"), Language(L"m_propModelInvertTurn", L"角色方向90度旋转")));
	//pGroup1->AddSubItem(m_propGroupScale	=	AddPropFloat(Language(L"角色基础缩放"), Language(L"m_propGroupScale", L"设置角色基础缩放大小"), 0, 0.001f, 10000.0f, 1.0f));
	pGroup1->AddSubItem(m_propGroupHeight	=	AddPropFloat(Language(L"角色高度(米)"), Language(L"m_propGroupHeight", L"设置角色个子高度"), true, 0.01f, 10000.0f, 1.0f));
	pGroup1->AddSubItem(m_propGroupHeightBase	=	AddPropFloat(Language(L"角色身高位置(米)"), Language(L"m_propGroupHeightBase", L"设置角色个子高度"), true, 0.01f, 10000.0f, 1.0f));
	pGroup1->AddSubItem(m_propGroupDiffuse	=	AddPropOption(Language(L"材质方案测试"), Language(L"m_propGroupDiffuse", L"材质方案测试"),g_typeGroupDiffuse));
	pGroup1->AddSubItem(m_propGroupSetCollision	=	AddPropButton(Language(L"重新计算动作盒子"), Language(L"m_propGroupSetCollision", L"设置成唯一碰撞模型，如果是骨骼动画才有效")));

	//m_propGroupHeight->AllowEdit(FALSE);
	//SetPropBool(m_groupShowSelect, false);
	CMFCPropertyGridProperty* pTable			= new CMFCPropertyGridProperty(Language(L"游戏属性设置"));
	//
	pTable->AddSubItem(m_propGroupType			= AddPropOption(Language(L"类型选择"), Language(L"GroupType", L"设置模型类型，类型不同在场景中创建的属性也不同"), g_typeGroupType));
	pTable->AddSubItem(m_propGroupTableLine		= AddPropOption(Language(L"表行选择"), Language(L"GroupTableLine", L"只有设置成了场景对象或者角色才能选择"), g_typeGroupType));
	Phantom::PtrID id(1,1);
	Phantom::Config* cfg = Phantom::GetGameMgr()->GetConfigPtr(id, "sceneunit_table");
	m_propGroupType->Enable(cfg!=0);
	m_propGroupTableLine->Enable(false);
	//
	CMFCPropertyGridProperty* pLod					= new CMFCPropertyGridProperty(Language(L"LOD设置"));
	//
	pLod->AddSubItem(m_propGroupLodRangeLength		= AddPropFloat(Language(L"LOD阶段距离倍数"), Language(L"Geometry_LODLength", L"设置一段LOD长度(对象半径乘这个值)，一个模型的所有LOD长度都是统一设置"), false, 0.001f, 100000.0f, 1.0f));
	pLod->AddSubItem(m_propGroupLodMaxEnable		= AddPropBool(Language(L"LOD最大距离有效"), Language(L"Geometry_LODEnable", L"LOD是否有效,设置LOD有效")));
	pLod->AddSubItem(m_propGroupLodMaxVisible		= AddPropFloat(Language(L"LOD最大可见距离(米)"), Language(L"Geometry_LODVisibleLen", L"设置最长可见距离,超过这个距离将会自动隐藏，主要用于大场景中的快速过滤渲染物体时使用"), true, 0.001f, 10000000.0f, 1.0f));
	//
	CMFCPropertyGridProperty* pShadow				= new CMFCPropertyGridProperty(Language(L"阴影相关"));
	pShadow->AddSubItem(m_propGroupRenderToShadow	= AddPropBool(Language(L"渲染到阴影图"), Language(L"Geometry_RenderToSMap", L"设置模型是否渲染到阴影图")));
	pShadow->AddSubItem(m_propGroupByShadowMap		= AddPropBool(Language(L"受到阴影图影响"), Language(L"Geometry_ShadowIn", L"设置模型是否受到阴影图的影响，被影射")));
	pShadow->AddSubItem(m_propGroupShadowEpsilon	= AddPropFloat(Language(L"渲染阴影精度"), Language(L"Geometry_ShadowElipse", L"设置模型渲染到阴影图的时候的精度"), false, 0.000000001f, 10.0f, .0000001f));
	//
	pShadow->AddSubItem(m_propGroupShadowVisible	= AddPropBool(Language(L"使用假阴影"), Language(L"m_propGroupShadowVisible", L"设置模型是否使用假阴影")));
	pShadow->AddSubItem(m_propGroupShadowSize		= AddPropFloat(Language(L"假阴影大小"), Language(L"m_propGroupShadowSize", L"设置模型假阴影大小"), false, 0.01f, 100.0f, 0.1f));
	pShadow->AddSubItem(m_propGroupShadowUVOffsetX		= AddPropFloat(Language(L"假阴影X偏移"), Language(L"m_propGroupShadowUVOffsetY", L"设置模型假阴影X偏移"), false, -1.0f, 1.0f, 0.1f));
	pShadow->AddSubItem(m_propGroupShadowUVOffsetY		= AddPropFloat(Language(L"假阴影Y偏移"), Language(L"m_propGroupShadowUVOffsetY", L"设置模型假阴影Y偏移"), false, -1.0f, 1.0f, 0.1f));
	pShadow->AddSubItem(m_propGroupShadowTexture		= AddPropFile(Language(L"假阴影贴图"), Language(L"m_propGroupShadowTexture", L"设置模型假阴影贴图"), GetImageFilter()));
	pShadow->AddSubItem(m_propGroupShadowTextureClear		= AddPropButton(Language(L"假阴影贴图=默认"), Language(L"m_propGroupShadowTextureClear", L"设置模型假阴影贴图=默认")));
	//
	m_groupProps.AddProperty(pGroup1);
	m_groupProps.AddProperty(pTable);
	m_groupProps.AddProperty(pLod);
	m_groupProps.AddProperty(pShadow);
}

VOID	GCViewGeometryEditor::InitShaderProp()
{
	InitProp(m_shaderProps);
	//
	CMFCPropertyGridProperty* pShader			=	new CMFCPropertyGridProperty(Language(L"着色器"));
	//{
	//	CMFCPropertyGridProperty* pProp = m_shaderTechnique = AddPropText(Language(L"着色方案"), Language(L"Geometry_ShaderModes", L"设置默认着色方案"));
	//	pProp->AllowEdit(FALSE);
	//	pShader->AddSubItem(pProp);
	//}
	//pShader->AddSubItem(m_shaderForceAlpha		=	AddPropBool(Language(L"强制透明"), Language(L"m_shaderForceAlpha", L"设置渲染方案是否强制透明")));
	//pShader->AddSubItem(m_shaderSrcBlend		=	AddPropOption(Language(L"源混合方式"), Language(L"m_shaderSrcBlend", L"设置渲染方案源混合方式"), g_typeBlend));
	//pShader->AddSubItem(m_shaderDestBlend		=	AddPropOption(Language(L"目标混合方式"), Language(L"m_shaderDestBlend", L"设置渲染方案目标混合方式"), g_typeBlend));
	//pShader->AddSubItem(m_shaderCullMode		=	AddPropOption(Language(L"渲染面"), Language(L"m_shaderCullMode", L"设置渲染方案渲染面选择"), g_typeCull));
	//m_shaderProps.AddProperty(pShader);
	//
	InitProp(m_ribbonEffect);
	CMFCPropertyGridProperty* pRibbon			=	new CMFCPropertyGridProperty(Language(L"刀光特效"));
	{
		CMFCPropertyGridProperty* pProp = m_ribbonBindBone = AddPropText(Language(L"选择骨骼"), Language(L"select_ribbon_bone", L"选择骨骼"));
		pProp->AllowEdit(FALSE);
		pRibbon->AddSubItem(pProp);
	}
	{
		CMFCPropertyGridProperty* pProp = m_ribbonBindBone2 = AddPropText(Language(L"选择骨骼2"), Language(L"select_ribbon_bone2", L"选择骨骼2"));
		pProp->AllowEdit(FALSE);
		pRibbon->AddSubItem(pProp);
	}
	pRibbon->AddSubItem(m_ribbonTexture			=	AddPropFile(Language(L"[刀光]贴图"), Language(L"ribbon_texture", L"设置刀光贴图"), GetImageFilter()));
	pRibbon->AddSubItem(m_ribbonLiveSecond		=	AddPropInt(Language(L"产生次数"), Language(L"ribbon_livesecond", L"产生次数")));
	pRibbon->AddSubItem(m_ribbonAlpha			=	AddPropFloat(Language(L"半透明"), Language(L"m_ribbonSrcBlend", L"半透明"), 0, 0, 1));
	pRibbon->AddSubItem(m_ribbonSrcBlend		=	AddPropOption(Language(L"源混合方式"), Language(L"m_ribbonSrcBlend", L"设置渲染方案源混合方式"), g_typeBlend));
	pRibbon->AddSubItem(m_ribbonDestBlend		=	AddPropOption(Language(L"目标混合方式"), Language(L"m_ribbonDestBlend", L"设置渲染方案目标混合方式"), g_typeBlend));

	m_ribbonEffect.AddProperty(pRibbon);
	//
	InitProp(m_particleProps);
	{
		CMFCPropertyGridProperty* pPariticle			=	new CMFCPropertyGridProperty(Language(L"基本属性"));
		{
			CMFCPropertyGridProperty* pProp = m_particleBindBone = AddPropText(Language(L"选择骨骼"), Language(L"m_particleBindBone", L"选择骨骼"));
			pProp->AllowEdit(FALSE);
			pPariticle->AddSubItem(pProp);
		}
		pPariticle->AddSubItem(m_particleTexture		=	AddPropFile(Language(L"[粒子]贴图"), Language(L"m_particleTexture", L"设置粒子贴图"), GetImageFilter()));
		pPariticle->AddSubItem(m_particleEnable			=	AddPropBool(Language(L"有效"), Language(L"m_particleEnable", L"有效")));
		pPariticle->AddSubItem(m_particleLifeSpan		=	AddPropFloat(Language(L"生命周期(秒)"), Language(L"m_particleLifeSpan", L"生命周期(秒)"), 0, 0, 1000));
		pPariticle->AddSubItem(m_particleMiddleTime		=	AddPropFloat(Language(L"相对时间(秒)"), Language(L"m_particleMiddleTime", L"相对时间(秒)"), 0, 0, 1000));
		pPariticle->AddSubItem(m_particleSpeed			=	AddPropFloat(Language(L"发射速度"), Language(L"m_particleSpeed", L"发射速度"), 0, -1000, 1000));
		pPariticle->AddSubItem(m_particleVariation		=	AddPropFloat(Language(L"发射速度(随机差)"), Language(L"m_particleVariation", L"发射速度(随机差)"), 0, -1000, 1000));
		pPariticle->AddSubItem(m_particleConeAngle		=	AddPropFloat(Language(L"喷射夹角"), Language(L"m_particleConeAngle", L"喷射夹角"), 0, -1000, 1000));
		pPariticle->AddSubItem(m_particleEmissionRate	=	AddPropFloat(Language(L"每秒创建粒子数"), Language(L"m_particleEmissionRate", L"每秒钟创建粒子数目"), 0, 0, 1000));
		pPariticle->AddSubItem(m_particleTailLength		=	AddPropFloat(Language(L"拖尾长度"), Language(L"m_particleTailLength", L"拖尾长度")));
		pPariticle->AddSubItem(m_particleWidth			=	AddPropFloat(Language(L"面板宽度"), Language(L"m_particleWidth", L"面板宽度"), 0, -1000, 1000));
		pPariticle->AddSubItem(m_particleHeight			=	AddPropFloat(Language(L"面板高度"), Language(L"m_particleHeight", L"面板高度"), 0, -1000, 1000));
		pPariticle->AddSubItem(m_particleSrcBlend		=	AddPropOption(Language(L"源混合方式"), Language(L"m_particleSrcBlend", L"设置渲染方案源混合方式"), g_typeBlend));
		pPariticle->AddSubItem(m_particleDestBlend		=	AddPropOption(Language(L"目标混合方式"), Language(L"m_particleDestBlend", L"设置渲染方案目标混合方式"), g_typeBlend));
		//
		CMFCPropertyGridProperty* pUV					=	new CMFCPropertyGridProperty(Language(L"UV动画属性"));
		pUV->AddSubItem(m_particleRow					=	AddPropInt(Language(L"贴图材质左右数"), Language(L"m_particleRow", L"贴图材质左右数")));
		pUV->AddSubItem(m_particleCol					=	AddPropInt(Language(L"贴图材质上下数"), Language(L"m_particleCol", L"贴图材质上下数")));
		pUV->AddSubItem(m_particleUVAnimFps				=	AddPropInt(Language(L"UV动画播放速度"), Language(L"m_particleUVAnimFps", L"UV动画播放速度")));
		//
		CMFCPropertyGridProperty* pColor					=	new CMFCPropertyGridProperty(Language(L"颜色控制"));
		pColor->AddSubItem(m_particleColor[0]				=	AddPropColor(Language(L"颜色[产生]"), Language(L"m_particleColor0", L"颜色[产生]")));
		pColor->AddSubItem(m_particleColor[1]				=	AddPropColor(Language(L"颜色[开始]"), Language(L"m_particleColor1", L"颜色[开始]")));
		pColor->AddSubItem(m_particleColor[2]				=	AddPropColor(Language(L"颜色[终结]"), Language(L"m_particleColor2", L"颜色[终结]")));
		//
		CMFCPropertyGridProperty* pAlpha					=	new CMFCPropertyGridProperty(Language(L"透明控制"));
		pAlpha->AddSubItem(m_particleAlpha[0]				=	AddPropFloat(Language(L"透明[产生]"), Language(L"m_particleAlpha0", L"透明[产生]"), 0, 0, 1));
		pAlpha->AddSubItem(m_particleAlpha[1]				=	AddPropFloat(Language(L"透明[开始]"), Language(L"m_particleAlpha1", L"透明[开始]"), 0, 0, 1));
		pAlpha->AddSubItem(m_particleAlpha[2]				=	AddPropFloat(Language(L"透明[终结]"), Language(L"m_particleAlpha2", L"透明[终结]"), 0, 0, 1));
		//
		CMFCPropertyGridProperty* pScaling					=	new CMFCPropertyGridProperty(Language(L"缩放控制"));
		pScaling->AddSubItem(m_particleScaling[0]				=	AddPropFloat(Language(L"缩放[产生]"), Language(L"m_particleScaling0", L"缩放[产生]"), 0, -1000, 1000));
		pScaling->AddSubItem(m_particleScaling[1]				=	AddPropFloat(Language(L"缩放[开始]"), Language(L"m_particleScaling1", L"缩放[开始]"), 0, -1000, 1000));
		pScaling->AddSubItem(m_particleScaling[2]				=	AddPropFloat(Language(L"缩放[终结]"), Language(L"m_particleScaling2", L"缩放[终结]"), 0, -1000, 1000));
		//
		CMFCPropertyGridProperty* SpanUVAnim					=	new CMFCPropertyGridProperty(Language(L"UV生命周期"));
		SpanUVAnim->AddSubItem(m_particleLifeSpanUVAnim[0]				=	AddPropInt(Language(L"UV生命周期[产生]"), Language(L"m_particleLifeSpanUVAnim0", L"UV生命周期[产生]")));
		SpanUVAnim->AddSubItem(m_particleLifeSpanUVAnim[1]				=	AddPropInt(Language(L"UV生命周期[开始]"), Language(L"m_particleLifeSpanUVAnim1", L"UV生命周期[开始]")));
		SpanUVAnim->AddSubItem(m_particleLifeSpanUVAnim[2]				=	AddPropInt(Language(L"UV生命周期[终结]"), Language(L"m_particleLifeSpanUVAnim2", L"UV生命周期[终结]")));
		//
		CMFCPropertyGridProperty* DecayUVAnim					=	new CMFCPropertyGridProperty(Language(L"UV衰败周期"));
		DecayUVAnim->AddSubItem(m_particleDecayUVAnim[0]				=	AddPropInt(Language(L"UV衰败周期[产生]"), Language(L"m_particleDecayUVAnim0", L"UV衰败周期[产生]")));
		DecayUVAnim->AddSubItem(m_particleDecayUVAnim[1]				=	AddPropInt(Language(L"UV衰败周期[开始]"), Language(L"m_particleDecayUVAnim1", L"UV衰败周期[开始]")));
		DecayUVAnim->AddSubItem(m_particleDecayUVAnim[2]				=	AddPropInt(Language(L"UV衰败周期[终结]"), Language(L"m_particleDecayUVAnim2", L"UV衰败周期[终结]")));
		//
		CMFCPropertyGridProperty* pPivot					=	new CMFCPropertyGridProperty(Language(L"中心位置"));
		pPivot->AddSubItem(m_particlePivot[0]				=	AddPropFloat(Language(L"位置[产生]"), Language(L"m_particlePivot0", L"位置[产生]"), 0, -1000, 1000));
		pPivot->AddSubItem(m_particlePivot[1]				=	AddPropFloat(Language(L"位置[开始]"), Language(L"m_particlePivot1", L"位置[开始]"), 0, -1000, 1000));
		pPivot->AddSubItem(m_particlePivot[2]				=	AddPropFloat(Language(L"位置[终结]"), Language(L"m_particlePivot2", L"位置[终结]"), 0, -1000, 1000));
		//
		CMFCPropertyGridProperty* PlaneNormal					=	new CMFCPropertyGridProperty(Language(L"粒子面法线"));
		PlaneNormal->AddSubItem(m_particlePlaneNormal[0]				=	AddPropFloat(Language(L"粒子面法线[产生]"), Language(L"m_particlePlaneNormal0", L"粒子面法线[产生]"), 0, 0, 1));
		PlaneNormal->AddSubItem(m_particlePlaneNormal[1]				=	AddPropFloat(Language(L"粒子面法线[开始]"), Language(L"m_particlePlaneNormal1", L"粒子面法线[开始]"), 0, 0, 1));
		PlaneNormal->AddSubItem(m_particlePlaneNormal[2]				=	AddPropFloat(Language(L"粒子面法线[终结]"), Language(L"m_particlePlaneNormal2", L"粒子面法线[终结]"), 0, 0, 1));
		//
		m_particleProps.AddProperty(pPariticle);
		m_particleProps.AddProperty(pUV);
		m_particleProps.AddProperty(pColor);
		m_particleProps.AddProperty(pAlpha);
		m_particleProps.AddProperty(pScaling);
		m_particleProps.AddProperty(SpanUVAnim);
		m_particleProps.AddProperty(DecayUVAnim);
		m_particleProps.AddProperty(pPivot);
		m_particleProps.AddProperty(PlaneNormal);
	}
}

VOID	GCViewGeometryEditor::InitModelProp()
{
	InitProp(m_modelProps);
	//
	CMFCPropertyGridProperty* pGroup1			=	new CMFCPropertyGridProperty(Language(L"基本属性"));
	pGroup1->AddSubItem(m_propModelName			=	AddPropText(Language(L"模型名称"), Language(L"Geometry_MeshName", L"模型的名称显示"), false, 120));
	pGroup1->AddSubItem(m_propModelID			=	AddPropInt(Language(L"模型ID"), Language(L"m_propModelID", L"模型的ID显示")));
	m_propModelID->AllowEdit(false);
	pGroup1->AddSubItem(m_propModelVisible		=	AddPropBool(Language(L"显示"), Language(L"Geometry_Visible", L"设置模型是否显示")));
	pGroup1->AddSubItem(m_propModelZWrite		=	AddPropBool(Language(L"Z缓冲区开关"), Language(L"Geometry_ZEnable", L"设置模型是否开启Z缓冲区写入,如果关闭Z缓冲区则不会对透明物体进行排序")));
	pGroup1->AddSubItem(m_propModelToShadow		=	AddPropBool(Language(L"渲染到阴影图"), Language(L"m_propModelToShadow", L"模型是否渲染到阴影图")));
	pGroup1->AddSubItem(m_propModelWithShadow	=	AddPropBool(Language(L"接受阴影映射"), Language(L"m_propModelWithShadow", L"模型是否接受阴影图的影响")));
	pGroup1->AddSubItem(m_propModelIsCollision	=	AddPropBool(Language(L"是否作为碰撞模型"), Language(L"m_propModelIsCollision", L"是否作为碰撞模型")));
	//pGroup1->AddSubItem(m_propModelUseColor		=	AddPropBool(Language(L"存在颜色"), Language(L"Geometry_VertexColor", L"设置模型是否存在顶点颜色")));
	//pGroup1->AddSubItem(m_propModelSelfAni		=	AddPropBool(Language(L"自身动作有效"), Language(L"Geometry_SelfAni", L"设置模型自身动作是否有效")));
	//pGroup1->AddSubItem(m_propModelGenTangent	=	AddPropBool(Language(L"构造法线、切线"), Language(L"Geometry_GenTangent", L"在顶点内构造法线或者删除法线信息")));
	//pGroup1->AddSubItem(m_propModelHighlight	=	AddPropBool(Language(L"支持高亮渲染"), Language(L"Geometry_SupportSpecular", L"支持全屏幕高亮渲染")));
	pGroup1->AddSubItem(m_propModelSetCollision	=	AddPropButton(Language(L"重新计算盒子"), Language(L"m_propModelSetCollision", L"设置成唯一碰撞模型，如果是骨骼动画才有效")));
	//
	m_modelProps.AddProperty(pGroup1);
	//
	CMFCPropertyGridProperty* pLod	=	new CMFCPropertyGridProperty(Language(L"LOD设置"));
	m_modelProps.AddProperty(pLod);
	pLod->AddSubItem(m_propModelGenLOD	=	AddPropBool(Language(L"创建LOD"), Language(L"Geometry_GenLOD", L"创建或者删除LOD模型")));
	for(int i=0;i<MAX_LOD_LEVEL_COUNT;i++)
	{
		wchar_t buf[128];
		swprintf(buf, L"%s%d", Language(L"LOD等级"), i);
		pLod->AddSubItem(m_propModelLODLevel[i]	=	AddPropText(buf, Language(L"Geometry_LODLevel", L"设置LOD等级的面数量"), false, 120));
		m_propModelLODLevel[i]->AllowEdit(false);
	}
	CMFCPropertyGridProperty* pShowOn	=	new CMFCPropertyGridProperty(Language(L"仅在以下动作播放时显示"));
	m_modelProps.AddProperty(pShowOn);
	for(int i=0;i<MAX_SHOW_ON_ACTION;i++)
	{
		wchar_t buf[128];
		swprintf(buf, L"%s%d", Language(L"动作类型"), i+1);
		pLod->AddSubItem(m_propModelOnAction[i]	=	AddPropOption(buf, Language(L"m_propModelOnAction", L"播放这个动作的时候会自动显示此模型"), g_typeUnitActionType));
	}
	//
	CMFCPropertyGridProperty* pRuntime	=	new CMFCPropertyGridProperty(Language(L"运行时测试"));
	m_modelProps.AddProperty(pRuntime);
	pRuntime->AddSubItem(m_propModelLodVisible	=	AddPropOption(Language(L"LOD显示等级"), Language(L"Geometry_LodLevel", L"设置当前显示的LOD等级"), g_typeLodLevel));
	//
	//pGroup1			=	new CMFCPropertyGridProperty(Language(L"通用材质过滤"));
	//pGroup1->AddSubItem(m_propModelMinFilter = AddPropOption(Language(L"缩小过滤"), Language(L"Geometry_MinFilter", L"设置缩小过滤方式，线形过滤或者点过滤"), g_typeFilter));
	//pGroup1->AddSubItem(m_propModelMagFilter = AddPropOption(Language(L"放大过滤"), Language(L"Geometry_MaxFilter", L"设置放大过滤方式，线形过滤或者点过滤"), g_typeFilter));
	//long n = 0;
	//pGroup1->AddSubItem(m_propModelMaxAnisotropy = AddPropInt(Language(L"最大过滤值"), Language(L"Geometry_MaxAniso", L"当过滤模式为各向异性过滤时，设置最大过滤值"), 0, 8, 1));
	//pGroup1->AddSubItem(m_propModelMipmapBias = AddPropFloat(Language(L"多层混合权重"), Language(L"Geometry_ModelMipmapBias", L"当过滤模式为多层过滤时，设置多层过滤权重"), false, -2.0f, 2.0f));
	//
	//m_modelProps.AddProperty(pGroup1);
	//
	CMFCPropertyGridProperty* pInfo				=	new CMFCPropertyGridProperty(Language(L"信息"));
	m_modelProps.AddProperty(pInfo);
	pInfo->AddSubItem(m_propModelVertexCount	=	AddPropInt(Language(L"顶点数量"), Language(L"Geometry_VertexCount", L"返回子模型中使用的顶点数量")));
	m_propModelVertexCount->Enable(false);
	pInfo->AddSubItem(m_propModelFaceCount		=	AddPropInt(Language(L"面数量"), Language(L"Geometry_FaceCount", L"返回子模型中使用的面数量")));
	m_propModelFaceCount->Enable(false);
	pInfo->AddSubItem(m_propModel32BitMesh		=	AddPropBool(Language(L"32位索引缓冲区"), Language(L"Geometry_32BitVertex", L"是否是32位索引缓冲区模型")));
	m_propModel32BitMesh->Enable(false);
	pInfo->AddSubItem(m_propModelIsBlend		=	AddPropBool(Language(L"存在骨骼动画"), Language(L"Geometry_ExistBoneAni", L"设置模型自身是否存在骨骼动画")));
	m_propModelIsBlend->Enable(false);
}

VOID	GCViewGeometryEditor::RebuildTrees()	//重新构造树型列表
{
	if(m_hViewItem)
		m_treeView.DeleteItem(m_hViewItem);
	wchar_t wFileName[256], wExt[256];
	_wsplitpath(m_strFile.c_str(), 0, 0, wFileName, wExt);
	wcscat(wFileName, wExt);
	m_hViewItem = m_treeView.InsertItem(wFileName, 1, 1);
	m_treeView.SetItemState(m_hViewItem, TVIS_BOLD, TVIS_BOLD);
	m_treeView.Expand(m_hViewItem, TVE_EXPAND);
	Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
	HTREEITEM hGeoms	=	m_hGeometryList = m_treeView.InsertItem(Language(L"模型列表"), 2, 2, m_hViewItem);
	this->ClearTreeItem();
	m_treeView.SetItemData(hGeoms, AddTreeItem(GEOMETRY_MODEL_LIST));
	BuildGeometrys();
	m_treeView.Expand(hGeoms, TVE_EXPAND);
	//
	HTREEITEM hActions	=	m_hActionList = m_treeView.InsertItem(Language(L"动画列表"), 8, 8, m_hViewItem);
	m_treeView.SetItemData(m_hActionList, AddTreeItem(this->GEOMETRY_ACTION_LIST));
	BuildActionList();
	m_treeView.Expand(hActions, TVE_EXPAND);
	//
	HTREEITEM hBinds	=	m_hRibbonList = m_treeView.InsertItem(Language(L"刀光列表"), 6, 6, m_hViewItem);
	m_treeView.SetItemData(m_hRibbonList, AddTreeItem(this->GEOMETRY_RIBBON_LIST));
	BuildRibbonList();
	//
	HTREEITEM hParticles	=	m_hParticleList = m_treeView.InsertItem(Language(L"粒子列表"), 6, 6, m_hViewItem);
	m_treeView.SetItemData(m_hParticleList, AddTreeItem(this->GEOMETRY_PARTICLE_LIST));
	BuildParticleList();

	//for(int iBind = 0;iBind < m_pGeometryPtr->GetDataPtr()->GetBindObjectCount();iBind++)
	//{
	//	BindPluginData* pBind	=	m_pGeometryPtr->GetDataPtr()->GetBindObjectData(iBind);
	//	wchar_t szBind[128];
	//	_swprintf(szBind, L"%s[ID:%d]", Language(L"临时绑定"), iBind);
	//	HTREEITEM hBind	=	m_treeView.InsertItem(szBind, 7, 7, hBinds);
	//	m_treeView.SetItemData(hBind, AddTreeItem(this->GEOMETRY_BIND, iBind));
	//}
	m_treeView.m_callback	=	this;
}

VOID		GCViewGeometryEditor::BuildGeometrys()
{
	m_treeView.SelectItem(m_hGeometryList);
	DeleteChildItems(m_hGeometryList);
	Phantom::MeshData*	dataPtr	=	m_pGeometryPtr->GetDataPtr();
	//if(this->m_bPartMode)
	//{
	//for(int iGeom=0;iGeom<m_pGeometryPtr->GetSubmeshCount();iGeom++)
	//{
	//	HTREEITEM hGeomOne	=	m_treeView.InsertItem(Wutf8(dataPtr->GetPartName(iGeom)), 3, 3, m_hGeometryList);
	//	m_treeView.SetItemData(hGeomOne, AddTreeItem(this->GEMOETRY_PART_TYPE, iGeom));
	//	int equipCount		=	dataPtr->GetPartEquipCount(iGeom);
	//	for(int eq = 0;eq < equipCount; eq++)
	//	{
	//		int nEquipID	=	dataPtr->GetPartEquipID(iGeom, eq);
	//		Phantom::SubMeshData* equipPtr	=	dataPtr->GetPartEquipDataPtr(iGeom, nEquipID);
	//		HTREEITEM hEquip	=	m_treeView.InsertItem(Wutf8(equipPtr->GetName()), 4, 4, hGeomOne);
	//		m_treeView.SetItemData(hEquip, AddTreeItem(this->GEOMETRY_MODELS, iGeom, eq));
	//		for(int mt=0;mt<equipPtr->GetMaterialCount();mt++)
	//		{
	//			wchar_t szMtrl[128];
	//			_swprintf(szMtrl, L"%s#%d", Language(L"材质"), mt + 1);
	//			HTREEITEM hMaterial	=	m_treeView.InsertItem(szMtrl, 5, 5, hEquip);
	//			m_treeView.SetItemData(hMaterial, AddTreeItem(this->GEOMETRY_MATERIAL, iGeom, eq, mt));
	//		}
	//		//HTREEITEM hShaders	=	m_treeView.InsertItem(Language(L"渲染方案"), 27, 27, hEquip);
	//		//m_treeView.SetItemData(hShaders, AddTreeItem(GEOMETRY_SHADER_LIST, iGeom, eq));
	//		//RefreshShaders(equipPtr, iGeom, hShaders);
	//		//for(int mt=0;mt<dataPtr->GetShaderCount();mt++)
	//		//{
	//		//	HTREEITEM hShader	=	m_treeView.InsertItem(Wutf8(equipPtr->GetShaderPtr((Phantom::MeshShaderType)mt)->GetName()).buf, 27, 27, hEquip);
	//		//	m_treeView.SetItemData(hShader, AddTreeItem(this->GEOMETRY_SHADER, iGeom, equipPtr->GetID(), mt));
	//		//}
	//		//HTREEITEM hShader	=	m_treeView.InsertItem(Wutf8(equipPtr->GetShaderPtr(Phantom::MeshShaderType_Default)->GetName()).buf, 27, 27, hEquip);
	//		//m_treeView.SetItemData(hShader, AddTreeItem(this->GEOMETRY_SHADER, iGeom, equipPtr->GetID(), GeometryShaderType_Select));
	//	}
	//}
	//}
	//else
	for(int iGeom=0;iGeom<m_pGeometryPtr->GetSubmeshCount();iGeom++)
	{
		//HTREEITEM hGeomOne	=	m_treeView.InsertItem(Wutf8(dataPtr->GetPartName(iGeom)), 3, 3, m_hGeometryList);
		//m_treeView.SetItemData(hGeomOne, AddTreeItem(this->GEMOETRY_PART_TYPE, iGeom));
		Phantom::SubMeshData* equipPtr	=	dataPtr->GetSubmeshPtr(iGeom);
		HTREEITEM hEquip	=	m_treeView.InsertItem(Wutf8(equipPtr->GetName()), 4, 4, m_hGeometryList);
		m_treeView.SetItemData(hEquip, AddTreeItem(this->GEOMETRY_MODELS, iGeom, 0));
		for(int mt=0;mt<equipPtr->GetMaterialCount();mt++)
		{
			if(equipPtr->GetMaterialPtr(mt)->name.t[0]==0)
			{
				wchar_t szMtrl[128];
				_swprintf(szMtrl, L"%s#%d", Language(L"材质"), mt + 1);
				equipPtr->GetMaterialPtr(mt)->name = Autf8(szMtrl);
			}
			HTREEITEM hMaterial	=	m_treeView.InsertItem(Wutf8(equipPtr->GetMaterialPtr(mt)->name.t), 5, 5, hEquip);
			m_treeView.SetItemData(hMaterial, AddTreeItem(this->GEOMETRY_MATERIAL, iGeom, 0, mt));
		}
	}
}
VOID		GCViewGeometryEditor::RefreshShaders(Phantom::SubMeshData* s, INT iGeom, HTREEITEM hItem)
{
}
VOID		GCViewGeometryEditor::BuildRibbonList()
{
	m_treeView.SelectItem(m_hRibbonList);
	DeleteChildItems(m_hRibbonList);
	for(int iR=0;iR<m_pGeometryPtr->GetDataPtr()->m_ribbons.size();iR++)
	{
		wchar_t buf[256];
		swprintf(buf, L"%s_%d", Language(L"刀光"), iR);
		HTREEITEM hRibbon	=	m_treeView.InsertItem(buf, 7, 7, m_hRibbonList);
		m_treeView.SetItemData(hRibbon, AddTreeItem(this->GEOMETRY_RIBBON_BIND, iR));
	}
	m_treeView.Invalidate();
}
VOID		GCViewGeometryEditor::BuildParticleList()
{
	m_treeView.SelectItem(m_hParticleList);
	DeleteChildItems(m_hParticleList);
	for(int iR=0;iR<m_pGeometryPtr->GetDataPtr()->GetParticleCount();iR++)
	{
		wchar_t buf[256];
		swprintf(buf, L"%s_%d", Language(L"粒子"), iR);
		HTREEITEM hParticle	=	m_treeView.InsertItem(buf, 7, 7, m_hParticleList);
		m_treeView.SetItemData(hParticle, AddTreeItem(this->GEOMETRY_PARTICLE_BIND, iR));
	}
	m_treeView.Invalidate();
}

VOID		GCViewGeometryEditor::BuildActionList()
{
	m_treeView.SelectItem(m_hActionList);
	DeleteChildItems(m_hActionList);
	for(int iAct=0;iAct<m_pGeometryPtr->GetDataPtr()->GetActionCount();iAct++)
	{
		int nID	=	m_pGeometryPtr->GetDataPtr()->GetActionID(iAct);
		Phantom::MeshActionRange* act	=	m_pGeometryPtr->GetDataPtr()->GetActionPtr(nID);
		char szAct[128];
		sprintf(szAct, "%s", act->GetName());
		HTREEITEM hAct	=	m_treeView.InsertItem(Wutf8(szAct), 9, 9, m_hActionList);
		m_treeView.SetItemData(hAct, AddTreeItem(this->GEOMETRY_ACTION, nID));
	}
	m_treeView.Invalidate();
}

VOID		GCViewGeometryEditor::OnActive()		//被激活
{
	UsePhantom3D(true);
	Phantom::GetCameraPtr()->m_bOffsetEnable=false;
	Phantom::GetCameraPtr()->m_bArea = false;
	Phantom::GetCameraPtr()->GetCameraData().SetEnable(Phantom::CameraEnable_EnableDistance, true);
	Phantom::GetCameraPtr()->GetCameraData().SetEnable(Phantom::CameraEnable_EnableMove, true);
	Phantom::GetCameraPtr()->GetCameraData().SetEnable(Phantom::CameraEnable_ReturnXRot, true);
	Phantom::GetCameraPtr()->GetCameraData().SetEnable(Phantom::CameraEnable_ReturnYRot, true);
	__super::OnActive();
}

VOID		GCViewGeometryEditor::OnDeActive()		//失去激活
{
	UsePhantom3D(false);
	__super::OnDeActive();
}
bool		GCViewGeometryEditor::OnWindowProc(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam)
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(hWnd, &pos);
	if(uMsg==WM_LBUTTONDOWN)
	{
		if(this->m_bAutoMoveMode)
		{
			Phantom::IntersectData* iter = Phantom::GetCameraPtr()->BeginIntersect(pos.x, pos.y);
			Phantom::float3 orig = iter->GetOriginal();
			Phantom::float3 dir = iter->GetDir();
			dir.x *= orig.z/dir.z;
			dir.y *= orig.z/dir.z;
			dir.z = 0;
			orig -= dir;
			orig.z = 0;
			Phantom::float3 mypos = m_pGeometryPtr->GetPosition();
			m_moveDir = orig - mypos;
			m_moveOrig		=	mypos;
			m_bMoveDir		=	true;
			m_fMoveTime		=	0;
			m_fMoveLength	=	m_moveDir.length();
			m_moveDir.normalize();
			if(m_pGeometryPtr->GetCurrentAction())
			{
				switch(m_pGeometryPtr->GetCurrentAction()->m_actionType){
				case Phantom::ActionType_Walk:
				case Phantom::ActionType_Run:
					m_pGeometryPtr->MoveTo(orig, m_pGeometryPtr->GetCurrentAction()->m_actionType);
					break;
				default:
					m_pGeometryPtr->MoveTo(orig, ActionType_Walk);
					break;
				}
			}
			//this->m_pGeometryPtr->SetActionType(Phantom::ActionType_Walk, false, Phantom::ActionType_Walk);
			//Phantom::matrix4x4 mat;
			//matrixRotationZ(&mat, getRotateZ(m_moveDir)-PI);
			//mat.SetPosition(mypos);
			//m_pGeometryPtr->SetWorldMatrix(mat);
		}
	}
	return false;
}
void		GCViewGeometryEditor::OnFrameMove()	//每次更新
{
//	GetProjectPtr()->m_fZFar	=	To3DValue(10000.0f);
	if(m_pGeometryPtr)
	{
		//if(!m_bAutoMoveMode)
		//	m_pGeometryPtr->SetWorldMatrix(Phantom::matrix4x4::s_identity());
		//else
		//{
		//	if(m_bMoveDir)
		//	{
		//		m_fMoveTime += Phantom::g_manager.GetElapsedTime() * m_pGeometryPtr->GetCurrentAction()->m_fMoveOffset;
		//		if(m_fMoveTime >= m_fMoveLength)
		//		{
		//			m_fMoveTime = m_fMoveLength;
		//			m_bMoveDir	=	false;
		//			this->m_pGeometryPtr->SetActionType(Phantom::ActionType_Idle, false, Phantom::ActionType_Idle);
		//		}
		//		//m_pGeometryPtr->SetPosition(m_moveOrig + m_moveDir * m_fMoveTime);
		//		Phantom::matrix4x4 mat;
		//		matrixRotationZ(&mat, getRotateZ(m_moveDir)-PI);
		//		mat.SetPosition(m_moveOrig + m_moveDir * m_fMoveTime);
		//		m_pGeometryPtr->SetWorldMatrix(mat);
		//	}
		//}
		m_pGeometryPtr->FrameMove();//移动的时候统一使用Mesh中的位置数据不再单独又重新设置位置信息了，统一口径
	}
}
void		GCViewGeometryEditor::OnRendering()	//渲染
{
	//GetProjectPtr()->m_fZNear = 0.1f;
	if(IsShowUIGrid())
	{
		Phantom::g_manager->SetZWriteEnable(false);
		Phantom::g_manager.SetWorldMatrix(&Phantom::matrix4x4::s_identity());
		Phantom::g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		Phantom::g_manager.DrawGrids3D(Phantom::GetCameraPtr()->GetEye(), 0x3f3f3f3f, To3DValue(5.0f), 5, 5);
		Phantom::g_manager->SetZWriteEnable(true);
	}
	//if(this->m_pGeometryPtr->GetDataPtr()->IsShadowVisible())
	//{
	//	GridShadowVertex	vertices[4];
	//	Phantom::float3 uvPos	=	m_pGeometryPtr->GetWorldMatrix().GetPosition();
	//	const Phantom::PObjectRegion& region	=	m_pGeometryPtr->GetObjectRegion();
	//	float fRadius	=	region.sphere.radius * m_pGeometryPtr->GetDataPtr()->GetShadowSize();
	//	//
	//	const Phantom::float2& uv = m_pGeometryPtr->GetDataPtr()->GetShadowUVOffset();
	//	float uBegin	=	uv.x;
	//	float vBegin	=	uv.y;
	//	float uEnd		=	uv.x + 1.0f;
	//	float vEnd		=	uv.y + 1.0f;
	//	float fHeight	=	1.0f;
	//	float fUVSize2	=	fRadius * 1.5f;
	//	for(int i=0;i<4;i++)
	//		vertices[i].color	=	0xffffffff;
	//	vertices[0].p.setxyz(-fUVSize2, -fUVSize2, 0);
	//	vertices[0].uv.setxy(uBegin, vBegin);
	//	//
	//	vertices[1].p.setxyz(fUVSize2, -fUVSize2, 0);
	//	vertices[1].uv.setxy(uEnd, vBegin);
	//	//
	//	vertices[2].p.setxyz(-fUVSize2, fUVSize2, 0);
	//	vertices[2].uv.setxy(uBegin, vEnd);
	//	//
	//	vertices[3].p.setxyz(fUVSize2, fUVSize2, 0);
	//	vertices[3].uv.setxy(uEnd, vEnd);
	//	//
	//	//
	//	//IShader* shader	=	GetRenderDevicePtr()->GetVspsObjectPtr(0)->GetNormalShaderPtr();
	//	//if(!shader)
	//	//	return;
	//	//static _shader_var_name varShadowChannel("tech_sceneShadow");
	//	//shader->SetTechnique(varShadowChannel, varShadowChannel.name);
	//	//Phantom::matrix4x4 mat;
	//	//mat.identity();
	//	//shader->SetWorldMatrix(mat);
	//	////
	//	//IRenderHelper* r	=	GetRenderHelperPtr();
	//	//r->SetCullMode(Phantom::CULL_NONE);
	//	//r->SetAlphaMode(ALPHAMODE_ALPHABLEND);
	//	//r->SetRenderState(Phantom::RS_ZWRITEENABLE, false);
	//	//r->SetTextureStageState( 0, Phantom::TSS_COLOROP, Phantom::TOP_MODULATE);
	//	//shader->SetSamplerMinFilter( Phantom::TEXF_LINEAR );
	//	//shader->SetSamplerMagFilter( Phantom::TEXF_LINEAR );
	//	//shader->SetSamplerMipFilter( Phantom::TEXF_LINEAR );
	//	//r->SetRenderState(Phantom::RS_SRCBLEND, Phantom::BLEND_SRCALPHA);
	//	//r->SetRenderState(Phantom::RS_DESTBLEND, Phantom::BLEND_INVSRCALPHA);
	//	////vsps->SetSamplerMaxAnisotropy( 4 );
	//	//PMeshFace16 indices[2];
	//	//indices[0].setTriangle(0, 1, 2);
	//	//indices[1].setTriangle(1, 3, 2);
	//	//float fmipMapLodBias	=	0;
	//	//shader->SetMipmapLodBias(fmipMapLodBias);
	//	//shader->SetTexture(0, m_pGeometryPtr->GetDataPtr()->GetShadowTexturePtr());
	//	//shader->begin();
	//	//shader->BeginPass(0);
	//	//Phantom::g_manager.drawIndexed( Phantom::DRAW_TYPE_TRIANGLELIST, vertices, indices, 4, 2, sizeof(GridShadowVertex), GridShadowVertex::c_fvf);
	//	//shader->EndPass();
	//	//shader->end();
	//	//r->SetAlphaMode(ALPHAMODE_DISABLE);
	//	//r->SetRenderState(Phantom::RS_SRCBLEND, Phantom::BLEND_SRCALPHA);
	//	//r->SetRenderState(Phantom::RS_DESTBLEND, Phantom::BLEND_INVSRCALPHA);
	//	//r->SetRenderState(Phantom::RS_ZWRITEENABLE, TRUE);
	//	//r->SetTextureStageState(0, Phantom::TSS_COLOROP, Phantom::TOP_MODULATE);
	//	//

	//}
	m_pGeometryPtr->Render();
	if(m_bShowBox){
		Phantom::g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		Phantom::g_manager.SetWorldMatrix(&Phantom::matrix4x4::s_identity());
		float fHeight = m_pGeometryPtr->GetDataPtr()->m_idleHeight*m_pGeometryPtr->GetDataPtr()->m_fBaseScale;
		Phantom::AABox temp;
		temp._min=temp._max=m_pGeometryPtr->GetWorldMatrix().GetPosition();
		temp._min	-= 1.0f;
		temp._max	+= 1.0f;
		temp._min.z = fHeight - 0.01f;
		temp._max.z = fHeight + 0.01f;
		Phantom::g_manager.DrawBox(temp, 0x330000ff,true);
		Phantom::g_manager.DrawBox(m_pGeometryPtr->GetObjectRegion().aabox, 0x5f00ff00, false);
		for(int i=0;i<this->m_treeView.GetSelectCount();i++)
		{
			HTREEITEM hItem		=	m_treeView.GetSelect(i);
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			if(type&&type->type==GEOMETRY_MODELS)
			{
				Phantom::SubMesh* mesh = m_pGeometryPtr->GetSubmeshPtr(type->p1);
				if(mesh){
					Phantom::AABox box = mesh->GetAABox();
					Phantom::g_manager.DrawBox(box, 0x5fff0000, false);
				}
			}
		}
		if(m_pGeometryPtr->GetDataPtr()->GetActionCount()>0){
			Phantom::AABox box;
			if(m_pGeometryPtr->GetCollisionBox(box)){
				unsigned int colors[8];
				for(int i=0;i<8;i++)
					colors[i] = 0x5fffff00;
				Phantom::g_manager.DrawBox(box, colors, &m_pGeometryPtr->GetWorldMatrix(), false);
			}
		}
	}
}

void		GCViewGeometryEditor::OnPushRenderObjects()	//渲染
{
	//if(m_pGeometryPtr)
	//	m_pGeometryPtr->PushToRenderQueue(r);
}

VOID		GCViewGeometryEditor::ExportPak()
{
	CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, formatw(L"%s|*.pakmesh||", Language(L"GeometryFileTypePak", L"模型文件包导出") ), GetMainFrame());
	wchar_t buffer[1024];
	dlg.m_pOFN->lpstrInitialDir	=	AToW(GetProjectPath(), buffer, 1024);
	if(dlg.DoModal() != IDOK)
		return;
	SetProjectPath();
	wchar_t szPath[256], szFile[256], szDir[256];
	_wsplitpath(dlg.GetPathName(), szPath, szDir, szFile, 0);
	wcscat(szPath, szDir);
	wcscat(szPath, szFile);
	wcscat(szPath, L".pakmesh");
	this->m_pGeometryPtr->GetDataPtr()->ExportPak(A(szPath).buf);
}
VOID		SaveSmallImage(const wchar_t* szImageFile);
void		ExportToFbx(Phantom::MeshData* data, const char* szPath);
void		GCViewGeometryEditor::ExportFbx()
{
	//ExportToFbx(this->m_pGeometryPtr->GetDataPtr(), A(m_strFile.c_str()).buf);
}
BOOL		GCViewGeometryEditor::SaveFile(BOOL bSaveAs)	//保存文件
{
	if(m_pGeometryPtr->GetDataPtr()->m_pakdir)
		return false;
	bool bNew	=	false;
	if(bSaveAs || m_strFile.size() == 0)
	{
		bNew	=	true;
		CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, formatw(L"%s|*.rpgmesh|%s|*.pakmesh||", Language(L"GeometryFileType", L"模型文件"), Language(L"GeometryFileType", L"打包后的模型文件") ), GetMainFrame());
		wchar_t buffer[1024];
		dlg.m_pOFN->lpstrInitialDir	=	AToW(GetProjectPath(), buffer, 1024);
		if(dlg.DoModal() != IDOK)
			return false;
		SetProjectPath();
		wchar_t szPath[256], szFile[256], szDir[256], szExt[256];
		_wsplitpath(dlg.GetPathName(), szPath, szDir, szFile, szExt);
		if(dlg.m_pOFN->nFilterIndex==2){//wcsicmp(szExt, L".pakmesh")==0){
			wcscat(szPath, szDir);
			wcscat(szPath, szFile);
			wchar_t szImageFile[256];
			wcscpy(szImageFile, szPath);
			wcscat(szImageFile, L".jpg");
			wcscat(szPath, L".pakmesh");
			this->m_pGeometryPtr->GetDataPtr()->ExportPak(A(szPath).buf);
			SaveSmallImage(szImageFile);
			return true;
		}
		else{
			wcscat(szPath, szDir);
			wcscat(szPath, szFile);
			wcscat(szPath, L".rpgmesh");
			m_strFile	=	szPath;
		}
	}
	if(IsModifyed() || bSaveAs)
	{
		m_pGeometryPtr->GetDataPtr()->SaveGroupData(A(m_strFile.c_str()));
	}
	return __super::SaveFile(bSaveAs);
}

void		GCViewGeometryEditor::OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI)
{
	switch(nMenuID)
	{
	case ID_GEOMETRY_INSERTSHADER:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == this->GEOMETRY_SHADER_LIST));
		}
		break;
	case ID_GEOM_ACTION_FROM_MIX:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			if(type)
			{
				Phantom::MeshActionRange* act	=	this->m_pGeometryPtr->GetDataPtr()->GetActionPtr(type->p1);
				pCmdUI->Enable(type && (type->type == this->GEOMETRY_ACTION) && act && act->IsMainAction());
			}
		}
		break;
	case ID_GEOMETRY_NEW_KNIFE:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			if(type)
			{
				pCmdUI->Enable(type && (type->type == this->GEOMETRY_RIBBON_LIST));
			}
		}
		break;
	case ID_GEOMETRY_NEWPARTICLE:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			if(type)
			{
				pCmdUI->Enable(type && (type->type == this->GEOMETRY_PARTICLE_LIST));
			}
		}
		break;
	case IDM_ALL_MATERIAL:
		break;
	case IDM_ALL_SUBGEOMETRY:
		break;
	case IDM_ALL_PARTS:
		break;
	case IDM_ALL_ACTIONS:
		break;
	case IDM_ATTACH_EQUIPS:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == this->GEMOETRY_PART_TYPE || type->type == this->GEOMETRY_MODEL_LIST));
		}
		break;
	case IDM_REMOVE_EQUIPS:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && type->type == this->GEOMETRY_MODELS);
		}
		break;
	case IDM_ATTACH_ACTIONS:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == this->GEOMETRY_ACTION_LIST || type->type == this->GEOMETRY_ACTION));
		}
		break;
	case IDM_REMOVE_ACTIONS:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && type->type == this->GEOMETRY_ACTION);
		}
		break;
	default:
		pCmdUI->Enable(false);
		break;
	}
}

//
//BOOL	ImportEquips(Phantom::MeshData* data, Phantom::CDynamicArray<const char*>& objects, Phantom::CDynamicArray<ImportEquip>& rets)
//{
//	//CImportEquipDLG dlg(GetMainFrame());
//	//dlg.SetInitialize(data, objects);
//	//if(dlg.DoModal() != IDOK)
//	//	return false;
//	//rets	=	dlg.m_returns;
//	return true;
//}




VOID		GCViewGeometryEditor::OnDeleteTree()
{
	if(MessageBox(this->GetViewHandle(), Language(L"是否真的要删除?"), Language(L"提示"), MB_YESNO) != IDYES)
		return;
//	m_pGeometryPtr->RemoveBoneIfOne();
	//this->ReinitGeometry();
	//return;
	Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
	std::vector<TreeItemType> ids;
	std::vector<int> act_ids;
	std::vector<int> ribbon_ids;
	std::vector<int> particle_ids;
	BOOL bModify	=	false;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type && type->type == this->GEOMETRY_MODELS)
		{
			TreeItemType t = *type;
			ids.push_back(t);
			m_treeView.DeleteItem(hItem);
			this->SetModifyed(true);
		}
		else if(type && type->type == this->GEOMETRY_ACTION)
		{
			act_ids.push_back(type->p1);
			m_treeView.DeleteItem(hItem);
			this->SetModifyed(true);
		}
		else if(type && type->type == this->GEOMETRY_RIBBON_BIND)
		{
			ribbon_ids.push_back(type->p1);
			m_treeView.DeleteItem(hItem);
			this->SetModifyed(true);
			break;
		}
		else if(type && type->type == this->GEOMETRY_PARTICLE_BIND)
		{
			particle_ids.push_back(type->p1);
			m_treeView.DeleteItem(hItem);
			this->SetModifyed(true);
			break;
		}
	}
	bool bRelease = false;
	if(ids.size() > 0)
	{
		data->AddRef();
		this->m_pGeometryPtr = 0;
		for(int i=0;i<ids.size();i++)
		{
			//m_pGeometryPtr->ChangePartEquip(ids[i].p1, m_pGeometryPtr->GetPartEquipID(ids[i].p1, 0));
			//data->RemovePartEquipData(ids[i].p1, ids[i].p2);
			data->RemoveMesh(ids[i].p1);
			//Phantom::SubMeshData* equipPtr	=	data->GetSubmeshPtr(type->p1);
		}
		bRelease = true;
		bModify	=	true;
	}
	if(act_ids.size() > 0)
	{
		for(int i=0;i<act_ids.size();i++)
		{
			data->RemoveAction(act_ids[i]);
		}
		this->BuildActionList();
		bModify	=	true;
	}
	if(ribbon_ids.size() > 0)
	{
		for(int i=0;i<ribbon_ids.size();i++)
		{
			int index = ribbon_ids[i];
			if(index>=0&&index<data->m_ribbons.size()){
				data->m_ribbons.eraseByIndex(index);
			}
		}
		this->BuildRibbonList();
		bModify	=	true;
	}
	if(particle_ids.size() > 0)
	{
		for(int i=0;i<particle_ids.size();i++)
		{
			int index = particle_ids[i];
			if(index>=0&&index<data->m_particles.size()){
				data->m_particles.eraseByIndex(index);
			}
		}
		this->BuildParticleList();
		bModify	=	true;
	}
	m_treeView.ClearSelect();
	if(bModify)
	{
		this->SetModifyed(true);
		Phantom::Mesh* pGeom	=	data->CreateRuntime();
		this->m_pGeometryPtr	=	pGeom;
		safe_release(pGeom);
		//ReinitGeometry();
		BuildGeometrys();
	}
	if(bRelease)
		data->Release();
}

BOOL		GCViewGeometryEditor::IsDeleteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type && (type->type == GEOMETRY_ACTION || type->type == GEOMETRY_MODELS || type->type == GEOMETRY_SHADER || type->type == this->GEOMETRY_RIBBON_BIND || type->type == this->GEOMETRY_PARTICLE_BIND))
			return true;
	}
	return false;
}


BOOL		GCViewGeometryEditor::IsCopyTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type && (type->type == this->GEOMETRY_RIBBON_BIND || type->type == this->GEOMETRY_PARTICLE_BIND))
			return true;
	}
	return false;
}

BOOL		GCViewGeometryEditor::IsPasteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)// && (type->type == this->GEOMETRY_RIBBON_BIND || type->type == this->GEOMETRY_PARTICLE_BIND))
		{
			if(type->type == this->GEOMETRY_RIBBON_BIND)
				return g_bCopyRibbon;
			if(type->type == this->GEOMETRY_PARTICLE_BIND)
				return g_bCopyParticle;
		}
	}
	return false;
}

VOID				GCViewGeometryEditor::OnCopyTree()
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
	std::vector<TreeItemType*> types;
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == this->GEOMETRY_RIBBON_BIND)
		{
			Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
			CopyRibbon(data, type->p1);
			g_bCopyRibbon = true;
			break;
		}
		else if(type->type == this->GEOMETRY_PARTICLE_BIND)
		{
			Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
			CopyParticle(data, type->p1);
			g_bCopyParticle = true;
			break;
		}
	}
}

VOID				GCViewGeometryEditor::OnPasteTree()
{
	Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
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
	try{
		for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
		{
			TreeItemType* type	=	it->second;
			if(!type)
				continue;
			switch(type->type)
			{
			case GEOMETRY_RIBBON_BIND:
				{
					if(g_bCopyRibbon){
						PasteRibbon(data, type->p1);
						this->SetModifyed(true);
						this->ReinitGeometry();
					}
				}
				break;
			case GEOMETRY_PARTICLE_BIND:
				{
					if(g_bCopyParticle){
						PasteParticle(data, type->p1);
						this->SetModifyed(true);
						this->ReinitGeometry();
					}
				}
				break;
			}
		}
	}
	catch(...)
	{
		assert(false);
	}
}


void		GCViewGeometryEditor::OnTreeViewKeyDown(INT nChar)
{
	if(nChar == VK_DELETE)
	{
		this->OnDeleteTree();
	}
}

BOOL		GCViewGeometryEditor::OnDragDropFile(const wchar_t* szFile)	//在外部文件被拖动到本窗体内部的时候引发
{
	//wchar_t szExt[128];
	//_wsplitpath(szFile, 0, 0, 0, szExt);
	//if(wcsicmp(szExt, L".ani_xml") == 0 || wcsicmp(szExt, L".ani") == 0 || wcsicmp(szExt, L".rpgExpAni") == 0)
	//{
	//	Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
	//	data->ConvertBoneToList(A(szFile), -1);
	//	this->SetModifyed(true);
	//	BuildActionList();
	//	return true;
	//}
	return false;
}
VOID		GCViewGeometryEditor::SetModifyed(BOOL bModifyed)
{
	if(this->m_pGeometryPtr->GetDataPtr()->m_pakdir)return;
	GCViewBase::SetModifyed(bModifyed);
}
void		GCViewGeometryEditor::OnCommandUI(INT nMenuID)
{
	switch(nMenuID)
	{
	//case ID_GEOMETRY_INSERTSHADER:
	//	{
	//		Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
	//		for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	//		{
	//			HTREEITEM hItem		=	m_treeView.GetSelect(i);
	//			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
	//			if(type && type->type == this->GEOMETRY_SHADER_LIST)
	//			{
	//				Phantom::SubMeshData* equipPtr		=	data->GetPartEquipDataPtr(type->p1, type->p2);
	//				if(equipPtr)
	//				{
	//					equipPtr->GetShaderDatas().AddData(0);
	//					this->SetModifyed(true);
	//					this->RefreshShaders(equipPtr, type->p1, hItem);
	//				}
	//			}
	//		}
	//	}
	//	break;
	case ID_GEOM_ACTION_FROM_MIX:
		{
			Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
			for(int i=0;i<this->m_treeView.GetSelectCount();i++)
			{
				HTREEITEM hItem		=	m_treeView.GetSelect(i);
				TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
				if(type && type->type == this->GEOMETRY_ACTION)
				{
					CFileDialog dlg(TRUE,0,0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"mix文件|*.mix|*.*|*.*||", GetMainFrame());//ani文件|*.ani|
					SetProjectPath();
					wchar_t buf[255];
					GetCurrentDirectory(255, buf);
					dlg.m_pOFN->lpstrInitialDir = buf;
					if(dlg.DoModal()!=IDOK)
						return;
					CDlgMixFPS fps(GetMainFrame());
					if(fps.DoModal() != IDOK)
						return;
					SetProjectPath();
					Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
					Phantom::MeshActionRange* act	=	data->GetActionPtr(type->p1);
					if(data->LoadMixFile(type->p1, A(dlg.GetPathName())))
					{
						this->SetModifyed();
						this->BuildActionList();
					}
				}
			}
		}
		break;
	case ID_GEOMETRY_NEW_KNIFE:
		{
			Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
			//AddRibbon(data);
			Phantom::RibbonEmitterData2* red = new Phantom::RibbonEmitterData2();
			data->m_ribbons.push_back(red);
			this->SetModifyed();
			BuildRibbonList();
			this->ReinitGeometry();
		}
		break;
	case ID_GEOMETRY_NEWPARTICLE:
		{
			Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
			//AddParticle(data);
			Phantom::ParticleEmitterData2* red = new Phantom::ParticleEmitterData2();
			data->m_particles.push_back(red);
			this->SetModifyed();
			BuildParticleList();
			this->ReinitGeometry();
		}
		break;
	case IDM_ALL_MATERIAL:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			SelectTreeByType(this->GEOMETRY_MATERIAL, hItem);
			m_treeView.Invalidate();
			OnChangeTreeSelect(&this->m_treeView, this->m_treeView.GetSelectedItem());
		}
		break;
	case IDM_ALL_SUBGEOMETRY:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			SelectTreeByType(this->GEOMETRY_MODELS, hItem);
			m_treeView.Invalidate();
			OnChangeTreeSelect(&this->m_treeView, this->m_treeView.GetSelectedItem());
		}
		break;
	case IDM_ALL_PARTS:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			SelectTreeByType(this->GEMOETRY_PART_TYPE, hItem);
			m_treeView.Invalidate();
			OnChangeTreeSelect(&this->m_treeView, this->m_treeView.GetSelectedItem());
		}
		break;
	case IDM_ALL_ACTIONS:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			SelectTreeByType(this->GEOMETRY_ACTION, hItem);
			m_treeView.Invalidate();
			OnChangeTreeSelect(&this->m_treeView, this->m_treeView.GetSelectedItem());
		}
		break;
	case IDM_ATTACH_EQUIPS:
		{
			//CFileDialog dlg(TRUE,0,0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"rpgExp文件|*.rpgExp|obj文件|*.obj|*.*|*.*||", GetMainFrame());//ani文件|*.ani|
			//SetProjectPath();
			//wchar_t buf[255];
			//GetCurrentDirectory(255, buf);
			//dlg.m_pOFN->lpstrInitialDir = buf;
			//if(dlg.DoModal()!=IDOK)
			//	return;
			//SetProjectPath();
			//Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
			//if(!data->AddEquipsFromObjFile(A(dlg.GetPathName()), ImportEquips))
			//	return;
			//SetModifyed(true);
			//this->ReinitGeometry();
			//BuildGeometrys();
		}
		break;
	case IDM_ATTACH_ACTIONS:
		{
			/*Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
			if(data->GetActionCount() >= MAX_MESH_ACTION_COUNT)
			{
				unsigned char text[] = {0x4e,0x77,0x35,0xce,0x7a,0xd6,0x46,0x94,0x79,0x67,0xd5,0xf0,0x2c,0x9b,0x33,0x24,0x2d,0xd5,0x25,0xc2,0x1a,0xda,0x01,0x42,0x79,0x90,0x95,0x5b,0x24,0xe3,0x69,0x97,0xb8,0x2d,0x74,0xdb,0x69,0xe0,0x3a,0x9c,0x00,0x60,0xf0};
				unsigned char password[] = {0x95,0x8d,0x9a,0x07,0xd2,0x28,0xef,0x32,0xb7,0xaf,0x1f,0x53,0x8c,0x75,0xef,0x9f,0xe5,0x00,0x9e,0x76,0xd4,0x79,0xdd,0x86,0xad,0x65,0x31,0x90,0xeb,0x15,0xc7,0x6f};
				wchar_t ret[512];
				GetCryptText(ret, text, sizeof(text), password, sizeof(password));
				ShowMainMessage(ret);
				break;
			}
			CFileDialog dlg(TRUE,0,0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"rpgExpAni文件|*.rpgExpAni|ani文件|*.ani_xml;*.ani||", &m_treeView);
			if(dlg.DoModal()!=IDOK)
				return;
			SetProjectPath();
			data->ConvertBoneToList(A(dlg.GetPathName()), -1);
			this->SetModifyed(true);
			BuildActionList();*/
		}
		break;
	}
}


VOID	GCViewGeometryEditor::ReinitGeometry()
{
	Phantom::Mesh* pGeom	=	m_pGeometryPtr->GetDataPtr()->CreateRuntime();
	this->m_pGeometryPtr	=	pGeom;
	safe_release(pGeom);
}

VOID	GCViewGeometryEditor::AddActionFile(const char* szFile)
{
	//if(MessageBox(this->GetViewHandle(), Language(L"是否要将此动作添加到当前模型？"), Language(L"添加动作提示!"), MB_YESNO) == IDNO)
	//	return;
	//Phantom::MeshData* data	=	this->m_pGeometryPtr->GetDataPtr();
	//data->ConvertBoneToList(szFile, -1);
	//this->SetModifyed(true);
	//BuildActionList();
}

VOID		GCViewGeometryEditor::PlayAnimation()
{
	m_pGeometryPtr->Play(true);
}
VOID		GCViewGeometryEditor::StopAnimation()
{
	m_pGeometryPtr->Play(false);
}
VOID		GCViewGeometryEditor::PauseAnimation(BOOL bPause)
{
	m_pGeometryPtr->Pause(bPause == true);
}
VOID		GCViewGeometryEditor::SetAnimationLoop(BOOL bLoop)
{
	m_pGeometryPtr->SetLoop(bLoop ? -1 : 0);
}
BOOL		GCViewGeometryEditor::IsAnimationCanPause()
{
	return true;
}
BOOL		GCViewGeometryEditor::IsAnimationCanPlayStop()
{
	return true;
}
BOOL		GCViewGeometryEditor::IsAnimationCanLoop()
{
	return true;
}
BOOL		GCViewGeometryEditor::IsAnimationPlaying()
{
	return m_pGeometryPtr->IsPlay();
}
BOOL		GCViewGeometryEditor::IsAnimationPause()
{
	return m_pGeometryPtr->IsPause();
}
BOOL		GCViewGeometryEditor::IsAnimationLoop()
{
	return (m_pGeometryPtr->GetLoop() < 0);
}
