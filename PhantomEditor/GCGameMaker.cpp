//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   GCUnit2DEditor.cpp
	软件名称   :   幻影游戏引擎

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	
	Copyright 2009-2016 Zhao Dexian
	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "GCGameMaker.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"
#include "SetElementTextureDlg.h"

class GCViewUnit2DEditor;

struct	Unit2DCopyData
{
	TreeItemType		type;
	GCViewUnit2DEditor*	editor;
	Unit2DCopyData()
	{
		editor	=	0;
	}
};

std::vector<Unit2DCopyData>		g_copyUnit2Ds;

VOID		SetUnit2DCopy(GCViewUnit2DEditor* e, std::vector<TreeItemType*>* vs)
{
	g_copyUnit2Ds.clear();
	for(int i=0;i<vs->size();i++)
	{
		Unit2DCopyData ud;
		ud.editor	=	e;
		ud.type		=	*vs->at(i);
		g_copyUnit2Ds.push_back(ud);
	}
}

BOOL		IsCanPasteUnit2D(int nType)
{
	for(int i=0;i<g_copyUnit2Ds.size();i++)
	{
		if(g_copyUnit2Ds[i].type.type == nType)
		{
			return true;
		}
	}
	return false;
}

GCViewUnit2DEditor::GCViewUnit2DEditor(CPhantomGCDoc* docPtr)
	:GCViewBase(docPtr, Language(L"2D对象编辑"), IDB_UI_VIEW_BMP)
{
	m_dataPtr	=	0;
	m_renderPtr	=	0;
	m_editMode		=	EditorMode_None;
	m_downPoint.SetPoint(0, 0);
	m_bLButtonDown	=	false;
}
VOID		GCViewUnit2DEditor::CloseFile(){
	for(int i=0;i<g_copyUnit2Ds.size();i++)
	{
		if(g_copyUnit2Ds[i].editor == this)
		{
			g_copyUnit2Ds.erase(g_copyUnit2Ds.begin() + i);
			i--;
			break;
		}
	}
	safe_release(m_renderPtr);
	safe_release(m_dataPtr);
}
GCViewUnit2DEditor::~GCViewUnit2DEditor(void)
{
	CloseFile();
}

HICON		GCViewUnit2DEditor::GetViewerIcon()
{
	static HICON	hIcon	=	AfxGetApp()->LoadIcon(IDI_UNIT2D_FILE_ICON);
	static HICON	hIconR	=	AfxGetApp()->LoadIcon(IDI_Z_UNIT2D_FILE_ICON);
	if(this->m_bIsReadOnly)
		return hIconR;
	return hIcon;
}

VOID		GCViewUnit2DEditor::LoopAllUnit2DEquips(ProcUnit2DEquip proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRename)
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
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == UNIT2D_EQUIP)
		{
			IUnit2DEquip* equip	=	m_dataPtr->GetEquipPtrByID(type->p1);
			(*proc)(equip, nParameter, nParameter2);
			SetModifyed(true);
			if(bRename)
				m_treeView.SetItemText(it->first, W(equip->GetEquipName()));
		}
	}
}

VOID		GCViewUnit2DEditor::LoopAllUnit2DAnimations(ProcUnit2DAnimation proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRename)
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
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == UNIT2D_ANIMATION_ID)
		{
			IUnit2DAnimationData* data	=	m_dataPtr->GetAnimationByID(type->p1);
			(*proc)(data, nParameter, nParameter2);
			SetModifyed(true);
			if(bRename)
			{
				m_treeView.SetItemText(it->first, W(data->GetName()));
				OnRenameAnimations(data);
			}
		}
	}
}

VOID		GCViewUnit2DEditor::LoopAllUnit2DTexture(ProcUnit2DTexture proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRect)
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
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == UNIT2D_TEXTURE)
		{
			IUnit2DEquip* equip		=	m_dataPtr->GetEquipPtrByID(type->p1);
			IUnit2DAnimation* ani	=	equip ? equip->GetAnimationPtrByID(type->p2) : 0;
			IUnit2DTexture* tex		=	ani ? ani->GetAngleTexture((Unit2DAngle)type->p3) : 0;
			if(!equip || !ani || !tex)
				continue;
			(*proc)(tex, nParameter, nParameter2);
			SetModifyed(true);
			if(bRect)
			{
				SetPropInt(m_textureRectL, tex->GetTextureRect().left);
				SetPropInt(m_textureRectT, tex->GetTextureRect().top);
				SetPropInt(m_textureRectW, tex->GetTextureRect().GetWidth());
				SetPropInt(m_textureRectH, tex->GetTextureRect().GetHeight());
			}
		}
	}
}

VOID		ProcUnit2D_AnimationName(IUnit2DAnimationData* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetName((const char*)nParameter);
}

VOID		ProcUnit2D_EquipName(IUnit2DEquip* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetEquipName((const char*)nParameter);
}

VOID		ProcUnit2D_EquipLayer(IUnit2DEquip* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetLayer(nParameter);
}

VOID		ProcUnit2D_TextureUVCountW(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetUVCountW(nParameter);
}
VOID		ProcUnit2D_TextureUVCountH(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetUVCountH(nParameter);
}
VOID		ProcUnit2D_TextureUVBeginIndex(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetUVPlayBeginIndex(nParameter);
}
VOID		ProcUnit2D_TextureUVPlayCount(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetUVPlayCount(nParameter);
}

VOID		ProcUnit2D_TextureTileCountW(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetTileCountW(nParameter);
}

VOID		ProcUnit2D_TextureTileCountH(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetTileCountH(nParameter);
}

