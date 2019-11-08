//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_MATH_H__
#define __PHANTOM_MATH_H__
#include "PhantomDefine.h"
#include "PhantomEnum.h"
#include "math.h"
#include "stdlib.h"

#define	PI_X2		6.283185308f
#define PI			3.141592654f		//圆周率
#define	PI_2X		1.570796327f		//1/2圆周率
#define PI_By1		0.318309886f		//1/圆周率
#define	MAX_UV_COUNT	2

namespace Phantom{
	inline	float	Radians(float fDegress){return fDegress * (PI / 180.0f);}//角度转弧度
	inline	float	Degress(float fRadian){return fRadian * (180.0f / PI);}//弧度转角度
	inline	float	To3DValue(float fMeter){return (fMeter * 10.0f);}//将米转换成3D坐标值
	inline	float	ToMeter(float f3DValue){return (f3DValue * (1.0f / 10.0f));}//将3D坐标值值转换成米
	inline	float	clamp01(float f){if(f < 0.0f) return 0.0f; if(f > 1.0f)return 1.0f; return f;}
	inline	BOOL	isClip(int n, int nMin, int nMax){return (n >= nMin && n <= nMax);}
	inline	float	clamp(float f, float fMin, float fMax)//将f限定值域范围
	{
		if(f < fMin)return fMin;
		else if(f > fMax)return fMax;
		return f;
	}

	struct	matrix4x4;
#pragma pack(push, 1)

	struct	SoundID
	{
		unsigned int			index/*保留1*/;
		unsigned int			rid/*保留2*/;
		int						fileIndex/*文件索引*/;
		/*END_STRUCT*/
		SoundID(){index=rid=fileIndex=0;}
		SoundID(unsigned int nid){
			rid = (nid&0x0000ffff);
			fileIndex = (nid&0xffff0000)>>16;
		}
		unsigned int ToInt(){
			return (unsigned int)rid | ((unsigned int)fileIndex<<16);
		}
	};
	
	/*STRUCT*/struct float2/*二维向量*/
	{
		float x;
		float y;
		/*END_STRUCT*/
		//
		float2()														{x=y=0;};
		float2( const float * p)										{x=p[0];y=p[1];}
		float2( float _x, float _y )									{x=_x;y=_y;}
		float2(const float2& v)										{x=v.x;y=v.y;}
		//返回浮点数足
		operator float* ()												{return (float*)&x;}
		//设置坐标值
		inline float2&				setxy(float px,float py)			{x=px;y=py;return *this;}
		//
		inline float2& operator	+=(const float2& v)				{x+=v.x;y+=v.y;return *this;}
		inline float2& operator	-=(const float2& v)				{x-=v.x;y-=v.y;return *this;}
		inline float2& operator	*=(const float2& v)				{x*=v.x;y*=v.y;return *this;}
		inline float2& operator	/=(const float2& v)				{x/=v.x;y/=v.y;return *this;}
		inline float2& operator	=(const float2& v)					{x=v.x;y=v.y;return *this;}
		//
		inline float2& operator	+=(float f)							{x+=f;y+=f;return *this;}
		inline float2& operator	-=(float f)							{x-=f;y-=f;return *this;}
		inline float2& operator	*=(float f)							{x*=f;y*=f;return *this;}
		inline float2& operator	/=(float f)							{x/=f;y/=f;return *this;}
		//
		inline float2& operator 	++()								{x+=1.0f;y+=1.0f;return *this;}
		inline float2& operator 	--()								{x-=1.0f;y-=1.0f;return *this;}
		//
		inline float2  operator 	+()									{return *this;}
		inline float2  operator 	-()									{return float2(-x,-y);}
		//
		inline float2  operator 	+(const float2& v)					{return float2(x+v.x,y+v.y);}
		inline float2  operator 	-(const float2& v)					{return float2(x-v.x,y-v.y);}
		inline float2  operator 	*(const float2& v)					{return float2(x*v.x,y*v.y);}
		inline float2  operator 	/(const float2& v)					{return float2(x/v.x,y/v.y);}
		//
		inline float2  operator 	+(float f)							{return float2(x+f,y+f);}
		inline float2  operator 	-(float f)							{return float2(x-f,y-f);}
		inline float2  operator 	*(float f)							{return float2(x*f,y*f);}
		inline float2  operator 	/(float f)							{float fInv = 1.0f / f;return float2(x*fInv,y*fInv);}
		//
		inline bool operator 		==(const float2& v)				{return (x==v.x&&y==v.y);}
		inline bool operator 		!=(const float2& v)				{return (x!=v.x||y!=v.y);}
		inline bool operator 		==(float f)							{return (x==f&&y==f);}
		//
		inline bool operator		!=(float f)							{return (x!=f||y!=f);}
		//返回矢量长度
		inline float 				length() const						{return sqrtf(x * x + y * y + 0.00001f);}
		inline float 				lengthSq() const					{return x * x + y * y;}
		//返回矢量点积
		inline float 				dot(const float2& v2) const				{return x * v2.x + y * v2.y;}
		inline float 				ccw(const float2& v2) const				{return x * v2.y - y * v2.x;}
		//返回矢量叉积
		inline float 				cross(const float2& v2) const				{return x * v2.y - y * v2.x;}
		//矢量置零
		inline void					identity()							{x = y = 0;}
		inline float2&				add(float2& _out,const float2& v2)		{_out.x = x + v2.x; _out.y = y + v2.y; return _out;}
		inline float2&				subtract(float2& _out,const float2& v2)	{_out.x = x - v2.x; _out.y = y - v2.y; return _out;}
		inline float2&				minimize(float2& _out,const float2& v2)	{_out.x = x<v2.x ? x:v2.x; _out.y = y<v2.y ? y:v2.y; return _out;}
		inline float2&				maximize(float2& _out,const float2& v2)	{_out.x = x>v2.x ? x:v2.x; _out.y = y>v2.y ? y:v2.y; return _out;}
		inline float2&				scale(float2& _out,float s)				{_out.x = x*s; _out.y = y*s; return _out;}
		inline float2&				lerp(float2& _out,const float2& v2,float s){_out.x = x + s*(v2.x - x); _out.y = y + s*(v2.y - y); return _out;}
		//矢量规格化
		inline void					normalize()							{float r = R();if(r>0.0001f){r = 1.0f/r; x*=r; y*=r;}}
		//
		inline float				R() const							{return sqrtf(x*x+y*y + 0.00001f);}
		inline float2				maxv(const float2& v)				{return float2(getmax(v.x,x),getmax(v.y,y));}
		inline float2				minv(const float2& v)				{return float2(getmin(v.x,x),getmin(v.y,y));}
		inline float2				maxf(float f)						{return float2(getmax(f,x),getmax(f,y));}
		inline float2				minf(float f)						{return float2(getmin(f,x),getmin(f,y));}
		inline float2				absv()								{return float2(fabsf(x),fabsf(y));}
		inline void					fill(float f)						{x=y=f;}
		//
	};


	template < typename DT>
	struct vec2
	{
		DT x,y;
		//
		vec2()														{x=y=0;};
		vec2( const DT * p)											{x=p[0];y=p[1];}
		vec2( DT _x, DT _y )										{x=_x;y=_y;}
		vec2(const vec2& v)											{x=v.x;y=v.y;}
		// casting
		operator DT* ()												{return (DT*)&x;}
		// assignment operators
		inline vec2&				setxy(DT px,DT py)				{x=px;y=py;return *this;}
		//
		inline vec2& operator	+=(const vec2& v)					{x+=v.x;y+=v.y;return *this;}
		inline vec2& operator	-=(const vec2& v)					{x-=v.x;y-=v.y;return *this;}
		inline vec2& operator	*=(const vec2& v)					{x*=v.x;y*=v.y;return *this;}
		inline vec2& operator	/=(const vec2& v)					{x/=v.x;y/=v.y;return *this;}
		inline vec2& operator	=(const vec2& v)					{x=v.x;y=v.y;return *this;}
		//
		inline vec2& operator	+=(DT f)							{x+=f;y+=f;return *this;}
		inline vec2& operator	-=(DT f)							{x-=f;y-=f;return *this;}
		inline vec2& operator	*=(DT f)							{x*=f;y*=f;return *this;}
		inline vec2& operator	/=(DT f)							{x/=f;y/=f;return *this;}
		//
		inline vec2& operator 	++()								{x+=1.0f;y+=1.0f;return *this;}
		inline vec2& operator 	--()								{x-=1.0f;y-=1.0f;return *this;}
		//
		inline vec2  operator 	+()									{return *this;}
		inline vec2  operator 	-()									{return vec2(-x,-y);}
		//
		inline vec2  operator 	+(const vec2& v)					{return vec2(x+v.x,y+v.y);}
		inline vec2  operator 	-(const vec2& v)					{return vec2(x-v.x,y-v.y);}
		inline vec2  operator 	*(const vec2& v)					{return vec2(x*v.x,y*v.y);}
		inline vec2  operator 	/(const vec2& v)					{return vec2(x/v.x,y/v.y);}
		//
		inline vec2  operator 	+(DT f)								{return vec2(x+f,y+f);}
		inline vec2  operator 	-(DT f)								{return vec2(x-f,y-f);}
		inline vec2  operator 	*(DT f)								{return vec2(x*f,y*f);}
		inline vec2  operator 	/(DT f)								{float fInv = 1.0f / f;return vec2(x*fInv,y*fInv);}
		//
		inline bool operator 	==(const vec2& v)					{return (x==v.x&&y==v.y);}
		inline bool operator 	!=(const vec2& v)					{return (x!=v.x||y!=v.y);}
		inline bool operator 	==(DT f)							{return (x==f&&y==f);}
		//
		inline bool operator	!=(DT f)							{return (x!=f||y!=f);}
		//
		inline DT 				length()							{return (DT)sqrtf((float)(x * x + y * y) + 0.00001f);}
		inline DT 				lengthSq()							{return x * x + y * y;}
		inline vec2&			minimize(vec2& _out,const vec2& v2)	{_out.x = x<v2.x ? x:v2.x; _out.y = y<v2.y ? y:v2.y; return _out;}
		inline vec2&			maximize(vec2& _out,const vec2& v2)	{_out.x = x>v2.x ? x:v2.x; _out.y = y>v2.y ? y:v2.y; return _out;}
		//
	};

