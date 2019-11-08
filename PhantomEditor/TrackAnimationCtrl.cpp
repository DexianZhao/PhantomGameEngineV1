/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// TrackAnimationCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "TrackAnimationCtrl.h"
#include "afxdialogex.h"
#include <map>
#include "GCLanguage.h"

// CTrackAnimationCtrl 对话框

IMPLEMENT_DYNAMIC(CTrackAnimationCtrl, CDialogEx)

CTrackAnimationCtrl::CTrackAnimationCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTrackAnimationCtrl::IDD, pParent)
{
	m_keyCount			=	0;	//关键帧数量
	m_callback			=	0;
	m_bLButtonDown		=	false;
	m_nTrackSelectID	=	-1;
	m_keySize.SetSize(5,12);
	m_nTrackBeginPixel	=	0;
	m_trackPosition		=	TrackPosition_None;
	m_bEditNameMode		=	false;
	m_saveFrame.SetPoint(0,0);
}

CTrackAnimationCtrl::~CTrackAnimationCtrl()
{
}

void CTrackAnimationCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BEGIN_TIME_S, m_stBegin);
	DDX_Control(pDX, IDC_BEGIN_TIME, m_edBegin);
	DDX_Control(pDX, IDC_END_TIME_S, m_stEnd);
	DDX_Control(pDX, IDC_END_TIME, m_edEnd);
}


BEGIN_MESSAGE_MAP(CTrackAnimationCtrl, CDialogEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDOK, &CTrackAnimationCtrl::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTrackAnimationCtrl::OnBnClickedCancel)
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


BOOL CTrackAnimationCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(&pos);
	int nmin,nmax;
	GetScrollRange(SB_VERT, &nmin, &nmax);
	int p	=	GetScrollPos(SB_VERT);
	if(zDelta < 0)
		p	+=	10;
	else
		p	-=	10;
	if(p < nmin)
		p	=	nmin;
	else if(p > nmax)
		p	=	nmax;
	SetScrollPos(SB_VERT, p);
	this->Invalidate();
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

VOID CTrackAnimationCtrl::ResizeScroll()
{
	CRect rc;
	GetClientRect(&rc);
	int width	=	this->m_keyCount + (TRACK_BEGIN_X + 100) / m_keySize.cx;
	int height	=	this->m_tracks.size() * this->m_keySize.cy + TRACK_BEGIN_Y + 30;
	int w		=	rc.Width() / m_keySize.cx;
	if(w < width)
	{
		this->ShowScrollBar(SB_HORZ);
		int nMax	=	width - w;
		int pos	=	this->GetScrollPos(SB_HORZ);
		if(pos > nMax)
		{
			pos	=	nMax;
			this->SetScrollPos(SB_HORZ, pos);
		}
		this->SetScrollRange(SB_HORZ, 0, nMax);
	}
	else
		this->ShowScrollBar(SB_HORZ, FALSE);
	if(rc.Height() < height)
	{
		this->ShowScrollBar(SB_VERT);
		int nMax	=	height - rc.Height();
		int pos	=	this->GetScrollPos(SB_VERT);
		if(pos > nMax)
		{
			pos	=	nMax;
			this->SetScrollPos(SB_VERT, pos);
		}
		this->SetScrollRange(SB_VERT, 0, nMax);
	}
	else
		this->ShowScrollBar(SB_VERT, FALSE);
	this->Invalidate();
}

// CTrackAnimationCtrl 消息处理程序

VOID CTrackAnimationCtrl::SetCallback(ITrackAnimationCallback* cb)
{
	this->m_callback	=	cb;
	if(!m_callback)
	{
		m_keyCount	=	0;
		m_tracks.clear();
		this->Invalidate();
		return;
	}
	m_keyCount	=	cb->GetKeyFrameCount();
	m_tracks.clear();
	//
	cb->EnumTracks(m_tracks);
	//for(int i=0;i<cb->GetTrackCount();i++)
	//{
	//	INT id	=	cb->GetTrackIDByIndex(i);
	//	INT begin, end;
	//	COLORREF c;
	//	std::string str;
	//	if(!cb->GetTrackData(id, begin, end, c, str))
	//		continue;
	//	TrackItem t;
	//	memset(&t, 0, sizeof(t));
	//	t.beginFrame	=	begin;
	//	t.endFrame		=	end;
	//	t.c				=	c;
	//	t.id			=	id;
	//	int len = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, t.name, 128);
	//	t.name[len]		=	0;
	//	m_tracks.push_back( t );
	//}
	for(int i=0;i<m_tracks.size();i++)
	{
		m_tracks[i].index	=	i;
	}
	ResizeScroll();
}

