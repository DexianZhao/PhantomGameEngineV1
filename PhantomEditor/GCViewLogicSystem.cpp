#include "StdAfx.h"
#include "GCViewLogicSystem.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"

struct	TableCopyData
{
	TreeItemType			type;
	GCViewLogicSystem*		editor;
	TableCopyData()
	{
		editor	=	0;
	}
};

std::vector<TableCopyData>		g_copyTable3Ds;

VOID		SetTableLineCopy(GCViewLogicSystem* e, std::vector<TreeItemType*>* vs)
{
	g_copyTable3Ds.clear();
	for(int i=0;i<vs->size();i++)
	{
		TableCopyData ud;
		ud.editor	=	e;
		ud.type		=	*vs->at(i);
		g_copyTable3Ds.push_back(ud);
	}
}

BOOL		IsCanPasteTable(int nType)
{
	for(int i=0;i<g_copyTable3Ds.size();i++)
	{
		if(g_copyTable3Ds[i].type.type == nType)
		{
			return true;
		}
	}
	return false;
}

GCViewLogicSystem::GCViewLogicSystem(CPhantomGCDoc* docPtr)
	:GCViewBase(docPtr, Language(L"数据编辑"), IDB_GAME_VIEWER)
{
	this->m_logicPtr	=	0;
}

GCViewLogicSystem::~GCViewLogicSystem(void)
{
	CloseFile();
}

HICON		GCViewLogicSystem::GetViewerIcon()
{
	static HICON	hIcon	=	AfxGetApp()->LoadIcon(IDI_LOGIC_FILE_ICON);
	static HICON	hIconR	=	AfxGetApp()->LoadIcon(IDI_Z_LOGIC_FILE_ICON);
	if(this->m_bIsReadOnly)
		return hIconR;
	return hIcon;
}

void		GCViewLogicSystem::OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp)
{
	__super::OnPropertyChanged(wnd, pProp);
	TableProperties* sel	=	0;
	for(int i=0;i<m_tableProperties.size();i++)
	{
		if(wnd == &m_tableProperties[i]->base)
		{
			sel	=	m_tableProperties[i];
			break;
		}
	}
	if(!sel)
		return;
	for(int s=0;s<sel->columns.size();s++)
	{
		TablePropertyPtr& col	=	sel->columns[s];
		if(col.ptr == pProp)
		{
			for(int i=0;i<this->m_treeView.GetSelectCount();i++)
			{
				HTREEITEM hItem		=	m_treeView.GetSelect(i);
				TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
				if(type)
				{
					if(type->type == LOGIC_LINE)
					{
						ILogicMemoryTable* table	=	this->m_logicPtr->GetTablePtrByID(type->p1);
						ILogicMemoryLine* line		=	table->GetLinePtrByID(type->p2);
						//
						ILogicColumnName* column	=	table->GetColumnNamePtr(col.columnIndex);
						this->GetPropertyValue(&col, sel, line, column, col.columnIndex, col.dataTypeIndex, column->GetDataTypeCount(), col.arrayIndex, table->GetArraySize(col.columnIndex));
						SetModifyed(true);
						if(table->GetNameColumnIndex() == col.columnIndex)
							m_treeView.SetItemText(hItem, W(line->GetText(col.columnIndex)));
					}
				}
			}
			break;
		}
	}
}

