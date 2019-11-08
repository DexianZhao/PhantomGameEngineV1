//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomSubMesh.h"
#include "PhantomMesh.h"
#include "PhantomManager.h"

namespace Phantom{
	
	RenderNodeManager::RenderNodeManager()
	{
	}
	RenderNodeManager::~RenderNodeManager()
	{
	}
	VOID			RenderNodeManager::PushNode(RenderNode* n)
	{
		if(!n->m_pManager){
			m_renders.push_back(n);
			n->m_pManager = this;
		}
	}
	VOID			RenderNodeManager::PopNode(RenderNode* n)
	{
		if(!n||n->m_pManager!=this)
			return;
		for(int i=0;i<m_renders.size();i++)
		{
			if(m_renders[i] == n)
			{
				n->m_pManager = 0;
				m_renders.eraseByIndex(i);
				break;
			}
		}
	}
	void			RenderNodeManager::Render()		//所有设置都有效并进行渲染(包括矩阵数据，透明设置等等)
	{CPUTimeT(RenderNodeManager);
		for(int i=0;i<m_renders.size();i++){
			m_renders[i]->Render(true);
		}
	}
	void			RenderNodeManager::FrameMove()				//计算
	{CPUTimeT(RenderNodeManager);
		for(int i=0;i<m_renders.size();i++){
			m_renders[i]->FrameMove();
		}
	}

#define	MeshEffectSupport_Count				10

	LODMesh::LODMesh(void)
	{
		memset(m_nLodLevelFaces, 0, sizeof(m_nLodLevelFaces));
	}

	VOID	LODMesh::DestroyAllData()
	{CPUTimeT(LODMesh);
		DestroyLODMesh();
	}

	LODMesh::~LODMesh(void)
	{
		DestroyAllData();
	}

	VOID			LODMesh::DestroyLODMesh()
	{CPUTimeT(LODMesh);
		for(INT i=0;i<MAX_LOD_LEVEL_COUNT;i++)
		{
			m_pLODMeshs[i] = 0;
		}
	}

	VOID		LODMesh::LoadLODMesh(StreamBase* s, BOOL bOldVersion)
	{CPUTimeT(LODMesh);
		DestroyAllData();
		INT nSLOD;
		s->read(&nSLOD, sizeof(nSLOD));
		if(nSLOD <= 0)return;
		//
		INT			nVersion;
		s->read(&nVersion, sizeof(INT));
		if(nVersion<4){
			INT		nLodLevelFaces[20];
			s->read(nLodLevelFaces, sizeof(nLodLevelFaces));
			for(int i=0;i<3;i++)
				m_nLodLevelFaces[i] = nLodLevelFaces[i];
		}
		else
			s->read(m_nLodLevelFaces, sizeof(m_nLodLevelFaces));

		if(nVersion<3){
			INT_64 nSeek;
			s->read(&nSeek, sizeof(INT_64));
		}
		if(nVersion<4){
			int count = 6;
			for(int i=0;i<count;i++)
			{
				if(nVersion > 1)
				{
					if(i == (count - 1))
						break;
				}
				INT nExist;
				s->read(&nExist, sizeof(INT));
				if(nExist > 0)
				{
					MeshPtr mesh;
					if(LoadMesh(s, mesh))
					{
						if(i<MAX_LOD_LEVEL_COUNT){
							m_pLODMeshs[i]	=	mesh.GetPtr();
							if(i == (count - 1))
								m_pLODMeshs[i] = 0;
						}
					}
				}
			}
		}
		else
		{
			for(int i=0;i<MAX_LOD_LEVEL_COUNT;i++)
			{
				INT nExist;
				s->read(&nExist, sizeof(INT));
				if(nExist > 0)
				{
					MeshPtr mesh;
					if(LoadMesh(s, mesh))
						m_pLODMeshs[i]	=	mesh.GetPtr();
				}
			}
		}
	}
	VOID				LODMesh::SaveLODMesh(StreamBase* s)
	{CPUTimeT(LODMesh);
		INT nSLOD = 1;
		s->write(&nSLOD, sizeof(nSLOD));
		INT nVersion = 4;
		s->write(&nVersion, sizeof(nVersion));
		s->write(m_nLodLevelFaces, sizeof(m_nLodLevelFaces));
		//
		for(int i=0;i<MAX_LOD_LEVEL_COUNT;i++)
		{
			INT nExist	=	(this->m_pLODMeshs[i] != NULL);
			s->write(&nExist, sizeof(INT));
			if(m_pLODMeshs[i])
				SaveMesh(s, m_pLODMeshs[i]);
		}
	}

	DeviceMesh*				LODMesh::GetLODMesh(unsigned int levelIndex, int& faceCount)
	{CPUTimeT(LODMesh);
		if(levelIndex < 0)
			return NULL;
		if(levelIndex >= MAX_LOD_LEVEL_COUNT)
			levelIndex	=	MAX_LOD_LEVEL_COUNT - 1;
		faceCount	=	this->m_nLodLevelFaces[ levelIndex ];
		return m_pLODMeshs[ levelIndex ];
	}

	VertexAnimationMesh::VertexAnimationMesh(void)
	{
		m_vertexMap				=	0;
		m_vertexFullCount		=	0;
		m_animationKeyCount		=	0;
		m_faceMap				=	0;
		m_faceCount				=	0;
		m_materialMap			=	0;
		m_materialCount			=	0;
		m_bTwoSide				=	0;
		m_meshVertexCount		=	0;
		m_lastVsps				=	0;
		m_technique.SetName("vsps_vertexAnimation");
		this->m_vertexLerp.SetName("m_fVertexAnimationLerp");
	}

	VertexAnimationMesh::~VertexAnimationMesh(void)
	{
		destroy();
	}

	void		VertexAnimationMesh::destroy()
	{CPUTimeT(VertexAnimationMesh);
		safe_deleteList(m_vertexMap);
		safe_deleteList(m_faceMap);
		safe_deleteList(m_materialMap);
		this->m_vertexFullCount		=	0;
		this->m_faceCount			=	0;
	}

	void		VertexAnimationMesh::RenderNoShader(Program* vsps, float fAnimationTime, BOOL bSetFrame)
	{CPUTimeT(VertexAnimationMesh);
		//float	fLerp	=	bSetFrame ? fAnimationTime : (fAnimationTime	/	m_fAnimationStep);
		//int		index	=	(int)fLerp;
		//fLerp			=	fLerp	-	(float)index;
		//index			%=	m_animationKeyCount;
		//vsps->SetFloat(this->m_vertexLerp, fLerp, m_vertexLerp.name);
		//for(int imtrl=0;imtrl<this->m_materialCount;imtrl++)
		//{
		//	AnimationVertexMul& mtrl = this->m_materialMap[imtrl];
		//	vsps->SetTexture(0, mtrl.texture, &mtrl.tap );
		//	vsps->commit();
		//	GetRenderHelperPtr()->drawIndexed(Phantom::DRAW_TYPE_TRIANGLELIST, m_vertexMap + (m_meshVertexCount * index), m_faceMap + mtrl.faceStart, m_meshVertexCount, mtrl.faceCount, sizeof(AnimationVertex), AnimationVertex::c_fvf, false);
		//}
	}

	void		VertexAnimationMesh::RenderNoShader(int attrID, Program* vsps, float fAnimationTime, BOOL bSetFrame)
	{CPUTimeT(VertexAnimationMesh);
		//float	fLerp	=	bSetFrame ? fAnimationTime : (fAnimationTime	/	m_fAnimationStep);
		//int		index	=	(int)fLerp;
		//fLerp			=	fLerp	-	(float)index;
		//index			%=	m_animationKeyCount;
		//if(index > (m_animationKeyCount - 2))
		//	index = (m_animationKeyCount - 2);
		//else if(index < 1)index = 1;
		//vsps->SetFloat(this->m_vertexLerp, fLerp, m_vertexLerp.name);
		//AnimationVertexMul& mtrl = this->m_materialMap[attrID];
		//vsps->commit();
		//GetRenderHelperPtr()->drawIndexed(Phantom::DRAW_TYPE_TRIANGLELIST, m_vertexMap + (m_meshVertexCount * index), m_faceMap + mtrl.faceStart, m_meshVertexCount, mtrl.faceCount, sizeof(AnimationVertex), AnimationVertex::c_fvf, false);
	}

	void		VertexAnimationMesh::Render(Program* vsps, float fAnimationTime)
	{CPUTimeT(VertexAnimationMesh);
		//float	fLerp	=	(fAnimationTime	/	m_fAnimationStep);
		//int		index	=	(int)fLerp;
		//fLerp			=	fLerp	-	(float)index;
		//index			%=	m_animationKeyCount;
		//vsps->SetFloat(this->m_vertexLerp, fLerp, m_vertexLerp.name);
		//for(int imtrl=0;imtrl<this->m_materialCount;imtrl++)
		//{
		//	AnimationVertexMul& mtrl = this->m_materialMap[imtrl];
		//	
		//	vsps->SetTexture(0, mtrl.texture, &mtrl.tap);
		//	vsps->commit();
		//	GetRenderHelperPtr()->drawIndexed(Phantom::DRAW_TYPE_TRIANGLELIST, m_vertexMap + (m_meshVertexCount * index), m_faceMap + mtrl.faceStart, m_meshVertexCount, mtrl.faceCount, sizeof(AnimationVertex), AnimationVertex::c_fvf, false);
		//}
	}

	void					VertexAnimationMesh::LoadGroupData(StreamBase* file)
	{CPUTimeT(VertexAnimationMesh);
		destroy();
		file->read(&m_vertexFullCount, sizeof(m_vertexFullCount));
		file->read(&m_animationKeyCount, sizeof(m_animationKeyCount));
		file->read(&m_meshVertexCount, sizeof(m_meshVertexCount));
		file->read(&m_materialCount, sizeof(m_materialCount));
		file->read(&m_faceCount, sizeof(m_faceCount));
		file->read(&m_fAnimationStep, sizeof(m_fAnimationStep));
		file->read(&m_vertexLerp, sizeof(m_vertexLerp));
		file->read(&m_bTwoSide, sizeof(m_bTwoSide));
		file->read(&m_technique, sizeof(m_technique));
		//
		m_vertexMap			=	new AnimationVertex[ m_vertexFullCount ];
		m_materialMap		=	new AnimationVertexMul[ m_materialCount ];
		m_faceMap			=	new Triangle[ m_faceCount ];
		//
		file->read(m_vertexMap, sizeof(AnimationVertex) * m_vertexFullCount);
		file->read(m_materialMap, sizeof(AnimationVertexMul) * m_materialCount);
		file->read(m_faceMap, sizeof(Triangle) * m_faceCount);
	}

	void					VertexAnimationMesh::SaveGroupData(StreamBase* file)
	{CPUTimeT(VertexAnimationMesh);
		//destroy();
		//this->m_szTexturePath		=	szTexturePath;

		//file->read(&m_vertexFullCount, sizeof(m_vertexFullCount));
		//file->read(&m_animationKeyCount, sizeof(m_animationKeyCount));
		//file->read(&m_meshVertexCount, sizeof(m_meshVertexCount));
		//file->read(&m_materialCount, sizeof(m_materialCount));
		//file->read(&m_faceCount, sizeof(m_faceCount));
		//file->read(&m_fAnimationStep, sizeof(m_fAnimationStep));
		//file->read(&m_vertexLerp, sizeof(m_vertexLerp));
		//file->read(&m_bTwoSide, sizeof(m_bTwoSide));
		//file->read(&m_technique, sizeof(m_technique));
		////
		//m_vertexMap			=	new AnimationVertex[ m_vertexFullCount ];
		//m_materialMap		=	new AnimationVertexMul[ m_materialCount ];
		//m_faceMap			=	new Triangle[ m_faceCount ];
		////
		//file->read(m_vertexMap, sizeof(AnimationVertex) * m_vertexFullCount);
		//file->read(m_materialMap, sizeof(AnimationVertexMul) * m_materialCount);
		//file->read(m_faceMap, sizeof(Triangle) * m_faceCount);
	}