void CTrackAnimationCtrl::OnSize(UINT nType, int cx, int cy)
{
	if(!m_bgImage.IsNull())
		m_bgImage.Destroy();
	ResizeScroll();
	CDialogEx::OnSize(nType, cx, cy);
}

INT	CTrackAnimationCtrl::GetTrackBeginX()
{
	return TRACK_BEGIN_X - GetScrollPos(SB_HORZ) * m_keySize.cx;
}

INT	CTrackAnimationCtrl::GetTrackBeginY()
{
	return TRACK_BEGIN_Y - GetScrollPos(SB_VERT);
}

VOID	DrawLine(HDC dc, const CPoint& from, const CPoint& to, const CPoint& offset)
{
	::MoveToEx(dc, from.x + offset.x, from.y + offset.y, 0);
	::LineTo(dc, to.x + offset.x, to.y + offset.y);
}

VOID	DrawLine(HDC dc, const CPoint& from, const CPoint& to, COLORREF c)
{
	CPen pen(PS_SOLID, 1, c);
	HGDIOBJ old	=	SelectObject(dc, pen.m_hObject);
	::MoveToEx(dc, from.x, from.y, 0);
	::LineTo(dc, to.x, to.y);
	if(old)
		SelectObject(dc, old);
}

COLORREF	InvertColor(COLORREF c)
{
	INT r	=	255 - (c & 0x000000FF);
	INT g	=	255 - ((c & 0x0000FF00) >> 8);
	INT b	=	255 - ((c & 0x00FF0000) >> 16);
	return RGB(r,g,b);
}

int		clampInt(int n, int nmin, int nmax)
{
	if(n < nmin)
		return nmin;
	else if(n > nmax)
		return nmax;
	return n;
}

VOID	DrawLine(HDC dc, const CPoint& from, const CPoint& to, const CPoint& offset, const CRect& rc)
{
	CPoint from1	=	CPoint(from.x + offset.x, from.y + offset.y);
	CPoint to1		=	CPoint(to.x + offset.x, to.y + offset.y);
	if(from1.x < rc.left && to1.x < rc.left)
		return;
	if(from1.x > rc.right && to1.x > rc.right)
		return;
	if(from1.y < rc.top && to1.y < rc.top)
		return;
	if(from1.y > rc.bottom && to1.y > rc.bottom)
		return;
	from1.x			=	clampInt(from1.x, rc.left, rc.right);
	from1.y			=	clampInt(from1.y, rc.top, rc.bottom);
	to1.x			=	clampInt(to1.x, rc.left, rc.right);
	to1.y			=	clampInt(to1.y, rc.top, rc.bottom);
	if(from1.x != to1.x || from1.y != to1.y)
	{
		::MoveToEx(dc, from1.x, from1.y, 0);
		::LineTo(dc, to1.x, to1.y);
	}
}

VOID	DrawLine(HDC dc, const CPoint& from, const CPoint& to, COLORREF c, const CRect& rc)
{
	CPoint from1	=	CPoint(from.x, from.y);
	CPoint to1		=	CPoint(to.x, to.y);
	if(from1.x < rc.left && to1.x < rc.left)
		return;
	if(from1.x > rc.right && to1.x > rc.right)
		return;
	if(from1.y < rc.top && to1.y < rc.top)
		return;
	if(from1.y > rc.bottom && to1.y > rc.bottom)
		return;
	from1.x			=	clampInt(from1.x, rc.left, rc.right);
	from1.y			=	clampInt(from1.y, rc.top, rc.bottom);
	to1.x			=	clampInt(to1.x, rc.left, rc.right);
	to1.y			=	clampInt(to1.y, rc.top, rc.bottom);
	if(from1.x != to1.x || from1.y != to1.y)
	{
		CPen pen(PS_SOLID, 1, c);
		HGDIOBJ old	=	SelectObject(dc, pen.m_hObject);
		::MoveToEx(dc, from1.x, from1.y, 0);
		::LineTo(dc, to1.x, to1.y);
		if(old)
			SelectObject(dc, old);
	}
}

