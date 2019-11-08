/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// TrackEditorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GCTrackEditor.h"
#include "afxdialogex.h"
#include "GCViewBase.h"

#define	TRACK_HEIGHT		20
#define	TRACK_TABLE_HEIGHT	20


// GCTrackEditor 对话框

IMPLEMENT_DYNAMIC(GCTrackEditor, CDialogEx)

GCTrackEditor::GCTrackEditor(CWnd* pParent /*=NULL*/)
	: CDialogEx(GCTrackEditor::IDD, pParent)
{
	m_nFrameCount	=	MAX_ANIMATION_KEY;
	m_fPlayIndex	=	0;
	m_keySize.SetSize(5, 10);
	m_trackStatus	=	TrackStatus_Pause;
	m_bDownTrack	=	false;
	m_baseView		=	0;
	m_fPlaySpeed	=	1.0f;
	m_lastTabSelect	=	0;
}

GCTrackEditor::~GCTrackEditor()
{
	for(int i=0;i<m_saveEditors.size();i++)
		delete m_saveEditors[i];
}

BOOL	GCTrackEditor::IsCanEditor(Phantom::FloatAnimation* f)
{
	for(int i=0;i<m_editors.size();i++)
	{
		if(m_editors[i]->ctrl.IsCanEditor(f))
			return true;
	}
	return false;
}

VOID	GCTrackEditor::SetPlaySpeed(float f)
{
	this->m_fPlaySpeed	=	f;
}

VOID	GCTrackEditor::SetModifyed()
{
	if(m_baseView)
		m_baseView->SetModifyed(TRUE);
}

void GCTrackEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_EDITOR, m_tabEditor);
	DDX_Control(pDX, IDC_PLAY_TRACK, m_btPlay);
	DDX_Control(pDX, IDC_PLAYBACK_TRACK, m_btPlayBack);
	DDX_Control(pDX, IDC_QPLAY_TRACK, m_btQuickPlay);
	DDX_Control(pDX, IDC_QPLAYBACK_TRACK, m_btQPlayBack);
	DDX_Control(pDX, IDC_STOP_TRACK, m_btStop);
	DDX_Control(pDX, IDC_PAUSE_TRACK, m_btPause);
	DDX_Control(pDX, IDC_FLOAT_VALUE, m_edValue);
	DDX_Control(pDX, IDC_SET_VALUE, m_btSetValue);
	DDX_Control(pDX, IDC_TRACK_ENABLE, m_chkEnable);
	//
	DDX_Control(pDX, IDC_TRACK_SEL_X, m_chkX);
	DDX_Control(pDX, IDC_TRACK_SEL_Y, m_chkY);
	DDX_Control(pDX, IDC_TRACK_SEL_Z, m_chkZ);
	//
	DDX_Control(pDX, IDC_FLOAT_MIN_VALUE, m_edMinValue);
	DDX_Control(pDX, IDC_FLOAT_MAX_VALUE, m_edMaxValue);
	DDX_Control(pDX, IDC_SET_MINMAX_VALUE, m_btSetMinMaxValue);
}


BEGIN_MESSAGE_MAP(GCTrackEditor, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_EDITOR, &GCTrackEditor::OnTcnSelchangeTabEditor)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &GCTrackEditor::OnBnClickedNull)
	ON_BN_CLICKED(IDCANCEL, &GCTrackEditor::OnBnClickedNull)
	ON_BN_CLICKED(IDC_PLAY_TRACK, &GCTrackEditor::OnBnClickedPlayTrack)
	ON_BN_CLICKED(IDC_PLAYBACK_TRACK, &GCTrackEditor::OnBnClickedPlaybackTrack)
	ON_BN_CLICKED(IDC_QPLAY_TRACK, &GCTrackEditor::OnBnClickedQplayTrack)
	ON_BN_CLICKED(IDC_QPLAYBACK_TRACK, &GCTrackEditor::OnBnClickedQplaybackTrack)
	ON_BN_CLICKED(IDC_STOP_TRACK, &GCTrackEditor::OnBnClickedStopTrack)
	ON_BN_CLICKED(IDC_PAUSE_TRACK, &GCTrackEditor::OnBnClickedPauseTrack)
	ON_BN_CLICKED(IDC_TRACK_ENABLE, &GCTrackEditor::OnBnClickedEnable)
	ON_BN_CLICKED(IDC_SET_MINMAX_VALUE, &GCTrackEditor::OnBnClickedMinMax)
	ON_BN_CLICKED(IDC_TRACK_SEL_X, &GCTrackEditor::OnBnClickedSel)
	ON_BN_CLICKED(IDC_TRACK_SEL_Y, &GCTrackEditor::OnBnClickedSel)
	ON_BN_CLICKED(IDC_TRACK_SEL_Z, &GCTrackEditor::OnBnClickedSel)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_SET_VALUE, &GCTrackEditor::OnBnClickedSetValue)