	MeshShaderProject::MeshShaderProject()
	{
		m_defaultTechnique.SetName("tech_default");
		m_nShaderTechniqueChanged	=	1;
		m_id						=	0;
		m_name						=	"shader";
		//
		m_bIsForceAlphaBlend		=	false;
		m_srcBlend					=	Phantom::BlendMode_SRCALPHA;
		m_destBlend					=	Phantom::BlendMode_INVSRCALPHA;
		m_cullMode					=	Phantom::CullMode_CCW;
		m_bZWriteEnable				=	true;
	}
	MeshShaderProject::~MeshShaderProject()
	{
	}
	INT		MeshShaderProject::GetID()
	{CPUTimeT(MeshShaderProject);
		return m_id;
	}
	VOID	MeshShaderProject::CloneFrom(MeshShaderProject* s)
	{CPUTimeT(MeshShaderProject);
		if(!s)
			return;
		MeshShaderProject* shader	=	static_cast<MeshShaderProject*>(s);
		m_defaultTechnique.SetName(shader->m_defaultTechnique.name);
		m_variants		=	shader->m_variants;
	}
	void	MeshShaderProject::SetTehcnique(const char* s)
	{CPUTimeT(MeshShaderProject);
		m_nShaderTechniqueChanged++;
		if(s)
			m_defaultTechnique.SetName(s);
		else
			m_defaultTechnique.SetName("tech_default");
	}

	VOID								MeshShaderProject::Save(CFileHelperW& w)
	{CPUTimeT(MeshShaderProject);
		char version = 6;
		w << version;
		w << m_defaultTechnique.name.t;
		w << m_id;
		w << m_name;
		m_variants.Save(w);
		//
		w << m_bIsForceAlphaBlend << m_srcBlend << m_destBlend << m_bZWriteEnable;
		w << m_cullMode;
	}

	VOID								MeshShaderProject::Load(CFileHelperR& r)
	{CPUTimeT(MeshShaderProject);
		char version = 3;
		r >> version;
		std::string str;
		r >> str;
		m_defaultTechnique.name	=	str.c_str();
		int count = 0;
		if(version < 2)
			r >> count;
		if(version > 2)
			r >> m_id;
		r >> m_name;
		if(version >= 2)
			m_variants.Load(r);
		if(version >= 5)
			r >> m_bIsForceAlphaBlend >> m_srcBlend >> m_destBlend >> m_bZWriteEnable;
		if(version >= 6)
			r >> m_cullMode;
	}
	//
	//PShaderValue*					MeshShaderProject::GetShaderVariant(int index)
	//{
	//	if(index < 0 || index >= (int)m_shaderValues.size())
	//		return 0;
	//	return &m_shaderValues.at(index);
	//}
	//
	//void							MeshShaderProject::AddShaderVariant(const char* f, int type)
	//{
	//	for(size_t i=0;i<m_shaderValues.size();i++)
	//	{
	//		if(stricmp(m_shaderValues.at(i).var, f) == 0)
	//			return;
	//	}
	//	PShaderValue v;
	//	v.type		=	type;
	//	v.value.setXYZW(0,0,0,0);
	//	strcpy(v.var, f);
	//	m_shaderValues.push_back( v );
	//	m_nShaderVariantChanged++;
	//}
	//
	//void							MeshShaderProject::SetShaderVariantFloat4(const char* f, const float4& v)
	//{
	//	for(size_t i=0;i<m_shaderValues.size();i++)
	//	{
	//		if(stricmp(m_shaderValues.at(i).var, f) == 0)
	//		{
	//			m_nShaderVariantChanged++;
	//			m_shaderValues.at(i).value	=	v;
	//			return;
	//		}
	//	}
	//}
	//
	//void							MeshShaderProject::SetShaderVariantFloat(const char* f, float fvalue)
	//{
	//	for(size_t i=0;i<m_shaderValues.size();i++)
	//	{
	//		if(stricmp(m_shaderValues.at(i).var, f) == 0)
	//		{
	//			m_nShaderVariantChanged++;
	//			m_shaderValues.at(i).value.x	=	fvalue;
	//			return;
	//		}
	//	}
	//}
	//
	//void							MeshShaderProject::RemoveShaderVariant(const char* f)
	//{
	//	for(size_t i=0;i<m_shaderValues.size();i++)
	//	{
	//		if(stricmp(m_shaderValues.at(i).var, f) == 0)
	//		{
	//			m_nShaderVariantChanged++;
	//			m_shaderValues.erase(m_shaderValues.begin() + i);
	//			return;
	//		}
	//	}
	//}
	//
	//int								MeshShaderProject::GetShaderVariantCount()
	//{
	//	return (int)m_shaderValues.size();
	//}

	void							BoneMatrix::reset(int numBone)
	{CPUTimeT(BoneMatrix);
		if(boneMatrix)
			delete[] boneMatrix;
		if(boneIdx)
			delete[] boneIdx;
		boneMatrix		=	0;
		boneIdx			=	0;
		if(!numBone)return;
		boneMatrix		=	new matrix4x4[numBone];
		boneIdx			=	new int[numBone];
	}

	BoneMatrix::BoneMatrix()
	{
		boneMatrix		=	0;
		boneIdx			=	0;
		localBoneIdx	=	0;
	}
	BoneMatrix::~BoneMatrix()
	{
		if(boneMatrix)
			delete[] boneMatrix;
		if(boneIdx)
			delete[] boneIdx;
	}
	const color4*	MeshMaterial::GetDiffuseColor(int index)
	{CPUTimeNone();
		if(index<=0||index>DIFFUSE_TEXTURE_COUNT)
			return &this->diffuse;
		return &this->diffuseColor[index-1];
	}
	void			MeshMaterial::SetTexture(TEX_ID id, Texture* tex)	//设置贴图
	{CPUTimeNone();
		texturePtr[id]		=	tex;
	}

	Texture*		MeshMaterial::GetTexture(TEX_ID id)	//取得贴图
	{CPUTimeNone();
		return texturePtr[id];
	}

	MeshMaterial::MeshMaterial()
	{
		memset_s(texturePtr, 0, sizeof(texturePtr));
		programID[0]=programID[1]	=	0;
		programToSMID	=	0;
		bEmissiveZero	=	0;
		m_uvmapLoop		=	0;
		m_uvmapSpeed	=	20;
	}

	MeshMaterial::~MeshMaterial()
	{
		for(int i=0;i<TEXTURE_MAX_COUNT;i++)
			texturePtr[i] = 0;
		for(int i=0;i<DIFFUSE_TEXTURE_COUNT;i++)
			diffusePtr[i] = 0;
		for(int i=0;i<m_uvmaps.size();i++){
			safe_release(m_uvmaps[i]);
		}
		m_uvmaps.clear();
	}

	SubMeshPolygon::SubMeshPolygon(){
		m_b32BitMesh		=	false;
		m_bBlendVertex		=	0;		//是否是blend模型
		m_bHasTBN			=	0;		//是否存在tangent，binormal，normal
		m_boneCount			=	0;
		m_boneMap			=	0;
		m_pRenderMesh		=	0;
		m_bLodEnable		=	false;
		for(int i=0;i<MAX_LOD_LEVEL_COUNT;i++)
			m_levelLodIndex[i]	=	i;
		m_id				=	0;
		m_bOptimized		=	false;
	}
	SubMeshPolygon::~SubMeshPolygon(){
		this->m_lodMesh.DestroyLODMesh();
		m_pRenderMesh	=	0;
		if(m_boneMap)
			delete[]	m_boneMap;
		m_boneMap		=	0;
	}

	const Triangle*			SubMeshPolygon::GetFacePtr16(int index)
	{CPUTimeNone();
		if(!this->m_pRenderMesh)
			return 0;
		if(!m_b32BitMesh)
			return (Triangle*)m_pRenderMesh->GetIndices() + index;
		return 0;
	}
	const Triangle32*		SubMeshPolygon::GetFacePtr32(int index)
	{CPUTimeNone();
		if(!m_pRenderMesh)
			return 0;
		if(m_b32BitMesh)
			return (Triangle32*)m_pRenderMesh->GetIndices() + index;
		return 0;
	}

	int		SubMeshPolygon::GetLodLevelFaceCount(int level)	//返回lod对应的模型显示的面数量
	{CPUTimeNone();
		if(level < 0)
			level = 0;
		else if(level >= MAX_LOD_LEVEL_COUNT)
			level	=	MAX_LOD_LEVEL_COUNT - 1;
		if(!this->m_lodMesh.IsLODCreated())
			return 0;
		int faceCount = 0;
		if(m_lodMesh.GetLODMesh(level, faceCount))
			return faceCount;
		return 0;
	}
	VOID	SubMeshPolygon::ScaleAll(float fScale)
	{CPUTimeT(SubMeshPolygon);
		if(!m_pRenderMesh)return;
		int count = GetVertexCount();
		char* pVertex	=	(char*)this->m_pRenderMesh->GetVertexPtr();
		for(int i=0;i<count;i++){
			Vertex* v = (Vertex*)(pVertex + i * m_pRenderMesh->GetStrideSize());
			v->pos *= fScale;
		}
	}
	char	SubMeshPolygon::SaveData(CSafeFileHelperW& w)
	{CPUTimeT(SubMeshPolygon);
		int version = 3;
		w << version;
		skip_w sw(w.pStream);
		w << m_id << m_objectRegion << m_b32BitMesh << m_bLodEnable << m_bBlendVertex << m_bHasTBN << m_bOptimized;
		w << m_boneCount;
		if(m_boneCount>0)
			w.pStream->write(m_boneMap, sizeof(MeshBoneHelper)*m_boneCount);
		w.pStream->write(m_levelLodIndex, sizeof(m_levelLodIndex));
		SaveMesh(w.pStream, m_pRenderMesh);
		m_lodMesh.SaveLODMesh(w.pStream);
		sw.end();
		return true;
	}
	char	SubMeshPolygon::LoadData(CSafeFileHelperR& r)
	{CPUTimeT(SubMeshPolygon);
		int version = 2;
		r >> version;
		skip_r sr;
		if(version>=3)
			sr.begin(r.pStream);
		r >> m_id >> m_objectRegion >> m_b32BitMesh >> m_bLodEnable >> m_bBlendVertex >> m_bHasTBN >> m_bOptimized;
		r >> m_boneCount;
		if(m_boneCount>0)
		{
			m_boneMap = new MeshBoneHelper[m_boneCount];
			r.pStream->read(m_boneMap, sizeof(MeshBoneHelper)*m_boneCount);
		}
		if(version<2)
		{
			char	oldIndex[6];
			r.pStream->read(oldIndex, sizeof(oldIndex));
		}
		else
			r.pStream->read(m_levelLodIndex, sizeof(m_levelLodIndex));
		MeshPtr mesh;
		if(LoadMesh(r.pStream, mesh))
			m_pRenderMesh = mesh;
		m_lodMesh.LoadLODMesh(r.pStream);
		//
		sr.end();
		m_aaboxVertex.setup(m_objectRegion.aabox._min, m_objectRegion.aabox._max);
		//ComputeNormals();
		return true;
	}
	extern void				computeNormals(char* pVertex, int vertexWidth/*顶点数据类型字节数*/, int vertexCount, const void* faces, int faceCount, int posOffset, int normalOffset);	//构造模型法线
	void	SubMeshPolygon::ComputeNormals()
	{
		if(!this->m_pRenderMesh)return;
#ifdef WIN32_OS
		if(m_pRenderMesh->GetElementPtr()[0].usage==VertexElement::VE_USAGE_POSITION&&m_pRenderMesh->GetElementPtr()[1].usage==VertexElement::VE_USAGE_NORMAL)
			computeNormals((char*)m_pRenderMesh->GetVertexPtr(), m_pRenderMesh->GetStrideSize(), m_pRenderMesh->GetNumVertices(), (void*)m_pRenderMesh->GetIndices(), m_pRenderMesh->GetNumFaces(), 0, sizeof(float3));
#endif
	}
	int		SubMeshPolygon::GetLevelLodIndex(int level)	//返回lod对应的模型index
	{CPUTimeT(SubMeshPolygon);
		if(level < 0)
			level = 0;
		else if(level >= MAX_LOD_LEVEL_COUNT)
			level	=	MAX_LOD_LEVEL_COUNT - 1;
		return this->m_levelLodIndex[level];
	}

