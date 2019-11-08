// FloatAnimationCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "FloatAnimationCtrl.h"
#include "afxdialogex.h"
#include <map>

CPoint   HermiteCurve(const CPoint& p1,const CPoint& p2,const CPoint& t1,const CPoint& t2, float fLerp)
{
	double i2 = fLerp * fLerp;
	double i3 = i2 * fLerp;
	double express = 3 * i2 - 2 * i3;
	// 计算横坐标和纵坐标
	int x = (int)((1 - express) * p1.x + express * p2.x + (fLerp - 2 * i2 + i3) * t1.x + (i3 - i2) * t2.x);
	int y = (int)((1 - express) * p1.y + express * p2.y + (fLerp - 2 * i2 + i3) * t1.y + (i3 - i2) * t2.y);
	return CPoint(x,y);
}

CPoint   LineCurve(const CPoint& p1,const CPoint& p2, float fLerp)
{
	int xx	=	p2.x - p1.x;
	int yy	=	p2.y - p1.y;
	return CPoint(p1.x + (int)((float)xx * fLerp), p1.y + (int)((float)yy * fLerp));
}

VOID				CFloatAnimationCtrl::Setup(INT type, float fMin, float fMax, bool bIsColor, INT heightCount)
{
	this->m_bIsColor	=	bIsColor;
	this->m_nType	=	type;
	m_fMaxValue		=	fMax;
	m_fMinValue		=	fMin;
	this->m_nHeightCount	=	heightCount;
}

// CFloatAnimationCtrl 对话框

IMPLEMENT_DYNAMIC(CFloatAnimationCtrl, CDialogEx)

CFloatAnimationCtrl::CFloatAnimationCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFloatAnimationCtrl::IDD, pParent)
{
	m_bLButtonDown		=	false;
	m_nTrackSelectID	=	-1;
	m_keySize.SetSize(5,10);
	m_nTrackBeginPixel	=	0;
	m_trackPosition		=	TrackPosition_None;
	m_bEditNameMode		=	false;
	m_saveFrame.SetPoint(0,0);
	m_fMaxValue			=	1.0f;
	m_fMinValue			=	0.0f;
	m_saveValue			=	0;
	m_nType				=	0;
	m_floatIndex		=	-1;
	m_nHeightCount		=	15;
	m_bIsColor			=	TRUE;
	memset(m_frames, 0, sizeof(m_frames));
}

VOID			CFloatAnimationCtrl::SetRange(float fMin, float fMax)
{
	m_fMinValue	=	fMin;
	m_fMaxValue	=	fMax;
}

CFloatAnimationCtrl::~CFloatAnimationCtrl()
{
}

void CFloatAnimationCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFloatAnimationCtrl, CDialogEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDOK, &CFloatAnimationCtrl::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFloatAnimationCtrl::OnBnClickedCancel)
	ON_WM_KEYDOWN()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_KEYUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

