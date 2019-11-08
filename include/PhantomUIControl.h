//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UICONTROL_H__
#define	__PHANTOM_UICONTROL_H__

#include "PhantomUIBase.h"
#include "PhantomFloatAnimation.h"
#include "PhantomMusic2D.h"
#include "PhantomDevice.h"

#define	DISABLE_COLOR		color4(0.5f, 0.5f, 0.5f, 1.0f)
namespace Phantom{

	class	UIGroup;
	class	UIDialog;
	class	UIManager;
	class	UIControl;
	class	UIScrollBar;
	class	TextFormat;
	class	Mesh;

	static const int	MaxElementCount	=	6;

	struct NextAction{
		char						mode;
		NameT<32>					name;
		int							type;
	};

	class	UIDrawMgr;
	struct UILuaEvent
	{
		int							id;
		Name						func;
	};
	class UIAnimationTrack;
	struct	AnimationTrackPlay{
		float							m_fSleepTimeCur;
		float							m_fWaveSleepTimeCur;
		float							m_fFrame;
		PlayAnimationType				m_bAnimationAutoPlay;
		char							m_bPlaying;
		float							m_fSpeed;
		char							m_bPlayingUV;
		UIControl*						m_controlPtr;
		short							m_nLoopCount;
		char							m_bWavePlayed;
		char							m_bIsPlayOver;
		VOID							Init(UIAnimationTrack& ut);
		AnimationTrackPlay();
	};
	//struct	ControlRuntime{
	//	Rect							rect;
	//	unsigned int					color;
	//	float							fFrame;
	//	CDynamicArrayS<short,0>			textIndexs;	//文本索引值
	//};
	class UIAnimationTrack: public Base
	{
	public:
		ImpComValue2();
		UIAnimationTrack();
		virtual ~UIAnimationTrack();
		const char*						GetName();
		VOID							SetName(const char* szName);
		//
		FloatAnimation*					GetAnimationPtr(ControlTrackType t);
		VOID							SetStatus(TrackStatus s);	//设置当前状态
		VOID							SetCurFrame(AnimationTrackPlay&data, float fTime);	//设置当前关键帧
		float							GetCurFrame(AnimationTrackPlay&data);				//返回当前关键帧
		VOID							CloneFrom(UIAnimationTrack* c);
		//
		VOID							Play(AnimationTrackPlay& data);
		VOID							SetTrackKeyCount(INT nFrame);
		INT								GetFrameCount(){return m_nFrameCount;}
		VOID							SetPlaySpeed(float f);
		float							GetPlaySpeed();
		float							GetAnimationFloat(AnimationTrackPlay&data, ControlTrackType t);
		float							GetAnimationFloat(ControlTrackType t, float fTime);
		//
		VOID							SetKeyDefaultValue(ControlTrackType t, float f);
		//
		VOID							Load(CSafeFileHelperR& r, UIControl* c);
		VOID							Save(CSafeFileHelperW& w);
		VOID							Stop(AnimationTrackPlay&data);
		//
		VOID							CopyFrom(UIAnimationTrack* e){ CloneFrom(static_cast<UIAnimationTrack*>(e)); }
		INT								GetID(){return m_nID;}
		//
		//VOID							SetPlayType(PlayAnimationType type){m_bAnimationAutoPlay = type;}	//动画自动播放模式
		//PlayAnimationType				GetPlayType(){return m_bAnimationAutoPlay;}
		VOID							SetLoopCount(INT nLoop = -1);
		INT								GetLoopCount();
		//
		const char*						GetWaveFile(){return m_soundFile;}	//绑定一个wav文件
		void							SetWaveFile(const char* szFile);
		float							GetWaveVolume(){return m_soundVolume;}
		VOID							SetWaveVolume(float f);
		VOID							SetWaveSpeed(float f);
		float							GetWaveSpeed(){return m_fSoundSpeed;}
		int								GetElementIndex(){return m_nElementIndex;}		//元素索引
		VOID							SetElementIndex(int n){m_nElementIndex = n;}
		BOOL							GetSleepAtHide(){return m_bHideOnSleep;}	//是否UV混合
		VOID							SetSleepAtHide(BOOL b){m_bHideOnSleep = b;}
		//
		float							GetWaveSleepTime(){return m_fWaveSleepTime;}
		VOID							SetWaveSleepTime(float f){m_fWaveSleepTime = f;}
		//
		float							GetSleepTime(){return m_fSleepTime;}
		VOID							SetSleepTime(float f){m_fSleepTime = f;}
		//
		VOID							SetNextActionType(ActionType nID){m_nNextAnimationID = nID;}
		ActionType						GetNextActionType(){return m_nNextAnimationID;}	//返回下一个动作ID，如果没有就不会再连接播放
		//
		BOOL							IsWaitTimeEnable(){return m_bWaitTimeEnable;}
		VOID							SetWaitTimeEnable(BOOL b){m_bWaitTimeEnable = b;}
		//
		//UV动画绑定
		VOID							SetUVAnimation(ActionType nID){m_nUVAnimationID = nID;}
		ActionType						GetUVAnimation(){return m_nUVAnimationID;}
		//
		VOID							SetActionType(ActionType type){m_nActionType = type;}
		ActionType						GetActionType(){return m_nActionType;}
		ActionType						m_nActionType;
		//int								m_soundFileIndex;
		//SoundPlayID						m_soundPlay;
		Music2D							m_music;
		Name							m_soundFile;
		float							m_soundVolume;
		float							m_fSoundSpeed;
		float							m_fWaveSleepTime;
		BOOL							m_bWaitTimeEnable;
		float							m_fSleepTime;
		//
		BOOL							OnFrameMove(AnimationTrackPlay& t, float fElapsedTime, float fBaseSpeed);//返回false表示正在休眠状态
		//
		VOID							SetMaxParticle(INT nCount){m_nMaxParticle = getmax(nCount, 0);}
		INT								GetMaxParticle(){return m_nMaxParticle;}
	public:
		int								m_nMaxParticle;
		ActionType						m_nUVAnimationID;
		ActionType						m_nNextAnimationID;
		NameT<32>						m_name;
		//char							m_szName[128];
		char							m_bIsDefault;
		TrackStatus						m_status;
		int								m_nFrameCount;
		INT								m_nID;
		//char							m_bPlayingUV;
		float							m_fSpeed;
		//
		int								m_nElementIndex;		//元素索引
		BOOL							m_bHideOnSleep;	//是否UV混合
		INT								m_nSaveLoopCount;
		char							m_bHideOnOver;//播放完畢之後是否隱藏
		//
		FloatAnimation					m_floats[ControlTrackType_Count];
	};

