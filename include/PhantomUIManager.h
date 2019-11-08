//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UIMANAGER_H__
#define	__PHANTOM_UIMANAGER_H__

#include "PhantomUIDialog.h"
#include "PhantomUIAnalyseText.h"

#define	MAX_FLOOR_TYPE_COUNT	10

namespace Phantom{

class dlg_base;
class CUnit2DData;
class	UIDialogRT;
class	UIControlRT;

struct	WayPointInfo
{
	float3					pos;
	float3					dir;
	float					length;
	float					fAngle;
};

class WayPoints: public Base
{
public:
	WayPoints();
	virtual ~WayPoints();
	ImplementComValue();
public:
	INT						GetWayCount(){return m_points.size();}
	VOID					SetWayCount(INT nNewCount){m_points.SetArrayCount(nNewCount);}
	const float3*			GetWayPoint(INT nIndex){if(nIndex < 0 || nIndex >= m_points.size())return 0; return &m_points[nIndex].pos;}
	VOID					SetWayPoint(INT nIndex, const float3& pos){if(nIndex < 0 || nIndex >= m_points.size())return;m_points[nIndex].pos = pos;}
	//
	VOID					Stop(){m_bPlaying = false; m_fLength = 0; m_nIndex = 0;}
	VOID					Pause(BOOL b);
	BOOL					IsPause(){return m_bPause;}
	BOOL					IsPlaying(){return m_bPlaying;}
	VOID					SetMoveSpeed(float fSpeed){m_fSpeed = getmax(fSpeed, 0.0f);}
	float					GetMoveSpeed(){return m_fSpeed;}
	//
	VOID					Play();
	BOOL					FrameMove(float3* ret, float* retRotation);	//返回最新位置
	VOID					SetIs2DMode(BOOL b){m_bIs2DMode = b;}	//是否2D寻路模式
	BOOL					GetIs2DMode(){return m_bIs2DMode;}
public:
	BOOL					m_bIs2DMode;
	BOOL					m_bPlaying;
	BOOL					m_bPause;
	float					m_fSpeed;
	int						m_nIndex;
	float					m_fLength;
	double					m_fLastTime;
	CDynamicArray<WayPointInfo>	m_points;
	//移动完毕之后会返回事件EventID_WayPointOver
};

struct Particle2D
{
	//Rect					saveRect;
	color4					saveColor;
	float2					saveCenter;
	float					fSize;
	float					fLife;
	float					fMaxLife;
	float					fRot;
	float					fOffset;
	int						index;
	Particle2D*				next;
};

typedef	classPtr<UIDialog>	Dialog;
class IMyUICallback
{
public:
	virtual	UIControl*		NewControl(int type, UIDialog* dialogPtr) = 0;//添加控件
	virtual	VOID			RemoveControl(UIControl* c) = 0;//删除控件
};
template <typename T>
class	TMyUICallback: public IMyUICallback
{
public:
	T*		m_ptr;
	TMyUICallback(T* t){m_ptr = t;}
	virtual	UIControl*		NewControl(int type, UIDialog* dialogPtr){if(m_ptr)return m_ptr->NewControl(type, dialogPtr); return 0;}
	virtual	VOID			RemoveControl(UIControl* c){if(m_ptr)m_ptr->RemoveControl(c);}
};

class UIManager
{
public:
	UIManager();
	~UIManager();
	//
	int 						AddTexture( const short_t* strFilename, int filter = TextureFilter_Point);
	int 						AddTexture( const short_t* strFilename, UIDialog* dlg, int filter = TextureFilter_Point);
	int 						AddFont( const char* szFontName, int height, int weight, char italic = false, char underline = false );
	int 						AddTexture( const char* strFilename, int filter = TextureFilter_Point);
	int 						AddTexture( const char* strFilename, UIDialog* dlg, int filter = TextureFilter_Point);
	//
	int 						AddTextureRef( int index );
	int 						DelTexture( int index );
	Texture*				GetTexturePtr(int textureID){return GetTextureNode(textureID, 0);}
	CDynamicArray<TextureRect>*	GetTextureRectPtr(int textureID);										//返回图片指针
	const TextureRect*			GetTextureRect(int textureID, const char* szName);										//返回图片指针
	UIAnalyseText*		GetAnalyseFormatText(){return &m_analyseText;}	//返回文本分析接口
	//
	UIDialog*			FindDialogByID(INT nID);	//根据ID查找对话框
	//
	BOOL						CreateDlg(Dialog& ret, classT<UIDialog, classMgr<UIDialog> >* myClass = 0);
	BOOL						LoadDialog(Dialog& ret, const char* szFile, ImportResult* result = 0, classT<UIDialog, classMgr<UIDialog> >* myClass = 0, BOOL bCreateNew = true);
	void						Render(float fTime, float fElapsedTime, BOOL bBackground = FALSE);
	UIControl*			Pick(int x, int y);
	classMgr<UIDialog>&	GetDialogManager(){return m_dialogs;}
	VOID						SetToolTipTextCB(IToolTipTextCallback* cb){m_toolTipTextCB = cb;}
	//
	Particle2D*					Alloc();					//分配粒子
	VOID						Free(Particle2D* p);	//释放粒子
	VOID						SetupParticles(INT nCount);
	//
	BOOL						OnInputEvent( unsigned int uMsg, const Pixel& pos, unsigned int touchIndex );
	//
	dlg_base*					GetDlgBase(INT nIndex){return m_dlgBases[nIndex];}
	int							GetDlgBaseCount(){return m_dlgBases.size();}
	BOOL						m_bShowUI;
	template <typename T>		VOID	SetCallback(T* t){safe_delete(m_callback); m_callback = new TMyUICallback<T>(t);}
	//
	VOID						DestroyAllDialogs();
	//
	UIDialog*					LoadAvatar(const char* szFileName);
	VOID						OnEditClose(int id, BOOL bIsCancel, const char* text);//关闭编辑框
protected://avatar
	friend class	UIDialogRT;
	BOOL						LocalInputEvent( unsigned int uMsg, const Pixel& pos, unsigned int touchIndex );
	VOID						PushAvatar(UIDialogRT* a);
	VOID						PopAvatar(UIDialogRT* a);
	CDynamicArray<UIDialogRT*>	m_avatars;//纸娃娃新建
	CDynamicArray<UIDialog*>	m_dlgAvatars;//纸娃娃对话框类
	char						m_requestSortAvatar;//是否需要重新排序
protected:
	IMyUICallback*				m_callback;
	CDynamicArray< TextureNode2D* >		m_textureMap;
	CDynamicArray< FontNode2D* >		m_fontMap;
	//
	Particle2D*					m_allParticles;		//所有系统中分配的粒子
	INT							m_particleCount;	//以分配的粒子数量
	INT							m_particleAssignIndex;	//分配到第几个
	Particle2D*					m_topParticle;		//可分配的粒子
protected:
	friend class dlg_base;
	VOID						AddDlgBase(dlg_base* b);
	VOID						RemoveDlgBase(dlg_base* b);
	CDynamicArray<dlg_base*>			m_dlgBases;
protected://textures
	classMgr<UIDialog>	m_dialogs;
	friend class classT<UIDialog, UIManager>;
public:
	BOOL						OnResize();
	FontNode2D*     			GetFontNode( int iIndex );
	Texture*  					GetTextureNode( int iIndex , UIDialog* dlg );
	TextureNode2D*  			GetTextureNode( int iIndex );
	//
	VOID						EnableScaling(BOOL b){m_bEnableScaling = b;}	//是否将界面缩放
	BOOL						IsEnableScaling(){return m_bEnableScaling;}		//
	//
	char						m_bResortDialogs;
	std::string					m_imageFilePath;
	IToolTipTextCallback*		m_toolTipTextCB;
	UIDialog*					m_topDialog;
	//
	VOID						SetDefaultTexture();
	VOID						SetTopMost(UIDialog* dlg, BOOL b);
	//
	struct	ToolTipInfo
	{
		int						id;
		CDynamicArray<short_t>			text;
		UIPopupType				type;
		float					fElapsedTime;
		Rect					rect;
		ToolTipInfo()
		{
			id					=	0;
			fElapsedTime		=	0;
		}
	};
	ToolTipInfo					m_toolTipInfo;
	UIAnalyseText		m_analyseText;
	VOID						SetToolTipTime(float fTime){m_fToolTipTime = fTime;}
	float						GetToolTipTime(){return m_fToolTipTime;}	//返回提示框显示时间
	VOID						SetToolTip(ToolTipData* t, const short_t* text, const Rect& rcInWindow, UIPopupType type);
	void						OnFrameMove( double fTime, float fElapsedTime );	//
	Result 						CreateTexture( unsigned int index, const char* szPath, UIDialog *dlg );
	//
	VOID						Destroy();
protected:
	VOID						OnDeleteDlg(UIDialog *dialogPtr );
	friend class	UIDialog;
	//
	float						m_fToolTipTime;
	BOOL						m_bEnableScaling;
	INT							m_nDlgOrder;
	UIDialog*			m_pModalDlg;
	UIDialog*			m_pSelectDlg;
};

//节约模式,只支持静态(Static)、按钮(Button)控件,进度(Progress)的复制，并且控制对话框的数据
//整套系统由界面管理器统一控制，将脱离原有的对话框渲染过程，也就是用来渲染2D角色和特效等模块专用功能
//控件体系中将直接控制控件作为最低端的控制器
//只有UIControlRT,UIDialogRT两个类,UIBody继承自对话框，UIPart继承自控件、元素、动画等功能
struct	UIEvent{
	char				type;
	Name				func;
};
//最大文本渲染数量128个
struct	UICustomText
{
	int					textIndexs[128];	//文本索引值
	NameAWT<128>		text;
};

class	UIControlRT: public Base//UI2D模块的身体部件
{
public:
	UIControlRT(UIDialogRT* av, UIControl* c);
	~UIControlRT();
	ImplementComValue();
	//函数列表
	void				SetTextColor(const color4& c);
	void				SetTextureColor(const color4& c);
	void				SetOpacity(float f);//写入到元素中
	void				SetTextureID(int id);
	VOID				SetFontID(int id);
	VOID				SetTextureRect(const Rect& rc);
	VOID				SetTextureMode(const char* szMode);
	const char*			GetUVAnimation();
	void				SetUVAnimation(const char* ani);
	ActionType			GetUVAnimationType();
	VOID				SetUVAnimationType(ActionType t);
	VOID				SetUVSpeed(float f);
	VOID				SetProgressValue(float f);
	VOID				SetLayer(int layer);
	VOID				SetText(const char* t);
	ActionType			GetNextActionType();
	VOID				SetNextActionType(ActionType t);
	const char*			GetAction();
	VOID				SetAction(const char* t);
	ActionType			GetActionType();
	VOID				SetActionType(ActionType t);
	VOID				SetActionSpeed(float f);
	VOID				SetRot2DAngle(float r);
	VOID				SetRot2DDir(const Pixel& dir);
	VOID				SetScale(float f);
	VOID				SetTextureFile(const char* file);
	//VOID				BindClick(const char* luaFunc);
	VOID				SetPos(const Pixel& p);
	VOID				StopEmit();
	VOID				SetCurrentTime(float f);
	float				GetTimeLength();
	//
	VOID				SetVisible(BOOL b);
	VOID				SetEnabled(BOOL b);
	//
	VOID				Render();//渲染部件
	VOID				RenderTexture(const PixelW& pos, const PixelW& center, const SizeW& size, const color4& color, float fRotation);//渲染图片
	BOOL				IsAnimationOver(){return (m_bUVPlaying==false&&m_bActionPlaying==false);}
protected:
	BOOL				UpdateAction(PixelW& pos, PixelW& center, SizeW& size, color4& color, float& fRotation);
	VOID				OnInitAction();
	//
	VOID				RecalcPosUV();
	VOID				RecalcUV();
public:
	UIDialogRT*			m_avatar;
	UIControl*			m_ptr;//控件指针
	PixelW				m_textureOffset;
	SizeW				m_textureOrig;
	char				m_unitAngle;
	//Element数据
	short				m_textureID;
	short				m_fontID;
	Rect				m_textureRect;
	float				m_fScale;//缩放值
	short				m_uvIndex;//当前UV动画播放索引
	float				m_uvCurrentTime;//当前UV动画播放时间
	float				m_uvspeed;
	char				m_bUVPlaying,m_bActionPlaying;
	//Control数据
	short				m_fProgressVal;//精确到10000的数值
	char				m_bEnabled;
	char				m_bVisible;
	PixelW				m_position;//相对于对话框的坐标
	PixelW				m_center;//中心点位置
	SizeW				m_renderSize;
	//Size				m_size;//原始大小是图片的区域大小
	char				m_layer;//显示层级
	color4				m_textureColor;
	float				m_fRotation;//转动值
	char				m_actionIndex;//动作索引，非UV动画，是独立的控件动画
	char				m_actionNextType;//下一个动作索引
	float				m_actionCurrentTime;//当前动作播放进度
	float				m_actionSpeed;//当前动作播放速度
	float				m_fRot2DAngle;//2D转动值，不是控件转动值
	float				m_actionForWait;//播放动作前等待时间计算，只用于计算
	float				m_actionForWaitWave;//播放动作前等待时间计算，只用于计算，播放wave音效
	Particle2D*			m_firstParticle;//第一个粒子指针，如果是粒子控件则需要这个,由UIManager控制总体粒子数量
	RenderText*			m_textPtr;//当前文本
	CDynamicArrayS<char,0>	m_textVal;//
};
class	UIDialogRT: public Base
{
public:
	UIDialogRT(UIDialog* dlg);
	~UIDialogRT();
	ImplementComValue();
	//
	VOID				SetCenter(const Pixel& v);//按照某个控件的位置作为中心点
	UIControlRT*			Pick(int x, int y);
	UIControlRT*			FromName(const char* f);
	VOID				SetActionType(ActionType t);
	//VOID				SetAction(const char* szAction);
	VOID				SetVisible(BOOL b);
	VOID				SetOpacity(float f);
	VOID				SetScale(float f);
	//VOID				SetTransform(const matrix4x4& mat);
	VOID				SetRotation(float f);
	VOID				SetPos(const Pixel& p);
	VOID				BindDraw(UIDrawMgr* mgr);
	VOID				SetLayer(int layer);
	//VOID				BindClick(const char* fnLua);
	VOID				SetActionSpeed(float f);
	VOID				SetEnabled(BOOL b);
	VOID				SetRot2DAngle(float fAngle);
	VOID				SetRot2DDir(const Pixel& dir);
	//
	//VOID				SetBindTime(float fSecond, const char* szLuaFunc);
	//
	VOID				Render();//渲染avatar
	//
	//VOID				PushAction(const char* actionName);
	VOID				PushAction(ActionType type);
	//VOID				SetActionFunc(LPCSTR szLuaBegin, LPCSTR szLuaEnd);
	VOID				OnBeginAction(int actiontype);
	VOID				OnEndAction(int actiontype);
	//
	VOID				OnSetActionType(ActionType t);
	//VOID				OnSetAction(const char* szAction);
	BOOL				CheckNextAction();
public:
	UIDialog*			m_ptr;//对话框指针,用指针控制，释放最后一个avatar保证内存会被释放掉
	char				m_requestSort;//需要重新排序
	//
	CDynamicArrayS<UIControlRT*, 0>	m_bodys;
	char				m_bEnabled;//设置是否有效
	char				m_bVisible;
	Pixel				m_center;//默认是所有控件的中心点平均值
	Pixel				m_pos;//默认是中心点就是初始位置
	float				m_fScale;//缩放值，从center点向外扩散缩放
	float				m_fRotation;//转动值
	char				m_actionType;//当前动作类型
	float				m_actionSpeed;
	color4				m_color;//整体图片颜色和透明
	//matrix4x4			m_matAvatar,m_matAvatarInv;//矩阵
	short				m_layer;//角色显示层级，和对话框的层级一并计算
	float				m_currentTime;//当前动作总的播放时间
	float				m_fRot2DAngle;
	CDynamicArrayS<UIDrawMgr*, 0>	m_drawMgrs;
	CDynamicArrayS<short,0>			m_nextActions;
};

template <typename T>
inline	T*	SearchControlUI(Dialog& d, int id, int type)
{
	T* t	=	static_cast<T*>(d->GetControl(id));
	assert(t != 0);
	if(type > 0)
	{
		assert(t->GetType() == type);
	}
	return t;
}

template <typename T>
inline	T*	SearchControlUI(Dialog& d, const char* szName, int type)
{
	T* t	=	static_cast<T*>(d->GetControlFromName(szName));
	assert(t != 0);
	if(type > 0)
	{
		assert(t->GetType() == type);
	}
	return t;
}

template <typename T>
inline	T*	SearchControlUI(UIGroup* g, const char* szName, int type)
{
	T* t	=	static_cast<T*>(g->GetControlFromName(szName));
	assert(t != 0);
	if(type > 0)
	{
		assert(t->GetType() == type);
	}
	return t;
}

inline	UIGroup*	SearchGroupUI(Dialog& d, int id)
{
	UIGroup* t	=	d->FindGroupPtr(id);
	assert(t != 0);
	return t;
}

inline	UIGroup*	SearchGroupUI(Dialog& d, const char* szName)
{
	UIGroup* t	=	d->FindGroupByName(szName);
	assert(t != 0);
	return t;
}

class	dlg_base
{
public:
	dlg_base();
	virtual ~dlg_base();
	Dialog		dlg;
	inline	UIDialog*	operator	->(){return dlg.operator->();}
	inline	operator UIDialog*(){return dlg.GetPtr();}
	//
	inline	BOOL	IsInit(){return (dlg.GetPtr() != 0);}
	inline	BOOL	operator == (BOOL b){return ((dlg.GetPtr() != 0) == b);}
	inline	BOOL	operator != (BOOL b){return ((dlg.GetPtr() != 0) != b);}
	inline	BOOL	operator == (UIDialog* b){return (dlg.GetPtr() == b);}
	inline	BOOL	operator != (UIDialog* b){return (dlg.GetPtr() != b);}
	//
	inline	operator BOOL(){return (dlg.GetPtr() != 0);}
	virtual	BOOL Load(const char* szDlg){return false;}
	virtual	void	clear(){}
	void		destroy(){dlg = 0; clear();}
};

extern	UIManager*				GetDlgManager();
extern	WayPoints*				CreateWayPoints();

};

#endif
