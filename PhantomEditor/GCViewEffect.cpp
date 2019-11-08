#include "StdAfx.h"
#include "GCViewEffect.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"

GCViewEffectEditor::GCViewEffectEditor(CPhantomGCDoc* docPtr)
	:GCViewBase(docPtr, Language(L"特效编辑"), IDB_UI_VIEW_BMP)
{
	this->m_effectPtr	=	0;
}
VOID		GCViewEffectEditor::CloseFile(){
	GetEffectMgr()->StopEffect(m_effectPlay);
	safe_delete(m_effectPtr);
}
GCViewEffectEditor::~GCViewEffectEditor(void)
{
	CloseFile();
}

HICON		GCViewEffectEditor::GetViewerIcon()
{
	static HICON	hIcon	=	AfxGetApp()->LoadIcon(IDI_TEXTURE_FILE_ICON);
	return hIcon;
}

#define	SET_PROP_F(v) if(pProp == v){p.v = GetPropFloat(v);}
#define	SET_PROP_F2(v,v2) if(pProp == v){p.v2 = GetPropFloat(v);}
#define	SET_PROP_B(v) if(pProp == v){p.v = GetPropBool(v);}
#define	SET_PROP_I(v) if(pProp == v){p.v = GetPropInt(v);}
#define	SET_PROP_O(v,o) if(pProp == v){p.v = GetPropOption(v, o);}
void		GCViewEffectEditor::OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp)
{
	__super::OnPropertyChanged(wnd, pProp);

	std::map<HTREEITEM,TreeItemType*> selectTypes;
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			selectTypes[hItem]	=	type;
		}
	}
	BOOL bRefresh = false;
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == EFFECT_NODE)
		{
			
			CEffectNode* node = this->m_effectPtr->FindNode(type->p1);
			SProperty& p = node->m_pPro;
			SetModifyed(true);
			//
			if(pProp == NULL){
				break;
			}
			SET_PROP_F(fDelayTime);
			SET_PROP_F(fCreateTime);
			SET_PROP_F(fLifeTime);
			SET_PROP_F(fScalarMax);
			SET_PROP_F(fScalarMin);
			SET_PROP_F(fAngleMax);
			SET_PROP_F(fAngleMin);
			SET_PROP_F(fMoveSpeedMax);
			SET_PROP_F(fMoveSpeedMin);
			SET_PROP_F2(sStartPosX, sStartPos.x);
			SET_PROP_F2(sStartPosY, sStartPos.y);
			SET_PROP_F2(sStartPosZ, sStartPos.z);
			SET_PROP_F2(sEndPosX, sEndPos.x);
			SET_PROP_F2(sEndPosY, sEndPos.y);
			SET_PROP_F2(sEndPosZ, sEndPos.z);
			SET_PROP_F(fStraightMoveSpeed);
			SET_PROP_F(fStraightAccelSpeed);
			SET_PROP_F2(sStraightDirX,sStraightDir.x);
			SET_PROP_F2(sStraightDirY,sStraightDir.y);
			SET_PROP_F2(sStraightDirZ,sStraightDir.z);
			SET_PROP_F(fRotateRadius);
			SET_PROP_F(fRotateAngleSpeed);
			SET_PROP_F2(sRotateDirX, sRotateDir.x);
			SET_PROP_F2(sRotateDirY, sRotateDir.y);
			SET_PROP_F2(sRotateDirZ, sRotateDir.z);
			SET_PROP_F(fGravityStart);
			SET_PROP_F(fGravitySpeed);
			SET_PROP_F2(sGravityDirX, sGravityDir.x);
			SET_PROP_F2(sGravityDirY, sGravityDir.y);
			SET_PROP_F2(sGravityDirZ, sGravityDir.z);
			SET_PROP_F(fFollowingRangeStart);
			SET_PROP_F(fFollowingRangeAccel);
			SET_PROP_F2(sDirStartX, sDirStart.x);
			SET_PROP_F2(sDirStartY, sDirStart.y);
			SET_PROP_F2(sDirStartZ, sDirStart.z);
			SET_PROP_F(fDirRotateSpeedX);
			SET_PROP_F(fDirRotateSpeedY);
			SET_PROP_F(fDirRotateSpeedZ);
			SET_PROP_F2(sDirChildStartX, sDirChildStart.x);
			SET_PROP_F2(sDirChildStartY, sDirChildStart.y);
			SET_PROP_F2(sDirChildStartZ, sDirChildStart.z);
			SET_PROP_F(fDirChildRotateSpeedX);
			SET_PROP_F(fDirChildRotateSpeedY);
			SET_PROP_F(fDirChildRotateSpeedZ);
			SET_PROP_F(fScaleBaseRegularStart);
			SET_PROP_F(fScaleBaseRegularSpeed);
			SET_PROP_F(fScaleBaseSinStart);
			SET_PROP_F(fScaleBaseSinEnd);
			SET_PROP_F(fScaleBaseSinPeriodicTime);
			SET_PROP_F(fScaleAdditionRegularStart);
			SET_PROP_F(fScaleAdditionRegularSpeed);
			SET_PROP_F(fScaleAdditionSinStart);
			SET_PROP_F(fScaleAdditionSinEnd);
			SET_PROP_F(fScaleAdditionSinPeriodicTime);
			SET_PROP_F(fAlphaRegularStart);
			SET_PROP_F(fAlphaRegularEnd);
			SET_PROP_F(fAlphaSinStart);
			SET_PROP_F(fAlphaSinEnd);
			SET_PROP_F(fAlphaSinPeriodicTime);
			SET_PROP_F(fAtomicInstAniTime);
			//
			SET_PROP_B(bScalarRegular);
			SET_PROP_B(bAngleRegular);
			SET_PROP_B(bInheritMatrix);
			SET_PROP_B(bMoveMode);
			SET_PROP_B(bExpansionMatrix);
			SET_PROP_B(bStartPos);
			SET_PROP_B(bEndPos);
			SET_PROP_B(bBoundCheck);
			SET_PROP_B(bInheritEndPos);
			SET_PROP_B(bPosRevers);
			SET_PROP_B(bStraight);
			SET_PROP_B(bRotate);
			SET_PROP_B(bGravity);
			SET_PROP_B(bFollowing);
			SET_PROP_B(bDirStart);
			SET_PROP_B(bDirRotate);
			SET_PROP_B(bDirBillBoard);
			SET_PROP_B(bDirEqualNodeDir);
			SET_PROP_B(bDirChildStart);
			SET_PROP_B(bDirChildRotate);
			SET_PROP_B(bDirChildRandomRotateX);
			SET_PROP_B(bDirChildRandomRotateY);
			SET_PROP_B(bDirChildRandomRotateZ);
			SET_PROP_B(bDirChildBillBoard);
			SET_PROP_B(bDirChildEqualNodeDir);
			SET_PROP_B(bScale);
			SET_PROP_B(bScaleBaseRegular);
			SET_PROP_B(bScaleBaseSin);
			SET_PROP_B(bScaleAdditionX);
			SET_PROP_B(bScaleAdditionY);
			SET_PROP_B(bScaleAdditionZ);
			SET_PROP_B(bScaleAdditionRegular);
			SET_PROP_B(bScaleAdditionSin);
			SET_PROP_B(bAlphaZBuffer);
			SET_PROP_B(bAlphaRegular);
			SET_PROP_B(bAlphaSin);
			SET_PROP_B(bAlphaColor);
			SET_PROP_B(bAtomicInstAnimation);
			SET_PROP_B(bAtomicInstTwoSide);
			SET_PROP_B(bAtomicInstAniTime);
			SET_PROP_O(eCreate, g_typeEffectCreate);
			SET_PROP_O(eStart, g_typeEffectStart);
			SET_PROP_O(eMove, g_typeEffectMove);
			SET_PROP_O(eAlphaBlend, g_typeEffectAlpha);
			SET_PROP_I(nNumNode);
			if(pProp == sAlphaColor)
			{
				color4 c;
				c.from(GetPropColor(sAlphaColor), 1);
				p.sAlphaColor.setxyz(c.r, c.g, c.b);
			}
			if(pProp == textureFile)
			{
				std::wstring str;
				GetPropFile(textureFile, str);
				A a(str.c_str());
				char szFile[256], szExt[128];
				_splitpath(a.buf, 0, 0, szFile, szExt);
				strcat(szFile, szExt);
				node->SetTextureFile(szFile);
			}
			bRefresh = true;
		}
	}
	if(bRefresh)
	{
		this->RefreshEnable();
		GetEffectMgr()->PlayEffect(m_effectPtr->m_effectName, float3(0, 0, 0), float3(2000, -2000, 0));
	}
}

