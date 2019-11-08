//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UITEXTBOX_H__
#define	__PHANTOM_UITEXTBOX_H__

#include "PhantomUIControl.h"
namespace Phantom{


	class UITextBox : public UIControl
	{
	public:
		UITextBox( BOOL bInit, UIDialog *dialogPtr = NULL );
		~UITextBox(){}

		virtual void 			Render( float fElapsedTime );
		virtual char 			canHaveFocus() { return (m_bVisible && IsEnabledCtrl() && m_bCanFocus); }
		virtual char 			onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex );

		virtual	char			LoadControl(CSafeFileHelperR& r);
		virtual	char			SaveControl(CSafeFileHelperW& w);
		char					m_bLButtonDown;
		INT						GetBorderWidth(){return m_borderWidth;}
		VOID					SetBorderWidth(int n){m_borderWidth = n;}
		virtual	VOID			OnEditClose(BOOL bIsCancel, const char* text);
		virtual	BOOL			IsPassword(){return m_bPassword;}
		//
		unsigned int 			GetType() const { return UIControlType_EDITBOX; }
		UIElement				m_bgElement;

		char					m_bPassword;
		int      				m_borderWidth;
		int      				m_nSpacing;
		Rect     				m_rcText;
		Rect     				m_rcRender[9];
		double   				m_dfBlink;
		double   				m_dfLastBlink;
		char     				m_bCaretOn;
		int      				m_nCaret;
		char     				m_bInsertMode;
		int      				m_nSelStart;
		int      				m_nFirstVisible;
		unsigned int 			m_TextColor;
		unsigned int 			m_SelTextColor;
		unsigned int 			m_SelBkColor;
		unsigned int 			m_CaretColor;
		char					m_bRenderBorder;
		char					m_bMouseDrag;
	};

};

#endif
