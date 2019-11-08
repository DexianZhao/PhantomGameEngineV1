//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <Windows.h>
#endif

#include "PhantomBase.h"
#include "encrypt.h"
#ifndef WIN32_OS
    extern "C"{
        int Utf8ToWideOS(const char* text, int textlen, unsigned short* ret);
        int WideToUtf8OS(const unsigned short* text, int textlen, char* ret);
    };
#endif
	VOID MyLogDebug(const char* szText)//输出信息
	{
#ifdef WIN32_OS
		::OutputDebugStringA(szText);
		::OutputDebugStringA("\r\n");
#elif defined(IPHONE_OS)
        LogPrint("%s", szText);
#elif defined(ANDROID_OS)
        __android_log_print(ANDROID_LOG_INFO, "PhantomGameEngine", szText);
#endif
	}
	MyLogPrint	g_mylog = &MyLogDebug;
	void	SetLogPrint(MyLogPrint m)
	{
		g_mylog = m;
	}
	void    myLogPrint(const char* szText, ...)
	{
		char temp[10241];
#ifdef WIN32
		_vsnprintf(temp,10240,szText,(va_list)(&szText+1));
#else
        va_list args;
        va_start(args, szText);
        vsnprintf(temp, 10240, szText, args);
        va_end(args);
#endif
		if(g_mylog)
		{
			(*g_mylog)(temp);
#ifdef WIN32_OS
		::OutputDebugStringA(temp);
		::OutputDebugStringA("\r\n");
#endif
		}
		else
		{
#ifdef IPHONE_OS
			LogPrint("%s\n", temp);
#elif defined(ANDROID_OS)
		__android_log_print(ANDROID_LOG_INFO, "PhantomGameEngine", temp);
#elif defined(WIN32_OS)
		::OutputDebugStringA(temp);
		::OutputDebugStringA("\r\n");
#endif
		}
	}
namespace Phantom{
	
	CDynamicArray<cputime*>	cputime::s_tops;//最高级别的时钟
	cputime*			cputime::s_curr = 0;
	unsigned char		cputime::s_channelId = 0;
	cputime::cputime(const char* className, const char* funcName, int index, const char* file, int line)
	{
		memset(m_timeBegin, 0, sizeof(m_timeBegin));
		memset(m_timeFull, 0, sizeof(m_timeFull));
		memset(m_beginCount, 0, sizeof(m_beginCount));
		m_timeMax = 0;
		m_timeMin = 1.0f;
		m_className = className;
		NameT<512> files;
		files = file;
		NameT<512> f,ext;
		files.splitpath(0, &f, &ext);
		sprintf(m_funcName.t, "%s%d", funcName, index);
		m_fileName = f.t;
		m_fileName += ext.t;
		m_line = line;
		m_parent = 0;
		m_push = 0;
		m_pushCount		=	0;
		if(s_curr)
			s_curr->addchild(this);
		//else
		s_tops.push_back(this);
	}
	cputime::cputime(const char* className, const char* funcName, const char* file, int line)
	{
		memset(m_timeBegin, 0, sizeof(m_timeBegin));
		memset(m_timeFull, 0, sizeof(m_timeFull));
		memset(m_beginCount, 0, sizeof(m_beginCount));
		m_timeMax = 0;
		m_timeMin = 1.0f;
		m_className = className;
		NameT<512> files;
		files = file;
		NameT<512> f,ext;
		files.splitpath(0, &f, &ext);
		m_funcName = funcName;
		m_fileName = f.t;
		m_fileName += ext.t;
		m_line = line;
		m_parent = 0;
		m_push = 0;
		m_pushCount		=	0;
		if(s_curr)
			s_curr->addchild(this);
		//else
		s_tops.push_back(this);
	}
	void		cputime::begin()
	{
		if(m_pushCount==0)
		{
			m_push = s_curr;
			s_curr = this;
		}
		m_pushCount++;
		m_beginCount[s_channelId]++;
		m_timeBegin[s_channelId] = GetTickTime();
	}
	void		cputime::end()
	{
		double dE = GetTickTime() - m_timeBegin[s_channelId];
		m_timeFull[s_channelId] += dE;
		m_timeMax = getmax(dE, m_timeMax);
		m_timeMin = getmin(dE, m_timeMin);
		m_pushCount--;
		if(m_pushCount==0)
		{
			//assert(s_curr==this);
			s_curr = m_push;
			m_push = 0;
		}
	}
	void		cputime::addchild(cputime* c)
	{
		c->m_parent = this;
		m_childs.push_back(c);
	}


