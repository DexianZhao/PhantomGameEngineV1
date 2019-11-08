//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   base_xmlFileA.cpp
	软件名称   :   幻影游戏引擎（www.aixspace.com）

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	

	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tinyxml.h"
#include "base_xmlFileA.h"

#define	getXMLA	((TiXmlDocument*)m_pdocFile)
#define	getElementA	((TiXmlElement*)m_pElement)
#define	getElementParentA	((TiXmlElement*)m_pElementParent)
#define	getAttrA	((TiXmlAttribute*)m_pAttr)

bool CXmlAttributeA::next()
{
	if(!m_pAttr)
		return false;
	m_pAttr = getAttrA->Next();
	return (m_pAttr!=NULL);
}

bool CXmlAttributeA::isXmlOK()
{
	return (m_pAttr!=NULL);
}

CXmlAttributeA::CXmlAttributeA(void* ptr)
{
	this->m_pAttr = ptr;
}

void	CXmlElementA::loadElement(IXmlLoaderA* ptr, const char* szElementName, int loadType, void* lpParameter)
{
	CXmlElementA item = this->first(szElementName);
	while(item.isXmlOK())
	{
		ptr->onElementLoader( item, szElementName, loadType, lpParameter );
		if(!item.next())
			break;
	}
}

CXmlElementA::CXmlElementA(void* ptr, const char* szItemName, bool b)
{
	m_bAllChildMode	=	b;
	m_pElement = ptr;
	if(szItemName)
		strcpy(m_szItemName, szItemName);
	else
		m_szItemName[0] = 0;
}

bool				CXmlElementA::isXmlOK()
{
	return (this->m_pElement!=NULL);
}
//
CXmlAttributeA			CXmlElementA::firstAttr()
{
	if(this->m_pElement)
		return CXmlAttributeA(getElementA->FirstAttribute());
	return CXmlAttributeA(NULL);
}
//

CXmlElementA		CXmlElementA::first()
{
	TiXmlElement* pEl	=	getElementA->FirstChildElement();
	return CXmlElementA(pEl, pEl ? pEl->GetValue().c_str() : "", true);
}

CXmlElementA			CXmlElementA::first(const char* szItem)
{
	if(getElementA)
		return CXmlElementA(getElementA->FirstChildElement(szItem), szItem);
	return CXmlElementA(0,0);
}
//
bool				CXmlElementA::next()
{
	if(m_pElement)
	{
		if(m_bAllChildMode)
		{
			this->m_pElement = getElementA->NextSiblingElement();
			if(m_pElement!=NULL)
				strcpy(m_szItemName, getElementA->GetValue().c_str());
		}
		else
			this->m_pElement = getElementA->NextSiblingElement(this->m_szItemName);
		return (m_pElement!=NULL);
	}
	return false;
}

CXmlElementA			CXmlElementA::insert(const char* szItem)
{
	if(getElementA)
		return CXmlElementA(getElementA->InsertEndChild(TiXmlElement(szItem))->ToElement(), szItem);
	return CXmlElementA(0,0);
}

//
void				CXmlElementA::write(const char* szAttr, const char* szValue)
{
	if(getElementA)
		getElementA->SetAttribute(szAttr,szValue);
}
void				CXmlElementA::write_int(const char* szAttr,int n)
{
	char szBuf[200];
	_itoa(n,szBuf,10);
	if(getElementA)
		getElementA->SetAttribute(szAttr,szBuf);
}
void				CXmlElementA::write_float(const char* szAttr,float n)
{
	char szBuf[200];
	sprintf(szBuf, "%.05f",n);
	if(getElementA)
		getElementA->SetAttribute(szAttr,szBuf);
}

const char*		CXmlElementA::read(const char* szAttr, const char* szDefault)
{
	if(!getElementA)
	{
		if(szDefault)
			return szDefault;
		return "";
	}
	const char* ret	=	getElementA->Attribute(szAttr);
	if(ret)
		return ret;
	return "";
}

void				CXmlElementA::enumAttr(std::vector<CXmlValueA>& rets)
{
	if(!getElementA)
		return;
	getElementA->enumAttrbutes(rets);
}

