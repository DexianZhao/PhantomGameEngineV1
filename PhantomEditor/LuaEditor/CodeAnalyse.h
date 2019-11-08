/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include <vector>
#include <string>
#include <map>
#include "LuaXmlModule.h"
// CodeEditor
#pragma pack(push,1)
struct	MaskEvent{//事件
	Phantom::NameTW<32>	name;
	Phantom::NameTW<256>	fulls;
	char		bExist;//是否存在这个事件
	int			type,index;
};

struct MaskSection{
	Phantom::NameTW<32>	name;//最大不会超过function
	long		begin;
	long		count;
	enum MS{
		Normal = 0,//正常文本，变量或者其他
		Section = 1,//是段落
		Comment = 2,//是注释
	};
	char		type;//0=代码,1=section
//	MaskType	mask;//关键字类型
};
struct MaskVariant{
	Phantom::NameTW<32>		name;//变量或者函数名
	Phantom::NameTW<256>	classname;//对应的类名，如果指向其他的变量或者函数，会穷举返回类名
	char			classset;
	char			isLocal;//是否local定义
	long			lineBegin;
};
struct MaskText//每一个空格或者段落的字段
{
	enum MaskType
	{
		MaskType_None	=	0,
		MaskType_Keywords,		//关键字
		MaskType_Token,	//符号
		MaskType_Variant,
		MaskType_Method,
		MaskType_Text,
		MaskType_Comment,
		MaskType_Enum,
		MaskType_Number,	//数字
		MaskType_PhantomAPI,//幻影API函数
		MaskType_LuaAPI,//lua的API函数
		MaskType_Normal,//正常文本颜色
		MaskType_BackColor,//背景颜色
		MaskType_LineText,//行文本颜色
		MaskType_LineBack,//行背景颜色
		MaskType_Count,
	};
	Phantom::NameW		name;//名字
	MaskType	type;
	MaskText(const wchar_t* _name = 0, MaskType t = MaskType_None);
	static COLORREF	Read(int mask, COLORREF c);
	static VOID		Write(int mask, COLORREF c);
};
class CCodeFunction{
public:
	CCodeFunction(const wchar_t* name, long lineBegin);
	~CCodeFunction();
	BOOL						AddVar(const wchar_t* name, const wchar_t* classname, long lineEnd, BOOL bLocal);
	VOID						SetReturn(const wchar_t* classname);//返回值
	const MaskVariant*			SearchVar(const wchar_t* name);
	BOOL						SearchInVar(NameW& ret, const wchar_t* var, int nLine);//从变量搜索
public:
	char						m_isEvent;
	Phantom::NameTW<32>					m_name;//名字
	Phantom::NameTW<256>					m_classname;
	long						m_lineBegin;//函数开始位置
	long						m_lineEnd;//函数结束位置
	Phantom::CDynamicArray<MaskVariant>	m_vars;
};
#pragma pack(pop)

struct	TextColor
{
	COLORREF	c;
	BOOL		bBold;
	TextColor(COLORREF cc = 0, BOOL b = false){c = cc; bBold = b;}
	VOID	Setup(COLORREF cc, BOOL b){c = cc; bBold = b;}
};
struct	LineText{
	int						lineBegin;
	wchar_t				*	text;
};
class CCodeUndo{
public:
	CCodeUndo();
	~CCodeUndo();
	VOID		Begin(CRichEditCtrl* rec);//从richedit备份所有需要的行
	VOID		End(CRichEditCtrl* rec);//结束了一个撤销保存操作
public:
	int			m_oldBegin;//旧的选择
	int			m_oldEnd;
	int			m_oldLineBegin;//旧的行开始
	int			m_oldLineEnd;//旧的行结束
	Phantom::CDynamicArray<LineText>		m_oldLineTexts;//旧的所有行文本复制
public:
	int			m_newBegin;//新的选择
	int			m_newEnd;
	int			m_newLineBegin;//新的行开始
	int			m_newLineEnd;//新的行结束
	Phantom::CDynamicArray<LineText>		m_newLineTexts;//新的所有行复制
};

