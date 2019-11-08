//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_BONE_H__
#define __PHANTOM_BONE_H__
#include "PhantomDevice.h"
#include <map>
#include "PhantomCamera.h"

#define	MAX_BONE_COUNT					512
#define	MAX_LOD_LEVEL_COUNT				3
#define	GEOMETRY_MATERIAL_FILE_LENGTH	128
#define	MAX_ACTION_SOUND_COUNT			2

namespace Phantom{

#pragma pack(push, 1)
	struct	PFileSeek//文件数据流位置
	{
		int								begin;
		int								end;
	};

	struct ActionEffect
	{
		FileName					file;//特效文件
		char						targetEnable;	//是否有目标
		char						bMatrixTransform;//是否矩阵变化
		float						speed;//特效移动速度
		Name						bornBoneName;//产生时绑定的骨骼
		ActionEffect(){
			speed = 100;
			targetEnable = false;
			bMatrixTransform = false;
		}
	};

	struct MaterialTexture
	{
		char						file[GEOMETRY_MATERIAL_FILE_LENGTH];
	};
	struct MaterialColorKey
	{
		color4	c;
		float	selfillum;	// self illumination
		float	uoffset, voffset;
	};

	struct MaterialAnim
	{
		int		bTiling;
		int		nUTile, nVTile;
		BOOL	bZWriteEnable;
		int		nStartFrame;
		float	fPlaybackRate;
		BOOL	bHasAnim;
		CDynamicArray<MaterialColorKey> keys;
	};
	struct MeshMaterialAttributeBase{
		int							cullMode;//渲染A面还是B面还是都渲染,选择Phantom::CullMode_Double, Phantom::CullMode_CW, Phantom::CullMode_CCW
		int							alphaMode;//设置透明通道使用类型AlphaMode_Blend,AlphaMode_AlphaDisable, ALPHAMODE_ALPHATEST
		color4						diffuse;//设置慢射光
		color4						emissive;//设置自发光
		color4						specular;//设置高光
		color4						ambient;//设置高光
		int							srcBlend;//源混合方式
		int							destBlend;//目标混和方式
		char						visibleInShadowMap;	//是否在shadowMap中进行显示
		char						lodEnable;	//此项是否使用LOD模型，因为一些纹理模型中会出现错误现象
		char						visible;
	};
	struct MeshMaterialTexture{
		Name						texture;
		int							nMinFilter;
		int							nMagFilter;
		int							nMaxAnisotropy;
		float						fMipMapLodBias;
	};
	#define	DIFFUSE_TEXTURE_COUNT	5
	struct MeshMaterialAttribute:MeshMaterialAttributeBase//只包含文件名，在模型文件目录下的相对路径
	{
		Name						name;
		Name						texture[TEXTURE_MAX_COUNT];
		color4						diffuseColor[DIFFUSE_TEXTURE_COUNT];//默认材质颜色
		Name						diffuseTexture[DIFFUSE_TEXTURE_COUNT];//其他的默认材质列表
		MeshMaterialAttribute();
	};
	struct	MeshBoneHelper
	{
		int							boneID;
		matrix4x4					invOriginal;
		MeshBoneHelper()			{this->boneID = 0;}
		~MeshBoneHelper()	{}
	};


	struct MeshVertexBlend: Vertex//带有骨骼动画的模型顶点数据类型
	{
		float							weight[4];
		unsigned char					boneID[4];
	};

	struct MeshVertexTBN: public Vertex//带有Tangent,Binormal,Normal值的顶点数据类型
	{
		float3							tangent;
		float3							binormal;
	};

	struct MeshVertexBlendTBN: public MeshVertexBlend//带有骨骼动画和Tangent,Binormal,Normal值的顶点数据类型
	{
		float3							tangent;
		float3							binormal;
	};

#pragma pack(pop)
	
