/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "GCStateDefault.h"
#include "GCViewManager.h"
#include "io.h"
#include "../PhantomEngine/encrypt.h"
#include "MainFrm.h"
#include "IDEHttp.h"
#include <zlib.h>
extern const char*	GetAppID();
Phantom::NameT<512>		g_strProjectPath;
void			SetProjectPath( const char* szBuf )
{
	char buf[512];
	if(szBuf)
		::SetCurrentDirectoryA( szBuf );
	else
	{
		GetModuleFileNameA(0, buf, 255);
		char szD[255], szR[255];
		_splitpath(buf, szD, szR, 0, 0);
		strcat(szD, szR);
		::SetCurrentDirectoryA( szD );
	}
	::GetCurrentDirectoryA(512, buf);
	strcat(buf, "\\");
	g_strProjectPath = buf;
	g_strProjectPath.setIsFile();
}

void			SetProjectPath()
{
	::SetCurrentDirectoryA( g_strProjectPath.t );
}

const char*		GetProjectPath()
{
	return g_strProjectPath.t;
}

//
//GCStateDefault::~GCStateDefault(void) 
//{
//}
//extern BOOL IsUsePhantom3D();
//BOOL	GCStateDefault::OnCreateState()
//{
//	this->m_bExit	=	false;
//	GetProjectPtr()->m_renderState.bRenderUI	=	true;
//	return true;
//}
//
//void	GCStateDefault::OnDestroyState()
//{
//	GetProjectPtr()->SetSleepMode(FALSE);
//}
//extern	BOOL	IsAuthError();
//extern	VOID	SetAuthError();
//void	GCStateDefault::OnFrameMove(const RenderParameter& r)	//每次更新
//{
//	GCViewBase* view	=	GetViewManager()->GetActiveView();
//	if(view&&!IsUsePhantom3D())
//		view->OnFrameMove();
//}
//
//void	GCStateDefault::OnRenderingBegin(const RenderParameter& r)	//渲染
//{
//	GCViewBase* view	=	GetViewManager()->GetActiveView();
//	if(view&&!IsUsePhantom3D())
//		view->OnRenderingBegin();
//}
//
//void	GCStateDefault::OnRendering(const RenderParameter& r)	//渲染
//{
//	GCViewBase* view	=	GetViewManager()->GetActiveView();
//	if(view&&!IsUsePhantom3D())
//		view->OnRendering();
//}
//
//void	GCStateDefault::OnPushRenderObjects(const RenderParameter& r)	//渲染
//{
//	GCViewBase* view	=	GetViewManager()->GetActiveView();
//	if(view&&!IsUsePhantom3D())
//		view->OnPushRenderObjects();
//}
//
//void	GCStateDefault::OnRenderingEnd(const RenderParameter& r)	//渲染
//{
//	GCViewBase* view	=	GetViewManager()->GetActiveView();
//	if(view&&!IsUsePhantom3D())
//		view->OnRenderingEnd();
//}
//
//void	GCStateDefault::OnRenderEndUI(const RenderParameter& r)	//界面渲染之后
//{
//	GCViewBase* view	=	GetViewManager()->GetActiveView();
//	if(view&&!IsUsePhantom3D())
//		view->OnRenderEndUI();
//}
VOID	OnViewFrameMove()
{
	//const RenderParameter& r = GetRenderHelperPtr()->GetRenderParameter();
	GCViewBase* view	=	GetViewManager()->GetActiveView();
	if(view)
		view->OnFrameMove();
}
VOID	OnViewRender()
{
	GCViewBase* view	=	GetViewManager()->GetActiveView();
	if(view)
	{
		view->OnPushRenderObjects();
		view->OnRenderingBegin();
		view->OnRendering();
	}
}
VOID	OnViewRenderUI()
{
	GCViewBase* view	=	GetViewManager()->GetActiveView();
	if(view)
	{
		view->OnRenderingEnd();
		view->OnRenderEndUI();
	}
}
extern VOID	OnAppMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//bool	GCStateDefault::OnWindowProc(void* hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam)
//{
//	return false;
//}

BOOL	FileExist(const char* szFile)
{
	WIN32_FIND_DATAA wsa;
	HANDLE h=FindFirstFileA(szFile,&wsa);
	if(h!=INVALID_HANDLE_VALUE)
	{
		FindClose(h);
		return true;
	}
	return false;
}
void			CreateFolders(const char* szSubFolders)
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
			if(!FileExist(strDir))
				if(!::CreateDirectoryA(strDir,NULL))
				{
				}
				else
				{
				}
				strDir[i]='\\';
		}
	}
}

