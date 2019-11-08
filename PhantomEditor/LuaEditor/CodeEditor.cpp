/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// CodeEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "CodeEditor.h"
#include "MyLuaEditor.h"
#include "Imm.h"
#include "io.h"
#include "MainFrm.h"

#ifdef TESTMODE
const wchar_t* Language(const wchar_t* t){return t;}
#else
extern	const wchar_t*			Language(const wchar_t* szName);
#endif
extern BOOL	issection(wchar_t c);
// CodeEditor
MaskText::MaskType		CodeEditor::m_currType;
std::vector<MaskText>	CodeEditor::m_masks;
TextColor				CodeEditor::m_maskColor[MaskText::MaskType_Count];
IMPLEMENT_DYNAMIC(CodeEditor, CRichEditCtrl)

std::vector<CodeEditor*>	g_codeEditors;
VOID	RefreshAllColors(){
	for(int i=0;i<g_codeEditors.size();i++){
		g_codeEditors[i]->InitColor();
		g_codeEditors[i]->CheckLineColor(-1,-1);
	}
}
CodeEditor::CodeEditor()
{
	m_currType		=	MaskText::MaskType_None;
	m_memberCaret	=	0;
	m_strFontFace	=	L"Courier New";
	m_startCombo	=	-1;
	m_bPaintEnable	=	true;
	m_rebuildCode	=	0;
	m_bIsChanged	=	false;
	m_saveIMEBegin = m_saveIMEEnd = m_saveIMELine = m_saveIMELineBegin = 0;
	m_undoPtr		=	0;
	m_lastUndoCaret	=	0;
	m_undoWritePtr	=	0;
	m_bWaitTip		=	100000;
	m_lastPos.SetPoint(0,0);
	m_firstLine		=	-1;
	m_callback		=	0;
	m_bModifyed		=	0;
	m_dblStart		=	0;
	m_dblEnd		=	0;
	m_controlPtr	=	0;
	m_bCharChanged	=	false;
	g_codeEditors.push_back(this);
}

CodeEditor::~CodeEditor()
{
	ClearRedos();
	for(int i=0;i<m_undos.size();i++)
		delete m_undos[i];
	m_undos.clear();
	for(int i=0;i<g_codeEditors.size();i++){
		if(g_codeEditors.at(i)==this){
			g_codeEditors.erase(g_codeEditors.begin()+i);
			break;
		}
	}
}


BEGIN_MESSAGE_MAP(CodeEditor, CRichEditCtrl)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_WM_MOUSEHWHEEL()
	ON_CONTROL_REFLECT(EN_VSCROLL, &CodeEditor::OnEnVscroll)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()
VOID		CodeEditor::InitColor()
{
	SetBackgroundColor(false, m_maskColor[MaskText::MaskType_BackColor].c);
	CHARFORMAT cf;
	this->GetDefaultCharFormat(cf);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects &= ~CFE_AUTOCOLOR;//这个最重要，设选中文字颜色的时候也要注意,dwEffects一定不能有CEF_AUTOCOLOR的属性
	cf.crTextColor=m_maskColor[MaskText::MaskType_Normal].c;//RGB(10,10,10);//red
	SetDefaultCharFormat(cf);
	this->Invalidate();
}
VOID		CodeEditor::SaveFile()
{

	CString str;
	GetWindowTextW(str);
	int slen = (int)wcslen(str);
	int len = (int)slen * 3 + 1;
	char* buf = new char[len];
	len = ::WideCharToMultiByte(CP_UTF8, 0, str, slen, buf, len, 0, 0);
	buf[len] = 0;
	//
	//if(m_controlPtr)
	//	m_controlPtr->SetCodeText(buf);
	//else{
		FILE* f = fopen(m_fileName, "wb");
		if(!f)return;
		unsigned char head[3];
		head[0] = 0xef;
		head[1] = 0xbb;
		head[2] = 0xbf;
		fwrite(head, 3, 1, f);
		fwrite(buf, len, 1, f);
		fclose(f);
	//}
	//
	delete buf;
}
VOID		CodeEditor::SetInitFile(const char* szFile)
{
	m_fileName = szFile;
	//SetProjectPath();
	FILE* f = fopen(m_fileName, "rb");
	if(!f)return;
	int len = _filelength(_fileno(f));
	if(len>3){
		unsigned char head[3];
		fread(head, 3, 1, f);
		if(!(head[0] == 0xef && head[1] == 0xbb && head[2] == 0xbf))
		{
			fclose(f);
			return;
		}
		int slen = _filelength(_fileno(f))-3;
		char* text = new char[slen+1];
		if(slen>0)
			fread(text, slen, 1, f);
		text[slen] = 0;
		int len = (int)slen * 3 + 1;
		wchar_t* buf = new wchar_t[len];
		len = ::MultiByteToWideChar(CP_UTF8, 0, text, slen, buf, len);
		buf[len] = 0;
		SetInitText(buf, szFile);
		delete[] text;
		delete[] buf;
	}
	else{
		SetInitText(L"\r", szFile);
	}
	fclose(f);
}
VOID		CodeEditor::Setup(CWnd* wnd, const CRect& rc, CFont* font, INT idToolTip, INT iconRes)
{
	Create(WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_VISIBLE | WS_HSCROLL | ES_AUTOHSCROLL | WS_TABSTOP, rc, wnd, 0);
	this->SetOptions(ECOOP_XOR, ECO_SAVESEL);
	int tabWidth = 12;
	this->SendMessage(EM_SETTABSTOPS, (WPARAM)1, (LPARAM)&tabWidth);
	this->SetFont(font);
	if(m_masks.size()==0){
		AddMask(MaskText::MaskType_Keywords) >> L"and" >> L"break" >> L"do" >> L"else" >> L"elseif" >> L"end" >> L"false";
		(*this) >> L"for" >> L"function" >> L"if" >> L"in" >> L"local" >> L"nil" >> L"not" >> L"or" >> L"repeat" >> L"return";
		(*this) >> L"then" >> L"true" >> L"until" >> L"while" >> L"null";
		m_maskColor[MaskText::MaskType_None].Setup(MaskText::Read(MaskText::MaskType_None, RGB(10, 10, 10)), false);
		m_maskColor[MaskText::MaskType_Keywords].Setup(MaskText::Read(MaskText::MaskType_Keywords, RGB(0, 0, 255)), true);
		m_maskColor[MaskText::MaskType_Token].Setup(MaskText::Read(MaskText::MaskType_Token, RGB(20,20,60)), false);
		m_maskColor[MaskText::MaskType_Variant].Setup(MaskText::Read(MaskText::MaskType_Variant, RGB(0, 0, 0)), false);//RGB(227, 109, 99), false);
		m_maskColor[MaskText::MaskType_Enum].Setup(MaskText::Read(MaskText::MaskType_Enum, RGB(50,150,25)), false);
		m_maskColor[MaskText::MaskType_Method].Setup(MaskText::Read(MaskText::MaskType_Method, RGB(0,0,0)), false);
		m_maskColor[MaskText::MaskType_Text].Setup(MaskText::Read(MaskText::MaskType_Text, RGB(163, 21, 21)), false);
		m_maskColor[MaskText::MaskType_Comment].Setup(MaskText::Read(MaskText::MaskType_Comment, RGB(0, 123, 0)), false);
		m_maskColor[MaskText::MaskType_Number].Setup(MaskText::Read(MaskText::MaskType_Number, RGB(250, 10, 10)), false);
		m_maskColor[MaskText::MaskType_Normal].Setup(MaskText::Read(MaskText::MaskType_Normal, RGB(60,60,60)), false);
		m_maskColor[MaskText::MaskType_BackColor].Setup(MaskText::Read(MaskText::MaskType_BackColor, RGB(255,255,255)), false);
		m_maskColor[MaskText::MaskType_PhantomAPI].Setup(MaskText::Read(MaskText::MaskType_PhantomAPI, RGB(0,128,192)), false);
		m_maskColor[MaskText::MaskType_LuaAPI].Setup(MaskText::Read(MaskText::MaskType_LuaAPI, RGB(0,0,160)), false);
		m_maskColor[MaskText::MaskType_LineText].Setup(MaskText::Read(MaskText::MaskType_LineText, RGB(230,230,230)), false);
		m_maskColor[MaskText::MaskType_LineBack].Setup(MaskText::Read(MaskText::MaskType_LineBack, RGB(50,50,50)), false);
	}
	this->InitColor();
	this->LimitText(1024 * 1024*10);
	m_toolTip.Create(idToolTip, this);
	m_toolTip.ShowWindow(SW_HIDE);
	//
	//m_lbFunctions.Create(CRect(0, 0, 200, 300), wnd);
	//m_lbFunctions.ShowWindow(SW_SHOW);
	//m_lbFunctions.SetFont(font);
	//m_lbFunctions.SetIconRes(iconRes);
	//m_lbFunctions.SetWindowTextW(L"");
	//m_lbFunctions.SetSel(0);
	//m_lbFunctions.m_codePtr	=	this;
	//m_lbFunctions.m_bIsToolTip = false;
	//
	m_listbox.Create(CRect(0, 0, 200, 300), this);
	::SetWindowLongW(m_listbox.m_hWnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | ::GetWindowLongW(m_listbox.m_hWnd, GWL_EXSTYLE));
	m_listbox.SetParent(0);
	m_listbox.SetFont(font);
	m_listbox.SetIconRes(iconRes);
	m_listbox.SetWindowTextW(L"");
	m_listbox.SetSel(0);
	m_listbox.m_codePtr	=	this;
	//
	int cb = WS_CHILD|CBS_HASSTRINGS|CBS_DISABLENOSCROLL|WS_VSCROLL | CBS_OWNERDRAWVARIABLE|CBS_DROPDOWNLIST|CBS_SORT;
	m_cbEvents.Create(CRect(0, 0, 200, 300), wnd, cb);
	m_cbEvents.SetFont(font);
	m_cbEvents.SetIconRes(iconRes);
	m_cbEvents.SetWindowTextW(L"");
	m_cbEvents.ShowWindow(SW_SHOW);
	m_cbEvents.m_codePtr	=	this;
	m_cbMethods.Create(CRect(0, 0, 200, 300), wnd, cb);
	m_cbMethods.SetFont(font);
	m_cbMethods.SetIconRes(iconRes);
	m_cbMethods.SetWindowTextW(L"");
	m_cbMethods.ShowWindow(SW_SHOW);
	m_cbMethods.m_codePtr	=	this;
	this->SetUndoLimit(0);
	SetTimer(1, 100, 0);
}
CodeEditor&	CodeEditor::operator >> (const wchar_t* v)
{
	this->m_masks.push_back(MaskText(v, m_currType));
	return *this;
}
CodeEditor&	CodeEditor::AddMask(MaskText::MaskType t)
{
	m_currType = t;
	return *this;
}
VOID	CodeEditor::SetTextColor(long begin, long count, const TextColor& c)
{
	if(count <= 0)
		return;
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask		=	CFM_COLOR;//CFM_STRIKEOUT|CFM_BOLD|CFM_COLOR|CFM_FACE|CFM_SIZE;
	wcscpy(cf.szFaceName, m_strFontFace);//L"Arial");
	cf.dwEffects	=	0;//(c.bBold ? CFE_BOLD : 0);
	cf.yHeight		=	10 * 20;
	cf.crTextColor	=	c.c;
	this->SetSel(begin, begin + count);
	this->SetSelectionCharFormat(cf);
}

