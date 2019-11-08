
// phantom-gcView.h : CPhantomGCView 类的接口
//

#pragma once
#include "GCMFCHelper.h"

#define	CmdDefine(name)		afx_msg void OnClick##name();\
	afx_msg void OnUpdate##name(CCmdUI *pCmdUI);

#define	CmdMsg(name)	ON_COMMAND(name, &CPhantomGCView::OnClick##name)\
	ON_UPDATE_COMMAND_UI(name, &CPhantomGCView::OnUpdate##name)\

#define	CmdClick(name)		void	CPhantomGCView::OnClick##name()
#define	CmdUpdate(name)		void	CPhantomGCView::OnUpdate##name(CCmdUI *pCmdUI)

class CPhantomGCView : public CView
{
protected: // 仅从序列化创建
	CPhantomGCView();
	DECLARE_DYNCREATE(CPhantomGCView)

// 特性
public:
	CPhantomGCDoc* GetDocument() const;
	BOOL				m_bIsPopup;
	ViewerWindow*		m_wndViewer;
// 操作
public:
	VOID	ChangeViewScroll();
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CPhantomGCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	//
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnActivateFrame(UINT nState, CFrameWnd* pDeactivateFrame);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMeshShowAll(CCmdUI *pCmdUI);
	afx_msg void OnMeshShowAll();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditUiMode(CCmdUI *pCmdUI);
	afx_msg void OnEditUiMode();
	afx_msg void OnFileNew();
	afx_msg void OnShowUiGrid();
	afx_msg void OnUpdateShowUiGrid(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUiGridEditor(CCmdUI *pCmdUI);
	afx_msg void OnUiGridEditor();
	afx_msg void OnUiUnitEditorMode();
	afx_msg void OnUpdateUiUnitEditorMode(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUiInsertNull(CCmdUI *pCmdUI);
	afx_msg void OnUiInsertNull();
	afx_msg void OnBgColor();
	afx_msg void OnUpdateSceneGridSet(CCmdUI *pCmdUI);
	afx_msg void OnSceneGridSet();
	afx_msg void OnUpdateSceneUnitEditor(CCmdUI *pCmdUI);
	afx_msg void OnSceneUnitEditor();
	//
	afx_msg void OnUpdateInsertFormatstatic(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertImeedit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertFormatedit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertCheckbox(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertCombobox(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertSlider(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertListbox(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertScrollbar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertProgress(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertListctrl(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertButton(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInsertStatic(CCmdUI *pCmdUI);
	//
	afx_msg void OnInsertFormatstatic();
	afx_msg void OnInsertImeedit();
	afx_msg void OnInsertFormatedit();
	afx_msg void OnInsertCheckbox();
	afx_msg void OnInsertCombobox();
	afx_msg void OnInsertSlider();
	afx_msg void OnInsertListbox();
	afx_msg void OnInsertScrollbar();
	afx_msg void OnInsertProgress();
	afx_msg void OnInsertListctrl();
	afx_msg void OnInsertButton();
	afx_msg void OnInsertStatic();
	afx_msg void OnUpdateTabSet(CCmdUI *pCmdUI);
	afx_msg void OnTabSet();
	afx_msg void OnOperatorNone();
	afx_msg void OnUpdateOperatorNone(CCmdUI *pCmdUI);
	afx_msg void OnSetFloorEnable();
	afx_msg void OnUpdateSetFloorEnable(CCmdUI *pCmdUI);
	afx_msg void OnSetFloorDisable();
	afx_msg void OnUpdateSetFloorDisable(CCmdUI *pCmdUI);
	afx_msg void OnDeleteView();
	afx_msg void OnUpdateDeleteView(CCmdUI *pCmdUI);
	//
	afx_msg void OnAlignLeft();
	afx_msg void OnUpdateAlignLeft(CCmdUI *pCmdUI);
	afx_msg void OnAlignRight();
	afx_msg void OnUpdateAlignRight(CCmdUI *pCmdUI);
	afx_msg void OnAlignTop();
	afx_msg void OnUpdateAlignTop(CCmdUI *pCmdUI);
	afx_msg void OnAlignBottom();
	afx_msg void OnUpdateAlignBottom(CCmdUI *pCmdUI);
	afx_msg void OnAlignWidth();
	afx_msg void OnUpdateAlignWidth(CCmdUI *pCmdUI);
	afx_msg void OnAlignHeight();
	afx_msg void OnUpdateAlignHeight(CCmdUI *pCmdUI);
	afx_msg void OnAlignSpaceX();
	afx_msg void OnUpdateAlignSpaceX(CCmdUI *pCmdUI);
	afx_msg void OnAlignSpaceY();
	afx_msg void OnUpdateAlignSpaceY(CCmdUI *pCmdUI);
	afx_msg void OnShowAlign();
	afx_msg void OnUpdateShowAlign(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBgColor(CCmdUI *pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPropertyProp();
	afx_msg void OnUpdatePropertyProp(CCmdUI *pCmdUI);
	afx_msg void OnPropertyOutput();
	afx_msg void OnUpdatePropertyOutput(CCmdUI *pCmdUI);
	afx_msg void OnPropertyView();
	afx_msg void OnUpdatePropertyView(CCmdUI *pCmdUI);
	afx_msg void OnPeropertyWindow();
	afx_msg void OnUpdatePeropertyWindow(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGameRun(CCmdUI *pCmdUI);
	afx_msg void OnGameRun();
	afx_msg void OnUpdateGamePause(CCmdUI *pCmdUI);
	afx_msg void OnGamePause();
	afx_msg void OnUpdateGameStop(CCmdUI *pCmdUI);
	afx_msg void OnGameStop();
	//
	afx_msg void OnHeightAdd		();
	afx_msg void OnHeightAvg		();
	afx_msg void OnHeightAlign		();
	afx_msg void OnHeightSet		();
	afx_msg void OnHeightGridVisible();
	afx_msg void OnHeightGridEnable	();
	afx_msg void OnUpdateHeightAdd			(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHeightAvg			(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHeightAlign		(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHeightSet			(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHeightGridVisible	(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHeightGridEnable	(CCmdUI *pCmdUI);
	//
	afx_msg void OnRegionModeEnable	();
	afx_msg void OnUpdateRegionModeEnable	(CCmdUI *pCmdUI);
	//
	afx_msg void OnSceneWireColor();
	afx_msg void OnUpdateSceneWireColor(CCmdUI *pCmdUI);
	//
	afx_msg void OnTextureAdd		();
	afx_msg void OnTextureAvg		();
	afx_msg void OnTextureSet		();
	afx_msg void OnTextureUVMoving	();
	afx_msg void OnTextureUVScaling	();
	afx_msg void OnTextureUVRotation();
	afx_msg void OnUpdateTextureAdd			(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTextureAvg			(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTextureSet			(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTextureUVMoving	(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTextureUVScaling	(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTextureUVRotation	(CCmdUI *pCmdUI);
	//
	afx_msg void OnObjectMove();
	afx_msg void OnObjectScale();
	afx_msg void OnObjectRotation();
	afx_msg void OnUpdateObjectMove(CCmdUI *pCmdUI);
	afx_msg void OnUpdateObjectScale(CCmdUI *pCmdUI);
	afx_msg void OnUpdateObjectRotation(CCmdUI *pCmdUI);
	afx_msg void OnAddGrass();
	afx_msg void OnRemoveGrass();
	afx_msg void OnUpdateAddGrass(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRemoveGrass(CCmdUI *pCmdUI);
	//afx_msg void OnLineEditorMode();
	//afx_msg void OnUpdateLineEditorMode(CCmdUI *pCmdUI);
	//
	afx_msg void OnUnit3DPlay();
	afx_msg void OnUnit3DPause();
	afx_msg void OnUnit3DStop();
	afx_msg void OnUnit3DLoop();
	afx_msg void OnUpdateUnit3DPlay(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUnit3DPause(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUnit3DStop(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUnit3DLoop(CCmdUI *pCmdUI);
	//
	afx_msg void OnShowFPS();
	afx_msg void OnUpdateShowFPS(CCmdUI *pCmdUI);
	//
	CmdDefine(ID_RENDER_POINT)
	CmdDefine(ID_RENDER_LINE)
	CmdDefine(ID_RENDER_TRIANGLE)
	CmdDefine(ID_RENDER_REGION_BOX)
	CmdDefine(ID_RENDER_TERRAIN_BOX)
	CmdDefine(ID_RENDER_UNIT_BOX)
	CmdDefine(ID_SHOW_UNIT_OBJECT)
	//CmdDefine(ID_SHOW_MOVE_DISABLE)
	CmdDefine(ID_SHOW_UNIT_FILE)
	CmdDefine(ID_SHOW_TERRAIN)
	CmdDefine(ID_SHOW_WATER)
	CmdDefine(ID_SHOW_SKY)
	CmdDefine(ID_SHOW_GRASS)
	CmdDefine(ID_WATER_MIRROR)
	CmdDefine(ID_MIRROR_TERRAIN)
	CmdDefine(ID_MIRROR_SKY)
	CmdDefine(ID_MIRROR_UNITs)
	CmdDefine(ID_MIRROR_ALPHAUNIT)
	CmdDefine(ID_MIRROR_GRASS)
	CmdDefine(ID_SCREEN_EFFECT_CONFIG)
	CmdDefine(ID_SHADOW_CONFIG)
	CmdDefine(ID_SMALLMAP_CONFIG)
	CmdDefine(ID_RENDER_CONFIG)
	//
	CmdDefine(ID_SCENE_VIEWTARGET)
	CmdDefine(ID_LINE_EDITOR_MODE)
	//
	CmdDefine(IDC_EXPORT_UI)
	CmdDefine(IDC_EXPORT_CURSOR)
	CmdDefine(IDC_EXPORT_2DUNIT)
	CmdDefine(IDC_EXPORT_MESH)
	CmdDefine(IDC_EXPORT_3DUNIT)
	CmdDefine(IDC_EXPORT_LOGIC)
	CmdDefine(IDC_EXPORT_PROJ)
	CmdDefine(ID_REPORT_FILE_LIST)
	CmdDefine(IDC_EXPORT_2DSCENE)
	CmdDefine(IDC_EXPORT_3DSCENE)
	//
	CmdDefine(ID_LANGUAGE_CHINESS)
	CmdDefine(ID_LANGUAGE_ENGLISH)
	CmdDefine(ID_LANGUAGE_RUSSIAN)
	CmdDefine(ID_LANGUAGE_KOREAN)
	CmdDefine(ID_LANGUAGE_JAPANESS)
	CmdDefine(ID_LANGUAGE_CHINESS_T)
	//
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnExportFull();
	afx_msg void OnUpdateShowCharacters(CCmdUI *pCmdUI);
	afx_msg void OnShowCharacters();
	afx_msg void OnUpdateMenucenter(CCmdUI *pCmdUI);
	afx_msg void OnMenucenter();
	afx_msg void OnUpdateBuildECode(CCmdUI *pCmdUI);
	afx_msg void OnBuildECode();
	afx_msg void OnUpdateShowCollision(CCmdUI *pCmdUI);
	afx_msg void OnShowCollision();
	afx_msg void OnUpdateGameSize(CCmdUI *pCmdUI);
	afx_msg void OnGameSize();
	afx_msg void OnUpdateSoftShadowMode(CCmdUI *pCmdUI);
	afx_msg void OnSoftShadowMode();
	afx_msg void OnUpdateUiPngMaker(CCmdUI *pCmdUI);
	afx_msg void OnUiPngMaker();
	afx_msg void OnUpdateShowuiMapgrid(CCmdUI *pCmdUI);
	afx_msg void OnShowuiMapgrid();
	afx_msg void OnUpdateLevelsLevel1(CCmdUI *pCmdUI);
	afx_msg void OnLevelsLevel1();
	afx_msg void OnUpdateLevelsLevel2(CCmdUI *pCmdUI);
	afx_msg void OnLevelsLevel2();
	afx_msg void OnUpdateLevelsLevel3(CCmdUI *pCmdUI);
	afx_msg void OnLevelsLevel3();
	afx_msg void OnUpdateLevelsLevel4(CCmdUI *pCmdUI);
	afx_msg void OnLevelsLevel4();
	afx_msg void OnUpdateLevelsLevel5(CCmdUI *pCmdUI);
	afx_msg void OnLevelsLevel5();
	afx_msg void OnUpdateLevelsLevel6(CCmdUI *pCmdUI);
	afx_msg void OnLevelsLevel6();
	afx_msg void OnUpdateUimapMovedisable(CCmdUI *pCmdUI);
	afx_msg void OnUimapMovedisable();
	afx_msg void OnUpdateUimapMovedisablemode(CCmdUI *pCmdUI);
	afx_msg void OnUimapMovedisablemode();
	afx_msg void OnUpdateShowCenter(CCmdUI *pCmdUI);
	afx_msg void OnShowCenter();
	afx_msg void OnUpdateSetLevelMode(CCmdUI *pCmdUI);
	afx_msg void OnSetLevelMode();
	afx_msg void OnUpdateSelImageMode(CCmdUI *pCmdUI);
	afx_msg void OnSelImageMode();
	afx_msg void OnUpdateBuildCppCode(CCmdUI *pCmdUI);
	afx_msg void OnBuildCppCode();
	afx_msg void OnUpdateInsertEffect2d(CCmdUI *pCmdUI);
	afx_msg void OnInsertEffect2d();
	afx_msg void OnBox2dUsage();
	afx_msg void OnUpdateBox2dUsage(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEncryptFile(CCmdUI *pCmdUI);
	afx_msg void OnEncryptFile();
	afx_msg void OnShowMycode();
	afx_msg void OnUpdateShowMycode(CCmdUI *pCmdUI);
	afx_msg void OnSetGridColor();
	afx_msg void OnUpdateSetGridColor(CCmdUI *pCmdUI);
	afx_msg void OnConvertAllTopng();
	afx_msg void OnUpdateConvertAllTopng(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGenCppCode(CCmdUI *pCmdUI);
	afx_msg void OnGenCppCode();
	afx_msg void OnUpdateGenericcodesGenericc(CCmdUI *pCmdUI);
	afx_msg void OnGenericcodesGenericc();
	afx_msg void OnUpdateGenericcodesGenerichtmlreaderjsfiles(CCmdUI *pCmdUI);
	afx_msg void OnGenericcodesGenerichtmlreaderjsfiles();
	afx_msg void OnUpdateGenericcodesGenericeinitdialogcode(CCmdUI *pCmdUI);
	afx_msg void OnGenericcodesGenericeinitdialogcode();
	afx_msg void OnMeshToScene();
	afx_msg void OnUpdateMeshToScene(CCmdUI *pCmdUI);
	afx_msg void OnBrushCircle();
	afx_msg void OnUpdateBrushCircle(CCmdUI *pCmdUI);
	afx_msg void OnBrushBox();
	afx_msg void OnUpdateBrushBox(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAutomoveMode(CCmdUI *pCmdUI);
	afx_msg void OnAutomoveMode();
	afx_msg void OnExportUi();
	afx_msg void OnUpdateExportUi(CCmdUI *pCmdUI);
	afx_msg void OnUserInfo();
	afx_msg void OnUpdateShowObjectsDlg(CCmdUI *pCmdUI);
	afx_msg void OnShowObjectsDlg();
	afx_msg void OnUpdateShowScript(CCmdUI *pCmdUI);
	afx_msg void OnShowScript();
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnColorSet();
	afx_msg void OnReloadTables();
	afx_msg void OnUpdateShowBox(CCmdUI *pCmdUI);
	afx_msg void OnShowBox();
	afx_msg void OnUpdateRegionSet(CCmdUI *pCmdUI);
	afx_msg void OnRegionSet();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateBuyResource(CCmdUI *pCmdUI);
	afx_msg void OnBuyResource();
	afx_msg void OnUpdateExportPak(CCmdUI *pCmdUI);
	afx_msg void OnExportPak();
	afx_msg void OnConvertAllFbx();
};


#ifndef _DEBUG  // phantom-gcView.cpp 中的调试版本
inline CPhantomGCDoc* CPhantomGCView::GetDocument() const
   { return reinterpret_cast<CPhantomGCDoc*>(m_pDocument); }
#endif

