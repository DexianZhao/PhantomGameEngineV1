/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "GameManager.h"
#ifdef ANDROID_OS
#include <jni.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include "PhantomOS.h"
#endif
#include "PhantomDB.h"
#include "../PhantomEngine/encrypt.h"

FileName	g_documents;
const char*	GetRealDocuments()
{
	return g_documents;
}
namespace PhantomV1{
	extern	VOID	SetPlatformMode(BOOL b);
};
VOID DeviceLogPrint(const char* szText);//输出信息
extern "C"{
	int GetAppID(){return 0;}
    extern BOOL CreatePhantom(int nDiv, int nOS, int* authcode);
	int g_guid[128] = {-881829239,531795469,205672448,133909,-279732572,-40749204,-1544058199,1115756337,-639047129,1877516808,-740304721,1521638536,-1207154134,408159893,-1290536934,-628714630,1312163671,-871496481,210487935,378976542,-920389104,1091360994,1824501840,1353990702,2100465822,41953219,1187414059,-484479293,-61857979,1836278236,1731518070,1000663815,-2145327700,-1893731463,-1491181980,1799716396,1382820141,-1567732596};
    void MyCreatePhantom(int nDiv, int nOS)
    {
        CreatePhantom(nDiv,nOS,g_guid);
    }
    void    saveConfig(const char* key, const char* value);
    int     loadConfig(const char* key,char* retvalue);
    void    SetViewOrientation(int ori);
};
char	g_szProjectDir[256] = {0};
const char*	GetProjectDir(){
	return g_szProjectDir;
}

struct	MyFileInfo{
	char			file[128];
	unsigned int	low,high;
};

