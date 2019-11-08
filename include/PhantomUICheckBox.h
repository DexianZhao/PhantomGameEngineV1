//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UICHECKBOX_H__
#define	__PHANTOM_UICHECKBOX_H__

#include "PhantomUIButton.h"
namespace Phantom{

	class UICheckBox : public UIButton
	{
	public:
		UICheckBox( BOOL bInit, UIDialog *dialogPtr = NULL );

		virtual BOOL 		isPtIn( Pixel pt );
		virtual void 		recalcRects();
		unsigned int 		GetType() const { return UIControlType_CHECKBOX; }
		virtual char 		onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex );

		virtual void 		Render( float fElapsedTime );
		virtual	INT			GetIntVal(){return m_bChecked;}
		virtual	double		GetDoubleVal(){return m_bChecked;}
		virtual	void 		SetIntVal( int nVal ){ SetChecked(nVal); }
		virtual	void 		SetDoubleVal( double dVal ){ SetChecked((int)dVal); }

		char 				GetChecked() { return m_bChecked; }
		void 				SetChecked( char bChecked ) { SetCheckedInternal( bChecked, false ); }

		virtual	char		LoadControl(CSafeFileHelperR& r);
		virtual	char		SaveControl(CSafeFileHelperW& w);

		VOID				OnAnimationRect();
	protected:
		virtual void		SetCheckedInternal( char bChecked, char bFromInput );

		char 				m_bChecked;
		Rect 				m_rcButton;
		Rect 				m_rcText;
	};

};

#endif
