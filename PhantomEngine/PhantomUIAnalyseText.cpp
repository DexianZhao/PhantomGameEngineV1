//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIAnalyseText.h"
#include "PhantomUIStaticFormat.h"
#include "PhantomUIManager.h"
#include "PhantomManager.h"

namespace Phantom{


UIAnalyseText::UIAnalyseText(void)
{
}

UIAnalyseText::~UIAnalyseText(void)
{
}


struct FunctionText
{
	int						type;
	int						parameter;
	int						color;
	std::string				str;
	std::string				url;
	AnalyseTextParam*				analyse;
	std::vector<std::string>	parameters;
	Name					cursor;
	FunctionText(){type = 0; parameter = 0; analyse = 0;}
	enum{
		TEXT_TYPE_COLOR_SET			=	1,
		TEXT_TYPE_COLOR_END			=	2,
		TEXT_TYPE_REF_SHOW_HELP		=	3,
		TEXT_TYPE_REF_END			=	4,
		TEXT_TYPE_INSERT_IMAGE		=	5,
		TEXT_TYPE_FONT_SET			=	6,
		TEXT_TYPE_FONT_END			=	7,
		//
		TEXT_TYPE_CUSTOM_BASE		=	100,
	};
};

int		analyseChars(const char* buf, int length, std::string& retName, std::vector<std::string>& params)
{
	char function[1024];
	char name[1024];
	params.clear();
	int len		=	length;
	int begin	=	0;
	int beginParam	=	0;
	int name_begin	=	0;
	int end		=	0;
	bool bFirst	=	false;
	bool bName	=	false;
	for(int i=0;i<len;i++)
	{
		if(!bFirst)
			if(buf[i] == L' ')
				continue;
		bFirst	=	true;
		if(buf[i] == L'\"')
			continue;
		function[begin]	=	buf[i];
		if(!bName)
		{
			if(function[begin] == L':' || function[begin] == L' ' || function[begin] == L'>')
			{
				strncpy(name, function, begin);
				name[begin]	=	0;
				retName		=	name;
				bName		=	true;
				begin		=	-1;
			}
		}
		else
		{
			if(function[begin] == L',' || function[begin] == L' ' || function[begin] == L'>')
			{
				strncpy(name, function, begin);
				name[begin]	=	0;
				params.push_back(name);
				begin		=	-1;
			}
		}
		if(buf[i] == L'>')
			return i + 1;
		begin++;
	}
	return 0;
}

unsigned char	hex_analyse(char ch)
{
	if(ch >= '0' && ch <= '9')
		return ch - '0';
	if(ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	if(ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	return 0;
}
int		FromHex(const char* ch)
{
	int len	=	strlen(ch);
	char hex[8];
	memset(hex, 0, sizeof(hex));
	int index	=	0;
	for(int i=len - 1;i>=0;i--)
	{
		if(i < 8)
			hex[index++] = ch[i];
	}
	int ret	=	0;
	for(int i=0;i<8;i++)
	{
		ret |= (hex_analyse(hex[i]) << (i * 4));
	}
	return ret;
}

void	UIAnalyseText::analyseFunction(char* wbuffer, std::vector<FunctionText>& ret, int length)
{CPUTime(UIAnalyseText);
	ret.clear();
	const int max_void_length	=	50;
	int begin					=	0;
	char* buf				=	new char[length + 1];//[5000];
	for(int i=0;i<=length;i++)
	{
		if(wbuffer[i] == L'\r')
			wbuffer[i] = L' ';
		if(wbuffer[i] == L'\\')
		{
			if(wbuffer[i + 1] == L'n' || wbuffer[i + 1] == L'N')
			{
				wbuffer[i] = L' ';
				wbuffer[i + 1] = L'\n';
				continue;
			}
			else if(wbuffer[i + 1] == L't' || wbuffer[i + 1] == L'T')
			{
				wbuffer[i] = L' ';
				wbuffer[i + 1] = L'\t';
				continue;
			}
		}
		else if(wbuffer[i] == L'<' || i == length)
		{
			//
			int le	=	i - begin;
			if(le > 0)
			{
				strncpy(buf, wbuffer + begin, le);
				buf[le] = 0;

				FunctionText tstr;
				tstr.str	=	buf;
				tstr.type	=	0;
				ret.push_back( tstr );
			}
			if(i == length)
				break;
			//
			std::string name;
			std::vector<std::string> params;
			int count	=	analyseChars(wbuffer + i + 1, max_void_length, name, params);
			if(count > 0)
			{
				//
				i		+=	count;
				begin	=	i + 1;
				//
				for(size_t i=0;i<m_analyses.size();i++)
				{
					if(stricmp(m_analyses.at(i).function_name.c_str(), name.c_str()) == 0)
					{
						FunctionText t;
						t.type		=	t.TEXT_TYPE_CUSTOM_BASE;
						t.analyse	=	&m_analyses.at(i);
						ret.push_back( t );
						break;
					}
				}
				/*if(stricmp(name.c_str(), "user_name") == 0)
				{
					FunctionText t;
					t.type	=	t.TEXT_TYPE_USER_NAME;
					ret.push_back( t );
				}
				else if(stricmp(name.c_str(), "dest_text") == 0)
				{
					FunctionText t;
					t.type	=	t.TEXT_TYPE_DEST_NAME;
					ret.push_back( t );
				}
				else */
				if(stricmp(name.c_str(), "c") == 0 || stricmp(name.c_str(), "color") == 0)
				{
					FunctionText t;
					t.type	=	t.TEXT_TYPE_COLOR_SET;
					if(params.size() == 3)
					{
						t.color	=	COLOR_ARGB(255, atoi(params.at(2).c_str()), atoi(params.at(1).c_str()), atoi(params.at(0).c_str()));
					}
					else if(params.size() == 1)
					{
						int r	=	FromHex(params.at(0).c_str());
						t.color	=	COLOR_ARGB(255, r, r, r);
					}
					else
					{
						t.color	=	0;
					}
					ret.push_back( t );
				}
				else if(stricmp(name.c_str(), "/c") == 0 || stricmp(name.c_str(), "/color") == 0)
				{
					FunctionText t;
					t.type	=	t.TEXT_TYPE_COLOR_END;
					ret.push_back( t );
				}
				else if(stricmp(name.c_str(), "image") == 0 || stricmp(name.c_str(), "img") == 0)
				{
					if(params.size() > 0)
					{
						FunctionText t;
						t.type	=	t.TEXT_TYPE_INSERT_IMAGE;
						t.url	=	params.at(0).c_str();
						t.str	+=	" ";
						if(params.size() > 1)
							t.cursor	=	params.at(1).c_str();
						ret.push_back( t );
					}
				}
				else if(stricmp(name.c_str(), "f") == 0 || stricmp(name.c_str(), "font") == 0)
				{
					FunctionText t;
					t.type	=	t.TEXT_TYPE_FONT_SET;
					if(params.size() == 2)
					{
						t.parameter	=	GetDlgManager()->AddFont(0, atoi(params.at(1).c_str()), atoi(params.at(0).c_str()));
					}
					else if(params.size() == 1)
					{
						t.parameter	=	GetDlgManager()->AddFont(0, 12.0f, atoi(params.at(0).c_str()));
					}
					else
					{
						t.parameter	=	0;
					}
					ret.push_back( t );
				}
				else if(stricmp(name.c_str(), "/font") == 0 || stricmp(name.c_str(), "/f") == 0)
				{
					FunctionText t;
					t.type	=	t.TEXT_TYPE_FONT_END;
					t.parameter	=	0;
					ret.push_back( t );
				}
				else if(stricmp(name.c_str(), "b") == 0 || stricmp(name.c_str(), "bold") == 0)
				{
					FunctionText t;
					t.type	=	t.TEXT_TYPE_FONT_SET;
					t.parameter	=	GetDlgManager()->AddFont(0, 12, 1);
					ret.push_back( t );
				}
				else if(stricmp(name.c_str(), "/b") == 0 || stricmp(name.c_str(), "/bold") == 0)
				{
					FunctionText t;
					t.type	=	t.TEXT_TYPE_FONT_END;
					t.parameter	=	0;
					ret.push_back( t );
				}
				else if(stricmp(name.c_str(), "refB") == 0 || stricmp(name.c_str(), "e") == 0)
				{
					if(params.size() > 0)
					{
						FunctionText t;
						t.type	=	t.TEXT_TYPE_REF_SHOW_HELP;
						t.parameter	=	atoi(params.at(0).c_str());
						if(params.size() > 1)
							t.color	=	FromHex(params.at(1).c_str());
						else
							t.color	=	0x000000ff;
						if(params.size() > 2)
							t.cursor	=	params.at(2).c_str();
						ret.push_back( t );
					}
				}
				else if(stricmp(name.c_str(), "refE") == 0 || stricmp(name.c_str(), "/e") == 0)
				{
					if(params.size() == 0)
					{
						FunctionText t;
						t.type	=	t.TEXT_TYPE_REF_END;
						ret.push_back( t );
					}
				}
			}
			else
			{
				begin	=	i;
			}
		}
	}
	delete[] buf;
}

const char*		UIAnalyseText::analyseFunctionText(std::vector<FunctionText>& texts, CSpeakParameter* sp)
{CPUTime(UIAnalyseText);
	sp->RemoveAll();
	static std::string wstr;
	wstr = "";

	int	beginBold		=	0;
	bool boldSet		=	false;
	int fontID			=	0;

	int beginColor		=	0;
	bool beginSet		=	false;
	int color			=	0;
	//
	int ref_beginColor		=	0;
	bool ref_beginSet		=	false;
	Name	ret_cursor;
	std::string	ref_url;
	int eventID			=	0;
	int	ref_color		=	0;
	//
	for(size_t i=0;i<texts.size();i++)
	{
		FunctionText& t = texts.at(i);
		switch(t.type)
		{
		case FunctionText::TEXT_TYPE_CUSTOM_BASE:
			{
				if(!t.analyse)
					continue;
				if(t.analyse->dest_text.size() > 0)
				{
					int nbegin	=	wstr.size();
					wstr		+=	t.analyse->dest_text.c_str();
					SpeakParameter c;
					c.begin	=	nbegin;
					c.type	=	SpeakParameter::SP_TYPE_COLOR;
					c.count	=	(int)wstr.size() - nbegin;
					c.c		=	t.analyse->color;
					sp->AddParameter( c );
				}
			}
			break;
		//case FunctionText::TEXT_TYPE_USER_NAME:
		//	{
		//		int nbegin	=	wstr.size();
		//		wstr += p.user_name.c_str();
		//		//
		//		SpeakParameter c;
		//		c.begin	=	nbegin;
		//		c.type	=	SpeakParameter::SP_TYPE_COLOR;
		//		c.count	=	(int)wstr.size() - nbegin;
		//		c.c		=	0xffffff00;
		//		colors.push_back( c );
		//	}
		//	break;
		case FunctionText::TEXT_TYPE_INSERT_IMAGE:
			{
				int nbegin	=	wstr.size();
				wstr	+=	t.str;
				//
				SpeakParameter c;
				c.begin	=	nbegin;
				c.type	=	SpeakParameter::SP_TYPE_IMAGE;
				c.count	=	(int)wstr.size() - nbegin;
				c.url	=	t.url.c_str();
				c.cursor	=	t.cursor.t;
				sp->AddParameter( c );
			}
			break;
		case FunctionText::TEXT_TYPE_COLOR_SET:
			{
				if(!beginSet)
				{
					beginSet	=	true;
					color		=	t.color;
					beginColor	=	(int)wstr.size();
				}
				else
				{
					SpeakParameter c;
					c.type	=	SpeakParameter::SP_TYPE_COLOR;
					c.begin	=	beginColor;
					c.count	=	(int)wstr.size() - beginColor;
					c.c		=	0xff000000 | color;
					sp->AddParameter( c );
					beginColor	=	wstr.size();
				}
			}
			break;
		case FunctionText::TEXT_TYPE_COLOR_END:
			{
				if(beginSet)
				{
					SpeakParameter c;
					c.type	=	SpeakParameter::SP_TYPE_COLOR;
					c.begin	=	beginColor;
					c.count	=	(int)wstr.size() - beginColor;
					c.c		=	0xff000000 | color;
					sp->AddParameter( c );
					//
					beginSet	=	false;
				}
				beginSet	=	false;
			}
			break;
		case FunctionText::TEXT_TYPE_FONT_SET:
			{
				if(!boldSet)
				{
					boldSet		=	true;
					beginBold	=	(int)wstr.size();
					fontID		=	t.parameter;
				}
				else
				{
					SpeakParameter c;
					c.type	=	SpeakParameter::SP_TYPE_FONT;
					c.begin	=	beginColor;
					c.count	=	(int)wstr.size() - beginColor;
					c.c		=	fontID;
					sp->AddParameter( c );
					beginColor	=	wstr.size();
				}
			}
			break;
		case FunctionText::TEXT_TYPE_FONT_END:
			{
				if(boldSet)
				{
					SpeakParameter c;
					c.type	=	SpeakParameter::SP_TYPE_FONT;
					c.begin	=	beginBold;
					c.count	=	(int)wstr.size() - beginBold;
					c.c		=	fontID;
					sp->AddParameter( c );
					//
					boldSet	=	false;
				}
				boldSet	=	false;
			}
			break;
		case FunctionText::TEXT_TYPE_REF_SHOW_HELP:
			{
				if(!ref_beginSet)
				{
					ref_beginSet	=	true;
					ref_url			=	t.url;
					eventID			=	t.parameter;
					ref_beginColor	=	(int)wstr.size();
					ret_cursor		=	t.cursor.t;
					ref_color		=	t.color;
				}
			}
			break;
		case FunctionText::TEXT_TYPE_REF_END:
			{
				if(ref_beginSet)
				{
					SpeakParameter c;
					c.type	=	SpeakParameter::SP_TYPE_URL;
					c.begin	=	ref_beginColor;
					c.eventID	=	eventID;
					c.count	=	(int)wstr.size() - ref_beginColor;
					c.url		=	ref_url.c_str();
					c.cursor	=	ret_cursor.t;
					c.c			=	ref_color;
					sp->AddParameter( c );
					//
					ref_beginSet	=	false;
				}
				ref_beginSet	=	false;
			}
			break;
		case 0:
			{
				wstr += t.str.c_str();
			}
			break;
		}
	}
	return wstr.c_str();
}

const char*	UIAnalyseText::AnalyseText(const char* text, CSpeakParameter* sp)
{CPUTime(UIAnalyseText);
	int length					=	(int)strlen(text);
	if(length <= 0)
		return text;
	//
	char * wbuffer			=	new char[length + 1];
	wbuffer[0]	=	0;
	memcpy(wbuffer, text, sizeof(char) * (length + 1));
	std::vector<FunctionText> t;
	analyseFunction(wbuffer, t, length);
	const char* r			=	analyseFunctionText(t, sp);
	delete[] wbuffer;
	return r;
}

const char*	UIAnalyseText::AnalyseFromFile(const char* szFile, CSpeakParameter* sp)
{CPUTime(UIAnalyseText);
	StreamBase* base	=	open_s(szFile, true);
	if(!base)
		return "";
	//
	const char* r	=	"";
	if(base->size() > 0)
	{
		std::vector<FunctionText> t;
		analyseFunction(base->getMemoryPtr(), t, base->size());
		r	=	analyseFunctionText(t, sp);
	}
	close_s(&base);
	return r;
}

BOOL			UIAnalyseText::SetAnalyseText(UIControl* pFormatText, const char* textIn)
{CPUTime(UIAnalyseText);
	if(!pFormatText || !pFormatText->GetTextFormatPtr())return false;
	CSpeakParameter ret;
	const char* text		=	AnalyseText(textIn, &ret);
	SetAnalyseFromSaved(pFormatText, text, &ret);
	return true;
}

BOOL			UIAnalyseText::SetAnalyseFromFile(UIControl* pFormatText, const char* szFile)
{CPUTime(UIAnalyseText);
	if(!pFormatText || !pFormatText->GetTextFormatPtr())return false;
	CSpeakParameter ret;
	const char* text		=	AnalyseFromFile(szFile, &ret);
	SetAnalyseFromSaved(pFormatText, text, &ret);
	return true;
}

BOOL			UIAnalyseText::SetAnalyseFromSaved(UIControl* pFormatText, const char* text, CSpeakParameter* sp)
{CPUTime(UIAnalyseText);
	if(!pFormatText || !pFormatText->GetTextFormatPtr())return false;
	if(pFormatText->GetType() == UIControlType_VIEW_FORMAT_TEXT)
		static_cast<UIStaticFormat*>(pFormatText)->SetOrigText(text);
	else
		return false;
	//
	for(size_t i=0;i<sp->GetParameterCount();i++)
	{
		SpeakParameter* p	=	sp->GetParameter( i );
		switch(p->type)
		{
		case SpeakParameter::SP_TYPE_COLOR:
			pFormatText->GetTextFormatPtr()->SetFormatColor(p->begin, p->count, p->c);
			break;
		case SpeakParameter::SP_TYPE_URL:
			pFormatText->GetTextFormatPtr()->SetFormatColor(p->begin, p->count, p->c);
			pFormatText->GetTextFormatPtr()->SetFormatEvent(p->begin, p->count, p->url.strW(), p->eventID);
			break;
		case SpeakParameter::SP_TYPE_IMAGE:
			{
				INT nImage	=	GetDlgManager()->AddTexture(p->url.strA());
				if(nImage > 0)
					pFormatText->GetTextFormatPtr()->InsertImage(p->begin, nImage);
			}
			break;
		}
	}
	for(int i=(int)sp->GetParameterCount() - 1; i >= 0; i--)
	{
		SpeakParameter* p	=	sp->GetParameter( i );
		switch(p->type)
		{
		case SpeakParameter::SP_TYPE_FONT:
			pFormatText->GetTextFormatPtr()->SetFormatFont(p->begin, p->count, p->c);
			break;
		}
	}
	return true;
}

VOID			UIAnalyseText::SetAnalyseFunction(const char* function_name, const char* dest_text, int color)	//将分析器内函数替换为目标文本
{CPUTime(UIAnalyseText);
	for(size_t i=0;i<m_analyses.size();i++)
	{
		if(stricmp(m_analyses.at(i).function_name.c_str(), function_name) == 0)
		{
			m_analyses.at(i).dest_text		=	dest_text;
			m_analyses.at(i).color			=	color;
			return;
		}
	}
	AnalyseTextParam a;
	a.function_name	=	function_name;
	a.parameter		=	0;
	a.dest_text		=	dest_text;
	a.color			=	color;
	this->m_analyses.push_back( a );
}

};
