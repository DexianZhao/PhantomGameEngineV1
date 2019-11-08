#include "StdAfx.h"
#include "resource.h"
#include "GCGameProject.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"
#include "SetElementTextureDlg.h"
#include "ChildFrm.h"
#include "phantom-gc.h"
#include "phantom-gcView.h"
#include "GCViewManager.h"

VOID		GCGameProject::LoopAllChapters(PropChapter proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRename)
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
		if(type->type == GAMEPROJ_CHAPTER || type->type == GAMEPROJ_MODULE)
		{
			if(type->type == GAMEPROJ_CHAPTER)
			{
				CChapter* ch	=	m_gameDirector.GetChapterPtrByID(type->p1);
				(*proc)(ch, nParameter, nParameter2);
				SetModifyed(true);
				if(bRename)
				{
					m_treeView.SetItemText(it->first, GetPlotName(ch));//ch->GetName());
				}
			}
			else if(type->type == GAMEPROJ_MODULE)
			{
				CModule* ch	=	m_gameDirector.GetModulePtrByID(type->p1);
				(*proc)(ch, nParameter, nParameter2);
				SetModifyed(true);
				if(bRename)
				{
					m_treeView.SetItemText(it->first, GetPlotName(ch));//ch->GetName());
				}
			}
		}
	}
}

VOID		GCGameProject::LoopAllDramas(PropDrama proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRename)
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
		if(type->type == GAMEPROJ_DRAMA)
		{
			CDrama* ch	=	m_gameDirector.SearchDrama(type->p1);
			if(ch)
			{
				(*proc)(ch, nParameter, nParameter2);
				SetModifyed(true);
				if(bRename)
				{
					m_treeView.SetItemText(it->first, GetPlotName(ch));//ch->GetName());
				}
			}
		}
	}
}

VOID		GCGameProject::LoopAllActions(PropAction proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRename)
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
		if(type->type == GAMEPROJ_ACTION)
		{
			CPlotAction* ch	=	m_gameDirector.SearchAction(type->p2);
			if(ch)
			{
				(*proc)(ch, nParameter, nParameter2);
				SetModifyed(true);
				if(bRename)
				{
					m_treeView.SetItemText(it->first, GetPlotName(ch));//ch->GetName());
				}
			}
		}
	}
}

VOID		GCGameProject::LoopAllStates(PropState proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRename)
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
		if(type->type == GAMEPROJ_STATE)
		{
			CPlotAction* ch	=	m_gameDirector.SearchAction(type->p1);
			if(ch)
			{
				CPlotState* state	=	ch->GetStatePtrByID(type->p2);
				(*proc)(state, nParameter, nParameter2);
				SetModifyed(true);
				if(bRename)
				{
					m_treeView.SetItemText(it->first, state->GetName());//ch->GetName());
				}
			}
		}
	}
}

VOID		GCGameProject::LoopAllEvents(PropEvent proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRename)
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
		if(type->type == GAMEPROJ_EVENT)
		{
			CPlotPropertiesMap* ch	=	m_gameDirector.SearchPropertiesMap(type->p1);
			if(ch)
			{
				CPlotEventCall* state	=	ch->GetEventCallPtrByID(type->p2);
				(*proc)(ch, state, nParameter, nParameter2);
				SetModifyed(true);
				if(bRename)
				{
					wchar_t wbuf[128];
					swprintf(wbuf, L"%s:%s", state->GetName(), GetEventName(state->GetEventID()));
					m_treeView.SetItemText(it->first, wbuf);//ch->GetName());
				}
			}
		}
	}
}
VOID		GCGameProject::LoopAllVariant(PropVariant proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRename)
{
	if(!m_selectAction)
		return;
	if(!m_selectVar)
		return;
	(*proc)(m_selectVar, nParameter, nParameter2);
	SetModifyed(true);
	//if(bRename)
		this->m_dlgVariant.SetActionPtr(m_selectAction, m_selectVar);
}

VOID		GCGameProject::LoopAllVariantSet(PropVariantSet proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRename)
{
	if(!this->m_selectState)
		return;
	if(!this->m_selectVarSet)
		return;
	(*proc)(m_selectVarSet, nParameter, nParameter2);
	SetModifyed(true);
	//if(bRename)
		this->m_callPlot.SetPlotCall(m_selectState);
}

VOID		GCGameProject::LoopAllPropertyCall(PropPropertyCall proc, LPARAM nParameter, LPARAM nParameter2, BOOL bRename)
{
	if(!this->m_selectState)
		return;
	if(!this->m_selectPropSet)
		return;
	(*proc)(m_selectPropSet, nParameter, nParameter2);
	SetModifyed(true);
	//if(bRename)
		this->m_callPlot.SetPlotCall(m_selectState);
}

VOID		Chapter_SetName(CChapter* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetName((const wchar_t*)nParameter);
}

VOID		State_SetName(CPlotState* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetName((const wchar_t*)nParameter);
}

VOID		State_SetElapsedTime(CPlotState* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetElapsedTime(*(float*)nParameter);
}

VOID		State_SetCallIndex(CPlotState* data, LPARAM nParameter, LPARAM nParameter2)
{
	//data->SetCallIndex(nParameter);
}

VOID		Drama_SetName(CDrama* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetName((const wchar_t*)nParameter);
}

VOID		Action_SetName(CPlotAction* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetName((const wchar_t*)nParameter);
}

VOID		Variant_SetName(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_name	=	(const wchar_t*)nParameter;
}

VOID		Variant_Int(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.m_nValue	=	nParameter;
}
VOID		Variant_Double(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.m_dValue	=	*(float*)nParameter;
}

VOID		Variant_Text(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.m_text		=	(const wchar_t*)nParameter;
}

VOID		Variant_Float1(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.m_f4.x	=	*(float*)nParameter;
}

VOID		Variant_Float2(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.m_f4.y	=	*(float*)nParameter;
}

VOID		Variant_Float3(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.m_f4.z	=	*(float*)nParameter;
}

VOID		Variant_Float4(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.m_f4.w	=	*(float*)nParameter;
}

VOID		Variant_Int1(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.m_i4.left	=	nParameter;
}

VOID		Variant_Int2(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.m_i4.top	=	nParameter;
}

VOID		Variant_Int3(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.m_i4.right=	nParameter;
}

VOID		Variant_Int4(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.m_i4.bottom	=	nParameter;
}


VOID		VariantSet_VariantID(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_variantSetID	=	nParameter;
}

VOID		VariantSet_OtherVariantID(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.otherVariantID		=	nParameter;
}

VOID		VariantSet_Int(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.value.m_nValue	=	nParameter;
}
VOID		VariantSet_Double(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.value.m_dValue	=	*(float*)nParameter;
}

VOID		VariantSet_Text(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.value.m_text		=	(const wchar_t*)nParameter;
}

VOID		VariantSet_Float1(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.value.m_f4.x	=	*(float*)nParameter;
}

VOID		VariantSet_Float2(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.value.m_f4.y	=	*(float*)nParameter;
}

VOID		VariantSet_Float3(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.value.m_f4.z	=	*(float*)nParameter;
}

VOID		VariantSet_Float4(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.value.m_f4.w	=	*(float*)nParameter;
}

VOID		VariantSet_Int1(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.value.m_i4.left	=	nParameter;
}

VOID		VariantSet_Int2(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.value.m_i4.top	=	nParameter;
}

VOID		VariantSet_Int3(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.value.m_i4.right=	nParameter;
}

VOID		VariantSet_Int4(CCallVariantSet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_value.value.m_i4.bottom	=	nParameter;
}


VOID		PropertyCall_PropertiesMapID(CCallPropertySet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_propertiesMapID	=	nParameter;
}
VOID		PropertyCall_PropertiesID(CCallPropertySet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_propertyCall	=	nParameter;
}

VOID		PropertyCall_OtherVariantID(CCallPropertySet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_params[nParameter].otherVariantID		=	nParameter2;
}

VOID		PropertyCall_Int(CCallPropertySet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_params[nParameter].value.m_nValue	=	nParameter2;
}
VOID		PropertyCall_Double(CCallPropertySet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_params[nParameter].value.m_dValue	=	*(float*)nParameter2;
}

VOID		PropertyCall_Text(CCallPropertySet* var, LPARAM nParameter, LPARAM nParameter2)
{
	var->m_params[nParameter].value.m_text		=	(const wchar_t*)nParameter2;
}


VOID		Event_SetName(CPlotPropertiesMap* map, CPlotEventCall* data, LPARAM nParameter, LPARAM nParameter2)
{
	data->SetName((const wchar_t*)nParameter);
}

VOID		Event_SetID(CPlotPropertiesMap* map, CPlotEventCall* data, LPARAM nParameter, LPARAM nParameter2)
{
	CPlotEvent* evt	=	map->GetEventPtrByID((EventID)data->GetEventID());
	if(!evt)
		return;
	PlotValueList* list	=	evt->GetParameter(nParameter);
	const wchar_t* szName	=	(const wchar_t*)nParameter2;
	wchar_t buf[128];
	for(int i=0;i<list->values.size();i++)
	{
		swprintf(buf, L"%s[%d]", list->values[i].name.str(), list->values[i].value);
		if(wcsicmp(buf, szName) == 0)
		{
			//data->SetEventParameter(nParameter, list->values[i].value);
			//data->SetName(list->values[i].name);
			break;
		}
	}
}

