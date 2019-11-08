//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIComboBox.h"
#include "PhantomUIDialog.h"
#include "PhantomUIManager.h"
#include "PhantomManager.h"

namespace Phantom{


	UIComboBox::UIComboBox( BOOL bInit, UIDialog *dialogPtr ) : m_scrollBar( bInit, dialogPtr ), UIButton(bInit, dialogPtr)
	{
		m_controlType		= UIControlType_COMBOBOX;
		m_dialogPtr			= dialogPtr;
		m_nDropHeight		= 100;
		m_scrollBarWidth	= 16;
		m_bOpened			= false;
		m_selected			= -1;
		m_focuses			= -1;
		//
		ClearElement();
		for(int i=0;i<this->MaxButtonElement;i++)
			AddElement(&m_btnElement[i]);
		if(bInit)
		{
			Rect rcTexture;
			for(int i=0;i<this->MaxButtonElement;i++)
			{
				rcTexture.Set( 0, 61, 180, 24 );
				m_btnElement[i].SetTexture( 0, &rcTexture );
				m_btnElement[i].SetFont( 0 );
				m_btnElement[i].textureColor = color4( COLOR_ARGB(255, 255, 255, 255));
				m_btnElement[i].fontColor = color4( COLOR_ARGB(255, 0, 0, 0));
				m_btnElement[i].SetSourceRect(Rect(85, 2, 85 + 26, 2 + 26));
				m_btnElement[i].SetRenderScale(false, false, Pixel(5, 5));
			}
//			m_btnElement[0].m_name = ("组合框[底]");
			//-------------------------------------
			rcTexture.Set( 3, 3, 16, 16 );
			m_btnElement[1].SetTexture( 0, &rcTexture );
			m_btnElement[1].textureColor = color4( COLOR_ARGB(255, 255, 255, 255));
			m_btnElement[1].SetSourceRect(Rect(117, 3, 117 + 22, 3 + 21));
			m_btnElement[1].SetRenderScale(false, false, Pixel(3, 3));
//			m_btnElement[1].m_name = ("组合框[按钮]");
			//-------------------------------------
			rcTexture.Set( 0, 88, 180, 38 );
			m_btnElement[2].SetTexture( 0, &rcTexture );
			m_btnElement[2].SetFont( 0, COLOR_ARGB(255, 0, 0, 0), DrawTextAlign_LEFT | DrawTextAlign_TOP );
			m_btnElement[2].SetSourceRect(Rect(85, 2, 85 + 26, 2 + 26));
			m_btnElement[2].SetRenderScale(false, false, Pixel(5, 5));
//			m_btnElement[2].m_name = ("组合框[下拉框]");
			//-------------------------------------
			rcTexture.Set( 0, 131, 180, 22 );
			m_btnElement[3].SetTexture( 0, &rcTexture );
			m_btnElement[3].SetFont( 0, COLOR_ARGB(255, 255, 255, 255), DrawTextAlign_LEFT | DrawTextAlign_TOP );
			m_btnElement[3].SetSourceRect(Rect(117, 33, 117 + 56, 33 + 7));
			m_btnElement[3].SetRenderScale(false, false, Pixel(5, 5));
//			m_btnElement[3].m_name = ("组合框[选择条]");
		}
	}

	UIComboBox::~UIComboBox()
	{
		RemoveAllItems();
	}

	int					UIComboBox::GetElementCount()
	{CPUTime(UIComboBox);
		return this->m_elementCount + m_scrollBar.m_elementCount;
	}
	UIElement*			UIComboBox::GetElementPtr(int index)
	{CPUTime(UIComboBox);
		if(index<0)
			return 0;
		if(index < m_elementCount)
			return this->m_elementList[index];
		index -= m_elementCount;
		if(index >= m_scrollBar.m_elementCount)
			return 0;
		return m_scrollBar.m_elementList[index];
	}

