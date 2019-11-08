/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "GCLanguage.h"

LanguagesType	g_languageType	=	Languages_Chiness;

BOOL			IsChiness()
{
	return (g_languageType	==	Languages_Chiness);
}

const wchar_t*	Language(const wchar_t* szID, const wchar_t* szDefault)
{
#ifdef _DEBUG
	GetLanguagesDebug()->SetText(szID, szDefault);
#endif
	if(GetLanguagesPtr()->m_language == Languages_Chiness)
		return szDefault;
	return GetLanguagesPtr()->GetText(szID, szDefault);
}

const wchar_t*	Language(const wchar_t* szIDAndDefault)
{
	return Language(szIDAndDefault, szIDAndDefault);
}

const char*		Language(const char* szID, const char* szDefault)
{
#ifdef _DEBUG
	GetLanguagesDebug()->SetText(W(szID), W(szDefault));
#endif
	if(GetLanguagesPtr()->m_language == Languages_Chiness)
		return szDefault;
	static std::string temp;
	temp = A(Language(W(szID), W(szDefault))).buf;
	return temp.c_str();
}

INT				GetLanguageFontSize(LanguagesType type)
{
	return _wtoi(Language(L"Size", L"12"));
	//if(type == Languages_Current)
	//	type	=	g_languageType;
	//switch(type)
	//{
	//case Languages_Chiness:
	//case Languages_ChinessF:
	//	return 12;
	//case Languages_English:
	//	return 15;
	//case Languages_Korean:
	//	return 12;
	//case Languages_Japan:
	//	return 15;
	//case Languages_Russia:
	//	return 15;
	//}
	//return 12;
}

CIDEConfigFile::CIDEConfigFile(void)
{
	m_added		=	false;
}

CIDEConfigFile::~CIDEConfigFile(void)
{
	Destroy();
}

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