END_MESSAGE_MAP()


VOID GCTrackEditor::OnSelectTab()
{
	int sel	=	m_tabEditor.GetCurSel();
	for(int i=0;i<this->m_editors.size();i++)
	{
		if(m_editors[i]->nTabItem == sel)
		{
			m_editors[i]->ctrl.ShowWindow(SW_SHOW);
			m_editors[i]->ctrl.MoveWindow(&m_rectEditor);
			m_editors[i]->ctrl.InvalidateFrames();
			m_chkX.EnableWindow(m_editors[i]->ctrl.m_floats[0].floatPtr != NULL);
			m_chkY.EnableWindow(m_editors[i]->ctrl.m_floats[1].floatPtr != NULL);
			m_chkZ.EnableWindow(m_editors[i]->ctrl.m_floats[2].floatPtr != NULL);
			m_chkX.SetCheck(m_editors[i]->ctrl.m_floats[0].bCanSelect);
			m_chkY.SetCheck(m_editors[i]->ctrl.m_floats[1].bCanSelect);
			m_chkZ.SetCheck(m_editors[i]->ctrl.m_floats[2].bCanSelect);
			//
			if(m_editors[i]->ctrl.m_floats[0].floatPtr)
			{
				m_btSetMinMaxValue.EnableWindow(true);
				m_edMinValue.EnableWindow(true);
				m_edMaxValue.EnableWindow(true);
				wchar_t buf[128];
				swprintf(buf, L"%.f", m_editors[i]->ctrl.m_floats[0].floatPtr->GetMinValue());
				m_edMinValue.SetWindowTextW(buf);
				swprintf(buf, L"%.f", m_editors[i]->ctrl.m_floats[0].floatPtr->GetMaxValue());
				m_edMaxValue.SetWindowTextW(buf);
			}
			else
			{
				m_btSetMinMaxValue.EnableWindow(false);
				m_edMinValue.EnableWindow(false);
				m_edMaxValue.EnableWindow(false);
			}
			m_chkEnable.SetCheck(m_editors[i]->ctrl.IsWindowEnabled());
		}
		else
			m_editors[i]->ctrl.ShowWindow(SW_HIDE);
	}
}

void GCTrackEditor::OnTcnSelchangeTabEditor(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	OnSelectTab();
}

VOID GCTrackEditor::ClearAnimationPtr()
{
	for(int i=0;i<m_editors.size();i++)
	{
		m_editors[i]->ctrl.SetCallback(0, 0);
		m_editors[i]->ctrl.SetCallback(0, 1);
		m_editors[i]->ctrl.SetCallback(0, 2);
	}
}

VOID GCTrackEditor::BeginSet()
{
	for(int i=0;i<m_saveEditors.size();i++)
	{
		m_saveEditors[i]->ctrl.ShowWindow(SW_HIDE);
	}
	m_lastTabSelect	=	m_tabEditor.GetCurSel();
	m_editors.clear();
	m_tabEditor.DeleteAllItems();
}

VOID GCTrackEditor::EndSet()
{
	for(int i=0;i<m_editors.size();i++)
		m_editors[i]->ctrl.Invalidate();
	if(m_lastTabSelect > m_tabEditor.GetItemCount())
		m_lastTabSelect = m_tabEditor.GetItemCount()-1;
	m_tabEditor.SetCurSel(m_lastTabSelect);
	OnSelectTab();
}

