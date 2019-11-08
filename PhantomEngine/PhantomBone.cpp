//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomBone.h"
#include "PhantomCamera.h"
#include "PhantomMesh.h"
#include "PhantomManager.h"
#ifdef WIN32_OS
#include "io.h"
#endif
namespace Phantom{
	MeshMaterialAttribute::MeshMaterialAttribute()
	{
		memset(texture, 0, sizeof(texture));
		cullMode	=	Phantom::CullMode_CCW;//渲染A面还是B面还是都渲染,选择Phantom::CullMode_Double, Phantom::CullMode_CW, Phantom::CullMode_CCW
		alphaMode	=	Phantom::AlphaMode_None;//设置透明通道使用类型AlphaMode_Blend,AlphaMode_AlphaDisable, ALPHAMODE_ALPHATEST
		//
		diffuse.setRGBA(1,1,1,1);//设置慢射光
		ambient.setRGBA(1,1,1,1);
		specular.setRGBA(1,1,1,1);
		emissive.setRGBA(0,0,0,0);
		visible		=	true;
		srcBlend = Phantom::BlendMode_SRCALPHA;
		destBlend = Phantom::BlendMode_INVSRCALPHA;
		visibleInShadowMap = true;/*默认显示*/
		lodEnable = true;
	}
	MeshBoneKey::MeshBoneKey()
	{
		m_matKey.identity();
		m_boneKey			= 0;
		//m_keyBeginIndex		= -1;	//开始针
		//m_keyEndIndex		= -1;		//结束针
		m_lastBoneKey		= 0;
		m_keyBlend			= 0;
		m_boneIndex			= -1;
		m_keyCurrent		= 0;
		m_lastEndIndex		= 0;
		m_blendKeyIndex		= 0;
	}

