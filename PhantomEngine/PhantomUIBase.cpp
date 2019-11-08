//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUIBase.h"
#include "PhantomUIControl.h"
#include "PhantomUIDialog.h"
#include "PhantomUIManager.h"
#include "PhantomManager.h"

namespace Phantom{
	ListBoxItem::ListBoxItem()
	{
		pData = 0;
		bVisible = true;
		bSelected = true;
		rcActive.Set(0, 0, 0, 0);
		bTextColor	=	false;
		bHighColor	=	false;
		textColor.setRGBA(1,1,1,1);
		textHighColor.setRGBA(1,1,1,1);
	}

	VOID			TextureRectUV::SetTextureID(int nID)
	{CPUTime(TextureRectUV);
		int old			=	textureIndex;
		textureIndex	=	nID;
		if(textureIndex > 0)
		{
			GetDlgManager()->AddTextureRef(textureIndex);
		}
		if(old > 0)
			GetDlgManager()->DelTexture(old);
	}

	UVAnimation::UVAnimation(UIElement* e)
	{
		m_nLoopCount		=	-1;
		m_nPlayCount		=	0;
		m_fPlaySpeed		=	1.0f;
		m_bIsDefault		=	false;
		m_elementPtr		=	e;
		m_fPlayTime			=	0;
		m_nBeginIndex		=	0;
		m_nFrameCount		=	0;
		m_angle2D			=	Unit2DAngle_Bottom;
		//
		m_nUVWidthCount		=	1;
		m_nUVHeightCount	=	1;
		m_textureIndex		=	-1;
		m_textureRect.Set(0, 0, 16, 16);
		m_bIsCustomSet		=	false;
		m_nAnimationID		=	0;
		m_nActionType		=	ActionType_Idle;
		m_nLayerAdd			=	0;
		m_bCustomAni		=	false;
		m_nextAction		=	ActionType_Idle;
		m_bIsInvW=m_bIsInvH = 0;
		m_nextRand			=	1.0f;
		m_moveOffset.Set(0,0);
		m_moveSpeed			=	0.1;
		m_bMoveOffset		=	0;
	}

	UVAnimation::~UVAnimation()
	{
		for(int i=0;i<m_rects.size();i++)
		{
			if(m_rects[i].textureIndex > 0)
				GetDlgManager()->DelTexture(m_rects[i].textureIndex);
		}
		if(m_textureIndex > 0)
			GetDlgManager()->DelTexture(m_textureIndex);
	}

	VOID				UVAnimation::CloneFrom(UVAnimation* e)
	{CPUTime(UVAnimation);
		m_nLoopCount	=	e->m_nLoopCount;
		m_nPlayCount	=	e->m_nPlayCount;
		m_nBeginIndex	=	e->m_nBeginIndex;
		m_nFrameCount	=	e->m_nFrameCount;
		m_fPlaySpeed	=	e->m_fPlaySpeed;
		m_fPlayTime		=	e->m_fPlayTime;
		m_name			=	e->m_name;
		m_bIsDefault	=	e->m_bIsDefault;
		m_nUVWidthCount	=	e->m_nUVWidthCount;
		m_bMoveOffset	=	e->m_bMoveOffset;
		m_moveOffset	=	e->m_moveOffset;
		m_moveSpeed		=	e->m_moveSpeed;
		m_nextRand		=	e->m_nextRand;
		m_nUVHeightCount	=	e->m_nUVHeightCount;
		SetTextureID(e->m_textureIndex);
		m_textureRect	=	e->m_textureRect;
		m_bIsCustomSet	=	e->m_bIsCustomSet;
		m_nActionType	=	e->m_nActionType;
		m_rects			=	e->m_rects;
		for(int i=0;i<m_rects.size();i++)
		{
			m_rects[i].textureIndex	=	0;
			if(e->m_rects[i].textureIndex > 0)
				m_rects[i].SetTextureID(e->m_rects[i].textureIndex);
		}
		m_angle2D		=	e->m_angle2D;
		m_nLayerAdd		=	e->m_nLayerAdd;
		m_nextAction	=	e->m_nextAction;
		m_bIsInvW		=	e->m_bIsInvW;
		m_bIsInvH		=	e->m_bIsInvH;
	}

	VOID				UVAnimation::SetUVAnimation(INT nWidthCount, INT nHeightCount)
	{CPUTime(UVAnimation);
		m_nUVWidthCount		=	nWidthCount;
		m_nUVHeightCount	=	nHeightCount;
	}
	VOID				UVAnimation::GetUVAnimation(INT& nWidthCount, INT& nHeightCount)
	{CPUTime(UVAnimation);
		nWidthCount			=	m_nUVWidthCount;
		nHeightCount		=	m_nUVHeightCount;
	}

