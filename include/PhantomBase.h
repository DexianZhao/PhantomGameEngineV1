//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
#ifndef __PHANTOM_BASE_H__
#define __PHANTOM_BASE_H__

#include <vector>
#include <string>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "PhantomMath.h"

#ifdef ANDROID_OS
#define	ANDROID_OS_X
#include <jni.h>
#include <android/log.h>
#define	assert(x)
#elif defined(IPHONE_OS)
#include <sys/time.h>
#include <QuartzCore/QuartzCore.h>
#include <assert.h>
#elif defined(WIN32_OS)
#include <assert.h>
#endif

#include "PhantomEnum.h"
#define	null				0
#define _MakeFileHead(ch0, ch1, ch2, ch3)	((unsigned int)(char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) | ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))
#define	ImplementComValue()	//PComValue m_ref;	long	AddRef(){return m_ref.AddRef();}	long	Release(){if(m_ref.Release()){delete this; return 0;} return m_ref.lRef;};
#define	ImpComValue2()	int	AddRef(){return m_ref.AddRef();}	int	Release(){if(m_ref.Release()){return 0;} return m_ref.lRef;};
#define COLOR_ARGB(a,r,g,b) (((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#ifdef _CPUTIME
#define	CPUTime(c)	static cputime _ctime(#c,__FUNCTION__,__FILE__,__LINE__);cputime_s s_ctime(_ctime);
#define	CPUTimeN(c,num)	static cputime _ctime##num(#c,__FUNCTION__, num,__FILE__,__LINE__);cputime_s s_ctime##num(_ctime##num);
#else
#define	CPUTime(c)	(void)0
#define	CPUTimeN(c,num)	(void)0
#endif
#if defined(_CPUTIME_TOP)||defined(_CPUTIME)
#define	CPUTimeT(c)	static cputime _ctime(#c,__FUNCTION__,__FILE__,__LINE__);cputime_s s_ctime(_ctime);
#else
#define	CPUTimeT(c)	(void)0
#endif
#define	CPUTimeNone()	(void)0
//memory operator
#define	memset_s			CDMOperator::Instance().Fill
#define	memmove_s			CDMOperator::Instance().Move
#define	malloc_s			CDMOperator::Instance().Allocate
#define	realloc_s			CDMOperator::Instance().Reallocate
#define	free_s				CDMOperator::Instance().Free
#define	memcpy_s			CDMOperator::Instance().Copy

#define	PHANTOM_VERSION		1800	//最少更新时间100，也就是1个版本之后必须更新最新软件
#define	PHANTOM_SUBVERSION	0		//小版本0到100，每次发布补丁都必须提高一次
	//第一次：2012-3月份，第一个版本发布, 1.8.0.0
	//第二次：
#define	MAX_UPDATE_VERSION	100
#define	PACKAGE_VERSION		PHANTOM_VERSION
#define MAX_TOUCH_COUNT     8
#define	MAX_LIGHT_COUNT		3	//一个模型最大光源数量
namespace Phantom{
	class	Program;
	class	DeviceMesh;
	class	Texture;
	class	ShaderV;
	class	ShaderP;
	class	Program;

	typedef	int	int64;//64浣峣nt
	class CDMOperator{public://CDefault Memory Operator榛樿鐨勫唴瀛樻搷浣滄帴鍙?缁ф壙IMemoryOperator鎺ュ彛
	//
	virtual	void*				Move(void* dest, const void* src, int size);
	virtual	void*				Copy(void* dest, const void* src, int size);
	virtual	void*				Allocate(int size);
	virtual	void*				Reallocate(void* ptr, int size);
	virtual	void				Free(void* ptr);
	virtual	void				Fill(void* ptr, int v, int size);
	static CDMOperator&	Instance(){static CDMOperator al; return al;}
	};
	extern	VOID	ThrowError();
	#pragma pack(push, 1)
	//
#ifdef WIN32_OS
#define	short_t		wchar_t
#else
	typedef	unsigned short	short_t;
#endif
	typedef	int				Result;
	//
	char*			strlwr_t(char *s);
	char*			strupr_t(char *s);
	int				u_strlen(const short_t* s1);
	const short_t*	u_strcpy(short_t* s1, const short_t* s2);
	const short_t*	u_strcat(short_t* s1, const short_t* s2);
	const short_t*	u_strlwr(short_t* s1);
	int				u_strcmp(const short_t* s1, const short_t* s2);
	int				u_stricmp(const short_t* s1, const short_t* s2);
	const short_t*	u_strncpy(short_t* s1, const short_t* s2, int length);
	short_t			utf8ToUnicode( const unsigned char *s, int *bytes );
	int				Utf8ToW(const char* szUTF8, short_t* ret, int length);
	int				WToUtf8(const short_t* szUTF8, char* ret, int length);
	//
	int				Utf8ToWide(const char* szUTF8, short_t* ret, int length);
	int				WideToUtf8(const short_t* szW, char* ret, int length);
	//
	struct PWindParameter
	{
		float3						dir;
		float						fTime;
	};
	struct BGR
	{
		unsigned char	b,g,r;
	};
	struct RGBA
	{
		unsigned char	r,g,b,a;
	};
	struct BGRA
	{
		unsigned char	b,g,r,a;
	};
	struct ARGB
	{
		unsigned char	a,r,g,b;
	};
	template <typename T, typename Ts = int, int Tgrow = 5>
	class CDynamicArray//动态数组接口,主要是在特定情况下快速的插入和删除数据时使用,一般你可以考虑使用stl库的std::vector或者std::list
	{
	public:
		static const Ts	TypeSize	=	sizeof(T);
		CDynamicArray(){ptr = 0; alloc_size = 0;array_size = 0; grow = Tgrow; }
		~CDynamicArray(){clear();}
		CDynamicArray(const CDynamicArray<T,Ts,Tgrow>& other)
		{
			grow = other.grow; 
			ptr = 0;
			alloc_size = 0;
			array_size = 0;
			this->SetArrayCount(other.GetCount());
			if(this->GetCount() > 0)
				memcpy_s(this->GetPtr(), other.ptr, sizeof(T) * this->GetCount());
		}
		inline	void	operator = (const CDynamicArray<T,Ts,Tgrow>& other)
		{
			clear();
			grow = other.grow; 
			this->SetArrayCount(other.GetCount(),0);
			if(this->GetCount() > 0)
				memcpy_s(this->GetPtr(), other.ptr, sizeof(T) * this->GetCount());
		}
		inline		void		setGrow(int g){grow = getmax(g, 1);}
		inline	T&	operator [](int index){assert(index >= 0 && index < array_size); return *(ptr + index);}
		inline	T&	at(int index){assert(index >= 0 && index < array_size); return *(ptr + index);}
		inline	T*	GetPtr(){return ptr;}	//返回数组的指针
		inline	int	size()		const {return array_size;}//返回数组的长度,元素数量
		inline	int	GetCount()	const {return array_size;}//返回数组的长度,元素数量,
		inline	int	GetAllocatedCount() const {return alloc_size;}//返回已经分配的数组长度, 分配数组长度始终大于真正的数组长度
		void		push_back(const T* t, int count = 1, int index = -1)//插入一段数据到数组中, t是数据的指针,count是数据的数组长度(默认是1),index设置要从哪个位置开始插入(默认是-1)
		{
			if(count <= 0)return;
			SetArrayCount(array_size + count);
			if(index < 0)
			{
				for(int i=0;i<count;i++)
					ptr[array_size - count + i] = t[i];
			}
			else
			{
				memmove_s(ptr + index + count, ptr + index, sizeof(T) * (array_size - count - index));
				for(int i=0;i<count;i++)
					ptr[index + i] = t[i];
			}
		}
		void		eraseByIndex(int index, int count = 1, BOOL bFromBack = false)//删除数组中的元素，index设置从哪个位置开始删除，count表示要删除的元素数量，bFromBack表示是否从数组最后面开始向前删除位置填充，以减少从内存中移动的数据量
		{
			if(!ptr || count <= 0)return;
			if(count >= array_size)
			{
				if(ptr)
					memset_s(&ptr[0], 0, sizeof(T));
				array_size	=	0;
				return;
			}
			if(bFromBack)
			{
				for(int i=0;i<count;i++)
				{
					int n1	=	i + index;
					int n2	=	(array_size - i - 1);
					if(n1 >= n2)
						break;
					memcpy_s(ptr + n1, ptr + n2, sizeof(T));
				}
			}
			else
			{
				memmove_s(ptr + index, ptr + index + count, sizeof(T) * (array_size - count - index));
			}
			SetArrayCount(array_size - count);
		}
		inline	void	resize(int _num){SetArrayCount(_num);}
		void		SetArrayCount(int _num, BOOL bZeroInit = true)//设置数组的长度，如果长度不够则函数内部自动增加数组长度，以minWidth+1个位单位自动增加。
		{
			array_size = _num;
			if(array_size < 0)array_size = 0;
			if((array_size + 1)>=alloc_size && array_size > 0)
			{
				int oldAlloc	=	alloc_size;
				alloc_size = array_size + grow + 1;
				if(ptr)
				{
					ptr = (T*)realloc_s(ptr, sizeof(T)*alloc_size);
					if(bZeroInit)
						memset_s(ptr + oldAlloc, 0, sizeof(T)*(alloc_size - oldAlloc));
				}
				else
				{
					ptr = (T*)malloc_s(sizeof(T)*alloc_size);
					if(bZeroInit)
						memset_s(ptr, 0, sizeof(T)*alloc_size);
				}
			}
			if(ptr)
				setZero(ptr[array_size]);//memset_s(&ptr[array_size], 0, sizeof(T));
		}
		inline	void	setZero(char& c){c = 0;}
		inline	void	setZero(short& c){c = 0;}
		inline	void	setZero(int& c){c = 0;}
		inline	void	setZero(double& c){c = 0;}
		inline	void	setZero(float& c){c = 0;}
		inline	void	setZero(unsigned int& c){c = 0;}
		inline	void	setZero(unsigned short& c){c = 0;}
		inline	void	setZero(unsigned char& c){c = 0;}
		template <typename vT>
		inline	void	setZero(vT& t){
			memset_s(&t, 0, sizeof(vT));
		}
		inline	void	clear() {alloc_size = 0; array_size = 0; if(ptr)free_s(ptr); ptr = 0;}//清空所有的数组元素，并释放所有的内存

