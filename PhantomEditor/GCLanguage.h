/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "GCMFCHelper.h"

enum LanguagesType
{
	Languages_Current	=	0,
	Languages_Chiness,	//中文
	Languages_English,	//英文
	Languages_Korean,	//韩文
	Languages_Japan,	//日文
	Languages_Russia,	//俄文
	Languages_ChinessF,	//繁体
};

//转换成当前所指定的国际语言
const wchar_t*	Language(const wchar_t* szID, const wchar_t* szDefault);
const wchar_t*	Language(const wchar_t* szIDAndDefault);
INT				GetLanguageFontSize(LanguagesType type = Languages_Current);
const char*		Language(const char* szID, const char* szDefault);

BOOL			IsChiness();

struct ConfigText
{
	std::wstring				id;
	std::wstring				value;
	VOID						Destroy()
	{
		id.clear();
		value.clear();
	}
	ConfigText(){}
	~ConfigText(){Destroy();}
};

class CIDEConfigFile
{
public:
	CIDEConfigFile(void);
	~CIDEConfigFile(void);
	//
	const wchar_t*				GetText(const wchar_t* szID, const wchar_t* szDefaultValue);	//创建一个配置信息
	VOID						SetText(const wchar_t* szID, const wchar_t* szDefaultValue);	//设置一个配置信息
	//
	VOID						SetChildsText(HWND hWnd);
	VOID						SetMenusText(HMENU hMenu);
	//
	VOID						SaveToFile();							//表格文件名字
	VOID						LoadFromFile();							//读取文件
	VOID						LoadDebug();							//读取文件
	VOID						ChangeLanguage(LanguagesType type);		//切换语言
	VOID						Destroy();
public:
	LanguagesType				m_language;
	std::string					m_configFile;
	bool						m_added;
	std::vector<ConfigText*>	m_texts;
};

wchar_t*			ConvertUnicodeAB(wchar_t* w);
CIDEConfigFile*		GetLanguagesPtr();
CIDEConfigFile*		GetLanguagesDebug();
