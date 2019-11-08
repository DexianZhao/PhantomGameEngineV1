// MyCodeEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "MyCodeEditor.h"
// CMyCodeEditor

wchar_t		g_spaceChar	=	L'\t';
int			g_spaceCharAdd	=	1;
bool IsSpace(wchar_t c)
{
	return (c == L' ' || c == L'\t');
}

wchar_t g_luaSymbol[] = {L' ',L'\t',L'+',L'-',L'*',L'/',L'\"',L'\\',L'*',L'%',L'^',L'#',L'=',L'~',
	L'!',L'<',L'>',L'(',L')',L'{',L'}',L'[',L']',L';',L':',L',',L'.',L'\r',L'\n',L'\'', L'\"', L'\r', L'\n', 0};

BOOL	IsSectionSymbol(wchar_t c)
{
	int len	=	sizeof(g_luaSymbol) / sizeof(wchar_t);
	for(int i=0;i<len;i++)
		if(c == g_luaSymbol[i])
			return true;
	return false;
}

IMPLEMENT_DYNAMIC(CMyCodeEditor, CMyEdit)

CMyCodeEditor::CMyCodeEditor()
{
	memset(&m_tempFormat, 0, sizeof(m_tempFormat));
	m_nMaxLineCaretPos	=	0;
	m_bPaintEnable		=	true;
	m_returnStartLine	=	0;
}

CMyCodeEditor::~CMyCodeEditor()
{
	for(int i=0;i<m_lineList.size();i++)
		delete m_lineList[i];
}

CMyCodeEditor::LineText*	CMyCodeEditor::AddNewLine()
{
	for(int i=0;i<m_lineList.size();i++)
	{
		if(m_lineList[i]->bUsed)
			continue;
		m_lineList[i]->bUsed	=	true;
		return m_lineList[i];
	}
	LineText* t	=	new LineText();
	t->bUsed	=	true;
	m_lineList.push_back(t);
	return t;
}

VOID						CMyCodeEditor::InsertLine(INT nIndex)
{
	if(nIndex >= 0 && nIndex < (int)m_lineSections.size())
	{
		LineText* t = AddNewLine();
		m_lineSections.insert(m_lineSections.begin() + nIndex, t);
	}
}

CMyCodeEditor::LineText*	CMyCodeEditor::GetLineText(INT nLine)	//查找一行文本
{
	int lineCount	=	this->GetLineCount();
	if(lineCount > 60000)
		return 0;
	for(int i=(int)m_lineSections.size();i<lineCount;i++)
	{
		LineText* t = AddNewLine();
		m_lineSections.push_back(t);
	}
	return m_lineSections[nLine];
}

BEGIN_MESSAGE_MAP(CMyCodeEditor, CMyEdit)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


VOID	CMyCodeEditor::SetTextColor(long begin, long count, const TextColor& c)
{
	if(count <= 0)
		return;
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask		=	CFM_STRIKEOUT|CFM_BOLD|CFM_COLOR|CFM_FACE|CFM_SIZE;
	wcscpy(cf.szFaceName, m_strFontFace);//L"Arial");
	cf.dwEffects	=	(c.bBold ? CFE_BOLD : 0);
	cf.yHeight		=	9 * 20;
	cf.crTextColor	=	c.c;
	this->SetSel(begin, begin + count);
#ifdef USE_EDITBOX
#else
	this->SetSelectionCharFormat(cf);
#endif
}


// CMyCodeEditor 消息处理程序


