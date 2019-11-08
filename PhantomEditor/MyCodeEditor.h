#pragma once
#include <vector>
#include <string>

#define	MAX_UNDOREDO_BUFFER		100

//#define	USE_EDITBOX

#ifdef USE_EDITBOX
#define	CMyEdit CEdit
#else
#define	CMyEdit CRichEditCtrl
#endif

class CMyCodeEditor : public CMyEdit
{
	DECLARE_DYNAMIC(CMyCodeEditor)

public:
	CMyCodeEditor();
	virtual ~CMyCodeEditor();

	VOID							Setup(CWnd* wnd, const CRect& rc, CFont* font);
public:
	//
	struct MaskText
	{
		wchar_t		name[32];
		enum TextColorType
		{
			MaskType_None	=	0,
			MaskType_Keywords,		//关键字
			MaskType_MyKeywords,	//关键字，自定义
			MaskType_Token,	//符号
			MaskType_Variant,
			MaskType_Method,
			MaskType_Text,
			MaskType_Description,
			MaskType_Enum,
			MaskType_EnumVar,
			MaskType_Count,
		};
		int length;
		TextColorType	type;
		MaskText(const wchar_t* _name = 0, TextColorType t = MaskType_None)
		{
			if(_name)
				wcscpy(name, _name);
			else
				name[0] = 0;
			length	=	wcslen(name);
			type	=	t;
		}
	};
	struct	TextColor
	{
		COLORREF	c;
		BOOL		bBold;
		TextColor(COLORREF cc = 0, BOOL b = false){c = cc; bBold = b;}
		VOID	Setup(COLORREF cc, BOOL b){c = cc; bBold = b;}
	};
	struct	LineTextSection
	{
		wchar_t			shortText[32];
		std::wstring	longText;
		int				textLength;
		VOID			SetText(const wchar_t* szText)
		{
			textLength	=	wcslen(szText);
			if(textLength < 32)
				wcscpy(shortText, szText);
			else
				longText	=	szText;
		}
		const wchar_t*	GetText()
		{
			if(textLength < 32)
				return shortText;
			return longText.c_str();
		}
		//
		long			lineBeginIndex;
		//
		TextColor*		maskColor;
	};
	struct	LineText
	{
		std::vector<LineTextSection>	sections;
		INT								beginIndex;
		INT								leftSpace;
		std::wstring					lineText;
		BOOL							bUsed;
		//
		VOID							AddToSection(const wchar_t* szText, int begin, int count, int charBegin, TextColor* maskColor);
		LineText()
		{
			bUsed		=	false;
			beginIndex	=	0;
			leftSpace	=	0;
		}
	};
	BOOL					m_bPaintEnable;
	//
	CHARFORMAT				m_tempFormat;
	TextColor				m_maskColor[MaskText::MaskType_Count];
	std::vector<MaskText>	m_maskTexts;
	VOID					FormatAll();
	//
	struct	CodeSection
	{
		std::vector<LineTextSection*>	names;
		CRect				rcName;		//名字的大小
		CRect				rcChild;	//括号内的大小
		std::vector<CodeSection*>		childs;	//括号内部的函数
		int					sectionBegin;
		int					sectionEnd;
		BOOL				bIsMethod;
		CodeSection()
		{
			sectionBegin	=	0;
			sectionEnd		=	0;
			bIsMethod		=	0;
		}
		~CodeSection()
		{
			for(int i=0;i<childs.size();i++)
				delete childs[i];
		}
	};
	//自动生成连接代码
	VOID					BuildCodeSection(CodeSection& s, int line);
	//
	CodeSection*			GetCodeByPos(CodeSection& base, const CPoint& pos, BOOL bMethodOnly = true);
	//
	VOID					RefindAndFormatAll(BOOL bFormatCode = FALSE);
	//
	LineText*				GetLineText(INT nLine);	//查找一行文本
	//
	VOID					SetCodeFormatLines(int nAutoAdd = 1);
	//
	VOID					SetTextColor(long begin, long count, const TextColor& c);
	//
	virtual	TextColor*		GetMaskMethodVar(const wchar_t* szSection, int nLength){return 0;}
	virtual	VOID			OnBuildAll();
	virtual	VOID			OnSaveAll(){}
	virtual	VOID			ExecuteCode(){}//执行脚本
	//
	const wchar_t*			Token(const wchar_t* szName);
	//
	virtual	VOID			PushUndo(){}
	virtual	VOID			UndoOne(){}
	VOID					GetSel(long&, long&);
	//struct	UndoBuffer
	//{
	//	UndoBuffer(){t = 0; s = e = 0;}
	//	VOID		Clear()
	//	{
	//		if(t)
	//			delete t;
	//		t	=	0;
	//	}
	//	wchar_t*	t;
	//	long		s,e;
	//};
	//std::vector<UndoBuffer>	m_undoBuffers;
protected:
	LineText*				AddNewLine();
	std::vector<LineText*>	m_lineList;
	//
	std::vector<LineText*>	m_lineSections;
	std::vector<int>		m_refreshLines;
	//
	VOID					SetLineFormat(INT nLine);
	VOID					InsertLine(INT nIndex);
	//
	int						BuildCodeSection2(LineText* lt, CodeSection& s, int line, int lineHeight);
	//
	BOOL					CreateLineFormat(LineText* lt, int nLine);	//创建段落
	//
	VOID					AddToSection(std::vector<std::wstring>& sections, const wchar_t* szText, int begin, int count);
	VOID					SetBeginLineSpace(INT nLine, INT nSpace);
	//
	int						m_nMaxLineCaretPos;
	int						m_returnStartLine;
	CString					m_strFontFace;
	//
	VOID					Redraw();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg	void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//
};

BOOL	IsSectionSymbol(wchar_t c);