	const char*			UVAnimation::GetName()
	{CPUTime(UVAnimation);
		return m_name.t;
	}
	int					UVAnimation::GetTextureRectFile(INT nIndex)
	{CPUTime(UVAnimation);
		if(nIndex < 0 || nIndex >= m_rects.size())
			return 0;
		return m_rects[nIndex].textureIndex;
	}
	const char*			UVAnimation::GetTextureRectFileName(INT nIndex)
	{
		if(nIndex < 0 || nIndex >= m_rects.size())
			return "";
		Texture* node		=	GetDlgManager()->GetTextureNode(m_rects[nIndex].textureIndex, m_elementPtr->m_controlPtr->m_dialogPtr);
		if(!node)return "";
		return node->GetFileName().t;
	}
	VOID				UVAnimation::SetTextureRectFile(INT nIndex, const char* strFilename)
	{CPUTime(UVAnimation);
		if(nIndex < 0 || nIndex >= m_rects.size())
			return;
		if(!strFilename||!strFilename[0])
			m_rects[nIndex].SetTextureID(-1);
		else
		{
			int nID						=	GetDlgManager()->AddTexture( strFilename, this->m_elementPtr->m_controlPtr->m_dialogPtr, this->m_elementPtr->m_imageFilter );
			m_rects[nIndex].SetTextureID( nID );
			if(nID > 0)
				GetDlgManager()->DelTexture(nID);
			{
				//Texture* node		=	GetDlgManager()->GetTextureNode(m_rects[nIndex].textureIndex, m_elementPtr->m_controlPtr->m_dialogPtr);
				//const TextureRect* rc = GetDlgManager()->GetTextureRect(m_rects[nIndex].textureIndex, m_rects[nIndex].name);										//返回图片指针
				//if(rc)
				//{
				//	m_rects[nIndex].rc	=	rc->rc;
				//	m_rects[nIndex].off	=	rc->off;
				//}
			}
		}
	}
	const char*			UVAnimation::GetTexture()
	{
		Texture* node		=	GetDlgManager()->GetTextureNode(m_textureIndex, m_elementPtr->m_controlPtr->m_dialogPtr);
		if(!node)
			return "";
		return node->GetFileName().t;
	}
	int 				UVAnimation::SetTexture( const char* strFilename )
	{CPUTime(UVAnimation);
		if(!strFilename||!strFilename[0])
			SetTextureID(-1);
		else
		{
			int nID						=	GetDlgManager()->AddTexture( strFilename, this->m_elementPtr->m_controlPtr->m_dialogPtr, this->m_elementPtr->m_imageFilter );
			SetTextureID( nID );
			if(nID > 0)
				GetDlgManager()->DelTexture(nID);
			{
				Texture* node		=	GetDlgManager()->GetTextureNode(m_textureIndex, m_elementPtr->m_controlPtr->m_dialogPtr);
				for(int i=0;i<m_rects.size();i++)
				{
					if(m_rects[i].textureIndex > 0)
						continue;
					const TextureRect* rc = GetDlgManager()->GetTextureRect(m_textureIndex, m_rects[i].name);										//返回图片指针
					if(rc)
					{
						m_rects[i].rc	=	rc->rc;
						m_rects[i].off	=	rc->off;
					}
				}
			}
		}
		return m_textureIndex;
	}
	VOID				UVAnimation::RegetRects()
	{CPUTime(UVAnimation);
		int texID	=	this->m_textureIndex;
		if(texID <= 0)
			texID	=	this->m_elementPtr->textureIndex;
		for(int i=0;i<m_rects.size();i++)
		{
			int textureID	=	texID;
			if(m_rects[i].textureIndex > 0)
				textureID	=	m_rects[i].textureIndex;
			const TextureRect* rc = GetDlgManager()->GetTextureRect(textureID, m_rects[i].name);										//返回图片指针
			if(rc)
			{
				m_rects[i].rc	=	rc->rc;
				m_rects[i].off	=	rc->off;
			}
		}
	}
	void				UVAnimation::SetTextureID(int textureID)
	{CPUTime(UVAnimation);
		int old			=	m_textureIndex;
		m_textureIndex	=	textureID;
		if(m_textureIndex > 0)
		{
			GetDlgManager()->AddTextureRef(m_textureIndex);
		}
		if(old > 0)
			GetDlgManager()->DelTexture(old);
	}
#pragma pack(push, 1)
struct	UVAnimationDataOld
{
	int								m_nextAction;
	char							m_bCustomAni;
	int								m_nLoopCount;
	int								m_nPlayCount;
	int								m_nBeginIndex;
	int								m_nFrameCount;
	float							m_fPlaySpeed;
	float							m_fPlayTime;
	NameAW							m_name;
	INT								m_nLayerAdd;
	char							m_bIsDefault;
	int								m_nActionType;
	int								m_nUVWidthCount;
	int								m_nUVHeightCount;
	int 							m_textureIndex;
	Rect							m_textureRect;
	int								m_nAnimationID;
	BOOL							m_bIsCustomSet;
	int								m_angle2D;
	BOOL							m_bIsInvW,m_bIsInvH;
};
#pragma pack(pop)
	VOID				UVAnimation::Load(CSafeFileHelperR& r)
	{CPUTime(UVAnimation);
		char version = 1;
		r >> version;
		skip_r sr;
		if(version>=23)
			sr.begin(r.pStream);
		if(version<22){
			UVAnimationDataOld data;
			int length	=	sizeof(UVAnimationDataOld);
			r >> length;
			r.pStream->read(&data, length);
			assert(length <= sizeof(UVAnimationDataOld));
			this->m_nextAction = data.m_nextAction;
			this->m_bCustomAni = data.m_bCustomAni;
			this->m_nLoopCount = data.m_nLoopCount;
			this->m_nPlayCount = data.m_nPlayCount;
			this->m_nBeginIndex = data.m_nBeginIndex;
			this->m_nFrameCount = data.m_nFrameCount;
			this->m_fPlaySpeed = data.m_fPlaySpeed;
			this->m_fPlayTime = data.m_fPlayTime;
			this->m_name = data.m_name.strA();
			this->m_nLayerAdd = data.m_nLayerAdd;
			this->m_bIsDefault = data.m_bIsDefault;
			this->m_nActionType = data.m_nActionType;
			this->m_nUVWidthCount = data.m_nUVWidthCount;
			this->m_nUVHeightCount = data.m_nUVHeightCount;
			this->m_textureIndex = data.m_textureIndex;
			this->m_textureRect = data.m_textureRect;
			this->m_nAnimationID = data.m_nAnimationID;
			this->m_bIsCustomSet = data.m_bIsCustomSet;
			this->m_angle2D = data.m_angle2D;
			this->m_bIsInvW = data.m_bIsInvW;
			this->m_bIsInvH = data.m_bIsInvH;
			if(m_textureIndex > 0)
			{
				short_t	strFilename[260];
				r.pStream->read(strFilename, sizeof(strFilename));
				m_textureIndex	=	GetDlgManager()->AddTexture( strFilename, m_elementPtr->m_controlPtr->m_dialogPtr, this->m_elementPtr->m_imageFilter );
			}
			int nCount;
			r >> nCount;
			if(nCount > 0)
			{
				m_rects.resize(nCount);
				memset(m_rects.GetPtr(), 0, sizeof(TextureRectUV) * m_rects.size());
				for(int i=0;i<nCount;i++)
				{
					r >> m_rects[i];
					int textureID	=	m_textureIndex;
					if(m_rects[i].textureIndex > 0)
					{
						short_t	strFilename[260];
						r.pStream->read(strFilename, sizeof(strFilename));
						m_rects[i].textureIndex	=	GetDlgManager()->AddTexture( strFilename, m_elementPtr->m_controlPtr->m_dialogPtr, this->m_elementPtr->m_imageFilter );
						//
						textureID	=	m_rects[i].textureIndex;
					}
					if(textureID <= 0)
						textureID	=	this->m_elementPtr->GetTextureID();
				}
			}
		}
		else{
			UVAnimationData* data = static_cast<UVAnimationData*>(this);
			int length	=	sizeof(UVAnimationData);
			r >> length;
			r.pStream->read(data, length);
			assert(length <= sizeof(UVAnimationData));
			if(m_textureIndex > 0)
			{
				char	strFilename[260];
				short	len = 0;
				r >> len;
				r.pStream->read(strFilename, len);
				strFilename[len] = 0;
				m_textureIndex	=	GetDlgManager()->AddTexture( strFilename, m_elementPtr->m_controlPtr->m_dialogPtr, this->m_elementPtr->m_imageFilter );
			}
			int nCount;
			r >> nCount;
			if(nCount > 0)
			{
				m_rects.resize(nCount);
				memset(m_rects.GetPtr(), 0, sizeof(TextureRectUV) * m_rects.size());
				for(int i=0;i<nCount;i++)
				{
					r >> m_rects[i];
					int textureID	=	m_textureIndex;
					if(m_rects[i].textureIndex > 0)
					{
						char	strFilename[260];
						short	len = 0;
						r >> len;
						r.pStream->read(strFilename, len);
						strFilename[len] = 0;
						m_rects[i].textureIndex	=	GetDlgManager()->AddTexture( strFilename, m_elementPtr->m_controlPtr->m_dialogPtr, this->m_elementPtr->m_imageFilter );
						//
						textureID	=	m_rects[i].textureIndex;
					}
					if(textureID <= 0)
						textureID	=	this->m_elementPtr->GetTextureID();
				}
			}
		}
		if(version>=24)
		{
			r >> m_bMoveOffset >> m_moveOffset >> m_moveSpeed >> m_nextRand;
		}
		sr.end();
//#ifdef EDITOR_OS
//			TextureNode2D* pNode	=	GetDlgManager()->GetTextureNode( this->m_elementPtr->textureIndex );
//			if(pNode){
//				for(int i=0;i<m_rects.size();i++){
//					int textureID		=	m_elementPtr->textureIndex;
//					if(m_rects[i].textureIndex > 0)
//						textureID		=	m_rects[i].textureIndex;
//					const TextureRect* rc = GetDlgManager()->GetTextureRect(textureID, m_rects[i].name.t);										//返回图片指针
//					if(rc)
//					{
//						m_rects[i].rc	=	rc->rc;
//						m_rects[i].off	=	rc->off;
//						m_rects[i].orig	=	rc->orig;
//					}
//				}
//			}
//#endif
	}

