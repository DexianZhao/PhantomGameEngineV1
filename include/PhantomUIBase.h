//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UIBase_H__
#define	__PHANTOM_UIBase_H__

#include "PhantomBase.h"

namespace Phantom{
	class	Texture;
	class	Font;
	class	UVAnimation;
	class	UIManager;
	class	UIControl;
	class	UIDialog;

	struct FontNode2D
	{
		Name			fontName;
		int				nFontHeight;
		int				nWeight;
		char			bItalic;
		char			bUnderline;
		inline	bool	operator == (const FontNode2D& f) const
		{
			if(nFontHeight != f.nFontHeight || nWeight != f.nWeight || bItalic != f.bItalic || bUnderline != f.bUnderline || (stricmp(fontName.t, f.fontName.t) != 0))
				return false;
			return true;
		}
		AutoPtr<Font>	fontPtr;
		inline	int		GetHeight(){if(nFontHeight < 0)return -nFontHeight; return nFontHeight;}
		FontNode2D()
		{
			nFontHeight	=	0;
			nWeight		=	0;
			bItalic		=	0;
			bUnderline	=	0;
		}
	};

	struct TextureNode2D
	{
		FileName					fileName;
		bool						bLoadFail;
		INT							nRef;
		int							filter;
		AutoPtr<Texture>			texturePtr;
		TextureNode2D(){nRef = 1; bLoadFail = false; filter = 1;}
	};
	struct	ToolTipData
	{
		UIControl*		c;
		ToolTipData(){c = 0;}
	};

	class IToolTipTextCallback
	{
	public:
		virtual	VOID					ShowToolTip(const short_t* szTipText, ToolTipData* c, const Rect& rc, UIPopupType type) = 0;	//显示或者隐藏提示框
		virtual	VOID					UpdatePos(const Rect& rc, UIPopupType type) = 0;	//显示或者隐藏提示框
	};
	struct ControlAnimation
	{
		Rect					rc;
		color4					textColor;
		color4					textureColor;
		float					timeLength;
		bool					bRotateEnable;	//转动有效
		float					rot;	//转动率
		Pixel					center;	//中心点
	};
#pragma pack(push, 1)
	struct	MapGridInfo
	{
		enum MapGridInfoFlags
		{
			MapGridInfoFlags_Disable	=	0x01,	//不可移动位置
		};
		unsigned char			index;
		unsigned char			flags;
		MapGridInfo(){index = flags = 0;}
		inline	bool			isEnabled(){return ((flags & MapGridInfoFlags_Disable) == 0);}
	};
	struct	Map2DGridInfo
	{
		enum MapGridInfoFlags
		{
			MapGridInfoFlags_Disable	=	0x01,	//不可移动位置
			MapGridInfoFlags_Disable2	=	0x02,
		};
		short					rx,ry;//寻路对应的坐标
		unsigned char			flags;
		Map2DGridInfo(){flags = 0;}
		inline	bool			isEnabled(){return ((flags & MapGridInfoFlags_Disable) == 0);}
	};
	struct	Map2DAstarInfo{
		short					mx,my;//地图对应的坐标
	};
#pragma pack(pop)
	//
	struct ListBoxItem
	{
		ListBoxItem();
		Text			itemText;//
		INT				pData;
		Rect  			rcActive;
		char  			bVisible;
		char  			bSelected;
		char			bTextColor;
		color4			textColor;
		char			bHighColor;
		color4			textHighColor;
	};

