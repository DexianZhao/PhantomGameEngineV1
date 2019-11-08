//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUITextBox.h"
#include "PhantomManager.h"
#include "PhantomUIDialog.h"

extern "C"{
	void	openEdit(const char* szDefault, int x, int y, int w, int h, int id);
	int		closeEdit(char* ret, int buflen);
};

namespace Phantom{

	UITextBox::UITextBox( BOOL bInit, UIDialog *dialogPtr ) : UIControl(bInit, dialogPtr)
	{
		m_controlType	= UIControlType_EDITBOX;
		m_dialogPtr		= dialogPtr;
		m_bLButtonDown	=	false;
		//
		m_bPassword		=	false;
		if(bInit)
		{
			m_bgElement.SetSourceRect(Rect(83, 0, 83 + 30, 30));
			m_bgElement.SetRenderScale(false, false, Pixel(5, 5));
//			m_bgElement.m_name = ("背景");
		}
		//
		ClearElement();
		AddElement(&m_bgElement);
		//
		m_borderWidth	=	0;
	}

	char UITextBox::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex )
	{CPUTime(UITextBox);
		if( !m_bVisible || this->m_bIsBackground )
			return false;
		if(!this->IsEnabledCtrl() && this->canHaveFocus())
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

		switch(uMsg)
		{
		case InputEventID_MouseMove:
			{
				char bMouseMove	=	isPtIn(pt);
				if(bMouseMove)
				{
					if(!m_bMouseEnter)
						this->onMouseEnter();
				}
			}
			break;
		case InputEventID_ButtonDown:
			{
				if(isPtIn(pt) && canHaveFocus())
				{
					m_bLButtonDown	=	(touchIndex + 1);
					for(int i=0;i<m_drawMgrs.size();i++)
						if(m_drawMgrs[i]->OnDownClick(pt, EventID_Click))
							break;
					return true;
				}
				//if(m_bLButtonDown && isPtIn(pt))
					//OnButtonClick(EventID_Click);
				//m_bLButtonDown	=	false;
			}
			break;
		case InputEventID_ButtonUp:
			{
				for(int i=0;i<m_drawMgrs.size();i++)
					m_drawMgrs[i]->OnUpClick(pt, EventID_Click);
				if(m_bLButtonDown == (touchIndex + 1) && isPtIn(pt) && !this->m_bReadOnly)
				{
					OnButtonClick(pt, EventID_Click);
					m_bLButtonDown	=	false;
					int x=0,y=0;
					this->m_dialogPtr->GetLocation(x,y);
					openEdit(this->m_text.str(), m_windowRect.left+x,m_windowRect.top+y, m_windowRect.GetWidth(), m_windowRect.GetHeight(), this->GetID());
					return true;
				}
			}
			break;
		case InputEventID_ButtonDblClk:
			{
				if(isPtIn(pt) && canHaveFocus())
					m_bLButtonDown	=	true;
				if(m_bLButtonDown && isPtIn(pt))
				{
					OnButtonClick(pt, EventID_DoubleClick);
					return true;
				}
				m_bLButtonDown	=	false;
			}
			break;
		}
		return UIControl::onMouseMessage( uMsg, pt, touchIndex );
	}
	VOID UITextBox::OnEditClose(BOOL bIsCancel, const char* text)
	{
		if(!bIsCancel)
			this->SetText(text);
	}
	void UITextBox::Render( float fElapsedTime )
	{CPUTime(UITextBox);    
		if( m_bVisible == false )
			return;
		//if(this->m_bMouseEnter && m_bMouseEnterEffect)
		//{
		//	g_manager->SetBlendMode((BlendMode)m_mouseEnterSrc,(BlendMode)m_mouseEnterDesc);
		//}
		//else
		//{
			g_manager->SetBlendMode((BlendMode)m_bgElement.m_nSrcBlend,(BlendMode)m_bgElement.m_nDestBlend);
		//}
		if(m_bgElement.GetTextureNewID() >= 0)
		{
			//if(m_bMouseEnter && m_bMouseEnterEffect)
				//m_dialogPtr->DrawElement(GetEnabled(), GetDisableColor(), &m_bgElement, &m_bgElement.rcTexture, &m_currentAnim.rc, this->m_mouseEnterColor * m_currentAnim.textureColor, true, &m_currentAnim);
			//else
			if(!this->m_bEnableUseImageText)
				m_dialogPtr->DrawElement(GetEnabled(), &m_bgElement, &m_bgElement.rcTexture, &m_currentAnim.rc, m_bgElement.textureColor * m_currentAnim.textureColor, &m_currentAnim, this);
		}
		Rect rc = m_currentAnim.rc;
		Pixel center(m_rotCenter.x + rc.left, m_rotCenter.y + rc.top);
		rc.left = (int)((float)(rc.left - center.x) * m_scale.x) + center.x;
		rc.right = (int)((float)(rc.right - center.x) * m_scale.x) + center.x;
		rc.top = (int)((float)(rc.top - center.y) * m_scale.y) + center.y;
		rc.bottom = (int)((float)(rc.bottom - center.y) * m_scale.y) + center.y;
		if(m_text.size() > 0)
		{
			m_bgElement.dwTextFormat			=	m_textFormat;
			//Rect	rc	=	m_currentAnim.rc;
			//rc.inflate(-m_borderWidth, -m_borderWidth);
			if(m_bEnableUseImageText)
				RenderImgText(m_bgElement);
		}
		if(m_textPtr&&!m_bEnableUseImageText)
		{
			m_dialogPtr->DrawElement(m_textPtr, GetEnabled(), &m_bgElement, 0, &rc, m_textColor * m_currentAnim.textColor, &m_currentAnim, this, 0);
		}
		if(m_text.size() > 0)
		{
			//m_bgElement.dwTextFormat			=	m_textFormat;
			//Rect	rc	=	m_currentAnim.rc;
			//rc.inflate(-m_borderWidth, -m_borderWidth);
			//if(m_bEnableUseImageText)
			//	RenderImgText(m_bgElement);
			//else
			//	m_dialogPtr->DrawElementText(GetEnabled(), m_currentAnim.rc, m_text.str(), &m_bgElement, &rc, m_textColor * m_currentAnim.textColor, m_nShadowWidth, m_shadowColor, true, &m_currentAnim);
		}
		this->RenderDrawMgrs();
	}

	char		UITextBox::LoadControl(CSafeFileHelperR& r)
	{CPUTime(UITextBox);
		UIControl::LoadControl( r );
		skip_r sr;
		if(m_loadVersion>=0x000000A4)
			sr.begin(r.pStream);
		r >> m_borderWidth >> m_nSpacing >> m_rcText >> m_dfBlink >> m_dfLastBlink >>
			m_bCaretOn >> m_nCaret >> m_bInsertMode >> m_nSelStart >> m_nFirstVisible >> m_TextColor
			>> m_SelTextColor >> m_SelBkColor >> m_CaretColor >> m_bRenderBorder >> m_bPassword;
		r.pStream->read(m_rcRender, sizeof(m_rcRender));
		if(this->m_loadVersion >= 0x00000066)
		{
			if(this->m_loadVersion<0x000000A2){
				CDynamicArray<short_t> text;
				r >> text;
			}
			else
			{
				std::string str;
				r >> str;
				SetText(str.c_str());
			}
			//SetText(text.c_str());
		}
		if(m_loadVersion >= 0x00000068)
		{
			int sb = 0;
			r >> sb;
			m_scrollBarWidth = sb;
		}
		sr.end();
		return true;
	}

	char		UITextBox::SaveControl(CSafeFileHelperW& w)
	{CPUTime(UITextBox);
		UIControl::SaveControl( w );
		skip_w sw(w.pStream);
		w << m_borderWidth << m_nSpacing << m_rcText << m_dfBlink << m_dfLastBlink <<
			m_bCaretOn << m_nCaret << m_bInsertMode << m_nSelStart << m_nFirstVisible << m_TextColor
			<< m_SelTextColor << m_SelBkColor << m_CaretColor << m_bRenderBorder << m_bPassword;
		w.pStream->write(m_rcRender, sizeof(m_rcRender));
		w << m_text.str();
		int sb = m_scrollBarWidth;
		w << sb;
		sw.end();
		return true;
	}
};
