//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   base_xmlFileW.cpp
	软件名称   :   幻影游戏引擎（www.aixspace.com）

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	

	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////



#include "StdAfx.h"
#include <windows.h>
#include "tinyxmlw.h"
#include "base_xmlFileW.h"

#define	getXML	((TiXmlDocumentW*)m_pdocFile)
#define	getElement	((TiXmlElementW*)m_pElement)
#define	getElementParent	((TiXmlElementW*)m_pElementParent)
#define	getAttr	((TiXmlAttributeW*)m_pAttr)

bool CXmlAttributeW::next()
{
	if(!m_pAttr)
		return false;
	m_pAttr = getAttr->Next();
	return (m_pAttr!=NULL);
}

bool CXmlAttributeW::isXmlOK()
{
	return (m_pAttr!=NULL);
}

CXmlAttributeW::CXmlAttributeW(void* ptr)
{
	this->m_pAttr = ptr;
}

CXmlElementW::CXmlElementW(void* ptr, const wchar_t* szItemName, bool b)
{
	m_bAllChildMode	=	b;
	m_pElement = ptr;
	if(szItemName)
		wcscpy(m_szItemName, szItemName);
	else
		m_szItemName[0] = 0;
}

void	CXmlElementW::loadElement(IXmlLoaderW* ptr, const wchar_t* szElementName, int loadType, void* lpParameter)
{
	CXmlElementW item = this->first(szElementName);
	while(item.isXmlOK())
	{
		ptr->onElementLoader( item, szElementName, loadType, lpParameter );
		if(!item.next())
			break;
	}
}

bool				CXmlElementW::isXmlOK()
{
	return (this->m_pElement!=NULL);
}
//
CXmlAttributeW			CXmlElementW::firstAttr()
{
	if(this->m_pElement)
		return CXmlAttributeW(getElement->FirstAttribute());
	return CXmlAttributeW(NULL);
}
//
CXmlElementW			CXmlElementW::first()	//返回第一个子元素的位置
{
	TiXmlElementW* pEl	=	getElement->FirstChildElement();
	return CXmlElementW(pEl, pEl ? pEl->GetValue() : L"", true);
}

CXmlElementW			CXmlElementW::first(const wchar_t* szItem)
{
	if(getElement)
		return CXmlElementW(getElement->FirstChildElement(szItem), szItem);
	return CXmlElementW(0,0);
}
//
bool				CXmlElementW::next()
{
	if(m_pElement)
	{
		if(this->m_bAllChildMode)
		{
			this->m_pElement = getElement->NextSiblingElement();
			if(m_pElement!=NULL)
				wcscpy(m_szItemName, getElement->GetValue());
		}
		else
			this->m_pElement = getElement->NextSiblingElement(this->m_szItemName);
		return (m_pElement!=NULL);
	}
	return false;
}

CXmlElementW			CXmlElementW::insert(const wchar_t* szItem)
{
	if(getElement)
		return CXmlElementW(getElement->InsertEndChild(TiXmlElementW(szItem))->ToElement(), szItem);
	return CXmlElementW(0,0);
}

//
void				CXmlElementW::write(const wchar_t* szAttr, const wchar_t* szValue)
{
	if(getElement)
		getElement->SetAttribute(szAttr,szValue);
}
void				CXmlElementW::write_int(const wchar_t* szAttr,int n)
{
	wchar_t szBuf[200];
	_itow(n,szBuf,10);
	if(getElement)
		getElement->SetAttribute(szAttr,szBuf);
}
void				CXmlElementW::write_float(const wchar_t* szAttr,float n)
{
	wchar_t szBuf[200];
	swprintf(szBuf, L"%.05f",n);
	if(getElement)
		getElement->SetAttribute(szAttr,szBuf);
}

const wchar_t*		CXmlElementW::read(const wchar_t* szAttr, const wchar_t* szDefault)
{
	if(!getElement)
	{
		if(szDefault)
			return szDefault;
		return L"";
	}
	const wchar_t* ret	=	getElement->Attribute(szAttr);
	if(ret)
		return ret;
	return L"";
}

bool				CXmlElementW::read(const wchar_t* szAttr,std::wstring& _out)
{
	if(!getElement)
		return false;
	const wchar_t* szBuffer = getElement->Attribute(szAttr);
	if(!szBuffer)
		return false;
	_out = szBuffer;
	return true;
}
bool				CXmlElementW::read(const wchar_t* szAttr,int& _out)
{
	if(!getElement)
		return false;
	const wchar_t* szBuffer = getElement->Attribute(szAttr);
	if(!szBuffer)
		return false;
	_out = _wtoi(szBuffer);
	return true;
}
bool				CXmlElementW::read(const wchar_t* szAttr,float& _out)
{
	if(!getElement)
		return false;
	const wchar_t* szBuffer = getElement->Attribute(szAttr);
	if(!szBuffer)
		return false;
	_out = (float)_wtof(szBuffer);
	return true;
}

//


CXmlFileW::CXmlFileW(const wchar_t* szFile)//从文件载入
{
	this->m_pdocFile = new TiXmlDocumentW();
	if(!getXML->LoadFile(szFile))
	{
		delete getXML;
		m_pdocFile = 0;
	}
}

bool		CXmlFileW::loadFromFile(const char* szFile)
{
	wchar_t wszFile[512];
	wszFile[::MultiByteToWideChar(CP_ACP, 0, szFile, -1, wszFile, 512)] = 0;
	if(!getXML->LoadFile(wszFile))
	{
		delete getXML;
		m_pdocFile = 0;
		return false;
	}
	return true;
}

bool		CXmlFileW::isXmlOK()//是否能使用此xml
{
	return (m_pdocFile!=NULL);
}

CXmlFileW::CXmlFileW(void)
{
	this->m_pdocFile = new TiXmlDocumentW();
}

CXmlElementW		CXmlFileW::insert(const wchar_t* szItem)
{
	return CXmlElementW(getXML->InsertEndChild(TiXmlElementW(szItem)), szItem);
}

CXmlElementW		CXmlFileW::first(const wchar_t* szItem)
{
	return CXmlElementW(getXML->FirstChildElement(szItem),szItem);
}

bool			CXmlFileW::loadFromMemory(const wchar_t* szText)
{
	return getXML->LoadFromMemory(szText);
}

CXmlFileW::~CXmlFileW(void)
{
	if(m_pdocFile)
		delete (TiXmlDocumentW*)m_pdocFile;
}

bool			CXmlFileW::saveXml(const wchar_t* szFile)
{
	if(!getXML)return false;
	//
	getXML->SaveFile(szFile, 0);
	//
	return true;

}