	void UIComboBox::SetTextColor( unsigned int c )
	{CPUTime(UIComboBox);
		UIElement* pElement = &m_btnElement[0];
		if( pElement )
			pElement->fontColor = color4(c);
		pElement = &m_btnElement[ 2 ];
		if( pElement )
			pElement->fontColor = color4(c);
	}
	VOID UIComboBox::OnAnimationRect()
	{CPUTime(UIComboBox);
		m_rcButton = m_windowRect;
		m_rcButton.left = m_rcButton.right - m_rcButton.GetHeight();
		m_rcText = m_windowRect;
		m_rcText.right = m_rcButton.left;
		m_rcDropdown = m_rcText;
		m_rcDropdown.Offset( 0, (int) (0.90f * m_rcText.GetHeight()) );
		m_rcDropdown.bottom += m_nDropHeight;
		m_rcDropdown.right -= m_scrollBarWidth;
		m_rcDropdownText = m_rcDropdown;
		m_rcDropdownText.left += (int) (0.1f * m_rcDropdown.GetWidth());
		m_rcDropdownText.right -= (int) (0.1f * m_rcDropdown.GetWidth());
		m_rcDropdownText.top += (int) (0.1f * m_rcDropdown.GetHeight());//RectHeight( m_rcDropdown ));
		m_rcDropdownText.bottom -= (int) (0.1f * m_rcDropdown.GetHeight());
		m_scrollBar.m_windowRect	=	Rect(m_rcDropdown.right, m_rcDropdown.top+2, m_rcDropdown.right + m_scrollBarWidth, m_rcDropdown.top+2 + m_rcDropdown.GetHeight() - 2);
		m_scrollBar.OnAnimationRect();
	}
	void UIComboBox::recalcRects()
	{CPUTime(UIComboBox);
		UIButton::recalcRects();
		OnAnimationRect();
		//
		FontNode2D* pFontNode = GetDlgManager()->GetFontNode( m_btnElement[2].fontIndex );
		if( pFontNode && pFontNode->GetHeight() )
		{
			m_scrollBar.SetPageSize( RectHeight( m_rcDropdownText ) / pFontNode->GetHeight() );
			m_scrollBar.ShowItem( m_selected );
		}
	}
	void UIComboBox::onFocusOut()
	{CPUTime(UIComboBox);
		UIButton::onFocusOut();
		m_bOpened = false;
	}

	void	UIComboBox::SetEnabled(char bEnabled)
	{CPUTime(UIComboBox);
		UIButton::SetEnabled( bEnabled );
		this->m_scrollBar.SetEnabled( bEnabled );
	}
	