		inline	void	push_back(const T t)	{push_back(&t);}//将数据从最后位置快速插入
		inline	void	pop_back(){if(array_size > 0)array_size--;else array_size = 0;}//删除最有位置的数据元素
		//
	protected:
		T*				ptr;	//数组指针
		Ts				alloc_size;//分配数组大小
		Ts				array_size;//当前数组大小
		unsigned char	grow;
	};
	template <typename T, int Tgrow=5>
	class CDynamicArrayS:public CDynamicArray<T,unsigned short,Tgrow>
	{
	};
#define	safe_delete_dyn(dyn)	for(int i=0;i<dyn.size();i++){delete dyn[i];};dyn.clear();
	//----------------------------------------------------------------------------------------------------------------
	//
	//对应windows的POINT
	//
	//----------------------------------------------------------------------------------------------------------------
	/*STRUCT*/struct	Pixel/*屏幕坐标数据*/
	{
		int		x/*左*/;
		int		y/*上*/;
		/*END_STRUCT*/
		///
		inline	Pixel&	Set(int _x,int _y)								{	x = _x; y = _y; return *this;	}
		inline	void		identity()									{x=y=0;}
		inline	Pixel()												{}
		Pixel(int _x,int _y)											{	x = _x; y = _y;	}
		//
		inline Pixel& operator	+=(const Pixel& v)						{x+=v.x;y+=v.y;return *this;}
		inline Pixel& operator	-=(const Pixel& v)						{x-=v.x;y-=v.y;return *this;}
		inline Pixel& operator	*=(const Pixel& v)						{x*=v.x;y*=v.y;return *this;}
		inline Pixel& operator	/=(const Pixel& v)						{x/=v.x;y/=v.y;return *this;}
		inline Pixel& operator	=(const Pixel& v)						{x=v.x;y=v.y;return *this;}
		//
		inline Pixel& operator	+=(int f)								{x+=f;y+=f;return *this;}
		inline Pixel& operator	-=(int f)								{x-=f;y-=f;return *this;}
		inline Pixel& operator	*=(int f)								{x*=f;y*=f;return *this;}
		inline Pixel& operator	/=(int f)								{x/=f;y/=f;return *this;}
		//
		inline Pixel& operator 	++()								{x+=1;y+=1;return *this;}
		inline Pixel& operator 	--()								{x-=1;y-=1;return *this;}
		//
		inline Pixel  operator 	+() const							{return *this;}
		inline Pixel  operator 	-() const							{return Pixel(-x,-y);}
		//
		inline Pixel  operator 	+(const Pixel& v) const			{return Pixel(x+v.x,y+v.y);}
		inline Pixel  operator 	-(const Pixel& v) const			{return Pixel(x-v.x,y-v.y);}
		inline Pixel  operator 	*(const Pixel& v) const			{return Pixel(x*v.x,y*v.y);}
		inline Pixel  operator 	/(const Pixel& v) const			{return Pixel(x/v.x,y/v.y);}
		//
		inline Pixel  operator 	+(int f) const						{return Pixel(x+f,y+f);}
		inline Pixel  operator 	-(int f) const						{return Pixel(x-f,y-f);}
		inline Pixel  operator 	*(int f) const						{return Pixel(x*f,y*f);}
		inline Pixel  operator 	/(int f) const						{float fInv = 1.0f / (float)f;return Pixel((int)((float)x*fInv),(int)((float)y*fInv));}
		//
		inline bool operator 		==(const Pixel& v) const			{return (x==v.x&&y==v.y);}
		inline bool operator 		!=(const Pixel& v) const			{return (x!=v.x||y!=v.y);}
		inline bool operator 		==(int f) const						{return (x==f&&y==f);}
		//
		inline bool operator		!=(int f) const						{return (x!=f||y!=f);}
	};
	/*STRUCT*/struct	Rect/*矩形数据*/
	{
		int					left/*左边*/;
		int					top/*上边*/;
		int					right/*右边*/;
		int					bottom/*下边*/;
		/*END_STRUCT*/
		inline	void		Offset(int x,int y)
		{
			left	+=	x;
			right	+=	x;
			top		+=	y;
			bottom	+=	y;
		}
		inline	int			GetHeight() const							{return bottom - top;}
		inline	int			GetWidth() const							{return right - left;}
		inline	Rect&		Set(int l,int t,int r,int b)				{	left=l;	top=t;	right=r;	bottom=b;	return *this;	}
		inline	void		identity(){left=top=right=bottom=0;}
		inline	void		inflate(int x, int y)
		{
			left	-=	x;
			right	+=	x;
			top		-=	y;
			bottom	+=	y;
		}
		inline	void		checkLTRB()
		{
			int l;
			if(left>right)	{	l = left;	left = right;	right = l;	}
			if(top>bottom)	{	l = top;	top = bottom;	bottom = l;	}
		}
		inline	bool		isPtIn(int x, int y) const {return ((left < x && right > x) && (top < y && bottom > y));}
		inline	bool		isPtIn(const Pixel& p) const {return ((left < p.x && right > p.x) && (top < p.y && bottom > p.y));}
		inline	void		clip( int minLeft, int minTop, int maxRight, int maxBottom)
		{
			//
			left = getmax(minLeft,left); top = getmax(minTop,top); bottom = getmin(maxBottom, bottom); right = getmin(maxRight, right);
		}
		inline	Rect()														{}
		Rect(int l,int t,int r,int b)								{	left=l;	top=t;	right=r;	bottom=b;	}
		Rect(Pixel pos, int w, int h)
		{
			left = pos.x;
			top = pos.y;
			right = pos.x + w;
			bottom = pos.y + h;
		}
		inline	Rect&	operator	=	(const Rect& rc)				{	left=rc.left;	top=rc.top;	right=rc.right;	bottom=rc.bottom;	return *this;	}
		//
		inline Rect  operator 	+(const Rect& v) const			{return Rect(left + v.left,top + v.top,right + v.right,bottom + v.bottom);}
		inline Rect  operator 	-(const Rect& v) const			{return Rect(left - v.left,top - v.top,right - v.right,bottom - v.bottom);}
		inline Rect  operator 	*(const Rect& v) const			{return Rect(left * v.left,top * v.top,right * v.right,bottom * v.bottom);}
		inline Rect  operator 	/(const Rect& v) const			{return Rect(v.left != 0 ? (left / v.left) : 0,v.top != 0 ? (top / v.top) : 0, v.right != 0 ? (right / v.right) : 0,v.bottom != 0? (bottom / v.bottom): 0);}
		inline bool	  operator 	==(const Rect& v) const		{return (left==v.left && top==v.top && right==v.right && bottom==v.bottom);}
		//
		inline	int			MiddleX(){return (left + (right - left) / 2);}
		inline	int			MiddleY(){return (top + (bottom - top) / 2);}
	};
	inline	VOID	SetRect(Rect* rc, int l, int t, int r, int b){rc->Set(l,t,r,b);}
	inline	VOID	OffsetRect(Rect* rc, int x, int y){rc->Offset(x, y);}
	inline	int		RectHeight(const Rect& rc){return rc.GetHeight();}
	inline	int		RectWidth(const Rect& rc){return rc.GetWidth();}
	struct	Size
	{
		int		cx;
		int		cy;
		///
		inline	Size&	Set(int _x,int _y)							{	cx = _x; cy = _y; return *this;	}
		inline	void		identity()									{	cx=cy=0;}
		inline	Size()													{}
		Size(int _x,int _y)											{	cx = _x; cy = _y;	}
		//
		inline Size& operator	+=(const Size& v)						{cx+=v.cx;cy+=v.cy;return *this;}
		inline Size& operator	-=(const Size& v)						{cx-=v.cx;cy-=v.cy;return *this;}
		inline Size& operator	*=(const Size& v)						{cx*=v.cx;cy*=v.cy;return *this;}
		inline Size& operator	/=(const Size& v)						{cx/=v.cx;cy/=v.cy;return *this;}
		inline Size& operator	=(const Size& v)						{cx=v.cx;cy=v.cy;return *this;}
		//
		inline Size& operator	+=(int f)								{cx+=f;cy+=f;return *this;}
		inline Size& operator	-=(int f)								{cx-=f;cy-=f;return *this;}
		inline Size& operator	*=(int f)								{cx*=f;cy*=f;return *this;}
		inline Size& operator	/=(int f)								{cx/=f;cy/=f;return *this;}
		//
		inline Size& operator 	++()									{cx+=1;cy+=1;return *this;}
		inline Size& operator 	--()									{cx-=1;cy-=1;return *this;}
		//
		inline Size  operator 	+() const								{return *this;}
		inline Size  operator 	-() const								{return Size(-cx,-cy);}
		//
		inline Size  operator 	+(const Size& v) const					{return Size(cx+v.cx,cy+v.cy);}
		inline Size  operator 	-(const Size& v) const					{return Size(cx-v.cx,cy-v.cy);}
		inline Size  operator 	*(const Size& v) const					{return Size(cx*v.cx,cy*v.cy);}
		inline Size  operator 	/(const Size& v) const					{return Size(cx/v.cx,cy/v.cy);}
		//
		inline Size  operator 	+(int f) const							{return Size(cx+f,cy+f);}
		inline Size  operator 	-(int f) const							{return Size(cx-f,cy-f);}
		inline Size  operator 	*(int f) const							{return Size(cx*f,cy*f);}
		inline Size  operator 	/(int f) const							{float fInv = 1.0f / (float)f;return Size((int)((float)cx*fInv),(int)((float)cy*fInv));}
		//
		inline BOOL operator 		==(const Size& v) const			{return (cx==v.cx&&cy==v.cy);}
		inline BOOL operator 		!=(const Size& v) const			{return (cx!=v.cx||cy!=v.cy);}
		inline BOOL operator 		==(int f) const					{return (cx==f&&cy==f);}
		//
		inline BOOL operator		!=(int f) const					{return (cx!=f||cy!=f);}
	};

	struct	PixelW
	{
		short	x;
		short	y;
		inline	operator Pixel(){return Pixel(x,y);}
		PixelW(const Pixel& p){x=p.x;y=p.y;}
		PixelW(short _x, short _y){x=_x;y=_y;}
		PixelW(){x=y=0;}
		inline	void	Set(short _x, short _y){x=_x;y=_y;}
	};
	struct	SizeW
	{
		short	cx;
		short	cy;
		inline	operator Size(){return Size(cx,cy);}
		SizeW(const Size& p){cx=p.cx;cy=p.cy;}
		SizeW(short _x, short _y){cx=_x;cy=_y;}
		SizeW(){cx=cy=0;}
		inline	void	Set(short _x, short _y){cx=_x;cy=_y;}
	};
	struct	RectW
	{
		short	left,top,right,bottom;
		inline	operator Rect(){return Rect(left, top, right, bottom);}
		RectW(const Rect& p){left=p.left;top=p.top;right=p.right;bottom=p.bottom;}
		RectW(short l, short t, short r, short b){left=l;top=t;right=r;bottom=b;}
		RectW(){left=top=right=bottom=0;}
		inline	void	Set(short l, short t, short r, short b){left=l;top=t;right=r;bottom=b;}
	};

	struct screenVertex3D
	{
		float			x, y, z, h;
		unsigned int	color;
		float			tu, tv;
		inline	void	set(float _x, float _y, float _z, float _h, float _tu, float _tv, unsigned int _color){
			x = _x;
			y = _y;
			z = _z;
			h = _h;
			color = _color;
			tu = _tu;
			tv = _tv;
		}
	};
	
	/*STRUCT*/struct sensordata/*传感器数据*/
	{
		double acceleration_x/*获取手机加速度数据x*/;
		double acceleration_y/*获取手机加速度数据y*/;
		double acceleration_z/*获取手机加速度数据z*/;
		double gravity_x/*获取手机的重力值在各个方向上的分量，根据这个就可以获得手机的空间位置，倾斜角度等x*/;
		double gravity_y/*获取手机的重力值在各个方向上的分量，根据这个就可以获得手机的空间位置，倾斜角度等y*/;
		double gravity_z/*获取手机的重力值在各个方向上的分量，根据这个就可以获得手机的空间位置，倾斜角度等z*/;
		double theta_z/*获取手机的倾斜角度z*/;
		double theta_xy/*获取手机的倾斜角度xy*/;
		double rotation_rate_x/*获取陀螺仪的数据 包括角速度x*/;
		double rotation_rate_y/*获取陀螺仪的数据 包括角速度y*/;
		double rotation_rate_z/*获取陀螺仪的数据 包括角速度z*/;
		double roll/*获取陀螺仪的数据 空间位置的欧拉角x*/;
		double pitch/*获取陀螺仪的数据 空间位置的欧拉角y*/;
		double yaw/*获取陀螺仪的数据 空间位置的欧拉角z*/;
		double qx/*空间位置的四元数x*/;
		double qy/*空间位置的四元数y*/;
		double qz/*空间位置的四元数z*/;
		double qw/*空间位置的四元数w*/;
		/*END_STRUCT*/
	};

