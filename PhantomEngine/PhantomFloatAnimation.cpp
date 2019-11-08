//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#include "PhantomFloatAnimation.h"
namespace Phantom{

	FloatAnimation::FloatAnimation()
	{
		FloatKey dyn;
		this->m_dynKeys.push_back( dyn );
		this->m_dynKeys.push_back( dyn );
		this->m_fMinValue	=	0.0f;
		this->m_fMaxValue	=	1.0f;
		m_bEnabled			=	true;
		m_bVisible			=	true;
		m_bLockIndex		=	false;
		m_bIsColorAni		=	false;
		m_fNormalValue		=	0;
	}
	FloatAnimation&	FloatAnimation::operator = (const FloatAnimation& c)
	{
		m_bLockIndex = c.m_bLockIndex;
		m_bEnabled = c.m_bEnabled;
		m_bIsColorAni = c.m_bIsColorAni;
		m_bVisible = c.m_bVisible;
		m_frames = c.m_frames;
		m_dynKeys = c.m_dynKeys;
		m_fMaxValue = c.m_fMaxValue;
		m_fMinValue = c.m_fMinValue;
		m_fNormalValue = c.m_fNormalValue;
		return *this;
	}
	VOID			FloatAnimation::SetDefaultValue(float f)
	{CPUTime(FloatAnimation);
		m_fNormalValue	=	f;
		if(f < this->m_fMinValue)
			m_fMinValue	=	f;
		else if(f > this->m_fMaxValue)
			m_fMaxValue	=	f;
		for(int i=0;i<m_dynKeys.size();i++)
			m_dynKeys[i].value	=	f;
		this->CreateFrames();
	}

	FloatAnimation::~FloatAnimation()
	{
	}

	VOID						FloatAnimation::InitKey(BOOL bEnable, BOOL bVisible, float fMin, float fMax, float fDefault, BOOL bIsColor)
	{CPUTime(FloatAnimation);
		m_bIsColorAni	=	bIsColor;
		m_bEnabled	=	bEnable;
		m_bVisible	=	bVisible;
		m_fMinValue	=	fMin;
		m_fMaxValue	=	fMax;
		SetDefaultValue(fDefault);
	}

	VOID						FloatAnimation::CloneFrom(FloatAnimation* f)
	{CPUTime(FloatAnimation);
		m_dynKeys.clear();
		for(int i=0;i<f->m_dynKeys.size();i++)
			m_dynKeys.push_back(f->m_dynKeys[i]);
		this->m_fMinValue	=	f->m_fMinValue;
		this->m_fMaxValue	=	f->m_fMaxValue;
		m_bEnabled	=	f->m_bEnabled;
		m_bVisible	=	f->m_bVisible;
		this->CreateFrames();
	}

	float						FloatAnimation::GetFrameValue(float f)	//返回0~1之间的浮点值，会自动取模，根据m_frameCount大小精度会不一样
	{CPUTime(FloatAnimation);
		if(!this->m_bEnabled)
			return m_fNormalValue;
		if(m_frames.size() == 0 || !m_bVisible)
			return 0.0f;
		if(m_bLockIndex){
			f = fmodf(f, m_frames.size()-1);
		}
		f	=	clamp(f, 0.0f, m_frames.size());
		//
		float fIndex	=	f;// * (float)m_frames.size();
		int nIndex		=	(int)fIndex;
		float fLerp		=	fmodf(fIndex, 1.0f);
		//
		if(nIndex >= (m_frames.size() - 1))
			return m_frames[m_frames.size() - 1];
		return (m_frames[nIndex + 1] - m_frames[nIndex]) * fLerp + m_frames[nIndex];
	}

	VOID						FloatAnimation::SetRange(float fMin, float fMax)
	{CPUTime(FloatAnimation);
		m_fMinValue	=	fMin;
		m_fMaxValue	=	fMax;
		for(int i=0;i<m_dynKeys.size();i++)
			m_dynKeys[i].value	=	clamp(m_dynKeys[i].value, m_fMinValue, m_fMaxValue);
		CreateFrames();
	}

	float2   HermiteCurve(const float2& p1,const float2& p2,const float2& t1,const float2& t2, float fLerp)
	{
		double i2 = fLerp * fLerp;
		double i3 = i2 * fLerp;
		double express = 3 * i2 - 2 * i3;
		// 计算横坐标和纵坐标
		double x = (double)((1 - express) * p1.x + express * p2.x + (fLerp - 2 * i2 + i3) * t1.x + (i3 - i2) * t2.x);
		double y = (double)((1 - express) * p1.y + express * p2.y + (fLerp - 2 * i2 + i3) * t1.y + (i3 - i2) * t2.y);
		return float2(x,y);
	}

