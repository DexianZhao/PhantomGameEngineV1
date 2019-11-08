//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UILISTCTRL_H__
#define	__PHANTOM_UILISTCTRL_H__

#include "PhantomUIScrollBar.h"
namespace Phantom{
	
struct	ListCtrlItem
{
	std::vector<Text*>	texts;//行列文本
	INT				pData;
	Rect  			rcActive;
	char  			bVisible;
	char  			bSelected;
	ListCtrlItem(){}
	~ListCtrlItem()
	{
		for(int i=0;i<texts.size();i++)
			delete texts[i];
	}
};

struct	ListColumnItem
{
	int					width;
	Text				itemText;
};

typedef	int	(* OnSortListCtrl3D)(const char* t1, const char* t2);	//排序回调

class UIListCtrl : public UIControl
{
public:
	UIListCtrl( BOOL bInit, UIDialog *dialogPtr = NULL );
	virtual ~UIListCtrl();
	virtual char    		canHaveFocus() { return (m_bVisible && IsEnabledCtrl()); }
	virtual char    		onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex );
	virtual void    		Render( float fElapsedTime );
	virtual void    		recalcRects();
	INT						GetSelectedData( int nPreviousSelected = -1 );
	int						GetItemCount() const { return m_listItems.size(); }
	void 					SetMultiSelect(BOOL b){m_bMultiselect = b;}
	int  					GetScrollBarWidth() const { return m_scrollBarWidth; }
	void 					SetScrollBarWidth( int nWidth ) { m_scrollBarWidth = nWidth; recalcRects(); this->RebuildRenderRect();}
	int						AddItem( const char *wszText, INT pData );
	int						InsertItem( int nIndex, const char *wszText, INT pData );
	void 					RemoveItem( int nIndex );
	void 					RemoveItemByText( char *wszText );
	void 					RemoveItemByData( INT pData );
	void 					RemoveAllItems();
	VOID					OnAnimationRect();
	void					SetLineHeight(int nHeight){m_nTextHeight = nHeight;}			//行高度
	int						GetLineHeight(){return this->m_nTextHeight;}
	void					SetLineTextFirst(int nBeginX){m_nBeginLeft = nBeginX;}			//文字开始显示点
	int						GetLineTextFirst(){return m_nBeginLeft;}
	void					SetEnabled(char bEnabled);
	unsigned int			GetNumItems() { return m_listItems.size(); }
	INT						GetItemData(unsigned int index){ListCtrlItem* item = GetItem(index); if(item)return item->pData; return 0;}
	VOID					SetItemData( int nIndex, INT d ){ListCtrlItem* item = GetItem(nIndex); if(item)item->pData = d;}
	ListCtrlItem *			GetItem( int nIndex );
	int						GetSelectedIndex( int nPreviousSelected = -1 );
	ListCtrlItem *			GetSelectedItem( int nPreviousSelected = -1 ) { return GetItem( GetSelectedIndex( nPreviousSelected ) ); }
	void					SelectItem( int nNewIndex );
	virtual	char			LoadControl(CSafeFileHelperR& r);
	virtual	char			SaveControl(CSafeFileHelperW& w);
	int						GetElementCount();
	UIElement*				GetElementPtr(int index);

	UIScrollBar				m_scrollBar;
	UIScrollBar*			GetScrollBar(){return &m_scrollBar;}

	void					SetItemText( int nIndex, int nColumnIndex, const char *wszText );	//设置指定列的文本
	const char*				GetItemText( int nIndex, int nColumnIndex );	//返回指定列的文本

	int						InsertColumn(const char* szColumn, int nColumnWidth, INT nInsert = -1);	//-1表示从最后一个插入
	void					RemoveColumn(int index);
	int						GetColumnCount();	//返回列数量
	void					RemoveAllColumn();	//删除所有列
	void					SetColumnWidth(int index, int nColumnWidth);	//设置一个列的宽度
	int						GetColumnWidth(int index);
	int						GetColumnHeight(){return m_nColumnHeight;}
	VOID					SetColumnHeight(INT n){m_nColumnHeight = n;}
	const char*				GetColumnText(int index);
	void					SetColumnText(int index, const char* szText);
	void					SortByColumn(int columnIndex, OnSortListCtrl3D cb = 0);	//根据
	VOID					SetSortEnable(BOOL b){m_bSortEnable = b;}
	BOOL					IsSortEnable(){return m_bSortEnable;}
	static const int		MaxListCtrlElement = 5;
	UIElement				m_lcElement[MaxListCtrlElement];
	unsigned int 			GetType() const { return UIControlType_LISTCTRL3D; }
protected:
	Rect					m_rcText;
	Rect					m_rcSelection;
	int						m_nColumnHeight;
	int 					m_borderWidth;
	int 					m_marginWidth;
	int 					m_nTextHeight;
	int						m_nBeginLeft;
	BOOL					m_bMultiselect;
	int 					m_nSelected;
	int 					m_nSelStart;
	char					m_bDrag;
	//
	BOOL					m_bSortEnable;	//是否支持排序
	int						m_sortColumn;	//
	char					m_sortUpdown;	//
	//
	std::vector< ListColumnItem* > m_columns;
	std::vector< ListCtrlItem* > m_listItems;
};

};

#endif