int	ReadFileAll(std::vector<char>& rets, const char* szFileName)
{
	FILE* f = fopen(szFileName, "rb");
	if(!f)
		return 0;
	int flen = _filelength(_fileno(f));
	int old = rets.size();
	if(flen > 0){
		rets.resize(rets.size() + flen);
		fread(&rets.at(0) + old, flen, 1, f);
	}
	fclose(f);
	return flen;
}

BOOL	setIsFile(char* t)
{
	int len	=	strlen(t);
	while((t[0] == '/' || t[0] == '\\' || (t[0] == '.' && t[1] != '.')) && t[0] != 0)
	{
		memmove(t, t + 1, len);
		len--;
	}
	for(int i=0;i<len;i++)
	{
		if(t[i]<0)return false;
		if(t[i] == '\\')
			t[i] = '/';
		else
			t[i]	=	tolower(t[i]);
	}
	return true;
}
std::vector<FileInfo>	g_reportFiles;
VOID	InitReport()
{
	g_reportFiles.clear();
	INT nVersion = 1;
	char buf[256];
	sprintf(buf, "../reportbin_%s.list", GetAppID());
	FILE* f = fopen(buf, "rb");
	if(!f)return;
	int count;
	fread(&nVersion, sizeof(int), 1, f);
	fread(&count, sizeof(int), 1, f);
	if(count > 0){
		g_reportFiles.resize(count);
		fread(&g_reportFiles.at(0), sizeof(FileInfo)*g_reportFiles.size(), 1, f);
	}
	fclose(f);
}
VOID	SaveReport()
{
	INT nVersion = 1;
	char buf[256];
	sprintf(buf, "../reportbin_%s.list", GetAppID());
	FILE* f = fopen(buf, "wb");
	if(!f)return;
	int count = g_reportFiles.size();
	fwrite(&nVersion, sizeof(int), 1, f);
	fwrite(&count, sizeof(int), 1, f);
	fwrite(&g_reportFiles.at(0), sizeof(FileInfo)*g_reportFiles.size(), 1, f);
	fclose(f);
}
BOOL	ReqReport(FileInfo& info){
	for(int i=0;i<g_reportFiles.size();i++)
	{
		if(stricmp(g_reportFiles[i].file, info.file) == 0)
		{
			if(g_reportFiles[i].w.dwHighDateTime == info.w.dwHighDateTime && g_reportFiles[i].w.dwLowDateTime == info.w.dwLowDateTime)
				return false;
			g_reportFiles[i] = info;
			return true;
		}
	}
	g_reportFiles.push_back(info);
	return true;
}
//
std::vector<FileInfo>	g_runFiles;
std::vector<FileInfo>	g_runTemps;
VOID	InitRunFiles(const char* szFileName)
{
	g_runFiles.clear();
	INT nVersion = 1;
	FILE* f = fopen(szFileName, "rb");
	if(!f)return;
	int count;
	fread(&nVersion, sizeof(int), 1, f);
	fread(&count, sizeof(int), 1, f);
	if(count > 0){
		g_runFiles.resize(count);
		fread(&g_runFiles.at(0), sizeof(FileInfo)*g_runFiles.size(), 1, f);
	}
	g_runTemps.clear();
	fclose(f);
}
VOID	SaveRunFiles(const char* szFileName)
{
	INT nVersion = 1;
	FILE* f = fopen(szFileName, "wb");
	if(!f)return;
	int count = g_runFiles.size();
	fwrite(&nVersion, sizeof(int), 1, f);
	fwrite(&count, sizeof(int), 1, f);
	fwrite(&g_runFiles.at(0), sizeof(FileInfo)*g_runFiles.size(), 1, f);
	fclose(f);
}
BOOL	ReqRunFiles(FileInfo& info){
	for(int i=0;i<g_runFiles.size();i++)
	{
		if(stricmp(g_runFiles[i].file, info.file) == 0)
		{
			if(g_runFiles[i].w.dwHighDateTime == info.w.dwHighDateTime && g_runFiles[i].w.dwLowDateTime == info.w.dwLowDateTime)
				return false;
			g_runTemps.push_back(info);
			//g_runTemps[i] = info;
			//g_runFiles[i] = info;
			return true;
		}
	}
	//g_runFiles.push_back(info);
	g_runTemps.push_back(info);
	return true;
}