	float2   LineCurve(const float2& p1,const float2& p2, float fLerp)
	{
		double xx	=	p2.x - p1.x;
		double yy	=	p2.y - p1.y;
		return float2(p1.x + ((float)xx * fLerp), p1.y + ((float)yy * fLerp));
	}
	VOID						FloatAnimation::SetEnabled(BOOL b)
	{CPUTime(FloatAnimation);
		m_bEnabled = b;
		CreateFrames();
	}
	VOID						FloatAnimation::SetVisible(BOOL b)
	{CPUTime(FloatAnimation);
		m_bVisible = b;
		CreateFrames();
	}
	VOID						FloatAnimation::CreateFrames()	//构造每帧数据
	{CPUTime(FloatAnimation);
		if(!m_bVisible || !m_bEnabled)
		{
			m_frames.clear();
			return;
		}
		if(m_frames.size() == 0)
		{
			if(m_dynKeys.size() > 0)
			{
				int frameCount	=	m_dynKeys[m_dynKeys.size() - 1].frameIndex + 1;
				m_frames.SetArrayCount(frameCount);
			}
			else
				return;
		}
		float fRange	=	(m_fMaxValue - m_fMinValue);
		for(int i=1;i<this->m_dynKeys.size();i++)
		{
			FloatKey* trackFrom		=	&m_dynKeys[i - 1];
			FloatKey* trackTo		=	&m_dynKeys[i];
			//
			float2 begin(trackFrom->frameIndex, trackFrom->value);
			float2 end(trackTo->frameIndex, trackTo->value);
			int	count	=	(int)(trackTo->frameIndex - trackFrom->frameIndex);
			if(count <= 0)
				continue;
			float2 t1(trackFrom->t2.x * trackFrom->fTangentMul, -trackFrom->t2.y * trackFrom->fTangentMul);
			float2 t2(trackTo->t1.x * trackTo->fTangentMul, trackTo->t1.y * trackTo->fTangentMul);
			if(trackFrom->t2.IsEnable() && trackTo->t1.IsEnable())
			{
				for(int x=0;x<=count;x++)
				{
					float fLerp	=	(float)x / (float)count;
					float2 pos	=	HermiteCurve(begin, end, t1, t2, fLerp);
					pos.x		=	x + begin.x;
					assert(pos.x >= 0 && pos.x < this->m_frames.size());
					if(pos.x >= 0 && pos.x < this->m_frames.size())
						m_frames[(int)pos.x] = clamp(pos.y, m_fMinValue, m_fMaxValue);
				}
			}
			else
			{
				for(int x=0;x<=count;x++)
				{
					float fLerp	=	(float)x / (float)count;
					float2 pos	=	LineCurve(begin, end, fLerp);
					pos.x		=	fmodf(x + begin.x, (float)m_frames.size());
					//assert(pos.x >= 0 && pos.x < this->m_frames.size());
					if(pos.x >= 0 && pos.x < this->m_frames.size())
						m_frames[(int)pos.x] = clamp(pos.y, m_fMinValue, m_fMaxValue);
				}
			}
		}
	}

	VOID						FloatAnimation::SetTrackKeyCount(INT nFrame)	//设置点数量
	{CPUTime(FloatAnimation);
		if(!m_bVisible)
			return;
		if(nFrame < 2)
			nFrame	=	2;
		float fOldFrame	=	this->m_frames.size();
		if(fOldFrame < 2)
			fOldFrame	=	2;
		for(int i=0;i<m_dynKeys.size();i++)
			m_dynKeys[i].frameIndex	=	(int)((float)m_dynKeys[i].frameIndex / fOldFrame * (float)nFrame);
		m_dynKeys[0].frameIndex	=	0;
		m_dynKeys[m_dynKeys.size() - 1].frameIndex	=	nFrame - 1;
		this->CreateFrames();
	}

	int		OnShortFloatKey(const void* v1, const void* v2)
	{
		FloatKey* t1	=	(FloatKey*)v1;
		FloatKey* t2	=	(FloatKey*)v2;
		if(t1->frameIndex < t2->frameIndex)
			return -1;
		else if(t1->frameIndex > t2->frameIndex)
			return 1;
		return 0;
	}

	VOID						FloatAnimation::SetKey(INT nIndex, const FloatKey* k, BOOL bRefresh)
	{CPUTime(FloatAnimation);
		if(nIndex < 0 || nIndex >= (int)(m_dynKeys.size()))
			return;
		if(k->frameIndex < 0 || k->frameIndex >= (int)(m_frames.size()))
			return;
		if(k->value < this->m_fMinValue)
			m_fMinValue	=	k->value;
		else if(k->value > this->m_fMaxValue)
			m_fMaxValue	=	k->value;
		if(k->frameIndex == m_dynKeys[nIndex].frameIndex || nIndex == 0 || nIndex == ((int)m_dynKeys.size() - 1))
		{
			int old				=	m_dynKeys[nIndex].frameIndex;
			m_dynKeys[nIndex]	=	*k;
			m_dynKeys[nIndex].frameIndex	=	old;
		}
		else
		{
			for(int i=0;i<m_dynKeys.size();i++)
			{
				if(m_dynKeys[i].frameIndex == k->frameIndex)
				{
					return;
				}
			}
			m_dynKeys[nIndex]	=	*k;
		}
		//
		if(bRefresh)
		{
			qsort(m_dynKeys.GetPtr(), m_dynKeys.size(), sizeof(FloatKey), OnShortFloatKey);
			this->CreateFrames();
		}
	}

