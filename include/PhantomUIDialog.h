//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UIDIALOG_H__
#define	__PHANTOM_UIDIALOG_H__

#include "PhantomBase.h"
#include "PhantomUIControl.h"
#include "PhantomAStarPath.h"
#include "PhantomUITextFormat.h"
#include "PhantomUIButton.h"
#include "PhantomUICheckBox.h"
#include "PhantomUIComboBox.h"
#include "PhantomUIEffect.h"
#include "PhantomUIListBox.h"
#include "PhantomUIListCtrl.h"
#include "PhantomUIProgress.h"
#include "PhantomUIScrollBar.h"
#include "PhantomUISlider.h"
#include "PhantomUITextBox.h"
#include "PhantomUIStaticFormat.h"
#include "Box2D_Usage.h"

namespace Phantom{

#define	MAX_UI_GROUP	100
#define	MAX_LAYER_COUNT	50
#define	MAX_MAP_LAYER_COUNT	6
#define	MAX_LINE_COUNT	256
	class UIDialog;
	class WayPoints;
	class PakDirectory;
	class UIGroup:public Base
	{
	public:
		UIGroup();
		~UIGroup();
		ImpComValue2();
		//
		VOID					SetVisible(BOOL b);
		BOOL					GetVisible();
		VOID					SetName(const char* szName);
		const char*				GetName();
		INT						GetID();
		//
		VOID					OnSetName();
		float					GetSleepTime(){return m_fSleepTime;}
		VOID					SetSleepTime(float f){m_fSleepTime	=  f;}
		VOID					PlayAction(ActionType type);
		ActionType				GetPlayAction(){return m_nPlayAction;}
		ActionType				m_nPlayAction;
		//
		float					GetPlayTime();
		VOID					SetPlayTime(float f);
		float					GetPlaySpeed();
		VOID					SetPlaySpeed(float f);
		float					m_fPlaySpeed;
		float					m_fPlayTime;
		//VOID					Move(int left, int top);
		VOID					SetLeft(int l);
		VOID					SetTop(int t);
		VOID					RecalcRect();
		Rect					m_movePos;
		float					m_fOpacity;
	public:
		VOID					SetGroupIsUnit(BOOL b){m_bIsUnit = b;}//作为2D对象使用
		BOOL					GetGroupIsUnit(){return m_bIsUnit;}
		Pixel					GetPosition();
		//
		INT						GetControlCount(){return m_controls.size();}	//返回组内控件数量
		UIControl*				GetControlPtr(INT nIndex);
		UIControl*				GetControlFromName(const char* szName);
		//
		VOID					SetEnabled(BOOL b){m_bEnabled = b;}	//是否有效
		BOOL					GetEnabled(){return m_bEnabled;}
		//
		VOID					SetPosition(const Pixel& pos);	//设置最新位置
		//
		VOID					PlayAction(const char* szName);
		Name					m_szPlayAction;
		VOID					SetUnitAngle(float fAngle);	//设置面对方向
		float					GetUnitAngle();
		VOID					SetUnitAngleDir(const float2& fAngle);	//设置面对方向
		//
		VOID					RemoveControl(UIControl* c);
		VOID					AddControl(UIControl* c);
		VOID					Resorts();
		VOID					CopyFrom(UIGroup* g);
		VOID					DeleteControls();
		UIDialog*				GetDialogPtr();
		Pixel					m_groupPosition;
		VOID					SetNextPlayAction(ActionType type);
		ActionType				GetNextPlayAction(){return m_nNextPlayAction;}
		ActionType				m_nNextPlayAction;
	public:
		VOID					Load(CSafeFileHelperR& r);
		VOID					Save(CSafeFileHelperW& w);
		VOID					SetID(INT nID);
		//
		float					m_fSleepTime;
		BOOL					m_bEnabled;
		BOOL					m_bVisible;
		int						m_nGroupID;
		PtrID					m_controlCenter;
		Name					m_name;
		UIDialog*				m_dialogPtr;
		//
		BOOL					m_bIsUnit;
		CDynamicArrayS<PtrID,0>		m_controls;
	};