VOID				CIDEConfigFile::SetText(const wchar_t* szID, const wchar_t* szDefaultValue)	//设置一个配置信息
{
	for(int i=0;i<m_texts.size();i++)
	{
		if(wcsicmp(m_texts.at(i)->id.c_str(), szID) == 0)
		{
			if(wcsicmp(m_texts.at(i)->value.c_str(), szDefaultValue) == 0)
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

const wchar_t*		CIDEConfigFile::GetText(const wchar_t* szID, const wchar_t* szDefaultValue)
{
	for(int i=0;i<m_texts.size();i++)
	{
		if(wcsicmp(m_texts.at(i)->id.c_str(), szID) == 0)
			return m_texts.at(i)->value.c_str();
	}
	return szDefaultValue;
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
VOID	CIDEConfigFile::SaveToFile()		//表格文件名字
{
	if(!m_added)return;
	char file[512];
	sprintf(file, "%s\\language.txt", GetDocumentPath());
	//CFileHelperW w;
	//if(!w.create(file))return;
	wchar_t buf[10240];
	wchar_t id[2048], value[2048];
	swprintf(buf, L"id\tvalue\r\n");
	std::wstring tempBuf;
	tempBuf = buf;
	//char cc[2] = {-2,-1};
	//w.pStream->write(cc, sizeof(cc));
	int len	=	wcslen(buf);
	tempBuf += ConvertUnicodeAB(buf);
	//w.pStream->write(ConvertUnicodeAB(buf), len * sizeof(wchar_t));

	for(int i=0;i<m_texts.size();i++)
	{
		ConfigText* l = m_texts.at(i);
		id[0]		=	0;
		value[0]	=	0;
		ReplaceFromTable(id, (const wchar_t*)l->id.c_str());
		ReplaceFromTable(value, (const wchar_t*)l->value.c_str());
		//
		swprintf(buf, L"\"%s\"\t\"%s\"\r\n", id, value);
		tempBuf += ConvertUnicodeAB(buf);
		//len		=	wcslen(buf);
		//w.pStream->write(ConvertUnicodeAB(buf), len * sizeof(wchar_t));
	}
	char* tempText = new char[tempBuf.size()*2+1];
	len = ::WideCharToMultiByte(CP_UTF8, 0, &tempBuf.at(0), tempBuf.size(), tempText, tempBuf.size()*2+1, 0, 0);
	tempText[len] = 0;
	Phantom::SaveUtf8(file, tempText);
	delete[] tempText;
	m_added	=	false;
	Destroy();
}

VOID	CIDEConfigFile::Destroy()
{
	for(int i=0;i<m_texts.size();i++)
		delete m_texts.at(i);
	m_texts.clear();
}
extern	VOID	WriteRegValue(const char* szKey, const char* szText);
VOID	CIDEConfigFile::ChangeLanguage(LanguagesType type)		//切换语言
{
	char buf[128];
	sprintf(buf, "%d", type);
	//::WriteProfileStringA("PhantomVD", "Language", buf);
	WriteRegValue("Language", buf);
	LoadFromFile();
}

const char* GetLanguageFile(int type)
{
	switch(type)
	{
	case Languages_English:
		return "English";
	case Languages_Korean:
		return "Korean";
	case Languages_Japan:
		return "Japan";
	case Languages_Russia:
		return "Russia";
	case Languages_ChinessF:
		return "ChinessT";
	}
	return "";
}
extern	BOOL	ReadRegValue(const char* szKey, char* szText);
VOID	CIDEConfigFile::LoadFromFile()	//读取文件
{
	Destroy();
	char buf[512];
	strcpy(buf, "1");
	ReadRegValue("Language", buf);
	//::GetProfileStringA("PhantomVD", "Language", "1", buf, 128);
	int nLanguage	=	atoi(buf);//Languages_Chiness);
	if(nLanguage == Languages_Chiness)
		return;
	wchar_t szDir[512], szDrive[512];
	_wsplitpath(_wpgmptr, szDir, szDrive, 0, 0);
	wcscat(szDir, szDrive);
	//
	m_language		=	(LanguagesType)nLanguage;
	sprintf(buf, "%s\\language\\language_%s.txt", A(szDir).buf, GetLanguageFile(nLanguage));
	TableTxt* r	=	Phantom::loadTable_s(buf);
	m_configFile	=	buf;
	if(!r)return;
	char cid[2048];
	char cvalue[2048];
	wchar_t id[2048];
	wchar_t value[2048];
	for(int i=0;i<r->getNumLine();i++)
	{
		r->getLineItem(i, "id", cid, 2048);
		r->getLineItem(i, "value", cvalue, 2048);
		int len = ::MultiByteToWideChar(CP_UTF8, 0, cid, strlen(cid), id, 2048);
		id[len] = 0;
		len = ::MultiByteToWideChar(CP_UTF8, 0, cvalue, strlen(cvalue), value, 2048);
		value[len] = 0;
		//
		wchar_t r_id[2048], r_value[2048];
		ReplaceToTable(r_id, id);
		ReplaceToTable(r_value, value);
		//
		ConfigText* l = new ConfigText;
		l->id		=	r_id;
		l->value	=	r_value;
		m_texts.push_back( l );
	}
	safe_release(r);
}
VOID	CIDEConfigFile::LoadDebug()							//读取文件
{
	char file[512];
	sprintf(file, "%s\\language.txt", GetDocumentPath());
	CFileHelperW w;
	TableTxt* r	=	Phantom::loadTable_s(file);
	m_configFile	=	file;
	if(!r)return;
	char cid[2048];
	char cvalue[2048];
	wchar_t id[2048];
	wchar_t value[2048];
	for(int i=0;i<r->getNumLine();i++)
	{
		r->getLineItem(i, "id", cid, 2048);
		r->getLineItem(i, "value", cvalue, 2048);
		int len = ::MultiByteToWideChar(CP_UTF8, 0, cid, strlen(cid), id, 2048);
		id[len] = 0;
		len = ::MultiByteToWideChar(CP_UTF8, 0, cvalue, strlen(cvalue), value, 2048);
		value[len] = 0;
		//
		wchar_t r_id[2048], r_value[2048];
		ReplaceToTable(r_id, id);
		ReplaceToTable(r_value, value);
		//
		ConfigText* l = new ConfigText;
		l->id		=	r_id;
		l->value	=	r_value;
		m_texts.push_back( l );
	}
}
BOOL	CALLBACK	EnumProcs(HWND hWndChild, LPARAM lpParam)
{
	CIDEConfigFile* lan = (CIDEConfigFile*)lpParam;
	if(!lan)return false;
	int id	=	::GetDlgCtrlID(hWndChild);
	wchar_t buf[1024];
	::GetWindowText(hWndChild, buf, 1024);
	if(buf[0])
	{
		const wchar_t*	szText	=	lan->GetText(buf, buf);
		::SetWindowText(hWndChild, szText);
	}
	if(::GetMenu(hWndChild))
	{
		GetLanguagesPtr()->SetMenusText(GetMenu(hWndChild));
	}
	EnumChildWindows(hWndChild, EnumProcs, lpParam);
	return true;
}

VOID	CIDEConfigFile::SetMenusText(HMENU hMenu)
{
	HMENU hMenu2	=	::GetSubMenu(hMenu, 0);
	int nCount	=	::GetMenuItemCount(hMenu);
	for(int i=0;i<nCount;i++)
	{
		wchar_t buffer[1024];
		buffer[0] = 0;
		if(::GetMenuString(hMenu, i, buffer, 1024, MF_BYPOSITION) > 0)
		{
			const wchar_t* text	=	this->GetText(buffer, buffer);
			ModifyMenu(hMenu, i, MF_BYPOSITION|MF_STRING, GetMenuItemID(hMenu, i), text);
			HMENU hSubMenu	=	::GetSubMenu(hMenu, i);
			if(hSubMenu)
				SetMenusText(hSubMenu);
		}
	}
}

VOID	CIDEConfigFile::SetChildsText(HWND hWnd)
{
	EnumChildWindows(hWnd, EnumProcs, (LPARAM)this);
}

CIDEConfigFile*		g_languageIDE = 0;
CIDEConfigFile*		g_languageDebug = 0;
CIDEConfigFile*		GetLanguagesPtr()
{
	if(!g_languageIDE)
		g_languageIDE	=	new CIDEConfigFile();
	return g_languageIDE;
}

CIDEConfigFile*		GetLanguagesDebug()
{
	if(!g_languageDebug)
		g_languageDebug	=	new CIDEConfigFile();
	return g_languageDebug;
}

VOID				DeleteLanguages()
{
	if(g_languageIDE)
		delete g_languageIDE;
	g_languageIDE	=	0;
	if(g_languageDebug)
		delete g_languageDebug;
	g_languageDebug	=	0;
}
