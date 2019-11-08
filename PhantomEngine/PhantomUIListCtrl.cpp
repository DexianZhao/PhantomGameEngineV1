//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIListCtrl.h"
#include "PhantomUIManager.h"
#include "PhantomManager.h"

namespace Phantom{

	UIListCtrl::UIListCtrl( BOOL bInit, UIDialog *dialogPtr ) : m_scrollBar( bInit, dialogPtr ), UIControl(bInit, dialogPtr)
	{
		m_controlType		=	UIControlType_LISTCTRL3D;
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
		m_nColumnHeight		=	16;
		//
		m_bSortEnable		=	true;	//是否支持排序
		m_sortColumn		=	0;	//
		m_sortUpdown		=	true;	//
		//
		ClearElement();
		for(int i=0;i<MaxListCtrlElement;i++)
			AddElement(&m_lcElement[i]);
		if(bInit)
		{
			Rect rcTexture;
			rcTexture.Set(0, 88, 180, 38 );
			m_lcElement[0].SetTexture( 0, &rcTexture );
			m_lcElement[0].SetFont( 0, COLOR_ARGB(255, 0, 0, 0), DrawTextAlign_LEFT | DrawTextAlign_TOP );
			m_lcElement[0].SetSourceRect(Rect(17, 90, 17 + 37, 90 + 26));
			m_lcElement[0].SetRenderScale(false, false, Pixel(10, 9));
//			m_lcElement[0].m_name = ("列表框[底]");
			rcTexture.Set(0, 131, 180, 22 );
			m_lcElement[1].SetTexture( 0, &rcTexture );
			m_lcElement[1].SetFont( 0, COLOR_ARGB(255, 255, 255, 255), DrawTextAlign_LEFT | DrawTextAlign_TOP );
			m_lcElement[1].SetSourceRect(Rect(117, 33, 117 + 56, 33 + 7));
			m_lcElement[1].SetRenderScale(false, false, Pixel(10, 3));
//			m_lcElement[1].m_name = ("列表框[选中条]");
			m_lcElement[2].SetSourceRect(Rect(85, 2, 85 + 26, 2 + 26));
			m_lcElement[2].SetRenderScale(false, false, Pixel(10, 3));
			m_lcElement[2].dwTextFormat	=	DrawTextAlign_CENTER | DrawTextAlign_VCENTER | DrawTextAlign_SINGLELINE;
//			m_lcElement[2].m_name = ("列表框[表头按钮]");
			m_lcElement[3].SetSourceRect(Rect(93, 40, 93 + 7, 40 + 5));
			m_lcElement[3].SetRenderScale(false, false, Pixel(10, 3));
			m_lcElement[3].dwTextFormat	=	DrawTextAlign_CENTER | DrawTextAlign_VCENTER | DrawTextAlign_SINGLELINE;
//			m_lcElement[3].m_name = ("列表框[排序向上]");
			m_lcElement[4].SetSourceRect(Rect(93, 65, 93 + 7, 65 + 5));
			m_lcElement[4].SetRenderScale(false, false, Pixel(10, 3));
			m_lcElement[4].dwTextFormat	=	DrawTextAlign_CENTER | DrawTextAlign_VCENTER | DrawTextAlign_SINGLELINE;
//			m_lcElement[4].m_name = ("列表框[排序向下]");
		}
	}

	UIListCtrl::~UIListCtrl()
	{
		RemoveAllItems();
	}

	int					UIListCtrl::GetElementCount()
	{CPUTime(UIListCtrl);
		return this->m_elementCount + m_scrollBar.m_elementCount;
	}
	UIElement*			UIListCtrl::GetElementPtr(int index)
	{CPUTime(UIListCtrl);
		if(index<0)
			return 0;
		if(index < m_elementCount)
			return this->m_elementList[index];
		index -= m_elementCount;
		if(index >= m_scrollBar.m_elementCount)
			return 0;
		return m_scrollBar.m_elementList[index];
	}
	VOID UIListCtrl::OnAnimationRect()
	{CPUTime(UIListCtrl);
		m_rcSelection = m_windowRect;
		m_rcSelection.right -= m_scrollBarWidth;
		m_rcSelection.inflate(-m_borderWidth, -m_borderWidth );
		m_rcText = m_rcSelection;
		m_rcText.inflate(-m_marginWidth, 0 );
		m_scrollBar.m_windowRect	=	Rect(m_windowRect.right - m_scrollBarWidth, m_windowRect.top,  m_windowRect.right, m_windowRect.top + GetHeight());
	}

