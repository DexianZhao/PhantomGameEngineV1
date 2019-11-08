//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIBase.h"
#include "PhantomUIControl.h"
#include "PhantomUIScrollBar.h"
#include "PhantomUIDialog.h"
#include "PhantomManager.h"
#include "PhantomUIManager.h"
#include "LuaManager.h"
#include "APILua.h"

namespace Phantom{

AnimationTrackPlay::AnimationTrackPlay(){
	m_bAnimationAutoPlay	=	PlayAnimationType_Play;
	m_fSleepTimeCur			=	0.0f;
	m_fWaveSleepTimeCur		=	0.0f;
	m_fFrame				=	0;
	m_bPlaying				=	false;
	this->m_fSpeed			=	TRACK_SECOND_LENGTH;
	m_bPlayingUV			=	false;
	m_controlPtr			=	0;
	m_bWavePlayed			=	false;
	m_bIsPlayOver			=	false;
	this->m_nLoopCount		=	0;
}
VOID	AnimationTrackPlay::Init(UIAnimationTrack& ut)
{CPUTime(AnimationTrackPlay);
	this->m_fSpeed = ut.m_fSpeed;
}
UIAnimationTrack::UIAnimationTrack()
{
	m_bIsDefault			=	false;
	m_status				=	TrackStatus_Stop;
	this->m_nFrameCount		=	0;
	SetTrackKeyCount(20);
	this->m_fSpeed			=	TRACK_SECOND_LENGTH;
	this->m_nID				=	0;
	this->m_soundVolume		=	1.0f;
	m_nSaveLoopCount		=	0;
	//m_soundFileIndex		=	-1;
	m_fSoundSpeed			=	1.0f;
	m_nElementIndex			=	0;		//元素索引
	m_bHideOnSleep		=	false;	//是否UV混合
	m_fSleepTime			=	0.0f;
	m_fWaveSleepTime		=	0.0f;
	m_bWaitTimeEnable		=	true;
	m_nMaxParticle			=	0;
	m_bHideOnOver			=	0;
	m_nNextAnimationID		=	ActionType_None;
	m_nActionType			=	ActionType_Idle;
	m_nUVAnimationID		=	ActionType_None;
}

UIAnimationTrack::~UIAnimationTrack()
{
	m_music.Stop();
}

VOID							UIAnimationTrack::CloneFrom(UIAnimationTrack* c)
{CPUTime(UIAnimationTrack);
	m_name				=	c->m_name;
	m_bIsDefault		=	c->m_bIsDefault;
	m_status			=	c->m_status;
	//m_fFrame			=	c->m_fFrame;
	this->m_soundVolume	=	c->m_soundVolume;
	this->m_fSoundSpeed	=	c->m_fSoundSpeed;
	m_nFrameCount		=	c->m_nFrameCount;
	m_bHideOnSleep	=	c->m_bHideOnSleep;
	m_nElementIndex		=	c->m_nElementIndex;
	m_fSleepTime		=	c->m_fSleepTime;
	m_fWaveSleepTime	=	c->m_fWaveSleepTime;
	m_bWaitTimeEnable	=	c->m_bWaitTimeEnable;
	m_nActionType		=	c->m_nActionType;
	m_nNextAnimationID		=	c->m_nNextAnimationID;
	m_nUVAnimationID	=	c->m_nUVAnimationID;
	m_nMaxParticle		=	c->m_nMaxParticle;
	this->m_bHideOnOver	=	c->m_bHideOnOver;
	SetTrackKeyCount(m_nFrameCount);
	m_fSpeed			=	c->m_fSpeed;
	for(int i=0;i<ControlTrackType_Count;i++)
		m_floats[i].CloneFrom(&c->m_floats[i]);
	SetWaveFile(c->m_soundFile);
}

const char*						UIAnimationTrack::GetName()
{CPUTime(UIAnimationTrack);
	return m_name.t;
}
VOID							UIAnimationTrack::SetName(const char* szName)
{CPUTime(UIAnimationTrack);
	m_name	=	szName;
}

VOID							UIAnimationTrack::SetKeyDefaultValue(ControlTrackType t, float f)
{CPUTime(UIAnimationTrack);
	m_floats[t].SetDefaultValue(f);
}

float							UIAnimationTrack::GetAnimationFloat(AnimationTrackPlay&data, ControlTrackType t)
{CPUTime(UIAnimationTrack);
	this->m_floats[t].SetReplay(data.m_nLoopCount<0);
	return this->m_floats[t].GetFrameValue(data.m_fFrame);
}
float							UIAnimationTrack::GetAnimationFloat(ControlTrackType t, float fTime)
{CPUTime(UIAnimationTrack);
	this->m_floats[t].SetReplay(this->m_nSaveLoopCount<0);
	return this->m_floats[t].GetFrameValue(fTime);
}
//
VOID							UIAnimationTrack::SetTrackKeyCount(INT nFrame)
{CPUTime(UIAnimationTrack);
	this->m_nFrameCount	=	nFrame;
	for(int i=0;i<ControlTrackType_Count;i++)
		this->m_floats[i].SetTrackKeyCount(nFrame);
}

float							UIAnimationTrack::GetPlaySpeed()
{CPUTime(UIAnimationTrack);
	return (m_fSpeed / TRACK_SECOND_LENGTH);
}

VOID							UIAnimationTrack::SetPlaySpeed(float f)
{CPUTime(UIAnimationTrack);
	m_fSpeed	=	f * TRACK_SECOND_LENGTH;
}

FloatAnimation*				UIAnimationTrack::GetAnimationPtr(ControlTrackType t)
{CPUTime(UIAnimationTrack);
	if(t < 0 || t > ControlTrackType_Count)
		return 0;
	return &this->m_floats[t];
}
VOID							UIAnimationTrack::SetStatus(TrackStatus s)
{CPUTime(UIAnimationTrack);
	this->m_status	=	s;
}	//设置当前状态

BOOL							UIAnimationTrack::OnFrameMove(AnimationTrackPlay&data, float fElapsedTime, float fBaseSpeed)
{CPUTime(UIAnimationTrack);
	float fE	=	fElapsedTime * fBaseSpeed * data.m_fSpeed;
	data.m_fSleepTimeCur		-=	fElapsedTime;//fE / TRACK_SECOND_LENGTH;
	data.m_fWaveSleepTimeCur	-=	fElapsedTime;//fE / TRACK_SECOND_LENGTH;
	if(data.m_fWaveSleepTimeCur <= 0.0f)
	{
		if(!data.m_bWavePlayed)
		{
			data.m_bWavePlayed	=	true;
			if(data.m_controlPtr->IsControlVisible())
			{
				m_music.Play(m_nSaveLoopCount, m_soundVolume, 0.0f, m_fSoundSpeed);
			}
		}
	}
	switch(data.m_bAnimationAutoPlay)
	{
	case PlayAnimationType_None:
		break;
	case PlayAnimationType_Play:
		{
			if(data.m_fSleepTimeCur <= 0.0f)
				SetCurFrame(data, GetCurFrame(data) + fE);
			else
				return false;
		}
		break;
	case PlayAnimationType_PlayBack:
		{
			if(data.m_fSleepTimeCur <= 0.0f)
				SetCurFrame(data, GetCurFrame(data) - fE);//fElapsedTime * this->m_fPlaySpeed * m_trackPtr->m_fSpeed);
			else
				return false;
		}
		break;
	}
	return true;
}

VOID							UIAnimationTrack::SetCurFrame(AnimationTrackPlay&data, float fTime)
{CPUTime(UIAnimationTrack);
	if(!data.m_bPlayingUV)
	{
		if(this->m_nUVAnimationID != ActionType_None)
		{
			if(m_nElementIndex >= 0 && m_nElementIndex < data.m_controlPtr->m_elementCount)
			{
				data.m_controlPtr->m_elementList[m_nElementIndex]->PlayAction(m_nUVAnimationID);
			}
		}
		data.m_bPlayingUV	=	true;
	}
	//
	data.m_fFrame	=	fTime;
	switch(data.m_bAnimationAutoPlay)
	{
	case PlayAnimationType_None:
		return;
	case PlayAnimationType_Play:
		{
			if(data.m_nLoopCount<0){
			}
			else if(data.m_fFrame > (float)m_nFrameCount)
				data.m_fFrame -= (float)m_nFrameCount;
			else
				return;
		}
		break;
	case PlayAnimationType_PlayBack:
		{
			if(data.m_fFrame < 0)
				data.m_fFrame	+=	(float)m_nFrameCount;
			else
				return;
		}
		break;
	}
	if(!data.m_bPlaying)
		return;
	if(data.m_nLoopCount > 1)
	{
		data.m_nLoopCount--;
		if(data.m_controlPtr->IsControlVisible())
		{
			if(m_music.IsLoaded())
			{
				m_music.Stop();
				m_music.Play(1, m_soundVolume, 0.0f, m_fSoundSpeed);
			}
		}
		data.m_controlPtr->m_dialogPtr->RaiseEvent(data.m_controlPtr, EventID_PlayOver, Pixel(this->m_nActionType,this->m_nID));
		data.m_bIsPlayOver	=	true;
	}
	else if(data.m_nLoopCount < 0)
	{
	}
	else
	{
		data.m_nLoopCount	=	0;
		switch(data.m_bAnimationAutoPlay)
		{
		case PlayAnimationType_None:
			return;
		case PlayAnimationType_Play:
			data.m_fFrame = (float)m_nFrameCount - 0.1f;
			break;
		case PlayAnimationType_PlayBack:
			data.m_fFrame = 0;
			break;
		}
		data.m_bAnimationAutoPlay	=	PlayAnimationType_None;
		data.m_bPlaying	=	false;
		data.m_controlPtr->m_dialogPtr->RaiseEvent(data.m_controlPtr, EventID_PlayOver, Pixel(this->m_nActionType,this->m_nID));
		data.m_bIsPlayOver	=	true;
		if(m_nNextAnimationID > ActionType_None)
		{
			data.m_controlPtr->PlayAction(m_nNextAnimationID);
		}
	}
#ifndef EDITOR_OS
	if(data.m_bIsPlayOver&&m_bHideOnOver)
		data.m_controlPtr->SetVisible(false);
#endif
}	//设置当前关键帧

VOID							UIAnimationTrack::SetWaveVolume(float f)
{CPUTime(UIAnimationTrack);
	m_soundVolume = clamp(f, 0, 1);
	if(m_music.IsPlaying())
		m_music.SetVolume(m_soundVolume);
}
VOID							UIAnimationTrack::SetWaveSpeed(float f)
{CPUTime(UIAnimationTrack);
	m_fSoundSpeed = clamp(f, 0.1f, 10.0f);
	if(m_music.IsPlaying())
		m_music.SetPlaySpeed(m_fSoundSpeed);
}
void							UIAnimationTrack::SetWaveFile(const char* szFile)
{CPUTime(UIAnimationTrack);
	m_music.Destroy();
	if(szFile)
		m_soundFile		=	szFile;
	if(m_soundFile.t[0])
	{
        m_soundFile.setIsFile();
		if(fexist(m_soundFile))
			m_music.Load(m_soundFile);
		//else
		//{
		//	FileName	path;
		//	Name file, ext;
		//	data.m_controlPtr->m_dialogPtr->m_strFile.splitpath(&path, 0, 0);
		//	m_soundFile.splitpath(0, &file, &ext);
		//	path	+=	file.t;
		//	path	+=	ext.t;
		//	if(fexist(path.t))
		//	{
		//		m_soundFile	=	path.t;
		//		m_music.Load(m_soundFile);
		//	}
		//}
	}
}

VOID							UIAnimationTrack::Stop(AnimationTrackPlay&data)
{CPUTime(UIAnimationTrack);
	//if(m_bPlaying)
	//	m_controlPtr->m_dialogPtr->RaiseEvent(m_controlPtr, EventID_PlayOver, m_nActionType);
	m_music.Stop();
	//if(m_soundFileIndex >= 0)
		//GetSoundManager()->Stop(m_soundFileIndex, &m_soundPlay);
	data.m_bPlaying		=	false;
	data.m_nLoopCount	=	m_nSaveLoopCount;
	data.m_bAnimationAutoPlay	=	PlayAnimationType_None;
}

VOID							UIAnimationTrack::Play(AnimationTrackPlay& data)
{CPUTime(UIAnimationTrack);
	data.Init(*this);
	if(!data.m_bPlaying)
	{
		//if(m_soundFileIndex >= 0)
		//{
			//GetSoundManager()->Stop(m_soundFileIndex, &m_soundPlay);
			//GetSoundManager()->Play(m_soundFileIndex, &m_soundPlay);
			//GetSoundManager()->SetPlaySpeed(m_soundFileIndex, &m_soundPlay, m_fSoundSpeed);
			//GetSoundManager()->SetVolume(m_soundFileIndex, &m_soundPlay, this->m_soundVolume);
		//}
	}
	//
	data.m_bPlaying		=	true;
	data.m_bPlayingUV	=	false;
	data.m_bWavePlayed	=	false;
	data.m_bIsPlayOver	=	false;
	data.m_bAnimationAutoPlay	=	PlayAnimationType_Play;
	data.m_nLoopCount	=	m_nSaveLoopCount;
	data.m_fSleepTimeCur		=	m_fSleepTime;
	data.m_fWaveSleepTimeCur	=	m_fWaveSleepTime + m_fSleepTime;
	switch(data.m_bAnimationAutoPlay)
	{
	case PlayAnimationType_None:
		data.m_fFrame	=	0;
		return;
	case PlayAnimationType_PlayBack:
		data.m_fFrame	=	(float)m_nFrameCount;
		break;
	case PlayAnimationType_Play:
		data.m_fFrame	=	0;
		break;
	}
}

VOID							UIAnimationTrack::SetLoopCount(INT nLoop)
{CPUTime(UIAnimationTrack);
	m_nSaveLoopCount	=	nLoop;
}
INT								UIAnimationTrack::GetLoopCount()
{CPUTime(UIAnimationTrack);
	return m_nSaveLoopCount;
}

float							UIAnimationTrack::GetCurFrame(AnimationTrackPlay&data)
{CPUTime(UIAnimationTrack);
	return data.m_fFrame;
}				//返回当前关键帧
//
VOID							UIAnimationTrack::Load(CSafeFileHelperR& r, UIControl* c)
{CPUTime(UIAnimationTrack);
	char version;
	r >> version >> m_bIsDefault >> m_nFrameCount >> m_fSpeed;
	r >> m_soundVolume;
	if(version<17)
	{
		FileName temp;
		r >> temp;
		m_soundFile = temp.t;
	}
	else
		r >> m_soundFile;
	r >> m_fSoundSpeed >> m_nSaveLoopCount;
	if(version>=18)
		r >> m_bHideOnOver;
	r >> m_fSleepTime;
	r >> m_nActionType;
	r >> m_fWaveSleepTime;
	r >> m_bWaitTimeEnable;
	r >> m_nNextAnimationID;
	r >> m_nMaxParticle;
	r >> this->m_nElementIndex >> this->m_bHideOnSleep;
	if(version < 17){
		NameAW temp;
		if(version >= 16)
		{
			r >>  temp;
		}
		else{
			CDynamicArray<short_t> s;
			r >> s;
			temp.setW(s.GetPtr());
		}
		m_name = temp.strA();
	}
	else
		r >> m_name;
	char count;
	r >> count;
	for(int i=0;i<count;i++)
		m_floats[i].Load(r);
	r >> m_nID;
	if(m_soundFile.t[0])
		SetWaveFile(0);
}

VOID							UIAnimationTrack::Save(CSafeFileHelperW& w)
{CPUTime(UIAnimationTrack);
	char version = 18;
	w << version << m_bIsDefault << m_nFrameCount << m_fSpeed;
	w << m_soundVolume << m_soundFile << m_fSoundSpeed << m_nSaveLoopCount;
	w << m_bHideOnOver;
	w << m_fSleepTime;
	w << m_nActionType;
	w << m_fWaveSleepTime;
	w << m_bWaitTimeEnable;
	w << m_nNextAnimationID;
	w << m_nMaxParticle;
	w << this->m_nElementIndex << this->m_bHideOnSleep;
	w << m_name;
	char count	=	ControlTrackType_Count;
	w << count;
	for(int i=0;i<count;i++)
		m_floats[i].Save(w);
	w << m_nID;
}

UIControl::UIControl( BOOL bInit, UIDialog *dialogPtr )
{
	m_from = 0;
	m_fromTime = 0;
	m_playdata.m_controlPtr	=	this;
	m_pChildDialog		=	0;
	m_scrollPtrV=m_scrollPtrH=0;
	m_nAlignMode		=	0;
	m_bIsCustomControl	=	false;
	m_bEnableUseImageText	=	false;
	m_bTrackSaved		=	false;
	m_nNextPlayAction	=	ActionType_Idle;
	m_bUseImageFont		=	false;
	m_loadRect.Set(0, 0, 0, 0);
	m_loadCenter.Set(0,0);
	m_loadRotation = 0;
	m_loadVersion		=	0;
	m_value				=	0;
	m_minValue			=	0;
	m_nPlayAction		=	ActionType_None;
	m_maxValue			=	100;
	m_bLoopMode			=	true;
	m_parentOffset.Set(0, 0);
	m_bIsGroupCenter	=	false;
	m_scrollBarWidth	= 16;
	m_fPlayTime			=	0.0f;
	this->m_bReadOnly	=	false;
	m_rotation		=	0;
	m_bEnableRotation	=	false;
	m_bCanFocus		=	true;
	m_bCustomRender	=	false;
	m_bSelect		=	true;
	m_nLevel		=	20;
	m_textColor		=	color4(0,0,0,1);
	m_textureColor	=	color4(1,1,1,1);
	m_bParentMove	=	0;
	m_controlType	= 0;
	m_dialogPtr 	= dialogPtr;
	m_bAutoSort		=	false;
	m_bIsGridObject	=	false;
	m_sGridOffsetY	=	0;
	m_controlID 	= 0;
	m_this.ptr		=	this;
	m_nLayerAdd		=	0;
	m_pUserData		= NULL;
	m_bIsEnabled 		= true;
	m_bVisible 		= true;
	m_fAngle		=	0;
	m_bEnableUnitAngle	=	0;
	m_unitAngle		=	Unit2DAngle_Bottom;
	m_bMouseEnter	= false;
	m_bHasFocus		= false;
	m_bIsDefault	= false;
	m_bMoveDown		=	false;
	m_textFormat	=	DrawTextAlign_VCENTER | DrawTextAlign_CENTER | DrawTextAlign_SINGLELINE;
	m_nShadowWidth	=	0;
	m_dialogPtr		= NULL;
	m_bVisibleOnScript	=	false;
	//
	m_fSleepTime	=	0;
	m_buttonType	=	BUTTON_TYPE_NORMAL;
	m_bDrawButtonFrame	=	false;
	m_bSetDefault		=	false;
	memset( &m_windowRect, 0, sizeof( m_windowRect ) );
	m_shadowColor	=	0xff000000;
	m_nProgressType		=	UIProgressType_HORZ;
	m_rotCenter.x		=	0;
	m_rotCenter.y		=	0;
	m_bIsBackground		=	false;
	//
	m_bHideOnEditor		=	false;
	m_bIsPickPixel		=	false;
	//
	m_renderCB				=	0;
	m_bTrackAutoBeginAni	=	TRUE;	//是否在对话框显示的时候自动开始计算动画
	m_bTrackPlayAni			=	0;
	m_bTrackPause			=	0;
	m_fTrackPlaySpeed		=	1.0f;	//轨迹动画播放速度
	m_nTabIndex				=	0;
	//
	m_fMax3DDist			=	To3DValue(100.0f);
	m_bind3DPos.setxyz(0,0,0);
	m_bind3DEnable			=	false;
	m_bind3DVisible			=	true;
	m_nPlayLoopCount		=	1;
	m_nTrackLoopCount		=	1;
	m_trackPtr				=	0;
	m_fPlaySpeed			=	1.0f;
	m_controlMat			=	m_controlMat.s_identity();
	//m_renderMat				=	m_controlMat.s_identity();
	m_saveWindowRect		=	m_windowRect;
	m_saveRotCenter			=	m_rotCenter;
	m_saveRotation			=	m_rotation;
	m_bIsInvalidate			=	false;
	m_nAnimationID			=	1;
	m_fSleepTimeCur			=	0;
	memset(m_elementList, 0, sizeof(m_elementList));
	m_elementCount			=	0;
	m_disableColor			=	color4(1.0f, 1.0f, 1.0f, 1.0f);
	m_scale.setxy(1,1);
	m_scalesave.setxy(1,1);
	memset(&m_currentAnim, 0, sizeof(m_currentAnim));
	m_currentAnim.textureColor.setRGBA(1,1,1,1);
	m_currentAnim.textColor.setRGBA(1,1,1,1);
	m_textPtr				=	0;
	m_numberSizeAdd			=	0;
	m_bMoveOffset			=	0;
	m_moveCurr				=	0;
	m_moveLength			=	0;
	m_moveSpeed				=	0;
	m_moveDir.setxy(0,0);
	m_movePos.setxy(0,0);
}

VOID			UIControl::BeginMove(Pixel off, float speed)//开始移动控制，播放完毕之后m_bMoveOffset自动设置成0
{
	m_moveDir.setxy((float)off.x - m_movePos.x, (float)off.y - m_movePos.y);
	m_moveSpeed = getmax(speed,0.1);
	m_bMoveOffset = true;
	m_moveCurr = 0;
	m_moveLength	=	m_moveDir.length();
	m_moveDir.normalize();
}

VOID			UIControl::PlayAction(const char* szName)
{CPUTime(UIControl);
	ActionType type = ActionType_None;
	if(!szName || stricmp(szName, "hide"))
		type = Phantom::ActionType_Hide;
	if(m_trackList.size() > 0)
	{
		for(int i=0;i<this->m_trackList.size();i++)
		{
			if(this->m_trackList[i]->m_name == szName)
			{
				type = m_trackList[i]->m_nActionType;
				break;
			}
		}
	}
	PlayAction(type);
}

VOID				UIControl::PlayAction(ActionType type)
{CPUTime(UIControl);
	if(type == Phantom::ActionType_Hide)
	{
		this->SetVisible(false);
		return;
	}
	//if(m_nPlayAction == type)
	//{
	//	if(m_nPlayAction > 0)
	//		this->PlayTrack();
	//	return;
	//}
	float savedAngle = this->m_fAngle;
	m_nPlayAction	=	type;
	BOOL bFind	=	false;
	if(m_trackList.size() > 0)
	{
		for(int i=0;i<this->m_trackList.size();i++)
		{
			if(this->m_trackList[i]->m_nActionType == type)
			{
				this->ActiveTrackByID(m_trackList[i]->m_nID);
				bFind	=	true;
				break;
			}
		}
	}
	if(!bFind)
	{
		this->ActiveTrackByID(-1);
		for(int i=0;i<this->GetElementCount();i++)
			this->GetElementPtr(i)->PlayAction(type);
		this->SetUnitAngle(savedAngle);
	}
}

void 				UIControl::SetID( int ID )
{CPUTime(UIControl);
	m_controlID	=	ID;
}

int					UIControl::GetElementCount()
{CPUTime(UIControl);
	return (int)this->m_elementCount;
}

UIElement*			UIControl::GetElementPtr(int index)
{CPUTime(UIControl);
	if(index < 0 || index >= m_elementCount)
		return 0;
	return m_elementList[index];
}
UIControl::~UIControl()
{
	if(m_innerMesh)
		m_innerMesh	=	0;
	if(m_pChildDialog)
		m_pChildDialog->m_pParentControl = 0;
	if(m_textPtr)
		g_manager.RemoveText(m_textPtr);
	m_textPtr = 0;
	for(int i=0;i<m_drawMgrs.size();i++)
	{
		safe_release(m_drawMgrs[i]);
	}
	for(int i=0;i<m_trackList.size();i++)
	{
		m_trackList[i]->Release();
	}
}

VOID			UIControl::RebuildRenderRect()
{CPUTime(UIControl);
	//this->m_dialogPtr->RebuildRenderRect();
}

char UIControl::IsFocus()
{CPUTime(UIControl);
    for(int i=0;i<MAX_TOUCH_COUNT;i++)
        if(m_dialogPtr->s_pControlFocus[i] == this)
            return true;
    return false;
}
void UIControl::SetTextColor( unsigned int c )
{CPUTime(UIControl);
	m_bSetDefault			=	false;
	color4 cc = c;
	SetTextColor(cc);
}

BOOL 	UIControl::isPtIn( Pixel pt, int element )
{CPUTime(UIControl);
	if(!m_bIsPickPixel)
    {
		return this->m_currentAnim.rc.isPtIn(pt.x, pt.y);
    }
	UIElement* ele	=	m_elementList[element];
	Texture* node	=	GetDlgManager()->GetTextureNode(ele->GetTextureNewID(), this->m_dialogPtr);
	if(!node)
		return m_currentAnim.rc.isPtIn(pt.x, pt.y);
	Rect rc	=	m_currentAnim.rc;//m_elementList[element]->rcTexture;
	Pixel center(m_rotCenter.x + rc.left, m_rotCenter.y + rc.top);
	rc.left = (int)((float)(rc.left - center.x) * m_scale.x) + center.x;
	rc.right = (int)((float)(rc.right - center.x) * m_scale.x) + center.x;
	rc.top = (int)((float)(rc.top - center.y) * m_scale.y) + center.y;
	rc.bottom = (int)((float)(rc.bottom - center.y) * m_scale.y) + center.y;
	if(rc.isPtIn(pt.x, pt.y))
	{
		//
		if(!node->GetImageBuffer().IsAlphaImage())
			return true;
		if(!node->GetImageBuffer().alphas)
			node->GenericAlphas();//构造透明
		if(ele->m_bRenderScale){
			float fx	=	(float)(pt.x - rc.left) * ((float)ele->m_imgSize.cx / (float)rc.GetWidth())  + ele->rcTexture.left - ele->m_imgOffset.x;
			float fy	=	(float)(pt.y - rc.top) * ((float)ele->m_imgSize.cy / (float)rc.GetHeight()) + ele->rcTexture.top - ele->m_imgOffset.y;
			if(ele->m_bIsInvWTo){
				fx = ele->rcTexture.left - ((float)(pt.x - rc.right) * ((float)ele->m_imgSize.cx / (float)rc.GetWidth())  + ele->m_imgOffset.x);
			}
			const Rect& tex = ele->rcTexture;
			if(!tex.isPtIn(Pixel(fx, fy)))
				return false;
			BOOL bPick	=	node->GetImageBuffer().IsAlpha((int)fx, (int)fy);
			return bPick;
		}
		else
		{
			float fx	=	(float)(pt.x - rc.left) + ele->rcTexture.left - ele->m_imgOffset.x;
			float fy	=	(float)(pt.y - rc.top) + ele->rcTexture.top - ele->m_imgOffset.y;
			//if(ele->m_bIsInvWTo){
			//	float fx	=	(float)(pt.x - rc.right) + ele->rcTexture.left - ele->m_imgOffset.x;
			//}
			const Rect& tex = ele->rcTexture;
			if(!tex.isPtIn(Pixel(fx, fy)))
				return false;
			BOOL bPick	=	node->GetImageBuffer().IsAlpha((int)fx, (int)fy);
			return bPick;
		}
	}
	return false;
}

BOOL UIControl::IsControlVisible()
{CPUTime(UIControl);
	if(!this->m_bVisible)
		return false;
	if(!this->m_dialogPtr->GetVisible())
		return false;
	UIGroup* g	=	this->m_dialogPtr->FindGroupPtr(m_groupID);
	if(g && !g->GetVisible())
		return false;
	return true;
}

void UIControl::SetSoundPlayID(UISoundPlay soundPlayType, const char* szFile)
{CPUTime(UIControl);
	if(soundPlayType < 0 || soundPlayType >= UISoundPlay_count)
		return;
	if(szFile)
		m_soundFile = szFile;
	else
		m_soundFile = "";
}

void UIControl::onMouseEnter()
{CPUTime(UIControl);
	if(m_bMouseEnter)
		return;
	m_bMouseEnter = true;
//	if(this->m_soundFile[UISoundPlay_mouseIn].t[0])//.strFile.empty())
//	{
//		LoadSnd(UISoundPlay_mouseIn);
//		m_soundPlay[UISoundPlay_mouseIn].Stop();
//		m_soundPlay[UISoundPlay_mouseIn].Play(1);
//	}
	//m_dialogPtr->RaiseEvent(this, EventID_UIMouseEnter);
}

void UIControl::onMouseLeave()
{CPUTime(UIControl);
	if(m_bMouseEnter)
	{
	}
	m_bMouseEnter = false;
	//m_dialogPtr->RaiseEvent(this, EventID_UIMouseLeave);
}
void UIControl::recalcRects()
{CPUTime(UIControl);
}

BOOL				UIControl::IsTrackAutoBegin()	//是否跟着对话框显示而自动播放
{CPUTime(UIControl);
	return m_bTrackAutoBeginAni;
}

VOID				UIControl::SetTrackAutoBegin(BOOL b)
{CPUTime(UIControl);
	m_playdata.m_bAnimationAutoPlay	=	b ? PlayAnimationType_Play : PlayAnimationType_None;
}

VOID				UIControl::PlayTrack()//播放
{CPUTime(UIControl);
	if(m_trackPtr)
	{
		if(m_trackPtr->m_bWaitTimeEnable)
		{
			m_fSleepTimeCur		=	m_fSleepTime;
			UIGroup* g			=	this->m_dialogPtr->FindGroupPtr(m_groupID);
			if(g)
				m_fSleepTimeCur	+=	static_cast<UIGroup*>(g)->m_fSleepTime;
		}
		m_trackPtr->Play(m_playdata);
	}
}

VOID				UIControl::StopTrack()
{CPUTime(UIControl);
	if(this->m_trackPtr)
		m_trackPtr->Stop(m_playdata);
}

BOOL				UIControl::IsPlayingTrack()
{CPUTime(UIControl);
	return m_playdata.m_bPlaying;
}

VOID				UIControl::PauseTrack(BOOL b)
{CPUTime(UIControl);
	m_playdata.m_bAnimationAutoPlay = b ? PlayAnimationType_None : PlayAnimationType_Play;
}

BOOL				UIControl::IsPauseTrack()
{CPUTime(UIControl);
	return (m_playdata.m_bAnimationAutoPlay == PlayAnimationType_None);
}

VOID				UIControl::SetTrackPlaySpeed(float f)	//设置轨迹播放速度
{CPUTime(UIControl);
	if(m_trackPtr)
		m_trackPtr->SetPlaySpeed(f);
}

float				UIControl::GetTrackPlaySpeed()
{CPUTime(UIControl);
	if(m_trackPtr)
		return m_trackPtr->m_fSpeed;
	return .0f;
}

VOID				AnimationLerp(ControlAnimation& r, const ControlAnimation& a1, const ControlAnimation& a2, float fLerp)
{CPUTime("global");
	a1.textColor.lerp(r.textColor, a2.textColor, fLerp);
	a1.textureColor.lerp(r.textureColor, a2.textureColor, fLerp);
	r.rc.left	=	(int)((float)(a2.rc.left - a1.rc.left) * fLerp + (float)a1.rc.left);
	r.rc.top	=	(int)((float)(a2.rc.top - a1.rc.top) * fLerp + (float)a1.rc.top);
	r.rc.right	=	(int)((float)(a2.rc.right - a1.rc.right) * fLerp + (float)a1.rc.right);
	r.rc.bottom	=	(int)((float)(a2.rc.bottom - a1.rc.bottom) * fLerp + (float)a1.rc.bottom);
	r.bRotateEnable	=	a1.bRotateEnable;
	r.rot			=	(a2.rot - a1.rot) * fLerp + a1.rot;
	r.center.x		=	(int)((float)(a2.center.x - a1.center.x) * fLerp + (float)a1.center.x);
	r.center.y		=	(int)((float)(a2.center.y - a1.center.y) * fLerp + (float)a1.center.y);
}

VOID				UIControl::SetWindowRect(const Rect& rc)
{CPUTime(UIControl);
	Rect r	=	rc;
	r.left		=	(int)((float)rc.left);
	r.top		=	(int)((float)rc.top);
	r.right	=	(int)((float)rc.right);
	r.bottom	=	(int)((float)rc.bottom);
	//
	r.Offset(-m_dialogPtr->m_x, -m_dialogPtr->m_y);
	UIControl* parent	=	this->GetParentControl();
	if(parent && parent != this)
	{
		Rect rcP	=	static_cast<UIControl*>(parent)->m_currentAnim.rc;
		r.Offset(-rcP.left, -rcP.top);
	}
	SetRenderRect(r);
}

VOID				UIControl::SetParentControlID(INT id)
{
	m_parentID.id = id;
	SetParentControl(m_dialogPtr->GetControl(m_parentID));
}
INT					UIControl::GetParentControlID()
{
	return m_parentID.id;
}

VOID				UIControl::SetParentControl(UIControl* c)
{CPUTime(UIControl);
	Rect r	=	m_windowRect;
	UIControl* parent	=	this->GetParentControl();
	if(parent && parent != this)
	{
		Rect rcP	=	static_cast<UIControl*>(parent)->m_currentAnim.rc;
		r.Offset(rcP.left, rcP.top);
	}
	m_parentID.id		=	0;
	m_parentID.index	=	0;
	if(c)
	{
		m_parentID.id		=	c->GetID();
		m_parentID.index	=	0;
	}
	parent	=	this->GetParentControl();
	if(parent && parent != this)
	{
		Rect rcP	=	static_cast<UIControl*>(parent)->m_currentAnim.rc;
		r.Offset(-rcP.left, -rcP.top);
	}
	SetRenderRect(r);
}

UIControl*			UIControl::GetParentControl()	//父物体设置
{CPUTime(UIControl);
	return m_dialogPtr->GetControl(m_parentID);
}

BOOL				g_bIsEditorMode	=	false;
BOOL	GetEditorMode()
{
	return g_bIsEditorMode;
}
VOID	SetEditorMode(BOOL b)
{
	g_bIsEditorMode	=	true;
}

BOOL				UIControl::LoadAllResource()	//载入所有需要的资源
{CPUTime(UIControl);
	for(int i=0;i<this->GetElementCount();i++)
	{
		UIElement* ele	=	static_cast<UIElement*>(this->GetElementPtr(i));
		if(ele->textureIndex > 0)
			GetDlgManager()->GetTextureNode(ele->textureIndex, this->m_dialogPtr);
		for(int k=0;k<ele->m_uvAnimations.size();k++)
		{
			if(ele->m_uvAnimations[k]->m_textureIndex > 0)
				GetDlgManager()->GetTextureNode(ele->m_uvAnimations[k]->m_textureIndex, this->m_dialogPtr);
		}
	}
	for(int i=0;i<this->m_trackList.size();i++)
	{
		if(m_trackList[i]->m_soundFile.t[0] && fexist(this->m_trackList[i]->m_soundFile))
		{
			m_trackList[i]->SetWaveFile(0);
		}
	}
	for(int i=0;i<UISoundPlay_count;i++)
		LoadSnd(i);
	return true;
}

VOID			UIControl::SetScale(float fx, float fy, const Pixel* pcenter)
{CPUTime(UIControl);
	m_scalesave.setxy(fx, fy);
	m_scale = m_scalesave;
	//Rect rctemp;
	//if(pcenter)
	//{
	//	rctemp.left = (int)((float)(m_saveWindowRect.left - pcenter->x) * fx) + pcenter->x;
	//	rctemp.right = (int)((float)(m_saveWindowRect.right - pcenter->x) * fx) + pcenter->x;
	//	rctemp.top = (int)((float)(m_saveWindowRect.top - pcenter->y) * fy) + pcenter->y;
	//	rctemp.bottom = (int)((float)(m_saveWindowRect.bottom - pcenter->y) * fy) + pcenter->y;
	//}
	//else
	//{
	//	Pixel center(m_rotCenter.x + m_saveWindowRect.left, m_rotCenter.y + m_saveWindowRect.top);
	//	rctemp.left = (int)((float)(m_saveWindowRect.left - center.x) * fx) + center.x;
	//	rctemp.right = (int)((float)(m_saveWindowRect.right - center.x) * fx) + center.x;
	//	rctemp.top = (int)((float)(m_saveWindowRect.top - center.y) * fy) + center.y;
	//	rctemp.bottom = (int)((float)(m_saveWindowRect.bottom - center.y) * fy) + center.y;
	//}
	//Rect& r = rctemp;
	//Rect rc = this->GetRenderRect();
	//int offsetX	=	r.left - rc.left;
	//int offsetY =	r.top - rc.top;
	//int sizeX	=	r.GetWidth() - rc.GetWidth();
	//int sizeY	=	r.GetHeight() - rc.GetHeight();
	//Rect& rcAni	=	this->m_currentAnim.rc;
	//rcAni.left		+=	offsetX;
	//rcAni.top		+=	offsetY;
	//rcAni.right		+=	offsetX + sizeX;
	//rcAni.bottom	+=	offsetY + sizeY;
	////
	//m_bSetDefault			=	false;
	//this->m_windowRect	=	r;
	//m_bChanged	=	true;
}

void				UIControl::SetRenderRect(const Rect& r)
{CPUTime(UIControl);
	Rect rc = this->GetRenderRect();
	int offsetX	=	r.left - rc.left;
	int offsetY =	r.top - rc.top;
	int sizeX	=	r.GetWidth() - rc.GetWidth();
	int sizeY	=	r.GetHeight() - rc.GetHeight();
	Rect& rcAni	=	this->m_currentAnim.rc;
	rcAni.left		+=	offsetX;
	rcAni.top		+=	offsetY;
	rcAni.right		+=	offsetX + sizeX;
	rcAni.bottom	+=	offsetY + sizeY;
	//
	m_bSetDefault			=	false;
	Rect rcOffset(this->m_saveWindowRect.left - m_windowRect.left, m_saveWindowRect.top - m_windowRect.top, m_saveWindowRect.right  - m_windowRect.right, m_saveWindowRect.bottom - m_windowRect.bottom);
	this->m_windowRect	=	r;
	if(this->m_trackPtr)
		m_saveWindowRect.Set(m_windowRect.left + rcOffset.left, m_windowRect.top + rcOffset.top, m_windowRect.right + rcOffset.right, m_windowRect.bottom + rcOffset.bottom);
	else
	{
		m_saveWindowRect	=	m_windowRect;
	}
	m_bChanged	=	true;
	if(m_bIsGridObject)
		this->m_dialogPtr->m_bRequestResort=true;
}

VOID				UIControl::Bind3DPos(const float3& pos)
{CPUTime(UIControl);
	this->m_bind3DEnable=true;
	this->m_bind3DPos = pos;
	this->m_bind3DObj.id = 0;
}
VOID				UIControl::Bind3DObj(const GameID& id, float fHeightAdd)
{CPUTime(UIControl);
	this->m_bind3DEnable=(id.id>0);
	this->m_bind3DPos.z = fHeightAdd;
	this->m_bind3DObj = id;
}
VOID				UIControl::Bind3DClear()
{CPUTime(UIControl);
	m_bind3DEnable=false;
	m_bind3DObj.id = 0;
}

VOID				UIControl::FrameMove(float fElapsedTime, const Rect* newRect, BOOL bChanged)
{CPUTime(UIControl);
	if(m_innerMesh)
		m_innerMesh->FrameMove();
	if(newRect)
		bChanged = true;
	if(!m_bChanged&&!m_bind3DEnable)
	{
		if(this->m_bIsBackground && !m_trackPtr)
			return;
	}
	if(m_bind3DEnable){
		m_bind3DVisible	=	true;
		float3 pos;
		if(m_bind3DObj.id>0){
			GameObject* obj = GetGameMgr()->Search(m_bind3DObj);
			if(!obj){
				m_bind3DEnable = false;
			}
			else{
				pos = obj->GetPosition();
				pos.z += m_bind3DPos.z;
			}
		}
		else{
			pos = this->m_bind3DPos;
		}
		if(m_bind3DEnable){
			Pixel pixel;
			if(GetCameraPtr()->Get2DFrom3D(pixel, pos)){
				int w = this->m_saveWindowRect.GetWidth();
				int h = this->m_saveWindowRect.GetHeight();
				m_saveWindowRect.left = pixel.x - this->m_saveRotCenter.x;
				m_saveWindowRect.top = pixel.y - this->m_saveRotCenter.y;
				m_saveWindowRect.right=m_saveWindowRect.left+w;
				m_saveWindowRect.bottom=m_saveWindowRect.top+h;
				this->m_currentAnim.rc = m_windowRect = m_saveWindowRect;
			}
			else
				m_bind3DVisible = false;
		}
	}
	BOOL	bRecalc	=	false;
	Pixel offset(0, 0);
	int layerAdd	=	0;
	for(int i=0;i<this->m_elementCount;i++)
	{
		this->m_elementList[i]->OnFrameMove(fElapsedTime, offset, layerAdd);
	}
	this->m_nLayerAdd	=	layerAdd;
	UIScrollBar* scroll	=	static_cast<UIScrollBar*>(GetScrollBar());
	if(scroll != this && scroll)
		scroll->FrameMove(fElapsedTime, newRect, bChanged);
	if(m_trackPtr && IsUseTrack())
	{
		CPUTimeN(UIControl, 1);
		float2 temps(1,1);
		//m_scale.setxy(1,1);
		char bSleep	=	false;
		m_fSleepTimeCur	-=	fElapsedTime;
		if(m_fSleepTimeCur <= 0.0f)
		{
			if(!m_trackPtr->OnFrameMove(m_playdata, fElapsedTime, this->m_fPlaySpeed))
				bSleep	=	true;
		}
		else
			bSleep	=	true;
		if(!m_trackPtr)return;
		//
		color4 text(1,1,1,1);
		color4 texture = this->m_textureColor;
		char bColor = false;
		char bAlpha = false;
		if(m_trackPtr->GetAnimationPtr(ControlTrackType_TextureColorR)->IsEnabled())
		{
			texture.r = m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_TextureColorR);
			texture.g = m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_TextureColorG);
			texture.b = m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_TextureColorB);
			bColor = true;
		}
		if(m_trackPtr->GetAnimationPtr(ControlTrackType_TextureColorA)->IsEnabled())
		{
			texture.a = m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_TextureColorA);
			bAlpha = true;
		}
		text = texture;
		//
		if(bSleep && m_trackPtr->m_bHideOnSleep)
		{
			text.a		=	0.0f;
			texture.a	=	0.0f;
		}
		m_currentAnim.center		=	this->m_saveRotCenter;//(PPixel&)PPixel(m_trackPtr->GetAnimationFloat(ControlTrackType_CenterX), m_trackPtr->GetAnimationFloat(ControlTrackType_CenterY));
		//
		if(m_trackPtr->GetAnimationPtr(ControlTrackType_CenterX)->IsEnabled())
		{
			m_currentAnim.center.x	+=	m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_CenterX);
			m_currentAnim.center.y	+=	m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_CenterY);
		}
		Rect	rc	=	m_saveWindowRect;
		if(newRect)
			rc = *newRect;
		if(m_trackPtr->GetAnimationPtr(ControlTrackType_RectX)->IsEnabled())
		{
			int offsetX	=	m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_RectX);
			rc.left		+=	offsetX;
			rc.right	+=	offsetX;
		}
		if(m_trackPtr->GetAnimationPtr(ControlTrackType_RectY)->IsEnabled())
		{
			int offsetY	=	m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_RectY);
			rc.top		+=	offsetY;
			rc.bottom	+=	offsetY;
		}
		if(m_trackPtr->GetAnimationPtr(ControlTrackType_RectW)->IsEnabled())
		{
			int w = m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_RectW);
			int width = rc.GetWidth();
			float fScaleV = (float)w / (float)width*0.5f;
			temps.x = 1 + fScaleV;
			rc.left -= (int)(fScaleV * (float)m_currentAnim.center.x);
			rc.right +=	(int)(fScaleV * (float)(width - m_currentAnim.center.x));
		}
		if(m_trackPtr->GetAnimationPtr(ControlTrackType_RectH)->IsEnabled())
		{
			int h = m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_RectH);
			int height = rc.GetHeight();
			float fScaleV = (float)h / (float)height*0.5f;
			temps.y = 1 + fScaleV;
			rc.top -= (int)(fScaleV * (float)m_currentAnim.center.y);
			rc.bottom +=	(int)(fScaleV * (float)(height - m_currentAnim.center.y));
		}
		//
		m_scale = m_scalesave * temps;
		m_currentAnim.rot			=	this->m_saveRotation;
		if(m_trackPtr->GetAnimationPtr(ControlTrackType_Rotation)->IsEnabled())
		{
			m_currentAnim.bRotateEnable = true;
			m_currentAnim.rot		+=	m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_Rotation);
		}
		else
		{
			m_currentAnim.bRotateEnable	=	this->m_bEnableRotation!=0;
			m_currentAnim.rot		=	m_rotation;
		}
		//
		m_rectDefault				=	rc;
		m_currentAnim.textColor		=	text;
		m_currentAnim.textureColor	=	texture;
		//
		m_windowRect				=	rc;
		m_rotCenter					=	m_currentAnim.center;
		m_rotation					=	m_currentAnim.rot;
		//
		if(m_trackPtr->m_music.IsPlaying())
		{
			if(m_trackPtr->GetAnimationPtr(ControlTrackType_SoundVolume)->IsEnabled())
				m_trackPtr->m_music.SetVolume(m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_SoundVolume) * m_trackPtr->m_soundVolume);
			if(m_trackPtr->GetAnimationPtr(ControlTrackType_SoundSpeed)->IsEnabled())
				m_trackPtr->m_music.SetPlaySpeed(m_trackPtr->GetAnimationFloat(m_playdata, ControlTrackType_SoundSpeed) * m_trackPtr->m_fSoundSpeed);
		}
		//
		this->OnAnimationRect();
		m_bSetDefault	=	false;
		bChanged		=	true;
		if(m_playdata.m_bIsPlayOver)
			this->ActiveTrackByID(-1);
	}
	else if(!m_bSetDefault)
	{
		m_bSetDefault	=	true;
		bChanged		=	true;
		m_currentAnim.bRotateEnable	=	this->m_bEnableRotation!=0;
		m_currentAnim.center		=	this->m_rotCenter;
		m_rectDefault				=	this->m_windowRect;
		m_currentAnim.rot			=	this->m_rotation;
		m_currentAnim.textColor		=	this->m_textColor;
		m_currentAnim.textureColor	=	this->m_textureColor;
	}
	m_currentAnim.rc	=	m_rectDefault;
	m_currentAnim.rc.Offset(offset.x, offset.y);
	float2 offpos = m_movePos;
	if(m_bMoveOffset){
		m_moveCurr += fElapsedTime*this->m_moveSpeed;
		if(m_moveCurr > m_moveLength)
		{
			m_moveCurr = m_moveLength;
			m_bMoveOffset = 0;
			m_movePos = m_moveDir*m_moveCurr + m_movePos;
			offpos = m_movePos;
		}
		else
			offpos = m_moveDir*m_moveCurr + m_movePos;
	}
	m_currentAnim.rc.Offset(offpos.x,offpos.y);
	if(bChanged)
		this->OnChangeMatrix();
}
void				UIControl::SetRotation(char bEnable, float rot)
{CPUTime(UIControl);
	m_bEnableRotation = bEnable;
	m_rotation		=	rot;
	m_bSetDefault	=	false;
}
VOID				UIControl::SetNextPlayAction(ActionType type)
{CPUTime(UIControl);
	m_nNextPlayAction = type;
	for(int i=0;i<this->m_elementCount;i++)
		if(this->m_elementList[i]->m_activeUVAnimation)
			this->m_elementList[i]->m_activeUVAnimation->SetNextAction(type);
}

