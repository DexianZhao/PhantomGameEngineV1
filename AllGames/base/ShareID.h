#ifndef __SHARE_ID_H__
#define	__SHARE_ID_H__

enum{
	MsgID_RequestRun	=	1,//请求运行
	MsgID_SendFileBegin,//开始传送某个文件,里面包括文件信息，相对路径，包数量等。
	MsgID_SendFileData,//传送文件数据
	MsgID_SendFileEnd,//某个文件传送结束
	MsgID_RequestAuth,//验证授权码，此时将从服务器获得授权码，获得授权码之后直接运行
	MsgID_RequestAuthFail,//获取验证码错误
	MsgID_LogMessage,//发送日志信息
	MsgID_UpdateFileOK,//文件更新完毕提醒
	MsgID_ResetServer,//文件更新完毕提醒
    MsgID_DeviceName,
	MsgID_FileListItem,//文件名字更新
	MsgID_FileListOK,
	MsgID_Exit,
};

#define	SVR_PORT	16168

#pragma pack(push, 1)
struct FileTime{
    unsigned int dwLowDateTime;
    unsigned int dwHighDateTime;
};
struct	FileInfo
{
	char		file[128];
	FileTime	w;
	int			length;
	char		bMustUpdate;
};
#pragma pack(pop)


#endif