VOID GCTrackEditor::SetAnimationPtr(int id, Phantom::FloatAnimation* ani1, Phantom::FloatAnimation* ani2, Phantom::FloatAnimation* ani3, const wchar_t* szName)
{
	for(int i=0;i<m_saveEditors.size();i++)
	{
		if(m_saveEditors[i]->id == id)
		{
			if(!ani1 && !ani2 && !ani3)
				break;
			BOOL bVisible[3];
			bVisible[0]	=	bVisible[1]	=	bVisible[2]	=	true;
			if(ani1&&!ani1->IsVisible())bVisible[0]	=	false;
			if(ani2&&!ani2->IsVisible())bVisible[1]	=	false;
			if(ani3&&!ani3->IsVisible())bVisible[2]	=	false;
			if(!bVisible[0] && !bVisible[1] && !bVisible[2])
				break;
			this->m_nFrameCount	=	ani1->GetFrameCount();
			if(szName)
				wcscpy(m_saveEditors[i]->szName, szName);
			//if(ani1->GetFloatName()[0])
			//	wcscpy(m_saveEditors[i]->szName, Language(W(ani1->GetFloatName())));
			//
			m_saveEditors[i]->ctrl.EnableWindow(ani1->IsEnabled());
			//
			m_saveEditors[i]->ctrl.SetCallback(ani1, 0, true);
			m_saveEditors[i]->ctrl.SetCallback(ani2, 1, true);
			m_saveEditors[i]->ctrl.SetCallback(ani3, 2, true);
			m_saveEditors[i]->ctrl.m_bIsColor	=	ani1 ? ani1->IsColorAni() : 0;
			m_editors.push_back(m_saveEditors[i]);
			m_saveEditors[i]->nTabItem	=	m_tabEditor.InsertItem(m_editors.size(), m_saveEditors[i]->szName);//m_saveEditors[i]->szName);
			break;
		}
	}
}

VOID GCTrackEditor::AddEditor(const wchar_t* name, int _id, float fMin, float fMax, bool bIsColor)
{
	for(int i=0;i<m_saveEditors.size();i++)
	{
		if(m_saveEditors[i]->id == _id)
		{
			wcscpy(m_saveEditors[i]->szName, name);
			return;
		}
	}
	TrackEditorKey* te	=	new TrackEditorKey;
	te->ctrl.Create(te->ctrl.IDD, this);
	te->ctrl.ShowWindow(SW_HIDE);
	te->id	=	_id;
	wcscpy(te->szName, name);
	te->nTabItem	=	m_tabEditor.InsertItem(m_editors.size(), name);
	te->ctrl.Setup(fMin, fMax, bIsColor);
	m_saveEditors.push_back(te);
	OnSelectTab();
	te->ctrl.m_trackDlg	=	this;
}