void		GCGameProject::OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp)
{
	__super::OnPropertyChanged(wnd, pProp);
	if(wnd == &m_gameProp)
	{
		if(pProp == this->m_gameName)
		{
			std::wstring name;
			GetPropText(m_gameName, name);
			SetGameRename(name.c_str());
		}
		else if(pProp == m_gameCreatorName)
		{
			std::wstring name;
			GetPropText(m_gameCreatorName, name);
			m_gameDirector.m_creatorName	=	name.c_str();
			this->SetModifyed();
		}
		else if(pProp == m_gameVersion)
		{
			std::wstring name;
			GetPropText(m_gameVersion, name);
			m_gameDirector.m_versionText	=	name.c_str();
			this->SetModifyed();
		}
		else if(pProp == m_gameCopyright)
		{
			std::wstring name;
			GetPropText(m_gameCopyright, name);
			m_gameDirector.m_copyright	=	name.c_str();
			this->SetModifyed();
		}
		else if(pProp == m_gameScreenW || pProp == m_gameScreenH)
		{
			m_gameDirector.SetScreenSize(PSize(GetPropInt(m_gameScreenW), GetPropInt(m_gameScreenH)));
			this->ChangeGameSize();
			this->SetModifyed();
		}
	}
	else if(wnd == &m_chapterProp)
	{
		if(pProp == m_chapterName)
		{
			std::wstring str;
			GetPropText(m_chapterName, str);
			LoopAllChapters(Chapter_SetName, (LPARAM)str.c_str(), 0, true);
		}
	}
	else if(wnd == &m_dramaProp)
	{
		if(pProp == m_dramaName)
		{
			std::wstring str;
			GetPropText(m_dramaName, str);
			LoopAllDramas(Drama_SetName, (LPARAM)str.c_str(), 0, true);
		}
	}
	else if(wnd == &m_actionProp)
	{
		if(pProp == m_actionName)
		{
			std::wstring str;
			GetPropText(m_actionName, str);
			LoopAllActions(Action_SetName, (LPARAM)str.c_str(), 0, true);
		}
	}
	else if(wnd == &m_stateProp)
	{
		if(pProp == m_stateName)
		{
			std::wstring str;
			GetPropText(m_stateName, str);
			LoopAllStates(State_SetName, (LPARAM)str.c_str(), 0, true);
		}
		else if(pProp == m_stateElapsedTime)
		{
			LoopAllStates(State_SetElapsedTime, (LPARAM)GetPropFloatPtr(m_stateElapsedTime), 0, true);
		}
		else if(pProp == m_stateIndex)
		{
			//LoopAllStates(State_SetCallIndex, GetPropBool(m_stateIndex), 0, false);
			////
			//std::vector<CPlotState*> states;
			//std::map<HTREEITEM, CPlotAction*> acts;
			//for(int i=0;i<this->m_treeView.GetSelectCount();i++)
			//{
			//	HTREEITEM hItem		=	m_treeView.GetSelect(i);
			//	TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			//	if(type && type->type == GAMEPROJ_STATE)
			//	{
			//		CPlotAction* ch		=	m_gameDirector.SearchAction(type->p1);
			//		CPlotState* state	=	ch->GetStatePtrByID(type->p2);
			//		acts[this->m_treeView.GetParentItem(hItem)] = ch;
			//	}
			//}
			//for(std::map<HTREEITEM, CPlotAction*>::iterator it = acts.begin(); it != acts.end(); it++)
			//{
			//	it->second->SortPlotStates();
			//	this->RefreshStates(it->second, it->first);
			//	this->m_treeView.Expand(it->first, TVE_EXPAND);
			//}
		}
		else
		{
			for(int s=0;s<MAX_SCRIPT_PARAMETER;s++)
			{
				if(this->m_stateScriptParam[s] == pProp)
				{
					std::wstring str;
					GetPropText(m_stateScriptParam[s], str);
					//
					for(int i=0;i<this->m_treeView.GetSelectCount();i++)
					{
						HTREEITEM hItem		=	m_treeView.GetSelect(i);
						TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
						if(type && type->type == GAMEPROJ_STATE)
						{
							CPlotAction* ch	=	m_gameDirector.SearchAction(type->p1);
							if(ch)
							{
								CPlotState* state	=	ch->GetStatePtrByID(type->p2);
								SetScriptParameter(state->m_scriptParameter[s], state, str.c_str());
								if(state->m_scriptParameter[s].id == state->GetID())
								{
									state->m_scriptParameter[s].id		=	0;
									state->m_scriptParameter[s].type	=	PlotType_Unknown;
								}
								//if(this->m_luaScript)
								//	this->m_luaScript.RefreshScriptParameters();
								//(*proc)(state, nParameter, nParameter2);
								SetModifyed(true);
							}
						}
					}
					break;
				}
			}
		}
	}
	else if(wnd == &m_eventProp)
	{
		if(pProp == m_eventName)
		{
			std::wstring str;
			GetPropText(m_eventName, str);
			//LoopAllEvents(Event_SetName, (LPARAM)str.c_str(), 0, true);
		}
		else
		{
			//for(int i=0;i<MAX_EVENT_PARAM_COUNT;i++)
			//{
			//	if(pProp == m_eventIDList[i])
			//	{
			//		std::wstring str;
			//		GetPropText(m_eventIDList[i], str);
			//		LoopAllEvents(Event_SetID, i, (LPARAM)str.c_str());
			//		break;
			//	}
			//}
			for(int s=0;s<MAX_SCRIPT_PARAMETER;s++)
			{
				if(this->m_eventScriptParam[s] == pProp)
				{
					std::wstring str;
					GetPropText(m_eventScriptParam[s], str);
					//
					for(int i=0;i<this->m_treeView.GetSelectCount();i++)
					{
						HTREEITEM hItem		=	m_treeView.GetSelect(i);
						TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
						if(type && type->type == GAMEPROJ_EVENT)
						{
							CPlotPropertiesMap* ch	=	m_gameDirector.SearchPropertiesMap(type->p1);
							if(ch)
							{
								CPlotEventCall* state	=	ch->GetEventCallPtrByID(type->p2);
								//
								SetScriptParameter(state->m_scriptParameter[s], state, str.c_str());
								if(state->m_scriptParameter[s].id == state->GetID())
								{
									state->m_scriptParameter[s].id		=	0;
									state->m_scriptParameter[s].type	=	PlotType_Unknown;
								}
								//if(this->m_luaScript)
								//	this->m_luaScript.RefreshScriptParameters();
								//(*proc)(state, nParameter, nParameter2);
								SetModifyed(true);
							}
						}
					}
					break;
				}
			}
		}
	}
	else if(wnd == &m_variantProp)
	{
		if(pProp == m_variantName)
		{
			std::wstring str;
			GetPropText(m_variantName, str);
			LoopAllVariant(Variant_SetName, (LPARAM)str.c_str(), 0, true);
		}
		else if(pProp == m_variantInt)
			LoopAllVariant(Variant_Int, GetPropInt(m_variantInt));
		else if(pProp == m_variantDouble)
			LoopAllVariant(Variant_Double, (LPARAM)GetPropFloatPtr(m_variantDouble));
		else if(pProp == m_variantText)
		{
			std::wstring str;
			GetPropText(m_variantText, str);
			LoopAllVariant(Variant_Text, (LPARAM)str.c_str());
		}
		else if(pProp == m_variantF1)
			LoopAllVariant(Variant_Float1, (LPARAM)GetPropFloatPtr(m_variantF1));
		else if(pProp == m_variantF2)
			LoopAllVariant(Variant_Float2, (LPARAM)GetPropFloatPtr(m_variantF2));
		else if(pProp == m_variantF3)
			LoopAllVariant(Variant_Float3, (LPARAM)GetPropFloatPtr(m_variantF3));
		else if(pProp == m_variantF4)
			LoopAllVariant(Variant_Float4, (LPARAM)GetPropFloatPtr(m_variantF4));
		//
		else if(pProp == m_variantI1)
			LoopAllVariant(Variant_Int1, GetPropInt(m_variantI1));
		else if(pProp == m_variantI2)
			LoopAllVariant(Variant_Int2, GetPropInt(m_variantI2));
		else if(pProp == m_variantI3)
			LoopAllVariant(Variant_Int3, GetPropInt(m_variantI3));
		else if(pProp == m_variantI4)
			LoopAllVariant(Variant_Int4, GetPropInt(m_variantI4));
	}
	else if(wnd == &m_callVarWnd && this->m_selectState && this->m_selectVarSet)
	{
		if(pProp == m_callVarIDs)
		{
			CDynamicArray<CPlotVariant*> vars;
			m_selectState->GetActionPtr()->GetAllPlotVariants(vars);
			{
				std::wstring str;
				GetPropText(m_callVarIDs, str);
				int varID	=	0;
				for(int i=0;i<vars.size();i++)
				{
					wchar_t buf[128];
					swprintf(buf, L"%s[%d]", vars[i]->GetName(), vars[i]->GetID());
					if(wcsicmp(buf, str.c_str()) == 0)
					{
						varID	=	vars[i]->m_variantID;
						break;
					}
				}
				LoopAllVariantSet(VariantSet_VariantID, varID);
			}
		}
		else if(pProp == m_callVarOtherIDs)
		{
			//CPlotVariant* var	=	m_gameDirector.SearchVariant(m_selectVarSet->m_value.otherVariantID);
			//if(var)
			{
				std::wstring str;
				GetPropText(m_callVarOtherIDs, str);
				int varID	=	0;
				CDynamicArray<CPlotVariant*> vars;
				m_selectState->GetActionPtr()->GetAllPlotVariants(vars);
				for(int i=0;i<vars.size();i++)
				{
					wchar_t buf[128];
					swprintf(buf, L"%s[%d]", vars[i]->GetName(), vars[i]->GetID());
					if(wcsicmp(buf, str.c_str()) == 0)
					{
						varID	=	vars[i]->m_variantID;
						break;
					}
				}
				LoopAllVariantSet(VariantSet_OtherVariantID, varID);
			}
		}
		else if(pProp == m_callVarInt)
			LoopAllVariantSet(VariantSet_Int, GetPropInt(m_callVarInt));
		else if(pProp == m_callVarDouble)
			LoopAllVariantSet(VariantSet_Double, (LPARAM)GetPropFloatPtr(m_callVarDouble));
		else if(pProp == m_callVarText)
		{
			std::wstring str;
			GetPropText(m_callVarText, str);
			LoopAllVariantSet(VariantSet_Text, (LPARAM)str.c_str());
		}
		else if(pProp == m_callVarF1)
			LoopAllVariantSet(VariantSet_Float1, (LPARAM)GetPropFloatPtr(m_callVarF1));
		else if(pProp == m_callVarF2)
			LoopAllVariantSet(VariantSet_Float2, (LPARAM)GetPropFloatPtr(m_callVarF2));
		else if(pProp == m_callVarF3)
			LoopAllVariantSet(VariantSet_Float3, (LPARAM)GetPropFloatPtr(m_callVarF3));
		else if(pProp == m_callVarF4)
			LoopAllVariantSet(VariantSet_Float4, (LPARAM)GetPropFloatPtr(m_callVarF4));
		//
		else if(pProp == m_callVarI1)
			LoopAllVariantSet(VariantSet_Int1, GetPropInt(m_callVarI1));
		else if(pProp == m_callVarI2)
			LoopAllVariantSet(VariantSet_Int2, GetPropInt(m_callVarI2));
		else if(pProp == m_callVarI3)
			LoopAllVariantSet(VariantSet_Int3, GetPropInt(m_callVarI3));
		else if(pProp == m_callVarI4)
			LoopAllVariantSet(VariantSet_Int4, GetPropInt(m_callVarI4));
	}
	else if(wnd == &this->m_callPropWnd && this->m_selectState && this->m_selectPropSet)
	{
		if(pProp == m_callPropActions)
		{
			std::wstring str;
			GetPropText(m_callPropActions, str);
			CDynamicArray<CPlotPropertiesMap*>	optionProperties;
			m_selectState->GetActionPtr()->GetAllPlotPropertiesMap(optionProperties);
			//
			int selID	=	0;
			this->m_callPropIDs->RemoveAllOptions();
			m_callPropIDs->AddOption(Language(L""));
			for(int i=0;i<optionProperties.size();i++)
			{
				wchar_t buf[128];
				swprintf(buf, L"%s[%d]", GetPlotName(optionProperties[i]), optionProperties[i]->GetID());
				if(wcsicmp(buf, str.c_str()) == 0)
				{
					selID	=	optionProperties[i]->GetID();
					for(int h=0;h<optionProperties[i]->GetPropertyCount();h++)
					{
						swprintf(buf, L"%s[%d]", optionProperties[i]->GetPropertyPtr(h)->GetName(), optionProperties[i]->GetPropertyPtr(h)->GetID());
						m_callPropIDs->AddOption(buf, false);
					}
					break;
				}
			}
			//
			LoopAllPropertyCall(PropertyCall_PropertiesMapID, selID);
			SetPropertyOption(m_selectPropSet);
		}
		else if(pProp == m_callPropIDs)
		{
			CPlotPropertiesMap* action	=	m_gameDirector.SearchPropertiesMap(m_selectPropSet->m_propertiesMapID);
			if(action)
			{
				std::wstring str;
				GetPropText(m_callPropIDs, str);
				int varID	=	0;
				for(int i=0;i<action->GetPropertyCount();i++)
				{
					wchar_t buf[128];
					swprintf(buf, L"%s[%d]", action->GetPropertyPtr(i)->GetName(), action->GetPropertyPtr(i)->GetID());
					if(wcsicmp(buf, str.c_str()) == 0)
					{
						varID	=	action->GetPropertyPtr(i)->GetID();
						this->m_selectPropSet->m_paramCount	=	action->GetPropertyPtr(i)->GetParameterCount();
						break;
					}
				}
				LoopAllPropertyCall(PropertyCall_PropertiesID, varID);
			}
			SetPropertyOption(m_selectPropSet);
		}
		else
		{
			for(int p=0;p<this->m_selectPropSet->m_paramCount;p++)
			{
				CPlotPropertiesMap* propMap	=	m_gameDirector.SearchPropertiesMap(m_selectPropSet->m_propertiesMapID);
				CPlotProperties* prop		=	propMap->GetPropertyPtrByID(m_selectPropSet->m_propertyCall);
				if(!m_callPropParamGroup[p]->IsVisible() || !propMap || !prop)
					continue;
				PlotValueList* list	=	propMap->GetPropParameter(m_selectPropSet->m_propertyCall, p);
				if(pProp == m_callPropParamOtherIDs[p])
				{
					//CPlotAction* action	=	m_gameDirector.SearchAction(this->m_selectPropSet->m_params[p].otherActionID);
					//if(action)
					{
						CDynamicArray<CPlotVariant*> vars;
						m_selectState->GetActionPtr()->GetAllPlotVariants(vars);
						std::wstring str;
						GetPropText(m_callPropParamOtherIDs[p], str);
						int varID	=	0;
						for(int i=0;i<vars.size();i++)
						{
							wchar_t buf[128];
							swprintf(buf, L"%s[%d]", vars[i]->GetName(), vars[i]->GetID());
							if(wcsicmp(buf, str.c_str()) == 0)
							{
								varID	=	vars[i]->m_variantID;
								break;
							}
						}
						LoopAllPropertyCall(PropertyCall_OtherVariantID, p, varID);
					}
				}
				else if(pProp == m_callPropParamInt[p])
					LoopAllPropertyCall(PropertyCall_Int, p, GetPropInt(m_callPropParamInt[p]));
				else if(pProp == m_callPropParamColor[p])
					LoopAllPropertyCall(PropertyCall_Int, p, color4(GetPropColor(m_callPropParamColor[p]), 1.0f));
				else if(pProp == m_callPropParamDouble[p])
					LoopAllPropertyCall(PropertyCall_Double, p, (LPARAM)GetPropFloatPtr(m_callPropParamDouble[p]));
				else if(pProp == m_callPropParamText[p])
				{
					std::wstring str;
					GetPropText(m_callPropParamText[p], str);
					LoopAllPropertyCall(PropertyCall_Text, p, (LPARAM)str.c_str());
				}
				else if(pProp == m_callPropParamList[p])
				{
					std::wstring str;
					GetPropText(m_callPropParamList[p], str);
					//
					switch(list->pvlType)
					{
					case PlotValueList::PVL_None:
						{
							int value	=	0;
							wchar_t buf[128];
							for(int i=0;i<list->values.size();i++)
							{
								swprintf(buf, L"%s[%d]", list->values[i].name.str(), list->values[i].value);
								if(wcsicmp(buf, str.c_str()) == 0)
								{
									value	=	list->values[i].value;
									break;
								}
							}
							LoopAllPropertyCall(PropertyCall_Int, p, value);
						}
						break;
					case PlotValueList::PVL_DramaList:
						{
							int value	=	0;
							for(int i=0;i<propMap->GetChapterPtr()->GetDramaCount();i++)
							{
								CDrama* pDrama		=	propMap->GetChapterPtr()->GetDramaPtr(i);
								wchar_t buf[128];
								swprintf(buf, L"%s[%d]", GetPlotName(pDrama), pDrama->GetID());
								if(wcsicmp(buf, str.c_str()) == 0)
								{
									value	=	pDrama->GetID();
									break;
								}
							}
							LoopAllPropertyCall(PropertyCall_Int, p, value);
						}
						break;
					case PlotValueList::PVL_ChapterList:
						{
							int value	=	0;
							for(int i=0;i<propMap->GetProjectPtr()->GetChapterCount();i++)
							{
								CChapter* pChapter	=	propMap->GetProjectPtr()->GetChapterPtr(i);
								wchar_t buf[128];
								swprintf(buf, L"%s[%d]", GetPlotName(pChapter), pChapter->GetID());
								if(wcsicmp(buf, str.c_str()) == 0)
								{
									value	=	pChapter->GetID();
									break;
								}
							}
							LoopAllPropertyCall(PropertyCall_Int, p, value);
						}
						break;
					case PlotValueList::PVL_ActionList:
						{
							CDynamicArray<CPlotAction*> actions;
							propMap->GetProcessPtr()->GetAllPlotActions(actions);
							int value	=	0;
							for(int i=0;i<actions.size();i++)
							{
								CPlotAction* action	=	actions[i];
								wchar_t buf[128];
								swprintf(buf, L"%s[%d]", GetPlotName(action), action->GetID());
								if(wcsicmp(buf, str.c_str()) == 0)
								{
									value	=	action->GetID();
									break;
								}
							}
							LoopAllPropertyCall(PropertyCall_Int, p, value);
						}
						break;
					//case PlotValueList::PVL_SelectUIControl:
					//	{
					//		if(propMap->GetPlotType() == PlotType_ElementUI)
					//		{
					//			CPlotElementUI* ui	=	static_cast<CPlotElementUI*>(propMap);
					//			int value	=	0;
					//			for(int i=0;i<ui->m_controls.size();i++)
					//			{
					//				wchar_t buf[128];
					//				swprintf(buf, L"%s[%d]", ui->m_controls[i].name.str(), ui->m_controls[i].value);
					//				if(wcsicmp(buf, str.c_str()) == 0)
					//				{
					//					value	=	ui->m_controls[i].value;
					//					break;
					//				}
					//			}
					//			LoopAllPropertyCall(PropertyCall_Int, p, value);
					//		}
					//	}
					//	break;
					//case PlotValueList::PVL_SelectUIAnimation:
					//	{
					//		if(propMap->GetPlotType() == PlotType_ElementUI)
					//		{
					//			CPlotElementUI* ui	=	static_cast<CPlotElementUI*>(propMap);
					//			int value	=	0;
					//			for(int i=0;i<ui->m_controls.size();i++)
					//			{
					//				wchar_t buf[128];
					//				swprintf(buf, L"%s[%d]", ui->m_controls[i].c.name.str(), ui->m_controls[i].c.value);
					//				if(wcsicmp(buf, str.c_str()) == 0)
					//				{
					//					value	=	ui->m_controls[i].c.value;
					//					break;
					//				}
					//			}
					//			LoopAllPropertyCall(PropertyCall_Int, p, value);
					//		}
					//	}
					//	break;
					case PlotValueList::PVL_VariantList:
						{
							CDynamicArray<CPlotVariant*> vars;
							propMap->GetProcessPtr()->GetAllPlotVariants(vars);
							int value	=	0;
							for(int i=0;i<vars.size();i++)
							{
								CPlotVariant* var	=	vars[i];
								wchar_t buf[128];
								swprintf(buf, L"%s[%d]", var->GetName(), var->GetID());
								if(wcsicmp(buf, str.c_str()) == 0)
								{
									value	=	var->GetID();
									break;
								}
							}
							LoopAllPropertyCall(PropertyCall_Int, p, value);
						}
						break;
					case PlotValueList::PVL_CallList:
						{
							CDynamicArray<CPlotState*> vars;
							CPlotAction* action	=	propMap->GetActionPtr();
							if(action)
							{
								int value	=	0;
								for(int i=0;i<action->GetStateCount();i++)
								{
									wchar_t buf[128];
									swprintf(buf, L"%s[%d]", action->GetStatePtr(i)->GetName(), action->GetStatePtr(i)->GetID());
									if(wcsicmp(buf, str.c_str()) == 0)
									{
										value	=	action->GetStatePtr(i)->GetID();
										break;
									}
								}
								LoopAllPropertyCall(PropertyCall_Int, p, value);
							}
						}
						break;
					}
				}
			}
		}
	}

}