	struct	TextRenderParam
	{
		UIControl*	c;
		UIElement*	e;
	};

	struct	ProgressDrawInfo
	{
		char	bHorzProgress;
		float	fProgress;
	};

	class	DialogAstar: public AstarCallback
	{
	public:
		UIDialog*		m_dlgPtr;
		virtual	int				IsAstarMoveEnable(int x, int y);	//是否可以移动到这里
		virtual	int				GetAstarWidth();					//地形宽度
		virtual	int				GetAstarHeight();				//地形高度
	};

	class	UIRenderDialog
	{
	public:
		virtual VOID	OnRenderDialog(UIDialog* c){}
	};
	template <typename T>
	class	UIRenderDialogT: public UIRenderDialog
	{
	public:
		UIRenderDialogT(){m_t = 0;}
		virtual VOID	OnRenderDialog(UIDialog* c){if(m_t)m_t->OnRenderDialog(c);}
		T*	m_t;
	};
	
	class UIDialog: public Base
	{
	public:
		ImplementComValue();
		VOID							GetFontData(INT nFontID, INT& nHeight, INT& nBold, INT& nUnderLine);
		//
		virtual	UIGroup*				AddGroupPtr();	//新加一个组合
		virtual	UIGroup*				FindGroupPtr(PtrID& id);		//查找组合ID
		virtual	UIGroup*				FindGroupPtr(int id);		//查找组合ID
		virtual	UIGroup*				FindGroupByName(const char* szName);		//查找组合ID
		virtual	VOID					RemoveGroupPtr(PtrID& id);	//删除组和其内部的所有控件
		virtual	UIControl*				FindControl(PtrID& id);	//根据ID查找控件
		//
		virtual	INT						DrawImage(INT nImageIndex, const Rect& rcImage, BOOL bSelect, VOID* lpParam, INT color = 0xffffffff);
		virtual	void					SetTransparent(float f);
		float							GetTransparent();
		VOID							SetRenderCallback(UIRenderDialog* c){m_renderCB = c;}
		UIRenderDialog*					m_renderCB;

		INT								GetID(){return this->m_nDialogID;}
		virtual	UIControl*				AddCustomControl(int type, const Rect& rect);//添加自定义控件
		virtual	UIControl*				FindControlByID(INT nID);	//根据ID查找控件
		virtual	UITextBox*				AddEditBox( BOOL bInit, const char* strText, int x, int y, int width, int height );
		virtual	UIStatic*				AddStatic(BOOL bInit, const char* strText, int x, int y, int width, int height, char bIsDefault=0);
		virtual	UIScrollBar*			AddScrollBar(BOOL bInit,  int x, int y, int width, int height);
		virtual	UIButton* 				AddButton(BOOL bInit,  const char* strText, int x, int y, int width, int height, char bIsDefault=0 );
		virtual	UICheckBox*				AddCheckBox(BOOL bInit,  const char* strText, int x, int y, int width, int height, char bChecked=0, char bIsDefault=0 );
		virtual	UIComboBox*				AddComboBox(BOOL bInit,  int x, int y, int width, int height, char bIsDefault=false );
		virtual	UISlider*				AddSlider(BOOL bInit,  int x, int y, int width, int height, int min=0, int max=100, int value=50, char bIsDefault=0 );
		virtual	UIListBox*				AddListBox(BOOL bInit,  int x, int y, int width, int height, BOOL bMultiselect = FALSE );
		virtual	UIProgress*				AddProgress(BOOL bInit,  int x, int y, int width, int height, char bIsDefault=0);
		virtual	UIEffect*				AddEffect(BOOL bInit,  int x, int y, int width, int height, char bIsDefault=0);
		virtual	UIStaticFormat*			AddFormatStatic(BOOL bInit,  const char* strText, int x, int y, int width, int height, char bIsDefault=0);
		virtual	UIListCtrl*				AddListCtrl(BOOL bInit,  int x, int y, int width, int height);							//添加滚动框
		//
		virtual	char					DrawRect2D( const Rect* pRect, unsigned int color, char bFill = true , BOOL bLingXing = false);
		virtual	char					CalcTextRect( const short_t* strText, Rect* prcDest, int nFontID, int nCount = -1, unsigned int textFormat = Phantom::DrawTextAlign_LEFT);
		virtual	BOOL					FindControl(UIControl* pC);