VOID		GCViewLogicSystem::OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem)	//更新选择树形控件
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
		case LOGIC_SYSTEM:
			{
				SetCurrentProperties(NULL, NULL, NULL);
			}
			break;
		case LOGIC_LINE:
			{
				ILogicMemoryTable* table	=	this->m_logicPtr->GetTablePtrByID(type->p1);
				TableProperties* t	=	FindProperties(table);
				SetCurrentProperties(&t->base, W(table->GetTableViewName()).buf, NULL);
				//
				t->base.EnableRender(false);
				t->base.EnableWindow(true);
				ILogicMemoryLine* line	=	table->GetLinePtrByID(type->p2);
				if(!line)
				{
					t->base.EnableWindow(false);
					t->base.EnableRender(true);
					break;
				}
				//
				for(int i=0;i<table->GetColumnCount();i++)
				{
					ILogicColumnName* name	=	table->GetColumnNamePtr(i);
					int	arrayCount	=	table->GetArraySize(i);
					for(int a=0;a<arrayCount;a++)
					{
						int dataCount			=	name->GetDataTypeCount();
						if(dataCount > 1)
						{
							for(int d=0;d<dataCount;d++)
								SetPropertyValue(t, line, name, i, d, dataCount, a, arrayCount);
						}
						else
							SetPropertyValue(t, line, name, i, 0, 1, a, arrayCount);
					}
				}
				t->base.EnableRender(true);
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

GCViewLogicSystem::TableProperties*		GCViewLogicSystem::FindProperties(ILogicMemoryTable* t)
{
	for(int i=0;i<m_tableProperties.size();i++)
	{
		if(m_tableProperties[i]->table == t)
			return m_tableProperties[i];
	}
	return 0;
}

VOID		GCViewLogicSystem::GetPropertyValue(TablePropertyPtr* tpp, TableProperties* tp, ILogicMemoryLine* line, ILogicColumnName* column, int columnIndex, int d, int dataCount, int arrayIndex, int arrayCount)
{
	PropertyPtr ptr = tpp->ptr;
	if(!ptr)
		return;
	switch(column->GetDataType(d))
	{
	case ColumnType_Content:
		{
			std::string str;
			GetPropText(ptr, str);
			line->SetText(columnIndex, str.c_str(), d, arrayIndex);
		}
		break;
	case ColumnType_File:
		{
			std::wstring str;
			GetPropFile(static_cast<PropertyFilePtr>(ptr), str);
			line->SetText(columnIndex, A(str.c_str()), d, arrayIndex);
		}
		break;
	case ColumnType_STRING:
		{
			std::string str;
			GetPropText(ptr, str);
			line->SetText(columnIndex, str.c_str(), d, arrayIndex);
		}
		break;
	case ColumnType_FLOAT2:
		{
			float2* f	=	line->GetFloat2Ptr(columnIndex, d, arrayIndex);
			if(tpp->subIndex == 0)
				f->x	=	GetPropFloat(ptr);
			else if(tpp->subIndex == 1)
				f->y	=	GetPropFloat(ptr);
		}
		break;
	case ColumnType_FLOAT3:
		{
			float3* f	=	line->GetFloat3Ptr(columnIndex, d, arrayIndex);
			if(tpp->subIndex == 0)
				f->x	=	GetPropFloat(ptr);
			else if(tpp->subIndex == 1)
				f->y	=	GetPropFloat(ptr);
			else if(tpp->subIndex == 2)
				f->z	=	GetPropFloat(ptr);
		}
		break;
	case ColumnType_FLOAT4:
		{
			float4* f	=	line->GetFloat4Ptr(columnIndex, d, arrayIndex);
			if(tpp->subIndex == 0)
				f->x	=	GetPropFloat(ptr);
			else if(tpp->subIndex == 1)
				f->y	=	GetPropFloat(ptr);
			else if(tpp->subIndex == 2)
				f->z	=	GetPropFloat(ptr);
			else if(tpp->subIndex == 3)
				f->w	=	GetPropFloat(ptr);
		}
		break;
	case ColumnType_INT2:
		{
			PPixel* f	=	line->GetPointPtr(columnIndex, d, arrayIndex);
			if(tpp->subIndex == 0)
				f->x	=	GetPropInt(ptr);
			else if(tpp->subIndex == 1)
				f->y	=	GetPropInt(ptr);
		}
		break;
	case ColumnType_INT4:
		{
			PRect* f	=	line->GetRectPtr(columnIndex, d, arrayIndex);
			if(tpp->subIndex == 0)
				f->left	=	GetPropInt(ptr);
			else if(tpp->subIndex == 1)
				f->top	=	GetPropInt(ptr);
			else if(tpp->subIndex == 2)
				f->right	=	GetPropInt(ptr);
			else if(tpp->subIndex == 3)
				f->bottom	=	GetPropInt(ptr);
		}
		break;
	case ColumnType_INT:
		line->SetInt(GetPropInt(ptr), columnIndex, d, arrayIndex);
		break;
	case ColumnType_FLOAT:
		line->SetFloat(GetPropFloat(ptr), columnIndex, d, arrayIndex);
		break;
	case ColumnType_DOUBLE:
		line->SetDouble(GetPropFloat(ptr), columnIndex, d, arrayIndex);
		break;
	case ColumnType_OtherTable:
		{
			ILogicMemoryTable* other	=	column->GetOtherTablePtr();
			if(other)
			{
				std::wstring str;
				GetPropText(ptr, str);
				bool bFind	=	false;
				for(int i=0;i<other->GetLineCount();i++)
				{
					ILogicMemoryLine* l	=	other->GetLinePtrByIndex(i);
					wchar_t buf[512];
					swprintf(buf, L"%s[%d]", W(l->GetLineName()).buf, l->GetLineID());
					if(wcsicmp(buf, str.c_str()) == 0)
					{
						bFind	=	true;
						line->SetOtherTableLine(columnIndex, l, d, arrayIndex);
						break;
					}
				}
				if(!bFind)
					line->SetOtherTableLine(columnIndex, 0, d, arrayIndex);
			}
		}
		break;
	case ColumnType_Boolean:
		line->SetInt(GetPropBool(ptr), columnIndex, d, arrayIndex);
		break;
	case ColumnType_Color:
		line->SetInt(GetPropColor(static_cast<PropertyColorPtr>(ptr)), columnIndex, d, arrayIndex);
		break;
	}
}

VOID		GCViewLogicSystem::SetPropertyValue(TableProperties* tp, ILogicMemoryLine* line, ILogicColumnName* column, int columnIndex, int d, int dataCount, int arrayIndex, int arrayCount)
{
	PropertyPtr ptr = tp->FindColumn(columnIndex, arrayIndex, d, 0);
	if(!ptr)
		return;
	SetPropInt(tp->idPtr, line->GetLineID());
	switch(column->GetDataType(d))
	{
	case ColumnType_Content:
		SetPropText(ptr, line->GetText(columnIndex, d, arrayIndex));
		break;
	case ColumnType_File:
		SetPropFile(static_cast<PropertyFilePtr>(ptr), line->GetText(columnIndex, d, arrayIndex));
		break;
	case ColumnType_STRING:
		SetPropText(ptr, line->GetText(columnIndex, d, arrayIndex));
		break;
	case ColumnType_FLOAT2:
		{
			float2* f	=	line->GetFloat2Ptr(columnIndex, d, arrayIndex);
			SetPropFloat(ptr, f->x);
			ptr = tp->FindColumn(columnIndex, arrayIndex, d, 1);
			SetPropFloat(ptr, f->y);
		}
		break;
	case ColumnType_FLOAT3:
		{
			float3* f	=	line->GetFloat3Ptr(columnIndex, d, arrayIndex);
			SetPropFloat(ptr, f->x);
			ptr = tp->FindColumn(columnIndex, arrayIndex, d, 1);
			SetPropFloat(ptr, f->y);
			ptr = tp->FindColumn(columnIndex, arrayIndex, d, 2);
			SetPropFloat(ptr, f->z);
		}
		break;
	case ColumnType_FLOAT4:
		{
			float4* f	=	line->GetFloat4Ptr(columnIndex, d, arrayIndex);
			SetPropFloat(ptr, f->x);
			ptr = tp->FindColumn(columnIndex, arrayIndex, d, 1);
			SetPropFloat(ptr, f->y);
			ptr = tp->FindColumn(columnIndex, arrayIndex, d, 2);
			SetPropFloat(ptr, f->z);
			ptr = tp->FindColumn(columnIndex, arrayIndex, d, 3);
			SetPropFloat(ptr, f->w);
		}
		break;
	case ColumnType_INT2:
		{
			PPixel* f	=	line->GetPointPtr(columnIndex, d, arrayIndex);
			SetPropInt(ptr, f->x);
			ptr = tp->FindColumn(columnIndex, arrayIndex, d, 1);
			SetPropInt(ptr, f->y);
		}
		break;
	case ColumnType_INT4:
		{
			PRect* f	=	line->GetRectPtr(columnIndex, d, arrayIndex);
			SetPropInt(ptr, f->left);
			ptr = tp->FindColumn(columnIndex, arrayIndex, d, 1);
			SetPropInt(ptr, f->top);
			ptr = tp->FindColumn(columnIndex, arrayIndex, d, 2);
			SetPropInt(ptr, f->right);
			ptr = tp->FindColumn(columnIndex, arrayIndex, d, 3);
			SetPropInt(ptr, f->bottom);
		}
		break;
	case ColumnType_INT:
		SetPropInt(ptr, line->GetInt(columnIndex, d, arrayIndex));
		break;
	case ColumnType_FLOAT:
		SetPropFloat(ptr, line->GetFloat(columnIndex, d, arrayIndex));
		break;
	case ColumnType_DOUBLE:
		SetPropFloat(ptr, line->GetDouble(columnIndex, d, arrayIndex));
		break;
	case ColumnType_OtherTable:
		{
			ILogicMemoryTable* other	=	column->GetOtherTablePtr();
			if(other)
			{
				ptr->RemoveAllOptions();
				ptr->AddOption(L"");
				NameW select;
				ILogicMemoryLine* sel	=	line->GetOtherTableLine(columnIndex, d, arrayIndex);
				for(int i=0;i<other->GetLineCount();i++)
				{
					ILogicMemoryLine* l	=	other->GetLinePtrByIndex(i);
					wchar_t buf[512];
					swprintf(buf, L"%s[%d]", W(l->GetLineName()).buf, l->GetLineID());
					if(l == sel)
						select	=	buf;
					ptr->AddOption(buf);
				}
				SetPropText(ptr, select);
			}
			else
				SetPropText(ptr, L"");
		}
		break;
	case ColumnType_Boolean:
		SetPropBool(ptr, line->GetInt(columnIndex, d, arrayIndex));
		break;
	case ColumnType_Color:
		SetPropColor(static_cast<PropertyColorPtr>(ptr), line->GetInt(columnIndex, d, arrayIndex));
		break;
	}
}

VOID		GCViewLogicSystem::AddDataType(TableProperties* tp, ILogicMemoryTable* table, CMFCPropertyGridProperty* groupTemp, ILogicColumnName* column, int columnIndex, int d, int dataCount, int arrayIndex, int arrayCount)
{
	wchar_t szName[256];
	int cold	=	d;
	if(dataCount == 1)
		cold	=	-1;
	if(arrayCount > 1)
		swprintf(szName, L"%s[%d],%s[%d]", W(column->GetColumnName(cold)).buf, arrayIndex, W(column->GetColumnViewName(d)).buf, arrayIndex);
	else
		swprintf(szName, L"%s,%s", W(column->GetColumnName(cold)).buf, W(column->GetColumnViewName(d)).buf);
	switch(column->GetDataType(d))
	{
	case ColumnType_Content:
		groupTemp->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, AddPropText(szName, W(column->GetColumnDescription()), true)));
		break;
	case ColumnType_File:
		{
			wchar_t wtext[512];
			wtext[0]	=	0;
			for(int f=0;f<column->GetFileTypeCount();f++)
			{
				wcscat(wtext, W(column->GetFileType(f)));
				wcscat(wtext, L"|");
				wcscat(wtext, L"*");
				wcscat(wtext, W(column->GetFileType(f)));
				wcscat(wtext, L"|");
			}
			wcscat(wtext, L"|");
			groupTemp->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, AddPropFile(szName, W(column->GetColumnDescription()), wtext, false)));
		}
		break;
	case ColumnType_STRING:
		groupTemp->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, AddPropText(szName, W(column->GetColumnDescription()), false)));
		break;
	case ColumnType_FLOAT2:
		{
			CMFCPropertyGridProperty* gt = new CMFCPropertyGridProperty(szName);
			groupTemp->AddSubItem(gt);
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 0, AddPropFloat(L"x", W(column->GetColumnDescription()))));
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 1, AddPropFloat(L"y", W(column->GetColumnDescription()))));
		}
		break;
	case ColumnType_FLOAT3:
		{
			CMFCPropertyGridProperty* gt = new CMFCPropertyGridProperty(szName);
			groupTemp->AddSubItem(gt);
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 0, AddPropFloat(L"x", W(column->GetColumnDescription()))));
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 1, AddPropFloat(L"y", W(column->GetColumnDescription()))));
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 2, AddPropFloat(L"z", W(column->GetColumnDescription()))));
		}
		break;
	case ColumnType_FLOAT4:
		{
			CMFCPropertyGridProperty* gt = new CMFCPropertyGridProperty(szName);
			groupTemp->AddSubItem(gt);
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 0, AddPropFloat(L"x", W(column->GetColumnDescription()))));
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 1, AddPropFloat(L"y", W(column->GetColumnDescription()))));
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 2, AddPropFloat(L"z", W(column->GetColumnDescription()))));
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 3, AddPropFloat(L"w", W(column->GetColumnDescription()))));
		}
		break;
	case ColumnType_INT2:
		{
			CMFCPropertyGridProperty* gt = new CMFCPropertyGridProperty(szName);
			groupTemp->AddSubItem(gt);
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 0, AddPropInt(L"x", W(column->GetColumnDescription()))));
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 1, AddPropInt(L"y", W(column->GetColumnDescription()))));
		}
		break;
	case ColumnType_INT4:
		{
			CMFCPropertyGridProperty* gt = new CMFCPropertyGridProperty(szName);
			groupTemp->AddSubItem(gt);
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 0, AddPropInt(L"left", W(column->GetColumnDescription()))));
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 1, AddPropInt(L"top", W(column->GetColumnDescription()))));
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 2, AddPropInt(L"right", W(column->GetColumnDescription()))));
			gt->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, 3, AddPropInt(L"bottom", W(column->GetColumnDescription()))));
		}
		break;
	case ColumnType_INT:
		groupTemp->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, AddPropInt(szName, W(column->GetColumnDescription()))));
		break;
	case ColumnType_FLOAT:
		groupTemp->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, AddPropFloat(szName, W(column->GetColumnDescription()))));
		break;
	case ColumnType_DOUBLE:
		groupTemp->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, AddPropFloat(szName, W(column->GetColumnDescription()))));
		break;
	case ColumnType_OtherTable:
		groupTemp->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, AddPropOption(szName, W(column->GetColumnDescription()), g_typeDefault)));
		break;
	case ColumnType_Boolean:
		groupTemp->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, AddPropBool(szName, W(column->GetColumnDescription()))));
		break;
	case ColumnType_Color:
		groupTemp->AddSubItem(tp->AddColumn(columnIndex, arrayIndex, d, AddPropColor(szName, W(column->GetColumnDescription()))));
		break;
	}
}

VOID		GCViewLogicSystem::AddTableProperties(ILogicMemoryTable* table)
{
	TableProperties* t	=	FindProperties(table);
	if(t)
		return;
	//
	t	=	new TableProperties();
	t->table			=	table;
	t->id				=	table->GetTableID();
	GCPropertyBase* base	=	&t->base;
	//
	InitProp(t->base);
	//
	m_tableProperties.push_back(t);
	//
	CMFCPropertyGridProperty* groupTemp = new CMFCPropertyGridProperty(Language(L"基本属性"));
	base->AddProperty(groupTemp);
	groupTemp->AddSubItem(t->idPtr = AddPropInt(L"ID", Language(L"ID", L"唯一ID。")));
	for(int i=0;i<table->GetColumnCount();i++)
	{
		ILogicColumnName* name	=	table->GetColumnNamePtr(i);
		int	arrayCount	=	table->GetArraySize(i);
		for(int a=0;a<arrayCount;a++)
		{
			int dataCount			=	name->GetDataTypeCount();
			if(dataCount > 1)
			{
				wchar_t szName[256];
				if(arrayCount > 1)
					swprintf(szName, L"%s[%d]", W(name->GetColumnName()).buf, a);
				else
					swprintf(szName, L"%s", W(name->GetColumnName()).buf);
				CMFCPropertyGridProperty* gt = new CMFCPropertyGridProperty(szName);
				base->AddProperty(gt);
				//
				for(int d=0;d<dataCount;d++)
				{
					AddDataType(t, table, gt, name, i, d, dataCount, a, arrayCount);
				}
			}
			else
				AddDataType(t, table, groupTemp, name, i, 0, 1, a, arrayCount);
		}
	}
}
VOID		GCViewLogicSystem::CloseFile(){
	for(int i=0;i<m_tableProperties.size();i++)
		delete m_tableProperties[i];
	safe_release(m_logicPtr);
}
BOOL		GCViewLogicSystem::OpenFile(const wchar_t* szFile)	//打开文件,如果失败则会自动删除
{
	this->m_strFile	=	szFile;
	if(!__super::OpenFile(szFile))
		return false;
	if(m_viewerProps.m_hWnd == 0)
		this->m_treeView.InsertItem(Language(L"LogicViewer", L"逻辑数据编辑器"), 0, 0);
	wchar_t dir[512];
	GetCurrentDirectory(512, dir);
	m_logicPtr	=	LoadLogicDataSystem(m_packagePtr, A(m_strFile.c_str()));
	if(!m_logicPtr)
	{
		safe_release(m_logicPtr);
		return false;
	}
	//
	RebuildTrees();
	if(m_viewerProps.m_hWnd == 0)
		InitViewerProps();
	return true;
}

