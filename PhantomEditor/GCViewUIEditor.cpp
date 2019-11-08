/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "GCViewUIEditor.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"
#include "phantom-gcDoc.h"
#include "phantom-gcView.h"
#include "AlignDialog.h"
#include "LuaXmlModule.h"

extern void	OnAppInputEvent(int nEvent, int x, int y, unsigned int touchIndex);
BOOL g_bLButtonDown = false;
BOOL g_bRButtonDown = false;
BOOL g_bMButtonDown = false;
VOID	OnAppMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(hWnd, &pos);
	switch(uMsg)
	{
	case WM_LBUTTONDBLCLK:
		OnAppInputEvent(2, pos.x, pos.y, 0);
		break;
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		OnAppInputEvent(2, pos.x, pos.y, 0);
		g_bLButtonDown = true;
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		OnAppInputEvent(4, pos.x, pos.y, 0);
		g_bLButtonDown = false;
		break;
	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		OnAppInputEvent(2, pos.x, pos.y, 1);
		g_bRButtonDown = true;
		break;
	case WM_RBUTTONUP:
		ReleaseCapture();
		OnAppInputEvent(4, pos.x, pos.y, 1);
		g_bRButtonDown = false;
		break;
	case WM_MBUTTONDOWN:
		SetCapture(hWnd);
		OnAppInputEvent(2, pos.x, pos.y, 2);
		g_bMButtonDown = true;
		break;
	case WM_MBUTTONUP:
		ReleaseCapture();
		OnAppInputEvent(4, pos.x, pos.y, 2);
		g_bMButtonDown = false;
		break;
	case WM_MOUSEMOVE:
		{
			if(g_bLButtonDown)
				OnAppInputEvent(1, pos.x, pos.y, 0);
			if(g_bRButtonDown)
				OnAppInputEvent(1, pos.x, pos.y, 1);
			if(g_bMButtonDown)
				OnAppInputEvent(1, pos.x, pos.y, 2);
		}
		break;
	case WM_MOUSEWHEEL:
		OnAppInputEvent(5, (short)HIWORD(wParam), 0, 0);
		break;
	case WM_KEYDOWN:
		if(wParam == VK_RETURN)
		{
			OnAppInputEvent(5, 0, 0, 0);
		}
		break;
	}
}

Phantom::UVAnimation*	GetUVAniByID(Phantom::UIElement* ele, INT nID)
{
	for(int i=0;i<ele->GetUVAnimationCount();i++)
	{
		if(ele->GetUVAnimationPtr(i)->GetID() == nID)
			return ele->GetUVAnimationPtr(i);
	}
	return 0;
}
extern int	GetGameWidth();
extern int	GetGameHeight();

struct	ElementUVRemove
{
	int						controlID;
	int						p2;
	Phantom::UIAnimationTrack*	track;
	Phantom::UVAnimation*	uvAni;
	int						type;
	//
	int						element;
	HTREEITEM				hItem;
	Phantom::UIElement*				elementPtr;
};

struct	UICopyData
{
	TreeItemType		type;
	GCViewUIEditor*		editor;
	UICopyData()
	{
		editor	=	0;
	}
};

std::vector<UICopyData>		g_copyUIs;

VOID		SetUICopy(GCViewUIEditor* e, std::vector<TreeItemType*>* vs)
{
	g_copyUIs.clear();
	for(int i=0;i<vs->size();i++)
	{
		UICopyData ud;
		ud.editor	=	e;
		ud.type		=	*vs->at(i);
		g_copyUIs.push_back(ud);
	}
}

BOOL		IsCanPaste(int nType)
{
	int tt	=	nType;
	switch(nType)
	{
	case GCViewUIEditor::UI_PROP_MAIN:
		tt	=	GCViewUIEditor::UI_PROP_GROUP;
		break;
	case GCViewUIEditor::UI_PROP_GROUP:
		tt	=	GCViewUIEditor::UI_PROP_CONTROL;
		break;
	case GCViewUIEditor::UI_PROP_ANIMATION_LIST:
		tt	=	GCViewUIEditor::UI_PROP_ANIMATION;
		break;
	}
	for(int i=0;i<g_copyUIs.size();i++)
	{
		if(g_copyUIs[i].type.type == tt)
		{
			return true;
		}
		else if(g_copyUIs[i].type.type == GCViewUIEditor::UI_PROP_UV_ANIMATION)
		{
			if(nType == GCViewUIEditor::UI_PROP_ELEMENTS)
				return true;
		}
	}
	return false;
}
//
//Phantom::UIElement*	GetCopyElement()
//{
//	if(!g_copyEditor)
//		return 0;
//	if(g_copyUI.type != GCViewUIEditor::UI_PROP_ELEMENTS)
//		return 0;
//	Phantom::UIControl* c	=	g_copyEditor->m_dialogPtr->GetControlPtrFromID(g_copyUI.p1);
//	if(!c)
//		return 0;
//	return c->GetElementPtr(g_copyUI.p2);
//}


GCViewUIEditor::GCViewUIEditor(CPhantomGCDoc* docPtr)
	:GCViewBase(docPtr, Language(L"界面编辑"), IDB_UI_VIEW_BMP)
{
	m_trackType=m_trackP1=m_trackP2=m_trackP3=m_trackP4=0;
	this->m_dialogPtr		=	0;
	m_bAlwaysVisible		=	false;
	m_nCurrentEditorMode	=	EditorMode_None;
	m_nEditorFlags			=	UIControlType_NONE;
	m_insertControl			=	0;
	m_bLButtonDown			=	false;
	m_editorMode			=	MouseMode_None;
	m_nTabIndex				=	1;
	m_moveControl			=	0;
	m_nAlignType			=	ToAlign_None;
	m_alignPointW.SetPoint(0, 0);
	m_alignPointH.SetPoint(0, 0);
	m_fFloorSetRadius		=	100.0f;
	m_bDisableMoveMode		=	false;
	m_bShowCenter			=	false;
	m_bDownMButton			=	false;
	m_bSelectElementMode	=	false;
	m_bSetLevelMode			=	false;
}
BOOL	GCViewUIEditor::IsCenterAlign()
{
	return false;//this->m_dialogPtr->m_bCenterAlign;
}

GCViewUIEditor::~GCViewUIEditor(void)
{
	CloseFile();
}

Phantom::UIControl*			GCViewUIEditor::PickControls(PickInfo& ret, CPoint pos)
{
	for(int i=0;i<m_controlSelects.size();i++)
	{
		if(IsPickControl(pos, m_controlSelects[i], ret))
		{
			return m_controlSelects[i];
		}
	}
	return 0;
}

BOOL	GCViewUIEditor::IsPickControl(CPoint pos, Phantom::UIControl* sel, PickInfo& ret)
{
	if(!sel->GetVisible())
		return false;
	Phantom::UIGroup* group = sel->GetGroupPtr();
	if(!group||!group->GetVisible())
		return false;
	if(!sel->GetEnabled() ||!group||!group->GetEnabled())
		return false;
	Phantom::Rect rc	=	sel->GetWindowRect();
	//
	Phantom::Rect moves[MovePointSide_Count];
	GetMovePointRect(sel, rc, moves);
	for(int i=0;i<MovePointSide_Count;i++)
	{
		if(!sel->IsRotation())
			if(/*i == MovePointSide_Center ||*/ i == MovePointSide_Rotation)
				continue;
		if(moves[i].isPtIn(pos.x, pos.y))
		{
			if(i == MovePointSide_Body)
			{
				ret.nPickType		=	this->PickType_Body;
			}
			else
			{
				ret.nPickType		=	this->PickType_MoveSide;
				ret.nMovePointSide	=	i;
			}
			return true;
		}
	}
	return false;
}

Phantom::UIControl*		GCViewUIEditor::PickControl(CPoint pos)
{
	//
	std::vector<Phantom::UIControl*> sels;
	for(int i=0;i<m_dialogPtr->GetControlCount();i++)
	{
		Phantom::UIControl* sel	=	m_dialogPtr->GetControlPtr(i);
		if(!sel->GetVisible())
			continue;
		Phantom::UIGroup* group = sel->GetGroupPtr();
		if(!group ||!group->GetVisible())
			continue;
		if(!sel->GetEnabled()||!sel->m_bSelect)
			continue;
		Phantom::Rect rc	=	sel->GetWindowRect();
		Phantom::Pixel clientPos	=	sel->ScreenToClient(Phantom::Pixel(pos.x, pos.y));
		if(rc.isPtIn(clientPos.x, clientPos.y))
			sels.push_back(sel);
	}
	static int index = 0;
	if(sels.size() > 0)
		return sels[(index++)%sels.size()];
	return 0;
}

void		GCViewUIEditor::OnRenderingEnd()	//渲染
{
	if(m_nCurrentEditorMode <= 0)
		return;
	float fX = (float)Phantom::g_manager.GetDeviceWidth()/(float)Phantom::g_manager.m_gameWidth;
	float fY = (float)Phantom::g_manager.GetDeviceHeight()/(float)Phantom::g_manager.m_gameHeight;
	int dx = 0;
	int dy = 0;
	int w = Phantom::g_manager.GetBufferWidth();
	int h = Phantom::g_manager.GetBufferHeight();
	m_dialogPtr->GetLocation(dx,dy);
	float bx = (float)dx*fX;
	float by = (float)dy*fY;
	Phantom::g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
	Phantom::g_manager.SetUIDialogSize(true, Phantom::g_manager.GetBufferWidth(), Phantom::g_manager.GetBufferHeight(), 0);
	if(IsShowUIGrid())
	{
		CPoint size	=	GetUIGridSize();
		float gx	=	((float)size.x);
		float gy	=	((float)size.y);
		if((int)gx > 1 && (int)gy > 1)
		{
			float offsetX		=	fmodf(bx, gx);
			float offsetY		=	fmodf(by, gy);
			int screenWidth		=	(int)((float)Phantom::g_manager.GetBufferWidth());
			int screenHeight	=	(int)((float)Phantom::g_manager.GetBufferHeight());
			int wCount	=	screenWidth / (int)gx;
			int hCount	=	screenHeight / (int)gy;
			for(int x=0;x<wCount;x++)
			{
				Phantom::Pixel pos[2];
				pos[0].Set((int)((float)(x * gx + offsetX)), 0);
				pos[1].Set((int)((float)(x * gx + offsetX)), screenHeight);
				Phantom::g_manager.DrawLines2D(pos, 2, Phantom::color4(0x6f8f8f8f), false);
			}
			for(int y=0;y<hCount;y++)
			{
				Phantom::Pixel pos[2];
				pos[0].Set(0, (int)((float)(y * gy + offsetY)));
				pos[1].Set(screenWidth, (int)((float)(y * gy + offsetY)));
				Phantom::g_manager.DrawLines2D(pos, 2, Phantom::color4(0x6f8f8f8f), false);
			}
		}
		if(this->m_bLButtonDown)
		{
			if(this->m_nAlignType & ToAlign_Left || m_nAlignType & ToAlign_Right)
			{
				Phantom::Pixel pos[2];
				pos[0].Set((int)((float)(this->m_alignPointW.x + bx)), 0);
				pos[1].Set((int)((float)(this->m_alignPointW.x + bx)), Phantom::g_manager.GetBufferHeight());
				Phantom::g_manager.DrawLines2D(pos, 2, Phantom::color4(0x6f8fffff), false);
			}
			if(this->m_nAlignType & ToAlign_Top || m_nAlignType & ToAlign_Bottom)
			{
				Phantom::Pixel pos[2];
				pos[0].Set(0, (int)((float)(m_alignPointH.y + by)));
				pos[1].Set(Phantom::g_manager.GetBufferWidth(), (int)((float)(m_alignPointH.y + by)));
				Phantom::g_manager.DrawLines2D(pos, 2, Phantom::color4(0x6f8fffff), false);
			}
		}
	}
	if(m_dialogPtr){
		Phantom::Pixel pos[2];
		//Phantom::g_manager.DrawRect2D(Phantom::Rect(bx,by,bx+w,by+h), 0x20af5555, true);
		pos[0].Set(bx,by);
		pos[1].Set(bx+w,by);
		Phantom::g_manager.DrawLines2D(pos, 2, 0xff00ff00, false);
		pos[0].Set(bx,by);
		pos[1].Set(bx,by+h);
		Phantom::g_manager.DrawLines2D(pos, 2, 0xff00ff00, false);
		pos[0].Set(bx+w,by);
		pos[1].Set(bx+w,by+h);
		Phantom::g_manager.DrawLines2D(pos, 2, 0xff00ff00, false);
		pos[0].Set(bx,by+h);
		pos[1].Set(bx+w,by+h);
		Phantom::g_manager.DrawLines2D(pos, 2, 0xff00ff00, false);
	}
}

