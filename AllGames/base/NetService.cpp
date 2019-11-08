#include "NetService.h"
#include "PhantomDB.h"
#ifdef IPHONE_OS
#include "encrypt.h"
#else
#include "../PhantomEngine/encrypt.h"
#endif
int			NetService::s_userID = 0;
NetService*	g_netservice = 0;
NetService*							GetNetService()
{
	if(!g_netservice)
		g_netservice = new NetService();
	return g_netservice;
}

NetService::NetService()
{
	m_index				=	-1;
    m_projectRun		=	false;
	this->m_begin       =	0;
    m_orientation       =   0;
	m_bOKRun			=	false;
	m_bConn				=	false;
	m_dwClient			=	0;
}
extern	const char*	GetRealDocuments();
extern	VOID		RunDocumentProj(const char* proj, unsigned int* guid, const char* szMainLua, int orientation);
extern	GameState*	GetManagerState();
extern VOID MyLogDebug(const char* szText);

VOID	OnNetLink(DWORD linkid)//某个客户端连接时被调用
{
	LogInfo("Connect OK");
	GetNetService()->SendInit();//发送初始化消息
}
VOID	OnNetDislink(DWORD linkid)//某个客户端断开连接时被调用
{
	GetNetService()->m_bConn = false;
	LogInfo("Disconnect");
	//gotoMain();
}
VOID	OnNetMessage(DWORD linkid, WORD wID, const char* dataPtr, unsigned int wSize)//某个客户端处理消息时被调用
{
	SocketID id;
	id.index = LWORD(linkid);
	id.id = HWORD(linkid);
	g_netservice->OnMessage(wID, dataPtr, wSize, id);
}
namespace Phantom{
	Name	g_serverIP;
	VOID	SetServerConnectIP(const char* ip){
		g_serverIP = ip;
		LogInfo("ServerIP:%s", ip);
	}
	VOID	SearchComplete(const char* szIP)
	{
		if(szIP){
			g_serverIP = szIP;
			GetNetService()->SendExit();//CloseWait(GetNetService()->m_dwClient, 0);
			LogInfo("ConnectIP:%s", g_serverIP.t);
			GetNetService()->m_dwClient = StartClient(szIP, SVR_PORT, OnNetLink, OnNetDislink, OnNetMessage, 0x12345678);
		}
		else{
		}
	}
};
void	StopClient()
{
	LogInfo("SendExit");
	GetNetService()->SendExit();
}
extern "C"{
    extern const char* GetDeviceName(char*ch);
    extern const char* onAlert(const char* message, const char* title, const char* type);
    void    saveConfig(const char* key, const char* value);
    int     loadConfig(const char* key,char* retvalue);
	VOID	SetUserID(int id){
		NetService::s_userID	=	id;
	}
	VOID	RestartServer()
	{
		if(g_serverIP.t[0]){
			GetNetService()->SendExit();//CloseWait(GetNetService()->m_dwClient, 0);
			GetNetService()->m_dwClient = StartClient(g_serverIP, SVR_PORT, OnNetLink, OnNetDislink, OnNetMessage, 0x12345678);
		}
		else{
			SearchServers(SearchComplete, SVR_PORT);
		}
	}
};

