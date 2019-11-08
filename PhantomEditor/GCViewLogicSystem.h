#pragma once
#include "GCViewBase.h"
#include "IUnit3D.h"
#include "TrackAnimationCtrl.h"
#include "ILogic.h"

class GCViewLogicSystem: public GCViewBase
{
public:
	GCViewLogicSystem(CPhantomGCDoc* docPtr);
	~GCViewLogicSystem(void);
	virtual	VOID				OnActive();		//被激活
	virtual	VOID				OnDeActive();	//失去激活
	virtual	VOID				CloseFile();
	virtual	BOOL				OpenFile(const wchar_t* szFile);	//打开文件,如果失败则会自动删除
	virtual	BOOL				SaveFile(BOOL bSaveAs);	//保存文件
	virtual void				OnFrameMove(const RenderParameter& r);	//每次更新
	virtual void				OnPushRenderObjects(const RenderParameter& r);	//渲染
	virtual	FileViewerType		GetViewerType(){return FileViewerType_LogicSystem;}
	virtual	HICON				GetViewerIcon();
	virtual	INT					GetPopupMenuID(){return 16;}
	virtual void				OnRenderingEnd(const RenderParameter& r);	//渲染
	//
	virtual	VOID				OnCopyTree();
	virtual	VOID				OnPasteTree();
	virtual	BOOL				IsCopyTree();
	virtual	BOOL				IsPasteTree();
	virtual	VOID				OnDeleteTree();
	virtual	BOOL				IsDeleteTree();
	//
	void						OnTreeViewKeyDown(INT nChar);
	//
	VOID						RebuildTrees();
	enum{
		LOGIC_NONE	=	0,
		LOGIC_SYSTEM,
		LOGIC_TABLE,
		LOGIC_LINE,
	};
	void						OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp);
	VOID						OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem);	//更新选择树形控件
	virtual	void				OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI);
	virtual	void				OnCommandUI(INT nMenuID);
	//
	VOID						BuildTable(HTREEITEM hItem, ILogicMemoryTable* table);
	//
	VOID						BuildAll();
	//
	struct	TablePropertyPtr
	{
		int							columnIndex;
		int							arrayIndex;
		int							dataTypeIndex;
		int							subIndex;
		PropertyPtr					ptr;
		TablePropertyPtr(){ptr = 0; columnIndex = 0; arrayIndex = 0; dataTypeIndex = 0; subIndex = 0;}
	};
	struct	TableProperties
	{
		int							id;
		GCPropertyBase				base;
		PropertyPtr					idPtr;
		ILogicMemoryTable*			table;
		std::vector<TablePropertyPtr>	columns;
		//
		PropertyPtr					FindColumn(int columnIndex, int arrayIndex, int dataTypeIndex, int subIndex = 0)
		{
			for(int i=0;i<columns.size();i++)
			{
				if(columns[i].columnIndex == columnIndex && columns[i].arrayIndex == arrayIndex && columns[i].dataTypeIndex == dataTypeIndex && columns[i].subIndex == subIndex)
					return columns[i].ptr;
			}
			return 0;
		}
		PropertyPtr					AddColumn(int columnIndex, int arrayIndex, int dataTypeIndex, PropertyPtr ptr)
		{
			TablePropertyPtr t;
			t.ptr			=	ptr;
			t.columnIndex	=	columnIndex;
			t.arrayIndex	=	arrayIndex;
			t.dataTypeIndex	=	dataTypeIndex;
			t.subIndex		=	0;
			columns.push_back(t);
			return ptr;
		}
		//
		PropertyPtr					AddColumn(int columnIndex, int arrayIndex, int dataTypeIndex, int subIndex, PropertyPtr ptr)
		{
			TablePropertyPtr t;
			t.ptr			=	ptr;
			t.columnIndex	=	columnIndex;
			t.arrayIndex	=	arrayIndex;
			t.dataTypeIndex	=	dataTypeIndex;
			t.subIndex		=	subIndex;
			columns.push_back(t);
			return ptr;
		}
	};
	std::vector<TableProperties*>	m_tableProperties;
	//
	TableProperties*	FindProperties(ILogicMemoryTable* t);
	//
	VOID				AddTableProperties(ILogicMemoryTable* table);
	VOID				AddDataType(TableProperties* tp, ILogicMemoryTable* table, CMFCPropertyGridProperty* groupTemp, ILogicColumnName* column, int columnIndex, int dataType, int dataCount, int arrayIndex, int arrayCount);
	VOID				SetPropertyValue(TableProperties* tp, ILogicMemoryLine* line, ILogicColumnName* column, int columnIndex, int dataType, int dataCount, int arrayIndex, int arrayCount);
	VOID				GetPropertyValue(TablePropertyPtr* tpp, TableProperties* tp, ILogicMemoryLine* line, ILogicColumnName* column, int columnIndex, int dataType, int dataCount, int arrayIndex, int arrayCount);
	//
	VOID				BuildECode();
	VOID				BuildCppCode();
public:
	ILogicDataSystem*	m_logicPtr;
};

