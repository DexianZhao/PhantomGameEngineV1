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
#include "NetService.h"
#include "PhantomDB.h"

FileName	g_documents;
const char*	GetRealDocuments()
{
	return g_documents;
}
VOID		RunDocumentProj(const char* proj, unsigned int* guid, const char* szMainLua,int)
{
}
namespace Phantom{
	extern	VOID	SetPlatformMode(BOOL b);
};
extern "C"{
	int GetAppID(){return 0;}
    extern BOOL CreatePhantom(int nDiv, int nOS, int* authcode);
    void MyCreatePhantom(int nDiv, int nOS)
    {
        int guid[] = {-1,2,3,4,-5,-6,-7,8,-9,1,-2,3,-4,5,-6,-7,8,-9,1,2,-3,4,5,6,7,8,9,-1,-2,3,4,5,-6,-7,-8,9,1,-2};
        CreatePhantom(nDiv,nOS,guid);
    }
};
#ifdef WIN32_OS
char	g_szProjectDir[256];
VOID	SetProjectDir(){
	SetCurrentDirectoryA(g_szProjectDir);
}
#endif

VOID	CreateClient()
{
}
class	CMyManager: public CGameManager
{
public:
	int		m_gameWidth;
	int		m_gameHeight;
	CMyManager()
	{
		m_gameWidth = 960;
		m_gameHeight = 640;
	}
	~CMyManager()
	{
	}
	virtual	VOID						OnCreate()
	{
		//g_manager.SetUIDialogSize(m_gameWidth, m_gameHeight);
		CGameManager::OnCreate();
	}
	virtual	BOOL	OnFrameMove(double dElapsedTime)
	{
		//
		return CGameManager::OnFrameMove(dElapsedTime);
	}
	virtual	VOID	OnDestroy()
	{
		CGameManager::OnDestroy();
	}
	char			m_szGameName[256];
	VOID			OnBeforeDeviceInit()
	{
		int temp[] = {2121146461,335361042,709957183,-1409153589,-1186168246,134540350,1569417824,1451482773,1851112207,2124548912,-532768907,802521239,1406042748,189487410,-1562989010,22718119,1557129000,1417298210,-1698681320,1944702083,-1662739364,-712583042,1609110364,-978315288,-1105227742,980190486,1978431245,934649790,819395594,1093787754,-1668912053,-1702509916,-455155607,183567363,2014523461,257849587,-800803515,766220338,-1146955138,-1191843921,355635503,-395588014,1981569136,-1666653231,1018947146,15579411,158741106,-1559844664,1672653343,-648222171,-133263093,2091125971,-1923978898,263725075,-1068475093,1537720194,-1132137103,-1092407791,1917025102,902810321,-1151489217,-1683245972,1178401832,600298189,-669474764,1746638959,-1509521633,1613539293,-1607202177,210861638,1414105183,-934019414,-603607002,1022026879,1501782045,-520600610,-1733418417,888833031,1373267319,-885532507,-86874521,524749870,335824897,1502050246,-666320316,-1972449707,105446171,1181302723,-213756103,-1333872819,-1828445337,1912154279,-346264548,1205646920,1669081139,913153205,-1412397712,-545640701,1647506258,463106032,273528354,1535244152,743605001,506192817,76618073,-1156153526,-571583900,1927580557,-565687510,-2032964767,132497736,-1563635248,-2130842334,-235093890,-931058897,-658290783,-586947718,1352520574,691760765,-323624532,118254653,929240401,1621160286,-279738222,-103891410,956942880,-1198899155,-748008575};
		SetPhantomGUID((unsigned int*)temp);
		Phantom::SetPlatformMode(TRUE);
#ifdef WIN32_OS
		//
		char wdrive[256], wdir[256];
		char szIni[256];// = 
		char screenW[128],screenH[128], current[128];
		char szExePath[1024];
		int len = ::WideCharToMultiByte(CP_ACP, 0, _wpgmptr, -1, szExePath, 1024, "", 0);
		szExePath[len] = 0;

		_splitpath(szExePath, wdrive, wdir, 0, 0);
		strcat(wdrive, wdir);
		strcpy(szIni, wdrive);
		strcat(szIni, "Phantom.ini");
		::GetPrivateProfileStringA("active", "name", "", m_szGameName, 128, szIni);
		::GetPrivateProfileStringA(m_szGameName, "game_width", "", screenW, 128, szIni);
		::GetPrivateProfileStringA(m_szGameName, "game_height", "", screenH, 128, szIni);
		::GetPrivateProfileStringA(m_szGameName, "path", "", current, 128, szIni);
		m_gameWidth = atoi(screenW);
		m_gameHeight = atoi(screenH);
		strcat(wdrive, current);
		strcat(wdrive, "\\");
		strcpy(g_szProjectDir, wdrive);
		SetCurrentDirectoryA(g_szProjectDir);
		g_documents	=	g_szProjectDir;
#else
		g_documents	=	GetDocumentDir();
		//CFileHelperR r;
		//if(r.openInDoc("gameinit.txt"))
		//{
		//	char szBuf[512];
		//	int len = r.read(szBuf, 512);
		//	szBuf[len] = 0;
		//	const char* doc = GetDocumentDir();
		//	char szNewDoc[512];
		//	sprintf(szNewDoc, "%s%s/", doc, szBuf);
		//	SetDocumentDir(szNewDoc);
		//}
#endif
		//台式机		int guid[] = {-881829239,531795469,205672448,133909,-279732572,-40749204,-1544058199,1115756337,-639047129,1877516808,-740304721,1521638536,-1207154134,408159893,-1290536934,-628714630,1312163671,-871496481,210487935,378976542,-920389104,1091360994,1824501840,1353990702,2100465822,41953219,1187414059,-484479293,-61857979,1836278236,1731518070,1000663815,-2145327700,-1893731463,-1491181980,1799716396,1382820141,-1567732596};
		//笔记本
//#ifdef WIN32_OS
//		int guid[] = {-376867224,348769038,21102336,145938,739959454,-989400060,58206874,1293456343,-1837456342,-1448429307,-2011564774,1600050609,-982019583,2004173093,-303031510,-1453305021,-1385579729,-1067078979,-1853199396,-1708762691,1308156945,-789828370,729070829,-1281449460,-1688336578,661990138,-929618305,-1682381116,346655857,-2002632347,-1148536883,193990150,-459755594,-1602693668,-1791137797,1014068145,-985821720,293334907};
//		SetPhantomGUID((unsigned int*)guid);
//#endif
		//Phantom::open_zip("PhantomPlatform.apk", "assets");
	}