	struct matrix3x4
	{
		float							v[12];
		void							from(matrix4x4& mat);
		void							get(matrix4x4& mat);
		void							lerp(matrix4x4& mat, matrix3x4& next, float fLerp);
		bool							isEqual(const matrix3x4& other);	//两个的值是否一样
	};

	struct colorkey_data
	{
		float							timerLerp;	//时间长度
		unsigned char					color[4];
		void							from(const color4& c);
		void							lerp(color4& out, colorkey_data& next, float fLerp);
		void							GetColor(color4& out);
	};

	struct VertexElement//详细参考DirectX的D3DUSAGE_...
	{
		enum
		{
			VE_USAGE_POSITION = 0,
			VE_USAGE_BLENDWEIGHT,
			VE_USAGE_BLENDINDICES,
			VE_USAGE_NORMAL,
			VE_USAGE_PSIZE,
			VE_USAGE_TEXCOORD,
			VE_USAGE_TANGENT,
			VE_USAGE_BINORMAL,
			VE_USAGE_TESSFACTOR,
			VE_USAGE_POSITIONT,
			VE_USAGE_COLOR,
			VE_USAGE_FOG,
			VE_USAGE_DEPTH,
			VE_USAGE_SAMPLE,
			VE_USAGE_COUNT,
		};

		enum
		{
			VE_TYPE_FLOAT1    =  0,
			VE_TYPE_FLOAT2    =  1,
			VE_TYPE_FLOAT3    =  2,
			VE_TYPE_FLOAT4    =  3,
			VE_TYPE_D3DCOLOR  =  4,
			VE_TYPE_UBYTE4    =  5,
			VE_TYPE_SHORT2    =  6,
			VE_TYPE_SHORT4    =  7,
		};

		int		type;
		int		usage;
		int		usageIndex;
		int		method;
		VertexElement(){ method = 0; }
		VertexElement(int _type, int _usage, int _usageIndex = 0){type = _type; usage = _usage; usageIndex = _usageIndex; method = 0;}
	};
	struct D3DVertexElement
	{
		WORD    Stream;     // Stream index
		WORD    Offset;     // Offset in the stream in bytes
		BYTE    Type;       // Data type
		BYTE    Method;     // Processing method
		BYTE    Usage;      // Semantics
		BYTE    UsageIndex; // Semantic index
	};
	struct INT_64
	{
		INT_64(){LowPart = HighPart = 0;}
		unsigned int LowPart;
		unsigned int HighPart;
		inline	operator int64(){return LowPart;}
	};
	struct	MeshConfigID
	{
		int								geometryIndex;
		int								geometryID;
	};
	template <int count>
	struct	NameT
	{
		static	const int	Length	=	count - 1;
		NameT(){t[0] = 0;}
		char							t[count];
		inline	int		size(){return strlen(t);}
		NameT(const char* s){if(s!=t){strncpy(t, s, Length); t[Length] = 0;}}
		inline	const char*	str(){return t;}
		inline	operator const char*	(){return t;}
		inline	void	operator = (int n){
			sprintf(t, "%d", n);
		}
		inline	void	operator = (const char* s)
		{
			if(s==t)return;
			int len = strlen(s);
			if(len > Length)
				len = Length;
			memcpy_s(t, s, len);
			t[len] = 0;
		}
		inline	void	setIsFile()
		{
			int len	=	strlen(t);
			while((t[0] == '/' || t[0] == '\\' || (t[0] == '.' && t[1] != '.')) && t[0] != 0)
			{
				memmove_s(t, t + 1, len);
				len--;
			}
			for(int i=0;i<len;i++)
			{
				if(t[i] == '\\')
					t[i] = '/';
				else if(t[i] == '\r' || t[i] == '\n')
					t[i] = 0;
				else
					t[i]	=	tolower(t[i]);
			}
		}
		inline	BOOL	operator == (const char* s) const {return (stricmp(t, s) == 0);}
		inline	BOOL	operator != (const char* s) const {return (stricmp(t, s) != 0);}
		//
		inline	void	operator += (const char* s)
		{
			int slen = strlen(t);
			int nlen = strlen(s);
			if((slen + nlen) < (Length))
				strcat(t, s);
		}
		inline	void	operator += (char s)
		{
			int slen = strlen(t);
			t[slen] = s;
			t[slen+1] = 0;
		}
		inline	void	clear(){t[0] = 0;}
		inline	BOOL	isFile(const char* szFile)	//是否同一个路径
		{
			NameT<count> ts;
			ts	=	szFile;
			ts.setIsFile();
			return isFile(ts);
		}
		inline	BOOL	isFile(const NameT<count>& f)	//是否同一个路径
		{
			int length	=	strlen(t);
			int length2	=	strlen(f.t);
			if(length < 1 || length2 < 1 || length != length2)
				return false;
			const char* sz1	=	t;
			const char* sz2	=	f.t;
			return (stricmp(sz1, sz2) == 0);
		}
		inline	VOID	splitpath(NameT<count>* retDir, NameT<count>* retFile, NameT<count>* retExt)
		{
			if(!retDir && !retFile && !retExt)
				return;
			if(retDir)
				retDir->t[0]	=	0;
			if(retFile)
				retFile->t[0]	=	0;
			if(retExt)
				retExt->t[0]	=	0;
			int len	=	strlen(t);
			int lastLen	=	len;
			for(int i=len-1;i>=0;i--)
			{
				if(t[i] == '.')
				{
					if(retExt)
					{
						strncpy(retExt->t, (char*)t + i, lastLen - i);
						retExt->t[lastLen - i]	=	0;
					}
					lastLen	=	i;
				}
				else if(t[i] == '/' || t[i] == '\\' || i == 0)
				{
					if(retFile)
					{
						if(i == 0)
						{
							int len2 = lastLen - i;
							if(len2>0){
								strncpy(retFile->t, (char*)t, len2 + 1);
								retFile->t[len2] = 0;
							}
							return;
						}
						else
						{
							int len2 = lastLen - i - 1;
							if(len2 > 0)
							{
								strncpy(retFile->t, (char*)t + (i + 1), len2);
								retFile->t[len2] = 0;
							}
						}
					}
					if(retDir)
					{
						int len2 = i + 1;
						strncpy(retDir->t, (char*)t, len2);
						retDir->t[len2] = 0;
					}
					return;
				}
			}
		}
	};
	template <int count>
	struct	NameTW
	{
		static	const int	Length	=	count - 1;
		NameTW(){t[0] = 0;}
		short_t				t[count];
		NameTW(const short_t* s){u_strncpy(t, s, Length); t[Length] = 0;}
		inline	operator const short_t*	(){return t;}
		inline	const short_t*	str(){return t;}
		inline	int		size(){return u_strlen(t);}
		inline	void	operator = (const short_t* s)
		{
			u_strncpy(t, s, Length);
			t[Length] = 0;
		}
		inline	void	operator = (const char* s)
		{
			int len = strlen(s);
			len = Utf8ToW(s, t, count);
			t[len] = 0;
			t[Length] = 0;
		}
		inline	void	setIsFile()
		{
			int len	=	u_strlen(t);
			while((t[0] == '/' || t[0] == '\\' || (t[0] == '.' && t[1] != '.')) && t[0] != 0)
			{
				memmove_s(t, t + 1, len);
				len--;
			}
			for(int i=0;i<len;i++)
			{
				if(t[i] == '/')
					t[i] = '\\';
			}
		}
		inline	char	operator == (const short_t* s) const {return (u_stricmp(t, s) == 0);}
		inline	char	operator != (const short_t* s) const {return (u_stricmp(t, s) != 0);}
		//
		inline	void	operator += (const short_t* s)
		{
			int slen = u_strlen(t);
			int nlen = u_strlen(s);
			if((slen + nlen) < (Length))
				u_strcat(t, s);
		}
		inline	void	clear(){t[0] = 0;}
		inline	char	isFile(const short_t* szFile)	//是否同一个路径
		{
			NameTW<count> ts;
			ts	=	szFile;
			ts.setIsFile();
			return isFile(ts);
		}
		inline	char	isFile(const NameTW<count>& f)	//是否同一个路径
		{
			int length	=	u_strlen(t);
			int length2	=	u_strlen(f.t);
			if(length < 1 || length2 < 1 || length != length2)
				return false;
			return (u_stricmp(t, f.t) == 0);
		}
	};

	typedef	NameT<60>	Name;
	typedef	NameT<128>	FileName;
	typedef	NameT<512>	LFileName;	//long file name
	typedef	NameT<32>	FileExt;

	typedef	NameTW<60>	NameW;
	typedef	NameTW<128>	FileNameW;
	typedef	NameTW<32>	FileExtW;
	typedef	NameTW<128>	LFileNameW;

	struct	Text
	{
		Text(){}
		~Text(){}
		CDynamicArray<char,int,1024>	texts;
		Text(const char* s)
		{
			texts.push_back(s, strlen(s));
		}
		inline	operator const char*	()
		{
			if(texts.GetPtr())
				return texts.GetPtr();
			return "";
		}
		inline	void	upper()
		{
			if(texts.size())
				strlwr_t(texts.GetPtr());
		}
		inline	int		size(){return texts.size();}
		inline	void	fill(char c)
		{
			if(texts.size())
				memset(texts.GetPtr(), c, texts.size());
		}
		inline	void	lower()
		{
			if(texts.size())
				strlwr_t(texts.GetPtr());
		}
		inline	const char*	str()
		{
			if(texts.GetPtr())
				return texts.GetPtr();
			return "";
		}
		inline	Text&	operator = (const char* s)
		{
			texts.clear();
			texts.push_back(s, strlen(s));
			return *this;
		}
		inline	char	operator == (const char* s)
		{
			if(texts.size() == 0)
				return false;
			return (stricmp(texts.GetPtr(), s) == 0);
		}
		//
		inline	Text&	operator += (const char* s)
		{
			if(s)
				texts.push_back(s, strlen(s), texts.size());
			return *this;
		}
		void			format(const char* fmt, ...);
		void			add(const char* fmt, ...);
		inline	Text&	operator >> (const char* text)
		{
			this->operator+=(text);
			this->operator+=("\r\n");
			return *this;
		}
		inline	void	clear(){texts.clear();}
	};