void		GCViewUIEditor::OnRenderEndUI()
{
#ifdef USE_BOX
	if(m_dialogPtr->m_box2World.IsEnable() && EditorMode_TerrainUnit == m_nCurrentEditorMode/* && m_dialogPtr->m_box2World.IsEnable()*/)
		m_dialogPtr->m_box2World.Render();
#endif
	if(m_nCurrentEditorMode <= 0)
		return;
	//
	Phantom::g_manager.SetUIDialogSize(true, Phantom::g_manager.GetBufferWidth(), Phantom::g_manager.GetBufferHeight(), 0);
	float fX = (float)Phantom::g_manager.GetDeviceWidth()/(float)Phantom::g_manager.m_gameWidth;
	float fY = (float)Phantom::g_manager.GetDeviceHeight()/(float)Phantom::g_manager.m_gameHeight;
	if(m_editorMode != MouseMode_MapRect)
	{
		switch(m_nCurrentEditorMode)
		{
		case EditorMode_Default:
			{
				for(int s=0;s<m_controlSelects.size();s++)
				{
					Phantom::UIControl* c	=	m_controlSelects[s];
					if(c)
					{
						//Phantom::g_manager.SetUIDialogSize(c->m_nAlignMode==0, c->m_dialogPtr->m_screenSize.cx, c->m_dialogPtr->m_screenSize.cy, 0);
						Phantom::UIGroup* g	=	c->GetGroupPtr();
						Phantom::Rect rcT	=	c->GetWindowRect();
						Phantom::Rect rc(rcT.left * fX, rcT.top * fY, rcT.right * fX, rcT.bottom * fY);
						Phantom::Rect saveRc	=	rc;
						//
						Phantom::Pixel rp[5];
						rp[0]	=	c->ClientToScreen(Phantom::Pixel(rc.left, rc.top));
						rp[1]	=	c->ClientToScreen(Phantom::Pixel(rc.right, rc.top));
						rp[2]	=	c->ClientToScreen(Phantom::Pixel(rc.right, rc.bottom));
						rp[3]	=	c->ClientToScreen(Phantom::Pixel(rc.left, rc.bottom));
						rp[4]	=	rp[0];
						int lineColor	=	0xffff0000;
						int rectColor	=	0xffaf3f00;
						if(s == (int)m_controlSelects.size() - 1)
						{
							lineColor	=	0xff00ff00;
							rectColor	=	0xff3faf00;
						}
						Phantom::g_manager.DrawLines2D(rp, 5, lineColor, true);
						Phantom::g_manager.DrawRect2D(rc, rectColor, false);
						//
						Phantom::Rect moves[MovePointSide_Count];
						GetMovePointRect(c, rc, moves);
						for(int i=0;i<MovePointSide_Count;i++)
						{
							if(i == MovePointSide_Center)
							{
								//if(/*c->IsRotation() ||*/ g && g->GetGroupIsUnit() || c->GetAutoSort())
								{
									Phantom::g_manager.DrawCircle(Phantom::Pixel(moves[i].MiddleX()*fX, moves[i].MiddleY()*fY), (float)moves[i].GetWidth() / 2, 0xffff0000,  12, false);
									Phantom::g_manager.DrawCircle(Phantom::Pixel(moves[i].MiddleX()*fX, moves[i].MiddleY()*fY), (float)moves[i].GetWidth() / 2 - 1, 0x8f8fff00,  12, true);
								}
							}
							else if(i == MovePointSide_Rotation)
							{
								if(c->IsRotation())
								{
									Phantom::g_manager.DrawCircle(Phantom::Pixel(moves[i].MiddleX()*fX, moves[i].MiddleY()*fY), (float)moves[i].GetWidth() / 2.0f, 0xffff0000,  12, false);
									Phantom::g_manager.DrawCircle(Phantom::Pixel(moves[i].MiddleX()*fX, moves[i].MiddleY()*fY), (float)moves[i].GetWidth() / 2.0f - 1.0f, 0xff00ff00,  12, false);
									Phantom::g_manager.DrawCircle(Phantom::Pixel(moves[i].MiddleX()*fX, moves[i].MiddleY()*fY), (float)moves[i].GetWidth() / 2.0f - 2.0f, 0x8f0000ff,  12, true);
								}
							}
							else if(i == MovePointSide_Body)
								continue;
							else
								Phantom::g_manager.DrawRect2D(moves[i], 0xffff8000, false);
						}
						//
						Phantom::Rect rcLT(rc.left - 100, rc.top - 15, rc.left, rc.top);
						Phantom::Rect rcRB(rc.right, rc.bottom, rc.right + 100, rc.bottom + 15);
						//
						char wbuf[128];
						int x, y;
						m_dialogPtr->GetLocation(x,y);
						Phantom::Rect rcWin = saveRc;
						rcWin.Offset(-x, -y);
						sprintf(wbuf, "%d,%d", rcWin.left, rcWin.top);
						Phantom::g_manager.DrawTexts(wbuf, rcLT, 0, 16.0f, 0, &Phantom::color4(0xfff06f00), Phantom::DrawTextAlign_RIGHT | Phantom::DrawTextAlign_BOTTOM | Phantom::DrawTextAlign_SINGLELINE);
						sprintf(wbuf, "%d,%d", rcWin.right, rcWin.bottom);
						Phantom::g_manager.DrawTexts(wbuf, rcRB, 0, 16.0f, 0, &Phantom::color4(0xfff06f00), Phantom::DrawTextAlign_LEFT | Phantom::DrawTextAlign_TOP | Phantom::DrawTextAlign_SINGLELINE);
					}
				}
				if(this->m_nEditorFlags == UIEditorFlags_TabIndex)
				{
					for(int i=0;i<m_dialogPtr->GetControlCount();i++)
					{
						Phantom::UIControl* sel	=	m_dialogPtr->GetControlPtr(i);
						if(!sel->GetVisible())
							continue;
						Phantom::UIGroup* group = sel->GetGroupPtr();
						if(!group||!group->GetVisible())
							continue;
						if(!sel->GetEnabled())
							continue;
						if(sel->GetType() == UIControlType_STATIC || sel->GetType() == UIControlType_VIEW_FORMAT_TEXT)
							continue;
						Phantom::Rect rcT	=	sel->GetWindowRect();
						rcT.Offset(-10, -10);
						Phantom::Rect rc(rcT.left * fX, rcT.top * fY, rcT.right * fX, rcT.bottom * fY);
						char wbuf[128];
						sprintf(wbuf, "%d", sel->GetTabIndex());
						Phantom::g_manager.DrawTexts(wbuf, rc, 0, 16.0f, 0, &Phantom::color4(0xffffffff),  Phantom::DrawTextAlign_LEFT | Phantom::DrawTextAlign_TOP | Phantom::DrawTextAlign_SINGLELINE);
					}
				}
			}
			break;
#ifdef USE_BOX
		case EditorMode_TerrainUnit:
			{
				for(int s=0;s<this->m_selectBodys.size();s++)
				{
					Phantom::B2Body* b	=	m_selectBodys[s].bodyPtr;
					if(b)
						this->RenderRect(b);
					else if(m_selectBodys[s].jointPtr)
						this->RenderRect(m_selectBodys[s].jointPtr->m_data.localAnchorA);
				}
			}
			break;
#endif
		}
	}
	if(m_editorMode == MouseMode_SelectRect || m_editorMode == MouseMode_MapRect)
	{
		Phantom::Rect rc(min(m_lastPoint.x, m_downPoint.x), min(m_lastPoint.y, m_downPoint.y), max(m_lastPoint.x, m_downPoint.x), max(m_lastPoint.y, m_downPoint.y));
		rc.left		=	(int)((float)rc.left * fX);
		rc.right	=	(int)((float)rc.right * fX);
		rc.top		=	(int)((float)rc.top * fY);
		rc.bottom	=	(int)((float)rc.bottom * fY);
		Phantom::g_manager.DrawRect2D(rc, 0xaf5f0000, false);
	}
	else if(m_editorMode == MouseMode_MoveCopy)
	{
		for(int i=0;i<m_moveCopys.size();i++)
		{
			Phantom::Rect rc	=	(Phantom::Rect&)m_moveCopys[i];
			rc.left		=	(int)((float)rc.left * fX);
			rc.right	=	(int)((float)rc.right * fX);
			rc.top		=	(int)((float)rc.top * fY);
			rc.bottom	=	(int)((float)rc.bottom * fY);
			if(!GetCopyKeyboard())
				Phantom::g_manager.DrawRect2D(rc, 0xffff0000, false);
			else
				Phantom::g_manager.DrawRect2D(rc, 0xff00ff00, false);
			//
			Phantom::Rect moves[MovePointSide_Count];
			GetMovePointRect(0, rc, moves);
			for(int i=0;i<MovePointSide_Count;i++)
			{
				Phantom::g_manager.DrawRect2D(moves[i], 0xff80ff00, false);
			}
		}
	}
}

VOID		GCViewUIEditor::OnTreeSelectGroup(Phantom::UIGroup* group)
{
	//if(this->m_nCurrentEditorMode == EditorMode_TerrainFloor)
	//{
	//	for(int i=0;i<m_dialogPtr->GetControlCount();i++)
	//	{
	//		if(m_dialogPtr->GetControlPtr(i)->GetGroupID() == group->GetID())
	//			m_dialogPtr->GetControlPtr(i)->SetCustomRender(false);
	//		else
	//			m_dialogPtr->GetControlPtr(i)->SetCustomRender(true);
	//	}
	//}
	//else
	{
		//for(int i=0;i<m_dialogPtr->GetControlCount();i++)
		//{
		//	m_dialogPtr->GetControlPtr(i)->SetCustomRender(false);
		//}
	}
}

VOID		GCViewUIEditor::OnTreeSelectControl(Phantom::UIControl* c)
{
	//if(this->m_nCurrentEditorMode == EditorMode_TerrainFloor)
	//{
	//	for(int i=0;i<m_dialogPtr->GetControlCount();i++)
	//	{
	//		if(m_dialogPtr->GetControlPtr(i) == c)
	//			m_dialogPtr->GetControlPtr(i)->SetCustomRender(false);
	//		else
	//			m_dialogPtr->GetControlPtr(i)->SetCustomRender(true);
	//	}
	//}
	//else if(this->m_nCurrentEditorMode == EditorMode_TerrainUnit)
	//{
	//	for(int i=0;i<m_dialogPtr->GetControlCount();i++)
	//	{
	//		if(m_dialogPtr->GetControlPtr(i) == m_dialogPtr->GetTerrain2DPtr())
	//			m_dialogPtr->GetControlPtr(i)->SetCustomRender(false);
	//		else
	//			m_dialogPtr->GetControlPtr(i)->SetCustomRender(true);
	//	}
	//}
	//else
	//{
	//	for(int i=0;i<m_dialogPtr->GetControlCount();i++)
	//	{
	//		m_dialogPtr->GetControlPtr(i)->SetCustomRender(false);
	//	}
	//}
}

VOID		GCViewUIEditor::SetEditorMode(INT n)	//设置编辑模式
{
	m_nCurrentEditorMode	=	n;
	//m_dialogPtr->SetEditorMode(m_nCurrentEditorMode > 0);
	m_nEditorFlags	=	UIControlType_NONE;
	if(m_nCurrentEditorMode == this->EditorMode_TerrainUnit)
	{
		//m_dialogPtr->m_box2World.SetEnabled(true);
	}
}

VOID		GCViewUIEditor::SetEditorFlags(int type)
{
	m_nEditorFlags	=	type;
	if(m_nEditorFlags == UIControlType_NONE)
		m_nCurrentEditorMode	=	EditorMode_Default;
	if(m_nEditorFlags == UIEditorFlags_TabIndex)
		m_nTabIndex	=	1;
}

VOID		GCViewUIEditor::AlignObjects(AlignType type, int value)
{
	if(m_nCurrentEditorMode <= 0)
		return;
	if(type==AlignType_CenterAlign)
	{
		//m_dialogPtr->m_bCenterAlign = value;
		return;
	}
	switch(this->m_nCurrentEditorMode)
	{
	case EditorMode_Default:
		{
			BeginUndo();
			int size = m_controlSelects.size();
			UndoRedoBuffer* buff = AddUndoRedo(UI_PROP_MOVE_CONTROL, (ProcUndoRedo)0);
			buff->m_bEnabled = true;
			(*buff) << size;
			bool bSpace	=	false;
			int	begin	=	0;
			for(int i=(int)m_controlSelects.size() - 1;i>=0;i--)
			{
				Phantom::UIControl* c	=	m_controlSelects[i];
				if(!c->GetVisible())
					continue;
				Phantom::Rect rc		=	c->GetRenderRect();
				int width		=	rc.GetWidth();
				int height		=	rc.GetHeight();
				switch(type)
				{
				case AlignType_Left:
					{
						rc.left		=	value;
						rc.right	=	rc.left + width;
					}
					break;
				case AlignType_Top:
					{
						rc.top		=	value;
						rc.bottom	=	rc.top + height;
					}
					break;
				case AlignType_Right:
					{
						rc.right	=	value;
						rc.left		=	rc.right - width;
					}
					break;
				case AlignType_Bottom:
					{
						rc.bottom	=	value;
						rc.top		=	rc.bottom - height;
					}
					break;
				case AlignType_Width:
					{
						rc.right	=	rc.left + value;
					}
					break;
				case AlignType_Height:
					{
						rc.bottom	=	rc.top + value;
					}
					break;
				case AlignType_OffsetX:
					{
						rc.Offset(value, 0);
					}
					break;
				case AlignType_OffsetY:
					{
						rc.Offset(0, value);
					}
					break;
				case AlignType_SpaceX:
					{
						if(!bSpace)
						{
							bSpace	=	true;
							begin	=	rc.left;
						}
						else
						{
							rc.left		=	begin;
							rc.right	=	rc.left + width;
						}
						begin	+=	width;
						begin	+=	value;
					}
					break;
				case AlignType_SpaceY:
					{
						if(!bSpace)
						{
							bSpace	=	true;
							begin	=	rc.top;
						}
						else
						{
							rc.top		=	begin;
							rc.bottom	=	rc.top + height;
						}
						begin	+=	height;
						begin	+=	value;
					}
					break;
				}
				Phantom::Rect oldrc = c->GetWindowRect();
				Phantom::Pixel oldcent = c->GetRotCenter();
				c->SetRenderRect(rc);
				(*buff) << m_controlSelects[i]->GetID() << oldrc << oldcent << c->GetWindowRect() << c->GetRotCenter();
				this->SetModifyed(true);
			}
		}
		break;
	}
}

VOID		GCViewUIEditor::ChangeSelectControls()
{
	m_controlSelects.clear();
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
	//
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		switch(it->second->type)
		{
		case UI_PROP_MAIN:
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(!c->GetVisible())
						continue;
					Phantom::UIGroup* group = c->GetGroupPtr();
					if(!group||!group->GetVisible())
						continue;
					bool bFind		=	false;
					for(int j=0;j<m_controlSelects.size();j++)
					{
						if(m_controlSelects[j] == c)
						{
							bFind	=	true;
							break;
						}
					}
					if(!bFind)
						m_controlSelects.push_back(c);
				}
			}
			break;
		case UI_PROP_GROUP:
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(c->GetGroupID() == it->second->p1)
					{
						bool bFind		=	false;
						if(!c->GetVisible())
							continue;
						Phantom::UIGroup* group = c->GetGroupPtr();
						if(!group||!group->GetVisible())
							continue;
						for(int j=0;j<m_controlSelects.size();j++)
						{
							if(m_controlSelects[j] == c)
							{
								bFind	=	true;
								break;
							}
						}
						if(!bFind)
							m_controlSelects.push_back(c);
					}
				}
			}
			break;
		case UI_PROP_CONTROL:
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(!c->GetVisible())
						continue;
					Phantom::UIGroup* group = c->GetGroupPtr();
					if(!group||!group->GetVisible())
						continue;
					if(c->GetID() == it->second->p1)
					{
						bool bFind		=	false;
						for(int j=0;j<m_controlSelects.size();j++)
						{
							if(m_controlSelects[j] == c)
							{
								bFind	=	true;
								break;
							}
						}
						if(!bFind)
							m_controlSelects.push_back(c);
					}
				}
			}
			break;
		}
	}
	if(this->m_controlSelects.size() > 0)
		OnChangeSelect(false);
}

