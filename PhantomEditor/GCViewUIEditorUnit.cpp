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
extern	CPoint		PtToDlgPt(const CPoint& ptIn, Phantom::UIDialog* dlg);
#ifdef USE_BOX
Phantom::float2		GCViewUIEditor::PtoB(const Phantom::Pixel& p)	//Pixel to Box2D
{
	if(!m_dialogPtr->m_box2World.IsEnable())
		return Phantom::float2(0, 0);
	return m_dialogPtr->m_box2World.GetScreenPosSize(p);
}
Phantom::Pixel		GCViewUIEditor::BtoP(const Phantom::float2& v)
{
	if(!m_dialogPtr->m_box2World.IsEnable())
		return Phantom::Pixel(0, 0);
	return m_dialogPtr->m_box2World.GetScreenPixelSize(v);
}

float		GCViewUIEditor::PtoB(int n)	//Pixel to Box2D
{
	return m_dialogPtr->m_box2World.GetScreenPosY(n);
}

int			GCViewUIEditor::BtoP(float f)
{
	return m_dialogPtr->m_box2World.GetScreenPixelY(f);
}
BOOL	GCViewUIEditor::IsPtIn(const Phantom::float2& v, const Phantom::Pixel& cursor, Phantom::Pixel& ret)
{
	Phantom::Pixel c	=	BtoP(v);
	Phantom::Rect rc(c.x - 10, c.y - 10, c.x + 10, c.y + 10);
	if(rc.isPtIn(cursor.x, cursor.y))
	{
		ret	=	c;
		return true;
	}
	return false;
}

VOID	GCViewUIEditor::RenderRect(const Phantom::float2& v)
{
	Phantom::Pixel c	=	BtoP(v);
	Phantom::Rect rc(c.x - 10, c.y - 10, c.x + 10, c.y + 10);
	Phantom::g_manager.DrawRect2D( rc, 0xffff0000, false);
}
VOID	GCViewUIEditor::RenderRect(Phantom::B2Body* b)
{
	Phantom::rect_f r	=	b->GetBodyRect();
	Phantom::Pixel p1	=	BtoP(Phantom::float2(r.left, r.top));
	Phantom::Pixel p2	=	BtoP(Phantom::float2(r.right, r.bottom));
	Phantom::Rect	rc(min(p1.x, p2.x), min(p1.y, p2.y), max(p1.x, p2.x), max(p1.y, p2.y));
	Phantom::g_manager.DrawRect2D( rc, 0xff00ff00, false);
	for(int i=0;i<b->GetShapeCount();i++)
	{
		Phantom::B2Shape* bs	=	b->GetShapePtr(i);
		switch(bs->m_data.type)
		{
		case b2Shape::e_circle:
			{
				b2Vec2 center = bs->ToBodyPos(bs->m_data.m_p);
				float32 radius = bs->m_data.m_radius;
				RenderRect(Phantom::float2(center.x + radius, center.y));
			}
			break;
		case b2Shape::e_edge:
			{
				b2Vec2 v1 = bs->ToBodyPos(bs->m_data.m_vertex1);
				b2Vec2 v2 = bs->ToBodyPos(bs->m_data.m_vertex2);
				RenderRect(Phantom::float2(v1.x, v1.y));
				RenderRect(Phantom::float2(v2.x, v2.y));
			}
			break;
		case b2Shape::e_chain:
			{
				int32 count = bs->m_vertices.size();
				if(count > 0)
				{
					for (int32 i = 0; i < count; ++i)
					{
						b2Vec2 v2 = bs->ToBodyPos(bs->m_vertices[i]);
						RenderRect(Phantom::float2(v2.x, v2.y));
					}
				}
			}
			break;
		case b2Shape::e_polygon:
			{
				int32 count = bs->m_vertices.size();
				if(count > 0)
				{
					int32 vertexCount	= count;
					for (int32 i = 0; i < vertexCount; ++i)
					{
						b2Vec2 v2 = bs->ToBodyPos(bs->m_vertices[i]);
						RenderRect(Phantom::float2(v2.x, v2.y));
					}
				}
			}
			break;
		case Phantom::B2ShapeData::e_polygon_box:
			{
				rect_f temp(bs->m_data.m_p.x - bs->m_data.m_boxWidth * 0.5f, bs->m_data.m_p.y - bs->m_data.m_boxHeight * 0.5f, bs->m_data.m_p.x + bs->m_data.m_boxWidth * 0.5f, bs->m_data.m_p.y + bs->m_data.m_boxHeight * 0.5f);
				b2Vec2 v2	=	bs->ToBodyPos(Phantom::float2(temp.left, temp.top));
				b2Vec2 v3	=	bs->ToBodyPos(Phantom::float2(temp.right, temp.bottom));
				RenderRect(Phantom::float2(v2.x, v2.y));
				RenderRect(Phantom::float2(v3.x, v3.y));
			}
			break;
		default:
			break;
		}
	}
}

