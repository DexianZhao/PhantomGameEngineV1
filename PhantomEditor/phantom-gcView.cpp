
// phantom-gcView.cpp : CPhantomGCView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "phantom-gc.h"
#endif

#include "MultiExportDlg.h"
#include "phantom-gcDoc.h"
#include "phantom-gcView.h"
#include "GCRenderSystem.h"
#include "GCBitmapMgr.h"
#include "GCMFCHelper.h"
#include "GCViewManager.h"
#include "MainFrm.h"
#include "GCViewUIEditor.h"
#include "AlignDialog.h"
#include "GCSceneEditor3D.h"
#include "GCViewGeometryEditor.h"
#include "GameScreenSize.h"
#include "UIPngMaker.h"
#include "../PhantomEngine/encrypt.h"
#include "PhantomManager.h"
#include "DlgCodeColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPhantomGCView

IMPLEMENT_DYNCREATE(CPhantomGCView, CView)

BEGIN_MESSAGE_MAP(CPhantomGCView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPhantomGCView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_SAVE, &CPhantomGCView::OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, &CPhantomGCView::OnFileSaveAs)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPEN, &CPhantomGCView::OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CPhantomGCView::OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_MESH_SHOW_ALL, &CPhantomGCView::OnUpdateMeshShowAll)
	ON_COMMAND(ID_MESH_SHOW_ALL, &CPhantomGCView::OnMeshShowAll)
	ON_COMMAND(ID_EDIT_COPY, &CPhantomGCView::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CPhantomGCView::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CPhantomGCView::OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CPhantomGCView::OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(IDM_EDIT_UI_MODE, &CPhantomGCView::OnUpdateEditUiMode)
	ON_COMMAND(IDM_EDIT_UI_MODE, &CPhantomGCView::OnEditUiMode)
	ON_COMMAND(ID_FILE_NEW, &CPhantomGCView::OnFileNew)
	ON_COMMAND(ID_SHOW_UI_GRID, &CPhantomGCView::OnShowUiGrid)
	ON_UPDATE_COMMAND_UI(ID_SHOW_UI_GRID, &CPhantomGCView::OnUpdateShowUiGrid)
	ON_UPDATE_COMMAND_UI(ID_UI_GRID_EDITOR, &CPhantomGCView::OnUpdateUiGridEditor)
	ON_COMMAND(ID_UI_GRID_EDITOR, &CPhantomGCView::OnUiGridEditor)
	ON_COMMAND(ID_UI_UNIT_EDITOR_MODE, &CPhantomGCView::OnUiUnitEditorMode)
	ON_UPDATE_COMMAND_UI(ID_UI_UNIT_EDITOR_MODE, &CPhantomGCView::OnUpdateUiUnitEditorMode)
	ON_UPDATE_COMMAND_UI(IDM_UI_INSERT_NULL, &CPhantomGCView::OnUpdateUiInsertNull)
	ON_COMMAND(IDM_UI_INSERT_NULL, &CPhantomGCView::OnUiInsertNull)
	ON_COMMAND(IDM_BG_COLOR, &CPhantomGCView::OnBgColor)
	ON_UPDATE_COMMAND_UI(IDM_SCENE_GRID_SET, &CPhantomGCView::OnUpdateSceneGridSet)
	ON_COMMAND(IDM_SCENE_GRID_SET, &CPhantomGCView::OnSceneGridSet)
	ON_UPDATE_COMMAND_UI(IDM_SCENE_UNIT_EDITOR, &CPhantomGCView::OnUpdateSceneUnitEditor)
	ON_COMMAND(IDM_SCENE_UNIT_EDITOR, &CPhantomGCView::OnSceneUnitEditor)
	//
	ON_UPDATE_COMMAND_UI(IDM_INSERT_STATIC, &CPhantomGCView::OnUpdateInsertStatic)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_FORMATSTATIC, &CPhantomGCView::OnUpdateInsertFormatstatic)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_IMEEDIT, &CPhantomGCView::OnUpdateInsertImeedit)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_FORMATEDIT, &CPhantomGCView::OnUpdateInsertFormatedit)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_CHECKBOX, &CPhantomGCView::OnUpdateInsertCheckbox)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_COMBOBOX, &CPhantomGCView::OnUpdateInsertCombobox)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_SLIDER, &CPhantomGCView::OnUpdateInsertSlider)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_LISTBOX, &CPhantomGCView::OnUpdateInsertListbox)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_SCROLLBAR, &CPhantomGCView::OnUpdateInsertScrollbar)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_PROGRESS, &CPhantomGCView::OnUpdateInsertProgress)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_LISTCTRL, &CPhantomGCView::OnUpdateInsertListctrl)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_BUTTON, &CPhantomGCView::OnUpdateInsertButton)
	//
	ON_COMMAND(IDM_INSERT_FORMATSTATIC, &CPhantomGCView::OnInsertFormatstatic)
	ON_COMMAND(IDM_INSERT_IMEEDIT, &CPhantomGCView::OnInsertImeedit)
	ON_COMMAND(IDM_INSERT_FORMATEDIT, &CPhantomGCView::OnInsertFormatedit)
	ON_COMMAND(IDM_INSERT_CHECKBOX, &CPhantomGCView::OnInsertCheckbox)
	ON_COMMAND(IDM_INSERT_COMBOBOX, &CPhantomGCView::OnInsertCombobox)
	ON_COMMAND(IDM_INSERT_SLIDER, &CPhantomGCView::OnInsertSlider)
	ON_COMMAND(IDM_INSERT_LISTBOX, &CPhantomGCView::OnInsertListbox)
	ON_COMMAND(IDM_INSERT_SCROLLBAR, &CPhantomGCView::OnInsertScrollbar)
	ON_COMMAND(IDM_INSERT_PROGRESS, &CPhantomGCView::OnInsertProgress)
	ON_COMMAND(IDM_INSERT_LISTCTRL, &CPhantomGCView::OnInsertListctrl)
	ON_COMMAND(IDM_INSERT_BUTTON, &CPhantomGCView::OnInsertButton)
	ON_COMMAND(IDM_INSERT_STATIC, &CPhantomGCView::OnInsertStatic)
	ON_UPDATE_COMMAND_UI(ID_TAB_SET, &CPhantomGCView::OnUpdateTabSet)
	ON_COMMAND(ID_TAB_SET, &CPhantomGCView::OnTabSet)
	ON_COMMAND(ID_OPERATOR_NONE, &CPhantomGCView::OnOperatorNone)
	ON_UPDATE_COMMAND_UI(ID_OPERATOR_NONE, &CPhantomGCView::OnUpdateOperatorNone)
	ON_COMMAND(ID_SET_FLOOR_ENABLE, &CPhantomGCView::OnSetFloorEnable)
	ON_UPDATE_COMMAND_UI(ID_SET_FLOOR_ENABLE, &CPhantomGCView::OnUpdateSetFloorEnable)
	ON_COMMAND(ID_SET_FLOOR_DISABLE, &CPhantomGCView::OnSetFloorDisable)
	ON_UPDATE_COMMAND_UI(ID_SET_FLOOR_DISABLE, &CPhantomGCView::OnUpdateSetFloorDisable)
	ON_COMMAND(ID_DELETE_VIEW, &CPhantomGCView::OnDeleteView)
	ON_UPDATE_COMMAND_UI(ID_DELETE_VIEW, &CPhantomGCView::OnUpdateDeleteView)
	ON_COMMAND(ID_ALIGN_LEFT, &CPhantomGCView::OnAlignLeft)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_LEFT, &CPhantomGCView::OnUpdateAlignLeft)
	ON_COMMAND(ID_SHOW_ALIGN, &CPhantomGCView::OnShowAlign)
	ON_UPDATE_COMMAND_UI(ID_SHOW_ALIGN, &CPhantomGCView::OnUpdateShowAlign)
	//
	ON_COMMAND(ID_ALIGN_RIGHT, &CPhantomGCView::OnAlignRight)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_RIGHT, &CPhantomGCView::OnUpdateAlignRight)
	ON_COMMAND(ID_ALIGN_TOP, &CPhantomGCView::OnAlignTop)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_TOP, &CPhantomGCView::OnUpdateAlignTop)
	ON_COMMAND(ID_ALIGN_BOTTOM, &CPhantomGCView::OnAlignBottom)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_BOTTOM, &CPhantomGCView::OnUpdateAlignBottom)
	ON_COMMAND(ID_ALIGN_WIDTH, &CPhantomGCView::OnAlignWidth)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_WIDTH, &CPhantomGCView::OnUpdateAlignWidth)
	ON_COMMAND(ID_ALIGN_HEIGHT, &CPhantomGCView::OnAlignHeight)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_HEIGHT, &CPhantomGCView::OnUpdateAlignHeight)
	ON_COMMAND(ID_SPACE_X, &CPhantomGCView::OnAlignSpaceX)
	ON_UPDATE_COMMAND_UI(ID_SPACE_X, &CPhantomGCView::OnUpdateAlignSpaceX)
	ON_COMMAND(ID_SPACE_Y, &CPhantomGCView::OnAlignSpaceY)
	ON_UPDATE_COMMAND_UI(ID_SPACE_Y, &CPhantomGCView::OnUpdateAlignSpaceY)
	ON_UPDATE_COMMAND_UI(IDM_BG_COLOR, &CPhantomGCView::OnUpdateBgColor)
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//
	ON_COMMAND(ID_PROPERTY_PROP, &CPhantomGCView::OnPropertyProp)
	ON_UPDATE_COMMAND_UI(ID_PROPERTY_PROP, &CPhantomGCView::OnUpdatePropertyProp)
	ON_COMMAND(ID_PROPERTY_OUTPUT, &CPhantomGCView::OnPropertyOutput)
	ON_UPDATE_COMMAND_UI(ID_PROPERTY_OUTPUT, &CPhantomGCView::OnUpdatePropertyOutput)
	ON_COMMAND(ID_PROPERTY_VIEW, &CPhantomGCView::OnPropertyView)
	ON_UPDATE_COMMAND_UI(ID_PROPERTY_VIEW, &CPhantomGCView::OnUpdatePropertyView)
	ON_COMMAND(ID_PEROPERTY_WINDOW, &CPhantomGCView::OnPeropertyWindow)
	ON_UPDATE_COMMAND_UI(ID_PEROPERTY_WINDOW, &CPhantomGCView::OnUpdatePeropertyWindow)
	ON_UPDATE_COMMAND_UI(IDM_GAME_RUN, &CPhantomGCView::OnUpdateGameRun)
	ON_COMMAND(IDM_GAME_RUN, &CPhantomGCView::OnGameRun)
	ON_UPDATE_COMMAND_UI(IDM_GAME_PAUSE, &CPhantomGCView::OnUpdateGamePause)
	ON_COMMAND(IDM_GAME_PAUSE, &CPhantomGCView::OnGamePause)
	ON_UPDATE_COMMAND_UI(IDM_GAME_STOP, &CPhantomGCView::OnUpdateGameStop)
	ON_COMMAND(IDM_GAME_STOP, &CPhantomGCView::OnGameStop)
	//
	ON_COMMAND(ID_SCENE3D_HEIGHTADD,	&CPhantomGCView::OnHeightAdd		)
	ON_COMMAND(ID_SCENE3D_HEIGHTAVG,	&CPhantomGCView::OnHeightAvg		)
	ON_COMMAND(ID_SCENE3D_HEIGHTALIGN, &CPhantomGCView::OnHeightAlign		)
	ON_COMMAND(ID_SCENE3D_HEIGHTSET,	&CPhantomGCView::OnHeightSet		)
	ON_COMMAND(ID_SCENE3D_SHOWGRID,		&CPhantomGCView::OnHeightGridVisible)
	ON_COMMAND(ID_SCENE3D_MOVEENABLE,	&CPhantomGCView::OnHeightGridEnable	)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_HEIGHTADD,	&CPhantomGCView::OnUpdateHeightAdd			)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_HEIGHTAVG,	&CPhantomGCView::OnUpdateHeightAvg			)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_HEIGHTALIGN, &CPhantomGCView::OnUpdateHeightAlign		)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_HEIGHTSET,	&CPhantomGCView::OnUpdateHeightSet			)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_SHOWGRID,		&CPhantomGCView::OnUpdateHeightGridVisible	)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_MOVEENABLE,	&CPhantomGCView::OnUpdateHeightGridEnable	)
	//
	ON_COMMAND(ID_SCENE_WIRE_COLOR,	&CPhantomGCView::OnSceneWireColor	)
	ON_UPDATE_COMMAND_UI(ID_SCENE_WIRE_COLOR,	&CPhantomGCView::OnUpdateSceneWireColor			)
	//
	ON_COMMAND(IDC_SHOW_FPS,	&CPhantomGCView::OnShowFPS	)
	ON_UPDATE_COMMAND_UI(IDC_SHOW_FPS,	&CPhantomGCView::OnUpdateShowFPS			)
	//
	ON_COMMAND(ID_SCENE3D_TEXTUREADD,	&CPhantomGCView::OnTextureAdd		)
	ON_COMMAND(ID_SCENE3D_TEXTUREAVG,	&CPhantomGCView::OnTextureAvg		)
	ON_COMMAND(ID_SCENE3D_TEXTURESET,	&CPhantomGCView::OnTextureSet		)
	ON_COMMAND(ID_SCENE3D_TEXTUREMOVING,	&CPhantomGCView::OnTextureUVMoving	)
	ON_COMMAND(ID_SCENE3D_TEXTURESCALILNG,		&CPhantomGCView::OnTextureUVScaling	)
	ON_COMMAND(ID_SCENE3D_TEXTUREROTATION,	&CPhantomGCView::OnTextureUVRotation)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_TEXTUREADD,	&CPhantomGCView::OnUpdateTextureAdd			)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_TEXTUREAVG,	&CPhantomGCView::OnUpdateTextureAvg			)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_TEXTURESET,&CPhantomGCView::OnUpdateTextureSet			)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_TEXTUREMOVING,	&CPhantomGCView::OnUpdateTextureUVMoving	)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_TEXTURESCALILNG,	&CPhantomGCView::OnUpdateTextureUVScaling	)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_TEXTUREROTATION,	&CPhantomGCView::OnUpdateTextureUVRotation	)
	//
	ON_COMMAND(ID_SCENE3D_MOVEOBJECT,		&CPhantomGCView::OnObjectMove		)
	ON_COMMAND(ID_SCENE3D_SCALEOBJECT,		&CPhantomGCView::OnObjectScale		)
	ON_COMMAND(ID_SCENE3D_ROTATIONOBJECT,	&CPhantomGCView::OnObjectRotation	)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_MOVEOBJECT,		&CPhantomGCView::OnUpdateObjectMove		)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_SCALEOBJECT,	&CPhantomGCView::OnUpdateObjectScale	)
	ON_UPDATE_COMMAND_UI(ID_SCENE3D_ROTATIONOBJECT,	&CPhantomGCView::OnUpdateObjectRotation	)
	//
	ON_COMMAND(ID_GRASS_ADDMODE,			&CPhantomGCView::OnAddGrass		)
	ON_COMMAND(ID_GRASS_DELMODE,			&CPhantomGCView::OnRemoveGrass	)
	ON_UPDATE_COMMAND_UI(ID_GRASS_ADDMODE,	&CPhantomGCView::OnUpdateAddGrass		)
	ON_UPDATE_COMMAND_UI(ID_GRASS_DELMODE,	&CPhantomGCView::OnUpdateRemoveGrass	)
	//
	ON_COMMAND(ID_AUTO_PLAY_UNIT3D,				&CPhantomGCView::OnUnit3DPlay		)
	ON_COMMAND(ID_PAUSE_UNIT3D,					&CPhantomGCView::OnUnit3DPause	)
	ON_COMMAND(ID_STOP_UNIT3D,					&CPhantomGCView::OnUnit3DStop	)
	ON_UPDATE_COMMAND_UI(ID_AUTO_PLAY_UNIT3D,	&CPhantomGCView::OnUpdateUnit3DPlay		)
	ON_UPDATE_COMMAND_UI(ID_PAUSE_UNIT3D,		&CPhantomGCView::OnUpdateUnit3DPause		)
	ON_UPDATE_COMMAND_UI(ID_STOP_UNIT3D,		&CPhantomGCView::OnUpdateUnit3DStop	)
	ON_COMMAND(ID_LOOP_UNIT3D,					&CPhantomGCView::OnUnit3DLoop	)
	ON_UPDATE_COMMAND_UI(ID_LOOP_UNIT3D,		&CPhantomGCView::OnUpdateUnit3DLoop		)
	//
	CmdMsg(ID_RENDER_POINT)
	CmdMsg(ID_RENDER_LINE)
	CmdMsg(ID_RENDER_TRIANGLE)
	CmdMsg(ID_RENDER_REGION_BOX)
	//CmdMsg(ID_RENDER_TERRAIN_BOX)
	CmdMsg(ID_RENDER_UNIT_BOX)
	CmdMsg(ID_SHOW_UNIT_OBJECT)
	//CmdMsg(ID_SHOW_MOVE_DISABLE)
	//CmdMsg(ID_SHOW_UNIT_FILE)
	CmdMsg(ID_SHOW_TERRAIN)
	//CmdMsg(ID_SHOW_WATER)
	//CmdMsg(ID_SHOW_SKY)
	//CmdMsg(ID_SHOW_GRASS)
	//CmdMsg(ID_WATER_MIRROR)
	//CmdMsg(ID_MIRROR_TERRAIN)
	//CmdMsg(ID_MIRROR_SKY)
	//CmdMsg(ID_MIRROR_UNITs)
	//CmdMsg(ID_MIRROR_ALPHAUNIT)
	//CmdMsg(ID_MIRROR_GRASS)
	CmdMsg(ID_SCREEN_EFFECT_CONFIG)
	CmdMsg(ID_SHADOW_CONFIG)
	CmdMsg(ID_SMALLMAP_CONFIG)
	CmdMsg(ID_RENDER_CONFIG)
	CmdMsg(ID_SCENE_VIEWTARGET)
	CmdMsg(ID_LINE_EDITOR_MODE)
	//
	CmdMsg(IDC_EXPORT_UI)
	CmdMsg(IDC_EXPORT_CURSOR)
	CmdMsg(IDC_EXPORT_2DUNIT)
	CmdMsg(IDC_EXPORT_MESH)
	CmdMsg(IDC_EXPORT_3DUNIT)
	CmdMsg(IDC_EXPORT_LOGIC)
	CmdMsg(IDC_EXPORT_PROJ)
	CmdMsg(ID_REPORT_FILE_LIST)

	CmdMsg(IDC_EXPORT_2DSCENE)
	CmdMsg(IDC_EXPORT_3DSCENE)
	//
	CmdMsg(ID_LANGUAGE_CHINESS)
	CmdMsg(ID_LANGUAGE_ENGLISH)
	CmdMsg(ID_LANGUAGE_RUSSIAN)
	CmdMsg(ID_LANGUAGE_KOREAN)
	CmdMsg(ID_LANGUAGE_JAPANESS)
	CmdMsg(ID_LANGUAGE_CHINESS_T)
	//
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CPhantomGCView::OnUpdateFileSave)
	ON_COMMAND(ID_EXPORT_FULL, &CPhantomGCView::OnExportFull)
	ON_UPDATE_COMMAND_UI(ID_SHOW_CHARACTERs, &CPhantomGCView::OnUpdateShowCharacters)
	ON_COMMAND(ID_SHOW_CHARACTERs, &CPhantomGCView::OnShowCharacters)
	ON_UPDATE_COMMAND_UI(ID_MenuCenter, &CPhantomGCView::OnUpdateMenucenter)
	ON_COMMAND(ID_MenuCenter, &CPhantomGCView::OnMenucenter)
	ON_UPDATE_COMMAND_UI(IDC_BUILD_E_CODE, &CPhantomGCView::OnUpdateBuildECode)
	ON_COMMAND(IDC_BUILD_E_CODE, &CPhantomGCView::OnBuildECode)
	ON_UPDATE_COMMAND_UI(ID_SHOW_COLLISION, &CPhantomGCView::OnUpdateShowCollision)
	ON_COMMAND(ID_SHOW_COLLISION, &CPhantomGCView::OnShowCollision)
	ON_UPDATE_COMMAND_UI(ID_GAME_SIZE, &CPhantomGCView::OnUpdateGameSize)
	ON_COMMAND(ID_GAME_SIZE, &CPhantomGCView::OnGameSize)
	ON_UPDATE_COMMAND_UI(ID_SOFT_SHADOW_MODE, &CPhantomGCView::OnUpdateSoftShadowMode)
	ON_COMMAND(ID_SOFT_SHADOW_MODE, &CPhantomGCView::OnSoftShadowMode)
	ON_UPDATE_COMMAND_UI(ID_UI_PNG_MAKER, &CPhantomGCView::OnUpdateUiPngMaker)
	ON_COMMAND(ID_UI_PNG_MAKER, &CPhantomGCView::OnUiPngMaker)
	ON_UPDATE_COMMAND_UI(ID_SHOWUI_MAPGRID, &CPhantomGCView::OnUpdateShowuiMapgrid)
	ON_COMMAND(ID_SHOWUI_MAPGRID, &CPhantomGCView::OnShowuiMapgrid)
	ON_UPDATE_COMMAND_UI(ID_LEVELS_LEVEL1, &CPhantomGCView::OnUpdateLevelsLevel1)
	ON_COMMAND(ID_LEVELS_LEVEL1, &CPhantomGCView::OnLevelsLevel1)
	ON_UPDATE_COMMAND_UI(ID_LEVELS_LEVEL2, &CPhantomGCView::OnUpdateLevelsLevel2)
	ON_COMMAND(ID_LEVELS_LEVEL2, &CPhantomGCView::OnLevelsLevel2)
	ON_UPDATE_COMMAND_UI(ID_LEVELS_LEVEL3, &CPhantomGCView::OnUpdateLevelsLevel3)
	ON_COMMAND(ID_LEVELS_LEVEL3, &CPhantomGCView::OnLevelsLevel3)
	ON_UPDATE_COMMAND_UI(ID_LEVELS_LEVEL4, &CPhantomGCView::OnUpdateLevelsLevel4)
	ON_COMMAND(ID_LEVELS_LEVEL4, &CPhantomGCView::OnLevelsLevel4)
	ON_UPDATE_COMMAND_UI(ID_LEVELS_LEVEL5, &CPhantomGCView::OnUpdateLevelsLevel5)
	ON_COMMAND(ID_LEVELS_LEVEL5, &CPhantomGCView::OnLevelsLevel5)
	ON_UPDATE_COMMAND_UI(ID_LEVELS_LEVEL6, &CPhantomGCView::OnUpdateLevelsLevel6)
	ON_COMMAND(ID_LEVELS_LEVEL6, &CPhantomGCView::OnLevelsLevel6)
	ON_UPDATE_COMMAND_UI(ID_UIMAP_MOVEDISABLE, &CPhantomGCView::OnUpdateUimapMovedisable)
	ON_COMMAND(ID_UIMAP_MOVEDISABLE, &CPhantomGCView::OnUimapMovedisable)
	ON_UPDATE_COMMAND_UI(ID_UIMAP_MOVEDISABLEMODE, &CPhantomGCView::OnUpdateUimapMovedisablemode)
	ON_COMMAND(ID_UIMAP_MOVEDISABLEMODE, &CPhantomGCView::OnUimapMovedisablemode)
	ON_UPDATE_COMMAND_UI(ID_SHOW_CENTER, &CPhantomGCView::OnUpdateShowCenter)
	ON_COMMAND(ID_SHOW_CENTER, &CPhantomGCView::OnShowCenter)
	ON_UPDATE_COMMAND_UI(ID_SET_LEVEL_MODE, &CPhantomGCView::OnUpdateSetLevelMode)
	ON_COMMAND(ID_SET_LEVEL_MODE, &CPhantomGCView::OnSetLevelMode)
	ON_UPDATE_COMMAND_UI(ID_SEL_IMAGE_MODE, &CPhantomGCView::OnUpdateSelImageMode)
	ON_COMMAND(ID_SEL_IMAGE_MODE, &CPhantomGCView::OnSelImageMode)
	ON_UPDATE_COMMAND_UI(ID_BUILD_CPP_CODE, &CPhantomGCView::OnUpdateBuildCppCode)
	ON_COMMAND(ID_BUILD_CPP_CODE, &CPhantomGCView::OnBuildCppCode)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_EFFECT2D, &CPhantomGCView::OnUpdateInsertEffect2d)
	ON_COMMAND(IDM_INSERT_EFFECT2D, &CPhantomGCView::OnInsertEffect2d)
	ON_COMMAND(ID_BOX2D_USAGE, &CPhantomGCView::OnBox2dUsage)
	ON_UPDATE_COMMAND_UI(ID_BOX2D_USAGE, &CPhantomGCView::OnUpdateBox2dUsage)
	ON_UPDATE_COMMAND_UI(ID_ENCRYPT_FILE, &CPhantomGCView::OnUpdateEncryptFile)
	ON_COMMAND(ID_ENCRYPT_FILE, &CPhantomGCView::OnEncryptFile)
	ON_COMMAND(ID_SHOW_MYCODE, &CPhantomGCView::OnShowMycode)
	ON_UPDATE_COMMAND_UI(ID_SHOW_MYCODE, &CPhantomGCView::OnUpdateShowMycode)
	ON_COMMAND(ID_SET_GRID_COLOR, &CPhantomGCView::OnSetGridColor)
	ON_UPDATE_COMMAND_UI(ID_SET_GRID_COLOR, &CPhantomGCView::OnUpdateSetGridColor)
	ON_COMMAND(ID_CONVERT_ALL_TOPNG, &CPhantomGCView::OnConvertAllTopng)
	ON_UPDATE_COMMAND_UI(ID_CONVERT_ALL_TOPNG, &CPhantomGCView::OnUpdateConvertAllTopng)
	ON_UPDATE_COMMAND_UI(ID_GEN_CPP_CODE, &CPhantomGCView::OnUpdateGenCppCode)
	ON_COMMAND(ID_GEN_CPP_CODE, &CPhantomGCView::OnGenCppCode)
	ON_UPDATE_COMMAND_UI(ID_GENERICCODES_GENERICC, &CPhantomGCView::OnUpdateGenericcodesGenericc)
	ON_COMMAND(ID_GENERICCODES_GENERICC, &CPhantomGCView::OnGenericcodesGenericc)
	ON_UPDATE_COMMAND_UI(ID_GENERICCODES_GENERICHTMLREADERJSFILES, &CPhantomGCView::OnUpdateGenericcodesGenerichtmlreaderjsfiles)
	ON_COMMAND(ID_GENERICCODES_GENERICHTMLREADERJSFILES, &CPhantomGCView::OnGenericcodesGenerichtmlreaderjsfiles)
	ON_UPDATE_COMMAND_UI(ID_GENERICCODES_GENERICEINITDIALOGCODE, &CPhantomGCView::OnUpdateGenericcodesGenericeinitdialogcode)
	ON_COMMAND(ID_GENERICCODES_GENERICEINITDIALOGCODE, &CPhantomGCView::OnGenericcodesGenericeinitdialogcode)
	ON_COMMAND(ID_MESH_TO_SCENE, &CPhantomGCView::OnMeshToScene)
	ON_UPDATE_COMMAND_UI(ID_MESH_TO_SCENE, &CPhantomGCView::OnUpdateMeshToScene)
	ON_COMMAND(ID_BRUSH_CIRCLE, &CPhantomGCView::OnBrushCircle)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_CIRCLE, &CPhantomGCView::OnUpdateBrushCircle)
	ON_COMMAND(ID_BRUSH_BOX, &CPhantomGCView::OnBrushBox)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_BOX, &CPhantomGCView::OnUpdateBrushBox)
	ON_UPDATE_COMMAND_UI(ID_AUTOMOVE_MODE, &CPhantomGCView::OnUpdateAutomoveMode)
	ON_COMMAND(ID_AUTOMOVE_MODE, &CPhantomGCView::OnAutomoveMode)
	ON_COMMAND(ID_USER_INFO, &CPhantomGCView::OnUserInfo)
	ON_UPDATE_COMMAND_UI(ID_SHOW_OBJECTS_DLG, &CPhantomGCView::OnUpdateShowObjectsDlg)
	ON_COMMAND(ID_SHOW_OBJECTS_DLG, &CPhantomGCView::OnShowObjectsDlg)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SCRIPT, &CPhantomGCView::OnUpdateShowScript)
	ON_COMMAND(ID_SHOW_SCRIPT, &CPhantomGCView::OnShowScript)
	ON_WM_MOVING()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_COLOR_SET, &CPhantomGCView::OnColorSet)
	ON_COMMAND(ID_RELOAD_TABLEs, &CPhantomGCView::OnReloadTables)
	ON_UPDATE_COMMAND_UI(ID_SHOW_BOX, &CPhantomGCView::OnUpdateShowBox)
	ON_COMMAND(ID_SHOW_BOX, &CPhantomGCView::OnShowBox)
	ON_UPDATE_COMMAND_UI(ID_REGION_SET, &CPhantomGCView::OnUpdateRegionSet)
	ON_COMMAND(ID_REGION_SET, &CPhantomGCView::OnRegionSet)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CPhantomGCView::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_UNDO, &CPhantomGCView::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CPhantomGCView::OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_REDO, &CPhantomGCView::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_BUY_RESOURCE, &CPhantomGCView::OnUpdateBuyResource)
	ON_COMMAND(ID_BUY_RESOURCE, &CPhantomGCView::OnBuyResource)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_PAK, &CPhantomGCView::OnUpdateExportPak)
	ON_COMMAND(ID_EXPORT_PAK, &CPhantomGCView::OnExportPak)
	ON_COMMAND(ID_CONVERT_ALL_FBX, &CPhantomGCView::OnConvertAllFbx)
	END_MESSAGE_MAP()