	void	Text::format(const char* fmt, ...)
	{
		char temp[10241];
#ifdef WIN32
		_vsnprintf(temp,10240,fmt,(va_list)(&fmt+1));
#else
        va_list args;
        va_start(args, fmt);
        vsnprintf(temp, 10240, fmt, args);
        va_end(args);
#endif
		this->operator=(temp);
	}
	void	Text::add(const char* fmt, ...)
	{
		char temp[10241];
#ifdef WIN32
		_vsnprintf(temp,10240,fmt,(va_list)(&fmt+1));
#else
        va_list args;
        va_start(args, fmt);
        vsnprintf(temp, 10240, fmt, args);
        va_end(args);
#endif
		this->operator+=(temp);
	}
	VOID			ReplaceAll(CDynamicArray<char>& ret, const char* szText, const char* szFinds, const char* szReplace){
		int nLen = strlen(szText);
		int nFind = strlen(szFinds);
		int nReplace = strlen(szReplace);
		const char* first = strstr(szText, szFinds);
		const char* last = szText;
		while(first){
			int len = first - last;
			int oldLen = ret.size();
			ret.SetArrayCount(ret.size()+len+nReplace, 0);
			memcpy(ret.GetPtr()+oldLen, last, len);
			memcpy(ret.GetPtr()+oldLen+len, szReplace, nReplace);
			first = first + nFind;
			last = first;
			first = strstr(last, szFinds);
		}
		first = szText + nLen;
		int len = first - last;
		int oldLen = ret.size();
		ret.SetArrayCount(ret.size()+len, 0);
		memcpy(ret.GetPtr()+oldLen, last, len);
	}
	const char* GetActionTypeName(int t)
	{
		switch(t)
		{
		case ActionType_Idle:
			return "Idle";
		case ActionType_Walk:
			return "Walk";
		case ActionType_Run:
			return "Run";
		case ActionType_Down:
			return "Down";
		case ActionType_Jump:
			return "Jump";
		case ActionType_Hit:
			return "Hit";
		case ActionType_Die:
			return "Die";
		case ActionType_Attack:
			return "Attack";
		case ActionType_Idle2:
			return "Idle2";
		case ActionType_Walk2:
			return "Walk2";
		case ActionType_Run2:
			return "Run2";
		case ActionType_Jump2:
			return "Jump2";
		case ActionType_Hit2:
			return "Hit2";
		case ActionType_Die2:
			return "Die2";
		case ActionType_Attack2:
			return "Attack2";
		case ActionType_Idle3:
			return "Idle3";
		case ActionType_Walk3:
			return "Walk3";
		case ActionType_Run3:
			return "Run3";
		case ActionType_Jump3:
			return "Jump3";
		case ActionType_Hit3:
			return "Hit3";
		case ActionType_Die3:
			return "Die3";
		case ActionType_Attack3:
			return "Attack3";
		case ActionType_Hide:
			return "Hide";
		case ActionType_Other1:
			return "Other1";
		case ActionType_Other2:
			return "Other2";
		case ActionType_Other3:
			return "Other3";
		case ActionType_Other4:
			return "Other4";
		case ActionType_Other5:
			return "Other5";
		case ActionType_Other6:
			return "Other6";
		case ActionType_Other7:
			return "Other7";
		case ActionType_Other8:
			return "Other8";
		case ActionType_Other9:
			return "Other9";
		case ActionType_Other10:
			return "Other10";
		}
		return "";
	}
	void							colorkey_data::from(const color4& c)
	{
		int ic[4];
		ic[0]	=	(int)(c.r * 255.0f);
		ic[1]	=	(int)(c.g * 255.0f);
		ic[2]	=	(int)(c.b * 255.0f);
		ic[3]	=	(int)(c.a * 255.0f);
		for(int i=0;i<4;i++)
		{
			if(ic[i] < 0)ic[i] = 0;
			if(ic[i] > 255)ic[i] = 255;
			color[i] = ic[i];
		}
	}
	void							colorkey_data::lerp(color4& out, colorkey_data& next, float fLerp)
	{
		float f2	=	(1.0f - fLerp);
		const float f255	=	1.0f / 255.0f;
		out.r		=	((float)next.color[0] * fLerp + (float)color[0] * f2) * f255;
		out.g		=	((float)next.color[1] * fLerp + (float)color[1] * f2) * f255;
		out.b		=	((float)next.color[2] * fLerp + (float)color[2] * f2) * f255;
		out.a		=	((float)next.color[3] * fLerp + (float)color[3] * f2) * f255;
	}
	void							colorkey_data::GetColor(color4& out)
	{
		const float f255	=	1.0f / 255.0f;
		out.r		=	(float)color[0] * f255;
		out.g		=	(float)color[1] * f255;
		out.b		=	(float)color[2] * f255;
		out.a		=	(float)color[3] * f255;
	}