VOID		GCViewLogicSystem::RebuildTrees()
{
	if(m_hViewItem)
		m_treeView.DeleteItem(m_hViewItem);
	wchar_t wFileName[256], wExt[256];
	_wsplitpath(m_strFile.c_str(), 0, 0, wFileName, wExt);
	wcscat(wFileName, wExt);
	m_hViewItem = m_treeView.InsertItem(wFileName, 23, 23);
	m_treeView.SetItemState(m_hViewItem, TVIS_BOLD, TVIS_BOLD);
	m_treeView.Expand(m_hViewItem, TVE_EXPAND);
	//
	this->ClearTreeItem();
	BuildAll();
	//
	m_treeView.m_callback	=	this;
}

VOID		GCViewLogicSystem::BuildAll()
{
	m_treeView.SelectItem(m_hViewItem);
	DeleteChildItems(m_hViewItem);
	m_treeView.SetItemData(m_hViewItem, AddTreeItem(LOGIC_SYSTEM));
	for(int i=0;i<this->m_logicPtr->GetTableCount();i++)
	{
		AddTableProperties(m_logicPtr->GetTablePtr(i));
		wchar_t buf[512];
		swprintf(buf, L"%s,%s", W(m_logicPtr->GetTablePtr(i)->GetTableName()).buf, W(m_logicPtr->GetTablePtr(i)->GetTableViewName()).buf);
		HTREEITEM hTable	=	this->m_treeView.InsertItem(buf, 21, 21, m_hViewItem);
		m_treeView.SetItemData(hTable, AddTreeItem(LOGIC_TABLE, m_logicPtr->GetTablePtr(i)->GetTableID()));
		BuildTable(hTable, m_logicPtr->GetTablePtr(i));
	}
}
VOID		GCViewLogicSystem::BuildTable(HTREEITEM hTable, ILogicMemoryTable* table)
{
	m_treeView.SelectItem(hTable);
	DeleteChildItems(hTable);
	for(int i=0;i<table->GetLineCount();i++)
	{
		ILogicMemoryLine* line	=	table->GetLinePtrByIndex(i);
		HTREEITEM hLine	=	this->m_treeView.InsertItem(W(line->GetLineName()), 35, 35, hTable);
		m_treeView.SetItemData(hLine, AddTreeItem(LOGIC_LINE, table->GetTableID(), line->GetLineID()));
	}
}
VOID		GCViewLogicSystem::OnActive()		//被激活
{
	__super::OnActive();
}

VOID		GCViewLogicSystem::OnDeActive()		//失去激活
{
	__super::OnDeActive();
}

void		GCViewLogicSystem::OnFrameMove(const RenderParameter& r)	//每次更新
{
}

void		GCViewLogicSystem::OnPushRenderObjects(const RenderParameter& r)	//渲染
{
}

BOOL		GCViewLogicSystem::SaveFile(BOOL bSaveAs)	//保存文件
{
	if(m_logicPtr)
		m_logicPtr->ExportLogicSystems(A(m_strFile.c_str()));
	return __super::SaveFile(bSaveAs);
}

void		GCViewLogicSystem::OnRenderingEnd(const RenderParameter& r)
{
}
//渲染
void		GCViewLogicSystem::OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI)
{
	switch(nMenuID)
	{
	case ID_LOGIC_INSERTLINE:
		pCmdUI->Enable(true);
		break;
	default:
		pCmdUI->Enable(false);
		break;
	}
}

void		GCViewLogicSystem::OnCommandUI(INT nMenuID)
{
	switch(nMenuID)
	{
	case ID_LOGIC_INSERTLINE:
		{
			for(int i=0;i<this->m_treeView.GetSelectCount();i++)
			{
				HTREEITEM hItem		=	m_treeView.GetSelect(i);
				TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
				if(type)
				{
					if(type->type == LOGIC_LINE || type->type == LOGIC_TABLE)
					{
						ILogicMemoryTable* table	=	this->m_logicPtr->GetTablePtrByID(type->p1);
						table->AddNewLine();
						if(type->type == LOGIC_LINE)
							this->BuildTable(m_treeView.GetParentItem(hItem), table);
						else
							this->BuildTable(hItem, table);
					}
				}
			}
		}
		break;
	}
}







BOOL		GCViewLogicSystem::IsCopyTree()
{
	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			switch(type->type)
			{
			case LOGIC_LINE:
				return true;
			}
		}
	}
	return false;
}

BOOL		GCViewLogicSystem::IsPasteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(IsCanPasteTable(type->type))
				return true;
		}
	}
	return false;
}

VOID		GCViewLogicSystem::OnDeleteTree()
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
		for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
		{
			TreeItemType* type	=	it->second;
			if(!type)
				continue;
			switch(type->type)
			{
			case LOGIC_LINE:
				{
					ILogicMemoryTable* table	=	this->m_logicPtr->GetTablePtrByID(type->p1);
					table->DelLineByID(type->p2);
					this->m_treeView.DeleteItem(it->first);
					this->SetModifyed();
				}
				break;
			}
		}
		this->m_treeView.ClearSelect();
		this->OnChangeTreeSelect(&m_treeView, NULL);
		this->SetModifyed();
	}
}

BOOL		GCViewLogicSystem::IsDeleteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			switch(type->type)
			{
			case LOGIC_LINE:
				return true;
			}
		}
	}
	return false;
}

VOID				GCViewLogicSystem::OnCopyTree()
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
		switch(type->type)
		{
		case LOGIC_LINE:
			types.push_back(type);
			break;
		}
	}
	SetTableLineCopy(this, &types);
}

VOID				GCViewLogicSystem::OnPasteTree()
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
		if(IsCanPasteTable(type->type))
		{
			bPaste	=	true;
			break;
		}
	}
	if(!bPaste)
		return;
	try{
		std::map<ILogicMemoryTable*, HTREEITEM> tables;
		for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
		{
			TreeItemType* type	=	it->second;
			if(!type)
				continue;
			switch(type->type)
			{
			case LOGIC_LINE:
				{
					for(int j=0;j<g_copyTable3Ds.size();j++)
					{
						if(g_copyTable3Ds[j].type.type != LOGIC_LINE)
							continue;
						ILogicMemoryTable* from		=	g_copyTable3Ds[j].editor->m_logicPtr->GetTablePtrByID(g_copyTable3Ds[j].type.p1);
						ILogicMemoryLine* fromLine	=	from->GetLinePtrByID(g_copyTable3Ds[j].type.p2);
						ILogicMemoryTable* to		=	m_logicPtr->GetTablePtrByID(type->p1);
						ILogicMemoryLine* toLine	=	to->GetLinePtrByID(type->p2);
						if(from && to && to->IsEqual(from))
						{
							tables[to]	=	m_treeView.GetParentItem(it->first);
							to->CloneLine(toLine, fromLine);
						}
						break;
					}
				}
				break;
			}
			for(std::map<ILogicMemoryTable*, HTREEITEM>::iterator it = tables.begin(); it != tables.end(); it++)
			{
				BuildTable(it->second, it->first);
			}
			this->OnChangeTreeSelect(&m_treeView, NULL);
			this->SetModifyed();
		}
	}
	catch(...)
	{
		assert(false);
		g_copyTable3Ds.clear();
	}
}

void		GCViewLogicSystem::OnTreeViewKeyDown(INT nChar)
{
	if(nChar == VK_DELETE)
	{
		this->OnDeleteTree();
	}
}



void GetColumnETypeNameD(char* ret, ColumnType t, ILogicColumnName* col, int dataIndex, const char* szOtherTableName)
{
	switch(t)
	{
	case ColumnType_STRING:
	case ColumnType_Content:
	case ColumnType_File:
		sprintf(ret, ".成员 %s, 文本型", col->GetColumnViewName(dataIndex));
		break;
	case ColumnType_INT:
		sprintf(ret, ".成员 %s, 整数型", col->GetColumnViewName(dataIndex));
		break;
	case ColumnType_FLOAT:
		sprintf(ret, ".成员 %s, 小数型", col->GetColumnViewName(dataIndex));
		break;
	case ColumnType_DOUBLE:
		sprintf(ret, ".成员 %s, 双精度小数型", col->GetColumnViewName(dataIndex));
		break;
	case ColumnType_FLOAT2:
		sprintf(ret, ".成员 %s, 幻影二维向量", col->GetColumnViewName(dataIndex));
		break;
	case ColumnType_FLOAT3:
		sprintf(ret, ".成员 %s, 幻影三维向量", col->GetColumnViewName(dataIndex));
		break;
	case ColumnType_FLOAT4:
		sprintf(ret, ".成员 %s, 幻影四维向量", col->GetColumnViewName(dataIndex));
		break;
	case ColumnType_INT2:
		sprintf(ret, ".成员 %s, 幻影屏幕坐标数据", col->GetColumnViewName(dataIndex));
		break;
	case ColumnType_INT4:
		sprintf(ret, ".成员 %s, 幻影矩形数据", col->GetColumnViewName(dataIndex));
		break;
	case ColumnType_OtherTable:
		sprintf(ret, ".成员 %s_%s_表索引值, 整数型", col->GetColumnViewName(dataIndex), szOtherTableName);
		break;
	case ColumnType_Boolean:
		sprintf(ret, ".成员 %s, 逻辑型", col->GetColumnViewName(dataIndex));
		break;
	case ColumnType_Color:
		sprintf(ret, ".成员 %s, 整数型", col->GetColumnViewName(dataIndex));
		break;
	}
}