VOID		ProcUnit2D_TextureScale(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetScaleTexture(nParameter);
}
VOID		ProcUnit2D_TextureRectL(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	PRect rc	=	data->GetTextureRect();
	int width	=	rc.right - rc.left;
	rc.left		=	nParameter;
	rc.right	=	rc.left + width;
	data->SetTextureRect(rc);
}
VOID		ProcUnit2D_TextureRectT(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	PRect rc	=	data->GetTextureRect();
	int height	=	rc.bottom - rc.top;
	rc.top		=	nParameter;
	rc.bottom	=	rc.top + height;
	data->SetTextureRect(rc);
}
VOID		ProcUnit2D_TextureRectW(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	PRect rc	=	data->GetTextureRect();
	rc.right	=	rc.left + nParameter;
	data->SetTextureRect(rc);
}
VOID		ProcUnit2D_TextureRectH(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	PRect rc	=	data->GetTextureRect();
	rc.bottom	=	rc.top + nParameter;
	data->SetTextureRect(rc);
}
VOID		ProcUnit2D_TextureResetRect(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->ResetTextureRect();
}
VOID		ProcUnit2D_TexturePick(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetPickEnable((Unit2DPick)nParameter);
}
VOID		ProcUnit2D_TextureRenderSide(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetRenderSide((UnitRenderSide)nParameter);
}
VOID		ProcUnit2D_TextureFile(IUnit2DTexture* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetTextureFile((const char*)nParameter);
}

void		GCViewUnit2DEditor::OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp)
{
	__super::OnPropertyChanged(wnd, pProp);
	if(wnd == &m_equipProp)
	{
		if(pProp == m_equipName)
		{
			std::string str;
			GetPropText(m_equipName, str);
			LoopAllUnit2DEquips(ProcUnit2D_EquipName, (LPARAM)str.c_str(), 0, true);
		}
		else if(pProp == m_equipLayer)
		{
			LoopAllUnit2DEquips(ProcUnit2D_EquipLayer, GetPropOption(m_equipLayer, g_typeControlLayer), 0);
		}
	}
	else if(wnd == &m_animationIDProp)
	{
		if(pProp == m_animationName)
		{
			std::string str;
			GetPropText(m_animationName, str);
			LoopAllUnit2DAnimations(ProcUnit2D_AnimationName, (LPARAM)str.c_str(), 0, true);
		}
	}
	else if(wnd == &m_textureProp)
	{
		if(pProp == m_textureUVCountW)
			LoopAllUnit2DTexture(ProcUnit2D_TextureUVCountW, GetPropInt(m_textureUVCountW));
		else if(pProp == m_textureUVCountH)
			LoopAllUnit2DTexture(ProcUnit2D_TextureUVCountH, GetPropInt(m_textureUVCountH));
		else if(pProp == m_textureUVBeginIndex)
			LoopAllUnit2DTexture(ProcUnit2D_TextureUVBeginIndex, GetPropInt(m_textureUVBeginIndex));
		else if(pProp == m_textureUVPlayCount)
			LoopAllUnit2DTexture(ProcUnit2D_TextureUVPlayCount, GetPropInt(m_textureUVPlayCount));
		else if(pProp == m_textureTileCountW)
			LoopAllUnit2DTexture(ProcUnit2D_TextureTileCountW, GetPropInt(m_textureTileCountW));
		else if(pProp == m_textureTileCountH)
			LoopAllUnit2DTexture(ProcUnit2D_TextureTileCountH, GetPropInt(m_textureTileCountH));
		else if(pProp == m_textureScale)
			LoopAllUnit2DTexture(ProcUnit2D_TextureScale, GetPropBool(m_textureScale));
		else if(pProp == m_textureRectL)
			LoopAllUnit2DTexture(ProcUnit2D_TextureRectL, GetPropInt(m_textureRectL));
		else if(pProp == m_textureRectT)
			LoopAllUnit2DTexture(ProcUnit2D_TextureRectT, GetPropInt(m_textureRectT));
		else if(pProp == m_textureRectW)
			LoopAllUnit2DTexture(ProcUnit2D_TextureRectW, GetPropInt(m_textureRectW));
		else if(pProp == m_textureRectH)
			LoopAllUnit2DTexture(ProcUnit2D_TextureRectH, GetPropInt(m_textureRectH));
		else if(pProp == m_textureRegetRect)
			LoopAllUnit2DTexture(ProcUnit2D_TextureResetRect);
		else if(pProp == m_texturePick)
			LoopAllUnit2DTexture(ProcUnit2D_TexturePick, GetPropOption(m_texturePick, g_type2DPickEnable));
		else if(pProp == m_textureRenderSide)
			LoopAllUnit2DTexture(ProcUnit2D_TextureRenderSide, GetPropOption(m_textureRenderSide, g_type2DUnitSide));
		else if(pProp == m_textureFileClear)
			LoopAllUnit2DTexture(ProcUnit2D_TextureFile, 0);
		else if(pProp == m_textureFile)
		{
			if(m_textureFile->m_browser.size() > 1)
			{
				std::vector<ElementIn> elements;
				for(int i=0;i<this->m_treeView.GetSelectCount();i++)
				{
					HTREEITEM hItem		=	m_treeView.GetSelect(i);
					TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
					if(type)
					{
						if(type->type == this->UNIT2D_TEXTURE)
						{
							IUnit2DEquip* equip		=	m_dataPtr->GetEquipPtrByID(type->p1);
							IUnit2DAnimation* ani	=	equip ? equip->GetAnimationPtrByID(type->p2) : 0;
							IUnit2DTexture* tex		=	ani ? ani->GetAngleTexture((Unit2DAngle)type->p3) : 0;
							if(!equip || !ani || !tex)
								continue;
							wchar_t buf[128];
							swprintf(buf, L"%d", type->p3, 0);
							elements.push_back(ElementIn(buf, (LPARAM)tex, 0));
						}
					}
				}
				//
				if(elements.size() > 1)
				{
					for(int i=0;i<m_textureFile->m_browser.size();i++)
					{
						this->GetSortFile(m_textureFile->m_browser[i], m_textureFile->m_browser[i].c_str());
					}
					CSetElementTextureDlg dlg(&this->m_unitProp);
					dlg.SetElements(elements, m_textureFile->m_browser);
					if(dlg.DoModal() == IDOK)
					{
						for(int i=0;i<dlg.m_elements.size();i++)
							((IUnit2DTexture*)dlg.m_elements[i].param.param1)->SetTextureFile(A(dlg.m_elements[i].str.c_str()));
					}
					SetModifyed();
					m_textureFile->m_browser.clear();
				}
				//
				this->OnChangeTreeSelect(&m_treeView, NULL);
			}
			else if(m_textureFile->m_browser.size() > 0)
			{
				std::wstring str;
				GetPropFile(m_textureFile, str);
				LoopAllUnit2DTexture(ProcUnit2D_TextureFile, (LPARAM)A(str.c_str()).buf, TRUE);
			}
		}
	}
	else if(wnd == &m_unitProp)
	{
		if(pProp == m_unitSizeW)
		{
			m_dataPtr->SetUnitSize(PSize(GetPropInt(m_unitSizeW), m_dataPtr->GetUnitSize().cy));
			m_renderPtr->ResetUnitSize();
			this->SetModifyed();
		}
		else if(pProp == m_unitSizeH)
		{
			m_dataPtr->SetUnitSize(PSize(m_dataPtr->GetUnitSize().cx, GetPropInt(m_unitSizeH)));
			m_renderPtr->ResetUnitSize();
			this->SetModifyed();
		}
		else if(pProp == m_unitCenterX)
		{
			m_dataPtr->SetCenterPos(PPixel(GetPropInt(m_unitCenterX), m_dataPtr->GetCenterPos().y));
			m_renderPtr->SetScenePos(m_renderPtr->GetScenePos());
			this->SetModifyed();
		}
		else if(pProp == m_unitCenterY)
		{
			m_dataPtr->SetCenterPos(PPixel(m_dataPtr->GetCenterPos().x, GetPropInt(m_unitCenterY)));
			m_renderPtr->SetScenePos(m_renderPtr->GetScenePos());
			this->SetModifyed();
		}
	}
}

