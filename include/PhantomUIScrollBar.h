//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UISCROLLBAR_H__
#define	__PHANTOM_UISCROLLBAR_H__

#include "PhantomUIControl.h"
namespace Phantom{

	class UIScrollBar : public UIControl
	{
	public:
		UIScrollBar( BOOL bInit, UIDialog *dialogPtr = NULL );
		virtual ~UIScrollBar();

		virtual char    	onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex );

		virtual void    	Render(float fElapsedTime );
		virtual void    	RenderScroll( UIControl* parent, float fElapsedTime , INT nThumbSize);
		virtual void    	RenderScroll( UIControl* parent, float fElapsedTime);
		virtual void    	recalcRects();
		virtual	INT			GetIntVal(){return m_nPosition;}
		virtual	double		GetDoubleVal(){return m_nPosition;}
		virtual	void 		SetIntVal( int nVal ){ SetScrollPos(nVal); if(m_containPtr)m_containPtr->OnScroll(this); }
		virtual	void 		SetDoubleVal( double dVal ){ SetScrollPos(dVal); if(m_containPtr)m_containPtr->OnScroll(this); }

		void				SetScrollRange( int nStart, int nEnd );
		int					GetScrollPos() { return m_nPosition; }
		void				SetScrollPos( int nPosition ) { m_nPosition = nPosition; ClampValue(); recalcThumbRect(); }
		int					GetPageSize() { return m_nPageSize; }
		void				SetPageSize( int nPageSize ) { m_nPageSize = nPageSize; ClampValue(); recalcThumbRect(); }

		void 				SetScroll( int nDelta );
		void 				ShowItem( int nIndex );
		virtual char 		canHaveFocus() { return this->m_bCanFocus; }

		virtual	char		LoadControl(CSafeFileHelperR& r);
		virtual	char		SaveControl(CSafeFileHelperW& w);

		VOID				OnAnimationRect();

		unsigned int 			GetType() const { return UIControlType_SCROLLBAR; }

		static const int	MaxScrollBarElement = 2;
		UIElement			m_scElement[MaxScrollBarElement];
		enum{ STATE_CLEAR, STATE_UP_BUTTON, STATE_CLICK_DOWN, STATE_HOLD_UP, STATE_HOLD_DOWN };
	
		UIControl*			m_containPtr;
		void 				recalcThumbRect();
		void 				ClampValue();
		char				m_bScrollExtend;
		float				m_fScrollSpeed;
		float				m_fScrollFirstSpeed;
		char 				m_showThumb;
		char 				m_bDrag;
		Rect 				m_upRect;
		Rect 				m_downRect;
		Rect 				m_rcTrack;
		Rect 				m_thumbRect;
		int					m_arrowState;
		int					m_nThumbSize;
		//
		int 				m_nPosition;
		int 				m_nPageSize;
		int 				m_nStart;
		int 				m_nEnd;
		Pixel				m_lastPoint;
		double				m_arrowTime;
	};

};

#endif