		virtual	BOOL					SetControlOrder(UIControl* pC, BOOL bUpDown);

		virtual	VOID					PlayAction(ActionType type);
		virtual	VOID					PlayAction(const char* szName);
		ActionType						GetPlayAction(){return m_nPlayAction;}
		virtual	VOID					SetFullscreen(BOOL b, int dlgWidth, int dlgHeight, int align = DrawTextAlign_CENTER | DrawTextAlign_VCENTER);	//是否填满整个窗体
		inline	BOOL					GetFullscreen(){return m_bFullScreen;}
		inline	int						GetFullscreenWidth(){return m_nFullscreenWidth;}
		inline	int						GetFullscreenHeight(){return m_nFullscreenHeight;}

		UIControl*						GetControlFromName(const char* szName);
		const char*						GetName();
		virtual	void					SetName(const char* s);
		virtual	void 					RemoveControlPtr( int index );
		virtual	void 					RemoveControl( int ID );
		virtual	void 					RemoveAllControls();
		//
		virtual	INT						GetGroupCount();
		virtual	UIGroup*				GetGroupPtr(INT nIndex);
		VOID							SetGroupVisibleByID(INT nID, BOOL bVisible);
		BOOL							GetGroupVisibleByID(INT nID);
		//
		//VOID							SetScale(float fx, float fy);
		//float2							m_scale;
		//
		ActionType						m_nPlayAction;
		Name							m_szPlayAction;
		std::vector<UIGroup*>			m_groups;//[MAX_UI_GROUP];
		UIGroup							m_defaultGroup;
		//
		BOOL							GetMapEnable(){return m_bMapEnable;}	//2D图片信息是否有效
		const Map2DGridInfo*				GetMapGridInfo(INT x, INT y);
		const Size&						GetMapGridCount(){return m_mapGridCount;}
		VOID							SetMapGridInfo(INT x, INT y, const Map2DGridInfo& f);
		const Size&						GetMapGridSize(){return m_mapGridSize;}
		//
		VOID							SetMapEnable(BOOL b);
		VOID							SetMapGridCount(const Size& s);
		VOID							SetMapGridSize(const Size& s);
		Pixel							GetMapGridIndex(const Pixel& pixel);//返回像素对应的格子索引，对话框位置开始的像素
		Pixel							GetMapGridCenter(int x, int y);//返回格子索引对应的中心点像素坐标
		//
		VOID							SetMapGridVisible(BOOL b){m_bMapGridVisible = b;}
		BOOL							GetMapGridVisible(){return m_bMapGridVisible;}
		virtual	BOOL					CreateWay(WayPoints* way, const Pixel& from, const Pixel& to);//
		virtual	BOOL					CreateWay(WayPoints* way, int fx, int fy, int tx, int ty);//
		Pixel							MapToAstarMap(int x, int y);//从地图坐标转换成寻路坐标
		Pixel							AstarMapToMap(int ax, int ay);//从寻路坐标转换成地图坐标
		//
		AStarPath						m_astar;
		//
		VOID							OnRenderMap();//渲染地形图片
		BOOL							m_bMapGridVisible;
		BOOL							m_bMapEnable;
		char							m_bMap25DMode;//是否2.5D模式
		CDynamicArray<Map2DGridInfo,int,0>		m_mapGrids;	//地图格子信息
		CDynamicArray<Map2DAstarInfo,int,0>		m_mapAstarGrids;	//地图格子信息
		Size							m_mapAstarGridCount;
		Size							m_mapGridCount;
		Size							m_mapGridSize;
		Pixel							m_mapGridBegin;
		//
		DialogAstar						m_astarCallback;
		CDynamicArray<UIControl*>		m_clones;
		//
		char 							GetVisible() { return m_bVisible; }
		virtual	void 					SetVisible( char bVisible );
		void 							SetBackgroundColors( unsigned int colorAllCorners ) { SetBackgroundColors( colorAllCorners, colorAllCorners, colorAllCorners, colorAllCorners ); }
		void 							GetLocation( int& x, int& y ) const;
		virtual	void 					SetLocation( int x, int y );
		virtual	void 					SetSize( int width, int height );
		void							SetLeft(int left);
		void							SetTop(int top);
		int 							GetWidth();
		int 							GetHeight();
		void 							SetBackgroundColors( unsigned int colorTopLeft, unsigned int colorTopRight, unsigned int colorBottomLeft, unsigned int colorBottomRight );
		//
		virtual	UIControl*				CreateFromType(BOOL bInit, int type);
		UIControl*						GetControlPtrFromID(int id);
		int								GetControlCount();
		UIControl*						GetControlPtr(int index);
		//
		VOID							SetContext(INT nParameter){m_nContextID = nParameter;}	//设置自定义参数
		INT								GetContext(){return m_nContextID;}				//返回自定义参数
		int								m_nContextID;
		UIControl* 						GetControl( int ID );
		UIControl* 						GetControl( int ID, unsigned int nControlType );
		UIControl*						PickControl( Pixel pt );
		UIControl*						GetControl(PtrID& id);
		char 							GetControlEnabled( int ID );
		void 							SetControlEnabled( int ID, char bEnabled );
		void 							ClearRadioButtonGroup( unsigned int nGroup );
		void 							RequestFocus( UIControl* pControl , unsigned int touchIndex);
		Result 							CalcTextRect( const short_t* strText, UIElement* pElement, Rect* prcDest, int nCount = -1 );
		VOID							DrawSpriteNoOffsetE(Texture* pTextureNode, Rect* screenRc, Rect* prcTexture, Rect* prcDest, const color4& c, float3* pCenter, matrix4x4* pMatRot, int nHighlight, BOOL bEnabled);
		BOOL							DrawTexture( BOOL bEnabled, Texture* tex, Rect* textureRect, Rect* prcDest, const color4& color, BOOL bScale = false, int colorop = 0, int tileWidth = 0, int tileHeight = 0);
		BOOL							DrawElement( BOOL bEnabled, UIElement* pElement, Rect* textureRect, Rect* prcDest, const color4& color, ControlAnimation* ani = 0, UIControl* c = 0, const matrix4x4* renderMat = 0);
		BOOL							DrawElement( RenderText* textPtr, BOOL bEnabled, UIElement* pElement, Rect* textureRect, Rect* prcDest, const color4& color, ControlAnimation* ani = 0, UIControl* c = 0, const matrix4x4* renderMat = 0);
		BOOL							DrawElementColor( BOOL bEnabled, UIElement* pElement, Rect* prcDest, const color4& color, ControlAnimation* ani = 0, UIControl* c = 0, const matrix4x4* renderMat = 0);
		char							IsTooltipWindow(){return m_bIsToolTipWindow;}					//是否是提示对话框
		void							SetTooltipWindow(char bEnable){m_bIsToolTipWindow = bEnable;}	//是否是提示对话框
		Size							GetImageSize(INT nImageIndex);
		char							m_bIsToolTipWindow;
		void							ResortControls();
		VOID							CalcRenderMatrix(matrix4x4* mat);
		VOID							OnChangeAlignMode();
		//
		UIControl*						Pick(int x, int y);
		void							DeleteControlPtr(UIControl* c);
		char							LoadDialog(const char* szFile, ImportResult* result = 0);
		char							LoadDialog(CSafeFileHelperR& r, ImportResult* result = 0, INT* version = 0);
		char							SaveDialog(const char* szFile);
		char							SaveDialog(CSafeFileHelperW& w);
		//
		PakDirectory*					m_pakdir;//是否压缩文件
		char							ExportPak(const char* szFile);
		//
		const Rect&						GetRenderRect(){return m_renderRect;}						//渲染区域
		//
		void							SetBackgroundWindow(char b);
		char							IsBackgroundWindow();
		inline	BOOL					IsTopMost(){return this->m_bTopMost;}
		void							SetTopMost(BOOL bTop);
		void							DoModal(char bModal);		//设置独占模式
		BOOL							IsDoModal();
		BOOL							m_bEnabled;
		void							SetShowSound(const char* szFile);