	class BoneMatrixKey//: public BoneAnimationKey<matrix3x4>
	{
	public:
		BoneMatrixKey(){}
		~BoneMatrixKey(){m_keys.clear();}
		inline	matrix4x4*				GetKeyPtr(unsigned int index){return &m_keys[index % m_keys.size()];}
		inline	int						GetKeyCount(){return m_keys.size();}
		void							SaveData(CFileHelperW& w)
		{
			int numKey = m_keys.size();
			w << numKey;
			if(m_keys.size()>0)
				w.pStream->write(m_keys.GetPtr(), sizeof(matrix4x4) * m_keys.size());
		}
		virtual	void					LoadData(CFileHelperR& r, int version)
		{
			int numKey = 0;
			if(version<0x00000008){
				int numIndexKey = 0;
				r >> numIndexKey;
				r >> numKey;
				unsigned short* keyIndex	=	(unsigned short*)malloc_s(sizeof(unsigned short) * numKey);
				matrix3x4* keyData			=	(matrix3x4*)malloc_s(sizeof(matrix3x4) * numIndexKey);
				r.pStream->read(keyIndex, sizeof(unsigned short) * numKey);
				r.pStream->read(keyData, sizeof(matrix3x4) * numIndexKey);
				m_keys.resize(numKey);
				for(int i=0;i<numKey;i++)
				{
					 keyData[keyIndex[i]].get(m_keys[i]);
					 m_keys[i]._44 = 1;
				}
				free(keyIndex);
				free(keyData);
			}
			else{
				r >> numKey;
				m_keys.resize(numKey);
				if(version<0x00000011)
				{
					CDynamicArray<matrix3x4, short, 0>	temp;
					temp.SetArrayCount(numKey, 0);
					r.pStream->read(temp.GetPtr(), sizeof(matrix3x4) * temp.size());
					for(int i=0;i<temp.size();i++){
						temp[i].get(m_keys[i]);
						m_keys[i]._44 = 1;
					}
				}
				else
					r.pStream->read(m_keys.GetPtr(), sizeof(matrix4x4) * m_keys.size());
			}
		}
		CDynamicArray<matrix4x4, short, 0>	m_keys;
	};


	class	MeshBone;
	struct	MeshBoneKey
	{
		MeshBoneKey();
		~MeshBoneKey();
		char							SetKeyFrame(float fKeyFrame, int nStartFrame, int nEndFrame, int nFrame);
		const matrix4x4&				getMatrix(){return m_matKey;}
		void							BeginBlend(MeshBone* lastBone, int lastKeyIndex);
		//
		MeshBone*						m_boneKey;
		MeshBone*						m_lastBoneKey;
		short							m_lastEndIndex;		//结束针
		short							m_blendKeyIndex;	//
		//short							m_keyBeginIndex;	//开始针
		//short							m_keyEndIndex;		//结束针
		char							m_keyBlend;
		short							m_keyCurrent;		//结束针
		matrix4x4						m_matKey;
		short							m_boneIndex;
	};

	class MeshBone
	{
	public:
		MeshBone();
		~MeshBone();
		//
		inline	unsigned int			GetKeyCount(){return m_keyPtr->GetKeyCount();}
		void							destroy();
		//
		//char							SetKeyFrame(matrix4x4& ret, color4& diffuse, float fKeyFrame, int nStartFrame, int nEndFrame, MeshBoneKey* keyPtr);
		int								GetParentBoneID(){return m_parentIndex;}
	public:
		int								m_globalBoneID;
		BoneMatrixKey					m_keyList;	//保存
		short							m_parentIndex;
	public:
		BoneMatrixKey*					m_keyPtr;//动画列表
		int								m_keyBoneID;//
	};

	class MeshData;
	class MeshActionData;
#pragma pack(push,1)
	template <typename Tv>
	struct rint{
		rint(Tv& v){n=0;pv=&v;}
		~rint(){*pv = n;}
		int n;
		Tv* pv;
	};
#pragma pack(pop)
	class	MeshActionRange: public Base
	{
	public:
		MeshActionRange(MeshActionData* a);
		~MeshActionRange();
		ImplementComValue();
		//
		inline	void							SetActionSpeed(float fSpeed){m_fBaseSpeed = fSpeed;}	//设置动作速度
		inline	float							GetActionSpeed(){return m_fBaseSpeed;}
		//
		virtual	void							SetLoop(int beginIndex, int endIndex);	//设置动作循环变量
		virtual	int								GetKeyCount(){return (m_endFrame - m_startFrame);}	//
		inline	int								GetLoopBegin(){return m_startFrame;}	//返回循环开始点
		inline	int								GetLoopEnd(){return m_endFrame;}		//返回循环结束点
		inline	int								GetActionID(){return m_actionID;}		//返回动作ID
		inline	float							GetNextLerpTime(){return m_fNextLerpTime;}
		inline	void							SetNextLerpTime(float f){m_fNextLerpTime = f;}//设置柔和混合到这个动作时需要多长时间
		virtual	const char*						GetName(){return m_name;}
		virtual	VOID							SetName(const char* szName){m_name = szName;}	//设置动作名称
		//
		void									SaveAction(CFileHelperW& w);
		virtual	void							LoadAction(CFileHelperR& r);
		virtual	BOOL							IsMainAction(){return false;}	//是否主动作
		//virtual	VOID							SetRotationZ(float fDegress);	//转动
		float									GetTimeLength();
	public:
		MeshActionData*							m_actionPtr;
		int										m_actionType;
		int										m_startFrame;
		int										m_endFrame;
		Name									m_name;
		float									m_fBaseSpeed;
		int										m_actionID;
		float									m_fNextLerpTime;
		int										m_toHitKeyID;//目标在这个点播放受击动作
		BOOL									m_rabbinEnable;
		Name									m_sndfile;//音效文件,从sounds目录中搜索
		//ActionEffect*							m_effectPtr;
		int										m_nSndKeyIndex[MAX_ACTION_SOUND_COUNT];//播放音效关键帧ID
		int										m_nWaveID;
		float									m_fMoveOffset;
		char									m_bLoopMode;//是否启用循环模式
		char									m_bRegionOK;//
		PObjectRegion							m_region;
		PBoxVertices							m_aaboxVertex;
	};

