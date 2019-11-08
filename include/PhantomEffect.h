//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   Effect.h
软件名称   :   幻影游戏引擎

设计编写者 :   赵德贤(英文:ZhaoDexian)
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __PHANTOM_EFFECT_H__
#define __PHANTOM_EFFECT_H__


#pragma once
#include "PhantomMath.h"
#include "PhantomManager.h"
namespace Phantom{
struct SoundIdx
{
	SoundIdx() { Release(); }
	int	m_nUserMark;
	int m_n3didx;
	void Release() {  m_nUserMark = m_n3didx = -1; }
};

#pragma pack(push,1)

struct EffectLine
{
	char		m_szLineName[32];
	char		m_szTextureName[32];
	BOOL		m_bLighting;
	float		m_fWidthMax;
	float		m_fWidthMin;
	int			m_nNumSegmentLarge;
	int			m_nNumSegmentSmall;
	float		m_fTensionLargeMax;
	float		m_fTensionLargeMin;
	float		m_fTensionSmallMax;
	float		m_fTensionSmallMin;
public:
	void Open(char* szFileName);
	void Save(char* szFileName);
	void Default();
	EffectLine()	{}
};

#pragma pack(pop)

struct EPlane
{
	union
	{
		struct
		{
			float a, b, c, d;
		};
		float v[4];
	};

public:
	float MultiPlanef(float3& sPos);
	bool MultiPlane(float3& sPos);
	float3 SetPlane(float3& sStartPos, float3& sEndPos);
};

#pragma pack(push, 1)
struct EffectUV
{
	float	u, v;
public:
	EffectUV()	{}
	inline EffectUV(float _u, float _v)		{ u = _u; v = _v; }
	inline void Set(float _u, float _v)	{ u = _u; v = _v; }
};

struct SProperty
{
	char			szNodeName[32];
	char			szAtomicName[32];
	char			szNextEffectName[32];
	char			szSoundName[32];
	BOOL			bSoundRepeat;
	BOOL			bAttachMode;
	BOOL			bAttachEndMode;
	// Create
	float			fDelayTime;
	float			fCreateTime;
	float			fLifeTime;
	int				nNumNode;
	int				eCreate;//ECreate
	// StartMode
	int				eStart;//EStart
	BOOL			bScalarRegular;
	float			fScalarMax;
	float			fScalarMin;
	BOOL			bAngleRegular;
	float			fAngleMax;
	float			fAngleMin;
	BOOL			bInheritMatrix;
	// MoveMode
	BOOL			bMoveMode;
	int				eMove;//EMove
	float			fMoveSpeedMax;
	float			fMoveSpeedMin;
	BOOL			bExpansionMatrix;
	// Pos
	BOOL			bStartPos;
	float3			sStartPos;
	BOOL			bEndPos;
	float3			sEndPos;
	BOOL			bBoundCheck;
	BOOL			bInheritEndPos;
	BOOL			bPosRevers;
	// Straight
	BOOL			bStraight;
	float			fStraightMoveSpeed;
	float			fStraightAccelSpeed;
	float3			sStraightDir;
	// Rotate
	BOOL			bRotate;
	float			fRotateRadius;
	float			fRotateAngleSpeed;
	float3			sRotateDir;
	// Gravity
	BOOL			bGravity;
	float			fGravityStart;
	float			fGravitySpeed;
	float3			sGravityDir;
	// Following
	BOOL			bFollowing;
	float			fFollowingRangeStart;
	float			fFollowingRangeAccel;
	// Dir
	BOOL			bDirStart;
	float3			sDirStart;
	BOOL			bDirRotate;
	float			fDirRotateSpeedX;
	float			fDirRotateSpeedY;
	float			fDirRotateSpeedZ;
	BOOL			bDirBillBoard;
	BOOL			bDirEqualNodeDir;
	// DirChild
	BOOL			bDirChildStart;
	float3			sDirChildStart;
	BOOL			bDirChildRotate;
	float			fDirChildRotateSpeedX;
	float			fDirChildRotateSpeedY;
	float			fDirChildRotateSpeedZ;
	BOOL			bDirChildRandomRotateX;
	BOOL			bDirChildRandomRotateY;
	BOOL			bDirChildRandomRotateZ;
	BOOL			bDirChildBillBoard;
	BOOL			bDirChildEqualNodeDir;
	// Scale
	BOOL			bScale;
	BOOL			bScaleBaseRegular;
	float			fScaleBaseRegularStart;
	float			fScaleBaseRegularSpeed;
	BOOL			bScaleBaseSin;
	float			fScaleBaseSinStart;
	float			fScaleBaseSinEnd;
	float			fScaleBaseSinPeriodicTime;
	BOOL			bScaleAdditionX;
	BOOL			bScaleAdditionY;
	BOOL			bScaleAdditionZ;
	BOOL			bScaleAdditionRegular;
	float			fScaleAdditionRegularStart;
	float			fScaleAdditionRegularSpeed;
	BOOL			bScaleAdditionSin;
	float			fScaleAdditionSinStart;
	float			fScaleAdditionSinEnd;
	float			fScaleAdditionSinPeriodicTime;
	// Alpha
	int				eAlphaBlend;//EBlend
	BOOL			bAlphaZBuffer;
	BOOL			bAlphaRegular;
	float			fAlphaRegularStart;
	float			fAlphaRegularEnd;
	BOOL			bAlphaSin;
	float			fAlphaSinStart;
	float			fAlphaSinEnd;
	float			fAlphaSinPeriodicTime;
	BOOL			bAlphaColor;
	float3			sAlphaColor;
	// AtomicInst
	BOOL			bAtomicInstAnimation;
	BOOL			bAtomicInstBmxBlend;
	BOOL			bAtomicInstTwoSide;
	BOOL			bAtomicInstAniTime;
	float			fAtomicInstAniTime;
	// DynamicLight
	BOOL			bDynamicLight;
	float			fDynamicLightRange;
	float3			sDynamicLightColor;
	// Vibration
	BOOL			bVibration;
	float			fVibrationPower;
	float			fVibrationRange;
	float			fVibrationTime;
public:
	void Default();
	SProperty();
};
#pragma pack(pop)

class AotmicData
{
public:
	AotmicData();
	virtual ~AotmicData(){}
	float3*				m_pArrayVer;
	EffectUV*			m_pArrayUV;
	color4*				m_pArrayColor;
	matrix4x4			m_sMat;
	matrix4x4			m_sMatPrev;
	matrix4x4			m_sMatStart;
	float3				m_sPos;
	float3				m_sExpansionPos;
	float3				m_sExpansionDir;
	float				m_fExpansionSpeed;
	matrix4x4			m_sMatExpansion;
	float				m_fPersentage;
	float				m_fLivedTime;
	float				m_fAlpha;
	float3				m_sScale;
	float3				m_sRotate;
	int					m_nNowTexture;
	float				m_fNowFrame;
};

#define	MAX_MULTITEX	4
class EffectMeshFrame
{
public:
	EffectMeshFrame();
	void Release();
	~EffectMeshFrame();
public:
	int		m_nFace;
	int		m_nVert;
	float *	m_pVert;
	int *	m_pVertIndex;
	float *	m_pFaceNorm;
	float *	m_pVertNorm;
	int		m_nMultiTex;
	int *	m_pTexID[MAX_MULTITEX];
	float *	m_pUV[MAX_MULTITEX];
};

class EffectMesh  
{
public:
	void	EnableMipmap(BOOL bEnable);
	BOOL	Load(const char* szFile, const char* szTexturePath);