class CCodeLine{//行数据，会根据编辑而变动
public:
	CCodeLine();
	~CCodeLine();
	enum CL{
		CL_Normal = 0,//正常行
		CL_VarSet,//变量赋值
		CL_If,//if或者else
		CL_ElseIf,
		CL_Else,
		CL_Do,
		CL_End,
		CL_Function,
		CL_For,
		CL_Return,
		//CL_Until,
		//CL_While,
	};
	VOID						SetText(const wchar_t* str);//设置文本,会自动分割字符串
	inline	BOOL				IsLocalVar(){return m_bLocalVar;}//是否以local形式定义了变量
	inline	CL					GetType(){return m_type;}
	inline	BOOL				HasName(){return m_name.t[0];}
	inline	BOOL				HasClassName(){return m_classname.t[0];}
	//
	static BOOL					CommentClass(const wchar_t* text, int length, Phantom::NameTW<256>& rets);
	static VOID					RemoveSection(Phantom::CDynamicArray<MaskSection>& secs, wchar_t tbegin, wchar_t tend, int beginCaret = 0);//在段落中删除所有对应的标识符内容，比如tbegin='(',tend=')'删除里面的内容和符号
	static BOOL					GetSection(Phantom::CDynamicArray<MaskSection>& rets, Phantom::CDynamicArray<MaskSection>& fulls, wchar_t tbegin, wchar_t tend);//返回tbegin到tend之间的数据,如果其中又有其他的tbegin类型的数据，会出错
	static VOID					SplitSection(Phantom::CDynamicArray<MaskSection>* fulls, const wchar_t* text, int length, Phantom::CDynamicArray<MaskSection>* codes = 0);
	static BOOL					SectionClassList(Phantom::CDynamicArray<MaskSection>& secs, int begin, wchar_t* ret);//从段落列表中返回整个类，比如a.b.c:e().f等会返回a.b.c:e.f
public:
	Phantom::CDynamicArray<MaskSection>	m_codes;//段落，代码部分
	Phantom::CDynamicArray<MaskSection>	m_fulls;//段落，代码部分，和所有部分
	CString						m_text;//保存的文本

	Phantom::NameTW<32>					m_name;//函数名,变量名
	Phantom::NameTW<256>					m_classname;//类名
	CCodeClass*					m_class;//如果这个行是类声明，则存在这个,在xmlcode中不会自动删除，这个行内自动删除
	CCodeMember*				m_member;
	Phantom::CDynamicArray<CCodeMember*>	m_args;//参数列表
protected:
	VOID						CheckMyClass(const wchar_t* name);//从{}中创建一个新的类
protected:
	bool						m_bLocalVar;
	CL							m_type;//类型
	//line必须与richedit始终都对应上，要不然绝对会出错
};

class CodeManager{//代码管理器
public:
	CodeManager();
	~CodeManager();
	VOID							ClearAllLines();//分析所有行代码，并生成函数表和变量表,Reset和pushclass
	VOID							AnalyseAllLines();//分析所有行代码，并生成函数表和变量表,Reset和pushclass
	VOID							AddEvent(const wchar_t* w, const wchar_t* fulls, int type, int index);
	CCodeLine*						GetLine(int lineIndex){if(lineIndex<0||lineIndex>=m_lines.size())return 0; return m_lines[lineIndex];}
	CCodeClass*						GetVariantClassPtr(const wchar_t* vars, int nLine, BOOL bOther=true);//返回一个变量的类名
	CCodeMember*					GetFunctionMemberPtr(const wchar_t* vars, int nLine);//返回一个函数的方法指针
	void							RefreshVariantList(int nLine);//返回当前行内可以调用的变量列表
	void							RefreshVariantListL(int nLine);//返回当前行内可以调用的变量列表
	//
	BOOL							SearchInGlobalVar(NameW& ret, const wchar_t* var, int nLine);//从全局变量搜索
	CCodeFunction*					GetFunctionInLine(int line);
	CCodeMember*					SearchFunction(const wchar_t* func);
	//
	VOID							Reset();
public://
	VOID							OnInsertLine(int lineIndex, const wchar_t* text);//新插入一行到这里
	VOID							OnRemoveLine(int lineIndex);//删除一行
	BOOL							SplitAllCalls(std::vector<NameW>& rets, NameW& first/*第一个名字*/, const wchar_t* vars, int nLine);//返回所有指向
public:
	std::vector<CCodeLine*>			m_lines;
	Phantom::CDynamicArray<CCodeFunction*>	m_functions;//本地函数列表，按照顺序加入
	Phantom::CDynamicArray<MaskVariant>		m_variants;//全局函数列表,变量表只有全局表和local表，如果没有声明local的变量则在全局表中存在
	Phantom::CDynamicArray<MaskEvent>		m_events;//可以被添加的事件列表,从外部设置
protected:
	int								LoopForEnd(int beginLine, CCodeFunction* func);//返回这个行开始的结束符，end对应
};
