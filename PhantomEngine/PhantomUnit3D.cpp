//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUnit3D.h"
#include "PhantomManager.h"

namespace Phantom{

    Unit3DActionData::Unit3DActionData(Unit3DData* u)
	{
		m_unitPtr			=	u;
		m_nTrackKeyCount	=	100;
		m_id				=	0;
		m_fPlaySpeed		=	1.0f;
		m_bIsTemplate		=	false;
		memset_s(m_parameter, 0, sizeof(m_parameter));
		m_actionType		=	ActionType_Idle;
	}

	Unit3DActionData::~Unit3DActionData(void)
	{
		Destroy();
	}

	VOID				Unit3DActionData::SetFloatParam(int index, float f)
	{CPUTime(Unit3DActionData);
		if(index < 0 || index >= MAX_FLOAT_PARAMETER)
			return;
		m_parameter[index]	=	f;
	}

	float				Unit3DActionData::GetFloatParam(int index)	//如果是攻击，则被击时间差
	{CPUTime(Unit3DActionData);
		if(index < 0 || index >= MAX_FLOAT_PARAMETER)
			return 0;
		return m_parameter[index];
	}

	VOID				Unit3DActionData::CloneFrom(Unit3DActionData* proj)
	{CPUTime(Unit3DActionData);
		Destroy();
		Unit3DActionData* p	=	static_cast<Unit3DActionData*>(proj);
		if(!p)
			return;
		m_name		=	p->m_name;
		memcpy(m_parameter, p->m_parameter, sizeof(m_parameter));
		m_fPlaySpeed	=	p->m_fPlaySpeed;
		m_bIsTemplate	=	p->m_bIsTemplate;
		m_actionType	=	p->m_actionType;
		this->SetTrackKeyCount(p->m_nTrackKeyCount);
		for(int i=0;i<p->m_elements.size();i++)
		{
			CActionElement* ele	=	this->CreateElement((ElementType)p->m_elements[i]->GetElementType());
			ele->CloneFrom(p->m_elements[i]);
			m_elements.push_back(ele);
		}
	}
	VOID				Unit3DActionData::Save(CSafeFileHelperW& w)
	{CPUTime(Unit3DActionData);
		char version = 1;
		w << version;
		skip_w sw(w.pStream);
		w.str(m_name);
		w << m_id << m_nTrackKeyCount << m_fPlaySpeed;
		w << m_bIsTemplate;
		char param = MAX_FLOAT_PARAMETER;
		w << param;
		for(int i=0;i<param;i++)
			w << m_parameter[i];
		w << m_actionType;
		int elements = this->m_elements.size();
		w << elements;
		for(int i=0;i<elements;i++)
		{
			int type = m_elements[i]->GetElementType();
			w << type;
			CFileHelperW fw(w.pStream);
			m_elements[i]->Save(fw);
		}
		sw.end();
	}
	VOID				Unit3DActionData::Load(CSafeFileHelperR& r)
	{CPUTime(Unit3DActionData);
		char version = 1;
		r >> version;
		skip_r sr;
		sr.begin(r.pStream);
		r.str(m_name);
		r >> m_id >> m_nTrackKeyCount >> m_fPlaySpeed;
		r >> m_bIsTemplate;
		char param = MAX_FLOAT_PARAMETER;
		r >> param;
		for(int i=0;i<param;i++)
		{
			if(i<MAX_FLOAT_PARAMETER)
				r >> m_parameter[i];
			else
			{
				float ftemp;
				r >> ftemp;
			}
		}
		r >> m_actionType;
		int elements;
		r >> elements;
		for(int i=0;i<elements;i++)
		{
			int type;
			r >> type;
			CActionElement* ele	=	this->CreateElement((ElementType)type);
			CFileHelperR fr(r.pStream);
			ele->Load(fr);
		}
		sr.end();
	}

	VOID				Unit3DActionData::Destroy()
	{CPUTime(Unit3DActionData);
		for(int i=0;i<m_elements.size();i++)
		{
			m_elements[i]->m_actionPtr	=	0;
			delete m_elements[i];
		}
		m_elements.clear();
	}
	VOID				Unit3DActionData::SetActionType(ActionType t)
	{CPUTime(Unit3DActionData);
		m_actionType = t;
		if(this->m_unitPtr)
			m_unitPtr->RebuildUnitActions();
	}
	INT					Unit3DActionData::GetElementCount()	//返回特效元素数量
	{CPUTime(Unit3DActionData);
		return (int)this->m_elements.size();
	}

	CActionElement*		Unit3DActionData::GetElementPtrByID(INT nID)	//返回特效元素
	{CPUTime(Unit3DActionData);
		for(int i=0;i<m_elements.size();i++)
		{
			if(m_elements[i]->m_id == nID)
				return m_elements[i];
		}
		return 0;
	}

	CActionElement*		Unit3DActionData::GetElementPtr(INT nIndex)	//返回特效元素
	{CPUTime(Unit3DActionData);
		if(nIndex < 0 || nIndex >= (int)m_elements.size())
			return 0;
		return m_elements[nIndex];
	}

	VOID				Unit3DActionData::DeleteElement(CActionElement* ele)	//从文件创建一个元素
	{CPUTime(Unit3DActionData);
		for(int i=0;i<m_elements.size();i++)
		{
			if(m_elements[i] == ele)
			{
				m_elements[i]->m_actionPtr	=	0;
				delete m_elements[i];
				m_elements.erase(m_elements.begin() + i);
				break;
			}
		}
	}

	int	SortActionElements(const void* v1, const void* v2)
	{
		CActionElement* p1	=	*(CActionElement**)v1;
		CActionElement* p2	=	*(CActionElement**)v2;
		if(p1->GetElementType() < p2->GetElementType())
			return -1;
		else if(p1->GetElementType() > p2->GetElementType())
			return 1;
		return stricmp(p1->GetName(), p2->GetName());
	}

	CActionElement*		Unit3DActionData::CreateElement(ElementType type)	//从文件创建一个元素
	{CPUTime(Unit3DActionData);
		CActionElement* elementPtr = 0;
		char name[128];
		switch(type)
		{
		case ElementType_Mesh:
			elementPtr	=	new CElementMeshControl(this);
			strcpy(name, "mesh");
			break;
		case ElementType_Emission:
			elementPtr	=	new CElementEmission(this);
			strcpy(name, "emit");
			break;
		case ElementType_Sound:
			elementPtr	=	new CElementMusic3D(this);
			strcpy(name, "sound");
			break;
		//case ElementType_Action:
		//	elementPtr	=	new CElementMeshControl(this);
		//	strcpy(name, "action");
		//	break;
		//case ElementType_ChildMesh:
		//	elementPtr	=	new CElementMeshControl(this);
		//	strcpy(name, "child_mesh");
		//	break;
		default:
			return 0;
		}
		elementPtr->m_id	=	m_unitPtr->GetNewIdentity();
		sprintf(elementPtr->m_name.t, "%s.%d", name, elementPtr->m_id);
		elementPtr->SetFrameCount(m_nTrackKeyCount);
		this->m_elements.push_back(elementPtr);
		return elementPtr;
	}

