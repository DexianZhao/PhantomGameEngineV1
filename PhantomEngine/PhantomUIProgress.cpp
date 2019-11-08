//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIProgress.h"
#include "PhantomUIDialog.h"
#include "PhantomUIManager.h"
#include "PhantomManager.h"

namespace Phantom{


	UIProgress::UIProgress( BOOL bInit, UIDialog *dialogPtr ) : UIControl(bInit, dialogPtr)
	{
		m_chInnerSize		=	0;
		this->m_bParentMove	=	false;
		m_fProgress			=	1.0f;
		m_controlType		=	UIControlType_PROGRESS;
		m_dialogPtr			=	dialogPtr;
		//
		ClearElement();
		for(int i=0;i<MaxProgressElement;i++)
			AddElement(&m_pgElement[i]);
		if(bInit)
		{
			m_pgElement[0].SetSourceRect(Rect(110, 52, 110 + 16, 52 + 11));
			m_pgElement[0].SetRenderScale(true, false, Pixel(4, 4));
			m_pgElement[1].SetSourceRect(Rect(129, 52, 129 + 38, 52 + 11));
			m_pgElement[1].SetRenderScale(true, false, Pixel(10, 5));
			//
//			m_pgElement[0].m_name = ("进度条[背景]");
//			m_pgElement[1].m_name = ("进度条[值]");
		}
		m_bLButtonDown		=	false;
		m_fMaxTime = m_fNowTime = 0;
		m_bAutoTime = false;
	}
	VOID	UIProgress::SetAutoTime(float fTime)
	{CPUTime(UIProgress);
		m_fMaxTime = fTime;
		m_fNowTime = 0;
		m_bAutoTime = true;
	}
	VOID	ClampLength(screenVertex3D& v, float fMiddleX, float fMiddleY, float fWidth, float fHeight)
	{
		float diffX	=	v.x - fMiddleX;
		if(diffX > fWidth)
		{
			v.y	=	(fWidth / (v.x - fMiddleX)) * (v.y - fMiddleY) + fMiddleY;
			v.x	=	fMiddleX + fWidth;
		}
		else if(diffX < -fWidth)
		{
			v.y	=	(-fWidth / (v.x - fMiddleX)) * (v.y - fMiddleY) + fMiddleY;
			v.x	=	fMiddleX - fWidth;
		}
		float diffY	=	v.y - fMiddleY;
		if(diffY > fHeight)
		{
			v.x	=	(fHeight / (v.y - fMiddleY)) * (v.x - fMiddleX) + fMiddleX;
			v.y	=	fMiddleY + fHeight;
		}
		else if(diffY < -fHeight)
		{
			v.x	=	(-fHeight / (v.y - fMiddleY)) * (v.x - fMiddleX) + fMiddleX;
			v.y	=	fMiddleY - fHeight;
		}
	}
	void 	UIProgress::SetValue( int nValue )
	{
		m_fProgress = (float)(nValue - m_minValue) / (m_maxValue - m_minValue);
		if(m_fProgress < 0.0f)
			m_fProgress = 0.0f;
		else if(m_fProgress > 1.0f)
			m_fProgress = 1.0f;
		this->m_bChanged = true;
	}
	VOID	UIProgress::SetProgressValue(float f){
		m_fProgress = clamp(f, 0, 1);
		OnChangeValue();
	}
	VOID	UIProgress::OnChangeValue(){
		m_value = (int)(m_fProgress * (float)(m_maxValue - m_minValue)) + m_minValue;
		this->m_bChanged = true;
	}

