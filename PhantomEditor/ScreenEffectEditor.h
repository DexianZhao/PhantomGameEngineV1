#pragma once
#include "afxwin.h"
#include "DialogBase.h"

class CScreenEffectEditor : public CDialogBase
{
	DECLARE_DYNAMIC(CScreenEffectEditor)

public:
	CScreenEffectEditor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CScreenEffectEditor();

// 对话框数据
	enum { IDD = IDD_SCREEN_EFFECT };

	VOID		RefreshMethods();
	VOID		RefreshPosts();
	VOID		RefreshVariants();
	VOID		RefreshTextures();
	CString		m_strFileName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoadMethodList();
	afx_msg void OnBnClickedSaveMethodList();
	afx_msg void OnBnClickedSaveMethodList2();
	afx_msg void OnBnClickedAddMethod();
	afx_msg void OnBnClickedDelMethod();
	afx_msg void OnBnClickedSetActive();
	afx_msg void OnLbnSelchangeMethodList();
	afx_msg void OnLbnSelchangePostItems();
	afx_msg void OnBnClickedAddMethodItem();
	afx_msg void OnBnClickedDeleteMethodItem();
	afx_msg void OnBnClickedMethodEnable();
	afx_msg void OnBnClickedSetMethodName();
	afx_msg void OnLbnSelchangeMethodVariantValue();
	afx_msg void OnBnClickedSetVariantName();
	afx_msg void OnLbnSelchangeTextureList();
	afx_msg void OnBnClickedSetTexture();
	CListBox m_lbMethods;
	CListBox m_lbPosts;
	CComboBox m_cbQPosts;
	CButton m_chkPostEnable;
	afx_msg void OnCbnSelchangePostLayer();
	CEdit m_edMethodName;
	CListBox m_lbVariants;
	CEdit m_edX;
	CEdit m_edY;
	CEdit m_edZ;
	CEdit m_edW;
	CScrollBar m_sbX;
	CScrollBar m_sbY;
	CScrollBar m_sbZ;
	CScrollBar m_sbW;
	CListBox m_lbTextures;
	afx_msg void OnBnClickedCloneMethod();
	CEdit m_edTextureFile;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CComboBox m_cbSupports;
	afx_msg void OnBnClickedUpper();
	afx_msg void OnBnClickedDownler();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeQrenderList();
	afx_msg void OnBnClickedAddMethodItem2();
};
