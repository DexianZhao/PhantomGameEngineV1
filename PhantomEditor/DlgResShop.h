#pragma once
#include <vector>
#include <resource.h>
// CDlgResShop 对话框
class	HomeResItem
{
public:
	HomeResItem();
	VOID		ResetState();
	VOID		SetRect(CRect rc);
public:
	int			m_itemID;
	CImage		m_image;
	int			m_state;
	CString		m_name;
	CString		m_desc;
	CString		m_payment;
	CString		m_power;
	CString		m_imgFile;
	FileName	m_pakFile;
	FileName	m_filename;
	int			m_nState;
	BOOL		m_bIsDownload;
	enum{
		OnBuy_None	=	0,
		OnBuy_Use	=	1,
		OnBuy_BuyVip2,
		OnBuy_BuyPayment,
		OnBuy_BuyDownload,
		OnBuy_UseToGame,//使用到游戏内部
	};
	//
	int			m_buyState;
	CString		m_buy;
	CRect		m_rcName;
	CRect		m_rcWindow;
	CRect		m_rcDetail;
	CRect		m_rcImage;
	CRect		m_rcBuy;
	CRect		m_rcVb;
	BOOL		m_bResize;
};
class CDlgResShop : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgResShop)

public:
	CDlgResShop(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgResShop();

	std::vector<HomeResItem*>		m_items;
	HomeResItem*					m_downDetailItem;
	HomeResItem*					m_downBuyItem;
	int			m_nSelect;
	//
	VOID		UpdateTitle(const wchar_t* t);
// 对话框数据
	enum { IDD = IDD_SHOP_LIST };
	int			m_nBeginY;
	CImage		m_imgGold;
	CImage		m_back;
	CImage		m_imgButton;
	CTabCtrl		m_tab;
	//
	VOID		Update();
	VOID		Resize();
	VOID		ResetScroll();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
