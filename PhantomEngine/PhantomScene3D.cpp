#include "PhantomScene3D.h"
#include "PhantomManager.h"
#include "APILua.h"
#include "LuaManager.h"
#include "PhantomGame.h"
namespace Phantom{
	SceneEnv::SceneEnv(Scene3D* s)
	{
		m_id		=	s?s->GetNewIdentity():0;//环境ID
		m_scenePtr	=	s;
		sprintf(m_name.t, "env%d", m_id);
		m_bBrightness	=	0;
		m_bContrast		=	0;
		m_fBrightness	=	0;
		m_fContrast		=	1;
		m_fogEnable		=	false;
		m_fogType		=	Phantom::FogMode_Fog3D;
		m_fogColor		=	0xffffffff;
		m_ambientColor	=	0xff5f5f5f;
		m_fNear			=	1;
		m_fFar			=	10;
		m_defaultLight.m_lightType = SceneLight::LightType_Direction;
		m_defaultLight.m_dir.setxyz(1,1,1);
		m_defaultLight.m_bEnabled = true;
		m_defaultLight.m_lightColor.setxyz(1,1,1);
		m_defaultLight.m_scenePtr = m_scenePtr;
		m_defaultLight.m_id = 0;
		m_lights.push_back(&m_defaultLight);
		m_shadowViewSize	=	100.0f;//阴影贴图长度修正
		m_shadowEnabled		=	0xffff;//ShadowEnable enum
		m_shadowDensity		=	0.8;//设置阴影浓度
		m_shadowPCF			=	0;
		m_shadowMapSize		=	1024;
		m_shadowBias		=	1;
		m_shadowCenter.setxyz(0,0,0);
		m_shadowMode			=	0;
		//m_shadowFront		=	1.0f;
		m_shadowCenterObj.id = m_shadowCenterObj.index = 0;
	}
	SceneEnv::~SceneEnv()
	{
		for(int i=1;i<m_lights.size();i++)
			delete m_lights[i];
		m_lights.clear();
		for(int i=0;i<m_points.size();i++)
			delete m_points[i];
		m_points.clear();
	}
	BOOL	SceneEnv::IsActive()
	{CPUTimeNone();
		return (this==m_scenePtr->GetActiveEnv());
	}
	VOID	SceneEnv::CommitBrightness()
	{CPUTimeNone();
		g_manager.SetBrightness(this->m_bBrightness, this->m_fBrightness);
	}
	VOID	SceneEnv::CommitContrast()
	{CPUTimeNone();
		g_manager.SetContrast(this->m_bContrast, this->m_fContrast);
	}
	VOID	SceneEnv::CommitFog()
	{CPUTimeNone();
		g_manager.SetFog(m_fogEnable, m_fogColor, m_fNear, m_fFar, (FogMode)m_fogType);
	}

	SceneLight*				SceneEnv::CreateLight()//创建一个灯光设定
	{CPUTimeNone();
		SceneLight* light = new SceneLight(this->m_scenePtr);
		light->m_id	=	m_scenePtr->m_saveLightID++;
		this->m_lights.push_back(light);
		return light;
	}
	VOID					SceneEnv::RemoveLight(int id)//根据id删除灯光
	{CPUTimeNone();
		for(int i=1;i<m_lights.size();i++){
			if(m_lights[i]->m_id == id)
			{
				delete m_lights[i];
				m_lights.eraseByIndex(i);
				break;
			}
		}
	}
	SceneLight*				SceneEnv::GetLightPtr(GameID& id)//返回灯光类指针
	{CPUTimeNone();
		if(id.id==m_defaultLight.m_id)
			return &m_defaultLight;
		if(id.id==0)return 0;
		if(id.index<m_lights.size()&&m_lights[id.index]->m_id == id.id)
			return m_lights[id.index];
		for(int i=1;i<m_lights.size();i++){
			if(m_lights[i]->m_id == id.id){
				id.index = i;
				id.id = m_lights[i]->m_id;
				return m_lights[i];
			}
		}
		id.id = 0;
		return 0;
	}

	BOOL	SceneEnv::RemoveObject(int id)
	{CPUTimeNone();
		for(int i=0;i<m_objects.size();i++){
			if(m_objects[i].id==id){
				m_objects.eraseByIndex(i);
				return true;
			}
		}
		return false;
	}
	const SavePosRot*	SceneEnv::GetObjectByID(int id)
	{CPUTimeT(SceneEnv);
		for(int i=0;i<m_objects.size();i++){
			if(m_objects[i].id==id){
				return &m_objects[i];
			}
		}
		return 0;
	}
	VOID	SceneEnv::AddObject(SceneObject* s)
	{CPUTimeNone();
		for(int i=0;i<m_objects.size();i++){
			if(m_objects[i].id==s->GetID()){
#ifdef EDITOR_OS
				m_objects[i].mat = s->m_worldMat;
#else
				m_objects[i].mat = *s->GetWorldPtr();
#endif
				m_objects[i].rot = s->m_fRotationZ;
				m_objects[i].visible = s->m_bVisible;
				return;
			}
		}
		SavePosRot spr;
		spr.id = s->GetID();
#ifdef EDITOR_OS
		spr.mat = s->m_worldMat;
#else
		spr.mat = *s->GetWorldPtr();
#endif
		spr.rot = s->m_fRotationZ;
		spr.visible = s->m_bVisible;
		m_objects.push_back(spr);
	}
	VOID	SceneEnv::CommitShadowMap()
	{CPUTimeNone();
	//m_shadowEnabled = 0xffff;
		g_manager.m_shadowViewSize = m_shadowViewSize;
		g_manager.m_shadowEnabled = m_shadowEnabled;
		g_manager.m_shadowDensity = m_shadowDensity;
		g_manager.m_shadowPCF = m_shadowPCF;
		g_manager.m_shadowEpsilon = m_shadowBias * 0.001f+1.0f;
		if(this->IsShadowEnable())
			g_manager.m_shadowMap.Create(m_shadowMapSize);
		else
			g_manager.m_shadowMap.Destroy();
	}
	VOID	SceneEnv::Active()
	{CPUTimeNone();
		for(int i=0;i<m_objects.size();i++){
			SceneID sid;
			sid.id = m_objects[i].id;
			SceneObject* obj = m_scenePtr->SearchObject(sid);
			if(obj){
				obj->ResetEnv(&m_objects[i]);
				continue;
			}
			else{
				SceneStaticMesh* smesh = m_scenePtr->SearchStaticObject(m_objects[i].id);
				if(smesh){
					smesh->SetWorld(m_objects[i].mat);
					smesh->m_bIsVisible=m_objects[i].visible;
					continue;
				}
			}
			m_objects.eraseByIndex(i);
			i--;
		}
		this->CommitBrightness();
		this->CommitContrast();
		this->CommitFog();
		this->CommitShadowMap();
		color4 c=m_ambientColor;
		g_manager.m_ambientColor.setxyz(c.r,c.g,c.b);
		this->m_scenePtr->m_lightChanged++;
	}

	ScenePoint*		SceneEnv::GetPoint(PtrID& id)
	{CPUTimeT(SceneEnv);
		if(id.id==0)
			return 0;
		if(id.index < m_points.size()&&m_points[id.index]->m_id==id.id)
			return m_points[id.index];
		for(int i=0;i<m_points.size();i++)
		{
			if(m_points[i]->m_id == id.id)
			{
				id.index = i;
				id.id = m_points[i]->m_id;
				return m_points[i];
			}
		}
		id.id = 0;
		return 0;
	}
	ScenePoint*		SceneEnv::GetPointByID(INT id)
	{CPUTimeT(SceneEnv);
		for(int i=0;i<m_points.size();i++)
		{
			if(m_points[i]->m_id == id)
				return m_points[i];
		}
		return 0;
	}
	ScenePoint*		SceneEnv::GetPointByEventID(INT id)
	{CPUTimeT(SceneEnv);
		for(int i=0;i<m_points.size();i++)
		{
			if(m_points[i]->m_eventid == id)
				return m_points[i];
		}
		return 0;
	}
	ScenePoint*		SceneEnv::GetPointByName(const char* szName)
	{CPUTimeT(SceneEnv);
		for(int i=0;i<m_points.size();i++)
		{
			if(m_points[i]->m_name == szName)
				return m_points[i];
		}
		return 0;
	}

	ScenePoint*		SceneEnv::InsertPoint(const float3& pos)
	{CPUTimeNone();
		ScenePoint* evt = new ScenePoint(this->m_scenePtr);
		evt->m_pos = pos;
		m_points.push_back(evt);
		return evt;
	}
	VOID			SceneEnv::RemovePoint(ScenePoint* obj)
	{CPUTimeNone();
		for(int i=0;i<m_points.size();i++)
		{
			if(m_points[i] == obj){
				delete m_points[i];
				m_points.eraseByIndex(i);
				break;
			}
		}
	}
	VOID	SceneEnv::Save(CSafeFileHelperW& w)
	{CPUTimeT(SceneEnv);
		int version = 10;
		w << version;
		w << m_id;
		w.str(m_name);
		int numObj = this->m_objects.size();
		w << numObj;
		if(numObj>0){
			w.pStream->write(m_objects.GetPtr(), sizeof(SavePosRot)*m_objects.size());
		}
		int numEvent = this->m_points.size();
		w << numEvent;
		for(int i=0;i<numEvent;i++)
			m_points[i]->Save(w);
		//
		w << m_bBrightness << m_bContrast << m_fBrightness << m_fContrast << m_fogEnable << m_fogType << m_fogColor << m_fNear << m_fFar;
		w << m_ambientColor;
		m_defaultLight.Save(w);
		int numLight = m_lights.size()-1;
		w << numLight;
		for(int i=1;i<m_lights.size();i++){
			m_lights[i]->Save(w);
		}
		w << m_shadowViewSize << m_shadowEnabled << m_shadowDensity << m_shadowPCF;//软化程度
		w << m_shadowMapSize << m_shadowBias;
		w << m_shadowCenterObj;
	}
	VOID	SceneEnv::Load(CSafeFileHelperR& r)
	{CPUTimeT(SceneEnv);
		int version = 1;
		r >> version;
		r >> m_id;
		r.str(m_name);
		int numObj = 0;
		r >> numObj;
		if(numObj>0){
			m_objects.SetArrayCount(numObj);
			r.pStream->read(m_objects.GetPtr(), sizeof(SavePosRot)*m_objects.size());
		}
		if(version>2){
			int numEvent = 0;
			r >> numEvent;
			if(numEvent>0){
				m_points.resize(numEvent);
				for(int i=0;i<numEvent;i++)
				{
					m_points[i] = new ScenePoint(this->m_scenePtr);
					m_points[i]->Load(r);
				}
			}
		}
		if(version>=4)
			r >> m_bBrightness >> m_bContrast >> m_fBrightness >> m_fContrast >> m_fogEnable >> m_fogType >> m_fogColor >> m_fNear >> m_fFar;
		if(version>=5)
			r >> m_ambientColor;
		if(version>=6){
			m_defaultLight.Load(r);
			int numLight;
			r >> numLight;
			for(int i=0;i<numLight;i++){
				SceneLight* light = new SceneLight(this->m_scenePtr);
				light->Load(r);
				m_lights.push_back(light);
			}
		}
		if(version>=8)
			r >> m_shadowViewSize >> m_shadowEnabled >> m_shadowDensity >> m_shadowPCF;//软化程度
		if(version>=9)
			r >> m_shadowMapSize >> m_shadowBias;
		if(version>=10)
			r >> m_shadowCenterObj;
	}
	SceneLight::SceneLight(Scene3D* s)
	{
		m_pos.setxyz(0,0,0);
		m_dir.setxyz(0,1,0);
		m_bEnabled		=	false;//是否灯光模型
		m_lightType		=	LightType_Omni;//灯光类型
		m_lightColor.setxyz(1,1,1);
		m_lightNear		=	1.0f;
		m_lightFar		=	10.0f;
		m_lightAngleSmall	=	PI*0.1f;//灯光大小
		m_lightAngleBig		=	PI*0.2f;//灯光大小
		m_angleZ			=	0;
		m_angleY			=	PI*0.1f;
		m_scenePtr			=	s;
		m_bEnabled			=	false;
		m_id				=	0;
		recalcAngle();
	}
	SceneLight::~SceneLight()
	{
	}
	void		SceneLight::recalcAngle()
	{
		this->m_dir.setxyz(0,1,0);
		m_dir.RotateX(m_dir, m_angleY);
		m_dir.RotateZ(m_dir, m_angleZ);
	}
	VOID		SceneLight::Save(CSafeFileHelperW& w)
	{CPUTimeT(SceneLight);
		char version = 3;
		w << version << m_pos << m_dir << m_bEnabled << m_lightType;//灯光类型
		w << m_lightColor << m_lightNear << m_lightFar << m_lightAngleSmall << m_lightAngleBig;//灯光大小
		w << m_angleZ << m_angleY;
		w << m_id;
		w.str(m_name);
	}
	VOID		SceneLight::Load(CSafeFileHelperR& r)
	{CPUTimeT(SceneLight);
		char version = 1;
		r >> version >> m_pos >> m_dir >> m_bEnabled >> m_lightType;//灯光类型
		r >> m_lightColor >> m_lightNear >> m_lightFar >> m_lightAngleSmall >> m_lightAngleBig;//灯光大小
		if(version>=3)
			r >> m_angleZ >> m_angleY;
		r >> m_id;
		if(version>=2)
			r.str(m_name);
	}
	VOID		SceneLight::SetLightType(SceneLight::LightType type)
	{CPUTimeNone();
		m_lightType = type;
	}
	SceneLight*	SceneLight::Clone()
	{CPUTimeT(SceneLight);
		if(!m_scenePtr)return 0;
		SceneLight* light = m_scenePtr->GetActiveEnv()->CreateLight();
		light->m_name = m_name;
		light->m_pos = m_pos;

		light->m_dir = m_dir;
		light->m_angleZ = m_angleZ;
		light->m_angleY = m_angleY;
		light->m_bEnabled = m_bEnabled;
		light->m_lightType = m_lightType;
		light->m_lightColor = m_lightColor;
		light->m_lightNear = m_lightNear;
		light->m_lightFar = m_lightFar;
		light->m_lightAngleSmall = m_lightAngleSmall;
		light->m_lightAngleBig = m_lightAngleBig;
		m_scenePtr->m_lightChanged++;
		return light;
	}
	VOID	SceneRenderBlood::Render3D()
	{CPUTimeT(SceneRenderBlood);
		this->m_obj->RenderBlood();
	}
	SceneStaticMesh::SceneStaticMesh(Scene3D* s)
	{
		m_meshPtr	=	0;
		m_bIsClone	=	0;
		m_id		=	0;
		m_lightChanged	=	0;
		m_bIsFrameMove	=	false;
		m_bIsVisible	=	true;//是否现实
		m_bIsEnabled	=	false;//是否有效
		m_saveVisible	=	true;
		m_bIsFogVisible	=	true;
		m_scenePtr		=	s;
		m_saveMat.identity();
		m_mat.identity();
	}
	SceneStaticMesh::~SceneStaticMesh()
	{
		if(m_bIsClone)
			safe_release(m_meshPtr);
	}
	void	SceneStaticMesh::RenderToShadowMap()
	{CPUTimeT(SceneStaticMesh);
		if(!m_meshPtr||!m_bIsVisible||!m_bIsFogVisible)
			return;
		if(m_bIsFrameMove)
			m_meshPtr->RenderToShadowMap();
		else
			m_meshPtr->RenderToShadowMap(this);
	}
	char	SceneStaticMesh::GetHeight(IntersectData* inter)
	{CPUTimeNone();
		if(!m_bIsVisible)
			return false;
		return m_meshPtr->GetHeight(inter, &this->m_mat, m_region);
	}
	VOID	SceneStaticMesh::SetWorld(const matrix4x4& mat)
	{CPUTimeT(SceneStaticMesh);
		if(m_bIsFrameMove)
		{
			m_mat = mat;
			m_meshPtr->SetWorldMatrix(m_mat);
		}
		else
		{
			if(m_meshPtr->GetDataPtr()->UseBaseMatrix())
				matrixMultiply(&m_mat, &m_meshPtr->GetDataPtr()->m_baseMatrix, &mat);
			else
				m_mat = mat;
		}
		onChange();
	}
	VOID	SceneStaticMesh::OnFrameMove()
	{CPUTimeT(SceneStaticMesh);
		if(!m_meshPtr||!this->m_bIsVisible)
			return;
		m_bIsFogVisible = true;
		if(g_manager.IsFogEnable()){
			float3 vPos = m_mat.GetPosition() - g_manager.GetFogPos();
			if(g_manager.GetFogMode()>=FogMode_Fog2D)
				vPos.z = 0;
			if((vPos.R()-this->m_region.sphere.radius)>g_manager.GetFogFar())
			{
				m_bIsFogVisible=false;
			}
		}
		if(m_bIsFogVisible){
			if(m_lightChanged!=m_scenePtr->m_lightChanged){
				m_lightChanged = m_scenePtr->m_lightChanged;
				m_scenePtr->RebuildLight(m_light, m_region.sphere.center, m_region.sphere.radius);
			}
			if(m_bIsFrameMove)
				m_meshPtr->FrameMove();
		}
	}
	void	SceneStaticMesh::Render()
	{CPUTimeT(SceneStaticMesh);
		if(!m_meshPtr||!m_bIsVisible||!m_bIsFogVisible)
			return;
		m_meshPtr->m_lightPtr = &m_light;
		if(m_bIsFrameMove)
			m_meshPtr->Render();
		else
			m_meshPtr->AutoPushRender(this, 1.0f);
		m_meshPtr->m_lightPtr = 0;
	}
	void	SceneStaticMesh::drawObject(const matrix4x4* matPtr)
	{CPUTimeT(SceneStaticMesh);
		if(!m_meshPtr||!m_bIsVisible)
			return;
		m_meshPtr->m_lightPtr = &m_light;
		m_meshPtr->RenderAlphaOnly(this, 1.0f);
		m_meshPtr->m_lightPtr = 0;
	}
	VOID			SceneStaticMesh::onChange()
	{CPUTimeT(SceneStaticMesh);
		if(!m_meshPtr)
			return;
		PObjectRegion obj;
		obj.aabox._min.setxyz(100000000.0f, 100000000.0f, 100000000.0f);
		obj.aabox._max.setxyz(-100000000.0f, -100000000.0f, -100000000.0f);
		for(int i=0;i<8;i++)
		{
			float3 v;
			m_meshPtr->GetDataPtr()->m_aaboxV.v[i].transform(v, this->m_mat);
			obj.aabox._max.maxv(obj.aabox._max, v);
			obj.aabox._min.minv(obj.aabox._min, v);
		}
		//
		float3 vM = (obj.aabox._max + obj.aabox._min)*0.5f;
		float fS = (obj.aabox._max - vM).R();
		m_region.sphere.setup(vM, fS);
		m_region.aabox = obj.aabox;
		if(this->m_bIsFrameMove)
			m_meshPtr->SetWorldMatrix(this->m_mat);
	}
	QuadBox::QuadBox(void)
	{
		m_pSceneFirst		=	0;
		m_nChildObjectCount	=	0;
		m_pParent			=	0;
		memset(m_childRegion, 0, sizeof(m_childRegion));
		m_nMaxLevel			=	0;
		m_nLevelIndex		=	0;
		m_scenePtr			=	0;
	}

	QuadBox::~QuadBox(void)
	{
		for(int i=0;i<m_staticObjects.size();i++)
			delete m_staticObjects[i];
		m_staticObjects.clear();
		while(m_pSceneFirst)
		{
			m_pSceneFirst->m_pCurrentRegion	=	NULL;
			m_pSceneFirst	=	m_pSceneFirst->m_pNext;
		}
		DestroyData();
	}
	BOOL			QuadBox::Create(int level)
	{CPUTimeNone();
		for(int i=0;i<QUAD_BOX_COUNT;i++)
		{
			if(m_childRegion[i])
				if(!m_childRegion[i]->Create(level+1))
					return false;
		}
		return true;
	}

	BOOL			QuadBox::Save(CSafeFileHelperW& w, int level)
	{CPUTimeT(QuadBox);
		int nVersion = 1;
		short rlevel = level;
		w << nVersion << rlevel;
		skip_w sw(w.pStream);
		w << this->m_regionBox;
		short numStaticObject = m_staticObjects.size();
		w << numStaticObject;
		CDynamicArray<SceneObject*> objects;
		SceneObject* obj = m_pSceneFirst;
		while(obj)
		{
			objects.push_back(obj);
			obj	=	obj->m_pNext;
		}
		short numObject = objects.size();
		w << numObject;
		for(int i=0;i<numObject;i++)
		{
			char type = objects[i]->GetType();
			w << type;
			objects[i]->Save(w);
		}
		for(int i=0;i<QUAD_BOX_COUNT;i++)
		{
			if(m_childRegion[i])
				if(!m_childRegion[i]->Save(w, level+1))
					return false;
		}
		sw.end();
		return true;
	}
	BOOL			QuadBox::Load(CSafeFileHelperR& r, int level)
	{CPUTimeT(QuadBox);
		int nVersion = 1;
		short rlevel = level;
		r >> nVersion >> rlevel;
		skip_r sr;
		sr.begin(r.pStream);
		r >> this->m_regionBox;
		short numStaticObject = 0;
		r >> numStaticObject;
		short numObject = 0;
		r >> numObject;
		for(int i=0;i<numObject;i++)
		{
			char type = 0;
			r >> type;
			SceneObject* obj = m_scenePtr->CreateObject((MeshData::MeshType)type);
			BOOL bLoadMesh = true;
#ifndef EDITOR_OS
			if(MeshData::MeshType_StaticMesh==type)
				bLoadMesh = false;
#endif
			if(!obj->Load(r, bLoadMesh))
			{
				m_scenePtr->RemoveObject(obj);
			}
			else
			{
				if(!bLoadMesh){
					SceneStaticMesh* temp = new SceneStaticMesh(this->m_scenePtr);
					temp->m_bIsEnabled = obj->m_bEnabled;
					temp->m_bIsVisible = obj->m_bVisible;
					temp->m_id = obj->m_gameID.sceneid.id;
					temp->m_meshPtr = m_scenePtr->GetStaticMeshPtr(obj->m_fileNameID, temp->m_bIsClone, temp->m_bIsFrameMove);
					temp->m_saveMat = obj->m_saveMat;
					temp->m_saveVisible = obj->m_saveVisible;
					m_staticObjects.push_back(temp);
					temp->SetWorld(obj->m_saveMat);
					temp->onChange();
					//
					m_scenePtr->RemoveObject(obj);
				}
				else{
					obj->m_pCurrentRegion = this;
					if(m_pSceneFirst)
						obj->m_pNext	=	m_pSceneFirst;
					m_pSceneFirst = obj;
				}
			}
		}
		for(int i=0;i<QUAD_BOX_COUNT;i++)
		{
			if(m_childRegion[i])
				if(!m_childRegion[i]->Load(r, level+1))
					return false;
		}
		AddObjectCount(numObject);
		sr.end();
		return true;
	}
	VOID			QuadBox::DestroyData()
	{CPUTimeNone();
		for(int i=0;i<QUAD_BOX_COUNT;i++)
			m_childRegion[i]	=	0;
	}

	VOID			QuadBox::Setup(Scene3D* scene, QuadBox* pParent, INT nLevel, INT nBeginX, INT nBeginY, INT nRegionWidth, float fRegionSize, INT nLevelIndex)	//构造
	{CPUTimeNone();
		DestroyData();
		//
		this->m_nLevelIndex	=	nLevelIndex;
		this->m_nMaxLevel	=	nLevel;
		this->m_pParent		=	pParent;
		//
		float fLevelHeight	=	(float)m_nLevelIndex;
		AABox box;
		box._min.setxyz((float)nBeginX * fRegionSize, (float)nBeginY * fRegionSize, -fLevelHeight);
		box._max.setxyz((float)nBeginX * fRegionSize + (float)nRegionWidth * fRegionSize, (float)nBeginY * fRegionSize + (float)nRegionWidth * fRegionSize, fLevelHeight);
		m_regionBox			=	box;
		this->m_scenePtr	=	scene;
	}

	VOID			QuadBox::InsertToRenderList(float fRegionSize, QuadBox** renderList, INT& renderCount, INT maxRenderCount, INT minX, INT minY, INT maxX, INT maxY)
	{CPUTimeNone();
		int regionLeft		=	(int)(m_regionBox._min.x / fRegionSize);
		int regionTop		=	(int)(m_regionBox._min.y / fRegionSize);
		int regionRight		=	(int)(m_regionBox._max.x / fRegionSize);
		int regionBottom	=	(int)(m_regionBox._max.y / fRegionSize);
		if(regionRight < minX || regionLeft > maxX || regionBottom < minY || regionTop > maxY || renderCount >= maxRenderCount)
			return;
		renderList[renderCount++]	=	this;
		for(int i=0;i<QUAD_BOX_COUNT;i++)
		{
			if(m_childRegion[i])
				m_childRegion[i]->InsertToRenderList(fRegionSize, renderList, renderCount, maxRenderCount, minX, minY, maxX, maxY);
		}
	}