	void	matrix3x4::from(matrix4x4& mat)
	{
		int index = 0;
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<3;j++)
			{
				v[index]	=	mat.m[i][j];
				index++;
			}
		}
	}

	void	matrix3x4::get(matrix4x4& mat)
	{
		int index = 0;
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<3;j++)
			{
				mat.m[i][j]	=	v[index];
				index++;
			}
		}
	}

	void	matrix3x4::lerp(matrix4x4& mat, matrix3x4& next, float fLerp)
	{
		int index = 0;
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<3;j++)
			{
				mat.m[i][j]	=	(next.v[index] - v[index]) * fLerp + v[index];
				index++;
			}
		}
	}

	char * strlwr_t(char *s)
	{
		unsigned char *ucs = (unsigned char *) s;
		for ( ; *ucs != '\0'; ucs++)
		{
			*ucs = tolower(*ucs);
		}
		return s;
	}

	char * strupr_t(char *s)
	{
		unsigned char *ucs = (unsigned char *) s;
		for ( ; *ucs != '\0'; ucs++)
		{
			*ucs = toupper(*ucs);
		}
		return s;
	}

	int				u_strlen(const short_t* s1)
	{
		if(!s1)
			return 0;
		int c = 0;
		while(*s1)
		{
			c++;
			s1++;
		}
		return c;
	}
	const short_t*	u_strcpy(short_t* s1, const short_t* s2)
	{
		if(!s1 || !s2)
			return 0;
		while(*s2)
		{
			*s1	=	*s2;
			s1++;
			s2++;
		}
		*s1	=	0;
		return s1;
	}
	const short_t*	u_strncpy(short_t* s1, const short_t* s2, int length)
	{
		if(!s1 || !s2)
			return 0;
		while(*s2 && length > 0)
		{
			*s1	=	*s2;
			s1++;
			s2++;
			length --;
		}
		*s1	=	0;
		return s1;
	}
	const short_t*	u_strcat(short_t* s1, const short_t* s2)
	{
		if(!s1 || !s2)
			return 0;
		while(*s1)
			s1++;
		//
		while(*s2)
		{
			*s1	=	*s2;
			s1++;
			s2++;
		}
		*s1	=	0;
		return s1;
	}
	const short_t*	u_strlwr(short_t* s1)
	{
		while(*s1)
		{
			if(*s1 < 128)
				*s1 = tolower(*s1);
			s1++;
		}
		return s1;
	}
	int				u_strcmp(const short_t* s1, const short_t* s2)
	{
        int ret = 0 ;
        while( ! (ret = (int)(*s1 - *s2)) && *s2)
                ++s1, ++s2;
        if ( ret < 0 )
                ret = -1 ;
        else if ( ret > 0 )
                ret = 1 ;
        return( ret );
	}
	int				u_stricmp(const short_t* s1, const short_t* s2)
	{
        int ret = 0 ;
        while(*s1 && *s2)
		{
			short_t u1 = *s1;
			short_t u2 = *s2;
			if(u1 < 128)
				u1 = tolower(u1);
			if(u2 < 128)
				u2 = tolower(u2);
			ret = (int)u1 - (int)u2;
			if(ret != 0)
				break;
			++s1;
			++s2;
		}
		if(ret==0)
		{
			if(*s1==*s2)
				return 0;
			if(*s1)
				return -1;
			return 1;
		}
        if ( ret < 0 )
                ret = -1 ;
        else if ( ret > 0 )
                ret = 1 ;
        return( ret );
	}

	void*				CDMOperator::Move(void* dest, const void* src, int size)
	{
		return memmove(dest, src, size);
	}
	void*				CDMOperator::Copy(void* dest, const void* src, int size)
	{
		return memcpy(dest, src, size);
	}
	void*				CDMOperator::Allocate(int size)
	{
		return malloc(size);
	}
	void*				CDMOperator::Reallocate(void* ptr, int size)
	{
		return realloc(ptr, size);
	}
	void				CDMOperator::Free(void* ptr)
	{
		free(ptr);
	}
	void				CDMOperator::Fill(void* ptr, int v, int size)
	{
		memset(ptr, v, size);
	}
	VOID		RandomStart()	//随机数开始
	{
#ifdef WIN32_OS
		srand(GetTickCount());
#else
		timeval t2;
		gettimeofday(&t2, NULL);
		srand(t2.tv_sec);
#endif
	}
	void		GetLocalTime(Time& tr)
	{
#ifdef WIN32_OS
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		tr.hour		=	sys.wHour;
		tr.minute	=	sys.wMinute;
		tr.second	=	sys.wSecond;
		tr.year		=	sys.wYear;
		tr.month	=	sys.wMonth;
		tr.day		=	sys.wDay;
		LARGE_INTEGER curr;
		QueryPerformanceCounter(&curr);
		tr.microsecond	=	curr.LowPart;
#else
		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );
		tr.hour		=	now->tm_hour;
		tr.minute	=	now->tm_min;
		tr.second	=	now->tm_sec;
		tr.year		=	now->tm_year+1900;
		tr.month	=	now->tm_mon + 1;
		tr.day		=	now->tm_mday;
		//
		timeval t2;
		gettimeofday(&t2, NULL);
		tr.microsecond	=	t2.tv_usec;
