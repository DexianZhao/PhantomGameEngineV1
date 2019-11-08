//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIEffect.h"
#include "PhantomUIManager.h"
#include "PhantomManager.h"

namespace Phantom{

	UIEffect::UIEffect( BOOL bInit, UIDialog *dialogPtr ) : UIControl(bInit, dialogPtr)
	{
		m_controlType	= UIControlType_ui2DEffect;
		m_dialogPtr		= dialogPtr;
		m_frameCount	=	10;
		m_fPlaySpeed	=	1.0f;
		m_fLastCount	=	0;
		m_bLButtonDown	=	false;
		m_saveRect.Set(0,0,0,0);
		m_saveColor.setRGBA(1,1,1,1);
		m_fLastTime = 0;
		//
		if(bInit)
		{
			m_bgElement.SetSourceRect(Rect(83, 0, 83 + 30, 30));
			m_bgElement.SetRenderScale(true, false, Pixel(5, 5));
//			m_bgElement.m_name = ("效果图片");
			for(int i=0;i<EffectRandom_Count;i++)
			{
				m_randoms[i].SetTrackKeyCount(m_frameCount);
				m_randoms[i].SetEnabled(false);
			}
			m_randoms[EffectRandom_EmitCount].SetEnabled(true);
			m_randoms[EffectRandom_LifeMin].SetEnabled(true);
			m_randoms[EffectRandom_LifeMax].SetEnabled(true);
			m_randoms[EffectRandom_EmitCount].SetDefaultValue(1.0f);
			m_randoms[EffectRandom_LifeMin].SetDefaultValue(0.1f);
			m_randoms[EffectRandom_LifeMax].SetDefaultValue(1.0f);
			m_randoms[EffectRandom_SizeMin].SetDefaultValue(0.1f);
			m_randoms[EffectRandom_SizeMax].SetDefaultValue(1.0f);
		}
		m_loopCount		=	-1;
		//
		ClearElement();
		AddElement(&m_bgElement);
		//
		m_borderWidth	=	0;
		m_fTime			=	0;
		m_bStop = false;
	}

	UIEffect::~UIEffect()
	{
		for(int i=0;i<this->m_particles.size();i++)
		{
			GetDlgManager()->Free(m_particles[i]);
		}
		m_particles.clear();
	}

	VOID UIEffect::CopyFrom(UIControl* e)
	{CPUTime(UIEffect);
		UIControl::CopyFrom(e);
		if(e->GetType() == this->GetType())
		{
			UIControl* c	=	static_cast<UIControl*>(e);
			UIEffect* eff	=	static_cast<UIEffect*>(c);
			for(int i=0;i<EffectRandom_Count;i++)
				m_randoms[i].CloneFrom(&eff->m_randoms[i]);
			m_frameCount	=	eff->m_frameCount;
			this->m_loopCount = eff->m_loopCount;
		}
	}