VOID CodeEditor::CheckLineColor(int lineIndex)//重新计算颜色
{
	static CDynamicArray<wchar_t> lineBuffer;
	static CDynamicArray<MaskSection> sections;
	//
	int length = this->LineLength(lineIndex);
	if(length<=0)return;
	if(length<1024)length=1024;
	lineBuffer.SetArrayCount(length);
	memset(lineBuffer.GetPtr(), lineBuffer.size(), sizeof(wchar_t)*lineBuffer.size());
	length = this->GetLine(lineIndex, lineBuffer.GetPtr(), lineBuffer.size());
	lineBuffer.GetPtr()[length] = 0;
	sections.SetArrayCount(0);
	CCodeLine::SplitSection(&sections, lineBuffer.GetPtr(), length);
	int start = this->LineIndex(lineIndex);
	int end = start + length;
	//SetTextColor(start, end - start, m_maskColor[MaskText::MaskType_None]);
	this->SetSel(start, end);
	CHARFORMAT cf;
	GetSelectionCharFormat(cf);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects &= ~CFE_AUTOCOLOR;//这个最重要，设选中文字颜色的时候也要注意,dwEffects一定不能有CEF_AUTOCOLOR的属性
	cf.crTextColor=m_maskColor[MaskText::MaskType_Normal].c;//RGB(10,10,10);//red
	SetSelectionCharFormat(cf);
	//
	memset(&cf, 0, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(cf);
	cf.dwMask		=	CFM_FACE|CFM_SIZE;
	wcscpy(cf.szFaceName, m_strFontFace);//L"Arial");
	cf.dwEffects	=	0;//(c.bBold ? CFE_BOLD : 0);
	cf.yHeight		=	9 * 20;
	this->SetSelectionCharFormat(cf);
	//
	for(int i=0;i<sections.size();i++){
		BOOL bFind = false;
		for(int j=0;j<m_masks.size();j++){
			if(wcsicmp(m_masks[j].name, sections[i].name)==0){
				SetTextColor(start + sections[i].begin, sections[i].count, m_maskColor[m_masks[j].type]);
				bFind = true;
				break;
			}
		}
		if(!bFind){
			if(sections[i].name[0]==L'('&&i>0){
				SetTextColor(start + sections[i-1].begin, sections[i-1].count, m_maskColor[MaskText::MaskType_Method]);
			}
			if((sections[i].name[0]==L'.'||sections[i].name[0]==L':')){
				SetTextColor(start + sections[i].begin, sections[i].count, m_maskColor[MaskText::MaskType_Token]);
				continue;
			}
			if(sections[i].name[0]==L'\"'){
				SetTextColor(start + sections[i].begin, sections[i].count, m_maskColor[MaskText::MaskType_Text]);
				continue;
			}
			if(sections[i].name[0]==L'-'&&sections[i].name[1]==L'-'){
				SetTextColor(start + sections[i].begin, sections[i].count, m_maskColor[MaskText::MaskType_Comment]);
				continue;
			}
			if(sections[i].count==1&&issection(sections[i].name[0])){
				SetTextColor(start + sections[i].begin, sections[i].count, m_maskColor[MaskText::MaskType_Token]);
				continue;
			}
			if(_wtoi(sections[i].name)>0||sections[i].name[0] == L'0'){
				SetTextColor(start + sections[i].begin, sections[i].count, m_maskColor[MaskText::MaskType_Number]);
				continue;
			}
			if(sections[i].type==MaskSection::Normal){
				CCodeClass* c = GetCodeXmlManager()->FindClass(sections[i].name);
				if(c&&c->IsEnumMember()){
					SetTextColor(start + sections[i].begin, sections[i].count, m_maskColor[MaskText::MaskType_Enum]);
				}
			}
		}
	}
}
VOID CodeEditor::CheckLineColorRange(int b, int e)
{
	long savestart,saveend;
	this->GetSel(savestart, saveend);
	int beginLine = min(b,e);
	int endLine = max(b,e);
	if(beginLine<0)beginLine = 0;
	if(endLine >= this->GetLineCount())
		endLine = this->GetLineCount();
	if(endLine<0)return;
	SetOptions(ECOOP_XOR, ECO_AUTOVSCROLL | ECO_AUTOHSCROLL);
	m_bPaintEnable = false;
	for(int i=beginLine;i<=endLine;i++)
	{
		this->CheckLineColor(i);
	}
	this->SetSel(savestart, saveend);
	SetOptions(ECOOP_OR, ECO_AUTOVSCROLL | ECO_AUTOHSCROLL);
	if(m_dlgEditor)m_dlgEditor->RedrawLines();
	m_bPaintEnable = true;
}
VOID CodeEditor::CheckLineColor(int nBeforeLine, int nLastLine)//重新计算颜色
{
	if(nBeforeLine==-1&&nLastLine==-1){
		CheckLineColorRange(0, this->GetLineCount());
	}
	else{
		long savestart,saveend;
		this->GetSel(savestart, saveend);
		int lineBegin = this->LineFromChar(savestart);
		CheckLineColorRange(lineBegin-nBeforeLine, lineBegin+nLastLine);
	}
}
void CodeEditor::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nChar==L'F'&&HIBYTE(GetKeyState(VK_CONTROL))){
		long b,e;
		GetSel(b,e);
		GetMainFrame()->m_dlgFind.m_editor = this;
		GetMainFrame()->m_dlgFind.ShowFind(GetSelText(), false);
		this->m_dlgEditor->InvalidateDlg();
		return;
	}
	else if(nChar==VK_F3)
	{
		GetMainFrame()->m_dlgFind.OnBnClickedOk();
	}
	else if(nChar==L'H'&&HIBYTE(GetKeyState(VK_CONTROL))){
		long b,e;
		GetSel(b,e);
		GetMainFrame()->m_dlgFind.m_editor = this;
		GetMainFrame()->m_dlgFind.ShowFind(GetSelText(), true);
	}
	switch(nChar){
	case VK_DELETE:
	case VK_UP:
	case VK_DOWN:
	case VK_PRIOR:
	case VK_NEXT:
		{
			CRichEditCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
			return;
		}
		break;
	}
	if(m_listbox.IsWindowVisible())
	{
		if((nChar >= '0' && nChar < 'Z') || nChar == VK_DELETE || nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_HOME || nChar == VK_END || nChar == VK_ESCAPE)
			AutoShowListBox();
		if(nChar==VK_BACK){
			long b,e;
			GetSel(b,e);
			CString str;
			GetTextRange(b-1,b,str);
			if(!(str==L"."||str==L":"||str==L"="))
				AutoShowListBox();
		}
	}
	//long b,e;
	//GetSel(b,e);
	//CString str;
	//GetTextRange(b-2,b,str);
	//BOOL bTip = false;
	//if(str.GetLength()>1){
	//	wchar_t wch = str[0];
	//	wchar_t wn = toupper(nChar);
	//	if(!!(HIBYTE(GetKeyState(VK_CONTROL))||HIBYTE(GetKeyState(VK_MENU))||HIBYTE(GetKeyState(VK_SHIFT))) && issection(wch)&&!(wch==L':'||wch==L'.')&&wn>=L'A'&&wn<=L'Z')
	//	{
	//		bTip = true;
	//	}
	//	else if(issection(wch))
	//	{
	//		m_toolTip.ShowWindow(SW_HIDE);
	//		m_listbox.ShowWindow(SW_HIDE);
	//		bTip = false;
	//	}
	//}
	if(/*bTip || */nChar==VK_F2 || (nChar==L'J'&&HIBYTE(GetKeyState(VK_CONTROL))))
	{
		long end = 0;
		this->GetSel(m_startCombo, end);
		CPoint pos	=	PosFromChar(m_startCombo);
		CPoint popup=	CPoint(pos.x, pos.y + 20);
		GetCodeXmlManager()->ResetVariants();
		this->m_codeMgr.RefreshVariantList(LineIndex(m_startCombo));
		ClientToScreen(&popup);
		if(!m_listbox.AutoPopup(0, popup, nChar))
		{
			m_listbox.ShowWindow(SW_HIDE);
			SetFocus();
		}
		return;
	}
	CRichEditCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}
