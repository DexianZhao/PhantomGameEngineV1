//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UITEXTFORMAT_H__
#define	__PHANTOM_UITEXTFORMAT_H__

#include "PhantomUIControl.h"
namespace Phantom{


	class UIControl;
	class UIDialog;

	struct TextFormatData
	{
		BOOL			bUsed;
		DWORD			dwColor;
		INT				nShadowWidth;
		DWORD			dwShadowColor;
		INT				nFont;
		INT				nEvent;
		INT				nImageID;	//图片大小为20x20
	};

	struct LineTextDraw
	{
		Rect			rc;
		int				nBegin;
		int				nCount;
		int				lineIndex;
		unsigned short	formatIndex;
	};

#define	RESERVED_BUFFER	256

	struct UndoRedoData
	{
		enum{
			TYPE_DELETE_TEXT	=	0,
			TYPE_INSERT_TEXT	=	1,
			TYPE_SET_FORMAT		=	2,
		};
		int				type;
		short_t			d_buffer[RESERVED_BUFFER];
		short_t*		buffer;
		unsigned short	d_format[RESERVED_BUFFER];
		unsigned short*	format;
		int				bufferLength;
		int				begin, count;
		int				selBegin, selEnd;
		UndoRedoData(){memset(this, 0, sizeof(UndoRedoData));}
		~UndoRedoData(){if(buffer)free(buffer);if(format)free(format);}
	};

	struct FormatEventText
	{
		INT							nEvent;
		Name						cursor;
		TextW						text;
		int							parameter;
	};

	struct FormatData
	{
		unsigned short			begin;
		unsigned short			end;
		unsigned int			color;
		char					shadowWidth;
		unsigned char			fontHeight;
		enum{
			fontType_Underline	=	0x01,
			fontType_Bold		=	0x02,
		};
		unsigned char			fontState;	//
		unsigned short			fontNameIndex;	//在索引表中的字体Index
		unsigned int			imageID;	//在文件中的ID
		char					imageType;
	};

	class	TextFormat;

	class	ITextFormatCallback
	{
	public:
		virtual	VOID				OnEventClick(TextFormat* ptr, TextW& eventText, int parameter)	=	0;
		virtual	VOID				OnEventMoving(TextFormat* ptr, TextW& eventText, int parameter)	=	0;
	};

