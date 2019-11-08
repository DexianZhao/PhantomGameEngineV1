//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UILISTBOX_H__
#define	__PHANTOM_UILISTBOX_H__

#include "PhantomUIScrollBar.h"
namespace Phantom{

class UIListBox : public UIControl
{
public:
	UIListBox( BOOL bInit, UIDialog *dialogPtr = NULL );
	virtual ~UIListBox();
	virtual char    		canHaveFocus() { return (m_bVisible && IsEnabledCtrl()); }
	virtual char    		onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex );
	virtual void    		Render( float fElapsedTime );
	virtual void    		recalcRects();
	INT						GetSelectedData( int nPreviousSelected = -1 );
	VOID					OnAnimationRect();
	//
	int						GetItemCount() const { return m_listItems.size(); }
	void 					SetMultiSelect(BOOL b){m_bMultiselect = b;}
	int  					GetScrollBarWidth() const { return m_scrollBarWidth; }
	void 					SetScrollBarWidth( int nWidth ) { m_scrollBarWidth = nWidth; recalcRects(); this->RebuildRenderRect();}
	int						AddItem( const char *wszText, INT pData );
	int						InsertItem( int nIndex, const char *wszText, INT pData );
	void 					RemoveItem( int nIndex );
	void 					RemoveItemByText( char *wszText );
	void 					RemoveItemByData( void *pData );
	void 					RemoveAllItems();

	VOID					SetItemData( int nIndex, INT d ){ListBoxItem* item = GetItem(nIndex); if(item)item->pData = d;}

	void					SetLineHeight(int nHeight){m_nTextHeight = nHeight;}			//行高度
	int						GetLineHeight(){return this->m_nTextHeight;}
	void					SetLineTextFirst(int nBeginX){m_nBeginLeft = nBeginX;}			//文字开始显示点
	int						GetLineTextFirst(){return m_nBeginLeft;}
	void					SetEnabled(char bEnabled);
	unsigned int			GetNumItems() { return m_listItems.size(); }
	const char*				GetItemText(unsigned int index){ListBoxItem* item = GetItem(index); if(item)return item->itemText.str(); return "";}
	INT						GetItemData(unsigned int index){ListBoxItem* item = GetItem(index); if(item)return item->pData; return 0;}
	void					SetItemText( int nIndex, const char *wszText );	//插入一行

	ListBoxItem *			GetItem( int nIndex );
	int						GetSelectedIndex( int nPreviousSelected = -1 );
	ListBoxItem *			GetSelectedItem( int nPreviousSelected = -1 ) { return GetItem( GetSelectedIndex( nPreviousSelected ) ); }
	void					SelectItem( int nNewIndex );

	virtual	char			LoadControl(CSafeFileHelperR& r);
	virtual	char			SaveControl(CSafeFileHelperW& w);

	int						GetElementCount();
	UIElement*			GetElementPtr(int index);

	UIScrollBar*		GetScrollBar(){return &m_scrollBar;}
	UIScrollBar		m_scrollBar;
	static const int		MaxListBoxElement = 3;
	UIElement		m_listElement[MaxListBoxElement];
	unsigned int 			GetType() const { return UIControlType_LISTBOX; }
	//
	VOID					SetItemColor(unsigned int index, const color4& c);
	const color4*			GetItemColor(unsigned int index);
	VOID					SetItemHighColor(unsigned int index, const color4& c);
	const color4*			GetItemHighColor(unsigned int index);
protected:
	Rect				m_rcText;
	Rect				m_rcSelection;
	int 				m_borderWidth;
	int 				m_marginWidth;
	int 				m_nTextHeight;
	int					m_nBeginLeft;
	BOOL				m_bMultiselect;
	int 				m_nSelected;
	int 				m_nSelStart;
	char				m_bDrag;

	std::vector< ListBoxItem* > m_listItems;
};


};

#endif