	VOID				Unit3DActionData::SortElements()
	{CPUTime(Unit3DActionData);
		if(m_elements.size() > 1)
		{
			qsort(&m_elements.at(0), m_elements.size(), sizeof(CActionElement*), SortActionElements);
		}
	}

	VOID				Unit3DActionData::SetTrackKeyCount(INT nFrameCount)	//关键帧数量
	{CPUTime(Unit3DActionData);
		if(nFrameCount <= 0)
			return;
		m_nTrackKeyCount	=	getmax(nFrameCount, 5);
		for(int i=0;i<m_elements.size();i++)
		{
			if(m_elements[i]->m_trackBeginIndex >= m_nTrackKeyCount)
				m_elements[i]->m_trackBeginIndex	=	m_nTrackKeyCount - 1;
			if(m_elements[i]->m_trackEndIndex >= m_nTrackKeyCount)
				m_elements[i]->m_trackEndIndex	=	m_nTrackKeyCount - 1;
		}
	}

	INT					Unit3DActionData::GetTrackKeyCount()
	{CPUTime(Unit3DActionData);
		return m_nTrackKeyCount;
	}

	Unit3DMeshData::Unit3DMeshData(Unit3DData* u)
	{
		m_nMeshSets	=	1;
		m_id		=	0;
		m_unitPtr	=	u;
	}
	Unit3DMeshData::~Unit3DMeshData()
	{
	}
	VOID		Unit3DMeshData::Save(CSafeFileHelperW& w)
	{CPUTime(Unit3DMeshData);
		char version = 1;
		w << version;
		skip_w sw(w.pStream);
		w << m_id;
		w.str(m_fileName);
		w.str(m_name);
		sw.end();
	}
	VOID		Unit3DMeshData::Load(CSafeFileHelperR& r)
	{CPUTime(Unit3DMeshData);
		char version = 1;
		r >> version;
		skip_r sr;
		sr.begin(r.pStream);
		r >> m_id;
		r.str(m_fileName);
		r.str(m_name);
		sr.end();
		m_nMeshSets++;
	}

	std::vector<Unit3DData*>	g_unit3Ds;
	Unit3DData::Unit3DData()
	{
		m_id				=	1;
		m_dlgFileChanged	=	1;
		m_bIsPickEnable		=	false;
		m_fUnitTopHeight	=	.76f;
		m_bStaticUnit		=	true;
		g_unit3Ds.push_back(this);
		memset_s(m_unitActions, 0, sizeof(m_unitActions));
	}
	Unit3DData::~Unit3DData()
	{
		for(int i=0;i<this->m_actions.size();i++)
		{
			safe_delete(m_actions[i]);
		}
		m_actions.clear();
		for(int i=0;i<g_unit3Ds.size();i++)
		{
			if(g_unit3Ds[i]	==	this)
			{
				g_unit3Ds.erase(g_unit3Ds.begin() + i);
				break;
			}
		}
	}
	
	Unit3DMeshData*						Unit3DData::CreateChildMesh()
	{CPUTime(Unit3DData);
		Unit3DMeshData* m = new Unit3DMeshData(this);
		m->m_id	=	this->GetNewIdentity();
		m_childMeshs.push_back(m);
		return m;
	}
	VOID								Unit3DData::DeleteChildMesh(Unit3DMeshData* m)
	{CPUTime(Unit3DData);
		for(int i=0;i<m_childMeshs.size();i++)
		{
			if(m_childMeshs[i] == m){
				m_childMeshs.eraseByIndex(i);
				delete m;
				break;
			}
		}
	}

	Unit3DMeshData*						Unit3DData::GetChildMesh(INT nIndex)
	{CPUTime(Unit3DData);
		if(nIndex < 0 || nIndex >= m_childMeshs.size())
			return 0;
		return m_childMeshs[nIndex];
	}
	Unit3DActionData*					Unit3DData::FindByID(int id)
	{CPUTime(Unit3DData);
		for(int i=0;i<m_actions.size();i++)
			if(m_actions[i]->m_id == id)
				return m_actions[i];
		return 0;
	}
	VOID								Unit3DData::SetPickEnable(BOOL b)	//是否可以被点击
	{CPUTime(Unit3DData);
		m_bIsPickEnable	=	b;
	}
	BOOL								Unit3DData::IsPickEnable()
	{CPUTime(Unit3DData);
		return m_bIsPickEnable;
	}

	VOID								Unit3DData::RebuildUnitActions()
	{CPUTime(Unit3DData);
		memset_s(m_unitActions, 0, sizeof(m_unitActions));
		for(int i=0;i<this->m_actions.size();i++)
		{
			m_unitActions[m_actions[i]->GetActionType()] = m_actions[i];
		}
	}

	const char*							Unit3DData::GetGeometryFile()
	{CPUTime(Unit3DData);
		return m_geometryFileName;
	}
	VOID								Unit3DData::CreateRender(AutoPtr<Unit3D>& ret)
	{CPUTime(Unit3DData);
		Unit3D* u = new Unit3D(this);
		ret = u;
		u->Release();
	}