		static BOOL						ShowTipText(UIControl* pControl);
		void							RebuildRenderRect();
		//是否在失去焦点的时候隐藏自身
		VOID							SetHideOnKillFocus(BOOL b){m_bHideOnKillFocus = b;}
		BOOL							GetHideOnKillFocus(){return m_bHideOnKillFocus;}
		//
		VOID							SetDialogColor(const color4& c){m_dlgColor = c;}
		const color4&					GetDialogColor(){return m_dlgColor;}
		//
		VOID							RaiseEvent(UIControl* c, INT nEventID, const Pixel& pos);
		//
		inline	const matrix4x4&		GetWorldMatrix(){return m_dlgMatrix;}
		inline	const matrix4x4&		GetWorldMatrixInv(){return m_dlgMatrixInv;}
		//virtual	VOID					SetWorldMatrix(const matrix4x4& mat);
		VOID							SetWorldRotationX(float fX, const Pixel& center);
		VOID							SetWorldRotationY(float fY, const Pixel& center);
		VOID							SetWorldRotationZ(float fZ, const Pixel& center);
		//virtual	VOID					SetWorldMatrixByCenter(const matrix4x4& mat);
		//
		VOID							SetWorldScaling(const float2& s, const Pixel& center);
		//inline	VOID					SetWorldMatrixEnable(BOOL b){m_bEnableDlgMatrix = b;}
		//inline	BOOL					GetWorldMatrixEnable(){return m_bEnableDlgMatrix;}
		Pixel							m_dlgCenter;
		float3							m_dlgRotation;
		float2							m_dlgScale;
		UIControl*						m_pParentControl;//在控件内渲染
		BOOL							m_bRequestResort;
		VOID							RecalcRect(BOOL bIncludeVisible=true);
		//
		VOID							RebuildAstar();
		VOID							Add2DChild(UIDialog* dlg);
		VOID							ClearChilds();
		VOID							BeginAutoMove(float fSpeed, LPCSTR szLuaFunc);
		VOID							AddMoveTo(const Pixel& fw);
		VOID							AddMoveTo(WayPoints* fw);
		VOID							StopAutoMove();
		BOOL							ToNextMove();
		VOID							MoveToFrame(float fElapsedTime);
		char							m_bIsChildRender;//是否作为子对话框渲染的
		struct RenderData{
			UIControl*					c;
			UIDialog*					d;
		};
		CDynamicArrayS<RenderData,0>	m_renders;//子对话框列表,指针根据对话框释放加减
		CDynamicArrayS<UIDialog*,0>		m_childs;//子对话框列表,指针根据对话框释放加减
		Name							m_luaFunc;
		CDynamicArrayS<float2,5>		m_wayPoints;//子对话框列表,指针根据对话框释放加减
		float2							m_wayFrom;
		float2							m_wayDir;
		float							m_wayCurr, m_wayLength,m_waySpeed;
		char							m_wayMode;//移动模式
		//
		UIControl*						m_centerObj;//中心坐标控件
		UIDialog*						m_dlgParent;
		Pixel							GetPosition();//
		VOID							SetPosition(const Pixel& pos);	//设置最新位置
	public:
		BOOL							OnChildInput(unsigned int uMsg, Pixel pt, unsigned int touchIndex);
		Pixel							m_childDown;
		int								m_childDownIndex;
		char							m_childDownMoved;
		float							m_childDownTime;
		BOOL							m_childAutoMove;
		Pixel							m_childDownMove;
        Pixel                           m_childBeforeMove;
		float2							m_childMoveStrength;//移动强度
		VOID							ClipChildOffset(int&x,int&y);
		//
		virtual	~UIDialog();
		BOOL							OnInputEvent( unsigned int uMsg, const Pixel& pos, unsigned int touchIndex );
		//
		VOID							RenderControl(UIControl* c, float fElapsedTime);
		Result							OnRender( float fElapsedTime, BOOL bBackground = false );
		VOID							OnRender2( float fElapsedTime, BOOL bChanged );
		//
		VOID							BeginRender();
		VOID							UpdateDialogSize(UIControl* c);
		void							OnLoadLua();
		void							OnHold(BOOL b);//保留对话框不会被自动删除
		char							m_bIsHold;
		char							m_bIsPause;
	public:
		int								m_layer;
		BOOL							m_bHideOnKillFocus;
		Rect							m_renderRect;
		char							m_bIsBackground;
		int								m_nControlID;
		FileName						m_strFile;
		//
		inline	int						GetLeft(){return m_x;}
		inline	int						GetTop(){return m_y;}
		int								m_nDefaultControlID;
		inline	INT						GetNewElementID(){return m_nElementID++;}

