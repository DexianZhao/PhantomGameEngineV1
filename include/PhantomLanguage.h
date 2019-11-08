#ifndef __PHANTOM_LANGUAGE_H__
#define	__PHANTOM_LANGUAGE_H__

#pragma once

#include "PhantomManager.h"
//转换成当前所指定的国际语言
namespace Phantom{

	const char*		Lan(const char* szID, const char* szDefault);
	struct ConfigText
	{
		Name					id;
		Text					value;
		ConfigText(){}
		~ConfigText(){
			value.clear();
		}
	};

	class CLanguageFile
	{
	public:
		enum Language
		{
			Chiness = 0,//中文简体
			English,//英文
			Korean,//韩文
			Japaness,//日文
			ChinessFan,//繁体
		};
		CLanguageFile(void);
		~CLanguageFile(void);
		//
		inline	BOOL				IsChiness(){return (m_nLanguage == Chiness);}
		inline	BOOL				IsChinessFan(){return (m_nLanguage == ChinessFan);}
		inline	BOOL				IsEnglish(){return (m_nLanguage == English);}
		inline	BOOL				IsKorean(){return (m_nLanguage == Korean);}
		inline	BOOL				IsJapaness(){return (m_nLanguage == Japaness);}
		//
		const char*					GetText(const char* szID, const char* szDefaultValue);	//创建一个配置信息
#ifdef WIN32_OS
		VOID						SetText(const char* szID, const char* szDefaultValue);	//设置一个配置信息
		VOID						SaveToFile();							//表格文件名字
#endif
		VOID						LoadFromFile(const char* lanFile);							//读取文件
		VOID						Destroy();
		const char*					GetDlgFile(const char* szFile);
		const char*					GetPath(){return m_lan;}
		const char*					GetPath(int type);
		char						m_lan[32];
	protected:
		int							m_nLanguage;
		std::vector<ConfigText*>	m_texts;
		BOOL						m_added;
	};

	CLanguageFile*					GetLanguagePtr();

#ifdef WIN32_OS
	CLanguageFile*					GetLanguageDebug();
#endif

};


#endif