	class MeshActionData: public MeshActionRange
	{
	public:
		MeshActionData();
		~MeshActionData();

		void							destroy();
		void							initBoneName(MeshData* data);
		//
		char							LoadData(CSafeFileHelperR& r);
		char							SaveData(CSafeFileHelperW& w);
		void							freeData();
		int								GetKeyCount();
		std::vector<MeshActionRange*>	m_actions;
		VOID							LoadMixFile(MeshData* group, const char* szMixFile);
		//
		BOOL							IsMainAction(){return true;}	//是否主动作
		char							RemoveAction(int id);
		CDynamicArrayS<AABox,0>			m_keyBoxs;
		int								m_boneCount;
		int								m_maxBoneIndex;
		MeshBone*						m_boneMap;
		MeshBone*						m_globalBoneMap[MAX_BONE_COUNT];
		int								m_loadVersion;
		//PFileSeek						m_fileSeek;
		friend class					MeshData;
		friend class					MeshAction;
		char							m_bMoveX,m_bMoveY,m_bMoveZ;
		CDynamicArrayS<float3,0>		m_moves;//移动数据
	};


	struct	ActionInterval
	{
		int					bone;
		MeshActionData*	actionPtr;
	};

	class	Mesh;
	struct	MeshBoneKeyR
	{
		MeshBoneKey					data;
		MeshBoneKey*				ptr;
	};
	struct	ActionData
	{
		short						keyBegin,keyEnd;
		short						keyLoop;
	};
	class	MeshAction
	{
	public:
		MeshAction();
		~MeshAction();
		//
		char							setGroup(MeshActionRange* pGroup, int bLoop, unsigned int keyBeginIndex = 0, unsigned int keyEndIndex = -1);
		char							SetActionInterval(MeshActionData* pGroup, int bLoop, int * boneIndexs, int boneCount);
		//char							FrameMove();
		float							GetTimeLength();
		void							SetKeyFrame(float fKeyFrame, ActionInterval* ai, int aiCount);
		void							initializeAction(int globalBoneCount);
		//
		char							IsInitialized(){return (GetBoneGroup() != 0);}
		int								GetBoneParentID(unsigned int boneIdx);
		//
		inline	const matrix4x4*		GetBoneMatrix(unsigned int boneIdx){if(m_boneGroup == 0|| boneIdx >= this->m_boneGroup->m_actionPtr->m_boneCount)return 0; return &this->m_boneKeys[boneIdx].ptr->m_matKey;}
		const matrix4x4*				GetBoneMatrixByGlobalID(unsigned int globalID);
		const MeshBoneKey*				getBoneKey(unsigned int boneIdx);
		int								GetBoneIDFromName(const char* szBoneName);
		//
		int								findBone(int globalID);
		//
#ifdef EDITOR_OS
		PtrID							m_boneGroupID;
		MeshActionRange*				GetBoneGroup();
#else
		inline	MeshActionRange*		GetBoneGroup(){return m_boneGroup;}
#endif
		ActionData						m_data;
	protected:
		Mesh*							m_groupPtr;
		char							m_bHitKey;
		CDynamicArrayS<MeshBoneKeyR, 0>		m_boneKeys;
		char							m_nPlayWave;
		char							m_bPlaySnd[2];
		MeshActionRange*				m_boneGroup;
		friend class					Mesh;
	};

};

#endif
