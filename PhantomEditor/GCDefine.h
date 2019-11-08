/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#ifndef __GC_DEFINE_H__
#define	__GC_DEFINE_H__

#define	SOFTWARE_NAME			"PhantomGameConstructors"
#define	SOFTWARE_NAME_W			L"PhantomGameConstructors"

enum
{
	StateID_Default	=	1,
};

VOID	UpdateMainText(const wchar_t* szEditorName, const wchar_t* szFileName = L"", BOOL bIsModifyed = false);
VOID	ShowFreeMessage(unsigned char* text, int textLen, unsigned char* textPass, int textPassLen);

#define	FreeMode(val, max, ret)	if(val >= max){ShowFreeMessage(g_text, sizeof(g_text), g_password, sizeof(g_password)); ret;}

#endif