	void	SubMeshPolygon::SetLevelLodIndex(int level, int lodIndex)
	{CPUTimeT(SubMeshPolygon);
		if(level < 0)
			level = 0;
		else if(level >= MAX_LOD_LEVEL_COUNT)
			level	=	MAX_LOD_LEVEL_COUNT - 1;
		if(lodIndex < 0)lodIndex	=	0;
		else if(lodIndex >= MAX_LOD_LEVEL_COUNT)lodIndex	=	MAX_LOD_LEVEL_COUNT;
		//
		m_levelLodIndex[level]	=	lodIndex;
	}

	const Vertex*	SubMeshPolygon::GetVertexPtr(INT nIndex)
	{CPUTimeT(SubMeshPolygon);
		if(!m_pRenderMesh)
			return 0;
		char* pVertex	=	(char*)this->m_pRenderMesh->GetVertexPtr();
		return (Vertex*)(pVertex + nIndex * m_pRenderMesh->GetStrideSize());
	}

	SubMeshData::SubMeshData()
	{
		m_lodEnable			=	false;
		m_polygons			=	0;
		m_bUseWorld			=	0;
		m_renderMaterialIndex	=	0;
		m_bIsAlphaBlend		=	false;
		m_bZWriteEnable		=	true;
		m_bIsCollision		=	0;
		m_bIsHeight			=	0;//是否高度获取模型
		m_bIsDisable		=	0;//是否禁止行走的模型
		m_world.identity();
		//
		m_materials			=	0;
		m_numMaterials		=	0;
		m_bEnableShadow		=	true;
		m_bToShadowMap		=	true;
		m_nID				=	0;
		m_bExistMeshAnim	=	false;
		m_geometryType		=	0;
		m_bVisible			=	true;
		m_groupPtr			=	0;
		m_bExistShadowMap	=	0;
		memset(m_showOnAction, 0, sizeof(m_showOnAction));
		m_showOnActionCount	=	0;
	}

	char	SubMeshData::IsAlphaBlend()
	{CPUTimeNone();
		for(int i=0;i<this->m_numMaterials;i++)
			if(m_materials[i].alphaMode == AlphaMode_Blend)
				return true;
		return false;
	}

