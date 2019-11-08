/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "GCViewUIEditor.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"
#include "phantom-gcDoc.h"
#include "phantom-gcView.h"
#include "AlignDialog.h"

#ifdef ZDX_TOOL_MODE

struct	CssType
{
	char name[128];
	char other[512];
	int nTextureID;// def -1
	//int nFontID;//def 0
	//int nFontSize;//def 12
};

extern	VOID		ToCode(char* v);
std::string	g_javaCode;
void	jsT(const char* szFmt, ...)
{
	char		szText[1024];
	_vsnprintf(szText,1024,szFmt,(va_list)(&szFmt+1));
	g_javaCode	+=	szText;
	g_javaCode	+=	"\n";
}
namespace Phantom{
extern	char	dec(unsigned char ch);
};
void	rgbToHex(char* ret, float f)
{
	int r = clamp(f * 255.0f, 0, 255);
	ret[0] = dec(r / 16);
	ret[1] = dec(r % 16);
}
void	colorToHex(char* ret, const Phantom::color4& c)
{
	rgbToHex(ret, c.r);
	rgbToHex(ret + 2, c.g);
	rgbToHex(ret + 4, c.b);
	ret[6] = 0;
}

VOID		GetElementText(std::string& str, Phantom::UIElement* e)
{

}

VOID		FileToHtmlFile(char* buf)
{
	const char* begin = buf;
	strlwr(buf);
	if(strstr(buf, "./default_init_zip/"))
	{
		begin += strlen("./default_init_zip/");
	}
	char temp[512];
	int index = 0;
	temp[0] = 0;
	int len = strlen(begin);
	for(int i=0;i<len;i++)
	{
		if(begin[i] == '\\')
			temp[index++] = '/';
		else
			temp[index++] = begin[i];
	}
	temp[index++] = 0;
	strcpy(buf, temp);
}

const char*			AddCss(std::vector<CssType>& css, int texID)
{
	for(int i=0;i<css.size();i++)
	{
		CssType& c = css[i];
		if(c.nTextureID == texID)
			return c.name;
	}
	CssType c;
	memset(&c, 0, sizeof(c));
	c.nTextureID = texID;
	char temp[128];
	if(c.nTextureID >= 0)
	{
		//wchar_t name[258];
		//_wsplitpath(Phantom::GetDlgManager()->GetTexturePtr(c.nTextureID)->strFilename, 0, 0, name, 0);
		strcpy(temp, Phantom::GetDlgManager()->GetTextureNode(texID)->fileName.t);
		ToCode(temp);
		sprintf(c.name, "s_%s", temp);
		FileName texFile = Phantom::GetDlgManager()->GetTextureNode(texID)->fileName.t;
		FileToHtmlFile(texFile.t);
		strcpy(c.other, format("background-image:url('%s');", texFile.t).szText);
	}
	else
		strcpy(c.name, "s_template");
	//

	css.push_back(c);
	return css.at(css.size() - 1).name;
}

