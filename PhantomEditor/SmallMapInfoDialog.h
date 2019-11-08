#pragma once
#include "afxwin.h"
#include <map>
#include <string>
#include "DialogBase.h"
#include "PhantomProject.h"

// CSmallMapInfoDialog 对话框

class CSmallMapInfoDialog : public CDialogBase
{
	DECLARE_DYNAMIC(CSmallMapInfoDialog)

public:
	CSmallMapInfoDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSmallMapInfoDialog();

// 对话框数据
	enum { IDD = IDD_SMALL_MAP_DLG };

	VOID	RebuildIncludes();
	std::map<int, std::string>		m_geometryTypes;
	IBigScene3D		*	m_scene3D;
	//
	VOID		OnChangeScene();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CButton m_chkSmallMapEnable;
	CButton m_chkSmallMapRenderObjects;
	CComboBox m_cbIncludes;
	CListBox m_lbIncludes;
	CButton m_btAddInclude;
	CButton m_btRemoveInclude;
	CEdit m_edSmalMapPlaneDistance;
	CEdit m_edSmallMapRebuildInterval;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedAddInclude();
	afx_msg void OnBnClickedRemoveInclude();
	afx_msg void OnBnClickedAllAll();
	afx_msg void OnBnClickedSmallMapEnable();
	afx_msg void OnBnClickedSmallMapRenderObjects();
	afx_msg void OnBnClickedSetMpd();
	afx_msg void OnBnClickedSetMri();
};

CSmallMapInfoDialog*	GetSmallMapInfoDlg();