	VOID				UVAnimation::Save(CSafeFileHelperW& w)
	{CPUTime(UVAnimation);
		char version = 24;
		w << version;
		skip_w sw(w.pStream);
		//
		UVAnimationData* data = static_cast<UVAnimationData*>(this);
		int length	=	sizeof(UVAnimationData);
		w << length;
		w.pStream->write(data, sizeof(UVAnimationData));
		if(m_textureIndex > 0)
		{
			TextureNode2D* pNode	=	GetDlgManager()->GetTextureNode( m_textureIndex );
			short len = strlen(pNode->fileName.t);
			w << len;
			w.pStream->write(pNode->fileName.t, len);
		}
		int nCount	=	this->m_rects.size();
		w << nCount;
		for(int i=0;i<nCount;i++)
		{
			w << m_rects[i];
			if(m_rects[i].textureIndex > 0)
			{
				TextureNode2D* pNode	=	GetDlgManager()->GetTextureNode( m_rects[i].textureIndex );
				short len = strlen(pNode->fileName.t);
				w << len;
				w.pStream->write(pNode->fileName.t, len);
			}
		}
		w << m_bMoveOffset << m_moveOffset << m_moveSpeed << m_nextRand;
		sw.end();
	}

	VOID				UVAnimation::SetTextureRect(INT nIndex, const char* szMode)
	{CPUTime(UVAnimation);
		if(!szMode)szMode = "";
		if(nIndex < 0 || nIndex >= m_rects.size())
			return;
		m_rects[nIndex].name	=	szMode;
		m_rects[nIndex].name.setIsFile();
		int textureID		=	m_textureIndex;
		if(m_rects[nIndex].textureIndex > 0)
			textureID		=	m_rects[nIndex].textureIndex;
		const TextureRect* rc = GetDlgManager()->GetTextureRect(textureID, m_rects[nIndex].name.t);										//返回图片指针
		if(rc)
		{
			m_rects[nIndex].rc	=	rc->rc;
			m_rects[nIndex].off	=	rc->off;
			m_rects[nIndex].orig	=	rc->orig;
		}
	}
	VOID				UVAnimation::SetPlaySpeed(float f)
	{CPUTime(UVAnimation);
		m_fPlaySpeed	=	f;
	}

	float				UVAnimation::GetPlaySpeed()
	{CPUTime(UVAnimation);
		return m_fPlaySpeed;
	}

	VOID				UVAnimation::SetLoopCount(INT nLoop)
	{CPUTime(UVAnimation);
		this->m_nLoopCount	=	nLoop;
	}
	int					UVAnimation::GetLoopCount()
	{CPUTime(UVAnimation);
		return this->m_nLoopCount;
	}

	VOID				UVAnimation::SetUVMapIndex(INT nBegin, INT nEnd)
	{CPUTime(UVAnimation);
		m_nBeginIndex	=	nBegin;
		m_nFrameCount	=	nEnd;
		CDynamicArrayS<TextureRectUV,0> rcTemp	=	m_rects;
		if(m_nFrameCount > 0)
		{
			m_rects.SetArrayCount(m_nFrameCount);
			memset(m_rects.GetPtr(), 0, sizeof(TextureRectUV) * m_rects.size());
			for(int i=0;i<m_nFrameCount;i++)
			{
				if(i < rcTemp.size())
					m_rects[i] = rcTemp[i];
			}
		}
		else
			m_rects.clear();
	}

	VOID				UVAnimation::GetUVMapIndex(INT& nBegin, INT& nEnd)
	{CPUTime(UVAnimation);
		nBegin	=	m_nBeginIndex;
		nEnd	=	m_nFrameCount;
	}
	TextureRect*			UVAnimation::SetCurrentIndex(Rect& ret, int& textureIndex, INT nIndex, Pixel& offset)
	{CPUTime(UVAnimation);
		if(m_nFrameCount <= 0)
			return 0;
		int nUVW	=	m_elementPtr->m_nUVWidthCount;
		int nUVH	=	m_elementPtr->m_nUVHeightCount;
		Rect*	rect	=	&m_elementPtr->textureRect;
		if(this->m_bIsCustomSet)
		{
			if(nIndex < 0 || nIndex >= this->m_rects.size())
				return 0;
			ret			=	m_rects[nIndex].rc;
			offset.x	=	m_rects[nIndex].offset.x;// + m_rects[nIndex].off.x;
			offset.y	=	m_rects[nIndex].offset.y;// + m_rects[nIndex].off.y;
			if(m_rects[nIndex].textureIndex > 0)
				textureIndex	=	m_rects[nIndex].textureIndex;
			else if(m_textureIndex > 0)
				textureIndex	=	this->m_textureIndex;
			return &m_rects[nIndex];
		}
		if(nUVW <= 0 || nUVW <= 0 || m_nFrameCount <= 0)
			return 0;
		int nWidth	=	(rect->right -  rect->left) / nUVW;
		int nHeight	=	(rect->bottom - rect->top) / nUVH;
		if(nWidth <= 0 || nHeight <= 0)
			return 0;
		//
		int nCount	=	nUVW * nUVH;
		nIndex		+=	this->m_nBeginIndex;
		if(nIndex >= nCount)
			nIndex	=	nCount - 1;
		ret.left	=	rect->left + nWidth * (nIndex % nUVW);
		ret.top		=	rect->top + nHeight * (nIndex / nUVW);
		ret.right	=	ret.left + nWidth;
		ret.bottom	=	ret.top + nHeight;
		return 0;
	}