VOID		CMyCodeEditor::Setup(CWnd* wnd, const CRect& rc, CFont* font)
{
	Create(WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_VISIBLE | WS_HSCROLL | ES_AUTOHSCROLL | WS_TABSTOP, rc, wnd, 0);
	this->SetFont(font);
	//
	m_maskTexts.push_back(MaskText(L"and", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"break", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"do", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"else", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"elseif", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"end", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"false", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"for", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"function", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"if", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"in", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"local", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"nil", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"not", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"or", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"repeat", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"return", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"then", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"true", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"until", MaskText::MaskType_Keywords));
	m_maskTexts.push_back(MaskText(L"while", MaskText::MaskType_Keywords));
	////
	//m_maskTexts.push_back(MaskText(L"并且", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"段", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"执行", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"否则", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"或者", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"结束", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"假", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"循环", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"函数", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"如果", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"在", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"本地", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"空", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"否", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"或", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"重复", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"返回", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"则", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"真", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"直到", MaskText::MaskType_MyKeywords));
	//m_maskTexts.push_back(MaskText(L"真循环", MaskText::MaskType_MyKeywords));
	//
	m_maskTexts.push_back(MaskText(L"+", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"-", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"*", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"/", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"%", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"^", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"#", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"==", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"~=", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"<=", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L">=", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"<", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L">", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"=", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"(", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L")", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"{", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"}", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"[", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"]", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L";", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L":", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L",", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"...", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"..", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L".", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"\"", MaskText::MaskType_Token));
	m_maskTexts.push_back(MaskText(L"\'", MaskText::MaskType_Token));
	m_maskColor[MaskText::MaskType_Keywords].Setup(RGB(0, 0, 255), false);
	m_maskColor[MaskText::MaskType_MyKeywords].Setup(RGB(50, 50, 200), false);
	m_maskColor[MaskText::MaskType_Token].Setup(RGB(100, 100, 100), false);
	m_maskColor[MaskText::MaskType_Variant].Setup(RGB(227, 109, 99), false);//RGB(227, 109, 99), false);
	m_maskColor[MaskText::MaskType_Enum].Setup(RGB(150, 80, 25), false);
	m_maskColor[MaskText::MaskType_EnumVar].Setup(RGB(180, 120, 25), false);
	m_maskColor[MaskText::MaskType_Method].Setup(RGB(0, 108, 255), false);
	m_maskColor[MaskText::MaskType_Text].Setup(RGB(123, 0, 0), false);
	m_maskColor[MaskText::MaskType_Description].Setup(RGB(10, 153, 10), false);
	this->LimitText(1024 * 1024);
	m_strFontFace	=	L"Arial";
	//this->SetUndoLimit(0);
}

VOID CMyCodeEditor::RefindAndFormatAll(BOOL bFormatCode)
{
	int tabWidth = 16;
	this->SendMessage(EM_SETTABSTOPS, (WPARAM)1, (LPARAM)&tabWidth);
#ifndef USE_EDITBOX
	UINT nOptionOld	=	this->GetOptions();
	UINT nOption	=	nOptionOld;
	if(nOption & ECO_AUTOVSCROLL)
		nOption ^= ECO_AUTOVSCROLL;
	if(nOption & ECO_AUTOHSCROLL)
		nOption ^= ECO_AUTOHSCROLL;
	this->SetOptions(ECOOP_SET, nOption);
#endif
	m_bPaintEnable	=	false;
	m_refreshLines.clear();
	long start,end;
	GetSel(start, end);
	int endLine		=	LineFromChar(start) + 2;
	int lineCount	=	GetLineCount();
	BOOL bChanged	=	false;
	if(lineCount != m_lineSections.size())
	{
		std::vector<LineText*> temp = m_lineSections;
		for(int i=0;i<temp.size();i++)
			temp[i]->bUsed	=	false;
		//
		m_lineSections.clear();
		wchar_t buffer[2048];
		m_lineSections.resize(lineCount);
		//
		for(int line=0;line<lineCount;line++)
		{
			int len = this->GetLine(line, buffer, 2048);
			buffer[len] = 0;
			LineText lineT;
			this->CreateLineFormat(&lineT, line);

			BOOL bFindSection	=	false;
			for(int j=0;j<temp.size();j++)
			{
				if(wcscmp(buffer, temp[j]->lineText.c_str()) == 0)
				{
					bFindSection	=	true;
					if(lineT.sections.size() != temp[j]->sections.size())
					{
						bFindSection	=	false;
					}
					else
					{
						for(int k=0;k<temp[j]->sections.size();k++)
						{
							if(wcscmp(temp[j]->sections[k].GetText(), lineT.sections[k].GetText()) != 0)
							{
								bFindSection	=	false;
								break;
							}
						}
					}
					if(!bFindSection)
						continue;
					m_lineSections[line] = temp[j];
					m_lineSections[line]->bUsed	=	true;
					if(temp.size() > 0)
						temp.erase(temp.begin() + j);
					else
						temp.clear();
					break;
				}
			}
		}
		for(int line=0;line<lineCount;line++)
		{
			if(m_lineSections[line] == 0)
			{
				m_lineSections[line]	=	AddNewLine();
				this->CreateLineFormat(m_lineSections[line], line);
				m_refreshLines.push_back(line);
				//this->SetLineFormat(line);
				bChanged	=	true;
			}
			else
			{
				m_lineSections[line]->beginIndex	=	LineIndex(line);
				//
				if(line >= m_returnStartLine && line <= endLine)
				{
					m_refreshLines.push_back(line);
					//this->SetLineFormat(line);
					bChanged	=	true;
				}
			}
		}
	}
	else
	{
		for(int line=m_returnStartLine;line<=endLine;line++)
		{
			if(line >= lineCount)
				break;
			this->CreateLineFormat(m_lineSections[line], line);
			m_refreshLines.push_back(line);
			//this->SetLineFormat(line);
			bChanged	=	true;
		}
	}
	for(int line=0;line<lineCount;line++)
	{
		m_lineSections[line]->beginIndex	=	LineIndex(line);
	}
	for(int i=0;i<this->m_lineList.size();i++)
	{
		if(!m_lineList[i]->bUsed)
		{
			m_lineList[i]->sections.clear();
			m_lineList[i]->lineText = L"";
		}
	}
	if(bFormatCode)
		SetCodeFormatLines();
	for(int line=0;line<lineCount;line++)
	{
		m_lineSections[line]->beginIndex	=	LineIndex(line);
	}
	this->OnBuildAll();
	for(int i=0;i<m_refreshLines.size();i++)
	{
		this->SetLineFormat(m_refreshLines[i]);
	}
	m_refreshLines.clear();
#ifndef USE_EDITBOX
	this->SetOptions(ECOOP_SET, nOptionOld);
#endif
	SetSel(start, end);
	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);
	rc.bottom++;
	MoveWindow(&rc);
	rc.bottom--;
	MoveWindow(&rc);
	m_bPaintEnable	=	true;
	if(bChanged)
		Redraw();
}