	VOID								Unit3DData::SetGeometryFile(const char* szName)
	{CPUTime(Unit3DData);
		m_geometryFileName	=	szName;
	}
	//
	BOOL								Unit3DData::SaveUnit(const char* szFile)
	{CPUTime(Unit3DData);
		m_fileName = szFile;
		CSafeFileHelperW w;
		if(!w.create(szFile))
			return false;
		int fileHead = _MakeFileHead('u','n','i','t');
		int version = 1;
		w << fileHead << version;
		w << m_id;
		w << m_bStaticUnit;
		w << m_fUnitTopHeight;
		w.str(m_geometryFileName);
		short count = m_childMeshs.size();
		w << count;
		for(int i=0;i<count;i++){
			m_childMeshs[i]->Save(w);
		}
		short actions = m_actions.size();
		w << actions;
		for(int i=0;i<actions;i++){
			m_actions[i]->Save(w);
		}
		return true;
	}
	BOOL								Unit3DData::LoadUnit(const char* szFile)
	{CPUTime(Unit3DData);
		m_fileName	=	szFile;
		m_fileName.setIsFile();
		//
		CSafeFileHelperR r;
		if(!r.open(m_fileName))
			return false;
		//
		int fileHead;
		int version = 1;
		r >> fileHead >> version;
		if(fileHead != _MakeFileHead('u','n','i','t'))
			return false;
		r >> m_id;
		r >> m_bStaticUnit;
		r >> m_fUnitTopHeight;
		r.str(m_geometryFileName);
		short count = 0;
		r >> count;
		if(count>0){
			m_childMeshs.resize(count);
			for(int i=0;i<count;i++)
			{
				m_childMeshs[i] = new Unit3DMeshData(this);
				m_childMeshs[i]->Load(r);
			}
		}
		short actions;
		r >> actions;
		if(actions)
			m_actions.resize(actions);
		for(int i=0;i<actions;i++){
			m_actions[i] = new Unit3DActionData(this);
			m_actions[i]->Load(r);
		}
		RebuildUnitActions();
		LogInfo("sytem->load rpgunit %s", m_fileName.t);
		return true;
	}
	BOOL								Unit3DData::DeleteAction(Unit3DActionData* a)
	{CPUTime(Unit3DData);
		for(int i=0;i<m_actions.size();i++)
		{
			if(m_actions[i] == a)
			{
				delete a;
				m_actions.eraseByIndex(i);
				break;
			}
		}
		RebuildUnitActions();
		return true;
	}
	Unit3DActionData*					Unit3DData::CreateAction(int type)
	{CPUTime(Unit3DData);
		Unit3DActionData* aff	=	new Unit3DActionData(this);
		aff->m_id	=	(m_id++);
		sprintf(aff->m_name.t, "proj.%d", aff->m_id);
		if(m_actions.size() == 0)
			aff->m_actionType	=	ActionType_Idle;
		this->m_actions.push_back(aff);
		return aff;
	}
	BOOL			CreateUnit3D(AutoPtr<Unit3D>& ret, const char* szMeshFile)//根据模型创建3D单位
	{
		FileName file	=	szMeshFile;
		FileName dir,f;
		file.splitpath(&dir, &f, 0);
		dir += f;
		dir += ".rpgunit";
		file = dir;
		file.setIsFile();
		for(int i=0;i<g_unit3Ds.size();i++)
		{
			if(g_unit3Ds[i]->m_fileName.isFile(file))
			{
				Unit3D* unit	=	new Unit3D(g_unit3Ds[i]);
				ret	=	unit;
				safe_release(unit);
				return true;
			}
		}
		Unit3DData* ef = new Unit3DData();
		ef->SetGeometryFile(szMeshFile);
		ef->m_fileName = file.t;
		//if(!ef->LoadUnit(szUnit3D))
		//{
		//	delete ef;
		//	return false;
		//}
		Unit3D* r	=	new Unit3D(ef);
		ret	=	r;
		safe_release(ef);
		safe_release(r);
		return true;
	}
	BOOL			LoadUnit3D(AutoPtr<Unit3D>& ret, const char* szUnit3D)
	{
		FileName file	=	szUnit3D;
		file.setIsFile();
		for(int i=0;i<g_unit3Ds.size();i++)
		{
			if(g_unit3Ds[i]->m_fileName.isFile(file))
			{
				Unit3D* unit	=	new Unit3D(g_unit3Ds[i]);
				ret	=	unit;
				safe_release(unit);
				return true;
			}
		}
		Unit3DData* ef = new Unit3DData();
		if(!ef->LoadUnit(szUnit3D))
		{
			delete ef;
			return false;
		}
		Unit3D* r	=	new Unit3D(ef);
		ret	=	r;
		safe_release(ef);
		safe_release(r);
		return true;
	}

	Unit3DAction::Unit3DAction()
	{
		m_fPlaySpeed		=	1.0f;
		m_fKeyFrameIndex	=	0;
		//m_lastOffset.setxyz(0,0,0);
	}
	VOID				Unit3DAction::Init(Unit3D* unitPtr, Unit3DActionData* act){CPUTime(Unit3DAction);
		m_unitPtr			=	unitPtr;
		m_actionPtr			=	act;
		CDynamicArray<CElementRuntime*> temp;
		for(int i=0;i<act->m_elements.size();i++)
		{
			CElementRuntime* e	=	0;
			switch(act->m_elements[i]->GetElementType())
			{
			case ElementType_Mesh:
				e	=	new CElementMeshRuntime(static_cast<CElementMeshControl*>(act->m_elements[i]), this);
				break;
			case ElementType_Emission:
				e	=	new CElementEmissionRuntime(static_cast<CElementEmission*>(act->m_elements[i]), this);
				break;
			case ElementType_Sound:
				e	=	new CElementMusic3DRuntime(static_cast<CElementMusic3D*>(act->m_elements[i]), this);
				break;
			//case ElementType_Action:
			//	e	=	new CElementMeshRuntime(static_cast<CElementMeshControl*>(act->m_elements[i]), this);
			//	break;
			//case ElementType_ChildMesh:
			//	e	=	new CElementMeshRuntime(static_cast<CElementMeshControl*>(act->m_elements[i]), this);
			//	break;
			default:
				continue;
			}
			temp.push_back(e);
		}
		if(temp.size()>0){
			m_elements.resize(temp.size());
			for(int i=0;i<temp.size();i++)
				m_elements[i] = temp[i];
		}
	}
	VOID				Unit3DAction::StopAction()
	{CPUTime(Unit3DAction);
		//m_lastOffset	=	float3(0, 0, 0);
		for(int i=0;i<m_elements.size();i++)
			m_elements[i]->StopAction();
	}

	VOID				Unit3DAction::BeginAction()
	{CPUTime(Unit3DAction);
		//m_lastOffset	=	float3(0, 0, 0);
		for(int i=0;i<m_elements.size();i++)
			m_elements[i]->BeginAction();
	}

	Unit3DAction::~Unit3DAction()
	{
		for(int i=0;i<m_elements.size();i++)
			delete m_elements[i];
	}

	VOID				Unit3DAction::BeforeFrameMove(EmissionFrameMove& fm, float fTrackIndex)
	{CPUTime(Unit3DAction);
		//EmissionFrameMove fm;
		int numAnimation	=	0;
		if(m_unitPtr->m_geometryPtr && m_unitPtr->m_geometryPtr->GetCurrentAction())
			numAnimation	=	m_unitPtr->m_geometryPtr->GetCurrentAction()->GetKeyCount();
		for(int i=0;i<m_elements.size();i++)
			m_elements[i]->BeforeFrameMove(fm, m_unitPtr->m_geometryPtr, fTrackIndex);
	}
/*
	VOID				Unit3DAction::FrameMoveEmision(Mesh* basePtr, const matrix4x4& actionMat)
	{CPUTime(Unit3DAction);
		for(int i=0;i<m_elements.size();i++)
			m_elements[i]->FrameMoveEmision(basePtr, actionMat);
	}*/

	//VOID				Unit3DAction::PushToRenderEmission()
	//{
	//	for(int i=0;i<m_elements.size();i++)
	//		m_elements[i]->PushToRenderEmission();
	//}

	VOID				Unit3DAction::FrameMove(Mesh* basePtr, float fTrackIndex, const matrix4x4& worldMat)
	{CPUTime(Unit3DAction);
		for(int i=0;i<m_elements.size();i++)
			m_elements[i]->FrameMove(basePtr, worldMat);
	}

	VOID				Unit3DAction::OnRender()
	{CPUTime(Unit3DAction);
		for(int i=0;i<m_elements.size();i++)
			m_elements[i]->OnRender();
	}