BOOL				UIControl::IsClipIn()	//是否包含进去了
{CPUTime(UIControl);
	if(m_currentAnim.bRotateEnable)
	{
		return true;
	}
	else
	{
		return g_manager.IsClipRender(m_currentAnim.rc);
		//int w	=	g_manager.GetUIBufferWidth();
		//int h	=	g_manager.GetUIBufferHeight();
		//if(m_currentAnim.rc.left > rc.right || m_currentAnim.rc.top > rc.bottom || m_currentAnim.rc.right < rc.left || m_currentAnim.rc.bottom < rc.top)
		//	return false;
	}
	return true;
}

UIDialog*			UIControl::GetDialogPtr()
{CPUTime(UIControl);
	return m_dialogPtr;
}

UIAnimationTrack*			UIControl::CloneTrack(UIAnimationTrack* c)
{CPUTime(UIControl);
	UIAnimationTrack* d	=	new UIAnimationTrack();
	this->m_trackList.push_back( d );
	d->CloneFrom(static_cast<UIAnimationTrack*>(c));
	return d;
}

UIAnimationTrack*	UIControl::AddTrack(const char* szName)
{CPUTime(UIControl);
	UIAnimationTrack* d	=	new UIAnimationTrack();
	d->m_nID	=	m_nAnimationID++;
	d->m_name	=	"action";
	this->m_trackList.push_back( d );
	//
	d->SetKeyDefaultValue(ControlTrackType_TextColorR, this->m_textColor.r);
	d->SetKeyDefaultValue(ControlTrackType_TextColorG, this->m_textColor.g);
	d->SetKeyDefaultValue(ControlTrackType_TextColorB, this->m_textColor.b);
	d->SetKeyDefaultValue(ControlTrackType_TextColorA, this->m_textColor.a);
	//
	d->SetKeyDefaultValue(ControlTrackType_TextureColorR, this->m_textureColor.r);
	d->SetKeyDefaultValue(ControlTrackType_TextureColorG, this->m_textureColor.g);
	d->SetKeyDefaultValue(ControlTrackType_TextureColorB, this->m_textureColor.b);
	d->SetKeyDefaultValue(ControlTrackType_TextureColorA, this->m_textureColor.a);
	//
	d->SetKeyDefaultValue(ControlTrackType_RectX, 0);
	d->SetKeyDefaultValue(ControlTrackType_RectY, 0);
	d->SetKeyDefaultValue(ControlTrackType_RectW, 0);
	d->SetKeyDefaultValue(ControlTrackType_RectH, 0);
	//
	d->SetKeyDefaultValue(ControlTrackType_CenterX, 0);
	d->SetKeyDefaultValue(ControlTrackType_CenterY, 0);
	//
	d->SetKeyDefaultValue(ControlTrackType_Rotation, 0);
	//
	d->SetKeyDefaultValue(ControlTrackType_SoundVolume, 1.0f);
	d->SetKeyDefaultValue(ControlTrackType_SoundSpeed, 1.0f);
	d->SetKeyDefaultValue(ControlTrackType_UStart, 0);
	d->SetKeyDefaultValue(ControlTrackType_UEnd, 1);
	d->SetKeyDefaultValue(ControlTrackType_VStart, 0);
	d->SetKeyDefaultValue(ControlTrackType_VEnd, 1.0f);
	//
	for(int i=0;i<ControlTrackType_Count;i++)
		d->m_floats[i].SetEnabled(false);
	return d;
}