	MeshBoneKey::~MeshBoneKey()
	{
	}
	void	lerp(matrix4x4& mat, matrix4x4& from, matrix3x4& next, float fLerp)
	{
		int index = 0;
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<3;j++)
			{
				mat.m[i][j]	=	(next.v[index] - from.f[index]) * fLerp + from.f[index];
				index++;
			}
		}
	}
	//char	MeshBone::SetKeyFrame(matrix4x4& ret, color4& diffuse, float fLerp, int beginIndex, int endIndex, MeshBoneKey* keyPtr)
	//{CPUTime(MeshBone);
	//	matrix4x4* pBeginPtr		=	m_matrixKey.GetKeyPtr(beginIndex);
	//	if(beginIndex==endIndex){
	//		memcpy(&ret, pBeginPtr, sizeof(matrix4x4));
	//		//pBeginPtr->get(ret);
	//		//ret._44 = 1;
	//		return true;
	//	}
	//	matrix4x4* pEndPtr		=	m_matrixKey.GetKeyPtr(endIndex);
	//	//pBeginPtr	=	&keyPtr->m_saveMatrix;
	//	//if(endIndex != keyPtr->m_lastEndKey)
	//	//{
	//	//	if(keyPtr->m_lastEndKey>=0)
	//	//		keyPtr->m_saveMatrix = *m_matrixKey.GetKeyPtr(keyPtr->m_lastEndKey);//->get(keyPtr->m_saveMatrix);
	//	//	else
	//	//		pBeginPtr	=	m_matrixKey.GetKeyPtr(beginIndex);
	//	//	keyPtr->m_lastEndKey	=	endIndex;
	//	//}
	//	memcpy(&ret, pBeginPtr, sizeof(matrix4x4));
	//	//pBeginPtr->get(ret);//pBeginPtr->lerp(ret, *pEndPtr, fLerp);
	//	//ret._44 = 1;
	//	return true;
	//}
	void	MeshBoneKey::BeginBlend(MeshBone* lastBone, int lastKeyIndex)
	{
		m_lastBoneKey = lastBone;
		if(m_lastBoneKey)
			m_keyBlend = 1;
		else
			m_keyBlend = 0;
		m_lastEndIndex = lastKeyIndex;
		m_blendKeyIndex = -1;
	}
	char	MeshBoneKey::SetKeyFrame(float fKeyFrame, int beginIndex, int endIndex, int nFrame)
	{CPUTime(MeshBoneKey);
		//static BOOL bBlend = true;
		//if(bBlend){
		matrix4x4* m1 = m_boneKey->m_keyPtr->GetKeyPtr(beginIndex);
		matrix4x4* m2 = m_boneKey->m_keyPtr->GetKeyPtr(endIndex);
//#ifndef EDITOR_OS
		if(m_keyBlend>0){
			if(m_keyBlend == 1)
			{
				m_keyBlend = 2;
				m_blendKeyIndex = nFrame;
			}
			if(m_blendKeyIndex!=nFrame){
				m_keyBlend = 0;
			}
			else{
				m1 = m_lastBoneKey->m_keyPtr->GetKeyPtr(m_lastEndIndex);
			}
		}
//#endif
		m_keyCurrent = endIndex;
		if(fKeyFrame<0.01f)
			m_matKey = *m1;
		else if(fKeyFrame>0.99f)
			m_matKey = *m2;
		else
			matrixLerp(&m_matKey, m1, m2, fKeyFrame);
		return true;
		//}
		//if(m_keyBeginIndex==beginIndex&&m_lastBoneKey==m_boneKey)
		//	return true;
		//m_lastBoneKey=m_boneKey;
		//m_keyBeginIndex=beginIndex;
		//m_matKey = *m_boneKey->m_matrixKey.GetKeyPtr(beginIndex);
		//return true;
	}
	MeshBone::MeshBone()
	{
		m_globalBoneID			=	0;
		m_parentIndex			=	-1;
		m_keyPtr				=	&m_keyList;
		m_keyBoneID				=	-1;
	}

	MeshBone::~MeshBone()
	{
		destroy();
	}
	void		MeshBone::destroy()
	{CPUTime(MeshBone);
		m_parentIndex			=	-1;
	}

	MeshActionRange::MeshActionRange(MeshActionData* a)
	{
		m_actionPtr		=	a;
		m_startFrame	=	0;
		m_endFrame		=	-1;
		m_fBaseSpeed	=	25;
		m_actionID		=	0;
		m_fNextLerpTime	=	0;
		m_actionType	=	0;
		for(int i=0;i<MAX_ACTION_SOUND_COUNT;i++)
			m_nSndKeyIndex[i]	=	-1;
		//m_effectPtr		=	0;
		this->m_nWaveID	=	-1;
		this->m_toHitKeyID = -1;
		this->m_fMoveOffset	=	0;
		m_bLoopMode = 0;
		m_bRegionOK		=	false;
		memset(&m_region, 0, sizeof(m_region));
	}
	MeshActionRange::~MeshActionRange()
	{
	}
	void							MeshActionRange::SetLoop(int beginIndex, int endIndex)	//设置动作循环变量
	{CPUTime(MeshActionRange);
		m_startFrame	=	beginIndex;
		m_endFrame		=	endIndex;
		if(m_startFrame >= m_actionPtr->GetKeyCount())
			m_startFrame	=	m_actionPtr->GetKeyCount() - 1;
		if(m_startFrame < -1)
			m_startFrame = 0;
		if(m_endFrame >= m_actionPtr->GetKeyCount() || m_endFrame < -1)
			m_endFrame	=	m_actionPtr->GetKeyCount() - 1;
	}
	void							MeshActionRange::SaveAction(CFileHelperW& w)
	{CPUTime(MeshActionRange);
		char version = 11;
		w << version;
		w << m_startFrame << m_endFrame << m_name << m_fBaseSpeed << m_actionID << m_fNextLerpTime;
		w << m_rabbinEnable;
		w << m_actionType;
		w << m_sndfile << m_nSndKeyIndex[0];
		//char bEffect = (m_effectPtr != 0);
		//w << bEffect;
		//if(m_effectPtr)
		//	w << *m_effectPtr;
		w << m_nSndKeyIndex[1];
		w << m_fMoveOffset << m_toHitKeyID;
		w << m_bLoopMode;
		w << m_region;
		w << m_bRegionOK;
	}
	void							MeshActionRange::LoadAction(CFileHelperR& r)
	{CPUTime(MeshActionRange);
		char version = 1;
		r >> version;
		r >> m_startFrame >> m_endFrame >> m_name >> m_fBaseSpeed >> m_actionID >> m_fNextLerpTime;
		if(version > 1)
		{
			int hitCount = 0;
			int	hitFrames[5];//最大可被攻击帧Index
			if(version < 7)
				r >> hitCount;
			r >> m_rabbinEnable;
			if(version < 7)
				r.pStream->read(hitFrames, sizeof(hitFrames));
		}
		if(version>=3)
			r >> m_actionType;
		if(version>=4)
		{
			r >> m_sndfile >> m_nSndKeyIndex[0];
			if(m_sndfile.t[0])
				m_nWaveID = g_manager.GetMusicFileIndex(m_sndfile.t);
		}
		if(version >= 5 && version < 10)
		{
			char bEffect = 0;
			r >> bEffect;
			//m_effectPtr = 0;
			if(bEffect){
				ActionEffect temp;// ActionEffect();
				r >> temp;
			}
		}
		if(version >= 6)
			r >> m_nSndKeyIndex[1];
		if(version >= 8)
		{
			r >> this->m_fMoveOffset >> this->m_toHitKeyID;
		}
		switch(m_actionType)
		{
		case Phantom::ActionType_Attack:
			if(m_toHitKeyID<0)
				m_toHitKeyID = (this->m_startFrame+this->m_endFrame)/2;
			break;
		}
		if(version>8)
			r >> m_bLoopMode;
		if(version>=11)
		{
			r >> m_region;
			r >> m_bRegionOK;
			m_aaboxVertex.setup(m_region.aabox._min, m_region.aabox._max);
		}
	}

	MeshActionData::MeshActionData()
		:MeshActionRange(0)
	{
		m_actionPtr				=	this;
		m_fBaseSpeed			=	25.0f;
		m_boneCount				=	0;
		m_boneMap				=	0;
		m_actionID				=	0;
		m_startFrame			=	0;
		m_endFrame				=	-1;
		m_fNextLerpTime			=	0.1f;
		m_loadVersion			=	0;
		memset_s(m_globalBoneMap, 0, sizeof(m_globalBoneMap));
		m_maxBoneIndex			=	0;
	}

	MeshActionData::~MeshActionData()
	{
		destroy();
	}

	int									MeshActionData::GetKeyCount()
	{CPUTime(MeshActionData);
		if(m_boneMap)
		{
			return m_boneMap[0].GetKeyCount();
		}
		return 0;
	}
	//

	void								MeshActionData::freeData()
	{CPUTime(MeshActionData);
		safe_deleteList(m_boneMap);
		memset_s(m_globalBoneMap, 0, sizeof(m_globalBoneMap));
	}

	char								MeshActionData::RemoveAction(int id)
	{CPUTime(MeshActionData);
		for(int i=0;i<m_actions.size();i++)
		{
			if(m_actions[i]->m_actionID == id)
			{
				delete m_actions[i];
				m_actions.erase(m_actions.begin() + i);
				return true;
			}
		}
		return false;
	}