VOID		GCViewUnit2DEditor::OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem)	//更新选择树形控件
{
	if(treePtr == &m_treeView)
	{
		//
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
		std::map<IUnit2DEquip*, int> equips;
		for(int i=0;i<m_renderPtr->GetEquipCount();i++)
		{
			equips[m_renderPtr->GetEquipPtr(i)] = 0;
		}
		for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin(); it!=selectTypes.end();it++)
		{
			if(it->second->type == UNIT2D_MAIN || it->second->type == UNIT2D_EQUIP_LIST)
			{
				for(int i=0;i<m_dataPtr->GetEquipCount();i++)
				{
					IUnit2DEquip* eq	=	m_dataPtr->GetEquipPtr(i);
					m_renderPtr->AddEquip(eq);
					equips[eq]			=	1;
				}
			}
			else if(it->second->type == UNIT2D_EQUIP)
			{
				IUnit2DEquip* eq	=	m_dataPtr->GetEquipPtrByID(it->second->p1);
				m_renderPtr->AddEquip(eq);
				equips[eq]			=	1;
			}
			else if(it->second->type == UNIT2D_TEXTURE)
			{
				IUnit2DEquip* equip		=	m_dataPtr->GetEquipPtrByID(it->second->p1);
				m_renderPtr->AddEquip(equip);
				m_renderPtr->SetActiveAnimationID(it->second->p2);
				m_renderPtr->SetAngle((Unit2DAngle)it->second->p3);
				equips[equip]			=	1;
			}
			else if(it->second->type == UNIT2D_ANIMATION_ID)
			{
				for(int i=0;i<m_dataPtr->GetEquipCount();i++)
				{
					IUnit2DEquip* eq	=	m_dataPtr->GetEquipPtr(i);
					m_renderPtr->AddEquip(eq);
					equips[eq]			=	1;
				}
				m_renderPtr->SetActiveAnimationID(it->second->p1);
			}
			else if(it->second->type == UNIT2D_ANIMATION)
			{
				IUnit2DEquip* equip		=	m_dataPtr->GetEquipPtrByID(it->second->p1);
				m_renderPtr->AddEquip(equip);
				m_renderPtr->SetActiveAnimationID(it->second->p2);
				equips[equip]			=	1;
			}
		}
		for(std::map<IUnit2DEquip*, int>::iterator it = equips.begin(); it!=equips.end();it++)
		{
			if(it->second == 0)
			{
				m_renderPtr->RemoveEquip(it->first);
			}
		}
		m_renderPtr->RecheckRequips();
		//
		TreeItemType* type	=	this->GetSelectedItem();
		if(!type)
		{
			SetCurrentProperties(&m_viewerProps, Language(L"文件属性"), NULL);
			return;
		}
		switch(type->type)
		{
		case UNIT2D_MAIN:
			{
				SetCurrentProperties(&m_unitProp, Language(L"单位属性"), NULL);
				SetPropInt(m_unitSizeW, this->m_dataPtr->GetUnitSize().cx);
				SetPropInt(m_unitSizeH, this->m_dataPtr->GetUnitSize().cy);
				SetPropInt(m_unitCenterX, this->m_dataPtr->GetCenterPos().x);
				SetPropInt(m_unitCenterY, this->m_dataPtr->GetCenterPos().y);
			}
			break;
		case UNIT2D_EQUIP:
			{
				SetCurrentProperties(&m_equipProp, Language(L"装备属性"), 0);
				//
				IUnit2DEquip* equip	=	m_dataPtr->GetEquipPtrByID(type->p1);
				m_equipProp.EnableRender(false);
				if(!equip)
				{
					m_equipProp.EnableWindow(FALSE);
					m_equipProp.EnableRender(true);
					break;
				}
				//
				SetPropText(m_equipName, equip->GetEquipName());
				SetPropInt(m_equipID, equip->GetEquipID());
				SetPropOption(m_equipLayer, g_typeControlLayer, equip->GetLayer());
				//
				m_equipProp.EnableWindow(TRUE);
				m_equipProp.EnableRender(true);
			}
			break;
		case UNIT2D_TEXTURE:
			{
				SetCurrentProperties(&m_textureProp, Language(L"贴图属性"), 0);
				//
				m_textureProp.EnableRender(false);
				//
				IUnit2DEquip* equip		=	m_dataPtr->GetEquipPtrByID(type->p1);
				IUnit2DAnimation* ani	=	equip ? equip->GetAnimationPtrByID(type->p2) : 0;
				IUnit2DTexture* tex		=	ani ? ani->GetAngleTexture((Unit2DAngle)type->p3) : 0;
				if(!equip || !ani || !tex)
				{
					m_textureProp.EnableWindow(FALSE);
					m_textureProp.EnableRender(true);
					break;
				}
				//
				SetPropInt(m_textureUVCountW, tex->GetUVCountW());
				SetPropInt(m_textureUVCountH, tex->GetUVCountH());
				SetPropInt(m_textureUVBeginIndex, tex->GetUVPlayBeginIndex());
				SetPropInt(m_textureUVPlayCount, tex->GetUVPlayCount());
				SetPropInt(m_textureTileCountW, tex->GetTileCountW());
				SetPropInt(m_textureTileCountH, tex->GetTileCountH());
				SetPropBool(m_textureScale, tex->IsScaleTexture());
				//
				SetPropFile(m_textureFile, W(tex->GetTextureFile()));
				//
				SetPropInt(m_textureRectL, tex->GetTextureRect().left);
				SetPropInt(m_textureRectT, tex->GetTextureRect().top);
				SetPropInt(m_textureRectW, tex->GetTextureRect().GetWidth());
				SetPropInt(m_textureRectH, tex->GetTextureRect().GetHeight());
				SetPropOption(m_textureRenderSide, g_type2DUnitSide, tex->GetRenderSide());
				SetPropOption(m_texturePick, g_type2DPickEnable, tex->GetPickEnable());
				//
				m_textureProp.EnableWindow(TRUE);
				m_textureProp.EnableRender(true);
			}
			break;
		case UNIT2D_ANIMATION_ID:
			{
				SetCurrentProperties(&m_animationIDProp, Language(L"动画属性"), 0);
				//
				IUnit2DAnimationData* ani	=	m_dataPtr->GetAnimationByID(type->p1);
				m_animationIDProp.EnableRender(false);
				if(!ani)
				{
					m_animationIDProp.EnableWindow(FALSE);
					m_animationIDProp.EnableRender(true);
					break;
				}
				//
				SetPropText(m_animationName, ani->GetName());
				SetPropInt(m_animationID, ani->GetID());
				//
				m_animationIDProp.EnableWindow(TRUE);
				m_animationIDProp.EnableRender(true);
			}
			break;
		default:
			SetCurrentProperties(NULL, NULL, NULL);
			break;
		}
		return;
	}
	SetCurrentProperties(NULL, NULL, NULL);
}