// CPhantomGCView 构造/析构
CPhantomGCView::CPhantomGCView()
{
	// TODO: 在此处添加构造代码
	m_wndViewer		=	0;
	m_bIsPopup		=	false;
}

CPhantomGCView::~CPhantomGCView()
{
}

BOOL CPhantomGCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style	|=	WS_HSCROLL|WS_VSCROLL;
	return CView::PreCreateWindow(cs);
}

// CPhantomGCView 绘制

void CPhantomGCView::OnDraw(CDC* /*pDC*/)
{
	CPhantomGCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CPhantomGCView 打印


void CPhantomGCView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CPhantomGCView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CPhantomGCView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CPhantomGCView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CPhantomGCView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CPhantomGCView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
//#ifndef SHARED_HANDLERS
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
//#endif
}


// CPhantomGCView 诊断

#ifdef _DEBUG
void CPhantomGCView::AssertValid() const
{
	CView::AssertValid();
}

void CPhantomGCView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPhantomGCDoc* CPhantomGCView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPhantomGCDoc)));
	return (CPhantomGCDoc*)m_pDocument;
}
#endif //_DEBUG


// CPhantomGCView 消息处理程序


void CPhantomGCView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CView::OnShowWindow(bShow, nStatus);
}


void CPhantomGCView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}