VOID GCTrackEditor::OnResize()
{
	CRect rc;
	GetClientRect(&rc);
	//rc.InflateRect(-1,-1);
	m_rectEditor.SetRect(rc.left, TRACK_HEIGHT, rc.right, rc.bottom - TRACK_TABLE_HEIGHT * 2);
	m_rectTrack.SetRect(rc.left, rc.top, rc.right, TRACK_HEIGHT);
	int nRightWidth		=	500;
	int	buttonHeight	=	18;
	m_rectTab.SetRect(rc.left, rc.bottom - TRACK_TABLE_HEIGHT - buttonHeight, rc.right, rc.bottom - buttonHeight);
	if(m_tabEditor.m_hWnd)
	{
		if(!m_imagePlayTrack.IsNull())
			m_imagePlayTrack.Destroy();
		//
		m_imagePlayTrack.Create(m_rectTrack.Width(), m_rectTrack.Height(), 24);
		CRect rc	=	m_rectTab;
		m_tabEditor.MoveWindow(&rc);
		m_tabEditor.ShowWindow(SW_SHOW);
		m_tabEditor.Invalidate();
		//
		CRect	rcButton	=	m_rectTab;
		//
		//rcButton.right		=	rc.right + 196;
		//rcButton.left		=	rcButton.right + 10;
		rcButton.top		=	rcButton.bottom;
		rcButton.bottom		=	rcButton.top + buttonHeight;
		//
		rcButton.right		=	rcButton.left + 25;
		this->m_chkX.MoveWindow(&rcButton);
		this->m_chkX.ShowWindow(SW_SHOW);
		rcButton.left		=	rcButton.right;
		rcButton.right		=	rcButton.left + 25;
		this->m_chkY.MoveWindow(&rcButton);
		this->m_chkY.ShowWindow(SW_SHOW);
		rcButton.left		=	rcButton.right;
		rcButton.right		=	rcButton.left + 25;
		this->m_chkZ.MoveWindow(&rcButton);
		this->m_chkZ.ShowWindow(SW_SHOW);
		rcButton.left		=	rcButton.right;
		//
		rcButton.right		=	rcButton.left + 50;
		this->m_edMinValue.MoveWindow(&rcButton);//&CRect(m_rectTab.right, m_rectTab.top, m_rectTab.right, m_rectTab.bottom));
		this->m_edMinValue.ShowWindow(SW_SHOW);
		rcButton.left		=	rcButton.right;
		rcButton.right		=	rcButton.left + 50;
		this->m_edMaxValue.MoveWindow(&rcButton);//&CRect(m_rectTab.right, m_rectTab.top, m_rectTab.right, m_rectTab.bottom));
		this->m_edMaxValue.ShowWindow(SW_SHOW);
		rcButton.left		=	rcButton.right;
		rcButton.right		=	rcButton.left + 50;
		this->m_btSetMinMaxValue.MoveWindow(&rcButton);//CRect(m_rectTab.right, m_rectTab.top, m_rectTab.right + 50, m_rectTab.bottom));
		this->m_btSetMinMaxValue.ShowWindow(SW_SHOW);
		//
		rcButton.left		=	rcButton.right;
		rcButton.right		=	rcButton.left + 50;
		this->m_edValue.MoveWindow(&rcButton);//&CRect(m_rectTab.right, m_rectTab.top, m_rectTab.right, m_rectTab.bottom));
		this->m_edValue.ShowWindow(SW_SHOW);
		rcButton.left		=	rcButton.right;
		rcButton.right		=	rcButton.left + 50;
		this->m_btSetValue.MoveWindow(&rcButton);//CRect(m_rectTab.right, m_rectTab.top, m_rectTab.right + 50, m_rectTab.bottom));
		this->m_btSetValue.ShowWindow(SW_SHOW);
		rcButton.left		=	rcButton.right;
		rcButton.right		=	rcButton.left + 50;
		this->m_chkEnable.MoveWindow(&rcButton);//CRect(m_rectTab.right + 70, m_rectTab.top, m_rectTab.right + 100, m_rectTab.bottom));
		this->m_chkEnable.ShowWindow(SW_SHOW);
		//
		rcButton.left		=	rcButton.right;
		rcButton.right		=	rcButton.left + 16;
		//m_btQPlayBack.MoveWindow(&rcButton);
		//OffsetRect(&rcButton, 16, 0);
		//m_btPlayBack.MoveWindow(&rcButton);
		//OffsetRect(&rcButton, 16, 0);
		//m_btPlay.MoveWindow(&rcButton);
		//OffsetRect(&rcButton, 16, 0);
		//m_btQuickPlay.MoveWindow(&rcButton);
		//OffsetRect(&rcButton, 16, 0);
		//m_btPause.MoveWindow(&rcButton);
		//OffsetRect(&rcButton, 16, 0);
		//m_btStop.MoveWindow(&rcButton);

		m_btQPlayBack.ShowWindow(SW_HIDE);
		m_btPlayBack.ShowWindow(SW_HIDE);
		m_btPlay.ShowWindow(SW_HIDE);
		m_btQuickPlay.ShowWindow(SW_HIDE);
		m_btPause.ShowWindow(SW_HIDE);
		m_btStop.ShowWindow(SW_HIDE);
		//
		OnSelectTab();
	}
	this->Invalidate();
}