VOID	NetService::InitRunFiles()
{
	m_requestFiles.clear();
	INT nVersion = 1;
	char buf[512];
	sprintf(buf, "%s%supdatelist", GetRealDocuments(), m_project.t);
	FILE* f = fopen(buf, "rb");
	if(!f)return;
	unsigned char head[4];
	fread(head, 4, 1, f);
	if(head[0] != 'u'||head[1] != 'p'||head[2] != 'l'||head[3] != 't')
	{
		fclose(f);
		removeFile(buf);
		return;
	}
	int count;
	fread(&nVersion, sizeof(int), 1, f);
	fread(&m_projectName, sizeof(FileName), 1, f);
	if(nVersion>1)
	{
		fread(&m_project, sizeof(FileName), 1, f);
		fread(&m_orientation, sizeof(int), 1, f);
	}
	Time tm;
	if(nVersion>=3)
	{
		fread(&tm, sizeof(Time), 1, f);
        if(nVersion>=4){
            int tempRun;
            fread(&tempRun, sizeof(int), 1, f);
        }
		int userID;
		fread(&userID, sizeof(int), 1, f);
	}

	fread(&count, sizeof(int), 1, f);
	if(count > 0){
		m_requestFiles.SetArrayCount(count);
		fread(m_requestFiles.GetPtr(), sizeof(FileInfo)*m_requestFiles.size(), 1, f);
	}
	fclose(f);
}
BOOL	NetService::ReadProjectName(const char* szFile, FileName& projectName, FileName& projectPath, Name& stm, int& ori)
{
	INT nVersion = 1;
	FILE* f = fopen(szFile, "rb");
	if(!f)
	{
		LogInfo("RPN:1");
		return false;
	}
	unsigned char head[4];
	fread(head, 4, 1, f);
	fread(&nVersion, sizeof(int), 1, f);
	if(nVersion<=1 || (head[0] != 'u'||head[1] != 'p'||head[2] != 'l'||head[3] != 't'))
	{
		LogInfo("RPN:2");
		fclose(f);
		return false;
	}
	int count;
	fread(&projectName, sizeof(FileName), 1, f);
	fread(&projectPath, sizeof(FileName), 1, f);
	fread(&ori, sizeof(int), 1, f);
	Time tm;
	if(nVersion>=3)
	{
		fread(&tm, sizeof(Time), 1, f);
		sprintf(stm.t, "%d-%02d-%02d %02d:%02d:%02d", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);
        if(nVersion>=4){
            int tempRun;
            fread(&tempRun, sizeof(int), 1, f);
            if(tempRun==0)
			{
				LogInfo("RPN:3");
                return false;
			}
        }
		int userid = 0;
		fread(&userid, sizeof(int), 1, f);
		if(userid != NetService::s_userID)
		{
			fclose(f);
			LogInfo("RPN:4");
			return false;
		}
	}
	fclose(f);
	return true;
}
VOID	NetService::SaveRunFiles()
{
	INT nVersion = 4;
	char buf[512];
	sprintf(buf, "%s%supdatelist", GetRealDocuments(), m_project.t);
	FILE* f = fopen(buf, "wb");
	if(!f)return;
	unsigned char head[4];
	head[0] = 'u';
	head[1] = 'p';
	head[2] = 'l';
	head[3] = 't';
	int count = m_requestFiles.size();
	fwrite(head, 4, 1, f);
	fwrite(&nVersion, sizeof(int), 1, f);
	fwrite(&m_projectName, sizeof(FileName), 1, f);
	fwrite(&m_project, sizeof(FileName), 1, f);
	fwrite(&m_orientation, sizeof(int), 1, f);
	Time tm;
	GetLocalTime(tm);
	fwrite(&tm, sizeof(Time), 1, f);
	fwrite(&m_bOKRun, sizeof(int), 1, f);
	fwrite(&NetService::s_userID, sizeof(int), 1, f);
	fwrite(&count, sizeof(int), 1, f);
	fwrite(m_requestFiles.GetPtr(), sizeof(FileInfo)*m_requestFiles.size(), 1, f);
	fclose(f);
}
BOOL	NetService::ReqRunFiles(FileInfo& info){
	for(int i=0;i<m_requestFiles.size();i++)
	{
		if(stricmp(m_requestFiles[i].file, info.file) == 0)
		{
            FileInfo& local = m_requestFiles[i];
			if(local.bMustUpdate)
				return true;
			if(local.w.dwHighDateTime == info.w.dwHighDateTime && local.w.dwLowDateTime == info.w.dwLowDateTime)
				return false;
            local.bMustUpdate=true;
            local.w=info.w;
			return true;
		}
	}
	info.bMustUpdate = true;
	m_requestFiles.push_back(info);
	return true;
}

