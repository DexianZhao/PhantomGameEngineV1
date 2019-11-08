//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUICheckBox.h"
#include "PhantomUIDialog.h"
#include "PhantomUIManager.h"
#include "PhantomManager.h"

namespace Phantom{

	UICheckBox::UICheckBox( BOOL bInit, UIDialog *dialogPtr )
		:UIButton(bInit, dialogPtr)
	{
		m_controlType	= UIControlType_CHECKBOX;
		m_dialogPtr		= dialogPtr;
		m_bChecked		= false;
		//
		ClearElement();
		for(int i=0;i<MaxButtonElement;i++)
			AddElement(&m_btnElement[i]);
		AddElement(&m_bgElement);
		if(bInit)
		{
			m_btnElement[0].SetSourceRect(Rect(88, 112, 88 + 13, 112 + 13));
			m_btnElement[0].SetRenderScale(true, false, Pixel(2, 2));
			//
			m_btnElement[1].SetSourceRect(Rect(88, 112, 88 + 13, 112 + 13));
			m_btnElement[1].SetRenderScale(true, false, Pixel(2, 2));
			//
			m_btnElement[2].SetSourceRect(Rect(88, 98, 88 + 13, 98 + 13));
			m_btnElement[2].SetRenderScale(true, false, Pixel(2, 2));
			//
			m_btnElement[3].SetSourceRect(Rect(88, 112, 88 + 13, 112 + 13));
			m_btnElement[3].SetRenderScale(true, false, Pixel(2, 2));
			m_btnElement[3].textureColor = color4(0xff808080);
			m_btnElement[3].fontColor = color4(0xff808080);
			//
			m_bgElement.SetSourceRect(Rect(85, 2, 85 + 26, 2 + 26));
			m_bgElement.SetRenderScale(true, false, Pixel(5, 5));
			//
			//m_btnElement[0].m_name = ("选择框[框]");
			//m_btnElement[1].m_name = ("选择框[按下]");
			//m_btnElement[2].m_name = ("选择框[选中]");
			//m_btnElement[3].m_name = ("选择框[无效]");
			//m_bgElement.m_name = ("选择框[背景]");
			m_btnElement[3].fontColor	=	color4(0.18f, 0.18f, 0.18f, 1.0f);
		}
		this->SetTextFormat(DrawTextAlign_LEFT | DrawTextAlign_VCENTER | DrawTextAlign_SINGLELINE);
	}
	void UICheckBox::SetCheckedInternal( char bChecked, char bFromInput ) 
	{CPUTime(UICheckBox); 
		m_bChecked		=	bChecked;
	}
	BOOL UICheckBox::isPtIn( Pixel pt ) 
	{CPUTime(UICheckBox); 
		return ( m_windowRect.isPtIn( pt.x,pt.y ) || m_rcButton.isPtIn( pt.x,pt.y ) ); 
	}
	VOID UICheckBox::OnAnimationRect()
	{CPUTime(UICheckBox);
		m_rcButton	= m_windowRect;
		m_rcText	= m_windowRect;
		m_rcText.left += (int) ( 1.25f * m_rcButton.GetHeight() );
	}
	void UICheckBox::recalcRects()
	{CPUTime(UICheckBox);
		UIButton::recalcRects();
		OnAnimationRect();
	}

	char UICheckBox::LoadControl(CSafeFileHelperR& r)
	{CPUTime(UICheckBox);
		UIButton::LoadControl( r );
		skip_r sr;
		if(m_loadVersion>=0x000000A4)
			sr.begin(r.pStream);
		r >> this->m_rcText >> m_rcButton;
		sr.end();
		return true;
	}
	char UICheckBox::SaveControl(CSafeFileHelperW& w)
	{CPUTime(UICheckBox);
		UIButton::SaveControl( w );
		skip_w sw(w.pStream);
		w << this->m_rcText << m_rcButton;
		sw.end();
		return true;
	}
	char UICheckBox::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex)
	{CPUTime(UICheckBox);
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
		case InputEventID_MouseMove:
			{
				this->m_bMouseMove	=	isPtIn(pt);
				if(this->m_bMouseMove)
				{
					if(!m_bMouseEnter)
						this->onMouseEnter();
				}
				return isPtIn( pt );
			}
			break;
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
			{
				if( isPtIn( pt ) )
				{
					this->m_bLButtonDown	=	touchIndex + 1;
					for(int i=0;i<m_drawMgrs.size();i++)
						if(m_drawMgrs[i]->OnDownClick(pt, EventID_Click))
							break;
					return true;
				}
				break;
			}
		case InputEventID_ButtonUp:
			{
				for(int i=0;i<m_drawMgrs.size();i++)
					m_drawMgrs[i]->OnUpClick(pt, EventID_Click);
				if( m_bLButtonDown == (touchIndex + 1) )
				{
					m_bLButtonDown = 0;
					
					if( isPtIn( pt ) )
					{
						m_bChecked	=	!m_bChecked;
						OnButtonClick(pt, EventID_Click);
					}
					return true;
				}
				break;
			}
		};
		return false;
	}

	void UICheckBox::Render( float fElapsedTime )
	{CPUTime(UICheckBox);
		if(!this->m_bVisible)
			return;
		for(int i=0;i<3;i++)
			m_btnElement[i].dwTextFormat		=	m_textFormat;
		UIElement* pElement;
		if(m_bLButtonDown)
			pElement	=	&m_btnElement[1];
		else if(m_bChecked)
			pElement	=	&m_btnElement[2];
		else
			pElement	=	&m_btnElement[0];
		if(!this->IsEnabledCtrl())
			pElement	=	&m_btnElement[3];
		//
		UIElement* backImage	=	&m_bgElement;//m_elements.at(4);
		g_manager->SetBlendMode((BlendMode)backImage->m_nSrcBlend,(BlendMode)backImage->m_nDestBlend);
		m_dialogPtr->DrawElement(GetEnabled(), backImage, &backImage->rcTexture, &m_currentAnim.rc, (IsEnabledCtrl() ? backImage->textureColor : pElement->textureColor) * this->m_currentAnim.textureColor, &m_currentAnim, this);
		//
		Rect	rcCheck			=	m_currentAnim.rc;
		rcCheck.right	=	rcCheck.left + (rcCheck.bottom - rcCheck.top);
		if(!pElement->m_bRenderScale)
		{
			int height		=	pElement->GetSourceRect().GetHeight();
			rcCheck.top		+=	(rcCheck.bottom - rcCheck.top - height) / 2;
			rcCheck.bottom	=	rcCheck.top + height;
			rcCheck.right	=	rcCheck.left + pElement->GetSourceRect().GetWidth();
		}
		g_manager->SetBlendMode((BlendMode)pElement->m_nSrcBlend,(BlendMode)pElement->m_nDestBlend);
		m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &rcCheck, pElement->textureColor * m_currentAnim.textureColor, &m_currentAnim, this);
		//
		//if(m_text.size() > 0)
		//{
		//	pElement->fontIndex			=	m_btnElement[0].fontIndex;
		//	pElement->dwTextFormat	=	this->m_textFormat;
		//	m_rcText.left	=	rcCheck.right;
		//	m_rcText.right	=	m_windowRect.right;
		//	m_dialogPtr->DrawElementText(GetEnabled(), m_currentAnim.rc, m_text.strW(), pElement, &m_rcText, pElement->fontColor * m_currentAnim.textColor, m_nShadowWidth, m_shadowColor, true, &m_currentAnim);
		//}
		this->RenderDrawMgrs();
	}

};