void				CXmlElementA::freeEnum(std::vector<CXmlValueA>& rets)
{
	for(int i=0;i<rets.size();i++)
	{
		rets[i].name.clear();
		rets[i].value.clear();
	}
	rets.clear();
}

bool				CXmlElementA::read(const char* szAttr,std::string& _out)
{
	if(!getElementA)
		return false;
	const char* szBuffer = getElementA->Attribute(szAttr);
	if(!szBuffer)
		return false;
	_out = szBuffer;
	return true;
}
bool				CXmlElementA::read(const char* szAttr,int& _out)
{
	if(!getElementA)
		return false;
	const char* szBuffer = getElementA->Attribute(szAttr);
	if(!szBuffer)
		return false;
	_out = atoi(szBuffer);
	return true;
}
bool				CXmlElementA::read(const char* szAttr,float& _out)
{
	if(!getElementA)
		return false;
	const char* szBuffer = getElementA->Attribute(szAttr);
	if(!szBuffer)
		return false;
	_out = (float)atof(szBuffer);
	return true;
}

bool				CXmlElementA::read(const char* szAttr,double& _out)
{
	if(!getElementA)
		return false;
	const char* szBuffer = getElementA->Attribute(szAttr);
	if(!szBuffer)
		return false;
	_out = atof(szBuffer);
	return true;
}

bool				CXmlElementA::read(const char* szAttr,bool& _out)
{
	if(!getElementA)
		return false;
	const char* szBuffer = getElementA->Attribute(szAttr);
	if(!szBuffer)
		return false;
	_out = atoi(szBuffer);
	return true;
}

//


CXmlFileA::CXmlFileA(const char* szFile)//从文件载入
{
	this->m_pdocFile = new TiXmlDocument();
	if(!getXMLA->LoadFile(szFile))
	{
		delete getXMLA;
		m_pdocFile = 0;
	}
}

bool		CXmlFileA::isXmlOK()//是否能使用此xml
{
	return (m_pdocFile!=NULL);
}

CXmlFileA::CXmlFileA(void)
{
	this->m_pdocFile = new TiXmlDocument();
}

CXmlElementA		CXmlFileA::insert(const char* szItem)
{
	return CXmlElementA(getXMLA->InsertEndChild(TiXmlElement(szItem)), szItem);
}

CXmlElementA		CXmlFileA::first(const char* szItem)
{
	return CXmlElementA(getXMLA->FirstChildElement(szItem),szItem);
}

#ifndef ENGINE_LIB_MODE
VOID	StudioMessageBox(const char* szText)
{
	//Sleep(1000);
	char buffer[512];
	sprintf(buffer, "%s\n请到www.aixspace.com购买商业版本。", szText);
	MessageBox(0, buffer, "你使用的是免费开发版本", MB_OK);
}
#endif

bool			CXmlFileA::loadFromFile(const char* szFile)
{
	return getXMLA->LoadFile(szFile);
}

bool			CXmlFileA::loadFromMemory(const char* szText)
{
	return getXMLA->LoadFromMemory(szText);
}

CXmlFileA::~CXmlFileA(void)
{
	if(m_pdocFile)
		delete (TiXmlDocument*)m_pdocFile;
}

bool			CXmlFileA::saveXml(const char* szFile)
{
#if defined(Developer_Authorize)
	//StudioMessageBox("你正在使用试用期版本，请购买授权版本。");
#endif
	if(!getXMLA)return false;
	//
	getXMLA->SaveFile(szFile, 0);
	//
	return true;
}


void	readAttr(CXmlElementA& element, const char* szName, float& f)
{
	f = atof(element.read(szName, ""));
}

void	readAttr(CXmlElementA& element, const char* szName, double& f)
{
	f = atof(element.read(szName, ""));
}

void	readAttr(CXmlElementA& element, const char* szName, int& f)
{
	f = atoi(element.read(szName, ""));
}

void	readAttr(CXmlElementA& element, const char* szName, bool& f)
{
	f = atoi(element.read(szName, ""));
}

void	readAttr(CXmlElementA& element, const char* szName, std::string& f)
{
	f = element.read(szName, "");
}