BOOL	GCViewUIEditor::GetMovePointRect(Phantom::B2Joint* b, const Phantom::Pixel& cursor, Phantom::Pixel& ret, int & nSide)
{
	nSide		=	0;
	{
		Phantom::Pixel p	=	BtoP(b->m_data.localAnchorA);
		Phantom::Rect	rc(p.x - 10, p.y - 10, p.x + 10, p.y + 10);
		if(rc.isPtIn(cursor.x, cursor.y))
		{
			ret	=	p;
			return true;
		}
	}
	switch(b->m_data.jointType)
	{
	case e_distanceJoint:
		break;
	case e_revoluteJoint:
	case e_prismaticJoint:
	case e_pulleyJoint:
	case e_mouseJoint:
	case e_gearJoint:
	case e_wheelJoint:
	case e_weldJoint:
	case e_frictionJoint:
	case e_ropeJoint:
		return false;
	}
	{
		nSide		=	1;
		Phantom::Pixel p	=	BtoP(b->m_data.localAnchorB);
		Phantom::Rect	rc(p.x - 10, p.y - 10, p.x + 10, p.y + 10);
		if(rc.isPtIn(cursor.x, cursor.y))
		{
			ret	=	p;
			return true;
		}
	}
	return false;
}

//返回全部可移动点操作对象,8个
BOOL	GCViewUIEditor::GetMovePointRect(Phantom::B2Body* b, const Phantom::Pixel& cursor, Phantom::Pixel& ret, int & nSide)
{
	nSide		=	0;
	Phantom::rect_f r	=	b->GetBodyRect();
	Phantom::Pixel p1	=	BtoP(Phantom::float2(r.left, r.top));
	Phantom::Pixel p2	=	BtoP(Phantom::float2(r.right, r.bottom));
	Phantom::Rect	rc(min(p1.x, p2.x), min(p1.y, p2.y), max(p1.x, p2.x), max(p1.y, p2.y));
	BOOL bRet	=	false;
	if(rc.isPtIn(cursor.x, cursor.y))
		bRet	=	true;
	ret	=	BtoP(b->GetPosition());
	for(int i=0;i<b->GetShapeCount();i++)
	{
		Phantom::B2Shape* bs	=	b->GetShapePtr(i);
		switch(bs->m_data.type)
		{
		case b2Shape::e_circle:
			{
				b2Vec2 center = bs->ToBodyPos(bs->m_data.m_p);
				float32 radius = bs->m_data.m_radius;
				if(IsPtIn(Phantom::float2(center.x + radius, center.y), cursor, ret))
					nSide	=	1;
			}
			break;
		case b2Shape::e_edge:
			{
				b2Vec2 v0 = bs->ToBodyPos(bs->m_data.m_vertex0);
				b2Vec2 v3 = bs->ToBodyPos(bs->m_data.m_vertex3);
				b2Vec2 v1 = bs->ToBodyPos(bs->m_data.m_vertex1);
				b2Vec2 v2 = bs->ToBodyPos(bs->m_data.m_vertex2);
				if(IsPtIn(Phantom::float2(v1.x, v1.y), cursor, ret))
					nSide	=	1;
				else if(IsPtIn(Phantom::float2(v2.x, v2.y), cursor, ret))
					nSide	=	2;
				else if(bs->m_data.m_hasVertex0 && IsPtIn(Phantom::float2(v0.x, v0.y), cursor, ret))
					nSide	=	3;
				else if(bs->m_data.m_hasVertex3 && IsPtIn(Phantom::float2(v3.x, v3.y), cursor, ret))
					nSide	=	4;
			}
			break;
		case b2Shape::e_chain:
			{
				int32 count = bs->m_vertices.size();
				if(count > 0)
				{
					for (int32 i = 0; i < count; ++i)
					{
						b2Vec2 v2 = bs->ToBodyPos(bs->m_vertices[i]);
						if(IsPtIn(Phantom::float2(v2.x, v2.y), cursor, ret))
							nSide	=	i + 1;
					}
				}
			}
			break;
		case b2Shape::e_polygon:
			{
				int32 count = bs->m_vertices.size();
				if(count > 0)
				{
					int32 vertexCount	= count;
					for (int32 i = 0; i < vertexCount; ++i)
					{
						b2Vec2 v2 = bs->ToBodyPos(bs->m_vertices[i]);
						if(IsPtIn(Phantom::float2(v2.x, v2.y), cursor, ret))
							nSide	=	i + 1;
					}
				}
			}
			break;
		case Phantom::B2ShapeData::e_polygon_box:
			{
				rect_f temp(bs->m_data.m_p.x - bs->m_data.m_boxWidth * 0.5f, bs->m_data.m_p.y - bs->m_data.m_boxHeight * 0.5f, bs->m_data.m_p.x + bs->m_data.m_boxWidth * 0.5f, bs->m_data.m_p.y + bs->m_data.m_boxHeight * 0.5f);
				b2Vec2 v2	=	bs->ToBodyPos(Phantom::float2(temp.left, temp.top));
				b2Vec2 v3	=	bs->ToBodyPos(Phantom::float2(temp.right, temp.bottom));
				if(IsPtIn(Phantom::float2(v3.x, v3.y), cursor, ret))
					nSide	=	2;
			}
			break;
		default:
			break;
		}
	}
	return (bRet || nSide > 0);
}