VOID		GCViewEffectEditor::OnChangeTreeSelect(CMTreeCtrl* treePtr, HTREEITEM hAddItem)	//更新选择树形控件
{
	if(treePtr == &m_treeView)
	{
		TreeItemType* type	=	this->GetSelectedItem();
		if(!type)
		{
			SetCurrentProperties(&m_viewerProps, L"文件属性", NULL);
			return;
		}
		switch(type->type)
		{
		case EFFECT_MAIN:
			{
				SetCurrentProperties(NULL, NULL, NULL);
				GetEffectMgr()->PlayEffect(m_effectPtr->m_effectName, float3(0, 0, 0), float3(2000, -2000, 0));
			}
			break;
		case EFFECT_NODE:
			{
				SetCurrentProperties(&this->m_nodeProp, L"粒子属性", NULL);
				m_nodeProp.EnableRender(false);
				CEffectNode* node = this->m_effectPtr->FindNode(type->p1);
				if(!node){
					m_nodeProp.EnableWindow(false);
					m_nodeProp.EnableRender(true);
					return;
				}
				SProperty& p = node->m_pPro;
				//
				SetPropFloat(fDelayTime, p.fDelayTime);
				SetPropFloat(fCreateTime, p.fCreateTime);
				SetPropFloat(fLifeTime, p.fLifeTime);
				SetPropFloat(fScalarMax, p.fScalarMax);
				SetPropFloat(fScalarMin, p.fScalarMin);
				SetPropFloat(fAngleMax, p.fAngleMax);
				SetPropFloat(fAngleMin, p.fAngleMin);
				SetPropFloat(fMoveSpeedMax, p.fMoveSpeedMax);
				SetPropFloat(fMoveSpeedMin, p.fMoveSpeedMin);
				SetPropFloat(sStartPosX, p.sStartPos.x);
				SetPropFloat(sStartPosY, p.sStartPos.y);
				SetPropFloat(sStartPosZ, p.sStartPos.z);
				SetPropFloat(sEndPosX, p.sEndPos.x);
				SetPropFloat(sEndPosY, p.sEndPos.y);
				SetPropFloat(sEndPosZ, p.sEndPos.z);
				SetPropFloat(fStraightMoveSpeed, p.fStraightMoveSpeed);
				SetPropFloat(fStraightAccelSpeed, p.fStraightAccelSpeed);
				SetPropFloat(sStraightDirX, p.sStraightDir.x);
				SetPropFloat(sStraightDirY, p.sStraightDir.y);
				SetPropFloat(sStraightDirZ, p.sStraightDir.z);
				SetPropFloat(fRotateRadius, p.fRotateRadius);
				SetPropFloat(fRotateAngleSpeed, p.fRotateAngleSpeed);
				SetPropFloat(sRotateDirX, p.sRotateDir.x);
				SetPropFloat(sRotateDirY, p.sRotateDir.y);
				SetPropFloat(sRotateDirZ, p.sRotateDir.z);
				//
				SetPropFloat(fGravityStart, p.fGravityStart);
				SetPropFloat(fGravitySpeed, p.fGravitySpeed);
				SetPropFloat(sGravityDirX, p.sGravityDir.x);
				SetPropFloat(sGravityDirY, p.sGravityDir.y);
				SetPropFloat(sGravityDirZ, p.sGravityDir.z);
				SetPropFloat(fFollowingRangeStart, p.fFollowingRangeStart);
				SetPropFloat(fFollowingRangeAccel, p.fFollowingRangeAccel);
				SetPropFloat(sDirStartX, p.sDirStart.x);
				SetPropFloat(sDirStartY, p.sDirStart.y);
				SetPropFloat(sDirStartZ, p.sDirStart.z);
				//
				SetPropFloat(fDirRotateSpeedX, p.fDirRotateSpeedX);
				SetPropFloat(fDirRotateSpeedY, p.fDirRotateSpeedY);
				SetPropFloat(fDirRotateSpeedZ, p.fDirRotateSpeedZ);
				SetPropFloat(sDirChildStartX, p.sDirChildStart.x);
				SetPropFloat(sDirChildStartY, p.sDirChildStart.y);
				SetPropFloat(sDirChildStartZ, p.sDirChildStart.z);
				SetPropFloat(fDirChildRotateSpeedX, p.fDirChildRotateSpeedX);
				SetPropFloat(fDirChildRotateSpeedY, p.fDirChildRotateSpeedY);
				SetPropFloat(fDirChildRotateSpeedZ, p.fDirChildRotateSpeedZ);
				SetPropFloat(fScaleBaseRegularStart, p.fScaleBaseRegularStart);
				SetPropFloat(fScaleBaseRegularSpeed, p.fScaleBaseRegularSpeed);
				SetPropFloat(fScaleBaseSinStart, p.fScaleBaseSinStart);
				SetPropFloat(fScaleBaseSinEnd, p.fScaleBaseSinEnd);
				//
				SetPropFloat(fScaleBaseSinPeriodicTime, p.fScaleBaseSinPeriodicTime);
				SetPropFloat(fScaleAdditionRegularStart, p.fScaleAdditionRegularStart);
				SetPropFloat(fScaleAdditionRegularSpeed, p.fScaleAdditionRegularSpeed);
				SetPropFloat(fScaleAdditionSinStart, p.fScaleAdditionSinStart);
				SetPropFloat(fScaleAdditionSinEnd, p.fScaleAdditionSinEnd);
				SetPropFloat(fScaleAdditionSinPeriodicTime, p.fScaleAdditionSinPeriodicTime);
				SetPropFloat(fAlphaRegularStart, p.fAlphaRegularStart);
				SetPropFloat(fAlphaRegularEnd, p.fAlphaRegularEnd);
				SetPropFloat(fAlphaSinStart, p.fAlphaSinStart);
				SetPropFloat(fAlphaSinEnd, p.fAlphaSinEnd);
				SetPropFloat(fAlphaSinPeriodicTime, p.fAlphaSinPeriodicTime);
				SetPropFloat(fAtomicInstAniTime, p.fAtomicInstAniTime);
				//
				SetPropBool(bScalarRegular, p.bScalarRegular);
				SetPropBool(bAngleRegular, p.bAngleRegular);
				SetPropBool(bInheritMatrix, p.bInheritMatrix);
				SetPropBool(bMoveMode, p.bMoveMode);
				SetPropBool(bExpansionMatrix, p.bExpansionMatrix);
				SetPropBool(bStartPos, p.bStartPos);
				SetPropBool(bEndPos, p.bEndPos);
				SetPropBool(bBoundCheck, p.bBoundCheck);
				SetPropBool(bInheritEndPos, p.bInheritEndPos);
				SetPropBool(bPosRevers, p.bPosRevers);
				SetPropBool(bStraight, p.bStraight);
				SetPropBool(bRotate, p.bRotate);
				SetPropBool(bGravity, p.bGravity);
				SetPropBool(bFollowing, p.bFollowing);
				SetPropBool(bDirStart, p.bDirStart);
				SetPropBool(bDirRotate, p.bDirRotate);
				SetPropBool(bDirBillBoard, p.bDirBillBoard);
				SetPropBool(bDirEqualNodeDir, p.bDirEqualNodeDir);
				//
				SetPropBool(bDirChildStart, p.bDirChildStart);
				SetPropBool(bDirChildRotate, p.bDirChildRotate);
				SetPropBool(bDirChildRandomRotateX, p.bDirChildRandomRotateX);
				SetPropBool(bDirChildRandomRotateY, p.bDirChildRandomRotateY);
				SetPropBool(bDirChildRandomRotateZ, p.bDirChildRandomRotateZ);
				SetPropBool(bDirChildBillBoard, p.bDirChildBillBoard);
				SetPropBool(bDirChildEqualNodeDir, p.bDirChildEqualNodeDir);
				SetPropBool(bScale, p.bScale);
				SetPropBool(bScaleBaseRegular, p.bScaleBaseRegular);
				SetPropBool(bScaleBaseSin, p.bScaleBaseSin);
				SetPropBool(bScaleAdditionX, p.bScaleAdditionX);
				SetPropBool(bScaleAdditionY, p.bScaleAdditionY);
				SetPropBool(bScaleAdditionZ, p.bScaleAdditionZ);
				SetPropBool(bScaleAdditionRegular, p.bScaleAdditionRegular);
				SetPropBool(bScaleAdditionSin, p.bScaleAdditionSin);
				SetPropBool(bAlphaZBuffer, p.bAlphaZBuffer);
				SetPropBool(bAlphaRegular, p.bAlphaRegular);
				SetPropBool(bAlphaSin, p.bAlphaSin);
				//
				SetPropBool(bAlphaColor, p.bAlphaColor);
				SetPropBool(bAtomicInstAnimation, p.bAtomicInstAnimation);
				SetPropBool(bAtomicInstTwoSide, p.bAtomicInstTwoSide);
				SetPropBool(bAtomicInstAniTime, p.bAtomicInstAniTime);
				//
				SetPropColor(sAlphaColor, RGB(p.sAlphaColor.x*255.0f, p.sAlphaColor.y*255.0f, p.sAlphaColor.z*255.0f));
				//
				SetPropOption(eCreate, g_typeEffectCreate, p.eCreate);
				SetPropOption(eAlphaBlend, g_typeEffectAlpha, p.eAlphaBlend);
				SetPropOption(eMove, g_typeEffectMove, p.eMove);
				SetPropOption(eStart, g_typeEffectStart, p.eStart);
				SetPropInt(nNumNode, p.nNumNode);
				SetPropFile(textureFile, node->GetTextureFile());
				RefreshEnable();
				//
				m_nodeProp.EnableWindow(TRUE);
				m_nodeProp.EnableRender(true);
			}
			break;
		default:
			SetCurrentProperties(NULL, NULL, NULL);
			break;
		}
		return;
	}
	SetCurrentProperties(NULL, NULL, NULL);
}

