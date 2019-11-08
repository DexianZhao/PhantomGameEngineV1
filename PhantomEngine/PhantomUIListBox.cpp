//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIListBox.h"
#include "PhantomUIDialog.h"
#include "PhantomUIManager.h"
#include "PhantomManager.h"

namespace Phantom{


	UIListBox::UIListBox( BOOL bInit, UIDialog *dialogPtr ) : m_scrollBar( bInit, dialogPtr ), UIControl(bInit, dialogPtr)
	{
		m_controlType		=	UIControlType_LISTBOX;
		m_dialogPtr			=	dialogPtr;
		m_bMultiselect		=	FALSE;
		m_scrollBarWidth	= 	16;
		m_nSelected			= 	-1;
		m_nSelStart			= 	0;
		m_bDrag				= 	false;
		m_borderWidth		= 	6;
		m_marginWidth		= 	5;
		m_nTextHeight		=	0;
		m_nBeginLeft		=	0;
		//
		ClearElement();
		for(int i=0;i<MaxListBoxElement;i++)
			AddElement(&m_listElement[i]);
		if(bInit)
		{
			m_listElement[0].SetSourceRect(Rect(4, 90, 4 + 58, 90 + 26));
			m_listElement[0].SetRenderScale(false, false, Pixel(20, 9));
//			m_listElement[0].m_name = ("列表框[底]");
			m_listElement[1].SetFont( 0, COLOR_ARGB(255, 255, 255, 255), DrawTextAlign_LEFT | DrawTextAlign_TOP );
			m_listElement[1].SetSourceRect(Rect(117, 33, 117 + 56, 33 + 7));
			m_listElement[1].SetRenderScale(false, false, Pixel(10, 3));
//			m_listElement[1].m_name = ("列表框[选中条]");
		}
	}

	UIListBox::~UIListBox()
	{
		RemoveAllItems();
	}

	int					UIListBox::GetElementCount()
	{CPUTime(UIListBox);
		return this->m_elementCount + m_scrollBar.m_elementCount;
	}
	UIElement*			UIListBox::GetElementPtr(int index)
	{CPUTime(UIListBox);
		if(index<0)
			return 0;
		if(index < m_elementCount)
			return this->m_elementList[index];
		index -= m_elementCount;
		if(index >= m_scrollBar.m_elementCount)
			return 0;
		return m_scrollBar.m_elementList[index];
	}

	VOID UIListBox::OnAnimationRect()
	{CPUTime(UIListBox);
		m_rcSelection = m_windowRect;
		m_rcSelection.right -= m_scrollBarWidth;
		m_rcSelection.inflate( -m_borderWidth, -m_borderWidth );
		m_rcText = m_rcSelection;
		m_rcText.inflate( -m_marginWidth, 0 );
		m_scrollBar.m_windowRect	=	Rect(m_windowRect.right - m_scrollBarWidth, m_windowRect.top, m_windowRect.right, m_windowRect.top + GetHeight());
		m_scrollBar.OnAnimationRect();
	}

	void UIListBox::recalcRects()
	{CPUTime(UIListBox);
		UIControl::recalcRects();
		OnAnimationRect();
		FontNode2D* pFontNode = GetDlgManager()->GetFontNode( this->m_listElement[0].fontIndex );
		if( pFontNode && pFontNode->GetHeight() )
		{
			m_scrollBar.SetPageSize( RectHeight( m_rcText ) / pFontNode->GetHeight() );
			m_scrollBar.ShowItem( m_nSelected );
		}
	}

	int UIListBox::AddItem( const char *wszText, INT pData )
	{CPUTime(UIListBox);
		ListBoxItem *pNewItem = new ListBoxItem;
		if( !pNewItem )
			return -1;
		pNewItem->itemText	=	wszText;
		pNewItem->pData = pData;
		pNewItem->rcActive.Set( 0, 0, 0, 0 );
		pNewItem->bSelected = false;
		m_listItems.push_back( pNewItem );
		m_scrollBar.SetScrollRange( 0, (int)m_listItems.size() );
		return (int)m_listItems.size() - 1;
	}

	int	UIListBox::InsertItem( int nIndex, const char *wszText, INT pData )
	{CPUTime(UIListBox);
		ListBoxItem *pNewItem = new ListBoxItem;
		if( !pNewItem )
			return -1;
		pNewItem->itemText	=	wszText;
		pNewItem->pData = pData;
		pNewItem->rcActive.Set( 0, 0, 0, 0 );
		pNewItem->bSelected = false;
		m_listItems.insert(m_listItems.begin() + nIndex, pNewItem );
		m_scrollBar.SetScrollRange( 0, (int)m_listItems.size() );
		return (int)m_listItems.size() - 1;
	}

