//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

	Design Writer :   赵德贤 Dexian Zhao
	Email: yuzhou_995@hotmail.com
	Copyright 2009-2016 Zhao Dexian

	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___PHANTOM_DIRECTX9__H___
#define ___PHANTOM_DIRECTX9__H___
#include "PhantomDevice.h"
#include "PhantomWin32.h"
#include "PhantomManager.h"
#ifdef DIRECT_X_9
#include <d3dx9.h>

#ifndef NDEBUG
#define BASE_CHECK_RET(x) {HRESULT hr = x; if(FAILED(hr)){Phantom::DxErrorOutput(hr); return false;}}
#define BASE_CHECK_NR(x) {HRESULT hr = x; if(FAILED(hr))Phantom::DxErrorOutput(hr);}
#else
#define	BASE_CHECK_RET(x)	x
#define	BASE_CHECK_NR(x)	x
#endif
extern	IDirect3DDevice9*				GetDevice9();
extern	HWND	GetHWND();
namespace Phantom{
	class	Device_Device9: public Device
	{
	public:
		Device_Device9();
		~Device_Device9();
		virtual	VOID													ClearError(){}
		virtual	classT<Texture, classMgr<Texture> >*					CreateTexture();
		virtual	classT<DeviceMesh, classMgr<DeviceMesh> >*				CreateMesh();
		virtual	RenderText*												CreateRenderText();
		virtual	Program*												CreateProgram(int mode);
		virtual	RenderTarget*											CreateShadowMap();
		//
		VOID							Clear(UINT flags);
		virtual VOID					SetUIClipRect(const Rect* rect);//设置UI裁剪,如果是0则不做任何裁剪
		virtual	VOID					SetCullMode(CullMode mode);
		virtual	VOID					SetBlendMode(BlendMode src, BlendMode dest);
		virtual	VOID					SetViewport(int x, int y, int w, int h);
		virtual	VOID					ResetAllDefault();
		virtual	VOID					SetEnableZ(BOOL b);
		virtual	VOID					SetZWriteEnable(BOOL b);
		virtual	VOID					SetAlphaMode(AlphaMode mode);
		VOID							SetSampler(SamplerState sampler, TextureFilter filter);
		const void*						SetDrawInfo(Program* prog, const DrawVInfos& info, int& strideSize);
		VOID							DrawIndexed(Program* prog, const DrawVInfos& info, const unsigned short* tList, int numIndices, DrawMode mode);
		VOID							DrawNoIndexed(Program* prog, const DrawVInfos& info, int numVertices, DrawMode mode);
		virtual	VOID					OnLostDevice();
		virtual	VOID					OnResetDevice();
		int								AddDecl(UINT nFVF);
		int								AddDecl(D3DVERTEXELEMENT9* ele, int numElement);
		VOID							ActiveDecl(Program* prog, int index);
		//
		struct Decl{
			IDirect3DVertexDeclaration9*	decl;
			D3DVERTEXELEMENT9				vdecl[MAX_DECL_COUNT];
			int								ndecl;
		};
		CDynamicArray<Decl>				m_decls;
		//要将DynamicMesh设置FVF
		int								m_declColor;
		int								m_declColorUV;
		int								m_declNormalColorUV;
		int								m_declUV;
		Program*						m_lastProgDecl;
		int								m_lastDecl;
	};

	class	DeviceTexture_Device9: public classT<Texture, classMgr<Texture> >
	{
	public:
		IDirect3DTexture9*	m_texturePtr;
		int	m_dxSamplerFilter[SamplerState_Max];
		DeviceTexture_Device9();
		~DeviceTexture_Device9();
		VOID					Destroy();
		BOOL					IsLoaded(){return (m_texturePtr != 0);}
		VOID					OnResetDevice();
		VOID					OnLoadDevice();
		VOID					OnLostDevice();
		VOID					SetSampler(SamplerState sampler, TextureFilter filter);
	};

	class	DeviceRenderText_Device9: public RenderText
	{
	public:
		IDirect3DTexture9*	m_texturePtr;
        virtual BOOL                    IsInit(){return (m_texturePtr!=0);}