#endif
	}
	void		GetTime(int& hour, int& minute, int& second)	//返回时钟
	{
#ifdef WIN32_OS
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		hour	=	sys.wHour;
		minute	=	sys.wMinute;
		second	=	sys.wSecond;
#else
		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );
		hour = now->tm_hour;
		minute = now->tm_min;
		second = now->tm_sec;
#endif
	}
	double		GetTickTime()	//秒时间
	{
#ifdef WIN32_OS
		static BOOL bInit		=	false;
		static LARGE_INTEGER	win32Frequency;
		if(!bInit)
			QueryPerformanceFrequency(&win32Frequency);
		LARGE_INTEGER curr;
		QueryPerformanceCounter(&curr);
		return (double)curr.QuadPart / (double)win32Frequency.QuadPart;
#else
		timeval t2;
		gettimeofday(&t2, NULL);
		double ret	=	t2.tv_sec;      // sec to ms
		ret			+=	t2.tv_usec / 1000000.0;   // us to ms
		return ret;
#endif
	}
	double		RequestIntervalTime()
	{
		static BOOL bInit		=	false;
		double	dElapsedTime	=	0;
#ifdef WIN32_OS
		static LARGE_INTEGER	win32Frequency;
		static LARGE_INTEGER	lastTimer;
		if(!bInit)
		{
			QueryPerformanceFrequency(&win32Frequency);
			QueryPerformanceCounter(&lastTimer);
		}
		else
		{
			LARGE_INTEGER curr;
			QueryPerformanceCounter(&curr);
			dElapsedTime	= ((double)curr.QuadPart - (double)lastTimer.QuadPart) / (double)win32Frequency.QuadPart;
			lastTimer	=	curr;
		}
#else
		static timeval	lastTimer;
		if(!bInit)
		{
			gettimeofday(&lastTimer, NULL);
		}
		else
		{
			timeval t2;
			gettimeofday(&t2, NULL);
			dElapsedTime	=	(t2.tv_sec - lastTimer.tv_sec);      // sec to ms
			dElapsedTime	+=	(t2.tv_usec - lastTimer.tv_usec) / 1000000.0;   // us to ms
			lastTimer	=	t2;
		}
#endif
		bInit	=	true;
		return dElapsedTime;
	}
	VOID		AppSleep(INT millisecond)
	{
#ifdef WIN32_OS
		Sleep(millisecond);
#else
		usleep(millisecond * 1000);
#endif
	}
	//

	short_t utf8ToUnicode( const unsigned char *s, int *bytes )
	{
		*bytes	=	1;
		if ( ( 0x80 & s[0] ) == 0 )//一个字节的UTF-8 
		{
			return s[0];
		}
		short_t a, b;
		if ( ( 0xE0 & s[0] ) == 0xC0 )//两字节的UTF-8 
		{
			*bytes = 2;
			a = ( 0x1F & s[0] ) << 6;
			b = 0x3F & s[1];
			return a + b;
		}
		short_t c;
		if ( ( 0xF0 & s[0] ) == 0xE0 )//三字节的UTF-8 
		{
			*bytes = 3;
			a = ( 0x0F & s[0] ) << 12;
			b = ( 0x3F & s[1] ) << 6;
			c = 0x3F & s[2];
			return a + b + c;
		}
		return 0;
	}
	int	UnicodeToUtf8(short_t w, unsigned char* rets)
	{
        if (w >= 0x0000 && w <= 0x007f)
        {
            *rets = w;
            return 1;
        }
        else if (w >= 0x0080 && w <= 0x07ff)
        {
            *rets = 0xc0 | (w >> 6);
            rets ++;
            *rets = 0x80 | (w & (0xff >> 2));
            return 2;
        }
        //else if (w >= 0x0800 && w <= 0xffff)
		*rets = 0xe0 | (w >> 12);
		rets ++;
		*rets = 0x80 | (w >> 6 & 0x00ff);
		rets ++;
		*rets = 0x80 | (w & (0xff >> 2));
		return 3;
	}
	int				Utf8ToWide(const char* szUTF8, short_t* ret, int length)
	{
#ifdef WIN32
		int len		=	::MultiByteToWideChar(CP_UTF8, 0, szUTF8, strlen(szUTF8), (LPWSTR)ret, length);
		ret[len]	=	0;
		return len;
#else
		return Utf8ToWideOS(szUTF8, strlen(szUTF8), ret);
#endif
	}
	int				WideToUtf8(const short_t* szW, char* ret, int length)
	{
#ifdef WIN32
		int len		=	::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)szW, -1, ret, length, 0, 0);
		ret[len]	=	0;
		return len;
