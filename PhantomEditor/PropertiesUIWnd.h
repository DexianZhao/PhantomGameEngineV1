/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "GCViewBase.h"

// CPropertiesUIWnd

class CPropertiesUIWnd : public CDockablePane
{
	DECLARE_DYNAMIC(CPropertiesUIWnd)

public:
	CPropertiesUIWnd();
	virtual ~CPropertiesUIWnd();
	
	VOID				SetEditorMode(FileViewerType type);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