VOID CFloatAnimationCtrl::ResizeScroll()
{
	CRect rc;
	GetClientRect(&rc);
	int width	=	MAX_FRAME_COUNT * this->m_keySize.cx + TRACK_BEGIN_X + 100;
	int height	=	GetHeightCount() * this->m_keySize.cy + TRACK_BEGIN_Y + 30;
	if(rc.Width() < width)
	{
		this->ShowScrollBar(SB_HORZ);
		int nMax	=	width - rc.Width();
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

// CFloatAnimationCtrl 消息处理程序

VOID CFloatAnimationCtrl::SetCallback(IFloatAnimationCtrlCB* cb, INT index, INT id, INT param)
{
	if(index < 0 || index >= MAX_FLOAT_EDITOR)
		return;
	this->m_floats[index].callback	=	cb;
	this->m_floats[index].nID		=	id;
	this->m_floats[index].param		=	param;
	//for(int iF=0;iF<MAX_FLOAT_EDITOR;iF++)
	int iF	=	index;
	{
		m_floats[iF].tracks.clear();
		if(!m_floats[iF].callback)
			return;
		if(iF == 0)
			m_floats[iF].trackColor	=	RGB(200,0,0);
		else if(iF == 1)
			m_floats[iF].trackColor	=	RGB(0,200,0);
		else if(iF == 2)
			m_floats[iF].trackColor	=	RGB(0,0,200);
		INT nKeyCount		=	m_floats[iF].callback->GetKeyCount(m_floats[iF].nID, m_floats[iF].param);
		for(int i=0;i<nKeyCount;i++)
		{
			FloatTrackItem k;
			m_floats[iF].callback->GetKeyData(m_floats[iF].nID, m_floats[iF].param, i, k);
			//
			FloatTrackItemCtrl kc;
			memset(&kc, 0, sizeof(kc));
			memcpy(&kc, &k, sizeof(k));
			m_floats[iF].tracks.push_back( kc );
		}
		for(int i=0;i<m_floats[iF].tracks.size();i++)
			m_floats[iF].tracks[i].nKeyIndex	=	i;
	}
	ResizeScroll();
}

void CFloatAnimationCtrl::OnSize(UINT nType, int cx, int cy)
{
	if(!m_bgImage.IsNull())
		m_bgImage.Destroy();
	ResizeScroll();
	CDialogEx::OnSize(nType, cx, cy);
}

INT	CFloatAnimationCtrl::GetTrackBeginX()
{
	return TRACK_BEGIN_X - GetScrollPos(SB_HORZ);
}

INT	CFloatAnimationCtrl::GetTrackBeginY()
{
	return TRACK_BEGIN_Y - GetScrollPos(SB_VERT);
}

VOID	CFloatAnimationCtrl::DrawKeyFrames(CDC& dc, const CRect& drawRect, const CSize& keySize, INT nFrameCount, INT nTrackCount)
{
	CPen pen(PS_SOLID, 1, RGB(180,180,180));
	CPen* oldPen	=	dc.SelectObject(&pen);
	CBrush brush(RGB(220,220,220));
	//
	int width	=	keySize.cx * nFrameCount;
	int height	=	keySize.cy * nTrackCount;
	for(int x=0;x<nFrameCount;x+=10)
	{
		dc.FillRect(&CRect(x * keySize.cx + GetTrackBeginX(), drawRect.top + GetTrackBeginY(), (x + 1) * keySize.cx + GetTrackBeginX() + 1, height + GetTrackBeginY() + 1), &brush);
	}
	for(int y=0;y<=nTrackCount;y++)
	{
		DrawLine(dc.m_hDC, CPoint(drawRect.left, y * keySize.cy), CPoint(width, y * keySize.cy), CPoint(GetTrackBeginX(), GetTrackBeginY()));
	}
	for(int x=0;x<=nFrameCount;x++)
	{
		DrawLine(dc.m_hDC, CPoint(x * keySize.cx, drawRect.top), CPoint(x * keySize.cx, height), CPoint(GetTrackBeginX(), GetTrackBeginY()));
	}
	if(oldPen)
		dc.SelectObject(oldPen);
}

VOID	CFloatAnimationCtrl::DrawTrackRect(CDC& dc, const CRect& rc, COLORREF c)
{
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
		DrawLine(dc.m_hDC, CPoint(rc.left, y), CPoint(rc.right, y), RGB(r1,g1,b1));
	}
}

VOID	CFloatAnimationCtrl::DrawTrackTangent(CDC& dc, const CRect& rc, const tangent2& t, COLORREF c, CRect& rcT)
{
	float	x	=	t.x * (float)this->m_keySize.cx;//cosf(t.rot) * t.length;
	float	y	=	t.y / (m_fMaxValue - m_fMinValue) * (float)(GetHeightCount() * this->m_keySize.cy);//sinf(t.rot) * t.length;
	//
	int centerX	=	rc.left + rc.Width() / 2;
	int centerY	=	rc.top + rc.Height() / 2;
	//
	CPoint pos(centerX + (int)x, centerY + (int)y);
	//DrawLine(dc.m_hDC, CPoint(centerX, centerY + 1), CPoint(centerX + (int)x, centerY + (int)y + 1), RGB(0,0,0));
	DrawLine(dc.m_hDC, CPoint(centerX, centerY), pos, c);
	rcT	=	CRect(pos.x-5, pos.y-5,pos.x+5,pos.y+5);
	CBrush br(c);
	CBrush* old = dc.SelectObject(&br);
	dc.Ellipse(&rcT);
	if(old)
		dc.SelectObject(old);
}

VOID	CFloatAnimationCtrl::LerpFrame(Float3& ret, INT frameIndex, float fLerp)
{
	if(frameIndex >= (MAX_FRAME_COUNT - 1))
	{
		ret	=	m_frames[frameIndex];
	}
	else
	{
		ret.v[0]	=	(m_frames[frameIndex + 1].v[0] - m_frames[frameIndex].v[0]) * fLerp + m_frames[frameIndex].v[0];
		ret.v[1]	=	(m_frames[frameIndex + 1].v[1] - m_frames[frameIndex].v[1]) * fLerp + m_frames[frameIndex].v[1];
		ret.v[2]	=	(m_frames[frameIndex + 1].v[2] - m_frames[frameIndex].v[2]) * fLerp + m_frames[frameIndex].v[2];
	}
}

VOID	CFloatAnimationCtrl::DrawTrackColor(CDC& dc, const CRect& drawRect, const CSize& keySize)
{
	CRect rcBegin(GetTrackBeginX(), 1, 0, 0);
	int width	=	m_keySize.cx * MAX_FRAME_COUNT;
	int height	=	m_keySize.cy * GetHeightCount();
	//
	for(int x=0;x<width;x++)
	{
		int xx	=	rcBegin.left + x;
		int yy	=	rcBegin.top;
		Float3 rv;
		LerpFrame(rv, x / m_keySize.cx, (float)(x % m_keySize.cx) / (float)m_keySize.cx);
		int r	=	rv.v[0] * 255.0f;
		int g	=	rv.v[1] * 255.0f;
		int b	=	rv.v[2] * 255.0f;
		CPen pen(PS_SOLID, 1, RGB(r,g,b));
		CPen* old	=	dc.SelectObject(&pen);
		dc.MoveTo(CPoint(xx, yy));
		dc.LineTo(CPoint(xx, yy + 3));
		if(old)
			dc.SelectObject(old);
	}
}

VOID	CFloatAnimationCtrl::FillFrame()
{
	memset(m_frames, 0, sizeof(m_frames));
	for(int iF=0;iF<MAX_FLOAT_EDITOR;iF++)
	{
		FloatTracks&	f	=	m_floats[iF];
		if(f.tracks.size() == 0)
			continue;
		f.tracks[0].rcBegin.SetRect(0, 0, 0, 0);
		for(int i=1;i<f.tracks.size();i++)
		{
			FloatTrackItemCtrl* trackFrom	=	&f.tracks[i - 1];
			FloatTrackItemCtrl* trackTo	=	&f.tracks[i];
			//
			CRect rcBegin(trackFrom->nFrameIndex * m_keySize.cx + GetTrackBeginX(), 0, 0, GetPosByValue(trackFrom->fValue));
			rcBegin.right	=	rcBegin.left	+	m_keySize.cx;
			rcBegin.top		=	rcBegin.bottom	-	m_keySize.cy;
			//
			CRect rcEnd(trackTo->nFrameIndex * m_keySize.cx + GetTrackBeginX(), 0, 0, GetPosByValue(trackTo->fValue));
			rcEnd.right		=	rcEnd.left		+	m_keySize.cx;
			rcEnd.top		=	rcEnd.bottom	-	m_keySize.cy;
			//
			trackFrom->rcEnd	=	rcBegin;
			trackTo->rcBegin	=	rcBegin;
			trackTo->rcEnd		=	rcEnd;
			//
			CPoint begin(rcBegin.right, rcBegin.bottom);
			CPoint end(rcEnd.left, rcEnd.bottom);
			int count	=	end.x - begin.x;
			CPoint t1(trackFrom->t2.x * (float)this->m_keySize.cx * trackFrom->fTangentMul, trackFrom->t2.y / (m_fMaxValue - m_fMinValue) * (float)(GetHeightCount() * this->m_keySize.cy) * trackFrom->fTangentMul);
			CPoint t2(trackTo->t1.x * (float)this->m_keySize.cx * trackTo->fTangentMul, trackTo->t1.y / (m_fMaxValue - m_fMinValue) * (float)(GetHeightCount() * this->m_keySize.cy) * trackTo->fTangentMul);
			t2.x	=	-t2.x;
			t2.y	=	-t2.y;
			if(trackFrom->t2.bEnable && trackTo->t1.bEnable)
			{
				for(int x=0;x<=count;x++)
				{
					float fLerp	=	(float)x / (float)count;
					CPoint pos	=	HermiteCurve(begin, end, t1, t2, fLerp);
					pos.x		=	x + begin.x;
					pos.y		=	ClampY(pos.y);
					m_frames[this->GetKeyFrameByPos(pos.x)].v[iF] = this->GetValueByPos(pos.y);
				}
			}
			else
			{
				for(int x=0;x<=count;x++)
				{
					float fLerp	=	(float)x / (float)count;
					CPoint pos	=	LineCurve(begin, end, fLerp);
					pos.x		=	x + begin.x;
					pos.y		=	ClampY(pos.y);
					m_frames[this->GetKeyFrameByPos(pos.x)].v[iF] = this->GetValueByPos(pos.y);
				}
			}
		}
	}
}

VOID	CFloatAnimationCtrl::DrawTrack(CDC& dc, const CRect& drawRect, const CSize& keySize, FloatTrackItemCtrl* trackFrom, FloatTrackItemCtrl* trackTo, COLORREF clr)
{
	CRect rcBegin(trackFrom->nFrameIndex * keySize.cx + GetTrackBeginX(), 0, 0, GetPosByValue(trackFrom->fValue));
	rcBegin.right	=	rcBegin.left + keySize.cx;
	rcBegin.top		=	rcBegin.bottom - keySize.cy;
	//
	CRect rcEnd(trackTo->nFrameIndex * keySize.cx + GetTrackBeginX(), 0, 0, GetPosByValue(trackTo->fValue));
	rcEnd.right		=	rcEnd.left + keySize.cx;
	rcEnd.top		=	rcEnd.bottom - keySize.cy;
	//
	trackFrom->rcEnd	=	rcBegin;
	trackTo->rcBegin	=	rcBegin;
	trackTo->rcEnd		=	rcEnd;
	//
	if(trackFrom->t2.bEnable && trackTo->t1.bEnable)
	{
		CPen pen(PS_SOLID, 1, clr);
		HGDIOBJ old	=	SelectObject(dc.m_hDC, pen.m_hObject);

		CPoint begin(rcBegin.right, rcBegin.bottom);
		CPoint end(rcEnd.left, rcEnd.bottom);
		int count	=	end.x - begin.x;
		CPoint t1(trackFrom->t2.x * (float)this->m_keySize.cx * trackFrom->fTangentMul, trackFrom->t2.y / (m_fMaxValue - m_fMinValue) * (float)(GetHeightCount() * this->m_keySize.cy) * trackFrom->fTangentMul);
		CPoint t2(trackTo->t1.x * (float)this->m_keySize.cx * trackTo->fTangentMul, trackTo->t1.y / (m_fMaxValue - m_fMinValue) * (float)(GetHeightCount() * this->m_keySize.cy) * trackTo->fTangentMul);
		t2.x	=	-t2.x;
		t2.y	=	-t2.y;
		CPoint posLast	=	begin;
		for(int x=0;x<=count;x++)
		{
			float fLerp	=	(float)x / (float)count;
			CPoint pos	=	HermiteCurve(begin, end, t1, t2, fLerp);
			pos.x		=	x + begin.x;
			pos.y		=	ClampY(pos.y);
			dc.MoveTo(posLast);
			dc.LineTo(pos);
			posLast	=	pos;
		}
		if(old)
			SelectObject(dc.m_hDC, old);
		//HermiteCurve
	}
	else
		DrawLine(dc.m_hDC, CPoint(rcBegin.right, rcBegin.bottom), CPoint(rcEnd.left, rcEnd.bottom), clr);
}

void CFloatAnimationCtrl::OnPaint()
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
	CDC& cdc	=	*CDC::FromHandle(dcImage);
	cdc.SetBkMode(TRANSPARENT);
	CFont* oldFont	=	cdc.SelectObject(font);
	wchar_t buf[512];
	//
	DrawKeyFrames(cdc, rcClient, m_keySize, MAX_FRAME_COUNT, this->GetHeightCount());
	//
	for(int iF=0;iF<MAX_FLOAT_EDITOR;iF++)
	{
		if(!m_floats[iF].callback)
			continue;
		FloatTracks&	f	=	m_floats[iF];
		if(f.tracks.size() == 0)
			continue;
		f.tracks[0].rcBegin.SetRect(0, 0, 0, 0);
		for(int i=1;i<f.tracks.size();i++)
		{
			DrawTrack(cdc, rcClient, m_keySize, &f.tracks[i - 1], &f.tracks[i], f.trackColor);
		}
		for(int i=0;i<f.tracks.size();i++)
		{
			DrawTrackRect(cdc, f.tracks[i].rcEnd, f.trackColor);
			//
			if(f.tracks[i].t1.bEnable)
				DrawTrackTangent(cdc, f.tracks[i].rcEnd, f.tracks[i].t1, RGB(100,0,0), f.tracks[i].rcTangent1);
			if(f.tracks[i].t2.bEnable)
				DrawTrackTangent(cdc, f.tracks[i].rcEnd, f.tracks[i].t2, RGB(0,100,0), f.tracks[i].rcTangent2);
			//
			if(i == this->m_floatIndex && f.tracks[i].nKeyIndex == this->m_nTrackSelectID)
			{
				CRect rc	=	f.tracks[i].rcEnd;
				rc.InflateRect(1,1);
				cdc.Draw3dRect(&rc, InvertColor(f.trackColor), InvertColor(f.trackColor));
			}
			//
			cdc.SetTextColor(0);
			wchar_t buf[128];
			swprintf(buf, L"%.03f", f.tracks[i].fValue);
			cdc.DrawText(buf, -1, &CRect(f.tracks[i].rcEnd.right, f.tracks[i].rcEnd.top, f.tracks[i].rcEnd.right + 100, f.tracks[i].rcEnd.bottom), DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		}
	}
	FillFrame();
	if(m_bIsColor)
	{
		DrawTrackColor(cdc, rcClient, m_keySize);
	}
	//
	int index = 0;
	while(index < (MAX_FRAME_COUNT - 1))
	{
		index	+=	10;
		if(index >= MAX_FRAME_COUNT)
			index	=	MAX_FRAME_COUNT - 1;
		int nBeginY	=	(int)GetHeightCount() * m_keySize.cy + GetTrackBeginY();
		int nBeginX	=	index * m_keySize.cx + GetTrackBeginX() + m_keySize.cx / 2;
		swprintf(buf, L"%.0f%%", (float)index / (float)MAX_FRAME_COUNT * 100.0f);
		cdc.DrawText(buf, -1, &CRect(nBeginX - 100, nBeginY, nBeginX + 100, nBeginY + m_keySize.cy), DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}
	if(oldFont)
		cdc.SelectObject(oldFont);
	BitBlt(dc.m_hDC, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), dcImage, 0, 0, SRCCOPY);
	m_bgImage.ReleaseDC();
}

INT	CFloatAnimationCtrl::GetKeyFrameByPos(INT x)
{
	int nFrame	=	(x - GetTrackBeginX()) / m_keySize.cx;
	if(nFrame < 0)
		nFrame = 0;
	else if(nFrame >= MAX_FRAME_COUNT)
		nFrame	=	MAX_FRAME_COUNT - 1;
	return nFrame;
}

int	CFloatAnimationCtrl::ClampY(int y)
{
	int height	=	GetHeightCount() * this->m_keySize.cy + GetTrackBeginY();
	if(y > height)
		y = height;
	else if(y < (GetTrackBeginY() + m_keySize.cy))
		y = (GetTrackBeginY() + m_keySize.cy);
	return y;
}

INT	CFloatAnimationCtrl::GetPosByFrame(INT nFrame)
{
	return (nFrame * m_keySize.cx) + GetTrackBeginX();
}

float	CFloatAnimationCtrl::GetValueByPos(INT pos)
{
	int height	=	GetHeightCount() * this->m_keySize.cy;
	float fValue = (1.0f - (float)(pos - GetTrackBeginY() - m_keySize.cy) / (float)(height - m_keySize.cy)) * (m_fMaxValue - m_fMinValue) + m_fMinValue;
	if(fValue < m_fMinValue)
		return m_fMinValue;
	else if(fValue > m_fMaxValue)
		return m_fMaxValue;
	return fValue;
}

INT	CFloatAnimationCtrl::GetPosByValue(float fValue)
{
	int height	=	GetHeightCount() * this->m_keySize.cy;
	return (height + GetTrackBeginY()) - (int)((float)(height - m_keySize.cy) * ((fValue - m_fMinValue) / (m_fMaxValue - m_fMinValue)));
}

BOOL CFloatAnimationCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void CFloatAnimationCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bLButtonDown)
	{
		FloatTrackItemCtrl* item		=	GetTrackItem(m_floatIndex, m_nTrackSelectID);
		if(item)
		{
			if(this->m_tangentSelect == this->TangentSelect_None)
			{
				item->fValue		=	GetValueByPos(point.y - m_nTrackBeginPixel);
				if(this->m_floats[m_floatIndex].callback)
					this->m_floats[m_floatIndex].callback->SetKeyData(m_floats[m_floatIndex].nID, m_floats[m_floatIndex].param, m_nTrackSelectID, *item);
			}
			else if(this->m_tangentSelect == this->TangentSelect_Left)
			{
				CPoint center	=	item->rcEnd.CenterPoint();
				item->t1.x	=	GetKeyFrameByPos(point.x) - GetKeyFrameByPos(center.x);
				item->t1.y	=	GetValueByPos(center.y) - GetValueByPos(point.y);
				if(this->m_floats[m_floatIndex].callback)
					this->m_floats[m_floatIndex].callback->SetKeyData(m_floats[m_floatIndex].nID, m_floats[m_floatIndex].param, m_nTrackSelectID, *item);
			}
			else if(this->m_tangentSelect == this->TangentSelect_Right)
			{
				CPoint center	=	item->rcEnd.CenterPoint();
				item->t2.x	=	GetKeyFrameByPos(point.x) - GetKeyFrameByPos(center.x);
				item->t2.y	=	GetValueByPos(center.y) - GetValueByPos(point.y);
				if(this->m_floats[m_floatIndex].callback)
					this->m_floats[m_floatIndex].callback->SetKeyData(m_floats[m_floatIndex].nID, m_floats[m_floatIndex].param, m_nTrackSelectID, *item);
			}
			this->Invalidate();
		}
	}
	int f = 0;
	SetMoveCursor(&f);
	CDialogEx::OnMouseMove(nFlags, point);
}