void CPhantomGCView::OnActivateFrame(UINT nState, CFrameWnd* pDeactivateFrame)
{
	if(WA_ACTIVE == nState)
	{
		//GetParent()->SetWindowTextW(this->m_pDocument->GetTitle());
	}
	CView::OnActivateFrame(nState, pDeactivateFrame);
}


void CPhantomGCView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CView::OnActivate(nState, pWndOther, bMinimized);
}


void CPhantomGCView::OnDestroy()
{
	if(GetViewManager() && GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetActiveView() == this)
	{
		GetViewManager()->GetActiveView()->OnDeActive();
	}
	CView::OnDestroy();
}


BOOL CPhantomGCView::OnEraseBkgnd(CDC* pDC)
{
	if(m_wndViewer)
		return true;
	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&rc, &CBrush(RGB(240,240,240)));
	//
	rc.OffsetRect( - GetScrollPos(SB_HORZ), - GetScrollPos(SB_VERT) );
	//
	pDC->SetBkMode(TRANSPARENT);
	//Draw Borders
	CPen pen(PS_SOLID, 2, RGB(50,200,85));
	CPen* oldPen = pDC->SelectObject(&pen);
	CRect rcBorder(rc.left + VIEW_LEFT_RESERVED - 1, rc.top + VIEW_TOP_RESERVED - 1, 0, 0);
	rcBorder.right	=	rcBorder.left + GetRenderWindowWidth() + 3;
	rcBorder.bottom	=	rcBorder.top + GetRenderWindowHeight() + 3;
	RenderRect(pDC, rcBorder);
	if(oldPen)
		pDC->SelectObject(oldPen);
	//Draw Texts
	wchar_t buf[512];
	swprintf(buf, L"%s[%dx%d]", Language(L"屏幕大小"), GetRenderWindowWidth(), GetRenderWindowHeight());
	CFont* oldFont	=	pDC->SelectObject(GetBitmapMgr()->GetFontPtr(FONT_MODE_NORMAL));
	CRect rcText(rc.left + VIEW_LEFT_RESERVED, rc.top, rc.right, rc.top + VIEW_TOP_RESERVED);
	pDC->DrawText(buf, &rcText, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	if(oldFont)
		pDC->SelectObject(oldFont);
	return TRUE;
}


void CPhantomGCView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	OnMfcScroll(m_hWnd, SB_VERT, nSBCode, nPos);
	ChangeViewScroll();
	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CPhantomGCView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	OnMfcScroll(m_hWnd, SB_HORZ, nSBCode, nPos);
	ChangeViewScroll();
	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

VOID CPhantomGCView::ChangeViewScroll()
{
	{
		int minpos;
		int maxpos;
		GetScrollRange(SB_VERT, &minpos, &maxpos); 
		int curpos = GetScrollPos(SB_VERT);
		if(curpos < minpos)
			curpos = minpos;
		else if(curpos >= maxpos)
			curpos	=	maxpos;
		SetScrollPos(SB_VERT, curpos, TRUE);
	}
	{
		int minpos;
		int maxpos;
		GetScrollRange(SB_HORZ, &minpos, &maxpos); 
		int curpos = GetScrollPos(SB_HORZ);
		if(curpos < minpos)
			curpos = minpos;
		else if(curpos >= maxpos)
			curpos	=	maxpos;
		SetScrollPos(SB_HORZ, curpos, TRUE);
	}
	if(m_wndViewer)
	{
		this->ShowScrollBar(SB_VERT, FALSE);
		this->ShowScrollBar(SB_HORZ, FALSE);
		//CRect rcView;
		//m_wndViewer->GetWindowRect(&rcView);
		//m_wndViewer->MoveWindow(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT), rcView.Width(), rcView.Height(), TRUE);
	}
	else
		MoveRenderWindow(m_hWnd, -GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));
	this->Invalidate();
}

void CPhantomGCView::OnSize(UINT nType, int cx, int cy)
{
	if(m_wndViewer && m_wndViewer->GetWindowPtr())
	{
		if(m_bIsPopup)
		{
		}
		else
			m_wndViewer->GetWindowPtr()->MoveWindow(0, 0, cx, cy);
		::ShowWindow(GetDeviceWindow(), SW_HIDE);
	}
	if(GetViewManager())
	{
		GetViewManager()->RefreshRenderSize();
		ChangeViewScroll();
	}
	CView::OnSize(nType, cx, cy);
	if(m_bIsPopup&&m_wndViewer)
	{
		CRect rc;
		GetWindowRect(&rc);
		m_wndViewer->OnViewSize(&rc);
	}

}

void CPhantomGCView::OnFileSave()
{
	if(GetViewManager()->GetActiveView())
		GetViewManager()->GetActiveView()->SaveFile();
}

void CPhantomGCView::OnFileSaveAs()
{
	if(GetViewManager()->GetActiveView())
		GetViewManager()->GetActiveView()->SaveFile(TRUE);
}



void CPhantomGCView::OnFileOpen()
{
	GetMainFrame()->OnFileOpen();
}


void CPhantomGCView::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
}

extern	bool g_bShowSelectedOnly;
void CPhantomGCView::OnUpdateMeshShowAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GeometryEditor);
	if(GetViewManager()->GetActiveView())
		pCmdUI->SetCheck(GetViewManager()->GetActiveView()->IsShowSelectOnly());
}


void CPhantomGCView::OnMeshShowAll()
{
	if(GetViewManager()->GetActiveView())
	{
		GetViewManager()->GetActiveView()->SetShowSelectOnly(!GetViewManager()->GetActiveView()->IsShowSelectOnly());
		GetViewManager()->GetActiveView()->OnChangeTreeSelect(GetViewManager()->GetActiveView()->GetTreeView(), 0);
	}
}


void CPhantomGCView::OnEditCopy()
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetTreeView())
	{
		if(GetViewManager()->GetActiveView()->GetTreeView()->m_hWnd == ::GetFocus() || GetDeviceWindow() == ::GetFocus())
			GetViewManager()->GetActiveView()->OnCopyTree();
	}
}


void CPhantomGCView::OnEditPaste()
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetTreeView())
	{
		if(GetViewManager()->GetActiveView()->GetTreeView()->m_hWnd == ::GetFocus() || GetDeviceWindow() == ::GetFocus())
			GetViewManager()->GetActiveView()->OnPasteTree();
	}
}

void CPhantomGCView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetTreeView())
	{
		if(GetViewManager()->GetActiveView()->GetTreeView()->m_hWnd == ::GetFocus() || GetDeviceWindow() == ::GetFocus())
			pCmdUI->Enable(GetViewManager()->GetActiveView()->IsCopyTree());
	}
}

void CPhantomGCView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetTreeView())
	{
		if(GetViewManager()->GetActiveView()->GetTreeView()->m_hWnd == ::GetFocus() || GetDeviceWindow() == ::GetFocus())
			pCmdUI->Enable(GetViewManager()->GetActiveView()->IsPasteTree());
	}
}

void CPhantomGCView::OnUpdateEditUiMode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->GetEditorMode() > 0);
	}
}

void CPhantomGCView::OnEditUiMode()
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		if(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->GetEditorMode() != GCViewUIEditor::EditorMode_Default)
			static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->SetEditorMode(GCViewUIEditor::EditorMode_Default);
		else
			static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->SetEditorMode(GCViewUIEditor::EditorMode_None);
	}
}


void CPhantomGCView::OnFileNew()
{
	GetMainFrame()->OnFileNew();
}


void CPhantomGCView::OnShowUiGrid()
{
	if(!GetViewManager()->GetActiveView())
		return;
	GetViewManager()->GetActiveView()->ShowUIGrid(!GetViewManager()->GetActiveView()->IsShowUIGrid());
	//SetConfigInt("UIGridVisible", IsShowUIGrid());
}

void CPhantomGCView::OnUpdateShowUiGrid(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL);
	if(GetViewManager()->GetActiveView())
	{
		pCmdUI->SetCheck(GetViewManager()->GetActiveView()->IsShowUIGrid());
	}
}


void CPhantomGCView::OnUpdateUiGridEditor(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(GetViewManager()->GetActiveView());
	//if(GetViewManager()->GetActiveView())
	//{
	//	//pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->IsEditorFloorMode() && (static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetTerrain2DPtr() != NULL));
	//}
}