CMyCodeEditor::CodeSection*	CMyCodeEditor::GetCodeByPos(CMyCodeEditor::CodeSection& base, const CPoint& pos, BOOL bMethodOnly)
{
	CodeSection* ret	=	0;
	BOOL bThis	=	false;
	if(bMethodOnly)
	{
		if(base.bIsMethod)
		{
			//if(base.rcName.PtInRect(pos))
				//ret	=	&base;
			if(base.rcChild.PtInRect(pos))
				ret	=	&base;
		}
	}
	else
	{
		if(base.rcName.PtInRect(pos))
			ret	=	&base;
		if(base.rcChild.PtInRect(pos))
			ret	=	&base;
	}
	for(int i=0;i<base.childs.size();i++)
	{
		CodeSection* r	=	GetCodeByPos(*base.childs[i], pos, bMethodOnly);
		if(r)
			ret	=	r;
	}
	return ret;
}

int CMyCodeEditor::BuildCodeSection2(CMyCodeEditor::LineText* text, CMyCodeEditor::CodeSection& s, int line, int lineHeight)
{
	CMyCodeEditor::CodeSection* tempPtr	=	new CMyCodeEditor::CodeSection();
	s.childs.push_back(tempPtr);
	tempPtr->sectionBegin	=	line;
	tempPtr->sectionEnd		=	line;
	BOOL bLink	=	false;
	const wchar_t* beginWord[] = {L"do", L"if", L"function", L"while", L"for", L"elseif", L"else", L"then"};
	int beginWorldCount	=	sizeof(beginWord) / sizeof(wchar_t*);
	for(;line<text->sections.size();line++)
	{
		const wchar_t* szText	=	text->sections[line].GetText();
		bool bIsKeyword	=	false;
		for(int i=0;i<beginWorldCount;i++)
		{
			if(wcscmp(szText, beginWord[i]) == 0)
			{
				bIsKeyword	=	true;
				break;
			}
		}
		if(szText[0] == L'(')
		{
			if(tempPtr->names.size() > 0)
				tempPtr->bIsMethod	=	true;
			int beginLine	=	line;
			line		=	BuildCodeSection2(text, *tempPtr, line + 1, lineHeight);
			CPoint endPos	=	0;
			BOOL bEnd	=	false;
			if(line >= text->sections.size())
			{
				bEnd	=	true;
				line	=	(int)text->sections.size() - 1;
				endPos	=	PosFromChar(text->beginIndex + (int)text->lineText.size() - 1);
			}
			else
			{
				endPos	=	PosFromChar(text->beginIndex + text->sections[line].lineBeginIndex);
			}
			
			{
				tempPtr->sectionEnd		=	line;
				CPoint pb	=	this->PosFromChar(text->beginIndex + text->sections[tempPtr->sectionBegin].lineBeginIndex);
				CPoint pe	=	this->PosFromChar(text->beginIndex + text->sections[beginLine].lineBeginIndex);
				tempPtr->rcName.SetRect(pb.x, pb.y, pe.x + 1, pb.y + lineHeight);
			}
			{
				CPoint pb	=	this->PosFromChar(text->beginIndex + text->sections[beginLine].lineBeginIndex);
				tempPtr->rcChild.SetRect(pb.x, pb.y, endPos.x + 1, pb.y + lineHeight);
			}
			//if(!bEnd)
			//{
			//	if(tempPtr->names.size() > 0)
			//	{
			//		tempPtr	=	new CMyCodeEditor::CodeSection();
			//		s.childs.push_back(tempPtr);
			//	}
			//	tempPtr->sectionBegin	=	line + 1;
			//	tempPtr->sectionEnd		=	line + 1;
			//}
			bLink					=	false;
		}
		else if(szText[0] == L':' || szText[0] == L'.')
		{
			bLink	=	true;
		}
		else if(IsSectionSymbol(szText[0]) || bIsKeyword)
		{
			if(tempPtr->names.size() > 0)
			{
				tempPtr->sectionEnd		=	line;
				CPoint pb	=	this->PosFromChar(text->beginIndex + text->sections[tempPtr->sectionBegin].lineBeginIndex);
				CPoint pe	=	this->PosFromChar(text->beginIndex + text->sections[tempPtr->sectionEnd].lineBeginIndex);
				tempPtr->rcName.SetRect(pb.x, pb.y, pe.x + 1, pb.y + lineHeight);
				//
				if(szText[0] == L')')
					return line;
				tempPtr	=	new CMyCodeEditor::CodeSection();
				s.childs.push_back(tempPtr);
			}
			if(szText[0] == L')')
				return line;
			tempPtr->sectionBegin	=	line + 1;
			tempPtr->sectionEnd		=	line;
		}
		else
		{
			//if(bLink)
			//{
			//	tempPtr->names.push_back(&text->sections[line]);
			//}
			//else
			{
				CPoint endPos	=	0;
				BOOL bEnd	=	false;
				int lineEnd	=	line + 1;
				if(lineEnd >= text->sections.size())
				{
					bEnd	=	true;
					endPos	=	PosFromChar(text->beginIndex + (int)text->lineText.size() - 1);
				}
				else
					endPos	=	PosFromChar(text->beginIndex + text->sections[lineEnd].lineBeginIndex);
				//
				tempPtr->names.push_back(&text->sections[line]);
				if(!bLink)
					tempPtr->sectionBegin	=	line;
				tempPtr->sectionEnd		=	line;
				CPoint pb	=	this->PosFromChar(text->beginIndex + text->sections[tempPtr->sectionBegin].lineBeginIndex);
				tempPtr->rcName.SetRect(pb.x, pb.y, endPos.x + 1, pb.y + lineHeight);
				if(!bEnd && !bLink)
				{
					const wchar_t* szEnd	=	text->sections[lineEnd].GetText();
					if(IsSectionSymbol(szEnd[0]) || IsSpace(szEnd[0]))
					{
					}
					else
					{
						tempPtr	=	new CMyCodeEditor::CodeSection();
						s.childs.push_back(tempPtr);
						//
						tempPtr->sectionBegin	=	line + 1;
						tempPtr->sectionEnd		=	line + 1;
					}
				}
			}
		}
	}
	return (int)text->sections.size() - 1;
}