INT CFloatAnimationCtrl::SetMoveCursor(INT* floatSelect)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	for(int iF=0;iF<MAX_FLOAT_EDITOR;iF++)
	{
		for(int i=0;i<this->m_floats[iF].tracks.size();i++)
		{
			FloatTrackItemCtrl& item	=	m_floats[iF].tracks[i];
			TrackPosition tp = TrackPosition_None;
			if(item.rcEnd.PtInRect(pt))
			{
				*floatSelect	=	iF;
				SetCursor(GetCursorHandle(IDC_HAND));
				return item.nKeyIndex;
			}
		}
	}
	*floatSelect	=	-1;
	return -1;
}


FloatTrackItemCtrl*	CFloatAnimationCtrl::GetTrackItem(INT floatIndex, INT id)
{
	if(floatIndex < 0 || floatIndex >= MAX_FLOAT_EDITOR)
		return 0;
	for(int i=0;i<this->m_floats[floatIndex].tracks.size();i++)
	{
		if(m_floats[floatIndex].tracks[i].nKeyIndex == id)
			return &m_floats[floatIndex].tracks[i];
	}
	return 0;
}

void CFloatAnimationCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_editName.ShowWindow(SW_HIDE);
	SetCapture();
	m_bLButtonDown		=	TRUE;
	m_nTrackSelectID	=	SetMoveCursor(&this->m_floatIndex);
	FloatTrackItemCtrl* item		=	GetTrackItem(this->m_floatIndex, m_nTrackSelectID);
	m_nTrackBeginPixel	=	0;
	this->m_tangentSelect	=	TangentSelect_None;
	if(!item)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		for(int iF=0;iF<MAX_FLOAT_EDITOR;iF++)
		{
			FloatTracks& f = m_floats[iF];
			for(int i=0;i<f.tracks.size();i++)
			{
				FloatTrackItemCtrl& fi	=	f.tracks[i];
				TrackPosition tp = TrackPosition_None;
				if(fi.rcTangent1.PtInRect(pt))
				{
					m_floatIndex		=	iF;
					item	=	&fi;
					m_nTrackSelectID	=	fi.nKeyIndex;
					m_tangentSelect	=	this->TangentSelect_Left;
					break;
				}
				else if(fi.rcTangent2.PtInRect(pt))
				{
					m_floatIndex		=	iF;
					item	=	&fi;
					m_nTrackSelectID	=	fi.nKeyIndex;
					m_tangentSelect	=	this->TangentSelect_Right;
					break;
				}
			}
		}
	}
	if(item)
	{
		m_nTrackBeginPixel	=	point.y - GetPosByValue(item->fValue);
		m_saveValue		=	item->fValue;
		m_saveFrame.x	=	item->nKeyIndex;
		m_saveFrame.x	=	item->nFrameIndex;
	}
	m_downPoint			=	point;
	//
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CFloatAnimationCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	//
	if(m_bLButtonDown)
	{
		int frame = this->GetKeyFrameByPos(point.x);
		if(m_floatIndex >= 0)
		{
			for(int i=0;i<this->m_floats[m_floatIndex].tracks.size();i++)
			{
				if(this->m_floats[m_floatIndex].tracks[i].nFrameIndex == frame)
				{
					m_nTrackSelectID		=	m_floats[m_floatIndex].tracks[i].nKeyIndex;
					//m_tracks[i].fValue		=	GetValueByPos(point.y);
					if(m_floats[m_floatIndex].callback)
						m_floats[m_floatIndex].callback->SetKeyData(m_floats[m_floatIndex].nID, m_floats[m_floatIndex].param, m_floats[m_floatIndex].tracks[i].nKeyIndex, m_floats[m_floatIndex].tracks[i]);
					break;
				}
			}
		}
		m_floatIndex	=	-1;
		this->Invalidate();
	}
	//
	m_bLButtonDown		=	FALSE;