	CElementRuntime*		Unit3DAction::SearchElement(const char* szElementName)	//模型指针
	{CPUTime(Unit3DAction);
		for(int i=0;i<m_elements.size();i++)
		{
			if(m_elements[i]->m_elementPtr->m_name == szElementName)
				return m_elements[i];
		}
		return 0;
	}
	CElementRuntime*		Unit3DAction::GetElementByID(INT nID)	//模型指针
	{CPUTime(Unit3DAction);
		for(int i=0;i<m_elements.size();i++)
		{
			if(m_elements[i]->m_elementPtr->GetID() == nID)
				return m_elements[i];
		}
		return 0;
	}
	CElementRuntime*		Unit3DAction::GetElementByIndex(INT nIndex)	//模型指针
	{CPUTime(Unit3DAction);
		if(nIndex < 0 || nIndex >= m_elements.size())
			return 0;
		return m_elements[nIndex];
	}
	INT							Unit3DAction::GetElementCount()
	{CPUTime(Unit3DAction);
		return m_elements.size();
	}
	VOID						Unit3DAction::SetActive(BOOL b)//激活
	{CPUTime(Unit3DAction);
		m_unitPtr->SetActiveAction(b ? this : 0);
	}
	BOOL						Unit3DAction::IsActive()
	{CPUTime(Unit3DAction);
		return (m_unitPtr->m_activePtr == this);
	}

	Unit3DMesh::Unit3DMesh()
	{
		m_geometryPtr	=	0;
		m_nMeshSets	=	-1;
		m_bIsFrameMove	=	false;
		m_meshPtr		=	0;
	}
	VOID			Unit3DMesh::Setup(Unit3DMeshData* meshPtr)
	{CPUTime(Unit3DMesh);
		m_meshPtr	=	meshPtr;
		Mesh* old	=	m_geometryPtr;
		if(m_meshPtr->m_fileName.t[0])
		{
			FileName geom;
			m_unitPtr->m_unitPtr->GetFileExist(geom, m_meshPtr->m_fileName);
			if(fexist(geom))
			{
				if(g_manager.LoadMesh(m_geometryPtr, geom))
				{
				}
			}
		}
		else
			m_geometryPtr	=	0;
		safe_release(old);
	}
	Unit3DMesh::~Unit3DMesh()
	{
		m_geometryPtr	=	0;
	}
	Mesh*				Unit3DMesh::GetMeshPtr()
	{CPUTime(Unit3DMesh);
		//if(!m_geometryPtr)
		//	return 0;
		//if(this->m_nPartSets != m_meshPtr->m_nPartSets)
		//{
		//	memcpy(m_nPartIndex, m_meshPtr->m_nPartIndex, sizeof(m_nPartIndex));
		//	m_nPartSets	=	m_meshPtr->m_nPartSets;
		//	//
		//	for(int i=0;i<m_geometryPtr->GetSubmeshCount();i++)
		//		m_geometryPtr->GetSubmeshPtr(i)->SetVisible(false);
		//	for(int i=0;i<MAX_ELEMENT_MESH;i++)
		//	{
		//		if(m_nPartIndex[i] >= 0 && m_nPartIndex[i] < m_geometryPtr->GetSubmeshCount())
		//		{
		//			m_geometryPtr->GetSubmeshPtr(m_nPartIndex[i])->SetVisible(true);
		//		}
		//	}
		//}
		return m_geometryPtr;
	}

	Unit3D::Unit3D(Unit3DData* p)
	{
		m_unitPtr			=	p;
		safe_addref(m_unitPtr);
		m_bIsPlaying		=	true;
		m_bIsPause			=	false;
		//m_fUnitScale		=	1.0f;
		m_geometryPtr		=	0;
		m_localMat.identity();
		m_worldMat.identity();
		m_activePtr			=	0;
		//m_bRenderTopPushed	=	false;
		//m_targetPos			=	float3(0, 0, 0);
		//m_fromPos			=	float3(0, 0, 0);
		m_bPlayOver			=	false;
		m_bIsLoopMode		=	false;
		m_callback			=	0;
		//
		m_nObjectID			=	0;
		m_bVisible			=	true;
		m_activeActionType	=	ActionType_None;
		this->m_childCount	=	m_unitPtr->m_childMeshs.size();
		m_childMeshs		=	0;
		if(m_childMeshs){
			m_childMeshs		=	new Unit3DMesh[m_childCount];
			for(int i=0;i<m_childCount;i++)
				m_childMeshs[i].Setup(m_unitPtr->m_childMeshs[i]);
		}
		m_actionCount		=	m_unitPtr->m_actions.size();
		m_activePtr			=	0;
		FileName geomFile;
		if(m_unitPtr->GetFileExist(geomFile, m_unitPtr->m_geometryFileName.t))
			g_manager.LoadMesh(m_geometryPtr, geomFile);
		m_actionList		=	0;
		if(m_actionCount>0){
			this->m_actionList = new Unit3DAction[m_actionCount];
			for(int i=0;i<m_actionCount;i++)
				m_actionList[i].Init(this, m_unitPtr->m_actions[i]);
		}
	}
	BOOL			Unit3DData::GetFileExist(FileName& ret, const char* in)
	{CPUTime(Unit3DData);
		ret	=	in;
        ret.setIsFile();
		if(!fexist(ret))
		{
			FileName path, file, ext;
			m_fileName.splitpath(&path, 0, 0);
			ret.splitpath(0, &file, &ext);
			path	+=	file;
			path	+=	ext;
			if(fexist(path))
			{
				ret	=	path.t;
				return true;
			}
			return false;
		}
		return true;
	}

	Unit3D::~Unit3D()
	{
		if(m_childMeshs)
			delete[] m_childMeshs;
		m_childCount	=	0;
		m_geometryPtr	=	0;
		if(m_actionList)
			delete[] m_actionList;
		m_actionList = 0;
		m_actionCount = 0;
		safe_release(m_unitPtr);
	}
	float									Unit3D::GetKeyFrameCount()
	{CPUTime(Unit3D);
		if(this->m_activePtr)
			return (float)m_activePtr->m_actionPtr->m_nTrackKeyCount;
		return 0;
	}
	float									Unit3D::GetKeyFrame()
	{CPUTime(Unit3D);
		if(m_activePtr)
			return m_activePtr->m_fKeyFrameIndex;
		return 0.0f;
	}
	VOID									Unit3D::SetKeyFrame(float f)
	{CPUTime(Unit3D);
		if(m_activePtr)
			m_activePtr->m_fKeyFrameIndex	=	f;
	}
	Unit3DData*								Unit3D::GetDataPtr(){CPUTime(Unit3D);return m_unitPtr;}
	VOID									Unit3D::SetActiveAction(PtrID& id)
	{CPUTime(Unit3D);
		if(id.id==0)return;
		if(id.index>=m_unitPtr->m_actions.size() || m_unitPtr->m_actions[id.index]->m_id != id.id){
			id.index = 0;
			id.id = 0;
			for(int i=0;i<m_unitPtr->m_actions.size();i++)
			{
				if(m_unitPtr->m_actions[i]->m_id == id.id){
					id.index = i;
					id.id = m_unitPtr->m_actions[i]->m_id;
					break;
				}
			}
			if(id.id==0)
			{
				return;
			}
		}
		SetActiveActionProj(m_unitPtr->m_actions[id.index]);
	}
	Unit3DAction*							Unit3D::GetActiveAction()
	{CPUTime(Unit3D);
		return m_activePtr;
	}