VOID	BuildEDatas(ILogicMemoryTable* pTable, std::string& ret, std::vector<std::string>& tables)
{
#ifdef Business_Authorize
	/*
.版本 2

.数据类型 幻影三维向量, 公开, dasdasdsadasd
    .成员 x, 小数型, , , adadsad
    .成员 y, 小数型, , , dasdsa
    .成员 z, 小数型, , , dsadsa
	*/
	char szBuffer[2048];
	for(int col=0;col<pTable->GetColumnCount();col++)
	{
		ILogicColumnName* pCol	=	pTable->GetColumnNamePtr(col);
		if(pCol->GetDataTypeCount() > 1)
		{
			sprintf(szBuffer, "data_%s", pCol->GetColumnViewName(-1));
			bool bFind	=	false;
			for(int j=0;j<tables.size();j++)
			{
				if(stricmp(tables[j].c_str(), szBuffer) == 0)
				{
					bFind	=	true;
					break;
				}
			}
			if(!bFind)
			{
				tables.push_back( szBuffer );
				sprintf(szBuffer, "\r\n.数据类型 data_%s,公开,%s\r\n", pCol->GetColumnViewName(-1), pCol->GetColumnDescription());
				ret	+=	szBuffer;
				//
				for(int d=0;d<pCol->GetDataTypeCount();d++)
				{
					GetColumnETypeNameD(szBuffer, pCol->GetDataType(d), pCol, d, pCol->GetOtherTablePtr() ? pCol->GetOtherTablePtr()->GetTableViewName() : "");
					ret	+=	szBuffer;
					ret	+=	"\r\n";
				}
			}
		}
	}
	//
	sprintf(szBuffer, "\r\n.数据类型 data_%s,公开\r\n", pTable->GetTableViewName());
	ret	+=	szBuffer;
	sprintf(szBuffer, ".成员 ID, 整数型\r\n");
	ret	+=	szBuffer;
	//for(int col=0;col<pTable->GetColumnCount();col++)
	//{
	//	ILogicColumnName* pCol	=	pTable->GetColumnNamePtr(col);
	//	int arrayCount	=	pTable->GetArraySize(col);
	//	if(arrayCount > 1)
	//	{
	//		sprintf(szBuffer, "	static const int				%s_Max = %d;\r\n", pCol->GetColumnName(), arrayCount);
	//		ret	+=	szBuffer;
	//	}
	//}
	for(int col=0;col<pTable->GetColumnCount();col++)
	{
		ILogicColumnName* pCol	=	pTable->GetColumnNamePtr(col);
		int arrayCount	=	pTable->GetArraySize(col);
		if(pCol->GetDataTypeCount() == 1)
		{
			GetColumnETypeNameD(szBuffer, pCol->GetDataType(0), pCol, 0, pCol->GetOtherTablePtr() ? pCol->GetOtherTablePtr()->GetTableViewName() : "");
		}
		else
		{
			sprintf(szBuffer, ".成员 %s, data_%s", pCol->GetColumnViewName(-1), pCol->GetColumnViewName(-1));
		}
		ret	+=	szBuffer;
		if(arrayCount > 1)
		{
            sprintf(szBuffer, ", , \"%d\"", arrayCount);
			ret	+=	szBuffer;
		}
		sprintf(szBuffer, ", %s\r\n", pCol->GetColumnDescription());
		ret	+=	szBuffer;
	}
#endif
}

void GetColumnEReadValue(char* ret, ColumnType t, ILogicColumnName* col, int columnIndex, const char* szTable, const char* szArray, const char* szArrayL, const char* szOtherTableName)
{
	switch(t)
	{
	case ColumnType_STRING:
	case ColumnType_Content:
	case ColumnType_File:
		sprintf(ret, "v_%s [行索引].%s%s = 逻辑行.返回文本2(%d, 0, %s)", szTable, col->GetColumnViewName(0), szArrayL, columnIndex, szArray);
		break;
	case ColumnType_INT:
		sprintf(ret, "v_%s [行索引].%s%s = 逻辑行.返回整数值2(%d, 0, %s)", szTable, col->GetColumnViewName(0), szArrayL, columnIndex, szArray);
		break;
	case ColumnType_FLOAT:
		sprintf(ret, "v_%s [行索引].%s%s = 逻辑行.返回小数值2(%d, 0, %s)", szTable, col->GetColumnViewName(0), szArrayL, columnIndex, szArray);
		break;
	case ColumnType_DOUBLE:
		sprintf(ret, "v_%s [行索引].%s%s = 逻辑行.返回双精度值2(%d, 0, %s)", szTable, col->GetColumnViewName(0), szArrayL, columnIndex, szArray);
		break;
	case ColumnType_FLOAT2:
		sprintf(ret, "逻辑行.返回二维向量2(v_%s [行索引].%s%s, %d, 0, %s)", szTable, col->GetColumnViewName(0), szArrayL, columnIndex, szArray);
		break;
	case ColumnType_FLOAT3:
		sprintf(ret, "逻辑行.返回三维向量2(v_%s [行索引].%s%s, %d, 0, %s)", szTable, col->GetColumnViewName(0), szArrayL, columnIndex, szArray);
		break;
	case ColumnType_FLOAT4:
		sprintf(ret, "逻辑行.返回四维向量2(v_%s [行索引].%s%s, %d, 0, %s)", szTable, col->GetColumnViewName(0), szArrayL, columnIndex, szArray);
		break;
	case ColumnType_OtherTable:
		sprintf(ret, "v_%s [行索引].%s_%s_表索引值%s = 逻辑行.返回其他表格行2(%d, 0, %s) + 1", szTable, col->GetColumnViewName(0), szOtherTableName, szArrayL, columnIndex, szArray);
		break;
	case ColumnType_Boolean:
		sprintf(ret, "v_%s [行索引].%s%s = 逻辑行.返回整数值2(%d, 0, %s) > 0", szTable, col->GetColumnViewName(0), szArrayL, columnIndex, szArray);
		break;
	case ColumnType_Color:
		sprintf(ret, "v_%s [行索引].%s%s = 逻辑行.返回整数值2(%d, 0, %s)", szTable, col->GetColumnViewName(0), szArrayL, columnIndex, szArray);
		break;
	}
}

void GetColumnEReadValueD(char* ret, ColumnType t, ILogicColumnName* col, int columnIndex, const char* szTable, const char* szArray, const char* szArrayL, const char* szOtherTableName, const char* szDataType, int dataIndex)
{
	switch(t)
	{
	case ColumnType_STRING:
	case ColumnType_Content:
	case ColumnType_File:
		sprintf(ret, "v_%s [行索引].%s%s.%s = 逻辑行.返回文本2(%d, %d, %s)", szTable, szDataType, szArrayL, col->GetColumnViewName(dataIndex), columnIndex, dataIndex, szArray);
		break;
	case ColumnType_INT:
		sprintf(ret, "v_%s [行索引].%s%s.%s = 逻辑行.返回整数值2(%d, %d, %s)", szTable, szDataType, szArrayL, col->GetColumnViewName(dataIndex), columnIndex, dataIndex, szArray);
		break;
	case ColumnType_FLOAT:
		sprintf(ret, "v_%s [行索引].%s%s.%s = 逻辑行.返回小数值2(%d, %d, %s)", szTable, szDataType, szArrayL, col->GetColumnViewName(dataIndex), columnIndex, dataIndex, szArray);
		break;
	case ColumnType_DOUBLE:
		sprintf(ret, "v_%s [行索引].%s%s.%s = 逻辑行.返回双精度值2(%d, %d, %s)", szTable, szDataType, szArrayL, col->GetColumnViewName(dataIndex), columnIndex, dataIndex, szArray);
		break;
	case ColumnType_FLOAT2:
		sprintf(ret, "逻辑行.返回二维向量2(v_%s [行索引].%s%s.%s, %d, %d, %s)", szTable, szDataType, szArrayL, col->GetColumnViewName(dataIndex), columnIndex, dataIndex, szArray);
		break;
	case ColumnType_FLOAT3:
		sprintf(ret, "逻辑行.返回三维向量2(v_%s [行索引].%s%s.%s, %d, %d, %s)", szTable, szDataType, szArrayL, col->GetColumnViewName(dataIndex), columnIndex, dataIndex, szArray);
		break;
	case ColumnType_FLOAT4:
		sprintf(ret, "逻辑行.返回四维向量2(v_%s [行索引].%s%s.%s, %d, %d, %s)", szTable, szDataType, szArrayL, col->GetColumnViewName(dataIndex), columnIndex, dataIndex, szArray);
		break;
	case ColumnType_OtherTable:
		sprintf(ret, "v_%s [行索引].%s%s.%s_%s_表索引值 = 逻辑行.返回其他表格行2(%d, %d, %s) + 1", szTable, szDataType, szArrayL, col->GetColumnViewName(dataIndex), szOtherTableName, columnIndex, dataIndex, szArray);
		break;
	case ColumnType_Boolean:
		sprintf(ret, "v_%s [行索引].%s%s.%s = 逻辑行.返回整数值2(%d, %d, %s) > 0", szTable, szDataType, szArrayL, col->GetColumnViewName(dataIndex), columnIndex, dataIndex, szArray);
		break;
	case ColumnType_Color:
		sprintf(ret, "v_%s [行索引].%s%s.%s = 逻辑行.返回整数值2(%d, %d, %s)", szTable, szDataType, szArrayL, col->GetColumnViewName(dataIndex), columnIndex, dataIndex, szArray);
		break;
	}
}

