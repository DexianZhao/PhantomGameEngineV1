//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UISTATIC_FORMAT_H__
#define	__PHANTOM_UISTATIC_FORMAT_H__

#include "PhantomUIStatic.h"
#include "PhantomUIScrollBar.h"
#include "PhantomUITextFormat.h"


namespace Phantom{

	class UIStaticFormat : public UIStatic, public ITextFormatCallback
	{
	public:
		UIStaticFormat( BOOL bInit, UIDialog *dialogPtr = NULL );
		//
		virtual char 		onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex );
		virtual char 		canHaveFocus() { return (m_bVisible && IsEnabledCtrl()); }
		virtual void 		Render( float fElapsedTime );
		void 				SetText( const char* strText );
		void 				SetTextA( const char* strText );
		virtual	VOID		FrameMove(float fElapsedTime);
		virtual void 		recalcRects();
		virtual	char		LoadControl(CSafeFileHelperR& r);
		virtual	char		SaveControl(CSafeFileHelperW& w);
		virtual	void 		SetAnalyseText( const char* strText );	//设置格式化文本,必须支持IFormatEditBox的控件才会有效
		TextFormat*			GetTextFormatPtr(){return &m_formatText;}
		TextFormat			m_formatText;
		void				SetOrigText(const char* szText);
		VOID				OnSetAnimationRect();
		VOID				OnAnimationRect();
		VOID				OnEventClick(TextFormat* ptr, TextW& eventText, int parameter);
		VOID				OnEventMoving(TextFormat* ptr, TextW& eventText, int parameter);
		UIScrollBar*	GetScrollBar(){return &m_scrollBar;}
		UIScrollBar	m_scrollBar;
		int					GetElementCount();
		UIElement*	GetElementPtr(int index);
		unsigned int 		GetType() const { return UIControlType_VIEW_FORMAT_TEXT; }
	protected:
		friend class		UIDialog;
		char				m_bMouseMove;
		char				m_bLButtonDown;
		char				m_bPressed;
	};
};

#endif