BOOL	CopyFromClip(CDynamicArray<wchar_t>& rets){
	//打开剪贴板
	if ( OpenClipboard(NULL) )
	{
		HANDLE hData = GetClipboardData(CF_TEXT);
		if(hData!=INVALID_HANDLE_VALUE){
			char * buffer = (char*)GlobalLock(hData);
			if(buffer){
				W w(buffer);
				rets.SetArrayCount(wcslen(w.buf)+1);
				memcpy(rets.GetPtr(), w.buf, sizeof(wchar_t)*rets.size());
			}
			GlobalUnlock(hData);
		}
		CloseClipboard();
	}
	return (rets.size()>0);
}
VOID	CopyTextToClip(const char* szText)
{
	if( OpenClipboard( NULL ))
	{
		int len	=	(int)strlen(szText) + 1;
		EmptyClipboard();
		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, len );
		if( hBlock )
		{
			char *pwszText = (char*)GlobalLock( hBlock );
			memcpy(pwszText, szText, len);
			GlobalUnlock( hBlock );
			SetClipboardData( CF_TEXT, hBlock );
		}
		CloseClipboard();
		if( hBlock )
			GlobalFree( hBlock );
	}
}
extern	BOOL	FindMethod(const wchar_t* m);
BOOL CodeEditor::ReturnCheckEvent()
{
	long b,e;
	GetSel(b,e);
	int nLine = LineFromChar(b);
	CDynamicArray<wchar_t> buff;
	int length = GetLineText(buff, nLine);
	CDynamicArray<MaskSection> codes;
	CDynamicArray<MaskSection> voids;
	CCodeLine::SplitSection(0, buff.GetPtr(), length, &codes);
	int beginSec = 0;
	for(int i=codes.size()-1;i>=0;i--){
		if(codes[i].type==MaskSection::Section&&codes[i].name[0]==L'(')
		{
			beginSec = i;
			break;
		}
	}
	bool begin = false;
	for(int i=beginSec;i<codes.size();i++){
		if(codes[i].type==MaskSection::Section&&codes[i].name[0]==L'('){
			begin = true;
		}
		else if(begin)
		{
			if(codes[i].type==MaskSection::Section&&codes[i].name[0]==L')'|| codes[i].type==MaskSection::Section&&codes[i].name[0]==L',')
				voids.push_back(codes[i-1]);
			if(codes[i].type==MaskSection::Section&&codes[i].name[0]==L')')
				break;
			if(codes[i].type==MaskSection::Section&&codes[i].name[0]==L',')
				continue;
		}
	}
	CCodeLine::RemoveSection(voids, L'[', L']');
	if(beginSec>=0&&beginSec<codes.size()){
		wchar_t buf[512];
		int len = RequestCalls(buf, nLine, codes[beginSec].begin+LineIndex(nLine)+1, 0);
		if(len>0){
			CCodeMember* m = this->m_codeMgr.GetFunctionMemberPtr(buf, nLine);
			if(m&&m->isEventIndex>=0&&m->isEventIndex<voids.size()){
				if(voids[m->isEventIndex].name[0]==L'\"')//如果是文本{
				{
					wchar_t name[512];
					memset(name, 0, 512*sizeof(wchar_t));
					wcsncpy(name, voids[m->isEventIndex].name+1,wcslen(voids[m->isEventIndex].name)-2);
					if(!FindMethod(name)){//如果没有找到这个函数
							if(MessageBox(L"没有找到这个事件函数,是否新建?", 0, MB_YESNO)==IDYES){
								int endof = GetTextLength();
								SetSel(endof, endof);
								swprintf(buf, L"function %s%s\r\t", name, m->eventText.str());
								endof = endof+wcslen(buf);
								wcscat(buf, L"\rend\r");
								ReplaceText(buf);
								SetSel(endof,endof);
								return true;
							}
					}
				}
			}
		}
	}
	return false;
}
void CodeEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL ctrl = HIBYTE(GetKeyState(VK_CONTROL));
	BOOL shift = HIBYTE(GetKeyState(VK_SHIFT));
	m_bIsSelect = false;
	if(nChar == VK_TAB)
	{
		this->ReplaceText(L"\t");
		return;
	}
	if(nChar==191&&ctrl){
		long s,e;
		GetSel(s,e);
		long bline = LineFromChar(s);
		long eline = LineFromChar(e);
		bool bComment = true;
		CDynamicArray<wchar_t> str;
		//
		int length = GetLineText(str, bline);
		int begin = 0;
		while(begin<length){
			if(str[begin]==L' '||str[begin]==L'\t')
			{
				begin++;
				continue;
			}
			break;
		}
		if(str.size()>(begin+2)&&str[begin]==L'-'&&str[begin+1]==L'-'){
			bComment = false;
		}
		CString full;
		for(int i=bline;i<=eline;i++){
			length = GetLineText(str, i);
			begin = 0;
			while(begin<length){
				if(str[begin]==L' '||str[begin]==L'\t')
				{
					begin++;
					continue;
				}
				break;
			}
			if(bComment){
				if(str.size()>(begin+2)&&str[begin]==L'-'&&str[begin+1]==L'-')
				{
				}
				else
					full += L"--";
			}
			else{
				if(str.size()>(begin+2)&&str[begin]==L'-'&&str[begin+1]==L'-'){
					str.eraseByIndex(begin,2);
				}
			}
			full += str.GetPtr();
		}
		//full.Remove(full.GetLength()-1);
		long ns = LineIndex(bline);
		long ne = LineIndex(eline+1)-1;
		SetSel(ns,ne);
		ReplaceText(full);
		SetSel(s,e);
		return;
	}
	if(nChar==L'F'&&HIBYTE(GetKeyState(VK_CONTROL)))
		return;
	if(nChar==189&&ctrl&&shift){
		RedoCaret();
	}
	else if(nChar==189&&ctrl){
		UndoCaret();
	}
	if(m_listbox.IsWindowVisible())
	{
		if(nChar == VK_UP)
		{
			int sel = this->m_listbox.GetCurSel();
			if(sel > 0)
				m_listbox.SetCurSel(sel - 1);
			return;
		}
		else if(nChar == VK_DOWN)
		{
			int sel = this->m_listbox.GetCurSel();
			if(sel < this->m_listbox.GetCount())
				this->m_listbox.SetCurSel(sel + 1);
			return;
		}
		else if(nChar == VK_PRIOR)
		{
			int sel = this->m_listbox.GetCurSel();
			if(sel > 0)
			{
				sel	-=	5;
				if(sel < 0)sel = 0;
				m_listbox.SetCurSel(sel);
			}
			return;
		}
		else if(nChar == VK_NEXT)
		{
			int sel = this->m_listbox.GetCurSel();
			if(sel < this->m_listbox.GetCount())
			{
				sel	+=	5;
				if(sel >= this->m_listbox.GetCount() - 1)
					sel	=	this->m_listbox.GetCount() - 1;
				this->m_listbox.SetCurSel(sel);
			}
			return;
		}
		else if(nChar == VK_ESCAPE)
		{
			m_toolTip.ShowWindow(SW_HIDE);
			m_listbox.ShowWindow(SW_HIDE);
			SetFocus();
		}
		else if(nChar == VK_RETURN||nChar==190||nChar==186)//||nChar==186||(nChar==57&&shift)||nChar==188)//逐渐提示中会出一些问题，所以只保留enter键可以有效的从列表中选择
		{
			m_bIsSelect = true;
			SelectFromListBox(nChar);
			return;
		}
		else if(nChar==187||(nChar==57&&shift)||(nChar==56&&shift)||(nChar==48&&shift)||nChar==188||nChar==221){
			m_toolTip.ShowWindow(SW_HIDE);
			m_listbox.ShowWindow(SW_HIDE);
			SetFocus();
		}
	}
	if(nChar==L'X'&&ctrl)
	{
		long b,e;
		GetSel(b,e);
		if(e>b){
			CString str = GetSelText();
			CopyTextToClip(A(str).buf);
			ReplaceText(L"");
		}
		return;
	}
	if(nChar == L'Z'&&ctrl)
	{
		this->m_toolTip.ShowWindow(SW_HIDE);
		Undo();
		return;
	}
	if(nChar == L'Y'&&ctrl)
	{
		this->m_toolTip.ShowWindow(SW_HIDE);
		Redo();
		return;
	}
	if(nChar == L'V'&&ctrl)
	{
		this->m_toolTip.ShowWindow(SW_HIDE);
		CDynamicArray<wchar_t> str;
		if(CopyFromClip(str)){
			ReplaceText(str.GetPtr());
		}
		return;
	}
	switch(nChar){
	case VK_UP:
	case VK_DOWN:
		{
			if(!shift&&!ctrl){
				if(m_bCharChanged){
					long s,e;
					GetSel(s,e);
					this->CheckLineColorRange(LineFromChar(s), LineFromChar(s));
					m_bCharChanged = false;
				}
			}
		}
		break;
	case VK_END:
		{
			if(shift){
				long s,e;
				GetSel(s,e);
				e = LineIndex(LineFromChar(e)+1) - 1;
				SetSel(s,e);
				return;
			}
		}
		break;
	}
	static int shift_begin = -1;
	if((nChar == VK_LEFT||nChar == VK_RIGHT)&&ctrl){
		long s,e;
		GetSel(s,e);
		int nline = LineFromChar(s);
		int begin = LineIndex(nline);
		if(s==begin&&nChar==VK_LEFT)
		{
			if(s>0)
			{
				if(shift)
					SetSel(s-1,e);
				else
					SetSel(s-1,s-1);
			}
			if(!shift)
				return;
		}
		CCodeLine* line = m_codeMgr.GetLine(nline);
		CDynamicArray<MaskSection>& secs = line->m_fulls;
		if(shift){
			if(shift_begin==-1)
				shift_begin = s;
			for(int i=0;i<secs.size();i++)
			{
				if(nChar==VK_LEFT){
					if(e>shift_begin){
						long ch = e - begin - secs[i].begin;
						if(ch>0&&ch<=secs[i].count){
							e = begin + secs[i].begin;
							SetSel(s, e);
							break;
						}
					}
					else{
						long ch = s - begin - secs[i].begin;
						if(ch>0&&ch<=secs[i].count){
							s = begin + secs[i].begin;
							SetSel(s, e);
							break;
						}
					}
				}
				else
				{
					if(s<shift_begin)//如果不是最新开始点
					{
						long ch = s - begin - secs[i].begin;
						if(ch>=0&&ch<secs[i].count){
							s = begin + secs[i].begin + secs[i].count;
							SetSel(s, e);
							break;
						}
					}
					else{
						long ch = e - begin - secs[i].begin;
						if(ch>=0&&ch<secs[i].count){
							e = begin + secs[i].begin + secs[i].count;
							SetSel(s, e);
							break;
						}
					}
				}
			}
		}
		else{
			shift_begin = -1;
			for(int i=0;i<secs.size();i++)
			{
				if(nChar==VK_LEFT){
					long ch = s - begin - secs[i].begin;
					if(ch>0&&ch<=secs[i].count){
						s = begin + secs[i].begin;
						e = s;
						SetSel(s, e);
						break;
					}
				}
				else
				{
					long ch = e - begin - secs[i].begin;
					if(ch>=0&&ch<secs[i].count){
						e = begin + secs[i].begin + secs[i].count;
						s = e;
						SetSel(s, e);
						break;
					}
				}
			}
		}
		return;
	}
	if(!shift)
		shift_begin = -1;
	if(nChar==VK_HOME&&!ctrl&&!shift){
		long s,e;
		GetSel(s,e);
		int line = LineFromChar(s);
		int begin = LineIndex(line);
		if(s==begin)
		{
			CDynamicArray<wchar_t> buff;
			int length = GetLineText(buff, LineFromChar(s));
			for(int i=0;i<length;i++){
				if(!(buff[i]==L'\t' || buff[i]==L' '))
					break;
				s++;
			}
		}
		else
			s = begin;
		SetSel(s,s);
		return;
	}
	if(nChar == VK_RETURN){
		if(ReturnCheckEvent())
			return;
		this->m_toolTip.ShowWindow(SW_HIDE);
		CDynamicArray<wchar_t> buff;
		long s,e;
		GetSel(s,e);
		int beginLine = LineFromChar(s);
		int length = GetLineText(buff, beginLine);
		wchar_t wspace[128];
		NameW first;
		int numSpace = 0;
		for(int i=0;i<length;i++)
		{
			if(buff[i]==L'\t'||buff[i]==L' ')
				wspace[numSpace++] = buff[i];
			else{
				first = buff.GetPtr() + i;
				break;
			}
		}
		wchar_t wReplace[256];
		wcscpy(wReplace, L"\r");
		wspace[numSpace] = 0;
		wchar_t wtemp[128];
		wcscpy(wtemp, wspace);
		wcscat(wspace, L"end");
		numSpace = wcslen(wspace);
		BOOL bIsFirst = false;
		if(wcsnicmp(first, L"if", 2) == 0||wcsnicmp(first, L"elseif", 6) == 0||wcsnicmp(first, L"do", 2) == 0||wcsnicmp(first, L"function", 8) == 0
			||wcsnicmp(first, L"for", 3) == 0||wcsnicmp(first, L"else", 4) == 0)
			bIsFirst = true;
		BOOL bFind = false;
		if(bIsFirst){
			int lineCount = GetLineCount();
			const wchar_t* func = L"function";
			int nFun = wcslen(func);
			for(int i=beginLine+1;i<lineCount;i++){
				GetLineText(buff, i);
				if(wcsnicmp(wspace, buff.GetPtr(), numSpace)==0){
					bFind = true;
					break;
				}
				else if(wcsnicmp(wspace, func, nFun) == 0){
					bFind = false;
					break;
				}
			}
		}
		else
			bFind = true;
		int beginCaret = -1;
		if(!bFind){
			wcscat(wtemp, L"\t");
			wcscat(wReplace, wtemp);
			beginCaret = wcslen(wReplace) + s;
			wcscat(wReplace, L"\r");
			wcscat(wReplace, wspace);
			wcscat(wReplace, L"\r");
		}
		else
		{
			wcscat(wReplace, wtemp);
			beginCaret = wcslen(wReplace) + s;
		}
		ReplaceText(wReplace, beginCaret);
		return;
	}
	switch(nChar){
	case VK_DELETE:
		{
			long s,e;
			GetSel(s,e);
			if(ctrl){
				int nline = LineFromChar(s);
				int begin = LineIndex(nline);
				CCodeLine* line = m_codeMgr.GetLine(nline);
				CDynamicArray<MaskSection>& secs = line->m_fulls;
				for(int i=0;i<secs.size();i++)
				{
					long ch = e - begin - secs[i].begin;
					if(ch>=0&&ch<secs[i].count){
						e = begin + secs[i].begin + secs[i].count;
						SetSel(s, e);
						break;
					}
				}
			}
			int length = GetTextLength();
			if(s!=e){
				if(s<length)
					ReplaceText(L"");
			}
			else if(e<length){
				SetSel(e,e+1);
				ReplaceText(L"");
			}
		}
		return;
	case VK_BACK:
		{
			long s,e;
			GetSel(s,e);
			if(ctrl){
				int nline = LineFromChar(s);
				int begin = LineIndex(nline);
				CCodeLine* line = m_codeMgr.GetLine(nline);
				CDynamicArray<MaskSection>& secs = line->m_fulls;
				for(int i=0;i<secs.size();i++)
				{
					long ch = s - begin - secs[i].begin;
					if(ch>0&&ch<=secs[i].count){
						s = begin + secs[i].begin;
						SetSel(s, e);
						break;
					}
				}
			}
			int length = GetTextLength();
			if(s!=e){
				if(s<length)
					ReplaceText(L"");
			}
			else if(s>0){
				SetSel(s-1,s);
				ReplaceText(L"");
			}
		}
		return;
	}
	//if(nChar==VK_LEFT&&shift){
	//	//long s,e;
	//	//this->GetSel(s,e);
	//	//if(s>0)
	//	//	this->SetSel(s-1,e);
	//	//OnChangeBegin();
	//	return;
	//}
	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	switch(nChar){
	case VK_UP:
	case VK_DOWN:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_LEFT:
	case VK_RIGHT:
		{
			OnChangeCaret();
		}
		break;
	}
	switch(nChar){
	case VK_DELETE:
	case VK_UP:
	case VK_DOWN:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_RETURN:
		CheckMemberTip();
	case VK_LEFT:
	case VK_RIGHT:
		{
			if(m_dlgEditor)m_dlgEditor->RedrawLines();
		}
		break;
	}
	if(nChar==L'S'&&HIBYTE(GetKeyState(VK_CONTROL)))
	{
		m_bModifyed = false;
		if(m_callback)
			this->m_callback->SaveFile();
		else
			this->SaveFile();
		this->m_dlgEditor->SetModifyed(this, false);
	}
}
void CodeEditor::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==8)return;
	switch(nChar){
	case 13:
	case 22://Ctrl+V
	case 24://Ctrl+X
	case 25://Ctrl+Y
	case 26://Ctrl+Z
	case 19:
	case 6:
	case 7:
	case 27:
		return;
	}
	if(m_bIsSelect&&nChar==13)return;
	long s,e;
	GetSel(s,e);
	if(m_lastUndoCaret!=s||!m_undoWritePtr||s!=e){
		m_undoWritePtr = new CCodeUndo();
		m_undoWritePtr->Begin(this);
		this->m_undos.push_back(m_undoWritePtr);
	}
	CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);
	GetSel(s,e);
	m_lastUndoCaret = s;
	m_undoWritePtr->End(this);
	OnChangeLine(m_undoWritePtr->m_oldLineTexts, m_undoWritePtr->m_oldLineBegin, m_undoWritePtr->m_newLineTexts, m_undoWritePtr->m_newLineBegin);
	//this->CheckLineColor(LineFromChar(s));
	if(this->m_listbox.IsWindowVisible())
	{
		if(nChar == L' ')
		{
			m_toolTip.ShowWindow(SW_HIDE);
			m_listbox.ShowWindow(SW_HIDE);
			//SelectFromListBox(nChar);
		}
	}
	int nLine = LineFromChar(s);
	if((nChar == L'.' || nChar == L':' || nChar == L'='))
	{
		long b,e;
		GetSel(b,e);
		CString str;
		GetTextRange(b-2,b-1,str);
		if(!(str==L"."||str==L":"))
			AutoPopup(nChar == L'=');
	}
	else if(nChar==L'(' || nChar==L','){
		this->AnalyseAllCodes();
		wchar_t buf[512];
		if(nChar==L',')
		{
			CDynamicArray<wchar_t> wcs;
			GetLineText(wcs, nLine);
			int begin = LineIndex(nLine);
			int nAdd = 0;
			for(int i=s;i>=begin;i--){
				int index  = i-begin;
				if(wcs[index]==L')')
					nAdd++;
				else if(wcs[index]==L'(')
				{
					nAdd--;
					if(nAdd<0)
					{
						s++;
						break;
					}
				}
				s--;
			}
		}
		int len = RequestCalls(buf, nLine, s, nChar);
		if(len>0){
			CCodeMember* m = this->m_codeMgr.GetFunctionMemberPtr(buf, nLine);
			if(m)
			{
				CPoint pos = PosFromChar(s);
				pos.y = PosFromChar(LineIndex(nLine+1)+1).y;
				ClientToScreen(&pos);
				SetMemberToolTip(m, pos, true);
			}
		}
	}
	else if(nChar==L')')
		m_toolTip.ShowWindow(SW_HIDE);
	if(issection(nChar))
		this->CheckLineColorRange(nLine, nLine);
	if(!m_bModifyed&&nChar!=3){
		m_bModifyed=true;
		if(m_callback)m_callback->SetModifyed();
		this->m_dlgEditor->SetModifyed(this, true);
	}
	m_bCharChanged = true;
	//
	if(!m_listbox.IsWindowVisible()){
		long b,e;
		GetSel(b,e);
		CString str;
		GetTextRange(b-2,b,str);
		BOOL bTip = false;
		if(str.GetLength()>1){
			wchar_t wch = str[0];
			wchar_t wn = toupper(nChar);
			if(!(HIBYTE(GetKeyState(VK_CONTROL))||HIBYTE(GetKeyState(VK_MENU))||HIBYTE(GetKeyState(VK_SHIFT))) && issection(wch)&&!(wch==L':'||wch==L'.')&&wn>=L'A'&&wn<=L'Z')
			{
				bTip = true;
			}
			else if(!issection(wn))
			{
				m_toolTip.ShowWindow(SW_HIDE);
				m_listbox.ShowWindow(SW_HIDE);
				bTip = false;
			}
		}
		if(bTip)
		{
			long end = 0;
			this->GetSel(m_startCombo, end);
			if(bTip)
				m_startCombo--;
			CPoint pos	=	PosFromChar(m_startCombo);
			CPoint popup=	CPoint(pos.x, pos.y + 20);
			GetCodeXmlManager()->ResetVariants();
			this->m_codeMgr.RefreshVariantList(LineIndex(m_startCombo));
			ClientToScreen(&popup);
			if(!m_listbox.AutoPopup(0, popup, nChar))
			{
				m_listbox.ShowWindow(SW_HIDE);
				SetFocus();
			}
		}
	}
}