//VOID				GCViewUIEditor::GenericHtml5Code()	//创建html5格式的读取信息文件. js文件
//{
//	char dlgName[256];
//	strcpy(dlgName, m_dialogPtr->GetName());
//	ToCode(dlgName);
//	g_javaCode = "";
//	std::vector<CssType> css;
//	jsT("<obj id=\"%s\" style=\"text-align:center;z-index:1;\">", dlgName);
//	jsT("<obj>");
//	jsT("<form id=\"form_%s\" method=\"post\" action=\"\">", dlgName);
//	//
//	for(int g=0;g<m_dialogPtr->GetGroupCount();g++)
//	{
//		Phantom::UIGroup* group	=	m_dialogPtr->GetGroupPtr(g);
//		Name groupName = group->GetName();
//		ToCode(groupName.t);
//		if(g > 0)
//		{
//			jsT("<obj id=\"g_%s\">", dlgName);
//		}
//		for(int c=0;c<m_dialogPtr->GetControlCount();c++)
//		{
//			Phantom::UIControl* control		=	m_dialogPtr->GetControlPtr(c);
//			if(control->IsBackground() || control->GetGroupID() != group->GetID())
//				continue;	//如果是背景控件，则不创建
//			Name controlName		=	control->GetName();
//			if(!controlName.t[0] || controlName == "n/a" || controlName == "nil" || controlName == "none" || controlName == "null")
//				controlName	=	"";
//			else
//				ToCode(controlName.t);
//			//
//			const Phantom::Rect& rc = control->GetRenderRect();
//			//
//			std::string tempFont;
//			char szcolor[10];
//			char szfont[128];
//			char szTemp[128];
//			memset(szfont, 0, sizeof(szfont));
//			colorToHex(szcolor, control->GetTextColor());
//			int fontID = control->GetElementPtr(0)->GetFontID();
//			FontNode2D* node	=	Phantom::GetDlgManager()->GetFontPtr(fontID);
//			BOOL bHasText	=	strlen(control->GetTextA()) > 0;
//			if(bHasText && node)
//			{
//				tempFont += format("font-size:%dpx; color:#%s;", abs(node->nFontHeight), szcolor).szText;
//				if(fontID > 0)
//					tempFont += format("font-family:%s;", A(Phantom::GetDlgManager()->GetFontName(fontID)).buf).szText;
//				if(node->nWeight > 0)
//					tempFont += "font-weight:bold;";
//				if(control->GetTextFormat() & DT_LEFT)
//					tempFont	+=	"text-align:left;";
//				else if(control->GetTextFormat() & DT_RIGHT)
//					tempFont	+=	"text-align:right;";
//				if(control->ShowTextShadow() > 0)
//				{
//					colorToHex(szcolor, control->GetShadowColor());
//					const color4& shadowC = control->GetShadowColor();
//					tempFont	+=	format("text-shadow: 0px 0px %dpx rgba(%d,%d,%d,%.02f);", control->ShowTextShadow(), shadowC.r * 255.0f, shadowC.g * 255.0f, shadowC.b * 255.0f, shadowC.a);
//				}
//				if(control->GetType() == UIControlType_STATIC || control->GetType() == UIControlType_VIEW_FORMAT_TEXT)
//				{
//					if(control->GetTextFormat() & DT_VCENTER)
//						tempFont	+=	format("padding-top:%dpx;", (rc.GetHeight() - abs(node->nFontHeight)) / 2);
//					else if(control->GetTextFormat() & DT_BOTTOM)
//						tempFont	+=	format("padding-top:%dpx;", (rc.GetHeight() - abs(node->nFontHeight)));
//				}
//			}
//			float fAlpha = control->GetTextColor().a;
//			int nTexture = control->GetElementPtr(0)->GetTextureID();
//			Phantom::Pixel texOffset(0, 0);
//			if(nTexture >= 0)
//			{
//				int w = Phantom::GetDlgManager()->GetTexturePtr(nTexture)->dwWidth;
//				int h = Phantom::GetDlgManager()->GetTexturePtr(nTexture)->dwHeight;
//				Phantom::TextureRect* modeRect = control->GetElementPtr(0)->GetModeRect();
//				//texOffset = modeRect->off;
//				fAlpha	=	control->GetElementPtr(0)->GetTextureColor().a;
//				if(fAlpha < 0.99f)
//					tempFont	+=	format("alpha(opacity=%d);opacity:%.02f;", fAlpha * 100.0f, fAlpha).szText;
//
//				const Phantom::Rect& texRect	=	control->GetElementPtr(0)->GetSourceRect();
//				Phantom::Rect srcRect = texRect;
//				//
//				if(!(srcRect.left == 0 && srcRect.top == 0 && srcRect.right == w && srcRect.bottom == h))
//				{
//					float fScaleX = 1;
//					float fScaleY = 1;
//					if(control->GetElementPtr(0)->GetRenderScale())
//					{
//						fScaleX	=	(float)rc.GetWidth() / (float)srcRect.GetWidth();
//						fScaleY	=	(float)rc.GetHeight() / (float)srcRect.GetHeight();
//					}
//					tempFont	+=	format("background-position: -%.0fpx -%.0fpx; background-size:%.0fpx %.0fpx;", (float)srcRect.left * fScaleX, (float)srcRect.top * fScaleY,
//						(float)w * fScaleX, (float)h * fScaleY).szText;
//				}
//			}
//			else
//			{
//			}
//			//
//			jsT("<obj id=\"div_%s\" class=\"pvd_group\" style=\"left:%dpx;top:%dpx;width:%dpx;height:%dpx;z-index:%d;\">", controlName.t, rc.left + texOffset.x, rc.top + texOffset.y, rc.GetWidth(), rc.GetHeight(), control->GetLayer());
//
//			const char* szStyle = AddCss(css, nTexture);
//			
//			switch(control->GetType())
//			{
//			case UIControlType_BUTTON://			/*控件类型_按钮*/=	1,
//				{
//					if(bHasText)
//						strcpy(szTemp, format("value=\"%s\"", control->GetTextA()).szText);
//					else
//						szTemp[0] = 0;
//					jsT("<obj id=\"%s\" class=\"%s\" %s style=\"%s\"></obj>", controlName.t, szStyle, szTemp, tempFont.c_str());
//				}
//				break;
//			case UIControlType_CHECKBOX://			/*控件类型_按钮*/=	1,
//				{
//					jsT("<input type=\"checkbox\" class=\"%s\" id=\"%s\" value=\"%s\" style=\"%s\"/>", szStyle, controlName.t, control->GetTextA(), tempFont.c_str());
//				}
//				break;
//			case UIControlType_COMBOBOX://			/*控件类型_按钮*/=	1,
//				{
//					jsT("<select id=\"%s\" class=\"%s\" style=\"%s\">\r\n</select>", controlName.t, szStyle, tempFont.c_str());
//				}
//				break;
//			case UIControlType_SLIDER://			/*控件类型_按钮*/=	1,
//				{
//					jsT("<obj id=\"%s\" value=\"%s\" class=\"%s\" style=\"%s\"></obj>", controlName.t, control->GetTextA(), szStyle, tempFont.c_str());
//				}
//				break;
//			case UIControlType_LISTBOX://			/*控件类型_按钮*/=	1,
//				{
//					jsT("<select id=\"%s\" class=\"%s\" style=\"%s\">\r\n</select>", controlName.t, szStyle, tempFont.c_str());
//				}
//				break;
//			case UIControlType_SCROLLBAR://			/*控件类型_按钮*/=	1,
//				{
//					jsT("<obj id=\"%s\" value=\"%s\" class=\"%s\" style=\"%s\"></obj>", controlName.t, control->GetTextA(), szStyle, tempFont.c_str());
//				}
//				break;
//			case UIControlType_PROGRESS://			/*控件类型_按钮*/=	1,
//				{
//					jsT("<progress id=\"%s\" value=\"%.03f\" max=\"%d\" class=\"%s\" style=\"%s\"/>", controlName.t, control->GetProgressValue(), 1, szStyle, tempFont.c_str());
//				}
//				break;
//			case UIControlType_STATIC://			/*控件类型_按钮*/=	1,
//				{
//					jsT("<obj id=\"%s\" class=\"%s\" style=\"%s\">%s</obj>", controlName.t, szStyle, tempFont.c_str(), control->GetTextA());
//				}
//				break;
//			case UIControlType_EDITBOX:
//			case UIControlType_IMEEDITBOX:
//				jsT("<input type=\"text\" id=\"%s\" value=\"%s\" class=\"%s\" style=\"%s\"/>", controlName.t, control->GetTextA(), szStyle, tempFont.c_str());
//				break;
//			case UIControlType_VIEW_FORMAT_TEXT://			/*控件类型_按钮*/=	1,
//				{
//					jsT("<textarea readonly=\"readonly\" id=\"%s\" class=\"%s\" style=\"%s\"></textarea>", controlName.t, szStyle, tempFont.c_str());
//				}
//				break;
//			case UIControlType_FORMAT_EDITBOX://			/*控件类型_按钮*/=	1,
//				{
//					jsT("<textarea id=\"%s\" class=\"%s\" style=\"%s\"></textarea>", controlName.t, szStyle, tempFont.c_str());
//				}
//				break;
//			case UIControlType_LISTCTRL3D://			/*控件类型_按钮*/=	1,
//				{
//					jsT("<obj id=\"%s\" class=\"%s\" style=\"%s\"></obj>", controlName.t, szStyle, tempFont.c_str());
//				}
//				break;
//			case UIControlType_2DEffect://			/*控件类型_按钮*/=	1,
//				{
//					jsT("<obj id=\"%s\" class=\"%s\" style=\"%s\"></obj>", controlName.t, szStyle, tempFont.c_str());
//				}
//				break;
//			}
//			jsT("</obj>");
//		}
//		if(g > 0)
//		{
//			jsT("</obj>");
//		}
//	}
//
//	//
//	jsT("</form>");
//	jsT("</obj>");
//	jsT("</obj>");
//	//
//	std::string	strBody = g_javaCode.c_str();
//	g_javaCode.clear();
//	//build css
//	jsT("<!DOCTYPE html>");
//	jsT("<html xmlns=\"http://www.w3.org/1999/xhtml\">");
//	jsT("<head>");
//	jsT("<style type=\"text/css\">");
//	for(int i=0;i<css.size();i++)
//	{
//		jsT(".%s{position:relative;width:100%%;height:100%%;border:0px;%s}", css[i].name, css[i].other);
//	}
//	jsT(".pvd_group{position:absolute;border:0px;overflow:hidden;}");
//	jsT("</style>");
//	jsT("</head>");
//	char szTemp[256];
//	colorToHex(szTemp, color4(GetProjectPtr()->m_renderState.uClearColor));
//	jsT("<body style=\"background-color:#%s\">", szTemp);
//	g_javaCode	+=	strBody.c_str();
//	jsT("</body>");
//	jsT("</html>");
//	//position:absolute;width:100%%;height:100%%;
//	//
//	if( OpenClipboard(0))
//	{
//		EmptyClipboard();
//		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, g_javaCode.size() + 1 );
//		if( hBlock )
//		{
//			char *pwszText = (char*)GlobalLock( hBlock );
//			strcpy(pwszText, g_javaCode.c_str());
//			GlobalUnlock( hBlock );
//			SetClipboardData( CF_TEXT, hBlock );
//		}
//		CloseClipboard();
//		if( hBlock )
//			GlobalFree( hBlock );
//	}
//	GetMainFrame()->MessageBox(Language(L"已经拷贝到剪贴板。"), Language(L"正版专用功能"), MB_OK);
//}