//	m_nTrackSelectID	=	-1;
	CDialogEx::OnLButtonUp(nFlags, point);
}

BOOL CFloatAnimationCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CDialogEx::OnEraseBkgnd(pDC);
}


BOOL CFloatAnimationCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_editName.Create(WS_CHILD|WS_BORDER|ES_CENTER | ES_AUTOVSCROLL, CRect(0, 0, 10, 10), this, 0);
	m_editName.SetFont(GetFont());
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CFloatAnimationCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMenu mnu;
	mnu.LoadMenuW(IDR_POPUP_MENU);
	if(m_floats[1].callback == NULL && m_floats[2].callback == NULL)
		m_floatIndex	=	0;
	else
	{
		POINT pos;
		GetCursorPos(&pos);
		int cmd = ::TrackPopupMenuEx(mnu.GetSubMenu(1)->m_hMenu, TPM_RETURNCMD|TPM_RIGHTALIGN, pos.x, pos.y, m_hWnd, 0);
		switch(cmd)
		{
		case ID_INSERT__X:
			m_floatIndex	=	0;
			break;
		case ID_INSERT__Y:
			m_floatIndex	=	1;
			break;
		case ID_INSERT__Z:
			m_floatIndex	=	2;
			break;
		}
	}
	float fValue	=	this->GetValueByPos(point.y);
	int frame		=	this->GetKeyFrameByPos(point.x);
	if(this->m_floatIndex >= 0 && m_floats[m_floatIndex].callback)
	{
		m_nTrackSelectID	=	this->m_floats[m_floatIndex].callback->InsertKey(m_floats[m_floatIndex].nID, m_floats[m_floatIndex].param, frame, fValue);
		SetCallback(m_floats[m_floatIndex].callback, m_floatIndex, m_floats[m_floatIndex].nID, m_floats[m_floatIndex].param);
	}
	//
	//int id	=	SetMoveCursor();
	//if(id >= 0 && m_bEditNameMode)
	//{
	//	FloatTrackItemCtrl* item		=	GetTrackItem(id);
	//	if(item->rcEnd.PtInRect(point))
	//	{
	//		m_editName.MoveWindow(&CRect(item->rcEnd.CenterPoint().x - 100, item->rcEnd.top - 5, item->rcEnd.CenterPoint().x + 100, item->rcEnd.bottom + 5));
	//		wchar_t buf[128];
	//		swprintf(buf, L"%.03f", item->fValue);
	//		m_editName.SetWindowTextW( buf );
	//		m_editName.ShowWindow(SW_SHOW);
	//		m_editName.SetFocus();
	//		m_editName.SetSel(0,-1);
	//	}
	//}
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CFloatAnimationCtrl::OnBnClickedOk()
{
	if(m_editName.IsWindowVisible())
	{
		CString str;
		m_editName.GetWindowTextW(str);
		FloatTrackItemCtrl* item		=	GetTrackItem(this->m_floatIndex, m_nTrackSelectID);
		if(item)
			item->fValue	=	_wtof(str);
		this->Invalidate();
	}
}


