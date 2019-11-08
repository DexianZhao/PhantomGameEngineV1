/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#ifndef __SUB_GEOMETRY_EDITOR_DLG_H__
#define	__SUB_GEOMETRY_EDITOR_DLG_H__

#pragma once
#include "resource.h"
#include "afxwin.h"
#include "MyEdit.h"
#include "afxcolorbutton.h"
#include "DialogBase.h"
#include "PhantomManager.h"

// CSubGeometryEditorDlg 对话框

class GCViewGeometryEditor;

class CSubGeometryEditorDlg : public CDialogBase
{
	DECLARE_DYNAMIC(CSubGeometryEditorDlg)

public:
	CSubGeometryEditorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSubGeometryEditorDlg();

// 对话框数据
	enum { IDD = IDD_SUB_GEOMETRY_DLG };

	GCViewGeometryEditor*		m_geomEditor;
	Phantom::Mesh*				m_geomRender;
	Phantom::SubMeshData*		m_geomData;
	Phantom::MeshShaderProject*	m_geomShader;
	VOID				SetGeometryData(Phantom::SubMeshData*	data, Phantom::Mesh* render, Phantom::MeshShaderProject*	shader);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CComboBox m_cbShaderVariants;
	CListBox m_lbExistVariants;
	CMyEdit2 m_edValueX;
	CMyEdit2 m_edValueY;
	CMyEdit2 m_edValueZ;
	CMyEdit2 m_edValueW;
	afx_msg void OnLbnSelchangeGeomGeometryVariant();
	afx_msg void OnBnClickedGeomAddVariant();
	afx_msg void OnBnClickedGeomDeleteVariant();
	afx_msg void OnBnClickedGeomSetVariantValue();
	afx_msg void OnBnClickedValueRgb();
	CMFCColorButton m_colorValue;
};

#endif
