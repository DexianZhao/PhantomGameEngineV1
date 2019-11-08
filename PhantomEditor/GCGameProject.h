
#pragma once
#include "GCViewBase.h"
#include "TrackAnimationCtrl.h"
#include "GamePlotViewer.h"
#include "GameDirector.h"
#include "PlotCallViewer.h"
//#include "LuaScriptEditor.h"
#include "PlotVariantDialog.h"

typedef	VOID	(*PropChapter)(CChapter* data, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*PropAction)(CPlotAction* data, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*PropDrama)(CDrama* data, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*PropState)(CPlotState* data, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*PropEvent)(CPlotPropertiesMap* map, CPlotEventCall* data, LPARAM nParameter, LPARAM nParameter2);

typedef	VOID	(*PropVariant)(CPlotVariant* var, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*PropVariantSet)(CCallVariantSet* state, LPARAM nParameter, LPARAM nParameter2);
typedef	VOID	(*PropPropertyCall)(CCallPropertySet* state, LPARAM nParameter, LPARAM nParameter2);

class GCGameProject: public GCViewBase
{
public:
	GCGameProject(CPhantomGCDoc* docPtr);
	~GCGameProject(void);
	virtual	VOID				OnActive();		//被激活
	virtual	VOID				OnDeActive();	//失去激活
	virtual	VOID				CloseFile();
	virtual	BOOL				OpenFile(const wchar_t* szFile);	//打开文件,如果失败则会自动删除
	virtual	BOOL				SaveFile(BOOL bSaveAs);	//保存文件
	virtual void				OnFrameMove(const RenderParameter& r);	//每次更新
	virtual void				OnPushRenderObjects(const RenderParameter& r);	//渲染
	virtual void				OnRenderingEnd(const RenderParameter& r);	//渲染
	virtual	bool				OnWindowProc(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam);
	virtual void				OnRendering(const RenderParameter& r);	//渲染
	virtual	void				OnRenderEndUI(const RenderParameter& r);	//界面渲染之后
	//
	virtual	FileViewerType		GetViewerType(){return FileViewerType_GameProject;}
	//virtual	IBigScene3D*		GetStateScene();
	virtual	HICON				GetViewerIcon();
	virtual	INT					GetPopupMenuID(){return 12;}
	VOID						LoopAllChapters(PropChapter proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE);
	VOID						LoopAllDramas(PropDrama proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE);
	VOID						LoopAllActions(PropAction proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE);
	VOID						LoopAllStates(PropState proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE);
	VOID						LoopAllEvents(PropEvent proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE);
	//
	VOID						LoopAllVariant(PropVariant proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE);
	//
	VOID						LoopAllVariantSet(PropVariantSet proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE);
	VOID						LoopAllPropertyCall(PropPropertyCall proc, LPARAM nParameter = 0, LPARAM nParameter2 = 0, BOOL bRename = FALSE);
	//
	BOOL						ImportFile(const char* szFile);	//ImportFileType type
	virtual	VOID				BuildFileViewText();
	VOID						RebuildTrees();
	enum{
		GAMEPROJ_NONE	=	0,
		GAMEPROJ_MAIN,
		GAMEPROJ_FILE,
		GAMEPROJ_FOLDER,
		GAMEPROJ_CHAPTER_LIST,
		//
		GAMEPROJ_ACTION_LIST,
		GAMEPROJ_EVENT_LIST,
		GAMEPROJ_ELEMENT_LIST,
		GAMEPROJ_STATE_LIST,
		//
		GAMEPROJ_CHAPTER,
		GAMEPROJ_MODULE,
		GAMEPROJ_DRAMA,
		GAMEPROJ_ELEMENT,
		GAMEPROJ_OBJECT,
		GAMEPROJ_ACTION,
		GAMEPROJ_EVENT,	//也有内部State
		GAMEPROJ_STATE,
		//GAMEPROJ_EVENT_STATE,
	};
	virtual	BOOL				IsCopyTree();
	virtual	BOOL				IsPasteTree();
	HTREEITEM					m_hViewChapters;
	VOID						GetNewFileName(wchar_t* szFile);
	void						OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp);
	VOID						OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem);	//更新选择树形控件
	virtual	void				OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI);
	virtual	void				OnCommandUI(INT nMenuID);
	void						OnTreeViewKeyDown(INT nChar);
	//
	VOID						SetGameRuntime(BOOL bRuntime);
	BOOL						IsGameRuntime();
	//
	VOID						EnumAllEvents(CDynamicArray<PlotEventName>& rets);
	//
	VOID						SetGameRename(const wchar_t* szName);
	//
	VOID						RebuildSubDir(const wchar_t* szDir, HTREEITEM hItem);
	//
	VOID						RefreshChapters();
	VOID						RefreshChapter(CChapter* chapter, HTREEITEM hItem);
	VOID						RefreshDrama(CDrama* drama, HTREEITEM hItem);
	VOID						RefreshActions(CPlotProcess* process, HTREEITEM hItem, BOOL bDeleteChilds = TRUE);
	VOID						RefreshProcess(CPlotProcess* process, HTREEITEM hItem);
	//
	VOID						RefreshElements(CPlotProcess* process, HTREEITEM hItem);
	VOID						RefreshElement(CPlotElementBase* process, HTREEITEM hItem);
	VOID						RefreshEvents(CPlotPropertiesMap* process, HTREEITEM hItem, BOOL bDeleteChilds = TRUE);
	VOID						RefreshStates(CPlotAction* action, HTREEITEM hItem, BOOL bDeleteChilds = TRUE);
	//VOID						RefreshEvent(CPlotPropertiesMap* process, HTREEITEM hItem, INT nEventID);
	//
	VOID						RefreshAction(CPlotAction* action, HTREEITEM hItem);
	//
	VOID						BuildAll();
	//
	HTREEITEM					AddFileItem(const wchar_t* szFileName, HTREEITEM hParent, const wchar_t* szDir, BOOL bIsFile);
	//
	void						OnDoubleClick(CMTreeCtrl* treePtr, HTREEITEM hItem);	//更新选择树形控件
	virtual	VOID				OnCopyTree();
	virtual	VOID				OnPasteTree();
	virtual	VOID				OnDeleteTree();
	virtual	BOOL				IsDeleteTree();
	//
	VOID						InitGameProp();
	GCPropertyBase				m_gameProp;
	PropertyPtr					m_gameName;
	PropertyPtr					m_gameScreenW;
	PropertyPtr					m_gameScreenH;
	PropertyPtr					m_gameCreatorName;
	PropertyPtr					m_gameVersion;
	PropertyPtr					m_gameCopyright;
	//
	FileNameW					m_gamePath;
	CProject					m_gameDirector;
	CPlotCallViewer				m_callPlot;
	//CLuaScriptEditor			m_luaScript;
	CPlotVariantDialog			m_dlgVariant;
	//
	GCPropertyBase				m_chapterProp;
	PropertyPtr					m_chapterName;
	GCPropertyBase				m_dramaProp;
	PropertyPtr					m_dramaName;
	GCPropertyBase				m_actionProp;
	PropertyPtr					m_actionName;
	//
	GCPropertyBase				m_stateProp;
	PropertyPtr					m_stateName;
	PropertyPtr					m_stateIndex;
	PropertyPtr					m_stateElapsedTime;
	PropertyPtr					m_stateScriptParam[MAX_SCRIPT_PARAMETER];
	//
	GCPropertyBase				m_eventProp;
	PropertyPtr					m_eventName;
	PropertyPtr					m_eventIDList[MAX_EVENT_PARAM_COUNT];
	PropertyPtr					m_eventScriptParam[MAX_SCRIPT_PARAMETER];
	//
	VOID						InitCallVariantSet();
	VOID						InitCallProperties();
	VOID						InitVariants();
	//
	VOID						InitChapterProp();
	VOID						InitDramaProp();
	VOID						InitActionProp();
	VOID						InitStateProp();
	VOID						InitEventProp();
	//
	GCPropertyBase				m_callVarWnd;
	PropertyPtr					m_callVarIDs;
	PropertyPtr					m_callVarOtherIDs;
	PropertyPtr					m_callVarInt;
	PropertyPtr					m_callVarDouble;
	PropertyPtr					m_callVarText;
	PropertyPtr					m_callVarF1;
	PropertyPtr					m_callVarF2;
	PropertyPtr					m_callVarF3;
	PropertyPtr					m_callVarF4;
	PropertyPtr					m_callVarI1;
	PropertyPtr					m_callVarI2;
	PropertyPtr					m_callVarI3;
	PropertyPtr					m_callVarI4;
	//
	GCPropertyBase				m_callPropWnd;
	PropertyPtr					m_callPropActions;
	PropertyPtr					m_callPropIDs;
	PropertyPtr					m_callPropParamGroup[MAX_EVENT_PARAM_COUNT];
	//
	PropertyPtr					m_callPropParamInt[MAX_EVENT_PARAM_COUNT];
	PropertyColorPtr			m_callPropParamColor[MAX_EVENT_PARAM_COUNT];
	PropertyFloatPtr			m_callPropParamDouble[MAX_EVENT_PARAM_COUNT];
	PropertyPtr					m_callPropParamText[MAX_EVENT_PARAM_COUNT];
	PropertyPtr					m_callPropParamList[MAX_EVENT_PARAM_COUNT];
	//
	PropertyPtr					m_callPropParamOtherIDs[MAX_EVENT_PARAM_COUNT];
	//
	GCPropertyBase				m_variantProp;
	PropertyPtr					m_variantName;
	PropertyPtr					m_variantInt;
	PropertyPtr					m_variantDouble;
	PropertyPtr					m_variantText;
	PropertyPtr					m_variantF1;
	PropertyPtr					m_variantF2;
	PropertyPtr					m_variantF3;
	PropertyPtr					m_variantF4;
	PropertyPtr					m_variantI1;
	PropertyPtr					m_variantI2;
	PropertyPtr					m_variantI3;
	PropertyPtr					m_variantI4;
	//
	VOID						ChangeGameSize();
	//
	VOID						SetVariantOption(PropertyPtr variant, CPlotState* c, int variantID);
	VOID						SetPropertyOption(CCallPropertySet* prop);
	//
	VOID						InitScriptParameter(PropertyPtr params, CPlotAction* act, int id, wchar_t* ret);
	VOID						InitScriptParameter(PropertyPtr* params, CPlotState* c);
	VOID						SetScriptParameter(ScriptParameter& ret, CPlotState* act, const wchar_t* szText);
	BOOL						SetScriptParameter2(ScriptParameter& ret, CPlotAction* act, const wchar_t* szText);
	//
	VOID						SetPropertyNull();
	VOID						SetPropertyGame();
	VOID						SetPropertyAction(CPlotAction* act);
	VOID						SetPropertyCall(CPlotState* c);
	//
	VOID						SetPropertyVariant(CPlotAction* act, CPlotVariant* var);
	//
	VOID						SetPropertyCallVarSet(CPlotState* c, CCallVariantSet* var);
	VOID						SetPropertyCallPropCall(CPlotState* c, CCallPropertySet* var);
	VOID						SetPropertyCallLuaScript(CPlotState* c, CCallLuaScript* var);
	//
	virtual	VOID		PlayAnimation();
	virtual	VOID		StopAnimation();
	virtual	VOID		PauseAnimation(BOOL bPause);
	virtual	VOID		SetAnimationLoop(BOOL bLoop);
	virtual	BOOL		IsAnimationCanPlayStop();
	virtual	BOOL		IsAnimationCanPause();
	virtual	BOOL		IsAnimationCanLoop();
	virtual	BOOL		IsAnimationPlaying();
	virtual	BOOL		IsAnimationPause();
	virtual	BOOL		IsAnimationLoop();
	//
	VOID						SetSelectNone();
	//
	CPlotVariant*				m_selectVar;
	CPlotState*					m_selectState;
	CCallVariantSet*			m_selectVarSet;
	CCallPropertySet*			m_selectPropSet;
	CCallLuaScript*				m_selectScript;
	CPlotAction*				m_selectAction;
public:
	CProjectRun*				m_projectRun;
	//IGameProject*				m_gameProject;
	BOOL						m_bLButtonDown;
	CPoint						m_downPoint;
	CPoint						m_downOffset;
};

const wchar_t*		GetPlotName(CPlotPropertiesMap* ch, wchar_t* wbuf = 0);
const wchar_t*		GetPlotName(PlotType type, CPlotSystemBase* ch, wchar_t* wbufRet);
