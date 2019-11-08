/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "PhantomManager.h"
class	CCodeClass;

class	CCodeMember
{
public:
	CCodeMember(){classPtr = 0; returnPtr = 0; funcPtr = 0; isEventIndex=-1;m_bIsModule=false;}
	//
	Phantom::NameW			name;
	Phantom::NameW			returnClass;	//返回类型
	int				isEventIndex;//是否事件绑定函数
	Phantom::NameW			eventName;
	Phantom::TextW	eventText;//事件文本
	BOOL			IsExistEvent(){return isEventIndex>=0;}
public:
	BOOL			m_bIsModule;
	CCodeClass*		returnPtr;
	CCodeClass*		classPtr;
	CCodeMember*	funcPtr;
	Phantom::TextW	args;
	Phantom::TextW	description;
	virtual	BOOL	IsClassMember(){return false;}
	virtual	BOOL	IsEnumMember(){return false;}
	//
	enum CodeMemberType{
		CodeMemberType_Void	=	0,
		CodeMemberType_Variant,
		CodeMemberType_Class,
		CodeMemberType_Module,
		CodeMemberType_Struct,	//类型
		CodeMemberType_Enum_Value,
		CodeMemberType_Enum,
		CodeMemberType_Event,
		CodeMemberType_Table,
		CodeMemberType_Keywords,
	};
	CodeMemberType	type;
};

class	CCodeClass: public CCodeMember
{
public:
	virtual	BOOL	IsClassMember(){return true;}
	CCodeClass();
	~CCodeClass();
	VOID		SearchReturnMembers();
	BOOL		IsEquip(CCodeClass& c);
	std::vector<CCodeMember*>				members;
	std::vector<CCodeClass*>				groups;
	CCodeMember*							FindMember(const wchar_t* szName);
	CCodeMember*							AddMember(const wchar_t* szName, const wchar_t* description, const wchar_t* returnClass, const wchar_t* args, CCodeMember::CodeMemberType type, BOOL bIsModule=false);
	VOID									SortMembers();
	virtual	BOOL	IsEnumMember(){return m_bIsEnum;}
public:
	BOOL		m_bIsLoad;//如果是载入的类
	BOOL		m_bIsEnum;
};

class CCodeXmlManager
{
public:
	CCodeXmlManager(void);
	~CCodeXmlManager(void);
	VOID							LoadContent(CDynamicArray<char>& codeXml);
	VOID							ResetGameContent();
	CCodeClass*						FindClass(const wchar_t* szClass);
	BOOL							AddMyClass(CCodeClass* cc);
	CCodeMember*					FindMethod(const wchar_t* m);
	CCodeMember*					FindVariant(const wchar_t* m);
	//
	CCodeClass*						m_globalClass;
	//
	VOID							SortMembers();
	VOID							ResetBaseClass();
	VOID							ResetVariants();
	VOID							AddMyMethod(CCodeMember* m);
	CCodeMember*					AddMyVariant(const wchar_t* name);
	VOID							AddUIClass(const wchar_t* fileName);//根据对话框文件读取变量表
	VOID							AddUIClass(Phantom::UIDialog* dlg);//重新初始化某个对话框的变量列表,会自动保存到配置文件中
	VOID							UpdateUIClass(CCodeClass* classPtr, Phantom::UIDialog* dlg);//重新初始化某个对话框的变量列表,会自动保存到配置文件中
	//
	std::vector<CCodeMember*>		m_methods;//全局可以被调用的方法和变量
	std::vector<CCodeMember*>		m_variants;
	std::vector<CCodeMember*>		m_savemethods;//全局可以被调用的方法和变量
	std::vector<CCodeMember*>		m_savevariants;
	std::vector<CCodeClass*>		m_uiclass;//UI对话框类,一个对话框对应一个类
	//
	std::vector<CCodeClass*>		m_saveClass;	//类型列表
	std::vector<CCodeClass*>		m_baseClass;	//类型列表
	std::vector<CCodeMember*>		m_dataTypes;
	//游戏相关
	CCodeClass*						m_globalGame;
	std::vector<CCodeClass*>		m_gameClass;	//游戏类,包括数据类
	std::vector<CCodeMember*>		m_gameMethods;	//游戏函数,包括数据函数
	std::vector<CCodeMember*>		m_gameVariants;	//游戏变量,包括数据变量
};

CCodeXmlManager*		GetCodeXmlManager();