BOOL GCTrackEditor::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ShowWindow(SW_SHOW);
	m_trackImage.LoadFromResource(AfxGetInstanceHandle(), IDB_TRACK_PLAYS);
	m_btQPlayBack.SetImage(m_trackImage, 2, 6, 3);
	m_btPlayBack.SetImage(m_trackImage, 2, 6, 1);
	m_btPause.SetImage(m_trackImage, 2, 6, 4);
	m_btStop.SetImage(m_trackImage, 2, 6, 0);
	m_btPlay.SetImage(m_trackImage, 2, 6, 2);
	m_btQuickPlay.SetImage(m_trackImage, 2, 6, 5);
	OnResize();
	return TRUE;
}


void GCTrackEditor::OnSize(UINT nType, int cx, int cy)
{
	OnResize();
	CDialogEx::OnSize(nType, cx, cy);
}

VOID GCTrackEditor::SetCheck(CImageCheck* current)
{
	m_btQPlayBack.SetCheck(false);
	m_btPlayBack.SetCheck(false);
	m_btPause.SetCheck(false);
	m_btStop.SetCheck(false);
	m_btPlay.SetCheck(false);
	m_btQuickPlay.SetCheck(false);
	if(&m_btQPlayBack == current)
	{
		m_btQPlayBack.SetCheck(true);
		SetStatus(TrackStatus_QuickPlayBack);
	}
	else if(&m_btPlayBack == current)
	{
		m_btPlayBack.SetCheck(true);
		SetStatus(TrackStatus_PlayBack);
	}
	else if(&m_btPause == current)
	{
		m_btPause.SetCheck(true);
		SetStatus(TrackStatus_Pause);
	}
	else if(&m_btStop == current)
	{
		m_btStop.SetCheck(true);
		SetStatus(TrackStatus_Stop);
		this->m_fPlayIndex	=	0;
	}
	else if(&m_btPlay == current)
	{
		m_btPlay.SetCheck(true);
		SetStatus(TrackStatus_Play);
	}
	else if(&m_btQuickPlay == current)
	{
		m_btQuickPlay.SetCheck(true);
		SetStatus(TrackStatus_QuickPlay);
	}
}

void GCTrackEditor::OnBnClickedPlayTrack()
{
	SetCheck(&m_btPlay);
}


void GCTrackEditor::OnBnClickedPlaybackTrack()
{
	SetCheck(&m_btPlayBack);
}


void GCTrackEditor::OnBnClickedQplayTrack()
{
	SetCheck(&m_btQuickPlay);
}


void GCTrackEditor::OnBnClickedQplaybackTrack()
{
	SetCheck(&m_btQPlayBack);
}


void GCTrackEditor::OnBnClickedStopTrack()
{
	SetCheck(&m_btStop);
}


void GCTrackEditor::OnBnClickedPauseTrack()
{
	SetCheck(&m_btPause);
}

void GCTrackEditor::OnBnClickedEnable()
{
	int sel	=	m_tabEditor.GetCurSel();
	for(int i=0;i<this->m_editors.size();i++)
	{
		if(m_editors[i]->nTabItem == sel)
		{
			m_editors[i]->ctrl.SetEditorEnable(m_chkEnable.GetCheck());
			m_editors[i]->ctrl.EnableWindow(m_chkEnable.GetCheck());
		}
	}
}


VOID	GCTrackEditor::SetTrackKeyCount(int nF)
{
	m_nFrameCount = nF;
	for(int i=0;i<this->m_editors.size();i++)
	{
		m_editors[i]->ctrl.SetFrameCount(nF);
	}
	this->Invalidate();
}

VOID	GCTrackEditor::OnFrameMove(float fElapsedTime)
{
	float fT	=	fElapsedTime * m_fPlaySpeed * (float)m_keySize.cx;
	switch(this->m_trackStatus)
	{
	case TrackStatus_Pause:
	case TrackStatus_Stop:
		return;
	case TrackStatus_Play:
		if(!SetPlayValue(this->m_fPlayIndex + fT))
			return;
		break;
	case TrackStatus_QuickPlay:
		if(!SetPlayValue(this->m_fPlayIndex + fT * 2.0f))
			return;
		break;
	case TrackStatus_PlayBack:
		if(!SetPlayValue(this->m_fPlayIndex - fT))
			return;
		break;
	case TrackStatus_QuickPlayBack:
		if(!SetPlayValue(this->m_fPlayIndex - fT * 2.0f))
			return;
		break;
	}
	this->InvalidateRect(&m_rectTrack);
}