VOID		GCViewEffectEditor::RefreshEnable()
{
	BOOL bTemp = 0;
	bTemp = GetPropBool(bScalarRegular);
	fScalarMax->Enable(bTemp);
	fScalarMin->Enable(bTemp);
	bTemp = GetPropBool(bAngleRegular);
	fAngleMax->Enable(bTemp);
	fAngleMin->Enable(bTemp);
	// MoveMode
	bTemp = GetPropBool(bMoveMode);
	fMoveSpeedMax->Enable(bTemp);
	fMoveSpeedMin->Enable(bTemp);
	// MoveMode
	bTemp = GetPropBool(bStartPos);
	sStartPosX->Enable(bTemp);
	sStartPosY->Enable(bTemp);
	sStartPosZ->Enable(bTemp);
	// MoveMode
	bTemp = GetPropBool(bEndPos);
	sEndPosX->Enable(bTemp);
	sEndPosY->Enable(bTemp);
	sEndPosZ->Enable(bTemp);
	// Straight
	bTemp = GetPropBool(bStraight);
	fStraightMoveSpeed->Enable(bTemp);
	fStraightAccelSpeed->Enable(bTemp);
	sStraightDirX->Enable(bTemp);
	sStraightDirY->Enable(bTemp);
	sStraightDirZ->Enable(bTemp);
	// Rotate
	bTemp = GetPropBool(bRotate);
	fRotateRadius->Enable(bTemp);
	fRotateAngleSpeed->Enable(bTemp);
	sRotateDirX->Enable(bTemp);
	sRotateDirY->Enable(bTemp);
	sRotateDirZ->Enable(bTemp);
	// Gravity
	bTemp = GetPropBool(bGravity);
	fGravityStart->Enable(bTemp);
	fGravitySpeed->Enable(bTemp);
	sGravityDirX->Enable(bTemp);
	sGravityDirY->Enable(bTemp);
	sGravityDirZ->Enable(bTemp);
	// Following
	bTemp = GetPropBool(bFollowing);
	fFollowingRangeStart->Enable(bTemp);
	fFollowingRangeAccel->Enable(bTemp);
	// Dir
	bTemp = GetPropBool(bDirStart);
	sDirStartX->Enable(bTemp);
	sDirStartY->Enable(bTemp);
	sDirStartZ->Enable(bTemp);
	bTemp = GetPropBool(bDirRotate);
	fDirRotateSpeedX->Enable(bTemp);
	fDirRotateSpeedY->Enable(bTemp);
	fDirRotateSpeedZ->Enable(bTemp);
	bTemp = GetPropBool(bDirChildStart);
	sDirChildStartX->Enable(bTemp);
	sDirChildStartY->Enable(bTemp);
	sDirChildStartZ->Enable(bTemp);
	bTemp = GetPropBool(bDirChildRotate);
	fDirChildRotateSpeedX->Enable(bTemp);
	fDirChildRotateSpeedY->Enable(bTemp);
	fDirChildRotateSpeedZ->Enable(bTemp);

	bTemp = GetPropBool(bScaleBaseRegular);
	fScaleBaseRegularStart->Enable(bTemp);
	fScaleBaseRegularSpeed->Enable(bTemp);

	bTemp = GetPropBool(bScaleBaseSin);
	fScaleBaseSinStart->Enable(bTemp);
	fScaleBaseSinEnd->Enable(bTemp);
	fScaleBaseSinPeriodicTime->Enable(bTemp);

	bTemp = GetPropBool(bScaleAdditionRegular);
	fScaleAdditionRegularStart->Enable(bTemp);
	fScaleAdditionRegularSpeed->Enable(bTemp);
	bTemp = GetPropBool(bScaleAdditionSin);
	fScaleAdditionSinStart->Enable(bTemp);
	fScaleAdditionSinEnd->Enable(bTemp);
	fScaleAdditionSinPeriodicTime->Enable(bTemp);

	bTemp = GetPropBool(bAlphaRegular);
	fAlphaRegularStart->Enable(bTemp);
	fAlphaRegularEnd->Enable(bTemp);

	bTemp = GetPropBool(bAlphaSin);
	fAlphaSinStart->Enable(bTemp);
	fAlphaSinEnd->Enable(bTemp);
	fAlphaSinPeriodicTime->Enable(bTemp);

	bTemp = GetPropBool(bAlphaColor);
	sAlphaColor->Enable(bTemp);
}

