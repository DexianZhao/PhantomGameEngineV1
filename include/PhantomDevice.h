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
#ifndef ___PHANTOM_DEVICE__H___
#define ___PHANTOM_DEVICE__H___
#include "PhantomBase.h"
#ifdef DIRECT_X_9
#define	DIRECT_X
#endif
#define	PROGRAM_BONE_COUNT	55
#define	MAX_DECL_COUNT		16
namespace Phantom{
	//
	class	Manager;
	class	Program;
	class	DeviceMesh;
#pragma pack(push,1)
	struct	ImageBuffer
	{
		enum BPP{
			PixelFormat_RGB = 1,
			PixelFormat_RGBA = 2,
			PixelFormat_RGB_565 = 3,
			PixelFormat_RGBA_4444 = 4,
		};
		unsigned int	width;
		unsigned int	height;
		unsigned int	oriwidth;
		unsigned int	oriheight;
		//
		int				nDiv;
		int				bpp;
		unsigned char*	pixels;
		//
		unsigned char*	alphas;	//使用与像素级测试的选项
		//
		BOOL			IsAlphaImage(){return (bpp == PixelFormat_RGBA||bpp == PixelFormat_RGBA_4444);}//是否透明图片
		BOOL			IsAlpha(int x, int y);	//是否透明
		BOOL			SetAlpha(int x, int y);
		BOOL			GenericAlpha();
		VOID			ToSmallRGB();
		INT				GetBufferSize();
		VOID			AutoResize(const char* szFile);
		//
		VOID			FreePixels(BOOL bAlpha = true);
		ImageBuffer();
		~ImageBuffer();
	};
#pragma pack(pop)
	//重要提示：手机上图片必须得是2的冪, 比如2*2*2*2*2...
	class	Texture: public Base
	{
	public:
		enum ImageFormat{
			ImageFormat_Unknown	=	0,
			ImageFormat_Png,
			ImageFormat_Tga,
			ImageFormat_Jpg,
			ImageFormat_Bmp,
		};

		ImplementComValue();
		Texture();
		virtual	~Texture();
		virtual	VOID					SetSampler(SamplerState sampler, TextureFilter filter) = 0;
		inline	int						GetSampler(SamplerState sampler){return m_samplerFilter[sampler];}
		//
		inline	FileName&				GetFileName(){return m_fileName;}
		//
		virtual	BOOL					LoadTextureFromFile(const char* szFile, BOOL bGenMipmap, BOOL bGenAlpha = false, int nFilter = TextureFilter_Linear, BOOL bLoadThread = false, BaseDirectory* basedir = 0);	//从文件读取, int nUseAlphaPixel = false/*是否使用像素级别的点击*/
		//
		BaseDirectory*					m_basedir;
		inline	ImageBuffer&			GetImageBuffer(){return m_imgBuffer;}
		inline	unsigned int			GetWidth(){return m_imgBuffer.width * m_imgBuffer.nDiv;}
		inline	unsigned int			GetHeight(){return m_imgBuffer.height * m_imgBuffer.nDiv;}
		inline	unsigned int			GetOriWidth(){return m_imgBuffer.oriwidth * m_imgBuffer.nDiv;}
		inline	unsigned int			GetOriHeight(){return m_imgBuffer.oriheight * m_imgBuffer.nDiv;}
		//
		virtual	VOID					GenericAlphas();
		virtual	BOOL					IsLoaded() = 0;
		//
		virtual	VOID					Destroy();
		TextureRect*					GetImageMode(const char* szMode);
		CDynamicArray<TextureRect>		m_rects;
		virtual	INT						GetType(){return 0;}
		unsigned int					m_nFilter;
		static unsigned int				s_maxImageBuffer;//最大图片数据大小
		static BOOL						s_bSmallImage;
		static VOID						LoadTextureData(const char* data, int dataLength, ImageBuffer& imgBuffer);
		virtual	VOID					OnLoadOk(ImageBuffer& imgBuffer);
	protected:
		BOOL							LoadTextureData(BOOL bMy=true);
		friend class					GameMgr;
		VOID							LoadRects(const char* memoryPtr, int len);
		StreamBase*						m_filePtr;
		char*							OpenFile(int& size);
		VOID							CloseFile();
		virtual	VOID					OnResetDevice() = 0;
		virtual	VOID					OnLostDevice() = 0;
		virtual	VOID					OnLoadDevice() = 0;
		friend class	Manager;
	protected:
		char							m_imageFormat;
		BOOL							m_bGenMipmap;
		BOOL							m_bGenAlpha;
		ImageBuffer						m_imgBuffer;
		FileName						m_fileName;
		char							m_loadThread;
		//
		enum{
			LoadTexture_FromFile	=	1,
		};
		int								m_nLoadType;
	protected:
		int								m_samplerFilter[SamplerState_Max];
	};