void	TextToHtml(char* ret, const char* in)
{
	int len = strlen(in);
	int index = 0;
	while(*in)
	{
		if(*in != '\r')
		{
			if(*in == '\n')
			{
				ret[index++] = '<';
				ret[index++] = 'b';
				ret[index++] = 'r';
				ret[index++] = '/';
				ret[index++] = '>';
			}
			else if(*in == ' ')
			{
				ret[index++] = '&';
				ret[index++] = 'n';
				ret[index++] = 'b';
				ret[index++] = 's';
				ret[index++] = 'p';
				ret[index++] = ';';
			}
			else
			{
				ret[index++] = *in;
			}
		}
		in++;
	}
	ret[index++] = 0;
}

const char*	GetControlCssName(int type)
{
	switch(type)
	{
	case UIControlType_BUTTON://			/*控件类型_按钮*/=	1,
		return "button";
	case UIControlType_CHECKBOX://			/*控件类型_按钮*/=	1,
		return "checkbox";
	case UIControlType_COMBOBOX://			/*控件类型_按钮*/=	1,
		return "combobox";
	case UIControlType_SLIDER://			/*控件类型_按钮*/=	1,
		return "slider";
	case UIControlType_LISTBOX://			/*控件类型_按钮*/=	1,
		return "listbox";
	case UIControlType_SCROLLBAR://			/*控件类型_按钮*/=	1,
		return "scrollbar";
	case UIControlType_PROGRESS://			/*控件类型_按钮*/=	1,
		return "progress";
	case UIControlType_STATIC://			/*控件类型_按钮*/=	1,
		return "static";
	case UIControlType_VIEW_FORMAT_TEXT://			/*控件类型_按钮*/=	1,
		return "text";
	case UIControlType_LISTCTRL3D://			/*控件类型_按钮*/=	1,
		return "listctrl";
	case UIControlType_ui2DEffect://			/*控件类型_按钮*/=	1,
		return "effect";
	}
	return "";
}

VOID				ExportElement(int index, Phantom::UIElement* e, const char* szC, Phantom::UIControl* control)
{
	BOOL bE = false;
	int nTexture = e->GetTextureID();
	if(nTexture > 0)
	{
		if(!bE){jsT("var e=this.%s.element(%d);", szC, index);bE=true;}
		Phantom::ElementTextureRect* modeRect = &e->m_modeRect;
		const Phantom::color4& c = control->GetTextureColor();
		const Phantom::Rect& texRect	=	e->GetSourceRect();
		//
		int offx = 0, offy= 0;
		if(modeRect)
		{
			offx = modeRect->off.x;
			offy = modeRect->off.y;
		}
		char texFile[128];
		sprintf(texFile, "t_%d", nTexture);
		jsT("e.setImage(%s, %d, %d, %d, %d,%s, %d,%d);", texFile, texRect.left, texRect.top, texRect.GetWidth(), texRect.GetHeight(), (e->GetTileWidth() > 0 || e->GetTileHeight() > 0) ? "true" : "false", offx, offy);
	}
	if(e->GetUVAnimationCount() > 0)
	{
		if(!bE){jsT("var e=c.element(%d);", index);bE=true;}
		for(int i=0;i<e->GetUVAnimationCount();i++)
		{
			Phantom::UVAnimation* uvAni = e->GetUVAnimationPtr(i);
			//
			std::string strNext;
			for(int tt=0;tt<e->GetUVAnimationCount();tt++)
			{
				if(e->GetUVAnimationPtr(tt)->GetActionType() == uvAni->GetNextAction())
				{
					strNext = e->GetUVAnimationPtr(tt)->GetName();
					break;
				}
			}
			//
			int nBegin, nCount;
			uvAni->GetUVMapIndex(nBegin, nCount);
			if(uvAni->IsCustomEnable())
			{
				char name[256];
				char buf[512];
				strcpy(name, uvAni->GetName());
				strlwr(name);
				sprintf(buf, "e.addUVAni(\"%s\",\"%s\", %.02f, \"", name, strNext.c_str(), uvAni->GetPlaySpeed());
				std::string strAni = buf;
				for(int tr=0;tr<nCount;tr++)
				{
					const Phantom::TextureRect* trect = uvAni->GetTextureRect(tr);
					sprintf(buf, "%d,%d,%d,%d,%d,%d", trect->rc.left, trect->rc.top, trect->rc.GetWidth(), trect->rc.GetHeight(), trect->off.x, trect->off.y);
					if(tr < (nCount-1))
						strcat(buf, "|");
					strAni += buf;
				}
				strAni += "\");\n";
				jsT("%s", strAni.c_str());
			}
		}
	}
	BOOL bHasText	=	(strlen(control->GetText()) > 0 && index == 0);
	if(bHasText)
	{
		if(!bE){jsT("var e=this.%s.element(%d);", szC, index);bE=true;}
		int fontID = e->GetFontID();
		Phantom::FontNode2D* node = Phantom::GetDlgManager()->GetFontNode(fontID);
		//
		char szcolor[120];
		colorToHex(szcolor, control->GetTextColor());
		strlwr(szcolor);
		char szBuf[10240];
		TextToHtml(szBuf, control->GetText());
		jsT("e.setText(\"%s\", %d, %s, \"%s\");", szBuf, abs(node->nFontHeight), node->nWeight > 400 ? "true":"false", szcolor);
		//if(fontID > 0 && stricmp(A(Phantom::GetDlgManager()->GetFontName(fontID)).buf, "@System") != 0)
		//	jsT("e.font = \"%s\";", A(Phantom::GetDlgManager()->GetFontName(fontID)).buf);
		if(control->GetTextFormat() & DT_RIGHT)
			jsT("e.align = \"right\";");
		else if(control->GetTextFormat() & DT_CENTER)
			jsT("e.align = \"center\";");
		else
			jsT("e.align = \"left\";");
		if(control->GetTextFormat() & DT_VCENTER || control->GetTextFormat() & DT_BOTTOM)
		{
			if(control->GetTextFormat() & DT_VCENTER)
				jsT("e.valign = 1;");
			else if(control->GetTextFormat() & DT_BOTTOM)
				jsT("e.valign = 2;");
		}
		if(control->ShowTextShadow() > 0)
		{
			colorToHex(szcolor, control->GetShadowColor());
			color4 shadowC = control->GetShadowColor();
			jsT("e.shadow = \"0px %dpx 0px rgba(%.0f,%.0f,%.0f,%.02f)\";", control->ShowTextShadow(), shadowC.r * 255.0f, shadowC.g * 255.0f, shadowC.b * 255.0f, shadowC.a);
		}
	}
}

