//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIStaticFormat.h"
#include "PhantomUIManager.h"
#include "PhantomUIDialog.h"

#include "PhantomManager.h"


namespace Phantom{

	UIStaticFormat::UIStaticFormat( BOOL bInit, UIDialog *dialogPtr ) : UIStatic(bInit, dialogPtr), m_scrollBar( bInit, dialogPtr )
	{
		m_controlType	= UIControlType_VIEW_FORMAT_TEXT;
		m_dialogPtr		= dialogPtr;
		m_bPressed		= false;
		this->m_bLButtonDown	=	false;
		this->m_bMouseMove		=	false;
		this->m_formatText.Setup(dialogPtr);
		//
		ClearElement();
		AddElement(&m_bgElement);
		if(bInit)
		{
			m_bgElement.SetSourceRect(Rect(83, 0, 83 + 30, 30));
			m_bgElement.SetRenderScale(false, false, Pixel(5, 5));
//			m_bgElement.m_name = ("背景");
			m_bgElement.fontColor	=	color4(1,1,1,1);
		}
		this->m_textColor		=	color4(1,1,1,1);
		m_borderWidth	=	3;
		m_formatText.SetTextFormatCB(this);
		m_scrollBarWidth	=	0;
	}
	int					UIStaticFormat::GetElementCount()
	{CPUTime(UIStaticFormat);
		return this->m_elementCount + m_scrollBar.m_elementCount;
	}
	UIElement*			UIStaticFormat::GetElementPtr(int index)
	{CPUTime(UIStaticFormat);
		if(index<0)
			return 0;
		if(index < m_elementCount)
			return this->m_elementList[index];
		index -= m_elementCount;
		if(index >= m_scrollBar.m_elementCount)
			return 0;
		return m_scrollBar.m_elementList[index];
	}

	char UIStaticFormat::LoadControl(CSafeFileHelperR& r)
	{CPUTime(UIStaticFormat);
		UIStatic::LoadControl( r );
		int version = 0;
		int spacing = 0;
		r >> version >> spacing;
		m_formatText.SetLineSpacingHeight( spacing );
		int num = 0;
		r >> num;
		if(num > 0)
		{
			if(this->m_loadVersion<0x000000A3){
				short_t* buffer = new short_t[num + 1];
				r.pStream->read(buffer, num * sizeof(short_t));
				buffer[num]		=	0;
				//this->m_text	=	buffer;
				delete[] buffer;
			}
			else
			{
				char* str = new char[num+1];
				r.pStream->read(str, num);
				str[num] = 0;
				SetText(str);
				delete[] str;
			}
		}
		//
		this->m_scrollBar.LoadControl(r);
		r >> m_scrollBarWidth;
		OnAnimationRect();
		return true;
	}
	char UIStaticFormat::SaveControl(CSafeFileHelperW& w)
	{CPUTime(UIStaticFormat);
		UIStatic::SaveControl( w );
		int version = 8;
		w << version << this->m_formatText.GetLineSpacingHeight();
		const char* str	=	GetText();
		int num = strlen(str);
		w << num;
		if(num > 0)
			w.pStream->write(str, num * sizeof(char));
		this->m_scrollBar.SaveControl(w);
		w << m_scrollBarWidth;
		return true;
	}
	VOID UIStaticFormat::OnEventClick(TextFormat* ptr, TextW& eventText, int parameter)
	{CPUTime(UIStaticFormat);
		//m_dialogPtr->RaiseEvent(this, EventID_UIFormatTextEventID, parameter);
	}

	VOID UIStaticFormat::OnEventMoving(TextFormat* ptr, TextW& eventText, int parameter)
	{CPUTime(UIStaticFormat);
	}