VOID CMyCodeEditor::BuildCodeSection(CMyCodeEditor::CodeSection& s, int nLine)
{
	LineText* text	=	this->GetLineText(nLine);
	CPoint p1		=	PosFromChar(LineIndex(nLine));
	CPoint p2		=	PosFromChar(LineIndex(nLine + 1));
	int lineHeight	=	p2.y - p1.y;
	if(lineHeight < 12)
		lineHeight	=	12;
	if(text->sections.size() == 0)
		return;
	BuildCodeSection2(text, s, 0, lineHeight);
}

void CMyCodeEditor::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	__super::OnChar(nChar, nRepCnt, nFlags);
	this->Redraw();
}

void CMyCodeEditor::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CMyEdit::OnKeyUp(nChar, nRepCnt, nFlags);
	//
	BOOL bReturn	=	(nChar == VK_RETURN);
	long s,e;
	GetSel(s,e);
	int line	=	LineFromChar(s);
	RefindAndFormatAll(bReturn);
	e	=	LineIndex(line+1);
	if(s > (e-1))
		s = e-1;
	if(bReturn)
		SetSel(s, s);
	//
	GetSel(s, e);
	int lineChar				=	LineIndex(line);
	switch(nChar)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_PRIOR:
	case VK_NEXT:
		break;
	default:
		m_nMaxLineCaretPos	=	s - lineChar;
		break;
	}
	this->Redraw();
}

void CMyCodeEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == '\t')
	{
		this->ReplaceSel(L"\t");
		return;
	}
	BOOL bControl	=	GetSelectKeyboard();
	BOOL bShift		=	GetCopyKeyboard();
	BOOL bPaste	=	false;
	if(nChar == L'V')
	{
		if(bControl)
		{
			PushUndo();
			bPaste	=	true;
		}
	}
	long start, end;
	GetSel(start, end);
	long startBegin				=	start;
	int currentLine				=	LineFromChar(start);
	int startLine				=	currentLine;
	long lineChar				=	LineIndex(currentLine);
	m_returnStartLine			=	currentLine;//this->GetFirstVisibleLine();//currentLine;
	//
	BOOL bDoParent = true;
	switch(nChar)
	{
	case VK_DELETE:
		{
			if(bShift && start == end)
			{
				if(currentLine < GetLineCount() - 1)
				{
					SetSel(lineChar, LineIndex(currentLine + 1));
					ReplaceSel(L"");
					long lineChar	=	LineIndex(currentLine + 1);
					if(start >= lineChar - 1)
						start = lineChar - 1;
					SetSel(start, start);
				}
				bDoParent	=	false;
			}
			else if(bControl)
			{
				bDoParent	=	false;
			}
		}
		break;
	}
	if(bDoParent)
		CMyEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	//
	GetSel(start, end);
	currentLine				=	LineFromChar(start);
	lineChar				=	LineIndex(currentLine);
	//
	switch(nChar)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_PRIOR:
	case VK_NEXT:
		{
			if(bShift == false && start == end)
			{
				long lineCharNext			=	LineIndex(currentLine + 1);
				start						=	lineChar + m_nMaxLineCaretPos;
				if(start > (lineCharNext - 1))
					start	=	lineCharNext - 1;
				SetSel(start, start);
			}
		}
		break;
	default:
		m_nMaxLineCaretPos	=	start - lineChar;
		break;
	}
	//if(nChar == L'V')
	//{
	//	if(bPaste)
	//	{
	//		long s,e;
	//		GetSel(s,e);
	//		FormatAll();
	//		SetSel(s,e);
	//	}
	//}
	//else 
	if(nChar == L'S')
	{
		if(bControl)
			OnSaveAll();
	}
	else if(nChar == L'Z')
	{
		//if(bControl)
		//	UndoOne();
	}
	else if(nChar == VK_F8)
	{
		SetCodeFormatLines();
	}
	else if(nChar == VK_F5)
		ExecuteCode();
	else if(nChar == VK_RETURN)
	{
		long start, end;
		GetSel(start, end);
		int lineBegin	=	LineFromChar(end);
		int nLineBegin	=	lineBegin - 1;
		//自动插入
		LineText lineText;
		CreateLineFormat(&lineText, nLineBegin);
		LineText* line	=	&lineText;
		BOOL bSetSpace	=	true;
		SetSel(start, end);
		const wchar_t* beginEnd[] = {L"do", L"if", L"function", L"while", L"for"};
		int count	=	sizeof(beginEnd) / sizeof(wchar_t*);
		if(line->sections.size() > 0)
		{
			int lineCount	=	this->GetLineCount();
			BOOL bExist		=	false;
			int nExistIndex	=	-1;
			for(int i=0;i<count;i++)
			{
				if(wcscmp(line->sections[0].GetText(), beginEnd[i]) == 0)
				{
					nExistIndex	=	i;
					bExist	=	true;
				}
			}
			if(bExist)
			{
				BOOL bFind	=	false;
				
				for(int i=nLineBegin;i<lineCount;i++)
				{
					LineText lineNext;
					CreateLineFormat(&lineNext, i);
					LineText* next	=	&lineNext;//GetLineText(i);
					if(next->sections.size() > 0)
					{
						if(wcscmp(next->sections[0].GetText(), L"end") == 0 && next->leftSpace == line->leftSpace)
						{
							bFind	=	true;
							break;
						}
						BOOL bExist2		=	false;
						for(int i=0;i<count;i++)
						{
							if(wcscmp(next->sections[0].GetText(), beginEnd[i]) == 0 && nExistIndex != i)
							{
								bExist2	=	true;
							}
						}
						if(bExist2)
						{
							bFind	=	false;
							break;
						}
					}
				}
				if(!bFind)
				{
					bSetSpace	=	false;
					wchar_t wbuf[1024];
					int index	=	0;
					wbuf[index]	=	0;
					for(int i=0;i<(line->leftSpace + 1)* g_spaceCharAdd;i++)
						wbuf[index++]	=	g_spaceChar;
					wbuf[index++]	=	L'\n';
					for(int i=0;i<line->leftSpace * g_spaceCharAdd;i++)
						wbuf[index++]	=	g_spaceChar;
					wbuf[index++]	=	0;
					wcscat(wbuf, L"end\n");
					this->ReplaceSel(wbuf);
				}
			}
			else
			{
				//if(wcscmp(line->sections[0].GetText(), L"else") == 0 || wcscmp(line->sections[0].GetText(), L"elseif") == 0)
				//{
				//	LineText* bLine	=	GetLineText(lineBegin);
				//	if(bLine)
				//	{
				//		int nSpace	=	bLine->leftSpace - 2;
				//		if(bLine->sections.size() > 0)
				//			if(wcscmp(bLine->sections[0].GetText(), L"if") == 0)
				//				nSpace	+=	2;
				//		this->SetBeginLineSpace(nLineBegin, nSpace);
				//	}
				//}
			}
		}
		LineText lineNew;
		CreateLineFormat(&lineNew, lineBegin);
		LineText* newline	=	&lineNew;
		if(bSetSpace && newline->sections.size() == 0)
		{
			wchar_t wbuf[1024];
			int index	=	0;
			wbuf[index]	=	0;
			//
			BOOL bExist2		=	false;
			if(line->sections.size() > 0)
			{
				for(int i=0;i<count;i++)
				{
					if(wcscmp(line->sections[0].GetText(), beginEnd[i]) == 0)
					{
						bExist2	=	true;
					}
				}
				if(wcscmp(line->sections[0].GetText(), L"else") == 0 || wcscmp(line->sections[0].GetText(), L"elseif") == 0)
					bExist2	=	true;
			}
			for(int i=0;i<line->leftSpace * g_spaceCharAdd;i++)
				wbuf[index++]	=	g_spaceChar;
			if(bExist2)
			{
				for(int i=0;i<g_spaceCharAdd;i++)
					wbuf[index++]	=	g_spaceChar;
			}
			wbuf[index++]	=	0;
			this->ReplaceSel(wbuf);
			//
			wchar_t buffer[5000];
			int length	=	GetLine(lineBegin, buffer, 5000);
			buffer[length]	=	0;
			if(index > 1)
				start	=	newline->beginIndex;
			const wchar_t* bufferPtr	=	buffer;
			while(*bufferPtr)
			{
				if(IsSpace(*bufferPtr))
				{
					bufferPtr++;
					start++;
				}
				else
					break;
			}
		}
		SetSel(start, start);
	}
	else
	{
	}
}