VOID		ToCodeHtml(char* v, std::string& retType, std::string& retClass, std::string& retLine)
{
	char szTemp[1024];
	szTemp[0] = 0;
	const char* vv = v;
	char* dest = szTemp;
	char szType[128];
	memset(szType, 0, sizeof(szType));
	char szClass[128];
	memset(szClass, 0, sizeof(szClass));
	const char* beginType = 0;
	const char* beginClass = 0;
	const char* beginLine = 0;
	char szLine[128];
	memset(szLine, 0, sizeof(szLine));
	while(*vv)
	{
		if(*vv == ' ' || *vv == '\r' || *vv == '\n')
		{
			vv++;
			continue;
		}
		if(beginType)
		{
			if(*vv == ']')
			{
				int nCount = vv-beginType;
				if(nCount > 0)
					strncpy(szType, beginType, nCount);
				vv++;
				beginType = 0;
				continue;
			}
			vv++;
			continue;
		}
		else if(beginClass)
		{
			if(*vv == '}')
			{
				int nCount = vv-beginClass;
				if(nCount > 0)
					strncpy(szClass, beginClass, nCount);
				vv++;
				beginClass = 0;
				continue;
			}
			vv++;
			continue;
		}
		else if(beginLine)
		{
			if(*vv == '>')
			{
				int nCount = vv-beginLine;
				if(nCount > 0)
					strncpy(szLine, beginLine, nCount);
				vv++;
				beginLine = 0;
				continue;
			}
			vv++;
			continue;
		}
		else if(*vv == '[')
		{
			vv++;
			beginType = vv;
			continue;
		}
		else if(*vv == '{')
		{
			vv++;
			beginClass = vv;
			continue;
		}
		else if(*vv == '<')
		{
			vv++;
			beginLine = vv;
			continue;
		}
		char ch = *vv;
		char chr = '_';
		if(ch >= 'a' && ch <= 'z')
			chr = ch;
		if(ch >= '0' && ch <= '9')
		{
			if((dest - szTemp) > 0)
				chr = ch;
		}
		if(ch >= 'A' && ch <= 'Z')
			chr = ch;
		if(ch == '_')
			chr = ch;
		*dest = chr;
		dest++;
		vv++;
	}
	retType		=	szType;
	retClass	=	szClass;
	retLine		=	szLine;
	*dest = 0;
	strcpy(v, szTemp);
}