VOID		GCGameProject::OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem)	//更新选择树形控件
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
		TreeItemType* type	=	this->GetSelectedItem();
		if(!type)
		{
			SetCurrentProperties(&m_viewerProps, Language(L"文件属性"), NULL);
			return;
		}
		switch(type->type)
		{
		case GAMEPROJ_MAIN:
			{
				SetSelectNone();
				m_gameProp.EnableRender(false);
				SetCurrentProperties(&m_gameProp, Language(L"游戏属性"), &m_dlgVariant);
				SetPropText(m_gameName, m_gameDirector.GetName());
				SetPropText(m_gameCreatorName, m_gameDirector.m_creatorName);
				SetPropText(m_gameVersion, m_gameDirector.m_versionText);
				SetPropText(m_gameCopyright, m_gameDirector.m_copyright);
				SetPropInt(m_gameScreenW, m_gameDirector.GetScreenSize().cx);
				SetPropInt(m_gameScreenH, m_gameDirector.GetScreenSize().cy);
				m_dlgVariant.SetActionPtr(&m_gameDirector);
				m_gameProp.EnableRender(true);
			}
			break;
		case GAMEPROJ_CHAPTER:
		case GAMEPROJ_MODULE:
			{
				SetSelectNone();
				SetCurrentProperties(&m_chapterProp, Language(L"章节属性"), &m_dlgVariant);
				CChapter* chapter	=	m_gameDirector.GetChapterPtrByID(type->p1);
				m_chapterProp.EnableRender(false);
				if(!chapter)
				{
					SetCurrentProperties(NULL, 0, NULL);
					m_chapterProp.EnableRender(true);
					return;
				}
				SetPropText(m_chapterName, chapter->GetName());
				m_dlgVariant.SetActionPtr(chapter);
				m_chapterProp.EnableRender(true);
			}
			break;
		case GAMEPROJ_DRAMA:
			{
				SetSelectNone();
				SetCurrentProperties(&m_dramaProp, Language(L"篇章属性"), &m_dlgVariant);
				CDrama* drama	=	m_gameDirector.SearchDrama(type->p1);
				m_dramaProp.EnableRender(false);
				if(!drama)
				{
					SetCurrentProperties(NULL, 0, NULL);
					m_dramaProp.EnableRender(true);
					return;
				}
				SetPropText(m_dramaName, drama->GetName());
				m_dlgVariant.SetActionPtr(drama);
				m_dramaProp.EnableRender(true);
			}
			break;
		case GAMEPROJ_ACTION:
			{
				SetSelectNone();
				SetCurrentProperties(&m_actionProp, Language(L"动作属性"), &m_dlgVariant);
				CPlotAction* action	=	m_gameDirector.SearchAction(type->p2);
				m_actionProp.EnableRender(false);
				if(!action)
				{
					SetCurrentProperties(NULL, 0, NULL);
					m_actionProp.EnableRender(true);
					return;
				}
				SetPropText(m_actionName, action->GetName());
				m_dlgVariant.SetActionPtr(action);
				m_actionProp.EnableRender(true);
			}
			break;
		case GAMEPROJ_STATE:
			{
				SetSelectNone();
				SetCurrentProperties(&m_stateProp, Language(L"状态属性"), &m_callPlot);
				CPlotAction* action	=	m_gameDirector.SearchAction(type->p1);
				CPlotState* state	=	action ? action->GetStatePtrByID(type->p2) : 0;
				m_stateProp.EnableRender(false);
				if(!action || !state)
				{
					//m_luaScript.SetScriptPtr(0, 0);
					SetCurrentProperties(NULL, 0, NULL);
					m_stateProp.EnableRender(true);
					return;
				}
				SetPropText(m_stateName, state->GetName());
				//SetPropOption(m_stateIndex, g_typeCallIndex, state->GetCallIndex());
				SetPropFloat(m_stateElapsedTime, state->GetElapsedTime());
				m_callPlot.SetPlotCall(state);
				m_stateProp.EnableRender(true);
				//
				InitScriptParameter(this->m_stateScriptParam, state);
				//
				//GetMainFrame()->m_wndClassView.SetChildWnd(m_luaScript.m_hWnd, L"脚本编辑器");
				//m_stateProp.SetChildWindow(&m_callPlot, 300);
				//m_luaScript.SetScriptPtr(state, &m_stateProp);
			}
			break;
		case GAMEPROJ_EVENT:
			{
				SetSelectNone();
				SetCurrentProperties(&m_eventProp, Language(L"事件属性"), &m_callPlot);
				CPlotPropertiesMap* action	=	m_gameDirector.SearchPropertiesMap(type->p1);
				CPlotEventCall* state		=	action ? action->GetEventCallPtrByID(type->p2) : 0;
				m_eventProp.EnableRender(false);
				if(!action || !state)
				{
					//m_luaScript.SetScriptPtr(0, 0);
					SetCurrentProperties(NULL, 0, NULL);
					m_eventProp.EnableRender(true);
					return;
				}
				SetPropText(m_eventName, GetEventName(state->GetEventID()));//state->GetName());
				//
				std::vector<std::string>	rets;
				GetEventParameterType(rets, (EventID)state->GetEventID());
				//CPlotEvent* evt	=	action->GetEventPtrByID((EventID)state->GetEventID());
				//
				wchar_t buf[128];
				//if(evt)
				{
					for(int i=0;i<MAX_EVENT_PARAM_COUNT;i++)
					{
						if(rets[i].empty())
						{
							m_eventIDList[i]->Show(false);
							continue;
						}
						m_eventIDList[i]->Show(true);
						m_eventIDList[i]->SetName(GetEventParameterName(i));
						SetPropText(m_eventIDList[i], W(rets[i].c_str()));
					}
				}
				//else
				//{
				//	for(int i=0;i<MAX_EVENT_PARAM_COUNT;i++)
				//	{
				//		m_eventIDList[i]->Show(false);
				//	}
				//}
				m_callPlot.SetPlotCall(state);
				m_eventProp.EnableRender(true);
				//
				InitScriptParameter(this->m_eventScriptParam, state);
				//
				//GetMainFrame()->m_wndClassView.SetChildWnd(m_luaScript.m_hWnd, L"脚本编辑器");
				//m_eventProp.SetChildWindow(&m_callPlot, 180);
				//m_luaScript.SetScriptPtr(state, &m_eventProp);
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

VOID		GCGameProject::InitChapterProp()
{
	CMFCPropertyGridProperty* pGroup1		= new CMFCPropertyGridProperty(Language(L"基本属性"));
	pGroup1->AddSubItem(m_chapterName		= AddPropText(Language(L"章节名称"), Language(L"章节名称_说明", L"所选的章节名称"), false, Name::Length));
	m_chapterProp.AddProperty(pGroup1);
}
VOID		GCGameProject::InitDramaProp()
{
	CMFCPropertyGridProperty* pGroup1		= new CMFCPropertyGridProperty(Language(L"基本属性"));
	pGroup1->AddSubItem(m_dramaName			= AddPropText(Language(L"篇章名称"), Language(L"篇章名称_说明", L"所选的篇章名称"), false, Name::Length));
	m_dramaProp.AddProperty(pGroup1);
}
VOID		GCGameProject::InitActionProp()
{
	CMFCPropertyGridProperty* pGroup1		= new CMFCPropertyGridProperty(Language(L"基本属性"));
	pGroup1->AddSubItem(m_actionName		= AddPropText(Language(L"动作名称"), Language(L"动作名称_说明", L"所选动作的动作名称"), false, Name::Length));
	m_actionProp.AddProperty(pGroup1);
}
VOID		GCGameProject::InitStateProp()
{
	CMFCPropertyGridProperty* pGroup1		= new CMFCPropertyGridProperty(Language(L"基本属性"));
	pGroup1->AddSubItem(m_stateName			= AddPropText(Language(L"状态名称"), Language(L"状态名称_说明", L"所选动作的状态名称"), false, Name::Length));
	//pGroup1->AddSubItem(m_stateIndex		= AddPropOption(Language(L"调用顺序"), Language(L"状态顺序_说明", L"所选动作的状态调用顺序"), g_typeCallIndex));
	pGroup1->AddSubItem(m_stateElapsedTime		= AddPropFloat(Language(L"调用间隔"), Language(L"调用间隔_说明", L"所选动作的状态调用间隔"), false, 0.0f, 100000.0f));
	CMFCPropertyGridProperty* pGroup2		= new CMFCPropertyGridProperty(Language(L"脚本参数列表"));
	for(int i=0;i<MAX_SCRIPT_PARAMETER;i++)
	{
		wchar_t buf[128];
		swprintf(buf, L"v%d", i+1);
		pGroup2->AddSubItem(m_stateScriptParam[i] = AddPropOption(buf, Language(L"脚本参数1_说明", L"所选项目的状态脚本参数1"), g_typeDefault));
	}
	m_stateProp.AddProperty(pGroup1);
	m_stateProp.AddProperty(pGroup2);
}

VOID		GCGameProject::InitEventProp()
{
	CMFCPropertyGridProperty* pGroup1		= new CMFCPropertyGridProperty(Language(L"基本属性"));
	pGroup1->AddSubItem(m_eventName			= AddPropText(Language(L"事件调用名称"), Language(L"事件调用名称_说明", L"所选动作的事件所对应的，调用名称"), false, Name::Length));
	m_eventName->AllowEdit(false);
	for(int i=0;i<MAX_EVENT_PARAM_COUNT;i++)
	{
		wchar_t buf[128];
		swprintf(buf, L"e%d", i+1);
		pGroup1->AddSubItem(m_eventIDList[i]		= AddPropText(buf, Language(L"事件调用选项1_说明", L"所选项目的事件调用选项1")));
		m_eventIDList[i]->AllowEdit(false);
	}
	CMFCPropertyGridProperty* pGroup2		= new CMFCPropertyGridProperty(Language(L"脚本参数列表"));
	for(int i=0;i<MAX_SCRIPT_PARAMETER;i++)
	{
		wchar_t buf[128];
		swprintf(buf, L"v%d", i+1);
		pGroup2->AddSubItem(m_eventScriptParam[i] = AddPropOption(buf, Language(L"脚本参数1_说明", L"所选项目的状态脚本参数1"), g_typeDefault));
	}
	m_eventProp.AddProperty(pGroup1);
	m_eventProp.AddProperty(pGroup2);
}

BOOL		GCGameProject::SetScriptParameter2(ScriptParameter& ret, CPlotAction* act, const wchar_t* szText)
{
	ret.id		=	0;
	ret.type	=	::PlotType_Unknown;
	if(szText[0] == 0)
		return true;
	//
	wchar_t wbuf[256];
	const wchar_t* szPtr	=	0;
	wcscpy(wbuf, GetPlotName(act));
	if(wcsicmp(wbuf, szText) == 0)
	{
		ret.id		=	act->GetID();
		ret.type	=	act->GetPlotType();
		return true;
	}
	if(act->GetProcessPtr())
	{
		for(int i=0;i<act->GetProcessPtr()->GetActionCount();i++)
		{
			szPtr	=	GetPlotName(act->GetProcessPtr()->GetActionPtr(i));
			if(wcsicmp(szPtr, szText) == 0)
			{
				ret.id		=	act->GetProcessPtr()->GetActionPtr(i)->GetID();
				ret.type	=	act->GetProcessPtr()->GetActionPtr(i)->GetPlotType();
				return true;
			}
		}
		for(int i=0;i<act->GetProcessPtr()->GetElementCount();i++)
		{
			szPtr	=	GetPlotName(act->GetProcessPtr()->GetElementPtr(i));
			if(wcsicmp(szPtr, szText) == 0)
			{
				ret.id		=	act->GetProcessPtr()->GetElementPtr(i)->GetID();
				ret.type	=	act->GetProcessPtr()->GetElementPtr(i)->GetPlotType();
				return true;
			}
		}
	}
	for(int i=0;i<act->GetStateCount();i++)
	{
		szPtr	=	GetPlotName(act->GetStatePtr(i)->GetPlotType(), act->GetStatePtr(i), 0);
		if(wcsicmp(szPtr, szText) == 0)
		{
			ret.id		=	act->GetStatePtr(i)->GetID();
			ret.type	=	act->GetStatePtr(i)->GetPlotType();
			return true;
		}
	}
	wchar_t wbufVar[256];
	for(int i=0;i<act->GetVariantCount();i++)
	{
		swprintf(wbufVar, L"[var:%s]:%s", act->GetVariantPtr(i)->GetName(), wbuf);
		if(wcsicmp(wbufVar, szText) == 0)
		{
			ret.id		=	act->GetVariantPtr(i)->GetID();
			ret.type	=	PlotType_Variant;
			return true;
		}
	}
	return false;
}

VOID		GCGameProject::SetScriptParameter(ScriptParameter& ret, CPlotState* c, const wchar_t* szText)
{
	if(SetScriptParameter2(ret, c->GetProjectPtr(), szText))
		return;
	if(c->GetChapterPtr())
		if(SetScriptParameter2(ret, c->GetChapterPtr(), szText))
			return;
	if(c->GetDramaPtr())
		if(SetScriptParameter2(ret, c->GetDramaPtr(), szText))
			return;
	if(c->GetActionPtr())
		if(SetScriptParameter2(ret, c->GetActionPtr(), szText))
			return;
}

VOID		GCGameProject::InitScriptParameter(PropertyPtr params, CPlotAction* act, int id, wchar_t* ret)
{
	wchar_t wbuf[256];
	const wchar_t* szPtr	=	0;
	wcscpy(wbuf, GetPlotName(act));
	params->AddOption(wbuf);
	if(id == act->GetID())
		wcscpy(ret, wbuf);
	if(act->GetProcessPtr())
	{
		for(int i=0;i<act->GetProcessPtr()->GetActionCount();i++)
		{
			szPtr	=	GetPlotName(act->GetProcessPtr()->GetActionPtr(i));
			params->AddOption(szPtr);
			if(id == act->GetProcessPtr()->GetActionPtr(i)->GetID())
				wcscpy(ret, szPtr);
		}
		for(int i=0;i<act->GetProcessPtr()->GetElementCount();i++)
		{
			szPtr	=	GetPlotName(act->GetProcessPtr()->GetElementPtr(i));
			params->AddOption(szPtr);
			if(id == act->GetProcessPtr()->GetElementPtr(i)->GetID())
				wcscpy(ret, szPtr);
		}
	}
	for(int i=0;i<act->GetStateCount();i++)
	{
		szPtr	=	GetPlotName(act->GetStatePtr(i)->GetPlotType(), act->GetStatePtr(i), 0);
		params->AddOption(szPtr);
		if(id == act->GetStatePtr(i)->GetID())
			wcscpy(ret, szPtr);
	}
	wchar_t wbufVar[256];
	for(int i=0;i<act->GetVariantCount();i++)
	{
		swprintf(wbufVar, L"[var:%s]:%s", act->GetVariantPtr(i)->GetName(), wbuf);
		params->AddOption(wbufVar);
		if(id == act->GetVariantPtr(i)->GetID())
			wcscpy(ret, wbufVar);
	}
}

VOID		GCGameProject::InitScriptParameter(PropertyPtr* params, CPlotState* c)
{
	for(int i=0;i<MAX_SCRIPT_PARAMETER;i++)
	{
		params[i]->SetName(c->GetScriptParameterName(i));
		params[i]->RemoveAllOptions();
		params[i]->AddOption(L"");
		wchar_t ret[256];
		ret[0] = 0;
		//
		InitScriptParameter(params[i], c->GetProjectPtr(), c->m_scriptParameter[i].id, ret);
		if(c->GetChapterPtr())
			InitScriptParameter(params[i], c->GetChapterPtr(), c->m_scriptParameter[i].id, ret);
		if(c->GetDramaPtr())
			InitScriptParameter(params[i], c->GetDramaPtr(), c->m_scriptParameter[i].id, ret);
		if(c->GetActionPtr())
			InitScriptParameter(params[i], c->GetActionPtr(), c->m_scriptParameter[i].id, ret);
		SetPropText(params[i], ret);
	}
}

VOID		GCGameProject::InitGameProp()
{
	CMFCPropertyGridProperty* pGroup1		= new CMFCPropertyGridProperty(Language(L"项目属性"));
	pGroup1->AddSubItem(m_gameName			= AddPropText(Language(L"项目名称"), Language(L"项目名称_说明", L"所选项目的项目名称"), false, Name::Length));
	pGroup1->AddSubItem(m_gameCreatorName	= AddPropText(Language(L"作者名称"), Language(L"作者名称_说明", L"所选项目的作者名称"), false, Name::Length));
	pGroup1->AddSubItem(m_gameVersion		= AddPropText(Language(L"版本相关"), Language(L"版本相关_说明", L"所选项目的版本相关"), false, 16));
	pGroup1->AddSubItem(m_gameCopyright		= AddPropText(Language(L"版权声明"), Language(L"版权声明_说明", L"所选项目的版权声明"), false, Name::Length));
	CMFCPropertyGridProperty* pGroup2		= new CMFCPropertyGridProperty(Language(L"游戏相关"));
	pGroup2->AddSubItem(m_gameScreenW		= AddPropInt(Language(L"屏幕分辨率(宽度)"), Language(L"屏幕宽度_说明", L"所选项目的屏幕分辨率(宽度)"), 32, 1680));
	pGroup2->AddSubItem(m_gameScreenH		= AddPropInt(Language(L"屏幕分辨率(高度)"), Language(L"屏幕高度_说明", L"所选项目的屏幕分辨率(高度)"), 32, 1680));
	m_gameProp.AddProperty(pGroup1);
	m_gameProp.AddProperty(pGroup2);
}

VOID		GCGameProject::InitCallVariantSet()
{
	CMFCPropertyGridProperty* pGroup1		= new CMFCPropertyGridProperty(Language(L"变量设置属性"));
	pGroup1->AddSubItem(m_callVarIDs		= AddPropOption(Language(L"变量名称"), Language(L"变量名称_说明", L"设置指定动作的，指定变量的值，所选项目的变量名称"), g_typeDefault));
	//
	CMFCPropertyGridProperty* pGroup2		= new CMFCPropertyGridProperty(Language(L"赋值设置"));
	pGroup2->AddSubItem(m_callVarInt		= AddPropInt(Language(L"整数值"), Language(L"整数值_说明", L"运行到此时设置的整数值")));
	pGroup2->AddSubItem(m_callVarDouble		= AddPropFloat(Language(L"小数值"), Language(L"小数值_说明", L"运行到此时设置的小数值")));
	pGroup2->AddSubItem(m_callVarText		= AddPropText(Language(L"文本值"), Language(L"文本值_说明", L"运行到此时设置的文本值"), true, 10000000));
	CMFCPropertyGridProperty* pGroup3		= new CMFCPropertyGridProperty(Language(L"其他赋值设置"));
	pGroup3->AddSubItem(m_callVarF1			= AddPropFloat(Language(L"小数1"), Language(L"小数_说明", L"运行到此时设置的小数，和小数值不是一个相同属性")));
	pGroup3->AddSubItem(m_callVarF2			= AddPropFloat(Language(L"小数2"), Language(L"小数_说明", L"运行到此时设置的小数，和小数值不是一个相同属性")));
	pGroup3->AddSubItem(m_callVarF3			= AddPropFloat(Language(L"小数3"), Language(L"小数_说明", L"运行到此时设置的小数，和小数值不是一个相同属性")));
	pGroup3->AddSubItem(m_callVarF4			= AddPropFloat(Language(L"小数4"), Language(L"小数_说明", L"运行到此时设置的小数，和小数值不是一个相同属性")));
	pGroup3->AddSubItem(m_callVarI1			= AddPropInt(Language(L"整数1"), Language(L"整数_说明", L"运行到此时设置的整数，和整数值不是一个相同属性")));
	pGroup3->AddSubItem(m_callVarI2			= AddPropInt(Language(L"整数2"), Language(L"整数_说明", L"运行到此时设置的整数，和整数值不是一个相同属性")));
	pGroup3->AddSubItem(m_callVarI3			= AddPropInt(Language(L"整数3"), Language(L"整数_说明", L"运行到此时设置的整数，和整数值不是一个相同属性")));
	pGroup3->AddSubItem(m_callVarI4			= AddPropInt(Language(L"整数4"), Language(L"整数_说明", L"运行到此时设置的整数，和整数值不是一个相同属性")));
	//
	CMFCPropertyGridProperty* pGroup4			= new CMFCPropertyGridProperty(Language(L"从其他变量赋值"));
	pGroup4->AddSubItem(m_callVarOtherIDs		= AddPropOption(Language(L"变量名称[来源]"), Language(L"变量名称_说明", L"设置指定动作的，指定变量的值，所选项目的变量名称"), g_typeDefault));
	//
	m_callVarWnd.AddProperty(pGroup1);
	m_callVarWnd.AddProperty(pGroup2);
	m_callVarWnd.AddProperty(pGroup3);
	m_callVarWnd.AddProperty(pGroup4);
}

VOID		GCGameProject::InitCallProperties()
{
	CMFCPropertyGridProperty* pGroup1		= new CMFCPropertyGridProperty(Language(L"调用属性"));
	pGroup1->AddSubItem(m_callPropActions	= AddPropOption(Language(L"动作名称"), Language(L"动作名称_说明", L"所选项目的动作名称"), g_typeDefault));
	pGroup1->AddSubItem(m_callPropIDs		= AddPropOption(Language(L"属性名称"), Language(L"变量名称_说明", L"设置指定动作的，指定变量的值，所选项目的变量名称"), g_typeDefault));
	m_callPropWnd.AddProperty(pGroup1);
	//
	for(int i=0;i<MAX_EVENT_PARAM_COUNT;i++)
	{
		wchar_t szParam[128];
		swprintf(szParam, L"%s%d", Language(L"参数"), i + 1);
		m_callPropParamGroup[i]		= new CMFCPropertyGridProperty(szParam);
		m_callPropParamGroup[i]->AddSubItem(m_callPropParamInt[i]			= AddPropInt(Language(L"整数值"), Language(L"整数值_说明", L"运行到此时设置的整数值")));
		m_callPropParamGroup[i]->AddSubItem(m_callPropParamDouble[i]		= AddPropFloat(Language(L"小数值"), Language(L"小数值_说明", L"运行到此时设置的小数值")));
		m_callPropParamGroup[i]->AddSubItem(m_callPropParamText[i]			= AddPropText(Language(L"文本值"), Language(L"文本值_说明", L"运行到此时设置的文本值"), true, 10000000));
		m_callPropParamGroup[i]->AddSubItem(m_callPropParamColor[i]			= AddPropColor(Language(L"颜色"), Language(L"参数颜色_说明", L"运行到此时设置的参数颜色")));
		m_callPropParamGroup[i]->AddSubItem(m_callPropParamList[i]			= AddPropOption(Language(L"列表中选择"), Language(L"列表中选择_说明", L"运行到此时设置的列表中选择"), g_typeDefault));
		m_callPropParamGroup[i]->AddSubItem(m_callPropParamOtherIDs[i]		= AddPropOption(Language(L"变量名称[来源]"), Language(L"变量名称_说明", L"设置指定动作的，指定变量的值，所选项目的变量名称"), g_typeDefault));
		m_callPropWnd.AddProperty(m_callPropParamGroup[i]);
	}
}

VOID		GCGameProject::InitVariants()
{
	CMFCPropertyGridProperty* pGroup1		= new CMFCPropertyGridProperty(Language(L"变量定义值"));
	pGroup1->AddSubItem(m_variantName		= AddPropText(Language(L"变量名称"), Language(L"变量名称_说明", L"设置变量名称"), false, NameW::Length));
	pGroup1->AddSubItem(m_variantInt		= AddPropInt(Language(L"整数值"), Language(L"整数值_说明", L"运行到此时设置的整数值")));
	pGroup1->AddSubItem(m_variantDouble		= AddPropFloat(Language(L"小数值"), Language(L"小数值_说明", L"运行到此时设置的小数值")));
	pGroup1->AddSubItem(m_variantText		= AddPropText(Language(L"文本值"), Language(L"文本值_说明", L"运行到此时设置的文本值"), true, 10000000));
	CMFCPropertyGridProperty* pGroup2		= new CMFCPropertyGridProperty(Language(L"其他赋值设置"));
	pGroup2->AddSubItem(m_variantF1			= AddPropFloat(Language(L"小数1"), Language(L"小数_说明", L"运行到此时设置的小数，和小数值不是一个相同属性")));
	pGroup2->AddSubItem(m_variantF2			= AddPropFloat(Language(L"小数2"), Language(L"小数_说明", L"运行到此时设置的小数，和小数值不是一个相同属性")));
	pGroup2->AddSubItem(m_variantF3			= AddPropFloat(Language(L"小数3"), Language(L"小数_说明", L"运行到此时设置的小数，和小数值不是一个相同属性")));
	pGroup2->AddSubItem(m_variantF4			= AddPropFloat(Language(L"小数4"), Language(L"小数_说明", L"运行到此时设置的小数，和小数值不是一个相同属性")));
	pGroup2->AddSubItem(m_variantI1			= AddPropInt(Language(L"整数1"), Language(L"整数_说明", L"运行到此时设置的整数，和整数值不是一个相同属性")));
	pGroup2->AddSubItem(m_variantI2			= AddPropInt(Language(L"整数2"), Language(L"整数_说明", L"运行到此时设置的整数，和整数值不是一个相同属性")));
	pGroup2->AddSubItem(m_variantI3			= AddPropInt(Language(L"整数3"), Language(L"整数_说明", L"运行到此时设置的整数，和整数值不是一个相同属性")));
	pGroup2->AddSubItem(m_variantI4			= AddPropInt(Language(L"整数4"), Language(L"整数_说明", L"运行到此时设置的整数，和整数值不是一个相同属性")));
	//
	m_variantProp.AddProperty(pGroup1);
	m_variantProp.AddProperty(pGroup2);
}

VOID		GCGameProject::SetVariantOption(PropertyPtr propVariant, CPlotState* c, int variantID)
{
	propVariant->RemoveAllOptions();
	propVariant->AddOption(Language(L""));
	CDynamicArray<CPlotVariant*> vars;
	wchar_t buf[256];
	if(c)
		c->GetActionPtr()->GetAllPlotVariants(vars);
	for(int i=0;i<vars.size();i++)
	{
		swprintf(buf, L"%s[%d]", vars[i]->GetName(), vars[i]->GetID());
		propVariant->AddOption(buf);
	}
	CPlotVariant* varPtr	=	this->m_gameDirector.SearchVariant(variantID);//c->GetProjectPtr()->SearchVariant(variantID);
	if(varPtr)
	{
		swprintf(buf, L"%s[%d]", varPtr->GetName(), varPtr->GetID());
		SetPropText(propVariant, buf);//varPtr->m_name.strW());
	}
	else
		SetPropText(propVariant, L"");
}

VOID						GCGameProject::SetSelectNone()
{
	m_selectVar		=	0;
	m_selectState	=	0;
	m_selectVarSet	=	0;
	m_selectPropSet	=	0;
	m_selectScript	=	0;
	m_selectAction	=	0;
}

VOID						GCGameProject::SetPropertyNull()
{
	SetSelectNone();
	SetCurrentProperties(NULL, NULL, NULL);;
}
VOID						GCGameProject::SetPropertyGame()
{
}

VOID						GCGameProject::SetPropertyAction(CPlotAction* act)
{
	SetSelectNone();
	if(act->GetPlotType() == PlotType_Chapter)
		SetCurrentProperties(NULL, Language(L"章节属性"), &m_dlgVariant);
	else if(act->GetPlotType() == PlotType_Drama)
		SetCurrentProperties(NULL, Language(L"篇章属性"), &m_dlgVariant);
	else// if(act->GetPlotType() == PlotType_Action)
		SetCurrentProperties(NULL, Language(L"动作属性"), &m_dlgVariant);
	m_dlgVariant.SetActionPtr(act);
}

VOID		GCGameProject::SetPropertyCallVarSet(CPlotState* c, CCallVariantSet* var)
{
	SetSelectNone();
	if(!c)
	{
		GetPropertiesWindow()->SetCurrentProperties(0, Language(L"无属性"));
		return;
	}
	GetPropertiesWindow()->SetCurrentProperties(&this->m_callVarWnd, Language(L"变量值设置"));
	//
	m_callVarWnd.EnableRender(false);
	this->m_selectState		=	c;
	this->m_selectVarSet	=	var;
	SetVariantOption(m_callVarIDs, c, var->m_variantSetID);
	SetVariantOption(m_callVarOtherIDs, c, var->m_value.otherVariantID);
	//
	SetPropInt(m_callVarInt, var->m_value.value.m_nValue);
	SetPropFloat(m_callVarDouble, var->m_value.value.m_dValue);
	SetPropText(m_callVarText, var->m_value.value.m_text);
	SetPropFloat(m_callVarF1, var->m_value.value.m_f4.x);
	SetPropFloat(m_callVarF2, var->m_value.value.m_f4.y);
	SetPropFloat(m_callVarF3, var->m_value.value.m_f4.z);
	SetPropFloat(m_callVarF4, var->m_value.value.m_f4.w);
	SetPropInt(m_callVarI1, var->m_value.value.m_i4.left);
	SetPropInt(m_callVarI2, var->m_value.value.m_i4.top);
	SetPropInt(m_callVarI3, var->m_value.value.m_i4.right);
	SetPropInt(m_callVarI4, var->m_value.value.m_i4.bottom);
	m_callVarWnd.EnableRender(true);
}

VOID		GCGameProject::SetPropertyOption(CCallPropertySet* propSet)
{
	m_callPropWnd.EnableRender(false);
	CPlotPropertiesMap* map	=	m_gameDirector.SearchPropertiesMap(propSet->m_propertiesMapID);
	CPlotProperties* prop	=	map ? map->GetPropertyPtrByID(propSet->m_propertyCall) : 0;
	for(int p=0;p<MAX_EVENT_PARAM_COUNT;p++)
	{
		if(p < propSet->m_paramCount && prop)
		{
			PlotValueList* list	=	map->GetPropParameter(propSet->m_propertyCall, p);
			m_callPropParamGroup[p]->Show(true);
			m_callPropParamGroup[p]->SetName(list->varName);
			//
			wchar_t buf[128];
			swprintf(buf, L"%s[int]", list->varName.operator LPCWSTR());
			m_callPropParamInt[p]->SetName(buf);
			swprintf(buf, L"%s[double]", list->varName.operator LPCWSTR());
			m_callPropParamDouble[p]->SetName(buf);
			swprintf(buf, L"%s[color]", list->varName.operator LPCWSTR());
			m_callPropParamColor[p]->SetName(buf);
			swprintf(buf, L"%s[text]", list->varName.operator LPCWSTR());
			m_callPropParamText[p]->SetName(buf);
			swprintf(buf, L"%s[list]", list->varName.operator LPCWSTR());
			m_callPropParamList[p]->SetName(buf);
			//
			swprintf(buf, L"%s[src var]", list->varName.operator LPCWSTR());
			m_callPropParamOtherIDs[p]->SetName(buf);
			//
			m_callPropParamInt[p]->Show(false);
			m_callPropParamColor[p]->Show(false);
			m_callPropParamDouble[p]->Show(false);
			m_callPropParamText[p]->Show(false);
			m_callPropParamList[p]->Show(false);
			//
			m_callPropParamOtherIDs[p]->Show(false);
			//
			switch(list->variantType)
			{
			case VariantType_Double:
				{
					m_callPropParamDouble[p]->SetMinMax(list->dMin, list->dMax);
					SetPropFloat(m_callPropParamDouble[p], propSet->m_params[p].value.getDouble());
					m_callPropParamDouble[p]->Show(true);
				}
				break;
			case VariantType_Text:
				{
					SetPropText(m_callPropParamText[p], propSet->m_params[p].value.getText());
					m_callPropParamText[p]->Show(true);
				}
				break;
			case VariantType_Int:
				{
					int nValue	=	propSet->m_params[p].value.getInt();
					std::vector<std::wstring> lists;
					lists.push_back(L"");
					wchar_t buf[128];
					int sel		=	0;
					switch(list->pvlType)
					{
					case PlotValueList::PVL_None:
						{
							if(list->values.size() > 0)
							{
								for(int j=0;j<list->values.size();j++)
								{
									swprintf(buf, L"%s[%d]", (const wchar_t*)list->values[j].name, list->values[j].value);
									if(list->values[j].value == nValue)
										sel	=	lists.size();
									lists.push_back(buf);
								}
							}
							else
							{
								SetPropInt(m_callPropParamInt[p], nValue);
								m_callPropParamInt[p]->Show(true);
								lists.clear();
							}
						}
						break;
					case PlotValueList::PVL_DramaList:
						{
							for(int j=0;j<map->GetChapterPtr()->GetDramaCount();j++)
							{
								swprintf(buf, L"%s[%d]", GetPlotName(map->GetChapterPtr()->GetDramaPtr(j)), map->GetChapterPtr()->GetDramaPtr(j)->GetID());
								if(map->GetChapterPtr()->GetDramaPtr(j)->GetID() == nValue)
									sel	=	lists.size();
								lists.push_back(buf);
							}
						}
						break;
					case PlotValueList::PVL_ChapterList:
						{
							for(int j=0;j<map->GetProjectPtr()->GetChapterCount();j++)
							{
								swprintf(buf, L"%s[%d]", GetPlotName(map->GetProjectPtr()->GetChapterPtr(j)), map->GetProjectPtr()->GetChapterPtr(j)->GetID());
								if(map->GetProjectPtr()->GetChapterPtr(j)->GetID() == nValue)
									sel	=	lists.size();
								lists.push_back(buf);
							}
						}
						break;
					case PlotValueList::PVL_ActionList:
						{
							CDynamicArray<CPlotAction*> rets;
							map->GetProcessPtr()->GetAllPlotActions(rets);
							for(int j=0;j<rets.size();j++)
							{
								swprintf(buf, L"%s[%d]", GetPlotName(rets[j]), rets[j]->GetID());
								if(rets[j]->GetID() == nValue)
									sel	=	lists.size();
								lists.push_back(buf);
							}
						}
						break;
					case PlotValueList::PVL_SelectUIControl:
						{
							if(map->GetPlotType() == PlotType_ElementUI)
							{
								CPlotElementUI* ui	=	static_cast<CPlotElementUI*>(map);
								if(ui->m_nDialogID == 0)
									ui->RefreshParameters();
								int value	=	0;
								for(int j=0;j<ui->m_controls.size();j++)
								{
									wchar_t buf[128];
									swprintf(buf, L"%s[%d]", ui->m_controls[j].name.str(), ui->m_controls[j].value);
									if(ui->m_controls[j].value == nValue)
										sel	=	lists.size();
									lists.push_back(buf);
								}
							}
						}
						break;
					case PlotValueList::PVL_VariantList:
						{
							CDynamicArray<CPlotVariant*> rets;
							map->GetProcessPtr()->GetAllPlotVariants(rets);
							for(int j=0;j<rets.size();j++)
							{
								swprintf(buf, L"%s[%d]", rets[j]->GetName(), rets[j]->GetID());
								if(rets[j]->GetID() == nValue)
									sel	=	lists.size();
								lists.push_back(buf);
							}
						}
						break;
					case PlotValueList::PVL_CallList:
						{
							CDynamicArray<CPlotState*> vars;
							CPlotAction* action	=	map->GetActionPtr();
							if(action)
							{
								int value	=	0;
								for(int j=0;j<action->GetStateCount();j++)
								{
									wchar_t buf[128];
									swprintf(buf, L"%s[%d]", action->GetStatePtr(j)->GetName(), action->GetStatePtr(j)->GetID());
									if(action->GetStatePtr(j)->GetID() == nValue)
										sel	=	lists.size();
									lists.push_back(buf);
								}
							}
						}
						break;
					case PlotValueList::PVL_Color:
						{
							SetPropColor(m_callPropParamColor[p], color4(propSet->m_params[p].value.getInt()).getRGB());
							m_callPropParamColor[p]->Show(true);
							lists.clear();
						}
						break;
					}
					m_callPropParamList[p]->RemoveAllOptions();
					if(lists.size() > 0)
					{
						m_callPropParamList[p]->Show(true);
						for(int k=0;k<lists.size();k++)
						{
							m_callPropParamList[p]->AddOption(lists[k].c_str());
						}
						SetPropText(m_callPropParamList[p], lists[sel].c_str());
						continue;
					}
				}
				break;
			}
			//
			m_callPropParamOtherIDs[p]->Show(true);
			SetVariantOption(m_callPropParamOtherIDs[p], m_selectState, propSet->m_params[p].otherVariantID);
		}
		else
		{
			m_callPropParamGroup[p]->Show(false);
		}
	}
	m_callPropWnd.EnableRender(true);
}

VOID		GCGameProject::SetPropertyCallPropCall(CPlotState* c, CCallPropertySet* var)
{
	SetSelectNone();
	if(!c)
	{
		GetPropertiesWindow()->SetCurrentProperties(0, Language(L"无属性"));
		return;
	}
	GetPropertiesWindow()->SetCurrentProperties(&this->m_callPropWnd, Language(L"调用设置"));
	//
	m_callPropWnd.EnableRender(false);
	this->m_selectState		=	c;
	this->m_selectPropSet	=	var;
	//
	m_callPropActions->RemoveAllOptions();
	//
	CDynamicArray<CPlotPropertiesMap*>	optionActions;
	m_selectState->GetActionPtr()->GetAllPlotPropertiesMap(optionActions);
	//
	m_callPropActions->AddOption(Language(L""));
	wchar_t buf[128];
	for(int i=0;i<optionActions.size();i++)
	{
		swprintf(buf, L"%s[%d]", GetPlotName(optionActions[i]), optionActions[i]->GetID());
		m_callPropActions->AddOption(buf);
	}
	//
	CPlotPropertiesMap* action	=	m_selectState->GetProjectPtr()->SearchPropertiesMap(var->m_propertiesMapID);
	m_callPropIDs->RemoveAllOptions();
	m_callPropIDs->AddOption(Language(L""));
	if(action)
	{
		swprintf(buf, L"%s[%d]", GetPlotName(action), action->GetID());
		SetPropText(m_callPropActions, buf);
		for(int i=0;i<action->GetPropertyCount();i++)
		{
			swprintf(buf, L"%s[%d]", action->GetPropertyPtr(i)->GetName(), action->GetPropertyPtr(i)->GetID());
			m_callPropIDs->AddOption(buf);
		}
	}
	else
		SetPropText(m_callPropActions, L"");
	CPlotProperties* varPtr	=	action ? action->GetPropertyPtrByID(var->m_propertyCall) : 0;
	if(varPtr)
	{
		swprintf(buf, L"%s[%d]", varPtr->GetName(), varPtr->GetID());
		SetPropText(m_callPropIDs, buf);//varPtr->m_name.strW());
	}
	else
		SetPropText(m_callPropIDs, L"");
	SetPropertyOption(this->m_selectPropSet);
	//
	m_callPropWnd.EnableRender(true);
}

VOID		GCGameProject::SetPropertyVariant(CPlotAction* act, CPlotVariant* var)
{
	SetSelectNone();
	if(!var)
	{
		GetPropertiesWindow()->SetCurrentProperties(0, Language(L"无属性"));
		return;
	}
	this->m_selectAction	=	act;
	this->m_selectVar		=	var;
	GetPropertiesWindow()->SetCurrentProperties(&this->m_variantProp, Language(L"变量设置"));
	//
	m_variantProp.EnableRender(false);
	SetPropText(m_variantName, var->GetName());
	SetPropInt(m_variantInt, var->m_value.m_nValue);
	SetPropFloat(m_variantDouble, var->m_value.m_dValue);
	SetPropText(m_variantText, var->m_value.m_text);
	SetPropFloat(m_variantF1, var->m_value.m_f4.x);
	SetPropFloat(m_variantF2, var->m_value.m_f4.y);
	SetPropFloat(m_variantF3, var->m_value.m_f4.z);
	SetPropFloat(m_variantF4, var->m_value.m_f4.w);
	SetPropInt(m_variantI1, var->m_value.m_i4.left);
	SetPropInt(m_variantI2, var->m_value.m_i4.top);
	SetPropInt(m_variantI3, var->m_value.m_i4.right);
	SetPropInt(m_variantI4, var->m_value.m_i4.bottom);
	m_variantProp.EnableRender(true);
}

VOID						GCGameProject::SetPropertyCall(CPlotState* c)
{
	SetSelectNone();
	SetCurrentPropertiesW(0, Language(L"调用属性"), &m_callPlot);
	m_callPlot.SetPlotCall(c);
}

VOID		GCGameProject::SetPropertyCallLuaScript(CPlotState* c, CCallLuaScript* var)
{
	SetSelectNone();
	if(!c)
	{
		GetPropertiesWindow()->SetCurrentProperties(0, Language(L"无属性"));
		return;
	}
	this->m_selectState		=	c;
	this->m_selectScript	=	var;
	//GetPropertiesWindow()->SetCurrentWindow(&this->m_luaScript, Language(L"脚本代码"));
	//m_luaScript.SetScriptPtr(var);
}
