/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// ChangeSpinButtonCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "ChangeSpinButtonCtrl.h"
#include "MyMFCPropertyGridProperty.h"

// CChangeSpinButtonCtrl

CChangeSpinButtonCtrl::CChangeSpinButtonCtrl()
{
	m_pIntProp		=	0;
	m_pFloatProp	=	0;
}

CChangeSpinButtonCtrl::~CChangeSpinButtonCtrl()
{
}


BEGIN_MESSAGE_MAP(CChangeSpinButtonCtrl, CMFCSpinButtonCtrl)
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, &CChangeSpinButtonCtrl::OnDeltapos)
END_MESSAGE_MAP()



// CChangeSpinButtonCtrl 消息处理程序




void CChangeSpinButtonCtrl::OnDeltapos(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if(pNMUpDown->iDelta > 0)
	{
		if(m_pIntProp)
			m_pIntProp->OnUpClick();
		else if(m_pFloatProp)
			m_pFloatProp->OnUpClick();
	}
	else
	{
		if(m_pIntProp)
			m_pIntProp->OnDownClick();
		else if(m_pFloatProp)
			m_pFloatProp->OnDownClick();
	}
	*pResult = 0;
}
