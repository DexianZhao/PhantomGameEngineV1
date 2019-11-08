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
#ifndef ___BASE_XMLFILEW__H___
#define ___BASE_XMLFILEW__H___



#pragma once
#include <string>

class CXmlAttributeW//Xml属性操作类,此项不需要用户主动调用
{
	void*				m_pAttr;
	CXmlAttributeW(void* ptr);
	friend class CXmlElementW;
public:
	bool				isXmlOK();
	//
	bool				next();
};

class CXmlElementW;

__interface	IXmlLoaderW//给用户提供的读取Xml文件的接口
{
	void				onElementLoader(CXmlElementW& item, const wchar_t* szElement, int loadType, void* lpParameter);
};

class CXmlElementW//Xml元素管理类，比如<xml></xml>，可以读取其内部的值，也可以写入到内部
{
	void*				m_pElement;
	wchar_t				m_szItemName[60];
	//
	CXmlElementW(void* ptr, const wchar_t* szItemName, bool bAll = false);
	bool				m_bAllChildMode;
	//
	friend class		CXmlFileW;
public:
	//
	bool				isXmlOK();//此项Xml元素是否有效，如果没有效则表示读完所有的同名称的子元素
	//
	CXmlElementW		insert(const wchar_t* szItem);//插入一个子元素
	//
	void				write(const wchar_t* szAttr, const wchar_t* szValue);//写入段文本到一个属性，szAttr属性名，szValue值
	void				write_int(const wchar_t* szAttr,int n);//写入一个整数类型到属性
	void				write_float(const wchar_t* szAttr,float n);//写入一个浮点数到属性
	//
	bool				read(const wchar_t* szAttr,std::wstring& _out);//读取一段字符串
	bool				read(const wchar_t* szAttr,int& _out);//读取一段整数
	bool				read(const wchar_t* szAttr,float& _out);//读取一段浮点数
	const wchar_t*		read(const wchar_t* szAttr, const wchar_t* szDefault = L"");//读取一段字符串
	//
	void				loadElement(IXmlLoaderW* ptr, const wchar_t* szElementName, int loadType, void* lpParameter);//根据接口读取同名称的所有元素，ptr用户继承的接口指针，szElementName元素名（比如<xml><element></element><element></element>...</xml>，loadType,用户自定义类型，在onElementLoader中进行判断，lpParameter用户自定义的参数
	CXmlElementW		first(const wchar_t* szItem);	//返回第一个子元素的位置
	CXmlElementW		first();	//返回第一个子元素的位置
	//
	CXmlAttributeW			firstAttr();
	//
	bool				next();//移动到下一个子元素
};

class CXmlFileW//Unicode格式的Xml操作类
{
	void*				m_pdocFile;
public:
	CXmlFileW(void);
	CXmlFileW(const wchar_t* szFile);//从文件载入
	~CXmlFileW(void);
	//
	bool				loadFromFile(const char* szFile);//从内存打开xml文件
	bool				loadFromMemory(const wchar_t* szText);//从文件打开xml文件
	//
	bool				isXmlOK();//是否能使用此xml
	//
	CXmlElementW		first(const wchar_t* szItem);	//查找第一个元素的位置
	//
	CXmlElementW		insert(const wchar_t* szItem);//插入第一个元素
	bool				saveXml(const wchar_t* szFile);//保存一个xml元素
	//
};

#endif