//返回全部可移动点操作对象,8个
BOOL	GCViewUIEditor::OnMoveBody(GCViewUIEditor::BodySelect& sel, const Phantom::Pixel& posIn)
{
	int nSide	=	sel.nSide;
	if(nSide == 0)
	{
		if(sel.bodyPtr)
			sel.bodyPtr->SetPosition(PtoB(posIn));
		else if(sel.jointPtr)
			sel.jointPtr->m_data.localAnchorA = PtoB(posIn);
		return true;
	}
	Phantom::Pixel newPos	=	posIn;
	//newPos	+=	sel.offset;
	if(sel.jointPtr)
	{
		sel.jointPtr->m_data.localAnchorB = PtoB(newPos);
		return true;
	}
	for(int i=0;i<sel.bodyPtr->GetShapeCount();i++)
	{
		Phantom::B2Shape* bs	=	sel.bodyPtr->GetShapePtr(i);
		switch(bs->m_data.type)
		{
		case b2Shape::e_circle:
			{
				b2Vec2 center = bs->ToBodyPos(bs->m_data.m_p);
				float32 radius = bs->m_data.m_radius;
				if(nSide == 1)
				{
					Phantom::float2 from(center.x, center.y);
					Phantom::float2 to			=	PtoB(newPos);
					Phantom::float2 dir			=	to - from;
					bs->m_data.m_radius	=	dir.length();
				}
			}
			break;
		case b2Shape::e_edge:
			{
				Phantom::float2 to			=	PtoB(newPos);
				if(nSide == 1)
					bs->m_data.m_vertex1.setxy(to.x - sel.bodyPtr->GetPosition().x, to.y - sel.bodyPtr->GetPosition().y);
				else if(nSide == 2)
					bs->m_data.m_vertex2.setxy(to.x - sel.bodyPtr->GetPosition().x, to.y - sel.bodyPtr->GetPosition().y);
				else if(nSide == 3)
					bs->m_data.m_vertex0.setxy(to.x - sel.bodyPtr->GetPosition().x, to.y - sel.bodyPtr->GetPosition().y);
				else if(nSide == 4)
					bs->m_data.m_vertex3.setxy(to.x - sel.bodyPtr->GetPosition().x, to.y - sel.bodyPtr->GetPosition().y);
			}
			break;
		case b2Shape::e_chain:
			{
				int32 count = bs->m_vertices.size();
				if(count > 0)
				{
					for (int32 i = 0; i < count; ++i)
					{
						if((i+1) == nSide)
						{
							Phantom::float2 to			=	PtoB(newPos);
							bs->m_vertices[i].setxy(to.x - sel.bodyPtr->GetPosition().x, to.y - sel.bodyPtr->GetPosition().y);
							break;
						}
					}
				}
			}
			break;
		case b2Shape::e_polygon:
			{
				int32 count = bs->m_vertices.size();
				if(count > 0)
				{
					int32 vertexCount	= count;
					for (int32 i = 0; i < vertexCount; ++i)
					{
						if((i+1) == nSide)
						{
							Phantom::float2 to			=	PtoB(newPos);
							bs->m_vertices[i].setxy(to.x - sel.bodyPtr->GetPosition().x, to.y - sel.bodyPtr->GetPosition().y);
							break;
						}
					}
				}
			}
			break;
		case Phantom::B2ShapeData::e_polygon_box:
			{
				rect_f temp(bs->m_data.m_p.x - bs->m_data.m_boxWidth * 0.5f, bs->m_data.m_p.y - bs->m_data.m_boxHeight * 0.5f, bs->m_data.m_p.x + bs->m_data.m_boxWidth * 0.5f, bs->m_data.m_p.y + bs->m_data.m_boxHeight * 0.5f);
				b2Vec2 v2	=	bs->ToBodyPos(Phantom::float2(temp.left, temp.top));
				b2Vec2 v3	=	bs->ToBodyPos(Phantom::float2(temp.right, temp.bottom));
				//
				if(nSide == 2)
				{
					Phantom::float2 from			=	PtoB(sel.downPoint);
					Phantom::float2 to			=	PtoB(newPos);
					Phantom::float2 dir(to.x - v2.x, to.y - v2.y);
					if(dir.x > 0)
						bs->m_data.m_boxWidth	=	dir.x;
					if(dir.y > 0)
						bs->m_data.m_boxHeight = dir.y;
				}
			}
			break;
		default:
			break;
		}
	}
	return true;
}