void CPhantomGCView::OnUiGridEditor()
{
	static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->SetEditorMode(GCViewUIEditor::EditorMode_TerrainFloor);
}


void CPhantomGCView::OnUiUnitEditorMode()
{
	if(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->GetEditorMode() != GCViewUIEditor::EditorMode_TerrainUnit)
		static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->SetEditorMode(GCViewUIEditor::EditorMode_TerrainUnit);
	else
		static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->SetEditorMode(GCViewUIEditor::EditorMode_Default);
}

void CPhantomGCView::OnUpdateUiUnitEditorMode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->IsEditorUnitMode());
	}
}


void CPhantomGCView::OnUpdateUiInsertNull(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetEditorMode() >= 0)
	{
		pCmdUI->SetCheck(GetViewManager()->GetActiveView()->GetEditorMode() == 0);
	}
}

void CPhantomGCView::OnUiInsertNull()
{
	GetViewManager()->GetActiveView()->SetEditorMode(0);
	//if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	//	GetMainFrame()->m_uiProperties.m_dlgUI.OnEnable(TRUE);
	//else if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Scene)
	//	GetMainFrame()->m_uiProperties.m_sceneEditor.OnEnable(TRUE);
}

void CPhantomGCView::OnUpdateSceneGridSet(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Scene);
	//if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Scene)
	//{
	//	pCmdUI->SetCheck(GetViewManager()->GetActiveView()->GetEditorMode() == GCViewSceneEditor2D::EditorMode_TerrainGrids);
	//}
}

void CPhantomGCView::OnSceneGridSet()
{
	//if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Scene)
	//{
	//	GetViewManager()->GetActiveView()->SetEditorMode(GCViewSceneEditor2D::EditorMode_TerrainGrids);
	//}
}

void CPhantomGCView::OnUpdateSceneUnitEditor(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Scene);
	//if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Scene)
	//{
	//	pCmdUI->SetCheck(GetViewManager()->GetActiveView()->GetEditorMode() == GCViewSceneEditor2D::EditorMode_UnitObjects);
	//}
}

void CPhantomGCView::OnSceneUnitEditor()
{
	//if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Scene)
	//{
	//	GetViewManager()->GetActiveView()->SetEditorMode(GCViewSceneEditor2D::EditorMode_UnitObjects);
	//}
}

extern	VOID	UI_SetEnableUI(CCmdUI *pCmdUI, INT nType);
extern	VOID	UI_SetInsertUI(INT nType);

void CPhantomGCView::OnUpdateInsertFormatstatic(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_VIEW_FORMAT_TEXT);}
void CPhantomGCView::OnUpdateInsertImeedit(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_IMEEDITBOX);}
void CPhantomGCView::OnUpdateInsertFormatedit(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_FORMAT_EDITBOX);}
void CPhantomGCView::OnUpdateInsertCheckbox(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_CHECKBOX);}
void CPhantomGCView::OnUpdateInsertCombobox(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_COMBOBOX);}
void CPhantomGCView::OnUpdateInsertSlider(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_SLIDER);}
void CPhantomGCView::OnUpdateInsertListbox(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_LISTBOX);}
void CPhantomGCView::OnUpdateInsertScrollbar(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_SCROLLBAR);}
void CPhantomGCView::OnUpdateInsertProgress(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_PROGRESS);}
void CPhantomGCView::OnUpdateInsertListctrl(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_LISTCTRL3D);}
void CPhantomGCView::OnUpdateInsertButton(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_BUTTON);}
void CPhantomGCView::OnUpdateInsertStatic(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIControlType_STATIC);}
void CPhantomGCView::OnInsertFormatstatic(){UI_SetInsertUI(UIControlType_VIEW_FORMAT_TEXT);}
void CPhantomGCView::OnInsertImeedit(){UI_SetInsertUI(UIControlType_IMEEDITBOX);}
void CPhantomGCView::OnInsertFormatedit(){UI_SetInsertUI(UIControlType_FORMAT_EDITBOX);}
void CPhantomGCView::OnInsertCheckbox(){UI_SetInsertUI(UIControlType_CHECKBOX);}
void CPhantomGCView::OnInsertCombobox(){UI_SetInsertUI(UIControlType_COMBOBOX);}
void CPhantomGCView::OnInsertSlider(){UI_SetInsertUI(UIControlType_SLIDER);}
void CPhantomGCView::OnInsertListbox(){UI_SetInsertUI(UIControlType_LISTBOX);}
void CPhantomGCView::OnInsertScrollbar(){UI_SetInsertUI(UIControlType_SCROLLBAR);}
void CPhantomGCView::OnInsertProgress(){UI_SetInsertUI(UIControlType_PROGRESS);}
void CPhantomGCView::OnInsertListctrl(){UI_SetInsertUI(UIControlType_LISTCTRL3D);}
void CPhantomGCView::OnInsertButton(){UI_SetInsertUI(UIControlType_BUTTON);}
void CPhantomGCView::OnInsertStatic(){UI_SetInsertUI(UIControlType_STATIC);}
void CPhantomGCView::OnUpdateTabSet(CCmdUI *pCmdUI){UI_SetEnableUI(pCmdUI, UIEditorFlags_TabIndex);}
void CPhantomGCView::OnTabSet(){UI_SetInsertUI(UIEditorFlags_TabIndex);}

void CPhantomGCView::OnOperatorNone()
{
	if(GetViewManager()->GetActiveView())
	{
		GetViewManager()->GetActiveView()->SetEditorMode(0);
		UpdateRobbinNow();
	}
}
void CPhantomGCView::OnUpdateOperatorNone(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView())
	{
		pCmdUI->Enable(true);
		pCmdUI->SetCheck(GetViewManager()->GetActiveView()->GetEditorMode() == 0);
	}
}


void CPhantomGCView::OnSetFloorEnable()
{
	//if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Scene)
	//	static_cast<GCViewSceneEditor2D*>(GetViewManager()->GetActiveView())->SetSubEditorMode(GCViewSceneEditor2D::TerrainGrid_Enable);
}

void CPhantomGCView::OnUpdateSetFloorEnable(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	//if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Scene)
	//{
	//	pCmdUI->Enable(GetViewManager()->GetActiveView()->GetEditorMode() == GCViewSceneEditor2D::EditorMode_TerrainGrids);
	//	pCmdUI->SetCheck(static_cast<GCViewSceneEditor2D*>(GetViewManager()->GetActiveView())->m_subEditorMode == GCViewSceneEditor2D::TerrainGrid_Enable);
	//}
}


void CPhantomGCView::OnSetFloorDisable()
{
	//if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Scene)
	//	static_cast<GCViewSceneEditor2D*>(GetViewManager()->GetActiveView())->SetSubEditorMode(GCViewSceneEditor2D::TerrainGrid_Disable);
}


void CPhantomGCView::OnUpdateSetFloorDisable(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	//if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Scene)
	//{
	//	pCmdUI->Enable(GetViewManager()->GetActiveView()->GetEditorMode() == GCViewSceneEditor2D::EditorMode_TerrainGrids);
	//	pCmdUI->SetCheck(static_cast<GCViewSceneEditor2D*>(GetViewManager()->GetActiveView())->m_subEditorMode == GCViewSceneEditor2D::TerrainGrid_Disable);
	//}
}


void CPhantomGCView::OnDeleteView()
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetTreeView())
	{
		if(GetViewManager()->GetActiveView()->GetTreeView()->m_hWnd == ::GetFocus())
			GetViewManager()->GetActiveView()->OnDeleteTree();
	}
}


void CPhantomGCView::OnUpdateDeleteView(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetTreeView())
	{
		if(GetViewManager()->GetActiveView()->GetTreeView()->m_hWnd == ::GetFocus())
			pCmdUI->Enable(GetViewManager()->GetActiveView()->IsDeleteTree());
	}
}


VOID	SetViewAlign(CCmdUI *pCmdUI, INT nAlign)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->IsAlignEnable())
		pCmdUI->Enable(true);
}

VOID	SetViewAlign(INT nAlign)
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->IsAlignEnable())
		GetViewManager()->GetActiveView()->AlignObjects((AlignType)nAlign, GetViewManager()->GetActiveView()->GetAlignValue((AlignType)nAlign));
}

void CPhantomGCView::OnAlignLeft(){SetViewAlign(AlignType_Left);}
void CPhantomGCView::OnUpdateAlignLeft(CCmdUI *pCmdUI){SetViewAlign(pCmdUI, AlignType_Left);}
void CPhantomGCView::OnAlignRight(){SetViewAlign(AlignType_Right);}
void CPhantomGCView::OnUpdateAlignRight(CCmdUI *pCmdUI){SetViewAlign(pCmdUI, AlignType_Right);}
void CPhantomGCView::OnAlignTop(){SetViewAlign(AlignType_Top);}
void CPhantomGCView::OnUpdateAlignTop(CCmdUI *pCmdUI){SetViewAlign(pCmdUI, AlignType_Top);}
void CPhantomGCView::OnAlignBottom(){SetViewAlign(AlignType_Bottom);}
void CPhantomGCView::OnUpdateAlignBottom(CCmdUI *pCmdUI){SetViewAlign(pCmdUI, AlignType_Bottom);}
void CPhantomGCView::OnAlignWidth(){SetViewAlign(AlignType_Width);}
void CPhantomGCView::OnUpdateAlignWidth(CCmdUI *pCmdUI){SetViewAlign(pCmdUI, AlignType_Width);}
void CPhantomGCView::OnAlignHeight(){SetViewAlign(AlignType_Height);}
void CPhantomGCView::OnUpdateAlignHeight(CCmdUI *pCmdUI){SetViewAlign(pCmdUI, AlignType_Height);}
void CPhantomGCView::OnAlignSpaceX(){SetViewAlign(AlignType_SpaceX);}
void CPhantomGCView::OnUpdateAlignSpaceX(CCmdUI *pCmdUI){SetViewAlign(pCmdUI, AlignType_SpaceX);}
void CPhantomGCView::OnAlignSpaceY(){SetViewAlign(AlignType_SpaceY);}
void CPhantomGCView::OnUpdateAlignSpaceY(CCmdUI *pCmdUI){SetViewAlign(pCmdUI, AlignType_SpaceY);}
//
void CPhantomGCView::OnShowAlign()
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
		GetMainFrame()->m_alignDlg.ShowWindow(GetMainFrame()->m_alignDlg.IsWindowVisible() ? SW_HIDE : SW_SHOW);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
		GetMainFrame()->m_alignDlg3D.ShowWindow(GetMainFrame()->m_alignDlg3D.IsWindowVisible() ? SW_HIDE : SW_SHOW);
}
void CPhantomGCView::OnUpdateShowAlign(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetMainFrame()->m_alignDlg.IsWindowVisible() || GetMainFrame()->m_alignDlg3D.IsWindowVisible());
}


void CPhantomGCView::OnBgColor()
{
	Phantom::color4 color(GetConfigInt("BgColor"));
	CColorDialog c(color.getRGB(), 0, this);
	if(c.DoModal() != IDOK)
		return;
	color	=	Phantom::color4(c.GetColor(), 1.0f);
	Phantom::g_manager->SetClearColor(color);
	//SetBackgroundColor(color);
	wchar_t buf[128];
	swprintf(buf, L"%d", color.getRGB());
	AfxGetApp()->WriteProfileStringW(L"PhantomVD", L"BgColor", buf);
}

void CPhantomGCView::OnUpdateBgColor(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);
}


BOOL CPhantomGCView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(this->m_wndViewer)
		this->m_wndViewer->OnMouseWheel(zDelta);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CPhantomGCView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_wndViewer)
		m_wndViewer->OnKeyDown(nChar);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CPhantomGCView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_wndViewer)
		m_wndViewer->OnKeyUp(nChar);
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CPhantomGCView::OnPropertyProp()
{
	GetMainFrame()->ShowPane(&GetMainFrame()->m_wndProperties, (GetMainFrame()->m_wndProperties.GetStyle() & WS_VISIBLE) == 0, FALSE, FALSE);
}


void CPhantomGCView::OnUpdatePropertyProp(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck((GetMainFrame()->m_wndProperties.GetStyle() & WS_VISIBLE) > 0);
}


void CPhantomGCView::OnPropertyOutput()
{
	GetMainFrame()->ShowPane(&GetMainFrame()->m_wndOutput, (GetMainFrame()->m_wndOutput.GetStyle() & WS_VISIBLE) == 0, FALSE, FALSE);
}


void CPhantomGCView::OnUpdatePropertyOutput(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetMainFrame()->m_wndOutput.GetStyle() & WS_VISIBLE);
}


void CPhantomGCView::OnPropertyView()
{
	GetMainFrame()->ShowPane(&GetMainFrame()->m_wndFileView, (GetMainFrame()->m_wndFileView.GetStyle() & WS_VISIBLE) == 0, FALSE, FALSE);
}


void CPhantomGCView::OnUpdatePropertyView(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetMainFrame()->m_wndFileView.GetStyle() & WS_VISIBLE);
}


void CPhantomGCView::OnPeropertyWindow()
{
	GetMainFrame()->ShowPane(&GetMainFrame()->m_wndPropertiesDialog, (GetMainFrame()->m_wndPropertiesDialog.GetStyle() & WS_VISIBLE) == 0, FALSE, FALSE);
}