	VOID UIProgress::DrawProgress(const Rect& rcIn, float fProgress, unsigned int color)
	{CPUTime(UIProgress);
		Rect rc = rcIn;
		Rect rcProg = rc;
		rcProg.inflate(m_chInnerSize,m_chInnerSize);
		if(rcProg.top>=rcProg.bottom||rcProg.left>=rcProg.right)
			return;
		color4 textureColor(color);
		UIElement* element1	=	&m_pgElement[0];
		m_dialogPtr->DrawElement(GetEnabled(), element1, &element1->rcTexture, &rc, element1->textureColor * m_currentAnim.textureColor, &m_currentAnim, this);
		UIElement* element2	=	&m_pgElement[1];
		if(m_nProgressType != UIProgressType_SKILL)
		{
			BOOL bHorzProgress		=	(m_nProgressType != UIProgressType_VERT);
			color4 c = element2->textureColor * textureColor;
			Rect rcTexture = element2->rcTexture;
			if(bHorzProgress)
			{
				int nWidth		=	rcProg.right - rc.left;
				rcProg.right	=	rcProg.left + (int)((float)nWidth * fProgress);
			}
			else
			{
				int nHeight		=	rcProg.bottom - rcProg.top;
				rcProg.top	=	rcProg.bottom - (int)((float)nHeight * fProgress);
				if(!element2->m_bRenderScale)
				{
					rcTexture.top	=	rcTexture.bottom - (int)((float)(rcTexture.bottom - rcTexture.top) * fProgress);
				}
			}
			m_dialogPtr->DrawElement(GetEnabled(), element2, &rcTexture, &rcProg, c, &this->m_currentAnim, this, 0);
		}
	}
	void UIProgress::Render( float fElapsedTime )
	{CPUTime(UIProgress);
		if( m_bVisible == false )
			return;
		if(m_bAutoTime){
			m_fNowTime+=fElapsedTime;
			if(m_fNowTime>=m_fMaxTime)
			{
				m_fNowTime = m_fMaxTime;
				m_bAutoTime = false;
				m_bVisible = false;
			}
			this->SetProgressValue(m_fNowTime / m_fMaxTime);
		}
		Rect rc = m_currentAnim.rc;
		Pixel center(m_rotCenter.x + rc.left, m_rotCenter.y + rc.top);
		rc.left = (int)((float)(rc.left - center.x) * m_scale.x) + center.x;
		rc.right = (int)((float)(rc.right - center.x) * m_scale.x) + center.x;
		rc.top = (int)((float)(rc.top - center.y) * m_scale.y) + center.y;
		rc.bottom = (int)((float)(rc.bottom - center.y) * m_scale.y) + center.y;
		if(this->m_pgElement[0].GetTextureNewID() >= 0||m_pgElement[0].m_bNoTexture)
		{
			UIElement* element	=	&m_pgElement[0];
			m_dialogPtr->DrawElement(GetEnabled(), element, &element->rcTexture, &rc, element->textureColor * m_currentAnim.textureColor, &m_currentAnim, this, 0);
		}
		UIElement* element	=	&m_pgElement[1];
		if(m_nProgressType != UIProgressType_SKILL)
		{
			BOOL bHorzProgress		=	(m_nProgressType != UIProgressType_VERT);
			color4 c = element->textureColor * m_currentAnim.textureColor;
			Rect rcTexture = element->rcTexture;
			Rect rcProg = rc;
			rcProg.inflate(m_chInnerSize,m_chInnerSize);
			if(bHorzProgress)
			{
				int nWidth		=	rcProg.right - rc.left;
				rcProg.right	=	rcProg.left + (int)((float)nWidth * m_fProgress);
			}
			else
			{
				int nHeight		=	rcProg.bottom - rcProg.top;
				rcProg.top	=	rcProg.bottom - (int)((float)nHeight * m_fProgress);
				if(!element->m_bRenderScale)
				{
					rcTexture.top	=	rcTexture.bottom - (int)((float)(rcTexture.bottom - rcTexture.top) * m_fProgress);
				}
			}
			m_dialogPtr->DrawElement(GetEnabled(), element, &rcTexture, &rcProg, c, &this->m_currentAnim, this);
		}
		else
		{
			Texture* pTextureNode = GetDlgManager()->GetTextureNode( element->GetTextureNewID(), m_dialogPtr );
			if( pTextureNode == NULL )
				return;
			color4 cc = element->textureColor * this->m_currentAnim.textureColor * m_dialogPtr->GetDialogColor();
			cc.a *= this->GetGroupPtr()->m_fOpacity;
			unsigned int c			=	cc;
			float fLeftU	= (float)element->rcTexture.left / (float)pTextureNode->GetWidth();
			float fTopV		= (float)element->rcTexture.top / (float)pTextureNode->GetHeight();
			float fRightU	= (float)element->rcTexture.right / (float)pTextureNode->GetWidth();
			float fBottomV	= (float)element->rcTexture.bottom / (float)pTextureNode->GetHeight();
			//Rect rc			=	m_currentAnim.rc;
			rc.Offset( this->m_dialogPtr->GetLeft(), m_dialogPtr->GetTop() );
			Pixel middle((rc.right - rc.left) / 2, (rc.bottom - rc.top) / 2);
			float	radius	=	360.0f * this->m_fProgress;
			float	length	=	1000.0f;
			//
			float fScaleX 		= 1.0f;
			float fScaleY 		= 1.0f;
			matrix4x4 matTransform;
			matTransform	=	matrix4x4::s_identity();
			//
			float3* pCenter	=	0;
			matrix4x4 matRotation;
			float3 center;
			matrix4x4* pMatRot = 0;
			if(this->m_currentAnim.bRotateEnable)
			{
				pCenter		=	&center;
				center		=	float3(m_currentAnim.center.x + m_currentAnim.rc.left - rc.left, m_currentAnim.center.y + m_currentAnim.rc.top - rc.top, 0);
				matrixRotationZ(&matRotation, Radians(m_currentAnim.rot));
				pMatRot		=	&matRotation;
			}
			if(pMatRot)
			{
				matTransform._41 = -(float)pCenter->x;
				matTransform._42 = -(float)pCenter->y;
				matrixMultiply(&matTransform, &matTransform, pMatRot);
				matTransform._41 = (float)rc.left + matTransform._41 + pCenter->x;// + (rcText->left - windowRect.left);
				matTransform._42 = (float)rc.top + matTransform._42 + pCenter->y;// + (rcText->top - windowRect.top);
			}
			else
			{
				matTransform._41 = (float)rc.left;
				matTransform._42 = (float)rc.top;
			}
			//
			for(int i=0;i<8;i++)
			{
				if(radius >= (float)(i + 1) * 45.0f)
					continue;
				float fRR	=	((float)i * 45.0f);
				fRR			=	getmax(fRR, radius);
				float rB	=	fRR / 180.0f * PI;
				float rE	=	((float)(i + 1) * 45.0f) / 180.0f * PI;
				rB			=	PI_X2 - (rB - PI_2X);
				rE			=	PI_X2 - (rE - PI_2X);
				screenVertex3D v[3] = {
					(float)middle.x + cosf(rB) * length -0.5f,  (float)middle.y - sinf(rB) * length -0.5f,    0.5f, 1.0f, c, 0, 0,
					(float)middle.x + cosf(rE) * length -0.5f,  (float)middle.y - sinf(rE) * length -0.5f,    0.5f, 1.0f, c, 0, 0,
					(float)middle.x -0.5f,  (float)middle.y -0.5f,    0.5f, 1.0f, c, 0, 0
				};
				ClampLength(v[0], (float)v[2].x, (float)v[2].y, (float)(rc.right - rc.left) / 2.0f, (float)(rc.bottom - rc.top) / 2.0f);
				ClampLength(v[1], (float)v[2].x, (float)v[2].y, (float)(rc.right - rc.left) / 2.0f, (float)(rc.bottom - rc.top) / 2.0f);
				for(int j=0;j<3;j++)
				{
					v[j].tu	=	(v[j].x /*- (float)rc.left*/) / (float)(rc.right - rc.left) * (fRightU - fLeftU) + fLeftU;
					v[j].tv	=	(v[j].y /*- (float)rc.top*/) / (float)(rc.bottom - rc.top) * (fBottomV - fTopV) + fTopV;
				}
				for(int i=0;i<3;i++)
				{
					((float3*)&v[i].x)->transform(*((float3*)&v[i].x), matTransform);
				}
				g_manager.DrawTriangle2D( pTextureNode, v[0], v[1], v[2], true );
			}
		}
		this->RenderDrawMgrs();
	}

	char UIProgress::onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex )
	{CPUTime(UIProgress);
		if( !m_bVisible )
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
				if(m_bLButtonDown && isPtIn(pt))
				{
					OnButtonClick(pt, EventID_Click);
				}
				m_bLButtonDown	=	false;
			}
			break;
		}
		return UIControl::onMouseMessage( uMsg, pt, touchIndex );
	}

	char			UIProgress::LoadControl(CSafeFileHelperR& r)
	{CPUTime(UIProgress);
		if(!UIControl::LoadControl(r))
			return false;
		skip_r sr;
		if(m_loadVersion>=0x000000A4)
			sr.begin(r.pStream);
		if(this->m_loadVersion >= 0x00000059)
			r >> this->m_fProgress;
		if(m_loadVersion >= 0x000000A6)
			r >> m_chInnerSize;
		LogInfo("LoadBlood(%d)",m_chInnerSize);
		sr.end();
		return true;
	}
	char			UIProgress::SaveControl(CSafeFileHelperW& w)
	{CPUTime(UIProgress);
		if(!UIControl::SaveControl(w))
			return false;
		skip_w sw(w.pStream);
		w << this->m_fProgress;
		w << m_chInnerSize;
		sw.end();
		return true;
	}


};