	template < typename DT>
	struct vec3
	{
		DT x;
		DT y;
		DT z;
		//
		inline	vec3()												{x=y=z=0;};
		inline	vec3( const DT * p)								{x=p[0];y=p[1];z=p[2];}
		inline	vec3( const vec3& v)								{x=v.x;y=v.y;z=v.z;}
		inline	vec3( DT _x, DT _y, DT _z )					{x=_x;y=_y;z=_z;}
		//
		operator DT* ()												{return (DT*)&x;}
		//矢量规格化
		inline void					normalize()							{DT r = R();if(r>0.0001f){r = 1.0f/r; x*=r; y*=r; z*=r;}}
		//
		inline vec3&				setxyz(DT px,DT py,DT pz)	{x=px;y=py;z=pz;return *this;}
		//
		inline vec3& operator	+=(const vec3& v)						{x+=v.x;y+=v.y;z+=v.z;return *this;}
		inline vec3& operator	-=(const vec3& v)						{x-=v.x;y-=v.y;z-=v.z;return *this;}
		inline vec3& operator	*=(const vec3& v)						{x*=v.x;y*=v.y;z*=v.z;return *this;}
		inline vec3& operator	/=(const vec3& v)						{x/=v.x;y/=v.y;z/=v.z;return *this;}
		inline vec3& operator	=(const vec3& v)						{x=v.x;y=v.y;z=v.z;return *this;}
		//
		inline vec3& operator	+=(DT f)								{x+=f;y+=f;z+=f;return *this;}
		inline vec3& operator	-=(DT f)								{x-=f;y-=f;z-=f;return *this;}
		inline vec3& operator	*=(DT f)								{x*=f;y*=f;z*=f;return *this;}
		inline vec3& operator	/=(DT f)								{x/=f;y/=f;z/=f;return *this;}
		//
		inline vec3& operator	++()									{x+=1.0f;y+=1.0f;z+=1.0f;return *this;}
		inline vec3& operator	--()									{x-=1.0f;y-=1.0f;z-=1.0f;return *this;}
		inline float2& xy(){return *(float2*)this;}
		inline float2& yz(){return *(float2*)((DT*)this + 1);}
		//
		inline vec3  operator	+()										{return *this;}
		inline vec3  operator	-() const									{return vec3(-x,-y,-z);}
		//
		inline vec3  operator	+(const vec3& v) const					{return vec3(x+v.x,y+v.y,z+v.z);}
		inline vec3  operator	-(const vec3& v) const					{return vec3(x-v.x,y-v.y,z-v.z);}
		inline vec3  operator	*(const vec3& v) const					{return vec3(x*v.x,y*v.y,z*v.z);}
		inline vec3  operator	/(const vec3& v) const					{return vec3(x/v.x,y/v.y,z/v.z);}
		//
		inline vec3  operator	+(DT f) const							{return vec3(x+f,y+f,z+f);}
		inline vec3  operator	-(DT f) const							{return vec3(x-f,y-f,z-f);}
		inline vec3  operator	*(DT f) const							{return vec3(x*f,y*f,z*f);}
		inline vec3  operator	/(DT f) const							{DT fInv = 1.0f / f;return vec3(x*fInv,y*fInv,z*fInv);}
		//
		inline bool operator 		==(const vec3& v)				{return (x==v.x&&y==v.y&&z==v.z);}
		inline bool operator 		!=(const vec3& v)				{return (x!=v.x||y!=v.y||z!=v.z);}
		inline bool operator 		==(DT f)							{return (x==f&&y==f&&z==f);}
		//
		inline bool operator 		!=(DT f)							{return (x!=f||y!=f||z!=f);}
		//
		inline DT				sum() const							{return x+y+z;}//x+y+z;
		inline void					chs()								{x=-x;y=-y;z=-z;}
		inline DT				R() const							{return sqrtf(x*x+y*y+z*z + 0.00001f);}
		inline DT				R(vec3& v2)						{vec3 v =(*this)*v2; return acosf(v.sum()/(R()*v2.R()));}
		inline DT				Rxz() const							{return sqrtf(x*x+z*z + 0.00001f);}
		inline DT				Rxy() const							{return sqrtf(x*x+y*y + 0.00001f);}
		inline DT				Ryz() const							{return sqrtf(y*y+z*z + 0.00001f);}
		inline DT				radius() const						{return sqrtf(x*x+y*y+z*z + 0.00001f);}
		inline vec3				maxv(const vec3& v)				{return vec3(getmax(v.x,x),getmax(v.y,y),getmax(v.z,z));}
		inline vec3				minv(const vec3& v)				{return vec3(getmin(v.x,x),getmin(v.y,y),getmin(v.z,z));}
		inline vec3				maxf(DT f)						{return vec3(getmax(f,x),getmax(f,y),getmax(f,z));}
		inline vec3				minf(DT f)						{return vec3(getmin(f,x),getmin(f,y),getmin(f,z));}
		inline vec3				absv()								{return vec3(fabsf(x),fabsf(y),fabsf(z));}
		inline vec3				cross(const vec3& v2)	 const		{return vec3(y*v2.z-v2.y*z,z*v2.x-v2.z*x,x*v2.y-v2.x*y);}
		//
		//inline void zero(){x=y=z=0.0f;};
		inline bool 				iszero(DT f=0.0f) const			{return (x==f&&y==f&&z==f);}//是否全为0.0f;
		inline bool 				isndef() const						{return (x==y&&x==z);}//是否x==y==z;
		//x+y+z 写入汇编语言时不能在一行内有两段代码.
		//dest 可以是this指针.
		inline	DT				cosfv(const	vec3& b) const					{return (x*b.x+y*b.y+z*b.z)/(R()*b.R());}
		inline	DT				dotproduct(const	vec3& v2) const				{return x*v2.x+y*v2.y+z*v2.z;}
		inline vec3& 				addv(vec3& dest,const	vec3& b) const 		{ dest.x=x+b.x; dest.y=y+b.y; dest.z=z+b.z; return dest;}//DEST=THIS+B;//矢量 //41ms
		inline vec3& 				subv(vec3& dest,const	vec3& b) const 		{ dest.x=x-b.x; dest.y=y-b.y; dest.z=z-b.z; return dest;}//DEST=THIS-B; //50ms
		inline vec3& 				mulv(vec3& dest,const	vec3& b) const 		{ dest.x=x*b.x; dest.y=y*b.y; dest.z=z*b.z; return dest;}//DEST=THIS*B; //20ms
		inline vec3& 				divv(vec3& dest,const	vec3& b) const 		{ dest.x=x/b.x; dest.y=y/b.y; dest.z=z/b.z; return dest;}//DEST=B/THIS; //120ms
		inline vec3& 				minv(vec3& dest,const	vec3& b) const 		{ dest.x=getmin(x,b.x); dest.y=getmin(y,b.y); dest.z=getmin(z,b.z); return dest;}//DEST=MIN(THIS,B) //30ms
		inline vec3& 				maxv(vec3& dest,const	vec3& b) const 		{ dest.x=getmax(x,b.x); dest.y=getmax(y,b.y); dest.z=getmax(z,b.z); return dest;}//DEST=MAX(THIS,B){} //20ms
		inline vec3& 				cosvv(vec3& dest,const	vec3& b) const		{ DT r=R()*b.R(); dest.x=(b.x*x/r); dest.y=(b.y*y/r); dest.z=(b.z*z/r); return dest;}//DEST=cos(THIS^B){} //330ms
		inline vec3& 				crossv(vec3& dest,const	vec3& b) const	{ DT f1,f2,f3; f1=y*b.z-b.y*z; f2=z*b.x-b.z*x; f3=x*b.y-b.x*y; dest.x=f1; dest.y=f2; dest.z=f3; return dest;}//DEST=THIS x B; //40ms // //
		inline vec3& 				normalv(vec3& dest) const				{DT fr=sqrtf(x*x+y*y+z*z + 0.00001f);dest.x=x/fr;dest.y=y/fr;dest.z=z/fr;return dest;}
		inline vec3& 				sqrtv(vec3& dest) const				{ if(x>=0.0f) dest.x=sqrtf(x + 0.00001f); else dest.x=-sqrtf(-x + 0.00001f); if(y>=0.0f) dest.y=sqrtf(y + 0.00001f); else dest.y=-sqrtf(-y + 0.00001f); if(z>=0.0f) dest.z=sqrtf(z + 0.00001f); else dest.z=-sqrtf(-z + 0.00001f); return dest;}//DEST=SQRT(THIS){} //240ms
		inline vec3& 				cosv(vec3& dest) const					{ DT r=R(); dest.x=x/r; dest.y=y/r; dest.z=z/r; return dest;}//DEST=cos(THIS){}//dest.x=x/R(),y=... //231ms
		inline vec3& 				subf(vec3& dest,DT f) 		{ dest.x=x-f; dest.y=y-f; dest.z=z-f; return dest;}//DEST=THIS-f;//dest.x=this.x-f;//dest.y=this.y-f;... //21ms
		inline vec3& 				addf(vec3& dest,DT f) 		{ dest.x=x+f; dest.y=y+f; dest.z=z+f; return dest;}//DEST=THIS+f; //10ms
		inline vec3& 				mulf(vec3& dest,DT f) 		{ dest.x=x*f; dest.y=y*f; dest.z=z*f; return dest;}//DEST=THIS*f; //0ms 可以在1000*1000次中偶尔为0
		inline vec3& 				divf(vec3& dest,DT f) 		{ dest.x=x/f; dest.y=y/f; dest.z=z/f; return dest;}//DEST=f/THIS; //30ms
		inline vec3& 				minf(vec3& dest,DT f) 		{ dest.x=getmin(x,f); dest.y=getmin(y,f); dest.z=getmin(z,f); return dest;}//DEST=Min(THIS,f){}//dest.x=getmin(this.x,f){}//../20ms
		inline vec3& 				maxf(vec3& dest,DT f) 		{ dest.x=getmax(x,f); dest.y=getmax(y,f); dest.z=getmax(z,f); return dest;}//DEST=Max(THIS,f){}// //20ms
		//矢量Boolean运算时，3个都一致时返回TRUE，如(x1==x2&&y1==y2&&z1==z2)或(x1<x2&&y1<y2&&z1<z2)
		inline bool 				isequalv(vec3& b)				{ return (x==b.x&&y==b.y&&z==b.z);}//(ax==bx&&ay==by&&az==bz) //0ms
		inline bool 				isminv(vec3& b) 					{ if(x>=b.x)return false; if(y>=b.y)return false; if(z>=b.z)return false; return true; }//ax<bx&&ay<by&&az<bz //20ms
		inline bool 				ismaxv(vec3& b) 					{ if(x<=b.x)return false; if(y<=b.y)return false; if(z<=b.z)return false; return true; }//ax>bx&&ay>by&&az>bz //10ms
		inline bool 				ismineqv(vec3& b) 				{ if(x>b.x) return false; if(y>b.y) return false; if(z>b.z) return false; return true; }//ax<=bx&&ay<=by&&az<=bz; //21ms
		inline bool 				ismaxeqv(vec3& b) 				{ if(x<b.x) return false; if(y<b.y) return false; if(z<b.z) return false; return true; }//ax>=bx&&ay>=by&&az>=bz; //10ms
		//(ax>bmin.x&&ay>bmin.y&&az>bmin.z)&&(ax<bmax.x&&ay<bmax.y&&az<bmax.z)
		inline bool					isinbox(vec3& bmin,vec3& bmax) { return ((!isminv(bmin))&&(!ismaxv(bmax))); }
		inline vec3&				movv(vec3& dest)					{dest.x=x;dest.y=y;dest.z=z;return dest;}//DEST=THIS;
		//x=f,y=f,z=f; {
		inline void					fill(DT f)						{x=y=z=f;}
		//矢量置零
		inline void					identity()							{x = y = z = 0;}
		//根据矩阵换算，受矩阵位置值影响
		inline vec3& 				transform(vec3& dest,const matrix4x4& _m) const ;
		//根据矩阵换算三维矢量，不受矩阵位置和矢量位置影响
		inline vec3& 				transformNoMove(vec3& dest,const matrix4x4& _m) const ;
		//
		inline vec3&				distancev(vec3& destLine,DT fdistance)		{	DT rd=sqrtf(x*x+y*y+z*z + 0.00001f);	destLine.x=(x/rd)*fdistance;	destLine.y=(y/rd)*fdistance;	destLine.z=(z/rd)*fdistance;	return destLine;	}
		//坐标围绕X轴转动
		inline vec3& 				RotateX(vec3& dest,DT fRads)					{	DT fy=y,fz=z;	DT cf=cosf(fRads);	DT sf=sinf(fRads);	dest.y=fy*cf+fz*sf;	dest.z=-fy*sf+fz*cf;	dest.x=x;	return dest;	}
		//坐标围绕Y轴转动
		inline vec3&				RotateY(vec3& dest,DT fRads)					{	DT fx=x,fz=z;	DT cf=cosf(fRads),sf=sinf(fRads);	dest.x=cf*fx-sf*fz;	dest.z=sf*fx+cf*fz;	dest.y=y;	return dest;	}
		//坐标围绕Z轴转动
		inline vec3&				RotateZ(vec3& dest,DT fRads)					{	DT fx=x,fy=y;	DT cf=cosf(fRads),sf=sinf(fRads);	dest.x=cf*fx+sf*fy;	dest.y=-sf*fx+cf*fy;	dest.z=z;	return dest;	}
		//坐标围绕所有轴转动
		inline vec3&				Rotate(vec3& dest,vec3& vRadius)				{	RotateX(dest,vRadius.x);	dest.RotateY(dest,vRadius.y);	dest.RotateZ(dest,vRadius.z);	return dest;	}
		//返回矢量长度
		inline DT				length() const										{DT f = x*x+y*y+z*z; if(f < 0.00001f)return 0; return sqrtf(f + 0.00001f);}
		inline DT				lengthSq() const									{return x*x + y*y + z*z;}
		//返回点积
		inline DT				dot(const vec3& v2) const						{return x*v2.x + y*v2.y + z*v2.z;}
		//返回叉积
		inline vec3&				cross(vec3& _out,const vec3& v2)				{vec3 v;	v.x = y * v2.z - z * v2.y;	v.y = z * v2.x - x * v2.z;	v.z = x * v2.y - y * v2.x;	_out = v;	return _out;}
		//
		inline vec3&				add(vec3& _out,const vec3& v2)				{_out.x  = x + v2.x; _out.y = y + v2.y; _out.z = z + v2.z; return _out;}
		inline vec3&				subtract(vec3& _out,const vec3& v2)			{_out.x = x - v2.x; _out.y = y - v2.y; _out.z = z - v2.z; return _out;}
		inline vec3&				minimize(vec3& _out,const vec3& v2)			{_out.x = x<v2.x ? x:v2.x; _out.y = y<v2.y ? y:v2.y; _out.z = z<v2.z ? z:v2.z; return _out;}
		inline vec3&				maximize(vec3& _out,const vec3& v2)			{_out.x = x>v2.x ? x:v2.x; _out.y = y>v2.y ? y:v2.y; _out.z = z>v2.z ? z:v2.z; return _out;}
		inline vec3&				scale(vec3& _out,DT s)						{_out.x = x*s; _out.y = y*s; _out.z = z*s; return _out;}
		//两个坐标之间插值计算，s=0~1
		inline vec3&				lerp(vec3& _out,const vec3& v2,DT s) const	{ _out.x = x + s*(v2.x - x); _out.y = y + s*(v2.y - y); _out.z = z + s*(v2.z - z); return _out;}
	};
	typedef	vec2<unsigned short>	v_ui2;
	typedef	vec2<short>				v_i2;
	typedef	vec2<unsigned int>		v_ui4;
	typedef	vec2<int>				v_i4;
	typedef	vec2<int>				int2;
	typedef	vec3<int>				int3;

	//----------------------------------------------------------------------------------------------------------------
	//
	//float3
	//
	//----------------------------------------------------------------------------------------------------------------

