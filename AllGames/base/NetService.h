
#ifndef __NET_SERVICE_H__
#define	__NET_SERVICE_H__
#include "GameManager.h"
#ifndef IPHONE_OS
#include "../../GameAssist/Assist/ShareID.h"
#else
#include "ShareID.h"
#endif

class	NetService
{
public:
	NetService();
	VOID							Connect(const char* szIP);//连接服务器IP
	VOID							SendInit();
	VOID							SendExit();
	virtual	VOID					OnMessage(WORD wID, const char* dataPtr, unsigned int wSize, const SocketID& id);
	VOID							Run(const char* authpass);
	static BOOL						ReadProjectName(const char* szFile, FileName& projectName, FileName& projectPath, Name& stm, int& ori);
protected:
	VOID							RequestAuth(const char* keys, const char* appid);
	CDynamicArray<FileInfo>			m_requestFiles;
	CDynamicArray<FileInfo>			m_recvInfos;
	VOID							InitRunFiles();
	VOID							SaveRunFiles();
	BOOL							ReqRunFiles(FileInfo& info);
public:
	BOOL							IsConn(){return m_bConn;}
	DWORD							m_dwClient;
	Name							m_deviceID;
	FileName						m_projectName;
	FileName						m_project;
	FileName						m_mainlua;
	BOOL							m_projectRun;
	BOOL							m_bConn;
	SocketID						m_projectID;
	FileName						m_file,m_appid;
	int								m_index;
	CDynamicArray<char>				m_buffers;
	int								m_begin;
	static int						s_userID;
	int								m_bOKRun;
    int                             m_orientation;
	int								m_bForceDelete;
};

NetService*							GetNetService();



#endif
