//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   ViewImageManager.cpp
	软件名称   :   幻影游戏引擎（www.aixspace.com）

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	Copyright 2009-2016 Zhao Dexian
	
	
	
	
	

	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include ".\ViewImageManager.h"
CViewImageManager::CViewImageManager(void)
{
}

CViewImageManager::~CViewImageManager(void)
{
	for(size_t i=0;i<m_imageMap.size();i++)
	{
		delete m_imageMap.at(i);
	}
	m_imageMap.clear();
}
namespace Phantom{
	extern BOOL	SaveToJpg(const char* szDestFile, const char* szFile, int w, int h);
};
extern wchar_t *	GetProjectName();
bool		textIS(const char* str1, const char* str2)
{
	return (stricmp(str1, str2) == 0);
}
extern	const char*	GetDocumentPath();
void CViewImageManager::addImage(const char* strFile)
{
	char szDrive[256], szDir[256], szFile[256];
	_splitpath(strFile, szDrive, szDir, szFile, 0);
	strcat(szDrive, szDir);
	strcat(szDrive, szFile);
	strcat(szDrive, "_Normal");
	strcat(szDrive, ".dds");
	bool bHaveNormal	=	false;
	if(Phantom::fexist(szDrive))
		bHaveNormal		=	true;
	SetProjectPath();
	if(textIS(strFile, "Empty"))
	{
		if(m_imageMap.size() > 0)
			return;
		ImageItem* item = new ImageItem();
		item->filePath	= strFile;
		item->img.Create(64, 64, 16);
		item->name		= szFile;
		item->fileName	=	strFile;
		item->filePath.setIsFile();
		//
		HDC dcD = item->img.GetDC();
		FillRect(dcD, &CRect(0,0, 64, 64), CBrush(RGB(255,255,255)));
		MoveToEx(dcD, 0, 0, NULL);
		LineTo(dcD, 64, 64);
		MoveToEx(dcD, 64, 0, NULL);
		LineTo(dcD, 0, 64);
		//
		item->img.ReleaseDC();
		m_imageMap.push_back( item );
	}
	else
	{
		Phantom::FileName file	=	strFile;
		file.setIsFile();
		for(int i=0;i<m_imageMap.size();i++)
		{
			if(m_imageMap.at(i)->filePath == file.t)
				return;
		}
		char szPath[255], szExt[255];
		_splitpath(strFile, 0, 0, szFile, szExt);
		char szSceneImage[512];
		sprintf(szSceneImage, "%s%sscene", GetDocumentPath(), A(GetProjectName()).buf);
		CreateDirectoryA(szSceneImage, NULL);
		sprintf(szPath, "%s\\%s.jpg", szSceneImage, szFile);
		ImageItem* item =	new ImageItem();
		item->filePath	=	strFile;
		item->name		=	szFile;
		if(bHaveNormal)
			item->name	+=	"[N]";
		//
		strcat(szFile, szExt);
		item->fileName	=	szFile;
		item->filePath.setIsFile();
		//
		if(!Phantom::fexist(szPath))
		{
			if(!Phantom::SaveToJpg(szPath, strFile, 64, 64))
			{
				delete item;
				return;
			}
		}
		if(FAILED(item->img.Load(W(szPath))))
		{
			delete item;
			return;
		}
		m_imageMap.push_back( item );
	}
}

void	enum_files(const char* path,std::vector<PFileName>& _out, int subFolderLevel, std::string* szFindType,int numFindTypes)
{
	int len=(int)strlen(path);
	if(!path||len==0)
		return;
	char szPath[512],szFind[512];
	strcpy(szPath,path);
	//if(szPath[len-1]=='\\'||szPath[len-1]=='/')
	//{
	//}
	if(szPath[len-1]!='\\'&&szPath[len-1]!='/')
	{
		strcat(szPath,"\\");
	}
	/*else if(!isFolder(szPath))
	{
	return;
	}*/
	//
	strcpy(szFind,szPath);
	strcat(szFind,"*.*");
	WIN32_FIND_DATAA wsa;
	HANDLE hFind=FindFirstFileA(szFind,&wsa);
	if(hFind==INVALID_HANDLE_VALUE)
		return;
	bool bFindAll = (szFindType==NULL);
	while(true)
	{
		//
		if(wsa.cFileName[0]!='.')
		{
			if((wsa.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
			{
				if(subFolderLevel > 0)
				{
					char szNewPath[512];
					strcpy(szNewPath,szPath);
					strcat(szNewPath,wsa.cFileName);
					strcat(szNewPath,"\\");
					//
					enum_files(szNewPath,_out,subFolderLevel - 1,szFindType,numFindTypes);
				}
			}
			else if(! (wsa.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				std::string sp=szPath;
				sp+=wsa.cFileName;
				if(!bFindAll)
				{
					char _ext[255];
					_splitpath(sp.c_str(),0,0,0,_ext);
					for(int i=0;i<numFindTypes;i++)
					{
						if(stricmp(_ext,szFindType[i].c_str())==0)
						{
							_out.push_back(PFileName(sp.c_str()));
							break;
						}
					}
				}
				else
					_out.push_back(PFileName(sp.c_str()));
			}
		}
		//
		if(!FindNextFileA(hFind,&wsa))
			break;
	}
	FindClose(hFind);
}
	void	destroyFileList(PMyFileList* fList)
	{
		if(fList)
			delete fList;
	}

	PMyFileList*	enum_files(const char* path, int subFolderLevel,std::string* szFindTypes,int numFindTypes)
	{
		PMyFileList* f = new PMyFileList();
		::enum_files(path,f->vFiles,subFolderLevel,szFindTypes,numFindTypes);
		return f;
	}
void CViewImageManager::reloadImages()
{
	SetProjectPath();
	std::vector<std::string> ft;
	ft.push_back(".dds");
	ft.push_back(".png");
	ft.push_back(".jpg");
	PMyFileList* fl	=	::enum_files("./scene3d/", 0, &ft.at(0), ft.size());
	//for(size_t i=0;i<m_imageMap.size();i++)
		//delete m_imageMap.at(i);
	//m_imageMap.clear();
	//
	addImage("Empty");
	for(size_t i=0;i<fl->vFiles.size();i++)
	{
		strlwr(fl->vFiles.at(i).szFileName);
		if(strstr(fl->vFiles.at(i).szFileName, "_normal."))
			continue;
		if(strstr(fl->vFiles.at(i).szFileName, "_specular."))
			continue;
		this->addImage( fl->vFiles.at(i).szFileName );
	}
	destroyFileList(fl);
}

ImageItem*					CViewImageManager::findImageByName(const char* szName)
{
	for(size_t i=0;i<m_imageMap.size();i++)
	{
		if(m_imageMap.at(i)->name == szName)
			return m_imageMap.at(i);
	}
	return 0;
}

ImageItem*					CViewImageManager::findImageByFile(const char* szFile)
{
	Phantom::FileName file	=	szFile;
	file.setIsFile();
	for(size_t i=0;i<m_imageMap.size();i++)
	{
		if(m_imageMap.at(i)->filePath == file.t)
			return m_imageMap.at(i);
	}
	return 0;
}

ImageItem*					CViewImageManager::findImageByFileName(const char* szFileName)
{
	for(size_t i=0;i<m_imageMap.size();i++)
	{
		Name& file	=	m_imageMap.at(i)->fileName;
		if(file == szFileName)
			return m_imageMap.at(i);
	}
	return 0;
}