int GetLeftCharCount(const wchar_t* szText)
{
	int count = 0;
	while(*szText)
	{
		if(!IsSpace(*szText))
			break;
		szText++;
		count++;
	}
	return count;
}

BOOL	IsTextEquip(const wchar_t* t1, const wchar_t* t2, int length, BOOL bSection = TRUE)
{
	for(int i=0;i<length;i++)
	{
		if(t1[i] != t2[i])
			return false;
	}
	if(bSection)
	{
		BOOL bIsSymbol1 = (t1[length] == 0 || IsSectionSymbol(t1[length]));
		BOOL bIsSymbol2 = (t2[length] == 0 || IsSectionSymbol(t2[length]));
		return (bIsSymbol1 && bIsSymbol2);
	}
	return true;
}

VOID	CMyCodeEditor::LineText::AddToSection(const wchar_t* szText, int begin, int count, int charBegin, TextColor* maskColor)
{
	wchar_t buf[1024];
	wcsncpy(buf, szText + begin, count);
	buf[count]	=	0;
	const wchar_t* beginText = 0;
	for(int i=0;i<count;i++)
	{
		if(!beginText)
			if(IsSpace(buf[i]))
				continue;
		if(buf[i] == L'\r' || buf[i] == L'\n')
		{
			if(!beginText)
				charBegin++;
			buf[i] = 0;
		}
		else if(!beginText)
			beginText	=	buf;
	}
	if(beginText && beginText[0])
	{
		CMyCodeEditor::LineTextSection s;
		s.SetText(beginText);
		s.lineBeginIndex	=	charBegin;
		s.maskColor			=	maskColor;
		sections.push_back(s);
	}
}

VOID	CMyCodeEditor::SetLineFormat(INT nLine)
{
	LineText* pLine	=	this->GetLineText(nLine);
	SetTextColor(pLine->beginIndex, (int)pLine->lineText.size() - 1, TextColor());
	for(int i=0;i<pLine->sections.size();i++)
	{
		if(pLine->sections[i].maskColor)
			SetTextColor(pLine->sections[i].lineBeginIndex + pLine->beginIndex, pLine->sections[i].textLength, *pLine->sections[i].maskColor);
		//else
		//	SetTextColor(pLine->sections[i].lineBeginIndex + pLine->beginIndex, pLine->sections[i].textLength, TextColor());
	}
}