	int		SubMeshData::GetNewIdentity()
	{CPUTimeNone();
		return m_groupPtr->GetNewIdentity();
	}
	char	SubMeshData::SaveGroupData(CSafeFileHelperW& wIn)
	{CPUTimeT(SubMeshData);
		//m_bLoadOldData	=	false;
		m_lodEnable = m_polygons->m_bLodEnable;
		CFileHelperW w(wIn.pStream);
		int version	=	0x00000040;
		w << version;
		skip_w sw(w.pStream);
		w.bCheck	=	true;
		w << m_lodEnable;
		w << m_nID << m_boneLocal	<< m_numMaterials	<< m_bEnableShadow	<< m_bExistMeshAnim << m_geometryType << m_bExistShadowMap;
		w << m_bUseWorld << m_world;
		w << m_bZWriteEnable;
		w.str(m_name);
		w << m_bVisible;
		int polyid = m_polygons->m_id;
		w << polyid;
		for(int i=0;i<m_numMaterials;i++)
		{
			MeshMaterialAttributeBase& attr	=	m_materials[i];
			w << attr;
			w.str(m_materials[i].name);
			for(int tex=0;tex<TEXTURE_MAX_COUNT;tex++){
				w.str(m_materials[i].texture[tex]);
				//w << t.nMagFilter << t.nMinFilter << t.fMipMapLodBias;
			}
			char numDiffuse = DIFFUSE_TEXTURE_COUNT;
			w << numDiffuse;
			for(int d=0;d<DIFFUSE_TEXTURE_COUNT;d++)
			{
				w.str(m_materials[i].diffuseTexture[d]);
				w << m_materials[i].diffuseColor[d];
			}
			MaterialAnim& temp = m_materials[i].anims;
			w << temp.bHasAnim << temp.bTiling << temp.fPlaybackRate << temp.nStartFrame << temp.nUTile << temp.nVTile;
			int nKey = temp.keys.size();
			w << nKey;
			if(nKey > 0)
			{
				temp.keys.SetArrayCount(nKey);
				w.pStream->write(temp.keys.GetPtr(), temp.keys.size() * sizeof(MaterialColorKey));
			}
		}
		w << m_bIsCollision << m_bIsHeight << m_bIsDisable;//是否禁止行走的模型
		w << m_bToShadowMap;
		w.pStream->write(m_showOnAction, sizeof(m_showOnAction));
		sw.end();
		int	end	=	_MakeFileHead('g','e','o','1');
		wIn << end;
		return true;
	}
	char	SubMeshData::LoadGroupData(CSafeFileHelperR& rIn)
	{CPUTimeT(SubMeshData);
		CFileHelperR r(rIn.pStream);
		int version;
		r	>> version;
		skip_r sr;
		if(version>=0x00000034)
			sr.begin(r.pStream);
		if(version >= 0x00000032){
			r.bCheck	=	true;
			if(version >= 0x00000033)
				r >> m_lodEnable;
			r >> m_nID >> m_boneLocal	>> m_numMaterials	>> m_bEnableShadow	>> m_bExistMeshAnim >> m_geometryType >> m_bExistShadowMap;
			r >> m_bUseWorld >> m_world;
			r >> m_bZWriteEnable;
			r.str(m_name);
			r >> m_bVisible;
			int polyid = 0;
			r >> polyid;
			m_polygons = m_groupPtr->SearchPolygon(polyid);
			m_materials = new MeshMaterial[m_numMaterials];
			for(int i=0;i<m_numMaterials;i++)
			{
				MeshMaterialAttributeBase& attr	=	m_materials[i];
				r >> attr;
				m_materials[i].bEmissiveZero = (attr.emissive.r+attr.emissive.g+attr.emissive.b+attr.emissive.a)<0.01f;
				r.str(m_materials[i].name);
				for(int tex=0;tex<TEXTURE_MAX_COUNT;tex++){
					r.str(m_materials[i].texture[tex]);
					//w << t.nMagFilter << t.nMinFilter << t.fMipMapLodBias;
				}
				if(version>=0x00000035){
					char numDiffuse = 0;
					r >> numDiffuse;
					for(int d=0;d<numDiffuse;d++)
					{
						if(d>=DIFFUSE_TEXTURE_COUNT)
						{
							Name temp;
							r.str(temp);
							color4 tempc;
							r >> tempc;
						}
						else
						{
							r.str(m_materials[i].diffuseTexture[d]);
							r >> m_materials[i].diffuseColor[d];
						}
					}
				}
				else
				{
					for(int d=0;d<DIFFUSE_TEXTURE_COUNT;d++)
					{
						m_materials[i].diffuseColor[d] = m_materials[i].diffuse;
					}
				}
				if(version>=0x00000036){
					MaterialAnim& temp = m_materials[i].anims;
					r >> temp.bHasAnim >> temp.bTiling >> temp.fPlaybackRate >> temp.nStartFrame >> temp.nUTile >> temp.nVTile;
					int nKey;
					r >> nKey;
					if(nKey > 0)
					{
						temp.keys.SetArrayCount(nKey);
						r.pStream->read(temp.keys.GetPtr(), temp.keys.size() * sizeof(MaterialColorKey));
					}
				}
			}
			r >> m_bIsCollision >> m_bIsHeight >> m_bIsDisable;//是否禁止行走的模型
			if(version>=0x00000037)
				r >> m_bToShadowMap;
			if(version>=0x00000038)
			{
				r.pStream->read(m_showOnAction, sizeof(int));
				m_showOnActionCount = 0;
				for(int i=0;i<MAX_SHOW_ON_ACTION;i++){
					if(m_showOnAction[i]>0)
						m_showOnActionCount++;
					else
						break;
				}
			}
		}
		else{
			if(version >= 0x00000022)
				r.bCheck	=	true;
			m_polygons	=	m_groupPtr->CreatePolygon();
			int							vertexCount = 0;
			int							faceCount = 0;
			char						bUseObjectRegion;
			int							nMinFilter;
			int							nMagFilter;
			int							nMaxAnisotropy;
			float						fMipMapLodBias;
			float						fSlopeScaleDepthBias, fDepthBias;
			if(version < 0x00000027){
				char								bIsAnimationMesh;
				char								bIsVertexColor;
				char								bIsNormalMapMesh;
				char								bIsVertexAnimation;
				char								bReserved;
				r	>> m_nID >> bIsAnimationMesh	>> bIsVertexColor >> bIsNormalMapMesh	>> m_polygons->m_boneCount
					>> m_boneLocal	>> vertexCount		>> faceCount			>> m_numMaterials	>> m_bEnableShadow		>> bUseObjectRegion
					>> m_polygons->m_objectRegion	>>	bIsVertexAnimation >> m_bExistMeshAnim >> m_polygons->m_bBlendVertex >> m_polygons->m_bHasTBN	>> fMipMapLodBias
					>> nMinFilter >> nMagFilter >> nMaxAnisotropy >> m_geometryType >> bReserved;
			}
			else
			{
				r	>> m_nID >> m_polygons->m_boneCount
					>> m_boneLocal	>> m_numMaterials	>> m_bEnableShadow		>> bUseObjectRegion
					>> m_polygons->m_objectRegion	>> m_bExistMeshAnim >> m_polygons->m_bBlendVertex >> m_polygons->m_bHasTBN >> fMipMapLodBias
					>> nMinFilter >> nMagFilter >> nMaxAnisotropy >> m_geometryType;
			}
			m_polygons->m_aaboxVertex.setup(m_polygons->m_objectRegion.aabox._min, m_polygons->m_objectRegion.aabox._max);
			char bExistShadowMap;
			if(version>=0x00000030)
				r >> bExistShadowMap;
			if(version>=0x00000031)
				r >> m_bUseWorld >> m_world;
			if(version >= 0x00000012)
				r >> m_polygons->m_b32BitMesh;
			if(version >= 0x00000015)
				r >> m_bZWriteEnable;
			//
			char			bLookatCamera;
			float			fLookatCameraSize;
			if(version > 0x00000002)
				r >> bLookatCamera >> fLookatCameraSize;
			if(version > 0x00000008)
				r >> fSlopeScaleDepthBias >> fDepthBias;
			//
			int supportEffectCount = 0;
			if(version < 0x00000007)
				r >> supportEffectCount;
			std::string strName;
			if(version < 0x00000018)
			{
				r >> strName;
				//m_defaultShader[0].m_defaultTechnique.SetName(strName.c_str());
			}
			if(version > 0x0000000A && version < 0x00000027)
			{
				char bExist;
				r >> bExist;
				if(bExist)
					r >> strName;
			}
			//
			r >> strName;
			m_name = strName.c_str();
			//
			if(supportEffectCount > 0)
			{
				if(version < 0x00000007)
				{
					for(int i=0;i<supportEffectCount;i++)
					{
						int supportID;
						if(version < 0x00000006)
							r >> strName >> supportID;
						else
							r >> supportID;
					}
				}
			}
			if(version >= 0x00000007 && version < 0x00000027)
			{
				char bSupportEffects[10];
				r.pStream->read(bSupportEffects, sizeof(bSupportEffects));
			}
			//
			if(version < 0x00000027){
				r >> strName;
			}
			//
			r >> m_bVisible;
			//m_defaultShader[0].m_id	=	this->m_groupPtr->GetNewIdentity();
			if(version > 0x00000018 && version<0x00000026)
			{
				MeshShaderProject temp;
				temp.Load(r);
				//m_defaultShader[0].Load(r);
				int count	=	0;
				r >> count;
				for(int i=1;i<count;i++)
				{
					temp.Load(r);
				}
				if(version >= 0x00000021)
				{
					for(int i=1;i<MeshShaderType_Count;i++)
						temp.Load(r);
				}
			}
			if(version < 0x00000027){
				int fileSeekMesh;
				PFileSeek temp;
				r.pStream->read(&fileSeekMesh, sizeof(int));
				r.pStream->read(&temp, sizeof(PFileSeek));
			}
			//r.pStream->seek(m_fileSeek.end);
			r >> m_polygons->m_boneCount;
			if(m_polygons->m_boneMap)
				delete[] m_polygons->m_boneMap;
			if(m_polygons->m_boneCount > 0)
				m_polygons->m_boneMap	=	new MeshBoneHelper[m_polygons->m_boneCount];
			for(int i=0;i<m_polygons->m_boneCount;i++)
				r >> m_polygons->m_boneMap[i];
			if(!m_materials)
				m_materials	=	new MeshMaterial[m_numMaterials];
			int nTest = 0;
			int nBlend = 0;
			for(int i=0;i<m_numMaterials;i++)
			{
				MeshMaterialAttribute& attr	=	m_materials[i];
				if(version < 0x00000029){
					int textureCount = 0;
					r >> textureCount;
					for(int itex=0;itex<textureCount;itex++)
					{
						if(version < 0x00000028){
							MaterialTexture temp;
							r >> temp;
							attr.texture[itex] = temp.file;
						}
						else{
							short len = 0;
							r >> len;
							if(len>0){
								r.pStream->read(attr.texture[itex].t, len);
								attr.texture[itex].t[len] = 0;
							}
						}
					}
					r >> attr.cullMode >> attr.alphaMode;
					if(version < 0x00000028)
					{
						int faceCount2;
						float animationSpeed;
						r >> faceCount2 >> animationSpeed;
					}
					r >> attr.diffuse >> attr.emissive >> attr.specular;
					if(version > 0x00000027)
						r >> attr.ambient;
					if(version < 0x00000020)
						attr.emissive.setRGBA(0, 0, 0, 0);
					if(version >= 0x00000008)
						r >> attr.srcBlend >> attr.destBlend;
					if(version > 0x00000009)
						r >> attr.visibleInShadowMap;
					if(version > 0x00000010)
						r >> attr.lodEnable;
					if(version < 0x00000013 && version >= 0x00000012)
						attr.lodEnable	=	true;
					if(version >= 0x00000017)
						r >> attr.visible;
					else
						attr.visible	=	true;
					if(attr.alphaMode == Phantom::AlphaMode_Test)
						nTest = true;
					else if(attr.alphaMode == Phantom::AlphaMode_Blend)
						nBlend = true;
					if(version >= 0x00000023 && version < 0x00000029)
					{
						//TODO: 更新
						MaterialAnim& temp = m_materials[i].anims;
						r >> temp.bHasAnim >> temp.bTiling >> temp.fPlaybackRate >> temp.nStartFrame >> temp.nUTile >> temp.nVTile;
						int nKey;
						r >> nKey;
						if(nKey > 0)
						{
							temp.keys.SetArrayCount(nKey);
							r.pStream->read(temp.keys.GetPtr(), temp.keys.size() * sizeof(MaterialColorKey));
						}
					}
					if(version >= 0x00000025 && version < 0x00000028)
					{
						char		bTextureChange,bColorChange;
						r >> bTextureChange >> bColorChange;
					}
				}
				else{
					MeshMaterialAttributeBase& base = attr;
					r >> base;
					char textureCount = TEXTURE_MAX_COUNT;
					r >> textureCount;
					for(int itex=0;itex<textureCount;itex++)
					{
						char len = attr.texture[itex].size();
						r >> len;
						if(len>0)
						{
							r.pStream->read(attr.texture[itex].t, len);
							attr.texture[itex].t[len] = 0;
						}
					}
					char len = attr.name.size();
					r >> len;
					if(len>0)
					{
						r.pStream->read(attr.name.t, len);
						attr.name.t[len] = 0;
					}
				}
			}
			//
			//for(int i=0;i<this->m_numMaterials;i++)
			//{
			//	if(m_materials[i].faceCount <= 8)
			//	{
			//		m_bLodEnable	=	false;
			//		break;
			//	}
			//}
			VertexElement*						elements = 0;		//保存渲染数据
			int									elementCount = 0;	//渲染顺序数量
			if(version < 0x00000027)
			{
				r >> elementCount;
				if(elementCount > 0)
				{
					elements	=	(VertexElement*)malloc_s(sizeof(VertexElement) * elementCount);
					r.pStream->read(elements, sizeof(VertexElement) * elementCount);
				}
			}
			char	bRender;
			char	bShadow;
			char	bAnimation = 0;
			r >> bRender >> bShadow;
			if(version < 0x00000027)
				r >> bAnimation;
			//
			if(bRender&&version < 0x00000027)
			{
				INT_64 nSeek;
				r.pStream->read(&nSeek, sizeof(INT_64));
			}
			//这里读取不对
			if(version < 0x00000027)
			{
				m_polygons->m_pRenderMesh	=	0;
				MeshPtr mesh;
				g_manager.CreateMesh(mesh);
				mesh->Create(vertexCount, faceCount, elements, elementCount);
				//
				int vertexWidth = mesh->GetStrideSize();
				void*							trianglePtr = 0;
				char*							vertexs = 0;
				unsigned char*	attributes = 0;
				trianglePtr		=	malloc_s((m_polygons->m_b32BitMesh ? sizeof(Triangle32) : sizeof(Triangle)) * faceCount);
				vertexs			=	(char*)malloc_s(vertexWidth * vertexCount);
				attributes		=	(unsigned char*)malloc_s(sizeof(unsigned char) * faceCount);
				rIn.pStream->read(trianglePtr, (m_polygons->m_b32BitMesh ? sizeof(Triangle32) : sizeof(Triangle)) * faceCount);
				rIn.pStream->read(vertexs, vertexWidth * vertexCount);
				rIn.pStream->read(attributes, sizeof(unsigned char) * faceCount);
				//
				mesh->WriteVertexBuffer(vertexs, vertexCount, vertexWidth);
				mesh->WriteIndexBuffer((unsigned short*)trianglePtr, faceCount * 3, attributes);
				mesh->Commit();
				m_polygons->m_pRenderMesh	=	mesh.GetPtr();
				safe_free(trianglePtr);
				safe_free(vertexs);
				safe_free(attributes);
				safe_free(elements);
				//
				CheckTypeFromName();
			}
			else
			{
				MeshPtr mesh;
				if(LoadMesh(r.pStream, mesh))
					m_polygons->m_pRenderMesh = mesh;
				r >> m_bIsCollision >> m_bIsHeight >> m_bIsDisable;//是否禁止行走的模型
			}
			if(bAnimation)
			{
				VertexAnimationMesh temp;
				temp.LoadGroupData(r.pStream);
				//m_animationMesh			=	new VertexAnimationMesh();
				//m_animationMesh->LoadGroupData(r.pStream, this->m_szTexturePath);
			}
			if(version >= 0x00000005)
			{
				char	oldIndex[6];
				r.pStream->read(oldIndex, sizeof(oldIndex));
				m_polygons->m_lodMesh.LoadLODMesh( r.pStream, version < 0x00000010);
			}
			//
		}
		sr.end();
		int	end	=	_MakeFileHead('g','e','o','1');
		int	endRead;
		rIn	>> endRead;
		if(endRead != end)
		{
			return false;
		}
		if(version<0x00000040)
			m_bEnableShadow = true;
		ReloadAllTextures();
		SortAlphaBlendGeometrys();
		this->m_bIsAlphaBlend = IsAlphaBlend() != 0;
		//if(version<0x00000039)
			//this->m_polygons->ComputeNormals();
		return true;
	}
	VOID	SubMeshData::CheckTypeFromName()
	{CPUTimeT(SubMeshData);
#ifdef WIN32_OS
		strlwr_t(this->m_name.t);
		if(strstr(m_name.t, "coli.")||strstr(m_name.t, "colishow."))// && geometryDataMap[i]->m_collisionMesh)
		{
			m_bIsCollision		=	true;
			m_bVisible			=	(strstr(m_name.t, "colishow.")!=0);
		}
		else if(strstr(m_name.t, "height.") || strstr(m_name.t, "floor."))
		{
			m_bIsHeight			=	true;
			m_bVisible			=	false;
		}
		//else if(strstr(m_name.t, "disable."))
		//{
		//	m_bIsDisable		=	true;
		//	m_bVisible			=	false;
		//	m_bVisible			=	false;
		//}
		else if(strstr(m_name.t, "bind.") || strstr(m_name.t, "box."))
		{
			m_bVisible			=	false;
		}
		if(strstr(m_name.t, "snmap."))
			this->m_bToShadowMap = false;
		if(strstr(m_name.t, "snwith."))
			this->m_bEnableShadow = false;
		if(strstr(m_name.t, "zdisable."))
			this->m_bZWriteEnable = false;
		this->m_showOnActionCount = 0;
		for(int i=1;i<ActionType_Count;i++){
			char buf[128];
			sprintf(buf, "a_%s", GetActionTypeName(i));
			strlwr(buf);
			if(strstr(m_name.t, buf))
			{
				if(m_showOnActionCount>=MAX_SHOW_ON_ACTION)
					break;
				this->m_showOnAction[m_showOnActionCount] = i;
				this->m_bVisible = false;
			}
		}
#endif
	}
#ifdef DIRECT_X_9
	extern	void	ConvertToPng(const char* szFile, BOOL bRemoveOld = true);
#endif
	BOOL	LoadTexture(TexturePtr& r, char* szTextureFile, const char* szTexturePath, BOOL bGenMipmap, BaseDirectory* basedir)
	{
#ifdef DIRECT_X_9
		char szExt[128], szFile[128], szDir[256];
		_splitpath(szTextureFile, 0, szDir, szFile, szExt);
		if(stricmp(szExt, ".dds")==0||stricmp(szExt, ".tga") == 0||stricmp(szExt, ".bmp") == 0){
			char szTemp[512];
			BOOL bConv = false;
			FILE* f = fopen(szTextureFile, "rb");
			if(f){
				fclose(f);
				bConv = true;
				strcpy(szTemp, szTextureFile);
			}
			char szFileTemp[512];
			strcpy(szFileTemp, szTexturePath);
			strcat(szFileTemp, szFile);
			if(!bConv){
				strcpy(szTemp, szFileTemp);
				strcat(szTemp, ".dds");
				f = fopen(szTemp, "rb");
				if(f){
					fclose(f);
					bConv = true;
				}
			}
			if(!bConv){
				strcpy(szTemp, szFileTemp);
				strcat(szTemp, ".tga");
				f = fopen(szTemp, "rb");
				if(f){
					fclose(f);
					bConv = true;
				}
			}
			if(!bConv){
				strcpy(szTemp, szFileTemp);
				strcat(szTemp, ".bmp");
				f = fopen(szTemp, "rb");
				if(f){
					fclose(f);
					bConv = true;
				}
			}
			if(bConv){
				ConvertToPng(szTemp);
			}
			strcpy(szTextureFile, szDir);
			strcat(szTextureFile, szFile);
			strcat(szTextureFile, ".png");
			strlwr(szTextureFile);
		}
		else if(stricmp(szExt, ".png")==0||stricmp(szExt, ".jpg") == 0){
			BOOL bLower = true;
			int len = strlen(szFile);
			for(int i=0;i<len;i++){
				if(isupper(szFile[i]))
				{
					bLower = false;
					break;
				}
			}
			if(!bLower){
				char szTemp[512];
				BOOL bConv = false;
				char szFileTemp[512];
				strcpy(szFileTemp, szTexturePath);
				strcat(szFileTemp, szFile);
				FILE* f;
				if(!bConv){
					strcpy(szTemp, szFileTemp);
					strcat(szTemp, ".png");
					f = fopen(szTemp, "rb");
					if(f){
						fclose(f);
						bConv = true;
					}
				}
				if(!bConv){
					strcpy(szTemp, szFileTemp);
					strcat(szTemp, ".jpg");
					f = fopen(szTemp, "rb");
					if(f){
						fclose(f);
						bConv = true;
					}
				}
				if(bConv){
					char szNewName[512];
					strcpy(szNewName, szTemp);
					strlwr(szNewName);
					::rename(szTemp, szNewName);
				}
				strlwr(szTextureFile);
			}
		}
#endif
		LFileName ret;
		if(fexist(szTextureFile, &ret))
		{
			return g_manager.LoadTexture(r, szTextureFile, bGenMipmap, 2, true, basedir);//return g_manager.LoadTexture(r, ret, bGenMipmap);
		}
		FileName fileName, origDir;
		FileName path, dir, file, ext;
		file	=	szTextureFile;
		file.setIsFile();
		file.splitpath(&origDir, &fileName, &ext);
		//
		file		=	fileName.t;
		file		+=	".png";
		dir			=	origDir;
		dir			+=	file.t;
		if(fexist(dir.t, &ret))
			return g_manager.LoadTexture(r, dir.t, bGenMipmap, 2, true, basedir);
		//
		file		=	fileName.t;
		file		+=	".jpg";
		dir			=	origDir;
		dir			+=	file.t;
		if(fexist(dir.t, &ret))
			return g_manager.LoadTexture(r, dir.t, bGenMipmap, 2, true, basedir);
		//
		//尝试本地目录.tga, .png
		file		=	fileName.t;
		file		+=	".limg";
		dir			=	origDir;
		dir			+=	file.t;
		if(fexist(dir.t, &ret))
		{
			return g_manager.LoadTexture(r, dir.t, bGenMipmap, 2, true, basedir);
		}
		path = szTexturePath;
		path.setIsFile();
		path.splitpath(&origDir, 0, 0);
		//尝试本地目录
		file		=	fileName.t;
		file		+=	ext.t;
		dir			=	origDir;
		dir			+=	file.t;
		if(fexist(dir.t, &ret))
		{
			return g_manager.LoadTexture(r, dir.t, bGenMipmap, 2, true, basedir);
		}
		file		=	fileName.t;
		file		+=	".png";
		dir			=	origDir;
		dir			+=	file.t;
		if(fexist(dir.t, &ret))
			return g_manager.LoadTexture(r, dir.t, bGenMipmap, 2, true, basedir);
		//
		file		=	fileName.t;
		file		+=	".jpg";
		dir			=	origDir;
		dir			+=	file.t;
		if(fexist(dir.t, &ret))
			return g_manager.LoadTexture(r, dir.t, bGenMipmap, 2, true, basedir);
		//尝试本地目录.tga, .png
		file		=	fileName.t;
		file		+=	".limg";
		dir			=	origDir;
		dir			+=	file.t;
		InitMyFile(dir.t, true, false);
		if(fexist(dir.t, &ret))
		{
			return g_manager.LoadTexture(r, dir.t, bGenMipmap, 2, true, basedir);
		}
		return false;
	}
	MeshMaterial*	SubMeshData::GetMaterialPtr(int nIndex)
	{CPUTimeNone();
		if(nIndex >= m_numMaterials || nIndex < 0)
			return NULL;
		if(!m_materials)
			return 0;
		return &m_materials[nIndex];
	}
	Texture*	SubMeshData::GetDiffuseTexture(int material, int index, float fuvmapIndex, BOOL& bRender)
	{CPUTimeNone();
		bRender = true;
		if(index <= 0 || index > DIFFUSE_TEXTURE_COUNT)
		{
			if(index == 0 && m_materials[material].m_uvmaps.size()>0){
				int uvmapIndex = (int)(fuvmapIndex*m_materials[material].m_uvmapSpeed);
				if(uvmapIndex<0)uvmapIndex=0;
				int count = m_materials[material].m_uvmaps.size() + 1;
#ifdef EDITOR_OS
				uvmapIndex%=count;
#else
				if(m_materials[material].m_uvmapLoop)
					uvmapIndex%=count;
				else if(uvmapIndex >= count)
					uvmapIndex = count-1;
#endif
				if(uvmapIndex==0)
					return m_materials[material].texturePtr[0];
				Texture* tex = m_materials[material].m_uvmaps[uvmapIndex-1];
				if(!tex)
					bRender = false;
				return tex;
			}
			return m_materials[material].texturePtr[0];
		}
		index--;
		if(m_materials[material].diffusePtr[index].t)
		{
			return m_materials[material].diffusePtr[index];
		}
		if(m_materials[material].diffuseTexture[index].t[0])
		{
			TexturePtr ret;
#ifdef DIRECT_X_9
			FileName fileName, szTexturePath;
			m_groupPtr->m_filename.splitpath(&szTexturePath, &fileName, 0);
			Name dir,file,ext;
			m_materials[material].diffuseTexture[index].splitpath(&dir, &file, &ext);
			if(ext==".tga"||ext==".bmp"||ext==".dds"){
				dir += file;
				dir += ".png";
				ConvertToPng(szTexturePath.t, m_materials[material].diffuseTexture[index].t, dir.t);
			}
#endif
			if(m_groupPtr->LoadTexture( ret, m_materials[material].diffuseTexture[index].t ))
			{
				m_materials[material].diffusePtr[index] = ret.GetPtr();
				return m_materials[material].diffusePtr[index];
			}
			else
				m_materials[material].diffuseTexture[index].t[0] = 0;
		}
		return m_materials[material].texturePtr[0];
	}
	char	SubMeshData::ReloadAllTextures(const char* szSrcPath)
	{CPUTimeNone();
		if(!m_materials)return false;
		char bNormalMap	=	false;
		char bret	=	true;
		for(int i=0;i<this->m_numMaterials;i++)
		{
			for(int itex=0;itex<TEXTURE_MAX_COUNT;itex++)
			{
				if(m_materials[i].texture[itex].t[0])
				{
					Texture* oldTex	=	m_materials[i].texturePtr[itex];
					TexturePtr ret;
#ifdef DIRECT_X_9
					FileName fileName, szTexturePath;
					m_groupPtr->m_filename.splitpath(&szTexturePath, &fileName, 0);
					Name dir,file,ext;
					m_materials[i].texture[itex].splitpath(&dir, &file, &ext);
					if(ext==".tga"||ext==".bmp"||ext==".dds"){
						dir += file;
						dir += ".png";
						ConvertToPng(szTexturePath.t, m_materials[i].texture[itex].t, dir.t);
					}
#endif
					if(m_groupPtr->LoadTexture( ret, m_materials[i].texture[itex].t ))
					{
						m_materials[i].SetTexture( (TEX_ID)itex,  ret.GetPtr() );
						if(m_materials[i].alphaMode == AlphaMode_Blend)
							m_groupPtr->m_isAlphaBlend = true;
						if(itex==Phantom::TEX_DIFFUSE){
							for(int k=0;k<m_materials[i].m_uvmaps.size();k++){
								safe_release(m_materials[i].m_uvmaps[k]);
							}
							Name dname,dext;
							m_materials[i].texture[itex].splitpath(0, &dname, &dext);
							if(strnicmp(dname.t, "uvmap_", 6)==0){
								m_materials[i].m_uvmaps.clear();
								FileName fdir,fname,ftemp;
								ret->GetFileName().splitpath(&fdir, &fname, 0);
								ftemp = fdir;
								ftemp += fname;
								ftemp += ".txt";
								if(fexist(ftemp.t)){
									TableTxt filelist;
									if(filelist.Load(ftemp.t))
									{
										for(int is=0;is<filelist.GetNumofLine();is++){
											const char* file = filelist.read(is, "file");
											if(file&&file[0])
											{
												ftemp = fdir;
												ftemp += file;
												if(stricmp(file,"null")==0){
													Texture* tt = 0;
													m_materials[i].m_uvmaps.push_back(tt);
												}
												else if(strnicmp(file, "speed(", 6)==0){
													m_materials[i].m_uvmapSpeed = atof(file + 6);
												}
												else if(stricmp(file,"loop")==0){
													m_materials[i].m_uvmapLoop = true;
													break;
												}
												else{
													TexturePtr tttemp;
													if(m_groupPtr->LoadTexture( tttemp, ftemp.t)){
														tttemp->AddRef();
														m_materials[i].m_uvmaps.push_back(tttemp);
													}
												}
											}
										}
									}
								}
							}
						}
					}
					else
						m_materials[i].SetTexture( (TEX_ID)itex,  0 );
				}
				else
					m_materials[i].SetTexture( (TEX_ID)itex,  0 );
			}
		}
		//for(int i=0;i<this->m_numMaterials;i++)
		//{
		//	if(!m_materials[i].texturePtr[0])
		//	{
		//		m_hasColorMaterial	=	1;
		//		break;
		//	}
		//}
		return bret;
	}