#else
		return WideToUtf8OS(szW, u_strlen(szW), ret);
#endif
	}

	VOID	AsciiToUtf8(CDynamicArray<char>& ret, const char* szIn)
	{
#ifdef WIN32
		int len = strlen(szIn);
		wchar_t* buf = new wchar_t[len*2+1];
		len			=	::MultiByteToWideChar(CP_ACP, 0, szIn, -1, (LPWSTR)buf, len*2+1);
		buf[len]	=	0;
		ret.resize(len*2+1);
		len			=	::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)buf, -1, ret.GetPtr(), len*2+1, 0, 0);
		ret.resize(len);
		delete[] buf;
#else
		int len = strlen(szIn);
		ret.resize(len);
		strcpy(ret.GetPtr(), szIn);
#endif
	}
	VOID	Utf8ToAscii(CDynamicArray<char>& ret, const char* szIn)
	{
#ifdef WIN32
		int len = strlen(szIn);
		wchar_t* buf = new wchar_t[len*2+1];
		len		=	::MultiByteToWideChar(CP_UTF8, 0, szIn, -1, (LPWSTR)buf, len*2+1);
		buf[len]	=	0;
		ret.resize(len*2+1);
		len			=	::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)buf, -1, ret.GetPtr(), len*2+1, 0, 0);
		ret.resize(len);
		delete[] buf;