		void 							OnMouseMove( Pixel pt );
		void 							OnMouseUp( Pixel pt );
		int								IsAstarMoveEnable(int x, int y);	//是否可以移动到这里
		//可以清除的运行时数据
		int								IsAstarMoveEnable2(int x, int y);	//是否可以移动到这里
		void							SetAstarMoveEnable2(int x, int y, BOOL b);
		void							ClearAstarMoveEnable2();
		//
		int								GetAstarWidth();					//地形宽度
		int								GetAstarHeight();				//地形高度
		//
#ifndef EXPORT_ENGINE
//		B2World							m_box2World;
#endif
		int								m_dlgX, m_dlgY;
		Size							m_screenSize;
		VOID							OnSetMouseMatrix(Pixel& point);
		int 							m_width;
		int 							m_height;
		BOOL							m_bUpdate;
		VOID							UpdateMatrix();
	protected:
		virtual	Result	 				OnAddControl( UIControl* pControl );
	protected:
		//
		static UIControl* 				s_pControlFocus[MAX_TOUCH_COUNT];
		static UIControl* 				s_pControlPressed;

		UIControl*						m_pControlMouseOver;
		//
		VOID							AlignDialog();
		//
		//int								m_nDlgAlign;
		//Pixel							m_dlgAlignOffset;

