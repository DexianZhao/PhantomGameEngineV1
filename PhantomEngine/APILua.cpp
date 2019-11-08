#include "APILua.h"
extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}
#include "lua/lua_tinker.h"
using namespace lua_tinker;namespace Phantom{
void			lua_color4::fromrgb(INT r,	INT g,	INT b)
{
color4_fromrgb(this,	r,	g,	b);
}
void			lua_color4::fromrgba(INT rgb,	float fAlpha)
{
color4_fromrgba(this,	rgb,	fAlpha);
}
INT			lua_color4::Getrgba()
{
return color4_Getrgba(this);
}
INT			lua_color4::rgb()
{
return color4_rgb(this);
}
void			lua_color4::set(float r,	float g,	float b,	float a)
{
color4_set(this,	r,	g,	b,	a);
}
void			lua_color4::Setrgba(INT bgra)
{
color4_Setrgba(this,	bgra);
}
void			lua_color4::setup(float r,	float g,	float b,	float a)
{
color4_setup(this,	r,	g,	b,	a);
}
void			lua_float2::barycentric(lua_float2& pV1,	lua_float2& pV2,	lua_float2& pV3,	float f,	float g)
{
float2_barycentric(this,	pV1,	pV2,	pV3,	f,	g);
}
void			lua_float2::catmullrom(lua_float2& pV0,	lua_float2& pV1,	lua_float2& pV2,	lua_float2& pV3,	float s)
{
float2_catmullrom(this,	pV0,	pV1,	pV2,	pV3,	s);
}
float			lua_float2::cross(lua_float2& v2)
{
return float2_cross(this,	v2);
}
float			lua_float2::dot(lua_float2& v2)
{
return float2_dot(this,	v2);
}
void			lua_float2::hermite(lua_float2& pV1,	lua_float2& pT1,	lua_float2& pV2,	lua_float2& pT2,	float s)
{
float2_hermite(this,	pV1,	pT1,	pV2,	pT2,	s);
}
float			lua_float2::length()
{
return float2_length(this);
}
void			lua_float2::lerp(lua_float2& v1,	lua_float2& v2,	float fLerp)
{
float2_lerp(this,	v1,	v2,	fLerp);
}
void			lua_float2::normalize()
{
float2_normalize(this);
}
void			lua_float2::setup(float x,	float y)
{
float2_setup(this,	x,	y);
}
void			lua_float2::transformcoord(lua_float2& pV,	lua_matrix4x4& pM)
{
float2_transformcoord(this,	pV,	pM);
}
void			lua_float2::transformnormal(lua_float2& pV,	lua_matrix4x4& pM)
{
float2_transformnormal(this,	pV,	pM);
}
void			lua_float3::barycentric(lua_float3& pV1,	lua_float3& pV2,	lua_float3& pV3,	float f,	float g)
{
float3_barycentric(this,	pV1,	pV2,	pV3,	f,	g);
}
void			lua_float3::catmullrom(lua_float3& pV0,	lua_float3& pV1,	lua_float3& pV2,	lua_float3& pV3,	float s)
{
float3_catmullrom(this,	pV0,	pV1,	pV2,	pV3,	s);
}
void			lua_float3::cross(lua_float3& v1,	lua_float3& v2)
{
float3_cross(this,	v1,	v2);
}
float			lua_float3::dot(lua_float3& v2)
{
return float3_dot(this,	v2);
}
float			lua_float3::getrotate_z()
{
return float3_getrotate_z(this);
}
void			lua_float3::hermite(lua_float3& pV1,	lua_float3& pT1,	lua_float3& pV2,	lua_float3& pT2,	float s)
{
float3_hermite(this,	pV1,	pT1,	pV2,	pT2,	s);
}
float			lua_float3::length()
{
return float3_length(this);
}
void			lua_float3::lerp(lua_float3& v1,	lua_float3& v2,	float fLerp)
{
float3_lerp(this,	v1,	v2,	fLerp);
}
void			lua_float3::normalize()
{
float3_normalize(this);
}
void			lua_float3::rotateX(lua_float3& v2,	float fDegress)
{
float3_rotateX(this,	v2,	fDegress);
}
void			lua_float3::rotateY(lua_float3& v2,	float fDegress)
{
float3_rotateY(this,	v2,	fDegress);
}
void			lua_float3::rotateZ(lua_float3& v2,	float fDegress)
{
float3_rotateZ(this,	v2,	fDegress);
}
void			lua_float3::setup(float x,	float y,	float z)
{
float3_setup(this,	x,	y,	z);
}
void			lua_float3::transform(lua_float3& v2,	lua_matrix4x4& m)
{
float3_transform(this,	v2,	m);
}
void			lua_float3::transform3x3(lua_float3& v2,	lua_matrix4x4& m)
{
float3_transform3x3(this,	v2,	m);
}
void			lua_float3::transformcoord(lua_float3& pV,	lua_matrix4x4& pM)
{
float3_transformcoord(this,	pV,	pM);
}
void			lua_float3::transformnormal(lua_float3& pV,	lua_matrix4x4& pM)
{
float3_transformnormal(this,	pV,	pM);
}
void			lua_float4::barycentric(lua_float4& pV1,	lua_float4& pV2,	lua_float4& pV3,	float f,	float g)
{
float4_barycentric(this,	pV1,	pV2,	pV3,	f,	g);
}
void			lua_float4::catmullrom(lua_float4& pV0,	lua_float4& pV1,	lua_float4& pV2,	lua_float4& pV3,	float s)
{
float4_catmullrom(this,	pV0,	pV1,	pV2,	pV3,	s);
}
void			lua_float4::copy(lua_float4& v1)
{
float4_copy(this,	v1);
}
float			lua_float4::dot(lua_float4& v2)
{
return float4_dot(this,	v2);
}
void			lua_float4::hermite(lua_float4& pV1,	lua_float4& pT1,	lua_float4& pV2,	lua_float4& pT2,	float s)
{
float4_hermite(this,	pV1,	pT1,	pV2,	pT2,	s);
}
float			lua_float4::length()
{
return float4_length(this);
}
void			lua_float4::lerp(lua_float4& v1,	lua_float4& v2,	float fLerp)
{
float4_lerp(this,	v1,	v2,	fLerp);
}
void			lua_float4::setup(float x,	float y,	float z,	float w)
{
float4_setup(this,	x,	y,	z,	w);
}
void			lua_float4::transform(lua_float3& pV,	lua_matrix4x4& pM)
{
float4_transform(this,	pV,	pM);
}
void			lua_matrix4x4::affineTransformation(float Scaling,	lua_float3& pRotationCenter,	lua_quaternion& pRotation,	lua_float3& pTranslation)
{
matrix4x4_affineTransformation(this,	Scaling,	pRotationCenter,	pRotation,	pTranslation);
}
void			lua_matrix4x4::affineTransformation2d(float Scaling,	lua_float2& pRotationCenter,	float Rotation,	lua_float2& pTranslation)
{
matrix4x4_affineTransformation2d(this,	Scaling,	pRotationCenter,	Rotation,	pTranslation);
}
bool			lua_matrix4x4::decompose(lua_float3& pOutScale,	lua_quaternion& pOutRotation,	lua_float3& pOutTranslation)
{
return (matrix4x4_decompose(pOutScale,	pOutRotation,	pOutTranslation,	this) != 0);
}
lua_float3			lua_matrix4x4::Getpos()
{
lua_float3 ret;
matrix4x4_Getpos(this,&ret);
return ret;
}
void			lua_matrix4x4::identity()
{
matrix4x4_identity(this);
}
void			lua_matrix4x4::inverse(lua_matrix4x4& pM)
{
matrix4x4_inverse(this,	pM);
}
void			lua_matrix4x4::inverse2(lua_float3& pDeterminant,	lua_matrix4x4& pM)
{
matrix4x4_inverse2(this,	pDeterminant,	pM);
}
void			lua_matrix4x4::lerp(lua_matrix4x4& v1,	lua_matrix4x4& v2,	float fLerp)
{
matrix4x4_lerp(this,	v1,	v2,	fLerp);
}
void			lua_matrix4x4::lookat_lh(lua_float3& pEye,	lua_float3& pAt,	lua_float3& pUp)
{
matrix4x4_lookat_lh(this,	pEye,	pAt,	pUp);
}
void			lua_matrix4x4::lookat_rh(lua_float3& pEye,	lua_float3& pAt,	lua_float3& pUp)
{
matrix4x4_lookat_rh(this,	pEye,	pAt,	pUp);
}
void			lua_matrix4x4::multiply_transpose(lua_matrix4x4& pM1,	lua_matrix4x4& pM2)
{
matrix4x4_multiply_transpose(this,	pM1,	pM2);
}
void			lua_matrix4x4::orth_offcenter_lh(float l,	float r,	float b,	float t,	lua_float2& znearfar)
{
matrix4x4_orth_offcenter_lh(this,	l,	r,	b,	t,	znearfar);
}
void			lua_matrix4x4::ortho_lh(float w,	float h,	float zn,	float zf)
{
matrix4x4_ortho_lh(this,	w,	h,	zn,	zf);
}
void			lua_matrix4x4::ortho_offcenter_rh(float l,	float r,	float b,	float t,	lua_float2& znearfar)
{
matrix4x4_ortho_offcenter_rh(this,	l,	r,	b,	t,	znearfar);
}
void			lua_matrix4x4::ortho_rh(float w,	float h,	float zn,	float zf)
{
matrix4x4_ortho_rh(this,	w,	h,	zn,	zf);
}
void			lua_matrix4x4::perspective_fovrh(float fovy,	float Aspect,	float zn,	float zf)
{
matrix4x4_perspective_fovrh(this,	fovy,	Aspect,	zn,	zf);
}
void			lua_matrix4x4::perspective_lv(float w,	float h,	float zn,	float zf)
{
matrix4x4_perspective_lv(this,	w,	h,	zn,	zf);
}
void			lua_matrix4x4::perspective_offcenter_lh(float l,	float r,	float b,	float t,	lua_float2& znearfar)
{
matrix4x4_perspective_offcenter_lh(this,	l,	r,	b,	t,	znearfar);
}
void			lua_matrix4x4::perspective_offcenter_rh(float l,	float r,	float b,	float t,	lua_float2& znearfar)
{
matrix4x4_perspective_offcenter_rh(this,	l,	r,	b,	t,	znearfar);
}
void			lua_matrix4x4::perspective_rh(float w,	float h,	float zn,	float zf)
{
matrix4x4_perspective_rh(this,	w,	h,	zn,	zf);
}
void			lua_matrix4x4::perspectivefov_lh(float fovy,	float Aspect,	float zn,	float zf)
{
matrix4x4_perspectivefov_lh(this,	fovy,	Aspect,	zn,	zf);
}
void			lua_matrix4x4::reflect(lua_Plane& pPlane)
{
matrix4x4_reflect(this,	pPlane);
}
void			lua_matrix4x4::rotation_yawpitchroll(float Yaw,	float Pitch,	float Roll)
{
matrix4x4_rotation_yawpitchroll(this,	Yaw,	Pitch,	Roll);
}
void			lua_matrix4x4::rotationaxis(lua_float3& pV,	float fDegress)
{
matrix4x4_rotationaxis(this,	pV,	fDegress);
}
void			lua_matrix4x4::rotationquaternion(lua_quaternion& pQ)
{
matrix4x4_rotationquaternion(this,	pQ);
}
void			lua_matrix4x4::rotationx(float fDegress)
{
matrix4x4_rotationx(this,	fDegress);
}
void			lua_matrix4x4::rotationy(float fDegress)
{
matrix4x4_rotationy(this,	fDegress);
}
void			lua_matrix4x4::rotationz(float fDegress)
{
matrix4x4_rotationz(this,	fDegress);
}
void			lua_matrix4x4::scaling(float sx,	float sy,	float sz)
{
matrix4x4_scaling(this,	sx,	sy,	sz);
}
void			lua_matrix4x4::Setpos(lua_float3& v)
{
matrix4x4_Setpos(this,	v);
}
void			lua_matrix4x4::transform(lua_float3& pos,	lua_quaternion& rot,	lua_float3& scale)
{
matrix4x4_transform(this,	pos,	rot,	scale);
}
void			lua_matrix4x4::translation(float x,	float y,	float z)
{
matrix4x4_translation(this,	x,	y,	z);
}
void			lua_matrix4x4::transpose(lua_matrix4x4& pOut)
{
matrix4x4_transpose(pOut,	this);
}
float			lua_Plane::dot(lua_float4& p2)
{
return Plane_dot(this,	p2);
}
float			lua_Plane::dotcoord(lua_float3& p2)
{
return Plane_dotcoord(this,	p2);
}
float			lua_Plane::dotnormal(lua_float3& p2)
{
return Plane_dotnormal(this,	p2);
}
void			lua_Plane::from_pointnormal(lua_float3& pPoint,	lua_float3& pNormal)
{
Plane_from_pointnormal(this,	pPoint,	pNormal);
}
void			lua_Plane::from_points(lua_float3& pV1,	lua_float3& pV2,	lua_float3& pV3)
{
Plane_from_points(this,	pV1,	pV2,	pV3);
}
void			lua_Plane::intersectline(lua_float3& r,	lua_float3& pV1,	lua_float3& pV2)
{
Plane_intersectline(r,	this,	pV1,	pV2);
}
void			lua_Plane::normalize(lua_Plane& pP)
{
Plane_normalize(this,	pP);
}
void			lua_Plane::scale(lua_Plane& p1,	float s)
{
Plane_scale(this,	p1,	s);
}
void			lua_Plane::transform(lua_Plane& pP,	lua_matrix4x4& pM)
{
Plane_transform(this,	pP,	pM);
}
void			lua_quaternion::barycentric(lua_quaternion& pQ1,	lua_quaternion& pQ2,	lua_quaternion& pQ3,	float f,	float g)
{
quaternion_barycentric(this,	pQ1,	pQ2,	pQ3,	f,	g);
}
void			lua_quaternion::exp(lua_quaternion& pQ)
{
quaternion_exp(this,	pQ);
}
void			lua_quaternion::inverse(lua_quaternion& pQ)
{
quaternion_inverse(this,	pQ);
}
void			lua_quaternion::ln(lua_quaternion& pQ)
{
quaternion_ln(this,	pQ);
}
void			lua_quaternion::multiply(lua_quaternion& pQ1,	lua_quaternion& pQ2)
{
quaternion_multiply(this,	pQ1,	pQ2);
}
void			lua_quaternion::normalize(lua_quaternion& pQ)
{
quaternion_normalize(this,	pQ);
}
void			lua_quaternion::rotation_yawpitchroll(float Yaw,	float Pitch,	float Roll)
{
quaternion_rotation_yawpitchroll(this,	Yaw,	Pitch,	Roll);
}
void			lua_quaternion::rotationaxis(lua_float3& pV,	float fDegress)
{
quaternion_rotationaxis(this,	pV,	fDegress);
}
void			lua_quaternion::rotationmatrix(lua_matrix4x4& pM)
{
quaternion_rotationmatrix(this,	pM);
}
void			lua_quaternion::slerp(lua_quaternion& pQ1,	lua_quaternion& pQ2,	float t)
{
quaternion_slerp(this,	pQ1,	pQ2,	t);
}
void			lua_quaternion::squad(lua_quaternion& pQ1,	lua_quaternion& pA,	lua_quaternion& pB,	lua_quaternion& pC,	float t)
{
quaternion_squad(this,	pQ1,	pA,	pB,	pC,	t);
}
lua_DataControl_s			lua_DataControl_s::add_child(const char* name)
{
lua_DataControl_s ret;
DataControl_add_child(&m_data,	name,&ret.m_data);
return ret;
}
lua_DataControl_s			lua_DataControl_s::at(INT index)
{
lua_DataControl_s ret;
DataControl_at(&m_data,	index,&ret.m_data);
return ret;
}
void			lua_DataControl_s::clear()
{
DataControl_clear(&m_data);
}
INT			lua_DataControl_s::count()
{
return DataControl_count(&m_data);
}
INT			lua_DataControl_s::Getlcount()
{
return DataControl_Getlcount(&m_data);
}
double			lua_DataControl_s::lreadf(INT index)
{
return DataControl_lreadf(&m_data,	index);
}
INT			lua_DataControl_s::lreadi(INT index)
{
return DataControl_lreadi(&m_data,	index);
}
const char*			lua_DataControl_s::lreadt(INT index)
{
return DataControl_lreadt(&m_data,	index);
}
void			lua_DataControl_s::lwritef(INT index,	double fValue)
{
DataControl_lwritef(&m_data,	index,	fValue);
}
void			lua_DataControl_s::lwritei(INT index,	INT nValue)
{
DataControl_lwritei(&m_data,	index,	nValue);
}
void			lua_DataControl_s::lwritet(INT index,	const char* szValue)
{
DataControl_lwritet(&m_data,	index,	szValue);
}
double			lua_DataControl_s::readf(const char* name)
{
return DataControl_readf(&m_data,	name);
}
INT			lua_DataControl_s::readi(const char* name)
{
return DataControl_readi(&m_data,	name);
}
const char*			lua_DataControl_s::readt(const char* name)
{
return DataControl_readt(&m_data,	name);
}
void			lua_DataControl_s::Setlcount(INT nCount)
{
DataControl_Setlcount(&m_data,	nCount);
}
void			lua_DataControl_s::writef(const char* name,	double fValue)
{
DataControl_writef(&m_data,	name,	fValue);
}
void			lua_DataControl_s::writei(const char* name,	INT nValue)
{
DataControl_writei(&m_data,	name,	nValue);
}
void			lua_DataControl_s::writet(const char* name,	const char* szValue)
{
DataControl_writet(&m_data,	name,	szValue);
}
bool			lua_FindWay_s::Getis2dway()
{
return (FindWay_Getis2dway(&m_data) != 0);
}
bool			lua_FindWay_s::Getpause()
{
return (FindWay_Getpause(&m_data) != 0);
}
bool			lua_FindWay_s::Getplay()
{
return (FindWay_Getplay(&m_data) != 0);
}
INT			lua_FindWay_s::Getpointcount()
{
return FindWay_Getpointcount(&m_data);
}
float			lua_FindWay_s::Getspeed()
{
return FindWay_Getspeed(&m_data);
}
lua_float3			lua_FindWay_s::point_at(INT nIndex)
{
lua_float3 ret;
FindWay_point_at(&m_data,	nIndex,&ret);
return ret;
}
bool			lua_FindWay_s::recalc(lua_float3& ret,	lua_floatPtr& retRotation)
{
return (FindWay_recalc(&m_data,	ret,	retRotation) != 0);
}
void			lua_FindWay_s::Setis2dway(bool bMode)
{
FindWay_Setis2dway(&m_data,	bMode);
}
void			lua_FindWay_s::Setpause(bool bPause)
{
FindWay_Setpause(&m_data,	bPause);
}
void			lua_FindWay_s::Setplay(bool bPlay)
{
FindWay_Setplay(&m_data,	bPlay);
}
void			lua_FindWay_s::setpoint_at(INT nIndex,	lua_float3& pos)
{
FindWay_setpoint_at(&m_data,	nIndex,	pos);
}
void			lua_FindWay_s::Setpointcount(INT nCount)
{
FindWay_Setpointcount(&m_data,	nCount);
}
void			lua_FindWay_s::Setspeed(float fSpeed)
{
FindWay_Setspeed(&m_data,	fSpeed);
}
void			lua_GameObject_s::action(INT/*ActionType*/ type,	float fTime)
{
GameObject_action(&m_data,	(ActionType)type,	fTime);
}
void			lua_GameObject_s::action2(INT/*ActionType*/ type,	INT nLoop)
{
GameObject_action2(&m_data,	(ActionType)type,	nLoop);
}
bool			lua_GameObject_s::attack(INT actionindex)
{
return (GameObject_attack(&m_data,	actionindex) != 0);
}
void			lua_GameObject_s::bind_action_event(const char* szLuaFunc)
{
GameObject_bind_action_event(&m_data,	szLuaFunc);
}
void			lua_GameObject_s::bind_action_over(const char* szLuaFunc)
{
GameObject_bind_action_over(&m_data,	szLuaFunc);
}
void			lua_GameObject_s::bind_attack_over(const char* szLuaFunc)
{
GameObject_bind_attack_over(&m_data,	szLuaFunc);
}
void			lua_GameObject_s::bind_die_over(const char* szLuaFunc)
{
GameObject_bind_die_over(&m_data,	szLuaFunc);
}
void			lua_GameObject_s::bind_hit_over(const char* szLuaFunc)
{
GameObject_bind_hit_over(&m_data,	szLuaFunc);
}
void			lua_GameObject_s::bind_idle_over(const char* szLuaFunc)
{
GameObject_bind_idle_over(&m_data,	szLuaFunc);
}
void			lua_GameObject_s::bind_move_over(const char* szLuaFunc)
{
GameObject_bind_move_over(&m_data,	szLuaFunc);
}
void			lua_GameObject_s::bind_nextbegin(const char* szLuaFunc)
{
GameObject_bind_nextbegin(&m_data,	szLuaFunc);
}
lua_GameObject_s			lua_GameObject_s::clone()
{
lua_GameObject_s ret;
GameObject_clone(&m_data,&ret.m_data);
return ret;
}
void			lua_GameObject_s::die()
{
GameObject_die(&m_data);
}
bool			lua_GameObject_s::doskill_r(INT skillid,	lua_float3& target)
{
return (GameObject_doskill_r(&m_data,	skillid,	target) != 0);
}
bool			lua_GameObject_s::doskill_t(INT skillid)
{
return (GameObject_doskill_t(&m_data,	skillid) != 0);
}
float			lua_GameObject_s::Getaffrange()
{
return GameObject_Getaffrange(&m_data);
}
float			lua_GameObject_s::Getatkrange()
{
return GameObject_Getatkrange(&m_data);
}
lua_float3			lua_GameObject_s::Getbornpos()
{
lua_float3 ret;
GameObject_Getbornpos(&m_data,&ret);
return ret;
}
bool			lua_GameObject_s::Gethit_text()
{
return (GameObject_Gethit_text(&m_data) != 0);
}
bool			lua_GameObject_s::Getmeshheight()
{
return (GameObject_Getmeshheight(&m_data) != 0);
}
bool			lua_GameObject_s::Getmovedir_enable()
{
return (GameObject_Getmovedir_enable(&m_data) != 0);
}
bool			lua_GameObject_s::Getmoverun()
{
return (GameObject_Getmoverun(&m_data) != 0);
}
bool			lua_GameObject_s::Getnextpause()
{
return (GameObject_Getnextpause(&m_data) != 0);
}
lua_float3			lua_GameObject_s::Getpos()
{
lua_float3 ret;
GameObject_Getpos(&m_data,&ret);
return ret;
}
float			lua_GameObject_s::Getrot()
{
return GameObject_Getrot(&m_data);
}
bool			lua_GameObject_s::Getsearchway()
{
return (GameObject_Getsearchway(&m_data) != 0);
}
lua_GameObject_s			lua_GameObject_s::Gettarget()
{
lua_GameObject_s ret;
GameObject_Gettarget(&m_data,&ret.m_data);
return ret;
}
float			lua_GameObject_s::height()
{
return GameObject_height(&m_data);
}
void			lua_GameObject_s::idle(float fTime)
{
GameObject_idle(&m_data,	fTime);
}
void			lua_GameObject_s::lookat(lua_float3& target)
{
GameObject_lookat(&m_data,	target);
}
void			lua_GameObject_s::lookat_t()
{
GameObject_lookat_t(&m_data);
}
void			lua_GameObject_s::movedir(lua_float3& dir)
{
GameObject_movedir(&m_data,	dir);
}
bool			lua_GameObject_s::moveto(lua_float3& pos)
{
return (GameObject_moveto(&m_data,	pos) != 0);
}
void			lua_GameObject_s::moveto_b()
{
GameObject_moveto_b(&m_data);
}
bool			lua_GameObject_s::moveto_e(const char* szEvent)
{
return (GameObject_moveto_e(&m_data,	szEvent) != 0);
}
bool			lua_GameObject_s::moveto_t()
{
return (GameObject_moveto_t(&m_data) != 0);
}
void			lua_GameObject_s::next_setid(INT index,	INT nid)
{
GameObject_next_setid(&m_data,	index,	nid);
}
void			lua_GameObject_s::next_setrandom(INT index,	float fRandom)
{
GameObject_next_setrandom(&m_data,	index,	fRandom);
}
INT			lua_GameObject_s::nextaction(INT/*ActionType*/ type,	float fTime)
{
return GameObject_nextaction(&m_data,	(ActionType)type,	fTime);
}
INT			lua_GameObject_s::nextaction2(INT/*ActionType*/ type,	INT nLoop)
{
return GameObject_nextaction2(&m_data,	(ActionType)type,	nLoop);
}
void			lua_GameObject_s::nextclear()
{
GameObject_nextclear(&m_data);
}
bool			lua_GameObject_s::nextenemy()
{
return (GameObject_nextenemy(&m_data) != 0);
}
INT			lua_GameObject_s::nextidle(float fTime)
{
return GameObject_nextidle(&m_data,	fTime);
}
INT			lua_GameObject_s::nextlookat(lua_float3& pos)
{
return GameObject_nextlookat(&m_data,	pos);
}
INT			lua_GameObject_s::nextlookat_t(lua_GameObject_s& obj)
{
return GameObject_nextlookat_t(&m_data,	&obj.m_data);
}
INT			lua_GameObject_s::nextloop(INT push_begin,	INT push_end,	INT push_loop)
{
return GameObject_nextloop(&m_data,	push_begin,	push_end,	push_loop);
}
INT			lua_GameObject_s::nextto(lua_float3& pos)
{
return GameObject_nextto(&m_data,	pos);
}
INT			lua_GameObject_s::nextto_e(const char* szEvent)
{
return GameObject_nextto_e(&m_data,	szEvent);
}
void			lua_GameObject_s::onhit(lua_GameObject_s& enemy,	INT val,	float fPlayTime)
{
GameObject_onhit(&m_data,	&enemy.m_data,	val,	fPlayTime);
}
lua_UIControl_s			lua_GameObject_s::onhit2(lua_GameObject_s& enemy,	INT val,	float fPlayTime)
{
lua_UIControl_s ret;
GameObject_onhit2(&m_data,	&enemy.m_data,	val,	fPlayTime,&ret.m_data);
return ret;
}
void			lua_GameObject_s::pause_t(float fSecond)
{
GameObject_pause_t(&m_data,	fSecond);
}
float			lua_GameObject_s::read(const char* szName)
{
return GameObject_read(&m_data,	szName);
}
INT			lua_GameObject_s::roleid()
{
return GameObject_roleid(&m_data);
}
INT			lua_GameObject_s::sceneid()
{
return GameObject_sceneid(&m_data);
}
lua_Map3DItem_s			lua_GameObject_s::sceneobj()
{
lua_Map3DItem_s ret;
GameObject_sceneobj(&m_data,&ret.m_data);
return ret;
}
void			lua_GameObject_s::Setaffrange(float affRange)
{
GameObject_Setaffrange(&m_data,	affRange);
}
void			lua_GameObject_s::Setatkrange(float atkRange)
{
GameObject_Setatkrange(&m_data,	atkRange);
}
void			lua_GameObject_s::Setbornpos(lua_float3& target)
{
GameObject_Setbornpos(&m_data,	target);
}
void			lua_GameObject_s::Sethit_text(bool bText)
{
GameObject_Sethit_text(&m_data,	bText);
}
void			lua_GameObject_s::Setmeshheight(bool bEnable)
{
GameObject_Setmeshheight(&m_data,	bEnable);
}
void			lua_GameObject_s::Setmovedir_enable(bool bRun)
{
GameObject_Setmovedir_enable(&m_data,	bRun);
}
void			lua_GameObject_s::Setmoverun(bool bRun)
{
GameObject_Setmoverun(&m_data,	bRun);
}
void			lua_GameObject_s::Setnextpause(bool bPause)
{
GameObject_Setnextpause(&m_data,	bPause);
}
void			lua_GameObject_s::Setpos(lua_float3& pos)
{
GameObject_Setpos(&m_data,	pos);
}
void			lua_GameObject_s::Setrot(float fRot)
{
GameObject_Setrot(&m_data,	fRot);
}
void			lua_GameObject_s::Setsearchway(bool bEnable)
{
GameObject_Setsearchway(&m_data,	bEnable);
}
void			lua_GameObject_s::Settarget(lua_GameObject_s& target)
{
GameObject_Settarget(&m_data,	&target.m_data);
}
void			lua_GameObject_s::update_blood(float fProgress)
{
GameObject_update_blood(&m_data,	fProgress);
}
void			lua_GameObject_s::write(const char* szName,	float nValue)
{
GameObject_write(&m_data,	szName,	nValue);
}
lua_Map3DItem_s			lua_Map3D_s::at(INT index)
{
lua_Map3DItem_s ret;
Map3D_at(&m_data,	index,&ret.m_data);
return ret;
}
void			lua_Map3D_s::bind_click(const char* szItemName,	const char* szLuaFunc)
{
Map3D_bind_click(&m_data,	szItemName,	szLuaFunc);
}
void			lua_Map3D_s::bind_draw(const char* szItemName,	lua_UIDrawMgr_s& drawPtr,	const char* szBoneName)
{
Map3D_bind_draw(&m_data,	szItemName,	&drawPtr.m_data,	szBoneName);
}
void			lua_Map3D_s::bind_event(INT/*EventID*/ nid,	const char* szItemName,	const char* szLuaFunc)
{
Map3D_bind_event(&m_data,	(EventID)nid,	szItemName,	szLuaFunc);
}
void			lua_Map3D_s::bind_eventby(const char* szEvent,	const char* szItemName,	const char* szLuaFunc)
{
Map3D_bind_eventby(&m_data,	szEvent,	szItemName,	szLuaFunc);
}
void			lua_Map3D_s::bind_mapevent(INT/*EventID*/ nid,	const char* szLuaFunc)
{
Map3D_bind_mapevent(&m_data,	(EventID)nid,	szLuaFunc);
}
void			lua_Map3D_s::bind_mapeventby(const char* szEvent,	const char* szLuaFunc)
{
Map3D_bind_mapeventby(&m_data,	szEvent,	szLuaFunc);
}
void			lua_Map3D_s::blend_camera(const char* szCamera,	float fBlendTime)
{
Map3D_blend_camera(&m_data,	szCamera,	fBlendTime);
}
void			lua_Map3D_s::blend_fog(const char* szCamera,	float fBlendTime)
{
Map3D_blend_fog(&m_data,	szCamera,	fBlendTime);
}
void			lua_Map3D_s::blend_sky(const char* szCamera,	float fBlendTime)
{
Map3D_blend_sky(&m_data,	szCamera,	fBlendTime);
}
void			lua_Map3D_s::camerafollow(lua_Map3DItem_s& obj)
{
Map3D_camerafollow(&m_data,	&obj.m_data);
}
lua_Map3DItem_s			lua_Map3D_s::classat(INT classID,	INT index)
{
lua_Map3DItem_s ret;
Map3D_classat(&m_data,	classID,	index,&ret.m_data);
return ret;
}
INT			lua_Map3D_s::classcount(INT classID)
{
return Map3D_classcount(&m_data,	classID);
}
INT			lua_Map3D_s::count()
{
return Map3D_count(&m_data);
}
void			lua_Map3D_s::enable(const char* szItemName,	bool bEnable)
{
Map3D_enable(&m_data,	szItemName,	bEnable);
}
void			lua_Map3D_s::enable_s(INT nID,	bool bEnable)
{
Map3D_enable_s(&m_data,	nID,	bEnable);
}
lua_Map3DEnv_s			lua_Map3D_s::envat(INT nIndex)
{
lua_Map3DEnv_s ret;
Map3D_envat(&m_data,	nIndex,&ret.m_data);
return ret;
}
INT			lua_Map3D_s::envcount()
{
return Map3D_envcount(&m_data);
}
lua_Map3DSkill_s			lua_Map3D_s::freeskill()
{
lua_Map3DSkill_s ret;
Map3D_freeskill(&m_data,&ret.m_data);
return ret;
}
const char*			lua_Map3D_s::Getactivecamera()
{
return Map3D_Getactivecamera(&m_data);
}
lua_Map3DEnv_s			lua_Map3D_s::GetactiveEnv()
{
lua_Map3DEnv_s ret;
Map3D_GetactiveEnv(&m_data,&ret.m_data);
return ret;
}
const char*			lua_Map3D_s::GetactiveEnvBy()
{
return Map3D_GetactiveEnvBy(&m_data);
}
bool			lua_Map3D_s::Getblooddisplay()
{
return (Map3D_Getblooddisplay(&m_data) != 0);
}
float			lua_Map3D_s::Getbloodsize()
{
return Map3D_Getbloodsize(&m_data);
}
lua_Pixel			lua_Map3D_s::Getcamera()
{
lua_Pixel ret;
Map3D_Getcamera(&m_data,&ret);
return ret;
}
INT			lua_Map3D_s::Getclickradius()
{
return Map3D_Getclickradius(&m_data);
}
bool			lua_Map3D_s::Getdisplay()
{
return (Map3D_Getdisplay(&m_data) != 0);
}
bool			lua_Map3D_s::Getdisplayterrain()
{
return (Map3D_Getdisplayterrain(&m_data) != 0);
}
bool			lua_Map3D_s::Getenabled()
{
return (Map3D_Getenabled(&m_data) != 0);
}
bool			lua_Map3D_s::Getenablemyclick()
{
return (Map3D_Getenablemyclick(&m_data) != 0);
}
bool			lua_Map3D_s::Getfindway2d()
{
return (Map3D_Getfindway2d(&m_data) != 0);
}
bool			lua_Map3D_s::Gethitdisplay()
{
return (Map3D_Gethitdisplay(&m_data) != 0);
}
bool			lua_Map3D_s::Getpause()
{
return (Map3D_Getpause(&m_data) != 0);
}
const char*			lua_Map3D_s::Getsky()
{
return Map3D_Getsky(&m_data);
}
lua_Map3DItem_s			lua_Map3D_s::insert(const char* szFileName,	lua_Pixel& mappos)
{
lua_Map3DItem_s ret;
Map3D_insert(&m_data,	szFileName,	mappos,&ret.m_data);
return ret;
}
void			lua_Map3D_s::remove(lua_Map3DItem_s& _item)
{
Map3D_remove(&m_data,	&_item.m_data);
}
void			lua_Map3D_s::replace_image(const char* szItemName,	const char* szImageSrc,	const char* szImageReplace)
{
Map3D_replace_image(&m_data,	szItemName,	szImageSrc,	szImageReplace);
}
lua_Pixel			lua_Map3D_s::scenepos(lua_Pixel& screen)
{
lua_Pixel ret;
Map3D_scenepos(&m_data,	screen,&ret);
return ret;
}
lua_Pixel			lua_Map3D_s::screenpos(lua_Pixel& scene)
{
lua_Pixel ret;
Map3D_screenpos(&m_data,	scene,&ret);
return ret;
}
lua_Map3DItem_s			lua_Map3D_s::search(INT cID)
{
lua_Map3DItem_s ret;
Map3D_search(&m_data,	cID,&ret.m_data);
return ret;
}
lua_Map3DItem_s			lua_Map3D_s::searchby(const char* szItemName)
{
lua_Map3DItem_s ret;
Map3D_searchby(&m_data,	szItemName,&ret.m_data);
return ret;
}
lua_Map3DPoint_s			lua_Map3D_s::searchpoint(INT id)
{
lua_Map3DPoint_s ret;
Map3D_searchpoint(&m_data,	id,&ret.m_data);
return ret;
}
lua_Map3DPoint_s			lua_Map3D_s::searchpointby(const char* szItemName)
{
lua_Map3DPoint_s ret;
Map3D_searchpointby(&m_data,	szItemName,&ret.m_data);
return ret;
}
lua_Map3DPoint_s			lua_Map3D_s::searchpointby_eid(INT eventid)
{
lua_Map3DPoint_s ret;
Map3D_searchpointby_eid(&m_data,	eventid,&ret.m_data);
return ret;
}
void			lua_Map3D_s::Setactivecamera(const char* szCamera)
{
Map3D_Setactivecamera(&m_data,	szCamera);
}
void			lua_Map3D_s::SetactiveEnv(lua_Map3DEnv_s& env)
{
Map3D_SetactiveEnv(&m_data,	&env.m_data);
}
void			lua_Map3D_s::SetactiveEnvBy(const char* szEnv)
{
Map3D_SetactiveEnvBy(&m_data,	szEnv);
}
bool			lua_Map3D_s::setblood(const char* szDlgName)
{
return (Map3D_setblood(&m_data,	szDlgName) != 0);
}
void			lua_Map3D_s::Setblooddisplay(bool bVisible)
{
Map3D_Setblooddisplay(&m_data,	bVisible);
}
void			lua_Map3D_s::Setbloodsize(float fSize)
{
Map3D_Setbloodsize(&m_data,	fSize);
}
void			lua_Map3D_s::Setcamera(lua_Pixel& pos)
{
Map3D_Setcamera(&m_data,	pos);
}
void			lua_Map3D_s::Setclickradius(INT radius)
{
Map3D_Setclickradius(&m_data,	radius);
}
void			lua_Map3D_s::Setdisplay(bool bVisible)
{
Map3D_Setdisplay(&m_data,	bVisible);
}
void			lua_Map3D_s::Setdisplayterrain(bool bVisible)
{
Map3D_Setdisplayterrain(&m_data,	bVisible);
}
void			lua_Map3D_s::Setenabled(bool bEnabled)
{
Map3D_Setenabled(&m_data,	bEnabled);
}
void			lua_Map3D_s::Setenablemyclick(bool bEnabled)
{
Map3D_Setenablemyclick(&m_data,	bEnabled);
}
void			lua_Map3D_s::Setfindway2d(bool b2DMode)
{
Map3D_Setfindway2d(&m_data,	b2DMode);
}
void			lua_Map3D_s::Sethitdisplay(bool bVisible)
{
Map3D_Sethitdisplay(&m_data,	bVisible);
}
void			lua_Map3D_s::Setpause(bool bPause)
{
Map3D_Setpause(&m_data,	bPause);
}
void			lua_Map3D_s::Setsky(const char* szCamera)
{
Map3D_Setsky(&m_data,	szCamera);
}
void			lua_Map3D_s::show(const char* szItemName,	bool bShow)
{
Map3D_show(&m_data,	szItemName,	bShow);
}
void			lua_Map3D_s::show_s(INT nID,	bool bShow)
{
Map3D_show_s(&m_data,	nID,	bShow);
}
ISkyPtr			lua_Map3D_s::skyat(INT nIndex)
{
ISkyPtr ret;
Map3D_skyat(&m_data,	nIndex,&ret);
return ret;
}
INT			lua_Map3D_s::skycount()
{
return Map3D_skycount(&m_data);
}
float			lua_Map3DEnv_s::Getbrightness()
{
return Map3DEnv_Getbrightness(&m_data);
}
lua_color4			lua_Map3DEnv_s::Getcolor()
{
lua_color4 ret;
Map3DEnv_Getcolor(&m_data,&ret);
return ret;
}
float			lua_Map3DEnv_s::Getcontrast()
{
return Map3DEnv_Getcontrast(&m_data);
}
bool			lua_Map3DEnv_s::Getenabled_brightness()
{
return (Map3DEnv_Getenabled_brightness(&m_data) != 0);
}
bool			lua_Map3DEnv_s::Getenabled_contrast()
{
return (Map3DEnv_Getenabled_contrast(&m_data) != 0);
}
float			lua_Map3DEnv_s::Getfar()
{
return Map3DEnv_Getfar(&m_data);
}
INT/*FogMode*/			lua_Map3DEnv_s::Getmode()
{
return (INT)Map3DEnv_Getmode(&m_data);
}
const char*			lua_Map3DEnv_s::Getname()
{
return Map3DEnv_Getname(&m_data);
}
float			lua_Map3DEnv_s::Getnear()
{
return Map3DEnv_Getnear(&m_data);
}
lua_float3			lua_Map3DEnv_s::Getshadow_center()
{
lua_float3 ret;
Map3DEnv_Getshadow_center(&m_data,&ret);
return ret;
}
lua_Map3DItem_s			lua_Map3DEnv_s::Getshadow_center_item()
{
lua_Map3DItem_s ret;
Map3DEnv_Getshadow_center_item(&m_data,&ret.m_data);
return ret;
}
float			lua_Map3DEnv_s::Getshadow_size()
{
return Map3DEnv_Getshadow_size(&m_data);
}
void			lua_Map3DEnv_s::Setbrightness(float fBrightness)
{
Map3DEnv_Setbrightness(&m_data,	fBrightness);
}
void			lua_Map3DEnv_s::Setcolor(lua_color4& c)
{
Map3DEnv_Setcolor(&m_data,	c);
}
void			lua_Map3DEnv_s::Setcontrast(float fContrast)
{
Map3DEnv_Setcontrast(&m_data,	fContrast);
}
void			lua_Map3DEnv_s::Setenabled_brightness(bool bEnabled)
{
Map3DEnv_Setenabled_brightness(&m_data,	bEnabled);
}
void			lua_Map3DEnv_s::Setenabled_contrast(bool bEnabled)
{
Map3DEnv_Setenabled_contrast(&m_data,	bEnabled);
}
void			lua_Map3DEnv_s::Setfar(float c)
{
Map3DEnv_Setfar(&m_data,	c);
}
void			lua_Map3DEnv_s::Setmode(INT/*FogMode*/ mode)
{
Map3DEnv_Setmode(&m_data,	(FogMode)mode);
}
void			lua_Map3DEnv_s::Setname(const char* szName)
{
Map3DEnv_Setname(&m_data,	szName);
}
void			lua_Map3DEnv_s::Setnear(float c)
{
Map3DEnv_Setnear(&m_data,	c);
}
void			lua_Map3DEnv_s::Setshadow_center(lua_float3& pos)
{
Map3DEnv_Setshadow_center(&m_data,	pos);
}
void			lua_Map3DEnv_s::Setshadow_center_item(lua_Map3DItem_s& obj)
{
Map3DEnv_Setshadow_center_item(&m_data,	&obj.m_data);
}
void			lua_Map3DEnv_s::Setshadow_size(float fSize)
{
Map3DEnv_Setshadow_size(&m_data,	fSize);
}
void			lua_Map3DItem_s::addhit(INT val)
{
Map3DItem_addhit(&m_data,	val);
}
void			lua_Map3DItem_s::bind_action_begin(const char* szLuaBegin)
{
Map3DItem_bind_action_begin(&m_data,	szLuaBegin);
}
void			lua_Map3DItem_s::bind_action_end(const char* szLuaEnd)
{
Map3DItem_bind_action_end(&m_data,	szLuaEnd);
}
void			lua_Map3DItem_s::bind_click(const char* szLuaFunc)
{
Map3DItem_bind_click(&m_data,	szLuaFunc);
}
void			lua_Map3DItem_s::bind_draw(lua_UIDrawMgr_s& drawPtr,	const char* szBindBone)
{
Map3DItem_bind_draw(&m_data,	&drawPtr.m_data,	szBindBone);
}
void			lua_Map3DItem_s::bind_event(INT/*EventID*/ nEvent,	const char* szLuaFunc)
{
Map3DItem_bind_event(&m_data,	(EventID)nEvent,	szLuaFunc);
}
void			lua_Map3DItem_s::bind_eventby(const char* szEvent,	const char* szLuaFunc)
{
Map3DItem_bind_eventby(&m_data,	szEvent,	szLuaFunc);
}
bool			lua_Map3DItem_s::bind_mesh(const char* szBoneName,	lua_Mesh_s& mesh)
{
return (Map3DItem_bind_mesh(&m_data,	szBoneName,	&mesh.m_data) != 0);
}
lua_Map3DItem_s			lua_Map3DItem_s::clone()
{
lua_Map3DItem_s ret;
Map3DItem_clone(&m_data,&ret.m_data);
return ret;
}
lua_Map3DItem_s			lua_Map3DItem_s::deleteme()
{
lua_Map3DItem_s ret;
Map3DItem_deleteme(&m_data,&ret.m_data);
return ret;
}
lua_GameObject_s			lua_Map3DItem_s::gameobj()
{
lua_GameObject_s ret;
Map3DItem_gameobj(&m_data,&ret.m_data);
return ret;
}
INT/*ActionType*/			lua_Map3DItem_s::Getaction()
{
return (INT)Map3DItem_Getaction(&m_data);
}
const char*			lua_Map3DItem_s::Getactionby()
{
return Map3DItem_Getactionby(&m_data);
}
float			lua_Map3DItem_s::Getaspeed()
{
return Map3DItem_Getaspeed(&m_data);
}
float			lua_Map3DItem_s::Getblood()
{
return Map3DItem_Getblood(&m_data);
}
bool			lua_Map3DItem_s::Getblood_display()
{
return (Map3DItem_Getblood_display(&m_data) != 0);
}
lua_color4			lua_Map3DItem_s::Getbloodcolor()
{
lua_color4 ret;
Map3DItem_Getbloodcolor(&m_data,&ret);
return ret;
}
INT			lua_Map3DItem_s::Getclassid()
{
return Map3DItem_Getclassid(&m_data);
}
bool			lua_Map3DItem_s::Getdisplay()
{
return (Map3DItem_Getdisplay(&m_data) != 0);
}
bool			lua_Map3DItem_s::Getenabled()
{
return (Map3DItem_Getenabled(&m_data) != 0);
}
float			lua_Map3DItem_s::Getheight()
{
return Map3DItem_Getheight(&m_data);
}
bool			lua_Map3DItem_s::Getloop()
{
return (Map3DItem_Getloop(&m_data) != 0);
}
const char*			lua_Map3DItem_s::Getname()
{
return Map3DItem_Getname(&m_data);
}
bool			lua_Map3DItem_s::Getname_display()
{
return (Map3DItem_Getname_display(&m_data) != 0);
}
float			lua_Map3DItem_s::Getopacity()
{
return Map3DItem_Getopacity(&m_data);
}
lua_Map3DItem_s			lua_Map3DItem_s::Getparent()
{
lua_Map3DItem_s ret;
Map3DItem_Getparent(&m_data,&ret.m_data);
return ret;
}
bool			lua_Map3DItem_s::Getpause()
{
return (Map3DItem_Getpause(&m_data) != 0);
}
bool			lua_Map3DItem_s::Getplay()
{
return (Map3DItem_Getplay(&m_data) != 0);
}
lua_Pixel			lua_Map3DItem_s::Getpos()
{
lua_Pixel ret;
Map3DItem_Getpos(&m_data,&ret);
return ret;
}
lua_float3			lua_Map3DItem_s::Getpos3d()
{
lua_float3 ret;
Map3DItem_Getpos3d(&m_data,&ret);
return ret;
}
float			lua_Map3DItem_s::Getrot()
{
return Map3DItem_Getrot(&m_data);
}
float			lua_Map3DItem_s::Getscale()
{
return Map3DItem_Getscale(&m_data);
}
float			lua_Map3DItem_s::Getspeed()
{
return Map3DItem_Getspeed(&m_data);
}
lua_matrix4x4			lua_Map3DItem_s::Getworldmat()
{
lua_matrix4x4 ret;
Map3DItem_Getworldmat(&m_data,&ret);
return ret;
}
INT			lua_Map3DItem_s::id()
{
return Map3DItem_id(&m_data);
}
bool			lua_Map3DItem_s::isgameobj()
{
return (Map3DItem_isgameobj(&m_data) != 0);
}
bool			lua_Map3DItem_s::issceneobj()
{
return (Map3DItem_issceneobj(&m_data) != 0);
}
void			lua_Map3DItem_s::lookat(lua_Pixel& m)
{
Map3DItem_lookat(&m_data,	m);
}
lua_Mesh_s			lua_Map3DItem_s::mesh()
{
lua_Mesh_s ret;
Map3DItem_mesh(&m_data,&ret.m_data);
return ret;
}
void			lua_Map3DItem_s::movedir(lua_Pixel& dir)
{
Map3DItem_movedir(&m_data,	dir);
}
void			lua_Map3DItem_s::moveto(lua_Pixel& pos,	const char* szFunc)
{
Map3DItem_moveto(&m_data,	pos,	szFunc);
}
void			lua_Map3DItem_s::pushaction(INT/*ActionType*/ type)
{
Map3DItem_pushaction(&m_data,	(ActionType)type);
}
void			lua_Map3DItem_s::pushactionby(const char* szName)
{
Map3DItem_pushactionby(&m_data,	szName);
}
void			lua_Map3DItem_s::replace_image(const char* szImageSrc,	const char* szImageReplace)
{
Map3DItem_replace_image(&m_data,	szImageSrc,	szImageReplace);
}
void			lua_Map3DItem_s::Setaction(INT/*ActionType*/ type)
{
Map3DItem_Setaction(&m_data,	(ActionType)type);
}
void			lua_Map3DItem_s::Setactionby(const char* szName)
{
Map3DItem_Setactionby(&m_data,	szName);
}
void			lua_Map3DItem_s::Setaspeed(float fSpeed)
{
Map3DItem_Setaspeed(&m_data,	fSpeed);
}
void			lua_Map3DItem_s::Setblood(float fValue)
{
Map3DItem_Setblood(&m_data,	fValue);
}
void			lua_Map3DItem_s::Setblood_display(bool bShow)
{
Map3DItem_Setblood_display(&m_data,	bShow);
}
void			lua_Map3DItem_s::Setbloodcolor(lua_color4& c)
{
Map3DItem_Setbloodcolor(&m_data,	c);
}
void			lua_Map3DItem_s::Setclassid(INT nClass)
{
Map3DItem_Setclassid(&m_data,	nClass);
}
void			lua_Map3DItem_s::Setdisplay(bool bVisible)
{
Map3DItem_Setdisplay(&m_data,	bVisible);
}
void			lua_Map3DItem_s::Setenabled(bool bVisible)
{
Map3DItem_Setenabled(&m_data,	bVisible);
}
void			lua_Map3DItem_s::Setheight(float fFly)
{
Map3DItem_Setheight(&m_data,	fFly);
}
void			lua_Map3DItem_s::Setloop(INT nLoop)
{
Map3DItem_Setloop(&m_data,	nLoop);
}
void			lua_Map3DItem_s::Setname(const char* szName)
{
Map3DItem_Setname(&m_data,	szName);
}
void			lua_Map3DItem_s::Setname_display(bool bShow)
{
Map3DItem_Setname_display(&m_data,	bShow);
}
void			lua_Map3DItem_s::Setopacity(float fAlpha)
{
Map3DItem_Setopacity(&m_data,	fAlpha);
}
void			lua_Map3DItem_s::Setparent(lua_Map3DItem_s& c)
{
Map3DItem_Setparent(&m_data,	&c.m_data);
}
void			lua_Map3DItem_s::Setpause(bool b)
{
Map3DItem_Setpause(&m_data,	b);
}
void			lua_Map3DItem_s::Setplay(bool b)
{
Map3DItem_Setplay(&m_data,	b);
}
void			lua_Map3DItem_s::Setpos(lua_Pixel& m)
{
Map3DItem_Setpos(&m_data,	m);
}
void			lua_Map3DItem_s::Setpos3d(lua_float3& m)
{
Map3DItem_Setpos3d(&m_data,	m);
}
void			lua_Map3DItem_s::Setrot(float fRotZ)
{
Map3DItem_Setrot(&m_data,	fRotZ);
}
void			lua_Map3DItem_s::Setscale(float fScale)
{
Map3DItem_Setscale(&m_data,	fScale);
}
void			lua_Map3DItem_s::Setspeed(float fSpeed)
{
Map3DItem_Setspeed(&m_data,	fSpeed);
}
void			lua_Map3DItem_s::Setworldmat(lua_matrix4x4& m)
{
Map3DItem_Setworldmat(&m_data,	m);
}
void			lua_Map3DItem_s::stopmove()
{
Map3DItem_stopmove(&m_data);
}
bool			lua_Map3DItem_s::unbind_mesh(lua_Mesh_s& mesh)
{
return (Map3DItem_unbind_mesh(&m_data,	&mesh.m_data) != 0);
}
const char*			lua_Map3DPoint_s::Getname()
{
return Map3DPoint_Getname(&m_data);
}
lua_float3			lua_Map3DPoint_s::Getpos()
{
lua_float3 ret;
Map3DPoint_Getpos(&m_data,&ret);
return ret;
}
float			lua_Map3DPoint_s::Getrange()
{
return Map3DPoint_Getrange(&m_data);
}
INT			lua_Map3DPoint_s::id()
{
return Map3DPoint_id(&m_data);
}
void			lua_Map3DPoint_s::Setname(const char* szName)
{
Map3DPoint_Setname(&m_data,	szName);
}
void			lua_Map3DPoint_s::Setpos(lua_float3& mappos)
{
Map3DPoint_Setpos(&m_data,	mappos);
}
void			lua_Map3DPoint_s::Setrange(float range)
{
Map3DPoint_Setrange(&m_data,	range);
}
void			lua_Map3DSkill_s::bind_collision(INT classid,	const char* szLuaFunc)
{
Map3DSkill_bind_collision(&m_data,	classid,	szLuaFunc);
}
float			lua_Map3DSkill_s::Getcontinuetime()
{
return Map3DSkill_Getcontinuetime(&m_data);
}
float			lua_Map3DSkill_s::Getmovespeed()
{
return Map3DSkill_Getmovespeed(&m_data);
}
float			lua_Map3DSkill_s::Getsize()
{
return Map3DSkill_Getsize(&m_data);
}
lua_Map3DItem_s			lua_Map3DSkill_s::getsource()
{
lua_Map3DItem_s ret;
Map3DSkill_getsource(&m_data,&ret.m_data);
return ret;
}
float			lua_Map3DSkill_s::Getsource_zscale()
{
return Map3DSkill_Getsource_zscale(&m_data);
}
lua_float3			lua_Map3DSkill_s::Getsourcepos()
{
lua_float3 ret;
Map3DSkill_Getsourcepos(&m_data,&ret);
return ret;
}
lua_Map3DItem_s			lua_Map3DSkill_s::gettarget()
{
lua_Map3DItem_s ret;
Map3DSkill_gettarget(&m_data,&ret.m_data);
return ret;
}
float			lua_Map3DSkill_s::Gettarget_zscale()
{
return Map3DSkill_Gettarget_zscale(&m_data);
}
lua_float3			lua_Map3DSkill_s::Gettargetpos()
{
lua_float3 ret;
Map3DSkill_Gettargetpos(&m_data,&ret);
return ret;
}
float			lua_Map3DSkill_s::Getwaittime()
{
return Map3DSkill_Getwaittime(&m_data);
}
void			lua_Map3DSkill_s::Setcontinuetime(float ftime)
{
Map3DSkill_Setcontinuetime(&m_data,	ftime);
}
void			lua_Map3DSkill_s::Setmovespeed(float speed)
{
Map3DSkill_Setmovespeed(&m_data,	speed);
}
void			lua_Map3DSkill_s::Setsize(float fScale)
{
Map3DSkill_Setsize(&m_data,	fScale);
}
void			lua_Map3DSkill_s::Setsource_zscale(float fScale)
{
Map3DSkill_Setsource_zscale(&m_data,	fScale);
}
void			lua_Map3DSkill_s::Setsourcepos(lua_float3& pos)
{
Map3DSkill_Setsourcepos(&m_data,	pos);
}
void			lua_Map3DSkill_s::Settarget_zscale(float fScale)
{
Map3DSkill_Settarget_zscale(&m_data,	fScale);
}
void			lua_Map3DSkill_s::Settargetpos(lua_float3& pos)
{
Map3DSkill_Settargetpos(&m_data,	pos);
}
void			lua_Map3DSkill_s::Setwaittime(float fwaittime)
{
Map3DSkill_Setwaittime(&m_data,	fwaittime);
}
void			lua_Map3DSkill_s::source(const char* szSkillMesh,	lua_Map3DItem_s& item,	const char* szBoneLike)
{
Map3DSkill_source(&m_data,	szSkillMesh,	&item.m_data,	szBoneLike);
}
void			lua_Map3DSkill_s::start(const char* szFinishFunc)
{
Map3DSkill_start(&m_data,	szFinishFunc);
}
void			lua_Map3DSkill_s::stop()
{
Map3DSkill_stop(&m_data);
}
void			lua_Map3DSkill_s::target(lua_Map3DItem_s& item,	const char* szBoneLike)
{
Map3DSkill_target(&m_data,	&item.m_data,	szBoneLike);
}
lua_AABox			lua_Mesh_s::aabox()
{
lua_AABox ret;
Mesh_aabox(&m_data,&ret);
return ret;
}
void			lua_Mesh_s::bind_draw(lua_UIDrawMgr_s& drawPtr,	const char* szBoneName)
{
Mesh_bind_draw(&m_data,	&drawPtr.m_data,	szBoneName);
}
bool			lua_Mesh_s::bind_mesh(const char* szBoneName,	lua_Mesh_s& mesh)
{
return (Mesh_bind_mesh(&m_data,	szBoneName,	&mesh.m_data) != 0);
}
INT			lua_Mesh_s::bonecount()
{
return Mesh_bonecount(&m_data);
}
lua_matrix4x4			lua_Mesh_s::bonemat_at(INT index)
{
lua_matrix4x4 ret;
Mesh_bonemat_at(&m_data,	index,&ret);
return ret;
}
const char*			lua_Mesh_s::bonename_at(INT index)
{
return Mesh_bonename_at(&m_data,	index);
}
void			lua_Mesh_s::clearParent()
{
Mesh_clearParent(&m_data);
}
lua_Mesh_s			lua_Mesh_s::clone()
{
lua_Mesh_s ret;
Mesh_clone(&m_data,&ret.m_data);
return ret;
}
INT/*ActionType*/			lua_Mesh_s::Getaction()
{
return (INT)Mesh_Getaction(&m_data);
}
const char*			lua_Mesh_s::Getactionby()
{
return Mesh_Getactionby(&m_data);
}
lua_color4			lua_Mesh_s::Getcolor()
{
lua_color4 ret;
Mesh_Getcolor(&m_data,&ret);
return ret;
}
INT			lua_Mesh_s::Getdefaultaction()
{
return Mesh_Getdefaultaction(&m_data);
}
INT/*BlendMode*/			lua_Mesh_s::Getdestblend()
{
return (INT)Mesh_Getdestblend(&m_data);
}
bool			lua_Mesh_s::Getdisplay()
{
return (Mesh_Getdisplay(&m_data) != 0);
}
bool			lua_Mesh_s::Getenabled()
{
return (Mesh_Getenabled(&m_data) != 0);
}
const char*			lua_Mesh_s::getfilepath()
{
return Mesh_getfilepath(&m_data);
}
bool			lua_Mesh_s::Getfollow_camera()
{
return (Mesh_Getfollow_camera(&m_data) != 0);
}
float			lua_Mesh_s::Getopacity()
{
return Mesh_Getopacity(&m_data);
}
lua_Mesh_s			lua_Mesh_s::Getparent()
{
lua_Mesh_s ret;
Mesh_Getparent(&m_data,&ret.m_data);
return ret;
}
bool			lua_Mesh_s::Getpause()
{
return (Mesh_Getpause(&m_data) != 0);
}
bool			lua_Mesh_s::Getplay()
{
return (Mesh_Getplay(&m_data) != 0);
}
lua_float3			lua_Mesh_s::Getpos()
{
lua_float3 ret;
Mesh_Getpos(&m_data,&ret);
return ret;
}
lua_float3			lua_Mesh_s::Getrot()
{
lua_float3 ret;
Mesh_Getrot(&m_data,&ret);
return ret;
}
bool			lua_Mesh_s::Gets_lodhide_onexceed()
{
return (Mesh_Gets_lodhide_onexceed(&m_data) != 0);
}
float			lua_Mesh_s::Gets_lodinterval_length()
{
return Mesh_Gets_lodinterval_length(&m_data);
}
float			lua_Mesh_s::Gets_lodmaxvisible()
{
return Mesh_Gets_lodmaxvisible(&m_data);
}
lua_float3			lua_Mesh_s::Getscale()
{
lua_float3 ret;
Mesh_Getscale(&m_data,&ret);
return ret;
}
lua_color4			lua_Mesh_s::Getselfluminous()
{
lua_color4 ret;
Mesh_Getselfluminous(&m_data,&ret);
return ret;
}
lua_color4			lua_Mesh_s::Getspecular()
{
lua_color4 ret;
Mesh_Getspecular(&m_data,&ret);
return ret;
}
float			lua_Mesh_s::Getspeed()
{
return Mesh_Getspeed(&m_data);
}
INT/*BlendMode*/			lua_Mesh_s::Getsrcblend()
{
return (INT)Mesh_Getsrcblend(&m_data);
}
lua_matrix4x4			lua_Mesh_s::Getworldmat()
{
lua_matrix4x4 ret;
Mesh_Getworldmat(&m_data,&ret);
return ret;
}
float			lua_Mesh_s::height()
{
return Mesh_height(&m_data);
}
bool			lua_Mesh_s::isskeleton()
{
return (Mesh_isskeleton(&m_data) != 0);
}
bool			lua_Mesh_s::isunwalkable(lua_float3& pos)
{
return (Mesh_isunwalkable(&m_data,	pos) != 0);
}
void			lua_Mesh_s::lookat(lua_float3& m)
{
Mesh_lookat(&m_data,	m);
}
void			lua_Mesh_s::playaction(INT keyBegin,	INT keyEnd,	INT nLoop)
{
Mesh_playaction(&m_data,	keyBegin,	keyEnd,	nLoop);
}
bool			lua_Mesh_s::ray(lua_Pixel& pixel)
{
return (Mesh_ray(&m_data,	pixel) != 0);
}
bool			lua_Mesh_s::ray_dir(lua_float3& orig,	lua_float3& dir)
{
return (Mesh_ray_dir(&m_data,	orig,	dir) != 0);
}
void			lua_Mesh_s::replace_image(const char* szImageSrc,	const char* szImageReplace)
{
Mesh_replace_image(&m_data,	szImageSrc,	szImageReplace);
}
lua_MeshAction_s			lua_Mesh_s::s_action_at(INT index)
{
lua_MeshAction_s ret;
Mesh_s_action_at(&m_data,	index,&ret.m_data);
return ret;
}
INT			lua_Mesh_s::s_actioncount()
{
return Mesh_s_actioncount(&m_data);
}
INT			lua_Mesh_s::searchboneid(const char* szName)
{
return Mesh_searchboneid(&m_data,	szName);
}
INT			lua_Mesh_s::searchboneid_like(const char* szName)
{
return Mesh_searchboneid_like(&m_data,	szName);
}
void			lua_Mesh_s::Setaction(INT/*ActionType*/ t)
{
Mesh_Setaction(&m_data,	(ActionType)t);
}
void			lua_Mesh_s::Setactionby(const char* szName)
{
Mesh_Setactionby(&m_data,	szName);
}
void			lua_Mesh_s::Setcolor(lua_color4& c)
{
Mesh_Setcolor(&m_data,	c);
}
void			lua_Mesh_s::Setdefaultaction(INT actionID)
{
Mesh_Setdefaultaction(&m_data,	actionID);
}
void			lua_Mesh_s::Setdestblend(INT/*BlendMode*/ blend)
{
Mesh_Setdestblend(&m_data,	(BlendMode)blend);
}
void			lua_Mesh_s::Setdisplay(bool b)
{
Mesh_Setdisplay(&m_data,	b);
}
void			lua_Mesh_s::Setenabled(bool b)
{
Mesh_Setenabled(&m_data,	b);
}
void			lua_Mesh_s::Setfollow_camera(bool b)
{
Mesh_Setfollow_camera(&m_data,	b);
}
void			lua_Mesh_s::Setopacity(float fAlpha)
{
Mesh_Setopacity(&m_data,	fAlpha);
}
void			lua_Mesh_s::Setparent(lua_Mesh_s& mesh)
{
Mesh_Setparent(&m_data,	&mesh.m_data);
}
void			lua_Mesh_s::Setpause(bool b)
{
Mesh_Setpause(&m_data,	b);
}
void			lua_Mesh_s::Setplay(bool b)
{
Mesh_Setplay(&m_data,	b);
}
void			lua_Mesh_s::Setpos(lua_float3& pos)
{
Mesh_Setpos(&m_data,	pos);
}
void			lua_Mesh_s::Setrot(lua_float3& rot)
{
Mesh_Setrot(&m_data,	rot);
}
void			lua_Mesh_s::Sets_lodhide_onexceed(bool b)
{
Mesh_Sets_lodhide_onexceed(&m_data,	b);
}
void			lua_Mesh_s::Sets_lodinterval_length(float f)
{
Mesh_Sets_lodinterval_length(&m_data,	f);
}
void			lua_Mesh_s::Sets_lodmaxvisible(float f)
{
Mesh_Sets_lodmaxvisible(&m_data,	f);
}
void			lua_Mesh_s::Setscale(lua_float3& scale)
{
Mesh_Setscale(&m_data,	scale);
}
void			lua_Mesh_s::Setselfluminous(lua_color4& c)
{
Mesh_Setselfluminous(&m_data,	c);
}
void			lua_Mesh_s::Setspecular(lua_color4& c)
{
Mesh_Setspecular(&m_data,	c);
}
void			lua_Mesh_s::Setspeed(float fSpeed)
{
Mesh_Setspeed(&m_data,	fSpeed);
}
void			lua_Mesh_s::Setsrcblend(INT/*BlendMode*/ blend)
{
Mesh_Setsrcblend(&m_data,	(BlendMode)blend);
}
void			lua_Mesh_s::Setworldmat(lua_matrix4x4& m)
{
Mesh_Setworldmat(&m_data,	m);
}
lua_SubMesh_s			lua_Mesh_s::submesh_at(INT index)
{
lua_SubMesh_s ret;
Mesh_submesh_at(&m_data,	index,&ret.m_data);
return ret;
}
INT			lua_Mesh_s::submeshcount()
{
return Mesh_submeshcount(&m_data);
}
void			lua_Mesh_s::unbind_mesh(lua_Mesh_s& mesh)
{
Mesh_unbind_mesh(&m_data,	&mesh.m_data);
}
INT			lua_MeshAction_s::Getkeybegin()
{
return MeshAction_Getkeybegin(&m_data);
}
INT			lua_MeshAction_s::Getkeyend()
{
return MeshAction_Getkeyend(&m_data);
}
float			lua_MeshAction_s::Getlerptime()
{
return MeshAction_Getlerptime(&m_data);
}
float			lua_MeshAction_s::Getspeed()
{
return MeshAction_Getspeed(&m_data);
}
INT			lua_MeshAction_s::keycount()
{
return MeshAction_keycount(&m_data);
}
void			lua_MeshAction_s::Setkeybegin(INT beginIndex)
{
MeshAction_Setkeybegin(&m_data,	beginIndex);
}
void			lua_MeshAction_s::Setkeyend(INT endIndex)
{
MeshAction_Setkeyend(&m_data,	endIndex);
}
void			lua_MeshAction_s::Setlerptime(float f)
{
MeshAction_Setlerptime(&m_data,	f);
}
void			lua_MeshAction_s::Setspeed(float fSpeed)
{
MeshAction_Setspeed(&m_data,	fSpeed);
}
lua_color4			lua_MeshMaterial_s::Getcolor()
{
lua_color4 ret;
MeshMaterial_Getcolor(&m_data,&ret);
return ret;
}
INT/*CullMode*/			lua_MeshMaterial_s::Getcullmode()
{
return (INT)MeshMaterial_Getcullmode(&m_data);
}
INT/*BlendMode*/			lua_MeshMaterial_s::Getdestblend()
{
return (INT)MeshMaterial_Getdestblend(&m_data);
}
lua_Texture_s			lua_MeshMaterial_s::Getnormal_texture()
{
lua_Texture_s ret;
MeshMaterial_Getnormal_texture(&m_data,&ret.m_data);
return ret;
}
INT/*AlphaMode*/			lua_MeshMaterial_s::Getopacitymode()
{
return (INT)MeshMaterial_Getopacitymode(&m_data);
}
lua_Texture_s			lua_MeshMaterial_s::Getreflect_texture()
{
lua_Texture_s ret;
MeshMaterial_Getreflect_texture(&m_data,&ret.m_data);
return ret;
}
lua_color4			lua_MeshMaterial_s::Getselfluminous()
{
lua_color4 ret;
MeshMaterial_Getselfluminous(&m_data,&ret);
return ret;
}
lua_color4			lua_MeshMaterial_s::Getspecular()
{
lua_color4 ret;
MeshMaterial_Getspecular(&m_data,&ret);
return ret;
}
lua_Texture_s			lua_MeshMaterial_s::Getspecular_texture()
{
lua_Texture_s ret;
MeshMaterial_Getspecular_texture(&m_data,&ret.m_data);
return ret;
}
INT/*BlendMode*/			lua_MeshMaterial_s::Getsrcblend()
{
return (INT)MeshMaterial_Getsrcblend(&m_data);
}
lua_Texture_s			lua_MeshMaterial_s::Gettexture()
{
lua_Texture_s ret;
MeshMaterial_Gettexture(&m_data,&ret.m_data);
return ret;
}
void			lua_MeshMaterial_s::Setcolor(lua_color4& c)
{
MeshMaterial_Setcolor(&m_data,	c);
}
void			lua_MeshMaterial_s::Setcullmode(INT/*CullMode*/ cull)
{
MeshMaterial_Setcullmode(&m_data,	(CullMode)cull);
}
void			lua_MeshMaterial_s::Setdestblend(INT/*BlendMode*/ blend)
{
MeshMaterial_Setdestblend(&m_data,	(BlendMode)blend);
}
void			lua_MeshMaterial_s::Setnormal_texture(lua_Texture_s& texturePtr)
{
MeshMaterial_Setnormal_texture(&m_data,	&texturePtr.m_data);
}
void			lua_MeshMaterial_s::Setopacitymode(INT/*AlphaMode*/ alpha)
{
MeshMaterial_Setopacitymode(&m_data,	(AlphaMode)alpha);
}
void			lua_MeshMaterial_s::Setreflect_texture(lua_Texture_s& texturePtr)
{
MeshMaterial_Setreflect_texture(&m_data,	&texturePtr.m_data);
}
void			lua_MeshMaterial_s::Setselfluminous(lua_color4& c)
{
MeshMaterial_Setselfluminous(&m_data,	c);
}
void			lua_MeshMaterial_s::Setspecular(lua_color4& c)
{
MeshMaterial_Setspecular(&m_data,	c);
}
void			lua_MeshMaterial_s::Setspecular_texture(lua_Texture_s& texturePtr)
{
MeshMaterial_Setspecular_texture(&m_data,	&texturePtr.m_data);
}
void			lua_MeshMaterial_s::Setsrcblend(INT/*BlendMode*/ blend)
{
MeshMaterial_Setsrcblend(&m_data,	(BlendMode)blend);
}
void			lua_MeshMaterial_s::Settexture(lua_Texture_s& texturePtr)
{
MeshMaterial_Settexture(&m_data,	&texturePtr.m_data);
}
bool			lua_SubMesh_s::Getdisplay()
{
return (SubMesh_Getdisplay(&m_data) != 0);
}
const char*			lua_SubMesh_s::getname()
{
return SubMesh_getname(&m_data);
}
lua_matrix4x4			lua_SubMesh_s::Getworldmat()
{
lua_matrix4x4 ret;
SubMesh_Getworldmat(&m_data,&ret);
return ret;
}
bool			lua_SubMesh_s::isskeleton()
{
return (SubMesh_isskeleton(&m_data) != 0);
}
bool			lua_SubMesh_s::lod_exists()
{
return (SubMesh_lod_exists(&m_data) != 0);
}
lua_MeshMaterial_s			lua_SubMesh_s::material_at(INT index)
{
lua_MeshMaterial_s ret;
SubMesh_material_at(&m_data,	index,&ret.m_data);
return ret;
}
INT			lua_SubMesh_s::materialcount()
{
return SubMesh_materialcount(&m_data);
}
void			lua_SubMesh_s::Setdisplay(bool b)
{
SubMesh_Setdisplay(&m_data,	b);
}
void			lua_SubMesh_s::Setworldmat(lua_matrix4x4& mat)
{
SubMesh_Setworldmat(&m_data,	mat);
}
lua_Triangle			lua_SubMesh_s::triangle_at(INT nIndex)
{
lua_Triangle ret;
SubMesh_triangle_at(&m_data,	nIndex,&ret);
return ret;
}
INT			lua_SubMesh_s::trianglecount()
{
return SubMesh_trianglecount(&m_data);
}
lua_Vertex			lua_SubMesh_s::vertex_at(INT nIndex)
{
lua_Vertex ret;
SubMesh_vertex_at(&m_data,	nIndex,&ret);
return ret;
}
INT			lua_SubMesh_s::vertexcount()
{
return SubMesh_vertexcount(&m_data);
}
float			lua_Table_s::id_f(INT id,	const char* colName)
{
return Table_id_f(&m_data,	id,	colName);
}
INT			lua_Table_s::id_i(INT id,	const char* colName)
{
return Table_id_i(&m_data,	id,	colName);
}
const char*			lua_Table_s::id_t(INT id,	const char* colName)
{
return Table_id_t(&m_data,	id,	colName);
}
INT			lua_Table_s::linecount()
{
return Table_linecount(&m_data);
}
float			lua_Table_s::name_f(const char* szName,	const char* colName)
{
return Table_name_f(&m_data,	szName,	colName);
}
INT			lua_Table_s::name_i(const char* szName,	const char* colName)
{
return Table_name_i(&m_data,	szName,	colName);
}
const char*			lua_Table_s::name_t(const char* szName,	const char* colName)
{
return Table_name_t(&m_data,	szName,	colName);
}
float			lua_Table_s::read_f(INT row,	INT col)
{
return Table_read_f(&m_data,	row,	col);
}
INT			lua_Table_s::read_i(INT row,	INT col)
{
return Table_read_i(&m_data,	row,	col);
}
const char*			lua_Table_s::read_t(INT row,	INT col)
{
return Table_read_t(&m_data,	row,	col);
}
INT			lua_Table_s::search_col(const char* colName)
{
return Table_search_col(&m_data,	colName);
}
float			lua_Table_s::search_f(const char* st_colName,	const char* st_rowValue,	const char* colName)
{
return Table_search_f(&m_data,	st_colName,	st_rowValue,	colName);
}
INT			lua_Table_s::search_i(const char* st_colName,	const char* st_rowValue,	const char* colName)
{
return Table_search_i(&m_data,	st_colName,	st_rowValue,	colName);
}
const char*			lua_Table_s::search_t(const char* st_colName,	const char* st_rowValue,	const char* colName)
{
return Table_search_t(&m_data,	st_colName,	st_rowValue,	colName);
}
bool			lua_Texture_s::commit(INT/*TextureStage*/ samplerIndex)
{
return (Texture_commit(&m_data,	(TextureStage)samplerIndex) != 0);
}
const char*			lua_Texture_s::getfilepath()
{
return Texture_getfilepath(&m_data);
}
INT			lua_Texture_s::getheight()
{
return Texture_getheight(&m_data);
}
INT			lua_Texture_s::getwidth()
{
return Texture_getwidth(&m_data);
}
void			lua_UIControl_s::bind_3d_clear()
{
UIControl_bind_3d_clear(&m_data);
}
void			lua_UIControl_s::bind_3d_maxdist(float fMaxDist)
{
UIControl_bind_3d_maxdist(&m_data,	fMaxDist);
}
void			lua_UIControl_s::bind_3d_obj(lua_GameObject_s& obj,	float fHeightAdd)
{
UIControl_bind_3d_obj(&m_data,	&obj.m_data,	fHeightAdd);
}
void			lua_UIControl_s::bind_3d_pos(lua_float3& pos)
{
UIControl_bind_3d_pos(&m_data,	pos);
}
void			lua_UIControl_s::bind_click(const char* szLuaFunc)
{
UIControl_bind_click(&m_data,	szLuaFunc);
}
void			lua_UIControl_s::bind_draw(lua_UIDrawMgr_s& drawPtr)
{
UIControl_bind_draw(&m_data,	&drawPtr.m_data);
}
void			lua_UIControl_s::bind_event(INT/*EventID*/ nid,	const char* szLuaFunc)
{
UIControl_bind_event(&m_data,	(EventID)nid,	szLuaFunc);
}
void			lua_UIControl_s::bind_eventby(const char* szEvent,	const char* szLuaFunc)
{
UIControl_bind_eventby(&m_data,	szEvent,	szLuaFunc);
}
void			lua_UIControl_s::bind_image(const char* szImageFile)
{
UIControl_bind_image(&m_data,	szImageFile);
}
lua_UIControl_s			lua_UIControl_s::clone(float fTime)
{
lua_UIControl_s ret;
UIControl_clone(&m_data,	fTime,&ret.m_data);
return ret;
}
lua_UIControl_s			lua_UIControl_s::cloneto(float fTime,	lua_UIDialog_s& dest)
{
lua_UIControl_s ret;
UIControl_cloneto(&m_data,	fTime,	&dest.m_data,&ret.m_data);
return ret;
}
lua_UIDialog_s			lua_UIControl_s::dialog()
{
lua_UIDialog_s ret;
UIControl_dialog(&m_data,&ret.m_data);
return ret;
}
lua_UIElement_s			lua_UIControl_s::element_at(INT index)
{
lua_UIElement_s ret;
UIControl_element_at(&m_data,	index,&ret.m_data);
return ret;
}
INT			lua_UIControl_s::elementcount()
{
return UIControl_elementcount(&m_data);
}
INT			lua_UIControl_s::framemove(const char* szLuaFunc,	INT nMaxMillisecond)
{
return UIControl_framemove(&m_data,	szLuaFunc,	nMaxMillisecond);
}
INT			lua_UIControl_s::framemove2(const char* szLuaFunc,	INT nMaxMillisecond,	const char* param)
{
return UIControl_framemove2(&m_data,	szLuaFunc,	nMaxMillisecond,	param);
}
INT/*ActionType*/			lua_UIControl_s::Getaction()
{
return (INT)UIControl_Getaction(&m_data);
}
const char*			lua_UIControl_s::Getactionby()
{
return UIControl_Getactionby(&m_data);
}
float			lua_UIControl_s::Getactiontime()
{
return UIControl_Getactiontime(&m_data);
}
bool			lua_UIControl_s::Getactive()
{
return (UIControl_Getactive(&m_data) != 0);
}
INT			lua_UIControl_s::Getbottom()
{
return UIControl_Getbottom(&m_data);
}
lua_Pixel			lua_UIControl_s::Getcenter()
{
lua_Pixel ret;
UIControl_Getcenter(&m_data,&ret);
return ret;
}
lua_color4			lua_UIControl_s::Getcolor()
{
lua_color4 ret;
UIControl_Getcolor(&m_data,&ret);
return ret;
}
bool			lua_UIControl_s::Getdisplay()
{
return (UIControl_Getdisplay(&m_data) != 0);
}
INT			lua_UIControl_s::Geteffect_loop()
{
return UIControl_Geteffect_loop(&m_data);
}
bool			lua_UIControl_s::Geteffect_stop()
{
return (UIControl_Geteffect_stop(&m_data) != 0);
}
bool			lua_UIControl_s::Getenabled()
{
return (UIControl_Getenabled(&m_data) != 0);
}
const char*			lua_UIControl_s::geteventsound(INT/*UISoundPlay*/ st)
{
return UIControl_geteventsound(&m_data,	(UISoundPlay)st);
}
lua_UIGroup_s			lua_UIControl_s::Getgroup()
{
lua_UIGroup_s ret;
UIControl_Getgroup(&m_data,&ret.m_data);
return ret;
}
INT			lua_UIControl_s::Getgroupid()
{
return UIControl_Getgroupid(&m_data);
}
const char*			lua_UIControl_s::Getgroupname()
{
return UIControl_Getgroupname(&m_data);
}
INT			lua_UIControl_s::Getheight()
{
return UIControl_Getheight(&m_data);
}
INT			lua_UIControl_s::getId()
{
return UIControl_getId(&m_data);
}
const char*			lua_UIControl_s::Getimage()
{
return UIControl_Getimage(&m_data);
}
lua_color4			lua_UIControl_s::Getimagecolor()
{
lua_color4 ret;
UIControl_Getimagecolor(&m_data,&ret);
return ret;
}
lua_Mesh_s			lua_UIControl_s::Getinnermesh()
{
lua_Mesh_s ret;
UIControl_Getinnermesh(&m_data,&ret.m_data);
return ret;
}
INT			lua_UIControl_s::Getlayer()
{
return UIControl_Getlayer(&m_data);
}
INT			lua_UIControl_s::Getleft()
{
return UIControl_Getleft(&m_data);
}
INT			lua_UIControl_s::Getmovebottom()
{
return UIControl_Getmovebottom(&m_data);
}
bool			lua_UIControl_s::Getmovedialog()
{
return (UIControl_Getmovedialog(&m_data) != 0);
}
INT			lua_UIControl_s::Getmoveleft()
{
return UIControl_Getmoveleft(&m_data);
}
INT			lua_UIControl_s::Getmoveright()
{
return UIControl_Getmoveright(&m_data);
}
INT			lua_UIControl_s::Getmovetop()
{
return UIControl_Getmovetop(&m_data);
}
const char*			lua_UIControl_s::Getname()
{
return UIControl_Getname(&m_data);
}
INT/*ActionType*/			lua_UIControl_s::Getnextaction()
{
return (INT)UIControl_Getnextaction(&m_data);
}
float			lua_UIControl_s::Getopacity()
{
return UIControl_Getopacity(&m_data);
}
bool			lua_UIControl_s::Getpixelpick()
{
return (UIControl_Getpixelpick(&m_data) != 0);
}
lua_Pixel			lua_UIControl_s::Getpos()
{
lua_Pixel ret;
UIControl_Getpos(&m_data,&ret);
return ret;
}
lua_Rect			lua_UIControl_s::Getrect()
{
lua_Rect ret;
UIControl_Getrect(&m_data,&ret);
return ret;
}
INT			lua_UIControl_s::Getright()
{
return UIControl_Getright(&m_data);
}
float			lua_UIControl_s::Getrot()
{
return UIControl_Getrot(&m_data);
}
float			lua_UIControl_s::Getrot2d()
{
return UIControl_Getrot2d(&m_data);
}
lua_Pixel			lua_UIControl_s::Getrotcenter()
{
lua_Pixel ret;
UIControl_Getrotcenter(&m_data,&ret);
return ret;
}
bool			lua_UIControl_s::Getrotenable()
{
return (UIControl_Getrotenable(&m_data) != 0);
}
float			lua_UIControl_s::Getscale()
{
return UIControl_Getscale(&m_data);
}
float			lua_UIControl_s::Getscalex()
{
return UIControl_Getscalex(&m_data);
}
float			lua_UIControl_s::Getscaley()
{
return UIControl_Getscaley(&m_data);
}
bool			lua_UIControl_s::Getscroll_extend()
{
return (UIControl_Getscroll_extend(&m_data) != 0);
}
float			lua_UIControl_s::Getscroll_firstspeed()
{
return UIControl_Getscroll_firstspeed(&m_data);
}
lua_UIControl_s			lua_UIControl_s::Getscroll_h()
{
lua_UIControl_s ret;
UIControl_Getscroll_h(&m_data,&ret.m_data);
return ret;
}
INT			lua_UIControl_s::Getscroll_max()
{
return UIControl_Getscroll_max(&m_data);
}
INT			lua_UIControl_s::Getscroll_min()
{
return UIControl_Getscroll_min(&m_data);
}
float			lua_UIControl_s::Getscroll_speed()
{
return UIControl_Getscroll_speed(&m_data);
}
lua_UIControl_s			lua_UIControl_s::Getscroll_v()
{
lua_UIControl_s ret;
UIControl_Getscroll_v(&m_data,&ret.m_data);
return ret;
}
float			lua_UIControl_s::Getspeed()
{
return UIControl_Getspeed(&m_data);
}
const char*			lua_UIControl_s::Gettext()
{
return UIControl_Gettext(&m_data);
}
INT/*DrawTextAlign*/			lua_UIControl_s::Gettextalign()
{
return (INT)UIControl_Gettextalign(&m_data);
}
lua_color4			lua_UIControl_s::Gettextcolor()
{
lua_color4 ret;
UIControl_Gettextcolor(&m_data,&ret);
return ret;
}
INT			lua_UIControl_s::Gettop()
{
return UIControl_Gettop(&m_data);
}
INT/*UIControlType*/			lua_UIControl_s::gettype()
{
return (INT)UIControl_gettype(&m_data);
}
float			lua_UIControl_s::Getvalf()
{
return UIControl_Getvalf(&m_data);
}
INT			lua_UIControl_s::Getvali()
{
return UIControl_Getvali(&m_data);
}
const char*			lua_UIControl_s::Getvalt()
{
return UIControl_Getvalt(&m_data);
}
float			lua_UIControl_s::Getwaitbefore_actionplay()
{
return UIControl_Getwaitbefore_actionplay(&m_data);
}
INT			lua_UIControl_s::Getwidth()
{
return UIControl_Getwidth(&m_data);
}
void			lua_UIControl_s::innermesh_clear()
{
UIControl_innermesh_clear(&m_data);
}
INT			lua_UIControl_s::interval(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond)
{
return UIControl_interval(&m_data,	szLuaFunc,	nMillisecond,	nMaxMillisecond);
}
INT			lua_UIControl_s::interval2(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond,	const char* param)
{
return UIControl_interval2(&m_data,	szLuaFunc,	nMillisecond,	nMaxMillisecond,	param);
}
bool			lua_UIControl_s::isfocus()
{
return (UIControl_isfocus(&m_data) != 0);
}
lua_Pixel			lua_UIControl_s::mousepoint()
{
lua_Pixel ret;
UIControl_mousepoint(&m_data,&ret);
return ret;
}
bool			lua_UIControl_s::pick(INT x,	INT y)
{
return (UIControl_pick(&m_data,	x,	y) != 0);
}
void			lua_UIControl_s::progress_time(float fTimeSecond)
{
UIControl_progress_time(&m_data,	fTimeSecond);
}
void			lua_UIControl_s::removeChild()
{
UIControl_removeChild(&m_data);
}
void			lua_UIControl_s::reset_trans()
{
UIControl_reset_trans(&m_data);
}
void			lua_UIControl_s::rot2d_dir(lua_float2& dir)
{
UIControl_rot2d_dir(&m_data,	dir);
}
lua_Pixel			lua_UIControl_s::saved_pos()
{
lua_Pixel ret;
UIControl_saved_pos(&m_data,&ret);
return ret;
}
lua_Rect			lua_UIControl_s::saved_rect()
{
lua_Rect ret;
UIControl_saved_rect(&m_data,&ret);
return ret;
}
float			lua_UIControl_s::saved_rot()
{
return UIControl_saved_rot(&m_data);
}
void			lua_UIControl_s::Setaction(INT/*ActionType*/ nType)
{
UIControl_Setaction(&m_data,	(ActionType)nType);
}
void			lua_UIControl_s::Setactionby(const char* ani)
{
UIControl_Setactionby(&m_data,	ani);
}
void			lua_UIControl_s::Setactiontime(float f)
{
UIControl_Setactiontime(&m_data,	f);
}
void			lua_UIControl_s::Setactive(bool b)
{
UIControl_Setactive(&m_data,	b);
}
void			lua_UIControl_s::Setbottom(INT nValue)
{
UIControl_Setbottom(&m_data,	nValue);
}
void			lua_UIControl_s::Setcenter(lua_Pixel& pos)
{
UIControl_Setcenter(&m_data,	pos);
}
void			lua_UIControl_s::setChild(lua_UIDialog_s& dlg)
{
UIControl_setChild(&m_data,	&dlg.m_data);
}
void			lua_UIControl_s::Setcolor(lua_color4& c)
{
UIControl_Setcolor(&m_data,	c);
}
void			lua_UIControl_s::Setdisplay(bool b)
{
UIControl_Setdisplay(&m_data,	b);
}
void			lua_UIControl_s::Seteffect_loop(INT nLoop)
{
UIControl_Seteffect_loop(&m_data,	nLoop);
}
void			lua_UIControl_s::Seteffect_stop(bool bStop)
{
UIControl_Seteffect_stop(&m_data,	bStop);
}
void			lua_UIControl_s::Setenabled(bool b)
{
UIControl_Setenabled(&m_data,	b);
}
void			lua_UIControl_s::seteventsound(INT/*UISoundPlay*/ st,	const char* szFile)
{
UIControl_seteventsound(&m_data,	(UISoundPlay)st,	szFile);
}
void			lua_UIControl_s::Setgroup(lua_UIGroup_s& g)
{
UIControl_Setgroup(&m_data,	&g.m_data);
}
void			lua_UIControl_s::Setgroupid(INT nID)
{
UIControl_Setgroupid(&m_data,	nID);
}
void			lua_UIControl_s::Setgroupname(const char* szName)
{
UIControl_Setgroupname(&m_data,	szName);
}
void			lua_UIControl_s::Setheight(INT nValue)
{
UIControl_Setheight(&m_data,	nValue);
}
void			lua_UIControl_s::Setimage(const char* szFile)
{
UIControl_Setimage(&m_data,	szFile);
}
void			lua_UIControl_s::Setimagecolor(lua_color4& c)
{
UIControl_Setimagecolor(&m_data,	c);
}
void			lua_UIControl_s::Setinnermesh(lua_Mesh_s& mesh)
{
UIControl_Setinnermesh(&m_data,	&mesh.m_data);
}
void			lua_UIControl_s::Setlayer(INT layer)
{
UIControl_Setlayer(&m_data,	layer);
}
void			lua_UIControl_s::Setleft(INT nValue)
{
UIControl_Setleft(&m_data,	nValue);
}
void			lua_UIControl_s::Setmovebottom(INT nValue)
{
UIControl_Setmovebottom(&m_data,	nValue);
}
void			lua_UIControl_s::Setmovedialog(bool b)
{
UIControl_Setmovedialog(&m_data,	b);
}
void			lua_UIControl_s::Setmoveleft(INT nValue)
{
UIControl_Setmoveleft(&m_data,	nValue);
}
void			lua_UIControl_s::Setmoveright(INT nValue)
{
UIControl_Setmoveright(&m_data,	nValue);
}
void			lua_UIControl_s::Setmovetop(INT nValue)
{
UIControl_Setmovetop(&m_data,	nValue);
}
void			lua_UIControl_s::Setname(const char* szName)
{
UIControl_Setname(&m_data,	szName);
}
void			lua_UIControl_s::Setnextaction(INT/*ActionType*/ nType)
{
UIControl_Setnextaction(&m_data,	(ActionType)nType);
}
void			lua_UIControl_s::Setopacity(float v)
{
UIControl_Setopacity(&m_data,	v);
}
void			lua_UIControl_s::Setpixelpick(bool b)
{
UIControl_Setpixelpick(&m_data,	b);
}
void			lua_UIControl_s::Setpos(lua_Pixel& pos)
{
UIControl_Setpos(&m_data,	pos);
}
void			lua_UIControl_s::Setrect(lua_Rect& rc)
{
UIControl_Setrect(&m_data,	rc);
}
void			lua_UIControl_s::Setright(INT nValue)
{
UIControl_Setright(&m_data,	nValue);
}
void			lua_UIControl_s::Setrot(float fDegress)
{
UIControl_Setrot(&m_data,	fDegress);
}
void			lua_UIControl_s::Setrot2d(float fDegress)
{
UIControl_Setrot2d(&m_data,	fDegress);
}
void			lua_UIControl_s::Setrotcenter(lua_Pixel& v)
{
UIControl_Setrotcenter(&m_data,	v);
}
void			lua_UIControl_s::Setrotenable(bool bEnable)
{
UIControl_Setrotenable(&m_data,	bEnable);
}
void			lua_UIControl_s::Setscale(float fScale)
{
UIControl_Setscale(&m_data,	fScale);
}
void			lua_UIControl_s::Setscalex(float fScale)
{
UIControl_Setscalex(&m_data,	fScale);
}
void			lua_UIControl_s::Setscaley(float fScale)
{
UIControl_Setscaley(&m_data,	fScale);
}
void			lua_UIControl_s::Setscroll_extend(bool bExtend)
{
UIControl_Setscroll_extend(&m_data,	bExtend);
}
void			lua_UIControl_s::Setscroll_firstspeed(float fSpeed)
{
UIControl_Setscroll_firstspeed(&m_data,	fSpeed);
}
void			lua_UIControl_s::Setscroll_h(lua_UIControl_s& scrollH)
{
UIControl_Setscroll_h(&m_data,	&scrollH.m_data);
}
void			lua_UIControl_s::Setscroll_max(INT nMax)
{
UIControl_Setscroll_max(&m_data,	nMax);
}
void			lua_UIControl_s::Setscroll_min(INT nMin)
{
UIControl_Setscroll_min(&m_data,	nMin);
}
void			lua_UIControl_s::Setscroll_speed(float fSpeed)
{
UIControl_Setscroll_speed(&m_data,	fSpeed);
}
void			lua_UIControl_s::Setscroll_v(lua_UIControl_s& scrollV)
{
UIControl_Setscroll_v(&m_data,	&scrollV.m_data);
}
void			lua_UIControl_s::Setspeed(float fSpeed)
{
UIControl_Setspeed(&m_data,	fSpeed);
}
void			lua_UIControl_s::Settext(const char* szText)
{
UIControl_Settext(&m_data,	szText);
}
void			lua_UIControl_s::Settextalign(INT/*DrawTextAlign*/ fmt)
{
UIControl_Settextalign(&m_data,	(DrawTextAlign)fmt);
}
void			lua_UIControl_s::Settextcolor(lua_color4& c)
{
UIControl_Settextcolor(&m_data,	c);
}
void			lua_UIControl_s::Settop(INT nValue)
{
UIControl_Settop(&m_data,	nValue);
}
void			lua_UIControl_s::Setvalf(float fValue)
{
UIControl_Setvalf(&m_data,	fValue);
}
void			lua_UIControl_s::Setvali(INT nValue)
{
UIControl_Setvali(&m_data,	nValue);
}
void			lua_UIControl_s::Setvalt(const char* szValue)
{
UIControl_Setvalt(&m_data,	szValue);
}
void			lua_UIControl_s::Setwaitbefore_actionplay(float fTime)
{
UIControl_Setwaitbefore_actionplay(&m_data,	fTime);
}
void			lua_UIControl_s::Setwidth(INT nValue)
{
UIControl_Setwidth(&m_data,	nValue);
}
void			lua_UIControl_s::sort_draws()
{
UIControl_sort_draws(&m_data);
}
INT			lua_UIControl_s::timeout(const char* szLuaFunc,	INT nMillisecond)
{
return UIControl_timeout(&m_data,	szLuaFunc,	nMillisecond);
}
INT			lua_UIControl_s::timeout2(const char* szLuaFunc,	INT nMillisecond,	const char* param)
{
return UIControl_timeout2(&m_data,	szLuaFunc,	nMillisecond,	param);
}
lua_UIControl_s			lua_UIDialog_s::at(INT index)
{
lua_UIControl_s ret;
UIDialog_at(&m_data,	index,&ret.m_data);
return ret;
}
void			lua_UIDialog_s::bind_click(const char* szControl,	const char* szLuaFunc)
{
UIDialog_bind_click(&m_data,	szControl,	szLuaFunc);
}
void			lua_UIDialog_s::bind_draw(const char* szControl,	lua_UIDrawMgr_s& drawPtr)
{
UIDialog_bind_draw(&m_data,	szControl,	&drawPtr.m_data);
}
void			lua_UIDialog_s::bind_event(const char* szControl,	INT/*EventID*/ nid,	const char* szLuaFunc)
{
UIDialog_bind_event(&m_data,	szControl,	(EventID)nid,	szLuaFunc);
}
void			lua_UIDialog_s::bind_eventby(const char* szControl,	const char* szEvent,	const char* szLuaFunc)
{
UIDialog_bind_eventby(&m_data,	szControl,	szEvent,	szLuaFunc);
}
void			lua_UIDialog_s::bind_image(const char* szControl,	const char* szImageFile)
{
UIDialog_bind_image(&m_data,	szControl,	szImageFile);
}
INT			lua_UIDialog_s::count()
{
return UIDialog_count(&m_data);
}
lua_UIControl_s			lua_UIDialog_s::create(INT/*UIControlType*/ type,	lua_Rect& rc)
{
lua_UIControl_s ret;
UIDialog_create(&m_data,	(UIControlType)type,	rc,&ret.m_data);
return ret;
}
void			lua_UIDialog_s::delete_at(INT index)
{
UIDialog_delete_at(&m_data,	index);
}
void			lua_UIDialog_s::deleteall()
{
UIDialog_deleteall(&m_data);
}
void			lua_UIDialog_s::deleteby(lua_UIControl_s& control)
{
UIDialog_deleteby(&m_data,	&control.m_data);
}
bool			lua_UIDialog_s::destroy()
{
return (UIDialog_destroy(&m_data) != 0);
}
void			lua_UIDialog_s::enable(const char* szControl,	bool bEnable)
{
UIDialog_enable(&m_data,	szControl,	bEnable);
}
INT			lua_UIDialog_s::framemove(const char* szLuaFunc,	INT nMaxMillisecond)
{
return UIDialog_framemove(&m_data,	szLuaFunc,	nMaxMillisecond);
}
INT			lua_UIDialog_s::framemove2(const char* szLuaFunc,	INT nMaxMillisecond,	const char* param)
{
return UIDialog_framemove2(&m_data,	szLuaFunc,	nMaxMillisecond,	param);
}
lua_UIGroup_s			lua_UIDialog_s::gat(INT nIndex)
{
lua_UIGroup_s ret;
UIDialog_gat(&m_data,	nIndex,&ret.m_data);
return ret;
}
INT			lua_UIDialog_s::gcount()
{
return UIDialog_gcount(&m_data);
}
lua_UIGroup_s			lua_UIDialog_s::gcreate()
{
lua_UIGroup_s ret;
UIDialog_gcreate(&m_data,&ret.m_data);
return ret;
}
void			lua_UIDialog_s::gdelete(lua_UIGroup_s& group)
{
UIDialog_gdelete(&m_data,	&group.m_data);
}
INT/*ActionType*/			lua_UIDialog_s::Getaction()
{
return (INT)UIDialog_Getaction(&m_data);
}
const char*			lua_UIDialog_s::Getactionby()
{
return UIDialog_Getactionby(&m_data);
}
lua_Pixel			lua_UIDialog_s::Getcenter()
{
lua_Pixel ret;
UIDialog_Getcenter(&m_data,&ret);
return ret;
}
lua_color4			lua_UIDialog_s::Getcolor()
{
lua_color4 ret;
UIDialog_Getcolor(&m_data,&ret);
return ret;
}
bool			lua_UIDialog_s::Getdisplay()
{
return (UIDialog_Getdisplay(&m_data) != 0);
}
bool			lua_UIDialog_s::Getdomodal()
{
return (UIDialog_Getdomodal(&m_data) != 0);
}
bool			lua_UIDialog_s::Getenabled()
{
return (UIDialog_Getenabled(&m_data) != 0);
}
INT			lua_UIDialog_s::Getheight()
{
return UIDialog_Getheight(&m_data);
}
bool			lua_UIDialog_s::Gethideon_killfocus()
{
return (UIDialog_Gethideon_killfocus(&m_data) != 0);
}
INT			lua_UIDialog_s::Getlayer()
{
return UIDialog_Getlayer(&m_data);
}
INT			lua_UIDialog_s::Getleft()
{
return UIDialog_Getleft(&m_data);
}
const char*			lua_UIDialog_s::Getname()
{
return UIDialog_Getname(&m_data);
}
float			lua_UIDialog_s::Getopacity()
{
return UIDialog_Getopacity(&m_data);
}
bool			lua_UIDialog_s::Getpause()
{
return (UIDialog_Getpause(&m_data) != 0);
}
lua_Pixel			lua_UIDialog_s::Getpos()
{
lua_Pixel ret;
UIDialog_Getpos(&m_data,&ret);
return ret;
}
lua_Rect			lua_UIDialog_s::Getrect()
{
lua_Rect ret;
UIDialog_Getrect(&m_data,&ret);
return ret;
}
float			lua_UIDialog_s::Getrot2d()
{
return UIDialog_Getrot2d(&m_data);
}
float			lua_UIDialog_s::Getrotx()
{
return UIDialog_Getrotx(&m_data);
}
float			lua_UIDialog_s::Getroty()
{
return UIDialog_Getroty(&m_data);
}
float			lua_UIDialog_s::Getrotz()
{
return UIDialog_Getrotz(&m_data);
}
float			lua_UIDialog_s::Getscale()
{
return UIDialog_Getscale(&m_data);
}
float			lua_UIDialog_s::Getscalex()
{
return UIDialog_Getscalex(&m_data);
}
float			lua_UIDialog_s::Getscaley()
{
return UIDialog_Getscaley(&m_data);
}
INT			lua_UIDialog_s::Gettop()
{
return UIDialog_Gettop(&m_data);
}
bool			lua_UIDialog_s::Gettopmost()
{
return (UIDialog_Gettopmost(&m_data) != 0);
}
float			lua_UIDialog_s::getvalf(const char* szControl)
{
return UIDialog_getvalf(&m_data,	szControl);
}
INT			lua_UIDialog_s::getvali(const char* szControl)
{
return UIDialog_getvali(&m_data,	szControl);
}
const char*			lua_UIDialog_s::getvalt(const char* szControl)
{
return UIDialog_getvalt(&m_data,	szControl);
}
INT			lua_UIDialog_s::Getwidth()
{
return UIDialog_Getwidth(&m_data);
}
lua_UIGroup_s			lua_UIDialog_s::gsearch(const char* szName)
{
lua_UIGroup_s ret;
UIDialog_gsearch(&m_data,	szName,&ret.m_data);
return ret;
}
lua_UIGroup_s			lua_UIDialog_s::gsearchbyid(INT nID)
{
lua_UIGroup_s ret;
UIDialog_gsearchbyid(&m_data,	nID,&ret.m_data);
return ret;
}
void			lua_UIDialog_s::hold()
{
UIDialog_hold(&m_data);
}
void			lua_UIDialog_s::image_mode(const char* szControl,	const char* szMode,	INT/*DrawTextAlign*/ align)
{
UIDialog_image_mode(&m_data,	szControl,	szMode,	(DrawTextAlign)align);
}
INT			lua_UIDialog_s::interval(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond)
{
return UIDialog_interval(&m_data,	szLuaFunc,	nMillisecond,	nMaxMillisecond);
}
INT			lua_UIDialog_s::interval2(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond,	const char* param)
{
return UIDialog_interval2(&m_data,	szLuaFunc,	nMillisecond,	nMaxMillisecond,	param);
}
bool			lua_UIDialog_s::istooltip_window()
{
return (UIDialog_istooltip_window(&m_data) != 0);
}
void			lua_UIDialog_s::map_add2dchild(lua_UIDialog_s& child)
{
UIDialog_map_add2dchild(&m_data,	&child.m_data);
}
void			lua_UIDialog_s::map_addfindway(lua_FindWay_s& fw)
{
UIDialog_map_addfindway(&m_data,	&fw.m_data);
}
void			lua_UIDialog_s::map_addmoveto(lua_Pixel& fw)
{
UIDialog_map_addmoveto(&m_data,	fw);
}
lua_Pixel			lua_UIDialog_s::map_areaindex(INT mapx,	INT mapy)
{
lua_Pixel ret;
UIDialog_map_areaindex(&m_data,	mapx,	mapy,&ret);
return ret;
}
void			lua_UIDialog_s::map_beginmoveto(float fSpeed,	const char* szLuaFunc)
{
UIDialog_map_beginmoveto(&m_data,	fSpeed,	szLuaFunc);
}
lua_Pixel			lua_UIDialog_s::map_center(lua_Pixel& pos)
{
lua_Pixel ret;
UIDialog_map_center(&m_data,	pos,&ret);
return ret;
}
void			lua_UIDialog_s::map_clear2dchild()
{
UIDialog_map_clear2dchild(&m_data);
}
void			lua_UIDialog_s::map_clear_moveable()
{
UIDialog_map_clear_moveable(&m_data);
}
lua_Pixel			lua_UIDialog_s::map_count()
{
lua_Pixel ret;
UIDialog_map_count(&m_data,&ret);
return ret;
}
lua_Pixel			lua_UIDialog_s::map_gridsize()
{
lua_Pixel ret;
UIDialog_map_gridsize(&m_data,&ret);
return ret;
}
lua_Pixel			lua_UIDialog_s::map_mapindex(INT areax,	INT areay)
{
lua_Pixel ret;
UIDialog_map_mapindex(&m_data,	areax,	areay,&ret);
return ret;
}
bool			lua_UIDialog_s::map_moveable(INT areax,	INT areay)
{
return (UIDialog_map_moveable(&m_data,	areax,	areay) != 0);
}
lua_Pixel			lua_UIDialog_s::map_pickarea(lua_Pixel& pos)
{
lua_Pixel ret;
UIDialog_map_pickarea(&m_data,	pos,&ret);
return ret;
}
lua_Pixel			lua_UIDialog_s::map_pickmap(lua_Pixel& pos)
{
lua_Pixel ret;
UIDialog_map_pickmap(&m_data,	pos,&ret);
return ret;
}
bool			lua_UIDialog_s::map_searchway(lua_FindWay_s& fw,	lua_Pixel& from,	lua_Pixel& to)
{
return (UIDialog_map_searchway(&m_data,	&fw.m_data,	from,	to) != 0);
}
bool			lua_UIDialog_s::map_searchway2(lua_FindWay_s& fw,	INT fareax,	INT fareay,	INT tareax,	INT tareay)
{
return (UIDialog_map_searchway2(&m_data,	&fw.m_data,	fareax,	fareay,	tareax,	tareay) != 0);
}
void			lua_UIDialog_s::map_set_moveable(INT areax,	INT areay,	bool bMoveable)
{
UIDialog_map_set_moveable(&m_data,	areax,	areay,	bMoveable);
}
void			lua_UIDialog_s::map_stopmove()
{
UIDialog_map_stopmove(&m_data);
}
void			lua_UIDialog_s::move(const char* szControl,	INT left,	INT top)
{
UIDialog_move(&m_data,	szControl,	left,	top);
}
lua_UIControl_s			lua_UIDialog_s::pick(INT x,	INT y)
{
lua_UIControl_s ret;
UIDialog_pick(&m_data,	x,	y,&ret.m_data);
return ret;
}
void			lua_UIDialog_s::playani(const char* szControl,	const char* szAniName)
{
UIDialog_playani(&m_data,	szControl,	szAniName);
}
void			lua_UIDialog_s::recalc_rect(bool bIncludeHide)
{
UIDialog_recalc_rect(&m_data,	bIncludeHide);
}
void			lua_UIDialog_s::rotdir(lua_Pixel& dir)
{
UIDialog_rotdir(&m_data,	dir);
}
lua_UIControl_s			lua_UIDialog_s::search(const char* szName)
{
lua_UIControl_s ret;
UIDialog_search(&m_data,	szName,&ret.m_data);
return ret;
}
lua_UIControl_s			lua_UIDialog_s::searchbyid(INT nID)
{
lua_UIControl_s ret;
UIDialog_searchbyid(&m_data,	nID,&ret.m_data);
return ret;
}
void			lua_UIDialog_s::Setaction(INT/*ActionType*/ nType)
{
UIDialog_Setaction(&m_data,	(ActionType)nType);
}
void			lua_UIDialog_s::Setactionby(const char* szName)
{
UIDialog_Setactionby(&m_data,	szName);
}
void			lua_UIDialog_s::Setcenter(lua_Pixel& _val)
{
UIDialog_Setcenter(&m_data,	_val);
}
void			lua_UIDialog_s::Setcolor(lua_color4& val)
{
UIDialog_Setcolor(&m_data,	val);
}
void			lua_UIDialog_s::Setdisplay(bool b)
{
UIDialog_Setdisplay(&m_data,	b);
}
void			lua_UIDialog_s::Setdomodal(bool bModel)
{
UIDialog_Setdomodal(&m_data,	bModel);
}
void			lua_UIDialog_s::Setenabled(bool bEnable)
{
UIDialog_Setenabled(&m_data,	bEnable);
}
void			lua_UIDialog_s::Setheight(INT _val)
{
UIDialog_Setheight(&m_data,	_val);
}
void			lua_UIDialog_s::Sethideon_killfocus(bool b)
{
UIDialog_Sethideon_killfocus(&m_data,	b);
}
void			lua_UIDialog_s::Setlayer(INT nLayer)
{
UIDialog_Setlayer(&m_data,	nLayer);
}
void			lua_UIDialog_s::Setleft(INT _val)
{
UIDialog_Setleft(&m_data,	_val);
}
void			lua_UIDialog_s::Setname(const char* szName)
{
UIDialog_Setname(&m_data,	szName);
}
void			lua_UIDialog_s::Setopacity(float val)
{
UIDialog_Setopacity(&m_data,	val);
}
void			lua_UIDialog_s::Setpause(bool bPaused)
{
UIDialog_Setpause(&m_data,	bPaused);
}
void			lua_UIDialog_s::Setpos(lua_Pixel& _val)
{
UIDialog_Setpos(&m_data,	_val);
}
void			lua_UIDialog_s::Setrect(lua_Rect& _val)
{
UIDialog_Setrect(&m_data,	_val);
}
void			lua_UIDialog_s::Setrot2d(float fDegress)
{
UIDialog_Setrot2d(&m_data,	fDegress);
}
void			lua_UIDialog_s::Setrotx(float fDegress)
{
UIDialog_Setrotx(&m_data,	fDegress);
}
void			lua_UIDialog_s::Setroty(float fDegress)
{
UIDialog_Setroty(&m_data,	fDegress);
}
void			lua_UIDialog_s::Setrotz(float fDegress)
{
UIDialog_Setrotz(&m_data,	fDegress);
}
void			lua_UIDialog_s::Setscale(float fScale)
{
UIDialog_Setscale(&m_data,	fScale);
}
void			lua_UIDialog_s::Setscalex(float fScale)
{
UIDialog_Setscalex(&m_data,	fScale);
}
void			lua_UIDialog_s::Setscaley(float fScale)
{
UIDialog_Setscaley(&m_data,	fScale);
}
void			lua_UIDialog_s::settooltip_window(bool b)
{
UIDialog_settooltip_window(&m_data,	b);
}
void			lua_UIDialog_s::Settop(INT _val)
{
UIDialog_Settop(&m_data,	_val);
}
void			lua_UIDialog_s::Settopmost(bool bTopMost)
{
UIDialog_Settopmost(&m_data,	bTopMost);
}
void			lua_UIDialog_s::setvalf(const char* szControl,	float fValue)
{
UIDialog_setvalf(&m_data,	szControl,	fValue);
}
void			lua_UIDialog_s::setvali(const char* szControl,	INT nValue)
{
UIDialog_setvali(&m_data,	szControl,	nValue);
}
void			lua_UIDialog_s::setvalt(const char* szControl,	const char* szValue)
{
UIDialog_setvalt(&m_data,	szControl,	szValue);
}
void			lua_UIDialog_s::Setwidth(INT _val)
{
UIDialog_Setwidth(&m_data,	_val);
}
void			lua_UIDialog_s::show(const char* szControl,	bool bShow)
{
UIDialog_show(&m_data,	szControl,	bShow);
}
INT			lua_UIDialog_s::timeout(const char* szLuaFunc,	INT nMillisecond)
{
return UIDialog_timeout(&m_data,	szLuaFunc,	nMillisecond);
}
INT			lua_UIDialog_s::timeout2(const char* szLuaFunc,	INT nMillisecond,	const char* param)
{
return UIDialog_timeout2(&m_data,	szLuaFunc,	nMillisecond,	param);
}
lua_UIDrawMgr_s			lua_UIDrawMgr_s::add_child()
{
lua_UIDrawMgr_s ret;
UIDrawMgr_add_child(&m_data,&ret.m_data);
return ret;
}
void			lua_UIDrawMgr_s::bind_click(const char* szFunc)
{
UIDrawMgr_bind_click(&m_data,	szFunc);
}
void			lua_UIDrawMgr_s::bind_dblclick(const char* szFunc)
{
UIDrawMgr_bind_dblclick(&m_data,	szFunc);
}
INT			lua_UIDrawMgr_s::child_count()
{
return UIDrawMgr_child_count(&m_data);
}
void			lua_UIDrawMgr_s::delete_child(INT nIndex)
{
UIDrawMgr_delete_child(&m_data,	nIndex);
}
lua_Rect			lua_UIDrawMgr_s::draw_rect()
{
lua_Rect ret;
UIDrawMgr_draw_rect(&m_data,&ret);
return ret;
}
lua_UIDrawMgr_s			lua_UIDrawMgr_s::get_child(INT nIndex)
{
lua_UIDrawMgr_s ret;
UIDrawMgr_get_child(&m_data,	nIndex,&ret.m_data);
return ret;
}
lua_color4			lua_UIDrawMgr_s::get_color(INT index)
{
lua_color4 ret;
UIDrawMgr_get_color(&m_data,	index,&ret);
return ret;
}
lua_Pixel			lua_UIDrawMgr_s::get_point(INT index)
{
lua_Pixel ret;
UIDrawMgr_get_point(&m_data,	index,&ret);
return ret;
}
lua_float2			lua_UIDrawMgr_s::get_uv(INT index)
{
lua_float2 ret;
UIDrawMgr_get_uv(&m_data,	index,&ret);
return ret;
}
lua_Pixel			lua_UIDrawMgr_s::Getcenter()
{
lua_Pixel ret;
UIDrawMgr_Getcenter(&m_data,&ret);
return ret;
}
float			lua_UIDrawMgr_s::Getclickscale()
{
return UIDrawMgr_Getclickscale(&m_data);
}
lua_color4			lua_UIDrawMgr_s::Getcolor()
{
lua_color4 ret;
UIDrawMgr_Getcolor(&m_data,&ret);
return ret;
}
INT/*CullMode*/			lua_UIDrawMgr_s::Getcullmode()
{
return (INT)UIDrawMgr_Getcullmode(&m_data);
}
bool			lua_UIDrawMgr_s::Getdisplay()
{
return (UIDrawMgr_Getdisplay(&m_data) != 0);
}
bool			lua_UIDrawMgr_s::Getenabled()
{
return (UIDrawMgr_Getenabled(&m_data) != 0);
}
INT			lua_UIDrawMgr_s::Gethighlight()
{
return UIDrawMgr_Gethighlight(&m_data);
}
INT			lua_UIDrawMgr_s::Getlayer()
{
return UIDrawMgr_Getlayer(&m_data);
}
INT			lua_UIDrawMgr_s::Getnumber()
{
return UIDrawMgr_Getnumber(&m_data);
}
float			lua_UIDrawMgr_s::Getoffset3d_z()
{
return UIDrawMgr_Getoffset3d_z(&m_data);
}
float			lua_UIDrawMgr_s::Getopacity()
{
return UIDrawMgr_Getopacity(&m_data);
}
lua_Pixel			lua_UIDrawMgr_s::Getpos()
{
lua_Pixel ret;
UIDrawMgr_Getpos(&m_data,&ret);
return ret;
}
INT			lua_UIDrawMgr_s::Getrotx()
{
return UIDrawMgr_Getrotx(&m_data);
}
INT			lua_UIDrawMgr_s::Getroty()
{
return UIDrawMgr_Getroty(&m_data);
}
INT			lua_UIDrawMgr_s::Getrotz()
{
return UIDrawMgr_Getrotz(&m_data);
}
float			lua_UIDrawMgr_s::Getscale()
{
return UIDrawMgr_Getscale(&m_data);
}
float			lua_UIDrawMgr_s::Getscalex()
{
return UIDrawMgr_Getscalex(&m_data);
}
float			lua_UIDrawMgr_s::Getscaley()
{
return UIDrawMgr_Getscaley(&m_data);
}
const char*			lua_UIDrawMgr_s::Gettext()
{
return UIDrawMgr_Gettext(&m_data);
}
void			lua_UIDrawMgr_s::image(const char* szTexture)
{
UIDrawMgr_image(&m_data,	szTexture);
}
void			lua_UIDrawMgr_s::image_mode(const char* szMode)
{
UIDrawMgr_image_mode(&m_data,	szMode);
}
void			lua_UIDrawMgr_s::image_rect(INT left,	INT top,	INT width,	INT height)
{
UIDrawMgr_image_rect(&m_data,	left,	top,	width,	height);
}
void			lua_UIDrawMgr_s::image_uvani(INT start,	INT count,	float speed)
{
UIDrawMgr_image_uvani(&m_data,	start,	count,	speed);
}
void			lua_UIDrawMgr_s::image_uvmap(INT width,	INT height,	INT wcount,	INT hcount)
{
UIDrawMgr_image_uvmap(&m_data,	width,	height,	wcount,	hcount);
}
void			lua_UIDrawMgr_s::number_align(const char* szModeFirst,	const char* szAlign)
{
UIDrawMgr_number_align(&m_data,	szModeFirst,	szAlign);
}
void			lua_UIDrawMgr_s::number_mode(const char* szModeFirst,	INT numberHeight,	INT spaceX)
{
UIDrawMgr_number_mode(&m_data,	szModeFirst,	numberHeight,	spaceX);
}
float			lua_UIDrawMgr_s::progress()
{
return UIDrawMgr_progress(&m_data);
}
void			lua_UIDrawMgr_s::progress_mode(float fValue)
{
UIDrawMgr_progress_mode(&m_data,	fValue);
}
void			lua_UIDrawMgr_s::set_color(INT index,	lua_color4& c)
{
UIDrawMgr_set_color(&m_data,	index,	c);
}
void			lua_UIDrawMgr_s::set_point(INT index,	INT x,	INT y)
{
UIDrawMgr_set_point(&m_data,	index,	x,	y);
}
void			lua_UIDrawMgr_s::set_rect(INT left,	INT top,	INT right,	INT bottom)
{
UIDrawMgr_set_rect(&m_data,	left,	top,	right,	bottom);
}
void			lua_UIDrawMgr_s::set_uv(INT index,	float u,	float v)
{
UIDrawMgr_set_uv(&m_data,	index,	u,	v);
}
void			lua_UIDrawMgr_s::Setcenter(lua_Pixel& center)
{
UIDrawMgr_Setcenter(&m_data,	center);
}
void			lua_UIDrawMgr_s::Setclickscale(float fscale)
{
UIDrawMgr_Setclickscale(&m_data,	fscale);
}
void			lua_UIDrawMgr_s::Setcolor(lua_color4& c)
{
UIDrawMgr_Setcolor(&m_data,	c);
}
void			lua_UIDrawMgr_s::Setcullmode(INT/*CullMode*/ layer)
{
UIDrawMgr_Setcullmode(&m_data,	(CullMode)layer);
}
void			lua_UIDrawMgr_s::Setdisplay(bool bDisplay)
{
UIDrawMgr_Setdisplay(&m_data,	bDisplay);
}
void			lua_UIDrawMgr_s::Setenabled(bool bEnabled)
{
UIDrawMgr_Setenabled(&m_data,	bEnabled);
}
void			lua_UIDrawMgr_s::Sethighlight(INT highlight)
{
UIDrawMgr_Sethighlight(&m_data,	highlight);
}
void			lua_UIDrawMgr_s::Setlayer(INT layer)
{
UIDrawMgr_Setlayer(&m_data,	layer);
}
void			lua_UIDrawMgr_s::Setnumber(INT num)
{
UIDrawMgr_Setnumber(&m_data,	num);
}
void			lua_UIDrawMgr_s::Setoffset3d_z(float fOffsetZ)
{
UIDrawMgr_Setoffset3d_z(&m_data,	fOffsetZ);
}
void			lua_UIDrawMgr_s::Setopacity(float fAlpha)
{
UIDrawMgr_Setopacity(&m_data,	fAlpha);
}
void			lua_UIDrawMgr_s::Setpos(lua_Pixel& center)
{
UIDrawMgr_Setpos(&m_data,	center);
}
void			lua_UIDrawMgr_s::Setrotx(INT fDegress)
{
UIDrawMgr_Setrotx(&m_data,	fDegress);
}
void			lua_UIDrawMgr_s::Setroty(INT fDegress)
{
UIDrawMgr_Setroty(&m_data,	fDegress);
}
void			lua_UIDrawMgr_s::Setrotz(INT fDegress)
{
UIDrawMgr_Setrotz(&m_data,	fDegress);
}
void			lua_UIDrawMgr_s::Setscale(float fScale)
{
UIDrawMgr_Setscale(&m_data,	fScale);
}
void			lua_UIDrawMgr_s::Setscalex(float fScalex)
{
UIDrawMgr_Setscalex(&m_data,	fScalex);
}
void			lua_UIDrawMgr_s::Setscaley(float fScaley)
{
UIDrawMgr_Setscaley(&m_data,	fScaley);
}
void			lua_UIDrawMgr_s::Settext(const char* text)
{
UIDrawMgr_Settext(&m_data,	text);
}
void			lua_UIDrawMgr_s::text_mode(const char* szValue,	float fFontScale)
{
UIDrawMgr_text_mode(&m_data,	szValue,	fFontScale);
}
INT/*BlendMode*/			lua_UIElement_s::Getdestblend()
{
return (INT)UIElement_Getdestblend(&m_data);
}
INT			lua_UIElement_s::Getfontid()
{
return UIElement_Getfontid(&m_data);
}
const char*			lua_UIElement_s::Getimage()
{
return UIElement_Getimage(&m_data);
}
lua_color4			lua_UIElement_s::Getimagecolor()
{
lua_color4 ret;
UIElement_Getimagecolor(&m_data,&ret);
return ret;
}
INT/*BlendMode*/			lua_UIElement_s::Getsrcblend()
{
return (INT)UIElement_Getsrcblend(&m_data);
}
lua_color4			lua_UIElement_s::Gettextcolor()
{
lua_color4 ret;
UIElement_Gettextcolor(&m_data,&ret);
return ret;
}
INT/*TextureBlend*/			lua_UIElement_s::Gettextureblend()
{
return (INT)UIElement_Gettextureblend(&m_data);
}
INT			lua_UIElement_s::Gettextureid()
{
return UIElement_Gettextureid(&m_data);
}
const char*			lua_UIElement_s::Gettexturemode()
{
return UIElement_Gettexturemode(&m_data);
}
lua_Rect			lua_UIElement_s::Gettexturerect()
{
lua_Rect ret;
UIElement_Gettexturerect(&m_data,&ret);
return ret;
}
INT			lua_UIElement_s::Gettileheight()
{
return UIElement_Gettileheight(&m_data);
}
INT			lua_UIElement_s::Gettilewidth()
{
return UIElement_Gettilewidth(&m_data);
}
const char*			lua_UIElement_s::Getuvaction()
{
return UIElement_Getuvaction(&m_data);
}
float			lua_UIElement_s::Getuvspeed()
{
return UIElement_Getuvspeed(&m_data);
}
bool			lua_UIElement_s::isscale()
{
return (UIElement_isscale(&m_data) != 0);
}
void			lua_UIElement_s::Setdestblend(INT/*BlendMode*/ m)
{
UIElement_Setdestblend(&m_data,	(BlendMode)m);
}
void			lua_UIElement_s::Setfontid(INT nFontID)
{
UIElement_Setfontid(&m_data,	nFontID);
}
void			lua_UIElement_s::Setimage(const char* szFileName)
{
UIElement_Setimage(&m_data,	szFileName);
}
void			lua_UIElement_s::Setimagecolor(lua_color4& c)
{
UIElement_Setimagecolor(&m_data,	c);
}
void			lua_UIElement_s::setscale(bool bScale,	bool bWrap,	INT wrapWidth,	INT wrapHeight)
{
UIElement_setscale(&m_data,	bScale,	bWrap,	wrapWidth,	wrapHeight);
}
void			lua_UIElement_s::Setsrcblend(INT/*BlendMode*/ m)
{
UIElement_Setsrcblend(&m_data,	(BlendMode)m);
}
void			lua_UIElement_s::Settextcolor(lua_color4& c)
{
UIElement_Settextcolor(&m_data,	c);
}
void			lua_UIElement_s::Settextureblend(INT/*TextureBlend*/ op)
{
UIElement_Settextureblend(&m_data,	(TextureBlend)op);
}
void			lua_UIElement_s::Settextureid(INT nID)
{
UIElement_Settextureid(&m_data,	nID);
}
void			lua_UIElement_s::Settexturemode(const char* szMode)
{
UIElement_Settexturemode(&m_data,	szMode);
}
void			lua_UIElement_s::Settexturerect(lua_Rect& _s)
{
UIElement_Settexturerect(&m_data,	_s);
}
void			lua_UIElement_s::Settileheight(INT nTileHeight)
{
UIElement_Settileheight(&m_data,	nTileHeight);
}
void			lua_UIElement_s::Settilewidth(INT nTileWidth)
{
UIElement_Settilewidth(&m_data,	nTileWidth);
}
void			lua_UIElement_s::Setuvaction(const char* map)
{
UIElement_Setuvaction(&m_data,	map);
}
void			lua_UIElement_s::Setuvspeed(float f)
{
UIElement_Setuvspeed(&m_data,	f);
}
const char*			lua_UIElement_s::uvaction_at(INT nIndex)
{
return UIElement_uvaction_at(&m_data,	nIndex);
}
INT			lua_UIElement_s::uvactioncount()
{
return UIElement_uvactioncount(&m_data);
}
lua_UIControl_s			lua_UIGroup_s::at(INT nIndex)
{
lua_UIControl_s ret;
UIGroup_at(&m_data,	nIndex,&ret.m_data);
return ret;
}
void			lua_UIGroup_s::bind_click(const char* szControl,	const char* szLuaFunc)
{
UIGroup_bind_click(&m_data,	szControl,	szLuaFunc);
}
void			lua_UIGroup_s::bind_draw(const char* szControl,	lua_UIDrawMgr_s& drawPtr)
{
UIGroup_bind_draw(&m_data,	szControl,	&drawPtr.m_data);
}
void			lua_UIGroup_s::bind_event(INT/*EventID*/ nid,	const char* szControl,	const char* szLuaFunc)
{
UIGroup_bind_event(&m_data,	(EventID)nid,	szControl,	szLuaFunc);
}
void			lua_UIGroup_s::bind_eventby(const char* szEvent,	const char* szControl,	const char* szLuaFunc)
{
UIGroup_bind_eventby(&m_data,	szEvent,	szControl,	szLuaFunc);
}
void			lua_UIGroup_s::bind_image(const char* szControl,	const char* szImageFile)
{
UIGroup_bind_image(&m_data,	szControl,	szImageFile);
}
lua_UIGroup_s			lua_UIGroup_s::clone()
{
lua_UIGroup_s ret;
UIGroup_clone(&m_data,&ret.m_data);
return ret;
}
lua_UIGroup_s			lua_UIGroup_s::cloneto(lua_UIDialog_s& dest)
{
lua_UIGroup_s ret;
UIGroup_cloneto(&m_data,	&dest.m_data,&ret.m_data);
return ret;
}
INT			lua_UIGroup_s::count()
{
return UIGroup_count(&m_data);
}
lua_UIDialog_s			lua_UIGroup_s::dialog()
{
lua_UIDialog_s ret;
UIGroup_dialog(&m_data,&ret.m_data);
return ret;
}
void			lua_UIGroup_s::enable(const char* szControl,	bool bEnable)
{
UIGroup_enable(&m_data,	szControl,	bEnable);
}
INT			lua_UIGroup_s::framemove(const char* szLuaFunc,	INT nMaxMillisecond)
{
return UIGroup_framemove(&m_data,	szLuaFunc,	nMaxMillisecond);
}
INT			lua_UIGroup_s::framemove2(const char* szLuaFunc,	INT nMaxMillisecond,	const char* param)
{
return UIGroup_framemove2(&m_data,	szLuaFunc,	nMaxMillisecond,	param);
}
INT/*ActionType*/			lua_UIGroup_s::Getaction()
{
return (INT)UIGroup_Getaction(&m_data);
}
const char*			lua_UIGroup_s::Getactionby()
{
return UIGroup_Getactionby(&m_data);
}
lua_Pixel			lua_UIGroup_s::Getcenter()
{
lua_Pixel ret;
UIGroup_Getcenter(&m_data,&ret);
return ret;
}
float			lua_UIGroup_s::Getcurrenttime()
{
return UIGroup_Getcurrenttime(&m_data);
}
bool			lua_UIGroup_s::Getdisplay()
{
return (UIGroup_Getdisplay(&m_data) != 0);
}
bool			lua_UIGroup_s::Getenabled()
{
return (UIGroup_Getenabled(&m_data) != 0);
}
INT			lua_UIGroup_s::Getleft()
{
return UIGroup_Getleft(&m_data);
}
const char*			lua_UIGroup_s::Getname()
{
return UIGroup_Getname(&m_data);
}
float			lua_UIGroup_s::Getopacity()
{
return UIGroup_Getopacity(&m_data);
}
lua_Pixel			lua_UIGroup_s::Getpos()
{
lua_Pixel ret;
UIGroup_Getpos(&m_data,&ret);
return ret;
}
float			lua_UIGroup_s::Getrot()
{
return UIGroup_Getrot(&m_data);
}
float			lua_UIGroup_s::Getspeed()
{
return UIGroup_Getspeed(&m_data);
}
INT			lua_UIGroup_s::Gettop()
{
return UIGroup_Gettop(&m_data);
}
float			lua_UIGroup_s::getvalf(const char* szControl)
{
return UIGroup_getvalf(&m_data,	szControl);
}
INT			lua_UIGroup_s::getvali(const char* szControl)
{
return UIGroup_getvali(&m_data,	szControl);
}
const char*			lua_UIGroup_s::getvalt(const char* szControl)
{
return UIGroup_getvalt(&m_data,	szControl);
}
INT			lua_UIGroup_s::height()
{
return UIGroup_height(&m_data);
}
void			lua_UIGroup_s::image_mode(const char* szControl,	const char* szMode,	INT/*DrawTextAlign*/ align)
{
UIGroup_image_mode(&m_data,	szControl,	szMode,	(DrawTextAlign)align);
}
INT			lua_UIGroup_s::interval(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond)
{
return UIGroup_interval(&m_data,	szLuaFunc,	nMillisecond,	nMaxMillisecond);
}
INT			lua_UIGroup_s::interval2(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond,	const char* param)
{
return UIGroup_interval2(&m_data,	szLuaFunc,	nMillisecond,	nMaxMillisecond,	param);
}
void			lua_UIGroup_s::playani(const char* szControl,	const char* szAniName)
{
UIGroup_playani(&m_data,	szControl,	szAniName);
}
lua_UIControl_s			lua_UIGroup_s::search(const char* szName)
{
lua_UIControl_s ret;
UIGroup_search(&m_data,	szName,&ret.m_data);
return ret;
}
void			lua_UIGroup_s::Setaction(INT/*ActionType*/ nType)
{
UIGroup_Setaction(&m_data,	(ActionType)nType);
}
void			lua_UIGroup_s::Setactionby(const char* szName)
{
UIGroup_Setactionby(&m_data,	szName);
}
void			lua_UIGroup_s::Setcenter(lua_Pixel& pos)
{
UIGroup_Setcenter(&m_data,	pos);
}
void			lua_UIGroup_s::Setcurrenttime(float fTime)
{
UIGroup_Setcurrenttime(&m_data,	fTime);
}
void			lua_UIGroup_s::Setdisplay(bool bVisible)
{
UIGroup_Setdisplay(&m_data,	bVisible);
}
void			lua_UIGroup_s::Setenabled(bool bEnable)
{
UIGroup_Setenabled(&m_data,	bEnable);
}
void			lua_UIGroup_s::Setleft(INT nLeft)
{
UIGroup_Setleft(&m_data,	nLeft);
}
void			lua_UIGroup_s::Setname(const char* szName)
{
UIGroup_Setname(&m_data,	szName);
}
void			lua_UIGroup_s::Setopacity(float fOpacity)
{
UIGroup_Setopacity(&m_data,	fOpacity);
}
void			lua_UIGroup_s::Setpos(lua_Pixel& _val)
{
UIGroup_Setpos(&m_data,	_val);
}
void			lua_UIGroup_s::Setrot(float fDegress)
{
UIGroup_Setrot(&m_data,	fDegress);
}
void			lua_UIGroup_s::setRotDir(lua_float2& dir)
{
UIGroup_setRotDir(&m_data,	dir);
}
void			lua_UIGroup_s::Setspeed(float fSpeed)
{
UIGroup_Setspeed(&m_data,	fSpeed);
}
void			lua_UIGroup_s::Settop(INT nTop)
{
UIGroup_Settop(&m_data,	nTop);
}
void			lua_UIGroup_s::setvalf(const char* szControl,	float fValue)
{
UIGroup_setvalf(&m_data,	szControl,	fValue);
}
void			lua_UIGroup_s::setvali(const char* szControl,	INT nValue)
{
UIGroup_setvali(&m_data,	szControl,	nValue);
}
void			lua_UIGroup_s::setvalt(const char* szControl,	const char* szValue)
{
UIGroup_setvalt(&m_data,	szControl,	szValue);
}
void			lua_UIGroup_s::show(const char* szControl,	bool bShow)
{
UIGroup_show(&m_data,	szControl,	bShow);
}
INT			lua_UIGroup_s::timeout(const char* szLuaFunc,	INT nMillisecond)
{
return UIGroup_timeout(&m_data,	szLuaFunc,	nMillisecond);
}
INT			lua_UIGroup_s::timeout2(const char* szLuaFunc,	INT nMillisecond,	const char* param)
{
return UIGroup_timeout2(&m_data,	szLuaFunc,	nMillisecond,	param);
}
INT			lua_UIGroup_s::width()
{
return UIGroup_width(&m_data);
}
bool			lua_Camera::aaboxin(lua_AABox& p)
{
return (Camera_aaboxin(p) != 0);
}
void			lua_Camera::dither(lua_float3& offset,	float fMaxOffsetTime)
{
Camera_dither(offset,	fMaxOffsetTime);
}
bool			lua_Camera::get2d_from3d(lua_Pixel& point,	lua_float3& pos)
{
return (Camera_get2d_from3d(point,	pos) != 0);
}
void			lua_Camera::get3d_from2d(lua_float3& orig,	lua_float3& dir,	lua_Pixel& point)
{
Camera_get3d_from2d(orig,	dir,	point);
}
lua_Rect			lua_Camera::Getarea()
{
lua_Rect ret;
Camera_Getarea(&ret);
return ret;
}
lua_CameraData			lua_Camera::Getdata()
{
lua_CameraData ret;
Camera_Getdata(&ret);
return ret;
}
float			lua_Camera::Getdistance()
{
return Camera_Getdistance();
}
float			lua_Camera::Getdistance_max()
{
return Camera_Getdistance_max();
}
float			lua_Camera::Getdistance_min()
{
return Camera_Getdistance_min();
}
bool			lua_Camera::Getenabled()
{
return (Camera_Getenabled() != 0);
}
lua_float3			lua_Camera::Geteye()
{
lua_float3 ret;
Camera_Geteye(&ret);
return ret;
}
float			lua_Camera::Getfov()
{
return Camera_Getfov();
}
INT			lua_Camera::Getheight()
{
return Camera_Getheight();
}
bool			lua_Camera::Getintersect_eye()
{
return (Camera_Getintersect_eye() != 0);
}
lua_float3			lua_Camera::Getlookat()
{
lua_float3 ret;
Camera_Getlookat(&ret);
return ret;
}
float			lua_Camera::Getoffset_down()
{
return Camera_Getoffset_down();
}
bool			lua_Camera::Getoffset_enable()
{
return (Camera_Getoffset_enable() != 0);
}
lua_float3			lua_Camera::Getpos()
{
lua_float3 ret;
Camera_Getpos(&ret);
return ret;
}
lua_matrix4x4			lua_Camera::Getprojmat()
{
lua_matrix4x4 ret;
Camera_Getprojmat(&ret);
return ret;
}
lua_float2			lua_Camera::Getrot()
{
lua_float2 ret;
Camera_Getrot(&ret);
return ret;
}
lua_float3			lua_Camera::Gettarget()
{
lua_float3 ret;
Camera_Gettarget(&ret);
return ret;
}
lua_matrix4x4			lua_Camera::Getviewmat()
{
lua_matrix4x4 ret;
Camera_Getviewmat(&ret);
return ret;
}
INT			lua_Camera::Getwidth()
{
return Camera_Getwidth();
}
float			lua_Camera::Getzfar()
{
return Camera_Getzfar();
}
float			lua_Camera::Getznearest()
{
return Camera_Getznearest();
}
bool			lua_Camera::isdisplay(lua_float3& pos,	float fRadius,	float fMaxDistance)
{
return (Camera_isdisplay(pos,	fRadius,	fMaxDistance) != 0);
}
bool			lua_Camera::isortho()
{
return (Camera_isortho() != 0);
}
bool			lua_Camera::isperspective()
{
return (Camera_isperspective() != 0);
}
void			lua_Camera::nextdist(float fDistance,	float fMoveSpeed)
{
Camera_nextdist(fDistance,	fMoveSpeed);
}
void			lua_Camera::nextdither(float offset,	float fMaxOffsetTime)
{
Camera_nextdither(offset,	fMaxOffsetTime);
}
void			lua_Camera::nextrot(float rotZ,	float rotX,	float fRotSpeed)
{
Camera_nextrot(rotZ,	rotX,	fRotSpeed);
}
void			lua_Camera::nextto(lua_float3& pos,	float fMoveSpeed)
{
Camera_nextto(pos,	fMoveSpeed);
}
void			lua_Camera::orthoproj(INT w,	INT h,	float znear,	float zfar)
{
Camera_orthoproj(w,	h,	znear,	zfar);
}
void			lua_Camera::perspectiveproj(float fov,	INT w,	INT h,	float znear,	float zfar)
{
Camera_perspectiveproj(fov,	w,	h,	znear,	zfar);
}
bool			lua_Camera::planein(lua_float3& p1,	lua_float3& p2,	lua_float3& p3,	lua_float3& p4)
{
return (Camera_planein(p1,	p2,	p3,	p4) != 0);
}
void			lua_Camera::play(const char* szLuaFunc,	INT nParam)
{
Camera_play(szLuaFunc,	nParam);
}
bool			lua_Camera::pointin(lua_float3& p)
{
return (Camera_pointin(p) != 0);
}
void			lua_Camera::Setarea(lua_Rect& rc)
{
Camera_Setarea(rc);
}
void			lua_Camera::Setdata(lua_CameraData& p)
{
Camera_Setdata(p);
}
void			lua_Camera::Setdistance(float fLength)
{
Camera_Setdistance(fLength);
}
void			lua_Camera::Setdistance_max(float fLength)
{
Camera_Setdistance_max(fLength);
}
void			lua_Camera::Setdistance_min(float fLength)
{
Camera_Setdistance_min(fLength);
}
void			lua_Camera::Setenabled(bool bEnabled)
{
Camera_Setenabled(bEnabled);
}
void			lua_Camera::Seteye(lua_float3& eye)
{
Camera_Seteye(eye);
}
void			lua_Camera::Setfov(float fValue)
{
Camera_Setfov(fValue);
}
void			lua_Camera::Setheight(INT fValue)
{
Camera_Setheight(fValue);
}
void			lua_Camera::Setintersect_eye(bool bEnabled)
{
Camera_Setintersect_eye(bEnabled);
}
void			lua_Camera::Setlookat(lua_float3& vLookat)
{
Camera_Setlookat(vLookat);
}
void			lua_Camera::Setoffset_down(float fSpeed)
{
Camera_Setoffset_down(fSpeed);
}
void			lua_Camera::Setoffset_enable(bool bEnable)
{
Camera_Setoffset_enable(bEnable);
}
void			lua_Camera::Setpos(lua_float3& pos)
{
Camera_Setpos(pos);
}
void			lua_Camera::Setprojmat(lua_matrix4x4& view)
{
Camera_Setprojmat(view);
}
void			lua_Camera::Setrot(lua_float2& rot)
{
Camera_Setrot(rot);
}
void			lua_Camera::Settarget(lua_float3& target)
{
Camera_Settarget(target);
}
void			lua_Camera::Setviewmat(lua_matrix4x4& view)
{
Camera_Setviewmat(view);
}
void			lua_Camera::Setwidth(INT fValue)
{
Camera_Setwidth(fValue);
}
void			lua_Camera::Setzfar(float fValue)
{
Camera_Setzfar(fValue);
}
void			lua_Camera::Setznearest(float fValue)
{
Camera_Setznearest(fValue);
}
bool			lua_Camera::spherein(lua_float3& p,	float fRadius)
{
return (Camera_spherein(p,	fRadius) != 0);
}
void			lua_Camera::stop()
{
Camera_stop();
}
bool			lua_GameMgr::create(INT sceneID,	const char* szLuaFunc)
{
return (GameMgr_create(sceneID,	szLuaFunc) != 0);
}
INT			lua_GameMgr::framemove(lua_GameObject_s& obj,	const char* szLuaFunc,	INT nMaxMillisecond)
{
return GameMgr_framemove(&obj.m_data,	szLuaFunc,	nMaxMillisecond);
}
bool			lua_GameMgr::Getcamerafollow()
{
return (GameMgr_Getcamerafollow() != 0);
}
lua_GameObject_s			lua_GameMgr::Getcameratarget()
{
lua_GameObject_s ret;
GameMgr_Getcameratarget(&ret.m_data);
return ret;
}
float			lua_GameMgr::getheight_3d(lua_float3& pos)
{
return GameMgr_getheight_3d(pos);
}
bool			lua_GameMgr::Getplotenable()
{
return (GameMgr_Getplotenable() != 0);
}
INT			lua_GameMgr::interval(lua_GameObject_s& obj,	const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond)
{
return GameMgr_interval(&obj.m_data,	szLuaFunc,	nMillisecond,	nMaxMillisecond);
}
lua_GameObject_s			lua_GameMgr::leader()
{
lua_GameObject_s ret;
GameMgr_leader(&ret.m_data);
return ret;
}
const char*			lua_GameMgr::loadtext(INT nID,	const char* szCol)
{
return GameMgr_loadtext(nID,	szCol);
}
const char*			lua_GameMgr::loadtextby(const char* szName,	const char* szCol)
{
return GameMgr_loadtextby(szName,	szCol);
}
lua_GameObject_s			lua_GameMgr::objectat(INT index)
{
lua_GameObject_s ret;
GameMgr_objectat(index,&ret.m_data);
return ret;
}
INT			lua_GameMgr::objectcount()
{
return GameMgr_objectcount();
}
void			lua_GameMgr::plotfinish()
{
GameMgr_plotfinish();
}
INT			lua_GameMgr::plotid()
{
return GameMgr_plotid();
}
void			lua_GameMgr::plotnext()
{
GameMgr_plotnext();
}
void			lua_GameMgr::plotto(INT id)
{
GameMgr_plotto(id);
}
lua_Map3DItem_s			lua_GameMgr::ray3d(lua_Pixel& pixel)
{
lua_Map3DItem_s ret;
GameMgr_ray3d(pixel,&ret.m_data);
return ret;
}
lua_Map3DItem_s			lua_GameMgr::ray3d_dir(lua_float3& orig,	lua_float3& dir)
{
lua_Map3DItem_s ret;
GameMgr_ray3d_dir(orig,	dir,&ret.m_data);
return ret;
}
lua_Map3DItem_s			lua_GameMgr::ray3d_dir2(lua_float3& orig,	lua_float3& dir,	float fMaxLength)
{
lua_Map3DItem_s ret;
GameMgr_ray3d_dir2(orig,	dir,	fMaxLength,&ret.m_data);
return ret;
}
const char*			lua_GameMgr::ray3d_getname()
{
return GameMgr_ray3d_getname();
}
float			lua_GameMgr::ray3d_length()
{
return GameMgr_ray3d_length();
}
lua_float3			lua_GameMgr::ray3d_normal()
{
lua_float3 ret;
GameMgr_ray3d_normal(&ret);
return ret;
}
lua_float3			lua_GameMgr::ray3d_point()
{
lua_float3 ret;
GameMgr_ray3d_point(&ret);
return ret;
}
void			lua_GameMgr::reborn(lua_GameObject_s& obj,	float fRebornTime)
{
GameMgr_reborn(&obj.m_data,	fRebornTime);
}
void			lua_GameMgr::resetAll()
{
GameMgr_resetAll();
}
lua_Map3D_s			lua_GameMgr::scene()
{
lua_Map3D_s ret;
GameMgr_scene(&ret.m_data);
return ret;
}
lua_GameObject_s			lua_GameMgr::search(INT nID)
{
lua_GameObject_s ret;
GameMgr_search(nID,&ret.m_data);
return ret;
}
lua_GameObject_s			lua_GameMgr::searchby(const char* szName)
{
lua_GameObject_s ret;
GameMgr_searchby(szName,&ret.m_data);
return ret;
}
lua_GameObject_s			lua_GameMgr::searchby_sceneid(INT sceneID)
{
lua_GameObject_s ret;
GameMgr_searchby_sceneid(sceneID,&ret.m_data);
return ret;
}
void			lua_GameMgr::Setcamerafollow(bool bEnable)
{
GameMgr_Setcamerafollow(bEnable);
}
void			lua_GameMgr::Setcameratarget(lua_GameObject_s& obj)
{
GameMgr_Setcameratarget(&obj.m_data);
}
void			lua_GameMgr::Setplotenable(bool bEnable)
{
GameMgr_Setplotenable(bEnable);
}
INT			lua_GameMgr::timeout(lua_GameObject_s& obj,	const char* szLuaFunc,	INT nMillisecond)
{
return GameMgr_timeout(&obj.m_data,	szLuaFunc,	nMillisecond);
}
INT			lua_System::fps()
{
return System_fps();
}
lua_color4			lua_System::Getbackcolor()
{
lua_color4 ret;
System_Getbackcolor(&ret);
return ret;
}
INT/*CullMode*/			lua_System::Getcullmode()
{
return (INT)System_Getcullmode();
}
bool			lua_System::Getenablevr()
{
return (System_Getenablevr() != 0);
}
bool			lua_System::Getenablevr2()
{
return (System_Getenablevr2() != 0);
}
bool			lua_System::Getenablez()
{
return (System_Getenablez() != 0);
}
INT			lua_System::Getgravity_smooth()
{
return System_Getgravity_smooth();
}
INT			lua_System::Getmaxfps()
{
return System_Getmaxfps();
}
bool			lua_System::Getmute()
{
return (System_Getmute() != 0);
}
INT/*AlphaMode*/			lua_System::Getopacitymode()
{
return (INT)System_Getopacitymode();
}
bool			lua_System::Getshowfps()
{
return (System_Getshowfps() != 0);
}
bool			lua_System::Getshowui()
{
return (System_Getshowui() != 0);
}
lua_float2			lua_System::Getuioffset()
{
lua_float2 ret;
System_Getuioffset(&ret);
return ret;
}
lua_float2			lua_System::Getuiscale()
{
lua_float2 ret;
System_Getuiscale(&ret);
return ret;
}
float			lua_System::Getvr_rotx()
{
return System_Getvr_rotx();
}
INT			lua_System::Getvr_smooth()
{
return System_Getvr_smooth();
}
float			lua_System::Getvrdistance()
{
return System_Getvrdistance();
}
lua_float2			lua_System::gravity()
{
lua_float2 ret;
System_gravity(&ret);
return ret;
}
lua_sensordata			lua_System::sensor()
{
lua_sensordata ret;
System_sensor(&ret);
return ret;
}
lua_float3			lua_System::sensor_rotator()
{
lua_float3 ret;
System_sensor_rotator(&ret);
return ret;
}
void			lua_System::Setbackcolor(lua_color4& c)
{
System_Setbackcolor(c);
}
void			lua_System::Setcullmode(INT/*CullMode*/ nCull)
{
System_Setcullmode((CullMode)nCull);
}
void			lua_System::Setenablevr(bool b)
{
System_Setenablevr(b);
}
void			lua_System::Setenablevr2(bool b)
{
System_Setenablevr2(b);
}
void			lua_System::Setenablez(bool b)
{
System_Setenablez(b);
}
void			lua_System::Setgravity_smooth(INT s)
{
System_Setgravity_smooth(s);
}
void			lua_System::Setmaxfps(INT n)
{
System_Setmaxfps(n);
}
void			lua_System::Setmute(bool bMute)
{
System_Setmute(bMute);
}
void			lua_System::Setopacitymode(INT/*AlphaMode*/ nMode)
{
System_Setopacitymode((AlphaMode)nMode);
}
void			lua_System::Setshowfps(bool b)
{
System_Setshowfps(b);
}
void			lua_System::Setshowui(bool b)
{
System_Setshowui(b);
}
void			lua_System::Setuioffset(lua_float2& off)
{
System_Setuioffset(off);
}
void			lua_System::Setuiscale(lua_float2& s)
{
System_Setuiscale(s);
}
void			lua_System::Setvr_rotx(float f)
{
System_Setvr_rotx(f);
}
void			lua_System::Setvr_smooth(INT s)
{
System_Setvr_smooth(s);
}
void			lua_System::Setvrdistance(float f)
{
System_Setvrdistance(f);
}
void			lua_System::vibrator(float fSecond)
{
System_vibrator(fSecond);
}
void			lua_System::vr_beginsmooth()
{
System_vr_beginsmooth();
}
void			lua_System::vr_front(lua_float3& dir)
{
System_vr_front(dir);
}
double			lua_Global::Gapptime()
{
return Global_apptime();
}
float			lua_Global::Gelapsedtime()
{
return Global_elapsedtime();
}
INT			lua_Global::Gtimeout(const char* szLuaFunc,	INT nMillisecond)
{
return Global_timeout(szLuaFunc,	nMillisecond);
}
INT			lua_Global::Ginterval(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond)
{
return Global_interval(szLuaFunc,	nMillisecond,	nMaxMillisecond);
}
INT			lua_Global::Gframemove(const char* szLuaFunc,	INT nMaxMillisecond)
{
return Global_framemove(szLuaFunc,	nMaxMillisecond);
}
INT			lua_Global::Gtimeout2(const char* szLuaFunc,	INT nMillisecond,	const char* param)
{
return Global_timeout2(szLuaFunc,	nMillisecond,	param);
}
INT			lua_Global::Ginterval2(const char* szLuaFunc,	INT nMillisecond,	INT nMaxMillisecond,	const char* param)
{
return Global_interval2(szLuaFunc,	nMillisecond,	nMaxMillisecond,	param);
}
INT			lua_Global::Gframemove2(const char* szLuaFunc,	INT nMaxMillisecond,	const char* param)
{
return Global_framemove2(szLuaFunc,	nMaxMillisecond,	param);
}
void			lua_Global::Gcleartimeby(const char* szFunc)
{
Global_cleartimeby(szFunc);
}
void			lua_Global::Gcleartimeby2(const char* szFunc,	const char* param)
{
Global_cleartimeby2(szFunc,	param);
}
void			lua_Global::Gcleartime(INT timeid)
{
Global_cleartime(timeid);
}
void			lua_Global::Gbreaktime()
{
Global_breaktime();
}
void			lua_Global::Gcleartime_all()
{
Global_cleartime_all();
}
lua_Time			lua_Global::Glocaltime()
{
lua_Time ret;
Global_localtime(&ret);
return ret;
}
INT			lua_Global::Gtimediff_d(lua_Time& t1,	lua_Time& t2)
{
return Global_timediff_d(t1,	t2);
}
INT			lua_Global::Gtimediff_h(lua_Time& t1,	lua_Time& t2)
{
return Global_timediff_h(t1,	t2);
}
INT			lua_Global::Gtimediff_m(lua_Time& t1,	lua_Time& t2)
{
return Global_timediff_m(t1,	t2);
}
INT			lua_Global::Gtimediff_s(lua_Time& t1,	lua_Time& t2)
{
return Global_timediff_s(t1,	t2);
}
void			lua_Global::Gpause(bool bPause)
{
Global_pause(bPause);
}
void			lua_Global::Gspeed(float fSpeed)
{
Global_speed(fSpeed);
}
lua_Pixel			lua_Global::Gmousepoint(INT nIndex)
{
lua_Pixel ret;
Global_mousepoint(nIndex,&ret);
return ret;
}
lua_Pixel			lua_Global::Gmousepoint2d(INT nIndex)
{
lua_Pixel ret;
Global_mousepoint2d(nIndex,&ret);
return ret;
}
INT			lua_Global::Gscreenwidth()
{
return Global_screenwidth();
}
INT			lua_Global::Gscreenheight()
{
return Global_screenheight();
}
INT			lua_Global::Ggamewidth()
{
return Global_gamewidth();
}
INT			lua_Global::Ggameheight()
{
return Global_gameheight();
}
void			lua_Global::Gexit()
{
Global_exit();
}
const char*			lua_Global::Galert(const char* szMessage,	const char* szTitle,	const char* szType)
{
return Global_alert(szMessage,	szTitle,	szType);
}
void			lua_Global::Gmsgbox(const char* szTitle,	const char* szMsg,	const char* szFlags,	const char* script)
{
Global_msgbox(szTitle,	szMsg,	szFlags,	script);
}
const char*			lua_Global::Glan(const char* szName)
{
return Global_lan(szName);
}
void			lua_Global::Gsetlanguage(const char* szlanPath)
{
Global_setlanguage(szlanPath);
}
const char*			lua_Global::Glanguage_image(const char* szFileName)
{
return Global_language_image(szFileName);
}
void			lua_Global::Gbind_input(const char* szEvent,	const char* szFunc)
{
Global_bind_input(szEvent,	szFunc);
}
bool			lua_Global::Ginfile(const char* szFile)
{
return (Global_infile(szFile) != 0);
}
bool			lua_Global::Ginsystem(const char* szSystem)
{
return (Global_insystem(szSystem) != 0);
}
void			lua_Global::Ggoto(const char* szLuaFile)
{
Global_goto(szLuaFile);
}
void			lua_Global::Gsendmessage(const char* szName,	const char* szParams,	const char* szParams2)
{
Global_sendmessage(szName,	szParams,	szParams2);
}
void			lua_Global::Gad_show(INT nType,	bool bShow)
{
Global_ad_show(nType,	bShow);
}
void			lua_Global::Gad_setpos(INT x,	INT y)
{
Global_ad_setpos(x,	y);
}
void			lua_Global::Gopenurl(const char* url)
{
Global_openurl(url);
}
void			lua_Global::Ggotocomment()
{
Global_gotocomment();
}
void			lua_Global::Gopenrank(const char* rank)
{
Global_openrank(rank);
}
void			lua_Global::Grankscore(const char* rank,	INT nValue)
{
Global_rankscore(rank,	nValue);
}
void			lua_Global::Gachievement(const char* achievement,	INT percent)
{
Global_achievement(achievement,	percent);
}
lua_matrix4x4			lua_Global::Gworldmat()
{
lua_matrix4x4 ret;
Global_worldmat(&ret);
return ret;
}
lua_matrix4x4			lua_Global::Gviewmat()
{
lua_matrix4x4 ret;
Global_viewmat(&ret);
return ret;
}
lua_matrix4x4			lua_Global::Gprojmat()
{
lua_matrix4x4 ret;
Global_projmat(&ret);
return ret;
}
INT			lua_Global::Ground(float d)
{
return Global_round(d);
}
INT			lua_Global::Gabs(double n)
{
return Global_abs(n);
}
INT			lua_Global::Gsymbol(double n)
{
return Global_symbol(n);
}
INT			lua_Global::Gatoi(const char* v)
{
return Global_atoi(v);
}
bool			lua_Global::Gisstr(const char* a,	const char* b)
{
return (Global_isstr(a,	b) != 0);
}
float			lua_Global::Grand(float fmin,	float fmax)
{
return Global_rand(fmin,	fmax);
}
INT			lua_Global::Gint(double d)
{
return Global_int(d);
}
bool			lua_Global::Gpoint_in(lua_Rect& rect,	lua_Pixel& point)
{
return (Global_point_in(rect,	point) != 0);
}
float			lua_Global::Gtovalue(float meter)
{
return Global_tovalue(meter);
}
float			lua_Global::Gtometer(float v)
{
return Global_tometer(v);
}
INT			lua_Global::Grgba(INT r,	INT g,	INT b,	INT a)
{
return Global_rgba(r,	g,	b,	a);
}
INT			lua_Global::Grgbaf(float r,	float g,	float b,	float a)
{
return Global_rgbaf(r,	g,	b,	a);
}
float			lua_Global::Gclamp_angle(float fDegress)
{
return Global_clamp_angle(fDegress);
}
float			lua_Global::Gclamp(float fValue,	float fMin,	float fMax)
{
return Global_clamp(fValue,	fMin,	fMax);
}
lua_UIDialog_s			lua_Global::Gdlg_create()
{
lua_UIDialog_s ret;
Global_dlg_create(&ret.m_data);
return ret;
}
void			lua_Global::Gdlg_delete(lua_UIDialog_s& ptr)
{
Global_dlg_delete(&ptr.m_data);
}
lua_UIDialog_s			lua_Global::Gdlg_load(const char* szFileName)
{
lua_UIDialog_s ret;
Global_dlg_load(szFileName,&ret.m_data);
return ret;
}
lua_UIDialog_s			lua_Global::Gdlg_loadnew(const char* szFileName)
{
lua_UIDialog_s ret;
Global_dlg_loadnew(szFileName,&ret.m_data);
return ret;
}
lua_UIDialog_s			lua_Global::Gdlg_loadAndProgress(const char* szFileName)
{
lua_UIDialog_s ret;
Global_dlg_loadAndProgress(szFileName,&ret.m_data);
return ret;
}
lua_UIDialog_s			lua_Global::Gdlg_search(const char* szDlgName)
{
lua_UIDialog_s ret;
Global_dlg_search(szDlgName,&ret.m_data);
return ret;
}
lua_UIDialog_s			lua_Global::Gdlg_at(INT index)
{
lua_UIDialog_s ret;
Global_dlg_at(index,&ret.m_data);
return ret;
}
INT			lua_Global::Gdlg_count()
{
return Global_dlg_count();
}
lua_UIDrawMgr_s			lua_Global::Gdraw_create()
{
lua_UIDrawMgr_s ret;
Global_draw_create(&ret.m_data);
return ret;
}
lua_FindWay_s			lua_Global::Gfindway_create()
{
lua_FindWay_s ret;
Global_findway_create(&ret.m_data);
return ret;
}
void			lua_Global::Gfindway_delete(lua_FindWay_s& ptr)
{
Global_findway_delete(&ptr.m_data);
}
lua_Table_s			lua_Global::Gtable_load(const char* szFile)
{
lua_Table_s ret;
Global_table_load(szFile,&ret.m_data);
return ret;
}
void			lua_Global::Gtable_delete(lua_Table_s& ptr)
{
Global_table_delete(&ptr.m_data);
}
lua_Texture_s			lua_Global::Gtexture_load(const char* szFile)
{
lua_Texture_s ret;
Global_texture_load(szFile,&ret.m_data);
return ret;
}
void			lua_Global::Gtexture_delete(lua_Texture_s& ptr)
{
Global_texture_delete(&ptr.m_data);
}
lua_Mesh_s			lua_Global::Gmesh_load(const char* szFile,	bool bAutoRender)
{
lua_Mesh_s ret;
Global_mesh_load(szFile,	bAutoRender,&ret.m_data);
return ret;
}
void			lua_Global::Gmesh_delete(lua_Mesh_s& mesh)
{
Global_mesh_delete(&mesh.m_data);
}
lua_Map3D_s			lua_Global::Gmap3d_load(const char* szName)
{
lua_Map3D_s ret;
Global_map3d_load(szName,&ret.m_data);
return ret;
}
void			lua_Global::Gmap3d_delete(lua_Map3D_s& mesh)
{
Global_map3d_delete(&mesh.m_data);
}
INT			lua_Global::Gtext_create(const char* szText,	INT x,	INT y)
{
return Global_text_create(szText,	x,	y);
}
INT			lua_Global::Gtext_create2(const char* szText,	INT x,	INT y,	lua_color4& c,	INT size)
{
return Global_text_create2(szText,	x,	y,	c,	size);
}
void			lua_Global::Gtext_update(INT nID,	const char* szText)
{
Global_text_update(nID,	szText);
}
void			lua_Global::Gtext_delete(INT nID)
{
Global_text_delete(nID);
}
bool			lua_Global::Ginclude(const char* szFile)
{
return (Global_include(szFile) != 0);
}
lua_DataControl_s			lua_Global::Gconfig(const char* name)
{
lua_DataControl_s ret;
Global_config(name,&ret.m_data);
return ret;
}
bool			lua_Global::Gisconfig(const char* name)
{
return (Global_isconfig(name) != 0);
}
const char*			lua_Global::Greadt(const char* name)
{
return Global_readt(name);
}
double			lua_Global::Greadf(const char* name)
{
return Global_readf(name);
}
INT			lua_Global::Greadi(const char* name)
{
return Global_readi(name);
}
void			lua_Global::Gwritet(const char* name,	const char* szValue)
{
Global_writet(name,	szValue);
}
void			lua_Global::Gwritef(const char* name,	double fValue)
{
Global_writef(name,	fValue);
}
void			lua_Global::Gwritei(const char* name,	INT nValue)
{
Global_writei(name,	nValue);
}
void			lua_Global::Gconfigfile(const char* name)
{
Global_configfile(name);
}
void			lua_Global::Gconfigsave(bool b)
{
Global_configsave(b);
}
lua_DataControl_s			lua_Global::Gvaldata(const char* name)
{
lua_DataControl_s ret;
Global_valdata(name,&ret.m_data);
return ret;
}
bool			lua_Global::Gisval(const char* name)
{
return (Global_isval(name) != 0);
}
const char*			lua_Global::Ggetvalt(const char* name)
{
return Global_getvalt(name);
}
double			lua_Global::Ggetvalf(const char* name)
{
return Global_getvalf(name);
}
INT			lua_Global::Ggetvali(const char* name)
{
return Global_getvali(name);
}
void			lua_Global::Gsetvalt(const char* name,	const char* szValue)
{
Global_setvalt(name,	szValue);
}
void			lua_Global::Gsetvalf(const char* name,	double fValue)
{
Global_setvalf(name,	fValue);
}
void			lua_Global::Gsetvali(const char* name,	INT nValue)
{
Global_setvali(name,	nValue);
}
INT			lua_Global::Gplaywave(const char* szName)
{
return Global_playwave(szName);
}
INT			lua_Global::Gplaymp3(const char* szFile)
{
return Global_playmp3(szFile);
}
INT			lua_Global::Gplaybgm(const char* szFile)
{
return Global_playbgm(szFile);
}
INT			lua_Global::Gplaybgm2(const char* szFile,	float fVolume)
{
return Global_playbgm2(szFile,	fVolume);
}
INT			lua_Global::Gplaymp3_2(const char* szFile,	INT nLoop)
{
return Global_playmp3_2(szFile,	nLoop);
}
INT			lua_Global::Gplaysnd(const char* szFile,	float fVolume,	float fPan,	float fRate)
{
return Global_playsnd(szFile,	fVolume,	fPan,	fRate);
}
void			lua_Global::Gstopsnd(INT s2d)
{
Global_stopsnd(s2d);
}
void			lua_Global::Gstopsndall()
{
Global_stopsndall();
}
void			lua_Global::Gsetsndvolume(INT snd,	float fVolume)
{
Global_setsndvolume(snd,	fVolume);
}
void			lua_Global::Gsetsndvolumepan(INT s2d,	float fVolume,	float fPan)
{
Global_setsndvolumepan(s2d,	fVolume,	fPan);
}
void			lua_Global::Gsetsndrate(INT snd,	float fRate)
{
Global_setsndrate(snd,	fRate);
}
void			lua_Global::Gsetupwave(const char* szName,	INT nDuplicateCount)
{
Global_setupwave(szName,	nDuplicateCount);
}
void			lua_Global::Gdraw_bind(const char* szLuaFunc)
{
Global_draw_bind(szLuaFunc);
}
void			lua_Global::Gdraw2d_rect(lua_Rect& _rect,	lua_color4& c,	bool bFill)
{
Global_draw2d_rect(_rect,	c,	bFill);
}
void			lua_Global::Gdraw2d_line(lua_Pixel& p1,	lua_Pixel& p2,	lua_color4& c)
{
Global_draw2d_line(p1,	p2,	c);
}
void			lua_Global::Gdraw2d_linec(lua_PixelColor& p1,	lua_PixelColor& p2)
{
Global_draw2d_linec(p1,	p2);
}
void			lua_Global::Gdraw2d_text(lua_Rect& _rect,	const char* szText,	lua_color4& c)
{
Global_draw2d_text(_rect,	szText,	c);
}
void			lua_Global::Gdraw2d_texture(lua_Texture_s& texture,	lua_Rect& _rect,	lua_Rect& _textureRect,	lua_color4& c)
{
Global_draw2d_texture(&texture.m_data,	_rect,	_textureRect,	c);
}
void			lua_Global::Gdraw2d_triangle(lua_Texture_s& texture,	lua_PixelUV& p1,	lua_PixelUV& p2,	lua_PixelUV& p3,	bool bFill)
{
Global_draw2d_triangle(&texture.m_data,	p1,	p2,	p3,	bFill);
}
void			lua_Global::Gdraw3d_triangle(lua_float3& p1,	lua_float3& p2,	lua_float3& p3,	lua_color4& c)
{
Global_draw3d_triangle(p1,	p2,	p3,	c);
}
void			lua_Global::Gdraw3d_box(lua_AABox& box,	lua_color4& c,	bool bFill)
{
Global_draw3d_box(box,	c,	bFill);
}
void			lua_Global::Gdraw3d_line(lua_float3& _from,	lua_float3& _to,	lua_color4& c)
{
Global_draw3d_line(_from,	_to,	c);
}
void			lua_Global::Gjinit(const char* szHttpInPhpFile)
{
Global_jinit(szHttpInPhpFile);
}
void			lua_Global::Gjbind(INT id,	const char* szFunc)
{
Global_jbind(id,	szFunc);
}
const char*			lua_Global::Gjget(const char* szName)
{
return Global_jget(szName);
}
INT			lua_Global::Gjgeti(const char* szName)
{
return Global_jgeti(szName);
}
float			lua_Global::Gjgetf(const char* szName)
{
return Global_jgetf(szName);
}
void			lua_Global::Gjclear(const char* szName,	float fValue)
{
Global_jclear(szName,	fValue);
}
void			lua_Global::Gjset(const char* szName,	const char* szValue)
{
Global_jset(szName,	szValue);
}
void			lua_Global::Gjseti(const char* szName,	INT nValue)
{
Global_jseti(szName,	nValue);
}
void			lua_Global::Gjsetf(const char* szName,	float fValue)
{
Global_jsetf(szName,	fValue);
}
void			lua_Global::Gjrequest()
{
Global_jrequest();
}
void			lua_Global::Gdownload(const char* httpRequest,	const char* inDocFile)
{
Global_download(httpRequest,	inDocFile);
}
void			lua_Global::Gnet_init(INT dwGUID)
{
Global_net_init(dwGUID);
}
void			lua_Global::Gnet_onlink(const char* szFunc)
{
Global_net_onlink(szFunc);
}
void			lua_Global::Gnet_ondislink(const char* szFunc)
{
Global_net_ondislink(szFunc);
}
void			lua_Global::Gnet_onmessage(const char* szFunc)
{
Global_net_onmessage(szFunc);
}
bool			lua_Global::Gnet_server(const char* name,	INT port)
{
return (Global_net_server(name,	port) != 0);
}
void			lua_Global::Gnet_search(const char* szFunc,	INT port)
{
Global_net_search(szFunc,	port);
}
void			lua_Global::Gnet_server_max_links(INT nMaxLinks)
{
Global_net_server_max_links(nMaxLinks);
}
INT			lua_Global::Gnet_connect(const char* szIP,	INT port)
{
return Global_net_connect(szIP,	port);
}
void			lua_Global::Gnet_close(INT linkid)
{
Global_net_close(linkid);
}
void			lua_Global::Gnet_stopall()
{
Global_net_stopall();
}
const char*			lua_Global::Gnet_get()
{
return Global_net_get();
}
INT			lua_Global::Gnet_geti()
{
return Global_net_geti();
}
float			lua_Global::Gnet_getf()
{
return Global_net_getf();
}
void			lua_Global::Gnet_begin()
{
Global_net_begin();
}
void			lua_Global::Gnet_set(const char* szValue)
{
Global_net_set(szValue);
}
void			lua_Global::Gnet_seti(INT nValue)
{
Global_net_seti(nValue);
}
void			lua_Global::Gnet_setf(float fValue)
{
Global_net_setf(fValue);
}
void			lua_Global::Gnet_send(INT linkid,	short msgid)
{
Global_net_send(linkid,	msgid);
}
void			lua_Global::Grandom_init(INT number)
{
Global_random_init(number);
}
void			lua_Global::Grandom_weight(INT index,	INT weight)
{
Global_random_weight(index,	weight);
}
void			lua_Global::Grandom_do()
{
Global_random_do();
}
void			lua_Global::Grandom_table(INT number)
{
Global_random_table(number);
}
INT			lua_Global::Grandom_get(INT b)
{
return Global_random_get(b);
}
void			lua_Global::Gc_add(const char* szName)
{
Global_c_add(szName);
}
void			lua_Global::Gc_set(const char* szName,	INT nValue)
{
Global_c_set(szName,	nValue);
}
INT			lua_Global::Gc_get(const char* szName)
{
return Global_c_get(szName);
}
int initPhantomEnum(lua_State* L)
{
static std::string s_enumlist;
static BOOL bInit = false;
if(!bInit){bInit = true;char buf[4096];
sprintf(buf, "ActionType={Idle=%d,Walk=%d,Run=%d,Down=%d,Jump=%d,Hit=%d,Die=%d,Attack=%d,Hide=%d,Idle2=%d,Walk2=%d,Run2=%d,Jump2=%d,Hit2=%d,Die2=%d,Attack2=%d,Idle3=%d,Walk3=%d,Run3=%d,Jump3=%d,Hit3=%d,Die3=%d,Attack3=%d,Other1=%d,Other2=%d,Other3=%d,Other4=%d,Other5=%d,Other6=%d,Other7=%d,Other8=%d,Other9=%d,Other10=%d,Count=%d};",(int)ActionType_Idle,(int)ActionType_Walk,(int)ActionType_Run,(int)ActionType_Down,(int)ActionType_Jump,(int)ActionType_Hit,(int)ActionType_Die,(int)ActionType_Attack,(int)ActionType_Hide,(int)ActionType_Idle2,(int)ActionType_Walk2,(int)ActionType_Run2,(int)ActionType_Jump2,(int)ActionType_Hit2,(int)ActionType_Die2,(int)ActionType_Attack2,(int)ActionType_Idle3,(int)ActionType_Walk3,(int)ActionType_Run3,(int)ActionType_Jump3,(int)ActionType_Hit3,(int)ActionType_Die3,(int)ActionType_Attack3,(int)ActionType_Other1,(int)ActionType_Other2,(int)ActionType_Other3,(int)ActionType_Other4,(int)ActionType_Other5,(int)ActionType_Other6,(int)ActionType_Other7,(int)ActionType_Other8,(int)ActionType_Other9,(int)ActionType_Other10,(int)ActionType_Count);
s_enumlist += buf;
sprintf(buf, "AlphaMode={Test=%d,Blend=%d};",(int)AlphaMode_Test,(int)AlphaMode_Blend);
s_enumlist += buf;
sprintf(buf, "BlendMode={ZERO=%d,ONE=%d,SRCCOLOR=%d,INVSRCCOLOR=%d,SRCALPHA=%d,INVSRCALPHA=%d,DESTALPHA=%d,INVDESTALPHA=%d,DESTCOLOR=%d,INVDESTCOLOR=%d,SRCALPHASAT=%d,BOTHSRCALPHA=%d,BOTHINVSRCALPHA=%d};",(int)BlendMode_ZERO,(int)BlendMode_ONE,(int)BlendMode_SRCCOLOR,(int)BlendMode_INVSRCCOLOR,(int)BlendMode_SRCALPHA,(int)BlendMode_INVSRCALPHA,(int)BlendMode_DESTALPHA,(int)BlendMode_INVDESTALPHA,(int)BlendMode_DESTCOLOR,(int)BlendMode_INVDESTCOLOR,(int)BlendMode_SRCALPHASAT,(int)BlendMode_BOTHSRCALPHA,(int)BlendMode_BOTHINVSRCALPHA);
s_enumlist += buf;
sprintf(buf, "CullMode={Double=%d,CW=%d,CCW=%d};",(int)CullMode_Double,(int)CullMode_CW,(int)CullMode_CCW);
s_enumlist += buf;
sprintf(buf, "DrawTextAlign={TOP=%d,LEFT=%d,CENTER=%d,RIGHT=%d,VCENTER=%d,BOTTOM=%d,WORDBREAK=%d,SINGLELINE=%d,EXPANDTABS=%d,TABSTOP=%d,NOCLIP=%d,CALCRECT=%d};",(int)DrawTextAlign_TOP,(int)DrawTextAlign_LEFT,(int)DrawTextAlign_CENTER,(int)DrawTextAlign_RIGHT,(int)DrawTextAlign_VCENTER,(int)DrawTextAlign_BOTTOM,(int)DrawTextAlign_WORDBREAK,(int)DrawTextAlign_SINGLELINE,(int)DrawTextAlign_EXPANDTABS,(int)DrawTextAlign_TABSTOP,(int)DrawTextAlign_NOCLIP,(int)DrawTextAlign_CALCRECT);
s_enumlist += buf;
sprintf(buf, "EventID={Click=%d,Down=%d,Up=%d,Move=%d,DoubleClick=%d,Changed=%d,PlayOver=%d,UVPlayOver=%d};",(int)EventID_Click,(int)EventID_Down,(int)EventID_Up,(int)EventID_Move,(int)EventID_DoubleClick,(int)EventID_Changed,(int)EventID_PlayOver,(int)EventID_UVPlayOver);
s_enumlist += buf;
sprintf(buf, "FogMode={Fog3D=%d,Fog3DGray=%d,Fog3DSub=%d,Fog3DHide=%d,Fog2D=%d,Fog2DGray=%d,Fog2DSub=%d,Fog2DHide=%d};",(int)FogMode_Fog3D,(int)FogMode_Fog3DGray,(int)FogMode_Fog3DSub,(int)FogMode_Fog3DHide,(int)FogMode_Fog2D,(int)FogMode_Fog2DGray,(int)FogMode_Fog2DSub,(int)FogMode_Fog2DHide);
s_enumlist += buf;
sprintf(buf, "MouseButton={Left=%d,Right=%d,Middle=%d};",(int)MouseButton_Left,(int)MouseButton_Right,(int)MouseButton_Middle);
s_enumlist += buf;
sprintf(buf, "TextureBlend={MODULATE=%d,MODULATE2X=%d,MODULATE4X=%d};",(int)TextureBlend_MODULATE,(int)TextureBlend_MODULATE2X,(int)TextureBlend_MODULATE4X);
s_enumlist += buf;
sprintf(buf, "TextureStage={Channel0=%d,Channel1=%d,Channel2=%d,Channel3=%d,Channel4=%d};",(int)TextureStage_Channel0,(int)TextureStage_Channel1,(int)TextureStage_Channel2,(int)TextureStage_Channel3,(int)TextureStage_Channel4);
s_enumlist += buf;
sprintf(buf, "UIControlType={BUTTON=%d,CHECKBOX=%d,COMBOBOX=%d,SLIDER=%d,EDITBOX=%d,IMEEDITBOX=%d,LISTBOX=%d,SCROLLBAR=%d,PROGRESS=%d,STATIC=%d,FORMAT_EDITBOX=%d,VIEW_FORMAT_TEXT=%d,LISTCTRL3D=%d,ui2DEffect=%d};",(int)UIControlType_BUTTON,(int)UIControlType_CHECKBOX,(int)UIControlType_COMBOBOX,(int)UIControlType_SLIDER,(int)UIControlType_EDITBOX,(int)UIControlType_IMEEDITBOX,(int)UIControlType_LISTBOX,(int)UIControlType_SCROLLBAR,(int)UIControlType_PROGRESS,(int)UIControlType_STATIC,(int)UIControlType_FORMAT_EDITBOX,(int)UIControlType_VIEW_FORMAT_TEXT,(int)UIControlType_LISTCTRL3D,(int)UIControlType_ui2DEffect);
s_enumlist += buf;
sprintf(buf, "UISoundPlay={mouseIn=%d,buttonDown=%d,count=%d};",(int)UISoundPlay_mouseIn,(int)UISoundPlay_buttonDown,(int)UISoundPlay_count);
s_enumlist += buf;}
if(luaL_loadstring(L, s_enumlist.c_str()) != 0)return false;
if(lua_pcall(L, 0, LUA_MULTRET, 0) != 0)return false;
return 0;
}

int initPhantomMath(lua_State* L)
{
class_add<lua_color4>(L, "color4");
class_mem<lua_color4>(L, "r", &lua_color4::r);
class_mem<lua_color4>(L, "g", &lua_color4::g);
class_mem<lua_color4>(L, "b", &lua_color4::b);
class_mem<lua_color4>(L, "a", &lua_color4::a);
class_con<lua_color4>(L, constructor<lua_color4>);
class_con<lua_color4>(L, constructor<lua_color4,float,float,float,float>);
class_def<lua_color4>(L, "__add", &lua_color4::operator+);
class_def<lua_color4>(L, "__sub", &lua_color4::operator-);
class_def<lua_color4>(L, "__mul", &lua_color4::operator*);
class_def<lua_color4>(L, "__div", &lua_color4::operator/);
class_def<lua_color4>(L, "__eq", &lua_color4::operator==);
class_def<lua_color4>(L, "fromrgb", &lua_color4::fromrgb);
class_def<lua_color4>(L, "fromrgba", &lua_color4::fromrgba);
class_def<lua_color4>(L, "rgb", &lua_color4::rgb);
class_def<lua_color4>(L, "set", &lua_color4::set);
class_property<lua_color4>(L, "rgba", &lua_color4::Getrgba, &lua_color4::Setrgba);
class_def<lua_color4>(L, "setup", &lua_color4::setup);

class_add<lua_float2>(L, "float2");
class_mem<lua_float2>(L, "x", &lua_float2::x);
class_mem<lua_float2>(L, "y", &lua_float2::y);
class_con<lua_float2>(L, constructor<lua_float2>);
class_con<lua_float2>(L, constructor<lua_float2,float,float>);
class_def<lua_float2>(L, "__add", &lua_float2::operator+);
class_def<lua_float2>(L, "__sub", &lua_float2::operator-);
class_def<lua_float2>(L, "__mul", &lua_float2::operator*);
class_def<lua_float2>(L, "__div", &lua_float2::operator/);
class_def<lua_float2>(L, "__eq", &lua_float2::operator==);
class_def<lua_float2>(L, "barycentric", &lua_float2::barycentric);
class_def<lua_float2>(L, "catmullrom", &lua_float2::catmullrom);
class_def<lua_float2>(L, "cross", &lua_float2::cross);
class_def<lua_float2>(L, "dot", &lua_float2::dot);
class_def<lua_float2>(L, "hermite", &lua_float2::hermite);
class_def<lua_float2>(L, "length", &lua_float2::length);
class_def<lua_float2>(L, "lerp", &lua_float2::lerp);
class_def<lua_float2>(L, "normalize", &lua_float2::normalize);
class_def<lua_float2>(L, "setup", &lua_float2::setup);
class_def<lua_float2>(L, "transformcoord", &lua_float2::transformcoord);
class_def<lua_float2>(L, "transformnormal", &lua_float2::transformnormal);

class_add<lua_float3>(L, "float3");
class_mem<lua_float3>(L, "x", &lua_float3::x);
class_mem<lua_float3>(L, "y", &lua_float3::y);
class_mem<lua_float3>(L, "z", &lua_float3::z);
class_con<lua_float3>(L, constructor<lua_float3>);
class_con<lua_float3>(L, constructor<lua_float3,float,float,float>);
class_def<lua_float3>(L, "__add", &lua_float3::operator+);
class_def<lua_float3>(L, "__sub", &lua_float3::operator-);
class_def<lua_float3>(L, "__mul", &lua_float3::operator*);
class_def<lua_float3>(L, "__div", &lua_float3::operator/);
class_def<lua_float3>(L, "__eq", &lua_float3::operator==);
class_def<lua_float3>(L, "barycentric", &lua_float3::barycentric);
class_def<lua_float3>(L, "catmullrom", &lua_float3::catmullrom);
class_def<lua_float3>(L, "cross", &lua_float3::cross);
class_def<lua_float3>(L, "dot", &lua_float3::dot);
class_def<lua_float3>(L, "getrotate_z", &lua_float3::getrotate_z);
class_def<lua_float3>(L, "hermite", &lua_float3::hermite);
class_def<lua_float3>(L, "length", &lua_float3::length);
class_def<lua_float3>(L, "lerp", &lua_float3::lerp);
class_def<lua_float3>(L, "normalize", &lua_float3::normalize);
class_def<lua_float3>(L, "rotateX", &lua_float3::rotateX);
class_def<lua_float3>(L, "rotateY", &lua_float3::rotateY);
class_def<lua_float3>(L, "rotateZ", &lua_float3::rotateZ);
class_def<lua_float3>(L, "setup", &lua_float3::setup);
class_def<lua_float3>(L, "transform", &lua_float3::transform);
class_def<lua_float3>(L, "transform3x3", &lua_float3::transform3x3);
class_def<lua_float3>(L, "transformcoord", &lua_float3::transformcoord);
class_def<lua_float3>(L, "transformnormal", &lua_float3::transformnormal);

class_add<lua_float4>(L, "float4");
class_mem<lua_float4>(L, "x", &lua_float4::x);
class_mem<lua_float4>(L, "y", &lua_float4::y);
class_mem<lua_float4>(L, "z", &lua_float4::z);
class_mem<lua_float4>(L, "w", &lua_float4::w);
class_con<lua_float4>(L, constructor<lua_float4>);
class_con<lua_float4>(L, constructor<lua_float4,float,float,float,float>);
class_def<lua_float4>(L, "__add", &lua_float4::operator+);
class_def<lua_float4>(L, "__sub", &lua_float4::operator-);
class_def<lua_float4>(L, "__mul", &lua_float4::operator*);
class_def<lua_float4>(L, "__div", &lua_float4::operator/);
class_def<lua_float4>(L, "__eq", &lua_float4::operator==);
class_def<lua_float4>(L, "barycentric", &lua_float4::barycentric);
class_def<lua_float4>(L, "catmullrom", &lua_float4::catmullrom);
class_def<lua_float4>(L, "copy", &lua_float4::copy);
class_def<lua_float4>(L, "dot", &lua_float4::dot);
class_def<lua_float4>(L, "hermite", &lua_float4::hermite);
class_def<lua_float4>(L, "length", &lua_float4::length);
class_def<lua_float4>(L, "lerp", &lua_float4::lerp);
class_def<lua_float4>(L, "setup", &lua_float4::setup);
class_def<lua_float4>(L, "transform", &lua_float4::transform);

class_add<lua_matrix4x4>(L, "matrix4x4");
class_mem<lua_matrix4x4>(L, "_11", &lua_matrix4x4::_11);
class_mem<lua_matrix4x4>(L, "_12", &lua_matrix4x4::_12);
class_mem<lua_matrix4x4>(L, "_13", &lua_matrix4x4::_13);
class_mem<lua_matrix4x4>(L, "_14", &lua_matrix4x4::_14);
class_mem<lua_matrix4x4>(L, "_21", &lua_matrix4x4::_21);
class_mem<lua_matrix4x4>(L, "_22", &lua_matrix4x4::_22);
class_mem<lua_matrix4x4>(L, "_23", &lua_matrix4x4::_23);
class_mem<lua_matrix4x4>(L, "_24", &lua_matrix4x4::_24);
class_mem<lua_matrix4x4>(L, "_31", &lua_matrix4x4::_31);
class_mem<lua_matrix4x4>(L, "_32", &lua_matrix4x4::_32);
class_mem<lua_matrix4x4>(L, "_33", &lua_matrix4x4::_33);
class_mem<lua_matrix4x4>(L, "_34", &lua_matrix4x4::_34);
class_mem<lua_matrix4x4>(L, "_41", &lua_matrix4x4::_41);
class_mem<lua_matrix4x4>(L, "_42", &lua_matrix4x4::_42);
class_mem<lua_matrix4x4>(L, "_43", &lua_matrix4x4::_43);
class_mem<lua_matrix4x4>(L, "_44", &lua_matrix4x4::_44);
class_con<lua_matrix4x4>(L, constructor<lua_matrix4x4>);
class_def<lua_matrix4x4>(L, "__add", &lua_matrix4x4::operator+);
class_def<lua_matrix4x4>(L, "__sub", &lua_matrix4x4::operator-);
class_def<lua_matrix4x4>(L, "__mul", &lua_matrix4x4::operator*);
class_def<lua_matrix4x4>(L, "__div", &lua_matrix4x4::operator/);
class_def<lua_matrix4x4>(L, "__eq", &lua_matrix4x4::operator==);
class_def<lua_matrix4x4>(L, "affineTransformation", &lua_matrix4x4::affineTransformation);
class_def<lua_matrix4x4>(L, "affineTransformation2d", &lua_matrix4x4::affineTransformation2d);
class_def<lua_matrix4x4>(L, "decompose", &lua_matrix4x4::decompose);
class_def<lua_matrix4x4>(L, "identity", &lua_matrix4x4::identity);
class_def<lua_matrix4x4>(L, "inverse", &lua_matrix4x4::inverse);
class_def<lua_matrix4x4>(L, "inverse2", &lua_matrix4x4::inverse2);
class_def<lua_matrix4x4>(L, "lerp", &lua_matrix4x4::lerp);
class_def<lua_matrix4x4>(L, "lookat_lh", &lua_matrix4x4::lookat_lh);
class_def<lua_matrix4x4>(L, "lookat_rh", &lua_matrix4x4::lookat_rh);
class_def<lua_matrix4x4>(L, "multiply_transpose", &lua_matrix4x4::multiply_transpose);
class_def<lua_matrix4x4>(L, "orth_offcenter_lh", &lua_matrix4x4::orth_offcenter_lh);
class_def<lua_matrix4x4>(L, "ortho_lh", &lua_matrix4x4::ortho_lh);
class_def<lua_matrix4x4>(L, "ortho_offcenter_rh", &lua_matrix4x4::ortho_offcenter_rh);
class_def<lua_matrix4x4>(L, "ortho_rh", &lua_matrix4x4::ortho_rh);
class_def<lua_matrix4x4>(L, "perspective_fovrh", &lua_matrix4x4::perspective_fovrh);
class_def<lua_matrix4x4>(L, "perspective_lv", &lua_matrix4x4::perspective_lv);
class_def<lua_matrix4x4>(L, "perspective_offcenter_lh", &lua_matrix4x4::perspective_offcenter_lh);
class_def<lua_matrix4x4>(L, "perspective_offcenter_rh", &lua_matrix4x4::perspective_offcenter_rh);
class_def<lua_matrix4x4>(L, "perspective_rh", &lua_matrix4x4::perspective_rh);
class_def<lua_matrix4x4>(L, "perspectivefov_lh", &lua_matrix4x4::perspectivefov_lh);
class_def<lua_matrix4x4>(L, "reflect", &lua_matrix4x4::reflect);
class_def<lua_matrix4x4>(L, "rotation_yawpitchroll", &lua_matrix4x4::rotation_yawpitchroll);
class_def<lua_matrix4x4>(L, "rotationaxis", &lua_matrix4x4::rotationaxis);
class_def<lua_matrix4x4>(L, "rotationquaternion", &lua_matrix4x4::rotationquaternion);
class_def<lua_matrix4x4>(L, "rotationx", &lua_matrix4x4::rotationx);
class_def<lua_matrix4x4>(L, "rotationy", &lua_matrix4x4::rotationy);
class_def<lua_matrix4x4>(L, "rotationz", &lua_matrix4x4::rotationz);
class_def<lua_matrix4x4>(L, "scaling", &lua_matrix4x4::scaling);
class_property<lua_matrix4x4>(L, "pos", &lua_matrix4x4::Getpos, &lua_matrix4x4::Setpos);
class_def<lua_matrix4x4>(L, "transform", &lua_matrix4x4::transform);
class_def<lua_matrix4x4>(L, "translation", &lua_matrix4x4::translation);
class_def<lua_matrix4x4>(L, "transpose", &lua_matrix4x4::transpose);

class_add<lua_Pixel>(L, "Pixel");
class_mem<lua_Pixel>(L, "x", &lua_Pixel::x);
class_mem<lua_Pixel>(L, "y", &lua_Pixel::y);
class_con<lua_Pixel>(L, constructor<lua_Pixel>);
class_con<lua_Pixel>(L, constructor<lua_Pixel,int,int>);
class_def<lua_Pixel>(L, "__add", &lua_Pixel::operator+);
class_def<lua_Pixel>(L, "__sub", &lua_Pixel::operator-);
class_def<lua_Pixel>(L, "__mul", &lua_Pixel::operator*);
class_def<lua_Pixel>(L, "__div", &lua_Pixel::operator/);
class_def<lua_Pixel>(L, "__eq", &lua_Pixel::operator==);

class_add<lua_PixelColor>(L, "PixelColor");
class_mem<lua_PixelColor>(L, "x", &lua_PixelColor::x);
class_mem<lua_PixelColor>(L, "y", &lua_PixelColor::y);
class_mem<lua_PixelColor>(L, "color", &lua_PixelColor::color);
class_con<lua_PixelColor>(L, constructor<lua_PixelColor>);
class_con<lua_PixelColor>(L, constructor<lua_PixelColor,int,int,color4>);
class_def<lua_PixelColor>(L, "__add", &lua_PixelColor::operator+);
class_def<lua_PixelColor>(L, "__sub", &lua_PixelColor::operator-);
class_def<lua_PixelColor>(L, "__mul", &lua_PixelColor::operator*);
class_def<lua_PixelColor>(L, "__div", &lua_PixelColor::operator/);
class_def<lua_PixelColor>(L, "__eq", &lua_PixelColor::operator==);

class_add<lua_Plane>(L, "Plane");
class_mem<lua_Plane>(L, "a", &lua_Plane::a);
class_mem<lua_Plane>(L, "b", &lua_Plane::b);
class_mem<lua_Plane>(L, "c", &lua_Plane::c);
class_mem<lua_Plane>(L, "d", &lua_Plane::d);
class_con<lua_Plane>(L, constructor<lua_Plane>);
class_con<lua_Plane>(L, constructor<lua_Plane,float,float,float,float>);
class_def<lua_Plane>(L, "__add", &lua_Plane::operator+);
class_def<lua_Plane>(L, "__sub", &lua_Plane::operator-);
class_def<lua_Plane>(L, "__mul", &lua_Plane::operator*);
class_def<lua_Plane>(L, "__div", &lua_Plane::operator/);
class_def<lua_Plane>(L, "__eq", &lua_Plane::operator==);
class_def<lua_Plane>(L, "dot", &lua_Plane::dot);
class_def<lua_Plane>(L, "dotcoord", &lua_Plane::dotcoord);
class_def<lua_Plane>(L, "dotnormal", &lua_Plane::dotnormal);
class_def<lua_Plane>(L, "from_pointnormal", &lua_Plane::from_pointnormal);
class_def<lua_Plane>(L, "from_points", &lua_Plane::from_points);
class_def<lua_Plane>(L, "intersectline", &lua_Plane::intersectline);
class_def<lua_Plane>(L, "normalize", &lua_Plane::normalize);
class_def<lua_Plane>(L, "scale", &lua_Plane::scale);
class_def<lua_Plane>(L, "transform", &lua_Plane::transform);

class_add<lua_quaternion>(L, "quaternion");
class_mem<lua_quaternion>(L, "x", &lua_quaternion::x);
class_mem<lua_quaternion>(L, "y", &lua_quaternion::y);
class_mem<lua_quaternion>(L, "z", &lua_quaternion::z);
class_mem<lua_quaternion>(L, "w", &lua_quaternion::w);
class_con<lua_quaternion>(L, constructor<lua_quaternion>);
class_con<lua_quaternion>(L, constructor<lua_quaternion,float,float,float,float>);
class_def<lua_quaternion>(L, "__add", &lua_quaternion::operator+);
class_def<lua_quaternion>(L, "__sub", &lua_quaternion::operator-);
class_def<lua_quaternion>(L, "__mul", &lua_quaternion::operator*);
class_def<lua_quaternion>(L, "__div", &lua_quaternion::operator/);
class_def<lua_quaternion>(L, "__eq", &lua_quaternion::operator==);
class_def<lua_quaternion>(L, "barycentric", &lua_quaternion::barycentric);
class_def<lua_quaternion>(L, "exp", &lua_quaternion::exp);
class_def<lua_quaternion>(L, "inverse", &lua_quaternion::inverse);
class_def<lua_quaternion>(L, "ln", &lua_quaternion::ln);
class_def<lua_quaternion>(L, "multiply", &lua_quaternion::multiply);
class_def<lua_quaternion>(L, "normalize", &lua_quaternion::normalize);
class_def<lua_quaternion>(L, "rotation_yawpitchroll", &lua_quaternion::rotation_yawpitchroll);
class_def<lua_quaternion>(L, "rotationaxis", &lua_quaternion::rotationaxis);
class_def<lua_quaternion>(L, "rotationmatrix", &lua_quaternion::rotationmatrix);
class_def<lua_quaternion>(L, "slerp", &lua_quaternion::slerp);
class_def<lua_quaternion>(L, "squad", &lua_quaternion::squad);

class_add<lua_Rect>(L, "Rect");
class_mem<lua_Rect>(L, "left", &lua_Rect::left);
class_mem<lua_Rect>(L, "top", &lua_Rect::top);
class_mem<lua_Rect>(L, "right", &lua_Rect::right);
class_mem<lua_Rect>(L, "bottom", &lua_Rect::bottom);
class_con<lua_Rect>(L, constructor<lua_Rect>);
class_con<lua_Rect>(L, constructor<lua_Rect,int,int,int,int>);
class_def<lua_Rect>(L, "__add", &lua_Rect::operator+);
class_def<lua_Rect>(L, "__sub", &lua_Rect::operator-);
class_def<lua_Rect>(L, "__mul", &lua_Rect::operator*);
class_def<lua_Rect>(L, "__div", &lua_Rect::operator/);
class_def<lua_Rect>(L, "__eq", &lua_Rect::operator==);

class_add<lua_AABox>(L, "AABox");
class_mem<lua_AABox>(L, "_min", &lua_AABox::_min);
class_mem<lua_AABox>(L, "_max", &lua_AABox::_max);
class_con<lua_AABox>(L, constructor<lua_AABox>);
class_con<lua_AABox>(L, constructor<lua_AABox,float3,float3>);

class_add<lua_CameraData>(L, "CameraData");
class_mem<lua_CameraData>(L, "flags", &lua_CameraData::flags);
class_mem<lua_CameraData>(L, "rotButton", &lua_CameraData::rotButton);
class_mem<lua_CameraData>(L, "moveButton", &lua_CameraData::moveButton);
class_mem<lua_CameraData>(L, "vRotate", &lua_CameraData::vRotate);
class_mem<lua_CameraData>(L, "fRotationSpeedMask", &lua_CameraData::fRotationSpeedMask);
class_mem<lua_CameraData>(L, "vUp", &lua_CameraData::vUp);
class_mem<lua_CameraData>(L, "vTarget", &lua_CameraData::vTarget);
class_mem<lua_CameraData>(L, "vEye", &lua_CameraData::vEye);
class_mem<lua_CameraData>(L, "fFov", &lua_CameraData::fFov);
class_mem<lua_CameraData>(L, "nWidth", &lua_CameraData::nWidth);
class_mem<lua_CameraData>(L, "nHeight", &lua_CameraData::nHeight);
class_mem<lua_CameraData>(L, "fZNear", &lua_CameraData::fZNear);
class_mem<lua_CameraData>(L, "fZFar", &lua_CameraData::fZFar);
class_mem<lua_CameraData>(L, "fMinDistance", &lua_CameraData::fMinDistance);
class_mem<lua_CameraData>(L, "fMaxDistance", &lua_CameraData::fMaxDistance);
class_mem<lua_CameraData>(L, "fMoveSpeed", &lua_CameraData::fMoveSpeed);
class_con<lua_CameraData>(L, constructor<lua_CameraData>);

class_add<lua_floatPtr>(L, "floatPtr");
class_mem<lua_floatPtr>(L, "value", &lua_floatPtr::value);
class_con<lua_floatPtr>(L, constructor<lua_floatPtr>);
class_con<lua_floatPtr>(L, constructor<lua_floatPtr,float>);

class_add<lua_GameID>(L, "GameID");
class_mem<lua_GameID>(L, "id", &lua_GameID::id);
class_mem<lua_GameID>(L, "index", &lua_GameID::index);
class_con<lua_GameID>(L, constructor<lua_GameID>);
class_con<lua_GameID>(L, constructor<lua_GameID,short,short>);

class_add<lua_Object>(L, "Object");
class_mem<lua_Object>(L, "s1", &lua_Object::s1);
class_mem<lua_Object>(L, "s2", &lua_Object::s2);
class_mem<lua_Object>(L, "i1", &lua_Object::i1);
class_mem<lua_Object>(L, "i2", &lua_Object::i2);
class_con<lua_Object>(L, constructor<lua_Object>);
class_con<lua_Object>(L, constructor<lua_Object,short,short,int,int>);

class_add<lua_PickPoint>(L, "PickPoint");
class_mem<lua_PickPoint>(L, "pos", &lua_PickPoint::pos);
class_mem<lua_PickPoint>(L, "normal", &lua_PickPoint::normal);
class_mem<lua_PickPoint>(L, "t", &lua_PickPoint::t);
class_con<lua_PickPoint>(L, constructor<lua_PickPoint>);
class_con<lua_PickPoint>(L, constructor<lua_PickPoint,float3,float3,float>);

class_add<lua_PixelUV>(L, "PixelUV");
class_mem<lua_PixelUV>(L, "x", &lua_PixelUV::x);
class_mem<lua_PixelUV>(L, "y", &lua_PixelUV::y);
class_mem<lua_PixelUV>(L, "u", &lua_PixelUV::u);
class_mem<lua_PixelUV>(L, "v", &lua_PixelUV::v);
class_mem<lua_PixelUV>(L, "c", &lua_PixelUV::c);
class_con<lua_PixelUV>(L, constructor<lua_PixelUV>);

class_add<lua_sensordata>(L, "sensordata");
class_mem<lua_sensordata>(L, "acceleration_x", &lua_sensordata::acceleration_x);
class_mem<lua_sensordata>(L, "acceleration_y", &lua_sensordata::acceleration_y);
class_mem<lua_sensordata>(L, "acceleration_z", &lua_sensordata::acceleration_z);
class_mem<lua_sensordata>(L, "gravity_x", &lua_sensordata::gravity_x);
class_mem<lua_sensordata>(L, "gravity_y", &lua_sensordata::gravity_y);
class_mem<lua_sensordata>(L, "gravity_z", &lua_sensordata::gravity_z);
class_mem<lua_sensordata>(L, "theta_z", &lua_sensordata::theta_z);
class_mem<lua_sensordata>(L, "theta_xy", &lua_sensordata::theta_xy);
class_mem<lua_sensordata>(L, "rotation_rate_x", &lua_sensordata::rotation_rate_x);
class_mem<lua_sensordata>(L, "rotation_rate_y", &lua_sensordata::rotation_rate_y);
class_mem<lua_sensordata>(L, "rotation_rate_z", &lua_sensordata::rotation_rate_z);
class_mem<lua_sensordata>(L, "roll", &lua_sensordata::roll);
class_mem<lua_sensordata>(L, "pitch", &lua_sensordata::pitch);
class_mem<lua_sensordata>(L, "yaw", &lua_sensordata::yaw);
class_mem<lua_sensordata>(L, "qx", &lua_sensordata::qx);
class_mem<lua_sensordata>(L, "qy", &lua_sensordata::qy);
class_mem<lua_sensordata>(L, "qz", &lua_sensordata::qz);
class_mem<lua_sensordata>(L, "qw", &lua_sensordata::qw);
class_con<lua_sensordata>(L, constructor<lua_sensordata>);

class_add<lua_short2>(L, "short2");
class_mem<lua_short2>(L, "x", &lua_short2::x);
class_mem<lua_short2>(L, "y", &lua_short2::y);
class_con<lua_short2>(L, constructor<lua_short2>);
class_con<lua_short2>(L, constructor<lua_short2,short,short>);

class_add<lua_short4>(L, "short4");
class_mem<lua_short4>(L, "x", &lua_short4::x);
class_mem<lua_short4>(L, "y", &lua_short4::y);
class_mem<lua_short4>(L, "z", &lua_short4::z);
class_mem<lua_short4>(L, "w", &lua_short4::w);
class_con<lua_short4>(L, constructor<lua_short4>);
class_con<lua_short4>(L, constructor<lua_short4,short,short,short,short>);

class_add<lua_Time>(L, "Time");
class_mem<lua_Time>(L, "year", &lua_Time::year);
class_mem<lua_Time>(L, "month", &lua_Time::month);
class_mem<lua_Time>(L, "day", &lua_Time::day);
class_mem<lua_Time>(L, "hour", &lua_Time::hour);
class_mem<lua_Time>(L, "minute", &lua_Time::minute);
class_mem<lua_Time>(L, "second", &lua_Time::second);
class_mem<lua_Time>(L, "microsecond", &lua_Time::microsecond);
class_con<lua_Time>(L, constructor<lua_Time>);

class_add<lua_Triangle>(L, "Triangle");
class_mem<lua_Triangle>(L, "v1", &lua_Triangle::v1);
class_mem<lua_Triangle>(L, "v2", &lua_Triangle::v2);
class_mem<lua_Triangle>(L, "v3", &lua_Triangle::v3);
class_con<lua_Triangle>(L, constructor<lua_Triangle>);
class_con<lua_Triangle>(L, constructor<lua_Triangle,short,short,short>);

class_add<lua_ubyte4>(L, "ubyte4");
class_mem<lua_ubyte4>(L, "x", &lua_ubyte4::x);
class_mem<lua_ubyte4>(L, "y", &lua_ubyte4::y);
class_mem<lua_ubyte4>(L, "z", &lua_ubyte4::z);
class_mem<lua_ubyte4>(L, "w", &lua_ubyte4::w);
class_con<lua_ubyte4>(L, constructor<lua_ubyte4>);
class_con<lua_ubyte4>(L, constructor<lua_ubyte4,char,char,char,char>);

class_add<lua_Vertex>(L, "Vertex");
class_mem<lua_Vertex>(L, "pos", &lua_Vertex::pos);
class_mem<lua_Vertex>(L, "normal", &lua_Vertex::normal);
class_mem<lua_Vertex>(L, "uv1", &lua_Vertex::uv1);
class_mem<lua_Vertex>(L, "uv2", &lua_Vertex::uv2);
class_con<lua_Vertex>(L, constructor<lua_Vertex>);
class_con<lua_Vertex>(L, constructor<lua_Vertex,float3,float3,float2,float2>);

class_add<lua_DataArray_s>(L, "DataArray");
class_property<lua_DataArray_s>(L, "context", &lua_DataArray_s::getcontext, &lua_DataArray_s::setcontext);
class_def<lua_DataArray_s>(L, "isnull", &lua_DataArray_s::isnull);
class_def<lua_DataArray_s>(L, "equal", &lua_DataArray_s::equal);
class_def<lua_DataArray_s>(L, "__eq", &lua_DataArray_s::operator == );

class_add<lua_DataControl_s>(L, "DataControl");
class_property<lua_DataControl_s>(L, "context", &lua_DataControl_s::getcontext, &lua_DataControl_s::setcontext);
class_def<lua_DataControl_s>(L, "isnull", &lua_DataControl_s::isnull);
class_def<lua_DataControl_s>(L, "equal", &lua_DataControl_s::equal);
class_def<lua_DataControl_s>(L, "__eq", &lua_DataControl_s::operator == );
class_def<lua_DataControl_s>(L, "add_child", &lua_DataControl_s::add_child);
class_def<lua_DataControl_s>(L, "at", &lua_DataControl_s::at);
class_def<lua_DataControl_s>(L, "clear", &lua_DataControl_s::clear);
class_def<lua_DataControl_s>(L, "count", &lua_DataControl_s::count);
class_def<lua_DataControl_s>(L, "lreadf", &lua_DataControl_s::lreadf);
class_def<lua_DataControl_s>(L, "lreadi", &lua_DataControl_s::lreadi);
class_def<lua_DataControl_s>(L, "lreadt", &lua_DataControl_s::lreadt);
class_def<lua_DataControl_s>(L, "lwritef", &lua_DataControl_s::lwritef);
class_def<lua_DataControl_s>(L, "lwritei", &lua_DataControl_s::lwritei);
class_def<lua_DataControl_s>(L, "lwritet", &lua_DataControl_s::lwritet);
class_def<lua_DataControl_s>(L, "readf", &lua_DataControl_s::readf);
class_def<lua_DataControl_s>(L, "readi", &lua_DataControl_s::readi);
class_def<lua_DataControl_s>(L, "readt", &lua_DataControl_s::readt);
class_property<lua_DataControl_s>(L, "lcount", &lua_DataControl_s::Getlcount, &lua_DataControl_s::Setlcount);
class_def<lua_DataControl_s>(L, "writef", &lua_DataControl_s::writef);
class_def<lua_DataControl_s>(L, "writei", &lua_DataControl_s::writei);
class_def<lua_DataControl_s>(L, "writet", &lua_DataControl_s::writet);

class_add<lua_FindWay_s>(L, "FindWay");
class_property<lua_FindWay_s>(L, "context", &lua_FindWay_s::getcontext, &lua_FindWay_s::setcontext);
class_def<lua_FindWay_s>(L, "isnull", &lua_FindWay_s::isnull);
class_def<lua_FindWay_s>(L, "equal", &lua_FindWay_s::equal);
class_def<lua_FindWay_s>(L, "__eq", &lua_FindWay_s::operator == );
class_def<lua_FindWay_s>(L, "point_at", &lua_FindWay_s::point_at);
class_def<lua_FindWay_s>(L, "recalc", &lua_FindWay_s::recalc);
class_property<lua_FindWay_s>(L, "is2dway", &lua_FindWay_s::Getis2dway, &lua_FindWay_s::Setis2dway);
class_property<lua_FindWay_s>(L, "pause", &lua_FindWay_s::Getpause, &lua_FindWay_s::Setpause);
class_property<lua_FindWay_s>(L, "play", &lua_FindWay_s::Getplay, &lua_FindWay_s::Setplay);
class_def<lua_FindWay_s>(L, "setpoint_at", &lua_FindWay_s::setpoint_at);
class_property<lua_FindWay_s>(L, "pointcount", &lua_FindWay_s::Getpointcount, &lua_FindWay_s::Setpointcount);
class_property<lua_FindWay_s>(L, "speed", &lua_FindWay_s::Getspeed, &lua_FindWay_s::Setspeed);

class_add<lua_GameObject_s>(L, "GameObject");
class_property<lua_GameObject_s>(L, "context", &lua_GameObject_s::getcontext, &lua_GameObject_s::setcontext);
class_def<lua_GameObject_s>(L, "isnull", &lua_GameObject_s::isnull);
class_def<lua_GameObject_s>(L, "equal", &lua_GameObject_s::equal);
class_def<lua_GameObject_s>(L, "__eq", &lua_GameObject_s::operator == );
class_def<lua_GameObject_s>(L, "action", &lua_GameObject_s::action);
class_def<lua_GameObject_s>(L, "action2", &lua_GameObject_s::action2);
class_def<lua_GameObject_s>(L, "attack", &lua_GameObject_s::attack);
class_def<lua_GameObject_s>(L, "bind_action_event", &lua_GameObject_s::bind_action_event);
class_def<lua_GameObject_s>(L, "bind_action_over", &lua_GameObject_s::bind_action_over);
class_def<lua_GameObject_s>(L, "bind_attack_over", &lua_GameObject_s::bind_attack_over);
class_def<lua_GameObject_s>(L, "bind_die_over", &lua_GameObject_s::bind_die_over);
class_def<lua_GameObject_s>(L, "bind_hit_over", &lua_GameObject_s::bind_hit_over);
class_def<lua_GameObject_s>(L, "bind_idle_over", &lua_GameObject_s::bind_idle_over);
class_def<lua_GameObject_s>(L, "bind_move_over", &lua_GameObject_s::bind_move_over);
class_def<lua_GameObject_s>(L, "bind_nextbegin", &lua_GameObject_s::bind_nextbegin);
class_def<lua_GameObject_s>(L, "clone", &lua_GameObject_s::clone);
class_def<lua_GameObject_s>(L, "die", &lua_GameObject_s::die);
class_def<lua_GameObject_s>(L, "doskill_r", &lua_GameObject_s::doskill_r);
class_def<lua_GameObject_s>(L, "doskill_t", &lua_GameObject_s::doskill_t);
class_def<lua_GameObject_s>(L, "height", &lua_GameObject_s::height);
class_def<lua_GameObject_s>(L, "idle", &lua_GameObject_s::idle);
class_def<lua_GameObject_s>(L, "lookat", &lua_GameObject_s::lookat);
class_def<lua_GameObject_s>(L, "lookat_t", &lua_GameObject_s::lookat_t);
class_def<lua_GameObject_s>(L, "movedir", &lua_GameObject_s::movedir);
class_def<lua_GameObject_s>(L, "moveto", &lua_GameObject_s::moveto);
class_def<lua_GameObject_s>(L, "moveto_b", &lua_GameObject_s::moveto_b);
class_def<lua_GameObject_s>(L, "moveto_e", &lua_GameObject_s::moveto_e);
class_def<lua_GameObject_s>(L, "moveto_t", &lua_GameObject_s::moveto_t);
class_def<lua_GameObject_s>(L, "next_setid", &lua_GameObject_s::next_setid);
class_def<lua_GameObject_s>(L, "next_setrandom", &lua_GameObject_s::next_setrandom);
class_def<lua_GameObject_s>(L, "nextaction", &lua_GameObject_s::nextaction);
class_def<lua_GameObject_s>(L, "nextaction2", &lua_GameObject_s::nextaction2);
class_def<lua_GameObject_s>(L, "nextclear", &lua_GameObject_s::nextclear);
class_def<lua_GameObject_s>(L, "nextenemy", &lua_GameObject_s::nextenemy);
class_def<lua_GameObject_s>(L, "nextidle", &lua_GameObject_s::nextidle);
class_def<lua_GameObject_s>(L, "nextlookat", &lua_GameObject_s::nextlookat);
class_def<lua_GameObject_s>(L, "nextlookat_t", &lua_GameObject_s::nextlookat_t);
class_def<lua_GameObject_s>(L, "nextloop", &lua_GameObject_s::nextloop);
class_def<lua_GameObject_s>(L, "nextto", &lua_GameObject_s::nextto);
class_def<lua_GameObject_s>(L, "nextto_e", &lua_GameObject_s::nextto_e);
class_def<lua_GameObject_s>(L, "onhit", &lua_GameObject_s::onhit);
class_def<lua_GameObject_s>(L, "onhit2", &lua_GameObject_s::onhit2);
class_def<lua_GameObject_s>(L, "pause_t", &lua_GameObject_s::pause_t);
class_def<lua_GameObject_s>(L, "read", &lua_GameObject_s::read);
class_def<lua_GameObject_s>(L, "roleid", &lua_GameObject_s::roleid);
class_def<lua_GameObject_s>(L, "sceneid", &lua_GameObject_s::sceneid);
class_def<lua_GameObject_s>(L, "sceneobj", &lua_GameObject_s::sceneobj);
class_property<lua_GameObject_s>(L, "affrange", &lua_GameObject_s::Getaffrange, &lua_GameObject_s::Setaffrange);
class_property<lua_GameObject_s>(L, "atkrange", &lua_GameObject_s::Getatkrange, &lua_GameObject_s::Setatkrange);
class_property<lua_GameObject_s>(L, "bornpos", &lua_GameObject_s::Getbornpos, &lua_GameObject_s::Setbornpos);
class_property<lua_GameObject_s>(L, "hit_text", &lua_GameObject_s::Gethit_text, &lua_GameObject_s::Sethit_text);
class_property<lua_GameObject_s>(L, "meshheight", &lua_GameObject_s::Getmeshheight, &lua_GameObject_s::Setmeshheight);
class_property<lua_GameObject_s>(L, "movedir_enable", &lua_GameObject_s::Getmovedir_enable, &lua_GameObject_s::Setmovedir_enable);
class_property<lua_GameObject_s>(L, "moverun", &lua_GameObject_s::Getmoverun, &lua_GameObject_s::Setmoverun);
class_property<lua_GameObject_s>(L, "nextpause", &lua_GameObject_s::Getnextpause, &lua_GameObject_s::Setnextpause);
class_property<lua_GameObject_s>(L, "pos", &lua_GameObject_s::Getpos, &lua_GameObject_s::Setpos);
class_property<lua_GameObject_s>(L, "rot", &lua_GameObject_s::Getrot, &lua_GameObject_s::Setrot);
class_property<lua_GameObject_s>(L, "searchway", &lua_GameObject_s::Getsearchway, &lua_GameObject_s::Setsearchway);
class_property<lua_GameObject_s>(L, "target", &lua_GameObject_s::Gettarget, &lua_GameObject_s::Settarget);
class_def<lua_GameObject_s>(L, "update_blood", &lua_GameObject_s::update_blood);
class_def<lua_GameObject_s>(L, "write", &lua_GameObject_s::write);

class_add<lua_Map3D_s>(L, "Map3D");
class_property<lua_Map3D_s>(L, "context", &lua_Map3D_s::getcontext, &lua_Map3D_s::setcontext);
class_def<lua_Map3D_s>(L, "isnull", &lua_Map3D_s::isnull);
class_def<lua_Map3D_s>(L, "equal", &lua_Map3D_s::equal);
class_def<lua_Map3D_s>(L, "__eq", &lua_Map3D_s::operator == );
class_def<lua_Map3D_s>(L, "at", &lua_Map3D_s::at);
class_def<lua_Map3D_s>(L, "bind_click", &lua_Map3D_s::bind_click);
class_def<lua_Map3D_s>(L, "bind_draw", &lua_Map3D_s::bind_draw);
class_def<lua_Map3D_s>(L, "bind_event", &lua_Map3D_s::bind_event);
class_def<lua_Map3D_s>(L, "bind_eventby", &lua_Map3D_s::bind_eventby);
class_def<lua_Map3D_s>(L, "bind_mapevent", &lua_Map3D_s::bind_mapevent);
class_def<lua_Map3D_s>(L, "bind_mapeventby", &lua_Map3D_s::bind_mapeventby);
class_def<lua_Map3D_s>(L, "blend_camera", &lua_Map3D_s::blend_camera);
class_def<lua_Map3D_s>(L, "blend_fog", &lua_Map3D_s::blend_fog);
class_def<lua_Map3D_s>(L, "blend_sky", &lua_Map3D_s::blend_sky);
class_def<lua_Map3D_s>(L, "camerafollow", &lua_Map3D_s::camerafollow);
class_def<lua_Map3D_s>(L, "classat", &lua_Map3D_s::classat);
class_def<lua_Map3D_s>(L, "classcount", &lua_Map3D_s::classcount);
class_def<lua_Map3D_s>(L, "count", &lua_Map3D_s::count);
class_def<lua_Map3D_s>(L, "enable", &lua_Map3D_s::enable);
class_def<lua_Map3D_s>(L, "enable_s", &lua_Map3D_s::enable_s);
class_def<lua_Map3D_s>(L, "envat", &lua_Map3D_s::envat);
class_def<lua_Map3D_s>(L, "envcount", &lua_Map3D_s::envcount);
class_def<lua_Map3D_s>(L, "freeskill", &lua_Map3D_s::freeskill);
class_def<lua_Map3D_s>(L, "insert", &lua_Map3D_s::insert);
class_def<lua_Map3D_s>(L, "remove", &lua_Map3D_s::remove);
class_def<lua_Map3D_s>(L, "replace_image", &lua_Map3D_s::replace_image);
class_def<lua_Map3D_s>(L, "scenepos", &lua_Map3D_s::scenepos);
class_def<lua_Map3D_s>(L, "screenpos", &lua_Map3D_s::screenpos);
class_def<lua_Map3D_s>(L, "search", &lua_Map3D_s::search);
class_def<lua_Map3D_s>(L, "searchby", &lua_Map3D_s::searchby);
class_def<lua_Map3D_s>(L, "searchpoint", &lua_Map3D_s::searchpoint);
class_def<lua_Map3D_s>(L, "searchpointby", &lua_Map3D_s::searchpointby);
class_def<lua_Map3D_s>(L, "searchpointby_eid", &lua_Map3D_s::searchpointby_eid);
class_property<lua_Map3D_s>(L, "activecamera", &lua_Map3D_s::Getactivecamera, &lua_Map3D_s::Setactivecamera);
class_property<lua_Map3D_s>(L, "activeEnv", &lua_Map3D_s::GetactiveEnv, &lua_Map3D_s::SetactiveEnv);
class_property<lua_Map3D_s>(L, "activeEnvBy", &lua_Map3D_s::GetactiveEnvBy, &lua_Map3D_s::SetactiveEnvBy);
class_def<lua_Map3D_s>(L, "setblood", &lua_Map3D_s::setblood);
class_property<lua_Map3D_s>(L, "blooddisplay", &lua_Map3D_s::Getblooddisplay, &lua_Map3D_s::Setblooddisplay);
class_property<lua_Map3D_s>(L, "bloodsize", &lua_Map3D_s::Getbloodsize, &lua_Map3D_s::Setbloodsize);
class_property<lua_Map3D_s>(L, "camera", &lua_Map3D_s::Getcamera, &lua_Map3D_s::Setcamera);
class_property<lua_Map3D_s>(L, "clickradius", &lua_Map3D_s::Getclickradius, &lua_Map3D_s::Setclickradius);
class_property<lua_Map3D_s>(L, "display", &lua_Map3D_s::Getdisplay, &lua_Map3D_s::Setdisplay);
class_property<lua_Map3D_s>(L, "displayterrain", &lua_Map3D_s::Getdisplayterrain, &lua_Map3D_s::Setdisplayterrain);
class_property<lua_Map3D_s>(L, "enabled", &lua_Map3D_s::Getenabled, &lua_Map3D_s::Setenabled);
class_property<lua_Map3D_s>(L, "enablemyclick", &lua_Map3D_s::Getenablemyclick, &lua_Map3D_s::Setenablemyclick);
class_property<lua_Map3D_s>(L, "findway2d", &lua_Map3D_s::Getfindway2d, &lua_Map3D_s::Setfindway2d);
class_property<lua_Map3D_s>(L, "hitdisplay", &lua_Map3D_s::Gethitdisplay, &lua_Map3D_s::Sethitdisplay);
class_property<lua_Map3D_s>(L, "pause", &lua_Map3D_s::Getpause, &lua_Map3D_s::Setpause);
class_property<lua_Map3D_s>(L, "sky", &lua_Map3D_s::Getsky, &lua_Map3D_s::Setsky);
class_def<lua_Map3D_s>(L, "show", &lua_Map3D_s::show);
class_def<lua_Map3D_s>(L, "show_s", &lua_Map3D_s::show_s);
class_def<lua_Map3D_s>(L, "skyat", &lua_Map3D_s::skyat);
class_def<lua_Map3D_s>(L, "skycount", &lua_Map3D_s::skycount);

class_add<lua_Map3DEnv_s>(L, "Map3DEnv");
class_property<lua_Map3DEnv_s>(L, "context", &lua_Map3DEnv_s::getcontext, &lua_Map3DEnv_s::setcontext);
class_def<lua_Map3DEnv_s>(L, "isnull", &lua_Map3DEnv_s::isnull);
class_def<lua_Map3DEnv_s>(L, "equal", &lua_Map3DEnv_s::equal);
class_def<lua_Map3DEnv_s>(L, "__eq", &lua_Map3DEnv_s::operator == );
class_property<lua_Map3DEnv_s>(L, "brightness", &lua_Map3DEnv_s::Getbrightness, &lua_Map3DEnv_s::Setbrightness);
class_property<lua_Map3DEnv_s>(L, "color", &lua_Map3DEnv_s::Getcolor, &lua_Map3DEnv_s::Setcolor);
class_property<lua_Map3DEnv_s>(L, "contrast", &lua_Map3DEnv_s::Getcontrast, &lua_Map3DEnv_s::Setcontrast);
class_property<lua_Map3DEnv_s>(L, "enabled_brightness", &lua_Map3DEnv_s::Getenabled_brightness, &lua_Map3DEnv_s::Setenabled_brightness);
class_property<lua_Map3DEnv_s>(L, "enabled_contrast", &lua_Map3DEnv_s::Getenabled_contrast, &lua_Map3DEnv_s::Setenabled_contrast);
class_property<lua_Map3DEnv_s>(L, "far", &lua_Map3DEnv_s::Getfar, &lua_Map3DEnv_s::Setfar);
class_property<lua_Map3DEnv_s>(L, "mode", &lua_Map3DEnv_s::Getmode, &lua_Map3DEnv_s::Setmode);
class_property<lua_Map3DEnv_s>(L, "name", &lua_Map3DEnv_s::Getname, &lua_Map3DEnv_s::Setname);
class_property<lua_Map3DEnv_s>(L, "near", &lua_Map3DEnv_s::Getnear, &lua_Map3DEnv_s::Setnear);
class_property<lua_Map3DEnv_s>(L, "shadow_center", &lua_Map3DEnv_s::Getshadow_center, &lua_Map3DEnv_s::Setshadow_center);
class_property<lua_Map3DEnv_s>(L, "shadow_center_item", &lua_Map3DEnv_s::Getshadow_center_item, &lua_Map3DEnv_s::Setshadow_center_item);
class_property<lua_Map3DEnv_s>(L, "shadow_size", &lua_Map3DEnv_s::Getshadow_size, &lua_Map3DEnv_s::Setshadow_size);

class_add<lua_Map3DItem_s>(L, "Map3DItem");
class_property<lua_Map3DItem_s>(L, "context", &lua_Map3DItem_s::getcontext, &lua_Map3DItem_s::setcontext);
class_def<lua_Map3DItem_s>(L, "isnull", &lua_Map3DItem_s::isnull);
class_def<lua_Map3DItem_s>(L, "equal", &lua_Map3DItem_s::equal);
class_def<lua_Map3DItem_s>(L, "__eq", &lua_Map3DItem_s::operator == );
class_def<lua_Map3DItem_s>(L, "addhit", &lua_Map3DItem_s::addhit);
class_def<lua_Map3DItem_s>(L, "bind_action_begin", &lua_Map3DItem_s::bind_action_begin);
class_def<lua_Map3DItem_s>(L, "bind_action_end", &lua_Map3DItem_s::bind_action_end);
class_def<lua_Map3DItem_s>(L, "bind_click", &lua_Map3DItem_s::bind_click);
class_def<lua_Map3DItem_s>(L, "bind_draw", &lua_Map3DItem_s::bind_draw);
class_def<lua_Map3DItem_s>(L, "bind_event", &lua_Map3DItem_s::bind_event);
class_def<lua_Map3DItem_s>(L, "bind_eventby", &lua_Map3DItem_s::bind_eventby);
class_def<lua_Map3DItem_s>(L, "bind_mesh", &lua_Map3DItem_s::bind_mesh);
class_def<lua_Map3DItem_s>(L, "clone", &lua_Map3DItem_s::clone);
class_def<lua_Map3DItem_s>(L, "deleteme", &lua_Map3DItem_s::deleteme);
class_def<lua_Map3DItem_s>(L, "gameobj", &lua_Map3DItem_s::gameobj);
class_def<lua_Map3DItem_s>(L, "Getloop", &lua_Map3DItem_s::Getloop);
class_def<lua_Map3DItem_s>(L, "id", &lua_Map3DItem_s::id);
class_def<lua_Map3DItem_s>(L, "isgameobj", &lua_Map3DItem_s::isgameobj);
class_def<lua_Map3DItem_s>(L, "issceneobj", &lua_Map3DItem_s::issceneobj);
class_def<lua_Map3DItem_s>(L, "lookat", &lua_Map3DItem_s::lookat);
class_def<lua_Map3DItem_s>(L, "mesh", &lua_Map3DItem_s::mesh);
class_def<lua_Map3DItem_s>(L, "movedir", &lua_Map3DItem_s::movedir);
class_def<lua_Map3DItem_s>(L, "moveto", &lua_Map3DItem_s::moveto);
class_def<lua_Map3DItem_s>(L, "pushaction", &lua_Map3DItem_s::pushaction);
class_def<lua_Map3DItem_s>(L, "pushactionby", &lua_Map3DItem_s::pushactionby);
class_def<lua_Map3DItem_s>(L, "replace_image", &lua_Map3DItem_s::replace_image);
class_property<lua_Map3DItem_s>(L, "action", &lua_Map3DItem_s::Getaction, &lua_Map3DItem_s::Setaction);
class_property<lua_Map3DItem_s>(L, "actionby", &lua_Map3DItem_s::Getactionby, &lua_Map3DItem_s::Setactionby);
class_property<lua_Map3DItem_s>(L, "aspeed", &lua_Map3DItem_s::Getaspeed, &lua_Map3DItem_s::Setaspeed);
class_property<lua_Map3DItem_s>(L, "blood", &lua_Map3DItem_s::Getblood, &lua_Map3DItem_s::Setblood);
class_property<lua_Map3DItem_s>(L, "blood_display", &lua_Map3DItem_s::Getblood_display, &lua_Map3DItem_s::Setblood_display);
class_property<lua_Map3DItem_s>(L, "bloodcolor", &lua_Map3DItem_s::Getbloodcolor, &lua_Map3DItem_s::Setbloodcolor);
class_property<lua_Map3DItem_s>(L, "classid", &lua_Map3DItem_s::Getclassid, &lua_Map3DItem_s::Setclassid);
class_property<lua_Map3DItem_s>(L, "display", &lua_Map3DItem_s::Getdisplay, &lua_Map3DItem_s::Setdisplay);
class_property<lua_Map3DItem_s>(L, "enabled", &lua_Map3DItem_s::Getenabled, &lua_Map3DItem_s::Setenabled);
class_property<lua_Map3DItem_s>(L, "height", &lua_Map3DItem_s::Getheight, &lua_Map3DItem_s::Setheight);
class_def<lua_Map3DItem_s>(L, "Setloop", &lua_Map3DItem_s::Setloop);
class_property<lua_Map3DItem_s>(L, "name", &lua_Map3DItem_s::Getname, &lua_Map3DItem_s::Setname);
class_property<lua_Map3DItem_s>(L, "name_display", &lua_Map3DItem_s::Getname_display, &lua_Map3DItem_s::Setname_display);
class_property<lua_Map3DItem_s>(L, "opacity", &lua_Map3DItem_s::Getopacity, &lua_Map3DItem_s::Setopacity);
class_property<lua_Map3DItem_s>(L, "parent", &lua_Map3DItem_s::Getparent, &lua_Map3DItem_s::Setparent);
class_property<lua_Map3DItem_s>(L, "pause", &lua_Map3DItem_s::Getpause, &lua_Map3DItem_s::Setpause);
class_property<lua_Map3DItem_s>(L, "play", &lua_Map3DItem_s::Getplay, &lua_Map3DItem_s::Setplay);
class_property<lua_Map3DItem_s>(L, "pos", &lua_Map3DItem_s::Getpos, &lua_Map3DItem_s::Setpos);
class_property<lua_Map3DItem_s>(L, "pos3d", &lua_Map3DItem_s::Getpos3d, &lua_Map3DItem_s::Setpos3d);
class_property<lua_Map3DItem_s>(L, "rot", &lua_Map3DItem_s::Getrot, &lua_Map3DItem_s::Setrot);
class_property<lua_Map3DItem_s>(L, "scale", &lua_Map3DItem_s::Getscale, &lua_Map3DItem_s::Setscale);
class_property<lua_Map3DItem_s>(L, "speed", &lua_Map3DItem_s::Getspeed, &lua_Map3DItem_s::Setspeed);
class_property<lua_Map3DItem_s>(L, "worldmat", &lua_Map3DItem_s::Getworldmat, &lua_Map3DItem_s::Setworldmat);
class_def<lua_Map3DItem_s>(L, "stopmove", &lua_Map3DItem_s::stopmove);
class_def<lua_Map3DItem_s>(L, "unbind_mesh", &lua_Map3DItem_s::unbind_mesh);

class_add<lua_Map3DPoint_s>(L, "Map3DPoint");
class_property<lua_Map3DPoint_s>(L, "context", &lua_Map3DPoint_s::getcontext, &lua_Map3DPoint_s::setcontext);
class_def<lua_Map3DPoint_s>(L, "isnull", &lua_Map3DPoint_s::isnull);
class_def<lua_Map3DPoint_s>(L, "equal", &lua_Map3DPoint_s::equal);
class_def<lua_Map3DPoint_s>(L, "__eq", &lua_Map3DPoint_s::operator == );
class_def<lua_Map3DPoint_s>(L, "id", &lua_Map3DPoint_s::id);
class_property<lua_Map3DPoint_s>(L, "name", &lua_Map3DPoint_s::Getname, &lua_Map3DPoint_s::Setname);
class_property<lua_Map3DPoint_s>(L, "pos", &lua_Map3DPoint_s::Getpos, &lua_Map3DPoint_s::Setpos);
class_property<lua_Map3DPoint_s>(L, "range", &lua_Map3DPoint_s::Getrange, &lua_Map3DPoint_s::Setrange);

class_add<lua_Map3DSkill_s>(L, "Map3DSkill");
class_property<lua_Map3DSkill_s>(L, "context", &lua_Map3DSkill_s::getcontext, &lua_Map3DSkill_s::setcontext);
class_def<lua_Map3DSkill_s>(L, "isnull", &lua_Map3DSkill_s::isnull);
class_def<lua_Map3DSkill_s>(L, "equal", &lua_Map3DSkill_s::equal);
class_def<lua_Map3DSkill_s>(L, "__eq", &lua_Map3DSkill_s::operator == );
class_def<lua_Map3DSkill_s>(L, "bind_collision", &lua_Map3DSkill_s::bind_collision);
class_def<lua_Map3DSkill_s>(L, "getsource", &lua_Map3DSkill_s::getsource);
class_def<lua_Map3DSkill_s>(L, "gettarget", &lua_Map3DSkill_s::gettarget);
class_property<lua_Map3DSkill_s>(L, "continuetime", &lua_Map3DSkill_s::Getcontinuetime, &lua_Map3DSkill_s::Setcontinuetime);
class_property<lua_Map3DSkill_s>(L, "movespeed", &lua_Map3DSkill_s::Getmovespeed, &lua_Map3DSkill_s::Setmovespeed);
class_property<lua_Map3DSkill_s>(L, "size", &lua_Map3DSkill_s::Getsize, &lua_Map3DSkill_s::Setsize);
class_property<lua_Map3DSkill_s>(L, "source_zscale", &lua_Map3DSkill_s::Getsource_zscale, &lua_Map3DSkill_s::Setsource_zscale);
class_property<lua_Map3DSkill_s>(L, "sourcepos", &lua_Map3DSkill_s::Getsourcepos, &lua_Map3DSkill_s::Setsourcepos);
class_property<lua_Map3DSkill_s>(L, "target_zscale", &lua_Map3DSkill_s::Gettarget_zscale, &lua_Map3DSkill_s::Settarget_zscale);
class_property<lua_Map3DSkill_s>(L, "targetpos", &lua_Map3DSkill_s::Gettargetpos, &lua_Map3DSkill_s::Settargetpos);
class_property<lua_Map3DSkill_s>(L, "waittime", &lua_Map3DSkill_s::Getwaittime, &lua_Map3DSkill_s::Setwaittime);
class_def<lua_Map3DSkill_s>(L, "source", &lua_Map3DSkill_s::source);
class_def<lua_Map3DSkill_s>(L, "start", &lua_Map3DSkill_s::start);
class_def<lua_Map3DSkill_s>(L, "stop", &lua_Map3DSkill_s::stop);
class_def<lua_Map3DSkill_s>(L, "target", &lua_Map3DSkill_s::target);

class_add<lua_Mesh_s>(L, "Mesh");
class_property<lua_Mesh_s>(L, "context", &lua_Mesh_s::getcontext, &lua_Mesh_s::setcontext);
class_def<lua_Mesh_s>(L, "isnull", &lua_Mesh_s::isnull);
class_def<lua_Mesh_s>(L, "equal", &lua_Mesh_s::equal);
class_def<lua_Mesh_s>(L, "__eq", &lua_Mesh_s::operator == );
class_def<lua_Mesh_s>(L, "aabox", &lua_Mesh_s::aabox);
class_def<lua_Mesh_s>(L, "bind_draw", &lua_Mesh_s::bind_draw);
class_def<lua_Mesh_s>(L, "bind_mesh", &lua_Mesh_s::bind_mesh);
class_def<lua_Mesh_s>(L, "bonecount", &lua_Mesh_s::bonecount);
class_def<lua_Mesh_s>(L, "bonemat_at", &lua_Mesh_s::bonemat_at);
class_def<lua_Mesh_s>(L, "bonename_at", &lua_Mesh_s::bonename_at);
class_def<lua_Mesh_s>(L, "clearParent", &lua_Mesh_s::clearParent);
class_def<lua_Mesh_s>(L, "clone", &lua_Mesh_s::clone);
class_def<lua_Mesh_s>(L, "getfilepath", &lua_Mesh_s::getfilepath);
class_def<lua_Mesh_s>(L, "height", &lua_Mesh_s::height);
class_def<lua_Mesh_s>(L, "isskeleton", &lua_Mesh_s::isskeleton);
class_def<lua_Mesh_s>(L, "isunwalkable", &lua_Mesh_s::isunwalkable);
class_def<lua_Mesh_s>(L, "lookat", &lua_Mesh_s::lookat);
class_def<lua_Mesh_s>(L, "playaction", &lua_Mesh_s::playaction);
class_def<lua_Mesh_s>(L, "ray", &lua_Mesh_s::ray);
class_def<lua_Mesh_s>(L, "ray_dir", &lua_Mesh_s::ray_dir);
class_def<lua_Mesh_s>(L, "replace_image", &lua_Mesh_s::replace_image);
class_def<lua_Mesh_s>(L, "s_action_at", &lua_Mesh_s::s_action_at);
class_def<lua_Mesh_s>(L, "s_actioncount", &lua_Mesh_s::s_actioncount);
class_def<lua_Mesh_s>(L, "searchboneid", &lua_Mesh_s::searchboneid);
class_def<lua_Mesh_s>(L, "searchboneid_like", &lua_Mesh_s::searchboneid_like);
class_property<lua_Mesh_s>(L, "action", &lua_Mesh_s::Getaction, &lua_Mesh_s::Setaction);
class_property<lua_Mesh_s>(L, "actionby", &lua_Mesh_s::Getactionby, &lua_Mesh_s::Setactionby);
class_property<lua_Mesh_s>(L, "color", &lua_Mesh_s::Getcolor, &lua_Mesh_s::Setcolor);
class_property<lua_Mesh_s>(L, "defaultaction", &lua_Mesh_s::Getdefaultaction, &lua_Mesh_s::Setdefaultaction);
class_property<lua_Mesh_s>(L, "destblend", &lua_Mesh_s::Getdestblend, &lua_Mesh_s::Setdestblend);
class_property<lua_Mesh_s>(L, "display", &lua_Mesh_s::Getdisplay, &lua_Mesh_s::Setdisplay);
class_property<lua_Mesh_s>(L, "enabled", &lua_Mesh_s::Getenabled, &lua_Mesh_s::Setenabled);
class_property<lua_Mesh_s>(L, "follow_camera", &lua_Mesh_s::Getfollow_camera, &lua_Mesh_s::Setfollow_camera);
class_property<lua_Mesh_s>(L, "opacity", &lua_Mesh_s::Getopacity, &lua_Mesh_s::Setopacity);
class_property<lua_Mesh_s>(L, "parent", &lua_Mesh_s::Getparent, &lua_Mesh_s::Setparent);
class_property<lua_Mesh_s>(L, "pause", &lua_Mesh_s::Getpause, &lua_Mesh_s::Setpause);
class_property<lua_Mesh_s>(L, "play", &lua_Mesh_s::Getplay, &lua_Mesh_s::Setplay);
class_property<lua_Mesh_s>(L, "pos", &lua_Mesh_s::Getpos, &lua_Mesh_s::Setpos);
class_property<lua_Mesh_s>(L, "rot", &lua_Mesh_s::Getrot, &lua_Mesh_s::Setrot);
class_property<lua_Mesh_s>(L, "s_lodhide_onexceed", &lua_Mesh_s::Gets_lodhide_onexceed, &lua_Mesh_s::Sets_lodhide_onexceed);
class_property<lua_Mesh_s>(L, "s_lodinterval_length", &lua_Mesh_s::Gets_lodinterval_length, &lua_Mesh_s::Sets_lodinterval_length);
class_property<lua_Mesh_s>(L, "s_lodmaxvisible", &lua_Mesh_s::Gets_lodmaxvisible, &lua_Mesh_s::Sets_lodmaxvisible);
class_property<lua_Mesh_s>(L, "scale", &lua_Mesh_s::Getscale, &lua_Mesh_s::Setscale);
class_property<lua_Mesh_s>(L, "selfluminous", &lua_Mesh_s::Getselfluminous, &lua_Mesh_s::Setselfluminous);
class_property<lua_Mesh_s>(L, "specular", &lua_Mesh_s::Getspecular, &lua_Mesh_s::Setspecular);
class_property<lua_Mesh_s>(L, "speed", &lua_Mesh_s::Getspeed, &lua_Mesh_s::Setspeed);
class_property<lua_Mesh_s>(L, "srcblend", &lua_Mesh_s::Getsrcblend, &lua_Mesh_s::Setsrcblend);
class_property<lua_Mesh_s>(L, "worldmat", &lua_Mesh_s::Getworldmat, &lua_Mesh_s::Setworldmat);
class_def<lua_Mesh_s>(L, "submesh_at", &lua_Mesh_s::submesh_at);
class_def<lua_Mesh_s>(L, "submeshcount", &lua_Mesh_s::submeshcount);
class_def<lua_Mesh_s>(L, "unbind_mesh", &lua_Mesh_s::unbind_mesh);

class_add<lua_MeshAction_s>(L, "MeshAction");
class_property<lua_MeshAction_s>(L, "context", &lua_MeshAction_s::getcontext, &lua_MeshAction_s::setcontext);
class_def<lua_MeshAction_s>(L, "isnull", &lua_MeshAction_s::isnull);
class_def<lua_MeshAction_s>(L, "equal", &lua_MeshAction_s::equal);
class_def<lua_MeshAction_s>(L, "__eq", &lua_MeshAction_s::operator == );
class_def<lua_MeshAction_s>(L, "keycount", &lua_MeshAction_s::keycount);
class_property<lua_MeshAction_s>(L, "keybegin", &lua_MeshAction_s::Getkeybegin, &lua_MeshAction_s::Setkeybegin);
class_property<lua_MeshAction_s>(L, "keyend", &lua_MeshAction_s::Getkeyend, &lua_MeshAction_s::Setkeyend);
class_property<lua_MeshAction_s>(L, "lerptime", &lua_MeshAction_s::Getlerptime, &lua_MeshAction_s::Setlerptime);
class_property<lua_MeshAction_s>(L, "speed", &lua_MeshAction_s::Getspeed, &lua_MeshAction_s::Setspeed);

class_add<lua_MeshMaterial_s>(L, "MeshMaterial");
class_property<lua_MeshMaterial_s>(L, "context", &lua_MeshMaterial_s::getcontext, &lua_MeshMaterial_s::setcontext);
class_def<lua_MeshMaterial_s>(L, "isnull", &lua_MeshMaterial_s::isnull);
class_def<lua_MeshMaterial_s>(L, "equal", &lua_MeshMaterial_s::equal);
class_def<lua_MeshMaterial_s>(L, "__eq", &lua_MeshMaterial_s::operator == );
class_property<lua_MeshMaterial_s>(L, "color", &lua_MeshMaterial_s::Getcolor, &lua_MeshMaterial_s::Setcolor);
class_property<lua_MeshMaterial_s>(L, "cullmode", &lua_MeshMaterial_s::Getcullmode, &lua_MeshMaterial_s::Setcullmode);
class_property<lua_MeshMaterial_s>(L, "destblend", &lua_MeshMaterial_s::Getdestblend, &lua_MeshMaterial_s::Setdestblend);
class_property<lua_MeshMaterial_s>(L, "normal_texture", &lua_MeshMaterial_s::Getnormal_texture, &lua_MeshMaterial_s::Setnormal_texture);
class_property<lua_MeshMaterial_s>(L, "opacitymode", &lua_MeshMaterial_s::Getopacitymode, &lua_MeshMaterial_s::Setopacitymode);
class_property<lua_MeshMaterial_s>(L, "reflect_texture", &lua_MeshMaterial_s::Getreflect_texture, &lua_MeshMaterial_s::Setreflect_texture);
class_property<lua_MeshMaterial_s>(L, "selfluminous", &lua_MeshMaterial_s::Getselfluminous, &lua_MeshMaterial_s::Setselfluminous);
class_property<lua_MeshMaterial_s>(L, "specular", &lua_MeshMaterial_s::Getspecular, &lua_MeshMaterial_s::Setspecular);
class_property<lua_MeshMaterial_s>(L, "specular_texture", &lua_MeshMaterial_s::Getspecular_texture, &lua_MeshMaterial_s::Setspecular_texture);
class_property<lua_MeshMaterial_s>(L, "srcblend", &lua_MeshMaterial_s::Getsrcblend, &lua_MeshMaterial_s::Setsrcblend);
class_property<lua_MeshMaterial_s>(L, "texture", &lua_MeshMaterial_s::Gettexture, &lua_MeshMaterial_s::Settexture);

class_add<lua_SubMesh_s>(L, "SubMesh");
class_property<lua_SubMesh_s>(L, "context", &lua_SubMesh_s::getcontext, &lua_SubMesh_s::setcontext);
class_def<lua_SubMesh_s>(L, "isnull", &lua_SubMesh_s::isnull);
class_def<lua_SubMesh_s>(L, "equal", &lua_SubMesh_s::equal);
class_def<lua_SubMesh_s>(L, "__eq", &lua_SubMesh_s::operator == );
class_def<lua_SubMesh_s>(L, "getname", &lua_SubMesh_s::getname);
class_def<lua_SubMesh_s>(L, "isskeleton", &lua_SubMesh_s::isskeleton);
class_def<lua_SubMesh_s>(L, "lod_exists", &lua_SubMesh_s::lod_exists);
class_def<lua_SubMesh_s>(L, "material_at", &lua_SubMesh_s::material_at);
class_def<lua_SubMesh_s>(L, "materialcount", &lua_SubMesh_s::materialcount);
class_property<lua_SubMesh_s>(L, "display", &lua_SubMesh_s::Getdisplay, &lua_SubMesh_s::Setdisplay);
class_property<lua_SubMesh_s>(L, "worldmat", &lua_SubMesh_s::Getworldmat, &lua_SubMesh_s::Setworldmat);
class_def<lua_SubMesh_s>(L, "triangle_at", &lua_SubMesh_s::triangle_at);
class_def<lua_SubMesh_s>(L, "trianglecount", &lua_SubMesh_s::trianglecount);
class_def<lua_SubMesh_s>(L, "vertex_at", &lua_SubMesh_s::vertex_at);
class_def<lua_SubMesh_s>(L, "vertexcount", &lua_SubMesh_s::vertexcount);

class_add<lua_Table_s>(L, "Table");
class_property<lua_Table_s>(L, "context", &lua_Table_s::getcontext, &lua_Table_s::setcontext);
class_def<lua_Table_s>(L, "isnull", &lua_Table_s::isnull);
class_def<lua_Table_s>(L, "equal", &lua_Table_s::equal);
class_def<lua_Table_s>(L, "__eq", &lua_Table_s::operator == );
class_def<lua_Table_s>(L, "id_f", &lua_Table_s::id_f);
class_def<lua_Table_s>(L, "id_i", &lua_Table_s::id_i);
class_def<lua_Table_s>(L, "id_t", &lua_Table_s::id_t);
class_def<lua_Table_s>(L, "linecount", &lua_Table_s::linecount);
class_def<lua_Table_s>(L, "name_f", &lua_Table_s::name_f);
class_def<lua_Table_s>(L, "name_i", &lua_Table_s::name_i);
class_def<lua_Table_s>(L, "name_t", &lua_Table_s::name_t);
class_def<lua_Table_s>(L, "read_f", &lua_Table_s::read_f);
class_def<lua_Table_s>(L, "read_i", &lua_Table_s::read_i);
class_def<lua_Table_s>(L, "read_t", &lua_Table_s::read_t);
class_def<lua_Table_s>(L, "search_col", &lua_Table_s::search_col);
class_def<lua_Table_s>(L, "search_f", &lua_Table_s::search_f);
class_def<lua_Table_s>(L, "search_i", &lua_Table_s::search_i);
class_def<lua_Table_s>(L, "search_t", &lua_Table_s::search_t);

class_add<lua_Texture_s>(L, "Texture");
class_property<lua_Texture_s>(L, "context", &lua_Texture_s::getcontext, &lua_Texture_s::setcontext);
class_def<lua_Texture_s>(L, "isnull", &lua_Texture_s::isnull);
class_def<lua_Texture_s>(L, "equal", &lua_Texture_s::equal);
class_def<lua_Texture_s>(L, "__eq", &lua_Texture_s::operator == );
class_def<lua_Texture_s>(L, "commit", &lua_Texture_s::commit);
class_def<lua_Texture_s>(L, "getfilepath", &lua_Texture_s::getfilepath);
class_def<lua_Texture_s>(L, "getheight", &lua_Texture_s::getheight);
class_def<lua_Texture_s>(L, "getwidth", &lua_Texture_s::getwidth);

class_add<lua_UIControl_s>(L, "UIControl");
class_property<lua_UIControl_s>(L, "context", &lua_UIControl_s::getcontext, &lua_UIControl_s::setcontext);
class_def<lua_UIControl_s>(L, "isnull", &lua_UIControl_s::isnull);
class_def<lua_UIControl_s>(L, "equal", &lua_UIControl_s::equal);
class_def<lua_UIControl_s>(L, "__eq", &lua_UIControl_s::operator == );
class_def<lua_UIControl_s>(L, "bind_3d_clear", &lua_UIControl_s::bind_3d_clear);
class_def<lua_UIControl_s>(L, "bind_3d_maxdist", &lua_UIControl_s::bind_3d_maxdist);
class_def<lua_UIControl_s>(L, "bind_3d_obj", &lua_UIControl_s::bind_3d_obj);
class_def<lua_UIControl_s>(L, "bind_3d_pos", &lua_UIControl_s::bind_3d_pos);
class_def<lua_UIControl_s>(L, "bind_click", &lua_UIControl_s::bind_click);
class_def<lua_UIControl_s>(L, "bind_draw", &lua_UIControl_s::bind_draw);
class_def<lua_UIControl_s>(L, "bind_event", &lua_UIControl_s::bind_event);
class_def<lua_UIControl_s>(L, "bind_eventby", &lua_UIControl_s::bind_eventby);
class_def<lua_UIControl_s>(L, "bind_image", &lua_UIControl_s::bind_image);
class_def<lua_UIControl_s>(L, "clone", &lua_UIControl_s::clone);
class_def<lua_UIControl_s>(L, "cloneto", &lua_UIControl_s::cloneto);
class_def<lua_UIControl_s>(L, "dialog", &lua_UIControl_s::dialog);
class_def<lua_UIControl_s>(L, "element_at", &lua_UIControl_s::element_at);
class_def<lua_UIControl_s>(L, "elementcount", &lua_UIControl_s::elementcount);
class_def<lua_UIControl_s>(L, "framemove", &lua_UIControl_s::framemove);
class_def<lua_UIControl_s>(L, "framemove2", &lua_UIControl_s::framemove2);
class_def<lua_UIControl_s>(L, "geteventsound", &lua_UIControl_s::geteventsound);
class_def<lua_UIControl_s>(L, "getId", &lua_UIControl_s::getId);
class_def<lua_UIControl_s>(L, "gettype", &lua_UIControl_s::gettype);
class_def<lua_UIControl_s>(L, "innermesh_clear", &lua_UIControl_s::innermesh_clear);
class_def<lua_UIControl_s>(L, "interval", &lua_UIControl_s::interval);
class_def<lua_UIControl_s>(L, "interval2", &lua_UIControl_s::interval2);
class_def<lua_UIControl_s>(L, "isfocus", &lua_UIControl_s::isfocus);
class_def<lua_UIControl_s>(L, "mousepoint", &lua_UIControl_s::mousepoint);
class_def<lua_UIControl_s>(L, "pick", &lua_UIControl_s::pick);
class_def<lua_UIControl_s>(L, "progress_time", &lua_UIControl_s::progress_time);
class_def<lua_UIControl_s>(L, "removeChild", &lua_UIControl_s::removeChild);
class_def<lua_UIControl_s>(L, "reset_trans", &lua_UIControl_s::reset_trans);
class_def<lua_UIControl_s>(L, "rot2d_dir", &lua_UIControl_s::rot2d_dir);
class_def<lua_UIControl_s>(L, "saved_pos", &lua_UIControl_s::saved_pos);
class_def<lua_UIControl_s>(L, "saved_rect", &lua_UIControl_s::saved_rect);
class_def<lua_UIControl_s>(L, "saved_rot", &lua_UIControl_s::saved_rot);
class_property<lua_UIControl_s>(L, "action", &lua_UIControl_s::Getaction, &lua_UIControl_s::Setaction);
class_property<lua_UIControl_s>(L, "actionby", &lua_UIControl_s::Getactionby, &lua_UIControl_s::Setactionby);
class_property<lua_UIControl_s>(L, "actiontime", &lua_UIControl_s::Getactiontime, &lua_UIControl_s::Setactiontime);
class_property<lua_UIControl_s>(L, "active", &lua_UIControl_s::Getactive, &lua_UIControl_s::Setactive);
class_property<lua_UIControl_s>(L, "bottom", &lua_UIControl_s::Getbottom, &lua_UIControl_s::Setbottom);
class_property<lua_UIControl_s>(L, "center", &lua_UIControl_s::Getcenter, &lua_UIControl_s::Setcenter);
class_def<lua_UIControl_s>(L, "setChild", &lua_UIControl_s::setChild);
class_property<lua_UIControl_s>(L, "color", &lua_UIControl_s::Getcolor, &lua_UIControl_s::Setcolor);
class_property<lua_UIControl_s>(L, "display", &lua_UIControl_s::Getdisplay, &lua_UIControl_s::Setdisplay);
class_property<lua_UIControl_s>(L, "effect_loop", &lua_UIControl_s::Geteffect_loop, &lua_UIControl_s::Seteffect_loop);
class_property<lua_UIControl_s>(L, "effect_stop", &lua_UIControl_s::Geteffect_stop, &lua_UIControl_s::Seteffect_stop);
class_property<lua_UIControl_s>(L, "enabled", &lua_UIControl_s::Getenabled, &lua_UIControl_s::Setenabled);
class_def<lua_UIControl_s>(L, "seteventsound", &lua_UIControl_s::seteventsound);
class_property<lua_UIControl_s>(L, "group", &lua_UIControl_s::Getgroup, &lua_UIControl_s::Setgroup);
class_property<lua_UIControl_s>(L, "groupid", &lua_UIControl_s::Getgroupid, &lua_UIControl_s::Setgroupid);
class_property<lua_UIControl_s>(L, "groupname", &lua_UIControl_s::Getgroupname, &lua_UIControl_s::Setgroupname);
class_property<lua_UIControl_s>(L, "height", &lua_UIControl_s::Getheight, &lua_UIControl_s::Setheight);
class_property<lua_UIControl_s>(L, "image", &lua_UIControl_s::Getimage, &lua_UIControl_s::Setimage);
class_property<lua_UIControl_s>(L, "imagecolor", &lua_UIControl_s::Getimagecolor, &lua_UIControl_s::Setimagecolor);
class_property<lua_UIControl_s>(L, "innermesh", &lua_UIControl_s::Getinnermesh, &lua_UIControl_s::Setinnermesh);
class_property<lua_UIControl_s>(L, "layer", &lua_UIControl_s::Getlayer, &lua_UIControl_s::Setlayer);
class_property<lua_UIControl_s>(L, "left", &lua_UIControl_s::Getleft, &lua_UIControl_s::Setleft);
class_property<lua_UIControl_s>(L, "movebottom", &lua_UIControl_s::Getmovebottom, &lua_UIControl_s::Setmovebottom);
class_property<lua_UIControl_s>(L, "movedialog", &lua_UIControl_s::Getmovedialog, &lua_UIControl_s::Setmovedialog);
class_property<lua_UIControl_s>(L, "moveleft", &lua_UIControl_s::Getmoveleft, &lua_UIControl_s::Setmoveleft);
class_property<lua_UIControl_s>(L, "moveright", &lua_UIControl_s::Getmoveright, &lua_UIControl_s::Setmoveright);
class_property<lua_UIControl_s>(L, "movetop", &lua_UIControl_s::Getmovetop, &lua_UIControl_s::Setmovetop);
class_property<lua_UIControl_s>(L, "name", &lua_UIControl_s::Getname, &lua_UIControl_s::Setname);
class_property<lua_UIControl_s>(L, "nextaction", &lua_UIControl_s::Getnextaction, &lua_UIControl_s::Setnextaction);
class_property<lua_UIControl_s>(L, "opacity", &lua_UIControl_s::Getopacity, &lua_UIControl_s::Setopacity);
class_property<lua_UIControl_s>(L, "pixelpick", &lua_UIControl_s::Getpixelpick, &lua_UIControl_s::Setpixelpick);
class_property<lua_UIControl_s>(L, "pos", &lua_UIControl_s::Getpos, &lua_UIControl_s::Setpos);
class_property<lua_UIControl_s>(L, "rect", &lua_UIControl_s::Getrect, &lua_UIControl_s::Setrect);
class_property<lua_UIControl_s>(L, "right", &lua_UIControl_s::Getright, &lua_UIControl_s::Setright);
class_property<lua_UIControl_s>(L, "rot", &lua_UIControl_s::Getrot, &lua_UIControl_s::Setrot);
class_property<lua_UIControl_s>(L, "rot2d", &lua_UIControl_s::Getrot2d, &lua_UIControl_s::Setrot2d);
class_property<lua_UIControl_s>(L, "rotcenter", &lua_UIControl_s::Getrotcenter, &lua_UIControl_s::Setrotcenter);
class_property<lua_UIControl_s>(L, "rotenable", &lua_UIControl_s::Getrotenable, &lua_UIControl_s::Setrotenable);
class_property<lua_UIControl_s>(L, "scale", &lua_UIControl_s::Getscale, &lua_UIControl_s::Setscale);
class_property<lua_UIControl_s>(L, "scalex", &lua_UIControl_s::Getscalex, &lua_UIControl_s::Setscalex);
class_property<lua_UIControl_s>(L, "scaley", &lua_UIControl_s::Getscaley, &lua_UIControl_s::Setscaley);
class_property<lua_UIControl_s>(L, "scroll_extend", &lua_UIControl_s::Getscroll_extend, &lua_UIControl_s::Setscroll_extend);
class_property<lua_UIControl_s>(L, "scroll_firstspeed", &lua_UIControl_s::Getscroll_firstspeed, &lua_UIControl_s::Setscroll_firstspeed);
class_property<lua_UIControl_s>(L, "scroll_h", &lua_UIControl_s::Getscroll_h, &lua_UIControl_s::Setscroll_h);
class_property<lua_UIControl_s>(L, "scroll_max", &lua_UIControl_s::Getscroll_max, &lua_UIControl_s::Setscroll_max);
class_property<lua_UIControl_s>(L, "scroll_min", &lua_UIControl_s::Getscroll_min, &lua_UIControl_s::Setscroll_min);
class_property<lua_UIControl_s>(L, "scroll_speed", &lua_UIControl_s::Getscroll_speed, &lua_UIControl_s::Setscroll_speed);
class_property<lua_UIControl_s>(L, "scroll_v", &lua_UIControl_s::Getscroll_v, &lua_UIControl_s::Setscroll_v);
class_property<lua_UIControl_s>(L, "speed", &lua_UIControl_s::Getspeed, &lua_UIControl_s::Setspeed);
class_property<lua_UIControl_s>(L, "text", &lua_UIControl_s::Gettext, &lua_UIControl_s::Settext);
class_property<lua_UIControl_s>(L, "textalign", &lua_UIControl_s::Gettextalign, &lua_UIControl_s::Settextalign);
class_property<lua_UIControl_s>(L, "textcolor", &lua_UIControl_s::Gettextcolor, &lua_UIControl_s::Settextcolor);
class_property<lua_UIControl_s>(L, "top", &lua_UIControl_s::Gettop, &lua_UIControl_s::Settop);
class_property<lua_UIControl_s>(L, "valf", &lua_UIControl_s::Getvalf, &lua_UIControl_s::Setvalf);
class_property<lua_UIControl_s>(L, "vali", &lua_UIControl_s::Getvali, &lua_UIControl_s::Setvali);
class_property<lua_UIControl_s>(L, "valt", &lua_UIControl_s::Getvalt, &lua_UIControl_s::Setvalt);
class_property<lua_UIControl_s>(L, "waitbefore_actionplay", &lua_UIControl_s::Getwaitbefore_actionplay, &lua_UIControl_s::Setwaitbefore_actionplay);
class_property<lua_UIControl_s>(L, "width", &lua_UIControl_s::Getwidth, &lua_UIControl_s::Setwidth);
class_def<lua_UIControl_s>(L, "sort_draws", &lua_UIControl_s::sort_draws);
class_def<lua_UIControl_s>(L, "timeout", &lua_UIControl_s::timeout);
class_def<lua_UIControl_s>(L, "timeout2", &lua_UIControl_s::timeout2);

class_add<lua_UIDialog_s>(L, "UIDialog");
class_property<lua_UIDialog_s>(L, "context", &lua_UIDialog_s::getcontext, &lua_UIDialog_s::setcontext);
class_def<lua_UIDialog_s>(L, "isnull", &lua_UIDialog_s::isnull);
class_def<lua_UIDialog_s>(L, "equal", &lua_UIDialog_s::equal);
class_def<lua_UIDialog_s>(L, "__eq", &lua_UIDialog_s::operator == );
class_def<lua_UIDialog_s>(L, "at", &lua_UIDialog_s::at);
class_def<lua_UIDialog_s>(L, "bind_click", &lua_UIDialog_s::bind_click);
class_def<lua_UIDialog_s>(L, "bind_draw", &lua_UIDialog_s::bind_draw);
class_def<lua_UIDialog_s>(L, "bind_event", &lua_UIDialog_s::bind_event);
class_def<lua_UIDialog_s>(L, "bind_eventby", &lua_UIDialog_s::bind_eventby);
class_def<lua_UIDialog_s>(L, "bind_image", &lua_UIDialog_s::bind_image);
class_def<lua_UIDialog_s>(L, "count", &lua_UIDialog_s::count);
class_def<lua_UIDialog_s>(L, "create", &lua_UIDialog_s::create);
class_def<lua_UIDialog_s>(L, "delete_at", &lua_UIDialog_s::delete_at);
class_def<lua_UIDialog_s>(L, "deleteall", &lua_UIDialog_s::deleteall);
class_def<lua_UIDialog_s>(L, "deleteby", &lua_UIDialog_s::deleteby);
class_def<lua_UIDialog_s>(L, "destroy", &lua_UIDialog_s::destroy);
class_def<lua_UIDialog_s>(L, "enable", &lua_UIDialog_s::enable);
class_def<lua_UIDialog_s>(L, "framemove", &lua_UIDialog_s::framemove);
class_def<lua_UIDialog_s>(L, "framemove2", &lua_UIDialog_s::framemove2);
class_def<lua_UIDialog_s>(L, "gat", &lua_UIDialog_s::gat);
class_def<lua_UIDialog_s>(L, "gcount", &lua_UIDialog_s::gcount);
class_def<lua_UIDialog_s>(L, "gcreate", &lua_UIDialog_s::gcreate);
class_def<lua_UIDialog_s>(L, "gdelete", &lua_UIDialog_s::gdelete);
class_def<lua_UIDialog_s>(L, "getvalf", &lua_UIDialog_s::getvalf);
class_def<lua_UIDialog_s>(L, "getvali", &lua_UIDialog_s::getvali);
class_def<lua_UIDialog_s>(L, "getvalt", &lua_UIDialog_s::getvalt);
class_def<lua_UIDialog_s>(L, "gsearch", &lua_UIDialog_s::gsearch);
class_def<lua_UIDialog_s>(L, "gsearchbyid", &lua_UIDialog_s::gsearchbyid);
class_def<lua_UIDialog_s>(L, "hold", &lua_UIDialog_s::hold);
class_def<lua_UIDialog_s>(L, "image_mode", &lua_UIDialog_s::image_mode);
class_def<lua_UIDialog_s>(L, "interval", &lua_UIDialog_s::interval);
class_def<lua_UIDialog_s>(L, "interval2", &lua_UIDialog_s::interval2);
class_def<lua_UIDialog_s>(L, "istooltip_window", &lua_UIDialog_s::istooltip_window);
class_def<lua_UIDialog_s>(L, "map_add2dchild", &lua_UIDialog_s::map_add2dchild);
class_def<lua_UIDialog_s>(L, "map_addfindway", &lua_UIDialog_s::map_addfindway);
class_def<lua_UIDialog_s>(L, "map_addmoveto", &lua_UIDialog_s::map_addmoveto);
class_def<lua_UIDialog_s>(L, "map_areaindex", &lua_UIDialog_s::map_areaindex);
class_def<lua_UIDialog_s>(L, "map_beginmoveto", &lua_UIDialog_s::map_beginmoveto);
class_def<lua_UIDialog_s>(L, "map_center", &lua_UIDialog_s::map_center);
class_def<lua_UIDialog_s>(L, "map_clear2dchild", &lua_UIDialog_s::map_clear2dchild);
class_def<lua_UIDialog_s>(L, "map_clear_moveable", &lua_UIDialog_s::map_clear_moveable);
class_def<lua_UIDialog_s>(L, "map_count", &lua_UIDialog_s::map_count);
class_def<lua_UIDialog_s>(L, "map_gridsize", &lua_UIDialog_s::map_gridsize);
class_def<lua_UIDialog_s>(L, "map_mapindex", &lua_UIDialog_s::map_mapindex);
class_def<lua_UIDialog_s>(L, "map_moveable", &lua_UIDialog_s::map_moveable);
class_def<lua_UIDialog_s>(L, "map_pickarea", &lua_UIDialog_s::map_pickarea);
class_def<lua_UIDialog_s>(L, "map_pickmap", &lua_UIDialog_s::map_pickmap);
class_def<lua_UIDialog_s>(L, "map_searchway", &lua_UIDialog_s::map_searchway);
class_def<lua_UIDialog_s>(L, "map_searchway2", &lua_UIDialog_s::map_searchway2);
class_def<lua_UIDialog_s>(L, "map_set_moveable", &lua_UIDialog_s::map_set_moveable);
class_def<lua_UIDialog_s>(L, "map_stopmove", &lua_UIDialog_s::map_stopmove);
class_def<lua_UIDialog_s>(L, "move", &lua_UIDialog_s::move);
class_def<lua_UIDialog_s>(L, "pick", &lua_UIDialog_s::pick);
class_def<lua_UIDialog_s>(L, "playani", &lua_UIDialog_s::playani);
class_def<lua_UIDialog_s>(L, "recalc_rect", &lua_UIDialog_s::recalc_rect);
class_def<lua_UIDialog_s>(L, "rotdir", &lua_UIDialog_s::rotdir);
class_def<lua_UIDialog_s>(L, "search", &lua_UIDialog_s::search);
class_def<lua_UIDialog_s>(L, "searchbyid", &lua_UIDialog_s::searchbyid);
class_property<lua_UIDialog_s>(L, "action", &lua_UIDialog_s::Getaction, &lua_UIDialog_s::Setaction);
class_property<lua_UIDialog_s>(L, "actionby", &lua_UIDialog_s::Getactionby, &lua_UIDialog_s::Setactionby);
class_property<lua_UIDialog_s>(L, "center", &lua_UIDialog_s::Getcenter, &lua_UIDialog_s::Setcenter);
class_property<lua_UIDialog_s>(L, "color", &lua_UIDialog_s::Getcolor, &lua_UIDialog_s::Setcolor);
class_property<lua_UIDialog_s>(L, "display", &lua_UIDialog_s::Getdisplay, &lua_UIDialog_s::Setdisplay);
class_property<lua_UIDialog_s>(L, "domodal", &lua_UIDialog_s::Getdomodal, &lua_UIDialog_s::Setdomodal);
class_property<lua_UIDialog_s>(L, "enabled", &lua_UIDialog_s::Getenabled, &lua_UIDialog_s::Setenabled);
class_property<lua_UIDialog_s>(L, "height", &lua_UIDialog_s::Getheight, &lua_UIDialog_s::Setheight);
class_property<lua_UIDialog_s>(L, "hideon_killfocus", &lua_UIDialog_s::Gethideon_killfocus, &lua_UIDialog_s::Sethideon_killfocus);
class_property<lua_UIDialog_s>(L, "layer", &lua_UIDialog_s::Getlayer, &lua_UIDialog_s::Setlayer);
class_property<lua_UIDialog_s>(L, "left", &lua_UIDialog_s::Getleft, &lua_UIDialog_s::Setleft);
class_property<lua_UIDialog_s>(L, "name", &lua_UIDialog_s::Getname, &lua_UIDialog_s::Setname);
class_property<lua_UIDialog_s>(L, "opacity", &lua_UIDialog_s::Getopacity, &lua_UIDialog_s::Setopacity);
class_property<lua_UIDialog_s>(L, "pause", &lua_UIDialog_s::Getpause, &lua_UIDialog_s::Setpause);
class_property<lua_UIDialog_s>(L, "pos", &lua_UIDialog_s::Getpos, &lua_UIDialog_s::Setpos);
class_property<lua_UIDialog_s>(L, "rect", &lua_UIDialog_s::Getrect, &lua_UIDialog_s::Setrect);
class_property<lua_UIDialog_s>(L, "rot2d", &lua_UIDialog_s::Getrot2d, &lua_UIDialog_s::Setrot2d);
class_property<lua_UIDialog_s>(L, "rotx", &lua_UIDialog_s::Getrotx, &lua_UIDialog_s::Setrotx);
class_property<lua_UIDialog_s>(L, "roty", &lua_UIDialog_s::Getroty, &lua_UIDialog_s::Setroty);
class_property<lua_UIDialog_s>(L, "rotz", &lua_UIDialog_s::Getrotz, &lua_UIDialog_s::Setrotz);
class_property<lua_UIDialog_s>(L, "scale", &lua_UIDialog_s::Getscale, &lua_UIDialog_s::Setscale);
class_property<lua_UIDialog_s>(L, "scalex", &lua_UIDialog_s::Getscalex, &lua_UIDialog_s::Setscalex);
class_property<lua_UIDialog_s>(L, "scaley", &lua_UIDialog_s::Getscaley, &lua_UIDialog_s::Setscaley);
class_def<lua_UIDialog_s>(L, "settooltip_window", &lua_UIDialog_s::settooltip_window);
class_property<lua_UIDialog_s>(L, "top", &lua_UIDialog_s::Gettop, &lua_UIDialog_s::Settop);
class_property<lua_UIDialog_s>(L, "topmost", &lua_UIDialog_s::Gettopmost, &lua_UIDialog_s::Settopmost);
class_def<lua_UIDialog_s>(L, "setvalf", &lua_UIDialog_s::setvalf);
class_def<lua_UIDialog_s>(L, "setvali", &lua_UIDialog_s::setvali);
class_def<lua_UIDialog_s>(L, "setvalt", &lua_UIDialog_s::setvalt);
class_property<lua_UIDialog_s>(L, "width", &lua_UIDialog_s::Getwidth, &lua_UIDialog_s::Setwidth);
class_def<lua_UIDialog_s>(L, "show", &lua_UIDialog_s::show);
class_def<lua_UIDialog_s>(L, "timeout", &lua_UIDialog_s::timeout);
class_def<lua_UIDialog_s>(L, "timeout2", &lua_UIDialog_s::timeout2);

class_add<lua_UIDrawMgr_s>(L, "UIDrawMgr");
class_property<lua_UIDrawMgr_s>(L, "context", &lua_UIDrawMgr_s::getcontext, &lua_UIDrawMgr_s::setcontext);
class_def<lua_UIDrawMgr_s>(L, "isnull", &lua_UIDrawMgr_s::isnull);
class_def<lua_UIDrawMgr_s>(L, "equal", &lua_UIDrawMgr_s::equal);
class_def<lua_UIDrawMgr_s>(L, "__eq", &lua_UIDrawMgr_s::operator == );
class_def<lua_UIDrawMgr_s>(L, "add_child", &lua_UIDrawMgr_s::add_child);
class_def<lua_UIDrawMgr_s>(L, "bind_click", &lua_UIDrawMgr_s::bind_click);
class_def<lua_UIDrawMgr_s>(L, "bind_dblclick", &lua_UIDrawMgr_s::bind_dblclick);
class_def<lua_UIDrawMgr_s>(L, "child_count", &lua_UIDrawMgr_s::child_count);
class_def<lua_UIDrawMgr_s>(L, "delete_child", &lua_UIDrawMgr_s::delete_child);
class_def<lua_UIDrawMgr_s>(L, "draw_rect", &lua_UIDrawMgr_s::draw_rect);
class_def<lua_UIDrawMgr_s>(L, "get_child", &lua_UIDrawMgr_s::get_child);
class_def<lua_UIDrawMgr_s>(L, "get_color", &lua_UIDrawMgr_s::get_color);
class_def<lua_UIDrawMgr_s>(L, "get_point", &lua_UIDrawMgr_s::get_point);
class_def<lua_UIDrawMgr_s>(L, "get_uv", &lua_UIDrawMgr_s::get_uv);
class_def<lua_UIDrawMgr_s>(L, "image", &lua_UIDrawMgr_s::image);
class_def<lua_UIDrawMgr_s>(L, "image_mode", &lua_UIDrawMgr_s::image_mode);
class_def<lua_UIDrawMgr_s>(L, "image_rect", &lua_UIDrawMgr_s::image_rect);
class_def<lua_UIDrawMgr_s>(L, "image_uvani", &lua_UIDrawMgr_s::image_uvani);
class_def<lua_UIDrawMgr_s>(L, "image_uvmap", &lua_UIDrawMgr_s::image_uvmap);
class_def<lua_UIDrawMgr_s>(L, "number_align", &lua_UIDrawMgr_s::number_align);
class_def<lua_UIDrawMgr_s>(L, "number_mode", &lua_UIDrawMgr_s::number_mode);
class_def<lua_UIDrawMgr_s>(L, "progress", &lua_UIDrawMgr_s::progress);
class_def<lua_UIDrawMgr_s>(L, "progress_mode", &lua_UIDrawMgr_s::progress_mode);
class_def<lua_UIDrawMgr_s>(L, "set_color", &lua_UIDrawMgr_s::set_color);
class_def<lua_UIDrawMgr_s>(L, "set_point", &lua_UIDrawMgr_s::set_point);
class_def<lua_UIDrawMgr_s>(L, "set_rect", &lua_UIDrawMgr_s::set_rect);
class_def<lua_UIDrawMgr_s>(L, "set_uv", &lua_UIDrawMgr_s::set_uv);
class_property<lua_UIDrawMgr_s>(L, "center", &lua_UIDrawMgr_s::Getcenter, &lua_UIDrawMgr_s::Setcenter);
class_property<lua_UIDrawMgr_s>(L, "clickscale", &lua_UIDrawMgr_s::Getclickscale, &lua_UIDrawMgr_s::Setclickscale);
class_property<lua_UIDrawMgr_s>(L, "color", &lua_UIDrawMgr_s::Getcolor, &lua_UIDrawMgr_s::Setcolor);
class_property<lua_UIDrawMgr_s>(L, "cullmode", &lua_UIDrawMgr_s::Getcullmode, &lua_UIDrawMgr_s::Setcullmode);
class_property<lua_UIDrawMgr_s>(L, "display", &lua_UIDrawMgr_s::Getdisplay, &lua_UIDrawMgr_s::Setdisplay);
class_property<lua_UIDrawMgr_s>(L, "enabled", &lua_UIDrawMgr_s::Getenabled, &lua_UIDrawMgr_s::Setenabled);
class_property<lua_UIDrawMgr_s>(L, "highlight", &lua_UIDrawMgr_s::Gethighlight, &lua_UIDrawMgr_s::Sethighlight);
class_property<lua_UIDrawMgr_s>(L, "layer", &lua_UIDrawMgr_s::Getlayer, &lua_UIDrawMgr_s::Setlayer);
class_property<lua_UIDrawMgr_s>(L, "number", &lua_UIDrawMgr_s::Getnumber, &lua_UIDrawMgr_s::Setnumber);
class_property<lua_UIDrawMgr_s>(L, "offset3d_z", &lua_UIDrawMgr_s::Getoffset3d_z, &lua_UIDrawMgr_s::Setoffset3d_z);
class_property<lua_UIDrawMgr_s>(L, "opacity", &lua_UIDrawMgr_s::Getopacity, &lua_UIDrawMgr_s::Setopacity);
class_property<lua_UIDrawMgr_s>(L, "pos", &lua_UIDrawMgr_s::Getpos, &lua_UIDrawMgr_s::Setpos);
class_property<lua_UIDrawMgr_s>(L, "rotx", &lua_UIDrawMgr_s::Getrotx, &lua_UIDrawMgr_s::Setrotx);
class_property<lua_UIDrawMgr_s>(L, "roty", &lua_UIDrawMgr_s::Getroty, &lua_UIDrawMgr_s::Setroty);
class_property<lua_UIDrawMgr_s>(L, "rotz", &lua_UIDrawMgr_s::Getrotz, &lua_UIDrawMgr_s::Setrotz);
class_property<lua_UIDrawMgr_s>(L, "scale", &lua_UIDrawMgr_s::Getscale, &lua_UIDrawMgr_s::Setscale);
class_property<lua_UIDrawMgr_s>(L, "scalex", &lua_UIDrawMgr_s::Getscalex, &lua_UIDrawMgr_s::Setscalex);
class_property<lua_UIDrawMgr_s>(L, "scaley", &lua_UIDrawMgr_s::Getscaley, &lua_UIDrawMgr_s::Setscaley);
class_property<lua_UIDrawMgr_s>(L, "text", &lua_UIDrawMgr_s::Gettext, &lua_UIDrawMgr_s::Settext);
class_def<lua_UIDrawMgr_s>(L, "text_mode", &lua_UIDrawMgr_s::text_mode);

class_add<lua_UIElement_s>(L, "UIElement");
class_property<lua_UIElement_s>(L, "context", &lua_UIElement_s::getcontext, &lua_UIElement_s::setcontext);
class_def<lua_UIElement_s>(L, "isnull", &lua_UIElement_s::isnull);
class_def<lua_UIElement_s>(L, "equal", &lua_UIElement_s::equal);
class_def<lua_UIElement_s>(L, "__eq", &lua_UIElement_s::operator == );
class_def<lua_UIElement_s>(L, "isscale", &lua_UIElement_s::isscale);
class_property<lua_UIElement_s>(L, "destblend", &lua_UIElement_s::Getdestblend, &lua_UIElement_s::Setdestblend);
class_property<lua_UIElement_s>(L, "fontid", &lua_UIElement_s::Getfontid, &lua_UIElement_s::Setfontid);
class_property<lua_UIElement_s>(L, "image", &lua_UIElement_s::Getimage, &lua_UIElement_s::Setimage);
class_property<lua_UIElement_s>(L, "imagecolor", &lua_UIElement_s::Getimagecolor, &lua_UIElement_s::Setimagecolor);
class_def<lua_UIElement_s>(L, "setscale", &lua_UIElement_s::setscale);
class_property<lua_UIElement_s>(L, "srcblend", &lua_UIElement_s::Getsrcblend, &lua_UIElement_s::Setsrcblend);
class_property<lua_UIElement_s>(L, "textcolor", &lua_UIElement_s::Gettextcolor, &lua_UIElement_s::Settextcolor);
class_property<lua_UIElement_s>(L, "textureblend", &lua_UIElement_s::Gettextureblend, &lua_UIElement_s::Settextureblend);
class_property<lua_UIElement_s>(L, "textureid", &lua_UIElement_s::Gettextureid, &lua_UIElement_s::Settextureid);
class_property<lua_UIElement_s>(L, "texturemode", &lua_UIElement_s::Gettexturemode, &lua_UIElement_s::Settexturemode);
class_property<lua_UIElement_s>(L, "texturerect", &lua_UIElement_s::Gettexturerect, &lua_UIElement_s::Settexturerect);
class_property<lua_UIElement_s>(L, "tileheight", &lua_UIElement_s::Gettileheight, &lua_UIElement_s::Settileheight);
class_property<lua_UIElement_s>(L, "tilewidth", &lua_UIElement_s::Gettilewidth, &lua_UIElement_s::Settilewidth);
class_property<lua_UIElement_s>(L, "uvaction", &lua_UIElement_s::Getuvaction, &lua_UIElement_s::Setuvaction);
class_property<lua_UIElement_s>(L, "uvspeed", &lua_UIElement_s::Getuvspeed, &lua_UIElement_s::Setuvspeed);
class_def<lua_UIElement_s>(L, "uvaction_at", &lua_UIElement_s::uvaction_at);
class_def<lua_UIElement_s>(L, "uvactioncount", &lua_UIElement_s::uvactioncount);

class_add<lua_UIGroup_s>(L, "UIGroup");
class_property<lua_UIGroup_s>(L, "context", &lua_UIGroup_s::getcontext, &lua_UIGroup_s::setcontext);
class_def<lua_UIGroup_s>(L, "isnull", &lua_UIGroup_s::isnull);
class_def<lua_UIGroup_s>(L, "equal", &lua_UIGroup_s::equal);
class_def<lua_UIGroup_s>(L, "__eq", &lua_UIGroup_s::operator == );
class_def<lua_UIGroup_s>(L, "at", &lua_UIGroup_s::at);
class_def<lua_UIGroup_s>(L, "bind_click", &lua_UIGroup_s::bind_click);
class_def<lua_UIGroup_s>(L, "bind_draw", &lua_UIGroup_s::bind_draw);
class_def<lua_UIGroup_s>(L, "bind_event", &lua_UIGroup_s::bind_event);
class_def<lua_UIGroup_s>(L, "bind_eventby", &lua_UIGroup_s::bind_eventby);
class_def<lua_UIGroup_s>(L, "bind_image", &lua_UIGroup_s::bind_image);
class_def<lua_UIGroup_s>(L, "clone", &lua_UIGroup_s::clone);
class_def<lua_UIGroup_s>(L, "cloneto", &lua_UIGroup_s::cloneto);
class_def<lua_UIGroup_s>(L, "count", &lua_UIGroup_s::count);
class_def<lua_UIGroup_s>(L, "dialog", &lua_UIGroup_s::dialog);
class_def<lua_UIGroup_s>(L, "enable", &lua_UIGroup_s::enable);
class_def<lua_UIGroup_s>(L, "framemove", &lua_UIGroup_s::framemove);
class_def<lua_UIGroup_s>(L, "framemove2", &lua_UIGroup_s::framemove2);
class_def<lua_UIGroup_s>(L, "getvalf", &lua_UIGroup_s::getvalf);
class_def<lua_UIGroup_s>(L, "getvali", &lua_UIGroup_s::getvali);
class_def<lua_UIGroup_s>(L, "getvalt", &lua_UIGroup_s::getvalt);
class_def<lua_UIGroup_s>(L, "height", &lua_UIGroup_s::height);
class_def<lua_UIGroup_s>(L, "image_mode", &lua_UIGroup_s::image_mode);
class_def<lua_UIGroup_s>(L, "interval", &lua_UIGroup_s::interval);
class_def<lua_UIGroup_s>(L, "interval2", &lua_UIGroup_s::interval2);
class_def<lua_UIGroup_s>(L, "playani", &lua_UIGroup_s::playani);
class_def<lua_UIGroup_s>(L, "search", &lua_UIGroup_s::search);
class_property<lua_UIGroup_s>(L, "action", &lua_UIGroup_s::Getaction, &lua_UIGroup_s::Setaction);
class_property<lua_UIGroup_s>(L, "actionby", &lua_UIGroup_s::Getactionby, &lua_UIGroup_s::Setactionby);
class_property<lua_UIGroup_s>(L, "center", &lua_UIGroup_s::Getcenter, &lua_UIGroup_s::Setcenter);
class_property<lua_UIGroup_s>(L, "currenttime", &lua_UIGroup_s::Getcurrenttime, &lua_UIGroup_s::Setcurrenttime);
class_property<lua_UIGroup_s>(L, "display", &lua_UIGroup_s::Getdisplay, &lua_UIGroup_s::Setdisplay);
class_property<lua_UIGroup_s>(L, "enabled", &lua_UIGroup_s::Getenabled, &lua_UIGroup_s::Setenabled);
class_property<lua_UIGroup_s>(L, "left", &lua_UIGroup_s::Getleft, &lua_UIGroup_s::Setleft);
class_property<lua_UIGroup_s>(L, "name", &lua_UIGroup_s::Getname, &lua_UIGroup_s::Setname);
class_property<lua_UIGroup_s>(L, "opacity", &lua_UIGroup_s::Getopacity, &lua_UIGroup_s::Setopacity);
class_property<lua_UIGroup_s>(L, "pos", &lua_UIGroup_s::Getpos, &lua_UIGroup_s::Setpos);
class_property<lua_UIGroup_s>(L, "rot", &lua_UIGroup_s::Getrot, &lua_UIGroup_s::Setrot);
class_def<lua_UIGroup_s>(L, "setRotDir", &lua_UIGroup_s::setRotDir);
class_property<lua_UIGroup_s>(L, "speed", &lua_UIGroup_s::Getspeed, &lua_UIGroup_s::Setspeed);
class_property<lua_UIGroup_s>(L, "top", &lua_UIGroup_s::Gettop, &lua_UIGroup_s::Settop);
class_def<lua_UIGroup_s>(L, "setvalf", &lua_UIGroup_s::setvalf);
class_def<lua_UIGroup_s>(L, "setvali", &lua_UIGroup_s::setvali);
class_def<lua_UIGroup_s>(L, "setvalt", &lua_UIGroup_s::setvalt);
class_def<lua_UIGroup_s>(L, "show", &lua_UIGroup_s::show);
class_def<lua_UIGroup_s>(L, "timeout", &lua_UIGroup_s::timeout);
class_def<lua_UIGroup_s>(L, "timeout2", &lua_UIGroup_s::timeout2);
class_def<lua_UIGroup_s>(L, "width", &lua_UIGroup_s::width);

class_add<lua_Camera>(L, "Camera");
class_con<lua_Camera>(L, constructor<lua_Camera>);
class_def<lua_Camera>(L, "aaboxin", &lua_Camera::aaboxin);
class_def<lua_Camera>(L, "dither", &lua_Camera::dither);
class_def<lua_Camera>(L, "get2d_from3d", &lua_Camera::get2d_from3d);
class_def<lua_Camera>(L, "get3d_from2d", &lua_Camera::get3d_from2d);
class_def<lua_Camera>(L, "isdisplay", &lua_Camera::isdisplay);
class_def<lua_Camera>(L, "isortho", &lua_Camera::isortho);
class_def<lua_Camera>(L, "isperspective", &lua_Camera::isperspective);
class_def<lua_Camera>(L, "nextdist", &lua_Camera::nextdist);
class_def<lua_Camera>(L, "nextdither", &lua_Camera::nextdither);
class_def<lua_Camera>(L, "nextrot", &lua_Camera::nextrot);
class_def<lua_Camera>(L, "nextto", &lua_Camera::nextto);
class_def<lua_Camera>(L, "orthoproj", &lua_Camera::orthoproj);
class_def<lua_Camera>(L, "perspectiveproj", &lua_Camera::perspectiveproj);
class_def<lua_Camera>(L, "planein", &lua_Camera::planein);
class_def<lua_Camera>(L, "play", &lua_Camera::play);
class_def<lua_Camera>(L, "pointin", &lua_Camera::pointin);
class_property<lua_Camera>(L, "area", &lua_Camera::Getarea, &lua_Camera::Setarea);
class_property<lua_Camera>(L, "data", &lua_Camera::Getdata, &lua_Camera::Setdata);
class_property<lua_Camera>(L, "distance", &lua_Camera::Getdistance, &lua_Camera::Setdistance);
class_property<lua_Camera>(L, "distance_max", &lua_Camera::Getdistance_max, &lua_Camera::Setdistance_max);
class_property<lua_Camera>(L, "distance_min", &lua_Camera::Getdistance_min, &lua_Camera::Setdistance_min);
class_property<lua_Camera>(L, "enabled", &lua_Camera::Getenabled, &lua_Camera::Setenabled);
class_property<lua_Camera>(L, "eye", &lua_Camera::Geteye, &lua_Camera::Seteye);
class_property<lua_Camera>(L, "fov", &lua_Camera::Getfov, &lua_Camera::Setfov);
class_property<lua_Camera>(L, "height", &lua_Camera::Getheight, &lua_Camera::Setheight);
class_property<lua_Camera>(L, "intersect_eye", &lua_Camera::Getintersect_eye, &lua_Camera::Setintersect_eye);
class_property<lua_Camera>(L, "lookat", &lua_Camera::Getlookat, &lua_Camera::Setlookat);
class_property<lua_Camera>(L, "offset_down", &lua_Camera::Getoffset_down, &lua_Camera::Setoffset_down);
class_property<lua_Camera>(L, "offset_enable", &lua_Camera::Getoffset_enable, &lua_Camera::Setoffset_enable);
class_property<lua_Camera>(L, "pos", &lua_Camera::Getpos, &lua_Camera::Setpos);
class_property<lua_Camera>(L, "projmat", &lua_Camera::Getprojmat, &lua_Camera::Setprojmat);
class_property<lua_Camera>(L, "rot", &lua_Camera::Getrot, &lua_Camera::Setrot);
class_property<lua_Camera>(L, "target", &lua_Camera::Gettarget, &lua_Camera::Settarget);
class_property<lua_Camera>(L, "viewmat", &lua_Camera::Getviewmat, &lua_Camera::Setviewmat);
class_property<lua_Camera>(L, "width", &lua_Camera::Getwidth, &lua_Camera::Setwidth);
class_property<lua_Camera>(L, "zfar", &lua_Camera::Getzfar, &lua_Camera::Setzfar);
class_property<lua_Camera>(L, "znearest", &lua_Camera::Getznearest, &lua_Camera::Setznearest);
class_def<lua_Camera>(L, "spherein", &lua_Camera::spherein);
class_def<lua_Camera>(L, "stop", &lua_Camera::stop);

class_add<lua_GameMgr>(L, "GameMgr");
class_con<lua_GameMgr>(L, constructor<lua_GameMgr>);
class_def<lua_GameMgr>(L, "create", &lua_GameMgr::create);
class_def<lua_GameMgr>(L, "framemove", &lua_GameMgr::framemove);
class_def<lua_GameMgr>(L, "getheight_3d", &lua_GameMgr::getheight_3d);
class_def<lua_GameMgr>(L, "interval", &lua_GameMgr::interval);
class_def<lua_GameMgr>(L, "leader", &lua_GameMgr::leader);
class_def<lua_GameMgr>(L, "loadtext", &lua_GameMgr::loadtext);
class_def<lua_GameMgr>(L, "loadtextby", &lua_GameMgr::loadtextby);
class_def<lua_GameMgr>(L, "objectat", &lua_GameMgr::objectat);
class_def<lua_GameMgr>(L, "objectcount", &lua_GameMgr::objectcount);
class_def<lua_GameMgr>(L, "plotfinish", &lua_GameMgr::plotfinish);
class_def<lua_GameMgr>(L, "plotid", &lua_GameMgr::plotid);
class_def<lua_GameMgr>(L, "plotnext", &lua_GameMgr::plotnext);
class_def<lua_GameMgr>(L, "plotto", &lua_GameMgr::plotto);
class_def<lua_GameMgr>(L, "ray3d", &lua_GameMgr::ray3d);
class_def<lua_GameMgr>(L, "ray3d_dir", &lua_GameMgr::ray3d_dir);
class_def<lua_GameMgr>(L, "ray3d_dir2", &lua_GameMgr::ray3d_dir2);
class_def<lua_GameMgr>(L, "ray3d_getname", &lua_GameMgr::ray3d_getname);
class_def<lua_GameMgr>(L, "ray3d_length", &lua_GameMgr::ray3d_length);
class_def<lua_GameMgr>(L, "ray3d_normal", &lua_GameMgr::ray3d_normal);
class_def<lua_GameMgr>(L, "ray3d_point", &lua_GameMgr::ray3d_point);
class_def<lua_GameMgr>(L, "reborn", &lua_GameMgr::reborn);
class_def<lua_GameMgr>(L, "resetAll", &lua_GameMgr::resetAll);
class_def<lua_GameMgr>(L, "scene", &lua_GameMgr::scene);
class_def<lua_GameMgr>(L, "search", &lua_GameMgr::search);
class_def<lua_GameMgr>(L, "searchby", &lua_GameMgr::searchby);
class_def<lua_GameMgr>(L, "searchby_sceneid", &lua_GameMgr::searchby_sceneid);
class_property<lua_GameMgr>(L, "camerafollow", &lua_GameMgr::Getcamerafollow, &lua_GameMgr::Setcamerafollow);
class_property<lua_GameMgr>(L, "cameratarget", &lua_GameMgr::Getcameratarget, &lua_GameMgr::Setcameratarget);
class_property<lua_GameMgr>(L, "plotenable", &lua_GameMgr::Getplotenable, &lua_GameMgr::Setplotenable);
class_def<lua_GameMgr>(L, "timeout", &lua_GameMgr::timeout);


class_add<lua_System>(L, "System");
class_con<lua_System>(L, constructor<lua_System>);
class_def<lua_System>(L, "fps", &lua_System::fps);
class_def<lua_System>(L, "gravity", &lua_System::gravity);
class_def<lua_System>(L, "sensor", &lua_System::sensor);
class_def<lua_System>(L, "sensor_rotator", &lua_System::sensor_rotator);
class_property<lua_System>(L, "backcolor", &lua_System::Getbackcolor, &lua_System::Setbackcolor);
class_property<lua_System>(L, "cullmode", &lua_System::Getcullmode, &lua_System::Setcullmode);
class_property<lua_System>(L, "enablevr", &lua_System::Getenablevr, &lua_System::Setenablevr);
class_property<lua_System>(L, "enablevr2", &lua_System::Getenablevr2, &lua_System::Setenablevr2);
class_property<lua_System>(L, "enablez", &lua_System::Getenablez, &lua_System::Setenablez);
class_property<lua_System>(L, "gravity_smooth", &lua_System::Getgravity_smooth, &lua_System::Setgravity_smooth);
class_property<lua_System>(L, "maxfps", &lua_System::Getmaxfps, &lua_System::Setmaxfps);
class_property<lua_System>(L, "mute", &lua_System::Getmute, &lua_System::Setmute);
class_property<lua_System>(L, "opacitymode", &lua_System::Getopacitymode, &lua_System::Setopacitymode);
class_property<lua_System>(L, "showfps", &lua_System::Getshowfps, &lua_System::Setshowfps);
class_property<lua_System>(L, "showui", &lua_System::Getshowui, &lua_System::Setshowui);
class_property<lua_System>(L, "uioffset", &lua_System::Getuioffset, &lua_System::Setuioffset);
class_property<lua_System>(L, "uiscale", &lua_System::Getuiscale, &lua_System::Setuiscale);
class_property<lua_System>(L, "vr_rotx", &lua_System::Getvr_rotx, &lua_System::Setvr_rotx);
class_property<lua_System>(L, "vr_smooth", &lua_System::Getvr_smooth, &lua_System::Setvr_smooth);
class_property<lua_System>(L, "vrdistance", &lua_System::Getvrdistance, &lua_System::Setvrdistance);
class_def<lua_System>(L, "vibrator", &lua_System::vibrator);
class_def<lua_System>(L, "vr_beginsmooth", &lua_System::vr_beginsmooth);
class_def<lua_System>(L, "vr_front", &lua_System::vr_front);

def(L, "apptime", &lua_Global::Gapptime);
def(L, "elapsedtime", &lua_Global::Gelapsedtime);
def(L, "timeout", &lua_Global::Gtimeout);
def(L, "interval", &lua_Global::Ginterval);
def(L, "framemove", &lua_Global::Gframemove);
def(L, "timeout2", &lua_Global::Gtimeout2);
def(L, "interval2", &lua_Global::Ginterval2);
def(L, "framemove2", &lua_Global::Gframemove2);
def(L, "cleartimeby", &lua_Global::Gcleartimeby);
def(L, "cleartimeby2", &lua_Global::Gcleartimeby2);
def(L, "cleartime", &lua_Global::Gcleartime);
def(L, "breaktime", &lua_Global::Gbreaktime);
def(L, "cleartime_all", &lua_Global::Gcleartime_all);
def(L, "localtime", &lua_Global::Glocaltime);
def(L, "timediff_d", &lua_Global::Gtimediff_d);
def(L, "timediff_h", &lua_Global::Gtimediff_h);
def(L, "timediff_m", &lua_Global::Gtimediff_m);
def(L, "timediff_s", &lua_Global::Gtimediff_s);
def(L, "pause", &lua_Global::Gpause);
def(L, "speed", &lua_Global::Gspeed);
def(L, "mousepoint", &lua_Global::Gmousepoint);
def(L, "mousepoint2d", &lua_Global::Gmousepoint2d);
def(L, "screenwidth", &lua_Global::Gscreenwidth);
def(L, "screenheight", &lua_Global::Gscreenheight);
def(L, "gamewidth", &lua_Global::Ggamewidth);
def(L, "gameheight", &lua_Global::Ggameheight);
def(L, "exit", &lua_Global::Gexit);
def(L, "alert", &lua_Global::Galert);
def(L, "msgbox", &lua_Global::Gmsgbox);
def(L, "lan", &lua_Global::Glan);
def(L, "setlanguage", &lua_Global::Gsetlanguage);
def(L, "language_image", &lua_Global::Glanguage_image);
def(L, "bind_input", &lua_Global::Gbind_input);
def(L, "infile", &lua_Global::Ginfile);
def(L, "insystem", &lua_Global::Ginsystem);
def(L, "goto", &lua_Global::Ggoto);
def(L, "sendmessage", &lua_Global::Gsendmessage);
def(L, "ad_show", &lua_Global::Gad_show);
def(L, "ad_setpos", &lua_Global::Gad_setpos);
def(L, "openurl", &lua_Global::Gopenurl);
def(L, "gotocomment", &lua_Global::Ggotocomment);
def(L, "openrank", &lua_Global::Gopenrank);
def(L, "rankscore", &lua_Global::Grankscore);
def(L, "achievement", &lua_Global::Gachievement);
def(L, "worldmat", &lua_Global::Gworldmat);
def(L, "viewmat", &lua_Global::Gviewmat);
def(L, "projmat", &lua_Global::Gprojmat);
def(L, "round", &lua_Global::Ground);
def(L, "abs", &lua_Global::Gabs);
def(L, "symbol", &lua_Global::Gsymbol);
def(L, "atoi", &lua_Global::Gatoi);
def(L, "isstr", &lua_Global::Gisstr);
def(L, "rand", &lua_Global::Grand);
def(L, "int", &lua_Global::Gint);
def(L, "point_in", &lua_Global::Gpoint_in);
def(L, "tovalue", &lua_Global::Gtovalue);
def(L, "tometer", &lua_Global::Gtometer);
def(L, "rgba", &lua_Global::Grgba);
def(L, "rgbaf", &lua_Global::Grgbaf);
def(L, "clamp_angle", &lua_Global::Gclamp_angle);
def(L, "clamp", &lua_Global::Gclamp);
def(L, "dlg_create", &lua_Global::Gdlg_create);
def(L, "dlg_delete", &lua_Global::Gdlg_delete);
def(L, "dlg_load", &lua_Global::Gdlg_load);
def(L, "dlg_loadnew", &lua_Global::Gdlg_loadnew);
def(L, "dlg_loadAndProgress", &lua_Global::Gdlg_loadAndProgress);
def(L, "dlg_search", &lua_Global::Gdlg_search);
def(L, "dlg_at", &lua_Global::Gdlg_at);
def(L, "dlg_count", &lua_Global::Gdlg_count);
def(L, "draw_create", &lua_Global::Gdraw_create);
def(L, "findway_create", &lua_Global::Gfindway_create);
def(L, "findway_delete", &lua_Global::Gfindway_delete);
def(L, "table_load", &lua_Global::Gtable_load);
def(L, "table_delete", &lua_Global::Gtable_delete);
def(L, "texture_load", &lua_Global::Gtexture_load);
def(L, "texture_delete", &lua_Global::Gtexture_delete);
def(L, "mesh_load", &lua_Global::Gmesh_load);
def(L, "mesh_delete", &lua_Global::Gmesh_delete);
def(L, "map3d_load", &lua_Global::Gmap3d_load);
def(L, "map3d_delete", &lua_Global::Gmap3d_delete);
def(L, "text_create", &lua_Global::Gtext_create);
def(L, "text_create2", &lua_Global::Gtext_create2);
def(L, "text_update", &lua_Global::Gtext_update);
def(L, "text_delete", &lua_Global::Gtext_delete);
def(L, "include", &lua_Global::Ginclude);
def(L, "config", &lua_Global::Gconfig);
def(L, "isconfig", &lua_Global::Gisconfig);
def(L, "readt", &lua_Global::Greadt);
def(L, "readf", &lua_Global::Greadf);
def(L, "readi", &lua_Global::Greadi);
def(L, "writet", &lua_Global::Gwritet);
def(L, "writef", &lua_Global::Gwritef);
def(L, "writei", &lua_Global::Gwritei);
def(L, "configfile", &lua_Global::Gconfigfile);
def(L, "configsave", &lua_Global::Gconfigsave);
def(L, "valdata", &lua_Global::Gvaldata);
def(L, "isval", &lua_Global::Gisval);
def(L, "getvalt", &lua_Global::Ggetvalt);
def(L, "getvalf", &lua_Global::Ggetvalf);
def(L, "getvali", &lua_Global::Ggetvali);
def(L, "setvalt", &lua_Global::Gsetvalt);
def(L, "setvalf", &lua_Global::Gsetvalf);
def(L, "setvali", &lua_Global::Gsetvali);
def(L, "playwave", &lua_Global::Gplaywave);
def(L, "playmp3", &lua_Global::Gplaymp3);
def(L, "playbgm", &lua_Global::Gplaybgm);
def(L, "playbgm2", &lua_Global::Gplaybgm2);
def(L, "playmp3_2", &lua_Global::Gplaymp3_2);
def(L, "playsnd", &lua_Global::Gplaysnd);
def(L, "stopsnd", &lua_Global::Gstopsnd);
def(L, "stopsndall", &lua_Global::Gstopsndall);
def(L, "setsndvolume", &lua_Global::Gsetsndvolume);
def(L, "setsndvolumepan", &lua_Global::Gsetsndvolumepan);
def(L, "setsndrate", &lua_Global::Gsetsndrate);
def(L, "setupwave", &lua_Global::Gsetupwave);
def(L, "draw_bind", &lua_Global::Gdraw_bind);
def(L, "draw2d_rect", &lua_Global::Gdraw2d_rect);
def(L, "draw2d_line", &lua_Global::Gdraw2d_line);
def(L, "draw2d_linec", &lua_Global::Gdraw2d_linec);
def(L, "draw2d_text", &lua_Global::Gdraw2d_text);
def(L, "draw2d_texture", &lua_Global::Gdraw2d_texture);
def(L, "draw2d_triangle", &lua_Global::Gdraw2d_triangle);
def(L, "draw3d_triangle", &lua_Global::Gdraw3d_triangle);
def(L, "draw3d_box", &lua_Global::Gdraw3d_box);
def(L, "draw3d_line", &lua_Global::Gdraw3d_line);
def(L, "jinit", &lua_Global::Gjinit);
def(L, "jbind", &lua_Global::Gjbind);
def(L, "jget", &lua_Global::Gjget);
def(L, "jgeti", &lua_Global::Gjgeti);
def(L, "jgetf", &lua_Global::Gjgetf);
def(L, "jclear", &lua_Global::Gjclear);
def(L, "jset", &lua_Global::Gjset);
def(L, "jseti", &lua_Global::Gjseti);
def(L, "jsetf", &lua_Global::Gjsetf);
def(L, "jrequest", &lua_Global::Gjrequest);
def(L, "download", &lua_Global::Gdownload);
def(L, "net_init", &lua_Global::Gnet_init);
def(L, "net_onlink", &lua_Global::Gnet_onlink);
def(L, "net_ondislink", &lua_Global::Gnet_ondislink);
def(L, "net_onmessage", &lua_Global::Gnet_onmessage);
def(L, "net_server", &lua_Global::Gnet_server);
def(L, "net_search", &lua_Global::Gnet_search);
def(L, "net_server_max_links", &lua_Global::Gnet_server_max_links);
def(L, "net_connect", &lua_Global::Gnet_connect);
def(L, "net_close", &lua_Global::Gnet_close);
def(L, "net_stopall", &lua_Global::Gnet_stopall);
def(L, "net_get", &lua_Global::Gnet_get);
def(L, "net_geti", &lua_Global::Gnet_geti);
def(L, "net_getf", &lua_Global::Gnet_getf);
def(L, "net_begin", &lua_Global::Gnet_begin);
def(L, "net_set", &lua_Global::Gnet_set);
def(L, "net_seti", &lua_Global::Gnet_seti);
def(L, "net_setf", &lua_Global::Gnet_setf);
def(L, "net_send", &lua_Global::Gnet_send);
def(L, "random_init", &lua_Global::Grandom_init);
def(L, "random_weight", &lua_Global::Grandom_weight);
def(L, "random_do", &lua_Global::Grandom_do);
def(L, "random_table", &lua_Global::Grandom_table);
def(L, "random_get", &lua_Global::Grandom_get);
def(L, "c_add", &lua_Global::Gc_add);
def(L, "c_set", &lua_Global::Gc_set);
def(L, "c_get", &lua_Global::Gc_get);


static const char* s_globalVar = "camera = Camera();\ngamemgr = GameMgr();\nsystem = System();\nnull=nil;\nfunction dlg_load_vars(name)\nlocal temp=dlg_load(name);\ntemp.vars=init_vars(temp);\nreturn temp\nend\nfunction dlg_loadnew_vars(name)\nlocal temp=dlg_loadnew(name);\ntemp.vars=init_vars(temp);\nreturn temp\nend\n";
if(luaL_loadstring(L, s_globalVar) != 0)return false;
if(lua_pcall(L, 0, LUA_MULTRET, 0) != 0)return false;


return 0;
}

};
