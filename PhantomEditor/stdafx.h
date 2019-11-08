
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持
#include <afxdhtml.h>




#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#

struct Wutf8
{
	wchar_t* buf;
	Wutf8(const char* w)
	{
		int slen = (int)strlen(w);
		int len = (int)slen * 3 + 1;
		buf = new wchar_t[len];
		len = ::MultiByteToWideChar(CP_UTF8, 0, w, slen, buf, len);
		buf[len] = 0;
	}
	inline	operator const wchar_t*(){return buf;}
	~Wutf8(){if(buf)delete[] buf;}
};

struct Autf8
{
	char* buf;
	Autf8(const wchar_t* w)
	{
		int slen = (int)wcslen(w);
		int len = (int)slen * 3 + 1;
		buf = new char[len];
		len = ::WideCharToMultiByte(CP_UTF8, 0, w, slen, buf, len, 0, 0);
		buf[len] = 0;
	}
	inline	operator const char*(){return buf;}
	~Autf8(){if(buf)delete[] buf;}
};


struct W
{
	wchar_t* buf;
	W(const char* w)
	{
		int slen = (int)strlen(w);
		int len = (int)slen * 3 + 1;
		buf = new wchar_t[len];
		len = ::MultiByteToWideChar(CP_ACP, 0, w, slen, buf, len);
		buf[len] = 0;
	}
	inline	operator const wchar_t*(){return buf;}
	~W(){if(buf)delete[] buf;}
};

struct A
{
	char* buf;
	A(const wchar_t* w)
	{
		int slen = (int)wcslen(w);
		int len = (int)slen * 3 + 1;
		buf = new char[len];
		len = ::WideCharToMultiByte(CP_ACP, 0, w, slen, buf, len, 0, 0);
		buf[len] = 0;
	}
	inline	operator const char*(){return buf;}
	~A(){if(buf)delete[] buf;}
};

struct	format//格式化ansi字符串
{
	char		szText[1024];
	inline	format(const char* szFmt,...){_vsnprintf(szText,1024,szFmt,(va_list)(&szFmt+1));}
	inline	operator	char*	(){return szText;}
};
struct	formatw//格式化字符串(宽字节)
{
	wchar_t		szText[1024];
	inline	formatw(const wchar_t* szFmt,...){_vsnwprintf(szText,1024,szFmt,(va_list)(&szFmt+1));}
	inline	operator	wchar_t*	(){return szText;}
};

class base_reg//注册表操作类
{
public:
	base_reg();
	~base_reg();
	bool	openKey(HKEY hKey, const char* szSubKey);//打开一个注册表项
	bool	createKey(HKEY hKey, const char* szSubKey);//创建一个注册表项
	void	close();//关闭打开的注册表

	int		setBinaryValue(const char* szName, const char* binary, int length);//读取二进制数据
	int		getBinaryValue(const char* szName, char* binary, int length);//读取二进制数据
	bool	addTextValue(const char* szName, const char* szValue);//根据当前打开的注册表项写入一段文本
	bool	addIntValue(const char* szName, int nValue);//根据当前打开的注册表项写入一个数据
	
	HKEY	m_hKey;
};

extern	VOID	WriteRegValue(const char* szKey, const char* szText);
extern	BOOL	ReadRegValue(const char* szKey, char* szText);
extern	BOOL	GetCopyKeyboard();
extern	BOOL	GetSelectKeyboard();
extern	BOOL	IsKeyDown(int key);
extern	void			SetProjectPath( const char* szBuf );
extern	void			SetProjectPath();
extern	const char*		GetProjectPath();
extern	HWND	GetDeviceWindow();

#include "PhantomManager.h"
using namespace Phantom;
