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

bool		GCViewUIEditor::OnWindowProc(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam)
{
	if(m_nCurrentEditorMode <= 0)
		return false;
	switch(this->m_nCurrentEditorMode)
	{
	case EditorMode_Default:
		this->OnEditorMode(hWnd, uMsg, wParam, lParam);
		break;
	//case EditorMode_TerrainFloor:
	//	this->OnEditorFloorMode(hWnd, uMsg, wParam, lParam);
	//	break;
	case EditorMode_TerrainUnit:
		this->OnEditorUnitMode(hWnd, uMsg, wParam, lParam);
		break;
	}
	return __super::OnWindowProc(hWnd, uMsg, wParam, lParam);
}

//返回全部可移动点操作对象,8个
VOID		GCViewUIEditor::GetMovePointRect(Phantom::UIControl* c, const Phantom::Rect& rc, Phantom::Rect* rets)
{
	const int size	=	3;
	int centerX		=	rc.left + rc.GetWidth() / 2;
	int centerY		=	rc.top + rc.GetHeight() / 2;
	CPoint point[MovePointSide_Count];
	point[MovePointSide_Left].SetPoint(rc.left, centerY);
	point[MovePointSide_Right].SetPoint(rc.right, centerY);
	point[MovePointSide_Top].SetPoint(centerX, rc.top);
	point[MovePointSide_Bottom].SetPoint(centerX, rc.bottom);
	point[MovePointSide_LeftTop].SetPoint(rc.left, rc.top);
	point[MovePointSide_LeftBottom].SetPoint(rc.left, rc.bottom);
	point[MovePointSide_RightTop].SetPoint(rc.right, rc.top);
	point[MovePointSide_RightBottom].SetPoint(rc.right, rc.bottom);
	if(c)
	{
		point[MovePointSide_Center]	=	CPoint(c->GetRotCenter().x + rc.left, c->GetRotCenter().y + rc.top);
	}
	else
		point[MovePointSide_Center].SetPoint(centerX, centerY);
	point[MovePointSide_Rotation]	=	CPoint(rc.right + 18, rc.bottom + 18);
	//
	//if(c)
	//{
	//	for(int i=0;i<=MovePointSide_RightBottom;i++)
	//		point[i]	=	*(CPoint*)&c->ClientToScreen(Phantom::Pixel(point[i].x, point[i].y));
	//}
	rets[MovePointSide_Left].Set(point[MovePointSide_Left].x - size, point[MovePointSide_Left].y - size, point[MovePointSide_Left].x + size, point[MovePointSide_Left].y + size);
	rets[MovePointSide_Right].Set(point[MovePointSide_Right].x - size, point[MovePointSide_Right].y - size, point[MovePointSide_Right].x + size, point[MovePointSide_Right].y + size);
	rets[MovePointSide_Top].Set(point[MovePointSide_Top].x - size, point[MovePointSide_Top].y - size, point[MovePointSide_Top].x + size, point[MovePointSide_Top].y + size);
	rets[MovePointSide_Bottom].Set(point[MovePointSide_Bottom].x - size, point[MovePointSide_Bottom].y - size, point[MovePointSide_Bottom].x + size, point[MovePointSide_Bottom].y + size);
	rets[MovePointSide_LeftTop].Set(point[MovePointSide_LeftTop].x - size, point[MovePointSide_LeftTop].y - size, point[MovePointSide_LeftTop].x + size, point[MovePointSide_LeftTop].y + size);
	rets[MovePointSide_LeftBottom].Set(point[MovePointSide_LeftBottom].x - size, point[MovePointSide_LeftBottom].y - size, point[MovePointSide_LeftBottom].x + size, point[MovePointSide_LeftBottom].y + size);
	rets[MovePointSide_RightTop].Set(point[MovePointSide_RightTop].x - size, point[MovePointSide_RightTop].y - size, point[MovePointSide_RightTop].x + size, point[MovePointSide_RightTop].y + size);
	rets[MovePointSide_RightBottom].Set(point[MovePointSide_RightBottom].x - size, point[MovePointSide_RightBottom].y - size, point[MovePointSide_RightBottom].x + size, point[MovePointSide_RightBottom].y + size);
	rets[MovePointSide_Center].Set(point[MovePointSide_Center].x - size, point[MovePointSide_Center].y - size, point[MovePointSide_Center].x + size, point[MovePointSide_Center].y + size);
	rets[MovePointSide_Rotation].Set(point[MovePointSide_Rotation].x - size * 2, point[MovePointSide_Rotation].y - size * 2, point[MovePointSide_Rotation].x + size * 2, point[MovePointSide_Rotation].y + size * 2);
	rets[MovePointSide_Body]	=	rc;
}


BOOL		GCViewUIEditor::AlignToGrid(CPoint& posIn, int align)
{
	m_nAlignType	^=	(m_nAlignType & align);//ToAlign_None;
	CPoint size	=	GetUIGridSize();
	int minAlignX	=	5;
	int minAlignY	=	5;
	minAlignX	=	size.x / 3;
	minAlignY	=	size.y / 3;
	size.x		=	(int)((float)size.x);
	size.y		=	(int)((float)size.y);
	if(minAlignX > 5)
		minAlignX = 5;
	if(minAlignY > 5)
		minAlignY = 5;
	if(!IsShowUIGrid())
	{
		return false;
	}
	int x, y;
	m_dialogPtr->GetLocation(x, y);
	x			=	(int)((float)x);
	y			=	(int)((float)y);
	CPoint pos(posIn.x - x, posIn.y - y);
	if(minAlignX > 1 && ((align & ToAlign_Left) || ( align & ToAlign_Right )))
	{
		int wM		=	(pos.x % size.x);
		if(wM < minAlignX)
		{
			m_nAlignType	|=	ToAlign_Left;
			pos.x	=	(pos.x / size.x) * size.x;
			m_alignPointW	=	pos;
		}
		else if((size.x - wM) < minAlignX)
		{
			m_nAlignType	|=	ToAlign_Right;
			pos.x	=	(pos.x / size.x + 1) * size.x;
			m_alignPointW	=	pos;
		}
	}
	if(minAlignY > 1 && ((align & ToAlign_Bottom) || (align & ToAlign_Top)))
	{
		int wM		=	(pos.y % size.y);
		if(wM < minAlignY)
		{
			m_nAlignType	|=	ToAlign_Top;
			pos.y	=	(pos.y / size.y) * size.y;
			m_alignPointH	=	pos;
		}
		else if((size.y - wM) < minAlignY)
		{
			m_nAlignType	|=	ToAlign_Bottom;
			pos.y	=	(pos.y / size.y + 1) * size.y;
			m_alignPointH	=	pos;
		}
	}
	BOOL ret	=	 ((m_nAlignType & align) > 0);
	if(!ret)
	{
		if(minAlignX > 5)
			minAlignX = 5;
		if(minAlignY > 5)
			minAlignY = 5;
		//
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
			bool bFind	=	false;
			for(int j=0;j<this->m_controlSelects.size();j++)
			{
				if(m_controlSelects[j] == sel)
				{
					bFind	=	true;
					break;
				}
			}
			if(bFind)
				continue;
			Phantom::Rect rc	=	sel->GetWindowRect();
			//
			int offsetLeft		=	abs(pos.x - rc.left);
			int offsetTop		=	abs(pos.y - rc.top);
			int offsetRight		=	abs(pos.x - rc.right);
			int offsetBottom	=	abs(pos.y - rc.bottom);
			//
			if(minAlignX > 1 && (align & ToAlign_Left))
			{
				if(offsetLeft < minAlignX)
				{
					m_nAlignType	|=	ToAlign_Left;
					pos.x			=	rc.left;
					m_alignPointW	=	pos;
				}
			}
			else if(minAlignX > 1 && (align & ToAlign_Right))
			{
				if(offsetRight < minAlignX)
				{
					m_nAlignType	|=	ToAlign_Right;
					pos.x			=	rc.right;
					m_alignPointW	=	pos;
				}
			}
			if(minAlignY > 1 && (align & ToAlign_Top))
			{
				if(offsetTop < minAlignY)
				{
					m_nAlignType	|=	ToAlign_Top;
					pos.y			=	rc.top;
					m_alignPointH	=	pos;
				}
			}
			else if(minAlignY > 1 && (align & ToAlign_Bottom))
			{
				if(offsetBottom < minAlignY)
				{
					m_nAlignType	|=	ToAlign_Bottom;
					pos.y			=	rc.bottom;
					m_alignPointH	=	pos;
				}
			}
		}
	}
	posIn.x	=	pos.x + x;
	posIn.y	=	pos.y + y;
	return ((m_nAlignType & align) > 0);
}