VOID CodeEditor::OnChangeCaret()
{
	long b,e;
	GetSel(b,e);
	if(b==e){
		wchar_t name[128];
		if(GetThisCaretFunction(b, name)>=0){
			//this->m_lbFunctions.SelectString(0, name);
		}
		int el = b;//LineFromChar(b);
		if(m_undoCaret.size()==0){
			m_redoCaret.SetArrayCount(0);
			m_undoCaret.push_back(b);
			return;
		}
		int bl = m_undoCaret[m_undoCaret.size()-1];
		if(abs(bl-el)>=5)
		{
			m_redoCaret.SetArrayCount(0);
			m_undoCaret.push_back(b);
		}
	}
}
VOID CodeEditor::UndoCaret()
{
	if(m_undoCaret.size()>0){
		long c = m_undoCaret[m_undoCaret.size()-1];
		m_redoCaret.push_back(c);
		m_undoCaret.SetArrayCount(m_undoCaret.size()-1);
		SetSel(c,c);
	}
}
VOID CodeEditor::RedoCaret()
{
	if(m_redoCaret.size()>0){
		long c = m_redoCaret[m_redoCaret.size()-1];
		m_undoCaret.push_back(c);
		m_redoCaret.SetArrayCount(m_redoCaret.size()-1);
		SetSel(c,c);
	}
}
VOID CodeEditor::OnChangeLine(CDynamicArray<LineText>& remove, int beginRemoveLine, CDynamicArray<LineText>& insert, int beginInsertLine)
{
	for(int i=remove.size()-1;i>=0;i--)
		m_codeMgr.OnRemoveLine(i+beginRemoveLine);
	for(int i=0;i<insert.size();i++)
		m_codeMgr.OnInsertLine(beginInsertLine+i, insert[i].text);
	this->m_dlgEditor->RedrawLines();
}
VOID CodeEditor::RemoveLine(int line, const wchar_t* equip)
{
	CDynamicArray<wchar_t> buff;
	GetLineText(buff, line);
	assert(wcsicmp(buff.GetPtr(), equip) == 0);
	int begin = LineIndex(line);
	int end = LineIndex(line+1);
	int len = wcslen(equip);
	if(end>=0){
		assert((end-begin)==len);
	}
	SetSel(begin, end);
	ReplaceSel(L"");
}
VOID CodeEditor::InsertLine(int line, const wchar_t* text)
{
	int begin = LineIndex(line);
	SetSel(begin, begin);
	ReplaceSel(text);
}
BOOL CodeEditor::Undo()
{
	m_undoWritePtr = 0;
	if(m_undos.size()>0){
		CCodeUndo* temp = m_undos[m_undos.size()-1];//从最后一个进行撤销
		for(int i=temp->m_newLineTexts.size()-1;i>=0;i--){
			int line = i + temp->m_newLineBegin;
			RemoveLine(line, temp->m_newLineTexts[i].text);
		}
		for(int i=0;i<temp->m_oldLineTexts.size();i++){
			int line = i + temp->m_oldLineBegin;
			InsertLine(line, temp->m_oldLineTexts[i].text);
		}
		this->OnChangeLine(temp->m_newLineTexts, temp->m_newLineBegin, temp->m_oldLineTexts, temp->m_oldLineBegin);
		CheckLineColorRange(temp->m_oldLineBegin, temp->m_oldLineEnd);
		this->SetSel(temp->m_oldBegin, temp->m_oldEnd);
		m_undos.SetArrayCount(m_undos.size()-1);
		m_redos.push_back(temp);
		this->CompareAllLines();
		return true;
	}
	return false;
}
BOOL CodeEditor::Redo()
{
	m_undoWritePtr = 0;
	if(m_redos.size()>0){
		CCodeUndo* temp = m_redos[m_redos.size()-1];//从最后一个进行撤销
		for(int i=temp->m_oldLineTexts.size()-1;i>=0;i--){
			int line = i + temp->m_oldLineBegin;
			RemoveLine(line, temp->m_oldLineTexts[i].text);
		}
		for(int i=0;i<temp->m_newLineTexts.size();i++){
			int line = i + temp->m_newLineBegin;
			InsertLine(line, temp->m_newLineTexts[i].text);
		}
		this->OnChangeLine(temp->m_oldLineTexts, temp->m_oldLineBegin, temp->m_newLineTexts, temp->m_newLineBegin);
		CheckLineColorRange(temp->m_newLineBegin, temp->m_newLineEnd);
		this->SetSel(temp->m_newBegin, temp->m_newEnd);
		m_redos.SetArrayCount(m_redos.size()-1);
		m_undos.push_back(temp);
		this->CompareAllLines();
		return true;
	}
	return false;
}
VOID CodeEditor::ReplaceText(const wchar_t* text, int beginCaret)//替换选中文本
{
	BeginUndo();
	long s,e;
	GetSel(s,e);
	ReplaceSel(text);
	long s1,e1;
	GetSel(s1,e1);
	SetSel(s, e1);
	//
	AddThisUndo();
	//
	if(beginCaret>=0)
		SetSel(beginCaret,beginCaret);
	else
		SetSel(s1,e1);
	this->CheckLineColorRange(LineFromChar(s),LineFromChar(e1));
	//最后比较是否全部相同
	this->CompareAllLines();
	if(!m_bModifyed){
		if(m_callback)m_callback->SetModifyed();
		this->m_dlgEditor->SetModifyed(this, true);
	}
}
VOID CodeEditor::ClearRedos()
{
	for(int i=0;i<m_redos.size();i++)
	{
		delete m_redos[i];
	}
	m_redos.SetArrayCount(0);
}