	BOOL					UVAnimation::SetCurrentIndex(Rect& ret, INT nIndex, Pixel& offset)
	{CPUTime(UVAnimation);
		int nUVW	=	m_elementPtr->m_nUVWidthCount;
		int nUVH	=	m_elementPtr->m_nUVHeightCount;
		Rect*	rect	=	&m_elementPtr->textureRect;
		BOOL bRet = true;
		if(this->m_bIsCustomSet && m_rects.size()>0)
		{
			if(m_nLoopCount>0){
				if(nIndex < 0 || nIndex >= m_rects.size()*m_nLoopCount)
				{
					nIndex = m_rects.size()-1;
					bRet = false;
				}
			}
			nIndex		%=	m_rects.size();
			ret			=	m_rects[nIndex].rc;
			offset.x	=	m_rects[nIndex].offset.x;// + m_rects[nIndex].off.x;
			offset.y	=	m_rects[nIndex].offset.y;// + m_rects[nIndex].off.y;
			return bRet;
		}
		if(nUVW <= 0 || nUVW <= 0 || m_nFrameCount <= 0)
		{
			ret = *rect;
			offset.Set(0,0);
			return false;
		}
		int nWidth	=	(rect->right -  rect->left) / nUVW;
		int nHeight	=	(rect->bottom - rect->top) / nUVH;
		if(nWidth <= 0 || nHeight <= 0)
		{
			ret = *rect;
			offset.Set(0,0);
			return false;
		}
		offset.Set(0,0);
		int nCount	=	nUVW * nUVH;
		if(m_nLoopCount>0){
			if(nIndex >= m_nFrameCount*m_nLoopCount)
			{
				bRet	=	false;
				nIndex  =	m_nFrameCount-1;
			}
		}
		else
		{
			nIndex %= m_nFrameCount;
		}
		nIndex		+=	this->m_nBeginIndex;
		nIndex		%=	nCount;
		ret.left	=	rect->left + nWidth * (nIndex % nUVW);
		ret.top		=	rect->top + nHeight * (nIndex / nUVW);
		ret.right	=	ret.left + nWidth;
		ret.bottom	=	ret.top + nHeight;
		return bRet;
	}

	VOID				UVAnimation::SetTextureOffset(INT nIndex, const Pixel& offset)
	{CPUTime(UVAnimation);
		if(nIndex < 0 || nIndex >= m_rects.size())
			return;
		m_rects[nIndex].offset	=	offset;
	}
	const Pixel*		UVAnimation::GetTextureOffset(INT nIndex)
	{CPUTime(UVAnimation);
		if(nIndex < 0 || nIndex >= m_rects.size())
			return 0;
		return &m_rects[nIndex].offset;
	}

	VOID				UVAnimation::SetTextureRect(INT nIndex, const TextureRect& rc)
	{CPUTime(UVAnimation);
		if(nIndex < 0 || nIndex >= m_rects.size())
			return;
		m_rects[nIndex].name	=	rc.name;
		m_rects[nIndex].rc		=	rc.rc;
		int textureID			=	m_rects[nIndex].textureIndex;;
		if(textureID <= 0)
			textureID	=	m_textureIndex;
		if(textureID <= 0)
			textureID	=	this->m_elementPtr->GetTextureID();
		const TextureRect* rect = GetDlgManager()->GetTextureRect(textureID, m_rects[nIndex].name);										//返回图片指针
		if(rect)
		{
			m_rects[nIndex].rc	=	rect->rc;
			m_rects[nIndex].off	=	rect->off;
			m_rects[nIndex].orig =	rect->orig;
		}
	}

	VOID				UVAnimation::SetActionType(ActionType t)
	{CPUTime(UVAnimation);
		m_nActionType = t;
	}

	const TextureRect*	UVAnimation::GetTextureRect(INT nIndex)
	{CPUTime(UVAnimation);
		if(nIndex < 0 || nIndex >= m_rects.size())
			return 0;
		return &m_rects[nIndex];
	}

	BOOL					UVAnimation::IsPlayOver()
	{CPUTime(UVAnimation);
		return (m_nPlayCount >= this->m_nLoopCount && m_nLoopCount >= 0);
	}

	TextureRect*			UVAnimation::FrameMove(Rect& ret, int& textureIndex, Pixel& offset, float fElapsedTime)
	{CPUTime(UVAnimation);
		if(m_nFrameCount <= 0)
			return 0;
		UIGroup* group	=	this->m_elementPtr->m_controlPtr->GetGroupPtr();
		if(group && group->GetGroupIsUnit() && !m_bCustomAni)
			m_fPlayTime	=	group->GetPlayTime() * m_fPlaySpeed;//(m_elementPtr->m_fUVPlaySpeed * this->m_fPlaySpeed * fElapsedTime);
		else
			m_fPlayTime	+=	(m_elementPtr->m_fUVPlaySpeed * this->m_fPlaySpeed * fElapsedTime);
		int nIndex	=	0;
		if(m_fPlayTime >= (float)m_nFrameCount)
		{
			this->m_nPlayCount	++;
			m_fPlayTime		-=	(float)m_nFrameCount;
			if(m_fPlayTime < 0)
				m_fPlayTime	=	0;
			//UV动画播放完毕
			m_elementPtr->m_controlPtr->m_dialogPtr->RaiseEvent(m_elementPtr->m_controlPtr, EventID_UVPlayOver, Pixel(0,0));
		}
		nIndex	=	(int)m_fPlayTime;
		nIndex	%=	m_nFrameCount;
		if((m_nPlayCount >= this->m_nLoopCount && m_nLoopCount >= 0))
		{
			nIndex	=	m_nFrameCount - 1;
			if(nIndex < 0)
				nIndex	=	0;
		}
		return SetCurrentIndex(ret, textureIndex, nIndex, offset);
	}
	void UIElement::SetTexture( unsigned int _textureIndex, Rect* prcTexture, unsigned int defaultTextureColor )
	{CPUTime(UIElement);
		SetTextureID(_textureIndex);
		if( prcTexture )
			textureRect = *prcTexture;
		else
			textureRect.Set(0, 0, 0, 0);
		textureColor = Phantom::color4( defaultTextureColor );
	}
	//--------------------------------------------------------------------------------------
	void UIElement::SetFont( unsigned int fontIndex, unsigned int defaultFontColor, DWORD dwTextFormat )
	{CPUTime(UIElement);
		this->fontIndex = fontIndex;
		this->dwTextFormat = dwTextFormat;
		fontColor = Phantom::color4( defaultFontColor );
	}

	VOID UIElement::SetUVAnimationIndex(INT nIndex)
	{CPUTime(UIElement);
		textureIndexNew	=	textureIndex;
		rcTexture		=	textureRect;
		if(!this->m_activeUVAnimation)
			return;
		Pixel offset(0, 0);
		this->m_activeUVAnimation->SetCurrentIndex(rcTexture, textureIndexNew, nIndex, offset);
		//if(m_nUVWidthCount <= 0 || m_nUVHeightCount <= 0 || nIndex < 0)
		//	return;
		//int nWidth	=	(textureRect.right - textureRect.left) / m_nUVWidthCount;
		//int nHeight	=	(textureRect.bottom - textureRect.top) / m_nUVHeightCount;
		//if(nWidth <= 0 || nHeight <= 0)
		//	return;
		//int nCount	=	m_nUVWidthCount * m_nUVHeightCount;
		//if(nIndex >= nCount)
		//	nIndex	=	nCount - 1;
		//rcTexture.left		=	textureRect.left + nWidth * (nIndex % m_nUVWidthCount);
		//rcTexture.top		=	textureRect.top + nHeight * (nIndex / m_nUVWidthCount);
		//rcTexture.right		=	rcTexture.left + nWidth;
		//rcTexture.bottom	=	rcTexture.top + nHeight;
	}