	//三维坐标数据
	/*STRUCT*/struct float3/*三维向量*/
	{
		float x;
		float y;
		float z;
		/*END_STRUCT*/
		//
		inline	float3()												{x=y=z=0;};
		inline	float3( const float * p)								{x=p[0];y=p[1];z=p[2];}
		inline	float3( const float3& v)								{x=v.x;y=v.y;z=v.z;}
		inline	float3( float _x, float _y, float _z )					{x=_x;y=_y;z=_z;}
		//
		operator float* ()												{return (float*)&x;}
		//矢量规格化
		inline void					normalize()							{float r = R();if(r>0.0001f){r = 1.0f/r; x*=r; y*=r; z*=r;}}
		//
		inline float3&				setxyz(float px,float py,float pz)	{x=px;y=py;z=pz;return *this;}
		//
		inline float3& operator	+=(const float3& v)						{x+=v.x;y+=v.y;z+=v.z;return *this;}
		inline float3& operator	-=(const float3& v)						{x-=v.x;y-=v.y;z-=v.z;return *this;}
		inline float3& operator	*=(const float3& v)						{x*=v.x;y*=v.y;z*=v.z;return *this;}
		inline float3& operator	/=(const float3& v)						{x/=v.x;y/=v.y;z/=v.z;return *this;}
		inline float3& operator	=(const float3& v)						{x=v.x;y=v.y;z=v.z;return *this;}
		//
		inline float3& operator	+=(float f)								{x+=f;y+=f;z+=f;return *this;}
		inline float3& operator	-=(float f)								{x-=f;y-=f;z-=f;return *this;}
		inline float3& operator	*=(float f)								{x*=f;y*=f;z*=f;return *this;}
		inline float3& operator	/=(float f)								{x/=f;y/=f;z/=f;return *this;}
		//
		inline float3& operator	++()									{x+=1.0f;y+=1.0f;z+=1.0f;return *this;}
		inline float3& operator	--()									{x-=1.0f;y-=1.0f;z-=1.0f;return *this;}
		inline float2& xy(){return *(float2*)this;}
		inline float2& yz(){return *(float2*)((float*)this + 1);}
		//
		inline float3  operator	+()										{return *this;}
		inline float3  operator	-() const									{return float3(-x,-y,-z);}
		//
		inline float3  operator	+(const float3& v) const					{return float3(x+v.x,y+v.y,z+v.z);}
		inline float3  operator	-(const float3& v) const					{return float3(x-v.x,y-v.y,z-v.z);}
		inline float3  operator	*(const float3& v) const					{return float3(x*v.x,y*v.y,z*v.z);}
		inline float3  operator	/(const float3& v) const					{return float3(x/v.x,y/v.y,z/v.z);}
		//
		inline float3  operator	+(float f) const							{return float3(x+f,y+f,z+f);}
		inline float3  operator	-(float f) const							{return float3(x-f,y-f,z-f);}
		inline float3  operator	*(float f) const							{return float3(x*f,y*f,z*f);}
		inline float3  operator	/(float f) const							{float fInv = 1.0f / f;return float3(x*fInv,y*fInv,z*fInv);}
		//
		inline bool operator 		==(const float3& v)				{return (x==v.x&&y==v.y&&z==v.z);}
		inline bool operator 		!=(const float3& v)				{return (x!=v.x||y!=v.y||z!=v.z);}
		inline bool operator 		==(float f)							{return (x==f&&y==f&&z==f);}
		//
		inline bool operator 		!=(float f)							{return (x!=f||y!=f||z!=f);}
		//
		inline float				sum() const							{return x+y+z;}//x+y+z;
		inline void					chs()								{x=-x;y=-y;z=-z;}
		inline float				R() const							{return sqrtf(x*x+y*y+z*z + 0.00001f);}
		inline float				R(float3& v2)						{float3 v =(*this)*v2; return acosf(v.sum()/(R()*v2.R()));}
		inline float				Rxz() const							{return sqrtf(x*x+z*z + 0.00001f);}
		inline float				Rxy() const							{return sqrtf(x*x+y*y + 0.00001f);}
		inline float				Ryz() const							{return sqrtf(y*y+z*z + 0.00001f);}
		inline float				radius() const						{return sqrtf(x*x+y*y+z*z + 0.00001f);}
		inline float3				maxv(const float3& v)				{return float3(getmax(v.x,x),getmax(v.y,y),getmax(v.z,z));}
		inline float3				minv(const float3& v)				{return float3(getmin(v.x,x),getmin(v.y,y),getmin(v.z,z));}
		inline float3				maxf(float f)						{return float3(getmax(f,x),getmax(f,y),getmax(f,z));}
		inline float3				minf(float f)						{return float3(getmin(f,x),getmin(f,y),getmin(f,z));}
		inline float3				absv()								{return float3(fabsf(x),fabsf(y),fabsf(z));}
		inline float3				cross(const float3& v2)	 const		{return float3(y*v2.z-v2.y*z,z*v2.x-v2.z*x,x*v2.y-v2.x*y);}
		//
		//inline void zero(){x=y=z=0.0f;};
		inline bool 				iszero(float f=0.0f) const			{return (x==f&&y==f&&z==f);}//是否全为0.0f;
		inline bool 				isndef() const						{return (x==y&&x==z);}//是否x==y==z;
		//x+y+z 写入汇编语言时不能在一行内有两段代码.
		//dest 可以是this指针.
		inline	float				cosfv(const	float3& b) const					{return (x*b.x+y*b.y+z*b.z)/(R()*b.R());}
		inline	float				dotproduct(const	float3& v2) const				{return x*v2.x+y*v2.y+z*v2.z;}
		inline float3& 				addv(float3& dest,const	float3& b) const 		{ dest.x=x+b.x; dest.y=y+b.y; dest.z=z+b.z; return dest;}//DEST=THIS+B;//矢量 //41ms
		inline float3& 				subv(float3& dest,const	float3& b) const 		{ dest.x=x-b.x; dest.y=y-b.y; dest.z=z-b.z; return dest;}//DEST=THIS-B; //50ms
		inline float3& 				mulv(float3& dest,const	float3& b) const 		{ dest.x=x*b.x; dest.y=y*b.y; dest.z=z*b.z; return dest;}//DEST=THIS*B; //20ms
		inline float3& 				divv(float3& dest,const	float3& b) const 		{ dest.x=x/b.x; dest.y=y/b.y; dest.z=z/b.z; return dest;}//DEST=B/THIS; //120ms
		inline float3& 				minv(float3& dest,const	float3& b) const 		{ dest.x=getmin(x,b.x); dest.y=getmin(y,b.y); dest.z=getmin(z,b.z); return dest;}//DEST=MIN(THIS,B) //30ms
		inline float3& 				maxv(float3& dest,const	float3& b) const 		{ dest.x=getmax(x,b.x); dest.y=getmax(y,b.y); dest.z=getmax(z,b.z); return dest;}//DEST=MAX(THIS,B){} //20ms
		inline float3& 				cosvv(float3& dest,const	float3& b) const		{ float r=R()*b.R(); dest.x=(b.x*x/r); dest.y=(b.y*y/r); dest.z=(b.z*z/r); return dest;}//DEST=cos(THIS^B){} //330ms
		inline float3& 				crossv(float3& dest,const	float3& b) const	{ float f1,f2,f3; f1=y*b.z-b.y*z; f2=z*b.x-b.z*x; f3=x*b.y-b.x*y; dest.x=f1; dest.y=f2; dest.z=f3; return dest;}//DEST=THIS x B; //40ms // //
		inline float3& 				normalv(float3& dest) const				{float fr=sqrtf(x*x+y*y+z*z + 0.00001f);dest.x=x/fr;dest.y=y/fr;dest.z=z/fr;return dest;}
		inline float3& 				sqrtv(float3& dest) const				{ if(x>=0.0f) dest.x=sqrtf(x + 0.00001f); else dest.x=-sqrtf(-x + 0.00001f); if(y>=0.0f) dest.y=sqrtf(y + 0.00001f); else dest.y=-sqrtf(-y + 0.00001f); if(z>=0.0f) dest.z=sqrtf(z + 0.00001f); else dest.z=-sqrtf(-z + 0.00001f); return dest;}//DEST=SQRT(THIS){} //240ms
		inline float3& 				cosv(float3& dest) const					{ float r=R(); dest.x=x/r; dest.y=y/r; dest.z=z/r; return dest;}//DEST=cos(THIS){}//dest.x=x/R(),y=... //231ms
		inline float3& 				subf(float3& dest,float f) 		{ dest.x=x-f; dest.y=y-f; dest.z=z-f; return dest;}//DEST=THIS-f;//dest.x=this.x-f;//dest.y=this.y-f;... //21ms
		inline float3& 				addf(float3& dest,float f) 		{ dest.x=x+f; dest.y=y+f; dest.z=z+f; return dest;}//DEST=THIS+f; //10ms
		inline float3& 				mulf(float3& dest,float f) 		{ dest.x=x*f; dest.y=y*f; dest.z=z*f; return dest;}//DEST=THIS*f; //0ms 可以在1000*1000次中偶尔为0
		inline float3& 				divf(float3& dest,float f) 		{ dest.x=x/f; dest.y=y/f; dest.z=z/f; return dest;}//DEST=f/THIS; //30ms
		inline float3& 				minf(float3& dest,float f) 		{ dest.x=getmin(x,f); dest.y=getmin(y,f); dest.z=getmin(z,f); return dest;}//DEST=Min(THIS,f){}//dest.x=getmin(this.x,f){}//../20ms
		inline float3& 				maxf(float3& dest,float f) 		{ dest.x=getmax(x,f); dest.y=getmax(y,f); dest.z=getmax(z,f); return dest;}//DEST=Max(THIS,f){}// //20ms
		//矢量Boolean运算时，3个都一致时返回TRUE，如(x1==x2&&y1==y2&&z1==z2)或(x1<x2&&y1<y2&&z1<z2)
		inline bool 				isequalv(float3& b)				{ return (x==b.x&&y==b.y&&z==b.z);}//(ax==bx&&ay==by&&az==bz) //0ms
		inline bool 				isminv(float3& b) 					{ if(x>=b.x)return false; if(y>=b.y)return false; if(z>=b.z)return false; return true; }//ax<bx&&ay<by&&az<bz //20ms
		inline bool 				ismaxv(float3& b) 					{ if(x<=b.x)return false; if(y<=b.y)return false; if(z<=b.z)return false; return true; }//ax>bx&&ay>by&&az>bz //10ms
		inline bool 				ismineqv(float3& b) 				{ if(x>b.x) return false; if(y>b.y) return false; if(z>b.z) return false; return true; }//ax<=bx&&ay<=by&&az<=bz; //21ms
		inline bool 				ismaxeqv(float3& b) 				{ if(x<b.x) return false; if(y<b.y) return false; if(z<b.z) return false; return true; }//ax>=bx&&ay>=by&&az>=bz; //10ms
		//(ax>bmin.x&&ay>bmin.y&&az>bmin.z)&&(ax<bmax.x&&ay<bmax.y&&az<bmax.z)
		inline bool					isinbox(float3& bmin,float3& bmax) { return ((!isminv(bmin))&&(!ismaxv(bmax))); }
		inline float3&				movv(float3& dest)					{dest.x=x;dest.y=y;dest.z=z;return dest;}//DEST=THIS;
		//x=f,y=f,z=f; {
		inline void					fill(float f)						{x=y=z=f;}
		//矢量置零
		inline void					identity()							{x = y = z = 0;}
		//根据矩阵换算，受矩阵位置值影响
		inline float3& 				transform(float3& dest,const matrix4x4& _m) const ;
		//根据矩阵换算三维矢量，不受矩阵位置和矢量位置影响
		inline float3& 				transformNoMove(float3& dest,const matrix4x4& _m) const ;
		//
		inline float3&				distancev(float3& destLine,float fdistance)		{	float rd=sqrtf(x*x+y*y+z*z + 0.00001f);	destLine.x=(x/rd)*fdistance;	destLine.y=(y/rd)*fdistance;	destLine.z=(z/rd)*fdistance;	return destLine;	}
		//坐标围绕X轴转动
		inline float3& 				RotateX(float3& dest,float fRads)					{	float fy=y,fz=z;	float cf=cosf(fRads);	float sf=sinf(fRads);	dest.y=fy*cf+fz*sf;	dest.z=-fy*sf+fz*cf;	dest.x=x;	return dest;	}
		//坐标围绕Y轴转动
		inline float3&				RotateY(float3& dest,float fRads)					{	float fx=x,fz=z;	float cf=cosf(fRads),sf=sinf(fRads);	dest.x=cf*fx-sf*fz;	dest.z=sf*fx+cf*fz;	dest.y=y;	return dest;	}
		//坐标围绕Z轴转动
		inline float3&				RotateZ(float3& dest,float fRads)					{	float fx=x,fy=y;	float cf=cosf(fRads),sf=sinf(fRads);	dest.x=cf*fx+sf*fy;	dest.y=-sf*fx+cf*fy;	dest.z=z;	return dest;	}
		//坐标围绕所有轴转动
		inline float3&				Rotate(float3& dest,float3& vRadius)				{	RotateX(dest,vRadius.x);	dest.RotateY(dest,vRadius.y);	dest.RotateZ(dest,vRadius.z);	return dest;	}
		//返回矢量长度
		inline float				length() const										{float f = x*x+y*y+z*z; if(f < 0.00001f)return 0; return sqrtf(f + 0.00001f);}
		inline float				lengthSq() const									{return x*x + y*y + z*z;}
		//返回点积
		inline float				dot(const float3& v2) const						{return x*v2.x + y*v2.y + z*v2.z;}
		//返回叉积
		inline float3&				cross(float3& _out,const float3& v2)				{float3 v;	v.x = y * v2.z - z * v2.y;	v.y = z * v2.x - x * v2.z;	v.z = x * v2.y - y * v2.x;	_out = v;	return _out;}
		//
		inline float3&				add(float3& _out,const float3& v2)				{_out.x  = x + v2.x; _out.y = y + v2.y; _out.z = z + v2.z; return _out;}
		inline float3&				subtract(float3& _out,const float3& v2)			{_out.x = x - v2.x; _out.y = y - v2.y; _out.z = z - v2.z; return _out;}
		inline float3&				minimize(float3& _out,const float3& v2)			{_out.x = x<v2.x ? x:v2.x; _out.y = y<v2.y ? y:v2.y; _out.z = z<v2.z ? z:v2.z; return _out;}
		inline float3&				maximize(float3& _out,const float3& v2)			{_out.x = x>v2.x ? x:v2.x; _out.y = y>v2.y ? y:v2.y; _out.z = z>v2.z ? z:v2.z; return _out;}
		inline float3&				scale(float3& _out,float s)						{_out.x = x*s; _out.y = y*s; _out.z = z*s; return _out;}
		//两个坐标之间插值计算，s=0~1
		inline float3&				lerp(float3& _out,const float3& v2,float s) const	{ _out.x = x + s*(v2.x - x); _out.y = y + s*(v2.y - y); _out.z = z + s*(v2.z - z); return _out;}
	};
	//四维坐标数据
	/*STRUCT*/struct float4/*四维向量*/
	{
		float x;
		float y;
		float z;
		float w;
		/*END_STRUCT*/
		//
		//float x,y,z,w;
		inline	float4()												{x=y=z=w=0;};
		inline	float4( const float* p)								{x=p[0];y=p[1];z=p[2];w=p[3];}
		inline	float4( const float3& xyz, float _w )					{x=xyz.x;y=xyz.y;z=xyz.z;w=_w;}
		inline	float4( float _x, float _y, float _z, float _w )		{x=_x;y=_y;z=_z;w=_w;}
		// casting
		inline	operator float* ()										{return (float *) &x;}
		// assignment operators
		inline float4& setXYZW(float px,float py,float pz,float pw)	{x=px;y=py;z=pz;w=pw;return *this;}
		//
		inline float2& xy(){return *(float2*)this;}
		inline float2& yz(){return *(float2*)((float*)this + 1);}
		inline float2& zw(){return *(float2*)((float*)this + 2);}
		inline float3& xyz(){return *(float3*)this;}
		//
		inline float4& operator	+=(const float4& v)				{x+=v.x;y+=v.y;z+=v.z;w+=v.w;return *this;}
		inline float4& operator	-=(const float4& v)				{x-=v.x;y-=v.y;z-=v.z;w-=v.w;return *this;}
		inline float4& operator	*=(const float4& v)				{x*=v.x;y*=v.y;z*=v.z;w*=v.w;return *this;}
		inline float4& operator	/=(const float4& v)				{x/=v.x;y/=v.y;z/=v.z;w/=v.w;return *this;}
		inline float4& operator	=(const float4& v)					{x=v.x;y=v.y;z=v.z;w=v.w;return *this;}
		//
		inline float4& operator	+=(float f)							{x+=f;y+=f;z+=f;w+=f;return *this;}
		inline float4& operator	-=(float f)							{x-=f;y-=f;z-=f;w-=f;return *this;}
		inline float4& operator	*=(float f)							{x*=f;y*=f;z*=f;w*=f;return *this;}
		inline float4& operator	/=(float f)							{x/=f;y/=f;z/=f;w/=f;return *this;}
		//
		inline float4& operator ++()									{x+=1.0f;y+=1.0f;z+=1.0f;w+=1.0f;return *this;}
		inline float4& operator --()									{x-=1.0f;y-=1.0f;z-=1.0f;w-=1.0f;return *this;}
		//
		inline float4  operator +()									{return *this;}
		inline float4  operator -()									{return float4(-x,-y,-z,-w);}
		//
		inline float4  operator +(const float4& v)					{return float4(x+v.x,y+v.y,z+v.z,w+v.w);}
		inline float4  operator -(const float4& v)					{return float4(x-v.x,y-v.y,z-v.z,w-v.w);}
		inline float4  operator *(const float4& v)					{return float4(x*v.x,y*v.y,z*v.z,w*v.w);}
		inline float4  operator /(const float4& v)					{return float4(x/v.x,y/v.y,z/v.z,w/v.w);}
		//
		inline float4  operator +(float f)								{return float4(x+f,y+f,z+f,w+f);}
		inline float4  operator -(float f)								{return float4(x-f,y-f,z-f,w-f);}
		inline float4  operator *(float f)								{return float4(x*f,y*f,z*f,w*f);}
		inline float4  operator /(float f)								{float fInv = 1.0f / f;return float4(x*fInv,y*fInv,z*fInv,w*fInv);}
		//
		inline bool 	operator ==(const float4& v)					{return (x==v.x&&y==v.y&&z==v.z&&w==v.w);}
		inline bool 	operator !=(const float4& v)					{return (x!=v.x||y!=v.y||z!=v.z||w!=v.w);}
		inline bool 	operator ==(float f)							{return (x==f&&y==f&&z==f&&w==f);}
		inline bool 	operator !=(float f)							{return (x!=f||y!=f||z!=f||w!=f);}
		//
		inline void					identity()							{x = y = z = w = 0;}
		inline float				length() const						{return sqrtf(x*x + y*y + z*z + w*w + 0.00001f);}
		inline float				lengthSq() const					{return x*x + y*y + z*z + w*w;}
		inline float				dot(const float4& v2)				{return x*v2.x + y* v2.y + z*v2.z + w*v2.w;}
		inline float4&				add(float4& _out,const float4& v2)		{_out.x = x+v2.x; _out.y = y+v2.y; _out.z = z+v2.z; _out.w = w+v2.w; return _out;}
		inline float4&				subtract(float4& _out,const float4& v2)	{_out.x = x - v2.x; _out.y = y-v2.y; _out.z = z-v2.z; _out.w = w-v2.w; return _out;}
		inline float4&				minimize(float4& _out,const float4& v2)	{_out.x = x<v2.x?x:v2.x; _out.y = y<v2.y?y:v2.y; _out.z = z<v2.z?z:v2.z; _out.w = w<v2.w?w:v2.w; return _out;}
		inline float4&				maximize(float4& _out,const float4& v2)	{_out.x = x>v2.x?x:v2.x; _out.y = y>v2.y?y:v2.y; _out.z = z>v2.z?z:v2.z; _out.w = w>v2.w?w:v2.w; return _out;}
		inline float4&				scale(float4& _out,float s)				{_out.x = x*s; _out.y = y*s; _out.z = z*s; _out.w = w*s; return _out;}
		inline float4&				lerp(float4& _out,const float4& v2,float s)		{_out.x = x + s*(v2.x - x); _out.y = y + s*(v2.y - y); _out.z = z + s*(v2.z - z); _out.w = w + s*(v2.w - w); return _out;}
	};