	EffectMesh();
	virtual ~EffectMesh();

	int m_nType, m_nBlendMode;
	float m_fRangeMin[3], m_fRangeMax[3], m_fRangeMid[3], m_fRadius;

	int m_nFrame;
	EffectMeshFrame *m_pFrame;
	BOOL m_bUseTile, m_bAutoHide, m_bCollideTest, m_bZBuffer, m_bGroundStruct, m_bVertLight;
	float m_fAnimSpeed, m_fHideRange;
	std::vector<std::string>	m_textureFiles;
	std::vector<Texture*>		m_textures;
	std::string m_strFileName;
	std::string m_strSound;
	SoundIdx snd_idx;
	//
	VOID	Save(const char* szFile);
	void	SaveMesh(CFileHelperW&ar);
	void	SaveMorph(CFileHelperW&ar);
protected:
	BOOL ParseScript();
	void LoadMorph(CFileHelperR &ar);
	void LoadMesh(CFileHelperR&ar);
};

class EffectNode;

class AtomicBase
{
public:
	virtual const char* GetFileName()										= 0;
	virtual void Release()											= 0;
	virtual void Init(char* szFileName)								= 0;
	virtual void Update(EffectNode* pNode)								= 0;
	virtual void DestroyAtomic(AotmicData* pAtomic)					= 0;
	virtual AotmicData* CreateAtomic(SProperty* pPro)					= 0;
	AtomicBase()	{}
	virtual ~AtomicBase()	{}