VOID CodeEditor::AutoPopup(BOOL bEquip)
{
	long s,e;
	GetSel(s,e);
	if(s != e || s <= 0)
		return;
	this->AnalyseAllCodes();
	CString str;
	GetTextRange(s-1,s,str);
	int nChar	=	str[0];
	if((nChar == L'.' || nChar == L':' || nChar == L'='))// || nChar == L','))
	{
		if(bEquip)
			if(nChar != L'=')
				return;
		m_listbox.ShowWindow(SW_HIDE);
		long end = 0;
		this->GetSel(m_startCombo, end);
		CPoint pos	=	PosFromChar(m_startCombo);
		long begin	=	m_startCombo - 256;
		if(begin < 0)
			begin = 0;
		//
		int nLine	=	this->LineFromChar(m_startCombo);
		//
		wchar_t buf[512];
		int len = RequestCalls(buf, nLine, m_startCombo, nChar);
		if(len>0){
			CPoint popup=	CPoint(pos.x, pos.y + 20);
			ClientToScreen(&popup);
			CCodeClass* classPtr = m_codeMgr.GetVariantClassPtr(buf, nLine);
			if(classPtr){
				if(!m_listbox.AutoPopup(classPtr, popup, nChar))
				{
					m_listbox.ShowWindow(SW_HIDE);
					SetFocus();
				}
			}
		}
	}
}
int		CodeEditor::RequestCalls(wchar_t* ret, int nLine, int beginCaret, int nChar)//分析一段代码内的调用列表
{
	CDynamicArray<wchar_t> buff;
	int length = GetLineText(buff, nLine);
	if(length>=512)return 0;
	int bb = (beginCaret-LineIndex(nLine))-1;
	if(bb<=0)return 0;
	BOOL bIsEquip = false;
	if(buff[bb]==L'=')
	{
		if(bb<=0)return 0;
		bIsEquip = true;
	}
	buff[bb] = 0;
	CDynamicArray<MaskSection> secs;
	CCodeLine::SplitSection(0, buff.GetPtr(), bb, &secs);
	CCodeLine::RemoveSection(secs, L'(', L')', bb);
	CCodeLine::RemoveSection(secs, L'[', L']', bb);
	int beginsec = secs.size();
	bool bIsMustIf = false;
	if(bIsEquip&&secs.size()>0){
		if(secs[secs.size()-1].name.t[0]==L'=')
		{
			secs.eraseByIndex(secs.size()-1);
			bIsMustIf = true;
		}
	}
	bool bIsIf = false;
	for(int i=secs.size()-1;i>=0;i--){
		if(secs[i].type==MaskSection::Section){
			if(!(secs[i].name[0]==L':'||secs[i].name[0]==L'.'))
			{
				break;
			}
			else if(i>0&&secs[i].name[0]==L'.'&&secs[i-1].name[0]==L'.')
				break;
		}
		if(secs[i].name==L"if"||secs[i].name==L"elseif"||secs[i].name==L"do"||secs[i].name==L"return")
		{
			bIsIf = true;
			break;
		}
		else if(secs[i].name==L"for")
			break;
		beginsec = i;
	}
	if(bIsIf&&bIsEquip&&!bIsMustIf)
		return 0;
	ret[0] = 0;
	for(int i=beginsec;i<secs.size();i++){
		wcscat(ret, secs[i].name);
		if(wcslen(ret)>256)
			return false;
	}
	return wcslen(ret);
}
VOID	CodeEditor::AutoShowListBox()
{
	long start = 0, end = 0;
	this->GetSel(start, end);
	if(start < this->m_startCombo)
	{
		this->m_listbox.ShowWindow(SW_HIDE);
		m_toolTip.ShowWindow(SW_HIDE);
		SetFocus();
		this->RedrawWindow();
		this->Invalidate();
		return;
	}
	if(m_startCombo != start)
	{
		CString str;
		this->GetTextRange(m_startCombo, start, str);
		str.MakeLower();
		m_listbox.AutoSelect(str);
	}
}
BOOL CodeEditor::SelectFromListBox(int nChar)
{
	if(!m_listbox.IsWindowVisible())
		return false;
	if(m_listbox.GetCurSel() >= 0)
	{
		CString str;
		m_listbox.GetCodeText(str);
		if(m_listbox.m_bIsString&&str[0] == L'.'&&nChar==190)
		{
		}
		else{
			long start = 0, end = 0;
			this->GetSel(start, end);
			if(start >= this->m_startCombo)
			{
				this->SetSel(m_startCombo > 0 ? (m_listbox.m_bIsGlobal?m_startCombo:m_startCombo - 1) : 0, start);
				this->ReplaceText(str);
			}
		}
	}
	m_toolTip.ShowWindow(SW_HIDE);
	m_listbox.ShowWindow(SW_HIDE);
	SetFocus();
	this->RedrawWindow();
	this->Invalidate();
	return true;
}
VOID CodeEditor::CheckMemberTip()//检查是否要显示函数说明
{
	long s,e;
	GetSel(s,e);
	int lineIndex = LineFromChar(m_memberCaret);
	if(s<m_memberCaret||s>=LineIndex(lineIndex+1))
	{
		m_toolTip.ShowWindow(SW_HIDE);
	}
}
VOID CodeEditor::SetMemberToolTip(CCodeMember* m, const CPoint& pos, BOOL bAlwaysShow)
{
	wchar_t buf[2048];
	buf[0]	=	0;
	switch(m->type)
	{
	case CCodeMember::CodeMemberType_Class:
		swprintf(buf, Language(L"类：%s\r\n构造函数：%s\r\n参数列表：%s\r\n说明：%s\r\n"), m->returnClass.str(), m->name.str(), m->args.str(), m->description.str());
		break;
	case CCodeMember::CodeMemberType_Module:
		swprintf(buf, Language(L"模块：%s\r\n指向变量：%s\r\n说明：%s\r\n"), m->returnClass.str(), m->name.str(), m->description.str());
		break;
	case CCodeMember::CodeMemberType_Struct:
		swprintf(buf, Language(L"%s"), m->args.str());
		break;
	case CCodeMember::CodeMemberType_Void:
	case CCodeMember::CodeMemberType_Table:
		//swprintf(buf, Language(L"类：%s\r\n类说明：%s\r\n函数：%s\r\n参数列表：%s\r\n返回值：%s\r\n说明：%s\r\n"), m->classPtr->name.str(), m->classPtr->description.c_str(), m->name.str(), m->args.c_str(), m->returnClass.str(), m->description.c_str());
		swprintf(buf, Language(L"%s\r\n%s"), m->args.str(), m->description.str());
		break;
	case CCodeMember::CodeMemberType_Variant:
	case CCodeMember::CodeMemberType_Keywords:
		swprintf(buf, Language(L"%s"), m->description.str());
		break;
	case CCodeMember::CodeMemberType_Enum:
		swprintf(buf, Language(L"枚举：%s\r\n枚举说明：%s\r\n"), m->name.str(), m->description.str());
		break;
	case CCodeMember::CodeMemberType_Enum_Value:
		swprintf(buf, Language(L"枚举：%s\r\n枚举说明：%s\r\n值：%s = %s\r\n说明：%s\r\n"), m->returnClass.str(), m->classPtr->description.str(), m->name.str(), m->returnClass.str(), m->description.str());
		break;
	default:
		m_toolTip.ShowWindow(SW_HIDE);
		return;
	}
	int len = wcslen(buf);
	for(int i=0;i<len;i++){
		if(buf[i]==L'\t')
			buf[i] = L' ';
	}
	m_toolTip.ShowToolTip(this, buf, pos, bAlwaysShow?10000000:10000);
}