BOOL		GCViewUIEditor::IsAlignEnable()
{
	return (m_controlSelects.size() > 1);
}

INT			GCViewUIEditor::GetAlignValue(AlignType type)
{
	if(m_controlSelects.size() > 0)
	{
		Phantom::Rect rc	=	m_controlSelects[m_controlSelects.size() - 1]->GetRenderRect();
		switch(type)
		{
		case AlignType_Left:
			return rc.left;
		case AlignType_Top:
			return rc.top;
		case AlignType_Right:
			return rc.right;
		case AlignType_Bottom:
			return rc.bottom;
		case AlignType_Width:
			return rc.GetWidth();
		case AlignType_Height:
			return rc.GetHeight();
		case AlignType_SpaceX:
		case AlignType_SpaceY:
		case AlignType_MatrixX:
		case AlignType_MatrixY:
			{
				int spaceX	=	0;
				int spaceY	=	0;
				int maxX	=	-10000;
				int maxY	=	-10000;
				int minX	=	10000;
				int minY	=	10000;
				for(int i=0;i<m_controlSelects.size();i++)
				{
					Phantom::Rect rc	=	m_controlSelects[i]->GetRenderRect();
					spaceX	+=	rc.GetWidth();
					spaceY	+=	rc.GetHeight();
					maxX	=	max(maxX, rc.right);
					minX	=	min(minX, rc.left);
					maxY	=	max(maxY, rc.bottom);
					minY	=	min(minY, rc.top);
				}
				if(type == AlignType_SpaceX || type == AlignType_MatrixX)
					return (maxX - minX - spaceX) / (int)m_controlSelects.size();
				if(type == AlignType_SpaceY || type == AlignType_MatrixY)
					return (maxY - minY - spaceY) / (int)m_controlSelects.size();
			}
			break;
		}
	}
	return -1;
}

VOID		GCViewUIEditor::OnChangeSelect(BOOL bTree)
{
	if(m_controlSelects.size() > 0)
	{
		Phantom::Rect rc	=	m_controlSelects[m_controlSelects.size() - 1]->GetRenderRect();
		wchar_t buf[128];
		swprintf(buf, L"%d", rc.left);
		GetAlignDialog()->m_edAlignLeft.SetWindowTextW(buf);
		swprintf(buf, L"%d", rc.top);
		GetAlignDialog()->m_edAlignTop.SetWindowTextW(buf);
		//
		swprintf(buf, L"%d", rc.right);
		GetAlignDialog()->m_edAlignRight.SetWindowTextW(buf);
		//
		swprintf(buf, L"%d", rc.bottom);
		GetAlignDialog()->m_edAlignBottom.SetWindowTextW(buf);
		//
		swprintf(buf, L"%d", rc.GetWidth());
		GetAlignDialog()->m_edAlignWidth.SetWindowTextW(buf);
		//
		swprintf(buf, L"%d", rc.GetHeight());
		GetAlignDialog()->m_edAlignHeight.SetWindowTextW(buf);
		//
		int spaceX	=	0;
		int spaceY	=	0;
		int maxX	=	-10000;
		int maxY	=	-10000;
		int minX	=	10000;
		int minY	=	10000;
		for(int i=0;i<m_controlSelects.size();i++)
		{
			Phantom::Rect rc	=	m_controlSelects[i]->GetRenderRect();
			spaceX	+=	rc.GetWidth();
			spaceY	+=	rc.GetHeight();
			maxX	=	max(maxX, rc.right);
			minX	=	min(minX, rc.left);
			maxY	=	max(maxY, rc.bottom);
			minY	=	min(minY, rc.top);
		}
		//
		swprintf(buf, L"%d", (maxX - minX - spaceX) / (int)m_controlSelects.size());
		GetAlignDialog()->m_edAlignSpaceX.SetWindowTextW(buf);
		//
		swprintf(buf, L"%d", (maxY - minY - spaceY) / (int)m_controlSelects.size());
		GetAlignDialog()->m_edAlignSpaceY.SetWindowTextW(buf);
		//
		if(bTree)
		{
			this->m_treeView.ClearSelect();
			for(int i=0;i<this->m_controlSelects.size();i++)
				AddSelectToTree(m_controlSelects[i]);
			this->m_treeView.Invalidate();
			this->OnChangeTreeSelect();
		}
	}
}

VOID		GCViewUIEditor::OnChangeBodySelect(BOOL bTree)
{
#ifdef USE_BOX
	if(this->m_selectBodys.size() > 0)
	{
		if(bTree)
		{
			this->m_treeView.ClearSelect();
			for(int i=0;i<this->m_selectBodys.size();i++)
			{
				HTREEITEM hChild	=	this->m_treeView.GetChildItem(m_hBoxBodyList);
				while(hChild)
				{
					TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hChild));
					if(type)
					{
						if(m_selectBodys[i].bodyPtr)
						{
						if(type->type == UI_PROP_B2_BODY)
						{
							if(type->p1 == m_selectBodys[i].bodyPtr->m_bodyData.id)
							{
								m_treeView.AddSelect(hChild, TRUE);
								break;
							}
						}
						}
						else
						{
						}
					}
					hChild	=	this->m_treeView.GetNextSiblingItem(hChild);
				}
			}
			this->m_treeView.Invalidate();
			this->OnChangeTreeSelect();
		}
	}
#endif
}
HICON		GCViewUIEditor::GetViewerIcon()
{
	static HICON	hIcon	=	AfxGetApp()->LoadIcon(IDI_UI_FILE_ICON);
	static HICON	hIconR	=	AfxGetApp()->LoadIcon(IDI_Z_UI_FILE_ICON);
	if(this->m_bIsReadOnly)
		return hIconR;
	return hIcon;
}

VOID		GCViewUIEditor::SetCurFrame(float fFrame)
{
	TreeItemType* type	=	this->GetSelectedItem();
	if(!type)
	{
		SetCurrentProperties(&m_viewerProps, L"文件属性", NULL);
		return;
	}
	switch(type->type)
	{
	case UI_PROP_ANIMATION:
		{
			Phantom::UIControl* control	=	m_dialogPtr->GetControlPtrFromID(type->p1);
			Phantom::UIAnimationTrack* ani	=	control->GetTrackPtr(type->p2);
			if(control && ani)
			{
				if(control->IsPlayingTrack())
					m_animationTrack.SetPlayValue(ani->GetCurFrame(control->m_playdata));
				else
					ani->SetCurFrame(control->m_playdata, m_animationTrack.GetPlayValue());
			}
		}
		break;
	}
}
//extern	BOOL	GetModifyed();
//extern	VOID	SetModifyed(BOOL b);
VOID		GCViewUIEditor::CloseFile(){
	for(int i=0;i<g_copyUIs.size();i++)
	{
		if(g_copyUIs[i].editor == this)
		{
			g_copyUIs.erase(g_copyUIs.begin() + i);
			i--;
			break;
		}
	}
	safe_release(m_dialogPtr);
}
extern int	GetGameWidth();
extern int	GetGameHeight();
extern VOID MessageLowVersion(int result);
BOOL		GCViewUIEditor::OpenFile(const wchar_t* szFile)	//打开文件,如果失败则会自动删除
{
	this->m_strFile	=	szFile;
	if(!__super::OpenFile(szFile))
		return false;
	//::SetModifyed(false);
	if(m_viewerProps.m_hWnd == 0)
		this->m_treeView.InsertItem(Language(L"UIEditorTree", L"界面编辑工具"), 25, 25);
	if(m_strFile.empty())
	{
		Phantom::GetDlgManager()->CreateDlg(m_dialogPtr);
		this->SetModifyed(true);
	}
	else
	{
		Phantom::ImportResult result;
		if(!Phantom::GetDlgManager()->LoadDialog(m_dialogPtr, A(m_strFile.c_str()).buf, &result, 0))
		{
			if(result == Phantom::ImportResult_FailureAuth)
			{
				GetMainFrame()->MessageBox(Language(L"你目前还没有获得这个资源的授权,无法使用\n这个资源必须得获得授权才能使用!"), Language(L"提示"), MB_OK);
				return false;
			}
			MessageLowVersion(result);
			return false;
		}
		//for(int i=0;i<this->m_dialogPtr->GetControlCount();i++){
		//	Phantom::UIControl* c = m_dialogPtr->GetControlPtr(i);
		//	for(int j=0;j<c->GetElementCount();j++){
		//		Phantom::UIElement* e = c->GetElementPtr(j);
		//		e->SetTextureMode(e->GetTextureMode());
		//		for(int k=0;k<e->GetUVAnimationCount();k++){
		//			e->GetUVAnimationPtr(k)->RegetRects();
		//		}
		//	}
		//}
		//m_dialogPtr->SaveDialog(A(m_strFile.c_str()));
	}
		//m_dialogPtr	=	Phantom::GetDlgManager()->LoadDialog(A(m_strFile.c_str()), m_packagePtr);
	if(!m_dialogPtr)
		return false;
	//
	//if(::GetModifyed())
	//this->SetModifyed(true);
	if(m_viewerProps.m_hWnd == 0){
	m_trackCtrl.Create(m_trackCtrl.IDD, GetPropertiesDlgWindowPtr());
	//m_uiEventSet.Create(m_uiEventSet.IDD, GetPropertiesDlgWindowPtr());
	m_floatAni.Create(m_floatAni.IDD, GetPropertiesDlgWindowPtr());
	m_trackCtrl.ShowWindow(SW_HIDE);
	//m_uiEventSet.ShowWindow(SW_HIDE);
	m_floatAni.ShowWindow(SW_HIDE);
	//
	InitEffectProp();
	}
	if(m_dialogPtr->m_screenSize.cx != GetGameWidth()||m_dialogPtr->m_screenSize.cy != GetGameHeight())
	{
		m_dialogPtr->m_screenSize.Set(GetGameWidth(), GetGameHeight());
		this->SetModifyed(true);
	}
	//
	RebuildTrees();
	m_dialogPtr->m_screenSize.Set(GetGameWidth(), GetGameHeight());
	ResetCenter();
	return true;
}

VOID		GCViewUIEditor::GetNewFileName(wchar_t* szFile)
{
	wcscpy(szFile, Language(L"新界面"));
}

VOID		GCViewUIEditor::RebuildTrees()
{
	if(m_hViewItem)
	{
		m_treeView.DeleteItem(m_hViewItem);
		m_treeView.RemoveSelect(m_hViewItem);
	}
	wchar_t wFileName[256], wExt[256];
	_wsplitpath(m_strFile.c_str(), 0, 0, wFileName, wExt);
	wcscat(wFileName, wExt);
	if(!wFileName[0])
		GetNewFileName(wFileName);
	m_hViewItem = m_treeView.InsertItem(wFileName, 0, 0);
	m_treeView.SetItemState(m_hViewItem, TVIS_BOLD, TVIS_BOLD);
	m_treeView.Expand(m_hViewItem, TVE_EXPAND);
	//
	this->ClearTreeItem();
	//
	BuildAll();
	m_treeView.m_callback	=	this;
}

INT			GCViewUIEditor::GetControlIcon(INT nControlType)
{
	switch(nControlType)
	{
	case UIControlType_BUTTON://			/*控件类型_按钮*/=	1,
		return 1;
	case UIControlType_CHECKBOX://			/*控件类型_单选框*/=	2,
		return 12;
	case UIControlType_COMBOBOX://			/*控件类型_组合框*/=	4,
		return 7;
	case UIControlType_SLIDER://			/*控件类型_Slider*/=	5,
		return 14;
	case UIControlType_EDITBOX://			/*控件类型_编辑框*/=	6,
		return 16;
	case UIControlType_IMEEDITBOX://		/*控件类型_带有IME的编辑框*/=	7,
		return 16;
	case UIControlType_LISTBOX://			/*控件类型_列表框*/=	8,
		return 9;
	case UIControlType_SCROLLBAR://			/*控件类型_滚动条*/=	9,
		return 17;
	case UIControlType_PROGRESS://			/*控件类型_进度条*/=	10,
		return 10;
	case UIControlType_STATIC://			/*控件类型_文本框*/=	11,
		return 15;
//	case UIControlType_STATIC_ANIMATION://	/*控件类型_动画框*/=	12,
//		return 8;
	case UIControlType_FORMAT_EDITBOX://	/*控件类型_格式化编辑框*/=	14,	//格式化输入框
		return 13;
	case UIControlType_VIEW_FORMAT_TEXT://	/*控件类型_格式化文本框*/=	15,	//格式化显示框
		return 6;
	case UIControlType_LISTCTRL3D://		/*控件类型_行列框*/=	16,	//格式化显示框
		return 11;
//	case UIControlType_2DTerrain:
//		return 20;
//	case UIControlType_2DUnit:
//		return 21;
//	case UIControlType_2DEffect:
//		return 22;
	}
	return 0;
}

VOID		GCViewUIEditor::RefreshAnimations(Phantom::UIControl* ele, HTREEITEM hAnimationList)
{
	DeleteChildItems(hAnimationList);
	for(int j=0;j<ele->GetTrackCount();j++)
	{
		HTREEITEM hElement	=	m_treeView.InsertItem(Wutf8(ele->GetTrackPtr(j)->GetName()), 18, 18, hAnimationList);
		m_treeView.SetItemData(hElement, AddTreeItem(UI_PROP_ANIMATION, ele->GetID(), j));
	}
}

VOID		GCViewUIEditor::RefreshUVRects(Phantom::UIControl* ele, Phantom::UIElement* element, int eIndex, Phantom::UVAnimation* uvAni, int aIndex, HTREEITEM hUVAnimation)
{
	DeleteChildItems(hUVAnimation);
	if(!uvAni->IsCustomEnable())
		return;
	int nBegin = 0, nCount = 0;
	uvAni->GetUVMapIndex(nBegin, nCount);
	for(int j=0;j<nCount;j++)
	{
		const Phantom::TextureRect* rect	=	uvAni->GetTextureRect(j);
		char buf[128];
		if(rect->name.t[0])
			strcpy(buf, rect->name.t);
		else
			sprintf(buf, "%d", j+1);
		HTREEITEM hElement	=	m_treeView.InsertItem(Wutf8(buf), 4, 4, hUVAnimation);
		m_treeView.SetItemData(hElement, AddTreeItem(UI_PROP_UV_ANIMATION_RECT, ele->GetID(), eIndex, aIndex, j));
	}
}

