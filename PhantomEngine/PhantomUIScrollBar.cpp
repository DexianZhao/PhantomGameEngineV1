//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIScrollBar.h"
#include "PhantomUIDialog.h"
#include "PhantomManager.h"

#define SB_MINTHUMBSIZE			32
#define SB_ARROWCLICK_DELAY		0.33
#define SB_ARROWCLICK_REPEAT	0.05


namespace Phantom{

	UIScrollBar::UIScrollBar( BOOL bInit, UIDialog *dialogPtr ) : UIControl(bInit, dialogPtr)
	{
		m_containPtr	=	0;
		m_controlType	= UIControlType_SCROLLBAR;
		m_dialogPtr		= dialogPtr;
		memset(&m_upRect, 0, sizeof(m_upRect));
		memset(&m_downRect, 0, sizeof(m_downRect));
		memset(&m_rcTrack, 0, sizeof(m_rcTrack));
		memset(&m_thumbRect, 0, sizeof(m_thumbRect));
		m_showThumb		= true;
		m_bDrag			= false;
		m_nPosition 	= 0;
		m_nPageSize 	= 1;
		m_nStart		= 0;
		m_nThumbSize	= 10;
		m_nEnd			= 100;
		m_arrowState	= STATE_CLEAR;
		m_arrowTime		= 0.0;
		m_bScrollExtend = true;
		m_fScrollSpeed	= 0.98;
		m_fScrollFirstSpeed = 3;
		ClearElement();
		for(int i=0;i<MaxScrollBarElement;i++)
			AddElement(&m_scElement[i]);
		//
		if(bInit)
		{
			Rect rcTexture(48, 18, 16, 16 );
			for(int i=0;i<MaxScrollBarElement;i++)
			{
				m_scElement[i].SetTexture( 0, &rcTexture );
				m_scElement[i].textureColor = color4(1,1,1,1);
			}
			m_scElement[1].SetSourceRect(Rect(247, 2, 247+7, 2 + 124));
			m_scElement[0].SetSourceRect(Rect(240, 4, 240+5, 4+120));
			//m_scElement[0].SetSourceRect(Rect(246, 2, 246+8, 2 + 124));
			//m_scElement[1].SetSourceRect(Rect(238, 4, 238+9, 4+120));
			m_scElement[0].SetRenderScale(true, false, Pixel(0, 15));
			m_scElement[1].SetRenderScale(true, false, Pixel(0, 15));
		}
	}
	UIScrollBar::~UIScrollBar()
	{
	}
	VOID UIScrollBar::OnAnimationRect()
	{CPUTime(UIScrollBar);
		m_upRect.Set(m_windowRect.left, m_windowRect.top, m_windowRect.right, m_windowRect.top + RectWidth( m_windowRect ) );
		m_downRect.Set(m_windowRect.left, m_windowRect.bottom - RectWidth( m_windowRect ), m_windowRect.right, m_windowRect.bottom );
		m_rcTrack = m_windowRect;//.Set(m_upRect.left, m_upRect.bottom, m_downRect.right, m_downRect.top );
		m_thumbRect = m_windowRect;
		recalcThumbRect();
	}
	void UIScrollBar::recalcRects()
	{CPUTime(UIScrollBar);
		UIControl::recalcRects();
		OnAnimationRect();
	}

	void UIScrollBar::recalcThumbRect()
	{CPUTime(UIScrollBar);
		m_rcTrack = m_windowRect;
		m_thumbRect = m_windowRect;
		if(m_rcTrack.GetWidth()>m_rcTrack.GetHeight()){
			int nMaxPosition = m_nEnd - m_nStart;
			m_thumbRect.left = m_rcTrack.left + ( m_nPosition - m_nStart ) * ( RectWidth( m_rcTrack ) - m_nThumbSize ) / nMaxPosition;
			m_thumbRect.right = m_thumbRect.left + m_nThumbSize;
		}
		else{
			int nMaxPosition = m_nEnd - m_nStart;
			m_thumbRect.top = m_rcTrack.top + ( m_nPosition - m_nStart ) * ( RectHeight( m_rcTrack ) - m_nThumbSize ) / nMaxPosition;
			m_thumbRect.bottom = m_thumbRect.top + m_nThumbSize;
		}
	}

	void UIScrollBar::SetScroll( int nDelta )
	{CPUTime(UIScrollBar);
		m_nPosition += nDelta;
		ClampValue();
		recalcThumbRect();
	}