	VOID		UIElement::SetControlPtr(UIControl* control)
	{CPUTime(UIElement);
		m_nElementID		=	control->m_dialogPtr->GetNewElementID();
		m_controlPtr		=	control;
	}

	UIElement::UIElement(UIControl* control)
	{
		m_bNoTexture = false;
		m_bIsInvW=m_bIsInvH = 0;
		m_bIsInvWTo=m_bIsInvHTo = 0;
		m_nElementID		=	0;
		textureIndex		=	0;
		fontIndex			=	0;
		dwTextFormat		=	DrawTextAlign_CENTER | DrawTextAlign_VCENTER | DrawTextAlign_SINGLELINE;
		memset(&textureRect, 0, sizeof(Rect));
		memset(&rcTexture, 0, sizeof(Rect));
		memset(&textureColor, 0, sizeof(Phantom::color4));
		memset(&fontColor, 0, sizeof(Phantom::color4));
		textureColor	=	Phantom::color4(1,1,1,1);
		fontColor		=	Phantom::color4(1,1,1,1);
		shadowColor.setRGBA(1,1,1,1);
		fontColor = Phantom::color4(0xff000000);
		textureColor = Phantom::color4(0xffffffff);
//		m_name			=	"n/a";
		m_bRenderScale	=	false;
		m_bWrap			=	false;
		m_wrapWidth.Set(5, 5);
		m_nTileOffX		=	0;
		m_nTileOffY		=	0;
		m_nTileScaleX	=	m_nTileScaleY = 1;
		//
		m_tileXieJiao		=	0;
		m_imageFilter		=	0;
		m_this.ptr			=	this;
		m_nSrcBlend			=	Phantom::BlendMode_SRCALPHA;
		m_nDestBlend		=	Phantom::BlendMode_INVSRCALPHA;
		m_nColorOP			=	0;
		m_nUVWidthCount		=	0;
		m_nUVHeightCount	=	0;
		m_fUVPlaySpeed		=	1.0f;
		m_fUVPlayTime		=	0;
		m_activeUVAnimation	=	0;
		m_defaultUVAni		=	0;
		memset(&m_rectOffset, 0, sizeof(m_rectOffset));
		memset(m_angleUV, 0, sizeof(m_angleUV));
		m_controlPtr		=	control;
		m_nTileWidth		=	0;
		m_nTileHeight		=	0;
		textureIndexNew		=	0;
		memset(&m_modeRect, 0, sizeof(m_modeRect));
		m_playAction		=	ActionType_None;
		m_imgOffset.Set(0, 0);
		m_imgSize.Set(0, 0);
	}

	VOID					UIElement::ChangeAngle()
	{CPUTime(UIElement);
		if(m_controlPtr->m_bEnableUnitAngle>0)
		{
			for(int i=0;i<this->m_uvAnimations.size();i++)
			{
				if(m_uvAnimations[i]->m_angle2D == m_controlPtr->m_unitAngle && m_uvAnimations[i]->m_nActionType == m_playAction)
				{
					m_activeUVAnimation	=	m_uvAnimations[i];
					break;
				}
			}
		}
		this->m_controlPtr->m_bChanged	=	true;
	}
	VOID					UIElement::PlayAction(ActionType type)
	{CPUTime(UIElement);
		m_playAction		=	type;
		for(int i=0;i<this->m_uvAnimations.size();i++)
		{
			if(m_controlPtr->m_bEnableUnitAngle>0)
			{
				if(m_uvAnimations[i]->m_angle2D == m_controlPtr->m_unitAngle && m_uvAnimations[i]->m_nActionType == m_playAction)
				{
					m_activeUVAnimation	=	m_uvAnimations[i];
					m_activeUVAnimation->m_nPlayCount	=	0;
					m_activeUVAnimation->m_fPlayTime	=	0;
					m_activeUVAnimation->m_bCustomAni	=	false;
					return;
				}
			}
			else
			{
				if(m_uvAnimations[i]->m_nActionType == m_playAction)
				{
					m_activeUVAnimation	=	m_uvAnimations[i];
					m_activeUVAnimation->m_nPlayCount	=	0;
					m_activeUVAnimation->m_fPlayTime	=	0;
					m_activeUVAnimation->m_bCustomAni	=	false;
					if(m_activeUVAnimation->m_bMoveOffset){
						this->m_controlPtr->BeginMove(m_activeUVAnimation->m_moveOffset, m_activeUVAnimation->m_moveSpeed);
					}
					return;
				}
			}
		}
		m_activeUVAnimation	=	0;
	}

	VOID					UIElement::RebuildUVAngles()
	{CPUTime(UIElement);
		memset(m_angleUV, 0, sizeof(m_angleUV));
		for(int i=0;i<this->m_uvAnimations.size();i++)
		{
			m_angleUV[m_uvAnimations[i]->m_angle2D % Unit2DAngle_Count]	=	m_uvAnimations[i];
		}
	}

	UVAnimation*	UIElement::FindUVMapAnimation(const char* szName)
	{CPUTime(UIElement);
		if(!szName || !szName[0])
			return m_defaultUVAni;
		for(int i=0;i<this->m_uvAnimations.size();i++)
		{
			if(m_uvAnimations[i]->m_name == szName)
				return m_uvAnimations[i];
		}
		return m_defaultUVAni;
	}

	VOID					UIElement::SetUVAnimation(INT nWidthCount, INT nHeightCount)
	{CPUTime(UIElement);
		m_nUVWidthCount		=	nWidthCount;
		m_nUVHeightCount	=	nHeightCount;
	}
	VOID					UIElement::GetUVAnimation(INT& nWidthCount, INT& nHeightCount)
	{CPUTime(UIElement);
		nWidthCount			=	m_nUVWidthCount;
		nHeightCount		=	m_nUVHeightCount;
	}
	VOID					UIElement::SetUVPlaySpeed(float f)
	{CPUTime(UIElement);
		m_fUVPlaySpeed	=	f;
	}
	float					UIElement::GetUVPlaySpeed()
	{CPUTime(UIElement);
		return m_fUVPlaySpeed;
	}
	VOID					UIElement::RandomSetMode()	//随即设置一个索引
	{CPUTime(UIElement);
		CDynamicArray<TextureRect>*	list	=	GetDlgManager()->GetTextureRectPtr(textureIndex);
		if(!list || !list->size())
			return;
		this->SetTextureMode(list->GetPtr()[rand() % list->size()].name);
	}
	const char*				UIElement::GetTextureMode()
	{
		return m_textureMode.t;
	}
	VOID					UIElement::SetTextureMode(const char* szName, DrawTextAlign align)
	{CPUTime(UIElement);
		if(!szName)szName = "";
		FileName name = szName;//m_textureMode.setIsFile();
		name.setIsFile();
		this->m_textureMode	=	name.t;
		const TextureRect* rc = GetDlgManager()->GetTextureRect(textureIndex, name);										//返回图片指针
		if(rc)
		{
			textureRect		=	rc->rc;
			m_modeRect.rc	=	rc->rc;
			m_modeRect.off	=	rc->off;
			m_modeRect.orig	=	rc->orig;
			//
			if(align != 0)
			{
				Rect rc = this->m_controlPtr->GetRenderRect();
				float height	=	rc.bottom - rc.top;
				float width		=	rc.right - rc.left;
				float modeWidth = m_modeRect.rc.GetWidth();
				float modeHeight = m_modeRect.rc.GetHeight();
				//
				if((align & Phantom::DrawTextAlign_RIGHT) == Phantom::DrawTextAlign_RIGHT)
					rc.left = rc.right - modeWidth;//(int)(height * (modeWidth / modeHeight));
				else if((align & Phantom::DrawTextAlign_CENTER) == Phantom::DrawTextAlign_CENTER)
					rc.left = rc.left + (rc.GetWidth() - modeWidth) / 2;//(rc.right - rc.left - (int)(height * (modeWidth / modeHeight))) / 2;
				if((align & Phantom::DrawTextAlign_BOTTOM) == Phantom::DrawTextAlign_BOTTOM)
					rc.top = rc.bottom - modeHeight;//(int)(width * (modeHeight / modeWidth));
				else if((align & Phantom::DrawTextAlign_VCENTER) == Phantom::DrawTextAlign_VCENTER)
					rc.top = rc.top + (rc.GetHeight() - modeHeight) / 2;//(rc.bottom - rc.top - (int)(width * (modeHeight / modeWidth))) / 2;
				rc.right = rc.left + modeWidth;
				rc.bottom = rc.top + modeHeight;
				m_controlPtr->SetRenderRect(rc);
				//
			}
		}
		this->m_controlPtr->m_bChanged	=	true;
	}