VOID CodeEditor::SetCharPosTip(const CPoint& pos)
{
	long start		=	this->CharFromPos(pos);
	int line		=	this->LineFromChar(start);
}
VOID CodeEditor::CheckToolTip(int begin)
{
	wchar_t tipBuf[1024];
	tipBuf[0] = 0;
	if(begin>=0&&begin<GetTextLength()){
		this->AnalyseAllCodes();
		int line = LineFromChar(begin);
		if(line>=0&&line<GetLineCount()){
			CDynamicArray<MaskSection> secs;
			CDynamicArray<wchar_t> buff;
			int length = GetLineText(buff, line);
			CCodeLine::SplitSection(0, buff.GetPtr(), length, &secs);
			int lineBegin = LineIndex(line);
			for(int i=0;i<secs.size();i++){
				int b = secs[i].begin+lineBegin;
				if((begin-b)<secs[i].count)
				{
					if(secs[i].type==MaskSection::Normal){
						wchar_t buf[512];
						int len = RequestCalls(buf, line, lineBegin+secs[i].begin+secs[i].count+1, 0);
						if(len>0){
							CCodeMember* m = this->m_codeMgr.GetFunctionMemberPtr(buf, line+1);
							if(m)
							{
								wcscpy(tipBuf, m->args.str());
								if(tipBuf[0]!=0)
									wcscat(tipBuf, L"\r\n");
								wcscat(tipBuf, m->description.str());
								int ll=wcslen(tipBuf);
								for(int i=0;i<ll;i++){
									if(tipBuf[i]==L'\t')
										tipBuf[i] = L' ';
								}
							}
						}
					}
					break;
				}
			}
		}
	}
	//if(m_lastTip==tipBuf)
	//	return;
	m_lastTip = tipBuf;
	m_dlgEditor->UpdateTipText(m_lastTip, this);
	m_dlgEditor->OffsetToolRect(100, 10);
}
void CodeEditor::OnMButtonDown(UINT nFlags, CPoint point)
{
	CRichEditCtrl::OnMButtonDown(nFlags, point);
	SetFocus();
}
void CodeEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	long s,e;
	GetSel(s,e);
	CRichEditCtrl::OnLButtonDown(nFlags, point);
	//
	CheckMemberTip();
	long begin,end;
	GetSel(begin,end);
	m_dblStart = m_dblEnd = -1;
	long s1,e1;
	GetSel(s1,e1);
	if(s1!=s||e1!=e)
		if(m_dlgEditor)m_dlgEditor->RedrawLines();
	CCodeFunction* fn = m_codeMgr.GetFunctionInLine(LineFromChar(s1));
	if(fn)this->m_cbMethods.SelectString(0, fn->m_name.t);
	if(this->m_listbox.IsWindowVisible())
	{
		m_toolTip.ShowWindow(SW_HIDE);
		m_listbox.ShowWindow(SW_HIDE);
		SetFocus();
	}
	else
	{
		this->SetCharPosTip(point);
	}
	if(m_bCharChanged){
		m_bCharChanged = false;
		this->CheckLineColorRange(LineFromChar(s), LineFromChar(s));
	}
	if(s1!=e1&&!HIBYTE(GetKeyState(VK_SHIFT)))
		SetSel(s1,s1);
}
void CodeEditor::OnKillFocus(CWnd* pNewWnd)
{
	CRichEditCtrl::OnKillFocus(pNewWnd);
	if(this->m_listbox.IsWindowVisible())
	{
		m_toolTip.ShowWindow(SW_HIDE);
		m_listbox.ShowWindow(SW_HIDE);
	}
	if(m_toolTip.IsWindowVisible())
		m_toolTip.ShowWindow(SW_HIDE);
}
void CodeEditor::OnSetFocus(CWnd* pNewWnd)
{
	CRichEditCtrl::OnSetFocus(pNewWnd);
}
VOID	CodeEditor::CompareAllLines()//比较所有行，代码和richedit是否一致
{
	int nLine = this->GetLineCount();
	assert(m_codeMgr.m_lines.size()==nLine);
	if(m_codeMgr.m_lines.size()!=nLine)
	{
		int line = nLine;
		m_codeMgr.Reset();
		CDynamicArray<wchar_t> buff;
		for(int i=0;i<line;i++)
		{
			GetLineText(buff, i);
			m_codeMgr.OnInsertLine(i, buff.GetPtr());
		}
	}
	CDynamicArray<wchar_t> buff;
	for(int i=0;i<m_codeMgr.m_lines.size();i++)
	{
		GetLineText(buff, i);
		assert(wcsicmp(m_codeMgr.m_lines[i]->m_text, buff.GetPtr()) == 0);
	}
}
VOID	CodeEditor::SetInitText(const wchar_t* code, const char* szFile)
{
	this->m_fileName = szFile;
	for(int i=0;i<m_undos.size();i++)
		delete m_undos[i];
	m_undos.SetArrayCount(0);
	this->ClearRedos();
	this->ReplaceSel(code);
	this->CheckLineColor(-1,-1);
	int line = this->GetLineCount();
	m_codeMgr.Reset();
	CDynamicArray<wchar_t> buff;
	for(int i=0;i<line;i++)
	{
		GetLineText(buff, i);
		m_codeMgr.OnInsertLine(i, buff.GetPtr());
	}
	CompareAllLines();
	line = this->GetLineCount();
	this->AnalyseAllCodes();
	line = this->GetLineCount();
}
VOID	CodeEditor::OnComboSelect(CWnd* combo, const wchar_t* text, int data)
{
	if(combo != &m_cbEvents){
		int begin = this->LineIndex(data);
		SetSel(begin+256, begin+256);
		SetSel(begin, begin);
		this->SetFocus();
		if(m_dlgEditor)m_dlgEditor->RedrawLines();
		return;
	}
	for(int i=0;i<m_codeMgr.m_functions.size();i++)
	{
		CCodeFunction* fun= m_codeMgr.m_functions[i];
		if(wcsicmp(fun->m_name, text) == 0){
			int begin = this->LineIndex(fun->m_lineBegin);
			SetSel(begin+256, begin+256);
			SetSel(begin, begin);
			this->SetFocus();
			if(m_dlgEditor)m_dlgEditor->RedrawLines();
			return;
		}
	}
	const wchar_t* fulls = 0;
	for(int i=0;i<m_codeMgr.m_events.size();i++)
	{
		MaskEvent& var = m_codeMgr.m_events[i];
		if(wcsicmp(var.name, text) == 0){
			fulls = var.fulls;
			break;
		}
	}
	if(fulls==0)return;
	if(combo==&m_cbEvents){//如果是选择事件
		int begin = GetTextLength();
		SetSel(begin,begin);
		std::wstring wbuf;
		wbuf = L"\n";
		wbuf += fulls;
		wbuf += L"\n";
		begin += wbuf.size();
		wbuf += L"\nend\n";
		ReplaceText(wbuf.c_str());
		this->AnalyseAllCodes();
		SetSel(begin, begin);
		this->SetFocus();
	}
}
//int	OnEventSort(const void* v1, const void* v2){
//	MaskEvent* m1 = (MaskEvent*)v1;
//	MaskEvent* m2 = (MaskEvent*)v2;
//	if(m1->bExist>m2->bExist)
//		return -1;
//	else if(m1->bExist<m2->bExist)
//		return 1;
//	return wcsicmp(m1->name, m2->name);
//}

