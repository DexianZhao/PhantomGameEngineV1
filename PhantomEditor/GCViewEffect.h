#pragma once
#include "GCViewBase.h"
#include "IUnit3D.h"
#include "TrackAnimationCtrl.h"
#include "GameEffect.h"

class GCViewEffectEditor: public GCViewBase
{
public:
	GCViewEffectEditor(CPhantomGCDoc* docPtr);
	~GCViewEffectEditor(void);
	virtual	VOID				OnActive();		//被激活
	virtual	VOID				OnDeActive();	//失去激活
	virtual	VOID				CloseFile();
	virtual	BOOL				OpenFile(const wchar_t* szFile);	//打开文件,如果失败则会自动删除
	virtual	BOOL				SaveFile(BOOL bSaveAs);	//保存文件
	virtual void				OnFrameMove(const RenderParameter& r);	//每次更新
	virtual void				OnPushRenderObjects(const RenderParameter& r);	//渲染
	virtual	FileViewerType		GetViewerType(){return FileViewerType_Effect;}
	virtual	HICON				GetViewerIcon();
	virtual void				OnRendering(const RenderParameter& r);	//渲染
	virtual void		OnRenderingEnd(const RenderParameter& r);	//渲染
	//
	VOID				RebuildTrees();
	enum{
		EFFECT_NONE	=	0,
		EFFECT_MAIN,
		EFFECT_NODE,
	};
	void				OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp);
	VOID				OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem);	//更新选择树形控件
	virtual	void		OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI);
	virtual	void		OnCommandUI(INT nMenuID);
	VOID				InitNodeProp();
	//
	// Create
	PropertyFloatPtr	fDelayTime;
	PropertyFloatPtr	fCreateTime;
	PropertyFloatPtr	fLifeTime;
	PropertyPtr			nNumNode;
	PropertyPtr			eCreate;//ECreate
	// StartMode
	PropertyPtr			eStart;//EStart
	PropertyPtr			bScalarRegular;
	PropertyFloatPtr	fScalarMax;
	PropertyFloatPtr	fScalarMin;
	PropertyPtr			bAngleRegular;
	PropertyFloatPtr	fAngleMax;
	PropertyFloatPtr	fAngleMin;
	PropertyPtr			bInheritMatrix;
	// MoveMode
	PropertyPtr			bMoveMode;
	PropertyPtr				eMove;//EMove
	PropertyFloatPtr			fMoveSpeedMax;
	PropertyFloatPtr			fMoveSpeedMin;
	PropertyPtr			bExpansionMatrix;
	// Pos
	PropertyPtr			bStartPos;
	PropertyFloatPtr	sStartPosX,sStartPosY,sStartPosZ;
	PropertyPtr			bEndPos;
	PropertyFloatPtr	sEndPosX, sEndPosY,sEndPosZ;
	PropertyPtr			bBoundCheck;
	PropertyPtr			bInheritEndPos;
	PropertyPtr			bPosRevers;
	// Straight
	PropertyPtr			bStraight;
	PropertyFloatPtr			fStraightMoveSpeed;
	PropertyFloatPtr			fStraightAccelSpeed;
	PropertyFloatPtr			sStraightDirX,sStraightDirY,sStraightDirZ;
	// Rotate
	PropertyPtr			bRotate;
	PropertyFloatPtr			fRotateRadius;
	PropertyFloatPtr			fRotateAngleSpeed;
	PropertyFloatPtr			sRotateDirX,sRotateDirY,sRotateDirZ;
	// Gravity
	PropertyPtr			bGravity;
	PropertyFloatPtr			fGravityStart;
	PropertyFloatPtr			fGravitySpeed;
	PropertyFloatPtr			sGravityDirX,sGravityDirY,sGravityDirZ;
	// Following
	PropertyPtr			bFollowing;
	PropertyFloatPtr			fFollowingRangeStart;
	PropertyFloatPtr			fFollowingRangeAccel;
	// Dir
	PropertyPtr			bDirStart;
	PropertyFloatPtr			sDirStartX,sDirStartY,sDirStartZ;
	PropertyPtr			bDirRotate;
	PropertyFloatPtr			fDirRotateSpeedX;
	PropertyFloatPtr			fDirRotateSpeedY;
	PropertyFloatPtr			fDirRotateSpeedZ;
	PropertyPtr			bDirBillBoard;
	PropertyPtr			bDirEqualNodeDir;
	// DirChild
	PropertyPtr			bDirChildStart;
	PropertyFloatPtr			sDirChildStartX,sDirChildStartY,sDirChildStartZ;
	PropertyPtr			bDirChildRotate;
	PropertyFloatPtr			fDirChildRotateSpeedX;
	PropertyFloatPtr			fDirChildRotateSpeedY;
	PropertyFloatPtr			fDirChildRotateSpeedZ;
	PropertyPtr			bDirChildRandomRotateX;
	PropertyPtr			bDirChildRandomRotateY;
	PropertyPtr			bDirChildRandomRotateZ;
	PropertyPtr			bDirChildBillBoard;
	PropertyPtr			bDirChildEqualNodeDir;
	// Scale
	PropertyPtr			bScale;
	PropertyPtr			bScaleBaseRegular;
	PropertyFloatPtr			fScaleBaseRegularStart;
	PropertyFloatPtr			fScaleBaseRegularSpeed;
	PropertyPtr			bScaleBaseSin;
	PropertyFloatPtr			fScaleBaseSinStart;
	PropertyFloatPtr			fScaleBaseSinEnd;
	PropertyFloatPtr			fScaleBaseSinPeriodicTime;
	PropertyPtr			bScaleAdditionX;
	PropertyPtr			bScaleAdditionY;
	PropertyPtr			bScaleAdditionZ;
	PropertyPtr			bScaleAdditionRegular;
	PropertyFloatPtr			fScaleAdditionRegularStart;
	PropertyFloatPtr			fScaleAdditionRegularSpeed;
	PropertyPtr			bScaleAdditionSin;
	PropertyFloatPtr			fScaleAdditionSinStart;
	PropertyFloatPtr			fScaleAdditionSinEnd;
	PropertyFloatPtr			fScaleAdditionSinPeriodicTime;
	// Alpha
	PropertyPtr				eAlphaBlend;//EBlend
	PropertyPtr			bAlphaZBuffer;
	PropertyPtr			bAlphaRegular;
	PropertyFloatPtr			fAlphaRegularStart;
	PropertyFloatPtr			fAlphaRegularEnd;
	PropertyPtr			bAlphaSin;
	PropertyFloatPtr			fAlphaSinStart;
	PropertyFloatPtr			fAlphaSinEnd;
	PropertyFloatPtr			fAlphaSinPeriodicTime;
	PropertyPtr			bAlphaColor;
	PropertyColorPtr			sAlphaColor;
	// AtomicInst
	PropertyPtr			bAtomicInstAnimation;
	PropertyPtr			bAtomicInstTwoSide;
	PropertyPtr			bAtomicInstAniTime;
	PropertyFloatPtr			fAtomicInstAniTime;
	PropertyFilePtr		textureFile;
	//
	VOID				RefreshEnable();
	VOID				BuildAll();
	VOID				BuildAll(HTREEITEM hItem, CEffectNode* n);
public:
	GCPropertyBase		m_nodeProp;
	//

public:
	CEffect*			m_effectPtr;
	EffectID			m_effectPlay;
};

