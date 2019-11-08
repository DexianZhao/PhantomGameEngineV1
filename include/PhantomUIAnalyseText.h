//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UIANALYSETEXT_H__
#define	__PHANTOM_UIANALYSETEXT_H__

#include "PhantomBase.h"
namespace Phantom{

struct	AnalyseTextParam
{
	int						parameter;
	std::string				function_name;
	std::string				dest_text;
	int						color;
};

struct SpeakParameter
{
	enum{
		SP_TYPE_COLOR	=	1,
		SP_TYPE_URL		=	2,
		SP_TYPE_IMAGE	=	3,
		SP_TYPE_FONT	=	4,
	};
	unsigned int	c;
	int				begin;
	int				count;
	int				type;
	int				eventID;
	TextAW			url;
	Name			cursor;
};

class CSpeakParameter
{
public:
	SpeakParameter*			GetParameter(INT nIndex){if(nIndex < 0 || nIndex >= (int)m_parameters.size())return NULL; return &m_parameters[nIndex];}
	INT						GetParameterCount(){return (INT)m_parameters.size();}
	VOID					AddParameter(const SpeakParameter& sp){m_parameters.push_back(sp);}
	VOID					RemoveAt(INT nIndex){if(nIndex < 0 || nIndex >= (INT)m_parameters.size())return; m_parameters.eraseByIndex(nIndex);}
	VOID					RemoveAll(){m_parameters.clear();}	//全部删除
public:
	CDynamicArray<SpeakParameter>	m_parameters;
};

class UIControl;
struct FunctionText;
class UIAnalyseText
{
public:
	UIAnalyseText(void);
	~UIAnalyseText(void);
	//
	CSpeakParameter*	CreateParameter(){return new CSpeakParameter();}
	const char*			AnalyseText(const char* text, CSpeakParameter* sp);
	const char*			AnalyseFromFile(const char* szFile, CSpeakParameter* sp);
	//
	BOOL				SetAnalyseText(UIControl* pFormatText, const char* text);
	BOOL				SetAnalyseFromFile(UIControl* pFormatText, const char* szFile);
	BOOL				SetAnalyseFromSaved(UIControl* pFormatText, const char* text, CSpeakParameter* sp);
	VOID				SetAnalyseFunction(const char* function_name, const char* dest_text, int color = 0xffffffff);	//将分析器内函数替换为目标文本
	std::vector<AnalyseTextParam>			m_analyses;
protected:
	void				analyseFunction(char* wbuffer, std::vector<FunctionText>& ret, int length);
	const char*			analyseFunctionText(std::vector<FunctionText>& texts, CSpeakParameter* sp);
};


};

#endif