VOID	CTrackAnimationCtrl::DrawTrackRect(CDC& dc, const CRect& rc, COLORREF c)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	float r	=	(float)(c & 0x000000FF) / 255.0f;
	float g	=	(float)((c & 0x0000FF00) >> 8) / 255.0f;
	float b	=	(float)((c & 0x00FF0000) >> 16) / 255.0f;
	for(int y=rc.top;y<rc.bottom;y++)
	{
		float f = 1.0f - fabsf((float)(y - (rc.top + rc.Height() / 2)) / (float)(rc.Height()));
		INT r1	=	(int)((r * f) * 255.0f);
		INT g1	=	(int)((g * f) * 255.0f);
		INT b1	=	(int)((b * f) * 255.0f);
		//
		DrawLine(dc.m_hDC, CPoint(rc.left, y), CPoint(rc.right, y), RGB(r1,g1,b1), rcClient);
	}
	DrawLine(dc.m_hDC, CPoint(rc.left, rc.top), CPoint(rc.left, rc.bottom), RGB(50,50,50), rcClient);
	DrawLine(dc.m_hDC, CPoint(rc.right, rc.top), CPoint(rc.right, rc.bottom), RGB(50,50,50), rcClient);
	DrawLine(dc.m_hDC, CPoint(rc.left + 5, rc.top), CPoint(rc.left + 5, rc.bottom), RGB(50,50,50), rcClient);
	DrawLine(dc.m_hDC, CPoint(rc.right - 5, rc.top), CPoint(rc.right - 5, rc.bottom), RGB(50,50,50), rcClient);
	//dc.FillRect(&rc, &CBrush(c));
}

VOID	CTrackAnimationCtrl::DrawKeyFrames(CDC& dc, const CRect& drawRect, const CSize& keySize, INT nFrameCount, INT nTrackCount)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	//dc.Draw3dRect(&drawRect, RGB(0,0,0), RGB(0,0,0));
	CPen pen(PS_SOLID, 1, RGB(120,120,120));
	CPen* oldPen	=	dc.SelectObject(&pen);
	CBrush brush(RGB(200,200,200));
	//
	int width	=	keySize.cx * nFrameCount;
	int height	=	keySize.cy * nTrackCount;
	for(int x=0;x<nFrameCount;x+=20)
	{
		CRect fill(x * keySize.cx + GetTrackBeginX(), drawRect.top + GetTrackBeginY(), (x + 1) * keySize.cx + GetTrackBeginX() + 1, height + GetTrackBeginY() + 1);
		if(fill.right < rcClient.left)
			continue;
		else if(fill.left > rcClient.right)
			continue;
		fill.top		=	max(rcClient.top, fill.top);
		fill.bottom		=	min(rcClient.bottom, fill.bottom);
		if(fill.top < fill.bottom)
			dc.FillRect(&fill, &brush);
	}
	for(int y=0;y<=nTrackCount;y++)
	{
		DrawLine(dc.m_hDC, CPoint(drawRect.left, y * keySize.cy), CPoint(width, y * keySize.cy), CPoint(GetTrackBeginX(), GetTrackBeginY()), rcClient);
	}
	for(int x=0;x<=nFrameCount;x+=20)
	{
		DrawLine(dc.m_hDC, CPoint(x * keySize.cx, drawRect.top), CPoint(x * keySize.cx, height), CPoint(GetTrackBeginX(), GetTrackBeginY()), rcClient);
		DrawLine(dc.m_hDC, CPoint((x + 1) * keySize.cx, drawRect.top), CPoint((x + 1) * keySize.cx, height), CPoint(GetTrackBeginX(), GetTrackBeginY()), rcClient);
	}
	if(oldPen)
		dc.SelectObject(oldPen);
}

