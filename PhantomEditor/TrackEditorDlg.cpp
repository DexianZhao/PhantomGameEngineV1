// TrackEditorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrackEditorDlg.h"
#include "afxdialogex.h"
#include "GCViewBase.h"

#define	TRACK_HEIGHT		20
#define	TRACK_TABLE_HEIGHT	20


// CTrackEditorDlg 对话框

IMPLEMENT_DYNAMIC(CTrackEditorDlg, CDialogEx)

CTrackEditorDlg::CTrackEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTrackEditorDlg::IDD, pParent)
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

CTrackEditorDlg::~CTrackEditorDlg()
{
	for(int i=0;i<m_saveEditors.size();i++)
		delete m_saveEditors[i];
}

BOOL	CTrackEditorDlg::IsCanEditor(CFloatAnimation* f)
{
	for(int i=0;i<m_editors.size();i++)
	{
		if(m_editors[i]->ctrl.IsCanEditor(f))
			return true;
	}
	return false;
}

VOID	CTrackEditorDlg::SetPlaySpeed(float f)
{
	this->m_fPlaySpeed	=	f;
}

VOID	CTrackEditorDlg::SetModifyed()
{
	if(m_baseView)
		m_baseView->SetModifyed(TRUE);
}

void CTrackEditorDlg::DoDataExchange(CDataExchange* pDX)
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


BEGIN_MESSAGE_MAP(CTrackEditorDlg, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_EDITOR, &CTrackEditorDlg::OnTcnSelchangeTabEditor)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CTrackEditorDlg::OnBnClickedNull)
	ON_BN_CLICKED(IDCANCEL, &CTrackEditorDlg::OnBnClickedNull)
	ON_BN_CLICKED(IDC_PLAY_TRACK, &CTrackEditorDlg::OnBnClickedPlayTrack)
	ON_BN_CLICKED(IDC_PLAYBACK_TRACK, &CTrackEditorDlg::OnBnClickedPlaybackTrack)
	ON_BN_CLICKED(IDC_QPLAY_TRACK, &CTrackEditorDlg::OnBnClickedQplayTrack)
	ON_BN_CLICKED(IDC_QPLAYBACK_TRACK, &CTrackEditorDlg::OnBnClickedQplaybackTrack)
	ON_BN_CLICKED(IDC_STOP_TRACK, &CTrackEditorDlg::OnBnClickedStopTrack)
	ON_BN_CLICKED(IDC_PAUSE_TRACK, &CTrackEditorDlg::OnBnClickedPauseTrack)
	ON_BN_CLICKED(IDC_TRACK_ENABLE, &CTrackEditorDlg::OnBnClickedEnable)
	ON_BN_CLICKED(IDC_SET_MINMAX_VALUE, &CTrackEditorDlg::OnBnClickedMinMax)
	ON_BN_CLICKED(IDC_TRACK_SEL_X, &CTrackEditorDlg::OnBnClickedSel)
	ON_BN_CLICKED(IDC_TRACK_SEL_Y, &CTrackEditorDlg::OnBnClickedSel)
	ON_BN_CLICKED(IDC_TRACK_SEL_Z, &CTrackEditorDlg::OnBnClickedSel)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_SET_VALUE, &CTrackEditorDlg::OnBnClickedSetValue)
END_MESSAGE_MAP()


VOID CTrackEditorDlg::OnSelectTab()
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

void CTrackEditorDlg::OnTcnSelchangeTabEditor(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	OnSelectTab();
}

VOID CTrackEditorDlg::ClearAnimationPtr()
{
	for(int i=0;i<m_editors.size();i++)
	{
		m_editors[i]->ctrl.SetCallback(0, 0);
		m_editors[i]->ctrl.SetCallback(0, 1);
		m_editors[i]->ctrl.SetCallback(0, 2);
	}
}

VOID CTrackEditorDlg::BeginSet()
{
	for(int i=0;i<m_saveEditors.size();i++)
	{
		m_saveEditors[i]->ctrl.ShowWindow(SW_HIDE);
	}
	m_lastTabSelect	=	m_tabEditor.GetCurSel();
	m_editors.clear();
	m_tabEditor.DeleteAllItems();
}

VOID CTrackEditorDlg::EndSet()
{
	for(int i=0;i<m_editors.size();i++)
		m_editors[i]->ctrl.Invalidate();
	if(m_lastTabSelect > m_tabEditor.GetItemCount())
		m_lastTabSelect = m_tabEditor.GetItemCount()-1;
	m_tabEditor.SetCurSel(m_lastTabSelect);
	OnSelectTab();
}

VOID CTrackEditorDlg::SetAnimationPtr(int id, CFloatAnimation* ani1, CFloatAnimation* ani2, CFloatAnimation* ani3, const char* szName)
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
				wcscpy(m_saveEditors[i]->szName, W(szName));
			if(ani1->GetFloatName()[0])
				wcscpy(m_saveEditors[i]->szName, Language(W(ani1->GetFloatName())));
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

VOID CTrackEditorDlg::AddEditor(const wchar_t* name, int _id, float fMin, float fMax, bool bIsColor)
{
	for(int i=0;i<m_saveEditors.size();i++)
	{
		if(m_saveEditors[i]->id == _id)
		{
			wcscpy(m_saveEditors[i]->szName, name);
			return;
		}
	}
	TrackEditor* te	=	new TrackEditor;
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

VOID CTrackEditorDlg::OnResize()
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

BOOL CTrackEditorDlg::OnInitDialog()
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


void CTrackEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	OnResize();
	CDialogEx::OnSize(nType, cx, cy);
}