void CPhantomGCView::OnUpdatePeropertyWindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetMainFrame()->m_wndPropertiesDialog.GetStyle() & WS_VISIBLE);
}

void CPhantomGCView::OnUpdateGameRun(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GameProject);
	//if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GameProject)
	//	pCmdUI->Enable(!static_cast<GCGameProject*>(GetViewManager()->GetActiveView())->IsGameRuntime());
}

void CPhantomGCView::OnGameRun()
{
	//if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GameProject)
	//{
	//	GCGameProject* project	=	static_cast<GCGameProject*>(GetViewManager()->GetActiveView());
	//	project->SetGameRuntime(TRUE);//!project->IsGameRuntime());
	//}
}


void CPhantomGCView::OnUpdateGamePause(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GameProject);
	//if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GameProject)
	//	pCmdUI->Enable(static_cast<GCGameProject*>(GetViewManager()->GetActiveView())->IsGameRuntime());
}

void CPhantomGCView::OnGamePause()
{
}

void CPhantomGCView::OnUpdateGameStop(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GameProject);
	//if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GameProject)
	//	pCmdUI->Enable(static_cast<GCGameProject*>(GetViewManager()->GetActiveView())->IsGameRuntime());
}

void CPhantomGCView::OnGameStop()
{
	//if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GameProject)
	//{
	//	GCGameProject* project	=	static_cast<GCGameProject*>(GetViewManager()->GetActiveView());
	//	project->SetGameRuntime(false);//!project->IsGameRuntime());
	//}
}
//FileViewerType_GCSceneEditor3D
#define	UPDATE_EDIT_MODE(fn, classType)	void CPhantomGCView::OnUpdate##fn(CCmdUI *pCmdUI){\
	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_##classType);\
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_##classType)\
	pCmdUI->SetCheck(GetViewManager()->GetActiveView()->GetEditorMode() == classType::EditorMode_##fn);}\
	void CPhantomGCView::On##fn(){\
	if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_##classType)\
		GetViewManager()->GetActiveView()->SetEditorMode(classType::EditorMode_##fn);}

UPDATE_EDIT_MODE(HeightAdd, GCSceneEditor3D);
UPDATE_EDIT_MODE(HeightAvg, GCSceneEditor3D);
UPDATE_EDIT_MODE(HeightAlign, GCSceneEditor3D);
UPDATE_EDIT_MODE(HeightSet, GCSceneEditor3D);
UPDATE_EDIT_MODE(HeightGridVisible, GCSceneEditor3D);
UPDATE_EDIT_MODE(HeightGridEnable, GCSceneEditor3D);
UPDATE_EDIT_MODE(RegionSet, GCSceneEditor3D);

void CPhantomGCView::OnSceneWireColor()
{
	//if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	//{
	//	GCSceneEditor3D* scene	=	static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView());
	//	CColorDialog c(scene->GetWireframeColor().getRGB(), 0, this);
	//	if(c.DoModal() != IDOK)
	//		return;
	//	scene->SetWireframeColor(color4(c.GetColor(), 1.0f));
	//}
}

void CPhantomGCView::OnUpdateSceneWireColor(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	{
		pCmdUI->Enable(true);
	}
}

UPDATE_EDIT_MODE(TextureAdd, GCSceneEditor3D);
UPDATE_EDIT_MODE(TextureAvg, GCSceneEditor3D);
UPDATE_EDIT_MODE(TextureSet, GCSceneEditor3D);
UPDATE_EDIT_MODE(TextureUVMoving, GCSceneEditor3D);
UPDATE_EDIT_MODE(TextureUVScaling, GCSceneEditor3D);
UPDATE_EDIT_MODE(TextureUVRotation, GCSceneEditor3D);
//
UPDATE_EDIT_MODE(ObjectMove, GCSceneEditor3D);
UPDATE_EDIT_MODE(ObjectScale, GCSceneEditor3D);
UPDATE_EDIT_MODE(ObjectRotation, GCSceneEditor3D);
//
UPDATE_EDIT_MODE(AddGrass, GCSceneEditor3D);
UPDATE_EDIT_MODE(RemoveGrass, GCSceneEditor3D);
//UPDATE_EDIT_MODE(LineEditorMode, GCSceneEditor3D);

GCSceneEditor3D*	GetCurrentScene()
{
	if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
		return static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView());
	return 0;
}

#define	SceneFunctionEnableCheck(fn)	GCSceneEditor3D* scene = GetCurrentScene();\
	pCmdUI->Enable(scene != NULL);\
	if(scene)pCmdUI->SetCheck(scene->m_scene3D->fn);

#define	SceneFunctionEnableCheckType(fn)	GCSceneEditor3D* scene = GetCurrentScene();\
	pCmdUI->Enable(scene != NULL);\
	if(scene)pCmdUI->SetCheck(scene->m_scene3D->IsEnabled(Phantom::Scene3D::Enable_##fn));

#define	SceneFunctionEnableCheck2(fn)	GCSceneEditor3D* scene = GetCurrentScene();\
	pCmdUI->Enable(scene != NULL);\
	if(scene)pCmdUI->SetCheck(scene->fn);

//#define	SceneFunctionEnableSet(fn, isfn)	GCSceneEditor3D* scene = GetCurrentScene();\
//	if(scene)scene->m_scene3D->fn(!scene->m_scene3D->isfn);
#define	SceneFunctionEnableSet(fn)	GCSceneEditor3D* scene = GetCurrentScene();\
	if(scene)scene->m_scene3D->SetEnabled(Phantom::Scene3D::Enable_##fn, !scene->m_scene3D->IsEnabled(Phantom::Scene3D::Enable_##fn));

CmdUpdate(ID_RENDER_POINT){}//pCmdUI->SetCheck(GetRenderHelperPtr()->GetFillMode() == FILL_POINT);}
CmdUpdate(ID_RENDER_LINE){}//pCmdUI->SetCheck(GetRenderHelperPtr()->GetFillMode() == FILL_WIREFRAME);}
CmdUpdate(ID_RENDER_TRIANGLE){}//pCmdUI->SetCheck(GetRenderHelperPtr()->GetFillMode() == FILL_SOLID);}
CmdUpdate(ID_RENDER_REGION_BOX){SceneFunctionEnableCheckType(ShowBox)}
//CmdUpdate(ID_RENDER_TERRAIN_BOX){SceneFunctionEnableCheck(IsShowTerrainBox())}
CmdUpdate(ID_RENDER_UNIT_BOX){SceneFunctionEnableCheckType(ShowCollisions)}
CmdUpdate(ID_SHOW_UNIT_OBJECT){SceneFunctionEnableCheckType(ShowObject)}
//CmdUpdate(ID_SHOW_MOVE_DISABLE){SceneFunctionEnableCheckType(ShowMoveGrids)}
//CmdUpdate(ID_SHOW_UNIT_FILE){SceneFunctionEnableCheck2(m_bShowObjectFile)}
CmdUpdate(ID_SHOW_TERRAIN){SceneFunctionEnableCheckType(ShowTerrain)}
CmdUpdate(ID_SHOW_WATER){SceneFunctionEnableCheckType(ShowWater)}
//CmdUpdate(ID_SHOW_SKY){SceneFunctionEnableCheck(IsShowSky())}
//CmdUpdate(ID_SHOW_GRASS){SceneFunctionEnableCheck(GetGrassVisible())}
//CmdUpdate(ID_WATER_MIRROR){SceneFunctionEnableCheck(IsShowWaterMirror())}
//CmdUpdate(ID_MIRROR_TERRAIN){SceneFunctionEnableCheck(IsShowTerrainMirror())}
//CmdUpdate(ID_MIRROR_SKY){SceneFunctionEnableCheck(IsShowSkyMirror())}
//CmdUpdate(ID_MIRROR_UNITs){SceneFunctionEnableCheck(IsShowObjectMirror())}
//CmdUpdate(ID_MIRROR_ALPHAUNIT){SceneFunctionEnableCheck(IsShowAlphaObjectMirror())}
//CmdUpdate(ID_MIRROR_GRASS){SceneFunctionEnableCheck(IsShowGrassMirror())}

CmdClick(ID_RENDER_POINT){}//GetRenderHelperPtr()->SetFillMode(FILL_POINT);}
CmdClick(ID_RENDER_LINE){}//GetRenderHelperPtr()->SetFillMode(FILL_WIREFRAME);}
CmdClick(ID_RENDER_TRIANGLE){}//GetRenderHelperPtr()->SetFillMode(FILL_SOLID);}
CmdClick(ID_RENDER_REGION_BOX){SceneFunctionEnableSet(ShowBox)}
//CmdClick(ID_RENDER_TERRAIN_BOX){SceneFunctionEnableSet(ShowTerrainBox,IsShowTerrainBox())}
CmdClick(ID_RENDER_UNIT_BOX){SceneFunctionEnableSet(ShowCollisions)}
CmdClick(ID_SHOW_UNIT_OBJECT){SceneFunctionEnableSet(ShowObject)}
//CmdClick(ID_SHOW_MOVE_DISABLE){SceneFunctionEnableSet(ShowMoveGrids)}

//CmdClick(ID_SHOW_UNIT_FILE){SceneFunctionEnableSet2(m_bShowObjectFile)}
CmdClick(ID_SHOW_TERRAIN){SceneFunctionEnableSet(ShowTerrain)}
CmdClick(ID_SHOW_WATER){SceneFunctionEnableSet(ShowWater)}
//CmdClick(ID_SHOW_SKY){SceneFunctionEnableSet(ShowSky,IsShowSky())}
//CmdClick(ID_SHOW_GRASS){SceneFunctionEnableSet(SetGrassVisible,GetGrassVisible())}
//CmdClick(ID_WATER_MIRROR){SceneFunctionEnableSet(ShowWaterMirror,IsShowWaterMirror())}
//CmdClick(ID_MIRROR_TERRAIN){SceneFunctionEnableSet(ShowTerrainMirror,IsShowTerrainMirror())}
//CmdClick(ID_MIRROR_SKY){SceneFunctionEnableSet(ShowSkyMirror,IsShowSkyMirror())}
//CmdClick(ID_MIRROR_UNITs){SceneFunctionEnableSet(ShowObjectMirror,IsShowObjectMirror())}
//CmdClick(ID_MIRROR_ALPHAUNIT){SceneFunctionEnableSet(ShowAlphaObjectMirror,IsShowAlphaObjectMirror())}
//CmdClick(ID_MIRROR_GRASS){SceneFunctionEnableSet(ShowGrassMirror,IsShowGrassMirror())}
//
CmdUpdate(ID_SCREEN_EFFECT_CONFIG)
{
	//pCmdUI->SetCheck(GetMainFrame()->m_screenEffect.IsWindowVisible());
}
CmdUpdate(ID_SHADOW_CONFIG)
{
	GCSceneEditor3D* scene = GetCurrentScene();
	pCmdUI->Enable(scene != NULL);
	//if(scene)
		//pCmdUI->SetCheck(scene->m_shadowMapInfo.IsWindowVisible());
}
CmdUpdate(ID_SMALLMAP_CONFIG)
{
	GCSceneEditor3D* scene = GetCurrentScene();
	pCmdUI->Enable(scene != NULL);
	//if(scene)
	//	pCmdUI->SetCheck(scene->m_smallMapInfo.IsWindowVisible());
}
CmdUpdate(ID_RENDER_CONFIG)
{
	GCSceneEditor3D* scene = GetCurrentScene();
	pCmdUI->Enable(scene != NULL);
	//if(scene)
	//	pCmdUI->SetCheck(scene->m_renderBufferInfo.IsWindowVisible());
}
CmdClick(ID_SCREEN_EFFECT_CONFIG)
{
//	GetMainFrame()->m_screenEffect.ShowWindow(SW_SHOW);
}
CmdClick(ID_SHADOW_CONFIG)
{
	GCSceneEditor3D* scene = GetCurrentScene();
	//if(scene)
	//	scene->m_shadowMapInfo.ShowWindow(SW_SHOW);
}
CmdClick(ID_SMALLMAP_CONFIG)
{
	GCSceneEditor3D* scene = GetCurrentScene();
	//if(scene)
	//	scene->m_smallMapInfo.ShowWindow(SW_SHOW);
}
CmdClick(ID_RENDER_CONFIG)
{
	GCSceneEditor3D* scene = GetCurrentScene();
	//if(scene)
	//	scene->m_renderBufferInfo.ShowWindow(SW_SHOW);
}
extern	int	g_nAuthModeTemp;//	=	AuthGUID::AuthFlags_Error;
CmdUpdate(IDC_EXPORT_UI)
{
	pCmdUI->Enable(true);
}
CmdUpdate(IDC_EXPORT_CURSOR)
{
//	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && !GetViewManager()->GetActiveView()->IsReadOnly() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_CursorEditor && (g_nAuthModeTemp > AuthGUID::AuthFlags_Error));
}
CmdUpdate(IDC_EXPORT_2DUNIT)
{
//	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && !GetViewManager()->GetActiveView()->IsReadOnly() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Unit2D && (g_nAuthModeTemp > AuthGUID::AuthFlags_Error));
}
CmdUpdate(IDC_EXPORT_MESH)
{
//	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && !GetViewManager()->GetActiveView()->IsReadOnly() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GeometryEditor && (g_nAuthModeTemp > AuthGUID::AuthFlags_Error));
}
CmdUpdate(IDC_EXPORT_3DUNIT)
{
//	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && !GetViewManager()->GetActiveView()->IsReadOnly() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Unit3D && (g_nAuthModeTemp > AuthGUID::AuthFlags_Error));
}
CmdUpdate(IDC_EXPORT_LOGIC)
{
//	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && !GetViewManager()->GetActiveView()->IsReadOnly() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_LogicSystem && (g_nAuthModeTemp > AuthGUID::AuthFlags_Error));
}
CmdUpdate(IDC_EXPORT_PROJ)
{
//	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && !GetViewManager()->GetActiveView()->IsReadOnly() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GameProject && (g_nAuthModeTemp > AuthGUID::AuthFlags_Error));
}
CmdUpdate(ID_REPORT_FILE_LIST)
{
//	pCmdUI->Enable((g_nAuthModeTemp > AuthGUID::AuthFlags_Error));
}
CmdUpdate(IDC_EXPORT_2DSCENE)
{
//	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && !GetViewManager()->GetActiveView()->IsReadOnly() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Scene && (g_nAuthModeTemp > AuthGUID::AuthFlags_Error));
}
CmdUpdate(IDC_EXPORT_3DSCENE)
{
//	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && !GetViewManager()->GetActiveView()->IsReadOnly() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D && (g_nAuthModeTemp > AuthGUID::AuthFlags_Error));
}
extern VOID	RequestRuntime();//请求在机器中运行
//添加导出功能，并制作帮助文档
extern VOID ReportFiles();
extern VOID	ClearDeviceIp();
CmdClick(IDC_EXPORT_UI)
{
	ClearDeviceIp();
	ReportFiles();
	//DefineAuth_2();
	//if(!CheckAuth_2(0))
	//	return;
	//InitReport();
	//PFileList* files = enum_files(0, GetProjectPath(), 1000, 0);
	//if(!files)return;
	//int len = strlen(GetProjectPath());
	//for(int i=0;i<files->vFiles.size();i++)
	//{
	//	const char* szFile = files->vFiles[i].szFileName + len;
	//	ReportLockFile(szFile);
	//}
	//RequestAuth();
	//SaveReport();
	//RequestRuntime();//请求在机器中运行
	//CReportDialog dlg(this);
	//dlg.m_strFileExport	=	GetViewManager()->GetActiveView()->GetFileName();
	//dlg.DoModal();
}
CmdClick(IDC_EXPORT_CURSOR)
{
}
CmdClick(IDC_EXPORT_2DUNIT)
{
	//DefineAuth_2();
	//if(!CheckAuth_2(0))
	//	return;
	//CReportDialog dlg(this);
	//dlg.m_strFileExport	=	GetViewManager()->GetActiveView()->GetFileName();
	//dlg.DoModal();
}
CmdClick(IDC_EXPORT_MESH)
{
	//DefineAuth_2();
	//if(!CheckAuth_2(0))
	//	return;
	//CReportDialog dlg(this);
	//dlg.m_strFileExport	=	GetViewManager()->GetActiveView()->GetFileName();
	//dlg.DoModal();
}
CmdClick(IDC_EXPORT_2DSCENE)
{
	//DefineAuth_2();
	//if(!CheckAuth_2(0))
	//	return;
	//CReportDialog dlg(this);
	//dlg.m_strFileExport	=	GetViewManager()->GetActiveView()->GetFileName();
	//dlg.DoModal();
}
CmdClick(IDC_EXPORT_3DSCENE)
{
	//DefineAuth_2();
	//if(!CheckAuth_2(0))
	//	return;
	//CReportDialog dlg(this);
	//dlg.m_strFileExport	=	GetViewManager()->GetActiveView()->GetFileName();
	//dlg.DoModal();
}
CmdClick(IDC_EXPORT_3DUNIT)
{
	//DefineAuth_2();
	//if(!CheckAuth_2(0))
	//	return;
	//CReportDialog dlg(this);
	//dlg.m_strFileExport	=	GetViewManager()->GetActiveView()->GetFileName();
	//dlg.DoModal();
}
CmdClick(IDC_EXPORT_LOGIC)
{
	//DefineAuth_2();
	//if(!CheckAuth_2(0))
	//	return;
	//CReportDialog dlg(this);
	//dlg.m_strFileExport	=	GetViewManager()->GetActiveView()->GetFileName();
	//dlg.DoModal();
}
CmdClick(IDC_EXPORT_PROJ)
{
	//DefineAuth_2();
	//if(!CheckAuth_2(0))
	//	return;
	//CBuildGameProject dlg(this);
	//dlg.m_projPtr		=	static_cast<GCGameProject*>(GetViewManager()->GetActiveView());
	//dlg.m_strGameName	=	static_cast<GCGameProject*>(GetViewManager()->GetActiveView())->m_gameDirector.GetName();
	//dlg.m_strFileExport	=	GetViewManager()->GetActiveView()->GetFileName();
	//dlg.DoModal();
}
CmdClick(ID_REPORT_FILE_LIST)
{
	//DefineAuth_2();
	//if(!CheckAuth_2(0))
	//	return;
	//CFileReportListDlg dlg(this);
	//dlg.DoModal();
}

void CPhantomGCView::OnShowFPS()
{
	Phantom::g_manager.m_bShowFPS = !Phantom::g_manager.m_bShowFPS;//GetProjectPtr()->m_bShowFPS	=	!GetProjectPtr()->m_bShowFPS;
}

void CPhantomGCView::OnUpdateShowFPS(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(Phantom::g_manager.m_bShowFPS);
}


void CPhantomGCView::OnUnit3DPlay()
{
	GetViewManager()->GetActiveView()->PlayAnimation();
	//if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Unit3D)
	//{
	//	static_cast<GCViewUnit3D*>(GetViewManager()->GetActiveView())->m_unitRender->Play();
	//}
	//else if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	//{
	//	GCSceneEditor3D* scene	=	static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView());
	//	scene->SetPlay();
	//}
}

void CPhantomGCView::OnUnit3DPause()
{
	GetViewManager()->GetActiveView()->PauseAnimation(!GetViewManager()->GetActiveView()->IsAnimationPause());
	//if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Unit3D)
	//{
	//	static_cast<GCViewUnit3D*>(GetViewManager()->GetActiveView())->m_unitRender->Pause(!static_cast<GCViewUnit3D*>(GetViewManager()->GetActiveView())->m_unitRender->IsPause());
	//}
	//else if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	//{
	//	GCSceneEditor3D* scene	=	static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView());
	//	scene->SetPause(!scene->m_bIsPause);
	//}
}

void CPhantomGCView::OnUnit3DStop()
{
	GetViewManager()->GetActiveView()->StopAnimation();
	//if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Unit3D)
	//{
	//	static_cast<GCViewUnit3D*>(GetViewManager()->GetActiveView())->m_unitRender->Stop();
	//}
	//else if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	//{
	//	GCSceneEditor3D* scene	=	static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView());
	//	scene->SetStop();
	//}
}

void CPhantomGCView::OnUpdateUnit3DPlay(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->IsAnimationCanPlayStop())
	{
		pCmdUI->Enable(true);
		pCmdUI->SetCheck(GetViewManager()->GetActiveView()->IsAnimationPlaying());
	}
	//if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Unit3D)
	//{
	//	pCmdUI->Enable(true);
	//	pCmdUI->SetCheck(static_cast<GCViewUnit3D*>(GetViewManager()->GetActiveView())->m_unitRender->IsPlaying());
	//}
	//else if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	//{
	//	pCmdUI->Enable(true);
	//	GCSceneEditor3D* scene	=	static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView());
	//	pCmdUI->SetCheck(scene->m_bIsPlaying);
	//}
}

void CPhantomGCView::OnUpdateUnit3DPause(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->IsAnimationCanPause())
	{
		pCmdUI->Enable(true);
		pCmdUI->SetCheck(GetViewManager()->GetActiveView()->IsAnimationPause());
	}
	//if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Unit3D)
	//{
	//	pCmdUI->Enable(true);
	//	pCmdUI->SetCheck(static_cast<GCViewUnit3D*>(GetViewManager()->GetActiveView())->m_unitRender->IsPause());
	//}
	//else if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	//{
	//	pCmdUI->Enable(true);
	//	GCSceneEditor3D* scene	=	static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView());
	//	pCmdUI->SetCheck(scene->m_bIsPause);
	//}
}

void CPhantomGCView::OnUpdateUnit3DStop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->IsAnimationCanPlayStop())
	{
		pCmdUI->Enable(true);
		pCmdUI->SetCheck(!GetViewManager()->GetActiveView()->IsAnimationPlaying());
	}
	//if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Unit3D)
	//{
	//	pCmdUI->Enable(true);
	//	pCmdUI->SetCheck(!static_cast<GCViewUnit3D*>(GetViewManager()->GetActiveView())->m_unitRender->IsPlaying());
	//}
	//else if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	//{
	//	pCmdUI->Enable(true);
	//	GCSceneEditor3D* scene	=	static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView());
	//	pCmdUI->SetCheck(!scene->m_bIsPlaying);
	//}
}

