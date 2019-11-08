//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomFile.h"
#include "myunzip.h"
#ifdef ANDROID_OS
#include <sys/stat.h>
#include <dirent.h>
#endif
#include "zlib.h"
#include "encrypt.h"

extern "C"
{
    #include "CInclude.h"
    GetRealPathProc g_realPathProc = 0;
    void    SetRealPathGetting(GetRealPathProc realPathProc)
    {
        g_realPathProc = realPathProc;
    }
    char*   PhantomAlloc(unsigned int size)
    {
        return (char*)Phantom::malloc_s(size);
    }
    void    PhantomFree(char* v)
    {
        Phantom::free_s(v);
    }
#ifdef IPHONE_OS
    extern void DeleteDirectorys(const char* folders);
#endif
	void	OnSearchFile(const char* szFile, const void* ptr)
	{
		Phantom::CDynamicArray<Phantom::FileName>* rets = (Phantom::CDynamicArray<Phantom::FileName>*)ptr;
		Phantom::FileName f;
		f = szFile;
		//f.setIsFile();
		rets->push_back(f);
	}
    void SearchFiles(const char* folders, const void* ptr, int subfolder);
}

namespace Phantom{
	struct ExtFile{
		NameT<16>		tori,tnew;
		ExtFile(){}
		ExtFile(const char* _ori, const char* _new){
			tori = _ori;
			tnew = _new;
		}
	};
	std::vector<ExtFile>&	GetExtCount()
	{
		static std::vector<ExtFile> exts;
		if(exts.size() == 0)
		{
			exts.push_back(ExtFile(".jpg", ".limg"));
			exts.push_back(ExtFile(".png", ".limg"));
			exts.push_back(ExtFile(".rpgui", ".lui"));
			exts.push_back(ExtFile(".rpgmesh", ".lmesh"));
			exts.push_back(ExtFile(".pakui", ".pui"));
			exts.push_back(ExtFile(".pakmesh", ".pmesh"));
			exts.push_back(ExtFile(".rpgscene", ".lscene"));
			exts.push_back(ExtFile(".lua", ".lsc"));
			exts.push_back(ExtFile(".table", ".lt"));
			exts.push_back(ExtFile(".inl", ".lt"));
			exts.push_back(ExtFile(".bin", ".lt"));
			exts.push_back(ExtFile(".dat", ".lt"));
			exts.push_back(ExtFile(".txt", ".lt"));
			//
		}
		return exts;
	}
	LFileName				g_documentDir;
	LFileName				g_projectDir;
	VOID					SetDocumentDir(const char* szDir)
	{
		LFileName dir;
		dir	=	szDir;
		//dir.setIsFile();
		dir.splitpath(&g_documentDir, 0, 0);
		dir	=	"";
		LogInfo("SetDocument(%s)", g_documentDir.t);
	}
    const char*                         GetDocumentDir()
    {
        return g_documentDir.t;
    }

