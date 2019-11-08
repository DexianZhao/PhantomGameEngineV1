//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   Manager.h
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 

Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___PHANTOMMANAGER__H___
#define ___PHANTOMMANAGER__H___
#include "PhantomBase.h"
#include "PhantomDevice.h"
#include "PhantomCamera.h"
#include "PhantomFont.h"
#include "PhantomUIManager.h"
#include "PhantomMusic2D.h"
#include "PhantomMesh.h"
#include "PhantomUnit3D.h"
#include "PhantomScene3D.h"
#include "PhantomSocket.h"
#include "PhantomShadowMap.h"

#define	MAX_RENDER_OPTIMIZE			5000
#define	MAX_RENDER_SORTLIST			1000

namespace Phantom{
#pragma pack(push,1)
	struct	TIDMessage{
		enum TID{
			TID_LoadTexture	=	1,
			TID_LoadMesh	=	2,
			TID_LoadSceneRegion	=	3,//载入场景区块
		};
		TIDMessage();
		~TIDMessage();
		unsigned short				mid;//message id
		int							objectid;//
		ImageBuffer					buffer;
		unsigned int				length;
		unsigned char*				ptr;//分配更多的数据
	};
#pragma pack(pop)
	class HttpRequest;
	class ActionBase;
	class MyMap2D;
	//sdk base
	class	SDKBase: public Base
	{
	public:
		SDKBase(){}
		virtual	~SDKBase(){}
		ImplementComValue();
		//
		virtual	VOID			OnCreate(){}
		virtual	VOID			OnDestroy(){}
		virtual	BOOL			OnFrameMove(double dElapsedTime){return true;}
		virtual	VOID			OnResize(int w, int h){}
		virtual	VOID			OnRender(double dElapsedTime){}
		virtual	VOID			OnRenderUI(){}
		virtual	VOID			OnInputEvent(Phantom::InputEventID evt, int x, int y, unsigned int touchIndex){}
		virtual	VOID			OnUIEvent(EventID id, UIDialog* dlg, UIControl* c, INT nParam){}
		virtual	VOID			On3DEvent(EventID id, UIDialog* dlg, UIControl* c, INT nParam){}
		//
		virtual	BOOL			GotoBack(){return false;}//是否可以返回
	public:
	};

	typedef	classPtr<Font>			FontPtr;
	typedef	classPtr<MeshData>		MeshDataPtr;
	typedef	classPtr<Scene3D>		Scene3DPtr;