	//浮点矩形数据
	struct	rect_f
	{
		float	left, top, right, bottom;
		//
		inline	rect_f()												{left=top=right=bottom=0;};
		inline	rect_f( const float* p)								{left=p[0];top=p[1];right=p[2];bottom=p[3];}
		inline	rect_f( float _left, float _top, float _right, float _bottom )		{left=_left;top=_top;right=_right;bottom=_bottom;}
		//
		inline	void	set(float l, float t, float r, float b){left = l; top = t; right = r; bottom = b;}
		inline rect_f& operator	+=(const rect_f& v)				{left+=v.left;top+=v.top;right+=v.right;bottom+=v.bottom;return *this;}
		inline rect_f& operator	-=(const rect_f& v)				{left-=v.left;top-=v.top;right-=v.right;bottom-=v.bottom;return *this;}
		inline rect_f& operator	*=(const rect_f& v)				{left*=v.left;top*=v.top;right*=v.right;bottom*=v.bottom;return *this;}
		inline rect_f& operator	/=(const rect_f& v)				{left/=v.left;top/=v.top;right/=v.right;bottom/=v.bottom;return *this;}
		inline rect_f& operator	=(const rect_f& v)					{left=v.left;top=v.top;right=v.right;bottom=v.bottom;return *this;}
		//
		inline rect_f& operator	+=(float f)							{left+=f;top+=f;right+=f;bottom+=f;return *this;}
		inline rect_f& operator	-=(float f)							{left-=f;top-=f;right-=f;bottom-=f;return *this;}
		inline rect_f& operator	*=(float f)							{left*=f;top*=f;right*=f;bottom*=f;return *this;}
		inline rect_f& operator	/=(float f)							{left/=f;top/=f;right/=f;bottom/=f;return *this;}
		//
		inline rect_f& operator ++()									{left+=1.0f;top+=1.0f;right+=1.0f;bottom+=1.0f;return *this;}
		inline rect_f& operator --()									{left-=1.0f;top-=1.0f;right-=1.0f;bottom-=1.0f;return *this;}
		//
		inline	float	GetWidth(){return right - left;}
		inline	float	GetHeight(){return bottom - top;}
		//
		inline rect_f  operator +()									{return *this;}
		inline rect_f  operator -()									{return rect_f(-left,-top,-right,-bottom);}
		//
		inline rect_f  operator +(const rect_f& v)					{return rect_f(left+v.left,top+v.top,right+v.right,bottom+v.bottom);}
		inline rect_f  operator -(const rect_f& v)					{return rect_f(left-v.left,top-v.top,right-v.right,bottom-v.bottom);}
		inline rect_f  operator *(const rect_f& v)					{return rect_f(left*v.left,top*v.top,right*v.right,bottom*v.bottom);}
		inline rect_f  operator /(const rect_f& v)					{return rect_f(left/v.left,top/v.top,right/v.right,bottom/v.bottom);}
		//
		inline rect_f  operator +(float f)								{return rect_f(left+f,top+f,right+f,bottom+f);}
		inline rect_f  operator -(float f)								{return rect_f(left-f,top-f,right-f,bottom-f);}
		inline rect_f  operator *(float f)								{return rect_f(left*f,top*f,right*f,bottom*f);}
		inline rect_f  operator /(float f)								{float fInv = 1.0f / f;return rect_f(left*fInv,top*fInv,right*fInv,bottom*fInv);}
		//
		inline bool 	operator ==(const rect_f& v)					{return (left==v.left&&top==v.top&&right==v.right&&bottom==v.bottom);}
		inline bool 	operator !=(const rect_f& v)					{return (left!=v.left||top!=v.top||right!=v.right||bottom!=v.bottom);}
		inline bool 	operator ==(float f)							{return (left==f&&top==f&&right==f&&bottom==f);}
		//
		inline bool 	operator !=(float f)							{return (left!=f||top!=f||right!=f||bottom!=f);}
	};

	//----------------------------------------------------------------------------------------------------------------
	//
	//quaternion
	//
	//----------------------------------------------------------------------------------------------------------------
	//旋转四元素数据
	/*STRUCT*/struct quaternion/*四元数*/
	{
		float	x;
		float	y;
		float	z;
		float	w;
		/*END_STRUCT*/
		//
		//float x, y, z, w;
		inline	quaternion()											{x=y=z=w=0;};
		inline	quaternion( const float* p)							{x=p[0];y=p[1];z=p[2];w=p[3];}
		inline	quaternion( const float3& xyz, float _w )			{x=xyz.x;y=xyz.y;z=xyz.z;w=_w;}
		inline	quaternion( float _x, float _y, float _z, float _w )	{x=_x;y=_y;z=_z;w=_w;}
		// casting
		inline	operator float* ()										{return (float *) &x;}
		// assignment operators
		inline quaternion& setXYZW(float px,float py,float pz,float pw)	{x=px;y=py;z=pz;w=pw;return *this;}
		//
		inline quaternion& operator	+=(const quaternion& v)		{x+=v.x;y+=v.y;z+=v.z;w+=v.w;return *this;}
		inline quaternion& operator	-=(const quaternion& v)		{x-=v.x;y-=v.y;z-=v.z;w-=v.w;return *this;}
		inline quaternion& operator	*=(const quaternion& v)		{x*=v.x;y*=v.y;z*=v.z;w*=v.w;return *this;}
		inline quaternion& operator	/=(const quaternion& v)		{x/=v.x;y/=v.y;z/=v.z;w/=v.w;return *this;}
		inline quaternion& operator	=(const quaternion& v)		{x=v.x;y=v.y;z=v.z;w=v.w;return *this;}
		//
		inline quaternion& operator	+=(float f)						{x+=f;y+=f;z+=f;w+=f;return *this;}
		inline quaternion& operator	-=(float f)						{x-=f;y-=f;z-=f;w-=f;return *this;}
		inline quaternion& operator	*=(float f)						{x*=f;y*=f;z*=f;w*=f;return *this;}
		inline quaternion& operator	/=(float f)						{x/=f;y/=f;z/=f;w/=f;return *this;}
		//
		inline quaternion& operator ++()								{x+=1.0f;y+=1.0f;z+=1.0f;w+=1.0f;return *this;}
		inline quaternion& operator --()								{x-=1.0f;y-=1.0f;z-=1.0f;w-=1.0f;return *this;}
		//
		inline quaternion  operator +()								{return *this;}
		inline quaternion  operator -()								{return quaternion(-x,-y,-z,-w);}
		//
		inline quaternion  operator +(const quaternion& v)		{return quaternion(x+v.x,y+v.y,z+v.z,w+v.w);}
		inline quaternion  operator -(const quaternion& v)		{return quaternion(x-v.x,y-v.y,z-v.z,w-v.w);}
		inline quaternion  operator *(const quaternion& v)		{return quaternion(x*v.x,y*v.y,z*v.z,w*v.w);}
		inline quaternion  operator /(const quaternion& v)		{return quaternion(x/v.x,y/v.y,z/v.z,w/v.w);}
		//
		inline quaternion  operator +(float f)						{return quaternion(x+f,y+f,z+f,w+f);}
		inline quaternion  operator -(float f)						{return quaternion(x-f,y-f,z-f,w-f);}
		inline quaternion  operator *(float f)						{return quaternion(x*f,y*f,z*f,w*f);}
		inline quaternion  operator /(float f)						{float fInv = 1.0f / f;return quaternion(x*fInv,y*fInv,z*fInv,w*fInv);}
		//
		inline bool operator ==(const quaternion& v)					{return (x==v.x&&y==v.y&&z==v.z&&w==v.w);}
		inline bool operator !=(const quaternion& v)					{return (x!=v.x||y!=v.y||z!=v.z||w!=v.w);}
		inline bool operator ==(float f)								{return (x==f&&y==f&&z==f&&w==f);}
		//
		inline float				length() const						{return sqrtf(x*x + y*y + z*z + w*w + 0.00001f);}
		inline float				lengthSq() const					{return x*x + y*y + z*z + w*w;}
		inline float				dot(const quaternion& v2)const		{return x*v2.x + y*v2.y + z*v2.z + w*v2.w;}
		inline void					identity()							{x = y = z = 0; w = 1.0f;}
		inline bool					isIdentity()						{return (x == 0.0f && y == 0.0f && z == 0.0f && w == 1.0f);}
		inline quaternion&		conjugate(quaternion& _out)		{_out.x = -x; _out.y = -y; _out.z = -z; _out.w = w; return _out;}
		//
	};