VOID		GCViewUnit2DEditor::GetNewFileName(wchar_t* szFile)
{
	wcscpy(szFile, Language(L"新2D单位"));
}
BOOL		GCViewUnit2DEditor::OpenFile(const wchar_t* szFile)	//打开文件,如果失败则会自动删除
{
	CloseFile();
	this->m_strFile	=	szFile;
	if(!__super::OpenFile(szFile))
		return false;
	this->m_treeView.InsertItem(Language(L"Unit2DViewer", L"2D对象编辑器"), 25, 25);
	if(m_strFile.empty())
	{
		m_dataPtr	=	CreateUnit2D();
		m_dataPtr->AddAnimation();
		m_dataPtr->AddEquip();
		m_renderPtr	=	m_dataPtr->CreateNewRender();
		this->SetModifyed(true);
	}
	else
	{
		SetProjectPath();
		//std::wstring str;
		//this->GetSortFile(str, m_strFile.c_str());
		m_renderPtr		=	LoadUnit2D(A(m_strFile.c_str()), m_packagePtr);
		if(!m_renderPtr)
			return false;
		m_dataPtr		=	m_renderPtr->GetUnit2DPtr();
		safe_addref(m_dataPtr);
	}
	//
	RebuildTrees();
	InitViewerProps();
	//
	InitUnitProp();
	InitEquipProp();
	InitTextureProp();
	InitAnimaionProp();
	return true;
}

VOID						GCViewUnit2DEditor::InitUnitProp()
{
	InitProp(m_unitProp);
	//
	CMFCPropertyGridProperty* group1			= new CMFCPropertyGridProperty(Language(L"2D单位属性"));
	group1->AddSubItem(m_unitSizeW				= AddPropInt(Language(L"单位宽度"), Language(L"Unit2D_UnitSizeW", L"2D对象单位宽度。"), 0, 4096));
	group1->AddSubItem(m_unitSizeH				= AddPropInt(Language(L"单位高度"), Language(L"Unit2D_UnitSizeH", L"2D对象单位高度。"), 0, 4096));
	group1->AddSubItem(m_unitCenterX				= AddPropInt(Language(L"中心点X"), Language(L"Unit2D_UnitCenterX", L"2D对象单位中心点X。"), -4096, 4096));
	group1->AddSubItem(m_unitCenterY				= AddPropInt(Language(L"中心点Y"), Language(L"Unit2D_UnitCenterY", L"2D对象单位中心点Y。"), -4096, 4096));
	//
	this->m_unitProp.AddProperty(group1);
}

VOID						GCViewUnit2DEditor::InitEquipProp()
{
	InitProp(m_equipProp);
	//
	CMFCPropertyGridProperty* group1			= new CMFCPropertyGridProperty(Language(L"装备属性"));
	group1->AddSubItem(m_equipName				= AddPropText(Language(L"装备名称"), Language(L"Unit2D_EquipName", L"设置2D对象装备名字。"), false, 59));
	group1->AddSubItem(m_equipID				= AddPropInt(Language(L"装备ID"), Language(L"Unit2D_EquipID", L"2D对象唯一装备ID。")));
	group1->AddSubItem(m_equipLayer				= AddPropOption(Language(L"优先渲染层级"), Language(L"Unit2D_EquipLayer", L"2D对象渲染层级,级别越高是在最上层显示。"), g_typeControlLayer));
	m_equipID->AllowEdit(FALSE);
	//
	this->m_equipProp.AddProperty(group1);
}