	void UIListBox::SetItemText( int nIndex, const char *wszText )	//插入一行
	{CPUTime(UIListBox);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return;
		ListBoxItem *pItem = m_listItems.at( nIndex );
		pItem->itemText	=	wszText;
	}

	void UIListBox::RemoveItem( int nIndex )
	{CPUTime(UIListBox);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return;
		ListBoxItem *pItem = m_listItems.at( nIndex );
		delete pItem;
		m_listItems.erase(m_listItems.begin() + nIndex);
		m_scrollBar.SetScrollRange( 0, (int)m_listItems.size() );
		if( m_nSelected >= (int)m_listItems.size() )
			m_nSelected = (int)m_listItems.size() - 1;
	}

	void		UIListBox::RemoveAllItems()
	{CPUTime(UIListBox);
		for( int i = 0; i < (int)m_listItems.size(); ++i )
		{
			ListBoxItem *pItem = m_listItems.at( i );
			delete pItem;
		}
		m_listItems.clear();
		m_scrollBar.SetScrollRange( 0, 1 );
	}

	char		UIListBox::LoadControl(CSafeFileHelperR& r)
	{CPUTime(UIListBox);
		UIControl::LoadControl( r );
		skip_r sr;
		if(m_loadVersion>=0x000000A4)
			sr.begin(r.pStream);
		int sb = 0;
		r >> m_rcText >> m_rcSelection >> sb >> m_borderWidth >> m_marginWidth >> m_nTextHeight >> m_bMultiselect >> m_nSelected >> m_nSelStart >> m_bDrag;
		m_scrollBarWidth = sb;
		m_scrollBar.LoadControl( r );
		for(int i=0;i<(int)m_listItems.size();i++)
		{
			safe_delete(m_listItems.at(i));
		}
		m_listItems.clear();
		int item;
		r >> item;
		for(int i=0;i<item;i++)
		{
			ListBoxItem* pItem = new ListBoxItem();
			if(this->m_loadVersion<0x000000A2){
				CDynamicArray<short_t> str;
				r >> str;
				//pItem->itemText	=	str.GetPtr();
			}
			else
			{
				std::string str;
				r >> str;
				pItem->itemText	=	str.c_str();
			}
			r >> pItem->rcActive;
			m_listItems.push_back( pItem );
		}
		sr.end();
		return true;
	}
	char		UIListBox::SaveControl(CSafeFileHelperW& w)
	{CPUTime(UIListBox);
		UIControl::SaveControl( w );
		skip_w sw(w.pStream);
		int sb = m_scrollBarWidth;
		w << m_rcText << m_rcSelection << sb << m_borderWidth << m_marginWidth << m_nTextHeight << m_bMultiselect << m_nSelected << m_nSelStart << m_bDrag;
		m_scrollBar.SaveControl( w );
		w << m_listItems.size();
		for(int i=0;i<(int)m_listItems.size();i++)
		{
			w << m_listItems.at(i)->itemText.str() << m_listItems.at(i)->rcActive;
		}
		sw.end();
		return true;
	}
	ListBoxItem* UIListBox::GetItem( int nIndex )
	{CPUTime(UIListBox);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return NULL;
		return m_listItems[nIndex];
	}

	int UIListBox::GetSelectedIndex( int nPreviousSelected )
	{CPUTime(UIListBox);
		if( nPreviousSelected < -1 )
			return -1;
		if( m_bMultiselect )
		{
			for( int i = nPreviousSelected + 1; i < (int)m_listItems.size(); ++i )
			{
				ListBoxItem *pItem = m_listItems.at( i );
				if( pItem->bSelected )
					return i;
			}
			return -1;
		}
		else
		{
			return m_nSelected;
		}
	}

	void UIListBox::SelectItem( int nNewIndex )
	{CPUTime(UIListBox);
		if( m_listItems.size() == 0 )
			return;
		int nOldSelected = m_nSelected;
		m_nSelected = nNewIndex;
		if( m_nSelected < 0 )
			m_nSelected = 0;
		if( m_nSelected >= (int)m_listItems.size() )
			m_nSelected = (int)m_listItems.size() - 1;
		if( nOldSelected != m_nSelected )
		{
			if( m_bMultiselect )
			{
				m_listItems[m_nSelected]->bSelected = true;
			}
			m_nSelStart = m_nSelected;
			m_scrollBar.ShowItem( m_nSelected );
		}
	}