	//----------------------------------------------------------------------------------------------------------------
	//
	//Plane
	//
	//----------------------------------------------------------------------------------------------------------------
	//三维平面数据
	/*STRUCT*/struct	Plane/*平面*/
	{
		float a;
		float b;
		float c;
		float d;
		/*END_STRUCT*/
		//float a, b, c, d;
		inline	Plane()												{a=b=c=d=0;};
		inline	Plane( const float* p)								{a=p[0];b=p[1];c=p[2];d=p[3];}
		inline	Plane( const float3& xyz, float _d )				{a=xyz.x;b=xyz.y;c=xyz.z;d=_d;}
		inline	Plane( float _a, float _b, float _c, float _d )		{a=_a;b=_b;c=_c;d=_d;}
		// casting
		inline	operator float* ()										{return (float *) &a;}
		// assignment operators
		inline Plane& setabcd(float pa,float pb,float pc,float pd)	{a=pa;b=pb;c=pc;d=pd;return *this;}
		//
		inline Plane& operator	+=(const Plane& v)				{a+=v.a;b+=v.b;c+=v.c;d+=v.d;return *this;}
		inline Plane& operator	-=(const Plane& v)				{a-=v.a;b-=v.b;c-=v.c;d-=v.d;return *this;}
		inline Plane& operator	*=(const Plane& v)				{a*=v.a;b*=v.b;c*=v.c;d*=v.d;return *this;}
		inline Plane& operator	/=(const Plane& v)				{a/=v.a;b/=v.b;c/=v.c;d/=v.d;return *this;}
		inline Plane& operator	=(const Plane& v)				{a=v.a;b=v.b;c=v.c;d=v.d;return *this;}
		//
		inline Plane& operator	+=(float f)							{a+=f;b+=f;c+=f;d+=f;return *this;}
		inline Plane& operator	-=(float f)							{a-=f;b-=f;c-=f;d-=f;return *this;}
		inline Plane& operator	*=(float f)							{a*=f;b*=f;c*=f;d*=f;return *this;}
		inline Plane& operator	/=(float f)							{a/=f;b/=f;c/=f;d/=f;return *this;}
		//
		inline Plane& operator ++()									{a+=1.0f;b+=1.0f;c+=1.0f;d+=1.0f;return *this;}
		inline Plane& operator --()									{a-=1.0f;b-=1.0f;c-=1.0f;d-=1.0f;return *this;}
		//
		inline Plane  operator +()									{return *this;}
		inline Plane  operator -()									{return Plane(-a,-b,-c,-d);}
		//
		inline Plane  operator +(const Plane& v)					{return Plane(a+v.a,b+v.b,c+v.c,d+v.d);}
		inline Plane  operator -(const Plane& v)					{return Plane(a-v.a,b-v.b,c-v.c,d-v.d);}
		inline Plane  operator *(const Plane& v)					{return Plane(a*v.a,b*v.b,c*v.c,d*v.d);}
		inline Plane  operator /(const Plane& v)					{return Plane(a/v.a,b/v.b,c/v.c,d/v.d);}
		//
		inline Plane  operator +(float f)							{return Plane(a+f,b+f,c+f,d+f);}
		inline Plane  operator -(float f)							{return Plane(a-f,b-f,c-f,d-f);}
		inline Plane  operator *(float f)							{return Plane(a*f,b*f,c*f,d*f);}
		inline Plane  operator /(float f)							{float fInv = 1.0f / f;return Plane(a*fInv,b*fInv,c*fInv,d*fInv);}
		//
		inline bool operator ==(const Plane& v)						{return (a==v.a&&b==v.b&&c==v.c&&d==v.d);}
		inline bool operator !=(const Plane& v)						{return (a!=v.a||b!=v.b||c!=v.c||d!=v.d);}
		inline bool operator ==(float f)								{return (a==f&&b==f&&c==f&&d==f);}
		//
		inline void					identity()							{a=b=c=d=0.0f;}
		// ax + by + cz + dw //pP = this
		inline float				dot(const float4& pV)				{return a*pV.x + b*pV.y + c*pV.z + d*pV.w;}
		// ax + by + cz + d
		inline float				dotCoord(const float3& pV)			{return a*pV.x + b*pV.y + c*pV.z + d;}
		// ax + by + cz
		inline float				dotNormal(const float3& pV)		{return a*pV.x + b*pV.y + c*pV.z;}
		inline Plane&			scale(Plane& _out,float s)		{_out.a = a*s; _out.b = b*s; _out.c = c*s; _out.d = d*s; return _out;}
		Plane( const float3& p1, const float3& p2, const float3& p3 );
	};




	//----------------------------------------------------------------------------------------------------------------
	//
	//color4
	//
	//----------------------------------------------------------------------------------------------------------------
	//颜色数据
	/*STRUCT*/struct color4/*颜色*/
	{
		float r;
		float g;
		float b;
		float a;
		/*END_STRUCT*/
		//从ARGB表转换为颜色数据
		color4( unsigned int dwBGRA )											{	const float f = 1.0f / 255.0f;	r = f * (float) (unsigned char) (dwBGRA >> 16);	g = f * (float) (unsigned char) (dwBGRA >>  8);	b = f * (float) (unsigned char) (dwBGRA >>  0);	a = f * (float) (unsigned char) (dwBGRA >> 24);	}
		color4( unsigned int dwRGB, float fAlpha)								{	const float f = 1.0f / 255.0f;	r = f * (float) (unsigned char) (dwRGB);	g = f * (float) (unsigned char) (dwRGB >>  8);	b = f * (float) (unsigned char) (dwRGB >>  16);	a = fAlpha;	}
		//返回ARGB值
		operator unsigned int ()											const {	unsigned int dwR = r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : (unsigned int) (r * 255.0f + 0.5f);	unsigned int dwG = g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : (unsigned int) (g * 255.0f + 0.5f);	unsigned int dwB = b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : (unsigned int) (b * 255.0f + 0.5f);	unsigned int dwA = a >= 1.0f ? 0xff : a <= 0.0f ? 0x00 : (unsigned int) (a * 255.0f + 0.5f);	return (dwA << 24) | (dwR << 16) | (dwG << 8) | dwB;	}
		//从ARGB值转换成颜色数据
		color4&					from(unsigned int dwBGRA)				{	const float f = 1.0f / 255.0f;	r = f * (float) (unsigned char) (dwBGRA >> 16);	g = f * (float) (unsigned char) (dwBGRA >>  8);	b = f * (float) (unsigned char) (dwBGRA >>  0);	a = f * (float) (unsigned char) (dwBGRA >> 24);	return *this;	}
		//从RGB值和透明值转换近来，GDI的RGB值
		color4&					from(unsigned int dwRGB,float fAlpha)	{	const float f = 1.0f / 255.0f;	b = f * (float) (unsigned char) (dwRGB >> 16);	g = f * (float) (unsigned char) (dwRGB >>  8);	r = f * (float) (unsigned char) (dwRGB >>  0);	a = fAlpha;	return *this;	}
		//返回GDI的RGB值
		unsigned int				getRGB()								const {	unsigned int dwR = r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : (unsigned int) (r * 255.0f + 0.5f);	unsigned int dwG = g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : (unsigned int) (g * 255.0f + 0.5f);	unsigned int dwB = b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : (unsigned int) (b * 255.0f + 0.5f);	return (dwB << 16) | (dwG << 8) | dwR;	}
		//
		inline	color4()													{r=g=b=a=1.0f;}
		inline	color4( const float* p)									{r=p[0];g=p[1];b=p[2];a=p[3];}
		inline	color4( const float3& xyz, float _a )					{r=xyz.x;g=xyz.y;b=xyz.z;a=_a;}
		inline	color4( float _r, float _g, float _b, float _a )			{r=_r;g=_g;b=_b;a=_a;}
		//
		inline	void				swapRB()								{float _r = r; r = b; b = _r;}
		// casting
		inline	operator float* ()											{return (float *) &r;}
		// assignment operators
		inline color4& setRGBA(float pr,float pg,float pb,float pa)		{r=pr;g=pg;b=pb;a=pa;return *this;}
		//
		inline color4& operator	+=(const color4& v)					{r+=v.r;g+=v.g;b+=v.b;a+=v.a;return *this;}
		inline color4& operator	-=(const color4& v)					{r-=v.r;g-=v.g;b-=v.b;a-=v.a;return *this;}
		inline color4& operator	*=(const color4& v)					{r*=v.r;g*=v.g;b*=v.b;a*=v.a;return *this;}
		inline color4& operator	/=(const color4& v)					{r/=v.r;g/=v.g;b/=v.b;a/=v.a;return *this;}
		inline color4& operator	=(const color4& v)					{r=v.r;g=v.g;b=v.b;a=v.a;return *this;}
		//
		inline color4& operator	+=(float f)								{r+=f;g+=f;b+=f;a+=f;return *this;}
		inline color4& operator	-=(float f)								{r-=f;g-=f;b-=f;a-=f;return *this;}
		inline color4& operator	*=(float f)								{r*=f;g*=f;b*=f;a*=f;return *this;}
		inline color4& operator	/=(float f)								{r/=f;g/=f;b/=f;a/=f;return *this;}
		//
		inline color4& operator ++()										{r+=1.0f;g+=1.0f;b+=1.0f;a+=1.0f;return *this;}
		inline color4& operator --()										{r-=1.0f;g-=1.0f;b-=1.0f;a-=1.0f;return *this;}
		//
		inline color4  operator +()						const			{return *this;}
		inline color4  operator -()						const			{return color4(-r,-g,-b,-a);}
		//
		inline color4  operator +(const color4& v)		const			{return color4(r+v.r,g+v.g,b+v.b,a+v.a);}
		inline color4  operator -(const color4& v)		const			{return color4(r-v.r,g-v.g,b-v.b,a-v.a);}
		inline color4  operator *(const color4& v)		const			{return color4(r*v.r,g*v.g,b*v.b,a*v.a);}
		inline color4  operator /(const color4& v)		const			{return color4(r/v.r,g/v.g,b/v.b,a/v.a);}
		//
		inline color4  operator +(float f)				const			{return color4(r+f,g+f,b+f,a+f);}
		inline color4  operator -(float f)				const			{return color4(r-f,g-f,b-f,a-f);}
		inline color4  operator *(float f)				const			{return color4(r*f,g*f,b*f,a*f);}
		inline color4  operator /(float f)				const			{float fInv = 1.0f / f;return color4(r*fInv,g*fInv,b*fInv,a*fInv);}
		//
		inline bool operator ==(const color4& v)		const			{return (r==v.r&&g==v.g&&b==v.b&&a==v.a);}
		inline bool operator !=(const color4& v)		const			{return (r!=v.r||g!=v.g||b!=v.b||a!=v.a);}
		inline bool operator ==(float f)				const			{return (r==f&&g==f&&b==f&&a==f);}
		//
		inline void					identity()										{r=g=b=0.0f;a=1.0f;}
		inline color4&			negative(color4& _out)						{_out.r = 1.0f - r;	_out.g = 1.0f - g;	_out.b = 1.0f - b;	_out.a = a; return _out;}
		inline color4&			add(color4& _out,const color4& v2)			{_out.r = r + v2.r;	_out.g = g + v2.g;	_out.b = b + v2.b;	_out.a = a + v2.a; return _out;}
		inline color4&			subtract(color4& _out,const color4& v2)		{_out.r = r - v2.r;	_out.g = g - v2.g;	_out.b = b - v2.b;	_out.a = a - v2.a;	return _out;}
		inline color4&			scale(color4& _out,float s)					{_out.r = r*s;	_out.g = g*s;	_out.b = b*s;	_out.a = a*s;	return _out;}
		inline color4&			modulate(color4& _out,const color4& v2)		{_out.r = r*v2.r;	_out.g = g*v2.g;	_out.b = b*v2.b;	_out.a = a*v2.a;	return _out;}
		inline color4&			lerp(color4& _out,const color4& v2,float s) const	{_out.r = r + s*(v2.r - r);	_out.g = g + s*(v2.g - g);	_out.b = b + s*(v2.b - b);	_out.a = a + s*(v2.a - a);	return _out;}
		static color4			s_color;
	};





	//----------------------------------------------------------------------------------------------------------------
	//
	//matrix4x4
	//
	//----------------------------------------------------------------------------------------------------------------
	//三维矩阵数据
	/*STRUCT*/struct matrix4x4/*矩阵*/
	{
		union{
			struct{
				float		_11;
				float		_12;
				float		_13;
				float		_14;
				float		_21;
				float		_22;
				float		_23;
				float		_24;
				float		_31;
				float		_32;
				float		_33;
				float		_34;
				float		_41;
				float		_42;
				float		_43;
				float		_44;
				/*END_STRUCT*/
			};
			struct{
				float	m[4][4];
			};
			struct{
				float	f[16];
			};
		};
		//
		inline	void				SetPosition(const float3& v)				{float3* pVec = (float3*)&_41; *pVec = v;}
		inline	void				SetPosition(float x, float y, float z)		{_41 = x; _42 = y; _43 = z;}
		inline	const float3&		GetPosition()								const {return *(float3*)&_41;}
		//inline	operator	D3DXMATRIX&	(){return (D3DXMATRIX&)*this;}
		matrix4x4();
		inline	static const matrix4x4&	s_identity(){ static matrix4x4 m(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f); return m;}
		//inline	matrix4x4( bool bIdentity );
		matrix4x4( const float* );
		matrix4x4( const matrix4x4& );
		matrix4x4( float _11, float _12, float _13, float _14,			float _21, float _22, float _23, float _24,			float _31, float _32, float _33, float _34,			float _41, float _42, float _43, float _44 );
		operator float* ();
		operator const float* () const;
		// assignment operators
		matrix4x4& 			operator *= ( const matrix4x4& );
		matrix4x4& 			operator += ( const matrix4x4& );
		matrix4x4& 			operator -= ( const matrix4x4& );
		matrix4x4& 			operator *= ( float );
		matrix4x4& 			operator /= ( float );
		// unary operators
		matrix4x4 			operator + () const;
		matrix4x4 			operator - () const;
		// binary operators
		matrix4x4 			operator * ( const matrix4x4& ) const;
		matrix4x4 			operator / ( const matrix4x4& ) const;
		bool				operator == ( const matrix4x4& ) const;
		//
		matrix4x4 			operator + ( const matrix4x4& ) const;
		matrix4x4 			operator - ( const matrix4x4& ) const;
		matrix4x4 			operator * ( float ) const;
		matrix4x4 			operator / ( float ) const;
		matrix4x4&			operator	=	(const matrix4x4& v);
		friend	matrix4x4			operator * ( float, const matrix4x4& );
		//
		void				PerspectiveFovRH(float fovy, float Aspect, float zn, float zf);
		void				OrthoRH(float w,float h,float zn,float zf);
		void				LookAtRH(const float3& eye,const float3& at,const float3& up);
		void				RotationX(float f);
		void				RotationY(float f);
		void				RotationZ(float f);
		float				CalculateDeterminant3( float _11, float _12, float _13, float _21, float _22, float _23, float _31, float _32, float _33) const;
		float				Determinant() const;
		void				Invert( float * pDeterminant );
		//
		void				identity();
		bool				isIdentity();
		//
		inline float3&				getM1()								{return (float3&)_11;}
		inline float3&				getM2()								{return (float3&)_21;}
		inline float3&				getM3()								{return (float3&)_31;}
	};