#ifdef WIN32_OS
	struct	ActionRange
	{
		int		startFrame;
		int		endFrame;
		char	name[32];
	};

	INT			g_nMixFPS	=	30;
	VOID		SetMixFPS(INT nMixFPS)
	{
		g_nMixFPS	=	max(nMixFPS, 1);
	}
	void		LoadMixFile(std::vector<ActionRange>& rets, const char* szFile)
	{
		FILE* f	=	fopen(szFile, "rb");
		if(!f)
			return;
		int length	=	filelength(_fileno(f));
		char* ch	=	new char[length + 1];
		fread(ch, length, 1, f);
		//
		char line[1024];
		int index	=	0;
		ActionRange range;
		bool bClip	=	false;
		bool bName	=	false;
		int clipIndex	=	0;
		for(int i=0;i<length;i++)
		{
			if(ch[i] == '\r')
				continue;
			if(ch[i] == '\n')
			{
				line[index++]	=	0;
				index	=	0;
				strlwr(line);
				if(strstr(line, "clip"))
				{
					clipIndex	=	0;
					bClip	=	true;
				}
				else if(bClip)
				{
					clipIndex++;
					if(clipIndex == 5)
						range.startFrame	=	(float)atoi(line) / (float)(160.0f * 30.0f / (float)g_nMixFPS);
					else if(clipIndex == 6)
						range.endFrame		=	(float)atoi(line) / (float)(160.0f * 30.0f / (float)g_nMixFPS);
					else if(strstr(line, "z_1"))
					{
						bClip	=	false;
						int len	=	strlen(line);
						const char* begin	=	line;
						for(int k=0;k<len;k++)
						{
							if(line[k] == ' ')
								begin	=	line + k + 1;
						}
						memset(range.name, 0, sizeof(range.name));
						strncpy(range.name, begin, strlen(begin) - 3);
						rets.push_back(range);
						//printf("%d.%d[%s]\n", range.startFrame, range.endFrame, range.name);
					}
				}
			}
			else
				line[index++]	=	ch[i];
		}
		//
		ch[length]	=	0;
		fclose(f);
	}

	VOID					MeshActionData::LoadMixFile(MeshData* group, const char* szMixFile)
	{
		for(int i=0;i<m_actions.size();i++)
			delete m_actions[i];
		m_actions.clear();
		std::vector<ActionRange> rets;
		Phantom::LoadMixFile(rets, szMixFile);
		for(int i=0;i<rets.size();i++)
		{
			if(rets[i].startFrame >= this->GetKeyCount())
				continue;
			MeshActionRange* action	=	new MeshActionRange(this);
			action->m_actionID		=	group->GetNewIdentity();
			action->m_startFrame	=	rets[i].startFrame;
			action->m_endFrame		=	rets[i].endFrame;
			action->m_name			=	rets[i].name;
			this->m_actions.push_back(action);
		}
	}