VOID	WriteLock(Phantom::RandomVal& rndSave, Phantom::RandomVal& rnd, std::vector<char>& buffers, char* head, const char* szLockFile)
{
	//if(FileExist(szLockFile))
	//	DeleteFileA(szLockFile);
	Phantom::EncryptVal val;
	int len = sizeof(Phantom::EncryptVal);
	FILE* f = fopen(szLockFile, "wb");
	int version = 1;
	fwrite(head, 4, 1, f);
	fwrite(&version, sizeof(int), 1, f);
	//old
	//fwrite(&rndSave, sizeof(Phantom::RandomVal), 1, f);
	//Phantom::EncryptData(&buffers.at(0), buffers.size(), rnd, val);
	//fwrite(&val, sizeof(Phantom::EncryptVal), 1, f);
	//fwrite(&buffers.at(0), buffers.size(), 1, f);
	Phantom::EncryptData(&buffers.at(0), buffers.size(), rnd, val);
	CDynamicArray<char> datas;
	datas.SetArrayCount(buffers.size()+sizeof(Phantom::RandomVal)+sizeof(Phantom::EncryptVal));
	int index = 0;
	memcpy(datas.GetPtr(), &rndSave, sizeof(Phantom::RandomVal));
	index += sizeof(Phantom::RandomVal);
	memcpy(datas.GetPtr() + index, &val, sizeof(Phantom::EncryptVal));
	index += sizeof(Phantom::EncryptVal);
	memcpy(datas.GetPtr() + index, &buffers.at(0), buffers.size());
	CDynamicArray<char> ziped;
	ziped.SetArrayCount(datas.size()+1000000);
	//压缩之后保存
	unsigned long length = ziped.size();
	if(compress((Bytef*)ziped.GetPtr(), &length, (Bytef*)datas.GetPtr(), datas.size()) != Z_OK)
	{
		GetMainFrame()->MessageBox(Language(L"compress_error", L"压缩数据是产生错误"));
		return;
	}
	unsigned int orig = datas.size();
	fwrite(&orig, sizeof(unsigned int), 1, f);
	fwrite(&length, sizeof(unsigned int), 1, f);
	fwrite(ziped.GetPtr(), length, 1, f);//&rets.at(0), rets.size(), 1, f);
	//
	fclose(f);
}
BOOL	IsCanReport(const char* szFileNameIn)
{
	if(szFileNameIn[1] == ':')
		return false;
	char szFileName[512];
	strcpy(szFileName, szFileNameIn);
	if(!setIsFile(szFileName)){
		return false;
	}
	char szDrive[256], szDir[256], szFile[256], szExt[256];
	_splitpath(szFileName, szDrive, szDir, szFile, szExt);
	char szTemp[256];
	strcpy(szTemp, szFile);
	strcat(szTemp, szExt);
	if(stricmp(szExt, ".exe") == 0 || stricmp(szExt, ".dll") == 0 || stricmp(szExt, ".rect") == 0 || stricmp(szExt, ".controls") == 0 ||
		stricmp(szExt, ".report") == 0 || stricmp(szExt, ".dmp") == 0 || strstr(szFileName, "app_store") || strstr(szFileName, "document")
		|| stricmp(szExt, ".dds") == 0 ||stricmp(szExt, ".tga") == 0 ||stricmp(szExt, ".bmp") == 0
		|| stricmp(szTemp, "phantom.ini") == 0 || stricmp(szTemp, "phantom.txt") == 0 || stricmp(szTemp, "desktop.txt") == 0
		|| stricmp(szExt, ".zip") == 0 || stricmp(szExt, ".7z") == 0 || stricmp(szExt, ".rar") == 0 || stricmp(szExt, ".db") == 0|| stricmp(szExt, ".phantom") == 0
		|| stricmp(szExt, ".objects") == 0 || stricmp(szExt, ".rgn") == 0 || stricmp(szExt, ".sec") == 0 || /*stricmp(szExt, ".rpgscene") == 0 ||*/ stricmp(szExt, ".rpg2ds") == 0 || stricmp(szExt, ".rpgexp") == 0 || stricmp(szExt, ".rpgexpani") == 0 ||stricmp(szExt, ".mix") == 0 ||
		stricmp(szExt, ".bld") == 0|| stricmp(szFile, "states") == 0|| stricmp(szFileName, "sc/config.txt") == 0)
		return false;
	return true;
}
extern	BOOL	GetRuntimeAuthGUID(Phantom::GenuineGUID* ret);
extern	void	renameFolders(const char* szSubFolders);
extern	int		GetOriginalSize(int n);
int	g_pngCompress = 0;
void SetPngCompress(int n)
{
	g_pngCompress = n;
}
#pragma pack(push,1)
struct MyImageInfo{
	unsigned short	width,height;
	unsigned short	version;
	unsigned int	length;
};
#pragma pack(pop)
//extern int compress2(unsigned char *dest,   unsigned long *destLen,
                                  //const unsigned char *source, unsigned long sourceLen,
                                  //int level);