BOOL		GCViewUIEditor::AlignRectToGrid(CPoint& offset, const CPoint& movePosIn, const Phantom::Rect& rc)
{
	CPoint movePos	=	movePosIn;
	CPoint savePos;
	{
		if(!AlignToGrid(movePos, ToAlign_Left | ToAlign_Right))
		{
			movePos.SetPoint(movePosIn.x + rc.GetWidth(), movePosIn.y + rc.GetHeight());
			savePos		=	movePos;
			AlignToGrid(movePos, ToAlign_Left | ToAlign_Right);
		}
		else
			savePos		=	movePosIn;
		offset.x		=	(movePos.x - savePos.x);
	}
	{
		movePos	=	movePosIn;
		if(!AlignToGrid(movePos, ToAlign_Top | ToAlign_Bottom))
		{
			movePos.SetPoint(movePosIn.x + rc.GetWidth(), movePosIn.y + rc.GetHeight());
			savePos		=	movePos;
			AlignToGrid(movePos, ToAlign_Top | ToAlign_Bottom);
		}
		else
			savePos		=	movePosIn;
		offset.y		=	(movePos.y - savePos.y);
	}
	return (m_nAlignType != 0);
}

BOOL		IsRectIn(const Phantom::Rect& rcParent, const Phantom::Rect& rcChild)
{
	if(rcChild.left < rcParent.left || rcChild.right > rcParent.right || rcChild.top < rcParent.top || rcChild.bottom > rcParent.bottom)
		return false;
	return true;
}

Phantom::Pixel	g_nearestControlPos(0, 0);
int		SortControlNear(const void* v1, const void* v2)
{
	Phantom::UIControl* u1	=	*(Phantom::UIControl**)v1;
	Phantom::UIControl* u2	=	*(Phantom::UIControl**)v2;
	const Phantom::Rect& rc1	=	u1->GetRenderRect();
	const Phantom::Rect& rc2	=	u2->GetRenderRect();
	Phantom::Pixel pos1(rc1.left + rc1.GetWidth() / 2, rc1.top + rc1.GetHeight() / 2);
	Phantom::Pixel pos2(rc2.left + rc2.GetWidth() / 2, rc2.top + rc2.GetHeight() / 2);
	//
	Phantom::Pixel p1(pos1.x - g_nearestControlPos.x, pos1.y - g_nearestControlPos.y);
	Phantom::Pixel p2(pos2.x - g_nearestControlPos.x, pos2.y - g_nearestControlPos.y);
	if(p1.x == 0 && p1.y == 0)
		return 1;
	if(p2.x == 0 && p2.y == 0)
		return -1;
	int f1	=	(int)sqrtf((float)(p1.x*p1.x +p1.y*p1.y));
	int f2	=	(int)sqrtf((float)(p2.x*p2.x +p2.y*p2.y));
	if(f1 < f2)
		return 1;
	if(f1 > f2)
		return -1;
	return 0;
}

unsigned char g_textControl[] = {0x8e,0x6c,0xb2,0xcf,0x74,0x49,0xf8,0x24,0x6e,0x31,0x1f,0xbe,0x6d,0x5a,0xf0,0x9a,0x9f,0x37,0x20,0x33,0x28,0xe7,0x48,0x9b,0x5c,0x34,0x09,0x1b,0xc6,0x85,0xa1,0xc6,0xfb,0x6e,0x37,0x2e,0x7c,0x59,0xf4,0x7d,0x7b,0x07,0x72,0xdd,0x69,0x52};
unsigned char g_textControlPass[] = {0x55,0x96,0x1d,0x06,0xdc,0xb7,0x51,0x82,0xa0,0xf9,0xd5,0x1d,0xcd,0xb4,0x5e,0x56,0x3c,0x9f,0x87,0xd9,0x93,0x53,0x86,0x38,0x80,0xf0,0xdd,0xee,0x62,0x4e,0x6e,0x30};

Phantom::Rect		RectToDlgRect(const Phantom::Rect& rc, Phantom::UIDialog* dlg)
{
	//float fX = (float)Phantom::g_manager.GetDeviceWidth()/(float)Phantom::g_manager.m_gameWidth;
	//float fY = (float)Phantom::g_manager.GetDeviceHeight()/(float)Phantom::g_manager.m_gameHeight;
	//return Phantom::Rect((float)rc.left/fX, (float)rc.top/fY, (float)rc.right/fX, (float)rc.bottom/fY);
	return rc;//Phantom::Rect(rc.left + x, rc.top + y, rc.right + x, rc.bottom + y);//Phantom::Rect((int)((float)rc.left),(int)((float)rc.top),rc.left + (int)((float)rc.right / dlg->m_dlgScale.x),(int)((float)rc.bottom / dlg->m_dlgScale.y));
}