struct	UVAnimationTemp
{
	int		actionType;
	std::vector<Phantom::UVAnimation*>	lists;
};

VOID		GCViewUIEditor::RefreshUVAnimations(Phantom::UIControl* ele, Phantom::UIElement* element, int eIndex, HTREEITEM hUVAnimationList)
{
	DeleteChildItems(hUVAnimationList);
	for(int j=0;j<element->GetUVAnimationCount();j++)
	{
		Phantom::UVAnimation* uvAni = element->GetUVAnimationPtr(j);
		wchar_t buf[128];
		if(ele->GetEnableUnitAngle())
			swprintf(buf, L"%s[%s:%s]", Wutf8(uvAni->GetName()).buf, g_typeUnitActionType[uvAni->GetActionType()], g_type2DUnitAngle[uvAni->GetAngle()]);
		else
			wcscpy(buf, Wutf8(uvAni->GetName()));
		HTREEITEM hElement	=	m_treeView.InsertItem(buf, 4, 4, hUVAnimationList);
		m_treeView.SetItemData(hElement, AddTreeItem(UI_PROP_UV_ANIMATION, ele->GetID(), eIndex, uvAni->GetID()));
		RefreshUVRects(ele, element, eIndex, uvAni, uvAni->GetID(), hElement);
	}
}

VOID		GCViewUIEditor::AddSelectToTree(Phantom::UIControl* ele)
{
	HTREEITEM hGroupItem	=	m_groupItems[ele->GetGroupID()];
	if(!hGroupItem)
		return;
	HTREEITEM hChild	=	this->m_treeView.GetChildItem(hGroupItem);
	while(hChild)
	{
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hChild));
		if(type)
		{
			if(type->type == UI_PROP_CONTROL)
			{
				if(type->p1 == ele->GetID())
				{
					if(m_bSelectElementMode)
					{
						HTREEITEM hElement	=	m_treeView.GetChildItem(hChild);
						while(hElement)
						{
							m_treeView.AddSelect(hElement, TRUE);
							hElement	=	m_treeView.GetNextSiblingItem(hElement);
						}
					}
					else
						m_treeView.AddSelect(hChild, TRUE);
					break;
				}
			}
		}
		hChild	=	this->m_treeView.GetNextSiblingItem(hChild);
	}
}
const wchar_t*	GetElementName(int type, int index){
	const wchar_t* str = L"default";
	switch(type){
	case Phantom::UIControlType_BUTTON:
		if(index==0)str= L"按钮[正常]";
		if(index==1)str= L"按钮[按下]";
		//if(index==2)str= L"按钮[释放]";
		if(index==2)str= L"按钮[无效]";
		if(index==3)str= L"按钮[图片文本]";
		break;
	case Phantom::UIControlType_CHECKBOX:
		if(index==0)str= L"选择框[框]";
		if(index==1)str= L"选择框[按下]";
		if(index==2)str= L"选择框[选中]";
		if(index==3)str= L"选择框[无效]";
		if(index==4)str= L"选择框[背景]";
		break;
	case Phantom::UIControlType_COMBOBOX:
		if(index==0)str= L"组合框[底]";
		if(index==1)str= L"组合框[按钮]";
		if(index==2)str= L"组合框[下拉框]";
		if(index==3)str= L"组合框[选择条]";
		break;
	case Phantom::UIControlType_SLIDER:
		if(index==0)str= L"轨道条[条]";
		if(index==1)str= L"轨道条[按钮]";
		break;
	case Phantom::UIControlType_EDITBOX:
		if(index==0)str= L"背景";
		break;
	case Phantom::UIControlType_LISTBOX:
		if(index==0)str= L"列表框[底]";
		if(index==1)str= L"列表框[选中条]";
		break;
	case Phantom::UIControlType_SCROLLBAR:
		if(index==0)str= L"滚动框[条]";
		if(index==1)str= L"滚动框[值]";
		break;
	case Phantom::UIControlType_PROGRESS:
		if(index==0)str= L"进度条[背景]";
		if(index==1)str= L"进度条[值]";
		break;
	case Phantom::UIControlType_STATIC:
		if(index==0)str= L"背景";
		break;
	case Phantom::UIControlType_LISTCTRL3D:
		if(index==0)str= L"列表框[底]";
		if(index==1)str= L"列表框[选中条]";
		if(index==2)str= L"列表框[表头按钮]";
		if(index==3)str= L"列表框[排序向上]";
		if(index==4)str= L"列表框[排序向下]";
		break;
	case Phantom::UIControlType_ui2DEffect:
		if(index==0)str= L"效果图片";
		break;
	}
	return Language(str, str);
}
VOID		GCViewUIEditor::RefreshElement(Phantom::UIControl* ele, HTREEITEM hImageList)
{
	this->DeleteChildItems(hImageList);
	for(int j=0;j<ele->GetElementCount();j++)
	{
		HTREEITEM hElement	=	m_treeView.InsertItem(GetElementName(ele->GetType(), j), 18, 18, hImageList);
		m_treeView.SetItemData(hElement, AddTreeItem(UI_PROP_ELEMENTS, ele->GetID(), j));
		RefreshUVAnimations(ele, ele->GetElementPtr(j), j, hElement);
	}
}

VOID		GCViewUIEditor::BeginUndoFloat(Phantom::FloatAnimation* a1, Phantom::FloatAnimation* a2, Phantom::FloatAnimation* a3)
{
	//this->BeginUndo();
	//UndoPtr uptr;
	//uptr.buff = AddUndoRedo(m_trackType, (ProcUndoRedo)0);
	//uptr.urMode = UndoRedoBuffer::URMode_Add;
	//uptr.buff->m_id		=	m_trackP1;
	//uptr.buff->m_id2	=	m_trackP2;
	//uptr.buff->m_id3	=	m_trackP3;
	//uptr.buff->m_id4	=	m_trackP4;
	//if(a1){
	//	if(!uptr.buff->m_oldAni[0])
	//		uptr.buff->m_oldAni[0] = new Phantom::FloatAnimation();
	//	*uptr.buff->m_oldAni[0] = *a1;
	//}
	//if(a2){
	//	if(!uptr.buff->m_oldAni[1])
	//		uptr.buff->m_oldAni[1] = new Phantom::FloatAnimation();
	//	*uptr.buff->m_oldAni[1] = *a2;
	//}
	//if(a3){
	//	if(!uptr.buff->m_oldAni[2])
	//		uptr.buff->m_oldAni[2] = new Phantom::FloatAnimation();
	//	*uptr.buff->m_oldAni[2] = *a3;
	//}
}
VOID		GCViewUIEditor::EndUndoFloat(Phantom::FloatAnimation* a1, Phantom::FloatAnimation* a2, Phantom::FloatAnimation* a3)
{
	//if(!m_currBuffer)return;
	//UndoPtr uptr;
	//uptr.buff = m_currBuffer;//AddUndoRedo(m_trackType, (ProcUndoRedo)0);
	//if(a1){
	//	if(!uptr.buff->m_newAni[0])
	//		uptr.buff->m_newAni[0] = new Phantom::FloatAnimation();
	//	uptr.buff->m_aniPtr[0] = a1;
	//	*uptr.buff->m_newAni[0] = *a1;
	//}
	//if(a2){
	//	if(!uptr.buff->m_newAni[1])
	//		uptr.buff->m_newAni[1] = new Phantom::FloatAnimation();
	//	uptr.buff->m_aniPtr[1] = a2;
	//	*uptr.buff->m_newAni[1] = *a2;
	//}
	//if(a3){
	//	if(!uptr.buff->m_newAni[2])
	//		uptr.buff->m_newAni[2] = new Phantom::FloatAnimation();
	//	uptr.buff->m_aniPtr[3] = a3;
	//	*uptr.buff->m_newAni[2] = *a3;
	//}
	//m_currBuffer = 0;
}
VOID		GCViewUIEditor::OnUndoRedo(UndoRedoBuffer* buffer, int ur_mode)
{
	UndoPtr uptr;
	uptr.buff = buffer;
	uptr.urMode = ur_mode;
	switch(buffer->type){
	case UI_PROP_CONTROL:
		{
			ProcControl3D proc = (ProcControl3D)buffer->proc;
			Phantom::UIControl* data = this->m_dialogPtr->FindControlByID(buffer->m_id);
			if(proc&&data)
				(*proc)(data, 0, 0, &uptr);
		}
		break;
	case UI_PROP_MOVE_CONTROL:
		{
			int size = 0;
			(*buffer) >> size;
			for(int i=0;i<size;i++){
				CRect rc,newrc;
				CPoint cent,newcent;
				int id = 0;
				(*buffer) >> id >> rc >> cent >> newrc >> newcent;
				Phantom::UIControl* data = this->m_dialogPtr->FindControlByID(id);
				if(data){
					if(ur_mode==UndoRedoBuffer::URMode_Undo){
						data->SetWindowRect((Phantom::Rect&)rc);
						data->SetRotCenter((Phantom::Pixel&)cent);
					}
					else{
						data->SetWindowRect((Phantom::Rect&)newrc);
						data->SetRotCenter((Phantom::Pixel&)newcent);
					}
				}
			}
		}
		break;
	case UI_PROP_GROUP:
		{
			ProcGroup3D proc = (ProcGroup3D)buffer->proc;
			Phantom::UIGroup* data = this->m_dialogPtr->FindGroupPtr(buffer->m_id);
			if(proc&&data)
				(*proc)(data, 0, 0, &uptr);
		}
		break;
	case UI_PROP_MAIN:
		{
			ProcDialog3D proc = (ProcDialog3D)buffer->proc;
			if(proc)
				(*proc)(m_dialogPtr, 0, 0, &uptr);
		}
		break;
	case UI_PROP_ANIMATION:
		{
			ProcControlAnimation3D proc = (ProcControlAnimation3D)buffer->proc;
			Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(buffer->m_id);
			if(!control)break;
			Phantom::UIAnimationTrack* e		=	control->GetTrackPtr(buffer->m_id2);
			if(proc)
				(*proc)(control, e, 0, 0, &uptr);
		}
		break;
	case UI_PROP_UV_ANIMATION_RECT:
		{
			ProcControlUVAnimationR proc = (ProcControlUVAnimationR)buffer->proc;
			Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(buffer->m_id);
			if(!control)break;
			Phantom::UIElement* e		=	control->GetElementPtr(buffer->m_id2);
			if(e)
			{
				Phantom::UVAnimation* eUV	=	GetUVAniByID(e, buffer->m_id3);
				if(eUV)
				{
					(*proc)(control, e, eUV, buffer->m_id4, 0, 0, &uptr);
				}
			}
		}
		break;
	case UI_PROP_UV_ANIMATION:
		{
			ProcControlUVAnimation proc = (ProcControlUVAnimation)buffer->proc;
			Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(buffer->m_id);
			if(!control)break;
			Phantom::UIElement* e		=	control->GetElementPtr(buffer->m_id2);
			if(e)
			{
				Phantom::UVAnimation* eUV	=	GetUVAniByID(e, buffer->m_id3);
				if(eUV)
				{
					(*proc)(control, e, eUV, 0, 0, &uptr);
					SetModifyed();
				}
			}
		}
		break;
	case UI_PROP_ELEMENTS:
		{
			ProcControlElement3D proc = (ProcControlElement3D)buffer->proc;
			Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(buffer->m_id);
			if(!control)break;
			Phantom::UIElement* e		=	control->GetElementPtr(buffer->m_id2);
			if(e)
				(*proc)(control, e, 0, 0, &uptr);
		}
		break;
	//case GCViewUIEditor::UI_PROP_ANIMATION_FLOAT:
	//	{
	//		Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(buffer->m_id);
	//		if(!control)break;
	//		Phantom::UIAnimationTrack* e		=	control->GetTrackPtr(buffer->m_id2);
	//		if(e)
	//		{
	//			for(int i=0;i<3;i++){
	//				for(int j=0;j<Phantom::ControlTrackType_Count;j++){
	//					if(e->GetAnimationPtr((Phantom::ControlTrackType)j)==buffer->m_aniPtr[i]){
	//						if(ur_mode==UndoRedoBuffer::URMode_Undo)
	//							(*e->GetAnimationPtr((Phantom::ControlTrackType)j)) = *buffer->m_oldAni[i];
	//						else
	//							(*e->GetAnimationPtr((Phantom::ControlTrackType)j)) = *buffer->m_newAni[i];
	//					}
	//				}
	//			}
	//		}
	//	}
	//	break;
	}
	this->SetModifyed();
}
VOID		GCViewUIEditor::RefreshEvents(Phantom::UIControl* ele, HTREEITEM hImageList)
{
	//this->DeleteChildItems(hImageList);
	//for(int j=0;j<ele->GetEventCount();j++)
	//{
	//	HTREEITEM hElement	=	m_treeView.InsertItem(GetEventName(ele->GetEventPtr(j)->GetObjectID().eventID), 32, 32, hImageList);
	//	m_treeView.SetItemData(hElement, AddTreeItem(UI_PROP_EVENT, ele->GetID(), ele->GetEventPtr(j)->GetID()));
	//}
}

VOID		GCViewUIEditor::RefreshControl(Phantom::UIControl* ele, HTREEITEM hGeomOne)
{
	this->DeleteChildItems(hGeomOne);
	if(ele->GetHideOnEditor())
		return;
	if(ele->GetElementCount() > 0)
	{
		for(int j=0;j<ele->GetElementCount();j++)
		{
			HTREEITEM hElement	=	m_treeView.InsertItem(GetElementName(ele->GetType(), j), 18, 18, hGeomOne);
			m_treeView.SetItemData(hElement, AddTreeItem(UI_PROP_ELEMENTS, ele->GetID(), j));
			RefreshUVAnimations(ele, ele->GetElementPtr(j), j, hElement);
		}
	}
	{
		//HTREEITEM hImageList	=	m_treeView.InsertItem(Language(L"动作列表"), 2, 2, hGeomOne);
		//m_treeView.SetItemData(hImageList, AddTreeItem(UI_PROP_ANIMATION_LIST, ele->GetID()));
		//RefreshAnimations(ele, hImageList);
		for(int j=0;j<ele->GetTrackCount();j++)
		{
			HTREEITEM hElement	=	m_treeView.InsertItem(Wutf8(ele->GetTrackPtr(j)->GetName()), 4, 4, hGeomOne);
			m_treeView.SetItemData(hElement, AddTreeItem(UI_PROP_ANIMATION, ele->GetID(), j));
		}
	}
	//if(ele->GetEventCount() > 0)
	//{
	//	HTREEITEM hImageList	=	m_treeView.InsertItem(Language(L"事件列表"), 2, 2, hGeomOne);
	//	m_treeView.SetItemData(hImageList, AddTreeItem(UI_PROP_EVENT_LIST, ele->GetID()));
	//	RefreshEvents(ele, hImageList);
	//}
}

