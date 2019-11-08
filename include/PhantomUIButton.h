//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UIBUTTON_H__
#define	__PHANTOM_UIBUTTON_H__

#include "PhantomUIStatic.h"
namespace Phantom{

	class UIButton : public UIStatic
	{
	public:
		UIButton( BOOL bInit, UIDialog *dialogPtr = NULL );
		virtual char 		onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex );
		unsigned int 		GetType() const { return UIControlType_BUTTON; }
		virtual char 		canHaveFocus();
		virtual void 		Render( float fElapsedTime );
		static const int	MaxButtonElement = 4;
		UIElement	m_btnElement[MaxButtonElement];
		//
		BOOL				m_bDownClick;
		int					m_clickCount;
		double				m_fClickTimeWait;
		double				m_fClickTime;//按下持续时间间隔
		Pixel				m_downPt;
	protected:
		friend class		UIDialog;
		char				m_bMouseMove;
		int					m_bLButtonDown;
		int					m_bLButtonDownPush;
		int                 m_bPressed;
	};


};

#endif