	void _SetColor(float3 rgb, float a, BOOL bColor);
	void _SetBlend(int nBlendType);
	FileName		m_textureFile;
	virtual void Draw(CDynamicArray<AotmicData*>& vAtomic, EffectNode* node){}
};


class AtomicLine : public AtomicBase
{
public:
	EffectLine	m_line;
	Texture*	m_pTex;
	float3*		m_pArrayVer;
public:
	inline void _SetLightingAtomic(AotmicData *pAtomic, SProperty* pPro);
	virtual void DestroyAtomic(AotmicData* pAtomic);
	virtual const char* GetFileName();
	virtual void Draw(CDynamicArray<AotmicData*>& vAtomic, EffectNode* node);
	virtual AotmicData* CreateAtomic(SProperty* pPro);
	virtual void Update(EffectNode* pNode);
	virtual void Release();
	virtual void Init(char* szFileName);
	AtomicLine(const char* szTexture)	{m_textureFile = szTexture;m_pTex = 0;}
	~AtomicLine(){safe_release(m_pTex);}
};

class AtomicPlane : public AtomicBase
{
public:
	Texture*		m_pTex;
public:
	virtual void DestroyAtomic(AotmicData* pAtomic);
	virtual const char* GetFileName();
	virtual void Draw(CDynamicArray<AotmicData*>& vAtomic, EffectNode* node);
	virtual AotmicData* CreateAtomic(SProperty* pPro);
	virtual void Update(EffectNode* pNode);
	virtual void Release();
	virtual void Init(char* szFileName);
	AtomicPlane(const char* szTexture)	{m_textureFile = szTexture;m_pTex = 0;}
	~AtomicPlane(){safe_release(m_pTex);}
};

class AtomicMesh : public AtomicBase
{
public:
	virtual void DestroyAtomic(AotmicData* pAtomic);
	virtual AotmicData* CreateAtomic(SProperty* pPro);
	virtual const char* GetFileName();
	virtual void Draw(CDynamicArray<AotmicData*>& vAtomic, EffectNode* node);
	virtual void Update(EffectNode* pNode);
	virtual void Release();
	virtual void Init(char* szFileName);
	//
	AtomicMesh(const char* szTexture)	{m_textureFile = szTexture;m_pTex = 0;}
	virtual ~AtomicMesh(){if(m_pArrayVer)delete [] m_pArrayVer; safe_release(m_pTex);}
	EffectMesh*		m_meshPtr;
	EffectUV*		m_pArrayUV;
	float3*			m_pArrayVer;
	int				m_nNumVer;
	Texture*		m_pTex;
};

class SkillBase;
class EffectNode : public AotmicData
{
public:
	SProperty			m_pPro;
	CDynamicArray<EffectNode*>		m_vNodeInst;
	CDynamicArray<AotmicData*>		m_vNodeActive;

	AtomicBase*		m_pAtomicInst;

	SoundIdx			m_eff_idx;
	int					m_nID;
	float*				m_pLight;
	EPlane				m_sPlane;
	SkillBase*			m_pSkill;
	bool				m_bActive;
	bool				m_bLife;
	bool				m_bNoCreate;
	bool				m_bNoKill;

	float				m_fDelayTime;
	float				m_fCreateTime;
	float				m_fFollowingRange;
	DWORD				m_dwFollowingEndID;
	DWORD				m_dwAttachEndID;
	float3				m_sPosStart;
	float3				m_sPosEnd;
	float3				m_sPosWorld;
	float3				m_sPosWorldPrev;
	float3				m_sDir;
	EffectNode*		m_pNodeParent;
	DWORD				m_dwNodeIndex;
	int					m_nBlendType, m_nScaleFunc, m_nAlphaFunc;
	DWORD				m_dwProp;
	float				m_fMoveX, m_fMoveY, m_fRotAng;
	float				m_fScaleArg[4], m_fAlphaArg[4];
	float				m_fFrameSpeed;
	EffectNode*		FindNode(int nID);
public:
	void			UpdateVibration();
	void			UpdateDynamicLight();
	void			DecodNextEffect();
	void			StopSound();
	void			UpdateSound();
	void			PlaySound();
	void			SetSoundName(char* szSound);
	void			SetNextEffectName(char* szNextEffect);
	virtual void	Update_Prev();
	void			UpdateCreate_At();
	void			UpdateKill_At();
	void			TransMatrix();
	void			UpdateTrans();
	void			UpdateCreate();
	void			UpdateKill();
	virtual	EffectNode* GetNodeInst(DWORD dwNodeIndex);
	void			ReleaseActive();
	void			ReleaseInst();
	void			Release();
	void			ReleaseProperty();
	void			InitProperty();
	void			InitAtomicInst(AtomicBase *pInst);
	void			Load(CFileHelperR& r);
	void			Save(CFileHelperW& w);
	void			Draw();
	void			Update();
	const char*		GetTextureFile(){return m_textureFile;}
	VOID			SetTextureFile(const char* s){m_textureFile = s;}
	FileName		m_textureFile;
	EffectNode();
	virtual ~EffectNode();

