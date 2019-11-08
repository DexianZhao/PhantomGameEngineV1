//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
软件名称   :   幻影游戏引擎, PhantomV1 Game Engine, PhantomV1 Engine（官方网站：www.aixspace.com）
设计编写者 :   赵德贤(英文:Zhao De Xian)
MSN        :   yuzhou_995@hotmail.com
版权所有 赵德贤.  保留所有权利. 在购买幻影游戏引擎正版授权版本之后才可以使用本系统功能. 按照幻影的授权形式，可以获得本系统终身使用授权和无限制项目数量的开发和使用功能。
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __IDE_HTTP_H__
#define __IDE_HTTP_H__
#ifdef WIN32
#include <Windows.h>
#endif

#include <vector>
#include <string>
#include "IDESocket.h"

//namespace PhantomV1{
	
	class MyHttpRequest;
	struct	JsonValue
	{
		JsonValue(){}
		Phantom::NameT<64>		name;//名字，最大不能超过64个字
		Phantom::NameT<2048>		svalue;
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
	};
	class	IDCJsonObject
	{
	public:
		IDCJsonObject();
		~IDCJsonObject();
		JsonValue&	operator [](const char* szName);
		IDCJsonObject&	add(const char* szName, const char* szValue);
		IDCJsonObject&	add(const char* szName, int nValue);
		//
		inline	void		Reset(){m_numValues = 0;}
		inline	int			size(){return m_numValues;}
		inline	JsonValue*	at(int nIndex){if(nIndex < 0 || nIndex>=m_numValues)return 0; return &m_values[nIndex];}
		//
		JsonValue&			NewValue();
		VOID				BuildText(char* ret);//构造字符串
	protected:
		int							m_numValues;
		Phantom::CDynamicArray<JsonValue>	m_values;
	};

	#define	jval(var,name)	var[#name]

	class IJsonAnalyseCallback
	{
	public:
		virtual	VOID	OnJsonAnalyse(IDCJsonObject& obj) = 0;
		virtual	VOID	OnJsonAnalyseFail(const char* type, const char* szFailText) = 0;
	};
	template <class T>
	class JsonAnalyseCallback: public IJsonAnalyseCallback
	{
	public:
		T*m_t;
		JsonAnalyseCallback(T* t){m_t = t;}
		VOID		SetPtr(T* t){m_t = t;}
		virtual	VOID	OnJsonAnalyse(IDCJsonObject& obj){if(m_t)m_t->OnJsonAnalyse(obj);}
		virtual	VOID	OnJsonAnalyseFail(const char* type, const char* szFailText){if(m_t)m_t->OnJsonAnalyseFail(type, szFailText);}
	};
	class MyHttpRequest: public PhantomV1::IDEThreadProc
	{
	public:
		MyHttpRequest();//构造函数
		virtual ~MyHttpRequest();//析构函数
		virtual	VOID					Initialize(const char* szJsonPHPUrl);
		inline	VOID					SetUseThread(BOOL b){m_bUseThread = b;}//设置是否使用线程来发送
		inline	BOOL					IsUseThread(){return m_bUseThread;}
		virtual BOOL					call(const char* szFormat, ...);//连接服务器,并发送一段json数据
		virtual BOOL					call(IDCJsonObject& obj);//连接服务器,并发送一段json数据
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
		//基础接口
		BOOL							OnParseJson(const char* szText);
		int								m_nResult;
		IDCJsonObject						m_tempJson;
		BOOL							m_bUseThread;
		Socket							m_hSocket;
		IJsonAnalyseCallback*			m_pJsonCallback;
		char							m_szHost[128];//设置地址
		char							m_szPage[256];//设置地址
		Phantom::NameT<4096>						m_requestText;
		Phantom::NameT<65536>					m_jsonCode;
		Phantom::CDynamicArray<char>				m_recvBuffers;
		int								m_jsonCount;
		BOOL							m_bFail;
		PhantomV1::IDE_Lock			m_lock;
		BOOL							m_bRequestComplete;
	};
	VOID		DoQueue();
	BOOL		OnDownloadFrame();
	char*		urldecode(char* inOut);//对文本进行反编
	bool		urlencode(char* ret, int bufferLength, const char* sIn, int len = 0);
	typedef	VOID	(*PDownloadComplte)(const char* requestText, const char* indocFile, std::vector<char>& buffers, BOOL bError, LPARAM lParam);//bError是否有错误
	BOOL		downloadFile(const char* requestText, const char* indocFile, PDownloadComplte callback = 0, LPARAM lParam = 0, BOOL bAutoSave = true);//从网络下载文件
	//
	typedef void (WINAPI*fnHttpCallback)(Phantom::Name& file, IDCJsonObject& obj, const char* errorText, void* param);//如果errorText==0则表示没有任何错误发生
	VOID		Request(const char* infile, IDCJsonObject& params, fnHttpCallback cb, BOOL bUseThread = false, void* param = 0);//请求数据
	VOID		RequestIP(fnHttpCallback cb, const char* szUrl);
	VOID		MessageResult(HWND hWnd, int result);
	VOID		md5(const char* str, std::string& ret);//md5加密
	//
	VOID	SetPPoint(int point);//设置当前V币数量
	struct MonthPoint{
		int month1,month6,month12,month24;
		Phantom::FileName	vipurl;
		MonthPoint(){month1=month6=month12=month24=0;}
	};
	VOID	SetMonthPoint(const MonthPoint& point);//设置月
	INT		GetPPoint();
	const MonthPoint&	GetMonthPoint();
	const char*	GetHttpKey();
	const char*	GetHomeUrl();//主页URL

//};

#endif