VOID	Compress(const wchar_t* szPngFile, std::vector<char>& rets)//const char* szBinFile)
{
	CImage img;
	img.Load(szPngFile);//"test.png");
	unsigned char* bits = (unsigned char*)img.GetBits();
	int pitch = img.GetPitch();
	MyImageInfo info;
	info.width = img.GetWidth();
	info.height = img.GetHeight();
	info.version = 1;
	unsigned char* outbits = new unsigned char[info.width*info.height*2];
	for(int y=0;y<img.GetHeight();y++)
	{
		unsigned char* xbegin = bits + y * pitch;
		for(int x=0;x<img.GetWidth();x++)
		{
			unsigned char* pbegin = xbegin + x * 4;
			unsigned char* dbegin = outbits + (x + y * info.width)*2;
			int r = ((int)pbegin[2])/16;
			int g = ((int)pbegin[1])/16;
			int b = ((int)pbegin[0])/16;
			int a = ((int)pbegin[3])/16;
			dbegin[0] = r + (g*16);
			dbegin[1] = b + (a*16);
			//pbegin[3] = 0;//bgra
		}
	}
	//img.Save("out.png");
	//
	int len = info.width*info.height*2;
	unsigned long destlen = len;
	unsigned char* dest = new unsigned char[len+1024*1024];
	//compress2(dest, &destlen, outbits, len, 9);

	/*FILE* f = fopen(szBinFile, "wb");
	info.length = destlen;
	fwrite(&info, sizeof(MyImageInfo), 1, f);
	fwrite(dest, destlen, 1, f);
	fclose(f);*/
	rets.resize(destlen+sizeof(MyImageInfo));
	memcpy((char*)&rets.at(0), &info, sizeof(MyImageInfo));
	memcpy((char*)&rets.at(0)+sizeof(MyImageInfo), dest, destlen);
	delete dest;
	delete outbits;
}
namespace Phantom{
	extern	void	ConvertToPng(const char* szFile, BOOL bRemoveOld = true);
};