	SubMeshData::~SubMeshData(void)
	{
		m_polygons = 0;
		if(m_renderMaterialIndex)
			free_s(m_renderMaterialIndex);
		destroy();
	}

	void						SubMeshData::SortAlphaBlendGeometrys()
	{CPUTimeT(SubMeshData);
		if(!m_numMaterials)return;
		if(!this->m_renderMaterialIndex)
			m_renderMaterialIndex	=	(int*)malloc_s(sizeof(int) * m_numMaterials);
		int alphaCount = 0;
		int* alphaList = new int[m_numMaterials];
		int index	=	0;
		for(int i=0;i<m_numMaterials;i++)
		{
			if( m_materials[i].alphaMode != AlphaMode_Blend )
				m_renderMaterialIndex[index++]	=	i;
			else
				alphaList[alphaCount++]	=	i;
		}
		for(int i=0;i<alphaCount;i++)
		{
			m_renderMaterialIndex[index++]	=	alphaList[i];
		}
		delete[] alphaList;
	}

	void	SubMeshData::renderLODData(Program* prog, int attribID, int nLod)
	{CPUTimeT(SubMeshData);
		if(!m_polygons->m_pRenderMesh.t)
			return;
		if(!m_materials[attribID].lodEnable || nLod == 0 || !m_polygons->m_lodMesh.IsLODCreated())
		{
			m_polygons->m_pRenderMesh->RenderSubset( prog, attribID );
		}
		else
		{
			nLod--;
			nLod	=	m_polygons->m_levelLodIndex[nLod];
			//
			if(nLod >= m_polygons->m_lodMesh.GetLODMeshCount())
				nLod = m_polygons->m_lodMesh.GetLODMeshCount() - 1;
			int faces;
			DeviceMesh* pMesh = m_polygons->m_lodMesh.GetLODMesh(nLod, faces);
			if(pMesh)
			{
				pMesh->RenderSubset( prog, attribID );
			}
		}
	}

	BOOL	SubMeshData::AddRenderTriangles(int nLod)
	{CPUTimeNone();
		//if(!this->m_pMaterialMap[0].lodEnable || nLod == 0 || !this->m_lodMesh.IsLODCreated())
		//	return ::AddRenderTriangles( this->m_faceCount );
		//else
		//{
		//	nLod	=	m_levelLodIndex[nLod--];
		//	//
		//	if(nLod >= m_lodMesh.GetLODMeshCount())
		//		nLod = m_lodMesh.GetLODMeshCount() - 1;
		//	int faces;
		//	DeviceMesh* pMesh = m_lodMesh.GetLODMesh(nLod, faces);
		//	return ::AddRenderTriangles( faces );
		//}
		return FALSE;
	}

	void	SubMeshData::renderShadowData()
	{CPUTimeNone();
		//if(this->m_pShadowMesh)
		//	this->m_pShadowMesh->DrawSubset( 0 );
	}