	class TextFormat
	{
	public:
		TextFormat(void);
		~TextFormat(void);
		INT							Insert(int nBegin, const short_t* str, int nCount = -1, BOOL bAddToUndo = TRUE, BOOL bAddToRedo = FALSE);
		INT							Remove(int nBegin, int nCount, BOOL bAddToUndo = TRUE, BOOL bAddToRedo = FALSE);
		//
		INT							GetFormatData(FormatData* buffer, INT nRange);		//取得格式属性
		VOID						SetFormatData(const FormatData* buffer, INT nRange);	//设置格式属性
		//
		VOID						InsertText(const short_t* str, int nCount);
		VOID						InsertFormatText(const short_t* str, int nCount, DWORD dwColor, INT nFontIndex, INT nImageID, INT nShadowWidth = 0, DWORD shadowColor = 0);
		VOID						RemoveLine(int nFirstLine, int nRemoveCount);	//删除几行
		VOID						RemoveText(int nBegin, int nCount);
		VOID						RemoveAll();
		INT							GetVisibleLineCount(){return m_nShowLineCount;}
		VOID						SetVisibleLineCount(INT nCount){m_nShowLineCount = nCount;}
		INT							GetLineCount(){return this->m_maxLineCount;}
		//
		VOID						SetEventColor(unsigned int color){this->m_nEventColor = color;}	//设置事件的背景色
		VOID						SetSelectColor(unsigned int color){this->m_nSelectColor = color;}	//设置选中项的背景色
		unsigned int				GetEventColor(){return m_nEventColor;}
		unsigned int				GetSelectColor(){return m_nSelectColor;}
		//
		//设置事件文本
		INT							SetEventText(INT nEvent, const short_t* text, int parameter);
		FormatEventText*			GetEventText(INT nEvent);
		VOID						CleanEvent();
		//
		VOID						SetTextFormat(int nBegin, int nCount, DWORD dwColor, INT nFontIndex, INT nImageID, INT nShadowWidth = 0, DWORD shadowColor = 0){SetFormat(nBegin, nCount, dwColor, nFontIndex, nImageID, nShadowWidth, shadowColor, 0);}
		enum{
			FORMAT_TYPE_COLOR		=	0x00000001,
			FORMAT_TYPE_FONT		=	0x00000002,
			FORMAT_TYPE_IMAGE		=	0x00000004,
			FORMAT_TYPE_EVENT		=	0x00000008,
			FORMAT_TYPE_SHADOW		=	0x00000010,
			//
			FORMAT_TYPE_ALL			=	0xffffffff,
		};
		VOID						SetFormat(int nBegin, int nCount, DWORD dwColor, INT nFontIndex, INT nImageID, INT nShadowWidth = 0, DWORD shadowColor = 0, BOOL bAddToUndo = TRUE, BOOL bAddToRedo = FALSE, INT nEvent = 0);
		//
		const short_t*				GetText(){return m_maxTextBuffer;}
		int							GetTextLength(){return m_textLength;}
		//
		VOID						Render(VOID* lpParam);
		//
		VOID						Setup(UIDialog* dlg){m_pTextRender = dlg;}
		VOID						SetDrawRect(const Rect& rc);
		//
		INT							GetCharFromPoint(Pixel pt);
		Pixel						GetPointFromChar(INT nCharIndex, INT* nHeight = 0);
		//
		VOID						InsertChar(short_t ch);
		//
		VOID						InsertImage(INT nIndex, INT nImage);
		//
		VOID						SetSel(int nIndex, int nCount = 0);
		VOID						SetSelRange(int nBegin, int nEnd);
		VOID						ResetSelect();
		//
		INT							GetSelBegin(){return m_selBegin;}
		INT							GetSelEnd(){return m_selEnd;}
		INT							GetSelRange(){return abs(m_selEnd - m_selBegin);}
		INT							GetFirstSel(){return getmin(m_selBegin, m_selEnd);}
		INT							GetCurSel(){return m_selBegin;}
		//
		INT							GetFirstChar(Pixel pt);
		INT							GetLastChar(Pixel pt);
		//
		VOID						BeginShowLine(INT nIndex);
		int							GetBeginShowLine(){return m_nShowBeginLine;}
		//
		INT							GetCharLineIndex(INT ch);	//字符串出现的行
		INT							GetCharFromLineX(INT nLineIndex, INT pixelX);	//返回pixel位置的第几个字符串
		//
		VOID						Invalidate();
		//
		VOID						SetFormatEvent(INT nBegin, INT nCount, const short_t* szEventText, int parameter);
		VOID						SetFormatColor(INT nBegin, INT nCount, DWORD dwColor);
		VOID						SetFormatFont(INT nBegin, INT nCount, INT nFontID);
		//
		BOOL						OnButtonUp(INT x, INT y);
		BOOL						OnMouseMove(INT x, INT y);
		//
		TextFormatData*				GetClickEvent(INT x, INT y);
		//
		ITextFormatCallback*		GetTextFormatCallback(){return m_pTextFormatCB;}
		//
		INT							GetLineSpacingHeight(){return m_lineSpacingHeight;}
		VOID						SetLineSpacingHeight(INT h){m_lineSpacingHeight = h;}	//设置行间距
		//
		const Rect&					GetTextDrawRegion(){return m_textDrawRegion;}	//返回文本渲染区域大小
		//
		INT							m_nShadowWidth, m_nShadowColor;
		//
		float						m_fTextAlpha;
		//
		const char*					GetTextA();
		//
		VOID						SetTextFormatCB(ITextFormatCallback* pCB){m_pTextFormatCB = pCB;}
	protected:
		VOID						RemoveSelect();
		INT							GetDrawTextLength(const short_t* szText, int numText, int iFont, int nPixelWidth);
		BOOL						GetDrawSize(const short_t* szText, int numText, int iFont, Size& sz);
		//
		unsigned short				AddFormat(DWORD dwColor, INT nFontIndex, INT nImageID, INT nShadowWidth, DWORD shadowColor, INT nEvent, INT nType);
		INT							DrawFormatText(INT nBegin, INT nCount, const Rect& rcRender, Rect& rcLine, unsigned int formatIndex, int& lineIndex, char bNewLine);
		//
		VOID						RemoveRedoBuffer();

		//
		INT							GetTextCount(){return m_textLength;}
		VOID						SetTextColor(INT nIndex, const color4& c);
		const color4*				GetTextColor(INT nIndex);
		VOID						SetTextOffset(INT nIndex, const Pixel& c);
		const Pixel*				GetTextOffset(INT nIndex);
	protected:
		std::vector<FormatEventText>	m_events;
		int							m_lastMoveEventID;
		int							m_eventID;
		//
		struct	TextOne
		{
			unsigned int			color;
			short					offsetX,offsetY;
			Rect					rc;
			char					bRc;
		};
		char						m_bUseTextOne;
		char						m_bRequestDraw;
		std::vector<TextOne>		m_maxTextOffset;
		VOID						EnableTextColor();
		//
		short_t*					m_maxTextBuffer;
		unsigned short*				m_formatBuffer;
		int							m_textLength;
		int							m_textAllocBuffer;
		//
		unsigned int				m_nSelectColor;
		unsigned int				m_nEventColor;
		//
		int							m_formatCount;
		int							m_formatAllocCount;
		TextFormatData*				m_textFormats;
		//
		TextFormatData*				m_pSelectEvent;
		//
		LineTextDraw*				m_lineTextDraw;
		int							m_lineDrawCount;
		//
		int							m_lineSpacingHeight;	//每个行间距
		//
		ITextFormatCallback*		m_pTextFormatCB;
		UIDialog*			m_pTextRender;
		//
		std::vector<UndoRedoData*>	m_undoBuffer;
		std::vector<UndoRedoData*>	m_redoBuffer;
		//
		static FormatData			s_copyFormat[128];
		static int					s_copyFormatCount;
		//
		int							m_selBegin;
		int							m_selEnd;
		int							m_visibleLineCount;
		int							m_visibleBegin;
		int							m_visibleEnd;
		//
		int							m_nShowBeginLine;
		int							m_maxLineCount;
		int							m_nShowLineCount;
		//
		BOOL						m_bInvalidate;
		Rect						m_rectDraw;
		Rect						m_textDrawRegion;
		//
		CDynamicArray<char>			m_bufferA;
		char						m_bChanged;
	};
};

#endif
