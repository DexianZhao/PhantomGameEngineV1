//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIStatic.h"
#include "PhantomManager.h"
#include "PhantomUIDialog.h"
#include "LuaManager.h"
#include "APILua.h"

namespace Phantom{

	UIStatic::UIStatic( BOOL bInit, UIDialog *dialogPtr ) : UIControl(bInit, dialogPtr)
	{
		m_controlType	= UIControlType_STATIC;
		m_dialogPtr		= dialogPtr;
		m_bLButtonDown	=	false;
		//
		if(bInit)
		{
			m_bgElement.SetSourceRect(Rect(83, 0, 83 + 30, 30));
			m_bgElement.SetRenderScale(true, false, Pixel(5, 5));
//			m_bgElement.m_name = ("背景");
		}
		//
		ClearElement();
		AddElement(&m_bgElement);
		//
		m_borderWidth	=	0;
		m_contain		=	0;
	}
	UIStatic::~UIStatic()
	{
	}
	char UIStatic::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex )
	{CPUTime(UIStatic);
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
				if(m_bLButtonDown==(touchIndex+1)){
					m_dialogPtr->RaiseEvent(this, Phantom::EventID_Move, pt);
				}
			}
			break;
		case InputEventID_ButtonDown:
			{
				if(isPtIn(pt) && canHaveFocus())
				{
					m_dialogPtr->RaiseEvent(this, Phantom::EventID_Down, pt);
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
				if(m_bLButtonDown == (touchIndex + 1))// && isPtIn(pt))
				{
					m_dialogPtr->RaiseEvent(this, Phantom::EventID_Up, pt);
					OnButtonClick(pt, EventID_Click);
					m_bLButtonDown	=	false;
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
	void UIStatic::Render( float fElapsedTime )
	{CPUTime(UIStatic);    
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
		Rect rc = m_currentAnim.rc;
		Pixel center(m_rotCenter.x + rc.left, m_rotCenter.y + rc.top);
		rc.left = (int)((float)(rc.left - center.x) * m_scale.x) + center.x;
		rc.right = (int)((float)(rc.right - center.x) * m_scale.x) + center.x;
		rc.top = (int)((float)(rc.top - center.y) * m_scale.y) + center.y;
		rc.bottom = (int)((float)(rc.bottom - center.y) * m_scale.y) + center.y;
		if(!this->m_bEnableUseImageText)
			m_dialogPtr->DrawElement(GetEnabled(), &m_bgElement, &m_bgElement.rcTexture, &rc, m_bgElement.textureColor * m_currentAnim.textureColor, &m_currentAnim, this, 0);
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
		this->RenderDrawMgrs();
		RenderMesh();
	}

	char		UIStatic::LoadControl(CSafeFileHelperR& r)
	{CPUTime(UIStatic);
		UIControl::LoadControl( r );
		skip_r sr;
		if(m_loadVersion>=0x000000A4)
			sr.begin(r.pStream);
		int bufferLength;
		r >> bufferLength;
		if(bufferLength > 0)
		{
			if(this->m_loadVersion<0x000000A2){
				short_t* buf	=	new short_t[bufferLength + 1];
				r.pStream->read(buf, sizeof(short_t) * bufferLength);
				buf[bufferLength]	=	0;
#ifdef WIN32_OS
				char cbuf[256];
				int len = ::WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)buf, bufferLength, cbuf, 256, 0, 0);
				cbuf[len] = 0;
				m_text = cbuf;
#endif

				delete[] buf;
				if(m_bEnableUseImageText)
				{
					int len = m_text.size();
					for(int i=0;i<len;i++)
					{
						if(m_text.texts[i] == '\\')
							m_text.texts[i] = '/';
					}
					len = strlen(this->m_szNumberBegin);
					for(int i=0;i<len;i++)
					{
						if(m_szNumberBegin[i] == '\\')
							m_szNumberBegin[i] = '/';
					}
				}
			}
			else
			{
				char* buf	=	new char[bufferLength + 1];
				r.pStream->read(buf, sizeof(char) * bufferLength);
				buf[bufferLength]	=	0;
				m_text		=	buf;
				delete[] buf;
			}
			OnChangeText();
			
		}
		else
			m_text	=	"";
		if(this->m_loadVersion >= 0x00000066)
			r >> m_borderWidth;
		sr.end();
		return true;
	}
	char		UIStatic::SaveControl(CSafeFileHelperW& w)
	{CPUTime(UIStatic);
		UIControl::SaveControl( w );
		skip_w sw(w.pStream);
		const char* str	=	m_text.str();
		int bufferLength	=	strlen(str);
		w << bufferLength;
		if(bufferLength > 0)
		{
			w.pStream->write( str, sizeof(char) * bufferLength );
		}
		w << m_borderWidth;
		sw.end();
		return true;
	}

};
