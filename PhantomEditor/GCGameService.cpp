/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "GCGameService.h"
#include "GCStateDefault.h"
#include "GCViewManager.h"
#include "io.h"
#include "../PhantomEngine/encrypt.h"
#include "MainFrm.h"
#include "IDEHttp.h"
#include <zlib.h>
extern	const char*	GetAuthKey();
extern void	ToUtf8(char* ret, const char* buf);
extern void	enum_files(const char* path,std::vector<FileName>& _out);
extern BOOL	IsCanReport(const char* szFileNameIn);
extern const char*		GetDeviceName();
extern int				GetUserID();
extern const char*	GetAppID();

VOID		AddLogUpdate(int channel, const char* szText)
{
	char buf[1024];
	char buf2[1024];
	ToUtf8(buf2, szText);
	sprintf(buf, "log:system->%s", buf2);
	GetMainFrame()->m_dlgLog.AddLog(channel, buf);
}
extern	int	GetGameWidth();
extern	int	GetGameHeight();
float g_lastUpdate = 0;


GCGameService*	g_service = 0;
GCGameService*	GetGameService()
{
	if(!g_service)
		g_service = new GCGameService();
	return g_service;
}
GCGameService::GCGameService(void)
{
	m_phone = 0;
}


GCGameService::~GCGameService(void)
{
}

VOID	OnNetLink(DWORD linkid)//某个客户端连接时被调用
{
}
VOID	GCGameService::RemoveClient(DWORD linkid)
{
	char buf[512];
	sprintf(buf, "log:system->error:%s", Autf8(Language(L"disconn", L"断开连接")).buf);
	GetMainFrame()->m_dlgLog.AddLog(buf);
	if(g_service->m_phone&&g_service->m_phone->link == linkid){
		delete g_service->m_phone;
		g_service->m_phone = 0;
	}
}

VOID	OnNetDislink(DWORD linkid)//某个客户端断开连接时被调用
{
	g_service->RemoveClient(linkid);
}
VOID	OnNetMessage(DWORD linkid, WORD wID, const char* dataPtr, unsigned int wSize)//某个客户端处理消息时被调用
{
	g_service->OnMessage(wID, dataPtr, wSize, linkid);
}

VOID	GCGameService::StartServer()
{
	if(Phantom::g_server)
		Phantom::g_server->StopServer();
	Phantom::SetupSocket(OnNetLink, OnNetDislink, OnNetMessage, 0x12345678);
	if(Phantom::StartServer("Device", SVR_PORT))
	{
		Phantom::g_server->SetMaxLinks(100);
		LogInfo("Server Started!");
	}
	else
		LogInfo("Server Fail!");
}
VOID	GCGameService::StopServer()
{
	if(m_phone)
		delete m_phone;
	m_phone = 0;
	if(Phantom::g_server)
		Phantom::g_server->StopServer();
}
extern wchar_t *	GetProjectName();
char	g_mainlua[128] = {0};
VOID	SetMainLua(const char* file)
{
	strcpy(g_mainlua, file);
}

VOID	EnableReset(BOOL b){
	if(GetMainFrame()->m_dlgLog.m_hWnd&&GetMainFrame()->m_dlgLog.GetDlgItem(IDC_RESET_Games))
		GetMainFrame()->m_dlgLog.GetDlgItem(IDC_RESET_Games)->EnableWindow(b);
}
int	g_bForceDelete = false;
VOID	SetForceDelete(BOOL b)
{
	g_bForceDelete = b;
}
VOID	GCGameService::ResetAll()
{
	PhoneClient* phone = m_phone;
	if(!m_phone)return;
	EnableReset(false);
	phone->m_bRequestAuth	=	false;
	phone->m_bRequest	=	false;
	wchar_t wdrive[256], wdir[256];
	_wsplitpath(_wpgmptr, wdrive, wdir, 0, 0);
	wcscat(wdrive, wdir);
	wchar_t szIni[256];
	wcscpy(szIni, wdrive);
	wcscat(szIni, L"Phantom.ini");
	char path[256];
	unsigned char name[256];
	int len = ::WideCharToMultiByte(CP_UTF8, 0, GetProjectName(), -1, (char*)name, 256, 0, 0);
	name[len] = 0;
	path[0] = 0;
	int plen = 0;
	for(int i=0;i<len;i++)
	{
		if(name[i]>=0x80)
		{
			char temp[32];
			sprintf(temp, "%x", name[i]);
			int tlen = strlen(temp);
			for(int j=0;j<tlen;j++)
				path[plen++] = temp[j];
		}
		else
			path[plen++] = name[i];
	}
	path[plen++] = 0;
	//strcpy(guid, ToUtf8((GetProjectName()).buf);
	char* buf = new char[SOCKET_PACKAGE_MAX];
	streamWrite w(buf, SOCKET_PACKAGE_MAX);
	FileName proj;
	FileName dir;
	FileName mainlua = g_mainlua;
	proj = (char*)name;
	dir = path;
	w << proj;
	w << dir;
	w << mainlua;
	w << g_bForceDelete;
	g_bForceDelete = false;
	int numFile = 1;
	w << numFile;
	Phantom::Send(phone->link, MsgID_RequestRun, w.m_ptr, w.m_point);
	delete[] buf;
}