	Unit3DAction*					Unit3D::FindAction(ActionType t)
	{CPUTime(Unit3D);
		for(int i=0;i<m_actionCount;i++)
		{
			if(m_actionList[i].m_actionPtr->GetActionType() == t)
				return &m_actionList[i];
		}
		return 0;
	}

	VOID									Unit3D::SetActiveAction(Unit3DAction* act)	//设置播放完毕之后下一个动作ID，如果不设置则自动引用Unit3DActionData的设置
	{CPUTime(Unit3D);
		//m_delayActionType	=	ActionType_None;
		if(act)
			SetActiveActionProj(act->GetActionPtr());
		else
			SetActiveActionProj(0);
	}

	VOID									Unit3D::SetActiveActionProj(Unit3DActionData* act)
	{CPUTime(Unit3D);
		Unit3DAction* lastActive	=	m_activePtr;
		m_activePtr	=	0;
		if(act)
		{
			for(int i=0;i<m_actionCount;i++)
			{
				if(m_actionList[i].m_actionPtr == act)
				{
					m_activePtr			=	&m_actionList[i];
					break;
				}
			}
		}
		if(!m_activePtr)
		{
			if(!m_activePtr)
				m_activePtr	=	FindAction(ActionType_Idle);
		}
		if(lastActive)
			lastActive->StopAction();
		if(m_activePtr)
		{
			m_activeActionType	=	m_activePtr->m_actionPtr->GetActionType();
			//if(this->m_bSwimEnable && m_bIsSwiming)
			//{
			//	Unit3DAction* act	=	m_activePtr;
			//	if(m_activePtr->m_actionPtr->GetActionType() == ActionType_Idle)
			//		act	=	FindAction(ActionType_SwimStandard);
			//	else if(m_activePtr->m_actionPtr->GetActionType() == ActionType_Standard2)
			//		act	=	FindAction(ActionType_SwimStandard2);
			//	else if(m_activePtr->m_actionPtr->GetActionType() == ActionType_Walk || m_activePtr->m_actionPtr->GetActionType() == ActionType_Run)
			//		act	=	FindAction(ActionType_Swim);
			//	else if(m_activePtr->m_actionPtr->GetActionType() == ActionType_Move2 || m_activePtr->m_actionPtr->GetActionType() == ActionType_Run2)
			//		act	=	FindAction(ActionType_Swim2);
			//	if(act)
			//		m_activePtr	=	act;
			//}
//			m_activeID.id		=	m_activePtr->m_actionPtr->m_id;
			m_activePtr->BeginAction();
//			m_nextActionID.id	=	m_activePtr->m_actionPtr->m_nextActionID;
			//if(m_nextActionID.id == 0 && lastActive)
			//	m_nextActionID.id	=	lastActive->GetID();
			//
			m_activePtr->m_fKeyFrameIndex	=	0;
			for(int i=0;i<m_actionCount;i++)
			{
				if(&m_actionList[i] != m_activePtr && m_actionList[i].m_actionPtr->m_bIsTemplate)
				{
					for(int k=0;k<m_actionList[i].m_elements.size();k++)
					{
						m_actionList[i].m_elements[k]->CheckChange();
						if(m_actionList[i].m_elements[k]->m_bTemplateVisible)
						{
							m_actionList[i].m_fKeyFrameIndex	=	0.0f;
							m_actionList[i].BeginAction();
						}
					}
				}
			}
		}
		m_bPlayOver				=	false;
	}

	BOOL								Unit3D::SetActiveAction(ActionType t, float fDelayTime)	//设置播放完毕之后下一个动作ID，如果不设置则自动引用Unit3DActionData的设置
	{CPUTime(Unit3D);
		if(t < 0 || t > ActionType_Count)
			return false;
		//if(m_activePtr)
			//if(m_activePtr->m_actionPtr == m_unitPtr->m_unitActions[t])
				//return false;
		//m_delayActionTime	=	fDelayTime;
		//m_delayActionType	=	t;
		//if(m_delayActionTime <= 0.001f)
		//{
		//	m_delayActionType	=	ActionType_None;
		//	SetActiveActionProj(m_unitPtr->m_unitActions[t]);
		//}
		return true;
	}