	int			SortUVAnimation(const void* v1, const void* v2)
	{
		UVAnimation* e1	=	*(UVAnimation**)v1;
		UVAnimation* e2	=	*(UVAnimation**)v2;
		if(e1->GetActionType() < e2->GetActionType())
			return -1;
		else if(e1->GetActionType() > e2->GetActionType())
			return 1;
		if(e1->GetAngle() < e2->GetAngle())
			return -1;
		else if(e1->GetAngle() > e2->GetAngle())
			return 1;
		return stricmp(e1->GetName(), e2->GetName());
	}

	VOID					UIElement::RemoveUVAnimation(UVAnimation* e)
	{CPUTime(UIElement);
		for(int i=0;i<m_uvAnimations.size();i++)
		{
			if(m_uvAnimations[i] == e)
			{
				delete m_uvAnimations[i];
				m_uvAnimations.eraseByIndex(i);//
				if(m_defaultUVAni == e)
					m_defaultUVAni	=	0;
				if(m_activeUVAnimation == e)
					this->ActiveUVAnimation(m_uvAnimations.size() > 0 ? m_uvAnimations[0] : 0);
				break;
			}
		}
	}
	VOID					UIElement::AddUVAnimation(UVAnimation* c)
	{CPUTime(UIElement);
		UVAnimation* e = new UVAnimation(this);
		e->m_nAnimationID	=	m_controlPtr->m_nAnimationID++;
		e->m_name			=	"UVAnimation";
		if(c)
			e->CloneFrom(c);
		m_uvAnimations.push_back( e );
		if(m_uvAnimations.size() > 1)
			qsort(&m_uvAnimations.at(0), m_uvAnimations.size(), sizeof(UVAnimation*), SortUVAnimation);
	}
	UVAnimation*	UIElement::GetUVAnimationPtr(INT nIndex)
	{CPUTime(UIElement);
		if(nIndex < 0 || nIndex >= m_uvAnimations.size())
			return 0;
		return m_uvAnimations[nIndex];
	}
	VOID					UIElement::SetUVAnimationDefault(UVAnimation* e)
	{CPUTime(UIElement);
		for(int i=0;i<m_uvAnimations.size();i++)
		{
			if(m_uvAnimations[i] == e)
			{
				m_uvAnimations[i]->m_bIsDefault	=	true;
				m_defaultUVAni		=	m_uvAnimations[i];
			}
			else
				m_uvAnimations[i]->m_bIsDefault	=	false;
		}
	}

	UVAnimation*	UIElement::GetUVAnimationDefault()
	{CPUTime(UIElement);
		//for(int i=0;i<m_uvAnimations.size();i++)
		//{
		//	if(m_uvAnimations[i]->m_bIsDefault)
		//		return m_uvAnimations[i];
		//}
		return m_defaultUVAni;
	}

	VOID					UIElement::ActiveUVAnimation(UVAnimation* e)
	{CPUTime(UIElement);
		m_activeUVAnimation	=	0;
		for(int i=0;i<m_uvAnimations.size();i++)
		{
			if(m_uvAnimations[i] == e)
			{
				m_activeUVAnimation	=	m_uvAnimations[i];
				m_activeUVAnimation->m_nPlayCount	=	0;
				m_activeUVAnimation->m_fPlayTime	=	0;
				m_activeUVAnimation->m_bCustomAni	=	false;
				m_playAction	=	m_uvAnimations[i]->m_nActionType;
				if(m_activeUVAnimation->m_bMoveOffset){
					this->m_controlPtr->BeginMove(m_activeUVAnimation->m_moveOffset, m_activeUVAnimation->m_moveSpeed);
				}
				break;
			}
		}
	}

	UVAnimation*	UIElement::GetActiveUVAnimation()
	{CPUTime(UIElement);
		return m_activeUVAnimation;
	}

	INT						UIElement::GetUVAnimationCount()
	{CPUTime(UIElement);
		return m_uvAnimations.size();
	}

