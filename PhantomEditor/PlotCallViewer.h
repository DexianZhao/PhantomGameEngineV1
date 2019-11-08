#pragma once
#include "afxwin.h"
#include "GameDirectorRun.h"
#include "GCViewTreeBase.h"
#include "ListBoxIcon.h"
#include "ImageButton.h"
#include "DialogBase.h"

// CPlotCallViewer 对话框

struct	PlotCallFrame
{
	enum FrameCall
	{
		FrameCall_None	=	0,		//快速访问控制
		FrameCall_Script,			//Lua脚本控制
		FrameCall_VariantDefine,	//变量赋值
		FrameCall_VariantPropCall,	//调用属性
	};
};

class GCGameProject;

class CPlotCallViewer : public CDialogBase
{
	DECLARE_DYNAMIC(CPlotCallViewer)

public:
	CPlotCallViewer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlotCallViewer();

	VOID				SetPlotCall(CPlotState* c);
public:
	GCGameProject*		m_gamePtr;
	CPlotState*			m_plotCall;
// 对话框数据
	enum { IDD = IDD_RENDER_PLOTCALL };
	//
	VOID				EnableToolBar();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnInserScript();
	afx_msg void OnInsertVariantSet();
	afx_msg void OnInsertPropCall();
	afx_msg void OnRemove();


	CListBoxIcon m_listCall;
	afx_msg void OnLbnSelchangeIconList();
	CImage			m_imageButton;
	CImageButton	m_btVariantSet;
	CImageButton	m_btPropertyCall;
	CImageButton	m_btLuaScript;
	CImageButton	m_btRemove;
};
