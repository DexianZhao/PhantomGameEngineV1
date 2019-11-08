//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUISlider.h"
#include "PhantomUIDialog.h"
#include "PhantomManager.h"

namespace Phantom{

	UISlider::UISlider( BOOL bInit, UIDialog *dialogPtr ) : UIControl(bInit, dialogPtr)
	{
		ClearElement();
		m_controlType = UIControlType_SLIDER;
		m_dialogPtr = dialogPtr;
		m_minValue = 0;
		m_maxValue = 10000;
		m_value = 0;
		m_bPressed = false;
		//
		for(int i=0;i<MaxSliderElement;i++)
			AddElement(&m_sdElement[i]);
		//
		if(bInit)
		{
			Rect rcTexture;
			rcTexture.Set(88, 53, 91, 5 );
			m_sdElement[0].SetTexture( 0, &rcTexture );
			m_sdElement[0].textureColor = color4( COLOR_ARGB(255, 255, 255, 255));
			m_sdElement[0].SetSourceRect(Rect(157, 9, 157 + 12, 9 + 11));
			m_sdElement[0].SetRenderScale(true, false, Pixel(3, 5));
//			m_sdElement[0].m_name = ("轨道条[条]");
			rcTexture.Set(24, 2, 19, 19 );
			m_sdElement[1].SetTexture( 0, &rcTexture );
			m_sdElement[1].SetSourceRect(Rect(176, 8, 176 + 12, 8 + 12));
			m_sdElement[1].SetRenderScale(true, false, Pixel(5, 5));
//			m_sdElement[1].m_name = ("轨道条[按钮]");
		}
		SetValueI(0, 0);
	}

	BOOL UISlider::isPtIn( Pixel pt ) 
	{CPUTime(UISlider);
		return ( m_windowRect.isPtIn( pt.x, pt.y ) || m_rcButton.isPtIn( pt.x, pt.y ) );
	}

	VOID UISlider::OnAnimationRect()
	{CPUTime(UISlider);
		m_rcButton = m_windowRect;
		m_rcButton.right = m_rcButton.left + RectHeight( m_rcButton );
		int nWidth			=	RectWidth( m_windowRect );
		int nBorderWidth	=	RectHeight( m_windowRect );
		if(nWidth < nBorderWidth * 3)
			nWidth	=	nBorderWidth * 3;
		m_nButtonX = (int) ( (float)(m_value - m_minValue) * (float)(nWidth - nBorderWidth*2) / (float)(m_maxValue - m_minValue) ) + nBorderWidth;
		m_rcButton.Set(m_windowRect.left + (m_nButtonX - nBorderWidth / 2), m_windowRect.top, m_windowRect.left + (m_nButtonX + nBorderWidth / 2), m_windowRect.bottom);
	}

	void UISlider::recalcRects()
	{CPUTime(UISlider);
		UIControl::recalcRects();
		OnAnimationRect();
	}

	int UISlider::ValueFromPos( int x )
	{CPUTime(UISlider); 
		int nWidth			=	RectWidth( m_windowRect );
		int nBorderWidth	=	RectHeight( m_windowRect );
		if(nWidth < nBorderWidth*3)
			nWidth	=	nBorderWidth * 3;
		float fValuePerPixel = (float)(m_maxValue - m_minValue) / (float)(nWidth - nBorderWidth * 2);
		return (int)(m_minValue + fValuePerPixel * (float)(x - (m_windowRect.left + nBorderWidth)));
	}