VOID					UIControl::RemoveTrack(UIAnimationTrack* ani)
{CPUTime(UIControl);
	for(int i=0;i<m_trackList.size();i++)
	{
		if(m_trackList[i] == ani)
		{
			if(ani == m_trackPtr)
			{
				StopTrack();
				m_trackPtr	=	0;
				OnTrackActiveChange(FALSE);
			}
			OnEraseAnimation(m_trackList[i]);
			delete m_trackList[i];
			m_trackList.eraseByIndex(i);
			break;
		}
	}
}

VOID					UIControl::OnTrackActiveChange(BOOL b)
{CPUTime(UIControl);
	if(this->m_bTrackSaved == b)
		return;
	m_bTrackSaved	=	b;
	if(m_trackPtr)
	{
		m_saveWindowRect			=	m_windowRect;
		m_saveRotCenter				=	m_rotCenter;
		m_saveRotation				=	m_rotation;
	}
	else
	{
		m_windowRect				=	m_saveWindowRect;//m_currentAnim.rc;
		m_rotCenter					=	m_saveRotCenter;//m_currentAnim.center;
		m_rotation					=	m_saveRotation;//m_currentAnim.rot;
	}
}

VOID					UIControl::ActiveTrackByID(INT nID)
{CPUTime(UIControl);
	if(m_trackList.size() == 0)
		return;
	StopTrack();
	BOOL bFind	=	false;
	for(int i=0;i<m_trackList.size();i++)
	{
		if(m_trackList[i]->m_nID == nID)
		{
			bFind	=	true;
			this->m_trackPtr = m_trackList[i];
			this->PlayTrack();
			OnTrackActiveChange(TRUE);
			m_nPlayAction	=	m_trackPtr->GetActionType();
			for(int i=0;i<this->GetElementCount();i++)
			{
				this->GetElementPtr(i)->PlayAction(m_trackPtr->GetActionType());
			}
			OnPlayStart();
			break;
		}
	}
	if(!bFind)
	{
		m_nPlayAction	=	ActionType_None;
		m_trackPtr	=	0;
		OnTrackActiveChange(FALSE);
		this->OnAnimationRect();
	}
}