CPoint		PtToDlgPt(const CPoint& ptIn, Phantom::UIDialog* dlg)
{
	//int x,y;
	//dlg->GetLocation(x, y);
	//Phantom::float2 xy(((float)x * dlg->m_dlgScale.x), ((float)y * dlg->m_dlgScale.y));
	//Phantom::float2 pt((float)ptIn.x - xy.x, (float)ptIn.y - xy.y);
	//return CPoint((int)(pt.x / dlg->m_dlgScale.x + xy.x), (int)(pt.y / dlg->m_dlgScale.y + xy.y));
	float fX = (float)Phantom::g_manager.GetDeviceWidth()/(float)Phantom::g_manager.m_gameWidth;
	float fY = (float)Phantom::g_manager.GetDeviceHeight()/(float)Phantom::g_manager.m_gameHeight;
	return CPoint((int)((float)ptIn.x / fX), (int)((float)ptIn.y / fY));
}

void		GCViewUIEditor::OnEditorMode(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam)
{
	switch(uMsg)
	{
	case WM_MOUSEWHEEL:
		{
			CPhantomGCView* wnd	=	GetActiveView();
			if(!wnd)
				return;
			int zDelta = (short) HIWORD(wParam) / WHEEL_DELTA;
			if(zDelta < 0)
			{
				if(GetSelectKeyboard())
					wnd->SetScrollPos(SB_HORZ, wnd->GetScrollPos(SB_HORZ) + 32);
				else
					wnd->SetScrollPos(SB_VERT, wnd->GetScrollPos(SB_VERT) + 32);
			}
			else
			{
				if(GetSelectKeyboard())
					wnd->SetScrollPos(SB_HORZ, wnd->GetScrollPos(SB_HORZ) - 32);
				else
					wnd->SetScrollPos(SB_VERT, wnd->GetScrollPos(SB_VERT) - 32);
			}
			wnd->ChangeViewScroll();
		}
		break;
	}
	switch(uMsg)
	{
	case WM_MBUTTONDOWN:
		{
			this->m_dialogPtr->GetLocation((int&)m_downDlg.x, (int&)m_downDlg.y);
			::SetCapture(hWnd);
			GetCursorPos(&m_downPoint);
			ScreenToClient(hWnd, &m_downPoint);
			m_downPoint	=	PtToDlgPt(m_downPoint, m_dialogPtr);
			m_bDownMButton	=	true;
		}
		break;
	case WM_MBUTTONUP:
		{
			m_bDownMButton	=	false;
			ReleaseCapture();
		}
		break;
	case WM_LBUTTONDOWN:
		{
			m_moveCopys.clear();
			::SetCapture(hWnd);
			m_bLButtonDown	=	true;
			CPoint posT;
			GetCursorPos(&posT);
			ScreenToClient(hWnd, &posT);
			m_downPoint = posT;
			m_downPoint	=	PtToDlgPt(m_downPoint, m_dialogPtr);
			m_lastPoint		=	m_downPoint;
			//
			m_moveControl	=	0;
			m_editorMode	=	MouseMode_None;
			if(m_nEditorFlags != UIControlType_NONE && m_nEditorFlags != UIEditorFlags_TrackAnimation)
			{
				if( m_nEditorFlags < 100 )//如果是控件
				{
					bool bInsert	=	true;
					//switch(m_nEditorFlags)
					//{
					//case UIControlType_2DTerrain:
					//	if(m_dialogPtr->GetTerrain2DPtr() != NULL)
					//	{
					//		MessageBox(hWnd, Language(L"一个对话框中只能存在一个地形控件"), Language(L"提示"), MB_OK);
					//		bInsert	=	false;
					//	}
					//	break;
					//}
					if(bInsert)
					{
						AlignToGrid(m_downPoint);
						m_insertControl	=	this->m_dialogPtr->CreateFromType(true, m_nEditorFlags);//, Phantom::Rect(0, 0, 10, 10));
						m_insertControl->SetWindowRect(RectToDlgRect(Phantom::Rect(m_downPoint.x, m_downPoint.y, m_downPoint.x + 1, m_downPoint.y + 1), m_dialogPtr));
						m_editorMode	=	MouseMode_InsertControl;
					}
				}
				else if(m_nEditorFlags == UIEditorFlags_TabIndex)
				{
					m_editorMode	=	MouseMode_TabIndex;
				}
			}
			else
			{
				//如果不进行任何操作
				//Phantom::UIControl* sel	=	this->PickControl(m_downPoint);
				//if(sel)
				m_editorMode = MouseMode_None;
				if(this->m_dialogPtr->GetMapEnable() && m_dialogPtr->GetMapGridVisible() && (m_bSetLevelMode || this->m_bDisableMoveMode))
				{
					this->m_editorMode	=	MouseMode_MapRect;
				}
				else
				{
					memset(&m_movePointSide, 0, sizeof(m_movePointSide));
					if(m_moveControl = PickControls(m_movePointSide, m_downPoint))
					{
						switch(m_movePointSide.nPickType)
						{
						case PickType_Body:
							{
								if(GetCopyKeyboard())
									m_editorMode	=	MouseMode_MoveCopy;
								else
									m_editorMode	=	MouseMode_MoveControl;
							}
							break;
						case PickType_MoveSide:
							{
								m_editorMode	=	MouseMode_SizeControl;
							}
							break;
						case PickType_Unit2DCenter:
							{
								m_editorMode	=	MouseMode_Unit2DCenterPos;
							}
							break;
						}
					}
					m_movePoints.clear();
					m_moveCopys.clear();
					m_moveCenters.clear();
					m_moveRotCents.clear();
					if(m_editorMode != MouseMode_None)
					{
						if(this->m_bIsReadOnly){
							m_editorMode = MouseMode_None;
							GetMainFrame()->MessageBox(Language(L"ReadOnly", L"这个文件只读，不能进行编辑"), 0, MB_OK);
							break;
						}

						for(int i=0;i<m_controlSelects.size();i++)
						{
							Phantom::Rect rc	=	m_controlSelects[i]->GetWindowRect();
							m_moveCopys.push_back( *(CRect*)&rc );
							Phantom::Pixel rotc = m_controlSelects[i]->GetRotCenter();
							m_moveRotCents.push_back((CPoint&)rotc);
							Phantom::float2 cf((float)rotc.x / (float)rc.GetWidth(), (float)rotc.y / (float)rc.GetHeight());
							m_moveCenters.push_back(cf);
							if(m_movePointSide.nPickType == this->PickType_MoveSide)
							{
								switch(m_movePointSide.nMovePointSide)
								{
								case MovePointSide_Left:
									m_movePoints.push_back(CPoint(rc.left - m_downPoint.x, rc.top - m_downPoint.y));
									break;
								case MovePointSide_Right:
									m_movePoints.push_back(CPoint(rc.right - m_downPoint.x, rc.top - m_downPoint.y));
									break;
								case MovePointSide_Top:
									m_movePoints.push_back(CPoint(rc.left - m_downPoint.x, rc.top - m_downPoint.y));
									break;
								case MovePointSide_Bottom:
									m_movePoints.push_back(CPoint(rc.left - m_downPoint.x, rc.bottom - m_downPoint.y));
									break;
								case MovePointSide_LeftTop:
									m_movePoints.push_back(CPoint(rc.left - m_downPoint.x, rc.top - m_downPoint.y));
									break;
								case MovePointSide_LeftBottom:
									m_movePoints.push_back(CPoint(rc.left - m_downPoint.x, rc.bottom - m_downPoint.y));
									break;
								case MovePointSide_RightTop:
									m_movePoints.push_back(CPoint(rc.right - m_downPoint.x, rc.top - m_downPoint.y));
									break;
								case MovePointSide_RightBottom:
									m_movePoints.push_back(CPoint(rc.right - m_downPoint.x, rc.bottom - m_downPoint.y));
									break;
								case MovePointSide_Center:
									{
										Phantom::Pixel center	=	m_controlSelects[i]->GetRotCenter();
										m_movePoints.push_back(CPoint((center.x + rc.left) - m_downPoint.x, (center.y + rc.top) - m_downPoint.y));
									}
									break;
								case MovePointSide_Rotation:
									{
										m_movePoints.push_back(CPoint((int)m_controlSelects[i]->GetRotationAngle(), m_downPoint.y));
									}
									break;
								}
							}
							else if(m_movePointSide.nPickType == this->PickType_Body)
							{
								m_movePoints.push_back(CPoint(rc.left - m_downPoint.x, rc.top - m_downPoint.y));
							}
							//else if(m_movePointSide.nPickType == this->PickType_Unit2DCenter)
							//{
							//	Phantom::Pixel pos(rc.left, rc.top);
							//	if(m_controlSelects[i]->GetUnit2D())
							//		pos	=	m_controlSelects[i]->GetUnit2D()->GetCenterPos();
							//	m_movePoints.push_back(CPoint(pos.x - m_downPoint.x, pos.y - m_downPoint.y));
							//}
						}
					}
				}
				//
				if(m_editorMode == MouseMode_None)
				{
					m_editorMode	=	MouseMode_SelectRect;
				}
			}
		}
		break;
	case WM_KEYDOWN:
		{
			int offsetX	=	0;
			int offsetY	=	0;
			switch(wParam)
			{
			case VK_LEFT:
				offsetX--;
				break;
			case VK_RIGHT:
				offsetX++;
				break;
			case VK_UP:
				offsetY--;
				break;
			case VK_DOWN:
				offsetY++;
				break;
			}
			if(offsetX != 0 || offsetY != 0)
			{
				for(int i=0;i<this->m_controlSelects.size();i++)
				{
					Phantom::Rect rc	=	m_controlSelects[i]->GetWindowRect();
					rc.Offset(offsetX, offsetY);
					m_controlSelects[i]->SetWindowRect(RectToDlgRect(rc, m_dialogPtr));
					this->SetModifyed(true);
				}
			}
		}
		break;
	case WM_KEYUP:
		{
			if(wParam == VK_ESCAPE)
			{
				if(m_bLButtonDown)
				{
					if(m_editorMode == MouseMode_InsertControl)
					{
						if(m_insertControl)
							m_dialogPtr->DeleteControlPtr(m_insertControl);
						m_insertControl		=	0;
					}
				}
				//
				this->m_nAlignType	=	ToAlign_None;
				m_nEditorFlags		=	UIControlType_NONE;
				m_editorMode		=	MouseMode_None;
				m_bLButtonDown	=	false;
			}
			else if(wParam == VK_DELETE)
			{
				if(m_controlSelects.size() > 0)
				{
					if(MessageBox(this->GetViewHandle(), Language(L"是否删除所有选中项?"), Language(L"提示"), MB_YESNO) == IDNO)
						return;
					std::map<int,bool> groups;
					for(int i=0;i<m_controlSelects.size();i++)
					{
						Phantom::UIControl* control	=	m_controlSelects[i];
						if(!control)
							continue;
						groups[control->GetGroupID()] = true;
						m_dialogPtr->DeleteControlPtr(control);
						this->SetModifyed(true);
					}
					for(std::map<int,bool>::iterator it = groups.begin(); it != groups.end(); it++)
					{
						RefreshGroup(it->first);
					}
					ChangeSelectControls();
					m_treeView.ClearSelect();
				}
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			this->m_nAlignType	=	ToAlign_None;
			CPoint pos;
			GetCursorPos(&pos);
			ScreenToClient(hWnd, &pos);
			pos	=	PtToDlgPt(pos, m_dialogPtr);
			if(m_bLButtonDown)
			{
				BOOL bUndo = false;
				m_bLButtonDown	=	false;
				ReleaseCapture();
				switch(m_editorMode)
				{
				case MouseMode_MapRect:
					{
						Phantom::Rect rc(min(m_lastPoint.x, m_downPoint.x), min(m_lastPoint.y, m_downPoint.y), max(m_lastPoint.x, m_downPoint.x), max(m_lastPoint.y, m_downPoint.y));
						int x, y;
						m_dialogPtr->GetLocation(x, y);
						Phantom::Pixel vbegin = m_dialogPtr->GetMapGridIndex(Phantom::Pixel(rc.left-x, rc.top-y));
						Phantom::Pixel vend = m_dialogPtr->GetMapGridIndex(Phantom::Pixel(rc.right-x, rc.bottom-y));
						int beginX	=	vbegin.x;//(int)((float)(rc.left - x) / m_dialogPtr->GetMapGridSize().cx);
						int beginY	=	vbegin.y;//(int)((float)(rc.top - y) / m_dialogPtr->GetMapGridSize().cy);
						int endX	=	vend.x;//(int)((float)(rc.right - x) / m_dialogPtr->GetMapGridSize().cx);
						int endY	=	vend.y;//(int)((float)(rc.bottom - y) / m_dialogPtr->GetMapGridSize().cy);
						BOOL bChanged = false;
						for(int x=beginX;x<=endX;x++)
						{
							for(int y=beginY;y<=endY;y++)
							{
								const Phantom::Map2DGridInfo* info	=	m_dialogPtr->GetMapGridInfo(x, y);
								if(info)
								{
									Phantom::Map2DGridInfo f	=	*info;
									if(this->m_bDisableMoveMode)
									{
										bool bEnable	=	m_gridInfo.isEnabled();
										if(GetSelectKeyboard())
											bEnable		=	!bEnable;
										if(!bEnable)
											f.flags			|=	f.MapGridInfoFlags_Disable;
										else if(f.flags & f.MapGridInfoFlags_Disable)
											f.flags	^=	f.MapGridInfoFlags_Disable;
										this->m_dialogPtr->SetMapGridInfo(x, y, f);
										bChanged=true;
									}
									else
									{
//										f.index	=	m_gridInfo.index;
										//if(GetSelectKeyboard())
										//{
//											if(f.index > 0)
//												f.index = 0;
//											else
//												f.index	=	f.index++;
										//}
										this->m_dialogPtr->SetMapGridInfo(x, y, f);
										bChanged=true;
									}
								}
							}
						}
						if(bChanged)
							this->SetModifyed(true);
					}
					break;
				case MouseMode_InsertControl:
					{
						this->RefreshGroup(m_insertControl->GetGroupID());
						m_insertControl		=	0;
						m_nEditorFlags		=	UIControlType_NONE;
						m_editorMode		=	MouseMode_None;
						this->SetModifyed(true);
					}
					break;
				case MouseMode_TabIndex:
					{
						Phantom::UIControl* sel	=	PickControl(pos);
						if(sel && sel->GetType() != UIControlType_STATIC && sel->GetType() != UIControlType_VIEW_FORMAT_TEXT)
						{
							sel->SetTabIndex(this->m_nTabIndex++);
							this->SetModifyed(true);
						}
					}
					break;
				case MouseMode_MoveControl:
					{
						this->m_animationTrack.OnSelectTab();
						if(this->m_downPoint.x == pos.x && this->m_downPoint.y == pos.y)
						{
							BOOL bControl	=	GetSelectKeyboard();
							if(!bControl)
								m_controlSelects.clear();
							Phantom::UIControl* sel	=	this->PickControl(m_downPoint);
							if(sel)
							{
								if(bControl)
								{
									for(int i=0;i<m_controlSelects.size();i++)
										if(m_controlSelects[i] == sel)
										{
											m_controlSelects.erase(m_controlSelects.begin() + i);
											break;
										}
								}
								m_controlSelects.push_back(sel);
								if(this->m_nEditorFlags == UIEditorFlags_TrackAnimation)
								{
									TreeItemType* type	=	this->GetSelectedItem();
									if(type)
									{
										Phantom::UIControl* control	=	m_dialogPtr->GetControlPtrFromID(type->p1);
										if(control == m_controlSelects[0] && m_controlSelects.size() == 1)
										{
											break;
										}
									}
								}
							}
							this->OnChangeSelect();
						}
						else
							bUndo = true;
					}
					break;
				case MouseMode_MoveCopy:
					{
						if(GetCopyKeyboard())
						{
							if(m_moveCopys.size() != this->m_controlSelects.size())
								break;
							std::map<int, bool> groups;
							std::vector<Phantom::UIControl*> selects;
							for(int i=0;i<m_controlSelects.size();i++)
							{
								Phantom::UIControl* c	=	m_controlSelects[i]->Clone(-1, m_dialogPtr);
								Phantom::Rect rc	=	(Phantom::Rect&)m_moveCopys[i];
								int w		=	rc.GetWidth();
								int h		=	rc.GetHeight();
								rc.left		=	(int)((float)rc.left);
								rc.top		=	(int)((float)rc.top);
								rc.right	=	rc.left + w;
								rc.bottom	=	rc.top + h;
								c->SetWindowRect(RectToDlgRect(rc, m_dialogPtr));
								groups[c->GetGroupID()]	=	true;
								selects.push_back(c);
								c->PlayAction(Phantom::ActionType_Idle);
							}
							this->SetModifyed();
							m_controlSelects	=	selects;
							this->OnChangeSelect();
							for(std::map<int,bool>::iterator it = groups.begin(); it != groups.end();it++)
							{
								this->RefreshGroup(it->first);
							}
						}
					}
					break;
				case MouseMode_SelectRect:
					{
						this->m_animationTrack.OnSelectTab();
						if(m_lastPoint.x == m_downPoint.x && m_lastPoint.y == m_downPoint.y)
						{
							Phantom::UIControl* sel	=	PickControl(pos);
							if(GetSelectKeyboard())
							{
								bool bFind	=	false;
								for(int i=0;i<m_controlSelects.size();i++)
								{
									if(m_controlSelects[i] == sel)
									{
										bFind	=	true;
										if(m_controlSelects.size() == 1)
											m_controlSelects.clear();
										else
											m_controlSelects.erase(m_controlSelects.begin() + i);
										break;
									}
								}
								if(!bFind && sel)
									m_controlSelects.push_back(sel);
							}
							else
							{
								m_controlSelects.clear();
								if(sel)
								{
									m_controlSelects.push_back(sel);
								}
							}
						}
						else
						{
							BOOL bShift	=	GetCopyKeyboard();
							BOOL bAlt	=	HIBYTE(GetKeyState(VK_MENU));
							if( bShift || bAlt )
							{
							}
							else
								m_controlSelects.clear();
							//
							Phantom::Rect rc(min(m_lastPoint.x, m_downPoint.x), min(m_lastPoint.y, m_downPoint.y), max(m_lastPoint.x, m_downPoint.x), max(m_lastPoint.y, m_downPoint.y));
							//
							for(int i=0;i<m_dialogPtr->GetControlCount();i++)
							{
								Phantom::UIControl* sel	=	m_dialogPtr->GetControlPtr(i);
								if(!sel->GetVisible())
									continue;
								Phantom::UIGroup* group = sel->GetGroupPtr();
								if(!group||!group->GetVisible())
									continue;
								if(!sel->GetEnabled()||!sel->m_bSelect)
									continue;
								Phantom::Rect rc2	=	sel->GetWindowRect();
								//
								Phantom::Pixel rp[4];
								rp[0]	=	sel->ClientToScreen(Phantom::Pixel(rc2.left, rc2.top));
								rp[1]	=	sel->ClientToScreen(Phantom::Pixel(rc2.right, rc2.top));
								rp[2]	=	sel->ClientToScreen(Phantom::Pixel(rc2.right, rc2.bottom));
								rp[3]	=	sel->ClientToScreen(Phantom::Pixel(rc2.left, rc2.bottom));
								//
								Phantom::Rect controlRc(10000, 10000, -10000, -10000);
								for(int k=0;k<4;k++)
								{
									controlRc.left		=	min(controlRc.left, rp[k].x);
									controlRc.top		=	min(controlRc.top, rp[k].y);
									controlRc.right		=	max(controlRc.right, rp[k].x);
									controlRc.bottom	=	max(controlRc.bottom, rp[k].y);
								}
								if(IsRectIn(rc, controlRc))
								{
									if(bShift)
									{
										bool bFind	=	false;
										for(int i=0;i<m_controlSelects.size();i++)
										{
											if(m_controlSelects[i] == sel)
											{
												bFind	=	true;
											}
										}
										if(!bFind)
											m_controlSelects.push_back( sel );
									}
									else if(bAlt)
									{
										for(int i=0;i<m_controlSelects.size();i++)
										{
											if(m_controlSelects[i] == sel)
											{
												if(m_controlSelects.size() == 1)
													m_controlSelects.clear();
												else
													m_controlSelects.erase(m_controlSelects.begin() + i);
												break;
											}
										}
									}
									else
										m_controlSelects.push_back( sel );
								}
							}
						}
						if(m_controlSelects.size() > 0)
						{
							if(m_controlSelects.size() > 1)
							{
								g_nearestControlPos.Set(pos.x, pos.y);
								qsort(&m_controlSelects.at(0), m_controlSelects.size(), sizeof(Phantom::UIControl*), SortControlNear);
							}
							if(this->m_nEditorFlags == UIEditorFlags_TrackAnimation)
							{
								TreeItemType* type	=	this->GetSelectedItem();
								if(type)
								{
									Phantom::UIControl* control	=	m_dialogPtr->GetControlPtrFromID(type->p1);
									if(control == m_controlSelects[0] && m_controlSelects.size() == 1)
										break;
								}
							}
							this->OnChangeSelect();
						}
					}
					break;
				}
				if(m_moveCopys.size()==m_controlSelects.size()&&(m_moveCopys.size()>0)&&(bUndo||m_editorMode==MouseMode_SizeControl)){
					BeginUndo();
					int size = m_moveCopys.size();
					UndoRedoBuffer* buff = AddUndoRedo(UI_PROP_MOVE_CONTROL, (ProcUndoRedo)0);
					buff->m_bEnabled = true;
					(*buff) << size;
					for(int i=0;i<m_moveCopys.size();i++){
						(*buff) << m_controlSelects[i]->GetID() << m_moveCopys[i] << m_moveRotCents[i] << m_controlSelects[i]->GetWindowRect() << m_controlSelects[i]->GetRotCenter();
					}
				}
			}
			m_editorMode	=	MouseMode_None;
			m_moveControl	=	0;
			ResetCenter();
		}
		break;
	case WM_MOUSEMOVE:
		{
			CPoint pos;
			GetCursorPos(&pos);
			ScreenToClient(hWnd, &pos);
			pos	=	PtToDlgPt(pos, m_dialogPtr);
			this->m_nAlignType	=	ToAlign_None;
			if(m_bDownMButton)
			{
				int x	=	(int)((float)(pos.x - m_downPoint.x) * m_dialogPtr->m_dlgScale.x);
				int y	=	(int)((float)(pos.y - m_downPoint.y) * m_dialogPtr->m_dlgScale.y);
				m_dialogPtr->SetLocation(m_downDlg.x + x, m_downDlg.y + y);
			}
			else if(m_bLButtonDown)
			{
				if(this->m_bIsReadOnly)break;
				switch(m_editorMode)
				{
				case MouseMode_InsertControl:
					{
						CPoint oldPos	=	pos;
						AlignToGrid(pos);
						Phantom::Rect	rcTemp(m_downPoint.x, m_downPoint.y, pos.x, pos.y);
						Phantom::Rect	rc(min(rcTemp.left, rcTemp.right), min(rcTemp.top, rcTemp.bottom), max(rcTemp.left, rcTemp.right), max(rcTemp.top, rcTemp.bottom));
						int w		=	rc.GetWidth();
						int h		=	rc.GetHeight();
						rc.left		=	(int)((float)rc.left);
						rc.top		=	(int)((float)rc.top);
						rc.right	=	rc.left + w;
						rc.bottom	=	rc.top + h;
						m_insertControl->SetWindowRect(RectToDlgRect(rc, m_dialogPtr));
						m_insertControl->SetRotCenter(Phantom::Pixel(rc.GetWidth()/2, rc.GetHeight()/2));
					}
					break;
				case MouseMode_MapRect:
				case MouseMode_SelectRect:
					{
						m_lastPoint	=	pos;
					}
					break;
				case MouseMode_SizeControl:
					{
						if(m_movePoints.size() != m_controlSelects.size() || m_moveCopys.size()  != m_controlSelects.size())
							break;
						if(m_moveControl)
						{
							CPoint savePos = pos;
							Phantom::Rect rc	=	m_moveControl->GetWindowRect();
							for(int i=0;i<this->m_controlSelects.size();i++)
							{
								if(m_controlSelects[i] == m_moveControl)
									savePos	=	m_movePoints[i];
							}
							{
								CPoint movePos(pos.x + savePos.x, pos.y + savePos.y);
								savePos	=	movePos;
								CPoint offset;
								if(AlignToGrid(movePos))
								{
									pos.x		+=	(movePos.x - savePos.x);
									pos.y		+=	(movePos.y - savePos.y);
								}
							}
						}
						for(int i=0;i<this->m_controlSelects.size();i++)
						{
							Phantom::Pixel clientDown	=	m_controlSelects[i]->ScreenToClient((Phantom::Pixel&)m_downPoint);
							Phantom::Pixel clientPos	=	m_controlSelects[i]->ScreenToClient((Phantom::Pixel&)pos);
							Phantom::Rect& rcOld		=	(Phantom::Rect&)m_moveCopys[i];
							//
							Phantom::Rect rc			=	(Phantom::Rect&)m_moveCopys[i];//m_controlSelects[i]->GetWindowRect();
							CPoint savePos		=	m_movePoints[i];
							int width			=	rc.GetWidth();
							int height			=	rc.GetHeight();
							//
							switch(this->m_movePointSide.nMovePointSide)
							{
							case MovePointSide_Left:
								m_nAlignType	=	(m_nAlignType & (ToAlign_Left | ToAlign_Right));
								rc.left			=	clientPos.x + (rcOld.left - clientDown.x);
								break;
							case MovePointSide_Right:
								m_nAlignType	=	(m_nAlignType & (ToAlign_Left | ToAlign_Right));
								rc.right		=	clientPos.x + (rcOld.right - clientDown.x);
								break;
							case MovePointSide_Top:
								m_nAlignType	=	(m_nAlignType & (ToAlign_Top | ToAlign_Bottom));
								rc.top			=	clientPos.y + (rcOld.top - clientDown.y);
								break;
							case MovePointSide_Bottom:
								m_nAlignType	=	(m_nAlignType & (ToAlign_Top | ToAlign_Bottom));
								rc.bottom		=	clientPos.y + (rcOld.bottom - clientDown.y);
								break;
							case MovePointSide_LeftTop:
								rc.left			=	clientPos.x + (rcOld.left - clientDown.x);
								rc.top			=	clientPos.y + (rcOld.top - clientDown.y);
								break;
							case MovePointSide_LeftBottom:
								rc.left			=	clientPos.x + (rcOld.left - clientDown.x);
								rc.bottom		=	clientPos.y + (rcOld.bottom - clientDown.y);
								break;
							case MovePointSide_RightTop:
								rc.right		=	clientPos.x + (rcOld.right - clientDown.x);
								rc.top			=	clientPos.y + (rcOld.top - clientDown.y);
								break;
							case MovePointSide_RightBottom:
								rc.right		=	clientPos.x + (rcOld.right - clientDown.x);
								rc.bottom		=	clientPos.y + (rcOld.bottom - clientDown.y);
								break;
							case MovePointSide_Center:
								{
									Phantom::Pixel center(pos.x + savePos.x - rc.left, pos.y + savePos.y - rc.top);
									if(m_nEditorFlags == UIEditorFlags_TrackAnimation)
									{
										Phantom::UIAnimationTrack* ani	=	m_controlSelects[i]->GetActiveTrack();
										if(!ani)
											break;
										int frameIndex	=	(int)m_animationTrack.GetPlayValue();
										if(frameIndex >= ani->GetFrameCount())
											frameIndex = ani->GetFrameCount() - 1;
										if(m_animationTrack.IsCanEditor(ani->GetAnimationPtr(Phantom::ControlTrackType_CenterX)))
											ani->GetAnimationPtr(Phantom::ControlTrackType_CenterX)->InsertKey(frameIndex, center.x - m_controlSelects[i]->GetSavedRotCenter().x);
										if(m_animationTrack.IsCanEditor(ani->GetAnimationPtr(Phantom::ControlTrackType_CenterY)))
											ani->GetAnimationPtr(Phantom::ControlTrackType_CenterY)->InsertKey(frameIndex, center.y - m_controlSelects[i]->GetSavedRotCenter().y);
									}
									else
										m_controlSelects[i]->SetRotCenter(center);
									this->SetModifyed(true);
								}
								continue;
							case MovePointSide_Rotation:
								{
									int offset	=	(pos.y - savePos.y) + savePos.x;
									if(m_nEditorFlags == UIEditorFlags_TrackAnimation)
									{
										Phantom::UIAnimationTrack* ani	=	m_controlSelects[i]->GetActiveTrack();
										if(!ani)
											break;
										int frameIndex	=	(int)m_animationTrack.GetPlayValue();
										if(frameIndex >= ani->GetFrameCount())
											frameIndex = ani->GetFrameCount() - 1;
										if(m_animationTrack.IsCanEditor(ani->GetAnimationPtr(Phantom::ControlTrackType_Rotation)))
											ani->GetAnimationPtr(Phantom::ControlTrackType_Rotation)->InsertKey(frameIndex, offset - m_controlSelects[i]->GetSavedRotation());
									}
									else
										m_controlSelects[i]->SetRotation(m_controlSelects[i]->IsRotation(), (float)offset);
									this->SetModifyed(true);
								}
								continue;
							}
							//
							if(m_nEditorFlags == UIEditorFlags_TrackAnimation)
							{
								Phantom::UIAnimationTrack* ani	=	m_controlSelects[i]->GetActiveTrack();
								if(!ani)
									break;
								int frameIndex	=	(int)m_animationTrack.GetPlayValue();
								if(frameIndex >= ani->GetFrameCount())
									frameIndex = ani->GetFrameCount() - 1;
								Phantom::Rect saveRc	=	m_controlSelects[i]->GetSavedWindowRect();
								if(m_animationTrack.IsCanEditor(ani->GetAnimationPtr(Phantom::ControlTrackType_RectX)))
									ani->GetAnimationPtr(Phantom::ControlTrackType_RectX)->InsertKey(frameIndex, (float)rc.left - saveRc.left);
								if(m_animationTrack.IsCanEditor(ani->GetAnimationPtr(Phantom::ControlTrackType_RectY)))
									ani->GetAnimationPtr(Phantom::ControlTrackType_RectY)->InsertKey(frameIndex, (float)rc.top - saveRc.top);
								if(m_animationTrack.IsCanEditor(ani->GetAnimationPtr(Phantom::ControlTrackType_RectW)))
									ani->GetAnimationPtr(Phantom::ControlTrackType_RectW)->InsertKey(frameIndex, (float)rc.GetWidth() - saveRc.GetWidth());
								if(m_animationTrack.IsCanEditor(ani->GetAnimationPtr(Phantom::ControlTrackType_RectH)))
									ani->GetAnimationPtr(Phantom::ControlTrackType_RectH)->InsertKey(frameIndex, (float)rc.GetHeight() - saveRc.GetHeight());
							}
							else
							{
								int w		=	rc.GetWidth();
								int h		=	rc.GetHeight();
								rc.left		=	(int)((float)rc.left);
								rc.top		=	(int)((float)rc.top);
								rc.right	=	rc.left + w;
								rc.bottom	=	rc.top + h;
								m_controlSelects[i]->SetWindowRect(RectToDlgRect(rc, m_dialogPtr));
								Phantom::Pixel pos(m_moveCenters[i].x * (float)rc.GetWidth(), m_moveCenters[i].y * (float)rc.GetHeight());
								m_controlSelects[i]->SetRotCenter(pos);
							}
							this->SetModifyed(true);
						}
					}
					break;
				case MouseMode_MoveControl:
					{
						if(m_movePoints.size() != m_controlSelects.size())
							break;
						{
							if(m_moveControl)
							{
								CPoint savePos = pos;
								Phantom::Rect rc	=	m_moveControl->GetWindowRect();
								for(int i=0;i<this->m_controlSelects.size();i++)
								{
									if(m_controlSelects[i] == m_moveControl)
										savePos	=	m_movePoints[i];
								}
								if(!IsShowUIGrid())
								{
									//for(int c=0;c<this->m_dialogPtr->GetControlCount();c++)
									//{
									//	Phantom::UIControl* control = m_dialogPtr->GetControlPtr(c);
									//	BOOL bFind = false;
									//	for(int i=0;i<this->m_controlSelects.size();i++)
									//	{
									//		if(m_controlSelects[i] == control)
									//		{
									//			bFind = true;
									//		}
									//	}
									//	if(bFind)
									//		continue;
									//	Phantom::Rect tempMove = m_moveControl->GetWindowRect();
									//	Phantom::Rect cMove = control->GetWindowRect();
									//	if(abs(tempMove.left-cMove.left)<5)
									//		pos.x += tempMove.left-cMove.left;
									//	else if(abs(tempMove.right-cMove.left)<5)
									//		pos.x += tempMove.right-cMove.left;
									//	else if(abs(tempMove.left-cMove.right)<5)
									//		pos.x += tempMove.left-cMove.right;
									//	else if(abs(tempMove.right-cMove.right)<5)
									//		pos.x += tempMove.right-cMove.right;
									//}
								}
								else
								{
									CPoint movePos(pos.x + savePos.x, pos.y + savePos.y);
									CPoint offset;
									if(AlignRectToGrid(offset, movePos, rc))
									{
										pos.x		+=	offset.x;
										pos.y		+=	offset.y;
									}
								}
							}
							std::map<Phantom::UIGroup*, Phantom::Pixel> groups;
							for(int i=0;i<this->m_controlSelects.size();i++)
							{
								Phantom::Rect rc		=	(Phantom::Rect&)m_moveCopys[i];//m_controlSelects[i]->GetWindowRect();
								CPoint savePos	=	m_movePoints[i];
								int width	=	rc.GetWidth();
								int height	=	rc.GetHeight();
								rc.left		=	pos.x + savePos.x;
								rc.top		=	pos.y + savePos.y;
								rc.right	=	rc.left + width;
								rc.bottom	=	rc.top + height;
								//
								if(m_nEditorFlags == UIEditorFlags_TrackAnimation)
								{
									Phantom::UIAnimationTrack* ani	=	m_controlSelects[i]->GetActiveTrack();
									if(!ani)
										break;
									int frameIndex	=	(int)m_animationTrack.GetPlayValue();
									if(frameIndex >= ani->GetFrameCount())
										frameIndex = ani->GetFrameCount() - 1;
									Phantom::Rect saveRc	=	m_controlSelects[i]->GetSavedWindowRect();
									if(m_animationTrack.IsCanEditor(ani->GetAnimationPtr(Phantom::ControlTrackType_RectX)))
										ani->GetAnimationPtr(Phantom::ControlTrackType_RectX)->InsertKey(frameIndex, (float)rc.left - saveRc.left);
									if(m_animationTrack.IsCanEditor(ani->GetAnimationPtr(Phantom::ControlTrackType_RectY)))
										ani->GetAnimationPtr(Phantom::ControlTrackType_RectY)->InsertKey(frameIndex, (float)rc.top - saveRc.top);
									if(m_animationTrack.IsCanEditor(ani->GetAnimationPtr(Phantom::ControlTrackType_RectW)))
										ani->GetAnimationPtr(Phantom::ControlTrackType_RectW)->InsertKey(frameIndex, (float)rc.GetWidth() - saveRc.GetWidth());
									if(m_animationTrack.IsCanEditor(ani->GetAnimationPtr(Phantom::ControlTrackType_RectH)))
										ani->GetAnimationPtr(Phantom::ControlTrackType_RectH)->InsertKey(frameIndex, (float)rc.GetHeight() - saveRc.GetHeight());
								}
								else
								{
									if(HIBYTE(GetKeyState(VK_SHIFT))){
										Phantom::Rect old = (Phantom::Rect&)m_moveCopys[i];
										int w = old.left - rc.left;
										int h = old.top - rc.top;
										if(abs(w)>5||abs(h)>5){
										if(abs(w)>abs(h)){
											rc.top = old.top;
											rc.bottom = old.bottom;
										}
										else
										{
											rc.left = old.left;
											rc.right = old.right;
										}
										}
									}
									m_controlSelects[i]->SetWindowRect(RectToDlgRect(rc, m_dialogPtr));
									if(m_controlSelects[i]->GetGroupPtr()->GetGroupIsUnit() && m_controlSelects[i]->GetGroupID() > 0)
									{
										if(GetSelectKeyboard())
										{
											Phantom::Pixel pos	=	m_controlSelects[i]->GetGroupPtr()->GetPosition();
											Phantom::Rect rcW	=	m_controlSelects[i]->GetRenderRect();
											m_controlSelects[i]->SetRotCenter(Phantom::Pixel(pos.x - rcW.left, pos.y - rcW.top));
										}
										else
											groups[m_controlSelects[i]->GetGroupPtr()] = m_controlSelects[i]->GetCenterPos();
									}
								}
								this->SetModifyed(true);
							}
							if(GetSelectKeyboard())
							{
							}
							else
							{
								for(std::map<Phantom::UIGroup*, Phantom::Pixel>::iterator it = groups.begin(); it!=groups.end();it++)
								{
									it->first->SetPosition(it->second);
								}
							}
						}
					}
					break;
				case MouseMode_Unit2DCenterPos:
					{
						if(m_movePoints.size() != m_controlSelects.size())
							break;
						{
							if(m_moveControl)
							{
								CPoint savePos = pos;
								Phantom::Rect rc	=	m_moveControl->GetWindowRect();
								for(int i=0;i<this->m_controlSelects.size();i++)
								{
									if(m_controlSelects[i] == m_moveControl)
										savePos	=	m_movePoints[i];
								}
								//{
								//	CPoint movePos(pos.x + savePos.x, pos.y + savePos.y);
								//	CPoint offset;
								//	if(AlignRectToGrid(offset, movePos, rc))
								//	{
								//		pos.x		+=	offset.x;
								//		pos.y		+=	offset.y;
								//	}
								//}
							}
							//for(int i=0;i<this->m_controlSelects.size();i++)
							//{
							//	if(!m_controlSelects[i]->GetUnit2D())
							//		continue;
							//	Phantom::Pixel centerPos	=	m_controlSelects[i]->GetUnit2D()->GetCenterPos();
							//	CPoint savePos	=	m_movePoints[i];
							//	centerPos.x		=	pos.x + savePos.x;
							//	centerPos.y		=	pos.y + savePos.y;
							//	m_controlSelects[i]->GetUnit2D()->SetCenterPos(centerPos);
							//	this->SetModifyed(true);
							//}
						}
					}
					break;
				case MouseMode_MoveCopy:
					{
						if(m_movePoints.size() != m_controlSelects.size())
							break;
						{
							if(m_moveControl)
							{
								CPoint savePos = pos;
								Phantom::Rect rc	=	m_moveControl->GetWindowRect();
								for(int i=0;i<this->m_controlSelects.size();i++)
								{
									if(m_controlSelects[i] == m_moveControl)
										savePos	=	m_movePoints[i];
								}
								{
									CPoint movePos(pos.x + savePos.x, pos.y + savePos.y);
									CPoint offset;
									if(AlignRectToGrid(offset, movePos, rc))
									{
										pos.x		+=	offset.x;
										pos.y		+=	offset.y;
									}
								}
							}
							m_moveCopys.clear();
							for(int i=0;i<this->m_controlSelects.size();i++)
							{
								Phantom::Rect rc	=	m_controlSelects[i]->GetWindowRect();
								CPoint savePos	=	m_movePoints[i];
								int width	=	rc.GetWidth();
								int height	=	rc.GetHeight();
								rc.left		=	pos.x + savePos.x;
								rc.top		=	pos.y + savePos.y;
								rc.right	=	rc.left + width;
								rc.bottom	=	rc.top + height;
								m_moveCopys.push_back( (CRect&)rc );
							}
						}
					}
					break;
				}
			}
		}
		break;
	}
}

VOID		GCViewUIEditor::OnSetFloorMode(const CPoint& pos)
{
	if(!m_moveControl)
		return;
}
