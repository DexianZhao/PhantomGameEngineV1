/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// ViewLayerImages.cpp : 实现文件
//

#include "stdafx.h"
#include ".\ViewLayerImages.h"
#include "GCSceneEditor3D.h"

// CViewLayerImages 对话框

IMPLEMENT_DYNAMIC(CViewLayerImages, CDialogBase)
CViewLayerImages::CViewLayerImages(CWnd* pParent /*=NULL*/)
	: CDialogBase(CViewLayerImages::IDD, pParent)
{
	memset(m_layerImages, 0, sizeof(m_layerImages));
	m_imageViewSize			=	64;
	m_imageViewSelect		=	0;
	m_imageViewSelectMove	=	-1;
	m_pIILCallback			=	0;
}

CViewLayerImages::~CViewLayerImages()
{
}

void CViewLayerImages::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_MOVE_ENABLE, m_chkMoveEnable);
	CDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CViewLayerImages, CDialogBase)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_MOVE_ENABLE, &CViewLayerImages::OnBnClickedMoveEnable)
END_MESSAGE_MAP()


// CViewLayerImages 消息处理程序

void CViewLayerImages::OnBnClickedOk()
{
}

void CViewLayerImages::OnBnClickedCancel()
{
}

VOID CViewLayerImages::SetImageFile(INT nIndex, const char* szFile, bool bMoveEnable)	//设置图片
{
	if(nIndex < 0 || nIndex >= MAX_IMAGE_VIEW)return;
	m_layerImages[nIndex].imgPtr		=	m_imageListView.m_imageManager.findImageByFile( szFile );
	m_layerImages[nIndex].bMoveEnable	=	bMoveEnable;
}

VOID CViewLayerImages::SetImageName(INT nIndex, const char* szName, bool bIsImage, const char* szDescription, bool bMoveEnable)	//设置图片
{
	if(nIndex < 0 || nIndex >= MAX_IMAGE_VIEW)return;
	m_layerImages[nIndex].viewName	=	szName;
	m_layerImages[nIndex].description	=	szDescription ? szDescription : szName;
	m_layerImages[nIndex].bIsImage	=	bIsImage;
	m_layerImages[nIndex].bMoveEnable	=	bMoveEnable;
}

BOOL CViewLayerImages::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	m_imageListView.Create(m_imageListView.IDD, this);
	m_imageListView.ShowWindow(SW_HIDE);
	int nImage	=	0;
	while(nImage < MAX_IMAGE_VIEW)
	{
		const int	widthCount	=	5;
		this->m_layerImages[nImage].rcImage.SetRect((nImage % widthCount) * this->m_imageViewSize + (nImage % widthCount) * 10, (nImage / widthCount) * this->m_imageViewSize + (nImage / widthCount) * 10, 0, 0);
		this->m_layerImages[nImage].rcImage.right	=	this->m_layerImages[nImage].rcImage.left + m_imageViewSize;
		this->m_layerImages[nImage].rcImage.bottom	=	this->m_layerImages[nImage].rcImage.top + m_imageViewSize;
		nImage++;
	}
	m_toolTip.AddTool(this, L"Test");
	return TRUE;
}

void CViewLayerImages::OnPaint()
{
	CPaintDC dc(this);
	CFont* pOld	=	0;
	if(GetFont())
		pOld	=	dc.SelectObject(GetFont());
	for(int i=0;i<MAX_IMAGE_VIEW;i++)
	{
		ViewImageData& data	=	m_layerImages[i];
		if(data.viewName.empty())
			continue;
		ImageItem* pItem	=	data.imgPtr;
		if(!pItem)
			pItem	=	m_imageListView.m_imageManager.getDefaultImage();
		HDC dcSrc	=	pItem->img.GetDC();
		//
		::SetStretchBltMode( dc.m_hDC, COLORONCOLOR );
		::SetStretchBltMode( dcSrc, COLORONCOLOR );
		::StretchBlt(dc.m_hDC, data.rcImage.left, data.rcImage.top, data.rcImage.Width(), data.rcImage.Height(), dcSrc, 0, 0, pItem->img.GetWidth(), pItem->img.GetHeight(), SRCCOPY);
		//
		if(i == m_imageViewSelect)
			dc.Draw3dRect(&data.rcImage, RGB(255,0,0), RGB(255,0,0));
		else
			dc.Draw3dRect(&data.rcImage, RGB(0,0,0), RGB(0,0,0));
		if(!data.bMoveEnable){
			CPen pen(PS_SOLID, 2, RGB(255,0,0));
			CPen* old = dc.SelectObject(&pen);
			dc.MoveTo(data.rcImage.right - 16, data.rcImage.bottom-16);
			dc.LineTo(data.rcImage.right, data.rcImage.bottom);
			dc.MoveTo(data.rcImage.right - 16, data.rcImage.bottom);
			dc.LineTo(data.rcImage.right, data.rcImage.bottom-16);
			dc.SelectObject(old);
		}
		dc.SetBkMode(TRANSPARENT);
		//
		::DrawTextA(dc.m_hDC, data.viewName.c_str(), -1, &data.rcImage, DT_CENTER | DT_BOTTOM);
		//
		pItem->img.ReleaseDC();
		//
	}
	if(pOld)
		dc.SelectObject( pOld );
}