	void UIScrollBar::ShowItem( int nIndex )
	{CPUTime(UIScrollBar);
		if( nIndex < 0 )
			nIndex = 0;
		if( nIndex >= m_nEnd )
			nIndex = m_nEnd - 1;
		if( m_nPosition > nIndex )
			m_nPosition = nIndex;
		else
			if( m_nPosition + m_nPageSize <= nIndex )
				m_nPosition = nIndex - m_nPageSize + 1;
		recalcThumbRect();
	}
	char UIScrollBar::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex )
	{CPUTime(UIScrollBar);
		return false;
	}
	char		UIScrollBar::LoadControl(CSafeFileHelperR& r)
	{CPUTime(UIScrollBar);
		UIControl::LoadControl( r );
		skip_r sr;
		if(m_loadVersion>=0x000000A4)
			sr.begin(r.pStream);
		r >> m_showThumb >> m_bDrag >> m_upRect >> m_downRect >> m_rcTrack >> m_thumbRect >> m_nPosition >> m_nPageSize >> m_nStart
			>> m_nEnd >> m_lastPoint >> m_arrowTime;
		sr.end();
		return true;
	}
	char		UIScrollBar::SaveControl(CSafeFileHelperW& w)
	{CPUTime(UIScrollBar);
		UIControl::SaveControl( w );
		skip_w sw(w.pStream);
		w << m_showThumb << m_bDrag << m_upRect << m_downRect << m_rcTrack << m_thumbRect << m_nPosition << m_nPageSize << m_nStart
			<< m_nEnd << m_lastPoint << m_arrowTime;
		sw.end();
		return true;
	}
	void UIScrollBar::RenderScroll( UIControl* parent, float fElapsedTime , INT nThumbSize )
	{CPUTime(UIScrollBar);
		UIElement* pElement = &m_scElement[0];
		g_manager->SetBlendMode((BlendMode)pElement->m_nSrcBlend,(BlendMode)pElement->m_nDestBlend);
		ControlAnimation* anim	=	&m_currentAnim;
		if(parent)
			anim	=	&parent->m_currentAnim;
		m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &m_rcTrack, pElement->textureColor, anim,this);
		pElement = &m_scElement[1];
		m_nThumbSize = nThumbSize;
		this->recalcThumbRect();
		m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &m_thumbRect, pElement->textureColor, anim,this);
	}
	
	void UIScrollBar::RenderScroll( UIControl* parent, float fElapsedTime )
	{CPUTime(UIScrollBar);
		UIElement* pElement = &m_scElement[0];
		g_manager->SetBlendMode((BlendMode)pElement->m_nSrcBlend,(BlendMode)pElement->m_nDestBlend);
		ControlAnimation* anim	=	&m_currentAnim;
		if(parent)
			anim	=	&parent->m_currentAnim;
		m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &m_rcTrack, pElement->textureColor, anim,this);
		pElement = &m_scElement[1];//m_elements.at( 1 );
		m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &m_thumbRect, pElement->textureColor, anim,this);
	}
	void UIScrollBar::Render( float fElapsedTime )
	{CPUTime(UIScrollBar);
		if(!m_bVisible)
			return;
		if(m_containPtr){
			RenderScroll(m_containPtr, g_manager.GetElapsedTime(), m_nThumbSize);
			return;
		}
		//if( m_arrowState != STATE_CLEAR )
		//{
		//	double dCurrTime = g_manager.GetAppTime();
		//	if( m_upRect.isPtIn( m_lastPoint.x, m_lastPoint.y ) )
		//	{
		//		switch( m_arrowState )
		//		{
		//		case STATE_UP_BUTTON:
		//			if( SB_ARROWCLICK_DELAY < dCurrTime - m_arrowTime )
		//			{
		//				SetScroll( -1 );
		//				m_arrowState = STATE_HOLD_UP;
		//				m_arrowTime = dCurrTime;
		//			}
		//			break;
		//		case STATE_HOLD_UP:
		//			if( SB_ARROWCLICK_REPEAT < dCurrTime - m_arrowTime )
		//			{
		//				SetScroll( -1 );
		//				m_arrowTime = dCurrTime;
		//			}
		//			break;
		//		}
		//	}
		//	else if( m_downRect.isPtIn( m_lastPoint.x, m_lastPoint.y ) )
		//	{
		//		switch( m_arrowState )
		//		{
		//		case STATE_CLICK_DOWN:
		//			if( SB_ARROWCLICK_DELAY < dCurrTime - m_arrowTime )
		//			{
		//				SetScroll( 1 );
		//				m_arrowState = STATE_HOLD_DOWN;
		//				m_arrowTime = dCurrTime;
		//			}
		//			break;
		//		case STATE_HOLD_DOWN:
		//			if( SB_ARROWCLICK_REPEAT < dCurrTime - m_arrowTime )
		//			{
		//				SetScroll( 1 );
		//				m_arrowTime = dCurrTime;
		//			}
		//			break;
		//		}
		//	}
		//}
		recalcThumbRect();
		this->RenderScroll(0, fElapsedTime);
		this->RenderDrawMgrs();
	}
	void UIScrollBar::SetScrollRange( int nStart, int nEnd )
	{CPUTime(UIScrollBar);
		m_nStart = nStart; m_nEnd = nEnd;
		ClampValue();
		recalcThumbRect();
	}
	void UIScrollBar::ClampValue()
	{CPUTime(UIScrollBar);
		if( m_nPosition < m_nStart || m_nEnd - m_nStart <= m_nPageSize )
		{
			m_nPosition = m_nStart;
		}
		else if( m_nPosition + m_nPageSize > m_nEnd )
			m_nPosition = m_nEnd - m_nPageSize;
	}
};