	VOID			QuadBox::FrameMoveAllLevels( Camera* c, INT nRenderLevel)
	{CPUTimeNone();
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
					m_childRegion[i]->FrameMoveAllLevels(c, nRenderLevel);
			}
		}
		FrameMove( c, nRenderLevel);
	}

	VOID			QuadBox::FrameMove( Camera* c, INT nRenderLevel)
	{CPUTimeNone();
		if(this->m_nChildObjectCount <= 0 && this->m_staticObjects.size()<=0)
			return;
		SceneObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			pFirst->FrameMove();
			pFirst	=	pFirst->m_pNext;
		}
		for(int i=0;i<m_staticObjects.size();i++){
			m_staticObjects[i]->OnFrameMove();
		}
	}
	VOID			QuadBox::RenderToShadowMap()
	{CPUTimeNone();
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
					m_childRegion[i]->RenderToShadowMap();
			}
		}
		if(this->m_nChildObjectCount <= 0 && this->m_staticObjects.size()<=0)
			return;
		if(!m_scenePtr->IsEnabled(Scene3D::Enable_ShowObject)||!g_manager.IsShadowEnable(Phantom::ShadowEnable_MeshToMap))
			return;
		SceneObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			pFirst->RenderToShadowMap();
			pFirst	=	pFirst->m_pNext;
		}
		for(int i=0;i<m_staticObjects.size();i++)
			m_staticObjects[i]->RenderToShadowMap();
	}
	VOID			QuadBox::RenderAllLevels(Camera* c)
	{CPUTimeNone();
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
					m_childRegion[i]->RenderAllLevels(c);
			}
		}
		Render( c );
	}
	VOID			QuadBox::Render( Camera* c, const AABox* clipRegion)
	{CPUTimeNone();
		if(this->m_nChildObjectCount <= 0 && this->m_staticObjects.size()<=0)
			return;
		if(!m_scenePtr->IsEnabled(Scene3D::Enable_ShowObject))
			return;
		SceneObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			if(clipRegion)
			{
				if(pFirst->GetBox())
				{
					const AABox* objBox = pFirst->GetBox();
					if(objBox->_max.x < clipRegion->_min.x || objBox->_max.y < clipRegion->_min.y ||
						objBox->_min.x > clipRegion->_max.x || objBox->_min.y > clipRegion->_max.y)
					{
					}
					else
						pFirst->Render();
				}
			}
			else
				pFirst->Render();
			pFirst	=	pFirst->m_pNext;
		}
		for(int i=0;i<m_staticObjects.size();i++)
		{
			m_staticObjects[i]->Render();
			/*m_scenePtr->GetObjectPtr(this->m_staticObjects[i].fileNameIndex);*/
		}
	}

	//
	//VOID			QuadBox::RenderTerrain(Camera* c,  INT nRenderLevel)
	//{
	//	if(!c->IsAABoxVisibleLRNF(this->m_regionBoxV))
	//		return;
	//	if(m_nMaxLevel > 0)
	//	{
	//		for(int i=0;i<QUAD_BOX_COUNT;i++)
	//		{
	//			if(m_childRegion[i])
	//			{
	//				this->m_childRegion[i]->RenderTerrain( c, r, nRenderLevel );
	//			}
	//		}
	//	}
	//	//
	//	if(nRenderLevel < 0 || nRenderLevel == this->m_nMaxLevel)
	//	{
	//		if(this->m_pSceneRegion)
	//			m_pSceneRegion->Render( c, r );
	//	}
	//}

	//插入场景物体
	VOID			QuadBox::InsertSceneNode(SceneObject* obj)
	{CPUTimeT(QuadBox);
		if(obj->m_pCurrentRegion)
			obj->m_pCurrentRegion->RemoveSceneNode(obj);
		//
		SceneObject* objFirst	=	m_pSceneFirst;
		while(objFirst)
		{
			if(objFirst == obj)
				return;
			objFirst	=	objFirst->m_pNext;
		}
		//
		if(m_pSceneFirst)
			obj->m_pNext	=	m_pSceneFirst;
		//
		if(obj->GetBox())
		{
			float3& _min		=	m_regionBox._min;
			float3& _max		=	m_regionBox._max;
			float fLevelHeight	=	(float)m_nLevelIndex;
			float minZ			=	obj->GetBox()->_min.z - fLevelHeight;
			float maxZ			=	obj->GetBox()->_max.z - fLevelHeight;
			float fMax			=	getmax(maxZ, _max.z);
			float fMin			=	getmin(minZ, _min.z);
			m_regionBox._max.z	=	fMax;
			m_regionBox._min.z	=	fMin;
		}
		m_pSceneFirst	=	obj;
		obj->m_pCurrentRegion		=	this;
		this->AddObjectCount(1);
	}

	//移出场景物体
	VOID			QuadBox::RemoveSceneNode(SceneObject* obj)
	{CPUTimeT(QuadBox);
		if(obj->m_pCurrentRegion != this)
		{
			obj->m_pCurrentRegion->RemoveSceneNode(obj);
			return;
		}
		if(obj == m_pSceneFirst)
		{
			m_pSceneFirst	=	m_pSceneFirst->m_pNext;
		}
		else
		{
			BOOL bFind	=	false;
			SceneObject* objFirst	=	m_pSceneFirst;
			while(objFirst)
			{
				if(objFirst->m_pNext == obj)
				{
					objFirst->m_pNext	=	obj->m_pNext;
					bFind	=	TRUE;
					break;
				}
				objFirst	=	objFirst->m_pNext;
			}
			assert(bFind == TRUE);
		}
		obj->m_pNext				=	NULL;
		obj->m_pCurrentRegion		=	NULL;
		this->AddObjectCount(-1);
	}

	BOOL			QuadBox::InsertObject(SceneObject* obj)
	{CPUTimeT(QuadBox);
		if(!obj || !obj->GetBox())return FALSE;
		if(obj->m_pCurrentRegion)
			obj->m_pCurrentRegion->RemoveSceneNode(obj);
		//
		if(IsBoxInRegion(*obj->GetBox()))	//如果全部在此物体内
		{
			if(this->m_nMaxLevel > 0)
			{
				for(int i=0;i<QUAD_BOX_COUNT;i++)
				{
					if(this->m_childRegion[i])
					{
						if(this->m_childRegion[i]->IsBoxInRegion(*obj->GetBox()))
						{
							return m_childRegion[i]->InsertObject(obj);
						}
					}
				}
			}
			InsertSceneNode(obj);
			return true;
		}
		if(m_pParent)	//让父区域添加物体
			return m_pParent->InsertObject(obj);
		else
		{
			InsertSceneNode(obj);
			return true;
		}
		return false;
	}

	BOOL			QuadBox::OnChangeSceneObject(SceneObject* obj)
	{CPUTimeT(QuadBox);
		if(IsBoxInRegion(*obj->GetBox()))	//如果全部在此物体内
		{
			if(this->m_nMaxLevel > 0)
			{
				for(int i=0;i<QUAD_BOX_COUNT;i++)
				{
					if(this->m_childRegion[i])
					{
						if(this->m_childRegion[i]->IsBoxInRegion(*obj->GetBox()))
						{
							return m_childRegion[i]->InsertObject(obj);
						}
					}
				}
			}
			//如果还是在原来的盒子内则返回
			if(obj->m_pCurrentRegion == this)
				return TRUE;
			InsertSceneNode(obj);
			return TRUE;
		}
		if(m_pParent)	//让父区域添加物体
			return m_pParent->InsertObject(obj);
		else
		{
			InsertSceneNode(obj);
			return true;
		}
		return false;
	}

	BOOL			QuadBox::IsBoxCollision(const AABox& box)//测试盒子是否交叉
	{CPUTimeNone();
		float3& _min		=	m_regionBox._min;
		float3& _max		=	m_regionBox._max;
		if(_min.x > box._max.x || _min.y > box._max.y)
			return false;
		if(_max.x < box._min.x || _max.y < box._min.y)
			return false;
		return true;
	}

	VOID			QuadBox::RecalcRegionBox()
	{CPUTimeT(QuadBox);
		if(this->m_nChildObjectCount <= 0)
			return;
		float fMaxHeight = -100000.0f;
		float fMinHeight = 100000.0f;
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
				{
					m_childRegion[i]->RecalcRegionBox();
					fMaxHeight	=	getmax(m_childRegion[i]->m_regionBox._max.z, fMaxHeight);
					fMinHeight	=	getmin(m_childRegion[i]->m_regionBox._min.z, fMinHeight);
				}
			}
		}
		//
		SceneObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			//if(pFirst->IsAllowSelect(SELECT_OBJECT_GEOMETRY))
			const AABox* box =	pFirst->GetBox();
			if(box)
			{
				fMaxHeight	=	getmax(fMaxHeight, box->_max.z);
				fMinHeight	=	getmin(fMinHeight, box->_min.z);
			}
			pFirst	=	pFirst->m_pNext;
		}
		this->m_regionBox._max.z	=	fMaxHeight;
		this->m_regionBox._min.z	=	fMinHeight;
	}

	BOOL			QuadBox::IsBoxInRegion(const AABox& box)	//测试盒子是否在区域内
	{CPUTimeNone();
		float3& _min		=	m_regionBox._min;
		float3& _max		=	m_regionBox._max;
		if(box._min.x >= _min.x && box._max.x <= _max.x
			&& box._min.y >= _min.y && box._max.y <= _max.y)
			return true;
		return false;
	}

	VOID			QuadBox::AddObjectCount(INT nCountAdd)
	{CPUTimeT(QuadBox);
		this->m_nChildObjectCount	+=	nCountAdd;
		if(this->m_pParent)
		{
			m_pParent->AddObjectCount( nCountAdd );
			m_pParent->m_regionBox._min.z	=	getmin(m_pParent->m_regionBox._min.z, this->m_regionBox._min.z);
			m_pParent->m_regionBox._max.z	=	getmax(m_pParent->m_regionBox._max.z, this->m_regionBox._max.z);
		}
		assert(m_nChildObjectCount >= 0);
	}
	VOID			QuadBox::SelectObjects(CDynamicArray<SceneObject*>& objs, const Rect& rc)
	{CPUTimeT(QuadBox);
		if(this->m_nChildObjectCount <= 0)
			return;
		SceneObject* ret = 0;
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
					m_childRegion[i]->SelectObjects(objs, rc);
			}
		}
		//
		SceneObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			Pixel p;
			if(GetCameraPtr()->Get2DFrom3D(p, pFirst->GetPosition()))
			{
				if(rc.isPtIn(p.x, p.y))
				{
					objs.push_back( pFirst );
				}
			}
			pFirst	=	pFirst->m_pNext;
		}
	}
	BOOL			QuadBox::IntersectStatic(IntersectData* intersect)	//只点击带碰撞盒的模型
	{CPUTimeT(QuadBox);
		BOOL bRet = false;
		if(this->m_nChildObjectCount <= 0)
			return false;
		float3& _min		=	m_regionBox._min;
		float3& _max		=	m_regionBox._max;
		if(!IntersectBox(intersect->GetOriginal(), intersect->GetDir(), _min, _max, NULL))
			return false;
		SceneObject* ret = 0;
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
				{
					if(m_childRegion[i]->IntersectStatic(intersect))
						bRet = true;
				}
			}
		}
		for(int i=0;i<this->m_staticObjects.size();i++){
			if(m_staticObjects[i]->m_bIsVisible&&m_staticObjects[i]->m_bIsEnabled&&m_staticObjects[i]->m_meshPtr){
				if(m_staticObjects[i]->m_meshPtr->PickRay(intersect, &m_staticObjects[i]->m_mat, m_staticObjects[i]->m_region, Mesh::MeshPick_Height))
					bRet = true;
			}
		}
		return bRet;
	}
	BOOL			QuadBox::IntersectStaticHeight(IntersectData* intersect)	//只点击带碰撞盒的模型
	{CPUTimeT(QuadBox);
		BOOL bRet = false;
		if(this->m_nChildObjectCount <= 0)
			return false;
		float3& _min		=	m_regionBox._min;
		float3& _max		=	m_regionBox._max;
		if(!IntersectBox(intersect->GetOriginal(), intersect->GetDir(), _min, _max, NULL))
			return false;
		SceneObject* ret = 0;
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
				{
					if(m_childRegion[i]->IntersectStaticHeight(intersect))
						bRet = true;
				}
			}
		}
		for(int i=0;i<this->m_staticObjects.size();i++){
			if(this->m_staticObjects[i]->m_meshPtr){
				if(m_staticObjects[i]->m_meshPtr->PickRay(intersect, &m_staticObjects[i]->m_mat, m_staticObjects[i]->m_region, Mesh::MeshPick_Collision))
					bRet = true;
			}
		}
		return bRet;
	}
	SceneObject*	QuadBox::Intersect(IntersectData* intersect, BOOL bCollisionOnly)
	{CPUTimeT(QuadBox);
		if(this->m_nChildObjectCount <= 0)
			return NULL;
		float3& _min		=	m_regionBox._min;
		float3& _max		=	m_regionBox._max;
		//if(!IntersectBox(intersect->GetOriginal(), intersect->GetDir(), _min, _max, NULL))
		//	return NULL;
		SceneObject* ret = 0;
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
				{
					SceneObject* obj = m_childRegion[i]->Intersect(intersect, bCollisionOnly);
					if(obj)
						ret = obj;
				}
			}
		}
		//
		SceneObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
//#ifndef EDITOR_OS
//			if(pFirst->m_gameID.gameid.id!=0)
//#endif
			{
				if(pFirst->Intersect( intersect, bCollisionOnly ))
					ret = pFirst;
			}
			pFirst	=	pFirst->m_pNext;
		}
		return ret;
	}

	BOOL					QuadBox::IsPointInBoxXY(const float3& pos)//测试盒子是否交叉
	{CPUTimeNone();
		float3& _min		=	m_regionBox._min;
		float3& _max		=	m_regionBox._max;
		return ((pos.x >= _min.x && pos.x <= _max.x)
			&& (pos.y >= _min.y && pos.y <= _max.y));
	}
	VOID					QuadBox::SetDefaultEnv()
	{CPUTimeNone();
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
					m_childRegion[i]->SetDefaultEnv();
			}
		}
		//
		for(int i=0;i<this->m_staticObjects.size();i++)
		{
			m_staticObjects[i]->SetWorld(m_staticObjects[i]->m_saveMat);
			m_staticObjects[i]->m_bIsVisible = m_staticObjects[i]->m_saveVisible;
		}
	}
	SceneStaticMesh*		QuadBox::SearchMesh(int id)
	{CPUTimeT(QuadBox);
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
				{
					SceneStaticMesh* m = m_childRegion[i]->SearchMesh(id);
					if(m)
						return m;
				}
			}
		}
		//
		for(int i=0;i<this->m_staticObjects.size();i++)
		{
			if(this->m_staticObjects[i]->m_id == id)
				return m_staticObjects[i];
		}
		return 0;
	}
	BOOL					QuadBox::GetHeight(IntersectData* intersect)
	{CPUTime(QuadBox);
		if(this->m_nChildObjectCount <= 0)
			return NULL;
		if(!IsPointInBoxXY(intersect->GetOriginal()))
			return NULL;
		//
		BOOL bRet	=	FALSE;
		SceneObject* ret = 0;
		if(m_nMaxLevel > 0)
		{
			for(int i=0;i<QUAD_BOX_COUNT;i++)
			{
				if(m_childRegion[i])
				{
					if(m_childRegion[i]->GetHeight(intersect))
						bRet = TRUE;
				}
			}
		}
		//
		SceneObject* pFirst	=	m_pSceneFirst;
		while(pFirst)
		{
			if(pFirst->GetHeight(intersect))
				bRet	=	TRUE;
			pFirst	=	pFirst->m_pNext;
		}
		for(int i=0;i<this->m_staticObjects.size();i++){
			if(m_staticObjects[i]->GetHeight(intersect))
				bRet = true;
		}
		return bRet;
	}
	SceneBox::SceneBox()
	{
		m_scenePtr = 0;
		m_regionX = 0;
		m_regionY = 0;
		m_grids = 0;
		m_moveGrids = 0;
		m_bVisible	=	true;
		m_lightChanged	=	0;
#ifdef EDITOR_OS
		m_attrs	=	0;
		m_bModifyed	=	false;
#endif
	}
	SceneBox::~SceneBox()
	{
		if(m_moveGrids)
			free(m_moveGrids);
		if(m_grids)
			free(m_grids);
#ifdef EDITOR_OS
		if(m_attrs)
			free(m_attrs);
#endif
		for(int i=0;i<MAX_LAYER_TEXTURE;i++)
		{
			m_mesh[i] = 0;
			m_meshAlpha[i] = 0;
		}
	}
	VOID					SceneBox::CreateTerrain()//创建地面数据
	{CPUTimeT(SceneBox);
		if(m_grids)return;
		int gridCount = REGION_VERTEX_COUNT_25d * REGION_VERTEX_COUNT_25d;
		m_grids = (GVertexData*)malloc(sizeof(GVertexData) * gridCount);
		memset(m_grids, 0, sizeof(GVertexData) * gridCount);
#ifdef EDITOR_OS
		m_attrs = (GridAttr25d*)malloc(sizeof(GridAttr25d)*gridCount);
		memset(m_attrs, 0, sizeof(GridAttr25d) * gridCount);
		RebuildMeshs(false);
#endif
	}
	VOID					SceneBox::DeleteTerrain()
	{CPUTimeT(SceneBox);
		if(!m_grids)
			return;
		if(m_grids)
			free(m_grids);
#ifdef EDITOR_OS
		if(m_attrs)
			free(m_attrs);
		m_attrs	=	0;
#endif
		m_grids	=	0;
	}
#ifdef EDITOR_OS
	VOID					SceneBox::CreateMoveGrids()
	{CPUTimeT(SceneBox);
		if(m_moveGrids)return;
		int gridCount = REGION_GRID_COUNT_25d * REGION_GRID_COUNT_25d;
		m_moveGrids = (GMoveData*)malloc(sizeof(GMoveData) * gridCount);
		memset(m_moveGrids, 0, sizeof(GMoveData) * gridCount);
		this->RebuildMoveGrids();
	}
#endif
	VOID					SceneBox::DeleteMoveGrids()
	{CPUTimeNone();
		if(!m_moveGrids)
			return;
		if(m_moveGrids)
			free(m_moveGrids);
		m_moveGrids	=	0;
	}
	VOID					SceneBox::RenderToShadowMap(){CPUTimeT(SceneBox);
		if(this->m_grids && this->m_scenePtr->IsEnabled(Scene3D::Enable_ShowTerrain) && m_bVisible && g_manager.IsShadowEnable(Phantom::ShadowEnable_TerrainToMap)){
			PBoxVertices box;
			box.setup(m_regionBox._min, m_regionBox._max);
			if(GetCameraPtr()->IsAABoxVisible(box))//IsAABoxVisibleLRNF(box))
			{
				Program* vsps = m_scenePtr->SetRenderToShadowMode();
				if(!vsps)return;
				g_manager->SetCullMode(Phantom::CullMode_CW);
				g_manager->SetAlphaMode(Phantom::AlphaMode_None);
				g_manager.SetWorldMatrix(&matrix4x4::s_identity());
				for(int i=0;i<MAX_LAYER_TEXTURE;i++)
				{
					if(m_mesh[i])
						this->m_mesh[i]->RenderMesh(vsps, m_scenePtr->GetLayerTexture(i), 0);
				}
			}
		}
		QuadBox::RenderToShadowMap();
	}
	VOID					SceneBox::Render( Camera* c, const AABox* clipRegion)
	{CPUTimeT(SceneBox);
		if(this->m_grids && this->m_scenePtr->IsEnabled(Scene3D::Enable_ShowTerrain) && m_bVisible){
			PBoxVertices box;
			box.setup(m_regionBox._min, m_regionBox._max);
			if(c->IsAABoxVisible(box))//IsAABoxVisibleLRNF(box))
			{
				if(m_lightChanged!=m_scenePtr->m_lightChanged){
					m_lightChanged = m_scenePtr->m_lightChanged;
					this->RebuildLight();
				}
				Program* vsps = m_scenePtr->SetRenderMode();
				if(!vsps)return;
				//g_manager.SetRenderMode((int)m_scenePtr->GetSceneRenderMode());
				g_manager->SetCullMode(Phantom::CullMode_CW);
				g_manager->SetAlphaMode(Phantom::AlphaMode_None);
				g_manager.SetWorldMatrix(&matrix4x4::s_identity());
				vsps->SetDiffuse(m_scenePtr->m_sceneDiffuse);
				//
				vsps->SetLight(m_light.count, m_light.lightPos, m_light.lightColor, m_light.rangeAngle);
				for(int i=0;i<MAX_LAYER_TEXTURE;i++)
				{
					if(m_mesh[i])
						this->m_mesh[i]->RenderMesh(vsps, m_scenePtr->GetLayerTexture(i), 0);
				}
				g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
				g_manager->SetZWriteEnable(false);
				for(int i=0;i<MAX_LAYER_TEXTURE;i++)
				{
					if(m_meshAlpha[i])
						this->m_meshAlpha[i]->RenderMesh(vsps, m_scenePtr->GetLayerTexture(i), 0);
				}
				g_manager->SetZWriteEnable(true);
				g_manager->SetAlphaMode(Phantom::AlphaMode_None);
			}
		}
#ifdef EDITOR_OS
		if((m_scenePtr->IsEnabled(Scene3D::Enable_ShowMoveGrids)||m_scenePtr->IsEnabled(Scene3D::Enable_ShowRegionSet)) && this->m_moveGrids && m_bVisible){
			PBoxVertices box;
			box.setup(m_regionBox._min, m_regionBox._max);
			if(c->IsAABoxVisible(box))
			{
				Program* prog = g_manager.SetRenderMode(m_scenePtr->m_renderModeMoveEnable);
				g_manager->SetCullMode(Phantom::CullMode_CW);
				g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
				prog->SetWorldMatrix(&matrix4x4::s_identity());
				if(prog){
					if(m_moveEnable){
						prog->SetDiffuse(color4(0,1,0,0.8));
						m_moveEnable->RenderMesh(prog, 0, 0);
					}
				}
			}
		}
#endif
		QuadBox::Render(c, clipRegion);
	}
	BOOL			SceneBox::InitRegion(INT nRegionX, INT nRegionY)
	{CPUTimeNone();
		m_regionX = nRegionX;
		m_regionY = nRegionY;
		return true;
	}
	GVertexData*	SceneBox::GetVertexPtr(int x, int y)
	{CPUTimeNone();
		if(x<0||y<0||x>=REGION_VERTEX_COUNT_25d||y>=REGION_VERTEX_COUNT_25d || !m_grids)
			return 0;
		return &m_grids[x+y*REGION_VERTEX_COUNT_25d];
	}
	GVertexData*	SceneBox::GetGridPtr(int x, int y)
	{CPUTimeNone();
		if(x<0||y<0||x>=REGION_VERTEX_COUNT_25d||y>=REGION_VERTEX_COUNT_25d || !m_grids)
			return 0;
		return &m_grids[x+y*REGION_VERTEX_COUNT_25d];
	}
	VOID				GetAttributeFaces25d(Triangle& facePtr1, Triangle& facePtr2, INT attrX, INT attrY, INT attrXCount, INT attrYCount, INT xWidth = REGION_VERTEX_COUNT_25d, INT nModX = 0, INT nModY = 0, INT attrX2 = -1, INT attrY2 = -1)
	{
		unsigned short f0	=	(unsigned short)(attrY * xWidth + attrX);
		unsigned short f1	=	(unsigned short)(attrY * xWidth + attrX + attrXCount);
		unsigned short f2	=	(unsigned short)((attrY + attrYCount) * xWidth + attrX + attrXCount);
		unsigned short f3	=	(unsigned short)((attrY + attrYCount) * xWidth + attrX);
		if(attrX2 == -1)
			attrX2	=	attrX;
		if(attrY2 == -1)
			attrY2	=	attrY;
		int aX				=	attrX2 / attrXCount;
		int aY				=	attrY2 / attrYCount;
		if(abs(aX % 2) == nModX)
		{
			if(abs(aY % 2) == nModY)
			{
				facePtr1.set( f0, f1, f2);
				facePtr2.set( f2, f3, f0);
			}
			else
			{
				facePtr1.set( f0, f1, f3);
				facePtr2.set( f1, f2, f3);
			}
		}
		else
		{
			if(abs(aY % 2) == nModY)
			{
				facePtr1.set( f0, f1, f3);
				facePtr2.set( f1, f2, f3);
			}
			else
			{
				facePtr1.set( f0, f1, f2);
				facePtr2.set( f2, f3, f0);
			}
		}
	}
	BOOL			SceneBox::IntersectAll(INT beginX, INT beginY, const float3& orig, const float3& dir, PickPoint& inout)
	{CPUTimeT(SceneBox);
		PIntersectData out;
		out.t = &inout.t;
		out.Pick = &inout.pos;
		out.normal = &inout.normal;
		Triangle f1;
		Triangle f2;
		GetAttributeFaces25d(f1, f2, beginX, beginY, 1, 1);
		//
		float3& _min	=	m_regionBox._min;
		//
		float3 v[3];
		int vx, vy;
		//
		GVertexData* level	=	this->GetGridPtr(beginX, beginY);
		if(!level)//如果没有场景则进行平面碰撞
			return false;
		//if(!level->isVisible())
		//	return false;
		float fGridSize = this->m_scenePtr->GetGridSize();
		vx	=	f1.v1 % REGION_VERTEX_COUNT_25d;			vy	=	f1.v1 / REGION_VERTEX_COUNT_25d;
		v[0].setxyz(_min.x + (float)vx * fGridSize, _min.y + (float)vy * fGridSize, this->GetVertexPtr(vx,vy)->GetHeight());
		//
		vx	=	f1.v2 % REGION_VERTEX_COUNT_25d;			vy	=	f1.v2 / REGION_VERTEX_COUNT_25d;
		v[1].setxyz(_min.x + (float)vx * fGridSize, _min.y + (float)vy * fGridSize, this->GetVertexPtr(vx,vy)->GetHeight());
		//
		vx	=	f1.v3 % REGION_VERTEX_COUNT_25d;			vy	=	f1.v3 / REGION_VERTEX_COUNT_25d;
		v[2].setxyz(_min.x + (float)vx * fGridSize, _min.y + (float)vy * fGridSize, this->GetVertexPtr(vx,vy)->GetHeight());
		if(Phantom::IntersectTriangle(&out, orig, dir, v[0], v[1], v[2], NULL, false))
			return TRUE;
		//
		vx	=	f2.v1 % REGION_VERTEX_COUNT_25d;			vy	=	f2.v1 / REGION_VERTEX_COUNT_25d;
		v[0].setxyz(_min.x + (float)vx * fGridSize, _min.y + (float)vy * fGridSize, this->GetVertexPtr(vx,vy)->GetHeight());
		//
		vx	=	f2.v2 % REGION_VERTEX_COUNT_25d;			vy	=	f2.v2 / REGION_VERTEX_COUNT_25d;
		v[1].setxyz(_min.x + (float)vx * fGridSize, _min.y + (float)vy * fGridSize, this->GetVertexPtr(vx,vy)->GetHeight());
		//
		vx	=	f2.v3 % REGION_VERTEX_COUNT_25d;			vy	=	f2.v3 / REGION_VERTEX_COUNT_25d;
		v[2].setxyz(_min.x + (float)vx * fGridSize, _min.y + (float)vy * fGridSize, this->GetVertexPtr(vx,vy)->GetHeight());
		if(Phantom::IntersectTriangle(&out, orig, dir, v[0], v[1], v[2], NULL, false))
			return TRUE;
		return FALSE;
	}

	BOOL			SceneBox::Save(CSafeFileHelperW& w, int level)
	{CPUTimeT(SceneBox);
		if(!QuadBox::Save(w, level))
			return false;
		int version = 2;
		w << version;
		skip_w sw(w.pStream);
		char bAttr	=	false;
		int vertexCount = REGION_VERTEX_COUNT_25d * REGION_VERTEX_COUNT_25d;
#ifdef EDITOR_OS
		bAttr	=	(m_attrs!=0);
#endif
		w << bAttr;
#ifdef EDITOR_OS
		if(m_attrs)
			w.pStream->write(m_attrs, sizeof(GridAttr25d)*vertexCount);
		//this->RebuildMeshs(true);
#endif
		char bGrids	=	(m_grids!=0);
		w << bGrids;
		if(m_grids)
			w.pStream->write(m_grids, sizeof(GVertexData)*vertexCount);
		char bMoveGrids = (m_moveGrids != 0);
		w << bMoveGrids;
		if(m_moveGrids)
			w.pStream->write(m_moveGrids, sizeof(GMoveData)*REGION_GRID_COUNT_25d*REGION_GRID_COUNT_25d);
		for(int i=0;i<MAX_LAYER_TEXTURE;i++)
		{
			char cMesh = (m_mesh[i].t!=0);
			w << cMesh;
			if(cMesh)
				SaveMesh(w.pStream, m_mesh[i]);
			char cMeshAlpha = (m_meshAlpha[i].t!=0);
			w << cMeshAlpha;
			if(cMeshAlpha)
				SaveMesh(w.pStream, m_meshAlpha[i]);
		}
		sw.end();
		return true;
	}
#pragma pack(push, 1)
	struct	GVertexDataOld{
		short						height;								//高度值，按照1点为1厘米计算，最高3千米最低-3千米
	};
	struct	GMoveDataOld{
		unsigned char				m1,m2;//(0x01,0x02,0x04,0x08)(0x10,0x20,0x40,0x80) x (m1,m2)第一行第二行是m1，第三行第四行m2
	};