	struct	TextureRectUV: TextureRect
	{
		Pixel							offset;
		int								textureIndex;
		VOID							SetTextureID(int nID);
	};

#pragma pack(push, 1)

struct	UVAnimationData
{
	short							m_nextAction;
	char							m_bCustomAni;
	short							m_nLoopCount;
	short							m_nPlayCount;
	short							m_nBeginIndex;
	short							m_nFrameCount;
	float							m_fPlaySpeed;
	float							m_fPlayTime;
	NameT<32>						m_name;
	char							m_nLayerAdd;
	char							m_bIsDefault;
	short							m_nActionType;
	short							m_nUVWidthCount;
	short							m_nUVHeightCount;
	short 							m_textureIndex;
	Rect							m_textureRect;
	int								m_nAnimationID;
	char							m_bIsCustomSet;
	short							m_angle2D;
	char							m_bIsInvW,m_bIsInvH;
};
struct	ElementTextureRect
{
	Rect			rc;
	Pixel			off;
	Size			orig;
};
struct	ElementData
{
	short					m_playAction;
	Rect					m_rectOffset;
	INT						m_nElementID;
	short					m_nSrcBlend;
	short					m_nDestBlend;
	short					m_nColorOP;
	//
	short 					textureIndex;
	short 					fontIndex;
	unsigned short			dwTextFormat;
	color4 					textureColor;
	color4 					fontColor;
	color4					shadowColor;
	//
	char					m_bRenderScale;
	char					m_bWrap;
	short					m_nTileWidth;
	short					m_nTileHeight;
	Pixel					m_wrapWidth;
	short					m_nUVWidthCount;
	short					m_nUVHeightCount;
	float					m_fUVPlaySpeed;
	Rect					textureRect;
	//
	float					m_fUVPlayTime;
	Rect					rcTexture;
	Pixel					m_imgOffset;
	Size					m_imgSize;
	ElementTextureRect		m_modeRect;
	char					m_bIsInvW,m_bIsInvH;
	char					m_imageFilter;
	Name					m_textureMode;
	char					m_bNoTexture;
};

struct	ControlData
{
	short					m_nPlayAction;
	BOOL					m_bIsBackground;
	PtrID					m_groupID;
	//
	short					m_value;
	short					m_minValue;
	short					m_maxValue;
	PtrID					m_parentID;
	short					m_buttonType;
	char					m_bDrawButtonFrame;
	float					m_fPlaySpeed;
	short					m_nTrackLoopCount;
	short					m_nPlayLoopCount;
	char					m_bChanged;
	unsigned int			m_shadowColor;
	short					m_nLevel;
	char					m_bReadOnly;
	char					m_bCustomRender;
	char 					m_bVisible;
	char 					m_bMouseEnter;
	char 					m_bHasFocus;
	char 					m_bIsDefault;
	Name					m_name;
	char					m_bEnableRotation;
	float					m_rotation;
	short					m_loadVersion;
	char					m_nProgressType;
	char					m_bLoopMode;
	float					m_fPlayTime;
	Name					m_soundFile;
	char					m_bParentMove;
	char					m_bMoveDown;
	Pixel					m_moveOffset;
	Pixel					m_rotCenter;
	char					m_bTrackAutoBeginAni;	//是否在对话框显示的时候自动开始计算动画
	char					m_bTrackPlayAni;
	char					m_bTrackPause;
	float					m_fTrackPlaySpeed;
	Rect					m_saveWindowRect;
	Pixel					m_saveRotCenter;
	float					m_saveRotation;
	Rect					m_windowRect;
	INT						m_nAnimationID;
	color4					m_textColor;
	color4					m_textureColor;
	float					m_fSleepTimeCur;
	float					m_fSleepTime;
	unsigned short			m_textFormat;
	char					m_nShadowWidth;
	char					m_bTrackSaved;
	int						m_controlID;
	char					m_bIsEnabled;
	char					m_bCanFocus;
	short					m_nTabIndex;
	char					m_bSelect;//是否可以被选择
	char					m_bReserved;//保留
	char					m_bIsInvalidate;
	color4					m_disableColor;
	char   					m_scrollBarWidth;
	char					m_bIsPickPixel;
	float					m_fAngle;
	short					m_unitAngle;
	char					m_bEnableUnitAngle;
	short					m_nLayerAdd;
	char					m_bAutoSort;
	char					m_bVisibleOnScript;
	char					m_bHideOnEditor;
	//
	//2013-1-3
	char					m_bEnableUseImageText;
	char					m_szNumberBegin[28];
	char					m_bAniCenter;	//是否以中心点作为左右上下的动画
};
#pragma pack(pop)

	class UIElement;