	class	UIRenderCallback
	{
	public:
		virtual VOID	OnRenderControl(UIControl* c){}
	};

	template <typename T>
	class	UIRenderCallbackT: public UIRenderCallback
	{
	public:
		UIRenderCallbackT(){m_t = 0;}
		virtual VOID	OnRenderControl(UIControl* c){if(m_t)m_t->OnRenderControl(c);}
		T*	m_t;
	};
	typedef BaseT<UIControl> PhantomUIControlBase;
	class UIControl: public ControlData
	{
	public:
		UIControl( BOOL bInit, UIDialog *dialogPtr );
		virtual ~UIControl();
		PhantomUIControlBase			m_this;
		virtual void 					SetEnabled( char bEnabled ) { m_bIsEnabled = bEnabled; }
		virtual char 					GetEnabled() { return IsEnabledCtrl(); }
		virtual void 					SetVisible( char bVisible ) { m_bVisible = bVisible; }
		virtual char 					GetVisible() { return m_bVisible; }
		virtual	unsigned int 			GetType() const { return m_controlType; }
		int  							GetID() const { return m_controlID; }
		virtual	VOID					RebuildRenderRect();
		virtual	UIControl*				Clone(float fTime, UIDialog* pDestDialog = 0);
		inline	BOOL	operator	==	(const char* szName){return (this->m_name == szName);}
		//
		UIControl*						m_from;
		float							m_fromTime;
		//
		virtual	BOOL					IsEnabledCtrl();
		virtual	VOID					CopyFrom(UIControl* e);
		//
		virtual	BOOL					LoadAllResource();	//载入所有需要的资源
		//
		const char*						GetName(){return m_name.t;}
		virtual	void					SetName(const char* szName);
		//
		void							SetRenderCallback(UIRenderCallback* c){m_renderCB = c;}
		UIRenderCallback*				m_renderCB;
		virtual char 					onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex);
		virtual char 					canHaveFocus() { return false; }
		virtual void 					onFocusIn() { m_bHasFocus = true; }
		virtual void 					onFocusOut() { m_bHasFocus = false; }
		virtual void 					onMouseEnter();
		virtual void 					onMouseLeave();
		virtual BOOL 					isPtIn( Pixel pt, int element = 0 );
		virtual	char					GetChecked(){return false;}
		virtual	void					SetChecked(char b){}
		virtual	VOID					SetHideOnEditor(BOOL b){m_bHideOnEditor = b;}	//编辑模式下隐藏所有元素之类
		virtual	BOOL					GetHideOnEditor(){return m_bHideOnEditor;}
		virtual	BOOL					IsPickPixel(){return m_bIsPickPixel;}
		virtual	VOID					SetPickPixel(BOOL b){m_bIsPickPixel = b;}
		virtual	VOID					SetProgressValue(float f){}
		virtual	float					GetProgressValue(){return 0.0f;}
		virtual	void 					SetID( int ID );
		virtual void					SetTextColor( unsigned int Color );
		virtual	BOOL					IsControlVisible();
		virtual	int						GetElementCount();
		VOID							SetPos(const Pixel& pos);
		Pixel							GetPos();
		VOID							SetTextureMode(const char* szName, DrawTextAlign align = DrawTextAlign_LEFT);//是否按照高度重新设置缩放
		virtual	UIElement*				GetElementPtr(int index);
		virtual	VOID					PlayAction(ActionType type);
		virtual	VOID					PlayAction(const char* szName);
		Name							m_szPlayAction;
		virtual	ActionType				GetPlayAction(){return (ActionType)m_nPlayAction;}
		virtual	VOID					SetNextPlayAction(ActionType type);
		virtual	ActionType				GetNextPlayAction(){return (ActionType)m_nNextPlayAction;}
		virtual	VOID					OnPlayStart(){}
		virtual	char					GetParentMove(){return m_bParentMove;}
		virtual	void					SetParentMove(char b);
		virtual	BOOL					IsBackground(){return this->m_bIsBackground;}		//是否背景图片控件
		virtual	void					SetBackground(BOOL bb){m_bIsBackground = bb;}		//是否自定义渲染
		virtual	void					SetReadOnly(char b){m_bReadOnly = b;}	//只读
		virtual	char					GetReadOnly(){return m_bReadOnly;}		//只读
		virtual	INT						GetGroupID(){return m_groupID.id;}
		virtual	VOID					SetGroupID(INT nGroupID);
		virtual	const Rect&				GetRenderRect(){return (Rect&)this->m_windowRect;}
		virtual	Rect					GetWindowRect();
		virtual	Rect					GetSavedWindowRect();
		virtual	VOID					SetWindowRect(const Rect& rc);
		virtual	void					SetRenderRect(const Rect& r);
		virtual	const char* 			GetText(){return m_text.str();}
		virtual	void 					SetText( const char* strText );
		virtual	INT						GetIntVal(){return atoi(m_text.str());}
		virtual	double					GetDoubleVal(){return atof(m_text.str());}
		virtual	void 					SetIntVal( int nVal );
		virtual	void 					SetDoubleVal( double dVal );