VOID						GCViewUnit2DEditor::InitTextureProp()
{
	InitProp(m_textureProp);
	//
	CMFCPropertyGridProperty* group1			= new CMFCPropertyGridProperty(Language(L"材质属性"));
	group1->AddSubItem(m_textureFile			= AddPropFile(Language(L"设置图片"), Language(L"Unit2D_TextureSet", L"设置2D对象贴图文件。"), GetImageFilter(), true));
	group1->AddSubItem(m_textureFileClear		= AddPropButton(Language(L"清空图片"), Language(L"Unit2D_TextureClear", L"清空2D对象贴图文件。")));
	//
	CMFCPropertyGridProperty* group2			= new CMFCPropertyGridProperty(Language(L"UV动画属性"));
	group2->AddSubItem(m_textureUVCountW		= AddPropInt(Language(L"UV横向数量"), Language(L"Unit2D_TextureUVW", L"设置2D对象贴图UV横向数量。"), 0, 1000));
	group2->AddSubItem(m_textureUVCountH		= AddPropInt(Language(L"UV纵向数量"), Language(L"Unit2D_TextureUVH", L"设置2D对象贴图UV纵向数量。"), 0, 1000));
	group2->AddSubItem(m_textureUVBeginIndex	= AddPropInt(Language(L"UV动画开始索引"), Language(L"Unit2D_TextureUVBegin", L"设置2D对象贴图UV动画开始索引。"), 0, 1000));
	group2->AddSubItem(m_textureUVPlayCount		= AddPropInt(Language(L"UV画面帧数"), Language(L"Unit2D_TextureUVCount", L"设置2D对象贴图UV画面帧数。"), 0, 1000));
	//
	CMFCPropertyGridProperty* group3			= new CMFCPropertyGridProperty(Language(L"渲染相关属性"));
	group3->AddSubItem(m_textureScale			= AddPropBool(Language(L"是否缩放"), Language(L"Unit2D_TextureScale", L"设置2D对象贴图是否缩放。")));
	group3->AddSubItem(m_textureTileCountW		= AddPropInt(Language(L"横向平铺数量"), Language(L"Unit2D_TextureTileW", L"设置2D对象贴图横向平铺数量。"), 0, 1000));
	group3->AddSubItem(m_textureTileCountH		= AddPropInt(Language(L"纵向平铺数量"), Language(L"Unit2D_TextureTileH", L"设置2D对象贴图纵向平铺数量。"), 0, 1000));
	group3->AddSubItem(m_textureRenderSide		= AddPropOption(Language(L"渲染位置"), Language(L"Unit2D_TextureRenderSide", L"设置2D对象贴图渲染位置。"), g_type2DUnitSide));
	//
	CMFCPropertyGridProperty* groupRect			= new CMFCPropertyGridProperty(Language(L"图片区域"));
	groupRect->AddSubItem(m_textureRectL		= AddPropInt(Language(L"左"), Language(L"Unit2D_TextureRectL", L"设置图片有效区域左边。"), 0, 8192));
	groupRect->AddSubItem(m_textureRectT		= AddPropInt(Language(L"上"), Language(L"Unit2D_TextureRectT", L"设置图片有效区域上边。"), 0, 8192));
	groupRect->AddSubItem(m_textureRectW		= AddPropInt(Language(L"宽度"), Language(L"Unit2D_TextureRectW", L"设置图片有效区域宽度。"), 0, 8192));
	groupRect->AddSubItem(m_textureRectH		= AddPropInt(Language(L"高度"), Language(L"Unit2D_TextureRectH", L"设置图片有效区域高度。"), 0, 8192));
	groupRect->AddSubItem(m_textureRegetRect	= AddPropButton(Language(L"重新取得图片区域"), Language(L"Unit2D_TextureResetRect", L"重新从图片获取图片有效区域。")));
	CMFCPropertyGridProperty* groupPick			= new CMFCPropertyGridProperty(Language(L"点击选项"));
	groupPick->AddSubItem(m_texturePick			= AddPropOption(Language(L"可点击属性"), Language(L"Unit2D_PickInfo", L"2D对象图片可点击属性。"), g_type2DPickEnable));
	//
	this->m_textureProp.AddProperty(group1);
	this->m_textureProp.AddProperty(group2);
	this->m_textureProp.AddProperty(group3);
	this->m_textureProp.AddProperty(groupRect);
	this->m_textureProp.AddProperty(groupPick);
}

VOID						GCViewUnit2DEditor::InitAnimaionProp()
{
	InitProp(m_animationIDProp);
	//
	CMFCPropertyGridProperty* group1			= new CMFCPropertyGridProperty(Language(L"动画属性"));
	group1->AddSubItem(m_animationName			= AddPropText(Language(L"动画名称"), Language(L"Unit2D_AnimationName", L"设置2D对象动画名字。"), false, 50));
	group1->AddSubItem(m_animationID			= AddPropInt(Language(L"动画ID"), Language(L"Unit2D_AnimationID", L"2D对象唯一动画ID。")));
	m_animationID->AllowEdit(FALSE);
	//
	this->m_animationIDProp.AddProperty(group1);
}

VOID		GCViewUnit2DEditor::RebuildTrees()
{
	if(m_hViewItem)
		m_treeView.DeleteItem(m_hViewItem);
	wchar_t wFileName[256], wExt[256];
	_wsplitpath(m_strFile.c_str(), 0, 0, wFileName, wExt);
	wcscat(wFileName, wExt);
	if(!wFileName[0])
		GetNewFileName(wFileName);
	m_hViewItem = m_treeView.InsertItem(wFileName, 21, 21);
	m_treeView.SetItemState(m_hViewItem, TVIS_BOLD, TVIS_BOLD);
	m_treeView.Expand(m_hViewItem, TVE_EXPAND);
	//
	this->ClearTreeItem();
	BuildAll();
	//
	m_treeView.m_callback	=	this;
}

VOID		GCViewUnit2DEditor::BuildAll()
{
	m_treeView.SelectItem(m_hViewItem);
	DeleteChildItems(m_hViewItem);
	m_treeView.SetItemData(m_hViewItem, AddTreeItem(UNIT2D_MAIN));
	//
	m_hEquipList = m_treeView.InsertItem(Language(L"装备列表"), 23, 23, m_hViewItem);
	m_treeView.SetItemData(m_hEquipList, AddTreeItem(UNIT2D_EQUIP_LIST));
	m_hAnimationList = m_treeView.InsertItem(Language(L"动画列表"), 24, 24, m_hViewItem);
	m_treeView.SetItemData(m_hAnimationList, AddTreeItem(UNIT2D_ANIMATION_LIST));
	//
	RefreshEquips();
	RefreshAnimations();
	//
}

