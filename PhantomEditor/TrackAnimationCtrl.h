/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include <vector>
#include "resource.h"
#include "afxwin.h"

#define	TRACK_BEGIN_X	5
#define	TRACK_BEGIN_Y	5

// CTrackAnimationCtrl 对话框

struct	TrackItem
{
	INT			beginFrame;
	INT			endFrame;
	INT			id;
	INT			index;
	COLORREF	c;
	Phantom::NameW		name;
	//
	CRect		beginRc, endRc, bodyRc;
};

__interface	ITrackAnimationCallback
{
	INT				GetKeyFrameCount();
	BOOL			OnTrackChange(INT id, INT nBeginFrame, INT nEndFrame);	//如果返回TRUE表示可以被改变
	VOID			EnumTracks(std::vector<TrackItem>& rets);
};

class CTrackAnimationCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CTrackAnimationCtrl)

public:
	CTrackAnimationCtrl(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTrackAnimationCtrl();
	enum TrackPosition
	{
		TrackPosition_None	=	0,
		TrackPosition_Left,
		TrackPosition_Center,
		TrackPosition_Right,
	};

// 对话框数据
	enum { IDD = IDD_CONTROL_DLG };
	//
	VOID						SetCallback(ITrackAnimationCallback* cb);//
	TrackItem*					GetTrackItem(INT id);
	INT							SetMoveCursor(TrackPosition* ret);
	INT							GetKeyFrameByPos(INT x);
	INT							GetPosByFrame(INT nFrame);
	//
	INT							GetTrackBeginX();
	INT							GetTrackBeginY();
	VOID						ResizeScroll();
	VOID						DrawTrack(CDC& dc, const CRect& drawRect, const CSize& keySize, TrackItem& track);
	VOID						DrawKeyFrames(CDC& dc, const CRect& drawRect, const CSize& keySize, INT nFrameCount, INT nTrackCount);
	VOID						DrawTrackRect(CDC& dc, const CRect& rc, COLORREF c);
protected:
	CImage						m_bgImage;
	INT							m_keyCount;	//关键帧数量
	std::vector<TrackItem>		m_tracks;
	ITrackAnimationCallback*	m_callback;
	CSize						m_keySize;	//一个关键帧的大小
	BOOL						m_bLButtonDown;
	//
	INT							m_nTrackSelectID;
	TrackPosition				m_trackPosition;
	INT							m_nTrackBeginPixel;
	CEdit						m_editName;
	BOOL						m_bEditNameMode;
	CPoint						m_saveFrame;
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
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	CStatic m_stBegin;
	CEdit m_edBegin;
	CStatic m_stEnd;
	CEdit m_edEnd;
};

VOID		DrawLine(HDC dc, const CPoint& from, const CPoint& to, const CPoint& offset);
VOID		DrawLine(HDC dc, const CPoint& from, const CPoint& to, COLORREF c);
COLORREF	InvertColor(COLORREF c);
HCURSOR		GetCursorHandle(LPCTSTR str);