VOID	CTrackAnimationCtrl::DrawTrack(CDC& dc, const CRect& drawRect, const CSize& keySize, TrackItem& track)
{
	CRect rcTrack(track.beginFrame * keySize.cx + GetTrackBeginX(), track.index * keySize.cy + GetTrackBeginY(), (track.endFrame + 1) * keySize.cx + GetTrackBeginX(), (track.index + 1) * keySize.cy + GetTrackBeginY());
	track.bodyRc.SetRect(rcTrack.left + 5, rcTrack.top, rcTrack.right - 5, rcTrack.bottom);
	track.beginRc.SetRect(rcTrack.left, rcTrack.top, rcTrack.left + 5, rcTrack.bottom);
	track.endRc.SetRect(rcTrack.right - 5, rcTrack.top, rcTrack.right, rcTrack.bottom);
	DrawTrackRect(dc, rcTrack, track.c);
	wchar_t buf[128];
	dc.SetTextColor(RGB(0,0,0));
	swprintf(buf, L"%s [%.01f, %.01f]%.01f%s", track.name.t, (float)track.beginFrame / TRACK_SECOND_LENGTH, (float)track.endFrame / TRACK_SECOND_LENGTH, (float)(track.endFrame - track.beginFrame) / TRACK_SECOND_LENGTH, Language(L"秒_单字", L"秒"));
	//dc.DrawText(buf, -1, &CRect(rcTrack.right + 11, rcTrack.top + 1, rcTrack.right + 150, rcTrack.bottom), DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	dc.DrawText(buf, -1, &CRect(rcTrack.left + 1, rcTrack.top + 1, rcTrack.right + 100, rcTrack.bottom), DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	dc.SetTextColor(RGB(255,55,0));
	dc.DrawText(buf, -1, &CRect(rcTrack.left, rcTrack.top, rcTrack.right + 100, rcTrack.bottom), DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	//
	//dc.SetTextColor(RGB(0,0,0));
	//dc.DrawText(buf, -1, &CRect(rcTrack.right + 11, rcTrack.top + 1, rcTrack.right + 100, rcTrack.bottom), DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	//dc.SetTextColor(RGB(255,55,0));
	//dc.DrawText(buf, -1, &CRect(rcTrack.right + 10, rcTrack.top, rcTrack.right + 100, rcTrack.bottom), DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	////
	//swprintf(buf, L"%d", track.beginFrame);
	//dc.DrawText(buf, -1, &CRect(50, rcTrack.top, rcTrack.left + 100, rcTrack.bottom), DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	////
	//swprintf(buf, L"%d", track.endFrame);
	//dc.DrawText(buf, -1, &CRect(100, rcTrack.top, rcTrack.right, rcTrack.bottom), DT_LEFT|DT_SINGLELINE|DT_VCENTER);
}

void CTrackAnimationCtrl::OnPaint()
{
	CPaintDC dc(this);
	CFont* font	=	GetFont();
	if(!font)return;
	//
	CRect rcClient;
	GetClientRect(&rcClient);
	if(m_bgImage.IsNull())
		m_bgImage.Create(rcClient.Width(), rcClient.Height(), 24);
	//
	HDC dcImage	=	m_bgImage.GetDC();
	FillRect(dcImage, &rcClient, CBrush(RGB(255,255,255)));
	//
	wchar_t buf[512];
	//
	CDC& cdc	=	*CDC::FromHandle(dcImage);
	cdc.SetBkMode(TRANSPARENT);
	CFont* oldFont	=	cdc.SelectObject(font);
	if(m_keyCount > 0)
	{
		DrawKeyFrames(cdc, rcClient, m_keySize, m_keyCount, m_tracks.size());
		for(int i=0;i<m_tracks.size();i++)
		{
			DrawTrack(cdc, rcClient, m_keySize, m_tracks[i]);
		}
		//
		cdc.SetTextColor(RGB(50,50,50));
		int index = 0;
		while(index < (m_keyCount - 1))//int i=0;i<=m_keyCount;i+=10)
		{
			index	+=	20;
			if(index >= m_keyCount)
				index	=	m_keyCount - 1;
			int nBeginY	=	(int)m_tracks.size() * m_keySize.cy + GetTrackBeginY();
			int nBeginX	=	index * m_keySize.cx + GetTrackBeginX() + m_keySize.cx / 2;
			swprintf(buf, L"%.01f%s", (float)index / TRACK_SECOND_LENGTH, Language(L"秒_单字", L"秒"));
			cdc.DrawText(buf, -1, &CRect(nBeginX - 100, nBeginY, nBeginX + 100, nBeginY + m_keySize.cy), DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		}
	}
	//
	if(oldFont)
		cdc.SelectObject(oldFont);
	BitBlt(dc.m_hDC, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), dcImage, 0, 0, SRCCOPY);
	m_bgImage.ReleaseDC();
}

INT	CTrackAnimationCtrl::GetKeyFrameByPos(INT x)
{
	int nFrame	=	(x - GetTrackBeginX()) / m_keySize.cx;
	if(nFrame < 0)
		nFrame = 0;
	else if(nFrame >= this->m_keyCount)
		nFrame	=	m_keyCount - 1;
	return nFrame;
}

INT	CTrackAnimationCtrl::GetPosByFrame(INT nFrame)
{
	return (nFrame * m_keySize.cx) + GetTrackBeginX();
}

BOOL CTrackAnimationCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void CTrackAnimationCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bLButtonDown)
	{
		TrackItem* item		=	GetTrackItem(m_nTrackSelectID);
		if(item)
		{
			switch(this->m_trackPosition)
			{
			case TrackPosition_Left:
				{
					item->beginFrame	=	GetKeyFrameByPos(point.x - m_nTrackBeginPixel);
				}
				break;
			case TrackPosition_Right:
				{
					item->endFrame		=	GetKeyFrameByPos(point.x - m_nTrackBeginPixel);
				}
				break;
			case TrackPosition_Center:
				{
					int count			=	item->endFrame - item->beginFrame;
					item->beginFrame	=	GetKeyFrameByPos(point.x - m_nTrackBeginPixel);
					item->endFrame		=	item->beginFrame + count;
					if(item->endFrame >= this->m_keyCount)
					{
						item->endFrame	=	this->m_keyCount - 1;
						item->beginFrame	=	item->endFrame - count;
					}
				}
				break;
			}
			//
			this->Invalidate();
		}
	}
	SetMoveCursor(0);
	CDialogEx::OnMouseMove(nFlags, point);
}