VOID					UIControl::ActiveTrackByName(const char* szName)
{CPUTime(UIControl);
	if(m_trackList.size() == 0)
		return;
	StopTrack();
	BOOL bFind	=	false;
	if(szName)
	{
		for(int i=0;i<m_trackList.size();i++)
		{
			if(m_trackList[i]->m_name == szName)
			{
				bFind	=	true;
				this->m_trackPtr = m_trackList[i];
				this->PlayTrack();
				OnTrackActiveChange(TRUE);
				m_nPlayAction	=	m_trackPtr->GetActionType();
				for(int i=0;i<this->GetElementCount();i++)
				{
					this->GetElementPtr(i)->PlayAction(m_trackPtr->GetActionType());
				}
				OnPlayStart();
				break;
			}
		}
	}
	if(!bFind)
	{
		m_trackPtr	=	0;
		OnTrackActiveChange(FALSE);
		this->OnAnimationRect();
	}
}

UIAnimationTrack*	UIControl::GetTrackPtr(INT index)
{CPUTime(UIControl);
	if(index < 0 || index >= m_trackList.size())
		return 0;
	return m_trackList[index];
}

UIAnimationTrack*	UIControl::GetTrackPtrByName(const char* szName)
{CPUTime(UIControl);
	for(int i=0;i<m_trackList.size();i++)
	{
		if(m_trackList[i]->m_name == szName)
			return m_trackList[i];
	}
	return 0;
}

INT						UIControl::GetTrackCount()
{CPUTime(UIControl);
	return m_trackList.size();
}

VOID					UIControl::SetTrackDefaultID(int id)
{
	for(int i=0;i<m_trackList.size();i++)
	{
		if(m_trackList[i]->m_nID == id)
		{
			m_trackList[i]->m_bIsDefault = true;
		}
		else
			m_trackList[i]->m_bIsDefault = false;
	}
}
int						UIControl::GetTrackDefaultID()
{
	for(int i=0;i<m_trackList.size();i++)
	{
		if(m_trackList[i]->GetActionType() == ActionType_Idle)
			return m_trackList[i]->m_nID;
	}
	return -1;
}

VOID					UIControl::SetTrackDefault(UIAnimationTrack* def)
{CPUTime(UIControl);
	for(int i=0;i<m_trackList.size();i++)
	{
		if(m_trackList[i] == def)
		{
			m_trackList[i]->m_bIsDefault = true;
		}
		else
			m_trackList[i]->m_bIsDefault = false;
	}
}

UIAnimationTrack*	UIControl::GetTrackDefault()
{CPUTime(UIControl);
	for(int i=0;i<m_trackList.size();i++)
	{
		if(m_trackList[i]->GetActionType() == ActionType_Idle)
			return m_trackList[i];
	}
	return 0;
}
	//返回默认的动画指针
UIAnimationTrack*	UIControl::GetActiveTrack()
{CPUTime(UIControl);
	return m_trackPtr;
}

UIGroup*			UIControl::GetGroupPtr()
{CPUTime(UIControl);
	return m_dialogPtr->FindGroupPtr(m_groupID);
}
VOID				UIControl::SetGroupPtr(UIGroup* g)
{CPUTime(UIControl);
	SetGroupID(g ? g->GetID() : 0);
}

VOID				UIControl::SetGroupID(INT nGroupID)
{CPUTime(UIControl);
	UIGroup* oldG	=	m_groupID.id >= 0 ? m_dialogPtr->FindGroupPtr(m_groupID) : 0;
	m_groupID.id	=	nGroupID;
	UIGroup* newG	=	this->m_dialogPtr->FindGroupPtr(m_groupID);
	if(oldG && oldG != newG)
		static_cast<UIGroup*>(oldG)->RemoveControl(this);
	if(newG && oldG != newG)
		static_cast<UIGroup*>(newG)->AddControl(this);
	this->SetUnitAngle(GetGroupPtr()->GetUnitAngle());
	this->PlayAction(GetGroupPtr()->GetPlayAction());
}

VOID				UIControl::LoadSnd(INT nIndex)
{CPUTime(UIControl);
	if(!m_soundFile.t[0])
		return;
	if(m_soundPlay.IsLoaded())
		return;
	if(fexist(m_soundFile.t))
    {
		//m_soundPlay[nIndex].Load(m_soundFile[nIndex].t);
    }
	else
	{
		FileName path;
		Name file, ext;
		m_dialogPtr->m_strFile.splitpath(&path, 0, 0);
        Name& f = m_soundFile;
		f.splitpath(0, &file, &ext);
		//
		path	+=	file.t;
		path	+=	ext.t;
		if(file.t[0] && ext.t[0] && fexist(path.t))
		{
			m_soundFile	=	path.t;
            m_soundFile.setIsFile();
			//m_soundPlay[nIndex].Load(m_soundFile[nIndex].t);//.strFile.c_str());
		}
	}
}

VOID				UIControl::CopyFrom(UIControl* c)
{CPUTime(UIControl);
	UIControl* ctrl	=	static_cast<UIControl*>(c);
	//
	ControlData*	from	=	static_cast<ControlData*>(ctrl);
	ControlData*	to		=	static_cast<ControlData*>(this);
	int save_m_controlID	=	to->m_controlID;
	PtrID save_m_groupID		=	to->m_groupID;
	PtrID save_m_parentID		=	to->m_parentID;
	int save_m_nAnimationID	=	to->m_nAnimationID;
	*to	=	*from;
	to->m_controlID			=	save_m_controlID;
	to->m_groupID			=	save_m_groupID;
	to->m_parentID			=	save_m_parentID;
	to->m_nAnimationID		=	save_m_nAnimationID;
	//
	m_bHideOnEditor		=	ctrl->m_bHideOnEditor;
	m_bIsGroupCenter	=	ctrl->m_bIsGroupCenter;
	m_bMoveOffset		=	ctrl->m_bMoveOffset;
	m_moveSpeed			=	ctrl->m_moveSpeed;
	m_moveCurr			=	ctrl->m_moveCurr;
	m_moveLength		=	ctrl->m_moveLength;
	m_moveDir			=	ctrl->m_moveDir;
	m_movePos			=	ctrl->m_movePos;
	this->m_bIsGridObject	=	ctrl->m_bIsGridObject;
	m_sGridOffsetY		=	ctrl->m_sGridOffsetY;
	//
	this->m_nAlignMode = ctrl->m_nAlignMode;
	m_bChanged			=	true;
	m_parentID			=	ctrl->m_parentID;
	m_textColor			=	ctrl->m_textColor;
	m_textureColor		=	ctrl->m_textureColor;
	m_textFormat		=	ctrl->m_textFormat;
	m_windowRect		=	ctrl->m_windowRect;
	m_nProgressType		=	ctrl->m_nProgressType;
	//
	m_bVisible			=	ctrl->m_bVisible;
	m_bParentMove		=	ctrl->m_bParentMove;
	m_nShadowWidth		=	ctrl->m_nShadowWidth;
	m_nLevel			=	ctrl->m_nLevel;
	m_bIsBackground		=	ctrl->m_bIsBackground;
	this->m_disableColor	=	ctrl->m_disableColor;
	m_shadowColor		=	ctrl->m_shadowColor;
//	m_toolTipText		=	ctrl->m_toolTipText;//m_szToolTip.c_str();
	this->SetGroupID(ctrl->m_groupID.id);
	m_name				=	ctrl->m_name;
	m_rotation			=	ctrl->m_rotation;
	m_rotCenter			=	ctrl->m_rotCenter;
	m_numberSizeAdd		=	ctrl->m_numberSizeAdd;
	m_loadRect			=	ctrl->m_loadRect;
	m_loadCenter		=	ctrl->m_loadCenter;
	m_loadRotation		=	ctrl->m_loadRotation;
	//
	m_saveWindowRect	=	ctrl->m_saveWindowRect;
	m_saveRotCenter		=	ctrl->m_saveRotCenter;
	m_saveRotation		=	ctrl->m_saveRotation;
	m_fSleepTime		=	ctrl->m_fSleepTime;
	this->m_bEnableRotation	=	ctrl->m_bEnableRotation;
	if(GetElementCount() == ctrl->GetElementCount())
	{
		for(int i=0;i<(int)ctrl->GetElementCount();i++)
		{
			*static_cast<UIElement*>(this->GetElementPtr(i))	=	*static_cast<UIElement*>(ctrl->GetElementPtr(i));
		}
	}
	m_soundFile	=	ctrl->m_soundFile;
	m_dialogPtr->m_bRequestResort = true;//ResortControls();
	m_fPlayTime			=	0;
	if(ctrl->GetText())
		SetText(ctrl->GetText());
	//for(int i=0;i<m_animationList.size();i++)
	//{
	//	CAnitemp2* data = new CAnitemp2();
	//	data->CloneFrom(m_animationList[i]);
	//	ctrl->m_animationList.push_back( data );
	//}
	m_fPlaySpeed		=	ctrl->m_fPlaySpeed;
	m_nPlayLoopCount	=	ctrl->m_nPlayLoopCount;
	m_nTrackLoopCount	=	ctrl->m_nTrackLoopCount;
	m_bSetDefault			=	false;
	m_bChanged			=	true;
//	GetDefaultPlayAni()->PlayAnimation(0);
	for(int i=0;i<ctrl->m_trackList.size();i++)
	{
#ifdef EDITOR_OS
		UIAnimationTrack* data = new UIAnimationTrack();
		m_trackList.push_back( data );
		data->CloneFrom(ctrl->m_trackList[i]);
#else
		UIAnimationTrack* data = ctrl->m_trackList[i];
		m_trackList.push_back( data );
		data->AddRef();
#endif
	}
	m_fAngle			=	ctrl->m_fAngle;
	m_bEnableUnitAngle	=	ctrl->m_bEnableUnitAngle;
	m_unitAngle			=	ctrl->m_unitAngle;
}
VOID				UIControl::SetInnerMesh(Mesh* mesh)//会自动clone,如果是0则清除
{CPUTime(UIControl);
	m_innerMesh = 0;
	if(mesh){
		Mesh* temp = static_cast<Mesh*>(mesh->Clone());
		if(temp)
		{
			m_innerMesh = temp;
			m_innerMesh->SetWorldMatrix(matrix4x4::s_identity());
			m_innerMesh->SetActionType(Phantom::ActionType_Idle, true, -1);
			m_innerMesh->m_bUseSystemProgram=false;
			temp->Release();
		}
	}
}
VOID				UIControl::RenderMesh()
{CPUTime(UIControl);
	if(!m_innerMesh)return;
	g_manager->SetEnableZ(true);
	SetClipRect(true);
	float fLength = 12.0f;
	int mx = g_manager.GetBufferWidth()/2;
	int my = g_manager.GetBufferHeight()/2;
	int rx = m_windowRect.GetWidth()/2;
	int ry = m_windowRect.GetHeight()/2;
	rect_f rcf((float)(m_windowRect.left-rx) / (float)m_dialogPtr->m_screenSize.cx, (float)m_windowRect.top/(float)m_dialogPtr->m_screenSize.cy,
		(float)(m_windowRect.right+rx)/(float)m_dialogPtr->m_screenSize.cx, (float)m_windowRect.bottom/(float)m_dialogPtr->m_screenSize.cy);
	g_manager.SetViewport(&rcf);
	float offX=0;
	float offZ=0;
	g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
	g_manager->SetBlendMode((BlendMode)Phantom::BlendMode_SRCALPHA,(BlendMode)Phantom::BlendMode_INVSRCALPHA);
	GetCameraPtr()->SetViewMatrixOnce(float3(offX, fLength, offZ), float3(offX, 0, offZ));
	m_innerMesh->Render(false);
	SetClipRect(false);
	GetCameraPtr()->SetViewMatrix();
	g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
	g_manager->SetCullVisible(CullMode_CW);
	g_manager->SetEnableZ(false);
	g_manager.SetViewport(0);
}

VOID				UIControl::SetClipRect(BOOL b)
{CPUTime(UIControl);
	if(b){
		int w = g_manager.GetDeviceWidth();
		int h = g_manager.GetDeviceHeight();
		float fScaleX = (float)w/(float)m_dialogPtr->m_screenSize.cx;
		float fScaleY = (float)h/(float)m_dialogPtr->m_screenSize.cy;
		Rect wrc = m_windowRect;
		wrc.Offset(m_dialogPtr->m_x, m_dialogPtr->m_y);
#ifdef WIN32_OS
		Rect clip(wrc.left * g_manager.GetUIScale().x, wrc.top * g_manager.GetUIScale().y, wrc.right * g_manager.GetUIScale().x, wrc.bottom * g_manager.GetUIScale().y);
#else
		float left = wrc.left*fScaleX;
		float right = wrc.right*fScaleX;
		float top = wrc.top*fScaleY;
		float bottom = wrc.bottom*fScaleY;
		Rect clip(left, h - bottom, right, 0);
		clip.bottom = clip.top + (bottom-top);
		Rect temp(wrc.left * g_manager.GetUIScale().x, wrc.top * g_manager.GetUIScale().y, wrc.right * g_manager.GetUIScale().x, wrc.bottom * g_manager.GetUIScale().y);
		if((m_nAlignMode & 0x04)>0||(m_nAlignMode & 0x01)>0){
			clip.left = temp.left;
			clip.right = temp.right;
		}
#ifdef IPHONE_OS
        clip.right*=2;
        clip.left*=2;
#endif
#endif
		g_manager->SetUIClipRect(&clip);
	}
	else
		g_manager->SetUIClipRect(0);
}
UIControl*			UIControl::Clone(float fTime, UIDialog* pDestDialog)
{CPUTime(UIControl);
	UIControl*	ctrl		=	0;
	UIDialog* base = m_dialogPtr;
	if(pDestDialog)base = pDestDialog;
#ifndef EDITOR_OS
	for(int i=0;i<base->m_clones.size();i++){
		if(base->m_clones[i]->m_from==this){
			if(base->m_clones[i]->m_fromTime<g_manager.GetAppTime())
				return base->m_clones[i];
		}
	}
#endif
	if(pDestDialog)
	{
		ctrl				=	(UIControl*)pDestDialog->CreateFromType( false, this->GetType() );
		ctrl->m_name		=	m_name;
		//strcpy(ctrl->m_szName, this->m_szName);
	}
	else
		ctrl				=	(UIControl*)m_dialogPtr->CreateFromType( false, this->GetType() );
	ctrl->m_from = this;
	if(fTime<=0)fTime = 10000000.0f;
	ctrl->m_fromTime = g_manager.GetAppTime()+fTime;
	base->m_clones.push_back(ctrl);
	ctrl->CopyFrom(this);
	return ctrl;
}

Pixel		UIControl::GetCenterPos()
{CPUTime(UIControl);
	return Pixel(this->m_windowRect.left + this->m_rotCenter.x, m_windowRect.top + m_rotCenter.y);
}
VOID		UIControl::SetCenterPos(const Pixel& s)
{CPUTime(UIControl);
	m_rotCenter = s;
	int left	=	s.x - m_rotCenter.x;
	int top		=	s.y - m_rotCenter.y;
	Rect rc = this->GetRenderRect();
	int w		=	rc.GetWidth();
	int h		=	rc.GetHeight();
	SetRenderRect(Rect(left, top, left + w, top + h));
	m_bChanged	=	true;
}
#pragma pack(push, 1)