VOID CTrackEditorDlg::SetCheck(CImageCheck* current)
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

void CTrackEditorDlg::OnBnClickedPlayTrack()
{
	SetCheck(&m_btPlay);
}


void CTrackEditorDlg::OnBnClickedPlaybackTrack()
{
	SetCheck(&m_btPlayBack);
}


void CTrackEditorDlg::OnBnClickedQplayTrack()
{
	SetCheck(&m_btQuickPlay);
}


void CTrackEditorDlg::OnBnClickedQplaybackTrack()
{
	SetCheck(&m_btQPlayBack);
}


void CTrackEditorDlg::OnBnClickedStopTrack()
{
	SetCheck(&m_btStop);
}


void CTrackEditorDlg::OnBnClickedPauseTrack()
{
	SetCheck(&m_btPause);
}

void CTrackEditorDlg::OnBnClickedEnable()
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


VOID	CTrackEditorDlg::SetTrackKeyCount(int nF)
{
	DefineAuth_2();
	if(!CheckAuth_2(0))
	{
		unsigned char g_text[] = {0x61,0xfb,0xb7,0x46,0xeb,0xc0,0xdd,0x68,0x8c,0xe8,0x61,0xe5,0x64,0xa8,0x7a,0xb1,0x7d,0x8d,0xad,0x0e,0x5a,0xd4,0xff,0x5d,0xd0,0x76,0xf2,0xe3,0x23,0xc1,0x27,0x4a,0x5f,0x28,0x30,0xa7,0xe3,0xd0,0x44,0xe3,0x99,0xd3,0x64};
		unsigned char g_password[] = {0xba,0x01,0x18,0x8f,0x43,0x3e,0x74,0xce,0x42,0x20,0x55,0x2a,0x92,0x04,0x91,0x10,0xbd,0x29,0x49,0xc0,0xe3,0x06,0x1a,0x84,0x77,0xdc,0x51,0x40,0xda,0x6a,0x93,0xeb};
		FreeMode(nF, 100, return);
	}
	m_nFrameCount = nF;
	for(int i=0;i<this->m_editors.size();i++)
	{
		m_editors[i]->ctrl.SetFrameCount(nF);
	}
	this->Invalidate();
}

VOID	CTrackEditorDlg::OnFrameMove(float fElapsedTime)
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

int		CTrackEditorDlg::GetTrackBeginX()
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

float	CTrackEditorDlg::GetValueByPos(INT nPosX)
{
	return (float)(nPosX - GetTrackBeginX()) / (float)m_keySize.cx;
}

INT		CTrackEditorDlg::GetPosByValue(float f)
{
	return (int)((f / (float)m_nFrameCount) * (float)(m_keySize.cx * m_nFrameCount)) + GetTrackBeginX();
}

VOID	CTrackEditorDlg::DrawKeyFrames(CDC& dc, const CRect& drawRect, const CSize& keySize, INT nFrameCount, INT nTrackCount)
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
		DrawLine(dc.m_hDC, CPoint(drawRect.left, y * keySize.cy), CPoint(width, y * keySize.cy), CPoint(GetTrackBeginX(), TRACK_BEGIN_Y));
	}
	for(int x=0;x<=nFrameCount;x++)
	{
		DrawLine(dc.m_hDC, CPoint(x * keySize.cx, drawRect.top), CPoint(x * keySize.cx, height), CPoint(GetTrackBeginX(), TRACK_BEGIN_Y));
	}
	if(oldPen)
		dc.SelectObject(oldPen);
}

void CTrackEditorDlg::OnPaint()
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


BOOL CTrackEditorDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

VOID	CTrackEditorDlg::SetStatus(TrackStatus s)
{
	m_trackStatus	=	s;
	this->InvalidateRect(&m_rectTrack);
}

BOOL	CTrackEditorDlg::SetPlayValue(float f)
{
	m_fPlayIndex	=	clamp(f, 0, (float)this->m_nFrameCount);
	m_fPlayIndex	=	(float)(int)m_fPlayIndex;
	this->InvalidateRect(&m_rectTrack);
	return true;
}

VOID	CTrackEditorDlg::Refresh()
{
	this->Invalidate();
	for(int i=0;i<this->m_editors.size();i++)
		if(this->m_editors[i]->ctrl.IsWindowVisible())
			m_editors[i]->ctrl.Invalidate();
}
float	CTrackEditorDlg::GetPlayValue()
{
	return this->m_fPlayIndex;
}

void CTrackEditorDlg::OnLButtonDown(UINT nFlags, CPoint point)
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


void CTrackEditorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bDownTrack	=	false;
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CTrackEditorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bDownTrack)
	{
		SetStatus(TrackStatus_Pause);
		SetPlayValue(GetValueByPos(point.x));
		//this->InvalidateRect(&m_rectTrack);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CTrackEditorDlg::OnBnClickedSetValue()
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

void CTrackEditorDlg::OnBnClickedMinMax()
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
void CTrackEditorDlg::OnBnClickedSel()
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
VOID CTrackEditorDlg::SetNull()
{
	for(int i=0;i<this->m_editors.size();i++)
	{
		m_editors[i]->ctrl.SetNull();
	}
}

void CTrackEditorDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTrackEditorDlg::OnDestroy()
{
	for(int i=0;i<this->m_saveEditors.size();i++)
		this->m_saveEditors[i]->ctrl.DestroyWindow();
}
