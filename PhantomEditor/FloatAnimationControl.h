#pragma once
#include <vector>
#include "resource.h"
#include "TrackAnimationCtrl.h"
#include "IEffect.h"

struct	FloatTrackItem
{
	INT				nKeyIndex;
	INT				nFrameIndex;
	float			fValue;
	float			fTangentMul;
	//
	tangent2		t1,t2;
};

struct	FloatTrackItemCtrl: FloatTrackItem
{
	CRect			rcBegin;
	CRect			rcEnd;
	CRect			rcTangent1,rcTangent2;
};

__interface	IFloatAnimationCtrlCB
{
	VOID			GetKeyData(INT id, INT param, INT nKeyIndex, FloatTrackItem& data);
	VOID			SetKeyData(INT id, INT param, INT nKeyIndex, FloatTrackItem& data);
	INT				GetKeyCount(INT id, INT param);
	INT				InsertKey(INT id, INT param, INT nFrameIndex/*0到200之间的值*/, float fValue);
	VOID			DeleteKey(INT id, INT param, INT nKeyIndex);
	VOID			SetKeyFrame(INT id, INT param, INT nKeyIndex, INT nFrameIndex);
};

#define	MAX_FRAME_COUNT		200

//使用三个浮点回调类来组织

struct	Float3
{
	float	v[MAX_FLOAT_EDITOR];
};

class CFloatAnimationCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CFloatAnimationCtrl)

public:
	CFloatAnimationCtrl(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFloatAnimationCtrl();
	enum TrackPosition
	{
		TrackPosition_None	=	0,
		TrackPosition_Track,
	};
// 对话框数据
	enum { IDD = IDD_CONTROL_DLG };
	//
	VOID						SetCallback(IFloatAnimationCtrlCB* cb, INT index, INT id, INT param);//
	VOID						SetRange(float fMin, float fMax);
	FloatTrackItemCtrl*			GetTrackItem(INT floatIndex, INT id);
	INT							SetMoveCursor(INT* floatSelect);
	INT							GetKeyFrameByPos(INT x);
	INT							GetPosByFrame(INT nFrame);
	INT							GetPosByValue(float fValue);
	float						GetValueByPos(INT pos);
	//
	INT							GetTrackBeginX();
	INT							GetTrackBeginY();
	VOID						ResizeScroll();
	VOID						DrawTrack(CDC& dc, const CRect& drawRect, const CSize& keySize, FloatTrackItemCtrl* trackFrom, FloatTrackItemCtrl* trackTo, COLORREF clr);
	VOID						DrawKeyFrames(CDC& dc, const CRect& drawRect, const CSize& keySize, INT nFrameCount, INT nTrackCount);
	VOID						DrawTrackRect(CDC& dc, const CRect& rc, COLORREF c);
	VOID						DrawTrackTangent(CDC& dc, const CRect& rc, const tangent2& t, COLORREF c, CRect& rcT);
	VOID						DrawTrackColor(CDC& dc, const CRect& drawRect, const CSize& keySize);
	INT							GetHeightCount(){return m_nHeightCount;}
	
	VOID						Setup(INT type, float fMin, float fMax, bool bIsColor = false, INT heightCount = 15);
	INT							m_nType;
	CPoint						m_downPoint;
	//
	int							ClampY(int y);
	struct	FloatTracks
	{
		IFloatAnimationCtrlCB*			callback;
		std::vector<FloatTrackItemCtrl>	tracks;
		INT								nID;
		COLORREF						trackColor;
		INT								param;
		FloatTracks(){callback = 0; nID = 0; param=0;}
	};
	VOID						FillFrame();
	Float3						m_frames[MAX_FRAME_COUNT];
	VOID						LerpFrame(Float3& ret, INT frameIndex, float fLerp);
	//
	//CFloatAnimation				m_temp;
	//CFloatAnimation*			m_floatPtr;
protected:
	CImage						m_bgImage;
	FloatTracks					m_floats[MAX_FLOAT_EDITOR];
	CSize						m_keySize;	//一个关键帧的大小
	BOOL						m_bLButtonDown;
	//
	INT							m_nHeightCount;
	//
	INT							m_floatIndex;
	INT							m_nTrackSelectID;
	TrackPosition				m_trackPosition;
	INT							m_nTrackBeginPixel;
	CEdit						m_editName;
	BOOL						m_bEditNameMode;
	CPoint						m_saveFrame;
	float						m_fMaxValue, m_fMinValue;
	float						m_saveValue;
	enum{
		TangentSelect_None	=	0,
		TangentSelect_Left,
		TangentSelect_Right,
		TangentSelect_Full,
	};
	INT							m_tangentSelect;
	BOOL						m_bIsColor;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};
