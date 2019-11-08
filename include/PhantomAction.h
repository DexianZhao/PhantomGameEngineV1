//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   PhantomAction.h
软件名称   :   幻影游戏引擎

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___PHANTOM_ACTION_CONTROL__H___
#define ___PHANTOM_ACTION_CONTROL__H___


#pragma once
#include "PhantomBase.h"

namespace Phantom{
	class	ActionBase;
	//动作基础类
	class	ActionBase: public Base
	{
	public:
		ActionBase();
		virtual ~ActionBase();
		virtual	BOOL		OnFrameMove(float dElapsedTime) = 0;//如果返回false则将会被删除
		virtual	const void*	GetReceiver() = 0;
		VOID		SetNextAction(ActionBase* act){//设置下一个动作，这个动作播放完毕之后紧接着会播放下一个动作
			m_next = act;
			safe_addref(m_next);
		}
		virtual	VOID	Reset(){}//重置所有并重新播放动作
		VOID			SetBegin(){m_bBegin = true;}
		int				m_context;
	protected:
		ActionBase*		m_next;
		ActionBase*		m_current;
		BOOL			m_bBegin;
	};
	template <typename T, typename TReceiver>
	class	ActionLerp: public ActionBase
	{
	public:
		ActionLerp(TReceiver* rcv, const T begin ,const T end, float timeLength, int loopCount = 0, int context = 0)
		{
			m_current = this;
			m_loopCount = loopCount;
			m_timeCurrent = timeLength;
			m_timeLength = timeLength;
			m_begin	= begin;
			m_end	= end;
			m_receiver = rcv;
			m_context	=	context;
			m_saveLoop = m_loopCount;
		}
		~ActionLerp(){}
		template <typename Tnext>	ActionLerp<Tnext, TReceiver>*	Next(Tnext begin, Tnext end, float timeLength, int loopCount = 0, int context = 0)
		{
			ActionLerp<Tnext,TReceiver>* next = new ActionLerp<Tnext,TReceiver>(m_receiver, begin, end, timeLength, loopCount, context);
			m_next = next;
			return next;
		}
		ActionLerp<T, TReceiver>*	Next(T end, float timeLength, int loopCount = 0, int context = 0)
		{
			ActionLerp<T,TReceiver>* next = new ActionLerp<T,TReceiver>(m_receiver, m_end, end, timeLength, loopCount, context);
			m_next = next;
			return next;
		}
		virtual	const void*	GetReceiver(){return m_receiver;}
		inline	T	GetCurrent(){return (m_begin - m_end) * (m_timeCurrent / m_timeLength) + m_end;}
		virtual	VOID		Reset(){
			m_bBegin		=	true;
			m_timeCurrent	=	m_timeLength;
			m_loopCount		=	m_saveLoop;
			if(m_next)
				m_next->Reset();
			m_current		=	this;
		}//重置计数器
		BOOL		OnFrameMove(float dElapsedTime)
		{
			if(m_current == this){
				if(!m_bBegin)
					return false;
				m_timeCurrent -= dElapsedTime;
				if(m_timeCurrent <0)
				{
					m_loopCount--;
					if(m_next)
					{
						m_timeCurrent = 0;
						m_receiver->OnChange(this, m_end);
						m_next->Reset();//重新开始动作
						m_current = m_next;//如果有下一个动作则自动跳转下一个动作
						return true;
					}
					if(m_loopCount<=0)
					{
						m_timeCurrent = 0;
						m_receiver->OnChange(this, m_end);
						m_bBegin = false;
						return false;
					}
					m_timeCurrent += m_timeLength;
					m_timeCurrent = clamp(m_timeCurrent, 0, m_timeLength);
				}
				m_receiver->OnChange(this, GetCurrent());
				return true;
			}
			if(!m_current->OnFrameMove(dElapsedTime))
			{
				if(m_loopCount<=0)//如果自动做都播放完毕则如果不会再循环则结束
					return false;
				m_timeCurrent += m_timeLength;
				m_timeCurrent = clamp(m_timeCurrent, 0, m_timeLength);
				m_current = this;
			}
			return true;
		}
	public:
		T				m_begin;
		T				m_end;
		int				m_loopCount;
		float			m_timeCurrent;
		inline	float	GetTimeLength(){return m_timeLength;}
		inline	void	SetTimeLength(float f){m_timeLength = f; if(m_timeLength<0.001f)m_timeLength = 0.001f;}
		VOID			SetReceiver(TReceiver* t){m_receiver = t;}
	protected:
		TReceiver*		m_receiver;
		float			m_timeLength;
		int				m_saveLoop;
	};
	template <typename TReceiver>
	class	Action
	{
	public:
		Action(){m_next = 0;}
		~Action(){safe_delete(m_next);}
		template <typename Tnext>	ActionLerp<Tnext, TReceiver>*	Next(TReceiver* r, Tnext begin, Tnext end, float timeLength, int loopCount = 0, int context = 0)
		{
			ActionLerp<Tnext,TReceiver>* next = new ActionLerp<Tnext,TReceiver>(r, begin, end, timeLength, loopCount, context);
			m_next = next;
			return next;
		}
		virtual	BOOL		OnFrameMove(float dElapsedTime)
		{
			if(m_next)
				return m_next->OnFrameMove(dElapsedTime);
			return false;
		}
		virtual	VOID	Reset(){
			if(m_next)
				m_next->Reset();
		}//重置所有并重新播放动作
		ActionBase*		GetNext(){return m_next;}
	protected:
		ActionBase*	m_next;
	};
};

#endif