	char UIEffect::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex )
	{CPUTime(UIEffect);
		if( !m_bVisible || this->m_bIsBackground )
			return false;
		if(!this->IsEnabledCtrl() && this->canHaveFocus())
		{
			switch(uMsg)
			{
			case InputEventID_ButtonDown:
			case InputEventID_ButtonDblClk:
			case InputEventID_MouseMove:
				return isPtIn( pt );
			default:
				return false;
			}
		}

		switch(uMsg)
		{
		case InputEventID_MouseMove:
			{
				char bMouseMove	=	isPtIn(pt);
				if(bMouseMove)
				{
					if(!m_bMouseEnter)
						this->onMouseEnter();
				}
			}
			break;
		case InputEventID_ButtonDown:
		case InputEventID_ButtonDblClk:
			{
				if(isPtIn(pt))
					m_bLButtonDown	=	(touchIndex + 1);
				if(m_bLButtonDown > 0 && isPtIn(pt))
				{
					OnButtonClick(pt, EventID_Click);
				}
				m_bLButtonDown	=	false;
			}
			break;
		}
		return UIControl::onMouseMessage( uMsg, pt, touchIndex );
	}

	VOID UIEffect::OnPlayStart()
	{CPUTime(UIEffect);
		this->m_fTime	=	0;
		m_fLastCount	=	0;
		m_bStop			=	false;
		m_saveRect		=	this->m_currentAnim.rc;
		m_saveColor		=	this->m_currentAnim.textureColor;
		m_fLastTime		=	m_fTime;
	}

	void UIEffect::Render( float fElapsedTime )
	{CPUTime(UIEffect);
		if( m_bVisible == false )
			return;
		if(!this->m_trackPtr)
			return;
		m_fTime			+=	fElapsedTime;
		float fCount	=	this->m_randoms[EffectRandom_EmitCount].GetFrameValue(m_fTime) + m_fLastCount;
		BOOL bAdd		=	(fCount >= 1.0f);
		if(m_loopCount > 0)
		{
			if(m_fTime >= (float)m_loopCount * ((float)this->GetFrameCount()  / (float)TRACK_SECOND_LENGTH) )
				bAdd	=	false;
		}
		if(bAdd && !m_bStop)//fCount >= 1.0f && m_fTime < (float)this->GetFrameCount())
		{
			m_fLastCount		=	0;
			for(int i=0;i<(int)fCount;i++)
			{
				Particle2D* part	=	GetDlgManager()->Alloc();
				if(!part)
					continue;
				InitParticle(part, (float)i/fCount);
				m_particles.push_back(part);
			}
			m_saveRect		=	this->m_currentAnim.rc;
			m_saveColor		=	this->m_currentAnim.textureColor;
			m_fLastTime		=	m_fTime;
		}
		else
			m_fLastCount	+=	fCount;
		//
		g_manager->SetBlendMode((BlendMode)m_bgElement.m_nSrcBlend,(BlendMode)m_bgElement.m_nDestBlend);
		//
		Texture* texture	=	GetDlgManager()->GetTextureNode(m_bgElement.GetTextureNewID(), this->m_dialogPtr);
		if(!texture)
			return;
		for(int i=0;i<m_particles.size();i++)
		{
			Particle2D* part	=	m_particles[i];
			part->fLife			-=	fElapsedTime;
			if(part->fLife <= 0)
			{
				m_particles.erase(m_particles.begin() + i);
				GetDlgManager()->Free(part);
				i--;
				continue;
			}
			UIAnimationTrack* track	=	m_trackPtr;//m_trackList[part->index];
			Particle2D render	=	*part;
			//
			float fLerp	=	(1.0f - (part->fLife / part->fMaxLife)) * (float)track->GetFrameCount();
			color4 textureColor = part->saveColor;
			if(track->m_floats[ControlTrackType_TextureColorR].IsEnabled())
			{
				textureColor.r *= track->m_floats[ControlTrackType_TextureColorR].GetFrameValue(fLerp);
				textureColor.g *= track->m_floats[ControlTrackType_TextureColorG].GetFrameValue(fLerp);
				textureColor.b *= track->m_floats[ControlTrackType_TextureColorB].GetFrameValue(fLerp);
			}
			if(track->m_floats[ControlTrackType_TextureColorA].IsEnabled())
				textureColor.a = track->m_floats[ControlTrackType_TextureColorA].GetFrameValue(fLerp);
			//
			Rect	rcRender;//	=	part->saveRect;
			Pixel middleSize(m_bgElement.textureRect.GetWidth() / 2, m_bgElement.textureRect.GetHeight() / 2);	//	=	this->m_rotCenter;//(Pixel&)Pixel(track->m_floats[ControlTrackType_CenterX), track->m_floats[ControlTrackType_CenterY));
			//
			//
			Pixel centerPos	=	m_currentAnim.center;//
			Pixel saveCenter	=	Pixel((int)part->saveCenter.x, (int)part->saveCenter.y);
			//
			int offsetX = 0;
			int offsetY = 0;
			if(track->m_floats[ControlTrackType_RectX].IsEnabled())
			{
				offsetX		=	track->m_floats[ControlTrackType_RectX].GetFrameValue(fLerp);
				//saveCenter.x	+=	offsetX;
			}
			if(track->m_floats[ControlTrackType_RectX].IsEnabled())
			{
				offsetY			=	track->m_floats[ControlTrackType_RectY].GetFrameValue(fLerp);
				//saveCenter.y		+=	offsetY;
			}
			//
			float3 rots(offsetX, render.fOffset * fElapsedTime + offsetY, 0);
			rots.RotateZ(rots, render.fRot + m_currentAnim.rot);
			part->saveCenter.x	+=	rots.x;
			part->saveCenter.y	+=	rots.y;
			//if(track->m_floats[ControlTrackType_RectW].IsEnabled())
			//			rcRender.right	+=	track->m_floats[ControlTrackType_RectW].GetFrameValue(fLerp);
			//if(track->m_floats[ControlTrackType_RectH].IsEnabled())
			//			rcRender.bottom	+=	track->m_floats[ControlTrackType_RectH].GetFrameValue(fLerp);
			//
			//
			if(track->m_floats[ControlTrackType_CenterX].IsEnabled())
			{
				centerPos.x	+=	track->m_floats[ControlTrackType_CenterX].GetFrameValue(fLerp);
				centerPos.y	+=	track->m_floats[ControlTrackType_CenterY].GetFrameValue(fLerp);
			}
			//
			float2 scale(render.fSize, render.fSize);
			int rw = 0;
			int rh = 0;
			if(track->m_floats[ControlTrackType_RectW].IsEnabled())
				rw =	track->m_floats[ControlTrackType_RectW].GetFrameValue(fLerp)/2;
			if(track->m_floats[ControlTrackType_RectH].IsEnabled())
				rh =	track->m_floats[ControlTrackType_RectH].GetFrameValue(fLerp)/2;
			//
			rcRender.Set(saveCenter.x - (int)((float)middleSize.x * scale.x) - rw, saveCenter.y - (int)((float)middleSize.y * scale.y)-rh, 
				saveCenter.x + (int)((float)middleSize.x * scale.x)+rw, saveCenter.y + (int)((float)middleSize.y * scale.y)+rh);
			//
			float fRotation		=	0;
			if(track->m_floats[ControlTrackType_Rotation].IsEnabled())
				fRotation		+=	track->m_floats[ControlTrackType_Rotation].GetFrameValue(fLerp);
			//
			if(textureColor.a <= 0.001f || rcRender.right <= rcRender.left || rcRender.bottom <= rcRender.top)
				continue;
			Rect rcTexture;
			rcTexture			=	m_bgElement.textureRect;
			int maxWidth		=	rcRender.right - rcRender.left;
			int maxHeight		=	rcRender.bottom - rcRender.top;
			int textureWidth	=	m_bgElement.textureRect.right - m_bgElement.textureRect.left;
			int textureHeight	=	m_bgElement.textureRect.bottom - m_bgElement.textureRect.top;
			if(textureWidth <= 0 || textureHeight <= 0)
				continue;
			unsigned int c		=	textureColor;
			//
			float3* pCenter	=	0;
			matrix4x4 matRotation;
			float3 center;
			Pixel* windowPos = 0;
			matrix4x4* pMatRot = 0;
			if(this->m_bEnableRotation)
			{
				pCenter		=	&center;
				center		=	float3(centerPos.x, centerPos.y, 0);
				matrixRotationZ(&matRotation, Radians(fRotation));
				pMatRot		=	&matRotation;
			}
			//
			//if(m_bgElement.m_bRenderScale)
			{
				float fScaleX 		= 1.0f;
				float fScaleY 		= 1.0f;
				fScaleX				*=	((float)maxWidth / (float)textureWidth);
				fScaleY				*=	((float)maxHeight / (float)textureHeight);
				matrix4x4 matTransform;
				matrixScaling( &matTransform, fScaleX, fScaleY, 1.0f );
				float3 tempCenter(0,0,0);
				float left			=	(float)rcRender.left + (float)m_bgElement.m_imgOffset.x * fScaleX;
				float top			=	(float)rcRender.top + (float)m_bgElement.m_imgOffset.y * fScaleY;
				if(pMatRot)
				{
					float3			tempCenter;
					pCenter		=	&tempCenter;
					tempCenter	=	center;
					tempCenter.x	/=	fScaleX;
					tempCenter.y	/=	fScaleY;
					matTransform._41 = -(float)pCenter->x;
					matTransform._42 = -(float)pCenter->y;
					matrixMultiply(&matTransform, &matTransform, pMatRot);
					matTransform._41 = (float)left + center.x;
					matTransform._42 = (float)top + center.y;
					pCenter			=	&tempCenter;
				}
				else
				{
					matTransform._41 = (float)left;
					matTransform._42 = (float)top;
				}
				color4 clr(c);
				g_manager.DrawSprite2D(texture, rcTexture, pCenter, &clr, &matTransform);
			}
		}
		this->RenderDrawMgrs();
	}

	char		UIEffect::LoadControl(CSafeFileHelperR& r)
	{CPUTime(UIEffect);
		UIControl::LoadControl( r );
		skip_r sr;
		if(m_loadVersion>=0x000000A4)
			sr.begin(r.pStream);
		int version = 1;
		r >> version;
		int count	=	EffectRandom_Count;
		r >> count;
		for(int i=0;i<count;i++)
		{
			if(i < EffectRandom_Count)
				m_randoms[i].Load(r);
			else
			{
				FloatAnimation f;
				f.Load(r);
			}
		}
		if(version >= 2)
			r >> m_loopCount;
		sr.end();
		return true;
	}
	char		UIEffect::SaveControl(CSafeFileHelperW& w)
	{CPUTime(UIEffect);
		UIControl::SaveControl( w );
		skip_w sw(w.pStream);
		int version = 2;
		w << version;
		int count	=	EffectRandom_Count;
		w << count;
		for(int i=0;i<count;i++)
			m_randoms[i].Save(w);
		w << m_loopCount;
		sw.end();
		return true;
	}

	VOID		UIEffect::InitParticle(Particle2D* p, float fTimeLerp)
	{CPUTime(UIEffect);
		memset(p, 0, sizeof(Particle2D));
		//
		Rect rc = m_currentAnim.rc;
		Pixel center(m_rotCenter.x + rc.left, m_rotCenter.y + rc.top);
		rc.left = (int)((float)(rc.left - center.x) * m_scale.x) + center.x;
		rc.right = (int)((float)(rc.right - center.x) * m_scale.x) + center.x;
		rc.top = (int)((float)(rc.top - center.y) * m_scale.y) + center.y;
		rc.bottom = (int)((float)(rc.bottom - center.y) * m_scale.y) + center.y;

		float fLeft		=	(float)(rc.left - m_saveRect.left) * fTimeLerp + (float)m_saveRect.left;
		float fRight	=	(float)(rc.right - m_saveRect.right) * fTimeLerp + (float)m_saveRect.right;
		float fTop		=	(float)(rc.top - m_saveRect.top) * fTimeLerp + (float)m_saveRect.top;
		float fBottom	=	(float)(rc.bottom - m_saveRect.bottom) * fTimeLerp + (float)m_saveRect.bottom;
		p->saveCenter.x	=	(int)RandomF(fLeft, fRight);
		p->saveCenter.y	=	(int)RandomF(fTop, fBottom);
		p->fRot			=	0;
		//
		float fLerp		=	fmodf((m_fLastTime+fTimeLerp*(m_fTime-m_fLastTime)) * this->GetPlaySpeed(), (float)this->GetFrameCount() / (float)TRACK_SECOND_LENGTH);
		if(m_randoms[EffectRandom_TextureColorMin].IsEnabled() || m_randoms[EffectRandom_TextureColorMax].IsEnabled())
		{
			p->saveColor	=	(m_currentAnim.textureColor - m_saveColor) * fTimeLerp + m_saveColor;
			p->saveColor.r	*= RandomF(m_randoms[EffectRandom_TextureColorMin].GetFrameValue(fLerp), m_randoms[EffectRandom_TextureColorMax].GetFrameValue(fLerp));
			p->saveColor.g	*= RandomF(m_randoms[EffectRandom_TextureColorMin].GetFrameValue(fLerp), m_randoms[EffectRandom_TextureColorMax].GetFrameValue(fLerp));
			p->saveColor.b	*= RandomF(m_randoms[EffectRandom_TextureColorMin].GetFrameValue(fLerp), m_randoms[EffectRandom_TextureColorMax].GetFrameValue(fLerp));
		}
		else
			p->saveColor	=	m_currentAnim.textureColor;
		p->saveColor.a	=	this->m_textureColor.a;
		if(m_randoms[EffectRandom_OffsetMin].IsEnabled() || m_randoms[EffectRandom_OffsetMax].IsEnabled())
			p->fOffset = RandomF(m_randoms[EffectRandom_OffsetMin].GetFrameValue(fLerp), m_randoms[EffectRandom_OffsetMax].GetFrameValue(fLerp));
		//
		if(m_randoms[EffectRandom_SizeMin].IsEnabled() || m_randoms[EffectRandom_SizeMax].IsEnabled())
			p->fSize = RandomF(m_randoms[EffectRandom_SizeMin].GetFrameValue(fLerp), m_randoms[EffectRandom_SizeMax].GetFrameValue(fLerp));
		else
			p->fSize	=	1;
		if(m_randoms[EffectRandom_RotationMin].IsEnabled() || m_randoms[EffectRandom_RotationMax].IsEnabled())
			p->fRot += RandomF(m_randoms[EffectRandom_RotationMin].GetFrameValue(fLerp), m_randoms[EffectRandom_RotationMax].GetFrameValue(fLerp));
		if(m_randoms[EffectRandom_LifeMin].IsEnabled() || m_randoms[EffectRandom_LifeMax].IsEnabled())
			p->fLife	=	p->fMaxLife	=	RandomF(m_randoms[EffectRandom_LifeMin].GetFrameValue(fLerp), m_randoms[EffectRandom_LifeMax].GetFrameValue(fLerp));
		else
			p->fLife	=	p->fMaxLife =	0;
		//p->fLife		+=	RandomF(0,0.5)*p->fMaxLife;
	}

	BOOL		UIEffect::IsClipIn()	//是否包含进去了
	{CPUTime(UIEffect);
		return true;
	}

	VOID		UIEffect::SetFrameCount(INT nCount)
	{CPUTime(UIEffect);
		m_frameCount = getmax(nCount, 10);
		for(int i=0;i<EffectRandom_Count;i++)
		{
			m_randoms[i].SetTrackKeyCount(m_frameCount);
		}
	}

};