	void			createFolders(const char* documents, const char* szSubFolders)
	{
		char* strPath=(char*)szSubFolders;
		int len=(int)strlen(strPath);
		char strDir[512];
		for(int i=0;i<len;i++)
		{
			strDir[i]=strPath[i];
			if(strPath[i]!='\\' && strPath[i]!='/')
			{
				continue;
			}
			else
			{
				strDir[i]=0;
				char buf[512];
				strcpy(buf, documents);
				strcat(buf, strDir);
#ifdef WIN32_OS
				CreateDirectoryA(buf,NULL);
#elif defined(IPHONE_OS)
				mkpath_np(buf, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#elif defined(ANDROID_OS)
				mkdir(buf, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
				strDir[i]='/';
			}
		}
	}

	void								removeFolders(const char* documents, const char* szSubFolders, int index)
	{
		char szNewPath[512];
		strcpy(szNewPath, documents);
		strcat(szNewPath, szSubFolders);
#ifdef WIN32_OS
		char	ch	=	szNewPath[strlen(szNewPath) - 1];
		if(ch != '\\' && ch != '/')
			strcat(szNewPath, "\\");
		char szTempPath[512];
		strcpy(szTempPath, szNewPath);
		strcat(szNewPath, "*.*");
		WIN32_FIND_DATAA win32;
		memset(&win32, 0, sizeof(win32));
		HANDLE hFile	=	::FindFirstFileA(szNewPath, &win32);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			while(true)
			{
				if(win32.cFileName[0] != '.' && !(win32.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
				{
					if((win32.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
					{
						strcpy(szNewPath, szSubFolders);
						strcat(szNewPath, win32.cFileName);
						strcat(szNewPath, "\\");
						removeFolders(documents, szNewPath, index+1);
					}
					else
					{
						strcpy(szNewPath, szTempPath);
						strcat(szNewPath, win32.cFileName);
						DeleteFile(szNewPath);
					}
				}
				if(!::FindNextFileA(hFile, &win32))
					break;
			}
		}
		FindClose(hFile);
		szTempPath[strlen(szTempPath)-1] = 0;
		if(index>0)
			RemoveDirectory(szTempPath);
		//
#elif defined(IPHONE_OS)
        char szTempPath[512];
        strcpy(szTempPath, szNewPath);
        char	ch	=	szNewPath[strlen(szNewPath) - 1];
        if(ch == '\\' || ch == '/')
            szNewPath[strlen(szNewPath) - 1] = 0;
        DeleteDirectorys(szNewPath);
#elif defined(ANDROID_OS)
	char szTempPath[512];
	strcpy(szTempPath, szNewPath);
	char	ch	=	szNewPath[strlen(szNewPath) - 1];
	if(ch == '\\' || ch == '/')
		szNewPath[strlen(szNewPath) - 1] = 0;
	DIR* dp = NULL;
    DIR* dpin = NULL;
    struct dirent* dirp;
    dp = opendir(szNewPath);
	LogInfo("removeFolder(%d)", dp);
    if(dp == NULL)
        return;
    while((dirp = readdir(dp)) != NULL)
    {
        if(strcmp(dirp->d_name, "..") == 0 || strcmp(dirp->d_name, ".") == 0)
            continue;
        strcpy(szNewPath, szSubFolders);
        strcat(szNewPath, dirp->d_name);
        dpin = opendir(szNewPath);
        if(dpin != NULL)
		{
			//strcat(szNewPath, "\\");
            removeFolders(documents, szNewPath, index+1);
		}
        else
            remove(szNewPath);
        strcpy(szNewPath, "");
        closedir(dpin);
        dpin = NULL;
    }
	if(index>0)
		rmdir(szTempPath);
    closedir(dp);
    dirp = NULL;
#endif
	}
	void								removeFolders(const char* documents, const char* szSubFolders)
	{
		removeFolders(documents, szSubFolders, 0);
	}

	void								searchFiles(CDynamicArray<FileName>& rets, const char* documents, const char* szSubFolders, int subfolder)
	{
		if(subfolder<0)return;
		char szNewPath[512];
		strcpy(szNewPath, documents);
		strcat(szNewPath, szSubFolders);
#ifdef WIN32_OS
		char	ch	=	szNewPath[strlen(szNewPath) - 1];
		if(ch != '\\' && ch != '/')
			strcat(szNewPath, "\\");
		char szTempPath[512];
		strcpy(szTempPath, szNewPath);
		strcat(szNewPath, "*.*");
		WIN32_FIND_DATAA win32;
		memset(&win32, 0, sizeof(win32));
		HANDLE hFile	=	::FindFirstFileA(szNewPath, &win32);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			while(true)
			{
				if(win32.cFileName[0] != '.' && !(win32.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
				{
					if((win32.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
					{
						strcpy(szNewPath, szSubFolders);
						strcat(szNewPath, win32.cFileName);
						strcat(szNewPath, "\\");
						searchFiles(rets, documents, szNewPath, subfolder-1);
					}
					else// if(stricmp(szFiles, win32.cFileName)==0)
					{
						strcpy(szNewPath, szSubFolders);
						strcat(szNewPath, win32.cFileName);
						FileName f = szNewPath;
						f.setIsFile();
						rets.push_back(f);
					}
				}
				if(!::FindNextFileA(hFile, &win32))
					break;
			}
		}
		FindClose(hFile);
		//
#elif defined(IPHONE_OS)
        char szTempPath[512];
        strcpy(szTempPath, szNewPath);
        char	ch	=	szNewPath[strlen(szNewPath) - 1];
        if(ch == '\\' || ch == '/')
            szNewPath[strlen(szNewPath) - 1] = 0;
        SearchFiles(szNewPath, &rets, subfolder);
#elif defined(ANDROID_OS)
	char	ch	=	szNewPath[strlen(szNewPath) - 1];
	if(ch == '\\' || ch == '/')
		szNewPath[strlen(szNewPath) - 1] = 0;
	DIR* dp = NULL;
    DIR* dpin = NULL;
    struct dirent* dirp;
    dp = opendir(szNewPath);
    if(dp == NULL)
        return;
    while((dirp = readdir(dp)) != NULL)
    {
        if(strcmp(dirp->d_name, "..") == 0 || strcmp(dirp->d_name, ".") == 0)
            continue;
		char szTemp[512], szNewSub[512];
        strcpy(szTemp, documents);
		strcpy(szNewSub, szSubFolders);
        strcat(szNewSub, dirp->d_name);
		strcat(szTemp, szNewSub);
        dpin = opendir(szTemp);
        if(dpin != NULL)
		{
			strcat(szNewSub, "/");
            searchFiles(rets, documents, szNewSub, subfolder-1);
		}
        else// if(stricmp(dirp->d_name, szFiles)==0)
		{
			rets.push_back(szNewSub);
		}
        strcpy(szNewPath, "");
        closedir(dpin);
        dpin = NULL;
    }
    closedir(dp);
    dirp = NULL;
#endif
	}
	void								removeFile(const char* szFile)
	{
#ifdef WIN32_OS
		DeleteFile(szFile);
#elif defined(IPHONE_OS)
		remove(szFile);
#elif defined(ANDROID_OS)
		remove(szFile);
#endif
		LogInfo("removefile:%s",szFile);
	}

	VOID					SetProjectDir(const char* szDir)	//设置工程目录
	{
		g_projectDir	=	szDir;
		g_projectDir.setIsFile();
		if(g_projectDir.size() > 0)
		{
			if(g_projectDir[g_projectDir.size() - 1] != '/')
				g_projectDir	+=	'/';
		}
	}
	const LFileName&		GetProjectDir()
	{
		return g_projectDir;
	}
    BOOL    RealPathExist(const char* szFile, char* rets = 0)
    {
		if(rets)
			strcpy(rets, szFile);
        const char* isdoc = strstr(szFile, "Documents");
        if(g_realPathProc&&!isdoc)
        {
            LFileName ret,dir,file,ext;
            ret = szFile;
            ret.splitpath(&dir,&file,&ext);
            if((*g_realPathProc)(ret.t, dir.t, file.t, ext.t, true))
            {
                FILE* f = fopen(ret.t, "rb");
                if(f)
                {
                    if(rets)
                        strcpy(rets, ret.t);
                    fclose(f);
                    return true;
                }
            }
        }
        else {
            FILE* f = fopen(szFile, "rb");
            if(f)
            {
                if(rets)
                    strcpy(rets, szFile);
                fclose(f);
                return true;
            }
        }
        return false;
    }
	BOOL	g_bPlatformMode = false;
	VOID					SetPlatformMode(BOOL b){
		g_bPlatformMode	=	b;
	}
	BOOL					fexistLocal(const char* szFile, LFileName* retNewPath)	//if file exist
	{
		if(g_bPlatformMode)
		{
			LFileName r;
			r		=	g_documentDir;
			r		+=	szFile;
            FILE* f = fopen(r.t,"rb");
            if(f)
			{
                fclose(f);
				if(retNewPath)
					*retNewPath	=	r;
				return true;
			}
		}
		//
        if(RealPathExist(szFile))
        {
            if(retNewPath)
				*retNewPath	=	szFile;
			return true;
		}
		LFileName r;
		r		=	g_projectDir;
		r		+=	szFile;
		if(RealPathExist(r.t))
		{
			if(retNewPath)
				*retNewPath	=	r;
			return true;
		}
 		if(retNewPath)
			*retNewPath	=	szFile;
		for(int i=0;i<ZipFilePackage::s_zipFiles.size();i++)
		{
			if(ZipFilePackage::s_zipFiles.at(i)->fexist(szFile))
				return true;
		}
		return false;
	}
	BOOL					fexistLocalDoc(const char* szFile, LFileName* retNewPath)	//if file exist
	{
		LFileName r;
		r		=	g_documentDir;
		r		+=	szFile;
        FILE* f = fopen(r.t, "rb");
        if(f)
        {
            fclose(f);
			if(retNewPath)
				*retNewPath	= r;
			return true;
		}
		return false;
	}
	BOOL					fexist_doc(const char* szFile, LFileName* retNewPath)
	{
		LFileName ft = szFile;
		LFileName dir, file, ext;
		ft.splitpath(&dir, &file, &ext);
		BOOL bImage = false;
		std::vector<ExtFile>& exts = GetExtCount();
		for(int i=0;i<exts.size();i++)
		{
			if(ext == exts[i].tori.t)
			{
				dir += file;
				dir += exts[i].tnew.t;
				if(fexistLocalDoc(dir, &file))//如果存在image文件
					return true;
				break;
			}
		}
		return fexistLocalDoc(szFile, retNewPath);
	}
	BOOL					fexist(const char* szFile, LFileName* retNewPath)	//if file exist
	{
		LFileName ft = szFile;
		LFileName dir, file, ext;
		ft.splitpath(&dir, &file, &ext);
		BOOL bImage = false;
		std::vector<ExtFile>& exts = GetExtCount();
		for(int i=0;i<exts.size();i++)
		{
			if(ext == exts[i].tori.t)
			{
				dir += file;
				dir += exts[i].tnew.t;
				if(fexist_mem(dir, &file) || fexistLocal(dir, &file))//如果存在image文件
					return true;
#ifdef ENGINE_LIB_MODE
				const char* def = "default_init_zip";
				if(strnicmp(szFile, def, strlen(def))==0){
				}
				else{
					//LogInfo("system->error:资源必须经过加密");
					return false;
				}
#endif
				break;
			}
		}
		return (fexist_mem(dir, &file) || fexistLocal(szFile, retNewPath));
	}
	StreamBase*		create_s(const char* szFile)		//create file in memory
	{
		LFileName temp = g_documentDir.t;
		temp += szFile;
		//temp.setIsFile();
		FILE* f	=	fopen(temp.t, "wb");
		if(!f)
		{
			//FileName path, file,ext;
			//path = szFile;
			//path.setIsFile();
			//path.splitpath(0, &file, &ext);
			//file	+=	ext.t;
			//LFileName doc;
			//doc = g_documentDir.t;
			//doc += file;
			f = fopen(szFile, "wb");
			if(!f)
            {
				return 0;
            }
            //LogInfo("success: create io file %s ok.", doc.t);
		}
		IOFile* file = new IOFile();
		file->m_pFile	=	f;
		return file;
	}
    StreamBase*					open_s_inDoc(const char* szFile, BOOL bUseMemoryFile)
	{
		FILE* f	=	0;
		{
			FileName path, file,ext;
			path = szFile;
			path.setIsFile();
			path.splitpath(0, &file, &ext);
			file	+=	ext.t;
			LFileName doc;
			doc = g_documentDir.t;
			doc += file;
			f = fopen(doc.t, "rb");
			if(!f)
            {
				LogError("open_s_inDoc[fail](%s)(%s)", doc.t, szFile);
				return 0;
            }
            //LogInfo("success: open document file %s ok.", doc.t);
		}
        if(bUseMemoryFile)
        {
            fseek(f, 0, SEEK_END); // seek to end of file
            int64 size = ftell(f); // get current file pointer
            fseek(f, 0, SEEK_SET);
            MemoryFile* base	=	new MemoryFile();
            char* buf = (char*)malloc_s(size + 1);
            fread(buf, size, 1, f);
            buf[size] = 0;
            base->open(buf, buf, size, true);
            fclose(f);
            return base;
        }
		IOFile* file = new IOFile();
		file->m_pFile	=	f;
		return file;
        
    }//load file from szFile in document path, return StreamBase
    BOOL    IsInDocument(const char* file, LFileName* ret){
		if(g_bPlatformMode)
		{
			LFileName r;
			r		=	g_documentDir;
			r		+=	file;
            FILE* f = fopen(r.t,"rb");
            if(f)
			{
                fclose(f);
				if(ret)
					*ret	=	r;
				return true;
			}
		}
        return false;
    }
	BOOL			open_s_full(const char* szFile, CDynamicArray<char>& rets)	//load file from szFile, return array
	{
		StreamBase* s = open_s(szFile, true);
		if(!s)return false;
		if(s->size()>0){
			rets.resize(s->size());
			s->read(rets.GetPtr(), rets.size());
		}
		close_s(&s);
		return true;
	}
	extern	unsigned char*	DecryptMemoryFile(const char* szExt, unsigned char* buf, int size, int& newsize, int& begin);
	StreamBase*		open_s(const char* szFile, BOOL bUseMemoryFile)	//load file from szFile, return StreamBase
	{
		LFileName ft = szFile;
		LFileName dir, file, ext;
		ft.splitpath(&dir, &file, &ext);
		BOOL bImage = false;
		std::vector<ExtFile>& exts = GetExtCount();
        if(!IsInDocument(ft.t,&file)){
		for(int i=0;i<exts.size();i++)
		{
			if(ext == exts[i].tori.t)
			{
				dir += file;
				dir += exts[i].tnew.t;
				if(fexist(dir, &file))//如果存在image文件
				{
					bImage	=	true;
					ext = exts[i].tnew.t;
					szFile = file;
				}
				break;
			}
		}
        }
        else
        {
            szFile = file;
        }
		if(bUseMemoryFile || bImage)
		{
			LFileName temp;
			if(fexist_mem(szFile, &temp))//内存文件优先打开
			{
				StreamBase* mem = open_mem_s(temp.t);
				if(!mem)
					return 0;
				temp.splitpath(&dir, &file, &ext);
				int size = mem->size();
				char* buf = (char*)malloc_s(size + 1);
				mem->read(buf, size);
				buf[size] = 0;
				close_s(&mem);
				//
				int newsize = 0;
				int begin = 0;
				char* beginPtr = (char*)DecryptMemoryFile(ext, (unsigned char*)buf, size, newsize, begin);
				if(!beginPtr)
				{
					return 0;
				}
				//beginPtr[newsize] = 0;
				//assert((int64)(beginPtr+newsize) <= (int64)(buf+size));
				if(buf != beginPtr)//如果重新分配了数据
					delete[] buf;
				MemoryFile* base	=	new MemoryFile();
				base->open((char*)beginPtr, (char*)beginPtr + begin, newsize, true);
				return base;

			}
			else if(fexist(szFile, &temp))
			{
                LFileName openText;
                RealPathExist(temp.t, openText.t);
				FILE* f	=	fopen(openText.t, "rb");
				if(!f)
				{
					for(int i=0;i<ZipFilePackage::s_zipFiles.size();i++)
					{
						if(ZipFilePackage::s_zipFiles.at(i)->fexist(szFile))
                        {
                            //LogInfo("success: open ziped file %s OK.", openText.t);
							return ZipFilePackage::s_zipFiles.at(i)->open(szFile);
                        }
					}
                    LogInfo("system->error:read ziped file %s failure.", openText.t);
					return 0;
				}

                //LogInfo("success: open momory file %s OK.", openText.t);
				fseek(f, 0, SEEK_END); // seek to end of file
				int64 size = ftell(f); // get current file pointer
				fseek(f, 0, SEEK_SET);
				char* buf = (char*)malloc_s(size + 1);
				fread(buf, size, 1, f);
				buf[size] = 0;
				int newsize = 0;
				int begin = 0;
				char* beginPtr = (char*)DecryptMemoryFile(ext, (unsigned char*)buf, size, newsize, begin);
				if(!beginPtr)
				{
					fclose(f);
					LogInfo("system->error:(%s)%s", szFile, utf8("不能打开没有获得授权"));
					return 0;
				}
				MemoryFile* base	=	new MemoryFile();
				//beginPtr[newsize] = 0;
				//assert((int64)(beginPtr+newsize) <= (int64)(buf+size));
				if(buf != beginPtr)//如果重新分配了数据
					delete[] buf;
				base->open((char*)beginPtr, (char*)beginPtr + begin, newsize, true);
				fclose(f);
				return base;
			}
			else//尝试打开zip文件
			{
                //LogInfo("error: read memory file %s not exist.", szFile);
				return 0;
			}
		}
		else
		{
			LFileName temp;
			if(fexist(szFile, &temp))
			{
                LFileName openText;
                RealPathExist(temp.t, openText.t);
				FILE* f	=	fopen(openText.t, "rb");
				if(!f)
                {
                    LogInfo("system->error: open file %s failure.", openText.t);
					return 0;
                }
                //LogInfo("success: open file %s OK.", openText.t);
				IOFile* file = new IOFile();
				file->m_pFile	=	f;
				return file;
			}
			else//尝试打开zip文件
			{
                LogInfo("system->error: io file %s not exist.", szFile);
				return 0;
			}
		}
		return 0;
	}
	MemoryFileStatic::MemoryFileStatic()
	{
		m_pOrigPtr		=	0;
		m_nAllocSize	=	0;
	}
	MemoryFileStatic::~MemoryFileStatic()
	{
		safe_delete(m_pOrigPtr);
	}
	std::vector<MemoryFileStatic*>	g_memoryFiles;
	VOID			FreeAllMemoryFiles()//释放所有创建的内存文件
	{
		for(int i=0;i<g_memoryFiles.size();i++)
		{
			safe_release(g_memoryFiles[i]);
		}
		g_memoryFiles.clear();
	}
	BOOL			fexist_mem(const char* szFile, LFileName* retNewPath)	//if file exist
	{
		LFileName temp = szFile;
		temp.setIsFile();
		LFileName dir, file, ext;
		temp.splitpath(&dir, &file, &ext);
		BOOL bImage = false;
		std::vector<ExtFile>& exts = GetExtCount();
		for(int i=0;i<exts.size();i++)
		{
			if(ext == exts[i].tori.t)
			{
				dir += file;
				dir += exts[i].tnew.t;
				for(int j=0;j<g_memoryFiles.size();j++)
				{
					MemoryFileStatic* ms = g_memoryFiles[j];
					if(ms->m_fileName == dir)
					{
						if(retNewPath)
							*retNewPath = ms->m_fileName.t;
						return true;
					}
				}
			}
		}
		for(int j=0;j<g_memoryFiles.size();j++)
		{
			MemoryFileStatic* ms = g_memoryFiles[j];
			if(ms->m_fileName == temp)
			{
				if(retNewPath)
					*retNewPath = ms->m_fileName.t;
				return true;
			}
		}
		return false;
	}
	StreamBase*		open_mem_s(const char* szFile)		//create file in memory
	{
		FileName temp = szFile;
		temp.setIsFile();
		MemoryFileStatic* s = 0;
		for(int i=0;i<g_memoryFiles.size();i++)
		{
			if(g_memoryFiles[i]->m_fileName == temp)
			{
				s = g_memoryFiles[i];
				break;
			}
		}
		if(!s)
			return 0;
		MemoryFile* base	=	new MemoryFile();
		base->open(s);
		return base;
	}
	StreamBase*		open_memory(const char* buffer, int64 nLength)
	{
		MemoryFile* base	=	new MemoryFile();
		char* newbuf = new char[nLength];
		memcpy(newbuf, buffer, nLength);
		base->open(newbuf, newbuf, nLength, true);
		return base;
	}
	StreamBase*		create_mem_s(const char* szFile, int64 nLength)		//create file in memory
	{
		FileName temp = szFile;
		temp.setIsFile();
		MemoryFileStatic* s = 0;
		for(int i=0;i<g_memoryFiles.size();i++)
		{
			if(g_memoryFiles[i]->m_fileName == temp)
			{
				safe_release(g_memoryFiles[i]);
				//s = g_memoryFiles[i];
				break;
			}
		}
		if(!s)
		{
			s = new MemoryFileStatic();
			g_memoryFiles.push_back(s);
			if(nLength>0)
				s->m_pOrigPtr = new char[nLength];
			else
				s->m_pOrigPtr = 0;
			s->m_nAllocSize = nLength;
			s->m_fileName = temp;
		}
		MemoryFile* base	=	new MemoryFile();
		base->open(s);
		return base;
	}
	void				close_s(StreamBase** file)		//create file in memory
	{
		if(*file)
			delete *file;
		*file	=	0;
	}

	IOFile::IOFile()
	{
		m_bReadOnly	=	false;
		m_pFile		=	0;
	}
	IOFile::~IOFile()
	{
		close();
	}

	int64						IOFile::write(const void* t,int64 tLength)
	{CPUTime(IOFile);
		if(!m_pFile)
			return 0;
		return fwrite(t, tLength, 1, m_pFile);
	}
	int64						IOFile::read(void* t,int64 tLength)
	{CPUTime(IOFile);
		if(!m_pFile)
			return 0;
		return fread(t, tLength, 1, m_pFile);
	}
	int64						IOFile::seek(int64 numSeek, int seekType)
	{CPUTime(IOFile);
		if(!m_pFile)
			return 0;
		return fseek(m_pFile, numSeek, seekType);
	}
	int64						IOFile::tell()
	{CPUTime(IOFile);
		if(!m_pFile)
			return 0;
		return ftell(m_pFile);
	}
	int64						IOFile::size()
	{CPUTime(IOFile);
		if(!m_pFile)
			return 0;
		int64 pos = ftell(m_pFile); // get current file pointer
		fseek(m_pFile, 0, SEEK_END); // seek to end of file
		int64 size = ftell(m_pFile); // get current file pointer
		fseek(m_pFile, pos, SEEK_SET);
		return size;
	}
	BOOL					IOFile::eof()
	{CPUTime(IOFile);
		if(!m_pFile)
			return 0;
		return feof(m_pFile);
	}
	void					IOFile::close()
	{CPUTime(IOFile);
		if(!m_pFile)
			return;
		fclose(m_pFile);
		m_pFile	=	0;
	}

	int64		MemoryFile::read(void* t,int64 nBuffer)
	{CPUTime(MemoryFile);
		if(!this->m_pFile)return 0;
		memset_s(t, 0, nBuffer);
		int64 num = getmin((int64)nBuffer,(int64)this->m_nFileSize - (int64)m_lPoint);
		if(num<=0)return 0;
		memcpy_s(t , m_pFile + m_lPoint,num);
		m_lPoint+=num;
		return num;
	}

	int64		MemoryFile::write(const void* t,int64 tLength)
	{CPUTime(MemoryFile);
		if(!this->m_pFile)return 0;
		int64 num = m_lPoint + tLength;
		if(num > m_nAllocSize)
		{
			if(m_bSelfCreate)
			{
				m_nAllocSize	=	m_nAllocSize + tLength + 1024*64;
				m_pFile			=	(char*)realloc_s(m_pFile, m_nAllocSize);
			}
			else
				return 0;
		}
		memcpy_s((char*)m_pFile + m_lPoint,t,tLength);
		//
		m_lPoint+=tLength;
		if(m_nFileSize < m_lPoint)
			m_nFileSize	=	m_lPoint;
		return tLength;
	}

	MemoryFile::MemoryFile()
	{
		this->m_pFile		=	0;
		this->m_nFileSize	=	0;
		m_nAllocSize		=	0;
		this->m_lPoint		=	0;
		m_bSelfCreate		=	false;
		m_pOrigPtr			=	0;
		m_memFile			=	0;
	}

	MemoryFile::~MemoryFile()
	{
		close();
	}

	BOOL	MemoryFile::create(int64 nFileSize)
	{CPUTime(MemoryFile);
		close();
		m_bSelfCreate		=	true;
		m_nAllocSize		=	nFileSize;
		if(m_nAllocSize > 0)
			m_pOrigPtr = this->m_pFile	=	(char*)malloc_s(m_nAllocSize);
		else
			m_pOrigPtr = this->m_pFile	=	0;
		this->m_nFileSize	=	m_nAllocSize;
		this->m_lPoint		=	0;
		return true;
	}
	BOOL	MemoryFile::open(MemoryFileStatic* mem)
	{CPUTime(MemoryFile);
		safe_addref(mem);
		BOOL bRet = open(mem->m_pOrigPtr, mem->m_pOrigPtr, mem->m_nAllocSize, false);
		this->m_memFile = mem;
		m_nAllocSize = mem->m_nAllocSize;
		return bRet;
	}
	BOOL	MemoryFile::open(char* origPtr, char* pFile, int64 dwFileSize, BOOL bAutoDelete)
	{CPUTime(MemoryFile);
		close();
		m_pOrigPtr			=	origPtr;
		this->m_bSelfCreate =	bAutoDelete;
		this->m_pFile		=	pFile;
		this->m_nFileSize	=	dwFileSize;
		this->m_lPoint		=	0;
		if(GetCallbackPtr())
			GetCallbackPtr()->OnOpenFile(pFile, dwFileSize);
		return (m_pFile!=NULL);
	}
	//
	int64		MemoryFile::seek(int64 nSeek,int nType)
	{CPUTime(MemoryFile);
		if(!this->m_pFile)
			return 0;
		switch(nType)
		{
		case SEEK_CUR:
			this->m_lPoint+=nSeek;
			break;
		case SEEK_SET:
			this->m_lPoint=nSeek;
			break;
		case SEEK_END:
			this->m_lPoint=this->m_nFileSize - nSeek;
			break;
		}
		if(m_lPoint>=this->m_nFileSize)
			m_lPoint = this->m_nFileSize - 1;
		return m_lPoint;
	}
	int		MemoryFile::tell()
	{CPUTime(MemoryFile);
		if(!this->m_pFile)
			return 0;
		return m_lPoint;
	}
	void	MemoryFile::close()
	{CPUTime(MemoryFile);
		safe_release(m_memFile);
		if(m_bSelfCreate)
			free_s(m_pOrigPtr);
		this->m_pOrigPtr=NULL;
		this->m_pFile=NULL;
	}



	TableTxt::TableTxt(void)
	{
	}

	TableTxt::~TableTxt(void)
	{
	}
	BOOL	TableTxt::read(int& ret, int lineIndex, const char* headName)
	{CPUTime(TableTxt);
		const char* sz = this->getLineItem(lineIndex, headName);
		if(!sz)return false;
		ret = atoi(sz);
		return true;
	}
	BOOL	TableTxt::read(long& ret, int lineIndex, const char* headName)
	{CPUTime(TableTxt);
		const char* sz = this->getLineItem(lineIndex, headName);
		if(!sz)return false;
		ret = atoi(sz);
		return true;
	}
	BOOL	TableTxt::read(double& ret, int lineIndex, const char* headName)
	{CPUTime(TableTxt);
		const char* sz = this->getLineItem(lineIndex, headName);
		if(!sz)return false;
		ret = atof(sz);
		return true;
	}
	BOOL	TableTxt::read(float& ret, int lineIndex, const char* headName)
	{CPUTime(TableTxt);
		const char* sz = this->getLineItem(lineIndex, headName);
		if(!sz)return false;
		ret = (float)atof(sz);
		return true;
	}
	BOOL	TableTxt::read(char* ret, int lineIndex, const char* headName)
	{CPUTime(TableTxt);
		const char* sz = this->getLineItem(lineIndex, headName);
		if(!sz)return false;
		strcpy(ret, sz);
		return true;
	}
	const char*			TableTxt::read(int lineIndex, const char* headName)
	{CPUTime(TableTxt);
		if(headName)
		{
			if((int)m_vTable.size()<=lineIndex)
			{
				return 0;
			}
			std::string* str = getLineItem(m_vTable.at(lineIndex), headName);
			if(str)
				return str->c_str();
		}
		return 0;
	}

	char*	TableTxt::readLine(char* ptr, TableHeader& lineData, char chSkin)
	{CPUTime(TableTxt);
		char szBuf[2056];
		int lineCount	=	0;
		int index		=	0;
		while(*ptr != 0)
		{
			if(*ptr == '\r')
			{
				ptr++;
				continue;
			}
			if(*ptr == chSkin || (ptr[0] == '\n'))
			{
				szBuf[index++] = 0;
				if(index > 0)
				{
					if(stricmp(szBuf, "n/a") == 0)
						szBuf[0] = 0;
				}
				lineData.push_back( szBuf );
				lineCount++;
				index	=	0;
				//
				if(*ptr == '\n')
				{
					ptr++;
					break;
				}
				ptr++;
				continue;
			}
			else if(*ptr == '\"')
			{
				ptr++;
				while(*ptr != 0)
				{
					if(*ptr == '\"')
						break;
					szBuf[index++] = *ptr;
					ptr++;
				}
				ptr++;
				continue;
			}
			else if(*ptr == '\'')
			{
				ptr++;
				while(*ptr != 0)
				{
					if(*ptr == '\'')
						break;
					szBuf[index++] = *ptr;
					ptr++;
				}
				ptr++;
				continue;
			}
			szBuf[index++] = *ptr;
			ptr++;
		}
		if(this->m_vTableHead.size() > 0)
		{
			if(lineCount < m_vTableHead.size())
			{
				for(int i=lineCount;i<m_vTableHead.size();i++)
				{
					lineData.push_back( "" );
				}
				return ptr;
			}
		}
		return ptr;
	}
	BOOL	TableTxt::readFromMemory(const char* szTableData,int szTableDataLength,char chSkin)
	{CPUTime(TableTxt);
		char* ptr	=	(char*)szTableData;
		BOOL bHead	=	false;
		ptr			=	readLine(ptr, this->m_vTableHead, chSkin);
		if(!ptr)
			return false;
		while(*ptr != 0)
		{
			while(*ptr == ' ' || *ptr == '\r' || *ptr == '\n')
				ptr++;
			if(*ptr == 0)
				break;
			if((ptr[0] == '*'&&ptr[1] == '*') || (ptr[0] == '/'&&ptr[1] == '/') || (ptr[0] == '\''&&ptr[1] == '\''))
			{
				while(*ptr != 0 && *ptr != '\n')
					ptr++;
				if(*ptr == 0)
					break;
				if(*ptr == '\n')ptr++;
			}
			//
			m_vTable.push_back(TableHeader());
			ptr		=	readLine(ptr, this->m_vTable.at(m_vTable.size() - 1), chSkin);
			if(!ptr)
				return false;
		}
		return true;
	}

	std::string*	TableTxt::getLineItem(TableHeader& vLineIn,const char* szHeadName)
	{CPUTime(TableTxt);
		int nLineItem=(int)vLineIn.size();
		if((int)m_vTableHead.size()!=nLineItem)return NULL;
		for(int i=0;i<nLineItem;i++)
		{
			if(stricmp(m_vTableHead.at(i).c_str(),szHeadName)==0)
				return &vLineIn.at(i);
		}
		return NULL;
	}


	int		TableTxt::getNumLine()
	{CPUTime(TableTxt);
		return (int)m_vTable.size();
	}

	const char*	TableTxt::getLineItem(int lineIndex,const char* headName, const char* szDefault)
	{CPUTime(TableTxt);
		if(headName)
		{
			if((int)m_vTable.size()<=lineIndex)
			{
				return szDefault;
			}
			std::string* str = getLineItem(m_vTable.at(lineIndex),headName);
			if(str)
				return str->c_str();
		}
		return szDefault;
	}

	BOOL	TableTxt::getLineItem(int lineIndex, const char* headName, float& f)
	{CPUTime(TableTxt);
		const char* str = getLineItem(lineIndex, headName);
		if(!str)return false;
		f = (float)atof(str);
		return true;
	}

	BOOL	TableTxt::getLineItem(int lineIndex, const char* headName, int& i)
	{CPUTime(TableTxt);
		const char* str = getLineItem(lineIndex, headName);
		if(!str)return false;
		i = atoi(str);
		return true;
	}

	BOOL	TableTxt::getLineItem(int lineIndex, const char* headName, char* buf, int numBuffer)
	{CPUTime(TableTxt);
		const char* str = getLineItem(lineIndex, headName);
		if(!str)return false;
		int len = (int)strlen(str);
		if(numBuffer>len)
			strcpy(buf, str);
		else
		{
			strncpy(buf, str, numBuffer - 1);
		}
		return true;
	}


	int		TableTxt::getHeadIndex(const char* headName)
	{CPUTime(TableTxt);
		if(headName)
		{
			int nLineItem=(int)m_vTableHead.size();
			for(int i=0;i<nLineItem;i++)
			{
				if(stricmp(m_vTableHead.at(i).c_str(),headName)==0)
					return i;
			}
		}
		return -1;
	}

	const char*	 TableTxt::getLineItemI(int lineIndex,int headIndex)	//得到第lineIndex行的第headIndex列
	{CPUTime(TableTxt);
		if(lineIndex >= (int)m_vTable.size())
		{
			return "";
		}
		int nLineItem=(int)m_vTableHead.size();
		if(headIndex>=nLineItem)
		{
			return "";
		}
		return m_vTable.at(lineIndex).at(headIndex).c_str();
	}

	int			TableTxt::getNumHead()		//返回标头个数
	{CPUTime(TableTxt);
		return (int)m_vTableHead.size();
	}
	int				TableTxt::GetIntVal(int row, int col)
	{CPUTime(TableTxt);
		return atoi(getLineItemI(row, col));
	}
	double			TableTxt::GetDoubleVal(int row, int col)
	{CPUTime(TableTxt);
		return atof(getLineItemI(row, col));
	}
	const char*		TableTxt::GetTextVal(int row, int col)
	{CPUTime(TableTxt);
		return getLineItemI(row, col);
	}
	int				TableTxt::SearchCol(const char* columnName)
	{CPUTime(TableTxt);
		return this->getHeadIndex(columnName);
	}
	int				TableTxt::SearchRow(int id)
	{CPUTime(TableTxt);
		int col = GetIDCol();
		if(col < 0)
			return -1;
		int numLine = (int)this->m_vTable.size();
		for(int i=0;i<numLine;i++)
		{
			if(atoi(m_vTable.at(i).at(col).c_str()) == id)
				return i;
		}
		return -1;
	}
	int				TableTxt::SearchRow(const char* name)
	{CPUTime(TableTxt);
		int col = GetNameCol();
		if(col < 0)
			return -1;
		int numLine = (int)this->m_vTable.size();
		for(int i=0;i<numLine;i++)
		{
			if(stricmp(m_vTable.at(i).at(col).c_str(), name) == 0)
				return i;
		}
		return -1;
	}
	int				TableTxt::SearchRow(const char* columnName, const char* name)
	{CPUTime(TableTxt);
		int col = this->getHeadIndex(columnName);
		if(col < 0)
			return -1;
		int numLine = (int)this->m_vTable.size();
		for(int i=0;i<numLine;i++)
		{
			if(stricmp(m_vTable.at(i).at(col).c_str(), name) == 0)
				return i;
		}
		return -1;
	}
	int				TableTxt::GetIDCol()
	{CPUTime(TableTxt);
		return getHeadIndex("id");
	}
	int				TableTxt::GetNameCol()
	{CPUTime(TableTxt);
		return getHeadIndex("name");
	}

	const char*	TableTxt::getHead(int headIndex)		//返回标头名称
	{CPUTime(TableTxt);
		if(headIndex>=(int)m_vTableHead.size())
		{
			return "";
		}
		return m_vTableHead.at(headIndex).c_str();
	}

	const char*	TableTxt::findLineText( const char* szFind, int headIndex , int headRet)//得到指定数据的行
	{CPUTime(TableTxt);
		int numLine = (int)this->m_vTable.size();
		for(int i=0;i<numLine;i++)
		{
			if(stricmp( m_vTable.at(i).at(headIndex).c_str() , szFind ) == 0)
				return m_vTable.at(i).at(headRet).c_str();
		}
		return "";
	}

	void	TableTxt::destroy()
	{CPUTime(TableTxt);
		this->m_vTable.clear();
		this->m_vTableHead.clear();
	}

	int		ConvertAB(short_t* ptr)
	{
		int index = 0;
		while(*ptr != 0)
		{
			char* ch	=	(char*)ptr;
			char c		=	ch[0];
			ch[0]		=	ch[1];
			ch[1]		=	c;
			ptr++;
			index++;
		}
		return index;
	}

	BOOL	TableTxt::Load(const char* szFile,char chSymbol)
	{CPUTime(TableTxt);
		//释放掉之前所有的资源
		destroy();
		//
		StreamBase* s	=	open_s(szFile);
		if(!s)
			return false;
		FileName v = szFile;
		v.setIsFile();
		v.splitpath(0, &m_tableName, 0);
		BOOL bret = false;
		unsigned char head[3];
		s->read(head, 3);
		if(!(head[0] == 0xef && head[1] == 0xbb && head[2] == 0xbf))
		{
#ifdef WIN32_OS
			LogInfo("system->error:%s. table file must be a utf8 char code.",utf8("表格txt文件格式必须得是有BOM的utf8格式编码,将被强制转换成utf8格式"));
			s->seek(0);
			int len = s->size();
			char* buffer = new char[len+1];
			s->read(buffer, len);
			buffer[len] = 0;
			CDynamicArray<char> buf;
			Phantom::AsciiToUtf8(buf, buffer);
			SaveUtf8(szFile, buf.GetPtr());
			delete[] buffer;
#else
			LogInfo("system->error:%s. table file must be a utf8 char code.",utf8("表格txt文件格式必须得是有BOM的utf8格式编码"));
#endif
			close_s(&s);
			return false;
		}
		int length = s->size()-3;
		char* szBuffer = new char[length+1];
		szBuffer[length] = 0;
		if(s->read(szBuffer,length))
		{
			//if(szBuffer[0] == -2 && szBuffer[1] == -1)
			//{
			//	short_t* szW = new short_t[length+2];
			//	memset(szW, 0, s->size()+2);
			//	s->seek(2, SEEK_SET);
			//	s->read(szW, s->size());
			//	int len = ConvertAB(szW);
			//	delete[] szBuffer;
			//	szBuffer = new char[s->size() * 2 + 2];
			//	length = Phantom::WToUtf8(szW, szBuffer, s->size() * 2+2);
			//	szBuffer[length] = 0;
			//	//delete[] szBuffer;
			//	//close_s(&s);
			//	//return false;
			//}
			bret = readFromMemory(szBuffer,length,chSymbol);
			if(!bret)
			{
			}
			else
			{
			}
		}
		delete[] szBuffer;
		close_s(&s);
		return bret;
	}

	TableTxt*		loadTable_s(const char* szTable,char chSymbol)		//读取表格文件
	{
		InitMyFile(szTable, true, false);
		TableTxt* tb = new TableTxt();
		if(tb->Load(szTable, chSymbol))
			return tb;
		delete tb;
		return 0;
	}
	LPCSTR				ITable::id_t/*表格数据_读文本根据ID*/(INT id/*ID值*/, LPCSTR szHeadName/*列标题*/)
	{CPUTime(ITable);
		if(!szHeadName)return "";
		int idCol = GetIDCol();
		if(idCol < 0)
		{
			LogInfo("system->warning:%s->id_t(%d,%s)%s",GetTableName(),id,szHeadName, utf8("调用表格数据错误"));
			return "";
		}
		int findCol = SearchCol(szHeadName);
		if(findCol < 0)
		{
			LogInfo("system->warning:%s->id_t(%d,%s)%s(%s)",GetTableName(),id,szHeadName, utf8("调用表格数据错误_表头不存在"), szHeadName);
			return "";
		}
		int numLine = GetLineCount();
		for(int i=0;i<numLine;i++)
		{
			if(GetIntVal(i, idCol) == id)
				return GetTextVal(i, findCol);
		}
		return "";
	}
	INT					ITable::id_i/*表格数据_读整数根据ID*/(INT id/*ID值*/, LPCSTR szHeadName/*列标题*/)
	{CPUTime(ITable);
		if(!szHeadName)return 0;
		int idCol = GetIDCol();
		if(idCol < 0)
		{
			LogInfo("system->warning:%s->id_i(%d,%s)%s",GetTableName(),id,szHeadName, utf8("调用表格数据错误"));
			return 0;
		}
		int findCol = SearchCol(szHeadName);
		if(findCol < 0)
		{
			LogInfo("system->warning:%s->id_i(%d,%s)%s(%s)",GetTableName(),id,szHeadName, utf8("调用表格数据错误_表头不存在"), szHeadName);
			return 0;
		}
		int numLine = GetLineCount();
		for(int i=0;i<numLine;i++)
		{
			if(GetIntVal(i, idCol) == id)
				return GetIntVal(i, findCol);
		}
		return 0;
	}
	float				ITable::id_f/*表格数据_读小数根据ID*/(INT id/*ID值*/, LPCSTR szHeadName/*列标题*/)
	{CPUTime(ITable);
		if(!szHeadName)return 0;
		int idCol = GetIDCol();
		if(idCol < 0)
		{
			LogInfo("system->warning:%s->id_f(%d,%s)%s",GetTableName(),id,szHeadName, utf8("调用表格数据错误"));
			return 0;
		}
		int findCol = SearchCol(szHeadName);
		if(findCol < 0)
		{
			LogInfo("system->warning:%s->id_f(%d,%s)%s(%s)",GetTableName(),id,szHeadName, utf8("调用表格数据错误_表头不存在"), szHeadName);
			return 0;
		}
		int numLine = GetLineCount();
		for(int i=0;i<numLine;i++)
		{
			if(GetIntVal(i, idCol) == id)
				return GetDoubleVal(i, findCol);
		}
		return 0;
	}
	LPCSTR				ITable::name_t/*表格数据_读文本根据Name*/(LPCSTR szName/*ID值*/, LPCSTR szHeadName/*列标题*/)
	{CPUTime(ITable);
		if(!szHeadName)return "";
		int idCol = GetNameCol();
		if(idCol < 0)
		{
			LogInfo("system->warning:%s->name_t(%s,%s)%s",GetTableName(),szName,szHeadName, utf8("调用表格数据错误"));
			return "";
		}
		int findCol = SearchCol(szHeadName);
		if(findCol < 0)
		{
			LogInfo("system->warning:%s->name_t(%s,%s)%s(%s)",GetTableName(),szName,szHeadName, utf8("调用表格数据错误_表头不存在"), szHeadName);
			return "";
		}
		int numLine = GetLineCount();
		for(int i=0;i<numLine;i++)
		{
			if(stricmp(GetTextVal(i, idCol), szName) == 0)
				return GetTextVal(i, findCol);
		}
		return "";
	}
	INT					ITable::name_i/*表格数据_读整数根据Name*/(LPCSTR szName/*ID值*/, LPCSTR szHeadName/*列标题*/)
	{CPUTime(ITable);
		if(!szHeadName|| !szName)return 0;
		int idCol = GetNameCol();
		if(idCol < 0)
		{
			LogInfo("system->warning:%s->name_i(%s,%s)%s",GetTableName(),szName,szHeadName, utf8("调用表格数据错误"));
			return 0;
		}
		int findCol = SearchCol(szHeadName);
		if(findCol < 0)
		{
			LogInfo("system->warning:%s->name_i(%s,%s)%s(%s)",GetTableName(),szName,szHeadName, utf8("调用表格数据错误_表头不存在"), szHeadName);
			return 0;
		}
		int numLine = GetLineCount();
		for(int i=0;i<numLine;i++)
		{
			if(stricmp(GetTextVal(i, idCol), szName) == 0)
				return GetIntVal(i, findCol);
		}
		return 0;
	}
	float				ITable::name_f/*表格数据_读小数根据Name*/(LPCSTR szName/*ID值*/, LPCSTR szHeadName/*列标题*/)
	{CPUTime(ITable);
		if(!szHeadName || !szName)return 0;
		int idCol = GetNameCol();
		if(idCol < 0)
		{
			LogInfo("system->warning:%s->name_f(%s,%s)%s",GetTableName(),szName,szHeadName, utf8("调用表格数据错误"));
			return 0;
		}
		int findCol = SearchCol(szHeadName);
		if(findCol < 0)
		{
			LogInfo("system->warning:%s->name_f(%s,%s)%s(%s)",GetTableName(),szName,szHeadName, utf8("调用表格数据错误_表头不存在"), szHeadName);
			return 0;
		}
		int numLine = GetLineCount();
		for(int i=0;i<numLine;i++)
		{
			if(stricmp(GetTextVal(i, idCol), szName) == 0)
				return GetDoubleVal(i, findCol);
		}
		return 0;
	}
	LPCSTR				ITable::search_t/*表格数据_读文本根据搜索*/(LPCSTR szFindHeader/*返回列名*/, LPCSTR szHeadName/*列标题*/, LPCSTR szLineValue/*行值*/)
	{CPUTime(ITable);
		if(!szHeadName || !szFindHeader || !szLineValue)return "";
		int idCol = SearchCol(szHeadName);
		if(idCol < 0)
		{
			LogInfo("system->warning:%s->search_t(%s,%s,%s)%s",GetTableName(),szFindHeader,szHeadName,szLineValue, utf8("调用表格数据错误"));
			return "";
		}
		int findCol = SearchCol(szFindHeader);
		if(findCol < 0)
		{
			LogInfo("system->warning:%s->search_t(%s,%s,%s)%s(%s)",GetTableName(),szFindHeader,szHeadName,szLineValue, utf8("调用表格数据错误_表头不存在"), szHeadName);
			return "";
		}
		int numLine = GetLineCount();
		for(int i=0;i<numLine;i++)
		{
			if(stricmp(GetTextVal(i, idCol), szLineValue) == 0)
				return GetTextVal(i, findCol);
		}
		return "";
	}
	INT					ITable::search_i/*表格数据_读整数根据搜索*/(LPCSTR szFindHeader/*返回列名*/, LPCSTR szHeadName/*列标题*/, LPCSTR szLineValue/*行值*/)
	{CPUTime(ITable);
		if(!szHeadName || !szFindHeader || !szLineValue)return 0;
		int idCol = SearchCol(szHeadName);
		if(idCol < 0)
		{
			LogInfo("system->warning:%s->search_i(%s,%s,%s)%s",GetTableName(),szFindHeader,szHeadName,szLineValue, utf8("调用表格数据错误"));
			return 0;
		}
		int findCol = SearchCol(szFindHeader);
		if(findCol < 0)
		{
			LogInfo("system->warning:%s->search_i(%s,%s,%s)%s(%s)",GetTableName(),szFindHeader,szHeadName,szLineValue, utf8("调用表格数据错误_表头不存在"), szHeadName);
			return 0;
		}
		int numLine = GetLineCount();
		for(int i=0;i<numLine;i++)
		{
			if(stricmp(GetTextVal(i, idCol), szLineValue) == 0)
				return GetIntVal(i, findCol);
		}
		return 0;
	}
	float				ITable::search_f/*表格数据_读小数根据搜索*/(LPCSTR szFindHeader/*返回列名*/, LPCSTR szHeadName/*列标题*/, LPCSTR szLineValue/*行值*/)
	{CPUTime(ITable);
		if(!szHeadName || !szFindHeader || !szLineValue)return 0;
		int idCol = SearchCol(szHeadName);
		if(idCol < 0)
		{
			LogInfo("system->warning:%s->search_f(%s,%s,%s)%s",GetTableName(),szFindHeader,szHeadName,szLineValue, utf8("调用表格数据错误"));
			return 0;
		}
		int findCol = SearchCol(szFindHeader);
		if(findCol < 0)
		{
			LogInfo("system->warning:%s->search_f(%s,%s,%s)%s(%s)",GetTableName(),szFindHeader,szHeadName,szLineValue, utf8("调用表格数据错误_表头不存在"), szHeadName);
			return 0;
		}
		int numLine = GetLineCount();
		for(int i=0;i<numLine;i++)
		{
			if(stricmp(GetTextVal(i, idCol), szLineValue) == 0)
				return GetDoubleVal(i, findCol);
		}
		return 0;
	}

	ZipFileFolder::ZipFileFolder()
	{
		directoryName[0]	=	0;
		parentDir			=	0;
	}

	void									ZipFileFolder::destroy()
	{CPUTime(ZipFileFolder);
		for(size_t i=0;i<directorys.size();i++)
			delete directorys.at(i);
		directorys.clear();
		files.clear();
	}

	ZipFileFolder::~ZipFileFolder()
	{
		destroy();
	}

	ZipFileFolder*								ZipFileFolder::findFolder(const char* szFolder)
	{CPUTime(ZipFileFolder);
		std::vector<std::string>	divs;
		std::string					file;
		str_divs(szFolder, divs, file);
		if(divs.size() == 0)
			return this;
		if(!file.empty())
			return NULL;
		return findFolder_loop(divs);
	}

	PFileList*					ZipFileFolder::enumFiles(const char* szPath, BOOL bsubFolder, std::string* szFindTypes, int numFindTypes)
	{CPUTime(ZipFileFolder);
		ZipFileFolder* folder = 0;
		if(szPath)
		{
			folder	=	findFolder(szPath);
			if(!folder)
				return NULL;
		}
		else
			folder	=	this;
		PFileList* f	=	new PFileList();
		folder->enumFiles(f, bsubFolder, szFindTypes, numFindTypes);
		return f;
	}

	void							ZipFileFolder::enumFiles(PFileList* list, BOOL bsubFolder, std::string* szFindTypes,int numFindTypes)
	{CPUTime(ZipFileFolder);
		if(bsubFolder)
		{
			for(size_t i=0;i<directorys.size();i++)
			{
				directorys.at(i)->enumFiles(list, bsubFolder, szFindTypes, numFindTypes);
			}
		}
		for(size_t i=0;i<files.size();i++)
		{
			for(int j=0;j<numFindTypes;j++)
			{
				if(stricmp(files.at(i).ext, szFindTypes[j].c_str()) == 0)
				{
					FileName n;
					n	=	files.at(i).zipFile;
					list->vFiles.push_back( n );
					break;
				}
			}
		}
	}

	ZipFileFolder*						ZipFileFolder::findFolder_loop(std::vector<std::string>& divs)
	{CPUTime(ZipFileFolder);
		if(divs.size() > 0)
		{
			std::string szDir	=	divs.at(0);
			divs.erase(divs.begin());
			if(stricmp(szDir.c_str(), "..") == 0)
			{
				if(!parentDir)
					return NULL;
				if(divs.size() == 0)
					return parentDir;
				return parentDir->findFolder_loop( divs );
			}
			else
			{
				for(size_t i=0;i<directorys.size();i++)
				{
					if(stricmp(directorys.at(i)->directoryName, szDir.c_str()) == 0)
					{
						if(divs.size() == 0)
							return directorys.at(i);
						else
							return directorys.at(i)->findFolder_loop( divs );
					}
				}
				return NULL;
			}
		}
		return NULL;
	}

	const ZipFileItem*						ZipFileFolder::find(const char* szFilePath)
	{CPUTime(ZipFileFolder);
		std::vector<std::string>	divs;
		std::string					file;
		str_divs(szFilePath, divs, file);
		return find_loop(divs, file);
	}

	const ZipFileItem*						ZipFileFolder::find_loop(std::vector<std::string>& divs, std::string& file)
	{CPUTime(ZipFileFolder);
		if(divs.size() > 0)
		{
			std::string szDir	=	divs.at(0);
			divs.erase(divs.begin());
			if(stricmp(szDir.c_str(), "..") == 0)
			{
				if(!parentDir)
					return NULL;
				return parentDir->find_loop(divs, file);
			}
			else
			{
				for(size_t i=0;i<directorys.size();i++)
				{
					if(stricmp(directorys.at(i)->directoryName, szDir.c_str()) == 0)
						return directorys.at(i)->find_loop(divs, file);
				}
				return NULL;
			}
		}
		for(size_t i=0;i<files.size();i++)
		{
			if(stricmp(files.at(i).fileName, file.c_str()) == 0)
				return &files.at(i);
		}
		return NULL;
	}

	BOOL									ZipFileFolder::add(const char* szFilePath, int zipIndex)
	{CPUTime(ZipFileFolder);
		ZipFileItem item;
		item.zipFile		=	szFilePath;
		item.zipIndex		=	zipIndex;
		std::vector<std::string>	divs;
		std::string					file;
		str_divs(szFilePath, divs, file);
		return add_loop(item, divs, file);
	}

	BOOL									ZipFileFolder::add_loop(const ZipFileItem& f, std::vector<std::string>& divs, std::string& file)
	{CPUTime(ZipFileFolder);
		if(divs.size() > 0)
		{
			std::string szDir	=	divs.at(0);
			divs.erase(divs.begin());
			if(stricmp(szDir.c_str(), "..") == 0)
			{
				if(!parentDir)
					return false;
				return parentDir->add_loop(f, divs, file);
			}
			else
			{
				for(size_t i=0;i<directorys.size();i++)
				{
					if(stricmp(directorys.at(i)->directoryName, szDir.c_str()) == 0)
						return directorys.at(i)->add_loop(f, divs, file);
				}
				ZipFileFolder* dir	=	new ZipFileFolder();
				dir->parentDir		=	this;
				strcpy(dir->directoryName, szDir.c_str());
				directorys.push_back( dir );
				return dir->add_loop(f, divs, file);
			}
		}
		if(!file.empty())
		{
			ZipFileItem item;
			item.fileName		=	file.c_str();
			item.zipFile		=	f.zipFile;
			item.fileName.splitpath(0, 0, &item.ext);
			item.zipIndex		=	f.zipIndex;
			files.push_back( item );
		}
		return true;
	}


	void									ZipFileFolder::str_divs(const char* szDir, std::vector<std::string>& divsRet, std::string& retFileName)
	{CPUTime(ZipFileFolder);
		char buffer[256];
		int len		=	strlen(szDir);
		int begin	=	0;
		for(int i=0;i<=len;i++)
		{
			if(szDir[i] == '\\' || szDir[i] == '/' || szDir[i] == 0)
			{
				int nLength	=	i - begin;
				memcpy(buffer, szDir + begin, i - begin);
				buffer[nLength]	=	0;
				begin			=	i + 1;
				if(!buffer[0])
					continue;
				if(szDir[i] == 0)
				{
					retFileName	=	buffer;
					continue;
				}
				if(buffer[0] == '.' && buffer[1] == 0)
					continue;
				strlwr_t(buffer);
				divsRet.push_back( buffer );
			}
		}
	}

	//zip files
	void		ZipFilePackage::enumFiles(PFileList* f, const char* path, BOOL bsubFolder, std::string* szFindTypes,int numFindTypes)
	{CPUTime(ZipFilePackage);
		if(this->m_curFolder)
		{
			ZipFileFolder* folder = 0;
			if(path)
			{
				folder	=	m_curFolder->findFolder(path);
				if(!folder)
					return;
			}
			else
				folder	=	m_curFolder;
			folder->enumFiles(f, bsubFolder, szFindTypes, numFindTypes);
		}
	}

	PFileList*	ZipFilePackage::enumFiles(const char* path, BOOL bsubFolder, std::string* szFindTypes,int numFindTypes)
	{CPUTime(ZipFilePackage);
		PFileList* f	=	0;
		for(int z=0;z<m_initDirs.size();z++){
			LFileName file = m_initDirs[z].t;
			file	+=	path;
			if(this->m_curFolder)
				f	=	m_curFolder->enumFiles(file, bsubFolder, szFindTypes, numFindTypes);
			int	fLen	=	strlen(m_initDirs[z].t);
			if(fLen > 0 && f)
			{
				for(int i=0;i<f->vFiles.size();i++)
				{
					if(strnicmp(f->vFiles[i].t, m_initDirs[z].t, fLen) == 0)
						memmove_s(f->vFiles[i].t, f->vFiles[i].t, fLen);
				}
			}
			if(f)
				return f;
		}
		return f;
	}
	BOOL		ZipFilePackage::fexist_local(const char* _file)
	{CPUTime(ZipFilePackage);
		const ZipFileItem* p = m_curFolder->find(_file);
		if(!p)
		{
			for(int z=0;z<m_initDirs.size();z++){
				LFileName lf = m_initDirs[z].t;
				lf += _file;
				p = m_curFolder->find(lf);
				if(p)
					return true;
			}
			return false;
		}
		return true;
	}
	BOOL		ZipFilePackage::fexist(const char* _file)
	{CPUTime(ZipFilePackage);
		if(!m_curFolder)
			return false;
		if(fexist_local(_file))
			return true;
		LFileName ft = _file;
		LFileName dir, file, ext;
		ft.setIsFile();
		ft.splitpath(&dir, &file, &ext);
		BOOL bImage = false;
		std::vector<ExtFile>& exts = GetExtCount();
		for(int i=0;i<exts.size();i++)
		{
			if(ext == exts[i].tori.t)
			{
				dir += file;
				dir += exts[i].tnew.t;
				if(fexist_local(dir.t))//如果存在image文件
					return true;
				break;
			}
		}
		return false;
	}

	class CWriteMemory: public IUnzipFile
	{
	public:
		VOID					Append(const char* ptr, int length)
		{
			if((m_length + length) >= m_allocLength)
			{
				m_allocLength	+=	length + 1024 * 256;
				if(m_buffer)
					m_buffer	=	(char*)realloc_s(m_buffer, m_allocLength);
				else
					m_buffer	=	(char*)malloc_s(m_allocLength);
			}
			memcpy_s(m_buffer + m_length, ptr, length);
			m_length			+=	length;
		}
		VOID					BeginWrite(){m_length = 0;}
		CWriteMemory(){m_buffer = 0; m_length = 0; m_allocLength = 0;}
		~CWriteMemory()
		{
			if(m_buffer)free_s(m_buffer);
		}
	public:
		char*					m_buffer;
		int						m_length;
		int						m_allocLength;
	};

	ZipResult		ZipFilePackage::openZip(const char* buffer, int bufferLength, const char* szDir, const char* szPassword)
	{CPUTime(ZipFilePackage);
		if(!szPassword || !szPassword[0])
			szPassword		=	0;
		m_bIsFile			=	FALSE;
		m_hZipFile			=	(void*)OpenZip((void*)buffer, bufferLength, szPassword);
		if(!m_hZipFile)
			return ZipResult_Error;
		m_zipFolder.destroy();
		std::vector<ZIPENTRY> files;
		::EnumZipFiles((HZIP)m_hZipFile, files);
		//
		CWriteMemory m;
		for(size_t i=0;i<m_fileBuffer.size();i++)
		{
			if(m_fileBuffer.at(i).buffer)
				free(m_fileBuffer.at(i).buffer);
		}
		m_fileBuffer.clear();
		Name initDir = "";
		if(szDir && szDir[0])
			initDir	=	szDir;
		if(initDir.t[0])
		{
			int len = initDir.size() - 1;
			if(initDir.t[len] != '/')
				initDir	+=	'/';
		}
		this->m_initDirs.clear();
		m_initDirs.push_back(initDir);
		//
		for(size_t i=0;i<files.size();i++)
		{
			const char* szName	=	files[i].name;
			int nameLen	=	strlen(szName);
			if(!szName || szName[nameLen - 1] == '/' || szName[nameLen - 1] == '\\')
				continue;
			m.BeginWrite();
			ZRESULT zr	=	UnzipItem((HZIP)m_hZipFile, i, &m);

			if(zr != ZR_OK)
			{
				if(zr == ZR_PASSWORD)
					return ZipResult_RequestPassword;
				return ZipResult_Error;
			}
			if(m.m_length == 0)
				continue;
			std::string strPath;
			strPath				+=	files.at(i).name;
			m_zipFolder.add(strPath.c_str(), i);
			//
			ZipFileBuffer buf;
			buf.buffer			=	(char*)malloc_s(m.m_length);
			buf.index			=	i;
			memcpy_s(buf.buffer, m.m_buffer, m.m_length);
			buf.bufferLength	=	m.m_length;
			m_fileBuffer.push_back( buf );
		}
		closeZip();
		//
		m_curFolder			=	&m_zipFolder;
		return ZipResult_OK;
	}

	StreamBase*	ZipFilePackage::open(const char* szFile)		//从map中打开文件
	{CPUTime(ZipFilePackage);
		if(!this->m_curFolder)
			return NULL;
		if(m_bIsFile)
		{
			LFileName ft = szFile;
			LFileName dir, file, ext;
			ft.splitpath(&dir, &file, &ext);
			if(!this->m_hZipFile)
				return NULL;
			const ZipFileItem* p = m_curFolder->find(szFile);
			if(!p)
			{
				//LogInfo("ZipFileSearchA(%s)", szFile);
				for(int z=0;z<m_initDirs.size();z++){
					LFileName lf = m_initDirs[z].t;
					lf += szFile;
					//LogInfo("ZipFileSearch(%s)", lf.t);
					p = m_curFolder->find(lf);
					if(p)
						break;
				}
				if(!p)
					return NULL;
			}
			//LogInfo("zipfileopen(%s,%d)", szFile, p);
			CWriteMemory m;
			m.BeginWrite();
			//
			UnzipItem((HZIP)m_hZipFile, p->zipIndex, &m);
			if(m.m_length == 0)
			{
				LogError("m_length==0(%s,%d)", szFile, p);
				return NULL;
			}
			//
			MemoryFile* mem = new MemoryFile();
			if(!mem->create(m.m_length+1))
			{
				delete mem;
				LogError("create==0(%s,%d)", szFile, p);
				return NULL;
			}
			int newsize = 0;
			int begin = 0;
			unsigned char* beginPtr = DecryptMemoryFile(ext, (unsigned char*)m.m_buffer, m.m_length, newsize, begin);
			if(!beginPtr)
			{
				LogError("beginPtr==0(%s,%d)", szFile, p);
				delete mem;
				return 0;
			}
			LogInfo("Unzip(%s):%d/%d", szFile, m.m_length, newsize);
			//beginPtr[newsize] = 0;
			mem->create(newsize + 1);
			memcpy_s(mem->getMemoryPtr(), beginPtr + begin, newsize+1);
			if((unsigned char*)m.m_buffer != beginPtr)//如果重新分配了数据
			{
				delete[] beginPtr;
			}
			mem->getMemoryPtr()[newsize] = 0;
			mem->m_nFileSize = newsize;
			//LogInfo("zipfileopenok(%s,%d)", szFile, p);
			return mem;
		}
		else
		{
			const ZipFileItem* p = m_curFolder->find(szFile);
			if(!p)
				return NULL;
			for(size_t i=0;i<this->m_fileBuffer.size();i++)
			{
				if(m_fileBuffer.at(i).index == p->zipIndex)
				{
					MemoryFile* mem = new MemoryFile();
					if(mem->open(m_fileBuffer.at(i).buffer, m_fileBuffer.at(i).buffer, m_fileBuffer.at(i).bufferLength, false))
						return mem;
					delete mem;
					return NULL;
				}
			}
			return NULL;
		}
	}

	std::vector<ZipFilePackage*>			ZipFilePackage::s_zipFiles;
	VOID								CloseAllZip()
	{
		std::vector<ZipFilePackage*>			zipFiles = ZipFilePackage::s_zipFiles;
		ZipFilePackage::s_zipFiles.clear();
		for(int i=0;i<zipFiles.size();i++)
			delete zipFiles[i];
	}
	ZipFilePackage*							open_zip_mem(const char* szData, int length, const char* szFirstDir)
	{
		ZipFilePackage* p = new ZipFilePackage();
		if(p->openZip(szData, length, szFirstDir, 0) != ZipResult_OK)
		{
			delete p;
			return 0;
		}
		return p;
	}
	ZipFilePackage*							open_zip(const char* szZipFile, const char* szFirstDir)
	{
		LFileName z;
		z = szZipFile;
		z.setIsFile();
		for(int i=0;i<ZipFilePackage::s_zipFiles.size();i++)
		{
			if(ZipFilePackage::s_zipFiles[i]->m_zipFileName.isFile(z))
			{
				delete ZipFilePackage::s_zipFiles[i];
				break;
			}
		}
		//StreamBase* pFile	=	open_s(szZipFile, true);
		ZipFilePackage* p = new ZipFilePackage();
		if(p->openZip(szZipFile, szFirstDir, 0) != ZipResult_OK)
		{
			//close_s(&pFile);
			delete p;
			return 0;
		}
		//close_s(&pFile);
		return p;
	}
	VOID								close_zip(ZipFilePackage* szZipFile)
	{
		for(int i=0;i<ZipFilePackage::s_zipFiles.size();i++)
		{
			if(ZipFilePackage::s_zipFiles[i] == szZipFile)
			{
				delete ZipFilePackage::s_zipFiles[i];
				return;
			}
		}
	}
	VOID								close_zip(const char* szZipFile)
	{
		LFileName z;
		z = szZipFile;
		z.setIsFile();
		for(int i=0;i<ZipFilePackage::s_zipFiles.size();i++)
		{
			if(ZipFilePackage::s_zipFiles[i]->m_zipFileName.isFile(z))
			{
				delete ZipFilePackage::s_zipFiles[i];
				return;
			}
		}
	}

	ZipResult		ZipFilePackage::openZip(const char* szFile, const char* szDir, const char* szPassword)
	{CPUTime(ZipFilePackage);
		closeZip();
		m_zipFileName			=	szFile;
		m_zipFileName.setIsFile();
		if(!szPassword || !szPassword[0])
			szPassword		=	0;
		m_bIsFile			=	TRUE;
		m_hZipFile			=	(void*)OpenZip(szFile, szPassword);
		if(!m_hZipFile)
			return ZipResult_Error;
		m_zipFolder.destroy();
		std::vector<ZIPENTRY> files;
		::EnumZipFiles((HZIP)m_hZipFile, files);
		//
		//if(files.size() > 0)
		//{
		//	static CWriteMemory m;
		//	m.BeginWrite();
		//	ZRESULT zr = UnzipItem(m_hZipFile, 0, &m);
		//	if(zr != ZR_OK)
		//	{
		//		if(zr == ZR_PASSWORD)
		//			return ZipResult_RequestPassword;
		//		return ZipResult_Error;
		//	}
		//}
		for(size_t i=0;i<m_fileBuffer.size();i++)
		{
			if(m_fileBuffer.at(i).buffer)
				free(m_fileBuffer.at(i).buffer);
		}
		m_fileBuffer.clear();
		//
		Name initDir = "";
		if(szDir)
			initDir = szDir;
		if(initDir.t[0])
		{
			int len = initDir.size() - 1;
			if(initDir.t[len] != '/')
				initDir	+=	'/';
		}
		this->m_initDirs.clear();
		m_initDirs.push_back(initDir);
		for(size_t i=0;i<files.size();i++)
		{
			std::string strPath;
			strPath				+=	files.at(i).name;
			m_zipFolder.add(strPath.c_str(), i);
		}
		//
		m_curFolder			=	&m_zipFolder;
		return ZipResult_OK;
	}
	void		ZipFilePackage::addDir(const char* szDir)
	{
		Name initDir = "";
		if(szDir)
			initDir = szDir;
		if(initDir.t[0])
		{
			int len = initDir.size() - 1;
			if(initDir.t[len] != '/')
				initDir	+=	'/';
		}
		m_initDirs.push_back(initDir);
	}
	void		ZipFilePackage::closeZip()
	{CPUTime(ZipFilePackage);
		m_bIsFile	=	FALSE;
		if((HZIP)m_hZipFile)
			CloseZip((HZIP)m_hZipFile);	//zip内部自动释放内存
		m_memoryPtr			=	0;
		m_memoryLength		=	0;
		m_hZipFile			=	0;
	}

	ZipFilePackage::ZipFilePackage()
	{
		m_bIsFile			=	FALSE;
		m_curFolder			=	0;
		m_hZipFile			=	0;
		m_memoryPtr			=	0;
		m_memoryLength		=	0;
		s_zipFiles.push_back( this );
	}

	ZipFilePackage::~ZipFilePackage()
	{
		for(size_t i=0;i<s_zipFiles.size();i++)
		{
			if(s_zipFiles.at(i) == this)
			{
				s_zipFiles.erase(s_zipFiles.begin() + i);
				break;
			}
		}
		for(size_t i=0;i<m_fileBuffer.size();i++)
		{
			if(m_fileBuffer.at(i).buffer)
				free(m_fileBuffer.at(i).buffer);
		}
		m_fileBuffer.clear();
		closeZip();
	}

	BOOL				LoadUtf8(const char* szFile, CDynamicArray<char>& ret)
	{
		StreamBase* sb = open_s(szFile);
		if(!sb)return false;
		unsigned char head[3];
		sb->read(head, 3);
		if(!(head[0] == 0xef && head[1] == 0xbb && head[2] == 0xbf))
			return false;
		ret.resize(sb->size()-2);
		sb->read(ret.GetPtr(), sb->size()-3);
		close_s(&sb);
		return true;
	}
	VOID				SaveUtf8(const char* szFile, const char* text)
	{
		StreamBase* sb = create_s(szFile);
		if(!sb)return;
		unsigned char head[3];
		head[0] = 0xef;
		head[1] = 0xbb;
		head[2] = 0xbf;
		sb->write(head, 3);
		sb->write(text, strlen(text));
		close_s(&sb);
	}
	PakFileData*	LoadFileData(const char* szFile, BOOL bCompress){
#ifdef EXPORT_ENGINE
		return 0;
#else
		PakFileData* data = new PakFileData();
		data->file = szFile;
		if(!open_s_full(szFile, data->buffers)||data->buffers.size()==0)
		{
			delete data;
			return 0;
		}
		data->length = data->buffers.size();
		data->bCompress = false;
		if(bCompress&&data->buffers.size()>1024){
			CDynamicArray<char> temp = data->buffers;
			data->buffers.resize(data->buffers.size()+1024*10);
			uLongf len = data->buffers.size();
			if(compress((Bytef*)data->buffers.GetPtr(), &len, (Bytef*)temp.GetPtr(), temp.size())==Z_OK){
				data->bCompress = true;
				data->buffers.resize(len);
			}
			else
				data->buffers = temp;
		}
		return data;
#endif
	}
	PakDirectory::PakDirectory()
	{
		m_streamPak = 0;
	}
	PakDirectory::~PakDirectory()
	{
		ClosePak();
	}
	BOOL				PakDirectory::fexist(const char* szFile)
	{
		FileName ftex = szFile;
		ftex.setIsFile();
		FileName fdir, ffile;
		ftex.splitpath(&fdir, &ffile, 0);
		fdir += ffile;
		for(int i=0;i<this->m_pakfiles.size();i++){
			if(m_pakfiles[i].bIsImage){
				FileName dir, file;
				m_pakfiles[i].lFile.splitpath(&dir, &file, 0);
				dir += file;
				if(fdir==dir){
					return true;
				}
			}
			else if(m_pakfiles[i].lFile == ftex)
				return true;
		}
		return false;
	}
	BOOL	IsExistFile(CDynamicArray<PakFileData*>& files, const char* szFile)
	{//查找文件,需要使用默认目录的形式再次搜索
		if(files.size()<=1)return 0;
		FileName defpath;
		files[0]->file.splitpath(&defpath, 0, 0);
		//
		FileName ftex = szFile;
		ftex.setIsFile();
		FileName fdir, ffile,fext;
		ftex.splitpath(&fdir, &ffile, &fext);
		fdir += ffile;
		defpath += ffile.t;
		FileName defpathi;
		defpathi = defpath.t;
		defpath += fext;
		for(int i=0;i<files.size();i++){
			if(files[i]->bIsImage){
				FileName dir, file;
				files[i]->file.splitpath(&dir, &file, 0);
				dir += file;
				if(fdir==dir)
					return true;
				else if(dir == defpathi)
					return true;
			}
			else if(files[i]->file == ftex)
				return true;
			else if(files[i]->file == defpath)
				return true;
		}
		return false;
	}
	StreamBase*			PakDirectory::open(const char* szFile)
	{//查找文件,需要使用默认目录的形式再次搜索
		if(m_pakfiles.size()==0)return 0;
		FileName defpath;
		m_pakfiles[0].lFile.splitpath(&defpath, 0, 0);
		//
		FileName ftex = szFile;
		ftex.setIsFile();
		FileName fdir, ffile,fext;
		ftex.splitpath(&fdir, &ffile, &fext);
		fdir += ffile;
		defpath += ffile.t;
		FileName defpathi;
		defpathi = defpath.t;
		defpath += fext;
		for(int i=0;i<this->m_pakfiles.size();i++){
			if(m_pakfiles[i].bIsImage){
				FileName dir, file;
				m_pakfiles[i].lFile.splitpath(&dir, &file, 0);
				dir += file;
				if(fdir==dir)
					return OpenPakFile(i);
				else if(dir == defpathi)
					return OpenPakFile(i);
			}
			else if(m_pakfiles[i].lFile == ftex)
				return OpenPakFile(i);
			else if(m_pakfiles[i].lFile == defpath)
				return OpenPakFile(i);
		}
		return 0;
	}
#define	PAK_FILE_CHK		0x168ab158
#define	PAK_FILE_HEADER		_MakeFileHead('p','a','k','1')
	BOOL				PakDirectory::IsPakFile(StreamBase* file)
	{
		PakFileHeader h;
		file->read(&h, sizeof(PakFileHeader));
		file->seek(0);
		if(h.chk != PAK_FILE_CHK)
			return false;
		if(h.head != PAK_FILE_HEADER)
			return false;
		return true;
	}
	char				PakDirectory::ExportPak(const char* szFile, CDynamicArray<PakFileData*>& files)//, const unsigned char* pass, int passlen)
	{
		CDynamicArray<PakFileInfo> infos;
		infos.resize(files.size());
		for(int i=0;i<files.size();i++){
			PakFileInfo& info = infos[i];
			memset(&info, 0, sizeof(PakFileInfo));
			PakFileData* data = files[i];
			info.lFile = data->file.t;
			info.lFile.setIsFile();
			info.bCompress	=	data->bCompress;
			info.bIsImage	=	data->bIsImage;
			info.length = data->length;
			info.compressLen = data->buffers.size();
			int len = data->buffers.size();
			if(len > PAK_FILE_HEAD_LEN)
				len = PAK_FILE_HEAD_LEN;
			memcpy(info.filehead, data->buffers.GetPtr(), len);
			for(int k=0;k<PAK_FILE_HEAD_LEN;k++)
				info.nadd += info.filehead[k];
			//if(pass){
			//	info.bPassword = true;
			//	for(int k=0;k<len;k++){
			//		info.filehead[k] = info.filehead[k] ^ pass[k%passlen];
			//	}
			//}
		}
		CSafeFileHelperW w;
		if(!w.create( szFile ))
			return false;
		PakFileHeader h;
		h.head = PAK_FILE_HEADER;
		h.chk = PAK_FILE_CHK;
		h.infobegin = 0;
		h.filecount = infos.size();
		w.pStream->write(&h, sizeof(PakFileHeader));
		char version = 1;
		w << version;
		int seek = w.pStream->tell();
		w.pStream->write(infos.GetPtr(), infos.size() * sizeof(PakFileInfo));
		for(int i=0;i<files.size();i++){
			PakFileData* data = files[i];
			int len = data->buffers.size();
			if(len > PAK_FILE_HEAD_LEN){
				infos[i].fbegin = w.pStream->tell();
				w.pStream->write(data->buffers.GetPtr() + PAK_FILE_HEAD_LEN, data->buffers.size()-PAK_FILE_HEAD_LEN);
			}
		}
		int snew = w.pStream->tell();
		w.pStream->seek(seek);
		w.pStream->write(infos.GetPtr(), infos.size() * sizeof(PakFileInfo));
		w.pStream->seek(0);
		h.infobegin = seek;
		w.pStream->write(&h, sizeof(PakFileHeader));
		return true;
	}
	BOOL				PakDirectory::OpenPak(CSafeFileHelperR& r, CSafeFileHelperR& ret)
	{
		PakFileHeader h;
		r.pStream->read(&h, sizeof(PakFileHeader));
		if(h.chk != PAK_FILE_CHK)
			return false;
		if(h.head != PAK_FILE_HEADER)
			return false;
		char version = 0;
		r >> version;
		m_pakfiles.resize(h.filecount);
		r.pStream->read(m_pakfiles.GetPtr(), m_pakfiles.size() * sizeof(PakFileInfo));
		m_streamPak = r.pStream;
		r.pStream = 0;
		//
		StreamBase* base = OpenPakFile(0);
		if(!base){
			ClosePak();
			return false;
		}
		ret.pStream = base;
		ret.bReadOnly = true;
		ret.bSelfOpen = true;
		return true;
	}
	BOOL				PakDirectory::OpenPak(const char* szFile, CSafeFileHelperR& ret)
	{
		m_fileName = szFile;
		ClosePak();
		CSafeFileHelperR r;
		if(!r.open(m_fileName))
			return false;
		return OpenPak(r, ret);
	}//设置文件名
	extern	BOOL	GetAuthCode(GenuineGUID* r);
	BOOL				PakDirectory::ResetPassword(CDynamicArray<char>& dest, CDynamicArray<char>& src, const unsigned char* oldpass, int oldpasslen)
	{
		GenuineGUID temp;
		if(!GetAuthCode(&temp))return false;
		int passlen = sizeof(GenuineGUID);
		unsigned char* pass = (unsigned char*)&temp;
		//
		dest = src;
		PakFileHeader& h = *(PakFileHeader*)src.GetPtr();
		if(h.chk != PAK_FILE_CHK)
			return false;
		if(h.head != PAK_FILE_HEADER)
			return false;
		PakFileInfo* infosrc = (PakFileInfo*)(src.GetPtr()+h.infobegin);
		PakFileInfo* infodest = (PakFileInfo*)(dest.GetPtr()+h.infobegin);
		for(int i=0;i<h.filecount;i++){
			PakFileInfo& infos = infosrc[i];
			PakFileInfo& infod = infodest[i];
			int len = infos.compressLen;
			if(len > PAK_FILE_HEAD_LEN)
				len = PAK_FILE_HEAD_LEN;
			if(oldpass&&oldpasslen>0){
				for(int k=0;k<len;k++){
					infod.filehead[k] = infod.filehead[k] ^ oldpass[k%oldpasslen];
				}
				infod.bPassword = false;
			}
			if(pass&&passlen>0){
				infod.bPassword = true;
				for(int k=0;k<len;k++){
					infod.filehead[k] = infod.filehead[k] ^ pass[k%passlen];
				}
			}
		}
		return true;
	}
	extern	BOOL	GetAuthCode(Phantom::GenuineGUID* r);
	StreamBase*			PakDirectory::OpenPakFile(int index)
	{
#ifdef EXPORT_ENGINE
		return 0;
#else
		Phantom::GenuineGUID temp;
		if(!GetAuthCode(&temp))return 0;
		int passlen = sizeof(Phantom::GenuineGUID);
		unsigned char* pass = (unsigned char*)&temp;
		///
		if(index<0||index>=this->m_pakfiles.size())
			return 0;
		PakFileInfo& info = m_pakfiles[index];
		if(!m_streamPak){
			m_streamPak = open_s(m_fileName);
			if(!m_streamPak)
				return 0;
		}
		CSafeFileHelperR r;
		r.pStream = m_streamPak;
		r.bReadOnly = true;
		r.bSelfOpen = false;
		//decrypt password
		int len = info.compressLen;
		if(len > PAK_FILE_HEAD_LEN)
			len = PAK_FILE_HEAD_LEN;
		if(info.bPassword){
			info.bPassword = false;
			for(int k=0;k<len;k++){
				info.filehead[k] = info.filehead[k] ^ pass[k%passlen];
			}
		}
		unsigned int nadd = 0;
		for(int k=0;k<PAK_FILE_HEAD_LEN;k++)
		{
			nadd += info.filehead[k];
		}
		if(nadd != info.nadd)
			return 0;
		CDynamicArray<char> rets;
		rets.resize(info.compressLen);
		int hlen = info.compressLen;
		if(hlen > PAK_FILE_HEAD_LEN)
			hlen = PAK_FILE_HEAD_LEN;
		memcpy(rets.GetPtr(), info.filehead, hlen);
		if(info.fbegin==0||info.compressLen <= PAK_FILE_HEAD_LEN)
		{
		}
		else{
			r.pStream->seek(info.fbegin);
			r.pStream->read(rets.GetPtr()+hlen, info.compressLen-hlen);
		}
		if(info.bCompress){
			CDynamicArray<char> temp = rets;
			rets.resize(info.length);
			uLongf dlen = info.length;
			if(uncompress((Bytef*)rets.GetPtr(), &dlen, (Bytef*)temp.GetPtr(), info.compressLen)!=Z_OK)
				return 0;
		}
		return open_memory(rets.GetPtr(), rets.size());
#endif
	}
	VOID				PakDirectory::ClosePak()
	{
		if(m_streamPak)
			close_s(&m_streamPak);
		m_streamPak = 0;
	}


};