BOOL	ReportLockFile(const char* szFileNameIn, char* ret)
{
	const char* path = GetProjectPath();
	SetProjectPath();
	if(szFileNameIn[1] == ':')
		return false;
	char szFileName[512];
	strcpy(szFileName, szFileNameIn);
	if(!setIsFile(szFileName))
	{
		return false;
	}
	renameFolders(szFileName);
	char szDrive[256], szDir[256], szFile[256], szExt[256];
	_splitpath(szFileName, szDrive, szDir, szFile, szExt);
	if(stricmp(szExt, ".dds") == 0 || stricmp(szExt, ".tga") == 0 || stricmp(szExt, ".bmp") == 0)
	{
		ConvertToPng(szFileName, true);
	}
	char szTemp[256];
	strcpy(szTemp, szFile);
	strcat(szTemp, szExt);
	if(!IsCanReport(szFileName))
		return false;
	int len = 0;
	BOOL bReqWrite = true;
	{
		HANDLE hFile=::CreateFileA(szFileName, GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return false;
		DWORD dwHigh = 0;
		DWORD dwSize = ::GetFileSize(hFile, &dwHigh);
		FILETIME c, l, w;
		::GetFileTime(hFile, &c, &l, &w);
		CloseHandle(hFile);
		FileInfo info;
		info.length = dwSize;
		info.w.dwHighDateTime = w.dwHighDateTime;
		info.w.dwLowDateTime = w.dwLowDateTime;
		strcpy(info.file, szFileName);
		if(!ReqReport(info))
			bReqWrite = false;
	}
	strcat(szDrive, szDir);
	char szLockFile[512];
	char szReport[512];
	sprintf(szReport, "../reportbin/");//%s/", GetAppID());;
	//const char* szReport = "../reportbin/";
	strcpy(szLockFile, szReport);
	strcat(szLockFile, szDrive);
	strlwr(szLockFile);
	int lens = strlen(szLockFile);
	for(int i=0;i<lens;i++)
	{
		if(szLockFile[i]<0)
		{
			MessageBox(GetForegroundWindow(), L"文件名只能使用英文和数字，不能使用中文", L"Fail", MB_OK);
			break;
		}
	}
	CreateFolders(szLockFile);
	std::vector<char> rets;
	if(bReqWrite)
		ReadFileAll(rets, szFileName);
	//if(rets.size() < 256)
	//{
	//	int old = rets.size();
	//	rets.resize(256);
	//	rets[old] = 0;
	//	for(int i=(old+1);i<256;i++)
	//	{
	//		rets[i] = rand()%255;
	//	}
	//}
	//
	strcat(szLockFile, szFile);
	strcat(szDrive, szFile);
	//
	Phantom::EncryptVal val;
	Phantom::GenuineGUID gen;
	if(!GetRuntimeAuthGUID(&gen))
		return false;
	Phantom::RandomVal rnd, rndSave;
	if(bReqWrite)
		Phantom::EncryptRandomVal(rndSave, rnd, gen);
	if(stricmp(szExt, ".png") == 0 || stricmp(szExt, ".jpg") == 0)
	{
		char szJpeg[256],szPng[256];
		strcpy(szJpeg, szDrive);
		strcat(szJpeg, ".jpg");
		strcpy(szPng, szDrive);
		strcat(szPng, ".png");
		if(fexist(szJpeg) && fexist(szPng))
		{
			MessageBox(GetForegroundWindow(), Language(L"NoJpgAndPng", L"一个文件不能同时存在jpg扩展名和png扩展名的文件"), W(szFileName).buf, MB_OK);
			return false;
		}
		strcat(szLockFile, ".limg");
		if(bReqWrite){
			//if(FileExist(szLockFile))
			//	DeleteFileA(szLockFile);
			char szRect[256];
			strcpy(szRect, szDrive);
			strcat(szRect, ".rect");
			int tableBegin = 0;
			if(FileExist(szRect))
			{
				tableBegin = rets.size();
				//if(g_pngCompress){
				//}
				//else
				ReadFileAll(rets, szRect);
			}
			int len = sizeof(Phantom::EncryptVal);
			FILE* f = fopen(szLockFile, "wb");
			int version = 2;
			char head[4] = {'l','o','c','i'};
			fwrite(head, sizeof(head), 1, f);
			if(g_pngCompress)
				version |= 0x00010000;//如果使用压缩图片功能则
			fwrite(&version, sizeof(int), 1, f);
			fwrite(&tableBegin, sizeof(int), 1, f);
			fwrite(&rndSave, sizeof(Phantom::RandomVal), 1, f);
			if(rets.size() < 128)
			{
				int old = rets.size();
				rets.resize(128);
				for(int i=(old+1);i<128;i++)
				{
					rets[i] = rand()%255;
				}
				rets[old] = 0;
			}

			Phantom::EncryptData(&rets.at(0), 128, rnd, val);
			fwrite(&val, sizeof(Phantom::EncryptVal), 1, f);
			if(tableBegin > 0){
				Phantom::EncryptData(&rets.at(0) + tableBegin, rets.size()-tableBegin, rnd, val);
				fwrite(&val, sizeof(Phantom::EncryptVal), 1, f);
			}
			fwrite(&rets.at(0), rets.size(), 1, f);
			fclose(f);
		}
	}
	else if(stricmp(szExt, ".rpgui") == 0)
	{
		strcat(szLockFile, ".lui");
		char head[4] = {'l','o','c','u'};
		if(bReqWrite)
			WriteLock(rndSave, rnd, rets, head, szLockFile);
	}
	else if(stricmp(szExt, ".rpgmesh") == 0)
	{
		strcat(szLockFile, ".lmesh");
		char head[4] = {'l','o','c','m'};
		if(bReqWrite)
		WriteLock(rndSave, rnd, rets, head, szLockFile);
	}
	else if(stricmp(szExt, ".pakui") == 0)
	{
		strcat(szLockFile, ".pui");
		char head[4] = {'p','o','c','u'};
		if(bReqWrite)
			WriteLock(rndSave, rnd, rets, head, szLockFile);
	}
	else if(stricmp(szExt, ".pakmesh") == 0)
	{
		strcat(szLockFile, ".pmesh");
		char head[4] = {'p','o','c','m'};
		if(bReqWrite)
		WriteLock(rndSave, rnd, rets, head, szLockFile);
	}
	else if(stricmp(szExt, ".rpgscene") == 0)
	{
		strcat(szLockFile, ".lscene");
		char head[4] = {'l','s','t','o'};
		if(bReqWrite)
		WriteLock(rndSave, rnd, rets, head, szLockFile);
	}
	else if(stricmp(szExt, ".lua") == 0)
	{
		strcat(szLockFile, ".lsc");
		char head[4] = {'l','o','c','l'};
		if(bReqWrite)
		WriteLock(rndSave, rnd, rets, head, szLockFile);
	}
	else if(stricmp(szExt, ".table") == 0 || stricmp(szExt, ".inl") == 0 || stricmp(szExt, ".bin") == 0 || stricmp(szExt, ".dat") == 0 || stricmp(szExt, ".txt") == 0)
	{
		strcat(szLockFile, ".lt");
		char head[4] = {'l','o','c','t'};
		if(bReqWrite)
		WriteLock(rndSave, rnd, rets, head, szLockFile);
	}
	else if(stricmp(szExt, ".rgn") == 0 || stricmp(szExt, ".sec") == 0 || stricmp(szExt, ".rpg2ds") == 0 || stricmp(szExt, ".objects") == 0 || stricmp(szExt, ".rpgss") == 0)
	{
		strcat(szLockFile, szExt);
		if(bReqWrite){
		FILE* f = fopen(szLockFile, "wb");
		CDynamicArray<char> ziped;
		ziped.SetArrayCount(rets.size()+1000000);
		//压缩之后保存
		unsigned long length = ziped.size();
		if(compress((Bytef*)ziped.GetPtr(), &length, (Bytef*)&rets.at(0), rets.size()) != Z_OK)
		{
			GetMainFrame()->MessageBox(Language(L"compress_error", L"压缩数据是产生错误"));
			return false;
		}
		unsigned int ori = rets.size();
		char fhead[128];
		strcpy(fhead, "comp");
		fwrite(fhead, 4, 1, f);
		fwrite(&ori, sizeof(unsigned int), 1, f);
		fwrite(&length, sizeof(unsigned int), 1, f);
		fwrite(ziped.GetPtr(), length, 1, f);//&rets.at(0), rets.size(), 1, f);
		fclose(f);
		}
	}
	else// if(stricmp(szExt, ".wav") == 0 || stricmp(szExt, ".mp3") == 0 || stricmp(szExt, ".ogg") == 0 || stricmp(szExt, ".mp4") == 0)
	{
		strcat(szLockFile, szExt);
		if(bReqWrite){
		FILE* f = fopen(szLockFile, "wb");
		fwrite(&rets.at(0), rets.size(), 1, f);
		fclose(f);
		}
	}
	strcpy(ret, szLockFile + strlen(szReport));
	return true;
}
//

void	enum_files(const char* path,std::vector<FileName>& _out)
{
	int len=(int)strlen(path);
	if(!path||len==0)
		return;
	char szPath[1024],szFind[1024];
	strcpy(szPath,path);
	if(szPath[len-1]!='\\'&&szPath[len-1]!='/')
		strcat(szPath,"\\");
	strcpy(szFind,szPath);
	strcat(szFind,"*.*");
	WIN32_FIND_DATAA wsa;
	HANDLE hFind=FindFirstFileA(szFind,&wsa);
	if(hFind==INVALID_HANDLE_VALUE)
		return;
	while(true)
	{
		//
		if(wsa.cFileName[0]!='.')
		{
			if((wsa.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
			{
				char szNewPath[512];
				strcpy(szNewPath,szPath);
				strcat(szNewPath,wsa.cFileName);
				strcat(szNewPath,"\\");
				//
				enum_files(szNewPath,_out);
			}
			else if(! (wsa.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				std::string sp=szPath;
				sp+=wsa.cFileName;
				_out.push_back(FileName(sp.c_str()));
			}
		}
		//
		if(!FindNextFileA(hFind,&wsa))
			break;
	}
	FindClose(hFind);
}