	VOID			UIElement::OnFrameMove(float fElapsedTime, Pixel& offset, INT& layerAdd)
	{CPUTime(UIElement);
		textureIndexNew	=	textureIndex;
		rcTexture		=	textureRect;
		m_imgOffset.Set(0, 0);
		m_imgSize.Set(rcTexture.GetWidth(), rcTexture.GetHeight());
		this->m_bIsInvWTo	=	m_bIsInvW;
		this->m_bIsInvHTo	=	m_bIsInvH;
		if(!this->m_activeUVAnimation)
		{
			if(m_modeRect.orig.cx > 0)
			{
				m_imgOffset	=	m_modeRect.off;
				m_imgSize	=	m_modeRect.orig;
			}
			//offset.x	=	m_modeRect.off.x;
			//offset.y	=	m_modeRect.off.y;
			return;
		}
		layerAdd	=	m_activeUVAnimation->m_nLayerAdd;
		TextureRect* rc	=	m_activeUVAnimation->FrameMove(rcTexture, textureIndexNew, offset, fElapsedTime);
		this->m_bIsInvWTo	=	m_activeUVAnimation->m_bIsInvW;
		this->m_bIsInvHTo	=	m_activeUVAnimation->m_bIsInvH;
		if(rc && rc->orig.cx > 0 && rc->orig.cy > 0)
		{
			m_imgOffset	=	rc->off;
			m_imgSize	=	rc->orig;
		}
		if(m_activeUVAnimation && m_activeUVAnimation->IsPlayOver() && m_activeUVAnimation->m_nextAction > 0)
		{
			if(m_activeUVAnimation->m_bMoveOffset){
				if(m_controlPtr->m_bMoveOffset)
				{
					m_activeUVAnimation->m_nPlayCount = 0;
					return;
				}
			}
			if(RandomF(0,1)<m_activeUVAnimation->m_nextRand)//如果有这个概率触发下一个动作则切换到下一个动作
				PlayAction((ActionType)m_activeUVAnimation->m_nextAction);
			else
				m_activeUVAnimation->m_nPlayCount = 0;
		}
	}
	void		UIElement::SetSourceRect(const Rect& rc)
	{CPUTime(UIElement);
		textureRect	=	(Rect&)rc;
		if((textureRect.right==-1||textureRect.bottom==-1))
		{
			const TextureRect* prc = GetDlgManager()->GetTextureRect(textureIndex, m_textureMode);
			if(prc)
				textureRect = prc->rc;
			else{
				Texture* tex = GetDlgManager()->GetTextureNode(textureIndex, this->m_controlPtr->m_dialogPtr);
				if(tex)
					textureRect.Set(0, 0, tex->GetOriWidth(), tex->GetOriHeight());
			}
		}
		if(m_controlPtr)
			this->m_controlPtr->m_bChanged	=	true;
	}
	const char*	UIElement::GetTexture()
	{
		Texture* node = GetDlgManager()->GetTextureNode(textureIndex, this->m_controlPtr->m_dialogPtr );
		if(!node)
			return "";
		return node->GetFileName().t;
	}
	int 		UIElement::SetTexture( const char* strFilename )
	{CPUTime(UIElement);
		if(!strFilename)
			SetTextureID(-1);
		else
		{
			int nID		=	GetDlgManager()->AddTexture( strFilename, this->m_controlPtr->m_dialogPtr, m_imageFilter );
			SetTextureID( nID );
			if(nID > 0)
				GetDlgManager()->DelTexture(nID);
			const TextureRect* rc = GetDlgManager()->GetTextureRect(textureIndex, m_textureMode);										//返回图片指针
			if(rc)
			{
				m_modeRect.off	=	rc->off;
				m_modeRect.orig =	rc->orig;
				m_modeRect.rc	=	rc->rc;
				textureRect		=	rc->rc;
			}
			else
				memset(&m_modeRect, 0, sizeof(m_modeRect));
			for(int i=0;i<this->m_uvAnimations.size();i++)
				this->m_uvAnimations[i]->RegetRects();
		}
		this->m_controlPtr->m_bChanged	=	true;
		return this->textureIndex;
	}

	void		UIElement::SetTextureID(int textureID)
	{CPUTime(UIElement);
		int old		=	textureIndex;
		textureIndex	=	textureID;
		textureIndexNew	=	textureIndex;
		if(textureIndex > 0)
		{
			GetDlgManager()->AddTextureRef(textureIndex);
		}
		if(old > 0)
			GetDlgManager()->DelTexture(old);
		this->m_controlPtr->m_bChanged	=	true;
		this->m_controlPtr->m_textIndexs.SetArrayCount(0);
	}

	UIElement::~UIElement()
	{
		for(int i=0;i<m_uvAnimations.size();i++)
			delete m_uvAnimations[i];
		if(GetDlgManager() && textureIndex > 0)
			GetDlgManager()->DelTexture(this->textureIndex);
	}