	//幻影管理器
	class	Manager: public ThreadProc
	{
	public:
		Manager();
		~Manager();
		ImplementComValue();
	public://engine
		inline	BOOL				IsDeviceSetup(){return m_bDeviceSetup;}
		inline	BOOL				IsInitialized(){return m_bIsInitialized;}
		virtual	BOOL				AppCreate(INT nOSType);	//创建使用幻影引擎系统
		virtual	BOOL				AppFrameMove();			//
		virtual	VOID				AppResize(int width, int height, int deviceWidth, int deviceHeight);
		virtual	BOOL				AppRender();
		virtual	VOID				AppDestroy();	//删除幻影使用
		virtual	VOID				AppInputEvent(Phantom::InputEventID btn, int x, int y, unsigned int touchIndex);
		//
		VOID						SetActiveSDK(SDKBase* base);
		VOID						SetNextActiveSDK(SDKBase* base){m_sdkNextPtr = base;}
		SDKBase*					GetActiveSDK(){return m_sdkPtr;}
		//
		int							GetRenderTriangles(){return m_numRenderTriangles;}
		VOID						ExitGame(){m_bExitGame = true;}
		inline	BOOL				IsExitGame(){return m_bExitGame;}
		//
		inline	Device*				operator ->(){return m_device;}
		inline	Device*				GetDevicePtr(){return m_device;}
	public:
		inline	double				GetFPS(){return m_dFPS;}
		inline	double				GetAppTime(){return m_dAppTime;}
		inline	double				GetElapsedTime(){return this->m_dElapsedTime;}
		//
	public://creates
		BOOL						LoadScene3D(Scene3DPtr& ret, const char* szFile);
		BOOL						CreateScene3D(Scene3DPtr& ret, float fGridSize/*格子大小1=0.1米*/, int nSceneSize/*场景大小2,4,8,16,32*/);//创建一个新的场景
		BOOL						LoadTexture(TexturePtr& ret, const char* szFile, BOOL bGenMipmap = false, int nFilter = TextureFilter_Linear, BOOL bLoadThread = false, BaseDirectory* basedir = 0);
		VOID						CreateMesh(MeshPtr& r);
		BOOL						LoadFont(FontPtr& ret, const char* szFile);	//fonts directory, like "fonts/abc.fig"
		//
		BOOL						LoadTexture(Texture** ret, const char* szFile, BOOL bGenMipmap = false, int nFilter = TextureFilter_Linear, BOOL bLoadThread = false, BaseDirectory* basedir = 0);
		//
		BOOL						LoadMesh(AutoPtr<Mesh>& ret, const char* szFile, ImportResult* result = 0);	//".rpgmesh"
		BOOL						LoadMesh(MeshDataPtr& ret, const char* szFile, ImportResult* result = 0);	//".rpgmesh"
		//render modes
		Program*					SetRenderMode(int shader, BOOL bBlendMode = false);
        Program*					GetRenderMode(int shader, BOOL bBlendMode = false);
		Program*					SetRenderMode(_shader_var& tech, BOOL bBlendMode, const char* szName);
		inline	VOID				SetTexture(TexturePtr& tex, TextureIndex index = TextureIndex_0) { SetTexture(tex.GetPtr(), index); }
		VOID						SetTexture(Texture* tex, TextureIndex index = TextureIndex_0);//
		//
		VOID						SetViewport(const rect_f* rc);//
	public://矩阵信息
		virtual	VOID				SetWorldMatrix(const matrix4x4* mat);
		virtual	VOID				SetViewMatrix(const matrix4x4& mat){m_camera->SetViewMatrix(mat);}
		virtual	VOID				SetProjMatrix(const matrix4x4& mat){m_camera->SetProjectionMatrix(mat);}
		inline	const matrix4x4&	GetWorldMatrix(){return m_world;}
		inline	const matrix4x4&	GetViewMatrix(){return m_camera->GetViewMatrix();}
		inline	const matrix4x4&	GetProjMatrix(){return m_camera->GetProjMatrix();}
		//
		VOID						SetWorldMatrix2D(const matrix4x4* mat);
		VOID						SetWorldDlgMatrix2D(const matrix4x4* mat = 0);	//设置对话框的矩阵
		//
		BOOL						Get2DFrom3D_2DProj(Pixel& _out,const float3& vPos) const;
		void						Get3DFrom2D_2DProj(float3& _orig,float3& _dir,const Pixel& _in) const;
		float2						ConvertPixelToTixel(const Pixel& pixel);
		Pixel						ConvertTixelToPixel(const float2& pos);
		float2						GetPixelToTixel(const Pixel& pixel);	//0~width之间缩放比例
		Pixel						GetTixelToPixel(const float2& pos);
		Callback*					m_callback;	//回调函数列表
	public://渲染
		VOID						RenderMeshChanged(){m_bRenderMeshChanged = true;}	//设置渲染模式被改变
		//
		VOID						DrawTriangles(const DrawVInfos& info, int numVertices, const Triangle* tList, int numTriangles, const matrix4x4* world = 0);
		VOID						DrawPoints(const DrawVInfos& info, int numVertices, const matrix4x4* world = 0);
		//
		VOID						DrawTriangle(const float3& t1, const float3& t2, const float3& t3, const color4* c = 0, const matrix4x4* world = 0);
		VOID						DrawTriangle(const PointColor& t1, const PointColor& t2, const PointColor& t3, const matrix4x4* world = 0);
		//
		BOOL						Draw3D(Texture* tex, const float3* pos, const float2* uvs, unsigned int* colors, const matrix4x4* world = 0, CullMode cull = CullMode_CW);
		BOOL						Draw3D(Texture* tex, const Rect& xyRect, const rect_f& texRc, const color4* c = 0, float* heights = 0, const matrix4x4* world = 0, CullMode cull = CullMode_CW);
		BOOL						Draw3D(Texture* tex, const Rect& xyRect, const float2* uvs, const color4* c = 0, float* heights = 0, const matrix4x4* world = 0, CullMode cull = CullMode_CW);
		//
		VOID						DrawLine(const float3& l1, const float3& l2, const color4* c = 0, const matrix4x4* world = 0);
		VOID						DrawLine(const PointColor& l1, const PointColor& l2, const matrix4x4* world = 0);
		//
		VOID						DrawBox(const AABox& box, const unsigned int* colors/*8个*/, const matrix4x4* world = 0, BOOL bFill = TRUE);
		VOID						DrawBox(const AABox& box, unsigned int color, BOOL bFill);
		VOID						DrawBoxTexture(const AABox& box, const unsigned int* colors/*24个*/, const float2* uvs/*24个*/, const matrix4x4* world = 0);
		//
		inline	int					GetBufferWidth(){return m_bufferWidth;}
		inline	int					GetBufferHeight(){return m_bufferHeight;}
		inline	int					GetUIBufferWidth(){return (int)((float)m_bufferWidth / this->m_uiScale.x);}
		inline	int					GetUIBufferHeight(){return (int)((float)m_bufferHeight / this->m_uiScale.y);}
	public://2D draws, x, y, (z must be 0), lookat z
		VOID						Set2DProjectionFov(float f2DFov);	//设置2D的FOV值
		BOOL						Draw2D(Texture* t, const rect_f* rects, int rectCount, const Rect& texRc, const color4* c = 0, const matrix4x4* world = 0, BOOL bEnabled = true, CullMode cull = CullMode_CW);	//渲染2D图片
		BOOL						Draw2D(Texture* t, const float2* points, int rectCount, const Rect& texRc, const color4* c = 0, const matrix4x4* world = 0, BOOL bEnabled = true, CullMode cull = CullMode_CW);	//渲染2D图片
		BOOL						Draw2D(Texture* t, const Pixel* points, const float2* uvs, unsigned int * colors, int count, BOOL bHighlight = false, const matrix4x4* world = 0, BOOL bEnabled = true, CullMode cull = CullMode_CW);	//渲染2D图片
		//
		BOOL						Draw2D(Texture* t, const rect_f& screen, const rect_f& texRc, const color4* c = 0, const matrix4x4* world = 0, BOOL bEnabled = true, CullMode cull = CullMode_CW);	//渲染2D图片
		BOOL						Draw2D(Texture* t, const rect_f& screen, const Rect& texRc, const color4* c = 0, const matrix4x4* world = 0, BOOL bEnabled = true, CullMode cull = CullMode_CW);	//渲染2D图片
		BOOL						Draw2D(Texture* t, const Rect& screen, const rect_f& texRc, const color4* c = 0, const matrix4x4* world = 0, BOOL bTo2DConv = TRUE, BOOL bEnabled = true, CullMode cull = CullMode_CW);
		BOOL						Draw2D(Texture* t, const Rect& screen, const Rect& texRc, const color4* c = 0, const matrix4x4* world = 0, BOOL bTo2DConv = TRUE, BOOL bEnabled = true, CullMode cull = CullMode_CW);
		//
		BOOL						DrawSprite2D(Texture* t, const Rect& textureRc, const float3* center, const color4* c = 0, const matrix4x4* world = 0, const rect_f* scale = 0, int nHighlight = 0, BOOL bEnabled = true, BOOL bInvW = false, BOOL bInvH = false, BOOL dlgMatrix=false);	//渲染2D图片
		BOOL						DrawSprite2D(const Rect& textureRc, const float3* center, const color4* c = 0, const matrix4x4* world = 0, const rect_f* scale = 0, int nHighlight = 0, BOOL bEnabled = true, BOOL bInvW = false, BOOL bInvH = false, BOOL dlgMatrix=false);	//渲染2D图片
		VOID						DrawRect2D(const Rect& rc, const color4& c, BOOL bFill/*是否填充*/, BOOL bLingXing=false/*渲染菱形*/);	//渲染2D线条
		VOID						DrawLines2D(const Pixel* xys, int count, const color4& c, BOOL bFill/*是否填充*/);	//渲染2D线条
		VOID						DrawLines2DColor(const PixelColor* xys, int count, BOOL bFill/*是否填充*/);	//渲染2D线条
		VOID						DrawCircle( const Pixel& center, float fRadius, const color4& c, int smooth, BOOL bFill);//渲染一个圆
		VOID						DrawGrids3D(const float3& eye, unsigned int uiGridColor,float fGridWidthHeight,int numW, int numH);
		VOID						DrawTriangleList2D(Texture* tex, DrawVInfos& d, int numVertices, BOOL bFill);
		VOID						SetupScreenPosList(float3* inOut, int numVertices);//初始化2D屏幕坐标到渲染数据
		VOID						SetupScreenPos(float3& inOut);
		VOID						DrawTriangle2D(Texture* texture, const screenVertex3D& t1, const screenVertex3D& t2, const screenVertex3D& t3, BOOL bFill);
		BOOL						DrawRectPoints2D(Texture* texture, const float2& leftTop, const float2& rightTop, const float2& leftBottom, const float2& rightBottom
			, const rect_f* newUV = 0, const color4* c = 0, const matrix4x4* world = 0);
		//
		BOOL						DrawTexts(const char* szUTF8, Rect& destRect, int nLength = 0, float fontSize = 16.0f, float fItalic = 0, const color4* c = 0, unsigned int dtFormat = DrawTextAlign_LEFT | DrawTextAlign_TOP, const matrix4x4* world = 0);	//
		BOOL						DrawTexts(RenderText* textPtr, BOOL bEnabled, Rect& destRect, unsigned int dtFormat, const color4& color, const matrix4x4* world = 0, int nHighlight = 0);
		BOOL						DrawSprite2D(RenderText* t, const Rect& textureRc, const float3* center, const color4* c, const matrix4x4* world, const rect_f* scale, int nHighlight, BOOL bEnabled, BOOL dlgMatrix=false);	//渲染2D图片
		//
		BOOL						DrawSprite2D(BOOL bEnabled, const color4& c, const matrix4x4* world, int nHighlight, BOOL bDlgMatrix);//渲染颜色
		BOOL						DrawSprite2D(Texture* tex, const Rect& textureRc, BOOL bEnabled, const color4& c, const matrix4x4* world, int nHighlight, BOOL bDlgMatrix);//渲染图片
		BOOL						DrawSprite2D(RenderText* t, const Rect& textureRc, BOOL bEnabled, const color4& c, const matrix4x4* world, int nHighlight, BOOL bDlgMatrix);//渲染2D图片
		//
		BOOL						SetDefaultENFont(const char* szFigFile);		//从fig文件创建
		BOOL						SetDefaultUnicodeFont(const char* szFigFile);	//支持国际语言的其他字体型
		inline	Font*				GetDefaultENFont(){return m_defaultENFont;}
		inline	Font*				GetDefaultUnicodeFont(){return m_defaultUnicodeFont;}
		inline	float				Get2DProjectionFov(){return m_f2DFov;}
		//
		inline	Program*			GetCurrProgram(){return this->m_currProg;}
		//
		inline	VOID				SetIsUtf8(BOOL b){m_bIsUtf8 = b;}
		inline	BOOL				GetIsUtf8(){return m_bIsUtf8;}
		inline	VOID				SetSleepTime(float fSecond){m_fSleedTime = fSecond;}
		inline	float				GetSleepTime(){return m_fSleedTime;}
		//
		VOID						FramePause(BOOL bPause);//暂停整个游戏
		VOID						SetFrameSpeed(float f);//设置整个游戏的播放速度,1是正常,不能小于0
		//
		VOID						SetCursorPos(const Pixel& c, unsigned int index = 0){if(index >= MAX_TOUCH_COUNT)index = MAX_TOUCH_COUNT - 1; m_cursorPos[index] = c;}
		const Pixel&				GetCursorPos(unsigned int index = 0){if(index >= MAX_TOUCH_COUNT)index = MAX_TOUCH_COUNT - 1; return m_cursorPos[index];}
		//
		VOID						GetCursorPos2D(Pixel& ret, unsigned int index = 0);
		//
		VOID						AddPrintText(const char* s){m_printTexts.push_back(s);}
		MultiMusic*					GetMusicFilePtr(const char* szFile);
		VOID						DeleteMusicFileIndex(const char* szFile);	//删除音乐文件和所有内存使用
		int							GetMusicFileIndex(const char* szFile);
		MultiMusic*					GetMusicFilePtrByIndex(int nIndex);
		VOID						PlayMusic(const char* szFile);
        inline  BOOL                IsWavEnabled(){return m_bWavEnabled;}//all wav file can play
        inline  BOOL                IsMp3Enabled(){return m_bMp3Enabled;}//all mp3 file can play
        inline  VOID                SetMp3Enabled(BOOL b){m_bMp3Enabled = b;}
        inline  VOID                SetWavEnabled(BOOL b){m_bWavEnabled = b;}
		//
		//
		VOID						StopAllMusic();
		//
		inline	double				GetDoubleClickTime(){return m_dDoubleClickTime;}
		inline	VOID				SetDoubleClickTime(double d){m_dDoubleClickTime = d;}
		VOID						OnLostDevice();
		VOID						OnResetDevice();
		VOID						OnPause();
		VOID						OnResume();
		//
		INT							CreateProgram(int/*ProgramType*/ type, BOOL bBlend);//如果不存在则自动创建shader方法
		INT							CreateProgramFromFile(const char* szFile, BOOL bHasBlend = false);
		VOID						CreateProgramFromMode(int mode, const char* szFile, BOOL bHasBlend = false );
		//
		VOID						SetMute(BOOL b);
		inline	BOOL				GetMute(){return m_bMute;}
		inline	INT					GetUIDialogSizeW(){return m_uiDialogSize.cx;}
		inline	INT					GetUIDialogSizeH(){return m_uiDialogSize.cy;}
		VOID						SetUIDialogSize(BOOL bScale, int width, int height, Pixel* ret, int touchIndex = 0);
		VOID						SetGameOverflow(BOOL bBlackOnOverflow);//如果益处则填充黑色
		BOOL						m_bGameOverflow;
		//
		VOID						RemoveText(RenderText* text);//计数器减1
		RenderText*					InsertText(const char* szText, int nFontSize, int nFontStyle, int nTextAlignment, const char* szFontName = 0);//添加一个文本，如果已经存在则返回存在的，节约内存和图片数量
	public://action
		VOID						AddAction(ActionBase* a);//添加一个新动作，此处内部自动删除指针，所以如果想自己保留一份数据必须要safe_addref一次否则会出错
		VOID						RemoveAction(ActionBase* a);
	public://base scales
		inline	VOID				SetUIOffset(float x, float y){m_uiOffset.setxy(x, y);m_uiOffsetSave = m_uiOffset;}	//界面缩放大小
		inline	const float2&		GetUIOffset(){return m_uiOffset;}
		VOID						SetUIScale(float x, float y);
		inline	const float2&		GetUIScale(){return m_uiScale;}
		inline	int					GetTextureDiv(){return m_nTextureBaseDiv;}	//图片载入时缩小大小，所有图片，针对不同平台图片大小不必要太大，比如可以设置一个1024x1024的图片，如果在手机上跑则可以缩小到512x512，值设置成m_nTextureBaseDiv = 2
		inline	void				SetTextureDiv(int nDiv){m_nTextureBaseDiv = getmax(nDiv, 1);}
		inline	BOOL				GetTextureDivAsFile(){return m_bTextureDivAsFile;}
		inline	void				SetTextureDivAsFile(BOOL b){m_bTextureDivAsFile = b;}//是否在文件中就已经压缩
		//
	public://VR 3d
		inline	BOOL				IsVR3DViewer()const{return m_bVR3DViewer;}
		inline	BOOL				IsVR3DSingleEye() const {return m_bVR3DSingleEye;}
		VOID						SetVR3DViewer(BOOL b);
		VOID						SetVR3DViewerSingle(BOOL b);
		VOID						RenderAll3DObjects();
	public://particles
		ParticleData*				AllocParticle();					//分配粒子
		VOID						FreeParticle(ParticleData* p);	//释放粒子
		INT							GetFreeParticleCount();								//返回空闲的粒子数量
		VOID						SetupParticles(INT nMaxParticles);	//设置最大粒子数量
	protected:
		//inline	PR_Base*			GetRenderPR(int n){return m_renderPR[n];}
		//PR_Base*					m_renderPR[MAX_PARTICLE_SHOW_TYPE];
		friend class				CElementEmissionRuntime;
		ParticleData*				m_allParticles;		//所有系统中分配的粒子
		INT							m_particleCount;	//以分配的粒子数量
		INT							m_particleAssignIndex;	//分配到第几个
		ParticleData*				m_topParticle;		//可分配的粒子
	public://render lists
		void						Push3DDrawMgr(UISortBase* mgr, const float3& pos);//渲染3D
		void						RenderAll3DDrawMgr();
		void						PushAlphaObject(RenderBase* objPtr, BOOL bZWriteEnable = true);
		void						ClearAlphaObjects();
		//
		void						SetAlphaDistance(float fDist = To3DValue(0.25f)){m_fRenderAlphaDistance = getmax(fDist, 0.1f);}
		//
		inline	INT					GetMeshCount(){return this->m_meshs.size();}
		inline	Mesh*				GetMeshPtr(INT index){if(index < 0 || index >= m_meshs.size())return 0; return m_meshs[index];}
		//
		VOID						ReleaseAllTextureState();
		VOID						SetUIScaleMode(BOOL b, Pixel* ret = 0);
		BOOL						IsClipRender(const Rect& clientRc);
		//
		sensordata					m_sensordata;
		struct DrawTextItem{
			NameT<256>	text;
			int			id;
			int			x,y;
			int			size;
			color4		c;
		};
		int							m_textid;
		CDynamicArray< DrawTextItem > m_textItems;
		INT							AddText(const char* text, int x, int y);
		INT							AddText(const char* text, int x, int y, const color4& c, int size);
		VOID						SetText(int nID, const char* text);
		VOID						RemoveText(int nID);
        enum{
            Orientation_Portrait = 1,
            Orientation_PortraitUpsideDown = 2,
            Orientation_LandscapeLeft = 3,
            Orientation_LandscapeRight = 4,
        };
        int                         m_nOrientation;
		VOID						SetGameSize(int w, int h){m_gameWidth = w; m_gameHeight = h;}
		int							m_gameWidth,m_gameHeight;
        int                         GetDeviceWidth();
        int                         GetDeviceHeight();
		//
		Mesh*						SearchMesh(GameID& id);
	protected:
		//
		void						RenderAllAlphas();
		//RenderNode*				m_renderNormalize[MAX_RENDER_OPTIMIZE];
		//RenderNode*				m_renderNoZBuffer[MAX_RENDER_OPTIMIZE];
		//int							m_renderNormalizeCount;
		RenderBase*					m_renderAlpha[MAX_RENDER_SORTLIST];
		int							m_renderAlphaCount;
		RenderBase*					m_renderNoZBuffer[MAX_RENDER_SORTLIST];
		int							m_renderNoZBufferCount;
		UISortBase*					m_renderDrawMgr[MAX_RENDER_SORTLIST];
		int							m_renderDrawMgrCount;
		//
		float						m_fRenderAlphaDistance;
		PhantomLock					m_lockRender;
		VOID						RecalcRenderRect();
		Rect						m_renderRect;
	protected://my app implemenets
		virtual	BOOL				OnCreate(){return true;}
		virtual	VOID				OnDestroy(){}
		virtual	BOOL				OnFrameMove(double dElapsedTime){return true;}
		virtual	VOID				OnRender(double dElapsedTime){}
		VOID						LoadAllResources();	//加载所有资源
	public://public parameters
        inline  INT                 GetOSType(){return m_nOSType;}
		BOOL						m_bShowFPS;
		Camera*						m_camera;
		UIManager*					m_uiManager;
		BOOL						m_bRenderUI;	//是否渲染界面
		std::vector<std::string>	m_printTexts;	//文本列表
        int                         m_deviceWidth,m_deviceHeight;
		BOOL						m_bInIDE;
		VOID						SetFog(BOOL bEnable, unsigned int color, float fnear, float ffar, FogMode type);
		inline	int					GetBaseProgramType(){return m_programType;}
		inline	BOOL				IsFogEnable(){return m_fogEnable;}
		inline	float				GetFogNear(){return m_fogNear;}
		inline	float				GetFogFar(){return m_fogFar;}
		const float3&				GetFogPos();
		inline	int					GetFogMode(){return m_fogMode;}
		VOID						SetBrightness(BOOL bEnable, float fBrightness);
		VOID						SetContrast(BOOL bEnable, float fContrast);
		VOID						CreateShadowMap(int textureSize = 1024);//是否开启动态阴影
		//
		VOID						AddGravity(float x, float y, float z);
		int							m_nGravitySmooth;
		CDynamicArray<float3>		m_vGravitys;
	public:
		float3						m_lightPos;
		float3						m_lightDir;
		float3						m_lightColor;
		char						m_lightEnable;
		float3						m_ambientColor;
		ShadowMap					m_shadowMap;
		inline	BOOL				IsShadowEnable(int s){return (m_shadowEnabled&s);}
		inline	BOOL				IsShadowEnable(){return (m_shadowEnabled&ShadowEnable_UseShadowMap);}
		char						m_shadowMapViewMode;
		float						m_shadowViewSize;//阴影贴图长度修正
		int							m_shadowEnabled;//ShadowEnable enum
		char						m_shadowPCF;
		float						m_shadowDensity;
		float						m_shadowEpsilon;
		char						m_bUseThread;
	protected:
		VOID						ResetProgramType();
		char						m_fogEnable;
		float						m_fogNear,m_fogFar;
		float3						m_fogColor;
		int							m_fogMode;
		int							m_programType;
		char						m_bBrightness,m_bContrast;
		float						m_fBrightness,m_fContrast;
		VOID						RenderAllToShadowMap();//渲染所有阴影参数到阴影贴图
	protected://enables
		BOOL						m_bPause;
		float						m_fFrameSpeed;
		Device*						m_device;
        BOOL                        m_nOSType;
		BOOL						m_bVR3DViewer;
		BOOL						m_bVR3DSingleEye;
		//
		matrix4x4					m_world;
		int							m_nSetMVP;
		Program*					m_pSetMVP;
		AutoPtr<SDKBase>			m_sdkPtr;	//当前运行的状态
		AutoPtr<SDKBase>			m_sdkNextPtr;
		CDynamicArray<Texture*>		m_loadTextures;	//预载入的图片
		//2D matrix
		double						m_f2DFov;
		double						m_fTan2DFov;
		double						m_dLastDownTime;
		double						m_dDoubleClickTime;//双击时间间隔
		BOOL						m_bExitGame;
		matrix4x4					m_view2D, m_projection2D, m_view2DInv, m_viewProjection2D, m_matrixDef2D;
	public:
		inline	const matrix4x4&	GetProjection2D(){return m_projection2D;}
		inline	const matrix4x4&	GetView2D(){return m_view2D;}
		inline	BOOL				IsCursorDown(int index){assert(index>=0&&index<MAX_TOUCH_COUNT);return m_cursorDown[index];}
		inline	INT					GetTextureCount(){return this->m_textures.size();}
		inline	Texture*			GetTexturePtr(int index){if(index<0||index>=m_textures.size())return 0;return m_textures[index];}
		unsigned int				m_renderIdx;
		inline	float				GetRealElapsedTime(){return m_dRealElapsedTime;}
	protected://systems
		double						m_dFPS;	//fps
		double						m_dLastFPS;//
		double						m_dAppTime;	//时间,从程序开始运行算起
		double						m_dElapsedTime;	//时间差
		double						m_dRealElapsedTime;
		int							m_nFrameMoveCount;
		BOOL						m_bIsInitialized;
		int							m_bufferWidth;
		int							m_bufferHeight;
        BOOL                        m_bMp3Enabled;
        BOOL                        m_bWavEnabled;
		//
		float						m_fSleedTime;
		BOOL						m_bIsUtf8;
		BOOL						m_cursorDown[MAX_TOUCH_COUNT];
		Pixel						m_cursorPos[MAX_TOUCH_COUNT];
	protected://shader modes
		Program*					m_progMode[RenderMode_Max];
		Program*					m_progModeB[RenderMode_Max];
		Program*					m_currProg;
		int							m_shaderMode;
		BOOL						m_bShaderModeBlend;
		//
		BOOL						m_bRenderMeshChanged;
		BOOL						m_bIsPause;
		BOOL						m_bDeviceSetup;
		//
		Size						m_uiDialogSize;
		AutoPtr<Font>				m_defaultENFont;
		AutoPtr<Font>				m_defaultUnicodeFont;	//中文字体
		int							m_numRenderTriangles;
		//
		int							m_nTextureBaseDiv;
		BOOL						m_bTextureDivAsFile;
		float2						m_uiScale;	//整体界面缩放
		float2						m_uiScaleSave;
		float2						m_uiOffset,m_uiOffsetSave;
		BOOL						m_bMute;
	protected://textures
		classMgr<Texture>			m_textures;
		classMgr<DeviceMesh>		m_deviceMesh;//meshs
		classMgr<Font>				m_fonts;//fonts
		classMgr<MeshData>			m_meshGroupDatas;
		classMgr<Scene3D>			m_scenes;//scene3D
		CDynamicArray<Mesh*>		m_meshs;
		CDynamicArray<Music2D*>		m_backMusics;
		CDynamicArray<HttpRequest*>	m_https;
		CDynamicArray<MyMap2D*>		m_map2ds;
		CDynamicArray<Scene3D*>		m_scene3ds;
		CDynamicArray<RenderTarget*>	m_RTs;
		friend class				Scene3D;
		unsigned short				m_meshid;
		//
		VOID						PushRT(RenderTarget* m);
		VOID						PopRT(RenderTarget* m);
		VOID						PushMap3D(Scene3D* m);
		VOID						PopMap3D(Scene3D* m);
		VOID						PushMap(MyMap2D* m);
		VOID						PopMap(MyMap2D* m);
		VOID						PushMesh(Mesh* m);
		VOID						PopMesh(Mesh* m);
		VOID						PushMusic(Music2D* m);
		VOID						PopMusic(Music2D* m);
		VOID						PushHttp(HttpRequest* hr);
		VOID						PopHttp(HttpRequest* hr);
		VOID						PushText(RenderText* hr);
		VOID						PopText(RenderText* hr);
		friend class				HttpRequest;
		friend class				Font;
		friend class				Mesh;
		friend class				Music2D;
		friend class				ActionBase;
		friend class				MyMap2D;
		friend class				RenderTarget;
		friend class				RenderText;
		friend class				Program;
		friend class				RenderTextFig;
        RenderText*                 m_tempText;
		RenderText*                 m_authText;
		AutoPtr<DeviceMesh>			m_textureMesh;
		BOOL						SetTextMode(BOOL bEnabled, const float4& textureRc, const color4& c, const matrix4x4* world, BOOL bDlgMatrix);
	protected://musics
		CDynamicArray<MultiMusic*>	m_musics;
		//
		CDynamicArray<float3>		m_tempPosList;
		CDynamicArray<unsigned int>	m_tempColorList;
		CDynamicArray<RenderText*>	m_texts;
		struct RenderTextPool{
			RenderText*				text;
			int						count;
		};
		CDynamicArray<RenderTextPool>	m_textPool;
		//
		VOID						OnRemoveAction(ActionBase* a);
		CDynamicArray<ActionBase*>	m_actions;
		//
	public:
		void						send(TIDMessage::TID messageID, const char* ptr, int length, int id);
	protected:
		PhantomLock					m_lockMessage;
		TIDMessage*					alloc();//分配数据块
		void						recycle(TIDMessage* msg);//回收数据块
		CDynamicArrayS<TIDMessage*,100>	m_frees;//空闲消息数据
		CDynamicArrayS<TIDMessage*,100>	m_allocs;//空闲消息数据
		CDynamicArrayS<TIDMessage*,100>	m_recvs;//接收的消息数据
		CDynamicArrayS<TIDMessage*,100>	m_tempRecvs;//接收的消息数据,当前处理
		CDynamicArrayS<TIDMessage*,100>	m_trecvs;//接收的消息数据,当前处理
		CDynamicArrayS<TIDMessage*,100>	m_tempTRecvs;//接收的消息数据,当前处理
		void						onMessage(TIDMessage* msg);//处理游戏消息接收
		virtual	VOID				OnThread();
	};

	extern	Manager	g_manager;
	inline	Camera*	GetCameraPtr(){return g_manager.m_camera;}
	extern	BOOL			LoadTexture(TexturePtr& r, char* szTextureFile, const char* szTexturePath, BOOL bGenMipmap, BaseDirectory* basedir = 0);
	extern	VOID			SetEncryptData(unsigned char* crypt, int len);
	extern "C"{
		const char*		call_lua(const char* type, const char* params, const char* params2);//调用lua内部的函数oncall
	};
};



#endif