CDynamicArray<MyFileInfo>	g_oldHttpFiles;//文件列表，本地
CDynamicArray<MyFileInfo>	g_newHttpFiles;//文件列表，外地
VOID	LoadOldFileInfos()//读取旧的文件数据
{
	int version = 1;
	StreamBase* s = open_s("infos");
	if(!s)return;
	s->read(&version, sizeof(int));
	int num;
	s->read(&num, sizeof(int));
	g_oldHttpFiles.SetArrayCount(num);
	for(int i=0;i<num;i++)
	{
		MyFileInfo info;
		s->read(&info, sizeof(MyFileInfo));
		g_oldHttpFiles[i] = info;
	}
	close_s(&s);
}
VOID	SaveOldFileInfos()//保存旧的文件数据
{
	int version = 1;
	StreamBase* s = create_s("infos");
	if(!s)return;
	s->write(&version, sizeof(int));
	int num = g_oldHttpFiles.size();
	s->write(&num, sizeof(int));
	for(int i=0;i<num;i++)
	{
		s->write(&g_oldHttpFiles[i], sizeof(MyFileInfo));
	}
	close_s(&s);
}
VOID	HttpDownloadOK(const char* requestText, const char* indocFile, BOOL bError, char* buffer, int length)
{
	FileName temp = indocFile;
	temp.setIsFile();
	for(int i=0;i<g_newHttpFiles.size();i++)
	{
		if(stricmp(g_newHttpFiles[i].file, temp.t) == 0)
		{
			BOOL bFind = false;
			BOOL bMustSave = false;
			for(int j=0;j<g_oldHttpFiles.size();j++)
			{
				if(g_oldHttpFiles[j].file == temp)
				{
					if(g_oldHttpFiles[j].low != g_newHttpFiles[i].low || g_oldHttpFiles[j].high != g_newHttpFiles[i].high)
					{
						bFind = true;
						bMustSave = true;
					}
					break;
				}
			}
			if(!bFind)
			{
				g_oldHttpFiles.push_back(g_newHttpFiles[i]);
				bMustSave = true;
			}
			SaveOldFileInfos();
			break;
		}
	}

}
LFileName	g_szHttpPath;
VOID MyHttpDownloadFile(const char* szFile, BOOL bDownloadNow, BOOL bInMemory)
{
	if(!g_szProjectDir[0] && !bInMemory)return;
	FileName temp = szFile;
	temp.setIsFile();
	int len = strlen(temp.t);
	for(int i=0;i<len;i++)
		if(temp.t[i] == '\r' || temp.t[i] == '\n')
			temp.t[i] = 0;
	if(fexist(szFile))
	{
		BOOL bMustDownload = false;
		if(!bInMemory){
			for(int i=0;i<g_newHttpFiles.size();i++)
			{
				if(stricmp(g_newHttpFiles[i].file, temp.t) == 0)
				{
					BOOL bFind = false;
					for(int j=0;j<g_oldHttpFiles.size();j++)
					{
						if(g_oldHttpFiles[j].file == temp)
						{
							if(g_oldHttpFiles[j].low != g_newHttpFiles[i].low || g_oldHttpFiles[j].high != g_newHttpFiles[i].high)
							{
								bFind = true;
								bMustDownload = true;
							}
							break;
						}
					}
					if(!bFind)
						bMustDownload = true;
					break;
				}
			}
		}
		if(!bMustDownload)
			return;
	}
	char buffer[512];
	FileName ext,file,dir;
	temp.splitpath(&dir, &file, &ext);
	dir += file;
	if(ext == ".png" || ext == ".jpg")
		ext = ".limg";
	else if(ext == ".rpgui")
		ext = ".lui";
	else if(ext == ".rpgmesh")
		ext = ".lmesh";
	else if(ext == ".pakui")
		ext = ".pui";
	else if(ext == ".pakmesh")
		ext = ".pmesh";
	else if(ext == ".lua")
		ext = ".lsc";
	else if(ext == ".table" || ext == ".inl" || ext == ".bin" || ext == ".dat" || ext == ".txt")
		ext = ".lt";
	dir += ext;
	sprintf(buffer, "%s%s", g_szHttpPath.t, dir.t);
	if(!bInMemory){
		createFolders(g_szProjectDir, dir.t);
		//file = g_szProjectDir;
		//file += dir.t;
	}
	//else
	{
		file = dir.t;
	}
	downloadFile(buffer, file.t, &HttpDownloadOK, !bDownloadNow, bInMemory);
}
BOOL g_bActive = false;
VOID	ActiveGameMode(BOOL b){
	g_bActive = b;
	FreeAllMemoryFiles();
	g_manager.StopAllMusic();
	g_luamanager.Destroy();
	if(b)
		SetLogPrint(DeviceLogPrint);
	else
		SetLogPrint(0);
	if(!b)
	{
		SetPhantomGUID((unsigned int*)g_guid);
//		SetDocumentDir(g_documents.t);
//		GetGameManager()->m_defaultLua = "sc/main.lua";
//		GetGameManager()->ResetAll();
	}
}
VOID		RunDocumentProj(const char* proj, unsigned int* guid, const char* szMainLua, int orientation)
{
	ActiveGameMode(TRUE);
	SetMyLoadFile(0);//本地运行不需要从网络下载任何数据
	SetPhantomGUID((unsigned int*)guid);
    SetViewOrientation(orientation);
	char szNewDoc[512];
	strcpy(g_szProjectDir, proj);
	strlwr_t(g_szProjectDir);
	sprintf(szNewDoc, "%s%s", g_documents.t, g_szProjectDir);
	SetDocumentDir(szNewDoc);
	GetGameManager()->m_defaultLua = szMainLua;//"sc/main.lua";
    int len = GetGameManager()->m_defaultLua.size();
    for(int i=0;i<len;i++)
    {
        if(GetGameManager()->m_defaultLua.t[i]=='\r' || GetGameManager()->m_defaultLua.t[i]=='\n')
            GetGameManager()->m_defaultLua.t[i]= 0;
    }
	LogInfo("RunDocumentProj");
	GetGameManager()->ResetAll();
}