#endif
	char								MeshActionData::LoadData(CSafeFileHelperR& rIn)
	{CPUTime(MeshActionData);
		destroy();
		CFileHelperR r(rIn.pStream);
		int version = 0x00000001;
		r	>>	version;
		m_loadVersion	=	version;
		r >> m_boneCount;
		LoadAction(r);
		int actionCount;
		r >> actionCount;
		for(int i=0;i<actionCount;i++)
		{
			MeshActionRange* action = new MeshActionRange(this);
			m_actions.push_back(action);
			action->LoadAction(r);
		}
		if(m_loadVersion<0x00000007){
			PFileSeek temp;
			r.pStream->read(&temp, sizeof(PFileSeek));
		}
		//
		memset_s(m_globalBoneMap, 0, sizeof(m_globalBoneMap));
		m_boneMap						=	new MeshBone[ m_boneCount ];
		m_maxBoneIndex	=	0;
		for(int i=0;i<m_boneCount;i++)
		{
			MeshBone* bone		=	&m_boneMap[i];
			if(version<0x00000009){
				int numKey = 0;
				r	>> numKey;
			}
			r	>> bone->m_parentIndex;
			r	>> bone->m_globalBoneID;
			if(version>=0x00000016){
				r >> bone->m_keyBoneID;
				if(bone->m_keyBoneID<0)
					bone->m_keyList.LoadData(r, m_loadVersion);
			}
			else{
				bone->m_keyList.LoadData(r, m_loadVersion);
			}
			if(m_boneMap[i].m_globalBoneID < 0)
				m_boneMap[i].m_globalBoneID = 0;
			if(m_boneMap[i].m_globalBoneID > MAX_BONE_COUNT)
				m_boneMap[i].m_globalBoneID = MAX_BONE_COUNT - 1;
			m_globalBoneMap[bone->m_globalBoneID]	=	bone;
			m_maxBoneIndex = getmax(m_maxBoneIndex, bone->m_globalBoneID);
		}
		for(int i=0;i<m_boneCount;i++){
			MeshBone* bone		=	&m_boneMap[i];
			if(bone->m_keyBoneID>=0){
				for(int j=0;j<m_boneCount;j++){
					MeshBone* bone2		=	&m_boneMap[j];
					if(bone2->m_globalBoneID==bone->m_keyBoneID){
						bone->m_keyPtr = &bone2->m_keyList;
						break;
					}
				}
			}
		}
		m_maxBoneIndex++;
		m_maxBoneIndex = getmax(m_maxBoneIndex, m_boneCount);
		if(version>=0x00000010)
		{
			short keyCount = 0;
			r >> keyCount;
			if(keyCount>0){
				m_keyBoxs.resize(keyCount);
				r.pStream->read(m_keyBoxs.GetPtr(), sizeof(AABox)*keyCount);
			}
		}
		if(version>=0x00000013){
			short moves = 0;
			r >> moves;
			if(moves>0)
			{
				m_moves.resize(moves);
				r.pStream->read(m_moves.GetPtr(), sizeof(float3)*moves);
			}
		}
		if(version>=0x00000014)
			r >> m_bMoveX >> m_bMoveY >> m_bMoveZ;
		return true;
	}
	char	MeshActionData::SaveData(CSafeFileHelperW& wIn)
	{CPUTime(MeshActionData);
		CFileHelperW w(wIn.pStream);
		int version = 0x00000016;
		w	<<	version <<	m_boneCount;
		SaveAction(w);
		int actionCount		=	m_actions.size();
		w << actionCount;
		for(int i=0;i<actionCount;i++)
			m_actions[i]->SaveAction(w);
		//
		for(int i=0;i<m_boneCount;i++)
		{
			MeshBone* bone		=	&m_boneMap[i];
			//
			w	<< bone->m_parentIndex;
			w	<<	bone->m_globalBoneID;
			w << bone->m_keyBoneID;
			if(bone->m_keyBoneID<0)
				bone->m_keyList.SaveData(w);
		}
		short keyCount = m_keyBoxs.size();
		w << keyCount;
		if(keyCount>0)
			w.pStream->write(m_keyBoxs.GetPtr(), sizeof(AABox)*keyCount);
		short moves = m_moves.size();
		w << moves;
		if(moves>0)
			w.pStream->write(m_moves.GetPtr(), sizeof(float3)*moves);
		w << m_bMoveX << m_bMoveY << m_bMoveZ;
		return true;
	}
	void	MeshActionData::destroy()
	{CPUTime(MeshActionData);
		freeData();
		for(int i=0;i<m_actions.size();i++)
			delete m_actions[i];
		m_actions.clear();
		if(m_boneMap)
			delete[]			m_boneMap;
		m_boneMap				=	0;
		m_boneCount				=	0;
		memset_s(m_globalBoneMap, 0, sizeof(m_globalBoneMap));
	}
	void	MeshActionData::initBoneName(MeshData* data)
	{CPUTime(MeshActionData);
		if(!m_boneMap)return;
	}
	float	MeshActionRange::GetTimeLength()
	{CPUTime(MeshActionRange);
		if(this->m_fBaseSpeed>0)
			return (float)(m_endFrame-m_startFrame)/m_fBaseSpeed;
		return 1.0f;
	}
	MeshAction::MeshAction()
	{
		m_boneGroup				= 0;
		m_groupPtr				= 0;
		m_nPlayWave				=	0;
		m_bHitKey				=	false;
		m_boneKeys.setGrow(0);
		memset(&m_data, 0, sizeof(m_data));
		for(int i=0;i<MAX_ACTION_SOUND_COUNT;i++)
			m_bPlaySnd[i]	=	false;
	}
	MeshAction::~MeshAction()
	{
		m_boneKeys.clear();
	}
	void	MeshAction::initializeAction(int globalBoneCount)
	{CPUTime(MeshAction);
	}
	float	MeshAction::GetTimeLength()
	{CPUTime(MeshAction);
		if(m_boneGroup)
			return m_boneGroup->GetTimeLength();
		return 1.0f;
	}
	void	MeshAction::SetKeyFrame(float fKeyFrame, ActionInterval* ai, int aiCount)
	{CPUTime(MeshAction);
		if(!m_boneGroup)
			return;
		int nStartFrame		=	m_data.keyBegin;// m_boneGroup->m_startFrame;
		int nEndFrame		=	m_data.keyEnd;// m_boneGroup->m_endFrame;
		if(nEndFrame < 0)
			nEndFrame		=	m_data.keyEnd-m_data.keyBegin;// m_boneGroup->GetKeyCount();
		int nKeyCount		=	(nEndFrame - nStartFrame);
		if(nKeyCount < 0)
			return;
		int		beginIndex;
		int		endIndex;
		int		nFrame = (int)fKeyFrame;
		beginIndex	=	(int)fKeyFrame;
		if(m_boneGroup->m_bLoopMode||m_data.keyLoop>0)//loop mode
		{
			endIndex	=	(beginIndex + 1) % nKeyCount;
		}
		else
		{
			nKeyCount++;
			if(beginIndex>=nKeyCount)
			{
				//if(m_data.keyLoop>1||m_data.keyLoop==-1)
				//{
				//	m_data.keyLoop--;
				//	beginIndex	=	0;
				//}
				//else{
					beginIndex	=	nKeyCount-1;
				//}
				fKeyFrame	=	beginIndex;
			}
			endIndex	=	(beginIndex + 1);
			if(endIndex>=nKeyCount)
				endIndex = nKeyCount-1;
		}
		//
		float	fLerp	= fKeyFrame - (float)beginIndex;
		beginIndex	+=	nStartFrame;
		endIndex	+=	nStartFrame;
		if(m_boneGroup->m_nSndKeyIndex[0]>=0||m_boneGroup->m_nSndKeyIndex[1]>=0)//
		{
			for(int i=0;i<MAX_ACTION_SOUND_COUNT;i++)
			{
				if(m_boneGroup->m_nSndKeyIndex[i]>=beginIndex&&m_boneGroup->m_nSndKeyIndex[i]<endIndex){
					if(this->m_bPlaySnd[i]==false){
						this->m_bPlaySnd[i]=true;
						if(this->m_groupPtr->m_callback)
							this->m_groupPtr->m_callback->ActionEventRaise(this->m_groupPtr, this->m_boneGroup->m_actionType);
						if(this->m_boneGroup->m_nWaveID>=0){
							MultiMusic* m = g_manager.GetMusicFilePtrByIndex(m_boneGroup->m_nWaveID);
							if(m)
								m->Play(0);
						}
					}
				}
				else if(m_boneGroup->m_nSndKeyIndex[i] < beginIndex || endIndex<m_boneGroup->m_nSndKeyIndex[i])
				{
					m_bPlaySnd[i] = false;
				}
			}
		}
		if(m_boneGroup->m_toHitKeyID >= 0)
		{
			if(m_boneGroup->m_toHitKeyID>=beginIndex&&m_boneGroup->m_toHitKeyID<endIndex){
				if(this->m_bHitKey==false){
					this->m_bHitKey=true;
					if(this->m_groupPtr->m_callback)
						this->m_groupPtr->m_callback->ActionAttackBy(this->m_groupPtr, this->m_boneGroup->GetActionID(), this->m_groupPtr->m_nPlayType);
				}
			}
			else if(m_boneGroup->m_toHitKeyID < beginIndex || endIndex<m_boneGroup->m_toHitKeyID)
			{
				m_bHitKey = false;
			}
		}
		for(unsigned int i=0;i<this->m_boneGroup->m_actionPtr->m_boneCount;i++)
			m_boneKeys[i].ptr->SetKeyFrame(fLerp, beginIndex, endIndex, nFrame);
	}