#else
		int len = strlen(szIn);
		ret.resize(len);
		strcpy(ret.GetPtr(), szIn);
#endif
	}
/*
	int				ShortToWide(const short_t* szUTF8, wchar_t* ret, int length)
	{
		int len = u_strlen(szUTF8);
		for(int i=0;i<=len;i++)
			ret[i] = szUTF8[i];
		return len;
	}
	int				WidtToShort(const wchar_t* szW, short_t* ret, int length)
	{
		int len = wcslen(szW);
		for(int i=0;i<=len;i++)
			ret[i] = szW[i];
		return len;
	}
*/
	int							Utf8ToW(const char* szUTF8, short_t* ret, int length)
	{
		int len		=	strlen(szUTF8);
#ifdef WIN32
		len		=	::MultiByteToWideChar(CP_ACP, 0, szUTF8, -1, (LPWSTR)ret, length);
		ret[len]	=	0;
		return len;
#else
//#ifdef ANDROID_OS
		int index	=	0;
		for(int i=0;i<len;i++)
		{
			int n = 0;
			ret[index++] = utf8ToUnicode((const unsigned char*)szUTF8 + i, &n);
			i	+=	(n - 1);
			if(index >= length)
			{
				ret[index - 1] = 0;
				return index;
			}
		}
		ret[index] = 0;
		return index;
#endif
//#else
//		static CDynamicArray<short_t> wcs;
//		wcs.SetArrayCount(len * 2);
//		::mbstowcs(wcs.GetPtr(), szUTF8, wcs.size());
//		for(int i=0;i<length;i++)
//		{
//			ret[i] = wcs[i];
//			if(i >= wcs.size())
//				return wcs.size();
//		}
//		ret[length - 1] = 0;
//		return length - 1;
//#endif
	}
	int							WToUtf8(const short_t* szW, char* ret, int length)
	{
#ifdef WIN32
		int len		=	::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)szW, -1, ret, length, 0, 0);
		ret[len]	=	0;
		return len;
#else
		int len		=	u_strlen(szW);
		int index	=	0;
		for(int i=0;i<len;i++)
		{
			unsigned char ch[8];
			int n = UnicodeToUtf8(szW[i], ch);
			for(int j=0;j<n;j++)
			{
				ret[index++] = ch[j];
				if(index >= length)
				{
					ret[length - 1] = 0;
					return length;
				}
			}
		}
		ret[index] = 0;
		return index;