VOID		GCViewUIEditor::RefreshGroup(int nGroup)
{
	m_treeView.ClearSelect();
	g_copyUIs.clear();
	HTREEITEM hGroupItem	=	m_groupItems[nGroup];
	assert(hGroupItem);
	if(!hGroupItem)
		return;
	DeleteChildItems(hGroupItem);
	if(nGroup <= 0)
	{
		for(int iGeom=0;iGeom<m_dialogPtr->GetControlCount();iGeom++)
		{
			Phantom::UIControl* ele		=	m_dialogPtr->GetControlPtr(iGeom);
			if(!ele)
				continue;
			Phantom::UIGroup* g			=	ele->GetGroupPtr();
			if(g && g->GetID() > 0)
				continue;
			if(ele->GetGroupID() <= 0)
			{
				INT nIcon	=	GetControlIcon(ele->GetType());
				HTREEITEM hGeomOne	=	m_treeView.InsertItem(Wutf8(ele->GetName()), nIcon, nIcon, hGroupItem);
				m_treeView.SetItemData(hGeomOne, AddTreeItem(UI_PROP_CONTROL, ele->GetID()));
				//
				RefreshControl(ele, hGeomOne);
			}
		}
		return;
	}
	Phantom::UIGroup* g	=	m_dialogPtr->FindGroupPtr(Phantom::PtrID(0, nGroup));
	if(!g)
		return;
	for(int iGeom=0;iGeom<g->GetControlCount();iGeom++)
	{
		Phantom::UIControl* ele		=	g->GetControlPtr(iGeom);
		if(!ele)
			continue;
		INT nIcon	=	GetControlIcon(ele->GetType());
		HTREEITEM hGeomOne	=	m_treeView.InsertItem(Wutf8(ele->GetName()), nIcon, nIcon, hGroupItem);
		m_treeView.SetItemData(hGeomOne, AddTreeItem(UI_PROP_CONTROL, ele->GetID()));
		//
		RefreshControl(ele, hGeomOne);
	}
}

int			SortUIGroup(const void* v1, const void* v2)
{
	Phantom::UIGroup* g1	=	*(Phantom::UIGroup**)v1;
	Phantom::UIGroup* g2	=	*(Phantom::UIGroup**)v2;
	return stricmp(g1->GetName(), g2->GetName());
}

VOID		GCViewUIEditor::BuildAll()
{
	m_treeView.ClearSelect();
	m_treeView.AddSelect(m_hViewItem);
	DeleteChildItems(m_hViewItem);
	m_treeView.SetItemData(m_hViewItem, AddTreeItem(UI_PROP_MAIN));
	m_groupItems.clear();
	/*
	HTREEITEM hAction	=	m_treeView.InsertItem(Language(L"动作播放列表"), 2, 2, m_hViewItem);
	m_treeView.SetItemData(hAction, AddTreeItem(UI_PROP_ACTION_PLAY));
	for(int i=0;i<g_typeUnitActionType.m_options.size();i++)
	{
		HTREEITEM hActionL	=	m_treeView.InsertItem(g_typeUnitActionType.m_options[i].text, 18, 18, hAction);
		m_treeView.SetItemData(hActionL, AddTreeItem(UI_PROP_ACTION_TYPE, g_typeUnitActionType.m_options[i].index));
	}
	*/
	m_groupItems.clear();
	std::vector<Phantom::UIGroup*> groups;
	for(int i=0;i<m_dialogPtr->GetGroupCount();i++)
		groups.push_back(m_dialogPtr->GetGroupPtr(i));
	if(groups.size() > 0)
	{
		if(groups.size() > 1)
			qsort(&groups.at(0), groups.size(), sizeof(Phantom::UIGroup*), SortUIGroup);
		for(int i=0;i<groups.size();i++)
		{
			wchar_t gname[256];
			wcscpy(gname, Wutf8(groups[i]->GetName()));
			//if(i == 0)
				//wcscpy(gname, Language(L"[默认组]"));
			HTREEITEM hGroupItem	=	m_treeView.InsertItem(gname, 19, 19, m_hViewItem);
			m_treeView.SetItemData(hGroupItem, AddTreeItem(UI_PROP_GROUP, groups[i]->GetID()));
			m_groupItems[groups[i]->GetID()]	=	hGroupItem;
			//
			RefreshGroup(groups[i]->GetID());
		}
	}
#ifdef USE_BOX
	//
	m_hBoxBodyList	=	m_treeView.InsertItem(Language(L"2D物理:对象列表"), 2, 2, m_hViewItem);
	m_treeView.SetItemData(m_hBoxBodyList, AddTreeItem(UI_PROP_BOX2D_BODY));
	m_hBoxJointList	=	m_treeView.InsertItem(Language(L"2D物理:控制器列表"), 2, 2, m_hViewItem);
	m_treeView.SetItemData(m_hBoxJointList, AddTreeItem(UI_PROP_BOX2D_JOINT));
	//
	RefreshBoxBodys();
	RefreshBoxJoints();
#endif
}
#ifdef USE_BOX
VOID		GCViewUIEditor::RefreshBoxBodys()
{
	DeleteChildItems(m_hBoxBodyList);
	if(!m_dialogPtr->m_box2World.IsEnable())
		return;
	for(int i=0;i<m_dialogPtr->m_box2World.GetBodyCount();i++)
	{
		if(!m_dialogPtr->m_box2World.GetBodyPtr(i)->m_bodyData.name.t[0])
			sprintf(m_dialogPtr->m_box2World.GetBodyPtr(i)->m_bodyData.name.t, "body%d", m_dialogPtr->m_box2World.GetBodyPtr(i)->m_bodyData.id);
		HTREEITEM hBody	=	m_treeView.InsertItem(W(m_dialogPtr->m_box2World.GetBodyPtr(i)->m_bodyData.name.t), 2, 2, m_hBoxBodyList);
		m_treeView.SetItemData(hBody, AddTreeItem(UI_PROP_B2_BODY, m_dialogPtr->m_box2World.GetBodyPtr(i)->m_bodyData.id));
		RefreshBoxShapes(hBody, m_dialogPtr->m_box2World.GetBodyPtr(i));
	}
}
VOID		GCViewUIEditor::RefreshBoxJoints()
{
	DeleteChildItems(m_hBoxJointList);
	if(!m_dialogPtr->m_box2World.IsEnable())
		return;
	for(int i=0;i<m_dialogPtr->m_box2World.GetJointCount();i++)
	{
		if(!m_dialogPtr->m_box2World.GetJointPtr(i)->m_data.name.t[0])
			sprintf(m_dialogPtr->m_box2World.GetJointPtr(i)->m_data.name.t, "joint%d", m_dialogPtr->m_box2World.GetJointPtr(i)->m_data.id);
		HTREEITEM hBody	=	m_treeView.InsertItem(W(m_dialogPtr->m_box2World.GetJointPtr(i)->m_data.name.t), 2, 2, m_hBoxJointList);
		m_treeView.SetItemData(hBody, AddTreeItem(UI_PROP_B2_JOINT, m_dialogPtr->m_box2World.GetJointPtr(i)->m_data.id));
	}
}
const wchar_t*	GetShapeTypeName(int n)
{
	switch(n)
	{
	case b2Shape::e_circle:
		return L"circle";
	case b2Shape::e_edge:
		return L"line";
	case b2Shape::e_polygon:
		return L"polygon";
	case b2Shape::e_chain:
		return L"chain";
	}
	return L"";
}
VOID		GCViewUIEditor::RefreshBoxShapes(HTREEITEM hItem, Phantom::B2Body* b)
{
	DeleteChildItems(hItem);
	if(!m_dialogPtr->m_box2World.IsEnable())
		return;
	for(int i=0;i<b->GetShapeCount();i++)
	{
		wchar_t buf[128];
		swprintf(buf, L"%s%d", GetShapeTypeName(b->GetShapePtr(i)->m_data.type), b->GetShapePtr(i)->m_data.id);
		HTREEITEM hBody	=	m_treeView.InsertItem(buf, 2, 2, hItem);
		m_treeView.SetItemData(hBody, AddTreeItem(UI_PROP_B2_SHAPE, b->m_bodyData.id, b->GetShapePtr(i)->m_data.id));
	}
}
#endif
//
BOOL g_bUsePhantom3D = false;
BOOL IsUsePhantom3D()
{
	return g_bUsePhantom3D;
}
VOID	UsePhantom3D(BOOL b){
	g_bUsePhantom3D = b;
}
VOID		GCViewUIEditor::OnActive()		//被激活
{
	g_bUsePhantom3D = true;
	if(this->m_dialogPtr)
		m_dialogPtr->SetVisible(true);
	__super::OnActive();
}

VOID		GCViewUIEditor::OnDeActive()		//失去激活
{
	g_bUsePhantom3D = false;
	if(this->m_dialogPtr && !m_bAlwaysVisible)
		m_dialogPtr->SetVisible(false);
	if(m_dialogPtr)
		GetCodeXmlManager()->AddUIClass(m_dialogPtr);
	__super::OnDeActive();
}

void		GCViewUIEditor::OnFrameMove()	//每次更新
{
	//if(m_dialogPtr->m_box2World.IsEnable())
		//m_dialogPtr->m_box2World.OnFrameMove();
	this->SetCurFrame(m_animationTrack.GetPlayValue());
}

void		GCViewUIEditor::OnPushRenderObjects()	//渲染
{
}
VOID		GCViewUIEditor::ExportPak()
{
	CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, formatw(L"%s|*.pakui||", Language(L"UIFileTypePak", L"界面文件包导出") ), GetMainFrame());
	wchar_t buffer[1024];
	dlg.m_pOFN->lpstrInitialDir	=	AToW(GetProjectPath(), buffer, 1024);
	if(dlg.DoModal() != IDOK)
		return;
	SetProjectPath();
	wchar_t szPath[256], szFile[256], szDir[256];
	_wsplitpath(dlg.GetPathName(), szPath, szDir, szFile, 0);
	wcscat(szPath, szDir);
	wcscat(szPath, szFile);
	wcscat(szPath, L".pakui");
	this->m_dialogPtr->ExportPak(A(szPath));
}
namespace Phantom{
	extern	BOOL	SaveScreenTo(const char* szFile);
};
VOID		SaveSmallImage(const wchar_t* szImageFile){
	SaveScreenTo(A(szImageFile).buf);
	CImage img;
	if(SUCCEEDED(img.Load(szImageFile))){
		CImage imgSmall;
		imgSmall.Create(128, 128, 24, 0);
		int s = img.GetWidth();
		if(s>img.GetHeight())s=img.GetHeight();
		HDC dcSrc = img.GetDC();
		HDC dcDest = imgSmall.GetDC();
		::SetStretchBltMode(dcSrc, COLORONCOLOR);
		::SetStretchBltMode(dcDest, COLORONCOLOR);
		StretchBlt(dcDest, 0, 0, 128, 128, dcSrc, (img.GetWidth()-s)/2, (img.GetHeight()-s)/2, s, s, SRCCOPY);
		img.ReleaseDC();
		imgSmall.ReleaseDC();
		imgSmall.Save(szImageFile);
	}
}
BOOL		GCViewUIEditor::SaveFile(BOOL bSaveAs)	//保存文件
{
	if(m_dialogPtr->m_pakdir)
		return false;
	bool bNew	=	false;
	if(bSaveAs || m_strFile.size() == 0)
	{
		bNew	=	true;
		CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, formatw(L"%s|*.rpgui|%s|*.pakui||", Language(L"UIFileType", L"界面文件"), Language(L"UIFileTypePak", L"界面文件包导出") ), GetMainFrame());
		wchar_t buffer[1024];
		dlg.m_pOFN->lpstrInitialDir	=	AToW(GetProjectPath(), buffer, 1024);
		if(dlg.DoModal() != IDOK)
			return false;
		SetProjectPath();
		wchar_t szPath[256], szFile[256], szDir[256];
		_wsplitpath(dlg.GetPathName(), szPath, szDir, szFile, 0);
		wcscat(szPath, szDir);
		wcscat(szPath, szFile);
		if(dlg.m_pOFN->nFilterIndex==2){
			wchar_t szImagePath[256];
			wcscpy(szImagePath, szPath);
			wcscat(szImagePath, L".jpg");
			wcscat(szPath, L".pakui");
			this->m_dialogPtr->ExportPak(A(szPath));
			SaveSmallImage(szImagePath);
			return true;
		}
		wcscat(szPath, L".rpgui");
		m_strFile	=	szPath;
		wcscat(szFile, L".rpgui");
		this->m_treeView.SetItemText(this->m_hViewItem, szFile);
	}
	if(IsModifyed() || bSaveAs)
	{
		//PSize size(g_nGameWidth, g_nGameHeight);//Phantom::g_manager.GetBufferWidth(), Phantom::g_manager.GetBufferHeight());
		//m_dialogPtr->SetScreenSize(size);
		m_dialogPtr->m_screenSize.Set(GetGameWidth(), GetGameHeight());
		this->m_dialogPtr->SaveDialog(A(m_strFile.c_str()));
	}
	return __super::SaveFile(bSaveAs);
}

