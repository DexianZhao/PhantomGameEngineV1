//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   PhantomAPI.cpp
	软件名称   :   幻影游戏引擎

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	
	
	Copyright 2009-2016 Zhao Dexian
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomAPI.h"
#include "PhantomMesh.h"
#include "PhantomManager.h"
#include "PhantomFile.h"
#include "LuaManager.h"
#include "PhantomLanguage.h"
#include "PhantomMap2D.h"
#include "PhantomGame.h"

#ifndef RGB
#define RGB(r,g,b)          ((DWORD)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif

namespace Phantom{

///*API*/VOID			WINAPI	Sky_Setname/*场景天空3D_设置名字*/(ISkyPtr* _self/*场景天空*/, LPCSTR szName/*名字*/);
///*API*/LPCSTR		WINAPI	Sky_Getname/*场景天空3D_取名字*/(ISkyPtr* _self/*场景天空*/);
///*API*/VOID			WINAPI	Sky_Setenabled/*场景天空3D_设置有效*/(ISkyPtr* _self/*场景天空*/, BOOL bEnable/*有效*/);
///*API*/BOOL			WINAPI	Sky_Getenabled/*场景天空3D_取有效*/(ISkyPtr* _self/*场景天空*/);
///*API*/VOID			WINAPI	Sky_Setcolor/*场景天空3D_设置颜色*/(ISkyPtr* _self/*场景天空*/, const color4* c/*颜色*/);
///*API*/VOID			WINAPI	Sky_Getcolor/*场景天空3D_取颜色*/(ISkyPtr* _self/*场景天空*/, color4* _return/*ret*/);
///*API*/VOID			WINAPI	Sky_Setambient/*场景天空3D_设置环境色*/(ISkyPtr* _self/*场景天空*/, const color4* c/*颜色*/);
///*API*/VOID			WINAPI	Sky_Getambient/*场景天空3D_取环境色*/(ISkyPtr* _self/*场景天空*/, color4* _return/*ret*/);
///*API*/VOID			WINAPI	Sky_Setsunangle_x/*场景天空3D_设置阳光角度X*/(ISkyPtr* _self/*场景天空*/, float c/*转动*/);
///*API*/float		WINAPI	Sky_Getsunangle_x/*场景天空3D_取阳光角度X*/(ISkyPtr* _self/*场景天空*/);
///*API*/VOID			WINAPI	Sky_Setsunangle_z/*场景天空3D_设置阳光角度Z*/(ISkyPtr* _self/*场景天空*/, float c/*转动*/);
///*API*/float		WINAPI	Sky_Getsunangle_z/*场景天空3D_取阳光角度Z*/(ISkyPtr* _self/*场景天空*/);
///*API*/VOID			WINAPI	Sky_Setwindangle/*场景天空3D_设置风向*/(ISkyPtr* _self/*场景天空*/, float c/*风向*/);
///*API*/float		WINAPI	Sky_Getwindangle/*场景天空3D_取风向*/(ISkyPtr* _self/*场景天空*/);
///*API*/VOID			WINAPI	Sky_Setwindpower/*场景天空3D_设置风力*/(ISkyPtr* _self/*场景天空*/, float c/*风力*/);
///*API*/float		WINAPI	Sky_Getwindpower/*场景天空3D_取风力*/(ISkyPtr* _self/*场景天空*/);

EventID	GetEventID(const char* szName)
{
	if(stricmp(szName, "onclick") == 0)
		return EventID_Click;
	else if(stricmp(szName, "ondblclick") == 0)
		return EventID_DoubleClick;
	else if(stricmp(szName, "onchanged") == 0)
		return EventID_Changed;
	else if(stricmp(szName, "onplayover") == 0)
		return EventID_PlayOver;
	else if(stricmp(szName, "ondown") == 0)
		return EventID_Down;
	else if(stricmp(szName, "onup") == 0)
		return EventID_Up;
	else if(stricmp(szName, "onmove") == 0)
		return EventID_Move;
//	else if(stricmp(szName, "onpinchopen") == 0)
//		return EventID_PinchOpen;
//	else if(stricmp(szName, "onpinchclose") == 0)
//		return EventID_PinchClose;
//	else if(stricmp(szName, "onlongpress") == 0)
//		return EventID_LongPress;
	else if(stricmp(szName, "ondblclick") == 0)
		return EventID_DoubleClick;
	else if(stricmp(szName, "onchanged") == 0)
		return EventID_Changed;
	else if(stricmp(szName, "onplayover") == 0)
		return EventID_PlayOver;
	else if(stricmp(szName, "onplayoveruv") == 0)
		return EventID_UVPlayOver;
	return EventID_None;
}

int CompareDrawMgr(const void* v1, const void* v2)
{
	UIDrawMgr* d1 = *(UIDrawMgr**)v1;
	UIDrawMgr* d2 = *(UIDrawMgr**)v2;
	if(d1->m_layer < d2->m_layer)
		return -1;
	if(d1->m_layer > d2->m_layer)
		return 1;
	return 0;
}

/*API*/VOID			WINAPI	System_Setshowfps/*渲染系统_设置显示FPS*/(BOOL b/*设置*/)
{
	g_manager.m_bShowFPS = b;
}

/*API*/BOOL			WINAPI	System_Getshowfps/*渲染系统_是否显示FPS*/()
{
	return g_manager.m_bShowFPS;
}

/*API*/VOID			WINAPI	System_Setshowui/*渲染系统_设置渲染UI*/(BOOL b/*设置*/)
{
	GetDlgManager()->m_bShowUI = b;
}

/*API*/BOOL			WINAPI	System_Getshowui/*渲染系统_是否渲染UI*/()
{
	return GetDlgManager()->m_bShowUI;
}


/*API*/VOID			WINAPI	MeshMaterial_Getcolor/*子模型材质_设置漫射光颜色*/(IMeshMaterialPtr* _self/*子模型ID*/, color4* c/*颜色*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	*c = gr->GetMaterialPtr(_self->reserved)->diffuse;
}
/*API*/VOID			WINAPI	MeshMaterial_Getspecular/*子模型材质_设置高光色*/(IMeshMaterialPtr* _self/*子模型ID*/, color4* c/*颜色*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	*c = gr->GetMaterialPtr(_self->reserved)->specular;
}
/*API*/VOID			WINAPI	MeshMaterial_Getselfluminous/*子模型材质_设置自发光色*/(IMeshMaterialPtr* _self/*子模型ID*/, color4* c/*颜色*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	*c = gr->GetMaterialPtr(_self->reserved)->emissive;
}
/*API*/BlendMode	WINAPI	MeshMaterial_Getsrcblend/*子模型材质_设置源混合*/(IMeshMaterialPtr* _self/*子模型ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return (BlendMode)0;
	return (BlendMode)gr->GetMaterialPtr(_self->reserved)->srcBlend;
}
/*API*/BlendMode	WINAPI	MeshMaterial_Getdestblend/*子模型材质_设置目标混合*/(IMeshMaterialPtr* _self/*子模型ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return (BlendMode)0;
	return (BlendMode)gr->GetMaterialPtr(_self->reserved)->destBlend;
}


/*API*/VOID	WINAPI	MeshMaterial_Setcolor/*子模型_设置漫射光颜色*/(IMeshMaterialPtr* _self/*子模型ID*/, const color4* c/*颜色*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	gr->GetMaterialPtr(_self->reserved)->diffuse = *(color4*)c;
}


/*API*/VOID	WINAPI	MeshMaterial_Setspecular/*子模型_设置高光色*/(IMeshMaterialPtr* _self/*子模型ID*/, const color4* c/*颜色*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	gr->GetMaterialPtr(_self->reserved)->specular = *(color4*)c;
}


/*API*/VOID	WINAPI	MeshMaterial_Setselfluminous/*子模型_设置自发光色*/(IMeshMaterialPtr* _self/*子模型ID*/, const color4* c/*颜色*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	gr->GetMaterialPtr(_self->reserved)->emissive = *(color4*)c;
}


/*API*/VOID	WINAPI	MeshMaterial_Setsrcblend/*子模型_设置源混合*/(IMeshMaterialPtr* _self/*子模型ID*/, BlendMode blend/*BLEND_开头常数*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	gr->GetMaterialPtr(_self->reserved)->srcBlend = blend;
}


/*API*/VOID	WINAPI	MeshMaterial_Setdestblend/*子模型_设置目标混合*/(IMeshMaterialPtr* _self/*子模型ID*/, BlendMode blend/*BLEND_开头常数*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	gr->GetMaterialPtr(_self->reserved)->destBlend = blend;
}

/*API*/VOID	WINAPI	MeshMaterial_Settexture/*子模型_设置默认纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* texturePtr/*纹理ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	gr->GetMaterialPtr(_self->reserved)->SetTexture(TEX_DIFFUSE, GetIPtr(texturePtr, Texture, ITexturePtr) );
}


/*API*/VOID	WINAPI	MeshMaterial_Setnormal_texture/*子模型_设置法线纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* texturePtr/*纹理ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	gr->GetMaterialPtr(_self->reserved)->SetTexture(TEX_NORMAL_MAP, GetIPtr(texturePtr, Texture, ITexturePtr) );
}


/*API*/VOID	WINAPI	MeshMaterial_Setspecular_texture/*子模型_设置高光纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* texturePtr/*纹理ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	gr->GetMaterialPtr(_self->reserved)->SetTexture(TEX_SPECULAR_MAP, GetIPtr(texturePtr, Texture, ITexturePtr) );
}


/*API*/VOID	WINAPI	MeshMaterial_Setreflect_texture/*子模型_设置反射纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* texturePtr/*纹理ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	gr->GetMaterialPtr(_self->reserved)->SetTexture(TEX_REFLECTION, GetIPtr(texturePtr, Texture, ITexturePtr) );
}
//

//
/*API*/VOID		WINAPI	MeshMaterial_Gettexture/*子模型_取得默认纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* _return/*返回*/)//, ITexturePtr* texturePtr/*纹理ID*/)
{
	DefineIPtr(ITexturePtr);
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	SetIPtr(_return, gr->GetMaterialPtr(_self->reserved)->GetTexture(TEX_DIFFUSE), Texture, ITexturePtr);
}

/*API*/void	WINAPI	MeshMaterial_Getnormal_texture/*子模型_取得法线纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* _return/*返回*/)//, ITexturePtr* texturePtr/*纹理ID*/)
{
	DefineIPtr(ITexturePtr);
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	SetIPtr(_return, gr->GetMaterialPtr(_self->reserved)->GetTexture(TEX_NORMAL_MAP), Texture, ITexturePtr);
}


/*API*/void	WINAPI	MeshMaterial_Getspecular_texture/*子模型_取得高光纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* _return/*返回*/)//, ITexturePtr* texturePtr/*纹理ID*/)
{
	DefineIPtr(ITexturePtr);
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	SetIPtr(_return, gr->GetMaterialPtr(_self->reserved)->GetTexture(TEX_SPECULAR_MAP), Texture, ITexturePtr);
}


/*API*/void	WINAPI	MeshMaterial_Getreflect_texture/*子模型_取得反射纹理*/(IMeshMaterialPtr* _self/*子模型ID*/, ITexturePtr* _return/*返回*/)//, ITexturePtr* texturePtr/*纹理ID*/)
{
	DefineIPtr(ITexturePtr);
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	SetIPtr(_return, gr->GetMaterialPtr(_self->reserved)->GetTexture(TEX_REFLECTION), Texture, ITexturePtr);
}


//
/*API*/CullMode	WINAPI	MeshMaterial_Getcullmode/*子模型_渲染面*/(IMeshMaterialPtr* _self/*子模型ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return (CullMode)0;
	return (CullMode)gr->GetMaterialPtr(_self->reserved)->cullMode;
}

//正面或背面或双面渲染
/*API*/VOID	WINAPI	MeshMaterial_Setcullmode/*子模型_设置渲染面*/(IMeshMaterialPtr* _self/*子模型ID*/, CullMode cull/*正面或背面或双面渲染*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	gr->GetMaterialPtr(_self->reserved)->cullMode = cull;
}


//
/*API*/AlphaMode	WINAPI	MeshMaterial_Getopacitymode/*子模型_取得透明渲染方法*/(IMeshMaterialPtr* _self/*子模型ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return (AlphaMode)0;
	return (AlphaMode)gr->GetMaterialPtr(_self->reserved)->alphaMode;
}

//AlphaBlend_开头常数
/*API*/VOID	WINAPI	MeshMaterial_Setopacitymode/*子模型_设置透明渲染方法*/(IMeshMaterialPtr* _self/*子模型ID*/, AlphaMode alpha/*AlphaBlend_开头常数*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, IMeshMaterialPtr);
	if(!gr || _self->reserved < 0 || _self->reserved >= gr->GetMaterialCount())return;
	gr->GetMaterialPtr(_self->reserved)->alphaMode = alpha;
}


/*API*/INT			WINAPI	SubMesh_materialcount/*子模型_取材质数量*/(ISubMeshPtr* _self/*子模型ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return 0;
	return gr->GetMaterialCount();
}
/*API*/VOID			WINAPI	SubMesh_material_at/*子模型_取子模型材质*/(ISubMeshPtr* _self/*渲染组ID*/, INT index/*索引*/, IMeshMaterialPtr* _return/*返回*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return;
	SetIPtrI(_return, gr, SubMesh, IMeshMaterialPtr, index);
}

/*API*/INT	WINAPI	SubMesh_vertexcount/*子模型材质_取得顶点数量*/(ISubMeshPtr* _self/*子模型材质ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return 0;
	return gr->GetDataPtr()->m_polygons->GetVertexCount();
}


/*API*/INT	WINAPI	SubMesh_trianglecount/*子模型材质_取得顶点数据*/(ISubMeshPtr* _self/*子模型材质ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return 0;
	return gr->GetDataPtr()->m_polygons->GetFaceCount();
}


/*API*/VOID	WINAPI	SubMesh_vertex_at/*子模型材质_取得顶点数据*/(ISubMeshPtr* _self/*子模型材质ID*/, INT nIndex/*顶点索引*/, Vertex* ret/*返回*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return;
	const Vertex* v	=	gr->GetDataPtr()->m_polygons->GetVertexPtr(nIndex);
	if(!v)return;
	*ret	=	*v;
}


/*API*/VOID	WINAPI	SubMesh_triangle_at/*子模型材质_取得面数据*/(ISubMeshPtr* _self/*子模型材质ID*/, INT nIndex/*面索引*/, Triangle* ret/*返回*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return;
	const Triangle* f		=	gr->GetDataPtr()->m_polygons->GetFacePtr16(nIndex);
	if(!f)return;
	*ret = *f;
}

//
/*API*/BOOL	WINAPI	SubMesh_isskeleton/*子模型材质_是否存在骨骼动画*/(ISubMeshPtr* _self/*子模型材质ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return false;
	return gr->GetDataPtr()->m_polygons->IsBlendBoneMesh();
}


/*API*/BOOL	WINAPI	SubMesh_lod_exists/*子模型材质_是否存在LOD*/(ISubMeshPtr* _self/*子模型材质ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return false;
	return gr->GetDataPtr()->m_polygons->IsLodExists();
}

/*API*/VOID			WINAPI	SubMesh_Setworldmat/*子模型_取子模型矩阵*/(ISubMeshPtr* _self/*渲染组ID*/, matrix4x4* mat/*取*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return;
	gr->SetWorldMatrix(*mat);
}
/*API*/VOID			WINAPI	SubMesh_Getworldmat/*子模型_取子模型矩阵*/(ISubMeshPtr* _self/*渲染组ID*/, matrix4x4* _return/*取*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return;
	*_return = gr->GetWorldMatrix();
}
/*API*/LPCSTR	WINAPI	SubMesh_getname/*子模型材质_取得名称*/(ISubMeshPtr* _self/*子模型材质ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return "";
	return gr->GetDataPtr()->GetName();
}
/*API*/BOOL	WINAPI	SubMesh_Getdisplay/*子模型_取得是否显示*/(ISubMeshPtr* _self/*子模型ID*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return false;

	return gr->IsVisible();
}


/*API*/VOID	WINAPI	SubMesh_Setdisplay/*子模型_设置是否显示*/(ISubMeshPtr* _self/*子模型ID*/, BOOL b/*是否显示*/)
{
	SubMesh* gr	=	GetIPtr(_self, SubMesh, ISubMeshPtr);
	if(!gr)return;
	gr->SetVisible( b == TRUE );
}

/*API*/BOOL	WINAPI	Mesh_isskeleton/*模型数据_是否带骨骼绑定*/(IMeshPtr* _self/*模型数据ID*/)
{
	Mesh* ggd	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!ggd)return false;
	return ggd->GetDataPtr()->IsBlendBoneGeometry();
}


/*API*/float	WINAPI	Mesh_Gets_lodinterval_length/*模型数据_取得LOD区间长度*/(IMeshPtr* _self/*模型数据ID*/)
{
	Mesh* ggd	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!ggd)return 0;
	return ggd->GetDataPtr()->GetLodRangeLength();
}

//LOD区间长度是每个LOD等级之间的长度，比如此值是10米，则每隔10米都会降低一个LOD等级
/*API*/VOID	WINAPI	Mesh_Sets_lodinterval_length/*模型数据_设置LOD区间长度*/(IMeshPtr* _self/*模型数据ID*/, float f/*LOD区间长度*/)
{
	Mesh* ggd	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!ggd)return;
	ggd->GetDataPtr()->SetLodRangeLength( f );
}


/*API*/float	WINAPI	Mesh_Gets_lodmaxvisible/*模型数据_取得LOD最大可见距离*/(IMeshPtr* _self/*模型数据ID*/)
{
	Mesh* ggd	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!ggd)return 0;
	return ggd->GetDataPtr()->GetMaxVisibleLength();
}

//当超过这个距离时自动隐藏
/*API*/VOID	WINAPI	Mesh_Sets_lodmaxvisible/*模型数据_保存数据*/(IMeshPtr* _self/*模型数据ID*/, float f/*最大可见距离*/)
{
	Mesh* ggd	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!ggd)return ;
	ggd->GetDataPtr()->SetMaxVisibleLength( f );
}


/*API*/BOOL	WINAPI	Mesh_Gets_lodhide_onexceed/*模型数据_LOD是否有效*/(IMeshPtr* _self/*模型数据ID*/)
{
	Mesh* ggd	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!ggd)return false;
	return ggd->GetDataPtr()->IsLodEnableMaxVisible();
}


/*API*/VOID	WINAPI	Mesh_Sets_lodhide_onexceed/*模型数据_设置LOD有效*/(IMeshPtr* _self/*模型数据ID*/, BOOL b/*是否有效*/)
{
	Mesh* ggd	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!ggd)return ;
	ggd->GetDataPtr()->SetLodEnableMaxVisible( b );
}


//数量是MeshAction_GetActionCount
//
/*API*/VOID	WINAPI	MeshAction_Setspeed/*模型数据_设置动作速度*/(IMeshActionPtr* _self/*模型数据ID*/, float fSpeed/*速度*/)
{
	MeshActionRange* a	=	GetIPtr(_self, MeshActionRange, IMeshActionPtr);
	if(!a)return;
	a->SetActionSpeed(fSpeed);
}

//动作文件在actionIDMap.editor中定义，记事本打开
/*API*/float	WINAPI	MeshAction_Getspeed/*模型数据_取得动作速度*/(IMeshActionPtr* _self/*模型数据ID*/)
{
	MeshActionRange* a	=	GetIPtr(_self, MeshActionRange, IMeshActionPtr);
	if(!a)return 0;
	return a->GetActionSpeed();
}


//
/*API*/VOID	WINAPI	MeshAction_Setkeybegin/*模型数据_动作循环设置*/(IMeshActionPtr* _self/*模型数据ID*/, INT beginIndex/*开始关键帧*/)
{
	MeshActionRange* a	=	GetIPtr(_self, MeshActionRange, IMeshActionPtr);
	if(!a)return;
	a->SetLoop( beginIndex, a->GetLoopEnd() );
}

/*API*/VOID	WINAPI	MeshAction_Setkeyend/*模型数据_动作循环设置*/(IMeshActionPtr* _self/*模型数据ID*/, INT endIndex/*结束关键帧*/)
{
	MeshActionRange* a	=	GetIPtr(_self, MeshActionRange, IMeshActionPtr);
	if(!a)return;
	a->SetLoop( a->GetLoopBegin(), endIndex );
}

/*API*/INT	WINAPI	MeshAction_keycount/*模型数据_取得动作关键帧数量*/(IMeshActionPtr* _self/*模型数据ID*/)
{
	MeshActionRange* a	=	GetIPtr(_self, MeshActionRange, IMeshActionPtr);
	if(!a)return 0;
	return a->GetKeyCount();
}


/*API*/INT	WINAPI	MeshAction_Getkeybegin/*模型数据_取得循环开始关键帧*/(IMeshActionPtr* _self/*模型数据ID*/)
{
	MeshActionRange* a	=	GetIPtr(_self, MeshActionRange, IMeshActionPtr);
	if(!a)return 0;
	return a->GetLoopBegin();
}


/*API*/INT	WINAPI	MeshAction_Getkeyend/*模型数据_取得循环结束关键帧*/(IMeshActionPtr* _self/*模型数据ID*/)
{
	MeshActionRange* a	=	GetIPtr(_self, MeshActionRange, IMeshActionPtr);
	if(!a)return 0;
	return a->GetLoopEnd();
}


//
/*API*/float	WINAPI	MeshAction_Getlerptime/*模型数据_取得平滑过渡时间*/(IMeshActionPtr* _self/*模型数据ID*/)
{
	MeshActionRange* a	=	GetIPtr(_self, MeshActionRange, IMeshActionPtr);
	if(!a)return 0;
	return a->GetNextLerpTime();
}

//取得连接到下一个动作的平滑过渡时间
/*API*/VOID		WINAPI	MeshAction_Setlerptime/*模型数据_设置平滑过渡时间*/(IMeshActionPtr* _self/*模型数据ID*/, float f/*过度时间*/)
{
	MeshActionRange* a	=	GetIPtr(_self, MeshActionRange, IMeshActionPtr);
	if(!a)return;
	a->SetNextLerpTime(f);
}


/*API*/INT	WINAPI	Mesh_s_actioncount/*模型数据_取得动作数量*/(IMeshPtr* _self/*模型数据ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return 0;
	return gg->GetDataPtr()->GetActionCount();
}


/*API*/VOID	WINAPI	Mesh_s_action_at/*模型数据_取得动作ID*/(IMeshPtr* _self/*模型数据ID*/, INT index/*索引*/, IMeshActionPtr* _return/*模型动作*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	MeshActionRange* act = gg->GetDataPtr()->GetActionPtr(gg->GetDataPtr()->GetActionID(index));
	if(!act)return;
	SetIPtr(_return, act, MeshActionRange, IMeshActionPtr);
}

/*API*/VOID		WINAPI	Mesh_Getpos/*模型_取得位置*/(IMeshPtr* _self/*渲染组ID*/, float3* _return/*ret*/)//, float3* pos/*返回*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	*_return	=	gg->GetWorldMatrix().GetPosition();
}


/*API*/VOID	WINAPI	Mesh_Setpos/*模型_保存数据*/(IMeshPtr* _self/*渲染组ID*/, const float3* pos/*位置*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetPosition(*pos);
}


//必须得在渲染过程中调用
/*API*/VOID	WINAPI	Mesh_Setrot/*模型_设置旋转数据*/(IMeshPtr* _self/*渲染组ID*/, const float3* rot/*旋转弧度*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetYawPitchRoll(*rot);
}


/*API*/VOID	WINAPI	Mesh_Setscale/*模型_设置缩放*/(IMeshPtr* _self/*渲染组ID*/, const float3* scale/*缩放*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetScale(*scale);
}

/*API*/VOID		WINAPI	Mesh_playaction/*模型_播放某段动作*/(IMeshPtr* _self/*渲染组ID*/, int keyBegin/*开始关键帧*/, int keyEnd/*结束关键帧*/, int nLoop/*循环次数*/)//按照设定值播放一次动作，从开始关键帧到结束关键帧，nLoop=-1表示无限循环，nLoop=0或1表示只播放一次
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	if(nLoop==0)nLoop=1;
	gg->PlayAction(keyBegin, keyEnd, nLoop);
}

/*API*/VOID	WINAPI	Mesh_Getrot/*模型_取得旋转数据*/(IMeshPtr* _self/*渲染组ID*/, float3* _return/*ret*/)//, float3* rot/*返回*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	*_return	=	gg->GetYawPitchRoll();
}


/*API*/VOID	WINAPI	Mesh_Getscale/*模型_取得缩放*/(IMeshPtr* _self/*渲染组ID*/, float3* _return/*ret*/)//, float3* scale/*返回*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	*_return	=	gg->GetScale();
}


/*API*/VOID	WINAPI	Mesh_Setcolor/*模型_设置漫射光颜色*/(IMeshPtr* _self/*渲染组ID*/, const color4* c/*颜色*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetDiffuseColor(*c);
}


/*API*/VOID	WINAPI	Mesh_Getcolor/*模型_设置漫射光颜色*/(IMeshPtr* _self/*渲染组ID*/, color4* _return/*ret*/)//, color4* c/*返回*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	*_return	=	gg->GetDiffuseColor();
}

/*API*/BOOL		WINAPI	Mesh_bind_mesh/*模型_绑定模型*/(IMeshPtr* _self/*渲染组*/, LPCSTR szBoneName/*骨骼名*/, IMeshPtr* mesh/*模型*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return false;
	return gg->BindMesh(szBoneName, GetIPtr(mesh, Mesh, IMeshPtr));
}
/*API*/VOID		WINAPI	Mesh_unbind_mesh/*模型_取消绑定模型*/(IMeshPtr* _self/*渲染组*/, IMeshPtr* mesh/*模型*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->UnbindMesh(GetIPtr(mesh, Mesh, IMeshPtr));
}
/*API*/VOID		WINAPI	Mesh_bind_draw/*模型_绑定渲染队列*/(IMeshPtr* _self/*场景对象*/, IUIDrawMgrPtr* drawPtr/*组合*/, LPCSTR szBoneName/*骨骼名字*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->BindDraw(szBoneName, GetIPtr(drawPtr, UIDrawMgr, IUIDrawMgrPtr));
}
/*API*/VOID		WINAPI	Mesh_replace_image/*模型_绑定图片*/(IMeshPtr* _self/*场景对象*/, LPCSTR szImageSrc/*原图片文件路径*/, LPCSTR szImageReplace/*替换图片文件路径*/)
{
	if(!szImageSrc||!szImageReplace)return;
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->ReplaceImage(szImageSrc, szImageReplace);
}
/*API*/VOID			WINAPI	Mesh_Getspecular/*模型_设置高光色*/(IMeshPtr* _self/*子模型ID*/,  color4* c/*颜色*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	*c	=	gg->GetSpecularColor();
}
/*API*/VOID			WINAPI	Mesh_Getselfluminous/*模型_设置自发光色*/(IMeshPtr* _self/*子模型ID*/,  color4* c/*颜色*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	*c	=	gg->GetEmissiveColor();
}
/*API*/VOID		WINAPI	Mesh_Setparent/*模型_设置父模型*/(IMeshPtr* _self/*渲染组*/, IMeshPtr* mesh/*模型*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	if(mesh)
		gg->SetParent(GetIPtr(mesh,Mesh,IMeshPtr));
	else
		gg->SetParent(0);
}
/*API*/BOOL		WINAPI	Mesh_Getfollow_camera/*模型_是否跟着镜头移动*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return false;
	return gg->m_bFollowCamera;
}
/*API*/VOID		WINAPI	Mesh_Setfollow_camera/*模型_跟着镜头移动*/(IMeshPtr* _self/*渲染组ID*/, BOOL b/*是否显示*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->m_bFollowCamera = b;
}
/*API*/VOID		WINAPI	Mesh_lookat/*模型_设置Y方向面向目标*/(IMeshPtr* _self/*场景对象*/, const float3* m/*目标*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	float3 up(0,0,1);
	matrix4x4 mat;
	Phantom::matrixLookAtRH(&mat, &gg->GetPosition(), m, &up);
	matrixInverse(&mat, 0, &mat);
	mat.SetPosition(gg->GetPosition());
	gg->SetWorldMatrix(mat);
}
/*API*/VOID		WINAPI	Mesh_clearParent/*模型_清除父模型*/(IMeshPtr* _self/*渲染组*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetParent(0);
}
/*API*/VOID		WINAPI	Mesh_Getparent/*模型_取消父模型*/(IMeshPtr* _self/*渲染组*/, IMeshPtr* _return/*模型*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	Mesh* p = gg->GetParent();
	if(p)
		SetIPtr(_return,p,Mesh,IMeshPtr);
}

/*API*/BlendMode	WINAPI	Mesh_Getsrcblend/*模型_设置源混合*/(IMeshPtr* _self/*子模型ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return BlendMode_ZERO;
	return (BlendMode)gg->GetSrcBlendMode();
}
/*API*/BlendMode	WINAPI	Mesh_Getdestblend/*模型_设置目标混合*/(IMeshPtr* _self/*子模型ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return BlendMode_ZERO;
	return (BlendMode)gg->GetSrcBlendMode();
}
/*API*/VOID			WINAPI	Mesh_Setspecular/*模型_设置高光色*/(IMeshPtr* _self/*子模型ID*/,  const color4* c/*颜色*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetSpecularColor(*c);
}
/*API*/VOID			WINAPI	Mesh_Setselfluminous/*模型_设置自发光色*/(IMeshPtr* _self/*子模型ID*/,  const color4* c/*颜色*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetEmissiveColor(*c);
}
/*API*/VOID			WINAPI	Mesh_Setsrcblend/*模型_设置源混合*/(IMeshPtr* _self/*子模型ID*/,  BlendMode blend/*值*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetSrcBlendMode(blend);
}
/*API*/VOID			WINAPI	Mesh_Setdestblend/*模型_设置目标混合*/(IMeshPtr* _self/*子模型ID*/,  BlendMode blend/*值*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetDestBlendMode(blend);
}


/*API*/VOID	WINAPI	Mesh_SetObjectRegionEnable/*模型_是否计算包围盒*/(IMeshPtr* _self/*渲染组ID*/, BOOL b/*开关*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetObjectRegionEnable(b == TRUE);
}

/*API*/VOID		WINAPI	Mesh_Setactionby/*模型_设置当前动作*/(IMeshPtr* _self/*渲染组ID*/, LPCSTR szName/*动作名称*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg|| !szName)return;
	gg->SetCurrentAction(szName, gg->GetLoop());
}
/*API*/LPCSTR	WINAPI	Mesh_Getactionby/*模型_取当前动作*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return 0;
	MeshActionRange* r = gg->GetCurrentAction();
	if(!r)
		return "";
	return r->GetName();
}

/*API*/VOID	WINAPI	Mesh_Setaction/*模型_设置当前动作*/(IMeshPtr* _self/*渲染组ID*/, ActionType type/*动作ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetActionType(type, gg->GetLoop(),0);// SetCurrentAction(actionID, gg->GetLoop());
}

//actionIDMap.editor中定义
/*API*/ActionType	WINAPI	Mesh_Getaction/*模型_取得当前动作*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return ActionType_None;
	return (ActionType)gg->GetCurrentAction()->m_actionType;
}


/*API*/VOID	WINAPI	Mesh_Setdefaultaction/*模型_设置默认动作*/(IMeshPtr* _self/*渲染组ID*/, INT actionID/*动作ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetDefaultActionID(actionID);
}

//actionIDMap.editor中定义
/*API*/INT	WINAPI	Mesh_Getdefaultaction/*模型_取得默认动作*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return 0;
	return gg->GetDefaultActionID();
}


//
/*API*/INT	WINAPI	Mesh_searchboneid/*模型_查找骨骼ID*/(IMeshPtr* _self/*渲染组ID*/, LPCSTR szName/*骨骼名*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg|| !szName)return 0;
	return gg->GetBoneIDFromName( szName );
}


/*API*/INT	WINAPI	Mesh_searchboneid_like/*模型_查找类似骨骼ID*/(IMeshPtr* _self/*渲染组ID*/, LPCSTR szName/*类似名*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg|| !szName)return 0;
	return gg->GetBoneIDLike( szName );
}


/*API*/LPCSTR	WINAPI	Mesh_bonename_at/*模型_返回骨骼名称*/(IMeshPtr* _self/*渲染组ID*/, INT actionID/*动作ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return "";
	return gg->GetBoneName( actionID );
}


/*API*/INT	WINAPI	Mesh_bonecount/*模型_取得骨骼数量*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return 0;
	return gg->GetBoneCount();
}


//
/*API*/VOID	WINAPI	Mesh_bonemat_at/*模型_取得骨骼矩阵*/(IMeshPtr* _self/*渲染组ID*/, INT boneID/*骨骼ID*/, matrix4x4* ret/*返回*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	const matrix4x4* p	=	gg->GetBoneMatrix(boneID);
	if(!p)return;
	*(matrix4x4*)ret		=	*p;
}


/*API*/VOID	WINAPI	Mesh_Setspeed/*模型_设置动作速度*/(IMeshPtr* _self/*渲染组ID*/, float fSpeed/*速度*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetActionSpeed(fSpeed);
}

//动作文件在actionIDMap.editor中定义，记事本打开
/*API*/float	WINAPI	Mesh_Getspeed/*模型_取得动作速度*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return 0;
	return gg->GetActionSpeed();
}

//
/*API*/BOOL	WINAPI	Mesh_isunwalkable/*模型_是否禁止移动*/(IMeshPtr* _self/*渲染组ID*/, const float3* pos/*当前位置*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return false;
	return gg->IsPointDisableMove(*pos, NULL);
}

//根据disable_开头的模型，检测是否可以移动到此点
//
/*API*/float	WINAPI	Mesh_height/*模型_根据碰撞盒取得高度*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return 0;
	return gg->GetDataPtr()->m_idleHeight*gg->GetDataPtr()->m_fBaseScale;
}
/*API*/VOID		WINAPI	Mesh_aabox/*模型_返回碰撞盒子*/(IMeshPtr* _self/*渲染组ID*/, AABox* _return)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	*_return = gg->GetObjectRegion().aabox;
}
/*API*/BOOL		WINAPI	Mesh_ray_region_dir/*模型_碰撞检测_是否可走*/(IMeshPtr* _self/*渲染组ID*/, const float3* orig/*原点*/, const float3* dir/*方向*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return 0;
	GetCameraPtr()->GetIntersectPtr()->begin(*orig, *dir, GetCameraPtr()->GetZFar());
	return gg->PickIsMoveRegion(GetCameraPtr()->GetIntersectPtr());
}
/*API*/BOOL		WINAPI	Mesh_ray_dir/*模型_碰撞检测_仅与碰撞子模型*/(IMeshPtr* _self/*渲染组ID*/, const float3* orig/*原点*/, const float3* dir/*方向*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return 0;
	GetCameraPtr()->GetIntersectPtr()->begin(*orig, *dir, GetCameraPtr()->GetZFar());
	return gg->PickRay(GetCameraPtr()->GetIntersectPtr(), Mesh::MeshPick_Full);
}
///*API*/BOOL		WINAPI	Mesh_ray_all_dir/*模型_碰撞检测_自动*/(IMeshPtr* _self/*渲染组ID*/, const float3* orig/*原点*/, const float3* dir/*方向*/)
//{
//	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
//	if(!gg)return 0;
//	GetCameraPtr()->GetIntersectPtr()->begin(*orig, *dir, GetCameraPtr()->GetZFar());
//	return gg->PickFull(GetCameraPtr()->GetIntersectPtr());
//}

//在3dsmax导出时已collision_开头的子模型都属于碰撞模型,必须初始化碰撞检测,Intersect_Begin
/*API*/INT	WINAPI	Mesh_ray_region/*模型_碰撞检测_是否可走*/(IMeshPtr* _self/*渲染组ID*/, const Pixel* p)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return 0;
	GetCameraPtr()->GetIntersectPtr()->begin(GetCameraPtr(), p->x, p->y);
	return gg->PickIsMoveRegion(GetCameraPtr()->GetIntersectPtr());
}

//点击可以行走的区域，如果点击的是disable_开头的模型则返回-1，如果没有点击到则返回0, 如果点击到则返回1,必须初始化碰撞检测后才能调用,Intersect_Begin
/*API*/BOOL	WINAPI	Mesh_ray/*模型_碰撞检测_仅与碰撞盒*/(IMeshPtr* _self/*渲染组ID*/, const Pixel* p)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return false;
	GetCameraPtr()->GetIntersectPtr()->begin(GetCameraPtr(), p->x, p->y);
	return gg->PickRay(GetCameraPtr()->GetIntersectPtr(), Mesh::MeshPick_Full);
}

/*API*/BOOL	WINAPI	Mesh_Getplay/*模型_是否播放*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return false;
	return gg->IsPlay();
}


/*API*/VOID	WINAPI	Mesh_Setplay/*模型_播放*/(IMeshPtr* _self/*渲染组ID*/, BOOL b/*播放与否*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->Play( b == TRUE );
}


/*API*/BOOL	WINAPI	Mesh_Getpause/*模型_是否暂停*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return false;
	return gg->IsPause();
}


/*API*/VOID	WINAPI	Mesh_Setpause/*模型_暂停*/(IMeshPtr* _self/*渲染组ID*/, BOOL b/*暂停与否*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->Pause( b == TRUE );
}
///*API*/LPCSTR	WINAPI	Mesh_name/*模型_取得名字*/(IMeshPtr* _self/*渲染组ID*/)
//{
//	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
//	if(!gg)return "";
//	return gg->GetName();
//}
//返回ggdID
//
/*API*/INT	WINAPI	Mesh_submeshcount/*模型_子模型数量*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return 0;
	return gg->GetSubmeshCount();
}


/*API*/VOID	WINAPI	Mesh_submesh_at/*模型_取得子模型ID*/(IMeshPtr* _self/*渲染组ID*/, INT index/*子模型索引*/, ISubMeshPtr* _return/*ret*/)//, ISubMeshPtr* ret/*返回子模型ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	SetIPtr(_return, gg->GetSubmeshPtr( index ), SubMesh, ISubMeshPtr);
}
//
////返回子模型ID(grID)
//
/*API*/float		WINAPI	Mesh_Getopacity/*模型_设置所有子模型透明值*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return 0;
	return gg->m_fAlphaValue;
}
//设置透明混合模式,如果子模型索引是-1则设置所有
/*API*/VOID	WINAPI	Mesh_Setopacity/*模型_设置子模型透明值*/(IMeshPtr* _self/*渲染组ID*/, float fAlpha/*透明值*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	color4 c = gg->GetDiffuseColor();
	c.a=clamp(fAlpha, 0, 1);
	gg->SetDiffuseColor(c);
}

/*API*/BOOL		WINAPI	Mesh_Getenabled/*模型_是否有效*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return false;
	return gg->m_bEnabled;
}
/*API*/VOID		WINAPI	Mesh_Setenabled/*模型_有效*/(IMeshPtr* _self/*渲染组ID*/, BOOL b/*是否有效*/)//设置无效之后将不会被点击和选取
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->m_bEnabled = b;
}

//所有设置都有效并进行渲染(包括矩阵数据，透明设置等等)，如果压入了渲染堆栈则不用调用此项
/*API*/BOOL	WINAPI	Mesh_Getdisplay/*模型_是否显示*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return false;
	return gg->IsVisible();
}


/*API*/VOID	WINAPI	Mesh_Setdisplay/*模型_是否显示*/(IMeshPtr* _self/*渲染组ID*/, BOOL b/*是否显示*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetVisible( b == TRUE );
}

//需要每次循环都调用,内部自动使用RenderQueue
/*API*/VOID	WINAPI	Mesh_Getworldmat/*模型_取得矩阵*/(IMeshPtr* _self/*渲染组ID*/, matrix4x4* ret/*矩阵返回*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	*(matrix4x4*)ret		=	gg->GetWorldMatrix();
}


/*API*/VOID	WINAPI	Mesh_Setworldmat/*模型_设置矩阵*/(IMeshPtr* _self/*渲染组ID*/, const matrix4x4* m/*矩阵*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	gg->SetWorldMatrix(*(matrix4x4*)m);
}

/*API*/LPCSTR	WINAPI	Mesh_getfilepath/*模型_取得名称*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return "";
	return gg->GetName();
}

/*API*/VOID 	WINAPI	Mesh_clone/*模型_克隆一个相同渲染组*/(IMeshPtr* _self/*渲染组ID*/, IMeshPtr* _return/*ret*/)//, IMeshPtr* retPtr/*返回渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(!gg)return;
	Mesh* newgg = static_cast<Mesh*>(gg->Clone());
	/*CreateIPtr*/CreateIPtr(_return, newgg, Mesh, IMeshPtr);
	g_luamanager.Push(gg->GetPtrID());
}

/*API*/VOID			WINAPI	Global_mesh_load/*系统_模型_载入*/(LPCSTR szFile/*rpgmesh文件路径*/, BOOL bAutoRender/*是否自动添加到渲染队列*/, IMeshPtr* _return/*取渲染组ID*/)
{
	if(!szFile)return;
	MeshDataPtr data;
	if(!g_manager.LoadMesh(data, szFile))
		return;
	Mesh* group	=	data->CreateRuntime();
	if(!group)
		return;
	CreateIPtr(_return, group, Mesh, IMeshPtr);
	g_luamanager.Push(group->GetPtrID());
	if(bAutoRender){
		g_luamanager.PushNode(group);
	}
}
/*API*/VOID			WINAPI	Global_dlg_load_vars/*全局_载入对话框仅一次并且读取变量表*/(LPCSTR szFileName/*文件名*/, IDialogPtr* _return/*返回*/)
{
}
/*API*/VOID			WINAPI	Global_dlg_loadnew_vars/*全局_载入新的对话框并且读取变量表*/(LPCSTR szFileName/*文件名*/, IDialogPtr* _return/*返回*/)
{
}

/*API*/VOID			WINAPI	Global_mesh_autorender/*全局_模型_设置自动渲染*/(IMeshPtr* mesh/*渲染组ID*/, BOOL bRender/*是否自动渲染*/)
{
	Mesh* gg	=	GetIPtr(mesh, Mesh, IMeshPtr);
	if(gg)
		g_luamanager.PushNode(gg);
}
/*API*/VOID			WINAPI	Global_mesh_delete/*系统_模型_释放*/(IMeshPtr* _self/*渲染组ID*/)
{
	Mesh* gg	=	GetIPtr(_self, Mesh, IMeshPtr);
	if(gg)
	{
		g_luamanager.Pop(gg->GetPtrID());
		ReleaseIPtr(_self, Mesh, IMeshPtr);
	}
}

/*API*/INT		WINAPI	Texture_getwidth/*纹理_取得宽度*/(ITexturePtr* _self/*纹理ID*/)
{
	Texture* tex	=	GetIPtr(_self, Texture, ITexturePtr);
	if(!tex)return 0;
	return tex->GetWidth();
}
/*API*/INT		WINAPI	Texture_getheight/*纹理_取得高度*/(ITexturePtr* _self/*纹理ID*/)
{
	Texture* tex	=	GetIPtr(_self, Texture, ITexturePtr);
	if(!tex)return 0;
	return tex->GetHeight();
}
//
/*API*/BOOL		WINAPI	Texture_commit/*纹理_更新到设备*/(ITexturePtr* _self/*纹理ID*/, TextureStage samplerIndex/*纹理*/)
{
	Texture* tex	=	GetIPtr(_self, Texture, ITexturePtr);
	if(!tex)return FALSE;
	g_manager.SetTexture(tex, (TextureIndex)samplerIndex);
	return true;
}

/*API*/VOID			WINAPI	Global_texture_load/*系统_纹理_载入*/(LPCSTR szFile/*文件名*/, ITexturePtr* _return/*纹理ID*/)
{
	if(!szFile)return;
	TexturePtr ret;
	if(!g_manager.LoadTexture(ret, szFile))
		return;
	Texture* tex = ret.GetPtr();
	safe_addref(tex);
	CreateIPtr(_return, tex, Texture, ITexturePtr);
	g_luamanager.Push(tex->GetPtrID());
}
/*API*/VOID			WINAPI	Global_texture_delete/*系统_纹理_释放*/(ITexturePtr* ptr/*纹理ID*/)
{
	PtrID id;
	id.id		=	ptr->idx;
	id.index	=	ptr->ptr;
	g_luamanager.Pop(id);
	ReleaseIPtr(ptr, Texture, ITexturePtr);
}

/*API*/LPCSTR	WINAPI	Texture_getfilepath/*纹理_取得纹理路径*/(ITexturePtr* _self/*纹理ID*/)
{
	Texture* tex	=	GetIPtr(_self, Texture, ITexturePtr);
	if(!tex)return "";
	return tex->GetFileName();
}


/*API*/BOOL		WINAPI	Camera_get2d_from3d/*镜头_3D坐标转换成屏幕坐标*/(Pixel* point/*返回屏幕坐标*/, const float3* pos/*三维坐标*/)
{
	return GetCameraPtr()->Get2DFrom3D(*(Pixel*)point, *pos);
}
/*API*/VOID		WINAPI	Camera_get3d_from2d/*镜头_屏幕坐标转换成3D坐标*/(float3* orig/*射线原点*/, float3* dir/*射线方向*/, const Pixel* point/*屏幕坐标*/)
{
	GetCameraPtr()->Get3DFrom2D(*orig, *dir, *(Pixel*)point);
}
/*API*/BOOL		WINAPI	Camera_isdisplay/*镜头_判断是否可见*/(const float3* pos/*位置*/, float fRadius/*物体半径*/, float fMaxDistance/*最远距离*/)
{
	return GetCameraPtr()->CheckObjectVisible(*pos, fRadius, fMaxDistance, 0);
}
/*API*/VOID		WINAPI	Camera_orthoproj/*镜头_设置为2D投影视图*/(int w/*宽度*/,int h/*高度*/,float znear/*最近可见*/,float zfar/*最远可见*/)
{
	GetCameraPtr()->SetOrthoProjection(w, h, znear, zfar);
}
/*API*/VOID		WINAPI	Camera_perspectiveproj/*镜头_设置为3D投影视图*/(float fov/*广角_弧度*/, int w/*宽度*/,int h/*高度*/,float znear/*最近可见*/,float zfar/*最远可见*/)
{
	GetCameraPtr()->SetPerspectiveProjection(fov, w, h, znear, zfar);
}
/*API*/BOOL		WINAPI	Camera_isperspective/*镜头_是否3D投影视图*/()
{
	return GetCameraPtr()->IsPerspectiveProjection();
}
/*API*/BOOL		WINAPI	Camera_isortho/*镜头_是否2D投影视图*/()
{
	return GetCameraPtr()->IsOrthoProjection();
}
/*API*/VOID		WINAPI	Camera_Setpos/*镜头_设置位置*/(const float3* pos/*位置*/)//设置镜头的位置
{
	if(!pos)return;
	float3 dir = GetCameraPtr()->GetEye() - GetCameraPtr()->GetTarget();
	float3 eye = *pos;
	float3 target = eye-dir;
	GetCameraPtr()->SetViewMatrix(&eye, &target);
}
/*API*/VOID		WINAPI	Camera_Getpos/*镜头_取观位置*/(float3* _return/*ret*/)//设置镜头的位置
{
	*_return  = GetCameraPtr()->GetEye();
}
/*API*/VOID		WINAPI	Camera_nextto/*镜头_加入下一个位置*/(float3* pos/*位置*/, float fMoveSpeed/*移动速度*/)//设置镜头的位置移动
{
	if(!pos)return;
	CameraAni ani;
	memset(&ani, 0, sizeof(CameraAni));
	ani.type = ani.CAT_Move;
	if(fMoveSpeed<0.1)fMoveSpeed=0.1;
	ani.v.setXYZW(pos->x, pos->y, pos->z, fMoveSpeed);
	GetCameraPtr()->m_nexts.push_back(ani);
}
/*API*/VOID		WINAPI	Camera_nextrot/*镜头_加入下一个转动*/(float rotZ/*Z轴转动*/, float rotX/*X轴转动*/, float fRotSpeed/*转动速度角度*/)//设置镜头的转动
{
	CameraAni ani;
	memset(&ani, 0, sizeof(CameraAni));
	ani.type = ani.CAT_Rot;
	if(fRotSpeed<0.1)fRotSpeed=0.1;
	float3& rot = GetCameraPtr()->GetCameraData().vRotate;
	ani.v.setXYZW(Radians(rotZ)+rot.z,Radians(rotX)+rot.x,0,Radians(fRotSpeed));
	GetCameraPtr()->m_nexts.push_back(ani);
}
/*API*/VOID		WINAPI	Camera_nextdist/*镜头_加入下一个远近*/(float fDistance/*目标距离*/, float fMoveSpeed/*远近移动速度*/)//设置镜头的远近
{
	CameraAni ani;
	memset(&ani, 0, sizeof(CameraAni));
	ani.type = ani.CAT_Dist;
	ani.v.setXYZW(fDistance,0,0,fMoveSpeed);
	if(fMoveSpeed<0.1)fMoveSpeed=0.1;
	GetCameraPtr()->m_nexts.push_back(ani);
}
/*API*/VOID		WINAPI	Camera_nextdither/*镜头_加入下一个剧烈抖动*/(float offset/*抖动范围*/, float fMaxOffsetTime/*抖动的时间*/)//设置镜头的抖动
{
	CameraAni ani;
	memset(&ani, 0, sizeof(CameraAni));
	ani.type = ani.CAT_Dither;
	ani.v.setXYZW(offset,0,0,fMaxOffsetTime);
	GetCameraPtr()->m_nexts.push_back(ani);
}
/*API*/VOID		WINAPI	Camera_play/*镜头_开始播放镜头动作*/(LPCSTR szLuaFunc/*脚本函数*/, INT nParam/*参数*/)//设置镜头的远近,事件[szLuaFunc()--镜头动作播放完毕]
{
	if(!szLuaFunc)szLuaFunc="";
	GetCameraPtr()->m_funcPlayOver = szLuaFunc;
	GetCameraPtr()->m_funcParam = nParam;
	GetCameraPtr()->Play();
}
/*API*/VOID		WINAPI	Camera_stop/*镜头_停止播放镜头动作*/()
{
	GetCameraPtr()->Stop();
}

///*API*/BOOL		WINAPI	Camera_boardworld/*镜头_取得蒙板矩阵信息*/(MatrixBoard nBoard/*BOARD_开头常数*/, matrix4x4* ret/*返回矩阵*/)
//{
//	const matrix4x4* mat	=	GetCameraPtr()->GetBoardWorld(nBoard);
//	if(!mat)return false;
//	*(matrix4x4*)ret	=	*mat;
//	return true;
//}
/*API*/VOID		WINAPI	Camera_Setviewmat/*镜头_强制设置观察矩阵*/(const matrix4x4* view/*观察矩阵*/)
{
	GetCameraPtr()->SetViewMatrix(*(matrix4x4*)view);
}
/*API*/VOID		WINAPI	Camera_Setprojmat/*镜头_强制设置投影矩阵*/(const matrix4x4* view/*投影矩阵*/)
{
	GetCameraPtr()->SetProjectionMatrix(*(matrix4x4*)view);
}
/*API*/VOID		WINAPI	Camera_Seteye/*镜头_设置观察点*/(const float3* eye/*参数1*/)
{
	GetCameraPtr()->SetEye(*eye);
}
/*API*/VOID		WINAPI	Camera_Settarget/*镜头_设置目标点*/(const float3* target/*参数1*/)
{
	GetCameraPtr()->SetTarget(*target);
}
/*API*/float	WINAPI	Camera_Getfov/*镜头_取得广角*/()
{
#ifdef WIN32_OS
	return Degress(GetCameraPtr()->GetFov()*2.0f);
#endif
	return Degress(GetCameraPtr()->GetFov());
}
/*API*/INT		WINAPI	Camera_Getwidth/*镜头_取得宽度*/()
{
	return GetCameraPtr()->GetWidth();
}
/*API*/INT		WINAPI	Camera_Getheight/*镜头_取得高度*/()
{
	return GetCameraPtr()->GetHeight();
}
/*API*/VOID		WINAPI	Camera_Setfov/*镜头_设置广角*/(float fValue/*值*/)
{
#ifdef WIN32_OS
	fValue *= 0.5f;
#endif
	GetCameraPtr()->SetFov(Radians(fValue));
}
/*API*/VOID		WINAPI	Camera_Setenabled/*镜头_设置屏幕控制有效性*/(BOOL bEnabled/*参数1*/)
{
	GetCameraPtr()->m_bEnabled = bEnabled;
}
/*API*/BOOL		WINAPI	Camera_Getenabled/*镜头_取视屏幕控制有效性*/()
{
	return GetCameraPtr()->m_bEnabled;
}

/*API*/VOID		WINAPI	Camera_Setznearest/*镜头_设置最近可见距离*/(float fValue/*值*/)
{
	GetCameraPtr()->SetZNearest(fValue);
}
/*API*/VOID		WINAPI	Camera_Setzfar/*镜头_设置取最远可见距离*/(float fValue/*值*/)
{
	GetCameraPtr()->SetZFar(fValue);
}
/*API*/VOID		WINAPI	Camera_Setwidth/*镜头_设置取宽度*/(INT fValue/*值*/)
{
	GetCameraPtr()->SetWidth(fValue);
}
/*API*/VOID		WINAPI	Camera_Setheight/*镜头_设置取高度*/(INT fValue/*值*/)
{
	GetCameraPtr()->SetHeight(fValue);
}


/*API*/float	WINAPI	Camera_Getznearest/*镜头_最近可见距离*/()
{
	return GetCameraPtr()->GetZNearest();
}
/*API*/float	WINAPI	Camera_Getzfar/*镜头_取得最远可见距离*/()
{
	return GetCameraPtr()->GetZFar();
}
/*API*/VOID		WINAPI	Camera_Getviewmat/*镜头_取得观察矩阵*/(matrix4x4* _return)
{
	*_return	=	GetCameraPtr()->GetViewMatrix();
}
/*API*/VOID		WINAPI	Camera_Getprojmat/*镜头_取得投影矩阵*/(matrix4x4* _return)
{
	*_return	=	GetCameraPtr()->GetProjMatrix();
}
/*API*/VOID		WINAPI	Camera_Geteye/*镜头_取得观察点*/(float3* _return)//float3* eye/*返回*/)
{
	*_return	=	GetCameraPtr()->GetEye();
}
/*API*/VOID		WINAPI	Camera_Gettarget/*镜头_取得目标点*/(float3* _return)//float3* target/*返回*/)
{
	*_return	=	GetCameraPtr()->GetTarget();
}
/*API*/VOID		WINAPI	Camera_Setrot/*镜头_设置转动率*/(const float2* rot)
{
	GetCameraPtr()->SetRotationXYEnable(true, true);
	GetCameraPtr()->SetCameraRotate(float3(Radians(rot->x), 0, Radians(rot->y)), false);
}
/*API*/VOID		WINAPI	Camera_Getrot/*镜头_取得转动率*/(float2* _return)
{
	const float3& rot = GetCameraPtr()->GetCameraRotate();
	_return->setxy(Degress(rot.x), Degress(rot.z));
}

/*API*/VOID		WINAPI	Camera_Setlookat/*镜头_设置视点*/(const float3* vLookat/*参数1*/)
{
	if(!vLookat)return;
	float3 dir = GetCameraPtr()->GetEye() - GetCameraPtr()->GetTarget();
	float3 eye = dir + *vLookat;
	GetCameraPtr()->SetViewMatrix(&eye, vLookat);
}
/*API*/VOID		WINAPI	Camera_Getlookat/*镜头_取视点*/(float3* _return/*ret*/)
{
	*_return = GetCameraPtr()->GetTarget();
}
/*API*/VOID		WINAPI	Camera_Setdistance/*镜头_设置视点*/(float fLength/*参数1*/)
{
	GetCameraPtr()->SetDistance(fLength);
}
/*API*/float	WINAPI	Camera_Getdistance/*镜头_取视点*/()
{
	return GetCameraPtr()->GetDistance();
}
/*API*/VOID		WINAPI	Camera_Setdistance_min/*镜头_设置视距离最近*/(float fLength/*参数1*/)
{
	GetCameraPtr()->SetMinWheelDistance(fLength);
}
/*API*/float	WINAPI	Camera_Getdistance_min/*镜头_取视距离最近*/()
{
	return GetCameraPtr()->GetMinWheelDistance();
}
/*API*/VOID		WINAPI	Camera_Setdistance_max/*镜头_设置视距离最远*/(float fLength/*参数1*/)
{
	GetCameraPtr()->SetMaxWheelDistance(fLength);
}
/*API*/float	WINAPI	Camera_Getdistance_max/*镜头_取视距离最远*/()
{
	return GetCameraPtr()->GetMaxWheelDistance();
}

/*API*/VOID		WINAPI	Camera_Setintersect_eye/*镜头_设置有效*/(BOOL bEnabled/*参数1*/)
{
	GetCameraPtr()->m_rayCamera = bEnabled;
}
/*API*/BOOL		WINAPI	Camera_Getintersect_eye/*镜头_取有效*/()
{
	return GetCameraPtr()->m_rayCamera;
}
/*API*/VOID		WINAPI	Camera_Setoffset_enable/*镜头_设置自动移动速度有效*/(BOOL bEnable/*参数1*/)
{
	GetCameraPtr()->m_bOffsetEnable = bEnable;
}
/*API*/BOOL		WINAPI	Camera_Getoffset_enable/*镜头_取观自动移动速度有效*/()
{
	return GetCameraPtr()->m_bOffsetEnable;
}

/*API*/VOID		WINAPI	Camera_Setoffset_down/*镜头_设置自动移动速度降速*/(float fSpeed/*参数1*/)
{
	GetCameraPtr()->m_fOffsetDown = clamp(fSpeed, 0.01f, 10.0f);
}
/*API*/float	WINAPI	Camera_Getoffset_down/*镜头_取观自动移动速度降速*/()
{
	return GetCameraPtr()->m_fOffsetDown;
}


/*API*/BOOL		WINAPI	Camera_pointin/*镜头_检测点是否可见*/(const float3* p/*点*/)
{
	return GetCameraPtr()->IsPointVisible(*p);
}
/*API*/BOOL		WINAPI	Camera_spherein/*镜头_检测圆是否可见*/(const float3* p/*点*/, float fRadius/*半径*/)
{
	return GetCameraPtr()->IsSphereVisible(*p, fRadius);
}
/*API*/BOOL		WINAPI	Camera_aaboxin/*镜头_检测盒子是否可见*/(const AABox* p/*点*/)
{
	return GetCameraPtr()->IsAABoxVisible(*(AABox*)p);
}
/*API*/BOOL		WINAPI	Camera_planein/*镜头_检测面是否可见*/(const float3* p1/*点1*/, const float3* p2/*点2*/, const float3* p3/*点3*/, const float3* p4/*点4*/)
{
	return GetCameraPtr()->IsPlaneVisible(*p1, *p2, *p3, *p4);
}

/*API*/VOID		WINAPI	Camera_Getdata/*镜头_保存数据*/(CameraData* _return)
{
	CameraDataLocal data;
	GetCameraPtr()->GetCameraConfig(data);
	*_return	=	data;
}
/*API*/VOID		WINAPI	Camera_Setdata/*镜头_保存数据*/(const CameraData* p/*数据*/)
{
	CameraDataLocal data;
	memcpy(&data, p, sizeof(CameraData));
	GetCameraPtr()->SetCameraConfig(data);
}

/*API*/VOID		WINAPI	Camera_dither/*镜头_设置进行抖动值*/(const float3* offset/*抖动值*/, float fMaxOffsetTime/*抖动的时间*/)
{
	GetCameraPtr()->SetDitherOffset(*offset, fMaxOffsetTime);
}
/*API*/VOID		WINAPI	Camera_Setarea/*镜头_设置目标点限制区域*/(const Rect* rc/*参数1*/)
{
	GetCameraPtr()->SetArea(rc);
}
/*API*/VOID		WINAPI	Camera_Getarea/*镜头_取观目标点限制区域*/(Rect* _return/*ret*/)
{
	*_return = GetCameraPtr()->GetArea();
}


/*API*/VOID			WINAPI	Global_setupwave/*全局_设置音效属性*/(LPCSTR szName/*WAV文件*/, INT nDuplicateCount/*拷贝数量*/)
{
	if(!szName)return;
	char buf[128];
	sprintf(buf, "sounds/%s.wav", szName);
	LogInfo("system->:setupwave(%s)", buf);
	int index =	g_manager.GetMusicFileIndex(buf);
	MultiMusic* mediaPtr = g_manager.GetMusicFilePtrByIndex(index);
	if(!mediaPtr)return;
	mediaPtr->Setup(nDuplicateCount);
}
/*API*/INT			WINAPI	Global_playwave/*全局_播放*/(LPCSTR szName/*WAV_MP3文件*/)
{
	if(!szName)return 0;
	if(g_manager.GetMute())
		return 0;
	char buf[128];
	sprintf(buf, "sounds/%s.wav", szName);
	InitMyFile(buf, true, false);
	int index =	g_manager.GetMusicFileIndex(buf);
	MultiMusic* mediaPtr = g_manager.GetMusicFilePtrByIndex(index);
	if(!mediaPtr){
		LogInfo("system->error:playwave(%s)", buf);
		return 0;
	}
	LogInfo("system->:playwave(%s)", buf);
	SoundID sid;
	PtrID id = mediaPtr->Play(0);
	sid.index	=	id.index;
	sid.rid	=	id.id;
	sid.fileIndex = index;
	//g_luamanager.Push(sid);
	return sid.ToInt();
}
/*API*/INT			WINAPI	Global_playmp3_2/*全局_播放*/(LPCSTR szFile/*WAV_MP3文件*/, INT nLoop/*循环次数*/)
{
	if(!szFile)return 0;
	char buf[128];
	sprintf(buf, "sounds/%s.mp3", szFile);
	InitMyFile(buf, true, false);
	int index =	g_manager.GetMusicFileIndex(buf);
	MultiMusic* mediaPtr = g_manager.GetMusicFilePtrByIndex(index);
	if(!mediaPtr)
	{
		LogInfo("system->error:playmp3_2(%s)", buf);
		return 0;
	}
	LogInfo("system->:playmp3_2(%s)", buf);
	SoundID sid;
	PtrID id = mediaPtr->Play(nLoop);
	sid.index	=	id.index;
	sid.rid	=	id.id;
	sid.fileIndex = index;
	//g_luamanager.Push(sid);
	return sid.ToInt();
}
/*API*/INT		WINAPI	Global_playmp3/*音效系统_播放2D音效*/(LPCSTR szFile/*WAV文件*/)
{
	if(!szFile)return 0;
	char buf[128];
	sprintf(buf, "sounds/%s.mp3", szFile);
	InitMyFile(buf, true, false);
	int index =	g_manager.GetMusicFileIndex(buf);
	MultiMusic* mediaPtr = g_manager.GetMusicFilePtrByIndex(index);
	if(!mediaPtr){
		LogInfo("system->error:playmp3(%s)", buf);
		return 0;
	}
	LogInfo("system->:playmp3(%s)", buf);
	SoundID sid;
	PtrID id = mediaPtr->Play(0);
	sid.index	=	id.index;
	sid.rid	=	id.id;
	sid.fileIndex = index;
	//g_luamanager.Push(sid);
	return sid.ToInt();
}
SoundID	g_bgmSID;
Name	g_bgmName;
void	ResetBGM(){
	g_bgmSID.fileIndex = 0;
	g_bgmSID.index = 0;
	g_bgmSID.rid = 0;
}
/*API*/INT		WINAPI	Global_playbgm/*音效系统_播放2D音效*/(LPCSTR szFile/*WAV文件*/)
{
	return Global_playbgm2(szFile, 1);
}
/*API*/INT		WINAPI	Global_playbgm2/*全局_循环播放背景音乐2*/(LPCSTR szFile/*WAV_MP3文件*/, float fVolume)//必须得是名字比如beep会自动选择sound\\beep.mp3,使用stopsnd来停止播放音乐
{
	Name temp = g_bgmName;
	g_bgmName = szFile;
	if(!szFile)return 0;
	char buf[128];
	sprintf(buf, "sounds/%s.mp3", szFile);
	InitMyFile(buf, true, false);
	int index =	g_manager.GetMusicFileIndex(buf);
	MultiMusic* mediaPtr = g_manager.GetMusicFilePtrByIndex(index);
	{
		MultiMusic* mediaPtr2	=	g_manager.GetMusicFilePtrByIndex(g_bgmSID.fileIndex);
		if(mediaPtr2){
			if(stricmp(temp.t, szFile)==0)//mediaPtr==mediaPtr2)//&&g_bgmSID.fileIndex==index)
				return g_bgmSID.ToInt();
			PtrID id;
			id.id = g_bgmSID.rid;
			id.index = g_bgmSID.index;
			LogInfo("system->:stopbgm(%d)", id.id);
			mediaPtr2->Stop(id);
			ResetBGM();
		}
	}
	LogInfo("system->:playbgm2(%s,%d)", buf, g_bgmSID.rid);
	if(!mediaPtr){
		return 0;
	}
	SoundID sid;
	PtrID id = mediaPtr->Play(-1, fVolume);
	sid.index	=	id.index;
	sid.rid	=	id.id;
	sid.fileIndex = index;
	//g_luamanager.Push(sid);
	g_bgmSID = sid;
	LogInfo("system->:playbgm(%s,%d)", buf, g_bgmSID.rid);
	return sid.ToInt();
}
/*API*/INT		WINAPI	Global_playsnd/*音效2D_播放2*/(LPCSTR szName/*WAV文件*/, float fVolume, float fPan, float fRate)//可以直接设置名字，比如sound\\beep.wav
{
	if(!szName)return 0;
	if(g_manager.GetMute())
		return 0;
	int index =	g_manager.GetMusicFileIndex(szName);
	InitMyFile(szName, true, false);
	MultiMusic* mediaPtr = g_manager.GetMusicFilePtrByIndex(index);
	if(!mediaPtr)return 0;
	SoundID sid;
	PtrID id = mediaPtr->Play(0, fVolume, fPan, fRate);
	sid.index	=	id.index;
	sid.rid	=	id.id;
	sid.fileIndex = index;
	//g_luamanager.Push(sid);
	return sid.ToInt();
}
//
/*API*/VOID			WINAPI	Global_stopsndall/*全局_停止所有音乐*/()
{
	g_manager.StopAllMusic();
	ResetBGM();
}
/*API*/VOID		WINAPI	Global_stopsnd/*音效系统_停止2D音效*/(INT s2d/*音效2D*/)
{
	SoundID sid(s2d);
	if(sid.rid<=0)return;
	MultiMusic* mediaPtr	=	g_manager.GetMusicFilePtrByIndex(sid.fileIndex);
	if(!mediaPtr)return;
	if(s2d==g_bgmSID.ToInt())
		ResetBGM();
	PtrID id;
	id.id = sid.rid;
	id.index = sid.index;
	mediaPtr->Stop(id);
	//g_luamanager.Pop(sid);
}
/*API*/VOID			WINAPI	Global_setsndvolume/*全局_设置音量*/(INT s2d/*音效2D*/, float fVolume/*音量*/)
{
	SoundID sid(s2d);
	if(sid.rid<=0)return;
	MultiMusic* mediaPtr	=	g_manager.GetMusicFilePtrByIndex(sid.fileIndex);
	if(!mediaPtr)return;
	PtrID id;
	id.id = sid.rid;
	id.index = sid.index;
	mediaPtr->SetVolumePan(id, fVolume, 0);
}

/*API*/VOID			WINAPI	Global_setsndrate/*全局_设置频率*/(INT s2d/*音效2D*/, float fRate/*速度*/)
{
	SoundID sid(s2d);
	if(sid.rid<=0)return;
	MultiMusic* mediaPtr	=	g_manager.GetMusicFilePtrByIndex(sid.fileIndex);
	if(!mediaPtr)return;
	PtrID id;
	id.id = sid.rid;
	id.index = sid.index;
	mediaPtr->SetPlaySpeed(id, fRate);
}

/*API*/VOID		WINAPI	Global_setsndvolumepan/*音效系统_设置2D音量*/(INT s2d/*音效2D*/, float fVolume/*音量*/, float fPan)
{
	SoundID sid(s2d);
	if(sid.rid<=0)return;
	MultiMusic* mediaPtr	=	g_manager.GetMusicFilePtrByIndex(sid.fileIndex);
	if(!mediaPtr)return;
	PtrID id;
	id.id = sid.rid;
	id.index = sid.index;
	mediaPtr->SetVolumePan(id, fVolume, fPan);
}//音量值是0到1之间，返回false表示此声音已经过期无效

/*API*/VOID			WINAPI	System_Getbackcolor/*系统_设置背景清除色*/(color4* _return/*ret*/)
{
	*_return = g_manager->GetClearColor();
}
/*API*/VOID			WINAPI	System_Setbackcolor/*系统_设置背景清除色*/(const color4* c/*颜色*/)
{
	g_manager->SetClearColor(*c);
}
/*API*/VOID			WINAPI	Global_draw_bind/*全局_绑定渲染函数*/(LPCSTR szLuaFunc/*渲染函数*/)
{
	if(!szLuaFunc)szLuaFunc = "";
	g_luamanager.m_funcRender = szLuaFunc;
}
/*API*/VOID		WINAPI	Global_draw2d_rect/*屏幕_渲染矩形*/(const Rect* _rect/*矩形*/, const color4* c/*颜色*/, BOOL bFill/*是否填充内部*/)
{
	g_manager.SetRenderMode(RenderMode_Color);
	g_manager.SetWorldMatrix2D(0);
	g_manager.DrawRect2D(*_rect, *c, bFill);
}//
/*API*/VOID			WINAPI	Global_draw2d_line/*屏幕_渲染2D线条*/(const Pixel* p1/*屏幕坐标1*/, const Pixel* p2/*屏幕坐标2*/, const color4* c/*颜色*/)//
{
	g_manager.SetRenderMode(RenderMode_Color);
	g_manager.SetWorldMatrix2D(0);
	Pixel p[2];
	p[0].x = p1->x;
	p[0].y = p1->y;
	p[1].x = p2->x;
	p[1].y = p2->y;
	g_manager.DrawLines2D(p, 2, *c, false);
}

/*API*/VOID			WINAPI	Global_draw2d_linec/*屏幕_渲染一个带颜色线条*/(const PixelColor* p1/*屏幕坐标1*/, const PixelColor* p2/*屏幕坐标2*/)//
{
	g_manager.SetRenderMode(RenderMode_Color);
	g_manager.SetWorldMatrix2D(0);
	PixelColor p[2];
	p[0].x = p1->x;
	p[0].y = p1->y;
	p[0].color	=	p1->color;
	p[1].x = p2->x;
	p[1].y = p2->y;
	p[1].color	=	p2->color;
	g_manager.DrawLines2DColor(p, 2, false);
}

/*API*/VOID			WINAPI	Global_draw2d_text/*屏幕_渲染文本*/(Rect* _rect/*屏幕区域*/, LPCSTR szText/*文本*/, const color4* c/*颜色*/)
{
	if(!szText)return;
	g_manager.SetRenderMode(RenderMode_ColorTex1);
	g_manager.SetWorldMatrix2D(0);
	g_manager.DrawTexts(szText, *_rect, 0, 16.f, 0, c);
}
/*API*/VOID			WINAPI	Global_draw2d_text2/*系统_渲染文本2*/(Rect* _rect/*屏幕区域*/, LPCSTR szText/*文本*/, const color4* c/*颜色*/, float fSize/*字体大小*/, DrawTextAlign textFormat/*值*/)
{
	if(!szText)return;
	g_manager.SetRenderMode(RenderMode_ColorTex1);
	g_manager.SetWorldMatrix2D(0);
	g_manager.DrawTexts(szText, *_rect, 0, 16.f * fSize, 0, c, textFormat);
}
/*API*/VOID		WINAPI	Global_draw2d_texture/*屏幕_渲染纹理*/(ITexturePtr* texture/*纹理*/, const Rect* _rect/*屏幕区域*/, const Rect* _textureRect/*纹理区域_默认0*/, const color4* c/*颜色_默认负1*/)
{
	Texture* pt = GetIPtr(texture, Texture, ITexturePtr);
	if(!pt)return;
	g_manager.SetRenderMode(RenderMode_ColorTex1);
	g_manager.SetWorldMatrix2D(0);
	g_manager.Draw2D(pt, *_rect, *_textureRect, c);
}

/*API*/VOID			WINAPI	Global_draw2d_triangle/*系统_渲染屏幕中的三角形*/(ITexturePtr* texture/*纹理*/, const PixelUV* p1/*点1*/, const PixelUV* p2/*点2*/, const PixelUV* p3/*点3*/, BOOL bFill/*是否填充*/)
{
	Texture* pt = GetIPtr(texture, Texture, ITexturePtr);
	if(!pt)return;
	g_manager.SetRenderMode(RenderMode_ColorTex1);
	g_manager.SetWorldMatrix2D(0);
	screenVertex3D v[3] = {
		(float)p1->x,  (float)p1->y, 0.5f, 1.0f, p1->c,  p1->u, p1->v,
		(float)p2->x,  (float)p2->y, 0.5f, 1.0f, p2->c,  p2->u, p2->v,
		(float)p3->x,  (float)p3->y, 0.5f, 1.0f, p3->c,  p3->u, p3->v
	};
	g_manager.DrawTriangle2D(pt, v[0], v[1], v[2], bFill);
}

/*API*/VOID			WINAPI	System_Setuioffset/*系统_设置界面总体位移*/(const float2* off/*值*/)
{
	g_manager.SetUIOffset(off->x, off->y);
}
/*API*/VOID			WINAPI	System_Getuioffset/*系统_取界面总体位移*/(float2* off/*值*/)
{
	*off = g_manager.GetUIOffset();
}
/*API*/VOID			WINAPI	System_Setuiscale/*系统_设置界面总体缩放*/(const float2* s/*值*/)
{
	g_manager.SetUIScale(s->x, s->y);
}
/*API*/VOID			WINAPI	System_Getuiscale/*系统_取界面总体缩放*/(float2* s/*值*/)
{
	*s = g_manager.GetUIScale();
}

/*API*/VOID			WINAPI	System_Setmute/*系统_设置静音*/(BOOL bMute/*是否静音*/)
{
	g_manager.SetMute(bMute);
}
/*API*/BOOL			WINAPI	System_Getmute/*系统_是否静音*/()
{
	return g_manager.GetMute();
}

/*API*/INT			WINAPI	Global_gamewidth/*全局_取得游戏需要宽度*/()
{
	return g_manager.m_gameWidth;//.GetUIDialogSizeW();
}
/*API*/INT			WINAPI	Global_gameheight/*全局_取得游戏需要高度*/()
{
	return g_manager.m_gameHeight;//GetUIDialogSizeH();
}

/*API*/INT		WINAPI	Global_screenwidth/*渲染系统_取得窗体宽度*/()
{
	return g_manager.GetBufferWidth();
}
/*API*/INT		WINAPI	Global_screenheight/*渲染系统_取得窗体高度*/()
{
	return g_manager.GetBufferHeight();
}
//
/*API*/float		WINAPI	Global_elapsedtime/*系统_当前更新时间差*/()//返回当前系统更新时间差
{
	return g_manager.GetElapsedTime();
}
/*API*/double		WINAPI	Global_apptime/*系统_当前时间*/()//返回当前系统时间
{
	return g_manager.GetAppTime();
}
/*API*/VOID		WINAPI	Global_worldmat/*系统_取得世界矩阵*/(matrix4x4* _return)
{
	*_return	=	g_manager.GetWorldMatrix();
}
/*API*/VOID		WINAPI	Global_viewmat/*系统_取得观察矩阵*/(matrix4x4* _return)
{
	*_return	=	g_manager.GetViewMatrix();
}
/*API*/VOID		WINAPI	Global_projmat/*系统_取得投影矩阵*/(matrix4x4* _return)
{
	*_return	=	g_manager.GetProjMatrix();
}

/*API*/VOID		WINAPI	Global_draw3d_line/*系统_渲染线条*/(const float3* _from/*开始点*/, const float3* _to/*结束点*/, const color4* c/*颜色*/)
{
	g_manager.SetRenderMode(RenderMode_Color);
	g_manager.SetWorldMatrix(&matrix4x4::s_identity());
	g_manager.DrawLine(*_from, *_to, c);
}//默认会首先将贴图状态设置为空
/*API*/VOID		WINAPI	Global_draw3d_triangle/*系统_渲染三角形*/(const float3* p1/*点1*/, const float3* p2/*点2*/, const float3* p3/*点3*/, const color4* c/*颜色*/)
{
	g_manager.SetRenderMode(RenderMode_Color);
	g_manager.SetWorldMatrix(&matrix4x4::s_identity());
	g_manager.DrawTriangle(*p1, *p2, *p3, c);
}
/*API*/VOID		WINAPI	Global_draw3d_box/*系统_渲染盒子*/(const AABox* box/*盒子*/, const color4* c/*颜色*/, BOOL bFill/*是否填充*/)
{
	g_manager.SetRenderMode(RenderMode_Color);
	g_manager.SetWorldMatrix(&matrix4x4::s_identity());
	unsigned int colors[8];
	unsigned int temp = *c;
	for(int i=0;i<8;i++)
		colors[i] = temp;
	g_manager.DrawBox(*box, colors, 0, bFill);
}
/*API*/VOID		WINAPI	System_Setopacitymode/*系统_设置透明混合模式*/(AlphaMode nMode/*AlphaBlend_开头*/)
{
	g_manager->SetAlphaMode(nMode);
}
/*API*/AlphaMode		WINAPI	System_Getopacitymode/*系统_取得透明混合模式*/()
{
	return g_manager->GetAlphaMode();
}
/*API*/VOID		WINAPI	System_Setenablez/*系统_开启Z缓冲*/(BOOL b/*开关*/)
{
	g_manager->SetZWriteEnable(b);
}
/*API*/BOOL		WINAPI	System_Getenablez/*系统_是否开启Z缓冲*/()
{
	return g_manager->GetZWriteEnable();
}
/*API*/VOID			WINAPI	System_Setenablevr/*系统_开启VR渲染模式*/(BOOL b/*开关*/)
{
	g_manager.SetVR3DViewer(b);
}
/*API*/BOOL			WINAPI	System_Getenablevr/*系统_是否开启VR渲染模式*/()
{
	return g_manager.IsVR3DViewer();
}
/*API*/VOID			WINAPI	System_Setenablevr2/*系统_开启VR渲染模式*/(BOOL b/*开关*/)
{
	g_manager.SetVR3DViewerSingle(b);
}
/*API*/BOOL			WINAPI	System_Getenablevr2/*系统_是否开启VR渲染模式*/()
{
	return g_manager.IsVR3DSingleEye();
}
/*API*/VOID			WINAPI	System_Setvrdistance/*系统_开启VR间距*/(float f/*间距*/)
{
	GetCameraPtr()->m_fOffsetEye = f;
}
/*API*/float		WINAPI	System_Getvrdistance/*系统_是否开启VR间距*/()
{
	return GetCameraPtr()->m_fOffsetEye;
}
extern	void	SaveVRSmooth();
/*API*/VOID			WINAPI	System_vr_beginsmooth/*系统_开始VR缓慢移动*/()
{
	SaveVRSmooth();
}
/*API*/VOID			WINAPI	System_Setvr_smooth/*系统_开启VR平滑数*/(int s/*间距*/)
{
	GetCameraPtr()->m_nVRSmooth = s;
}
/*API*/int			WINAPI	System_Getvr_smooth/*系统_是否开启VR间距*/()
{
	return GetCameraPtr()->m_nVRSmooth;
}
/*API*/VOID			WINAPI	System_Setuse_thread_t/*系统_是否开启线程载入贴图操作*/(BOOL s/*间距*/)
{
	g_manager.m_bUseThread = s;
}
/*API*/BOOL			WINAPI	System_Getuse_thread_t/*系统_是否开启线程载入贴图操作*/()
{
	return g_manager.m_bUseThread;
}
/*API*/VOID		WINAPI	System_Setcullmode/*系统_设置渲染面*/(CullMode nCull/*CullMode_开头*/)
{
	g_manager->SetCullMode(nCull);
}//渲染正面、反面或者双面
/*API*/CullMode		WINAPI	System_Getcullmode/*系统_取得渲染面*/()
{
	return g_manager->GetCullMode();
}


/*API*/VOID			WINAPI	Global_table_load/*系统_表格数据_载入*/(LPCSTR szFile/*表格文件*/, ITableReaderPtr* _return/*返回指针*/)
{
	if(!szFile)return;
	ITable* r	=	loadTable_s(szFile);
	if(!r)return;
	CreateIPtr(_return, r, ITable, ITableReaderPtr);
	g_luamanager.Push(r->GetPtrID());
}
/*API*/VOID			WINAPI	Global_table_delete/*系统_表格数据_释放*/(ITableReaderPtr* ptr/*指针*/)
{
	ITable* tr		=	GetIPtr(ptr, ITable, ITableReaderPtr);
	if(tr)
	{
		g_luamanager.Pop(tr->GetPtrID());
		if(tr->IsCanDelete())
			safe_release(tr);
	}
}

/*API*/BOOL		WINAPI	ValueControl_Getenabled/*值控制_是否有效*/(IValueControlPtr* _self/*接口*/)
{
	LuaAction* tr		=	GetIPtr(_self, LuaAction, IValueControlPtr);
	if(!tr)return false;
	return tr->m_bEnabled;
}
/*API*/VOID		WINAPI	ValueControl_Setenabled/*值控制_设置有效*/(IValueControlPtr* _self/*接口*/, BOOL bEnabled/*有效*/)
{
	LuaAction* tr		=	GetIPtr(_self, LuaAction, IValueControlPtr);
	if(!tr)return;
	tr->m_bEnabled = bEnabled;
}

/*API*/VOID		WINAPI	ValueControl_addf/*值控制_下一个浮点数*/(IValueControlPtr* _self/*接口*/, float fEnd/*结束*/, float timeLength/*时钟*/, int loopCount/*循环次数*/)
{
	LuaAction* tr		=	GetIPtr(_self, LuaAction, IValueControlPtr);
	if(!tr)return;
	float4 v(fEnd, fEnd, fEnd, fEnd);
	tr->AddNext(v, timeLength, loopCount);
}
/*API*/VOID		WINAPI	ValueControl_addfloat3/*值控制_下一个浮点*/(IValueControlPtr* _self/*接口*/, const float3* fEnd/*结束*/, float timeLength/*时钟*/, int loopCount/*循环次数*/)
{
	LuaAction* tr		=	GetIPtr(_self, LuaAction, IValueControlPtr);
	if(!tr)return;
	float4 v(fEnd->x, fEnd->y, fEnd->z, fEnd->x);
	tr->AddNext(v, timeLength, loopCount);
}
/*API*/VOID		WINAPI	ValueControl_addcolor/*值控制_下一个浮点*/(IValueControlPtr* _self/*接口*/, const color4* fEnd/*结束*/, float timeLength/*时钟*/, int loopCount/*循环次数*/)
{
	LuaAction* tr		=	GetIPtr(_self, LuaAction, IValueControlPtr);
	if(!tr)return;
	float4 v(fEnd->r, fEnd->g, fEnd->b, fEnd->a);
	tr->AddNext(v, timeLength, loopCount);
}

/*API*/VOID			WINAPI	Global_val_delete/*全局_值控制删除*/(IValueControlPtr* ptr/*指针*/)
{
	LuaAction* tr		=	GetIPtr(ptr, LuaAction, IValueControlPtr);
	if(tr)
		delete tr;
}
/*API*/VOID		WINAPI	ValueControl_reset/*值控制_重新从第一个点开始*/(IValueControlPtr* _self/*接口*/)
{
	LuaAction* tr		=	GetIPtr(_self, LuaAction, IValueControlPtr);
	if(!tr)return;
	tr->m_dTime = 0;
	tr->m_currentIndex = 0;
}
/*API*/VOID		WINAPI	ValueControl_clear/*值控制_删除所有点*/(IValueControlPtr* _self/*接口*/)
{
	LuaAction* tr		=	GetIPtr(_self, LuaAction, IValueControlPtr);
	if(!tr)return;
	tr->m_vals.clear();
	tr->m_dTime = 0;
	tr->m_currentIndex = 0;
}

/*API*/VOID			WINAPI	Global_val_create/*全局_值控制*/(LPCSTR szFunc/*lua函数*/, IValueControlPtr* _return/*指针*/)
{
	LuaAction* a = new LuaAction();
	a->m_luaFunc = szFunc;
	CreateIPtr(_return, a, LuaAction, IValueControlPtr);
	g_luamanager.Push(a->GetPtrID());
}


/*API*/INT		WINAPI	Table_linecount/*表格数据_取行数*/(ITableReaderPtr* _self/*指针*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)return 0;
	return tr->GetLineCount();
}
/*API*/LPCSTR	WINAPI	Table_read_t/*表格数据_读文本*/(ITableReaderPtr* _self/*指针*/, INT lineIndex/*行索引*/, int headIndex/*表格头*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)return "";
	return tr->GetTextVal(lineIndex, headIndex);
}
/*API*/INT		WINAPI	Table_read_i/*表格数据_读整数*/(ITableReaderPtr* _self/*指针*/, INT lineIndex/*行索引*/, int headIndex/*列标题*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)return 0;
	return tr->GetIntVal(lineIndex, headIndex);
}
/*API*/float	WINAPI	Table_read_f/*表格数据_读小数*/(ITableReaderPtr* _self/*指针*/, INT lineIndex/*行索引*/, int headIndex/*列标题*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)return 0;
	return tr->GetDoubleVal(lineIndex, headIndex);
}
/*API*/LPCSTR	WINAPI	Table_id_t/*表格数据_读文本根据ID*/(ITableReaderPtr* _self/*指针*/, INT id/*ID值*/, LPCSTR szHeadName/*列标题*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)
		return "";
	return tr->id_t(id, szHeadName);
}
/*API*/INT		WINAPI	Table_id_i/*表格数据_读整数根据ID*/(ITableReaderPtr* _self/*指针*/, INT id/*ID值*/, LPCSTR szHeadName/*列标题*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)
		return 0;
	return tr->id_i(id, szHeadName);
}
/*API*/float	WINAPI	Table_id_f/*表格数据_读小数根据ID*/(ITableReaderPtr* _self/*指针*/, INT id/*ID值*/, LPCSTR szHeadName/*列标题*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)
		return 0;
	return tr->id_f(id, szHeadName);
}
/*API*/LPCSTR	WINAPI	Table_name_t/*表格数据_读文本根据Name*/(ITableReaderPtr* _self/*指针*/, LPCSTR szName/*ID值*/, LPCSTR szHeadName/*列标题*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)
		return "";
	return tr->name_t(szName, szHeadName);
}
/*API*/INT		WINAPI	Table_name_i/*表格数据_读整数根据Name*/(ITableReaderPtr* _self/*指针*/, LPCSTR szName/*ID值*/, LPCSTR szHeadName/*列标题*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)
		return 0;
	return tr->name_i(szName, szHeadName);
}
/*API*/float	WINAPI	Table_name_f/*表格数据_读小数根据Name*/(ITableReaderPtr* _self/*指针*/, LPCSTR szName/*ID值*/, LPCSTR szHeadName/*列标题*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)
		return 0;
	return tr->name_f(szName, szHeadName);
}
/*API*/LPCSTR	WINAPI	Table_search_t/*表格数据_读文本根据搜索*/(ITableReaderPtr* _self/*指针*/, LPCSTR szHeadName/*列标题*/, LPCSTR szLineValue/*行值*/, LPCSTR szFindHeader/*返回列名*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)
		return "";
	return tr->search_t(szFindHeader, szHeadName, szLineValue);
}
/*API*/INT		WINAPI	Table_search_i/*表格数据_读整数根据搜索*/(ITableReaderPtr* _self/*指针*/, LPCSTR szHeadName/*列标题*/, LPCSTR szLineValue/*行值*/, LPCSTR szFindHeader/*返回列名*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)
		return 0;
	return tr->search_i(szFindHeader, szHeadName, szLineValue);
}
/*API*/float	WINAPI	Table_search_f/*表格数据_读小数根据搜索*/(ITableReaderPtr* _self/*指针*/, LPCSTR szHeadName/*列标题*/, LPCSTR szLineValue/*行值*/, LPCSTR szFindHeader/*返回列名*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr)
		return 0;
	return tr->search_f(szFindHeader, szHeadName, szLineValue);
}
/*API*/INT		WINAPI	Table_search_col/*表格数据_取列标题索引*/(ITableReaderPtr* _self/*指针*/, LPCSTR szHeadName/*列标题*/)
{
	ITable* tr		=	GetIPtr(_self, ITable, ITableReaderPtr);
	if(!tr || !szHeadName)return 0;
	return tr->SearchCol(szHeadName);
}
/*API*/LPCSTR	WINAPI	UIElement_Getimage/*界面元素_取得纹理*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return "";
	return e3->GetTexture();
}
/*API*/VOID		WINAPI	UIElement_Setimage/*界面元素_设置纹理*/(IElementPtr* _self/*接口*/, LPCSTR szFileName/*纹理文件*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3|| !szFileName)return;
	e3->SetTexture(szFileName);
}//
/*API*/VOID		WINAPI	UIElement_Setfontid/*界面元素_设置字体ID*/(IElementPtr* _self/*接口*/, INT nFontID/*字体ID*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	e3->SetFontID(nFontID);
}
/*API*/INT		WINAPI	UIElement_Gettextureid/*界面元素_取纹理ID*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return 0;
	return e3->GetTextureID();
}//在界面管理器中能找到对应的纹理文件
/*API*/VOID		WINAPI	UIElement_Settextureid/*界面元素_设置纹理ID*/(IElementPtr* _self/*接口*/, INT nID/*id*/)//在界面管理器中能找到对应的纹理ID
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	e3->SetTextureID(nID);
}

/*API*/INT		WINAPI	UIElement_Getfontid/*界面元素_取字体ID*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return 0;
	return e3->GetFontID();
}
/*API*/LPCSTR	WINAPI	UIElement_Gettexturemode/*界面元素_取得纹理区域*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return "";
	return "";//e3->GetTextureMode();
}
/*API*/VOID		WINAPI	UIElement_Settexturemode/*界面元素_设置纹理区域*/(IElementPtr* _self/*接口*/, LPCSTR szMode/*模式*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3 || !szMode)return;
	e3->SetTextureMode(szMode);
}

/*API*/VOID		WINAPI	UIElement_Gettexturerect/*界面元素_取得纹理区域*/(IElementPtr* _self/*接口*/, Rect* _return)
{
	memset(_return, 0, sizeof(Rect));
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	*_return	=	(Rect&)e3->GetSourceRect();
}
/*API*/VOID		WINAPI	UIElement_Settexturerect/*界面元素_取得纹理区域*/(IElementPtr* _self/*接口*/, const Rect* v/*返回*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	e3->SetSourceRect(*(Rect*)v);
}
/*API*/VOID		WINAPI	UIElement_Gettextcolor/*界面元素_取文本颜色*/(IElementPtr* _self/*接口*/, color4* _return)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	*_return	=	e3->GetTextColor();
}
/*API*/VOID		WINAPI	UIElement_Settextcolor/*界面元素_设置文本颜色*/(IElementPtr* _self/*接口*/, const color4* c/*颜色*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	e3->SetTextColor(*c);
}
/*API*/VOID	WINAPI	UIElement_Getimagecolor/*界面元素_取纹理颜色*/(IElementPtr* _self/*接口*/, color4* _return)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	*_return	=	e3->GetTextureColor();
}
/*API*/VOID		WINAPI	UIElement_Setimagecolor/*界面元素_设置纹理颜色*/(IElementPtr* _self/*接口*/, const color4* c/*颜色*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	e3->SetTextureColor(*c);
}
/*API*/BOOL		WINAPI	UIElement_isscale/*界面元素_是否缩放*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return false;
	return e3->GetRenderScale();
}
/*API*/BOOL		WINAPI	UIElement_GetRenderScaleAsWrap/*界面元素_是否缩放_可扩展*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return false;
	return e3->GetRenderScaleAsWrap();
}
/*API*/VOID		WINAPI	UIElement_setscale/*界面元素_设置缩放*/(IElementPtr* _self/*接口*/, BOOL bScale/*缩放*/, BOOL bWrap/*可扩展*/, INT wrapWidth/*扩展宽度*/, INT wrapHeight/*扩展高度*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	e3->SetRenderScale(bScale == TRUE, bWrap == TRUE, Pixel(wrapWidth, wrapHeight));
}
/*API*/INT		WINAPI	UIElement_GetWrapWidth/*界面元素_取扩展宽度*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return 0;
	return e3->GetWrapWidth().x;
}
/*API*/INT		WINAPI	UIElement_getWrapHeight/*界面元素_取扩展高度*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return 0;
	return e3->GetWrapWidth().y;
}
/*API*/BlendMode	WINAPI	UIElement_Getsrcblend/*界面元素_源混合*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return (BlendMode)0;
	return (BlendMode)e3->GetSrcBlend();
}
/*API*/BlendMode	WINAPI	UIElement_Getdestblend/*界面元素_目标混合*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return (BlendMode)0;
	return (BlendMode)e3->GetDestBlend();
}
/*API*/TextureBlend	WINAPI	UIElement_Gettextureblend/*界面元素_图片混合*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return (TextureBlend)0;
	return (TextureBlend)e3->GetColorOP();
}
/*API*/INT					WINAPI	UIElement_uvactioncount/*界面元素_取UV动画数量*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return 0;
	return e3->GetUVAnimationCount();
}
/*API*/LPCSTR				WINAPI	UIElement_uvaction_at/*界面元素_取UV动画*/(IElementPtr* _self/*接口*/, INT nIndex/*索引*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return "";
	UVAnimation* ani = e3->GetUVAnimationPtr(nIndex);
	if(ani)
		return ani->GetName();
	return "";
}
/*API*/LPCSTR				WINAPI	UIElement_Getuvaction/*界面元素_取激活UV动画*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return "";
	if(e3->GetActiveUVAnimation())
		return e3->GetActiveUVAnimation()->GetName();
	return "";
}
/*API*/VOID					WINAPI	UIElement_Setuvaction/*界面元素_设置激活UV动画*/(IElementPtr* _self/*接口*/, LPCSTR map/*UV动画名字*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3 || !map)return;
	for(int i=0;i<e3->GetUVAnimationCount();i++)
	{
		if(stricmp(e3->GetUVAnimationPtr(i)->GetName(), map) == 0)
		{
			e3->ActiveUVAnimation(e3->GetUVAnimationPtr(i));
			break;
		}
	}
}

/*API*/VOID		WINAPI	UIElement_Settilewidth/*界面元素_设置横向平铺*/(IElementPtr* _self/*接口*/, INT nTileWidth/*横向平铺*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	e3->SetTileWidth(nTileWidth);
}
/*API*/VOID		WINAPI	UIElement_Settileheight/*界面元素_设置纵向平铺*/(IElementPtr* _self/*接口*/, INT nTileHeight/*纵向平铺*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	e3->SetTileHeight(nTileHeight);
}
/*API*/VOID		WINAPI	UIElement_Setuvspeed/*界面元素_设置UV播放速度*/(IElementPtr* _self/*接口*/, float f/*播放速度*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	e3->SetUVPlaySpeed(f);
}
/*API*/float	WINAPI	UIElement_Getuvspeed/*界面元素_取UV播放速度*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return 0;
	return e3->GetUVPlaySpeed();
}

/*API*/INT		WINAPI	UIElement_Gettilewidth/*界面元素_取横向平铺*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return 0;
	return e3->GetTileWidth();
}
/*API*/INT		WINAPI	UIElement_Gettileheight/*界面元素_取纵向平铺*/(IElementPtr* _self/*接口*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return 0;
	return e3->GetTileHeight();
}

/*API*/VOID		WINAPI	UIElement_Setsrcblend/*界面元素_源混合*/(IElementPtr* _self/*接口*/, BlendMode m/*方式*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	e3->SetSrcBlend(m);
}
/*API*/VOID		WINAPI	UIElement_Setdestblend/*界面元素_目标混合*/(IElementPtr* _self/*接口*/, BlendMode m/*方式*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	e3->SetDestBlend(m);
}
/*API*/VOID		WINAPI	UIElement_Settextureblend/*界面元素_图片混合*/(IElementPtr* _self/*接口*/, TextureBlend op/*方式*/)
{
	UIElement* e3 = GetIPtrT(_self, UIElementBase, IElementPtr);
	if(!e3)return;
	e3->SetColorOP(op);
}


/*API*/float		WINAPI	UIControl_Getactiontime/*界面控件_取播放动作时间*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->m_playdata.m_fFrame;
}
/*API*/VOID			WINAPI	UIControl_Setactiontime/*界面控件_设置播放动作时间*/(IControlPtr* _self/*接口*/, float f/*动作*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->m_playdata.m_fFrame = f;
}

/*API*/VOID		WINAPI	UIControl_Setenabled/*控件_设置有效*/(IControlPtr* _self/*接口*/, BOOL b/*有效*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetEnabled(b == TRUE);
}
/*API*/BOOL		WINAPI	UIControl_Getenabled/*控件_是否有效*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return false;
	return c->GetEnabled();
}
/*API*/VOID		WINAPI	UIControl_Setdisplay/*控件_设置显示*/(IControlPtr* _self/*接口*/, BOOL b/*有效*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetVisible(b == TRUE);
}
/*API*/BOOL		WINAPI	UIControl_Getdisplay/*控件_是否显示*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return false;
	return c->GetVisible();
}
/*API*/UIControlType		WINAPI	UIControl_gettype/*控件_取控件类型*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return (UIControlType)0;
	return (UIControlType)c->GetType();
}
/*API*/INT		WINAPI	UIControl_getId/*控件_取控件ID*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetID();
}

/*API*/BOOL		WINAPI	UIControl_Getpixelpick/*界面控件_设置像素级点击*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->IsPickPixel();
}
/*API*/VOID		WINAPI	UIControl_Setpixelpick/*界面控件_取像素级点击ID*/(IControlPtr* _self/*接口*/, BOOL b/*移动对话框*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetPickPixel(b);
}
/*API*/BOOL		WINAPI	UIControl_pick/*界面控件_像素级点击*/(IControlPtr* _self/*接口*/, INT x, INT y)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	Pixel pt(x,y);
	c->m_dialogPtr->OnSetMouseMatrix(pt);
	if( !c->m_bIsEnabled || !c->m_bVisible || c->m_bIsBackground)
		return 0;
	Pixel clientPos;
	clientPos	=	c->ScreenToClient(pt);
	if(c->isPtIn( clientPos ))
		return true;
	return false;
}
/*API*/float		WINAPI	UIControl_Getspeed/*界面控件_取得动作播放速度*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetPlaySpeed();
}
/*API*/VOID			WINAPI	UIControl_Setspeed/*界面控件_设置动作播放速度*/(IControlPtr* _self/*接口*/, float fSpeed/*速度*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetPlaySpeed(fSpeed);
}

/*API*/BOOL		WINAPI	UIControl_Getmovedialog/*界面控件_设置移动对话框*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetParentMove();
}
/*API*/VOID		WINAPI	UIControl_Setmovedialog/*界面控件_取移动对话框ID*/(IControlPtr* _self/*接口*/, BOOL b/*移动对话框*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetParentMove(b);
}
Rect	GetCRect(UIControl* c){
	Rect rc	=	c->GetRenderRect();
	Pixel center(c->m_rotCenter.x + rc.left, c->m_rotCenter.y + rc.top);
	rc.left = (int)((float)(rc.left - center.x) * c->m_scalesave.x) + center.x;
	rc.right = (int)((float)(rc.right - center.x) * c->m_scalesave.x) + center.x;
	rc.top = (int)((float)(rc.top - center.y) * c->m_scalesave.y) + center.y;
	rc.bottom = (int)((float)(rc.bottom - center.y) * c->m_scalesave.y) + center.y;
	return rc;
}
VOID	SetCRect(UIControl* c, Rect rc){
	Pixel center(c->m_rotCenter.x + rc.left, c->m_rotCenter.y + rc.top);
	rc.left = (int)((float)(rc.left - center.x) / c->m_scalesave.x) + center.x;
	rc.right = (int)((float)(rc.right - center.x) / c->m_scalesave.x) + center.x;
	rc.top = (int)((float)(rc.top - center.y) / c->m_scalesave.y) + center.y;
	rc.bottom = (int)((float)(rc.bottom - center.y) / c->m_scalesave.y) + center.y;
	c->SetRenderRect(rc);
}
/*API*/INT		WINAPI	UIControl_Getleft/*界面控件_取左*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	Rect rc	=	GetCRect(c);//->GetRenderRect());
	return rc.left;
}
/*API*/VOID		WINAPI	UIControl_Setleft/*界面控件_设置左*/(IControlPtr* _self/*接口*/, INT nValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	Rect rc	=	GetCRect(c);//->GetRenderRect());
	int w		=	rc.GetWidth();
	rc.left		=	nValue;
	//rc.right	=	rc.left + w;
	SetCRect(c, rc);//c->SetRenderRect(rc);
}
/*API*/INT		WINAPI	UIControl_Gettop/*界面控件_取上*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	Rect rc	=	GetCRect(c);//->GetRenderRect());
	return rc.top;
}
/*API*/VOID		WINAPI	UIControl_Settop/*界面控件_设置上*/(IControlPtr* _self/*接口*/, INT nValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	Rect rc	=	GetCRect(c);//->GetRenderRect());
	int w		=	rc.GetHeight();
	rc.top		=	nValue;
	//rc.bottom	=	rc.top + w;
	SetCRect(c, rc);//c->SetRenderRect(rc);
}
/*API*/INT		WINAPI	UIControl_Getwidth/*界面控件_取宽*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	Rect rc	=	GetCRect(c);//->GetRenderRect());
	return rc.GetWidth();
}
/*API*/VOID		WINAPI	UIControl_Setwidth/*界面控件_设置宽*/(IControlPtr* _self/*接口*/, INT nValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	Rect rc	=	GetCRect(c);//->GetRenderRect());
	rc.right	=	rc.left + nValue;
	SetCRect(c, rc);//c->SetRenderRect(rc);
}
/*API*/INT		WINAPI	UIControl_Getheight/*界面控件_取高*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	Rect rc	=	GetCRect(c);//->GetRenderRect());
	return rc.GetHeight();
}
/*API*/VOID		WINAPI	UIControl_Setheight/*界面控件_设置高*/(IControlPtr* _self/*接口*/, INT nValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	Rect rc	=	GetCRect(c);//->GetRenderRect());
	rc.bottom	=	rc.top + nValue;
	SetCRect(c, rc);//SetRenderRect(rc);
}
/*API*/INT		WINAPI	UIControl_Getright/*界面控件_取右*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return GetCRect(c/*->GetRenderRect()*/).right;
}
/*API*/VOID		WINAPI	UIControl_Setright/*界面控件_设置右*/(IControlPtr* _self/*接口*/, INT nValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	Rect rc	=	GetCRect(c);//->GetRenderRect();
	rc.right = nValue;
	SetCRect(c, rc);//c->SetRenderRect(rc);
}
/*API*/INT		WINAPI	UIControl_Getbottom/*界面控件_取下*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return GetCRect(c).bottom;//->GetRenderRect().bottom;
}
/*API*/VOID		WINAPI	UIControl_Setbottom/*界面控件_设置下*/(IControlPtr* _self/*接口*/, INT nValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	Rect rc	=	GetCRect(c);//->GetRenderRect();
	rc.bottom = nValue;
	SetCRect(c, rc);//c->SetRenderRect(rc);
}
/*API*/INT		WINAPI	UIControl_Getmoveleft/*界面控件_取左*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	Rect rc	=	GetCRect(c);//->GetRenderRect();
	return rc.left;
}
/*API*/VOID		WINAPI	UIControl_Setmoveleft/*界面控件_设置左*/(IControlPtr* _self/*接口*/, INT nValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	Rect rc	=	GetCRect(c);//->GetRenderRect();
	int w		=	rc.GetWidth();
	rc.left		=	nValue;
	rc.right	=	rc.left + w;
	SetCRect(c, rc);//c->SetRenderRect(rc);
}
/*API*/INT		WINAPI	UIControl_Getmovetop/*界面控件_取上*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	Rect rc	=	GetCRect(c);//->GetRenderRect();
	return rc.top;
}
/*API*/VOID		WINAPI	UIControl_Setmovetop/*界面控件_设置上*/(IControlPtr* _self/*接口*/, INT nValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	Rect rc	=	GetCRect(c);//->GetRenderRect();
	int w		=	rc.GetHeight();
	rc.top		=	nValue;
	rc.bottom	=	rc.top + w;
	SetCRect(c, rc);//c->SetRenderRect(rc);
}
/*API*/INT		WINAPI	UIControl_Getmoveright/*界面控件_取右*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return GetCRect(c).right;//->GetRenderRect().right;
}
/*API*/VOID		WINAPI	UIControl_Setmoveright/*界面控件_设置右*/(IControlPtr* _self/*接口*/, INT nValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	Rect rc	=	GetCRect(c);//->GetRenderRect();
	int w = rc.GetWidth();
	rc.right = nValue;
	rc.left = rc.right - w;
	SetCRect(c, rc);//c->SetRenderRect(rc);
}
/*API*/INT		WINAPI	UIControl_Getmovebottom/*界面控件_取下*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return GetCRect(c).bottom;//->GetRenderRect().bottom;
}
/*API*/VOID		WINAPI	UIControl_Setmovebottom/*界面控件_设置下*/(IControlPtr* _self/*接口*/, INT nValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	Rect rc	=	GetCRect(c);//->GetRenderRect();
	int h = rc.GetHeight();
	rc.bottom = nValue;
	rc.top = rc.bottom - h;
	SetCRect(c, rc);//c->SetRenderRect(rc);
}



/*API*/VOID		WINAPI	UIControl_cloneto/*界面控件_克隆到其他对话框*/(IControlPtr* _self/*接口*/, float fTime/*克隆周期，如果是负的则是无限时间*/, IDialogPtr* dest/*目标对话框*/, IControlPtr* _return/*ret*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	UIDialog* d	=	GetIPtr(dest, UIDialog, IDialogPtr);
	if(!c || !d)return;
	UIControl* cr	=	c->Clone(fTime, d);
	if(!cr)return;
	SetIPtr(_return, &cr->m_this, PhantomUIControlBase, IControlPtr);
}
/*API*/VOID		WINAPI	UIControl_clone/*界面控件_克隆*/(IControlPtr* _self/*接口*/, float fTime/*克隆周期，如果是负的则是无限时间*/, IControlPtr* _return/*ret*/)
{
///*API*/VOID		WINAPI	UIControl_clone/*控件_克隆*/(IControlPtr* _self/*接口*/, IControlPtr* _return/*ret*/)
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	UIControl* cr	=	c->Clone(fTime);
	if(!cr)return;
	SetIPtr(_return, &cr->m_this, PhantomUIControlBase, IControlPtr);
}
/*API*/VOID		WINAPI	UIControl_Setname/*控件_设置名字*/(IControlPtr* _self/*接口*/, LPCSTR szName/*名字*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c|| !szName)return;
	c->SetName(szName);
}
/*API*/LPCSTR	WINAPI	UIControl_Getname/*控件_取名字*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return "";
	return c->GetName();
}
/*API*/DrawTextAlign	WINAPI	UIControl_Gettextalign/*控件_设置文本格式*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return DrawTextAlign_TOP;
	return (DrawTextAlign)c->GetTextFormat();
}//
/*API*/VOID		WINAPI	UIControl_Settextalign/*控件_设置文本格式*/(IControlPtr* _self/*接口*/, DrawTextAlign fmt/*格式*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetTextFormat(fmt);
}//
/*API*/INT		WINAPI	UIControl_Gettextshadow_width/*控件_取阴影宽度*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->ShowTextShadow();
}
/*API*/VOID		WINAPI	UIControl_Settextshadow_width/*控件_设置阴影宽度*/(IControlPtr* _self/*接口*/, INT width/*宽度*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->ShowTextShadow(width);
}
/*API*/INT		WINAPI	UIControl_Gettextshadow_color/*控件_取阴影颜色*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetShadowColor();
}
/*API*/VOID		WINAPI	UIControl_Settextshadow_color/*控件_设置阴影颜色*/(IControlPtr* _self/*接口*/, color4* clr/*颜色*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetShadowColor(*clr);
}
/*API*/VOID		WINAPI	UIControl_Getrect/*控件_取控件区域*/(IControlPtr* _self/*接口*/, Rect* _return/*ret*/)
{
	memset(_return, 0, sizeof(Rect));
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	*_return	=	GetCRect(c);//->GetRenderRect();
}
/*API*/VOID		WINAPI	UIControl_Setrect/*控件_设置控件区域*/(IControlPtr* _self/*接口*/, const Rect* rc/*矩形区域*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	SetCRect(c, *rc);//c->SetRenderRect(*rc);
}
//

/*API*/float		WINAPI	UIControl_Getscale/*界面控件_缩放*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 1;
	return c->m_scalesave.x;
}
/*API*/VOID			WINAPI	UIControl_Setscale/*界面控件_缩放*/(IControlPtr* _self/*接口*/, float fScale/*缩放*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetScale(fScale, fScale);
}
/*API*/float		WINAPI	UIControl_Getscalex/*界面控件_缩放x*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 1;
	return c->m_scalesave.x;
}
/*API*/VOID			WINAPI	UIControl_Setscalex/*界面控件_缩放x*/(IControlPtr* _self/*接口*/, float fScale/*缩放*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetScale(fScale, c->m_scalesave.y);
}
/*API*/float		WINAPI	UIControl_Getscaley/*界面控件_缩放y*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 1;
	return c->m_scalesave.y;
}
/*API*/VOID			WINAPI	UIControl_Setscaley/*界面控件_缩放y*/(IControlPtr* _self/*接口*/, float fScale/*缩放*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetScale(c->m_scalesave.x, fScale);
}

/*API*/INT		WINAPI	UIControl_Getlayer/*控件_取层级*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetLayer();
}
/*API*/VOID		WINAPI	UIControl_Setlayer/*控件_设置层级*/(IControlPtr* _self/*接口*/, INT layer/*层级*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetLayer(layer);
}
/*API*/INT		WINAPI	UIControl_elementcount/*控件_取元素数量*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetElementCount();
}
/*API*/VOID		WINAPI	UIControl_element_at/*控件_取元素*/(IControlPtr* _self/*接口*/, INT index/*索引*/, IElementPtr* _return/*ret*/)//, IElementPtr* _ret/*返回*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	UIElement* e	=	c->GetElementPtr(index);
	if(!e)return;
	SetIPtr(_return, &e->m_this, UIElementBase, IElementPtr);
}

/*API*/VOID		WINAPI	UIControl_GetActiveAction/*控件_取当前动画*/(IControlPtr* _self/*接口*/, IUIAnimationPtr* _return/*ret*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	if(!c->GetActiveTrack())
		return;
	SetIPtr(_return, c->GetActiveTrack(), UIAnimationTrack, IUIAnimationPtr);
}

/*API*/VOID				WINAPI	UIControl_SetactionClass/*控件_设置当前动画*/(IControlPtr* _self/*接口*/, IUIAnimationPtr* ani/*当前动画*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	UIAnimationTrack* track	=	GetIPtr(ani, UIAnimationTrack, IUIAnimationPtr);
	if(!track)
		return;
	c->ActiveTrackByID(track->GetID());
}

/*API*/LPCSTR			WINAPI	UIControl_Getactionby/*控件_取当前动画根据名称*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return "";
	if(!c->GetActiveTrack())
		return "";
	return c->GetActiveTrack()->GetName();
}

/*API*/VOID				WINAPI	UIControl_Setactionby/*控件_设置当前动画根据名称*/(IControlPtr* _self/*接口*/, LPCSTR ani/*当前动画*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c|| !ani)return;
	c->ActiveTrackByName(ani);
}

/*API*/INT				WINAPI	UIControl_actioncount/*控件_设置当前动画*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetTrackCount();
}

/*API*/VOID	WINAPI	UIControl_action_at/*控件_取动画*/(IControlPtr* _self/*接口*/, INT nIndex/*索引*/, IUIAnimationPtr* _return/*ret*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	UIAnimationTrack* ani	=	c->GetTrackPtr(nIndex);
	if(!ani)
		return;
	SetIPtr(_return, ani, UIAnimationTrack, IUIAnimationPtr);
}

/*API*/VOID	WINAPI	UIControl_FindAction/*控件_查找动画*/(IControlPtr* _self/*接口*/, LPCSTR ani/*动画名称*/, IUIAnimationPtr* _return/*ret*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c|| !ani)return;
	UIAnimationTrack* track	=	c->GetTrackPtrByName(ani);
	if(!track)
		return;
	SetIPtr(_return, track, UIAnimationTrack, IUIAnimationPtr);
}
/*API*/float			WINAPI	UIControl_Getwaitbefore_actionplay/*界面控件_取得播放前等待*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetSleepTime();
}
/*API*/VOID				WINAPI	UIControl_Setwaitbefore_actionplay/*界面控件_设置播放前等待*/(IControlPtr* _self/*接口*/, float fTime/*等待时间*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetSleepTime(fTime);
}
/*API*/VOID		WINAPI	UIControl_Setscroll_min/*界面控件_滚动条最小*/(IControlPtr* _self/*接口*/, INT nMin/*滚动条最小*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	if(c->GetType() == UIControlType_SCROLLBAR)
	{
		UIScrollBar* sc = static_cast<UIScrollBar*>(c);
		sc->SetScrollRange(nMin, sc->m_nEnd);
	}
}
/*API*/VOID		WINAPI	UIControl_Setscroll_max/*界面控件_滚动条最大*/(IControlPtr* _self/*接口*/, INT nMax/*滚动条最大*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	if(c->GetType() == UIControlType_SCROLLBAR)
	{
		UIScrollBar* sc = static_cast<UIScrollBar*>(c);
		sc->SetScrollRange(sc->m_nStart, nMax);
	}
}
/*API*/VOID		WINAPI	UIControl_Setscroll_pos/*界面控件_滚动条位置*/(IControlPtr* _self/*接口*/, INT nPos/*滚动条值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	if(c->GetType() == UIControlType_SCROLLBAR)
	{
		UIScrollBar* sc = static_cast<UIScrollBar*>(c);
		sc->SetIntVal(nPos);
	}
}
/*API*/INT		WINAPI	UIControl_Getscroll_min/*界面控件_滚动条最小*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	if(c->GetType() == UIControlType_SCROLLBAR)
	{
		UIScrollBar* sc = static_cast<UIScrollBar*>(c);
		return sc->m_nStart;
	}
	return 0;
}
/*API*/INT		WINAPI	UIControl_Getscroll_max/*界面控件_滚动条最大*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	if(c->GetType() == UIControlType_SCROLLBAR)
	{
		UIScrollBar* sc = static_cast<UIScrollBar*>(c);
		return sc->m_nEnd;
	}
	return 0;
}
/*API*/INT		WINAPI	UIControl_Getscroll_pos/*界面控件_滚动条位置*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	if(c->GetType() == UIControlType_SCROLLBAR)
	{
		UIScrollBar* sc = static_cast<UIScrollBar*>(c);
		return sc->m_nPosition;
	}
	return 0;
}
/*API*/INT			WINAPI	UIControl_timeout/*界面控件_定时引发*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/)//定时引发，只引发一次，毫秒记,脚本无参数,一个函数可以创建多个实例,事件[szLuaFunc(obj)--(obj=UIControl)定期事件,只引发一次];返回TimeID
{
	return g_luamanager.AddTimeout(nMillisecond, szLuaFunc, 0, LuaTimeData::IDT_Control, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIControl_interval/*界面控件_间隔引发*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用次数*/)//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(tm,endof,obj,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
{
	return g_luamanager.AddInterval(nMillisecond, nMaxMillisecond, szLuaFunc, 0, LuaTimeData::IDT_Control, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIControl_framemove/*界面控件_循环引发*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/)//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(tm,endof,obj,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
{
	return g_luamanager.AddFramemove(nMaxMillisecond, szLuaFunc, 0, LuaTimeData::IDT_Control, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIControl_timeout2/*界面控件_定时引发带一个文本参数*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, LPCSTR param/*文本参数*/)//定时引发，只引发一次，毫秒记,代替以前的timeout_1,脚本第一个参数是传入的参数,文本,一个函数可以创建多个实例,事件[szLuaFunc(obj,param)--(obj=UIControl)传入的自定义参数];返回TimeID
{
	return g_luamanager.AddTimeout(nMillisecond, szLuaFunc, param, LuaTimeData::IDT_Control, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIControl_interval2/*界面控件_间隔引发带一个文本参数*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/)//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,代替以前的interval_1,参数为之有所改动,事件[szLuaFunc(obj,tm,endof,param,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID
{
	return g_luamanager.AddInterval(nMillisecond, nMaxMillisecond, szLuaFunc, param, LuaTimeData::IDT_Control, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIControl_framemove2/*界面控件_循环引发*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/)//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,第二个参数是传入的参数,文本,一个函数只能创建一个,事件[szLuaFunc(obj,tm,endof,param,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID
{
	return g_luamanager.AddFramemove(nMaxMillisecond, szLuaFunc, param, LuaTimeData::IDT_Control, _self->t, _self->ptr, _self->idx);
}

/*API*/VOID		WINAPI	UIControl_Setscroll_speed/*界面控件_滚动速度*/(IControlPtr* _self/*接口*/, float fSpeed/*滚动速度*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	if(c->GetType() == UIControlType_SCROLLBAR)
	{
		UIScrollBar* sc = static_cast<UIScrollBar*>(c);
		sc->m_fScrollSpeed = getmax(fSpeed, 0.1);
	}
}
/*API*/float	WINAPI	UIControl_Getscroll_speed/*界面控件_取滚动速度*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	if(c->GetType() == UIControlType_SCROLLBAR)
	{
		UIScrollBar* sc = static_cast<UIScrollBar*>(c);
		return sc->m_fScrollSpeed;
	}
	return 0;
}
/*API*/VOID		WINAPI	UIControl_Setscroll_firstspeed/*界面控件_滚动初始速度*/(IControlPtr* _self/*接口*/, float fSpeed/*滚动速度*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	if(c->GetType() == UIControlType_SCROLLBAR)
	{
		UIScrollBar* sc = static_cast<UIScrollBar*>(c);
		sc->m_fScrollFirstSpeed = getmax(fSpeed, 0.01);
	}
}
/*API*/float	WINAPI	UIControl_Getscroll_firstspeed/*界面控件_取滚动初始速度*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	if(c->GetType() == UIControlType_SCROLLBAR)
	{
		UIScrollBar* sc = static_cast<UIScrollBar*>(c);
		return sc->m_fScrollFirstSpeed;
	}
	return 0;
}
/*API*/VOID		WINAPI	UIControl_Setscroll_extend/*界面控件_滚动条是否可以拽出*/(IControlPtr* _self/*接口*/, BOOL bExtend/*滚动拽出*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	if(c->GetType() == UIControlType_SCROLLBAR)
	{
		UIScrollBar* sc = static_cast<UIScrollBar*>(c);
		sc->m_bScrollExtend = bExtend;
	}
}
/*API*/BOOL		WINAPI	UIControl_Getscroll_extend/*界面控件_取滚动速度*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	if(c->GetType() == UIControlType_SCROLLBAR)
	{
		UIScrollBar* sc = static_cast<UIScrollBar*>(c);
		return sc->m_bScrollExtend;
	}
	return 0;
}

/*API*/VOID		WINAPI	UIControl_Setscroll_v/*界面控件_设置纵向滚动条*/(IControlPtr* _self/*接口*/, IControlPtr* scrollV/*滚动条纵向*/)//设置子窗体在控件内部显示滚动条
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	UIControl* sc = GetIPtrT(scrollV, PhantomUIControlBase, IControlPtr);
	if(sc->GetType() == UIControlType_SCROLLBAR)
		c->SetScrollBarV(static_cast<UIScrollBar*>(sc));
}
/*API*/VOID		WINAPI	UIControl_Getscroll_v/*界面控件_设置纵向滚动条*/(IControlPtr* _self/*接口*/, IControlPtr* _return/*滚动条纵向*/)//设置子窗体在控件内部显示滚动条
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c || !c->m_scrollPtrV)return;
	SetIPtr(_return, &c->m_scrollPtrV->m_this, PhantomUIControlBase, IControlPtr);
}
/*API*/VOID		WINAPI	UIControl_Setscroll_h/*界面控件_设置横向滚动条*/(IControlPtr* _self/*接口*/, IControlPtr* scrollH/*滚动条纵向*/)//设置子窗体在控件内部显示滚动条
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	UIControl* sc = GetIPtrT(scrollH, PhantomUIControlBase, IControlPtr);
	if(sc->GetType() == UIControlType_SCROLLBAR)
		c->SetScrollBarH(static_cast<UIScrollBar*>(sc));
}
/*API*/VOID		WINAPI	UIControl_Getscroll_h/*界面控件_设置横向滚动条*/(IControlPtr* _self/*接口*/, IControlPtr* _return/*滚动条纵向*/)//设置子窗体在控件内部显示滚动条
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c || !c->m_scrollPtrH)return;
	SetIPtr(_return, &c->m_scrollPtrH->m_this, PhantomUIControlBase, IControlPtr);
}
/*API*/VOID			WINAPI	UIControl_Setinnermesh/*界面控件_设置控件内部模型*/(IControlPtr* _self/*接口*/, IMeshPtr* mesh/*模型*/)
{
	if(!mesh)return;
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetInnerMesh(GetIPtr(mesh,Mesh,IMeshPtr));
}
/*API*/VOID			WINAPI	UIControl_Getinnermesh/*界面控件_取控件内部模型*/(IControlPtr* _self/*接口*/, IMeshPtr* _return/*矩形*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c||!c->m_innerMesh)return;
	Mesh* mesh = c->m_innerMesh;
	SetIPtr(_return, mesh, Mesh, IMeshPtr);
}
/*API*/VOID			WINAPI	UIControl_innermesh_clear/*界面控件_清除控件内部模型*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetInnerMesh(0);
}

/*API*/VOID		WINAPI	UIControl_setChild/*界面控件_设置子窗体*/(IControlPtr* _self/*接口*/, IDialogPtr* dlg/*子窗体*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetChildDialog(GetIPtr(dlg, UIDialog, IDialogPtr));
}
/*API*/VOID		WINAPI	UIControl_removeChild/*界面控件_取子窗体*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetChildDialog(0);
}

/*API*/float	WINAPI	UIControl_Getrot2d/*界面控件_取得角度*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetUnitAngle();
}
/*API*/VOID		WINAPI	UIControl_Setrot2d/*界面控件_设置角度*/(IControlPtr* _self/*接口*/, float fAngle/*角度*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetUnitAngle(fAngle);
}
/*API*/VOID		WINAPI	UIControl_rot2d_dir/*界面控件_设置角度根据方向*/(IControlPtr* _self/*接口*/, const float2* dir/*角度*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetUnitAngleDir(*dir);
}

/*API*/VOID		WINAPI	UIControl_Settextcolor/*控件_设置文本颜色*/(IControlPtr* _self/*接口*/, const color4* clr/*颜色*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetTextColor(*clr);
}//
/*API*/VOID		WINAPI	UIControl_Gettextcolor/*控件_取文本颜色*/(IControlPtr* _self/*接口*/, color4* _return/*ret*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	*_return	=	c->GetTextColor();
}//
/*API*/VOID		WINAPI	UIControl_Setimagecolor/*控件_设置纹理颜色*/(IControlPtr* _self/*接口*/, const color4* clr/*颜色*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetTextureColor(*clr);
}//
/*API*/VOID		WINAPI	UIControl_Getimagecolor/*控件_取纹理颜色*/(IControlPtr* _self/*接口*/, color4* _return/*ret*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	*_return	=	c->GetTextureColor();
}//

/*API*/VOID		WINAPI	UIControl_Setcolor/*界面控件_设置纹理颜色*/(IControlPtr* _self/*接口*/, const color4* clr/*颜色*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetTextureColor(*clr);
	c->SetTextColor(*clr);
}
/*API*/VOID		WINAPI	UIControl_Getcolor/*界面控件_取纹理颜色*/(IControlPtr* _self/*接口*/, color4* _return/*ret*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	*_return	=	c->GetTextureColor();
}
/*API*/VOID		WINAPI	UIControl_Setopacity/*界面控件_设置纹理颜色*/(IControlPtr* _self/*接口*/, float v/*颜色*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	color4 clr = c->GetTextureColor();
	clr.a = clamp(v, 0, 1);
	c->SetTextureColor(clr);
	clr = c->GetTextColor();
	clr.a = clamp(v, 0, 1);
	c->SetTextColor(clr);
}
/*API*/float	WINAPI	UIControl_Getopacity/*界面控件_取纹理颜色*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetTextureColor().a;
}

/*API*/VOID			WINAPI	UIControl_combobox/*控件_取组合框接口*/(IControlPtr* _self/*接口*/, IComboBoxPtr* _return)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	UIComboBox* cc	=	static_cast<UIComboBox*>(c);
	if(!cc)
		return;
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, IComboBoxPtr);
}//
/*API*/VOID		WINAPI	UIControl_listbox/*控件_取列表框接口*/(IControlPtr* _self/*接口*/, IListBoxPtr* _return)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	UIListBox* l	=	static_cast<UIListBox*>(c);
	if(!l)
		return;
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, IListBoxPtr);
}//

/*API*/VOID		WINAPI	UIControl_listctrl/*控件_取行列框接口*/(IControlPtr* _self/*接口*/, IListCtrlPtr* _return)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	UIListCtrl* l	=	static_cast<UIListCtrl*>(c);
	if(!l)
		return;
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, IListCtrlPtr);
}//

/*API*/VOID		WINAPI	UIControl_sliderbar/*控件_拖动条*/(IControlPtr* _self/*接口*/, ISliderPtr* _return)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c || c->GetType() != UIControlType_SLIDER)return;
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, ISliderPtr);
}

/*API*/VOID		WINAPI	UIControl_progressbar/*控件_进度条*/(IControlPtr* _self/*接口*/, IProgressPtr* _return)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c || c->GetType() != UIControlType_PROGRESS)return;
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, IProgressPtr);
}

/*API*/VOID		WINAPI	UIControl_checkbox/*控件_单选框*/(IControlPtr* _self/*接口*/, ICheckBoxPtr* _return)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c || c->GetType() != UIControlType_CHECKBOX)return;
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, ICheckBoxPtr);
}

/*API*/VOID		WINAPI	UIControl_saved_rect/*界面对话框_取载入时位置矩形区域*/(IControlPtr* _self/*接口*/, Rect* _return/*矩形*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	*_return = c->m_loadRect;
}
/*API*/VOID		WINAPI	UIControl_saved_pos/*界面对话框_取载入时位置信息*/(IControlPtr* _self/*接口*/, Pixel* _return/*矩形*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	*_return = c->GetLoadPos();
}

/*API*/float	WINAPI	UIControl_saved_rot/*界面对话框_取载入时转动值*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->m_loadRotation;
}
/*API*/VOID		WINAPI	UIControl_reset_trans/*界面对话框_重置所有位置和转动等信息到载入之初*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetRenderRect(c->m_loadRect);
	c->SetRotation(c->m_bEnableRotation, c->m_loadRotation);
	c->SetRotCenter(c->m_loadCenter);
}


/*API*/float	WINAPI	UIControl_Getrot/*界面控件_设置转动率*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetRotationAngle();
}//
/*API*/VOID		WINAPI	UIControl_Setrot/*界面控件_设置转动率*/(IControlPtr* _self/*接口*/, float fAngle/*角度*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetRotation(TRUE, fAngle);
}
/*API*/BOOL		WINAPI	UIControl_Getrotenable/*界面控件_设置转动率*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return false;
	return c->IsRotation();
}
/*API*/VOID		WINAPI	UIControl_Setrotenable/*界面控件_设置转动率*/(IControlPtr* _self/*接口*/, BOOL bEnable/*设置*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetRotation(bEnable, c->GetRotationAngle());
}

/*API*/VOID		WINAPI	UIControl_Setrotcenter/*界面控件_设置转动中心*/(IControlPtr* _self/*接口*/, const Pixel* v/*相对于控件的中心位置*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetRotCenter(*v);
}
/*API*/VOID		WINAPI	UIControl_Getrotcenter/*界面控件_取转动中心*/(IControlPtr* _self/*接口*/, Pixel* _return/*相对于控件的中心位置*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	*_return = c->GetRotCenter();
}

/*API*/LPCSTR	WINAPI	UIControl_Gettext/*控件_取文本*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return "";
	return c->GetText();
}
/*API*/VOID		WINAPI	UIControl_Settext/*控件_设置文本*/(IControlPtr* _self/*接口*/, LPCSTR szText/*文本*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c || !szText)return;
	c->SetText(szText);
}
/*API*/VOID		WINAPI	UIControl_Setimage/*界面控件_设置纹理*/(IControlPtr* _self/*接口*/, LPCSTR szFile/*图片路径*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c || !szFile)return;
	for(int i=0;i<c->GetElementCount();i++){
		c->GetElementPtr(i)->SetTexture(szFile);
	}
}
/*API*/LPCSTR	WINAPI	UIControl_Getimage/*界面控件_取纹理*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return "";
	return c->GetElementPtr(0)->GetTexture();
}
/*API*/VOID			WINAPI	UIControl_bind_3d_maxdist/*界面控件_绑定3D的最远距离显示设置*/(IControlPtr* _self/*接口*/, float fMaxDist/*最远距离*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->m_fMax3DDist = fMaxDist;
}
/*API*/VOID			WINAPI	UIControl_bind_3d_pos/*界面控件_绑定3D位置*/(IControlPtr* _self/*接口*/, float3* pos/*固定位置*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->Bind3DPos(*pos);
}
/*API*/VOID			WINAPI	UIControl_bind_3d_obj/*界面控件_绑定3D对象位置*/(IControlPtr* _self/*接口*/, IGameObjectPtr* obj, float fHeightAdd/*高度增加*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->Bind3DObj(*obj, fHeightAdd);
}
/*API*/VOID			WINAPI	UIControl_bind_3d_clear/*界面控件_绑定3D清除*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->Bind3DClear();
}

/*API*/VOID		WINAPI	UIControl_SetIsUnitMode/*界面控件_设置单位渲染模式*/(IControlPtr* _self/*接口*/, BOOL b/*是否单位渲染模式*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetAutoSort(b);
}
/*API*/BOOL		WINAPI	UIControl_GetIsUnitMode/*界面控件_取组单位渲染模式*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return false;
	return c->GetAutoSort();
}
/*API*/VOID		WINAPI	UIControl_Setcenter/*界面控件_设置位置*/(IControlPtr* _self/*接口*/, const Pixel* pos/*ID*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->m_rotCenter = *pos;
}
/*API*/VOID		WINAPI	UIControl_Getcenter/*界面控件_取得位置*/(IControlPtr* _self/*接口*/, Pixel* _return/*ret*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	_return->x	=	_return->y = 0;
	if(!c)return;
	*_return	=	c->m_rotCenter;
}

//
/*API*/VOID		WINAPI	UICheckBox_Setchecked/*控件_设置选中*/(ICheckBoxPtr* _self/*接口*/, BOOL b/*有效*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, ICheckBoxPtr);
	if(!c)return;
	c->SetChecked(b == TRUE);
}
/*API*/BOOL		WINAPI	UICheckBox_Getchecked/*控件_是否选中*/(ICheckBoxPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, ICheckBoxPtr);
	if(!c)return false;
	return c->GetChecked();
}
/*API*/VOID		WINAPI	UIControl_Setactive/*控件_设置是否可以被激活*/(IControlPtr* _self/*接口*/, BOOL b/*设置*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetCanFocus(b == TRUE);
}
/*API*/BOOL		WINAPI	UIControl_Getactive/*控件_取是否可以被激活*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return false;
	return c->GetCanFocus();
}

/*API*/ActionType	WINAPI	UIControlGetaction/*界面控件_取播放动作*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return ActionType_None;
	return c->GetPlayAction();
}
/*API*/VOID		WINAPI	UIControl_Setaction/*界面控件_设置播放动作*/(IControlPtr* _self/*接口*/, ActionType nType/*动作*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->PlayAction(nType);
}

/*API*/ActionType	WINAPI	UIControlGetnextaction/*界面控件_取下一个播放动作*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return ActionType_None;
	return c->GetNextPlayAction();
}
/*API*/VOID		WINAPI	UIControl_Setnextaction/*界面控件_设置下一个播放动作*/(IControlPtr* _self/*接口*/, ActionType nType/*动作*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetNextPlayAction(nType);
}

/*API*/BOOL		WINAPI	UIControl_isfocus/*控件_取是否被激活*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return false;
	return c->IsFocus();
}
/*API*/VOID		WINAPI	UIControl_seteventsound/*控件_设置光标文件*/(IControlPtr* _self/*接口*/, UISoundPlay st/*类型*/, LPCSTR szFile/*文件*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c|| !szFile)return;
	c->SetSoundPlayID((UISoundPlay)st, szFile);
}
/*API*/LPCSTR	WINAPI	UIControl_geteventsound/*控件_取光标名称*/(IControlPtr* _self/*接口*/, UISoundPlay st/*类型*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return "";
	return c->GetSoundPlayID((UISoundPlay)st);
}
//
/*API*/VOID		WINAPI	UIControl_Getgroup/*界面控件_取控件组*/(IControlPtr* _self/*接口*/, IUIGroupPtr* _return/*ret*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	SetIPtr(_return, c->GetGroupPtr(), UIGroup, IUIGroupPtr);
}
/*API*/VOID		WINAPI	UIControl_Setgroup/*界面控件_设置控件组*/(IControlPtr* _self/*接口*/, IUIGroupPtr* g/*ret*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	UIGroup* gr	=	GetIPtr(g, UIGroup, IUIGroupPtr);
	if(!gr)return;
	c->SetGroupPtr(gr);
}

/*API*/VOID		WINAPI	UIControl_Setgroupname/*控件_设置组*/(IControlPtr* _self/*接口*/, LPCSTR szName/*名字*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c|| !szName)
		return;
	UIGroup* g	=	0;
	for(int i=0;i<c->GetDialogPtr()->GetGroupCount();i++)
	{
		if(stricmp(c->GetDialogPtr()->GetGroupPtr(i)->GetName(), szName) == 0)
		{
			c->SetGroupID(c->GetDialogPtr()->GetGroupPtr(i)->GetID());
		}
	}
}

/*API*/LPCSTR	WINAPI	UIControl_Getgroupname/*控件_取组*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)
		return "";
	UIGroup* g	=	0;
	for(int i=0;i<c->GetDialogPtr()->GetGroupCount();i++)
	{
		if(c->GetDialogPtr()->GetGroupPtr(i)->GetID() == c->GetGroupID())
			return  c->GetDialogPtr()->GetGroupPtr(i)->GetName();
	}
	return "";
}


/*API*/VOID		WINAPI	UIGroup_dialog/*界面控件组_是否显示*/(IUIGroupPtr* _self/*接口*/, IDialogPtr* _return/*取父对话框*/)
{
	UIGroup* g	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!g)return;
	SetIPtr(_return, g->m_dialogPtr, UIDialog, IDialogPtr);
}
/*API*/BOOL		WINAPI	UIGroup_Getenabled/*界面控件组_是否有效*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return false;
	return d->GetEnabled();
}
/*API*/VOID		WINAPI	UIGroup_Setenabled/*界面控件组_设置有效*/(IUIGroupPtr* _self/*接口*/, BOOL bEnable/*是否有效*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return;
	d->SetEnabled(bEnable);
}

/*API*/BOOL		WINAPI	UIGroup_Getdisplay/*界面控件组_是否显示*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return false;
	return d->GetVisible();
}
/*API*/VOID		WINAPI	UIGroup_Setdisplay/*界面控件组_设置显示*/(IUIGroupPtr* _self/*接口*/, BOOL bVisible/*是否显示*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return;
	d->SetVisible(bVisible);
}
/*API*/LPCSTR	WINAPI	UIGroup_Getname/*界面控件组_取名字*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return "";
	return d->GetName();
}
/*API*/VOID		WINAPI	UIGroup_Setname/*界面控件组_设置名字*/(IUIGroupPtr* _self/*接口*/, LPCSTR szName/*组名字*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d|| !szName)return;
	d->SetName(szName);
}
/*API*/VOID		WINAPI	UIGroup_clone/*界面控件组_克隆*/(IUIGroupPtr* _self/*接口*/, IUIGroupPtr* _return/*ret*/)
{
	UIGroup* g		=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!g)return;
	UIGroup* to	=	g->GetDialogPtr()->AddGroupPtr();
	to->CopyFrom(g);
	SetIPtr(_return, to, UIGroup, IUIGroupPtr);
}
/*API*/VOID		WINAPI	UIGroup_cloneto/*界面控件组_克隆到其他对话框*/(IUIGroupPtr* _self/*接口*/, IDialogPtr* dest/*目标对话框*/, IUIGroupPtr* _return/*ret*/)
{
	UIGroup* g		=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!g)return;
	UIDialog* d	=	GetIPtr(dest, UIDialog, IDialogPtr);
	if(!d)return;
	UIGroup* to	=	d->AddGroupPtr();
	to->CopyFrom(g);
	SetIPtr(_return, to, UIGroup, IUIGroupPtr);
}
/*API*/VOID		WINAPI	UIGroup_Getcenter/*界面控件组_取得中心位置*/(IUIGroupPtr* _self/*接口*/, Pixel* _return/*ret*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return;
	*_return	=	d->GetPosition();
}
/*API*/VOID		WINAPI	UIGroup_Setcenter/*界面控件组_设置中心位置*/(IUIGroupPtr* _self/*接口*/, const Pixel* pos/*位置*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return;
	d->SetPosition(*(Pixel*)pos);
}
/*API*/INT		WINAPI	UIGroup_count/*界面控件组_取得控件数量*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return 0;
	return d->GetControlCount();
}
/*API*/VOID		WINAPI	UIGroup_search/*界面控件组_查找组内控件指针*/(IUIGroupPtr* _self/*接口*/, LPCSTR szName/*名称*/, IControlPtr* _return/*ret*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d|| !szName)return;
	UIControl* c = d->GetControlFromName(szName);
	if(!c)
	{
		LogInfo("system->warning:[%s.%s:search(\"%s\")]%s. not find in group", d->m_dialogPtr->GetName(), d->GetName(), szName, utf8("组中找不到控件"));
		return;
	}
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, IControlPtr);
}
/*API*/VOID		WINAPI	UIGroup_at/*界面控件组_取得控件*/(IUIGroupPtr* _self/*接口*/, INT nIndex/*索引*/, IControlPtr* _return/*ret*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return;
	UIControl* c	=	d->GetControlPtr(nIndex);
	if(!c)
		return;
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, IControlPtr);
}

/*API*/float	WINAPI	UIGroup_Getspeed/*界面控件组_取得播放速度*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return 0;
	return d->GetPlaySpeed();
}
/*API*/VOID		WINAPI	UIGroup_Setspeed/*界面控件组_设置播放速度*/(IUIGroupPtr* _self/*接口*/, float fSpeed/*速度*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return;
	d->SetPlaySpeed(fSpeed);
}
/*API*/float	WINAPI	UIGroup_Getcurrenttime/*界面控件组_取得播放时间*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return 0;
	return d->GetPlayTime();
}
/*API*/VOID		WINAPI	UIGroup_Setcurrenttime/*界面控件组_设置播放时间*/(IUIGroupPtr* _self/*接口*/, float fTime/*位置*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return;
	d->SetPlayTime(fTime);
}

/*API*/float	WINAPI	UIGroup_Getrot/*界面控件组_取得角度*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return 0;
	return d->GetUnitAngle();
}
/*API*/VOID		WINAPI	UIGroup_Setrot/*界面控件组_设置角度*/(IUIGroupPtr* _self/*接口*/, float fAngle/*角度*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return;
	d->SetUnitAngle(fAngle);
}
/*API*/VOID		WINAPI	UIGroup_setRotDir/*界面控件组_设置角度根据方向*/(IUIGroupPtr* _self/*接口*/, const float2* dir/*角度*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return;
	d->SetUnitAngleDir(*dir);
}
/*API*/ActionType	WINAPI	UIGroup_Getaction/*界面控件组_取播放动作*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return ActionType_None;
	return d->GetPlayAction();
}
/*API*/VOID		WINAPI	UIGroup_Setaction/*界面控件组_设置播放动作*/(IUIGroupPtr* _self/*接口*/, ActionType nType/*动作*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return;
	d->PlayAction(nType);
}
/*API*/LPCSTR	WINAPI	UIGroup_Getactionby/*界面控件组_取播放动作根据名字*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return "";
	return d->m_szPlayAction;
}
/*API*/VOID		WINAPI	UIGroup_Setactionby/*界面控件组_设置播放动作根据名字*/(IUIGroupPtr* _self/*接口*/, LPCSTR szName/*名字*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d|| !szName)return;
	d->PlayAction(szName);
}
UIControl*		FindC(IUIGroupPtr* _self, LPCSTR szControl)
{
	if(!szControl)return 0;
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return 0;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)
	{
		LogInfo("system->warning:[%s.%s:search(\"%s\")]%s. not find in group", d->m_dialogPtr->GetName(), d->GetName(), szControl, utf8("组中找不到控件"));
		return 0;
	}
	return c;
}
/*API*/VOID		WINAPI	UIGroup_setvalt/*界面控件组_设置变换有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szValue/*值*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return;
	c->SetText(szValue);
}
/*API*/VOID		WINAPI	UIGroup_setvali/*界面控件组_设置变换有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, int nValue/*值*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return;
	c->SetIntVal(nValue);
}
/*API*/VOID		WINAPI	UIGroup_setvalf/*界面控件组_设置变换有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, float fValue/*值*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return;
	c->SetDoubleVal(fValue);
}
/*API*/LPCSTR	WINAPI	UIGroup_getvalt/*界面控件组_设置变换有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return "";
	return c->GetText();
}
/*API*/INT		WINAPI	UIGroup_getvali/*界面控件组_设置变换有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return 0;
	return c->GetIntVal();
}
/*API*/float	WINAPI	UIGroup_getvalf/*界面控件组_设置变换有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return 0;
	return c->GetDoubleVal();
}
/*API*/float	WINAPI	UIGroup_Getopacity/*界面控件组_取得透明值*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return 0;
	return d->m_fOpacity;
}
/*API*/VOID		WINAPI	UIGroup_Setopacity/*界面控件组_设置透明值*/(IUIGroupPtr* _self/*接口*/, float fOpacity/*透明值*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return;
	d->m_fOpacity = clamp(fOpacity, 0, 1);
}
/*API*/INT		WINAPI	UIGroup_Getleft/*界面控件组_取得左*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return 0;
	return d->m_movePos.left;
}
/*API*/VOID		WINAPI	UIGroup_Setleft/*界面控件组_设置左*/(IUIGroupPtr* _self/*接口*/, INT nLeft/*左*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return;
	d->SetLeft(nLeft);
}
/*API*/INT		WINAPI	UIGroup_Gettop/*界面控件组_取得上*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return 0;
	return d->m_movePos.top;
}
/*API*/INT		WINAPI	UIGroup_width/*界面控件组_取得宽度*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return 0;
	return d->m_movePos.GetWidth();
}
/*API*/INT		WINAPI	UIGroup_height/*界面控件组_取得高度*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return 0;
	return d->m_movePos.GetHeight();
}
/*API*/VOID		WINAPI	UIGroup_image_mode/*界面控件组_绑定图片模式*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szMode/*图片模式*/, DrawTextAlign align/*对齐*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)
		return;
	c->GetElementPtr(0)->SetTextureMode(szMode, align);
}
/*API*/VOID		WINAPI	UIGroup_Settop/*界面控件组_设置上*/(IUIGroupPtr* _self/*接口*/, INT nTop/*上*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return;
	d->SetTop(nTop);
}
/*API*/VOID		WINAPI	UIGroup_recalc_rect/*界面控件组_重新计算矩形区域*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return;
	d->RecalcRect();
}
/*API*/VOID		WINAPI	UIGroup_Setpos/*界面控件组_设置对话框位置*/(IUIGroupPtr* _self/*接口*/, const Pixel* _val/*矩形*/)
{
	if(!_val)return;
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return;
	d->SetPosition(*_val);
}
/*API*/VOID		WINAPI	UIGroup_Getpos/*界面控件组_取位置左*/(IUIGroupPtr* _self/*接口*/, Pixel* _return/*矩形*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)
		return;
	*_return = d->GetPosition();
}
/*API*/VOID		WINAPI	UIGroup_bind_draw/*界面控件组_绑定渲染队列*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, IUIDrawMgrPtr* drawPtr/*组合*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return;
	UIDrawMgr* ud	=	GetIPtr(drawPtr, UIDrawMgr, IUIDrawMgrPtr);
	if(!ud)return;
	c->AddDrawMgr(ud);
}
/*API*/VOID		WINAPI	UIGroup_bind_image/*界面控件组_绑定图片*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szImageFile/*图片文件路径*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return;
	c->GetElementPtr(0)->SetTexture(szImageFile);
}
/*API*/VOID		WINAPI	UIGroup_bind_eventby/*界面控件组_绑定脚本到按下事件*/(IUIGroupPtr* _self/*接口*/, LPCSTR szEvent/*事件名*/, LPCSTR szControl/*控件名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	if(!szEvent)return;
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return;
	if(szLuaFunc)
		c->SetEventFunc(GetEventID(szEvent), szLuaFunc);
	else
		c->SetEventFunc(GetEventID(szEvent), "");
}
/*API*/VOID		WINAPI	UIGroup_bind_event/*界面控件组_绑定脚本到按下事件*/(IUIGroupPtr* _self/*接口*/, EventID nid/*事件名*/, LPCSTR szControl/*控件名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return;
	if(szLuaFunc)
		c->SetEventFunc(nid, szLuaFunc);
	else
		c->SetEventFunc(nid, "");
}
/*API*/VOID		WINAPI	UIGroup_bind_click/*界面控件组_绑定脚本到事件*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return;
	if(szLuaFunc)
		c->SetEventFunc(EventID_Click, szLuaFunc);
	else
		c->SetEventFunc(EventID_Click, "");
}
/*API*/VOID		WINAPI	UIGroup_show/*界面控件组_显示控件*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, BOOL bShow/*是否显示*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return;
	c->SetVisible(bShow);
}
/*API*/VOID		WINAPI	UIGroup_enable/*界面控件组_让控件有效*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, BOOL bEnable/*是否有效*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return;
	c->SetEnabled(bEnable);
}
/*API*/VOID		WINAPI	UIGroup_playani/*界面控件组_播放控件动画*/(IUIGroupPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szAniName/*动画名称*/)
{
	UIControl* c;
	if(!(c = FindC(_self, szControl)))return;
	c->PlayAction(szAniName);
}


/*API*/INT			WINAPI	UIGroup_timeout/*界面控件_定时引发*/(IUIGroupPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/)//定时引发，只引发一次，毫秒记,脚本无参数,一个函数可以创建多个实例,事件[szLuaFunc(obj)--(obj=UIControl)定期事件,只引发一次];返回TimeID
{
	return g_luamanager.AddTimeout(nMillisecond, szLuaFunc, 0, LuaTimeData::IDT_Group, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIGroup_interval/*界面控件_间隔引发*/(IUIGroupPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用次数*/)//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(tm,endof,obj,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
{
	return g_luamanager.AddInterval(nMillisecond, nMaxMillisecond, szLuaFunc, 0, LuaTimeData::IDT_Group, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIGroup_framemove/*界面控件_循环引发*/(IUIGroupPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/)//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(tm,endof,obj,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
{
	return g_luamanager.AddFramemove(nMaxMillisecond, szLuaFunc, 0, LuaTimeData::IDT_Group, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIGroup_timeout2/*界面控件_定时引发带一个文本参数*/(IUIGroupPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, LPCSTR param/*文本参数*/)//定时引发，只引发一次，毫秒记,代替以前的timeout_1,脚本第一个参数是传入的参数,文本,一个函数可以创建多个实例,事件[szLuaFunc(obj,param)--(obj=UIControl)传入的自定义参数];返回TimeID
{
	return g_luamanager.AddTimeout(nMillisecond, szLuaFunc, param, LuaTimeData::IDT_Group, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIGroup_interval2/*界面控件_间隔引发带一个文本参数*/(IUIGroupPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/)//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,代替以前的interval_1,参数为之有所改动,事件[szLuaFunc(obj,tm,endof,param,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID
{
	return g_luamanager.AddInterval(nMillisecond, nMaxMillisecond, szLuaFunc, param, LuaTimeData::IDT_Group, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIGroup_framemove2/*界面控件_循环引发*/(IUIGroupPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/)//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,第二个参数是传入的参数,文本,一个函数只能创建一个,事件[szLuaFunc(obj,tm,endof,param,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID
{
	return g_luamanager.AddFramemove(nMaxMillisecond, szLuaFunc, param, LuaTimeData::IDT_Group, _self->t, _self->ptr, _self->idx);
}


/*API*/ActionType	WINAPI	UIGroup_GetNextActionType/*界面控件组_取下一个播放动作*/(IUIGroupPtr* _self/*接口*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return ActionType_None;
	return d->GetNextPlayAction();
}
/*API*/VOID		WINAPI	UIGroup_SetNextActionType/*界面控件组_设置下一个播放动作*/(IUIGroupPtr* _self/*接口*/, ActionType nType/*动作*/)
{
	UIGroup* d	=	GetIPtr(_self, UIGroup, IUIGroupPtr);
	if(!d)return;
	d->SetNextPlayAction(nType);
}

/*API*/INT			WINAPI	UIDialog_timeout/*界面控件_定时引发*/(IDialogPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/)//定时引发，只引发一次，毫秒记,脚本无参数,一个函数可以创建多个实例,事件[szLuaFunc(obj)--(obj=UIControl)定期事件,只引发一次];返回TimeID
{
	return g_luamanager.AddTimeout(nMillisecond, szLuaFunc, 0, LuaTimeData::IDT_Dialog, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIDialog_interval/*界面控件_间隔引发*/(IDialogPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用次数*/)//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(tm,endof,obj,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
{
	return g_luamanager.AddInterval(nMillisecond, nMaxMillisecond, szLuaFunc, 0, LuaTimeData::IDT_Dialog, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIDialog_framemove/*界面控件_循环引发*/(IDialogPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/)//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,一个函数只能创建一个实例,事件[szLuaFunc(tm,endof,obj,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm=是最大时间,endof只有在计时器结束的时候被调用一次];返回TimeID
{
	return g_luamanager.AddFramemove(nMaxMillisecond, szLuaFunc, 0, LuaTimeData::IDT_Dialog, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIDialog_timeout2/*界面控件_定时引发带一个文本参数*/(IDialogPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, LPCSTR param/*文本参数*/)//定时引发，只引发一次，毫秒记,代替以前的timeout_1,脚本第一个参数是传入的参数,文本,一个函数可以创建多个实例,事件[szLuaFunc(obj,param)--(obj=UIControl)传入的自定义参数];返回TimeID
{
	return g_luamanager.AddTimeout(nMillisecond, szLuaFunc, param, LuaTimeData::IDT_Dialog, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIDialog_interval2/*界面控件_间隔引发带一个文本参数*/(IDialogPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/)//间隔引发，每间隔一段时间引发一次，会持续引发，毫秒记,代替以前的interval_1,参数为之有所改动,事件[szLuaFunc(obj,tm,endof,param,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID
{
	return g_luamanager.AddInterval(nMillisecond, nMaxMillisecond, szLuaFunc, param, LuaTimeData::IDT_Dialog, _self->t, _self->ptr, _self->idx);
}
/*API*/INT			WINAPI	UIDialog_framemove2/*界面控件_循环引发*/(IDialogPtr* _self/*接口*/, LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR param/*文本参数*/)//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高,脚本第一个参数是时间,按{秒}计算的持续时间,第二个参数是传入的参数,文本,一个函数只能创建一个,事件[szLuaFunc(obj,tm,endof,param,maxtm)--(obj=UIControl,endof=bool,tm=float,maxtm=float)tm是秒计数的小数,maxtm最大时间,endof只有在计时器结束的时候被调用一次,param是自定义传入的参数];返回TimeID
{
	return g_luamanager.AddFramemove(nMaxMillisecond, szLuaFunc, param, LuaTimeData::IDT_Dialog, _self->t, _self->ptr, _self->idx);
}


/*API*/VOID		WINAPI	UIDialog_recalc_rect/*界面对话框_重新计算矩形区域*/(IDialogPtr* _self/*接口*/, BOOL b)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->RecalcRect(b);
}
/*API*/INT		WINAPI	UIDialog_gcount/*界面对话框_取得控件组数量*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return d->GetGroupCount();
}
/*API*/VOID		WINAPI	UIDialog_gat/*界面对话框_取得控件组*/(IDialogPtr* _self/*接口*/, INT nIndex/*索引*/, IUIGroupPtr* _return/*组合*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIGroup* g	=	d->GetGroupPtr(nIndex);
	if(!g)
		return;
	SetIPtr(_return, g, UIGroup, IUIGroupPtr);
}
/*API*/VOID		WINAPI	UIDialog_gsearch/*界面对话框_取得控件组根据名字*/(IDialogPtr* _self/*接口*/, LPCSTR szName/*名字*/, IUIGroupPtr* _return/*组合*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d|| !szName)return;
	for(int i=0;i<d->GetGroupCount();i++)
	{
		UIGroup* g	=	d->GetGroupPtr(i);
		if(stricmp(g->GetName(), szName) == 0)
		{
			SetIPtr(_return, g, UIGroup, IUIGroupPtr);
			break;
		}
	}
}
/*API*/VOID		WINAPI	UIDialog_gsearchbyid/*界面对话框_取得控件组根据名字*/(IDialogPtr* _self/*接口*/, INT nID/*名字*/, IUIGroupPtr* _return/*组合*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d|| !nID)return;
	for(int i=0;i<d->GetGroupCount();i++)
	{
		UIGroup* g	=	d->GetGroupPtr(i);
		if(g->m_nGroupID==nID)
		{
			SetIPtr(_return, g, UIGroup, IUIGroupPtr);
			break;
		}
	}
}
/*API*/VOID				WINAPI	UISlider_Setvalue/*拖动条_设置0_1值*/(ISliderPtr* _self/*接口*/, float value/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, ISliderPtr);
	if(!c)return;
	int nOldMin = 0, nOldMax = 0;
	c->GetRange(nOldMin, nOldMax);
	c->SetValue((int)(value * (float)(nOldMax - nOldMin)) + nOldMin);
}

/*API*/float			WINAPI	UISlider_Getvalue/*拖动条_取0_1值*/(ISliderPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, ISliderPtr);
	if(!c)return 0;
	int nOldMin = 0, nOldMax = 0;
	c->GetRange(nOldMin, nOldMax);
	return (float)(c->GetValue() - nOldMin) / (float)(nOldMax - nOldMin);
}

/*API*/VOID				WINAPI	UISlider_Setmin/*拖动条_取最小值*/(ISliderPtr* _self/*接口*/, INT nMin/*最小值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, ISliderPtr);
	if(!c)return;
	int nOldMin = 0, nOldMax = 0;
	c->GetRange(nOldMin, nOldMax);
	if(nMin >= nOldMax)
		nMin	=	nOldMax - 1;
	c->SetRange(nMin, nOldMax);
}//
/*API*/VOID				WINAPI	UISlider_Setmax/*拖动条_取最大值*/(ISliderPtr* _self/*接口*/, INT nMax/*最大值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, ISliderPtr);
	if(!c)return;
	int nOldMin = 0, nOldMax = 0;
	c->GetRange(nOldMin, nOldMax);
	if(nMax <= nOldMin)
		nMax	=	nOldMin + 1;
	c->SetRange(nOldMin, nMax);
}//

/*API*/INT		WINAPI	UISlider_Getmin/*控件_取最小值*/(ISliderPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, ISliderPtr);
	if(!c)return 0;
	int nMin = 0, nMax = 0;
	c->GetRange(nMin, nMax);
	return nMin;
}//
/*API*/INT		WINAPI	UISlider_Getmax/*控件_取最大值*/(ISliderPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, ISliderPtr);
	if(!c)return 0;
	int nMin = 0, nMax = 0;
	c->GetRange(nMin, nMax);
	return nMax;
}//
/*API*/VOID		WINAPI	UIProgress_Settype/*控件_设置进度条类型*/(IProgressPtr* _self/*接口*/, UIProgressType value/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IProgressPtr);
	if(!c)return;
	c->SetProgressType(value);
}//
/*API*/UIProgressType		WINAPI	UIProgress_Gettype/*控件_取进度条类型*/(IProgressPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IProgressPtr);
	if(!c)return (UIProgressType)0;
	return (UIProgressType)c->GetProgressType();
}//

/*API*/VOID			WINAPI	UIProgress_Setvalue/*控件_设置进度条值*/(IProgressPtr* _self/*接口*/, float value/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IProgressPtr);
	if(!c)return;
	c->SetProgressValue(value);
}
/*API*/float		WINAPI	UIProgress_Getvalue/*控件_取进度条值*/(IProgressPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IProgressPtr);
	if(!c)return 0;
	return c->GetProgressValue();
}

/*API*/VOID			WINAPI	UIControl_dialog/*控件_取对话框*/(IControlPtr* _self/*接口*/, IDialogPtr* _return)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	SetIPtr(_return, c->GetDialogPtr(), UIDialog, IDialogPtr);
}
/*API*/VOID		WINAPI	UIControl_Setgroupid/*界面控件_设置组ID*/(IControlPtr* _self/*接口*/, INT nID/*ID*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetGroupID(nID);
}
/*API*/INT		WINAPI	UIControl_Getgroupid/*界面控件_取组ID*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return -1;
	return c->GetGroupID();
}

/*API*/VOID		WINAPI	UIDialog_gcreate/*界面对话框_创建新组合*/(IDialogPtr* _self/*接口*/, IUIGroupPtr* _return/*组合*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIGroup* g		=	d->AddGroupPtr();
	if(!g)return;
	SetIPtr(_return, g, UIGroup, IUIGroupPtr);
}
/*API*/VOID		WINAPI	UIDialog_gdelete/*界面对话框_删除新组合*/(IDialogPtr* _self/*接口*/, IUIGroupPtr* group/*组合*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIGroup* g	=	GetIPtr(group, UIGroup, IUIGroupPtr);
	if(!g)return;
	PtrID val(0, g->GetID());
	d->RemoveGroupPtr(val);
	memset(group, 0, sizeof(IUIGroupPtr));
}

/*API*/VOID		WINAPI	UIDialog_Sethideon_killfocus/*对话框_设置失去焦点自动隐藏*/(IDialogPtr* _self/*接口*/, BOOL b/*设置*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetHideOnKillFocus(b);
}
/*API*/BOOL		WINAPI	UIDialog_Gethideon_killfocus/*对话框_是否失去焦点自动隐藏*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return false;
	return d->GetHideOnKillFocus();
}
/*API*/VOID		WINAPI	UIDialog_hold/*界面对话框_对话框不被自动删除*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->OnHold(true);
}
/*API*/VOID		WINAPI	UIDialog_deleteall/*对话框_删除所有控件*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->RemoveAllControls();
}

/*API*/INT		WINAPI	UIDialog_count/*对话框_取控件数量*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return d->GetControlCount();
}

/*API*/VOID		WINAPI	UIDialog_pick/*界面对话框_点选控件*/(IDialogPtr* _self/*接口*/, INT x/*x*/, INT y/*y*/, IControlPtr* _return/*ret*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)
		return;
	UIControl* c	=	d->PickControl(Pixel(x, y));
	if(!c)return;
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, IControlPtr);
}

/*API*/VOID		WINAPI	UIDialog_at/*对话框_取控件指针*/(IDialogPtr* _self/*接口*/, INT index/*索引*/, IControlPtr* _return/*ret*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)
		return;
	UIControl* c	=	d->GetControlPtr(index);
	if(!c)return;
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, IControlPtr);
}
/*API*/VOID		WINAPI	UIDialog_delete_at/*对话框_移除控件*/(IDialogPtr* _self/*接口*/, INT index/*索引*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	for(int i=0;i<d->GetControlCount();i++)
	{
		if(d->GetControlPtr(i)->GetID() == index)
		{
			d->RemoveControlPtr(i);
			break;
		}
	}
}
/*API*/VOID		WINAPI	UIDialog_deleteby/*界面对话框_移除控件*/(IDialogPtr* _self/*接口*/, IControlPtr* control/*控件指针*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d|| !control)return;
	UIControl* c	=	GetIPtrT(control, PhantomUIControlBase, IControlPtr);
	if(c)
		d->RemoveControl(c->GetID());
}
/*API*/VOID		WINAPI	UIDialog_searchbyid/*界面对话框_查找控件指针*/(IDialogPtr* _self/*接口*/, INT nID/*名称*/, IControlPtr* _return/*ret*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d|| !nID)return;
	UIControl* c	=	d->GetControlPtrFromID(nID);
	if(!c)
	{
		LogInfo("system->warning:[%s:searchbyid(%d)]%s. not find in dialog.", d->GetName(), nID, utf8("对话框中找不到控件"));
		return;
	}
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, IControlPtr);
}
/*API*/VOID		WINAPI	UIDialog_search/*对话框_查找控件指针*/(IDialogPtr* _self/*接口*/, LPCSTR szName/*名称*/, IControlPtr* _return/*ret*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d|| !szName)return;
	UIControl* c	=	d->GetControlFromName(szName);
	if(!c)
	{
		LogInfo("system->warning:[%s:search(\"%s\")]%s. not find in dialog.", d->GetName(), szName, utf8("对话框中找不到控件"));
		return;
	}
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, IControlPtr);
}

/*API*/VOID		WINAPI	UIDialog_Getcolor/*界面对话框_取对话框颜色透明值*/(IDialogPtr* _self/*接口*/, color4* _return/*返回*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	*_return = d->GetDialogColor();
}

/*API*/VOID		WINAPI	UIDialog_Setcolor/*界面对话框_设置对话框颜色透明值*/(IDialogPtr* _self/*接口*/, const color4* val/*值*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetDialogColor(*val);
}
/*API*/float	WINAPI	UIDialog_Getopacity/*界面对话框_取对话框透明值*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return d->GetTransparent();
}
/*API*/VOID		WINAPI	UIDialog_Setopacity/*界面对话框_设置对话框透明值*/(IDialogPtr* _self/*接口*/, float val/*值*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetTransparent(clamp(val, 0, 1));
}

/*API*/LPCSTR	WINAPI	UIDialog_Getname/*对话框_取名字*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return "";
	return d->GetName();
}
/*API*/VOID		WINAPI	UIDialog_Setname/*对话框_设置名字*/(IDialogPtr* _self/*接口*/, LPCSTR szName/*名字*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d|| !szName)return;
	d->SetName(szName);
}

/*API*/LPCSTR	WINAPI	UIDialog_Getactionby/*界面对话框_取播放动作按照动作名*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return "";
	return d->m_szPlayAction;
}
/*API*/VOID		WINAPI	UIDialog_Setactionby/*界面对话框_设置播放动作按照动作名*/(IDialogPtr* _self/*接口*/, LPCSTR szName/*动作名*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d|| !szName)return;
	d->PlayAction(szName);
}

/*API*/ActionType	WINAPI	UIDialog_Getaction/*界面对话框_取动作*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return ActionType_None;
	return d->GetPlayAction();
}
/*API*/VOID		WINAPI	UIDialog_Setaction/*界面对话框_设置动作*/(IDialogPtr* _self/*接口*/, ActionType nType/*动作*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->PlayAction(nType);
}

/*API*/BOOL		WINAPI	UIDialog_istooltip_window/*对话框_是否是提示对话框*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return false;
	return d->IsTooltipWindow();
}
/*API*/VOID		WINAPI	UIDialog_settooltip_window/*对话框_设置是提示对话框*/(IDialogPtr* _self/*接口*/, BOOL b/*设置*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetTooltipWindow(b == TRUE);
}
/*API*/float	WINAPI	UIDialog_Getscale/*界面对话框_缩放*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return d->m_dlgScale.x;
}
/*API*/VOID		WINAPI	UIDialog_Setscale/*界面对话框_缩放*/(IDialogPtr* _self/*接口*/, float fScale/*缩放*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetWorldScaling(float2(fScale,fScale), d->m_dlgCenter);
}
/*API*/float	WINAPI	UIDialog_Getscalex/*界面对话框_缩放x*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return d->m_dlgScale.x;
}
/*API*/VOID		WINAPI	UIDialog_Setscalex/*界面对话框_缩放x*/(IDialogPtr* _self/*接口*/, float fScale/*缩放*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetWorldScaling(float2(fScale,d->m_dlgScale.y), d->m_dlgCenter);
}
/*API*/float	WINAPI	UIDialog_Getscaley/*界面对话框_缩放y*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return d->m_dlgScale.y;
}
/*API*/VOID		WINAPI	UIDialog_Setscaley/*界面对话框_缩放y*/(IDialogPtr* _self/*接口*/, float fScale/*缩放*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetWorldScaling(float2(d->m_dlgScale.x,fScale), d->m_dlgCenter);
}

/*API*/BOOL		WINAPI	UIDialog_Getdisplay/*对话框_是否显示*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return false;
	return d->GetVisible();
}
/*API*/VOID		WINAPI	UIDialog_Setdisplay/*对话框_显示*/(IDialogPtr* _self/*接口*/, BOOL b/*设置*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetVisible(b == TRUE);
}
/*API*/VOID		WINAPI	UIDialog_Setleft/*界面对话框_设置位置左*/(IDialogPtr* _self/*接口*/, int _val/*值*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetLocation(_val, d->GetTop());
}
/*API*/int		WINAPI	UIDialog_Getleft/*界面对话框_取位置左*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return d->GetLeft();
}
/*API*/VOID		WINAPI	UIDialog_Settop/*界面对话框_设置位置上*/(IDialogPtr* _self/*接口*/, int _val/*值*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetLocation(d->GetLeft(), _val);
}
/*API*/int		WINAPI	UIDialog_Gettop/*界面对话框_取位置上*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return d->GetTop();
}
/*API*/VOID		WINAPI	UIDialog_Setwidth/*界面对话框_设置位置宽*/(IDialogPtr* _self/*接口*/, int _val/*值*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->m_width = _val;//SetSize(_val, d->GetFullscreenHeight());
	d->m_renderRect.right = d->m_renderRect.left + _val;
}
/*API*/int		WINAPI	UIDialog_Getwidth/*界面对话框_取位置宽*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return d->m_width;//GetFullscreenWidth();
}
/*API*/VOID		WINAPI	UIDialog_Setheight/*界面对话框_设置位置高*/(IDialogPtr* _self/*接口*/, int _val/*值*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->m_height = _val;//SetSize(d->GetFullscreenWidth(), _val);
	d->m_renderRect.bottom = d->m_renderRect.top + _val;
}
/*API*/int		WINAPI	UIDialog_Getheight/*界面对话框_取位置高*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return d->m_height;//d->GetHeight();
}

/*API*/VOID		WINAPI	UIDialog_Setpos/*界面对话框_设置对话框位置*/(IDialogPtr* _self/*接口*/, const Pixel* _val/*矩形*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	if(d->m_bIsChildRender)
		d->SetPosition(*_val);
	else
		d->SetLocation(_val->x, _val->y);
}
/*API*/VOID		WINAPI	UIDialog_Getpos/*界面对话框_取位置左*/(IDialogPtr* _self/*接口*/, Pixel* _return/*矩形*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	if(d->m_bIsChildRender)
		*_return = d->GetPosition();
	else
		d->GetLocation(_return->x, _return->y);
}
/*API*/VOID		WINAPI	UIDialog_Setrect/*界面对话框_设置对话框位置*/(IDialogPtr* _self/*接口*/, const Rect* _val/*矩形*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetLocation(_val->left, _val->top);
	d->SetSize(_val->GetWidth(), _val->GetHeight());
}
/*API*/VOID		WINAPI	UIDialog_Getrect/*界面对话框_取位置左*/(IDialogPtr* _self/*接口*/, Rect* _return/*矩形*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	int x,y;
	d->GetLocation(x,y);
	_return->Set(x, y, d->GetWidth(), d->GetHeight());
}

/*API*/VOID		WINAPI	UIDialog_Setcenter/*界面对话框_设置中心点位置*/(IDialogPtr* _self/*接口*/, const Pixel* _val/*矩形*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->m_dlgCenter = *_val;
}
/*API*/VOID		WINAPI	UIDialog_Getcenter/*界面对话框_取中心点位置*/(IDialogPtr* _self/*接口*/, Pixel* _return/*矩形*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	*_return = d->m_dlgCenter;
}

/*API*/VOID		WINAPI	UIDialog_transform/*界面对话框_设置矩阵*/(IDialogPtr* _self/*接口*/, const matrix4x4* _val/*矩形*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	//d->SetWorldMatrixByCenter(*_val);
}

/*API*/BOOL		WINAPI	UIDialog_map_moveable/*界面对话框_是否可以移动到此格子内*/(IDialogPtr* _self/*接口*/, int x, int y)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return false;
	return d->IsAstarMoveEnable(x,y);
}
/*API*/VOID		WINAPI	UIDialog_map_set_moveable/*界面对话框_设置可以移动到此格子内*/(IDialogPtr* _self/*接口*/, int x, int y, BOOL bMoveable)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetAstarMoveEnable2(x,y,bMoveable);
}
/*API*/VOID		WINAPI	UIDialog_map_clear_moveable/*界面对话框_清除所有新增加的可以动格子数据*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->ClearAstarMoveEnable2();
}


/*API*/float	WINAPI	UIDialog_Getrotx/*界面对话框_取转动X*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return Degress(d->m_dlgRotation.x);
}
/*API*/VOID		WINAPI	UIDialog_Setrotx/*界面对话框_设置转动X*/(IDialogPtr* _self/*接口*/, float fAngle/*角度*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetWorldRotationX(Radians(fAngle), d->m_dlgCenter);
}
/*API*/float	WINAPI	UIDialog_Getroty/*界面对话框_取转动Y*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return Degress(d->m_dlgRotation.y);
}
/*API*/VOID		WINAPI	UIDialog_Setroty/*界面对话框_设置转动Y*/(IDialogPtr* _self/*接口*/, float fAngle/*角度*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetWorldRotationY(Radians(fAngle), d->m_dlgCenter);
}
/*API*/float	WINAPI	UIDialog_Getrotz/*界面对话框_取转动Z*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return Degress(d->m_dlgRotation.z);
}
/*API*/VOID		WINAPI	UIDialog_Setrotz/*界面对话框_设置转动Z*/(IDialogPtr* _self/*接口*/, float fAngle/*角度*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetWorldRotationZ(Radians(fAngle), d->m_dlgCenter);
}
/*API*/float	WINAPI	UIDialog_Getrot2d/*界面对话框_取转动X*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d||d->GetControlCount()==0)return 0;
	return d->GetControlPtr(0)->m_fAngle;
}
/*API*/VOID		WINAPI	UIDialog_Setrot2d/*界面对话框_设置转动X*/(IDialogPtr* _self/*接口*/, float fDegress/*角度*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	for(int i=0;i<d->GetControlCount();i++){
		d->GetControlPtr(i)->SetUnitAngle(fDegress);
	}
}
/*API*/VOID		WINAPI	UIDialog_rotdir/*界面对话框_转动2DDir*/(IDialogPtr* _self/*接口*/, Pixel* dir)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	float fDegress = Degress(getRotateZ(float3(dir->x, dir->y, 0)));
	for(int i=0;i<d->GetControlCount();i++){
		d->GetControlPtr(i)->SetUnitAngle(fDegress);
	}
}
/*API*/BOOL		WINAPI	UIDialog_Getenabled/*界面对话框_取有效*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	return d->m_bEnabled;
}
/*API*/VOID		WINAPI	UIDialog_Setenabled/*界面对话框_设置有效*/(IDialogPtr* _self/*接口*/, BOOL bEnable/*是否有效*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->m_bEnabled = bEnable;
}

/*API*/VOID		WINAPI	UIDialog_setvalt/*界面对话框_设置变换有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szValue/*值*/)
{
	if(!szControl || !szValue)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)return;
	c->SetText(szValue);
}
/*API*/VOID		WINAPI	UIDialog_setvali/*界面对话框_设置变换有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, int nValue/*值*/)
{
	if(!szControl)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)return;
	c->SetIntVal(nValue);
}
/*API*/VOID		WINAPI	UIDialog_image_mode/*界面对话框_绑定图片模式*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szMode/*图片模式*/, DrawTextAlign align/*对齐*/)
{
	if(!szControl)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)return;
	c->GetElementPtr(0)->SetTextureMode(szMode, align);
}
/*API*/BOOL		WINAPI	UIDialog_Getpause/*界面对话框_取有效*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return false;
	return d->m_bIsPause;
}
/*API*/VOID		WINAPI	UIDialog_Setpause/*界面对话框_设置有效*/(IDialogPtr* _self/*接口*/, BOOL bPaused/*是否有效*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->m_bIsPause = bPaused;
}

/*API*/VOID		WINAPI	UIDialog_setvalf/*界面对话框_设置变换有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, float fValue/*值*/)
{
	if(!szControl)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)return;
	c->SetDoubleVal(fValue);
}
/*API*/LPCSTR	WINAPI	UIDialog_getvalt/*界面对话框_设置变换有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/)
{
	if(!szControl)return "";
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return "";
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)return "";
	return c->GetText();
}
/*API*/INT		WINAPI	UIDialog_getvali/*界面对话框_设置变换有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/)
{
	if(!szControl)return 0;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)return 0;
	return c->GetIntVal();
}
/*API*/float	WINAPI	UIDialog_getvalf/*界面对话框_设置变换有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/)
{
	if(!szControl)return 0;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)return 0;
	return c->GetDoubleVal();
}

/*API*/VOID		WINAPI	UIDrawMgr_image/*渲染2D_设置图片*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR szTexture/*图片*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->SetTexture(szTexture);
}
/*API*/VOID		WINAPI	UIDrawMgr_image_mode/*渲染2D_设置图片矩形*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR szMode/*图片配置信息*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->SetTextureMode(szMode);
}
/*API*/VOID		WINAPI	UIDrawMgr_image_uvmap/*渲染2D_设置图片矩形*/(IUIDrawMgrPtr* _self/*纹理ID*/, int width/*width*/, int height/*height*/, int wcount/*左右数量*/, int hcount/*上下数量*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->InitUVMap(width,height,wcount,hcount);
}
/*API*/VOID		WINAPI	UIDrawMgr_image_uvani/*渲染2D_设置图片矩形*/(IUIDrawMgrPtr* _self/*纹理ID*/, int start/*开始索引*/, int count/*图片数量*/, float speed/*播放速度*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->InitUVAni(start,count,speed);
}
/*API*/VOID		WINAPI	UIDrawMgr_image_rect/*渲染2D_设置图片矩形*/(IUIDrawMgrPtr* _self/*纹理ID*/, int left/*left*/, int top/*top*/, int width/*right*/, int height/*bottom*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->SetTextureRect(Rect(left, top, left + width, top + height));
}
/*API*/VOID		WINAPI	UIDrawMgr_number_mode/*渲染2D_设置数字渲染模式*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR szModeFirst, int numberHeight/*数字高度*/, int spaceX/*宽度字间隔*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->SetNumberMode(szModeFirst, numberHeight, spaceX);
}
/*API*/VOID		WINAPI	UIDrawMgr_set_number/*渲染2D_设置数字渲染模式*/(IUIDrawMgrPtr* _self/*纹理ID*/, int num/*设置数字*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->SetNumber(num);
}
/*API*/VOID		WINAPI	UIDrawMgr_add_child/*渲染2D_添加子渲染*/(IUIDrawMgrPtr* _self/*纹理ID*/, IUIDrawMgrPtr* _return/*返回*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	UIDrawMgr* s = d->AddChild();
	SetIPtr(_return, s, UIDrawMgr, IUIDrawMgrPtr);
}
/*API*/VOID		WINAPI	UIDrawMgr_delete_child/*渲染2D_添加子渲染*/(IUIDrawMgrPtr* _self/*纹理ID*/, INT nIndex/*索引*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->DeleteChild(nIndex);
}
/*API*/VOID		WINAPI	UIDrawMgr_get_child/*渲染2D_添加子渲染*/(IUIDrawMgrPtr* _self/*纹理ID*/, INT nIndex/*索引*/, IUIDrawMgrPtr* _return/*返回*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	UIDrawMgr* c = d->GetChild(nIndex);
	if(!c)
		return;
	SetIPtr(_return, c, UIDrawMgr, IUIDrawMgrPtr);
}
/*API*/INT		WINAPI	UIDrawMgr_child_count/*渲染2D_子渲染数量*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	return d->GetChildCount();
}
/*API*/VOID		WINAPI	UIDrawMgr_Settext/*渲染2D_设置数字渲染模式*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR text/*设置数字*/)
{
	if(!text)return;
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->SetNumber(0);
	d->m_value = text;
}
/*API*/LPCSTR	WINAPI	UIDrawMgr_Gettext/*渲染2D_返回数字渲染模式*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return "";
	return d->m_value.t;
}

/*API*/VOID		WINAPI	UIDrawMgr_Setnumber/*渲染2D_设置数字渲染模式*/(IUIDrawMgrPtr* _self/*纹理ID*/, int num/*设置数字*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->SetNumber(num);
}
/*API*/INT		WINAPI	UIDrawMgr_Getnumber/*渲染2D_返回数字渲染模式*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	return d->GetNumber();
}
int	GetAlignMode(const char* szT)
{
	if(!szT || (stricmp(szT, "middle") == 0))
		return (Phantom::DrawTextAlign_CENTER | Phantom::DrawTextAlign_VCENTER);
	int ret = 0;
	if(strstr(szT, "left"))
		ret |= Phantom::DrawTextAlign_LEFT;
	if(strstr(szT, "right"))
		ret |= Phantom::DrawTextAlign_RIGHT;
	if(strstr(szT, "top"))
		ret |= Phantom::DrawTextAlign_TOP;
	if(strstr(szT, "bottom"))
		ret |= Phantom::DrawTextAlign_BOTTOM;
	if(strstr(szT, "center"))
		ret |= Phantom::DrawTextAlign_CENTER;
	if(strstr(szT, "vcenter"))
		ret |= Phantom::DrawTextAlign_VCENTER;
	return ret;
}
/*API*/VOID		WINAPI	UIDrawMgr_number_align/*渲染2D_设置数字对齐模式*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR szModeFirst, LPCSTR szAlign/*对齐模式*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_numberAlign = GetAlignMode(szAlign);
}
//
/*API*/VOID		WINAPI	UIDrawMgr_Setrotx/*渲染2D_转动x*/(IUIDrawMgrPtr* _self/*纹理ID*/, int fAngle/*角度*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->SetRotationX(fAngle);
}
/*API*/VOID		WINAPI	UIDrawMgr_Setroty/*渲染2D_转动x*/(IUIDrawMgrPtr* _self/*纹理ID*/, int fAngle/*角度*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->SetRotationY(fAngle);
}
/*API*/VOID		WINAPI	UIDrawMgr_Setrotz/*渲染2D_转动x*/(IUIDrawMgrPtr* _self/*纹理ID*/, int fAngle/*角度*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->SetRotationZ(fAngle);
}
/*API*/int		WINAPI	UIDrawMgr_Getrotx/*渲染2D_转动x*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	return (int)Degress(d->m_rotx);
}
/*API*/int		WINAPI	UIDrawMgr_Getroty/*渲染2D_转动x*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	return (int)Degress(d->m_roty);
}
/*API*/int		WINAPI	UIDrawMgr_Getrotz/*渲染2D_转动x*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	return (int)Degress(d->m_rotz);
}
/*API*/VOID		WINAPI	UIDrawMgr_Sethighlight/*渲染2D_设置高亮*/(IUIDrawMgrPtr* _self/*纹理ID*/, int highlight/*nHighlight*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_nHighlight = highlight;
}
/*API*/int		WINAPI	UIDrawMgr_Gethighlight/*渲染2D_取得高亮*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	return d->m_nHighlight;
}
/*API*/VOID		WINAPI	UIDrawMgr_Setcenter/*渲染2D_设置中心坐标*/(IUIDrawMgrPtr* _self/*纹理ID*/, Pixel* center/*left*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d || !center)return;
	d->m_center = *center;
}
/*API*/VOID		WINAPI	UIDrawMgr_Getcenter/*渲染2D_取得中心坐标*/(IUIDrawMgrPtr* _self/*纹理ID*/, Pixel* _return/*ret*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	*_return = d->m_center;
}
/*API*/VOID		WINAPI	UIDrawMgr_Setpos/*渲染2D_设置位置*/(IUIDrawMgrPtr* _self/*纹理ID*/, Pixel* center/*left*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d || !center)return;
	d->m_pixel = *center;
}
/*API*/VOID		WINAPI	UIDrawMgr_Getpos/*渲染2D_取得位置*/(IUIDrawMgrPtr* _self/*纹理ID*/, Pixel* _return/*ret*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	*_return = d->m_pixel;
}

/*API*/VOID		WINAPI	UIDrawMgr_draw_rect/*渲染2D_取得矩形区域*/(IUIDrawMgrPtr* _self/*纹理ID*/, Rect* _return/*pos*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	*_return = d->m_renderRect;
}

/*API*/VOID		WINAPI	UIDrawMgr_Setlayer/*渲染2D_设置渲染层次*/(IUIDrawMgrPtr* _self/*纹理ID*/, int layer/*layer*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_layer = layer;
}
/*API*/VOID		WINAPI	UIDrawMgr_Setcullmode/*渲染2D_设置渲染面*/(IUIDrawMgrPtr* _self/*纹理ID*/, CullMode layer/*layer*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_cullMode = layer;
}
/*API*/CullMode	WINAPI	UIDrawMgr_Getcullmode/*渲染2D_取得渲染面*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return Phantom::CullMode_UNKNOWN;
	return (CullMode)d->m_cullMode;
}
/*API*/VOID		WINAPI	UIDrawMgr_Setscale/*渲染2D_设置縮放*/(IUIDrawMgrPtr* _self/*纹理ID*/, float fScale/*fScale*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_scale.setxy(fScale, fScale);
}
/*API*/float	WINAPI	UIDrawMgr_Getscale/*渲染2D_取得縮放*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 1;
	return d->m_scale.x;
}
/*API*/VOID		WINAPI	UIDrawMgr_Setscalex/*渲染2D_设置縮放x*/(IUIDrawMgrPtr* _self/*纹理ID*/, float fScalex/*fScalex*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_scale.x = fScalex;
}
/*API*/float	WINAPI	UIDrawMgr_Getscalex/*渲染2D_取得縮放x*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	return d->m_scale.x;
}
/*API*/VOID		WINAPI	UIDrawMgr_Setscaley/*渲染2D_设置縮放y*/(IUIDrawMgrPtr* _self/*纹理ID*/, float fScaley/*fScaley*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_scale.y = fScaley;
}
/*API*/float	WINAPI	UIDrawMgr_Getscaley/*渲染2D_取得縮放y*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 1;
	return d->m_scale.y;
}
/*API*/VOID		WINAPI	UIDrawMgr_Setoffset3d_z/*渲染2D_位移3D坐标Z*/(IUIDrawMgrPtr* _self/*纹理ID*/, float fOffsetZ/*z*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_offsetZ=fOffsetZ;
}
/*API*/float	WINAPI	UIDrawMgr_Getoffset3d_z/*渲染2D_位移3D坐标Z*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	return d->m_offsetZ;
}

/*API*/int		WINAPI	UIDrawMgr_Getlayer/*渲染2D_取得渲染层次*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	return d->m_layer;
}

/*API*/VOID		WINAPI	UIDrawMgr_Setcontext/*渲染2D_设置脚本参数*/(IUIDrawMgrPtr* _self/*纹理ID*/, int context/*contxt*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_data = context;
}
/*API*/int		WINAPI	UIDrawMgr_Getcontext/*渲染2D_取得脚本参数*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	return d->m_data;
}

/*API*/VOID		WINAPI	UIDrawMgr_bind_click/*渲染2D_绑定点击*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR szFunc/*脚本函数*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_dblClick=EventID_Click;
	if(szFunc&&szFunc[0])
		d->m_fnclick = szFunc;
	else
		d->m_fnclick = "";
}

/*API*/VOID		WINAPI	UIDrawMgr_bind_dblclick/*渲染2D_绑定点击*/(IUIDrawMgrPtr* _self/*纹理ID*/, LPCSTR szFunc/*脚本函数*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_dblClick=EventID_DoubleClick;
	if(szFunc&&szFunc[0])
		d->m_fnclick = szFunc;
	else
		d->m_fnclick = "";
}
/*API*/VOID		WINAPI	UIDrawMgr_text_mode/*渲染2D_设置变换有效*/(IUIDrawMgrPtr* _self/*接口*/, LPCSTR szValue/*值*/, float fFontScale/*字体缩放*/)
{
	if(!szValue)return;
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->SetText(szValue);
	d->m_fProgress = fFontScale;
}
/*API*/VOID		WINAPI	UIDrawMgr_progress_mode/*渲染2D_设置变换有效*/(IUIDrawMgrPtr* _self/*接口*/, float fValue/*值*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	fValue = clamp(fValue, 0, 1);
	d->SetProgress(fValue);
}
/*API*/float	WINAPI	UIDrawMgr_progress/*渲染2D_设置变换有效*/(IUIDrawMgrPtr* _self/*接口*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	return d->GetProgress();
}

/*API*/VOID		WINAPI	UIDrawMgr_get_point/*渲染2D_取得点*/(IUIDrawMgrPtr* _self/*纹理ID*/, int index/*点索引*/, Pixel* _return/*y位置*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d || index < 0 || index >= 4)return;
	*_return = d->m_points[index];
}
/*API*/VOID		WINAPI	UIDrawMgr_get_uv/*渲染2D_取得uv*/(IUIDrawMgrPtr* _self/*纹理ID*/, int index/*点索引*/, float2* _return/*v坐标*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d || index < 0 || index >= 4)return;
	*_return = d->m_uv[index];
}
/*API*/VOID		WINAPI	UIDrawMgr_get_color/*渲染2D_取得颜色*/(IUIDrawMgrPtr* _self/*纹理ID*/, int index/*点索引*/, color4* _return/*颜色*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d || index < 0 || index >= 4)return;
	*_return = d->m_color[index];
}

/*API*/VOID		WINAPI	UIDrawMgr_set_rect/*渲染2D_设置矩形*/(IUIDrawMgrPtr* _self/*纹理ID*/, int left/*left*/, int top/*left*/, int right/*right*/, int bottom/*bottom*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->SetRect(left, top, right, bottom);
}
/*API*/VOID		WINAPI	UIDrawMgr_set_point/*渲染2D_设置点*/(IUIDrawMgrPtr* _self/*纹理ID*/, int index/*点索引*/, int x/*x位置*/, int y/*y位置*/)
{
	if(index < 0 || index >= 4)return;
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_points[index].Set(x,y);
}
/*API*/VOID		WINAPI	UIDrawMgr_set_uv/*渲染2D_设置uv*/(IUIDrawMgrPtr* _self/*纹理ID*/, int index/*点索引*/, float u/*u坐标*/, float v/*v坐标*/)
{
	if(index < 0 || index >= 4)return;
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_uv[index].setxy(u,v);
}

/*API*/VOID		WINAPI	UIDrawMgr_Setclickscale/*渲染2D_点击缩放值*/(IUIDrawMgrPtr* _self/*纹理ID*/, float fscale/*点击缩放值*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_clickScale = fscale;
}
/*API*/float	WINAPI	UIDrawMgr_Getclickscale/*渲染2D_点击缩放值*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 1;
	return d->m_clickScale;
}

/*API*/VOID		WINAPI	UIDrawMgr_set_color/*渲染2D_设置颜色*/(IUIDrawMgrPtr* _self/*纹理ID*/, int index/*点索引*/, color4* c/*颜色*/)
{
	if(index < 0 || index >= 4 || !c)return;
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_color[index] = *c;
}

/*API*/VOID		WINAPI	UIDrawMgr_Setcolor/*渲染2D_设置位置*/(IUIDrawMgrPtr* _self/*纹理ID*/, color4* c/*color*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d||!c)return;
	unsigned int cc = *c;
	d->m_color[0] = cc;
	d->m_color[1] = cc;
	d->m_color[2] = cc;
	d->m_color[3] = cc;
}
/*API*/VOID		WINAPI	UIDrawMgr_Getcolor/*渲染2D_取得位置*/(IUIDrawMgrPtr* _self/*纹理ID*/, color4* _return/*ret*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	*_return = d->m_color[0];
}
/*API*/VOID		WINAPI	UIDrawMgr_Setopacity/*渲染2D_设置位置*/(IUIDrawMgrPtr* _self/*纹理ID*/, float fAlpha/*left*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	color4 cc = d->m_color[0];
	cc.a = clamp(fAlpha, 0, 1);
	d->m_color[0] = cc;
	d->m_color[1] = cc;
	d->m_color[2] = cc;
	d->m_color[3] = cc;
}
/*API*/float	WINAPI	UIDrawMgr_Getopacity/*渲染2D_取得位置*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	color4 cc = d->m_color[0];
	return cc.a;
}
/*API*/VOID		WINAPI	UIDrawMgr_Setdisplay/*渲染2D_设置显示*/(IUIDrawMgrPtr* _self/*纹理ID*/, BOOL bDisplay/*bDisplay*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_bVisible = bDisplay;
}
/*API*/BOOL		WINAPI	UIDrawMgr_Getdisplay/*渲染2D_取得显示*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return false;
	return d->m_bVisible;
}
/*API*/VOID		WINAPI	UIDrawMgr_Setenabled/*渲染2D_设置有效*/(IUIDrawMgrPtr* _self/*纹理ID*/, BOOL bEnabled/*bEnabled*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return;
	d->m_bEnabled = bEnabled;
}
/*API*/BOOL		WINAPI	UIDrawMgr_Getenabled/*渲染2D_取得有效*/(IUIDrawMgrPtr* _self/*纹理ID*/)
{
	UIDrawMgr* d	=	GetIPtr(_self, UIDrawMgr, IUIDrawMgrPtr);
	if(!d)return 0;
	return d->m_bEnabled;
}

/*API*/VOID		WINAPI	UIDialog_bind_draw/*界面对话框_绑定脚本到事件*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, IUIDrawMgrPtr* drawPtr/*组合*/)
{
	if(!szControl)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)return;
	UIDrawMgr* ud	=	GetIPtr(drawPtr, UIDrawMgr, IUIDrawMgrPtr);
	if(!ud)return;
	c->AddDrawMgr(ud);
}
/*API*/VOID			WINAPI	Global_draw_create/*全局_创建寻路类*/(IUIDrawMgrPtr* _return/*接口*/)
{
	UIDrawMgr* mgr = new UIDrawMgr();
	CreateIPtr(_return, mgr, UIDrawMgr, IUIDrawMgrPtr);
	g_luamanager.Push(mgr->GetPtrID());
}

/*API*/VOID			WINAPI	UIControl_bind_draw/*界面控件_绑定渲染队列*/(IControlPtr* _self/*接口*/, IUIDrawMgrPtr* drawPtr/*组合*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	UIDrawMgr* ud	=	GetIPtr(drawPtr, UIDrawMgr, IUIDrawMgrPtr);
	if(!ud)return;
	c->AddDrawMgr(ud);
}
/*API*/VOID			WINAPI	UIControl_bind_image/*界面控件_绑定图片*/(IControlPtr* _self/*接口*/, LPCSTR szImageFile/*图片文件路径*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->GetElementPtr(0)->SetTexture(szImageFile);
}
/*API*/VOID			WINAPI	UIControl_bind_eventby/*界面控件_绑定脚本到按下事件*/(IControlPtr* _self/*接口*/, LPCSTR szEvent/*事件名*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	if(!szEvent)return;
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	if(szLuaFunc)
		c->SetEventFunc(GetEventID(szEvent), szLuaFunc);
	else
		c->SetEventFunc(GetEventID(szEvent), "");
}
/*API*/VOID			WINAPI	UIControl_bind_event/*界面控件_绑定脚本到按下事件*/(IControlPtr* _self/*接口*/, EventID nid/*事件名*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	if(szLuaFunc)
		c->SetEventFunc(nid, szLuaFunc);
	else
		c->SetEventFunc(nid, "");
}
/*API*/VOID			WINAPI	UIControl_sort_draws/*界面控件_排序渲染列表*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	if(c->m_drawMgrs.size() > 1)
	{
		qsort(c->m_drawMgrs.GetPtr(), c->m_drawMgrs.size(), sizeof(UIDrawMgr*), CompareDrawMgr);
	}
}
/*API*/VOID			WINAPI	UIControl_Setvalt/*界面控件_设置变换有效*/(IControlPtr* _self/*接口*/, LPCSTR szValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetText(szValue);
}
/*API*/VOID			WINAPI	UIControl_Setvali/*界面控件_设置变换有效*/(IControlPtr* _self/*接口*/, int nValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetIntVal(nValue);
}
/*API*/VOID			WINAPI	UIControl_Setvalf/*界面控件_设置变换有效*/(IControlPtr* _self/*接口*/, float fValue/*值*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetDoubleVal(fValue);
}
/*API*/LPCSTR		WINAPI	UIControl_Getvalt/*界面控件_设置变换有效*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return "";
	return c->GetText();
}
/*API*/INT			WINAPI	UIControl_Getvali/*界面控件_设置变换有效*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetIntVal();
}
/*API*/float		WINAPI	UIControl_Getvalf/*界面控件_设置变换有效*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return 0;
	return c->GetDoubleVal();
}
/*API*/VOID			WINAPI	UIControl_Setpos/*界面控件_设置对话框位置*/(IControlPtr* _self/*接口*/, Pixel* pos)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	c->SetPos(*pos);
}
/*API*/VOID			WINAPI	UIControl_Getpos/*界面控件_取位置左*/(IControlPtr* _self/*接口*/, Pixel* _return/*矩形*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	_return->Set(c->m_windowRect.left + c->m_rotCenter.x, c->m_windowRect.top + c->m_rotCenter.y);
}
/*API*/VOID		WINAPI	UIControl_progress_time/*界面控件_时间内转进度条*/(IControlPtr* _self/*接口*/, float fTimeSecond/*秒*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	if(c->GetType()==Phantom::UIControlType_PROGRESS)
	{
		UIProgress* prog = static_cast<UIProgress*>(c);
		prog->SetVisible(true);
		prog->SetAutoTime(fTimeSecond);
	}
}
/*API*/VOID			WINAPI	UIControl_mousepoint/*界面控件_按照控件对齐的鼠标点*/(IControlPtr* _self/*接口*/, Pixel* _return/*矩形*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	g_manager.SetUIScaleMode(c->m_nAlignMode==0, _return);
	g_manager.SetUIScaleMode(true);
}
/*API*/int			WINAPI	UIControl_Geteffect_loop/*界面控件_特效播放次数*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c || c->GetType() != UIControlType_ui2DEffect)return 0;
	UIEffect* eff = static_cast<UIEffect*>(c);
	return eff->m_loopCount;
}
/*API*/VOID			WINAPI	UIControl_Seteffect_loop/*界面控件_特效播放次数*/(IControlPtr* _self/*接口*/, int nLoop/*播放时间*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c || c->GetType() != UIControlType_ui2DEffect)return;
	UIEffect* eff = static_cast<UIEffect*>(c);
	eff->m_loopCount = nLoop;
}
/*API*/BOOL			WINAPI	UIControl_Geteffect_stop/*界面控件_停止释放粒子*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c || c->GetType() != UIControlType_ui2DEffect)return false;
	UIEffect* eff = static_cast<UIEffect*>(c);
	return eff->m_bStop;
}
/*API*/VOID			WINAPI	UIControl_Seteffect_stop/*界面控件_停止释放粒子*/(IControlPtr* _self/*接口*/, BOOL bStop/*是否停止*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c || c->GetType() != UIControlType_ui2DEffect)return;
	UIEffect* eff = static_cast<UIEffect*>(c);
	eff->m_bStop = bStop;
	if(!eff->m_bStop)
		eff->OnPlayStart();
}

/*API*/VOID			WINAPI	UIControl_bind_click/*界面控件_绑定脚本到事件*/(IControlPtr* _self/*接口*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return;
	if(szLuaFunc)
		c->SetEventFunc(EventID_Click, szLuaFunc);
	else
		c->SetEventFunc(EventID_Click, "");
}

/*API*/VOID		WINAPI	UIDialog_bind_image/*界面对话框_绑定脚本到事件*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szImageFile/*图片文件路径*/)
{
	if(!szControl)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)return;
	c->GetElementPtr(0)->SetTexture(szImageFile);
}
/*API*/VOID		WINAPI	UIDialog_bind_eventby/*界面对话框_绑定脚本到按下事件*/(IDialogPtr* _self/*接口*/,LPCSTR szControl/*控件名字*/,  LPCSTR szEvent/*事件名*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	if(!szEvent)return;
	if(!szControl)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)
	{
		LogInfo("system->warning:[%s:bind_eventby(\"%s\")]%s.control not exist",d->GetName(), szControl, utf8("控件不存在"));
		return;
	}
	if(szLuaFunc)
		c->SetEventFunc(GetEventID(szEvent), szLuaFunc);
	else
		c->SetEventFunc(GetEventID(szEvent), "");
}
/*API*/VOID		WINAPI	UIDialog_bind_event/*界面对话框_绑定脚本到按下事件*/(IDialogPtr* _self/*接口*/,LPCSTR szControl/*控件名字*/,  EventID nid/*事件名*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	if(!szControl)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)
	{
		LogInfo("system->warning:[%s:bind_event(\"%s\")]%s.control not exist",d->GetName(), szControl, utf8("控件不存在"));
		return;
	}
	if(szLuaFunc)
		c->SetEventFunc(nid, szLuaFunc);
	else
		c->SetEventFunc(nid, "");
}
/*API*/VOID		WINAPI	UIDialog_bind_click/*界面对话框_绑定脚本到事件*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	if(!szControl)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)
	{
		LogInfo("system->warning:[%s:bind_click(\"%s\")]%s.control not exist",d->GetName(), szControl, utf8("控件不存在"));
		return;
	}
	if(szLuaFunc)
		c->SetEventFunc(EventID_Click, szLuaFunc);
	else
		c->SetEventFunc(EventID_Click, "");
}

/*API*/VOID		WINAPI	UIDialog_show/*界面对话框_显示控件*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, BOOL bShow/*是否显示*/)
{
	if(!szControl)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)
	{
		UIGroup* g = d->FindGroupByName(szControl);
		if(g)
			g->SetVisible(bShow == 1);
		else
			LogInfo("system->warning:[%s:show(\"%s\")]%s.control not exist",d->GetName(), szControl, utf8("控件不存在"));
		return;
	}
	c->SetVisible(bShow == 1);
}

/*API*/VOID		WINAPI	UIDialog_enable/*界面对话框_让控件有效*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, BOOL bEnable/*是否有效*/)
{
	if(!szControl)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)
	{
		UIGroup* g = d->FindGroupByName(szControl);
		if(g)
			g->SetEnabled(bEnable == 1);
		else
			LogInfo("system->warning:[%s:enable(\"%s\")]%s.control not exist",d->GetName(), szControl, utf8("控件不存在"));
		return;
	}
	c->SetEnabled(bEnable == 1);
}

/*API*/VOID		WINAPI	UIDialog_playani/*界面对话框_播放控件动画*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, LPCSTR szAniName/*动画名称*/)
{
	if(!szControl)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)
	{
		UIGroup* g = d->FindGroupByName(szControl);
		if(g)
			g->PlayAction(szAniName);
		else
			LogInfo("system->warning:[%s:playani(\"%s\")]%s.control not exist",d->GetName(), szControl, utf8("控件不存在"));
		return;
	}
	c->PlayAction(szAniName);
}

/*API*/VOID		WINAPI	UIDialog_move/*界面对话框_移动控件*/(IDialogPtr* _self/*接口*/, LPCSTR szControl/*控件名字*/, INT left/*左*/, INT top/*上*/)
{
	if(!szControl)return;
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = d->GetControlFromName(szControl);
	if(!c)
	{
		UIGroup* g = d->FindGroupByName(szControl);
		if(g){
			g->SetLeft(left);
			g->SetTop(top);
		}
		else
			LogInfo("system->warning:[%s:move(\"%s\")]%s.control not exist",d->GetName(), szControl, utf8("控件不存在"));
		return;
	}
	Rect rc = c->GetRenderRect();
	int w = rc.GetWidth();
	int h = rc.GetHeight();
	rc.Set(left, top, left + w, top + h);
	c->SetRenderRect(rc);
}


/*API*/VOID		WINAPI	UIDialog_Settopmost/*对话框_设置成最顶层*/(IDialogPtr* _self/*接口*/, BOOL bTopMost)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	GetDlgManager()->m_topDialog = d;
	GetDlgManager()->m_bResortDialogs = true;
//	d->SetTopMost(bTopMost);
}
/*API*/BOOL		WINAPI	UIDialog_Gettopmost/*界面对话框_设置成最顶层*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return false;
	return (GetDlgManager()->m_topDialog==d);//d->IsTopMost();
}
/*API*/BOOL		WINAPI	UIDialog_Getdomodal/*界面对话框_有模式显示*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return false;
	return d->IsDoModal();
}

/*API*/VOID		WINAPI	UIDialog_Setdomodal/*对话框_有模式显示*/(IDialogPtr* _self/*接口*/, BOOL bModel/*是否有模式*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->DoModal(bModel == TRUE);
}
/*API*/INT		WINAPI	UIDialog_getmap_gridlevel/*界面对话框_取格子级别*/(IDialogPtr* _self/*接口*/, INT xx/*x*/, INT yy/*y*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	int x	=	xx / d->GetMapGridSize().cx;
	int y	=	yy / d->GetMapGridSize().cy;
	const Map2DGridInfo* info = d->GetMapGridInfo(x, y);
	//if(info)
	//	return info->index;
	return 0;
}
/*API*/VOID		WINAPI	UIDialog_setmap_gridlevel/*界面对话框_设置格子级别*/(IDialogPtr* _self/*接口*/, INT xx/*x*/, INT yy/*y*/, INT n/*设置level*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	int x	=	xx / d->GetMapGridSize().cx;
	int y	=	yy / d->GetMapGridSize().cy;
	const Map2DGridInfo* info = d->GetMapGridInfo(x, y);
	//if(info)
	//{
	//	Map2DGridInfo in = *info;
	//	in.index	=	n;
	//	d->SetMapGridInfo(x, y, in);
	//}
}
/*API*/INT		WINAPI	UIDialog_getmap_gridflags/*界面对话框_取格子状态*/(IDialogPtr* _self/*接口*/, INT xx/*x*/, INT yy/*y*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return 0;
	int x	=	xx / d->GetMapGridSize().cx;
	int y	=	yy / d->GetMapGridSize().cy;
	const Map2DGridInfo* info = d->GetMapGridInfo(x, y);
	if(info)
		return info->flags;
	return 0;
}
/*API*/VOID		WINAPI	UIDialog_setmap_gridflags/*界面对话框_设置格子状态*/(IDialogPtr* _self/*接口*/, INT xx/*x*/, INT yy/*y*/, INT n/*设置level*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	int x	=	xx / d->GetMapGridSize().cx;
	int y	=	yy / d->GetMapGridSize().cy;
	const Map2DGridInfo* info = d->GetMapGridInfo(x, y);
	if(info)
	{
		Map2DGridInfo in = *info;
		in.flags	=	n;
		d->SetMapGridInfo(x, y, in);
	}
}
/*API*/BOOL		WINAPI	UIDialog_Getmap_display/*界面对话框_是否显示地形格子*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return false;
	return d->GetMapGridVisible();
}
/*API*/VOID		WINAPI	UIDialog_Setmap_display/*界面对话框_显示地形格子*/(IDialogPtr* _self/*接口*/, BOOL b/*设置*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->SetMapGridVisible(b);
}


/*API*/BOOL		WINAPI	UIDialog_destroy/*对话框_释放*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return false;
	ReleaseIPtr(_self, UIDialog, IDialogPtr);
	return true;
}

/*API*/VOID		WINAPI	UIDialog_create/*界面对话框_创建控件*/(IDialogPtr* _self/*接口*/, UIControlType type/*界面类型*/,  const Rect* rc/*创建区域*/,  IControlPtr* _return/*ret*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	UIControl* c = 0;
	switch(type)
	{
	case UIControlType_BUTTON:
		c = d->AddButton(true, "", rc->left, rc->top, rc->GetWidth(), rc->GetHeight());
		break;
	case UIControlType_CHECKBOX:
		c = d->AddCheckBox(true, "", rc->left, rc->top, rc->GetWidth(), rc->GetHeight());
		break;
	case UIControlType_COMBOBOX:
		c = d->AddComboBox(true, rc->left, rc->top, rc->GetWidth(), rc->GetHeight());
		break;
	case UIControlType_SLIDER:
		c = d->AddSlider(true, rc->left, rc->top, rc->GetWidth(), rc->GetHeight());
		break;
	case UIControlType_LISTBOX:
		c = d->AddListBox(true, rc->left, rc->top, rc->GetWidth(), rc->GetHeight());
		break;
	case UIControlType_SCROLLBAR:
		c = d->AddScrollBar(true, rc->left, rc->top, rc->GetWidth(), rc->GetHeight());
		break;
	case UIControlType_PROGRESS:
		c = d->AddProgress(true, rc->left, rc->top, rc->GetWidth(), rc->GetHeight());
		break;
	case UIControlType_STATIC:
		c = d->AddStatic(true, "", rc->left, rc->top, rc->GetWidth(), rc->GetHeight());
		break;
	case UIControlType_VIEW_FORMAT_TEXT:
		c = d->AddFormatStatic(true, "", rc->left, rc->top, rc->GetWidth(), rc->GetHeight());
		break;
	case UIControlType_LISTCTRL3D:
		c = d->AddListCtrl(true, rc->left, rc->top, rc->GetWidth(), rc->GetHeight());
		break;
	case UIControlType_ui2DEffect:
		c = d->AddEffect(true, rc->left, rc->top, rc->GetWidth(), rc->GetHeight());
		break;
	}
	if(!c)
		return;
	SetIPtr(_return, &c->m_this, PhantomUIControlBase, IControlPtr);
}
/*API*/BOOL		WINAPI	UIDialog_map_searchway/*界面对话框_执行寻路*/(IDialogPtr* _self/*接口*/, IFindWayPtr* fw/*寻路接口*/, const Pixel* from/*开始*/, const Pixel* to/*目标*/)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	WayPoints* findWay	=	GetIPtr(fw, WayPoints, IFindWayPtr);
	if(!d || !findWay)return false;
	return d->CreateWay(findWay, Pixel(from->x, from->y), Pixel(to->x, to->y));
}
/*API*/BOOL		WINAPI	UIDialog_map_searchway2/*界面对话框_执行寻路2*/(IDialogPtr* _self/*接口*/, IFindWayPtr* fw/*寻路接口*/, int fareax, int fareay, int tareax, int tareay)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	WayPoints* findWay	=	GetIPtr(fw, WayPoints, IFindWayPtr);
	if(!d || !findWay)return false;
	return d->CreateWay(findWay, fareax,fareay,tareax,tareay);
}
/*API*/VOID		WINAPI	UIDialog_map_gridsize/*界面对话框_取地图格子大小*/(IDialogPtr* _self/*接口*/, Pixel* _return/*矩形*/)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	_return->Set(d->m_mapGridSize.cx, d->m_mapGridSize.cy);
}
/*API*/VOID		WINAPI	UIDialog_map_count/*界面对话框_取地图方块数量*/(IDialogPtr* _self/*接口*/, Pixel* _return/*矩形*/)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	_return->Set(d->m_mapGridCount.cx, d->m_mapGridCount.cy);
}
/*API*/VOID		WINAPI	UIDialog_map_center/*界面对话框_取某个地图坐标方块的中心位置*/(IDialogPtr* _self/*接口*/, Pixel* pos/*矩形*/, Pixel* _return/*矩形*/)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	*_return = d->GetMapGridCenter(pos->x, pos->y);
}
/*API*/VOID		WINAPI	UIDialog_map_areaindex/*界面对话框_取某个索引的寻路坐标*/(IDialogPtr* _self/*接口*/, int x, int y, Pixel* _return/*矩形*/)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	*_return = d->MapToAstarMap(x, y);
}
/*API*/VOID		WINAPI	UIDialog_map_mapindex/*界面对话框_取某个寻路索引的地图坐标*/(IDialogPtr* _self/*接口*/, int x, int y, Pixel* _return/*矩形*/)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	*_return = d->AstarMapToMap(x, y);
}
/*API*/VOID		WINAPI	UIDialog_map_pickmap/*界面对话框_点击一个位置获得地图坐标*/(IDialogPtr* _self/*接口*/, Pixel* pos, Pixel* _return/*矩形*/)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	*_return = d->GetMapGridIndex(*pos);
}
/*API*/VOID		WINAPI	UIDialog_map_pickarea/*界面对话框_点击一个位置获得寻路坐标*/(IDialogPtr* _self/*接口*/, Pixel* pos, Pixel* _return/*矩形*/)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	Pixel temp = d->GetMapGridIndex(Pixel(pos->x, pos->y));
	*_return = d->MapToAstarMap(temp.x, temp.y);
}
/*API*/VOID		WINAPI	UIDialog_map_add2dchild/*界面对话框_添加一个子对话框*/(IDialogPtr* _self/*接口*/, IDialogPtr* child/*子对话框*/)//子对话框中组中心对象坐标作为中心点进行排序，作为2D对象排序
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->Add2DChild(GetIPtr(child, UIDialog, IDialogPtr));
}
/*API*/VOID		WINAPI	UIDialog_map_clear2dchild/*界面对话框_清除所有的子对象*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->ClearChilds();
}
/*API*/VOID		WINAPI	UIDialog_map_beginmoveto/*界面对话框_自动移动寻路*/(IDialogPtr* _self/*接口*/, float fSpeed, LPCSTR szLuaFunc/*脚本函数名*/)//按照寻路数据自己移动,事件[szLuaFunc(obj,flerp,endof)--(obj=UIDialog,endof=bool,flerp=float),endof只有在移动结束的时候被调用一次
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->BeginAutoMove(fSpeed, szLuaFunc);
}
/*API*/VOID		WINAPI	UIDialog_map_addfindway/*界面对话框_添加新寻路数据*/(IDialogPtr* _self/*接口*/, IFindWayPtr* fw/*寻路接口*/)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	WayPoints* findWay	=	GetIPtr(fw, WayPoints, IFindWayPtr);
	if(!d||!findWay)return;
	d->AddMoveTo(findWay);
}
/*API*/VOID		WINAPI	UIDialog_map_addmoveto/*界面对话框_添加新移动点*/(IDialogPtr* _self/*接口*/, Pixel* fw)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->AddMoveTo(*fw);
}
/*API*/VOID		WINAPI	UIDialog_map_stopmove/*界面对话框_结束自动移动*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d		=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)return;
	d->StopAutoMove();
}

/*API*/INT		WINAPI	FindWay_Getpointcount/*寻路_取路经点数量*/(IFindWayPtr* _self/*接口*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return 0;
	return findWay->GetWayCount();
}

/*API*/VOID		WINAPI	FindWay_Setpointcount/*寻路_设置路经点数量*/(IFindWayPtr* _self/*接口*/, INT nCount/*路径点*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return;
	findWay->SetWayCount(nCount);
}

/*API*/BOOL		WINAPI	FindWay_Getis2dway/*寻路_取2D模式*/(IFindWayPtr* _self/*接口*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return false;
	return findWay->GetIs2DMode();
}

/*API*/VOID		WINAPI	FindWay_Setis2dway/*寻路_设置2D模式*/(IFindWayPtr* _self/*接口*/, BOOL bMode/*2D模式*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return;
	findWay->SetIs2DMode(bMode);
}

/*API*/VOID		WINAPI	FindWay_point_at/*寻路_取路经点*/(IFindWayPtr* _self/*接口*/, INT nIndex/*索引*/, float3* pos/*点*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return;
	const float3* v = findWay->GetWayPoint(nIndex);
	if(v)
		*pos	=	*v;
}

/*API*/VOID		WINAPI	FindWay_setpoint_at/*寻路_设置路经点*/(IFindWayPtr* _self/*接口*/, INT nIndex/*索引*/, const float3* pos/*点*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return;
	findWay->SetWayPoint(nIndex, *pos);
}

/*API*/BOOL		WINAPI	FindWay_Getplay/*寻路_是否播放*/(IFindWayPtr* _self/*接口*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return false;
	return findWay->IsPlaying();
}

/*API*/VOID		WINAPI	FindWay_Setplay/*寻路_设置播放*/(IFindWayPtr* _self/*接口*/, BOOL bPlay/*播放*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return;
	if(bPlay)
		findWay->Play();
	else
		findWay->Stop();
}

/*API*/BOOL		WINAPI	FindWay_Getpause/*寻路_是否暂停*/(IFindWayPtr* _self/*接口*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return 0;
	return findWay->IsPause();
}

/*API*/VOID		WINAPI	FindWay_Setpause/*寻路_设置暂停*/(IFindWayPtr* _self/*接口*/, BOOL bPause/*暂停*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return;
	findWay->Pause(bPause);
}

/*API*/float	WINAPI	FindWay_Getspeed/*寻路_取得速度*/(IFindWayPtr* _self/*接口*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return 0;
	return findWay->GetMoveSpeed();
}

/*API*/VOID		WINAPI	FindWay_Setspeed/*寻路_设置速度*/(IFindWayPtr* _self/*接口*/, float fSpeed/*速度*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return;
	findWay->SetMoveSpeed(fSpeed);
}

/*API*/BOOL		WINAPI	FindWay_recalc/*寻路_更新时间*/(IFindWayPtr* _self/*接口*/, float3* ret/*返回最新位置*/, floatPtr* retRotation)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return 0;
	return findWay->FrameMove(ret, &retRotation->value);
}

/*API*/VOID			WINAPI	Global_findway_create/*系统_创建寻路类*/(IFindWayPtr* _return/*接口*/)
{
	WayPoints* findWay	=	CreateWayPoints();
	const char* szName	=	0;
	safe_addref(findWay);
	CreateIPtr(_return, findWay, WayPoints, IFindWayPtr);
	g_luamanager.Push(findWay->GetPtrID());
}
/*API*/VOID			WINAPI	Global_findway_delete/*系统_删除寻路类*/(IFindWayPtr* _self/*接口*/)
{
	WayPoints* findWay	=	GetIPtr(_self, WayPoints, IFindWayPtr);
	if(!findWay)
		return;
	g_luamanager.Pop(findWay->GetPtrID());
	delete findWay;
}
/*API*/VOID			WINAPI	Global_map3d_load/*全局_地图3d_载入*/(LPCSTR szName/*m2x1文件路径*/, IMap3DPtr* _return/*取渲染组ID*/)
{
	Scene3D* map3d = 0;
	Scene3DPtr ret;
	if(!g_manager.LoadScene3D(ret, szName))//	if(!map3d->LoadScene(buf))
	{
		safe_release(map3d);
		return;
	}
	map3d = ret;
	safe_addref(map3d);
	g_luamanager.Push(map3d->GetPtrID());
	CreateIPtr(_return, map3d, Scene3D, IMap3DPtr);
}
/*API*/VOID			WINAPI	Global_map3d_delete/*全局_地图3d_释放*/(IMap3DPtr* mesh/*渲染组ID*/)
{
	Scene3D* scene		=	GetIPtr(mesh, Scene3D, IMap3DPtr);
	if(scene){
		safe_release(scene);
	}
}
/*API*/VOID			WINAPI	Global_dlg_create/*系统_创建空对话框*/(IDialogPtr* _return/*返回*/)
{
	Dialog d;
	if(!GetDlgManager()->CreateDlg(d))
		return;
	UIDialog* dlg	=	d;
	safe_addref(dlg);
	CreateIPtr(_return, dlg, UIDialog, IDialogPtr);
	g_luamanager.Push(dlg->GetPtrID());
}
/*API*/VOID			WINAPI	Global_dlg_delete/*系统_删除对话框*/(IDialogPtr* _dlg/*对话框*/)
{
	UIDialog* d	=	GetIPtr(_dlg, UIDialog, IDialogPtr);
	if(!d)
		return;
	g_luamanager.Pop(d->GetPtrID());
	safe_release(d);
}

/*API*/VOID			WINAPI	Global_dlg_load/*系统_载入对话框仅一次*/(LPCSTR szFileName/*文件名*/, IDialogPtr* _return/*返回*/)
{
	if(!szFileName)
	{
		LogInfo("system->error:dlg_load(nil)文件名不能为空");
		return;
	}
	const char* szFile = GetLanguagePtr()->GetDlgFile(szFileName);
	InitMyFile(szFile, true, false);
	Dialog d;
	int nRet = GetDlgManager()->LoadDialog(d, szFile, 0, 0, false);
	if(nRet == 0)
		return;
	if(nRet == 1){
		UIDialog* dlg	=	d;
		safe_addref(dlg);
		CreateIPtr(_return, dlg, UIDialog, IDialogPtr);
		g_luamanager.Push(dlg->GetPtrID());
	}
	else
	{
		UIDialog* dlg	=	d;
		SetIPtr(_return, dlg, UIDialog, IDialogPtr);
	}
}
	/*API*/VOID			WINAPI	Global_dlg_loadAndProgress/*游戏_载入对话框仅一次使用进度*/(LPCSTR szFileName/*文件名*/, IDialogPtr* _return/*返回*/)
	{
		if(!szFileName)
		{
			LogInfo("system->error:dlg_load(nil)文件名不能为空");
			return;
		}
		const char* szFile = GetLanguagePtr()->GetDlgFile(szFileName);
		InitMyFile(szFile, true, false);
		Dialog d;
		g_manager.m_bUseThread = true;
		int nRet = GetDlgManager()->LoadDialog(d, szFile, 0, 0, false);
		g_manager.m_bUseThread = false;
		GetGameMgr()->m_loadCount = GetGameMgr()->m_loadlist.size();
		if(nRet == 0)
			return;
		if(nRet == 1){
			UIDialog* dlg	=	d;
			safe_addref(dlg);
			CreateIPtr(_return, dlg, UIDialog, IDialogPtr);
			g_luamanager.Push(dlg->GetPtrID());
		}
		else
		{
			UIDialog* dlg	=	d;
			SetIPtr(_return, dlg, UIDialog, IDialogPtr);
		}
	}

/*API*/VOID			WINAPI	Global_dlg_loadnew/*系统_载入新的对话框*/(LPCSTR szFileName/*文件名*/, IDialogPtr* _return/*返回*/)
{
	if(!szFileName)return;
	Dialog d;
	if(!GetDlgManager()->LoadDialog(d, GetLanguagePtr()->GetDlgFile(szFileName), 0, 0, true))
		return;
	UIDialog* dlg	=	d;
	safe_addref(dlg);
	CreateIPtr(_return, dlg, UIDialog, IDialogPtr);
	g_luamanager.Push(dlg->GetPtrID());
}
/*API*/VOID			WINAPI	Global_dlg_search/*系统_搜索对话框*/(LPCSTR szDlgName/*文件名*/, IDialogPtr* _return/*返回*/)
{
	DefineIPtr(IDialogPtr);
	if(!szDlgName)
		return;
	UIDialog* dlg	=	0;
	for(int i=0;i<GetDlgManager()->GetDialogManager().size();i++)
	{
		if(stricmp(GetDlgManager()->GetDialogManager()[i]->GetName(), szDlgName) == 0)
		{
			dlg = GetDlgManager()->GetDialogManager()[i];
			break;
		}
	}
	if(!dlg)return;
	SetIPtr(_return, dlg, UIDialog, IDialogPtr);
}
/*API*/VOID			WINAPI	Global_dlg_at/*系统_返回索引对话框*/(int index, IDialogPtr* _return/*返回*/)
{
	UIDialog* dlg	=	GetDlgManager()->GetDialogManager()[index];
	DefineIPtr(IDialogPtr);
	if(!dlg)return;
	SetIPtr(_return, dlg, UIDialog, IDialogPtr);
}
/*API*/INT			WINAPI	Global_dlg_count/*系统_返回对话框个数*/()
{
	return GetDlgManager()->GetDialogManager().size();
}
/*API*/VOID			WINAPI	DataControl_clear/*数据控制_清除所有的数据*/(IDataControlPtr* _self/*自身*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);
	if(!data)return;
	data->Destroy();
	data->OnChanged();
}
/*API*/VOID			WINAPI	DataControl_add_child/*数据控制_创建子数据*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项子数据名字*/, IDataControlPtr*_return/*返回*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);
	if(!data)return;
	GameState* g = data->addChild(name);
	SetIPtr(_return, g, GameState, IDataControlPtr);
}
/*API*/LPCSTR		WINAPI	DataControl_readt/*数据控制_从配置文件读取一个文本*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项名字*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);
	if(!data)return "";
	return data->readt(name, "");
}
/*API*/double		WINAPI	DataControl_readf/*数据控制_从配置文件读取一个浮点数*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项名字*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);
	if(!data)return 0;
	return data->readf(name, 0);
}
/*API*/int			WINAPI	DataControl_readi/*数据控制_从配置文件读取一个整数*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项名字*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);
	if(!data)return 0;
	return data->readi(name, 0);
}
/*API*/VOID			WINAPI	DataControl_writet/*数据控制_写入一个文本到配置文件*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项名字*/, LPCSTR szValue/*值*/)
{
	if(!szValue)return;
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);
	if(!data)return;
	data->write(name, szValue);
}
/*API*/VOID			WINAPI	DataControl_writef/*数据控制_写入一个浮点到配置文件*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项名字*/, double fValue/*值*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);
	if(!data)return;
	data->writef(name, fValue);
}
/*API*/VOID			WINAPI	DataControl_writei/*数据控制_写入一个整数到配置文件*/(IDataControlPtr* _self/*自身*/, LPCSTR name/*配置项名字*/, INT nValue/*值*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);
	if(!data)return;
	data->writei(name, nValue);
}
/*API*/INT			WINAPI	DataControl_count/*数据控制_子数据数量*/(IDataControlPtr* _self/*自身*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);
	if(!data)return 0;
	return data->m_childs.size();
}
/*API*/VOID			WINAPI	DataControl_at/*数据控制_子数据指针*/(IDataControlPtr* _self/*自身*/, INT index/*配置项子数据名字*/, IDataControlPtr*_return/*返回*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);
	if(!data)return;
	if(index<0||index>=data->m_childs.size())return;
	GameState* c = data->m_childs[index];
	SetIPtr(_return, c, GameState, IDataControlPtr);
}
/*API*/INT			WINAPI	DataControl_Getlcount/*数据控制_读取数组长度*/(IDataControlPtr* _self/*自身*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);
	if(!data)return 0;
	return data->m_arrays.size();
}
/*API*/VOID			WINAPI	DataControl_Setlcount/*数据控制_设置数组长度*/(IDataControlPtr* _self/*自身*/, INT nCount/*值*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);
	if(!data)return;
	data->SetArrayCount(nCount);
}
/*API*/LPCSTR		WINAPI	DataControl_lreadt/*数据控制_读取数组文本*/(IDataControlPtr* _self/*自身*/, INT index/*索引*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);	
	if(!data)return "";
	return data->lreadt(index, "");
}
/*API*/double		WINAPI	DataControl_lreadf/*数据控制_读取数组浮点数*/(IDataControlPtr* _self/*自身*/, INT index/*索引*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);	
	if(!data)return 0;
	return data->lreadf(index, 0);
}
/*API*/int			WINAPI	DataControl_lreadi/*数据控制_读取数组整数*/(IDataControlPtr* _self/*自身*/, INT index/*索引*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);	
	if(!data)return 0;
	return data->lreadi(index, 0);
}
/*API*/VOID			WINAPI	DataControl_lwritet/*数据控制_写入数组文本*/(IDataControlPtr* _self/*自身*/, INT index/*索引*/, LPCSTR szValue/*值*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);	
	if(!data)return;
	data->lwritet(index, szValue);
}
/*API*/VOID			WINAPI	DataControl_lwritef/*数据控制_写入数组浮点*/(IDataControlPtr* _self/*自身*/, INT index/*索引*/, double fValue/*值*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);	
	if(!data)return;
	data->lwritef(index, fValue);
}
/*API*/VOID			WINAPI	DataControl_lwritei/*数据控制_写入数组整数*/(IDataControlPtr* _self/*自身*/, INT index/*索引*/, INT nValue/*值*/)
{
	GameState* data = GetIPtr(_self, GameState, IDataControlPtr);	
	if(!data)return;
	data->lwritei(index, nValue);
}

/*API*/VOID			WINAPI	Global_valdata/*全局_返回内存变量的类*/(LPCSTR name/*配置项子数据名字*/, IDataControlPtr*_return/*返回*/)
{
	GameState* g = g_luamanager.m_memvals;
	SetIPtr(_return, g, GameState, IDataControlPtr);
}
/*API*/BOOL			WINAPI	Global_isval/*全局_从内存查找变量是否存在*/(LPCSTR name/*配置项名字*/)
{
	return (g_luamanager.m_memvals->find(name)>=0);
}
/*API*/BOOL			WINAPI	Global_isconfig/*全局_从配置文件查找变量是否存在*/(LPCSTR name/*配置项名字*/)
{
	return (GetStateMgr()->find(name)>=0);
}
/*API*/VOID			WINAPI	Global_config/*全局_创建配置文件子数据*/(LPCSTR name/*配置项子数据名字*/, IDataControlPtr*_return/*返回*/)
{
	GameState* g = GetStateMgr();
	g = g->addChild(name);
	SetIPtr(_return, g, GameState, IDataControlPtr);
}
/*API*/LPCSTR		WINAPI	Global_readt/*系统_从配置文件读取一个文本*/(LPCSTR name/*配置项名字*/)
{
	if(!name)return "";
	return GetStateMgr()->readt(name, "");
}
/*API*/double		WINAPI	Global_readf/*系统_从配置文件读取一个浮点数*/(LPCSTR name/*配置项名字*/)
{
	if(!name)return 0;
	return GetStateMgr()->readf(name, 0);
}
/*API*/int			WINAPI	Global_readi/*系统_从配置文件读取一个整数*/(LPCSTR name/*配置项名字*/)
{
	if(!name)return 0;
	return GetStateMgr()->readi(name, 0);
}
/*API*/VOID			WINAPI	Global_writet/*系统_写入一个文本到配置文件*/(LPCSTR name/*配置项名字*/, LPCSTR szValue/*值*/)
{
	if(!name || !szValue)return;
	GetStateMgr()->write(name, szValue);
}
/*API*/VOID			WINAPI	Global_writef/*系统_写入一个浮点到配置文件*/(LPCSTR name/*配置项名字*/, double fValue/*值*/)
{
	if(!name)return;
	GetStateMgr()->writef(name, fValue);
}
/*API*/VOID			WINAPI	Global_writei/*系统_写入一个整数到配置文件*/(LPCSTR name/*配置项名字*/, INT nValue/*值*/)
{
	if(!name)return;
	GetStateMgr()->writei(name, nValue);
}
/*API*/VOID			WINAPI	float2_setup/*二维向量_设置*/(float2* _self/*返回*/, float x, float y)
{
	_self->x	=	x;
	_self->y	=	y;
}

/*API*/VOID			WINAPI	float3_setup/*二维向量_设置*/(float3* _self/*返回*/, float x, float y, float z)
{
	_self->x	=	x;
	_self->y	=	y;
	_self->z	=	z;
}

/*API*/VOID			WINAPI	float4_setup/*二维向量_设置*/(float4* _self/*返回*/, float x, float y, float z, float w)
{
	_self->x	=	x;
	_self->y	=	y;
	_self->z	=	z;
	_self->w	=	w;
}

/*API*/VOID			WINAPI	color4_setup/*二维向量_设置*/(color4* _self/*返回*/, float r, float g, float b, float a)
{
	_self->r	=	r;
	_self->g	=	g;
	_self->b	=	b;
	_self->a	=	a;
}
/*API*/INT			WINAPI	Global_rgba/*_rgba*/(INT r, INT g, INT b, INT a)
{
	return (RGB(b,g,r) | (((DWORD)((BYTE)a)) << 24));
}

/*API*/INT			WINAPI	Global_rgbaf/*_rgba_F*/(float r, float g, float b, float a)
{
	return color4(r,g,b,a);
}

///////////////////////数学库
/*API*/VOID			WINAPI	float2_Add/*二维向量_加*/(float2* ret/*返回*/, const float2* v1/*参数1*/, const float2* v2/*参数2*/)
{
	float2& _ret	=	*(float2*)ret;
	float2& _v1		=	*(float2*)v1;
	float2& _v2		=	*(float2*)v2;
	_ret	=	_v1 + _v2;
}
//ret = v1 + v2
/*API*/VOID			WINAPI	float2_Subtract/*二维向量_减*/(float2* ret/*返回*/, const float2* v1/*参数1*/, const float2* v2/*参数2*/)
{
	float2& _ret	=	*(float2*)ret;
	float2& _v1		=	*(float2*)v1;
	float2& _v2		=	*(float2*)v2;
	_ret			=	_v1 - _v2;
}
//ret = v1 - v2
/*API*/VOID			WINAPI	float2_Mul/*二维向量_乘*/(float2* ret/*返回*/, const float2* v1/*参数1*/, const float2* v2/*参数2*/)
{
	float2& _ret	=	*(float2*)ret;
	float2& _v1		=	*(float2*)v1;
	float2& _v2		=	*(float2*)v2;
	_ret			=	_v1 * _v2;
}
//ret = v1 * v2
/*API*/VOID			WINAPI	float2_Div/*二维向量_除*/(float2* ret/*返回*/, const float2* v1/*参数1*/, const float2* v2/*参数2*/)
{
	float2& _ret	=	*(float2*)ret;
	float2& _v1		=	*(float2*)v1;
	float2& _v2		=	*(float2*)v2;
	_ret			=	_v1 / _v2;
}
//ret = v1 / v2
/*API*/float		WINAPI	float2_length/*二维向量_长度*/(float2* v1/*参数1*/)
{
	return ((float2*)v1)->length();
}
//返回向量长度
/*API*/float		WINAPI	float2_dot/*二维向量_点乘*/(float2* v1/*参数1*/, const float2* v2/*参数2*/)
{
	float2& _v1		=	*(float2*)v1;
	float2& _v2		=	*(float2*)v2;
	return _v1.dot(_v2);
}
//返回向量点乘
/*API*/float		WINAPI	float2_cross/*二维向量_叉乘*/(const float2* v1/*参数1*/, const float2* v2/*参数2*/)
{
	float2& _v1		=	*(float2*)v1;
	float2& _v2		=	*(float2*)v2;
	return _v1.cross(_v2);
}
//返回向量叉乘
/*API*/VOID			WINAPI	float2_normalize/*二维向量_归一*/(float2* v1/*参数1*/)
{
	((float2*)v1)->normalize();
}
//向量归一
/*API*/VOID			WINAPI	float2_lerp/*二维向量_插值*/(float2* ret/*返回*/, const float2* v1/*参数1*/, const float2* v2/*参数2*/, float fLerp/*变量*/)
{
	float2& _ret	=	*(float2*)ret;
	float2& _v1		=	*(float2*)v1;
	float2& _v2		=	*(float2*)v2;
	_v1.lerp(_ret, _v2, fLerp);
}
//向量插值，变量0~1之间
/*API*/VOID			WINAPI	float2_copy/*二维向量_拷贝*/(float2* ret/*返回*/, const float2* v1/*参数1*/)
{
	*ret	=	*v1;
}
//向量拷贝
/*API*/VOID			WINAPI	float2_hermite/*二维向量_曲线插值*/( float2 *pOut, const float2 *pV1, const float2 *pT1,const float2 *pV2, const float2 *pT2, float s )
{
	vec2Hermite((float2*)pOut, (float2*)pV1, (float2*)pT1, (float2*)pV2, (float2*)pT2, s);
}
// Hermite interpolation between position V1, tangent T1 (when s == 0// and position V2, tangent T2 (when s == 1).
/*API*/VOID			WINAPI	float2_catmullrom( float2 *pOut, const float2 *pV0, const float2 *pV1,const float2 *pV2, const float2 *pV3, float s )
{
	vec2CatmullRom((float2*)pOut, (float2*)pV0, (float2*)pV1, (float2*)pV2, (float2*)pV3, s);
}
// CatmullRom interpolation between V1 (when s == 0) and V2 (when s == 1)
/*API*/VOID			WINAPI	float2_barycentric( float2 *pOut, const float2 *pV1, const float2 *pV2,const float2 *pV3, float f, float g)
{
	vec2BaryCentric((float2*)pOut, (float2*)pV1, (float2*)pV2, (float2*)pV3, f, g);
}
// Barycentric coordinates.  V1 + f(V2-V1) + g(V3-V1)
/*API*/VOID			WINAPI	float2_transformcoord( float2 *pOut, const float2 *pV, const matrix4x4 *pM )
{
	vec2TransformCoord((float2*)pOut, (float2*)pV, (matrix4x4*)pM);
}
// Transform (x, y, 0, 1) by matrix, project result back into w=1.
/*API*/VOID			WINAPI	float2_transformnormal( float2 *pOut, const float2 *pV, const matrix4x4 *pM )
{
	vec2TransformNormal((float2*)pOut, (float2*)pV, (matrix4x4*)pM);
}
// Transform (x, y, 0, 0) by matrix.
/*API*/VOID			WINAPI	float2_TransformCoordArray( float2 *pOut, unsigned int OutStride, const float2 *pV, unsigned int VStride, const matrix4x4 *pM, unsigned int n )
{
	vec2TransformCoordArray((float2*)pOut, OutStride, (float2*)pV, VStride, (matrix4x4*)pM, n);
}
// Transform Array (x, y, 0, 1) by matrix, project result back into w=1.
/*API*/VOID			WINAPI	float2_TransformNormalArray( float2 *pOut, unsigned int OutStride, const float2 *pV, unsigned int VStride, const matrix4x4 *pM, unsigned int n )
{
	vec2TransformNormalArray((float2*)pOut, OutStride, (float2*)pV, VStride, (matrix4x4*)pM, n);
}
//
/*API*/VOID			WINAPI	float3_Add/*三维向量_加*/(float3* ret/*返回*/, const float3* v1/*参数1*/, const float3* v2/*参数2*/)
{
	float3& _ret	=	*ret;
	const float3& _v1		=	*v1;
	const float3& _v2		=	*v2;
	_ret			=	_v1 + _v2;
}
//ret = v1 + v2
/*API*/VOID			WINAPI	float3_Subtract/*三维向量_减*/(float3* ret/*返回*/, const float3* v1/*参数1*/, const float3* v2/*参数2*/)
{
	float3& _ret	=	*ret;
	const float3& _v1		=	*v1;
	const float3& _v2		=	*v2;
	_ret			=	_v1 - _v2;
}
//ret = v1 - v2
/*API*/VOID			WINAPI	float3_Mul/*三维向量_乘*/(float3* ret/*返回*/, const float3* v1/*参数1*/, const float3* v2/*参数2*/)
{
	float3& _ret	=	*ret;
	const float3& _v1		=	*v1;
	const float3& _v2		=	*v2;
	_ret			=	_v1 * _v2;
}
//ret = v1 * v2
/*API*/VOID			WINAPI	float3_Div/*三维向量_除*/(float3* ret/*返回*/, const float3* v1/*参数1*/, const float3* v2/*参数2*/)
{
	float3& _ret	=	*ret;
	const float3& _v1		=	*v1;
	const float3& _v2		=	*v2;
	_ret			=	_v1 / _v2;
}
//ret = v1 / v2
/*API*/float		WINAPI	float3_length/*三维向量_长度*/(float3* v1/*参数1*/)
{
	return ((float3*)v1)->length();
}
//返回向量长度
/*API*/float		WINAPI	float3_dot/*三维向量_点乘*/(const float3* v1/*参数1*/, const float3* v2/*参数2*/)
{
	const float3& _v1		=	*v1;
	const float3& _v2		=	*v2;
	return _v1.dot(_v2);
}
//返回向量点乘
/*API*/VOID			WINAPI	float3_cross/*三维向量_叉乘*/(float3* ret/*返回*/, const float3* v1/*参数1*/, const float3* v2/*参数2*/)
{
	float3& _ret	=	*ret;
	const float3& _v1		=	*v1;
	const float3& _v2		=	*v2;
	_ret = _v1.cross(_v2);
}
//返回向量叉乘
/*API*/VOID			WINAPI	float3_normalize/*三维向量_归一*/(float3* v1/*参数1*/)
{
	((float3*)v1)->normalize();
}
//向量归一
/*API*/VOID			WINAPI	float3_lerp/*三维向量_插值*/(float3* ret/*返回*/, const float3* v1/*参数1*/, const float3* v2/*参数2*/, float fLerp/*变量*/)
{
	float3& _ret	=	*ret;
	const float3& _v1		=	*v1;
	const float3& _v2		=	*v2;
	_v1.lerp(_ret, _v2, fLerp);
}
//向量插值，变量0~1之间
/*API*/VOID			WINAPI	float3_Copy/*三维向量_拷贝*/(float3* ret/*返回*/, const float3* v1/*参数1*/)
{
	*ret	=	*v1;
}
//向量拷贝
/*API*/VOID			WINAPI	float3_transform/*三维向量_矩阵变换*/(float3* v1/*返回*/, const float3* v2/*参数1*/, const matrix4x4* m/*矩阵1*/)
{
	((float3*)v2)->transform(*v1, *(matrix4x4*)m);
}
//向量与矩阵进行变换
/*API*/VOID			WINAPI	float3_transform3x3/*三维向量_矩阵旋转变换*/(float3* v1/*返回*/, const float3* v2/*参数1*/, const matrix4x4* m/*矩阵1*/)
{
	((float3*)v2)->transformNoMove(*v1, *(matrix4x4*)m);
}
//向量与矩阵进行变换
/*API*/VOID			WINAPI	float3_rotateX/*三维向量_转动X轴*/(float3* v1/*返回*/, const float3* v2/*参数1*/, float rot/*转动率*/)
{
	((float3*)v2)->RotateX(*v1, Radians(rot));
}
//向量随X轴转动
/*API*/VOID			WINAPI	float3_rotateY/*三维向量_转动Y轴*/(float3* v1/*返回*/, const float3* v2/*参数1*/, float rot/*转动率*/)
{
	((float3*)v2)->RotateY(*v1, Radians(rot));
}
//向量随Y轴转动
/*API*/VOID			WINAPI	float3_rotateZ/*三维向量_转动Z轴*/(float3* v1/*返回*/, const float3* v2/*参数1*/, float rot/*转动率*/)
{
	((float3*)v2)->RotateZ(*v1, Radians(rot));
}
	//得到z的转动
/*API*/VOID		 	WINAPI	float3_hermite/*三维向量_曲线插值*/( float3 *pOut, const float3 *pV1, const float3 *pT1,const float3 *pV2, const float3 *pT2, float s )
{
	vec3Hermite((float3*)pOut, (float3*)pV1, (float3*)pT1, (float3*)pV2, (float3*)pT2, s);
}
// Hermite interpolation between position V1, tangent T1 (when s == 0)// and position V2, tangent T2 (when s == 1).
/*API*/VOID			WINAPI	float3_catmullrom( float3 *pOut, const float3 *pV0, const float3 *pV1,const float3 *pV2, const float3 *pV3, float s )
{
	vec3CatmullRom((float3*)pOut, (float3*)pV0, (float3*)pV1, (float3*)pV2, (float3*)pV3, s);
}
// CatmullRom interpolation between V1 (when s == 0) and V2 (when s == 1)
/*API*/VOID			WINAPI	float3_barycentric( float3 *pOut, const float3 *pV1, const float3 *pV2,const float3 *pV3, float f, float g)
{
	vec3BaryCentric((float3*)pOut, (float3*)pV1, (float3*)pV2, (float3*)pV3, f, g);
}
// Barycentric coordinates.  V1 + f(V2-V1) + g(V3-V1)
/*API*/VOID			WINAPI	float3_transformcoord( float3 *pOut, const float3 *pV, const matrix4x4 *pM )
{
	vec3TransformCoord((float3*)pOut, (float3*)pV, (matrix4x4*)pM);
}
// Transform (x, y, z, 1) by matrix, project result back into w=1.
/*API*/VOID			WINAPI	float3_transformnormal( float3 *pOut, const float3 *pV, const matrix4x4 *pM )
{
	vec3TransformNormal((float3*)pOut, (float3*)pV, (matrix4x4*)pM);
}
// Transform (x, y, z, 0) by matrix.  If you transforming a normal by a // non-affine matrix, the matrix you pass to this function should be the // transpose of the inverse of the matrix you would use to transform a coord.
/*API*/VOID			WINAPI	float3_TransformCoordArray( float3 *pOut, unsigned int OutStride, const float3 *pV, unsigned int VStride, const matrix4x4 *pM, unsigned int n )
{
	vec3TransformCoordArray((float3*)pOut, OutStride, (float3*)pV, VStride, (matrix4x4*)pM, n);
}
// Transform Array (x, y, z, 1) by matrix, project result back into w=1.
/*API*/VOID			WINAPI	float3_TransformNormalArray( float3 *pOut, unsigned int OutStride, const float3 *pV, unsigned int VStride, const matrix4x4 *pM, unsigned int n )
{
	vec3TransformNormalArray((float3*)pOut, OutStride, (float3*)pV, VStride, (matrix4x4*)pM, n);
}
// Transform (x, y, z, 0) by matrix.  If you transforming a normal by a // non-affine matrix, the matrix you pass to this function should be the // transpose of the inverse of the matrix you would use to transform a coord.
//
/*API*/VOID			WINAPI	float4_Add/*四维向量_加*/(float4* ret/*返回*/, const float4* v1/*参数1*/, const float4* v2/*参数2*/)//ret = v1 + v2
{
	float4& _ret	=	*(float4*)ret;
	float4& _v1		=	*(float4*)v1;
	float4& _v2		=	*(float4*)v2;
	_ret			=	_v1 + _v2;
}

/*API*/VOID			WINAPI	float4_Subtract/*四维向量_减*/(float4* ret/*返回*/, const float4* v1/*参数1*/, const float4* v2/*参数2*/)//ret = v1 - v2
{
	float4& _ret	=	*(float4*)ret;
	float4& _v1		=	*(float4*)v1;
	float4& _v2		=	*(float4*)v2;
	_ret			=	_v1 - _v2;
}
/*API*/VOID			WINAPI	float4_Mul/*四维向量_乘*/(float4* ret/*返回*/, const float4* v1/*参数1*/, const float4* v2/*参数2*/)//ret = v1 * v2
{
	float4& _ret	=	*(float4*)ret;
	float4& _v1		=	*(float4*)v1;
	float4& _v2		=	*(float4*)v2;
	_ret			=	_v1 * _v2;
}
/*API*/VOID			WINAPI	float4_Div/*四维向量_除*/(float4* ret/*返回*/, const float4* v1/*参数1*/, const float4* v2/*参数2*/)//ret = v1 / v2
{
	float4& _ret	=	*(float4*)ret;
	float4& _v1		=	*(float4*)v1;
	float4& _v2		=	*(float4*)v2;
	_ret			=	_v1 - _v2;
}
/*API*/float		WINAPI	float4_length/*四维向量_长度*/(float4* v1/*参数1*/)//返回向量长度
{
	return ((float4*)v1)->length();
}
/*API*/float		WINAPI	float4_dot/*四维向量_点积*/(const float4* v1/*参数1*/, const float4* v2/*参数2*/)//返回向量点乘
{
	return ((float4*)v1)->dot(*(float4*)v2);
}

/*API*/VOID			WINAPI	float4_lerp/*四维向量_插值*/(float4* ret/*返回*/, const float4* v1/*参数1*/, const float4* v2/*参数2*/, float fLerp/*变量*/)//向量插值，变量0~1之间
{
	float4& _ret	=	*(float4*)ret;
	float4& _v1		=	*(float4*)v1;
	float4& _v2		=	*(float4*)v2;
	_v1.lerp(_ret, _v2, fLerp);
}
/*API*/VOID			WINAPI	float4_copy/*四维向量_拷贝*/(float4* ret/*返回*/, const float4* v1/*参数1*/)//向量拷贝
{
	*ret	=	*v1;
}
/*API*/VOID		 	WINAPI	float4_hermite/*四维向量_曲线插值*/( float4 *pOut, const float4 *pV1, const float4 *pT1,const float4 *pV2, const float4 *pT2, float s )// Hermite interpolation between position V1, tangent T1 (when s == 0)// and position V2, tangent T2 (when s == 1).
{
	vec4Hermite((float4*)pOut, (float4*)pV1, (float4*)pT1, (float4*)pV2, (float4*)pT2, s);
}
/*API*/VOID			WINAPI	float4_catmullrom/*四维向量_Catmull_Rom插值*/( float4 *pOut, const float4 *pV0, const float4 *pV1,const float4 *pV2, const float4 *pV3, float s )// CatmullRom interpolation between V1 (when s == 0) and V2 (when s == 1)
{
	vec4CatmullRom((float4*)pOut, (float4*)pV0, (float4*)pV1, (float4*)pV2, (float4*)pV3, s);
}
/*API*/VOID			WINAPI	float4_barycentric/*四维向量_重心*/( float4 *pOut, const float4 *pV1, const float4 *pV2,const float4 *pV3, float f, float g)// Barycentric coordinates.  V1 + f(V2-V1) + g(V3-V1)
{
	vec4BaryCentric((float4*)pOut, (float4*)pV1, (float4*)pV2, (float4*)pV3, f, g);
}
/*API*/VOID			WINAPI	float4_transform/*四维向量_单位化*/( float4 *pOut, const float3 *pV, const matrix4x4 *pM )// Transform (x, y, z, 0) by matrix.  If you transforming a normal by a // non-affine matrix, the matrix you pass to this function should be the // transpose of the inverse of the matrix you would use to transform a coord.
{
	vec4Transform((float4*)pOut, (float4*)pV, (matrix4x4*)pM);
}
/*API*/VOID			WINAPI	float4_TransformArray/*四维向量_单位化一组*/( float4 *pOut, unsigned int OutStride, const float4 *pV, unsigned int VStride, const matrix4x4 *pM, unsigned int n )// Transform (x, y, z, 0) by matrix.  If you transforming a normal by a // non-affine matrix, the matrix you pass to this function should be the // transpose of the inverse of the matrix you would use to transform a coord.
{
	vec4TransformArray((float4*)pOut, OutStride, (float4*)pV, VStride, (matrix4x4*)pM, n);
}

/*API*/INT			WINAPI	color4_rgb/*颜色_返回RGB色*/(color4* c/*颜色*/)
{
	return ((color4*)c)->getRGB();
}

/*API*/VOID			WINAPI	color4_fromrgb/*颜色_设置RGB色*/(color4* _self/*颜色*/, INT r/*R颜色*/, INT g/*G颜色*/, INT b/*B颜色*/)
{
	_self->from(RGB(r,g,b), _self->a);
}
/*API*/VOID			WINAPI	color4_setAlpha/*颜色_设置透明*/(color4* _self/*返回*/, float fAlpha/*透明*/)
{
	_self->a = fAlpha;
}
/*API*/float		WINAPI	color4_getAlpha/*颜色_取透明*/(color4* _self/*返回*/)
{
	return _self->a;
}

/*API*/INT			WINAPI	color4_Getrgba/*颜色_返回ARGB色*/(color4* c/*颜色*/)
{
	return *((color4*)c);
}

//
/*API*/VOID			WINAPI	color4_fromrgba/*颜色_从RGB色*/(color4* c/*返回*/, INT rgb/*RGB颜色*/, float fAlpha/*透明*/)
{
	((color4*)c)->from(rgb, fAlpha);
}
//
/*API*/VOID			WINAPI	color4_Setrgba/*颜色_从BGRA色*/(color4* c/*返回*/, INT bgra/*BGRA颜色*/)
{
	*((color4*)c)	=	bgra;
}

/*API*/VOID			WINAPI	color4_set/*颜色_设置*/(color4* ret/*返回*/, float r, float g, float b, float a)
{
	((color4*)ret)->setRGBA(r,g,b,a);
}

//
/*API*/VOID			WINAPI	matrix4x4_Setpos/*矩阵_设置位置*/(matrix4x4* m4/*矩阵*/, const float3* v)
{
	m4->SetPosition(*v);
}
//
/*API*/VOID			WINAPI	matrix4x4_Getpos/*矩阵_取得位置*/(const matrix4x4* m4/*矩阵*/, float3* ret/*返回*/)
{
	*ret	=	m4->GetPosition();
}
//
/*API*/VOID			WINAPI	matrix4x4_identity/*矩阵_设置单位矩阵*/(matrix4x4* m4/*指针*/)
{
	m4->identity();
}
//
/*API*/VOID			WINAPI	matrix4x4_lerp/*矩阵_插值*/(matrix4x4* m4/*返回*/, const matrix4x4* v1/*参数1*/, const matrix4x4* v2/*参数2*/, float fLerp/*变量*/)
{
	matrixLerp(m4, v1, v2, fLerp);
}
//矩阵插值，变量0~1之间
/*API*/VOID			WINAPI	matrix4x4_Add/*矩阵_加*/(matrix4x4* ret/*返回*/, const matrix4x4* v1/*参数1*/, const matrix4x4* v2/*参数2*/)
{
	matrix4x4& _ret		=	*(matrix4x4*)ret;
	matrix4x4& _v1		=	*(matrix4x4*)v1;
	matrix4x4& _v2		=	*(matrix4x4*)v2;
	_ret				=	_v1 + _v2;
}
//ret = v1 + v2
/*API*/VOID			WINAPI	matrix4x4_Subtract/*矩阵_减*/(matrix4x4* ret/*返回*/, const matrix4x4* v1/*参数1*/, const matrix4x4* v2/*参数2*/)
{
	matrix4x4& _ret		=	*(matrix4x4*)ret;
	matrix4x4& _v1		=	*(matrix4x4*)v1;
	matrix4x4& _v2		=	*(matrix4x4*)v2;
	_ret				=	_v1 - _v2;
}
//ret = v1 - v2
/*API*/VOID			WINAPI	matrix4x4_Mul/*矩阵_乘*/(matrix4x4* ret/*返回*/, const matrix4x4* v1/*参数1*/, const matrix4x4* v2/*参数2*/)
{
	matrix4x4& _ret		=	*(matrix4x4*)ret;
	matrix4x4& _v1		=	*(matrix4x4*)v1;
	matrix4x4& _v2		=	*(matrix4x4*)v2;
	_ret				=	_v1 * _v2;
}
//ret = v1 * v2
/*API*/VOID			WINAPI	matrix4x4_Div/*矩阵_除*/(matrix4x4* ret/*返回*/, const matrix4x4* v1/*参数1*/, float v2/*参数2*/)
{
	matrix4x4& _ret		=	*(matrix4x4*)ret;
	matrix4x4& _v1		=	*(matrix4x4*)v1;
	_ret				=	_v1 / v2;
}
//ret = v1 / v2
//
/*API*/VOID		 	WINAPI	quaternion_rotationmatrix( quaternion *pOut, const matrix4x4 *pM)
{
	qtnRotationMatrix((quaternion*)pOut, (matrix4x4*)pM);
}
// Build a quaternion from a rotation matrix.
/*API*/VOID		 	WINAPI	quaternion_rotationaxis( quaternion *pOut, const float3 *pV, float Angle )
{
	qtnRotationAxis((quaternion*)pOut, (float3*)pV, Radians(Angle));
}
// Rotation about arbitrary axis.
/*API*/VOID		 	WINAPI	quaternion_rotation_yawpitchroll( quaternion *pOut, float Yaw, float Pitch, float Roll )
{
	qtnRotationYawPitchRoll((quaternion*)pOut, Radians(Yaw), Radians(Pitch), Radians(Roll));
}
// Yaw around the Y axis, a pitch around the X axis,// and a roll around the Z axis.
/*API*/VOID		 	WINAPI	quaternion_multiply( quaternion *pOut, const quaternion *pQ1,const quaternion *pQ2 )
{
	qtnMultiply((quaternion*)pOut, (quaternion*)pQ1, (quaternion*)pQ2);
}
// Quaternion multiplication.  The result represents the rotation Q2// followed by the rotation Q1.  (Out = Q2 * Q1)
/*API*/VOID		 	WINAPI	quaternion_normalize( quaternion *pOut, const quaternion *pQ )
{
	qtnNormalize((quaternion*)pOut, (quaternion*)pQ);
}
// Conjugate and re-norm
/*API*/VOID		 	WINAPI	quaternion_inverse( quaternion *pOut, const quaternion *pQ )
{
	qtnInverse((quaternion*)pOut, (quaternion*)pQ);
}
/*API*/VOID		 	WINAPI	quaternion_ln( quaternion *pOut, const quaternion *pQ )
{
	qtnLn((quaternion*)pOut, (quaternion*)pQ);
}

/*API*/VOID		 	WINAPI	quaternion_exp( quaternion *pOut, const quaternion *pQ )
{
	qtnExp((quaternion*)pOut, (quaternion*)pQ);
}

/*API*/VOID		 	WINAPI	quaternion_slerp( quaternion *pOut, const quaternion *pQ1,const quaternion *pQ2, float t )
{
	qtnSlerp((quaternion*)pOut, (quaternion*)pQ1, (quaternion*)pQ2, t);
}
// Spherical linear interpolation between Q1 (t == 0) and Q2 (t == 1).// Expects unit quaternions.
/*API*/VOID		 	WINAPI	quaternion_squad( quaternion *pOut, const quaternion *pQ1,const quaternion *pA, const quaternion *pB,const quaternion *pC, float t )
{
	qtnSquad((quaternion*)pOut, (quaternion*)pQ1, (quaternion*)pA, (quaternion*)pB, (quaternion*)pC, t);
}
// Setup control points for spherical quadrangle interpolation// from Q1 to Q2.  The control points are chosen in such a way // to ensure the continuity of tangents with adjacent segments.
/*API*/VOID		 	WINAPI	quaternion_barycentric( quaternion *pOut, const quaternion *pQ1,const quaternion *pQ2, const quaternion *pQ3,float f, float g )
{
	qtnBaryCentric((quaternion*)pOut, (quaternion*)pQ1, (quaternion*)pQ2, (quaternion*)pQ3, f, g);
}
// Barycentric interpolation.// Slerp(Slerp(Q1, Q2, f+g), Slerp(Q1, Q3, f+g), g/(f+g))
//
/*API*/VOID		 	WINAPI	Plane_normalize( Plane *pOut, const Plane *pP)
{
	planeNormalize(pOut, pP);
}
// Normalize Plane (so that |a,b,c| == 1)
/*API*/VOID		 	WINAPI	Plane_intersectline( float3 *pOut, const Plane *pP, const float3 *pV1,const float3 *pV2)
{
	planeIntersectLine((float3*)pOut, pP, (float3*)pV1, (float3*)pV2);
}
// Find the intersection between a Plane and a line.  If the line is// parallel to the Plane, NULL is returned.
/*API*/VOID		 	WINAPI	Plane_from_pointnormal( Plane *pOut, const float3 *pPoint, const float3 *pNormal)
{
	planeFromPointNormal(pOut, (float3*)pPoint, (float3*)pNormal);
}
// Construct a Plane from a point and a normal
/*API*/VOID		 	WINAPI	Plane_from_points( Plane *pOut, const float3 *pV1, const float3 *pV2,const float3 *pV3)
{
	planeFromPoints(pOut, (float3*)pV1, (float3*)pV2, (float3*)pV3);
}
// Construct a Plane from 3 points
/*API*/VOID		 	WINAPI	Plane_transform( Plane *pOut, const Plane *pP, const matrix4x4 *pM )
{
	planeTransform(pOut, pP, (matrix4x4*)pM);
}

/*API*/float		WINAPI	Plane_dot(const Plane* p1, const float4* p2)// ax + by + cz + dw平面方程与4D向量的点乘
{
	return planeDot(p1, (float4*)p2);
}
/*API*/float		WINAPI	Plane_dotcoord(const Plane* p1, const float3* p2)// ax + by + cz + d平面与3D向量的点乘,w=1
{
	return planeDotCoord(p1, (float3*)p2);
}

/*API*/float		WINAPI	Plane_dotnormal(const Plane* p1, const float3* p2)// ax + by + cz平面与3D向量的点乘,w=0
{
	return planeDotNormal(p1, (float3*)p2);
}

/*API*/VOID			WINAPI	Plane_scale(Plane* pOut, const Plane* p1, float s)
{
	planeScale(pOut, p1, s);
}

// Transform a Plane by a matrix.  The vector (a,b,c) must be normal.// M should be the inverse transpose of the transformation desired.
/*API*/VOID		 	WINAPI	Plane_TransformArray( Plane *pOut, unsigned int OutStride, const Plane *pP, unsigned int PStride, const matrix4x4 *pM, unsigned int n )
{
	planeTransformArray(pOut, OutStride, pP, PStride, (matrix4x4*)pM, n);
}
// Transform an array of planes by a matrix.  The vectors (a,b,c) must be normal.// M should be the inverse transpose of the transformation desired.
//
/*API*/VOID		 	WINAPI	matrix4x4_transform(matrix4x4* bzOut, const float3* pos, const quaternion* rot, const float3* scale)
{
	matrixTransform((matrix4x4*)bzOut, (float3*)pos, (quaternion*)rot, (float3*)scale);
}
/*API*/BOOL			WINAPI	matrix4x4_decompose( float3 *pOutScale, quaternion *pOutRotation, float3 *pOutTranslation, const matrix4x4 *pM )
{
	return matrixDecompose((float3*)pOutScale, (quaternion*)pOutRotation, (float3*)pOutTranslation, (matrix4x4*)pM);
}

/*API*/VOID			WINAPI	matrix4x4_transpose( matrix4x4 *pOut, const matrix4x4 *pM )
{
	matrixTranspose((matrix4x4*)pOut, (matrix4x4*)pM);
}

/*API*/VOID 		WINAPI	matrix4x4_Multiply( matrix4x4 *pOut, const matrix4x4 *pM1, const matrix4x4 *pM2 )
{
	matrixMultiply((matrix4x4*)pOut, (matrix4x4*)pM1, (matrix4x4*)pM2);
}
// Matrix multiplication.  The result represents the transformation M2// followed by the transformation M1.  (Out = M1 * M2)
/*API*/VOID 		WINAPI	matrix4x4_multiply_transpose( matrix4x4 *pOut, const matrix4x4 *pM1, const matrix4x4 *pM2 )
{
	matrixMultiplyTranspose((matrix4x4*)pOut, (matrix4x4*)pM1, (matrix4x4*)pM2);
}
/*API*/VOID 		WINAPI	matrix4x4_inverse/*矩阵_逆矩阵*/( matrix4x4 *pOut/*返回*/, const matrix4x4 *pM )
{
	matrixInverse((matrix4x4*)pOut, 0, (matrix4x4*)pM);
}
// Matrix multiplication, followed by a transpose. (Out = T(M1 * M2))
/*API*/VOID 		WINAPI	matrix4x4_inverse2( matrix4x4 *pOut, float3* pDeterminant, const matrix4x4 *pM )
{
	matrixInverse((matrix4x4*)pOut, (float*)pDeterminant, (matrix4x4*)pM);
}
// Calculate inverse of matrix.  Inversion my fail, in which case NULL will// be returned.  The determinant of pM is also returned it pfDeterminant// is non-NULL.
/*API*/VOID 		WINAPI	matrix4x4_scaling( matrix4x4 *pOut, float sx, float sy, float sz )
{
	matrixScaling((matrix4x4*)pOut, sx, sy, sz);
}
// Build a matrix which scales by (sx, sy, sz)
/*API*/VOID 		WINAPI	matrix4x4_translation( matrix4x4 *pOut, float x, float y, float z )
{
	matrixTranslation((matrix4x4*)pOut, x, y, z);
}
// Build a matrix which translates by (x, y, z)
/*API*/VOID 		WINAPI	matrix4x4_rotationx( matrix4x4 *pOut, float Angle )
{
	matrixRotationX((matrix4x4*)pOut, Radians(Angle));
}
// Build a matrix which rotates around the X axis
/*API*/VOID 		WINAPI	matrix4x4_rotationy( matrix4x4 *pOut, float Angle )
{
	matrixRotationY((matrix4x4*)pOut, Radians(Angle));
}
// Build a matrix which rotates around the Y axis
/*API*/VOID 		WINAPI	matrix4x4_rotationz( matrix4x4 *pOut, float Angle )
{
	matrixRotationZ((matrix4x4*)pOut, Radians(Angle));
}
// Build a matrix which rotates around the Z axis
/*API*/VOID 		WINAPI	matrix4x4_rotationaxis( matrix4x4 *pOut, const float3 *pV, float Angle )
{
	matrixRotationAxis((matrix4x4*)pOut, (float3*)pV, Radians(Angle));
}
// Build a matrix which rotates around an arbitrary axis
/*API*/VOID 		WINAPI	matrix4x4_rotationquaternion( matrix4x4 *pOut, const quaternion *pQ)
{
	matrixRotationQuaternion((matrix4x4*)pOut, (quaternion*)pQ);
}
// Build a matrix from a quaternion
/*API*/VOID 		WINAPI	matrix4x4_rotation_yawpitchroll( matrix4x4 *pOut, float Yaw, float Pitch, float Roll )
{
	matrixRotationYawPitchRoll((matrix4x4*)pOut, Radians(Yaw), Radians(Pitch), Radians(Roll));
}
// Yaw around the Y axis, a pitch around the X axis,// and a roll around the Z axis.
/*API*/VOID 		WINAPI	matrix4x4_transformation( matrix4x4 *pOut, const float3 *pScalingCenter,const quaternion *pScalingRotation, const float3 *pScaling,const float3 *pRotationCenter, const quaternion *pRotation,const float3 *pTranslation)
{
	matrixTransformation((matrix4x4*)pOut, (float3*)pScalingCenter, (quaternion*)pScalingRotation, (float3*)pScaling, (float3*)pRotationCenter, (quaternion*)pRotation, (float3*)pTranslation);
}
// Build transformation matrix.  NULL arguments are treated as identity.// Mout = Msc-1 * Msr-1 * Ms * Msr * Msc * Mrc-1 * Mr * Mrc * Mt
/*API*/VOID 		WINAPI	matrix4x4_transformation2d( matrix4x4 *pOut, const float2* pScalingCenter, float ScalingRotation, const float2* pScaling, const float2* pRotationCenter, float Rotation, const float2* pTranslation)
{
	matrixTransformation2D((matrix4x4*)pOut, (float2*)pScalingCenter, ScalingRotation, (float2*)pScaling, (float2*)pRotationCenter, Rotation, (float2*)pTranslation);
}
// Build 2D transformation matrix in XY Plane.  NULL arguments are treated as identity.// Mout = Msc-1 * Msr-1 * Ms * Msr * Msc * Mrc-1 * Mr * Mrc * Mt
/*API*/VOID 		WINAPI	matrix4x4_affineTransformation( matrix4x4 *pOut, float Scaling, const float3 *pRotationCenter,const quaternion *pRotation, const float3 *pTranslation)
{
	matrixAffineTransformation((matrix4x4*)pOut, Scaling, (float3*)pRotationCenter, (quaternion*)pRotation, (float3*)pTranslation);
}
// Build affine transformation matrix.  NULL arguments are treated as identity.// Mout = Ms * Mrc-1 * Mr * Mrc * Mt
/*API*/VOID 		WINAPI	matrix4x4_affineTransformation2d( matrix4x4 *pOut, float Scaling, const float2* pRotationCenter, float Rotation, const float2* pTranslation)
{
	matrixAffineTransformation2D((matrix4x4*)pOut, Scaling, (float2*)pRotationCenter, Rotation, (float2*)pTranslation);
}
// Build 2D affine transformation matrix in XY Plane.  NULL arguments are treated as identity.// Mout = Ms * Mrc-1 * Mr * Mrc * Mt
/*API*/VOID 		WINAPI	matrix4x4_lookat_rh( matrix4x4 *pOut, const float3 *pEye, const float3 *pAt,const float3 *pUp )
{
	matrixLookAtRH((matrix4x4*)pOut, (float3*)pEye, (float3*)pAt, (float3*)pUp);
}
// Build a lookat matrix. (right-handed)
/*API*/VOID 		WINAPI	matrix4x4_lookat_lh( matrix4x4 *pOut, const float3 *pEye, const float3 *pAt,const float3 *pUp )
{
	matrixLookAtLH((matrix4x4*)pOut, (float3*)pEye, (float3*)pAt, (float3*)pUp);
}
// Build a lookat matrix. (left-handed)
/*API*/VOID 		WINAPI	matrix4x4_perspective_rh( matrix4x4 *pOut, float w, float h, float zn, float zf )
{
	matrixPerspectiveRH((matrix4x4*)pOut, w, h, zn, zf);
}
// Build a perspective projection matrix. (right-handed)
/*API*/VOID 		WINAPI	matrix4x4_perspective_lv( matrix4x4 *pOut, float w, float h, float zn, float zf )
{
	matrixPerspectiveLH((matrix4x4*)pOut, w, h, zn , zf);
}
// Build a perspective projection matrix. (left-handed)
/*API*/VOID 		WINAPI	matrix4x4_perspective_fovrh( matrix4x4 *pOut, float fovy, float Aspect, float zn, float zf )
{
	matrixPerspectiveFovRH((matrix4x4*)pOut, fovy, Aspect, zn ,zf);
}
// Build a perspective projection matrix. (right-handed)
/*API*/VOID 		WINAPI	matrix4x4_perspectivefov_lh( matrix4x4 *pOut, float fovy, float Aspect, float zn, float zf )
{
	matrixPerspectiveFovLH((matrix4x4*)pOut, fovy, Aspect, zn, zf);
}
// Build a perspective projection matrix. (left-handed)
/*API*/VOID 		WINAPI	matrix4x4_perspective_offcenter_rh( matrix4x4 *pOut, float l, float r, float b, float t, const float2* znearfar )
{
	matrixPerspectiveOffCenterRH((matrix4x4*)pOut, l, r, b, t, znearfar->x, znearfar->y);
}
// Build a perspective projection matrix. (right-handed)
/*API*/VOID 		WINAPI	matrix4x4_perspective_offcenter_lh( matrix4x4 *pOut, float l, float r, float b, float t, const float2* znearfar )
{
	matrixPerspectiveOffCenterLH((matrix4x4*)pOut, l, r, b, t, znearfar->x, znearfar->y);
}
// Build a perspective projection matrix. (left-handed)
/*API*/VOID 		WINAPI	matrix4x4_ortho_rh( matrix4x4 *pOut, float w, float h, float zn, float zf )
{
	matrixOrthoRH((matrix4x4*)pOut, w, h, zn, zf);
}
// Build an ortho projection matrix. (right-handed)
/*API*/VOID 		WINAPI	matrix4x4_ortho_lh( matrix4x4 *pOut, float w, float h, float zn, float zf )
{
	matrixOrthoLH((matrix4x4*)pOut, w, h, zn, zf);
}
// Build an ortho projection matrix. (left-handed)
/*API*/VOID 		WINAPI	matrix4x4_ortho_offcenter_rh( matrix4x4 *pOut, float l, float r, float b, float t, const float2* znearfar )
{
	matrixOrthoOffCenterRH((matrix4x4*)pOut, l, r, b, t, znearfar->x, znearfar->y);
}
// Build an ortho projection matrix. (right-handed)
/*API*/VOID 		WINAPI	matrix4x4_orth_offcenter_lh( matrix4x4 *pOut, float l, float r, float b, float t, const float2* znearfar )
{
	matrixOrthoOffCenterLH((matrix4x4*)pOut, l, r, b, t, znearfar->x, znearfar->y);
}
// Build an ortho projection matrix. (left-handed)
/*API*/VOID 		WINAPI	matrix4x4_reflect( matrix4x4 *pOut, const Plane *pPlane )
{
	matrixReflect((matrix4x4*)pOut, pPlane);
}
// Build a matrix which reflects the coordinate system about a Plane

///*API*/BOOL			WINAPI	System_ray_triangle(raydata* pOutPick, const float3* vOrig, const float3* vDir, const float3* v0, const float3* v1, const float3* v2)
//{
//	PIntersectData data;
//	if(pOutPick)
//	{
//		data.normal	=	(float3*)&pOutPick->normal;
//		data.Pick	=	(float3*)&pOutPick->pick;
//		data.t		=	&pOutPick->t;
//	}
//	return IntersectTriangle(&data, *vOrig, *vDir, *v0, *v1, *v2, (matrix4x4*)pWorld, bDoubleSide);
//}
//检测三角型是否会和射线碰撞
/*API*/BOOL			WINAPI	Global_ray3d_box(const float3* vOrig, const float3* vDir, const AABox* box, const matrix4x4* pWorld)
{
	return IntersectBox(*vOrig, *vDir, box->_min, box->_max, (matrix4x4*)pWorld);
}

/*API*/BOOL			WINAPI	Global_isunwalkable/*系统_是否禁止移动*/(const float3* pos/*当前位置*/)
{
	for(int i=0;i<g_manager.GetMeshCount();i++)
	{
		Mesh* m = g_manager.GetMeshPtr(i);
		if(!m->IsVisible() || !m->m_bEnabled)
			continue;
		if(m->IsPointDisableMove(*pos, NULL))
			return true;
	}
	return false;
}
/*API*/float		WINAPI	Global_getheight_3d/*系统_根据碰撞子模型取高度*/(const float3* pos/*位置*/)
{
	IntersectData* ii		=	GetCameraPtr()->GetIntersectPtr();
	float newZ = pos->z + 1.0f;
	ii->begin(float3(pos->x, pos->y, newZ), float3(0.0f, 0.0f, -1.0f), To3DValue(1000.0f));
	float fMinZ = -100000.0f;
	BOOL bFind = false;
	for(int i=0;i<g_manager.GetMeshCount();i++)
	{
		Mesh* m = g_manager.GetMeshPtr(i);
		if(!m->IsVisible() || !m->m_bEnabled)
			continue;
		if(!m->GetHeight(ii))
			continue;
		if(ii->GetPickInfo().vPick.z > fMinZ && ii->GetPickInfo().vPick.z < newZ)
		{
			bFind	=	true;
			fMinZ	=	ii->GetPickInfo().vPick.z;
		}
	}
	if(!bFind)
		return pos->z;
	return fMinZ;
}
/*API*/VOID			WINAPI	Global_ray3d/*系统_碰撞检测_仅与碰撞子模型*/(const Pixel* p/*屏幕坐标位置*/, IMeshPtr* _return/*返回*/)
{
	Mesh* ret = 0;
	GetCameraPtr()->GetIntersectPtr()->begin(GetCameraPtr(), p->x, p->y);
	for(int i=0;i<g_manager.GetMeshCount();i++)
	{
		Mesh* m = g_manager.GetMeshPtr(i);
		if(!m->IsVisible() || !m->m_bEnabled)
			continue;
		if(m->PickRay(GetCameraPtr()->GetIntersectPtr(),Mesh::MeshPick_Full))
			ret = m;
	}
	if(!ret)
		return;
	SetIPtr(_return, ret, Mesh, IMeshPtr);
}
///*API*/VOID			WINAPI	Global_ray3d_all/*系统_碰撞检测_自动*/(const Pixel* p/*屏幕坐标位置*/, IMeshPtr* _return/*返回*/)
//{
//	Mesh* ret = 0;
//	GetCameraPtr()->GetIntersectPtr()->begin(GetCameraPtr(), p->x, p->y);
//	for(int i=0;i<g_manager.GetMeshCount();i++)
//	{
//		Mesh* m = g_manager.GetMeshPtr(i);
//		if(!m->IsVisible() || !m->m_bEnabled)
//			continue;
//		if(m->Pick(GetCameraPtr()->GetIntersectPtr()))
//			ret = m;
//	}
//	if(!ret)
//		return;
//	SetIPtr(_return, ret, Mesh, IMeshPtr);
//}
/*API*/VOID			WINAPI	Global_ray3d_dir/*系统_碰撞检测_仅与碰撞子模型*/(const float3* orig/*原点*/, const float3* dir/*方向*/, IMeshPtr* _return/*返回*/)
{
	Mesh* ret = 0;
	GetCameraPtr()->GetIntersectPtr()->begin(*orig, *dir, GetCameraPtr()->GetZFar());
	for(int i=0;i<g_manager.GetMeshCount();i++)
	{
		Mesh* m = g_manager.GetMeshPtr(i);
		if(!m->IsVisible() || !m->m_bEnabled)
			continue;
		if(m->PickRay(GetCameraPtr()->GetIntersectPtr(), Mesh::MeshPick_Full))
			ret = m;
	}
	if(!ret)
		return;
	SetIPtr(_return, ret, Mesh, IMeshPtr);
}
///*API*/VOID			WINAPI	Global_ray3d_dir_all/*系统_碰撞检测_自动*/(const float3* orig/*原点*/, const float3* dir/*方向*/, IMeshPtr* _return/*返回*/)
//{
//	Mesh* ret = 0;
//	GetCameraPtr()->GetIntersectPtr()->begin(*orig, *dir, GetCameraPtr()->GetZFar());
//	for(int i=0;i<g_manager.GetMeshCount();i++)
//	{
//		Mesh* m = g_manager.GetMeshPtr(i);
//		if(!m->IsVisible() || !m->m_bEnabled)
//			continue;
//		if(m->Pick(GetCameraPtr()->GetIntersectPtr()))
//			ret = m;
//	}
//	if(!ret)
//		return;
//	SetIPtr(_return, ret, Mesh, IMeshPtr);
//}
///*API*/VOID			WINAPI	GameMgr_ray3d_getdata/*系统_返回上一次碰撞检测之后的值*/(raydata* _return/*返回*/)
//{
//	_return->normal = GetCameraPtr()->GetIntersectPtr()->m_info.vFaceNormal;
//	_return->point = GetCameraPtr()->GetIntersectPtr()->m_info.vPick;
//	_return->t = GetCameraPtr()->GetIntersectPtr()->m_info.fDist;
//}

//检测盒子是否和射线碰撞
/*API*/float		WINAPI	Global_clamp_angle/*弧度限制*/(float fAngle)
{
	return Degress(clampAngle(Radians(fAngle)));
}
//将弧度限制范围在 -bz_pi ~ bz_pi 之间

//向量随Z轴转动
/*API*/float		WINAPI	float3_getrotate_z/*三位向量_返回角度Z*/(const float3* _dir)
{
	return Degress(getRotateZ(*_dir)+PI);
}
float				WINAPI	Global_tovalue/*坐标*/(float meter/*米*/)//从米计算到坐标(返回 meter * 10)
{
	return To3DValue(meter);
}

float				WINAPI	Global_tometer/*米*/(float v/*坐标*/)
{
	return ToMeter(v);
}
//从坐标计算到米(返回 v * 0.1)
/*API*/VOID			WINAPI	Global_mousepoint/*系统_返回鼠标或者手指的位置*/(INT nIndex/*手指索引*/, Pixel* _return/*返回*/)//手机系统中可以很多个手指位置同时获取
{
	*_return = g_manager.GetCursorPos(nIndex);
}
/*API*/VOID			WINAPI	Global_mousepoint2d/*系统_返回鼠标或者手指的位置*/(INT nIndex/*手指索引*/, Pixel* _return/*返回*/)//手机系统中可以很多个手指位置同时获取
{
	g_manager.GetCursorPos2D(*_return, nIndex);
}
/*API*/VOID			WINAPI	System_Setmaxfps/*系统_设置最高FPS*/(INT n/*设置*/)
{
	if(n < 1)n = 1;
	else if(n > 1000)n = 1000;
	g_manager.SetSleepTime(1.0f / (float)n);
}
/*API*/INT			WINAPI	System_Getmaxfps/*系统_取得最高FPS*/()
{
	if(g_manager.GetSleepTime() < 0.001f)
		return 1000;
	return (1 / g_manager.GetSleepTime());
}
/*API*/VOID			WINAPI	System_Setvr_rotx/*系统_开启VR转动*/(float f/*转动X*/)
{
	GetCameraPtr()->m_fRotationVRX = Radians(f);
}
/*API*/float		WINAPI	System_Getvr_rotx/*系统_是否VR转动*/()
{
	return Degress(GetCameraPtr()->m_fRotationVRX);
}

/*API*/INT			WINAPI	System_fps/*系统_返回当前FPS*/()
{
	return (int)g_manager.GetFPS();
}
/*API*/VOID			WINAPI	Global_exit/*系统_调用退出游戏函数*/()
{
	g_manager.ExitGame();
}
/*API*/float		WINAPI	Global_clamp/*系统_值限制*/(float fValue/*值*/, float fMin/*最小*/, float fMax/*最大*/)//设置值在某个范围内
{
	return clamp(fValue, fMin, fMax);
}
/*API*/VOID			WINAPI	System_sensor_rotator/*系统_获取传感器转动角度*/(float3* _return/*返回*/)
{
	*_return = GetCameraPtr()->m_vrRotator;
}
/*API*/LPCSTR		WINAPI	Global_getvalt/*全局_从程序读取一个文本*/(LPCSTR name/*配置项名字*/)
{
	return g_luamanager.m_memvals->readt(name, "");
}
/*API*/double		WINAPI	Global_getvalf/*全局_从程序读取一个浮点数*/(LPCSTR name/*配置项名字*/)
{
	return g_luamanager.m_memvals->readf(name, 0);
}
/*API*/int			WINAPI	Global_getvali/*全局_从程序读取一个整数*/(LPCSTR name/*配置项名字*/)
{
	return g_luamanager.m_memvals->readi(name, 0);
}
/*API*/VOID			WINAPI	Global_setvalt/*全局_写入一个文本到程序*/(LPCSTR name/*配置项名字*/, LPCSTR szValue/*值*/)
{
	g_luamanager.m_memvals->write(name, szValue);
}
/*API*/VOID			WINAPI	Global_setvalf/*全局_写入一个浮点到程序*/(LPCSTR name/*配置项名字*/, double fValue/*值*/)
{
	g_luamanager.m_memvals->writef(name, fValue);
}
/*API*/VOID			WINAPI	Global_setvali/*全局_写入一个整数到程序*/(LPCSTR name/*配置项名字*/, INT nValue/*值*/)
{
	g_luamanager.m_memvals->writei(name, nValue);
}
/*API*/INT			WINAPI	Global_timeout2/*全局_定时引发带一个文本参数*/(LPCSTR szName/*脚本函数名*/, INT nMillisecond/*毫秒*/, LPCSTR szParameter/*文本参数*/)
{
	if(!szName)return 0;
	return g_luamanager.AddTimeout(nMillisecond, szName, szParameter);
}
/*API*/INT			WINAPI	Global_interval/*全局_间隔引发*/(LPCSTR szName/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用次数*/)
{
	if(!szName)return 0;
	return g_luamanager.AddInterval(nMillisecond, nMaxMillisecond, szName);
}

/*API*/VOID			WINAPI	Global_cleartimeby2/*全局_关闭任何定时和间隔引发*/(LPCSTR szFunc, LPCSTR szParameter/*删除对应参数的*/)
{
	if(szFunc&&szFunc[0])
		g_luamanager.ClearTime(szFunc, szParameter);
}
/*API*/VOID			WINAPI	Global_cleartimeby/*全局_关闭任何定时和间隔引发*/(LPCSTR szFunc/*id*/)
{
	if(szFunc&&szFunc[0])
		g_luamanager.ClearTime(szFunc, 0);
}
/*API*/VOID			WINAPI	Global_cleartime/*全局_关闭任何定时和间隔引发根据ID*/(INT id)
{
	g_luamanager.ClearTime(id);
}
/*API*/VOID			WINAPI	Global_cleartime_all/*全局_清除所有定时器*/()
{
	g_luamanager.ClearTimeAll();
}
/*API*/VOID			WINAPI	Global_breaktime/*全局_关闭任何定时和间隔引发*/()
{
	g_luamanager.BreakTime();
}
/*API*/VOID			WINAPI	Global_localtime/*全局_返回当前时间*/(Time* _return/*返回*/)
{
	GetLocalTime(*_return);
}
/*API*/int			WINAPI	Global_timediff_d/*全局_返回时间差_日*/(Time* t1/*时间1*/, Time* t2/*时间2*/)
{
	int day1 = t1->year * 365 + (t1->year/4) + t1->day;
	int day2 = t2->year * 365 + (t2->year/4) + t2->day;
	return day1-day2;
}
/*API*/int			WINAPI	Global_timediff_h/*全局_返回时间差_时*/(Time* t1/*时间1*/, Time* t2/*时间2*/)
{
	return (Global_timediff_d(t1, t2) * 24 + (t1->hour-t2->hour));
}
/*API*/int			WINAPI	Global_timediff_m/*全局_返回时间差_分*/(Time* t1/*时间1*/, Time* t2/*时间2*/)
{
	return (Global_timediff_h(t1, t2) * 60 + (t1->minute-t2->minute));
}
/*API*/int			WINAPI	Global_timediff_s/*全局_返回时间差_秒*/(Time* t1/*时间1*/, Time* t2/*时间2*/)
{
	return (Global_timediff_m(t1, t2) * 60 + (t1->second-t2->second));
}
/*API*/LPCSTR		WINAPI	Global_language_image/*全局_返回当前语言对应的路径名*/(LPCSTR szFileName/*名字*/)
{
	if(!szFileName)return 0;
	static char buf[128];
	sprintf(buf, "images/%s/%s", GetLanguagePtr()->GetPath(), szFileName);
	return buf;
}

/*API*/BOOL			WINAPI	Global_point_in/*全局_判断点是否在矩形内*/(const Rect* rect/*矩形*/, const Pixel* point/**/)
{
	if(!rect || !point)return false;
	return rect->isPtIn(*point);
}
/*API*/VOID			WINAPI	Global_bind_input/*全局_全局绑定*/(LPCSTR szEvent/*事件名*/, LPCSTR szFunc/*函数*/)//szEvent="down,up,move", 参数是，pt1,pt2
{
	if(!szEvent)
		return;
	if(!szFunc)szFunc = "";
	if(stricmp(szEvent, "down") == 0)
		g_luamanager.m_downFunc = szFunc;
	else if(stricmp(szEvent, "up") == 0)
		g_luamanager.m_upFunc = szFunc;
	else if(stricmp(szEvent, "move") == 0)
		g_luamanager.m_moveFunc = szFunc;
	else if(stricmp(szEvent, "dbldown") == 0)
		g_luamanager.m_doubleDownFunc = szFunc;
	else if(stricmp(szEvent, "dblup") == 0)
		g_luamanager.m_doubleUpFunc = szFunc;
	//else if(stricmp(szEvent, "dblmove") == 0)
		//g_luamanager.m_doubleMoveFunc = szFunc;
	else if(stricmp(szEvent, "onsize") == 0)
		g_luamanager.m_doubleMoveFunc = szFunc;
}

/*API*/VOID			WINAPI	Global_setlanguage/*全局_设置地区语言所在目录*/(LPCSTR szlanPath/*函数*/)
{
	g_manager.SetIsUtf8(true);
	GetLanguagePtr()->LoadFromFile(szlanPath);
	if(stricmp(szlanPath, "en") == 0){
		g_manager.SetDefaultUnicodeFont(0);
	}
	else{
		char szFig[256];
		sprintf(szFig, "default_init_zip/fonts/%s.fig", szlanPath);
		g_manager.SetDefaultUnicodeFont(szFig);
	}
}
/*API*/LPCSTR		WINAPI	Global_lan/*全局_读取文本*/(LPCSTR szName/*指针*/)
{
	return Lan(szName, szName);
}
/*API*/INT			WINAPI	Global_interval2/*全局_间隔引发带一个文本参数*/(LPCSTR szName/*脚本函数名*/, INT nMillisecond/*毫秒*/, INT nMaxMillisecond/*最大调用次数*/, LPCSTR szParameter/*文本参数*/)
{
	if(!szName)return 0;
	return g_luamanager.AddInterval(nMillisecond, nMaxMillisecond, szName, szParameter);
}

/*API*/INT			WINAPI	Global_timeout/*全局_定时引发*/(LPCSTR szName/*脚本函数名*/, INT nMillisecond/*毫秒*/)
{
	if(!szName)return 0;
	return g_luamanager.AddTimeout(nMillisecond, szName);
}

/*API*/VOID			WINAPI	Global_pause/*全局_暂停整个游戏*/(BOOL bPause/*是否暂停*/)
{
	g_manager.FramePause(bPause);
	for(int i=0;i<GetDlgManager()->GetDialogManager().size();i++){
		GetDlgManager()->GetDialogManager()[i]->m_bIsPause = bPause;
	}
}
/*API*/VOID			WINAPI	Global_speed/*全局_单个时间的加速*/(float fSpeed/*速度*/)
{
	g_manager.SetFrameSpeed(fSpeed);
}
/*API*/INT			WINAPI	Global_framemove/*全局_循环引发*/(LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/)//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高
{
	return g_luamanager.AddFramemove(nMaxMillisecond,szLuaFunc);
}
/*API*/INT			WINAPI	Global_framemove2/*全局_循环引发*/(LPCSTR szLuaFunc/*脚本函数名*/, INT nMaxMillisecond/*最大调用毫秒时间*/, LPCSTR szParameter/*文本参数*/)//时时引发，每次循环都会被调用引发一次，会持续引发，毫秒记,频率很高
{
	return g_luamanager.AddFramemove(nMaxMillisecond, szLuaFunc, szParameter);
}



/*API*/VOID		WINAPI	Map3DItem_Getworldmat/*场景对象_取世界矩阵*/(IMap3DItemPtr* _self/*场景对象*/, matrix4x4* _return)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)
	{
		_return->identity();
	}
	else
		*_return	=	*so->GetWorldPtr();
}
/*API*/VOID		WINAPI	Map3DItem_Setworldmat/*场景对象_设置世界矩阵*/(IMap3DItemPtr* _self/*场景对象*/, const matrix4x4* m/*设置*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetWorld(m);
}

/*API*/BOOL		WINAPI	Map3DItem_Getdisplay/*场景对象_是否显示*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return false;
	return so->GetVisible();
}

/*API*/VOID		WINAPI	Map3DItem_Setdisplay/*场景对象_显示*/(IMap3DItemPtr* _self/*场景对象*/, BOOL bVisible/*显示与否*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetVisible(bVisible);
}

/*API*/VOID		WINAPI	Map3DItem_Getpos3d/*场景对象_取位置*/(IMap3DItemPtr* _self/*场景对象*/, float3* _return)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)
	{
		_return->setxyz(0,0,0);
	}
	else
		*_return	=	so->GetPosition();
}
/*API*/VOID		WINAPI	Map3DItem_Setpos3d/*场景对象_设置位置*/(IMap3DItemPtr* _self/*场景对象*/, const float3* m/*设置*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetPosition( *m );
}

/*API*/VOID		WINAPI	Map3DItem_Getpos/*场景对象_取位置*/(IMap3DItemPtr* _self/*场景对象*/, Pixel* _return)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)
	{
		_return->Set(0,0);
	}
	else
	{
		const float3& pos = so->GetPosition();
		_return->Set(pos.x, pos.y);
	}
}
/*API*/VOID		WINAPI	Map3DItem_Setpos/*场景对象_设置位置*/(IMap3DItemPtr* _self/*场景对象*/, const Pixel* m/*设置*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetPosition(*m);
}
/*API*/INT		WINAPI	Map3DItem_id/*场景对象_取ID*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return 0;
	return so->m_nContentID;//GetID();
}
/*API*/VOID		WINAPI	Map3DItem_Setname/*地图对象3D_设置名字*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szName/*名字*/)
{
	if(!szName)return;
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetName(szName, true);
}
/*API*/LPCSTR	WINAPI	Map3DItem_Getname/*场景对象_取文件路径*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return "";
	return so->GetName();
}
/*API*/BOOL				WINAPI	Map3DItem_Getenabled/*地图对象3D_是否有效*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return false;
	return so->GetEnabled();
}
/*API*/VOID				WINAPI	Map3DItem_Setenabled/*地图对象3D_有效*/(IMap3DItemPtr* _self/*场景对象*/, BOOL bVisible/*显示与否*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetEnabled(bVisible);
}
/*API*/VOID				WINAPI	Map3DItem_Setheight/*地图对象3D_设置飞行高度*/(IMap3DItemPtr* _self/*场景对象*/, float fFly/*飞行高度*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetFly(fFly);
}
/*API*/float			WINAPI	Map3DItem_Getheight/*地图对象3D_取飞行高度*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return 0;
	return so->m_fFly;
}

/*API*/VOID				WINAPI	Map3DItem_mesh/*地图对象3D_返回模型*/(IMap3DItemPtr* _self/*场景对象*/, IMeshPtr* _return/*返回*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	SetIPtr(_return , so->GetMeshPtr(), Mesh, IMeshPtr);
}

/*API*/float			WINAPI	Map3DItem_Getrot/*地图对象3D_取转动Z值角度*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return 0;
	return Degress(so->m_fRotationZ);
}

/*API*/VOID				WINAPI	Map3DItem_Setrot/*地图对象3D_设置转动Z值角度*/(IMap3DItemPtr* _self/*场景对象*/, float fRotZ/*转动Z*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetRotation(Radians(fRotZ));
}

/*API*/VOID				WINAPI	Map3DItem_movedir/*地图对象3D_自动移动*/(IMap3DItemPtr* _self/*场景对象*/, const Pixel* dir/*自动移动*/)
{
	if(!dir)return;
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->MoveDir(*dir);
}

/*API*/VOID				WINAPI	Map3DItem_moveto/*地图对象3D_移动到指定位置*/(IMap3DItemPtr* _self/*场景对象*/, const Pixel* pos/*目标点*/, LPCSTR szMoveFunc)
{
	if(!pos)return;
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->MoveTo(*pos, szMoveFunc);
}
/*API*/VOID				WINAPI	Map3DItem_lookat/*地图对象3D_设置面向目标*/(IMap3DItemPtr* _self/*场景对象*/, const Pixel* m/*设置*/)
{
	if(!m)return;
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->LookatTo(m);
}


/*API*/float			WINAPI	Map3DItem_Getscale/*地图对象3D_取缩放*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return 0;
	return so->m_fScale;
}

/*API*/VOID				WINAPI	Map3DItem_Setscale/*地图对象3D_设置缩放*/(IMap3DItemPtr* _self/*场景对象*/, float fScale/*缩放*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetScale(fScale);
}
/*API*/VOID				WINAPI	Map3DItem_bind_action_begin/*地图对象3D_设置动作的脚本回调*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szLuaBegin/*动作开始脚本函数*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetActionBeginFunc(szLuaBegin);
}
/*API*/VOID				WINAPI	Map3DItem_bind_action_end/*地图对象3D_设置动作的脚本回调*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szLuaEnd/*动作结束脚本函数*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetActionEndFunc(szLuaEnd);
}
/*API*/VOID				WINAPI	Map3DItem_Setactionby/*地图对象3D_设置当前动作*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szName/*动作名称*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetAction(szName);
}
/*API*/LPCSTR			WINAPI	Map3DItem_Getactionby/*地图对象3D_设置当前动作*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return "";
	return so->GetActionName();
}
/*API*/VOID				WINAPI	Map3DItem_pushactionby/*地图对象3D_设置下一个动作*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szName/*动作名称*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!szName)return;
	so->SetNextAction(szName);
}

/*API*/VOID				WINAPI	Map3DItem_pushaction/*地图对象3D_设置下一个动作根据动作类型*/(IMap3DItemPtr* _self/*场景对象*/, ActionType type/*动作类型*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetNextAction(type);
}
/*API*/VOID				WINAPI	Map3DItem_stopmove/*地图对象3D_停止移动*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return;
	so->StopMove();
}
/*API*/VOID				WINAPI	Map3DItem_Setaction/*地图对象3D_设置当前动作根据动作ID*/(IMap3DItemPtr* _self/*场景对象*/, ActionType type/*动作ID*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return;
	so->SetAction(type);
}
/*API*/ActionType		WINAPI	Map3DItem_Getaction/*地图对象3D_设置当前动作根据动作ID*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return ActionType_None;
	return so->GetActionType();
}
/*API*/float			WINAPI	Map3DItem_Getspeed/*地图对象3D_取移动速度*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return 0;
	return so->m_fSpeed;
}
/*API*/VOID				WINAPI	Map3DItem_Setspeed/*地图对象3D_设置移动速度*/(IMap3DItemPtr* _self/*场景对象*/, float fSpeed/*移动速度*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetSpeed(fSpeed);
}
///*API*/VOID				WINAPI	Map3DItem_rotadd/*地图对象3D_转动Z值调整值*/(IMap3DItemPtr* _self/*场景对象*/, float fRotZ/*转动Z调整*/)
//{
//	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
//	if(!so)return;
//	so->RotateAdd(Radians(fRotZ));
//}
/*API*/float			WINAPI	Map3DItem_Getaspeed/*地图对象3D_取动作播放速度*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return 0;
	return so->GetMeshPtr()->GetActionSpeed();
}

/*API*/VOID				WINAPI	Map3DItem_Setaspeed/*地图对象3D_设置动作播放速度*/(IMap3DItemPtr* _self/*场景对象*/, float fSpeed/*动作播放速度*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return;
	so->GetMeshPtr()->SetActionSpeed(fSpeed);
}
///*API*/VOID				WINAPI	Map3DItem_Gettarget3d/*地图对象3D_取目标位置*/(IMap3DItemPtr* _self/*场景对象*/, float3* _return/*ret*/)
//{
//	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
//	if(!so||!so->GetMeshPtr())return;
//	*_return = so->m_targetPos;
//}
///*API*/VOID				WINAPI	Map3DItem_Settarget3d/*地图对象3D_设置目标位置*/(IMap3DItemPtr* _self/*场景对象*/, const float3* m/*设置*/)
//{
//	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
//	if(!so||!so->GetMeshPtr())return;
//	so->SetTargetPos(*m);
//}

/*API*/BOOL				WINAPI	Map3DItem_Getloop/*地图对象3D_是否循环播放*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return false;
	return so->GetMeshPtr()->GetLoop();
}

/*API*/VOID				WINAPI	Map3DItem_Setloop/*地图对象3D_设置循环播放*/(IMap3DItemPtr* _self/*场景对象*/, INT nLoop/*循环与否*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return;
	so->GetMeshPtr()->SetLoop(nLoop);
}

/*API*/BOOL				WINAPI	Map3DItem_Getplay/*地图对象3D_是否播放*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return false;
	return so->GetMeshPtr()->IsPlay();
}

/*API*/VOID				WINAPI	Map3DItem_Setplay/*地图对象3D_播放*/(IMap3DItemPtr* _self/*场景对象*/, BOOL b/*播放与否*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return;
	so->GetMeshPtr()->Play(b);
}
/*API*/VOID				WINAPI	Map3DItem_Setparent/*地图对象3D_设置父对象*/(IMap3DItemPtr* _self/*场景对象*/, IMap3DItemPtr* c/*父对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return;
	SceneObject* p = 0;
	if(c)
		p = GetIPtr(c, SceneObject, IMap3DItemPtr);
	so->SetParent(p);
}
/*API*/VOID				WINAPI	Map3DItem_Getparent/*地图对象3D_返回父对象*/(IMap3DItemPtr* _self/*场景对象*/, IMap3DItemPtr* _return)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return;
	SceneObject* p = so->GetParent();
	if(p){
		SetIPtr(_return, p, SceneObject, IMap3DItemPtr);
	}
}
/*API*/BOOL				WINAPI	Map3DItem_Getpause/*地图对象3D_是否暂停*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return false;
	return so->GetMeshPtr()->IsPause();
}

/*API*/VOID				WINAPI	Map3DItem_Setpause/*地图对象3D_暂停*/(IMap3DItemPtr* _self/*场景对象*/, BOOL b/*暂停与否*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so||!so->GetMeshPtr())return;
	so->GetMeshPtr()->Pause(b);
}

/*API*/float			WINAPI	Map3DItem_Getopacity/*地图对象3D__设置所有子模型透明值*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return 0;
	if(so->GetMeshPtr())
		return so->GetMeshPtr()->m_fAlphaValue;
	return 0;
}

/*API*/VOID				WINAPI	Map3DItem_Setopacity/*地图对象3D__设置所有子模型透明值*/(IMap3DItemPtr* _self/*场景对象*/,float fAlpha/*透明值*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	fAlpha = clamp(fAlpha, 0, 1);
	if(so->GetMeshPtr())
		so->GetMeshPtr()->SetAlpha(fAlpha);
}

/*API*/BOOL				WINAPI	Map3DItem_bind_mesh/*地图对象3D_绑定模型*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szBoneName/*骨骼名*/, IMeshPtr* mesh/*模型*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return false;
	Mesh* gg	=	so->GetMeshPtr();
	if(!gg)return false;
	return gg->BindMesh(szBoneName, GetIPtr(mesh, Mesh, IMeshPtr));
}
/*API*/BOOL				WINAPI	Map3DItem_unbind_mesh/*地图对象3D_解除绑定模型*/(IMap3DItemPtr* _self/*场景对象*/, IMeshPtr* mesh/*模型*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return false;
	Mesh* gg	=	so->GetMeshPtr();
	if(!gg)return false;
	return gg->UnbindMesh(GetIPtr(mesh, Mesh, IMeshPtr));
}
/*API*/VOID				WINAPI	Map3DItem_bind_draw/*地图对象3D_绑定渲染队列*/(IMap3DItemPtr* _self/*场景对象*/, IUIDrawMgrPtr* drawPtr/*组合*/, LPCSTR szBindBone/*绑定骨骼*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	Mesh* gg	=	so->GetMeshPtr();
	if(!gg)return;
	gg->BindDraw(szBindBone, GetIPtr(drawPtr, UIDrawMgr, IUIDrawMgrPtr));
}

/*API*/VOID				WINAPI	Map3DItem_replace_image/*地图对象3D_绑定图片*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szImageSrc/*原图片文件路径*/, LPCSTR szImageReplace/*替换图片文件路径*/)
{
	if(!szImageSrc||!szImageReplace)return;
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	Mesh* gg	=	so->GetMeshPtr();
	if(!gg)return;
	gg->ReplaceImage(szImageSrc, szImageReplace);
}

/*API*/VOID				WINAPI	Map3DItem_bind_eventby/*地图对象3D_绑定事件*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szEvent/*事件名*/, LPCSTR szLuaFunc/*函数*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->bind_event(GetEventID(szEvent), szLuaFunc);
}

/*API*/VOID				WINAPI	Map3DItem_bind_event/*地图对象3D_绑定事件*/(IMap3DItemPtr* _self/*场景对象*/, EventID nID/*事件名*/, LPCSTR szLuaFunc/*函数*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->bind_event(nID, szLuaFunc);
}

/*API*/VOID				WINAPI	Map3DItem_bind_click/*地图对象3D_绑定脚本到事件*/(IMap3DItemPtr* _self/*场景对象*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->bind_event(Phantom::EventID_Click, szLuaFunc);
}

/*API*/VOID				WINAPI	Map3DItem_Setblood_display/*地图对象3D_显示血条*/(IMap3DItemPtr* _self/*场景对象*/, BOOL bShow/*血条*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->m_bShowBlood = bShow;
}
/*API*/VOID				WINAPI	Map3DItem_Setblood/*地图对象3D_设置血条*/(IMap3DItemPtr* _self/*场景对象*/, float fValue/*血条*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetBlood(fValue);
}
/*API*/VOID				WINAPI	Map3DItem_Setname_display/*地图对象3D_显示名字*/(IMap3DItemPtr* _self/*场景对象*/, BOOL bShow/*名字*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->m_bShowName = bShow;
}
/*API*/BOOL				WINAPI	Map3DItem_Getblood_display/*地图对象3D_显示血条*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return false;
	return so->m_bShowBlood;
}
/*API*/float			WINAPI	Map3DItem_Getblood/*地图对象3D_返回血条值*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return 0;
	return so->m_fBloodProgress;
}
/*API*/VOID				WINAPI	Map3DItem_Setblood_offset/*地图对象3D_设置血条调整*/(IMap3DItemPtr* _self/*场景对象*/, Pixel* off/*血条*/)
{
	if(!off)return;
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->m_bloodOffset = *off;
}
/*API*/VOID				WINAPI	Map3DItem_Getblood_offset/*地图对象3D_返回血条调整*/(IMap3DItemPtr* _self/*场景对象*/, Pixel* _return/*血条*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	*_return = so->m_bloodOffset;
}

/*API*/BOOL				WINAPI	Map3DItem_Getname_display/*地图对象3D_显示名字*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return false;
	return so->m_bShowName;
}

/*API*/VOID				WINAPI	Map3DItem_clone/*地图对象3D_克隆对象*/(IMap3DItemPtr* _self/*场景对象*/, IMap3DItemPtr* _return/*返回*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	SceneObject* newso = so->Clone();
	SetIPtr(_return, newso, SceneObject, IMap3DItemPtr);
}
/*API*/VOID				WINAPI	Map3DItem_Setclassid/*地图对象3D_设置类聚*/(IMap3DItemPtr* _self/*场景对象*/, INT nClass/*类聚*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->SetClassID(nClass);
}
/*API*/INT				WINAPI	Map3DItem_Getclassid/*地图对象3D_设置类聚*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return 0;
	return so->m_nClass;
}
/*API*/VOID				WINAPI	Map3DItem_addhit/*地图对象3D_添加被击效果*/(IMap3DItemPtr* _self/*接口*/, int val/*值*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->AddHit(val, 1, true);
}
/*API*/VOID				WINAPI	Map3DItem_Setbloodcolor/*地图对象3D_设置血条颜色*/(IMap3DItemPtr* _self/*场景对象*/, const color4* c/*血条颜色*/)
{
	if(!c)return;
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->m_bloodColor = *c;
}
/*API*/VOID				WINAPI	Map3DItem_Getbloodcolor/*地图对象3D_返回血条颜色*/(IMap3DItemPtr* _self/*场景对象*/, color4* _return)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	*_return  = so->m_bloodColor;
}

/*API*/VOID				WINAPI	Map3DItem_deleteme/*地图对象3D_删除对象*/(IMap3DItemPtr* _self/*场景对象*/, IMap3DItemPtr* _return/*返回*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	so->GetScenePtr()->RemoveObject(so);
}
/*API*/BOOL				WINAPI	Map3DItem_issceneobj/*地图对象3D_返回是否场景对象*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return false;
	return (so->GetType()==MeshData::MeshType_SceneUnit);
}
/*API*/BOOL				WINAPI	Map3DItem_isgameobj/*地图对象3D_返回是否游戏对象*/(IMap3DItemPtr* _self/*场景对象*/)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return false;
	return (so->GetType()==MeshData::MeshType_Role);
}
/*API*/VOID				WINAPI	Map3DItem_gameobj/*地图对象3D_返回游戏对象*/(IMap3DItemPtr* _self/*场景对象*/, IGameObjectPtr* _return)
{
	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
	if(!so)return;
	GameObject* gobj = GetGameMgr()->Search(so->m_gameID.gameid);
	if(gobj)
		*_return = gobj->GetID().gameid;
}

//
///*API*/VOID				WINAPI	Map3DItem_create_effect/*地图对象3D_添加特效*/(IMap3DItemPtr* _self/*接口*/, IMap3DEffectPtr* _return/*返回*/);//添加特效对象,返回特效ID
///*API*/INT				WINAPI	Map3DItem_effect_count/*地图对象3D_返回特效数量*/(IMap3DItemPtr* _self/*接口*/);//返回特效数量
///*API*/VOID				WINAPI	Map3DItem_effect_at/*地图对象3D_返回特效模型*/(IMap3DItemPtr* _self/*接口*/, INT index/*索引*/, IMap3DEffectPtr* _return/*返回*/);//返回特效模型
//
//
///*API*/VOID				WINAPI	Map3DEffect_init_mesh/*地图特效3D_初始化模型*/(IMap3DEffectPtr* _self/*接口*/, LPCSTR szBegin/*开始时候的特效*/, LPCSTR szByAttack/*被攻击时候的特效*/);//初始化特效模型列表
///*API*/VOID				WINAPI	Map3DEffect_show/*地图特效3D_设置显示位置*/(IMap3DEffectPtr* _self/*接口*/);//显示特效，只有开始特效有效,在角色正下方显示
///*API*/VOID				WINAPI	Map3DEffect_bind_bone/*地图特效3D_设置绑定骨骼*/(IMap3DEffectPtr* _self/*接口*/, LPCSTR szBoneName/*在某个骨骼点显示*/);//设置骨骼绑定信息，初始化特效会使用被这个骨骼所绑定
///*API*/VOID				WINAPI	Map3DEffect_bind_target/*地图特效3D_设置目标*/(IMap3DEffectPtr* _self/*接口*/, IMap3DItemPtr* target/*目标*/, LPCSTR szTargetBoneName/*目标骨骼名字*/);//设置目标之后这个特效的跟踪特效会起作用
///*API*/VOID				WINAPI	Map3DEffect_Setloop/*地图特效3D_设置循环*/(IMap3DEffectPtr* _self/*接口*/, INT nLoop/*循环次数*/);//默认是循环1次,-1是无线循环
///*API*/INT				WINAPI	Map3DEffect_Getloop/*地图特效3D_取循环*/(IMap3DEffectPtr* _self/*接口*/);

//
///*API*/VOID				WINAPI	Map3DItem_create_effect/*地图对象3D_添加特效*/(IMap3DItemPtr* _self/*接口*/, IMap3DEffectPtr* _return/*返回*/)
//{
//	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
//	if(!so)return;
//	SceneEffect* eff = so->CreateEffect();
//	SetIPtr(_return, eff, SceneEffect, IMap3DEffectPtr);
//}
///*API*/INT				WINAPI	Map3DItem_effect_count/*地图对象3D_返回特效数量*/(IMap3DItemPtr* _self/*接口*/)
//{
//	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
//	if(!so)return 0;
//	return so->m_effects.size();
//}
///*API*/VOID				WINAPI	Map3DItem_effect_at/*地图对象3D_返回特效模型*/(IMap3DItemPtr* _self/*接口*/, INT index/*索引*/, IMap3DEffectPtr* _return/*返回*/)
//{
//	SceneObject* so		=	GetIPtr(_self, SceneObject, IMap3DItemPtr);
//	if(!so)return;
//	if(index<0||index>=so->m_effects.size())
//		return;
//	SetIPtr(_return, so->m_effects[index], SceneEffect, IMap3DEffectPtr);
//}

//
/*API*/INT			WINAPI	Map3DPoint_id/*地图事件3D_返回ID*/(IMap3DPointPtr* _self/*接口*/)
{
	ScenePoint* so		=	GetIPtr(_self, ScenePoint, IMap3DPointPtr);
	if(!so)
		return 0;
	return so->GetID();
}
/*API*/LPCSTR		WINAPI	Map3DPoint_Getname/*地图事件3D_返回名字*/(IMap3DPointPtr* _self/*接口*/)
{
	ScenePoint* so		=	GetIPtr(_self, ScenePoint, IMap3DPointPtr);
	if(!so)
		return "";
	return so->m_name.t;
}
/*API*/VOID			WINAPI	Map3DPoint_Setname/*地图事件3D_设置名字*/(IMap3DPointPtr* _self/*接口*/, LPCSTR szName/*名字*/)
{
	if(!szName)return;
	ScenePoint* so		=	GetIPtr(_self, ScenePoint, IMap3DPointPtr);
	if(!so)
		return;
	so->m_name = szName;
}
/*API*/VOID			WINAPI	Map3DPoint_Getpos/*地图事件3D_返回位置*/(IMap3DPointPtr* _self/*接口*/, float3* mappos/*地图位置*/)
{
	ScenePoint* so		=	GetIPtr(_self, ScenePoint, IMap3DPointPtr);
	if(!so)
		return;
	*mappos = so->GetPosition();
}
/*API*/VOID			WINAPI	Map3DPoint_Setpos/*地图事件3D_设置位置*/(IMap3DPointPtr* _self/*接口*/, const float3* mappos/*地图位置*/)
{
	if(!mappos)return;
	ScenePoint* so		=	GetIPtr(_self, ScenePoint, IMap3DPointPtr);
	if(!so)
		return;
	so->SetPosition(*mappos);
}
/*API*/float			WINAPI	Map3DPoint_Getrange/*地图事件3D_返回区域*/(IMap3DPointPtr* _self/*接口*/)
{
	ScenePoint* so		=	GetIPtr(_self, ScenePoint, IMap3DPointPtr);
	if(!so)
		return 0;
	return so->GetRange();
}
/*API*/VOID			WINAPI	Map3DPoint_Setrange/*地图事件3D_设置区域*/(IMap3DPointPtr* _self/*接口*/, float range/*大小*/)
{
	ScenePoint* so		=	GetIPtr(_self, ScenePoint, IMap3DPointPtr);
	if(!so)
		return;
	so->SetRange(range);
}

/*API*/VOID			WINAPI	Map3DSkill_source/*地图技能3D_设置源对象*/(IMap3DSkillPtr* _self/*接口*/, LPCSTR szSkillMesh/*技能模型*/, IMap3DItemPtr* item/*目标对象*/, LPCSTR szBoneLike/*骨骼名字*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->SetSkillMesh(szSkillMesh);
	so->SetSource(GetIPtr(item, SceneObject, IMap3DItemPtr), szBoneLike);
}
/*API*/VOID			WINAPI	Map3DSkill_getsource/*地图技能3D_设置源对象*/(IMap3DSkillPtr* _self/*接口*/, IMap3DItemPtr* _return/*目标对象*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	SetIPtr(_return, so->GetSource(), SceneObject, IMap3DItemPtr);
}
/*API*/VOID			WINAPI	Map3DSkill_Setsourcepos/*地图技能3D_设置源对象位置*/(IMap3DSkillPtr* _self/*接口*/, float3* pos/*目标对象*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->SetSourcePos(*pos);
}
/*API*/VOID			WINAPI	Map3DSkill_Getsourcepos/*地图技能3D_取得源对象位置*/(IMap3DSkillPtr* _self/*接口*/, float3* _return/*目标对象*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	*_return = so->GetSourcePos();
}
/*API*/VOID			WINAPI	Map3DSkill_Setcontinuetime/*地图技能3D_设置持续时间*/(IMap3DSkillPtr* _self/*接口*/, float ftime/*速度*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->m_fContinueTime = ftime;
	so->m_bContinueTime = ftime>0;
}
/*API*/float		WINAPI	Map3DSkill_Getcontinuetime/*地图技能3D_取得持续时间*/(IMap3DSkillPtr* _self/*接口*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return 0;
	return so->m_fContinueTime;
}

/*API*/VOID			WINAPI	Map3DSkill_Setsource_zscale/*地图技能3D_设置准备技能高度位置*/(IMap3DSkillPtr* _self/*接口*/, float fHeight/*高度*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->m_fHeight = fHeight;
}
/*API*/float		WINAPI	Map3DSkill_Getsource_zscale/*地图技能3D_设置准备技能高度位置*/(IMap3DSkillPtr* _self/*接口*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return 0;
	return so->m_fHeight;
}
/*API*/VOID			WINAPI	Map3DSkill_Settarget_zscale/*地图技能3D_设置准备技能高度位置*/(IMap3DSkillPtr* _self/*接口*/, float fHeight/*高度*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->m_fHeightTarget = fHeight;
}
/*API*/float		WINAPI	Map3DSkill_Gettarget_zscale/*地图技能3D_设置准备技能高度位置*/(IMap3DSkillPtr* _self/*接口*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return 0;
	return so->m_fHeightTarget;
}
/*API*/VOID			WINAPI	Map3DSkill_Setsize/*地图技能3D_设置准备技能高度位移*/(IMap3DSkillPtr* _self/*接口*/, float fScale/*缩放*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->m_fSourceScale = fScale;
	if(so->m_mesh)
		so->m_mesh->SetParticleScale(so->m_fSourceScale);
}
/*API*/float		WINAPI	Map3DSkill_Getsize/*地图技能3D_设置准备技能高度位移*/(IMap3DSkillPtr* _self/*接口*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return 0;
	return so->m_fSourceScale;
}

/*API*/VOID			WINAPI	Map3DSkill_Setmovespeed/*地图技能3D_设置跟踪速度*/(IMap3DSkillPtr* _self/*接口*/, float speed/*速度*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->m_fSpeed = getmax(speed, 0.001);
}
/*API*/float		WINAPI	Map3DSkill_Getmovespeed/*地图技能3D_取得跟踪速度*/(IMap3DSkillPtr* _self/*接口*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return 0;
	return so->m_fSpeed;
}
/*API*/VOID			WINAPI	Map3DSkill_target/*地图技能3D_设置目标对象*/(IMap3DSkillPtr* _self/*接口*/, IMap3DItemPtr* item/*目标对象*/, LPCSTR szBoneLike/*骨骼名字*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->SetTarget(GetIPtr(item, SceneObject, IMap3DItemPtr), szBoneLike);
}
/*API*/VOID			WINAPI	Map3DSkill_bind_collision/*地图技能3D_绑定碰撞检测*/(IMap3DSkillPtr* _self/*接口*/, int classid/*要进行碰撞的目标类型,如果是0则全部类型*/, LPCSTR szLuaFunc/*当在技能移动时碰撞了就会被调用*/)//只有在targetpos设置有效值的时候才会触发碰撞检测函数，其他任何情况都不会触发,参数是oncollision(Map3DSkill[当前对象],Map3DItem[碰撞对象],float3[位置],float3[碰撞面法线]))
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->BindCollision(classid, szLuaFunc);
}
/*API*/VOID			WINAPI	Map3DSkill_gettarget/*地图技能3D_设置目标对象*/(IMap3DSkillPtr* _self/*接口*/, IMap3DItemPtr* _return/*目标对象*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	SetIPtr(_return, so->GetTarget(), SceneObject, IMap3DItemPtr);
}
/*API*/VOID			WINAPI	Map3DSkill_Settargetpos/*地图技能3D_设置目标对象位置*/(IMap3DSkillPtr* _self/*接口*/, float3* pos/*目标对象*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->SetTargetPos(*pos);
}
/*API*/VOID			WINAPI	Map3DSkill_Gettargetpos/*地图技能3D_设置目标对象位置*/(IMap3DSkillPtr* _self/*接口*/, float3* _return/*目标对象*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	*_return = so->GetTargetPos();
}
/*API*/VOID			WINAPI	Map3DSkill_Setwaittime/*地图技能3D_设置等待时间*/(IMap3DSkillPtr* _self/*接口*/, float fwaittime/*速度*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->m_fWaitTime = fwaittime;
}
/*API*/float		WINAPI	Map3DSkill_Getwaittime/*地图技能3D_取得等待时间*/(IMap3DSkillPtr* _self/*接口*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return 0;
	return so->m_fWaitTime;
}
/*API*/VOID			WINAPI	Map3DSkill_start/*地图技能3D_开始播放技能*/(IMap3DSkillPtr* _self/*接口*/, LPCSTR szFinishFunc/*到达目标点之后被调用的函数*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->StartSkill(szFinishFunc);
}
/*API*/VOID			WINAPI	Map3DSkill_stop/*地图技能3D_停止播放技能*/(IMap3DSkillPtr* _self/*接口*/)
{
	SceneObjectSkill* so		=	GetIPtr(_self, SceneObjectSkill, IMap3DSkillPtr);
	if(!so)return;
	so->SetFree();
}
/*API*/VOID			WINAPI	Map3D_freeskill/*地图3D_返回空闲技能*/(IMap3DPtr* _self/*场景3D*/, IMap3DSkillPtr* _return/*返回*/)//创建一个新的技能，每次技能发起之前都需要调用这个来取得新的空闲技能指针
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SetIPtr(_return, scene->GetFreeSkill(), SceneObjectSkill, IMap3DSkillPtr);
}
/*API*/INT			WINAPI	Map3D_count/*地图3D_返回对象数量*/(IMap3DPtr* _self/*接口*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return 0;
	return scene->GetObjectCount();
}
/*API*/VOID			WINAPI	Map3D_at/*地图3D_返回对象*/(IMap3DPtr* _self/*接口*/, INT index/*索引*/, IMap3DItemPtr* _return/*返回*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneObject* obj = scene->GetObjectPtr(index);
	if(!obj)return;
	SetIPtr(_return, obj, SceneObject, IMap3DItemPtr);
}

/*API*/INT			WINAPI	Map3D_classcount/*地图3D_返回类聚对象数量*/(IMap3DPtr* _self/*接口*/, INT classID/*类ID*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return 0;
	return scene->GetClassCount(classID);
}
/*API*/VOID			WINAPI	Map3D_classat/*地图3D_返回类聚对象*/(IMap3DPtr* _self/*接口*/, INT classID/*类ID*/, INT index/*索引*/, IMap3DItemPtr* _return/*返回*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneObject* obj = scene->GetClassPtr(classID, index);
	if(!obj)return;
	SetIPtr(_return, obj, SceneObject, IMap3DItemPtr);
}

///*API*/VOID			WINAPI	Map3D_Setprotagonist/*地图3D_设置主角*/(IMap3DPtr* _self/*接口*/, IMap3DItemPtr* obj/*对象*/)
//{
//	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
//	if(!scene)
//		return;
//	if(!obj)
//		scene->SetProtagonist(0);
//	else
//		scene->SetProtagonist(GetIPtr(obj, SceneObject, IMap3DItemPtr));
//}
///*API*/VOID			WINAPI	Map3D_Getprotagonist/*地图3D_返回主角*/(IMap3DPtr* _self/*接口*/, IMap3DItemPtr* _return/*返回*/)
//{
//	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
//	if(!scene || !scene->m_mainObj)
//		return;
//	SetIPtr(_return, scene->m_mainObj, SceneObject, IMap3DItemPtr);
//}
/*API*/VOID			WINAPI	Map3D_camerafollow/*地图3D_镜头跟随设置*/(IMap3DPtr* _self/*接口*/, IMap3DItemPtr* obj/*对象*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	scene->SetCameraFollow(GetIPtr(obj, SceneObject, IMap3DItemPtr));
}
/*API*/VOID			WINAPI	Map3D_scenepos/*地图3D_返回场景中的位置*/(IMap3DPtr* _self/*接口*/, Pixel* screen/*屏幕位置*/, Pixel* _return/*位置*/)
{
	_return->Set(0,0);
	if(!screen)return;
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	IntersectData* data = GetCameraPtr()->BeginIntersect(screen->x, screen->y);
	PickPoint po;
	if(!scene->IntersectTerrain(data->GetOriginal(), data->GetDir(), po))
		return;
	_return->Set(po.pos.x, po.pos.y);
}
/*API*/VOID			WINAPI	Map3D_screenpos/*地图3D_返回屏幕中的位置*/(IMap3DPtr* _self/*接口*/, Pixel* p/*场景位置*/, Pixel* _return/*位置*/)
{
	_return->Set(0,0);
	if(!p)return;
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	float3 v(p->x, p->y, 0);
	v.z = scene->GetHeight(v);
	GetCameraPtr()->Get2DFrom3D(*_return, v);
}
/*API*/BOOL			WINAPI	Map3D_setblood/*地图3D_设置场景血条对话框*/(IMap3DPtr* _self/*接口*/, LPCSTR szDlgName/*对象名字*/)
{
	if(!szDlgName)return false;
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return false;
	return scene->LoadBloodDlg(szDlgName);
}
/*API*/BOOL			WINAPI	Map3D_Gethitdisplay/*地图3D_是否显示被击效果*/(IMap3DPtr* _self/*接口*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return false;
	return scene->m_bHitDisplay;
}
/*API*/VOID			WINAPI	Map3D_Sethitdisplay/*地图3D_是否隐藏被击效果*/(IMap3DPtr* _self/*接口*/, BOOL bVisible/*是否隐藏*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	scene->m_bHitDisplay = bVisible;
}
/*API*/VOID			WINAPI	Map3D_searchby/*地图3D_搜素对象*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*返回第一个找到的对象*/, IMap3DItemPtr* _return/*返回*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneObject* obj = scene->GetObjectByName(szItemName);
	if(!obj)
	{
		LogInfo("system->error:not exist Map3D::searchby(\"%s\")", szItemName);
		return;
	}
	SetIPtr(_return, obj, SceneObject, IMap3DItemPtr);
}

/*API*/VOID			WINAPI	Map3D_search/*地图3D_搜素对象ByID*/(IMap3DPtr* _self/*接口*/, INT id/*返回第一个找到的对象*/, IMap3DItemPtr* _return/*返回*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneObject* obj = scene->GetObjectByContentID(id);
	if(!obj){
		LogInfo("system->error:not exist Map3D::search(%d)", id);
	}
	SetIPtr(_return, obj, SceneObject, IMap3DItemPtr);
}
/*API*/VOID			WINAPI	Map3D_searchpointby/*地图3D_搜素移动点*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*返回第一个找到的对象*/, IMap3DPointPtr* _return/*返回*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	ScenePoint* obj = scene->GetActiveEnv()->GetPointByName(szItemName);
	if(!obj)
	{
		LogInfo("system->error:not exist Map3D::searchpointby(\"%s\") at Env(%s)", szItemName, scene->GetActiveEnv()->m_name.t);
		return;
	}
	SetIPtr(_return, obj, ScenePoint, IMap3DPointPtr);
}
/*API*/VOID			WINAPI	Map3D_searchpoint/*地图3D_搜素移动点ByID*/(IMap3DPtr* _self/*接口*/, INT id/*返回第一个找到的对象*/, IMap3DPointPtr* _return/*返回*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	ScenePoint* obj = scene->GetActiveEnv()->GetPointByID(id);
	if(!obj)
	{
		LogInfo("system->error:not exist Map3D::searchpoint(%d) at Env(%s)", id, scene->GetActiveEnv()->m_name.t);
		return;
	}
	SetIPtr(_return, obj, ScenePoint, IMap3DPointPtr);
}
/*API*/VOID			WINAPI	Map3D_searchpointby_eid/*地图3D_搜素移动点根据事件类型ID*/(IMap3DPtr* _self/*接口*/, INT eventid/*返回第一个找到的对象*/, IMap3DPointPtr* _return/*返回*/)//返回搜素移动点,事件ID在event_table.txt中定义，并在编辑器内给移动点设置，只返回第一个
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	ScenePoint* obj = scene->GetActiveEnv()->GetPointByEventID(eventid);
	if(!obj)
	{
		LogInfo("system->error:not exist Map3D::searchpointby_eid(%d) at Env(%s)", eventid, scene->GetActiveEnv()->m_name.t);
		return;
	}
	SetIPtr(_return, obj, ScenePoint, IMap3DPointPtr);
}

/*API*/BOOL			WINAPI	Map3D_Getdisplay/*地图3D_是否显示*/(IMap3DPtr* _self/*接口*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return false;
	return scene->IsEnabled(Scene3D::Enable_ShowScene);
}
/*API*/VOID			WINAPI	Map3D_Setdisplay/*地图3D_是否隐藏*/(IMap3DPtr* _self/*接口*/, BOOL bVisible/*是否隐藏*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	scene->SetEnabled(Scene3D::Enable_ShowScene, bVisible);
}
/*API*/BOOL			WINAPI	Map3D_Getblooddisplay/*地图3D_是否显示血条*/(IMap3DPtr* _self/*接口*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return false;
	return scene->m_bShowBlood;
}
/*API*/VOID			WINAPI	Map3D_Setblooddisplay/*地图3D_是否隐藏血条*/(IMap3DPtr* _self/*接口*/, BOOL bVisible/*是否隐藏*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	scene->m_bShowBlood = bVisible;
}
/*API*/float		WINAPI	Map3D_Getbloodsize/*地图3D_取得血条大小*/(IMap3DPtr* _self/*接口*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return 0;
	return scene->m_fBloodSize;
}
/*API*/VOID			WINAPI	Map3D_Setbloodsize/*地图3D_设置血条大小*/(IMap3DPtr* _self/*接口*/, float fSize/*大小*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	scene->m_fBloodSize = clamp(fSize, 0.01, 10.0);
}

/*API*/BOOL			WINAPI	Map3D_Getdisplayterrain/*地图3D_是否显示地形*/(IMap3DPtr* _self/*接口*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return false;
	return scene->IsEnabled(Scene3D::Enable_ShowTerrain);
}
/*API*/VOID			WINAPI	Map3D_Setdisplayterrain/*地图3D_是否隐藏地形*/(IMap3DPtr* _self/*接口*/, BOOL bVisible/*是否隐藏*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	scene->SetEnabled(Scene3D::Enable_ShowTerrain, bVisible);
}

/*API*/BOOL			WINAPI	Map3D_Getfindway2d/*地图3D_是否2D寻路*/(IMap3DPtr* _self/*接口*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return false;
	return scene->m_bIs2DFindway;
}
/*API*/VOID			WINAPI	Map3D_Setfindway2d/*地图3D_设置2D寻路*/(IMap3DPtr* _self/*接口*/, BOOL b2DMode/*是否*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	scene->m_bIs2DFindway = b2DMode;
}
/*API*/VOID			WINAPI	Map3D_Getcamera/*地图3D_返回镜头*/(IMap3DPtr* _self/*接口*/, Pixel* _return/*位置*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	*_return = scene->GetCameraPos();
}
/*API*/VOID			WINAPI	Map3D_Setcamera/*地图3D_设置镜头*/(IMap3DPtr* _self/*接口*/, Pixel* pos/*位置*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene || !pos)
		return;
	scene->SetCameraPos(*pos);
}
//
///*API*/INT			WINAPI	Map3D_eventcount/*地图3D_返回事件数量*/(IMap3DPtr* _self/*接口*/)
//{
//	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
//	if(!scene)
//		return 0;
//	return scene->m_events.size();
//}
///*API*/VOID			WINAPI	Map3D_eventat/*地图3D_返回事件*/(IMap3DPtr* _self/*接口*/, INT index/*索引*/, IMap3DPointPtr* _return/*返回*/)
//{
//	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
//	if(!scene)
//		return;
//	SetIPtrI(_return, scene, Scene3D, IMap3DPointPtr, index);
//}
///*API*/INT			WINAPI	Map3D_eventcount/*地图3D_返回事件数量*/(IMap3DPtr* _self/*接口*/);//返回对象数量
///*API*/VOID			WINAPI	Map3D_eventat/*地图3D_返回事件*/(IMap3DPtr* _self/*接口*/, INT index/*索引*/, IMap3DPointPtr* _return/*返回*/);//返回第N个对象
///*API*/VOID			WINAPI	Map3D_addevent/*地图3D_添加新事件*/(IMap3DPtr* _self/*接口*/, LPCSTR szName/*事件名*/, Pixel* mappos/*位置*/, IMap3DPointPtr* _return/*返回*/);//新建角色
///*API*/VOID			WINAPI	Map3D_delevent/*地图3D_删除事件*/(IMap3DPtr* _self/*接口*/, IMap3DPointPtr* ptr/*返回*/);//新建角色
///*API*/VOID			WINAPI	Map3D_searchevent/*地图3D_搜索并返回事件*/(IMap3DPtr* _self/*接口*/, LPCSTR szEvent/*事件名*/, IMap3DPointPtr* _return/*返回*/);//新建角色
//
///*API*/VOID			WINAPI	Map3D_addevent/*地图3D_添加新事件*/(IMap3DPtr* _self/*接口*/, LPCSTR szName/*事件名*/, Pixel* mappos/*位置*/, IMap3DPointPtr* _return/*返回*/)
//{
//	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
//	if(!scene || !szName || !mappos)
//		return;
//	for(int i=0;i<scene->m_events.size();i++)
//	{
//		if(scene->m_events[i].id==0)
//		{
//			scene->m_events[i].name = szName;
//			scene->m_events[i].pos = *mappos;
//			SetIPtrI(_return, scene, Scene3D, IMap3DPointPtr, i);
//			return;
//		}
//	}
//	int size = scene->m_events.size();
//	SceneEventDB evt;
//	evt.id = scene->GetNewIdentity();
//	evt.pos = *mappos;
//	evt.name = szName;
//	scene->m_events.push_back(evt);
//	SetIPtrI(_return, scene, Scene3D, IMap3DPointPtr, size);
//}
///*API*/VOID			WINAPI	Map3D_delevent/*地图3D_删除事件*/(IMap3DPtr* _self/*接口*/, IMap3DPointPtr* ptr/*返回*/)
//{
//	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
//	if(!scene)
//		return;
//	if(ptr->reserved<0||ptr->reserved>=scene->m_events.size())
//		return;
//	scene->m_events[ptr->reserved].id = 0;
//}
///*API*/VOID			WINAPI	Map3D_searchevent/*地图3D_搜索并返回事件*/(IMap3DPtr* _self/*接口*/, LPCSTR szEvent/*事件名*/, IMap3DPointPtr* _return/*返回*/)
//{
//	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
//	if(!scene)
//		return;
//	for(int i=0;i<scene->m_events.size();i++)
//	{
//		if(scene->m_events[i].name == szEvent)
//		{
//			SetIPtrI(_return, scene, Scene3D, IMap3DPointPtr, i);
//			return;
//		}
//	}
//}

/*API*/VOID			WINAPI	Map3D_replace_image/*地图3D_绑定图片*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*对象名字*/, LPCSTR szImageSrc/*原图片文件路径*/, LPCSTR szImageReplace/*替换图片文件路径*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneObject* obj = scene->GetObjectByName(szItemName);
	if(!obj)return;
	if(!szImageSrc||!szImageReplace)return;
	Mesh* gg	=	obj->GetMeshPtr();
	if(!gg)return;
	gg->ReplaceImage(szImageSrc, szImageReplace);
}
/*API*/VOID			WINAPI	Map3D_bind_eventby/*地图3D_绑定脚本到按下事件*/(IMap3DPtr* _self/*接口*/, LPCSTR szEvent/*事件名*/, LPCSTR szItemName/*对象名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneObject* obj = scene->GetObjectByName(szItemName);
	if(!obj)return;
	obj->bind_event(GetEventID(szEvent), szLuaFunc);
}
/*API*/VOID			WINAPI	Map3D_bind_event/*地图3D_绑定脚本到按下事件*/(IMap3DPtr* _self/*接口*/, EventID nid/*事件名*/, LPCSTR szItemName/*对象名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneObject* obj = scene->GetObjectByName(szItemName);
	if(!obj)return;
	obj->bind_event(nid, szLuaFunc);
}
///*API*/VOID			WINAPI	Map3D_initGame/*地图3D_初始化游戏数据*/(IMap3DPtr* _self/*接口*/)//从地形初始化所有的游戏数据,地形对象与游戏对象关联
//{
//	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
//	if(!scene)
//		return;
//	scene->BindAllObjects();
//}
/*API*/VOID			WINAPI	Map3D_bind_click/*地图3D_绑定脚本到事件*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*对象名字*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneObject* obj = scene->GetObjectByName(szItemName);
	if(!obj)return;
	obj->bind_event(Phantom::EventID_Click, szLuaFunc);
}
/*API*/VOID			WINAPI	Map3D_bind_draw/*地图3D_绑定渲染队列*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*对象名字*/, IUIDrawMgrPtr* drawPtr/*组合*/, LPCSTR szBoneName)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneObject* obj = scene->GetObjectByName(szItemName);
	if(!obj)return;
	if(!obj->GetMeshPtr())return;
	obj->GetMeshPtr()->BindDraw(szBoneName, GetIPtr(drawPtr, UIDrawMgr, IUIDrawMgrPtr));
}
/*API*/BOOL			WINAPI	Map3D_Getpause/*地图3D_是否暂停*/(IMap3DPtr* _self/*接口*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return false;
	return scene->m_bPause;
}
/*API*/VOID			WINAPI	Map3D_Setpause/*地图3D_是否暂停*/(IMap3DPtr* _self/*接口*/, BOOL bPause/*是否暂停*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	scene->Pause(bPause);
}
/*API*/VOID			WINAPI	Map3D_insert/*地图3D_插入对象*/(IMap3DPtr* _self/*接口*/, LPCSTR szFileName/*模型路径*/, Pixel* mappos/*地图坐标*/, IMap3DItemPtr* _return/*返回*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene||!mappos)
		return;
	float3 pos(mappos->x, mappos->y, 0);
	SceneObject* obj = scene->InsertObject(szFileName, pos);
	SetIPtr(_return, obj, SceneObject, IMap3DItemPtr);
}
/*API*/VOID			WINAPI	Map3D_remove/*地图3D_删除对象*/(IMap3DPtr* _self/*接口*/, IMap3DItemPtr* _item/*返回*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneObject* obj = GetIPtr(_item, SceneObject, IMap3DItemPtr);
	scene->RemoveObject(obj);
}

/*API*/VOID			WINAPI	Map3D_show/*地图3D_显示对象*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*对象名字*/, BOOL bShow/*是否显示*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneObject* obj = scene->GetObjectByName(szItemName);
	if(!obj)return;
	obj->SetVisible(bShow);
}
/*API*/VOID			WINAPI	Map3D_enable/*地图3D_让对象有效*/(IMap3DPtr* _self/*接口*/, LPCSTR szItemName/*对象名字*/, BOOL bEnable/*是否有效*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneObject* obj = scene->GetObjectByName(szItemName);
	if(!obj)return;
	obj->SetEnabled(bEnable);
}
/*API*/VOID			WINAPI	Map3D_show_s/*地图3D_显示静态对象*/(IMap3DPtr* _self/*接口*/, INT nID/*对象ID*/, BOOL bShow/*是否显示*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneStaticMesh* smesh = scene->SearchStaticObject(nID);
	if(!smesh)return;
	smesh->SetVisible(bShow);
}
/*API*/VOID			WINAPI	Map3D_enable_s/*地图3D_让静态对象有效*/(IMap3DPtr* _self/*接口*/, INT nID/*对象ID*/, BOOL bEnable/*是否有效*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneStaticMesh* smesh = scene->SearchStaticObject(nID);
	if(!smesh)return;
	smesh->SetEnabled(bEnable);
}
/*API*/VOID			WINAPI	Map3D_bind_mapeventby/*地图3D_绑定场景脚本事件*/(IMap3DPtr* _self/*接口*/, LPCSTR szEvent/*事件名*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	scene->bind_event(GetEventID(szEvent), szLuaFunc);
}
/*API*/VOID			WINAPI	Map3D_bind_mapevent/*地图3D_绑定场景脚本事件*/(IMap3DPtr* _self/*接口*/, EventID nid/*事件ID*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	scene->bind_event(nid, szLuaFunc);
}

/*API*/LPCSTR	WINAPI	Map3D_Getactivecamera/*地图3D_取得激活镜头*/(IMap3DPtr* _self/*场景3D*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return "";
	SceneCameraData* data = scene->m_cameraDataPtr.FindDataPtr(scene->m_activeCamera);
	if(!data)
		return "";
	return data->name;
}
/*API*/VOID		WINAPI	Map3D_Setactivecamera/*地图3D_设置单位名字大小*/(IMap3DPtr* _self/*场景3D*/, LPCSTR szCamera/*设置*/)
{
	if(!szCamera)
		return;
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	scene->SetActiveCamera(szCamera);
}

/*API*/BOOL			WINAPI	Map3D_Getenablemyclick/*地图3D_是否可以点击地形*/(IMap3DPtr* _self/*接口*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return false;
	return scene->m_bEnableTerrainEvent;
}
/*API*/VOID			WINAPI	Map3D_Setenablemyclick/*地图3D_是否可以点击地形*/(IMap3DPtr* _self/*接口*/, BOOL bEnabled/*是否有效*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(scene)
		scene->m_bEnableTerrainEvent = bEnabled;
}
/*API*/BOOL			WINAPI	Map3D_Getenabled/*地图3D_是否有效并能点击*/(IMap3DPtr* _self/*接口*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return false;
	return scene->m_bEnabled;
}
/*API*/VOID			WINAPI	Map3D_Setenabled/*地图3D_是否有效并能点击*/(IMap3DPtr* _self/*接口*/, BOOL bEnabled/*是否有效*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(scene)
		scene->m_bEnabled = bEnabled;
}
/*API*/int			WINAPI	Map3D_Getclickradius/*地图3D_返回点击有效范围*/(IMap3DPtr* _self/*接口*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(scene)
		return scene->m_fTerrainDownRadius;
	return 0;
}
/*API*/VOID			WINAPI	Map3D_Setclickradius/*地图3D_设置点击有效范围*/(IMap3DPtr* _self/*接口*/, int radius/*范围*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(scene)
		scene->m_fTerrainDownRadius = radius;
}
/*API*/LPCSTR	WINAPI	Map3D_Getsky/*地图3D_取得激活镜头*/(IMap3DPtr* _self/*场景3D*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return "";
	return "";
}
/*API*/VOID		WINAPI	Map3D_Setsky/*地图3D_设置单位名字大小*/(IMap3DPtr* _self/*场景3D*/, LPCSTR szCamera/*设置*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
}

/*API*/VOID		WINAPI	Map3D_blend_camera/*地图3D_设置单位名字大小*/(IMap3DPtr* _self/*场景3D*/, LPCSTR szCamera/*设置*/, float fBlendTime/*混合时间*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
}
/*API*/VOID		WINAPI	Map3D_blend_fog/*地图3D_设置单位名字大小*/(IMap3DPtr* _self/*场景3D*/, LPCSTR szCamera/*设置*/, float fBlendTime/*混合时间*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
}
/*API*/VOID		WINAPI	Map3D_blend_sky/*地图3D_设置单位名字大小*/(IMap3DPtr* _self/*场景3D*/, LPCSTR szCamera/*设置*/, float fBlendTime/*混合时间*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
}

/*API*/VOID			WINAPI	Sky_Setname/*场景天空3D_设置名字*/(ISkyPtr* _self/*场景天空*/, LPCSTR szName/*名字*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || !szName || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	sky->name	=	szName;
}
/*API*/LPCSTR		WINAPI	Sky_Getname/*场景天空3D_取名字*/(ISkyPtr* _self/*场景天空*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return "";
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	return sky->name;
}
/*API*/VOID			WINAPI	Sky_Setcolor/*场景天空3D_设置颜色*/(ISkyPtr* _self/*场景天空*/, const color4* c/*颜色*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	sky->skyColor	=	*c;
}
/*API*/VOID			WINAPI	Sky_Getcolor/*场景天空3D_取颜色*/(ISkyPtr* _self/*场景天空*/, color4* _return/*ret*/)
{
	_return->setRGBA(1,1,1,1);
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	*_return	=	color4(sky->skyColor);
}
/*API*/VOID			WINAPI	Sky_Setambient/*场景天空3D_设置环境色*/(ISkyPtr* _self/*场景天空*/, const color4* c/*颜色*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	sky->ambientColor	=	*c;
}
/*API*/VOID		WINAPI	Sky_Getambient/*场景天空3D_取环境色*/(ISkyPtr* _self/*场景天空*/, color4* _return/*ret*/)
{
	_return->setRGBA(1,1,1,1);
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	*_return	=	sky->ambientColor;
}
/*API*/VOID			WINAPI	Sky_Setsunangle_x/*场景天空3D_设置阳光角度X*/(ISkyPtr* _self/*场景天空*/, float c/*转动*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	sky->fAngleX	=	Radians(c);
}
/*API*/float		WINAPI	Sky_Getsunangle_x/*场景天空3D_取阳光角度X*/(ISkyPtr* _self/*场景天空*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return 0;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	return Degress(sky->fAngleX);
}
/*API*/VOID			WINAPI	Sky_Setsunangle_z/*场景天空3D_设置阳光角度Z*/(ISkyPtr* _self/*场景天空*/, float c/*转动*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	sky->fAngleZ	=	Radians(c);
}
/*API*/float		WINAPI	Sky_Getsunangle_z/*场景天空3D_取阳光角度Z*/(ISkyPtr* _self/*场景天空*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return 0;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	return Degress(sky->fAngleZ);
}
/*API*/VOID			WINAPI	Sky_Setwindangle/*场景天空3D_设置风向*/(ISkyPtr* _self/*场景天空*/, float c/*风向*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	sky->fWindDirection	=	Radians(c);
}
/*API*/float		WINAPI	Sky_Getwindangle/*场景天空3D_取风向*/(ISkyPtr* _self/*场景天空*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return 0;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	return Degress(sky->fWindDirection);
}
/*API*/VOID			WINAPI	Sky_Setwindpower/*场景天空3D_设置风力*/(ISkyPtr* _self/*场景天空*/, float c/*风力*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	sky->fWindPower	=	c;
}
/*API*/float		WINAPI	Sky_Getwindpower/*场景天空3D_取风力*/(ISkyPtr* _self/*场景天空*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, ISkyPtr);
	if(!scene || _self->reserved>=scene->GetSkyDataPtr()->GetDataCount())return 0;
	SceneSkyData* sky = scene->GetSkyDataPtr()->GetDataPtr(_self->reserved);
	return sky->fWindPower;
}

/*API*/LPCSTR	WINAPI	Map3D_GetactiveEnvBy/*地图3D_取得激活镜头*/(IMap3DPtr* _self/*场景3D*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return "";
	return  scene->GetActiveEnv()->GetName();
}
/*API*/VOID		WINAPI	Map3D_SetactiveEnvBy/*地图3D_设置单位名字大小*/(IMap3DPtr* _self/*场景3D*/, LPCSTR szName/*设置*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneEnv* env = 0;
	if(szName){
		for(int i=0;i<scene->m_sceneEnvs.size();i++){
			if(scene->m_sceneEnvs[i]->m_name==szName)
			{
				env = scene->m_sceneEnvs[i];
				break;
			}
		}
	}
	scene->ActiveEnv(env);
}
/*API*/VOID			WINAPI	Map3D_GetactiveEnv/*地图3D_取得激活雾方案*/(IMap3DPtr* _self/*场景3D*/, IMap3DEnvPtr* _return/*设置*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneEnv* env = scene->GetActiveEnv();
	SetIPtr(_return, env, SceneEnv, IMap3DEnvPtr);
}
/*API*/VOID			WINAPI	Map3D_SetactiveEnv/*地图3D_设置激活雾方案*/(IMap3DPtr* _self/*场景3D*/, IMap3DEnvPtr* fog/*设置*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene||!fog)
		return;
	SceneEnv* env = GetIPtr(fog, SceneEnv, IMap3DEnvPtr);
	if(env)
		scene->ActiveEnv(env);
}

/*API*/BOOL			WINAPI	Map3DEnv_Getenabled_brightness/*地图3D_是否有效亮度*/(IMap3DEnvPtr* _self/*接口*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return false;
	return env->m_bBrightness;
}
/*API*/VOID			WINAPI	Map3DEnv_Setenabled_brightness/*地图3D_是否有效亮度*/(IMap3DEnvPtr* _self/*接口*/, BOOL bEnabled/*是否有效*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	env->m_bBrightness = bEnabled;
	if(env->IsActive())
		env->CommitBrightness();
}
/*API*/BOOL			WINAPI	Map3DEnv_Getenabled_contrast/*地图3D_是否有效对比度*/(IMap3DEnvPtr* _self/*接口*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return false;
	return env->m_bContrast;
}
/*API*/VOID			WINAPI	Map3DEnv_Setenabled_contrast/*地图3D_是否有效对比度*/(IMap3DEnvPtr* _self/*接口*/, BOOL bEnabled/*是否有效*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	env->m_bContrast = bEnabled;
	if(env->IsActive())
		env->CommitContrast();
}
/*API*/float		WINAPI	Map3DEnv_Getbrightness/*地图3D_是否亮度*/(IMap3DEnvPtr* _self/*接口*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return 0;
	return env->m_fBrightness;
}
/*API*/VOID			WINAPI	Map3DEnv_Setbrightness/*地图3D_是否亮度*/(IMap3DEnvPtr* _self/*接口*/, float fBrightness/*亮度*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	env->m_fBrightness = fBrightness;
	if(env->IsActive())
		env->CommitBrightness();
}
/*API*/float		WINAPI	Map3DEnv_Getcontrast/*地图3D_是否对比度*/(IMap3DEnvPtr* _self/*接口*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return 0;
	return env->m_fContrast;
}
/*API*/VOID			WINAPI	Map3DEnv_Setcontrast/*地图3D_是否对比度*/(IMap3DEnvPtr* _self/*接口*/, float fContrast/*对比度*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	env->m_fContrast = fContrast;
	if(env->IsActive())
		env->CommitContrast();
}
///*API*/float		WINAPI	Map3DEnv_Getshadow_front/*场景环境3D_取得阴影位置向前移动距离*/(IMap3DEnvPtr* _self/*接口*/)
//{
//	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
//	if(!env)return 0;
//	return ToMeter(env->m_shadowFront);
//}
///*API*/VOID			WINAPI	Map3DEnv_Setshadow_front/*场景环境3D_设置阴影位置向前移动距离*/(IMap3DEnvPtr* _self/*接口*/, float ffar)
//{
//	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
//	if(!env)return;
//	env->m_shadowFront = To3DValue(ffar);
//	env->m_shadowMode = SceneEnv::ShadowMode_Front;
//}

/*API*/VOID			WINAPI	Map3DEnv_Setshadow_center_item/*场景环境3D_设置阴影中心点对象*/(IMap3DEnvPtr* _self/*接口*/, IMap3DItemPtr* obj)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	SceneObject* so		=	GetIPtr(obj, SceneObject, IMap3DItemPtr);
	if(!so)return;
	env->m_shadowCenterObj.id = so->GetID();
	env->m_shadowMode = SceneEnv::ShadowMode_Object;
}
/*API*/VOID			WINAPI	Map3DEnv_Getshadow_center_item/*场景环境3D_取得阴影中心点对象*/(IMap3DEnvPtr* _self/*接口*/, IMap3DItemPtr* _return)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	SceneObject* so = env->GetScenePtr()->SearchObject(env->m_shadowCenterObj);
	if(!so)return;
	SetIPtr(_return, so, SceneObject, IMap3DItemPtr);
}
/*API*/VOID			WINAPI	Map3DEnv_Setshadow_center/*场景环境3D_设置阴影位置*/(IMap3DEnvPtr* _self/*接口*/, const float3* pos)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	env->m_shadowCenterObj.id = 0;
	env->m_shadowCenter = *pos;
	env->m_shadowMode = SceneEnv::ShadowMode_Center;
}
/*API*/VOID			WINAPI	Map3DEnv_Getshadow_center/*场景环境3D_取得阴影位置*/(IMap3DEnvPtr* _self/*接口*/, float3* _return)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	if(env->m_shadowCenterObj.id>0){
		SceneObject* so = env->GetScenePtr()->SearchObject(env->m_shadowCenterObj);
		if(so)
			*_return = so->GetPosition();
	}
	else
		*_return = env->m_shadowCenter;
}
/*API*/VOID			WINAPI	System_vr_front/*系统_设置VR正前方*/(const float3* dir)
{
	GetCameraPtr()->SetVRFront(*dir);
}
/*API*/VOID			WINAPI	Map3DEnv_Setshadow_size/*场景环境3D_设置阴影扩散范围*/(IMap3DEnvPtr* _self/*接口*/, float fSize/*影响范围*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	env->m_shadowViewSize = To3DValue(fSize);
	g_manager.m_shadowViewSize = env->m_shadowViewSize;
}
/*API*/float		WINAPI	Map3DEnv_Getshadow_size/*场景环境3D_取得阴影扩散范围*/(IMap3DEnvPtr* _self/*接口*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return 0;
	return ToMeter(env->m_shadowViewSize);
}

/*API*/VOID			WINAPI	Map3DEnv_Setname/*场景雾3D_设置名字*/(IMap3DEnvPtr* _self/*场景天空*/, LPCSTR szName/*名字*/)
{
	if(!szName)return;
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	env->m_name = szName;
}
/*API*/LPCSTR		WINAPI	Map3DEnv_Getname/*场景雾3D_取名字*/(IMap3DEnvPtr* _self/*场景天空*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return "";
	return env->GetName();
}

/*API*/VOID			WINAPI	Map3DEnv_Setcolor/*场景雾3D_设置颜色*/(IMap3DEnvPtr* _self/*场景天空*/, const color4* c/*颜色*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	env->m_fogColor	=	*c;
	if(env->IsActive())
		env->CommitFog();
}
/*API*/VOID			WINAPI	Map3DEnv_Getcolor/*场景雾3D_取颜色*/(IMap3DEnvPtr* _self/*场景天空*/, color4* _return)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	*_return	=	env->m_fogColor;
}
/*API*/VOID			WINAPI	Map3DEnv_Setnear/*场景雾3D_设置最近*/(IMap3DEnvPtr* _self/*场景天空*/, float c/*最近*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	env->m_fNear	=	c;
	if(env->IsActive())
		env->CommitFog();
}
/*API*/float		WINAPI	Map3DEnv_Getnear/*场景雾3D_取最近*/(IMap3DEnvPtr* _self/*场景天空*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return 0;
	return env->m_fNear;
}
/*API*/VOID			WINAPI	Map3DEnv_Setfar/*场景雾3D_设置最远*/(IMap3DEnvPtr* _self/*场景天空*/, float c/*最远*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	env->m_fFar	=	c;
	if(env->IsActive())
		env->CommitFog();
}
/*API*/float		WINAPI	Map3DEnv_Getfar/*场景雾3D_取最远*/(IMap3DEnvPtr* _self/*场景天空*/)
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return 0;
	return env->m_fFar;
}
/*API*/VOID			WINAPI	Map3DEnv_Setmode/*场景雾3D_设置雾模式*/(IMap3DEnvPtr* _self/*场景天空*/, FogMode mode/*模式*/)//0=3D雾，1=2D雾，2=2D雾+灰度
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return;
	env->m_fogType = mode;
	if(env->IsActive())
		env->CommitFog();
}
/*API*/FogMode			WINAPI	Map3DEnv_Getmode/*场景雾3D_取雾模式*/(IMap3DEnvPtr* _self/*场景天空*/)//0=3D雾，1=2D雾，2=2D雾+灰度
{
	SceneEnv* env = GetIPtr(_self, SceneEnv, IMap3DEnvPtr);
	if(!env)return (FogMode)0;
	return (FogMode)env->m_fogType;
}

/*API*/BOOL			WINAPI	Map3D_isunwalkable/*全局_是否禁止移动*/(const float3* pos/*当前位置*/)
{
	return false;
}
/*API*/float		WINAPI	Map3D_getheight_3d/*全局_根据碰撞子模型取高度*/(const float3* pos/*位置*/)
{
	return 0;
}
/*API*/VOID			WINAPI	Map3D_ray3d/*全局_碰撞检测_仅与碰撞子模型*/(const Pixel* pixel/*屏幕坐标位置*/, IMeshPtr* _return/*返回*/)
{
}
/*API*/VOID			WINAPI	Map3D_ray3d_all/*全局_碰撞检测_自动*/(const Pixel* pixel/*屏幕坐标位置*/, IMeshPtr* _return/*返回*/)
{
}
/*API*/VOID			WINAPI	Map3D_ray3d_dir/*全局_碰撞检测_仅与碰撞子模型*/(const float3* orig/*原点*/, const float3* dir/*方向*/, IMeshPtr* _return/*返回*/)
{
}
/*API*/VOID			WINAPI	Map3D_ray3d_dir_all/*全局_碰撞检测_自动*/(const float3* orig/*原点*/, const float3* dir/*方向*/, IMeshPtr* _return/*返回*/)
{
}
/*API*/BOOL			WINAPI	Map3D_ray3d_box/*全局_碰撞检测_盒子*/(const float3* vOrig, const float3* vDir, const AABox* box, const matrix4x4* pWorld)
{
	return false;
}

/*API*/INT			WINAPI	Map3D_skycount/*地图3D_取加天空数量*/(IMap3DPtr* _self/*场景3D*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return 0;
	return scene->GetSkyDataPtr()->GetDataCount();
}
/*API*/VOID			WINAPI	Map3D_skyat/*地图3D_取天空*/(IMap3DPtr* _self/*场景3D*/, INT nIndex/*索引*/, ISkyPtr* _return/*ret*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene||nIndex<0||nIndex>=scene->GetSkyDataPtr()->GetDataCount())
		return;
	SetIPtrI(_return, scene, Scene3D, ISkyPtr, nIndex);
}
/*API*/INT			WINAPI	Map3D_envcount/*地图3D_取雾数量*/(IMap3DPtr* _self/*场景3D*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return 0;
	return scene->m_sceneEnvs.size()+1;
}
/*API*/VOID			WINAPI	Map3D_envat/*地图3D_取雾*/(IMap3DPtr* _self/*场景3D*/, INT nIndex/*索引*/, IMap3DEnvPtr* _return/*ret*/)
{
	Scene3D* scene		=	GetIPtr(_self, Scene3D, IMap3DPtr);
	if(!scene)
		return;
	SceneEnv* env = 0;
	if(nIndex == 0)
		env = &scene->m_defaultEnv;
	else if(nIndex>0&&nIndex<=scene->m_sceneEnvs.size()){
		nIndex--;
		env = scene->m_sceneEnvs[nIndex];
	}
	if(!env)
		return;
	SetIPtr(_return, env, SceneEnv, IMap3DEnvPtr);
}


/*API*/INT			WINAPI	UIBodyPart_Gettextureid/*身体部件_取纹理ID*/(IBodyPartPtr* _self/*接口*/)//在界面管理器中能找到对应的纹理文件
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	return bp->m_textureID;
}
/*API*/VOID			WINAPI	UIBodyPart_Settextureid/*身体部件_设置纹理ID*/(IBodyPartPtr* _self/*接口*/, INT nID/*id*/)//在界面管理器中能找到对应的纹理ID
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetTextureID(nID);
}
/*API*/VOID			WINAPI	UIBodyPart_Setfontid/*身体部件_设置字体ID*/(IBodyPartPtr* _self/*接口*/, INT nFontID/*字体ID*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetFontID(nFontID);
}
/*API*/INT			WINAPI	UIBodyPart_Getfontid/*身体部件_取字体ID*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	return bp->m_fontID;
}
/*API*/VOID			WINAPI	UIBodyPart_Gettexturerect/*身体部件_取得纹理区域*/(IBodyPartPtr* _self/*接口*/, Rect* _return/*ret*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	*_return = bp->m_textureRect;
}
/*API*/VOID			WINAPI	UIBodyPart_Settexturerect/*身体部件_设置纹理区域*/(IBodyPartPtr* _self/*接口*/, const Rect* _s/*矩形*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetTextureRect(*_s);
}
/*API*/VOID			WINAPI	UIBodyPart_texture_mode/*身体部件_设置方案名字*/(IBodyPartPtr* _self/*接口*/, LPCSTR szMode/*模式*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetTextureMode(szMode);
}
/*API*/LPCSTR		WINAPI	UIBodyPart_Getuv_animation/*身体部件_取激活UV动画*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return "";
	return bp->GetUVAnimation();
}
/*API*/VOID			WINAPI	UIBodyPart_Setuv_animation/*身体部件_设置激活UV动画*/(IBodyPartPtr* _self/*接口*/, LPCSTR map/*UV动画名字*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetUVAnimation(map);
}
/*API*/ActionType	WINAPI	UIBodyPart_Getuv_animationby/*身体部件_取激活UV动画*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return Phantom::ActionType_None;
	return bp->GetUVAnimationType();
}
/*API*/VOID			WINAPI	UIBodyPart_Setuv_animationby/*身体部件_设置激活UV动画*/(IBodyPartPtr* _self/*接口*/, ActionType map/*动作类型*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetUVAnimationType(map);
}
/*API*/VOID			WINAPI	UIBodyPart_Setuv_speed/*身体部件_设置UV播放速度*/(IBodyPartPtr* _self/*接口*/, float f/*播放速度*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetUVSpeed(f);
}
/*API*/float		WINAPI	UIBodyPart_Getuv_speed/*身体部件_取UV播放速度*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 1;
	return bp->m_uvspeed;
}
/*API*/float		WINAPI	UIBodyPart_Getcurrenttime/*身体部件_取播放时间*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	return bp->m_uvCurrentTime;
}
/*API*/VOID			WINAPI	UIBodyPart_Setcurrenttime/*身体部件_设置播放时间*/(IBodyPartPtr* _self/*接口*/, float fFrame/*时间*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetCurrentTime(fFrame);
}
/*API*/float		WINAPI	UIBodyPart_timelength/*身体部件_取播放时长*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	return bp->GetTimeLength();
}
/*API*/VOID			WINAPI	UIBodyPart_Setprogress_value/*身体部件_设置进度条值*/(IBodyPartPtr* _self/*接口*/, float value/*值*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetProgressValue(value);
}
/*API*/float		WINAPI	UIBodyPart_Getprogress_value/*身体部件_取进度条值*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	return bp->m_fProgressVal;
}
/*API*/VOID			WINAPI	UIBodyPart_Setenabled/*身体部件_设置有效*/(IBodyPartPtr* _self/*接口*/, BOOL b/*有效*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->m_bEnabled = b;
}
/*API*/BOOL			WINAPI	UIBodyPart_Getenabled/*身体部件_是否有效*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	return bp->m_bEnabled;
}
/*API*/VOID			WINAPI	UIBodyPart_Setdisplay/*身体部件_设置显示*/(IBodyPartPtr* _self/*接口*/, BOOL b/*有效*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->m_bVisible = b;
}
/*API*/BOOL			WINAPI	UIBodyPart_Getdisplay/*身体部件_是否显示*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	return bp->m_bVisible;
}
/*API*/INT			WINAPI	UIBodyPart_Getlayer/*身体部件_取层级*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	return bp->m_layer;
}
/*API*/VOID			WINAPI	UIBodyPart_Setlayer/*身体部件_设置层级*/(IBodyPartPtr* _self/*接口*/, INT layer/*层级*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetLayer(layer);
}
/*API*/VOID			WINAPI	UIBodyPart_Setcolor/*身体部件_设置纹理颜色*/(IBodyPartPtr* _self/*接口*/, const color4* c/*颜色*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetTextureColor(*c);
}
/*API*/VOID			WINAPI	UIBodyPart_Getcolor/*身体部件_取纹理颜色*/(IBodyPartPtr* _self/*接口*/, color4* _return/*ret*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	*_return = bp->m_textureColor;
}
/*API*/VOID			WINAPI	UIBodyPart_Setopacity/*身体部件_设置透明值*/(IBodyPartPtr* _self/*接口*/, float v/*颜色*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	color4 c = bp->m_textureColor;
	c.a = clamp(v, 0, 1);
	bp->SetTextureColor(c);
}
/*API*/float		WINAPI	UIBodyPart_Getopacity/*身体部件_取透明值*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	color4 c = bp->m_textureColor;
	return c.a;
}
/*API*/VOID			WINAPI	UIBodyPart_Setcenter/*身体部件_设置中心位置*/(IBodyPartPtr* _self/*接口*/, const Pixel* pos/*ID*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->m_center = *pos;
}
/*API*/VOID			WINAPI	UIBodyPart_Getcenter/*身体部件_取得中心位置*/(IBodyPartPtr* _self/*接口*/, Pixel* _return/*ret*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	*_return = bp->m_center;
}
/*API*/float		WINAPI	UIBodyPart_Getrot/*身体部件_设置转动率*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	return Degress(bp->m_fRotation);
}
/*API*/VOID			WINAPI	UIBodyPart_Setrot/*身体部件_设置转动率*/(IBodyPartPtr* _self/*接口*/, float fAngle/*角度*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->m_fRotation = Radians(fAngle);
}
/*API*/LPCSTR		WINAPI	UIBodyPart_Gettext/*身体部件_取文本*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return "";
	const char* ret = bp->m_textVal.GetPtr();
	if(!ret)ret="";
	return ret;
}
/*API*/VOID			WINAPI	UIBodyPart_Settext/*身体部件_设置文本*/(IBodyPartPtr* _self/*接口*/, LPCSTR szText/*文本*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetText(szText);
}
/*API*/ActionType	WINAPI	UIBodyPart_Getaction_next/*身体部件_取下一个播放动作*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return ActionType_None;
	return bp->GetNextActionType();
}
/*API*/VOID			WINAPI	UIBodyPart_Setaction_next/*身体部件_设置下一个播放动作*/(IBodyPartPtr* _self/*接口*/, ActionType nType/*动作*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetNextActionType(nType);
}
/*API*/LPCSTR		WINAPI	UIBodyPart_Getactionby/*身体部件_取当前动作根据名称*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return "";
	return bp->GetAction();
}
/*API*/VOID			WINAPI	UIBodyPart_Setactionby/*身体部件_设置当前动作根据名称*/(IBodyPartPtr* _self/*接口*/, LPCSTR ani/*当前动作*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp||!ani)return;
	bp->SetAction(ani);
}
/*API*/ActionType	WINAPI	UIBodyPart_Getaction/*身体部件_取播放动作*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return ActionType_None;
	return bp->GetActionType();
}
/*API*/VOID			WINAPI	UIBodyPart_Setaction/*身体部件_设置播放动作*/(IBodyPartPtr* _self/*接口*/, ActionType nType/*动作*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetActionType(nType);
}
/*API*/float		WINAPI	UIBodyPart_Getaction_speed/*身体部件_取得动作播放速度*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	return bp->m_actionSpeed;
}
/*API*/VOID			WINAPI	UIBodyPart_Setaction_speed/*身体部件_设置动作播放速度*/(IBodyPartPtr* _self/*接口*/, float fSpeed/*速度*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetActionSpeed(fSpeed);
}
/*API*/float		WINAPI	UIBodyPart_Getrot2d/*身体部件_取得角度*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	return bp->m_fRot2DAngle;
}
/*API*/VOID			WINAPI	UIBodyPart_Setrot2d/*身体部件_设置角度*/(IBodyPartPtr* _self/*接口*/, float fAngle/*角度*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetRot2DAngle(fAngle);
}
/*API*/VOID			WINAPI	UIBodyPart_rot2d_dir/*身体部件_设置角度根据方向*/(IBodyPartPtr* _self/*接口*/, const Pixel* dir/*角度*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetRot2DDir(*dir);
}
/*API*/float		WINAPI	UIBodyPart_Getscale/*身体部件_缩放*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return 0;
	return bp->m_fScale;
}
/*API*/VOID			WINAPI	UIBodyPart_Setscale/*身体部件_缩放*/(IBodyPartPtr* _self/*接口*/, float fScale/*缩放*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetScale(fScale);
}
/*API*/VOID			WINAPI	UIBodyPart_bind_image/*身体部件_绑定图片*/(IBodyPartPtr* _self/*接口*/, LPCSTR szImageFile/*图片文件路径*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetTextureFile(szImageFile);
}

/*API*/VOID			WINAPI	UIBodyPart_Setpos/*身体部件_设置对话框位置*/(IBodyPartPtr* _self/*接口*/, Pixel* pos/*最新位置*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->SetPos(*pos);
}
/*API*/VOID			WINAPI	UIBodyPart_Getpos/*身体部件_取位置左*/(IBodyPartPtr* _self/*接口*/, Pixel* _return/*矩形*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	*_return = bp->m_position;
}
/*API*/VOID			WINAPI	UIBodyPart_emitter_stop/*身体部件_停止释放粒子*/(IBodyPartPtr* _self/*接口*/)
{
	UIControlRT* bp		=	GetIPtr(_self, UIControlRT, IBodyPartPtr);
	if(!bp)return;
	bp->StopEmit();
}
//UIDialogRT
/*API*/VOID			WINAPI	Global_avatar_create/*全局_创建纸娃娃*/(LPCSTR szFileName/*rpgui文件名*/, IAvatarPtr* _return/*纸娃娃*/)
{
	UIDialog* dlg = GetDlgManager()->LoadAvatar(szFileName);//自动载入，如果不存在则自动创建
	if(!dlg)return;
	UIDialogRT* a = new UIDialogRT(dlg);
	SetIPtr(_return,a,UIDialogRT,IAvatarPtr);
	g_luamanager.Push(a->GetPtrID());
}
/*API*/VOID			WINAPI	Global_avatar_delete/*全局_删除纸娃娃系统*/(IAvatarPtr* ptr/*纸娃娃*/)
{
	UIDialogRT* av		=	GetIPtr(ptr, UIDialogRT, IAvatarPtr);
	if(!av)return;
	safe_release(av);
}
/*API*/VOID			WINAPI	UIAvatar_pick/*纸娃娃_点选按钮和可以被激活的控件*/(IAvatarPtr* _self/*接口*/, INT x/*x*/, INT y/*y*/, IBodyPartPtr* _return/*ret*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	UIControlRT* bp = av->Pick(x,y);
	if(!bp)return;
	SetIPtr(_return, bp, UIControlRT, IBodyPartPtr);
}
/*API*/INT			WINAPI	UIAvatar_count/*纸娃娃_取控件数量*/(IAvatarPtr* _self/*接口*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return 0;
	return av->m_bodys.size();
}
/*API*/VOID			WINAPI	UIAvatar_at/*纸娃娃_取控件指针*/(IAvatarPtr* _self/*接口*/, INT index/*索引*/, IBodyPartPtr* _return/*ret*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	if(index<0||index>=av->m_bodys.size())
		return;
	UIControlRT* bp = av->m_bodys[index];
	SetIPtr(_return, bp, UIControlRT, IBodyPartPtr);
}
/*API*/VOID			WINAPI	UIAvatar_search/*纸娃娃_查找控件指针*/(IAvatarPtr* _self/*接口*/, LPCSTR szName/*名称*/, IBodyPartPtr* _return/*ret*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	UIControlRT* bp = av->FromName(szName);
	if(!bp)return;
	SetIPtr(_return, bp, UIControlRT, IBodyPartPtr);
}
/*API*/ActionType	WINAPI	UIAvatar_Getaction/*纸娃娃_取播放动作*/(IAvatarPtr* _self/*接口*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return ActionType_None;
	return (ActionType)av->m_actionType;
}
/*API*/VOID			WINAPI	UIAvatar_Setaction/*纸娃娃_设置播放动作*/(IAvatarPtr* _self/*接口*/, ActionType nType/*动作*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->SetActionType(nType);
}

/*API*/BOOL			WINAPI	UIAvatar_Getdisplay/*纸娃娃_是否显示*/(IAvatarPtr* _self/*接口*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return false;
	return av->m_bVisible;
}
/*API*/VOID			WINAPI	UIAvatar_Setdisplay/*纸娃娃_显示*/(IAvatarPtr* _self/*接口*/, BOOL b/*设置*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->SetVisible(b);
}
/*API*/VOID			WINAPI	UIAvatar_Getcolor/*纸娃娃_取对话框颜色透明值*/(IAvatarPtr* _self/*接口*/, color4* _return/*返回*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	*_return = av->m_color;
}
/*API*/VOID			WINAPI	UIAvatar_Setcolor/*纸娃娃_设置对话框颜色透明值*/(IAvatarPtr* _self/*接口*/, const color4* val/*值*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->m_color = *val;
}
/*API*/float		WINAPI	UIAvatar_Getopacity/*纸娃娃_取对话框透明值*/(IAvatarPtr* _self/*接口*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return 0;
	return av->m_color.a;
}
/*API*/VOID			WINAPI	UIAvatar_Setopacity/*纸娃娃_设置对话框透明值*/(IAvatarPtr* _self/*接口*/, float val/*值*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->SetOpacity(clamp(val, 0, 1));
}
/*API*/float		WINAPI	UIAvatar_Getscale/*纸娃娃_缩放*/(IAvatarPtr* _self/*接口*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return 0;
	return av->m_fScale;
}
/*API*/VOID			WINAPI	UIAvatar_Setscale/*纸娃娃_缩放*/(IAvatarPtr* _self/*接口*/, float fScale/*缩放*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->SetScale(fScale);
}
/*API*/VOID			WINAPI	UIAvatar_Getcenter/*纸娃娃_取中心点位置*/(IAvatarPtr* _self/*接口*/, Pixel* _return/*矩形*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	*_return = av->m_center;
}
/*API*/VOID			WINAPI	UIAvatar_Setcenter/*纸娃娃_设置中心点位置*/(IAvatarPtr* _self/*接口*/, const Pixel* _val/*矩形*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->SetCenter(*_val);
}

/*API*/float		WINAPI	UIAvatar_Getrot/*纸娃娃_取转动X*/(IAvatarPtr* _self/*接口*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return 0;
	return Degress(av->m_fRotation);
}
/*API*/VOID			WINAPI	UIAvatar_Setrot/*纸娃娃_设置转动X*/(IAvatarPtr* _self/*接口*/, float fAngle/*角度*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->SetRotation(Radians(fAngle));
}
/*API*/VOID			WINAPI	UIAvatar_Setpos/*纸娃娃_设置对话框位置*/(IAvatarPtr* _self/*接口*/, const Pixel* _val/*矩形*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->SetPos(*_val);
}
/*API*/VOID			WINAPI	UIAvatar_Getpos/*纸娃娃_取位置左*/(IAvatarPtr* _self/*接口*/, Pixel* _return/*矩形*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	*_return = av->m_pos;
}
/*API*/VOID			WINAPI	UIAvatar_bind_draw/*纸娃娃_绑定渲染队列*/(IAvatarPtr* _self/*接口*/, IUIDrawMgrPtr* drawPtr/*组合*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->BindDraw(GetIPtr(drawPtr, UIDrawMgr, IUIDrawMgrPtr));
}
/*API*/VOID			WINAPI	UIAvatar_bind_image/*纸娃娃_绑定图片*/(IAvatarPtr* _self/*接口*/, LPCSTR szBody/*身体部件名字*/, LPCSTR szImageFile/*图片文件路径*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	UIControlRT* bd = av->FromName(szBody);
	if(!bd)return;
	bd->SetTextureFile(szImageFile);
}
/*API*/VOID			WINAPI	UIAvatar_image_mode/*纸娃娃_绑定图片模式*/(IAvatarPtr* _self/*接口*/, LPCSTR szBody/*身体部件名字*/, LPCSTR szMode/*图片模式*/, DrawTextAlign align/*对齐*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	UIControlRT* bd = av->FromName(szBody);
	if(!bd)return;
	bd->SetTextureMode(szMode);
}
///*API*/VOID			WINAPI	UIAvatar_bind_click/*纸娃娃_绑定脚本到事件*/(IAvatarPtr* _self/*接口*/, LPCSTR szLuaFunc/*Lua脚本函数名*/)
//{
//	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
//	if(!av)return;
//	av->BindClick(szLuaFunc);
//}
/*API*/VOID			WINAPI	UIAvatar_show/*纸娃娃_显示控件*/(IAvatarPtr* _self/*接口*/, LPCSTR szBody/*身体部件名字*/, BOOL bShow/*是否显示*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	UIControlRT* bd = av->FromName(szBody);
	if(!bd)return;
	bd->SetVisible(bShow);
}
/*API*/VOID			WINAPI	UIAvatar_enable/*纸娃娃_让控件有效*/(IAvatarPtr* _self/*接口*/, LPCSTR szBody/*身体部件名字*/, BOOL bEnable/*是否有效*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	UIControlRT* bd = av->FromName(szBody);
	if(!bd)return;
	bd->SetEnabled(bEnable);
}
/*API*/VOID			WINAPI	UIAvatar_Setlayer/*纸娃娃_设置显示层级*/(IAvatarPtr* _self/*接口*/, INT nLayer/*层级，从-30000到30000*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->SetLayer(nLayer);
}
/*API*/INT			WINAPI	UIAvatar_Getlayer/*纸娃娃_取得显示层级*/(IAvatarPtr* _self/*接口*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return 0;
	return av->m_layer;
}
//
/*API*/float		WINAPI	UIAvatar_Getspeed/*纸娃娃_取得动作播放速度*/(IAvatarPtr* _self/*接口*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return 0;
	return av->m_actionSpeed;
}
/*API*/VOID			WINAPI	UIAvatar_Setspeed/*纸娃娃_设置动作播放速度*/(IAvatarPtr* _self/*接口*/, float fSpeed/*速度*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->SetActionSpeed(fSpeed);
}
/*API*/VOID			WINAPI	UIAvatar_Setenabled/*纸娃娃_设置有效*/(IAvatarPtr* _self/*接口*/, BOOL b/*有效*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->SetEnabled(b);
}
/*API*/BOOL			WINAPI	UIAvatar_Getenabled/*纸娃娃_是否有效*/(IAvatarPtr* _self/*接口*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return 0;
	return av->m_bEnabled;
}
/*API*/float		WINAPI	UIAvatar_Getrot2d/*纸娃娃_取得角度*/(IAvatarPtr* _self/*接口*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return 0;
	return av->m_fRot2DAngle;
}
/*API*/VOID			WINAPI	UIAvatar_Setrot2d/*纸娃娃_设置角度*/(IAvatarPtr* _self/*接口*/, float fAngle/*角度*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->SetRot2DAngle(fAngle);
}
/*API*/VOID			WINAPI	UIAvatar_rot2d_dir/*纸娃娃_设置角度根据方向*/(IAvatarPtr* _self/*接口*/, const Pixel* dir/*角度*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->SetRot2DDir(*dir);
}

/*API*/VOID			WINAPI	UIAvatar_pushaction/*纸娃娃_增加下一个播放动作*/(IAvatarPtr* _self/*接口*/, ActionType nType/*动作*/)
{
	UIDialogRT* av		=	GetIPtr(_self, UIDialogRT, IAvatarPtr);
	if(!av)return;
	av->PushAction(nType);
}
/*API*/VOID		WINAPI	UIDialog_Setlayer/*界面对话框_设置显示层级*/(IDialogPtr* _self/*接口*/, INT nLayer/*层级，从-30000到30000*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)
		return;
	d->m_layer = nLayer;
	GetDlgManager()->m_bResortDialogs = true;
}
/*API*/INT		WINAPI	UIDialog_Getlayer/*界面对话框_设置显示层级*/(IDialogPtr* _self/*接口*/)
{
	UIDialog* d	=	GetIPtr(_self, UIDialog, IDialogPtr);
	if(!d)
		return 0;
	return d->m_layer;
}
/*API*/ActionType	WINAPI	UIControl_Getaction/*界面控件_取播放动作*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return ActionType_None;
	UIAnimationTrack* track	=	c->GetActiveTrack();
	if(!track)
		return ActionType_None;
	return track->GetActionType();
}
/*API*/ActionType	WINAPI	UIControl_Getnextaction/*界面控件_取下一个播放动作*/(IControlPtr* _self/*接口*/)
{
	UIControl* c	=	GetIPtrT(_self, PhantomUIControlBase, IControlPtr);
	if(!c)return ActionType_None;
	return (ActionType)c->m_nNextPlayAction;
}

/*API*/VOID			WINAPI	System_sensor/*全局_获取传感器数据*/(sensordata* _return/*返回*/)//获取传感器数据，包括陀螺仪角速度和空间位置等数据
{
	*_return = g_manager.m_sensordata;
}
/*API*/int			WINAPI	Global_text_create/*全局_文本创建*/(LPCSTR szText/*文本*/, int x/*左*/, int y/*上*/)//返回文本ID
{
	return g_manager.AddText(szText, x, y);
}
/*API*/VOID			WINAPI	Global_text_update/*全局_文本创建*/(int nID, LPCSTR szText/*文本*/)
{
	g_manager.SetText(nID, szText);
}
/*API*/int			WINAPI	Global_text_create2/*全局_文本创建2*/(LPCSTR szText/*文本*/, int x/*左*/, int y/*上*/, const color4* c/*颜色*/, int size/*文字大小*/)
{
	return g_manager.AddText(szText, x, y, *c, size);
}
/*API*/VOID			WINAPI	Global_text_delete/*全局_文本删除*/(int nID)//根据ID删除文本，从text_create中返回
{
	g_manager.RemoveText(nID);
}

/*API*/VOID			WINAPI	System_gravity/*全局_获取传感器调整之后重力感应值*/(float2* _return/*返回*/)//获取传感器重力感应数据，左右和前后控制，在任意屏幕转向下都是向左是接近-1，向右是接近1，直立是接近0，横放是接近1
{
	_return->x = g_manager.m_sensordata.gravity_x;
	_return->y = g_manager.m_sensordata.gravity_y;
}
/*API*/VOID			WINAPI	System_Setgravity_smooth/*系统_开启Gravity平滑数*/(int s/*间距*/)
{
	g_manager.m_nGravitySmooth = s;
}
/*API*/int			WINAPI	System_Getgravity_smooth/*系统_是否开启Gravity间距*/()
{
	return g_manager.m_nGravitySmooth;
}

extern void			initState(const char* szFile, unsigned int* guid, int length);
extern VOID			reloadState();
extern VOID			ResetStateMgr();
extern VOID			SetAutoSave(BOOL b);
/*API*/VOID			WINAPI	Global_configfile/*全局_设置配置文件名字*/(LPCSTR name/*配置文件名字*/)
{
	ResetStateMgr();
	initState(name, 0, 0);
	reloadState();
}
/*API*/VOID			WINAPI	Global_configsave/*全局_设置配置数据是否自动保存*/(BOOL b/*自动保存*/)
{
	SetAutoSave(b);
}

};