VOID	GCGameService::OnSendFile(PhoneClient* phone, ReqUploadFile& ff)
{
	const char* szExistFile = ff.realPath.t;
	const char* szFile = ff.file.t;
	char* buf = new char[SOCKET_PACKAGE_MAX];
	FILE* f = fopen(szExistFile, "rb");
	//Phantom::StreamBase* s = open_s(szExistFile);
	if(f)
	{
		streamWrite w(buf, SOCKET_PACKAGE_MAX);
		FileName file = szFile;
		w << file;
		int len = filelength(_fileno(f));//s->size();
		w << len;
		int index = phone->m_nMaxFileUpdate-phone->m_uploadFiles.size();
		w << index;
		Phantom::Send(phone->link, MsgID_SendFileBegin, w.m_ptr, w.m_point);
		int count = (len / SOCKET_PACKAGE_MAX);
		if((len%SOCKET_PACKAGE_MAX)>0)count++;
		int rl = 0;
		for(int i=0;i<count;i++)
		{
			int wlen = SOCKET_PACKAGE_MAX;
			if((rl+wlen)>len)
				wlen = len - rl;
			int rlen = fread(buf, wlen, 1, f);//s->read(buf, SOCKET_PACKAGE_MAX);
			Phantom::Send(phone->link,MsgID_SendFileData, buf, wlen);
			rl = rl + SOCKET_PACKAGE_MAX;
			if(rl>=len)
				break;
		}
		Phantom::Send(phone->link, MsgID_SendFileEnd, (const char*)&ff.info, sizeof(FileInfo));
		//close_s(&s);
		fclose(f);
	}
	delete[] buf;
}