void		GCViewUIEditor::OnUpdateUI(INT nMenuID, CCmdUI* pCmdUI)
{
	switch(nMenuID)
	{
	case ID_UI_COPY:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == this->UI_PROP_ANIMATION || type->type == this->UI_PROP_CONTROL || type->type == this->UI_PROP_UV_ANIMATION || type->type == this->UI_PROP_GROUP));
		}
		break;
	case ID_UI_PASTE:
		{
			pCmdUI->Enable(false);
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
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(IsCanPaste(type->type))
				{
					pCmdUI->Enable(true);
					return;
				}
			}
		}
		break;
	case ID_UI_INSERTANIMATION:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == UI_PROP_ANIMATION_LIST || type->type == UI_PROP_CONTROL));
		}
		break;
	case ID_UI_INSERTBODY:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == UI_PROP_BOX2D_BODY));
		}
		break;
	case ID_UI_INSERTJOINT:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == UI_PROP_BOX2D_JOINT));
		}
		break;
	case ID_UI_INSERTSHAPE:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == UI_PROP_B2_BODY));
		}
		break;
	case ID_INSERT_UIGROUP:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == UI_PROP_MAIN));
		}
		break;
	case ID_SEL_ALL_ELEMENT:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == UI_PROP_GROUP));
		}
		break;
	case ID_UI_HIDE_CONTROLE:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == UI_PROP_CONTROL));
			Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
			if(control)
				pCmdUI->SetCheck(control->GetHideOnEditor());
		}
		break;
	case ID_AUTO_GENERIC_UVMAP:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == this->UI_PROP_ELEMENTS));
		}
		break;
	case ID_UI_CLONETRACK:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == this->UI_PROP_ANIMATION));
		}
		break;
	case ID_UI_INSERTUVANIMATION:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == this->UI_PROP_ELEMENTS));
		}
		break;
	case ID_UI_CLONEUVMAP:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == this->UI_PROP_UV_ANIMATION));
		}
		break;
	case ID_UI_REMOVECONTROL:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == this->UI_PROP_CONTROL));
		}
		break;
	case ID_UI_REMOVEANIMATION:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == this->UI_PROP_ANIMATION));
		}
		break;
	case ID_UI_REMOVEUVANIMATION:
		{
			HTREEITEM hItem		=	m_treeView.GetSelectedItem();
			if(!hItem)
			{
				pCmdUI->Enable(false);
				return;
			}
			TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
			pCmdUI->Enable(type && (type->type == this->UI_PROP_UV_ANIMATION));
		}
		break;
	default:
		pCmdUI->Enable(false);
		break;
	}
}


BOOL		GCViewUIEditor::IsCopyTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type && (type->type == this->UI_PROP_ANIMATION || type->type == this->UI_PROP_CONTROL || type->type == this->UI_PROP_UV_ANIMATION || type->type == this->UI_PROP_GROUP || type->type == this->UI_PROP_ELEMENTS))
			return true;
	}
	return false;
}

BOOL		GCViewUIEditor::IsPasteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(IsCanPaste(type->type))
				return true;
		}
	}
	return false;
}

struct	RefreshCEUV
{
	Phantom::UIControl* c;
	Phantom::UIElement* e;
	int element;
};
VOID		GCViewUIEditor::OnDeleteTree()
{
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
	std::vector<ElementUVRemove> removes;
	std::map<Phantom::UIGroup*, HTREEITEM>	removeGroups;
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		ElementUVRemove e;
		memset(&e, 0, sizeof(ElementUVRemove));
		//
		e.type		=	type->type;
		if(type->type == UI_PROP_CONTROL)
		{
			Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
			if(!control)
				continue;
			e.controlID	=	type->p1;
			e.hItem		=	it->first;
			removes.push_back(e);
		}
		else if(type->type == UI_PROP_GROUP)
		{
			Phantom::UIGroup* g	=	m_dialogPtr->FindGroupPtr(Phantom::PtrID(0, type->p1));
			if(g && g->GetID() > 0)
				removeGroups[g] = it->first;
			//for(int i=0;i<m_dialogPtr->GetControlCount();i++)
			//{
			//	Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtr(i);
			//	if(!control || control->GetGroupID() != type->p1)
			//		continue;
			//	e.controlID	=	control->GetID();
			//	removes.push_back(e);
			//}
		}
		else if(e.type == UI_PROP_B2_BODY)
		{
			e.controlID	=	type->p1;
			e.hItem		=	it->first;
			removes.push_back(e);
		}
		else if(e.type == UI_PROP_B2_SHAPE)
		{
			e.controlID	=	type->p1;
			e.p2		=	type->p2;
			e.hItem		=	it->first;
			removes.push_back(e);
		}
		else if(e.type == UI_PROP_B2_JOINT)
		{
			e.controlID	=	type->p1;
			e.hItem		=	it->first;
			removes.push_back(e);
		}
		else if(type->type == UI_PROP_ANIMATION)
		{
			Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
			if(!control)
				continue;
			e.controlID	=	type->p1;
			e.track		=	control->GetTrackPtr(type->p2);
			e.hItem		=	it->first;
			removes.push_back(e);
		}
		else if(type->type == UI_PROP_UV_ANIMATION)
		{
			Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
			if(!control)
				continue;
			Phantom::UIElement* element	=	control->GetElementPtr(type->p2);
			if(!element)
				continue;
			e.controlID	=	type->p1;
			e.element	=	type->p2;
			e.hItem		=	it->first;
			e.uvAni		=	GetUVAniByID(element, type->p3);
			removes.push_back(e);
		}
	}
	if(removes.size() > 0 || removeGroups.size() > 0)
	{
		if(MessageBox(this->GetViewHandle(), Language(L"是否删除所有选中项?"), Language(L"提示"), MB_YESNO) == IDNO)
			return;
		std::map<int,bool> groups;
		std::map<HTREEITEM,RefreshCEUV> elements;
		for(int i=0;i<removes.size();i++)
		{
			ElementUVRemove& e = removes[i];
			if(e.type == UI_PROP_CONTROL)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(e.controlID);
				if(!control)
					continue;
				groups[control->GetGroupID()] = true;
				m_dialogPtr->DeleteControlPtr(control);
				this->m_treeView.DeleteItem(e.hItem);
				m_treeView.RemoveSelect(e.hItem);
				m_animationTrack.SetNull();
				this->SetModifyed(true);
			}
			//else if(e.type == UI_PROP_GROUP)
			//{
			//	Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(e.controlID);
			//	if(!control)
			//		continue;
			//	groups[control->GetGroupID()] = true;
			//	m_dialogPtr->DeleteControlPtr(control);
			//	this->SetModifyed(true);
			//}
			else if(e.type == UI_PROP_ANIMATION)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(e.controlID);
				if(!control)
					continue;
				m_animationTrack.SetNull();
				control->RemoveTrack(e.track);
				this->m_treeView.DeleteItem(e.hItem);
				m_treeView.RemoveSelect(e.hItem);
				//RefreshControl(control, m_treeView.GetParentItem(e.hItem));
				//RefreshAnimations(control, m_treeView.GetParentItem(e.hItem));
				this->SetModifyed(true);
			}
#ifdef USE_BOX
			else if(e.type == UI_PROP_B2_BODY)
			{
				Phantom::B2Body* body	=	m_dialogPtr->m_box2World.GetBodyPtr(Phantom::PtrID(0, e.controlID));
				if(body)
					delete body;
				this->m_treeView.DeleteItem(e.hItem);
				m_treeView.RemoveSelect(e.hItem);
				this->SetModifyed(true);
			}
			else if(e.type == UI_PROP_B2_SHAPE)
			{
				Phantom::B2Body* body	=	m_dialogPtr->m_box2World.GetBodyPtr(Phantom::PtrID(0, e.controlID));
				if(body)
				{
					Phantom::B2Shape* s	=	body->GetShapePtr(Phantom::PtrID(0, e.p2));
					if(s)
						delete s;
				}
				this->m_treeView.DeleteItem(e.hItem);
				m_treeView.RemoveSelect(e.hItem);
				this->SetModifyed(true);
			}
			else if(e.type == UI_PROP_B2_JOINT)
			{
				Phantom::B2Joint* joint	=	m_dialogPtr->m_box2World.GetJointPtr(Phantom::PtrID(0, e.controlID));
				if(joint)
					delete joint;
				this->m_treeView.DeleteItem(e.hItem);
				m_treeView.RemoveSelect(e.hItem);
				this->SetModifyed(true);
			}
#endif
			else if(e.type == UI_PROP_UV_ANIMATION)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(e.controlID);
				if(!control)
					continue;
				Phantom::UIElement* element	=	control->GetElementPtr(e.element);
				if(!element)
					continue;
				RefreshCEUV u;
				u.c	=	control;
				u.element	=	e.element;
				u.e	=	element;
				element->RemoveUVAnimation(e.uvAni);
				this->m_treeView.DeleteItem(e.hItem);
				m_treeView.RemoveSelect(e.hItem);
				//elements[m_treeView.GetParentItem(e.hItem)]	=	u;
				//RefreshUVAnimations(control, element, e.element, m_treeView.GetParentItem(e.hItem));
				this->SetModifyed(true);
			}
			else if(e.type == UI_PROP_ELEMENTS)
			{
			}
		}
		m_treeView.ClearSelect();
		for(std::map<HTREEITEM,RefreshCEUV>::iterator it = elements.begin(); it != elements.end(); it++)
		{
			RefreshUVAnimations(it->second.c, it->second.e, it->second.element, it->first);
		}
		for(std::map<Phantom::UIGroup*, HTREEITEM>::iterator it = removeGroups.begin(); it != removeGroups.end(); it++)
		{
			this->m_groupItems[it->first->GetID()] = 0;
			this->m_dialogPtr->RemoveGroupPtr(Phantom::PtrID(0, it->first->GetID()));
			this->m_treeView.DeleteItem(it->second);
			SetModifyed(true);
		}
		for(std::map<int,bool>::iterator it = groups.begin(); it != groups.end(); it++)
		{
			RefreshGroup(it->first);
		}
		m_animationTrack.ClearAnimationPtr();
		ChangeSelectControls();
	}
}

namespace Phantom{
	VOID			ResetCenter(UIDialog* dlg);
};
VOID		GCViewUIEditor::ResetCenter(){
	Phantom::ResetCenter(this->m_dialogPtr);
}
BOOL		GCViewUIEditor::IsDeleteTree()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type && (type->type == UI_PROP_CONTROL || type->type == UI_PROP_GROUP || type->type == UI_PROP_ANIMATION || type->type == UI_PROP_UV_ANIMATION || type->type == UI_PROP_ELEMENTS))
			return true;
	}
	return false;
}

VOID		GCViewUIEditor::OnCopyTree()
{
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
	std::vector<TreeItemType*> types;
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(type->type == this->UI_PROP_ANIMATION || type->type == this->UI_PROP_CONTROL || type->type == this->UI_PROP_UV_ANIMATION || type->type == this->UI_PROP_GROUP || type->type == this->UI_PROP_ELEMENTS)
			types.push_back(type);
	}
	SetUICopy(this, &types);
}

VOID		GCViewUIEditor::OnPasteTree()
{
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
	bool bPaste	=	false;
	for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	{
		TreeItemType* type	=	it->second;
		if(!type)
			continue;
		if(IsCanPaste(type->type))
		{
			bPaste	=	true;
			break;
		}
	}
	if(!bPaste)
		return;
	try{
		for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
		{
			TreeItemType* type	=	it->second;
			if(!type)
				continue;
			switch(type->type)
			{
			case UI_PROP_MAIN:
				{
					std::map<int,bool> groups;
					for(int j=0;j<g_copyUIs.size();j++)
					{
						if(g_copyUIs[j].type.type != UI_PROP_GROUP)
							continue;
						Phantom::UIGroup* group	=	g_copyUIs[j].editor->m_dialogPtr->FindGroupPtr(Phantom::PtrID(0, g_copyUIs[j].type.p1));
						if(group)
						{
							Phantom::UIGroup* to = m_dialogPtr->AddGroupPtr();
							to->CopyFrom(group);
							groups[to->GetID()] = true;
						}
					}
					this->BuildAll();
					g_copyUIs.clear();
					this->SetModifyed(true);
				}
				break;
			case UI_PROP_GROUP:
				{
					BOOL bClone	=	false;
					std::map<int,bool> groups;
					for(int j=0;j<g_copyUIs.size();j++)
					{
						if(g_copyUIs[j].type.type != UI_PROP_CONTROL)
							continue;
						Phantom::UIControl* c1	=	g_copyUIs[j].editor->m_dialogPtr->GetControlPtrFromID(g_copyUIs[j].type.p1);
						Phantom::UIControl* c2	=	c1->Clone(-1, m_dialogPtr);
						bClone	=	true;
						c2->SetGroupID(type->p1);
						//
						groups[c2->GetGroupID()] = true;
					}
					for(std::map<int,bool>::iterator it = groups.begin(); it != groups.end(); it++)
					{
						RefreshGroup(it->first);
					}
					//BuildAll();
					g_copyUIs.clear();
					this->SetModifyed(true);
				}
				break;
			case UI_PROP_CONTROL:
				{
					Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
					if(!control)
						continue;
					BOOL bClone	=	false;
					for(int j=0;j<g_copyUIs.size();j++)
					{
						if(g_copyUIs[j].type.type != UI_PROP_CONTROL)
							continue;
						Phantom::UIControl* c1	=	g_copyUIs[j].editor->m_dialogPtr->GetControlPtrFromID(g_copyUIs[j].type.p1);
						control->CopyFrom(c1);
						bClone	=	true;
						break;
					}
					//copy tracks
					for(int j=0;j<g_copyUIs.size();j++)
					{
						if(g_copyUIs[j].type.type != UI_PROP_ANIMATION)
							continue;
						Phantom::UIControl* c2	=	g_copyUIs[j].editor->m_dialogPtr->GetControlPtrFromID(g_copyUIs[j].type.p1);
						Phantom::UIAnimationTrack* track	=	c2->GetTrackPtr(g_copyUIs[j].type.p2);
						control->CloneTrack(track);
						bClone	=	true;
					}
					if(bClone)
						this->OnChangeTreeSelect(&m_treeView, 0);
					this->SetModifyed(true);
				}
				break;
			case UI_PROP_ANIMATION_LIST:
				{
					Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
					if(!control)
						continue;
					BOOL bClone	=	false;
					for(int j=0;j<g_copyUIs.size();j++)
					{
						if(g_copyUIs[j].type.type != UI_PROP_ANIMATION)
							continue;
						Phantom::UIControl* c2	=	g_copyUIs[j].editor->m_dialogPtr->GetControlPtrFromID(g_copyUIs[j].type.p1);
						Phantom::UIAnimationTrack* track	=	c2->GetTrackPtr(g_copyUIs[j].type.p2);
						control->CloneTrack(track);
						bClone	=	true;
					}
					RefreshAnimations(control, it->first);
					g_copyUIs.clear();
					this->SetModifyed(true);
				}
				break;
			case UI_PROP_ANIMATION:
				{
					Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
					if(!control)
						continue;
					Phantom::UIAnimationTrack* t	=	control->GetTrackPtr(type->p2);
					if(!t)
						continue;
					BOOL bClone	=	false;
					for(int j=0;j<g_copyUIs.size();j++)
					{
						if(g_copyUIs[j].type.type != UI_PROP_ANIMATION)
							continue;
						Phantom::UIControl* c2	=	g_copyUIs[j].editor->m_dialogPtr->GetControlPtrFromID(g_copyUIs[j].type.p1);
						Phantom::UIAnimationTrack* track	=	c2->GetTrackPtr(g_copyUIs[j].type.p2);
						t->CopyFrom(track);
						bClone	=	true;
						break;
					}
					RefreshControl(control, m_treeView.GetParentItem(it->first));//RefreshAnimations(control, m_treeView.GetParentItem(it->first));
					if(bClone)
						this->OnChangeTreeSelect(&m_treeView, 0);
					this->SetModifyed(true);
				}
				break;
			case UI_PROP_UV_ANIMATION:
				{
					Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
					if(!control)
						continue;
					Phantom::UIElement* element	=	control->GetElementPtr(type->p2);
					Phantom::UVAnimation* uvAni	=	GetUVAniByID(element, type->p3);//element->GetUVAnimationPtr(type->p3);//control->GetElementPtr(type->p2);
					BOOL bClone	=	false;
					for(int j=0;j<g_copyUIs.size();j++)
					{
						if(g_copyUIs[j].type.type != UI_PROP_UV_ANIMATION)
							continue;
						Phantom::UIControl* c1	=	g_copyUIs[j].editor->m_dialogPtr->GetControlPtrFromID(g_copyUIs[j].type.p1);
						Phantom::UIElement* ele	=	c1->GetElementPtr(g_copyUIs[j].type.p2);
						uvAni->CopyFrom(GetUVAniByID(ele, g_copyUIs[j].type.p3));
						//
					}
					if(bClone)
						this->OnChangeTreeSelect(&m_treeView, 0);
					RefreshUVAnimations(control, element, type->p2, m_treeView.GetParentItem(it->first));
					this->SetModifyed(true);
				}
				break;
			case UI_PROP_ELEMENTS:
				{
					Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
					if(!control)
						continue;
					Phantom::UIElement* element	=	control->GetElementPtr(type->p2);
					BOOL bClone	=	false;
					for(int j=0;j<g_copyUIs.size();j++)
					{
						if(g_copyUIs[j].type.type == UI_PROP_UV_ANIMATION)
						{
							Phantom::UIControl* c1	=	g_copyUIs[j].editor->m_dialogPtr->GetControlPtrFromID(g_copyUIs[j].type.p1);
							Phantom::UIElement* ele	=	c1->GetElementPtr(g_copyUIs[j].type.p2);
							element->AddUVAnimation(GetUVAniByID(ele, g_copyUIs[j].type.p3));
							continue;
						}
						if(g_copyUIs[j].type.type != UI_PROP_ELEMENTS)
							continue;
						if(bClone)
							continue;
						Phantom::UIControl* c1	=	g_copyUIs[j].editor->m_dialogPtr->GetControlPtrFromID(g_copyUIs[j].type.p1);
						Phantom::UIElement* ele	=	c1->GetElementPtr(g_copyUIs[j].type.p2);
						element->CopyFrom(ele);
						bClone	=	true;
					}
					this->SetModifyed(true);
					RefreshUVAnimations(control, element, type->p2, it->first);
					if(bClone)
						this->OnChangeTreeSelect(&m_treeView, 0);
				}
				break;
			}
		}
	}
	catch(...)
	{
		assert(false);
		g_copyUIs.clear();
	}
}