	VOID								Unit3D::Render()
	{CPUTime(Unit3D);
		if(!m_activePtr || !m_bVisible || m_activePtr->m_actionPtr->m_actionType==ActionType_Hide)
			return;
		if(m_geometryPtr && m_geometryPtr->GetDiffuseColor().a > 0.001f)
		{
			m_geometryPtr->Render();
		}
		for(int i=0;i<m_childCount;i++)
		{
			if(m_childMeshs[i].m_bIsFrameMove)
			{
				if(m_childMeshs[i].m_geometryPtr)
					m_childMeshs[i].m_geometryPtr->Render();
			}
		}
		if(m_activePtr)
			m_activePtr->OnRender();
		//粒子系统继续计算
		for(int i=0;i<m_actionCount;i++)
		{
			if(&m_actionList[i] != m_activePtr && m_actionList[i].m_actionPtr->m_bIsTemplate)
			{
				for(int k=0;k<m_actionList[i].m_elements.size();k++)
				{
					if(m_actionList[i].m_elements[k]->m_bTemplateVisible)
						m_actionList[i].m_elements[k]->OnRender();
				}
			}
			//m_actions[i]->PushToRenderEmission();
		}
		//if(m_geometryPtr && m_scenePtr && !m_unitPtr->m_bStaticUnit)
		//{
		//	float3 position	=	m_world.GetPosition();
		//	float3 d	=	GetCameraPtr()->GetEye() - position;
		//	float fLength	=	d.length();
		//	if(fLength < m_scenePtr->GetUnitTopVisibleLength())
		//	{
		//		if(m_scenePtr->IsUnitNameVisible() && GetText3DWriterL() && m_bNameVisible && m_name.t[0])
		//		{

		//			const PObjectRegion& region = this->m_geometryPtr->GetObjectRegion();
		//			position.z		+=	(region.aabox._max.z - region.aabox._min.z) * m_unitPtr->m_fUnitTopHeight + 3.0f + fLength / 25.0f;// * ((1.0f - fR) * 3.0f + 1.0f);
		//			GetText3DWriterL()->Render(position, 0, m_name, m_nameColor, m_scenePtr->GetUnitNameSize());//m_unitName, m_unitNameColor);
		//		}
		//		if(m_scenePtr->IsUnitBloodVisible() && this->m_bBloodVisible && this->m_fBloodValue > 0.0f)
		//		{
		//			GetRenderQueuePtr()->PushRenderObject( RenderQueue_AlphaBlend, &m_world, this, m_renderData, 0, 0, 0);
		//		}
		//	}
		//}
	}
	//
	//struct	PVertexBlood
	//{
	//	union{
	//		struct{
	//			float3						p;
	//			float3						n;
	//			unsigned int				diffuse;
	//			float2						t;
	//			float2						t2;
	//		};
	//		struct{
	//			float						x,y,z,nx,ny,nz;
	//			unsigned int				dwColor;
	//			float						tu,tv;
	//			float						tu2,tv2;
	//		};
	//	};
	//	static	const unsigned int			c_fvf	=	Phantom::FVF_XYZ|Phantom::FVF_NORMAL|Phantom::FVF_DIFFUSE|Phantom::FVF_TEX2;
	//	//
	//};
	//
	//VOID						RenderBlood(Program* shader, Texture* tex, const float3& pos, float fSize, unsigned int color)
	//{
	//	PVertexBlood				vertexs[4];
	//	Triangle					faces[2];
	//	faces[0].set(0, 1, 2);
	//	faces[1].set(1, 3, 2);
	//	//
	//	float	nTexWidth		=	(float)tex->GetWidth();
	//	float	nTexHeight		=	(float)tex->GetHeight();
	//	float	fWidth		=	fSize * 0.5f;
	//	float fHeight		=	fWidth * ((float)nTexHeight / (float)nTexWidth);// * 2.0f;
	//	vertexs[3].p.setxyz(0.0f, -fWidth, 0);
	//	vertexs[2].p.setxyz(0.0f, -fWidth, fHeight);
	//	vertexs[1].p.setxyz(0.0f, fWidth, 0);
	//	vertexs[0].p.setxyz(0.0f, fWidth, fHeight);
	//	for(int i=0;i<4;i++)
	//		vertexs[i].diffuse	=	color;
	//	//
	//	{
	//		Rect rect(0, 0, nTexWidth, nTexHeight / 2);
	//		float fTextureW	=	(float)nTexWidth;
	//		float fTextureH	=	(float)nTexHeight;
	//		float u1		=	(float)rect.left	/ (float)fTextureW;
	//		float v1		=	(float)rect.top	/ (float)fTextureH;
	//		float u2		=	(float)rect.right / (float)fTextureW;
	//		float v2		=	(float)rect.bottom / (float)fTextureH;
	//		vertexs[3].tu		= u1;
	//		vertexs[3].tv		= v2;
	//		//
	//		vertexs[2].tu		= u1;
	//		vertexs[2].tv		= v1;
	//		//
	//		vertexs[1].tu		= u2;
	//		vertexs[1].tv		= v2;
	//		//
	//		vertexs[0].tu		= u2;
	//		vertexs[0].tv		= v1;
	//	}
	//	//
	//	{
	//		Rect rect(0, nTexHeight / 2, nTexWidth, nTexHeight);
	//		float fTextureW	=	(float)nTexWidth;
	//		float fTextureH	=	(float)nTexHeight;
	//		float u1		=	(float)rect.left	/ (float)fTextureW;
	//		float v1		=	(float)rect.top	/ (float)fTextureH;
	//		float u2		=	(float)rect.right / (float)fTextureW;
	//		float v2		=	(float)rect.bottom / (float)fTextureH;
	//		vertexs[3].tu2		= u1;
	//		vertexs[3].tv2		= v2;
	//		//
	//		vertexs[2].tu2		= u1;
	//		vertexs[2].tv2		= v1;
	//		//
	//		vertexs[1].tu2		= u2;
	//		vertexs[1].tv2		= v2;
	//		//
	//		vertexs[0].tu2		= u2;
	//		vertexs[0].tv2		= v1;
	//	}
	//	//
	//	for(int i=0;i<4;i++)
	//		vertexs[i].n		=	pos;
	//	shader->SetTexture(0, tex);
	//	shader->commit();
	//	g_manager->DrawIndexed( Phantom::DrawMode_Triangles, vertexs, faces, 4, 2, sizeof(PVertexBlood), PVertexBlood::c_fvf, false );
	//}
	//
	//void								Unit3D::drawObject(const matrix4x4* matPtr, Program* vsps, int nDrawState)
	//{
	//	const matrix4x4* mat	= GetCameraPtr()->GetBoardWorld(0);
	//	if(mat)
	//	{
	//		matrix4x4 m = *mat;
	//		m.SetPosition(0, 0, 0);
	//		vsps->SetWorldMatrix(m);
	//	}
	//	float fSize	=	m_scenePtr->GetUnitBloodSize();
	//	static _shader_var_name bloodValue("m_fBloodProgress");
	//	float3 pos	=	m_world.GetPosition();
	//	const PObjectRegion& region = m_geometryPtr->GetObjectRegion();
	//	pos.z		+=	(region.aabox._max.z - region.aabox._min.z) * (m_unitPtr->GetUnitTopHeight()) + 1.0f;
	//	vsps->SetFloat(bloodValue, this->m_fBloodValue, bloodValue.name);
	//	//
	//	const TextureInfo3D* bloodInfo	=	m_scenePtr->GetBloodTextureInfo(BloodTextureInfo_Blood);
	//	if(bloodInfo->texture)
	//		RenderBlood(vsps, bloodInfo->texture, pos, fSize, this->m_bloodColor);
	//}
	//const float3*						Unit3D::GetPosition()
	//{
	//	return &m_world.GetPosition();
	//}					//取得物体位置
	VOID								Unit3D::SetObjectID(int id)
	{CPUTime(Unit3D);
		this->m_nObjectID	=	id;
	}
	INT									Unit3D::GetObjectID()
	{CPUTime(Unit3D);
		return this->m_nObjectID;
	}//返回唯一标识符
	BOOL								Unit3D::GetVisible()
	{CPUTime(Unit3D);
		return this->m_bVisible;
	}
	VOID								Unit3D::SetVisible(BOOL b)
	{CPUTime(Unit3D);
		this->m_bVisible	=	b;
	}	//设置显示与否
	//VOID								Unit3D::ResetMoveAction()
	//{
	//	ActionType type	=	ActionType_Run;
	//	switch(m_moveMode)
	//	{
	//	case UnitMoveMode_Move:
	//		type	=	ActionType_Walk;
	//		break;
	//	case UnitMoveMode_Run:
	//		type	=	ActionType_Run;
	//		break;
	//	case UnitMoveMode_Fly:
	//		type	=	ActionType_Fly;
	//		break;
	//	}
	//	if(!m_unitPtr->m_unitActions[type])
	//		type	=	ActionType_Run;
	//	SetActiveAction(type);
	//}
	//VOID								Unit3D::RecalcHeight(BOOL bCollision)
	//{
	//	char bIsSwiming	=	false;
	//	if(!bIsSwiming)
	//	{
	//		if(m_bIsSwiming)
	//		{
	//			m_bIsSwiming	=	false;
	//			this->SetActiveAction((ActionType)m_activeActionType);
	//		}
	//	}
	//}

	//float								Unit3D::GetUnitScale()
	//{
	////	return m_fUnitScale;
	//}

