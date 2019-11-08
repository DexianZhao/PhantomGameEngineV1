//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_UIPROGRESS_H__
#define	__PHANTOM_UIPROGRESS_H__

#include "PhantomUIControl.h"
namespace Phantom{
	class UIProgress : public UIControl
	{
	public:
		UIProgress( BOOL bInit, UIDialog *dialogPtr = NULL );
		~UIProgress(){}

		virtual void 		Render( float fElapsedTime );
		virtual BOOL		isPtIn( Pixel pt ) { return false; }
		virtual	VOID		DrawProgress(const Rect& rc, float fProgress, unsigned int color);
		void 				SetText( const char* strText ){m_fProgress = (float)atof(strText); if(m_fProgress < 0.0f)m_fProgress = 0.0f; else if(m_fProgress > 1.0f)m_fProgress = 1.0f; OnChangeValue();}
		//
		virtual	INT			GetIntVal(){return GetValue();}
		virtual	double		GetDoubleVal(){return m_fProgress;}
		virtual	void 		SetIntVal( int nVal ){ SetValue(nVal); }
		virtual	void 		SetDoubleVal( double dVal ){ SetProgressValue(dVal); }

		void 				SetValue( int nValue );
		VOID				SetProgressValue(float f);
		VOID				OnChangeValue();

		int  				GetValue() const{return (int)(m_fProgress * (float)(m_maxValue - m_minValue)) + m_minValue;}
		float				GetProgressValue(){return m_fProgress;}
		virtual char 		onMouseMessage( unsigned int uMsg, Pixel pt, unsigned int touchIndex );
		virtual	char		LoadControl(CSafeFileHelperR& r);
		virtual	char		SaveControl(CSafeFileHelperW& w);
		//
		VOID				SetAutoTime(float fTime);
		float				m_fMaxTime,m_fNowTime;
		bool				m_bAutoTime;
		//
		static const int	MaxProgressElement	=	2;
		UIElement			m_pgElement[MaxProgressElement];
		unsigned int 		GetType() const { return UIControlType_PROGRESS; }
		char				m_chInnerSize;
	protected:
		float				m_fProgress;
		char				m_bLButtonDown;
	};

};

#endif