VOID	CodeEditor::RefreshCombos()//刷新组合框数据
{
	int seldata = -1;
	int sel = m_cbMethods.GetCurSel();
	if(sel>=0)
		seldata = m_cbMethods.m_items[sel]->data;
	m_cbMethods.ResetContent();
	for(int i=0;i<m_codeMgr.m_functions.size();i++)
	{
		int index = m_cbMethods.AddString(m_codeMgr.m_functions[i]->m_name, CCodeMember::CodeMemberType_Void, RGB(255,255,255), RGB(20,20,20), m_codeMgr.m_functions[i]->m_lineBegin);
		if(seldata==m_codeMgr.m_functions[i]->m_lineBegin)
			m_cbMethods.SetCurSel(index);
	}
	seldata = -1;
	sel = m_cbEvents.GetCurSel();
	if(sel>=0)
		seldata = m_cbEvents.m_items[sel]->data;
	m_cbEvents.ResetContent();
	COLORREF textColor = RGB(0, 0, 0);
	COLORREF existTextColor = RGB(255, 0, 0);
	COLORREF bgColor = RGB(200, 200, 200);
	COLORREF existbgColor = RGB(255, 255, 255);
	for(int i=0;i<m_codeMgr.m_events.size();i++)
	{
		int index = m_cbEvents.AddString(m_codeMgr.m_events[i].name, CCodeMember::CodeMemberType_Event, m_codeMgr.m_events[i].bExist?existbgColor:bgColor, m_codeMgr.m_events[i].bExist?existTextColor:textColor, i);
		if(seldata==i)
			m_cbEvents.SetCurSel(index);
	}
	//m_cbEvents.AddString(L"oninit", CCodeMember::CodeMemberType_Event, 
}

void CodeEditor::OnPaint()
{
	if(!m_bPaintEnable)
		return;
	CRichEditCtrl::OnPaint();
}

BOOL CodeEditor::OnEraseBkgnd(CDC* pDC)
{
	if(!m_bPaintEnable)
		return true;
	return CRichEditCtrl::OnEraseBkgnd(pDC);
}

void CodeEditor::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(this->m_dlgEditor)
		m_dlgEditor->RedrawLines();

	CRichEditCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CodeEditor::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(this->m_dlgEditor)
		m_dlgEditor->RedrawLines();
	CRichEditCtrl::OnMouseHWheel(nFlags, zDelta, pt);
}


