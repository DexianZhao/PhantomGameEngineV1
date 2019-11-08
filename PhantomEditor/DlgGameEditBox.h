/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once


// DlgGameEditBox
class CDlgEditBox;
class DlgGameEditBox : public CEdit
{
	DECLARE_DYNAMIC(DlgGameEditBox)

public:
	DlgGameEditBox();
	virtual ~DlgGameEditBox();
	CDlgEditBox*	m_dlgEdit;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};