	void UIListCtrl::recalcRects()
	{CPUTime(UIListCtrl);
		UIControl::recalcRects();
		OnAnimationRect();
		FontNode2D* pFontNode = GetDlgManager()->GetFontNode( m_lcElement[0].fontIndex );
		if( pFontNode && pFontNode->GetHeight() )
		{
			m_scrollBar.SetPageSize( RectHeight( m_rcText ) / pFontNode->GetHeight() );
			m_scrollBar.ShowItem( m_nSelected );
		}
	}

	int UIListCtrl::AddItem( const char *wszText, INT pData )
	{CPUTime(UIListCtrl);
		return InsertItem((int)m_listItems.size(), wszText, pData);
	}

	int	UIListCtrl::InsertItem( int nIndex, const char *wszText, INT pData )
	{CPUTime(UIListCtrl);
		if(m_columns.size() == 0)
			return -1;
		ListCtrlItem *pNewItem = new ListCtrlItem;
		if( !pNewItem )
			return -1;
		//
		pNewItem->texts.resize(m_columns.size());
		for(int i=0;i<pNewItem->texts.size();i++)
		{
			pNewItem->texts[i]	=	new Text();
		}
		//
		pNewItem->texts[0]->operator=(wszText);
		pNewItem->pData = pData;
		pNewItem->rcActive.Set( 0, 0, 0, 0 );
		pNewItem->bSelected = false;
		if(nIndex >= m_listItems.size())
			m_listItems.push_back( pNewItem );
		else
			m_listItems.insert(m_listItems.begin() + nIndex, pNewItem );
		m_scrollBar.SetScrollRange( 0, (int)m_listItems.size() );
		return (int)m_listItems.size() - 1;
	}

	void UIListCtrl::RemoveItem( int nIndex )
	{CPUTime(UIListCtrl);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return;
		ListCtrlItem *pItem = m_listItems.at( nIndex );
		delete pItem;
		m_listItems.erase(m_listItems.begin() + nIndex);
		m_scrollBar.SetScrollRange( 0, (int)m_listItems.size() );
		if( m_nSelected >= (int)m_listItems.size() )
			m_nSelected = (int)m_listItems.size() - 1;
	}

	void		UIListCtrl::RemoveAllItems()
	{CPUTime(UIListCtrl);
		for( int i = 0; i < (int)m_listItems.size(); ++i )
		{
			ListCtrlItem *pItem = m_listItems.at( i );
			delete pItem;
		}
		m_listItems.clear();
		m_scrollBar.SetScrollRange( 0, 1 );
	}

	char		UIListCtrl::LoadControl(CSafeFileHelperR& r)
	{CPUTime(UIListCtrl);
		UIControl::LoadControl( r );
		int sb = 0;
		r >> m_rcText >> m_rcSelection >> sb >> m_borderWidth >> m_marginWidth >> m_nTextHeight >> m_bMultiselect >> m_nSelected >> m_nSelStart >> m_bDrag;
		m_scrollBarWidth = sb;
		m_scrollBar.LoadControl( r );
		for(int i=0;i<m_columns.size();i++)
			delete m_columns[i];
		m_columns.clear();
		int column;
		r >> column;
		m_columns.resize(column);
		for(int i=0;i<column;i++)
		{
			m_columns[i]	=	new ListColumnItem;
			if(this->m_loadVersion<0x000000A3){
				CDynamicArray<short_t> str;
				r >> str;
				//TextAW temp;
				//temp =	str.GetPtr();
				//m_columns[i]->itemText = temp.strA();
			}
			else
			{
				CDynamicArray<char> str;
				r >> str;
				m_columns[i]->itemText = str.GetPtr();
			}
			r >> m_columns[i]->width;
			
		}
		//
		for(int i=0;i<(int)m_listItems.size();i++)
		{
			safe_delete(m_listItems.at(i));
		}
		m_listItems.clear();
		int item;
		r >> item;
		for(int i=0;i<item;i++)
		{
			ListCtrlItem* pItem = new ListCtrlItem();
			r >> pItem->bSelected >> pItem->bVisible >> pItem->rcActive;
			if(m_columns.size() > 0)
			{
				pItem->texts.resize(m_columns.size());
				for(int c=0;c<m_columns.size();c++)
				{
					pItem->texts[c] = new Text();
					if(this->m_loadVersion<0x000000A3){
						CDynamicArray<short_t> str;
						r >> str;
						//TextAW temp;
						//temp.setW(str.GetPtr());
						//pItem->texts[c]->operator=(temp.strA());
					}
					else
					{
						CDynamicArray<char> str;
						r >> str;
						pItem->texts[c]->operator=(str.GetPtr());
					}
				}
			}
			m_listItems.push_back( pItem );
		}
		r >> this->m_nColumnHeight;
		r >> m_bSortEnable >> m_sortColumn >> m_sortUpdown;
		return true;
	}

