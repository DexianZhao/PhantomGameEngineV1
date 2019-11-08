/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "LuaXmlModule.h"
#include "CodeAnalyse.h"
#ifdef EDITOR_OS
#include "PhantomDB.h"
#include "GCViewBase.h"
#endif
#include "PhantomLanguage.h"
#include "base_xmlFileW.h"

CCodeXmlManager*		g_luaCodeManager	=	0;
CCodeXmlManager*		GetCodeXmlManager()
{
	if(!g_luaCodeManager)
		g_luaCodeManager	=	new CCodeXmlManager();
	return g_luaCodeManager;
}
VOID	DeleteCodeManager()
{
	if(g_luaCodeManager)
		delete g_luaCodeManager;
	g_luaCodeManager	=	0;
}

CCodeClass::CCodeClass()
{
	m_bIsLoad = false;
	m_bIsEnum = false;
}
CCodeClass::~CCodeClass()
{
	for(int i=0;i<members.size();i++)
		delete members[i];
	for(int i=0;i<groups.size();i++)
		delete groups[i];
}
VOID	RemoveTab(wchar_t* t){
	int len = wcslen(t);
	for(int i=0;i<len;i++)
	{
		if(t[i] == L'\t')
			t[i] = L' ';
	}
}
CCodeMember*	CCodeClass::AddMember(const wchar_t* szName, const wchar_t* description, const wchar_t* returnClass, const wchar_t* args, CCodeMember::CodeMemberType type, BOOL bIsModule)
{
	CCodeMember* m	=	new CCodeMember();
	m->name			=	szName;
	m->m_bIsModule	=	bIsModule;
	m->returnClass	=	returnClass;
	m->args			=	args;
	m->type			=	type;
	m->description	=	description;
	RemoveTab((wchar_t*)m->description.str());
	m->classPtr		=	this;
	m->returnPtr	=	0;
	members.push_back( m );
	//
	const wchar_t* szEvent = wcsstr(m->description.str(), L"事件[");
	if(szEvent){
		szEvent += 3;
		const wchar_t* szEventEnd = wcsstr(szEvent, L"]");
		wchar_t buf[512];
		memset(buf, 0, sizeof(wchar_t)*512);
		wcsncpy(buf, szEvent, szEventEnd-szEvent);
		int beginS = 0;
		int len = wcslen(buf);
		for(int i=0;i<len;i++){
			if(buf[i]==L'(')
			{
				beginS = i;
				break;
			}
		}
		memset(&m->eventName, 0, sizeof(m->eventName));
		wcsncpy(m->eventName.t, buf, beginS);
		m->eventText = buf+beginS;
		Phantom::CDynamicArray<MaskSection> codes;
		CCodeLine::SplitSection(0, m->args.str(), m->args.size(), &codes);
		m->isEventIndex=-1;
		bool begin = false;
		for(int i=0;i<codes.size();i++){
			if(codes[i].name[0]==L'('){
				begin=true;
				m->isEventIndex=0;
			}
			else if(begin&&codes[i].name[0]==L','){
				m->isEventIndex++;
			}
			else if(begin){
				if(wcsicmp(codes[i].name, m->eventName)==0)
					break;
			}
		}
		//m->isEventIndex = 1;
		//int				isEventIndex;//是否事件绑定函数
		//std::wstring	eventText;//事件文本
	}
	//
	return m;
}

CCodeMember*		CCodeClass::FindMember(const wchar_t* szName)
{
	int len = wcslen(szName);
	for(int i=0;i<members.size();i++)
	{
		if(members[i]->name.t[len]==L'['){
			if(wcsnicmp(members[i]->name.t, szName, len) == 0)
				return members[i];
		}
		else if(members[i]->name == szName)
			return members[i];
	}
	return 0;
}

int		SortMemberProc(const void* v1, const void* v2)
{
	CCodeMember* m1 = *(CCodeMember**)v1;
	CCodeMember* m2 = *(CCodeMember**)v2;
	if(m1->type < m2->type)
		return 1;
	if(m1->type > m2->type)
		return -1;
	return wcscmp(m1->name, m2->name);
}
VOID	CCodeClass::SortMembers()
{
	if(members.size() > 1)
		qsort(&members.at(0), members.size(), sizeof(CCodeMember*), SortMemberProc);
}