VOID		GCViewUnit2DEditor::RefreshEquip(IUnit2DEquip* ele, HTREEITEM hEquip)
{
	this->DeleteChildItems(hEquip);
	for(int j=0;j<ele->GetAnimationCount();j++)
	{
		IUnit2DAnimation* ani		=	ele->GetAnimationPtr(j);
		IUnit2DAnimationData* d		=	m_dataPtr->GetAnimationByID(ani->GetAnimationID());
		const char* szName	=	"error";
		if(d)
			szName	=	d->GetName();
		//
		HTREEITEM hAni	=	m_treeView.InsertItem(W(szName), 27, 27, hEquip);
		m_treeView.SetItemData(hAni, AddTreeItem(UNIT2D_ANIMATION, ele->GetEquipID(), ani->GetAnimationID()));
		for(int k=0;k<Unit2DAngle_Count;k++)
		{
			HTREEITEM hMat	=	m_treeView.InsertItem(g_type2DUnitAngle[k], 28, 28, hAni);
			m_treeView.SetItemData(hMat, AddTreeItem(UNIT2D_TEXTURE, ele->GetEquipID(), ani->GetAnimationID(), k));
		}
	}
}

VOID		GCViewUnit2DEditor::RefreshEquips()
{
	m_treeView.SelectItem(m_hEquipList);
	DeleteChildItems(m_hEquipList);
	for(int i=0;i<m_dataPtr->GetEquipCount();i++)
	{
		IUnit2DEquip* e		=	m_dataPtr->GetEquipPtr(i);
		HTREEITEM hEquip	=	m_treeView.InsertItem(W(e->GetEquipName()), 26, 26, m_hEquipList);
		m_treeView.SetItemData(hEquip, AddTreeItem(UNIT2D_EQUIP, e->GetEquipID()));
		RefreshEquip(e, hEquip);
	}
}

VOID		GCViewUnit2DEditor::OnRenameAnimations(IUnit2DAnimationData* ani)
{
	HTREEITEM hEquipItem	=	m_treeView.GetChildItem(m_hEquipList);
	while(hEquipItem)
	{
		HTREEITEM hEquipAni	=	m_treeView.GetChildItem(hEquipItem);
		while(hEquipAni)
		{
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hEquipAni));
			if(type && type->type == UNIT2D_ANIMATION && type->p2 == ani->GetID())
			{
				m_treeView.SetItemText(hEquipAni, W(ani->GetName()));
				break;
			}
			hEquipAni	=	m_treeView.GetNextSiblingItem(hEquipAni);
		}
		hEquipItem	=	m_treeView.GetNextSiblingItem(hEquipItem);
	}
}

VOID		GCViewUnit2DEditor::RefreshAnimations()
{
	m_treeView.SelectItem(m_hAnimationList);
	DeleteChildItems(m_hAnimationList);
	for(int i=0;i<m_dataPtr->GetAnimationCount();i++)
	{
		IUnit2DAnimationData* d		=	m_dataPtr->GetAnimationByIndex(i);
		HTREEITEM hAniID			=	m_treeView.InsertItem(W(d->GetName()), 29, 29, m_hAnimationList);
		m_treeView.SetItemData(hAniID, AddTreeItem(UNIT2D_ANIMATION_ID, d->GetID()));
	}
}

VOID		GCViewUnit2DEditor::OnActive()		//被激活
{
	__super::OnActive();
}

VOID		GCViewUnit2DEditor::OnDeActive()		//失去激活
{
	__super::OnDeActive();
}

void		GCViewUnit2DEditor::OnFrameMove(const RenderParameter& r)	//每次更新
{
	if(m_renderPtr)
	{
		m_renderPtr->FrameMove(r.fElapsedTime);
	}
}

void		GCViewUnit2DEditor::OnPushRenderObjects(const RenderParameter& r)	//渲染
{
}

BOOL		GCViewUnit2DEditor::SaveFile(BOOL bSaveAs)	//保存文件
{
	bool bNew	=	false;
	if(bSaveAs || m_strFile.size() == 0)
	{
		bNew	=	true;
		CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, formatw(L"%s|*.rpg2D||", Language(L"Unit2DFileType", L"2D对象文件") ), GetMainFrame());
		wchar_t buffer[1024];
		dlg.m_pOFN->lpstrInitialDir	=	AToW(GetProjectPath(), buffer, 1024);
		if(dlg.DoModal() != IDOK)
			return false;
		SetProjectPath();
		wchar_t szPath[256], szFile[256], szDir[256];
		_wsplitpath(dlg.GetPathName(), szPath, szDir, szFile, 0);
		wcscat(szPath, szDir);
		wcscat(szPath, szFile);
		wcscat(szPath, L".rpg2D");
		m_strFile	=	szPath;
		wcscat(szFile, L".rpg2D");
		this->m_treeView.SetItemText(this->m_hViewItem, szFile);
	}
	if(IsModifyed() || bSaveAs)
		m_dataPtr->Save(A(m_strFile.c_str()));
	return __super::SaveFile(bSaveAs);
}

//渲染
void		GCViewUnit2DEditor::OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI)
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

	switch(nMenuID)
	{
	case ID_UNIT2D_INSERTEQUIP:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(!type)
				pCmdUI->Enable(false);
			else
				pCmdUI->Enable(type->type == UNIT2D_EQUIP_LIST);
		}
		break;
	case ID_UNIT2D_REMOVEEQUIP:
		{
			pCmdUI->Enable(false);
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UNIT2D_EQUIP)
				{
					pCmdUI->Enable(true);
					break;
				}
			}
		}
		break;
	case ID_UNIT2D_INSERTANIMATION:
		{
			pCmdUI->Enable(false);
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UNIT2D_ANIMATION_LIST)
				{
					pCmdUI->Enable(true);
					break;
				}
			}
		}
		break;
	case ID_UNIT2D_REMOVEANIMATION:
		{
			pCmdUI->Enable(false);
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UNIT2D_ANIMATION_ID)
				{
					pCmdUI->Enable(true);
					break;
				}
			}
		}
		break;
	default:
		pCmdUI->Enable(false);
		break;
	}
}