	char		UIListCtrl::SaveControl(CSafeFileHelperW& w)
	{CPUTime(UIListCtrl);
		UIControl::SaveControl( w );
		int sb = m_scrollBarWidth;
		w << m_rcText << m_rcSelection << sb << m_borderWidth << m_marginWidth << m_nTextHeight << m_bMultiselect << m_nSelected << m_nSelStart << m_bDrag;
		m_scrollBar.SaveControl( w );
		int column	=	m_columns.size();
		w << column;
		for(int i=0;i<column;i++)
		{
			w << m_columns[i]->itemText.str() << m_columns[i]->width;
		}
		int itemCount	=	m_listItems.size();
		w << itemCount;
		for(int i=0;i<(int)m_listItems.size();i++)
		{
			w << m_listItems[i]->bSelected << m_listItems[i]->bVisible << m_listItems[i]->rcActive;
			if(m_columns.size() > 0)
			{
				for(int c=0;c<m_columns.size();c++)
				{
					w << m_listItems[i]->texts[c]->str();
				}
			}
		}
		w << this->m_nColumnHeight;
		w << m_bSortEnable << m_sortColumn << m_sortUpdown;
		return true;
	}
	ListCtrlItem* UIListCtrl::GetItem( int nIndex )
	{CPUTime(UIListCtrl);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return NULL;
		return m_listItems[nIndex];
	}