	struct	TextW
	{
		TextW(){}
		~TextW(){}
		CDynamicArray<short_t>	texts;
		TextW(const short_t* s)
		{
			texts.push_back(s, u_strlen(s));
		}
		inline	operator const short_t*	()
		{
			return str();
		}
		inline	const short_t*	str()
		{
			if(texts.GetPtr())
				return texts.GetPtr();
			static short_t s[1];
			return s;
		}
		inline	void	upper()
		{
			if(texts.size())
				u_strlwr(texts.GetPtr());
		}
		inline	int		size(){return texts.size();}
		inline	void	fill(char c)
		{
			if(texts.size())
				memset(texts.GetPtr(), c, texts.size() * sizeof(short_t));
		}
		inline	void	lower()
		{
			if(texts.size())
				u_strlwr(texts.GetPtr());
		}
		inline	TextW&	operator = (const short_t* s)
		{
			texts.clear();
			texts.push_back(s, u_strlen(s));
			return *this;
		}
		inline	TextW&	operator = (const char* s)
		{
			texts.clear();
			int len = strlen(s);
			texts.SetArrayCount(len);
			len = Utf8ToW(s, texts.GetPtr(), texts.size());
			texts[len] = 0;
			return *this;
		}
		inline	char	operator == (const short_t* s)
		{
			if(texts.size() == 0)
				return false;
			return (u_stricmp(texts.GetPtr(), s) == 0);
		}
		//
		inline	TextW&	operator += (const short_t* s)
		{
			if(s)
				texts.push_back(s, u_strlen(s), texts.size());
			return *this;
		}
		inline	void	clear(){texts.clear();}
	};
	/*STRUCT*/struct	GameID/*游戏对象ID*/
	{
		unsigned short id;
		unsigned short index;
		/*END_STRUCT*/
		inline	BOOL	operator == (const GameID& g){return (id==g.id&&index==g.index);}
		GameID(){id=index=0;}
		GameID(unsigned short _index, unsigned short _id){id = _id; index = _index;}
	};
	//智能索引
	struct	PtrID
	{
		unsigned int	index;
		unsigned int	id;
		PtrID(){index = id = 0;}
		PtrID(unsigned int _index, unsigned int _id){index = _index; id = _id;}
	};
	#pragma pack(pop)


	struct	AutoPtrData;
	class	Base;
	struct	Ptr
	{
		int				id;
		Base*	ptr;
	};


	//------------------------------------------------------------------------------------------------------------
	//调用接口时候的几点重要点
	//1. 只要有Release就表明释放时在自身函数中调用
	//2. 如果没有Release则需要找到相对应的释放函数进行释放
	struct	PComValue//自动初始化com接口函数
	{
		int						lRef;
		PComValue(){lRef = 1;}
		int						AddRef(){lRef++;return lRef;}
		BOOL					Release(){lRef--;return (lRef <= 0);}
	};

	struct	BufferPointer//数据缓冲指针
	{
		int						length;
		char					buffer[1];
	};
	//all base
	class	Base
	{
	public:
		Base();
		virtual ~Base();
		virtual		int			AddRef(){return m_ref.AddRef();}
		virtual		int			Release(){if(m_ref.Release()){delete this; return 0;} return m_ref.lRef;};
		inline	const PtrID&	GetPtrID(){return m_baseid;}
		int						m_context;
	protected:
		PComValue				m_ref;
	private:
		PtrID					m_baseid;
	};
	template <typename T>
	class BaseT: public Base
	{
	public:
		typedef T*	thisType;
		T* ptr;
		ImplementComValue();
	};
	//接口自动操作
	template <typename T>
	struct	AutoPtr
	{
		T*				t;
		AutoPtr(){t	=	0;}
		AutoPtr(T* _t){t = 0; Set(_t);}
		~AutoPtr(){safe_release(t);}
		inline	operator		T*	(){return t;}
		inline	AutoPtr<T>&	operator = (const AutoPtr<T>& _t)
		{
			Set(_t.t);
			return *this;
		}
		inline	operator	BOOL(){return (t != 0);}
		inline	T*	operator->(){return t;}
		inline	AutoPtr<T>&	operator = (T* _t)
		{
			Set(_t);
			return *this;
		}
		inline	VOID	Set(T* _t)
		{
			safe_addref(_t);
			safe_release(t);
			t	=	_t;
		}
	};

	//------------------------------------------------------------------------------------------------------------
	//stream base
	class	StreamBase//文件流接口，读写文件，但是不能写入到文件中
	{
	public:
		enum{
			Seek_Set	=	0,//从0位置开始移动文件写入位置
			Seek_Cur	=	1,//从当前位置移动用户指定单位的位置
			Seek_End	=	2
		};//
		//
		StreamBase(){}
		virtual ~StreamBase(){}
		virtual	BOOL				readOnly()	=	0;//
		virtual	int64				write(const void* t,int64 tLength)	=	0;		//将数据写入到流中
		virtual	int64				read(void* t,int64 tLength)			=	0;		//将数据从流中读取
		virtual	int					seek(int64 numSeek, int seekType = StreamBase::Seek_Set)	=	0;		//设置当前读取的位置
		virtual	int64				tell()	=	0;		//返回当前读取的位置
		virtual	int64				size()	=	0;		//返回文件大小
		virtual	BOOL				eof()	=	0;		//是否在结尾
		//
		virtual	char*				getMemoryPtr()	=	0;	//如果是内存流，返回内存地址
	};
	////////////////////////////////////////////////////////////////////////////
	//TODO: _shader_var_name	说明:
	//_shader_var_name
	//日期:2006-12-26 22:05:55
	////////////////////////////////////////////////////////////////////////////
	struct	_shader_var_name:public _shader_var
	{
		Name					name;
		void					SetName(const char* _name)	{name = _name; reset_handle();}
		_shader_var_name(const char* str)		{name = str;}
		_shader_var_name()						{}
	};

	/*********************************************************************************************************************/
	//global functions
	class	TableTxt;
	StreamBase*							open_s(const char* szFile, BOOL bUseMemoryFile = TRUE);	//load file from szFile, return StreamBase
	StreamBase*							open_s_inDoc(const char* szFile, BOOL bUseMemoryFile = TRUE);	//load file from szFile in document path, return StreamBase
	StreamBase*							open_mem_s(const char* szFile);//从内存打开文件
	StreamBase*							create_mem_s(const char* szFile, int64 nLength);		//create file in memory
	StreamBase*							open_memory(const char* buffer, int64 nLength);//使用内存数据打开文件，会自动拷贝参数中的内存数据
	StreamBase*							create_s(const char* szFile);		//create file in document
	BOOL								open_s_full(const char* szFile, CDynamicArray<char>& rets);	//load file from szFile, return array
	void								close_s(StreamBase** file);		//create file in memory
	void								createFolders(const char* documents, const char* szSubFolders);//创建整个目录到documents文件夹
	void								removeFolders(const char* documents, const char* szSubFolders);
	void								searchFiles(CDynamicArray<FileName>& rets, const char* documents, const char* szSubFolders, int subfolder);
	void								removeFile(const char* szFile);
	TableTxt*							loadTable_s(const char* szTable, char chSymbol = '\t');		//读取表格文件
	class	ZipFilePackage;
	ZipFilePackage*						open_zip(const char* szZipFile, const char* szFirstDir = 0);
	ZipFilePackage*						open_zip_mem(const char* szData, int length, const char* szFirstDir = 0);
	VOID								close_zip(ZipFilePackage* szZipFile);
	VOID								close_zip(const char* szZipFile);
	BOOL								fexist(const char* szFile, LFileName* retNewPath = 0);	//if file exist
	BOOL								fexist_mem(const char* szFile, LFileName* retNewPath = 0);	//if file exist
	BOOL								fexist_doc(const char* szFile, LFileName* retNewPath = 0);	//if file exist
	VOID								SetDocumentDir(const char* szDir);
    const char*                         GetDocumentDir();
	VOID								FreeAllMemoryFiles();//释放所有创建的内存文件
	VOID								Uncompress(char* newbuf, int& origsize, const char* inbuf, int length);//解压缩数据
	//
	class	BaseDirectory: public Base{
	public:
		ImpComValue2();
		BaseDirectory(){}
		virtual	~BaseDirectory(){}
		virtual	BOOL				fexist(const char* szFile) = 0;
		virtual	StreamBase*			open(const char* szFile) = 0;
	};

	struct	CFileHelper	//文件读取封装
	{
		StreamBase*					pStream;//文件流接口
		BOOL						bSelfOpen;
		BOOL						bCheck;
		//
		CFileHelper(StreamBase* p)	{pStream = p; bSelfOpen = false; bCheck = false;}
		CFileHelper()					{pStream = 0; bSelfOpen = false; bCheck = false;}
		~CFileHelper()				{if(bSelfOpen)close();}
		//
		virtual	BOOL				readOnly(){if(pStream)return pStream->readOnly(); return false;}
		virtual	int					seek(int64 numSeek, int seekType = StreamBase::Seek_Set){if(pStream)return pStream->seek(numSeek, seekType);return 0;}		//设置当前读取的位置
		virtual	int64				tell(){if(pStream)return pStream->tell(); return 0;}
		virtual	int64				size(){if(pStream)return pStream->size(); return 0;}
		virtual	BOOL				eof(){if(pStream)return pStream->eof(); return 0;}
		//
		virtual	char*				getMemoryPtr(){if(pStream)return pStream->getMemoryPtr(); return 0;}
		//
		inline	void			close(){if(pStream)close_s(&pStream);pStream = 0; bSelfOpen = false;}//关闭文件
		inline	BOOL			open(const char* szFile, BaseDirectory* dir = 0){
			close();
			if(dir)
				pStream = dir->open(szFile);
			else
				pStream = open_s(szFile,true);
			bSelfOpen = true;
			return (pStream!=NULL);
		}//打开文件,_map指定是否从zip文件读取
		inline	BOOL			openInDoc(const char* szFile){ close(); pStream = open_s_inDoc(szFile,true); bSelfOpen = true; return (pStream!=NULL);}//打开文件,_map指定是否从zip文件读取
		inline	BOOL			create(const char* szFile){ close(); pStream = create_s(szFile); bSelfOpen = true; return (pStream!=NULL);}//创建文件,只能创建到文件中,不支持创建到zip文件中
		//
		inline	void			writeHead(char v1,char v2,char v3,char v4);		//将文件头写入到文件中
		inline	BOOL			checkHead(char v1,char v2,char v3,char v4);		//检查文件头是否和认定的一样
		inline	BOOL			wstr(const char* str,int size = 0);		//将ansi字符串写入到文件流中,如果size为0则表示自动计算字符串长度
		inline	BOOL			rstr(std::string&	str);		//读取ansi字符串
		//读取unicode字符串
		inline	BOOL			rstr(CDynamicArray<short_t>&	str)
		{
			if(!pStream)return FALSE;
			int length = 0;
			if(pStream->read(&length,sizeof(unsigned int))>0)
			{
				str.SetArrayCount(length);
				pStream->read(str.GetPtr(), length*sizeof(short_t));
				return TRUE;
			}
			return FALSE;
		}
		inline const char*		r_text();
		inline	char*			loadToBuffer(int& lengthOut);		//将文件全部读取并写入到一个内存段中，并返回其指针，如果指针为空则表示没有返回任何值
		inline	void			releaseBuffer(char* buf);		//释放loadToBuffer中返回的指针
		//读写类型到文件流
	};