VOID	BuildEBodyReads(ILogicMemoryTable* pTable, std::string& ret)
{
#ifdef Business_Authorize
/*
.版本 2

.子程序 读取_身体内部组成, 逻辑型, 公开
.局部变量 接口_逻辑表指针, 幻影接口_逻辑表
.局部变量 逻辑表, 幻影逻辑表_类
.局部变量 接口_逻辑行指针, 幻影接口_逻辑行
.局部变量 行索引, 整数型
.局部变量 数组索引, 整数型
.局部变量 类型索引, 整数型
.局部变量 逻辑行, 幻影逻辑行_类

.如果 (V逻辑数据系统.返回逻辑表_根据名称 (接口_逻辑表指针, “body_type”) ＝ 假)
    返回 (假)
.否则
.如果结束

逻辑表.设置接口指针 (接口_逻辑表指针)
.计次循环首 (逻辑表.返回行数 (), 行索引)
    逻辑表.返回逻辑行_根据索引 (接口_逻辑行指针, 行索引 － 1)
    逻辑行.设置接口指针 (接口_逻辑行指针)


    v_身体内部组成 [行索引].ID ＝ 逻辑行.行ID ()
    v_身体内部组成 [行索引].名称 ＝ 逻辑行.取行名称 ()
    .计次循环首 (3, 数组索引)
        .计次循环首 (2, 类型索引)
            v_身体内部组成 [行索引].说明 ＝ 逻辑行.返回文本 (1, 类型索引, 数组索引)
        .计次循环尾 ()
    .计次循环尾 ()


.计次循环尾 ()

*/
	char szBuffer[2048];
	sprintf(szBuffer, ".子程序 读取_%s, 逻辑型, 公开\r\n.局部变量 逻辑表, 幻影逻辑表_类\r\n.局部变量 接口_逻辑行指针, 幻影接口_逻辑行\r\n"
		".局部变量 行索引, 整数型\r\n.局部变量 数组索引, 整数型\r\n.局部变量 逻辑行, 幻影逻辑行_类\r\n", pTable->GetTableViewName());
	ret	+=	szBuffer;
	//
	sprintf(szBuffer, "逻辑表 = V逻辑数据系统.返回逻辑表_根据名称 (“%s”)\r\n.如果 (逻辑表.接口是否有效 () ＝ 假)\r\n返回 (假)\r\n.否则\r\n.如果结束\r\n", pTable->GetTableName());
	ret	+=	szBuffer;
	//
	sprintf(szBuffer, ".如果(取数组成员数(v_%s) < 逻辑表.返回行数())\r\n信息框(\"数组长度太小,必须得增加到\" + 到文本(逻辑表.返回行数()) + \"以上\")\r\n.否则\r\n.如果结束\r\n\r\n"
		".计次循环首 (逻辑表.返回行数 (), 行索引)\r\n逻辑行 = 逻辑表.返回逻辑行_根据索引 (行索引 － 1)\r\n", pTable->GetTableViewName());
	ret	+=	szBuffer;
	//
	sprintf(szBuffer, "v_%s [行索引].ID ＝ 逻辑行.行ID ()\r\nv_%s [行索引].名称 ＝ 逻辑行.取行名称 ()\r\n", pTable->GetTableViewName(), pTable->GetTableViewName());
	ret	+=	szBuffer;
	//
	char szFmt[128];
	for(int col=0;col<pTable->GetColumnCount();col++)
	{
		int arrayCount	=	pTable->GetArraySize(col);
		ILogicColumnName* pCol		=	pTable->GetColumnNamePtr(col);
		const char* szOtherTable	=	"";
		if(pCol->GetOtherTablePtr())
			szOtherTable			=	pCol->GetOtherTablePtr()->GetTableViewName();
		if(arrayCount > 1)
		{
			sprintf(szBuffer, ".计次循环首 (%d, 数组索引)\r\n", arrayCount);
			ret	+=	szBuffer;
			if(pCol->GetDataTypeCount() <= 1)
			{
				GetColumnEReadValue(szBuffer, pCol->GetDataType(0), pCol, col, pTable->GetTableViewName(), "数组索引 - 1", "[数组索引]", szOtherTable);
				ret	+=	szBuffer;
				ret	+=	"\r\n";
			}
			else
			{
				for(int d=0;d<pCol->GetDataTypeCount();d++)
				{
					GetColumnEReadValueD(szBuffer, pCol->GetDataType(d), pCol, col, pTable->GetTableViewName(), "数组索引 - 1", "[数组索引]", szOtherTable, pCol->GetColumnViewName(-1), d);
					ret	+=	szBuffer;
					ret	+=	"\r\n";
				}
			}
			sprintf(szBuffer, ".计次循环尾 ()\r\n");
			ret	+=	szBuffer;
		}
		else
		{
			if(pCol->GetDataTypeCount() <= 1)
			{
				GetColumnEReadValue(szBuffer, pCol->GetDataType(0), pCol, col, pTable->GetTableViewName(), "0", "", szOtherTable);
				ret	+=	szBuffer;
				ret	+=	"\r\n";
			}
			else
			{
				for(int d=0;d<pCol->GetDataTypeCount();d++)
				{
					GetColumnEReadValueD(szBuffer, pCol->GetDataType(d), pCol, col, pTable->GetTableViewName(), "0", "", szOtherTable, pCol->GetColumnViewName(-1), d);
					ret	+=	szBuffer;
					ret	+=	"\r\n";
				}
			}
		}
	}
	sprintf(szBuffer, ".计次循环尾 ()\r\n\r\n返回(真)\r\n");
	ret		+=	szBuffer;
#endif
}
//.版本 2
//
//.数据类型 数据类型1, 公开
//    .成员 aa, 整数型, , "235"
//    .成员 aa, 整数型, , "235", dsadsa

VOID	GCViewLogicSystem::BuildECode()	//构造易语言表格类
{
	char szBuffer[2048];
	{
		char szHFile[256];
		strcpy(szHFile, GetProjectPath());
		strcat(szHFile, "src/自定义类型数据.txt");
		FILE* pFile	=	fopen(szHFile, "wb");
		if(!pFile)
		{
			return;
		}
		std::string strWrite;
		//
//.版本 2
//.数据类型 幻影二维向量, 公开
//    .成员 x, 小数型
//    .成员 y, 小数型
//
//.数据类型 幻影三维向量, 公开
//    .成员 x, 小数型
//    .成员 y, 小数型
//    .成员 z, 小数型

		strWrite	=	".版本 2\r\n";
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		std::vector<std::string> properties;
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			strWrite	=	"";
			BuildEDatas(pTablePtr, strWrite, properties);
			fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		}
		fclose(pFile);
	}
	{
		char szHFile[256];
		strcpy(szHFile, GetProjectPath());
		strcat(szHFile, "src/全局变量列表.txt");
		FILE* pFile	=	fopen(szHFile, "wb");
		if(!pFile)
		{
			return;
		}
		std::string strWrite;
		strWrite	=	".版本 2\r\n";
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			strWrite	=	"";
				/*.版本 2

.全局变量 进度1, 小数型, 公开
.全局变量 读取类型1, 整数型, 公开
.全局变量 显示文本, 逻辑型, 公开.版本 2

.全局变量 线条1, 幻影屏幕坐标颜色, 公开, "500"
.版本 2

.如果 (读取_身体内部组成 () ＝ 假)
    返回 (假)
.否则

.如果结束

.如果 (读取_物品类型 () ＝ 假)
    返回 (假)
.否则

.如果结束

.如果 (读取_光标 () ＝ 假)
    返回 (假)
.否则

.如果结束


*/
			sprintf(szBuffer, ".全局变量 v_%s,data_%s,公开,\"%d\"\r\n", pTablePtr->GetTableViewName(), pTablePtr->GetTableViewName(), pTablePtr->GetLineCount() * 2 + 255);
			strWrite	=	szBuffer;
			fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		}
		fclose(pFile);
	}
	{
		char szHFile[256];
		strcpy(szHFile, GetProjectPath());
		strcat(szHFile, "src/读取数据程序集.txt");
		FILE* pFile	=	fopen(szHFile, "wb");
		if(!pFile)
		{
			return;
		}
		std::string strWrite;
		strWrite	=	".版本 2\r\n";
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			strWrite	=	"";
			BuildEBodyReads(pTablePtr, strWrite);
			fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		}
		strWrite	=	".子程序 读取逻辑数据, 逻辑型, 公开\r\n.参数 文件名称, 文本型\r\n.如果(V逻辑数据系统.载入(文件名称) = 假)\r\n返回(假)\r\n.否则\r\n.如果结束\r\n\r\n";
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			strWrite	=	"";
			sprintf(szBuffer, ".如果 (读取_%s () ＝ 假)\r\n返回 (假)\r\n.否则\r\n.如果结束\r\n\r\n", pTablePtr->GetTableViewName());
			strWrite	=	szBuffer;
			fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		}
		strWrite	=	"返回(真)\r\n";
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		//
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			sprintf(szBuffer, ".子程序 查找_%s, 整数型, 公开\r\n.参数 ID, 整数型\r\n.局部变量 行索引, 整数型\r\n.计次循环首 (取数组成员数(v_%s), 行索引)\r\n"
				".如果(v_%s[行索引].ID ＝ ID)\r\n返回(行索引)\r\n.否则\r\n.如果结束\r\n\r\n.计次循环尾 ()\r\n返回(0)\r\n", pTablePtr->GetTableViewName(), pTablePtr->GetTableViewName(), pTablePtr->GetTableViewName());
			strWrite = szBuffer;
			fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		}
		fclose(pFile);
	}
	GetMainFrame()->MessageBox(L"易模块生成完毕");
}







