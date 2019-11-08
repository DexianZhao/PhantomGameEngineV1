//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   Box2D_Usage.h
软件名称   :   幻影游戏引擎

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifndef ___BOX2D_USAGE__H___
//#define ___BOX2D_USAGE__H___
//
//#ifndef EXPORT_ENGINE
//#pragma once
//#include "PhantomBase.h"
//#include "box2d/Box2D.h"
//
//namespace Phantom{
//
//	class	UIDialog;
//
//#pragma pack(push, 1)
//
//	inline	b2Vec2	v2(const float2& v){return b2Vec2(v.x,v.y);}
//	inline	float2	v2(const b2Vec2& v){return float2(v.x, v.y);}
//	struct	B2ShapeData
//	{
//		enum{
//			e_polygon_box	=	10,
//		};
//		int					id;
//		int					type;//b2Shape::Type
//		//base
//		float				m_density;
//		float				m_radius;//
//		//circle only圆, box
//		float2				m_p;//position
//		//box
//		float				m_fAngle;//box
//		float				m_boxWidth;//box
//		float				m_boxHeight;//box
//		//chain only链
//		BOOL				m_bChainLoop;
//		//edge only边缘
//		float2				m_vertex1, m_vertex2;
//		float2				m_vertex0, m_vertex3;
//		BOOL				m_hasVertex0, m_hasVertex3;
//		//
//		PtrID				m_bindControl;	//绑定控件
//		//polygon only多边形
//	};
//
//
//	struct	B2JointData//联合
//	{
//		int				id;
//		Name			name;
//		int				jointType;//b2JointType
//		//
//		int				bodyA;
//		int				bodyB;
//		BOOL			collideConnected;
//		//friction joint only摩擦
//		float2			localAnchorA;//锚and prismatic ,pulley, revolute
//		float2			localAnchorB;//锚and prismatic ,pulley, revolute
//		float			maxForce;//强制and mouse
//		float			maxTorque;//转矩
//		//gear joint only齿轮
//		int				joint1,joint2;//id
//		//B2JointData*	joints;//joint1,*joint2;,绑定
//		float			ratio;//or mouse dumping Ratio, or pulley radio
//		//mouse joint only鼠标
//		float2			target;
//		//float			maxForce;
//		float			frequencyHz;
//		//float			dampingRatio;
//		//prismatic joint only棱镜
//		//float2			localAnchorA;
//		//float2			localAnchorB;
//		float2			localAxisA;//轴
//		float			referenceAngle;//revolute
//		int				enableLimit;//revolute
//		float			lowerTranslation;
//		float			upperTranslation;
//		int				enableMotor;//revolute
//		float			maxMotorForce;
//		float			motorSpeed;//and revolute
//		//pulley joint only滑轮
//		float2			groundAnchorA;
//		float2			groundAnchorB;
//		//float2			localAnchorA;
//		//float2			localAnchorB;
//		float			lengthA;
//		float			lengthB;
//		//float32 ratio;
//		//revolute joint only旋转
//		//float2			localAnchorA;
//		//float2			localAnchorB;
//		//float			referenceAngle;
//		//int				enableLimit;
//		float			lowerAngle;
//		float			upperAngle;
//		//int				enableMotor;
//		//float			motorSpeed;
//		float			maxMotorTorque;
//		//rope joint绳
//		//float2			localAnchorA;
//		//float2			localAnchorB;
//		float			maxLength;
//		//weld joint//焊接
//		//b2Vec2 localAnchorA;
//		//b2Vec2 localAnchorB;
//		//float32 referenceAngle;
//		//float32 frequencyHz;
//		//float32 dampingRatio;
//		//wheel joint
//		//b2Vec2 localAnchorA;
//		//b2Vec2 localAnchorB;
//		//b2Vec2 localAxisA;
//		//int	enableMotor;
//		//float32 maxMotorTorque;
//		//float32 motorSpeed;
//		//float32 frequencyHz;
//		//float32 dampingRatio;
//	};
//
//	struct	B2BodyData
//	{
//		int				id;
//		Name			name;
//		int				bodyType;//b2BodyType
//		float2			position;
//		float			angle;
//		float2			linearVelocity;
//		float			angularVelocity;
//		float			linearDamping;
//		float			angularDamping;
//		float			gravityScale;
//		BOOL			allowSleep;
//		BOOL			awake;
//		BOOL			fixedRotation;
//		BOOL			bullet;
//		BOOL			active;
//	};
//
//#pragma pack(pop)
//
//	class	B2World;
//	class	B2Body;
//
//	class	B2Shape
//	{
//	public:
//		virtual ~B2Shape();
//	public:
//		B2ShapeData				m_data;
//		std::vector<float2>		m_vertices;	//
//		//
//		B2Shape*				Clone(B2Body* v);
//		//
//		VOID					CreateCircle(const float2& pos, float fRadius);
//		VOID					CreateBox(float fWidth, float fHeight, const float2& center, float fAngle);
//		VOID					CreateEdge(const float2& t1, const float2& t2, const float2* v0 = 0, const float2* v3 = 0);
//		VOID					CreatePolygon(const float2* v, int nCount);//<8
//		VOID					CreateChain(const float2* v, int nCount);
//		VOID					CreateChainLoop(const float2* v, int nCount);
//		b2Fixture*				GetShapePtr(){return m_shapePtr;}
//		//
//		b2Vec2					ToBodyPos(const float2& v);
//		float2					GetWorldPos();
//		VOID					Render();
//		VOID					GetRect(rect_f& ret);
//		VOID					ChangeToEngine();	//刷新到物理引擎
//		//
//		VOID					OnFrameMove();	//刷新最新位置和转动率
//		//
//		VOID					Save(CSafeFileHelperW& w);
//		VOID					Load(CSafeFileHelperR& r);
//	protected:
//		VOID					OnDestroyBox2D(){m_shapePtr = 0;}
//		B2Shape(B2Body* body);
//		//
//		friend class			B2Body;
//		b2Fixture*				m_shapePtr;
//		B2Body*					m_bodyPtr;
//	};
//
//	class	B2Body
//	{
//	public:
//		B2Body(B2World* w);
//		virtual ~B2Body();
//	public:
//		B2BodyData					m_bodyData;
//		//
//		VOID						ChangeToEngine();	//刷新到物理引擎
//		b2Body*						GetBodyPtr(){return m_bodyPtr;}
//		//
//		B2Body*						Clone();
//		//
//		inline	VOID				SetIsDynamic(BOOL bChange = false){m_bodyData.bodyType = b2_dynamicBody; if(bChange)ChangeToEngine();}
//		VOID						SetPosition(const float2& v, BOOL bChange = false){m_bodyData.position = v; if(bChange)ChangeToEngine();}
//		const float2&				GetPosition(){return m_bodyData.position;}
//		//
//		B2Shape*					AddShape();
//		INT							GetShapeCount(){return m_shapes.size();}
//		B2Shape*					GetShapePtr(INT nIndex){if(nIndex < 0 || nIndex >= m_shapes.size())return 0; return m_shapes[nIndex];}
//		B2Shape*					GetShapePtr(PtrID& id);
//		float2						ToShapePos(const float2& v){return float2(v.x - m_bodyData.position.x, v.y - m_bodyData.position.y);}
//		float2						ToBodyPos(const float2& v){return float2(v.x + m_bodyData.position.x, v.y + m_bodyData.position.y);}
//		//
//		VOID						RenderShapes();
//		rect_f						GetBodyRect();
//		B2World*					GetWorldPtr(){return m_worldPtr;}
//		VOID						OnFrameMove();	//刷新最新位置和转动率
//		VOID						Save(CSafeFileHelperW& w);
//		VOID						Load(CSafeFileHelperR& r);
//	protected:
//		VOID						OnDestroyBox2D();
//		friend class	B2Joint;
//		friend class	B2Shape;
//		friend class	B2World;
//		//
//		VOID						RemoveShape(B2Shape* b);
//		B2World	*					m_worldPtr;
//		b2Body*						m_bodyPtr;
//		CDynamicArray<B2Shape*>		m_shapes;	//样子
//	};
//
//	class	B2Joint
//	{
//	public:
//		virtual ~B2Joint();
//		B2JointData				m_data;
//		VOID					ChangeToEngine();	//刷新到物理引擎
//		VOID					Render();
//		b2Joint*				GetJointPtr(){return m_jointPtr;}
//		B2World*				GetWorldPtr(){return m_worldPtr;}
//		VOID					SetType(int t);
//		VOID					Save(CSafeFileHelperW& w);
//		VOID					Load(CSafeFileHelperR& r);
//	protected:
//		VOID					OnDestroyBox2D(){m_jointPtr = 0;}
//		B2Joint(B2World* body);
//		friend class			B2World;
//		B2World*				m_worldPtr;
//		b2Joint*				m_jointPtr;
//	};
//
//	class	B2DebugDraw: public b2Draw
//	{
//	public:
//		void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
//		void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
//		void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
//		void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
//		void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
//		void DrawTransform(const b2Transform& xf);
//		void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
//		void DrawString(int x, int y, const char* string, ...); 
//		void DrawAABB(b2AABB* aabb, const b2Color& color);
//		CDynamicArray<Pixel>	m_temp;
//		B2World	*				m_b2World;
//	};
//
//	//被删除时调用
//	class B2DestructionListener : public b2DestructionListener
//	{
//	public:
//		void SayGoodbye(b2Fixture* fixture) { B2_NOT_USED(fixture); }
//		void SayGoodbye(b2Joint* joint) { B2_NOT_USED(joint); }
//	};
//
//	const int32 k_maxContactPoints = 2048;
//
//	struct ContactPoint
//	{
//		b2Fixture* fixtureA;
//		b2Fixture* fixtureB;
//		b2Vec2 normal;
//		b2Vec2 position;
//		b2PointState state;
//	};
//
//	class	B2World: public b2ContactListener
//	{
//	public:
//		B2World();
//		virtual	~B2World();
//		virtual void BeginContact(b2Contact* contact);
//		virtual void EndContact(b2Contact* contact);
//		virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
//		virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
//		virtual	VOID			OnFrameMove(float fElapsedTime, BOOL enableWarmStarting = true, BOOL enableContinuous = true, BOOL enableSubStepping = false);
//		VOID					Render();
//		//
//		B2Body*					AddBody();
//		B2Joint*				AddJoint();
//		b2World*				GetWorldPtr(){return m_world;}
//		//
//		INT						GetBodyCount(){return m_bodys.size();}
//		B2Body*					GetBodyPtr(INT nIndex){if(nIndex < 0 || nIndex >= m_bodys.size())return 0; return m_bodys[nIndex];}
//		INT						GetJointCount(){return m_joints.size();}
//		B2Joint*				GetJointPtr(INT nIndex){if(nIndex < 0 || nIndex >= m_joints.size())return 0; return m_joints[nIndex];}
//		B2Body*					GetBodyPtr(PtrID& id);
//		B2Joint*				GetJointPtr(PtrID& id);
//		inline	b2Body*			GetBox2dBodyPtr(PtrID& id){B2Body* b = GetBodyPtr(id); if(b) return b->GetBodyPtr(); return 0;}
//		inline	b2Joint*		GetBox2dJointPtr(PtrID& id){B2Joint* b = GetJointPtr(id); if(b) return b->GetJointPtr(); return 0;}
//		//
//		float2					ConvertPixelToBox2d(const Pixel& pos);
//		Pixel					ConvertBox2dToPixel(const float2& pos, BOOL bOffsetToDlg = false);
//		float2					GetScreenPosSize(const Pixel& pos);
//		Pixel					GetScreenPixelSize(const float2& pos);
//		float					ConvertPixelToBox2d(int pos);
//		int						ConvertBox2dToPixel(float pos);
//		float					GetScreenPosX(int pos);
//		int						GetScreenPixelX(float pos);
//		float					GetScreenPosY(int pos);
//		int						GetScreenPixelY(float pos);
//		inline	int				GetNewID(){return (m_id++);}
//		//
//		B2Body*					AddDynBox(const float2& min, const float2& max);	//产生盒子
//		B2Body*					AddDynCircle(const float2& center, float fRadius);	//产生circle
//		//
//		VOID					Pause(BOOL b){m_bPause = b;}
//		BOOL					IsPause(){return m_bPause;}
//		//
//		VOID					CreateBox2D();	//创建场景
//		VOID					DestroyBox2D();	//删除场景
//		VOID					SetDialogPtr(UIDialog* d){m_dlgPtr = d;}
//		//
//		VOID					Save(CSafeFileHelperW& w);
//		VOID					Load(CSafeFileHelperR& r);
//		BOOL					IsEnable(){return m_bEnabled;}
//		VOID					SetEnabled(BOOL b){m_bEnabled = b;}
//		//
//		BOOL					IsRenderEnable(){return m_bIsRender;}
//		VOID					SetRenderEnable(BOOL b){m_bIsRender = b;}
//	protected:
//		VOID					OnRemoveBody(B2Body* b);
//		VOID					OnRemoveJoint(B2Joint* b);
//		friend class			B2Body;
//		friend class			B2Shape;
//		friend class			B2Joint;
//	protected:
//		CDynamicArray<B2Body*>		m_bodys;		//动态创建的
//		CDynamicArray<B2Joint*>		m_joints;	//样子
//		BOOL					m_bPause;
//		BOOL					m_bEnabled;
//		BOOL					m_bIsRender;
//		//
//		float2					m_viewScale;	//((box2d.x / m_viewScale.x) + 0.5f) * screenWidth, height...
//		int						m_velocityIterations;	//
//		int						m_positionIterations;	//
//		ContactPoint			m_points[k_maxContactPoints];
//		int32					m_pointCount;
//		//
//		int						m_count;
//		B2DestructionListener	m_destroyCB;
//		B2DebugDraw				m_debugDraw;
//		b2World*				m_world;
//		b2Body*					m_groundBody;
//		UIDialog*		m_dlgPtr;
//		//
//		int						m_id;
//	};
//
//};
//
//#endif
//
//#endif
