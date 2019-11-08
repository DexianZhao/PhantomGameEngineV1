//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUnit3DElement.h"
#include "PhantomUnit3D.h"
#include "PhantomManager.h"
namespace Phantom
{

CActionElement::CActionElement(Unit3DActionData* effectPtr)
{
	m_nChangeV			=	1;
	m_id				=	0;
	m_trackBeginIndex			=	0;
	m_trackEndIndex			=	effectPtr->m_nTrackKeyCount - 1;
	m_bVisible			=	true;
	m_actionPtr		=	effectPtr;
	m_bindPoint			=	-1;
	m_frameCount		=	0;
	//

	//
	//InitKey(MeshKeyIndex_Parameter1, "参数1", false, false, 0.0f, 1.0f, 0);
	//InitKey(MeshKeyIndex_Parameter2, "参数2", false, false, 0.0f, 1.0f, 0);
	//InitKey(MeshKeyIndex_Parameter3, "参数3", false, false, 0.0f, 1.0f, 0);
	//
	m_bTemplateVisible	=	false;
}

CActionElement::~CActionElement()
{
}

VOID					CActionElement::InitKey(FloatAnimation& fani, BOOL bEnable, BOOL bVisible, float fMin, float fMax, float fDefault, BOOL bIsColor)
{CPUTime(CActionElement);
	fani.SetRange(fMin, fMax);
	fani.SetDefaultValue(fDefault);
	fani.SetEnabled(bEnable);
	fani.SetVisible(bVisible);
	fani.SetColorAni(bIsColor);
}

INT					CActionElement::GetBeginTrackIndex()	//取得开始显示的关键帧
{CPUTime(CActionElement);
	return m_trackBeginIndex;
}

INT					CActionElement::GetEndTrackIndex()	//取得结束显示的关键帧
{CPUTime(CActionElement);
	return m_trackEndIndex;
}

const char*			CActionElement::GetName()	//设置名称
{CPUTime(CActionElement);
	return m_name;
}

VOID				CActionElement::SetName(const char* szName)
{CPUTime(CActionElement);
	m_name	=	szName;
}
VOID				CActionElement::SetBeginTrackIndex(INT nFrame)	//设置开始显示的关键帧
{CPUTime(CActionElement);
	this->m_trackBeginIndex	=	nFrame;
	if(m_trackEndIndex < m_trackBeginIndex)
		m_trackEndIndex	=	m_trackBeginIndex;
}

VOID				CActionElement::SetEndTrackIndex(INT nFrame)	//设置结束显示的关键帧
{CPUTime(CActionElement);
	this->m_trackEndIndex	=	nFrame;
	if(m_trackBeginIndex > m_trackEndIndex)
		m_trackBeginIndex	=	m_trackEndIndex;
}

VOID				CActionElement::SetFrameCount(INT nFrameCount)
{CPUTime(CActionElement);
	m_frameCount	=	nFrameCount;
	if(m_frameCount < 0)
		m_frameCount	=	0;
	for(int i=0;i<GetKeyCount();i++)
		GetKeyPtr(i)->SetTrackKeyCount(m_frameCount);
}
VOID								CActionElement::Save(CFileHelperW& w)
{CPUTime(CActionElement);
	char version = 1;
	w << version;
	skip_w sw(w.pStream);
	w << m_id << m_name << m_trackBeginIndex << m_trackEndIndex << m_bVisible << m_bindPoint << m_frameCount;
	w << m_bTemplateVisible;
	sw.end();
}
VOID								CActionElement::Load(CFileHelperR& r)
{CPUTime(CActionElement);
	char version = 1;
	r >> version;
	skip_r sr;
	sr.begin(r.pStream);
	r >> m_id >> m_name >> m_trackBeginIndex >> m_trackEndIndex >> m_bVisible >> m_bindPoint >> m_frameCount;
	r >> m_bTemplateVisible;
	sr.end();
}

VOID								CActionElement::CloneFrom(CActionElement* eff)
{CPUTime(CActionElement);
	CActionElement* e	=	static_cast<CActionElement*>(eff);
	if(this->m_actionPtr != e->m_actionPtr)
		this->m_name	=	e->m_name;
	m_trackBeginIndex	=	e->m_trackBeginIndex;
	m_trackEndIndex	=	e->m_trackEndIndex;
	m_bVisible		=	e->m_bVisible;
	m_bindPoint		=	e->m_bindPoint;
	m_frameCount	=	e->m_frameCount;
	m_bTemplateVisible	=	e->m_bTemplateVisible;
	//SetFileName(e->m_fileName);
}

CElementMusic3D::CElementMusic3D(Unit3DActionData* e)
	: CActionElement(e)
{
	m_fIncidence	=	To3DValue(5.0f);
	m_fWeight		=	1.0f;
	m_fPlaySpeed	=	1.0f;
	m_bLoop			=	false;
}

CElementMusic3D::~CElementMusic3D()
{
}

float		CElementMusic3D::GetPlaySpeed()
{CPUTime(CElementMusic3D);
	return m_fPlaySpeed;
}

VOID		CElementMusic3D::SetPlaySpeed(float f)
{CPUTime(CElementMusic3D);
	m_fPlaySpeed	=	f;
}

VOID		CElementMusic3D::CloneFrom(CActionElement* eff)
{CPUTime(CElementMusic3D);
	CActionElement::CloneFrom(eff);
	if(!eff || eff->GetElementType() != ElementType_Sound)
		return;
	CElementMusic3D* geom	=	static_cast<CElementMusic3D*>(eff);
	m_fIncidence	=	geom->m_fIncidence;
	m_fWeight		=	geom->m_fWeight;
	m_fPlaySpeed	=	geom->m_fPlaySpeed;
	m_bLoop			=	geom->m_bLoop;
}

VOID		CElementMusic3D::Save(CFileHelperW& w)
{CPUTime(CElementMusic3D);
	CActionElement::Save(w);
	char version = 1;
	w << version;
	skip_w sw(w.pStream);
	w << m_fIncidence << m_fWeight;
	w << m_fPlaySpeed;
	w << m_bLoop;
	w.str(m_fileName);
	sw.end();
}
VOID		CElementMusic3D::Load(CFileHelperR& r)
{CPUTime(CElementMusic3D);
	CActionElement::Load(r);
	char version;
	r >> version;
	skip_r sr;
	sr.begin(r.pStream);
	r >> m_fIncidence >> m_fWeight;
	r >> m_fPlaySpeed;
	r >> m_bLoop;
	r.str(m_fileName);
	sr.end();
}
void		CElementMusic3D::SetIncidence(float fDistance)//设置影响范围
{CPUTime(CElementMusic3D);
	m_fIncidence	=	fDistance;
}
float		CElementMusic3D::GetIncidence()	//返回影响范围
{CPUTime(CElementMusic3D);
	return m_fIncidence;
}
float		CElementMusic3D::GetWeight()			//设置声音权重
{CPUTime(CElementMusic3D);
	return m_fWeight;
}
void		CElementMusic3D::SetWeight(float f)	//设置声音权重
{CPUTime(CElementMusic3D);
	m_fWeight	=	f;
}

CElementMeshControl::CElementMeshControl(Unit3DActionData* e)
	: CActionElement(e)
{
	m_nActionID	=	0;
	m_nKeyBegin	=	0;
	m_nKeyEnd	=	-1;
	m_nLoopCount	=	1;
	m_fActionSpeed	=	1.0f;
	InitKey(m_keys[MeshKeyIndex_ColorR], false, true, 0, 1, 0, true);
	InitKey(m_keys[MeshKeyIndex_ColorG], false, true, 0, 1, 0, true);
	InitKey(m_keys[MeshKeyIndex_ColorB], false, true, 0, 1, 0, true);
	InitKey(m_keys[MeshKeyIndex_Alpha], false, true, 0, 1, 1);
	//
	InitKey(m_keys[MeshKeyIndex_ScaleX], false, true, -1, 1, 1);
	InitKey(m_keys[MeshKeyIndex_ScaleY], false, true, -1, 1, 1);
	InitKey(m_keys[MeshKeyIndex_ScaleZ], false, true, -1, 1, 1);
	InitKey(m_keys[MeshKeyIndex_ScaleFull], false, true, -1, 1, 1);
	//
	InitKey(m_keys[MeshKeyIndex_PositionX], false, true, -1, 1, 0);
	InitKey(m_keys[MeshKeyIndex_PositionY], false, true, -1, 1, 0);
	InitKey(m_keys[MeshKeyIndex_PositionZ], false, true, -1, 1, 0);
	//
	InitKey(m_keys[MeshKeyIndex_RotationOffset], false, true, -1, 1, 0);
	InitKey(m_keys[MeshKeyIndex_RotationX], false, true, -360.0f, 360.0f, 0);
	InitKey(m_keys[MeshKeyIndex_RotationZ], false, true, -360.0f, 360.0f, 0);
}

CElementMeshControl::~CElementMeshControl()
{
}

BOOL		CElementMeshControl::IsEnabled(int index)
{CPUTime(CElementMeshControl);
	if(index<0||index>=MeshKeyIndex_Count)
		return false;
	return m_keys[index].IsEnabled();
}
float		CElementMeshControl::GetKeyValue(int index, float fTime)
{CPUTime(CElementMeshControl);
	if(index<0||index>=MeshKeyIndex_Count)
		return false;
	return m_keys[index].GetFrameValue(fTime);
}

VOID		CElementMeshControl::CloneFrom(CActionElement* eff)
{CPUTime(CElementMeshControl);
	CActionElement::CloneFrom(eff);
	if(!eff || eff->GetElementType() != ElementType_Mesh)
		return;
	CElementMeshControl* geom	=	static_cast<CElementMeshControl*>(eff);
	m_nActionID		=	geom->m_nActionID;
	m_nKeyBegin		=	geom->m_nKeyBegin;
	m_nKeyEnd		=	geom->m_nKeyEnd;
	m_nLoopCount	=	geom->m_nLoopCount;
	m_fActionSpeed	=	geom->m_fActionSpeed;
	m_meshid		=	geom->m_meshid;//模型数据id，如果是0则表示控制主模型
	m_submeshs		=	geom->m_submeshs;//控制的所有子模型数量,只有在控制所有模型的时候动作才能被设置有效
	for(int i=0;i<MeshKeyIndex_Count;i++)
		m_keys[i].CloneFrom(&geom->m_keys[i]);
}
const char*	CElementMeshControl::GetKeyName(int index)//返回中文
{CPUTime(CElementMeshControl);
	switch(index){
	case MeshKeyIndex_ColorR:
	case MeshKeyIndex_ColorG:
	case MeshKeyIndex_ColorB:
		return utf8("颜色");
	case MeshKeyIndex_Alpha:
		return utf8("透明");
	case MeshKeyIndex_ScaleX:
	case MeshKeyIndex_ScaleY:
	case MeshKeyIndex_ScaleZ:
		return utf8("缩放");
	case MeshKeyIndex_ScaleFull:
		return utf8("整体缩放");
	case MeshKeyIndex_PositionX:
	case MeshKeyIndex_PositionY:
	case MeshKeyIndex_PositionZ:
		return utf8("位置");
	case MeshKeyIndex_RotationOffset:
		return utf8("转动偏移");
	case MeshKeyIndex_RotationX:
	case MeshKeyIndex_RotationZ:
		return utf8("转动");
	}
	return "";
}
VOID		CElementMeshControl::Save(CFileHelperW& w)
{CPUTime(CElementMeshControl);
	CActionElement::Save(w);
	char version = 1;
	w << version;
	skip_w sw(w.pStream);
	w << m_nActionID << m_nKeyBegin << m_nKeyEnd << m_nLoopCount << m_fActionSpeed << m_meshid;
	short subMesh = m_submeshs.size();
	w << subMesh;
	if(subMesh)
		w.pStream->write(m_submeshs.GetPtr(), m_submeshs.size() * sizeof(short));
	CSafeFileHelperW fw;
	fw.pStream		=	w.pStream;
	fw.bReadOnly	=	false;
	//
	char count = MeshKeyIndex_Count;
	w << count;
	for(int i=0;i<count;i++)
		m_keys[i].Save(fw);
	sw.end();
}
VOID		CElementMeshControl::Load(CFileHelperR& r)
{CPUTime(CElementMeshControl);
	CActionElement::Load(r);
	char version;
	r >> version;
	skip_r sr;
	sr.begin(r.pStream);
	r >> m_nActionID >> m_nKeyBegin >> m_nKeyEnd >> m_nLoopCount >> m_fActionSpeed >> m_meshid;
	short subMesh = 0;
	r >> subMesh;
	if(subMesh){
		m_submeshs.resize(subMesh);
		r.pStream->read(m_submeshs.GetPtr(), m_submeshs.size()*sizeof(short));
	}
	char count;
	r >> count;
	CSafeFileHelperR fr;
	fr.pStream	=	r.pStream;
	for(int i=0;i<count;i++)
	{
		if(i<MeshKeyIndex_Count)
			m_keys[i].Load(fr);
		else{
			FloatAnimation temp;
			temp.Load(fr);
		}
	}
	sr.end();
}
int				CElementMeshControl::GetActionID(){CPUTime(CElementMeshControl);return m_nActionID;}
VOID			CElementMeshControl::SetActionID(int nID){CPUTime(CElementMeshControl);m_nActionID = nID;}
int				CElementMeshControl::GetKeyBeginIndex(){CPUTime(CElementMeshControl);return m_nKeyBegin;}
int				CElementMeshControl::GetKeyEndIndex(){CPUTime(CElementMeshControl);return m_nKeyEnd;}
VOID			CElementMeshControl::SetKeyBeginIndex(int n){CPUTime(CElementMeshControl);m_nKeyBegin = n;}
VOID			CElementMeshControl::SetKeyEndIndex(int n){CPUTime(CElementMeshControl);m_nKeyEnd = n;}
void			CElementMeshControl::SetActionLoop(int loopCount){CPUTime(CElementMeshControl);m_nLoopCount = loopCount;}	//设置是否循环播放
int				CElementMeshControl::GetActionLoop(){CPUTime(CElementMeshControl);return m_nLoopCount;}		//是否循环播放
VOID			CElementMeshControl::SetActionSpeed(float fSpeed){CPUTime(CElementMeshControl);m_fActionSpeed = fSpeed;}
float			CElementMeshControl::GetActionSpeed(){CPUTime(CElementMeshControl);return m_fActionSpeed;}




CElementRuntime::CElementRuntime(CActionElement* ele, Unit3DAction* a)
{
	m_elementPtr	=	ele;
	//m_worldMat.identity();
	m_bRender		=	false;
	m_fLerp			=	0.0f;
	m_fFrameIndex	=	0.0f;
	m_actionPtr		=	a;
	m_bPlaying		=	false;
	m_bVisible		=	true;
	m_nChangeV		=	ele->m_nChangeV;
	m_bTemplateVisible	=	ele->m_bTemplateVisible;
}

CElementRuntime::~CElementRuntime()
{
}

VOID			CElementRuntime::CheckChange()
{CPUTime(CElementRuntime);
	if(m_nChangeV != m_elementPtr->m_nChangeV)
	{
		m_nChangeV	= m_elementPtr->m_nChangeV;
		m_bTemplateVisible	=	m_elementPtr->m_bTemplateVisible;
	}
}

BOOL			CElementRuntime::BeforeFrameMove(EmissionFrameMove& fm, Mesh* basePtr, float fTrackIndex)
{CPUTime(CElementRuntime);
	if(!m_elementPtr->m_bVisible || !m_bVisible)
	{
		if(this->m_bPlaying)
		{
			this->m_bPlaying	=	false;
			this->OnStopElement();
		}
		return false;
	}
	int nTrackIndex	=	(int)fTrackIndex;
	if(nTrackIndex < m_elementPtr->m_trackBeginIndex || nTrackIndex > m_elementPtr->m_trackEndIndex)
	{
		if(this->m_bPlaying)
		{
			this->m_bPlaying	=	false;
			this->OnStopElement();
		}
		return false;
	}
	int trackLength	=	m_elementPtr->m_trackEndIndex - m_elementPtr->m_trackBeginIndex;
	m_bRender		=	true;
	if(m_elementPtr->m_frameCount <= 0 || trackLength <= 0)
	{
		m_fFrameIndex	=	0;
		if(!this->m_bPlaying)
		{
			this->m_bPlaying	=	true;
			this->OnStartElement();
		}
		return true;
	}
	m_fLerp			=	(fTrackIndex - (float)m_elementPtr->m_trackBeginIndex) / (float)trackLength;
	m_fFrameIndex	=	m_fLerp * (float)m_elementPtr->m_frameCount;
	//
	if(!this->m_bPlaying)
	{
		this->m_bPlaying	=	true;
		this->OnStartElement();
	}
	return true;
}

BOOL			CElementRuntime::GetKeyMatrix(matrix4x4& ret, const matrix4x4* parent, float fFrameIndex, float3* scaleRet)
{CPUTime(CElementRuntime);
	return false;
}

VOID			CElementRuntime::FrameMove(Mesh* basePtr, const matrix4x4& actionMat)
{CPUTime(CElementRuntime);
	if(!m_bRender)
		return;
	if(!IsRequestMatrix())
	{
		OnFrameMove(matrix4x4::s_identity());
		return;
	}
	const matrix4x4* parent	=	0;
	if(basePtr)
		parent	=	basePtr->GetBoneMatrix(m_elementPtr->m_bindPoint);
	if(!parent)
		parent	=	&actionMat;
	//
	matrix4x4 worldMat;
	GetKeyMatrix(worldMat, parent, m_fFrameIndex);
	//
	OnFrameMove(worldMat);
}

VOID			CElementRuntime::BeginAction()
{CPUTime(CElementRuntime);
	StopAction();
}
VOID			CElementRuntime::StopAction()
{CPUTime(CElementRuntime);
	if(this->m_bPlaying)
	{
		this->m_bPlaying	=	false;
		this->OnStopElement();
	}
}

VOID			CElementRuntime::OnRender()
{CPUTime(CElementRuntime);
	m_bRender		=	false;
}

CElementMeshRuntime::CElementMeshRuntime(CElementMeshControl* act, Unit3DAction* a)
	:CElementRuntime(act, a)
{
}

CElementMeshRuntime::~CElementMeshRuntime()
{
}
BOOL				CElementMeshRuntime::GetKeyMatrix(matrix4x4& ret, const matrix4x4* parent, float fFrameIndex, float3* scaleRet)
{CPUTime(CElementMeshRuntime);
	matrix4x4 mat, matS;
	//color4	color(m_keys[MeshKeyIndex_PositionX]->GetFrameValue(m_fFrameIndex), m_keys[MeshKeyIndex_PositionY]->GetFrameValue(m_fFrameIndex), m_keys[MeshKeyIndex_PositionZ]->GetFrameValue(m_fFrameIndex));
	float3	offset(0, 0, 0);
	if(IsEnabled(MeshKeyIndex_PositionX))offset.x	=	To3DValue(GetValueFrame(MeshKeyIndex_PositionX, fFrameIndex));
	if(IsEnabled(MeshKeyIndex_PositionY))offset.y	=	To3DValue(GetValueFrame(MeshKeyIndex_PositionY, fFrameIndex));
	if(IsEnabled(MeshKeyIndex_PositionZ))offset.z	=	To3DValue(GetValueFrame(MeshKeyIndex_PositionZ, fFrameIndex));
	//
	float	offsetSelf	=	0;
	if(IsEnabled(MeshKeyIndex_RotationOffset))offsetSelf	=	To3DValue(GetValueFrame(MeshKeyIndex_RotationOffset, fFrameIndex));
	//
	float3	scale(1,1,1);
	float	scaleFull	=	1;
	//
	if(IsEnabled(MeshKeyIndex_ScaleX))scale.x	=	GetValueFrame(MeshKeyIndex_ScaleX, fFrameIndex);
	if(IsEnabled(MeshKeyIndex_ScaleY))scale.y	=	GetValueFrame(MeshKeyIndex_ScaleY, fFrameIndex);
	if(IsEnabled(MeshKeyIndex_ScaleZ))scale.z	=	GetValueFrame(MeshKeyIndex_ScaleZ, fFrameIndex);
	//
	if(IsEnabled(MeshKeyIndex_ScaleFull))scaleFull	=	GetValueFrame(MeshKeyIndex_ScaleFull, fFrameIndex);
	//
	//scale	*=	(scaleFull * GetBaseScale());
	if(scaleRet)
		*scaleRet	=	scale;
	//
	float	rotationX	=	0;
	float	rotationZ	=	0;
	if(IsEnabled(MeshKeyIndex_RotationX))rotationX	=	Radians(GetValueFrame(MeshKeyIndex_RotationX, fFrameIndex));
	if(IsEnabled(MeshKeyIndex_RotationZ))rotationZ	=	Radians(GetValueFrame(MeshKeyIndex_RotationZ, fFrameIndex));
	//
	matrixScaling(&matS, scale.x, scale.y, scale.z);
	matS._42	=	offsetSelf;
	matrixRotationX(&mat, rotationX);
	matrixMultiply(&matS, &matS, &mat);
	matrixRotationZ(&mat, rotationZ);
	matrixMultiply(&ret, &matS, &mat);
	ret.SetPosition(offset);
	//
	if(parent)
		matrixMultiply(&ret, &ret, parent);
	return true;
}
BOOL				CElementMeshRuntime::BeforeFrameMove(EmissionFrameMove& fm, Mesh* basePtr, float fTrackIndex)
{CPUTime(CElementMeshRuntime);
	if(!CElementRuntime::BeforeFrameMove(fm, basePtr, fTrackIndex))
		return false;
	CElementMeshControl*		act	=	GetActionPtr();
	if(act->m_nActionID >= 0 && basePtr)
	{
		fm.actionID		=	act->m_nActionID;
		//
		int nEnd		=	act->m_nKeyEnd;
		if(nEnd < 0)
		{
			m_actionID.id	=	act->m_nActionID;
			MeshActionRange* action	=	basePtr->GetDataPtr()->GetActionPtr(m_actionID);
			if(action)
				nEnd	=	action->GetKeyCount() - 1;
		}
		float fCount	=	(float)nEnd - (float)act->m_nKeyBegin;
		if(fCount > 0)
		{
			float fMax				=	fCount * (float)act->m_nLoopCount - 1.0f;
			float fIndex			=	m_fLerp * fMax;///*act->m_fActionSpeed * */fCount * (float)act->m_nLoopCount;
			if(fIndex >= fMax)fIndex	=	fMax - 0.0001f;
			fm.actionKeyIndex		=	fIndex + (float)act->m_nKeyBegin;
			//int nLoop				=	(int)(fIndex / fCount);
			//if(act->m_nLoopCount < 0)
			//else
			//{
			//	if(nLoop >= act->m_nLoopCount)
			//		fm.actionKeyIndex		=	(float)nEnd;
			//	else
			//		fm.actionKeyIndex		=	fmodf(fIndex, fCount) + (float)act->m_nKeyBegin;
			//}
		}
	}
	if(fm.bOffset[0] = IsEnabled(MeshKeyIndex_PositionX))fm.offset.x	+=	To3DValue(GetValue(MeshKeyIndex_PositionX));
	if(fm.bOffset[1] = IsEnabled(MeshKeyIndex_PositionY))fm.offset.y	+=	To3DValue(GetValue(MeshKeyIndex_PositionY));
	if(fm.bOffset[2] = IsEnabled(MeshKeyIndex_PositionZ))fm.offset.z	+=	To3DValue(GetValue(MeshKeyIndex_PositionZ));
	if(IsEnabled(MeshKeyIndex_RotationOffset))fm.offsetSelf	+=	To3DValue(GetValue(MeshKeyIndex_RotationOffset));
	//
	if(IsEnabled(MeshKeyIndex_ScaleFull))fm.scale	*=	GetValue(MeshKeyIndex_ScaleFull);
	if(IsEnabled(MeshKeyIndex_ScaleX))fm.scale.x	*=	GetValue(MeshKeyIndex_ScaleX);
	if(IsEnabled(MeshKeyIndex_ScaleY))fm.scale.y	*=	GetValue(MeshKeyIndex_ScaleY);
	if(IsEnabled(MeshKeyIndex_ScaleZ))fm.scale.z	*=	GetValue(MeshKeyIndex_ScaleZ);
	//
	if(IsEnabled(MeshKeyIndex_RotationX))fm.rotationX	+=	Radians(GetValue(MeshKeyIndex_RotationX));
	if(IsEnabled(MeshKeyIndex_RotationZ))fm.rotationZ	+=	Radians(GetValue(MeshKeyIndex_RotationZ));
	//
	if(fm.bColor[0] = IsEnabled(MeshKeyIndex_ColorR))fm.color.r		+=	GetValue(MeshKeyIndex_ColorR);
	if(fm.bColor[1] = IsEnabled(MeshKeyIndex_ColorG))fm.color.g		+=	GetValue(MeshKeyIndex_ColorG);
	if(fm.bColor[2] = IsEnabled(MeshKeyIndex_ColorB))fm.color.b		+=	GetValue(MeshKeyIndex_ColorB);
	if(fm.bColor[3] = IsEnabled(MeshKeyIndex_Alpha))fm.color.a		*=	GetValue(MeshKeyIndex_Alpha);
	//
	//if(IsEnabled(MeshKeyIndex_Parameter1))fm.shaderParam	*=	GetValue(MeshKeyIndex_Parameter1);
	//if(IsEnabled(MeshKeyIndex_Parameter2))fm.shaderParam[1]	*=	GetValue(MeshKeyIndex_Parameter2);
	//if(IsEnabled(MeshKeyIndex_Parameter3))fm.shaderParam[2]	*=	GetValue(MeshKeyIndex_Parameter3);
	return true;
}

CElementMusic3DRuntime::CElementMusic3DRuntime(CElementMusic3D* act, Unit3DAction* a)
	:CElementRuntime(act, a)
{
    m_fileIndex = -1;
	if(fexist(act->m_fileName))
	{
        FileName ext;
        act->m_fileName.splitpath(0,0,&ext);
        if(ext==".wav"){
            m_fileIndex = g_manager.GetMusicFileIndex(act->m_fileName.t);
        }
	}
	m_bPlaying		=	false;
}

CElementMusic3DRuntime::~CElementMusic3DRuntime()
{
	//m_musicPtr.Destroy();
}

VOID		CElementMusic3DRuntime::OnStartElement()
{CPUTime(CElementMusic3DRuntime);
    if(m_fileIndex>=0)
    {
        MultiMusic* snd=g_manager.GetMusicFilePtrByIndex(m_fileIndex);
        if(snd){
            CElementMusic3D*	music	=	GetMusicPtr();
            PtrID id = snd->Play();
            snd->SetPlaySpeed(id, music->m_fPlaySpeed * this->m_actionPtr->m_actionPtr->m_fPlaySpeed);
        }
    }
//	if(m_musicPtr.IsLoaded())
//	{
//		CElementMusic3D*	music	=	GetMusicPtr();
//		m_musicPtr.Play(music->m_bLoop ? -1 : 1);
//		m_musicPtr.SetPlaySpeed(music->m_fPlaySpeed * this->m_actionPtr->m_actionPtr->m_fPlaySpeed);
//	}
}
VOID		CElementMusic3DRuntime::OnStopElement()
{CPUTime(CElementMusic3DRuntime);
	//if(m_musicPtr.IsLoaded())
	//	m_musicPtr.Stop();
}

BOOL		CElementMusic3DRuntime::BeforeFrameMove(EmissionFrameMove& fm, Mesh* basePtr, float fTrackIndex)
{CPUTime(CElementMusic3DRuntime);
	if(!CElementRuntime::BeforeFrameMove(fm, basePtr, fTrackIndex))
		return false;
	//
	//CElementMusic3D*	music	=	GetMusicPtr();
	return true;
}

VOID		CElementMusic3DRuntime::OnFrameMove(const matrix4x4& worldMat)
{CPUTime(CElementMusic3DRuntime);
	if(!m_bRender && this->m_bPlaying)
		this->StopAction();
//	if(m_musicPtr.IsLoaded())
//	{
//		CElementMusic3D*	music	=	GetMusicPtr();
//		//m_musicPtr->SetWorldMatrix(m_worldMat);
//		//m_musicPtr->FrameMove(r);
//	}
}

CElementEmission::CElementEmission(Unit3DActionData* effectPtr)
	:CActionElement(effectPtr)
{
	m_bLookAtNext		=	false;	//正视下一个方向点,模型渲染方式下有效
	//
	m_bUseSystemWind	=	true;	//系统风向对粒子有效
	m_bIsControlCamera	=	false;
	//
	m_nEmissionType		=	EmissionType_Matrix;
	m_nRenderType		=	SHOW_Particle_Billboard;					//显示类型,比如billboard,cameraboard, zboard等
	m_nTextureOP		=	Phantom::TextureBlend_MODULATE;
	//m_fCreationTime		=	1.0f;
	//m_bAutoInZBoard		=	false;
	//
	m_texturePtr		=	0;
	//for(int i=0;i<MAX_EMISSION_BIND_POINTS;i++)
	//	m_bindPoints[i]	=	-1;
	//m_nMaxParticle		=	100;
	//m_nLoopCount		=	1;					//是否循环播放
	m_nEmissionPartIndex	=	-1;				//是否在模型面上随机产生
	//
	//m_keys[MeshKeyIndex_ColorR].SetEnabled(true);
	//m_keys[MeshKeyIndex_ColorG].SetEnabled(true);
	//m_keys[MeshKeyIndex_ColorB].SetEnabled(true);
	//m_keys[MeshKeyIndex_Alpha].SetEnabled(true);
	////
	//m_keys[MeshKeyIndex_ColorR].SetDefaultValue(.50f);
	//m_keys[MeshKeyIndex_ColorG].SetDefaultValue(.50f);
	//m_keys[MeshKeyIndex_ColorB].SetDefaultValue(.50f);
	//InitKey(MeshKeyIndex_Parameter1, "存活期", true, true, 0.0f, 10.0f, 1.0f);
	//InitKey(MeshKeyIndex_Parameter2, "参数2", false, false, 0.0f, 1.0f, 0);
	//InitKey(MeshKeyIndex_Parameter3, "参数3", false, false, 0.0f, 1.0f, 0);
	//
	//InitParticleKey(ParticleKey_ColorR, "颜色", false, true, 0, 1, 0);
	//InitParticleKey(ParticleKey_ColorG, "颜色", false, true, 0, 1, 0);
	//InitParticleKey(ParticleKey_ColorB, "颜色", false, true, 0, 1, 0);
	//InitParticleKey(ParticleKey_Alpha, "透明", true, true, 0, 1, 1);
	//InitParticleKey(ParticleKey_ScaleX, "缩放", false, true, -1, 1, 1);
	//InitParticleKey(ParticleKey_ScaleY, "缩放", false, true, -1, 1, 1);
	//InitParticleKey(ParticleKey_ScaleZ, "缩放", false, true, -1, 1, 1);
	//InitParticleKey(ParticleKey_ScaleFull, "整体缩放", false, true, -1, 1, 1);
	//InitParticleKey(ParticleKey_PositionX, "移动", false, true, -1, 1, 0);
	//InitParticleKey(ParticleKey_PositionY, "移动", false, true, -1, 1, 0);
	//InitParticleKey(ParticleKey_PositionZ, "移动", false, true, -1, 1, 0);
	//InitParticleKey(ParticleKey_PositionSelf, "转动偏移", false, true, -1, 1, 0);
	//InitParticleKey(ParticleKey_RotationX, "转动", false, true, -360.0f, 360.0f, 0);
	//InitParticleKey(ParticleKey_RotationZ, "转动", false, true, -360.0f, 360.0f, 0);
	////
	//InitParticleKey(ParticleKey_PlaneOffsetX, "面偏移", false, true, -1, 1, 0);
	//InitParticleKey(ParticleKey_PlaneOffsetY, "面偏移", false, true, -1, 1, 0);
	//InitParticleKey(ParticleKey_PlaneOffsetZ, "面偏移", false, true, -1, 1, 0);
	//InitParticleKey(ParticleKey_SelfRotation, "自转", false, true, -360.0f, 360.0f, 0);
	//m_nParticleFrameCount	=	1;
}

CElementEmission::~CElementEmission(void)
{
	DestroyAll();
}

//
//int						CElementEmission::GetParticleFrameCount()
//{
//	return this->m_nParticleFrameCount;
//}
//
//VOID					CElementEmission::SetParticleFrameCount(int n)
//{
//	m_nParticleFrameCount	=	getmax(n, 5);
//	for(int i=0;i<ParticleKey_Count;i++)
//		m_particleKeys[i].SetTrackKeyCount(m_nParticleFrameCount);
//}

VOID					CElementEmission::CloneFrom(CActionElement* eff)
{CPUTime(CElementEmission);
	CActionElement::CloneFrom(eff);
	if(!eff || eff->GetElementType() != ElementType_Emission)
		return;
	CElementEmission* geom	=	static_cast<CElementEmission*>(eff);
	m_data					=	geom->m_data;
	m_bLookAtNext			=	geom->m_bLookAtNext;	//正视下一个方向点,模型渲染方式下有效
	m_bUseSystemWind		=	geom->m_bUseSystemWind;	//系统风向对粒子有效
	m_bIsControlCamera		=	geom->m_bIsControlCamera;
	m_nEmissionType			=	geom->m_nEmissionType;
	m_nRenderType			=	geom->m_nRenderType;					//显示类型,比如billboard,cameraboard, zboard等
	m_nTextureOP			=	geom->m_nTextureOP;
	m_nEmissionPartIndex	=	geom->m_nEmissionPartIndex;				//是否在模型面上随机产生
	m_nBindPoint			=	geom->m_nBindPoint;
	m_fileName				=	geom->m_fileName;
}

VOID					CElementEmission::Save(CFileHelperW& w)
{CPUTime(CElementEmission);
	CActionElement::Save(w);
	char version = 1;
	w << version;
	skip_w sw(w.pStream);
	w << m_data;
	w << m_bLookAtNext << m_bUseSystemWind << m_bIsControlCamera << m_nEmissionType << m_nRenderType << m_nTextureOP;// << m_nDestBlend << m_nSrcBlend;
	w << m_nEmissionPartIndex;
	w.str(m_fileName);
	w << m_nBindPoint;
	sw.end();
}
VOID					CElementEmission::Load(CFileHelperR& r)
{CPUTime(CElementEmission);
	CActionElement::Load(r);
	char version;
	r >> version;
	skip_r sr;
	sr.begin(r.pStream);
	r >> m_data;
	r >> m_bLookAtNext >> m_bUseSystemWind >> m_bIsControlCamera >> m_nEmissionType >> m_nRenderType >> m_nTextureOP;// >> m_nDestBlend >> m_nSrcBlend;
	r >> m_nEmissionPartIndex;
	r.str(m_fileName);
	r >> m_nBindPoint;
	sr.end();
	TexturePtr tex;
	FileName path;
	m_actionPtr->m_unitPtr->m_fileName.splitpath(&path, 0, 0);
	if(LoadTexture(tex, m_fileName.t, path, true))
		m_texturePtr	=	tex.GetPtr();
}
//始终显示多少个粒子,那个得加
VOID				CElementEmission::SetFileName(const char* szFile)
{CPUTime(CElementEmission);
	m_fileName	=	szFile;
	m_fileName.setIsFile();
	AutoPtr<Texture> old	=	m_texturePtr;
	TexturePtr tex;
	FileName path;
	m_actionPtr->m_unitPtr->m_fileName.splitpath(&path, 0, 0);
	if(LoadTexture(tex, m_fileName.t, path, true))
		m_texturePtr	=	tex.GetPtr();
}

VOID				CElementEmission::DestroyAll()
{CPUTime(CElementEmission);
	m_texturePtr	=	0;
}


CElementEmissionRuntime::CElementEmissionRuntime(CElementEmission* e, Unit3DAction* a)
	:CElementRuntime(e, a)
{
}

CElementEmissionRuntime::~CElementEmissionRuntime()
{
	//for(int i=0;i<this->m_nParticleCount;i++)
	//	g_manager.FreeParticle(m_localParticles[i]);
	//m_nParticleCount	=	0;
}

CElementEmission*		CElementEmissionRuntime::GetEmissionPtr()
{CPUTime(CElementEmissionRuntime);
	return static_cast<CElementEmission*>(m_elementPtr);
}

VOID					CElementEmissionRuntime::OnStartElement()
{CPUTime(CElementEmissionRuntime);
	//BOOL bIsAlwaysShow	=	!this->IsEnabled(MeshKeyIndex_Parameter1);
	//if(bIsAlwaysShow)
	//	m_nCreateCount	=	this->m_nParticleCount;
	//else
	//	m_nCreateCount	=	0;
	//m_fLastFrameIndex	=	g_manager.GetAppTime();
	//m_fLastTrackInsert	=	g_manager.GetAppTime();
}
VOID					CElementEmissionRuntime::OnStopElement()
{CPUTime(CElementEmissionRuntime);
	//BOOL bIsAlwaysShow	=	!this->IsEnabled(MeshKeyIndex_Parameter1);
	//if(bIsAlwaysShow)
	//{
		//for(int i=0;i<m_nParticleCount;i++)
		//{
		//	g_manager.FreeParticle( m_localParticles[i] );
		//}
		//m_nParticleCount	=	0;
	//}
}

VOID					CElementEmissionRuntime::BeginAction()
{CPUTime(CElementEmissionRuntime);
	//BOOL bIsAlwaysShow	=	!this->IsEnabled(MeshKeyIndex_Parameter1);
	//if(bIsAlwaysShow)
	//	m_nCreateCount	=	this->m_nParticleCount;
	//else
	//	m_nCreateCount	=	0;
	//m_fLastFrameIndex	=	g_manager.GetAppTime();
	//m_fLastTrackInsert	=	g_manager.GetAppTime();
	////m_trackLight.Reset();
	////
	//CElementEmission*	emit	=	GetEmissionPtr();
	//if(emit->m_trackData.bTrackEnable)
	//{
	//	int bindCount	=	0;
	//	for(int i=0;i<MAX_EMISSION_BIND_POINTS;i++)
	//		if(emit->m_bindPoints[i] >= 0)
	//			bindCount++;
	//	//m_trackLight.Initialize(bindCount, emit->m_trackData.nTrackCount);
	//}
}

VOID					CElementEmissionRuntime::FrameMove(Mesh* basePtr, const matrix4x4& actionMat)
{CPUTime(CElementEmissionRuntime);
	if(!m_bRender || m_actionPtr->m_unitPtr->m_bIsPause)
		return;
	CElementEmission*	emit	=	GetEmissionPtr();
	//if(emit->m_nMaxParticle < 0)
	//	return;
	//int trackLength		=	emit->m_trackEndIndex - emit->m_trackBeginIndex;
	//if(!m_bRender || m_elementPtr->m_frameCount <= 0 || trackLength <= 0)
	//	return;
	//const matrix4x4* parent	=	0;
	//if(basePtr)
	//	parent	=	basePtr->GetBoneMatrix(m_elementPtr->m_bindPoint);
	//if(!parent)
	//	parent	=	&actionMat;
	//float fCreationTime	=	(float)trackLength / TRACK_SECOND_LENGTH * emit->m_fCreationTime;
	//float fElapsedTime	=	(g_manager.GetAppTime() - m_fLastFrameIndex) * (float)trackLength / (float)m_elementPtr->m_frameCount;
	//int numCreate		=	0;
	//if(fCreationTime < 0.01f)
	//	numCreate		=	emit->m_nMaxParticle;
	//else
	//	numCreate		=	(int)(fElapsedTime / fCreationTime * (float)emit->m_nMaxParticle);
	//if(numCreate > 0 && m_nCreateCount < emit->m_nMaxParticle)//比如至少创建一个才可以开始
	//{
	//	BOOL bIsAlwaysShow	=	!this->IsEnabled(MeshKeyIndex_Parameter1);
	//	if(bIsAlwaysShow)
	//	{
	//		if(m_nParticleCount >= emit->m_nMaxParticle)
	//			return;
	//	}
	//	m_fLastFrameIndex	=	g_manager.GetAppTime();
	//	matrix4x4 mat;
	//	for(int i=0;i<numCreate;i++)
	//	{
	//		if(/*m_nParticleCount*/ m_nCreateCount >= emit->m_nMaxParticle)
	//			break;
	//		float fFrameIndex	=	(float)(m_nCreateCount % emit->m_nMaxParticle) / (float)emit->m_nMaxParticle * (float)emit->m_frameCount;
	//		//
	//		float3 scaleBase;
	//		switch(emit->m_nEmissionType)
	//		{
	//		case EmissionType_Matrix://如果是跟随矩阵，则相对位置
	//			GetKeyMatrixEmission(mat, 0, fFrameIndex, &scaleBase);
	//			break;
	//		case EmissionType_Self:
	//			GetKeyMatrixEmission(mat, parent, fFrameIndex, &scaleBase);
	//			break;
	//		default:
	//			continue;
	//		}
	//		if(AddParticleOne(mat, basePtr, scaleBase, fFrameIndex))//创建
	//			m_nCreateCount++;
	//	}
	//}
}

VOID					CElementEmissionRuntime::GetKeyMatrixEmission(matrix4x4& ret, const matrix4x4* parent, float fFrameIndex, float3* scaleRet)
{CPUTime(CElementEmissionRuntime);
	//matrix4x4 mat, matS;
	//float3	offset(0, 0, 0);
	//if(IsEnabled(MeshKeyIndex_PositionX))offset.x	=	To3DValue(GetValueFrame(MeshKeyIndex_PositionX, fFrameIndex));
	//if(IsEnabled(MeshKeyIndex_PositionY))offset.y	=	To3DValue(GetValueFrame(MeshKeyIndex_PositionY, fFrameIndex));
	//if(IsEnabled(MeshKeyIndex_PositionZ))offset.z	=	To3DValue(GetValueFrame(MeshKeyIndex_PositionZ, fFrameIndex));
	////
	//float	offsetSelf	=	0;
	//if(IsEnabled(MeshKeyIndex_RotationOffset))offsetSelf	=	To3DValue(GetValueFrame(MeshKeyIndex_RotationOffset, fFrameIndex));
	////
	//float3	scale(1,1,1);
	//float	scaleFull	=	1;
	////
	//if(IsEnabled(MeshKeyIndex_ScaleX))scale.x	=	GetValueFrame(MeshKeyIndex_ScaleX, fFrameIndex);
	//if(IsEnabled(MeshKeyIndex_ScaleY))scale.y	=	GetValueFrame(MeshKeyIndex_ScaleY, fFrameIndex);
	//if(IsEnabled(MeshKeyIndex_ScaleZ))scale.z	=	GetValueFrame(MeshKeyIndex_ScaleZ, fFrameIndex);
	////
	//if(IsEnabled(MeshKeyIndex_ScaleFull))scaleFull	=	GetValueFrame(MeshKeyIndex_ScaleFull, fFrameIndex);
	////
	//scale	*=	scaleFull;
	//if(scaleRet)
	//	*scaleRet	=	scale;
	////
	//float	rotationX	=	0;
	//float	rotationZ	=	0;
	//if(IsEnabled(MeshKeyIndex_RotationX))rotationX	=	Radians(GetValueFrame(MeshKeyIndex_RotationX, fFrameIndex));
	//if(IsEnabled(MeshKeyIndex_RotationZ))rotationZ	=	Radians(GetValueFrame(MeshKeyIndex_RotationZ, fFrameIndex));
	////
	////matrixScaling(&matS, scale.x, scale.y, scale.z);
	//matS		=	matrix4x4::s_identity();
	//matS._42	=	offsetSelf;
	//matrixRotationX(&mat, rotationX);
	//matrixMultiply(&matS, &matS, &mat);
	//matrixRotationZ(&mat, rotationZ);
	//matrixMultiply(&ret, &matS, &mat);
	////
	//CElementEmission*	emit	=	GetEmissionPtr();
	//EmissionData& emin	=	emit->m_emissionMin;
	//EmissionData& emax	=	emit->m_emissionMax;
	////CRandom::setSeed(g_manager.GetAppTime());
	//offset.x	+=	RandomF(emin.range.x, emax.range.x);//(float)rand() / (float)RAND_MAX * (emax.range.x - emin.range.x) + emin.range.x;//
	//offset.y	+=	RandomF(emin.range.y, emax.range.y);//(float)rand() / (float)RAND_MAX * (emax.range.y - emin.range.y) + emin.range.y;//
	//offset.z	+=	RandomF(emin.range.z, emax.range.z);
	//ret.SetPosition(offset);
	////
	//if(parent)
	//	matrixMultiply(&ret, &ret, parent);
}

BOOL					CElementEmissionRuntime::AddParticleOne(matrix4x4& world, Mesh* basePtr, float3& scaleBase, float fSingleFrameIndex)
{CPUTime(CElementEmissionRuntime);
	//CElementEmission*	emit	=	GetEmissionPtr();
	//if(this->m_nCreateCount >= emit->m_nMaxParticle || this->m_nParticleCount >= MAX_EMISSION_PARTICLEs)
	//	return 0;
	//EmissionParticle* pParticle	=	g_manager.AllocParticle();
	//if(!pParticle)
	//	return 0;
	//memset_s(pParticle, 0, sizeof(EmissionParticle));
	////
	//EmissionTexture& tex	=	emit->m_textureData;
	//EmissionTrack& track	=	emit->m_trackData;
	//EmissionData& emin		=	emit->m_emissionMin;
	//EmissionData& emax		=	emit->m_emissionMax;
	////
	//if(tex.bTextureCreateIDRandom && tex.nTextureFrameCount > 0)
	//	pParticle->fImageBeginIndex	=	RandomF(0.0f, (float)tex.nTextureFrameCount - 0.1f);
	////
	//pParticle->worldBegin		=	world;
	////
	//pParticle->speed.setxyz(RandomF(emin.speed.x, emax.speed.x), RandomF(emin.speed.y, emax.speed.y), RandomF(emin.speed.z, emax.speed.z));
	//pParticle->scale.setxyz(RandomF(emin.scale.x, emax.scale.x), RandomF(emin.scale.y, emax.scale.y), RandomF(emin.scale.z, emax.scale.z));
	//pParticle->scale	*=	RandomF(emin.scaleFull, emax.scaleFull);
	//pParticle->color.setRGBA(RandomF(emin.color.r, emax.color.r), RandomF(emin.color.g, emax.color.g), RandomF(emin.color.b, emax.color.b), RandomF(emin.color.a, emax.color.a));
	//pParticle->scaleBegin	=	scaleBase * pParticle->scale;
	////
	//if(IsEnabled(MeshKeyIndex_ColorR))pParticle->color.r	+=	GetValueFrame(MeshKeyIndex_ColorR, fSingleFrameIndex);
	//if(IsEnabled(MeshKeyIndex_ColorG))pParticle->color.g	+=	GetValueFrame(MeshKeyIndex_ColorG, fSingleFrameIndex);
	//if(IsEnabled(MeshKeyIndex_ColorB))pParticle->color.b	+=	GetValueFrame(MeshKeyIndex_ColorB, fSingleFrameIndex);
	//if(IsEnabled(MeshKeyIndex_Alpha))pParticle->color.a	+=	GetValueFrame(MeshKeyIndex_Alpha, fSingleFrameIndex);
	////
	//pParticle->rotation.setxy(RandomF(emin.rotation.x, emax.rotation.x), RandomF(emin.rotation.y, emax.rotation.y));
	//pParticle->offsetSelf		=	RandomF(emin.offsetSelf, emax.offsetSelf);
	//pParticle->selfRotation		=	RandomF(emin.selfRotation, emax.selfRotation);
	//pParticle->fLifeTimeLength	=	RandomF(emin.life, emax.life) + this->GetValueFrame(MeshKeyIndex_Parameter1, fSingleFrameIndex);
	////
	//if(emit->m_nEmissionPartIndex >= 0 && emit->m_nEmissionType == EmissionType_Self)
	//{
	//	if(basePtr)
	//	{
	//		SubMesh* geom	=	basePtr->GetSubmeshPtr(emit->m_nEmissionPartIndex);
	//		const matrix4x4* geomMat	=	basePtr->GetSubmeshMatrix(emit->m_nEmissionPartIndex);
	//		if(geomMat && geom)
	//		{
	//			int faceID	=	RandomF(0.0f, (float)geom->GetDataPtr()->m_polygons->GetFaceCount() - .1f);
	//			float u		=	RandomF(0.0f, 1.0f);
	//			float v		=	RandomF(0.0f, 1.0f);
	//			//
	//			float3 pos;
	//			if(geom->GetDataPtr()->m_polygons->Is32BitMesh())
	//			{
	//				const Triangle32* t	=	geom->GetDataPtr()->m_polygons->GetFacePtr32(faceID);
	//				const float3& v1	=	geom->GetDataPtr()->m_polygons->GetVertexPtr(t->v1)->pos;
	//				const float3& v2	=	geom->GetDataPtr()->m_polygons->GetVertexPtr(t->v2)->pos;
	//				const float3& v3	=	geom->GetDataPtr()->m_polygons->GetVertexPtr(t->v3)->pos;
	//				//
	//				float3 v21 =	v2 - v1;
	//				float3 m	=	v21 * u + v1;
	//				float3 v31	=	m - v3;
	//				pos			=	v31 * v + v3;
	//			}
	//			else
	//			{
	//				const Triangle* t	=	geom->GetDataPtr()->m_polygons->GetFacePtr16(faceID);
	//				const float3& v1	=	geom->GetDataPtr()->m_polygons->GetVertexPtr(t->v1)->pos;
	//				const float3& v2	=	geom->GetDataPtr()->m_polygons->GetVertexPtr(t->v2)->pos;
	//				const float3& v3	=	geom->GetDataPtr()->m_polygons->GetVertexPtr(t->v3)->pos;
	//				//
	//				float3 v21 =	v2 - v1;
	//				float3 m	=	v21 * u + v1;
	//				float3 v31	=	m - v3;
	//				pos			=	v31 * v + v3;
	//			}
	//			pos.transform(pos, *geomMat);
	//			pParticle->worldBegin.SetPosition(pos);
	//		}
	//	}
	//}
	//pParticle->worldRender		=	pParticle->worldBegin;
	//this->m_localParticles[m_nParticleCount++]	=	pParticle;
	return true;
}

VOID					CElementEmissionRuntime::FreeParticle(int nIndex)
{CPUTime(CElementEmissionRuntime);
	//EmissionParticle* ptr	=	m_localParticles[nIndex];
	////
	//if(m_nParticleCount > 1)
	//	m_localParticles[nIndex]	=	m_localParticles[m_nParticleCount - 1];
	//m_nParticleCount	--;
	//if(m_nParticleCount < 0)m_nParticleCount = 0;
	//g_manager.FreeParticle( ptr );
}

//VOID					CElementEmissionRuntime::FrameMoveEmision(Mesh* basePtr, const matrix4x4& actionMat)
//{
	//if(m_nParticleCount <= 0)
	//	return;
	//CElementEmission*	emit	=	GetEmissionPtr();
	//int trackLength		=	emit->m_trackEndIndex - emit->m_trackBeginIndex;
	//if(emit->m_frameCount <= 0 || trackLength <= 0)
	//{
	//	for(int i=0;i<m_nParticleCount;i++)
	//	{
	//		g_manager.FreeParticle( m_localParticles[i] );
	//	}
	//	m_nParticleCount	=	0;
	//	return;
	//}
	//float fLerp	=	(float)m_elementPtr->m_frameCount / (float)trackLength * g_manager.GetElapsedTime();
	//const matrix4x4* parent	=	0;
	//if(basePtr)
	//	parent	=	basePtr->GetBoneMatrix(emit->m_bindPoint);
	//if(!parent)
	//{
	//	if(emit->m_nEmissionType == EmissionType_Matrix)
	//	{
	//		for(int i=0;i<m_nParticleCount;i++)
	//		{
	//			g_manager.FreeParticle( m_localParticles[i] );
	//		}
	//		m_nParticleCount	=	0;
	//		return;
	//	}
	//	parent	=	&actionMat;
	//}
	////
	////
	//matrix4x4 mat, matS;
	////
	//matrix4x4 tempMat;
	//BOOL bIsAlwaysShow	=	!this->IsEnabled(MeshKeyIndex_Parameter1);
	//for(int nIndex=0;nIndex<m_nParticleCount;nIndex++)
	//{
	//	EmissionParticle* pParticle	=	this->m_localParticles[nIndex];
	//	pParticle->fLifeTime		+=	fLerp;
	//	if(bIsAlwaysShow)
	//	{
	//		pParticle->fLifeTime	=	fmodf(fLerp, pParticle->fLifeTimeLength);
	//	}
	//	else if(pParticle->fLifeTime >= pParticle->fLifeTimeLength)//fFrameIndex >= 0)
	//	{
	//		g_manager.FreeParticle( pParticle );
	//		//
	//		m_localParticles[nIndex]	=	m_localParticles[m_nParticleCount - 1];
	//		m_nParticleCount--;
	//		if(m_nParticleCount <= 0)
	//		{
	//			m_nParticleCount = 0;
	//			break;
	//		}
	//		nIndex--;
	//		continue;
	//	}
	//	float fFrameIndex	=	pParticle->fLifeTime / pParticle->fLifeTimeLength * (float)emit->m_nParticleFrameCount;
	//	matrix4x4* worldParent	=	&pParticle->worldBegin;
	//	switch(emit->m_nEmissionType)
	//	{
	//	case EmissionType_Matrix://如果是跟随矩阵，则相对位置
	//		{
	//			matrixMultiply(&tempMat, &pParticle->worldBegin, parent);
	//			worldParent		=	&tempMat;
	//		}
	//		break;
	//	}
	//	//
	//	pParticle->planeOffset_r.setxyz(0, 0, 0);
	//	pParticle->offset_r.setxyz(0, 0, 0);
	//	pParticle->offsetSelf_r	=	0;
	//	pParticle->scale_r	=	pParticle->scaleBegin * this->m_actionPtr->m_unitPtr->m_baseScale * this->m_actionPtr->m_unitPtr->m_unitPtr->m_fBaseScale;//.setxyz(1,1,1);
	//	pParticle->rotation_r.setxy(0, 0);
	//	pParticle->color_r	=	pParticle->color;
	//	pParticle->selfRotation_r	=	0;
	//	//
	//	if(IsEnableEmit(emit, ParticleKey_PositionX))pParticle->offset_r.x	=	To3DValue(GetValue(emit, ParticleKey_PositionX, fFrameIndex)) * pParticle->speed.x;
	//	if(IsEnableEmit(emit, ParticleKey_PositionY))pParticle->offset_r.y	=	To3DValue(GetValue(emit, ParticleKey_PositionY, fFrameIndex)) * pParticle->speed.y;
	//	if(IsEnableEmit(emit, ParticleKey_PositionZ))pParticle->offset_r.z	=	To3DValue(GetValue(emit, ParticleKey_PositionZ, fFrameIndex)) * pParticle->speed.z;
	//	//
	//	if(IsEnableEmit(emit, ParticleKey_PositionSelf))pParticle->offsetSelf_r	=	To3DValue(GetValue(emit, ParticleKey_PositionSelf, fFrameIndex) * pParticle->offsetSelf);
	//	//
	//	if(IsEnableEmit(emit, ParticleKey_ScaleX))pParticle->scale_r.x		*=	GetValue(emit, ParticleKey_ScaleX, fFrameIndex);
	//	if(IsEnableEmit(emit, ParticleKey_ScaleY))pParticle->scale_r.y		*=	GetValue(emit, ParticleKey_ScaleY, fFrameIndex);
	//	if(IsEnableEmit(emit, ParticleKey_ScaleZ))pParticle->scale_r.z		*=	GetValue(emit, ParticleKey_ScaleZ, fFrameIndex);
	//	//
	//	float scaleFull	=	1.0f;
	//	if(IsEnableEmit(emit, ParticleKey_ScaleFull))scaleFull	=	GetValue(emit, ParticleKey_ScaleFull, fFrameIndex);
	//	//
	//	pParticle->scale_r	*=	scaleFull;
	//	//
	//	if(IsEnableEmit(emit, ParticleKey_RotationX))pParticle->rotation_r.x	=	Radians(GetValue(emit, ParticleKey_RotationX, fFrameIndex)) * pParticle->rotation.x;
	//	if(IsEnableEmit(emit, ParticleKey_RotationZ))pParticle->rotation_r.y	=	Radians(GetValue(emit, ParticleKey_RotationZ, fFrameIndex)) * pParticle->rotation.y;
	//	//
	//	if(IsEnableEmit(emit, ParticleKey_ColorR))pParticle->color_r.r		+=	GetValue(emit, ParticleKey_ColorR, fFrameIndex);
	//	if(IsEnableEmit(emit, ParticleKey_ColorG))pParticle->color_r.g		+=	GetValue(emit, ParticleKey_ColorG, fFrameIndex);
	//	if(IsEnableEmit(emit, ParticleKey_ColorB))pParticle->color_r.b		+=	GetValue(emit, ParticleKey_ColorB, fFrameIndex);
	//	if(IsEnableEmit(emit, ParticleKey_Alpha))pParticle->color_r.a		*=	GetValue(emit, ParticleKey_Alpha, fFrameIndex);
	//	//
	//	pParticle->bSelfRotation		=	IsEnableEmit(emit, ParticleKey_SelfRotation);
	//	if(pParticle->bSelfRotation)pParticle->selfRotation_r	=	Radians(GetValue(emit, ParticleKey_SelfRotation, fFrameIndex) * pParticle->selfRotation);
	//	//
	//	if(IsEnableEmit(emit, ParticleKey_PlaneOffsetX))pParticle->planeOffset_r.x		=	GetValue(emit, ParticleKey_PlaneOffsetX, fFrameIndex);
	//	if(IsEnableEmit(emit, ParticleKey_PlaneOffsetY))pParticle->planeOffset_r.y		=	GetValue(emit, ParticleKey_PlaneOffsetY, fFrameIndex);
	//	if(IsEnableEmit(emit, ParticleKey_PlaneOffsetZ))pParticle->planeOffset_r.z		=	GetValue(emit, ParticleKey_PlaneOffsetZ, fFrameIndex);
	//	//
	//	//matrixScaling(&matS, pParticle->scale_r.x, pParticle->scale_r.y, pParticle->scale_r.z);
	//	matS		=	matrix4x4::s_identity();
	//	matS._42	=	pParticle->offsetSelf_r;
	//	matrixRotationX(&mat, pParticle->rotation_r.x);
	//	matrixMultiply(&matS, &matS, &mat);
	//	matrixRotationZ(&mat, pParticle->rotation_r.y);
	//	matrixMultiply(&matS, &matS, &mat);
	//	matS.SetPosition(pParticle->offset_r);
	//	//
	//	matrixMultiply(&pParticle->worldRender, &matS, worldParent);
	//	if(emit->m_bUseSystemWind)
	//	{
	//		pParticle->windOffset	+=	g_manager->m_windParameter.dir * g_manager.GetElapsedTime() * 0.1f;
	//		pParticle->worldRender.SetPosition(pParticle->worldRender.GetPosition() + pParticle->windOffset);
	//	}
	//}
//}


};
