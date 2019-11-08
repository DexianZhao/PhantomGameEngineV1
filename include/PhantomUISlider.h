//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UISLIDER_H__
#define	__PHANTOM_UISLIDER_H__

#include "PhantomUIControl.h"
namespace Phantom{
	class UISlider : public UIControl
	{
	public:
		UISlider( BOOL bInit, UIDialog *dialogPtr = NULL );

		virtual BOOL 		isPtIn( Pixel pt ); 
		virtual char 		canHaveFocus() { return (m_bVisible && IsEnabledCtrl()); }
		virtual char 		onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex );
		void 				SetText( const char* strText );
		virtual void		recalcRects();
		virtual void		Render(float fElapsedTime );
		virtual	char		LoadControl(CSafeFileHelperR& r);
		virtual	char		SaveControl(CSafeFileHelperW& w);
		VOID				OnAnimationRect();
		static const int	MaxSliderElement	=	2;
		UIElement	m_sdElement[MaxSliderElement];
		virtual	INT			GetIntVal(){return m_value;}
		virtual	double		GetDoubleVal(){return (float)m_value/(float)(this->m_maxValue-m_minValue);}
		virtual	void 		SetIntVal( int nVal ){ SetValueI(m_value, 0); }
		virtual	void 		SetDoubleVal( double dVal ){ SetValueI(dVal*(float)(m_maxValue-m_minValue) + m_minValue, 0); }
		unsigned int 		GetType() const { return UIControlType_SLIDER; }
	protected:
		void 				SetValueI( int nValue, char bFromInput );
		int  				ValueFromPos( int x );

		int 				m_nDragX;
		int 				m_nDragOffset;
		int 				m_nButtonX;
		char 				m_bPressed;
		Rect 				m_rcButton;
	};


};

#endif