extern VOID DeviceLogPrint(const char* szText);
VOID					NetService::SendInit()
{
	if(m_dwClient==0)
		return;
	m_bConn	=	true;
	char buf[1024];
	streamWrite w(buf, SOCKET_PACKAGE_MAX);
	int userid = 0;
	w << NetService::s_userID;
	FileName fname;
	GetDeviceName(fname.t);
	w << fname;
	Send(m_dwClient, MsgID_DeviceName, (const char*)w.m_ptr, w.m_point);
}
VOID					NetService::SendExit()
{
	if(m_dwClient==0)
		return;
	m_bConn	=	true;
	Send(m_dwClient, MsgID_Exit, 0, 0);
	m_dwClient = 0;
}
#define	USE_LOCAL_AUTH
VOID					NetService::OnMessage(WORD wID, const char* dataPtr, unsigned int wSize, const SocketID& id)
{
	streamRead r((char*)dataPtr, wSize);
	switch(wID)
	{
    case MsgID_DeviceName:
        {
//			int userid = 0;
//			r >> userid;
//#ifndef WIN32_OS
//			if(NetService::s_userID!=userid){
//				m_server.Send(id, MsgID_DeviceName, 0, 0);
//				break;
//			}
//#endif
//            FileName fname;
//            GetDeviceName(fname.t);
//            Send(m_dwClient, MsgID_DeviceName, (const char*)&fname, sizeof(FileName));
//			LogInfo("Device Has Ben Connected! %s", fname.t);
//			m_projectID	=	id;
        }
        break;
	case MsgID_RequestRun:
		{
			m_bOKRun	=	false;
			r >> m_projectName;
			r >> m_project;
			r >> m_mainlua;
			r >> m_bForceDelete;
			int numFile = 0;
			r >> numFile;
			char buf[128];
			sprintf(buf, "%x", NetService::s_userID);
			m_project += buf;
			int l = m_project.Length;
			m_project += "/";
			strlwr_t(m_project.t);
			if(m_bForceDelete)
			{
				char file[512];
				sprintf(file, "%s%supdatelist", GetRealDocuments(), m_project.t);
				removeFile(file);
				removeFolders(GetRealDocuments(), m_project.t);
			}
			createFolders(GetRealDocuments(), m_project);
			m_projectRun	=	false;
			LogInfo("Request Run Done", m_project.t);
			m_projectID	=	id;
			InitRunFiles();
			m_recvInfos.clear();
			Send(m_dwClient, MsgID_RequestRun, (const char*)&m_deviceID, sizeof(Name));
		}
		break;
	case MsgID_FileListItem:
		{
			int count = 0;
			r >> count;
			for(int i=0;i<count;i++){
				FileInfo info;
				r >> info;
				if(ReqRunFiles(info))
					m_recvInfos.push_back(info);
			}
		}
		break;
	case MsgID_FileListOK:
		{
			int numSend = m_recvInfos.size()/500;
			if((m_recvInfos.size()%500)>0)
				numSend++;
			char* buf = new char[SOCKET_PACKAGE_MAX];
			for(int is=0;is<numSend;is++){
				streamWrite w2(buf, SOCKET_PACKAGE_MAX);
				int begin = is*500;
				int count = m_recvInfos.size() - begin;
				if(count>500)count = 500;
				w2 << count;
				for(int i=0;i<count;i++){
					w2 << m_recvInfos[i+begin];
				}
				Send(m_dwClient, MsgID_FileListItem, (const char*)w2.m_ptr, w2.m_point);
			}
			delete[] buf;
			Send(m_dwClient, MsgID_FileListOK, 0,0);
		}
		break;
	case MsgID_SendFileBegin:
		{
			r >> m_file;
			m_file.setIsFile();
			int len;
			r >> len;
			r >> m_index;
			m_buffers.SetArrayCount(len);
			m_begin = 0;
		}
		break;
	case MsgID_SendFileData:
		{
			assert((m_begin+wSize)<=m_buffers.size());
			memcpy(m_buffers.GetPtr()+m_begin, dataPtr, wSize);
			m_begin += wSize;
		}
		break;
	case MsgID_SendFileEnd:
		{
			FileInfo info;
			memcpy(&info, dataPtr, sizeof(FileInfo));
			assert(m_begin == m_buffers.size());
			char buf[512];
			sprintf(buf, "%s%s", GetRealDocuments(), m_project.t);
			createFolders(buf, m_file.t);
			sprintf(buf, "%s%s%s", GetRealDocuments(), m_project.t, m_file.t);
			FILE* w = fopen(buf, "wb");
			if(w){
				fwrite(m_buffers.GetPtr(), m_buffers.size(), 1, w);
				fclose(w);
				char* buf = new char[SOCKET_PACKAGE_MAX];
				streamWrite w2(buf, SOCKET_PACKAGE_MAX);
				w2 << m_index;
				w2 << m_file;
				Send(m_dwClient, MsgID_SendFileEnd, (const char*)w2.m_ptr, w2.m_point);
				delete[] buf;
				for(int j=0;j<m_requestFiles.size();j++)
				{
					if(stricmp(m_requestFiles[j].file, info.file) == 0)
					{
						//m_requestFiles[j] = info;
						m_requestFiles[j].bMustUpdate = false;
						SaveRunFiles();
						break;
					}
				}
			}
			m_buffers.clear();
			//LogInfo("File Has Ben Received %s", m_file.t);
		}
		break;
	case MsgID_RequestAuth:
		{
			//m_server.Send(id, MsgID_UpdateFileOK, 0, 0);
			char authpass[4096];// = loadConfig GetManagerState()->readt(this->m_project.t, "");
			FileName f;
			r.read(&f, sizeof(FileName));
            int version = 2002;
            r.read(&version, sizeof(int));
            m_orientation = 0;
            r.read(&m_orientation, sizeof(int));
			r.read(&m_appid, sizeof(FileName));
			if(m_bForceDelete)
				saveConfig(m_project.t, "");
            if(PHANTOM_ENGINE_VERSION<version){
                onAlert("本手机版本低于编辑器版本，需要更新，请进入AppStore更新一下", "版本更新提醒", "ok");
            }
#ifdef USE_LOCAL_AUTH
			Run(0);
#else
			if(m_bForceDelete || !loadConfig(this->m_project.t, authpass) || authpass[0] == 0)
			{
                RequestAuth(f.t, m_appid.t);
			}
			else
			{
				Run(authpass);
			}
#endif
            //
		}
		break;
	case MsgID_ResetServer:
		{
		}
		break;
	default:
		assert(false);
		break;
	}

}

