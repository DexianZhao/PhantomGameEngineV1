#pragma once
#include "DialogBase.h"
#include "afxwin.h"

// CSceneCreateDlg 对话框

class CSceneCreateDlg : public CDialogBase
{
	DECLARE_DYNAMIC(CSceneCreateDlg)

public:
	CSceneCreateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSceneCreateDlg();
	float			m_fGridSize;
	INT				m_nMapSizeLevel;
	VOID			RebuildInfoView();
// 对话框数据
	enum { IDD = IDD_SCENE_CreateScene };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	CEdit m_edGridSize;
	CComboBox m_cbMapSizeLevel;
	afx_msg void OnCbnSelchangeMapSizeLevel();
	afx_msg void OnEnChangeGridSize();
};
