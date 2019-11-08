/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "atlimage.h"
#include <vector>
#include "ViewImageManager.h"
#include "resource.h"
#include "DialogBase.h"

// CImageListViewDLG 对话框


class	CImageListViewDLG;
class	GCSceneEditor3D;

class CImageListViewDLG : public CDialogBase
{
	DECLARE_DYNAMIC(CImageListViewDLG)

public:
	CImageListViewDLG(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CImageListViewDLG();

	int							m_imageSize;

	void						setSelect(int textureID);
	void						setSelect(const char* strFile);
	
	//弹出选择对话框
	void						showPopup(ImageItem* pItem, int x, int y, GCSceneEditor3D* cb, INT nParameter);
	struct ImageViewData
	{
		CRect					rcImage;
		ImageItem*				pImageItem;
	};
	int							m_imageViewSelect;
	std::vector<ImageViewData>	m_imageViews;
// 对话框数据
	void						rebuildImageViews();
	enum { IDD = IDD_IMAGE_VIEW };

	INT							m_nParameter;
	GCSceneEditor3D*			m_iClickCallback;
	CViewImageManager			m_imageManager;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnShowMovedisable();
	afx_msg void OnUpdateShowMovedisable(CCmdUI *pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL OnInitDialog();
};