VOID				GCViewUIEditor::GenericHtml5Code()	//创建html5格式的读取信息文件. js文件
{
	char dlgName[256];
	char szDir[256], szDrive[256];
	_splitpath(m_dialogPtr->m_strFile.t, szDrive, szDir, dlgName, 0);
	SetProjectPath();
	strcat(szDrive, szDir);
	strcat(szDrive, dlgName);
	strcat(szDrive, ".js");
	ToCode(dlgName);
	strlwr(dlgName);
	g_javaCode = "";
	std::string strType, strClass;
	jsT("function dlg_%s(){", dlgName);
	int x=0,y=0;
	m_dialogPtr->GetLocation(x,y);
	Phantom::Rect dlgRect(x, y, x + m_dialogPtr->GetWidth(), y + m_dialogPtr->GetHeight());
	jsT("this.dlg = new PDialog(\"%s\", %d,%d,%d,%d);", dlgName,x,y,m_dialogPtr->GetWidth(), m_dialogPtr->GetHeight());
	std::map<int,std::string> textures;
	for(int c=0;c<m_dialogPtr->GetControlCount();c++)
	{
		Phantom::UIControl* ctl = m_dialogPtr->GetControlPtr(c);
		for(int e=0;e<ctl->GetElementCount();e++)
		{
			int texID = ctl->GetElementPtr(e)->GetTextureID();
			if(texID <= 0)
				continue;
			if(textures[texID].size() == 0)
			{
				char szTex[128];
				sprintf(szTex, "t_%d", texID);
				textures[texID] = szTex;
			}
			for(int a=0;a<ctl->GetElementPtr(e)->GetUVAnimationCount();a++)
			{
				texID = ctl->GetElementPtr(e)->GetUVAnimationPtr(a)->GetTextureID();
				if(texID <0 )
					continue;
				if(textures[texID].size() == 0)
				{
					char szTex[128];
					sprintf(szTex, "t_%d", texID);
					textures[texID] = szTex;
				}
			}
		}
	}
	//
	for(std::map<int,std::string>::iterator it=textures.begin();it!=textures.end();it++)
	{
		FileName texFile = Phantom::GetDlgManager()->GetTextureNode(it->first)->fileName.t;
		FileToHtmlFile(texFile.t);
		strlwr(texFile.t);
		jsT("var %s = \"%s\";", it->second.c_str(), texFile.t);
	}
	std::map<int, std::vector<std::string> > controls;
	for(int g=0;g<m_dialogPtr->GetGroupCount();g++)
	{
		Phantom::UIGroup* group	=	m_dialogPtr->GetGroupPtr(g);
		Name groupName = group->GetName();
		for(int gg=0;gg<m_dialogPtr->GetGroupCount();gg++)
		{
			if(gg == g)
				continue;
			if(stricmp(m_dialogPtr->GetGroupPtr(gg)->GetName(), groupName) == 0 || groupName.t[0] == 0)
			{
				sprintf(groupName.t, "g%d", m_dialogPtr->GetGroupPtr(gg)->GetID());
				break;
			}
		}
		Phantom::Pixel dlgOff(0, 0);
		std::string strLine;
		ToCodeHtml(groupName.t, strType, strClass, strLine);
		strlwr(groupName.t);
		if(g > 0)
		{
			int nMaxLayer = -1;
			Phantom::Rect groupRect(10000,10000,-10000,-10000);
			for(int c=0;c<m_dialogPtr->GetControlCount();c++)
			{
				Phantom::UIControl* control		=	m_dialogPtr->GetControlPtr(c);
				if(control->IsBackground() || control->GetGroupID() != group->GetID())
					continue;
				nMaxLayer = max(nMaxLayer, control->GetLayer());
				const Phantom::Rect& cR = control->GetRenderRect();
				groupRect.left = min(groupRect.left, cR.left);
				groupRect.top = min(groupRect.top, cR.top);
				groupRect.right = max(groupRect.right, cR.right);
				groupRect.bottom = max(groupRect.bottom, cR.bottom);
			}
			if(nMaxLayer < 0)
				groupRect	=	dlgRect;
			dlgOff.x	=	groupRect.left;
			dlgOff.y	=	groupRect.top;
			jsT("this.group%s = function(d){", groupName.t);
			jsT("this.grp = new PGroup(\"%s\",d,%d,%d,%d,%d,%s,%s,%d);", groupName.t, groupRect.left, groupRect.top, groupRect.GetWidth(), groupRect.GetHeight(), group->GetEnabled() ? "true" : "false", group->GetVisible() ? "true" : "false", nMaxLayer);
			if(strType.size())
				jsT("this.grp.settype(\"%s\");", strType.c_str());
			if(strClass.size())
				jsT("this.grp.setclass(\"%s\");", strClass.c_str());
		}
		else
			groupName = "";
		for(int c=0;c<m_dialogPtr->GetControlCount();c++)
		{
			Phantom::UIControl* control		=	m_dialogPtr->GetControlPtr(c);
			if(control->IsBackground() || control->GetGroupID() != group->GetID())
				continue;	//如果是背景控件，则不创建
			Name controlName		=	control->GetName();
			if(!controlName.t[0] || controlName == "n/a" || controlName == "nil" || controlName == "none" || controlName == "null")
				sprintf(controlName.t, "c%d", control->GetID());
			Name controlNameS		=	controlName.t;
			strlwr(controlName.t);
			strlwr(controlNameS.t);
			sprintf(controlNameS.t, "%s_%s", g > 0 ? group->GetName() : dlgName, control->GetName());
			for(int k=c;k<m_dialogPtr->GetControlCount();k++)
			{
				Phantom::UIControl* temp =	m_dialogPtr->GetControlPtr(k);
				if(temp->IsBackground() || temp->GetGroupID() != group->GetID() || temp == control)
					continue;	//如果是背景控件，则不创建
				Name tempc;
				sprintf(tempc.t, "%s_%s", temp->GetGroupID() > 0 ? temp->GetGroupPtr()->GetName() : dlgName, temp->GetName());
				if(stricmp(tempc.t, controlNameS.t) == 0)
				{
					sprintf(controlName.t, "c%d", control->GetID());
					break;
				}
			}

			ToCodeHtml(controlName.t, strType, strClass, strLine);
			strlwr(controlName.t);
			//
			const Phantom::Rect& rc = control->GetRenderRect();
			//
			std::string tempFont;
			char szcolor[10];
			char szfont[128];
			char szTemp[128];
			memset(szfont, 0, sizeof(szfont));
			colorToHex(szcolor, control->GetTextColor());
			int fontID = control->GetElementPtr(0)->GetFontID();
			Phantom::FontNode2D* node	=	Phantom::GetDlgManager()->GetFontNode(fontID);
			BOOL bHasText	=	strlen(control->GetText()) > 0;
			if(bHasText && node)
			{
			}
			float fAlpha = control->GetTextColor().a;
			int nTexture = control->GetElementPtr(0)->GetTextureID();
			Phantom::Pixel texOffset(0, 0);
			if(nTexture >= 0)
			{
			}
			else
			{
			}
			//
			char szGrp[128];
			if(g > 0)
				strcpy(szGrp, "this.grp");
			else
				strcpy(szGrp, "this.dlg");
			int type = control->GetType();
			if(type==7)type=6;
			if(g == 0 && strnicmp(controlName.t, "sp_", 3) == 0)
			{
				jsT("this.%s = new divSplit(\"%s\",this.dlg,%d,%d,%d);", controlName.t, controlName.t, (rc.top + rc.bottom)/2, rc.left, rc.GetWidth());
				continue;
			}
			std::string backColor = "null";
			std::string backAlpha = "null";
			char ret[128];
			if(control->GetElementPtr(0)->GetTextureID() == 0)
			{
				colorToHex(ret, control->GetTextureColor());
				backColor	=	"\"";
				backColor	+=	ret;
				backColor	+=	"\"";
				if(control->GetTextureColor().a < 0.99f)
				{
					sprintf(ret, "%.02f", control->GetTextureColor().a);
					backAlpha = ret;
				}
			}
			else
			{
				if(control->GetTextColor().a < 0.99f)
				{
					sprintf(ret, "%.02f", control->GetTextColor().a);
					backAlpha = ret;
				}
			}
			strlwr((char*)backColor.c_str());
			
			jsT("this.%s = new PControl(\"%s\", %s, %d,%d,%d,%d,%d,%s,%s,%d,%s,%s,%s);", controlName.t, controlName.t, szGrp, rc.left - dlgOff.x, rc.top - dlgOff.y, rc.GetWidth(), rc.GetHeight(), control->GetLayer(),
				control->GetVisible() ? "true" : "false", control->GetEnabled() ? "true" : "false", type, control->GetElementPtr(0)->GetTextureID() == 0 ? "true" : "false", backColor.c_str(), backAlpha.c_str());
			//
			//controls[groupName.t].push_back(controlName.t);
			std::string controlType = "";
			switch(type)
			{
			case UIControlType_BUTTON://			/*控件类型_按钮*/=	1,
				{
					controlType = "button";
				}
				break;
			case UIControlType_CHECKBOX://			/*控件类型_按钮*/=	1,
				{
					jsT("this.%s.check(%s);", controlName.t, control->GetChecked() ? "true" : "false");
					controlType = "checkbox";
				}
				break;
			case UIControlType_COMBOBOX://			/*控件类型_按钮*/=	1,
				{
					controlType = "combobox";
				}
				break;
			case UIControlType_SLIDER://			/*控件类型_按钮*/=	1,
				{
					controlType = "slider";
				}
				break;
			case UIControlType_LISTBOX://			/*控件类型_按钮*/=	1,
				{
					controlType = "listbox";
				}
				break;
			case UIControlType_SCROLLBAR://			/*控件类型_按钮*/=	1,
				{
					controlType = "scrollbar";
				}
				break;
			case UIControlType_PROGRESS://			/*控件类型_按钮*/=	1,
				{
					controlType = "progress";
				}
				break;
			case UIControlType_STATIC://			/*控件类型_按钮*/=	1,
				{
					controlType = "static";
				}
				break;
			case UIControlType_EDITBOX:
				{
					controlType = "edit";
					if(static_cast<Phantom::UITextBox*>(control)->m_bPassword)//->GetEditBox()->IsPassword())
						jsT("this.%s.setPass(true);",controlName.t);
				}
				break;
			case UIControlType_VIEW_FORMAT_TEXT://			/*控件类型_按钮*/=	1,
				{
					controlType = "static";
				}
				break;
			case UIControlType_FORMAT_EDITBOX://			/*控件类型_按钮*/=	1,
				{
					controlType = "edit";
				}
				break;
			case UIControlType_LISTCTRL3D://			/*控件类型_按钮*/=	1,
				{
					controlType = "listctrl";
				}
				break;
			case UIControlType_ui2DEffect://			/*控件类型_按钮*/=	1,
				{
				}
				break;
			}
			int nTextureID = control->GetElementPtr(0)->GetTextureID();
			for(int e=0;e<control->GetElementCount();e++)
			{
				ExportElement(e, control->GetElementPtr(e), controlName.t, control);
				//nTextureID = max(nTextureID, control->GetElementPtr(e)->GetTextureID());
			}
			if(strType.size())
				jsT("this.%s.settype(\"%s\");", controlName.t, strType.c_str());
			if(strClass.size())
				jsT("this.%s.setclass(\"%s\");", controlName.t, strClass.c_str());
			if(strLine.size())
				jsT("this.%s.setline(\"%s\");", controlName.t, strLine.c_str());
			//
			for(int t=0;t<control->GetTrackCount();t++)
			{
				Phantom::UIAnimationTrack* ani = control->GetTrackPtr(t);
				std::string strAni;
				char buf[512];
				std::string strNext;
				for(int tt=0;tt<control->GetTrackCount();tt++)
				{
					if(control->GetTrackPtr(tt)->GetActionType() == ani->GetNextActionType())
					{
						strNext = control->GetTrackPtr(tt)->GetName();
						break;
					}
				}
				char name[256];
				strcpy(name, ani->GetName());
				strlwr(name);
				sprintf(buf, "this.%s.addAni(\"%s\",\"%s\", %.02f, %s, \"%s\", %.02f, \"", controlName.t, name, ani->GetWaveFile(), ani->GetSleepTime(), ani->GetSleepAtHide() ? "true" : "false", strNext.c_str(), 
					ani->GetPlaySpeed());
				strAni = buf;
				Phantom::FloatAnimation* rectX = ani->GetAnimationPtr(Phantom::ControlTrackType_RectX);
				Phantom::FloatAnimation* rectY = ani->GetAnimationPtr(Phantom::ControlTrackType_RectY);
				Phantom::FloatAnimation* rectW = ani->GetAnimationPtr(Phantom::ControlTrackType_RectW);
				Phantom::FloatAnimation* rectH = ani->GetAnimationPtr(Phantom::ControlTrackType_RectH);
				Phantom::FloatAnimation* alpha = ani->GetAnimationPtr(Phantom::ControlTrackType_TextureColorA);
				Phantom::FloatAnimation* rot = ani->GetAnimationPtr(Phantom::ControlTrackType_Rotation);
				Phantom::Rect rc = control->GetWindowRect();
				for(int f=0;f<ani->GetFrameCount();f++)
				{
					rc.left = (int)rectX->GetFrameValue(f);
					rc.top = (int)rectY->GetFrameValue(f);
					rc.right = (int)rectW->GetFrameValue(f);// + rc.left;
					rc.bottom = (int)rectH->GetFrameValue(f);// + rc.top;
					sprintf(buf, "%d,%d,%d,%d,%.02f,%.0f", rc.left, rc.top, rc.right, rc.bottom, alpha->GetFrameValue(f), rot->GetFrameValue(f));
					if(f < (ani->GetFrameCount() - 1))
						strcat(buf, "|");
					strAni += buf;
				}
				strAni += "\");\n";
				jsT(strAni.c_str());
			}
			//
		}
		if(g > 0)
		{
			jsT("}");
			jsT("this.%s=new this.group%s(this.dlg);", groupName.t, groupName.t);
		}
	}
	jsT("this.load=function(){this.dlg.load();}");
	//for(std::map<std::string, std::vector<std::string> >::iterator it = controls.begin(); it!=controls.end(); it++)
	//{
	//	for(int i=0;i<it->second.size();i++)
	//	{
	//		jsT("this.%s = this.%s.obj;", it->second.at(i).c_str(), it->second.at(i).c_str());
	//	}
	//}
	//jsT("}");
	jsT("this.destroy=function(){this.dlg.destroy();}");
	jsT("this.show=function(){this.dlg.obj.show();}");
	jsT("this.hide=function(){this.dlg.obj.hide();}");
	jsT("};");
	jsT("var %s = new dlg_%s();", dlgName, dlgName);
	//
	//W w(g_javaCode.c_str());
	//int nBufSize = WideCharToMultiByte(CP_UTF8, 0, w.buf, -1, NULL, 0, 0, FALSE);
	//char *szBuf = new char[nBufSize];
	//WideCharToMultiByte(CP_UTF8, 0, w.buf, -1, szBuf, nBufSize, 0, FALSE);
	//szBuf[nBufSize] = 0;
	//CFileHelperW w;
	//if(w.create(szDrive))
		//w.pStream->write(g_javaCode.c_str(), g_javaCode.size());
	FILE* pFile = 0;
	pFile = fopen(szDrive, "w+");
	if(pFile)
	{
		unsigned char head[3];
		head[0] = 0xef;
		head[1] = 0xbb;
		head[2] = 0xbf;
		fwrite(head, 3, 1, pFile);
		fwrite(g_javaCode.c_str(), g_javaCode.size(), 1, pFile);
		const char* e = "\n";
		fwrite(e, 1, 1, pFile);
		//fwrite(szBuf, nBufSize, 1, pFile);
		//fputs(szBuf, pFile);
		//fputc('\n',pFile);
		fclose(pFile);
	}
	//if( OpenClipboard(0))
	//{
	//	EmptyClipboard();
	//	HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, g_javaCode.size() + 1 );
	//	if( hBlock )
	//	{
	//		char *pwszText = (char*)GlobalLock( hBlock );
	//		strcpy(pwszText, g_javaCode.c_str());
	//		GlobalUnlock( hBlock );
	//		SetClipboardData( CF_TEXT, hBlock );
	//	}
	//	CloseClipboard();
	//	if( hBlock )
	//		GlobalFree( hBlock );
	//}
	GetMainFrame()->MessageBox(Language(L"已经创建文件。"), Language(L"正版专用功能"), MB_OK);
}


