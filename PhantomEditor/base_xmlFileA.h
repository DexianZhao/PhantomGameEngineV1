//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   base.h
	软件名称   :   幻影游戏引擎（www.aixspace.com）

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	

	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___BASE_XMLFILEA__H___
#define ___BASE_XMLFILEA__H___



#pragma once
#include <string>
#include <vector>

#define	XML_LOAD_TYPE_HEAD	0

struct CXmlValueA//ASCII 格式Xml文件，Xml元素的内容数据
{
	std::string			name, value;
};

class CXmlElementA;

__interface	IXmlLoaderA//给用户提供的读取Xml文件的回调接口
{
	void				onElementLoader(CXmlElementA& item, const char* szElement, int loadType, void* lpParameter);
};

class CXmlAttributeA//Xml属性操作类,此项不需要用户主动调用
{
	void*				m_pAttr;
	CXmlAttributeA(void* ptr);
	friend class CXmlElementA;
public:
	bool				isXmlOK();
	//
	bool				next();
};

class CXmlElementA//Xml元素管理类，比如<xml></xml>，可以读取其内部的值，也可以写入到内部
{
public:
	//
	bool				isXmlOK();//此项Xml元素是否有效，如果没有效则表示读完所有的同名称的子元素
	//
	CXmlElementA		insert(const char* szItem);//插入一个子元素
	//
	void				write(const char* szAttr, const char* szValue);//写入段文本到一个属性，szAttr属性名，szValue值
	void				write_int(const char* szAttr,int n);//写入一个整数类型到属性
	void				write_float(const char* szAttr,float n);//写入一个浮点数到属性
	//
	bool				read(const char* szAttr,std::string& _out);//读取一段字符串
	bool				read(const char* szAttr,int& _out);//读取一段整数
	bool				read(const char* szAttr,float& _out);//读取一段浮点数
	bool				read(const char* szAttr,double& _out);//
	bool				read(const char* szAttr,bool& _out);//
	const char*			read(const char* szAttr, const char* szDefault);//读取一段字符串
	//
	void				enumAttr(std::vector<CXmlValueA>& rets);//枚举所有的属性值
	void				freeEnum(std::vector<CXmlValueA>& rets);
	//
	void				loadElement(IXmlLoaderA* ptr, const char* szElementName, int loadType, void* lpParameter);//根据接口读取同名称的所有元素，ptr用户继承的接口指针，szElementName元素名（比如<xml><element></element><element></element>...</xml>，loadType,用户自定义类型，在onElementLoader中进行判断，lpParameter用户自定义的参数
	//
	CXmlElementA		first(const char* szItem);	//返回第一个子元素的位置
	CXmlElementA		first();	//返回第一个子元素的位置
	//
	CXmlAttributeA		firstAttr();
	//
	bool				next();//移动到下一个子元素
	const char*			getItemName(){return m_szItemName;}
protected:
	CXmlElementA(void* ptr, const char* szItemName, bool bAll = false);
	//
	friend class		CXmlFileA;
	void*				m_pElement;
	char				m_szItemName[60];
	bool				m_bAllChildMode;
	//
};

void	readAttr(CXmlElementA& element, const char* szName, float& f);
void	readAttr(CXmlElementA& element, const char* szName, double& f);
void	readAttr(CXmlElementA& element, const char* szName, int& f);
void	readAttr(CXmlElementA& element, const char* szName, bool& f);
void	readAttr(CXmlElementA& element, const char* szName, std::string& f);

class CXmlFileA//Ansi格式的Xml操作类
{
public:
	CXmlFileA(void);
	CXmlFileA(const char* szFile);//从文件载入
	~CXmlFileA(void);
	//
	bool				loadFromMemory(const char* szText);//从内存打开xml文件
	bool				loadFromFile(const char* szFile);//从文件打开xml文件
	//
	bool				isXmlOK();//是否能使用此xml
	//
	CXmlElementA		first(const char* szItem);	//查找第一个元素的位置
	//
	CXmlElementA		insert(const char* szItem);//插入第一个元素
	bool				saveXml(const char* szFile);//保存一个xml元素
	//
protected:
	void*				m_pdocFile;
};

#endif
