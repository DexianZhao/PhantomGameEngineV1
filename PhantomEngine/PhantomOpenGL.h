//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   PhantomOpenGL2_0.cpp
	幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

	Design Writer :   赵德贤 Dexian Zhao
	Email: yuzhou_995@hotmail.com
	
	
	Copyright 2009-2016 Zhao Dexian
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __PHANTOM_OPENGL_H__
#define	__PHANTOM_OPENGL_H__

#ifndef DIRECT_X_9
#include "PhantomDevice.h"
#include "PhantomFile.h"
#include "PhantomManager.h"

#ifndef	NDEBUG
#define BASE_CHECK_RET(x) x; { if(!Phantom::GlErrorOutput()) return false; }
#define BASE_CHECK_NR(x) x; { if(!Phantom::GlErrorOutput()) return;}
#else
#define	BASE_CHECK_RET(x)	x
#define	BASE_CHECK_NR(x)	x
#endif
extern "C"{
    typedef void (*PBindVertexArray)(uint32_t array);
    typedef void (*PDeleteVertexArrays)(int32_t n, const uint32_t *arrays);
    typedef void (*PGenVertexArrays)(int32_t n, uint32_t *arrays);
    VOID    SetupGL2();
};

namespace Phantom{

	class	Device_OGL;
	class	DeviceProgram_OGL;
	//
	class	Device_OGL: public Device
	{
	public:
		Device_OGL();
		~Device_OGL();
        virtual	VOID											ClearError();
		virtual	classT<Texture, classMgr<Texture> >*			CreateTexture();
		virtual	Program*										CreateProgram(int mode);
		virtual	classT<DeviceMesh, classMgr<DeviceMesh> >*		CreateMesh();
		virtual	RenderTarget*									CreateShadowMap();
		virtual	RenderText*										CreateRenderText();
		//
		VOID							Clear(UINT flags);
		virtual	VOID					AppCreate();
		virtual	VOID					SetCullMode(CullMode mode);
		virtual	VOID					SetBlendMode(BlendMode src, BlendMode dest);
        virtual VOID					SetUIClipRect(const Rect* rect);
		virtual	VOID					SetViewport(int x, int y, int w, int h);
		virtual	VOID					ResetAllDefault();
		virtual	VOID					SetEnableZ(BOOL b);
		virtual	VOID					SetZWriteEnable(BOOL b);
		virtual	VOID					SetAlphaMode(AlphaMode mode);
		VOID							SetDrawInfo(const DrawVInfos& info, DeviceProgram_OGL* prog);
		VOID							DrawIndexed(Program* prog, const DrawVInfos& info, const unsigned short* tList, int numIndices, DrawMode mode);
		VOID							DrawNoIndexed(Program* prog, const DrawVInfos& info, int numVertices, DrawMode mode);
		virtual	VOID					SetSampler(SamplerState sampler, TextureFilter filter);
		//
		int								m_CullMode[CullMode_Count];
		int								m_BlendMode[BlendMode_Count];
		int								m_SamplerState[SamplerState_Max];
		int								m_DrawMode[DrawMode_Count];
		virtual	VOID					OnLostDevice(){}
		virtual	VOID					OnResetDevice(){}
        //
        static BOOL                     s_bIsGL3;//if system support OpenGLES3
        static PBindVertexArray         s_fnBindVertex;
        static PDeleteVertexArrays      s_fnDeleteVertex;
        static PGenVertexArrays         s_fnGenVertex;
	public:
		VOID							SetUsageName(int usage, const char* szName);
		static const int	MAX_USAGE_INDEX	=	8;
		Name							m_usageName[VertexElement::VE_USAGE_COUNT][MAX_USAGE_INDEX];
	};