#else
VOID				GCViewUIEditor::GenericHtml5Code()	//创建html5格式的读取信息文件. js文件
{
}
#endif


extern	VOID		AddText(std::string& str, const char* szText, ...);
VOID		ToCodeE(char* v)
{
	int len = strlen(v);
	for(int i=0;i<len;i++)
	{
		char ch = v[i];
		switch(ch)
		{
		case ' ':
		case ',':
		case '.':
		case '*':
		case '/':
		case '\\':
		case '|':
		case '+':
		case '-':
		case '@':
		case '#':
		case '!':
		case '~':
		case '$':
		case '%':
		case '^':
		case '&':
			v[i] = '_';
			break;
		}
	}
}

VOID		GCViewUIEditor::GenericUIECode()	//创建可以在IPhone系统开始时使用的对话框自动初始化代码
{
	wchar_t fileName[256];
	_wsplitpath(m_strFile.c_str(), 0, 0, fileName, 0);
	std::string codeInit, codeDefine, code;
	std::string codeCombobox, codeCheckbox, codeSlider, codeButton, codeListbox, codeScrollbox, codeEvent;
	A className(fileName);
	ToCodeE(className.buf);
	char buf[1024];
	AddText(code, ".版本 2");
	//AddText(code, ".程序集 %s", className.buf);
	AddText(codeDefine, ".程序集变量 %s, 幻影界面对话框_类,,此界面读取代码由幻影自动生成", className.buf);
	FileName files = A(m_strFile.c_str()).buf;
	if(strnicmp(files.t, GetProjectPath(), strlen(GetProjectPath())) == 0)
	{
		FileName fileTemp = files.t + strlen(GetProjectPath());
		files = fileTemp.t;
	}
	AddText(codeInit, "%s.加载(\"%s\", \"%s\")", className.buf, className.buf, files.t);
	//.程序集变量 集_视频播放类, 幻影视频3D_类, , , 视频播放类
	//AddText(code, "'幻影界面控件_类", className.buf, className.buf, className.buf, className.buf);
	for(int g=0;g<m_dialogPtr->GetGroupCount();g++)
	{
		Phantom::UIGroup* group	=	m_dialogPtr->GetGroupPtr(g);
		Name groupName = group->GetName();
		ToCodeE(groupName.t);
		Name first = "";
		if(g > 0)
		{
			first = groupName.t;
			first += "_";
			AddText(codeDefine, ".程序集变量 %s, 幻影界面控件组_类", groupName.t);
			AddText(codeInit, "%s	=	%s.取得控件组根据名字(\"%s\")", groupName.t, className.buf, groupName.t);
		}
		for(int c=0;c<m_dialogPtr->GetControlCount();c++)
		{
			Phantom::UIControl* control		=	m_dialogPtr->GetControlPtr(c);
			if(control->IsBackground() || control->GetGroupID() != group->GetID())
				continue;	//如果是背景控件，则不创建
			Name controlName		=	control->GetName();
			if(!controlName.t[0] || controlName == "n/a" || controlName == "nil" || controlName == "none" || controlName == "null")
				continue;
			ToCodeE(controlName.t);
			AddText(codeDefine, ".程序集变量 %s%s, 幻影界面控件_类", first.t, controlName.t);
			if(g > 0)
			{
				AddText(codeInit, "%s%s	=	查找组内控件(%s, \"%s\")", first.t, controlName.t, groupName.t, controlName.t);
			}
			else
			{
				AddText(codeInit, "%s	=	%s.查找控件指针(\"%s\")", controlName.t, className.buf, controlName.t);
			}
			//构造事件处理
			if(control->GetVisibleOnScript())
			{
				std::string szBegin, szEnd;
				AddText(szBegin, ".如果 (是否相同控件 (控件, %s%s))", first.t, controlName.t);
				AddText(szEnd, ".否则");
				AddText(szEnd, ".如果结束");
				switch(control->GetType())
				{
				case UIControlType_BUTTON:
					codeButton	+=	szBegin.c_str();
					AddText(codeButton, "%s%s%s()'按回车自动添加子程序，加入自己的代码", className.buf, first.t, controlName.t);
					codeButton	+=	szEnd.c_str();
					break;
				case UIControlType_CHECKBOX:
					codeCheckbox	+=	szBegin.c_str();
					AddText(codeCheckbox, "%s%s%s()'按回车自动添加子程序，加入自己的代码", className.buf, first.t, controlName.t);
					codeCheckbox	+=	szEnd.c_str();
					break;
				case UIControlType_COMBOBOX:
					codeCombobox	+=	szBegin.c_str();
					AddText(codeCombobox, "%s%s%s()'按回车自动添加子程序，加入自己的代码", className.buf, first.t, controlName.t);
					codeCombobox	+=	szEnd.c_str();
					break;
				case UIControlType_SLIDER:
					codeSlider	+=	szBegin.c_str();
					AddText(codeSlider, "%s%s%s()'按回车自动添加子程序，加入自己的代码", className.buf, first.t, controlName.t);
					codeSlider	+=	szEnd.c_str();
					break;
				case UIControlType_LISTBOX:
					codeListbox	+=	szBegin.c_str();
					AddText(codeListbox, "%s%s%s()'按回车自动添加子程序，加入自己的代码", className.buf, first.t, controlName.t);
					codeListbox	+=	szEnd.c_str();
					break;
				case UIControlType_SCROLLBAR:
					codeScrollbox	+=	szBegin.c_str();
					AddText(codeScrollbox, "%s%s%s()'按回车自动添加子程序，加入自己的代码", className.buf, first.t, controlName.t);
					codeScrollbox	+=	szEnd.c_str();
					break;
				case UIControlType_LISTCTRL3D:
					codeListbox	+=	szBegin.c_str();
					AddText(codeListbox, "%s%s%s()'按回车自动添加子程序，加入自己的代码", className.buf, first.t, controlName.t);
					codeListbox	+=	szEnd.c_str();
					codeListbox	+=	szBegin.c_str();
					AddText(codeScrollbox, "%s%s%s_表头(参数)'参数=0表示第一列按钮被按下，其他的依次自动处理，在子程序中。", className.buf, first.t, controlName.t);
					codeListbox	+=	szEnd.c_str();
					break;
				case UIControlType_VIEW_FORMAT_TEXT:
					codeEvent	+=	szBegin.c_str();
					AddText(codeEvent, "%s%s%s(参数)'按回车自动添加子程序，加入自己的代码,参数是事件ID", className.buf, first.t, controlName.t);
					codeEvent	+=	szEnd.c_str();
					break;
				}
			}
		}
	}
	code	+=	codeDefine.c_str();
	AddText(code, ".子程序 载入%s, , 公开, 没有加载之前对话框不能正常使用[由幻影自动生成]", className.buf);
	AddText(code, ".如果 (%s.接口是否有效 () ＝ 假)", className.buf);
	code	+=	codeInit.c_str();
	AddText(code, "添加对话框回调 (&%s界面事件, %s)", className.buf, className.buf);
	AddText(code, "载入%s完毕 ()'按回车自动添加子程序，加入自己的代码", className.buf);
	AddText(code, ".否则");
	AddText(code, ".如果结束");
	AddText(code, ".子程序 释放%s, , 公开, [由幻影自动生成]释放内存，并将对话框设置为空，释放之后对话框不能继续使用，必须重新加载才可以正常使用", className.buf);
	AddText(code, ".如果 (%s.接口是否有效 () ＝ 真)", className.buf);
	AddText(code, "删除对话框回调 (%s)", className.buf);
	AddText(code, "将释放%s()'按回车自动添加子程序，加入自己的代码", className.buf);
	AddText(code, "%s.释放()", className.buf);
	AddText(code, ".否则");
	AddText(code, ".如果结束");
	//
	AddText(code, ".子程序 显示%s, , 公开, [由幻影自动生成]", className.buf);
	AddText(code, "%s.显示(真)", className.buf);
	AddText(code, ".子程序 隐藏%s, , 公开, ", className.buf);
	AddText(code, "%s.显示(假)", className.buf);
	//
	AddText(code, ".子程序 %s界面事件, , , [由幻影自动生成]自动回调事件，不要去修改他，必须要通过[幻影可视化设计器]生成此代码", className.buf);
	AddText(code, ".参数 事件类型, 整数型");
	AddText(code, ".参数 IControl_ptr, 整数型");
	AddText(code, ".参数 IControl_idx, 整数型");
	AddText(code, ".参数 IControl_t, 整数型");
	AddText(code, ".参数 参数, 整数型");
	AddText(code, ".局部变量 控件, 幻影界面控件_类");
	AddText(code, ".局部变量 控件_指针, 幻影接口_控件");
	AddText(code, "控件_指针.ptr ＝ IControl_ptr");
	AddText(code, "控件_指针.idx ＝ IControl_idx");
	AddText(code, "控件_指针.t ＝ IControl_t");
	AddText(code, "控件.设置接口指针 (控件_指针)");
	AddText(code, "' 必须自己添加以下的所有未定义子程序，系统会自动调用这个子程序，此部分代码都是幻影自动生成的。");
	if(codeButton.size() > 0)
	{
		AddText(code, ".如果 (事件类型 ＝ #界面事件ID_按钮按下)");
		AddText(code, codeButton.c_str());
		AddText(code, ".否则");
		AddText(code, ".如果结束");
	}
	if(codeSlider.size() > 0)
	{
		AddText(code, ".如果 (事件类型 ＝ #界面事件ID_Slider变化)");
		AddText(code, codeSlider.c_str());
		AddText(code, ".否则");
		AddText(code, ".如果结束");
	}
	if(codeCheckbox.size() > 0)
	{
		AddText(code, ".如果 (事件类型 ＝ #界面事件ID_单选框按下)");
		AddText(code, codeCheckbox.c_str());
		AddText(code, ".否则");
		AddText(code, ".如果结束");
	}
	if(codeListbox.size() > 0)
	{
		AddText(code, ".如果 (事件类型 ＝ #界面事件ID_列表框选择)");
		AddText(code, codeListbox.c_str());
		AddText(code, ".否则");
		AddText(code, ".如果结束");
	}
	if(codeScrollbox.size() > 0)
	{
		AddText(code, ".如果 (事件类型 ＝ #界面事件_左键点击)");
		AddText(code, codeScrollbox.c_str());
		AddText(code, ".否则");
		AddText(code, ".如果结束");
	}
	if(codeCombobox.size() > 0)
	{
		AddText(code, ".如果 (事件类型 ＝ #界面事件ID_组合框已经选择完)");
		AddText(code, codeCombobox.c_str());
		AddText(code, ".否则");
		AddText(code, ".如果结束");
	}
	if(codeEvent.size() > 0)
	{
		AddText(code, ".如果 (事件类型 ＝ #界面事件_格式化文本框事件点击)");
		AddText(code, codeEvent.c_str());
		AddText(code, ".否则");
		AddText(code, ".如果结束");
	}
	AddText(code, "'*******************以上幻影自动生成代码到此为止");
	AddText(code, "'*******************下面你可以写入自己的代码");
	AddText(code, "'*******************每次替换的时候千万记住要把自动生成的子程序删除之后覆盖");
	AddText(code, "'*******************要不然会出现重复的子程序名字");
	AddText(code, "'*******************以下写入你自己的代码");
	//
	if( OpenClipboard(0))
	{
		EmptyClipboard();
		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, code.size() + 1 );
		if( hBlock )
		{
			char *pwszText = (char*)GlobalLock( hBlock );
			strcpy(pwszText, code.c_str());
			GlobalUnlock( hBlock );
			SetClipboardData( CF_TEXT, hBlock );
		}
		CloseClipboard();
		if( hBlock )
			GlobalFree( hBlock );
	}
	GetMainFrame()->MessageBox(Language(L"已经拷贝到剪贴板。"), Language(L"正版专用功能"), MB_OK);
}
