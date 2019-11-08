// FloatAnimationCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "FloatAnimationCtrl.h"
#include "afxdialogex.h"
#include <map>
#include "TrackEditorDlg.h"
#include "GCLanguage.h"
#include "GCDefine.h"

int	sortKeys(const void* v1, const void* v2)
{
	CFloatAnimationCtrl::SelectKey* k1	=	(CFloatAnimationCtrl::SelectKey*)v1;
	CFloatAnimationCtrl::SelectKey* k2	=	(CFloatAnimationCtrl::SelectKey*)v2;
	if(k1->nFloatIndex < k2->nFloatIndex)
		return -1;
	if(k1->nFloatIndex > k2->nFloatIndex)
		return 1;
	if(k1->nTrackKeyIndex < k2->nTrackKeyIndex)
		return 1;
	if(k1->nTrackKeyIndex > k2->nTrackKeyIndex)
		return -1;
	return 0;
}

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

VOID				CFloatAnimationCtrl::Setup(float fMin, float fMax, bool bIsColor, INT heightCount)
{
	this->m_bIsColor	=	bIsColor;
	for(int i=0;i<MAX_FLOAT_EDITOR;i++)
		if(this->m_floats[i].floatPtr)
			this->m_floats[i].floatPtr->SetRange(fMin, fMax);
	m_fMaxValue		=	fMax;
	m_fMinValue		=	fMin;
	m_fViewMinValue	=	fMin;
	m_fViewMaxValue	=	fMax;
	this->m_nHeightCount	=	heightCount;
}

// CFloatAnimationCtrl 对话框

IMPLEMENT_DYNAMIC(CFloatAnimationCtrl, CDialogEx)

CFloatAnimationCtrl::CFloatAnimationCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFloatAnimationCtrl::IDD, pParent)
{
	m_bLButtonDown		=	false;
	m_keySize.SetSize(5,10);
	m_trackPosition		=	TrackPosition_None;
	m_bEditNameMode		=	false;
	m_fMaxValue			=	1.0f;
	m_fMinValue			=	0.0f;
	m_saveValue			=	0;
	m_nHeightCount		=	15;
	m_bIsColor			=	TRUE;
	m_nFrameCount		=	MAX_ANIMATION_KEY;
	m_fViewScale		=	1.0f;
	m_fViewMaxValue		=	1.0f;
	m_fViewMinValue		=	0.0f;
	m_bMButtonDown		=	false;
	m_trackDlg			=	0;
	m_bDrawSelect		=	false;
	m_bMouseSelect		=	false;
	m_selectRect.SetRect(0, 0, 0, 0);
}

