/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "ViewImageManager.h"
#include "ImageListViewDLG.h"

#define	MAX_IMAGE_VIEW		9
class CViewLayerImages : public CDialogBase
{
	DECLARE_DYNAMIC(CViewLayerImages)

public:
	CViewLayerImages(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CViewLayerImages();

	VOID						SetImageFile(INT nIndex, const char* szFile, bool bMoveEnable);	//设置图片
	VOID						SetImageName(INT nIndex, const char* szName, bool bIsImage, const char* szDescription, bool bMoveEnable);	//设置图片

	void						selectImage(CPoint point);
	GCSceneEditor3D*			m_pIILCallback;
	//void						OnSelectImage(ImageItem* pItem, INT nParameter);
// 对话框数据
	enum { IDD = IDD_LAYER_IMAGES };

	struct ViewImageData
	{
		CRect			rcImage;
		ImageItem*		imgPtr;
		std::string		viewName;
		std::string		description;
		bool			bIsImage;
		bool			bMoveEnable;
	};
	ViewImageData		m_layerImages[MAX_IMAGE_VIEW];
	int					m_imageViewSize;
	int					m_imageViewSelect;
	int					m_imageViewSelectMove;
	CImageListViewDLG	m_imageListView;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CButton		m_chkMoveEnable;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedMoveEnable();
};