BOOL		GCViewEffectEditor::OpenFile(const wchar_t* szFile)	//打开文件,如果失败则会自动删除
{
	if(GetEffectMgr())
		GetEffectMgr()->ClearEffect();
	safe_delete(m_effectPtr);
	this->m_strFile	=	szFile;
	if(!__super::OpenFile(szFile))
		return false;
	if(!m_viewerProps.m_hWnd)
		this->m_treeView.InsertItem(Language(L"TextureViewer", L"图片察看器"), 25, 25);
	CreateEffectMgr();
	wchar_t dir[512];
	GetCurrentDirectory(512, dir);
	m_effectPtr = GetEffectMgr()->LoadEffect(A(m_strFile.c_str()));
	m_effectPlay = GetEffectMgr()->PlayEffect(m_effectPtr->m_effectName, float3(0, 0, 0), float3(2000, -2000, 0));
	//
	this->m_bShowUIGrid = true;
	RebuildTrees();
	if(!m_viewerProps.m_hWnd)
	{
		InitViewerProps();
		InitNodeProp();
	}
	return true;
}

VOID		GCViewEffectEditor::RebuildTrees()
{
	if(m_hViewItem)
		m_treeView.DeleteItem(m_hViewItem);
	wchar_t wFileName[256], wExt[256];
	_wsplitpath(m_strFile.c_str(), 0, 0, wFileName, wExt);
	wcscat(wFileName, wExt);
	m_hViewItem = m_treeView.InsertItem(wFileName, 18, 18);
	m_treeView.SetItemState(m_hViewItem, TVIS_BOLD, TVIS_BOLD);
	m_treeView.Expand(m_hViewItem, TVE_EXPAND);
	//
	this->ClearTreeItem();
	BuildAll();
	//
	m_treeView.m_callback	=	this;
}

VOID		GCViewEffectEditor::BuildAll(HTREEITEM hItem, CEffectNode* n)
{
	for(int i=0;i<n->m_vNodeInst.size();i++)
	{
		HTREEITEM hCursor		=	m_treeView.InsertItem(W(n->m_vNodeInst[i]->m_pPro.szNodeName), 18, 18, m_hViewItem);
		m_treeView.SetItemData(hCursor, AddTreeItem(EFFECT_NODE, n->m_vNodeInst[i]->m_nID));
		if(n->m_vNodeInst[i]->m_vNodeInst.size() > 0)
			BuildAll(hCursor, n->m_vNodeInst[i]);
	}
}

VOID		GCViewEffectEditor::BuildAll()
{
	m_treeView.SelectItem(m_hViewItem);
	DeleteChildItems(m_hViewItem);
	m_treeView.SetItemData(m_hViewItem, AddTreeItem(EFFECT_MAIN));
	BuildAll(m_hViewItem, m_effectPtr);
}

VOID		GCViewEffectEditor::OnActive()		//被激活
{
	__super::OnActive();
}