BOOL	CMyCodeEditor::CreateLineFormat(LineText* lt, int nLine)
{
	wchar_t buffer[5000];
	int l = this->GetLine(nLine, buffer, 5000);
	buffer[l] = 0;
	//
	LineText* pLine	=	lt;
	if(wcscmp(pLine->lineText.c_str(), buffer) == 0)
		return false;
	//
	int textBeginIndex			=	LineIndex(nLine);
	//c.text				=	buffer;
	int textLength			=	wcslen(buffer);
	int textLeftSpace		=	GetLeftCharCount(buffer);
	//
	pLine->lineText			=	buffer;
	int length				=	textLength - textLeftSpace;
	const wchar_t* szText	=	buffer + textLeftSpace;
	if(!szText[0])
		return true;
	pLine->leftSpace	=	textLeftSpace;
	pLine->beginIndex	=	textBeginIndex;
	pLine->sections.clear();
	//
	if(szText[0] == L'-' && szText[1] == L'-')
	{
		//SetTextColor(textBeginIndex, textLength, m_maskColor[MaskText::MaskType_Description]);
		pLine->AddToSection(szText, 0, length, textLeftSpace, &m_maskColor[MaskText::MaskType_Description]);
		return true;
	}
	//
	//SetTextColor(textBeginIndex, textLength, TextColor());
	wchar_t sectionCode[1024];
	int symbolCount	=	sizeof(g_luaSymbol);
	//
	bool	symbolBegin			=	true;
	int		sectionBegin		=	0;
	sectionCode[0]				=	0;
	for(int t=0;t<=length;t++)
	{
		//while(IsSpace(szText[t]))
		//{
		//	t++;
		//	if(t >= length)
		//		return;
		//}
		const wchar_t* tcs	=	szText + t;
		wchar_t ch	=	szText[t];
		if(szText[t] == L'-' && szText[t+1] == L'-')
		{
			pLine->AddToSection(szText, t, length, textLeftSpace + t, &m_maskColor[MaskText::MaskType_Description]);
			return true;
		}

		if(IsSectionSymbol(ch))
		{
			int section2	=	t - sectionBegin;
			if(section2 > 0)
			{
				wcsncpy(sectionCode, szText + sectionBegin, section2);
				sectionCode[section2]	=	0;
				//
				int sectionLength	=	wcslen(sectionCode);
				for(int i = sectionLength - 1;i>=0;i--)
				{
					if(IsSpace(sectionCode[i]))
						sectionCode[i] = 0;
					else
						break;
				}
				sectionLength	=	wcslen(sectionCode);
				if(sectionLength > 0)
				{
					TextColor* tcColor	=	0;
					BOOL bFind	=	false;
					for(int i=0;i<m_maskTexts.size();i++)
					{
						MaskText& tc	=	m_maskTexts[i];
						if(tc.type == tc.MaskType_Token)
							continue;
						if(sectionLength == tc.length)
						{
							if(IsTextEquip(tc.name, sectionCode, tc.length))
							{
								bFind	=	true;
								tcColor	=	&m_maskColor[tc.type];
								break;
							}
						}
					}
					if(!bFind)
					{
						TextColor*	tcVar	=	GetMaskMethodVar(sectionCode, sectionLength);
						tcColor	=	tcVar;
					}
					pLine->AddToSection(szText, sectionBegin, sectionLength, sectionBegin + textLeftSpace, tcColor);
				}
			}
			int beginT	=	t;
			TextColor*	tcVar = 0;
			for(int i=0;i<m_maskTexts.size();i++)
			{
				MaskText& tc	=	m_maskTexts[i];
				if(tc.type != tc.MaskType_Token)
					continue;
				if(IsTextEquip(tc.name, tcs, tc.length, false))
				{
					tcVar = &m_maskColor[tc.type];
					t	+=	tc.length - 1;
					break;
				}
			}
			BOOL bAddSection	=	true;
			int add_begin	=	beginT;
			int add_length	=	t - beginT + 1;
			sectionBegin		=	t + 1;
			//
			if(szText[t] == L'\"')
			{
				int beginT	=	t;
				t++;
				wchar_t ch2 = szText[t];
				while( ch2 != L'\"')
				{
					t++;
					if(t >= length)
					{
						pLine->AddToSection(szText, beginT, length - beginT, beginT + textLeftSpace, &m_maskColor[MaskText::MaskType_Text]);
						return true;
					}
					ch2 = szText[t];
				}
				if((t + 1 - beginT) > 0)
				{
					pLine->AddToSection(szText, beginT, t + 1 - beginT, beginT + textLeftSpace, &m_maskColor[MaskText::MaskType_Text]);
				}
				sectionBegin	=	t + 1;
				bAddSection	=	false;
				continue;
			}
			else if(szText[t] == L'\'')
			{
				int beginT	=	t;
				t++;
				wchar_t ch2 = szText[t];
				while(ch2 != L'\'')
				{
					t++;
					if(t >= length)
					{
						pLine->AddToSection(szText, beginT, length - beginT, beginT + textLeftSpace, &m_maskColor[MaskText::MaskType_Text]);
						return true;
					}
					ch2 = szText[t];
				}
				if((t + 1 - beginT) > 0)
				{
					pLine->AddToSection(szText, beginT, t + 1 - beginT, beginT + textLeftSpace, &m_maskColor[MaskText::MaskType_Text]);
				}
				sectionBegin	=	t + 1;
				bAddSection	=	false;
				continue;
			}
			if(bAddSection)
				pLine->AddToSection(szText, add_begin, add_length, add_begin + textLeftSpace, tcVar);
		}
		//
	}
	return true;
}