	unsigned char*						ToSmallRGBA(unsigned char* pixels, int width, int height);
//	//
//	class	DynamicMesh
//	{
//	public:
//		DynamicMesh();
//		virtual ~DynamicMesh();
//		virtual	VOID					Create(unsigned int nFVF, unsigned short numVertices, DrawMode mode = DrawMode_Triangles, BOOL bIs2DMode = false/*设置成2D渲染模式*/);
//		//
//		VOID							SetDrawMode(DrawMode mode){m_drawMode = mode;}
//		VOID							SetNumVertices(unsigned short numVertices);
//		VOID							SetNumTriangles(unsigned short numTriangles);
//		int								GetNumTriangles(){return m_triangles.size();}
//		VOID							SetTriangle(unsigned int triangleIndex, const Triangle& t);
//		VOID							WriteIndexBuffer(const unsigned short* indices, unsigned int indiceCount);
//		//
//		Triangle*						GetTriangle(unsigned int triangleIndex);
//		//
//		VOID							SetPos(unsigned int index, const float3& pos);
//		VOID							SetNormal(unsigned int index, const float3& normal);
//		VOID							SetDiffuse(unsigned int index, unsigned int diffuse);
//		VOID							SetSpecular(unsigned int index, unsigned int specular);
//		VOID							SetUV(unsigned int index, const float2& pos);
//		VOID							SetUV2(unsigned int index, const float2& pos);
//		VOID							SetUV3(unsigned int index, const float2& pos);
//		VOID							SetUV4(unsigned int index, const float2& pos);
//		VOID							SetUV5(unsigned int index, const float2& pos);
//		//
//		VOID							Render();	//渲染,使用m_world, m_texturePtr, m_technique
//		VOID							DrawOnly();	//渲染
//		//
//		BOOL							GetPos(unsigned int index, float3& ret);
//		BOOL							GetNormal(unsigned int index, float3& ret);
//		BOOL							GetDiffuse(unsigned int index, unsigned int& ret);
//		BOOL							GetSpecular(unsigned int index, unsigned int& ret);
//		BOOL							GetUV(unsigned int index, float2& ret);
//		BOOL							GetUV2(unsigned int index, float2& ret);
//		BOOL							GetUV3(unsigned int index, float2& ret);
//		BOOL							GetUV4(unsigned int index, float2& ret);
//		BOOL							GetUV5(unsigned int index, float2& ret);
//		//
//		inline	matrix4x4&				GetWorld(){return m_world;}
//		inline	void					SetWorld(const matrix4x4& m){m_world = m;}
//		inline	void					SetPosition(const float3& p){m_world.SetPosition(p);}
//		inline	const float3&			GetPosition(){return m_world.GetPosition();}
//		inline	_shader_var_name&		GetTechnique(){return m_technique;}
//		inline	VOID					SetTechnique(const char* szName){m_technique.SetName(szName);}
//		VOID							Destroy();
//		inline	VOID					SetCullMode(unsigned int c){m_nCullMode = c;}
//		inline	VOID					SetRenderMode(int n){m_nRenderMode = n;}
//		BOOL							PickMesh(const float3& orig, const float3& dir, PIntersectData* pOutPick = 0, BOOL bDoubleSide = FALSE);//点击这个模型判断
//		//
//		VOID							SetTexture(int attrID, Texture* t);
//		int								AddAttribute(Texture* texPtr, int beginIndex = 0, int count = 0);//添加多个贴图组合返回attrID
//		VOID							SetAttribute(int triangleIndex, int attrID);
//		VOID							RebuildAttributes();//重新构造渲染列表
//	protected:
//		int								m_nRenderMode;
//		unsigned int					m_nCullMode;
//		struct MeshAttribute{
//			Texture*				texPtr;
//			int							beginIndex;	//开始渲染位置
//			int							count;		//数量
//		};
//		CDynamicArray<MeshAttribute>	m_attributes;
//		_shader_var_name				m_technique;
//		matrix4x4						m_world;
//		//
//		unsigned int					m_nNumVertices;
//		int								m_nFVF;
//		int								m_nStrideSize;
//		DrawMode						m_drawMode;
//		BOOL							m_bIs2DMode;
//		//
//		unsigned char					m_pos;
//		unsigned char					m_normal;
//		unsigned char					m_diffuse;
//		unsigned char					m_specular;
//		unsigned char					m_uv;
//		unsigned char					m_uv2;
//		unsigned char					m_uv3;
//		unsigned char					m_uv4;
//		unsigned char					m_uv5;
//#ifdef DIRECT_X//directx中无效
//		CDynamicArray<char>				m_vertices;
//		//
//#else
//		CDynamicArray<float3>			m_posV;
//		CDynamicArray<float3>			m_normalV;
//		CDynamicArray<unsigned int>		m_diffuseV;
//		CDynamicArray<unsigned int>		m_specularV;
//		CDynamicArray<float2>			m_uvV;
//		CDynamicArray<float2>			m_uv2V;
//		CDynamicArray<float2>			m_uv3V;
//		CDynamicArray<float2>			m_uv4V;
//		CDynamicArray<float2>			m_uv5V;
//#endif
//		CDynamicArray<Triangle>			m_triangles;
//		CDynamicArray<Triangle>			m_trianglesRenders;
//		CDynamicArray<unsigned char>	m_triangleAttrs;
//	};
	//
	class	DeviceMesh: public Base
	{
	public:
		DeviceMesh();
		ImplementComValue();
	public://
		virtual	void					Destroy();
		//创建三角形索引缓冲区
		virtual	VOID					Create(INT nNumVertices, INT nNumTriangles, VertexElement* elements, int elementCount);//INT nVertexStride, INT nFVF);
		virtual	VOID					Create(INT nNumVertices, INT nNumTriangles, INT nFVF);
		virtual	VOID					SetVertexDecl(VertexElement* elements, int elementCount);
		virtual	VOID					SetVertexFVF(INT nFVF);
		virtual	VOID					WriteVertexBuffer(const void* vertexPtr, int vertexCount, int strideSize);
		virtual	VOID					WriteIndexBuffer(const unsigned short* indexPtr, int indexCount, unsigned char* attrib/*material indexs*/);
		virtual	VOID					UpdateIndexBuffer(const unsigned short* indexPtr, int indexCount) = 0;
		virtual	VOID					UpdateVertexBuffer(const void* vertexPtr, int vertexCount) = 0;
		virtual	BOOL					RenderIndexed(Program* prog, int numTriangles) = 0;	//渲染
		//
		inline	unsigned int			GetNumFaces(){return (m_nNumIndices / 3);}
		int								GetElementWidth();
		//
		template <typename T>
		T*								GetVertex(INT nIndex)
		{
			if(nIndex < 0 || nIndex >= this->m_nNumVertices)
				return 0;
			return (T*)((const char*)m_vertices.GetPtr() + m_nStrideSize * nIndex);
		}
		inline	void*					GetVertexPtr(){return m_vertices.GetPtr();}
		inline	unsigned char*			GetAttributePtr(){return m_attributes.GetPtr();}
		inline	unsigned int			GetVertexBufferSize(){return m_vertices.size();}
		virtual	BOOL					RenderMesh(Program* prog, Texture* tex, const matrix4x4* mat) = 0;	//渲染
		virtual	BOOL					RenderSubset(Program* prog, DWORD dwAttrib = 0) = 0;	//渲染
		//
		virtual VOID					Commit();	//重新刷新设备缓冲区
		//
		inline	INT						GetNumElements(){return m_elements.size();}
		VertexElement*					GetElementPtr(){return m_elements.GetPtr();}
	public:
		virtual	BOOL					IsLoaded() = 0;
		virtual	BOOL					OptimizeMesh(){return false;}
		//
		inline	INT						GetStrideSize(){return m_nStrideSize;}
		inline	INT						GetNumIndices(){return m_nNumIndices;}
		inline	INT						GetNumVertices(){return m_nNumVertices;}
		virtual	unsigned short*			GetIndices(){return m_indices.GetPtr();}
	protected:
		virtual	VOID					OnResetDevice() = 0;
		virtual	VOID					OnLostDevice() = 0;
		friend class	Manager;
	protected:
		virtual	~DeviceMesh();
		CDynamicArray<char,int,0>			m_vertices;	//顶点列表
		CDynamicArray<unsigned short,int,0>	m_indices;	//Indices
		CDynamicArray<unsigned char,int,0>	m_attributes;
		CDynamicArray<VertexElement,int,0>	m_elements;
		//BOOL							m_bHasTBNormal;
		int								m_nStrideSize;	//顶点宽度
		//int								m_nFVF;
		int								m_nNumIndices;
		int								m_nNumVertices;
	};
	//

