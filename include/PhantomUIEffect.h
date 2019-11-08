//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UIEFFECT_H__
#define	__PHANTOM_UIEFFECT_H__

#include "PhantomUIControl.h"
namespace Phantom{


struct Particle2D;
class UIEffect : public UIControl
{
public:
	UIEffect( BOOL bInit, UIDialog *dialogPtr = NULL );
	~UIEffect();

	virtual void 			Render( float fElapsedTime );
	virtual char 			canHaveFocus() { return (m_bVisible && IsEnabledCtrl() && m_bCanFocus); }
	virtual char 			onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex );

	virtual	char			LoadControl(CSafeFileHelperR& r);
	virtual	char			SaveControl(CSafeFileHelperW& w);
	char					m_bLButtonDown;
	INT						GetBorderWidth(){return m_borderWidth;}
	VOID					SetBorderWidth(int n){m_borderWidth = n;}
	//
	VOID					InitParticle(Particle2D* p, float fTimeLerp);
	BOOL					IsClipIn();	//是否包含进去了
	unsigned int 			GetType() const { return UIControlType_ui2DEffect; }
	//
	VOID					SetFrameCount(INT nCount);
	INT						GetFrameCount(){return m_frameCount;}
	virtual	BOOL			IsUseTrack(){return false;}
	VOID					CopyFrom(UIControl* e);
	virtual	VOID			OnPlayStart();
	//
	VOID					SetLoopCount(INT nCount){m_loopCount = nCount;}
	INT						GetLoopCount(){return m_loopCount;}
	INT						m_frameCount;
	INT						m_loopCount;
	BOOL					m_bStop;
	//
	Rect					m_saveRect;
	color4					m_saveColor;
	float					m_fLastTime;
	UIElement				m_bgElement;
	FloatAnimation			m_randoms[EffectRandom_Count];
	FloatAnimation*			GetAnimationPtr(EffectRandom t)
	{
		if(t < 0 || t >= EffectRandom_Count)
			return 0;
		return &m_randoms[t];
	}
protected:
	int							m_borderWidth;
	float						m_fTime;
	float						m_fLastCount;
	std::vector<Particle2D*>	m_particles;
};

};

#endif
