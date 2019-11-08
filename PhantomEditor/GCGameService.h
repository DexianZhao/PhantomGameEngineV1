/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include "GCDefine.h"
#include "IDESocket.h"
#include "../AllGames/base/ShareID.h"
#include "PhantomSocket.h"

struct ReqUploadFile{
	FileInfo		info;
	Phantom::NameT<512>		realPath;
	Phantom::FileName		file;
};

struct	PhoneClient{
	DWORD						link;
	int							channel;
	Phantom::Name						deviceName;
	std::vector<ReqUploadFile>	m_uploadFiles;
	std::vector<ReqUploadFile>	m_uploadTemps;
	int							m_nMaxFileUpdate;
	BOOL						m_bRequest;
	BOOL						m_bRequestAuth;
	PhoneClient(){
		link = 0;
		channel = 0;
		m_nMaxFileUpdate = 0;
		m_bRequest = false;
		m_bRequestAuth = false;
	}
};

class GCGameService
{
public:
	GCGameService(void);
	~GCGameService(void);
public:
	VOID					StartServer();
	VOID					StopServer();
	virtual	VOID			OnMessage(WORD wID, const char* dataPtr, unsigned int wSize, DWORD link);
	VOID					ResetAll();
	VOID					OnDisconnect();
	//
	VOID					RemoveClient(DWORD linkid);
	VOID					OnSendFile(PhoneClient* phone, ReqUploadFile& file);
	VOID					OnFrameMove();
	PhoneClient*			m_phone;
};

GCGameService*				GetGameService();