	class	RenderText: public Base
	{
	public:
		ImplementComValue();
		RenderText();
		virtual	~RenderText();
		int								GetFontSize(){return m_nFontSize;}
		int								SetFontStyle(){return m_nFontStyle;}
		//
		virtual	VOID					SetFontSize(int nSize);//
		VOID							SetFontStyle(int nStyle);
		virtual	VOID					SetText(const char* text);//文本内容有改变,设置字体大小和样式之后，必须要重新更新文本才会生效
        inline  BOOL                    IsEmpty(){return (m_text.size()==0);}
        virtual BOOL                    IsInit(){return false;}
		virtual	VOID					SetFontName(const char* name);
		//
		inline	int						GetWidth(){return m_imgWidth;}
		inline	int						GetHeight(){return m_imgHeight;}
		inline	int						GetRealWidth(){return m_realWidth;}
		inline	int						GetRealHeight(){return m_realHeight;}
		Rect							GetTextureRect(){return Rect(0,0,m_imgWidth,m_imgHeight);}
		//
		inline	Size&					GetClipSize(){return m_clip;}
		VOID							SetClipSize(const Size& rc){m_clip = rc;}
		virtual	BOOL					IsFigFont(){return false;}
	protected:
		virtual	VOID					OnResetDevice() = 0;
		virtual	VOID					OnLostDevice() = 0;
		friend class	Manager;
	protected:
		NameT<16>						m_name;
		short							m_nTextAlignment;
		unsigned short					m_imgWidth,m_imgHeight;
		short							m_nFontSize;
		short							m_nFontStyle;//0=normal,1=bold,2=italic,3=bolditalic
		unsigned short					m_realWidth,m_realHeight;
		Size							m_clip;//限制范围
        CDynamicArrayS<char>            m_text;
	};

#define	MAX_TEXT_MTRL	5
	class	RenderTextFig: public RenderText
	{
	public:
		RenderTextFig();
		~RenderTextFig();
		virtual	VOID					SetText(const char* text);//文本内容有改变,设置字体大小和样式之后，必须要重新更新文本才会生效
		VOID							SetTextSize(unsigned short index, float fSize);
		VOID							SetTextColor(unsigned short index, unsigned int color);
		VOID							SetTextAlpha(unsigned short index, float fAlpha);
		VOID							SetTextVisible(unsigned short index, BOOL bVisible);//
		virtual	VOID					SetFontSize(int nSize);//
		//
		virtual	BOOL					IsFigFont(){return true;}
		virtual BOOL                    IsInit(){return true;}
		BOOL							DrawTexts(const Rect& screen, unsigned int dtFormat, const color4& c, const matrix4x4* world, int nHighlight);
		BOOL							DrawSprite2D(BOOL bEnabled, const color4& c, const matrix4x4* world, int nHighlight, BOOL bDlgMatrix);
		//SetText,SetFontSize等设置完毕之后设置这个
		VOID							SetInsideSize(const Size& size, const Pixel& center/*中心点基于0点*/, unsigned int dtFormat);//文本有变化了
	public:
		struct RTFigRect{
			char						fontEn;//如果不是en就是cn
			unsigned int				color;
			float						size;//按照最新字体缩放
			unsigned char				w,h;//宽高
			//unsigned short				x,y;//图片位置
			rect_f						uv;
			char						visible;//是否显示
			unsigned char				channel;//通道颜色,如果是0表示没有通道
			unsigned char				imageIndex;
			char						offy;
			//float3						vDrawPos[4];//最终的计算结果要渲染的数据，最终在屏幕坐标
		};
		float							m_sizeBase;//按照最新字体缩放基础
		CDynamicArrayS<RTFigRect>		m_figs;
	protected:
		unsigned char					m_numMtrl;
		unsigned char					m_mtrls[MAX_TEXT_MTRL];//文本的mtrl
		AutoPtr<DeviceMesh>				m_meshs;//模型数据指针
		Size							m_size;
		Pixel							m_center;
		unsigned int					m_dtFormat;
		char							m_bNeedUpdate;
		virtual	VOID					Update();
		BOOL							DrawText2(const color4& c, const matrix4x4* world, int nHighlight, const float4& uvScale);
		//
		BOOL							ResetFontFig(RenderTextFig::RTFigRect& fig, short_t ch);
		virtual	VOID					OnResetDevice(){}
		virtual	VOID					OnLostDevice(){}
	};
	class	Program//方法
	{
	public:
		Program(int mode);
		virtual ~Program();
		virtual	NameT<32>&							GetName(){return m_name;}
		virtual	BOOL								OpenShaderText(const char* szText, int len);
		virtual	BOOL								SetMatrix(_shader_var_name& var, const matrix4x4* mat) = 0;//
		virtual	VOID								SetTexture(TextureIndex index, Texture* tex) = 0;
		virtual	VOID								SetTextTexture(TextureIndex index, RenderText* tex) = 0;//渲染文本的时候被调用
		virtual	BOOL								FindVariant(_shader_var& out, const char* szVariantName) = 0;//查找uniform shader元素
		//
		virtual	BOOL								SetMatrix(_shader_var& var, const matrix4x4* mat, const char* szName) = 0;//
		//
		virtual	BOOL								SetInt(_shader_var_name& var, int i) = 0;
		virtual	BOOL								SetInt(_shader_var& var, int f, const char* szName) = 0;
		virtual	BOOL								SetFloat(_shader_var& var, float f, const char* szName) = 0;
		virtual	BOOL								SetFloat2(_shader_var& var, const float2& v, const char* szName) = 0;
		virtual	BOOL								SetFloat3(_shader_var& var, const float3& v, const char* szName) = 0;
		virtual	BOOL								SetFloat4(_shader_var& var, const float4& v, const char* szName) = 0;
		//
		virtual	BOOL								SetFloatPtr(_shader_var& var, const float* f, int nCount, const char* szName) = 0;
		virtual	BOOL								SetFloat2Ptr(_shader_var& var, const float2* v, int nCount, const char* szName) = 0;
#ifndef ANDROID_OS//安卓上使用这个会崩溃
		virtual	BOOL								SetFloat3Ptr(_shader_var& var, const float3* v, int nCount, const char* szName) = 0;
#endif
		virtual	BOOL								SetFloat4Ptr(_shader_var& var, const float4* v, int nCount, const char* szName) = 0;
		virtual	BOOL								SetMatrixPtr(_shader_var& var, const matrix4x4* v, int nCount, const char* szName) = 0;
		inline	BOOL								IsCreateFail(){return m_bFail;}
		inline	BOOL								IsCreated(){return m_bCreated;}
		virtual	BOOL								Active() = 0;
		//
		void										SetLight(unsigned char lightCount, float4* posTypelist, float4* colorlist, float4* range_angles);//设置灯光属性
	public:
		void										SetDiffuse(const color4& color);	//设置漫射光
		void										SetCameraValue(const float3& _eye, const float3& _target, const float3& _dir);	//设置镜头
		void										SetAmbientColor(const color4& color);	//设置环境光
		void										SetEmissiveColor(const color4& color);	//
		void										SetSpecular(const color4& color,float fPower);	//设置高光
		void										SetElapsedTime(float fElapsedTime);		//设置间隔时间
		void										SetTime(float fTime);					//设置时间
		//
		void										SetSkyColor(const color4& color);
		void										SetLightMap(Texture* t);
		void										SetSpecularMap(Texture* t);
		void										SetReflectMap(Texture* t);
		void										SetNormalMap(Texture* t);
		void										SetScreenMap(Texture* t);	//设置屏幕渲染图片拷贝
		//
		void										SetWorldMatrix(const matrix4x4* mat, BOOL bHasNormalMap = false);	//设置世界矩阵
		void										SetBillboardMatrix(const matrix4x4* mat);	//设置世界矩阵
		void										SetWindParameter(const PWindParameter& wind);			//设置风向
		void										SetBoneMatrixs(const matrix4x4* bones, unsigned int numofBone);	//设置骨骼列表
		//
		VOID										ForceCommitMatrixToShaders();
		VOID										CommitMatrixToShaders();
		VOID										CommitShadowMap();
		VOID										CommitToShadowMap();
		//
		void										SetLightDirInv(BOOL bInv);
		inline	INT									GetRenderMode(){return m_nRenderMode;}
		//
		enum ProgramType{
			Type_Material		=	0x00000001,
			Type_Normal			=	0x00000002,
			Type_Color			=	0x00000004,
			Type_UV				=	0x00000008,
			Type_StaticShadowMap=	0x00000010,
			Type_Contrast		=	0x00000040,
			Type_Fog			=	0x00000080,
			Type_Light			=	0x00000100,
			Type_AlphaTest		=	0x00000200,
			Type_Fog2D			=	0x00000400,
			Type_Fog2DGray		=	0x00000800,//2D+灰度*雾色而不是全颜色
			Type_Fog3DGray		=	0x00001000,//3D+灰度*雾色而不是全颜色
			Type_Fog3DSub		=	0x00002000,
			Type_Fog2DSub		=	0x00004000,
			Type_Brightness		=	0x00008000,//亮度
			Type_ToShadowMap	=	0x00010000,//渲染到阴影贴图
			Type_WithShadowMap	=	0x00020000,//渲染并使用阴影贴图
			Type_ShadowUsePCF3	=	0x00040000,
			Type_ShadowUsePCF5	=	0x00080000,
			//Type_NormalMap		=	0x00000020,
			//Type_Light2			=	0x00000200,
			//Type_Cartoon		=	0x00000400,
			//Type_Specular		=	0x00000800,
			//Type_UVAnimation	=	0x00001000,
		};
		inline	INT									GetProgramType(){return m_nProgramType;}
		VOID										SetProgramType(INT/*ProgramType*/ nType, BOOL bBlendMode);
		VOID										SetFileName(const char* szFile, BOOL bBlendMode);
		NameT<32>									m_name;
		CDynamicArrayS<char, 0>						m_fileName;
	public:
		_shader_var									m_varTex[TextureIndex_Max];			//world view projection
		_shader_var									m_varMVP;
		_shader_var									m_h_world,m_h_worldInverse,m_h_viewProjection,m_h_boneList;
		_shader_var									m_h_cameraEye,m_h_cameraTarget,m_h_cameraDir;
		_shader_var									m_h_time;
		_shader_var									m_h_lightDirectionView,m_h_lightDirectionViewEnable,m_h_lightDirectionViewColor;
		_shader_var									m_h_lightCount,m_h_lightPos,m_h_lightColor,m_h_lightRangeAngle;
		_shader_var									m_h_elapsedTime;
		_shader_var									m_h_waterHeight;
		_shader_var									m_h_billboardMat;
		_shader_var									m_h_skyColor;
		_shader_var									m_h_quadScreenSize;
		_shader_var									m_h_materialDiffuse,m_h_materialAmbient,m_h_materialEmissive,m_h_materialSpecular,m_h_materialPower;
		_shader_var									m_h_worldViewTranspose;
		_shader_var									m_h_brightnass;
		_shader_var									m_h_constrast;
		_shader_var									m_h_alphaConstrast;
		_shader_var									m_h_shadowDensity,m_h_shadowTextureSize,m_h_ViewToLightProj,m_h_m_shadowEpsilon,m_h_shadowMap;
		_shader_var									m_h_fogColor,m_h_fogRange,m_h_fogPos;
		//
		inline	BOOL								IsProgram(int t){return (this->m_nProgramType&t)>0;}
	protected:
		char										m_bBlendMode;
		_shader_var									m_h_windDirection;
		_shader_var									m_hScreenMatrix;
		int											m_nMVP;
		//
	protected://
		friend class	Manager;
		virtual	VOID								OnResetDevice();	//根据渲染模式创建shader
		virtual	VOID								OnLostDevice() = 0;
		virtual	VOID								OnCreateShader() = 0;
	protected:
		CDynamicArray<char>							m_vsps_directx9;
		CDynamicArray<char>							m_vsps_opengles_v;
		CDynamicArray<char>							m_vsps_opengles_p;
		//
		unsigned int								m_shadowMapIdx;
		unsigned int								m_toShadowMapIdx;
		unsigned int								m_renderIdx;
		unsigned int								m_cameraViewIdx;
		unsigned short								m_resetID;
		short										m_nRenderMode;
		char										m_bCreated;
		char										m_bFail;
		int											m_nProgramType;//如果是0则都是文件
		unsigned int								m_lastEmissive;
		const matrix4x4*							m_lastBoneMat;
		const matrix4x4*							m_lastWorldMat;
		unsigned int								m_lastDiffuse;
		unsigned char								m_lastLightCount;
		char										m_bLightDirInv,m_bLightSeted;
	};
	//
	class	RenderTarget: public Texture
	{
	public:
		RenderTarget();
		virtual ~RenderTarget();
		enum RTFormat{
			RTFormat_ShadowMap	= 0,//用于阴影渲染
		};
		virtual	BOOL					Create(int width, int height, RTFormat format = RTFormat_ShadowMap);//创建一个渲染目标
		virtual	VOID					Destroy();
		virtual	BOOL					Begin() = 0;//开始渲染
		virtual	VOID					End() = 0;//结束渲染
		inline	BOOL					IsInit(){return m_bInit;}
		virtual	VOID					GenericAlphas(){}
		virtual	BOOL					IsLoaded(){return m_bInit;}
		virtual	VOID					OnLoadDevice(){}
	protected:
		virtual	VOID					SetSampler(SamplerState sampler, TextureFilter filter){}
		//
		virtual	INT						GetType(){return 1;}
		friend class	Manager;
		char							m_bInit;
		char							m_rtFormat;
	};
	struct	DrawVInfos
	{
		const float3*		posList;
		const float3*		normalList;
		const unsigned int* colorList;
		const float2*		uvList;
		int		m_numVertices;
		DrawVInfos(int numVertices, const float3* positions, const unsigned int* colors = 0, const float2* uvs = 0, const float3* normals = 0)
		{
			normalList	=	normals;
			m_numVertices	=	numVertices;
			posList		=	positions;
			colorList	=	colors;
			uvList		=	uvs;
		}
	};
	class	Device
	{
	public:
		Device();
		virtual ~Device(){}
		virtual	classT<Texture, classMgr<Texture> >*			CreateTexture() = 0;
		virtual	classT<DeviceMesh, classMgr<DeviceMesh> >*				CreateMesh() = 0;
		virtual	Program*				CreateProgram(int mode) = 0;
		virtual	RenderTarget*			CreateShadowMap() = 0;
	public:
		virtual	VOID					SetCullMode(CullMode mode) = 0;
		virtual	VOID					SetBlendMode(BlendMode src, BlendMode dest) = 0;
		virtual	VOID					SetViewport(int x, int y, int w, int h) = 0;
		virtual	VOID					ResetAllDefault();
		virtual	VOID					SetEnableZ(BOOL b) = 0;
		inline	BOOL					GetEnableZ(){return m_bEnableZ;}
		virtual	VOID					SetZWriteEnable(BOOL b) = 0;
		inline	BOOL					GetZWriteEnable(){return m_bZWriteEnable;}
		//
		virtual	VOID					AppCreate(){}
		virtual	VOID					SetAlphaMode(AlphaMode mode) = 0;
		virtual	VOID					Clear(UINT flags = CLEAR_TARGET | CLEAR_ZBUFFER) = 0;
		virtual	VOID					ClearError() = 0;
		virtual	VOID					SetSampler(SamplerState sampler, TextureFilter filter) = 0;
        virtual VOID					SetUIClipRect(const Rect* rect) = 0;//设置UI裁剪,如果是0则不做任何裁剪
		//
		inline	VOID					SetClearDepth(float f){m_fDepth = f;}
		inline	float					GetClearDepth(){return m_fDepth;}
		inline	VOID					SetClearColor(const color4& c){m_clearColor = c;}
		inline	const color4&			GetClearColor(){return m_clearColor;}
		inline	AlphaMode				GetAlphaMode(){return m_nAlphaMode;}
		inline	CullMode				GetCullMode(){return m_nCullMode;}
		inline	BlendMode				GetSrcBlend(){return m_nSrcBlend;}
		inline	BlendMode				GetDestBlend(){return m_nDestBlend;}
		//
		inline	int						GetRenderTriangles(){return m_numTriangles;}
		inline	void					ResetRenderTriangles(){m_numTriangles = 0;}
		inline	void					AddRenderTriangle(int num){m_numTriangles += num;}
		virtual	VOID					DrawIndexed(Program* prog, const DrawVInfos& info, const unsigned short* tList, int numIndices, DrawMode mode) = 0;
		virtual	VOID					DrawNoIndexed(Program* prog, const DrawVInfos& info, int numVertices, DrawMode mode) = 0;
		//
		virtual	VOID					OnLostDevice() = 0;
		virtual	VOID					OnResetDevice() = 0;
		//
		inline	VOID					SetCullVisible(CullMode mode){if(this->m_nCullMode == mode || this->m_nCullMode == CullMode_Double)return;this->SetCullMode(mode);}
		inline	BOOL					GetCullVisible(CullMode mode){return (this->m_nCullMode == mode || this->m_nCullMode == CullMode_Double);}
		//
		float							GetLODMinVisible(){return m_fLODMinVisible;}
		float							GetLODViewDelta(){return m_fLODViewDelta;}
		PWindParameter					m_windParameter;
		float							m_fAlphaContract;//透明值对比度
	protected:
		virtual	RenderText*				CreateRenderText() = 0;
	protected:
		friend class					Manager;
		unsigned int					m_nFVF;
		VOID							InitShaderModes();
		//
		BOOL							m_bZWriteEnable;
		float							m_fLODMinVisible;
		float							m_fLODViewDelta;
		int								m_numTriangles;
		float							m_fDepth;
		color4							m_clearColor;
		AlphaMode						m_nAlphaMode;
		BlendMode						m_nSrcBlend;
		BlendMode						m_nDestBlend;
		CullMode						m_nCullMode;
		BOOL							m_bEnableZ;
		int								m_samplerFilter[SamplerState_Max];
	};
	//
	typedef	classPtr<Texture>			TexturePtr;
	typedef	classPtr<DeviceMesh>		MeshPtr;
	//
	VOID								DisableColor(color4& c);
	int									FillElemenetByFVF(VertexElement* ele, INT nFVF);
	VOID								AddElement(VertexElement* ele, int& eleIndex, int type, int usage, int usageIndex);
	Device*								GetDevicePtr();	//唯一的设备
	BOOL								IsSizeOK(int s);
	BOOL								LoadMesh(StreamBase* stream, MeshPtr& mesh);
	VOID								SaveMesh(StreamBase* stream, DeviceMesh* pMesh);
};

#endif