//void CViewLayerImages::OnSelectImage(ImageItem* pItem, INT nParameter)
//{
//	this->m_layerImages[nParameter].imgPtr	=	pItem;
//	this->Invalidate();
//}

void CViewLayerImages::OnLButtonDown(UINT nFlags, CPoint point)
{
	selectImage(point);
	CDialogBase::OnLButtonDown(nFlags, point);
}

void	CViewLayerImages::selectImage(CPoint point)
{
	if(!this->m_pIILCallback)
		return;
	if(!m_pIILCallback->m_scene3D)
		return;
	m_imageViewSelect	=	-1;
	for(int i=0;i<MAX_IMAGE_VIEW;i++)
	{
		ViewImageData& data	=	m_layerImages[i];
		if(data.viewName.empty())
			continue;
		if(data.rcImage.PtInRect(point))
		{
			m_imageViewSelect	=	i;
			m_chkMoveEnable.SetCheck(m_layerImages[i].bMoveEnable);
			m_pIILCallback->OnSelectLayer(m_imageViewSelect);
			this->Invalidate();
			break;
		}
	}
}

void CViewLayerImages::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(!m_pIILCallback)
		return;
	if(!m_pIILCallback->m_scene3D)
		return;
	selectImage(point);
	if(m_imageViewSelect >= 0 && m_pIILCallback)
	{
		POINT pt	=	point;
		ClientToScreen(&pt);
		CRect rcP;
		m_imageListView.GetClientRect(&rcP);
		m_imageListView.showPopup(m_layerImages[m_imageViewSelect].imgPtr, pt.x, pt.y, m_pIILCallback, m_imageViewSelect);
	}
	else
		CDialogBase::OnRButtonUp(nFlags, point);
}

BOOL CViewLayerImages::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CDialogBase::OnEraseBkgnd(pDC);
}

void CViewLayerImages::OnMouseMove(UINT nFlags, CPoint point)
{
	CDialogBase::OnMouseMove(nFlags, point);
	//
	if(!m_pIILCallback)
		return;
	if(!m_pIILCallback->m_scene3D)
		return;
	for(int i=0;i<MAX_IMAGE_VIEW;i++)
	{
		ViewImageData& data	=	m_layerImages[i];
		if(data.viewName.empty())
			continue;
		if(data.rcImage.PtInRect(point))
		{
			if(m_imageViewSelectMove != i)
			{
				static CPoint pos(0,0);
				CPoint point2 = point;
				ClientToScreen(&point2);
				if(pos.x != point2.x || pos.y != point2.y)
				{
					this->m_toolTip.UpdateTipText(W(data.description.c_str()), this);
					pos = point2;
				}
			}
			break;
		}
	}
}


void CViewLayerImages::OnBnClickedMoveEnable()
{
	if(m_imageViewSelect<0||m_imageViewSelect>=MAX_IMAGE_VIEW)
		return;
	m_layerImages[m_imageViewSelect].bMoveEnable = !m_layerImages[m_imageViewSelect].bMoveEnable;
	Phantom::SceneTextureRuntime* r = m_pIILCallback->m_scene3D->GetLayerTextureData(m_imageViewSelect);
	if(r)
	{
		if(this->m_chkMoveEnable.GetCheck())
			r->bMoveEnable = 2;//m_layerImages[m_imageViewSelect].bMoveEnable;
		else
			r->bMoveEnable = 0;
		m_pIILCallback->m_scene3D->RebuildAllMoveGrids();
	}
	this->Invalidate();
}