	INT							FloatAnimation::GetKeyCount()
	{CPUTime(FloatAnimation);
		return m_dynKeys.size();
	}

	const FloatKey*				FloatAnimation::GetKey(INT nIndex)
	{CPUTime(FloatAnimation);
		if(nIndex < 0 || nIndex >= (int)m_dynKeys.size())
			return 0;
		return &m_dynKeys[nIndex];
	}

	VOID						FloatAnimation::DeleteKeyByFrame(int nFrameIndex)
	{CPUTime(FloatAnimation);
		for(int i=0;i<m_dynKeys.size();i++)
		{
			if(m_dynKeys[i].frameIndex == nFrameIndex)
			{
				m_dynKeys.eraseByIndex(i);
				break;
			}
		}
	}
	VOID						FloatAnimation::DeleteKey(INT nIndex, BOOL bReset)
	{CPUTime(FloatAnimation);
		if(nIndex < 1 || nIndex >= ((int)m_dynKeys.size() - 1))
			return;
		m_dynKeys.eraseByIndex(nIndex);
		if(bReset)
			this->CreateFrames();
	}

	VOID						FloatAnimation::DeleteKeyAll()
	{CPUTime(FloatAnimation);
		m_dynKeys.clear();
	}

	INT							FloatAnimation::InsertKey(INT nFrameIndex, float fValue, BOOL bReset)
	{CPUTime(FloatAnimation);
		if(nFrameIndex < 0 || nFrameIndex >= this->m_frames.size())
			return -1;
		if(fValue < this->m_fMinValue)
			m_fMinValue	=	fValue;
		else if(fValue > this->m_fMaxValue)
			m_fMaxValue	=	fValue;
		//
		for(int i=0;i<m_dynKeys.size();i++)
		{
			if(m_dynKeys[i].frameIndex == nFrameIndex)
			{
				m_dynKeys[i].value	=	fValue;
				CreateFrames();
				return i;
			}
		}
		FloatKey dyn;
		dyn.value	=	fValue;
		dyn.frameIndex	=	nFrameIndex;
		m_dynKeys.push_back( dyn );
		if(!bReset)
			return (int)m_dynKeys.size() - 1;
		qsort(m_dynKeys.GetPtr(), m_dynKeys.size(), sizeof(FloatKey), OnShortFloatKey);
		this->CreateFrames();
		for(int i=0;i<m_dynKeys.size();i++)
		{
			if(m_dynKeys[i].frameIndex == nFrameIndex)
				return i;
		}
		return -1;
	}

	VOID						FloatAnimation::Load(CSafeFileHelperR& r)
	{CPUTime(FloatAnimation);
		char version;
		r >> version;
		skip_r sr;
		if(version > 5)
			sr.begin(r.pStream);
		r >> m_fMaxValue >> m_fMinValue;
		if(version >= 2 && version<= 5)
		{
			BOOL	bEnabled;
			BOOL	bVisible;
			r >> bEnabled >> bVisible;
			m_bEnabled = bEnabled;
			m_bVisible = bVisible;
		}
		else
			r >> m_bEnabled >> m_bVisible;
		int frameCount	=	0;
		if(version >= 3)
			r >> frameCount;
		if(version >= 5)
			r >> m_fNormalValue;
		short dynCount;
		r >> dynCount;
		if(dynCount>0){
			m_dynKeys.SetArrayCount(dynCount);
			if(version<= 5){
				for(int i=0;i<dynCount;i++)
					r >> m_dynKeys[i];
			}
			else
				r.pStream->read(m_dynKeys.GetPtr(), sizeof(FloatKey)*m_dynKeys.size());
		}
		sr.end();
		CreateFrames();
	}

	VOID						FloatAnimation::Save(CSafeFileHelperW& w)
	{CPUTime(FloatAnimation);
		char version	=	6;
		int frameCount	=	m_frames.size();
		w << version;
		skip_w sw(w.pStream);
		w << m_fMaxValue << m_fMinValue << m_bEnabled << m_bVisible << frameCount;
		w << m_fNormalValue;
		short dynCount	=	m_dynKeys.size();
		w << dynCount;
		if(dynCount>0)
			w.pStream->write(m_dynKeys.GetPtr(), sizeof(FloatKey) * m_dynKeys.size());
		sw.end();
	}

};