	void	SubMeshData::destroy()
	{CPUTimeT(SubMeshData);
		if(m_materials)
		{
			for(int i=0;i<this->m_numMaterials;i++)
			{
				for(int itex=0;itex<TEXTURE_MAX_COUNT;itex++)
				{
					m_materials[i].SetTexture((TEX_ID)itex, 0);
				}
			}
		}
		if(m_materials)
			delete[]	m_materials;
		m_materials			=	0;
		m_numMaterials		=	0;
		m_bEnableShadow		=	false;
	}

	////////////////////////////////////////////////////////////////////////////
	//TODO: 
	//SubMeshData::create
	//
	////////////////////////////////////////////////////////////////////////////
	BOOL	IsPointEqual(const float3* v1, const float3* v2, float fPrecision)
	{
		float x	=	v1->x - v2->x;
		float y =	v1->y - v2->y;
		float z =	v1->z - v2->z;
		return ((x < fPrecision && x > -fPrecision)
			&& (y < fPrecision && y > -fPrecision)
			&& (z < fPrecision && z > -fPrecision));
	}

	struct EqualPointData
	{
		float3					point;
		DWORD					pointID;
		char					bcalc;
	};
#pragma pack(push, 1)
	struct VertexTempBlend//带有骨骼动画的模型顶点数据类型
	{
		float3							pos/*位置*/;
		float3							normal/*法线*/;
		unsigned int					color;
		float2							uv1;/*纹理坐标1*/;
		float2							uv2;/*纹理坐标1*/;
		float							weight[4];
		unsigned char					boneID[4];
	};
	struct VertexTempBlendNew//带有骨骼动画的模型顶点数据类型
	{
		float3							pos/*位置*/;
		float3							normal/*法线*/;
		float2							uv1;/*纹理坐标1*/;
		float							weight[4];
		unsigned char					boneID[4];
	};
	struct VertexTemp
	{
		float3							pos/*位置*/;
		float3							normal/*法线*/;
		float2							uv1;/*纹理坐标1*/;
	};
#pragma pack(pop)
	AABox	SubMesh::RecalcIdleBox()
	{CPUTimeT(SubMesh);
		AABox temp = this->m_geometryData->m_polygons->m_objectRegion.aabox;
		if(!this->m_geometryData->m_polygons->m_pRenderMesh)
			return temp;
		if(this->m_boneMatrix.boneMatrix&&m_geometryData->m_polygons->m_boneMap)
		{
			matrix4x4*	matList = m_boneMatrix.boneMatrix;
			DeviceMesh* mesh	=	m_geometryData->m_polygons->m_pRenderMesh;
			//assert(sizeof(VertexTempBlend)==mesh->GetElementWidth());
			if(sizeof(VertexTempBlend)==mesh->GetElementWidth()){
				//	return temp;
				char* pVertex	=	(char*)mesh->GetVertexPtr();//m_vertexs;
				temp._min.setxyz(100000,100000,100000);
				temp._max.setxyz(-100000,-100000,-100000);
				float3 v0,v1,v2,v3;
				for(int ivert = 0;ivert<mesh->GetNumVertices();ivert++)
				{
					float3 dest;
					dest.setxyz(0,0,0);
					VertexTempBlend* pBlend	=	mesh->GetVertex<VertexTempBlend>(ivert);
					for(int iB=0;iB<4;iB++)
					{
						if(pBlend->boneID[iB]<m_geometryData->m_polygons->m_boneCount)
						{
							float3 v1 = pBlend->pos;
							v1.transform( v1, matList[ pBlend->boneID[iB] ] );
							v1 = v1 * pBlend->weight[iB];
							dest += v1;
						}
					}
					temp._min.minv(temp._min, dest);
					temp._max.maxv(temp._max, dest);
				}
			}
			else if(sizeof(VertexTempBlendNew)==mesh->GetElementWidth()){
				//	return temp;
				char* pVertex	=	(char*)mesh->GetVertexPtr();//m_vertexs;
				temp._min.setxyz(100000,100000,100000);
				temp._max.setxyz(-100000,-100000,-100000);
				float3 v0,v1,v2,v3;
				for(int ivert = 0;ivert<mesh->GetNumVertices();ivert++)
				{
					float3 dest;
					dest.setxyz(0,0,0);
					VertexTempBlendNew* pBlend	=	mesh->GetVertex<VertexTempBlendNew>(ivert);
					for(int iB=0;iB<4;iB++)
					{
						if(pBlend->boneID[iB]<m_geometryData->m_polygons->m_boneCount)
						{
							float3 v1 = pBlend->pos;
							v1.transform( v1, matList[ pBlend->boneID[iB] ] );
							v1 = v1 * pBlend->weight[iB];
							dest += v1;
						}
					}
					temp._min.minv(temp._min, dest);
					temp._max.maxv(temp._max, dest);
				}
			}
		}
		return temp;
	}
	void	SubMesh::RemoveBoneIfOne()//如果只有一个关键帧则去掉所有动作
	{CPUTimeT(SubMesh);
		if(!this->m_geometryData->m_polygons->m_pRenderMesh)
			return;
		if(this->m_boneMatrix.boneMatrix&&m_geometryData->m_polygons->m_boneMap)
		{
			matrix4x4*	matList = m_boneMatrix.boneMatrix;
			DeviceMesh* mesh	=	m_geometryData->m_polygons->m_pRenderMesh;
			//CDynamicArray<float3>		vPickList;
			//vPickList.SetArrayCount(mesh->GetNumVertices());
			//float3* destVerts = vPickList.GetPtr();
			////
			char* pVertex	=	(char*)mesh->GetVertexPtr();//m_vertexs;
			CDynamicArray<VertexTemp>		vPickList;
			vPickList.SetArrayCount(mesh->GetNumVertices());
			//
			if(sizeof(VertexTempBlend)==mesh->GetElementWidth()){
				float3 v0,v1,v2,v3;
				for(int ivert = 0;ivert<mesh->GetNumVertices();ivert++)
				{
					float3 dest,nor;
					dest.setxyz(0,0,0);
					nor.setxyz(0,0,0);
					VertexTempBlend* pBlend	=	mesh->GetVertex<VertexTempBlend>(ivert);
					for(int iB=0;iB<4;iB++)
					{
						if(pBlend->boneID[iB]<m_geometryData->m_polygons->m_boneCount)
						{
							float3 v1 = pBlend->pos;
							float3 vn = pBlend->normal;
							v1.transform( v1, matList[ pBlend->boneID[iB] ] );
							v1 = v1 * pBlend->weight[iB];
							dest += v1;
							vn.transformNoMove( vn, matList[ pBlend->boneID[iB] ] );
							vn = vn * pBlend->weight[iB];
							nor += vn;
						}
					}
					VertexTemp& temp = vPickList[ivert];
					temp.pos = dest;
					temp.normal = nor;
					temp.uv1 = pBlend->uv1;
				}
			}
			else if(sizeof(VertexTempBlendNew)==mesh->GetElementWidth()){
				float3 v0,v1,v2,v3;
				for(int ivert = 0;ivert<mesh->GetNumVertices();ivert++)
				{
					float3 dest,nor;
					dest.setxyz(0,0,0);
					nor.setxyz(0,0,0);
					VertexTempBlendNew* pBlend	=	mesh->GetVertex<VertexTempBlendNew>(ivert);
					for(int iB=0;iB<4;iB++)
					{
						if(pBlend->boneID[iB]<m_geometryData->m_polygons->m_boneCount)
						{
							float3 v1 = pBlend->pos;
							float3 vn = pBlend->normal;
							v1.transform( v1, matList[ pBlend->boneID[iB] ] );
							v1 = v1 * pBlend->weight[iB];
							dest += v1;
							vn.transformNoMove( vn, matList[ pBlend->boneID[iB] ] );
							vn = vn * pBlend->weight[iB];
							nor += vn;
						}
					}
					VertexTemp& temp = vPickList[ivert];
					temp.pos = dest;
					temp.normal = nor;
					temp.uv1 = pBlend->uv1;
				}
			}
			float fBaseScale = 0.5f;
			AABox box;
			box._min.setxyz(100000,100000,100000);
			box._max.setxyz(-100000,-100000,-100000);
			for(int i=0;i<vPickList.size();i++){
				//vPickList[i].pos *= fBaseScale;
				box._min.minv(box._min, vPickList[i].pos);
				box._max.maxv(box._max, vPickList[i].pos);
			}
			for(int i=0;i<m_geometryData->m_numMaterials;i++){
				m_geometryData->m_materials[i].cullMode=Phantom::CullMode_CW;
				m_geometryData->m_materials[i].alphaMode = Phantom::AlphaMode_None;
			}
			m_geometryData->m_polygons->m_objectRegion.aabox = box;
			m_geometryData->m_polygons->m_aaboxVertex.setup(m_geometryData->m_polygons->m_objectRegion.aabox._min, m_geometryData->m_polygons->m_objectRegion.aabox._max);
			float3 vM = (m_geometryData->m_polygons->m_objectRegion.aabox._max + m_geometryData->m_polygons->m_objectRegion.aabox._min)*0.5f;
			float fS = (m_geometryData->m_polygons->m_objectRegion.aabox._max - vM).R();
			m_geometryData->m_polygons->m_objectRegion.sphere.setup(vM, fS);
			//
			mesh->SetVertexFVF(Phantom::FVF_XYZ|Phantom::FVF_NORMAL|Phantom::FVF_TEX1);
			mesh->UpdateVertexBuffer(vPickList.GetPtr(), vPickList.size());
			assert(mesh->GetElementWidth()==sizeof(VertexTemp));
			if(m_geometryData->m_polygons->m_boneMap)
				delete[] m_geometryData->m_polygons->m_boneMap;
			m_geometryData->m_polygons->m_lodMesh.DestroyLODMesh();
			m_geometryData->m_polygons->m_boneMap = 0;
			m_geometryData->m_polygons->m_boneCount = 0;
			m_geometryData->m_polygons->m_bBlendVertex = false;
		}
	}
	char	SubMeshData::pickGeometry(const PObjectRegion* region, BoneMatrix* boneMatrix, matrix4x4* vWorld, IntersectData* p, void* pickData, char bDoubleSide)
	{CPUTimeT(SubMeshData);
		if(!m_polygons->m_pRenderMesh)
			return false;
		if(region)
		{
			if(!ObjectInLinear(p, *region, vWorld))
				return false;
		}
		//INT nWidth		=	GetVertexWidth();
		//assert(nWidth == m_pRenderMesh->GetStrideSize());
		if(!boneMatrix || !boneMatrix->boneMatrix)
		{
			PIntersectData d;
			float3 vPick;
			float u,v;
			d.Pick	= &vPick;
			d.u		= &u;
			d.v		= &v;
			float f = p->GetPickInfo().fDist;
			d.t = &f;
			float3 vNormal;
			d.normal		=	&vNormal;
			//
			int cull = Phantom::CullMode_CCW;
			if(this->m_materials)
				cull = m_materials[0].cullMode;
			//if(Phantom::IntersectTriangleList(&d, p->GetOriginal(), p->GetDir(), this->m_vertexs, m_trianglePtr, this->m_faceCount, vWorld, GetVertexWidth(), bDoubleSide))
			if(Phantom::IntersectTriangleList(&d, p->GetOriginal(), p->GetDir(), m_polygons->m_pRenderMesh->GetVertexPtr(), m_polygons->m_pRenderMesh->GetIndices(), m_polygons->m_pRenderMesh->GetNumFaces(), vWorld, m_polygons->m_pRenderMesh->GetStrideSize(), cull))
			{
				p->m_info.fDist	=	f;
				p->m_info.fU	=	u;
				p->m_info.fV	=	v;
				p->m_info.vFaceNormal	=	vNormal;
				p->m_info.vPick	=	vPick;
				p->m_info.name	=	this->m_name.t;
				return true;
			}
		}
		else// if(m_bBlendVertex)
		{
			static CDynamicArray<float3>		vPickList;
			vPickList.SetArrayCount(m_polygons->m_pRenderMesh->GetNumVertices());
			float3* destVerts = vPickList.GetPtr();
			//
			matrix4x4*	matList =	boneMatrix->boneMatrix;
			DeviceMesh* mesh	=	m_polygons->m_pRenderMesh;
			if(sizeof(VertexTempBlend)==mesh->GetElementWidth()){
				for(int ivert = 0;ivert<mesh->GetNumVertices();ivert++)
				{
					float3& dest = destVerts[ivert];
					dest.setxyz(0,0,0);
					VertexTempBlend* pBlend	=	mesh->GetVertex<VertexTempBlend>(ivert);
					for(int iB=0;iB<4;iB++)
					{
						if(pBlend->boneID[iB]<m_polygons->m_boneCount)
						{
							float3 v1 = pBlend->pos;
							v1.transform( v1, matList[ pBlend->boneID[iB] ] );
							v1 = v1 * pBlend->weight[iB];
							dest += v1;
						}
					}
				}
			}
			else if(sizeof(VertexTempBlendNew)==mesh->GetElementWidth()){
				//	return temp;
				for(int ivert = 0;ivert<mesh->GetNumVertices();ivert++)
				{
					float3& dest = destVerts[ivert];
					dest.setxyz(0,0,0);
					VertexTempBlendNew* pBlend	=	mesh->GetVertex<VertexTempBlendNew>(ivert);
					for(int iB=0;iB<4;iB++)
					{
						if(pBlend->boneID[iB]<m_polygons->m_boneCount)
						{
							float3 v1 = pBlend->pos;
							v1.transform( v1, matList[ pBlend->boneID[iB] ] );
							v1 = v1 * pBlend->weight[iB];
							dest += v1;
						}
					}
				}
			}
			else
				return false;
			PIntersectData d;
			float3 vPick;
			float u,v;
			d.Pick	= &vPick;
			d.u		= &u;
			d.v		= &v;
			float f = p->GetPickInfo().fDist;
			d.t = &f;
			float3 vNormal;
			d.normal		=	&vNormal;
			int cull = Phantom::CullMode_CCW;
			if(this->m_materials)
				cull = m_materials[0].cullMode;
			if(Phantom::IntersectTriangleList(&d, p->GetOriginal(), p->GetDir(), destVerts, m_polygons->m_pRenderMesh->GetIndices(), m_polygons->m_pRenderMesh->GetNumFaces(), vWorld, sizeof(float3), cull))
			{
				p->m_info.fDist	=	f;
				p->m_info.fU	=	u;
				p->m_info.fV	=	v;
				p->m_info.vFaceNormal	=	vNormal;
				p->m_info.vPick	=	vPick;
				p->m_info.name	=	this->m_name.t;
				return true;
			}
		}
		return false;
	}
char	SubMeshData::pickBox(const PObjectRegion* region, BoneMatrix* boneMatrix, matrix4x4* vWorld, IntersectData* p, void* pickData, char bDoubleSide)
	{CPUTimeT(SubMeshData);
		if(!m_polygons->m_pRenderMesh)
			return false;
		if(region)
		{
			if(!ObjectInLinear(p, *region, vWorld))
				return false;
		}
		return Phantom::IntersectBox(p, m_polygons->m_objectRegion.aabox._min,m_polygons->m_objectRegion.aabox._max, vWorld);
	}