struct	ControlDataOld
{
	int						m_nPlayAction;
	BOOL					m_bIsBackground;
	PtrID					m_groupID;
	//
	int 					m_value;
	int 					m_minValue;
	int 					m_maxValue;
	PtrID					m_parentID;
	int						m_buttonType;
	BOOL					m_bDrawButtonFrame;
	float					m_fPlaySpeed;
	INT						m_nTrackLoopCount;
	INT						m_nPlayLoopCount;
	BOOL					m_bChanged;
	char					m_szCursorFile[256];
	char					m_bCursorSet;
	unsigned int			m_shadowColor;
	int						m_nLevel;
	char					m_bReadOnly;
	char					m_bCustomRender;
	char 					m_bVisible;
	char 					m_bMouseEnter;
	char 					m_bHasFocus;
	char 					m_bIsDefault;
	NameAW					m_name;
	char					m_bEnableRotation;
	float					m_rotation;
	int						m_loadVersion;
	INT						m_nProgressType;
	char					m_bLoopMode;
	float					m_fPlayTime;
	Name					m_soundFile[UISoundPlay_count];
	unsigned int			m_Index;
	INT						m_nToolTipPopup;
	char					m_bParentMove;
	char					m_bMoveDown;
	Pixel					m_moveOffset;
	char					m_bRenderIndicator;
	char					m_bRenderCandList;
	char					m_bEnableIME;
	Pixel					m_rotCenter;
	//
	BOOL					m_bTrackAutoBeginAni;	//是否在对话框显示的时候自动开始计算动画
	BOOL					m_bTrackPlayAni;
	BOOL					m_bTrackPause;
	float					m_fTrackPlaySpeed;
	Rect					m_saveWindowRect;
	Pixel					m_saveRotCenter;
	float					m_saveRotation;
	Rect					m_windowRect;
	INT						m_nAnimationID;
	color4					m_textColor;
	color4					m_textureColor;
	float					m_fSleepTimeCur;
	float					m_fSleepTime;
	unsigned int			m_textFormat;
	int						m_nShadowWidth;
	int						m_bTrackSaved;
	int						m_controlID;
	char					m_bIsEnabled;
	char					m_bCanFocus;
	INT						m_nTabIndex;
	BOOL					m_bIsInvalidate;
	color4					m_disableColor;
	int    					m_scrollBarWidth;
	BOOL					m_bIsPickPixel;
	float					m_fAngle;
	int						m_unitAngle;
	int						m_bEnableUnitAngle;
	INT						m_nLayerAdd;
	BOOL					m_bAutoSort;
	INT						m_nContext;
	char					m_bVisibleOnScript;
	BOOL					m_bHideOnEditor;
	//
	int						m_mouseEnterSrc;
	int						m_mouseEnterDesc;
	color4					m_mouseEnterColor;
	BOOL					m_bMouseEnterEffect;
	//2013-1-3
	BOOL					m_bEnableUseImageText;
	char					m_szNumberBegin[28];
	BOOL					m_bAniCenter;	//是否以中心点作为左右上下的动画
	char					reserved[32];
};
#pragma pack(pop)
char		UIControl::SaveControl(CSafeFileHelperW& w)
{CPUTime(UIControl);
	UIAnimationTrack* saveActive	=	this->m_trackPtr;
	if(m_trackPtr)
	{
		m_trackPtr	=	0;
		OnTrackActiveChange(FALSE);
	}
	int version = 0x000000AB;
	w << version;
	skip_w sw(w.pStream);
	w << this->m_nAlignMode;
	//
	ControlData* data = static_cast<ControlData*>(this);
	int length = sizeof(ControlData);
	w << length;
	w.pStream->write(data, length);
	int elementCount = (int)m_elementCount;
	w << elementCount;
	for(int i=0;i<(int)m_elementCount;i++)
		m_elementList[i]->SaveElement( GetDlgManager(), w );
	short nCount	=	this->m_trackList.size();
	w << nCount;
	for(int i=0;i<nCount;i++)
		this->m_trackList[i]->Save(w);
	//
	w << m_numberSizeAdd;
	w << this->m_bUseImageFont;
	w << m_bIsGroupCenter;
	w << m_bIsGridObject;
	w << m_sGridOffsetY;
	sw.end();
	if(saveActive)
	{
		this->m_trackPtr	=	saveActive;
		this->OnTrackActiveChange(TRUE);
	}
	return true;
}
char		UIControl::LoadControl(CSafeFileHelperR& r)
{CPUTime(UIControl);
	int version;
	r >> version;
	skip_r sr;
	if(version>=0x000000A4)
		sr.begin(r.pStream);
	if(version >= 0x000000A1)
		r >> m_nAlignMode;
	m_loadVersion	=	version;
	//ControlData* data = static_cast<ControlData*>(this);
	if(version<0x000000A3){
		ControlDataOld data;
		int length;
		int cS	=	sizeof(ControlDataOld);
		r >> length;
		assert(length <= cS);
		r.pStream->read(&data, length);
		m_name = data.m_name.strA();
#ifdef WIN32_OS
		if(version<0x000000A2){
			char buf[256];
			int len = ::WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)data.m_name.strW(), data.m_name.sizeW(), buf, 256, 0, 0);
			buf[len] = 0;
			m_name = buf;
		}
#endif
		m_soundFile = data.m_soundFile[UISoundPlay_buttonDown];
		memcpy(m_szNumberBegin, data.m_szNumberBegin, sizeof(m_szNumberBegin));
		//
		m_nPlayAction=data.m_nPlayAction;
		m_bIsBackground=data.m_bIsBackground;
		m_groupID=data.m_groupID;
		m_value=data.m_value;
		m_minValue=data.m_minValue;
		m_maxValue=data.m_maxValue;
		m_parentID=data.m_parentID;
		m_buttonType=data.m_buttonType;
		m_bDrawButtonFrame=data.m_bDrawButtonFrame;
		m_fPlaySpeed=data.m_fPlaySpeed;
		m_nTrackLoopCount=data.m_nTrackLoopCount;
		m_nPlayLoopCount=data.m_nPlayLoopCount;
		m_bChanged=data.m_bChanged;
		m_shadowColor=data.m_shadowColor;
		m_nLevel=data.m_nLevel;
		m_bReadOnly=data.m_bReadOnly;
		m_bCustomRender=data.m_bCustomRender;
		m_bVisible=data.m_bVisible;
		m_bMouseEnter=data.m_bMouseEnter;
		m_bHasFocus=data.m_bHasFocus;
		m_bIsDefault=data.m_bIsDefault;
		m_bEnableRotation=data.m_bEnableRotation;
		m_rotation=data.m_rotation;
		m_loadVersion=data.m_loadVersion;
		m_nProgressType=data.m_nProgressType;
		m_bLoopMode=data.m_bLoopMode;
		m_fPlayTime=data.m_fPlayTime;
		m_bParentMove=data.m_bParentMove;
		m_bMoveDown=data.m_bMoveDown;
		m_moveOffset=data.m_moveOffset;
		m_rotCenter=data.m_rotCenter;
		m_bTrackAutoBeginAni=data.m_bTrackAutoBeginAni;
		m_bTrackPlayAni=data.m_bTrackPlayAni;
		m_bTrackPause=data.m_bTrackPause;
		m_fTrackPlaySpeed=data.m_fTrackPlaySpeed;
		m_saveWindowRect=data.m_saveWindowRect;
		m_saveRotCenter=data.m_saveRotCenter;
		m_saveRotation=data.m_saveRotation;
		m_windowRect=data.m_windowRect;
		m_nAnimationID=data.m_nAnimationID;
		m_textColor=data.m_textColor;
		m_textureColor=data.m_textureColor;
		m_fSleepTimeCur=data.m_fSleepTimeCur;
		m_fSleepTime=data.m_fSleepTime;
		m_textFormat=data.m_textFormat;
		m_nShadowWidth=data.m_nShadowWidth;
		m_bTrackSaved=data.m_bTrackSaved;
		m_controlID=data.m_controlID;
		m_bIsEnabled=data.m_bIsEnabled;
		m_bCanFocus=data.m_bCanFocus;
		m_nTabIndex=data.m_nTabIndex;
		m_bIsInvalidate=data.m_bIsInvalidate;
		m_disableColor=data.m_disableColor;
		m_scrollBarWidth=data.m_scrollBarWidth;
		m_bIsPickPixel=data.m_bIsPickPixel;
		m_fAngle=data.m_fAngle;
		m_unitAngle=data.m_unitAngle;
		m_bEnableUnitAngle=data.m_bEnableUnitAngle;
		m_nLayerAdd=data.m_nLayerAdd;
		m_bAutoSort=data.m_bAutoSort;
		m_bVisibleOnScript=data.m_bVisibleOnScript;
		m_bHideOnEditor=data.m_bHideOnEditor;
		m_bEnableUseImageText=data.m_bEnableUseImageText;
		m_bAniCenter=data.m_bAniCenter;
	}
	else
	{
		int length;
		int cS	=	sizeof(ControlData);
		r >> length;
		assert(length <= cS);
		ControlData* data = static_cast<ControlData*>(this);
		r.pStream->read(data, length);
	}
	m_loadVersion	=	version;
	int elementCount = (int)m_elementCount;
	r >> elementCount;
	if(version<0x000000A3){
		int tiplength;//	=	m_toolTipText.sizeW();//(int)m_szToolTip.size();
		r >> tiplength;
		if(tiplength > 0)
		{
			CDynamicArray<short_t> szToolTip;
			szToolTip.SetArrayCount(tiplength + 1);
			r.pStream->read((short_t*)szToolTip.GetPtr(), sizeof(short_t) * tiplength);
			szToolTip[tiplength] = 0;
			//
		}
	}
	if(version<0x000000A8){
		if(this->GetType()==Phantom::UIControlType_BUTTON)
		{
			for(int i=0;i<elementCount;i++)
			{
				int newi = -1;
				if(i==0)newi=0;
				if(i==1)newi=1;
				if(i==2)newi=2;
				if(i==4)newi=3;
				if(newi>=0){
					m_elementList[newi]->LoadElement( GetDlgManager(), this->m_dialogPtr, r , false, true);
				}
				else{
					UIElement ele(this);
					ele.LoadElement( GetDlgManager(), this->m_dialogPtr, r , false, true);
				}
			}
		}
		else
		{
			for(int i=0;i<elementCount;i++)
			{
				if(i >= m_elementCount)
				{
					UIElement ele(this);
					ele.LoadElement( GetDlgManager(), this->m_dialogPtr, r , false, true);
				}
				else
					m_elementList[i]->LoadElement( GetDlgManager(), this->m_dialogPtr, r , false, true);
			}
		}
	}
	else{
		for(int i=0;i<elementCount;i++)
		{
			if(i >= m_elementCount)
			{
				UIElement ele(this);
				ele.LoadElement( GetDlgManager(), this->m_dialogPtr, r , false, true);
			}
			else
				m_elementList[i]->LoadElement( GetDlgManager(), this->m_dialogPtr, r , false, true);
		}
	}
	short nCount;
	r >>nCount;
	for(int i=0;i<nCount;i++)
	{
		UIAnimationTrack* t	=	new UIAnimationTrack();
		t->Load(r, this);
		m_trackList.push_back(t);
	}
	if(version>=0x000000A5)
		r >> m_numberSizeAdd;
	if(version>=0x000000A7)
		r >> m_bUseImageFont;
	if(version>=0x000000A9)
		r >> m_bIsGroupCenter;
	if(version>=0x000000AA)
		r >> m_bIsGridObject;
	if(version>=0x000000AB)
		r >> m_sGridOffsetY;
	sr.end();
	m_trackPtr	=	0;
	m_saveWindowRect		=	m_windowRect;
	m_saveRotCenter			=	m_rotCenter;
	m_saveRotation			=	m_rotation;
	m_bSetDefault			=	false;
	m_bChanged				=	true;
	m_loadRect		=	m_windowRect;
	m_loadCenter	=	m_rotCenter;
	m_loadRotation	=	m_rotation;
	if(m_soundFile.t[0])
		m_soundFile.setIsFile();
	return true;
}
void		UIControl::SetParentMove(char b){
	this->m_dialogPtr->m_centerObj=this;
	m_bParentMove = b;
}
VOID		UIControl::OnChangeText()
{CPUTime(UIControl);
	if(this->m_text.size()==0)
	{
		if(this->m_textPtr)
			g_manager.RemoveText(m_textPtr);
		m_textPtr = 0;
		return;
	}
	FontNode2D* pFontNode = GetDlgManager()->GetFontNode(GetElementPtr(0)->GetFontID());
	if(!pFontNode)
		return;
	if(m_bUseImageFont){
		if(m_textPtr&&!m_textPtr->IsFigFont())
		{
			g_manager.RemoveText(m_textPtr);
			m_textPtr = 0;
		}
		if(!m_textPtr)
			m_textPtr = new RenderTextFig();
		RenderTextFig* fig = static_cast<RenderTextFig*>(m_textPtr);
		fig->SetFontSize(pFontNode->nFontHeight>0?pFontNode->nFontHeight:-pFontNode->nFontHeight);
		fig->SetFontStyle(0);
		fig->SetText(m_text.str());
		return;
	}
	int nStyle = 0;
	if(pFontNode->nWeight)
	{
		nStyle = 1;
		if(pFontNode->bItalic)
			nStyle = 3;
	}
	else if(pFontNode->bItalic)
		nStyle = 2;
	RenderText* old = m_textPtr;
	int count = 0;//(m_windowRect.GetWidth()-5) / (int)abs(pFontNode->nFontHeight)-1;
	int fw = (int)abs(pFontNode->nFontHeight)/2;
	int ww = m_windowRect.GetWidth()-20;
	CDynamicArray<char> texts;
	int len = m_text.size();
	int nw = 0;
	//for(int i=0;i<len;i++){
	//	if(isascii(m_text.texts[i]))//if(m_text.texts[i]>0)
	//	{
	//		nw+=fw;
	//	}
	//	else{
	//		i+=2;
	//		nw+=fw*2;
	//	}
	//	if(nw>=ww)
	//		break;
	//	count++;
	//}
	int last = 0;
	texts.SetArrayCount(len + 100);
	int cnt = 0;
	for(int i=0;i<len;i++){
		if(m_text.texts[i] == '\r')
			continue;
		if(m_text.texts[i] == '\\' && m_text.texts[i+1] == 'n'){
			i++;
			last = i;
			nw = 0;
			texts[cnt++] = '\n';
		}
		else if(m_text.texts[i] == '\n'){
			last = i;
			nw = 0;
			texts[cnt++] = '\n';
			//if(newline!=0)
			//	texts[cnt++] = '\n';
			//newline = 0;
		}
		else{
			if(nw>ww){
				if(cnt<(len-1))
				{
					nw = 0;
					texts[cnt++] = '\n';
				}
			}
			if(isascii(m_text.texts[i]))
			{
				texts[cnt++] = m_text.texts[i];
				nw+=fw+2;
			}
			else
			{
				texts[cnt++] = m_text.texts[i++];
				if(i<len)
					texts[cnt++] = m_text.texts[i++];
				if(i<len)
					texts[cnt++] = m_text.texts[i];
				nw+=fw*2+2;
			}
		}
	}
	texts[cnt] = 0;
	texts.SetArrayCount(cnt);
	if(this->IsPassword()){
		for(int i=0;i<texts.size();i++)
			texts[i] = '*';
		m_textPtr = g_manager.InsertText(texts.GetPtr(), pFontNode->nFontHeight, nStyle, GetElementPtr(0)->dwTextFormat, pFontNode->fontName.t);
	}
	else
		m_textPtr = g_manager.InsertText(texts.GetPtr(), pFontNode->nFontHeight, nStyle, GetElementPtr(0)->dwTextFormat, pFontNode->fontName.t);
	m_bChanged = true;
	if(old)
		g_manager.RemoveText(old);
}

void		UIControl::SetLayer(int level)
{CPUTime(UIControl);
	m_nLevel = level;
	static_cast<UIGroup*>(this->GetGroupPtr())->Resorts();
	this->m_dialogPtr->ResortControls();
}

const matrix4x4&		UIControl::GetControlMat()
{CPUTime(UIControl);
	if(this->m_bIsInvalidate)
	{
		Pixel pos(m_windowRect.left + m_dialogPtr->m_x, m_windowRect.top + m_dialogPtr->m_y);
		//
		matrix4x4 matRotation;
		matrix4x4* pMatRot = 0;
		float3* pCenter;
		float3 center;
		//float fScaleX 		= 1.0f;
		//float fScaleY 		= 1.0f;
		//fScaleX				*=	((float)m_currentAnim.rc.GetWidth() / (float)this->m_elementList[0]->rcTexture.GetWidth());
		//fScaleY				*=	((float)m_currentAnim.rc.GetHeight() / (float)m_elementList[0]->rcTexture.GetHeight());
		//matrixScaling( &m_controlMat, fScaleX, fScaleY, 1.0f );
		m_controlMat.identity();
		if(m_currentAnim.bRotateEnable)
		{
			pCenter			=	&center;
			center.setxyz(m_currentAnim.center.x + m_parentOffset.x, m_currentAnim.center.y + m_parentOffset.y, 0);
			matrixRotationZ(&matRotation, Radians(m_currentAnim.rot));
			pMatRot		=	&matRotation;
		}
		if(pMatRot)
		{
			m_controlMat._41 = -(float)pCenter->x;
			m_controlMat._42 = -(float)pCenter->y;
			matrixMultiply(&m_controlMat, &m_controlMat, pMatRot);
			m_controlMat._41 = (float)pos.x + pCenter->x;
			m_controlMat._42 = (float)pos.y + pCenter->y;
		}
		else
		{
			m_controlMat._41 = (float)pos.x;
			m_controlMat._42 = (float)pos.y;
		}
	}
	m_bIsInvalidate	=	false;
	return m_controlMat;
}
VOID		UIControl::OnChangeMatrix()
{
	m_bChanged	=	false;
	//Rect rcScreen	=	this->m_currentAnim.rc;
	//OffsetRect( &rcScreen, m_dialogPtr->m_dlgX, m_dialogPtr->m_dlgY );
	//BOOL bRot = m_currentAnim.bRotateEnable;
	//if(bRot&&(int)(m_currentAnim.rot*1000.0f)==0)
	//	bRot = false;
	//matrixScaling(&m_renderMat, rcScreen.GetWidth(), rcScreen.GetHeight(), 1);
	//Phantom::matrix4x4 matRotation;
	//if(bRot)
	//{
	//	matrixRotationZ(&matRotation, Radians(m_currentAnim.rot));
	//	matrixMultiply(&m_renderMat, &m_renderMat, &matRotation);
	//}
	//float cx = (float)m_currentAnim.center.x * m_scale.x;
	//float cy = (float)m_currentAnim.center.y * m_scale.y;
	//m_renderMat._41 = (float)rcScreen.left + cx;
	//m_renderMat._42 = (float)rcScreen.top + cy;
	//m_renderMat._43 = cx / (float)rcScreen.GetWidth();
	//m_renderMat._44 = cy / (float)rcScreen.GetHeight();
	//
}
Pixel		UIControl::ClientToScreen(const Pixel& pixel)
{CPUTime(UIControl);
	const matrix4x4& mat	=	GetControlMat();
	float3 pos(pixel.x, pixel.y, 0);
	pos.x	-=	mat._41;
	pos.y	-=	mat._42;
	pos.transform(pos, mat);
	return Pixel(pos.x, pos.y);
}

