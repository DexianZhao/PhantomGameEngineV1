//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_FLOAT_ANIMATION_H__
#define __PHANTOM_FLOAT_ANIMATION_H__
#include "PhantomBase.h"
#define	MAX_ANIMATION_KEY		200
#define	MAX_FLOAT_EDITOR		3

namespace Phantom {
	#pragma pack(push, 1)
	struct tangent2
	{
		tangent2(){flags = TANGENT_MIRROR;x=5.0f; y=.1f;}
		float	x,y;
		enum{
			TANGENT_ENABLE	=	0x01,
			TANGENT_MIRROR	=	0x02,	//另一个会跟随自己反方向移动
		};
		inline	BOOL	IsEnable(){return (flags & TANGENT_ENABLE);}
		inline	void	SetEnable(bool b){if(b)flags |= TANGENT_ENABLE; else if(flags & TANGENT_ENABLE)flags ^= TANGENT_ENABLE;}
		inline	BOOL	IsMirror(){return (flags & TANGENT_MIRROR);}
		inline	void	SetMirror(bool b){if(b)flags |= TANGENT_MIRROR; else if(flags & TANGENT_MIRROR)flags ^= TANGENT_MIRROR;}
		char	flags;
	};

	struct FloatKey
	{
		float						value;
		tangent2					t1,t2;
		float						fTangentMul;
		INT							frameIndex;
		FloatKey()
		{
			value		=	0;
			fTangentMul	=	8.0f;
			frameIndex	=	0;
		}
	};
	#pragma pack(pop)
	class FloatAnimation
	{
	public:
		FloatAnimation();
		virtual ~FloatAnimation();
		INT									GetKeyCount();
		const FloatKey*						GetKey(INT nIndex);
		VOID								SetKey(INT nIndex, const FloatKey* k, BOOL bRefresh = TRUE);
		INT									InsertKey(INT nFrameIndex, float fValue, BOOL bReset = true);
		VOID								DeleteKey(INT nIndex, BOOL bReset = true);
		VOID								DeleteKeyByFrame(int nFrameIndex);
		VOID								DeleteKeyAll();
		//
		VOID								SetTrackKeyCount(INT nFrame);	//设置点数量
		INT									GetFrameCount(){return m_frames.size();}
		VOID								CloneFrom(FloatAnimation* f);
		//
		float								GetFrameValue(float fLerp);	//返回0~1之间的浮点值，会自动取模，根据m_frameCount大小精度会不一样
		//
		inline	float						GetMaxValue(){return m_fMaxValue;}
		inline	float						GetMinValue(){return m_fMinValue;}
		VOID								SetRange(float fMin, float fMax);
		VOID								SetDefaultValue(float f);
		inline	VOID						SetNormalValue(float f){m_fNormalValue = f;}
		inline	float						GetNormalValue(){return m_fNormalValue;}
		//
		VOID								Load(CSafeFileHelperR& r);
		VOID								Save(CSafeFileHelperW& w);
		inline	BOOL						IsEnabled(){return m_bEnabled;}		//是否有效
		VOID								SetEnabled(BOOL b);
		inline	BOOL						IsVisible(){return m_bVisible;}		//是否有效
		VOID								SetVisible(BOOL b);
		inline	BOOL						IsReplay(){return m_bLockIndex;}
		inline	VOID						SetReplay(BOOL b){m_bLockIndex = b;}
		inline	BOOL						IsColorAni(){return m_bIsColorAni;}
		inline	VOID						SetColorAni(BOOL b){m_bIsColorAni = b;}
		//
		VOID								InitKey(BOOL bEnable, BOOL bVisible, float fMin, float fMax, float fDefault, BOOL bIsColor = false);
		//
		VOID								CreateFrames();	//构造每帧数据
		//
		FloatAnimation&	operator = (const FloatAnimation& c);
		//Name								m_name;
	protected:
		char								m_bLockIndex;
		char								m_bEnabled;
		char								m_bIsColorAni;
		char								m_bVisible;
		CDynamicArrayS<float,0>				m_frames;	//运行时数据
		CDynamicArrayS<FloatKey,0>			m_dynKeys;
		float								m_fMaxValue, m_fMinValue;	//最大最小值
		float								m_fNormalValue;
	};

};

#endif