		char 							m_bVisible;
		char 							m_bDrag;

		char							m_szName[50];

		std::string						m_showDlgSound;	//显示对话框时候的声效
		//
		int 							m_x;
		int 							m_y;
		Pixel							m_pos;

		unsigned int 					m_colorTopLeft;
		unsigned int 					m_colorTopRight;
		unsigned int 					m_colorBottomLeft;
		unsigned int 					m_colorBottomRight;
		INT								m_nDialogID;
		CDynamicArrayS< UIControl*, 0 >		m_controls;
		color4							m_dlgColor;
		BOOL							m_bTopMost;
		INT								m_nDlgOrder;
		int								m_nElementID;
		//
		BOOL							m_bFullScreen;
		int								m_nFullscreenWidth;
		int								m_nFullscreenHeight;
		matrix4x4						m_dlgMatrix;
		matrix4x4						m_dlgMatrixInv;
		BOOL							m_bEnableDlgMatrix;
		//
		INT								m_nAvatarRef;
		int								m_nLastAlignMode;
		int								m_lastDlgX,m_lastDlgY;
		//
		friend INT	compareDlg(const void* v1, const void* v2);
		UIDialog();
		friend class UIManager;
		friend class UIControl;
		friend class UIDialogRT;
	};
	extern	VOID	RenderScroll(UIDialog* dlg, const Rect& dest, const color4& color, Texture* scrollTex = 0, const Rect* scrollRc = 0);
};

#endif