	struct	CFileHelperW: public CFileHelper	//文件写入封装
	{
		CFileHelperW(StreamBase* p):CFileHelper(p){}
		CFileHelperW()				{pStream = 0;}
		~CFileHelperW()				{}
		//
		virtual	int64				write(const void* t,int64 tLength){if(pStream)return pStream->write(t, tLength); return 0;}		//将数据写入到流中
		template	<int Tcount>	inline	int	str(NameT<Tcount>& t)
		{
			if(pStream)
			{
				int len = t.size();
				pStream->write(&len, sizeof(int));
				return pStream->write(t.t, len) + sizeof(int);
			}
			return 0;
		}
		//读写类型到文件流
		template	<class	_type>	inline	size_t	w(const _type& t)
		{
			if(pStream)
			{
				if(bCheck && sizeof(_type) <= 4)
				{
					int n = *(int*)&t;
					pStream->write(&n, sizeof(int));
					pStream->write(&n, sizeof(int));
				}
				return pStream->write(&t,sizeof(_type));
			}
			return 0;
		}//写入一个数据类型到文件
		template	<class	_type>	inline	size_t	w(const _type* t,int nrange)
		{
			if(pStream)
			{
				if(bCheck)
				{
					int len = sizeof(_type) * nrange;
					pStream->write(&len, sizeof(int));
				}
				return pStream->write(t,sizeof(_type)*nrange);
			}
			return 0;
		}//写入一个数组到文件,nrange指定数组的长度
		//
		//读写类型到文件流
		template	<class	_type>	inline	CFileHelperW&	operator << (const _type& t)
		{
			if(pStream)
			{
				if(bCheck && sizeof(_type) <= 4)
				{
					int n = *(int*)&t;
					pStream->write(&n, sizeof(int));
					pStream->write(&n, sizeof(int));
				}
				pStream->write(&t,sizeof(_type));
			}
			return *this;
		}//写入一个数据类型到文件
		inline	CFileHelperW&								operator << (const char* str){wstr(str); return *this;}//写入一段文本到文件
		inline	CFileHelperW&								operator << (char* str){wstr(str); return *this;}//写入一段文本到文件

		template	<class	_type>	inline	CFileHelperW&	operator << (std::vector<_type>& t)//写入一段数组到文件中
		{
			int num = (int)t.size();
			w(num);
			for(int i=0;i<num;i++)
				w(t.at(i));
			return *this;
		}
	};


	struct	CFileHelperR: public CFileHelper	//文件读取封装
	{
		CFileHelperR(StreamBase* p):CFileHelper(p)	{}
		CFileHelperR()				{pStream = 0;}
		~CFileHelperR()				{}
		//
		template	<int Tcount>	inline	int	str(NameT<Tcount>& t)
		{
			if(pStream)
			{
				int len = 0;
				pStream->read(&len, sizeof(int));
				int ret = pStream->read(t.t, len) + sizeof(int);
				t.t[len] = 0;
				return ret;
			}
			return 0;
		}
		//
		virtual	int64				read(void* t,int64 tLength){if(pStream)return pStream->read(t, tLength); return 0;}
		//读写类型到文件流
		template	<class	_type>	inline	size_t	r(_type& t)
		{
			if(pStream)
			{
				if(bCheck && sizeof(_type) <= 4)
				{
					int n1,n2;
					pStream->read(&n1, sizeof(int));
					pStream->read(&n2, sizeof(int));
					assert(n1 == n2);
				}
				return pStream->read(&t,sizeof(_type));
			}
			return 0;
		}//读取一个数据类型
		template	<class	_type>	inline	size_t	r(_type* t,int nrange)
		{
			if(pStream)
			{
				if(bCheck)
				{
					int len;
					pStream->read(&len, sizeof(int));
					assert(len == sizeof(_type) * nrange);
				}
				return pStream->read(t,sizeof(_type)*nrange);
			}
			return 0;
		}//读取一个数组,nrange指定数组的长度
		//
		const char*			ReadText(char* buf)
		{
			int len = 0;
			pStream->read(&len,sizeof(int));
			buf[0] = 0;
			if(len >= 8192)
				return "";
			pStream->read(buf, len);
			buf[len] = 0;
			return buf;
		}
		//inline	CFileHelperR&	operator >>(Name& t){
		//	return this->operator>>(t);
		//}//读取一段数据
		//inline	CFileHelperR&	operator >>(FileName& t){
		//	return this->operator>>(t);
		//}//读取一段数据
		//template	<int _type>
		//inline	CFileHelperR&	operator >>(NameT<_type>& t){
		//	char buf[8192];
		//	ReadText(buf);
		//	t = buf;
		//	return *this;
		//}//读取一段数据
		inline	CFileHelperR&	readt(char* buf){
			ReadText(buf);
			return *this;
		}//读取一段数据
		template	<class	_type>	inline	CFileHelperR&	operator >>(_type& t)
		{
			if(pStream)
			{
				if(bCheck && sizeof(_type) <= 4)
				{
					int n1,n2;
					pStream->read(&n1, sizeof(int));
					pStream->read(&n2, sizeof(int));
					assert(n1 == n2);
				}
				pStream->read(&t,sizeof(_type));
			}
			return *this;
		}//读取一段数据
		inline	CFileHelperR&								operator >>(std::string&	str){rstr(str); return *this;}//读取一段文本
		template	<class	_type>	inline	CFileHelperR&	operator >> (std::vector<_type>& t)//读取数组,然后返回到t里面
		{
			int num;
			r(num);
			for(int i=0;i<num;i++)
			{
				_type tt;
				r(tt);
				t.push_back( tt );
			}
			return *this;
		}
		inline	CFileHelperR&								operator >> (std::vector<std::string>& t)//读取文本列表,然后返回到t里面
		{
			int num = (int)t.size();
			r(num);
			for(int i=0;i<num;i++)
			{
				std::string str;
				rstr(str);
				t.push_back( str );
			}
			return *this;
		}
	};

	class	CSafeFileHelper//安全文件访问接口
	{
	public:
		StreamBase*		pStream;
		BOOL			bReadOnly;
		int				fileHead;
		BOOL			bSelfOpen;
		//
		CSafeFileHelper(StreamBase* p)	{pStream = p; bSelfOpen = false;}
		CSafeFileHelper(){pStream = 0; bReadOnly = true; fileHead = 0; bSelfOpen = false;}
		~CSafeFileHelper(){close();}
		//
		inline	void			close()
		{
			if(pStream && bSelfOpen)
				close_s(&pStream);
			pStream		=	0;
			bSelfOpen	=	false;
		}
		inline	BOOL			open_mem(const char* buffer, int length)
		{
			close();
			bReadOnly	=	true;
			pStream		=	open_memory(buffer, length);
			bSelfOpen	=	true;
			return (pStream!=NULL);
		}
		inline	BOOL			openInDoc(const char* szFile)
		{
			close();
			pStream = open_s_inDoc(szFile,true);
			bSelfOpen = true;
			return (pStream!=NULL);
		}//打开文件,_map指定是否从zip文件读取
		inline	BOOL			open(const char* szFile, BOOL _bReadOnly = true, BaseDirectory* dir = 0)
		{
			close();
			bReadOnly	=	_bReadOnly;
			if(dir)
				pStream	=	dir->open(szFile);
			else
				pStream		=	open_s(szFile, _bReadOnly);
			bSelfOpen	=	true;
			return (pStream!=NULL);
		}
		inline	BOOL			create(const char* szFile)
		{
			close();
			bReadOnly	=	false;
			pStream		=	create_s(szFile);
			bSelfOpen	=	true;
			return (pStream!=NULL);
		}
		//
	};

	class CSafeFileHelperW: public CSafeFileHelper//安全写入接口,安全写入接口表示每段写入都设定好检查选项,等用户读取数据时如果出现不一致的情况则直接返回并提示读取错误,主要用于避免人为修改文件导致程序异常.
	{
	public:
		CSafeFileHelperW(StreamBase* p):CSafeFileHelper(p){}
		CSafeFileHelperW(){}
		~CSafeFileHelperW(){close();}
		template	<class	_type>	inline	int	w(const _type& t){if(pStream)return pStream->write(&t,sizeof(_type));return 0;}
		template	<class	_type>	inline	int	w(const _type* t,int nrange){if(pStream)return pStream->write(t,sizeof(_type)*nrange);return 0;}
		template	<int Tcount>	inline	int	str(NameT<Tcount>& t)
		{
			if(pStream)
			{
				int len = t.size();
				pStream->write(&len, sizeof(int));
				return pStream->write(t.t, len) + sizeof(int);
			}
			return 0;
		}
		CSafeFileHelperW&				operator	<< (char* ch)
		{
			assert(!bReadOnly); int len = (int)strlen(ch) + 1;
			pStream->write(&len, sizeof(int));
			pStream->write(&len, sizeof(int));
			pStream->write(ch, len);
			return *this;
		}
		CSafeFileHelperW&				operator	<< (const char* ch)
		{
			assert(!bReadOnly); int len = (int)strlen(ch) + 1;
			pStream->write(&len, sizeof(int));
			pStream->write(&len, sizeof(int));
			pStream->write(ch, len);
			return *this;
		}
		CSafeFileHelperW&				operator	<< (std::string& str)
		{
			assert(!bReadOnly); int len = (int)str.size() + 1;
			pStream->write(&len, sizeof(int));
			pStream->write(&len, sizeof(int));
			pStream->write(str.c_str(), len);
			return *this;
		}
		template < typename T >
		CSafeFileHelperW&				operator	<< (const T& ch)
		{
			assert(!bReadOnly);
			int len = sizeof(T);
			pStream->write(&len, sizeof(int));
			pStream->write(&ch, len);
			return *this;
		}
		template < typename T >
		CSafeFileHelperW&				operator	<< (CDynamicArray<T>& ch)
		{
			assert(!bReadOnly);
			int len = sizeof(T) * ch.size();
			int len2 = sizeof(T);
			int size = ch.size();
			pStream->write(&size, sizeof(int));
			pStream->write(&len, sizeof(int));
			pStream->write(&len2, sizeof(int));
			if(len > 0)
				pStream->write(ch.GetPtr(), len);
			return *this;
		}

	};

	class CSafeFileHelperR: public CSafeFileHelper//安全读取接口,安全写入接口表示每段写入都设定好检查选项,等用户读取数据时如果出现不一致的情况则直接返回并提示读取错误,主要用于避免人为修改文件导致程序异常.
	{
	public:
		CSafeFileHelperR():CSafeFileHelper(){bError = false;}
		~CSafeFileHelperR(){close();}
		BOOL	bError;
		inline	BOOL	IsError(){return bError;}
		template	<int Tcount>	inline	int	str(NameT<Tcount>& t)
		{
			if(pStream)
			{
				int len = 0;
				pStream->read(&len, sizeof(int));
				int ret = pStream->read(t.t, len) + sizeof(int);
				t.t[len] = 0;
				return ret;
			}
			return 0;
		}