void		GCViewUIEditor::OnTreeViewKeyDown(INT nChar)
{
	if(nChar == VK_DELETE)
	{
		this->OnDeleteTree();
	}
}

void		GCViewUIEditor::OnCommandUI(INT nMenuID)
{
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

	switch(nMenuID)
	{
	case ID_UI_COPY:
		{
			OnCopyTree();
		}
		break;
	case ID_UI_PASTE:
		{
			OnPasteTree();
		}
		break;

	case ID_UI_INSERTANIMATION:
		{
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UI_PROP_ANIMATION_LIST || type->type == UI_PROP_CONTROL)
				{
					Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
					if(!control)
						continue;
					control->AddTrack("Animation");
					RefreshControl(control, it->first);//RefreshAnimations(control, it->first);
					this->SetModifyed();
				}
			}
		}
		break;
#ifdef USE_BOX
	case ID_UI_INSERTBODY:
		{
			if(!m_dialogPtr->m_box2World.IsEnable())
				return;
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UI_PROP_BOX2D_BODY)
				{
					m_dialogPtr->m_box2World.AddDynCircle(PtoB(Phantom::Pixel(256, 256)), PtoB(100));
					this->RefreshBoxBodys();
					this->SetModifyed();
					break;
				}
			}
		}
		break;
	case ID_UI_INSERTJOINT:
		{
			if(!m_dialogPtr->m_box2World.IsEnable())
				return;
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UI_PROP_BOX2D_JOINT)
				{
					m_dialogPtr->m_box2World.AddJoint();
					this->RefreshBoxJoints();
					this->SetModifyed();
					break;
				}
			}
		}
		break;
	case ID_UI_INSERTSHAPE:
		{
			if(!m_dialogPtr->m_box2World.IsEnable())
				return;
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UI_PROP_B2_BODY)
				{
					Phantom::PtrID id(0, type->p1);
					Phantom::B2Body* body		=	m_dialogPtr->m_box2World.GetBodyPtr(id);
					if(body)
					{
						body->AddShape();
						this->RefreshBoxShapes(it->first, body);
						this->SetModifyed();
					}
				}
			}
		}
		break;
#endif
	case ID_INSERT_UIGROUP:
		{
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UI_PROP_MAIN)
				{
					Phantom::UIGroup* g	=	m_dialogPtr->AddGroupPtr();
					g->SetName("group");
					this->SetModifyed();
					BuildAll();
					break;
				}
			}
		}
		break;
	case ID_UI_HIDE_CONTROLE:
		{
			std::map<HTREEITEM,Phantom::UIControl*> controls;
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UI_PROP_CONTROL)
				{
					Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
					if(control)
						control->SetHideOnEditor(!control->GetHideOnEditor());
					controls[it->first] = control;
				}
			}
			for(std::map<HTREEITEM,Phantom::UIControl*>::iterator it = controls.begin();it != controls.end(); it++)
			{
				this->RefreshControl(it->second, it->first);
			}
			this->SetModifyed();
		}
		break;
	case ID_SEL_ALL_ELEMENT:
		{
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UI_PROP_GROUP)
				{
					m_treeView.ClearSelect();
					Phantom::UIGroup* g	=	m_dialogPtr->AddGroupPtr();
					HTREEITEM hControl	=	m_treeView.GetChildItem(it->first);
					while(hControl)
					{
						HTREEITEM hElement	=	m_treeView.GetChildItem(hControl);
						while(hElement)
						{
							TreeItemType* data	=	this->GetTreeItem(m_treeView.GetItemData(hElement));
							if(data && data->type == this->UI_PROP_ELEMENTS)
							{
								m_treeView.AddSelect(hElement, true);
							}
							hElement	=	m_treeView.GetNextSiblingItem(hElement);
						}
						hControl	=	m_treeView.GetNextSiblingItem(hControl);
					}
				}
			}
		}
		break;
	//case ID_AUTO_GENERIC_UVMAP:
	//	{
	//		for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
	//		{
	//			TreeItemType* type	=	it->second;
	//			if(!type)
	//				continue;
	//			if(type->type == this->UI_PROP_ELEMENTS)
	//			{
	//				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
	//				if(!control)
	//					continue;
	//				Phantom::UIElement* element	=	control->GetElementPtr(type->p2);
	//				if(!element)
	//					continue;
	//				RefreshUVAnimations(control, element, type->p2, it->first);
	//			}
	//		}
	//	}
	//	break;
	case ID_UI_INSERTUVANIMATION:
		{
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UI_PROP_ELEMENTS)
				{
					Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
					if(!control)
						continue;
					Phantom::UIElement* element	=	control->GetElementPtr(type->p2);
					if(!element)
						continue;
					element->AddUVAnimation();
					RefreshUVAnimations(control, element, type->p2, it->first);
				}
			}
		}
		break;
	case ID_UI_CLONETRACK:
		{
			for(std::map<HTREEITEM,TreeItemType*>::iterator it = selectTypes.begin();it != selectTypes.end(); it++)
			{
				TreeItemType* type	=	it->second;
				if(!type)
					continue;
				if(type->type == UI_PROP_ANIMATION)
				{
					Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
					Phantom::UIAnimationTrack* t	=	control->GetTrackPtr(type->p2);
					if(!control)
						continue;
					control->CloneTrack(t);
					RefreshAnimations(control, m_treeView.GetParentItem(it->first));
				}
			}
		}
		break;
	default:
		break;
	}
}

void				GCViewUIEditor::OnDoubleClick(CMTreeCtrl* treePtr, HTREEITEM hItem)
{
}
//更新选择树形控件

VOID		GCViewUIEditor::PlayAnimation()
{
	m_animationTrack.SetPlayValue(0);
	//
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_CONTROL)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				if(control->GetTrackDefault())//->GetID())
					control->ActiveTrackByID(control->GetTrackDefault()->GetID());
			}
			else if(type->type == UI_PROP_GROUP)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(c->GetGroupID() == type->p1)
					{
						if(c->GetTrackDefault())//->GetID())
							c->ActiveTrackByID(c->GetTrackDefault()->GetID());
					}
				}
			}
			else if(type->type == this->UI_PROP_MAIN)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(c->GetTrackDefault())//->GetID())
						c->ActiveTrackByID(c->GetTrackDefault()->GetID());
				}
			}
#ifdef USE_BOX
			else if(type->type == UI_PROP_BOX2D_BODY || type->type == UI_PROP_BOX2D_JOINT
				|| type->type == UI_PROP_B2_BODY || type->type == UI_PROP_B2_SHAPE || type->type == UI_PROP_B2_JOINT)
			{
				if(m_dialogPtr->m_box2World.IsEnable())
				{
					m_dialogPtr->m_box2World.DestroyBox2D();
					m_dialogPtr->m_box2World.CreateBox2D();
				}
			}
#endif
		}
	}
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_ANIMATION)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				Phantom::UIAnimationTrack* e		=	control->GetTrackPtr(type->p2);
				if(e)
					control->ActiveTrackByID(e->GetID());
			}
		}
	}
}
VOID		GCViewUIEditor::StopAnimation()
{
	//
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_CONTROL)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				control->StopTrack();
			}
			else if(type->type == UI_PROP_GROUP)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(c->GetGroupID() == type->p1)
					{
						c->StopTrack();
					}
				}
			}
			else if(type->type == this->UI_PROP_MAIN)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					c->StopTrack();
				}
			}
#ifdef USE_BOX
			else if(type->type == UI_PROP_BOX2D_BODY || type->type == UI_PROP_BOX2D_JOINT
				|| type->type == UI_PROP_B2_BODY || type->type == UI_PROP_B2_SHAPE || type->type == UI_PROP_B2_JOINT)
			{
				if(m_dialogPtr->m_box2World.IsEnable())
				{
					m_dialogPtr->m_box2World.DestroyBox2D();
				}
			}
#endif
		}
	}
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_ANIMATION)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				control->StopTrack();
			}
		}
	}
}
VOID		GCViewUIEditor::PauseAnimation(BOOL bPause)
{
	//
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_CONTROL)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				control->PauseTrack(bPause);
			}
			else if(type->type == UI_PROP_GROUP)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(c->GetGroupID() == type->p1)
					{
						c->PauseTrack(bPause);
					}
				}
			}
			else if(type->type == this->UI_PROP_MAIN)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					c->PauseTrack(bPause);
				}
			}
#ifdef USE_BOX
			else if(type->type == UI_PROP_BOX2D_BODY || type->type == UI_PROP_BOX2D_JOINT
				|| type->type == UI_PROP_B2_BODY || type->type == UI_PROP_B2_SHAPE || type->type == UI_PROP_B2_JOINT)
			{
				if(m_dialogPtr->m_box2World.IsEnable())
				{
					m_dialogPtr->m_box2World.Pause(bPause);
				}
			}
#endif
		}
	}
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_ANIMATION)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				control->PauseTrack(bPause);
			}
		}
	}
}
VOID		GCViewUIEditor::SetAnimationLoop(BOOL bLoop)
{
}
BOOL		GCViewUIEditor::IsAnimationCanPause()
{
	//
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_CONTROL)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				if(control->GetTrackDefault())
					return true;
			}
			else if(type->type == UI_PROP_GROUP)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(c->GetGroupID() == type->p1)
					{
						if(c->GetTrackDefault())
							return true;
					}
				}
			}
			else if(type->type == this->UI_PROP_MAIN)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(c->GetTrackDefault())
						return true;
				}
			}
#ifdef USE_BOX
			else if(type->type == UI_PROP_BOX2D_BODY || type->type == UI_PROP_BOX2D_JOINT
				|| type->type == UI_PROP_B2_BODY || type->type == UI_PROP_B2_SHAPE || type->type == UI_PROP_B2_JOINT)
			{
				if(m_dialogPtr->m_box2World.IsEnable())
					return true;
			}
#endif
		}
	}
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_ANIMATION)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				Phantom::UIAnimationTrack* e		=	control->GetTrackPtr(type->p2);
				if(e)
					return true;
			}
		}
	}
	return false;
}
BOOL		IsExistAni(Phantom::UIControl* c)
{
	if(c->GetTrackDefault())
		return true;
	for(int i=0;i<c->GetElementCount();i++)
	{
		if(c->GetElementPtr(i)->GetUVAnimationCount() > 0)
			return true;
	}
	return false;
}
BOOL		GCViewUIEditor::IsAnimationCanPlayStop()
{
	//
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_CONTROL)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				if(IsExistAni(control))//->GetTrackDefault())
					return true;
			}
			else if(type->type == UI_PROP_GROUP)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(c->GetGroupID() == type->p1)
					{
						if(IsExistAni(c))//if(c->GetTrackDefault())
							return true;
					}
				}
			}
			else if(type->type == this->UI_PROP_MAIN)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(IsExistAni(c))//if(c->GetTrackDefault())
						return true;
				}
			}