int		GCTrackEditor::GetTrackBeginX()
{
	int scroll	=	0;
	for(int i=0;i<this->m_editors.size();i++)
	{
		if(this->m_editors[i]->ctrl.IsWindowVisible())
		{
			return (TRACK_BEGIN_X) - m_editors[i]->ctrl.GetScrollPos(SB_HORZ);
		}
	}
	return (TRACK_BEGIN_X);
}

float	GCTrackEditor::GetValueByPos(INT nPosX)
{
	return (float)(nPosX - GetTrackBeginX()) / (float)m_keySize.cx;
}

INT		GCTrackEditor::GetPosByValue(float f)
{
	return (int)((f / (float)m_nFrameCount) * (float)(m_keySize.cx * m_nFrameCount)) + GetTrackBeginX();
}

VOID	GCTrackEditor::DrawKeyFrames(CDC& dc, const CRect& drawRect, const CSize& keySize, INT nFrameCount, INT nTrackCount)
{
	CPen pen(PS_SOLID, 1, RGB(180,180,180));
	CPen* oldPen	=	dc.SelectObject(&pen);
	CBrush brush(RGB(220,220,220));
	//
	int width	=	keySize.cx * nFrameCount;
	int height	=	keySize.cy * nTrackCount;
	for(int x=0;x<nFrameCount;x+=10)
	{
		dc.FillRect(&CRect(x * keySize.cx + GetTrackBeginX(), drawRect.top + TRACK_BEGIN_Y, (x + 1) * m_keySize.cx + GetTrackBeginX() + 1, height + TRACK_BEGIN_Y + 1), &brush);
	}
	for(int y=0;y<=nTrackCount;y++)
	{
		DrawLine2(dc.m_hDC, CPoint(drawRect.left, y * keySize.cy), CPoint(width, y * keySize.cy), CPoint(GetTrackBeginX(), TRACK_BEGIN_Y));
	}
	for(int x=0;x<=nFrameCount;x++)
	{
		DrawLine2(dc.m_hDC, CPoint(x * keySize.cx, drawRect.top), CPoint(x * keySize.cx, height), CPoint(GetTrackBeginX(), TRACK_BEGIN_Y));
	}
	if(oldPen)
		dc.SelectObject(oldPen);
}