	enum{
		transType_Position		=	0x0001,	//位置有效
		transType_Scale			=	0x0002,	//缩放有效
		transType_Rotation		=	0x0004,	//转动有效
	};
	struct double3
	{
		double x,y,z;
		double3(){x=y=z=0;}
		double3(double _x, double _y, double _z){x=_x;y=_y;z=_z;}
		void setxyz(double _x, double _y, double _z){x=_x;y=_y;z=_z;}
		void setComponent(int i, double val){
			if(i==0)
				x=val;
			else if(i==1)
				y=val;
			else
				z=val;
		}
		void clear(){x=y=z=0;}
		void normalize(){
			double d = length();
			if (d != 0.0){
				d=1.0/d;
				x*=d;y*=d;z*=d;
			}
		}
		double	dot(const double3& b) const {return x * b.x + y * b.y + z * b.z;}
		double	length() const {return sqrt(x * x + y * y + z * z);}
		void operator *= (double s){x *= s;y *= s;z *= s;}
		BOOL	operator == (const double3& other) const {return (x == other.x) && (y == other.y) && (z == other.z);}
		double3	operator -(const double3& b) const {return double3(x - b.x, y - b.y, z - b.z);}
		double3	cross(const double3& b) const {return double3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);}
		double3	ortho() const {
			int k = largestAbsComponent() - 1;
			if (k < 0) {
				k = 2;
			}
			double3 ret;
			ret.setComponent(k, 1.0);
			ret = cross(ret);
			ret.normalize();
			return ret;
		}
		int		largestAbsComponent() const 
		{
			double xAbs = fabs(x);
			double yAbs = fabs(y);
			double zAbs = fabs(z);
			if (xAbs > yAbs) {
				if (xAbs > zAbs) {
					return 0;
				}
				return 2;
			}
			if (yAbs > zAbs) {
				return 1;
			}
			return 2;
		}
	};
	struct matrix3x3d
	{
		union{
			struct{
				float	mm[3][3];
			};
			struct{
				float	f[9];
			};
		};
		matrix3x3d(){}
		matrix3x3d(double m00, double m01, double m02, double m10, double m11, double m12, double m20, double m21, double m22){
			f[0] = m00;f[1] = m01;f[2] = m02;f[3] = m10;f[4] = m11;f[5] = m12;f[6] = m20;f[7] = m21;f[8] = m22;
		}
		void clear(){
			for(int i=0;i<9;i++)
				f[i] = 0;
		}
		void identity(){
			f[1] = f[2] = f[3] = f[5] = f[6] = f[7] = 0.0;
			f[0] = f[4] = f[8] = 1.0;
		}
		void setval(double d){
			f[0] = f[4] = f[8] = d;
		}
		void getcol(int col, double3& ret){
			ret.x = f[col];
			ret.y = f[(col + 3)];
			ret.z = f[(col + 6)];
		}
		void setcol(int col, const double3& v){
			f[col] = v.x;
			f[(col + 3)] = v.y;
			f[(col + 6)] = v.z;
		}
		void operator *= (double s){
			for(int i=0;i<9;i++)
				f[i]*=s;
		}
		void operator += (const matrix3x3d& b){
			for(int i=0;i<9;i++)
				f[i]+=b.f[i];
		}
		void operator -= (const matrix3x3d& b){
			for(int i=0;i<9;i++)
				f[i]-=b.f[i];
		}
		void transpose(){
			double tmp = f[1];
			f[1] = f[3];
			f[3] = tmp;
			tmp = f[2];
			f[2] = f[6];
			f[6] = tmp;
			tmp = f[5];
			f[5] = f[7];
			f[7] = tmp;
		}
		void transpose(matrix3x3d& ret) const {
			double f1 = f[1];double f2 = f[2];double f5 = f[5];
			ret.f[0] = f[0];ret.f[1] = f[3];ret.f[2] = f[6];
			ret.f[3] = f1;ret.f[4] = f[4];ret.f[5] = f[7];
			ret.f[6] = f2;ret.f[7] = f5;ret.f[8] = f[8];
		}
		matrix3x3d operator + (const matrix3x3d& b) const {
			matrix3x3d mat;
			for(int i=0;i<9;i++)
				mat.f[i] = f[i] + b.f[i];
			return mat;
		}
		matrix3x3d operator * (const matrix3x3d& b) const {
			return matrix3x3d(f[0] * b.f[0] + f[1] * b.f[3] + f[2] * b.f[6], f[0] * b.f[1] + f[1] * b.f[4] + f[2] * b.f[7], f[0] * b.f[2] + f[1] * b.f[5] + f[2] * b.f[8], f[3] * b.f[0] + f[4] * b.f[3] + f[5] * b.f[6], f[3] * b.f[1] + f[4] * b.f[4] + f[5] * b.f[7], f[3] * b.f[2] + f[4] * b.f[5] + f[5] * b.f[8], f[6] * b.f[0] + f[7] * b.f[3] + f[8] * b.f[6], f[6] * b.f[1] + f[7] * b.f[4] + f[8] * b.f[7], f[6] * b.f[2] + f[7] * b.f[5] + f[8] * b.f[8]);
		}
		double3	operator * (const double3& v) const {
			return double3(
				f[0] * v.x + f[1] * v.y + f[2] * v.z,
				f[3] * v.x + f[4] * v.y + f[5] * v.z, 
				f[6] * v.x + f[7] * v.y + f[8] * v.z);
		}
		double determinant() const {
			return mm[0][0] * (mm[1][1] * mm[2][2] - mm[2][1] * mm[1][2]) - mm[0][1] * (mm[1][0] * mm[2][2] - mm[1][2] * mm[2][0]) + mm[0][2] * (mm[1][0] * mm[2][1] - mm[1][1] * mm[2][0]);
		}
		BOOL	invert(matrix3x3d& ret) const {
			double d = determinant();
			if (d == 0.0){
				return false;
			}
			double invdet = 1.0/d;
			ret = matrix3x3d((f[4] * f[8] - f[7] * f[5]) * invdet, -(f[1] * f[8] - f[2] * f[7]) * invdet, (f[1] * f[5] - f[2] * f[4]) * invdet, -(f[3] * f[8] - f[5] * f[6]) * invdet, (f[0] * f[8] - f[2] * f[6]) * invdet, -(f[0] * f[5] - f[3] * f[2]) * invdet, (f[3] * f[7] - f[6] * f[4]) * invdet, -(f[0] * f[7] - f[6] * f[1]) * invdet, (f[0] * f[4] - f[3] * f[1]) * invdet);
			return true;
		}
	};
	struct	PosColor
	{
		float3			pos;
		unsigned int	color;
	};
	struct	PosColorUV
	{
		float3			pos;
		unsigned int	color;
		float2			uv;
	};
	struct	PosUV2
	{
		float3			p;
		float2			uv[2];
		static const int	FVF	=	FVF_XYZ | FVF_TEX2;
	};
	struct	PosUV3
	{
		float3			p;
		float2			uv[3];
		static const int	FVF	=	FVF_XYZ | FVF_TEX3;
	};
	struct	PosUV
	{
		float3			pos;
		float2			uv;
	};
	struct	PosNormalColor
	{
		float3			pos;
		float3			normal;
		unsigned int	color;
		static const int	FVF	=	FVF_XYZ | FVF_DIFFUSE | FVF_NORMAL;
	};
	struct	PosNormalColorUV
	{
		float3			p;
		float3			n;
		unsigned int	diffuse;
		float			tu,tv;
		static const int	FVF	=	FVF_XYZ | FVF_DIFFUSE | FVF_NORMAL | FVF_TEX1;
	};
	struct	PosNormalUV
	{
		float3			pos;
		float3			normal;
		float2			uv;
		static const int	FVF	=	FVF_XYZ | FVF_NORMAL | FVF_TEX1;
	};
	struct	PosNormalColorUV2
	{
		float3			pos;
		float3			normal;
		unsigned int	color;
		float2			uv[2];
		static const int	FVF	=	FVF_XYZ | FVF_DIFFUSE | FVF_NORMAL | FVF_TEX2;
	};
	struct	PosNormalUV2
	{
		float3			pos;
		float3			normal;
		float2			uv[2];
		static const int	FVF	=	FVF_XYZ | FVF_NORMAL | FVF_TEX2;
	};