	char		UISlider::LoadControl(CSafeFileHelperR& r)
	{CPUTime(UISlider);
		UIControl::LoadControl( r );
		skip_r sr;
		if(m_loadVersion>=0x000000A4)
			sr.begin(r.pStream);
		r >> m_value >> m_maxValue >> m_minValue >> m_nDragX >> m_nDragOffset >> m_nButtonX >> m_bPressed >> m_rcButton;
		sr.end();
		return true;
	}
	char		UISlider::SaveControl(CSafeFileHelperW& w)
	{CPUTime(UISlider);
		UIControl::SaveControl( w );
		skip_w sw(w.pStream);
		w << m_value << m_maxValue << m_minValue << m_nDragX << m_nDragOffset << m_nButtonX << m_bPressed << m_rcButton;
		sw.end();
		return true;
	}
	char UISlider::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex )
	{CPUTime(UISlider);
		if( !m_bVisible )
			return false;
		if(!this->IsEnabledCtrl())
		{
			switch(uMsg)
			{
			case InputEventID_ButtonDown:
			case InputEventID_ButtonDblClk:
			case InputEventID_MouseMove:
				return isPtIn( pt );
			default:
				return false;
			}
		}
		switch( uMsg )
		{
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
			{
				if( m_rcButton.isPtIn( pt.x, pt.y ) )
				{
					m_bPressed = (touchIndex + 1);
					m_nDragX = pt.x;
					m_nDragOffset = m_nButtonX - m_nDragX;
					return true;
				}
				if( m_windowRect.isPtIn( pt.x, pt.y ) )
				{
					m_nDragX = pt.x;
					m_nDragOffset = 0;               
					m_bPressed = (touchIndex + 1);
					int offset	=	(this->m_maxValue - m_minValue) / 10;
					if(offset < 0)
						offset = 1;
					if( pt.x > m_nButtonX + m_windowRect.left )
					{
						SetValueI( m_value + offset, true );
						return true;
					}
					if( pt.x < m_nButtonX + m_windowRect.left )
					{
						SetValueI( m_value - offset, true );
						return true;
					}
				}
				break;
			}
		case InputEventID_ButtonUp:
			{
				if( m_bPressed == (touchIndex + 1))
				{
					m_bPressed = false;
					m_dialogPtr->RaiseEvent(this, EventID_Changed, pt);//CallEventCallback( EVENT_SLIDER_CHANGED, true, this );
					return true;
				}
				break;
			}
		case InputEventID_MouseMove:
			{
				char bMouseMove	=	isPtIn(pt);
				if(bMouseMove)
				{
					if(!m_bMouseEnter)
						this->onMouseEnter();
				}
				if( m_bPressed == (touchIndex + 1) )
				{
					SetValueI( ValueFromPos( m_windowRect.left + pt.x + m_nDragOffset ), true );
					m_dialogPtr->RaiseEvent(this, EventID_Changed, pt);
					return true;
				}
				return isPtIn( pt );
			}
		};
		return false;
	}

	void 	UISlider::SetText( const char* strText )
	{CPUTime(UISlider);
		float fProgress = (float)::atof(strText);
		if(fProgress < 0.0f)
			fProgress = 0.0f;
		else if(fProgress > 1.0f)
			fProgress = 1.0f;
		SetValueI((int)(fProgress * (float)(m_maxValue - m_minValue)) + m_minValue, false);
	}

	void UISlider::SetValueI( int nValue, char bFromInput )
	{CPUTime(UISlider);
		nValue = getmax( m_minValue, nValue );
		nValue = getmin( m_maxValue, nValue );
		if( nValue == m_value )
			return;
		m_value = nValue;
		recalcRects();
	}

	void UISlider::Render( float fElapsedTime )
	{CPUTime(UISlider);
		if(!this->m_bVisible)
			return;
		UIElement* pElement = &m_sdElement[0];
		g_manager->SetBlendMode((BlendMode)pElement->m_nSrcBlend,(BlendMode)pElement->m_nDestBlend);
		//
		Rect rc = m_currentAnim.rc;
		Pixel center(m_rotCenter.x + rc.left, m_rotCenter.y + rc.top);
		rc.left = (int)((float)(rc.left - center.x) * m_scale.x) + center.x;
		rc.right = (int)((float)(rc.right - center.x) * m_scale.x) + center.x;
		rc.top = (int)((float)(rc.top - center.y) * m_scale.y) + center.y;
		rc.bottom = (int)((float)(rc.bottom - center.y) * m_scale.y) + center.y;

		Rect	rcBg	=	rc;
		int height		=	RectHeight(rc);
		rcBg.left		+=	height;
		rcBg.right		-=	height;
		m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &rcBg, pElement->textureColor * m_currentAnim.textureColor, &m_currentAnim,this);
		pElement = &m_sdElement[1];//m_elements.at( 1 );
		OnAnimationRect();
		Rect rcButton	=	m_rcButton;
		if(!pElement->m_bRenderScale)
		{
			int width	=	pElement->rcTexture.right - pElement->rcTexture.left;
			int height	=	pElement->rcTexture.bottom - pElement->rcTexture.top;
			if(width > 0 && height > 0)
			{
				rcButton.left	=	((rcButton.right - rcButton.left) - width) / 2 + rcButton.left;
				rcButton.top	=	((rcButton.bottom - rcButton.top) - height) / 2 + rcButton.top;
				rcButton.right	=	rcButton.left + width;
				rcButton.bottom	=	rcButton.top + height;
			}
		}
		m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &rcButton, pElement->textureColor * m_currentAnim.textureColor, &m_currentAnim,this);
		this->RenderDrawMgrs();
	}
};