	class	DeviceTexture_OGL: public classT<Texture, classMgr<Texture> >
	{
	public:
		unsigned int		m_texID;
		DeviceTexture_OGL();
		~DeviceTexture_OGL();
		VOID					Destroy();
		BOOL					IsLoaded(){return (m_texID > 0);}
		//BOOL					LoadTextureFromFile(const char* szFile, BOOL bGenMipmap, BOOL bGenAlpha, int nFilter);
		//VOID					GenericAlphas();
		VOID					OnResetDevice();
		VOID					OnLostDevice();
		VOID					SetSampler(SamplerState sampler, TextureFilter filter);
		VOID					OnLoadDevice();
		//BOOL					LoadTextureData();
	};
	class	DeviceRenderText_OGL: public RenderText
	{
	public:
        virtual BOOL                    IsInit(){return (m_texID>0);}
		unsigned int			m_texID;
		DeviceRenderText_OGL();
		~DeviceRenderText_OGL();
		VOID					OnResetDevice();
		VOID					OnLostDevice();
		//
#ifdef ANDROID_OS
		VOID					OnResetAndroidDevice(int* pixels, int width, int height, int textWidth, int textHeight);
#endif
	};
	//渲染目标
	class	DeviceShadowMap_OGL: public RenderTarget
	{
	public:
		DeviceShadowMap_OGL();
		~DeviceShadowMap_OGL();
		virtual	BOOL			Begin();//开始渲染
		virtual	VOID			End();//结束渲染
		void					OnLostDevice();
		VOID					OnResetDevice();	//根据渲染模式创建shader
	public:
		uint32_t				m_depthBuffer;
		uint32_t				m_frameBuffer;
		uint32_t				m_renderBuffer;
		uint32_t				m_textureBuffer;
		int32_t					m_saveFrameBuffer;
	};
	//渲染方法
	class	DeviceProgram_OGL: public Program
	{
	public:
		DeviceProgram_OGL(int mode);
		void					OnLostDevice();
		VOID					OnResetDevice();	//根据渲染模式创建shader
		//
		BOOL					FindAttrib(_shader_var& out, const char* szVariantName);//查找attribute shader元素
		BOOL					FindVariant(_shader_var& out, const char* szVariantName);//查找uniform shader元素
		//
		BOOL					SetPointer(_shader_var& var, int size, int type, int normalized, int stride, const void* ptr, const char* szName);
        BOOL                    SetPointer(_shader_var& var, int size, int type, const void* ptr, const char* szName);
        BOOL                    SetPointer(_shader_var& var, const float3* ptr, const char* szName);
        BOOL                    SetPointer(_shader_var& var, const color4* ptr, const char* szName);
        BOOL                    SetPointer(_shader_var& var, const float2* ptr, const char* szName);
		//
		BOOL					SetPointer(_shader_var_name& var, int size, int type, int normalized, int stride, const void* ptr);
        BOOL                    SetPointer(_shader_var_name& var, int size, int type, const void* ptr);
        BOOL                    SetPointer(_shader_var_name& var, const float3* ptr);
        BOOL                    SetPointer(_shader_var_name& var, const color4* ptr);
        BOOL                    SetPointer(_shader_var_name& var, const float2* ptr);
		BOOL					SetPointerEnable(_shader_var& var, BOOL bEnable, const char* szName);
		//
		BOOL					SetMatrix(_shader_var_name& var, const matrix4x4* mat);//
		BOOL					SetMatrix(_shader_var& var, const matrix4x4* mat, const char* szName);//
		BOOL					SetMatrixPtr(_shader_var& var, const matrix4x4* v, int nCount, const char* szName);
		BOOL					SetTexture(_shader_var_name& var, Texture*tex);//
		BOOL					Active();
		VOID					SetSampler(SamplerState sampler, TextureFilter filter);
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
		//
		inline	BOOL			IsCreateFail(){return m_bFail;}
		inline	BOOL			IsCreated(){return m_bCreated;}
		inline	unsigned int	GetProgramID(){return m_programID;}
		VOID					SetTexture(TextureIndex index, Texture* tex);
		VOID					SetTextTexture(TextureIndex index, RenderText* tex);//渲染文本的时候被调用
		virtual	VOID			OnCreateShader();
	public:
		AutoPtr<Texture>		m_varTextPtr[TextureIndex_Max];		//world view projection
		~DeviceProgram_OGL();
		_shader_var				m_varPositions;	//顶点位置列表
		_shader_var				m_varColors;	//顶点颜色列表
		_shader_var				m_varNormals;	//顶点颜色列表
		_shader_var				m_varUVs;		//顶点UV坐标列表
        char                    m_bOESAttr;
		//_shader_var				m_varUV2s;		//顶点UV坐标列表
		//_shader_var				m_varUV3s;		//顶点UV坐标列表
		//_shader_var				m_varSpecular;	//顶点颜色列表
		//_shader_var				m_varUV4s;		//顶点UV坐标列表
		//_shader_var				m_varUV5s;		//顶点UV坐标列表
	protected:
		unsigned int			m_programID;
		unsigned int			m_vertID;
		unsigned int			m_pixelID;
	};

	struct	OGLElement
	{
		_shader_var				var;
		unsigned short			offset;
		unsigned short			type;
		unsigned char			typeCount;
		unsigned char			normalized;
		unsigned char			usage;
		unsigned char			usageIndex;
		OGLElement(){offset = usage = type = usageIndex = 0; normalized = 0; typeCount = 0;}
	};
	struct	OGLMeshAttribute
	{
		int								indexBuffer;
		CDynamicArray<Triangle>			faces;
		int								numFaces;
		OGLMeshAttribute(){indexBuffer	=	-1; numFaces = 0;}
	};
	class	DeviceMesh_OGL: public classT<DeviceMesh, classMgr<DeviceMesh> >
	{
	public:
		DeviceMesh_OGL();
		virtual ~DeviceMesh_OGL();
		virtual	VOID					Create(INT nNumVertices, INT nNumFaces, VertexElement* elements, int elementCount);
		VOID							Destroy();
		virtual	BOOL					IsLoaded(){return (m_vertexBuffer >= 0);}
	public:
		virtual	VOID					UpdateIndexBuffer(const unsigned short* indexPtr, int indexCount);
		VOID							WriteIndexBuffer(const unsigned short* indexPtr, int indexCount, unsigned char* attrib);
		virtual	VOID					OnResetDevice();
		virtual	VOID					OnLostDevice();
		VOID							SetVertexDecl(VertexElement* elements, int elementCount);
		BOOL							RenderMesh(Program* prog, Texture* tex, const matrix4x4* mat);
		BOOL							RenderSubset(Program* p, DWORD dwAttrib);
        VOID                            SetupOES(DeviceProgram_OGL* prog);
		virtual	VOID					UpdateVertexBuffer(const void* vertexPtr, int vertexCount);
		virtual	BOOL					RenderIndexed(Program* prog, int numTriangles);	//渲染
	public:
		CDynamicArray<OGLElement,short,0>	m_elementAttrs;
		int									m_vertexBuffer;
		CDynamicArray<OGLMeshAttribute*>	m_meshAttribs;
        int                                 m_meshOES;
		//int						m_indexBuffer;
	};
};


#endif

#endif