#pragma pack(pop)
	//
	/*STRUCT*/struct	AABox/*三维盒子*/
	{
		float3			_min/*最小点*/;
		float3			_max/*最大点*/;
	};/*END_STRUCT*/

	struct	PBoxVertices
	{
		float3			v[8];
		float3&		get_min(){return v[0];}
		float3&		get_max(){return v[6];}
		void			setup(const float3& _min,const float3& _max)
		{
			//
			v[0].setxyz(_min.x, _min.y, _min.z);
			v[1].setxyz(_min.x, _max.y, _min.z);
			v[2].setxyz(_max.x, _max.y, _min.z);
			v[3].setxyz(_max.x, _min.y, _min.z);
			v[4].setxyz(_min.x, _min.y, _max.z);
			v[5].setxyz(_min.x, _max.y, _max.z);
			v[6].setxyz(_max.x, _max.y, _max.z);
			v[7].setxyz(_max.x, _min.y, _max.z);
			//
		}
		void			setMinMaxZ(float fminZ, float fmaxZ)
		{
			//
			v[0].z = fminZ;
			v[1].z = fminZ;
			v[2].z = fminZ;
			v[3].z = fminZ;

			v[4].z = fmaxZ;
			v[5].z = fmaxZ;
			v[6].z = fmaxZ;
			v[7].z = fmaxZ;
		}
		inline	char		is_ptInBox(const float3& p)
		{
			if(p.x<v[0].x)
				return false;
			if(p.y<v[0].y)
				return false;
			if(p.z<v[0].z)
				return false;
			if(p.x>v[6].x)
				return false;
			if(p.y>v[6].y)
				return false;
			if(p.z>v[6].z)
				return false;
			return true;
		}
		inline	char		is_boxInBox(const float3& _min,const float3& _max)
		{
			if(_max.x<v[0].x)
				return false;
			if(_max.y<v[0].y)
				return false;
			if(_max.z<v[0].z)
				return false;
			if(_min.x>v[6].x)
				return false;
			if(_min.y>v[6].y)
				return false;
			if(_min.z>v[6].z)
				return false;
			return true;
		}
		inline	char		is_sphereInBox(const float3& c,float r)
		{
			float3 _min(c.x - r, c.y - r, c.z - r);
			float3 _max(c.x + r, c.y + r, c.z + r);
			return is_boxInBox(_min,_max);
		}
	};
	//
	struct	PSphereData
	{
		float3	center;
		float	radius;
		float	radius2x;
		void	setup(const float3& _center,float _radius)
		{
			center = _center;
			radius = _radius;
			radius2x = _radius*_radius;
		}
	};

	//
	struct	PDirection3D
	{
		float3	orig;
		float3	ldir;
		float	dirLength;
		void	setup(float3& _orig,float3& _dir){orig = _orig; ldir = _dir; ldir.normalv(ldir); dirLength = ldir.radius();}
		//t0 = dir dot (Q - P) //Q另一个点，P是线段始点
		//dist = Q - (P+ t0*dir) = Q-P - t0*dir
		inline	bool	isSpherePick(const PSphereData& s) const //float3& objectCenter,float objectRadius)
		{
			return isSpherePick(s.center, s.radius2x);
		}
		inline	bool	isSpherePick(const float3& center, float radius2x) const //float3& objectCenter,float objectRadius)
		{
			if(radius2x<=0)return true;
			float3 QP = center - orig;
			float t0 = ldir.dot(QP);
			if(t0<0)//如果物体在线段后方则返回
			{
				return radius2x>(QP.dot(QP));
			}
			float3 r2x(QP.x - t0*ldir.x,QP.y - t0*ldir.y,QP.z - t0*ldir.z);// = QP - t0*ldir;
			return (r2x.dot(r2x)<radius2x);
		}

		inline	bool	isSpherePick( const PSphereData& e , float* t) const
		{
			int numSoln = 0;
			float a, b, c, discrm;
			float3 oc = orig - e.center;

			a = ldir.dot(ldir);
			b = 2 * ldir.dot(oc);
			c = oc.dot(oc) - e.radius2x;
			discrm = b*b - 4*a*c;

			if(discrm > 0)
			{
				//if(t)
				//	t[numSoln] = (-b + sqrt(discrm))/ (2 * a);
				numSoln++;
				if(t)
					t[0] = (-b - sqrt(discrm))/ (2 * a);
				numSoln++;
			}
			else if(discrm == 0)
			{
				if(t)
					t[0] = -b / (2 * a);
				numSoln ++;
			}
			return (numSoln > 0);
		}
	};
	struct	PObjectRegion
	{
		PSphereData			sphere;
		AABox				aabox;
	};


	struct	PIntersectData
	{
		float*		t;
		float3*		Pick;
		float*		u,*v;
		int			faceID;
		float3*		normal;
		PIntersectData(){t = 0; Pick = 0; faceID = 0; u = 0; v = 0; normal = 0;}
		bool		getPick(float3& _out, const float3& vOrig, const float3& vDir){if(t){_out = vOrig + vDir * (*t); return true;}return false;}
	};

	inline float3&			float3::transform(float3& dest,const matrix4x4& _m) const 
	{
		float vx=x,vy=y,vz=z;
		dest.x	=	vx	*	_m._11	+	vy	*	_m._21	+	vz	*	_m._31	+	_m._41;
		dest.y	=	vx	*	_m._12	+	vy	*	_m._22	+	vz	*	_m._32	+	_m._42;
		dest.z	=	vx	*	_m._13	+	vy	*	_m._23	+	vz	*	_m._33	+	_m._43;
		return dest;
	}
	inline float3&			float3::transformNoMove(float3& dest,const matrix4x4& _m) const 
	{
		float vx=x,vy=y,vz=z;
		dest.x	=	vx	*	_m._11	+	vy	*	_m._21	+	vz	*	_m._31;
		dest.y	=	vx	*	_m._12	+	vy	*	_m._22	+	vz	*	_m._32;
		dest.z	=	vx	*	_m._13	+	vy	*	_m._23	+	vz	*	_m._33;
		return dest;
	}

	/*STRUCT*/struct	short2/*短整数2*/
	{
		short	x;
		short	y;
		/*END_STRUCT*/
		short2(){x=y=0;}
	};

	/*STRUCT*/struct	short4/*短整数4*/
	{
		short	x;
		short	y;
		short	z;
		short	w;
		/*END_STRUCT*/
		short4(){x=y=z=w=0;}
	};

	/*STRUCT*/struct	ubyte4/*字节4*/
	{
		char	x;
		char	y;
		char	z;
		char	w;
		/*END_STRUCT*/
		ubyte4(){x=y=z=w=0;}
	};

	/*STRUCT*/struct	PixelColor/*屏幕坐标颜色*/
	{
		int			x/*左*/;
		int			y/*上*/;
		color4		color/*颜色*/;
		/*END_STRUCT*/
		PixelColor(){x=y=0;}
		PixelColor(int _x, int _y, const color4& c){x=_x;y=_y;color=c;}
		///
		inline PixelColor  operator 	+(const PixelColor& v) const			{return PixelColor(x+v.x,y+v.y,color+v.color);}
		inline PixelColor  operator 	-(const PixelColor& v) const			{return PixelColor(x-v.x,y-v.y,color-v.color);}
		inline PixelColor  operator 	*(const PixelColor& v) const			{return PixelColor(x*v.x,y*v.y,color*v.color);}
		inline PixelColor  operator 	/(const PixelColor& v) const			{return PixelColor(x/v.x,y/v.y,color/v.color);}
		//
		inline bool operator 		==(const PixelColor& v) const				{return (x==v.x&&y==v.y&&color==v.color);}
	};

	/*STRUCT*/struct Vertex/*顶点数据*/
	{
		float3							pos/*位置*/;
		float3							normal/*法线*/;
		float2							uv1;/*纹理坐标1*/;
		float2							uv2;/*纹理坐标2*/;
	};/*END_STRUCT*/
	//unsigned int					color/*颜色*/;

	/*STRUCT*/struct	Triangle/*三角形索引*/
	{
		unsigned short	v1/*顶点索引1*/;
		unsigned short	v2/*顶点索引2*/;
		unsigned short	v3/*顶点索引3*/;
		/*END_STRUCT*/
		Triangle(){}
		Triangle(unsigned short _v1, unsigned short _v2, unsigned short _v3)
		{
			v1	=	_v1;
			v2	=	_v2;
			v3	=	_v3;
		}
		inline	void	set(unsigned short _v1, unsigned short _v2, unsigned short _v3)
		{
			v1	=	_v1;
			v2	=	_v2;
			v3	=	_v3;
		}
	};

	struct	Triangle32
	{
		Triangle32(){}
		Triangle32(unsigned int _v1, unsigned int _v2, unsigned int _v3)
		{
			v1	=	_v1;
			v2	=	_v2;
			v3	=	_v3;
		}
		unsigned int	v1,v2,v3;
		inline	void	set(unsigned int _v1, unsigned int _v2, unsigned int _v3)
		{
			v1	=	_v1;
			v2	=	_v2;
			v3	=	_v3;
		}
	};

	struct	PointColor
	{
		float3		pos;
		color4		c;
		PointColor(){}
		PointColor(const float3& v, const color4& _c){pos = v; c = _c;}
		PointColor(float x, float y, float z, float r, float g, float b, float a)
		{
			pos.setxyz(x,y,z);
			c.setRGBA(r,g,b,a);
		}
	};

	struct ScreenVertex
	{
		float3 pos;
		color4 c;
		float2 uv;
	};

	struct EmissionParticle
	{
		float						fLifeTime;				//当前播放时间
		float						fLifeTimeLength;		//总播放时间
		float						fImageBeginIndex;		//开始图片动画索引
		//
		float3						scale;					//当前大小
		float2						rotation;				//当前转动角度
		float						offsetSelf;
		color4						color;
		float3						speed;					//当前位置
		float						selfRotation;
		//
		float3						scale_r;				//当前大小
		float2						rotation_r;				//当前转动角度
		float						offsetSelf_r;
		color4						color_r;
		float3						offset_r;				//当前位置
		float3						planeOffset_r;
		float3						windOffset;
		//
		BOOL						bSelfRotation;
		float						selfRotation_r;
		//
		float3						scaleBegin;
		//
		matrix4x4					worldBegin;				//发射时候的矩阵
		matrix4x4					worldRender;			//发射时候的矩阵
		EmissionParticle*			next;
	};

	//shader中一个变量或者函数的指针寄存器
	struct	_shader_var
	{
		enum{
			shader_find_ok			=	1,
			shader_find_failure		=	2,
			shader_find_technique	=	3,
		};
		//
		unsigned char			itype;
		__int64					handle;
		unsigned int			programID;
		unsigned char			resetID;
		//unsigned char			st;
		//
		//enum S_T{
		//	S_T_Attribuate	=	0,
		//	S_T_Uniform,
		//};
		_shader_var()			{resetID = 0; itype = 0; handle = 0; programID = 0;}// st = S_T_Attribuate;}
		//inline	void			setUniform(){st = S_T_Uniform;}
		//
		inline	bool			is_findFail(){return (itype == shader_find_failure);}
		inline	bool			is_findOK(){return (itype == shader_find_ok);}
		inline	bool			is_findTechniqueOK(){return (itype == shader_find_technique);}
		//
		void					set_null(){itype = shader_find_failure;}
		void					set_handle(__int64 h)		{handle = h;	itype = shader_find_ok;}
		void					set_technique(__int64 h)		{handle = h;	itype = shader_find_technique;}
		void					reset_handle()					{handle = 0;	itype = 0;}
		//
	};

	inline	float		RandomF(float fMin, float fMax)					{return (fMin + (fMax - fMin) * ((float)rand() / RAND_MAX));}
	extern	float		getRotateZ(const float3& _dir);
	extern	float		clampAngle(float fAngle);
	class	IntersectData;
	struct Pixel;
	extern	BOOL		ObjectInLinear( IntersectData* intersect, const PObjectRegion& obj , const matrix4x4* world);
	extern	BOOL		ObjectInLinear( IntersectData* intersect, const PSphereData& obj , const matrix4x4* world);
	extern	BOOL		IntersectTriangle(PIntersectData* pOutPick, const float3& vOrig, const float3& vDir, const float3& v0, const float3& v1, const float3& v2, const matrix4x4* pWorld, bool bDoubleSide = false);
	extern	BOOL		IntersectTriangle(const Pixel& cursor, const Pixel& v0, const Pixel& v1, const Pixel& v2);
	extern	BOOL		IntersectTriangleList(PIntersectData* pOutPick, const float3& vOrig, const float3& vDir, const void* _verts, const void* pIndices, int numFace, const matrix4x4* pWorld, int strideSize, int cullMode = CullMode_Double);
	extern	float		LineCrossPoint(float3* pOutCross, const float3& orig, const float3& dir, const float3& point);
	extern	float		LineInPointY(float2* vOutCross, const float2& orig, const float2& dir, const float2& point);
	extern	float		LineInPointX(float2* vOutCross, const float2& orig, const float2& dir, const float2& point);
	extern	BOOL		IntersectBox(IntersectData* p, const float3& vMin, const float3& vMax, const matrix4x4* pWorld);
	extern	BOOL		PickBoxLinear(const float2& orig, const float2& dir, const float2& vMin, const float2& vMax, float2* retPick);
	extern	BOOL		IntersectBox(const float3& vOrig, const float3& vDir, const float3& vMin, const float3& vMax, const matrix4x4* pWorld);
	extern	BOOL		IsSphereInAABox(const AABox& parent, const PSphereData& child);
	extern	BOOL		IsPointInAABox(const AABox& parent, const float3& child);
	extern	BOOL		IsFullClipAABox(const AABox& parent, const AABox& child);
	extern	BOOL		IsAABoxInAABox(const AABox& parent, const AABox& child);
	//
	extern	float2		HermiteCurve(const float2& p1,const float2& p2,const float2& t1,const float2& t2, float fLerp);
	extern	float2		LineCurve(const float2& p1,const float2& p2, float fLerp);
	extern	float		clampDegress(float fDegress);//将弧度限制范围在 -PI ~ PI 之间
	//----------------------------------------------------------------------------------------------------------------
	extern	void		IntersectPlane(const float3& point, const float3& normal, const float3& from, const float3& to, float3& pickPoint);
	//----------------------------------------------------------------------------------------------------------------
	float2*				vec2Normalize( float2 *pOut, const float2 *pV );
	float2*				vec2Hermite( float2 *pOut, const float2 *pV1, const float2 *pT1,const float2 *pV2, const float2 *pT2, float s );// Hermite interpolation between position V1, tangent T1 (when s == 0// and position V2, tangent T2 (when s == 1).
	float2*				vec2CatmullRom( float2 *pOut, const float2 *pV0, const float2 *pV1,const float2 *pV2, const float2 *pV3, float s );// CatmullRom interpolation between V1 (when s == 0) and V2 (when s == 1)
	float2*				vec2BaryCentric( float2 *pOut, const float2 *pV1, const float2 *pV2,const float2 *pV3, float f, float g);// Barycentric coordinates.  V1 + f(V2-V1) + g(V3-V1)
	float4*				vec2Transform( float4 *pOut, const float2 *pV, const matrix4x4 *pM );// Transform (x, y, 0, 1) by matrix.
	float2*				vec2TransformCoord( float2 *pOut, const float2 *pV, const matrix4x4 *pM );// Transform (x, y, 0, 1) by matrix, project result back into w=1.
	float2*				vec2TransformNormal( float2 *pOut, const float2 *pV, const matrix4x4 *pM );// Transform (x, y, 0, 0) by matrix.
	float4*				vec2TransformArray( float4 *pOut, unsigned int OutStride, const float2 *pV, unsigned int VStride, const matrix4x4 *pM, unsigned int n);// Transform Array (x, y, 0, 1) by matrix.
	float2*				vec2TransformCoordArray( float2 *pOut, unsigned int OutStride, const float2 *pV, unsigned int VStride, const matrix4x4 *pM, unsigned int n );// Transform Array (x, y, 0, 1) by matrix, project result back into w=1.
	float2*				vec2TransformNormalArray( float2 *pOut, unsigned int OutStride, const float2 *pV, unsigned int VStride, const matrix4x4 *pM, unsigned int n );// Transform Array (x, y, 0, 0) by matrix.
	inline	float		vec2Length( const float2 *pV ){return pV->length();}
	inline	float		vec2LengthSq( const float2 *pV ){return pV->lengthSq();}
	inline	float		vec2Dot( const float2 *pV1,  const float2 *pV2 ){return pV1->dot(*pV2);}
	inline	float		vec2CCW( const float2 *pV1,  const float2 *pV2 ){return pV1->ccw(*pV2);}
	float2*				vec2Add( float2* pOut, const float2 *pV1,  const float2 *pV2 );
	float2*				vec2Subtract( float2* pOut, const float2 *pV1,  const float2 *pV2 );
	float2*				vec2Minimize( float2* pOut, const float2 *pV1,  const float2 *pV2 );
	float2*				vec2Maximize( float2* pOut, const float2 *pV1,  const float2 *pV2 );
	float2*				vec2Scale( float2* pOut, const float2 *pV1,  float s );
	float2*				vec2Lerp( float2* pOut, const float2 *pV1,  const float2 *pV2 , float s);
	//----------------------------------------------------------------------------------------------------------------
	//float3
	float3*				vec3Normalize( float3 *pOut, const float3 *pV );
	float3*				vec3Hermite( float3 *pOut, const float3 *pV1, const float3 *pT1,const float3 *pV2, const float3 *pT2, float s );// Hermite interpolation between position V1, tangent T1 (when s == 0)// and position V2, tangent T2 (when s == 1).
	float3*				vec3CatmullRom( float3 *pOut, const float3 *pV0, const float3 *pV1,const float3 *pV2, const float3 *pV3, float s );// CatmullRom interpolation between V1 (when s == 0) and V2 (when s == 1)
	float3*				vec3BaryCentric( float3 *pOut, const float3 *pV1, const float3 *pV2,const float3 *pV3, float f, float g);// Barycentric coordinates.  V1 + f(V2-V1) + g(V3-V1)
	float3*				vec3Transform( float3 *pOut, const float3 *pV, const matrix4x4 *pM );// Transform (x, y, z, 1) by matrix.
	float3*				vec3TransformCoord( float3 *pOut, const float3 *pV, const matrix4x4 *pM );// Transform (x, y, z, 1) by matrix, project result back into w=1.
	float3*				vec3TransformNormal( float3 *pOut, const float3 *pV, const matrix4x4 *pM );// Transform (x, y, z, 0) by matrix.  If you transforming a normal by a // non-affine matrix, the matrix you pass to this function should be the // transpose of the inverse of the matrix you would use to transform a coord.
	float4*				vec3TransformArray( float4 *pOut, unsigned int OutStride, const float3 *pV, unsigned int VStride, const matrix4x4 *pM, unsigned int n );// Transform Array (x, y, z, 1) by matrix. 
	float3*				vec3TransformCoordArray( float3 *pOut, unsigned int OutStride, const float3 *pV, unsigned int VStride, const matrix4x4 *pM, unsigned int n );// Transform Array (x, y, z, 1) by matrix, project result back into w=1.
	float3*				vec3TransformNormalArray( float3 *pOut, unsigned int OutStride, const float3 *pV, unsigned int VStride, const matrix4x4 *pM, unsigned int n );// Transform (x, y, z, 0) by matrix.  If you transforming a normal by a // non-affine matrix, the matrix you pass to this function should be the // transpose of the inverse of the matrix you would use to transform a coord.
	float				vec3Length( const float3 *pV );
	float				vec3LengthSq( const float3 *pV );
	float				vec3Dot( const float3 *pV1, const float3 *pV2 );
	float3*				vec3Cross( float3 *pOut, const float3 *pV1, const float3 *pV2 );
	float3*				vec3Add( float3 *pOut, const float3 *pV1, const float3 *pV2 );
	float3*				vec3Subtract( float3 *pOut, const float3 *pV1, const float3 *pV2 );
	float3*				vec3Minimize( float3 *pOut, const float3 *pV1, const float3 *pV2 );
	float3*				vec3Maximize( float3 *pOut, const float3 *pV1, const float3 *pV2 );
	float3*				vec3Scale( float3 *pOut, const float3 *pV, float s);
	float3*				vec3Lerp( float3 *pOut, const float3 *pV1, const float3 *pV2,float s );
	//----------------------------------------------------------------------------------------------------------------
	//float4
	float4*				vec4Cross( float4 *pOut, const float4 *pV1, const float4 *pV2,const float4 *pV3);// Cross-product in 4 dimensions.
	float4*				vec4Normalize( float4 *pOut, const float4 *pV );
	float4*				vec4Hermite( float4 *pOut, const float4 *pV1, const float4 *pT1,const float4 *pV2, const float4 *pT2, float s );// Hermite interpolation between position V1, tangent T1 (when s == 0)// and position V2, tangent T2 (when s == 1).
	float4*				vec4CatmullRom( float4 *pOut, const float4 *pV0, const float4 *pV1,const float4 *pV2, const float4 *pV3, float s );// CatmullRom interpolation between V1 (when s == 0) and V2 (when s == 1)
	float4*				vec4BaryCentric( float4 *pOut, const float4 *pV1, const float4 *pV2,const float4 *pV3, float f, float g);// Barycentric coordinates.  V1 + f(V2-V1) + g(V3-V1)
	float4*				vec4Transform( float4 *pOut, const float4 *pV, const matrix4x4 *pM );// Transform vector by matrix.
	float4*				vec4TransformArray( float4 *pOut, unsigned int OutStride, const float4 *pV, unsigned int VStride, const matrix4x4 *pM, unsigned int n );// Transform vector array by matrix.
	//----------------------------------------------------------------------------------------------------------------
	//quaternion
	void				qtnFromDirection(quaternion& qtn,float3& vFrom, float3 &vTo);
	void				qtnToAxisAngle( const quaternion *pQ, float3 *pAxis, float *pAngle );
	quaternion* 		qtnRotationMatrix( quaternion *pOut, const matrix4x4 *pM);// Build a quaternion from a rotation matrix.
	quaternion* 		qtnRotationAxis( quaternion *pOut, const float3 *pV, float Angle );// Rotation about arbitrary axis.
	quaternion* 		qtnRotationYawPitchRoll( quaternion *pOut, float Yaw, float Pitch, float Roll );// Yaw around the Y axis, a pitch around the X axis,// and a roll around the Z axis.
	quaternion* 		qtnMultiply( quaternion *pOut, const quaternion *pQ1,const quaternion *pQ2 );// Quaternion multiplication.  The result represents the rotation Q2// followed by the rotation Q1.  (Out = Q2 * Q1)
	quaternion* 		qtnNormalize( quaternion *pOut, const quaternion *pQ );// Conjugate and re-norm
	quaternion* 		qtnInverse( quaternion *pOut, const quaternion *pQ );// Expects unit quaternions.// if q = (cos(theta), sin(theta) * v); ln(q) = (0, theta * v)
	quaternion* 		qtnLn( quaternion *pOut, const quaternion *pQ );
	quaternion* 		qtnExp( quaternion *pOut, const quaternion *pQ );// Expects pure quaternions. (w == 0)  w is ignored in calculation.// if q = (0, theta * v); exp(q) = (cos(theta), sin(theta) * v)
	quaternion* 		qtnSlerp( quaternion *pOut, const quaternion *pQ1,const quaternion *pQ2, float t );// Spherical linear interpolation between Q1 (t == 0) and Q2 (t == 1).// Expects unit quaternions.
	quaternion* 		qtnSquad( quaternion *pOut, const quaternion *pQ1,const quaternion *pA, const quaternion *pB,const quaternion *pC, float t );// Spherical quadrangle interpolation.// Slerp(Slerp(Q1, C, t), Slerp(A, B, t), 2t(1-t))
	void				qtnSquadSetup( quaternion *pAOut, quaternion *pBOut, quaternion *pCOut,const quaternion *pQ0, const quaternion *pQ1, const quaternion *pQ2, const quaternion *pQ3 );// Setup control points for spherical quadrangle interpolation// from Q1 to Q2.  The control points are chosen in such a way // to ensure the continuity of tangents with adjacent segments.
	quaternion*			qtnBaryCentric( quaternion *pOut, const quaternion *pQ1,const quaternion *pQ2, const quaternion *pQ3,float f, float g );// Barycentric interpolation.// Slerp(Slerp(Q1, Q2, f+g), Slerp(Q1, Q3, f+g), g/(f+g))
	//----------------------------------------------------------------------------------------------------------------
	//Plane
	Plane*				planeNormalize( Plane *pOut, const Plane *pP);//点法创建平面
	float3*				planeIntersectLine( float3 *pOut, const Plane *pP, const float3 *pV1,const float3 *pV2);// Find the intersection between a plane and a line.  If the line is// parallel to the plane, NULL is returned.
	Plane*				planeFromPointNormal( Plane *pOut, const float3 *pPoint, const float3 *pNormal);//根据点和法线创建平面
	Plane*				planeFromPoints( Plane *pOut, const float3 *pV1, const float3 *pV2,const float3 *pV3);//根据三点创建平面
	Plane*				planeTransform( Plane *pOut, const Plane *pP, const matrix4x4 *pM );// Transform a plane by a matrix.  The vector (a,b,c) must be normal.// M should be the inverse transpose of the transformation desired.
	Plane*				planeTransformArray( Plane *pOut, unsigned int OutStride, const Plane *pP, unsigned int PStride, const matrix4x4 *pM, unsigned int n );// Transform an array of planes by a matrix.  The vectors (a,b,c) must be normal.// M should be the inverse transpose of the transformation desired.
	float				planeDot(const Plane* p1, const float4* p2);// ax + by + cz + dw平面方程与4D向量的点乘
	float				planeDotCoord(const Plane* p1, const float3* p2);// ax + by + cz + d平面与3D向量的点乘,w=1
	float				planeDotNormal(const Plane* p1, const float3* p2);// ax + by + cz平面与3D向量的点乘,w=0
	Plane*				planeScale(Plane* pOut, const Plane* p1, float s);
	//----------------------------------------------------------------------------------------------------------------
	//color4
	color4*				colorAdjustSaturation(color4 *pOut, const color4 *pC, float s);// Interpolate r,g,b between desaturated color and color.// DesaturatedColor + s(Color - DesaturatedColor)
	color4*				colorAdjustContrast(color4 *pOut, const color4 *pC, float c);// Interpolate r,g,b between 50% grey and color.  Grey + s(Color - Grey)
	//----------------------------------------------------------------------------------------------------------------
	//matrix4x4
	matrix4x4*			matrixLerp(matrix4x4* pOut, const matrix4x4* pFrom, const matrix4x4* pTo, float fLerp);
	matrix4x4*			matrixTransform(matrix4x4* bzOut, const float3* pos, const quaternion* rot, const float3* scale);	//转变,根据参数
	void				getRSP(quaternion& outQtn,float3& outScale,float3& outPosition,const matrix4x4& matIn_);
	void				getMatrixQuaternion(quaternion& _out,const matrix4x4& _matIn);
	void				getMatrixScale(float3& outScale,const matrix4x4& _matIn);
	inline	void		matrixIdentity(matrix4x4* m){m->identity();}
	//
	matrix4x4* 			matrixRotationXYZ( matrix4x4 *pOut, const float3& xyz);
	float				matrixDeterminant( const matrix4x4 *pM );
	BOOL				matrixDecompose( float3 *pOutScale, quaternion *pOutRotation, float3 *pOutTranslation, const matrix4x4 *pM );
	matrix4x4*			matrixTranspose( matrix4x4 *pOut, const matrix4x4 *pM );
	matrix4x4* 			matrixMultiply( matrix4x4 *pOut, const matrix4x4 *pM1, const matrix4x4 *pM2 );// Matrix multiplication.  The result represents the transformation M2// followed by the transformation M1.  (Out = M1 * M2)
	matrix4x4* 			matrixMultiplyTranspose( matrix4x4 *pOut, const matrix4x4 *pM1, const matrix4x4 *pM2 );// Matrix multiplication, followed by a transpose. (Out = T(M1 * M2))
	matrix4x4* 			matrixInverse( matrix4x4 *pOut, float *pDeterminant, const matrix4x4 *pM );// Calculate inverse of matrix.  Inversion my fail, in which case NULL will// be returned.  The determinant of pM is also returned it pfDeterminant// is non-NULL.
	matrix4x4* 			matrixScaling( matrix4x4 *pOut, float sx, float sy, float sz );// Build a matrix which scales by (sx, sy, sz)
	matrix4x4* 			matrixTranslation( matrix4x4 *pOut, float x, float y, float z );// Build a matrix which translates by (x, y, z)
	matrix4x4* 			matrixRotationX( matrix4x4 *pOut, float Angle );// Build a matrix which rotates around the X axis
	matrix4x4* 			matrixRotationY( matrix4x4 *pOut, float Angle );// Build a matrix which rotates around the Y axis
	matrix4x4* 			matrixRotationZ( matrix4x4 *pOut, float Angle );// Build a matrix which rotates around the Z axis
	matrix4x4* 			matrixRotationAxis( matrix4x4 *pOut, const float3 *pV, float Angle );// Build a matrix which rotates around an arbitrary axis
	matrix4x4* 			matrixRotationQuaternion( matrix4x4 *pOut, const quaternion *pQ);// Build a matrix from a quaternion
	matrix4x4* 			matrixRotationYawPitchRoll( matrix4x4 *pOut, float Yaw, float Pitch, float Roll );// Yaw around the Y axis, a pitch around the X axis,// and a roll around the Z axis.
	matrix4x4* 			matrixTransformation( matrix4x4 *pOut, const float3 *pScalingCenter,const quaternion *pScalingRotation, const float3 *pScaling,const float3 *pRotationCenter, const quaternion *pRotation,const float3 *pTranslation);// Build transformation matrix.  NULL arguments are treated as identity.// Mout = Msc-1 * Msr-1 * Ms * Msr * Msc * Mrc-1 * Mr * Mrc * Mt
	matrix4x4* 			matrixTransformation2D( matrix4x4 *pOut, const float2* pScalingCenter, float ScalingRotation, const float2* pScaling, const float2* pRotationCenter, float Rotation, const float2* pTranslation);// Build 2D transformation matrix in XY plane.  NULL arguments are treated as identity.// Mout = Msc-1 * Msr-1 * Ms * Msr * Msc * Mrc-1 * Mr * Mrc * Mt
	matrix4x4* 			matrixAffineTransformation( matrix4x4 *pOut, float Scaling, const float3 *pRotationCenter,const quaternion *pRotation, const float3 *pTranslation);// Build affine transformation matrix.  NULL arguments are treated as identity.// Mout = Ms * Mrc-1 * Mr * Mrc * Mt
	matrix4x4* 			matrixAffineTransformation2D( matrix4x4 *pOut, float Scaling, const float2* pRotationCenter, float Rotation, const float2* pTranslation);// Build 2D affine transformation matrix in XY plane.  NULL arguments are treated as identity.// Mout = Ms * Mrc-1 * Mr * Mrc * Mt
	matrix4x4* 			matrixLookAtRH( matrix4x4 *pOut, const float3 *pEye, const float3 *pAt,const float3 *pUp );// Build a lookat matrix. (right-handed)
	matrix4x4* 			matrixLookAtLH( matrix4x4 *pOut, const float3 *pEye, const float3 *pAt,const float3 *pUp );// Build a lookat matrix. (left-handed)
	matrix4x4* 			matrixPerspectiveRH( matrix4x4 *pOut, float w, float h, float zn, float zf );// Build a perspective projection matrix. (right-handed)
	matrix4x4* 			matrixPerspectiveLH( matrix4x4 *pOut, float w, float h, float zn, float zf );// Build a perspective projection matrix. (left-handed)
	matrix4x4* 			matrixPerspectiveFovRH( matrix4x4 *pOut, float fovy, float Aspect, float zn, float zf );// Build a perspective projection matrix. (right-handed)
	matrix4x4* 			matrixPerspectiveFovLH( matrix4x4 *pOut, float fovy, float Aspect, float zn, float zf );// Build a perspective projection matrix. (left-handed)
	matrix4x4* 			matrixPerspectiveOffCenterRH( matrix4x4 *pOut, float l, float r, float b, float t, float zn,float zf );// Build a perspective projection matrix. (right-handed)
	matrix4x4* 			matrixPerspectiveOffCenterLH( matrix4x4 *pOut, float l, float r, float b, float t, float zn,float zf );// Build a perspective projection matrix. (left-handed)
	matrix4x4* 			matrixOrthoRH( matrix4x4 *pOut, float w, float h, float zn, float zf );// Build an ortho projection matrix. (right-handed)
	matrix4x4* 			matrixOrthoLH( matrix4x4 *pOut, float w, float h, float zn, float zf );// Build an ortho projection matrix. (left-handed)
	matrix4x4* 			matrixOrthoOffCenterRH( matrix4x4 *pOut, float l, float r, float b, float t, float zn,float zf );// Build an ortho projection matrix. (right-handed)
	matrix4x4* 			matrixOrthoOffCenterLH( matrix4x4 *pOut, float l, float r, float b, float t, float zn,float zf );// Build an ortho projection matrix. (left-handed)
	matrix4x4* 			matrixShadow( matrix4x4 *pOut, const float4 *pLight,const Plane *pPlane );// Build a matrix which flattens geometry into a plane, as if casting a shadow from a light.
	matrix4x4* 			matrixReflect( matrix4x4 *pOut, const Plane *pPlane );// Build a matrix which reflects the coordinate system about a plane
	void				getMatrixScale(float3& outScale,const matrix4x4& matIn);
};

#endif
