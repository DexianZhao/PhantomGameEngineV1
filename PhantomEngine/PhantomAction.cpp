//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   PhantomAction.cpp
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIDialog.h"
#include "PhantomManager.h"
#include "PhantomAction.h"

namespace Phantom{

	ActionBase::ActionBase()
	{
		m_context = 0;
		m_bBegin	=	true;
		m_next = 0;
		m_current = this;
	}
	ActionBase::~ActionBase()
	{
		safe_release(m_next);
	}

};