std::map<int, HCURSOR> g_cursorMap;
HCURSOR	GetCursorHandle(LPCTSTR str)
{
	if(g_cursorMap[(int)str] != NULL)
		return g_cursorMap[(int)str];
	HCURSOR hCursor;
	g_cursorMap[(int)str]	=	hCursor	=	LoadCursor(0, str);
	return hCursor;
}

INT CTrackAnimationCtrl::SetMoveCursor(TrackPosition* ret)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	for(int i=0;i<this->m_tracks.size();i++)
	{
		TrackItem& item	=	m_tracks[i];
		TrackPosition tp = TrackPosition_None;
		if(item.beginRc.PtInRect(pt))
			tp	=	TrackPosition_Left;
		else if(item.endRc.PtInRect(pt))
			tp	=	TrackPosition_Right;
		else if(item.bodyRc.PtInRect(pt))
			tp	=	TrackPosition_Center;
		else
			continue;
		//
		if(ret)
			*ret	=	tp;
		switch(tp)
		{
		case TrackPosition_Left:
		case TrackPosition_Right:
			SetCursor(GetCursorHandle(IDC_SIZEWE));
			break;
		case TrackPosition_Center:
			SetCursor(GetCursorHandle(IDC_HAND));
			break;
		}
		return item.id;
	}
	return -1;
}


TrackItem*	CTrackAnimationCtrl::GetTrackItem(INT id)
{
	for(int i=0;i<this->m_tracks.size();i++)
	{
		if(m_tracks[i].id == id)
			return &m_tracks[i];
	}
	return 0;
}

void CTrackAnimationCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_editName.ShowWindow(SW_HIDE);
	SetCapture();
	m_bLButtonDown		=	TRUE;
	m_nTrackSelectID	=	SetMoveCursor(&this->m_trackPosition);
	TrackItem* item		=	GetTrackItem(m_nTrackSelectID);
	m_nTrackBeginPixel	=	0;
	if(item)
	{
		switch(this->m_trackPosition)
		{
		case TrackPosition_Left:
		case TrackPosition_Center:
			{
				m_nTrackBeginPixel	=	point.x - GetPosByFrame(item->beginFrame);
			}
			break;
		case TrackPosition_Right:
			{
				m_nTrackBeginPixel	=	point.x - GetPosByFrame(item->endFrame);
			}
			break;
		}
		m_saveFrame.x	=	item->beginFrame;
		m_saveFrame.y	=	item->endFrame;
	}
	//
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CTrackAnimationCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	//
	if(m_bLButtonDown)
	{
		TrackItem* item		=	GetTrackItem(m_nTrackSelectID);
		if(item)
		{
			int begin			=	min(item->beginFrame, item->endFrame);
			int end				=	max(item->beginFrame, item->endFrame);
			item->beginFrame	=	begin;
			item->endFrame		=	end;
			if(item->beginFrame != this->m_saveFrame.x || item->endFrame != this->m_saveFrame.y)
			{
				if(this->m_callback)
					this->m_callback->OnTrackChange(item->id, item->beginFrame, item->endFrame);
			}
		}
		this->Invalidate();
	}
	//
	m_bLButtonDown		=	FALSE;
	m_nTrackSelectID	=	-1;
	CDialogEx::OnLButtonUp(nFlags, point);
}

BOOL CTrackAnimationCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CDialogEx::OnEraseBkgnd(pDC);
}


