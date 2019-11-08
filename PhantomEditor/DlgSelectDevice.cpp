// DlgSelectDevice.cpp : 实现文件
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "stdafx.h"
#include "DlgSelectDevice.h"
#include "afxdialogex.h"
#include "GCRenderSystem.h"
#include "MainFrm.h"
#include "GCViewManager.h"
#include "GCBitmapMgr.h"
#include "CreateFileDlg.h"
#include "GCViewUIEditor.h"
#include "UIPngMaker.h"
#include "../PhantomEngine/encrypt.h"
#include "DlgSelectDevice.h"
#include "IDESocket.h"
using namespace PhantomV1;
// DlgSelectDevice 对话框

IMPLEMENT_DYNAMIC(DlgSelectDevice, CDialogEx)

DlgSelectDevice::DlgSelectDevice(CWnd* pParent /*=NULL*/)
	: CDialogEx(DlgSelectDevice::IDD, pParent)
{

}

DlgSelectDevice::~DlgSelectDevice()
{
}

void DlgSelectDevice::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_DEVICE_LIST, m_lbDevices);
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgSelectDevice, CDialogEx)
	ON_BN_CLICKED(IDC_RUN, &DlgSelectDevice::OnBnClickedRun)
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_DEVICE_LIST, &DlgSelectDevice::OnLbnDblclkDeviceList)
END_MESSAGE_MAP()

extern int		GetUserID();
BOOL	TestDeviceIP(const char* ip, char* name)
{
	//SOCKET hSocket	=	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//int tv = 3000;//10分钟内如果没有任何消息响应则默认是断开，自动关闭连接
	//setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(int));
	//sockaddr_in addr;
	//memset(&addr,0,sizeof(sockaddr_in));
	//addr.sin_addr.s_addr		=	inet_addr( ip );
	//addr.sin_family					=	AF_INET;
	//addr.sin_port					=	htons( SVR_PORT );
	//int	nError	=	::connect(hSocket, (sockaddr*)&addr, sizeof(sockaddr_in));
	//if(nError != 0){
	//	closesocket(hSocket);
	//	return false;
	//}
	//if(!name)
	//	return true;
	//char bytes[1024];
	//MessageID* msg = (MessageID*)bytes;
	//msg->wID = MsgID_DeviceName;
	//msg->wSize = 4;
	//*(int*)(bytes+sizeof(MessageID)) = GetUserID();//验证长度
	//*(unsigned int*)(bytes+sizeof(MessageID)+sizeof(int)) = 4;//验证长度
	//*(unsigned int*)(bytes+sizeof(MessageID)+sizeof(int)+sizeof(unsigned int)) = 0x12345678;//验证代码
	//send(hSocket, bytes, sizeof(MessageID)+sizeof(int)+sizeof(unsigned int)+sizeof(unsigned int), 0);
	//int rc = recv(hSocket, bytes, 1024, 0);
	//closesocket(hSocket);
	//MessageID* ret = (MessageID*)bytes;
	//if(rc < sizeof(MessageID) || ret->wSize==0)
	//	return false;
	//if(name)
	//{
	//	//strcpy(name, bytes+4);
	//	wchar_t buf[1024];
	//	memset(buf, 0, sizeof(buf));
	//	const char* texts = bytes+sizeof(MessageID);
	//	::MultiByteToWideChar(CP_UTF8, 0, bytes+sizeof(MessageID), strlen(bytes+sizeof(MessageID)), buf, 1024);
	//	int len = ::WideCharToMultiByte(CP_ACP, 0, buf, -1, name, 128, 0, 0);
	//	//name[len] = 0;
	//}

	return true;
}

struct IPs{
	Name		name;
	Name		ip;
};
CDynamicArray<IPs>	g_ips;
//base_critical		g_sec;
IPs		g_deviceIp;
VOID	ClearDeviceIp(){g_deviceIp.ip.t[0] = 0;}
const char*	GetDeviceIp(){return g_deviceIp.ip.t;}
const char*		GetDeviceName()
{
	return g_deviceIp.name.t;
}
BOOL	RunGame()
{
	//if(g_deviceIp.ip.t[0] == 0)
	//	return false;
	//if(!TestDeviceIP(g_deviceIp.ip.t, 0))
	//	return false;
	GetMainFrame()->m_dlgLog.StartRuntime();
	return true;
}
DWORD	WINAPI	ThreadTestServer(LPVOID lpVoid)
{
	//char ip[128];
	//unsigned char* nIP = (unsigned char*)&lpVoid;
	//sprintf(ip, "%d.%d.%d.%d", nIP[0], nIP[1], nIP[2], nIP[3]);
	//char out[128];
	//if(TestDeviceIP(ip,out))
	//{
	//	base_lock l(g_sec);
	//	char buf[1024];
	//	sprintf(buf, "%s", out, ip);
	//	IPs s;
	//	s.name = buf;
	//	s.ip = ip;
	//	g_ips.push_back(s);
	//}
	return 0;
}
VOID	RefreshIPs(){//重新搜索ip段获得正在运行的设备
	//char szHostName[MAX_PATH] = {0};
	//int nRetCode;
	//nRetCode = gethostname(szHostName, sizeof(szHostName));
	//char* lpLocalIP;
	//PHOSTENT hostinfo;
	//if (nRetCode != 0){
	//	return;
	//}
	//{
	//	base_lock l(g_sec);
	//	g_ips.clear();
	//}
	//hostinfo = gethostbyname(szHostName);
	//lpLocalIP = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
	//unsigned int nIP = ((struct in_addr*)*hostinfo->h_addr_list)->S_un.S_addr;
	//for(int i=2;i<200;i++)
	//{
	//	char* cip = (char*)&nIP;
	//	cip[3] = i;
	//	HANDLE hThread	=	CreateThread(0, 0, ThreadTestServer, (LPVOID)nIP, 0, 0);
	//	CloseHandle(hThread);
	//}
}

BOOL DlgSelectDevice::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_nLastCount = 0;
	RefreshIPs();
	SetTimer(0, 100, 0);
	return TRUE;
}

void DlgSelectDevice::OnBnClickedRun()
{
	//if(m_lbDevices.GetCurSel()<0)return;
	//base_lock l(g_sec);
	//g_deviceIp = g_ips[m_lbDevices.GetCurSel()];
	//this->OnOK();
}


void DlgSelectDevice::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//{
	//	if(g_ips.size() > m_nLastCount){
	//		base_lock l(g_sec);
	//		for(int i=m_nLastCount;i<g_ips.size();i++){
	//			m_lbDevices.SetItemData(m_lbDevices.AddString(W(g_ips[i].name.t).buf), i);
	//		}
	//		m_nLastCount = g_ips.size();
	//		if(m_lbDevices.GetCount()>0&&m_lbDevices.GetCurSel()==0)
	//			m_lbDevices.SetCurSel(0);
	//	}
	//}
	CDialogEx::OnTimer(nIDEvent);
}


void DlgSelectDevice::OnLbnDblclkDeviceList()
{
	OnBnClickedRun();
}