void CPhantomGCView::OnUnit3DLoop()
{
	GetViewManager()->GetActiveView()->SetAnimationLoop(!GetViewManager()->GetActiveView()->IsAnimationLoop());
	//if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Unit3D)
	//{
	//	GCViewUnit3D* unit	=	static_cast<GCViewUnit3D*>(GetViewManager()->GetActiveView());
	//	unit->m_unitRender->SetLoopMode(!unit->m_unitRender->IsLoopMode());
	//}
	//else if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	//{
	//	GCSceneEditor3D* scene	=	static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView());
	//	scene->SetLoopMode(!scene->m_bLoopMode);
	//}
}

void CPhantomGCView::OnUpdateUnit3DLoop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->IsAnimationCanLoop())
	{
		pCmdUI->Enable(true);
		pCmdUI->SetCheck(GetViewManager()->GetActiveView()->IsAnimationLoop());
	}
	//if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_Unit3D)
	//{
	//	pCmdUI->Enable(true);
	//	pCmdUI->SetCheck(static_cast<GCViewUnit3D*>(GetViewManager()->GetActiveView())->m_unitRender->IsLoopMode());
	//}
	//else if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	//{
	//	pCmdUI->Enable(true);
	//	GCSceneEditor3D* scene	=	static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView());
	//	pCmdUI->SetCheck(scene->m_bLoopMode);
	//}
}

CmdUpdate(ID_SCENE_VIEWTARGET)
{
	GCSceneEditor3D* scene = GetCurrentScene();
	pCmdUI->Enable(scene != NULL);
}
CmdClick(ID_SCENE_VIEWTARGET)
{
	GCSceneEditor3D* scene = GetCurrentScene();
	if(scene)
		scene->SetViewCenter();
}

CmdUpdate(ID_LINE_EDITOR_MODE)
{
	GCSceneEditor3D* scene = GetCurrentScene();
	pCmdUI->Enable(false);
	if(!scene)
		return;
	//TreeItemType* type	=	scene->GetSelectedItem();
	//if(type && type->type == scene->SCENE3D_SCA_TRACK)
	//{
	//	ISCA_Animation* ani	=	scene->m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
	//	if(ani)
	//	{
	//		ISCA_Base* base	=	ani->GetScaManager().SearchDataPtr(type->p2);
	//		if(base)
	//		{
	//			ISCA_Track* track	=	base->GetTrackManager().SearchDataPtr(type->p3);
	//			pCmdUI->Enable(track->IsFrameCapture());
	//		}
	//	}
	//}
}
CmdClick(ID_LINE_EDITOR_MODE)
{
	GCSceneEditor3D* scene = GetCurrentScene();
	//if(scene)
	//{
	//	TreeItemType* type	=	scene->GetSelectedItem();
	//	if(type && type->type == scene->SCENE3D_SCA_TRACK)
	//	{
	//		ISCA_Animation* ani	=	scene->m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
	//		if(ani)
	//		{
	//			ISCA_Base* base	=	ani->GetScaManager().SearchDataPtr(type->p2);
	//			if(base)
	//			{
	//				//ISCA_Track* track	=	base->GetTrackManager().SearchDataPtr(type->p3);
	//				//track->CaptureFrame(scene->m_trackDlg.GetPlayValue() / TRACK_SECOND_LENGTH);
	//				//scene->SetModifyed();
	//				//scene->m_trackDlg.Refresh();
	//			}
	//		}
	//	}
	//}
}


void CPhantomGCView::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->IsModifyed())
		pCmdUI->Enable(true);
}


CmdUpdate(ID_LANGUAGE_CHINESS)
{
	pCmdUI->SetCheck(GetLanguagesPtr()->m_language == Languages_Chiness);
}
CmdClick(ID_LANGUAGE_CHINESS)
{
	GetLanguagesPtr()->ChangeLanguage(Languages_Chiness);
}

CmdUpdate(ID_LANGUAGE_ENGLISH)
{
	pCmdUI->SetCheck(GetLanguagesPtr()->m_language == Languages_English);
}
CmdClick(ID_LANGUAGE_ENGLISH)
{
	GetLanguagesPtr()->ChangeLanguage(Languages_English);
}