#pragma pack(pop)
	BOOL			SceneBox::Load(CSafeFileHelperR& r, int level)
	{CPUTimeT(SceneBox);
		if(!QuadBox::Load(r, level))
			return false;
		int version = 1;
		r >> version;
		skip_r sr;
		sr.begin(r.pStream);
		char bAttr	=	false;
		int vertexCount = REGION_VERTEX_COUNT_25d * REGION_VERTEX_COUNT_25d;
		r >> bAttr;
		if(bAttr){
#ifdef EDITOR_OS
			m_attrs = (GridAttr25d*)malloc(sizeof(GridAttr25d)*vertexCount);
			r.pStream->read(m_attrs, sizeof(GridAttr25d)*vertexCount);
#else
			GridAttr25d* tempAttrs = (GridAttr25d*)malloc(sizeof(GridAttr25d)*vertexCount);
			r.pStream->read(tempAttrs, sizeof(GridAttr25d)*vertexCount);
			free(tempAttrs);
#endif
		}

		char bGrids	=	0;
		r >> bGrids;
		if(bGrids)
		{
			assert(m_grids==0);
			if(version<2){
				m_grids = (GVertexData*)malloc(sizeof(GVertexData) * vertexCount);
				memset(m_grids, 0, sizeof(GVertexData) * vertexCount);
				GVertexDataOld* temp = (GVertexDataOld*)malloc(sizeof(GVertexDataOld) * vertexCount);
				r.pStream->read(temp, sizeof(GVertexDataOld)*vertexCount);
				for(int i=0;i<vertexCount;i++){
					m_grids[i].height = temp[i].height;
				}
				free(temp);
			}
			else
			{
				m_grids = (GVertexData*)malloc(sizeof(GVertexData) * vertexCount);
				r.pStream->read(m_grids, sizeof(GVertexData)*vertexCount);
			}
		}
		char bMoveGrids = 0;
		r >> bMoveGrids;
		if(bMoveGrids)
		{
			if(version<2){
				m_moveGrids = (GMoveData*)malloc(sizeof(GMoveData) * REGION_GRID_COUNT_25d*REGION_GRID_COUNT_25d);
				memset(m_moveGrids, 0, sizeof(GMoveData) * REGION_GRID_COUNT_25d*REGION_GRID_COUNT_25d);
				GMoveDataOld* moveGrids = (GMoveDataOld*)malloc(sizeof(GMoveDataOld) * REGION_GRID_COUNT_25d*REGION_GRID_COUNT_25d);
				r.pStream->read(moveGrids, sizeof(GMoveDataOld)*REGION_GRID_COUNT_25d*REGION_GRID_COUNT_25d);
				for(int x=0;x<REGION_GRID_COUNT_25d;x++){
					for(int y=0;y<REGION_GRID_COUNT_25d;y++){
						m_moveGrids[x+y*REGION_GRID_COUNT_25d].setMoveEnable(moveGrids[x+y*REGION_GRID_COUNT_25d].m1>0&&moveGrids[x+y*REGION_GRID_COUNT_25d].m2>0);
					}
				}
				free(moveGrids);
			}
			else{
				m_moveGrids = (GMoveData*)malloc(sizeof(GMoveData) * REGION_GRID_COUNT_25d*REGION_GRID_COUNT_25d);
				r.pStream->read(m_moveGrids, sizeof(GMoveData)*REGION_GRID_COUNT_25d*REGION_GRID_COUNT_25d);
			}
		}
		for(int i=0;i<MAX_LAYER_TEXTURE;i++)
		{
			char cMesh;
			r >> cMesh;
			if(cMesh)
			{
				MeshPtr temp;
				LoadMesh(r.pStream, temp);
				m_mesh[i] = temp;
			}
			char cMeshAlpha;
			r >> cMeshAlpha;
			if(cMeshAlpha)
			{
				MeshPtr temp;
				LoadMesh(r.pStream, temp);
				m_meshAlpha[i] = temp;
			}
		}
		sr.end();
		return true;
	}
	VOID			Scene3D::SelectEvents(CDynamicArray<ScenePoint*>& rets, const Rect& rc)
	{CPUTimeNone();
		for(int i=0;i<GetActiveEnv()->m_points.size();i++){
			Pixel p;
			if(GetCameraPtr()->Get2DFrom3D(p, GetActiveEnv()->m_points[i]->m_pos))
			{
				if(rc.isPtIn(p.x, p.y))
				{
					rets.push_back( GetActiveEnv()->m_points[i] );
				}
			}
		}
	}
	//多选一组模型
	VOID			Scene3D::SelectObjects(CDynamicArray<SceneObject*>& objs, const Rect& rc)
	{CPUTimeT(Scene3D);
		if(m_pRegionPtr)
			m_pRegionPtr->SelectObjects(objs, rc);
	}
	//
	BOOL			Scene3D::IntersectStatic(IntersectData* intersect)
	{CPUTimeT(Scene3D);
		if(m_pRegionPtr)
			return m_pRegionPtr->IntersectStatic(intersect);
		return NULL;
	}
	BOOL			Scene3D::IntersectStaticHeight(IntersectData* intersect)
	{CPUTimeT(Scene3D);
		if(m_pRegionPtr)
			return m_pRegionPtr->IntersectStaticHeight(intersect);
		return NULL;
	}
	SceneObject*	Scene3D::Intersect(IntersectData* intersect, BOOL bCollisionOnly)	//只点击带碰撞盒的模型
	{CPUTimeT(Scene3D);
		if(m_pRegionPtr)
			return m_pRegionPtr->Intersect(intersect, bCollisionOnly);
		return NULL;
	}
	BOOL			Scene3D::GetHeight(const float3& pos, float& fHeight, float3* pNormalRet)		//取得当前点的高度
	{CPUTimeT(Scene3D);
		GetCameraPtr()->GetIntersectPtr()->begin(float3(pos.x, pos.y, fHeight + 10000.0f), float3(0.0001f, 0.0001f, -1.0f), 1000000.0f);
		if(m_pRegionPtr&&m_pRegionPtr->GetHeight(GetCameraPtr()->GetIntersectPtr()))
		{
			fHeight = GetCameraPtr()->GetIntersectPtr()->GetPickInfo().vPick.z;
			if(pNormalRet)
				*pNormalRet	=	GetCameraPtr()->GetIntersectPtr()->GetPickInfo().vFaceNormal;
			return TRUE;
		}
		return FALSE;
	}
	Scene3D::Scene3D()
	{
		this->m_nEnables	=	0;
		m_fNormalVar		=	1;
		m_fGridSize			=	1.0f;
		m_nMaxRegionWidth	=	8;
		m_pRegionPtr		=	0;
		m_sceneRegions		=	0;
		m_regions			=	0;
		m_saveObjectID		=	1;
		m_lightChanged		=	1;
		m_skyActiveIndex	=	-1;
		m_bIs2DFindway		=	false;
		m_fMaxViewDistance	=	Phantom::To3DValue(100.0f);
		m_bShowBlood		=	0;
		m_saveLightID		=	1;
		m_fBloodSize		=	0.5;
		m_bPause			=	false;
		m_bEnabled			=	true;
		m_bClipCamera		=	false;
		m_fClipCameraSize	=	1.0f;
		m_bEnableTerrainEvent	=	false;
		m_fTerrainDownRadius	=	10.0f;
		//
		m_downPos.Set(0, 0);
		memset(&m_regionList, 0, sizeof(m_regionList));
		m_objectFilePtr.SetManagerPtr(this);
		m_skyDataPtr.SetManagerPtr(this);
		m_cameraDataPtr.SetManagerPtr(this);
		m_activeCamera = PtrID(-1,0);
		m_activeSky = PtrID(-1,0);
		m_activeFog = PtrID(-1,0);
		this->SetEnabled(Enable_ShowScene, true);
		this->SetEnabled(Enable_EnableScene, true);
		this->SetEnabled(Enable_ShowTerrain, true);
		this->SetEnabled(Enable_ShowObject, true);
		//m_mainObj			=	0;
		m_cameraFollow		=	0;
		memset(this->m_layerTextures, 0, sizeof(m_layerTextures));
		memset(m_classIDs, 0, sizeof(m_classIDs));
		for(int i=0;i<MAX_LAYER_TEXTURE;i++)
		{
			m_layerTextures[i].bExist = true;
			m_layerTextures[i].bMoveEnable = true;
			m_layerTextures[i].bIsMask = true;
		}
		g_manager.PushMap3D(this);
		this->m_bloodDraw.SetTexture("default_init_zip/blood.png");
		this->m_bloodDraw.SetRect(-96,0,0,8);
		this->m_nameDraw.SetRect(-16,0,64,16);
		m_selectObj			=	0;
		m_classChanged		=	true;
		m_sceneDiffuse.setRGBA(1,1,1,1);
		m_specularExponent	=	25.0f;
		m_specularPower		=	0.3f;
		m_activeEnv			=	&m_defaultEnv;//当前激活的环境
		m_programID			=	0;
		m_programTSM_ID		=	0;
		//m_sceneRenderMode	=	g_manager.CreateProgram(Program::Type_UV|Program::Type_Light|Program::Type_Normal, false);
		m_nEventExist		=	0;
#ifdef EDITOR_OS
		m_bChangeEnable		=	true;
		m_renderModeMoveEnable	=	g_manager.CreateProgram(Program::Type_Color, false);
#endif
		m_bloodProgress		=	0;//血条控件
		m_hitControl		=	0;//被击特效控件第一个
		m_dlgScene			=	0;//场景对象对话框
		m_defaultEnv.m_scenePtr = this;
	}

	Scene3D::~Scene3D()
	{
		m_activeEnv		=	0;
		for(int i=0;i<m_sceneEnvs.size();i++)
			delete m_sceneEnvs[i];
		m_sceneEnvs.clear();
		m_dlgScene	=	0;//场景对象对话框
		for(int i=0;i<m_skills.size();i++)
			delete m_skills[i];
		m_skills.clear();
		for(int i=0;i<m_gameSkills.size();i++)
			delete m_gameSkills[i];
		m_gameSkills.clear();
		for(int i=0;i<m_staticMeshs.size();i++)
			safe_release(m_staticMeshs[i].mesh);
		for(int i=0;i<MAX_LAYER_TEXTURE;i++)
			safe_release(m_layerTextures[i].texturePtr);
		DestroyScene();
		g_manager.PopMap3D(this);
	}
	Program*				Scene3D::SetRenderToShadowMode()
	{CPUTimeT(Scene3D);
		Program* vsps = 0;
		int programType		=	Program::Type_ToShadowMap|Program::Type_UV|Program::Type_Normal;
		if(m_programTSM_ID > 0)
			vsps = g_manager.SetRenderMode(m_programTSM_ID, false);
		if(vsps && programType != vsps->GetProgramType())
			m_programTSM_ID = 0;
		if(m_programTSM_ID == 0){
			m_programTSM_ID = g_manager.CreateProgram(programType, false);
			if(m_programTSM_ID > 0)
				vsps = g_manager.SetRenderMode(m_programTSM_ID, false);
			else
				return 0;
		}
		else if(m_programTSM_ID < 0)
			return 0;
		if(vsps)
			vsps->CommitToShadowMap();
		return vsps;
	}
	Program*				Scene3D::SetRenderMode()
	{CPUTimeT(Scene3D);
		Program* vsps = 0;
		int programType		=	Program::Type_UV|Program::Type_Light|Program::Type_Normal | g_manager.GetBaseProgramType();
		if(programType&Program::Type_WithShadowMap&&!g_manager.IsShadowEnable(Phantom::ShadowEnable_TerrainWithMap))
			programType^=Program::Type_WithShadowMap;
		if(m_programID > 0)
			vsps = g_manager.SetRenderMode(m_programID, false);
		if(vsps && programType != vsps->GetProgramType())
			m_programID = 0;
		if(m_programID == 0){
			m_programID = g_manager.CreateProgram(programType, false);
			if(m_programID > 0)
				vsps = g_manager.SetRenderMode(m_programID, false);
			else
				return 0;
		}
		else if(m_programID < 0)
			return 0;
		if(programType&Program::Type_WithShadowMap)
			vsps->CommitShadowMap();
		return vsps;
	}
	const char*				Scene3D::GetRegionName(unsigned int index)
	{CPUTimeNone();
		if(index>=MAX_REGION_COUNT)
			return "";
		if(m_regionList[index].bCustomName||!GetGameMgr()->m_tblCity)
			return m_regionList[index].name.t;
		ConfigLine* line = GetGameMgr()->m_tblCity->GetLineFromID(m_regionList[index].regionid);
		if(!line)
			return m_regionList[index].name.t;
#ifndef EDITOR_OS
		m_regionList[index].name = line->GetLineName();
		m_regionList[index].bCustomName = true;
#endif
		return line->GetLineName();

	}
	SceneRegionData*		Scene3D::GetRegionPtr(unsigned int index)
	{CPUTimeNone();
		if(index>=MAX_REGION_COUNT)
			return 0;
		return &m_regionList[index];
	}
	VOID					Scene3D::ActiveEnv(SceneEnv* env)
	{CPUTimeNone();
		if(m_activeEnv==env&&m_activeEnv)return;
		if(env==0)
			env=&m_defaultEnv;
#ifdef EDITOR_OS
		m_bChangeEnable = false;
#endif
		if(env==&m_defaultEnv||env==0){
			this->m_activeEnv = &m_defaultEnv;
			for(int i=0;i<this->m_sceneObjects.size();i++){
				m_sceneObjects[i]->ResetEnv(0);
			}
			this->m_pRegionPtr->SetDefaultEnv();
			m_activeEnv->Active();
		}
		else{
			for(int i=0;i<m_sceneEnvs.size();i++){
				if(m_sceneEnvs[i]==env){
					this->m_activeEnv = env;
					m_activeEnv->Active();
					break;
				}
			}
		}
		LogInfo("system->log:%s(%s)", utf8("激活环境"), env->m_name.t);
#ifdef EDITOR_OS
		m_bChangeEnable = true;
#endif
	}
	SceneEnv*				Scene3D::GetEnvByID(int id)
	{CPUTimeNone();
		for(int i=0;i<m_sceneEnvs.size();i++){
			if(m_sceneEnvs[i]->m_id==id)
				return m_sceneEnvs[i];
		}
		return 0;
	}
	SceneEnv*				Scene3D::CreateEnv()//创建一个环境设定
	{CPUTimeNone();
		SceneEnv* env = new SceneEnv(this);
		m_sceneEnvs.push_back(env);
		return env;
	}
	VOID					Scene3D::RemoveEnv(int id)//根据id删除env
	{CPUTimeNone();
		for(int i=0;i<m_sceneEnvs.size();i++){
			if(m_sceneEnvs[i]->m_id == id)
			{
				delete m_sceneEnvs[i];
				break;
			}
		}
	}

	VOID					Scene3D::RemoveHit(UIControl* c)
	{CPUTimeT(Scene3D);
		for(int i=0;i<m_hits.size();i++){
			if(m_hits[i].hit == c){
				m_hits[i].id.id = 0;
				break;
			}
		}
	}
	UIControl*				Scene3D::AddHit(SceneObject* obj, int val)
	{CPUTimeT(Scene3D);
		if(!m_hitControl)return 0;
		int index = -1;
		for(int i=0;i<m_hits.size();i++){
			if(m_hits[i].id.id==0){
				index = i;
				break;
			}
			else if(!this->SearchObject(m_hits[i].id)){
				index = i;
				break;
			}
		}
		if(index<0){
			index = m_hits.size();
			HitObject hit;
			hit.hit = this->m_hitControl->Clone(-1);
			hit.id = obj->m_gameID.sceneid;
			m_hits.push_back(hit);
		}
		HitObject& hit = m_hits[index];
		char buf[128];
		sprintf(buf, "%d", val);
		hit.hit->SetText(buf);
		hit.hit->PlayAction(Phantom::ActionType_Idle);
		hit.id = obj->m_gameID.sceneid;
		return hit.hit;
		//HitObject hit;
		//hit.id
	}
	BOOL					Scene3D::LoadBloodDlg(const char* szDlg)//载入场景设置对话框,其中health控件是血条控件,hit控件是伤害控件
	{CPUTimeT(Scene3D);
		m_hits.clear();
		m_bloodProgress = 0;
		m_hitControl = 0;
		Dialog dlg;
		if(!GetDlgManager()->LoadDialog(dlg, szDlg))
			return false;
		m_dlgScene = dlg;
		UIControl* bloodProgress = m_dlgScene->GetControlFromName("blood");
		m_hitControl = m_dlgScene->GetControlFromName("hit");
		if(!bloodProgress||bloodProgress->GetType()!=Phantom::UIControlType_PROGRESS){
			LogInfo("system->error:(%s)%s", szDlg, utf8("对话框中找不到blood血条进度条控件"));
			m_dlgScene = 0;
			return false;
		}
		if(!m_hitControl){
			LogInfo("system->error:(%s)%s", szDlg, utf8("对话框中找不到hit伤害动画控件"));
			m_bloodProgress = 0;
			m_dlgScene = 0;
			return false;
		}
		m_bloodProgress = static_cast<UIProgress*>(bloodProgress);
		m_dlgScene->SetVisible(true);
		m_dlgScene->SetVisible(false);
		m_dlgScene->m_layer=-1000;
		return true;
	}
	BOOL					Scene3D::IsEventExist(unsigned char index)
	{CPUTimeNone();
		if(index>=32)return false;
		int n = 1<<index;
		return (n&m_nEventExist)>0;
	}
	VOID					Scene3D::SetEventExist(unsigned char index, BOOL b)
	{CPUTimeNone();
		if(index>=32)return;
		int n = 1<<index;
		if(b)
			m_nEventExist |= n;
		else if((m_nEventExist&n)>0)
			m_nEventExist^=n;
	}

	VOID	Scene3D::SetEnabled(SceneEnable n, BOOL b)
	{CPUTimeNone();
		if(b)
			this->m_nEnables	|= n;
		else if((m_nEnables&n)>0)
			m_nEnables ^= (m_nEnables & n);
	}
	BOOL	Scene3D::IntersectTerrain(PickPoint& out)
	{CPUTimeT(Scene3D);
		Pixel pos = g_manager.GetCursorPos(0);
		IntersectData* data = GetCameraPtr()->BeginIntersect(pos.x, pos.y);
		return IntersectTerrain(data->GetOriginal(), data->GetDir(), out);
	}
	BOOL	Scene3D::IntersectTerrain(const float3& orig, const float3& dir, PickPoint& out)//点击地形
	{CPUTimeT(Scene3D);
		float fMaxDistance	=	10000.0f;
		out.t = 100000.0f;
		float3 _to = orig + dir*(fMaxDistance/dir.R());
		PixelLinear p2d;
		if(p2d.Pick(this->m_fGridSize, this->m_fGridSize, orig.x, orig.y, _to.x, _to.y) == 0)
			return FALSE;
		//
		unsigned int numPoint = p2d.GetNumPoint();
		for(unsigned int i=0;i<numPoint;i++)
		{
			const Pixel& p = p2d.GetLinePoint(i);
			int xRegion	=	p.x / REGION_GRID_COUNT_25d;
			int yRegion	=	p.y / REGION_GRID_COUNT_25d;
			SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
			if(!pRegion)
				continue;
			if(p.x < 0 || p.y < 0 || p.x >= this->GetNumofGridW() || p.y >= GetNumofGridW())
				continue;
			if(pRegion->IntersectAll( p.x % REGION_GRID_COUNT_25d, p.y % REGION_GRID_COUNT_25d, orig, dir, out))
			{
				out.normal.normalize();
				return TRUE;
			}
		}
		return FALSE;
	}
	VOID	Scene3D::OnInputEvent(Phantom::InputEventID btn, int x, int y, unsigned int touchIndex)
	{CPUTimeT(Scene3D);
		if(!this->m_bEnabled)return;
		if(btn==Phantom::InputEventID_ButtonDown)
		{
			m_downPos.Set(x,y);
			if(m_bEnableTerrainEvent){
				IntersectData* data = GetCameraPtr()->BeginIntersect(x, y);
				m_selectObj = this->Intersect(data, true);
				if(m_selectObj){
					for(int i=0;i<m_selectObj->m_binds.size();i++)
					{
						if(m_selectObj->m_binds[i].id == Phantom::EventID_Down)
						{
							lua_Map3DItem_s mgr;
							SetIPtr(&mgr.m_data, m_selectObj, SceneObject, IMap3DItemPtr);
							lua_tinker::call<void>(g_luamanager.GetLuaState(), m_selectObj->m_binds[i].func.t, mgr);
							break;
						}
					}
				}
				else
				{
					PickPoint out;
					if(IntersectTerrain(data->GetOriginal(), data->GetDir(), out))
					{
						//else if(touchIndex==1)
						//	GetGameMgr()->send(GameMessage::ToGame_ClickTerrain1, temp, &out.pos, sizeof(float3));
						for(int i=0;i<m_binds.size();i++)
						{
							if(m_binds[i].id == Phantom::EventID_Down)
							{
								lua_float3 pos,normal;
								pos.setxyz(out.pos.x, out.pos.y, out.pos.z);
								normal.setxyz(out.normal.x, out.normal.y, out.normal.z);
								lua_tinker::call<void>(g_luamanager.GetLuaState(), m_binds[i].func.t, pos, normal, out.t);
								break;
							}
						}
					}
				}
			}
		}
		else if(btn==Phantom::InputEventID_ButtonUp)
		{
			int offx = x - m_downPos.x;
			int offy = y - m_downPos.y;
			if(abs(offx)<m_fTerrainDownRadius && abs(offy)<m_fTerrainDownRadius){
				IntersectData* data = GetCameraPtr()->BeginIntersect(x, y);
				SceneObject* obj = this->Intersect(data, true);
				if(obj){
					if(obj->m_gameID.gameid.id>0){
						GameSceneID temp;
						GetGameMgr()->send(GameMessage::ToGame_ClickObject, temp, &obj->m_gameID, sizeof(GameSceneID));
					}
				}
				else{
					BOOL bPick = false;
					PickPoint out;
					if(IntersectTerrain(data->GetOriginal(), data->GetDir(), out))
						bPick=true;
					if(this->IntersectStaticHeight(data))
					{
						if(data->GetPickInfo().fDist<out.t)
						{
							out.t = data->GetPickInfo().fDist;
							out.normal = data->GetPickInfo().vFaceNormal;
							out.pos = data->GetPickInfo().vPick;
						}
						bPick = true;
					}
					if(bPick){
						if(IsEnabled(this->Enable_MoveGridsData)){
							if(!this->IsMoveGridEnable(out.pos))
								bPick=false;
						}
					}
					if(bPick)
					{
						GameSceneID temp;
						if(touchIndex==0)
							GetGameMgr()->send(GameMessage::ToGame_ClickTerrain, temp, &out.pos, sizeof(float3));
					}
				}
				if(m_bEnableTerrainEvent){
					if(m_selectObj)
					{
						if(m_selectObj==obj){
							for(int i=0;i<m_selectObj->m_binds.size();i++)
							{
								if(m_selectObj->m_binds[i].id == Phantom::EventID_Up)
								{
									lua_Map3DItem_s mgr;
									SetIPtr(&mgr.m_data, m_selectObj, SceneObject, IMap3DItemPtr);
									lua_tinker::call<void>(g_luamanager.GetLuaState(), m_selectObj->m_binds[i].func.t, mgr);
									break;
								}
							}
							for(int i=0;i<m_selectObj->m_binds.size();i++)
							{
								if(m_selectObj->m_binds[i].id == Phantom::EventID_Click)
								{
									lua_Map3DItem_s mgr;
									SetIPtr(&mgr.m_data, m_selectObj, SceneObject, IMap3DItemPtr);
									lua_tinker::call<void>(g_luamanager.GetLuaState(), m_selectObj->m_binds[i].func.t, mgr);
									break;
								}
							}
							m_selectObj = 0;
						}
					}
					else{
						PickPoint out;
						if(IntersectTerrain(data->GetOriginal(), data->GetDir(), out))
						{
							for(int i=0;i<m_binds.size();i++)
							{
								if(m_binds[i].id == Phantom::EventID_Up)
								{
									lua_float3 pos,normal;
									pos.setxyz(out.pos.x, out.pos.y, out.pos.z);
									normal.setxyz(out.normal.x, out.normal.y, out.normal.z);
									lua_tinker::call<void>(g_luamanager.GetLuaState(), m_binds[i].func.t, pos, normal, out.t);
									break;
								}
							}
							for(int i=0;i<m_binds.size();i++)
							{
								if(m_binds[i].id == Phantom::EventID_Click)
								{
									lua_float3 pos,normal;
									pos.setxyz(out.pos.x, out.pos.y, out.pos.z);
									normal.setxyz(out.normal.x, out.normal.y, out.normal.z);
									lua_tinker::call<void>(g_luamanager.GetLuaState(), m_binds[i].func.t, pos, normal, out.t);
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	VOID	Scene3D::SetActiveLight(const char* szLight)
	{CPUTimeNone();
	}

	VOID	Scene3D::DestroyScene()
	{CPUTimeNone();
		if(m_sceneRegions)
			delete[] m_sceneRegions;
		m_sceneRegions	=	0;
		if(m_regions)
			delete[] m_regions;
		m_regions		=	0;
		m_pRegionPtr	=	0;
		for(int i=0;i<m_sceneObjects.size();i++)
		{
			safe_release(m_sceneObjects[i]);
		}
		m_sceneObjects.clear();
		m_skyDataPtr.ClearDatas();
		m_cameraDataPtr.ClearDatas();
		m_objectFilePtr.ClearDatas();
	}

	SceneObjectFile*	Scene3D::GetObjectFile(int id)
	{CPUTimeNone();
		return m_objectFilePtr.SearchDataPtr(id);
	}
	VOID				Scene3D::CreateTerrain()//创建地表数据
	{CPUTimeT(Scene3D);
		if(this->HasTerrainData())
			return;
		this->SetEnabled(Scene3D::Enable_TerrainData, true);
		for(int x=0;x<m_nMaxRegionWidth;x++){
			for(int y=0;y<m_nMaxRegionWidth;y++){
				m_sceneRegions[x + y * m_nMaxRegionWidth].CreateTerrain();
			}
		}
	}
	VOID				Scene3D::DeleteTerrain()//释放地表数据，释放之后不能被恢复，会清空所有的地表数据
	{CPUTimeT(Scene3D);
		if(!this->HasTerrainData())
			return;
		this->SetEnabled(Scene3D::Enable_TerrainData, false);
		for(int x=0;x<m_nMaxRegionWidth;x++){
			for(int y=0;y<m_nMaxRegionWidth;y++){
				m_sceneRegions[x + y * m_nMaxRegionWidth].DeleteTerrain();
			}
		}
	}
	VOID				Scene3D::CreateMoveGrids()//创建地表可移动格子数据,用于findway寻路操作,格子数量是每个普通格子占用16(4x4)个可移动格子数据
	{CPUTimeNone();
#ifdef EDITOR_OS
		if(IsEnabled(Enable_MoveGridsData))return;
		this->SetEnabled(Scene3D::Enable_MoveGridsData, true);
		for(int x=0;x<m_nMaxRegionWidth;x++){
			for(int y=0;y<m_nMaxRegionWidth;y++){
				m_sceneRegions[x + y * m_nMaxRegionWidth].CreateMoveGrids();
			}
		}
#endif
	}
	VOID				Scene3D::DeleteMoveGrids()//删除地表可移动格子数据,用于findway寻路操作
	{CPUTimeNone();
#ifdef EDITOR_OS
		if(!IsEnabled(Enable_MoveGridsData))return;
		this->SetEnabled(Scene3D::Enable_MoveGridsData, false);
		for(int x=0;x<m_nMaxRegionWidth;x++){
			for(int y=0;y<m_nMaxRegionWidth;y++){
				m_sceneRegions[x + y * m_nMaxRegionWidth].DeleteMoveGrids();
			}
		}
#endif
	}

	BOOL				Scene3D::CreateScene(float fGridSize, int nSceneSize)
	{CPUTimeT(Scene3D);
		m_nMaxRegionWidth	=	nSceneSize / REGION_GRID_COUNT_25d;
		int nSize = 1;
		for(int i=0;i<16;i++)
		{
			nSize *= 2;
			if(m_nMaxRegionWidth<=nSize)
			{
				m_nMaxRegionWidth = nSize;
				break;
			}
		}
		int nBoxLevel;
		nBoxLevel			=	m_nMaxRegionWidth / 2;
		INT nRegionCount	=	0;
		while(nBoxLevel > 0)
		{
			nRegionCount	+=	nBoxLevel * nBoxLevel;
			nBoxLevel		=	nBoxLevel / 2;
		}
		m_sceneRegions		=	new SceneBox[m_nMaxRegionWidth * m_nMaxRegionWidth];
		m_regions			=	new QuadBox[nRegionCount];
		INT nIndex			=	0;
		m_pRegionPtr		=	&m_regions[nIndex++];
		m_fGridSize			=	fGridSize;
		BuildRegions(m_pRegionPtr, NULL, nIndex, 1, 0, 0, 0);
		for(int x=0;x<m_nMaxRegionWidth;x++){
			for(int y=0;y<m_nMaxRegionWidth;y++){
				m_sceneRegions[x + y * m_nMaxRegionWidth].InitRegion(x, y);
			}
		}
		if(!this->m_pRegionPtr->Create(1))
			return false;
		LogInfo("system->create scene3d ok");
		SceneCameraData* cam	=	GetCameraDataPtr()->AddData(0);
		memcpy(cam, &GetCameraPtr()->GetCameraData(), sizeof(CameraDataLocal));
		cam->rotButton	=	Phantom::MouseButton_Right;
		cam->fMoveSpeed	=	To3DValue(1.0f);
		cam->vTarget.setxyz(GetWorldSize()/2.0f, GetWorldSize()/2.0f, 0);
		cam->vEye.setxyz(GetWorldSize()/2.0f - 10.0f, GetWorldSize()/2.0f-10.0f, -10.0f);
		cam->name		=	"current";
		cam->fFov		=	PI * 0.333333333f;
		cam->SetEnable(Phantom::CameraEnable_MoveHeightIsZ, false);
		SetActiveScene("current");
		return true;
	}
	INT					Scene3D::GetSceneRegionCount()
	{CPUTimeNone();
		return m_nMaxRegionWidth*m_nMaxRegionWidth;
	}
	SceneBox*			Scene3D::GetSceneRegionPtr(int index)
	{CPUTimeNone();
		if(index<0||index>=m_nMaxRegionWidth*m_nMaxRegionWidth)
			return 0;
		return &m_sceneRegions[index];
	}

	SceneBox*			Scene3D::GetSceneRegion(INT xRegion, INT yRegion)	//返回地形区域
	{CPUTimeNone();
		if(xRegion < 0 || yRegion < 0 || xRegion >= m_nMaxRegionWidth || yRegion >= m_nMaxRegionWidth)
			return NULL;
		return &m_sceneRegions[xRegion + yRegion * m_nMaxRegionWidth];
	}

	VOID	Scene3D::BuildRegions(QuadBox* pRegion, QuadBox* pRegionParent, INT& nIndex, INT nLevel, INT nBeginX, INT nBeginY, INT nLevelIndex)
	{CPUTimeNone();
		INT nWidth		=	m_nMaxRegionWidth / nLevel;
		pRegion->Setup(this, pRegionParent, nLevel, nBeginX, nBeginY, nWidth, m_fGridSize * (float)REGION_GRID_COUNT_25d, nLevelIndex);
		//
		nLevel				=	nLevel * 2;
		if(nLevel >= m_nMaxRegionWidth)
		{
			assert(nWidth == 2);
			pRegion->m_childRegion[0]	=	GetSceneRegion(nBeginX, nBeginY);
			pRegion->m_childRegion[1]	=	GetSceneRegion(nBeginX + 1, nBeginY);
			pRegion->m_childRegion[2]	=	GetSceneRegion(nBeginX, nBeginY + 1);
			pRegion->m_childRegion[3]	=	GetSceneRegion(nBeginX + 1, nBeginY + 1);
			//
			pRegion->m_childRegion[0]->Setup( this, pRegion, nLevel, nBeginX, nBeginY, nWidth / 2, this->m_fGridSize * (float)REGION_GRID_COUNT_25d, nLevelIndex + 1); 
			pRegion->m_childRegion[1]->Setup( this, pRegion, nLevel, nBeginX + 1, nBeginY, nWidth / 2, this->m_fGridSize * (float)REGION_GRID_COUNT_25d, nLevelIndex + 1); 
			pRegion->m_childRegion[2]->Setup( this, pRegion, nLevel, nBeginX, nBeginY + 1, nWidth / 2, this->m_fGridSize * (float)REGION_GRID_COUNT_25d, nLevelIndex + 1); 
			pRegion->m_childRegion[3]->Setup( this, pRegion, nLevel, nBeginX + 1, nBeginY + 1, nWidth / 2, this->m_fGridSize * (float)REGION_GRID_COUNT_25d, nLevelIndex + 1); 
			//定位
			return;
		}
		for(int i=0;i<QUAD_BOX_COUNT;i++)
			pRegion->m_childRegion[i]	=	&m_regions[nIndex++];
		//
		BuildRegions(pRegion->m_childRegion[0], pRegion, nIndex, nLevel, nBeginX, nBeginY, nLevelIndex + 1);
		BuildRegions(pRegion->m_childRegion[1], pRegion, nIndex, nLevel, nBeginX + nWidth / 2, nBeginY, nLevelIndex + 1);
		BuildRegions(pRegion->m_childRegion[2], pRegion, nIndex, nLevel, nBeginX, nBeginY + nWidth / 2, nLevelIndex + 1);
		BuildRegions(pRegion->m_childRegion[3], pRegion, nIndex, nLevel, nBeginX + nWidth / 2, nBeginY + nWidth / 2, nLevelIndex + 1);
	}

	BOOL	Scene3D::IsObjectFileExist(const char* szFileName, char* szOut)
	{CPUTimeNone();
		if(fexist(szFileName))
		{
			if(szOut)
				strcpy(szOut, szFileName);
			return true;
		}
		return false;
	}
	SceneObject*		Scene3D::CreateObject(MeshData::MeshType type)
	{CPUTimeNone();
		SceneObject* obj	=	new SceneObject(this);
		m_sceneObjects.push_back(obj);
		return obj;
	}
	SceneObject*		Scene3D::InsertObject(const char* szMeshFile, int nEventLineID)
	{CPUTimeNone();
		ScenePoint* point = GetActiveEnv()->GetPointByEventID(nEventLineID);
		if(point){
			SceneObjectFile* f = this->m_objectFilePtr.AddData(0);
			f->name = szMeshFile;
			f->name.setIsFile();
			return InsertObject(szMeshFile, point->GetPosition(), 0);
		}
		return 0;
	}
	SceneObject*		Scene3D::InsertObject(const char* szFile, const float3& pos, const matrix4x4* mat)
	{CPUTimeT(Scene3D);
		FileName file = szFile;
		file.setIsFile();
		SceneObjectFile* objFile = this->m_objectFilePtr.SearchDataByName(file.t);
		if(!objFile)return 0;
		int fileNameID = objFile->id;
		AutoPtr<Mesh> temp;
		if(!g_manager.LoadMesh(temp, objFile->name.t))
			return 0;
		SceneObject* obj	=	0;
		switch(temp->GetDataPtr()->m_meshType){
		case MeshData::MeshType_SceneUnit:
		case MeshData::MeshType_Role:
		case MeshData::MeshType_StaticMesh:
			{
				obj = new SceneObject(this);
			}
			break;
		default:
			return 0;
		}
		if(!obj->LoadObject(fileNameID, temp))
		{
			delete obj;
			return 0;
		}
		else
		{
			obj->SetPosition(pos);
			if(mat)
				obj->SetWorld(mat);
			obj->m_gameID.sceneid.index = m_sceneObjects.size();
			m_sceneObjects.push_back(obj);
		}
		if(obj)
			this->m_pRegionPtr->InsertObject(obj);
		return obj;
	}

	VOID	Scene3D::RemoveObject(SceneObject* obj)
	{CPUTimeT(Scene3D);
		for(int i=0;i<m_sceneObjects.size();i++)
		{
			if(m_sceneObjects[i] == obj)
			{
				if(m_selectObj==obj)
					m_selectObj = 0;
				if(m_cameraFollow==obj)
					m_cameraFollow = 0;
				delete m_sceneObjects[i];
				m_sceneObjects.eraseByIndex(i);
				this->m_classChanged = true;
				break;
			}
		}
	}
	VOID				Scene3D::Pause(BOOL b){CPUTimeNone();
		m_bPause = b;
	}
	VOID				Scene3D::SaveScene(const char* szFile){CPUTimeT(Scene3D);
		CSafeFileHelperW w;
		if(!w.create(szFile))
			return;
		int nVersion = 14;
		w << nVersion;
		unsigned int nHead = _MakeFileHead('s','c','f','k');
		w << nHead;
		//
		w << m_saveObjectID << m_fNormalVar << m_nMaxRegionWidth << m_fGridSize << m_nEnables << m_bIs2DFindway << m_sceneDiffuse;//漫射光色
		w << m_specularExponent << m_specularPower << m_fMaxViewDistance << m_activeCamera;
		w << m_activeSky << m_activeFog << m_skyActiveIndex;
		m_skyDataPtr.Save(w);
		m_cameraDataPtr.Save(w);
		m_objectFilePtr.Save(w);
		int numLayer = MAX_LAYER_TEXTURE;
		w << numLayer;
		for(int i=0;i<numLayer;i++){
			SceneTextureData& data = m_layerTextures[i];
			w << data;
			w.str(m_layerTextures[i].fileName);
		}
		m_pRegionPtr->Save(w, 1);

		w << m_nEventExist;
		m_defaultEnv.Save(w);
		int numEnv = m_sceneEnvs.size();
		w << numEnv;
		for(int i=0;i<m_sceneEnvs.size();i++){
			m_sceneEnvs[i]->Save(w);
		}
		w << m_saveLightID;
		w << m_bClipCamera << m_fClipCameraSize;
		w.pStream->write(this->m_regionList, sizeof(SceneRegionData)*MAX_REGION_COUNT);
		w << nHead;
	}
	BOOL	Scene3D::LoadScene(const char* szScene)
	{CPUTimeT(Scene3D);
		DestroyScene();
		CSafeFileHelperR r;
		if(!r.open(szScene))
			return false;
		int nVersion = 1;
		r >> nVersion;
		unsigned int nHead = _MakeFileHead('s','c','f','k');
		int readHead = 0;
		r >> readHead;
		if(nHead != readHead)
			return false;
		//
		r >> m_saveObjectID >> m_fNormalVar >> m_nMaxRegionWidth >> m_fGridSize >> m_nEnables >> m_bIs2DFindway >> m_sceneDiffuse;//漫射光色
		r >> m_specularExponent >> m_specularPower;
		if(nVersion<10){
			float worldBrightness,worldContrast;
			r >> worldBrightness >> worldContrast;
		}
		r >> m_fMaxViewDistance >> m_activeCamera;
		r >> m_activeSky >> m_activeFog >> m_skyActiveIndex;
		m_skyDataPtr.Load(r);
		if(nVersion<10){
			DataManager<SceneFogDataOld,Scene3D> fogDataPtr;
			fogDataPtr.Load(r);
		}
		m_cameraDataPtr.Load(r);
		m_objectFilePtr.Load(r);
		int numLayer = MAX_LAYER_TEXTURE;
		r >> numLayer;
		for(int i=0;i<numLayer;i++){
			SceneTextureData& data = m_layerTextures[i];
			r >> data;
			r.str(m_layerTextures[i].fileName);
			if(strstr(m_layerTextures[i].fileName.t, "e_"))
				m_layerTextures[i].bIsMask=true;
			else
				m_layerTextures[i].bIsMask=false;
		}
		///
		int nBoxLevel;
		nBoxLevel			=	m_nMaxRegionWidth / 2;
		INT nRegionCount	=	0;
		while(nBoxLevel > 0)
		{
			nRegionCount	+=	nBoxLevel * nBoxLevel;
			nBoxLevel		=	nBoxLevel / 2;
		}
		m_sceneRegions		=	new SceneBox[m_nMaxRegionWidth * m_nMaxRegionWidth];
		m_regions			=	new QuadBox[nRegionCount];
		INT nIndex			=	0;
		m_pRegionPtr		=	&m_regions[nIndex++];
		m_fileName			=	szScene;
		m_staticMeshs.SetArrayCount(m_objectFilePtr.GetDataCount());
		BuildRegions(m_pRegionPtr, NULL, nIndex, 1, 0, 0, 0);
		assert( nIndex == nRegionCount );
		for(int x=0;x<m_nMaxRegionWidth;x++){
			for(int y=0;y<m_nMaxRegionWidth;y++){
				m_sceneRegions[x + y * m_nMaxRegionWidth].InitRegion(x, y);
			}
		}
		if(!this->m_pRegionPtr->Load(r, 1))
			return false;
		if(nVersion<6){
			int numEvent = 0;
			r >> numEvent;
			if(numEvent>0){
				this->m_defaultEnv.m_points.resize(numEvent);
				for(int i=0;i<numEvent;i++)
				{
					m_defaultEnv.m_points[i] = new ScenePoint(this);
					m_defaultEnv.m_points[i]->Load(r);
				}
			}
		}
		if(nVersion>=3)
			r >> m_nEventExist;
		//
		if(nVersion>=5)
			m_defaultEnv.Load(r);
		if(nVersion>=4){
			int numEnv = 0;
			r >> numEnv;
			for(int i=0;i<numEnv;i++){
				SceneEnv* env = new SceneEnv(this);
				env->Load(r);
				m_sceneEnvs.push_back(env);
			}
		}
		if(nVersion<=12){
			SceneLight* temp = new SceneLight(this);
			temp->Load(r);
			r >> m_saveLightID;
			int numLight;
			r >> numLight;
			for(int i=0;i<numLight;i++){
				temp->Load(r);
			}
			delete temp;
		}
		else
			r >> m_saveLightID;
		if(nVersion>=8)
			r >> m_bClipCamera >> m_fClipCameraSize;
		if(nVersion>=9)
			r.pStream->read(this->m_regionList, sizeof(SceneRegionData)*MAX_REGION_COUNT);
		r >> readHead;
		if(nHead != readHead)
			return false;
		LogInfo("system->load scene3d %s ok", m_fileName.t);
		SetActiveScene("current");
		this->ActiveEnv(0);
		return true;
	}
	SceneObject*	Scene3D::CloneAndBind(SceneObject* obj2){
		SceneObject* obj = obj2->Clone();
		int lineID = obj->GetMeshPtr()->GetDataPtr()->m_lineID;
		if(lineID<=0)return obj;
		int meshType = obj->GetMeshPtr()->GetDataPtr()->m_meshType;
		switch(meshType){
		case MeshData::MeshType_Role://如果是角色
			{
				obj->m_bShowBlood = true;
				GameBornData data;
				data.lineID = lineID;
				data.pos = obj->GetPosition();
				data.rot = obj->m_fRotationZ;
				data.sceneid = obj->m_gameID.sceneid;
				data.range = obj->m_fRange;
				MeshData* md = obj->GetMeshPtr()->GetDataPtr();
				data.height	=	obj->GetObjectHeight();
				for(int k=0;k<md->m_actionMap.size();k++){
					MeshActionRange* act = md->m_actionMap[k];
					if(act->m_actionType==Phantom::ActionType_Walk)
						data.walkSpeed = act->m_fMoveOffset;
					else if(act->m_actionType==Phantom::ActionType_Run)
						data.runSpeed = act->m_fMoveOffset;
				}
				GameObject* obj2 = GetGameMgr()->CreateObject(data.lineID);
				if(!obj2){
					LogInfo("system->error:(id=%d)%s", data.lineID, utf8("不能初始化GameObject对象,在roles.txt中找不到对应的行ID"));
					break;
				}
				obj2->Setup(data);
				obj->m_gameID.gameid = obj2->GetID().gameid;
				//GameSceneID temp;
				//GetGameMgr()->send(GameMessage::ToGame_Born, temp, &data, sizeof(GameBornData));
			}
			break;
		}
		return obj;
	}
	VOID			Scene3D::BindAllObjects()//绑定所有的角色
	{CPUTimeT(Scene3D);
		for(int i=0;i<this->m_sceneObjects.size();i++){
			SceneObject* obj = m_sceneObjects[i];
			//obj->m_sceneID.id = obj->m_id;
			obj->m_gameID.sceneid.index = i;
			int lineID = obj->GetMeshPtr()->GetDataPtr()->m_lineID;
			if(lineID<=0)continue;
			int meshType = obj->GetMeshPtr()->GetDataPtr()->m_meshType;
			switch(meshType){
			case MeshData::MeshType_Role://如果是角色
				{
					obj->m_bShowBlood = true;
					GameBornData data;
					data.lineID = lineID;
					data.pos = obj->GetPosition();
					data.rot = obj->m_fRotationZ;
					data.sceneid = obj->m_gameID.sceneid;
					data.range = obj->m_fRange;
					MeshData* md = obj->GetMeshPtr()->GetDataPtr();
					data.height	=	obj->GetObjectHeight();
					for(int k=0;k<md->m_actionMap.size();k++){
						MeshActionRange* act = md->m_actionMap[k];
						if(act->m_actionType==Phantom::ActionType_Walk)
							data.walkSpeed = act->m_fMoveOffset;
						else if(act->m_actionType==Phantom::ActionType_Run)
							data.runSpeed = act->m_fMoveOffset;
					}
					GameObject* obj2 = GetGameMgr()->CreateObject(data.lineID);
					if(!obj2){
						LogInfo("system->error:(id=%d)%s", data.lineID, utf8("不能初始化GameObject对象,在roles.txt中找不到对应的行ID"));
						break;
					}
					obj2->Setup(data);
					obj->m_gameID.gameid = obj2->GetID().gameid;
					//GameSceneID temp;
					//GetGameMgr()->send(GameMessage::ToGame_Born, temp, &data, sizeof(GameBornData));
				}
				break;
			}
		}
	}
	VOID			Scene3D::OnMessage(GameMessage* msg)
	{CPUTimeT(Scene3D);
		SceneObject* obj = SearchObject(msg->uid.sceneid);
		if(!obj)return;
		obj->m_gameID.gameid = msg->uid.gameid;
		streamRead r((const char*)msg->GetDataPtr(), msg->length);
		switch(msg->mid){
		case GameMessage::ToScene_PosRot:
			{
				float4* pos = (float4*)msg->GetDataPtr();
				obj->Change(float3(pos->x, pos->y, pos->z), pos->w);
			}
			break;
		case GameMessage::ToScene_Rot:
			{
				float* rot = (float*)msg->GetDataPtr();
				obj->SetRotation(*rot);
			}
			break;
		case GameMessage::ToScene_Action:
			{
				char action,aindex;
				r >> action >> aindex;
				obj->SetAction((ActionType)action, aindex);
			}
			break;
		case GameMessage::ToScene_Attack:
			{
				char action,aindex;
				float fTime;
				r >> action >> aindex >> fTime;
				obj->SetAttack((ActionType)action, aindex, fTime);
			}
			break;
		case GameMessage::ToScene_Hit:
			{
				//int hit = 0;
				//float fTime = 0;
				//r >> hit >> fTime;
				//obj->AddHit(hit, fTime);
			}
			break;
		case GameMessage::ToScene_BloodProgress:
			{
				float fProg = 0;
				r >> fProg;
				obj->SetBlood(fProg);
			}
			break;
		case GameMessage::ToScene_ResetID:
			{
				obj->SetVisible(false);
				obj->m_gameID.sceneid.id = this->GetNewIdentity();
				GetGameMgr()->send(GameMessage::ToGame_ResetID, obj->m_gameID, 0, 0);
			}
			break;
		case GameMessage::ToScene_Reborn:
			{
				float3 pos;
				r >> pos;
				obj->Reborn(pos);
			}
			break;
		case GameMessage::ToScene_Die:
			{
				char action,aindex;
				r >> action >> aindex;
				obj->SetDie((ActionType)action, aindex);
			}
			break;
		case GameMessage::ToScene_Skill:
			{
				GameSceneID targetid;
				float3 tpos;
				char toTarget;
				unsigned short meshfileindex;
				float fContinueTime;
				r >> meshfileindex >> targetid >> tpos >> toTarget >> fContinueTime;
				SceneGameSkill* skill = GetFreeGameSkill(meshfileindex);
				if(skill){
					switch(toTarget)
					{
					case SceneGameSkill::SGS_ToTarget:
						{
							SceneObject* target = SearchObject(targetid.sceneid);
							if(!target)//如果没有目标则无法继续
								break;
							SceneObject* src = SearchObject(msg->uid.sceneid);
							skill->StartSkill(src, target, fContinueTime);
						}
						break;
					case SceneGameSkill::SGS_InSource:
						{
							SceneObject* src = SearchObject(targetid.sceneid);
							if(!src)//如果没有目标则无法继续
								break;
							skill->StartSkill(src, fContinueTime);
						}
						break;
					case SceneGameSkill::SGS_InPoint:
						{
							SceneObject* src = SearchObject(targetid.sceneid);
							if(!src)//如果没有目标则无法继续
								break;
							skill->StartSkill(tpos, fContinueTime);
						}
						break;
					}
				}
			}
			break;
		}
	}
	void			Scene3D::StartSkill(GameSceneID& srcid, GameSceneID& targetid, float3 tpos, char toTarget, unsigned short meshfileindex, float fContinueTime)
	{
		SceneGameSkill* skill = GetFreeGameSkill(meshfileindex);
		if(skill){
			switch(toTarget)
			{
			case SceneGameSkill::SGS_ToTarget:
				{
					SceneObject* target = SearchObject(targetid.sceneid);
					if(!target)//如果没有目标则无法继续
						break;
					SceneObject* src = SearchObject(srcid.sceneid);
					skill->StartSkill(src, target, fContinueTime);
				}
				break;
			case SceneGameSkill::SGS_InSource:
				{
					SceneObject* src = SearchObject(targetid.sceneid);
					if(!src)//如果没有目标则无法继续
						break;
					skill->StartSkill(src, fContinueTime);
				}
				break;
			case SceneGameSkill::SGS_InPoint:
				{
					SceneObject* src = SearchObject(targetid.sceneid);
					if(!src)//如果没有目标则无法继续
						break;
					skill->StartSkill(tpos, fContinueTime);
				}
				break;
			}
		}
	}
	VOID			Scene3D::SetCameraPos(const Pixel& pos)
	{CPUTimeNone();
		this->m_cameraPos = pos;
		float3 p(pos.x, pos.y, 0);
		p.z = this->GetHeight(p);
		float3 dir = GetCameraPtr()->GetEye() - GetCameraPtr()->GetTarget();
		GetCameraPtr()->SetTarget(p);
		GetCameraPtr()->SetEye(GetCameraPtr()->GetTarget() + dir);
	}
	int	ObjectCompare(const void* v1, const void* v2){
		SceneObject* so1 = *(SceneObject**)v1;
		SceneObject* so2 = *(SceneObject**)v2;
		if(so1->m_nClass<so2->m_nClass)
			return -1;
		if(so1->m_nClass>so2->m_nClass)
			return 1;
		return 0;
	}
	VOID			Scene3D::ResortClass()
	{CPUTimeT(Scene3D);
		if(m_sceneObjects.size()>1){
			qsort(m_sceneObjects.GetPtr(), m_sceneObjects.size(), sizeof(SceneObject*), ObjectCompare);
			for(int i=0;i<MAX_CLASS_IDs;i++){
				m_classIDs[i].begin = -1;
				m_classIDs[i].count = 0;
			}
			for(int i=0;i<m_sceneObjects.size();i++)
			{
				int c = m_sceneObjects[i]->m_nClass;
				if(c<0)c=0;
				else if(c>=MAX_CLASS_IDs)
					c = MAX_CLASS_IDs - 1;
				if(m_classIDs[c].begin==-1)
					m_classIDs[c].begin = i;
				m_classIDs[c].count++;
			}
			m_classChanged = false;
		}
	}
	INT				Scene3D::GetClassCount(int nID)
	{CPUTimeNone();
		if(m_classChanged)
			ResortClass();
		if(nID<0)nID=0;
		else if(nID>MAX_CLASS_IDs)
			nID = MAX_CLASS_IDs-1;
		return this->m_classIDs[nID].count;
	}
	SceneObject*	Scene3D::GetClassPtr(int nID, int nIndex)
	{CPUTimeNone();
		if(m_classChanged)
			ResortClass();
		if(nID<0)nID=0;
		else if(nID>MAX_CLASS_IDs)
			nID = MAX_CLASS_IDs-1;
		if(nIndex<0||nIndex>=m_classIDs[nID].count)
			return 0;
		return this->m_sceneObjects[this->m_classIDs[nID].begin + nIndex];
	}
	VOID			Scene3D::RebuildTypes()
	{CPUTimeNone();
		//for(int i=0;i<this->m_sceneObjects.size();i++){
		//	SceneObject* obj = m_sceneObjects[i];
		//	//if(obj->m_type == StaticObjectData::Type_Monster)
		//	//{
		//	//	for(int i=0;i<m_monsters.size();i++)
		//	//		if(m_monsters[i] == obj)
		//	//			return;
		//	//	m_monsters.push_back(obj);
		//	//}
		////	else if(obj->m_type == StaticObjectData::Type_Npc)
		////	{
		////		for(int i=0;i<m_npcs.size();i++)
		////			if(m_npcs[i] == obj)
		////				return;
		////		m_npcs.push_back(obj);
		////	}
		//}
	}
	VOID			Scene3D::AddObjectType(SceneObject* obj)
	{CPUTimeNone();
		//if(obj->m_type == StaticObjectData::Type_Monster)
		//{
		//	for(int i=0;i<m_monsters.size();i++)
		//		if(m_monsters[i] == obj)
		//			return;
		//	m_monsters.push_back(obj);
		//}
		//else if(obj->m_type == StaticObjectData::Type_Npc)
		//{
		//	for(int i=0;i<m_npcs.size();i++)
		//		if(m_npcs[i] == obj)
		//			return;
		//	m_npcs.push_back(obj);
		//}
	}
	//SceneObject*	Scene3D::SearchMonster(INT nRange, LPCSTR szFunc)
	//{
	//	//for(int i=0;i<this->m_monsters.size();i++)
	//	//{

	//	//}
	//	return 0;
	//}
	//SceneObject*	Scene3D::SearchNpc(INT nRange, LPCSTR szFunc)
	//{
	//	for(int i=0;i<this->m_npcs.size();i++)
	//	{
	//	}
	//	return 0;
	//}

	Mesh*			Scene3D::GetStaticMeshPtr(int id, char& bIsClone, char& bIsFramMove)
	{CPUTimeT(Scene3D);
		bIsFramMove = false;
		bIsClone = false;
		int index = -1;
		for(int i=0;i<m_objectFilePtr.GetDataCount();i++)
		{
			if(m_objectFilePtr.GetDataPtr(i)->id == id)
			{
				index = i;
				break;
			}
		}
		if(index<0||index>=this->m_staticMeshs.size())
			return 0;
		if(m_staticMeshs[index].bFail)
			return 0;
		if(!m_staticMeshs[index].mesh)
		{
			AutoPtr<Mesh> temp;
			SceneObjectFile* filePtr = this->m_objectFilePtr.GetDataPtr(index);
			if(!filePtr || !g_manager.LoadMesh(temp, filePtr->name.t))
			{
				m_staticMeshs[index].bFail = true;
				return 0;
			}
			//temp->RemoveBoneIfOne();
			//temp = temp->GetDataPtr()->CreateRuntime();
			m_staticMeshs[index].mesh = temp;
			safe_addref(m_staticMeshs[index].mesh);
			m_staticMeshs[index].nCount = 0;
		}
		int oldCount = m_staticMeshs[index].nCount;
		m_staticMeshs[index].nCount++;
		MeshData* data = m_staticMeshs[index].mesh->GetDataPtr();
		if(data->m_particles.size()>0||data->m_ribbons.size()>0||(data->GetSubmeshCount()>0&&data->GetSubmeshPtr(0)->m_bUseWorld))
		{
			if(oldCount==0)
			{
				bIsFramMove = true;
				return m_staticMeshs[index].mesh;
			}
			bIsFramMove = true;
			bIsClone = true;
			return m_staticMeshs[index].mesh->GetDataPtr()->CreateRuntime();
		}
		return m_staticMeshs[index].mesh;
	}
	float			Scene3D::GetHeight(const float3& pos)
	{CPUTimeT(Scene3D);
		INT nGridX		=	(INT)(pos.x / this->m_fGridSize);
		INT nGridY		=	(INT)(pos.y / this->m_fGridSize);
		if(nGridX < 0 || nGridY < 0 || nGridX >= GetNumofGridW() || nGridY >= GetNumofGridW())
			return 0;
		INT xRegion	=	nGridX / REGION_GRID_COUNT_25d;
		INT yRegion	=	nGridY / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		if(!pRegion)
			return 0;
		float3 orig(pos.x, pos.y, 100000.0f);
		float3 dir(0, 0, -1);
		PickPoint data;
		data.t = orig.z;
		float fRet = 100000.0f;
		BOOL bObjH = this->GetHeight(pos, fRet);
		if(pRegion->IntersectAll(nGridX % REGION_GRID_COUNT_25d, nGridY % REGION_GRID_COUNT_25d, orig, dir, data))
		{
			if(bObjH)
				return getmax(data.pos.z, fRet);
			return data.pos.z;
		}
		else if(bObjH)
			return fRet;
		return 0;
	}
	//VOID			Scene3D::SetActiveFog(SceneFogData* fog)
	//{
	//	if(fog)
	//	{
	//		this->m_activeFog.id = fog->id;
	//		g_manager.UpdateFog(fog);
	//	}
	//	else
	//	{
	//		this->m_activeFog.id = 0;
	//		g_manager.UpdateFog(0);
	//	}
	//}
	VOID			Scene3D::SetActiveScene(const char* szActiveCamera)
	{CPUTimeNone();
		SetActiveCamera(szActiveCamera);
	}
	VOID			Scene3D::SetActiveCamera(const char* szActiveCamera)
	{CPUTimeNone();
		SceneCameraData* data	=	m_cameraDataPtr.SearchDataByName(szActiveCamera);
		if(data)
		{
			GetCameraPtr()->SetCameraConfig(*data);
			m_cameraPos.Set(data->vTarget.x, data->vTarget.y);
			LogInfo("system->active camera %s", data->name.t);
		}
		else
		{
			LogInfo("system->error not exist camera %s", szActiveCamera);
		}
#ifdef EDITOR_OS
		if(strstr(szActiveCamera, "game")==0)
			return;
#endif
		GetCameraPtr()->m_bArea = this->m_bClipCamera;
		float center = this->GetWorldSize()/2;
		float fmin = center - (center*this->m_fClipCameraSize);
		float fmax = center + (center*this->m_fClipCameraSize);
		Rect rc(fmin,fmin,fmax,fmax);
		GetCameraPtr()->SetArea(&rc);
	}
	VOID			Scene3D::OnFrameMove()
	{CPUTimeT(Scene3D);
		if(m_bPause)return;
		SceneEnv* env = this->GetActiveEnv();
		g_manager.m_lightColor = env->m_defaultLight.m_lightColor;
		g_manager.m_lightDir = -env->m_defaultLight.m_dir;
		g_manager.m_lightEnable = env->m_defaultLight.m_bEnabled;
		//if(env->m_shadowMode==SceneEnv::ShadowMode_Object){
			SceneObject* obj = this->SearchObject(env->m_shadowCenterObj);
			if(obj)
				g_manager.m_lightPos = obj->GetPosition();
		//}
		//else if(env->m_shadowMode==SceneEnv::ShadowMode_Center)
			else{
				if((int)env->m_shadowCenter.x==0&&(int)env->m_shadowCenter.y==0&&(int)env->m_shadowCenter.z==0){
					float3 dir = GetCameraPtr()->GetTarget() - GetCameraPtr()->GetEye();
					float fR = dir.R();
					dir *= (GetCameraPtr()->GetEye().z / -dir.z);
					g_manager.m_lightPos = GetCameraPtr()->GetEye() + dir;
				}
				else
					g_manager.m_lightPos = env->m_shadowCenter;
			}
		//else if(env->m_shadowMode==SceneEnv::ShadowMode_Front)
		//{
		//	float3 vz(0,0,-100);//, 0, 0);
		//	vz.transform(vz, GetCameraPtr()->m_matViewInverse);
		//	vz.z = 0;
		//	float rot = getRotateZ(vz);
		//	vz.setxyz(0, -env->m_shadowFront, 0);
		//	vz.RotateZ(vz, rot);
		//	g_manager.m_lightPos = vz + GetCameraPtr()->m_matViewInverse.GetPosition();
		//}
		//
		if(this->m_pRegionPtr)
			m_pRegionPtr->FrameMoveAllLevels(GetCameraPtr());
		if(this->m_cameraFollow)
		{
			const float3& pos = m_cameraFollow->GetPosition();
			float3 dir = GetCameraPtr()->GetEye() - GetCameraPtr()->GetTarget();
			GetCameraPtr()->SetTarget(pos);
			GetCameraPtr()->SetEye(pos + dir);
		}
		for(int i=0;i<this->m_skills.size();i++)
			m_skills[i]->OnFrameMove();
		for(int i=0;i<this->m_gameSkills.size();i++)
			m_gameSkills[i]->OnFrameMove();
	}
	VOID			Scene3D::RenderToShadowMap()
	{CPUTimeT(Scene3D);
		if(!this->IsEnabled(Scene3D::Enable_ShowScene))
			return;
		if(this->m_pRegionPtr)
			m_pRegionPtr->RenderToShadowMap();
	}
	VOID			Scene3D::RenderScene()
	{CPUTimeT(Scene3D);
		if(!this->IsEnabled(Scene3D::Enable_ShowScene))
			return;
		GetCameraPtr()->onChangeView();
		if(this->m_pRegionPtr)
			m_pRegionPtr->RenderAllLevels(GetCameraPtr());
		for(int i=0;i<this->m_skills.size();i++)
			m_skills[i]->OnRender();
		for(int i=0;i<this->m_gameSkills.size();i++)
			m_gameSkills[i]->OnRender();
	}
#ifdef EDITOR_OS
	VOID					Scene3D::OnChangeWorld(SceneObject* obj)
	{CPUTimeNone();
		if(!m_bChangeEnable)return;
		if(this->m_activeEnv!=&m_defaultEnv&&m_activeEnv!=0)
			m_activeEnv->AddObject(obj);
		else{
			obj->ResetSaveEnv();
		}
	}
#endif
	VOID					Scene3D::GetObjectByName(const char* szName, CDynamicArray<SceneObject*>& rets)//返回相关所有对象数组
	{CPUTimeT(Scene3D);
		for(int i=0;i<m_sceneObjects.size();i++)
		{
			if(m_sceneObjects[i]->m_name == szName)
				rets.push_back(m_sceneObjects[i]);
		}
	}
	SceneObject*		Scene3D::SearchObject(SceneID& id)
	{CPUTimeT(Scene3D);
		if(id.id<=0)return 0;
		if(id.index>=0&&id.index<this->m_sceneObjects.size()&&m_sceneObjects[id.index]->m_gameID.sceneid.id==id.id)
			return m_sceneObjects[id.index];
		for(int i=0;i<m_sceneObjects.size();i++){
			if(m_sceneObjects[i]->m_gameID.sceneid.id==id.id){
				id.index = i;
				return m_sceneObjects[i];
			}
		}
		id.id=0;
		return 0;
	}
	SceneObject*		Scene3D::GetObject(PtrID& id)
	{CPUTimeNone();
		if(id.id==0)
			return 0;
		if(id.index<m_sceneObjects.size()&&m_sceneObjects[id.index]->m_gameID.sceneid.id==id.id)
			return m_sceneObjects[id.index];
		id.index = 0;
		for(int i=0;i<m_sceneObjects.size();i++)
		{
			if(m_sceneObjects[i]->m_gameID.sceneid.id == id.id)
			{
				id.index = i;
				id.id = m_sceneObjects[i]->m_gameID.sceneid.id;
				return m_sceneObjects[id.index];
			}
		}
		id.id = 0;
		return m_sceneObjects[id.index];
	}
	SceneObject*		Scene3D::GetObjectByContentID(INT id)
	{
		for(int i=0;i<m_sceneObjects.size();i++)
		{
			if(m_sceneObjects[i]->m_nContentID == id)
				return m_sceneObjects[i];
		}
		return 0;
	}
	SceneObject*		Scene3D::GetObjectByID(INT id)
	{CPUTimeT(Scene3D);
		for(int i=0;i<m_sceneObjects.size();i++)
		{
			if(m_sceneObjects[i]->m_gameID.sceneid.id == id)
				return m_sceneObjects[i];
		}
		return 0;
	}
	SceneObject*		Scene3D::GetObjectByName(const char* szName)
	{CPUTimeT(Scene3D);
		for(int i=0;i<m_sceneObjects.size();i++)
		{
			if(m_sceneObjects[i]->m_name == szName)
				return m_sceneObjects[i];
		}
		return 0;
	}
	SceneObject*		Scene3D::GetObjectPtr(int index)
	{CPUTimeNone();
		if(index < 0 || index >= m_sceneObjects.size())
			return 0;
		return m_sceneObjects[index];
	}
	INT						Scene3D::GetObjectCount()
	{CPUTimeNone();
		return m_sceneObjects.size();
	}
	SceneObject*			Scene3D::CloneObject(SceneObject* obj)
	{CPUTimeNone();
		return obj->Clone();
	}
	VOID					Scene3D::RebuildLight(LightAffectData& light, const float3& pos, float fsize)
	{CPUTimeT(Scene3D);
		//float3 dir = pos - this->m_lastLightPos;
		//if(dir.length()>To3DValue(1.0f))//每隔一米计算一次光源
		//{
		//	m_lastLightPos = m_worldMat.GetPosition();
		//	m_lightCount = 0;
		//}
		SceneEnv* env = this->GetActiveEnv();
		light.count = 0;
		const float fRebuildLength = To3DValue(1.0f);//每1秒距离重新创建一次
		for(int i=1;i<env->m_lights.size();i++){
			SceneLight* lightPtr = env->m_lights[i];
			if(!lightPtr->m_bEnabled)
				continue;
			float3 dir = pos - lightPtr->m_pos;
			float fLen = dir.length()-fsize-fRebuildLength;
			float fLightRange = lightPtr->m_lightFar;
			if(fLen < fLightRange){
				light.lightPos[light.count].setXYZW(lightPtr->m_pos.x,lightPtr->m_pos.y,lightPtr->m_pos.z,lightPtr->m_lightType);
				light.lightColor[light.count].setXYZW(lightPtr->m_lightColor.x,lightPtr->m_lightColor.y,lightPtr->m_lightColor.z,1);
				light.rangeAngle[light.count].setXYZW(lightPtr->GetNear(), lightPtr->GetFar()-lightPtr->GetNear(), lightPtr->m_lightAngleSmall, lightPtr->m_lightAngleBig);
				light.count++;
				if(light.count>=MAX_LIGHT_COUNT)
					break;
			}
		}
	}
	BOOL					Scene3D::FindPath(CDynamicArrayS<WayPoint2D>& ways, const float3& from, const float3& to)
	{CPUTimeT(Scene3D);
		if(!this->IsEnabled(this->Enable_MoveGridsData))
			return false;
		m_astarcb.m_mapPtr = this;
		m_astar.setupMap(&this->m_astarcb);
		float fMoveGridSize = m_fGridSize;
		Pixel begin(from.x/fMoveGridSize, from.y/fMoveGridSize);
		Pixel end(to.x/fMoveGridSize, to.y/fMoveGridSize);
		Pixel last = begin;
		BOOL bBeginD = false;
		if(!m_astarcb.IsAstarMoveEnable(begin.x, begin.y))
		{
			bBeginD = true;
			int num = 1;
			while(true){
				bool bfind = false;
				for(int x=-num;x<=num;x++)
				{
					for(int y=-num;y<=num;y++)
					{
						if(m_astarcb.IsAstarMoveEnable(begin.x+x,begin.y+y))
						{
							begin.Set(begin.x+x, begin.y+y);
							bfind = true;
							break;
						}
					}
					if(bfind)break;
				}
				if(bfind)
					break;
				num++;
				if(num>=10)
					return false;
			}
		}
		if(!m_astar.find_linear(begin.x, begin.y, end.x, end.y, m_bIs2DFindway))
			return false;
		float2 vfrom(from.x, from.y);
		float2 vto(to.x, to.y);
		float2 vnewb((float)begin.x*fMoveGridSize+fMoveGridSize*0.5f, (float)begin.y*fMoveGridSize+fMoveGridSize*0.5f);
		if(m_astar.getfindPath().getNumPath() <= 2)
		{
			ways.SetArrayCount(2+(bBeginD?1:0));
			ways[0].Setup(vfrom);
			if(bBeginD)
			{
				ways[1].Setup(vnewb);
				ways[2].Setup(vto);
			}
			else
				ways[1].Setup(vto);
			return true;
		}
		int index = 0;
		ways.SetArrayCount(m_astar.getfindPath().getNumPath()+5);
		ways[index++].Setup(vfrom);
		if(bBeginD)
		{
			ways[index++].Setup(vnewb);
			last = begin;
		}
		for(int i= m_astar.getfindPath().getNumPath() - 1;i >= 0; i--)// - 1; i > 0; i--)
		{
			Pixel* p	=	m_astar.getfindPath().getPath(i);
			if(p->x==last.x&&p->y==last.y)
				continue;
			last = *p;
			float2 v((float)p->x*fMoveGridSize+fMoveGridSize*0.5f, (float)p->y*fMoveGridSize+fMoveGridSize*0.5f);
			ways[index++].Setup(v);
		}
		if(last.x!=end.x&&last.y!=end.y)
			ways[index++].Setup(vto);
		ways.SetArrayCount(index);
		return true;
	}
	SceneStaticMesh*		Scene3D::SearchStaticObject(int nID)
	{CPUTimeT(Scene3D);
		return this->m_pRegionPtr->SearchMesh(nID);
	}
	VOID					Scene3D::SetCameraFollow(SceneObject* obj)
	{CPUTimeNone();
		this->m_cameraFollow = obj;
	}
	VOID					Scene3D::SetGridRegionIndex(int mx, int my, int index)
	{CPUTimeNone();
		if(mx<0||my<0|| mx >= GetMoveGridsW() || my >= GetMoveGridsW())return;
		INT xRegion	=	mx / REGION_GRID_COUNT_25d;
		INT yRegion	=	my / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	&m_sceneRegions[xRegion + yRegion * m_nMaxRegionWidth];
		int x = mx%REGION_GRID_COUNT_25d;
		int y = my%REGION_GRID_COUNT_25d;
		if(pRegion->m_moveGrids)
			pRegion->m_moveGrids[x+y*REGION_GRID_COUNT_25d].SetRegionType(index);
	}
	VOID					Scene3D::SetMoveGridEnable(int mx, int my, BOOL bEnable)
	{CPUTimeNone();
		if(mx<0||my<0|| mx >= GetMoveGridsW() || my >= GetMoveGridsW())return;
		INT xRegion	=	mx / REGION_GRID_COUNT_25d;
		INT yRegion	=	my / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	&m_sceneRegions[xRegion + yRegion * m_nMaxRegionWidth];
		int x = mx%REGION_GRID_COUNT_25d;
		int y = my%REGION_GRID_COUNT_25d;
		if(pRegion->m_moveGrids)
			pRegion->m_moveGrids[x+y*REGION_GRID_COUNT_25d].setMoveEnable(bEnable);
	}
	GMoveData*				Scene3D::GetMoveAttribute(INT mx, INT my)
	{CPUTimeNone();
		if(mx<0||my<0|| mx >= GetMoveGridsW() || my >= GetMoveGridsW())return 0;
		INT xRegion	=	mx / REGION_GRID_COUNT_25d;
		INT yRegion	=	my / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	&m_sceneRegions[xRegion + yRegion * m_nMaxRegionWidth];
		int x = mx%REGION_GRID_COUNT_25d;
		int y = my%REGION_GRID_COUNT_25d;
		if(pRegion->m_moveGrids)
			return &pRegion->m_moveGrids[x+y*REGION_GRID_COUNT_25d];
		return 0;
	}
	BOOL					Scene3D::IsMoveGridEnable(const float3& pos)
	{CPUTimeNone();
		float fMove = GetMoveGridSize();
		int mx = (int)(pos.x/fMove);
		int my = (int)(pos.y/fMove);
		return IsMoveGridEnable(mx, my);
	}
	int						Scene3D::GetRegionIndex(const float3& pos)
	{CPUTimeNone();
		float fMove = GetMoveGridSize();
		int mx = (int)(pos.x/fMove);
		int my = (int)(pos.y/fMove);
		if(mx<0||my<0|| mx >= GetMoveGridsW() || my >= GetMoveGridsW())return 0;
		INT xRegion	=	mx / REGION_GRID_COUNT_25d;
		INT yRegion	=	my / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	&m_sceneRegions[xRegion + yRegion * m_nMaxRegionWidth];
		int x = mx%REGION_GRID_COUNT_25d;
		int y = my%REGION_GRID_COUNT_25d;
		if(pRegion->m_moveGrids)
			return pRegion->m_moveGrids[x+y*REGION_GRID_COUNT_25d].GetRegionType();
		return 0;
	}
	BOOL					Scene3D::IsMoveGridEnable(int mx, int my)
	{CPUTimeNone();
		if(mx<0||my<0|| mx >= GetMoveGridsW() || my >= GetMoveGridsW())return false;
		INT xRegion	=	mx / REGION_GRID_COUNT_25d;
		INT yRegion	=	my / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	&m_sceneRegions[xRegion + yRegion * m_nMaxRegionWidth];
		int x = mx%REGION_GRID_COUNT_25d;
		int y = my%REGION_GRID_COUNT_25d;
		if(pRegion->m_moveGrids)
			return pRegion->m_moveGrids[x+y*REGION_GRID_COUNT_25d].isMoveEnable();
		return true;
	}
	int						Scene3DAstar::IsAstarMoveEnable(int x, int y)
	{CPUTimeNone();
		return m_mapPtr->IsMoveGridEnable(x,y);
	}
	int						Scene3DAstar::GetAstarWidth()
	{CPUTimeNone();
		return this->m_mapPtr->GetMoveGridsW();
	}
	int						Scene3DAstar::GetAstarHeight()
	{CPUTimeNone();
		return this->m_mapPtr->GetMoveGridsW();
	}
	BOOL					Scene3D::GetGridMiddle(INT nGridX, INT nGridY, float3& out)	//返回单元格中心点
	{CPUTimeNone();
		if(nGridX < 0 || nGridY < 0 || nGridX >= GetMaxGridCount() || nGridY >= GetMaxGridCount())
			return FALSE;
		int xRegion	=	nGridX / REGION_GRID_COUNT_25d;
		int yRegion	=	nGridY / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		if(!pRegion)
			return FALSE;
		GVertexData* pAttr	=	pRegion->GetVertexAttributePtr(nGridX % REGION_GRID_COUNT_25d, nGridY % REGION_GRID_COUNT_25d);
		if(!pAttr)
			return FALSE;
		out			=	float3((float)nGridX * m_fGridSize + m_fGridSize * 0.5f, (float)nGridY * m_fGridSize + m_fGridSize * 0.5f, pAttr->GetHeight());
		return TRUE;
	}
	SceneTextureRuntime*	Scene3D::GetLayerTextureData(int nLayer)
	{CPUTimeNone();
		if(nLayer>=MAX_LAYER_TEXTURE||nLayer<0)return 0;
		return &m_layerTextures[nLayer];
	}
	Texture*				Scene3D::GetLayerTexture(int nLayer)
	{CPUTimeNone();
		if(nLayer < 0 || nLayer >= MAX_LAYER_TEXTURE)
			return 0;
		SceneTextureRuntime& t	=	m_layerTextures[nLayer];
		if(!t.bExist || !t.fileName.t[0])
			return 0;
		if(!t.texturePtr)
		{
			g_manager.LoadTexture(&t.texturePtr, t.fileName.t, true, Phantom::TextureFilter_Linear, true);
			if(!t.texturePtr)
			{
				t.bExist	=	false;
				return 0;
			}
		}
		return t.texturePtr;
	}
	VOID					Scene3D::SetLayerTexture(int nLayer, const char* szFileName)
	{CPUTimeNone();
		if(nLayer<0||nLayer>=MAX_LAYER_TEXTURE)
			return;
		SceneTextureRuntime& t	=	m_layerTextures[nLayer];
		t.fileName		=	szFileName;
		t.fileName.setIsFile();
		t.bExist		=	true;
		if(strstr(t.fileName, "e_"))
			t.bIsMask = true;
		else
			t.bIsMask = false;
		safe_release(t.texturePtr);
	}
	VOID					SceneBox::SetModifyed(BOOL b){CPUTimeNone();
#ifdef EDITOR_OS
		m_bModifyed = b;
#endif
	}
#ifdef EDITOR_OS
	color4					Scene3D::GetGridAttrColor(int x, int y)
	{CPUTimeNone();
		if(x < 0 || y < 0 || x >= GetMaxGridCount() || y >= GetMaxGridCount())
			return color4(1,1,1,1);
		int xRegion	=	x / REGION_GRID_COUNT_25d;
		int yRegion	=	y / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		if(!pRegion)
			return color4(0,0,0,0);
		int nGridX = x % REGION_GRID_COUNT_25d;
		int nGridY = y % REGION_GRID_COUNT_25d;
		if(!pRegion->m_attrs)
			return color4(0,0,0,0);
		GridAttr25d& gr = pRegion->m_attrs[nGridX + nGridY * REGION_VERTEX_COUNT_25d];
		return color4(RGB(gr.r, gr.g, gr.b), 1);
	}
	VOID					Scene3D::SetGridAttrVisible(int x, int y, int visible)
	{CPUTimeNone();
		if(x < 0 || y < 0 || x >= GetMaxGridCount() || y >= GetMaxGridCount())
			return;
		int xRegion	=	x / REGION_GRID_COUNT_25d;
		int yRegion	=	y / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		if(!pRegion)
			return;
		int nGridX = x % REGION_GRID_COUNT_25d;
		int nGridY = y % REGION_GRID_COUNT_25d;
		if(!pRegion->m_attrs)
			return;
		GridAttr25d& attr = pRegion->m_attrs[nGridX + nGridY * REGION_VERTEX_COUNT_25d];
		attr.SetVisible(visible);
	}
	VOID					Scene3D::SetGridAttrColor(int x, int y, const color4& rgb)
	{CPUTimeNone();
		if(x < 0 || y < 0 || x >= GetMaxGridCount() || y >= GetMaxGridCount())
			return;
		int xRegion	=	x / REGION_GRID_COUNT_25d;
		int yRegion	=	y / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		if(!pRegion)
			return;
		int nGridX = x % REGION_GRID_COUNT_25d;
		int nGridY = y % REGION_GRID_COUNT_25d;
		if(!pRegion->m_attrs)
			return;
		GridAttr25d& gr = pRegion->m_attrs[nGridX + nGridY * REGION_VERTEX_COUNT_25d];
		gr.r = clamp(rgb.r, 0, 1)*255.0f;
		gr.g = clamp(rgb.g, 0, 1)*255.0f;
		gr.b = clamp(rgb.b, 0, 1)*255.0f;
	}

	INT						Scene3D::GetGridAttrIndex(int x, int y)
	{CPUTimeNone();
		if(x<0)x=0;
		if(y<0)y=0;
		if(x>GetMaxGridCount())
			x = GetMaxGridCount();
		if(y>GetMaxGridCount())
			y = GetMaxGridCount();
		//if(x < 0 || y < 0 || x >= GetMaxGridCount() || y >= GetMaxGridCount())
			//return 1;
		int xRegion	=	x / REGION_GRID_COUNT_25d;
		int yRegion	=	y / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		if(!pRegion)
			return 0;
		int nGridX = x % REGION_GRID_COUNT_25d;
		int nGridY = y % REGION_GRID_COUNT_25d;
		if(!pRegion->m_attrs)
			return FALSE;
		return pRegion->m_attrs[nGridX + nGridY * REGION_VERTEX_COUNT_25d].texid;
	}
	VOID					Scene3D::SetGridAttrIndex(int x, int y, int nTexture)
	{CPUTimeNone();
		if(x < 0 || y < 0 || x >= GetMaxGridCount() || y >= GetMaxGridCount())
			return;
		int xRegion	=	x / REGION_GRID_COUNT_25d;
		int yRegion	=	y / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		if(!pRegion)
			return;
		int nGridX = x % REGION_GRID_COUNT_25d;
		int nGridY = y % REGION_GRID_COUNT_25d;
		if(!pRegion->m_attrs)
			return;
		GridAttr25d& attr = pRegion->m_attrs[nGridX + nGridY * REGION_VERTEX_COUNT_25d];
		attr.texid = nTexture;
		attr.fullindex = 0;
		//memset(attr.tex, 0, sizeof(attr.tex));
		//memset(attr.blend, 0, sizeof(attr.blend));
	}
#endif
	BOOL					Scene3D::GetVertexPosition(INT nGridX, INT nGridY, float3& out)	//返回顶点索引
	{CPUTimeNone();
		if(nGridX < 0 || nGridY < 0 || nGridX > GetMaxGridCount() || nGridY > GetMaxGridCount())
		{
			out			=	float3((float)nGridX * m_fGridSize, (float)nGridY * m_fGridSize, 0);
			return FALSE;
		}
		int nOldX = nGridX;
		int nOldY = nGridY;
		bool bEndofX = false;
		bool bEndofY = false;
		if(nGridX == GetMaxGridCount())
		{
			bEndofX = true;
			nGridX--;
		}
		if(nGridY == GetMaxGridCount())
		{
			bEndofY = true;
			nGridY--;
		}
		int xRegion	=	nGridX / REGION_GRID_COUNT_25d;
		int yRegion	=	nGridY / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		if(!pRegion)
			return FALSE;
		GVertexData* pAttr;
		if(bEndofX&&bEndofY)
			pAttr = pRegion->GetVertexAttributePtr(REGION_GRID_COUNT_25d, REGION_GRID_COUNT_25d);
		else if(bEndofX)
			pAttr = pRegion->GetVertexAttributePtr(REGION_GRID_COUNT_25d, nGridY % REGION_GRID_COUNT_25d);
		else if(bEndofY)
			pAttr = pRegion->GetVertexAttributePtr(nGridX % REGION_GRID_COUNT_25d, REGION_GRID_COUNT_25d);
		else
			pAttr = pRegion->GetVertexAttributePtr(nGridX % REGION_GRID_COUNT_25d, nGridY % REGION_GRID_COUNT_25d);
		if(!pAttr)
			return FALSE;
		out			=	float3((float)nOldX * m_fGridSize, (float)nOldY * m_fGridSize, pAttr->GetHeight());
		return TRUE;
	}
	BOOL					Scene3D::RecalcRegionGrassHeight(INT xRegion, INT yRegion)	//重新设置格子内所有草地的高度
	{CPUTimeNone();
		SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		if(!pRegion)
			return false;
		int xBegin	=	xRegion * REGION_GRID_COUNT_25d;
		int xEnd	=	(xRegion + 1) * REGION_GRID_COUNT_25d;
		int yBegin	=	yRegion * REGION_GRID_COUNT_25d;
		int yEnd	=	(yRegion + 1) * REGION_GRID_COUNT_25d;
		for(int x=xBegin;x<xEnd;x++)
		{
			for(int y=yBegin;y<yEnd;y++)
			{
				//RecalcGrassHeight(x, y);
			}
		}
		return true;
	}
	void				computeNormals(char* pVertex, int vertexWidth/*顶点数据类型字节数*/, int vertexCount, const void* faces, int faceCount, int posOffset, int normalOffset)	//构造模型法线
	{
		for(int i=0;i<vertexCount;i++)
		{
			float3* normal	=	(float3*)(pVertex + (i * vertexWidth + normalOffset));
			normal->setxyz(0, 0, 0);
		}
		//
		Triangle* face16	=	(Triangle*)faces;
		//
		for(int i=0;i<faceCount;i++)
		{
			int i1	=	face16[i].v1;
			int i2	=	face16[i].v2;
			int i3	=	face16[i].v3;
			float3& v1	=	*(float3*)(pVertex + ((int)i1 * vertexWidth + posOffset));
			float3& v2	=	*(float3*)(pVertex + ((int)i2 * vertexWidth + posOffset));
			float3& v3	=	*(float3*)(pVertex + ((int)i3 * vertexWidth + posOffset));
			float3 edge1	=	v2 - v1;
			float3 edge2	=	v3 - v1;
			float3 n;
			edge1.cross(n, edge2);
			n.normalize();
			//
			float3& n1	=	*(float3*)(pVertex + ((int)i1 * vertexWidth + normalOffset));
			float3& n2	=	*(float3*)(pVertex + ((int)i2 * vertexWidth + normalOffset));
			float3& n3	=	*(float3*)(pVertex + ((int)i3 * vertexWidth + normalOffset));
			n1			+=	n;
			n2			+=	n;
			n3			+=	n;
		}
		//
		for(int i=0;i<vertexCount;i++)
		{
			float3* normal	=	(float3*)(pVertex + i * vertexWidth + normalOffset);
			normal->normalize();
		}
	}
#ifdef EDITOR_OS
	enum blendid{
		blendid_lt = 0,
		blendid_t,
		blendid_rt,
		blendid_r,
		blendid_lb_rb_rt,
		blendid_full,
		blendid_lt_lb_rb,
		blendid_lt_rb,
		blendid_rb_rt_lt,
		blendid_full2,
		blendid_rt_lt_lb,
		blendid_lb_rt,
		blendid_lb,
		blendid_b,
		blendid_rb,
		blendid_l,
	};
	VOID					GetUV(float2& x1y1, float2& x2y1, float2& x1y2, float2& x2y2, int type, BOOL bFull, float width, float height)
	{
		float l,t,r,b;
		l=t=r=b=0;
		int x = type % 4;
		int y = type / 4;
		float fSizeW = 0.25f;
		float fSizeH = 0.25f;
		if(bFull)
		{
			fSizeW = 0.125f;
			if(type>=16)
			{
				type %= 16;
				x = (type%4) + 4;
				y = type/4;
			}
		}
		l = (float)x * fSizeW;
		t = (float)y * fSizeH;
		r = l + fSizeW;
		b = t + fSizeH;
		float fAddW = 0.5/width;
		float fAddH = 0.5/height;
		//l+=fAddW;
		//t+=fAddH;
		//r-=fAddW;
		//b-=fAddH;
		x1y1.setxy(l, t);
		x2y1.setxy(r, t);
		x1y2.setxy(l, b);
		x2y2.setxy(r, b);
	}

	inline	int	texid(int* tid, int x, int y)
	{
		return tid[(x+1)+(y+1)*3];
	}
	VOID					SceneBox::SetBlend(GridAttributeT* attrs, int x, int y, int texid, int offx, int offy, int blend)
	{CPUTimeNone();
		int xx = x + offx;
		int yy = y + offy;
		if(xx<0||yy<0||xx>=REGION_GRID_COUNT_25d||yy>=REGION_GRID_COUNT_25d)
			return;
		if(blend == blendid_full)
		{
			attrs[xx+yy*REGION_VERTEX_COUNT_25d].tex[0] = texid;
		}
		else
			attrs[xx+yy*REGION_VERTEX_COUNT_25d].tex[texid] = texid;
		attrs[xx+yy*REGION_VERTEX_COUNT_25d].blend[texid] = blend;
	}

	SceneVertices25d*		GetVertexPtr25d(SceneVertices25d* verts, INT x, INT y)
	{
		return &verts[x + y * REGION_VERTEX_COUNT_25d];
	}
#pragma pack(push,1)
	struct PVertex
	{
		float3							p/*位置*/;
		float3							n/*法线*/;
	};
#pragma pack(pop)
	VOID					SceneBox::BuildNormals(CDynamicArray<SceneVertices25d>& vertices, CDynamicArray<Triangle>& faces, INT nWidth, INT nHeight)
	{CPUTimeNone();
		INT nBeginGridX		=	this->m_regionX * REGION_GRID_COUNT_25d;
		INT nBeginGridY		=	this->m_regionY * REGION_GRID_COUNT_25d;
		//
		SceneVertices25d* verts	=	vertices.GetPtr();
		computeNormals((char*)vertices.GetPtr(), sizeof(SceneVertices25d), REGION_FULL_VERTICE_25d, faces.GetPtr(), faces.size(), 0, sizeof(float3));
		float fk = this->m_scenePtr->GetNormalBuildVar();
		float	fGridSize = m_scenePtr->GetGridSize();
		//
		PVertex		s_pos_verticesT[(REGION_VERTEX_COUNT_25d + 2) * 2];
		PVertex		s_pos_verticesB[(REGION_VERTEX_COUNT_25d + 2) * 2];
		Triangle	s_faceT[(REGION_GRID_COUNT_25d + 2) * 2];
		Triangle	s_faceB[(REGION_GRID_COUNT_25d + 2) * 2];
		PVertex		s_pos_verticesL[(REGION_VERTEX_COUNT_25d + 2) * 2];
		PVertex		s_pos_verticesR[(REGION_VERTEX_COUNT_25d + 2) * 2];
		Triangle	s_faceL[(REGION_GRID_COUNT_25d + 2) * 2];
		Triangle	s_faceR[(REGION_GRID_COUNT_25d + 2) * 2];
		memset(s_pos_verticesT, 0, sizeof(s_pos_verticesT));
		memset(s_pos_verticesB, 0, sizeof(s_pos_verticesB));
		memset(s_pos_verticesL, 0, sizeof(s_pos_verticesL));
		memset(s_pos_verticesR, 0, sizeof(s_pos_verticesR));
		//
		for(int i=0;i<(REGION_GRID_COUNT_25d + 2);i++)
		{
			GetAttributeFaces25d(s_faceT[i * 2], s_faceT[i * 2 + 1], i, 0, 1, 1, REGION_VERTEX_COUNT_25d + 2, 0);
			GetAttributeFaces25d(s_faceB[i * 2], s_faceB[i * 2 + 1], i, 0, 1, 1, REGION_VERTEX_COUNT_25d + 2, 1);
		}
		for(int x=-1;x<=REGION_VERTEX_COUNT_25d;x++)
		{
			for(int y=0;y<2;y++)
			{
				s_pos_verticesT[x + 1 + (REGION_VERTEX_COUNT_25d + 2) * y].p.setxyz((float)(x+nBeginGridX) * fGridSize, (float)(y+nBeginGridY - 1) * fGridSize, 0);
				s_pos_verticesB[x + 1 + (REGION_VERTEX_COUNT_25d + 2) * y].p.setxyz((float)(x+nBeginGridX) * fGridSize, (float)(y+nBeginGridY + REGION_GRID_COUNT_25d) * fGridSize, 0);
			}
		}
		for(int i=0;i<(REGION_GRID_COUNT_25d + 2);i++)
		{
			GetAttributeFaces25d(s_faceL[i * 2], s_faceL[i * 2 + 1], 0, i, 1, 1, 2, 0, 0);
			GetAttributeFaces25d(s_faceR[i * 2], s_faceR[i * 2 + 1], 0, i, 1, 1, 2, 0, 1);
		}
		for(int x=0;x<2;x++)
		{
			for(int y=-1;y<=REGION_VERTEX_COUNT_25d;y++)
			{
				s_pos_verticesL[(y + 1) * 2 + x].p.setxyz((float)(x + nBeginGridX - 1) * fGridSize, (float)(y+nBeginGridY) * fGridSize, 0);
				s_pos_verticesR[(y + 1) * 2 + x].p.setxyz((float)(x + nBeginGridX + REGION_GRID_COUNT_25d) * fGridSize, (float)(y+nBeginGridY) * fGridSize, 0);
			}
		}
		for(int x=-1;x<=REGION_VERTEX_COUNT_25d;x++)
		{
			for(int y=0;y<2;y++)
			{
				GVertexData* attr	=	m_scenePtr->GetVertexAttribute(nBeginGridX + x, nBeginGridY + y - 1);
				s_pos_verticesT[x + 1 + y * (REGION_VERTEX_COUNT_25d + 2)].p.z	=	(attr ? attr->GetHeight() : 0.0f) + To3DValue(0.1f);
				//
				attr					=	m_scenePtr->GetVertexAttribute(nBeginGridX + x, nBeginGridY + REGION_GRID_COUNT_25d + y);
				s_pos_verticesB[x + 1 + y * (REGION_VERTEX_COUNT_25d + 2)].p.z	=	(attr ? attr->GetHeight() : 0.0f) + To3DValue(0.1f);
			}
		}
		for(int y=-1;y<=REGION_VERTEX_COUNT_25d;y++)
		{
			for(int x=0;x<2;x++)
			{
				GVertexData* attr	=	m_scenePtr->GetVertexAttribute(nBeginGridX + x - 1, nBeginGridY + y);
				s_pos_verticesL[(y + 1) * 2 + x].p.z	=	(attr ? attr->GetHeight() : 0.0f) + To3DValue(0.1f);
				//
				attr					=	m_scenePtr->GetVertexAttribute(nBeginGridX + x + REGION_GRID_COUNT_25d, nBeginGridY + y);
				s_pos_verticesR[(y + 1) * 2 + x].p.z	=	(attr ? attr->GetHeight() : 0.0f) + To3DValue(0.1f);
			}
		}
		computeNormals((char*)s_pos_verticesT, sizeof(PVertex), (REGION_VERTEX_COUNT_25d + 2) * 2, s_faceT, (REGION_GRID_COUNT_25d + 2) * 2, 0, sizeof(float3));
		computeNormals((char*)s_pos_verticesB, sizeof(PVertex), (REGION_VERTEX_COUNT_25d + 2) * 2, s_faceB, (REGION_GRID_COUNT_25d + 2) * 2, 0, sizeof(float3));
		computeNormals((char*)s_pos_verticesL, sizeof(PVertex), (REGION_VERTEX_COUNT_25d + 2) * 2, s_faceL, (REGION_GRID_COUNT_25d + 2) * 2, 0, sizeof(float3));
		computeNormals((char*)s_pos_verticesR, sizeof(PVertex), (REGION_VERTEX_COUNT_25d + 2) * 2, s_faceR, (REGION_GRID_COUNT_25d + 2) * 2, 0, sizeof(float3));
		for(int x=0;x<REGION_VERTEX_COUNT_25d;x++)
		{
			GetVertexPtr25d(verts, x, 0)->n					+=	s_pos_verticesT[x + 1 + (REGION_VERTEX_COUNT_25d + 2)].n;
			GetVertexPtr25d(verts, x, REGION_GRID_COUNT_25d)->n	+=	s_pos_verticesB[x + 1].n;
		}
		for(int y=0;y<REGION_VERTEX_COUNT_25d;y++)
		{
			GetVertexPtr25d(verts, 0, y)->n					+=	s_pos_verticesL[1 + (y + 1) * 2].n;
			GetVertexPtr25d(verts, REGION_GRID_COUNT_25d, y)->n	+=	s_pos_verticesR[(y + 1) * 2].n;
		}
		for(int x=0;x<REGION_VERTEX_COUNT_25d;x++)
		{
			GetVertexPtr25d(verts, x, 0)->n.normalize();
			GetVertexPtr25d(verts, x, REGION_GRID_COUNT_25d)->n.normalize();
		}
		for(int y=0;y<REGION_VERTEX_COUNT_25d;y++)
		{
			GetVertexPtr25d(verts, 0, y)->n.normalize();
			GetVertexPtr25d(verts, REGION_GRID_COUNT_25d, y)->n.normalize();
		}
		//
		for(int x=0;x<REGION_VERTEX_COUNT_25d;x++)
		{
			for(int y=0;y<REGION_VERTEX_COUNT_25d;y++)
			{
				float3& n	=	verts[x + y * REGION_VERTEX_COUNT_25d].n;
				n.z *= fk;
				n.normalize();
			}
		}
		//for(int x=0;x<REGION_GRID_COUNT_25d;x++)
		//{
		//	for(int y=0;y<REGION_GRID_COUNT_25d;y++)
		//	{
		//		GridAttributeRuntime* ptr	=	&m_pDataPtr->m_gridLevels[x + y * REGION_GRID_COUNT_25d];
		//		RecalcSplitVertexNormal(ptr, verts);
		//	}
		//}
		//
	}
	//VOID					AddAttr(CDynamicArray<SceneVertices25d>& attrs, SceneVertices25d& v1, SceneVertices25d& v2, SceneVertices25d& v3)
	//{
	//	int old = attrs.size();
	//	attrs.SetArrayCount(attrs.size()+3);
	//	attrs[old++] = v1;
	//	attrs[old++] = v2;
	//	attrs[old++] = v3;
	//}
	struct VertexDiffuse/*顶点数据*/
	{
		float3							pos/*位置*/;
		unsigned int					color;
	};
	VOID					SceneBox::RebuildMoveGrids()
	{CPUTimeNone();
		m_moveEnable = 0;
		if(!this->m_moveGrids)// || !m_scenePtr->IsEnabled(Scene3D::Enable_ShowMoveGrids))
			return;
		INT nBeginGridX		=	this->m_regionX * REGION_GRID_COUNT_25d;
		INT nBeginGridY		=	this->m_regionY * REGION_GRID_COUNT_25d;
		float fBeginX		=	nBeginGridX * m_scenePtr->GetGridSize();
		float fBeginY		=	nBeginGridY * m_scenePtr->GetGridSize();
		float fGridSize	=	this->m_scenePtr->GetGridSize();
		CDynamicArray<VertexDiffuse>	s_vertices;
		s_vertices.SetArrayCount(REGION_VERTEX_COUNT_25d*REGION_VERTEX_COUNT_25d);
		CDynamicArray<Triangle>	moveEnables;//,moveDisables;
		int numGrids	=	(REGION_GRID_COUNT_25d + 1);
		//
		for(int x=0;x<REGION_GRID_COUNT_25d;x++)
		{
			for(int y=0;y<REGION_GRID_COUNT_25d;y++)
			{
				GridAttr25d* level	=	&m_attrs[x + y * REGION_VERTEX_COUNT_25d];//(x,y);
				SceneTextureRuntime* tr = m_scenePtr->GetLayerTextureData(level->texid);
				if(tr&&tr->bMoveEnable!=1){
					GMoveData* data = &m_moveGrids[x + y * REGION_GRID_COUNT_25d];
					if(tr->bMoveEnable==2)
						data->setMoveEnable(true);
					else
						data->setMoveEnable(false);
				}
			}
		}
		for(int x=0;x<REGION_VERTEX_COUNT_25d;x++)
		{
			float fX = (float)(x - REGION_VERTEX_COUNT_25d/2) / (float)REGION_VERTEX_COUNT_25d;
			for(int y=0;y<REGION_VERTEX_COUNT_25d;y++)
			{
				GVertexData* attr		=	&m_grids[x + y * REGION_VERTEX_COUNT_25d];
				if(!m_attrs[x+y*REGION_VERTEX_COUNT_25d].isVisible())
					continue;
				VertexDiffuse& vertex	=	s_vertices[x + y * REGION_VERTEX_COUNT_25d];
				vertex.pos.x	=	(float)(x + nBeginGridX) * fGridSize;
				vertex.pos.y	=	(float)(y + nBeginGridY) * fGridSize;
				vertex.pos.z	=	attr->GetHeight() + 0.1f;
				vertex.color	=	0x5f00ff00;
			}
		}
		for(int y=0;y<REGION_GRID_COUNT_25d;y++)
		{
			for(int x=0;x<REGION_GRID_COUNT_25d;x++)
			{
				GMoveData& m = this->m_moveGrids[x+y*REGION_GRID_COUNT_25d];
				if(m.isMoveEnable()){
					Triangle f1;
					Triangle f2;
					GetAttributeFaces25d(f1, f2, x, y, 1, 1);
					moveEnables.push_back(f1);
					moveEnables.push_back(f2);
				}
			}
		}
		if(moveEnables.size() > 0)
		{
			MeshPtr temp;
			g_manager.CreateMesh(temp);
			m_moveEnable = temp.GetPtr();
			m_moveEnable->Create(s_vertices.size(), moveEnables.size(), Phantom::FVF_XYZ|Phantom::FVF_DIFFUSE);
			m_moveEnable->WriteVertexBuffer(s_vertices.GetPtr(), s_vertices.size(), sizeof(VertexDiffuse));
			m_moveEnable->WriteIndexBuffer((unsigned short*)moveEnables.GetPtr(), moveEnables.size() * 3, 0);
			m_moveEnable->Commit();
		}
	}

	VOID					SceneBox::RebuildRegionSets(int selindex)
	{CPUTimeNone();
		m_moveEnable = 0;
		if(!this->m_moveGrids)// || !m_scenePtr->IsEnabled(Scene3D::Enable_ShowMoveGrids))
			return;
		INT nBeginGridX		=	this->m_regionX * REGION_GRID_COUNT_25d;
		INT nBeginGridY		=	this->m_regionY * REGION_GRID_COUNT_25d;
		float fBeginX		=	nBeginGridX * m_scenePtr->GetGridSize();
		float fBeginY		=	nBeginGridY * m_scenePtr->GetGridSize();
		float fGridSize	=	this->m_scenePtr->GetGridSize();
		CDynamicArray<VertexDiffuse>	s_vertices;
		s_vertices.SetArrayCount(REGION_VERTEX_COUNT_25d*REGION_VERTEX_COUNT_25d);
		CDynamicArray<VertexDiffuse>	vertices;
		CDynamicArray<WORD>	moveEnables;
		int numGrids	=	(REGION_GRID_COUNT_25d + 1);
		//
		static int colors[] = {0x2fff0000,0x2f00ff00,0x2f0000ff,0x2fffff00,0x2f00ffff,0x2fff00ff,0x2fffffff};
		for(int x=0;x<REGION_VERTEX_COUNT_25d;x++)
		{
			float fX = (float)(x - REGION_VERTEX_COUNT_25d/2) / (float)REGION_VERTEX_COUNT_25d;
			for(int y=0;y<REGION_VERTEX_COUNT_25d;y++)
			{
				GVertexData* attr		=	&m_grids[x + y * REGION_VERTEX_COUNT_25d];
				if(!m_attrs[x+y*REGION_VERTEX_COUNT_25d].isVisible())
					continue;
				VertexDiffuse& vertex	=	s_vertices[x + y * REGION_VERTEX_COUNT_25d];
				vertex.pos.x	=	(float)(x + nBeginGridX) * fGridSize;
				vertex.pos.y	=	(float)(y + nBeginGridY) * fGridSize;
				vertex.pos.z	=	attr->GetHeight() + 0.1f;
			}
		}
		for(int y=0;y<REGION_GRID_COUNT_25d;y++)
		{
			for(int x=0;x<REGION_GRID_COUNT_25d;x++)
			{
				GMoveData& m = this->m_moveGrids[x+y*REGION_GRID_COUNT_25d];
				char type = m.GetRegionType();
				Triangle f1;
				Triangle f2;
				GetAttributeFaces25d(f1, f2, x, y, 1, 1);
				int begin = vertices.size();
				unsigned short fw[6];
				fw[0] = f1.v1;
				fw[1] = f1.v2;
				fw[2] = f1.v3;
				fw[3] = f2.v1;
				fw[4] = f2.v2;
				fw[5] = f2.v3;
				for(int k=0;k<6;k++){
					VertexDiffuse v = s_vertices[fw[k]];
					v.color=colors[type];
					if(selindex==type)
						v.color |= 0x8f000000;
					vertices.push_back(v);
					moveEnables.push_back(begin+k);
				}
			}
		}
		if(moveEnables.size() > 0)
		{
			MeshPtr temp;
			g_manager.CreateMesh(temp);
			m_moveEnable = temp.GetPtr();
			m_moveEnable->Create(vertices.size(), moveEnables.size()/3, Phantom::FVF_XYZ|Phantom::FVF_DIFFUSE);
			m_moveEnable->WriteVertexBuffer(vertices.GetPtr(), vertices.size(), sizeof(VertexDiffuse));
			m_moveEnable->WriteIndexBuffer((unsigned short*)moveEnables.GetPtr(), moveEnables.size(), 0);
			m_moveEnable->Commit();
		}
	}
	struct	SceneVertices25I
	{
		int3								p;
		int2								uv;
		inline	BOOL	operator == (const SceneVertices25I& v){
			if(p.x != v.p.x||p.y != v.p.y||p.z != v.p.z)return false;
			if(uv.x!=v.uv.x||uv.y!=v.uv.y)return false;
			return true;
		}
		SceneVertices25d					vert;
		VOID	Set(SceneVertices25d& v){
			vert = v;
			p.setxyz((int)(vert.p.x*1000), (int)(vert.p.y*1000), (int)(vert.p.z*1000));
			uv.setxy((int)(vert.uv.x*1000), (int)(vert.uv.y*1000));
		}
	};
	VOID					AddAttr(CDynamicArray<SceneVertices25I>& verts, CDynamicArray<Triangle>& faces, SceneVertices25d& v1, SceneVertices25d& v2, SceneVertices25d& v3, BOOL bOptimize)
	{
		if(bOptimize){
			SceneVertices25I i1,i2,i3;
			i1.Set(v1);
			i2.Set(v2);
			i3.Set(v3);
			int s1 = -1;
			int s2 = -1;
			int s3 = -1;
			for(int i=0;i<verts.size();i++)
			{
				if(s1 < 0 && verts[i] == i1)
					s1 = i;
				if(s2 < 0 && verts[i] == i2)
					s2 = i;
				if(s3 < 0 && verts[i] == i3)
					s3 = i;
				if(s1>=0&&s2>=0&&s3>=0)
					break;
			}
			if(s1<0)
			{
				s1 = verts.size();
				verts.push_back(&i1,1);
			}
			if(s2<0)
			{
				s2 = verts.size();
				verts.push_back(&i2,1);
			}
			if(s3<0)
			{
				s3 = verts.size();
				verts.push_back(&i3,1);
			}
			int old = faces.size();
			faces.SetArrayCount(faces.size()+1);
			faces[old].set(s1,s2,s3);
		}
		else
		{
			unsigned short s1,s2,s3;
			s1 = verts.size();
			s2 = s1 + 1;
			s3 = s2 + 1;
			SceneVertices25I si[3];
			si[0].Set(v1);
			si[1].Set(v2);
			si[2].Set(v3);
			verts.push_back(si,3);
			int old = faces.size();
			faces.SetArrayCount(faces.size()+1);
			faces[old].set(s1,s2,s3);
		}
	}
	VOID					SceneBox::RebuildMeshs(BOOL bOptimize)
	{CPUTimeNone();
		if(!this->m_grids)
			return;
		for(int i=0;i<MAX_LAYER_TEXTURE;i++)
		{
			if(m_mesh[i])
				m_mesh[i] = 0;
			if(m_meshAlpha[i])
				m_meshAlpha[i] = 0;
		}
		float fGridSize	=	this->m_scenePtr->GetGridSize();
		int faceCount		=	0;
		CDynamicArray<SceneVertices25d>	s_vertices;
		s_vertices.SetArrayCount(REGION_VERTEX_COUNT_25d*REGION_VERTEX_COUNT_25d);
		CDynamicArray<GridAttributeT> s_attrs;
		s_attrs.SetArrayCount(REGION_VERTEX_COUNT_25d*REGION_VERTEX_COUNT_25d);
		for(int i=0;i<REGION_VERTEX_COUNT_25d*REGION_VERTEX_COUNT_25d;i++)
			s_attrs[i].texid = m_attrs[i].texid;
		int nGridSize	=	(int)(fGridSize * 100.0f);
		INT nBeginGridX		=	this->m_regionX * REGION_GRID_COUNT_25d;
		INT nBeginGridY		=	this->m_regionY * REGION_GRID_COUNT_25d;
		unsigned int c			=	0xffffffff;//color;
		float fMaxHeight		=	-320000.0f;
		float fMinHeight		=	320000.0f;
		for(int x=0;x<REGION_VERTEX_COUNT_25d;x++)
		{
			float fX = (float)(x - REGION_VERTEX_COUNT_25d/2) / (float)REGION_VERTEX_COUNT_25d;
			for(int y=0;y<REGION_VERTEX_COUNT_25d;y++)
			{
				GVertexData* attr		=	&m_grids[x + y * REGION_VERTEX_COUNT_25d];
				GridAttributeT* level	=	&s_attrs[x + y * REGION_VERTEX_COUNT_25d];
				if(!m_attrs[x+y*REGION_VERTEX_COUNT_25d].isVisible())
					continue;
				float fY = (float)(y - REGION_VERTEX_COUNT_25d/2) / (float)REGION_VERTEX_COUNT_25d;
				SceneVertices25d& vertex	=	s_vertices[x + y * REGION_VERTEX_COUNT_25d];
				vertex.p.x	=	(float)(x + nBeginGridX) * fGridSize;
				vertex.p.y	=	(float)(y + nBeginGridY) * fGridSize;
				vertex.p.z	=	0;
				vertex.n.z	=	1.0f;
				vertex.uv.setxy((float)(x+nBeginGridX), (float)(y+nBeginGridY));
				if(x==0)
					vertex.uv.x += 1/256.0f;
				else if(x==(REGION_VERTEX_COUNT_25d-1))
					vertex.uv.x -= 1/256.0f;
				if(y==0)
					vertex.uv.y += 1/256.0f;
				else if(y==(REGION_VERTEX_COUNT_25d-1))
					vertex.uv.y -= 1/256.0f;
				float fHeight		=	attr->GetHeight();
				//int nAlign = (int)this->m_pScenePtr->GetHeightAlignValue();
				//if(nAlign > 0){
				//	int nHeight = ((int)fHeight / nAlign) * nAlign;
				//	fHeight = nHeight;
				//}
				vertex.p.z			=	fHeight;
				color4 c(1,1,1,1);
				//for(int xx=-1;xx<=1;xx++)
				//{
				//	for(int yy=-1;yy<=1;yy++)
				//	{
				//		c += this->m_scenePtr->GetGridAttrColor(x+this->nBeginGridX+xx, y+this->nBeginGridY+yy);
				//	}
				//}
				//c *= (1/9.0f);
				//vertex.nDiffuse		=	c;
				fMaxHeight			=	max(fMaxHeight, fHeight);
				fMinHeight			=	min(fMinHeight, fHeight);
			}
		}
		m_regionBox._max.z = getmax(fMaxHeight, m_regionBox._max.z);
		m_regionBox._min.z = getmin(fMinHeight, m_regionBox._min.z);
		CDynamicArray<Triangle>	facePtr;
		facePtr.SetArrayCount(REGION_GRID_COUNT_25d*REGION_GRID_COUNT_25d*2);
		for(int y=0;y<REGION_GRID_COUNT_25d;y++)
		{
			for(int x=0;x<REGION_GRID_COUNT_25d;x++)
			{
				Triangle& f1	=	facePtr[faceCount++];
				Triangle& f2	=	facePtr[faceCount++];
				GetAttributeFaces25d(f1, f2, x, y, 1, 1);
			}
		}
		//
		BuildNormals(s_vertices, facePtr, REGION_VERTEX_COUNT_25d, REGION_VERTEX_COUNT_25d);
		int faceIdx = 0;
		for(int y=0;y<REGION_GRID_COUNT_25d;y++)
		{
			for(int x=0;x<REGION_GRID_COUNT_25d;x++)
			{
				for(int i=0;i<MAX_DETAIL_TEXTURE;i++)
				{
					s_attrs[x+y*REGION_VERTEX_COUNT_25d].blend[i] = blendid_full;
					s_attrs[x+y*REGION_VERTEX_COUNT_25d].tex[i] = 0;
				}
			}
		}
		for(int y=-1;y<=(REGION_GRID_COUNT_25d+1);y++)
		{
			for(int x=-1;x<=(REGION_GRID_COUNT_25d+1);x++)
			{
				int thist = m_scenePtr->GetGridAttrIndex(nBeginGridX + x,nBeginGridY + y);
				int	tid[9];
				for(int xx=-1;xx<=1;xx++)
				{
					for(int yy=-1;yy<=1;yy++)
					{
						tid[xx+1+(yy+1)*3] = m_scenePtr->GetGridAttrIndex(nBeginGridX+x+xx,nBeginGridY+y+yy);
						if(tid[xx+1+(yy+1)*3]!=thist)
							tid[xx+1+(yy+1)*3] = 0;
					}
				}
				if(texid(tid, 0, 0))
				{
					//0,0
					if(texid(tid, -1,-1) && texid(tid, -1, 0) && texid(tid, 0, -1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,0,blendid_full);
					else if(texid(tid, 0, -1)&&texid(tid, -1, -1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,0,blendid_rb_rt_lt);
					else if(texid(tid, -1, -1)&&texid(tid, -1, 0))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,0,blendid_lt_lb_rb);
					else if(texid(tid, 0, -1)&&texid(tid, -1, 0))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,0,blendid_lb_rb_rt);
					else if(texid(tid, 0, -1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,0,blendid_l);
					else if(texid(tid, -1, -1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,0,blendid_lt_rb);
					else if(texid(tid, -1, 0))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,0,blendid_t);
					else
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,0,blendid_lt);
					//1,0
					if(texid(tid, 0,-1) && texid(tid, 1, -1) && texid(tid, 1, 0))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,0,blendid_full);
					else if(texid(tid, 0, -1)&&texid(tid, 1, -1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,0,blendid_rt_lt_lb);
					else if(texid(tid, 1, 0)&&texid(tid, 1, -1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,0,blendid_lb_rb_rt);
					else if(texid(tid, 1, 0)&&texid(tid, 0, -1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,0,blendid_lt_lb_rb);
					else if(texid(tid, 1, 0))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,0,blendid_t);
					else if(texid(tid, 0, -1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,0,blendid_r);
					else if(texid(tid, 1, -1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,0,blendid_lb_rt);
					else
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,0,blendid_rt);
					//--1,1
					if(texid(tid, 1, 0)&&texid(tid, 1, 1)&&texid(tid,0,1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,1,blendid_full);
					else if(texid(tid, 1,0)&&texid(tid, 1,1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,1,blendid_rb_rt_lt);
					else if(texid(tid, 1,0)&&texid(tid, 0,1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,1,blendid_rt_lt_lb);
					else if(texid(tid, 1,1)&&texid(tid, 0,1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,1,blendid_lt_lb_rb);
					else if(texid(tid, 1, 1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,1,blendid_lt_rb);
					else if(texid(tid, 0, 1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,1,blendid_r);
					else if(texid(tid, 1, 0))
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,1,blendid_b);
					else
						SetBlend(s_attrs.GetPtr(), x,y,thist,1,1,blendid_rb);
					//0,1
					if(texid(tid, 0, 1)&&texid(tid, -1, 1)&&texid(tid,-1,0))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,1,blendid_full);
					else if(texid(tid, 0, 1)&&texid(tid, -1, 1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,1,blendid_lb_rb_rt);
					else if(texid(tid, 0, 1)&&texid(tid, -1, 0))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,1,blendid_rb_rt_lt);
					else if(texid(tid, -1, 0)&&texid(tid, -1, 1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,1,blendid_rt_lt_lb);
					else if(texid(tid, -1, 0))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,1,blendid_b);
					else if(texid(tid, 0, 1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,1,blendid_l);
					else if(texid(tid, -1, 1))
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,1,blendid_lb_rt);
					else
						SetBlend(s_attrs.GetPtr(), x,y,thist,0,1,blendid_lb);
				}
			}
		}
		BOOL bFull[MAX_LAYER_TEXTURE];
		float2 texsize[MAX_LAYER_TEXTURE];
		memset(bFull, 0, sizeof(bFull));
		for(int i=0;i<MAX_LAYER_TEXTURE;i++)
		{
			Texture* tex = m_scenePtr->GetLayerTexture(i);//m_layerTextureID[i]);
			if(tex)
			{
				texsize[i].setxy(tex->GetWidth(), tex->GetHeight());
				bFull[i] = (tex->GetWidth() > tex->GetHeight());
			}
			else
			{
				texsize[i].setxy(128,128);
			}
		}
		CDynamicArray<SceneVertices25I>	attrs[MAX_LAYER_TEXTURE];
		CDynamicArray<SceneVertices25I>	attrsA[MAX_LAYER_TEXTURE];
		CDynamicArray<Triangle>	faceS[MAX_LAYER_TEXTURE];
		CDynamicArray<Triangle>	faceSA[MAX_LAYER_TEXTURE];
		for(int y=0;y<REGION_GRID_COUNT_25d;y++)
		{
			for(int x=0;x<REGION_GRID_COUNT_25d;x++)
			{
				int nGIndex = x+y*REGION_VERTEX_COUNT_25d;
				Triangle& f1	=	facePtr[faceIdx++];
				Triangle& f2	=	facePtr[faceIdx++];
				int x1y1 = x+y*REGION_VERTEX_COUNT_25d;
				int x2y1 = x+1+y*REGION_VERTEX_COUNT_25d;
				int x1y2 = x+(y+1)*REGION_VERTEX_COUNT_25d;
				int x2y2 = x+1+(y+1)*REGION_VERTEX_COUNT_25d;
				GridAttributeT& g = s_attrs[nGIndex];
				GridAttr25d&	ga = m_attrs[nGIndex];
				assert(x1y1>=0&&x1y1<REGION_FULL_VERTICE_25d);
				assert(x2y1>=0&&x2y1<REGION_FULL_VERTICE_25d);
				assert(x1y2>=0&&x1y2<REGION_FULL_VERTICE_25d);
				assert(x2y2>=0&&x2y2<REGION_FULL_VERTICE_25d);
				int fullBlend = g.blend[0];
				bool bfullblend = false;
				if(ga.fullindex<=0){
					if(bFull[g.tex[0]])
					{
						fullBlend	=	RandomF(0, 17.99f);
						if(fullBlend==16)
							fullBlend = blendid_full;
						else if(fullBlend == 17)
							fullBlend = blendid_full2;
						else
						{
							bfullblend	=	true;
							fullBlend += 16;
						}
					}
					else
					{
						if(RandomF(0,1)>0.5)
							fullBlend = blendid_full2;
					}
					ga.fullindex = fullBlend;
				}
				BOOL bExt = true;
				bExt = this->m_scenePtr->m_layerTextures[g.tex[0]].bIsMask;
				if(!bExt){
					s_vertices[x1y1].uv.setxy((float)(x+nBeginGridX), (float)(y+nBeginGridY));
					s_vertices[x2y1].uv.setxy((float)(x+1+nBeginGridX), (float)(y+nBeginGridY));
					s_vertices[x1y2].uv.setxy((float)(x+nBeginGridX), (float)(y+1+nBeginGridY));
					s_vertices[x2y2].uv.setxy((float)(x+1+nBeginGridX), (float)(y+1+nBeginGridY));
				}
				else
					GetUV(s_vertices[x1y1].uv, s_vertices[x2y1].uv, s_vertices[x1y2].uv, s_vertices[x2y2].uv, ga.fullindex, bFull[g.tex[0]], texsize[g.tex[0]].x, texsize[g.tex[0]].y);
				//if(this->m_pScenePtr->m_bShowMoveEnable)
				//{
				//	unsigned int c = 0xffff0000;
				//	if(g.isMoveEnable(m_pScenePtr->m_bShowMoveEnable-1))
				//		c = 0xff00ff00;
				//	s_vertices[f1.v1].nDiffuse = c;
				//	s_vertices[f1.v2].nDiffuse = c;
				//	s_vertices[f1.v3].nDiffuse = c;
				//	s_vertices[f2.v1].nDiffuse = c;
				//	s_vertices[f2.v2].nDiffuse = c;
				//	s_vertices[f2.v3].nDiffuse = c;
				//}
				AddAttr(attrs[g.tex[0]], faceS[g.tex[0]], s_vertices[f1.v1], s_vertices[f1.v2], s_vertices[f1.v3], bOptimize);
				AddAttr(attrs[g.tex[0]], faceS[g.tex[0]], s_vertices[f2.v1], s_vertices[f2.v2], s_vertices[f2.v3], bOptimize);
				for(int i=1;i<MAX_DETAIL_TEXTURE;i++)
				{
					if(g.blend[i]!=blendid_full)
					{
						GetUV(s_vertices[x1y1].uv, s_vertices[x2y1].uv, s_vertices[x1y2].uv, s_vertices[x2y2].uv, g.blend[i], bFull[g.tex[i]], texsize[g.tex[i]].x, texsize[g.tex[i]].y);
						AddAttr(attrsA[g.tex[i]], faceSA[g.tex[i]], s_vertices[f1.v1], s_vertices[f1.v2], s_vertices[f1.v3], bOptimize);
						AddAttr(attrsA[g.tex[i]], faceSA[g.tex[i]], s_vertices[f2.v1], s_vertices[f2.v2], s_vertices[f2.v3], bOptimize);
					}
				}
			}
		}
		//CDynamicArray<WORD> indices;
		CDynamicArray<SceneVertices25d> tempV;
		for(int i=0;i<MAX_LAYER_TEXTURE;i++)
		{
			if(faceS[i].size() > 0)
			{
				//indices.resize(attrs[i].size());
				//for(int j=0;j<indices.size();j++){
				//	indices[j] = j;
				//}
				tempV.SetArrayCount(attrs[i].size());
				for(int j=0;j<attrs[i].size();j++)
					tempV[j] = attrs[i].at(j).vert;
				MeshPtr temp;
				g_manager.CreateMesh(temp);
				m_mesh[i] = temp.GetPtr();
				m_mesh[i]->Create(tempV.size(), faceS[i].size(), Phantom::FVF_XYZ|Phantom::FVF_NORMAL|Phantom::FVF_TEX1);
				assert(sizeof(SceneVertices25d)==m_mesh[i]->GetStrideSize());
				m_mesh[i]->WriteVertexBuffer(tempV.GetPtr(), tempV.size(), sizeof(SceneVertices25d));
				m_mesh[i]->WriteIndexBuffer((unsigned short*)faceS[i].GetPtr(), faceS[i].size()*3, 0);
				if(bOptimize)
					m_mesh[i]->OptimizeMesh();
				m_mesh[i]->Commit();
			}
			if(faceSA[i].size() > 0)
			{
				//indices.resize(attrsA[i].size());
				//for(int j=0;j<indices.size();j++){
				//	indices[j] = j;
				//}
				tempV.SetArrayCount(attrsA[i].size());
				for(int j=0;j<attrsA[i].size();j++)
					tempV[j] = attrsA[i].at(j).vert;
				MeshPtr temp;
				g_manager.CreateMesh(temp);
				m_meshAlpha[i] = temp.GetPtr();
				m_meshAlpha[i]->Create(tempV.size(), faceSA[i].size(), Phantom::FVF_XYZ|Phantom::FVF_NORMAL|Phantom::FVF_TEX1);
				assert(sizeof(SceneVertices25d)==m_meshAlpha[i]->GetStrideSize());
				m_meshAlpha[i]->WriteVertexBuffer(tempV.GetPtr(), tempV.size(), sizeof(SceneVertices25d));
				m_meshAlpha[i]->WriteIndexBuffer((unsigned short*)faceSA[i].GetPtr(), faceSA[i].size()*3, 0);
				if(bOptimize)
					m_meshAlpha[i]->OptimizeMesh();
				m_meshAlpha[i]->Commit();
			}
		}
		RebuildMoveGrids();
	}
#endif
	//VOID					SceneBox::SetLayerTextureID(int index, int id)//设置层级图片
	//{
	//	if(index<0||index>=MAX_LAYER_TEXTURE)
	//		return;
	//	m_layerTextureID[index] = id;
	//}
	VOID					SceneBox::RebuildLight()
	{CPUTimeNone();
		float3 pos = m_regionBox._min + m_regionBox._max;
		pos *= 0.5f;
		float r = m_scenePtr->GetRegionSize();
		r = sqrtf(r*r+r*r);
		m_scenePtr->RebuildLight(m_light, pos, r);
	}
	GVertexData*			SceneBox::GetVertexAttributePtr(INT nGridX, INT nGridY)	//需要返回整个场景中的一个格子数据
	{CPUTimeNone();
		if(!m_grids)return NULL;
		if(nGridX < 0 || nGridY < 0)
			return NULL;
		if(nGridX >= REGION_VERTEX_COUNT_25d || nGridY >= REGION_VERTEX_COUNT_25d)
			return NULL;
		return &m_grids[nGridX + nGridY * REGION_VERTEX_COUNT_25d];
	}

	float					SceneBox::GetVertexHeight(INT nVertX, INT nVertY)
	{CPUTimeNone();
		if(!m_grids)return 0;
		if(nVertX < 0 || nVertY < 0)
			return 0.0f;
		if(nVertX >= REGION_VERTEX_COUNT_25d || nVertY >= REGION_VERTEX_COUNT_25d)
			return 0.0f;
		return m_grids[ nVertX + nVertY * REGION_VERTEX_COUNT_25d ].GetHeight();
	}

	VOID					SceneBox::SetVertexAttributeValue(INT nGridX, INT nGridY, const GVertexData* attr)	//设置单元格属性
	{CPUTimeNone();
		if(!m_grids)return;
		if(nGridX < 0 || nGridY < 0)
			return;
		if(nGridX >= REGION_VERTEX_COUNT_25d || nGridY >= REGION_VERTEX_COUNT_25d)
			return;
		m_grids[nGridX + nGridY * REGION_VERTEX_COUNT_25d]	=	*attr;
		SetModifyed(true);
	}
	VOID					Scene3D::RebuildAllMoveGrids()
	{CPUTimeNone();
#ifdef EDITOR_OS
		if(IsEnabled(Scene3D::Enable_MoveGridsData)&&IsEnabled(Scene3D::Enable_ShowMoveGrids)){
			for(int x=0;x<m_nMaxRegionWidth;x++){
				for(int y=0;y<m_nMaxRegionWidth;y++){
					m_sceneRegions[x + y * m_nMaxRegionWidth].RebuildMoveGrids();
				}
			}
		}
		for(int i=0;i<MAX_LAYER_TEXTURE;i++)
			m_layerTextures[i].bMoveEnable=true;
#endif
	}
	VOID					Scene3D::RebuildRegionSets(int selindex)
	{CPUTimeNone();
#ifdef EDITOR_OS
		if(IsEnabled(Scene3D::Enable_MoveGridsData)&&IsEnabled(Scene3D::Enable_ShowRegionSet)){
			for(int x=0;x<m_nMaxRegionWidth;x++){
				for(int y=0;y<m_nMaxRegionWidth;y++){
					m_sceneRegions[x + y * m_nMaxRegionWidth].RebuildRegionSets(selindex);
				}
			}
		}
#endif
	}
	VOID					Scene3D::RebuildRegionSets(INT xRegion, INT yRegion, int selindex)
	{CPUTimeNone();
#ifdef EDITOR_OS
		if(xRegion < 0 || yRegion < 0 || xRegion >= m_nMaxRegionWidth || yRegion >= m_nMaxRegionWidth)
			return;
		if(IsEnabled(Scene3D::Enable_MoveGridsData)&&IsEnabled(Scene3D::Enable_ShowRegionSet))
			m_sceneRegions[xRegion + yRegion * m_nMaxRegionWidth].RebuildRegionSets(selindex);
#endif
	}
	VOID					Scene3D::RebuildMoveGrids(INT xRegion, INT yRegion)
	{CPUTimeNone();
#ifdef EDITOR_OS
		if(xRegion < 0 || yRegion < 0 || xRegion >= m_nMaxRegionWidth || yRegion >= m_nMaxRegionWidth)
			return;
		for(int i=0;i<MAX_LAYER_TEXTURE;i++)
			m_layerTextures[i].bMoveEnable=true;
		if(IsEnabled(Scene3D::Enable_MoveGridsData)&&IsEnabled(Scene3D::Enable_ShowMoveGrids))
			m_sceneRegions[xRegion + yRegion * m_nMaxRegionWidth].RebuildMoveGrids();
#endif
	}
	VOID					Scene3D::RebuildRegionFaces(INT xRegion, INT yRegion, BOOL bOptimize)		//刷新颜色
	{CPUTimeNone();
#ifdef EDITOR_OS
		if(xRegion < 0 || yRegion < 0 || xRegion >= m_nMaxRegionWidth || yRegion >= m_nMaxRegionWidth)
			return;
		m_sceneRegions[xRegion + yRegion * m_nMaxRegionWidth].RebuildMeshs(bOptimize);
#endif
	}
	VOID					Scene3D::RefreshEditVertices(INT xRegion, INT yRegion)
	{CPUTimeNone();
	}
	GVertexData*			Scene3D::GetVertexAttribute(INT nGridX, INT nGridY)	//返回单元格属性
	{CPUTimeNone();
		//if(nGridX < 0 || nGridY < 0 || nGridX >= GetMaxGridCount() || nGridY >= GetMaxGridCount())
		//	return FALSE;
		//int xRegion	=	nGridX / REGION_GRID_COUNT_25d;
		//int yRegion	=	nGridY / REGION_GRID_COUNT_25d;
		//SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		//if(!pRegion)
		//	return NULL;
		//return pRegion->GetVertexAttributePtr(nGridX % REGION_GRID_COUNT_25d, nGridY % REGION_GRID_COUNT_25d);

		if(nGridX < 0 || nGridY < 0 || nGridX > GetMaxGridCount() || nGridY > GetMaxGridCount())
			return 0;
		int nOldX = nGridX;
		int nOldY = nGridY;
		bool bEndofX = false;
		bool bEndofY = false;
		if(nGridX == GetMaxGridCount())
		{
			bEndofX = true;
			nGridX--;
		}
		if(nGridY == GetMaxGridCount())
		{
			bEndofY = true;
			nGridY--;
		}
		int xRegion	=	nGridX / REGION_GRID_COUNT_25d;
		int yRegion	=	nGridY / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		if(!pRegion)
			return 0;
		if(bEndofX&&bEndofY)
			return pRegion->GetVertexAttributePtr(REGION_GRID_COUNT_25d, REGION_GRID_COUNT_25d);
		else if(bEndofX)
			return pRegion->GetVertexAttributePtr(REGION_GRID_COUNT_25d, nGridY % REGION_GRID_COUNT_25d);
		else if(bEndofY)
			return pRegion->GetVertexAttributePtr(nGridX % REGION_GRID_COUNT_25d, REGION_GRID_COUNT_25d);
		return pRegion->GetVertexAttributePtr(nGridX % REGION_GRID_COUNT_25d, nGridY % REGION_GRID_COUNT_25d);
	}

	VOID						Scene3D::SetVertexAttribute(INT xRegion, INT yRegion, INT nGridX, INT nGridY, const GVertexData* v)	//设置单元格属性
	{CPUTimeNone();
		if(nGridX < 0 || nGridY < 0 || nGridX >= GetMaxGridCount() || nGridY >= GetMaxGridCount())
			return;
		SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		if(!pRegion)
			return;
		pRegion->SetVertexAttributeValue(nGridX % REGION_VERTEX_COUNT_25d, nGridY % REGION_VERTEX_COUNT_25d, v);
	}

	VOID						Scene3D::SetVertexAttribute(INT nGridX, INT nGridY, const GVertexData* v)	//设置单元格属性
	{CPUTimeNone();
		if(nGridX < 0 || nGridY < 0 || nGridX > GetMaxGridCount() || nGridY > GetMaxGridCount())
			return;
		bool bEndofX = false;
		bool bEndofY = false;
		if(nGridX == GetMaxGridCount())
		{
			bEndofX = true;
			nGridX--;
		}
		if(nGridY == GetMaxGridCount())
		{
			bEndofY = true;
			nGridY--;
		}
		int xRegion	=	nGridX / REGION_GRID_COUNT_25d;
		int yRegion	=	nGridY / REGION_GRID_COUNT_25d;
		SceneBox* pRegion	=	GetSceneRegion(xRegion, yRegion);
		if(!pRegion)
			return;
		nGridX %= REGION_GRID_COUNT_25d;
		nGridY %= REGION_GRID_COUNT_25d;
		if(nGridX <= 0)
			SetVertexAttribute( xRegion - 1, yRegion, REGION_GRID_COUNT_25d, nGridY+(bEndofY?1:0), v );
		if(nGridY <= 0)
			SetVertexAttribute( xRegion, yRegion - 1, nGridX+(bEndofX?1:0), REGION_GRID_COUNT_25d, v );
		if(nGridX <= 0 && nGridY <= 0)
			SetVertexAttribute( xRegion - 1, yRegion - 1, REGION_GRID_COUNT_25d, REGION_GRID_COUNT_25d, v );
		if(bEndofX&&bEndofY)
			pRegion->SetVertexAttributeValue(REGION_GRID_COUNT_25d, REGION_GRID_COUNT_25d, v);
		else if(bEndofX)
			pRegion->SetVertexAttributeValue(REGION_GRID_COUNT_25d, nGridY, v);
		else if(bEndofY)
			pRegion->SetVertexAttributeValue(nGridX, REGION_GRID_COUNT_25d, v);
		else
			pRegion->SetVertexAttributeValue(nGridX, nGridY, v);
	}
	//PhantomROAMTriangles::PhantomROAMTriangles(void)
	//{
	//	m_baseLevel			=	0;
	//	m_levelList			=	0;
	//	m_nTriangleAlloc	=	0;
	//}
	//
	//PhantomROAMTriangles::~PhantomROAMTriangles(void)
	//{
	//	if(m_levelList)free(m_levelList);
	//	m_levelList	=	0;
	//}
	//
	//VOID				PhantomROAMTriangles::InitializeROAMs()	//创建地形三角形组合
	//{
	//	if(m_levelList)free(m_levelList);
	//	m_baseLevel	=	0;
	//	m_levelList	=	0;
	//	int nLevel	=	1;
	//	int nIndex	=	0;
	//	int nTriangleCount	=	0;
	//	while(nLevel < REGION_GRID_COUNT_25d)
	//	{
	//		nLevel	=	1 << nIndex;
	//		nIndex++;
	//		//
	//		nTriangleCount	+=	nLevel * nLevel;
	//		//
	//	}
	//	m_nTriangleAlloc	=	nTriangleCount;
	//	//
	//	m_levelList	=	(SceneGridTriangles*)malloc(sizeof(SceneGridTriangles) * m_nTriangleAlloc);
	//	memset(m_levelList, 0, sizeof(SceneGridTriangles) * m_nTriangleAlloc);
	//	//
	//	INT allocIndex	=	0;
	//	m_baseLevel		=	AllocTriangle(allocIndex);
	//	ConstructROAMTriangles(*m_baseLevel, allocIndex, 0, 0, REGION_GRID_COUNT_25d, 0);
	//	//BuildROAMFaces(m_baseLevel, 1);
	//	assert(allocIndex == m_nTriangleAlloc);
	//}
	//
	//SceneGridTriangles*	PhantomROAMTriangles::AllocTriangle(INT& indexInOut)
	//{
	//	assert(indexInOut < m_nTriangleAlloc);
	//	this->m_levelList[indexInOut].alloc_index	=	indexInOut;
	//	return &this->m_levelList[indexInOut++];
	//}
	//
	//VOID				SceneGridTriangles::CreateGridFaces()
	//{
	//	INT height			=	width;
	//	assert(width >= 1 && height >= 1);
	//	int vertexCount		=	(REGION_GRID_COUNT_25d + 1);
	//	unsigned short f0	=	(unsigned short)(y * vertexCount + x);
	//	unsigned short f1	=	(unsigned short)(y * vertexCount + x + width);
	//	unsigned short f2	=	(unsigned short)((y + height) * vertexCount + x + width);
	//	unsigned short f3	=	(unsigned short)((y + height) * vertexCount + x);
	//	int aX				=	x / width;
	//	int aY				=	y / height;
	//	int width2			=	width / 2;
	//	int height2			=	height / 2;
	//	unsigned short m	=	(unsigned short)((y + height2) * vertexCount + x + width2);
	//	//
	//	split[TRI_SIDE_TOP].setTriangle( f0, f1, m );
	//	split[TRI_SIDE_RIGHT].setTriangle( f1, f2, m );
	//	split[TRI_SIDE_BOTTOM].setTriangle( f2, f3, m );
	//	split[TRI_SIDE_LEFT].setTriangle( f3, f0, m );
	//	if(abs(aX % 2) == 0)
	//	{
	//		if(abs(aY % 2) == 0)
	//		{
	//			tri[0].setTriangle( f0, f1, f2);
	//			tri[1].setTriangle( f2, f3, f0);
	//			quad_index[TRI_SIDE_LEFT]	=	1;
	//			quad_index[TRI_SIDE_RIGHT]	=	0;
	//			quad_index[TRI_SIDE_TOP]	=	1;
	//			quad_index[TRI_SIDE_BOTTOM]	=	0;
	//		}
	//		else
	//		{
	//			tri[0].setTriangle( f3, f0, f1);
	//			tri[1].setTriangle( f1, f2, f3);
	//			quad_index[TRI_SIDE_LEFT]	=	0;
	//			quad_index[TRI_SIDE_RIGHT]	=	1;
	//			quad_index[TRI_SIDE_TOP]	=	1;
	//			quad_index[TRI_SIDE_BOTTOM]	=	0;
	//		}
	//	}
	//	else
	//	{
	//		if(abs(aY % 2) == 0)
	//		{
	//			tri[0].setTriangle( f3, f0, f1);
	//			tri[1].setTriangle( f1, f2, f3);
	//			quad_index[TRI_SIDE_LEFT]	=	0;
	//			quad_index[TRI_SIDE_RIGHT]	=	1;
	//			quad_index[TRI_SIDE_TOP]	=	1;
	//			quad_index[TRI_SIDE_BOTTOM]	=	0;
	//		}
	//		else
	//		{
	//			tri[0].setTriangle( f0, f1, f2);
	//			tri[1].setTriangle( f2, f3, f0);
	//			quad_index[TRI_SIDE_LEFT]	=	1;
	//			quad_index[TRI_SIDE_RIGHT]	=	0;
	//			quad_index[TRI_SIDE_TOP]	=	1;
	//			quad_index[TRI_SIDE_BOTTOM]	=	0;
	//		}
	//	}
	//}
	//
	//ROAMLevelInfo::ROAMLevelInfo()
	//{
	//	middleX		=	0;
	//	middleY		=	0;//格子中心点
	//	memset(levels, 0, sizeof(levels));
	//}
	//
	////unsigned int	g_levelIndexs[32] = {0, 1, 2, 4, 8, 16, 32, 64, 8};
	////unsigned int	g_levelIndexs[32] = {0, 1, 4, 8, 16, 32, 64, 7, 8};
	//VOID				ROAMLevelInfo::Setup(const float3& eye, float fGridWidth, float fMaxViewDistance, float fMinViewDistance)
	//{
	//	middleX		=	(int)(eye.x / fGridWidth);
	//	middleY		=	(int)(eye.y / fGridWidth);
	//	int	maxWidth	=	(int)(fMaxViewDistance / fGridWidth);
	//	if(maxWidth < REGION_GRID_COUNT_25d)
	//		maxWidth	=	REGION_GRID_COUNT_25d;
	//	else if(maxWidth > (REGION_GRID_COUNT_25d * 16))
	//		maxWidth	=	REGION_GRID_COUNT_25d * 16;
	//	//
	//	int step		=	maxWidth / MAX_LEVEL_COUNT;
	//	int	minWidth	=	(int)(fMinViewDistance / fGridWidth) / MAX_LEVEL_COUNT;
	//	levels[0].Setup(middleX, middleY, middleX, middleY);
	//	for(int i=1;i<MAX_LEVEL_COUNT;i++)
	//	{
	//		int nC	=	1 << (i / 2 + 1);
	//		int step_c	=	step;
	//		if(i == 1)
	//		{
	//			if(step_c < minWidth)
	//				step_c	=	minWidth;
	//		}
	//		levels[i].Setup(((levels[i-1].beginX - step_c) / nC) * nC, ((levels[i-1].beginY - step_c) / nC) * nC, ((levels[i-1].endX + step_c) / nC) * nC, ((levels[i-1].endY + step_c) / nC) * nC);
	//		if(levels[i].beginX >= levels[i - 1].beginX)
	//			levels[i].beginX	=	levels[i - 1].beginX - nC;
	//		if(levels[i].beginY >= levels[i - 1].beginY)
	//			levels[i].beginY	=	levels[i - 1].beginY - nC;
	//		if(levels[i].endX	<= levels[i - 1].endX)
	//			levels[i].endX		=	levels[i - 1].endX	+ nC;
	//		if(levels[i].endY	<= levels[i - 1].endY)
	//			levels[i].endY	=	levels[i - 1].endY + nC;
	//	}
	//	levels[MAX_LEVEL_COUNT - 1].Setup(0, 0, REGION_GRID_COUNT_25d * 512, REGION_GRID_COUNT_25d * 512);
	//	//
	//}
	//
	//VOID				PhantomROAMTriangles::ConstructROAMTriangles(SceneGridTriangles& data, INT& indexInOut, INT x, INT y, INT width, INT level)
	//{
	//	int width2						=	width/2;
	//	data.width						=	width;
	//	data.x							=	x;
	//	data.y							=	y;
	//	data.level						=	level;
	//	data.CreateGridFaces();
	//	//
	//	int nLevel		=	data.level * 2;
	//	assert(data.level < MAX_LEVEL_ARRAY_COUNT);
	//	m_levelArrays[data.level].push_back( &data );
	//	if(width2 >= 1)
	//	{
	//		for(int i=0;i<SceneGridTriangles::TRI_SIDE_COUNT;i++)
	//		{
	//			data.childPtr[i]			=	AllocTriangle(indexInOut);
	//			data.childPtr[i]->parentPtr	=	&data;
	//		}
	//		ConstructROAMTriangles(*data.childPtr[SceneGridTriangles::TRI_SIDE_LEFTTOP], indexInOut, x, y, width2, level + 1);
	//		ConstructROAMTriangles(*data.childPtr[SceneGridTriangles::TRI_SIDE_RIGHTTOP], indexInOut, x + width2, y, width2, level + 1);
	//		ConstructROAMTriangles(*data.childPtr[SceneGridTriangles::TRI_SIDE_LEFTBOTTOM], indexInOut, x, y + width2, width2, level + 1);
	//		ConstructROAMTriangles(*data.childPtr[SceneGridTriangles::TRI_SIDE_RIGHT_BOTTOM], indexInOut, x + width2, y + width2, width2, level + 1);
	//	}
	//}
	//
	////int	datalevel_levels[]  = {0, 1, 2, 3, 4, 5, 6};
	//int	datalevel_levels[]		= {13,12, 11,10, 9,8, 7,6, 5,4,   3,2,   1,0, -1};
	//
	//INT					PhantomROAMTriangles::CreateLevelTriangles(ROAMLevelInfo& createInfo, PhantomTerrainBox* pRegionPtr, Triangle* faces, BOOL bEnable)//构造三角形列表
	//{
	//	if(!this->m_baseLevel)return 0;
	//	INT faceIndex = 0;
	//	//this->CreateLevelTriangles(createInfo, pRegionPtr, faces, nFaceCount, bEnable);
	//	//return nFaceCount;
	//	if(!bEnable)
	//	{
	//		INT nCount	=	m_levelArrays[ MAX_LEVEL_ARRAY_COUNT - 1 ].GetCount();
	//		for(int i=0;i<nCount;i++)
	//		{
	//			SceneGridTriangles& data	=	*m_levelArrays[ MAX_LEVEL_ARRAY_COUNT - 1 ].GetPtr()[i];
	//			faces[faceIndex++]	=	data.tri[0];
	//			faces[faceIndex++]	=	data.tri[1];
	//		}
	//		return faceIndex;
	//	}
	//	for(int level = 0;level < MAX_LEVEL_ARRAY_COUNT; level++)
	//	{
	//		int nLevel	=	level * 2;
	//		if(datalevel_levels[nLevel] < MAX_LEVEL_COUNT)
	//		{
	//			MinLevelInfo& mInfoMax		=	createInfo.levels[datalevel_levels[nLevel]];
	//			MinLevelInfo& mInfoMiddle	=	createInfo.levels[datalevel_levels[nLevel + 1]];
	//			int lid	=	datalevel_levels[nLevel + 2];
	//			MinLevelInfo* mInfoMin		=	0;
	//			if(lid >= 0)
	//				mInfoMin				=	&createInfo.levels[lid];
	//			//
	//			int regionX			=	pRegionPtr->GetBeginGridX();
	//			int regionY			=	pRegionPtr->GetBeginGridY();
	//			int regionMaxX		=	pRegionPtr->GetBeginGridX() + REGION_GRID_COUNT_25d;
	//			int regionMaxY		=	pRegionPtr->GetBeginGridY() + REGION_GRID_COUNT_25d;
	//			//此块区域不用重新构造此等级三角形
	//			if(regionX > mInfoMax.endX || regionMaxX < mInfoMax.beginX || regionY > mInfoMax.endY || regionMaxY < mInfoMax.beginY)
	//				continue;
	//			//
	//			int nCount	=	m_levelArrays[ level ].GetCount();
	//			for(int id=0;id<nCount;id++)
	//			{
	//				SceneGridTriangles& data	=	*m_levelArrays[ level ].GetPtr()[id];
	//				int x		=	pRegionPtr->GetBeginGridX() + data.x;
	//				int y		=	pRegionPtr->GetBeginGridY() + data.y;
	//				if(data.width >= 1)
	//				{
	//					GridAttributeLevel* level	=	pRegionPtr->GetBaseGridAttributePtr((data.x / data.width) * data.width, (data.y / data.width) * data.width);
	//					if(!level->isVisible())
	//						continue;
	//				}
	//				//
	//				if((x >= mInfoMax.beginX && x < mInfoMax.endX) && (y >= mInfoMax.beginY && y < mInfoMax.endY))
	//				{
	//					if((x < mInfoMiddle.beginX || x >= mInfoMiddle.endX) || (y < mInfoMiddle.beginY || y >= mInfoMiddle.endY))
	//					{
	//						faces[faceIndex++]	=	data.tri[0];
	//						faces[faceIndex++]	=	data.tri[1];
	//					}
	//				}
	//				//
	//				if(mInfoMin)
	//				{
	//					assert(data.width > 1);
	//					
	//					if((x >= mInfoMiddle.beginX && x < mInfoMiddle.endX) && (y >= mInfoMiddle.beginY && y < mInfoMiddle.endY))
	//					{
	//						if((x < mInfoMin->beginX || x >= mInfoMin->endX) || (y < mInfoMin->beginY || y >= mInfoMin->endY))
	//						{
	//							if(x == (mInfoMin->beginX - data.width) && (y >= mInfoMin->beginY && y < mInfoMin->endY))
	//							{
	//								assert(data.childPtr[SceneGridTriangles::TRI_SIDE_RIGHTTOP]);
	//								faces[faceIndex++]	=	data.childPtr[SceneGridTriangles::TRI_SIDE_RIGHTTOP]->tri[1];
	//								faces[faceIndex++]	=	data.childPtr[SceneGridTriangles::TRI_SIDE_RIGHT_BOTTOM]->tri[0];
	//							}
	//							else
	//							{
	//								faces[faceIndex++]	=	data.split[SceneGridTriangles::TRI_SIDE_RIGHT];
	//							}
	//							if(x == mInfoMin->endX && (y >= mInfoMin->beginY && y < mInfoMin->endY))
	//							{
	//								assert(data.childPtr[SceneGridTriangles::TRI_SIDE_RIGHTTOP]);
	//								faces[faceIndex++]	=	data.childPtr[SceneGridTriangles::TRI_SIDE_LEFTTOP]->tri[1];
	//								faces[faceIndex++]	=	data.childPtr[SceneGridTriangles::TRI_SIDE_LEFTBOTTOM]->tri[0];
	//							}
	//							else
	//							{
	//								faces[faceIndex++]	=	data.split[SceneGridTriangles::TRI_SIDE_LEFT];
	//							}
	//							if(y == (mInfoMin->beginY - data.width) && (x >= mInfoMin->beginX && x < mInfoMin->endX))
	//							{
	//								assert(data.childPtr[SceneGridTriangles::TRI_SIDE_RIGHTTOP]);
	//								faces[faceIndex++]	=	data.childPtr[SceneGridTriangles::TRI_SIDE_LEFTBOTTOM]->tri[1];
	//								faces[faceIndex++]	=	data.childPtr[SceneGridTriangles::TRI_SIDE_RIGHT_BOTTOM]->tri[1];
	//							}
	//							else
	//							{
	//								faces[faceIndex++]	=	data.split[SceneGridTriangles::TRI_SIDE_BOTTOM];
	//							}
	//							if(y == mInfoMin->endY && (x >= mInfoMin->beginX && x < mInfoMin->endX))
	//							{
	//								assert(data.childPtr[SceneGridTriangles::TRI_SIDE_RIGHTTOP]);
	//								faces[faceIndex++]	=	data.childPtr[SceneGridTriangles::TRI_SIDE_LEFTTOP]->tri[0];
	//								faces[faceIndex++]	=	data.childPtr[SceneGridTriangles::TRI_SIDE_RIGHTTOP]->tri[0];
	//							}
	//							else
	//							{
	//								faces[faceIndex++]	=	data.split[SceneGridTriangles::TRI_SIDE_TOP];
	//							}
	//						}
	//					}
	//				}
	//				else
	//				{
	//					assert(data.width == 1);
	//				}
	//			}
	//		}
	//	}
	//	return faceIndex;
	//}
	//
	//VOID				PhantomROAMTriangles::CreateLevelTriangles(ROAMLevelInfo& createInfo, PhantomTerrainBox* pRegionPtr, Triangle* faces, INT& faceIndex, BOOL bEnable)//构造三角形列表
	//{
	//}
	//
	VOID				Scene3D::bind_event(int eventID, const char* szLuaFunc)
	{CPUTimeNone();
		for(int i=0;i<m_binds.size();i++)
		{
			if(m_binds[i].id == eventID)
			{
				m_binds[i].func = szLuaFunc;
				return;
			}
		}
		BindEvent m;
		m.id = eventID;
		m.func = szLuaFunc;
		m_binds.push_back(m);
	}
	SceneObjectSkill*		Scene3D::GetFreeSkill()
	{CPUTimeT(Scene3D);
		for(int i=0;i<m_skills.size();i++)
		{
			if(m_skills[i]->IsFree())
			{
				m_skills[i]->m_fWaitFreeTime = 0.1;//如果本来是空闲的，调用这个函数之后要让他在0.1秒之后空闲，这样就不会在同一段代码内被多次使用
				return m_skills[i];
			}
		}
		SceneObjectSkill* s = new SceneObjectSkill(this);
		m_skills.push_back(s);
		return s;
	}
	SceneGameSkill*		Scene3D::GetFreeGameSkill(unsigned short fileid)
	{CPUTimeT(Scene3D);
		for(int i=0;i<m_gameSkills.size();i++)
		{
			if(m_gameSkills[i]->IsFree()&&m_gameSkills[i]->m_fileid==fileid)
			{
				m_gameSkills[i]->m_fWaitFreeTime = 0.1;//如果本来是空闲的，调用这个函数之后要让他在0.1秒之后空闲，这样就不会在同一段代码内被多次使用
				return m_gameSkills[i];
			}
		}
		SceneGameSkill* s = new SceneGameSkill(this);
		if(!s->Setup(fileid)){
			delete s;
			return 0;
		}
		m_gameSkills.push_back(s);
		return s;
	}

	SceneObjectSkill::SceneObjectSkill(Scene3D* s)
	{
		m_scenePtr		=	s;
		m_sourceUnit	=	PtrID(-1,-1);
		m_targetUnit	=	PtrID(-1,-1);
		m_fSpeed		=	To3DValue(10);
		m_bHasTarget	=	0;
		m_sourceBoneID	=	-1;
		m_targetBoneID	=	-1;
		m_sourcePos.setxyz(0,0,0);//技能开始点
		m_targetPos.setxyz(0,0,0);
		m_readyTime		=	0;
		m_bFree			=	true;
		m_fHeight		=	0;
		m_fHeightTarget	=	0;
		m_fWaitTime		=	0;
		m_fWaitFreeTime =	0.1;
		m_fContinueTime	=	0;
		m_bContinueTime =	false;
		m_fSourceScale	=	1;
		m_collisionClassID	=	0;
		m_collisionPos.setxyz(0,0,0);
	}
	SceneObjectSkill::~SceneObjectSkill()
	{
		m_mesh = 0;
	}
	VOID						SceneObjectSkill::SetSkillMesh(const char* szFile)
	{CPUTimeT(SceneObjectSkill);
		g_manager.LoadMesh(m_mesh, szFile);
		if(m_mesh.t==0){
			LogInfo("system->error:%s(%s)", utf8("技能找不到模型"), szFile);
		}
	}
	VOID						SceneObjectSkill::SetTarget(SceneObject* target, const char* szBoneLike)
	{CPUTimeT(SceneObjectSkill);
		m_targetBoneID = -1;
		if(target)
			m_targetUnit.id = target->GetID();
		else
			m_targetUnit.id = 0;
		SceneObject* obj = GetTarget();
		if(!obj)
			return;
		m_bHasTarget = true;
		if(szBoneLike&&szBoneLike[0] && obj->GetMeshPtr())
		{
			m_targetBoneID = obj->GetMeshPtr()->GetBoneIDLike(szBoneLike);
			if(m_targetBoneID<0){
				LogInfo("system->error:target %s(%s)%s(%s)",utf8("场景对象"),obj->GetName(),utf8("中找不到骨骼"),szBoneLike);
			}
		}
	}
	//
	VOID						SceneObjectSkill::SetSource(SceneObject* unit, const char* szBoneLike)
	{CPUTimeT(SceneObjectSkill);
		m_sourceBoneID = -1;
		if(unit)
			m_sourceUnit.id = unit->GetID();
		else
			m_sourceUnit.id = 0;
		SceneObject* obj = GetSource();
		if(obj)
		{
			m_sourcePos = obj->GetPosition();
			if(szBoneLike&&szBoneLike[0] && obj->GetMeshPtr())
			{
				m_sourceBoneID = obj->GetMeshPtr()->GetBoneIDLike(szBoneLike);
				if(m_sourceBoneID<0){
					LogInfo("system->error:source %s(%s)%s(%s)",utf8("场景对象"),obj->GetName(),utf8("中找不到骨骼"),szBoneLike);
				}
			}
		}
	}
	VOID						SceneObjectSkill::SetTargetPos(const float3& pos)
	{CPUTimeT(SceneObjectSkill);
		m_bHasTarget = true;
		m_targetUnit.id = 0;
		m_targetPos = pos;
	}
	VOID						SceneObjectSkill::SetSourcePos(const float3& pos)
	{CPUTimeT(SceneObjectSkill);
		m_sourceUnit.id = 0;
		m_sourcePos = pos;
	}
	VOID						SceneObjectSkill::StartSkill(LPCSTR szFinishFunc)
	{CPUTimeT(SceneObjectSkill);
		if(!m_mesh)return;
		this->m_finishFunc = szFinishFunc;
		matrix4x4 mat;
		BOOL bSource = false;
		if(m_sourceUnit.id>0){//如果没有目标的情况下才起作用
			SceneObject* obj = GetSource();
			if(obj)
			{
				Mesh* mesh = obj->GetMeshPtr();
				if(mesh)
				{
					mesh->GetBoneMatrix(m_sourceBoneID, &mat);
					const AABox& box = mesh->GetObjectRegion().aabox;
					mat._43 += box._max.z*this->m_fHeight;
					m_sourcePos = mat.GetPosition();
					bSource = true;
				}
			}
			if(!bSource)
				LogInfo("system->error:missing source object!");
		}
		if(!bSource){
			mat.identity();
			mat.SetPosition(m_sourcePos);
		}
		if(m_bHasTarget && m_targetUnit.id>0){//如果没有目标的情况下才起作用
			SceneObject* obj = GetTarget();
			BOOL bTarget = false;
			if(obj)
			{
				Mesh* mesh = obj->GetMeshPtr();
				if(mesh)
				{
					matrix4x4 mat;
					mesh->GetBoneMatrix(m_targetBoneID, &mat);
					m_targetPos = mat.GetPosition();
					const AABox& box = mesh->GetObjectRegion().aabox;
					m_targetPos.z += box._max.z*this->m_fHeightTarget;
					bTarget = true;
				}
			}
			if(!bTarget)
				LogInfo("system->error:missing target object!");
		}
		m_mesh->m_callback = this;
		if((int)(m_fSourceScale*1000)!=1000)
		{
			matrix4x4 ms;
			matrixScaling(&ms, m_fSourceScale, m_fSourceScale, m_fSourceScale);
			matrixMultiply(&mat, &ms, &mat);
		}
		m_collisionPos = m_sourcePos;
		m_mesh->SetWorldMatrix(mat);
		m_mesh->SetCurrentAction(0, 1);
		m_mesh->m_bGeometryVisible = true;
		m_mesh->Pause(false);
		m_bFree = false;
	}
	VOID						SceneObjectSkill::SetFree()
	{CPUTimeT(SceneObjectSkill);
		if(this->m_finishFunc.t[0])
		{
			lua_Map3DSkill_s mgr;
			SetIPtr(&mgr.m_data, this, SceneObjectSkill, IMap3DSkillPtr);
			lua_tinker::call<void>(g_luamanager.GetLuaState(), m_finishFunc.t, mgr);
			m_finishFunc.t[0] = 0;
		}
		m_mesh->m_bGeometryVisible = false;
		m_mesh->Pause(true);
		this->m_bFree		= true;
		this->m_bHasTarget	= false;
		this->m_fWaitTime	=	0;
		m_fWaitFreeTime		=	5;//2秒缓冲消失时间
		m_sourceBoneID = -1;
		m_targetBoneID = -1;
		m_sourceUnit.id = 0;
		m_targetUnit.id = 0;
		this->m_fHeight=this->m_fHeightTarget=0;
		m_fContinueTime		=	0;
		m_bContinueTime = false;
	}
	VOID						SceneObjectSkill::OnFrameMove()
	{CPUTimeT(SceneObjectSkill);
		if(m_bFree&&m_fWaitFreeTime<0)return;
		float fElapsedTime = g_manager.GetElapsedTime();
		if(m_bFree){
			m_fWaitFreeTime -= fElapsedTime;
			if(m_fWaitFreeTime<0)
				return;
		}
		else{
			if(m_fWaitTime>0)
			{
				m_fWaitTime -= fElapsedTime;
				return;
			}
			if(m_bContinueTime){
				m_fContinueTime -= fElapsedTime;
				if(m_fContinueTime<0){
					SetFree();
				}
			}
			if(m_sourceUnit.id>0&&!this->m_bHasTarget){//如果没有目标的情况下才起作用
				SceneObject* obj = GetSource();
				if(obj)
				{
					Mesh* mesh = obj->GetMeshPtr();
					if(mesh)
					{
						matrix4x4 mat;
						mesh->GetBoneMatrix(m_sourceBoneID, &mat);
						const AABox& box = mesh->GetObjectRegion().aabox;
						mat._43 += box._max.z*this->m_fHeight;
						if((int)(m_fSourceScale*1000)!=1000)
						{
							matrix4x4 ms;
							matrixScaling(&ms, m_fSourceScale, m_fSourceScale, m_fSourceScale);
							matrixMultiply(&mat, &ms, &mat);
						}
						m_mesh->SetWorldMatrix(mat);
					}
				}
			}
			if(m_bHasTarget){
				if(m_targetUnit.id>0){
					BOOL bTarget = false;
					SceneObject* obj = GetTarget();
					if(obj)
					{
						Mesh* mesh = obj->GetMeshPtr();
						if(mesh)
						{
							matrix4x4 mat;
							mesh->GetBoneMatrix(m_targetBoneID, &mat);
							m_targetPos = mat.GetPosition();
							const AABox& box = mesh->GetObjectRegion().aabox;
							m_targetPos.z += box._max.z*this->m_fHeightTarget;
							bTarget = true;
						}
					}
				}
				float3 dir = m_sourcePos - m_targetPos;
				float fLength = dir.R();
				float fMove = fElapsedTime * this->m_fSpeed;
				if(fLength<fMove){
					matrix4x4 mat;
					matrixRotationZ(&mat, m_fRotationZ);
					mat.SetPosition(m_sourcePos=m_targetPos);
					m_mesh->SetWorldMatrix(mat);
					SetFree();
				}
				else
				{
					m_sourcePos = dir * (1-fMove/fLength) + m_targetPos;
					m_fRotationZ = getRotateZ(dir) + PI;
					matrix4x4 mat;
					matrixRotationZ(&mat, m_fRotationZ);
					mat.SetPosition(m_sourcePos);
					if((int)(m_fSourceScale*1000)!=1000)
					{
						matrix4x4 ms;
						matrixScaling(&ms, m_fSourceScale, m_fSourceScale, m_fSourceScale);
						matrixMultiply(&mat, &ms, &mat);
					}
					m_mesh->SetWorldMatrix(mat);
				}
				if(m_collisionFunc.t[0]){//如果存在碰撞检测要求
					float3 dir = m_sourcePos - m_collisionPos;
					float length = dir.R();
					if(length>1){
						dir.normalize();
						GetCameraPtr()->GetIntersectPtr()->begin(m_collisionPos, dir, length);
						PickPoint out;
						BOOL bScene = false;
						if(m_scenePtr->IsEnabled(Scene3D::Enable_ShowTerrain)&&m_scenePtr->IntersectTerrain(GetCameraPtr()->GetIntersectPtr()->GetOriginal(), GetCameraPtr()->GetIntersectPtr()->GetDir(), out))
						{
							bScene = true;
						}
						SceneObject* obj = this->m_scenePtr->Intersect(GetCameraPtr()->GetIntersectPtr(), true);//this->m_collisionClassID);
						if(obj || bScene){
							if(bScene&&obj){
								float3 sdir = out.pos - m_collisionPos;
								float3 odir = GetCameraPtr()->GetIntersectPtr()->GetPickInfo().vPick - m_collisionPos;
								float sl = sdir.R();
								float ol = odir.R();
								if(sl<ol)//如果场景碰撞更近则
									obj = 0;
							}
							lua_Map3DItem_s objmgr;
							lua_Map3DSkill_s skillmgr;
							lua_PickPoint posmgr;
							if(obj)
							{
								SetIPtr(&objmgr.m_data, obj, SceneObject, IMap3DItemPtr);
								out.pos = GetCameraPtr()->GetIntersectPtr()->GetPickInfo().vPick;
								out.normal = GetCameraPtr()->GetIntersectPtr()->GetPickInfo().vFaceNormal;
								out.t = GetCameraPtr()->GetIntersectPtr()->GetPickInfo().fDist;
							}
							SetIPtr(&skillmgr.m_data, this, SceneObjectSkill, IMap3DSkillPtr);
							posmgr.pos = out.pos;
							posmgr.normal = out.normal;
							posmgr.t = out.t;
							lua_tinker::call<void>(g_luamanager.GetLuaState(), m_collisionFunc.t, skillmgr, objmgr, posmgr);
							m_collisionFunc.t[0] = 0;
							SetFree();
						}
						m_collisionPos = m_sourcePos;
					}
				}
			}
		}
		if(m_mesh)
		{
			m_mesh->FrameMove();
		}
	}
	VOID						SceneObjectSkill::BindCollision(int classID, const char* szFunc)
	{CPUTimeT(SceneObjectSkill);
		this->m_collisionClassID = classID;
		this->m_collisionFunc = szFunc;
	}
	VOID						SceneObjectSkill::OnRender()
	{CPUTimeT(SceneObjectSkill);
		if(m_bFree&&m_fWaitFreeTime<0)return;
		if(m_fWaitTime>0)return;
		if(m_mesh)
			m_mesh->Render();
	}

	SceneObject*				SceneObjectSkill::GetTarget()
	{CPUTimeT(SceneObjectSkill);
		if(m_targetUnit.id==0)return 0;
		return m_scenePtr->GetObject(m_targetUnit);
	}
	SceneObject*				SceneObjectSkill::GetSource()
	{CPUTimeT(SceneObjectSkill);
		if(m_sourceUnit.id==0)return 0;
		return m_scenePtr->GetObject(m_sourceUnit);
	}
	VOID						SceneObjectSkill::ActionPlayOver(Mesh* m, int actiontype, const char* actionname)
	{CPUTimeT(SceneObjectSkill);
		if(!this->m_bHasTarget)
		{
			if(m_fContinueTime<=0.001)
				SetFree();
		}
	}
	VOID						SceneObjectSkill::ActionAttackBy(Mesh* m, int actionID, int playtype)
	{CPUTimeT(SceneObjectSkill);
	}


};
//
//
//