VOID CMyCodeEditor::OnBuildAll()
{
}
VOID CMyCodeEditor::FormatAll()
{
	int nLine	=	this->GetLineCount();
	//
	this->m_lineSections.clear();
	RefindAndFormatAll(TRUE);
	//
	SetSel(this->GetWindowTextLengthW(), this->GetWindowTextLengthW());
	this->Redraw();
}


void CMyCodeEditor::OnPaint()
{
	if(!m_bPaintEnable)
		return;
	__super::OnPaint();
}

BOOL CMyCodeEditor::OnEraseBkgnd(CDC* pDC)
{
	if(!m_bPaintEnable)
		return true;
	return CMyEdit::OnEraseBkgnd(pDC);
}

VOID CMyCodeEditor::SetBeginLineSpace(INT nLine, INT nSpace)
{
	int lineCount	=	this->GetLineCount();
	if(nLine >= lineCount)
		return;
	LineText* line	=	GetLineText(nLine);
	int nLeft	=	0;
	const wchar_t* szBuffer = line->lineText.c_str();
	while(*szBuffer)
	{
		if(IsSpace(*szBuffer))
		{
			szBuffer++;
			nLeft++;
			continue;
		}
		break;
	}
	wchar_t buf[128];
	buf[0]	=	0;
	if(nLeft != nSpace && nSpace >= 0)
	{
		int beginChar	=	LineIndex(nLine);
		this->SetSel(beginChar, beginChar + nLeft);
		for(int i=0;i<nSpace;i++)
			buf[i] = g_spaceChar;
		buf[nSpace] = 0;
		this->ReplaceSel(buf);
		LineText* lt	=	this->GetLineText(nLine);
		this->CreateLineFormat(lt, nLine);
		m_refreshLines.push_back(nLine);
		//this->SetLineFormat(nLine);
	}
}

VOID CMyCodeEditor::SetCodeFormatLines(int nAutoAdd)
{
	nAutoAdd	=	g_spaceCharAdd;
	int nSpace	=	0;
	const wchar_t* beginWord[] = {L"do", L"if", L"function", L"while", L"for"};
	int lineCount	=	this->GetLineCount();
	for(int nIndex = 0; nIndex < lineCount; nIndex++)
	{
		LineText* line	=	GetLineText(nIndex);
		if(line->sections.size() > 0)
		{
			int count	=	sizeof(beginWord) / sizeof(wchar_t*);
			BOOL bExist	=	false;
			for(int j=0;j<count;j++)
			{
				if(wcscmp(beginWord[j], line->sections[0].GetText()) == 0)
				{
					SetBeginLineSpace(nIndex, nSpace);
					nSpace	+=	nAutoAdd;
					bExist	=	true;
					break;
				}
			}
			if(bExist)
				continue;
			if(wcscmp(L"elseif", line->sections[0].GetText()) == 0 || wcscmp(L"else", line->sections[0].GetText()) == 0)
			{
				SetBeginLineSpace(nIndex, nSpace - nAutoAdd);
				continue;
			}
			for(int j=0;j<line->sections.size();j++)
				if(wcscmp(L"end", line->sections[j].GetText()) == 0)
				{
					nSpace	-=	nAutoAdd;
					break;
				}
		}
		SetBeginLineSpace(nIndex, nSpace);
	}
}

void CMyCodeEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
	long start, end;
	GetSel(start, end);
	int currentLine				=	LineFromChar(start);
	long lineChar				=	LineIndex(currentLine);
	m_nMaxLineCaretPos			=	start - lineChar;
}

VOID CMyCodeEditor::GetSel(long& s, long& e)
{
#ifdef USE_EDITBOX
	int s1= 0;
	int e1= 0;
	__super::GetSel(s1, e1);
	s = s1;
	e = e1;
#else
	__super::GetSel(s, e);
#endif
}

VOID CMyCodeEditor::Redraw()
{
	//this->Invalidate();
}

void CMyCodeEditor::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove(nFlags, point);
	this->Redraw();
}