	void	UIElement::operator	=	(UIElement& c)
	{
		SetTextureID(c.textureIndex);
		//
		//const ElementData* to		=	static_cast<const ElementData*>(&c);
		//ElementData* from	=	static_cast<ElementData*>(this);
		//*from				=	*to;
		////

		fontIndex			=	c.fontIndex;
		dwTextFormat		=	c.dwTextFormat;
		memcpy(&textureRect, &c.textureRect, sizeof(Rect));
		textureColor		=	c.textureColor;
		fontColor			=	c.fontColor;
		m_tileXieJiao		=	c.m_tileXieJiao;
		this->m_bNoTexture	=	c.m_bNoTexture;
//		m_name				=	c.m_name;
		//
		this->m_nColorOP	=	c.m_nColorOP;
		this->m_nSrcBlend	=	c.m_nSrcBlend;
		this->m_nDestBlend	=	c.m_nDestBlend;
		m_bRenderScale		=	c.m_bRenderScale;
		m_bWrap				=	c.m_bWrap;
		m_wrapWidth			=	c.m_wrapWidth;
		m_nTileWidth		=	c.m_nTileWidth;
		m_nTileHeight		=	c.m_nTileHeight;
		m_textureMode		=	c.m_textureMode;
		m_rectOffset		=	c.m_rectOffset;
		this->m_modeRect	=	c.m_modeRect;
		for(int i=0;i<c.m_uvAnimations.size();i++)
		{
			UVAnimation* e	=	new UVAnimation(this);
			e->m_nAnimationID		=	m_controlPtr->m_nAnimationID++;
			e->CloneFrom(c.m_uvAnimations[i]);
			this->m_uvAnimations.push_back(e);
		}
		m_nUVWidthCount		=	c.m_nUVWidthCount;
		m_nUVHeightCount	=	c.m_nUVHeightCount;
		m_fUVPlaySpeed		=	c.m_fUVPlaySpeed;
		RebuildUVAngles();
		for(int i=0;i<this->m_uvAnimations.size();i++)
			if(m_uvAnimations[i]->m_bIsDefault)
			{
				m_defaultUVAni	=	m_uvAnimations[i];
				break;
			}
	}
#pragma pack(push,1)
struct	ElementDataOld
{
	Name					m_textureMode;
	int						m_playAction;
	Rect					m_rectOffset;
	INT						m_nElementID;
	INT						m_nSrcBlend;
	INT						m_nDestBlend;
	INT						m_nColorOP;
	//
	int 					textureIndex;
	int 					fontIndex;
	DWORD					dwTextFormat;
	Phantom::color4 					textureColor;
	Phantom::color4 					fontColor;
	Phantom::color4					shadowColor;
	//
	char					m_bRenderScale;
	char					m_bWrap;
	int						m_nTileWidth;
	int						m_nTileHeight;
	Pixel					m_wrapWidth;
	int						m_nUnitRenderSide;
	NameAW					m_name;
	INT						m_nUVWidthCount;
	INT						m_nUVHeightCount;
	float					m_fUVPlaySpeed;
	Rect					textureRect;
	//
	float					m_fUVPlayTime;
	Rect					rcTexture;
	Pixel					m_imgOffset;
	Size					m_imgSize;
	TextureRect				m_modeRect;
	BOOL							m_bIsInvW,m_bIsInvH;
	char					imageFilter;
	char					reserved[32];
};
#pragma pack(pop)
	char			UIElement::LoadElement(UIManager* p, UIDialog* dlg, CSafeFileHelperR& r, char bOld, char bNewVersion)
	{CPUTime(UIElement);
		int version;
		r >> version;
		skip_r sr;
		if(version>=21)
			sr.begin(r.pStream);
		if(version<20){
			int length		=	sizeof(ElementData);
			r >> length;
			//ElementData* data	=	static_cast<ElementData*>(this);
			ElementDataOld data;
			assert(length <= sizeof(ElementDataOld));
			r.pStream->read(&data, length);
			//
			m_playAction=data.m_playAction;
			m_rectOffset=data.m_rectOffset;
			m_nElementID=data.m_nElementID;
			m_nSrcBlend=data.m_nSrcBlend;
			m_nDestBlend=data.m_nDestBlend;
			m_nColorOP=data.m_nColorOP;
			textureIndex=data.textureIndex;
			fontIndex=data.fontIndex;
			dwTextFormat=data.dwTextFormat;
			textureColor=data.textureColor;
			fontColor=data.fontColor;
			shadowColor=data.shadowColor;
			m_bRenderScale=data.m_bRenderScale;
			m_bWrap=data.m_bWrap;
			m_nTileWidth=data.m_nTileWidth;
			m_nTileHeight=data.m_nTileHeight;
			m_wrapWidth=data.m_wrapWidth;
			m_nUVWidthCount=data.m_nUVWidthCount;
			m_nUVHeightCount=data.m_nUVHeightCount;
			m_fUVPlaySpeed=data.m_fUVPlaySpeed;
			textureRect=data.textureRect;
			m_fUVPlayTime=data.m_fUVPlayTime;
			rcTexture=data.rcTexture;
			m_imgOffset=data.m_imgOffset;
			m_imgSize=data.m_imgSize;
			m_modeRect.off=data.m_modeRect.off;
			m_modeRect.orig=data.m_modeRect.orig;
			m_modeRect.rc=data.m_modeRect.rc;
			m_bIsInvW=data.m_bIsInvW;
			m_bIsInvH=data.m_bIsInvH;
			m_imageFilter = data.imageFilter;
			//
			if(textureIndex > 0)
			{
				short_t	strFilename[260];
				r.pStream->read(strFilename, sizeof(strFilename));
				textureIndex			=	p->AddTexture( strFilename, dlg, m_imageFilter );
				//
				const TextureRect* rc = GetDlgManager()->GetTextureRect(textureIndex, m_textureMode);										//返回图片指针
				if(rc)
				{
					m_modeRect.off	=	rc->off;
					m_modeRect.orig =	rc->orig;
					m_modeRect.rc	=	rc->rc;
					textureRect		=	rc->rc;
				}
			}
			if(fontIndex > 0)
			{
				FontNode2D font;
				memset(&font, 0, sizeof(font));

				r >> font.nFontHeight >> font.nWeight;
				r >> font.bItalic >> font.bUnderline;
				short_t		strFace[260];
				r.pStream->read( strFace, sizeof(strFace) );
				//
#ifdef WIN32_OS
				char buf[256];
				int len = ::WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)strFace, u_strlen(strFace), buf, 256, 0, 0);
				buf[len] = 0;
				font.fontName = buf;
#endif
				//WToUtf8(strFace, font.fontName.t, font.fontName.Length);
				fontIndex		=	p->AddFont( font.fontName, font.nFontHeight, font.nWeight, font.bItalic, font.bUnderline );
			}
		}
		else{
			int length		=	sizeof(ElementData);
			r >> length;
			ElementData* data	=	static_cast<ElementData*>(this);
			assert(length <= sizeof(ElementData));
			r.pStream->read(data, length);
			//
			if(textureIndex > 0)
			{
				char	strFilename[260];
				short len = 0;
				r >> len;
				r.pStream->read(strFilename, len);
				strFilename[len] = 0;
				if(m_imageFilter<0||version<23)
					m_imageFilter=0;
				int filter = m_imageFilter;
				if(this->m_controlPtr->m_bIsPickPixel)
					filter |= 0x00040000;
				textureIndex	=	p->AddTexture( strFilename, dlg, filter );
				//
				const TextureRect* rc = GetDlgManager()->GetTextureRect(textureIndex, m_textureMode);										//返回图片指针
				if(rc)
				{
					m_modeRect.off	=	rc->off;
					m_modeRect.orig =	rc->orig;
					m_modeRect.rc	=	rc->rc;
					textureRect		=	rc->rc;
				}
			}
			if(fontIndex > 0)
			{
				FontNode2D font;
				memset(&font, 0, sizeof(font));
                //int len1 = sizeof(font.nFontHeight);
                //int len2 = sizeof(font.nWeight);
				r >> font.nFontHeight >> font.nWeight;
				r >> font.bItalic >> font.bUnderline;
				char	strFace[260];
				short len = 0;
				r >> len;
				r.pStream->read( strFace, len );
				strFace[len] = 0;
				fontIndex		=	p->AddFont( strFace, font.nFontHeight, font.nWeight, font.bItalic, font.bUnderline );
			}
		}
		//
		short uvCount	=	0;
		r >> uvCount;
		for(int i=0;i<uvCount;i++)
		{
			UVAnimation* e = new UVAnimation(this);
			e->Load(r);
			m_uvAnimations.push_back(e);
			if(e->m_bIsDefault)
			{
				m_defaultUVAni	=	e;
				this->ActiveUVAnimation(e);
			}
		}
		if(m_controlPtr && m_controlPtr->m_bEnableUnitAngle>0)
			RebuildUVAngles();
		m_textureMode.setIsFile();
		if(version>=24)
			r >> m_tileXieJiao;
		if(version>=25)
			r >> m_nTileOffX >> m_nTileOffY;
		if(version>=26)
			r >> m_nTileScaleX >> m_nTileScaleY;
		sr.end();
		if(version<22){
			this->m_bWrap = false;
		}
		return true;
	}

	BOOL			UIElement::SaveElement(UIManager* p, CSafeFileHelperW& w)
	{CPUTime(UIElement);
		int version = 26;
		w << version;
		skip_w sw(w.pStream);
		int length		=	sizeof(ElementData);
		w << length;
		ElementData* data	=	static_cast<ElementData*>(this);
		w.pStream->write(data, length);
		//
		TextureNode2D* pNode	=	p->GetTextureNode( textureIndex );
		if(textureIndex > 0)
		{
			short len = strlen(pNode->fileName.t);
			w << len;
			w.pStream->write(pNode->fileName.t, len);
		}
		FontNode2D* pFont		=	p->GetFontNode( fontIndex );
		if(fontIndex > 0)
		{
			w << pFont->nFontHeight << pFont->nWeight << pFont->bItalic << pFont->bUnderline;
			short len = strlen(pFont->fontName.t);
			w << len;
			w.pStream->write( pFont->fontName.t, len );
		}
		//
		short uvCount	=	this->m_uvAnimations.size();
		w << uvCount;
		for(int i=0;i<uvCount;i++)
		{
			m_uvAnimations[i]->Save(w);
		}
		w << m_tileXieJiao;
		w << m_nTileOffX << m_nTileOffY << m_nTileScaleX << m_nTileScaleY;
		sw.end();
		return true;
	}
	VOID	SetRegion(Rect* pRC, INT x, INT y, INT w, INT h)
	{
		pRC->left	=	x;
		pRC->top	=	y;
		pRC->right	=	pRC->left + w;
		pRC->bottom	=	pRC->top  + h;
	}

	VOID					UIElement::CopyFrom(UIElement* e)
	{CPUTime(UIElement);
		if(!e)
			return;
//		Name	temp	=	m_name.strA();
		*this = *static_cast<UIElement*>(e);
//		m_name	=	temp;
	}


};
