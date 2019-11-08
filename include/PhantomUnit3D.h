//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UNIT3D_H__
#define __PHANTOM_UNIT3D_H__
#include "PhantomUnit3DElement.h"
#include "PhantomMesh.h"

namespace Phantom{

class Unit3DActionData;

class Unit3DData;

class Unit3DActionData
{
public:
	//
	Unit3DActionData(Unit3DData* u);
	virtual	~Unit3DActionData(void);
	int									GetID(){return m_id;}
	virtual	int							GetType(){return 0;}
	//
	virtual	INT							GetElementCount();	//返回特效元素数量
	virtual	CActionElement*				GetElementPtr(INT nIndex);	//返回特效元素
	virtual	CActionElement*				GetElementPtrByID(INT nID);	//返回特效元素
	virtual	CActionElement*				CreateElement(ElementType type);	//从文件创建一个元素
	virtual	VOID						DeleteElement(CActionElement* ele);	//从文件创建一个元素
	virtual	VOID						SetTrackKeyCount(INT nFrameCount);	//关键帧数量
	virtual	INT							GetTrackKeyCount();
	virtual	VOID						SetPlaySpeed(float fSpeed){m_fPlaySpeed = getmax(fSpeed, 0.01f);}
	virtual	float						GetPlaySpeed(){return m_fPlaySpeed;}
	//
	virtual	VOID						CloneFrom(Unit3DActionData* proj);
	virtual	VOID						Load(CSafeFileHelperR& r);
	virtual	VOID						Save(CSafeFileHelperW& w);
	virtual	VOID						Destroy();
	virtual	ActionType					GetActionType(){return (ActionType)m_actionType;}	//是否默认动作
	virtual	VOID						SetActionType(ActionType t);
	//
	virtual	VOID						SetFloatParam(int index, float f);
	virtual	float						GetFloatParam(int index);	//如果是攻击，则被击时间差
	//
	virtual	VOID						SortElements();
	//
	virtual	BOOL						IsTemplateProject(){return m_bIsTemplate;}		//是否模板动作
	virtual	VOID						SetTemplateProject(BOOL b){m_bIsTemplate = b;}	//设置为模板动作
public:
	char								m_bIsTemplate;
	float								m_parameter[MAX_FLOAT_PARAMETER];
	//
	NameT<32>							m_name;
	int									m_id;
	short								m_nTrackKeyCount;
	float								m_fPlaySpeed;
	std::vector<CActionElement*>		m_elements;
	char								m_actionType;
public:
	Unit3DData*							m_unitPtr;
};

class Unit3DMeshData
{
public:
	Unit3DMeshData(Unit3DData* u);
	virtual ~Unit3DMeshData();
public:
	VOID									SetName(const char* szName){m_name = szName;}
	const char*								GetName(){return m_name;}
	VOID									SetMeshFile(const char* szFile){m_fileName = szFile; m_nMeshSets++;}
	const char*								GetMeshFile(){return m_fileName;}
	VOID									Load(CSafeFileHelperR& r);
	VOID									Save(CSafeFileHelperW& w);
public:
	int										m_id;
	FileName								m_fileName;
	NameT<32>								m_name;
	short									m_nMeshSets;
	Unit3DData*								m_unitPtr;
};

class	Unit3D;

class Unit3DData: public Base
{
public:
	Unit3DData();
	virtual	~Unit3DData();
	ImplementComValue();
public:
	const char*								GetFileName(){return m_fileName;}	//设置名称
	const char*								GetGeometryFile();	//设置模型文件
	VOID									SetGeometryFile(const char* szName);
	//
	VOID									SetActiveAction(PtrID& id);
	Unit3DActionData*						GetActiveAction();
	VOID									SetActiveAction(Unit3DActionData* act);
	//
	BOOL									DeleteAction(Unit3DActionData* a);
	Unit3DActionData*						CreateAction(int type);
	Unit3DActionData*						FindByID(int id);
	//
	BOOL									LoadUnit(const char* szFile);
	BOOL									SaveUnit(const char* szFile);
	int										GetNewIdentity(){return (m_id++);}
	VOID									CreateRender(AutoPtr<Unit3D>& ret);
	VOID									RebuildUnitActions();
	Unit3DMeshData*							GetChildMesh(INT nIndex);
	Unit3DMeshData*							CreateChildMesh();
	VOID									DeleteChildMesh(Unit3DMeshData* m);
	VOID									SetStaticUnit(BOOL b){m_bStaticUnit = b;}	//是否静态单位
	BOOL									IsStaticUnit(){return m_bStaticUnit;}
	VOID									SetTopDialog(const char* szDlgFile){m_dlgFileName = szDlgFile; m_dlgFileChanged++;}	//角色顶部对话框
	const char*								GetTopDialog(){return m_dlgFileName;}
	VOID									SetUnitTopHeight(float f){m_fUnitTopHeight = f;}
	float									GetUnitTopHeight(){return m_fUnitTopHeight;}
	float									m_fUnitTopHeight;
	VOID									SetPickEnable(BOOL b);	//是否可以被点击
	BOOL									IsPickEnable();
	BOOL									m_bIsPickEnable;
	BOOL									GetFileExist(FileName& ret, const char* in);
public:
	FileName								m_dlgFileName;
	int										m_dlgFileChanged;
	char									m_bStaticUnit;
	Unit3DActionData*						m_unitActions[ActionType_Count];
	FileName								m_fileName;
	FileName								m_geometryFileName;
	CDynamicArrayS<Unit3DActionData*,0>		m_actions;
	CDynamicArrayS<Unit3DMeshData*,0>		m_childMeshs;//							m_meshs[MAX_UNIT3D_CHILD_MESH];
	short									m_id;
};

class Unit3D;

class Unit3DAction
{
public:
	Unit3DAction();
	virtual ~Unit3DAction();
	VOID									Init(Unit3D* unitPtr, Unit3DActionData* act);
public:
	INT										GetID(){return m_actionPtr->GetID();}
	const char*								GetName(){return m_actionPtr->m_name;}
	//设置播放速度
	VOID									SetPlaySpeed(float fSpeed){m_fPlaySpeed = fSpeed;}
	float									GetPlaySpeed(){return m_fPlaySpeed;}
	//
	CElementRuntime*						SearchElement(const char* szElementName);	//模型指针
	CElementRuntime*						GetElementByID(INT nID);	//模型指针
	CElementRuntime*						GetElementByIndex(INT nIndex);	//模型指针
	INT										GetElementCount();
	//
	VOID									SetActive(BOOL b);//激活
	BOOL									IsActive();
	Unit3DActionData*						GetActionPtr(){return this->m_actionPtr;}
public:
	//
	virtual	VOID							BeforeFrameMove(EmissionFrameMove& fm, float fTrackIndex);
	//frameMove阶段，只有m_bRender有效时才会自动被调用
	virtual	VOID							FrameMove(Mesh* basePtr, float fTrackIndex, const matrix4x4& worldMat);
	virtual	VOID							OnRender();
	//
	VOID									BeginAction();
	VOID									StopAction();
	//
	//virtual	VOID							FrameMoveEmision(Mesh* basePtr, const matrix4x4& actionMat);
public:
	Unit3D*									m_unitPtr;
	Unit3DActionData*						m_actionPtr;
	CDynamicArrayS<CElementRuntime*, 0>		m_elements;
	float									m_fKeyFrameIndex;
	float									m_fPlaySpeed;
	float3									m_lastOffset;
};


class Unit3DMesh
{
public:
	Unit3DMesh();
	virtual ~Unit3DMesh();
public:
	const char*								GetName(){return m_meshPtr->m_name;}
	Mesh*									GetMeshPtr();
public:
	VOID									Setup(Unit3DMeshData* meshPtr);
public:
	AutoPtr<Mesh>							m_geometryPtr;
	Unit3DMeshData*							m_meshPtr;
	short									m_nMeshSets;
	char									m_bIsFrameMove;
	Unit3D*									m_unitPtr;
};

class Unit3D: public Base
{
public:
	ImplementComValue();
	//
	Unit3D(Unit3DData* p);
	virtual ~Unit3D();
	Mesh*									GetGeometryPtr(){return m_geometryPtr;}	//模型指针
	//
	VOID									Render();
	VOID									FrameMove();//
	BOOL									IsPlayOver(){return m_bPlayOver;}	//是否结束了至少一次循环
	//
	VOID									SetActiveAction(PtrID& id);
	Unit3DAction*							GetActiveAction();
	VOID									SetActiveAction(Unit3DAction* act);
	//
	float									GetKeyFrameCount();
	float									GetKeyFrame();
	VOID									SetKeyFrame(float f);
	//
	VOID									SetNextActionID(PtrID& id);	//设置播放完毕之后下一个动作ID，如果不设置则自动引用Unit3DActionData的设置
	BOOL									SetActiveAction(ActionType t, float fDelayTime = 0.0f);	//设置播放完毕之后下一个动作ID，如果不设置则自动引用Unit3DActionData的设置
	inline	VOID							PlayAction(ActionType t){SetActiveAction(t);}
	ActionType								GetActiveActionType(){return (ActionType)m_activeActionType;}	//设置播放完毕之后下一个动作ID，如果不设置则自动引用Unit3DActionData的设置
	VOID									SetActiveActionProj(Unit3DActionData* act);
	//
	const float3&							GetPosition();
	VOID									SetPosition(const float3& pos);
	const matrix4x4&						GetWorldMatrix();
	VOID									SetWorldMatrix(const matrix4x4& m);
	Unit3DData*								GetDataPtr();
	//
	VOID									Play(){m_bIsPlaying = true; if(m_activePtr)m_activePtr->m_fKeyFrameIndex = 0;}
	VOID									Pause(BOOL b){m_bIsPause = b;}
	VOID									Stop(){m_bIsPlaying = false; if(m_activePtr)m_activePtr->m_fKeyFrameIndex = 0;}
	BOOL									IsPlaying(){return m_bIsPlaying;}
	BOOL									IsPause(){return m_bIsPause;}
	//
	VOID									SetObjectID(int id);
	INT										GetObjectID();//返回唯一标识符
	BOOL									GetVisible();
	VOID									SetVisible(BOOL b);	//设置显示与否
	//
	BOOL									IsLookAtTarget();
	VOID									SetLookAtTarget(BOOL b);
	//
	Unit3DAction*							SearchActionPtr(const char* szName);
	Unit3DAction*							GetActionPtr(INT nIndex);
	Unit3DAction*							GetActiveByID(INT id);
	INT										GetActionCount();
	Unit3DMesh*								GetChildMesh(INT nIndex);
	Unit3DMesh*								GetChildMeshBy(INT id);
	//
	VOID									SetElementVisible(int nElementID, BOOL b);		//是否模板动作
	BOOL									GetElementVisible(int nElementID);	//设置为模板动作
	//
	VOID									SetElementVisible(const char* name, BOOL b);		//是否模板动作
	BOOL									GetElementVisible(const char* name);	//设置为模板动作
	//
	Unit3DAction*							FindAction(ActionType t);
public:
	//
	MeshCallback*							m_callback;
	Unit3DData*								m_unitPtr;
	AutoPtr<Mesh>							m_geometryPtr;
	int										m_nObjectID;
	char									m_bVisible;
	char									m_bPlayOver;
	char									m_activeActionType;
	short									m_childCount;
	Unit3DMesh*								m_childMeshs;
	short									m_actionCount;
	Unit3DAction*							m_actionList;
	//
	Unit3DAction*							m_activePtr;
	char									m_bIsPlaying;
	char									m_bIsPause;
	char									m_bIsLoopMode;
	matrix4x4								m_localMat;//各种动作合成的矩阵
	matrix4x4								m_worldMat;//世界矩阵
};

extern	BOOL								LoadUnit3D(AutoPtr<Unit3D>& ret, const char* szUnit3D);
extern	BOOL								CreateUnit3D(AutoPtr<Unit3D>& ret, const char* szMeshFile);//根据模型创建3D单位

};

#endif