	class UVAnimation: public UVAnimationData
	{
	public:
		UVAnimation(UIElement* e);
		~UVAnimation();
		INT								GetID(){return m_nAnimationID;}
		const char*						GetName();
		//
		VOID							SetPlaySpeed(float f);
		float							GetPlaySpeed();
		VOID							SetLoopCount(INT nLoop = -1);//-1表示一直循环
		int								GetLoopCount();
		VOID							SetInvW(BOOL b){m_bIsInvW = b;}
		VOID							SetInvH(BOOL b){m_bIsInvH = b;}
		BOOL							IsInvW(){return m_bIsInvW;}
		BOOL							IsInvH(){return m_bIsInvH;}
		//
		VOID							SetUVMapIndex(INT nBegin, INT nEnd);
		VOID							GetUVMapIndex(INT& nBegin, INT& nEnd);
		VOID							CloneFrom(UVAnimation* e);
		VOID							CopyFrom(UVAnimation* e){CloneFrom(e);}
		//
		TextureRect*					FrameMove(Rect& ret, int& textureIndex, Pixel& offset, float fElapsedTime);
		TextureRect*					SetCurrentIndex(Rect& ret, int& textureIndex, INT nIndex, Pixel& offset);
		BOOL							SetCurrentIndex(Rect& ret, INT nIndex, Pixel& offset);
		//
		VOID							Load(CSafeFileHelperR& r);
		VOID							Save(CSafeFileHelperW& w);
		Unit2DAngle						GetAngle(){return (Unit2DAngle)m_angle2D;}
		//
		VOID							SetLayerAdd(INT nAdd){m_nLayerAdd = nAdd;}	//层级增减
		INT								GetLayerAdd(){return m_nLayerAdd;}			//渲染优先级增减
		//
		BOOL							IsCustomEnable(){return m_bIsCustomSet;}
		VOID							SetCustomEnable(BOOL b){m_bIsCustomSet = b;if(m_bIsCustomSet)SetUVMapIndex(m_nBeginIndex, m_nFrameCount);}
		int 							SetTexture( const char* strFilename );
		const char*						GetTexture();
		void							SetTextureID(int textureID);
		int								GetTextureID(){return m_textureIndex;}
		//
		VOID							SetTextureRect(INT nIndex, const TextureRect& rc);
		VOID							SetTextureRectFile(INT nIndex, const char* strFilename);
		int								GetTextureRectFile(INT nIndex);
		const char*						GetTextureRectFileName(INT nIndex);
		//
		VOID							SetTextureRect(INT nIndex, const char* szMode);
		const TextureRect*				GetTextureRect(INT nIndex);
		VOID							SetTextureOffset(INT nIndex, const Pixel& offset);
		const Pixel*					GetTextureOffset(INT nIndex);
		//
		VOID							SetUVAnimation(INT nWidthCount, INT nHeightCount);
		VOID							GetUVAnimation(INT& nWidthCount, INT& nHeightCount);
		const Rect&					GetSourceRect(){return (Rect&)m_textureRect;}
		void							SetSourceRect(const Rect& rc){m_textureRect	=	(Rect&)rc;}
		//
		ActionType				GetActionType(){return (ActionType)m_nActionType;}
		VOID							SetActionType(ActionType t);
		//
		VOID							SetNextAction(ActionType t){m_nextAction = t;}
		ActionType				GetNextAction(){return (ActionType)m_nextAction;}
		//
		VOID							SetPlayTime(float f){m_fPlayTime = f; m_bCustomAni = true;}
		float							GetPlayTime(){return m_fPlayTime;}
		VOID							RegetRects();
		BOOL							IsPlayOver();
		char							m_bMoveOffset;//移动是否有效
		Pixel							m_moveOffset;//UV动画附带移动功能，从原始控件位置(savedPos)+moveOffset点的位置，并不是当前点位置移动，这样可以避免错误发生
		float							m_moveSpeed;//移动速度,最少0.1
		float							m_nextRand;//下一个动作播放概率，如果不进入播放则继续循环这个过程
	public:
		UIElement*						m_elementPtr;
		CDynamicArrayS<TextureRectUV,0>		m_rects;
	};
	class UIDialog;
	class UIControl;
	class UIElement;

	typedef BaseT<UIElement> UIElementBase;
	class UIElement: public ElementData
	{
	public:
		void 					SetTexture( unsigned int textureIndex, Rect* prcTexture, unsigned int defaultTextureColor = COLOR_ARGB(255, 255, 255, 255) );
		void 					SetFont( unsigned int fontIndex, unsigned int defaultFontColor = COLOR_ARGB(255, 255, 255, 255), DWORD dwTextFormat = DrawTextAlign_CENTER | DrawTextAlign_VCENTER);
		//
		UIElementBase	m_this;
		VOID					SetTextureMode(const char* szName, DrawTextAlign align = DrawTextAlign_LEFT);//是否按照高度重新设置缩放
		const char*				GetTextureMode();
		int 					SetTexture( const char* strFilename );
		const char*				GetTexture();
		void					SetTextureID(int textureID);
		void					SetFontID(int fontID){fontIndex	=	fontID;}
		int						GetTextureID(){return textureIndex;}
		int						GetTextureNewID(){return textureIndexNew;}
		int						GetFontID(){return fontIndex;}
		const Rect&				GetSourceRect(){return (Rect&)textureRect;}
		void					SetSourceRect(const Rect& rc);
		const color4&			GetTextColor(){return (color4&)fontColor;}
		void					SetTextColor(const color4& c){fontColor	=	(color4&)c;}
		const color4&			GetTextureColor(){return (color4&)textureColor;}
		void					SetTextureColor(const color4& c){textureColor = (color4&)c;}
		const color4&			GetShadowColor(){return shadowColor;}
		void					SetShadowColor(const color4& c){shadowColor = c;}
//		const char*				GetElementNameA(){return m_name.strA();}
//		VOID					SetElementNameA(const char* szName){m_name = szName;}
		char					LoadElement(UIManager*, UIDialog* dlg, CSafeFileHelperR& r, char bOld, char bNewVersion);
		BOOL					SaveElement(UIManager* p, CSafeFileHelperW& w);
		VOID					RandomSetMode();	//随即设置一个索引