		DeviceRenderText_Device9();
		~DeviceRenderText_Device9();
		VOID					SetText(const char* text);
		VOID					OnResetDevice();
		VOID					OnLostDevice();
		CDynamicArray<short_t>	m_textW;
	};
	//渲染目标
	class	DeviceShadowMap_Device9: public RenderTarget
	{
	public:
		DeviceShadowMap_Device9();
		~DeviceShadowMap_Device9();
		virtual	BOOL			Begin();//开始渲染
		virtual	VOID			End();//结束渲染
		void					OnLostDevice();
		VOID					OnResetDevice();	//根据渲染模式创建shader
	public:
		IDirect3DTexture9*		m_pRenderTarget;
		IDirect3DSurface9*		m_pRenderSurface;
		IDirect3DSurface9*		m_pZBuffer;
		IDirect3DSurface9*		m_pSaveRenderTargetBuffer;
		IDirect3DSurface9*		m_pSaveZBuffer;
	};
	//渲染方法
	class	DeviceProgram_Device9: public Program
	{
	public:
		DeviceProgram_Device9(int mode);
		void					OnLostDevice();
		VOID					OnResetDevice();	//根据渲染模式创建shader
		//
		BOOL					FindVariant(_shader_var& out, const char* szVariantName);//查找attribute shader元素
		//
		BOOL					SetMatrix(_shader_var& var, const matrix4x4* mat, const char* szName);//
		BOOL					SetMatrix(_shader_var_name& var, const matrix4x4* mat);//
		BOOL					SetTexture(_shader_var_name& var, Texture*tex);//
		VOID					SetTextTexture(TextureIndex index, RenderText* tex);//渲染文本的时候被调用
		BOOL					Active();
		//
		BOOL					SetInt(_shader_var_name& var, int i);
		BOOL					SetInt(_shader_var& var, int f, const char* szName);
		BOOL					SetFloat(_shader_var& var, float f, const char* szName);
		BOOL					SetFloat2(_shader_var& var, const float2& v, const char* szName);
		BOOL					SetFloat3(_shader_var& var, const float3& v, const char* szName);
		BOOL					SetFloat4(_shader_var& var, const float4& v, const char* szName);
		//
		BOOL					SetFloatPtr(_shader_var& var, const float* f, int nCount, const char* szName);
		BOOL					SetFloat2Ptr(_shader_var& var, const float2* v, int nCount, const char* szName);
		BOOL					SetFloat3Ptr(_shader_var& var, const float3* v, int nCount, const char* szName);
		BOOL					SetFloat4Ptr(_shader_var& var, const float4* v, int nCount, const char* szName);
		BOOL					SetMatrixPtr(_shader_var& var, const matrix4x4* v, int nCount, const char* szName);
		//
		inline	BOOL			IsCreateFail(){return m_bFail;}
		inline	BOOL			IsCreated(){return m_bCreated;}
		VOID					SetTexture(TextureIndex index, Texture* tex);
		virtual	VOID			OnCreateShader();
	public:
		_shader_var				m_h_minFilter,m_h_magFilter,m_h_mipFilter;
		AutoPtr<Texture>		m_varTextPtr[TextureIndex_Max];		//world view projection
		~DeviceProgram_Device9();
		LPD3DXEFFECT			m_shaderPtr;
		D3DXHANDLE				m_hMain;
	};
	struct	DirectXMeshAttribute
	{
		int								indexBuffer;
		CDynamicArray<Triangle>			faces;
		int								numFaces;
		DirectXMeshAttribute(){indexBuffer	=	-1; numFaces = 0;}
	};

	class	DeviceMesh_Device9: public classT<DeviceMesh, classMgr<DeviceMesh> >
	{
	public:
		DeviceMesh_Device9();
		virtual ~DeviceMesh_Device9();
		BOOL					IsLoaded();
		VOID					Destroy();
		VOID					GenElements();
	public:
		virtual	VOID					UpdateIndexBuffer(const unsigned short* indexPtr, int indexCount);
		virtual	VOID					WriteIndexBuffer(const unsigned short* indexPtr, int indexCount, unsigned char* attrib);
		virtual	VOID					UpdateVertexBuffer(const void* vertexPtr, int vertexCount);
		virtual	BOOL					RenderIndexed(Program* prog, int numTriangles);	//渲染
		virtual	BOOL					RenderMesh(Program* prog, Texture* tex, const matrix4x4* mat);
		virtual	BOOL					RenderSubset(Program* p, DWORD dwAttrib);
		virtual	VOID					SetVertexFVF(INT nFVF);
		virtual	BOOL					OptimizeMesh();
		virtual	VOID					OnResetDevice();
		virtual	VOID					OnLostDevice();
	public:
		//ID3DXMesh*					m_meshPtr;//
		int								m_decl;
		CDynamicArray<DirectXMeshAttribute*>	m_meshAttribs;
	};
};

#endif

#endif