	virtual	const char*					GetGameName(){return m_szGameName;}
};

CMyManager		g_gameMgr;
GameState*	GetManagerState()
{
	return 0;
}
CGameManager*					GetGameManager()
{
	return &g_gameMgr;
}
#ifdef ANDROID_OS

BOOL	g_bShowAD = true;
extern "C"
{
	int		OnAndroidBack()
	{
		return 0;
	}
	void    showAd(int x, int y, int type, int bShow)
	{
		g_bShowAD = bShow;
		if(bShow)
			AddWaveCommand(WaveCommand::command_id_showad, type, 0);//g_bShowAD	=	true;
		else
			AddWaveCommand(WaveCommand::command_id_hidead, 0, 0);
	}
	void    shopBuy(const char* buyName)
	{
		AddWaveCommand(WaveCommand::command_id_buy, 0, buyName);
	}
	void    shopRestore(const char* buyName)
	{
		AddWaveCommand(WaveCommand::command_id_restore, 0, buyName);
	}
	//void    hideAd()
	//{
	//	if(g_manager.GetUIOffset().y>50)
	//	{
	//		AddWaveCommand(WaveCommand::command_id_showad, 0, 0);//g_bShowAD	=	true;
	//		g_bShowAD = true;
	//	}
	//	else{
	//		g_bShowAD	=	false;
	//		AddWaveCommand(WaveCommand::command_id_hidead, 0, 0);
	//	}
	//}
	void	showMyAD(const char* image, const char* url, int advid, int type)
	{
	}
	void	setAdMode(int mode, const char** urls, const char** images, int count, int myadv_gailv)//设置我的广告条列表
	{
	}
	extern void SetBuyADOK(BOOL bFail);
	void	gotoBuyRemoveAD(int)
	{
		//SetBuyADOK(false);
	}
	void	gotoComment()
	{
		AddWaveCommand(WaveCommand::command_id_openurl, 0, 0);
	}
	void	openUrl(const char* url)
	{
		AddWaveCommand(WaveCommand::command_id_openurl, 0, url);
	}
	void	openEdit(const char* szDefault, int x, int y, int w, int h)
	{
	}
	void	closeEdit(char* ret)
	{
	}

	JNIEXPORT jboolean	JNICALL Java_com_android_phantom_Phantom_isShowAdmob(JNIEnv * env, jobject obj);

};

int	InitGame()
{
	g_manager.SetTextureDivAsFile(false);	//是否在文件内已经缩小
	return 1;
}

#endif