	unsigned int		SubMesh::GetMaterialCount()
	{CPUTimeNone();
		return (unsigned int)m_geometryData->m_numMaterials;
	}
	VOID		SubMesh::ResetData(SubMeshData* data)
	{CPUTimeNone();
		m_bVisible				=	data->m_bVisible;
		m_geometryData			=	data;
		m_bHasNormalMap			=	false;
		m_isAlphaBlend			=	false;
		this->m_boneMatrix.localBoneIdx	=	-1;
		m_fVertexAnimationTime	=	0;
	}

	SubMesh::SubMesh()
	{
		m_bExistMeshAnim		=	0;
		m_geometryData			=	0;
		m_nMtrlFrameID			=	0;
		m_groupPtr				=	0;
		m_diffusePtr			=	0;
		m_emissivePtr			=	0;
		m_customTexture			=	0;
		m_fAlpha				=	1;
		m_bShowOnAction			=	0;
		m_bVisible				=	1;
	}
	void	SubMesh::Create(SubMeshData* data, Mesh* groupPtr)
	{CPUTimeNone();
		m_bExistMeshAnim	=	data->m_bExistMeshAnim;
		m_geometryData			=	0;
		m_nMtrlFrameID			=	0;
		m_groupPtr				=	groupPtr;
		m_diffusePtr			=	0;
		m_emissivePtr			=	0;
		m_customTexture			=	0;
		m_fAlpha				=	1;
		ResetData(data);
	}
	SubMesh::~SubMesh(void)
	{
		if(m_customTexture)
		{
			for(int i=0;i<this->m_geometryData->m_numMaterials;i++)
				safe_release(m_customTexture[i]);
			delete[] m_customTexture;
		}
	}

	void		SubMesh::drawToShadowMap(const matrix4x4* matPtr, int nLodLevel)//渲染到阴影贴图模式，如果存在alphatest物体则需要单独设置alphaTestEnable值
	{CPUTimeT(SubMesh);
		if(!m_geometryData->m_bToShadowMap)
			return;
		if(nLodLevel<0)
			nLodLevel = m_groupPtr->m_nOnceLodLevel;
		Program* vsps = 0;
		if(!m_geometryData || !IsVisibled())return;
		if(m_geometryData->m_bUseWorld)
			if(!GetCameraPtr()->IsSphereVisible(m_sphere.center, m_sphere.radius))
				return;
		if(!m_geometryData->m_bZWriteEnable)return;
		g_manager->SetAlphaMode(AlphaMode_None);
		int alphaMode	=	-1;
		int cullMode	=	-1;
		int last_alphaMode	=	-1;
		for(int im = 0; im < this->m_geometryData->m_numMaterials; im++)
		{
			int materialIndex	=	im;
			if(m_geometryData->m_renderMaterialIndex)
				materialIndex	=	m_geometryData->m_renderMaterialIndex[im];
			MeshMaterial& material	=	this->m_geometryData->m_materials[materialIndex];
			if(!material.visible)
				continue;
			if( material.cullMode != cullMode ){
				g_manager->SetCullMode((CullMode)material.cullMode);
				cullMode	=	material.cullMode;
			}
			if(alphaMode != material.alphaMode){
				alphaMode	=	material.alphaMode;
			}
			if(alphaMode==Phantom::AlphaMode_Blend)
				continue;
			int srcblend = material.srcBlend;
			int destblend = material.destBlend;
			if(alphaMode != last_alphaMode)
			{
				last_alphaMode = alphaMode;
				if(alphaMode<0||alphaMode>=AlphaMode_Count)
					continue;
				else
				{
                    int nProgramType = Program::Type_ToShadowMap|Program::Type_UV|Program::Type_Normal;
					if(alphaMode==Phantom::AlphaMode_Test)
						nProgramType |= Program::Type_AlphaTest;
					vsps = drawSetShader(matPtr, material.programToSMID, nProgramType);
					vsps->CommitToShadowMap();
				}
			}
			if(vsps)
			{
				BOOL bRender = true;
                if(alphaMode==Phantom::AlphaMode_Test){
					int diffuseIndex = m_groupPtr->m_diffuseIndex;
					if(m_customTexture&&m_customTexture[im])
						vsps->SetTexture((TextureIndex)0, m_customTexture[im]);
					else
						vsps->SetTexture((TextureIndex)0, this->m_geometryData->GetDiffuseTexture(materialIndex, diffuseIndex, m_groupPtr->m_fMeshIndex, bRender));
                }
				if(bRender)
					m_geometryData->renderLODData(vsps, materialIndex, nLodLevel);
			}
		}
	}
	//
	//LPCSTR		SubMesh::GetActiveShaderBy()
	//{
	//	if(!m_geometryData)
	//		return "";
	//	return m_geometryData->m_defaultShader[m_activeShader].m_name;
	//}
	//
	//VOID		SubMesh::SetActiveShaderBy(LPCSTR szName)
	//{
	//	if(!m_geometryData)
	//		return;
	//	for(int i=0;i<MeshShaderType_Count;i++)
	//	{
	//		if(m_geometryData->m_defaultShader[i].m_name == szName)
	//		{
	//			this->m_activeShader = (MeshShaderType)i;
	//			break;
	//		}
	//	}
	//}

	void		SubMesh::ReplaceImage(const char* szImageSrc, const char* szImageReplace)
	{CPUTimeT(SubMesh);
		if(!m_customTexture)
		{
			m_customTexture = new Texture*[this->m_geometryData->m_numMaterials];
			memset(m_customTexture, 0, sizeof(Texture*)*this->m_geometryData->m_numMaterials);
		}
		for(int i=0;i<this->m_geometryData->m_numMaterials;i++)
		{
			if(m_geometryData->m_materials[i].texture[Phantom::TEX_DIFFUSE] == szImageSrc){
				safe_release(m_customTexture[i]);
				g_manager.LoadTexture(&m_customTexture[i], szImageReplace, true);
			}
		}
	}
	void		SubMesh::SetAlphaValue(float fAlphaValue)
	{CPUTimeNone();
		m_fAlpha = fAlphaValue;
	}