	int UIListCtrl::GetSelectedIndex( int nPreviousSelected )
	{CPUTime(UIListCtrl);
		if( nPreviousSelected < -1 )
			return -1;
		if( m_bMultiselect )
		{
			for( int i = nPreviousSelected + 1; i < (int)m_listItems.size(); ++i )
			{
				ListCtrlItem *pItem = m_listItems.at( i );
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

	void UIListCtrl::SelectItem( int nNewIndex )
	{CPUTime(UIListCtrl);
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

	void	UIListCtrl::SetEnabled(char bEnabled)
	{CPUTime(UIListCtrl);
		UIControl::SetEnabled( bEnabled );
		this->m_scrollBar.SetEnabled( bEnabled );
	}

	char UIListCtrl::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex )
	{CPUTime(UIListCtrl);
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
			//
			if(m_columns.size() > 0 && m_nColumnHeight > 0 && this->m_bSortEnable)
			{
				Rect rc = m_rcText;
				rc.left	 += m_nBeginLeft;
				rc.right = m_rcText.right;
				Rect rcText	=	rc;
				rcText.bottom	=	rcText.top + m_nColumnHeight;
				for(int c=0;c<m_columns.size();c++)
				{
					rcText.right	=	rcText.left + m_columns[c]->width + 1;
					//
					rcText.right	=	getmin(rcText.right, rc.right);
					if(rcText.left >= rc.right)
						break;
					//
					if(rcText.isPtIn( pt.x, pt.y ))
					{
						this->SortByColumn(c, 0);
						return true;
					}
					//
					rcText.left		+=	m_columns[c]->width;
				}
			}
			if( m_listItems.size() > 0 && m_rcSelection.isPtIn( pt.x, pt.y ) )
			{
				int nClicked;
				if( m_nTextHeight )
					nClicked = m_scrollBar.GetScrollPos() + ( pt.y - m_rcText.top - GetColumnHeight() ) / m_nTextHeight;
				else
					nClicked = -1;
				if( nClicked >= m_scrollBar.GetScrollPos() &&
					nClicked < (int)m_listItems.size() &&
					nClicked < m_scrollBar.GetScrollPos() + m_scrollBar.GetPageSize() )
				{
					m_bDrag = true;
					if( uMsg == InputEventID_ButtonDblClk )
					{
						m_dialogPtr->RaiseEvent(this, EventID_DoubleClick, pt);
						return true;
					}
					m_nSelected = nClicked;
					m_nSelStart = m_nSelected;
					//m_dialogPtr->RaiseEvent(this, (EventID_Changed));
					return true;
				}
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
					if( m_nSelStart != m_nSelected )
						m_dialogPtr->RaiseEvent(this, EventID_Changed, pt);
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
				if( m_bDrag )
				{
					int nItem;
					if( m_nTextHeight )
						nItem = m_scrollBar.GetScrollPos() + ( pt.y - m_rcText.top - GetColumnHeight() ) / m_nTextHeight;
					else
						nItem = -1;
					if( nItem >= (int)m_scrollBar.GetScrollPos() &&
						nItem < (int)m_listItems.size() &&
						nItem < m_scrollBar.GetScrollPos() + m_scrollBar.GetPageSize() )
					{
						m_nSelected = nItem;
						//m_dialogPtr->RaiseEvent(this, (EventID_Changed));
					}
					else
						if( nItem < (int)m_scrollBar.GetScrollPos() )
						{
							m_scrollBar.SetScroll( -1 );
							m_nSelected = m_scrollBar.GetScrollPos();
							//m_dialogPtr->RaiseEvent(this, (EventID_Changed));
						}
						else
							if( nItem >= m_scrollBar.GetScrollPos() + m_scrollBar.GetPageSize() )
							{
								m_scrollBar.SetScroll( 1 );
								m_nSelected = getmin( (int)m_listItems.size(), m_scrollBar.GetScrollPos() + m_scrollBar.GetPageSize() ) - 1;
								//m_dialogPtr->RaiseEvent(this, (EventID_Changed));
							}
				}
				return isPtIn( pt );
			}
		}
		return false;
	}

	void UIListCtrl::Render( float fElapsedTime )
	{CPUTime(UIListCtrl);
		if( m_bVisible == false )
			return;
		UIElement* pElement = &m_lcElement[0];//m_elements.at( 0 );
		pElement->dwTextFormat		=	this->m_textFormat;
		m_dialogPtr->DrawElement(GetEnabled(), pElement, &pElement->rcTexture, &m_currentAnim.rc, pElement->textureColor * m_currentAnim.textureColor, &m_currentAnim, this);
		UIElement* pSelElement = &m_lcElement[1];//m_elements.at( 1 );
		pSelElement->dwTextFormat		=	this->m_textFormat;
		pSelElement->fontIndex			=	pElement->fontIndex;
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
			if(m_columns.size() > 0 && m_nColumnHeight > 0)
			{
				UIElement* columnElement	=	&m_lcElement[2];//m_elements[2];
				columnElement->dwTextFormat	=	this->m_textFormat;
				Rect rcText	=	rc;
				rcText.bottom	=	rcText.top + m_nColumnHeight;
				for(int c=0;c<m_columns.size();c++)
				{
					rcText.right	=	rcText.left + m_columns[c]->width + 1;
					//
					rcText.right	=	getmin(rcText.right, rc.right);
					if(rcText.left >= rc.right)
						break;
					if(columnElement->GetTextureNewID() >= 0)
						m_dialogPtr->DrawElement(GetEnabled(), columnElement, &columnElement->rcTexture, &rcText, columnElement->textureColor * m_currentAnim.textureColor, &m_currentAnim, this);
					//m_dialogPtr->DrawElementText(GetEnabled(), m_currentAnim.rc, m_columns[c]->itemText.str(), columnElement, &rcText, columnElement->fontColor * m_currentAnim.textColor, m_nShadowWidth, m_shadowColor, true, &m_currentAnim);
					if(this->m_bSortEnable)
					{
						if(this->m_sortColumn == c)
						{
							UIElement* sortElement	=	&m_lcElement[3];//this->m_elements[3];
							if(!m_sortUpdown)
								sortElement		=	&m_lcElement[4];//this->m_elements[4];
							if(sortElement->GetTextureNewID() >= 0)
							{
								Rect rcSort	=	rcText;
								rcSort.left	=	rcText.right - RectHeight(rcText);
								rcSort.inflate(-3, -3);
								m_dialogPtr->DrawElement(GetEnabled(), sortElement, &sortElement->rcTexture, &rcSort, sortElement->textureColor * m_currentAnim.textureColor, &m_currentAnim);
							}
						}
					}
					//
					rcText.left		+=	m_columns[c]->width;
				}
				rc.Offset(0, m_nColumnHeight );
			}
			if(m_listItems.size() > 0)
			{
				for( int i = m_scrollBar.GetScrollPos(); i < (int)m_listItems.size(); ++i )
				{
					if( rc.bottom > m_rcText.bottom )
						break;
					ListCtrlItem *pItem = m_listItems.at( i );
					char bSelectedStyle = false;
					if( !m_bMultiselect && i == m_nSelected )
						bSelectedStyle = true;
					else if(m_bMultiselect)
					{
						if( m_bDrag &&
							( ( i >= m_nSelected && i < m_nSelStart ) ||
							( i <= m_nSelected && i > m_nSelStart ) ) )
							bSelectedStyle = m_listItems[m_nSelStart]->bSelected;
						else
							if( pItem->bSelected )
								bSelectedStyle = true;
					}
					UIElement* pTextElement	=	pElement;
					if( bSelectedStyle )
					{
						rcSel.top = rc.top; rcSel.bottom = rcSel.top + m_nTextHeight;
						m_dialogPtr->DrawElement(GetEnabled(), pSelElement, &pSelElement->rcTexture, &rcSel, pSelElement->textureColor * m_currentAnim.textureColor, &m_currentAnim);
						pTextElement	=	pSelElement;
					}
					int beginRc	=	0;
					Rect rcText	=	rc;
					for(int c=0;c<m_columns.size();c++)
					{
						rcText.right	=	rcText.left + m_columns[c]->width;
						//
						rcText.right	=	getmin(rcText.right, rc.right);
						if(rcText.left >= rc.right)
							break;
						//m_dialogPtr->DrawElementText(GetEnabled(), m_currentAnim.rc, pItem->texts[c]->str(), pTextElement, &rcText, pTextElement->fontColor * m_currentAnim.textColor, m_nShadowWidth, m_shadowColor, true, &m_currentAnim);
						rcText.left		+=	m_columns[c]->width;
					}
					pItem->rcActive			=	rc;
					pItem->rcActive.bottom	=	rc.top + m_nTextHeight;
					rc.Offset( 0, m_nTextHeight );
				}
			}
		}
		if(this->m_scrollBarWidth > 0)
			m_scrollBar.RenderScroll( this, fElapsedTime );
		this->RenderDrawMgrs();
	}

	INT	UIListCtrl::GetSelectedData( int nPreviousSelected )
	{CPUTime(UIListCtrl);
		ListCtrlItem* pItem = GetItem( GetSelectedIndex( nPreviousSelected ) );
		if(pItem)
			return pItem->pData;
		return 0;
	}

	void		UIListCtrl::SetItemText( int nIndex, int nColumnIndex, const char *wszText )	//设置指定列的文本
	{CPUTime(UIListCtrl);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return;
		ListCtrlItem *pItem = m_listItems.at( nIndex );
		if(nColumnIndex < 0 || nColumnIndex >= m_columns.size())
			return;
		pItem->texts[nColumnIndex]->operator=(wszText);
	}

	const char*	UIListCtrl::GetItemText( int nIndex, int nColumnIndex )	//返回指定列的文本
	{CPUTime(UIListCtrl);
		if( nIndex < 0 || nIndex >= (int)m_listItems.size() )
			return "";
		ListCtrlItem *pItem = m_listItems.at( nIndex );
		if(nColumnIndex < 0 || nColumnIndex >= m_columns.size())
			return "";
		return pItem->texts[nColumnIndex]->str();
	}

	int			g_sortColumn	=	0;
	char		g_sortUpdown	=	false;
	int			OnListCtrlSort(const void* p1, const void* p2)
	{
		ListCtrlItem* v1	=	*(ListCtrlItem**)p1;
		ListCtrlItem* v2	=	*(ListCtrlItem**)p2;
		const char* sz1	=	v1->texts[g_sortColumn]->str();
		const char* sz2	=	v2->texts[g_sortColumn]->str();
		int ret	=	stricmp(sz1, sz2);
		if(g_sortUpdown)
		{
			if(ret == -1)
				ret = 1;
			else if(ret == 1)
				ret = -1;
		}
		return ret;
	}

	void		UIListCtrl::SortByColumn(int columnIndex, OnSortListCtrl3D cb)	//根据
	{CPUTime(UIListCtrl);
		if(!m_bSortEnable)
			return;
		if(columnIndex < 0 || columnIndex >= m_columns.size())
			return;
		this->m_sortColumn	=	columnIndex;
		this->m_sortUpdown	=	!m_sortUpdown;
		//
		if(this->m_listItems.size() > 0)
		{
			g_sortColumn	=	m_sortColumn;
			g_sortUpdown	=	m_sortUpdown;
			qsort(&m_listItems.at(0), m_listItems.size(), sizeof(ListCtrlItem*), OnListCtrlSort);
		}
	}

	int			UIListCtrl::InsertColumn(const char* szColumn, int nColumnWidth, INT nInsert)	//-1表示从最后一个插入
	{CPUTime(UIListCtrl);
		ListColumnItem* c	=	new ListColumnItem;
		c->width	=	nColumnWidth;
		c->itemText	=	szColumn;
		if(nInsert < 0 || nInsert >= m_columns.size())
		{
			nInsert	=	(int)m_columns.size();
			m_columns.push_back( c );
			for(int i=0;i<this->m_listItems.size();i++)
				m_listItems[i]->texts.push_back(new Text());
		}
		else
		{
			m_columns.insert(m_columns.begin() + nInsert, c );
			for(int i=0;i<this->m_listItems.size();i++)
				m_listItems[i]->texts.insert(m_listItems[i]->texts.begin() + nInsert, new Text());
		}
		return nInsert;
	}

	void		UIListCtrl::RemoveColumn(int index)
	{CPUTime(UIListCtrl);
		if(index < 0 || index >= m_columns.size())
			return;
		delete m_columns[index];
		m_columns.erase(m_columns.begin() + index);
		//
		for(int i=0;i<m_listItems.size();i++)
		{
			delete m_listItems[i]->texts[index];
			m_listItems[i]->texts.erase(m_listItems[i]->texts.begin() + index);
		}
	}

	int			UIListCtrl::GetColumnCount()	//返回列数量
	{CPUTime(UIListCtrl);
		return m_columns.size();
	}

	void		UIListCtrl::RemoveAllColumn()	//删除所有列
	{CPUTime(UIListCtrl);
		for(int i=0;i<m_columns.size();i++)
			delete m_columns[i];
		this->m_listItems.clear();
		m_columns.clear();
	}

	int			UIListCtrl::GetColumnWidth(int index)
	{CPUTime(UIListCtrl);
		if(index < 0 || index >= m_columns.size())
			return 0;
		return m_columns[index]->width;
	}

	void		UIListCtrl::SetColumnWidth(int index, int nColumnWidth)	//设置一个列的宽度
	{CPUTime(UIListCtrl);
		if(index < 0 || index >= m_columns.size())
			return;
		m_columns[index]->width	=	nColumnWidth;
	}

	const char*	UIListCtrl::GetColumnText(int index)
	{CPUTime(UIListCtrl);
		if(index < 0 || index >= m_columns.size())
			return "";
		return m_columns[index]->itemText.str();
	}

	void			UIListCtrl::SetColumnText(int index, const char* szText)
	{CPUTime(UIListCtrl);
		if(index < 0 || index >= m_columns.size())
			return;
		m_columns[index]->itemText.operator=(szText);
	}

};
