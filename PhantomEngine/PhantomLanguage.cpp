#include "PhantomLanguage.h"
#include "PhantomFile.h"

namespace Phantom{

CLanguageFile*					GetLanguagePtr()
{
	static CLanguageFile file;
	return &file;
}
const char*		Lan(const char* szID, const char* szDefault)
{
//#ifdef WIN32_OS
//	GetLanguageDebug()->SetText(szID, szDefault);
//#endif
	return GetLanguagePtr()->GetText(szID, szDefault);
}

CLanguageFile::CLanguageFile(void)
{
	m_added		=	false;
	m_nLanguage =	0;
	strcpy(m_lan, "cn");
}

CLanguageFile::~CLanguageFile(void)
{
	Destroy();
}

const char*		CLanguageFile::GetText(const char* szID, const char* szDefaultValue)
{CPUTime(CLanguageFile);
	for(int i=0;i<m_texts.size();i++)
	{
		if(stricmp(m_texts.at(i)->id, szID) == 0)
			return m_texts.at(i)->value;
	}
	return szDefaultValue;
}

#ifdef WIN32_OS
VOID	ReplaceFromTable(wchar_t* buffer, const wchar_t* szEnText)
{
	while(*szEnText != 0)
	{
		if(*szEnText == L'\t')
		{
			szEnText++;
			*buffer	=	L'<';
			buffer++;
			*buffer	=	L't';
			buffer++;
			*buffer	=	L'a';
			buffer++;
			*buffer	=	L'b';
			buffer++;
			*buffer	=	L'>';
			buffer++;
			continue;
		}
		else if(*szEnText == L'\r')
		{
			szEnText++;
			continue;
		}
		else if(*szEnText == L'\n')
		{
			szEnText++;
			*buffer	=	L'<';
			buffer++;
			*buffer	=	L'/';
			buffer++;
			*buffer	=	L'b';
			buffer++;
			*buffer	=	L'r';
			buffer++;
			*buffer	=	L'>';
			buffer++;
			continue;
		}
		*buffer	=	*szEnText;
		szEnText++;
		buffer++;
	}
	*buffer	=	0;
}

VOID	ReplaceToTable(wchar_t* buffer, const wchar_t* szEnText)
{
	while(*szEnText != 0)
	{
		if(wcsnicmp(szEnText, L"<tab>", 5) == 0)// == '' && szEnText[1] == 't')
		{
			*buffer	=	L'\t';
			buffer++;
			szEnText	+=	5;
			continue;
		}
		else if(wcsnicmp(szEnText, L"</br>", 5) == 0)// == '' && szEnText[1] == 't')
		{
			*buffer	=	L'\n';
			buffer++;
			szEnText	+=	5;
			continue;
		}
		*buffer	=	*szEnText;
		szEnText++;
		buffer++;
	}
	*buffer	=	0;
}

VOID				CLanguageFile::SetText(const char* szID, const char* szDefaultValue)	//设置一个配置信息
{CPUTime(CLanguageFile);
	for(int i=0;i<m_texts.size();i++)
	{
		if(stricmp(m_texts.at(i)->id, szID) == 0)
		{
			if(stricmp(m_texts.at(i)->value, szDefaultValue) == 0)
				return;
			m_added		=	true;
			m_texts.at(i)->value	=	szDefaultValue;
			return;
		}
	}
	ConfigText* l = new ConfigText;
	l->id		=	szID;
	l->value	=	szDefaultValue;
	m_texts.push_back( l );
	m_added		=	true;
}

wchar_t*	ConvertUnicodeAB(wchar_t* ptr)
{
	wchar_t* ori	=	ptr;
	while(*ptr != NULL)
	{
		char* ch	=	(char*)ptr;
		char c	=	ch[1];
		ch[1]	=	ch[0];
		ch[0]	=	c;
		ptr++;
	}
	return ori;
}

extern	const char*	GetDocumentPath();
VOID	CLanguageFile::SaveToFile()		//表格文件名字
{CPUTime(CLanguageFile);
	if(!m_added)return;
	char file[512];
	strcpy(file, "lan_cn.txt");
	CFileHelperW w;
	if(!w.create(file))return;
	wchar_t buf[10240];
	wchar_t id[2048], value[2048];
	swprintf(buf, L"id\tvalue\r\n");
	char cc[2] = {-2,-1};
	w.pStream->write(cc, sizeof(cc));
	int len	=	wcslen(buf);
	w.pStream->write(ConvertUnicodeAB(buf), len * sizeof(wchar_t));

	for(int i=0;i<m_texts.size();i++)
	{
		ConfigText* l = m_texts.at(i);
		id[0]		=	0;
		value[0]	=	0;
		int len = ::MultiByteToWideChar(CP_ACP, 0, l->id, l->id.size(), buf, 10240);
		buf[len] = 0;
		ReplaceFromTable(id, buf);
		len = ::MultiByteToWideChar(CP_ACP, 0, l->value, l->value.size(), buf, 10240);
		buf[len] = 0;
		ReplaceFromTable(value, buf);
		//
		swprintf(buf, L"\"%s\"\t\"%s\"\r\n", id, value);
		len		=	wcslen(buf);
		w.pStream->write(ConvertUnicodeAB(buf), len * sizeof(wchar_t));
	}
	m_added	=	false;
	Destroy();
}
#endif

VOID	CLanguageFile::Destroy()
{CPUTime(CLanguageFile);
	for(int i=0;i<m_texts.size();i++)
		delete m_texts.at(i);
	m_texts.clear();
}

const char* CLanguageFile::GetPath(int type)
{CPUTime(CLanguageFile);
	switch(type)
	{
	case English:
		return "en";
	case Korean:
		return "kr";
	case Japaness:
		return "jp";
	case Chiness:
		return "cn";
	case ChinessFan:
		return "cn2";
	}
	return "";
}
VOID	ReplaceToTable(char* buffer, const char* szEnText)
{
	while(*szEnText != 0)
	{
		if(strnicmp(szEnText, "<tab>", 5) == 0)// == '' && szEnText[1] == 't')
		{
			*buffer	=	'\t';
			buffer++;
			szEnText	+=	5;
			continue;
		}
		else if(strnicmp(szEnText, "</br>", 5) == 0)// == '' && szEnText[1] == 't')
		{
			*buffer	=	'\n';
			buffer++;
			szEnText	+=	5;
			continue;
		}
		*buffer	=	*szEnText;
		szEnText++;
		buffer++;
	}
	*buffer	=	0;
}

const char*		CLanguageFile::GetDlgFile(const char* szFile)
{CPUTime(CLanguageFile);
	FileName fn = szFile;
	FileName ext,fname;
	fn.setIsFile();
	fn.splitpath(0, &fname, &ext);
	if(ext=="")
		ext = ".rpgui";
	static char ret[256];
	strcpy(ret, "dlgs/");
	strcat(ret, fname.t);
	strcat(ret, ext.t);//".rpgui");
	if(fexist(ret))
		return ret;
	sprintf(ret, "dlgs/%s/%s%s", this->m_lan, fname.t, ext.t);
	return ret;
}


VOID	CLanguageFile::LoadFromFile(const char* lanFile)	//读取文件
{CPUTime(CLanguageFile);
	Destroy();
	char buf[128];
	strcpy(m_lan, lanFile);
	sprintf(buf, "dlgs/%s.txt", m_lan);
	TableTxt* r	=	loadTable_s(buf);
	if(!r)return;
	char id[2048];
	char value[2048];
	for(int i=0;i<r->getNumLine();i++)
	{
		id[0] = 0;
		value[0] = 0;
		r->getLineItem(i, "id", id, 2048);
		r->getLineItem(i, "value", value, 2048);
		////
		//char r_id[2048], r_value[2048];
		//ReplaceToTable(r_id, id);
		//ReplaceToTable(r_value, value);
		//
		ConfigText* l = new ConfigText;
		l->id		=	id;
		l->value	=	value;
		m_texts.push_back( l );
	}
	safe_delete(r);
}

};