CCodeXmlManager::CCodeXmlManager()
{
	//g_luaCodeManager	=	this;
	m_globalClass = 0;
	m_globalGame = 0;
}
CCodeXmlManager::~CCodeXmlManager()
{
	if(m_globalClass)
		delete m_globalClass;
	if(m_globalGame)
		delete m_globalGame;
	for(int i=0;i<m_saveClass.size();i++)
	{
		delete m_saveClass[i];
	}
	for(int i=0;i<m_dataTypes.size();i++)
		delete m_dataTypes[i];
	for(int i=0;i<m_gameClass.size();i++)
		delete m_gameClass[i];
	for(int i=0;i<m_gameVariants.size();i++)
		delete m_gameVariants[i];
	for(int i=0;i<m_uiclass.size();i++)
		delete m_uiclass[i];
}
VOID	CCodeXmlManager::AddUIClass(Phantom::UIDialog* dlg)//重新初始化某个对话框的变量列表,会自动保存到配置文件中
{
	Phantom::FileName file;
	dlg->m_strFile.splitpath(0, &file, 0);
	Wutf8 wf(file.t);
	NameW nw = L"dlgs_";
	nw += wf.buf;
	for(int i=0;i<m_uiclass.size();i++){
		if(m_uiclass[i]->name==nw){
			UpdateUIClass(m_uiclass[i], dlg);
			return;
		}
	}
	CCodeClass* ptr = new CCodeClass();
	ptr->name = L"dlgs_";
	ptr->name += wf.buf;
	ptr->returnClass = wf.buf;
	m_uiclass.push_back(ptr);
	UpdateUIClass(ptr, dlg);
	ResetBaseClass();
}
VOID	CCodeXmlManager::AddUIClass(const wchar_t* fileName)//根据对话框文件读取变量表
{
	NameW temp = L"dlgs_";
	temp += fileName;
	for(int i=0;i<m_uiclass.size();i++){
		if(m_uiclass[i]->name==temp){//如果已经存在了就不再读取了
			return;
		}
	}
	const char* szFile = Phantom::GetLanguagePtr()->GetDlgFile(Autf8(fileName));
	for(int i=0;i<Phantom::GetDlgManager()->GetDialogManager().m_lists.size();i++){
		if(stricmp(szFile, Phantom::GetDlgManager()->GetDialogManager().m_lists[i]->m_strFile.t)==0){
			AddUIClass(Phantom::GetDlgManager()->GetDialogManager().m_lists[i]);
			return;
		}
	}
	Phantom::Dialog dlg;
	if(!Phantom::GetDlgManager()->LoadDialog(dlg, szFile))
		return;
	AddUIClass(dlg);
}
struct	XmlControlVarAry{
	int		id;
	short	aryindex;
};
struct	XmlControlVar{
	Phantom::Name	name;
	short	isary;//-1
	int		id;
};
struct	XmlGroupVar{
	Phantom::CDynamicArray<XmlControlVar>	vars;
	Phantom::Name	name;
	int				id;
};
extern BOOL	issection(wchar_t c);
VOID	CCodeXmlManager::UpdateUIClass(CCodeClass* ptr, Phantom::UIDialog* dlg)//重新初始化某个对话框的变量列表,会自动保存到配置文件中
{
	for(int i=0;i<ptr->members.size();i++){
		delete ptr->members[i];
	}
	ptr->members.clear();
	for(int i=0;i<ptr->groups.size();i++){
		delete ptr->groups[i];
	}
	ptr->groups.clear();
	//
	Phantom::CDynamicArray<XmlGroupVar*> groups;
	for(int i=0;i<dlg->GetControlCount();i++){
		Phantom::UIControl* c = dlg->GetControlPtr(i);
		if(strnicmp(c->GetName(), "var_", 4)==0){
			Phantom::Name temp;
			temp = c->GetName() + 4;
			BOOL bIsArray = false;
			int arrayIndex = -1;
			const char* ab = strstr(temp.t, "[");
			if(ab){
				arrayIndex = atoi(ab+1);
				temp.t[ab-temp.t] = 0;
				bIsArray = true;
			}
			int len = temp.size();
			for(int k=0;k<len;k++){
				if(issection(temp[k])){
					temp.t[k] = '_';
				}
			}
			XmlGroupVar* gvar = 0;
			for(int g=0;g<groups.size();g++){
				if(groups[g]->id==c->m_groupID.id){
					gvar = groups[g];
					break;
				}
			}
			if(!gvar){
				gvar = new XmlGroupVar();
				gvar->id = c->m_groupID.id;
				Phantom::Name gtemp;
				gtemp = c->GetGroupPtr()->GetName();
				int len = gtemp.size();
				for(int k=0;k<len;k++){
					if(issection(gtemp[k])){
						gtemp.t[k] = '_';
					}
				}
				gvar->name = gtemp;
				groups.push_back(gvar);
			}
			bool bFind = false;
			for(int k=0;k<gvar->vars.size();k++){
				if(gvar->vars[k].name==temp.t){
					bFind = true;
					break;
				}
			}
			if(!bFind){
				XmlControlVar cv;
				cv.name =temp;
				cv.isary=bIsArray;
				gvar->vars.push_back(cv);
			}
		}
	}
	CCodeClass* cui = this->FindClass(L"UIControl");
	char buf[128];
	for(int i=0;i<groups.size();i++){
		if(groups[i]->id==0){
			for(int k=0;k<groups[i]->vars.size();k++){
				if(groups[i]->vars[k].isary){
					sprintf(buf, "%s[]", groups[i]->vars[k].name.t);
					CCodeMember* m = ptr->AddMember(Wutf8(buf), L"界面控件[]数组", L"UIControl", L"", CCodeMember::CodeMemberType_Variant, false);
					if(m)
						m->returnPtr = cui;
				}
				else{
					CCodeMember* m = ptr->AddMember(Wutf8(groups[i]->vars[k].name.t), L"界面控件", L"UIControl", L"", CCodeMember::CodeMemberType_Variant, false);
					if(m)
						m->returnPtr = cui;
				}
			}
			continue;
		}
		wchar_t wbuf[128];
		swprintf(wbuf, L"%s_%s", ptr->name.t, Wutf8(groups[i]->name.t).buf);
		CCodeClass* gptr = new CCodeClass();
		gptr->name = wbuf;
		gptr->returnClass = wbuf;
		ptr->groups.push_back(gptr);
		{
			CCodeMember* m = ptr->AddMember(Wutf8(groups[i]->name.t), L"界面组合类", wbuf, L"", CCodeMember::CodeMemberType_Variant, false);
			if(m)
				m->returnPtr = gptr;
		}
		for(int k=0;k<groups[i]->vars.size();k++){
			if(groups[i]->vars[k].isary){
				sprintf(buf, "%s[]", groups[i]->vars[k].name.t);
				CCodeMember* m = gptr->AddMember(Wutf8(buf), L"界面控件[]数组", L"UIControl", L"", CCodeMember::CodeMemberType_Variant, false);
				if(m)
					m->returnPtr = cui;
			}
			else{
				CCodeMember* m = gptr->AddMember(Wutf8(groups[i]->vars[k].name.t), L"界面控件", L"UIControl", L"", CCodeMember::CodeMemberType_Variant, false);
				if(m)
					m->returnPtr = cui;
			}
		}
		{
			CCodeMember* m = gptr->AddMember(L"self", L"本对组合类", L"UIGroup", L"", CCodeMember::CodeMemberType_Variant, false);
			if(m)
				m->returnPtr = this->FindClass(m->returnClass);
		}
	}
	CCodeMember* m = ptr->AddMember(L"self", L"本对话框类", L"UIDialog", L"", CCodeMember::CodeMemberType_Variant, false);
	if(m)
		m->returnPtr = this->FindClass(m->returnClass);
	for(int i=0;i<groups.size();i++)
		delete groups[i];
}
VOID	CCodeXmlManager::LoadContent(Phantom::CDynamicArray<char>& codeXml)
{
	int slen = codeXml.size();
	int len = (int)slen * 3 + 1;
	wchar_t* buftext = new wchar_t[len];
	len = ::MultiByteToWideChar(CP_UTF8, 0, codeXml.GetPtr(), slen, buftext, len);
	buftext[len] = 0;
	CXmlFileW xmlFile;
	if(!xmlFile.loadFromMemory(buftext))//(const wchar_t*)codeXml.GetPtr()))
	{
		delete buftext;
		return;
	}
	delete buftext;
	CXmlElementW xml	=	xmlFile.first(L"xml");
	if(!xml.isXmlOK())
		return;
	m_globalClass = 0;
	CXmlElementW xmlClass	=	xml.first(L"class");
	while(xmlClass.isXmlOK())
	{
		const wchar_t* className	=	xmlClass.read(L"name");
		if(className[0])
		{
			CCodeClass* classPtr		=	new CCodeClass();
			classPtr->m_bIsLoad = true;
			if(wcsicmp(className, L"Global") == 0)
				m_globalClass = classPtr;
			else
				this->m_baseClass.push_back(classPtr);
			//
			classPtr->name				=	className;
			classPtr->description		=	xmlClass.read(L"description");
			RemoveTab((wchar_t*)classPtr->description.str());
			//
			CXmlElementW xmlVar		=	xmlClass.first(L"var");
			while(xmlVar.isXmlOK())
			{
				const wchar_t* varName	=	xmlVar.read(L"name");
				if(varName[0])
					classPtr->AddMember(varName, xmlVar.read(L"description"), xmlVar.read(L"return"), L"", CCodeMember::CodeMemberType_Variant);
				//
				xmlVar.next();
			}
			CXmlElementW xmlMethod	=	xmlClass.first(L"method");
			while(xmlMethod.isXmlOK())
			{
				const wchar_t* varName	=	xmlMethod.read(L"name");
				if(varName[0])
					classPtr->AddMember(varName, xmlMethod.read(L"description"), xmlMethod.read(L"return"), xmlMethod.read(L"args"), CCodeMember::CodeMemberType_Void);
				//
				xmlMethod.next();
			}
			classPtr->SortMembers();
		}
		//
		xmlClass.next();
	}
	//
	CXmlElementW xmlEnum	=	xml.first(L"enum");
	while(xmlEnum.isXmlOK())
	{
		const wchar_t* enumName	=	xmlEnum.read(L"name");
		if(enumName[0])
		{
			CCodeClass* enumPtr		=	new CCodeClass();
			enumPtr->m_bIsEnum = true;
			m_baseClass.push_back(enumPtr);
			//
			enumPtr->name				=	enumName;
			enumPtr->description		=	xmlEnum.read(L"description");
			RemoveTab((wchar_t*)enumPtr->description.str());
			enumPtr->type				=	CCodeMember::CodeMemberType_Enum;
			//
			CXmlElementW xmlVar		=	xmlEnum.first(L"item");
			while(xmlVar.isXmlOK())
			{
				const wchar_t* varName	=	xmlVar.read(L"name");
				//wcslwr((wchar_t*)varName);
				if(varName[0])
					enumPtr->AddMember(varName, xmlVar.read(L"description"), xmlVar.read(L"return"), L"", CCodeMember::CodeMemberType_Enum_Value);
				//
				xmlVar.next();
			}
		}
		//
		xmlEnum.next();
	}
	//
	CCodeClass* enumPtr		=	new CCodeClass();
	m_baseClass.push_back(enumPtr);
	enumPtr->name				=	L"bool";
	enumPtr->type				=	CCodeMember::CodeMemberType_Enum;
	enumPtr->m_bIsEnum			=	true;
	enumPtr->AddMember(L"false", L"假", L"0", L"", CCodeMember::CodeMemberType_Enum_Value);
	enumPtr->AddMember(L"true", L"真", L"1", L"", CCodeMember::CodeMemberType_Enum_Value);
	//
	CXmlElementW xmlUsage	=	xml.first(L"usage");
	CXmlElementW xmlContent	=	xmlUsage.first(L"variant");
	while(xmlContent.isXmlOK())
	{
		const wchar_t* szContent	=	xmlContent.read(L"content");
		if(szContent[0])
		{
			CCodeClass* c	=	0;//this->m_baseContent.AddContent(szContent, );
			const wchar_t* szType	=	xmlContent.read(L"type");
			for(int i=0;i<m_baseClass.size();i++)
			{
				if(m_baseClass[i]->name == szType)
				{
					c	=	m_baseClass[i];
					break;
				}
			}
			if(c)
			{
				const wchar_t* obj	=	xmlContent.read(L"object");
				CCodeMember* m	=	new CCodeMember();
				m->name			=	szContent;
				m->args			=	xmlContent.read(L"args");
				std::wstring wtype;
				if(wcsicmp(obj, L"class") == 0)
					m->type		=	m->CodeMemberType_Class;
				else if(wcsicmp(obj, L"module") == 0)
				{
					m->type		=	m->CodeMemberType_Module;
				}
				else if(xmlContent.read(L"type", wtype))
					m->type		=	m->CodeMemberType_Struct;
				m->returnClass	=	szType;
				m->classPtr		=	c;
				m_dataTypes.push_back(m);
			}
		}
		xmlContent.next();
	}
	if(m_dataTypes.size() > 1)
		qsort(&m_dataTypes.at(0), m_dataTypes.size(), sizeof(CCodeMember*), SortMemberProc);
	//lua 原生函数
	{
		CCodeClass* classPtr		=	new CCodeClass();
		classPtr->m_bIsLoad = true;
		this->m_baseClass.push_back(classPtr);
		//
		classPtr->name				=	L"math";
		classPtr->AddMember(L"pi", L"圆周率(math.pi=3.1415926535898)", L"float", L"math.pi", CCodeMember::CodeMemberType_Variant, true);
		classPtr->AddMember(L"abs", L"取绝对值(math.abs(-2012)=2012)", L"float", L"math.abs(-2012)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"ceil", L"向上取整(math.ceil(9.1)=10)", L"float", L"math.ceil(9.1)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"floor", L"向下取整(math.floor(9.9)=9)", L"float", L"math.floor(9.9)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"max", L"取参数最大值(math.max(2,4,6,8)=8)", L"float", L"math.max(2,4,6,8)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"min", L"取参数最小值(math.min(2,4,6,8)=2)", L"float", L"math.min(2,4,6,8)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"pow", L"计算x的y次幂(math.pow(2,16)=65536)", L"float", L"math.pow(2,16)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"sqrt", L"开平方(math.sqrt(65536)=256)", L"float", L"math.sqrt(65536)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"mod", L"取模(math.mod(65535,2)=1)", L"float", L"math.mod(65535,2)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"modf", L"取整数和小数部分(math.modf(20.12)=20   0.12)", L"float", L"math.modf(20.12)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"randomseed", L"设随机数种子(math.randomseed(os.time())=)", L"float", L"math.randomseed(os.time())", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"random", L"取随机数(math.random()=0~1)", L"float", L"math.random(5,90)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"rad", L"角度转弧度(math.rad(180)=3.1415926535898)", L"float", L"math.rad(180)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"deg", L"弧度转角度(math.deg(math.pi)=180)", L"float", L"math.deg(math.pi)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"exp", L"e的x次方(math.exp(4)=54.598150033144)", L"float", L"math.exp(4)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"log", L"计算x的自然对数(math.log(54.598150033144)=4)", L"float", L"math.log(54.598150033144)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"log10", L"计算10为底，x的对数(math.log10(1000)=3)", L"float", L"math.log10(1000)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"frexp", L"将参数拆成x * (2 ^ y)的形式(math.frexp(160)=0.625    8)", L"float", L"math.frexp(160)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"ldexp", L"计算x * (2 ^ y)(math.ldexp(0.625,8)=160)", L"float", L"math.ldexp(0.625,8)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"sin", L"正弦(math.sin(math.rad(30))=0.5)", L"float", L"math.sin(math.rad(30))", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"cos", L"余弦(math.cos(math.rad(60))=0.5)", L"float", L"math.cos(math.rad(60))", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"tan", L"正切(math.tan(math.rad(45))=1)", L"float", L"math.tan(math.rad(45))", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"asin", L"反正弦(math.deg(math.asin(0.5))=30)", L"float", L"math.deg(math.asin(0.5))", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"acos", L"反余弦(math.deg(math.acos(0.5))=60)", L"float", L"math.deg(math.acos(0.5))", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"atan", L"反正切(math.deg(math.atan(1))=45)", L"float", L"math.deg(math.atan(1))", CCodeMember::CodeMemberType_Void, true);
		classPtr->SortMembers();
	}
	{
		CCodeClass* classPtr		=	new CCodeClass();
		classPtr->m_bIsLoad = true;
		this->m_baseClass.push_back(classPtr);
		//
		classPtr->name				=	L"string";
		classPtr->AddMember(L"len", L"计算字符串长度(string.len(\"abcd\")=4)", L"float", L"string.len(\"abcd\")", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"rep", L"返回字符串s的n个拷贝(string.rep(\"abcd\",2)=abcdabcd)", L"float", L"string.rep(\"abcd\",2)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"lower", L"返回字符串全部字母大写(string.lower(\"AbcD\")=abcd)", L"float", L"string.lower(\"AbcD\")", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"upper", L"返回字符串全部字母小写(string.upper(\"AbcD\")=ABCD)", L"float", L"string.upper(\"AbcD\")", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"format", L"返回一个类似printf的格式化字符串(string.format(\"the value is:%d\",4)=the value is:4)", L"float", L"string.format(\"the value is:%d\",4)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"sub", L"returns substring from index i to j of s(string.sub(\"abcd\",2)=bcd)\r\n\r\n(string.sub(\"abcd\",-2)=cd)\r\n(string.sub(\"abcd\",2,-2)=bc)\r\n(string.sub(\"abcd\",2,3)=bc)", L"float", L"string.sub(\"abcd\",2)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"find", L"在字符串中查找(string.find(\"cdcdcdcd\",\"ab\")=nil)\r\n(string.find(\"cdcdcdcd\",\"cd\")=1 2)\r\n(string.find(\"cdcdcdcd\",\"cd\",7)=7 8)", L"float", L"string.find(\"cdcdcdcd\",\"ab\")", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"gsub", L"在字符串中替换(string.gsub(\"abcdabcd\",\"a\",\"z\");=zbcdzbcd 2)", L"float", L"string.gsub(\"abcdabcd\",\"a\",\"z\");", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"byte", L"返回字符的整数形式(string.byte(\"ABCD\",4)=68)", L"float", L"string.byte(\"ABCD\",4)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"char", L"将整型数字转成字符并连接(string.char(97,98,99,100)=abcd)", L"float", L"string.char(97,98,99,100)", CCodeMember::CodeMemberType_Void, true);
		classPtr->SortMembers();
	}
	{
		CCodeClass* classPtr		=	new CCodeClass();
		classPtr->m_bIsLoad = true;
		this->m_baseClass.push_back(classPtr);
		//
		classPtr->name				=	L"table";
		classPtr->AddMember(L"concat", L"table.concat()函数列出参数中指定table的数组部分从start位置到end位置的所有元素, 元素间以指定的分隔符(sep)隔开。除了table外, 其他的参数都不是必须的, 分隔符的默认值是空字符, start的默认值是1, end的默认值是数组部分的总长.sep, start, end这三个参数是顺序读入的, 所以虽然它们都不是必须参数, 但如果要指定靠后的参数, 必须同时指定前面的参数.", L"string", L"string table.concat(table, sep,  start, end)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"insert", L"table.insert()函数在table的数组部分指定位置(pos)插入值为value的一个元素. pos参数可选, 默认为数组部分末尾.", L"", L"table.insert(table, value)\ntable.insert(table, pos, value)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"maxn", L"table.maxn()函数返回指定table中所有正数key值中最大的key值. 如果不存在key值为正数的元素, 则返回0. 此函数不限于table的数组部分.", L"", L"int table.maxn(table_var)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"remove", L"table.remove()函数删除并返回table数组部分位于pos位置的元素. 其后的元素会被前移. pos参数可选, 默认为table长度, 即从最后一个元素删起.\n> sortFunc = function(a, b) return b < a end\n> table.sort(tbl, sortFunc)", L"", L"table.remove(table, pos)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"foreachi", L"会期望一个从 1（数字 1）开始的连续整数范围，遍历table中的key和value逐对进行function(i, v)操作", L"", L"table.foreachi(table, function(i, v))", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"foreach", L"t1 = {2, 4, 6, language=\"Lua\", version=\"5\", 8, 10, 12, web=\"hello lua\"};\ntable.foreach(t1, function(i, v) print (i, v) end) ;\n输出结果：\n1 2\n2 4\n3 6\n4 8\n5 10\n6 12\nweb hello lua\nlanguage Lua\nversion 5", L"", L"table.foreach(table, function(i, v))", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"getn", L"返回table中元素的个数", L"int", L"table.getn(table)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"setn", L"设置table中的元素个数", L"", L"table.setn(table, nSize)", CCodeMember::CodeMemberType_Void, true);
		classPtr->AddMember(L"sort", L"table.sort(table, comp)函数对给定的table进行升序排序\nfunction comp(a,b)\n return a.int>b.int\nend", L"", L"table.sort(table, comp)", CCodeMember::CodeMemberType_Void, true);
		classPtr->SortMembers();
	}
	m_globalClass->AddMember(L"setmetatable", L"设置setmetatable", L"", L"setmetatable(list, class)", CCodeMember::CodeMemberType_Void, true);
	m_saveClass = m_baseClass;
	m_savemethods = m_globalClass->members;
	for(int i=0;i<m_dataTypes.size();i++)
	{
		if(m_dataTypes[i]->type == CCodeMember::CodeMemberType_Struct)
			m_savemethods.push_back(m_dataTypes[i]);//类型创建
	}
	if(m_globalClass){
		m_savevariants.push_back(m_globalClass->AddMember(L"camera", L"3D镜头操作类", L"Camera", L"", CCodeMember::CodeMemberType_Variant));
		m_savevariants.push_back(m_globalClass->AddMember(L"system", L"系统属性调用方法", L"System", L"", CCodeMember::CodeMemberType_Variant));
		m_savevariants.push_back(m_globalClass->AddMember(L"gamemgr", L"游戏管理器", L"GameMgr", L"", CCodeMember::CodeMemberType_Variant));
		//
		//m_savevariants.push_back(m_globalClass->AddMember(L"if", L"if 条件 then ... elseif 条件 then ... else ... end \nif 1+1=2 then\nprint(\"true\")\nelseif 1+2~=3 then\nprint(\"true\")\nelse\nprint(\"false\")\nend", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"while", L"While 	while 条件 do ... end\nwhile 1+1~=2 do print(\"true\") end", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"repeat", L"Repeat 	repeat ... until 条件 \nrepeat print(\"Hello\") until 1+1~=2", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"for", L"for 变量=初值, 终点值, 步进 do ... end \nfor i = 1, 10, 2 do print(i) end", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"and", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"break", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"do", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"else", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"elseif", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"end", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"false", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"function", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"in", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"local", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"nil", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"not", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"or", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"return", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"then", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"true", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"until", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
		//m_savevariants.push_back(m_globalClass->AddMember(L"null", L"关键字", L"", L"", CCodeMember::CodeMemberType_Keywords));
	}
	for(int j=0;j<m_savevariants.size();j++)
	{
		CCodeMember* m	=	m_savevariants[j];
		if(m)
		{
			if(m->returnClass.t[0])
				m->returnPtr	=	this->FindClass(m->returnClass);
		}
	}
	for(int j=0;j<m_savemethods.size();j++)
	{
		CCodeMember* m	=	m_savemethods[j];
		if(m)
		{
			if(m->returnClass.t[0])
				m->returnPtr	=	this->FindClass(m->returnClass);
		}
	}
	for(int i=0;i<this->m_baseClass.size();i++)
	{
		for(int j=0;j<m_baseClass[i]->members.size();j++)
		{
			CCodeMember* m	=	m_baseClass[i]->members[j];
			if(m)
			{
				if(m->returnClass.t[0])
					m->returnPtr	=	this->FindClass(m->returnClass);
			}
		}
	}
	ResetGameContent();
}
VOID				CCodeXmlManager::ResetGameContent()
{
	for(int i=0;i<m_gameClass.size();i++)
		delete m_gameClass[i];
	m_gameClass.clear();
	//for(int i=0;i<m_gameMethods.size();i++)
	//	delete m_gameMethods[i];
	//m_gameMethods.clear();
	for(int i=0;i<m_gameVariants.size();i++)
		delete m_gameVariants[i];
	m_gameVariants.clear();
	if(m_globalGame)
		delete m_globalGame;
	m_globalGame = 0;
#ifdef EDITOR_OS
	if(Phantom::GetGameMgr()->m_configLua.size()==0)
		Phantom::GetGameMgr()->ResearchAllFiles();
	wchar_t wbuf[512],wname[256];
	m_globalGame = new CCodeClass();
	Phantom::CDynamicArray<Phantom::Config*> regs = Phantom::GetGameMgr()->m_configLua;
	int olds = regs.size();
	Phantom::Config* tblRole = Phantom::GetGameMgr()->GetRoleTable();
	if(tblRole)
		regs.push_back(tblRole);
	Phantom::Config* leaderTbl = Phantom::GetGameMgr()->GetLeaderTable();
	if(leaderTbl)
		regs.push_back(leaderTbl);
	wchar_t table_gen[128],stable_gen[128],table_id[128],table_name[128],gtable_id[128],gtable_name[128];
	wcscpy(stable_gen, Language(L"stable_gen", L"搜索并获得(%s)游戏运行中对应gameid的（游戏）对象"));
	wcscpy(table_gen, Language(L"table_gen", L"搜索并获得(%s)表格中对应id的（行）数据"));
	wcscpy(table_id, Language(L"table_id", L"搜索并获得ID对应的（行）数据"));
	wcscpy(table_name, Language(L"table_name", L"搜索并获得Name对应的（行）数据"));
	wchar_t countname[128];
	wcscpy(countname, Language(L"countname", L"变量数组的元素数量"));
	for(int i=0;i<regs.size();i++){
		Phantom::Config* cfg = regs[i];
		CCodeClass* c		=	0;
		if(regs[i] != leaderTbl&&regs[i] != tblRole){
			c = new CCodeClass();
			m_gameClass.push_back(c);
			//
			c->name				=	Wutf8(cfg->GetTableName());
			c->description		=	c->name;
			c->type				=	CCodeMember::CodeMemberType_Table;
			swprintf(wbuf, L"bool %s::isnull()", c->name.t);
			c->AddMember(L"isnull", L"判断类变量是否为空", L"bool", wbuf, CCodeMember::CodeMemberType_Void);
		}
		else
		{
			if(regs[i] == leaderTbl)
				c = this->FindClass(L"GameMgr");
			else if(regs[i] == tblRole)
				c = this->FindClass(L"GameObject");
			if(!c)
				continue;
		}
		if(regs[i]==tblRole)
		{
			c->AddMember(L"gameid", stable_gen, L"int", L"", CCodeMember::CodeMemberType_Variant);
		}
		for(int j=0;j<cfg->GetNumCol();j++)
		{
			Phantom::ConfigHeader* col = cfg->GetCol(j);
			char type[256];
			strcpy(type, col->type_s.t);
			if(stricmp(type, "text")==0)
				strcpy(type, "string");
			if(col->listcount>1){
				swprintf(wbuf, L"int %s::%scount()\r\n%s\r\n%s", c->name.t, Wutf8(col->name.t).buf, Wutf8(col->desc.str()).buf, countname);
				swprintf(wname, L"%scount", Wutf8(col->name.t));
				c->AddMember(wname, wbuf, L"int", L"", CCodeMember::CodeMemberType_Void);
				wchar_t warg[256],wtip[256];
				swprintf(warg, L"%s %s::%s(int index)", Wutf8(type).buf, c->name.t, Wutf8(col->name.t).buf);
				swprintf(wtip, L"获得数组数据(index是索引)，数量从%scount中取得", Wutf8(col->name.t).buf);
				swprintf(wbuf, L"%s\r\n%s\r\n%s", warg, wtip, Wutf8(col->desc.str()).buf);
				c->AddMember(Wutf8(col->name.t), wbuf, Wutf8(type), L"", CCodeMember::CodeMemberType_Void);
				swprintf(warg, L"void %s::set%s(int index, %s)", c->name.t, Wutf8(col->name.t).buf, Wutf8(type).buf);
				swprintf(wtip, L"设置数组数据(index是索引)，数量从%scount中取得", Wutf8(col->name.t).buf);
				swprintf(wbuf, L"%s\r\n%s\r\n%s", warg, wtip, Wutf8(col->desc.str()).buf);
				swprintf(wname, L"set%s", Wutf8(col->name.t));
				c->AddMember(wname, wbuf, Wutf8(type), L"", CCodeMember::CodeMemberType_Void);
			}
			else
			{
				swprintf(wbuf, L"%s %s::%s\r\n%s", Wutf8(type).buf, c->name.t, Wutf8(col->name.t).buf, Wutf8(col->desc.str()).buf);
				if(regs[i] != leaderTbl){
					if(j==0)
						wcscat(wbuf, table_id);
					else if(j==1)
						wcscat(wbuf, table_name);
				}
				c->AddMember(Wutf8(col->name.t), wbuf, Wutf8(type), L"", CCodeMember::CodeMemberType_Variant);
			}
		}
		if(leaderTbl!=cfg){
			wchar_t wname[512];
			if(regs[i]==tblRole)
			{
				swprintf(wbuf, L"%s %s::%s(int gameid)", c->name.t, c->name.t, c->name.t);
				swprintf(wname, stable_gen, c->name.t);
			}
			else
			{
				swprintf(wbuf, L"%s %s::%s(int id)", c->name.t, c->name.t, c->name.t);
				swprintf(wname, table_gen, c->name.t);
			}
			m_gameMethods.push_back(m_globalGame->AddMember(Wutf8(cfg->GetTableName()), wname, c->name.t, wbuf, CCodeMember::CodeMemberType_Table));
		}
	}
	//
	ResetBaseClass();
	for(int i=0;i<this->m_gameClass.size();i++)
	{
		for(int j=0;j<m_gameClass[i]->members.size();j++)
		{
			CCodeMember* m	=	m_gameClass[i]->members[j];
			if(m)
			{
				if(m->returnClass.t[0])
				{
					m->returnPtr	=	this->FindClass(m->returnClass);
				}
			}
		}
	}
	for(int j=0;j<m_globalGame->members.size();j++)
	{
		CCodeMember* m	=	m_globalGame->members[j];
		if(m)
		{
			if(m->returnClass.t[0])
				m->returnPtr	=	this->FindClass(m->returnClass);
		}
	}
#endif
}
VOID				CCodeXmlManager::SortMembers(){
	if(m_methods.size()>1)
		qsort(&m_methods.at(0), m_methods.size(), sizeof(CCodeMember*), SortMemberProc);
	if(m_variants.size()>1)
		qsort(&m_variants.at(0), m_variants.size(), sizeof(CCodeMember*), SortMemberProc);
}
VOID				CCodeXmlManager::ResetBaseClass()
{
	m_baseClass.clear();
	m_baseClass = m_saveClass;
	m_methods = m_savemethods;
	m_variants = m_savevariants;
	for(int i=0;i<m_gameVariants.size();i++)
		delete m_gameVariants[i];
	m_gameVariants.clear();
	for(int i=0;i<m_gameClass.size();i++){
		m_baseClass.push_back(m_gameClass[i]);
	}
	for(int i=0;i<m_uiclass.size();i++){
		m_baseClass.push_back(m_uiclass[i]);
		for(int k=0;k<m_uiclass[i]->groups.size();k++){
			m_baseClass.push_back(m_uiclass[i]->groups[k]);
		}
	}
	for(int i=0;i<m_gameMethods.size();i++){
		m_methods.push_back(m_gameMethods[i]);
	}
	for(int i=0;i<m_gameVariants.size();i++){
		m_variants.push_back(m_gameVariants[i]);
	}
}
VOID				CCodeXmlManager::AddMyMethod(CCodeMember* m)
{
	m_methods.push_back(m);
}
CCodeMember*		CCodeXmlManager::FindMethod(const wchar_t* m)
{
	for(int i=0;i<m_methods.size();i++)
	{
		if(m_methods[i]->name == m)
			return m_methods[i];
	}
	return 0;
}
VOID				CCodeXmlManager::ResetVariants()
{
	m_variants = m_savevariants;
}
CCodeMember*		CCodeXmlManager::AddMyVariant(const wchar_t* name)
{
	CCodeMember* m = new CCodeMember();
	m->name = name;
	m_gameVariants.push_back(m);
	m_variants.push_back(m);
	return m;
}
CCodeMember*		CCodeXmlManager::FindVariant(const wchar_t* m)
{
	for(int i=0;i<m_variants.size();i++)
	{
		if(m_variants[i]->name == m)
			return m_variants[i];
	}
	return 0;
}
BOOL				CCodeXmlManager::AddMyClass(CCodeClass* cc)
{
	if(cc->members.size()<=0){
		return false;
	}
	for(int i=0;i<this->m_baseClass.size();i++)
	{
		if(m_baseClass[i]->m_bIsLoad)
			continue;
		if(m_baseClass[i]->IsEquip(*cc)){
			return false;
		}
	}
	m_baseClass.push_back(cc);
	cc->SearchReturnMembers();
	return true;
}
VOID				CCodeClass::SearchReturnMembers()
{
	for(int j=0;j<members.size();j++)
	{
		CCodeMember* m	=	members[j];
		if(m)
		{
			if(m->returnClass.t[0])
				m->returnPtr	=	GetCodeXmlManager()->FindClass(m->returnClass);
		}
	}
}
BOOL				CCodeClass::IsEquip(CCodeClass& c)
{
	if(this->members.size()!=c.members.size())
		return false;
	if(wcsicmp(this->name.t, c.name.t)!=0)
		return false;
	for(int i=0;i<members.size();i++)
	{
		if(wcsicmp(members[i]->name.t, c.name.t)!=0)
			return false;
	}
	return true;
}
CCodeClass*			CCodeXmlManager::FindClass(const wchar_t* szClass)
{
	for(int i=0;i<this->m_baseClass.size();i++)
	{
		if(wcscmp(m_baseClass[i]->name , szClass) == 0)
			return m_baseClass[i];
	}
	return 0;
}