	char UIStaticFormat::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex )
	{CPUTime(UIStaticFormat);
		if( !m_bVisible )
			return false;
		if(m_scrollBar.onMouseMessage(uMsg, pt, touchIndex))
		{
			m_formatText.BeginShowLine(m_scrollBar.GetScrollPos());
			return true;
		}
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
				if( m_bMouseMove )
					m_formatText.OnMouseMove(pt.x - this->m_currentAnim.rc.left, pt.y - this->m_currentAnim.rc.top);
				if(m_bMouseMove)
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
					this->m_bLButtonDown	=	true;
					return true;
				}
				break;
			}
		case InputEventID_ButtonUp:
			{
				if( m_bLButtonDown )
				{
					m_bLButtonDown = false;
					if( isPtIn( pt ) )
					{
						if(m_formatText.OnButtonUp(pt.x - this->m_currentAnim.rc.left, pt.y - this->m_currentAnim.rc.top))
						{
						}
						else
						{
							OnButtonClick(pt, EventID_Click);
						}
					}
					return true;
				}
				break;
			}
		};
		return false;
	}

	extern	UIManager*	GetDlgManager();

	void UIStaticFormat::SetOrigText(const char* szText)
	{CPUTime(UIStaticFormat);
		m_formatText.RemoveAll();
		CDynamicArray<short_t> ret;
		ret.SetArrayCount(strlen(szText) + 1);
		int len = Utf8ToW(szText, ret.GetPtr(), ret.size());
		ret.SetArrayCount(len);
		m_formatText.Insert(0, ret.GetPtr(), ret.size(), false, false);
		m_formatText.SetFormat(0, -1, this->m_textColor, this->GetElementPtr(0)->GetFontID(), 0,0, 0, false, false);
		m_formatText.Invalidate();
	}

	void UIStaticFormat::SetTextA( const char* strText )
	{CPUTime(UIStaticFormat);
		SetText(strText);
	}

	void UIStaticFormat::SetText( const char* strText )
	{CPUTime(UIStaticFormat);
		UIStatic::SetText(strText);
		CSpeakParameter ret;
		const char* text		= GetDlgManager()->m_analyseText.AnalyseText(m_text.str(), &ret);
		GetDlgManager()->m_analyseText.SetAnalyseFromSaved(this, text, &ret);
		m_formatText.Invalidate();
		m_scrollBar.ShowItem( m_formatText.GetBeginShowLine() );
		m_scrollBar.SetScrollRange(0, m_formatText.GetLineCount());
	}

	void UIStaticFormat::Render( float fElapsedTime )
	{CPUTime(UIStaticFormat);
		if(!this->m_bVisible)
			return;
		g_manager->SetBlendMode((BlendMode)m_bgElement.m_nSrcBlend,(BlendMode)m_bgElement.m_nDestBlend);
		//
		if(m_bgElement.GetTextureNewID() >= 0)
			m_dialogPtr->DrawElement(IsEnabled(), &m_bgElement, &m_bgElement.rcTexture, &m_currentAnim.rc, m_bgElement.textureColor * m_currentAnim.textureColor, &m_currentAnim);
		//
		m_bgElement.dwTextFormat			=	m_textFormat;
		m_formatText.m_nShadowWidth	=	this->m_nShadowWidth;
		m_formatText.m_nShadowColor	=	this->m_shadowColor;
		m_formatText.m_fTextAlpha	=	this->m_textColor.a * this->m_currentAnim.textColor.a;
		TextRenderParam param;
		param.c	=	this;
		param.e	=	&m_bgElement;
		this->m_formatText.Render(&param);
		//
		if(this->m_scrollBarWidth > 0)
			m_scrollBar.RenderScroll( this, fElapsedTime );
		this->RenderDrawMgrs();
	}

	VOID UIStaticFormat::OnAnimationRect()
	{CPUTime(UIStaticFormat);
		Rect rc	=	m_windowRect;
		rc.inflate(-m_borderWidth, -m_borderWidth);
		rc.right	-=	m_scrollBarWidth;
		m_formatText.SetDrawRect(rc);
		m_scrollBar.m_windowRect	=	Rect(m_windowRect.right - m_scrollBarWidth, m_windowRect.top, m_windowRect.right, m_windowRect.top + GetHeight());
		m_scrollBar.OnAnimationRect();
	}

	void UIStaticFormat::recalcRects()
	{CPUTime(UIStaticFormat);
		UIStatic::recalcRects();
		OnAnimationRect();
		m_scrollBar.SetPageSize( 1 );
		m_scrollBar.SetScrollPos( m_formatText.GetBeginShowLine() );
		m_scrollBar.SetScrollRange(0, m_formatText.GetLineCount());
	}

	VOID UIStaticFormat::OnSetAnimationRect()
	{CPUTime(UIStaticFormat);
		OnAnimationRect();
	}

	VOID UIStaticFormat::FrameMove(float fElapsedTime)
	{CPUTime(UIStaticFormat);
		char Inv	=	m_bIsInvalidate;
		UIStatic::FrameMove(fElapsedTime, 0, 0);
		if(Inv)
		{
		}
		OnSetAnimationRect();
	}

	void 				UIStaticFormat::SetAnalyseText( const char* strText )	//设置格式化文本,必须支持IFormatEditBox的控件才会有效
	{CPUTime(UIStaticFormat);
		GetDlgManager()->GetAnalyseFormatText()->SetAnalyseText(this, strText);
	}

};