	void	UIListBox::SetEnabled(char bEnabled)
	{CPUTime(UIListBox);
		UIControl::SetEnabled( bEnabled );
		this->m_scrollBar.SetEnabled( bEnabled );
	}

	char UIListBox::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex )
	{CPUTime(UIListBox);
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
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
			if( m_listItems.size() > 0 && m_rcSelection.isPtIn(pt.x, pt.y) )
			{
				int nClicked;
				if( m_nTextHeight )
					nClicked = m_scrollBar.GetScrollPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
				else
					nClicked = -1;
				if( nClicked >= m_scrollBar.GetScrollPos() &&
					nClicked < (int)m_listItems.size() &&
					nClicked < m_scrollBar.GetScrollPos() + m_scrollBar.GetPageSize() )
				{
					m_bDrag = (touchIndex + 1);
					if( uMsg == InputEventID_ButtonDblClk )
					{
						m_dialogPtr->RaiseEvent(this, EventID_DoubleClick, pt);
						return true;
					}
					m_nSelected = nClicked;
					m_nSelStart = m_nSelected;
					//m_dialogPtr->RaiseEvent(this, (EventID_Changed));
				}
				return true;
			}
			break;
		case InputEventID_ButtonUp:
			{
				m_bDrag = false;
				if( m_nSelected >= 0 && m_nSelected < m_listItems.size())
				{
					int nEnd = getmax( m_nSelStart, m_nSelected );
					for( int n = getmin( m_nSelStart, m_nSelected ) + 1; n < nEnd; ++n )
						m_listItems[n]->bSelected = m_listItems[m_nSelStart]->bSelected;
					m_listItems[m_nSelected]->bSelected = m_listItems[m_nSelStart]->bSelected;
					m_dialogPtr->RaiseEvent(this, EventID_Click, pt);
				}
				else
					m_nSelected	=	-1;
				return false;
			}
		case InputEventID_MouseMove:
			{
				char bMouseMove	=	isPtIn(pt);
				if(bMouseMove)
				{
					if(!m_bMouseEnter)
						this->onMouseEnter();
				}
				//if( m_bDrag )
				//{
				//	int nItem;
				//	if( m_nTextHeight )
				//		nItem = m_scrollBar.GetScrollPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
				//	else
				//		nItem = -1;
				//	if( nItem >= (int)m_scrollBar.GetScrollPos() &&
				//		nItem < (int)m_listItems.size() &&
				//		nItem < m_scrollBar.GetScrollPos() + m_scrollBar.GetPageSize() )
				//	{
				//		m_nSelected = nItem;
				//		m_dialogPtr->RaiseEvent(this, (EventID_Changed));
				//	}
				//	else
				//		if( nItem < (int)m_scrollBar.GetScrollPos() )
				//		{
				//			m_scrollBar.SetScroll( -1 );
				//			m_nSelected = m_scrollBar.GetScrollPos();
				//			m_dialogPtr->RaiseEvent(this, (EventID_Changed));
				//		}
				//		else
				//			if( nItem >= m_scrollBar.GetScrollPos() + m_scrollBar.GetPageSize() )
				//			{
				//				m_scrollBar.SetScroll( 1 );
				//				m_nSelected = getmin( (int)m_listItems.size(), m_scrollBar.GetScrollPos() + m_scrollBar.GetPageSize() ) - 1;
				//				m_dialogPtr->RaiseEvent(this, (EventID_Changed));
				//			}
				//}
				return isPtIn( pt );
			}
		}
		return false;
	}

	void UIListBox::Render( float fElapsedTime )
	{CPUTime(UIListBox);
		if( m_bVisible == false )
			return;
		UIElement* pElement = &m_listElement[0];
		pElement->dwTextFormat		=	this->m_textFormat;
		m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &m_currentAnim.rc, pElement->textureColor * m_currentAnim.textureColor, &m_currentAnim, this);
		UIElement* pSelElement = &m_listElement[1];//m_elements.at( 1 );
		pSelElement->dwTextFormat		=	this->m_textFormat;
		pSelElement->fontIndex			=	pElement->fontIndex;
		if( m_listItems.size() > 0 )
		{
			Rect rc = m_rcText;
			Rect rcSel = m_rcSelection;
			rc.bottom = rc.top + GetDlgManager()->GetFontNode( pElement->fontIndex )->GetHeight();
			if(m_nTextHeight <= 0)
				m_nTextHeight = rc.bottom - rc.top;
			static char bSBInit;
			if( !bSBInit )
			{
				if( m_nTextHeight )
					m_scrollBar.SetPageSize( RectHeight( m_rcText ) / m_nTextHeight );
				else
					m_scrollBar.SetPageSize( RectHeight( m_rcText ) );
				bSBInit = true;
			}
			rc.left	 += m_nBeginLeft;
			rc.right = m_rcText.right;
			for( int i = m_scrollBar.GetScrollPos(); i < (int)m_listItems.size(); ++i )
			{
				if( rc.bottom > m_rcText.bottom )
					break;
				ListBoxItem *pItem = m_listItems.at( i );
				char bSelectedStyle = false;
				if( !m_bMultiselect && i == m_nSelected )
					bSelectedStyle = true;
				else
					if(m_bMultiselect)
					{
						if( m_bDrag &&
							( ( i >= m_nSelected && i < m_nSelStart ) ||
							( i <= m_nSelected && i > m_nSelStart ) ) )
							bSelectedStyle = m_listItems[m_nSelStart]->bSelected;
						else
							if( pItem->bSelected )
								bSelectedStyle = true;
					}
					rc.bottom	=	rc.top + m_nTextHeight;
					pElement->dwTextFormat	=	Phantom::DrawTextAlign_LEFT | Phantom::DrawTextAlign_BOTTOM;
					pSelElement->dwTextFormat	=	Phantom::DrawTextAlign_LEFT | Phantom::DrawTextAlign_BOTTOM;
					if( bSelectedStyle )
					{
						rcSel.top = rc.top; rcSel.bottom = rcSel.top + m_nTextHeight;
						m_dialogPtr->DrawElement(GetEnabled(), pSelElement, &pSelElement->rcTexture, &rcSel, pSelElement->textureColor * m_currentAnim.textureColor, &m_currentAnim, this);
						color4 fontColor;
						if(pItem->bHighColor)
							fontColor	=	pItem->textHighColor;//pSelectionElement->fontColor;
						else
							fontColor	=	pSelElement->fontColor;
						//m_dialogPtr->DrawElementText(GetEnabled(), m_currentAnim.rc, pItem->itemText.strW(), pSelElement, &rc, fontColor * m_currentAnim.textColor, m_nShadowWidth, m_shadowColor, true, &m_currentAnim);
					}
					else
					{
						color4 fontColor;
						if(pItem->bTextColor)
							fontColor	=	pItem->textColor;//pSelectionElement->fontColor;
						else
							fontColor	=	pElement->fontColor;
						//m_dialogPtr->DrawElementText(GetEnabled(), m_currentAnim.rc, pItem->itemText.strW(), pElement, &rc, fontColor * m_currentAnim.textColor, m_nShadowWidth, m_shadowColor, true, &m_currentAnim);
					}
					pItem->rcActive			=	rc;
					pItem->rcActive.bottom	=	rc.top + m_nTextHeight;
					rc.Offset( 0, m_nTextHeight );
			}
		}
		if(this->m_scrollBarWidth > 0)
			m_scrollBar.RenderScroll( this, fElapsedTime );
		this->RenderDrawMgrs();
	}

	INT	UIListBox::GetSelectedData( int nPreviousSelected )
	{CPUTime(UIListBox);
		ListBoxItem* pItem = GetItem( GetSelectedIndex( nPreviousSelected ) );
		if(pItem)
			return pItem->pData;
		return 0;
	}

	VOID			UIListBox::SetItemColor(unsigned int nIndex, const color4& c)
	{CPUTime(UIListBox);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return;
		ListBoxItem *pItem = m_listItems.at( nIndex );
		pItem->bTextColor	=	true;
		pItem->textColor	=	c;
	}
	const color4*	UIListBox::GetItemColor(unsigned int nIndex)
	{CPUTime(UIListBox);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return 0;
		ListBoxItem *pItem = m_listItems.at( nIndex );
		return &pItem->textColor;
	}
	VOID			UIListBox::SetItemHighColor(unsigned int nIndex, const color4& c)
	{CPUTime(UIListBox);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return;
		ListBoxItem *pItem = m_listItems.at( nIndex );
		pItem->bHighColor		=	true;
		pItem->textHighColor	=	c;
	}
	const color4*	UIListBox::GetItemHighColor(unsigned int nIndex)
	{CPUTime(UIListBox);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return 0;
		ListBoxItem *pItem = m_listItems.at( nIndex );
		return &pItem->textHighColor;
	}

};