#ifdef USE_BOX
			else if(type->type == UI_PROP_BOX2D_BODY || type->type == UI_PROP_BOX2D_JOINT
				|| type->type == UI_PROP_B2_BODY || type->type == UI_PROP_B2_SHAPE || type->type == UI_PROP_B2_JOINT)
			{
				if(m_dialogPtr->m_box2World.IsEnable())
					return true;
			}
#endif
		}
	}
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_ANIMATION)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				Phantom::UIAnimationTrack* e		=	control->GetTrackPtr(type->p2);
				if(e)
					return true;
			}
		}
	}
	return false;
}
BOOL		GCViewUIEditor::IsAnimationCanLoop()
{
	return false;
}
BOOL		GCViewUIEditor::IsAnimationPlaying()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_CONTROL)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				if(control->IsPlayingTrack())
					return true;
			}
			else if(type->type == UI_PROP_GROUP)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(c->GetGroupID() == type->p1)
					{
						if(c->IsPlayingTrack())
							return true;
					}
				}
			}
			else if(type->type == this->UI_PROP_MAIN)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(c->IsPlayingTrack())
						return true;
				}
			}
#ifdef USE_BOX
			else if(type->type == UI_PROP_BOX2D_BODY || type->type == UI_PROP_BOX2D_JOINT
				|| type->type == UI_PROP_B2_BODY || type->type == UI_PROP_B2_SHAPE || type->type == UI_PROP_B2_JOINT)
			{
				if(m_dialogPtr->m_box2World.IsEnable())
					return (m_dialogPtr->m_box2World.GetWorldPtr() != 0);
			}
#endif
		}
	}
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_ANIMATION)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				if(control->IsPlayingTrack())
					return true;
			}
		}
	}
	return false;
}
BOOL		GCViewUIEditor::IsAnimationPause()
{
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_CONTROL)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				if(control->IsPauseTrack())
					return true;
			}
			else if(type->type == UI_PROP_GROUP)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(c->GetGroupID() == type->p1)
					{
						if(c->IsPauseTrack())
							return true;
					}
				}
			}
			else if(type->type == this->UI_PROP_MAIN)
			{
				for(int i=0;i<m_dialogPtr->GetControlCount();i++)
				{
					Phantom::UIControl* c	=	m_dialogPtr->GetControlPtr(i);
					if(c->IsPauseTrack())
						return true;
				}
			}
#ifdef USE_BOX
			else if(type->type == UI_PROP_BOX2D_BODY || type->type == UI_PROP_BOX2D_JOINT
				|| type->type == UI_PROP_B2_BODY || type->type == UI_PROP_B2_SHAPE || type->type == UI_PROP_B2_JOINT)
			{
				if(m_dialogPtr->m_box2World.IsEnable())
					return m_dialogPtr->m_box2World.IsPause();
			}
#endif
		}
	}
	for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			if(type->type == UI_PROP_ANIMATION)
			{
				Phantom::UIControl* control	=	this->m_dialogPtr->GetControlPtrFromID(type->p1);
				if(!control)
					continue;
				if(control->IsPauseTrack())
					return true;
			}
		}
	}
	return false;
}
BOOL		GCViewUIEditor::IsAnimationLoop()
{
	return false;
}

VOID		AddText(std::string& str, const char* szText, ...)
{
	char buf[20480];
	_vsnprintf(buf, 20480, szText, (va_list)(&szText + 1));
	str	+=	buf;
	str	+=	"\r\n";
}
VOID		ToCode(char* v)
{
	int len = strlen(v);
	for(int i=0;i<len;i++)
	{
		char ch = v[i];
		if(ch >= 'a' && ch <= 'z')
			continue;
		if(ch >= '0' && ch <= '9')
		{
			if(i > 0)
				continue;
		}
		if(ch >= 'A' && ch <= 'Z')
			continue;
		if(ch == '_')
			continue;
		v[i] = '_';
	}
}

const char*	GetControlTypeName(int type)
{
	switch(type)
	{
	case UIControlType_BUTTON://			/*控件类型_按钮*/=	1,
		return "UIButton";
	case UIControlType_CHECKBOX://			/*控件类型_按钮*/=	1,
		return "UICheckBox";
	case UIControlType_COMBOBOX://			/*控件类型_按钮*/=	1,
		return "UIComboBox";
	case UIControlType_SLIDER://			/*控件类型_按钮*/=	1,
		return "UISlider";
	case UIControlType_LISTBOX://			/*控件类型_按钮*/=	1,
		return "UIListBox";
	case UIControlType_SCROLLBAR://			/*控件类型_按钮*/=	1,
		return "UIScrollBar";
	case UIControlType_PROGRESS://			/*控件类型_按钮*/=	1,
		return "UIProgress";
	case UIControlType_STATIC://			/*控件类型_按钮*/=	1,
		return "UIStatic";
	case UIControlType_VIEW_FORMAT_TEXT://			/*控件类型_按钮*/=	1,
		return "UIStaticFormat";
	case UIControlType_LISTCTRL3D://			/*控件类型_按钮*/=	1,
		return "UIListCtrl";
//	case UIControlType_2DEffect://			/*控件类型_按钮*/=	1,
//		return "UIEffect";
	}
	return "UIControl";
}

const char*	GetControlType(int type)
{
	switch(type)
	{
	case UIControlType_BUTTON://			/*控件类型_按钮*/=	1,
		return "UIControlType_BUTTON";
	case UIControlType_CHECKBOX://			/*控件类型_按钮*/=	1,
		return "UIControlType_CHECKBOX";
	case UIControlType_COMBOBOX://			/*控件类型_按钮*/=	1,
		return "UIControlType_COMBOBOX";
	case UIControlType_SLIDER://			/*控件类型_按钮*/=	1,
		return "UIControlType_SLIDER";
	case UIControlType_LISTBOX://			/*控件类型_按钮*/=	1,
		return "UIControlType_LISTBOX";
	case UIControlType_SCROLLBAR://			/*控件类型_按钮*/=	1,
		return "UIControlType_SCROLLBAR";
	case UIControlType_PROGRESS://			/*控件类型_按钮*/=	1,
		return "UIControlType_PROGRESS";
	case UIControlType_STATIC://			/*控件类型_按钮*/=	1,
		return "UIControlType_STATIC";
	case UIControlType_VIEW_FORMAT_TEXT://			/*控件类型_按钮*/=	1,
		return "UIControlType_VIEW_FORMAT_TEXT";
	case UIControlType_LISTCTRL3D://			/*控件类型_按钮*/=	1,
		return "UIControlType_LISTCTRL3D";
//	case UIControlType_2DEffect://			/*控件类型_按钮*/=	1,
//		return "UIControlType_2DEffect";
	}
	return 0;
}

struct	DefineType
{
	std::string str;
	std::string name;
};
VOID		GCViewUIEditor::GenericCPPCode(BOOL bByName)	//创建可以在IPhone系统开始时使用的对话框自动初始化代码
{
#ifdef ZDX_TOOL_MODE
	wchar_t fileName[256];
	_wsplitpath(m_strFile.c_str(), 0, 0, fileName, 0);
	std::string codeInit, codeDefine, code, codeClear;
	A className(fileName);
	ToCode(className.buf);
	char buf[1024];
	AddText(code, "//%s2005-2017, generic codes for UI loader\r\nclass dlg_%s:public dlg_base\r\n{\r\npublic:\r\ndlg_%s(){clear();}\r\nvirtual ~dlg_%s(){}", className.buf, className.buf, className.buf, className.buf);
	std::vector<DefineType*> groupType;
	for(int g=0;g<m_dialogPtr->GetGroupCount();g++)
	{
		Phantom::UIGroup* group	=	m_dialogPtr->GetGroupPtr(g);
		Name groupName = group->GetName();
		ToCode(groupName.t);
		std::string strDefine;
		if(g > 0)
		{
			AddText(strDefine, "{\r\nUIGroup* group;");
			if(bByName)
				AddText(codeInit, "%s.group		=	SearchGroupUI(dlg, \"%s\");//%s", groupName.t, group->GetName(), group->GetName());
			else
				AddText(codeInit, "%s.group		=	SearchGroupUI(dlg, %d);//%s", groupName.t, group->GetID(), group->GetName());
			AddText(codeClear, "%s.group	=	0;//%s", groupName.t, group->GetName());
		}
		for(int c=0;c<m_dialogPtr->GetControlCount();c++)
		{
			Phantom::UIControl* control		=	m_dialogPtr->GetControlPtr(c);
			if(control->IsBackground() || control->GetGroupID() != group->GetID())
				continue;	//如果是背景控件，则不创建
			Name controlName		=	control->GetName();
			if(!controlName.t[0] || controlName == "n/a" || controlName == "nil" || controlName == "none" || controlName == "null")
				continue;
			ToCode(controlName.t);
			const char* szClassName	=	GetControlTypeName(control->GetType());
			if(g > 0)
				AddText(strDefine, "%s*		%s;", szClassName, controlName.t);
			else
				AddText(codeDefine, "%s*		%s;", szClassName, controlName.t);
			if(g > 0)
			{
				if(bByName)
					AddText(codeInit, "%s.%s	=	SearchControlUI<%s>(%s.group, \"%s\", %s);//%s", groupName.t, controlName.t, szClassName, groupName.t, control->GetName(), GetControlType(control->GetType()), control->GetName());
				else
					AddText(codeInit, "%s.%s	=	SearchControlUI<%s>(%s.group, %d, %s);//%s", groupName.t, controlName.t, szClassName, groupName.t, control->GetID(), GetControlType(control->GetType()), control->GetName());
				AddText(codeClear, "%s.%s	=	0;//%s", groupName.t, controlName.t, control->GetName());
			}
			else
			{
				if(bByName)
					AddText(codeInit, "%s	=	SearchControlUI<%s>(dlg, \"%s\", %s);//%s", controlName.t, szClassName, control->GetName(), GetControlType(control->GetType()), control->GetName());
				else
					AddText(codeInit, "%s	=	SearchControlUI<%s>(dlg, %d, %s);//%s", controlName.t, szClassName, control->GetID(), GetControlType(control->GetType()), control->GetName());
				AddText(codeClear, "%s	=	0;//%s", controlName.t, control->GetName());
			}
		}
		if(g > 0)
		{
			AddText(strDefine, "inline UIGroup* operator->(){return group;}\r\n};", groupName.t, groupName.t);
			//
			std::string strType;
			for(int k=0;k<groupType.size();k++)
			{
				if(stricmp(groupType[k]->str.c_str(), strDefine.c_str()) == 0)
				{
					strType = groupType[k]->name.c_str();
					break;
				}
			}
			if(strType.empty())
			{
				DefineType* pp = new DefineType();
				pp->name = groupName.t;
				pp->str = strDefine.c_str();
				groupType.push_back(pp);
				AddText(codeDefine, "struct s_%s\r\n\r\n %s", groupName.t, strDefine.c_str(), groupName.t, groupName.t);
				strType = groupName.t;
			}
			AddText(codeDefine, "s_%s %s;", strType.c_str(), groupName.t);
		}
	}
	for(int i=0;i<groupType.size();i++)
		delete groupType[i];
	groupType.clear();
	code	+=	codeDefine.c_str();
	AddText(code, "\r\nvoid clear()\r\n{");
	code	+=	codeClear.c_str();
	AddText(code, "}\r\npublic:\r\n"
		"BOOL Load(const char* szDlg)\r\n"
		"{\r\nif(!GetDlgManager()->LoadDialog(dlg, szDlg))\r\nreturn false;\r\ndlg->SetVisible(false);");
	code	+=	codeInit.c_str();
	AddText(code, "\r\nreturn true;\r\n}\r\n};");
	SetProjectPath();
	char heads[512];
	strlwr(className.buf);
	sprintf(heads, "#ifndef __%s__\r\n#define __%s__\r\n", className.buf, className.buf);
	char fileOut[256];
	sprintf(fileOut, "../ui/dlg_%s.h", className.buf);
	FILE* f = fopen(fileOut, "wb");
	if(!f)
	{
		GetMainFrame()->MessageBox(Language(L"必须存在这个../ui/目录才能被创建。"), Language(L"正版专用功能"), MB_OK);
		return;
	}
	fwrite(heads, strlen(heads), 1, f);
	fwrite(code.c_str(), code.size(), 1, f);
	sprintf(heads, "\r\n#endif\r\n");
	fwrite(heads, strlen(heads), 1, f);
	fclose(f);
	/*if( OpenClipboard(0))
	{
		EmptyClipboard();
		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, code.size() + 1 );
		if( hBlock )
		{
			char *pwszText = (char*)GlobalLock( hBlock );
			strcpy(pwszText, code.c_str());
			GlobalUnlock( hBlock );
			SetClipboardData( CF_TEXT, hBlock );
		}
		CloseClipboard();
		if( hBlock )
			GlobalFree( hBlock );
	}*/
	GetMainFrame()->MessageBox(Language(L"已经拷贝到剪贴板。"), Language(L"正版专用功能"), MB_OK);
#endif

}

VOID	ExportPngs(const char* szFile, Phantom::Texture* tex)
{
	static std::vector<std::string> files;
	if(tex->m_rects.size()==0)return;
	for(int i=0;i<files.size();i++){
		if(files[i]==szFile)
			return;
	}
	files.push_back(szFile);
		CImage img;
		char path[512];
		strcpy(path, "images/res/");
		if(SUCCEEDED(img.Load(Wutf8(szFile).buf))){
			unsigned char* srcBits = (unsigned char*)img.GetBits();
			int spitch = img.GetPitch();
			for(int i=0;i<tex->m_rects.size();i++){
				TextureRect& rc = tex->m_rects[i];
				if(rc.orig.cx==0||rc.orig.cy==0)
					continue;
				char file[256];
				sprintf(file, "%s%s.png", path, rc.name.t);
				CImage outimg;
				outimg.Create(rc.orig.cx, rc.orig.cy, 32, CImage::createAlphaChannel);
				unsigned char* destBits = (unsigned char*)outimg.GetBits();
				int dpitch = outimg.GetPitch();
				for(int x=0;x<rc.rc.GetWidth();x++)
				{
					for(int y=0;y<rc.rc.GetHeight();y++)
					{
						int xx = x + rc.rc.left;
						int yy = y + rc.rc.top;
						int dx = x + rc.off.x;
						int dy = y + rc.off.y;
						unsigned int* di = (unsigned int*)(destBits + dy*dpitch);
						unsigned int* si = (unsigned int*)(srcBits + yy*spitch);
						di[dx] = si[xx];
					}
				}
				outimg.Save(Wutf8(file).buf);
			}
		}
}