BOOL		GCViewUIEditor::PickBox2D(GCViewUIEditor::BodySelect& ret, const Phantom::Pixel& cur)
{
	for(int i=0;i<m_dialogPtr->m_box2World.GetBodyCount();i++)
	{
		Phantom::B2Body* body	=	m_dialogPtr->m_box2World.GetBodyPtr(i);
		Phantom::Pixel center;
		int side = 0;
		if(GetMovePointRect(body, (Phantom::Pixel&)m_downPoint, center, side))
		{
			ret.downPoint	=	center;
			ret.jointPtr	=	0;
			ret.bodyPtr		=	body;
			ret.offset		=	center - cur;
			return true;
		}
	}
	for(int i=0;i<m_dialogPtr->m_box2World.GetJointCount();i++)
	{
		Phantom::B2Joint* body	=	m_dialogPtr->m_box2World.GetJointPtr(i);
		Phantom::Pixel center;
		int side = 0;
		if(GetMovePointRect(body, (Phantom::Pixel&)m_downPoint, center, side))
		{
			ret.bodyPtr		=	0;
			ret.downPoint	=	cur;
			ret.jointPtr	=	body;
			ret.offset		=	center - cur;
			return true;
		}
	}
	return false;
}
#endif
void		GCViewUIEditor::OnEditorUnitMode(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam)
{
#ifdef USE_BOX
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
			GetCursorPos(&m_downPoint);
			ScreenToClient(hWnd, &m_downPoint);
			m_lastPoint		=	m_downPoint;
			m_moveControl	=	0;
			m_downCenter.Set(0, 0);
			m_editorMode	=	MouseMode_None;
			//if(HIBYTE(VK_CONTROL))
			//{
			//	m_editorMode	=	MouseMode_InsertControl;
			//}
			//else
			if(!m_dialogPtr->m_box2World.GetWorldPtr())//不能再计算时移动
			{
				if(GetSelectKeyboard())
				{
					std::map<HTREEITEM,TreeItemType*> selectTypes;
					for(int i=0;i<this->m_treeView.GetSelectCount();i++)
					{
						HTREEITEM hItem		=	m_treeView.GetSelect(i);
						TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
						if(type && type->type == UI_PROP_B2_SHAPE)
						{
							Phantom::B2Body* body	=	m_dialogPtr->m_box2World.GetBodyPtr(Phantom::PtrID(0, type->p1));
							if(body)
							{
								Phantom::B2Shape* shape	=	body->GetShapePtr(Phantom::PtrID(0, type->p2));
								if(shape)
								{
									if(shape->m_data.type == b2Shape::e_polygon)
									{
										if(shape->m_vertices.size() < 8)
											shape->m_vertices.push_back(body->ToShapePos(PtoB((Phantom::Pixel&)m_downPoint)));
									}
									else if(shape->m_data.type == b2Shape::e_chain)
									{
										shape->m_vertices.push_back(body->ToShapePos(PtoB((Phantom::Pixel&)m_downPoint)));
									}
									SetModifyed();
									break;
								}
							}
						}
					}
				}
				else
				{
					//如果不进行任何操作
					m_editorMode	=	MouseMode_None;
					m_moveCopys.clear();
					for(int i=0;i<m_selectBodys.size();i++)
					{
						Phantom::Pixel center;
						if(m_selectBodys[i].bodyPtr)
						{
							if(GetMovePointRect(m_selectBodys[i].bodyPtr, (Phantom::Pixel&)m_downPoint, center, m_selectBodys[i].nSide))
							{
								m_selectBodys[i].downPoint	=	center;
								m_selectBodys[i].offset		=	center - (Phantom::Pixel&)m_downPoint;
								m_editorMode = MouseMode_MoveControl;
								break;
							}
						}
						else if(m_selectBodys[i].jointPtr)
						{
							if(GetMovePointRect(m_selectBodys[i].jointPtr, (Phantom::Pixel&)m_downPoint, center, m_selectBodys[i].nSide))
							{
								m_selectBodys[i].downPoint	=	center;
								m_editorMode = MouseMode_MoveControl;
								break;
							}
						}
						m_selectBodys[i].offset		=	m_selectBodys[i].downPoint - (Phantom::Pixel&)m_downPoint;
					}
					if(GetCopyKeyboard() && m_editorMode == MouseMode_MoveControl)
					{
						m_editorMode = MouseMode_MoveCopy;
						for(int i=0;i<m_selectBodys.size();i++)
						{
							if(m_selectBodys[i].bodyPtr)
							{
								Phantom::Pixel pos	=	BtoP(m_selectBodys[i].bodyPtr->GetPosition());
								m_moveCopys.push_back(CRect(pos.x - 10, pos.y - 10, pos.x + 10, pos.y + 10));
							}
							else
								m_moveCopys.push_back(CRect(0,0,1,1));
						}
					}
					if(m_editorMode == MouseMode_None)
					{
						m_editorMode	=	MouseMode_SelectRect;
					}
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
				for(int i=0;i<this->m_selectBodys.size();i++)
				{
					if(m_selectBodys[i].bodyPtr)
					{
						Phantom::Pixel pos	=	BtoP(m_selectBodys[i].bodyPtr->GetPosition());
						pos.x		+=	offsetX;
						pos.y		+=	offsetY;
						m_selectBodys[i].bodyPtr->SetPosition(PtoB(pos));
					}
					else
					{
						Phantom::Pixel pos	=	BtoP(m_selectBodys[i].jointPtr->m_data.localAnchorA);
						pos.x		+=	offsetX;
						pos.y		+=	offsetY;
						m_selectBodys[i].jointPtr->m_data.localAnchorA = PtoB(pos);
					}
					this->SetModifyed(true);
				}
			}
		}
		break;
	case WM_KEYUP:
		{
			if(wParam == VK_DELETE)
			{
				if(m_controlSelects.size() > 0)
				{
					if(MessageBox(this->GetViewHandle(), Language(L"是否删除所有选中项?"), Language(L"提示"), MB_YESNO) == IDNO)
						return;
					std::map<int,bool> groups;
					for(int i=0;i<m_selectBodys.size();i++)
					{
						Phantom::B2Body* body	=	m_selectBodys[i].bodyPtr;
						if(body)
						{
							delete body;
							this->SetModifyed(true);
						}
					}
					m_treeView.ClearSelect();
					m_selectBodys.clear();
					this->RefreshBoxBodys();
				}
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			CPoint pos;
			GetCursorPos(&pos);
			ScreenToClient(hWnd, &pos);
			if(m_bLButtonDown)
			{
				m_bLButtonDown	=	false;
				ReleaseCapture();
				switch(m_editorMode)
				{
				case MouseMode_MoveControl:
					{
						if(this->m_downPoint.x == pos.x && this->m_downPoint.y == pos.y)
						{
							BOOL bControl	=	GetSelectKeyboard();
							if(!bControl)
								m_selectBodys.clear();
							BodySelect sel;
							if(PickBox2D(sel, (Phantom::Pixel&)m_downPoint))
							{
								if(bControl)
								{
									for(int i=0;i<m_selectBodys.size();i++)
										if(m_selectBodys[i].bodyPtr == sel.bodyPtr)
										{
											m_selectBodys.erase(m_selectBodys.begin() + i);
											break;
										}
								}
								m_selectBodys.push_back(sel);
							}
							this->OnChangeBodySelect();
						}
						else
						{
							Phantom::Pixel offset		=	(Phantom::Pixel&)pos	-	(Phantom::Pixel&)this->m_downPoint;
							for(int i=0;i<this->m_selectBodys.size();i++)
							{
								if(m_selectBodys[i].bodyPtr)
								{
									m_selectBodys[i].downPoint	=	(Phantom::Pixel&)pos + m_selectBodys[i].offset;
								}
								else if(m_selectBodys[i].jointPtr)
								{
									m_selectBodys[i].downPoint	=	(Phantom::Pixel&)pos + m_selectBodys[i].offset;
								}
								this->SetModifyed(true);
							}
						}
					}
					break;
				case MouseMode_MoveCopy:
					{
						if(GetCopyKeyboard())
						{
							if(m_moveCopys.size() != this->m_selectBodys.size())
								break;
							//
							std::map<int, bool> groups;
							std::vector<BodySelect> selects;
							for(int i=0;i<m_selectBodys.size();i++)
							{
								if(!m_selectBodys[i].bodyPtr)
									continue;
								Phantom::B2Body* c	=	m_selectBodys[i].bodyPtr->Clone();
								Phantom::Rect rc	=	(Phantom::Rect&)m_moveCopys[i];
								BodySelect sel;
								sel.downPoint	=	Phantom::Pixel((rc.left + rc.right) / 2, (rc.bottom + rc.top) / 2);
								sel.bodyPtr	=	c;
								c->SetPosition(PtoB(sel.downPoint));
								selects.push_back(sel);
							}
							this->RefreshBoxBodys();
							m_selectBodys	=	selects;
							this->OnChangeBodySelect();
						}
					}
					break;
				case MouseMode_SelectRect:
					{
						if(m_lastPoint.x == m_downPoint.x && m_lastPoint.y == m_downPoint.y)
						{
							BodySelect sel;
							PickBox2D(sel, (Phantom::Pixel&)m_downPoint);
							if(GetSelectKeyboard())
							{
								bool bFind	=	false;
								for(int i=0;i<m_selectBodys.size();i++)
								{
									if(m_selectBodys[i].bodyPtr == sel.bodyPtr)
									{
										bFind	=	true;
										if(m_selectBodys.size() == 1)
											m_selectBodys.clear();
										else
											m_selectBodys.erase(m_selectBodys.begin() + i);
										break;
									}
								}
								if(!bFind && sel.bodyPtr)
									m_selectBodys.push_back(sel);
							}
							else
							{
								m_selectBodys.clear();
								if(sel.bodyPtr)
								{
									m_selectBodys.push_back(sel);
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
								m_selectBodys.clear();
							//
							Phantom::Rect rc(min(m_lastPoint.x, m_downPoint.x), min(m_lastPoint.y, m_downPoint.y), max(m_lastPoint.x, m_downPoint.x), max(m_lastPoint.y, m_downPoint.y));
							//
							for(int i=0;i<m_dialogPtr->m_box2World.GetBodyCount();i++)
							{
								Phantom::B2Body* body	=	m_dialogPtr->m_box2World.GetBodyPtr(i);
								BodySelect sel;
								sel.bodyPtr		=	body;
								sel.downPoint	=	BtoP(body->GetPosition());
								Phantom::Pixel pos	=	BtoP(body->GetPosition());
								if(rc.isPtIn(pos.x, pos.y))//BodyInRect(body, rc))
								{
									if(bShift)
									{
										bool bFind	=	false;
										for(int i=0;i<m_selectBodys.size();i++)
										{
											if(m_selectBodys[i].bodyPtr == body)
											{
												bFind	=	true;
											}
										}
										if(!bFind)
										{
											m_selectBodys.push_back( sel );
										}
									}
									else if(bAlt)
									{
										for(int i=0;i<m_selectBodys.size();i++)
										{
											if(m_selectBodys[i].bodyPtr == sel.bodyPtr)
											{
												if(m_selectBodys.size() == 1)
													m_selectBodys.clear();
												else
													m_selectBodys.erase(m_selectBodys.begin() + i);
												break;
											}
										}
									}
									else
										m_selectBodys.push_back( sel );
								}
							}
							for(int i=0;i<m_dialogPtr->m_box2World.GetJointCount();i++)
							{
								Phantom::B2Joint* joint	=	m_dialogPtr->m_box2World.GetJointPtr(i);
								BodySelect sel;
								sel.bodyPtr		=	0;
								sel.jointPtr	=	joint;
								sel.downPoint	=	BtoP(joint->m_data.localAnchorA);
								Phantom::Pixel pos	=	BtoP(joint->m_data.localAnchorA);
								if(rc.isPtIn(pos.x, pos.y))//BodyInRect(body, rc))
								{
									if(bShift)
									{
										bool bFind	=	false;
										for(int i=0;i<m_selectBodys.size();i++)
										{
											if(m_selectBodys[i].jointPtr == joint)
											{
												bFind	=	true;
											}
										}
										if(!bFind)
										{
											m_selectBodys.push_back( sel );
										}
									}
									else if(bAlt)
									{
										for(int i=0;i<m_selectBodys.size();i++)
										{
											if(m_selectBodys[i].jointPtr == sel.jointPtr)
											{
												if(m_selectBodys.size() == 1)
													m_selectBodys.clear();
												else
													m_selectBodys.erase(m_selectBodys.begin() + i);
												break;
											}
										}
									}
									else
										m_selectBodys.push_back( sel );
								}
							}
						}
						if(m_selectBodys.size() > 0)
						{
							this->OnChangeBodySelect();
						}
					}
					break;
				}
			}
			m_editorMode	=	MouseMode_None;
			m_moveControl	=	0;
		}
		break;
	case WM_MOUSEMOVE:
		{
			CPoint pos;
			GetCursorPos(&pos);
			ScreenToClient(hWnd, &pos);
			this->m_nAlignType	=	ToAlign_None;
			if(m_bDownMButton)
			{
				int x	=	(int)((float)(pos.x - m_downPoint.x) * m_dialogPtr->m_dlgScale.x);
				int y	=	(int)((float)(pos.y - m_downPoint.y) * m_dialogPtr->m_dlgScale.y);
				m_dialogPtr->SetLocation(m_downDlg.x + x, m_downDlg.y + y);
			}
			else if(m_bLButtonDown)
			{
				switch(m_editorMode)
				{
				case MouseMode_SelectRect:
					{
						m_lastPoint	=	pos;
					}
					break;
				case MouseMode_MoveControl:
					{
						Phantom::Pixel offset		=	(Phantom::Pixel&)pos	-	(Phantom::Pixel&)this->m_downPoint;
						for(int i=0;i<this->m_selectBodys.size();i++)
						{
							if(m_selectBodys[i].bodyPtr)
							{
								OnMoveBody(m_selectBodys[i], (Phantom::Pixel&)pos + m_selectBodys[i].offset);
								if(GetSelectKeyboard())
								{
									//Phantom::Pixel pos	=	m_controlSelects[i]->GetGroupPtr()->GetPosition();
									//Phantom::Rect rcW	=	m_controlSelects[i]->GetRenderRect();
									//m_controlSelects[i]->SetRotCenter(Phantom::Pixel(pos.x - rcW.left, pos.y - rcW.top));
								}
								//else
								//	m_selectBodys[i].bodyPtr->SetPosition(PtoB(offset + m_selectBodys[i].downPoint));
							}
							else if(m_selectBodys[i].jointPtr)
							{
								OnMoveBody(m_selectBodys[i], (Phantom::Pixel&)pos + m_selectBodys[i].offset);
							}
							this->SetModifyed(true);
						}
					}
					break;
				case MouseMode_MoveCopy:
					{
						Phantom::Pixel offset		=	(Phantom::Pixel&)pos	-	(Phantom::Pixel&)this->m_downPoint;
						if(m_moveCopys.size() != m_selectBodys.size())
							break;
						{
							for(int i=0;i<this->m_selectBodys.size();i++)
							{
								Phantom::Pixel off	=	(Phantom::Pixel&)pos + m_selectBodys[i].offset;
								m_moveCopys[i].SetRect(off.x - 10, off.y - 10, off.x + 10, off.y + 10);
							}
						}
					}
					break;
				}
			}
		}
		break;
	}
#endif
}