	void		SubMesh::drawObject(const matrix4x4* matPtr)
	{CPUTimeNone();
		drawObject(matPtr, m_groupPtr->m_nOnceLodLevel);
	}
	MeshMaterial*	SubMesh::GetMaterialPtr(unsigned int nIndex)
	{CPUTimeNone();
		return this->m_geometryData->GetMaterialPtr(nIndex);
	}
	void		SubMesh::recalcObjectRegion()
	{CPUTimeT(SubMesh);
		if(this->m_geometryData->m_bUseWorld)
		{
			float3 _min(100000000.0f, 100000000.0f, 100000000.0f);
			float3 _max(-100000000.0f, -100000000.0f, -100000000.0f);
			for(int i=0;i<8;i++)
			{
				float3 v;
				m_geometryData->m_polygons->m_aaboxVertex.v[i].transform(v, this->m_worldLocal);
				_max.maxv(_max, v);
				_min.minv(_min, v);
			}
			//
			float3 vM = (_max + _min)*0.5f;
			float fS = (_max - vM).R();
			m_sphere.setup(vM, fS );
		}
	}
	Program*	SubMesh::drawSetShader(const matrix4x4* matPtr, int& programID, int nProgramType)
	{CPUTimeT(SubMesh);
		Program* vsps = 0;
		if(programID > 0)
			vsps = g_manager.SetRenderMode(programID, m_boneMatrix.boneMatrix != 0);
		if(vsps && nProgramType != vsps->GetProgramType())
			programID = 0;
		if(programID == 0){
			programID = g_manager.CreateProgram(nProgramType, m_boneMatrix.boneMatrix != 0);
			//LogInfo("CreateProgOK(%08X)%d", nProgramType, programID);
			if(programID > 0)
				vsps = g_manager.SetRenderMode(programID, m_boneMatrix.boneMatrix != 0);
			else
				return 0;
		}
		else if(programID < 0)
			return 0;
		//if(!vsps)
		//{
		//	_shader_var temp;
		//	vsps = g_manager.SetRenderMode(temp, m_boneMatrix.boneMatrix != 0, "tech_default");
		//}
		if(!vsps)
			return 0;
		if(m_boneMatrix.boneMatrix)
			vsps->SetBoneMatrixs(m_boneMatrix.boneMatrix, m_geometryData->m_polygons->m_boneCount);
		if(matPtr)
			vsps->SetWorldMatrix(matPtr, m_bHasNormalMap);
		else if(this->m_boneMatrix.localBoneIdx >= 0 || this->m_geometryData->m_bUseWorld)// && !m_geometryData->m_animationMesh)
			vsps->SetWorldMatrix(&m_worldLocal, m_bHasNormalMap);
		else
			vsps->SetWorldMatrix(&m_groupPtr->m_world, m_bHasNormalMap);
		return vsps;
	}
#pragma pack(push,1)
	struct GeometryVertex//模型顶点数据类型
	{
		float3							pos;
		float3							normal;
		unsigned int					color;
		float2							tex0;
	};
#pragma pack(pop)
	void		SubMesh::drawObject(const matrix4x4* matPtr, int lodLevel)
	{CPUTimeT(SubMesh);
		Program* vsps = 0;
		if(!m_geometryData || !IsVisibled())return;
		if(m_geometryData->m_bUseWorld)
			if(!GetCameraPtr()->IsSphereVisible(m_sphere.center, m_sphere.radius))
				return;
		BOOL bChangeA = 0;
		BOOL bChangeB = 0;
		int alphaMode	=	-1;
		int cullMode	=	-1;
		int last_alphaMode	=	-1;
		char bAlphaSet	=	true;
		color4 diffuse,emissive;
		if((int)(m_fAlpha*1000)<1000)
		{
			CPUTimeN(SubMesh,1);
			bAlphaSet=false;
			g_manager->SetAlphaMode(AlphaMode_Blend);
			alphaMode = AlphaMode_Blend;
			g_manager->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_INVSRCALPHA);
		}
		BOOL bZWrite = m_geometryData->m_bZWriteEnable;
		BOOL bLastZWrite = bZWrite;
		g_manager->SetZWriteEnable(bZWrite);
		LightAffectData* light = m_groupPtr->m_lightPtr;
		int nProgramType = Program::Type_Material|Program::Type_Normal;
		if(m_groupPtr->m_bUseSystemProgram)
			nProgramType|=g_manager.GetBaseProgramType();
		if(this->m_geometryData->m_bExistShadowMap)
			nProgramType |= Program::Type_StaticShadowMap;
		if(nProgramType&Program::Type_WithShadowMap&&(!g_manager.IsShadowEnable(Phantom::ShadowEnable_MeshWithMap)||!m_geometryData->m_bEnableShadow))
			nProgramType^=Program::Type_WithShadowMap;
		else if(m_geometryData->m_bEnableShadow){
			nProgramType|=Program::Type_WithShadowMap;
		}
		int srcblend = 0;
		int destblend = 0;
		for(int im = 0; im < this->m_geometryData->m_numMaterials; im++)
		{
			CPUTimeN(SubMesh,2);
			int materialIndex	=	im;
			if(m_geometryData->m_renderMaterialIndex)
				materialIndex	=	m_geometryData->m_renderMaterialIndex[im];
			MeshMaterial& material	=	this->m_geometryData->m_materials[materialIndex];
			if(!material.visible)
				continue;
			if( material.cullMode != cullMode ){
				g_manager->SetCullMode((CullMode)material.cullMode);
				cullMode	=	material.cullMode;
			}
			if(bAlphaSet){
				CPUTimeN(SubMesh,3);
				if(alphaMode != material.alphaMode){
					alphaMode	=	material.alphaMode;
					if(alphaMode == AlphaMode_Blend)
						g_manager->SetAlphaMode(AlphaMode_Blend);
					else if(alphaMode == AlphaMode_Test)
						g_manager->SetAlphaMode(AlphaMode_Test);
					else
						g_manager->SetAlphaMode(AlphaMode_None);
				}
			}
			if(alphaMode == AlphaMode_Blend)
			{
				if(srcblend != material.srcBlend || destblend != material.destBlend)
				{
					srcblend = material.srcBlend;
					destblend = material.destBlend;
					g_manager->SetBlendMode((BlendMode)srcblend, (BlendMode)destblend);
				}
			}
			if(alphaMode != last_alphaMode)
			{
				CPUTimeN(SubMesh,4);
				last_alphaMode = alphaMode;
				if(alphaMode<0||alphaMode>=AlphaMode_Count)
				{
				}
				else
				{
					int nProgram = nProgramType;
					if(material.texture[0].t[0] > 0)
						nProgram |= Program::Type_UV;
					else
						nProgram |= Program::Type_Material;
					if(alphaMode==Phantom::AlphaMode_Test)
						nProgram |= Program::Type_AlphaTest;
					if(alphaMode != AlphaMode_Blend && m_groupPtr->m_bUseSystemProgram && light)
						nProgram |= Program::Type_Light;
					char bLight = (nProgram&Program::Type_Light)>0;
					vsps = drawSetShader(matPtr, material.programID[bLight], nProgram);
					if(vsps&&(nProgram&Program::Type_Light)){
						vsps->SetLight(light->count, light->lightPos, light->lightColor, light->rangeAngle);
					}
					if(nProgram&Program::Type_WithShadowMap)
						vsps->CommitShadowMap();
				}
			}
			if(vsps)
			{
				CPUTimeN(SubMesh,5);
				MaterialAnim& anim = this->m_geometryData->m_materials[materialIndex].anims;
				if(anim.bHasAnim && anim.keys.size() > 0)
				{
					CPUTimeN(SubMesh,6);
					int nTileFrameId = 0;
					int bTiling = false;
					int nUTiles = 1;
					int nVTiles = 1;
					float fu = 0;
					float fv = 0;
					if(m_nMtrlFrameID >= anim.keys.size())
						m_nMtrlFrameID = (int)anim.keys.size() - 1;
					MaterialColorKey& ck = anim.keys[m_nMtrlFrameID];
					diffuse = ck.c;
					vsps->SetEmissiveColor( color4(ck.selfillum, ck.selfillum, ck.selfillum, ck.selfillum) );
					if(ck.selfillum >= 0.99f)
					{
						destblend = Phantom::BlendMode_ONE;
						g_manager->SetBlendMode((BlendMode)srcblend, (BlendMode)destblend);
					}
					//
					nTileFrameId = (int)((m_nMtrlFrameID*33+anim.nStartFrame)/anim.fPlaybackRate);
					nTileFrameId %= anim.nUTile*anim.nVTile;
					bTiling = anim.bTiling;
					nUTiles = anim.nUTile;
					nVTiles = anim.nVTile;
					fu = ck.uoffset;
					fv = ck.voffset;
					if(bTiling && (nUTiles > 1 || nVTiles > 1))
					{
						//assert(!(bTiling && (nUTiles > 1 || nVTiles > 1)));
						int row = nTileFrameId/nUTiles;
						int col = nTileFrameId%nUTiles;
						float du = 1.0f/(float)nUTiles;
						float dv = 1.0f/(float)nVTiles;
						DeviceMesh* devmesh = m_geometryData->m_polygons->m_pRenderMesh;
						int nBoardCount = devmesh->GetNumVertices()/4;
						for( int i = 0; i < nBoardCount; i++ )
						{
							GeometryVertex* v = devmesh->GetVertex<GeometryVertex>(i*4+0);
							v->tex0.x = du * (float)col;
							v->tex0.y = dv * (float)row + dv;
							v = devmesh->GetVertex<GeometryVertex>(i*4+1);
							v->tex0.x = du * (float)col;
							v->tex0.y = dv * (float)row;
							v = devmesh->GetVertex<GeometryVertex>(i*4+2);
							v->tex0.x = du * (float)col + du;
							v->tex0.y = dv * (float)row + dv;
							v = devmesh->GetVertex<GeometryVertex>(i*4+3);
							v->tex0.x = du * (float)col + du;
							v->tex0.y = dv * (float)row;
						}
						devmesh->UpdateVertexBuffer(0, 0);
					}
				}
				else{
					const color4* mtrldiffuse = material.GetDiffuseColor(m_groupPtr->m_diffuseIndex);
					if(m_diffusePtr)
						diffuse = (*mtrldiffuse)*(*m_diffusePtr);
					else
						diffuse = (*mtrldiffuse);
					if(m_emissivePtr)
						vsps->SetEmissiveColor( material.emissive*(*m_emissivePtr) );// + g_manager->m_ambientColor;
					else
						vsps->SetEmissiveColor( material.emissive );
				}
				{
					CPUTimeN(SubMesh,7);
					bZWrite = m_geometryData->m_bZWriteEnable;
					//
					BOOL bRender = true;
					diffuse.a *= m_fAlpha;
					vsps->SetDiffuse( diffuse );
					if(m_customTexture&&m_customTexture[im])
						vsps->SetTexture((TextureIndex)0, m_customTexture[im]);
					else
						vsps->SetTexture((TextureIndex)0, this->m_geometryData->GetDiffuseTexture(materialIndex, m_groupPtr->m_diffuseIndex, m_groupPtr->m_fMeshIndex, bRender));
					if(bRender){
						if(material.texturePtr[Phantom::TEX_SHADOWMAP])
						{
							vsps->SetTexture(TextureIndex_3, material.texturePtr[Phantom::TEX_SHADOWMAP]);
							vsps->SetFloat2(material.shadowTextureSize, float2(1.0f / (float)material.texturePtr[Phantom::TEX_SHADOWMAP]->GetWidth(), 1.0f / (float)material.texturePtr[Phantom::TEX_SHADOWMAP]->GetHeight()), "shadowTextureSize");
						}
						if(bLastZWrite!=bZWrite)
							g_manager->SetZWriteEnable(bLastZWrite = bZWrite);
						if(material.cullMode==Phantom::CullMode_CW||material.cullMode==Phantom::CullMode_Double)
							vsps->SetLightDirInv(false);
						else
							vsps->SetLightDirInv(true);
						m_geometryData->renderLODData(vsps, materialIndex, lodLevel);
					}
				}
			}
		}
		if(!bLastZWrite)
			g_manager->SetZWriteEnable(true);
	}

	char		SubMesh::IsAffectByShadowMap()			//是否支持自身阴影渲染
	{CPUTimeNone();
		//检查是否可被渲染阴影
		if(!m_geometryData->m_groupPtr->m_bCanSelfShadow)
			return false;
		return true;
	}

	AABox		SubMesh::GetAABox()
	{CPUTimeT(SubMesh);
		matrix4x4* matin = &this->m_groupPtr->m_world;
		if(m_geometryData->m_bUseWorld)
			matin				=	&m_worldLocal;
		else if(m_boneMatrix.localBoneIdx >= 0)
			matin				=	&m_worldLocal;
		AABox temp;
		temp._min.setxyz(100000000.0f, 100000000.0f, 100000000.0f);
		temp._max.setxyz(-100000000.0f, -100000000.0f, -100000000.0f);
		for(int i=0;i<8;i++)
		{
			float3 v;
			PBoxVertices& boxv = this->m_geometryData->m_polygons->m_aaboxVertex;
			boxv.v[i].transform(v, *matin);
			temp._max.maxv(temp._max, v);
			temp._min.minv(temp._min, v);
		}
		return temp;
	}
	char		SubMesh::pickGeometry(IntersectData* p, matrix4x4* matWorld, void* pickParam)
	{CPUTimeNone();
		return m_geometryData->pickGeometry(0, &m_boneMatrix, matWorld, p, pickParam, false);
	}
	char		SubMesh::pickBox(IntersectData* p, matrix4x4* matWorld, void* pickParam)
	{CPUTimeNone();
		return m_geometryData->pickBox(0, &m_boneMatrix, matWorld, p, pickParam, true);
	}
	VOID				SubMesh::SetWorldMatrix(const matrix4x4& mat)
	{CPUTimeNone();
	}
	const matrix4x4&	SubMesh::GetWorldMatrix()
	{CPUTimeNone();
		if(m_boneMatrix.localBoneIdx >= 0 && m_bExistMeshAnim)
		{
			const MeshBoneKey* key	=	m_groupPtr->m_actionDo.getBoneKey(m_boneMatrix.localBoneIdx);
			if(key)
			{
				matrixMultiply(&m_worldLocal, &key->m_matKey, &m_groupPtr->m_world);
				return m_worldLocal;
			}
		}
		return m_groupPtr->m_world;
	}

};