		CSafeFileHelperR&				operator	>> (std::string& ch)
		{
			int len,lenCheck;
			pStream->read(&len, sizeof(int));
			pStream->read(&lenCheck, sizeof(int));
			assert(len == lenCheck);
			if(len != lenCheck)
			{
				ThrowError();
				bError = true;
				return *this;
			}

			static char szBuffer[1024] = {0};
			if(len < 1024)
			{
				pStream->read(szBuffer, len);
				ch = szBuffer;
			}
			else
			{
				char* s = new char[len + 1];
				pStream->read(s, len);
				ch = s;
				delete s;
			}
			return *this;
		}
		CSafeFileHelperR&				operator	>> (CDynamicArray<short_t>& ch)
		{
			int len,lenCheck;
			pStream->read(&len, sizeof(int));
			pStream->read(&lenCheck, sizeof(int));
			assert(len == lenCheck);
			if(len != lenCheck)
			{
				bError = true;
				ThrowError();
				return *this;
			}
			ch.SetArrayCount(len);
			pStream->read(ch.GetPtr(), len * sizeof(short_t));
			return *this;
		}
		const char*		r_text()
		{
			int len,lenCheck;
			pStream->read(&len, sizeof(int));
			pStream->read(&lenCheck, sizeof(int));
			assert(len == lenCheck);
			if(len != lenCheck)
			{
				bError = true;
				ThrowError();
				return 0;
			}
			static char szBuffer[1024] = {0};
			if(len < 1024)
			{
				pStream->read(szBuffer, len);
				return szBuffer;
			}
			return 0;
		}
		template < typename T >
		CSafeFileHelperR&				operator	>> (T& ch)
		{
			int typeLen	=	sizeof(T);
			int len;
			pStream->read(&len, sizeof(int));
			assert(len == typeLen);
			if(len != typeLen)
			{
				bError = true;
				ThrowError();
				return *this;
			}
			pStream->read(&ch, len);
			return *this;
		}
		template < typename T >
		CSafeFileHelperR&				operator	>> (CDynamicArray<T>& ch)
		{
			int len = 0;
			int len2 = 0;
			int size = 0;
			pStream->read(&size, sizeof(int));
			pStream->read(&len, sizeof(int));
			pStream->read(&len2, sizeof(int));
			if(len > 0)
			{
				assert(len2 == (len / size));
				if(len2 != (len / size))
				{
					bError = true;
					ThrowError();
					return *this;
				}
				ch.SetArrayCount(size);
				pStream->read(ch.GetPtr(), len);
			}
			return *this;
		}
	};

	struct	streamWrite//根据用户提供的固定数据中进行数据流快速写入操作
	{
		CDynamicArray<char> buffers;
		BOOL	m_bUseBuffer;
		char*	m_ptr;
		int		m_length;
		int		m_point;
		inline	int		size(){return m_point;}//返回写入的文件大小
		inline	char*	buffer(){return m_ptr;}
		streamWrite()
		{
			m_bUseBuffer	=	false;
			m_ptr		=	0;
			m_length	=	0;
			m_point		=	0;
		}
		streamWrite(int nInitBuffer)
		{
			m_bUseBuffer	=	true;
			m_ptr		=	0;
			m_length	=	0;
			m_point		=	0;
			buffers.SetArrayCount(nInitBuffer);
			m_length	=	buffers.size();
			m_ptr		=	buffers.GetPtr();
		}
		streamWrite(char* ptr, int length)
		{
			m_bUseBuffer	=	false;
			m_ptr		=	ptr;
			m_length	=	length;
			m_point		=	0;
		}
		~streamWrite()				{}
		//
		template <typename T> inline	T*	stream(int point){return (T*)(m_ptr + point);}
		inline		void			write(const void* ptr, int length)
		{
			if(m_bUseBuffer)
			{
				if((m_point + length) >= m_length)
				{
					buffers.SetArrayCount(m_point + length + 10240);
					m_ptr = buffers.GetPtr();
					m_length	=	buffers.size();
				}
			}
			if(m_point + length > m_length)return;
			memcpy(m_ptr + m_point, ptr, length);
			m_point			+=	length;
		}
		inline	streamWrite&	operator << (char* str)
		{
			return this->operator <<((const char*)str);
		}
		inline	streamWrite&	operator << (const char* str)
		{
			unsigned short length = (unsigned short)strlen(str);
			write(&length, sizeof(unsigned short));
			write(str, length);
			return *this;
		}
		inline		void			reset(){m_point = 0;}	//重置读取位置
		//读写类型到文件流
		template	<class	_type>	inline	int	w(const _type& t){write(&t,sizeof(_type));return 0;}
		template	<class	_type>	inline	int	w(const _type* t,int nrange){write(t,sizeof(_type)*nrange);return 0;}
		//
		//读写类型到文件流
		template	<class	_type>	inline	streamWrite&	operator << (const _type& t){write(&t,sizeof(_type));return *this;}
	};


	struct	streamRead//根据用户提供的固定数据中进行数据流快速读取操作
	{
		CDynamicArray<char> buffers;
		BOOL		m_bUseBuffer;
		const char*	m_ptr;
		int		m_length;
		int		m_point;
		inline	int			size(){return m_length;}//返回文件大小
		inline	const char*	buffer(){return m_ptr;}
		streamRead(const char* ptr, int length)
		{
			m_ptr		=	ptr;
			m_length	=	length;
			m_point		=	0;
		}
		streamRead()
		{
			m_ptr		=	0;
			m_length	=	0;
			m_point		=	0;
		}
		streamRead(int nLength)
		{
			m_bUseBuffer	=	true;
			buffers.SetArrayCount(nLength);
			m_ptr		=	buffers.GetPtr();
			m_length	=	buffers.size();
			m_point		=	0;
		}
		~streamRead()				{}
		//
		inline		void			read(void* ptr, int length)
		{
			if(m_point + length > m_length)return;
			memcpy(ptr, m_ptr + m_point, length);
			m_point			+=	length;
		}
		inline	streamRead&	operator >>(std::string& str)
		{
			unsigned short l;
			read(&l, sizeof(unsigned short));
			static char buf[512];
			if(l < 511)
			{
				read(buf, l);
				buf[l] = 0;
				str	=	buf;
			}
			else
			{
				char* pbuf = new char[l + 1];
				read(pbuf, l);
				pbuf[l] =	0;
				str		=	pbuf;
				delete[] pbuf;
			}
			return *this;
		}
		inline	const char*	r_text()
		{
			unsigned short l;
			read(&l, sizeof(unsigned short));
			static char buf[1024];
			if(l < 1024)
			{
				read(buf, l);
				buf[l] = 0;
				return buf;
			}
			return 0;
		}
		//读写类型到文件流
		template	<class	_type>	inline	int	r(_type& t){read(&t,sizeof(_type));return 0;}
		template	<class	_type>	inline	int	r(_type* t,int nrange){read(t,sizeof(_type)*nrange);return 0;}
		//
		template	<class	_type>	inline	streamRead&	operator >>(_type& t){read(&t,sizeof(_type));return *this;}
	};

	inline	void			CFileHelper::writeHead(char v1,char v2,char v3,char v4)//将文件头写入到文件中
	{
		unsigned int dwHead=_MakeFileHead(v1,v2,v3,v4);
		if(pStream)pStream->write(&dwHead,sizeof(int));
	}

	inline	BOOL			CFileHelper::checkHead(char v1,char v2,char v3,char v4)//检查文件头是否和认定的一样
	{
		if(!pStream)return false;
		unsigned int dwHead;
		pStream->read(&dwHead,sizeof(int));
		return ((dwHead&0x00ffffff)&(_MakeFileHead(v1,v2,v3,v4)&0x00ffffff))>0;
	}

	inline	BOOL			CFileHelper::wstr(const char* str,int size)//将ansi字符串写入到文件流中,如果size为0则表示自动计算字符串长度
	{
		if(!str||!pStream)return false;
		if(size<=0)
			size = strlen(str)+1;
		else
			size++;
		int w = pStream->write(&size,sizeof(int));
		w += pStream->write(str,size);
		return (w>0);
	}

	inline	BOOL			CFileHelper::rstr(std::string&	str)//读取ansi字符串
	{
		if(!pStream)return false;
		int length = 0;
		if(pStream->read(&length,sizeof(int))>0)
		{
			if(length > 0)
			{
				str.resize(length);
				pStream->read(&str.at(0), length);
				str[str.size() - 1] = 0;
			}
			return true;
		}
		return false;
	}
	inline const char*		CFileHelper::r_text()
	{
		if(!pStream)return 0;
		int length = 0;
		if(pStream->read(&length,sizeof(int))>0)
		{
			if(length < 1024)
			{
				static char readPtr[1024];
				pStream->read(readPtr, length);
				return readPtr;
			}
			else
			{
			}
			return 0;
		}
		return 0;
	}
	//将文件全部读取并写入到一个内存段中，并返回其指针，如果指针为空则表示没有返回任何值
	inline	char*			CFileHelper::loadToBuffer(int& lengthOut)
	{
		if(!pStream)return NULL;
		lengthOut = (int)pStream->size();//
		if(lengthOut<=0)
			return 0;
		char* buf = new char[lengthOut+1];
		pStream->read(buf,lengthOut);
		buf[lengthOut] = 0;
		return buf;
	}
	//释放loadToBuffer中返回的指针
	inline	void			CFileHelper::releaseBuffer(char* buf)
	{
		if(buf)delete buf;
	}


	template <typename T, class Tmanager>
	class	DataManager
	{
	public:
		DataManager(){m_managerPtr = 0;}
		virtual	~DataManager()
		{
			for(int i=0;i<(int)m_lists.size();i++)
				delete m_lists[i];
		}
		VOID						ClearDatas()
		{
			for(int i=0;i<(int)m_lists.size();i++)
				delete m_lists[i];
			m_lists.clear();
		}
		VOID						SetManagerPtr(Tmanager* s){m_managerPtr = s;}
		//
		T*							GetDataPtr(PtrID& id){return FindDataPtr(id);}
		T*							AddData(const T* d)
		{
			T* t	=	new T();
			if(d)
				*t = *d;
			t->id	=	m_managerPtr->GetNewIdentity();
			sprintf(t->name.t, "data%d", t->id);
			m_lists.push_back(t);
			return t;
		}
		BOOL						SetData(PtrID& id, const T& d)
		{
			T* t		=	FindDataPtr(id);
			if(!t)
				return false;
			int idSave	=	t->id;
			*t			=	d;
			t->id		=	idSave;
			return true;
		}
		BOOL						RemoveData(PtrID& id)
		{
			T* t	=	FindDataPtr(id);
			if(!t)
				return false;
			delete m_lists[id.index];
			m_lists.eraseByIndex(id.index);
			return true;
		}
		T*							GetDataPtr(int index)
		{
			if(index < 0 || index >= (int)m_lists.size())
				return 0;
			return m_lists[index];
		}
		int							GetDataCount(){return m_lists.size();}
		//
		T*							SearchDataPtr(int id)
		{
			PtrID d;
			d.id = id;
			return FindDataPtr(d);
		}
		//
		T*							SearchDataByName(const char* szName)
		{
			for(int i=0;i<(int)m_lists.size();i++)
			{
				if(m_lists[i]->name == szName)
					return m_lists[i];
			}
			return 0;
		}
		T*							FindDataPtr(PtrID& id)
		{
			if(id.id <= 0)
				return 0;
			if(id.index >= 0 && id.index < (int)m_lists.size())
				if(m_lists[id.index]->id == id.id)
					return m_lists[id.index];
			for(int i=0;i<(int)m_lists.size();i++)
			{
				if(m_lists[i]->id == id.id)
				{
					id.index	=	i;
					return m_lists[i];
				}
			}
			id.id = 0;
			return 0;
		}
		VOID					Load(CSafeFileHelperR& r)
		{
			char version = 1;
			int count;
			r >> version >> count;
			if(version == 1)
			{
				for(int i=0;i<count;i++)
				{
					T* t	=	new T();
					r >> *t;
					m_lists.push_back(t);
				}
			}
			else
			{
				int length	=	sizeof(T);
				r >> length;
				for(int i=0;i<count;i++)
				{
					T* t	=	new T();
					r.pStream->read(t, length);
					m_lists.push_back(t);
				}
			}
		}
		VOID					Save(CSafeFileHelperW& w)
		{
			char version = 2;
			int count	=	(int)m_lists.size();
			w << version << count;
			int length	=	sizeof(T);
			w << length;
			for(int i=0;i<m_lists.size();i++)
				w.pStream->write(m_lists[i], length);
		}
		VOID					Load(CFileHelperR& r)
		{
			char version = 1;
			int count;
			r >> version >> count;
			if(version == 1)
			{
				for(int i=0;i<count;i++)
				{
					T* t	=	new T();
					r >> *t;
					m_lists.push_back(t);
				}
			}
			else
			{
				int length	=	sizeof(T);
				r >> length;
				for(int i=0;i<count;i++)
				{
					T* t	=	new T();
					r.pStream->read(t, length);
					m_lists.push_back(t);
				}
			}
		}
		VOID					Save(CFileHelperW& w)
		{
			char version = 2;
			int count	=	(int)m_lists.size();
			w << version << count;
			int length	=	sizeof(T);
			w << length;
			for(int i=0;i<m_lists.size();i++)
			{
				w << *m_lists[i];
			}
		}
		void	operator	=	(DataManager<T,Tmanager>& from)
		{
			for(int i=0;i<m_lists.size();i++)
			{
				delete m_lists[i];
			}
			m_lists.clear();
			//
			for(int i=0;i<from.m_lists.size();i++)
				this->AddData(from.m_lists[i]);
		}
	protected:
		Tmanager*					m_managerPtr;
		CDynamicArray<T*>			m_lists;
	};
	//管理基础类
	template <class T, class Tmgr>
	class classT: public T
	{
	public:
		classT()
		{
			m_id	=	0;
			m_mgr	=	0;
			T::m_ref.lRef = 0;	//计数器忽略创建的时候那一次,确保创建几次就会被释放几次计数器
		}
		inline	int		GetID(){return m_id;}
		inline	VOID	Init(Tmgr* mgr, int id)
		{
			if(m_mgr)
				m_mgr->Pop(this);
			m_mgr = mgr;
			if(m_mgr)
				m_mgr->Push(this);
			m_id = id;
		}
	protected:
		virtual ~classT()
		{
			if(m_mgr)
				m_mgr->Pop(this);
		}
	protected:
		int		m_id;
		Tmgr*	m_mgr;
	};