#endif
	}


	char	dec(unsigned char ch)
	{
		const unsigned char s_decVector[] = "0123456789ABCDEF";
		if(ch < 16)
			return s_decVector[ch];
		return 0;
	}
	unsigned char	hex(char ch)
	{
		if(ch >= '0' && ch <= '9')
			return ch - '0';
		if(ch >= 'A' && ch <= 'F')
			return ch - 'A' + 10;
		return 0;
	}
	VOID	HexEncode(std::vector<unsigned char>& ret, const char* szText)
	{
		std::string str = szText;
		strupr_t((char*)str.c_str());
		int len	=	str.size();
		for(int i=0;i<len; i+=2)
		{
			ret.push_back((hex(str[i]) * 16 + hex(str[i + 1])));
		}
	}
	int	HexDecode(char* ret, unsigned char* val, int length)
	{
		int count	=	0;
		for(int i=0;i<length;i++)
		{
			unsigned char ch	=	val[i];
			ret[count++]		=	dec(ch / 16);
			ret[count++]		=	dec(ch % 16);
		}
		ret[count++] = 0;
		return count;
	}

	HttpGUID::HttpGUID()
	{
		Generate();
	}
	HttpGUID::~HttpGUID()
	{
	}
	VOID	HttpGUID::ToText(char* szText)//返回一串文本
	{
		HexDecode(szText, (unsigned char*)this, sizeof(HttpGUID));
	}
	VOID	HttpGUID::FromText(const char* szText)//从一串文本
	{
		std::vector<unsigned char> rets;
		HexEncode(rets, szText);
		if(rets.size() == sizeof(HttpGUID))
			memcpy(this, &rets.at(0), rets.size());
	}
	VOID	HttpGUID::Generate()	//创建一个GUID
	{
		Time tm;
		GetLocalTime(tm);
		Data1[0] = (tm.year - 2000);
		Data1[1] = tm.month;
		Data1[2] = tm.day;
		Data1[3] = tm.hour;
		Data1[4] = tm.second;
		Data1[5] = tm.minute;
		Data2 = tm.microsecond;
		Data3 = rand();
		ucSystem	= 0;
		ucRoleID	= 0;
#ifdef ANDROID_OS
		ucSystem	= Flags_Android;
#elif defined(IPHONE_OS)
		ucSystem	= Flags_IPhone;
#endif
	}


	//数据指针管理器
	class	CPtrManager
	{
	public:
		CPtrManager();
		~CPtrManager();
		BOOL					IsExistPtr(const PtrID& id);
		Base*			GetPtr(const PtrID& id);
	protected:
		PtrID					Push(Base* ptr);
		VOID					Pop(const PtrID& id);
		friend class			Base;
	protected:
		CDynamicArray<Ptr>		m_pointers;
		CDynamicArray<int>		m_pointersFree;
		int						m_nID;
	};

	CPtrManager::CPtrManager()
	{
		m_pointers.setGrow(1000);
		m_pointersFree.setGrow(1000);
		m_nID	=	1;
	}
	CPtrManager::~CPtrManager()
	{
	}

	PtrID					CPtrManager::Push(Base* autoPtr)
	{
		if(m_pointersFree.size() > 0)
		{
			int newSize = m_pointersFree.size()-1;
			int index = m_pointersFree[newSize];
			assert(m_pointers[index].id == 0);
			m_pointersFree.SetArrayCount(newSize);
			m_pointers[index].id		=	m_nID++;
			m_pointers[index].ptr		=	autoPtr;
			return PtrID(index, m_pointers[index].id);
		}
		//for(int i=0;i<m_pointers.size();i++)
		//{
		//	if(m_pointers[i].id == 0)
		//	{
		//		m_pointers[i].id		=	m_nID++;
		//		m_pointers[i].ptr		=	autoPtr;
		//		return PtrID(i, m_pointers[i].id);
		//	}
		//}
		Ptr p;
		p.id	=	m_nID++;
		p.ptr	=	autoPtr;
		int index	=	m_pointers.size();
		m_pointers.push_back(p);
		return PtrID(index, m_pointers[index].id);
	}
	VOID					CPtrManager::Pop(const PtrID& id)
	{
		if(id.index >= m_pointers.size())
			return;
		assert(m_pointers[id.index].id == id.id);
		if(m_pointers[id.index].id != id.id)
			return;
		int oldSize = m_pointersFree.size();
		m_pointersFree.SetArrayCount(oldSize+1);
		m_pointersFree[oldSize] = id.index;
		m_pointers[id.index].id			=	0;
		m_pointers[id.index].ptr		=	0;
	}

	Base*				CPtrManager::GetPtr(const PtrID& id)
	{
		if(id.index >= m_pointers.size())
			return 0;
		if(m_pointers[id.index].id != id.id)//   ？？？ID不一致是咋回事?
			return 0;
		return m_pointers[id.index].ptr;
	}

	BOOL					CPtrManager::IsExistPtr(const PtrID& id)
	{
		if(id.index >= m_pointers.size())
			return false;
		if(m_pointers[id.index].id != id.id)
			return false;
		return true;
	}

	CPtrManager*	g_pPtrManager	=	0;
	CPtrManager*	GetPtrManager()
	{
		if(!g_pPtrManager)
			g_pPtrManager	=	new CPtrManager();
		return g_pPtrManager;
	}
	int						GetBaseContext(const PtrID& id)
	{
		if(g_pPtrManager)
		{
			Base*  b = g_pPtrManager->GetPtr(id);
			if(!b)return 0;
			return b->m_context;
		}
		return 0;
	}
	VOID					SetBaseContext(const PtrID& id, int n)
	{
		if(g_pPtrManager)
		{
			Base*  b = g_pPtrManager->GetPtr(id);
			if(b)
				b->m_context	=	n;
		}
	}
	Base*			GetBasePtr(const PtrID& id)
	{
		if(g_pPtrManager)
			return g_pPtrManager->GetPtr(id);
		return 0;
	}
	Base::Base(){
		m_baseid	=	GetPtrManager()->Push(this);
	}
	Base::~Base(){
		assert(m_baseid.id > 0 && m_baseid.index >= 0);
		GetPtrManager()->Pop(m_baseid);
	}
	GenuineGUID				g_phantomGUID;
	VOID					SetPhantomGUID(unsigned int* guid)//设置幻影授权码，写入授权码之后很多操作才能正确运作，包括读取资源操作
	{
		memcpy(&g_phantomGUID, guid, sizeof(g_phantomGUID));
	}


	skip_w::skip_w(StreamBase* s){
		m_stream = s;
		m_seek = m_stream->tell();
		m_stream->write(&m_filePointer, sizeof(int));
	}
	char	skip_w::end(){
		if(!m_stream)
			return false;
		int skip_type = 0xD25F0821;
		m_filePointer = m_stream->tell();
		m_stream->seek(m_seek, 0);
		m_stream->write(&m_filePointer, sizeof(int));
		m_stream->seek(m_filePointer, 0);
		m_stream->write(&skip_type, sizeof(int));
		return true;
	}
	skip_w::~skip_w(){
	}
	skip_r::skip_r(){
		m_stream = 0;
	}
	void	skip_r::begin(StreamBase* s){
		m_stream = s;
		m_stream->read(&m_filePointer, sizeof(int));
	}
	char	skip_r::end(){
		if(!m_stream)
			return false;
		int currPointer = m_stream->tell();
		m_stream->seek(m_filePointer);
		int skip_type = 0xD25F0821;
		m_stream->read(&skip_type, sizeof(int));
		if(skip_type != 0xD25F0821)
		{
			assert(skip_type != 0xD25F0821);
			return 0;
		}
		if(currPointer <  m_filePointer)//如果当前指针小于需要的指针则表示目前处于版本过低状态
		{
			LogInfo("system->error:%s", utf8("你的引擎版本过低，将会丢失某些功能，请更新到最新版本"));
			return 2;
		}
		//如果当前指针和读取的指针一致则返回1，就是相同版本
		return 1;
	}
	skip_r::~skip_r(){
	}
};

MyHttpLoadFile	g_httpLoad = 0;
VOID	SetMyLoadFile(MyHttpLoadFile f)
{
	g_httpLoad = f;
}

VOID	InitMyFile(const char* szFile, BOOL bDownloadNow, BOOL bInMemory)
{
	if(g_httpLoad)
		(*g_httpLoad)(szFile, bDownloadNow, bInMemory);
}
