/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include <vector>
#include <string>

VOID	OnMfcScroll(HWND hWnd, INT scBar, INT nSBCode, INT nPos);

VOID	RenderRect(CDC* pDC, const CRect& rc);
VOID	CopyTextToClipboard(const char* szText);
const wchar_t*	AToW(const char* szText, wchar_t * buffer, int bufferLength);
const char*		WToA(const wchar_t* szText, char * buffer, int bufferLength);
inline	bool	wcmp(const wchar_t* t, const wchar_t* t2){return (wcsicmp(t, t2) == 0);}

const wchar_t*	GetImageFilter();
const wchar_t*	GetSoundFilter();
const wchar_t*	GetMusicFilter();
const wchar_t*	GetMeshFilter();
const wchar_t*	GetSceneObjectFilter();
const wchar_t*	GetPhantomFilters();
const wchar_t*	GetScene3DObjectFilter();

enum AlignType{
	AlignType_Left	=	0,
	AlignType_Top,
	AlignType_Right,
	AlignType_Bottom,
	AlignType_Width,
	AlignType_Height,
	AlignType_OffsetX,
	AlignType_OffsetY,
	AlignType_SpaceX,
	AlignType_SpaceY,
	AlignType_MatrixX,
	AlignType_MatrixY,
	AlignType_MinZ,
	AlignType_MaxZ,
	AlignType_SpaceZ,
	AlignType_OffsetZ,
	AlignType_CenterAlign,//中心点对齐
	AlignType_Length,
};

enum MovePointSide{
	MovePointSide_Left	=	0,
	MovePointSide_Right,
	MovePointSide_Top,
	MovePointSide_Bottom,
	MovePointSide_LeftTop,
	MovePointSide_LeftBottom,
	MovePointSide_RightTop,
	MovePointSide_RightBottom,
	MovePointSide_Center,
	MovePointSide_Rotation,
	MovePointSide_Body,
	MovePointSide_Count,
};

enum MouseMode{
	MouseMode_None	=	0,
	MouseMode_InsertControl,
	MouseMode_MoveControl,
	MouseMode_TabIndex,
	MouseMode_SelectRect,
	MouseMode_SizeControl,
	MouseMode_MoveCopy,
	MouseMode_Unit2DCenterPos,
	MouseMode_SetFloor,
	MouseMode_InsertUnit2D,
	MouseMode_MapRect,
	//
	MouseMode_User,
};

struct	TypeOption
{
	wchar_t		text[128];
	int			index;
	TypeOption(){text[0] = 0; index = 0;}
	TypeOption(const wchar_t* t, int n){wcscpy(text, t); index = n;}
};

class	CTypeOption
{
public:
	CTypeOption();
	~CTypeOption(){}
	VOID				AddOption(const wchar_t* szText, INT nType, BOOL bDefault = FALSE);
	INT					operator[](const wchar_t* szText);
	const wchar_t*		operator[](INT nIndex);
	//
	VOID				RemoveAll();
public:
	TypeOption					m_default;
	std::vector<TypeOption>		m_options;
};

BOOL	GetConfigBool(const char* szName, BOOL bDefault = FALSE);
VOID	SetConfigBool(const char* szName, BOOL b);
INT		GetConfigInt(const char* szName, INT nDefault = 0);
VOID	SetConfigInt(const char* szName, INT b);
double	GetConfigDouble(const char* szName, double dDefault = 0);
VOID	SetConfigDouble(const char* szName, double b);
BOOL	GetConfigText(const char* szName, std::string& ret, const char* szDefault = "");
VOID	SetConfigText(const char* szName, const char* szRet);
//
VOID	SaveConfigs(const char* szPath);
VOID	LoadConfigs(const char* szPath);
VOID	DestroyConfigs();

inline	int		GetMinWindowPos(){return -8192;}
inline	int		GetMaxWindowPos(){return 8192;}
inline	int		GetWindowPosPage(){return 5;}

VOID			InitTypeOptions();

extern	CTypeOption		g_typeTemplate;
extern	CTypeOption		g_typeFilter;
extern	CTypeOption		g_typeCull;
extern	CTypeOption		g_typeAlpha;
extern	CTypeOption		g_typeBlend;
extern	CTypeOption		g_typeLodLevel;
extern	CTypeOption		g_typeDialogAlignLR;
extern	CTypeOption		g_typeDialogAlignTB;
extern	CTypeOption		g_typeToolTipAlign;
extern	CTypeOption		g_typeControlLayer;
extern	CTypeOption		g_typeTextAlignLR;
extern	CTypeOption		g_typeTextAlignTB;
extern	CTypeOption		g_typeProgress;
extern	CTypeOption		g_typeColorOP;
extern	CTypeOption		g_typeControlGroup;
extern	CTypeOption		g_typeAlignModeW;
extern	CTypeOption		g_typeAlignModeH;
extern	CTypeOption		g_typeControlButtons;
extern	CTypeOption		g_type2DUnitAngle;
extern	CTypeOption		g_type2DUnitSide;
extern	CTypeOption		g_type2DPickEnable;
extern	CTypeOption		g_typeDefault;
extern	CTypeOption		g_typeMouseButton;
extern	CTypeOption		g_typeCallIndex;
extern	CTypeOption		g_typeLightType;
extern	CTypeOption		g_typeInputKey;
extern	CTypeOption		g_typeUnitActionType;
extern	CTypeOption		g_typeGroupType;
extern	CTypeOption		g_typeGroupDiffuse;
//
extern	CTypeOption		g_typeParticleType;
extern	CTypeOption		g_typeEmissionType;
extern	CTypeOption		g_typeShaderType;
extern	CTypeOption		g_typeShadowMap;
extern	CTypeOption		g_typeCenterObj;
extern	CTypeOption		g_typeCascadeMap;
extern	CTypeOption		g_typeSoftPCF;
extern	CTypeOption		g_typeSoftBlur;
extern	CTypeOption		g_typeViewScale;
extern	CTypeOption		g_typeBox2DBodyType;
extern	CTypeOption		g_typeBox2DShapeType;
extern	CTypeOption		g_typeBox2DJointType;
//
extern	CTypeOption		g_typeEffectCreate;
extern	CTypeOption		g_typeEffectStart;
extern	CTypeOption		g_typeEffectMove;
extern	CTypeOption		g_typeEffectAlpha;
extern	CTypeOption		g_typeTableEvents;
extern	CTypeOption		g_typeTableRoles;
extern	CTypeOption		g_typeFogMode;
extern	CTypeOption		g_typeUnitAngleType;
//

VOID	AddTool(CToolTipCtrl& toolTip, CWnd& wnd, const wchar_t* szText);
VOID	InitTool(CToolTipCtrl& toolTip, CWnd* dlg);

__interface	ViewerWindow
{
	CWnd*			GetWindowPtr();
	VOID			OnMouseWheel(int delta);
	VOID			OnKeyDown(int key);
	VOID			OnKeyUp(int key);
	VOID			OnViewSize(CRect* rc);
};