VOID		GCViewEffectEditor::OnDeActive()		//失去激活
{
	__super::OnDeActive();
}

void		GCViewEffectEditor::OnFrameMove(const RenderParameter& r)	//每次更新
{
	GetEffectMgr()->FrameMove();
}

void		GCViewEffectEditor::OnPushRenderObjects(const RenderParameter& r)	//渲染
{
	GetEffectMgr()->Render();
}

BOOL		GCViewEffectEditor::SaveFile(BOOL bSaveAs)	//保存文件
{
	bool bNew	=	false;
	if(bSaveAs || m_strFile.size() == 0)
	{
		bNew	=	true;
		CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, formatw(L"%s|*.rpgmesh||", Language(L"EffectFile", L"特效文件") ), GetMainFrame());
		wchar_t buffer[1024];
		dlg.m_pOFN->lpstrInitialDir	=	AToW(GetProjectPath(), buffer, 1024);
		if(dlg.DoModal() != IDOK)
			return false;
		SetProjectPath();
		wchar_t szPath[256], szFile[256], szDir[256];
		_wsplitpath(dlg.GetPathName(), szPath, szDir, szFile, 0);
		wcscat(szPath, szDir);
		wcscat(szPath, szFile);
		wcscat(szPath, L".rpgmesh");
		m_strFile	=	szPath;
	}
	//if(m_effectPtr)
		//m_effectPtr->Save(A(m_strFile.c_str()));
	return __super::SaveFile(bSaveAs);
}

void		GCViewEffectEditor::OnRendering(const RenderParameter& r)	//渲染
{
	if(IsShowUIGrid())
	{
		GetRenderHelperPtr()->SetZWriteEnable(false);
		GetRenderHelperPtr()->SetAlphaMode(ALPHAMODE_ALPHABLEND);
		GetRenderHelperPtr()->SetTexture(0, 0);
		GetRenderHelperPtr()->SetSrcBlend(Enum::BLEND_SRCALPHA);
		GetRenderHelperPtr()->SetDestBlend(Enum::BLEND_INVSRCALPHA);
		GetRenderHelperPtr()->drawGrid(GetCameraPtr()->GetEye(), 0x3f3f3f3f, To3DValue(5.0f), 5, 5);
		GetRenderHelperPtr()->SetZWriteEnable(true);
	}
}

void		GCViewEffectEditor::OnRenderingEnd(const RenderParameter& r)
{
}
//渲染
void		GCViewEffectEditor::OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI)
{
	switch(nMenuID)
	{
	default:
		pCmdUI->Enable(false);
		break;
	}
}


void		GCViewEffectEditor::OnCommandUI(INT nMenuID)
{
}