		virtual	UIScrollBar*			GetScrollBar(){return 0;}
		virtual	const color4&			GetTextColor(){ return (color4&)m_textColor;}
		virtual	void					SetTextColor(const color4& c);
		virtual	const color4&			GetTextureColor(){ return (color4&)m_textureColor;}
		virtual	void					SetTextureColor(const color4& c);
		//2013-1-3
		void							RenderImgText(UIElement& element);
		//
		virtual	char					LoadControl(CSafeFileHelperR& r);
		virtual	char					SaveControl(CSafeFileHelperW& w);
		virtual	void					SetTextFormat(unsigned int format){m_textFormat = format;}
		virtual	unsigned int			GetTextFormat(){return m_textFormat;}
		virtual	int						ShowTextShadow(){return m_nShadowWidth;}
		virtual	void					ShowTextShadow(int width){m_nShadowWidth = width;}
		virtual	unsigned int			GetShadowColor(){return m_shadowColor;}
		virtual	void					SetShadowColor(unsigned int c){m_shadowColor = c;}
		virtual	void					SetLayer(int level);
		virtual	int						GetLayer(){return m_nLevel;}
		virtual	void					SetValueI(int value, char b){}
		virtual	void 					SetValue( int nValue ){SetValueI( nValue, false );}
		virtual	int  					GetValue() const{return m_value;}
		virtual	void 					GetRange( int &nMin, int &nMax ){nMin = m_minValue; nMax = m_maxValue;}
		virtual	void 					SetRange( int nMin, int nMax ){m_minValue = nMin; m_maxValue = nMax; SetValueI( m_value, false );}
		//virtual	const char*				GetToolTipTextA(){return m_toolTipText.strA();}
		//virtual	const char*				GetToolTipText(){return m_toolTipText.strA();}
		virtual	void					SetCanFocus(char b){m_bCanFocus = b;}
		virtual	char					GetCanFocus(){return this->m_bCanFocus;}
		virtual	char					IsCustomRender(){return m_bCustomRender;}		//是否自定义渲染
		virtual	void					SetCustomRender(char b){m_bCustomRender = b;}		//是否自定义渲染
		virtual	char					IsFocus();
		virtual	const char*				GetSoundPlayID(UISoundPlay soundPlayType){if(soundPlayType < 0 || soundPlayType >= UISoundPlay_count)return 0; return m_soundFile.t;}//.strFile.c_str();}
		virtual	void					SetSoundPlayID(UISoundPlay soundPlayType, const char* szFile);
		//
		virtual	UIAnimationTrack*		CloneTrack(UIAnimationTrack* c);
		//
		virtual	void					SetRotation(char bEnable, float rot);
		virtual	char					IsRotation(){return m_bEnableRotation;}
		virtual	float					GetRotationAngle(){return m_rotation;}
		VOID							SetScale(float fx, float fy, const Pixel* center = 0);
		float2							m_scale,m_scalesave;
		//
		virtual	void					SetLoopMode(char b){m_bLoopMode = b;}
		virtual	char					GetLoopMode(){return m_bLoopMode;}	//动画是否循环
		//
		virtual	VOID					FrameMove(float fElapsedTime, const Rect* newRect, BOOL bChanged);
		UIDialog*						GetDialogPtr();

