//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UICOMBOBOX_H__
#define	__PHANTOM_UICOMBOBOX_H__

#include "PhantomUIButton.h"
#include "PhantomUIScrollBar.h"

namespace Phantom{

	class UIComboBox : public UIButton
	{
	public:
		UIComboBox( BOOL bInit, UIDialog *dialogPtr = NULL );
		virtual ~UIComboBox();

		virtual void		SetTextColor( unsigned int Color );
		virtual char 		onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex );
		virtual char 		canHaveFocus() { return (m_bVisible && IsEnabledCtrl()); }
		virtual void 		onFocusOut();
		virtual void 		Render( float fElapsedTime );

		virtual void 		recalcRects();

		int					AddItem( const char* strText, INT pData );
		void    			RemoveAllItems();
		void    			RemoveItem( unsigned int index );
		char    			ContainsItem( const char* strText, unsigned int iStart=0 );
		int     			FindItem( const char* strText, unsigned int iStart=0 );
		INT		  			GetItemData( const char* strText );
		INT		 			GetItemData( int nIndex );
		VOID				SetItemData( int nIndex, INT d );
		void    			SetDropHeight( int nHeight ) { m_nDropHeight = getmax(nHeight, 0); recalcRects(); this->RebuildRenderRect();}
		int     			GetScrollBarWidth() const { return m_scrollBarWidth; }
		void    			SetScrollBarWidth( int nWidth ) { m_scrollBarWidth = nWidth; recalcRects(); this->RebuildRenderRect();}
		int					GetDropHeight(){return m_nDropHeight;}

		void				SetEnabled(char bEnabled);
		INT		   			GetSelectedData();
		INT					GetSelectIndex(){return this->m_selected;}

		VOID				OnAnimationRect();

		unsigned int		GetNumItems() { return m_listItems.size(); }
		const char*			GetItemText(unsigned int index){ListBoxItem* item = GetItem(index); if(item)return item->itemText.str(); return "";}

		char				SetSelectedByIndex( unsigned int index );
		char				SetSelectedByText( const char* strText );
		char				SetSelectedByData( INT pData );
		void				SetItemText( int nIndex, const char *wszText );

		ListBoxItem* 			GetSelectedItem();
		ListBoxItem* 			GetItem( unsigned int index ) { return m_listItems.at( index ); }

		virtual	char		LoadControl(CSafeFileHelperR& r);
		virtual	char		SaveControl(CSafeFileHelperW& w);
		int					GetElementCount();
		UIElement*			GetElementPtr(int index);

		unsigned int 			GetType() const { return UIControlType_COMBOBOX; }
		UIScrollBar*		GetScrollBar(){return &m_scrollBar;}
		UIScrollBar		m_scrollBar;
		//
		VOID					SetItemColor(unsigned int index, const color4& c);
		const color4*			GetItemColor(unsigned int index);
		VOID					SetItemHighColor(unsigned int index, const color4& c);
		const color4*			GetItemHighColor(unsigned int index);
	protected:
		int     			m_selected;
		int     			m_focuses;
		int     			m_nDropHeight;
		char   				m_bOpened;

		Rect 				m_rcText;
		Rect 				m_rcButton;
		Rect 				m_rcDropdown;
		Rect 				m_rcDropdownText;
		std::vector< ListBoxItem* > m_listItems;
	};
};

#endif
