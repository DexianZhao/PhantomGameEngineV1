#ifndef __API__LUA__H__
#define __API__LUA_H__
#include "PhantomAPI.h"
namespace Phantom{
template <int Tindex> class lua_GameTable;class Config;class lua_AABox;
class lua_CameraData;
class lua_color4;
class lua_float2;
class lua_float3;
class lua_float4;
class lua_floatPtr;
class lua_GameID;
class lua_matrix4x4;
class lua_Object;
class lua_PickPoint;
class lua_Pixel;
class lua_PixelColor;
class lua_PixelUV;
class lua_Plane;
class lua_quaternion;
class lua_Rect;
class lua_sensordata;
class lua_short2;
class lua_short4;
class lua_Time;
class lua_Triangle;
class lua_ubyte4;
class lua_Vertex;
class lua_DataArray_s;
class lua_DataControl_s;
class lua_FindWay_s;
class lua_GameObject_s;
class lua_Map3D_s;
class lua_Map3DEnv_s;
class lua_Map3DItem_s;
class lua_Map3DPoint_s;
class lua_Map3DSkill_s;
class lua_Mesh_s;
class lua_MeshAction_s;
class lua_MeshMaterial_s;
class lua_SubMesh_s;
class lua_Table_s;
class lua_Texture_s;
class lua_UIControl_s;
class lua_UIDialog_s;
class lua_UIDrawMgr_s;
class lua_UIElement_s;
class lua_UIGroup_s;
class lua_Camera;
class lua_GameMgr;
class lua_MyGlobal;
class lua_System;
class lua_Global;
//(三维盒子)  说明：
class lua_AABox: public AABox
{
public:
lua_AABox(){memset(this, 0, sizeof(lua_AABox));}
lua_AABox(float3 __min,float3 __max){_min= __min;_max= __max;}
lua_AABox(const AABox& v){memcpy(this, &v, sizeof(lua_AABox));}
~lua_AABox(){}
inline lua_AABox& operator = (const AABox& v){memcpy(this, &v, sizeof(AABox));return *this;}
inline operator AABox* ()					{return this;}
};

//(镜头数据)  说明：
class lua_CameraData: public CameraData
{
public:
lua_CameraData(){memset(this, 0, sizeof(lua_CameraData));}
lua_CameraData(const CameraData& v){memcpy(this, &v, sizeof(lua_CameraData));}
~lua_CameraData(){}
inline lua_CameraData& operator = (const CameraData& v){memcpy(this, &v, sizeof(CameraData));return *this;}
inline operator CameraData* ()					{return this;}
};

//(颜色)  说明：
class lua_color4: public color4
{
public:
lua_color4(){memset(this, 0, sizeof(lua_color4));}
lua_color4(float _r,float _g,float _b,float _a){r= _r;g= _g;b= _b;a= _a;}
lua_color4(const color4& v){memcpy(this, &v, sizeof(lua_color4));}
~lua_color4(){}
inline lua_color4& operator = (const color4& v){memcpy(this, &v, sizeof(color4));return *this;}
inline operator color4* ()					{return this;}
lua_color4 operator + (const lua_color4& val){return lua_color4(color4::operator + (val));}
lua_color4 operator - (const lua_color4& val){return lua_color4(color4::operator - (val));}
lua_color4 operator * (const lua_color4& val){return lua_color4(color4::operator * (val));}
lua_color4 operator / (const lua_color4& val){return lua_color4(color4::operator / (val));}
bool operator == (const lua_color4& val){return (color4::operator == (val));}
void			fromrgb(INT r,	INT g,	INT b);
void			fromrgba(INT rgb,	float fAlpha);
INT	Getrgba();
INT	rgb();
void			set(float r,	float g,	float b,	float a);
void			Setrgba(INT bgra);
void			setup(float r,	float g,	float b,	float a);
};

//(二维向量)  说明：
class lua_float2: public float2
{
public:
lua_float2(){memset(this, 0, sizeof(lua_float2));}
lua_float2(float _x,float _y){x= _x;y= _y;}
lua_float2(const float2& v){memcpy(this, &v, sizeof(lua_float2));}
~lua_float2(){}
inline lua_float2& operator = (const float2& v){memcpy(this, &v, sizeof(float2));return *this;}
inline operator float2* ()					{return this;}
lua_float2 operator + (const lua_float2& val){return lua_float2(float2::operator + (val));}
lua_float2 operator - (const lua_float2& val){return lua_float2(float2::operator - (val));}
lua_float2 operator * (const lua_float2& val){return lua_float2(float2::operator * (val));}
lua_float2 operator / (const lua_float2& val){return lua_float2(float2::operator / (val));}
bool operator == (const lua_float2& val){return (float2::operator == (val));}
void			barycentric(lua_float2& pV1,	lua_float2& pV2,	lua_float2& pV3,	float f,	float g);
void			catmullrom(lua_float2& pV0,	lua_float2& pV1,	lua_float2& pV2,	lua_float2& pV3,	float s);
float	cross(lua_float2& v2);
float	dot(lua_float2& v2);
void			hermite(lua_float2& pV1,	lua_float2& pT1,	lua_float2& pV2,	lua_float2& pT2,	float s);
float	length();
void			lerp(lua_float2& v1,	lua_float2& v2,	float fLerp);
void			normalize();
void			setup(float x,	float y);
void			transformcoord(lua_float2& pV,	lua_matrix4x4& pM);
void			transformnormal(lua_float2& pV,	lua_matrix4x4& pM);
};

//(三维向量)  说明：
class lua_float3: public float3
{
public:
lua_float3(){memset(this, 0, sizeof(lua_float3));}
lua_float3(float _x,float _y,float _z){x= _x;y= _y;z= _z;}
lua_float3(const float3& v){memcpy(this, &v, sizeof(lua_float3));}
~lua_float3(){}
inline lua_float3& operator = (const float3& v){memcpy(this, &v, sizeof(float3));return *this;}
inline operator float3* ()					{return this;}
lua_float3 operator + (const lua_float3& val){return lua_float3(float3::operator + (val));}
lua_float3 operator - (const lua_float3& val){return lua_float3(float3::operator - (val));}
lua_float3 operator * (const lua_float3& val){return lua_float3(float3::operator * (val));}
lua_float3 operator / (const lua_float3& val){return lua_float3(float3::operator / (val));}
bool operator == (const lua_float3& val){return (float3::operator == (val));}
void			barycentric(lua_float3& pV1,	lua_float3& pV2,	lua_float3& pV3,	float f,	float g);
void			catmullrom(lua_float3& pV0,	lua_float3& pV1,	lua_float3& pV2,	lua_float3& pV3,	float s);
void			cross(lua_float3& v1,	lua_float3& v2);
float	dot(lua_float3& v2);
float	getrotate_z();
void			hermite(lua_float3& pV1,	lua_float3& pT1,	lua_float3& pV2,	lua_float3& pT2,	float s);
float	length();
void			lerp(lua_float3& v1,	lua_float3& v2,	float fLerp);
void			normalize();
void			rotateX(lua_float3& v2,	float fDegress);
void			rotateY(lua_float3& v2,	float fDegress);
void			rotateZ(lua_float3& v2,	float fDegress);
void			setup(float x,	float y,	float z);
void			transform(lua_float3& v2,	lua_matrix4x4& m);
void			transform3x3(lua_float3& v2,	lua_matrix4x4& m);
void			transformcoord(lua_float3& pV,	lua_matrix4x4& pM);
void			transformnormal(lua_float3& pV,	lua_matrix4x4& pM);
};

//(四维向量)  说明：
class lua_float4: public float4
{
public:
lua_float4(){memset(this, 0, sizeof(lua_float4));}
lua_float4(float _x,float _y,float _z,float _w){x= _x;y= _y;z= _z;w= _w;}
lua_float4(const float4& v){memcpy(this, &v, sizeof(lua_float4));}
~lua_float4(){}
inline lua_float4& operator = (const float4& v){memcpy(this, &v, sizeof(float4));return *this;}
inline operator float4* ()					{return this;}
lua_float4 operator + (const lua_float4& val){return lua_float4(float4::operator + (val));}
lua_float4 operator - (const lua_float4& val){return lua_float4(float4::operator - (val));}
lua_float4 operator * (const lua_float4& val){return lua_float4(float4::operator * (val));}
lua_float4 operator / (const lua_float4& val){return lua_float4(float4::operator / (val));}
bool operator == (const lua_float4& val){return (float4::operator == (val));}
void			barycentric(lua_float4& pV1,	lua_float4& pV2,	lua_float4& pV3,	float f,	float g);
void			catmullrom(lua_float4& pV0,	lua_float4& pV1,	lua_float4& pV2,	lua_float4& pV3,	float s);
void			copy(lua_float4& v1);
float	dot(lua_float4& v2);
void			hermite(lua_float4& pV1,	lua_float4& pT1,	lua_float4& pV2,	lua_float4& pT2,	float s);
float	length();
void			lerp(lua_float4& v1,	lua_float4& v2,	float fLerp);
void			setup(float x,	float y,	float z,	float w);
void			transform(lua_float3& pV,	lua_matrix4x4& pM);
};

//(小数型数据)  说明：
class lua_floatPtr: public floatPtr
{
public:
lua_floatPtr(){memset(this, 0, sizeof(lua_floatPtr));}
lua_floatPtr(float _value){value= _value;}
lua_floatPtr(const floatPtr& v){memcpy(this, &v, sizeof(lua_floatPtr));}
~lua_floatPtr(){}
inline lua_floatPtr& operator = (const floatPtr& v){memcpy(this, &v, sizeof(floatPtr));return *this;}
inline operator floatPtr* ()					{return this;}
};

//(游戏对象ID)  说明：
class lua_GameID: public GameID
{
public:
lua_GameID(){memset(this, 0, sizeof(lua_GameID));}
lua_GameID(short _id,short _index){id= _id;index= _index;}
lua_GameID(const GameID& v){memcpy(this, &v, sizeof(lua_GameID));}
~lua_GameID(){}
inline lua_GameID& operator = (const GameID& v){memcpy(this, &v, sizeof(GameID));return *this;}
inline operator GameID* ()					{return this;}
};

//(矩阵)  说明：
class lua_matrix4x4: public matrix4x4
{
public:
lua_matrix4x4(){memset(this, 0, sizeof(lua_matrix4x4));}
lua_matrix4x4(const matrix4x4& v){memcpy(this, &v, sizeof(lua_matrix4x4));}
~lua_matrix4x4(){}
inline lua_matrix4x4& operator = (const matrix4x4& v){memcpy(this, &v, sizeof(matrix4x4));return *this;}
inline operator matrix4x4* ()					{return this;}
lua_matrix4x4 operator + (const lua_matrix4x4& val){return lua_matrix4x4(matrix4x4::operator + (val));}
lua_matrix4x4 operator - (const lua_matrix4x4& val){return lua_matrix4x4(matrix4x4::operator - (val));}
lua_matrix4x4 operator * (const lua_matrix4x4& val){return lua_matrix4x4(matrix4x4::operator * (val));}
lua_matrix4x4 operator / (const lua_matrix4x4& val){return lua_matrix4x4(matrix4x4::operator / (val));}
bool operator == (const lua_matrix4x4& val){return (matrix4x4::operator == (val));}
void			affineTransformation(float Scaling,	lua_float3& pRotationCenter,	lua_quaternion& pRotation,	lua_float3& pTranslation);
void			affineTransformation2d(float Scaling,	lua_float2& pRotationCenter,	float Rotation,	lua_float2& pTranslation);
bool	decompose(lua_float3& pOutScale,	lua_quaternion& pOutRotation,	lua_float3& pOutTranslation);
lua_float3	Getpos();
void			identity();
void			inverse(lua_matrix4x4& pM);
void			inverse2(lua_float3& pDeterminant,	lua_matrix4x4& pM);
void			lerp(lua_matrix4x4& v1,	lua_matrix4x4& v2,	float fLerp);
void			lookat_lh(lua_float3& pEye,	lua_float3& pAt,	lua_float3& pUp);
void			lookat_rh(lua_float3& pEye,	lua_float3& pAt,	lua_float3& pUp);
void			multiply_transpose(lua_matrix4x4& pM1,	lua_matrix4x4& pM2);
void			orth_offcenter_lh(float l,	float r,	float b,	float t,	lua_float2& znearfar);
void			ortho_lh(float w,	float h,	float zn,	float zf);
void			ortho_offcenter_rh(float l,	float r,	float b,	float t,	lua_float2& znearfar);
void			ortho_rh(float w,	float h,	float zn,	float zf);
void			perspective_fovrh(float fovy,	float Aspect,	float zn,	float zf);
void			perspective_lv(float w,	float h,	float zn,	float zf);
void			perspective_offcenter_lh(float l,	float r,	float b,	float t,	lua_float2& znearfar);
void			perspective_offcenter_rh(float l,	float r,	float b,	float t,	lua_float2& znearfar);
void			perspective_rh(float w,	float h,	float zn,	float zf);
void			perspectivefov_lh(float fovy,	float Aspect,	float zn,	float zf);
void			reflect(lua_Plane& pPlane);
void			rotation_yawpitchroll(float Yaw,	float Pitch,	float Roll);
void			rotationaxis(lua_float3& pV,	float fDegress);
void			rotationquaternion(lua_quaternion& pQ);
void			rotationx(float fDegress);
void			rotationy(float fDegress);
void			rotationz(float fDegress);
void			scaling(float sx,	float sy,	float sz);
void			Setpos(lua_float3& v);
void			transform(lua_float3& pos,	lua_quaternion& rot,	lua_float3& scale);
void			translation(float x,	float y,	float z);
void			transpose(lua_matrix4x4& pOut);
};

//(对象指针)  说明：
class lua_Object: public Object
{
public:
lua_Object(){memset(this, 0, sizeof(lua_Object));}
lua_Object(short _s1,short _s2,int _i1,int _i2){s1= _s1;s2= _s2;i1= _i1;i2= _i2;}
lua_Object(const Object& v){memcpy(this, &v, sizeof(lua_Object));}
~lua_Object(){}
inline lua_Object& operator = (const Object& v){memcpy(this, &v, sizeof(Object));return *this;}
inline operator Object* ()					{return this;}
};

//(碰撞数据)  说明：
class lua_PickPoint: public PickPoint
{
public:
lua_PickPoint(){memset(this, 0, sizeof(lua_PickPoint));}
lua_PickPoint(float3 _pos,float3 _normal,float _t){pos= _pos;normal= _normal;t= _t;}
lua_PickPoint(const PickPoint& v){memcpy(this, &v, sizeof(lua_PickPoint));}
~lua_PickPoint(){}
inline lua_PickPoint& operator = (const PickPoint& v){memcpy(this, &v, sizeof(PickPoint));return *this;}
inline operator PickPoint* ()					{return this;}
};

//(屏幕坐标数据)  说明：
class lua_Pixel: public Pixel
{
public:
lua_Pixel(){memset(this, 0, sizeof(lua_Pixel));}
lua_Pixel(int _x,int _y){x= _x;y= _y;}
lua_Pixel(const Pixel& v){memcpy(this, &v, sizeof(lua_Pixel));}
~lua_Pixel(){}
inline lua_Pixel& operator = (const Pixel& v){memcpy(this, &v, sizeof(Pixel));return *this;}
inline operator Pixel* ()					{return this;}
lua_Pixel operator + (const lua_Pixel& val){return lua_Pixel(Pixel::operator + (val));}
lua_Pixel operator - (const lua_Pixel& val){return lua_Pixel(Pixel::operator - (val));}
lua_Pixel operator * (const lua_Pixel& val){return lua_Pixel(Pixel::operator * (val));}
lua_Pixel operator / (const lua_Pixel& val){return lua_Pixel(Pixel::operator / (val));}
bool operator == (const lua_Pixel& val){return (Pixel::operator == (val));}
};

//(屏幕坐标颜色)  说明：
class lua_PixelColor: public PixelColor
{
public:
lua_PixelColor(){memset(this, 0, sizeof(lua_PixelColor));}
lua_PixelColor(int _x,int _y,color4 _color){x= _x;y= _y;color= _color;}
lua_PixelColor(const PixelColor& v){memcpy(this, &v, sizeof(lua_PixelColor));}
~lua_PixelColor(){}
inline lua_PixelColor& operator = (const PixelColor& v){memcpy(this, &v, sizeof(PixelColor));return *this;}
inline operator PixelColor* ()					{return this;}
lua_PixelColor operator + (const lua_PixelColor& val){return lua_PixelColor(PixelColor::operator + (val));}
lua_PixelColor operator - (const lua_PixelColor& val){return lua_PixelColor(PixelColor::operator - (val));}
lua_PixelColor operator * (const lua_PixelColor& val){return lua_PixelColor(PixelColor::operator * (val));}
lua_PixelColor operator / (const lua_PixelColor& val){return lua_PixelColor(PixelColor::operator / (val));}
bool operator == (const lua_PixelColor& val){return (PixelColor::operator == (val));}
};

//(屏幕坐标数据UV)  说明：
class lua_PixelUV: public PixelUV
{
public:
lua_PixelUV(){memset(this, 0, sizeof(lua_PixelUV));}
lua_PixelUV(const PixelUV& v){memcpy(this, &v, sizeof(lua_PixelUV));}
~lua_PixelUV(){}
inline lua_PixelUV& operator = (const PixelUV& v){memcpy(this, &v, sizeof(PixelUV));return *this;}
inline operator PixelUV* ()					{return this;}
};

//(平面)  说明：
class lua_Plane: public Plane
{
public:
lua_Plane(){memset(this, 0, sizeof(lua_Plane));}
lua_Plane(float _a,float _b,float _c,float _d){a= _a;b= _b;c= _c;d= _d;}
lua_Plane(const Plane& v){memcpy(this, &v, sizeof(lua_Plane));}
~lua_Plane(){}
inline lua_Plane& operator = (const Plane& v){memcpy(this, &v, sizeof(Plane));return *this;}
inline operator Plane* ()					{return this;}
lua_Plane operator + (const lua_Plane& val){return lua_Plane(Plane::operator + (val));}
lua_Plane operator - (const lua_Plane& val){return lua_Plane(Plane::operator - (val));}
lua_Plane operator * (const lua_Plane& val){return lua_Plane(Plane::operator * (val));}
lua_Plane operator / (const lua_Plane& val){return lua_Plane(Plane::operator / (val));}
bool operator == (const lua_Plane& val){return (Plane::operator == (val));}
float	dot(lua_float4& p2);
float	dotcoord(lua_float3& p2);
float	dotnormal(lua_float3& p2);
void			from_pointnormal(lua_float3& pPoint,	lua_float3& pNormal);
void			from_points(lua_float3& pV1,	lua_float3& pV2,	lua_float3& pV3);
void			intersectline(lua_float3& r,	lua_float3& pV1,	lua_float3& pV2);
void			normalize(lua_Plane& pP);
void			scale(lua_Plane& p1,	float s);
void			transform(lua_Plane& pP,	lua_matrix4x4& pM);
};

//(四元数)  说明：
class lua_quaternion: public quaternion
{
public:
lua_quaternion(){memset(this, 0, sizeof(lua_quaternion));}
lua_quaternion(float _x,float _y,float _z,float _w){x= _x;y= _y;z= _z;w= _w;}
lua_quaternion(const quaternion& v){memcpy(this, &v, sizeof(lua_quaternion));}
~lua_quaternion(){}
inline lua_quaternion& operator = (const quaternion& v){memcpy(this, &v, sizeof(quaternion));return *this;}
inline operator quaternion* ()					{return this;}
lua_quaternion operator + (const lua_quaternion& val){return lua_quaternion(quaternion::operator + (val));}
lua_quaternion operator - (const lua_quaternion& val){return lua_quaternion(quaternion::operator - (val));}
lua_quaternion operator * (const lua_quaternion& val){return lua_quaternion(quaternion::operator * (val));}
lua_quaternion operator / (const lua_quaternion& val){return lua_quaternion(quaternion::operator / (val));}
bool operator == (const lua_quaternion& val){return (quaternion::operator == (val));}
void			barycentric(lua_quaternion& pQ1,	lua_quaternion& pQ2,	lua_quaternion& pQ3,	float f,	float g);
void			exp(lua_quaternion& pQ);
void			inverse(lua_quaternion& pQ);
void			ln(lua_quaternion& pQ);
void			multiply(lua_quaternion& pQ1,	lua_quaternion& pQ2);
void			normalize(lua_quaternion& pQ);
void			rotation_yawpitchroll(float Yaw,	float Pitch,	float Roll);
void			rotationaxis(lua_float3& pV,	float fDegress);
void			rotationmatrix(lua_matrix4x4& pM);
void			slerp(lua_quaternion& pQ1,	lua_quaternion& pQ2,	float t);
void			squad(lua_quaternion& pQ1,	lua_quaternion& pA,	lua_quaternion& pB,	lua_quaternion& pC,	float t);
};

//(矩形数据)  说明：
class lua_Rect: public Rect
{
public:
lua_Rect(){memset(this, 0, sizeof(lua_Rect));}
lua_Rect(int _left,int _top,int _right,int _bottom){left= _left;top= _top;right= _right;bottom= _bottom;}
lua_Rect(const Rect& v){memcpy(this, &v, sizeof(lua_Rect));}
~lua_Rect(){}
inline lua_Rect& operator = (const Rect& v){memcpy(this, &v, sizeof(Rect));return *this;}
inline operator Rect* ()					{return this;}
lua_Rect operator + (const lua_Rect& val){return lua_Rect(Rect::operator + (val));}
lua_Rect operator - (const lua_Rect& val){return lua_Rect(Rect::operator - (val));}
lua_Rect operator * (const lua_Rect& val){return lua_Rect(Rect::operator * (val));}
lua_Rect operator / (const lua_Rect& val){return lua_Rect(Rect::operator / (val));}
bool operator == (const lua_Rect& val){return (Rect::operator == (val));}
};

//(传感器数据)  说明：
class lua_sensordata: public sensordata
{
public:
lua_sensordata(){memset(this, 0, sizeof(lua_sensordata));}
lua_sensordata(const sensordata& v){memcpy(this, &v, sizeof(lua_sensordata));}
~lua_sensordata(){}
inline lua_sensordata& operator = (const sensordata& v){memcpy(this, &v, sizeof(sensordata));return *this;}
inline operator sensordata* ()					{return this;}
};

//(短整数2)  说明：
class lua_short2: public short2
{
public:
lua_short2(){memset(this, 0, sizeof(lua_short2));}
lua_short2(short _x,short _y){x= _x;y= _y;}
lua_short2(const short2& v){memcpy(this, &v, sizeof(lua_short2));}
~lua_short2(){}
inline lua_short2& operator = (const short2& v){memcpy(this, &v, sizeof(short2));return *this;}
inline operator short2* ()					{return this;}
};

//(短整数4)  说明：
class lua_short4: public short4
{
public:
lua_short4(){memset(this, 0, sizeof(lua_short4));}
lua_short4(short _x,short _y,short _z,short _w){x= _x;y= _y;z= _z;w= _w;}
lua_short4(const short4& v){memcpy(this, &v, sizeof(lua_short4));}
~lua_short4(){}
inline lua_short4& operator = (const short4& v){memcpy(this, &v, sizeof(short4));return *this;}
inline operator short4* ()					{return this;}
};

//(时间)  说明：
class lua_Time: public Time
{
public:
lua_Time(){memset(this, 0, sizeof(lua_Time));}
lua_Time(const Time& v){memcpy(this, &v, sizeof(lua_Time));}
~lua_Time(){}
inline lua_Time& operator = (const Time& v){memcpy(this, &v, sizeof(Time));return *this;}
inline operator Time* ()					{return this;}
};

//(三角形索引)  说明：
class lua_Triangle: public Triangle
{
public:
lua_Triangle(){memset(this, 0, sizeof(lua_Triangle));}
lua_Triangle(short _v1,short _v2,short _v3){v1= _v1;v2= _v2;v3= _v3;}
lua_Triangle(const Triangle& v){memcpy(this, &v, sizeof(lua_Triangle));}
~lua_Triangle(){}
inline lua_Triangle& operator = (const Triangle& v){memcpy(this, &v, sizeof(Triangle));return *this;}
inline operator Triangle* ()					{return this;}
};

//(字节4)  说明：
class lua_ubyte4: public ubyte4
{
public:
lua_ubyte4(){memset(this, 0, sizeof(lua_ubyte4));}
lua_ubyte4(char _x,char _y,char _z,char _w){x= _x;y= _y;z= _z;w= _w;}
lua_ubyte4(const ubyte4& v){memcpy(this, &v, sizeof(lua_ubyte4));}
~lua_ubyte4(){}
inline lua_ubyte4& operator = (const ubyte4& v){memcpy(this, &v, sizeof(ubyte4));return *this;}
inline operator ubyte4* ()					{return this;}
};

//(顶点数据)  说明：
class lua_Vertex: public Vertex
{
public:
lua_Vertex(){memset(this, 0, sizeof(lua_Vertex));}
lua_Vertex(float3 _pos,float3 _normal,float2 _uv1,float2 _uv2){pos= _pos;normal= _normal;uv1= _uv1;uv2= _uv2;}
lua_Vertex(const Vertex& v){memcpy(this, &v, sizeof(lua_Vertex));}
~lua_Vertex(){}
inline lua_Vertex& operator = (const Vertex& v){memcpy(this, &v, sizeof(Vertex));return *this;}
inline operator Vertex* ()					{return this;}
};

class lua_DataArray_s
{
public:
IDataArrayPtr	m_data;
lua_DataArray_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IDataArrayPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_DataArray_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_DataArray_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
};
class lua_DataControl_s
{
public:
IDataControlPtr	m_data;
lua_DataControl_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IDataControlPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_DataControl_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_DataControl_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
lua_DataControl_s	add_child(const char* name);
lua_DataControl_s	at(INT index);
void			clear();
INT	count();
INT	Getlcount();
double	lreadf(INT index);
INT	lreadi(INT index);
const char*	lreadt(INT index);
void			lwritef(INT index,	double fValue);
void			lwritei(INT index,	INT nValue);
void			lwritet(INT index,	const char* szValue);
double	readf(const char* name);
INT	readi(const char* name);
const char*	readt(const char* name);
void			Setlcount(INT nCount);
void			writef(const char* name,	double fValue);
void			writei(const char* name,	INT nValue);
void			writet(const char* name,	const char* szValue);
};
class lua_FindWay_s
{
public:
IFindWayPtr	m_data;
lua_FindWay_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IFindWayPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_FindWay_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_FindWay_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
bool	Getis2dway();
bool	Getpause();
bool	Getplay();
INT	Getpointcount();
float	Getspeed();
lua_float3	point_at(INT nIndex);
bool	recalc(lua_float3& ret,	lua_floatPtr& retRotation);
void			Setis2dway(bool bMode);
void			Setpause(bool bPause);
void			Setplay(bool bPlay);
void			setpoint_at(INT nIndex,	lua_float3& pos);
void			Setpointcount(INT nCount);
void			Setspeed(float fSpeed);
};
class lua_GameObject_s
{
public:
IGameObjectPtr	m_data;
lua_GameObject_s(){m_data.id = m_data.index = 0;}
lua_GameObject_s(int id){m_data.id = id; m_data.index = 0;}
bool	isnull(){if(m_data.id == 0)return true; return false;}
bool	equal(const lua_GameObject_s& s){return (m_data.id == s.m_data.id && m_data.index == s.m_data.index);}
int				GetID();
VOID			SetID(int id);
template <typename Tclass>	Tclass	GetT(int id);
template <typename Tclass>	void	SetT(Tclass val, int id);
template <int Tother>	lua_GameTable<Tother>	GetClass(int id);
template <int Tother>	 void				SetClass(const lua_GameTable<Tother>& other, int id);
template <int Tindex>					int		T_Count();
template <int Tindex, typename Tclass>	Tclass	T_Get(int id);
template <int Tindex, typename Tclass>	void	T_Set(int id, Tclass val);
template <int Tindex,int Tother>	lua_GameTable<Tother>	T_GetClass(int id);
template <int Tindex,int Tother>	 void				T_SetClass(int id, const lua_GameTable<Tother>& other);
static Config* s_config;
static void registerLua();
int			getcontext();
void		setcontext(int n);
bool operator == (const lua_GameObject_s& val){return (m_data.id == val.m_data.id && m_data.index == val.m_data.index);}
void			action(INT/*ActionType*/ type,	float fTime);
void			action2(INT/*ActionType*/ type,	INT nLoop);
bool	attack(INT actionindex);
void			bind_action_event(const char* szLuaFunc);
void			bind_action_over(const char* szLuaFunc);
void			bind_attack_over(const char* szLuaFunc);
void			bind_die_over(const char* szLuaFunc);
void			bind_hit_over(const char* szLuaFunc);
void			bind_idle_over(const char* szLuaFunc);
void			bind_move_over(const char* szLuaFunc);
void			bind_nextbegin(const char* szLuaFunc);
lua_GameObject_s	clone();
void			die();
bool	doskill_r(INT skillid,	lua_float3& target);
bool	doskill_t(INT skillid);
float	Getaffrange();
float	Getatkrange();
lua_float3	Getbornpos();
bool	Gethit_text();
bool	Getmeshheight();
bool	Getmovedir_enable();
bool	Getmoverun();
bool	Getnextpause();
lua_float3	Getpos();
float	Getrot();
bool	Getsearchway();
lua_GameObject_s	Gettarget();
float	height();
void			idle(float fTime);
void			lookat(lua_float3& target);
void			lookat_t();
void			movedir(lua_float3& dir);
bool	moveto(lua_float3& pos);
void			moveto_b();
bool	moveto_e(const char* szEvent);
bool	moveto_t();
void			next_setid(INT index,	INT nid);
void			next_setrandom(INT index,	float fRandom);
INT	nextaction(INT/*ActionType*/ type,	float fTime);
INT	nextaction2(INT/*ActionType*/ type,	INT nLoop);
void			nextclear();
bool	nextenemy();
INT	nextidle(float fTime);
INT	nextlookat(lua_float3& pos);
INT	nextlookat_t(lua_GameObject_s& obj);
INT	nextloop(INT push_begin,	INT push_end,	INT push_loop);
INT	nextto(lua_float3& pos);
INT	nextto_e(const char* szEvent);
void			onhit(lua_GameObject_s& enemy,	INT val,	float fPlayTime);
lua_UIControl_s	onhit2(lua_GameObject_s& enemy,	INT val,	float fPlayTime);
void			pause_t(float fSecond);
float	read(const char* szName);
INT	roleid();
INT	sceneid();
lua_Map3DItem_s	sceneobj();
void			Setaffrange(float affRange);
void			Setatkrange(float atkRange);
void			Setbornpos(lua_float3& target);
void			Sethit_text(bool bText);
void			Setmeshheight(bool bEnable);
void			Setmovedir_enable(bool bRun);
void			Setmoverun(bool bRun);
void			Setnextpause(bool bPause);
void			Setpos(lua_float3& pos);
void			Setrot(float fRot);
void			Setsearchway(bool bEnable);
void			Settarget(lua_GameObject_s& target);
void			update_blood(float fProgress);
void			write(const char* szName,	float nValue);
};
class lua_Map3D_s
{
public:
IMap3DPtr	m_data;
lua_Map3D_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IMap3DPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_Map3D_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_Map3D_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
lua_Map3DItem_s	at(INT index);
void			bind_click(const char* szItemName,	const char* szLuaFunc);
void			bind_draw(const char* szItemName,	lua_UIDrawMgr_s& drawPtr,	const char* szBoneName);
void			bind_event(INT/*EventID*/ nid,	const char* szItemName,	const char* szLuaFunc);
void			bind_eventby(const char* szEvent,	const char* szItemName,	const char* szLuaFunc);
void			bind_mapevent(INT/*EventID*/ nid,	const char* szLuaFunc);
void			bind_mapeventby(const char* szEvent,	const char* szLuaFunc);
void			blend_camera(const char* szCamera,	float fBlendTime);
void			blend_fog(const char* szCamera,	float fBlendTime);
void			blend_sky(const char* szCamera,	float fBlendTime);
void			camerafollow(lua_Map3DItem_s& obj);
lua_Map3DItem_s	classat(INT classID,	INT index);
INT	classcount(INT classID);
INT	count();
void			enable(const char* szItemName,	bool bEnable);
void			enable_s(INT nID,	bool bEnable);
lua_Map3DEnv_s	envat(INT nIndex);
INT	envcount();
lua_Map3DSkill_s	freeskill();
const char*	Getactivecamera();
lua_Map3DEnv_s	GetactiveEnv();
const char*	GetactiveEnvBy();
bool	Getblooddisplay();
float	Getbloodsize();
lua_Pixel	Getcamera();
INT	Getclickradius();
bool	Getdisplay();
bool	Getdisplayterrain();
bool	Getenabled();
bool	Getenablemyclick();
bool	Getfindway2d();
bool	Gethitdisplay();
bool	Getpause();
const char*	Getsky();
lua_Map3DItem_s	insert(const char* szFileName,	lua_Pixel& mappos);
void			remove(lua_Map3DItem_s& _item);
void			replace_image(const char* szItemName,	const char* szImageSrc,	const char* szImageReplace);
lua_Pixel	scenepos(lua_Pixel& screen);
lua_Pixel	screenpos(lua_Pixel& scene);
lua_Map3DItem_s	search(INT cID);
lua_Map3DItem_s	searchby(const char* szItemName);
lua_Map3DPoint_s	searchpoint(INT id);
lua_Map3DPoint_s	searchpointby(const char* szItemName);
lua_Map3DPoint_s	searchpointby_eid(INT eventid);
void			Setactivecamera(const char* szCamera);
void			SetactiveEnv(lua_Map3DEnv_s& env);
void			SetactiveEnvBy(const char* szEnv);
bool	setblood(const char* szDlgName);
void			Setblooddisplay(bool bVisible);
void			Setbloodsize(float fSize);
void			Setcamera(lua_Pixel& pos);
void			Setclickradius(INT radius);
void			Setdisplay(bool bVisible);
void			Setdisplayterrain(bool bVisible);
void			Setenabled(bool bEnabled);
void			Setenablemyclick(bool bEnabled);
void			Setfindway2d(bool b2DMode);
void			Sethitdisplay(bool bVisible);
void			Setpause(bool bPause);
void			Setsky(const char* szCamera);
void			show(const char* szItemName,	bool bShow);
void			show_s(INT nID,	bool bShow);
ISkyPtr	skyat(INT nIndex);
INT	skycount();
};
class lua_Map3DEnv_s
{
public:
IMap3DEnvPtr	m_data;
lua_Map3DEnv_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IMap3DEnvPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_Map3DEnv_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_Map3DEnv_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
float	Getbrightness();
lua_color4	Getcolor();
float	Getcontrast();
bool	Getenabled_brightness();
bool	Getenabled_contrast();
float	Getfar();
INT/*FogMode*/	Getmode();
const char*	Getname();
float	Getnear();
lua_float3	Getshadow_center();
lua_Map3DItem_s	Getshadow_center_item();
float	Getshadow_size();
void			Setbrightness(float fBrightness);
void			Setcolor(lua_color4& c);
void			Setcontrast(float fContrast);
void			Setenabled_brightness(bool bEnabled);
void			Setenabled_contrast(bool bEnabled);
void			Setfar(float c);
void			Setmode(INT/*FogMode*/ mode);
void			Setname(const char* szName);
void			Setnear(float c);
void			Setshadow_center(lua_float3& pos);
void			Setshadow_center_item(lua_Map3DItem_s& obj);
void			Setshadow_size(float fSize);
};
class lua_Map3DItem_s
{
public:
IMap3DItemPtr	m_data;
lua_Map3DItem_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IMap3DItemPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_Map3DItem_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_Map3DItem_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
void			addhit(INT val);
void			bind_action_begin(const char* szLuaBegin);
void			bind_action_end(const char* szLuaEnd);
void			bind_click(const char* szLuaFunc);
void			bind_draw(lua_UIDrawMgr_s& drawPtr,	const char* szBindBone);
void			bind_event(INT/*EventID*/ nEvent,	const char* szLuaFunc);
void			bind_eventby(const char* szEvent,	const char* szLuaFunc);
bool	bind_mesh(const char* szBoneName,	lua_Mesh_s& mesh);
lua_Map3DItem_s	clone();
lua_Map3DItem_s	deleteme();
lua_GameObject_s	gameobj();
INT/*ActionType*/	Getaction();
const char*	Getactionby();
float	Getaspeed();
float	Getblood();
bool	Getblood_display();
lua_color4	Getbloodcolor();
INT	Getclassid();
bool	Getdisplay();
bool	Getenabled();
float	Getheight();
bool	Getloop();
const char*	Getname();
bool	Getname_display();
float	Getopacity();
lua_Map3DItem_s	Getparent();
bool	Getpause();
bool	Getplay();
lua_Pixel	Getpos();
lua_float3	Getpos3d();
float	Getrot();
float	Getscale();
float	Getspeed();
lua_matrix4x4	Getworldmat();
INT	id();
bool	isgameobj();
bool	issceneobj();
void			lookat(lua_Pixel& m);
lua_Mesh_s	mesh();
void			movedir(lua_Pixel& dir);
void			moveto(lua_Pixel& pos,	const char* szFunc);
void			pushaction(INT/*ActionType*/ type);
void			pushactionby(const char* szName);
void			replace_image(const char* szImageSrc,	const char* szImageReplace);
void			Setaction(INT/*ActionType*/ type);
void			Setactionby(const char* szName);
void			Setaspeed(float fSpeed);
void			Setblood(float fValue);
void			Setblood_display(bool bShow);
void			Setbloodcolor(lua_color4& c);
void			Setclassid(INT nClass);
void			Setdisplay(bool bVisible);
void			Setenabled(bool bVisible);
void			Setheight(float fFly);
void			Setloop(INT nLoop);
void			Setname(const char* szName);
void			Setname_display(bool bShow);
void			Setopacity(float fAlpha);
void			Setparent(lua_Map3DItem_s& c);
void			Setpause(bool b);
void			Setplay(bool b);
void			Setpos(lua_Pixel& m);
void			Setpos3d(lua_float3& m);
void			Setrot(float fRotZ);
void			Setscale(float fScale);
void			Setspeed(float fSpeed);
void			Setworldmat(lua_matrix4x4& m);
void			stopmove();
bool	unbind_mesh(lua_Mesh_s& mesh);
};
class lua_Map3DPoint_s
{
public:
IMap3DPointPtr	m_data;
lua_Map3DPoint_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IMap3DPointPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_Map3DPoint_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_Map3DPoint_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
const char*	Getname();
lua_float3	Getpos();
float	Getrange();
INT	id();
void			Setname(const char* szName);
void			Setpos(lua_float3& mappos);
void			Setrange(float range);
};
class lua_Map3DSkill_s
{
public:
IMap3DSkillPtr	m_data;
lua_Map3DSkill_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IMap3DSkillPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_Map3DSkill_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_Map3DSkill_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
void			bind_collision(INT classid,	const char* szLuaFunc);
float	Getcontinuetime();
float	Getmovespeed();
float	Getsize();
lua_Map3DItem_s	getsource();
float	Getsource_zscale();
lua_float3	Getsourcepos();
lua_Map3DItem_s	gettarget();
float	Gettarget_zscale();
lua_float3	Gettargetpos();
float	Getwaittime();
void			Setcontinuetime(float ftime);
void			Setmovespeed(float speed);
void			Setsize(float fScale);
void			Setsource_zscale(float fScale);
void			Setsourcepos(lua_float3& pos);
void			Settarget_zscale(float fScale);
void			Settargetpos(lua_float3& pos);
void			Setwaittime(float fwaittime);
void			source(const char* szSkillMesh,	lua_Map3DItem_s& item,	const char* szBoneLike);
void			start(const char* szFinishFunc);
void			stop();
void			target(lua_Map3DItem_s& item,	const char* szBoneLike);
};
class lua_Mesh_s
{
public:
IMeshPtr	m_data;
lua_Mesh_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IMeshPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_Mesh_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_Mesh_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
lua_AABox	aabox();
void			bind_draw(lua_UIDrawMgr_s& drawPtr,	const char* szBoneName);
bool	bind_mesh(const char* szBoneName,	lua_Mesh_s& mesh);
INT	bonecount();
lua_matrix4x4	bonemat_at(INT index);
const char*	bonename_at(INT index);
void			clearParent();
lua_Mesh_s	clone();
INT/*ActionType*/	Getaction();
const char*	Getactionby();
lua_color4	Getcolor();
INT	Getdefaultaction();
INT/*BlendMode*/	Getdestblend();
bool	Getdisplay();
bool	Getenabled();
const char*	getfilepath();
bool	Getfollow_camera();
float	Getopacity();
lua_Mesh_s	Getparent();
bool	Getpause();
bool	Getplay();
lua_float3	Getpos();
lua_float3	Getrot();
bool	Gets_lodhide_onexceed();
float	Gets_lodinterval_length();
float	Gets_lodmaxvisible();
lua_float3	Getscale();
lua_color4	Getselfluminous();
lua_color4	Getspecular();
float	Getspeed();
INT/*BlendMode*/	Getsrcblend();
lua_matrix4x4	Getworldmat();
float	height();
bool	isskeleton();
bool	isunwalkable(lua_float3& pos);
void			lookat(lua_float3& m);
void			playaction(INT keyBegin,	INT keyEnd,	INT nLoop);
bool	ray(lua_Pixel& pixel);
bool	ray_dir(lua_float3& orig,	lua_float3& dir);
void			replace_image(const char* szImageSrc,	const char* szImageReplace);
lua_MeshAction_s	s_action_at(INT index);
INT	s_actioncount();
INT	searchboneid(const char* szName);
INT	searchboneid_like(const char* szName);
void			Setaction(INT/*ActionType*/ t);
void			Setactionby(const char* szName);
void			Setcolor(lua_color4& c);
void			Setdefaultaction(INT actionID);
void			Setdestblend(INT/*BlendMode*/ blend);
void			Setdisplay(bool b);
void			Setenabled(bool b);
void			Setfollow_camera(bool b);
void			Setopacity(float fAlpha);
void			Setparent(lua_Mesh_s& mesh);
void			Setpause(bool b);
void			Setplay(bool b);
void			Setpos(lua_float3& pos);
void			Setrot(lua_float3& rot);
void			Sets_lodhide_onexceed(bool b);
void			Sets_lodinterval_length(float f);
void			Sets_lodmaxvisible(float f);
void			Setscale(lua_float3& scale);
void			Setselfluminous(lua_color4& c);
void			Setspecular(lua_color4& c);
void			Setspeed(float fSpeed);
void			Setsrcblend(INT/*BlendMode*/ blend);
void			Setworldmat(lua_matrix4x4& m);
lua_SubMesh_s	submesh_at(INT index);
INT	submeshcount();
void			unbind_mesh(lua_Mesh_s& mesh);
};
class lua_MeshAction_s
{
public:
IMeshActionPtr	m_data;
lua_MeshAction_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IMeshActionPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_MeshAction_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_MeshAction_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
INT	Getkeybegin();
INT	Getkeyend();
float	Getlerptime();
float	Getspeed();
INT	keycount();
void			Setkeybegin(INT beginIndex);
void			Setkeyend(INT endIndex);
void			Setlerptime(float f);
void			Setspeed(float fSpeed);
};
class lua_MeshMaterial_s
{
public:
IMeshMaterialPtr	m_data;
lua_MeshMaterial_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IMeshMaterialPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_MeshMaterial_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_MeshMaterial_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
lua_color4	Getcolor();
INT/*CullMode*/	Getcullmode();
INT/*BlendMode*/	Getdestblend();
lua_Texture_s	Getnormal_texture();
INT/*AlphaMode*/	Getopacitymode();
lua_Texture_s	Getreflect_texture();
lua_color4	Getselfluminous();
lua_color4	Getspecular();
lua_Texture_s	Getspecular_texture();
INT/*BlendMode*/	Getsrcblend();
lua_Texture_s	Gettexture();
void			Setcolor(lua_color4& c);
void			Setcullmode(INT/*CullMode*/ cull);
void			Setdestblend(INT/*BlendMode*/ blend);
void			Setnormal_texture(lua_Texture_s& texturePtr);
void			Setopacitymode(INT/*AlphaMode*/ alpha);
void			Setreflect_texture(lua_Texture_s& texturePtr);
void			Setselfluminous(lua_color4& c);
void			Setspecular(lua_color4& c);
void			Setspecular_texture(lua_Texture_s& texturePtr);
void			Setsrcblend(INT/*BlendMode*/ blend);
void			Settexture(lua_Texture_s& texturePtr);
};
class lua_SubMesh_s
{
public:
ISubMeshPtr	m_data;
lua_SubMesh_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != ISubMeshPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_SubMesh_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_SubMesh_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
bool	Getdisplay();
const char*	getname();
lua_matrix4x4	Getworldmat();
bool	isskeleton();
bool	lod_exists();
lua_MeshMaterial_s	material_at(INT index);
INT	materialcount();
void			Setdisplay(bool b);
void			Setworldmat(lua_matrix4x4& mat);
lua_Triangle	triangle_at(INT nIndex);
INT	trianglecount();
lua_Vertex	vertex_at(INT nIndex);
INT	vertexcount();
};
class lua_Table_s
{
public:
ITableReaderPtr	m_data;
lua_Table_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != ITableReaderPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_Table_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_Table_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
float	id_f(INT id,	const char* colName);
INT	id_i(INT id,	const char* colName);
const char*	id_t(INT id,	const char* colName);
INT	linecount();
float	name_f(const char* szName,	const char* colName);
INT	name_i(const char* szName,	const char* colName);
const char*	name_t(const char* szName,	const char* colName);
float	read_f(INT row,	INT col);
INT	read_i(INT row,	INT col);
const char*	read_t(INT row,	INT col);
INT	search_col(const char* colName);
float	search_f(const char* st_colName,	const char* st_rowValue,	const char* colName);
INT	search_i(const char* st_colName,	const char* st_rowValue,	const char* colName);
const char*	search_t(const char* st_colName,	const char* st_rowValue,	const char* colName);
};
class lua_Texture_s
{
public:
ITexturePtr	m_data;
lua_Texture_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != ITexturePtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_Texture_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_Texture_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
bool	commit(INT/*TextureStage*/ samplerIndex);
const char*	getfilepath();
INT	getheight();
INT	getwidth();
};
class lua_UIControl_s
{
public:
IControlPtr	m_data;
lua_UIControl_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IControlPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_UIControl_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_UIControl_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
void			bind_3d_clear();
void			bind_3d_maxdist(float fMaxDist);
void			bind_3d_obj(lua_GameObject_s& obj,	float fHeightAdd);
void			bind_3d_pos(lua_float3& pos);
void			bind_click(const char* szLuaFunc);
void			bind_draw(lua_UIDrawMgr_s& drawPtr);
void			bind_event(INT/*EventID*/ nid,	const char* szLuaFunc);
void			bind_eventby(const char* szEvent,	const char* szLuaFunc);
void			bind_image(const char* szImageFile);
lua_UIControl_s	clone(float fTime);
lua_UIControl_s	cloneto(float fTime,	lua_UIDialog_s& dest);
lua_UIDialog_s	dialog();
lua_UIElement_s	element_at(INT index);
INT	elementcount();
INT	framemove(const char* szLuaFunc,	INT nMaxMillisecond);
INT	framemove2(const char* szLuaFunc,	INT nMaxMillisecond,	const char* param);
INT/*ActionType*/	Getaction();
const char*	Getactionby();
float	Getactiontime();
bool	Getactive();
INT	Getbottom();
lua_Pixel	Getcenter();
lua_color4	Getcolor();
bool	Getdisplay();
INT	Geteffect_loop();
bool	Geteffect_stop();
bool	Getenabled();
const char*	geteventsound(INT/*UISoundPlay*/ st);
lua_UIGroup_s	Getgroup();
INT	Getgroupid();
const char*	Getgroupname();
INT	Getheight();
INT	getId();
const char*	Getimage();
lua_color4	Getimagecolor();
lua_Mesh_s	Getinnermesh();
INT	Getlayer();
INT	Getleft();
INT	Getmovebottom();
bool	Getmovedialog();
INT	Getmoveleft();
INT	Getmoveright();
INT	Getmovetop();
const char*	Getname();
INT/*ActionType*/	Getnextaction();
float	Getopacity();
bool	Getpixelpick();
lua_Pixel	Getpos();
lua_Rect	Getrect();
INT	Getright();
float	Getrot();
float	Getrot2d();
lua_Pixel	Getrotcenter();
bool	Getrotenable();
float	Getscale();
float	Getscalex();
float	Getscaley();
bool	Getscroll_extend();
float	Getscroll_firstspeed();
lua_UIControl_s	Getscroll_h();
INT	Getscroll_max();
INT	Getscroll_min();
float	Getscroll_speed();
lua_UIControl_s	Getscroll_v();
float	Getspeed();
const char*	Gettext();
INT/*DrawTextAlign*/	Gettextalign();
lua_color4	Gettextcolor();
INT	Gettop();
INT/*UIControlType*/	gettype();
float	Getvalf();
INT	Getvali();
const char*	Getvalt();
float	Getwaitbefore_actionplay();
INT	Getwidth();
void			innermesh_clear();
INT	interval(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond);
INT	interval2(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond,	const char* param);
bool	isfocus();
lua_Pixel	mousepoint();
bool	pick(INT x,	INT y);
void			progress_time(float fTimeSecond);
void			removeChild();
void			reset_trans();
void			rot2d_dir(lua_float2& dir);
lua_Pixel	saved_pos();
lua_Rect	saved_rect();
float	saved_rot();
void			Setaction(INT/*ActionType*/ nType);
void			Setactionby(const char* ani);
void			Setactiontime(float f);
void			Setactive(bool b);
void			Setbottom(INT nValue);
void			Setcenter(lua_Pixel& pos);
void			setChild(lua_UIDialog_s& dlg);
void			Setcolor(lua_color4& c);
void			Setdisplay(bool b);
void			Seteffect_loop(INT nLoop);
void			Seteffect_stop(bool bStop);
void			Setenabled(bool b);
void			seteventsound(INT/*UISoundPlay*/ st,	const char* szFile);
void			Setgroup(lua_UIGroup_s& g);
void			Setgroupid(INT nID);
void			Setgroupname(const char* szName);
void			Setheight(INT nValue);
void			Setimage(const char* szFile);
void			Setimagecolor(lua_color4& c);
void			Setinnermesh(lua_Mesh_s& mesh);
void			Setlayer(INT layer);
void			Setleft(INT nValue);
void			Setmovebottom(INT nValue);
void			Setmovedialog(bool b);
void			Setmoveleft(INT nValue);
void			Setmoveright(INT nValue);
void			Setmovetop(INT nValue);
void			Setname(const char* szName);
void			Setnextaction(INT/*ActionType*/ nType);
void			Setopacity(float v);
void			Setpixelpick(bool b);
void			Setpos(lua_Pixel& pos);
void			Setrect(lua_Rect& rc);
void			Setright(INT nValue);
void			Setrot(float fDegress);
void			Setrot2d(float fDegress);
void			Setrotcenter(lua_Pixel& v);
void			Setrotenable(bool bEnable);
void			Setscale(float fScale);
void			Setscalex(float fScale);
void			Setscaley(float fScale);
void			Setscroll_extend(bool bExtend);
void			Setscroll_firstspeed(float fSpeed);
void			Setscroll_h(lua_UIControl_s& scrollH);
void			Setscroll_max(INT nMax);
void			Setscroll_min(INT nMin);
void			Setscroll_speed(float fSpeed);
void			Setscroll_v(lua_UIControl_s& scrollV);
void			Setspeed(float fSpeed);
void			Settext(const char* szText);
void			Settextalign(INT/*DrawTextAlign*/ fmt);
void			Settextcolor(lua_color4& c);
void			Settop(INT nValue);
void			Setvalf(float fValue);
void			Setvali(INT nValue);
void			Setvalt(const char* szValue);
void			Setwaitbefore_actionplay(float fTime);
void			Setwidth(INT nValue);
void			sort_draws();
INT	timeout(const char* szLuaFunc,	INT nMillisecond);
INT	timeout2(const char* szLuaFunc,	INT nMillisecond,	const char* param);
};
class lua_UIDialog_s
{
public:
IDialogPtr	m_data;
lua_UIDialog_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IDialogPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_UIDialog_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_UIDialog_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
lua_UIControl_s	at(INT index);
void			bind_click(const char* szControl,	const char* szLuaFunc);
void			bind_draw(const char* szControl,	lua_UIDrawMgr_s& drawPtr);
void			bind_event(const char* szControl,	INT/*EventID*/ nid,	const char* szLuaFunc);
void			bind_eventby(const char* szControl,	const char* szEvent,	const char* szLuaFunc);
void			bind_image(const char* szControl,	const char* szImageFile);
INT	count();
lua_UIControl_s	create(INT/*UIControlType*/ type,	lua_Rect& rc);
void			delete_at(INT index);
void			deleteall();
void			deleteby(lua_UIControl_s& control);
bool	destroy();
void			enable(const char* szControl,	bool bEnable);
INT	framemove(const char* szLuaFunc,	INT nMaxMillisecond);
INT	framemove2(const char* szLuaFunc,	INT nMaxMillisecond,	const char* param);
lua_UIGroup_s	gat(INT nIndex);
INT	gcount();
lua_UIGroup_s	gcreate();
void			gdelete(lua_UIGroup_s& group);
INT/*ActionType*/	Getaction();
const char*	Getactionby();
lua_Pixel	Getcenter();
lua_color4	Getcolor();
bool	Getdisplay();
bool	Getdomodal();
bool	Getenabled();
INT	Getheight();
bool	Gethideon_killfocus();
INT	Getlayer();
INT	Getleft();
const char*	Getname();
float	Getopacity();
bool	Getpause();
lua_Pixel	Getpos();
lua_Rect	Getrect();
float	Getrot2d();
float	Getrotx();
float	Getroty();
float	Getrotz();
float	Getscale();
float	Getscalex();
float	Getscaley();
INT	Gettop();
bool	Gettopmost();
float	getvalf(const char* szControl);
INT	getvali(const char* szControl);
const char*	getvalt(const char* szControl);
INT	Getwidth();
lua_UIGroup_s	gsearch(const char* szName);
lua_UIGroup_s	gsearchbyid(INT nID);
void			hold();
void			image_mode(const char* szControl,	const char* szMode,	INT/*DrawTextAlign*/ align);
INT	interval(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond);
INT	interval2(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond,	const char* param);
bool	istooltip_window();
void			map_add2dchild(lua_UIDialog_s& child);
void			map_addfindway(lua_FindWay_s& fw);
void			map_addmoveto(lua_Pixel& fw);
lua_Pixel	map_areaindex(INT mapx,	INT mapy);
void			map_beginmoveto(float fSpeed,	const char* szLuaFunc);
lua_Pixel	map_center(lua_Pixel& pos);
void			map_clear2dchild();
void			map_clear_moveable();
lua_Pixel	map_count();
lua_Pixel	map_gridsize();
lua_Pixel	map_mapindex(INT areax,	INT areay);
bool	map_moveable(INT areax,	INT areay);
lua_Pixel	map_pickarea(lua_Pixel& pos);
lua_Pixel	map_pickmap(lua_Pixel& pos);
bool	map_searchway(lua_FindWay_s& fw,	lua_Pixel& from,	lua_Pixel& to);
bool	map_searchway2(lua_FindWay_s& fw,	INT fareax,	INT fareay,	INT tareax,	INT tareay);
void			map_set_moveable(INT areax,	INT areay,	bool bMoveable);
void			map_stopmove();
void			move(const char* szControl,	INT left,	INT top);
lua_UIControl_s	pick(INT x,	INT y);
void			playani(const char* szControl,	const char* szAniName);
void			recalc_rect(bool bIncludeHide);
void			rotdir(lua_Pixel& dir);
lua_UIControl_s	search(const char* szName);
lua_UIControl_s	searchbyid(INT nID);
void			Setaction(INT/*ActionType*/ nType);
void			Setactionby(const char* szName);
void			Setcenter(lua_Pixel& _val);
void			Setcolor(lua_color4& val);
void			Setdisplay(bool b);
void			Setdomodal(bool bModel);
void			Setenabled(bool bEnable);
void			Setheight(INT _val);
void			Sethideon_killfocus(bool b);
void			Setlayer(INT nLayer);
void			Setleft(INT _val);
void			Setname(const char* szName);
void			Setopacity(float val);
void			Setpause(bool bPaused);
void			Setpos(lua_Pixel& _val);
void			Setrect(lua_Rect& _val);
void			Setrot2d(float fDegress);
void			Setrotx(float fDegress);
void			Setroty(float fDegress);
void			Setrotz(float fDegress);
void			Setscale(float fScale);
void			Setscalex(float fScale);
void			Setscaley(float fScale);
void			settooltip_window(bool b);
void			Settop(INT _val);
void			Settopmost(bool bTopMost);
void			setvalf(const char* szControl,	float fValue);
void			setvali(const char* szControl,	INT nValue);
void			setvalt(const char* szControl,	const char* szValue);
void			Setwidth(INT _val);
void			show(const char* szControl,	bool bShow);
INT	timeout(const char* szLuaFunc,	INT nMillisecond);
INT	timeout2(const char* szLuaFunc,	INT nMillisecond,	const char* param);
};
class lua_UIDrawMgr_s
{
public:
IUIDrawMgrPtr	m_data;
lua_UIDrawMgr_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IUIDrawMgrPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_UIDrawMgr_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_UIDrawMgr_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
lua_UIDrawMgr_s	add_child();
void			bind_click(const char* szFunc);
void			bind_dblclick(const char* szFunc);
INT	child_count();
void			delete_child(INT nIndex);
lua_Rect	draw_rect();
lua_UIDrawMgr_s	get_child(INT nIndex);
lua_color4	get_color(INT index);
lua_Pixel	get_point(INT index);
lua_float2	get_uv(INT index);
lua_Pixel	Getcenter();
float	Getclickscale();
lua_color4	Getcolor();
INT/*CullMode*/	Getcullmode();
bool	Getdisplay();
bool	Getenabled();
INT	Gethighlight();
INT	Getlayer();
INT	Getnumber();
float	Getoffset3d_z();
float	Getopacity();
lua_Pixel	Getpos();
INT	Getrotx();
INT	Getroty();
INT	Getrotz();
float	Getscale();
float	Getscalex();
float	Getscaley();
const char*	Gettext();
void			image(const char* szTexture);
void			image_mode(const char* szMode);
void			image_rect(INT left,	INT top,	INT width,	INT height);
void			image_uvani(INT start,	INT count,	float speed);
void			image_uvmap(INT width,	INT height,	INT wcount,	INT hcount);
void			number_align(const char* szModeFirst,	const char* szAlign);
void			number_mode(const char* szModeFirst,	INT numberHeight,	INT spaceX);
float	progress();
void			progress_mode(float fValue);
void			set_color(INT index,	lua_color4& c);
void			set_point(INT index,	INT x,	INT y);
void			set_rect(INT left,	INT top,	INT right,	INT bottom);
void			set_uv(INT index,	float u,	float v);
void			Setcenter(lua_Pixel& center);
void			Setclickscale(float fscale);
void			Setcolor(lua_color4& c);
void			Setcullmode(INT/*CullMode*/ layer);
void			Setdisplay(bool bDisplay);
void			Setenabled(bool bEnabled);
void			Sethighlight(INT highlight);
void			Setlayer(INT layer);
void			Setnumber(INT num);
void			Setoffset3d_z(float fOffsetZ);
void			Setopacity(float fAlpha);
void			Setpos(lua_Pixel& center);
void			Setrotx(INT fDegress);
void			Setroty(INT fDegress);
void			Setrotz(INT fDegress);
void			Setscale(float fScale);
void			Setscalex(float fScalex);
void			Setscaley(float fScaley);
void			Settext(const char* text);
void			text_mode(const char* szValue,	float fFontScale);
};
class lua_UIElement_s
{
public:
IElementPtr	m_data;
lua_UIElement_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IElementPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_UIElement_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_UIElement_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
INT/*BlendMode*/	Getdestblend();
INT	Getfontid();
const char*	Getimage();
lua_color4	Getimagecolor();
INT/*BlendMode*/	Getsrcblend();
lua_color4	Gettextcolor();
INT/*TextureBlend*/	Gettextureblend();
INT	Gettextureid();
const char*	Gettexturemode();
lua_Rect	Gettexturerect();
INT	Gettileheight();
INT	Gettilewidth();
const char*	Getuvaction();
float	Getuvspeed();
bool	isscale();
void			Setdestblend(INT/*BlendMode*/ m);
void			Setfontid(INT nFontID);
void			Setimage(const char* szFileName);
void			Setimagecolor(lua_color4& c);
void			setscale(bool bScale,	bool bWrap,	INT wrapWidth,	INT wrapHeight);
void			Setsrcblend(INT/*BlendMode*/ m);
void			Settextcolor(lua_color4& c);
void			Settextureblend(INT/*TextureBlend*/ op);
void			Settextureid(INT nID);
void			Settexturemode(const char* szMode);
void			Settexturerect(lua_Rect& _s);
void			Settileheight(INT nTileHeight);
void			Settilewidth(INT nTileWidth);
void			Setuvaction(const char* map);
void			Setuvspeed(float f);
const char*	uvaction_at(INT nIndex);
INT	uvactioncount();
};
class lua_UIGroup_s
{
public:
IUIGroupPtr	m_data;
lua_UIGroup_s(){m_data.t = m_data.ptr = 0;}
bool	isnull(){if(m_data.ptr == 0 || (m_data.t % 1000) != IUIGroupPtr_Class)return true; return !IsExistPtr(m_data);}
bool	equal(const lua_UIGroup_s& s){return (m_data.ptr == s.m_data.ptr && m_data.idx == s.m_data.idx);}
int			getcontext(){return GetContext(m_data);}
void		setcontext(int n){SetContext(m_data, n);}
bool operator == (const lua_UIGroup_s& val){return (m_data.ptr == val.m_data.ptr && m_data.idx == val.m_data.idx);}
lua_UIControl_s	at(INT nIndex);
void			bind_click(const char* szControl,	const char* szLuaFunc);
void			bind_draw(const char* szControl,	lua_UIDrawMgr_s& drawPtr);
void			bind_event(INT/*EventID*/ nid,	const char* szControl,	const char* szLuaFunc);
void			bind_eventby(const char* szEvent,	const char* szControl,	const char* szLuaFunc);
void			bind_image(const char* szControl,	const char* szImageFile);
lua_UIGroup_s	clone();
lua_UIGroup_s	cloneto(lua_UIDialog_s& dest);
INT	count();
lua_UIDialog_s	dialog();
void			enable(const char* szControl,	bool bEnable);
INT	framemove(const char* szLuaFunc,	INT nMaxMillisecond);
INT	framemove2(const char* szLuaFunc,	INT nMaxMillisecond,	const char* param);
INT/*ActionType*/	Getaction();
const char*	Getactionby();
lua_Pixel	Getcenter();
float	Getcurrenttime();
bool	Getdisplay();
bool	Getenabled();
INT	Getleft();
const char*	Getname();
float	Getopacity();
lua_Pixel	Getpos();
float	Getrot();
float	Getspeed();
INT	Gettop();
float	getvalf(const char* szControl);
INT	getvali(const char* szControl);
const char*	getvalt(const char* szControl);
INT	height();
void			image_mode(const char* szControl,	const char* szMode,	INT/*DrawTextAlign*/ align);
INT	interval(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond);
INT	interval2(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond,	const char* param);
void			playani(const char* szControl,	const char* szAniName);
lua_UIControl_s	search(const char* szName);
void			Setaction(INT/*ActionType*/ nType);
void			Setactionby(const char* szName);
void			Setcenter(lua_Pixel& pos);
void			Setcurrenttime(float fTime);
void			Setdisplay(bool bVisible);
void			Setenabled(bool bEnable);
void			Setleft(INT nLeft);
void			Setname(const char* szName);
void			Setopacity(float fOpacity);
void			Setpos(lua_Pixel& _val);
void			Setrot(float fDegress);
void			setRotDir(lua_float2& dir);
void			Setspeed(float fSpeed);
void			Settop(INT nTop);
void			setvalf(const char* szControl,	float fValue);
void			setvali(const char* szControl,	INT nValue);
void			setvalt(const char* szControl,	const char* szValue);
void			show(const char* szControl,	bool bShow);
INT	timeout(const char* szLuaFunc,	INT nMillisecond);
INT	timeout2(const char* szLuaFunc,	INT nMillisecond,	const char* param);
INT	width();
};
class lua_Camera
{
public:
bool	aaboxin(lua_AABox& p);
void			dither(lua_float3& offset,	float fMaxOffsetTime);
bool	get2d_from3d(lua_Pixel& point,	lua_float3& pos);
void			get3d_from2d(lua_float3& orig,	lua_float3& dir,	lua_Pixel& point);
lua_Rect	Getarea();
lua_CameraData	Getdata();
float	Getdistance();
float	Getdistance_max();
float	Getdistance_min();
bool	Getenabled();
lua_float3	Geteye();
float	Getfov();
INT	Getheight();
bool	Getintersect_eye();
lua_float3	Getlookat();
float	Getoffset_down();
bool	Getoffset_enable();
lua_float3	Getpos();
lua_matrix4x4	Getprojmat();
lua_float2	Getrot();
lua_float3	Gettarget();
lua_matrix4x4	Getviewmat();
INT	Getwidth();
float	Getzfar();
float	Getznearest();
bool	isdisplay(lua_float3& pos,	float fRadius,	float fMaxDistance);
bool	isortho();
bool	isperspective();
void			nextdist(float fDistance,	float fMoveSpeed);
void			nextdither(float offset,	float fMaxOffsetTime);
void			nextrot(float rotZ,	float rotX,	float fRotSpeed);
void			nextto(lua_float3& pos,	float fMoveSpeed);
void			orthoproj(INT w,	INT h,	float znear,	float zfar);
void			perspectiveproj(float fov,	INT w,	INT h,	float znear,	float zfar);
bool	planein(lua_float3& p1,	lua_float3& p2,	lua_float3& p3,	lua_float3& p4);
void			play(const char* szLuaFunc,	INT nParam);
bool	pointin(lua_float3& p);
void			Setarea(lua_Rect& rc);
void			Setdata(lua_CameraData& p);
void			Setdistance(float fLength);
void			Setdistance_max(float fLength);
void			Setdistance_min(float fLength);
void			Setenabled(bool bEnabled);
void			Seteye(lua_float3& eye);
void			Setfov(float fValue);
void			Setheight(INT fValue);
void			Setintersect_eye(bool bEnabled);
void			Setlookat(lua_float3& vLookat);
void			Setoffset_down(float fSpeed);
void			Setoffset_enable(bool bEnable);
void			Setpos(lua_float3& pos);
void			Setprojmat(lua_matrix4x4& view);
void			Setrot(lua_float2& rot);
void			Settarget(lua_float3& target);
void			Setviewmat(lua_matrix4x4& view);
void			Setwidth(INT fValue);
void			Setzfar(float fValue);
void			Setznearest(float fValue);
bool	spherein(lua_float3& p,	float fRadius);
void			stop();
};
class lua_GameMgr
{
public:
template <typename Tclass>	Tclass	GetT(int id);
template <typename Tclass>	void	SetT(Tclass val, int id);
template <int Tother>	lua_GameTable<Tother>	GetClass(int id);
template <int Tother>	 void				SetClass(const lua_GameTable<Tother>& other, int id);
template <int Tindex>					int		T_Count();
template <int Tindex, typename Tclass>	Tclass	T_Get(int id);
template <int Tindex, typename Tclass>	void	T_Set(int id, Tclass val);
template <int Tindex,int Tother>	lua_GameTable<Tother>	T_GetClass(int id);
template <int Tindex,int Tother>	 void				T_SetClass(int id, const lua_GameTable<Tother>& other);
static void registerLua();
bool	create(INT sceneID,	const char* szLuaFunc);
INT	framemove(lua_GameObject_s& obj,	const char* szLuaFunc,	INT nMaxMillisecond);
bool	Getcamerafollow();
lua_GameObject_s	Getcameratarget();
float	getheight_3d(lua_float3& pos);
bool	Getplotenable();
INT	interval(lua_GameObject_s& obj,	const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond);
lua_GameObject_s	leader();
const char*	loadtext(INT nID,	const char* szCol);
const char*	loadtextby(const char* szName,	const char* szCol);
lua_GameObject_s	objectat(INT index);
INT	objectcount();
void			plotfinish();
INT	plotid();
void			plotnext();
void			plotto(INT id);
lua_Map3DItem_s	ray3d(lua_Pixel& pixel);
lua_Map3DItem_s	ray3d_dir(lua_float3& orig,	lua_float3& dir);
lua_Map3DItem_s	ray3d_dir2(lua_float3& orig,	lua_float3& dir,	float fMaxLength);
const char*	ray3d_getname();
float	ray3d_length();
lua_float3	ray3d_normal();
lua_float3	ray3d_point();
void			reborn(lua_GameObject_s& obj,	float fRebornTime);
void			resetAll();
lua_Map3D_s	scene();
lua_GameObject_s	search(INT nID);
lua_GameObject_s	searchby(const char* szName);
lua_GameObject_s	searchby_sceneid(INT sceneID);
void			Setcamerafollow(bool bEnable);
void			Setcameratarget(lua_GameObject_s& obj);
void			Setplotenable(bool bEnable);
INT	timeout(lua_GameObject_s& obj,	const char* szLuaFunc,	INT nMillisecond);
};
class lua_MyGlobal
{
public:
};
class lua_System
{
public:
INT	fps();
lua_color4	Getbackcolor();
INT/*CullMode*/	Getcullmode();
bool	Getenablevr();
bool	Getenablevr2();
bool	Getenablez();
INT	Getgravity_smooth();
INT	Getmaxfps();
bool	Getmute();
INT/*AlphaMode*/	Getopacitymode();
bool	Getshowfps();
bool	Getshowui();
lua_float2	Getuioffset();
lua_float2	Getuiscale();
float	Getvr_rotx();
INT	Getvr_smooth();
float	Getvrdistance();
lua_float2	gravity();
lua_sensordata	sensor();
lua_float3	sensor_rotator();
void			Setbackcolor(lua_color4& c);
void			Setcullmode(INT/*CullMode*/ nCull);
void			Setenablevr(bool b);
void			Setenablevr2(bool b);
void			Setenablez(bool b);
void			Setgravity_smooth(INT s);
void			Setmaxfps(INT n);
void			Setmute(bool bMute);
void			Setopacitymode(INT/*AlphaMode*/ nMode);
void			Setshowfps(bool b);
void			Setshowui(bool b);
void			Setuioffset(lua_float2& off);
void			Setuiscale(lua_float2& s);
void			Setvr_rotx(float f);
void			Setvr_smooth(INT s);
void			Setvrdistance(float f);
void			vibrator(float fSecond);
void			vr_beginsmooth();
void			vr_front(lua_float3& dir);
};
class lua_Global
{
public:
static double	Gapptime();
static float	Gelapsedtime();
static INT	Gtimeout(const char* szLuaFunc,	INT nMillisecond);
static INT	Ginterval(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond);
static INT	Gframemove(const char* szLuaFunc,	INT nMaxMillisecond);
static INT	Gtimeout2(const char* szLuaFunc,	INT nMillisecond,	const char* param);
static INT	Ginterval2(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond,	const char* param);
static INT	Gframemove2(const char* szLuaFunc,	INT nMaxMillisecond,	const char* param);
static void	Gcleartimeby(const char* szFunc);
static void	Gcleartimeby2(const char* szFunc,	const char* param);
static void	Gcleartime(INT timeid);
static void	Gbreaktime();
static void	Gcleartime_all();
static lua_Time	Glocaltime();
static INT	Gtimediff_d(lua_Time& t1,	lua_Time& t2);
static INT	Gtimediff_h(lua_Time& t1,	lua_Time& t2);
static INT	Gtimediff_m(lua_Time& t1,	lua_Time& t2);
static INT	Gtimediff_s(lua_Time& t1,	lua_Time& t2);
static void	Gpause(bool bPause);
static void	Gspeed(float fSpeed);
static lua_Pixel	Gmousepoint(INT nIndex);
static lua_Pixel	Gmousepoint2d(INT nIndex);
static INT	Gscreenwidth();
static INT	Gscreenheight();
static INT	Ggamewidth();
static INT	Ggameheight();
static void	Gexit();
static const char*	Galert(const char* szMessage,	const char* szTitle,	const char* szType);
static void	Gmsgbox(const char* szTitle,	const char* szMsg,	const char* szFlags,	const char* script);
static const char*	Glan(const char* szName);
static void	Gsetlanguage(const char* szlanPath);
static const char*	Glanguage_image(const char* szFileName);
static void	Gbind_input(const char* szEvent,	const char* szFunc);
static bool	Ginfile(const char* szFile);
static bool	Ginsystem(const char* szSystem);
static void	Ggoto(const char* szLuaFile);
static void	Gsendmessage(const char* szName,	const char* szParams,	const char* szParams2);
static void	Gad_show(INT nType,	bool bShow);
static void	Gad_setpos(INT x,	INT y);
static void	Gopenurl(const char* url);
static void	Ggotocomment();
static void	Gopenrank(const char* rank);
static void	Grankscore(const char* rank,	INT nValue);
static void	Gachievement(const char* achievement,	INT percent);
static lua_matrix4x4	Gworldmat();
static lua_matrix4x4	Gviewmat();
static lua_matrix4x4	Gprojmat();
static INT	Ground(float d);
static INT	Gabs(double n);
static INT	Gsymbol(double n);
static INT	Gatoi(const char* v);
static bool	Gisstr(const char* a,	const char* b);
static float	Grand(float fmin,	float fmax);
static INT	Gint(double d);
static bool	Gpoint_in(lua_Rect& rect,	lua_Pixel& point);
static float	Gtovalue(float meter);
static float	Gtometer(float v);
static INT	Grgba(INT r,	INT g,	INT b,	INT a);
static INT	Grgbaf(float r,	float g,	float b,	float a);
static float	Gclamp_angle(float fDegress);
static float	Gclamp(float fValue,	float fMin,	float fMax);
static lua_UIDialog_s	Gdlg_create();
static void	Gdlg_delete(lua_UIDialog_s& ptr);
static lua_UIDialog_s	Gdlg_load(const char* szFileName);
static lua_UIDialog_s	Gdlg_loadnew(const char* szFileName);
static lua_UIDialog_s	Gdlg_loadAndProgress(const char* szFileName);
static lua_UIDialog_s	Gdlg_search(const char* szDlgName);
static lua_UIDialog_s	Gdlg_at(INT index);
static INT	Gdlg_count();
static lua_UIDrawMgr_s	Gdraw_create();
static lua_FindWay_s	Gfindway_create();
static void	Gfindway_delete(lua_FindWay_s& ptr);
static lua_Table_s	Gtable_load(const char* szFile);
static void	Gtable_delete(lua_Table_s& ptr);
static lua_Texture_s	Gtexture_load(const char* szFile);
static void	Gtexture_delete(lua_Texture_s& ptr);
static lua_Mesh_s	Gmesh_load(const char* szFile,	bool bAutoRender);
static void	Gmesh_delete(lua_Mesh_s& mesh);
static lua_Map3D_s	Gmap3d_load(const char* szName);
static void	Gmap3d_delete(lua_Map3D_s& mesh);
static INT	Gtext_create(const char* szText,	INT x,	INT y);
static INT	Gtext_create2(const char* szText,	INT x,	INT y,	lua_color4& c,	INT size);
static void	Gtext_update(INT nID,	const char* szText);
static void	Gtext_delete(INT nID);
static bool	Ginclude(const char* szFile);
static lua_DataControl_s	Gconfig(const char* name);
static bool	Gisconfig(const char* name);
static const char*	Greadt(const char* name);
static double	Greadf(const char* name);
static INT	Greadi(const char* name);
static void	Gwritet(const char* name,	const char* szValue);
static void	Gwritef(const char* name,	double fValue);
static void	Gwritei(const char* name,	INT nValue);
static void	Gconfigfile(const char* name);
static void	Gconfigsave(bool b);
static lua_DataControl_s	Gvaldata(const char* name);
static bool	Gisval(const char* name);
static const char*	Ggetvalt(const char* name);
static double	Ggetvalf(const char* name);
static INT	Ggetvali(const char* name);
static void	Gsetvalt(const char* name,	const char* szValue);
static void	Gsetvalf(const char* name,	double fValue);
static void	Gsetvali(const char* name,	INT nValue);
static INT	Gplaywave(const char* szName);
static INT	Gplaymp3(const char* szFile);
static INT	Gplaybgm(const char* szFile);
static INT	Gplaybgm2(const char* szFile,	float fVolume);
static INT	Gplaymp3_2(const char* szFile,	INT nLoop);
static INT	Gplaysnd(const char* szFile,	float fVolume,	float fPan,	float fRate);
static void	Gstopsnd(INT s2d);
static void	Gstopsndall();
static void	Gsetsndvolume(INT snd,	float fVolume);
static void	Gsetsndvolumepan(INT s2d,	float fVolume,	float fPan);
static void	Gsetsndrate(INT snd,	float fRate);
static void	Gsetupwave(const char* szName,	INT nDuplicateCount);
static void	Gdraw_bind(const char* szLuaFunc);
static void	Gdraw2d_rect(lua_Rect& _rect,	lua_color4& c,	bool bFill);
static void	Gdraw2d_line(lua_Pixel& p1,	lua_Pixel& p2,	lua_color4& c);
static void	Gdraw2d_linec(lua_PixelColor& p1,	lua_PixelColor& p2);
static void	Gdraw2d_text(lua_Rect& _rect,	const char* szText,	lua_color4& c);
static void	Gdraw2d_texture(lua_Texture_s& texture,	lua_Rect& _rect,	lua_Rect& _textureRect,	lua_color4& c);
static void	Gdraw2d_triangle(lua_Texture_s& texture,	lua_PixelUV& p1,	lua_PixelUV& p2,	lua_PixelUV& p3,	bool bFill);
static void	Gdraw3d_triangle(lua_float3& p1,	lua_float3& p2,	lua_float3& p3,	lua_color4& c);
static void	Gdraw3d_box(lua_AABox& box,	lua_color4& c,	bool bFill);
static void	Gdraw3d_line(lua_float3& _from,	lua_float3& _to,	lua_color4& c);
static void	Gjinit(const char* szHttpInPhpFile);
static void	Gjbind(INT id,	const char* szFunc);
static const char*	Gjget(const char* szName);
static INT	Gjgeti(const char* szName);
static float	Gjgetf(const char* szName);
static void	Gjclear(const char* szName,	float fValue);
static void	Gjset(const char* szName,	const char* szValue);
static void	Gjseti(const char* szName,	INT nValue);
static void	Gjsetf(const char* szName,	float fValue);
static void	Gjrequest();
static void	Gdownload(const char* httpRequest,	const char* inDocFile);
static void	Gnet_init(INT dwGUID);
static void	Gnet_onlink(const char* szFunc);
static void	Gnet_ondislink(const char* szFunc);
static void	Gnet_onmessage(const char* szFunc);
static bool	Gnet_server(const char* name,	INT port);
static void	Gnet_search(const char* szFunc,	INT port);
static void	Gnet_server_max_links(INT nMaxLinks);
static INT	Gnet_connect(const char* szIP,	INT port);
static void	Gnet_close(INT linkid);
static void	Gnet_stopall();
static const char*	Gnet_get();
static INT	Gnet_geti();
static float	Gnet_getf();
static void	Gnet_begin();
static void	Gnet_set(const char* szValue);
static void	Gnet_seti(INT nValue);
static void	Gnet_setf(float fValue);
static void	Gnet_send(INT linkid,	short msgid);
static void	Grandom_init(INT number);
static void	Grandom_weight(INT index,	INT weight);
static void	Grandom_do();
static void	Grandom_table(INT number);
static INT	Grandom_get(INT b);
static void	Gc_add(const char* szName);
static void	Gc_set(const char* szName,	INT nValue);
static INT	Gc_get(const char* szName);
};
};

#endif
