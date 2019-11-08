//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   Box2D_Usage.cpp
	幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

	Design Writer :   赵德贤 Dexian Zhao
	Email: yuzhou_995@hotmail.com
	
	
	Copyright 2009-2016 Zhao Dexian
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "Box2D_Usage.h"
#include "PhantomUIDialog.h"
#include "PhantomManager.h"
//#ifndef EXPORT_ENGINE
//#define	BOX2D_SCREEN_WIDTH		1024.0f
//#define	BOX2D_SCREEN_HEIGHT		1024.0f
//
//namespace Phantom{
//
//	B2Shape::B2Shape(B2Body * body)
//	{
//		m_bodyPtr		=	body;
//		memset(&m_data, 0, sizeof(m_data));
//		m_data.id		=	m_bodyPtr->m_worldPtr->GetNewID();
//		m_shapePtr	=	0;
//		m_data.m_density	=	1.0f;
//	}
//
//	B2Shape*	B2Shape::Clone(B2Body* b)
//	{
//		B2Shape* s		=	b->AddShape();
//		int oldID		=	s->m_data.id;
//		memset(&s->m_data, 0, sizeof(m_data));
//		s->m_data.id	=	oldID;
//		s->m_vertices	=	m_vertices;
//		return s;
//	}
//
//	VOID		B2Shape::Save(CSafeFileHelperW& w)
//	{
//		int version = 1;
//		w << version;
//		int num = sizeof(B2ShapeData);
//		w << num;
//		w.pStream->write(&m_data, num);
//		int numVertices = m_vertices.size();
//		w << numVertices;
//		for(int i=0;i<numVertices;i++)
//			w << m_vertices[i];
//	}
//	VOID		B2Shape::Load(CSafeFileHelperR& r)
//	{
//		int version = 1;
//		r >> version;
//		int num;
//		r >> num;
//		r.pStream->read(&m_data, num);
//		int numVertices;
//		r >> numVertices;
//		if(numVertices > 0)
//			m_vertices.resize(numVertices);
//		for(int i=0;i<numVertices;i++)
//			r >> m_vertices[i];
//	}
//
//
//	VOID		B2Shape::OnFrameMove()	//刷新最新位置和转动率
//	{
//		if(!m_bodyPtr->m_worldPtr->m_dlgPtr || !this->m_shapePtr)
//			return;
//		UIControl* c	=	this->m_bodyPtr->m_worldPtr->m_dlgPtr->FindControl(this->m_data.m_bindControl);
//		if(!c)
//			return;
//		const b2Vec2& v = m_bodyPtr->m_bodyPtr->GetPosition();
//		switch(m_data.type)
//		{
//		case b2Shape::e_circle:
//			{
//				float2 center(v.x + m_data.m_p.x, v.y + m_data.m_p.y);
//				c->SetCenterPos(m_bodyPtr->m_worldPtr->ConvertBox2dToPixel(center));
//				c->SetRotation(true, -Degress(this->m_bodyPtr->m_bodyPtr->GetAngle()));
//			}
//			break;
//		case b2Shape::e_edge:
//			{
//				float2 center(v.x + m_data.m_vertex1.x, v.y + m_data.m_vertex1.y);
//				c->SetCenterPos(m_bodyPtr->m_worldPtr->ConvertBox2dToPixel(center));
//				c->SetRotation(true, -Degress(this->m_bodyPtr->m_bodyPtr->GetAngle()));
//			}
//			break;
//		case b2Shape::e_polygon:
//			{
//				if(m_vertices.size() > 0)
//				{
//					float2 center(v.x + m_vertices[0].x, v.y + m_vertices[0].y);
//					c->SetCenterPos(m_bodyPtr->m_worldPtr->ConvertBox2dToPixel(center));
//					c->SetRotation(true, -Degress(this->m_bodyPtr->m_bodyPtr->GetAngle()));
//				}
//			}
//			break;
//		case b2Shape::e_chain:
//			{
//				if(m_vertices.size() > 0)
//				{
//					float2 center(v.x + m_vertices[0].x, v.y + m_vertices[0].y);
//					c->SetCenterPos(m_bodyPtr->m_worldPtr->ConvertBox2dToPixel(center));
//					c->SetRotation(true, -Degress(this->m_bodyPtr->m_bodyPtr->GetAngle()));
//				}
//			}
//			break;
//		case B2ShapeData::e_polygon_box:
//			{
//				//if(m_vertices.size() > 0)
//				{
//					float2 center(v.x + m_data.m_p.x, v.y + m_data.m_p.y);
//					c->SetCenterPos(m_bodyPtr->m_worldPtr->ConvertBox2dToPixel(center));
//					c->SetRotation(true, -Degress(this->m_bodyPtr->m_bodyPtr->GetAngle()));
//				}
//			}
//			break;
//		}
//	}
//
//	VOID		B2Shape::ChangeToEngine()	//刷新到物理引擎
//	{
//		if(!m_bodyPtr->m_bodyPtr)
//			return;
//		if(m_shapePtr)
//			m_bodyPtr->m_bodyPtr->DestroyFixture(this->m_shapePtr);
//		m_shapePtr	=	0;
//		switch(m_data.type)
//		{
//		case b2Shape::e_circle:
//			{
//				b2CircleShape s;
//				s.m_p		=	v2(m_data.m_p);
//				s.m_radius	=	m_data.m_radius;
//				m_shapePtr	=	m_bodyPtr->m_bodyPtr->CreateFixture(&s, m_data.m_density);
//			}
//			break;
//		case b2Shape::e_edge:
//			{
//				b2EdgeShape shape;
//				shape.Set(v2(m_data.m_vertex1), v2(m_data.m_vertex2));
//				if(m_data.m_hasVertex0)
//				{
//					shape.m_hasVertex0	=	true;
//					shape.m_vertex0.Set(m_data.m_vertex0.x, m_data.m_vertex0.y);
//				}
//				if(m_data.m_hasVertex3)
//				{
//					shape.m_hasVertex3	=	true;
//					shape.m_vertex3.Set(m_data.m_vertex3.x, m_data.m_vertex3.y);
//				}
//				m_shapePtr	=	m_bodyPtr->m_bodyPtr->CreateFixture(&shape, m_data.m_density);
//			}
//			break;
//		case b2Shape::e_polygon:
//			{
//				int maxV	=	m_vertices.size() > 8 ? 8 : m_vertices.size();
//				if(maxV > 0)
//				{
//					b2PolygonShape shape;
//					shape.Set((b2Vec2*)&m_vertices.at(0), maxV);
//					m_shapePtr	=	m_bodyPtr->m_bodyPtr->CreateFixture(&shape, m_data.m_density);
//				}
//			}
//			break;
//		case b2Shape::e_chain:
//			{
//				if(m_vertices.size() > 0)
//				{
//					b2ChainShape shape;
//					if(m_data.m_bChainLoop)
//						shape.CreateLoop((b2Vec2*)&m_vertices.at(0), m_vertices.size());
//					else
//						shape.CreateChain((b2Vec2*)&m_vertices.at(0), m_vertices.size());
//					m_shapePtr	=	m_bodyPtr->m_bodyPtr->CreateFixture(&shape, m_data.m_density);
//				}
//			}
//			break;
//		case B2ShapeData::e_polygon_box:
//			{
//				b2PolygonShape shape;
//				shape.SetAsBox(m_data.m_boxWidth * 0.5f, m_data.m_boxHeight * 0.5f, v2(m_data.m_p), m_data.m_fAngle);
//				m_shapePtr	=	m_bodyPtr->m_bodyPtr->CreateFixture(&shape, m_data.m_density);
//			}
//			break;
//		}
//	}
//	//
//	VOID		B2Shape::CreateBox(float fWidth, float fHeight, const float2& center, float fAngle)
//	{
//		m_data.type			=	B2ShapeData::e_polygon_box;
//		m_data.m_fAngle		=	fAngle;
//		m_data.m_boxWidth	=	fWidth;
//		m_data.m_boxHeight	=	fHeight;
//		m_data.m_p			=	center;
//	}
//	VOID		B2Shape::CreateCircle(const float2& pos, float fRadius)
//	{
//		m_data.type			=	b2Shape::e_circle;
//		m_data.m_p			=	pos;
//		m_data.m_radius		=	fRadius;
//	}
//	VOID		B2Shape::CreateEdge(const float2& from, const float2& to, const float2* v0, const float2* v3)
//	{
//		m_data.type			=	b2Shape::e_edge;
//		m_data.m_vertex1	=	from;
//		m_data.m_vertex2	=	to;
//		if(v0)
//		{
//			m_data.m_hasVertex0	=	true;
//			m_data.m_vertex0	=	*v0;
//		}
//		else
//			m_data.m_hasVertex0	=	false;
//		if(v3)
//		{
//			m_data.m_hasVertex3	=	true;
//			m_data.m_vertex3	=	*v3;
//		}
//		else
//			m_data.m_hasVertex3	=	false;
//	}
//	VOID		B2Shape::CreatePolygon(const float2* v, int nCount)
//	{
//		m_data.type			=	b2Shape::e_polygon;
//		if(nCount > 0)
//		{
//			m_vertices.resize(nCount);
//			memcpy(&m_vertices.at(0), v, sizeof(float2) * nCount);
//		}
//		else
//			this->m_vertices.clear();
//	}
//	VOID		B2Shape::CreateChain(const float2* v, int nCount)
//	{
//		m_data.type			=	b2Shape::e_chain;
//		m_data.m_bChainLoop	=	false;
//		if(nCount > 0)
//		{
//			m_vertices.resize(nCount);
//			memcpy(&m_vertices.at(0), v, sizeof(float2) * nCount);
//		}
//		else
//			this->m_vertices.clear();
//	}
//	VOID		B2Shape::CreateChainLoop(const float2* v, int nCount)
//	{
//		CreateChain(v, nCount);
//		m_data.m_bChainLoop	=	true;
//	}
//	b2Vec2		B2Shape::ToBodyPos(const float2& v)
//	{
//		return b2Vec2(m_bodyPtr->m_bodyData.position.x + v.x, m_bodyPtr->m_bodyData.position.y + v.y);
//	}
//
//	VOID		B2Shape::GetRect(rect_f& ret)
//	{
//		ret	=	rect_f(1000, 1000, -1000, -1000);
//		B2DebugDraw* draw	=	&m_bodyPtr->m_worldPtr->m_debugDraw;
//		b2Color color(1,1,1);
//		switch (m_data.type)
//		{
//		case b2Shape::e_circle:
//			{
//				float x1	=	m_data.m_p.x - m_data.m_radius;
//				float x2	=	m_data.m_p.x + m_data.m_radius;
//				float y1	=	m_data.m_p.y - m_data.m_radius;
//				float y2	=	m_data.m_p.y + m_data.m_radius;
//				ret.left	=	getmin(x1, ret.left);
//				ret.top		=	getmin(y1, ret.top);
//				ret.right	=	getmax(x2, ret.right);
//				ret.bottom	=	getmax(y2, ret.bottom);
//			}
//			break;
//		case b2Shape::e_edge:
//			{
//				ret.left		=	getmin(m_data.m_vertex1.x, ret.left);
//				ret.left		=	getmin(m_data.m_vertex2.x, ret.left);
//				if(m_data.m_hasVertex0)
//					ret.left	=	getmin(m_data.m_vertex0.x, ret.left);
//				if(m_data.m_hasVertex3)
//					ret.left	=	getmin(m_data.m_vertex3.x, ret.left);
//				//
//				ret.right		=	getmax(m_data.m_vertex1.x, ret.right);
//				ret.right		=	getmax(m_data.m_vertex2.x, ret.right);
//				if(m_data.m_hasVertex0)
//					ret.right	=	getmax(m_data.m_vertex0.x, ret.right);
//				if(m_data.m_hasVertex3)
//					ret.right	=	getmax(m_data.m_vertex3.x, ret.right);
//				//
//				ret.top			=	getmin(m_data.m_vertex1.y, ret.top);
//				ret.top			=	getmin(m_data.m_vertex2.y, ret.top);
//				if(m_data.m_hasVertex0)
//					ret.top		=	getmin(m_data.m_vertex0.y, ret.top);
//				if(m_data.m_hasVertex3)
//					ret.top		=	getmin(m_data.m_vertex3.y, ret.top);
//				//
//				ret.bottom		=	getmax(m_data.m_vertex1.y, ret.bottom);
//				ret.bottom		=	getmax(m_data.m_vertex2.y, ret.bottom);
//				if(m_data.m_hasVertex0)
//					ret.bottom	=	getmax(m_data.m_vertex0.y, ret.bottom);
//				if(m_data.m_hasVertex3)
//					ret.bottom	=	getmax(m_data.m_vertex3.y, ret.bottom);
//			}
//			break;
//
//		case b2Shape::e_chain:
//			{
//				int32 count = this->m_vertices.size();
//				if(count > 0)
//				{
//					const float2* vertices = &m_vertices.at(0);
//					for (int32 i = 0; i < count; ++i)
//					{
//						ret.left	=	getmin(vertices[i].x, ret.left);
//						ret.top		=	getmin(vertices[i].y, ret.top);
//						ret.right	=	getmax(vertices[i].x, ret.right);
//						ret.bottom	=	getmax(vertices[i].y, ret.bottom);
//					}
//				}
//			}
//			break;
//
//		case b2Shape::e_polygon:
//			{
//				int32 count = this->m_vertices.size();
//				if(count > 0)
//				{
//					int32 vertexCount	= count;
//					for (int32 i = 0; i < vertexCount; ++i)
//					{
//						ret.left	=	getmin(m_vertices[i].x, ret.left);
//						ret.top		=	getmin(m_vertices[i].y, ret.top);
//						ret.right	=	getmax(m_vertices[i].x, ret.right);
//						ret.bottom	=	getmax(m_vertices[i].y, ret.bottom);
//					}
//				}
//			}
//			break;
//		case B2ShapeData::e_polygon_box:
//			{
//				rect_f temp(m_data.m_p.x - m_data.m_boxWidth * 0.5f, m_data.m_p.y - m_data.m_boxHeight * 0.5f, m_data.m_p.x + m_data.m_boxWidth * 0.5f, m_data.m_p.y + m_data.m_boxHeight * 0.5f);
//				ret.left	=	getmin(temp.left, ret.left);
//				ret.top		=	getmin(temp.top, ret.top);
//				ret.right	=	getmax(temp.right, ret.right);
//				ret.bottom	=	getmax(temp.bottom, ret.bottom);
//			}
//			break;
//		default:
//			break;
//		}
//	}
//
//b2Color	GetBodyTypeColor(int bt)
//{
//	switch(bt)
//	{
//	case b2_staticBody:
//		return b2Color(0.2f,0.2f,0.2f);
//	case b2_kinematicBody:
//		return b2Color(0.2f,0.2f,1.f);
//	case b2_dynamicBody:
//		return b2Color(1.0f,0.2f,0.2f);
//	}
//	return b2Color(0.2f,0.2f,0.2f);
//}
//
//	VOID		B2Shape::Render()
//	{
//		B2DebugDraw* draw	=	&m_bodyPtr->m_worldPtr->m_debugDraw;
//		b2Color color = GetBodyTypeColor(m_bodyPtr->m_bodyData.bodyType);//(1,0,0);
//		switch (m_data.type)
//		{
//		case b2Shape::e_circle:
//			{
//				b2Vec2 center = ToBodyPos(m_data.m_p);
//				float32 radius = m_data.m_radius;
//				b2Vec2 axis = b2Vec2(1.0f, 0.0f);
//				draw->DrawSolidCircle(center, radius, axis, color);
//			}
//			break;
//
//		case b2Shape::e_edge:
//			{
//				b2Vec2 v1 = ToBodyPos(m_data.m_vertex1);
//				b2Vec2 v2 = ToBodyPos(m_data.m_vertex2);
//				draw->DrawSegment(v1, v2, color);
//			}
//			break;
//
//		case b2Shape::e_chain:
//			{
//				int32 count = this->m_vertices.size();
//				if(count > 0)
//				{
//					const float2* vertices = &m_vertices.at(0);
//					b2Vec2 v1 = ToBodyPos(vertices[0]);
//					for (int32 i = 1; i < count; ++i)
//					{
//						b2Vec2 v2 = ToBodyPos(vertices[i]);
//						draw->DrawSegment(v1, v2, color);
//						draw->DrawCircle(v1, 0.05f, color);
//						v1 = v2;
//					}
//					//
//					if(m_data.m_bChainLoop)
//					{
//						draw->DrawSegment(ToBodyPos(m_vertices[0]), ToBodyPos(m_vertices[count - 1]), color);
//					}
//				}
//			}
//			break;
//
//		case b2Shape::e_polygon:
//			{
//				int32 count = this->m_vertices.size();
//				if(count > 0)
//				{
//					int32 vertexCount	= count;
//					b2Assert(vertexCount <= b2_maxPolygonVertices);
//					b2Vec2 vertices[b2_maxPolygonVertices];
//					for (int32 i = 0; i < vertexCount; ++i)
//					{
//						vertices[i] = ToBodyPos(m_vertices[i]);
//					}
//					draw->DrawSolidPolygon(vertices, vertexCount, color);
//				}
//			}
//			break;
//		case B2ShapeData::e_polygon_box:
//			{
//				b2Vec2 vertices[4];
//				float fW	=	m_data.m_boxWidth * 0.5f;
//				float fH	=	m_data.m_boxHeight * 0.5f;
//				b2Vec2 v1 = ToBodyPos(float2(m_data.m_p.x - fW, m_data.m_p.y - fH));
//				b2Vec2 v2 = ToBodyPos(float2(m_data.m_p.x + fW, m_data.m_p.y - fH));
//				b2Vec2 v3 = ToBodyPos(float2(m_data.m_p.x + fW, m_data.m_p.y + fH));
//				b2Vec2 v4 = ToBodyPos(float2(m_data.m_p.x - fW, m_data.m_p.y + fH));
//				draw->DrawSegment(v1, v2, color);
//				draw->DrawSegment(v2, v3, color);
//				draw->DrawSegment(v3, v4, color);
//				draw->DrawSegment(v4, v1, color);
//			}
//			break;
//		default:
//			break;
//		}
//	}
//
//	B2Shape::~B2Shape()
//	{
//		if(m_shapePtr && m_bodyPtr && m_bodyPtr->m_bodyPtr)
//			m_bodyPtr->m_bodyPtr->DestroyFixture(m_shapePtr);
//		m_shapePtr	=	0;
//	}
//
//	float2	B2Shape::GetWorldPos()
//	{
//		return m_data.m_p + m_bodyPtr->m_bodyData.position;
//	}
//
//	B2Joint::B2Joint(B2World* body)
//	{
//		m_worldPtr	=	body;
//		memset(&m_data, 0, sizeof(m_data));
//		m_data.id		=	m_worldPtr->GetNewID();
//		m_jointPtr	=	0;
//	}
//	B2Joint::~B2Joint()
//	{
//		if(m_jointPtr && m_worldPtr && m_worldPtr->m_world)
//			m_worldPtr->m_world->DestroyJoint(m_jointPtr);
//		if(m_worldPtr)
//			m_worldPtr->OnRemoveJoint(this);
//		m_jointPtr	=	0;
//	}
//
//	VOID	B2Joint::Save(CSafeFileHelperW& w)
//	{
//		int version = 1;
//		w << version;
//		int num = sizeof(B2JointData);
//		w << num;
//		w.pStream->write(&m_data, num);
//	}
//	VOID	B2Joint::Load(CSafeFileHelperR& r)
//	{
//		int version = 1;
//		r >> version;
//		int num = sizeof(B2JointData);
//		r >> num;
//		r.pStream->read(&m_data, num);
//	}
//
//	VOID	B2Joint::SetType(int t)
//	{
//		m_data.jointType	=	t;
//		switch(m_data.jointType)
//		{
//		case e_revoluteJoint:
//			m_data.localAnchorA.setxy(0.0f, 0.0f);
//			m_data.localAnchorB.setxy(0.0f, 0.0f);
//			m_data.referenceAngle = 0.0f;
//			m_data.lowerAngle = 0.0f;
//			m_data.upperAngle = 0.0f;
//			m_data.maxMotorTorque = 0.0f;
//			m_data.motorSpeed = 0.0f;
//			m_data.enableLimit = false;
//			m_data.enableMotor = false;
//			break;
//		case e_prismaticJoint:
//			m_data.localAnchorA.setxy(0, 0);
//			m_data.localAnchorB.setxy(0, 0);
//			m_data.localAxisA.setxy(1.0f, 0.0f);
//			m_data.referenceAngle = 0.0f;
//			m_data.enableLimit = false;
//			m_data.lowerTranslation = 0.0f;
//			m_data.upperTranslation = 0.0f;
//			m_data.enableMotor = false;
//			m_data.maxMotorForce = 0.0f;
//			m_data.motorSpeed = 0.0f;
//			break;
//		case e_distanceJoint:
//			m_data.localAnchorA.setxy(0.0f, 0.0f);
//			m_data.localAnchorB.setxy(0.0f, 0.0f);
//			m_data.maxLength = 1.0f;
//			m_data.frequencyHz = 0.0f;
//			m_data.ratio = 0.0f;
//			break;
//		case e_pulleyJoint:
//			m_data.groundAnchorA.setxy(-1.0f, 1.0f);
//			m_data.groundAnchorB.setxy(1.0f, 1.0f);
//			m_data.localAnchorA.setxy(-1.0f, 0.0f);
//			m_data.localAnchorB.setxy(1.0f, 0.0f);
//			m_data.lengthA = 0.0f;
//			m_data.lengthB = 0.0f;
//			m_data.ratio = 1.0f;
//			m_data.collideConnected = true;
//			break;
//		case e_mouseJoint:
//			m_data.target.setxy(0.0f, 0.0f);
//			m_data.maxForce = 0.0f;
//			m_data.frequencyHz = 5.0f;
//			m_data.ratio = 0.7f;
//			break;
//		case e_gearJoint:
//			m_data.joint1 = 0;
//			m_data.joint2 = 0;
//			m_data.ratio = 1.0f;
//			break;
//		case e_wheelJoint:
//			m_data.localAnchorA.setxy(0.0f, 0.0f);
//			m_data.localAnchorB.setxy(0.0f, 0.0f);
//			m_data.localAxisA.setxy(1.0f, 0.0f);
//			m_data.enableMotor = false;
//			m_data.maxMotorTorque = 0.0f;
//			m_data.motorSpeed = 0.0f;
//			m_data.frequencyHz = 2.0f;
//			m_data.ratio = 0.7f;
//			break;
//		case e_weldJoint:
//			m_data.localAnchorA.setxy(0.0f, 0.0f);
//			m_data.localAnchorB.setxy(0.0f, 0.0f);
//			m_data.referenceAngle = 0.0f;
//			m_data.frequencyHz = 0.0f;
//			m_data.ratio = 0.0f;
//			break;
//		case e_frictionJoint:
//			m_data.localAnchorA.setxy(0.0f, 0.0f);
//			m_data.localAnchorB.setxy(0.0f, 0.0f);
//			m_data.maxForce = 0.0f;
//			m_data.maxTorque = 0.0f;
//			break;
//		case e_ropeJoint:
//			m_data.localAnchorA.setxy(-1.0f, 0.0f);
//			m_data.localAnchorB.setxy(1.0f, 0.0f);
//			m_data.maxLength = 0.0f;
//			break;
//		}
//	}
//
//	VOID	B2Joint::ChangeToEngine()	//刷新到物理引擎
//	{
//		if(m_jointPtr && m_worldPtr && m_worldPtr->m_world)
//			m_worldPtr->m_world->DestroyJoint(m_jointPtr);
//		m_jointPtr	=	0;
//		//必须两个都存在
//		PtrID id1(0, m_data.bodyA);
//		b2Body* bodyA				=	m_worldPtr->GetBox2dBodyPtr(id1);
//		PtrID id2(0, m_data.bodyB);
//		b2Body*	bodyB				=	m_worldPtr->GetBox2dBodyPtr(id2);
//		if(!bodyA || !bodyB)
//			return;
//		switch(m_data.jointType)
//		{
//		case e_revoluteJoint:
//			{
//				b2RevoluteJointDef def;
//				def.collideConnected	=	(m_data.collideConnected > 0);
//				def.Initialize(bodyA, bodyB, v2(m_data.localAnchorA));
//				def.referenceAngle	=	m_data.referenceAngle;
//				def.lowerAngle		=	m_data.lowerAngle;
//				def.upperAngle		=	m_data.upperAngle;
//				def.maxMotorTorque	=	m_data.maxMotorTorque;
//				def.motorSpeed		=	m_data.motorSpeed;
//				def.enableLimit		=	(m_data.enableLimit > 0);
//				def.enableMotor		=	(m_data.enableMotor > 0);
//				m_jointPtr			=	m_worldPtr->m_world->CreateJoint(&def);
//			}
//			break;
//		case e_prismaticJoint:
//			{
//				b2PrismaticJointDef def;
//				def.collideConnected	=	(m_data.collideConnected > 0);
//				def.Initialize(bodyA, bodyB, v2(m_data.localAnchorA), v2(m_data.localAxisA));
//				def.referenceAngle	=	m_data.referenceAngle;
//				//
//				def.enableLimit	=	(m_data.enableLimit > 0);
//				def.lowerTranslation	=	m_data.lowerTranslation;
//				def.upperTranslation	=	m_data.upperTranslation;
//				def.enableMotor		=	(m_data.enableMotor > 0);
//				def.maxMotorForce	=	m_data.maxMotorForce;
//				def.motorSpeed	=	m_data.motorSpeed;
//				m_jointPtr			=	m_worldPtr->m_world->CreateJoint(&def);
//			}
//			break;
//		case e_distanceJoint:
//			{
//				b2DistanceJointDef def;
//				def.collideConnected	=	(m_data.collideConnected > 0);
//				def.bodyA				=	bodyA;
//				def.bodyB				=	bodyB;
//				def.localAnchorA = b2Vec2(m_data.localAnchorA.x - bodyA->GetPosition().x, m_data.localAnchorA.y - bodyA->GetPosition().y);
//				def.localAnchorB = b2Vec2(m_data.localAnchorB.x - bodyB->GetPosition().x, m_data.localAnchorB.y - bodyB->GetPosition().y);
//				def.length			=	m_data.maxLength;
//				def.frequencyHz		=	m_data.frequencyHz;
//				def.dampingRatio	=	m_data.ratio;
//				m_jointPtr			=	m_worldPtr->m_world->CreateJoint(&def);
//			}
//			break;
//		case e_pulleyJoint:
//			{
//				b2PulleyJointDef def;
//				def.collideConnected	=	(m_data.collideConnected > 0);
//				def.Initialize(bodyA, bodyB, v2(m_data.groundAnchorA), v2(m_data.groundAnchorB), v2(m_data.localAnchorA), v2(m_data.localAnchorB), m_data.ratio);
//				m_jointPtr			=	m_worldPtr->m_world->CreateJoint(&def);
//			}
//			break;
//		case e_mouseJoint:
//			{
//				b2MouseJointDef def;
//				def.collideConnected	=	(m_data.collideConnected > 0);
//				def.bodyA				=	bodyA;
//				def.bodyB				=	bodyB;
//				def.target		=	v2(m_data.target);
//				def.maxForce	=	m_data.maxForce;
//				def.frequencyHz	=	m_data.frequencyHz;
//				def.dampingRatio	=	m_data.ratio;
//				m_jointPtr			=	m_worldPtr->m_world->CreateJoint(&def);
//			}
//			break;
//		case e_gearJoint:
//			{
//				b2GearJointDef def;
//				def.collideConnected	=	(m_data.collideConnected > 0);
//				def.bodyA				=	bodyA;
//				def.bodyB				=	bodyB;
//				PtrID id1(0, m_data.joint1);
//				PtrID id2(0, m_data.joint2);
//				def.joint1				=	m_worldPtr->GetBox2dJointPtr(id1);
//				def.joint2				=	m_worldPtr->GetBox2dJointPtr(id2);
//				def.ratio				=	m_data.ratio;
//				m_jointPtr			=	m_worldPtr->m_world->CreateJoint(&def);
//			}
//			break;
//		case e_wheelJoint:
//			{
//				b2WheelJointDef def;
//				def.collideConnected	=	(m_data.collideConnected > 0);
//				def.Initialize(bodyA, bodyB, v2(m_data.localAnchorA), v2(m_data.localAxisA));
//				def.enableMotor	=	(m_data.enableMotor > 0);
//				def.maxMotorTorque	=	m_data.maxMotorTorque;
//				def.motorSpeed	=	m_data.motorSpeed;
//				def.frequencyHz	=	m_data.frequencyHz;
//				def.dampingRatio	=	m_data.ratio;
//				m_jointPtr			=	m_worldPtr->m_world->CreateJoint(&def);
//			}
//			break;
//		case e_weldJoint:
//			{
//				b2WeldJointDef def;
//				def.collideConnected	=	(m_data.collideConnected > 0);
//				def.Initialize(bodyA, bodyB, v2(m_data.localAnchorA));
//				def.referenceAngle	=	m_data.referenceAngle;
//				def.frequencyHz		=	m_data.frequencyHz;
//				def.dampingRatio	=	m_data.ratio;
//				m_jointPtr			=	m_worldPtr->m_world->CreateJoint(&def);
//			}
//			break;
//		case e_frictionJoint:
//			{
//				b2FrictionJointDef def;
//				def.collideConnected	=	(m_data.collideConnected > 0);
//				def.Initialize(bodyA, bodyB, v2(m_data.localAnchorA));
//				def.maxForce		=	m_data.maxForce;
//				def.maxTorque		=	m_data.maxTorque;
//				m_jointPtr			=	m_worldPtr->m_world->CreateJoint(&def);
//			}
//			break;
//		case e_ropeJoint:
//			{
//				b2RopeJointDef def;
//				def.collideConnected	=	(m_data.collideConnected > 0);
//				def.bodyA				=	bodyA;
//				def.bodyB				=	bodyB;
//				def.localAnchorA	=	v2(m_data.localAnchorA);
//				def.localAnchorB	=	v2(m_data.localAnchorB);
//				def.maxLength		=	m_data.maxLength;
//				m_jointPtr			=	m_worldPtr->m_world->CreateJoint(&def);
//			}
//			break;
//		}
//	}
//
//b2Color GetJointTypeColor(int type)
//{
//	switch(type)
//	{
//	case e_distanceJoint:
//		return b2Color(1,1,0);
//	case e_revoluteJoint:
//		return b2Color(0,0.8f,0);
//	case e_prismaticJoint:
//		return b2Color(0,0.8f,0.8f);
//	case e_pulleyJoint:
//		return b2Color(0.3,0.3f,0.3f);
//	case e_mouseJoint:
//		return b2Color(0.9,0.9f,0.9f);
//	case e_gearJoint:
//		return b2Color(0.8,0,0);
//	case e_wheelJoint:
//		return b2Color(0.8,0,0.8f);
//	case e_weldJoint:
//		return b2Color(0.1,0.1f,0.8f);
//	case e_frictionJoint:
//		return b2Color(0.1,0.8f,0.2f);
//	case e_ropeJoint:
//		return b2Color(0.8,0.5f,0.1f);
//	}
//	return b2Color(0,0,0);
//}
//	VOID	B2Joint::Render()	//刷新到物理引擎
//	{
//		//必须两个都存在
//		PtrID id1(0, m_data.bodyA);
//		PtrID id2(0, m_data.bodyB);
//		B2Body* bodyA				=	m_worldPtr->GetBodyPtr(id1);
//		B2Body*	bodyB				=	m_worldPtr->GetBodyPtr(id2);
//		if(!bodyA || !bodyB)
//			return;
//		b2Color color = GetJointTypeColor(this->m_data.jointType);//(1,0,0);
//		b2AABB ab;
//		ab.lowerBound.Set(m_data.localAnchorA.x - 0.25f, m_data.localAnchorA.y - 0.25f);
//		ab.upperBound.Set(m_data.localAnchorA.x + 0.25f, m_data.localAnchorA.y + 0.25f);
//		m_worldPtr->m_debugDraw.DrawAABB(&ab, color);
//		switch(m_data.jointType)
//		{
//		case e_distanceJoint:
//			{
//				m_worldPtr->m_debugDraw.DrawSegment(v2(bodyA->GetPosition()), v2(m_data.localAnchorA), color);
//				m_worldPtr->m_debugDraw.DrawSegment(v2(bodyB->GetPosition()), v2(m_data.localAnchorB), color);
//				b2AABB abB;
//				abB.lowerBound.Set(m_data.localAnchorB.x - 0.25f, m_data.localAnchorB.y - 0.25f);
//				abB.upperBound.Set(m_data.localAnchorB.x + 0.25f, m_data.localAnchorB.y + 0.25f);
//				m_worldPtr->m_debugDraw.DrawAABB(&abB, color);
//			}
//			break;
//		case e_revoluteJoint:
//		case e_prismaticJoint:
//		case e_pulleyJoint:
//		case e_mouseJoint:
//		case e_gearJoint:
//		case e_wheelJoint:
//		case e_weldJoint:
//		case e_frictionJoint:
//		case e_ropeJoint:
//			m_worldPtr->m_debugDraw.DrawSegment(v2(bodyA->GetPosition()), v2(m_data.localAnchorA), color);
//			m_worldPtr->m_debugDraw.DrawSegment(v2(bodyB->GetPosition()), v2(m_data.localAnchorA), color);
//			break;
//		}
//	}
//
//	B2Body::B2Body(B2World* w)
//	{
//		memset(&m_bodyData, 0, sizeof(m_bodyData));
//		m_bodyData.position.setxy(0.0f, 0.0f);
//		m_bodyData.angle = 0.0f;
//		m_bodyData.linearVelocity.setxy(0.0f, 0.0f);
//		m_bodyData.angularVelocity = 0.0f;
//		m_bodyData.linearDamping = 0.0f;
//		m_bodyData.angularDamping = 0.0f;
//		m_bodyData.allowSleep	=	true;
//		m_bodyData.awake		=	true;
//		m_bodyData.active		=	true;
//		m_bodyData.bodyType		=	b2_staticBody;
//		m_bodyData.gravityScale = 1.0f;
//		//
//		m_worldPtr		=	w;
//		m_bodyData.id	=	m_worldPtr->GetNewID();
//		m_bodyPtr		=	0;
//	}
//
//	VOID	B2Body::Save(CSafeFileHelperW& w)
//	{
//		int version = 1;
//		w << version;
//		int num = sizeof(B2BodyData);
//		w << num;
//		w.pStream->write(&m_bodyData, num);
//		int shape = this->m_shapes.size();
//		w << shape;
//		for(int i=0;i<m_shapes.size();i++)
//			m_shapes[i]->Save(w);
//	}
//	VOID	B2Body::Load(CSafeFileHelperR& r)
//	{
//		int version = 1;
//		r >> version;
//		int num = sizeof(B2BodyData);
//		r >> num;
//		r.pStream->read(&m_bodyData, num);
//		int shape;
//		r >> shape;
//		for(int i=0;i<shape;i++)
//		{
//			B2Shape* s = new B2Shape(this);
//			s->Load(r);
//			m_shapes.push_back(s);
//		}
//	}
//
//	rect_f			B2Body::GetBodyRect()
//	{
//		if(this->m_shapes.size() == 0)
//			return rect_f(0, 0, 1, 1);
//		rect_f rc;
//		rc.left		=	1000;
//		rc.top		=	1000;
//		rc.right	=	-1000;
//		rc.bottom	=	-1000;
//		for(int i=0;i<m_shapes.size();i++)
//		{
//			rect_f rr;
//			m_shapes[i]->GetRect(rr);
//			rr.left		+=	this->m_bodyData.position.x;
//			rr.right	+=	this->m_bodyData.position.x;
//			rr.top		+=	this->m_bodyData.position.y;
//			rr.bottom	+=	this->m_bodyData.position.y;
//			rc.left		=	getmin(rc.left, rr.left);
//			rc.top		=	getmin(rc.top, rr.top);
//			rc.right	=	getmax(rc.right, rr.right);
//			rc.bottom	=	getmax(rc.bottom, rr.bottom);
//		}
//		if(rc.right < rc.left)
//			rc.right = rc.left + 1.0f;
//		if(rc.bottom < rc.top)
//			rc.bottom = rc.top + 1.0f;
//		return rc;
//	}
//
//	B2Body::~B2Body()
//	{
//		if(m_bodyPtr && m_worldPtr->m_world)
//		{
//			m_worldPtr->m_world->DestroyBody(m_bodyPtr);
//			m_bodyPtr	=	0;
//		}
//		if(m_worldPtr)
//			m_worldPtr->OnRemoveBody(this);
//		for(int i=0;i<m_shapes.size();i++)
//		{
//			m_shapes[i]->m_bodyPtr	=	0;
//			delete m_shapes[i];
//		}
//		m_shapes.clear();
//	}
//
//	B2Shape*			B2Body::GetShapePtr(PtrID& id)
//	{
//		if(id.id == 0)
//			return 0;
//		if(id.index < m_shapes.size() && m_shapes[id.index]->m_data.id == id.id)
//			return m_shapes[id.index];
//		for(int i=0;i<m_shapes.size();i++)
//		{
//			if(m_shapes[i]->m_data.id == id.id)
//			{
//				id.index	=	i;
//				id.id = m_shapes[i]->m_data.id;
//				return m_shapes[i];
//			}
//		}
//		id.id = 0;
//		return 0;
//	}
//
//	VOID				B2Body::RenderShapes()
//	{
//		for(int i=0;i<m_shapes.size();i++)
//			m_shapes[i]->Render();
//	}
//	VOID				B2Body::OnFrameMove()	//刷新最新位置和转动率
//	{
//		for(int i=0;i<m_shapes.size();i++)
//			m_shapes[i]->OnFrameMove();
//	}
//	VOID				B2Body::ChangeToEngine()	//刷新到物理引擎
//	{
//		if(!m_worldPtr->m_world)
//			return;
//		if(m_bodyPtr)
//			m_worldPtr->m_world->DestroyBody(m_bodyPtr);
//		m_bodyPtr	=	0;
//		b2BodyDef def;
//		def.type		=	(b2BodyType)m_bodyData.bodyType;//b2BodyType
//		def.position	=	v2(m_bodyData.position);
//		def.linearVelocity	=	v2(m_bodyData.linearVelocity);
//		def.angle		=	m_bodyData.angle;
//		def.angularVelocity		=	m_bodyData.angularVelocity;
//		def.linearDamping		=	m_bodyData.linearDamping;
//		def.angularDamping		=	m_bodyData.angularDamping;
//		def.gravityScale		=	m_bodyData.gravityScale;
//		def.allowSleep			=	(m_bodyData.allowSleep > 0);
//		def.awake				=	(m_bodyData.awake > 0);
//		def.fixedRotation		=	(m_bodyData.fixedRotation > 0);
//		def.bullet				=	(m_bodyData.bullet > 0);
//		def.active				=	(m_bodyData.active > 0);
//		m_bodyPtr	=	m_worldPtr->m_world->CreateBody(&def);
//		for(int i=0;i<this->m_shapes.size();i++)
//			this->m_shapes[i]->ChangeToEngine();
//	}
//
//	VOID				B2Body::OnDestroyBox2D()
//	{
//		m_bodyPtr = 0;
//		for(int i=0;i<m_shapes.size();i++)
//			m_shapes[i]->OnDestroyBox2D();
//	}
//	B2Body*				B2Body::Clone()
//	{
//		B2Body* b2	=	m_worldPtr->AddBody();
//		int id		=	b2->m_bodyData.id;
//		memcpy(&b2->m_bodyData, &m_bodyData, sizeof(B2BodyData));
//		b2->m_bodyData.id	=	id;
//		for(int i=0;i<m_shapes.size();i++)
//		{
//			m_shapes[i]->Clone(b2);
//		}
//		return b2;
//	}
//
//	B2Shape*			B2Body::AddShape()
//	{
//		B2Shape* s = new B2Shape(this);
//		this->m_shapes.push_back(s);
//		return s;
//	}
//
//	void B2DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
//	{
//		m_temp.SetArrayCount(vertexCount);
//		for(int i=0;i<vertexCount;i++)
//		{
//			m_temp[i]	=	m_b2World->ConvertBox2dToPixel(float2(vertices[i].x, vertices[i].y), true);
//		}
//		if(m_temp.size() > 0)
//			g_manager.DrawLines2D(m_temp.GetPtr(), m_temp.size(), color4(color.r, color.g, color.b, 1), false);
//	}
//
//	void B2DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
//	{
//		m_temp.SetArrayCount(vertexCount);
//		for(int i=0;i<vertexCount;i++)
//		{
//			m_temp[i]	=	m_b2World->ConvertBox2dToPixel(float2(vertices[i].x, vertices[i].y), true);
//		}
//		if(m_temp.size() > 0)
//		{
//			g_manager.DrawLines2D(m_temp.GetPtr(), m_temp.size(), color4(color.r, color.g, color.b, 1), true);
//			g_manager.DrawLines2D(m_temp.GetPtr(), m_temp.size(), color4(color.r, color.g, color.b, 1), false);
//		}
//	}
//
//	void B2DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
//	{
//		const int	  k_segments = 16.0f;
//		const float32 k_increment = 2.0f * b2_pi / (float)k_segments;
//		float32 theta = 0.0f;
//		m_temp.SetArrayCount(k_segments);
//		for (int32 i = 0; i < k_segments; ++i)
//		{
//			b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
//			m_temp[i]	=	m_b2World->ConvertBox2dToPixel(float2(v.x, v.y), true);
//			theta += k_increment;
//		}
//		if(m_temp.size() > 0)
//			g_manager.DrawLines2D(m_temp.GetPtr(), m_temp.size(), color4(color.r, color.g, color.b, 1), false);
//	}
//
//	void B2DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
//	{
//		const float32 k_segments = 16.0f;
//		const float32 k_increment = 2.0f * b2_pi / k_segments;
//		float32 theta = 0.0f;
//		m_temp.SetArrayCount(k_segments);
//		for (int32 i = 0; i < k_segments; ++i)
//		{
//			b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
//			m_temp[i]	=	m_b2World->ConvertBox2dToPixel(float2(v.x, v.y), true);
//			theta += k_increment;
//		}
//		if(m_temp.size() > 0)
//		{
//			g_manager.DrawLines2D(m_temp.GetPtr(), m_temp.size(), color4(color.r, color.g, color.b, 1), true);
//			g_manager.DrawLines2D(m_temp.GetPtr(), m_temp.size(), color4(color.r, color.g, color.b, 1), false);
//		}
//	}
//
//	void B2DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
//	{
//		m_temp.SetArrayCount(2);
//		m_temp[0]	=	m_b2World->ConvertBox2dToPixel(float2(p1.x, p1.y), true);
//		m_temp[1]	=	m_b2World->ConvertBox2dToPixel(float2(p2.x, p2.y), true);
//		g_manager.DrawLines2D(m_temp.GetPtr(), m_temp.size(), color4(color.r, color.g, color.b, 1), false);
//	}
//
//	void B2DebugDraw::DrawTransform(const b2Transform& xf)
//	{
//	}
//
//	void B2DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
//	{
//		Pixel begin	=	m_b2World->ConvertBox2dToPixel(float2(p.x - size, p.y - size), true);
//		Pixel end		=	m_b2World->ConvertBox2dToPixel(float2(p.x + size, p.y + size), true);
//		g_manager.DrawRect2D(Rect(begin.x, begin.y, end.x, end.y), color4(color.r, color.g, color.b, 1), false);
//	}
//
//	void B2DebugDraw::DrawString(int x, int y, const char* string, ...)
//	{
//	}
//	void B2DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& color)
//	{
//		Pixel begin	=	m_b2World->ConvertBox2dToPixel(float2(aabb->lowerBound.x, aabb->lowerBound.y), true);
//		Pixel end		=	m_b2World->ConvertBox2dToPixel(float2(aabb->upperBound.x, aabb->upperBound.y), true);
//		g_manager.DrawRect2D(Rect(begin.x, begin.y, end.x, end.y), color4(color.r, color.g, color.b, 1), false);
//	}
//
//	VOID	B2World::CreateBox2D()	//创建场景
//	{
//		DestroyBox2D();
//		b2Vec2 gravity;
//		gravity.Set(0.0f, -10.0f);
//		m_world = new b2World(gravity);
//		m_world->SetDestructionListener(&m_destroyCB);
//		m_world->SetContactListener(this);
//		m_world->SetDebugDraw(&m_debugDraw);
//		b2BodyDef bodyDef;
//		m_groundBody = m_world->CreateBody(&bodyDef);
//		for(int i=0;i<m_bodys.size();i++)
//		{
//			this->m_bodys[i]->ChangeToEngine();
//		}
//		for(int i=0;i<m_joints.size();i++)
//		{
//			this->m_joints[i]->ChangeToEngine();
//		}
//		//
//		//m_count	=	0;
//		//b2Body* ground = NULL;
//		//{
//		//	b2BodyDef bd;
//		//	ground = m_world->CreateBody(&bd);
//		//}
//
//		//{
//		//	b2BodyDef bd;
//		//	bd.type = b2_dynamicBody;
//		//	bd.allowSleep = false;
//		//	bd.position.Set(0.0f, 10.0f);
//		//	b2Body* body = m_world->CreateBody(&bd);
//
//		//	b2PolygonShape shape;
//		//	shape.SetAsBox(0.5f, 10.0f, b2Vec2( 10.0f, 0.0f), 0.0);
//		//	body->CreateFixture(&shape, 5.0f);
//		//	shape.SetAsBox(0.5f, 10.0f, b2Vec2(-10.0f, 0.0f), 0.0);
//		//	body->CreateFixture(&shape, 5.0f);
//		//	shape.SetAsBox(10.0f, 0.5f, b2Vec2(0.0f, 10.0f), 0.0);
//		//	body->CreateFixture(&shape, 5.0f);
//		//	shape.SetAsBox(10.0f, 0.5f, b2Vec2(0.0f, -10.0f), 0.0);
//		//	body->CreateFixture(&shape, 5.0f);
//
//		//	b2RevoluteJointDef jd;
//		//	jd.bodyA = ground;
//		//	jd.bodyB = body;
//		//	jd.localAnchorA.Set(0.0f, 10.0f);
//		//	jd.localAnchorB.Set(0.0f, 0.0f);
//		//	jd.referenceAngle = 0.0f;
//		//	jd.motorSpeed = 0.05f * b2_pi;
//		//	jd.maxMotorTorque = 1e8f;
//		//	jd.enableMotor = true;
//		//	m_world->CreateJoint(&jd);
//		//}
//
//		m_count = 0;
//	}
//	VOID	B2World::DestroyBox2D()	//删除场景
//	{
//		for(int i=0;i<m_bodys.size();i++)
//			m_bodys[i]->OnDestroyBox2D();
//		for(int i=0;i<m_joints.size();i++)
//			m_joints[i]->OnDestroyBox2D();
//		if(m_world)
//			delete m_world;
//		m_world	=	0;
//	}
//
//	B2World::B2World()
//	{
//		m_dlgPtr	=	0;
//		m_world		=	0;
//		m_debugDraw.m_b2World	=	this;
//		m_velocityIterations	=	8;
//		m_positionIterations	=	3;
//		m_pointCount = 0;
//		m_viewScale.setxy(20.0f, 20.0f);
//		memset(m_points, 0, sizeof(m_points));
//		m_bPause	=	false;
//		m_id		=	1;
//		m_bEnabled	=	false;
//		m_bIsRender	=	false;
//		///
//	}
//
//	B2World::~B2World()
//	{
//		DestroyBox2D();
//		for(int i=0;i<m_bodys.size();i++)
//		{
//			m_bodys[i]->m_worldPtr	=	0;
//			delete m_bodys[i];
//		}
//		m_bodys.clear();
//		for(int i=0;i<m_joints.size();i++)
//		{
//			m_joints[i]->m_worldPtr	=	0;
//			delete m_joints[i];
//		}
//		m_joints.clear();
//	}
//
//	float		B2World::ConvertPixelToBox2d(int pos)
//	{
//		//if(this->m_dlgPtr)
//		//{
//		//	int x, y;
//		//	m_dlgPtr->GetLocation(x, y);
//		//	pos	-=	x;
//		//}
//		return ((float)pos / (float)BOX2D_SCREEN_HEIGHT) * m_viewScale.x;
//	}
//	float		B2World::GetScreenPosY(int pos)
//	{
//		return ((float)pos / (float)BOX2D_SCREEN_WIDTH) * m_viewScale.y;
//	}
//	float		B2World::GetScreenPosX(int pos)
//	{
//		return ((float)pos / (float)BOX2D_SCREEN_WIDTH) * m_viewScale.x;
//	}
//	int			B2World::GetScreenPixelY(float pos)
//	{
//		int ret = (int)((pos / m_viewScale.y) * (float)BOX2D_SCREEN_WIDTH);
//		//if(this->m_dlgPtr)
//		//{
//		//	int x, y;
//		//	m_dlgPtr->GetLocation(x, y);
//		//	ret	+=	x;
//		//}
//		return ret;
//	}
//	int			B2World::GetScreenPixelX(float pos)
//	{
//		int ret = (int)((pos / m_viewScale.x) * (float)BOX2D_SCREEN_WIDTH);
//		//if(this->m_dlgPtr)
//		//{
//		//	int x, y;
//		//	m_dlgPtr->GetLocation(x, y);
//		//	ret	+=	x;
//		//}
//		return ret;
//	}
//	int			B2World::ConvertBox2dToPixel(float pos)
//	{
//		int ret = (int)((pos / m_viewScale.x) * (float)BOX2D_SCREEN_HEIGHT);
//		//if(this->m_dlgPtr)
//		//{
//		//	int x, y;
//		//	m_dlgPtr->GetLocation(x, y);
//		//	ret	+=	x;
//		//}
//		return ret;
//	}
//
//	float2		B2World::GetScreenPosSize(const Pixel& pos)
//	{
//		float2 r;
//		r.x	=	((float)pos.x / (float)BOX2D_SCREEN_HEIGHT) * m_viewScale.x;
//		r.y	=	((float)pos.y / (float)BOX2D_SCREEN_WIDTH) * m_viewScale.y;
//		return r;
//	}
//
//	Pixel		B2World::GetScreenPixelSize(const float2& pos)
//	{
//		Pixel p;
//		p.x	=	(int)(((pos.x / m_viewScale.x)) * (float)BOX2D_SCREEN_HEIGHT);
//		p.y	=	(int)(((pos.y / m_viewScale.y)) * (float)BOX2D_SCREEN_WIDTH);
//		return p;
//	}
//
//	float2		B2World::ConvertPixelToBox2d(const Pixel& pos)
//	{
//		Pixel p = pos;
//		//if(this->m_dlgPtr)
//		//{
//		//	int x, y;
//		//	m_dlgPtr->GetLocation(x, y);
//		//	p.x	-=	x;
//		//	p.y	-=	y;
//		//}
//		float2 r;
//		r.x	=	((float)p.x / (float)BOX2D_SCREEN_HEIGHT - 0.5f) * m_viewScale.x;
//		r.y	=	(0.5f-(float)p.y / (float)BOX2D_SCREEN_WIDTH) * m_viewScale.y;
//		return r;
//	}
//
//	Pixel		B2World::ConvertBox2dToPixel(const float2& pos, BOOL bOffsetToDlg)
//	{
//		Pixel p;
//		p.x	=	(int)(((pos.x / m_viewScale.x) + 0.5f) * (float)BOX2D_SCREEN_HEIGHT);
//		p.y	=	(int)(((0.5f - pos.y / m_viewScale.y)) * (float)BOX2D_SCREEN_WIDTH);
//		if(this->m_dlgPtr && bOffsetToDlg)
//		{
//			int x, y;
//			m_dlgPtr->GetLocation(x, y);
//			p.x	+=	x;
//			p.y	+=	y;
//		}
//		return p;
//	}
//
//	void B2World::BeginContact(b2Contact* contact) { B2_NOT_USED(contact); }
//	void B2World::EndContact(b2Contact* contact) { B2_NOT_USED(contact); }
//	void B2World::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
//	{
//		const b2Manifold* manifold = contact->GetManifold();
//
//		if (manifold->pointCount == 0)
//		{
//			return;
//		}
//
//		b2Fixture* fixtureA = contact->GetFixtureA();
//		b2Fixture* fixtureB = contact->GetFixtureB();
//
//		b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
//		b2GetPointStates(state1, state2, oldManifold, manifold);
//
//		b2WorldManifold worldManifold;
//		contact->GetWorldManifold(&worldManifold);
//
//		for (int32 i = 0; i < manifold->pointCount && m_pointCount < k_maxContactPoints; ++i)
//		{
//			ContactPoint* cp = m_points + m_pointCount;
//			cp->fixtureA = fixtureA;
//			cp->fixtureB = fixtureB;
//			cp->position = worldManifold.points[i];
//			cp->normal = worldManifold.normal;
//			cp->state = state2[i];
//			++m_pointCount;
//		}
//	}
//
//	B2Body*			B2World::AddBody()
//	{
//		B2Body* body	=	new B2Body(this);
//		this->m_bodys.push_back(body);
//		return body;
//	}
//
//	B2Joint*		B2World::AddJoint()
//	{
//		B2Joint* body	=	new B2Joint(this);
//		this->m_joints.push_back(body);
//		return body;
//	}
//
//	B2Body*			B2World::AddDynBox(const float2& getmin, const float2& getmax)	//产生盒子
//	{
//		B2Body* b	=	AddBody();
//		B2Shape* s	=	b->AddShape();
//		float2 center((getmax.x + getmin.x) * 0.5f, (getmax.y + getmin.y) * 0.5f);
//		b->SetPosition(center);
//		s->CreateBox(getmax.x - getmin.x, getmax.y - getmin.y, 0, 0);
//		return b;
//	}
//	B2Body*			B2World::AddDynCircle(const float2& center, float fRadius)	//产生circle
//	{
//		B2Body* b	=	AddBody();
//		B2Shape* s	=	b->AddShape();
//		b->SetPosition(center);
//		s->CreateCircle(float2(0, 0), fRadius);
//		return b;
//	}
//
//	VOID B2World::OnRemoveBody(B2Body* b)
//	{
//		for(int i=0;i<m_bodys.size();i++)
//		{
//			if(m_bodys[i] == b)
//			{
//				m_bodys.eraseByIndex(i);
//				break;
//			}
//		}
//	}
//
//	VOID B2World::OnRemoveJoint(B2Joint* b)
//	{
//		for(int i=0;i<m_joints.size();i++)
//		{
//			if(m_joints[i] == b)
//			{
//				m_joints.eraseByIndex(i);
//				break;
//			}
//		}
//	}
//
//	void B2World::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
//	{
//		B2_NOT_USED(contact);
//		B2_NOT_USED(impulse);
//	}
//
//	void	B2World::OnFrameMove(float fElapsedTime, BOOL enableWarmStarting, BOOL enableContinuous, BOOL enableSubStepping)
//	{
//		if(m_world && !m_bPause)
//		{
//			m_world->SetWarmStarting(enableWarmStarting == TRUE);
//			m_world->SetContinuousPhysics(enableContinuous == TRUE);
//			m_world->SetSubStepping(enableSubStepping == TRUE);
//			m_world->Step(fElapsedTime, m_velocityIterations, m_positionIterations);
//			//
//			for(int i=0;i<m_bodys.size();i++)
//			{
//				m_bodys[i]->OnFrameMove();
//			}
//			//if (m_count < 50)
//			//{
//			//	b2BodyDef bd;
//			//	bd.type = b2_dynamicBody;
//			//	bd.position.Set(0.0f, 10.0f);
//			//	b2Body* body = m_world->CreateBody(&bd);
//
//			//	b2PolygonShape shape;
//			//	shape.SetAsBox(0.125f, 0.125f);
//			//	body->CreateFixture(&shape, 1.0f);
//
//			//	++m_count;
//			//}
//		}
//	}
//
//	VOID	B2World::Render()
//	{
//		if(m_bIsRender)
//		{
//			uint32 flags = 0;
//			flags += b2Draw::e_shapeBit | b2Draw::e_jointBit;
//			m_debugDraw.SetFlags(flags);
//			g_manager->SetCullMode(CullMode_Double);
//			if(m_world)
//			{
//				m_world->DrawDebugData();
//			}
//			else
//			{
//				for(int i=0;i<m_bodys.size();i++)
//				{
//					m_bodys[i]->RenderShapes();
//				}
//				for(int i=0;i<m_joints.size();i++)
//				{
//					m_joints[i]->Render();
//				}
//			}
//		}
//	}
//
//	B2Body*		B2World::GetBodyPtr(PtrID& id)
//	{
//		if(id.id == 0)
//			return 0;
//		if(id.index < m_bodys.size() && m_bodys[id.index]->m_bodyData.id == id.id)
//			return m_bodys[id.index];
//		for(int i=0;i<m_bodys.size();i++)
//		{
//			if(m_bodys[i]->m_bodyData.id == id.id)
//			{
//				id.index	=	i;
//				id.id = m_bodys[i]->m_bodyData.id;
//				return m_bodys[i];
//			}
//		}
//		id.id = 0;
//		return 0;
//	}
//
//	B2Joint*	B2World::GetJointPtr(PtrID& id)
//	{
//		if(id.id == 0)
//			return 0;
//		if(id.index < m_joints.size() && m_joints[id.index]->m_data.id == id.id)
//			return m_joints[id.index];
//		for(int i=0;i<m_joints.size();i++)
//		{
//			if(m_joints[i]->m_data.id == id.id)
//			{
//				id.index	=	i;
//				id.id = m_joints[i]->m_data.id;
//				return m_joints[i];
//			}
//		}
//		id.id = 0;
//		return 0;
//	}
//
//	VOID		B2World::Save(CSafeFileHelperW& w)
//	{
//		int version = 1;
//		w << version << m_viewScale << m_velocityIterations << m_positionIterations << m_bEnabled;	//
//		w << m_id;
//		int bodys	=	m_bodys.size();
//		int joints	=	m_joints.size();
//		w << bodys << joints;
//		for(int i=0;i<bodys;i++)
//			m_bodys[i]->Save(w);		//动态创建的
//		for(int i=0;i<joints;i++)
//			m_joints[i]->Save(w);	//样子
//	}
//
//	VOID		B2World::Load(CSafeFileHelperR& r)
//	{
//		int version = 1;
//		r >> version >> m_viewScale >> m_velocityIterations >> m_positionIterations >> m_bEnabled;	//
//		r >> m_id;
//		int bodys;
//		int joints;
//		r >> bodys >> joints;
//		for(int i=0;i<bodys;i++)
//		{
//			B2Body* body	=	new B2Body(this);
//			body->Load(r);		//动态创建的
//			m_bodys.push_back(body);
//		}
//		for(int i=0;i<joints;i++)
//		{
//			B2Joint* joint	=	new B2Joint(this);
//			joint->Load(r);		//动态创建的
//			m_joints.push_back(joint);
//		}
//	}
//
//};
//#endif