	//VOID								Unit3D::RebuildWorld()
	//{
	//	//float3 pos	=	m_world.GetPosition();
	//	//float fScale	=	m_baseScale * this->m_unitPtr->m_fBaseScale;
	//	//matrix4x4 mat, matS;
	//	//matrixScaling(&matS, fScale, fScale, fScale);
	//	//matrixRotationZ(&mat, m_rotationZ);
	//	//matrixMultiply(&m_world, &matS, &mat);
	//	////pos.z	+=	m_fOffsetZ;
	//	//m_world.SetPosition(pos);
	//	//this->RecalcHeight(true);
	//}
	//BOOL								Unit3D::IsLookAtTarget()
	//{
	//	return this->m_bIsLookAtTarget;
	//}
	//VOID								Unit3D::SetLookAtTarget(BOOL b)
	//{
	//	this->m_bIsLookAtTarget	=	b;
	//}
	//BOOL								Unit3D::IsMoveOver()
	//{
	////	return (this->m_fCurrentMoveLength >= m_fMoveLength);
	//}

	VOID								Unit3D::FrameMove()
	{CPUTime(Unit3D);
		if(!m_activePtr || m_activePtr->m_actionPtr->m_actionType==ActionType_Hide || !m_bVisible)
		{
			if(m_geometryPtr)
				m_geometryPtr->SetVisible(false);
			return;
		}
		if(m_geometryPtr)
			m_geometryPtr->SetVisible(true);
		//
		for(int i=0;i<m_childCount;i++)
			m_childMeshs[i].m_bIsFrameMove	=	false;
		char bSetNextAction	=	false;
		if(m_bIsPlaying && m_activePtr && !m_bIsPause)
		{
			m_activePtr->m_fKeyFrameIndex	+=	g_manager.GetElapsedTime() * m_activePtr->m_fPlaySpeed * m_activePtr->m_actionPtr->m_fPlaySpeed * TRACK_SECOND_LENGTH;
			if(m_activePtr->m_fKeyFrameIndex >= ((float)m_activePtr->m_actionPtr->m_nTrackKeyCount - 1))
			{
				//if(m_bIsLoopMode)
				//{
				//	m_activePtr->m_fKeyFrameIndex	=	fmodf(m_activePtr->m_fKeyFrameIndex, m_activePtr->m_actionPtr->m_nTrackKeyCount);
				//}
				//else
				{
					m_activePtr->m_fKeyFrameIndex	=	(float)m_activePtr->m_actionPtr->m_nTrackKeyCount - 1.01f;
					bSetNextAction	=	true;
				}
				m_bPlayOver			=	true;
				if(m_callback)
				{
					m_callback->ActionPlayOver(this, m_activePtr->m_actionPtr->m_actionType, m_activePtr->m_actionPtr->m_name.t);
				}
				//设置最终移动点为当前位置
				//m_world.SetPosition(m_world.GetPosition() + m_activePtr->m_offset);
			}
		}
		//
		if(!m_activePtr)
		{
			return;
		}
		EmissionFrameMove fm;
		color4 diffuse(1,1,1,1);
		if(this->m_geometryPtr)
		{
			diffuse		=	m_geometryPtr->GetDiffuseColor();
			fm.color.setRGBA(0,0,0,0);//	=	m_geometryPtr->GetEmissiveColor();
			fm.color.a	=	1.0f;
		}
		m_activePtr->BeforeFrameMove(fm, m_activePtr->m_fKeyFrameIndex);
		int saveAction			=	fm.actionID;
		float actionKeyIndex	=	fm.actionKeyIndex;
		for(int i=0;i<m_actionCount;i++)
		{
			if(&m_actionList[i] != m_activePtr && m_actionList[i].m_actionPtr->m_bIsTemplate)
			{
				Unit3DAction& act = m_actionList[i];
				for(int k=0;k<act.m_elements.size();k++)
				{
					act.m_elements[k]->CheckChange();
					if(act.m_elements[k]->m_bTemplateVisible)
					{
						if(!m_bIsPause)
							act.m_fKeyFrameIndex	+=	g_manager.GetElapsedTime() * act.m_fPlaySpeed * act.m_actionPtr->m_fPlaySpeed * TRACK_SECOND_LENGTH;
						if(act.m_fKeyFrameIndex >= (float)act.m_actionPtr->m_nTrackKeyCount)
						{
							//act.StopAction();
							act.m_fKeyFrameIndex	=	0.0f;
							act.BeginAction();
						}
						else
							act.m_elements[k]->BeforeFrameMove(fm, this->m_geometryPtr, act.m_fKeyFrameIndex);
					}
				}
			}
		}
		fm.actionKeyIndex	=	actionKeyIndex;
		fm.actionID			=	saveAction;
		//
		if(m_geometryPtr)
		{
			if(fm.actionID >= 0)
			{
				if(fm.actionID != m_geometryPtr->GetCurrentActionID())
					m_geometryPtr->SetCurrentAction(fm.actionID, 1000000);
				m_geometryPtr->SetAnimationKey(fm.actionID, fm.actionKeyIndex, true);
			}
		}
		if(fm.bOffset[0] && fm.bOffset[1] && fm.bOffset[2])
		{
			//float3 offset	=	fm.offset - m_activePtr->m_lastOffset;
			//m_activePtr->m_lastOffset	=	fm.offset;
			////if(!this->m_bLastHeight)
			////{
			////	m_bLastHeight			=	true;
			////	this->m_fLastHeightZ	=	m_world.GetPosition().z;
			////}
			////m_fOffsetZ					=	fm.offset.z;// - m_fLastDiffZ;//m_fOffsetZ + offset.z;
			//offset.transformNoMove(offset, m_world);
			//float3 pos					=	m_world.GetPosition() + offset;
			////pos.z						=	(m_fLastHeightZ - m_fRecalcHeight) + m_fOffsetZ;
			//m_world.SetPosition(pos);
		}
		matrix4x4 mat, matS;
		matrixScaling(&matS, fm.scale.x, fm.scale.y, fm.scale.z);
		matS._42	=	fm.offsetSelf;
		matrixRotationX(&mat, fm.rotationX);
		matrixMultiply(&matS, &matS, &mat);
		matrixRotationZ(&mat, fm.rotationZ);
		//
		matrixMultiply(&matS, &matS, &mat);
		mat	=	m_localMat;
		matrix4x4 worldMat;
		//mat.SetPosition(mat.GetPosition() + m_lastOffset);
		matrixMultiply(&worldMat, &matS, &m_localMat);
		if(m_geometryPtr)
		{
			fm.color.r	=	getmin(fm.color.r, 1.0f);
			fm.color.g	=	getmin(fm.color.g, 1.0f);
			fm.color.b	=	getmin(fm.color.b, 1.0f);
			//				if(fm.color.a < 0.99f)
			//					m_geometryPtr->SetAlphaMode(AlphaMode_Blend);
			//				else
			//					m_geometryPtr->SetAlphaMode(AlphaMode_None);
			diffuse.a	=	fm.color.a;
			m_geometryPtr->SetDiffuseColor(diffuse);
			m_geometryPtr->SetEmissiveColor(fm.color);
			m_geometryPtr->SetWorldMatrix(worldMat);
		}
		//
		if(m_activePtr)
		{
			m_activePtr->FrameMove(m_geometryPtr, m_activePtr->m_fKeyFrameIndex, worldMat);
			//
			for(int i=0;i<m_actionCount;i++)
			{
				if(&m_actionList[i] != m_activePtr && m_actionList[i].m_actionPtr->m_bIsTemplate)
				{
					for(int k=0;k<m_actionList[i].m_elements.size();k++)
					{
						if(m_actionList[i].m_elements[k]->m_bTemplateVisible)
							m_actionList[i].m_elements[k]->FrameMove(m_geometryPtr, worldMat);//m_actions[i]->m_fKeyFrameIndex);
					}
				}
			}
			//粒子系统继续计算
			//if(!m_bIsPause)
			//{
			//	for(int i=0;i<m_actionCount;i++)
			//		m_actionList[i].FrameMoveEmision(m_geometryPtr, worldMat);
			//}
		}
		if(bSetNextAction)
		{
			//SetActiveAction(m_nextActionID);
			if(m_activePtr)
				m_activePtr->m_fKeyFrameIndex	=	0;//g_manager.GetElapsedTime() * m_activePtr->m_fPlaySpeed * m_activePtr->m_actionPtr->m_fPlaySpeed * TRACK_SECOND_LENGTH;
		}
		if(m_geometryPtr)
			m_geometryPtr->FrameMove();
	}
	const float3&				Unit3D::GetPosition()
	{CPUTime(Unit3D);
		return this->m_worldMat.GetPosition();
	}
	VOID						Unit3D::SetPosition(const float3& pos)
	{CPUTime(Unit3D);
		this->m_worldMat.SetPosition(pos);
		if(m_geometryPtr)
		{
			matrix4x4 mat;
			matrixMultiply(&mat, &m_localMat, &m_worldMat);
			m_geometryPtr->SetWorldMatrix(mat);
		}
	}