CmdUpdate(ID_LANGUAGE_RUSSIAN)
{
	pCmdUI->SetCheck(GetLanguagesPtr()->m_language == Languages_Russia);
}
CmdClick(ID_LANGUAGE_RUSSIAN)
{
	GetLanguagesPtr()->ChangeLanguage(Languages_Russia);
}

CmdUpdate(ID_LANGUAGE_KOREAN)
{
	pCmdUI->SetCheck(GetLanguagesPtr()->m_language == Languages_Korean);
}
CmdClick(ID_LANGUAGE_KOREAN)
{
	GetLanguagesPtr()->ChangeLanguage(Languages_Korean);
}

CmdUpdate(ID_LANGUAGE_JAPANESS)
{
	pCmdUI->SetCheck(GetLanguagesPtr()->m_language == Languages_Japan);
}
CmdClick(ID_LANGUAGE_JAPANESS)
{
	GetLanguagesPtr()->ChangeLanguage(Languages_Japan);
}

CmdUpdate(ID_LANGUAGE_CHINESS_T)
{
	pCmdUI->SetCheck(GetLanguagesPtr()->m_language == Languages_ChinessF);
}
CmdClick(ID_LANGUAGE_CHINESS_T)
{
	GetLanguagesPtr()->ChangeLanguage(Languages_ChinessF);
}


void CPhantomGCView::OnExportFull()
{
	CMultiExportDlg dlg(this);
	dlg.DoModal();
}


void CPhantomGCView::OnUpdateShowCharacters(CCmdUI *pCmdUI)
{
//	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && !GetViewManager()->GetActiveView()->IsReadOnly() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D && (g_nAuthModeTemp > AuthGUID::AuthFlags_Error));
}


void CPhantomGCView::OnShowCharacters()
{
	//DefineAuth_2();
	//if(!CheckAuth_2(0))
	//	return;
	//if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	//{
	//	static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_bShowCharacter = !static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_bShowCharacter;
	//	if(static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_character)
	//		static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_character->SetPosition(GetCameraPtr()->GetTarget(), false);
	//}
}


void CPhantomGCView::OnUpdateMenucenter(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && !GetViewManager()->GetActiveView()->IsReadOnly() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D);
}


void CPhantomGCView::OnMenucenter()
{
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	{
		if(static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_selects.size() > 0)
		{
			//static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_controlObject.id = static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_selects[0].objectPtr->GetID();
			Phantom::GetCameraPtr()->SetTarget(static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_selectCenter);
		}
	}
}


void CPhantomGCView::OnUpdateBuildECode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_LogicSystem)
	{
		pCmdUI->Enable(true);
	}
}


void CPhantomGCView::OnBuildECode()
{
	//if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_LogicSystem)
	//{
	//	static_cast<GCViewLogicSystem*>(GetViewManager()->GetActiveView())->BuildECode();
	//}
}


void CPhantomGCView::OnUpdateShowCollision(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	{
		pCmdUI->Enable(true);
	}
}


void CPhantomGCView::OnShowCollision()
{
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	{
		static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_scene3D->SetEnabled(Phantom::Scene3D::Enable_ShowCollisions, !static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_scene3D->IsEnabled(Phantom::Scene3D::Enable_ShowCollisions));
	}
}


void CPhantomGCView::OnUpdateGameSize(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);
}

void CPhantomGCView::OnGameSize()
{
	CGameScreenSize size(this);
	size.DoModal();
}


void CPhantomGCView::OnUpdateSoftShadowMode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	{
		//pCmdUI->Enable(true);
		//pCmdUI->SetCheck(static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_scene3D->IsSoftShadowMode());
	}
}

BOOL	AutoConnection()
{
	return true;
}


void CPhantomGCView::OnSoftShadowMode()
{
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	{
		//static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_scene3D->SetSoftShadowMode(!static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_scene3D->IsSoftShadowMode());
	}
}


void CPhantomGCView::OnUpdateUiPngMaker(CCmdUI *pCmdUI)
{
}


void CPhantomGCView::OnUiPngMaker()
{
	CUIPngMaker mk(this);
	mk.DoModal();
}




void CPhantomGCView::OnUpdateShowuiMapgrid(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapEnable());
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapGridVisible());
	}
}


void CPhantomGCView::OnShowuiMapgrid()
{
	static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->SetMapGridVisible(!static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapGridVisible());
}


void CPhantomGCView::OnUpdateLevelsLevel1(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapEnable());
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.index == 0);
	}
}
void CPhantomGCView::OnLevelsLevel1()
{
	static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.index = 0;
}
void CPhantomGCView::OnUpdateLevelsLevel2(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapEnable());
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.index == 1);
	}
}
void CPhantomGCView::OnLevelsLevel2()
{
	static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.index = 1;
}
void CPhantomGCView::OnUpdateLevelsLevel3(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapEnable());
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.index == 2);
	}
}
void CPhantomGCView::OnLevelsLevel3()
{
	static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.index = 2;
}
void CPhantomGCView::OnUpdateLevelsLevel4(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapEnable());
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.index == 3);
	}
}
void CPhantomGCView::OnLevelsLevel4()
{
	static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.index = 3;
}
void CPhantomGCView::OnUpdateLevelsLevel5(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapEnable());
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.index == 4);
	}
}
void CPhantomGCView::OnLevelsLevel5()
{
	static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.index = 4;
}
void CPhantomGCView::OnUpdateLevelsLevel6(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapEnable());
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.index == 5);
	}
}
void CPhantomGCView::OnLevelsLevel6()
{
	static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.index = 5;
}


void CPhantomGCView::OnUpdateUimapMovedisable(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapEnable());
		pCmdUI->SetCheck(!static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.isEnabled());
	}
}


void CPhantomGCView::OnUimapMovedisable()
{
	if(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.isEnabled())
		static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.flags	|=	MapGridInfo::MapGridInfoFlags_Disable;
	else
		static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_gridInfo.flags	^=	MapGridInfo::MapGridInfoFlags_Disable;
}


void CPhantomGCView::OnUpdateUimapMovedisablemode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapEnable());
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bDisableMoveMode);
	}
}


void CPhantomGCView::OnUimapMovedisablemode()
{
	static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bDisableMoveMode	=	!static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bDisableMoveMode;
	if(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bDisableMoveMode)
		static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bSetLevelMode	=	false;
}


void CPhantomGCView::OnUpdateShowCenter(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapEnable());
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bShowCenter);
	}
}


void CPhantomGCView::OnShowCenter()
{
	static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bShowCenter	=	!static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bShowCenter;
}


void CPhantomGCView::OnUpdateSetLevelMode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->GetMapEnable());
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bSetLevelMode );
	}
}


void CPhantomGCView::OnSetLevelMode()
{
	static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bSetLevelMode	=	!static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bSetLevelMode;
	if(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bSetLevelMode)
		static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bDisableMoveMode	=	false;
}


void CPhantomGCView::OnUpdateSelImageMode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(true);
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bSelectElementMode );
	}
}


void CPhantomGCView::OnSelImageMode()
{
	static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bSelectElementMode = !static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_bSelectElementMode;
}


void CPhantomGCView::OnUpdateBuildCppCode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_LogicSystem)
		pCmdUI->Enable(true);
}


void CPhantomGCView::OnBuildCppCode()
{
	//if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_LogicSystem)
	//{
	//	static_cast<GCViewLogicSystem*>(GetViewManager()->GetActiveView())->BuildCppCode();
	//}
}


void CPhantomGCView::OnUpdateInsertEffect2d(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(true);
		UI_SetEnableUI(pCmdUI, Phantom::UIControlType_ui2DEffect);
	}
}


void CPhantomGCView::OnInsertEffect2d()
{
	UI_SetInsertUI(UIControlType_ui2DEffect);
}


void CPhantomGCView::OnBox2dUsage()
{
#ifdef USE_BOX
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->m_box2World.SetEnabled(!static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->m_box2World.IsEnable());
	}
#endif
}


void CPhantomGCView::OnUpdateBox2dUsage(CCmdUI *pCmdUI)
{
#ifdef USE_BOX
	pCmdUI->Enable(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->SetCheck(static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->m_dialogPtr->m_box2World.IsEnable());
	}
#endif
}


void CPhantomGCView::OnUpdateEncryptFile(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(!g_bIsFreeModeV);
	//if(GetViewManager()->GetActiveView())
	//	pCmdUI->SetCheck(GetViewManager()->GetActiveView()->IsEncrypt());
}

void CPhantomGCView::OnEncryptFile()
{
	//if(GetViewManager()->GetActiveView())
	//	GetViewManager()->GetActiveView()->SetEncrypt(!GetViewManager()->GetActiveView()->IsEncrypt());
}

void CPhantomGCView::OnShowMycode()
{
	/*AuthGUID guid;
	if(GetAuthGUID(&guid))
	{
		const unsigned char* szCH	=	(const unsigned char*)&guid;
		int len = sizeof(AuthGUID);
		char buf[512];
		strcpy(buf, "unsigned char g_myAuthCode[] = {");
		char temp[128];
		for(int i=0;i<len;i++)
		{
			sprintf(temp, "0x%02x", szCH[i]);
			strcat(buf, temp);
			if(i < (len - 1))
				strcat(buf, ",");
		}
		strcat(buf, "};");
		if( OpenClipboard())
		{
			EmptyClipboard();
			HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, strlen(buf) + 1 );
			if( hBlock )
			{
				char *pwszText = (char*)GlobalLock( hBlock );
				strcpy(pwszText, buf);
				GlobalUnlock( hBlock );
				SetClipboardData( CF_TEXT, hBlock );
			}
			CloseClipboard();
			if( hBlock )
				GlobalFree( hBlock );
		}
		MessageBox(Language(L"已经拷贝到剪贴板，粘贴到您C++代码里面就可以获得该用户授权码。"), Language(L"用户授权码获得"), MB_OK);
	}*/
}


void CPhantomGCView::OnUpdateShowMycode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);
}


UPDATE_EDIT_MODE(SetGridColor, GCSceneEditor3D);

BOOL	IsOriginalSize(int n)
{
	int size	=	1024;
	while(size > 0)
	{
		if(n == size)
			return true;
		size	=	size / 2;
	}
	return false;
}
int		GetOriginalSize(int n)
{
	int size	=	1024;
	while(size > 0)
	{
		if(n >= size)
			return size;
		size	=	size / 2;
	}
	return 1;
}
extern	void	enum_files(const char* path,std::vector<PFileName>& _out, int subFolderLevel, std::string* szFindType,int numFindTypes);
bool	lowercase(const char* c){
	int len = strlen(c);
	for(int i=0;i<len;i++)
	{
		if(c[i] >='A' && c[i]<='Z')
			return false;
	}
	return true;
}
	void			renameFolders(const char* szSubFolders)
	{
		char* strPath=(char*)szSubFolders;
		int len=(int)strlen(strPath);
		char strDir[512];
		for(int i=0;i<len;i++)
		{
			strDir[i]=strPath[i];
			if(strPath[i]!='\\' && strPath[i]!='/')
			{
				continue;
			}
			else
			{
				strDir[i]=0;
				char szTemp[512];
				strcpy(szTemp, strDir);
				strlwr(szTemp);
				if(strnicmp(strDir, GetProjectPath(), strlen(GetProjectPath())) == 0){
					if(!lowercase(strDir + strlen(GetProjectPath())))
						rename(strDir, szTemp);
				}
				strDir[i]='\\';
			}
		}
	}
	namespace Phantom{
		extern	void	ConvertToPng(const char* szFile, BOOL bRemoveOld = true);
	};
void CPhantomGCView::OnConvertAllTopng()
{
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"Phantom.txt|Phantom.txt||", this);
	if(dlg.DoModal() != IDOK)
		return;
	SetProjectPath();
	BOOL bDelete	=	false;
	if(MessageBox(Language(L"是否要删除原来的文件，请做好备份？"), L"直接删除，请做好备份!", MB_YESNO) == IDYES)
		bDelete		=	true;
	BOOL bDiv2		=	false;
	if(MessageBox(Language(L"是否进行同步缩小一半？"), L"把所有图片文件同步缩小一半!", MB_YESNO) == IDYES)
		bDiv2	=	true;
	BOOL bIs2 = false;
