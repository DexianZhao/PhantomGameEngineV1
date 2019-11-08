/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once


// CChangeSpinButtonCtrl
#include "afxspinbuttonctrl.h"

class CMFCPropertyGridIntProperty;
class CMFCPropertyGridFloatProperty;

class CChangeSpinButtonCtrl : public CMFCSpinButtonCtrl
{
public:
	CChangeSpinButtonCtrl();
	virtual ~CChangeSpinButtonCtrl();

	CMFCPropertyGridIntProperty	*	m_pIntProp;
	CMFCPropertyGridFloatProperty*	m_pFloatProp;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDeltapos(NMHDR *pNMHDR, LRESULT *pResult);
};