	void			_InitActive(EffectNode* pNode, matrix4x4& sMat);
	void			_InitActive(EffectNode* pNode, const float3& sStartPos, const float3& sEndPos);
	float3			_CreatePosStart(int nStartIndex, float& fRadianX, float& fRadianY);
	float3			_CreatePosMove(float3& sPosStart, float& fRadianX, float& fRadianY);
	float			_CreateMoveSpeed();
	matrix4x4		_CreatePosMatrix(float fRadianX, float fRadianY);

};


class Effect;
class EffectRender: public RenderBase
{
public:
	virtual	void							drawObject(const matrix4x4* matPtr = 0);
	virtual	void							drawObjectShadow(const matrix4x4* matPtr = 0){}
	virtual	char							IsAffectByShadowMap(){return 0;}			//是否支持自身阴影渲染
	virtual	void							drawToShadowMap(const matrix4x4* matPtr, Program* vsps, int nDrawState = RenderNode_SetState){}
	virtual	const matrix4x4&				GetWorldMatrix();
	//
	Effect*			m_ptr;
};


struct	EffectID
{
	int		wIndex;
	int		wMask;
	EffectID(){wIndex = -1; wMask = -1;}
	EffectID(int _index, int _mask){wIndex = _index; wMask = _mask;}
};

class Effect : public EffectNode
{
public:
	void							SetFollowingPos(DWORD dwUniqueID);
	void							SetFollowingPos(float3& sPos);
	void							SetAttachEndPos(float3 &sPos);
	void							SetAttachPos(DWORD dwUniqueID);
	void							SetAttachPos(matrix4x4& sMat);
	void							SetAttachPos(float3& sPos);
	void							Update_Prev();
	void							LoadFile(const char* szFileName);
	void							SaveFile(const char* szFileName);
	void							drawObject(const matrix4x4* matPtr = 0);
	//
	Effect();
	virtual ~Effect();
	EffectRender					m_render;
	int								m_nID;
	Name							m_effectName;
	FileName						m_fileName;
};


class EffectManager
{
public:
	void						Render();
	void						FrameMove();
	VOID						ClearEffect();
	EffectManager();
	virtual ~EffectManager();
	//
	int							IsAtomicInst(char* szFileName);
	AtomicBase*				GetAtomicInst(int nIndex);
	int							AddAtomicInst(char* szFileName, const char* szTextureFile);
	void						SetAttachEndPos(EffectID& id, float3& sPos);
	void						SetFollowingPos(EffectID& id, float3& sPos);
	void						SetAttachPos(EffectID& id, float3& sPos);
	void						SetAttachPos(EffectID& id, matrix4x4& sMat);
	//
	Effect*						GetEffectPtr(EffectID& id);
	Effect*						LoadEffect(const char* szEffectFile);//预载入一个特效
	EffectID					PlayEffect(const char* szEffectName, const float3& sStartPos, const float3& sEndPos);
	VOID						StopEffect(EffectID& id);
	//
	EffectMesh*					LoadMesh(const char* szMeshFile);	//自动载入模型文件
	inline	int					CreateNewID(){return (m_nCreateID++);}
protected:
	CDynamicArray<EffectMesh*>	m_meshs;
	CDynamicArray<Effect*>		m_effectLoads;
	CDynamicArray<Effect*>		m_effectRuntimes;
	std::vector<AtomicBase*>	m_vAtomicInst;
	int							m_nCreateID;
};

VOID							CreateEffectMgr();
EffectManager*					GetEffectMgr();
VOID							ReleaseEffectMgr();

};


#endif