		virtual	VOID					OnLoadControl(CSafeFileHelperR& r, int version){}
		virtual	VOID					OnSaveControl(CSafeFileHelperW& w){}
		virtual void					OnButtonClick(const Pixel& pt, int nType);
		virtual	BOOL					IsPassword(){return false;}
		//
		BOOL							IsTrackAutoBegin();	//是否跟着对话框显示而自动播放
		VOID							SetTrackAutoBegin(BOOL b);
		VOID							PlayTrack();//播放
		VOID							StopTrack();
		BOOL							IsPlayingTrack();
		VOID							PauseTrack(BOOL b);
		BOOL							IsPauseTrack();
		INT								GetTrackPlayLoopCount();	//返回循环播放次数
		VOID							SetTrackPlayLoopCount(INT nLoop = -1);//-1表示无限次重复
		VOID							SetTrackPlaySpeed(float f);	//设置轨迹播放速度
		float							GetTrackPlaySpeed();
		//
		inline	INT						GetHeight(){return m_windowRect.bottom - m_windowRect.top;}
		inline	INT						GetWidth(){return m_windowRect.right - m_windowRect.left;}
		//
		UIAnimationTrack*				AddTrack(const char* szName);
		VOID							RemoveTrack(UIAnimationTrack* ani);
		VOID							ActiveTrackByName(const char* szName);
		VOID							ActiveTrackByID(INT nID);
		UIAnimationTrack*				GetTrackPtr(INT index);
		UIAnimationTrack*				GetTrackPtrByName(const char* szName);
		INT								GetTrackCount();
		VOID							SetTrackDefault(UIAnimationTrack* def);
		UIAnimationTrack*				GetTrackDefault();	//返回默认的动画指针
		UIAnimationTrack*				GetActiveTrack();
		VOID							SetTrackDefaultID(int id);
		int								GetTrackDefaultID();	//返回默认的动画指针
		//
		virtual	VOID					SetParentControl(UIControl* c);
		virtual	UIControl*				GetParentControl();	//父物体设置
		virtual	VOID					SetParentControlID(INT id);
		virtual	INT						GetParentControlID();	//父物体设置
		//
		VOID							SetTabIndex(INT index){m_nTabIndex = index;}
		INT								GetTabIndex(){return m_nTabIndex;}
		//
		virtual	void					Render( float fElapsedTime ){}
		virtual	VOID					SetButtonType(UIButtonType type){m_buttonType = type;}
		virtual	UIButtonType			GetButtonType(){return (UIButtonType)m_buttonType;}
		virtual	VOID					DrawFocusFrame(BOOL b){m_bDrawButtonFrame = b;}
		virtual	BOOL					IsDrawFocusFrame(){return m_bDrawButtonFrame;}
		const matrix4x4&				GetControlMat();
		virtual	TextFormat*				GetTextFormatPtr(){return 0;}
		VOID							SetTrackLoop(INT nLoop){m_nTrackLoopCount = nLoop;}
		INT								GetTrackLoop(){return m_nTrackLoopCount;}
		VOID							SetPlayLoop(INT nLoop){m_nPlayLoopCount = nLoop;}
		INT								GetPlayLoop(){return m_nPlayLoopCount;}
		VOID							SetPlaySpeed(float fSpeed){m_fPlaySpeed = fSpeed;}
		float							GetPlaySpeed(){return m_fPlaySpeed;}
	public:
		virtual	VOID					SetProgressType(INT b){m_nProgressType = b;}	//是否圆形进度框,一般在技能图标上面覆盖一层用来显示技能冷却时间
		virtual	INT						GetProgressType(){return m_nProgressType;}
		virtual	const Pixel&			GetRotCenter(){return m_rotCenter;}	//转动中心点
		virtual	VOID					SetRotCenter(const Pixel& p){m_rotCenter = p;}
		virtual	float					GetSavedRotation(){return this->m_saveRotation;}
		virtual	Pixel					GetSavedRotCenter(){return this->m_saveRotCenter;}
		//
		//
		virtual	VOID					OnEraseAnimation(UIAnimationTrack* c){}
		virtual	VOID					OnEditClose(BOOL bIsCancel, const char* text){}
		//
		VOID							SetEventFunc(INT nEventID, const char* szFunc);
		CDynamicArrayS<UILuaEvent,0>			m_events;//
		ControlAnimation				m_currentAnim;
		Music2D							m_soundPlay;//[UISoundPlay_count];
		UIDialog*						m_dialogPtr;
		UIDialog*						m_pChildDialog;
		VOID							SetChildDialog(UIDialog* d);
		inline	VOID					ClearElement(){m_elementCount = 0;}
		inline	VOID					AddElement(UIElement* e){m_elementList[m_elementCount++] = e; if(e)e->SetControlPtr(this);}
		//会自动计算矩阵
		virtual	Pixel					ScreenToClient(const Pixel& pixel);
		virtual	Pixel					ClientToScreen(const Pixel& pixel);
		//
		virtual	VOID					OnSetAnimationRect(){}
		inline	BOOL					IsEnabled(){return this->m_bIsEnabled;}
		virtual	VOID					OnAnimationRect(){}
		virtual	VOID					OnTrackActiveChange(BOOL b);
		//设置用户自定义ID
		char							GetVisibleOnScript(){return m_bVisibleOnScript;}		//在脚本内调用
		void							SetVisibleOnScript(char b){m_bVisibleOnScript = b;}		//是否自定义渲染
		//
		float							GetSleepTime(){return m_fSleepTime;}
		VOID							SetSleepTime(float f){m_fSleepTime = f;}
		//
		VOID							SetAutoSort(BOOL b){m_bAutoSort = b;}
		BOOL							GetAutoSort(){return m_bAutoSort;}
		Pixel							GetCenterPos();
		VOID							SetCenterPos(const Pixel& s);
		virtual	BOOL					IsClipIn();	//是否包含进去了
		UIGroup*						GetGroupPtr();
		VOID							SetGroupPtr(UIGroup* g);
		//
		enum AngleMode{
			AngleMode_None	=	0,
			AngleMode_8		=	1,//8个方向模式
			AngleMode_LRTB	=	2,//上下左右模式
			AngleMode_XLRTB	=	3,//左上，左下，右上，右下模式
		};
		VOID							SetUnitAngle(float fAngle);	//设置面对方向
		float							GetUnitAngle();
		VOID							SetUnitAngleDir(const float2& fAngle);	//设置面对方向
		BOOL							GetEnableUnitAngle(){return m_bEnableUnitAngle;}
		VOID							SetEnableUnitAngle(BOOL b){m_bEnableUnitAngle = b;}
		int     						GetScrollBarWidth() const { return m_scrollBarWidth; }
		void    						SetScrollBarWidth( int nWidth ) { m_scrollBarWidth = nWidth; recalcRects(); }
		//
		virtual	VOID					LoadSnd(INT nIndex);
		virtual	BOOL					IsUseTrack(){return true;}
		//2013-1-3
		VOID							EnableUseImageText(BOOL b){m_bEnableUseImageText = b;}//是否使用图片文本进行渲染？
		BOOL							IsEnableUseImageText(){return m_bEnableUseImageText;}
		CDynamicArrayS<int,0>				m_textIndexs;	//文本索引值
		virtual	VOID					AddDrawMgr(UIDrawMgr* m);
		//
		int								m_nAlignMode;
		VOID							RenderDrawMgrs();
		VOID							RenderScroll();
		VOID							SetScrollBarV(UIScrollBar* v);
		VOID							SetScrollBarH(UIScrollBar* v);
		UIScrollBar*					m_scrollPtrV,*m_scrollPtrH;
		VOID							OnScroll(UIScrollBar*);
		VOID							RecalcScrollValue();
		//
		Pixel							GetLoadPos();
	protected:
		virtual void					recalcRects();
		friend class					UIDialog;
		friend class					UIAnimationTrack;
		friend class					UIControlRT;
		char							m_bIsCustomControl;
	public://
		float							m_fMax3DDist;
		float3							m_bind3DPos;
		GameID							m_bind3DObj;
		char							m_bind3DEnable;
		char							m_bind3DVisible;
		VOID							Bind3DPos(const float3& pos);
		VOID							Bind3DObj(const GameID& id, float fHeightAdd);
		VOID							Bind3DClear();
		//
		VOID							SetClipRect(BOOL b);
		VOID							SetInnerMesh(Mesh* mesh);//会自动clone,如果是0则清除
		VOID							RenderMesh();
	public://
		AutoPtr<Mesh>					m_innerMesh;//内部的模型渲染文件
		VOID							OnChangeText();
		char							m_numberSizeAdd;
		RenderText*						m_textPtr;
		UIAnimationTrack*				m_trackPtr;
		matrix4x4						m_controlMat;
		//matrix4x4						m_renderMat;
		VOID							OnChangeMatrix();
		//
		Rect							m_rectDefault;
		Pixel							m_parentOffset;
		CDynamicArrayS<UIAnimationTrack*,0>	m_trackList;
		CDynamicArrayS<UIDrawMgr*,0>			m_drawMgrs;
		UIElement*						m_elementList[MaxElementCount];
		Rect							m_loadRect;
		Pixel							m_loadCenter;
		float							m_loadRotation;
		char							m_elementCount;
		Text							m_text;
		int								m_controlType;
		void *							m_pUserData;
		int								m_nNextPlayAction;//ActionType
		char							m_bSetDefault;
		char							m_bIsChangedRot;
		char							m_bIsGroupCenter;//作为组的中心控件使用
		AnimationTrackPlay				m_playdata;
		char							m_bUseImageFont;//是否使用图片字体
		char							m_bIsGridObject;//是不是格子对象
		short							m_sGridOffsetY;
		//控件自动移动设置
		VOID							BeginMove(Pixel off, float speed);//开始移动控制，播放完毕之后m_bMoveOffset自动设置成0
		char							m_bMoveOffset;
		float							m_moveSpeed;
		float							m_moveCurr;
		float							m_moveLength;
		float2							m_moveDir;
		float2							m_movePos;
	};

	typedef	UIControl*	ControlPtr;

	class	UISortBase: public Base
	{
	public:
		UISortBase(){m_pNext = 0;}
		virtual	VOID			Render3D() = 0;
		UISortBase*				m_pNext;
	};
	class	UIDrawMgr: public UISortBase
	{
	public:
		UIDrawMgr();
		~UIDrawMgr();
		ImplementComValue();
		VOID					SetTexture(const char* szTexture);
		VOID					SetTextureRect(const Rect& rc);
		VOID					SetTextureMode(const char* szMode);
		VOID					Render(UIDialog* dlg, const Rect& rect, const Pixel* offset = 0);
		VOID					Render(const matrix4x4& world, const Pixel* offset = 0);
		VOID					Render(const Pixel& off, int renderMode = -1, const char* text = 0, float fScale = 1, const Pixel* offset = 0);
		virtual	VOID			Render3D(){Render3D(0,-1,0);}
		VOID					Render3D(const float3* pos = 0, int renderMode = -1, const char* text = 0, float fScale = 1, const Pixel* offset = 0);
		VOID					SetRect(int left, int top, int right, int bottom);
		//
		VOID					SetNumberMode(const char* szFirst, int numberHeight, int space);
		VOID					SetNumber(int num);
		int						GetNumber(){return m_number;}
		//
		VOID					SetProgress(float fProgress){m_fProgress = fProgress; this->m_renderMode = this->DrawMgrMode_Progress;}
		VOID					SetText(const char* szText);
		const char*				GetText(){return m_numberFirst.t;}
		float					GetProgress(){return m_fProgress;}
		//
		VOID					SetRotationX(int f);
		VOID					SetRotationY(int f);
		VOID					SetRotationZ(int f);
		//
		UIDrawMgr*				AddChild();
		VOID					DeleteChild(int n);
		UIDrawMgr*				GetChild(INT index){if(index < 0 || index >= m_childs.size())return 0; return m_childs[index];}
		INT						GetChildCount(){return m_childs.size();}
		BOOL					OnClick(const Pixel& pt, int type);
		BOOL					OnDownClick(const Pixel& pt, int type);
		VOID					OnUpClick(const Pixel& pt, int type);
		//
		VOID					InitUVMap(int width,int height,int wcount,int hcount);
		VOID					InitUVAni(int start,int count,float speed);
		int						m_uvWidth,m_uvHeight,m_uvNumW,m_uvNumH,m_uvStart,m_uvCount;
		float					m_fuvSpeed,m_fuvPlayIndex,m_fuvPlayIndexLast;
	public:
		UIDrawMgr*				m_pNext;
		Rect					m_renderRect;
		float3					m_pos3d;
		//
		float					m_offsetZ;
		BOOL					m_bSetRect;
		float					m_clickScale;
		BOOL					m_bClickDown;
		int						m_nHighlight;
		int						m_dblClick;
		Rect					m_texRect;
		Pixel					m_pixel;
		Pixel					m_center;
		Pixel					m_points[4];//最多4个点
		float2					m_uv[4];
		unsigned int			m_color[4];
		Pixel					m_offPixel;
		int						m_number;
		int						m_data;
		int						m_layer;
		float2					m_scale;
		//
		NameT<32>				m_fnclick;//脚本函数，按下时引发
		float					m_rotx,m_roty,m_rotz;
		BOOL					m_bVisible, m_bEnabled;
		int						m_numberAlign;
		int						m_cullMode;
		NameT<32>				m_value;//如果是数字则显示值
		NameTW<32>				m_textW;//如果是数字则显示值
		short					m_textLength;
		enum{
			DrawMgrMode_Normal = 0,
			DrawMgrMode_Number,
			DrawMgrMode_Progress,
			DrawMgrMode_Text,
		};
		int						m_renderMode;
		float					m_fProgress;
		VOID					RenderProgress(Texture* tex, Pixel* pos, int left, int right, int uBegin, int uEnd, int bBack);
	protected:
		CDynamicArray<UIDrawMgr*,int,0>	m_childs;	//子渲染列表
		CDynamicArrayS<int,0>	m_textIndexs;	//文本索引值
		int						m_textureID;
		bool					m_brotx,m_broty,m_brotz;
		//
		NameT<32>				m_numberFirst;
		int						m_numberHeight, m_numberSpace;
	};


};
#endif