void		GCViewUnit2DEditor::OnCommandUI(INT nMenuID)
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
	switch(nMenuID)
	{
	case ID_UNIT2D_INSERTEQUIP:
		{
			TreeItemType* type	=	this->GetSelectedItem();
			if(type && type->type == UNIT2D_EQUIP_LIST)
			{
				IUnit2DEquip* equip	=	this->m_dataPtr->AddEquip();
				this->SetModifyed();
				this->RefreshEquips();
			}
		}
		break;
	case ID_UNIT2D_REMOVEEQUIP:
		{
			if(MessageBox(this->GetViewHandle(), Language(L"是否真的要删除?"), Language(L"提示"), MB_YESNO) == IDYES)
			{
				for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
				{
					TreeItemType* type	=	it->second;
					if(!type)
						continue;
					if(type->type == UNIT2D_EQUIP)
					{
						IUnit2DEquip* equip	=	m_dataPtr->GetEquipPtrByID(type->p1);
						this->m_treeView.DeleteItem(it->first);
						m_dataPtr->RemoveEquip(equip);
					}
				}
				m_treeView.ClearSelect();
				this->m_renderPtr->RecheckRequips();
				this->OnChangeTreeSelect(&m_treeView, NULL);
				this->SetModifyed();
			}
		}
		break;
	case ID_UNIT2D_INSERTANIMATION:
		{
			BOOL bInsert	=	false;
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UNIT2D_ANIMATION_LIST)
				{
					m_dataPtr->AddAnimation();
					this->RefreshAnimations();
					this->SetModifyed();
					bInsert	=	true;
					break;
				}
			}
			//
			if(bInsert)
				this->RefreshEquips();
		}
		break;
	case ID_UNIT2D_REMOVEANIMATION:
		{
			if(MessageBox(this->GetViewHandle(), Language(L"是否真的要删除?"), Language(L"提示"), MB_YESNO) == IDYES)
			{
				BOOL bRemove	=	false;
				for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
				{
					TreeItemType* type	=	it->second;
					if(!type)
						continue;
					if(type->type == UNIT2D_ANIMATION_ID)
					{
						IUnit2DAnimationData* d	=	m_dataPtr->GetAnimationByID(type->p1);
						if(d)
						{
							m_dataPtr->RemoveAnimation(d);
							bRemove	=	true;
							this->m_treeView.DeleteItem(it->first);
						}
					}
				}
				//
				if(bRemove)
				{
					m_treeView.ClearSelect();
					this->m_renderPtr->RecheckRequips();
					this->OnChangeTreeSelect(&m_treeView, NULL);
					this->SetModifyed();
					this->RefreshEquips();
				}
			}
		}
		break;
	default:
		break;
	}
}


BOOL		GCViewUnit2DEditor::IsCopyTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type && (type->type == this->UNIT2D_EQUIP || type->type == this->UNIT2D_TEXTURE || type->type == this->UNIT2D_ANIMATION || type->type == this->UNIT2D_ANIMATION_ID))
			return true;
	}
	return false;
}

BOOL		GCViewUnit2DEditor::IsPasteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(IsCanPasteUnit2D(type->type))
				return true;
		}
	}
	return false;
}

VOID		GCViewUnit2DEditor::OnDeleteTree()
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
	if(MessageBox(this->GetViewHandle(), Language(L"是否真的要删除?"), Language(L"提示"), MB_YESNO) == IDYES)
	{
		BOOL bDelete	=	false;
		for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
		{
			TreeItemType* type	=	it->second;
			if(!type)
				continue;
			switch(type->type)
			{
			case UNIT2D_EQUIP:
				{
					IUnit2DEquip* to	=	m_dataPtr->GetEquipPtrByID(type->p1);
					if(to)
						m_dataPtr->RemoveEquip(to);
					this->m_treeView.DeleteItem(it->first);
					bDelete	=	true;
				}
				break;
			case UNIT2D_ANIMATION_ID:
				{
					IUnit2DAnimationData* to		=	m_dataPtr->GetAnimationByID(type->p1);
					if(to)
						m_dataPtr->RemoveAnimation(to);
					this->m_treeView.DeleteItem(it->first);
					bDelete	=	true;
				}
				break;
			}
		}
		if(bDelete)
		{
			m_treeView.ClearSelect();
			this->SetModifyed(true);
			this->m_renderPtr->RecheckRequips();
			OnChangeTreeSelect(&m_treeView, 0);
			this->RefreshEquips();
		}
	}
}

BOOL		GCViewUnit2DEditor::IsDeleteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type && (type->type == UNIT2D_EQUIP || type->type == UNIT2D_ANIMATION_ID))
			return true;
	}
	return false;
}

VOID				GCViewUnit2DEditor::OnCopyTree()
{
	DefineAuth_6();
	if(!CheckAuth_6(0))
		return;
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
		if(type->type == this->UNIT2D_EQUIP || type->type == this->UNIT2D_TEXTURE || type->type == this->UNIT2D_ANIMATION || type->type == this->UNIT2D_ANIMATION_ID)
			types.push_back(type);
	}
	SetUnit2DCopy(this, &types);
}

