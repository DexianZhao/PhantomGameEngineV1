#pragma once
#include "afxwin.h"
#include "ListBoxIcon.h"
#include "ImageButton.h"
#include "DialogBase.h"
#include "PhantomManager.h"

// CDlgUIEventSet 对话框

class CDlgUIEventSet : public CDialogBase
{
	DECLARE_DYNAMIC(CDlgUIEventSet)

public:
	CDlgUIEventSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgUIEventSet();

	VOID			SetControlPtr(Phantom::UIControl* c);
	//
	VOID			SetUIEvent(INT nEventID, INT nDialogID, INT nControlID, const wchar_t* szName);
	enum { IDD = IDD_UI_EVENT_SET };
	BOOL			OnInitDialog();
protected:
	INT				m_nEventID;
	INT				m_nDialogID;
	INT				m_nControlID;
	wchar_t			m_name[128];
	//
	Phantom::UIControl*		m_controlPtr;
// 对话框数据
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CImage			m_imgButton;
	CListBoxIcon	m_lbEventLinks;
	CImageButton	m_btInsert;
	CImageButton	m_btRemove;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedInsertEventcall();
	afx_msg void OnBnClickedRemoveSelect();
	CListBoxIcon	m_lbEvent;
	afx_msg void OnLbnSelchangeEventList();
	afx_msg void OnLbnDblclkIconList();
};