void GCTrackEditor::OnPaint()
{
	CPaintDC dc(this);
	CRect	rcClient;
	GetClientRect(&rcClient);
	rcClient.top	=	this->m_rectTrack.bottom;
	CRect rcBottom(rcClient.left, rcClient.bottom - TRACK_TABLE_HEIGHT, rcClient.right, rcClient.bottom);
	dc.FillRect(&rcBottom, &CBrush(RGB(255, 255, 255)));
	//
	HDC dcImage	=	m_imagePlayTrack.GetDC();
	HGDIOBJ	oldFont	=	SelectObject(dcImage, GetFont() ? GetFont()->m_hObject : NULL);
	//
	FillRect(dcImage, &m_rectTrack, CBrush(RGB(255,255,255)));
	DrawKeyFrames(*CDC::FromHandle(dcImage), m_rectTrack, m_keySize, m_nFrameCount, 1);
	int nPixelX	=	(int)((m_fPlayIndex / (float)m_nFrameCount) * (float)(m_keySize.cx * m_nFrameCount)) + GetTrackBeginX();
	CRect	rcIndex(nPixelX, 3, nPixelX + m_keySize.cx, m_rectTrack.bottom - 3);
	FillRect(dcImage, &rcIndex, CBrush(RGB(255, 0, 0)));
	wchar_t buf[128];
	swprintf(buf, L"%.2f", this->m_fPlayIndex / TRACK_SECOND_LENGTH);
	SetBkMode(dcImage, TRANSPARENT);
	SetTextColor(dcImage, RGB(0, 0, 0));
	DrawText(dcImage, buf, -1, CRect(rcIndex.right, rcIndex.top, rcIndex.right + 100, rcIndex.bottom), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	//
	if(oldFont)
		SelectObject(dcImage, oldFont);
	BitBlt(dc.m_hDC, m_rectTrack.left, m_rectTrack.top, m_rectTrack.Width(), m_rectTrack.Height(), dcImage, 0, 0, SRCCOPY);
	//
	m_imagePlayTrack.ReleaseDC();
}


BOOL GCTrackEditor::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

VOID	GCTrackEditor::SetStatus(TrackStatus s)
{
	m_trackStatus	=	s;
	this->InvalidateRect(&m_rectTrack);
}

BOOL	GCTrackEditor::SetPlayValue(float f)
{
	m_fPlayIndex	=	clamp(f, 0, (float)this->m_nFrameCount);
	m_fPlayIndex	=	(float)(int)m_fPlayIndex;
	this->InvalidateRect(&m_rectTrack);
	return true;
}

VOID	GCTrackEditor::Refresh()
{
	this->Invalidate();
	for(int i=0;i<this->m_editors.size();i++)
		if(this->m_editors[i]->ctrl.IsWindowVisible())
			m_editors[i]->ctrl.Invalidate();
}
float	GCTrackEditor::GetPlayValue()
{
	return this->m_fPlayIndex;
}

void GCTrackEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(this->m_rectTrack.PtInRect(point))
	{
		m_bDownTrack	=	true;
		SetCapture();
		SetStatus(TrackStatus_Pause);
		SetPlayValue(GetValueByPos(point.x));
		//this->InvalidateRect(&m_rectTrack);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void GCTrackEditor::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bDownTrack	=	false;
	CDialogEx::OnLButtonUp(nFlags, point);
}


void GCTrackEditor::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bDownTrack)
	{
		SetStatus(TrackStatus_Pause);
		SetPlayValue(GetValueByPos(point.x));
		//this->InvalidateRect(&m_rectTrack);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void GCTrackEditor::OnBnClickedSetValue()
{
	for(int i=0;i<this->m_editors.size();i++)
	{
		if(m_editors[i]->ctrl.IsWindowVisible())
		{
			CString str;
			this->m_edValue.GetWindowTextW(str);
			m_editors[i]->ctrl.SetSelectValue(_wtof(str));
			break;
		}
	}
}

void GCTrackEditor::OnBnClickedMinMax()
{
	CString strMin, strMax;
	this->m_edMinValue.GetWindowTextW(strMin);
	this->m_edMaxValue.GetWindowTextW(strMax);
	for(int i=0;i<this->m_editors.size();i++)
	{
		if(m_editors[i]->ctrl.IsWindowVisible())
		{
			CString str;
			this->m_edValue.GetWindowTextW(str);
			for(int j=0;j<3;j++)
				if(m_editors[i]->ctrl.m_floats[j].floatPtr)
				{
					m_editors[i]->ctrl.m_floats[j].floatPtr->SetRange(_wtof(strMin), _wtof(strMax));
					m_editors[i]->ctrl.SetCallback(m_editors[i]->ctrl.m_floats[j].floatPtr, j);
					m_editors[i]->ctrl.m_fViewMaxValue		=	_wtof(strMax);
					m_editors[i]->ctrl.m_fViewMinValue		=	_wtof(strMin);
				}
			m_editors[i]->ctrl.Invalidate();
			break;
		}
	}
}
void GCTrackEditor::OnBnClickedSel()
{
	for(int i=0;i<this->m_editors.size();i++)
	{
		if(m_editors[i]->ctrl.IsWindowVisible())
		{
			m_editors[i]->ctrl.m_floats[0].bCanSelect	=	m_chkX.GetCheck();
			m_editors[i]->ctrl.m_floats[1].bCanSelect	=	m_chkY.GetCheck();
			m_editors[i]->ctrl.m_floats[2].bCanSelect	=	m_chkZ.GetCheck();
			//
			break;
		}
	}
}
VOID GCTrackEditor::SetNull()
{
	for(int i=0;i<this->m_editors.size();i++)
	{
		m_editors[i]->ctrl.SetNull();
	}
}

void GCTrackEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

void GCTrackEditor::OnDestroy()
{
	for(int i=0;i<this->m_saveEditors.size();i++)
		this->m_saveEditors[i]->ctrl.DestroyWindow();
}