BOOL		RunDocumentHttp(const char* proj)
{
	ActiveGameMode(TRUE);
	LFileName f = proj;
	LFileName file,ext;
	f.splitpath(&g_szHttpPath, &file, &ext);
	file += ext;
	if(!(ext == ".phantom"))
		return false;
	SetMyLoadFile(&MyHttpDownloadFile);//本地运行不需要从网络下载任何数据
	InitMyFile(file.t, true, true);
	GameInfo info;
	CFileHelperR rp;
	if(!rp.open(file.t))
		return false;
	char head[4];
	head[3] = 0;
	rp.pStream->read(head, 3);
	if(stricmp(head, "web")!=0)
		return false;
	int version = 0;
	rp >> version;
	int orig,length;
	rp >> orig;
	rp >> length;
	char* comp = new char[length];
	rp.pStream->read(comp, length);
	//
	rp.close();
	Phantom::FreeAllMemoryFiles();//释放所有之前的内存文件
	//
	CDynamicArray<char> reads;
	reads.SetArrayCount(orig);
	Uncompress(reads.GetPtr(), orig, comp, length);
	assert(orig == reads.size());
	delete[] comp;
	//
	streamRead r(reads.GetPtr(), reads.size());
	int numFile = 0;
	r >> numFile;
	g_newHttpFiles.SetArrayCount(numFile);
	for(int i=0;i<numFile;i++)
	{
		r >> g_newHttpFiles[i].low >> g_newHttpFiles[i].high;
		int filesize;
		r >> filesize;
		r.read(g_newHttpFiles[i].file, filesize);
	}
	r >> info;
	int leninfo = strlen(info.mainfile);
	for(int i=0;i<leninfo;i++)
		if(info.mainfile[i] == '\r' || info.mainfile[i] == '\n')
			info.mainfile[i] = 0;
	//
	std::vector<unsigned char> guid;
	HexEncode(guid, info.auth);
	if(guid.size() != (38*4))
		return false;
	SetPhantomGUID((unsigned int*)&guid.at(0));
	GetGameManager()->m_defaultLua = info.mainfile;
	char szNewDoc[512];
	strcpy(g_szProjectDir, info.guid);
	strcat(g_szProjectDir, "/");
	strlwr_t(g_szProjectDir);
	sprintf(szNewDoc, "%s%s", g_documents.t, g_szProjectDir);
	createFolders("", g_szProjectDir);
	SetDocumentDir(szNewDoc);
	//
	LoadOldFileInfos();//读取旧的文件数据
	//
	GetGameManager()->ResetAll();
	return true;
}

//void OnRequestIP(Name& file, IDENetClient& obj, const char* errorText)
//{
//    //const char* pvdapp = obj["pvdapp"];
//    //int newV = obj["version"];//new game version;
//    //const char* szfile = obj["downloadurl"];
//    //SetServerIP(pvdapp);
//}
class	CMyManager: public CGameManager
{
public:
	int		m_gameWidth;
	int		m_gameHeight;
	BOOL	m_nDownType;
	Pixel	m_downPoint;
	double	m_dDownTime;
	GameState* m_mystate;
	CMyManager()
	{
		m_mystate	=	0;
		m_gameWidth = 960;
		m_gameHeight = 640;
		m_nDownType = 0;
		m_dDownTime = 0;
		m_downPoint.Set(0, 0);
		LogInfo("CMyManager");
	}
	~CMyManager()
	{
	}
	virtual	VOID						OnCreate()
	{
		LogInfo("CMyManager::OnCreate");
		SetMyLoadFile(0);
        //IDENetClient obj;
        //RequestIP(OnRequestIP,obj);
//		g_manager.SetGameSize(m_gameWidth, m_gameHeight);
        CGameManager::OnCreate();
	}
	virtual	BOOL	OnFrameMove(double dElapsedTime)
	{
        if(m_nDownType>0)
		{
			this->m_dDownTime += dElapsedTime;
			if(m_dDownTime>=2.0f)
			{
				m_nDownType = 0;
				m_dDownTime = 0;
			}
		}
		return CGameManager::OnFrameMove(dElapsedTime);
	}
	virtual	VOID	OnInputEvent(Phantom::InputEventID evt, int x, int y, unsigned int touchIndex)
	{
		CGameManager::OnInputEvent(evt, x, y, touchIndex);
		if(!g_bActive)
			return;
		if(evt == Phantom::InputEventID_ButtonDown && touchIndex == 0)
		{
			m_nDownType++;
			m_downPoint.Set(x,y);
		}
		else if(evt == Phantom::InputEventID_ButtonUp && touchIndex == 0)
		{
			int dx = x - m_downPoint.x;
			int dy = y - m_downPoint.y;
			switch(m_nDownType)
			{
			case 1:
				{
					if(dx<-100&&dy>100){
						m_dDownTime = 0;
					}
					else
						m_nDownType = 0;
				}
				break;
			case 2:
				{
					if(dx>100&&dy>100){
						ActiveGameMode(false);
					}
					else
						m_nDownType = 0;
				}
				break;
			}
			m_dDownTime = true;
		}
	}
	virtual	VOID	OnDestroy()
	{
		safe_delete(m_mystate);
		CGameManager::OnDestroy();
		//m_service.m_server.StopServer();
	}
	char			m_szGameName[256];
	VOID			OnBeforeDeviceInit()
	{
	}