VOID			CFloatAnimationCtrl::SetRange(float fMin, float fMax)
{
	m_fMinValue	=	fMin;
	m_fMaxValue	=	fMax;
	for(int i=0;i<MAX_FLOAT_EDITOR;i++)
		if(this->m_floats[i].floatPtr)
			this->m_floats[i].floatPtr->SetRange(fMin, fMax);
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
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
END_MESSAGE_MAP()

VOID CFloatAnimationCtrl::ResizeScroll()
{
	CRect rc;
	GetClientRect(&rc);
	int width	=	m_nFrameCount * this->m_keySize.cx + TRACK_BEGIN_X + 100;
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
	this->Invalidate();
}

// CFloatAnimationCtrl 消息处理程序

VOID CFloatAnimationCtrl::SetEditorEnable(BOOL b)
{
	for(int i=0;i<MAX_FLOAT_EDITOR;i++)
		if(m_floats[i].floatPtr)
		{
			m_floats[i].floatPtr->SetEnabled(b);
			SetCallback(m_floats[i].floatPtr, i);
		}
}

VOID CFloatAnimationCtrl::SetCallback(CFloatAnimation* cb, INT index, BOOL bSetView)
{
	if(index < 0 || index >= MAX_FLOAT_EDITOR)
		return;
	this->m_floats[index].floatPtr	=	cb;
	if(this->m_floats[index].floatPtr)
	{
		m_fMinValue	=	m_floats[index].floatPtr->GetMinValue();
		m_fMaxValue	=	m_floats[index].floatPtr->GetMaxValue();//this->m_floats[index].floatPtr->SetRange(m_fMinValue, m_fMaxValue);
		m_nFrameCount	=	m_floats[index].floatPtr->GetFrameCount();
	}
	if(bSetView)
	{
		m_fViewMaxValue	=	m_fMaxValue;
		m_fViewMinValue	=	m_fMinValue;
	}
	else
	{
		m_fViewMaxValue	=	clamp(m_fViewMaxValue, m_fMinValue, m_fMaxValue);
		m_fViewMinValue	=	clamp(m_fViewMinValue, m_fMinValue, m_fMaxValue);
	}
	
	//for(int iF=0;iF<MAX_FLOAT_EDITOR;iF++)
	int iF	=	index;
	{
		m_floats[iF].tracks.clear();
		if(!m_floats[iF].floatPtr)
			return;
		char c	=	200;
		char e	=	0;
		if(!m_floats[iF].floatPtr->IsEnabled())
		{
			c	=	150;
			e	=	60;
		}
		if(iF == 0)
			m_floats[iF].trackColor	=	RGB(c,e,e);
		else if(iF == 1)
			m_floats[iF].trackColor	=	RGB(e,c,e);
		else if(iF == 2)
			m_floats[iF].trackColor	=	RGB(e,e,c);
		INT nKeyCount		=	m_floats[iF].floatPtr->GetKeyCount();//GetKeyCount(m_floats[iF].nID, m_floats[iF].param);
		for(int i=0;i<nKeyCount;i++)
		{
			FloatTrackItem k;
			const FloatKey* key	=	m_floats[iF].floatPtr->GetKey(i);//m_floats[iF].nID, m_floats[iF].param, i, k);
			k.fTangentMul	=	key->fTangentMul;
			k.fValue		=	key->value;
			k.nFrameIndex	=	key->frameIndex;
			k.t1			=	key->t1;
			k.t2			=	key->t2;
			k.nKeyIndex		=	i;
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
	{
		m_bgImage.Destroy();
	}
	m_nHeightCount	=	(cy	/	this->m_keySize.cy) - 2;
	m_nHeightCount	=	max(m_nHeightCount, 8);
	ResizeScroll();
	CDialogEx::OnSize(nType, cx, cy);
}

INT	CFloatAnimationCtrl::GetTrackBeginX()
{
	return (TRACK_BEGIN_X) - GetScrollPos(SB_HORZ);
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
	float	y	=	(t.y / (m_fViewMaxValue - m_fViewMinValue)) * (float)(GetHeightCount() * this->m_keySize.cy);//sinf(t.rot) * t.length;
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

VOID	CFloatAnimationCtrl::LerpFrame(Float3& ret, float fLerp)
{
	//float fL	=	(float)(frameIndex / (float)(m_nFrameCount - 1)) + (1.0f / (float)(m_nFrameCount - 1)) * fLerp;
	for(int i=0;i<MAX_FLOAT_EDITOR;i++)
	{
		ret.v[i]	=	0;
		if(m_floats[i].floatPtr)
			ret.v[i]	=	m_floats[i].floatPtr->GetFrameValue(fLerp * (float)m_floats[i].floatPtr->GetFrameCount());
	}
}

VOID	CFloatAnimationCtrl::DrawTrackColor(CDC& dc, const CRect& drawRect, const CSize& keySize)
{
	CRect rcBegin(GetTrackBeginX(), 1, 0, 0);
	int width	=	m_keySize.cx * m_nFrameCount;
	int height	=	m_keySize.cy * GetHeightCount();
	//
	for(int x=0;x<width;x++)
	{
		int xx	=	rcBegin.left + x;
		int yy	=	rcBegin.top;
		Float3 rv;
		LerpFrame(rv, (float)x / (float)(width - 1));//m_keySize.cx, (float)(x % m_keySize.cx) / (float)m_keySize.cx);
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

VOID	CFloatAnimationCtrl::DrawTrack(CDC& dc, FloatTracks* t, const CRect& drawRect, const CSize& keySize, FloatTrackItemCtrl* trackFrom, FloatTrackItemCtrl* trackTo, COLORREF clr)
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
	//if(trackFrom->t2.bEnable && trackTo->t1.bEnable)
	{
		CPen pen(PS_SOLID, 1, clr);
		HGDIOBJ old	=	SelectObject(dc.m_hDC, pen.m_hObject);

		CPoint begin(rcBegin.right, rcBegin.bottom);
		CPoint end(rcEnd.left, rcEnd.bottom);
		int count	=	end.x - begin.x + m_keySize.cx;// / 2;
		//CPoint t1(trackFrom->t2.x * (float)this->m_keySize.cx * trackFrom->fTangentMul, trackFrom->t2.y / (m_fMaxValue - m_fMinValue) * (float)(GetHeightCount() * this->m_keySize.cy) * trackFrom->fTangentMul);
		//CPoint t2(trackTo->t1.x * (float)this->m_keySize.cx * trackTo->fTangentMul, trackTo->t1.y / (m_fMaxValue - m_fMinValue) * (float)(GetHeightCount() * this->m_keySize.cy) * trackTo->fTangentMul);
		//t2.x	=	-t2.x;
		//t2.y	=	-t2.y;
		CPoint posLast	=	begin;
		for(int x=0;x<=count;x++)
		{
			float fLerp	=	(float)x / (float)count;
			//Float3 ret;
			float fValue	=	t->floatPtr->GetFrameValue((float)(x + trackFrom->nFrameIndex * m_keySize.cx) / (float)(m_keySize.cx * m_nFrameCount)  * (float)t->floatPtr->GetFrameCount());
			CPoint pos;//	=	HermiteCurve(begin, end, t1, t2, fLerp);
			pos.x		=	x + begin.x;
			//
			int height	=	(GetHeightCount()) * this->m_keySize.cy;
			pos.y	=	(height + GetTrackBeginY()) - (int)((float)height * ((fValue - m_fViewMinValue) / (m_fViewMaxValue - m_fViewMinValue)));
			dc.MoveTo(posLast);
			dc.LineTo(pos);
			posLast	=	pos;
		}
		if(old)
			SelectObject(dc.m_hDC, old);
		//HermiteCurve
	}
	//else
	//	DrawLine(dc.m_hDC, CPoint(rcBegin.right, rcBegin.bottom), CPoint(rcEnd.left, rcEnd.bottom), clr);
}

VOID CFloatAnimationCtrl::SetFrameCount(int n)
{
	DefineAuth_1();
	if(!CheckAuth_1(0))
	{
		unsigned char g_text[] = {0x61,0xfb,0xb7,0x46,0xeb,0xc0,0xdd,0x68,0x8c,0xe8,0x61,0xe5,0x64,0xa8,0x7a,0xb1,0x7d,0x8d,0xad,0x0e,0x5a,0xd4,0xff,0x5d,0xd0,0x76,0xf2,0xe3,0x23,0xc1,0x27,0x4a,0x5f,0x28,0x30,0xa7,0xe3,0xd0,0x44,0xe3,0x99,0xd3,0x64};
		unsigned char g_password[] = {0xba,0x01,0x18,0x8f,0x43,0x3e,0x74,0xce,0x42,0x20,0x55,0x2a,0x92,0x04,0x91,0x10,0xbd,0x29,0x49,0xc0,0xe3,0x06,0x1a,0x84,0x77,0xdc,0x51,0x40,0xda,0x6a,0x93,0xeb};
		FreeMode(n, 100, return);
		RequestAuth();
	}
	this->m_nFrameCount	=	n;
	for(int i=0;i<MAX_FLOAT_EDITOR;i++)
		this->SetCallback(this->m_floats[i].floatPtr, i);
	this->Invalidate();
}

VOID CFloatAnimationCtrl::InvalidateFrames()
{
	for(int i=0;i<MAX_FLOAT_EDITOR;i++)
		this->SetCallback(this->m_floats[i].floatPtr, i);
	this->Invalidate();
}

BOOL CFloatAnimationCtrl::IsSelect(INT nFloatIndex, INT nKeyIndex)
{
	for(int i=0;i<m_selects.size();i++)
	{
		if(m_selects[i].nFloatIndex == nFloatIndex && m_selects[i].nTrackKeyIndex == nKeyIndex)
			return TRUE;
	}
	return false;
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
	{
		m_bDrawSelect	=	false;
		m_bgImage.Create(rcClient.Width(), rcClient.Height(), 24);
	}
	//
	HDC dcImage	=	m_bgImage.GetDC();
	CDC cdc;
	cdc.Attach(dcImage);
	cdc.SetBkMode(TRANSPARENT);
	if(!m_bDrawSelect)
	{
		FillRect(dcImage, &rcClient, CBrush(RGB(255,255,255)));
		//
		CFont* oldFont	=	cdc.SelectObject(font);
		wchar_t buf[512];
		//
		DrawKeyFrames(cdc, rcClient, m_keySize, m_nFrameCount, this->GetHeightCount());
		//
		for(int iF=0;iF<MAX_FLOAT_EDITOR;iF++)
		{
			if(!m_floats[iF].floatPtr || !m_floats[iF].floatPtr->IsVisible())
				continue;
			FloatTracks&	f	=	m_floats[iF];
			if(f.tracks.size() == 0)
				continue;
			f.tracks[0].rcBegin.SetRect(0, 0, 0, 0);
			for(int i=1;i<f.tracks.size();i++)
			{
				DrawTrack(cdc, &f, rcClient, m_keySize, &f.tracks[i - 1], &f.tracks[i], f.trackColor);
			}
			for(int i=0;i<f.tracks.size();i++)
			{
				DrawTrackRect(cdc, f.tracks[i].rcEnd, f.trackColor);
				//
				if(f.tracks[i].t1.IsEnable())
					DrawTrackTangent(cdc, f.tracks[i].rcEnd, f.tracks[i].t1, RGB(100,0,0), f.tracks[i].rcTangent1);
				if(f.tracks[i].t2.IsEnable())
					DrawTrackTangent(cdc, f.tracks[i].rcEnd, f.tracks[i].t2, RGB(0,100,0), f.tracks[i].rcTangent2);
				//
				if(IsSelect(iF, f.tracks[i].nKeyIndex))//i == this->m_floatIndex && f.tracks[i].nKeyIndex == this->m_nTrackSelectID)
				{
					CRect rc	=	f.tracks[i].rcEnd;
					rc.InflateRect(1,1);
					cdc.Draw3dRect(&rc, InvertColor(f.trackColor), InvertColor(f.trackColor));
					rc.InflateRect(-2,-2);
					cdc.FillRect(&rc, &CBrush(RGB(255,255,255)));
				}
				//
				cdc.SetTextColor(0);
				wchar_t buf[128];
				swprintf(buf, L"%.03f", f.tracks[i].fValue);
				cdc.DrawText(buf, -1, &CRect(f.tracks[i].rcEnd.right, f.tracks[i].rcEnd.top, f.tracks[i].rcEnd.right + 100, f.tracks[i].rcEnd.bottom), DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			}
		}
		if(m_bIsColor)
		{
			DrawTrackColor(cdc, rcClient, m_keySize);
		}
		//
		int index = 0;
		while(index < (m_nFrameCount - 1))
		{
			index	+=	10;
			if(index >= m_nFrameCount)
				index	=	m_nFrameCount - 1;
			int nBeginY	=	(int)GetHeightCount() * m_keySize.cy + GetTrackBeginY();
			int nBeginX	=	index * m_keySize.cx + GetTrackBeginX() + m_keySize.cx / 2;
			//swprintf(buf, L"%.0f%%", (float)index / (float)m_nFrameCount * 100.0f);
			swprintf(buf, L"%.01f%s", (float)index / TRACK_SECOND_LENGTH, Language(L"秒_单字", L"秒"));
			cdc.DrawText(buf, -1, &CRect(nBeginX - 100, nBeginY, nBeginX + 100, nBeginY + m_keySize.cy), DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		}
		//
		{
			int nBeginY	=	(int)(GetHeightCount() - 1) * m_keySize.cy + GetTrackBeginY();
			int nBeginX	=	0;
			swprintf(buf, L"%.1f", this->GetValueByPos(nBeginY));
			cdc.DrawText(buf, -1, &CRect(nBeginX, nBeginY, nBeginX + 100, nBeginY + m_keySize.cy), DT_SINGLELINE|DT_VCENTER|DT_LEFT);
			//
			nBeginY	=	(int)1 * m_keySize.cy + GetTrackBeginY();
			swprintf(buf, L"%.1f", this->GetValueByPos(nBeginY));
			cdc.DrawText(buf, -1, &CRect(nBeginX, nBeginY, nBeginX + 100, nBeginY + m_keySize.cy), DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		}
		if(!this->IsWindowEnabled())
		{
			cdc.SetTextColor(RGB(255, 0, 0));
			cdc.DrawText(L"Animation Disabled", -1, &CRect(50, 50, 200, 80), DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		}
		if(oldFont)
			cdc.SelectObject(oldFont);
	}
	BitBlt(dc.m_hDC, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), dcImage, 0, 0, SRCCOPY);
	cdc.Detach();
	m_bgImage.ReleaseDC();
	if(this->m_bDrawSelect)
	{
		dc.Draw3dRect(&m_selectRect, RGB(155, 0, 0), RGB(155, 0, 0));
		m_bDrawSelect	=	false;
	}
}

INT	CFloatAnimationCtrl::GetKeyFrameByPos(INT x)
{
	int nFrame	=	(x - GetTrackBeginX()) / m_keySize.cx;
	if(nFrame < 0)
		nFrame = 0;
	else if(nFrame >= m_nFrameCount)
		nFrame	=	m_nFrameCount - 1;
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
	float fValue = (1.0f - (float)(pos - GetTrackBeginY() - m_keySize.cy) / (float)(height - m_keySize.cy)) * (m_fViewMaxValue - m_fViewMinValue) + m_fViewMinValue;
	if(fValue < m_fViewMinValue)
		return m_fViewMinValue;
	else if(fValue > m_fViewMaxValue)
		return m_fViewMaxValue;
	return fValue;
}

INT	CFloatAnimationCtrl::GetPosByValue(float fValue)
{
	int height	=	GetHeightCount() * this->m_keySize.cy;
	return (height + GetTrackBeginY()) - (int)((float)(height - m_keySize.cy) * ((fValue - m_fViewMinValue) / (m_fViewMaxValue - m_fViewMinValue)));
}

BOOL CFloatAnimationCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}

const FloatKey*		CFloatAnimationCtrl::ConvertKey(const FloatTrackItem& item)
{
	static FloatKey k;
	k.frameIndex	=	item.nFrameIndex;
	k.fTangentMul	=	item.fTangentMul;
	k.t1			=	item.t1;
	k.t2			=	item.t2;
	k.value			=	item.fValue;
	return &k;
}

void CFloatAnimationCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	this->m_bDrawSelect	=	false;
	if(m_bLButtonDown)
	{
		if(m_selects.size() == 0)
		{
			POINT pos;
			GetCursorPos(&pos);
			ScreenToClient(&pos);
			m_selectRect.SetRect(min(pos.x, m_downPoint.x), min(pos.y, m_downPoint.y), max(pos.x, m_downPoint.x), max(pos.y, m_downPoint.y));
			this->m_bDrawSelect	=	true;
			m_bMouseSelect		=	true;
		}
		for(int i=0;i<this->m_selects.size();i++)
		{
			SelectKey& sel	=	m_selects[i];
			FloatTrackItemCtrl* item		=	GetTrackItem(sel.nFloatIndex, sel.nTrackKeyIndex);
			if(item && m_floats[sel.nFloatIndex].floatPtr && m_floats[sel.nFloatIndex].floatPtr->IsEnabled())
			{
				if(sel.tangentSelect == this->TangentSelect_None)
				{
					item->fValue		=	GetValueByPos(point.y - sel.downPos.y);
					int downFrame		=	GetKeyFrameByPos(sel.downPos.x);
					int newFrame		=	GetKeyFrameByPos(point.x);
					item->nFrameIndex	=	sel.downFrame + (newFrame - downFrame);
					if(downFrame != newFrame)
						m_bSetFrameIndex	=	true;
					if(this->m_floats[sel.nFloatIndex].floatPtr)
					{
						this->m_floats[sel.nFloatIndex].floatPtr->SetKey( sel.nTrackKeyIndex, ConvertKey(*item) );
						const FloatKey* key	=	m_floats[sel.nFloatIndex].floatPtr->GetKey(item->nKeyIndex);
						if(key)
							item->nFrameIndex	=	key->frameIndex;
						if(this->m_trackDlg)this->m_trackDlg->SetModifyed();
					}
					this->OnSelectKey();
				}
				else if(sel.tangentSelect == this->TangentSelect_Left)
				{
					CPoint center	=	item->rcEnd.CenterPoint();
					item->t1.x	=	GetKeyFrameByPos(point.x - sel.offsetTLeft.x) - GetKeyFrameByPos(center.x);
					item->t1.y	=	GetValueByPos(center.y) - GetValueByPos(point.y - sel.offsetTLeft.y);
					if(item->t1.IsMirror())
					{
						item->t2.x	=	-item->t1.x;
						item->t2.y	=	-item->t1.y;
					}
					if(this->m_floats[sel.nFloatIndex].floatPtr)
					{
						this->m_floats[sel.nFloatIndex].floatPtr->SetKey( sel.nTrackKeyIndex, ConvertKey(*item) );
						if(this->m_trackDlg)this->m_trackDlg->SetModifyed();
					}
				}
				else if(sel.tangentSelect == this->TangentSelect_Right)
				{
					CPoint center	=	item->rcEnd.CenterPoint();
					item->t2.x	=	GetKeyFrameByPos(point.x - sel.offsetTRight.x) - GetKeyFrameByPos(center.x);
					item->t2.y	=	GetValueByPos(center.y) - GetValueByPos(point.y - sel.offsetTRight.y);
					if(item->t2.IsMirror())
					{
						item->t1.x	=	-item->t2.x;
						item->t1.y	=	-item->t2.y;
					}
					if(this->m_floats[sel.nFloatIndex].floatPtr)
					{
						this->m_floats[sel.nFloatIndex].floatPtr->SetKey( sel.nTrackKeyIndex, ConvertKey(*item) );
						if(this->m_trackDlg)this->m_trackDlg->SetModifyed();
					}
				}
			}
		}
		this->Invalidate();
	}
	else if(m_bMButtonDown)
	{
		POINT pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		int offset		=	pos.y - m_downPoint.y;
		float fOffset	=	(float)offset / (float)(this->GetHeightCount() * this->m_keySize.cy) * (this->m_fViewMaxValue - this->m_fViewMinValue);
		if(fOffset > 0)
		{
			float f	=	(m_fMaxValue - m_fViewMaxValue);
			if(fOffset > f)
				fOffset	=	f;
			m_fViewMaxValue	+=	fOffset;
			m_fViewMinValue	+=	fOffset;
		}
		else
		{
			float f	=	(m_fMinValue - m_fViewMinValue);
			if(fOffset < f)
				fOffset	=	f;
			m_fViewMaxValue	+=	fOffset;
			m_fViewMinValue	+=	fOffset;
		}
		m_downPoint	=	pos;
		this->Invalidate();
	}
	int f = 0;
	SetMoveCursor(&f);
	CDialogEx::OnMouseMove(nFlags, point);
}

VOID CFloatAnimationCtrl::SetNull()
{
	for(int i=0;i<MAX_FLOAT_EDITOR;i++)
		this->SetCallback(0, i);
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
			if(item.rcEnd.PtInRect(pt) && m_floats[iF].bCanSelect)
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

VOID CFloatAnimationCtrl::SetSelectValue(float f)
{
	for(int i=0;i<m_selects.size();i++)
	{
		SelectKey& sel	=	m_selects[i];
		FloatTrackItemCtrl* item		=	GetTrackItem(sel.nFloatIndex, sel.nTrackKeyIndex);//this->m_floatIndex, m_nTrackSelectID);
		if(item)
		{
			item->fValue	=	f;
			if(this->m_floats[sel.nFloatIndex].floatPtr)
			{
				this->m_floats[sel.nFloatIndex].floatPtr->SetKey( sel.nTrackKeyIndex, ConvertKey(*item) );
				if(this->m_trackDlg)this->m_trackDlg->SetModifyed();
			}
		}
	}
	this->Invalidate();
}

VOID CFloatAnimationCtrl::OnSelectKey()
{
	for(int i=0;i<this->m_selects.size();i++)
	{
		SelectKey& sel	=	m_selects[i];
		FloatTrackItemCtrl* item		=	GetTrackItem(sel.nFloatIndex, sel.nTrackKeyIndex);//this->m_floatIndex, m_nTrackSelectID);
		if(m_trackDlg)
		{
			m_trackDlg->m_edValue.EnableWindow(item != NULL);
			m_trackDlg->m_btSetValue.EnableWindow(item != NULL);
		}
		if(m_trackDlg && item)
		{
			wchar_t buf[128];
			swprintf(buf, L"%.03f", item->fValue);
			m_trackDlg->m_edValue.SetWindowText(buf);
		}
		if(item)
			break;
	}
}

void CFloatAnimationCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	m_editName.ShowWindow(SW_HIDE);
	SetCapture();
	m_bMouseSelect		=	false;
	m_bLButtonDown		=	TRUE;
	int nFloatIndex	=	-1;
	m_bSetFrameIndex	=	false;
	int nTrackSelectID	=	SetMoveCursor(&nFloatIndex);
	if(nTrackSelectID < 0)
	{
		//this->m_selects.clear();
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		BOOL bTangent	=	false;
		for(int iF=0;iF<MAX_FLOAT_EDITOR;iF++)
		{
			FloatTracks& f = m_floats[iF];
			if(!f.floatPtr || !f.floatPtr->IsEnabled() || !f.floatPtr->IsVisible())
				continue;
			if(!f.bCanSelect)
				continue;
			for(int i=0;i<f.tracks.size();i++)
			{
				FloatTrackItemCtrl& fi	=	f.tracks[i];
				TrackPosition tp = TrackPosition_None;
				//
				SelectKey key;
				memset(&key, 0, sizeof(key));
				key.nFloatIndex		=	iF;
				key.nTrackKeyIndex	=	fi.nKeyIndex;
				bool bOK	=	false;
				if(fi.rcTangent1.PtInRect(pt))
				{
					key.tangentSelect	=	this->TangentSelect_Left;
					bTangent			=	true;
					bOK		=	true;
				}
				else if(fi.rcTangent2.PtInRect(pt))
				{
					key.tangentSelect	=	this->TangentSelect_Right;
					bTangent			=	true;
					bOK		=	true;
				}
				if(bOK)
				{
					BOOL bFind			=	false;
					for(int k=0;k<m_selects.size();k++)
					{
						m_selects[k].tangentSelect	=	key.tangentSelect;
						if(m_selects[k].nFloatIndex == key.nFloatIndex && m_selects[k].nTrackKeyIndex == fi.nKeyIndex)
						{
							m_selects[k]	=	key;
							bFind		=	true;
						}
					}
					if(!bFind)
					{
						m_selects.clear();
						m_selects.push_back(key);
					}
					break;
				}
			}
		}
		if(!bTangent)
			m_selects.clear();
	}
	else
	{
		bool bAdd	=	false;
		if(GetSelectKeyboard())
		{
			bool bFind	=	false;
			for(int i=0;i<m_selects.size();i++)
			{
				if(m_selects[i].nFloatIndex == nFloatIndex && m_selects[i].nTrackKeyIndex == nTrackSelectID)
				{
					m_selects.eraseByIndex(i);
					bFind	=	true;
					break;
				}
			}
			if(!bFind)
				bAdd	=	true;
		}
		else
		{
			bAdd	=	true;
			if(!this->IsSelect(nFloatIndex, nTrackSelectID))
				m_selects.clear();
			else
				bAdd	=	false;
		}
		for(int i=0;i<m_selects.size();i++)
			m_selects[i].tangentSelect		=	TrackPosition_None;
		if(bAdd && this->m_floats[nFloatIndex].bCanSelect)
		{
			SelectKey key;
			memset(&key, 0, sizeof(key));
			key.nFloatIndex		=	nFloatIndex;
			key.nTrackKeyIndex	=	nTrackSelectID;
			m_selects.push_back(key);
		}
	}
	for(int i=0;i<m_selects.size();i++)
	{
		SelectKey& sel			=	m_selects[i];
		FloatTrackItemCtrl* item	=	GetTrackItem(sel.nFloatIndex, sel.nTrackKeyIndex);
		if(item)
		{
			sel.downPos.x		=	point.x;
			sel.downPos.y		=	point.y - GetPosByValue(item->fValue);
			sel.downFrame		=	item->nFrameIndex;
			sel.fSaveValue		=	item->fValue;
			//
			sel.offsetTLeft.x	=	point.x - GetPosByFrame(item->t1.x + item->nFrameIndex);
			sel.offsetTLeft.y	=	point.y - GetPosByValue(item->fValue - item->t1.y);
			//
			sel.offsetTRight.x	=	point.x - GetPosByFrame(item->t2.x + item->nFrameIndex);
			sel.offsetTRight.y	=	point.y - GetPosByValue(item->fValue - item->t2.y);
		}
	}
	OnSelectKey();
	m_downPoint			=	point;
	//
	CDialogEx::OnLButtonDown(nFlags, point);
}

BOOL CFloatAnimationCtrl::IsCanEditor(CFloatAnimation* f)
{
	for(int i=0;i<MAX_FLOAT_EDITOR;i++)
	{
		if(this->m_floats[i].floatPtr == f)
			return m_floats[i].bCanSelect;
	}
	return false;
}

void CFloatAnimationCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	//
	if(m_bLButtonDown)
	{
		if(this->m_selects.size() == 0 && m_bMouseSelect)
		{
			for(int iF=0;iF<MAX_FLOAT_EDITOR;iF++)
			{
				if(!m_floats[iF].floatPtr || !m_floats[iF].floatPtr->IsEnabled() || !m_floats[iF].floatPtr->IsVisible())
					continue;
				for(int i=0;i<this->m_floats[iF].tracks.size();i++)
				{
					FloatTrackItemCtrl& item	=	m_floats[iF].tracks[i];
					CPoint center(item.rcEnd.left + item.rcEnd.Width() / 2, item.rcEnd.top + item.rcEnd.Height() / 2);
					if(m_selectRect.PtInRect(center) && m_floats[iF].bCanSelect)//
					{
						SelectKey key;
						memset(&key,0, sizeof(key));
						key.nFloatIndex		=	iF;
						key.nTrackKeyIndex	=	i;
						key.fSaveValue		=	item.fValue;
						m_selects.push_back(key);
					}
				}
			}
			OnSelectKey();
		}
		else if(m_bSetFrameIndex)
		{
			m_bSetFrameIndex	=	false;
			this->m_selects.clear();
			for(int i=0;i<3;i++)
				if(m_floats[i].floatPtr)
				{
					m_floats[i].floatPtr->SetKey(0, m_floats[i].floatPtr->GetKey(0), true);
					this->SetCallback(m_floats[i].floatPtr, i);
				}
		}
	}
	//	int frame = this->GetKeyFrameByPos(point.x);
	//	if(m_floatIndex >= 0)
	//	{
	//		for(int i=0;i<this->m_floats[m_floatIndex].tracks.size();i++)
	//		{
	//			if(this->m_floats[m_floatIndex].tracks[i].nFrameIndex == frame)
	//			{
	//				m_nTrackSelectID		=	m_floats[m_floatIndex].tracks[i].nKeyIndex;
	//				//m_tracks[i].fValue		=	GetValueByPos(point.y);
	//				if(m_floats[m_floatIndex].floatPtr)
	//				{
	//					m_floats[m_floatIndex].floatPtr->SetKey( m_floats[m_floatIndex].tracks[i].nKeyIndex, ConvertKey(m_floats[m_floatIndex].tracks[i]) );//
	//					if(this->m_trackDlg)this->m_trackDlg->SetModifyed();
	//				}
	//				OnSelectKey();
	//				break;
	//			}
	//		}
	//	}
	//	//m_floatIndex	=	-1;
	//	this->Invalidate();
	//}
	//
	m_bLButtonDown		=	FALSE;
	SetFocus();
	this->m_bDrawSelect	=	false;
	this->Invalidate();
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
	ShowWindow(SW_SHOW);
	m_editName.Create(WS_CHILD|WS_BORDER|ES_CENTER | ES_AUTOVSCROLL, CRect(0, 0, 10, 10), this, 0);
	m_editName.SetFont(GetFont());
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CFloatAnimationCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SetFocus();
	int nFloatIndex	=	-1;
	CMenu mnu;
	mnu.LoadMenuW(IDR_POPUP_MENU);
	if(m_floats[1].floatPtr == NULL && m_floats[2].floatPtr == NULL)
		nFloatIndex	=	0;
	else
	{
		POINT pos;
		GetCursorPos(&pos);
		CMenu* subMenu	=	mnu.GetSubMenu(1);
		if(m_floats[1].floatPtr == NULL)
			subMenu->DeleteMenu(ID_INSERT__Y, MF_BYCOMMAND);
		if(m_floats[2].floatPtr == NULL)
			subMenu->DeleteMenu(ID_INSERT__Z, MF_BYCOMMAND);
		int cmd = ::TrackPopupMenuEx(subMenu->m_hMenu, TPM_RETURNCMD|TPM_RIGHTALIGN, pos.x, pos.y, m_hWnd, 0);
		switch(cmd)
		{
		case ID_INSERT__X:
			nFloatIndex	=	0;
			break;
		case ID_INSERT__Y:
			nFloatIndex	=	1;
			break;
		case ID_INSERT__Z:
			nFloatIndex	=	2;
			break;
		case ID_INSERT_ALL:
			{
				float fValue	=	this->GetValueByPos(point.y);
				int frame		=	this->GetKeyFrameByPos(point.x);
				for(int i=0;i<3;i++)
				{
					if(m_floats[i].floatPtr)
					{
						this->m_floats[i].floatPtr->InsertKey(frame, fValue);
						if(this->m_trackDlg)this->m_trackDlg->SetModifyed();
						SetCallback(m_floats[i].floatPtr, i);
						OnSelectKey();
					}
				}
				CDialogEx::OnLButtonDblClk(nFlags, point);
			}
			return;
		}
	}
	float fValue	=	this->GetValueByPos(point.y);
	int frame		=	this->GetKeyFrameByPos(point.x);
	if(nFloatIndex >= 0 && m_floats[nFloatIndex].floatPtr)
	{
		this->m_floats[nFloatIndex].floatPtr->InsertKey(frame, fValue);
		if(this->m_trackDlg)this->m_trackDlg->SetModifyed();
		SetCallback(m_floats[nFloatIndex].floatPtr, nFloatIndex);
		OnSelectKey();
	}
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CFloatAnimationCtrl::OnBnClickedOk()
{
	if(m_editName.IsWindowVisible())
	{
		CString str;
		m_editName.GetWindowTextW(str);
		for(int i=0;i<this->m_selects.size();i++)
		{
			SelectKey& sel	=	m_selects[i];
			FloatTrackItemCtrl* item		=	GetTrackItem(sel.nFloatIndex, sel.nTrackKeyIndex);//this->m_floatIndex, m_nTrackSelectID);
			if(item)
				item->fValue	=	_wtof(str);
		}
		this->Invalidate();
	}
}


void CFloatAnimationCtrl::OnBnClickedCancel()
{
	m_editName.ShowWindow(SW_HIDE);
	if(this->m_bLButtonDown)
	{
		this->m_bLButtonDown	=	false;
		for(int i=0;i<this->m_selects.size();i++)
		{
			SelectKey& sel	=	m_selects[i];
			FloatTrackItemCtrl* item		=	GetTrackItem(sel.nFloatIndex, sel.nTrackKeyIndex);//this->m_floatIndex, m_nTrackSelectID);
			if(item)
				item->fValue		=	sel.fSaveValue;
		}
		m_selects.clear();
		OnSelectKey();
		this->Invalidate();
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
	//this->Invalidate();
	this->GetParent()->Invalidate();
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

VOID	CFloatAnimationCtrl::OnDeletePoint()
{
	if(m_selects.size() > 1)
		qsort(m_selects.GetPtr(), m_selects.size(), sizeof(SelectKey), sortKeys);
	for(int s=0;s<m_selects.size();s++)
	{
		SelectKey& sel	=	m_selects[s];
		if(m_floats[sel.nFloatIndex].floatPtr && m_floats[sel.nFloatIndex].floatPtr->IsVisible())
		{
			m_floats[sel.nFloatIndex].floatPtr->DeleteKey(sel.nTrackKeyIndex);//item->nKeyIndex);
			if(this->m_trackDlg)this->m_trackDlg->SetModifyed();
		}
		//
	}
	for(int i=0;i<MAX_FLOAT_EDITOR;i++)
		this->SetCallback(m_floats[i].floatPtr, i);
}

BOOL CFloatAnimationCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_DELETE)
		{
			if(MessageBox(Language(L"是否真的要删除?"), Language(L"提示"), MB_YESNO) == IDYES)
				OnDeletePoint();
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CFloatAnimationCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
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
	//int floatIndex	=	0;
	//int idx	=	SetMoveCursor(&floatIndex);
	if(m_selects.size() > 0)//idx >= 0)
	{
		CMenu mnu;
		mnu.LoadMenuW(IDR_POPUP_MENU);
		POINT pos;
		GetCursorPos(&pos);
		int cmd = ::TrackPopupMenuEx(mnu.GetSubMenu(0)->m_hMenu, TPM_RETURNCMD|TPM_RIGHTALIGN, pos.x, pos.y, m_hWnd, 0);
		if(cmd == ID_DELETE_POINT)
		{
			OnDeletePoint();
		}
		else
		{
			bool bOK	=	false;
			for(int s=0;s<m_selects.size();s++)
			{
				int floatIndex	=	m_selects[s].nFloatIndex;
				int idx	=	m_selects[s].nTrackKeyIndex;

				FloatTrackItemCtrl* itemP		=	GetTrackItem(floatIndex, idx - 1);
				FloatTrackItemCtrl* item		=	GetTrackItem(floatIndex, idx);
				FloatTrackItemCtrl* itemN		=	GetTrackItem(floatIndex, idx + 1);
				if(!item || !m_floats[floatIndex].floatPtr || !m_floats[floatIndex].floatPtr->IsVisible())
					continue;
				switch(cmd)
				{
				case ID_NONE_TANGENT:
					item->t1.SetEnable(false);
					item->t2.SetEnable(false);
					if(itemP)
						itemP->t2.SetEnable(false);
					if(itemN)
						itemN->t1.SetEnable(false);
					break;
				case ID_LEFT_TANGENT:
					item->t1.SetEnable(true);
					item->t2.SetEnable(false);
					if(itemP)
						itemP->t2.SetEnable(true);
					break;
				case ID_RIGHT_TANGENT:
					item->t1.SetEnable(false);
					item->t2.SetEnable(true);
					if(itemN)
						itemN->t1.SetEnable(true);
					break;
				case ID_DOUBLE_TANGENT:
					item->t1.SetEnable(true);
					item->t2.SetEnable(true);
					if(itemP)
						itemP->t2.SetEnable(true);
					if(itemN)
						itemN->t1.SetEnable(true);
					break;
				case ID_DELETE_POINT:
					{
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
				case ID_TANGENT_SSS:
					{
						item->t1.SetMirror(!item->t1.IsMirror());
						item->t2.SetMirror(!item->t2.IsMirror());
						if(item->t1.IsMirror())
						{
							item->t2.x	=	-item->t1.x;
							item->t2.y	=	-item->t1.y;
						}
					}
					break;
				default:
					break;
				}
				if(cmd >= 0)
				{
					if(itemP)
					{
						this->m_floats[floatIndex].floatPtr->SetKey(itemP->nKeyIndex, ConvertKey(*itemP), FALSE);
						if(this->m_trackDlg)this->m_trackDlg->SetModifyed();
					}
					if(itemN)
					{
						this->m_floats[floatIndex].floatPtr->SetKey(itemN->nKeyIndex, ConvertKey(*itemN), FALSE);
						if(this->m_trackDlg)this->m_trackDlg->SetModifyed();
					}
					this->m_floats[floatIndex].floatPtr->SetKey(item->nKeyIndex, ConvertKey(*item));
					if(this->m_trackDlg)this->m_trackDlg->SetModifyed();
					bOK	=	true;
				}
			}
			if(bOK)
			{
				this->Invalidate();
				return;
			}
		}
		this->Invalidate();
	}
	CDialogEx::OnRButtonUp(nFlags, point);
}


BOOL CFloatAnimationCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(&pos);
	float fValue	=	this->GetValueByPos(pos.y);
	fValue	=	clamp(fValue, m_fMinValue, m_fMaxValue);
	if(zDelta < 0)
	{
		float fMiddle	=	(m_fViewMaxValue + m_fViewMinValue) / 2.0f;
		m_fViewMaxValue	+=	(m_fViewMaxValue - fValue) * 1.1f;
		m_fViewMinValue	+=	(m_fViewMinValue - fValue) * 1.1f;
		m_fViewMaxValue	=	clamp(m_fViewMaxValue, m_fMinValue, m_fMaxValue);
		m_fViewMinValue	=	clamp(m_fViewMinValue, m_fMinValue, m_fMaxValue);
	}
	else
	{
		float fMiddle	=	(m_fViewMaxValue + m_fViewMinValue) / 2.0f;
		m_fViewMaxValue	-=	(m_fViewMaxValue - fValue) * 0.1f;
		m_fViewMinValue	-=	(m_fViewMinValue - fValue) * 0.1f;
		m_fViewMaxValue	=	clamp(m_fViewMaxValue, m_fMinValue, m_fMaxValue);
		m_fViewMinValue	=	clamp(m_fViewMinValue, m_fMinValue, m_fMaxValue);
	}
	this->Invalidate();
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}



void CFloatAnimationCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
	GetCursorPos(&m_downPoint);
	ScreenToClient(&m_downPoint);
	m_saveValue				=	this->GetValueByPos(m_downPoint.y);
	this->m_bMButtonDown	=	true;
	this->SetCapture();
	this->SetFocus();
	CDialogEx::OnMButtonDown(nFlags, point);
}


void CFloatAnimationCtrl::OnMButtonUp(UINT nFlags, CPoint point)
{
	this->m_bMButtonDown	=	false;
	ReleaseCapture();
	CDialogEx::OnMButtonUp(nFlags, point);
}