	template <class T>
	class classMgr
	{
	public:
		classMgr(){m_id = 1;}
		~classMgr(){}
		inline	void		setSeed(int id){m_id = id;}
		inline	int			size(){return m_lists.size();}
		inline	classT<T, classMgr<T> >*	operator[](int index){return m_lists[index];}
		inline	classT<T, classMgr<T> >*	operator[](PtrID& id){return GetPtr(id);}
		//
		inline	int			GetNewID(){return (m_id++);}
		classT<T, classMgr<T> >*	addNew(int* id, classT<T, classMgr<T> > * c)
		{
			int _id	=	m_id++;
			if(id)
				*id	=	_id;
			c->Init(this, _id);
			return c;
		}
		VOID						freeAll()
		{
			CDynamicArray< classT< T, classMgr<T> >*> temp = m_lists;
			m_lists.clear();
			for(int i=0;i<(int)temp.size();i++)
				while(temp[i]->Release() != 0){}
		}
		classT<T, classMgr<T> >*			GetPtr(PtrID& id)
		{
			if(id.id <= 0)
				return 0;
			if(id.index >= 0 && id.index < (int)m_lists.size())
				if(m_lists[id.index]->GetID() == id.id)
					return m_lists[id.index];
			for(int i=0;i<(int)m_lists.size();i++)
			{
				if(m_lists[i]->GetID() == id.id)
				{
					id.index	=	i;
					return m_lists[i];
				}
			}
			id.id = 0;
			return 0;
		}
		VOID						AddRefPtr(PtrID& id)
		{
			T* t	=	GetPtr(id);
			safe_addref(t);
		}
		VOID						ReleasePtr(PtrID& id)
		{
			T* t	=	GetPtr(id);
			safe_release(t);
		}
		T*							SearchDataPtr(int id)
		{
			PtrID d;
			d.id = id;
			return GetPtr(d);
		}
	public:
		VOID						Push(classT< T, classMgr<T> >* t)
		{
			m_lists.push_back(t);
		}
		VOID						Pop(classT< T, classMgr<T> >* t)
		{
			for(int i=0;i<m_lists.size();i++)
			{
				if(m_lists[i] == t)
				{
					m_lists.eraseByIndex(i);
					break;
				}
			}
		}

		CDynamicArray< classT < T, classMgr<T> >*>	m_lists;
	protected:
		int											m_id;
	};
	//客户使用
	template <class T>
	class	classPtr
	{
	public:
		classPtr()
		{
			m_mgr	=	0;
		}
		classPtr(classMgr<T>* mgr, int id)
		{
			m_mgr		=	mgr;
			m_id.id		=	id;
			if(m_mgr)
				m_mgr->AddRefPtr(m_id);
		}
		classPtr(const classPtr<T>& c)
		{
			m_mgr	=	0;
			m_mgr	=	c.m_mgr;
			m_id	=	c.m_id;
			if(m_mgr)
				m_mgr->AddRefPtr(m_id);
		}
		inline	void	set(classMgr<T>& _mgr, int id)
		{
			PtrID newID;
			newID.id	=	id;
			_mgr.AddRefPtr(newID);
			if(m_mgr)
				m_mgr->ReleasePtr(m_id);
			m_mgr	=	&_mgr;
			m_id	=	newID;
		}
		~classPtr()
		{
			if(m_mgr)
				m_mgr->ReleasePtr(m_id);
		}
		inline	void	operator	=	(const int id)
		{
			PtrID oldID	=	m_id;
			m_id.id	=	id;
			if(m_mgr)
			{
				m_mgr->AddRefPtr(m_id);
				m_mgr->ReleasePtr(oldID);
			}
		}
		inline	void	operator	=	(const classPtr<T>& c)
		{
			if(c.m_mgr)
			{
				PtrID tempID = c.m_id;
				c.m_mgr->AddRefPtr(tempID);
			}
			if(m_mgr)
				m_mgr->ReleasePtr(m_id);
			m_mgr	=	c.m_mgr;
			m_id	=	c.m_id;
		}
		inline	T*		GetPtr()
		{
			if(!m_mgr || m_id.id <= 0)
				return 0;
			return m_mgr->GetPtr(m_id);
		}
		inline	operator	BOOL(){return (GetPtr() != NULL);}
		inline	T*		operator ->(){return GetPtr();}
		inline	operator	T*(){return GetPtr();}
		//
		inline	const	PtrID&	GetID(){return m_id;}
	protected:
		PtrID			m_id;
		classMgr<T>*	m_mgr;
	};
	struct	PFileList
	{
		std::vector<FileName>	vFiles;
	};
	struct	TextureRect
	{
		Name			name;
		Rect			rc;
		Pixel			off;
		Size			orig;
	};

	template <int count>
	class	NameAWT
	{
	public:
		static	const int	MaxLengthA	=	count * 2 - 1;
		static	const int	MaxLengthW	=	count - 1;
		NameAWT()
		{
			tA[0] = 0;
			tW[0] = 0;
			bSetA = bSetW = false;
		}
		//
		NameAWT(const char* s)
		{
			setA(s);
		}
		NameAWT(const short_t* s)
		{
			setW(s);
		}
		inline	int		sizeW()
		{
			return u_strlen(strW());
		}
		inline	int		sizeA()
		{
			return strlen(strA());
		}
		VOID	setA(const char* s)
		{
			if(tA == s)
				return;
			if(s)
				strncpy(tA, s, MaxLengthA);
			else
				tA[0]			=	0;
			tA[MaxLengthA]	=	0;
			tW[0]			=	0;
			bSetA			=	true;
			bSetW			=	false;
		}
		VOID	setW(const short_t* s)
		{
			if(tW == s)
				return;
			if(s)
			{
				u_strncpy(tW, s, MaxLengthA);
			}
			else
				tW[0]			=	0;
			tW[MaxLengthA]	=	0;
			tA[0]			=	0;
			bSetA			=	false;
			bSetW			=	true;
		}
		inline	operator const short_t*(){return strW();}
		const char*		strA()
		{
			return tA;
		}
		const short_t*	strW()
		{
			return tW;
		}
		//
		inline	void	operator = (const char* s)
		{
			setA(s);
		}
		inline	void	operator = (const short_t* s)
		{
			setW(s);
		}
		inline	bool	operator == (const char* s) {return (stricmp(strA(), s) == 0);}
		inline	bool	operator != (const char* s) {return (stricmp(strA(), s) != 0);}
		inline	bool	operator == (const short_t* s) {return (u_stricmp(strW(), s) == 0);}
		inline	bool	operator != (const short_t* s) {return (u_stricmp(strW(), s) != 0);}
		//
		inline	void	operator += (const char* s)
		{
			int slen = strlen(tA);
			int nlen = strlen(s);
			if((slen + nlen) < MaxLengthA)
			{
				strcat(tA, s);
				bSetA	=	true;
				bSetW	=	false;
			}
		}
		//
		inline	void	operator += (const short_t* s)
		{
			int slen = u_strlen(tW);
			int nlen = u_strlen(s);
			if((slen + nlen) < MaxLengthW)
			{
				u_strcat(tW, s);
				bSetW	=	true;
				bSetA	=	false;
			}
		}
		inline	void	clear(){tA[0] = 0; tW[0] = 0;bSetA = false; bSetW = false;}
	protected:
		short_t							tW[MaxLengthW + 1];
		char							tA[MaxLengthA + 1];
		bool							bSetA;
		bool							bSetW;
	};