VOID				GCViewUnit2DEditor::OnPasteTree()
{
	DefineAuth_6();
	if(!CheckAuth_6(0))
	{
		ShowMainMessage(L"免费版：不支持拷贝粘贴.");
		return;
	}
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
		if(IsCanPasteUnit2D(type->type))
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
			switch(type->type)
			{
			case UNIT2D_EQUIP:
				{
					for(int j=0;j<g_copyUnit2Ds.size();j++)
					{
						if(g_copyUnit2Ds[j].type.type != UNIT2D_EQUIP)
							continue;
						IUnit2DEquip* from	=	g_copyUnit2Ds[j].editor->m_dataPtr->GetEquipPtrByID(g_copyUnit2Ds[j].type.p1);
						IUnit2DEquip* to	=	m_dataPtr->GetEquipPtrByID(type->p1);
						if(from && to)
							to->CloneFrom(from);
					}
					this->SetModifyed(true);
					this->m_renderPtr->RecheckRequips();
					OnChangeTreeSelect(&m_treeView, 0);
				}
				break;
			case UNIT2D_ANIMATION:
				{
					for(int j=0;j<g_copyUnit2Ds.size();j++)
					{
						if(g_copyUnit2Ds[j].type.type != UNIT2D_ANIMATION)
							continue;
						IUnit2DEquip* from		=	g_copyUnit2Ds[j].editor->m_dataPtr->GetEquipPtrByID(g_copyUnit2Ds[j].type.p1);
						IUnit2DAnimation* ani	=	from ? from->GetAnimationPtrByID(g_copyUnit2Ds[j].type.p2) : 0;
						IUnit2DEquip* to		=	m_dataPtr->GetEquipPtrByID(type->p1);
						IUnit2DAnimation* aniTo	=	to ? to->GetAnimationPtrByID(type->p2) : 0;
						if(ani && aniTo)
							aniTo->CloneFrom(ani);
					}
					this->SetModifyed(true);
					this->m_renderPtr->RecheckRequips();
					OnChangeTreeSelect(&m_treeView, 0);
				}
				break;
			case UNIT2D_TEXTURE:
				{
					for(int j=0;j<g_copyUnit2Ds.size();j++)
					{
						if(g_copyUnit2Ds[j].type.type != UNIT2D_TEXTURE)
							continue;
						IUnit2DEquip* from		=	g_copyUnit2Ds[j].editor->m_dataPtr->GetEquipPtrByID(g_copyUnit2Ds[j].type.p1);
						IUnit2DAnimation* ani	=	from ? from->GetAnimationPtrByID(g_copyUnit2Ds[j].type.p2) : 0;
						IUnit2DTexture* texFrom	=	ani ? ani->GetAngleTexture((Unit2DAngle)g_copyUnit2Ds[j].type.p3) : 0;
						IUnit2DEquip* to		=	m_dataPtr->GetEquipPtrByID(type->p1);
						IUnit2DAnimation* aniTo	=	to ? to->GetAnimationPtrByID(type->p2) : 0;
						IUnit2DTexture* texTo	=	aniTo ? aniTo->GetAngleTexture((Unit2DAngle)type->p3) : 0;
						if(texFrom && texTo)
							texTo->CloneFrom(texFrom);
					}
					this->SetModifyed(true);
					this->m_renderPtr->RecheckRequips();
					OnChangeTreeSelect(&m_treeView, 0);
				}
				break;
			}
		}
	}
	catch(...)
	{
		assert(false);
		g_copyUnit2Ds.clear();
	}

}

void				GCViewUnit2DEditor::OnTreeViewKeyDown(INT nChar)
{
	if(nChar == VK_DELETE)
	{
		this->OnDeleteTree();
	}
}

void		GCViewUnit2DEditor::OnRenderingEnd(const RenderParameter& r)
{
	GetDlgResManager()->BeginDraw2D();
	if(m_renderPtr)
	{
		float fS	=	100.0f;
		static float ff	=	0.0f;
		ff			+=	r.fElapsedTime;
		//m_renderPtr->SetScenePos(PUnit(200.0f + cosf(ff) * 100.0f, 200.0f + sinf(ff) * 100.0f));
		//m_renderPtr->SetRotationZEnable(TRUE);
		//m_renderPtr->SetRotationZAngle(m_renderPtr->GetRotationZAngle() + r.fElapsedTime * 10.0f);
		m_renderPtr->SetScenePos(PUnit(m_dataPtr->GetCenterPos().x, m_dataPtr->GetCenterPos().y));
		m_renderPtr->Render(PUnit(0, 0));
	}
	GetDlgResManager()->DrawRect((const PRect*)&m_renderPtr->GetSceneRect(), 0xffff0000, false);
	PPixel center	=	m_dataPtr->GetCenterPos();
	PPixel pos(center.x, center.y);
	GetDlgResManager()->DrawCircle(pos, 2.0f, 0xff00af00);
	GetDlgResManager()->DrawCircle(pos, 3.0f, 0xffaf1f00, 32, false);
	GetDlgResManager()->EndDraw2D();
//	if(m_renderPtr)
//		m_renderPtr->Render();
}
bool				GCViewUnit2DEditor::OnWindowProc(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam)
{
	if(uMsg == WM_LBUTTONDOWN)
	{
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(hWnd, &pos);
		m_downPoint		=	pos;
		SetCapture(hWnd);
		m_bLButtonDown	=	true;
		m_editMode		=	EditorMode_None;
		if(m_renderPtr)
		{
			PPixel center	=	m_dataPtr->GetCenterPos();
			PPixel centerPos(center.x, center.y);
			PRect rcCenter(centerPos.x - 3, centerPos.y - 3, centerPos.x + 3, centerPos.y + 3);
			if(rcCenter.isPtIn(pos.x, pos.y))
			{
				m_downOffset.SetPoint(centerPos.x - m_downPoint.x, centerPos.y - m_downPoint.y);
				m_editMode	=	EditorMode_CenterPos;
			}
			//if(m_renderPtr->Pick(PPixel(pos.x, pos.y)))
				//MessageBox(hWnd, L"", L"", MB_OK);
		}
	}
	else if(uMsg == WM_LBUTTONUP)
	{
		ReleaseCapture();
		m_editMode		=	EditorMode_None;
		this->m_bLButtonDown	=	false;
	}
	else if(uMsg == WM_MOUSEMOVE)
	{
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(hWnd, &pos);
		if(m_bLButtonDown)
		{
			if(m_editMode == EditorMode_CenterPos)
			{
				PPixel center(pos.x + m_downOffset.x, pos.y + m_downOffset.y);
				PPixel centerPos(center.x, center.y);
				m_dataPtr->SetCenterPos(centerPos);
				this->SetModifyed();
			}
		}
	}
	return __super::OnWindowProc(hWnd, uMsg, wParam, lParam);
}