void GetColumnVariantName(char* ret, ColumnType t, ILogicColumnName* col, int dataIndex)
{
	switch(t)
	{
	case ColumnType_STRING:
		if(stricmp(col->GetColumnName(), "name") == 0)
			sprintf(ret, "m_sz%s", col->GetColumnName(dataIndex));
		else
			sprintf(ret, "m_sz%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_Content:
	case ColumnType_File:
		sprintf(ret, "m_sz%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_INT:
		sprintf(ret, "m_n%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_FLOAT:
		sprintf(ret, "m_f%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_DOUBLE:
		sprintf(ret, "m_d%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_FLOAT2:
		sprintf(ret, "m_v%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_FLOAT3:
		sprintf(ret, "m_v%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_FLOAT4:
		sprintf(ret, "m_v%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_INT2:
		sprintf(ret, "m_v%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_INT4:
		sprintf(ret, "m_v%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_OtherTable:
		sprintf(ret, "m_p%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_Boolean:
		sprintf(ret, "m_b%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_Color:
		sprintf(ret, "m_n%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_Unknown:
		sprintf(ret, "m_v%s", col->GetColumnName(dataIndex));
		break;
	}
}

void GetColumnVariantType(char* ret, ColumnType t, ILogicColumnName* col, int dataIndex)
{
	switch(t)
	{
	case ColumnType_STRING:
		{
			if(stricmp(col->GetColumnName(), "name") == 0)
				sprintf(ret, "	char					m_sz%s[128]", col->GetColumnName(dataIndex));
			else
				sprintf(ret, "	std::string				m_sz%s", col->GetColumnName(dataIndex));
		}
		break;
	case ColumnType_Content:
	case ColumnType_File:
		sprintf(ret, "	std::string				m_sz%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_INT:
		sprintf(ret, "	int						m_n%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_FLOAT:
		sprintf(ret, "	float					m_f%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_DOUBLE:
		sprintf(ret, "	double					m_d%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_FLOAT2:
		sprintf(ret, "	float2					m_v%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_FLOAT3:
		sprintf(ret, "	float3					m_v%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_FLOAT4:
		sprintf(ret, "	float4					m_v%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_INT2:
		sprintf(ret, "	PPixel					m_v%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_INT4:
		sprintf(ret, "	PRect					m_v%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_OtherTable:
		{
			if(col->GetOtherTablePtr())
				sprintf(ret, "	data_%s*					m_p%s", col->GetOtherTablePtr()->GetTableName(), col->GetColumnName(dataIndex));
		}
		break;
	case ColumnType_Boolean:
		sprintf(ret, "	BOOL					m_b%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_Color:
		sprintf(ret, "	int						m_n%s", col->GetColumnName(dataIndex));
		break;
	case ColumnType_Unknown:
		sprintf(ret, "	data_%s					m_v%s", col->GetColumnName(), col->GetColumnName());
		break;
	}
}

VOID	BuildTableHead(ILogicMemoryTable* pTable, std::string& ret)
{
	char szBuffer[2048];
	sprintf(szBuffer, "//***************************************%s*****************************************\r\n", pTable->GetTableViewName());
	ret	+=	szBuffer;
	sprintf(szBuffer, "class data_%s\r\n{\r\n", pTable->GetTableName());
	ret	+=	szBuffer;
	sprintf(szBuffer, "public:\r\n	data_%s(){}\r\n", pTable->GetTableName());
	ret	+=	szBuffer;
	sprintf(szBuffer, "public:\r\n", pTable->GetTableName());
	ret	+=	szBuffer;
	for(int col=0;col<pTable->GetColumnCount();col++)
	{
		ILogicColumnName* pCol	=	pTable->GetColumnNamePtr(col);
		if(pCol->GetDataTypeCount() > 1)
		{
			sprintf(szBuffer, "struct data_%s//%s\r\n{\r\n", pCol->GetColumnName(-1), pCol->GetColumnDescription());
			ret	+=	szBuffer;
			//
			for(int d=0;d<pCol->GetDataTypeCount();d++)
			{
				GetColumnVariantType(szBuffer, pCol->GetDataType(d), pCol, d);
				ret	+=	szBuffer;
				sprintf(szBuffer, ";//%s\r\n", pCol->GetColumnViewName(d));
				ret	+=	szBuffer;
			}
			sprintf(szBuffer, "\r\n};\r\n");
			ret	+=	szBuffer;
		}
	}
	sprintf(szBuffer, "	int						m_nID;\r\n");
	ret	+=	szBuffer;
	sprintf(szBuffer, "	int						m_nIndex;\r\n");
	ret	+=	szBuffer;
	for(int col=0;col<pTable->GetColumnCount();col++)
	{
		ILogicColumnName* pCol	=	pTable->GetColumnNamePtr(col);
		int arrayCount	=	pTable->GetArraySize(col);
		if(arrayCount > 1)
		{
			sprintf(szBuffer, "	static const int				%s_Max = %d;\r\n", pCol->GetColumnName(), arrayCount);
			ret	+=	szBuffer;
		}
	}
	for(int col=0;col<pTable->GetColumnCount();col++)
	{
		ILogicColumnName* pCol	=	pTable->GetColumnNamePtr(col);
		int arrayCount	=	pTable->GetArraySize(col);
		if(pCol->GetDataTypeCount() == 1)
		{
			GetColumnVariantType(szBuffer, pCol->GetDataType(0), pCol, -1);
		}
		else
		{
			sprintf(szBuffer, "	data_%s			m_v%s", pCol->GetColumnName(), pCol->GetColumnName());
		}
		ret	+=	szBuffer;
		if(arrayCount > 1)
			sprintf(szBuffer, "[%s_Max];//%s\r\n", pCol->GetColumnName(), pCol->GetColumnDescription());
		else
			sprintf(szBuffer, ";//%s\r\n", pCol->GetColumnDescription());
		ret	+=	szBuffer;
	}
	sprintf(szBuffer, "};\r\n\r\n");
	ret		+=	szBuffer;
}


void GetColumnCPPReadValue(char* ret, ColumnType t, ILogicColumnName* col, int colIndex, int dataIndex, const char* szArray, const char* szExt, BOOL bIsDataType = false)
{
	if(bIsDataType)
	{
		switch(t)
		{
		case ColumnType_STRING:
			{
				if(stricmp("name", col->GetColumnName()) == 0)
					sprintf(ret, "			strcpy(ts->m_v%s%s, pLinePtr.GetText_2(%d, %d, %s)); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
				else
				{
					sprintf(ret, "			if(bLoadStrings)\r\nts->m_v%s%s	=	pLinePtr.GetText_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
				}
			}
			break;
		case ColumnType_Content:
		case ColumnType_File:
			sprintf(ret, "			if(bLoadStrings)\r\nts->m_v%s%s	=	pLinePtr.GetText_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			break;
		case ColumnType_INT:
			sprintf(ret, "			ts->m_v%s%s	=	pLinePtr.GetInt_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			break;
		case ColumnType_FLOAT:
			sprintf(ret, "			ts->m_v%s%s	=	pLinePtr.GetFloat_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			break;
		case ColumnType_DOUBLE:
			sprintf(ret, "			ts->m_v%s%s	=	pLinePtr.GetDouble_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			break;
		case ColumnType_FLOAT2:
			sprintf(ret, "			ts->m_v%s%s	=	*pLinePtr.->GetFloat2Ptr_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			break;
		case ColumnType_FLOAT3:
			sprintf(ret, "			ts->m_v%s%s	=	*pLinePtr.GetFloat3Ptr_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			break;
		case ColumnType_FLOAT4:
			sprintf(ret, "			ts->m_v%s%s	=	*pLinePtr.GetFloat4Ptr_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			break;
		case ColumnType_INT2:
			sprintf(ret, "			ts->m_v%s%s	=	*pLinePtr.GetPointPtr_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			break;
		case ColumnType_INT4:
			sprintf(ret, "			ts->m_v%s%s	=	*pLinePtr.GetRectPtr_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			break;
		case ColumnType_OtherTable:
			sprintf(ret, "			ts->m_v%s%s	=	0;", col->GetColumnName(), szExt);
			break;
		case ColumnType_Boolean:
			sprintf(ret, "			ts->m_v%s%s	=	pLinePtr.GetInt_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			break;
		case ColumnType_Color:
			sprintf(ret, "			ts->m_v%s%s	=	pLinePtr.GetInt_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			break;
		}
		return;
	}
	switch(t)
	{
	case ColumnType_STRING:
		{
			if(stricmp("name", col->GetColumnName()) == 0)
				sprintf(ret, "			strcpy(ts->m_sz%s%s, pLinePtr.GetText_2(%d, %d, %s)); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			else
			{
				sprintf(ret, "			if(bLoadStrings)\r\nts->m_sz%s%s	=	pLinePtr.GetText_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
			}
		}
		break;
	case ColumnType_Content:
	case ColumnType_File:
		sprintf(ret, "			if(bLoadStrings)\r\nts->m_sz%s%s	=	pLinePtr.GetText_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
		break;
	case ColumnType_INT:
		sprintf(ret, "			ts->m_n%s%s	=	pLinePtr.GetInt_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
		break;
	case ColumnType_FLOAT:
		sprintf(ret, "			ts->m_f%s%s	=	pLinePtr.GetFloat_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
		break;
	case ColumnType_DOUBLE:
		sprintf(ret, "			ts->m_d%s%s	=	pLinePtr.GetDouble_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
		break;
	case ColumnType_FLOAT2:
		sprintf(ret, "			ts->m_v%s%s	=	*pLinePtr.GetFloat2Ptr_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
		break;
	case ColumnType_FLOAT3:
		sprintf(ret, "			ts->m_v%s%s	=	*pLinePtr.GetFloat3Ptr_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
		break;
	case ColumnType_FLOAT4:
		sprintf(ret, "			ts->m_v%s%s	=	*pLinePtr.GetFloat4Ptr_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
		break;
	case ColumnType_INT2:
		sprintf(ret, "			ts->m_v%s%s	=	*pLinePtr.GetPointPtr_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
		break;
	case ColumnType_INT4:
		sprintf(ret, "			ts->m_v%s%s	=	*pLinePtr.GetRectPtr_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
		break;
	case ColumnType_OtherTable:
		sprintf(ret, "			ts->m_p%s%s	=	0;", col->GetColumnName(), szExt);
		break;
	case ColumnType_Boolean:
		sprintf(ret, "			ts->m_b%s%s	=	pLinePtr.GetInt_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
		break;
	case ColumnType_Color:
		sprintf(ret, "			ts->m_n%s%s	=	pLinePtr.GetInt_2(%d, %d, %s); ", col->GetColumnName(), szExt, colIndex, dataIndex, szArray);
		break;
	}
}

VOID	BuildTableBodyReads(ILogicMemoryTable* pTable, std::string& ret)
{
	char szBuffer[2048];
	sprintf(szBuffer, 
		"{\r\n"
		"	CLogicTable pTable	=	pDataPtr.GetLogicTableByName(\"%s\");\r\n"
		"	if(pTable.Null)\r\n"
		"	{\r\n"
		"		pDataPtr.Destroy();\r\n"
		"		return false;\r\n"
		"	}\r\n"
		"	for(int line=0;line<pTable.GetLineCount();line++)\r\n"
		"	{\r\n"
		"		CLogicLine pLinePtr	=	pTable.GetLine(line);\r\n"
		, pTable->GetTableName()
		);
	ret	+=	szBuffer;
	//
	sprintf(szBuffer, "		data_%s * ts	=	new data_%s();\r\n	memset(ts, 0, sizeof(data_%s));\r\n	ts->m_nIndex	=	line;\r\n		ts->m_nID	=	pLinePtr.GetID();\r\n", pTable->GetTableName(), pTable->GetTableName(), pTable->GetTableName());
	ret	+=	szBuffer;
	//
	char szFmt[128];
	char szVarName[128];
	for(int col=0;col<pTable->GetColumnCount();col++)
	{
		int arrayCount	=	pTable->GetArraySize(col);
		ILogicColumnName* pCol	=	pTable->GetColumnNamePtr(col);
		if(arrayCount > 1)
		{
			sprintf(szBuffer, "for(int ary=0;ary<data_%s::%s_Max;ary++)\r\n"
				"{\r\n"
				, pTable->GetTableName(), pCol->GetColumnName());
			ret	+=	szBuffer;
			if(pCol->GetDataTypeCount() <= 1)
			{
				GetColumnCPPReadValue(szBuffer, pCol->GetDataType(0), pCol, col, 0, "ary", "[ary]");
				ret	+=	szBuffer;
				ret	+=	"\r\n";
			}
			else
			{
				for(int d=0;d<pCol->GetDataTypeCount();d++)
				{
					GetColumnVariantName(szVarName, pCol->GetDataType(d), pCol, d);
					sprintf(szFmt, "[ary].%s", szVarName);
					GetColumnCPPReadValue(szBuffer, pCol->GetDataType(d), pCol, col, d, "ary", szFmt, true);
					ret	+=	szBuffer;
					ret	+=	"\r\n";
				}
			}
			sprintf(szBuffer, "}\r\n");
			ret	+=	szBuffer;
		}
		else
		{
			if(pCol->GetDataTypeCount() <= 1)
			{
				GetColumnCPPReadValue(szBuffer, pCol->GetDataType(0), pCol, col, 0, "0", "");
				ret	+=	szBuffer;
				ret	+=	"\r\n";
			}
			else
			{
				for(int d=0;d<pCol->GetDataTypeCount();d++)
				{
					GetColumnVariantName(szVarName, pCol->GetDataType(d), pCol, d);
					sprintf(szFmt, ".%s", szVarName);
					GetColumnCPPReadValue(szBuffer, pCol->GetDataType(d), pCol, col, d, "0", szFmt, true);
					ret	+=	szBuffer;
					ret	+=	"\r\n";
				}
			}
		}
	}
	//
	sprintf(szBuffer, "		m_v%sMap.push_back(ts);\r\n", pTable->GetTableName());
	ret	+=	szBuffer;
	sprintf(szBuffer, "}\r\n}\r\n");
	ret		+=	szBuffer;
}

VOID	BuildTableBodyFinds(ILogicMemoryTable* pTable, std::string& ret)
{
	char szBuffer[2048];
	sprintf(szBuffer, 
		"{int l_index;\r\n"
		"	CLogicTable pTable	=	pDataPtr.GetLogicTableByName(\"%s\");\r\n"
		"	for(int ilist=0;ilist<m_v%sMap.size();ilist++)\r\n"
		"	{\r\n"
		"		data_%s* ts	=	m_v%sMap[ilist];\r\n"
		, pTable->GetTableName(), pTable->GetTableName(), pTable->GetTableName(), pTable->GetTableName()
		);
	ret	+=	szBuffer;
	//
	char szFmt[128];
	char szVarName[128];
	for(int col=0;col<pTable->GetColumnCount();col++)
	{
		int arrayCount	=	pTable->GetArraySize(col);
		ILogicColumnName* pCol	=	pTable->GetColumnNamePtr(col);
		bool bFind	=	false;
		for(int d=0;d<pCol->GetDataTypeCount();d++)
		{
			if(pCol->GetDataType(0) == ColumnType_OtherTable)
			{
				bFind	=	true;
				break;
			}
		}
		if(!bFind)
			continue;
		if(arrayCount > 1)
		{
			sprintf(szBuffer, "for(int ary=0;ary<data_%s::%s_Max;ary++)\r\n"
				"{\r\n"
				, pTable->GetTableName(), pCol->GetColumnName());
			ret	+=	szBuffer;
			if(pCol->GetDataTypeCount() <= 1)
			{
				if(pCol->GetDataType(0) == ColumnType_OtherTable)
				{
					if(pCol->GetOtherTablePtr())
					{
						GetColumnVariantName(szVarName, pCol->GetDataType(0), pCol, -1);
						sprintf(szBuffer, "		l_index	=	pTable.GetLine(ts->m_nIndex).GetOtherTableLine_2(%d, 0, ary);\r\n", col);
						ret	+=	szBuffer;
						sprintf(szBuffer, "		if(l_index>=0)\r\n{\r\n"
							"			data_%s* f	=	m_v%sMap[l_index];\r\n"
						"			if(f)\r\nts->%s[ary]	=	f;\r\nelse\r\nts->%s[ary] = 0;\r\n"
						"		}\r\n"
						, pCol->GetOtherTablePtr()->GetTableName(), pCol->GetOtherTablePtr()->GetTableName(), szVarName, szVarName);
						ret	+=	szBuffer;//
					}
				}
			}//sprintf(ret, "			ts->m_%s%s	=	0", col->GetColumnName(), szExt);
			else
			{
				for(int d=0;d<pCol->GetDataTypeCount();d++)
				{
					if(pCol->GetDataType(d) == ColumnType_OtherTable)
					{
						if(pCol->GetOtherTablePtr())
						{
							char szVarName2[128];
							GetColumnVariantName(szVarName2, pCol->GetDataType(d), pCol, d);
							GetColumnVariantName(szVarName, ColumnType_Unknown, pCol, -1);

							sprintf(szBuffer, "		l_index	=	pTable.GetLine(ts->m_nIndex).GetOtherTableLine_2(%d, %d, ary);\r\n", col, d);
							ret	+=	szBuffer;
							sprintf(szBuffer, "		if(l_index>=0)\r\n{\r\n"
								"			data_%s* f	=	m_v%sMap[l_index];\r\n"
								"			if(f)\r\nts->%s[ary].%s	=	f;\r\nelse\r\nts->%s[ary].%s = 0;\r\n"
								"		}\r\n"
								, pCol->GetOtherTablePtr()->GetTableName(), pCol->GetOtherTablePtr()->GetTableName(), szVarName, szVarName2, szVarName, szVarName2);//pCol->GetColumnName(), pCol->GetColumnName(d), pCol->GetColumnName(), pCol->GetColumnName(d)
							ret	+=	szBuffer;//
						}
					}
				}
			}
			sprintf(szBuffer, "}\r\n");
			ret	+=	szBuffer;
		}
		else
		{
			if(pCol->GetDataTypeCount() <= 1)
			{
				if(pCol->GetDataType(0) == ColumnType_OtherTable)
				{
					if(pCol->GetOtherTablePtr())
					{
						GetColumnVariantName(szVarName, pCol->GetDataType(0), pCol, -1);
						sprintf(szBuffer, "		l_index	=	pTable.GetLine(ts->m_nIndex).GetOtherTableLine_2(%d, 0, 0);\r\n", col);
						ret	+=	szBuffer;
						sprintf(szBuffer, "		if(l_index>=0)\r\n{\r\n"
							"			data_%s* f	=	m_v%sMap[l_index];\r\n"
						"			if(f)\r\nts->%s	=	f;\r\nelse\r\nts->%s = 0;\r\n"
						"		}\r\n"
						, pCol->GetOtherTablePtr()->GetTableName(), pCol->GetOtherTablePtr()->GetTableName(), szVarName, szVarName);//pCol->GetColumnName(), pCol->GetColumnName());
						ret	+=	szBuffer;//
					}
				}
			}//sprintf(ret, "			ts->m_%s%s	=	0", col->GetColumnName(), szExt);
			else
			{
				for(int d=0;d<pCol->GetDataTypeCount();d++)
				{
					if(pCol->GetDataType(d) == ColumnType_OtherTable)
					{
						if(pCol->GetOtherTablePtr())
						{
							char szVarName2[128];
							GetColumnVariantName(szVarName, ColumnType_Unknown, pCol, -1);
							GetColumnVariantName(szVarName2, pCol->GetDataType(d), pCol, d);
							sprintf(szBuffer, "		l_index	=	pTable.GetLine(ts->m_nIndex).GetOtherTableLine_2(%d, %d, 0);\r\n", col, d);
							ret	+=	szBuffer;
							sprintf(szBuffer, "		if(l_index>=0)\r\n{\r\n"
								"			data_%s* f	=	m_v%sMap[l_index];\r\n"
								"			if(f)\r\nts->%s.%s	=	f;\r\nelse\r\nts->%s.%s = 0;\r\n"
								"		}\r\n"
								, pCol->GetOtherTablePtr()->GetTableName(), pCol->GetOtherTablePtr()->GetTableName(), szVarName,szVarName2,szVarName,szVarName2);//pCol->GetColumnName(), pCol->GetColumnName(d), pCol->GetColumnName(), pCol->GetColumnName(d)
							ret	+=	szBuffer;//
						}
					}
				}
			}
		}
	}
	sprintf(szBuffer, "}\r\n}\r\n");
	ret		+=	szBuffer;
}




VOID GCViewLogicSystem::BuildCppCode()	//构造C++表格类
{
	DefineAuth_6();
	if(!CheckAuth_6(0))
		return;
	SetProjectPath();
	char szBuffer[2048];
	{
		char szHFile[256];
		strcpy(szHFile, GetProjectPath());
		strcat(szHFile, "src/dataMgr.h");
		FILE* pFile	=	fopen(szHFile, "wb");
		if(!pFile)
		{
			GetMainFrame()->MessageBoxW(Language(L"文件打不开"));
			return;
		}
		std::string strWrite;
		//
		strWrite	=	"//////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
			"/*\r\n"
			"	文件       :   dataMgr.h\r\n"
			"	软件名称   :   幻影游戏引擎\r\n"
			"\r\n"
			"	设计编写者 :   赵德贤(英文:ZhaoDexian)\r\n"
			"	Email: yuzhou_995@hotmail.com\r\n"
			"	\r\n"
			"	Copyright 2009-2016 Zhao Dexian\r\n"
			"	\r\n"
			"	-------------------------------------------------------------------------------------------------\r\n"
			"\r\n"
			"\r\n"
			"	-------------------------------------------------------------------------------------------------\r\n"
			"	*/\r\n"
			"//////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
			"#ifndef ___DATA_MANAGER__H___\r\n"
			"#define ___DATA_MANAGER__H___\r\n"
			"\r\n"
			"\r\n"
			"\r\n"
			"#pragma once\r\n"
			"#include <vector>\r\n"
			"#include <string>\r\n"
			"\r\n"
			"\r\n"
			"\r\n";
		//
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		strWrite	=	"";
		for(int i=0;i<this->m_logicPtr->GetTableCount();i++)
		{
			sprintf(szBuffer, "class\tdata_%s;\r\n", m_logicPtr->GetTablePtr(i)->GetTableName());
			strWrite	+=	szBuffer;
		}
		strWrite	+=	"\r\n";
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			strWrite	=	"";
			BuildTableHead(pTablePtr, strWrite);
			fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		}
		sprintf(szBuffer, "\r\n#pragma pack(push, 1)\r\nstruct DataID{\r\n	unsigned short		index;\r\n	unsigned short		id;\r\nDataID(int _id = -1, int _index = -1){id = _id; index = _index;}\r\n};\r\n#pragma pack(pop)\r\n");
		strWrite		=	szBuffer;
		//
		sprintf(szBuffer, "\r\n//***************************************CDataManager*****************************************\r\n");
		strWrite		+=	szBuffer;
		sprintf(szBuffer, "class CDataManager\r\n{\r\npublic:\r\n	CDataManager();\r\n	~CDataManager();\r\n");
		strWrite		+=	szBuffer;
		//
		sprintf(szBuffer, "	bool								LoadLogicSystem(const char* szHeaderCSVFile, BOOL bEncrypted = false, bool bLoadStrings = true/*这段表示是否读取所有字符串信息,在服务器状态下这段可以设置成false,以节省内存占用量*/);\r\n");
		strWrite		+=	szBuffer;
		//
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			sprintf(szBuffer, "	data_%s*						Find%s(DataID& idInOut);\r\n", pTablePtr->GetTableName(), pTablePtr->GetTableName());
			strWrite	+=	szBuffer;
		}
		//
		sprintf(szBuffer, "//******************************************************************************\r\n");
		strWrite		+=	szBuffer;
		//
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			sprintf(szBuffer, "	DataID							Get%sByIndex(int index);\r\n", pTablePtr->GetTableName());
			strWrite	+=	szBuffer;
			sprintf(szBuffer, "	DataID							Get%sByID(int id);\r\n", pTablePtr->GetTableName());
			strWrite	+=	szBuffer;
		}
		//
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			sprintf(szBuffer, "	std::vector<data_%s*>			m_v%sMap;//%s\r\n", pTablePtr->GetTableName(), pTablePtr->GetTableName(), pTablePtr->GetTableViewName());
			strWrite	+=	szBuffer;
		}
		//
		sprintf(szBuffer, "};\r\n\r\nCDataManager*				GetDataManager();\r\n\r\n#endif\r\n");
		strWrite		+=	szBuffer;
		//
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		//char szCPPFile[256];
		//sprintf(szCPPFile, "%s.cpp", A(this->m_strFile));
		fclose(pFile);
	}
	{
		char szHFile[256];
		strcpy(szHFile, GetProjectPath());
		strcat(szHFile, "src/dataMgr.cpp");
		FILE* pFile	=	fopen(szHFile, "wb");
		if(!pFile)
		{
			GetMainFrame()->MessageBoxW(Language(L"文件打不开"));
			return;
		}
		std::string strWrite;
		strWrite	=	"//////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
			"/*\r\n"
			"	文件       :   dataMgr.cpp\r\n"
			"	软件名称   :   幻影游戏引擎\r\n"
			"\r\n"
			"	设计编写者 :   赵德贤(英文:ZhaoDexian)\r\n"
			"	Email: yuzhou_995@hotmail.com\r\n"
			"	\r\n"
			"	Copyright 2009-2016 Zhao Dexian\r\n"
			"	\r\n"
			"	-------------------------------------------------------------------------------------------------\r\n"
			"\r\n"
			"\r\n"
			"	-------------------------------------------------------------------------------------------------\r\n"
			"	*/\r\n"
			"//////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
			"\r\n"
			"\r\n"
			"#include \"StdAfx.h\"\r\n"
			"#include \"dataMgr.h\"\r\n"
			"#include \"MyAppBase.h\"\r\n"
			"#include <assert.h>\r\n"
			"\r\n"
			"\r\n";
		//
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		//
		strWrite	=	"CDataManager*		g_data_manager	=	0;\r\nCDataManager*		GetDataManager()\r\n{\r\nreturn g_data_manager;\r\n}\r\n";
		sprintf(szBuffer, "CDataManager::CDataManager()\r\n{\r\ng_data_manager	=	this;\r\n}\r\n");
		strWrite	+=	szBuffer;
		sprintf(szBuffer, "CDataManager::~CDataManager()\r\n{\r\ng_data_manager	=	0;\r\n");
		strWrite	+=	szBuffer;
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			sprintf(szBuffer, "	for(int i=0;i<m_v%sMap.size();i++)\r\n{\r\n	delete m_v%sMap[i];\r\n}\r\n", pTablePtr->GetTableName(), pTablePtr->GetTableName());
			strWrite	+=	szBuffer;
		}
		sprintf(szBuffer, "\r\n}\r\n");
		strWrite	+=	szBuffer;
		//
		strcpy(szBuffer, "bool			CDataManager::LoadLogicSystem(const char* szLogicFile, BOOL bEncrypted, bool bLoadStrings)\r\n{\r\n"
			"	CLogicData pDataPtr;\r\n"
			"	if(!pDataPtr.Load(\"\", szLogicFile))\r\n"
			"		return false;\r\n"
			);
		strWrite	+=	szBuffer;
		//
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		//
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			strWrite	=	"";
			BuildTableBodyReads(pTablePtr, strWrite);
			fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		}
		//
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			strWrite	=	"";
			BuildTableBodyFinds(pTablePtr, strWrite);
			fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		}
		sprintf(szBuffer, "pDataPtr.Destroy();\r\nreturn true;\r\n}\r\n");
		strWrite	=	szBuffer;
		//
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		//
		strWrite	=	"";
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			sprintf(szBuffer, "//*********************************************************************************************************\r\n	data_%s*						CDataManager::Find%s(DataID& idInOut)\r\n{\r\n", pTablePtr->GetTableName(), pTablePtr->GetTableName());
			strWrite	+=	szBuffer;
			//
			sprintf(szBuffer, "	if(idInOut.id < 0) return 0;\r\n if(idInOut.index < 0 || idInOut.index >= m_v%sMap.size() || (m_v%sMap[idInOut.index]->m_nID != idInOut.id))\r\n"
				"		idInOut	=	Get%sByID(idInOut.id);\r\n"
				"	if(idInOut.id < 0)return 0;\r\n"
				"	return m_v%sMap[idInOut.index];\r\n"
				"}\r\n"
				, pTablePtr->GetTableName(), pTablePtr->GetTableName(), pTablePtr->GetTableName(), pTablePtr->GetTableName());
			strWrite	+=	szBuffer;
			//
		}
		//
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		//
		strWrite	=	"";
		for(int i=0;i<m_logicPtr->GetTableCount();i++)
		{
			ILogicMemoryTable* pTablePtr	=	m_logicPtr->GetTablePtr(i);
			sprintf(szBuffer, "	DataID						CDataManager::Get%sByIndex(int index)\r\n{\r\nassert(index >= 0 && index < m_v%sMap.size());\r\n if(index < 0 || index >= m_v%sMap.size())\r\nreturn DataID(-1, -1);\r\nreturn DataID(m_v%sMap[index]->m_nID, index);\r\n}\r\n", pTablePtr->GetTableName(), pTablePtr->GetTableName(), pTablePtr->GetTableName(), pTablePtr->GetTableName());
			strWrite	+=	szBuffer;
			sprintf(szBuffer, "	DataID						CDataManager::Get%sByID(int id)\r\n{\r\n"
				"int index = -1;\r\n"
				"for(int i=0;i<m_v%sMap.size();i++)\r\n"
				"{if(m_v%sMap[i]->m_nID == id)return DataID(id, i);}\r\n"
				" if(index < 0)return DataID(-1, -1);\r\n return DataID(id, index);\r\n}\r\n", pTablePtr->GetTableName(), pTablePtr->GetTableName(), pTablePtr->GetTableName());
			strWrite	+=	szBuffer;
		}
		fwrite(strWrite.c_str(), strWrite.size(), 1, pFile);
		//
		//char szCPPFile[256];
		//sprintf(szCPPFile, "%s.cpp", A(this->m_strFile));
		fclose(pFile);
	}
	GetMainFrame()->MessageBoxW(Language(L"C++模块生成完毕"));
}