	typedef	NameAWT<60>	NameAW;
//
	class	TextAW
	{
	public:
		TextAW(){}
		~TextAW(){}
#ifndef WIN32_OS
		TextAW(const wchar_t* s)
		{
			int len = wcslen(s);
			CDynamicArray<short_t> v;
			v.SetArrayCount(len + 1);
			for(int i=0;i<=len;i++)
				v[i] = s[i];
			setW(v.GetPtr());
		}
#endif
		TextAW(const short_t* s)
		{
			setW(s);
		}
		TextAW(const char* s)
		{
			setA(s);
		}
		VOID			setW(const short_t* szText)
		{
			if(szText == textW.GetPtr())
				return;
			bSetA	=	false;
			bSetW	=	true;
			textW.SetArrayCount(0);
			if(szText)
				textW.push_back(szText, u_strlen(szText));
		}
		VOID			setA(const char* szText)
		{
			if(szText == textA.GetPtr())
				return;
			bSetW	=	false;
			bSetA	=	true;
			textA.SetArrayCount(0);
			if(szText)
				textA.push_back(szText, strlen(szText));
		}
		const short_t*	strW()
		{
			if(!bSetW)
			{
				if(textA.size() == 0)
					textW.SetArrayCount(0);
				else
				{
					textW.SetArrayCount(textA.size() + 1);
					int len = Utf8ToW(textA.GetPtr(), textW.GetPtr(), textW.size());
					textW.SetArrayCount(len);
				}
				bSetW	=	true;
			}
			if(textW.GetPtr())
				return textW.GetPtr();
			static short_t s[1];
			s[0] = 0;
			return s;
		}
		const char*		strA()
		{
			if(!bSetA)
			{
				if(textW.size() == 0)
					textA.SetArrayCount(0);
				else
				{
					textA.SetArrayCount(textW.size() * 2 + 1);
					int len	=	WToUtf8(textW.GetPtr(), textA.GetPtr(), textA.size());
					textA.SetArrayCount(len);
				}
				bSetA	=	true;
			}
			if(textA.GetPtr())
				return textA.GetPtr();
			return "";
		}
		inline	int		sizeW()
		{
			strW();
			return textW.size();
		}
		inline	int		sizeA()
		{
			strA();
			return textA.size();
		}
		inline	void	lower()
		{
			if(textW.size() && bSetW)
				u_strlwr(textW.GetPtr());
			if(textA.size() && bSetA)
				strlwr_t(textA.GetPtr());
		}
		inline	TextAW&	operator = (const short_t* s)
		{
			setW(s);
			return *this;
		}
#ifndef WIN32_OS
		inline	TextAW&	operator = (const wchar_t* s)
		{
			int len = wcslen(s);
			CDynamicArray<short_t> v;
			v.SetArrayCount(len + 1);
			for(int i=0;i<=len;i++)
				v[i] = s[i];
			setW(v.GetPtr());
			return *this;
		}
#endif
		inline	bool	operator == (const short_t* s)
		{
			return (u_stricmp(strW(), s) == 0);
		}
		inline	bool	operator != (const short_t* s)
		{
			return (u_stricmp(strW(), s) != 0);
		}
		//
		inline	TextAW&	operator = (const char* s)
		{
			setA(s);
			return *this;
		}
		inline	bool	operator == (const char* s)
		{
			return (stricmp(strA(), s) == 0);
		}
		inline	bool	operator != (const char* s)
		{
			return (stricmp(strA(), s) != 0);
		}
		//
		inline	TextAW&	operator += (const short_t* s)
		{
			strW();
			if(s)
			{
				bSetW	=	true;
				bSetA	=	false;
				textW.push_back(s, u_strlen(s), textW.size());
			}
			return *this;
		}
		inline	TextAW&	operator += (const char* s)
		{
			strA();
			if(s)
			{
				bSetW	=	false;
				bSetA	=	true;
				textA.push_back(s, strlen(s), textA.size());
			}
			return *this;
		}
		inline	operator const short_t*(){return strW();}
		inline	void	clear(){textW.clear();textA.clear();bSetA = bSetW = false;}
	protected:
		CDynamicArray<short_t>	textW;
		CDynamicArray<char>		textA;
		bool					bSetA;
		bool					bSetW;
	};


	struct	VersionWriter
	{
		CDynamicArray<char>			vars[MAX_UPDATE_VERSION];
		streamWrite					writes[MAX_UPDATE_VERSION];
		streamRead					reads[MAX_UPDATE_VERSION];
		streamWrite*	GetWriter(streamWrite& w, int version, int maxBuffer = 1024)
		{
			if(version < 0 || version >= MAX_UPDATE_VERSION)
				return 0;
			vars[version].SetArrayCount(maxBuffer);
			writes[version].m_length	=	maxBuffer;
			writes[version].m_point		=	0;
			writes[version].m_ptr		=	vars[version].GetPtr();
			return &writes[version];
		}
		streamRead*		GetReader(char version)
		{
			if(version < 0 || version >= MAX_UPDATE_VERSION)
				return 0;
			if(vars[version].size() <= 0)
				return 0;
			reads[version].m_length		=	vars[version].size();
			reads[version].m_point		=	0;
			reads[version].m_ptr		=	vars[version].GetPtr();
			return &reads[version];
		}
		//
		VOID		WriteAll(StreamBase* s)
		{
			unsigned short phantomV	=	PHANTOM_VERSION;
			char version = PHANTOM_SUBVERSION;
			s->write(&phantomV, sizeof(unsigned short));
			s->write(&version, sizeof(char));
			char	versionCount	=	0;
			//
			std::vector<int> versions;
			for(int i=0;i<MAX_UPDATE_VERSION;i++)
				if(vars[i].size() > 0)
					versions.push_back(i);
			versionCount	=	(int)versions.size();
			s->write(&versionCount, sizeof(unsigned char));
			for(int i=0;i<versionCount;i++)
			{
				unsigned char versionIndex	=	(unsigned char)i;
				unsigned short writeLen		=	writes[i].m_point;
				s->write(&versionIndex, sizeof(unsigned char));
				s->write(&writeLen, sizeof(unsigned short));
				s->write(&writeLen, sizeof(unsigned short));
				if(writeLen > 0)
					s->write(writes[i].m_ptr, writes[i].m_point);
			}
		}
		VOID		ReadAll(StreamBase* s)
		{
			bool bShow	=	false;
			unsigned short phantomV	=	PHANTOM_VERSION;
			char version = PHANTOM_SUBVERSION;
			s->read(&phantomV, sizeof(unsigned short));
			s->read(&version, sizeof(char));
			if(phantomV > PHANTOM_VERSION)
				bShow	=	true;
			else if(phantomV == PHANTOM_VERSION && version > PHANTOM_SUBVERSION)
				bShow	=	true;
			if(bShow)
			{
				//MessageBoxW(GetForegroundWindow(), L"您软件版本太低，某些数据会丢失，请到幻影官方网站http://www.aixspace.com下载最新的软件更新。", L"请及时更新", MB_OK);
			}
			//
			unsigned char versionCount;
			s->read(&versionCount, sizeof(unsigned char));
			for(int i=0;i<versionCount;i++)
			{
				unsigned short readLen	=	0;
				unsigned short readLen2	=	0;
				unsigned char versionIndex = 0;
				s->read(&versionIndex, sizeof(unsigned char));
				s->read(&readLen, sizeof(unsigned short));
				s->read(&readLen2, sizeof(unsigned short));
				assert(readLen == readLen2);
				if(readLen > 0)
				{
					vars[versionIndex].SetArrayCount(readLen);
					s->read(vars[versionIndex].GetPtr(), vars[versionIndex].size());
				}
			}
		}
	};
	class	skip_w{
	public:
		skip_w(StreamBase* s);
		~skip_w();
		char	end();
	protected:
		int			m_seek;
		int			m_filePointer;
		StreamBase* m_stream;
	};
	class	skip_r{
	public:
		skip_r();
		~skip_r();
		void	begin(StreamBase* s);
		char	end();
	protected:
		int			m_filePointer;
		StreamBase* m_stream;
	};
#pragma pack(push, 1)
	struct PhantomGUID {
		unsigned int   Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char  Data4[ 8 ];
	};
	struct	HttpGUID
	{
		HttpGUID();//初始化时会自动创建一个GUID
		~HttpGUID();
		unsigned char	Data1[6];	//year+2000, month,day,hour,minute,second
		unsigned int	Data2;	//micro second//微秒
		unsigned short	Data3;	//random,0,65535
		unsigned char	ucSystem;	//系统是哪一个
		unsigned char	ucRoleID;	//游戏的角色ID,最多可以创建255个角色
		enum{
			Flags_Android = 1,
			Flags_IPhone = 2,
		};
		//
		VOID			ToText(char* szText);//返回一串文本
		VOID			FromText(const char* szText);//从一串文本
		VOID			Generate();	//创建一个GUID
	};
    /*STRUCT*/struct	Time/*时间*/
    {
        int					year/*年*/;
        int					month/*月*/;
        int					day/*日*/;
        int					hour/*时*/;
        int					minute/*分*/;
        int					second/*秒*/;
        int					microsecond/*毫秒*/;
        /*END_STRUCT*/
    };
#pragma pack(pop)
	//运行效率评测功能类
	class	cputime{
	public:
		cputime(const char* className, const char* funcName, const char* file, int line);
		cputime(const char* className, const char* funcName, int index, const char* file, int line);
		void		begin();
		void		end();
		//
		void		addchild(cputime* c);
		static		cputime*			s_curr;
		static		unsigned char		s_channelId;
		static		CDynamicArray<cputime*>	s_tops;//最高级别的时钟
		enum ChannelID{
			cid_create	=	0,
			cid_framemove,
			cid_render,
			cid_input,
			cid_resize,
			cid_resetdev,
			cid_count,
		};
	public:
		double			m_timeBegin[cid_count];//开始计时
		double			m_timeFull[cid_count];//所有时间
		unsigned int	m_beginCount[cid_count];//总共调用次数
		double		m_timeMax;//峰值
		double		m_timeMin;//最低值
		Name		m_className,m_funcName;
		Name		m_fileName;
		int			m_line;
		int			m_pushCount;
		cputime*	m_parent;
		cputime*	m_push;
		CDynamicArray<cputime*>	m_childs;//子测试列表
	};
	struct	cputime_s{
		cputime_s(cputime&c){p=&c;p->begin();}
		~cputime_s(){p->end();}
		cputime* p;
	};

	class	UIControl;
	class	Callback
	{
	public:
		virtual	BOOL				OnOpenFile(char* pFile, int64 dwFileSize) = 0;	//可以在这里进行解密,算法根据用户自己的算法来制定
		virtual	UIControl*			OnCreateUIControl(INT nType) = 0;
	};
	extern	Callback*	GetCallbackPtr();

//global functions
	inline	BOOL			Failed(Result n){return (n < 0);}
	double					RequestIntervalTime();	//返回时钟周期

	VOID					HexEncode(std::vector<unsigned char>& ret, const char* szText);//16进制文本转换成整数
	int						HexDecode(char* ret, unsigned char* val, int length);//16进制整数转换成文本
	void					GetLocalTime(Time& t);
	void					GetTime(int& hour, int& minute, int& second);	//, int* microsecond = 微秒
	double					GetTickTime();	//秒时间
	VOID					RandomStart();	//随机数开始
	VOID					AppSleep(INT millisecond);
	inline	BOOL			isText(const char* t1, const char* t2){return (stricmp(t1, t2) == 0);}
	VOID					SetProjectDir(const char* szDir);	//设置工程目录
	const LFileName&		GetProjectDir();
	Base*			GetBasePtr(const PtrID& id);
	int						GetBaseContext(const PtrID& id);
	VOID					SetBaseContext(const PtrID& id, int n);
	VOID					SetPhantomGUID(unsigned int* chs);//设置幻影授权码，写入授权码之后很多操作才能正确运作，包括读取资源操作
	VOID					ReplaceAll(CDynamicArray<char>& ret, const char* szText, const char* szFinds, const char* szReplace);//替换所有文本
	VOID					Utf8ToAscii(CDynamicArray<char>& ret, const char* szIn);
	VOID					AsciiToUtf8(CDynamicArray<char>& ret, const char* szIn);
};

#ifdef ANDROID_OS
#define  LogAndroid(...)  __android_log_print(ANDROID_LOG_INFO, "PhantomGameEngine", __VA_ARGS__)
//#define  LogError(...)  __android_log_print(ANDROID_LOG_ERROR, "PhantomGameEngine", __VA_ARGS__)
#elif defined(IPHONE_OS)
extern  "C"{
    void    LogPrint(const char* szText, ...);
};
#endif
typedef VOID (*MyLogPrint)(const char* szText);//输出信息
void    myLogPrint(const char* szText, ...);
void	SetLogPrint(MyLogPrint m);
#define  LogInfo(...)	myLogPrint(__VA_ARGS__);
#define  LogError(...)  myLogPrint(__VA_ARGS__);

typedef VOID (*MyHttpLoadFile)(const char* szFile, BOOL bDownloadNow, BOOL bInMemory);//输出信息
VOID	SetMyLoadFile(MyHttpLoadFile f);
VOID	InitMyFile(const char* szFile, BOOL bDownloadNow, BOOL bInMemory);//初始化文件
#ifdef WIN32_OS
extern	const char* utf8(const char* szIn);
#else
#define	utf8(s)	s
#endif
#endif
