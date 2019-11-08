#pragma once
#include "afxwin.h"
#include "ListBoxIcon.h"
#include "ImageButton.h"
#include "DialogBase.h"

class GCGameProject;
class CPlotVariantDialog : public CDialogBase
{
	DECLARE_DYNAMIC(CPlotVariantDialog)

public:
	CPlotVariantDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlotVariantDialog();

	VOID				SetActionPtr(CPlotAction* act, CPlotVariant* sel = 0);
public:
	CPlotAction*		m_actionPtr;
	GCGameProject*		m_gamePtr;
// 对话框数据
	enum { IDD = IDD_PLOT_VARIANTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CImage			m_imageButton;
	CImageButton	m_btVariant;
	CImageButton	m_btRemove;

	CListBoxIcon m_lists;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLbnSelchangeIconList();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPlotcallInsertvariantset();
	afx_msg void OnPlotcallRemove();
	virtual BOOL OnInitDialog();
};