void CodeEditor::OnEnVscroll()
{
	if(this->m_dlgEditor)
		m_dlgEditor->RedrawLines();
}
int		CodeEditor::GetLineText(CDynamicArray<wchar_t>& buff, int lineIndex)//返回一行文本
{
	int length = this->LineLength(lineIndex);
	if(length>=0){
		if(length<1024)length=1024;
		buff.SetArrayCount(length);
		length = this->GetLine(lineIndex, buff.GetPtr(), length);
		if(LineIndex(lineIndex+1)==-1&&length>0)
			length--;
		assert(length>=0);
		buff.GetPtr()[length] = 0;
	}
	return length;
}
VOID		CodeEditor::BeginUndo()
{
	m_undoWritePtr = 0;
	if(m_undoPtr!=0){
		AddThisUndo();
		//assert(m_undoPtr==0);
	}
	m_undoPtr = new CCodeUndo();
	m_undoPtr->Begin(this);
}
VOID		CodeEditor::AddThisUndo()
{
	if(m_undoPtr)
	{
		ClearRedos();
		m_undoPtr->End(this);
		this->m_undos.push_back(m_undoPtr);
		//添加代码变动
		this->OnChangeLine(m_undoPtr->m_oldLineTexts, m_undoPtr->m_oldLineBegin, m_undoPtr->m_newLineTexts, m_undoPtr->m_newLineBegin);
		//
	}
	m_undoPtr = 0;
}
LRESULT CodeEditor::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_IME_STARTCOMPOSITION)
	{
		BeginUndo();
	}
	if (message == WM_IME_ENDCOMPOSITION)
	{
		if(m_compositionStr.GetLength()>0||m_saveIMEStr.GetLength()>0){//如果有任何改变
			if(m_undoPtr)
			{
				int s = m_undoPtr->m_oldBegin;
				int e = m_undoPtr->m_newEnd;
				AddThisUndo();
				this->CheckLineColorRange(LineFromChar(s),LineFromChar(e));
			}
		}
	}
	if (message == WM_IME_COMPOSITION)
	{
		if (lParam & GCS_RESULTSTR)
		{
			//Get result Composition String from IME 
			HIMC hIMC;
			HWND hWnd;
			DWORD dwSize;
			HGLOBAL hstr;
			char * lpstr;
			LOGFONT lplf;
			hWnd = m_hWnd;
			hIMC = ImmGetContext(hWnd);
			if (!hIMC)
				return CRichEditCtrl::DefWindowProc( message, wParam, lParam );
			dwSize = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);
			hstr = GlobalAlloc(GHND,dwSize);
			if (hstr == NULL)
				return CRichEditCtrl::DefWindowProc( message, wParam, lParam );
			lpstr = (char *)GlobalLock(hstr);
			if (lpstr == NULL)
				return CRichEditCtrl::DefWindowProc( message, wParam, lParam );
			ImmGetCompositionString(hIMC, GCS_RESULTSTR, lpstr, dwSize);
			ImmReleaseContext(hWnd, hIMC);
			UINT IMEChar;
			for (int i = 0; i < dwSize/sizeof(WCHAR); i++)
			{
				IMEChar = lpstr[ i*2 ] + lpstr[ i*2+1 ] * 256;
				m_compositionStr = m_compositionStr + (TCHAR)IMEChar;
			}
			GlobalUnlock(hstr);
			GlobalFree(hstr);
		}
	}
	return CRichEditCtrl::DefWindowProc(message, wParam, lParam);
}
void CodeEditor::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	long beginch = this->CharFromPos(this->GetCaretPos());
	long begin = this->LineFromChar(beginch);
	if(begin>=0){
		this->CompareAllLines();//比较所有行，纠错
		long beginl = LineIndex(begin);
		CCodeLine* line = this->m_codeMgr.GetLine(begin);
		CDynamicArray<MaskSection>& temp = line->m_fulls;
		long bl = beginch - beginl;
		for(int i=0;i<temp.size();i++)
		{
			long c = bl - temp[i].begin;
			if(c >= 0 && c < temp[i].count)
			{
				long tb = temp[i].begin;
				long te = temp[i].count + temp[i].begin;
				if(temp[i].count==1&&issection(temp[i].name[0])){//如果是分段，左右扩散至所有分段
					for(int j=i-1;j>=0;j--){
						if(temp[j].count==1&&issection(temp[j].name[0]))
							tb = temp[j].begin;
						else
							break;
					}
					for(int j=i+1;j<temp.size();j++){
						if(temp[j].count==1&&issection(temp[j].name[0]))
							te = temp[j].count + temp[j].begin;
						else
							break;
					}
				}
				if(temp[i].name[0]==L'\"')
				{
					tb++;
					te--;
				}
				m_dblStart = tb+beginl;
				m_dblEnd = te+beginl;
				CString str;
				GetTextRange(m_dblStart,m_dblEnd, str);
				if(str.GetLength()>0&&str[str.GetLength()-1]==L'\r')
					m_dblEnd--;
				this->SetSel(m_dblStart, m_dblEnd);
				return;
			}
		}
	}
	CRichEditCtrl::OnLButtonDblClk(nFlags, point);
}

void CodeEditor::OnTimer(UINT_PTR nIDEvent)
{
	m_rebuildCode++;
	if(m_bIsChanged){
		m_bIsChanged = false;
		if(m_rebuildCode>=10){
			m_rebuildCode = 0;
			this->AnalyseAllCodes();
		}
	}
	if(m_bWaitTip<500){
		m_bWaitTip += 100;
		if(m_bWaitTip>=500){
			CPoint pos;
			GetCursorPos(&pos);
			ScreenToClient(&pos);
			if(pos.x!=m_lastPos.x||pos.y!=m_lastPos.y)
			{
				this->CheckToolTip(CharFromPos(pos));
				m_lastPos = pos;
			}
		}
	}
	CRichEditCtrl::OnTimer(nIDEvent);
}

int	CodeEditor::GetThisCaretFunction(int beginCaret, wchar_t* buf)//返回这个光标函数所在的第一行，并且返回函数名字
{
	return -1;
}

VOID CodeEditor::AnalyseAllCodes()//自动计算并生成函数和变量列表
{
	for(int i=0;i<g_codeEditors.size();i++){
		g_codeEditors[i]->m_codeMgr.ClearAllLines();
	}
	GetCodeXmlManager()->ResetBaseClass();
	for(int i=0;i<g_codeEditors.size();i++){
		if(g_codeEditors[i]!=this)
			g_codeEditors[i]->m_codeMgr.AnalyseAllLines();
	}
	m_codeMgr.AnalyseAllLines();
	RefreshCombos();
}


void CodeEditor::OnMouseMove(UINT nFlags, CPoint point)
{
	long beginch = this->CharFromPos(point);
	long begin = this->LineFromChar(beginch);
	if(begin>=0&&nFlags==1&&m_dblStart>=0&&m_dblEnd>m_dblStart){
		this->CompareAllLines();//比较所有行，纠错
		long beginl = LineIndex(begin);
		CCodeLine* line = this->m_codeMgr.GetLine(begin);
		CDynamicArray<MaskSection>& temp = line->m_fulls;
		long bl = beginch - beginl;
		for(int i=0;i<temp.size();i++)
		{
			long c = bl - temp[i].begin;
			if(c >= 0 && c < temp[i].count)
			{
				long tb = temp[i].begin;
				long te = temp[i].count + temp[i].begin;
				if(temp[i].count==1&&issection(temp[i].name[0])){//如果是分段，左右扩散至所有分段
					for(int j=i-1;j>=0;j--){
						if(temp[j].count==1&&issection(temp[j].name[0]))
							tb = temp[j].begin;
						else
							break;
					}
					for(int j=i+1;j<temp.size();j++){
						if(temp[j].count==1&&issection(temp[j].name[0]))
							te = temp[j].count + temp[j].begin;
						else
							break;
					}
				}
				//if(temp[i].name[0]==L'\"')
				//{
				//	tb++;
				//	te--;
				//}
				int dblStart = tb+beginl;
				int dblEnd = te+beginl;
				if(dblStart>m_dblStart)
					dblStart = m_dblStart;
				else if(dblEnd<m_dblEnd)
					dblEnd = m_dblEnd;
				CString str;
				GetTextRange(m_dblStart,m_dblEnd, str);
				if(str.GetLength()>0&&str[str.GetLength()-1]==L'\r')
					dblEnd--;
				this->SetSel(dblStart,dblEnd);
				return;
			}
		}
	}
	m_bWaitTip = 0;
	CRichEditCtrl::OnMouseMove(nFlags, point);
}


void CodeEditor::OnLButtonUp(UINT nFlags, CPoint point)
{
	long s,e;
	GetSel(s,e);
	m_dblEnd=m_dblStart=-1;
	if(s!=e && PosFromChar(e-1).x<point.x){//LineFromChar(s)==LineFromChar(e)){
		CString str = GetSelText();
		if(str.GetLength()>0&&str[str.GetLength()-1]==L'\r')
			SetSel(s,e-1);
	}
	CRichEditCtrl::OnLButtonUp(nFlags, point);
}