void		md5_2(const char* str, unsigned char* ret);
VOID		NetService::Run(const char* authpass)
{
	if(m_projectRun)
		return;
	std::vector<unsigned char> guid;
	if(authpass){
		HexEncode(guid, authpass);
		if(guid.size() != 512)
		{
			Send(m_dwClient, MsgID_RequestAuthFail, 0, 0);
			saveConfig(m_project.t, "");
			return;
		}
	}
	else{
		guid.resize(512);
		memset(&guid.at(0), 0, guid.size());
	}
	Send(m_dwClient, MsgID_RequestAuth, 0, 0);
	LogInfo("Running %s", m_project.t);
	RunDocumentProj(this->m_project.t, (unsigned int*)&guid.at(0), m_mainlua.t, m_orientation);
	m_projectRun	=	true;
    m_bOKRun	=	true;
	SaveRunFiles();
}
void OnAuthCallback(Name& file, JsonObject& obj, const char* errorText, void* param)
{
    if(errorText)
    {
        onAlert(errorText, "Error", "ok");
        return;
    }
    int result = obj["result"];
    if(result==1){
        const char* szValue = obj["auth"].svalue.t;
		//std::vector<unsigned char> guid;
		//HexEncode(guid, szValue);
		//if(guid.size() == 512)
		//{
		//	unsigned char buf[32];
		//	md5_2(GetNetService()->m_appid, buf);
		//	for(int i=0;i<guid.size();i++){
		//		guid[i] ^= buf[i%16];
		//	}
		//	char tempVal[4096];
		//	memset(tempVal, 0, sizeof(tempVal));
		//	HexDecode(tempVal, &guid.at(0), guid.size());
//		onAlert(szValue, "", "ok");
		saveConfig(GetNetService()->m_project.t, szValue);
		GetNetService()->Run(szValue);
		//}
		//else
		//	onAlert("没有获得授权或者无法连接服务器", "无法真机运行", "ok");
    }
    else{
        onAlert("没有获得授权或者无法连接服务器", "无法真机运行", "ok");
    }
}
VOID		NetService::RequestAuth(const char* keys, const char* appid)
{
    JsonObject obj;
    obj["key"] = keys;
	obj["appid"] = appid;
    Request("pvd_phoneauth", obj, OnAuthCallback, false);
}