BOOL CTrackAnimationCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ShowWindow(SW_SHOW);
	m_editName.Create(WS_CHILD|WS_BORDER|ES_CENTER | ES_AUTOVSCROLL, CRect(0, 0, 10, 10), this, 0);
	m_editName.SetFont(GetFont());
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CTrackAnimationCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int id	=	SetMoveCursor(0);
	if(id >= 0 && m_bEditNameMode)
	{
		TrackItem* item		=	GetTrackItem(id);
		if(item->bodyRc.PtInRect(point))
		{
			m_editName.MoveWindow(&CRect(item->bodyRc.CenterPoint().x - 100, item->bodyRc.top - 5, item->bodyRc.CenterPoint().x + 100, item->bodyRc.bottom + 5));
			m_editName.SetWindowTextW(item->name.t);
			m_editName.ShowWindow(SW_SHOW);
			m_editName.SetFocus();
			m_editName.SetSel(0,-1);
		}
	}
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CTrackAnimationCtrl::OnBnClickedOk()
{
}


void CTrackAnimationCtrl::OnBnClickedCancel()
{
	m_editName.ShowWindow(SW_HIDE);
	if(this->m_bLButtonDown)
	{
		this->m_bLButtonDown	=	false;
		TrackItem* item		=	GetTrackItem(this->m_nTrackSelectID);
		if(item)
		{
			item->beginFrame	=	m_saveFrame.x;
			item->endFrame		=	m_saveFrame.y;
		}
		this->m_nTrackSelectID	=	-1;
	}
}


void CTrackAnimationCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_ESCAPE)
		this->OnBnClickedCancel();
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CTrackAnimationCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int minpos;
	int maxpos;
	GetScrollRange(SB_VERT, &minpos, &maxpos); 
	int curpos = GetScrollPos(SB_VERT);
	switch (nSBCode)
	{
	case SB_LEFT:
		curpos = minpos;
		break;
	case SB_RIGHT:
		curpos = maxpos;
		break;
	case SB_ENDSCROLL:
		break;
	case SB_LINELEFT:
		if (curpos > minpos)
			curpos--;
		break;
	case SB_LINERIGHT:   // Scroll right.
		if (curpos < maxpos)
			curpos++;
		break;
	case SB_PAGELEFT:    // Scroll one page left.
		{
			SCROLLINFO   info;
			GetScrollInfo(SB_VERT, &info, SIF_ALL);
			if (curpos > minpos)
				curpos = max(minpos, curpos - 100);
		}
		break;
	case SB_PAGERIGHT:      // Scroll one page right.
		{
			SCROLLINFO   info;
			GetScrollInfo(SB_VERT, &info, SIF_ALL);

			if (curpos < maxpos)
				curpos = min(maxpos, curpos + 100);
		}
		break;
	case SB_THUMBPOSITION:
		curpos = nPos;
		break;
	case SB_THUMBTRACK:
		curpos = nPos;
		break;
	}
	if(curpos < minpos)
		curpos = minpos;
	else if(curpos >= maxpos)
		curpos	=	maxpos;
	SetScrollPos(SB_VERT, curpos);
	this->Invalidate();
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CTrackAnimationCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int minpos;
	int maxpos;
	GetScrollRange(SB_HORZ, &minpos, &maxpos); 
	int curpos = GetScrollPos(SB_HORZ);
	switch (nSBCode)
	{
	case SB_LEFT:
		curpos = minpos;
		break;
	case SB_RIGHT:
		curpos = maxpos;
		break;
	case SB_ENDSCROLL:
		break;
	case SB_LINELEFT:
		if (curpos > minpos)
			curpos--;
		break;
	case SB_LINERIGHT:   // Scroll right.
		if (curpos < maxpos)
			curpos++;
		break;
	case SB_PAGELEFT:    // Scroll one page left.
		{
			SCROLLINFO   info;
			GetScrollInfo(SB_HORZ, &info, SIF_ALL);
			if (curpos > minpos)
				curpos = max(minpos, curpos - 100);
		}
		break;
	case SB_PAGERIGHT:      // Scroll one page right.
		{
			SCROLLINFO   info;
			GetScrollInfo(SB_HORZ, &info, SIF_ALL);

			if (curpos < maxpos)
				curpos = min(maxpos, curpos + 100);
		}
		break;
	case SB_THUMBPOSITION:
		curpos = nPos;
		break;
	case SB_THUMBTRACK:
		curpos = nPos;
		break;
	}
	if(curpos < minpos)
		curpos = minpos;
	else if(curpos >= maxpos)
		curpos	=	maxpos;
	SetScrollPos(SB_HORZ, curpos);
	this->Invalidate();
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