		char					GetRenderScaleAsWrap(){return m_bWrap;}			//是否叠加缩放
		Pixel					GetWrapWidth(){return m_wrapWidth;}
		char					GetRenderScale(){return this->m_bRenderScale;}
		void					SetRenderScale(char b, char bWrap, Pixel wrapWidth){m_bRenderScale = b; m_bWrap = bWrap; m_wrapWidth = wrapWidth;}
		VOID					SetInvW(BOOL b){m_bIsInvW = b;}
		VOID					SetInvH(BOOL b){m_bIsInvH = b;}
		BOOL					IsInvW(){return m_bIsInvW;}
		BOOL					IsInvH(){return m_bIsInvH;}
		//
		BOOL					m_bIsInvWTo,m_bIsInvHTo;
		char					m_tileXieJiao;//是否斜角平铺
		//
		INT						GetSrcBlend(){return this->m_nSrcBlend;}
		INT						GetDestBlend(){return this->m_nDestBlend;}
		INT						GetColorOP(){return this->m_nColorOP;}
		VOID					SetSrcBlend(INT nBlend){m_nSrcBlend = nBlend;}
		VOID					SetDestBlend(INT nBlend){m_nDestBlend = nBlend;}
		VOID					SetColorOP(INT nOP){m_nColorOP = nOP;}

		VOID					SetUVAnimation(INT nWidthCount, INT nHeightCount);
		VOID					GetUVAnimation(INT& nWidthCount, INT& nHeightCount);
		VOID					SetUVAnimationIndex(INT nIndex);
		VOID					SetUVPlaySpeed(float f);
		float					GetUVPlaySpeed();

		VOID					OnFrameMove(float fElapsedTime, Pixel& offset, INT& layerAdd);

		UVAnimation*	GetUVAnimationPtr(INT nIndex);
		VOID					AddUVAnimation(UVAnimation* c = 0);//添加一个
		VOID					RemoveUVAnimation(UVAnimation* e);
		VOID					SetUVAnimationDefault(UVAnimation* e);
		UVAnimation*	GetUVAnimationDefault();
		VOID					ActiveUVAnimation(UVAnimation* e);
		UVAnimation*	GetActiveUVAnimation();
		INT						GetUVAnimationCount();
		VOID					ChangeAngle();
		//
		int						textureIndexNew;
		short					m_nTileOffX;
		short					m_nTileOffY;
		float					m_nTileScaleX,m_nTileScaleY;
		CDynamicArrayS<UVAnimation*,0>	m_uvAnimations;
		UVAnimation*		m_activeUVAnimation;
		UVAnimation*		m_defaultUVAni;
		UVAnimation*		FindUVMapAnimation(const char* szName);
		//
		VOID					RebuildUVAngles();
		UVAnimation*		m_angleUV[Unit2DAngle_Count];
		//
		VOID					PlayAction(ActionType type);
		ActionType		GetPlayAction(){return (ActionType)m_playAction;}
		//
		UIElement(UIControl* dlg = 0);
		VOID					SetControlPtr(UIControl* dlg);
		virtual	~UIElement();
		void	operator	=	(UIElement& c);
		VOID					CopyFrom(UIElement* e);
		//
		VOID					SetTileWidth(int nTileWidth){m_nTileWidth = nTileWidth;}
		VOID					SetTileHeight(int nTileHeight){m_nTileHeight = nTileHeight;}
		int						GetTileWidth(){return m_nTileWidth;}
		int						GetTileHeight(){return m_nTileHeight;}
		const Rect&				GetRectOffset(){return m_rectOffset;}
		VOID					SetRectOffset(const Rect& r){m_rectOffset = r;}
		//
	public:
		//
		UIControl*		m_controlPtr;
	};

};

#endif