Pixel		UIControl::ScreenToClient(const Pixel& pixel)
{CPUTime(UIControl);
	const matrix4x4& mat	=	GetControlMat();
	matrix4x4 matInv;
	matrixInverse(&matInv, 0, &mat);
	float3 pos(pixel.x, pixel.y, 0);
	pos.transform(pos, matInv);
	pos.x	+=	mat._41;
	pos.y	+=	mat._42;
	return Pixel(pos.x, pos.y);
}

BOOL		UIControl::IsEnabledCtrl()
{CPUTime(UIControl);
	if(!this->m_bIsEnabled || !m_dialogPtr->m_bEnabled)
		return false;
	UIGroup* g	=	this->m_dialogPtr->FindGroupPtr(m_groupID);
	if(!g || !g->GetEnabled())
		return false;
	return true;
}
char		UIControl::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex )
{CPUTime(UIControl);
	if( !m_bVisible )
		return false;
	if(!IsEnabledCtrl()){
		switch( uMsg ){
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
			{
				if( isPtIn( pt ) )
					return true;
			}
			break;
		}
	}
	if(!this->m_bParentMove)
	{
		if(GetType() == UIControlType_STATIC)
		{
			switch( uMsg )
			{
			case InputEventID_MouseMove:
				{
					return isPtIn( pt );
				}
				break;
			case InputEventID_ButtonDown:
			case InputEventID_ButtonDblClk:
				{
					if( isPtIn( pt ) )
					{
						return true;
					}
					break;
				}
			case InputEventID_ButtonUp:
				{
					m_bParentMove	=	false;
					return isPtIn( pt );
				}
				break;
			}
			return false;
		}
		else
			return isPtIn( pt);
	}
	switch( uMsg )
	{
	case InputEventID_MouseMove:
		{
			if(this->m_bMoveDown == (touchIndex + 1))
			{
				Pixel pos;
				g_manager.GetCursorPos2D(pos, touchIndex);
				int x, y;
				m_dialogPtr->GetLocation(x, y);
				int w	=	m_dialogPtr->GetWidth();
				int h	=	m_dialogPtr->GetHeight();
				x		=	pos.x - m_moveOffset.x;
				y		=	pos.y - m_moveOffset.y;
				int sw	=	g_manager.GetUIBufferWidth();
				int sh	=	g_manager.GetUIBufferHeight();
				if(x > (sw - w))
					x	=	sw - w;
				else if(x < 0)
					x	=	0;
				if(y > (sh - h))
					y = sh - h;
				else if(y < 0)
					y	=	0;
				m_dialogPtr->SetLocation(x, y);
				return true;
			}
			return isPtIn( pt );
		}
		break;
	case InputEventID_ButtonDown:
	case InputEventID_ButtonDblClk:
		{
			if( isPtIn( pt ) )
			{
				int x, y;
				m_dialogPtr->GetLocation(x, y);
				Pixel pos;
				g_manager.GetCursorPos2D(pos, touchIndex);
				this->m_moveOffset.Set( pos.x - x, pos.y - y );
				this->m_bMoveDown	=	(touchIndex + 1);
				return true;
			}
			break;
		}
	case InputEventID_ButtonUp:
		{
			if( m_bMoveDown )
			{
				m_bMoveDown = false;
				return true;
			}
			break;
		}
	}
	return false;
}

void				UIControl::SetTextColor(const color4& c)
{CPUTime(UIControl);
	m_bSetDefault			=	false;
	m_textColor = (color4&)c;
}

void				UIControl::SetTextureColor(const color4& c)
{CPUTime(UIControl);
	m_bSetDefault			=	false;
	m_textureColor = (color4&)c;
}

VOID				UIControl::SetText(const char* szText)
{CPUTime(UIControl);
	m_textIndexs.SetArrayCount(0);
	if(!szText)
	{
		m_text.clear();
		OnChangeText();
		return;
	}
	if(m_text.str()!=szText)
		m_text	=	szText;
	OnChangeText();
}
//void 				UIControl::SetTextW( const wchar_t* strText )
//{
//	m_textIndexs.SetArrayCount(0);
//	if(!strText)
//	{
//		m_text.clear();
//		return;
//	}
//	m_text	=	strText;
//	OnChangeText();
//}

Rect				UIControl::GetSavedWindowRect()
{CPUTime(UIControl);
	Rect rc	=	this->m_saveWindowRect;
	UIControl* parent	=	this->GetParentControl();
	if(parent && parent != this)
	{
		Rect rcP	=	static_cast<UIControl*>(parent)->m_currentAnim.rc;
		rc.Offset(rcP.left, rcP.top);
	}
	rc.Offset(m_dialogPtr->m_x, m_dialogPtr->m_y);
	return rc;
}

Rect				UIControl::GetWindowRect()
{CPUTime(UIControl);
	Rect rc	=	(Rect&)GetRenderRect();
	UIControl* parent	=	this->GetParentControl();
	if(parent && parent != this)
	{
		Rect rcP	=	static_cast<UIControl*>(parent)->m_currentAnim.rc;
		rc.Offset(rcP.left, rcP.top);
	}
	rc.left		=	(int)((float)rc.left);
	rc.top		=	(int)((float)rc.top);
	rc.right	=	(int)((float)rc.right);
	rc.bottom	=	(int)((float)rc.bottom);
	rc.Offset(m_dialogPtr->m_x, m_dialogPtr->m_y);
	return rc;
}

void				UIControl::SetName(const char* szName)
{CPUTime(UIControl);
	m_name = szName;
	UIScrollBar* scroll	=	static_cast<UIScrollBar*>(GetScrollBar());
	if(scroll != this && scroll)
	{
		char szScrollName[256];
		sprintf(szScrollName, "%s_scrollbar", m_name.t);
		scroll->SetName( szScrollName );
	}
}

VOID		UIControl::SetUnitAngle(float fAngle)	//设置面对方向
{CPUTime(UIControl);
	m_fAngle		=	(int)fAngle % 360;
	if(m_fAngle < 0.0f)
		m_fAngle	+=	360.0f;
	if(m_bEnableUnitAngle==AngleMode_None)return;
	//float fDegress	=	Radians(m_fAngle);
	//fDegress		=	fmodf(fDegress, PI_X2);
	//if(fDegress < 0)
		//fDegress	+=	PI_X2;
	
	int nAngle		=	0;
	Unit2DAngle* dir = 0;
	if(m_bEnableUnitAngle==AngleMode_LRTB)
	{
		nAngle		=	(int)((m_fAngle + 45.0f) / 90.0f) % 4;
		static Unit2DAngle s_dir[4] = {Unit2DAngle_Bottom, Unit2DAngle_Left, Unit2DAngle_Top, Unit2DAngle_Right};
		dir = s_dir;
	}
	else if(m_bEnableUnitAngle==AngleMode_8)
	{
		nAngle = (int)((m_fAngle + 22.5f) / 45.0f) % Unit2DAngle_Count;
		static Unit2DAngle s_dir[Unit2DAngle_Count] = {Unit2DAngle_Bottom, Unit2DAngle_LeftBottom, Unit2DAngle_Left, Unit2DAngle_LeftTop, Unit2DAngle_Top, Unit2DAngle_RightTop, Unit2DAngle_Right, Unit2DAngle_RightBottom};
		dir = s_dir;
	}
	else if(m_bEnableUnitAngle==AngleMode_XLRTB)
	{
		nAngle		=	(int)(m_fAngle / 90.0f) % 4;
		static Unit2DAngle s_dir[4] = {Unit2DAngle_LeftBottom, Unit2DAngle_LeftTop, Unit2DAngle_RightTop, Unit2DAngle_RightBottom};
		dir = s_dir;
	}
	if(m_unitAngle != dir[nAngle])
	{
		m_unitAngle		=	dir[nAngle];
		if(m_bEnableUnitAngle)
		{
			for(int i=0;i<this->GetElementCount();i++)
				static_cast<UIElement*>(GetElementPtr(i))->ChangeAngle();
		}
	}
}

float		UIControl::GetUnitAngle()
{CPUTime(UIControl);
	return this->m_fAngle;
}

VOID		UIControl::SetTextureMode(const char* szName, DrawTextAlign align)//是否按照高度重新设置缩放
{CPUTime(UIControl);
	for(int i=0;i<this->m_elementCount;i++)
	{
		this->m_elementList[i]->SetTextureMode(szName, align);
	}
}

VOID		UIControl::SetUnitAngleDir(const float2& dir)	//设置面对方向
{CPUTime(UIControl);
	SetUnitAngle(Degress(getRotateZ(float3(dir.x, dir.y, 0))));
}