VOID	GCGameService::OnFrameMove()
{
	if(m_phone){
		PhoneClient* phone = m_phone;
		if(phone->m_bRequestAuth){
			if(phone->m_uploadFiles.size()>0){
				OnSendFile(phone, phone->m_uploadFiles[0]);
				phone->m_uploadFiles.erase(phone->m_uploadFiles.begin());
			}
			else
			{
				//char* buf = new char[SOCKET_PACKAGE_MAX];
				//streamWrite w(buf, SOCKET_PACKAGE_MAX);
				//FileName f;
				//f = GetHttpKey();
				//w.write(&f, sizeof(FileName));
				//int oldV = PHANTOM_ENGINE_VERSION;//::GetProfileIntA("PhantomV1", "version", 1900);//最新版本1900
				//w.write(&oldV, sizeof(int));
				//int orientation = (GetGameWidth()>GetGameHeight());
				//w.write(&orientation, sizeof(int));
				//FileName appid = GetAppID();
				//w.write(&appid, sizeof(FileName));
				//Phantom::Send(phone->link, MsgID_RequestAuth, w.m_ptr, w.m_point);
				//AddLogUpdate(phone->channel, Language("request_auth", "正在请求用户连接..."));
				//EnableReset(true);
				//phone->m_bRequestAuth = false;
				//delete[] buf;
			}
		}
	}
}
VOID	GCGameService::OnMessage(WORD wID, const char* dataPtr, unsigned int wSize, DWORD linkid)
{
	Phantom::SocketID id;
	id.index = LWORD(linkid);
	id.id = HWORD(linkid);
	streamRead r((char*)dataPtr, wSize);
	PhoneClient* phone = m_phone;
	if(wID == MsgID_DeviceName)
	{
		if(m_phone)
			Phantom::CloseWait(m_phone->link, 1000);
		m_phone = 0;
		int userid = 0;
		r >> userid;
		Phantom::FileName fname;
		r >> fname;
		m_phone = new PhoneClient();
		m_phone->link = linkid;
		m_phone->channel = 0;
		m_phone->deviceName = fname.t;
		AddLogUpdate(m_phone->channel, format("%s(%s)",Language("device_link", "成功连接设备"), fname.t));
		//conn->channel = GetMainFrame()->m_dlgLog.AddChannel(conn->deviceName);
		ResetAll();
	}
	else if(!m_phone)
		return;
	
	switch(wID)
	{
	case MsgID_Exit:
		{
			Phantom::CloseWait(linkid, 1000);
		}
		break;
	case MsgID_RequestRun://服务器同意了，可以运行了
		{
			char szProj[512];
			sprintf(szProj, "../reportbin/");//%s/", GetAppID());;
			int pathlen = strlen(szProj);
			phone->m_uploadFiles.clear();
			phone->m_uploadTemps.clear();
			std::vector<FileName> files;
			enum_files(szProj, files);
			char* buf = new char[SOCKET_PACKAGE_MAX];
			GetMainFrame()->m_dlgLog.GetDlgItem(IDC_RESET_Games)->EnableWindow(false);
			for(int i=0;i<files.size();i++)
			{
				const char* szFile = files[i].t + pathlen;
				if(IsCanReport(files[i].t))
				{
					HANDLE hFile=::CreateFileA(files[i].t, GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
					if(hFile == INVALID_HANDLE_VALUE)
						continue;
					DWORD dwHigh = 0;
					DWORD dwSize = ::GetFileSize(hFile, &dwHigh);
					FILETIME c, l, w;
					::GetFileTime(hFile, &c, &l, &w);
					CloseHandle(hFile);
					FileInfo info;
					info.length = dwSize;
					info.w.dwHighDateTime = w.dwHighDateTime;
					info.w.dwLowDateTime = w.dwLowDateTime;
					strcpy(info.file, szFile);
					FileName temp;
					temp = szFile;
					temp.setIsFile();
					strcpy(info.file, temp);
					//if(!ReqRunFiles(info))
					//	continue;
					ReqUploadFile r;
					r.info = info;
					r.file = szFile;
					r.realPath = files[i].t;
					phone->m_uploadTemps.push_back(r);
					//OnSendFile(files[i].t, szFile);
				}
			}
			int count = phone->m_uploadTemps.size();
			int numSend = count / 500;
			if((count%500)>0)
				numSend++;
			for(int is=0;is<numSend;is++){
				streamWrite w2(buf, SOCKET_PACKAGE_MAX);
				int begin = is*500;
				int count2 = phone->m_uploadTemps.size() - begin;
				if(count2>500)count2 = 500;
				w2 << count2;
				for(int i=0;i<count2;i++){
					w2 << phone->m_uploadTemps[i+begin].info;
				}
				Phantom::Send(phone->link, MsgID_FileListItem, w2.m_ptr, w2.m_point);
			}
			delete[] buf;
			Phantom::Send(phone->link, MsgID_FileListOK, 0, 0);
			phone->m_nMaxFileUpdate=0;
		}
		break;
	case MsgID_FileListItem:
		{
			int count = 0;
			r >> count;
			for(int k=0;k<count;k++){
				FileInfo info;
				r >> info;
				for(int i=0;i<phone->m_uploadTemps.size();i++)
				{
					if(stricmp(phone->m_uploadTemps[i].info.file, info.file)==0)
					{
						phone->m_uploadFiles.push_back(phone->m_uploadTemps[i]);
						break;
					}
				}
			}
		}
		break;
	case MsgID_FileListOK:
		{
			phone->m_nMaxFileUpdate=phone->m_uploadFiles.size();
			AddLogUpdate(phone->channel, format("%s:%d", Language("update_count", "总共需要更新文件数量"), phone->m_nMaxFileUpdate));
			phone->m_bRequest = true;
			phone->m_bRequestAuth = true;
		}
		break;
	case MsgID_RequestAuthFail:
		{
			if(phone->m_bRequest){
				phone->m_bRequest = false;
				MessageBox(GetDeviceWindow(), Language(L"NoAuth", L"没有通过授权审核，可能是服务器没有相应，或者授权已经过期，请到http://www.aixspace.com找技术人员咨询"), Language(L"AuthError", L"授权失败"), MB_OK);
			}
			AddLogUpdate(phone->channel, Language("request_authfail", "请求连接失败!"));
			Phantom::CloseWait(phone->link, 0);
			EnableReset(true);
		}
		break;
	case MsgID_SendFileEnd:
		{
			int index;
			r >> index;
			FileName file;
			r >> file;
			AddLogUpdate(phone->channel, format("%s(%s)(%d/%d)", Language("update_fok", "成功更新文件"), file.t, index, phone->m_nMaxFileUpdate));
		}
		break;
	case MsgID_UpdateFileOK:
		{
		}
		break;
	case MsgID_RequestAuth:
		{
			AddLogUpdate(phone->channel, Language("request_authok", "有效用户验证成功!"));
			EnableReset(true);
		}
		break;
	case MsgID_LogMessage:
		{
			GetMainFrame()->m_dlgLog.AddLog(phone->channel, (const char*)dataPtr);
		}
		break;
	}
}