	virtual	const char*					GetGameName(){return m_szGameName;}
};

CMyManager*	g_gameMgr = 0;
CMyManager*	GetMyManager(){
	if(!g_gameMgr)
		g_gameMgr = new CMyManager();
	return g_gameMgr;
}
CGameManager*					GetGameManager()
{
	return GetMyManager();
}
bool g_bServerRunning = false;
extern "C"{
    VOID StartServer()
    {
        if(!g_bServerRunning){
            g_bServerRunning=true;
            //LogInfo("start server");
            //GetMyManager()->m_service.StartServer();
        }
    }
    VOID StopServer()
    {
        if(g_bServerRunning){
            g_bServerRunning=false;
            //GetMyManager()->m_service.m_server.StopServer();
            //SetLogPrint(0);
            //LogInfo("stop server");
        }
    }
};
INT			GetBrowserUrlCount()
{
	int index = 0;
	for(int i=0;i<50;i++)
	{
		char buf[128];
		sprintf(buf, "browser_%d", i);
		if(GetMyManager()->m_mystate->find(buf)>=0)
			index++;
	}
	if(index == 0)
	{
		index = 1;
		GetMyManager()->m_mystate->write("browser_0", "http://www.aixspace.com");
	}
	return index;
}
const char* GetBrowserUrl(int index)
{
	char buf[128];
	sprintf(buf, "browser_%d", index);
	return GetMyManager()->m_mystate->readt(buf);
}
namespace PhantomV1{
extern VOID		SetAutoSave(BOOL b);
};
BOOL		AddBrowserUrl(const char* url)
{
	int index = 0;
	for(int i=0;i<50;i++)
	{
		char buf[128];
		sprintf(buf, "browser_%d", i);
		const char* szUrl = GetMyManager()->m_mystate->readt(buf);
		if(stricmp(szUrl, url) == 0)
			return false;
		else if(!szUrl[0])
			break;
		index++;
	}
	index %= 50;
	char buf[128];
	sprintf(buf, "browser_%d", index);
	GetMyManager()->m_mystate->write(buf, url);
	//Phantom::SetAutoSave(true);
	return true;
}

VOID DeviceLogPrint(const char* szText)//输出信息
{
}

GameState*	GetManagerState()
{
	return GetMyManager()->m_mystate;
}
#ifdef ANDROID_OS

extern "C"
{
	BOOL CreatePhantom(int nDiv, int nOS, int* authcode)
	{
		return false;
	}

	JNIEXPORT jboolean	JNICALL Java_com_android_phantom_Phantom_isShowAdmob(JNIEnv * env, jobject obj);

};

int	InitGame()
{
	g_manager.SetTextureDivAsFile(false);	//是否在文件内已经缩小
	return 1;
}

#endif