//	if(MessageBox(Language(L"是否自动改变大小？"), L"把所有图片文件自动改变大小!", MB_YESNO) == IDYES)
//		bIs2 =	true;
	char szPath[256], szDir[256], szFileName[256], szExt[128];
	_splitpath(A(dlg.GetPathName()), szPath, szDir, 0, 0);
	strcat(szPath, szDir);
	//
	std::vector<PFileName> allFiles;
	enum_files(szPath, allFiles, 128, 0, 0);
	for(int i=0;i<allFiles.size();i++)
	{
		char newName[512];
		strcpy(newName, allFiles[i].szRealName);
		strlwr(newName);
		::rename(allFiles[i].szRealName, newName);
		renameFolders(allFiles[i].szRealName);
	}
	//{
	//	std::vector<std::string> finds;
	//	finds.push_back(".rpgmesh");
	//	strcat(szPath, "mesh/npcs/");
	//	PFileList* fl	=	PhantomV1::System::enum_files(0, szPath, 128, &finds.at(0), finds.size());
	//	float scale = 0.18f;//缩放0.15
	//	if(fl){
	//		for(int i=0;i<fl->vFiles.size();i++){
	//			Phantom::MeshDataPtr m;
	//			if(Phantom::g_manager.LoadMesh(m, fl->vFiles[i].szRealName))
	//			{
	//				//m->m_fBaseScale = scale;
	//				m->m_bTurn180 = true;
	//				for(int i=0;i<m->m_actionMap.size();i++){
	//					Phantom::MeshActionRange* range = m->m_actionMap[i];
	//					if(strstr(range->GetName(), "idle"))
	//						range->m_actionType = Phantom::ActionType_Idle;
	//					else if(strstr(range->GetName(), "walk"))
	//					{
	//						range->m_actionType = Phantom::ActionType_Walk;
	//						if((int)range->m_fMoveOffset==0)
	//							range->m_fMoveOffset = m->GetObjectRegion().sphere.radius*0.3f;
	//					}
	//					else if(strstr(range->GetName(), "run"))
	//					{
	//						range->m_actionType = Phantom::ActionType_Run;
	//						if((int)range->m_fMoveOffset==0)
	//							range->m_fMoveOffset = m->GetObjectRegion().sphere.radius*0.6f;
	//					}
	//					else if(strstr(range->GetName(), "attack"))
	//						range->m_actionType = Phantom::ActionType_Attack;
	//					else if(strstr(range->GetName(), "dead")||strstr(range->GetName(), "die"))
	//						range->m_actionType = Phantom::ActionType_Die;
	//					else if(strstr(range->GetName(), "hurt")||strstr(range->GetName(), "hit"))
	//						range->m_actionType = Phantom::ActionType_Hit;
	//					
	//				}
	//				//m->ScaleAll(scale);
	//				m->SaveGroupData(fl->vFiles[i].szRealName);
	//			}
	//		}
	//	}
	//}
	//{
	//std::vector<std::string> finds;
	//finds.push_back(".rpgmesh");
	//strcat(szPath, "meshs/sceneobj/");
	//PFileList* fl	=	PhantomV1::System::enum_files(0, szPath, 128, &finds.at(0), finds.size());
	//if(fl)
	//{
	//	for(int i=0;i<fl->vFiles.size();i++)
	//	{
	//		const char* szFile	=	fl->vFiles[i].szRealName + strlen(GetProjectPath());
	//		Mesh* geom =	LoadGeometry(0, szFile, false, 0);
	//		if(geom)
	//		{
	//			while(geom->GetDataPtr()->GetActionCount()>0)
	//				geom->GetDataPtr()->RemoveAction(geom->GetDataPtr()->GetActionID(0));
	//			geom->GetDataPtr()->SaveGroup(szFile);
	//		}
	//		safe_release(geom);
	//	}
	//}
	//return;
	//}
	//{
	//	std::vector<std::string> finds;
	//	finds.push_back(".rpgmesh");
	//	strcat(szPath, "mesh/t/");
	//	PFileList* fl	=	PhantomV1::System::enum_files(0, szPath, 128, &finds.at(0), finds.size());
	//	float scale = 0.18f;//缩放0.15
	//	if(fl){
	//		for(int i=0;i<fl->vFiles.size();i++){
	//			//Phantom::MeshDataPtr m;
	//			Phantom::AutoPtr<Phantom::Mesh> m;
	//			if(Phantom::g_manager.LoadMesh(m, fl->vFiles[i].szRealName))
	//			{
	//				m->FrameMove();
	//				//if(m->RemoveBoneIfOne())
	//				//if(m->RecalcIdleBox())
	//				m->GetDataPtr()->m_bUseBoneIndex = false;
	//				m->GetDataPtr()->SaveGroupData(fl->vFiles[i].szRealName);
	//				//m->m_fBaseScale = scale;
	//				//m->SetEmissiveAll(Phantom::color4(0,0,0,0));
	//				//m->ScaleAll(scale);
	//				
	//			}
	//		}
	//	}
	//}
	
	std::vector<std::string> finds;
	finds.push_back(".dds");
	finds.push_back(".jpg");
	finds.push_back(".bmp");
	finds.push_back(".tga");
	//finds.push_back(".png");
	PMyFileList* fl	=	enum_files(szPath, 128, &finds.at(0), finds.size());
	if(fl)
	{
		for(int i=0;i<fl->vFiles.size();i++)
		{
			const char* szFile	=	fl->vFiles[i].szRealName;
			ConvertToPng(szFile);
			//ITexture* tex		=	LoadTexture(0, szFile, 0);
			//_splitpath(szFile, szPath, szDir, szFileName, szExt);
			//strcat(szPath, szDir);
			//strcat(szPath, szFileName);
			//strcat(szPath, ".png");
			//if(tex)// && bIs2)
			//{
			//	int width	=	tex->GetWidth();
			//	int height	=	tex->GetHeight();
			//	tex->Release();
			//	//
			//	if(IsOriginalSize(width) && IsOriginalSize(height) && stricmp(szExt, ".png") == 0 && !bDiv2)
			//		continue;
			//	width		=	GetOriginalSize(width);
			//	height		=	GetOriginalSize(height);
			//	if(bDiv2)
			//	{
			//		if(width > 16 && height > 16)
			//		{
			//			width	/=	2;
			//			height	/=	2;
			//		}
			//	}
			//	ITexture* tex		=	LoadTexture(0, szFile, width, height, 0, 0);
			//	if(bDelete)
			//		DeleteFile(W(fl->vFiles[i].szRealName));
			//	tex->SaveTextureTo(szPath);
			//}
			//else if(tex && !bIs2)
			//{
			//	char buf[512];
			//	strcpy(buf, fl->vFiles[i].szRealName);
			//	strlwr(buf);
			//	::rename(fl->vFiles[i].szRealName, buf);
			//}
		}
		destroyFileList(fl);
	}
	MessageBox(L"ok", L"ok", MB_OK);
}


void CPhantomGCView::OnUpdateConvertAllTopng(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);//!g_bIsFreeModeV);
}


void CPhantomGCView::OnUpdateGenCppCode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(true);
	}
}


void CPhantomGCView::OnGenCppCode()
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
		static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->GenericCPPCode();
}


void CPhantomGCView::OnUpdateGenericcodesGenericc(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(true);
	}
}


void CPhantomGCView::OnGenericcodesGenericc()
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
		static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->GenericCPPCode(true);
}


void CPhantomGCView::OnUpdateGenericcodesGenerichtmlreaderjsfiles(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(true);
	}
}


void CPhantomGCView::OnGenericcodesGenerichtmlreaderjsfiles()
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
		static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->GenericHtml5Code();
}


void CPhantomGCView::OnUpdateGenericcodesGenericeinitdialogcode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(false);
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
	{
		pCmdUI->Enable(true);
	}
}


void CPhantomGCView::OnGenericcodesGenericeinitdialogcode()
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
		static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->GenericUIECode();
}


void CPhantomGCView::OnMeshToScene()
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GeometryEditor)
	{
		for(int i=0;i<GetViewManager()->GetViewCount();i++)
		{
			if(GetViewManager()->GetView(i)->GetViewerType() == FileViewerType_GCSceneEditor3D)
			{
				static_cast<GCSceneEditor3D*>(GetViewManager()->GetView(i))->InsertGeometry(GetViewManager()->GetActiveView()->GetFileName());
				return;
			}
		}
	}
}


void CPhantomGCView::OnUpdateMeshToScene(CCmdUI *pCmdUI)
{
	for(int i=0;i<GetViewManager()->GetViewCount();i++)
	{
		if(GetViewManager()->GetView(i)->GetViewerType() == FileViewerType_GCSceneEditor3D)
		{
			pCmdUI->Enable(true);
			return;
		}
	}
	pCmdUI->Enable(false);
}


void CPhantomGCView::OnBrushCircle()
{
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	{
		static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_bBrushMode = 0;
		SetConfigInt("SceneBrush", 0);
	}
}


void CPhantomGCView::OnUpdateBrushCircle(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D);
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	{
		pCmdUI->SetCheck(static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_bBrushMode == 0);
	}
}


void CPhantomGCView::OnBrushBox()
{
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	{
		static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_bBrushMode = 1;
		SetConfigInt("SceneBrush", 1);
	}
}


void CPhantomGCView::OnUpdateBrushBox(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D);
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	{
		pCmdUI->SetCheck(static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_bBrushMode == 1);
	}
}


void CPhantomGCView::OnUpdateAutomoveMode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GeometryEditor);
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GeometryEditor && GetViewManager()->GetActiveView())
		pCmdUI->SetCheck(static_cast<GCViewGeometryEditor*>(GetViewManager()->GetActiveView())->m_bAutoMoveMode);
}


void CPhantomGCView::OnAutomoveMode()
{
	if(GetViewManager()->GetActiveView())
	{
		static_cast<GCViewGeometryEditor*>(GetViewManager()->GetActiveView())->m_bAutoMoveMode = !static_cast<GCViewGeometryEditor*>(GetViewManager()->GetActiveView())->m_bAutoMoveMode;
	}
}


void CPhantomGCView::OnExportUi()
{
	ReportFiles();
}


void CPhantomGCView::OnUpdateExportUi(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);
}

void CPhantomGCView::OnUserInfo()
{
}


void CPhantomGCView::OnUpdateShowObjectsDlg(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D);
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
		pCmdUI->SetCheck(static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_dlgObjects.IsWindowVisible());
}


void CPhantomGCView::OnShowObjectsDlg()
{
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D)
	{
		if(static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_dlgObjects.IsWindowVisible())
			static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_dlgObjects.ShowWindow(SW_HIDE);
		else
			static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->m_dlgObjects.ShowWindow(SW_SHOW);
	}
}


void CPhantomGCView::OnUpdateShowScript(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(GetViewManager()->GetActiveView()&&GetViewManager()->GetActiveView()->IsExistLua());
	//if(GetViewManager()->GetActiveView())
}


void CPhantomGCView::OnShowScript()
{
}


void CPhantomGCView::OnMoving(UINT fwSide, LPRECT pRect)
{
	CView::OnMoving(fwSide, pRect);
	if(this->m_wndViewer&&this->m_bIsPopup){
		CRect rc;
		GetWindowRect(&rc);
		m_wndViewer->OnViewSize(&rc);
	}
}


void CPhantomGCView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
	if(GetViewManager()->GetActiveView())
		GetViewManager()->GetActiveView()->OnSetFocus();
	// TODO: 在此处添加消息处理程序代码
}


void CPhantomGCView::OnColorSet()
{
	CDlgCodeColor cc(this);
	cc.DoModal();
}


void CPhantomGCView::OnReloadTables()
{
	Phantom::GetGameMgr()->ResearchAllFiles();
	GetCodeXmlManager()->ResetGameContent();
}


void CPhantomGCView::OnUpdateShowBox(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView() != NULL && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GeometryEditor);
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GeometryEditor && GetViewManager()->GetActiveView())
		pCmdUI->SetCheck(static_cast<GCViewGeometryEditor*>(GetViewManager()->GetActiveView())->m_bShowBox);
}


void CPhantomGCView::OnShowBox()
{
	if(GetViewManager()->GetActiveView())
		static_cast<GCViewGeometryEditor*>(GetViewManager()->GetActiveView())->m_bShowBox = !static_cast<GCViewGeometryEditor*>(GetViewManager()->GetActiveView())->m_bShowBox;
}



void CPhantomGCView::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView()&&GetViewManager()->GetActiveView()->IsCanUndo());
}


void CPhantomGCView::OnEditUndo()
{
	GetViewManager()->GetActiveView()->OnUndo();
}


void CPhantomGCView::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetViewManager()->GetActiveView()&&GetViewManager()->GetActiveView()->IsCanRedo());
}


void CPhantomGCView::OnEditRedo()
{
	GetViewManager()->GetActiveView()->OnRedo();
}


void CPhantomGCView::OnUpdateBuyResource(CCmdUI *pCmdUI)
{
}


void CPhantomGCView::OnBuyResource()
{
}

BOOL IsUIEditorMode()
{
	if(GetViewManager()->GetActiveView() && GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_UI)
		return static_cast<GCViewUIEditor*>(GetViewManager()->GetActiveView())->GetEditorMode()>0;
	return false;
}

void CPhantomGCView::OnUpdateExportPak(CCmdUI *pCmdUI)
{
#ifdef _DEBUG
	pCmdUI->Enable(GetViewManager()->GetActiveView()&&GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GeometryEditor);
#else
	pCmdUI->Enable(false);
#endif
}


void CPhantomGCView::OnExportPak()
{
#ifdef _DEBUG
	if(GetViewManager()->GetActiveView()&&GetViewManager()->GetActiveView()->IsExportPak())//&&GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GeometryEditor)
	{
		GetViewManager()->GetActiveView()->ExportPak();//m_pGeometryPtr->GetDataPtr()->ExportPak();//导出成pak,然后可以在打开的时候LoadMesh自动判断是不是用pak方式，并且使用授权码打开
	}
#endif
}


void CPhantomGCView::OnConvertAllFbx()
{
	if(GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GeometryEditor && GetViewManager()->GetActiveView())
		static_cast<GCViewGeometryEditor*>(GetViewManager()->GetActiveView())->ExportFbx();
}