//2013-1-3
void		RenderImageText(const Rect& windowRect, const char* numberBegin, const char* szText, int textLength, int textureID, int align,
	const color4& color, const float2& scale, const Pixel& rotcenter, CDynamicArrayS<int,0>& indexs, int imageSize, int imageOffset, const Pixel* offsetXY, BOOL bIsEnabled, int cullmode)
{
	CDynamicArray<TextureRect>* rects = GetDlgManager()->GetTextureRectPtr(textureID);
	if(!rects)return;
	Texture* node = GetDlgManager()->GetTextureNode(textureID, 0);
	if(!node)
		return;
	if(indexs.size() == 0)// != m_text.sizeA())
	{
		indexs.SetArrayCount(textLength);
		const char* text = szText;
		for(int i=0;i<textLength;i++)
		{
			indexs[i]=-1;
			//2013-1-3:2
			if(text[i] == '\r')
				continue;
			if(text[i] == '\n')
			{
				indexs[i]=-2;
				continue;
			}
			char name[128];
			strcpy(name, numberBegin);//图片数字前缀
			char ch = text[i];
			{
				char cc2[5];
				cc2[0] = text[i];
				cc2[1] = text[i+1];
				cc2[2] = 0;
				if(text[i]>0)
					cc2[1] = 0;
				strcat(name, cc2);
			}
			for(int j=0;j<rects->size();j++)
			{
				if(strnicmp(rects->GetPtr()[j].name.t, name, rects->GetPtr()[j].name.size()) == 0)//.t[0] == wtext[i])if(stricmp(rects->GetPtr()[j].name.t, name) == 0)//.t[0] == wtext[i])
				{
					indexs[i] = j;
					break;
				}
			}
		}
	}
	int left = 0;
	Rect rect = windowRect;
	int centerX = (rect.left+rect.right)/2;
	int centerY = (rect.top+rect.bottom)/2;
	int nMaxHeight = 0;
	short	lineTextIndex[1024];//
	int		lineTextCount = 0;
	int		nMaxWidth = 0;
	Rect	saveRect = rect;
	float scaleX = g_manager.GetUIScale().x;
	float scaleY = g_manager.GetUIScale().y;
	BOOL bScale = false;
	if(((int)(scale.x *1000) != 1000) || ((int)(scale.y*1000)!=1000))
		bScale = true;
	for(int i=0;i<=indexs.size();i++)
	{
		BOOL bNewLine = false;
		BOOL bDrawText = true;
		if(i == indexs.size())
			bNewLine	=	true;
		else if(indexs[i] < 0)
		{
			bDrawText	=	false;
			if(indexs[i] == -2)
				bNewLine	=	true;
		}

		if(bDrawText && !bNewLine)
		{
			TextureRect& rc = rects->GetPtr()[indexs[i]];
			int cy = imageSize;
			int cx = imageSize * rc.orig.cx / rc.orig.cy;
			if((rect.left + cx) >= windowRect.right)
			{
				bNewLine = true;
			}
		}
		if(bNewLine || lineTextCount >= 1024)
		{
			//
			rect	=	saveRect;
			BOOL bRight = false;
			if((align & DrawTextAlign_RIGHT) == DrawTextAlign_RIGHT)
			{
				bRight	=	true;
				rect.right	=	windowRect.right;
			}
			else if((align & DrawTextAlign_CENTER) == DrawTextAlign_CENTER)
			{
				rect.left	=	(windowRect.right - windowRect.left - nMaxWidth) / 2 + windowRect.left;
			}
			else
				rect.left	=	windowRect.left;
			BOOL bBottom = ((align & DrawTextAlign_BOTTOM) == DrawTextAlign_BOTTOM);
			BOOL bCenterV = ((align & DrawTextAlign_VCENTER) == DrawTextAlign_VCENTER);
			//
			if((align & DrawTextAlign_RIGHT) == DrawTextAlign_RIGHT)
			{
				for(int j=(lineTextCount-1);j>=0;j--)
				{
					int index = lineTextIndex[j];
					if(index >= 0)
					{
						TextureRect& rc = rects->GetPtr()[index];
						//
						int cy = imageSize;
						int cx = imageSize * rc.orig.cx / rc.orig.cy;
						if(cy > nMaxHeight)nMaxHeight = cy;
						Rect draw(rect.left, rect.top, rect.left + cx, rect.top + cy);
						if(bBottom)
							draw.top = rect.top + windowRect.GetHeight() - cy;
						else if(bCenterV)
							draw.top = rect.top + (windowRect.GetHeight() - cy) / 2;
						draw.bottom = draw.top + cy;
						if(bRight)
						{
							draw.left	=	rect.right - cx;
							draw.right	=	rect.right;
						}
						draw.Offset(rc.off.x, rc.off.y);
						if(offsetXY)
							draw.Offset(offsetXY->x, offsetXY->y);
						if(bScale){
							draw.left = (int)((float)(draw.left - centerX) * scale.x) + centerX;
							draw.right = (int)((float)(draw.right - centerX) * scale.x) + centerX;
							draw.top = (int)((float)(draw.top - centerY) * scale.y) + centerY;
							draw.bottom = (int)((float)(draw.bottom - centerY) * scale.y) + centerY;
						}
						int dw = draw.GetWidth();
						int dh = draw.GetHeight();
						draw.right = draw.left + rc.rc.GetWidth() * dw / rc.orig.cx;
						draw.bottom = draw.top + rc.rc.GetHeight() * dh / rc.orig.cy;

						g_manager.Draw2D(node, draw, rc.rc, &color, 0, 1, bIsEnabled, (CullMode)cullmode);
						cx += imageOffset;
						if(bRight)
							rect.right	-= cx;
						else
							rect.left	+= cx;
					}
				}
			}
			else
			{
				for(int j=0;j<lineTextCount;j++)
				{
					int index = lineTextIndex[j];
					if(index >= 0)
					{
						TextureRect& rc = rects->GetPtr()[index];
						//
						int cy = imageSize;
						int cx = imageSize * rc.orig.cx / rc.orig.cy;
						if(cy > nMaxHeight)nMaxHeight = cy;
						Rect draw(rect.left, rect.top, rect.left + cx, rect.top + cy);
						if(bBottom)
							draw.top = rect.top + windowRect.GetHeight() - cy;
						else if(bCenterV)
							draw.top = rect.top + (windowRect.GetHeight() - cy) / 2;
						draw.bottom = draw.top + cy;
						draw.Offset(rc.off.x, rc.off.y);
						if(offsetXY)
							draw.Offset(offsetXY->x, offsetXY->y);
						if(bScale){
							draw.left = (int)((float)(draw.left - centerX) * scale.x) + centerX;
							draw.right = (int)((float)(draw.right - centerX) * scale.x) + centerX;
							draw.top = (int)((float)(draw.top - centerY) * scale.y) + centerY;
							draw.bottom = (int)((float)(draw.bottom - centerY) * scale.y) + centerY;
						}
						int dw = draw.GetWidth();
						int dh = draw.GetHeight();
						draw.right = draw.left + rc.rc.GetWidth() * dw / rc.orig.cx;
						draw.bottom = draw.top + rc.rc.GetHeight() * dh / rc.orig.cy;
						g_manager.Draw2D(node, draw, rc.rc, &color, 0, 1, bIsEnabled, (CullMode)cullmode);
						cx += imageOffset;
						rect.left	+= cx;
					}
				}
			}
			nMaxWidth		=	0;
			lineTextCount	=	0;
			//
			rect.left	=	windowRect.left;
			rect.right	=	windowRect.right;
			rect.top	+= nMaxHeight;
			if(rect.top >= rect.bottom)
				break;
			saveRect	=	rect;
		}
		//
		if(i < indexs.size() && indexs[i] >= 0)
		{
			lineTextIndex[lineTextCount] = indexs[i];
			lineTextCount++;
			TextureRect& rc = rects->GetPtr()[indexs[i]];
			int cy = imageSize;
			int cx = imageSize * rc.orig.cx / rc.orig.cy;
			cx += imageOffset;
			rect.left	+=	cx;
			nMaxWidth	+=	cx;
		}
	}
}
void		UIControl::RenderImgText(UIElement& element)
{CPUTime(UIControl);
	color4 color = m_textColor * this->m_currentAnim.textColor*this->m_dialogPtr->m_dlgColor;//element.GetTextColor();
	//if(!this->m_bIsEnabled)
		//color = this->GetDisableColor();
	Pixel off(m_dialogPtr->m_x, m_dialogPtr->m_y);
	Pixel center(m_rotCenter.x+m_windowRect.left, m_rotCenter.y+m_windowRect.top);
	RenderImageText(this->m_windowRect, this->m_szNumberBegin, this->m_text.str(), this->m_text.size(), element.GetTextureNewID(), 
		this->m_textFormat, color, this->m_scale, center, this->m_textIndexs, element.textureRect.GetHeight(), m_numberSizeAdd, &off, this->m_bIsEnabled, CullMode_CW);
}
	VOID			UIControl::SetPos(const Pixel& pos)
	{CPUTime(UIControl);
		Rect r = m_windowRect;
		int width = r.GetWidth();
		int height = r.GetHeight();
		r.left = pos.x - m_rotCenter.x;
		r.top = pos.y - m_rotCenter.y;
		r.right = r.left + width;
		r.bottom = r.top + height;
		SetRenderRect(r);
	}
	Pixel			UIControl::GetLoadPos()
	{
		return Pixel(m_loadRect.left + this->m_loadCenter.x, m_loadRect.top + m_loadCenter.y);
	}
	Pixel			UIControl::GetPos()
	{CPUTime(UIControl);
		return Pixel(m_windowRect.left + m_rotCenter.x, m_windowRect.top + m_rotCenter.y);
	}

	VOID	UIControl::RenderDrawMgrs()
	{CPUTime(UIControl);
		g_manager->SetCullMode(Phantom::CullMode_CW);
		for(int i=0;i<m_drawMgrs.size();i++)
		{
			if(m_drawMgrs[i]->m_bVisible)
			{
				m_drawMgrs[i]->Render(m_dialogPtr, m_windowRect);
			}
		}
	}
	void 	UIControl::SetIntVal( int nVal )
	{CPUTime(UIControl);
		char buf[128];
		sprintf(buf, "%d", nVal);
		SetText(buf);
	}
	void 	UIControl::SetDoubleVal( double dVal )
	{CPUTime(UIControl);
		char buf[128];
		sprintf(buf, "%.03f", dVal);
		SetText(buf);
	}

	VOID	UIControl::SetEventFunc(INT nEventID, const char* szFunc)
	{CPUTime(UIControl);
		for(int i=0;i<m_events.size();i++)
		{
			if(m_events[i].id == nEventID)
			{
				if(szFunc&&szFunc[0])
					m_events[i].func = szFunc;
				else
					m_events[i].func.t[0] = 0;//.erase(m_events.begin() + i);
				return;
			}
		}
		for(int i=0;i<m_events.size();i++)
		{
			if(m_events[i].func.t[0] == 0)
			{
				m_events[i].func = szFunc;
				m_events[i].id = nEventID;
				return;
			}
		}
		if(szFunc && szFunc[0]&&nEventID>0){
			UILuaEvent d;
			d.func = szFunc;
			d.id = nEventID;
			m_events.push_back(d);
		}
	}
	VOID	UIControl::AddDrawMgr(UIDrawMgr* m)
	{CPUTime(UIControl);
		safe_addref(m);
		m_drawMgrs.push_back(m);
	}
	VOID UIControl::SetChildDialog(UIDialog* d)
	{CPUTime(UIControl);
		if(m_pChildDialog)
			m_pChildDialog->m_pParentControl = 0;
		m_pChildDialog = d;
		if(m_pChildDialog)
		{
			m_pChildDialog->m_pParentControl = this;
			m_pChildDialog->SetLocation(0,0);
			RecalcScrollValue();
		}
	}
	VOID		UIControl::SetScrollBarV(UIScrollBar* v)
	{CPUTime(UIControl);
		if(m_scrollPtrV)
			m_scrollPtrV->m_containPtr = 0;
		m_scrollPtrV = v;
		if(m_scrollPtrV)
			m_scrollPtrV->m_containPtr=this;
		RecalcScrollValue();
	}
	VOID		UIControl::SetScrollBarH(UIScrollBar* v)
	{CPUTime(UIControl);
		if(m_scrollPtrH)
			m_scrollPtrH->m_containPtr = 0;
		m_scrollPtrH = v;
		if(m_scrollPtrH)
			m_scrollPtrH->m_containPtr=this;
		RecalcScrollValue();
	}
	VOID		UIControl::RecalcScrollValue()
	{CPUTime(UIControl);
		if(this->m_pChildDialog)
		{
			int nTop = 0;
			int nLeft = 0;
			int nScrollMaxWidth = m_pChildDialog->GetWidth() - m_windowRect.GetWidth();
			int nScrollMaxHeight = m_pChildDialog->GetHeight() - m_windowRect.GetHeight();
			int x,y;
			m_pChildDialog->GetLocation(x,y);
			if(m_scrollPtrV)
			{
				m_scrollPtrV->SetVisible(nScrollMaxHeight>0);
				if(nScrollMaxHeight>0){
					m_scrollPtrV->m_nStart = 0;
					m_scrollPtrV->m_nEnd = nScrollMaxHeight;
					m_scrollPtrV->SetScrollPos(-y);
				}
			}
			if(m_scrollPtrH)
			{
				m_scrollPtrH->SetVisible(nScrollMaxWidth>0);
				if(nScrollMaxWidth>0){
					m_scrollPtrH->m_nStart = 0;
					m_scrollPtrH->m_nEnd = nScrollMaxWidth;
					m_scrollPtrH->SetScrollPos(-x);
				}
			}
		}
	}
	VOID		UIControl::OnScroll(UIScrollBar* sc)
	{CPUTime(UIControl);
		if(this->m_pChildDialog)
		{
			int x,y;
			m_pChildDialog->GetLocation(x,y);
			if(m_scrollPtrV==sc)
				y = -m_scrollPtrV->m_nPosition;
			if(m_scrollPtrH==sc)
				x = -m_scrollPtrH->m_nPosition;
			m_pChildDialog->SetLocation(x, y);
		}
	}
	
	VOID		UIControl::RenderScroll()
	{CPUTime(UIControl);
		if(!m_pChildDialog)return;
		int w = m_pChildDialog->GetWidth();
		int h = m_pChildDialog->GetHeight();
		if(w<=0||h<=0)
			return;
		int regionW = m_windowRect.GetWidth();
		int regionH = m_windowRect.GetHeight();
		if(w<regionW)
			w = regionW;
		if(h<regionH)
			h = regionH;
		BOOL bMinX = true;
		float fScaleX = 1;
		BOOL bMinY = true;
		float fScaleY = 1;
		//if(m_childDownIndex<0){
		{
			int x,y;
			m_pChildDialog->GetLocation(x,y);
//			int oldX = x;
//			int oldY = y;
			BOOL bXOK = true;
			BOOL bYOK = true;
			if(m_pChildDialog->m_childAutoMove)
			{
				bXOK = false;
				bYOK = false;
				int offX = m_pChildDialog->m_childMoveStrength.x;
				int offY = m_pChildDialog->m_childMoveStrength.y;
				if(m_scrollPtrH)
					x+=offX;
				if(m_scrollPtrV)
					y+=offY;
				m_pChildDialog->ClipChildOffset(x,y);
				m_pChildDialog->SetLocation(x,y);
				float fSpeedX = 0.9;
				float fSpeedY = 0.9;
				if(m_scrollPtrH)
					fSpeedX = m_scrollPtrH->m_fScrollSpeed;
				if(m_scrollPtrV)
					fSpeedY = m_scrollPtrV->m_fScrollSpeed;
				if(m_pChildDialog->m_childMoveStrength.x>0 && x>0)
					m_pChildDialog->m_childMoveStrength.x *= fSpeedX*0.6;
				else if(m_pChildDialog->m_childMoveStrength.x<0&&x<(regionW-w))
					m_pChildDialog->m_childMoveStrength.x *= fSpeedX*0.6;
				if(m_pChildDialog->m_childMoveStrength.y>0 && y>0)
					m_pChildDialog->m_childMoveStrength.y *= fSpeedY*0.6;
				else if(m_pChildDialog->m_childMoveStrength.y<0&&y<(regionH-h))
					m_pChildDialog->m_childMoveStrength.y *= fSpeedY*0.6;
				m_pChildDialog->m_childMoveStrength.x *= fSpeedX;
				m_pChildDialog->m_childMoveStrength.y *= fSpeedY;
				if(abs(m_pChildDialog->m_childMoveStrength.x)<=1.0f)
				{
					m_pChildDialog->m_childMoveStrength.x = 0;
					bXOK = true;
				}
				if(abs(m_pChildDialog->m_childMoveStrength.y)<=1.0f)
				{
					m_pChildDialog->m_childMoveStrength.y = 0;
					bYOK = true;
				}
				if(bXOK&&bYOK)
					m_pChildDialog->m_childAutoMove = false;
			}
			m_pChildDialog->GetLocation(x,y);
			if(x>0||y>0||((x+w)<regionW&&x<0) || ((y+h)<regionH&&y<0))
			{
				if(m_scrollPtrH){
					if(x>0){
						bMinX = true;
						fScaleX = 1.0 - (float)x / (float)regionW;
						//
						int off = (float)x*0.35;
						if(off<10)off = 10;
						x -= off;
						if(x<0)x=0;
					}
					else if(((x+w)<regionW&&x<0))
					{
						int newx = regionW - (x+w);
						bMinX = false;
						fScaleX = 1.0 - (float)newx / (float)regionW;
						//
						int off = (float)newx*0.35;
						if(off<10)off = 10;
						newx -= off;
						if(newx<0)newx=0;
						x = regionW - newx - w;
					}
				}
				if(m_scrollPtrV){
					if(y>0){
						bMinY = true;
						fScaleY = 1.0f - (float)y / (float)regionH;
						int off = (float)y*0.35;
						if(off<10)off = 10;
						y -= off;
						if(y<0)y=0;
					}
					else if(((y+h)<regionH&&y<0))
					{
						int newy = regionH - (y+h);
						bMinY = false;
						fScaleY = 1.0 - (float)newy / (float)regionH;
						int off = (float)newy*0.35;
						if(off<10)off = 10;
						newy -= off;
						if(newy<0)newy=0;
						y = regionH - newy - h;
					}
				}
				if(m_pChildDialog->m_childDownIndex<0)
					m_pChildDialog->SetLocation(x,y);
			}
		}
		if(m_scrollPtrV)
		{
			if(m_scrollPtrV->m_bVisible)
			{
				fScaleY *= (float)regionH/(float)h*(float)regionH;
				fScaleY = getmax(fScaleY, 10);
				m_scrollPtrV->m_nThumbSize = fScaleY;
				//m_scrollPtrV->RenderScroll(this, g_manager.GetElapsedTime(), fScaleY);
			}
		}
		if(m_scrollPtrH)
		{
			if(m_scrollPtrH->m_bVisible)
			{
				fScaleX *= (float)regionW/(float)w*(float)regionW;
				fScaleX = getmax(fScaleX, 10);
				m_scrollPtrH->m_nThumbSize = fScaleX;
				//m_scrollPtrH->RenderScroll(this, g_manager.GetElapsedTime(), fScaleX);
			}
		}
		//if(this->m_bVert)
		//{
			//Rect rcScroll(this->m_windowRect
			//m_dialogPtr->DrawElement(GetEnabled(), GetDisableColor(), &m_bgElement, &m_bgElement.rcTexture, &m_currentAnim.rc, m_bgElement.textureColor * m_currentAnim.textureColor, true, &m_currentAnim, 0, 0, m_bMouseEnter && m_bMouseEnterEffect);
		//}
	}
	void UIControl::OnButtonClick(const Pixel& pt, int type)
	{CPUTime(UIControl);
		m_dialogPtr->RaiseEvent(this, type, pt);
		//
		//if(this->m_soundFile.t[0])//.strFile.empty())
		//{
  //          Global_playsnd(m_soundFile, 1, 0, 1);
		//}
		for(int i=0;i<m_drawMgrs.size();i++)
		{
			if(m_drawMgrs[i]->OnClick(pt, type))
				break;
		}
	}
	UIDrawMgr::UIDrawMgr()
	{
		m_dblClick = 0;
		m_texRect.Set(0,0,32,32);
		m_pixel.Set(0,0);
		m_center.Set(0, 0);
		memset(m_points, 0, sizeof(m_points));
		m_rotx = m_roty = m_rotz = 0;
		m_textureID = 0;
		m_layer = 0;
		m_brotx = m_broty = m_brotz = false;
		m_numberHeight = 64;
		m_numberSpace = 5;
		m_renderMode = 0;
		m_pNext	=	0;
		m_number = 0;
		m_uv[0].setxy(0, 0);
		m_uv[1].setxy(1, 0);
		m_uv[2].setxy(1, 1);
		m_uv[3].setxy(0, 1);
		m_fProgress	=	0;
		m_offsetZ	=	0;
		m_bVisible = true;
		m_bEnabled = true;
		m_color[0]=m_color[1]=m_color[2]=m_color[3] = 0xffffffff;
		m_offPixel.Set(0, 0);
		m_nHighlight	=	0;
		m_clickScale	=	1.0f;
		m_bClickDown	=	false;
		m_numberAlign	=	Phantom::DrawTextAlign_CENTER | Phantom::DrawTextAlign_VCENTER;
		m_data			=	0;
		m_bSetRect		=	false;
		m_cullMode		=	Phantom::CullMode_CW;
		m_renderRect.Set(0,0,0,0);
		m_scale.setxy(1,1);
		m_textLength	=	0;
		m_uvWidth=m_uvHeight=m_uvNumW=m_uvNumH=m_uvStart=m_uvCount=0;
		m_fuvSpeed=m_fuvPlayIndex=m_fuvPlayIndexLast=0;
	}

	VOID		UIDrawMgr::InitUVMap(int width,int height,int wcount,int hcount)
	{CPUTime(UIDrawMgr);
		m_uvWidth	=	width;
		m_uvHeight	=	height;
		m_uvNumW	=	wcount;
		m_uvNumH	=	hcount;
		m_uvStart	=	0;
		m_uvCount	=	wcount*hcount;
		m_fuvSpeed	=	10;
		m_fuvPlayIndex=0;
		m_fuvPlayIndexLast=10000000;
	}
	VOID		UIDrawMgr::InitUVAni(int start,int count,float speed)
	{CPUTime(UIDrawMgr);
		m_uvStart	=	start;
		m_uvCount	=	count;
		m_fuvSpeed	=	speed;
		m_fuvPlayIndex=0;
		m_fuvPlayIndexLast=10000000;
	}

	UIDrawMgr::~UIDrawMgr()
	{
		if(m_textureID>0)
			GetDlgManager()->DelTexture(m_textureID);
		for(int i=0;i<m_childs.size();i++)
		{
			safe_release(m_childs[i]);
		}
	}
	UIDrawMgr*	g_downMgr = 0;
	VOID		UIDrawMgr::OnUpClick(const Pixel& pt, int type)
	{CPUTime(UIDrawMgr);
		this->m_bClickDown = false;
	}
	BOOL		UIDrawMgr::OnDownClick(const Pixel& pt, int type)
	{CPUTime(UIDrawMgr);
		if(!this->m_bVisible || !this->m_bEnabled)
			return false;
		if(this->m_fnclick.t[0] && type == m_dblClick){
			//Pixel pos[4];
			//int xoff = m_offPixel.x;
			//int yoff = m_offPixel.y;
			//for(int i=0;i<4;i++)
			//{
			//	float3 v(this->m_points[i].x - this->m_center.x, this->m_points[i].y - this->m_center.y , 0);
			//	if(m_brotx)
			//		v.RotateX(v, m_rotx);
			//	if(m_broty)
			//		v.RotateY(v, m_roty);
			//	if(m_brotz)
			//		v.RotateZ(v, m_rotz);
			//	pos[i].Set((int)v.x + xoff, (int)v.y + yoff);
			//}
			//if(Phantom::IntersectTriangle(pt, pos[3], pos[1], pos[0]) || Phantom::IntersectTriangle(pt, pos[3], pos[2], pos[1]))
			if(m_renderRect.isPtIn(pt))
			{
				g_downMgr = this;
				m_bClickDown = true;
				return true;
			}
		}
		for(int i=0;i<m_childs.size();i++)
		{
			if(m_childs[i]->OnDownClick(pt, type))
				return true;
		}
		return false;
	}
	BOOL		UIDrawMgr::OnClick(const Pixel& pt, int type)
	{CPUTime(UIDrawMgr);
		this->m_bClickDown=false;
		if(g_downMgr != this)
			return false;
		g_downMgr = 0;
		if(!this->m_bVisible || !this->m_bEnabled)
			return false;
		if(this->m_fnclick.t[0] && type == m_dblClick){
			//Pixel pos[4];
			//int xoff = m_offPixel.x;
			//int yoff = m_offPixel.y;
			//for(int i=0;i<4;i++)
			//{
			//	float3 v(this->m_points[i].x - this->m_center.x, this->m_points[i].y - this->m_center.y , 0);
			//	if(m_brotx)
			//		v.RotateX(v, m_rotx);
			//	if(m_broty)
			//		v.RotateY(v, m_roty);
			//	if(m_brotz)
			//		v.RotateZ(v, m_rotz);
			//	pos[i].Set((int)v.x + xoff, (int)v.y + yoff);
			//}
			////static unsigned short indexBuffer[] ={3,1,0,3,2,1};
			//if(Phantom::IntersectTriangle(pt, pos[3], pos[1], pos[0]) || Phantom::IntersectTriangle(pt, pos[3], pos[2], pos[1]))
			if(m_renderRect.isPtIn(pt))
			{
				if(type == m_dblClick)
				{
					lua_UIDrawMgr_s mgr;
					SetIPtr(&mgr.m_data, this, UIDrawMgr, IUIDrawMgrPtr);
					lua_tinker::call<void>(g_luamanager.GetLuaState(), this->m_fnclick.t, mgr);
				}
				return true;
			}
		}
		for(int i=0;i<m_childs.size();i++)
		{
			if(m_childs[i]->OnClick(pt, type))
				return true;
		}
		return false;
	}
	VOID		UIDrawMgr::SetTexture(const char* szTexture)
	{CPUTime(UIDrawMgr);
		int old = m_textureID;
		m_textureID = GetDlgManager()->AddTexture(szTexture);
		if(!m_bSetRect){
			Texture* texture	=	GetDlgManager()->GetTextureNode(this->m_textureID, 0);
			if(texture){
				SetRect(0, 0, texture->GetWidth(), texture->GetHeight());
			}
		}
		if(old)
			GetDlgManager()->DelTexture(old);
	}
	VOID		UIDrawMgr::SetTextureRect(const Rect& rc)
	{CPUTime(UIDrawMgr);
		this->m_texRect = rc;
		Texture* texture	=	GetDlgManager()->GetTextureNode(this->m_textureID, 0);
		if(texture)
		{
			float ub = (float)m_texRect.left / (float)texture->GetWidth();
			float ue = (float)m_texRect.right / (float)texture->GetWidth();
			float vb = (float)m_texRect.top / (float)texture->GetHeight();
			float ve = (float)m_texRect.bottom / (float)texture->GetHeight();
			//
			m_uv[0].setxy(ub, vb);
			m_uv[1].setxy(ue, vb);
			m_uv[2].setxy(ue, ve);
			m_uv[3].setxy(ub, ve);
		}
	}
	VOID		UIDrawMgr::SetTextureMode(const char* szMode)
	{CPUTime(UIDrawMgr);
		const TextureRect* tr = GetDlgManager()->GetTextureRect(this->m_textureID, szMode);
		if(tr)
		{
			SetTextureRect(tr->rc);
			SetRect(tr->off.x, tr->off.y, tr->off.x+tr->rc.GetWidth(), tr->off.y+tr->rc.GetHeight());
		}
	}
	VOID		UIDrawMgr::RenderProgress(Texture* tex, Pixel* pos, int left, int right, int uvBegin, int uvEnd, int bBack)
	{CPUTime(UIDrawMgr);
		Pixel		points[4];//最多4个点
		float2		uv[4];
		float2 t = float2(pos[1].x - pos[0].x, pos[1].y - pos[0].y);
		float2 b = float2(pos[2].x - pos[3].x, pos[2].y - pos[3].y);
		int width = t.x;
		float2 tl = t*((float)left/(float)width)+float2(pos[0].x, pos[0].y);
		float2 tr = t*((float)right/(float)width)+float2(pos[0].x, pos[0].y);
		float2 bl = b*((float)left/(float)width)+float2(pos[3].x, pos[3].y);
		float2 br = b*((float)right/(float)width)+float2(pos[3].x, pos[3].y);
		points[0].Set(tl.x, tl.y);
		points[1].Set(tr.x, tr.y);
		points[2].Set(br.x, br.y);
		points[3].Set(bl.x, bl.y);
		//tex->GetWidth();
		float ub = (float)uvBegin / (float)tex->GetWidth();
		float ue = (float)uvEnd / (float)tex->GetWidth();
		float vb = 0;
		float ve = 0.5;
		if(!bBack)
		{
			vb = 0.5;
			ve = 1.0;
		}
		//
		uv[0].setxy(ub, vb);
		uv[1].setxy(ue, vb);
		uv[2].setxy(ue, ve);
		uv[3].setxy(ub, ve);
		g_manager.Draw2D(tex, points, uv, this->m_color, 4, m_nHighlight, 0, m_bEnabled, (CullMode)m_cullMode);
	}
	VOID		UIDrawMgr::Render(const Pixel& off, int renderMode, const char* text, float fScale2, const Pixel* offset)
	{CPUTime(UIDrawMgr);
		g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		g_manager->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_INVSRCALPHA);
		Pixel pos[4];
		float2 scale(m_scale.x*fScale2, m_scale.y*fScale2);
		m_renderRect.Set(10000,10000,-10000,-10000);
		for(int i=0;i<4;i++)
		{
			int px = m_points[i].x - this->m_center.x;
			int py = m_points[i].y - this->m_center.y;
			if(offset)
			{
				px += offset->x;
				py += offset->y;
			}
			float3 v((float)px*scale.x, (float)py * scale.y, 0);
			if(m_bClickDown)
			{
				v.x *= this->m_clickScale;
				v.y *= this->m_clickScale;
			}
			if(m_brotx)
				v.RotateX(v, m_rotx);
			if(m_broty)
				v.RotateY(v, m_roty);
			if(m_brotz)
				v.RotateZ(v, m_rotz);
			pos[i].Set((int)v.x + off.x, (int)v.y + off.y);
			px += off.x;
			py += off.y;
			if(px < m_renderRect.left)
				m_renderRect.left = px;
			if(px > m_renderRect.right)
				m_renderRect.right = px;
			if(py < m_renderRect.top)
				m_renderRect.top = py;
			if(py > m_renderRect.bottom)
				m_renderRect.bottom = py;
		}
		if(renderMode<0)
			renderMode = m_renderMode;

		switch(renderMode)
		{
		case DrawMgrMode_Number:
			{
				float2 scale(m_clickScale,m_clickScale);
				Pixel off(m_renderRect.left, m_renderRect.top);
				color4 c(m_color[0]);
				Rect rcc(0, 0, m_renderRect.GetWidth(), m_renderRect.GetHeight());
				RenderImageText(rcc, this->m_numberFirst.t, this->m_value.t, this->m_value.size(), m_textureID, this->m_numberAlign, c, scale, this->m_center, this->m_textIndexs, 
					this->m_numberHeight, this->m_numberSpace, &off, this->m_bEnabled, (CullMode)m_cullMode);
			}
			break;
		case DrawMgrMode_Progress:
			{
				Texture* texture	=	GetDlgManager()->GetTextureNode(this->m_textureID, 0);
				if(texture)
				{
					int zcx = (m_renderRect.left+m_renderRect.right)/2;
					int zcy = (m_renderRect.top+m_renderRect.bottom)/2;
					int zw = (int)((float)m_renderRect.GetWidth()/2 * fScale2);
					int zh = (int)((float)m_renderRect.GetHeight()/2 * fScale2);
					int oldW = m_renderRect.GetWidth();
					m_renderRect.Set(zcx-zw, zcy-zh,zcx+zw,zcy+zh);

					if(m_renderRect.right < -g_manager.GetUIOffset().x || m_renderRect.bottom < -g_manager.GetUIOffset().y || m_renderRect.left >= g_manager.GetUIBufferWidth() || m_renderRect.top >= g_manager.GetUIBufferHeight())
						return;
					float fScale = (float)m_renderRect.GetHeight() / (float)(texture->GetHeight()/2);
					int nTexW = (texture->GetWidth()/8);
					RenderProgress(texture, pos, 0, (float)nTexW*fScale, 0, nTexW, true);//m_renderRect.GetWidth()-(float)nTexW*fScale, 
					int width = m_renderRect.GetWidth();
					RenderProgress(texture, pos, width-(float)nTexW*fScale, width, texture->GetWidth()-nTexW, texture->GetWidth(), true);
					RenderProgress(texture, pos, nTexW*fScale, width-(float)nTexW*fScale, nTexW, texture->GetWidth()-nTexW, true);
					//
					int nProgress = this->m_fProgress * (float)width;
					int nTexR = width-(float)nTexW*fScale;
					if(nProgress <= nTexW)
					{
						RenderProgress(texture, pos, 0, (float)nProgress*fScale, 0, nProgress, false);//m_renderRect.GetWidth()-(float)nTexW*fScale, 
					}
					else if(nProgress <= nTexR)
					{
						RenderProgress(texture, pos, 0, (float)nTexW*fScale, 0, nTexW, false);//m_renderRect.GetWidth()-(float)nTexW*fScale, 
						RenderProgress(texture, pos, nTexW*fScale, nProgress, nTexW, texture->GetWidth()-nTexW, false);
					}
					else
					{
						RenderProgress(texture, pos, 0, (float)nTexW*fScale, 0, nTexW, false);//m_renderRect.GetWidth()-(float)nTexW*fScale, 
						int nR = nProgress - nTexR;
						RenderProgress(texture, pos, nTexR, nProgress, texture->GetWidth()-nTexW, texture->GetWidth()-nTexW+nR, false);
						RenderProgress(texture, pos, nTexW*fScale, nTexR, nTexW, texture->GetWidth()-nTexW, false);
					}
				}
			}
			break;
		case DrawMgrMode_Text:
			{
				Texture* texture	=	GetDlgManager()->GetTextureNode(this->m_textureID, 0);
				if(texture)
				{
					//m_renderRect.left += ((float)m_renderRect.GetWidth()*(1-fScale2)*0.3, 0);
					int len = strlen(text);
					m_renderRect.left -= (100 - len*2);
					m_renderRect.right += 100;
					if(m_renderRect.right < -g_manager.GetUIOffset().x || m_renderRect.bottom < -g_manager.GetUIOffset().y || m_renderRect.left >= g_manager.GetUIBufferWidth() || m_renderRect.top >= g_manager.GetUIBufferHeight())
						return;
					color4 tc(m_color[0]);
					Font* font = g_manager.GetDefaultUnicodeFont();
					if(!font)
						font = g_manager.GetDefaultENFont();
					font->SetFontScale(fScale2);
					font->SetTextColor(tc);
					if(text)
						font->DrawTexts(text, m_renderRect, m_textLength, Phantom::DrawTextAlign_CENTER);
					else
						font->DrawTexts(m_textW.t, m_renderRect, m_textLength, Phantom::DrawTextAlign_CENTER);
				}
			}
			break;
		default:
			{
				Texture* texture	=	GetDlgManager()->GetTextureNode(this->m_textureID, 0);
				if(texture)
				{
					if(m_renderRect.right < -g_manager.GetUIOffset().x || m_renderRect.bottom < -g_manager.GetUIOffset().y || m_renderRect.left >= g_manager.GetUIBufferWidth() || m_renderRect.top >= g_manager.GetUIBufferHeight())
						return;
					//
					if(m_uvCount>0&&m_uvNumW>0&&m_uvNumH>0){
						m_fuvPlayIndex+=m_fuvSpeed*g_manager.GetElapsedTime();
						int index = ((int)m_fuvPlayIndex%m_uvCount) + m_uvStart;
						int lastindex = ((int)m_fuvPlayIndexLast%m_uvCount) + m_uvStart;
						if(index != lastindex){
							m_fuvPlayIndexLast=m_fuvPlayIndex;
							int w = m_uvWidth / m_uvNumW;
							int h = m_uvHeight / m_uvNumH;
							int x = index%m_uvNumW;
							int y = index / m_uvNumW;
							int left = x * w;
							int top = y * h;
							int right = (x+1)*w;
							int bottom = (y+1)*h;
							float ub = (float)left / (float)texture->GetWidth();
							float ue = (float)right / (float)texture->GetWidth();
							float vb = (float)top / (float)texture->GetHeight();
							float ve = (float)bottom / (float)texture->GetHeight();
							m_uv[0].setxy(ub, vb);
							m_uv[1].setxy(ue, vb);
							m_uv[2].setxy(ue, ve);
							m_uv[3].setxy(ub, ve);
						}
					}
					//
					g_manager.Draw2D(texture, pos, this->m_uv, this->m_color, 4, m_nHighlight, 0, m_bEnabled, (CullMode)m_cullMode);
				}
			}
			break;
		}
	}
	VOID		UIDrawMgr::SetText(const char* szText)
	{CPUTime(UIDrawMgr);
		m_value = szText;
		m_textLength = strlen(m_value.t);
		int index	=	0;
		for(int i=0;i<m_textLength;i++)
		{
			int n = 0;
			this->m_textW.t[index++] = utf8ToUnicode((const unsigned char*)szText + i, &n);
			i	+=	(n - 1);
		}
		m_textW.t[index++] = 0;
		m_textLength = m_textW.size();
		this->m_renderMode = this->DrawMgrMode_Text;
	}
	VOID		UIDrawMgr::Render3D(const float3* inPos, int renderMode, const char* text, float fScale, const Pixel* offset)
	{CPUTime(UIDrawMgr);
		Pixel out;
		float3 pos = this->m_pos3d;
		if(inPos)
			pos = *inPos;
		pos.z += m_offsetZ;
		if(renderMode<0)
			renderMode = m_renderMode;
		if(GetCameraPtr()->Get2DFrom3D(out, pos))
		{
			float xoff;
			float yoff;
			xoff = (out.x + m_center.x + this->m_pixel.x);
			yoff = (out.y + m_center.y + this->m_pixel.y);
			if(renderMode==this->DrawMgrMode_Text)
			{
			}
			else
			{
				xoff /= g_manager.GetUIScale().x;
				yoff /= g_manager.GetUIScale().y;
			}
			m_offPixel.Set(xoff, yoff);
			g_manager->SetEnableZ(false);
			g_manager->SetZWriteEnable(false);
			Render(m_offPixel, renderMode, text, fScale, offset);
			g_manager->SetZWriteEnable(true);
			g_manager->SetEnableZ(true);
			Pixel child = m_pixel;
			for(int i=0;i<m_childs.size();i++)
			{
				if(m_childs[i]->m_bVisible)
				{
					m_childs[i]->Render3D(&m_pos3d);
				}
			}
		}
	}
	VOID		UIDrawMgr::Render(const matrix4x4& world, const Pixel* offset)
	{CPUTime(UIDrawMgr);
		m_pos3d = world.GetPosition();
		g_manager.Push3DDrawMgr(this, m_pos3d);
	}
	VOID		UIDrawMgr::Render(UIDialog* dlg, const Rect& rect, const Pixel* offset)
	{CPUTime(UIDrawMgr);
		int xoff;
		int yoff;
		if(dlg)
		{
			xoff = rect.left + m_center.x + this->m_pixel.x;
			yoff = rect.top + m_center.y + this->m_pixel.y;
			xoff += dlg->m_dlgX;
			yoff += dlg->m_dlgY;
		}
		else
		{
			xoff = (rect.left + rect.right)/2 + m_center.x + this->m_pixel.x;
			yoff = rect.top + m_center.y + this->m_pixel.y;
		}
		if(offset)
		{
			xoff += offset->x;
			yoff += offset->y;
		}
		m_offPixel.Set(xoff, yoff);
		Render(m_offPixel);
		Pixel child = m_pixel;
		if(offset)
			child += *offset;
		for(int i=0;i<m_childs.size();i++)
		{
			if(m_childs[i]->m_bVisible)
			{
				m_childs[i]->Render(dlg, rect, &child);
			}
		}
	}
	VOID		UIDrawMgr::SetRect(int left, int top, int right, int bottom)
	{CPUTime(UIDrawMgr);
		m_bSetRect	=	true;
		m_points[0].Set(left, top);
		m_points[1].Set(right, top);
		m_points[2].Set(right, bottom);
		m_points[3].Set(left, bottom);
	}
	VOID		UIDrawMgr::SetNumberMode(const char* szFirst, int numberHeight, int space)
	{CPUTime(UIDrawMgr);
		m_renderMode = DrawMgrMode_Number;
		m_numberFirst = szFirst;
		m_numberHeight = numberHeight;
		m_numberSpace = space;
	}
	VOID		UIDrawMgr::SetNumber(int num)
	{CPUTime(UIDrawMgr);
		m_number = num;
		sprintf(this->m_value.t, "%d", num);
		m_textIndexs.SetArrayCount(0);
	}
	VOID		UIDrawMgr::SetRotationX(int f)
	{CPUTime(UIDrawMgr);
		m_brotx = (f!=0);
		m_rotx = Radians(f);
	}
	VOID		UIDrawMgr::SetRotationY(int f)
	{CPUTime(UIDrawMgr);
		m_broty = (f!=0);
		m_roty = Radians(f);
	}
	VOID		UIDrawMgr::SetRotationZ(int f)
	{CPUTime(UIDrawMgr);
		m_brotz = (f!=0);
		m_rotz = Radians(f);
	}
	UIDrawMgr*	UIDrawMgr::AddChild()
	{CPUTime(UIDrawMgr);
		UIDrawMgr* c = new UIDrawMgr();
		c->m_textureID = m_textureID;
		if(c->m_textureID > 0)
			GetDlgManager()->AddTextureRef(m_textureID);
		//
		c->m_texRect = m_texRect;
		c->m_pixel.Set(0,0);
		c->m_center = m_center;
		c->m_cullMode = m_cullMode;
		memcpy(c->m_points, m_points, sizeof(m_points));
		memcpy(c->m_uv, m_uv, sizeof(m_uv));
		memcpy(c->m_color, m_color, sizeof(m_color));
		c->m_fnclick = m_fnclick;//脚本函数，按下时引发
		c->m_rotx = m_rotx;
		c->m_roty = m_roty;
		c->m_rotz = m_rotz;
		c->m_brotx = m_brotx;
		c->m_broty = m_broty;
		c->m_brotz = m_brotz;
		c->m_bVisible = m_bVisible;
		c->m_bEnabled = m_bEnabled;
		c->m_renderMode = m_renderMode;
		c->m_numberFirst = m_numberFirst;
		c->m_numberHeight = m_numberHeight;
		c->m_numberSpace = m_numberSpace;
		c->m_offPixel = m_offPixel;
		c->m_number = m_number;
		c->m_numberAlign = m_numberAlign;
		c->m_value = m_value;;//如果是数字则显示值
		m_childs.push_back(c);
		return c;
	}
	VOID		UIDrawMgr::DeleteChild(int n)
	{CPUTime(UIDrawMgr);
		if(n<m_childs.size() && n>=0)
		{
			delete m_childs[n];
			m_childs.eraseByIndex(n);
		}
	}

};
