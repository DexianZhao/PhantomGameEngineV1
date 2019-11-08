//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_HTTP_H__
#define __PHANTOM_HTTP_H__
#ifdef WIN32
#include <Windows.h>
#endif

#include <vector>
#include <string>
#include "PhantomMath.h"
#include "PhantomBase.h"
#include "PhantomSocket.h"
#define	PLATFORM_OSX		1
#define	PLATFORM_ANDROID	2
namespace Phantom{
	class HttpRequest;
	struct	JsonValue
	{
		JsonValue(){}
		NameT<64>		name;//名字，最大不能超过64个字
		NameT<4096>		svalue;
		//各种判断
		inline	operator int(){return atoi(svalue.t);}
		inline	operator const char* (){return svalue.t;}
		inline	BOOL	operator != (int nValue){if(!name.t[0])return false; return (atoi(svalue.t) != nValue);}
		inline	BOOL	operator != (const char* textValue){if(!name.t[0])return false; return (stricmp(textValue, svalue.t) != 0);}//判断是否相同字符串
		inline	BOOL	operator == (const char* textValue){if(!name.t[0])return false; return (stricmp(textValue, svalue.t) == 0);}//判断是否相同字符串
		inline	BOOL	operator == (int nValue){if(!name.t[0])return false; return (atoi(svalue.t) == nValue);}
		//
		inline	void	operator = (int n){sprintf(svalue.t, "%d", n);}
		inline	void	operator = (const char* szText){svalue = szText;}
		//
		//inline	BOOL	operator == (double dValue){if(!name.t[0])return false; return (atof(svalue.t) == dValue);}
		//inline	void	operator = (double n){sprintf(svalue.t, "%.05f", d);}
	};
	class	JsonObject
	{
	public:
		JsonObject();
		~JsonObject();
		JsonValue&	operator [](const char* szName);
		BOOL		exist(const char* szName);
		JsonObject&	add(const char* szName, const char* szValue);
		JsonObject&	add(const char* szName, int nValue);
		//
		inline	void		Reset(){m_numValues = 0;}
		inline	int			size(){return m_numValues;}
		inline	JsonValue*	at(int nIndex){if(nIndex < 0 || nIndex>=m_numValues)return 0; return &m_values[nIndex];}
		//
		JsonValue&			NewValue();
		VOID				BuildText(char* ret);//构造字符串
	protected:
		int							m_numValues;
		CDynamicArray<JsonValue>	m_values;
	};

#define	jval(var,name)	var[#name]

	class IJsonAnalyseCallback
	{
	public:
		virtual	VOID	OnJsonAnalyse(JsonObject& obj) = 0;
		virtual	VOID	OnJsonAnalyseFail(const char* type, const char* szFailText) = 0;
	};
	template <class T>
	class JsonAnalyseCallback: public IJsonAnalyseCallback
	{
	public:
		T*m_t;
		JsonAnalyseCallback(T* t){m_t = t;}
		VOID		SetPtr(T* t){m_t = t;}
		virtual	VOID	OnJsonAnalyse(JsonObject& obj){if(m_t)m_t->OnJsonAnalyse(obj);}
		virtual	VOID	OnJsonAnalyseFail(const char* type, const char* szFailText){if(m_t)m_t->OnJsonAnalyseFail(type, szFailText);}
	};
	class HttpRequest: public ThreadProc
	{
	public:
		HttpRequest();//构造函数
		virtual ~HttpRequest();//析构函数
		virtual	VOID					Initialize(const char* szJsonPHPUrl);
		inline	VOID					SetUseThread(BOOL b){m_bUseThread = b;}//设置是否使用线程来发送
		inline	BOOL					IsUseThread(){return m_bUseThread;}
		virtual BOOL					call(const char* szFormat, ...);//连接服务器,并发送一段json数据
		virtual BOOL					call(JsonObject& obj);//连接服务器,并发送一段json数据
		virtual BOOL					request(const char* requestText);//连接服务器,并发送一段json数据
		virtual	VOID					cancel();
		virtual	const char*				GetRecvText(){return m_recvBuffers.GetPtr();}
		//
		template <class T>
		inline	VOID					SetCallback(T* t){if(m_pJsonCallback)delete m_pJsonCallback; m_pJsonCallback = new JsonAnalyseCallback<T>(t);}
		virtual	VOID					DoMessageQueue();
		//
		virtual	BOOL					RequestPage();	//需求某个页面执行
		virtual	BOOL					Connect(const char* szUrl);
		virtual	VOID					OnThread();
		//
		friend class					Manager;
		//基础接口
		BOOL							OnParseJson(const char* szText);
		int								m_nResult;
		JsonObject						m_tempJson;
		BOOL							m_bUseThread;
		Socket							m_hSocket;
		IJsonAnalyseCallback*			m_pJsonCallback;
		char							m_szHost[128];//设置地址
		char							m_szPage[256];//设置地址
		NameT<4096>						m_requestText;
		NameT<65536>					m_jsonCode;
		CDynamicArray<char>				m_recvBuffers;
		int								m_jsonCount;
		BOOL							m_bFail;
		PhantomLock						m_lock;
		BOOL							m_bRequestComplete;
	};
	class	AuthHttpRequest: public HttpRequest, public IJsonAnalyseCallback
	{
	public:
		AuthHttpRequest();
		~AuthHttpRequest();
		virtual	VOID					OnThread();
		virtual	VOID					OnJsonAnalyse(JsonObject& obj);
		virtual	VOID					OnJsonAnalyseFail(const char* type, const char* szFailText);
		BOOL							m_bRequest;
	};
	typedef void (*fnHttpCallback)(Name& file, JsonObject& obj, const char* errorText, void* param);//如果errorText==0则表示没有任何错误发生
	VOID			RequestIP(fnHttpCallback cb, JsonObject& params, const char* szHome, void* param);
	void			SetServerIP(const char* ip);
	VOID			Request(const char* infile, JsonObject& params, fnHttpCallback cb, BOOL bUseThread = false, void* param = 0);//请求数据
	///
	VOID			OnDownloadFrame();
	char*			urldecode(char* inOut);//对文本进行反编
	bool			urlencode(char* ret, int bufferLength, const char* sIn, int len = 0);
	//下载文件
	typedef	VOID	(*PDownloadComplte)(const char* requestText, const char* indocFile, BOOL bError, char* buffer, int length);//bError是否有错误
	BOOL			downloadFile(const char* requestText, const char* indocFile,PDownloadComplte callback = 0, BOOL bUseThread = true, BOOL bInMemory = false);//从网络下载文件
};
#endif