VOID		GCViewEffectEditor::InitNodeProp()
{
	InitProp(m_nodeProp);
	CMFCPropertyGridProperty* create	= new CMFCPropertyGridProperty(Language(L"创建属性"));
	create->AddSubItem(textureFile		=	AddPropFile(Language(L"[粒子]贴图"), Language(L"textureFile", L"设置粒子贴图"), GetImageFilter()));
	create->AddSubItem(fDelayTime		= AddPropFloat(Language(L"延迟时间"), Language(L"fDelayTime", L"延迟时间。"), false, 0.000f, 10000.0f));
	create->AddSubItem(fCreateTime		= AddPropFloat(Language(L"创建时间"), Language(L"fCreateTime", L"创建时间。"), false, 0.000f, 10000.0f));
	create->AddSubItem(fLifeTime		= AddPropFloat(Language(L"声明周期"), Language(L"fLifeTime", L"声明周期。"), false, 0.000f, 10000.0f));
	create->AddSubItem(nNumNode			= AddPropInt(Language(L"节点数量"), Language(L"nNumNode", L"节点数量。"), 0, 1000));
	create->AddSubItem(eCreate			= AddPropOption(Language(L"创建方式"), Language(L"eCreate", L"创建方式"), g_typeEffectCreate));
	// StartMode
	CMFCPropertyGridProperty* start		= new CMFCPropertyGridProperty(Language(L"开始属性"));
	start->AddSubItem(eStart			= AddPropOption(Language(L"开始方式"), Language(L"eStart", L"开始方式"), g_typeEffectStart));
	start->AddSubItem(bScalarRegular	= AddPropBool(Language(L"定期缩放"), Language(L"bScalarRegular", L"定期缩放。")));
	start->AddSubItem(fScalarMax		= AddPropFloat(Language(L"最大缩放"), Language(L"fScalarMax", L"最大缩放。"), false, -10000.0f, 10000.0f));
	start->AddSubItem(fScalarMin		= AddPropFloat(Language(L"最小缩放"), Language(L"fScalarMin", L"最小缩放。"), false, -10000.0f, 10000.0f));
	start->AddSubItem(bAngleRegular	= AddPropBool(Language(L"定期转动"), Language(L"bAngleRegular", L"定期转动。")));
	start->AddSubItem(fAngleMax		= AddPropFloat(Language(L"最大角度"), Language(L"fAngleMax", L"最大角度。"), false, -10000.0f, 10000.0f));
	start->AddSubItem(fAngleMin		= AddPropFloat(Language(L"最小角度"), Language(L"fAngleMin", L"最小角度。"), false, -10000.0f, 10000.0f));
	start->AddSubItem(bInheritMatrix	= AddPropBool(Language(L"继承矩阵"), Language(L"bInheritMatrix", L"继承矩阵。")));
	// MoveMode
	CMFCPropertyGridProperty* movemode		= new CMFCPropertyGridProperty(Language(L"移动属性"));
	movemode->AddSubItem(bMoveMode			= AddPropBool(Language(L"是否移动"), Language(L"bMoveMode", L"是否移动。")));
	movemode->AddSubItem(eMove				= AddPropOption(Language(L"移动模式"), Language(L"eMove", L"移动模式"), g_typeEffectMove));
	movemode->AddSubItem(fMoveSpeedMax		= AddPropFloat(Language(L"移动速度最大"), Language(L"fMoveSpeedMax", L"移动速度最大。"), false, -10000.0f, 10000.0f));
	movemode->AddSubItem(fMoveSpeedMin		= AddPropFloat(Language(L"移动速度最小"), Language(L"fMoveSpeedMin", L"移动速度最小。"), false, -10000.0f, 10000.0f));
	movemode->AddSubItem(bExpansionMatrix	= AddPropBool(Language(L"扩展矩阵"), Language(L"bExpansionMatrix", L"扩展矩阵。")));
	// Pos
	CMFCPropertyGridProperty* startPos		= new CMFCPropertyGridProperty(Language(L"开始位置"));
	startPos->AddSubItem(bStartPos			= AddPropBool(Language(L"开始位置有效"), Language(L"bStartPos", L"开始位置有效。")));
	startPos->AddSubItem(sStartPosX			= AddPropFloat(Language(L"开始位置X"), Language(L"sStartPosX", L"开始位置X。"), false, -100000.0f, 100000.0f));
	startPos->AddSubItem(sStartPosY			= AddPropFloat(Language(L"开始位置Y"), Language(L"sStartPosY", L"开始位置Y。"), false, -100000.0f, 100000.0f));
	startPos->AddSubItem(sStartPosZ			= AddPropFloat(Language(L"开始位置Z"), Language(L"sStartPosZ", L"开始位置Z。"), false, -100000.0f, 100000.0f));
	startPos->AddSubItem(bEndPos			= AddPropBool(Language(L"结束位置有效"), Language(L"bEndPos", L"结束位置有效。")));
	startPos->AddSubItem(sEndPosX			= AddPropFloat(Language(L"结束位置X"), Language(L"sEndPosX", L"结束位置X。"), false, -100000.0f, 100000.0f));
	startPos->AddSubItem(sEndPosY			= AddPropFloat(Language(L"结束位置Y"), Language(L"sEndPosY", L"结束位置Y。"), false, -100000.0f, 100000.0f));
	startPos->AddSubItem(sEndPosZ			= AddPropFloat(Language(L"结束位置Z"), Language(L"sEndPosZ", L"结束位置Z。"), false, -100000.0f, 100000.0f));
	startPos->AddSubItem(bBoundCheck	= AddPropBool(Language(L"约束检查"), Language(L"bBoundCheck", L"约束检查。")));
	startPos->AddSubItem(bInheritEndPos	= AddPropBool(Language(L"继承结束位置"), Language(L"bInheritEndPos", L"继承结束位置。")));
	startPos->AddSubItem(bPosRevers	= AddPropBool(Language(L"位置反向"), Language(L"bPosRevers", L"位置反向。")));
	// Straight
	CMFCPropertyGridProperty* straight		= new CMFCPropertyGridProperty(Language(L"直线设置"));
	straight->AddSubItem(bStraight			= AddPropBool(Language(L"直线有效"), Language(L"bStraight", L"直线有效。")));
	straight->AddSubItem(fStraightMoveSpeed		= AddPropFloat(Language(L"移动速度值"), Language(L"sEndPosX", L"移动速度值。"), false, -100000.0f, 100000.0f));
	straight->AddSubItem(fStraightAccelSpeed	= AddPropFloat(Language(L"直线加速速度"), Language(L"sEndPosX", L"直线加速速度。"), false, -100000.0f, 100000.0f));
	straight->AddSubItem(sStraightDirX			= AddPropFloat(Language(L"直线方向X"), Language(L"sEndPosX", L"直线方向X。"), false, -100000.0f, 100000.0f));
	straight->AddSubItem(sStraightDirY			= AddPropFloat(Language(L"直线方向Y"), Language(L"sEndPosX", L"直线方向Y。"), false, -100000.0f, 100000.0f));
	straight->AddSubItem(sStraightDirZ			= AddPropFloat(Language(L"直线方向Z"), Language(L"sEndPosX", L"直线方向Z。"), false, -100000.0f, 100000.0f));
	// Rotate
	CMFCPropertyGridProperty* rotate		= new CMFCPropertyGridProperty(Language(L"转动设置"));
	rotate->AddSubItem(bRotate			= AddPropBool(Language(L"转动有效"), Language(L"bRotate", L"转动有效。")));
	rotate->AddSubItem(fRotateRadius		= AddPropFloat(Language(L"转动角度"), Language(L"fRotateRadius", L"转动角度。"), false, -100000.0f, 100000.0f));
	rotate->AddSubItem(fRotateAngleSpeed	= AddPropFloat(Language(L"转动角度速度"), Language(L"fRotateAngleSpeed", L"转动角度速度。"), false, -100000.0f, 100000.0f));
	rotate->AddSubItem(sRotateDirX			= AddPropFloat(Language(L"转动方向X"), Language(L"sRotateDirX", L"转动方向X。"), false, -100000.0f, 100000.0f));
	rotate->AddSubItem(sRotateDirY			= AddPropFloat(Language(L"转动方向Y"), Language(L"sRotateDirY", L"转动方向Y。"), false, -100000.0f, 100000.0f));
	rotate->AddSubItem(sRotateDirZ			= AddPropFloat(Language(L"转动方向Z"), Language(L"sRotateDirZ", L"转动方向Z。"), false, -100000.0f, 100000.0f));
	// Gravity
	CMFCPropertyGridProperty* gravity		= new CMFCPropertyGridProperty(Language(L"引力设置"));
	gravity->AddSubItem(bGravity			= AddPropBool(Language(L"引力有效"), Language(L"bGravity", L"引力有效。")));
	gravity->AddSubItem(fGravityStart		= AddPropFloat(Language(L"重力开始"), Language(L"fGravityStart", L"重力开始。"), false, -100000.0f, 100000.0f));
	gravity->AddSubItem(fGravitySpeed		= AddPropFloat(Language(L"重力速度"), Language(L"fGravitySpeed", L"重力速度。"), false, -100000.0f, 100000.0f));
	gravity->AddSubItem(sGravityDirX			= AddPropFloat(Language(L"重力方向X"), Language(L"sGravityDirX", L"重力方向X。"), false, -100000.0f, 100000.0f));
	gravity->AddSubItem(sGravityDirY			= AddPropFloat(Language(L"重力方向Y"), Language(L"sGravityDirY", L"重力方向Y。"), false, -100000.0f, 100000.0f));
	gravity->AddSubItem(sGravityDirZ			= AddPropFloat(Language(L"重力方向Z"), Language(L"sGravityDirZ", L"重力方向Z。"), false, -100000.0f, 100000.0f));
	// Following
	CMFCPropertyGridProperty* following		= new CMFCPropertyGridProperty(Language(L"引力设置"));
	following->AddSubItem(bFollowing			= AddPropBool(Language(L"跟随有效"), Language(L"bFollowing", L"跟随有效。")));
	following->AddSubItem(fFollowingRangeStart		= AddPropFloat(Language(L"跟随开始"), Language(L"fFollowingRangeStart", L"跟随开始。"), false, -100000.0f, 100000.0f));
	following->AddSubItem(fFollowingRangeAccel		= AddPropFloat(Language(L"跟随加速"), Language(L"fFollowingRangeAccel", L"跟随加速。"), false, -100000.0f, 100000.0f));
	// Dir
	CMFCPropertyGridProperty* dirstart		= new CMFCPropertyGridProperty(Language(L"方向设置"));
	dirstart->AddSubItem(bDirStart			= AddPropBool(Language(L"开始方向有效"), Language(L"bDirStart", L"开始方向有效。")));
	dirstart->AddSubItem(sDirStartX			= AddPropFloat(Language(L"开始方向X"), Language(L"sDirStartX", L"开始方向X。"), false, -100000.0f, 100000.0f));
	dirstart->AddSubItem(sDirStartY			= AddPropFloat(Language(L"开始方向Y"), Language(L"sDirStartY", L"开始方向Y。"), false, -100000.0f, 100000.0f));
	dirstart->AddSubItem(sDirStartZ			= AddPropFloat(Language(L"开始方向Z"), Language(L"sDirStartZ", L"开始方向Z。"), false, -100000.0f, 100000.0f));
	dirstart->AddSubItem(bDirRotate			= AddPropBool(Language(L"转动速度有效"), Language(L"bDirRotate", L"转动方向有效。")));
	dirstart->AddSubItem(fDirRotateSpeedX			= AddPropFloat(Language(L"转动速度X"), Language(L"fDirRotateSpeedX", L"转动速度X。"), false, -100000.0f, 100000.0f));
	dirstart->AddSubItem(fDirRotateSpeedY			= AddPropFloat(Language(L"转动速度Y"), Language(L"fDirRotateSpeedY", L"转动速度Y。"), false, -100000.0f, 100000.0f));
	dirstart->AddSubItem(fDirRotateSpeedZ			= AddPropFloat(Language(L"转动速度Z"), Language(L"fDirRotateSpeedZ", L"转动速度Z。"), false, -100000.0f, 100000.0f));
	dirstart->AddSubItem(bDirBillBoard			= AddPropBool(Language(L"Billboard"), Language(L"bDirBillBoard", L"Billboard。")));
	dirstart->AddSubItem(bDirEqualNodeDir			= AddPropBool(Language(L"前方方向"), Language(L"bDirEqualNodeDir", L"前方方向。")));
	// DirChild
	CMFCPropertyGridProperty* dirchild		= new CMFCPropertyGridProperty(Language(L"子方向设置"));
	dirchild->AddSubItem(bDirChildStart			= AddPropBool(Language(L"子方向有效"), Language(L"bDirChildStart", L"子方向有效。")));
	dirchild->AddSubItem(sDirChildStartX			= AddPropFloat(Language(L"子方向X"), Language(L"sDirChildStartX", L"子方向X。"), false, -100000.0f, 100000.0f));
	dirchild->AddSubItem(sDirChildStartY			= AddPropFloat(Language(L"子方向Y"), Language(L"sDirChildStartY", L"子方向Y。"), false, -100000.0f, 100000.0f));
	dirchild->AddSubItem(sDirChildStartZ			= AddPropFloat(Language(L"子方向Z"), Language(L"sDirChildStartZ", L"子方向Z。"), false, -100000.0f, 100000.0f));
	dirchild->AddSubItem(bDirChildRotate			= AddPropBool(Language(L"子转动有效"), Language(L"bDirChildRotate", L"子转动有效。")));
	dirchild->AddSubItem(fDirChildRotateSpeedX			= AddPropFloat(Language(L"子转动速度X"), Language(L"fDirChildRotateSpeedX", L"子转动速度X。"), false, -100000.0f, 100000.0f));
	dirchild->AddSubItem(fDirChildRotateSpeedY			= AddPropFloat(Language(L"子转动速度Y"), Language(L"fDirChildRotateSpeedY", L"子转动速度Y。"), false, -100000.0f, 100000.0f));
	dirchild->AddSubItem(fDirChildRotateSpeedZ			= AddPropFloat(Language(L"子转动速度Z"), Language(L"fDirChildRotateSpeedZ", L"子转动速度Z。"), false, -100000.0f, 100000.0f));
	dirchild->AddSubItem(bDirChildRandomRotateX			= AddPropBool(Language(L"随机方向X"), Language(L"bDirChildRandomRotateX", L"随机方向X。")));
	dirchild->AddSubItem(bDirChildRandomRotateY			= AddPropBool(Language(L"随机方向Y"), Language(L"bDirChildRandomRotateY", L"随机方向Y。")));
	dirchild->AddSubItem(bDirChildRandomRotateZ			= AddPropBool(Language(L"随机方向Z"), Language(L"bDirChildRandomRotateZ", L"随机方向Z。")));
	dirchild->AddSubItem(bDirChildBillBoard				= AddPropBool(Language(L"子对象Billbaord"), Language(L"bDirChildBillBoard", L"子对象Billbaord。")));
	dirchild->AddSubItem(bDirChildEqualNodeDir			= AddPropBool(Language(L"子对象前方方向"), Language(L"bDirChildEqualNodeDir", L"子对象前方方向。")));
	// Scale
	CMFCPropertyGridProperty* scale			= new CMFCPropertyGridProperty(Language(L"缩放设置"));
	scale->AddSubItem(bScale				= AddPropBool(Language(L"缩放有效"), Language(L"bScale", L"缩放有效。")));
	scale->AddSubItem(bScaleBaseRegular			= AddPropBool(Language(L"定期缩放"), Language(L"bScaleBaseRegular", L"定期缩放。")));
	scale->AddSubItem(fScaleBaseRegularStart			= AddPropFloat(Language(L"定期开始"), Language(L"fScaleBaseRegularStart", L"定期开始。"), false, -100000.0f, 100000.0f));
	scale->AddSubItem(fScaleBaseRegularSpeed			= AddPropFloat(Language(L"定期速度"), Language(L"fScaleBaseRegularSpeed", L"定期速度。"), false, -100000.0f, 100000.0f));
	scale->AddSubItem(bScaleBaseSin			= AddPropBool(Language(L"缩放Sin"), Language(L"bScaleBaseSin", L"缩放Sin。")));
	scale->AddSubItem(fScaleBaseSinStart			= AddPropFloat(Language(L"Sin开始"), Language(L"fScaleBaseSinStart", L"Sin开始。"), false, -100000.0f, 100000.0f));
	scale->AddSubItem(fScaleBaseSinEnd			= AddPropFloat(Language(L"Sin结束"), Language(L"fScaleBaseSinEnd", L"Sin结束。"), false, -100000.0f, 100000.0f));
	scale->AddSubItem(fScaleBaseSinPeriodicTime	= AddPropFloat(Language(L"Sin定期时间"), Language(L"fScaleBaseSinPeriodicTime", L"Sin定期时间。"), false, -100000.0f, 100000.0f));
	//
	scale->AddSubItem(bScaleAdditionX			= AddPropBool(Language(L"缩放增加X"), Language(L"bScaleAdditionX", L"缩放增加X。")));
	scale->AddSubItem(bScaleAdditionY			= AddPropBool(Language(L"缩放增加Y"), Language(L"bScaleAdditionY", L"缩放增加Y。")));
	scale->AddSubItem(bScaleAdditionZ			= AddPropBool(Language(L"缩放增加Z"), Language(L"bScaleAdditionZ", L"缩放增加Z。")));
	scale->AddSubItem(bScaleAdditionRegular			= AddPropBool(Language(L"缩放定期增加"), Language(L"bScaleAdditionRegular", L"缩放定期增加。")));
	scale->AddSubItem(fScaleAdditionRegularStart	= AddPropFloat(Language(L"缩放定期增加开始"), Language(L"fScaleAdditionRegularStart", L"缩放定期增加开始。"), false, -100000.0f, 100000.0f));
	scale->AddSubItem(fScaleAdditionRegularSpeed	= AddPropFloat(Language(L"缩放定期增加速度"), Language(L"fScaleAdditionRegularSpeed", L"缩放定期增加速度。"), false, -100000.0f, 100000.0f));
	//
	scale->AddSubItem(bScaleAdditionSin			= AddPropBool(Language(L"缩放增加Sin"), Language(L"bScaleAdditionSin", L"缩放增加Sin。")));
	scale->AddSubItem(fScaleAdditionSinStart			= AddPropFloat(Language(L"Sin增加开始"), Language(L"fScaleAdditionSinStart", L"Sin增加开始。"), false, -100000.0f, 100000.0f));
	scale->AddSubItem(fScaleAdditionSinEnd			= AddPropFloat(Language(L"Sin增加结束"), Language(L"fScaleAdditionSinEnd", L"Sin增加结束。"), false, -100000.0f, 100000.0f));
	scale->AddSubItem(fScaleAdditionSinPeriodicTime	= AddPropFloat(Language(L"Sin增加定期时间"), Language(L"fScaleAdditionSinPeriodicTime", L"Sin增加定期时间。"), false, -100000.0f, 100000.0f));
	// Alpha
	CMFCPropertyGridProperty* alpha			= new CMFCPropertyGridProperty(Language(L"透明设置"));
	alpha->AddSubItem(eAlphaBlend			= AddPropOption(Language(L"透明模式"), Language(L"eAlphaBlend", L"透明模式"), g_typeEffectAlpha));
	alpha->AddSubItem(bAlphaZBuffer			= AddPropBool(Language(L"Z缓冲开启"), Language(L"bAlphaZBuffer", L"Z缓冲开启。")));
	alpha->AddSubItem(bAlphaRegular			= AddPropBool(Language(L"定期透明"), Language(L"bAlphaRegular", L"定期透明。")));
	alpha->AddSubItem(fAlphaRegularStart	= AddPropFloat(Language(L"透明开始"), Language(L"fAlphaRegularStart", L"透明开始。"), false, -100000.0f, 100000.0f));
	alpha->AddSubItem(fAlphaRegularEnd	= AddPropFloat(Language(L"透明结束"), Language(L"fAlphaRegularEnd", L"透明结束。"), false, -100000.0f, 100000.0f));
	//
	alpha->AddSubItem(bAlphaSin			= AddPropBool(Language(L"透明Sin"), Language(L"bAlphaSin", L"透明Sin。")));
	alpha->AddSubItem(fAlphaSinStart	= AddPropFloat(Language(L"透明Sin开始"), Language(L"fAlphaSinStart", L"透明Sin开始。"), false, -100000.0f, 100000.0f));
	alpha->AddSubItem(fAlphaSinEnd	= AddPropFloat(Language(L"透明Sin结束"), Language(L"fAlphaSinEnd", L"透明Sin结束。"), false, -100000.0f, 100000.0f));
	alpha->AddSubItem(fAlphaSinPeriodicTime	= AddPropFloat(Language(L"透明Sin定期时间"), Language(L"fAlphaSinPeriodicTime", L"透明Sin定期时间。"), false, -100000.0f, 100000.0f));
	alpha->AddSubItem(bAlphaColor			= AddPropBool(Language(L"透明颜色有效"), Language(L"bAlphaColor", L"透明颜色有效。")));
	alpha->AddSubItem(sAlphaColor			= AddPropColor(Language(L"透明颜色"), Language(L"sAlphaColor", L"透明颜色。")));
	// AtomicInst
	CMFCPropertyGridProperty* atomic			= new CMFCPropertyGridProperty(Language(L"粒子设置"));
	atomic->AddSubItem(bAtomicInstAnimation			= AddPropBool(Language(L"粒子克隆有效"), Language(L"bAtomicInstAnimation", L"粒子克隆有效")));
	atomic->AddSubItem(bAtomicInstTwoSide			= AddPropBool(Language(L"粒子双面渲染"), Language(L"bAtomicInstTwoSide", L"粒子双面渲染")));
	atomic->AddSubItem(bAtomicInstAniTime			= AddPropBool(Language(L"粒子动画时间有效"), Language(L"bAtomicInstAniTime", L"粒子动画时间有效")));
	atomic->AddSubItem(fAtomicInstAniTime	= AddPropFloat(Language(L"粒子动画时间"), Language(L"fAtomicInstAniTime", L"粒子动画时间。"), false, -100000.0f, 100000.0f));
	m_nodeProp.AddProperty(create);
	m_nodeProp.AddProperty(start);
	m_nodeProp.AddProperty(movemode);
	m_nodeProp.AddProperty(startPos);
	m_nodeProp.AddProperty(straight);
	m_nodeProp.AddProperty(rotate);
	m_nodeProp.AddProperty(gravity);
	m_nodeProp.AddProperty(following);
	m_nodeProp.AddProperty(dirstart);
	m_nodeProp.AddProperty(dirchild);
	m_nodeProp.AddProperty(scale);
	m_nodeProp.AddProperty(alpha);
	m_nodeProp.AddProperty(atomic);
}
