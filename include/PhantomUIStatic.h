//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UISTATIC_H__
#define	__PHANTOM_UISTATIC_H__

#include "PhantomUIControl.h"
namespace Phantom{


	class UIStatic : public UIControl
	{
	public:
		UIStatic( BOOL bInit, UIDialog *dialogPtr = NULL );
		~UIStatic();

		virtual void 			Render( float fElapsedTime );
		virtual char 			canHaveFocus() { return (m_bVisible /*&& IsEnabledCtrl() */&& m_bCanFocus); }
		virtual char 			onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex );

		virtual	char			LoadControl(CSafeFileHelperR& r);
		virtual	char			SaveControl(CSafeFileHelperW& w);
		char					m_bLButtonDown;
		INT						GetBorderWidth(){return m_borderWidth;}
		VOID					SetBorderWidth(int n){m_borderWidth = n;}
		//
		unsigned int 			GetType() const { return UIControlType_STATIC; }
		UIElement				m_bgElement;
		UIDialog*				m_contain;//内容对话框
	protected:
		int						m_borderWidth;
	};

};

#endif
