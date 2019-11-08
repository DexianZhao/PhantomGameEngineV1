//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIButton.h"
#include "PhantomUIDialog.h"
#include "PhantomUIManager.h"
#include "PhantomManager.h"
#include "LuaManager.h"
#include "APILua.h"

namespace Phantom{


	UIButton::UIButton( BOOL bInit, UIDialog *dialogPtr )
		:UIStatic(bInit, dialogPtr)
	{
		m_bMouseEnter	=	false;
		m_bDownClick	=	false;
		m_bLButtonDownPush	=	false;
		m_fClickTime	=	0;
		m_clickCount	=	0;
		m_controlType = UIControlType_BUTTON;
		m_dialogPtr = dialogPtr;
		m_bPressed = false;
		this->m_bLButtonDown	=	false;
		this->m_bMouseMove	=	false;
		//
		ClearElement();
		for(int i=0;i<MaxButtonElement;i++)
			AddElement(&m_btnElement[i]);
		AddElement(&m_bgElement);
		if(bInit)
		{
			for(int i=0;i<MaxButtonElement;i++)
			{
				m_btnElement[i].textureColor = color4(COLOR_ARGB(255, 255, 255, 255));
				m_btnElement[i].fontColor = color4(COLOR_ARGB(255, 0, 0, 0));
				m_btnElement[i].SetRenderScale(true, false, Pixel(10, 13));
			}
			m_btnElement[0].SetSourceRect(Rect(Pixel(0, 0), 321, 112));
//			m_btnElement[0].m_name = ("按钮[正常]");
			m_btnElement[1].SetSourceRect(Rect(Pixel(0, 114), 321, 112));
//			m_btnElement[1].m_name = ("按钮[按下]");
			//m_btnElement[2].SetSourceRect(Rect(0, 58, 80, 28 + 58));
//			m_btnElement[2].m_name = ("按钮[释放]");
			//
			m_btnElement[2].SetSourceRect(Rect(Pixel(0, 0), 321, 112));
//			m_btnElement[3].m_name = ("按钮[无效]");
			m_btnElement[2].fontColor	=	color4(0.18f, 0.18f, 0.18f, 1.0f);
			//
			m_bgElement.SetTextureID(-1);
//			m_bgElement.m_name = ("按钮[背景]");
		}
	}
	char UIButton::canHaveFocus() 
	{CPUTime(UIButton);
		return (m_bVisible /*&& IsEnabledCtrl()*/ && m_bCanFocus);
	}
	char UIButton::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex )
	{CPUTime(UIButton);
		if( !m_bVisible )
			return false;
		if(!this->IsEnabledCtrl())
		{
			switch(uMsg)
			{
			case InputEventID_ButtonDown:
			case InputEventID_ButtonDblClk:
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
                int lbtn=m_bLButtonDown;
				if(lbtn==(touchIndex+1)){
					m_dialogPtr->RaiseEvent(this, Phantom::EventID_Move, pt);
				}
				return false;//m_bMouseMove;
			}
			break;
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
			{
				m_bMouseEnter = false;
				if( isPtIn( pt ) )
				{
					m_dialogPtr->RaiseEvent(this, Phantom::EventID_Down, pt);
					this->m_bLButtonDown	=	touchIndex+1;//(uMsg == InputEventID_ButtonDown ? 1 : 2);
					m_downPt	=	g_manager.GetCursorPos(touchIndex);
					m_bLButtonDownPush		=	true;
					m_clickCount	=	0;
					m_fClickTime	=	0.5f;
					m_fClickTimeWait	=	0.1f;
					m_bDownClick	=	false;
					for(int i=0;i<m_drawMgrs.size();i++)
						if(m_drawMgrs[i]->OnDownClick(pt, EventID_Click))
							break;
					for(int i=0;i<m_events.size();i++)
					{
						if(m_events[i].id == Phantom::EventID_Changed){
							m_bDownClick = true;
							break;
						}
					}
					m_bMouseEnter = true;
					return true;
				}
				break;
			}
		case InputEventID_ButtonUp:
			{
				m_bMouseEnter = false;
				for(int i=0;i<m_drawMgrs.size();i++)
					m_drawMgrs[i]->OnUpClick(pt, EventID_Click);
				m_bDownClick = false;
				m_bLButtonDownPush = false;
                int lbtn=m_bLButtonDown;
				if( lbtn == (touchIndex + 1))
				{
					m_dialogPtr->RaiseEvent(this, Phantom::EventID_Up, pt);
					m_bMouseMove	=	isPtIn(pt);
					if( isPtIn( pt ) )
					{
						Pixel temp = g_manager.GetCursorPos(touchIndex);
						Pixel off = m_downPt - temp;
						if(!this->m_dialogPtr->m_pParentControl || abs(off.x)<10&&abs(off.y)<10){
							OnButtonClick(pt, m_bLButtonDown > 0);
						}
                        m_bLButtonDown	=	0;
					}
					return true;
				}
				break;
			}
		}
		return false;
	}

	void UIButton::Render( float fElapsedTime )
	{CPUTime(UIButton);
		if(m_bDownClick){
			m_fClickTime -= fElapsedTime;
			if(m_fClickTime <= 0)
			{
				if((m_clickCount%2) == 0){
					m_bLButtonDownPush = false;
					for(int i=0;i<m_events.size();i++)
					{
						if(m_events[i].id == Phantom::EventID_Changed){
							if(m_events[i].func.t[0])
							{
								m_dialogPtr->RaiseEvent(this, Phantom::EventID_Changed, g_manager.GetCursorPos(this->m_bLButtonDown-1));
							}
							else
							{
								m_bLButtonDownPush = false;
								m_bDownClick = false;
							}
							break;
						}
					}
				}
				else
					m_bLButtonDownPush = true;
				m_fClickTime = m_fClickTimeWait;
				m_fClickTimeWait -= 0.008f;
				if(m_fClickTimeWait < 0.05f)m_fClickTimeWait = 0.05f;
				m_clickCount++;
			}
		}
		if(!this->m_bVisible)
			return;
		UIElement* pElement		=	&m_btnElement[0];
		color4 textColor		=	m_textColor;
		if(!this->IsEnabledCtrl())
		{
			textColor			=	m_btnElement[2].fontColor;//DISABLE_COLOR;
			pElement			=	&m_btnElement[2];
		}
		else
		{
			if(m_bLButtonDownPush)
				pElement			=	&m_btnElement[1];
			//else if(this->m_bMouseEnter)
			//	pElement			=	&m_btnElement[2];
			else
				pElement			=	&m_btnElement[0];
		}
		//
		UIElement* bgElement	=	&m_bgElement;//m_elements.at(4);
		//
		g_manager->SetBlendMode((BlendMode)bgElement->m_nSrcBlend,(BlendMode)bgElement->m_nDestBlend);
		//
		Rect	rc		=	m_currentAnim.rc;
		rc.left			+=	pElement->m_rectOffset.left;
		rc.right		+=	pElement->m_rectOffset.right;
		rc.top			+=	pElement->m_rectOffset.top;
		rc.bottom		+=	pElement->m_rectOffset.bottom;
		Pixel center(m_rotCenter.x + rc.left, m_rotCenter.y + rc.top);
		rc.left = (int)((float)(rc.left - center.x) * m_scale.x) + center.x;
		rc.right = (int)((float)(rc.right - center.x) * m_scale.x) + center.x;
		rc.top = (int)((float)(rc.top - center.y) * m_scale.y) + center.y;
		rc.bottom = (int)((float)(rc.bottom - center.y) * m_scale.y) + center.y;

		m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &rc, pElement->textureColor * m_currentAnim.textureColor, &m_currentAnim, this, 0);
		//
		if(bgElement->textureIndex > 0||bgElement->m_bNoTexture)
		{
			CPUTimeN(UIButton,1);
			g_manager->SetBlendMode((BlendMode)bgElement->m_nSrcBlend, (BlendMode)bgElement->m_nDestBlend);
			Rect rect;
			int newW = (bgElement->rcTexture.GetWidth() * rc.GetWidth() / m_btnElement[0].rcTexture.GetWidth());
			int newH = (bgElement->rcTexture.GetHeight() * rc.GetHeight() / m_btnElement[0].rcTexture.GetHeight());
			if(m_btnElement[0].GetRenderScale() && m_btnElement[0].m_bWrap)
			{
				newW = bgElement->rcTexture.GetWidth();
				newH = bgElement->rcTexture.GetHeight();
			}
			rect.left = rc.left + (rc.GetWidth() - newW) / 2;
			rect.top = rc.top + (rc.GetHeight() - newH) / 2;
			rect.right = rect.left + newW;
			rect.bottom = rect.top + newH;
			if(m_bLButtonDownPush)
			{
				rect.Offset(0, 1);
				m_dialogPtr->DrawElement(GetEnabled(), bgElement, &bgElement->rcTexture, &rect, pElement->textureColor * m_currentAnim.textureColor, &m_currentAnim, this);
			}
			else
				m_dialogPtr->DrawElement(GetEnabled(), bgElement, &bgElement->rcTexture, &rect, bgElement->textureColor * m_currentAnim.textureColor, &m_currentAnim, this);
		}
		//
		if(m_text.size() > 0)
		{
			CPUTimeN(UIButton,2);
			pElement->dwTextFormat		=	m_textFormat;
			pElement->fontIndex			=	m_btnElement[0].fontIndex;
			rc	=	m_windowRect;
			//rc.inflate(-m_borderWidth, -m_borderWidth);
			if(m_bLButtonDownPush)
			{
				rc.left++;
				rc.top++;
				rc.bottom++;
				rc.right++;
			}
			//2013-1-3
			if(m_bEnableUseImageText)
				RenderImgText(*pElement);
            else if(m_textPtr)
            {
				m_dialogPtr->DrawElement(m_textPtr, GetEnabled(), &m_bgElement, 0, &rc, m_textColor * m_currentAnim.textColor, &m_currentAnim, this, 0);
                //m_dialogPtr->DrawElementText(GetEnabled(), m_currentAnim.rc, m_textPtr, bgElement, &rc, m_textColor * m_currentAnim.textColor, m_nShadowWidth, m_shadowColor, true, &m_currentAnim, -1);
            }
		}
		{
			CPUTimeN(UIButton,3);
			this->RenderDrawMgrs();
		}
		{
			CPUTimeN(UIButton,4);
			RenderMesh();
		}
	}

};