#ifdef EDITOR_OS
	MeshActionRange*	MeshAction::GetBoneGroup()
	{
		MeshData* data = m_groupPtr->m_groupData;
		return data->GetActionPtr(m_boneGroupID);
	}
#endif
	struct LastBoneKey{
		MeshBone*		lastBoneKey;
		short			lastEndIndex;		//结束针
	};
	char	MeshAction::setGroup(MeshActionRange* pGroup, int bLoop, unsigned int keyBeginIndex, unsigned int keyEndIndex)
	{CPUTime(MeshAction);
		for(int i=0;i<MAX_ACTION_SOUND_COUNT;i++)
			this->m_bPlaySnd[i]	=	false;
		static LastBoneKey s_lastKeys[MAX_BONE_COUNT];
		memset(s_lastKeys, 0, sizeof(s_lastKeys));
		if(m_boneGroup){
			for(int i=0;i<m_boneGroup->m_actionPtr->m_boneCount;i++)
			{
				int gid = m_boneGroup->m_actionPtr->m_boneMap[i].m_globalBoneID;
				assert(gid<m_boneKeys.size());
				MeshBoneKey* ptr	=	&m_boneKeys[gid].data;
				s_lastKeys[gid].lastBoneKey = ptr->m_boneKey;
				s_lastKeys[gid].lastEndIndex = ptr->m_keyCurrent;
			}
		}
		m_boneGroup				=	pGroup;
#ifdef EDITOR_OS
		if(m_boneGroup)
			m_boneGroupID.id	=	m_boneGroup->GetActionID();
		else
			m_boneGroupID.id	=	0;
#endif
		if(m_boneGroup)
		{
			m_data.keyBegin = m_boneGroup->m_startFrame;
			m_data.keyEnd = m_boneGroup->m_endFrame;
			if(m_data.keyEnd<0)
				m_data.keyEnd = m_boneGroup->GetKeyCount()-1;
			m_data.keyLoop = bLoop;
			m_boneKeys.SetArrayCount(m_boneGroup->m_actionPtr->m_maxBoneIndex);
			for(int i=0;i<m_boneKeys.size();i++)
			{
				m_boneKeys[i].data.m_boneIndex	=	-1;
				m_boneKeys[i].data.m_boneKey = 0;
			}
			for(int i=0;i<m_boneGroup->m_actionPtr->m_boneCount;i++)
			{
				int gid = m_boneGroup->m_actionPtr->m_boneMap[i].m_globalBoneID;
				assert(gid<m_boneKeys.size());
				m_boneKeys[i].ptr	=	&m_boneKeys[gid].data;
				m_boneKeys[i].ptr->m_boneIndex	=	i;
				m_boneKeys[i].ptr->m_boneKey	=	&m_boneGroup->m_actionPtr->m_boneMap[i];
				m_boneKeys[i].ptr->BeginBlend(s_lastKeys[gid].lastBoneKey, s_lastKeys[gid].lastEndIndex);
			}
		}
		else
		{
			m_boneKeys.SetArrayCount(0);
		}
		return true;
	}

	const MeshBoneKey*		MeshAction::getBoneKey(unsigned int boneIdx)
	{CPUTime(MeshAction);
		if(m_boneGroup == 0)return 0;
		if(boneIdx >= this->m_boneGroup->m_actionPtr->m_boneCount)
			return 0;
		return m_boneKeys[boneIdx].ptr;
	}

	int					MeshAction::GetBoneParentID(unsigned int boneIdx)
	{CPUTime(MeshAction);
		if(m_boneGroup == 0)return 0;
		if(boneIdx >= this->m_boneGroup->m_actionPtr->m_boneCount)
			return 0;
		return m_boneGroup->m_actionPtr->m_boneMap[m_boneKeys[boneIdx].ptr->m_boneIndex].m_parentIndex;
	}
	const matrix4x4*	MeshAction::GetBoneMatrixByGlobalID(unsigned int globalID)
	{CPUTime(MeshAction);
		if(m_boneGroup == 0)return 0;
		if(globalID >= m_boneKeys.size())
			return 0;
		if(m_boneKeys[globalID].data.m_boneIndex<0)return 0;
		return &m_boneKeys[globalID].data.m_matKey;
	}
	//
	int		MeshAction::findBone(int globalID)
	{CPUTime(MeshAction);
		if(m_boneGroup == 0 || globalID < 0 || globalID >= m_boneKeys.size())return -1;
		return m_boneKeys[globalID].data.m_boneIndex;
	}

};