	Unit3DAction*				Unit3D::SearchActionPtr(const char* szName)
	{CPUTime(Unit3D);
		for(int i=0;i<m_actionCount;i++)
			if(m_actionList[i].m_actionPtr->m_name == szName)
				return &m_actionList[i];
		return 0;
	}

	Unit3DAction*				Unit3D::GetActionPtr(INT nIndex)
	{CPUTime(Unit3D);
		if(nIndex < 0 || nIndex >= m_actionCount)
			return 0;
		return &m_actionList[nIndex];
	}
	Unit3DAction*				Unit3D::GetActiveByID(INT id)
	{CPUTime(Unit3D);
		for(int i=0;i<m_actionCount;i++)
			if(m_actionList[i].GetID() == id)
				return &m_actionList[i];
		return 0;
	}
	INT									Unit3D::GetActionCount()
	{CPUTime(Unit3D);
		return m_actionCount;
	}

	const matrix4x4&					Unit3D::GetWorldMatrix()
	{CPUTime(Unit3D);
		if(this->m_geometryPtr)
			return m_geometryPtr->GetWorldMatrix();
		return m_worldMat;
	}
	VOID								Unit3D::SetWorldMatrix(const matrix4x4& m)
	{CPUTime(Unit3D);
		m_worldMat	=	m;
		if(m_geometryPtr)
		{
			matrix4x4 mat;
			matrixMultiply(&mat, &m_localMat, &m_worldMat);
			m_geometryPtr->SetWorldMatrix(mat);
		}
	}

	//VOID								Unit3D::SetNextAction(Unit3DAction* act)	//设置播放完毕之后下一个动作ID，如果不设置则自动引用Unit3DActionData的设置
	//{
	//	m_nextActionID.id	=	act ? act->GetID() : 0;
	//}

	//Unit3DAction*				Unit3D::GetNextAction()	//设置播放完毕之后下一个动作ID，如果不设置则自动引用Unit3DActionData的设置
	//{
	//	Unit3DActionData* proj	=	m_unitPtr->m_datas.GetDataPtr(this->m_nextActionID);
	//	if(!proj)
	//		return 0;
	//	for(int i=0;i<m_actions.size();i++)
	//	{
	//		if(m_actions[i]->m_actionPtr == proj)
	//			return m_actions[i];
	//	}
	//	return 0;
	//}
	VOID								Unit3D::SetElementVisible(int nElementID, BOOL b)		//是否模板动作
	{CPUTime(Unit3D);
		for(int i=0;i<this->m_actionCount;i++)
		{
			if(this->m_actionList[i].m_actionPtr->m_bIsTemplate)
			{
				for(int k=0;k<this->m_actionList[i].m_elements.size();k++)
				{
					CElementRuntime* ele	=	this->m_actionList[i].m_elements[k];
					if(ele->GetID() == nElementID)
					{
						ele->m_bTemplateVisible	=	b;
						return;
					}
				}
			}
		}
	}
	BOOL								Unit3D::GetElementVisible(int nElementID)	//设置为模板动作
	{CPUTime(Unit3D);
		for(int i=0;i<this->m_actionCount;i++)
		{
			if(this->m_actionList[i].m_actionPtr->m_bIsTemplate)
			{
				for(int k=0;k<this->m_actionList[i].m_elements.size();k++)
				{
					CElementRuntime* ele	=	this->m_actionList[i].m_elements[k];
					if(ele->GetID() == nElementID)
					{
						return ele->m_bTemplateVisible;
					}
				}
			}
		}
		return false;
	}
	Unit3DMesh*							Unit3D::GetChildMeshBy(INT id)
	{CPUTime(Unit3D);
		for(int i=0;i<m_childCount;i++)
		{
			if(m_childMeshs[i].m_meshPtr->m_id == id)
				return &m_childMeshs[i];
		}
		return 0;
	}
	Unit3DMesh*							Unit3D::GetChildMesh(INT nIndex)
	{CPUTime(Unit3D);
		if(nIndex < 0||nIndex>=m_childCount)
			return 0;
		return &m_childMeshs[nIndex];
	}

	VOID								Unit3D::SetElementVisible(const char* name, BOOL b)	//是否模板动作
	{CPUTime(Unit3D);
		for(int i=0;i<this->m_actionCount;i++)
		{
			if(this->m_actionList[i].m_actionPtr->m_bIsTemplate)
			{
				for(int k=0;k<this->m_actionList[i].m_elements.size();k++)
				{
					CElementRuntime* ele	=	this->m_actionList[i].m_elements[k];
					if(stricmp(ele->GetName(), name) == 0)
					{
						ele->m_bTemplateVisible	=	b;
						return;
					}
				}
			}
		}
	}
	BOOL								Unit3D::GetElementVisible(const char* name)	//设置为模板动作
	{CPUTime(Unit3D);
		for(int i=0;i<this->m_actionCount;i++)
		{
			if(this->m_actionList[i].m_actionPtr->m_bIsTemplate)
			{
				for(int k=0;k<this->m_actionList[i].m_elements.size();k++)
				{
					CElementRuntime* ele	=	this->m_actionList[i].m_elements[k];
					if(stricmp(ele->GetName(), name) == 0){
						return ele->m_bTemplateVisible;
					}
				}
			}
		}
		return false;
	}


};