void CFloatAnimationCtrl::OnBnClickedCancel()
{
	m_editName.ShowWindow(SW_HIDE);
	if(this->m_bLButtonDown)
	{
		this->m_bLButtonDown	=	false;
		FloatTrackItemCtrl* item		=	GetTrackItem(this->m_floatIndex, this->m_nTrackSelectID);
		if(item)
		{
			item->fValue		=	m_saveValue;
		}
		this->m_nTrackSelectID	=	-1;
	}
}


void CFloatAnimationCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CFloatAnimationCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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


void CFloatAnimationCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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


void CFloatAnimationCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_ESCAPE)
		this->OnBnClickedCancel();
	else if(nChar == VK_DELETE)
	{
		if(m_floatIndex >= 0)
		{
			if(this->m_floats[m_floatIndex].callback)
			{
				this->m_floats[m_floatIndex].callback->DeleteKey(m_floats[m_floatIndex].nID, m_floats[m_floatIndex].param, this->m_nTrackSelectID);
				this->SetCallback(m_floats[m_floatIndex].callback, m_floatIndex, m_floats[m_floatIndex].nID, m_floats[m_floatIndex].param);
				this->Invalidate();
			}
		}
	}
	CDialogEx::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CFloatAnimationCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//TrackPopupMenuEx( 切线
	CDialogEx::OnRButtonDown(nFlags, point);
}


void CFloatAnimationCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int floatIndex	=	0;
	int idx	=	SetMoveCursor(&floatIndex);
	if(idx >= 0)
	{
		CMenu mnu;
		mnu.LoadMenuW(IDR_POPUP_MENU);
		POINT pos;
		GetCursorPos(&pos);
		int cmd = ::TrackPopupMenuEx(mnu.GetSubMenu(0)->m_hMenu, TPM_RETURNCMD|TPM_RIGHTALIGN, pos.x, pos.y, m_hWnd, 0);
		FloatTrackItemCtrl* item		=	GetTrackItem(floatIndex, idx);
		switch(cmd)
		{
		case ID_NONE_TANGENT:
			item->t1.bEnable	=	false;
			item->t2.bEnable	=	false;
			break;
		case ID_LEFT_TANGENT:
			item->t1.bEnable	=	true;
			item->t2.bEnable	=	false;
			break;
		case ID_RIGHT_TANGENT:
			item->t1.bEnable	=	false;
			item->t2.bEnable	=	true;
			break;
		case ID_DOUBLE_TANGENT:
			item->t1.bEnable	=	true;
			item->t2.bEnable	=	true;
			break;
		case ID_DELETE_POINT:
			{
				m_floats[floatIndex].callback->DeleteKey(m_floats[floatIndex].nID, m_floats[floatIndex].param, item->nKeyIndex);
				this->SetCallback(m_floats[floatIndex].callback, floatIndex, m_floats[floatIndex].nID, m_floats[floatIndex].param);
			}
			break;
		case IDC_TANGENT_1_MUL:
			item->fTangentMul	=	1.0f;
			break;
		case IDC_TANGENT_2_MUL:
			item->fTangentMul	=	2.0f;
			break;
		case IDC_TANGENT_4_MUL:
			item->fTangentMul	=	4.0f;
			break;
		case IDC_TANGENT_8_MUL:
			item->fTangentMul	=	8.0f;
			break;
		default:
			break;
		}
		if(cmd >= 0)
		{
			this->m_floats[floatIndex].callback->SetKeyData(m_floats[floatIndex].nID, m_floats[floatIndex].param, item->nKeyIndex, *item);
			this->Invalidate();
			return;
		}
	}
	CDialogEx::OnRButtonUp(nFlags, point);
}