	char UIComboBox::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex )
	{CPUTime(UIComboBox);
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
		if( m_scrollBar.onMouseMessage( uMsg, pt, touchIndex) )
			return true;
		switch( uMsg )
		{
		case InputEventID_MouseMove:
			{
				char bMouseMove	=	isPtIn(pt);
				if(bMouseMove)
				{
					if(!m_bMouseEnter)
						this->onMouseEnter();
				}
				//else if(m_bMouseEnter)
				//this->onMouseLeave();
				//
				if( m_bOpened && m_rcDropdown.isPtIn( pt ) )
				{
					for( int i=0; i < (int)m_listItems.size(); i++ )
					{
						ListBoxItem* pItem = m_listItems.at( i );
						if( pItem -> bVisible &&
							pItem->rcActive.isPtIn( pt ) )
						{
							m_focuses = i;
						}
					}
				}
				return isPtIn( pt );
			}
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
			{
				if( isPtIn( pt ) )
				{
					m_bPressed = (touchIndex + 1);
					if( m_bHasFocus )
					{
						m_bOpened = !m_bOpened;
						if( !m_bOpened )
						{
						}
					}
					return true;
				}
				if( m_bOpened && m_rcDropdown.isPtIn( pt ) )
				{
					for( int i=m_scrollBar.GetScrollPos(); i < (int)m_listItems.size(); i++ )
					{
						ListBoxItem* pItem = m_listItems.at( i );
						if( pItem -> bVisible &&
							pItem->rcActive.isPtIn( pt ) )
						{
							m_focuses = m_selected = i;
							m_dialogPtr->RaiseEvent(this, EventID_Click, pt);
							m_bOpened = false;
							break;
						}
					}
					return true;
				}
				if( m_bOpened )
				{
					m_focuses = m_selected;
					m_dialogPtr->RaiseEvent(this, EventID_Click, pt);
					m_bOpened = false;
				}
				m_bPressed = false;
				break;
			}
		case InputEventID_ButtonUp:
			{
				if( m_bPressed == (touchIndex + 1) && isPtIn( pt ) )
				{
					m_bPressed = false;
					return true;
				}
				break;
			}
		}
		return false;
	}

	void UIComboBox::Render( float fElapsedTime )
	{CPUTime(UIComboBox);
		if(!this->m_bVisible)
			return;
		UIElement* pElement = &m_btnElement[ 2 ];
		static char bSBInit;
		if( !bSBInit )
		{
			if( GetDlgManager()->GetFontNode( pElement->fontIndex )->GetHeight() )
				m_scrollBar.SetPageSize( RectHeight( m_rcDropdownText ) / GetDlgManager()->GetFontNode( pElement->fontIndex )->GetHeight() );
			else
				m_scrollBar.SetPageSize( RectHeight( m_rcDropdownText ) );
			bSBInit = true;
		}
		g_manager->SetBlendMode((BlendMode)pElement->m_nSrcBlend, (BlendMode)pElement->m_nDestBlend);
		if( m_bOpened )
		{
			if(this->m_scrollBarWidth > 0)
				m_scrollBar.RenderScroll( this, fElapsedTime );
			pElement->dwTextFormat			=	m_textFormat;
			//
			m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &m_rcDropdown, pElement->textureColor * m_currentAnim.textureColor, &m_currentAnim);
			//
			UIElement* pSelectionElement = &m_btnElement[ 3 ];
			pSelectionElement->dwTextFormat		=	m_textFormat;
			FontNode2D* pFont = GetDlgManager()->GetFontNode( pElement->fontIndex );
			//
			if( pFont )
			{
				int curY = m_rcDropdownText.top;
				int nRemainingHeight = RectHeight( m_rcDropdownText );
				for( int i = m_scrollBar.GetScrollPos(); i < (int)m_listItems.size(); i++ )
				{
					ListBoxItem* pItem = m_listItems.at( i );
					nRemainingHeight -= pFont->GetHeight();
					if( nRemainingHeight < 0 )
					{
						pItem->bVisible = false;
						continue;
					}
					pItem->rcActive.Set( m_rcDropdownText.left, curY, m_rcDropdownText.right, curY + pFont->GetHeight() );
					curY += pFont->GetHeight();
					pItem->bVisible = true;
					if( m_bOpened )
					{
						if( (int)i == m_focuses )
						{
							Rect rc;
							rc.Set( m_rcDropdown.left, pItem->rcActive.top-2, m_rcDropdown.right, pItem->rcActive.bottom+2 );
							m_dialogPtr->DrawElement(GetEnabled(), pSelectionElement, &pSelectionElement->rcTexture, &rc, pSelectionElement->textureColor * m_currentAnim.textureColor, &m_currentAnim);
							color4 fontColor;
							if(pItem->bHighColor)
								fontColor	=	pItem->textHighColor;//pSelectionElement->fontColor;
							else
								fontColor	=	pSelectionElement->fontColor;
							//m_dialogPtr->DrawElementText(GetEnabled(), m_currentAnim.rc, pItem->itemText.strW(), pSelectionElement, &pItem->rcActive, fontColor * this->m_currentAnim.textColor, m_nShadowWidth, m_shadowColor, true, &m_currentAnim);
						}
						else
						{
							color4 fontColor;
							if(pItem->bTextColor)
								fontColor	=	pItem->textColor;//pSelectionElement->fontColor;
							else
								fontColor	=	pElement->fontColor;
							//m_dialogPtr->DrawElementText(GetEnabled(), m_currentAnim.rc, pItem->itemText.strW(), pElement, &pItem->rcActive, fontColor * this->m_currentAnim.textColor, m_nShadowWidth, m_shadowColor, true, &m_currentAnim);
						}
					}
				}
			}
		}
		int nOffsetX = 0;
		int nOffsetY = 0;
		float fBlendRate = 1.0f;
		pElement = &m_btnElement[ 1 ];
		Rect rcWindow = m_rcButton;
		rcWindow.Offset( nOffsetX, nOffsetY );
		//
		m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &rcWindow, pElement->textureColor * m_currentAnim.textureColor, &m_currentAnim);
		pElement			= &m_btnElement[ 0 ];
		pElement->dwTextFormat			=	m_textFormat;
		m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &m_rcText, pElement->textureColor * m_currentAnim.textureColor, &m_currentAnim);
		if( m_selected >= 0 && m_selected < (int) m_listItems.size() )
		{
			ListBoxItem* pItem = m_listItems.at( m_selected );
			if( pItem != NULL )
			{
				color4 fontColor;
				if(pItem->bTextColor)
					fontColor	=	pItem->textColor;//pSelectionElement->fontColor;
				else
					fontColor	=	pElement->fontColor;
				//m_dialogPtr->DrawElementText(GetEnabled(), m_currentAnim.rc, pItem->itemText.strW(), pElement, &m_rcText, fontColor * m_currentAnim.textColor, m_nShadowWidth, m_shadowColor, true, &m_currentAnim);
			}
		}
		this->RenderDrawMgrs();
	}

	int	UIComboBox::AddItem( const char* strText, INT pData )
	{CPUTime(UIComboBox);
		if( strText== NULL )
		{
			return -1;
		}
		ListBoxItem* pItem = new ListBoxItem;
		if( pItem == NULL )
		{
			return -1;
		}
		pItem->itemText	=	strText;
		pItem->pData	=	pData;
		m_listItems.push_back( pItem );
		m_scrollBar.SetScrollRange( 0, (int)m_listItems.size() );
		if( GetNumItems() == 1 )
		{
			m_selected = 0;
			m_focuses = 0;
		}
		return GetNumItems() - 1;
	}
	void UIComboBox::RemoveItem( unsigned int index )
	{CPUTime(UIComboBox);
		ListBoxItem* pItem = m_listItems.at( index );
		safe_delete( pItem );
		m_listItems.erase(m_listItems.begin() + index);
		m_scrollBar.SetScrollRange( 0, (int)m_listItems.size() );
		if( m_selected >= (int)m_listItems.size() )
			m_selected = (int)m_listItems.size() - 1;
	}
	void UIComboBox::SetItemText( int nIndex, const char *wszText )	//插入一行
	{CPUTime(UIComboBox);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return;
		ListBoxItem *pItem = m_listItems.at( nIndex );
		pItem->itemText	=	wszText;
	}

	void UIComboBox::RemoveAllItems()
	{CPUTime(UIComboBox);
		for( int i=0; i < (int)m_listItems.size(); i++ )
		{
			ListBoxItem* pItem = m_listItems.at( i );
			safe_delete( pItem );
		}
		m_listItems.clear();
		m_scrollBar.SetScrollRange( 0, 1 );
		m_focuses = m_selected = -1;
	}
	char UIComboBox::ContainsItem( const char* strText, unsigned int iStart )
	{CPUTime(UIComboBox);
		return ( -1 != FindItem( strText, iStart ) );
	}
	int UIComboBox::FindItem( const char* strText, unsigned int iStart )
	{CPUTime(UIComboBox);
		if( strText == NULL )
			return -1;
		for( int i = iStart; i < (int)m_listItems.size(); i++ )
		{
			ListBoxItem* pItem = m_listItems.at(i);
			if(pItem->itemText == strText)
			{
				return i;
			}
		}
		return -1;
	}
	INT UIComboBox::GetSelectedData()
	{CPUTime(UIComboBox);
		if( m_selected < 0 )
			return NULL;
		ListBoxItem* pItem = m_listItems.at( m_selected );
		return pItem->pData;
	}
	ListBoxItem* UIComboBox::GetSelectedItem()
	{CPUTime(UIComboBox);
		if( m_selected < 0 )
			return NULL;
		return m_listItems.at( m_selected );
	}
	INT UIComboBox::GetItemData( const char* strText )
	{CPUTime(UIComboBox);
		int index = FindItem( strText );
		if( index == -1 )
		{
			return NULL;
		}
		ListBoxItem* pItem = m_listItems.at(index);
		if( pItem == NULL )
		{
			return NULL;
		}
		return pItem->pData;
	}
	INT UIComboBox::GetItemData( int nIndex )
	{CPUTime(UIComboBox);
		if( nIndex < 0 || nIndex >= m_listItems.size() )
			return NULL;
		return m_listItems.at( nIndex )->pData;
	}
	VOID	UIComboBox::SetItemData( int nIndex, INT d )
	{CPUTime(UIComboBox);
		if( nIndex < 0 || nIndex >= m_listItems.size() )
			return;
		m_listItems.at( nIndex )->pData	=	d;
	}
	char	UIComboBox::SetSelectedByIndex( unsigned int index )
	{CPUTime(UIComboBox);
		if( index < 0 || index >= GetNumItems() )
			return false;
		m_focuses = m_selected = index;
		return true;
	}
	char	UIComboBox::SetSelectedByText( const char* strText )
	{CPUTime(UIComboBox);
		if( strText == NULL )
			return false;
		int index = FindItem( strText );
		if( index == -1 )
			return false;
		m_focuses = m_selected = index;
		return true;
	}
	char	UIComboBox::SetSelectedByData( INT pData )
	{CPUTime(UIComboBox);
		for( int i=0; i < (int)m_listItems.size(); i++ )
		{
			ListBoxItem* pItem = m_listItems.at(i);
			if( pItem->pData == pData )
			{
				m_focuses = m_selected = i;
				return true;
			}
		}
		return false;
	}
	char		UIComboBox::LoadControl(CSafeFileHelperR& r)
	{CPUTime(UIComboBox);
		UIButton::LoadControl( r );
		int sb=0;
		r >> m_selected >> m_focuses >> m_nDropHeight >> sb >> m_bOpened >> m_rcText >> m_rcButton >> m_rcDropdown >> m_rcDropdownText;
		m_scrollBarWidth = sb;
		for(int i=0;i<(int)m_listItems.size();i++)
		{
			safe_delete(m_listItems.at(i));
		}
		m_listItems.clear();
		//
		int item;
		r >> item;
		for(int i=0;i<item;i++)
		{
			ListBoxItem* pItem = new ListBoxItem;
			if(this->m_loadVersion<0x000000A2){
				CDynamicArray<short_t> str;
				//pItem->itemText	=	str.GetPtr();
				r >> str;
			}
			else
			{
				std::string str;
				r >> str;
				pItem->itemText = str.c_str();
			}
			r >> pItem->rcActive;
			m_listItems.push_back( pItem );
		}
		m_scrollBar.LoadControl( r );
		return true;
	}
	char			UIComboBox::SaveControl(CSafeFileHelperW& w)
	{CPUTime(UIComboBox);
		UIButton::SaveControl( w );
		int sb = m_scrollBarWidth;
		w << m_selected << m_focuses << m_nDropHeight << sb << m_bOpened << m_rcText << m_rcButton << m_rcDropdown << m_rcDropdownText;
		w << m_listItems.size();
		for(int i=0;i<(int)m_listItems.size();i++)
		{
			w << m_listItems.at(i)->itemText.str() << m_listItems.at(i)->rcActive;
		}
		m_scrollBar.SaveControl( w );
		return true;
	}

	VOID			UIComboBox::SetItemColor(unsigned int nIndex, const color4& c)
	{CPUTime(UIComboBox);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return;
		ListBoxItem *pItem = m_listItems.at( nIndex );
		pItem->bTextColor	=	true;
		pItem->textColor	=	c;
	}
	const color4*	UIComboBox::GetItemColor(unsigned int nIndex)
	{CPUTime(UIComboBox);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return 0;
		ListBoxItem *pItem = m_listItems.at( nIndex );
		return &pItem->textColor;
	}
	VOID			UIComboBox::SetItemHighColor(unsigned int nIndex, const color4& c)
	{CPUTime(UIComboBox);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return;
		ListBoxItem *pItem = m_listItems.at( nIndex );
		pItem->bHighColor		=	true;
		pItem->textHighColor	=	c;
	}
	const color4*	UIComboBox::GetItemHighColor(unsigned int nIndex)
	{CPUTime(UIComboBox);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return 0;
		ListBoxItem *pItem = m_listItems.at( nIndex );
		return &pItem->textHighColor;
	}

};
