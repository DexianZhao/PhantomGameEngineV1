//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#include "PhantomUIStatic.h"
#include "PhantomUIDialog.h"
#include "PhantomUIManager.h"
#include "PhantomManager.h"
#include "PhantomFont.h"
#include "LuaManager.h"
#include "APILua.h"
#include "encrypt.h"

namespace Phantom{

	UIControl*		UIDialog::s_pControlFocus[MAX_TOUCH_COUNT] = {0,0,0,0,0,0,0,0};
	UIControl*		UIDialog::s_pControlPressed = NULL;
	//
	int				DialogAstar::IsAstarMoveEnable(int x, int y)	//是否可以移动到这里
	{CPUTime(DialogAstar);
		return m_dlgPtr->IsAstarMoveEnable(x, y);
	}
	int				DialogAstar::GetAstarWidth()					//地形宽度
	{CPUTime(DialogAstar);
		return m_dlgPtr->GetAstarWidth();
	}
	int				DialogAstar::GetAstarHeight()				//地形高度
	{CPUTime(DialogAstar);
		return m_dlgPtr->GetAstarHeight();
	}

	UIGroup::UIGroup()
	{
		m_fOpacity		=	1;
		m_bVisible		=	true;
		m_nGroupID		=	0;
		m_fSleepTime	=	0;
		m_dialogPtr		=	0;
		m_nPlayAction	=	ActionType_None;
		m_bIsUnit		=	false;
		m_bEnabled		=	true;
		m_fPlaySpeed	=	15.0f;
		m_fPlayTime		=	0.0f;
		m_nNextPlayAction	=	ActionType_Idle;
		m_movePos.Set(0, 0, 1, 1);
		m_groupPosition.Set(0,0);
		this->m_controlCenter = PtrID(0,0);
	}
	VOID				UIGroup::SetNextPlayAction(ActionType type)
	{CPUTime(UIGroup);
		m_nNextPlayAction	=	type;
	}
	UIControl*	UIGroup::GetControlFromName(const char* szName)
	{CPUTime(UIGroup);
		for(int i=0;i<this->m_controls.size();i++)
		{
			UIControl* c	=	m_dialogPtr->GetControl(this->m_controls[i]);
			if(c)
			{
				if(stricmp(c->m_name.t, szName) == 0)
					return c;
			}
		}
		return 0;
	}
	float				UIGroup::GetPlayTime()
	{CPUTime(UIGroup);
		return m_fPlayTime;
	}
	VOID				UIGroup::SetPlayTime(float f)
	{CPUTime(UIGroup);
		m_fPlayTime	=	f;
	}
	float				UIGroup::GetPlaySpeed()
	{CPUTime(UIGroup);
		return m_fPlaySpeed;
	}
	VOID				UIGroup::SetPlaySpeed(float f)
	{CPUTime(UIGroup);
		m_fPlaySpeed	=	f;
	}

	UIDialog*	UIGroup::GetDialogPtr()
	{CPUTime(UIGroup);
		return this->m_dialogPtr;
	}

	VOID	UIGroup::CopyFrom(UIGroup* in)
	{CPUTime(UIGroup);
		UIGroup* g		=	static_cast<UIGroup*>(in);
		m_fOpacity		=	g->m_fOpacity;
		m_bVisible		=	g->m_bVisible;
		m_fSleepTime	=	g->m_fSleepTime;
		m_nPlayAction	=	g->m_nPlayAction;
		m_bIsUnit		=	g->m_bIsUnit;
		m_bEnabled		=	g->m_bEnabled;
		this->m_name	=	g->m_name;
		m_movePos		=	g->m_movePos;
		for(int i=0;i<m_controls.size();i++)
		{
			UIControl* c	=	m_dialogPtr->FindControl(m_controls[i]);
			if(c)
			{
				m_dialogPtr->DeleteControlPtr(c);
			}
		}
		m_controls.clear();
		for(int i=0;i<g->m_controls.size();i++)
		{
			UIControl* fromC	=	g->m_dialogPtr->FindControl(g->m_controls[i]);
			if(fromC)
			{
				UIControl* toC		=	m_dialogPtr->CreateFromType(false, fromC->GetType());
				toC->CopyFrom(fromC);
				static_cast<UIControl*>(toC)->SetGroupID(this->m_nGroupID);
				//m_controls.push_back(Phantom::PtrID(0, toC->GetID()));
			}
		}
		m_fPlaySpeed	=	g->m_fPlaySpeed;
		m_fPlayTime		=	g->m_fPlayTime;
	}

	UIControl*				UIGroup::GetControlPtr(INT nIndex)
	{CPUTime(UIGroup);
		if(nIndex < 0 || nIndex >= m_controls.size())
			return 0;
		UIControl* fromC	=	m_dialogPtr->FindControl(m_controls[nIndex]);
		return fromC;
	}	//返回控件

	int		SortGroupControls(const void* v1, const void* v2)
	{
		UIControl* c1	=	*(UIControl**)v1;
		UIControl* c2	=	*(UIControl**)v2;
		if(c1->m_nLevel < c2->m_nLevel)
			return -1;
		if(c1->m_nLevel > c2->m_nLevel)
			return 1;
		return 0;
	}

	VOID	UIGroup::Resorts()
	{CPUTime(UIGroup);
		//if(!m_bIsUnit)
		//	return;
		//std::vector<UIControl*> controls;
		//for(int i=0;i<m_controls.size();i++)
		//{
		//	UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
		//	if(c)
		//		controls.push_back(static_cast<UIControl*>(c));
		//}
		//m_controls.clear();
		//if(controls.size() > 1)
		//{
		//	qsort(&controls.at(0), controls.size(), sizeof(UIControl*), SortGroupControls);
		//}
		//for(int i=0;i<controls.size();i++)
		//	m_controls.push_back(Phantom::PtrID(0, controls[i]->GetID()));
	}

	Pixel		UIGroup::GetPosition()
	{CPUTime(UIGroup);
		//m_groupPosition	=	Pixel(0, 0);
		//for(int i=0;i<m_controls.size();i++)
		//{
		//	UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
		//	if(c)
		//	{
		//		m_groupPosition	=	c->GetCenterPos();
		//		break;
		//	}
		//}
		return m_groupPosition;
	}
	VOID	UIGroup::SetLeft(int l)
	{CPUTime(UIGroup);
		int offX = l - m_movePos.left;
		m_movePos.left += offX;
		m_movePos.right += offX;
		for(int i=0;i<m_controls.size();i++)
		{
			UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
			if(c)
			{
				Rect rc = c->GetWindowRect();
				rc.left+=offX;
				rc.right+=offX;
				c->SetWindowRect(rc);
			}
		}
	}
	VOID	UIGroup::SetTop(int t)
	{CPUTime(UIGroup);
		int offY = t - m_movePos.top;
		m_movePos.top += offY;
		m_movePos.bottom += offY;
		for(int i=0;i<m_controls.size();i++)
		{
			UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
			if(c)
			{
				Rect rc = c->GetWindowRect();
				rc.top+=offY;
				rc.bottom+=offY;
				c->SetWindowRect(rc);
			}
		}
	}
	VOID	UIGroup::RecalcRect()
	{CPUTime(UIGroup);
		if(m_controls.size() == 0)
			return;
		m_movePos.Set(10000, 10000, -10000, -10000);
		for(int i=0;i<m_controls.size();i++)
		{
			UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
			if(c)//&&c->m_bVisible)
			{
				const Rect& rc = c->GetRenderRect();
				m_movePos.left = getmin(m_movePos.left, rc.left);
				m_movePos.top = getmin(m_movePos.top, rc.top);
				m_movePos.right = getmax(m_movePos.right, rc.right);
				m_movePos.bottom = getmax(m_movePos.bottom, rc.bottom);
			}
		}
	}

	VOID	UIGroup::SetPosition(const Pixel& pos)	//设置最新位置
	{CPUTime(UIGroup);
#ifdef EDITOR_OS
		m_controlCenter.id = 0;
#endif
		if(this->m_controlCenter.id<=0){
			for(int i=0;i<m_controls.size();i++)
			{
				UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
				if(c&&c->m_bIsGroupCenter){
					m_controlCenter.id = c->m_controlID;
				}
			}
		}
		UIControl* cnt = m_dialogPtr->FindControl(m_controlCenter);
		m_groupPosition = pos;
		Pixel off = m_groupPosition;
		if(cnt){
			off = m_groupPosition - cnt->GetLoadPos();
		}
		for(int i=0;i<m_controls.size();i++)
		{
			UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
			if(c)
			{
				Pixel lp = c->GetLoadPos() + off;
				//lp += m_groupPosition;
				c->SetPos(lp);
			}
			//if(c)
			//	c->SetCenterPos(pos);
		}
	}
	//
	VOID		UIGroup::SetUnitAngle(float fAngle)	//设置面对方向
	{CPUTime(UIGroup);
		for(int i=0;i<m_controls.size();i++)
		{
			UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
			if(c)
				c->SetUnitAngle(fAngle);
		}
	}
	float		UIGroup::GetUnitAngle()
	{CPUTime(UIGroup);
		for(int i=0;i<m_controls.size();i++)
		{
			UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
			if(c)
				return static_cast<UIControl*>(c)->m_fAngle;
		}
		return 0;
	}
	VOID		UIGroup::SetUnitAngleDir(const Phantom::float2& fAngle)	//设置面对方向
	{CPUTime(UIGroup);
		for(int i=0;i<m_controls.size();i++)
		{
			UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
			if(c)
				c->SetUnitAngleDir(fAngle);
		}
	}

	//
	//VOID	UIGroup::RebuildRects()	//重新构造矩形
	//{
	//	if(m_controls.size() == 0)
	//		this->m_windowRect.Set(0, 0, 0, 0);
	//	else
	//		this->m_windowRect.Set(-10000, -10000, 100000, 100000);
	//	for(int i=0;i<m_controls.size();i++)
	//	{
	//		UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
	//		if(c)
	//		{
	//			UIControl* cc	=	static_cast<UIControl*>(c);
	//			m_windowRect.left	=	getmin(m_windowRect.left, cc->m_windowRect.left);
	//			m_windowRect.top	=	getmin(m_windowRect.top, cc->m_windowRect.top);
	//			m_windowRect.right	=	getmax(m_windowRect.right, cc->m_windowRect.right);
	//			m_windowRect.bottom	=	getmax(m_windowRect.bottom, cc->m_windowRect.bottom);
	//		}
	//	}
	//}

	VOID	UIGroup::DeleteControls()
	{CPUTime(UIGroup);
		for(int i=0;i<m_controls.size();i++)
		{
			UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
			if(c)
				m_dialogPtr->DeleteControlPtr(c);
		}
		m_controls.clear();
	}
	VOID	UIGroup::RemoveControl(UIControl* c)
	{CPUTime(UIGroup);
		for(int i=0;i<m_controls.size();i++)
		{
			if(m_controls[i].id == c->GetID())
			{
				if(m_controls.size() <= 1)
					m_controls.clear();
				else
					m_controls.eraseByIndex(i);
				break;
			}
		}
		Resorts();
	}

	VOID	UIGroup::AddControl(UIControl* c)
	{CPUTime(UIGroup);
		if(this->m_nGroupID <= 0)
			return;
		for(int i=0;i<m_controls.size();i++)
		{
			if(m_controls[i].id == c->GetID())
				return;
		}
		m_controls.push_back(Phantom::PtrID(0, c->GetID()));
		Resorts();
	}

	UIGroup::~UIGroup()
	{
	}
	//
	VOID					UIGroup::SetVisible(BOOL b)
	{CPUTime(UIGroup);
		m_bVisible	=	b;
	}
	BOOL					UIGroup::GetVisible()
	{CPUTime(UIGroup);
		return m_bVisible;
	}
	VOID					UIGroup::SetName(const char* szName)
	{CPUTime(UIGroup);
		m_name	=	szName;
		OnSetName();
	}
	const char*				UIGroup::GetName()
	{CPUTime(UIGroup);
		return m_name.t;
	}
	VOID					UIGroup::OnSetName()
	{CPUTime(UIGroup);
		//short_t szBuf[128];
		//u_strcpy(szBuf, m_name.strW());
		//int len	=	u_strlen(szBuf);
		//for(int i=len-1;i>=0;i--)
		//{
		//	if(szBuf[i] == L':')
		//		szBuf[i] = 0;
		//}
		//short_t buf[128];
		//sprintf(buf, ":%d", this->m_nGroupID);
		//m_name	=	buf;
	}
	INT						UIGroup::GetID()
	{CPUTime(UIGroup);
		return m_nGroupID;
	}
	VOID					UIGroup::SetID(int nID)
	{CPUTime(UIGroup);
		m_nGroupID	=	nID;
		OnSetName();
	}
	VOID					UIGroup::PlayAction(const char* szName)
	{CPUTime(UIGroup);
		this->m_szPlayAction = szName;
		for(int i=0;i<m_controls.size();i++)
		{
			UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
			if(c)
				c->PlayAction(szName);
		}
	}
	VOID					UIGroup::PlayAction(ActionType type)
	{CPUTime(UIGroup);
		m_nPlayAction	=	type;
		m_fPlayTime		=	0;
		for(int i=0;i<m_controls.size();i++)
		{
			UIControl* c = m_dialogPtr->FindControl(m_controls[i]);
			if(c)
				c->PlayAction(type);
		}
	}
	VOID					UIGroup::Load(CSafeFileHelperR& r)
	{CPUTime(UIGroup);
		char version = 1;
		r >> version;
		skip_r sr;
		if(version>=9)
			sr.begin(r.pStream);
		r >> m_bVisible >> m_nGroupID;
		if(version < 2)
		{
			std::string str;
			r >> str;
			m_name	=	str.c_str();
		}
		else
		{

			NameAW temp;
			if(version < 7){
				CDynamicArray<short_t> str;
				r >> str;
				temp.setW(str.GetPtr());
			}
			else if(version<8)
			{
				r >> temp;
			}
#ifdef WIN32_OS
			if(version<8){
				char buf[256];
				int len = ::WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)temp.strW(), temp.sizeW(), buf, 256, 0, 0);	
				buf[len] = 0;
				m_name = buf;
			}
#endif
			if(version>=8)
				r >> m_name;
		}
		if(version >= 3)
			r >> m_fSleepTime;
		if(version >= 6)
			r >> m_fPlayTime >> m_fPlaySpeed;
		if(version >= 5)
		{
			r >> m_bIsUnit >> m_bEnabled;
			int controls;
			r >> controls;
			if(controls > 0)
			{
				m_controls.resize(controls);
				r.pStream->read(m_controls.GetPtr(), sizeof(Phantom::PtrID) * m_controls.size());
			}
		}
		sr.end();
	}
	VOID					UIGroup::Save(CSafeFileHelperW& w)
	{CPUTime(UIGroup);
		char version = 9;
		w << version;
		skip_w sw(w.pStream);
		w << m_bVisible << m_nGroupID;
		w << m_name;
		w << m_fSleepTime;
		w << m_fPlayTime << m_fPlaySpeed;
		w << m_bIsUnit << m_bEnabled;
		int controls	=	m_controls.size();
		w << controls;
		if(controls > 0)
			w.pStream->write(m_controls.GetPtr(), sizeof(Phantom::PtrID) * m_controls.size());
		sw.end();
	}

	UIDialog::UIDialog()
	{
		m_nLastAlignMode	=	-1;
		m_bUpdate			=	true;
		m_childDownTime		=	0;
		m_childAutoMove		=	0;
		m_childMoveStrength.setxy(0,0);//移动强度
		m_childDownMove.Set(0,0);
		m_childBeforeMove.Set(0,0);
		m_childDownMoved	=	0;
		m_childDownIndex	=	-1;
		m_childDown.Set(0,0);
		m_bIsHold			=	false;
		//
		m_pos.Set(0,0);
		m_mapGridBegin.Set(0,0);
		m_dlgParent			=	0;
		m_centerObj			=	0;
		m_bIsPause			=	false;
		m_pakdir			=	0;
		m_bEnabled			=	true;
		m_bRequestResort	=	true;
		m_bMap25DMode		=	false;
		m_pParentControl	=	0;
		m_lastDlgX	=	m_lastDlgY	=	0;
		m_nAvatarRef		=	0;
		m_bIsBackground		=	false;
		m_nContextID		=	0;
		m_bEnableDlgMatrix	=	false;
		m_dlgMatrix.identity();
		matrixInverse(&m_dlgMatrixInv, NULL, &m_dlgMatrix);
		m_screenSize.Set(1024, 768);
		m_mapAstarGridCount.Set(0,0);
		m_bIsToolTipWindow	=	false;
		m_szName[0]			=	0;
		m_nControlID		=	0;
		m_x 				= 	0;
		m_y 				= 	0;
		m_dlgX				=	0;
		m_dlgY				=	0;
		m_width				= 	0;
		m_height			= 	0;
		//m_scale.setxy(1,1);
		m_dlgCenter.Set(0,0);
		m_bVisible			=	true;
		m_bDrag				=	false;
		m_colorTopLeft		=	0;
		m_renderCB			=	0;
		m_colorTopRight		=	0;
		m_colorBottomLeft	=	0;
		m_colorBottomRight	=	0;
		m_pControlMouseOver = NULL;
		m_nDefaultControlID = 0xffff;
		m_astarCallback.m_dlgPtr	=	this;
		m_bHideOnKillFocus	=	FALSE;
		this->m_nDialogID	=	0;
		m_dlgRotation.setxyz(0,0,0);
		m_dlgScale.setxy(1,1);
		m_dlgColor			=	Phantom::color4(1,1,1,1);
		m_bTopMost			=	FALSE;
		m_nDlgOrder			=	1;
		m_nElementID		=	1;
		m_bMapGridVisible	=	false;
		m_bMapEnable		=	false;
		m_mapGridCount		=	Size(1,1);
		m_mapGridSize		=	Size(16,16);
		m_bFullScreen		=	false;
		m_layer				=	0;
		//
		m_nPlayAction		=	ActionType_None;
		m_astar.setupMap(&this->m_astarCallback);
		m_defaultGroup.m_dialogPtr	=	this;
		m_defaultGroup.m_nGroupID	=	0;
		m_wayFrom.setxy(0,0);
		m_wayDir.setxy(1,1);
		m_wayCurr	=	m_wayLength	=	0;
		m_waySpeed	=	1;
		m_wayMode	=	0;
		m_bIsChildRender	=	false;
#ifndef EXPORT_ENGINE
		//m_box2World.SetDialogPtr(this);
#endif
	}
	void		UIDialog::OnHold(BOOL b)
	{
		if(b&&!this->m_bIsHold){
			m_bIsHold = true;
			this->AddRef();
		}
		else if(!b&&m_bIsHold){
			m_bIsHold = false;
			this->OnLoadLua();
			this->Release();
		}
	}

	VOID		UIDialog::ClipChildOffset(int&x,int&y)
	{CPUTime(UIDialog);
		if(!m_pParentControl)
			return;
		int w = GetWidth();
		int h = GetHeight();
		int regionW = m_pParentControl->m_windowRect.GetWidth();
		int regionH = m_pParentControl->m_windowRect.GetHeight();
		int wMin = regionW-15;
		int wMax = -w+15;
		int hMin = regionH-15;
		int hMax = -h+15;
		if(m_pParentControl->m_scrollPtrH&&!m_pParentControl->m_scrollPtrH->m_bScrollExtend)
		{
			if(w<regionW)w=regionW;
			wMin = 0;
			wMax = -w+regionW;
		}
		if(m_pParentControl->m_scrollPtrV&&!m_pParentControl->m_scrollPtrV->m_bScrollExtend)
		{
			if(h<regionH)h=regionH;
			hMin = 0;
			hMax = -h+regionH;
		}
		if(x>wMin)
			x=wMin;
		if(y>hMin)
			y=hMin;
		if(x<wMax)
			x=wMax;
		if(y<hMax)
			y=hMax;
	}

	BOOL		UIDialog::OnChildInput(unsigned int uMsg, Pixel pt, unsigned int touchIndex)
	{CPUTime(UIDialog);
		if(!m_pParentControl)
			return false;
        g_manager.SetUIDialogSize(true, m_screenSize.cx, m_screenSize.cy, 0);
		if(uMsg == Phantom::InputEventID_ButtonDown){
			g_manager.GetCursorPos2D(m_childDownMove, touchIndex);
            m_childBeforeMove = m_childDownMove;
			int x,y;
			GetLocation(x,y);
			m_childDown.Set(x,y);
			m_childDownIndex	=	touchIndex;
			m_childDownMoved	=	false;
			m_childAutoMove		=	false;
			m_childMoveStrength.setxy(0,0);
			m_childDownTime		=	g_manager.GetAppTime();
		}
		else if(uMsg == Phantom::InputEventID_MouseMove && m_childDownIndex == touchIndex){
			m_childDownMoved	=	true;
			Pixel temp;
			g_manager.GetCursorPos2D(temp, touchIndex);
			Pixel offset = temp - m_childDownMove;
			int x,y;
			GetLocation(x,y);
			if(m_pParentControl->m_scrollPtrH)
                x = m_childDown.x + offset.x;
			if(m_pParentControl->m_scrollPtrV)
				y = m_childDown.y + offset.y;
			ClipChildOffset(x,y);
			SetLocation(x,y);
			//m_childDown = temp;
			return false;
		}
		else if(uMsg == Phantom::InputEventID_ButtonUp){
			m_childDownIndex = -1;
			if(m_childDownMoved){
				m_childDownMoved = false;
				float fOffTime = g_manager.GetAppTime() - m_childDownTime;
				if(fOffTime<0.5){
					Pixel temp;
					g_manager.GetCursorPos2D(temp, touchIndex);
					Pixel offset = temp - m_childDownMove;
					m_childAutoMove	=	true;
					m_childMoveStrength.setxy(offset.x, offset.y);
					m_childMoveStrength *= (0.5 - fOffTime);
					if(m_pParentControl->m_scrollPtrV)
						m_childMoveStrength.y *= m_pParentControl->m_scrollPtrV->m_fScrollFirstSpeed;
					if(m_pParentControl->m_scrollPtrH)
						m_childMoveStrength.x *= m_pParentControl->m_scrollPtrH->m_fScrollFirstSpeed;
					int w = GetWidth();
					int h = GetHeight();
					int regionW = this->m_pParentControl->m_windowRect.GetWidth();
					int regionH = this->m_pParentControl->m_windowRect.GetHeight();
					int x,y;
					GetLocation(x,y);
					if(m_childMoveStrength.x>0 && x>0)
						m_childMoveStrength.x = 0;
					else if(m_childMoveStrength.x<0&&x<(regionW-w))
						m_childMoveStrength.x = 0;
					if(m_childMoveStrength.y>0 && y>0)
						m_childMoveStrength.y = 0;
					else if(m_childMoveStrength.y<0&&y<(regionH-h))
						m_childMoveStrength.y = 0;
				}
			}
		}
		return false;
	}
	//VOID			UIDialog::SetScale(float fx, float fy)
	//{
	//	m_scale.setxy(fx,fy);
	//	for(int i=0;i<m_controls.size();i++)
	//	{
	//		m_controls[i]->SetScale(fx, fy, &this->m_dlgCenter);
	//	}
	//}

	UIGroup*		UIDialog::AddGroupPtr()	//新加一个组合
	{CPUTime(UIDialog);
		UIGroup* g	=	new UIGroup();
		g->m_dialogPtr	=	this;
		g->m_nGroupID	=	this->m_nControlID++;
		this->m_groups.push_back(g);
		return g;
	}
	UIGroup*		UIDialog::FindGroupByName(const char* szName)		//查找组合ID
	{CPUTime(UIDialog);
		for(int i=0;i<m_groups.size();i++)
		{
			if(!m_groups[i])
				continue;
			if(stricmp(m_groups[i]->m_name.t, szName) == 0)
				return m_groups[i];
		}
		if(stricmp(m_defaultGroup.m_name.t, szName) == 0)
			return &m_defaultGroup;
		return 0;
	}
	UIGroup*		UIDialog::FindGroupPtr(int id)		//查找组合ID
	{CPUTime(UIDialog);
		for(int i=0;i<m_groups.size();i++)
		{
			if(m_groups[i]->m_nGroupID == id)
				return m_groups[i];
		}
		return &m_defaultGroup;
	}
	UIGroup*		UIDialog::FindGroupPtr(Phantom::PtrID& id)		//查找组合ID
	{CPUTime(UIDialog);
		if(id.index < m_groups.size() && m_groups[id.index]->m_nGroupID == id.id)
			return m_groups[id.index];
		if(id.id == 0)
			return &m_defaultGroup;
		for(int i=0;i<m_groups.size();i++)
		{
			if(m_groups[i]->m_nGroupID == id.id)
			{
				id.index	=	i;
				id.id = m_groups[i]->m_nGroupID;
				return m_groups[i];
			}
		}
		id.id	=	0;
		return &m_defaultGroup;
	}
	VOID			UIDialog::RemoveGroupPtr(Phantom::PtrID& id)	//删除组和其内部的所有控件
	{CPUTime(UIDialog);
		if(id.id == 0)
			return;
		for(int i=0;i<m_groups.size();i++)
		{
			if(m_groups[i]->GetID() == id.id)
			{
				m_groups[i]->DeleteControls();
				delete m_groups[i];
				m_groups.erase(m_groups.begin() + i);
				break;
			}
		}
	}
	UIControl*		UIDialog::FindControl(Phantom::PtrID& id)	//根据ID查找控件
	{CPUTime(UIDialog);
		if(id.index < m_controls.size() && m_controls[id.index]->m_controlID == id.id)
			return m_controls[id.index];
		if(id.id == 0)
			return 0;
		for(int i=0;i<m_controls.size();i++)
		{
			if(m_controls[i]->m_controlID == id.id)
			{
				id.index	=	i;
				id.id = m_controls[i]->m_controlID;
				return m_controls[i];
			}
		}
		id.id	=	0;
		return 0;
	}

	VOID			UIDialog::PlayAction(ActionType type)
	{CPUTime(UIDialog);
		m_nPlayAction	=	type;
		for(int i=0;i<this->m_controls.size();i++)
		{
			this->m_controls[i]->PlayAction(type);
		}
	}
	VOID			UIDialog::PlayAction(const char* szName)
	{CPUTime(UIDialog);
		this->m_szPlayAction = szName;
		for(int i=0;i<this->m_controls.size();i++)
		{
			this->m_controls[i]->PlayAction(szName);
		}
	}
	VOID			UIDialog::RaiseEvent(UIControl* c, INT nEventID, const Pixel& pos)
	{CPUTime(UIDialog);
		if(nEventID==Phantom::EventID_Down){
			if(c->m_soundFile.t[0])
				Global_playsnd(c->m_soundFile, 1, 0, 1);
		}
		if(g_luamanager.GetLuaState())
		{
			for(int i=0;i<c->m_events.size();i++)
			{
				if(c->m_events[i].id ==nEventID && c->m_events[i].func.t[0]){
					lua_UIControl_s mgr;
					SetIPtr(&mgr.m_data, &c->m_this, PhantomUIControlBase, IControlPtr);
					lua_Pixel p;
					p.Set(pos.x, pos.y);
					lua_tinker::call<void>(g_luamanager.GetLuaState(), c->m_events[i].func.t, mgr, nEventID, p);
					break;
				}
			}
		}
		if(c && g_manager.GetActiveSDK())
			g_manager.GetActiveSDK()->OnUIEvent((EventID)nEventID, this, c, 0);
	}

	INT				UIDialog::GetGroupCount()
	{CPUTime(UIDialog);
		return (int)m_groups.size() + 1;
	}

	UIGroup*		UIDialog::GetGroupPtr(INT nIndex)
	{CPUTime(UIDialog);
		if(nIndex < 1 || nIndex > m_groups.size())
			return &m_defaultGroup;
		nIndex--;
		return m_groups[nIndex];
	}

	void			UIDialog::SetTransparent(float f)
	{CPUTime(UIDialog);
		this->m_dlgColor.a	=	f;
	}

	float			UIDialog::GetTransparent()
	{CPUTime(UIDialog);
		return m_dlgColor.a;
	}

	void			UIDialog::SetShowSound(const char* szFile)
	{CPUTime(UIDialog);
		if(szFile)
		{
			m_showDlgSound	=	szFile;
		}
		else
		{
			m_showDlgSound.clear();
		}
	}

	BOOL			UIDialog::SetControlOrder(UIControl* pC, BOOL bUpDown)
	{CPUTime(UIDialog);
		for(size_t i=0;i<this->m_controls.size();i++)
		{
			if(m_controls[i] == pC)
			{
				if(bUpDown)
				{
					if(i > 0)
					{
						UIControl* p = m_controls.at(i - 1);
						m_controls[i - 1] = m_controls[i];
						m_controls[i] = p;
						return true;
					}
				}
				else
				{
					if(i < (int)this->m_controls.size() - 1)
					{
						UIControl* p = m_controls[i + 1];
						m_controls[i + 1] = m_controls[i];
						m_controls[i] = p;
						return true;
					}
				}
			}
		}
		return FALSE;
	}

	UIControl*		UIDialog::GetControlFromName(const char* szName)
	{CPUTime(UIDialog);
		for(int i=0;i<m_controls.size();i++)
		{
			if(stricmp(m_controls[i]->GetName(), szName) == 0)
				return m_controls[i];
		}
		return 0;
	}
	void			UIDialog::RecalcRect(BOOL bIncludeVisible)
	{CPUTime(UIDialog);
		m_renderRect.Set(10000, 10000, 0, 0);
		m_width		=	0;
		m_height	=	0;
		for(size_t i=0;i<this->m_controls.size();i++)
		{
			if(!bIncludeVisible&&!m_controls[i]->m_bVisible)continue;
			if(!m_controls[i]->GetGroupPtr()->m_bVisible)continue;
			Rect rect			=	m_controls[i]->m_windowRect;
			rect.left			+=	this->m_x;
			rect.right			+=	this->m_x;
			rect.top			+=	this->m_y;
			rect.bottom			+=	this->m_y;
			m_renderRect.left	=	getmin(m_renderRect.left, rect.left);
			m_renderRect.top	=	getmin(m_renderRect.top, rect.top);
			m_renderRect.right	=	getmax(m_renderRect.right, rect.right);
			m_renderRect.bottom	=	getmax(m_renderRect.bottom, rect.bottom);
		}
		if(m_renderRect.left==10000)
			m_renderRect.Set(0,0,0,0);
		m_width = m_renderRect.GetWidth();
		m_height = m_renderRect.GetHeight();
		if(this->m_pParentControl)
			this->m_pParentControl->RecalcScrollValue();
	}
	void			UIDialog::RebuildRenderRect()
	{CPUTime(UIDialog);
		m_renderRect.Set(10000, 10000, 0, 0);
		m_width		=	0;
		m_height	=	0;
		for(size_t i=0;i<this->m_controls.size();i++)
		{
			Rect rect			=	(Rect&)m_controls[i]->m_windowRect;
			//m_width				=	getmax(m_width, rect.right);
			//m_height			=	getmax(m_height, rect.bottom);
			rect.left			+=	this->m_x;
			rect.right			+=	this->m_x;
			rect.top			+=	this->m_y;
			rect.bottom			+=	this->m_y;
			m_renderRect.left	=	getmin(m_renderRect.left, rect.left);
			m_renderRect.top	=	getmin(m_renderRect.top, rect.top);
			m_renderRect.right	=	getmax(m_renderRect.right, rect.right);
			m_renderRect.bottom	=	getmax(m_renderRect.bottom, rect.bottom);
		}
		m_width = m_renderRect.GetWidth();
		m_height = m_renderRect.GetHeight();
		int offsetX	=	m_renderRect.left - m_x;
		int offsetY	=	m_renderRect.top - m_y;
		if(m_controls.size() > 0)
			AlignDialog();
		int width	=	m_renderRect.GetWidth();
		int height	=	m_renderRect.GetHeight();
		//
		m_renderRect.left	=	m_x + offsetX;
		m_renderRect.top	=	m_y + offsetY;
		m_renderRect.right	=	m_renderRect.left + width;
		m_renderRect.bottom	=	m_renderRect.top + height;
		m_nFullscreenWidth	=	m_width;
		m_nFullscreenHeight	=	m_height;
	}

	VOID			UIDialog::AlignDialog()
	{CPUTime(UIDialog);
		int left	=	m_x;//m_renderRect.left;
		int top		=	m_y;//m_renderRect.top;
		int width	=	m_renderRect.GetWidth();
		int height	=	m_renderRect.GetHeight();
		//
		int screenW	=	g_manager.GetBufferWidth();//.GetUIBufferWidth();
		int screenH	=	g_manager.GetBufferHeight();//.GetUIBufferHeight();
		//LogInfo("UIDialog::AlignDialog(%s,%d/%d,%d/%d)", this->m_szName,width,screenW,height,screenH);
		//
		/*if(m_nDlgAlign==0)
		{
		}
		else
		{
			if(m_nDlgAlign&UIDialogAlign_LEFT)
			{
				left	=	0;
				left	+=	this->m_dlgAlignOffset.x;
			}
			else if(m_nDlgAlign&UIDialogAlign_CENTER)
			{
				left	=	(screenW - width) / 2;
				left	+=	this->m_dlgAlignOffset.x;
			}
			else if(m_nDlgAlign&UIDialogAlign_RIGHT)
			{
				left	=	screenW - width;
				left	+=	this->m_dlgAlignOffset.x;
			}
			if(m_nDlgAlign&UIDialogAlign_TOP)
			{
				top	=	0;
				top	+=	this->m_dlgAlignOffset.y;
			}
			else if(m_nDlgAlign&UIDialogAlign_VCENTER)
			{
				top	=	(screenH - height) / 2;
				top	+=	this->m_dlgAlignOffset.y;
			}
			else if(m_nDlgAlign&UIDialogAlign_BOTTOM)
			{
				top	=	screenH - height;
				top	+=	this->m_dlgAlignOffset.y;
			}
		}*/
		m_x	=	left;// - m_renderRect.left);
		m_y	=	top;// - m_renderRect.top);
		for(int i=0;i<this->m_controls.size();i++)
		{
			UIControl* c = m_controls[i];
			if(c->m_nAlignMode==0)
				continue;
			g_manager.SetUIDialogSize(false, m_screenSize.cx, m_screenSize.cy, 0);
			Rect loadRect = c->m_loadRect;
			int offL = loadRect.left;
			int offT = loadRect.top;
			int offR = (m_screenSize.cx - loadRect.right);
			int offB = (m_screenSize.cy - loadRect.bottom);
			//
			int screenW = g_manager.GetUIDialogSizeW();
			int screenH = g_manager.GetUIDialogSizeH();
			int spaceW = (screenW - m_screenSize.cx)/2;
			int spaceH = (screenH - m_screenSize.cy)/2;
			//
			int loadW = c->m_loadRect.GetWidth();
			int loadH = c->m_loadRect.GetHeight();
            int middleX = loadW/2 + c->m_loadRect.left;
            int middleY = loadH/2 + c->m_loadRect.top;

			//Pixel newc((c->m_loadRect.left+c->m_loadRect.right)/2, (c->m_loadRect.top+c->m_loadRect.bottom)/2);
			//
			if((c->m_nAlignMode & 0x01)>0)//左对齐
			{
			}
			else if((c->m_nAlignMode & 0x02)>0)//剧中对齐
			{
				loadRect.left = middleX*screenW/m_screenSize.cx - loadW/2;
				loadRect.right = loadRect.left + loadW;
//				loadRect.left = spaceW;//(screenW-loadW)/2;
//				loadRect.right = loadRect.left + loadW;
			}
			else if((c->m_nAlignMode & 0x04)>0)//右对齐
			{
				loadRect.right = screenW - offR;
				loadRect.left = loadRect.right - loadW;
			}
			else if((c->m_nAlignMode & 0x08)>0)//两边对齐
			{
				int off = (m_screenSize.cx/2)-loadRect.left;
				loadRect.left = screenW/2 - off;// - loadW/2;
				loadRect.right = loadRect.left + loadW;
				//loadRect.left = offL;
				//loadRect.right = screenW - offR;
			}
			if((c->m_nAlignMode & 0x10)>0)//上对齐
			{
			}
			else if((c->m_nAlignMode & 0x20)>0)//剧中对齐
			{
				loadRect.top = middleY*screenH/m_screenSize.cy - loadH/2;
				loadRect.bottom = loadRect.top + loadH;
//				loadRect.top = spaceH;//(screenH - loadH)/2;
//				loadRect.bottom = loadRect.top + loadH;
			}
			else if((c->m_nAlignMode & 0x40)>0)//下对齐
			{
				loadRect.bottom = screenH - offB;
				loadRect.top = loadRect.bottom - loadH;
			}
			else if((c->m_nAlignMode & 0x80)>0)//两边对齐
			{
				int off = (m_screenSize.cy/2)-loadRect.top;
				loadRect.top = screenH/2 - off;
				loadRect.bottom = loadRect.top + loadH;
				//loadRect.top = offT;
				//loadRect.bottom = screenH - offB;
			}
			c->m_loadRect = loadRect;
			c->SetRenderRect(loadRect);
		}
		//int bufferW = g_manager.GetBufferWidth();
		//int bufferH = g_manager.GetBufferHeight();
		//int gameW = g_manager.GetUIDialogSizeW();
		//int gameH = g_manager.GetUIDialogSizeH();
		//float fX = (float)bufferW / (float)gameW;
		//float fY = (float)bufferH / (float)gameH;
		//float fScale = getmin(fX, fY);
		//gameW = (float)gameW*fScale;
		//gameH = (float)gameH*fScale;
		//int offsetW = (float)(bufferW-gameW)/fScale/2;
		//int offsetH = (float)(bufferH-gameH)/fScale/2;
		//for(int i=0;i<this->m_controls.size();i++)
		//{
		//	UIControl* c = m_controls[i];
		//	Pixel newc((c->m_loadRect.left+c->m_loadRect.right)/2, (c->m_loadRect.top+c->m_loadRect.bottom)/2);
		//	switch(c->m_nAlignMode)
		//	{
		//	case 2://左对齐"), 2);
		//		newc.x -= offsetW;
		//		break;
		//	case 3://右对齐"), 3);
		//		newc.x += offsetW;
		//		break;
		//	case 4://上对齐"), 4);
		//		newc.y -= offsetH;
		//		break;
		//	case 5://下对齐"), 5);
		//		newc.y += offsetH;
		//		break;
		//	case 6://左上对齐"), 6);
		//		newc.x -= offsetW;
		//		newc.y -= offsetH;
		//		break;
		//	case 7://左下对齐"), 7);
		//		newc.x -= offsetW;
		//		newc.y += offsetH;
		//		break;
		//	case 8://右上对齐"), 8);
		//		newc.x += offsetW;
		//		newc.y -= offsetH;
		//		break;
		//	case 9://右下对齐"), 9);
		//		newc.x += offsetW;
		//		newc.y += offsetH;
		//		break;
		//	case 10://所有边对齐
		//		{
		//			Rect rc = c->m_windowRect;
		//			rc.left = c->m_loadRect.left - offsetW;
		//			rc.right = c->m_loadRect.right + offsetW;
		//			rc.top = c->m_loadRect.top - offsetH;
		//			rc.bottom = c->m_loadRect.bottom + offsetH;
		//			c->SetRenderRect(rc);
		//		}
		//		break;
		//	default:
		//		continue;
		//	}
		//	Rect rc = c->m_windowRect;
		//	Pixel center((rc.left+rc.right)/2, (rc.top+rc.bottom)/2);
		//	rc.left = (rc.left-center.x)+newc.x;
		//	rc.right = (rc.right-center.x)+newc.x;
		//	rc.top = (rc.top-center.y)+newc.y;
		//	rc.bottom = (rc.bottom-center.y)+newc.y;
		//	c->SetRenderRect(rc);
		//}
	}

	const char*		UIDialog::GetName()
	{CPUTime(UIDialog);
		return m_szName;
	}

	void			UIDialog::SetName(const char* s)
	{CPUTime(UIDialog);
		strcpy(m_szName, s);
	}

	UIDialog::~UIDialog()
	{
		safe_release(m_pakdir);
		if(m_pParentControl)
			m_pParentControl->m_pChildDialog = 0;
		m_astar.destroy();
		if(GetDlgManager())
			GetDlgManager()->OnDeleteDlg(this);
		RemoveAllControls();
		for(int i=0;i<m_groups.size();i++)
			delete m_groups[i];
		m_groups.clear();
		ClearChilds();
	}
	void UIDialog::SetBackgroundWindow(char b)
	{CPUTime(UIDialog);
		m_bIsBackground = b;
		GetDlgManager()->SetTopMost(0, 0);
	}

	char UIDialog::IsBackgroundWindow()
	{CPUTime(UIDialog);
		return m_bIsBackground;
	}

	void UIDialog::SetTopMost(BOOL bTop)
	{CPUTime(UIDialog);
		this->m_bTopMost	=	bTop;
		GetDlgManager()->SetTopMost(this, bTop);
	}

	void UIDialog::SetVisible( char bVisible )
	{CPUTime(UIDialog);
		m_bVisible = bVisible;
		if(GetDlgManager()->m_pModalDlg == this && !m_bVisible)
			GetDlgManager()->m_pModalDlg	=	0;
		if(m_bVisible)
		{
			this->PlayAction(ActionType_Idle);
			for(int i=0;i<this->m_controls.size();i++)
			{
				if(this->m_controls[i]->m_bTrackAutoBeginAni)
					m_controls[i]->PlayTrack();
				m_controls[i]->FrameMove(0, 0, true);
			}
		}
	}
	BOOL UIDialog::IsDoModal()
	{CPUTime(UIDialog);
		return (GetDlgManager()->m_pModalDlg == this);
	}
	void UIDialog::DoModal(char bModal)		//设置独占模式
	{CPUTime(UIDialog);
		if(bModal)
		{
			this->SetVisible(true);
			GetDlgManager()->m_pModalDlg	=	this;
			this->m_bTopMost = TRUE;
			this->m_nDlgOrder	=	GetDlgManager()->m_nDlgOrder++;
			GetDlgManager()->SetTopMost(0, 0);
		}
		else if(GetDlgManager()->m_pModalDlg == this)
			GetDlgManager()->m_pModalDlg	=	0;
	}
	void UIDialog::RemoveControlPtr( int index )
	{CPUTime(UIDialog);
		UIControl* pControl = m_controls.at( index );
		if( pControl )
		{
            for(int i=0;i<MAX_TOUCH_COUNT;i++)
                if( s_pControlFocus[i] == pControl )
                    s_pControlFocus[i] = NULL;
			if( s_pControlPressed == pControl )
				s_pControlPressed = NULL;
			if( m_pControlMouseOver == pControl )
				m_pControlMouseOver = NULL;
			if(pControl&&pControl->m_bIsCustomControl)
			{
				if(GetDlgManager()->m_callback)
					GetDlgManager()->m_callback->RemoveControl(pControl);
			}
			else
				safe_delete( pControl );
			if(pControl==m_centerObj)m_centerObj=0;
			m_controls.eraseByIndex(index);
		}
	}

	BOOL UIDialog::FindControl(UIControl* pC)
	{CPUTime(UIDialog);
		for( int i=0; i < (int)m_controls.size(); i++ )
		{
			UIControl* pControl = m_controls.at( i );
			if( pControl == pC )
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	void UIDialog::DeleteControlPtr(UIControl* c)
	{CPUTime(UIDialog);
		for( int i=0; i < (int)m_controls.size(); i++ )
		{
			UIControl* pControl = m_controls.at( i );
			if( pControl == c )
			{
                for(int ii=0;ii<MAX_TOUCH_COUNT;ii++)
                    if( s_pControlFocus[ii] == pControl )
                        s_pControlFocus[ii] = NULL;
				if( s_pControlPressed == pControl )
					s_pControlPressed = NULL;
				if( m_pControlMouseOver == pControl )
					m_pControlMouseOver = NULL;
				if(pControl&&pControl->m_bIsCustomControl)
				{
					if(GetDlgManager()->m_callback)
						GetDlgManager()->m_callback->RemoveControl(pControl);
				}
				else
					safe_delete( pControl );
				if(pControl==m_centerObj)m_centerObj=0;
				m_controls.eraseByIndex(i);
				return;
			}
		}
		this->m_renders.clear();
	}
	void UIDialog::RemoveControl( int ID )
	{CPUTime(UIDialog);
		for( int i=0; i < (int)m_controls.size(); i++ )
		{
			UIControl* pControl = m_controls.at( i );
			if( pControl->GetID() == ID )
			{
                for(int ii=0;ii<MAX_TOUCH_COUNT;ii++)
                    if( s_pControlFocus[ii] == pControl )
                        s_pControlFocus[ii] = NULL;
				if( s_pControlPressed == pControl )
					s_pControlPressed = NULL;
				if( m_pControlMouseOver == pControl )
					m_pControlMouseOver = NULL;
				if(pControl&&pControl->m_bIsCustomControl)
				{
					if(GetDlgManager()->m_callback)
						GetDlgManager()->m_callback->RemoveControl(pControl);
				}
				else
					safe_delete( pControl );
				if(pControl==m_centerObj)m_centerObj=0;
				m_controls.eraseByIndex(i);
				return;
			}
		}
		this->m_renders.clear();
	}
	void UIDialog::RemoveAllControls()
	{CPUTime(UIDialog);
        for(int ii=0;ii<MAX_TOUCH_COUNT;ii++)
            if( s_pControlFocus[ii] && s_pControlFocus[ii]->m_dialogPtr == this )
                s_pControlFocus[ii] = NULL;
		if( s_pControlPressed && s_pControlPressed->m_dialogPtr == this )
			s_pControlPressed = NULL;
		m_pControlMouseOver = NULL;
		for( int i=0; i < (int)m_controls.size(); i++ )
		{
			UIControl* pControl = m_controls.at( i );
			if(pControl&&pControl->m_bIsCustomControl)
			{
				if(GetDlgManager()->m_callback)
					GetDlgManager()->m_callback->RemoveControl(pControl);
			}
			else
				safe_delete( pControl );
		}
		m_controls.clear();
		this->m_renders.clear();
	}
	UIControl*		UIDialog::Pick(int x, int y)
	{CPUTime(UIDialog);
		Pixel pt;
		pt.x	=	x;
		pt.y	=	y;
		OnSetMouseMatrix(pt);
		for(int i=0;i< (int)m_controls.size();i++)
		{
			if(!m_controls[i]->m_bVisible || !m_controls[i]->IsEnabledCtrl())
				continue;
			if(!m_controls[i]->canHaveFocus())
				continue;
			if(m_controls[i]->m_windowRect.isPtIn(pt))
				return m_controls[i];
		}
		return 0;
	}
	int		UIDialog::IsAstarMoveEnable(int x, int y)	//是否可以移动到这里
	{CPUTime(UIDialog);
		if(x<0||y<0||x>=m_mapAstarGridCount.cx||y>=m_mapAstarGridCount.cy)
			return 0;
		Map2DAstarInfo& info = m_mapAstarGrids[x+m_mapAstarGridCount.cx * y];
		if(info.mx<0||info.my<0||info.mx>=m_mapGridCount.cx||info.my>=m_mapGridCount.cy)
			return 0;
		return (m_mapGrids[info.mx+info.my*m_mapGridCount.cx].flags==0);// & Map2DGridInfo::MapGridInfoFlags_Disable) == 0);
	}
	int				UIDialog::IsAstarMoveEnable2(int x, int y)	//是否可以移动到这里
	{
		if(m_mapAstarGrids.size()==0||x<0||y<0||x>=m_mapAstarGridCount.cx||y>=m_mapAstarGridCount.cy)
			return 0;
		Map2DAstarInfo& info = m_mapAstarGrids[x+m_mapAstarGridCount.cx * y];
		if(info.mx<0||info.my<0||info.mx>=m_mapGridCount.cx||info.my>=m_mapGridCount.cy)
			return 0;
		return ((m_mapGrids[info.mx+info.my*m_mapGridCount.cx].flags & Map2DGridInfo::MapGridInfoFlags_Disable2) == 0);
	}
	void			UIDialog::SetAstarMoveEnable2(int x, int y, BOOL b)
	{
		if(m_mapAstarGrids.size()==0||x<0||y<0||x>=m_mapAstarGridCount.cx||y>=m_mapAstarGridCount.cy)
			return;
		Map2DAstarInfo& info = m_mapAstarGrids[x+m_mapAstarGridCount.cx * y];
		if(info.mx<0||info.my<0||info.mx>=m_mapGridCount.cx||info.my>=m_mapGridCount.cy)
			return;
		int index = info.mx+info.my*m_mapGridCount.cx;
		if(!b)
			m_mapGrids[index].flags |= Map2DGridInfo::MapGridInfoFlags_Disable2;
		else if(m_mapGrids[index].flags & Map2DGridInfo::MapGridInfoFlags_Disable2)
			m_mapGrids[index].flags ^= Map2DGridInfo::MapGridInfoFlags_Disable2;
	}
	void			UIDialog::ClearAstarMoveEnable2()
	{
		int cx = m_mapGridCount.cx;
		int cy = m_mapGridCount.cy;
		for(int x=0;x<cx;x++)
		{
			for(int y=0;y<cy;y++)
			{
				int index = x + y * cx;
				if(m_mapGrids[index].flags & Map2DGridInfo::MapGridInfoFlags_Disable2)
					m_mapGrids[index].flags ^= Map2DGridInfo::MapGridInfoFlags_Disable2;
			}
		}
	}

	int		UIDialog::GetAstarWidth()					//地形宽度
	{CPUTime(UIDialog);
		if(m_mapAstarGrids.size()>0)
			return m_mapAstarGridCount.cx;
		return 0;
	}
	int		UIDialog::GetAstarHeight()				//地形高度
	{CPUTime(UIDialog);
		if(m_mapAstarGrids.size()>0)
			return m_mapAstarGridCount.cy;
		return 0;
	}

	int	SortControlsLayer(const void* v1, const void* v2)
	{
		UIControl* c1	=	*(UIControl**)v1;
		UIControl* c2	=	*(UIControl**)v2;
		int y1			=	c1->m_windowRect.top + c1->m_rotCenter.y;
		int y2			=	c2->m_windowRect.top + c2->m_rotCenter.y;
		int x1			=	c1->m_windowRect.left + c1->m_rotCenter.x;
		int x2			=	c2->m_windowRect.left + c2->m_rotCenter.x;
		if(y1 < y2)
			return -1;
		else if(y1 > y2)
			return 1;
		if(c1->m_nLevel < c2->m_nLevel)
			return -1;
		else if(c1->m_nLevel > c2->m_nLevel)
			return 1;
		if(x1 < x2)
			return -1;
		else if(x1 > x2)
			return 1;
		return 0;
	}

	struct	LineItem
	{
		UIGroup*	g;
		UIControl* c;
		LineItem(){g = 0; c = 0;}
		LineItem(UIGroup* _g, UIControl* _c)
		{
			g	=	_g;
			c	=	_c;
		}
	};

	int	SortGroupLayer(const void* v1, const void* v2)
	{
		LineItem& c1	=	*(LineItem*)v1;
		LineItem& c2	=	*(LineItem*)v2;
		int y1,y2,x1,x2;
		if(c1.c)
		{
			y1			=	c1.c->m_windowRect.top + c1.c->m_rotCenter.y;
			x1			=	c1.c->m_windowRect.left + c1.c->m_rotCenter.x;
		}
		else
		{
			y1			=	c1.g->m_groupPosition.y;
			x1			=	c1.g->m_groupPosition.x;
		}
		if(c2.c)
		{
			y2			=	c2.c->m_windowRect.top + c2.c->m_rotCenter.y;
			x2			=	c2.c->m_windowRect.left + c2.c->m_rotCenter.x;
		}
		else
		{
			y2			=	c2.g->m_groupPosition.y;
			x2			=	c2.g->m_groupPosition.x;
		}
		if(y1 < y2)
			return -1;
		else if(y1 > y2)
			return 1;
		if(x1 < x2)
			return -1;
		else if(x1 > x2)
			return 1;
		return 0;
	}

	struct	LineObjects
	{
		CDynamicArray<LineItem>	objs;//1000*256*6*4
	};
	extern void	AddUVScaleIndex();
	VOID	UIDialog::UpdateDialogSize(UIControl* c){
		if(m_nLastAlignMode!=c->m_nAlignMode){
			g_manager.SetUIDialogSize(c->m_nAlignMode==0, m_screenSize.cx, m_screenSize.cy, 0);//SetUIScaleMode(c->m_nAlignMode==0);
			m_nLastAlignMode=c->m_nAlignMode;
			AddUVScaleIndex();
		}
	}

	VOID	UIDialog::RenderControl(UIControl* c, float fElapsedTime)
	{CPUTime(UIDialog);
		UpdateDialogSize(c);
		//g_manager.SetUIDialogSize(m_screenSize.cx, m_screenSize.cy);//SetUIScaleMode(c->m_nAlignMode==0);
		//if(!this->m_bEnableDlgMatrix)
		//{
		//	if(!c->IsClipIn())
		//		return;
		//}
		if(c->m_bind3DEnable&&!c->m_bind3DVisible)
			return;
		c->Render(fElapsedTime);
		if(c->m_pChildDialog)
		{
			c->SetClipRect(true);
			c->m_pChildDialog->OnRender(g_manager.GetElapsedTime(), 0);
			c->SetClipRect(false);
			c->RenderScroll();
		}
		//g_manager.SetUIScaleMode(true);
		if(c->m_renderCB)
			c->m_renderCB->OnRenderControl(c);
        for(int ii=0;ii<MAX_TOUCH_COUNT;ii++)
		if(c == s_pControlFocus[ii] && c->IsDrawFocusFrame())
		{
			Rect rc	=	c->m_windowRect;
			rc.inflate(-2, -2);
			this->DrawRect2D((Rect*)&rc, 0x8f5f5f5f, false);
		}
	}

	//VOID	UIDialog::SetWorldMatrix(const Phantom::matrix4x4& mat)
	//{
	//	m_bEnableDlgMatrix	=	true;
	//	m_dlgMatrix	=	mat;
	//	matrixInverse(&m_dlgMatrixInv, NULL, &m_dlgMatrix);
	//}

	//VOID	UIDialog::SetWorldMatrixByCenter(const Phantom::matrix4x4& mat)
	//{
	//	Phantom::matrix4x4 matT;
	//	Phantom::float2 screen	=	g_manager.ConvertPixelToTixel(this->m_dlgCenter);
	//	matT.identity();
	//	matT.SetPosition(-screen.x, -screen.y, 0);
	//	matrixMultiply(&matT, &matT, &mat);
	//	matT._41	+=	screen.x;
	//	matT._42	+=	screen.y;
	//	SetWorldMatrix(matT);
	//}
	VOID	UIDialog::UpdateMatrix()
	{CPUTime(UIDialog);
		int rotx = m_dlgRotation.x*100000.0f;
		int roty = m_dlgRotation.y*100000.0f;
		int rotz = m_dlgRotation.z*100000.0f;
		BOOL bRot = !(rotx==0&&roty==0&&rotz==0);
		int scalex = this->m_dlgScale.x*100000.0f;
		int scaley = this->m_dlgScale.y*100000.0f;
		BOOL bScale = !(scalex==100000&&scaley==100000);
		m_bEnableDlgMatrix = (bRot||bScale);
		if(!m_bEnableDlgMatrix)
			return;
		if(!this->m_bUpdate)return;
		m_bUpdate = false;
		Phantom::float2 screen	=	g_manager.ConvertPixelToTixel(m_dlgCenter);
		Phantom::matrix4x4 mat;
		mat.identity();
		mat.SetPosition(-screen.x, -screen.y, 0);
		Phantom::matrix4x4 matT;
		if(bRot){
			::matrixRotationYawPitchRoll(&matT, m_dlgRotation.x, m_dlgRotation.y, m_dlgRotation.z);
			matrixMultiply(&mat, &mat, &matT);
		}
		if(bScale){
			matrixScaling(&matT, m_dlgScale.x, m_dlgScale.y, 1);
			matrixMultiply(&mat, &mat, &matT);
		}
		mat._41	+=	screen.x;
		mat._42	+=	screen.y;
		m_dlgMatrix	=	mat;
		matrixInverse(&m_dlgMatrixInv, NULL, &m_dlgMatrix);
	}
	VOID	UIDialog::SetWorldRotationX(float fX, const Pixel& center)
	{CPUTime(UIDialog);
		this->m_dlgRotation.x = fX;
		m_bUpdate = true;
	}
	VOID	UIDialog::SetWorldRotationY(float fY, const Pixel& center)
	{CPUTime(UIDialog);
		this->m_dlgRotation.y = fY;
		m_bUpdate = true;
	}
	VOID	UIDialog::SetWorldRotationZ(float fZ, const Pixel& center)
	{CPUTime(UIDialog);
		this->m_dlgRotation.z = fZ;
		m_bUpdate = true;
	}
	VOID	UIDialog::SetWorldScaling(const Phantom::float2& s, const Pixel& center)
	{CPUTime(UIDialog);
		this->m_dlgScale = s;
		this->m_bUpdate = true;
		////m_dlgCenter	=	center;
		//Pixel cent(m_dlgCenter.x*g_manager.GetBufferWidth()/this->m_screenSize.cx, m_dlgCenter.y*g_manager.GetBufferHeight()/m_screenSize.cy);
		////dlggame.center = Pixel(screenwidth()/2,screenheight()/2);
		//
		//Phantom::matrix4x4 mat, matT;
		//matrixScaling(&mat, s.x, s.y, s.z);
		//Phantom::float2 screen	=	g_manager.ConvertPixelToTixel(Pixel(0, 0));
		//matT.identity();
		//matT.SetPosition(-screen.x, -screen.y, 0);
		//matrixMultiply(&mat, &matT, &mat);
		////
		//float fov	=	tanf(g_manager.Get2DProjectionFov() / 2.0f) * 2.0f;
		//float fZ	=	0;
		//float fY	=	fov;
		//float fX	=	fY * (float)g_manager.GetBufferWidth() / (float)g_manager.GetBufferHeight();
		//float fXx	=	fX/2.0f;
		//float fYy	=	fY/2.0f;
		//mat._41		-=	fXx * s.x - (cent.x - g_manager.GetBufferWidth()/2) / (float)g_manager.GetBufferWidth()*fX;
		//mat._42		+=	fYy * s.y - (cent.y - g_manager.GetBufferHeight()/2)/ (float)g_manager.GetBufferHeight()*fY;
		//SetWorldMatrix(mat);
	}
	VOID	UIDialog::BeginRender()
	{CPUTime(UIDialog);
		g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		g_manager->SetBlendMode(BlendMode_SRCALPHA,BlendMode_INVSRCALPHA);
		g_manager->SetCullVisible(CullMode_CW);
		g_manager.SetWorldDlgMatrix2D(0);
		g_manager.SetUIDialogSize(true, g_manager.GetBufferWidth(), g_manager.GetBufferHeight(), 0);
		g_manager->SetEnableZ(false);
		UpdateMatrix();
	}
	Result UIDialog::OnRender( float fElapsedTime, BOOL bBackground )
	{CPUTime(UIDialog);
		if(!m_bVisible)
		{
			return Result_OK;
		}
		m_nLastAlignMode = -1;
		g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		g_manager->SetEnableZ(false);
		Rect rcClip(-m_x, -m_y, -m_x + g_manager.GetUIBufferWidth(), -m_y + g_manager.GetUIBufferHeight());
		if(!m_bIsPause)
			fElapsedTime = g_manager.GetRealElapsedTime();
		if(bBackground)
		{
			for(int ctrl = m_controls.size()-1; ctrl >= 0; ctrl--)
			{
				UIControl* c	=	m_controls[ctrl];
				if(!c->m_bVisible || !c->m_bIsBackground)
					continue;
				UIGroup*	g	=	static_cast<UIGroup*>(this->FindGroupPtr(c->m_groupID));
				if(!g->m_bVisible)
					continue;
				c->FrameMove( fElapsedTime, 0, false );
				RenderControl(c, fElapsedTime);
			}
			return Result_OK;
		}
#ifndef EXPORT_ENGINE
		//if(m_box2World.IsEnable())
			//m_box2World.OnFrameMove(fElapsedTime);
#endif
		UpdateMatrix();
		if(this->m_bEnableDlgMatrix)
		{
			g_manager.SetWorldDlgMatrix2D(&m_dlgMatrix);
			g_manager->SetCullMode(CullMode_CW);//CullMode_Double);
		}
		else
		{
			g_manager->SetCullVisible(CullMode_CW);
			g_manager.SetWorldDlgMatrix2D(0);
		}
		char bFindTrans =false;
		//测试鼠标移开之后的表现
		int beginY	=	0;//
		if(m_dlgY < 0)
			beginY	=	(-m_dlgY) / this->m_mapGridSize.cy;
		OnRender2(fElapsedTime, false);
		if(this->m_bMapEnable){
			if(this->m_bMapGridVisible){
				this->OnRenderMap();
			}
		}
		//int nGroupCount	=	this->m_groups.size();
		//for(int group=0;group<nGroupCount;group++)
		//{
		//	UIGroup*	g	=	this->m_groups[group];
		//	//
		//	if(!g->m_bVisible)
		//		continue;
		//	if(g->m_bIsUnit && g->m_nGroupID > 0)
		//	{
		//		g->m_fPlayTime	+=	g->m_fPlaySpeed * fElapsedTime;
		//	}
		//}
		//if(m_bRequestResort||m_renders.size()==0)
		//{
		//	m_bRequestResort = false;
		//	this->ResortControls();
		//}
		////for(int ctrl = (m_controls.size()-1); ctrl >= 0 ; ctrl--)
		//for(int ctrl = (m_renders.size()-1); ctrl >= 0 ; ctrl--)
		//{
		//	RenderData &d	=	m_renders[ctrl];
		//	if(d.d){
		//		d.d->m_x = m_x;
		//		d.d->m_y = m_y;
		//		d.d->m_dlgX = m_dlgX;
		//		d.d->m_dlgY = m_dlgY;
		//		d.d->m_lastDlgX = m_lastDlgX;
		//		d.d->m_lastDlgY = m_lastDlgY;
		//		d.d->OnRender2(fElapsedTime);
		//		//d.d->OnRender(fElapsedTime);
		//		continue;
		//	}
		//	UIControl* c	=	d.c;//m_controls[ctrl];
		//	if(!c->m_bVisible || c->m_bIsBackground)
		//		continue;
		//	UIGroup*	g	=	static_cast<UIGroup*>(this->FindGroupPtr(c->m_groupID));
		//	if(!g->m_bVisible)
		//		continue;
		//	c->FrameMove( fElapsedTime, 0, bChanged );
		//	RenderControl(c, fElapsedTime);
		//}
		g_manager->SetBlendMode(BlendMode_SRCALPHA,BlendMode_INVSRCALPHA);
		if(m_renderCB)
			m_renderCB->OnRenderDialog(this);
#ifndef EXPORT_ENGINE
		//if(m_box2World.IsEnable())
			//m_box2World.Render();
#endif
		if(this->m_bEnableDlgMatrix)
		{
			g_manager->SetCullVisible(CullMode_CW);
			g_manager.SetWorldDlgMatrix2D(0);
		}
		m_nLastAlignMode = -1;
		return Result_OK;
	}
	VOID	UIDialog::OnRender2( float fElapsedTime, BOOL bChanged2 )
	{
		m_dlgX	=	m_x+m_pos.x;
		m_dlgY	=	m_y+m_pos.y;
		BOOL bChanged = false;
		if(m_dlgX!=m_lastDlgX || m_dlgY != m_lastDlgY)
		{
			bChanged = true;
			m_lastDlgX = m_dlgX;
			m_lastDlgY = m_dlgY;
		}
		int nGroupCount	=	this->m_groups.size();
		for(int group=0;group<nGroupCount;group++)
		{
			UIGroup*	g	=	this->m_groups[group];
			//
			if(!g->m_bVisible)
				continue;
			if(g->m_bIsUnit && g->m_nGroupID > 0)
			{
				g->m_fPlayTime	+=	g->m_fPlaySpeed * fElapsedTime;
			}
		}
		if(m_bRequestResort||m_renders.size()==0)
		{
			m_bRequestResort = false;
			this->ResortControls();
		}
		MoveToFrame(fElapsedTime);
		//for(int ctrl = (m_controls.size()-1); ctrl >= 0 ; ctrl--)
		for(int ctrl = (m_renders.size()-1); ctrl >= 0 ; ctrl--)
		{
			RenderData &d	=	m_renders[ctrl];
			if(d.d){
				d.d->m_x = m_x;
				d.d->m_y = m_y;
				d.d->m_dlgX = m_dlgX;
				d.d->m_dlgY = m_dlgY;
				d.d->m_lastDlgX = m_lastDlgX;
				d.d->m_lastDlgY = m_lastDlgY;
				d.d->OnRender2(fElapsedTime, bChanged);
				//d.d->OnRender(fElapsedTime);
				continue;
			}
			UIControl* c	=	d.c;//m_controls[ctrl];
			if(!c->m_bVisible || c->m_bIsBackground)
				continue;
			UIGroup*	g	=	static_cast<UIGroup*>(this->FindGroupPtr(c->m_groupID));
			if(!g->m_bVisible)
				continue;
			c->FrameMove( fElapsedTime, 0, bChanged );
			RenderControl(c, fElapsedTime);
		}
	}
	UIControl* UIDialog::PickControl( Pixel pt )
	{CPUTime(UIDialog);
		this->OnSetMouseMatrix(pt);
		for( int i=0; i < (int)m_controls.size(); i++ )
		{
			UIControl* pControl = m_controls[i];
			if( pControl == NULL || !pControl->m_bIsEnabled || !pControl->m_bVisible || pControl->m_bIsBackground)
				continue;
			Pixel clientPos;
			clientPos	=	pControl->ScreenToClient(pt);
			if(pControl->isPtIn( clientPos ))
				return pControl;
		}
		return NULL;
	}
	char UIDialog::GetControlEnabled( int ID )
	{CPUTime(UIDialog);
		UIControl* pControl = GetControl( ID );
		if( pControl == NULL )
			return false;
		return pControl->GetEnabled();
	}
	void UIDialog::SetControlEnabled( int ID, char bEnabled )
	{CPUTime(UIDialog);
		UIControl* pControl = GetControl( ID );
		if( pControl == NULL )
			return;
		pControl->SetEnabled( bEnabled );
	}
	void UIDialog::OnMouseUp( Pixel pt )
	{CPUTime(UIDialog);
		s_pControlPressed = NULL;
		m_pControlMouseOver = NULL;
	}
	UIScrollBar*	UIDialog::AddScrollBar(BOOL bInit,  int x, int y, int width, int height)
	{CPUTime(UIDialog);
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		UIScrollBar* pStatic = new UIScrollBar(bInit, this );
		if( pStatic == NULL )
			return 0;
		hr = OnAddControl( pStatic );
		if( Failed(hr) )
			return 0;
		pStatic->SetID( ID ); 
		pStatic->SetRenderRect(Rect(x, y, x + width, y + height));
		return pStatic;
	}
	UIStatic*	UIDialog::AddStatic(BOOL bInit,  const char* strText, int x, int y, int width, int height, char bIsDefault )
	{CPUTime(UIDialog);
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		UIStatic* pStatic = new UIStatic(bInit, this );
		if( pStatic == NULL )
			return 0;
		hr = OnAddControl( pStatic );
		if( Failed(hr) )
			return 0;
		pStatic->SetID( ID ); 
//		pStatic->SetText( strText );
		pStatic->SetRenderRect(Rect(x, y, x + width, y + height));
		pStatic->m_bIsDefault	=	bIsDefault;
		pStatic->m_bCanFocus	=	false;
		return pStatic;
	}

	UIProgress*	UIDialog::AddProgress( BOOL bInit, int x, int y, int width, int height, char bIsDefault )
	{CPUTime(UIDialog);
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		UIProgress* pStatic = new UIProgress( bInit, this );
		if( pStatic == NULL )
			return 0;
		hr = OnAddControl( pStatic );
		if( Failed(hr) )
			return 0;
		pStatic->SetID( ID ); 
		pStatic->SetRenderRect(Rect(x, y, x + width, y + height));
		pStatic->m_bIsDefault	=	bIsDefault;
		pStatic->m_bCanFocus	=	false;
		return pStatic;
	}
	UIEffect*	UIDialog::AddEffect( BOOL bInit, int x, int y, int width, int height, char bIsDefault )
	{CPUTime(UIDialog);
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		UIEffect* pStatic = new UIEffect( bInit, this );
		if( pStatic == NULL )
			return 0;
		hr = OnAddControl( pStatic );
		if( Failed(hr) )
			return 0;
		pStatic->SetID( ID ); 
		pStatic->SetRenderRect(Rect(x, y, x + width, y + height));
		pStatic->m_bIsDefault	=	bIsDefault;
		pStatic->m_bCanFocus	=	false;
		return pStatic;
	}
	UIControl*	UIDialog::AddCustomControl(int type, const Rect& rect)//添加自定义控件
	{CPUTime(UIDialog);
		if(!GetDlgManager()->m_callback)
			return 0;
		UIControl* c = GetDlgManager()->m_callback->NewControl(type, this);
		if(!c)
			return 0;
		c->m_bIsCustomControl	=	true;
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		hr = OnAddControl( c );
		if( Failed(hr) )
			return 0;
		c->SetID( ID ); 
		c->SetRenderRect(rect);
		return c;
	}
	UIButton*	UIDialog::AddButton( BOOL bInit, const char* strText, int x, int y, int width, int height, char bIsDefault)
	{CPUTime(UIDialog);
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		UIButton* pButton = new UIButton( bInit, this );
		if( pButton == NULL )
			return 0;
		hr = OnAddControl( pButton );
		if( Failed(hr) )
			return 0;
		pButton->SetID( ID ); 
		pButton->SetName( strText );
		//pButton->SetText( strText );
		pButton->SetRenderRect(Rect(x, y, x + width, y + height));
		pButton->m_bIsDefault = bIsDefault;
		return pButton;
	}
	UICheckBox*	UIDialog::AddCheckBox( BOOL bInit, const char* strText, int x, int y, int width, int height, char bChecked, char bIsDefault )
	{CPUTime(UIDialog);
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		UICheckBox* pCheckBox = new UICheckBox( bInit, this );
		if( pCheckBox == NULL )
			return 0;
		hr = OnAddControl( pCheckBox );
		if( Failed(hr) )
			return 0;
		pCheckBox->SetID( ID ); 
		//pCheckBox->SetText( strText );
		pCheckBox->SetRenderRect(Rect(x, y, x + width, y + height));
		pCheckBox->m_bIsDefault = bIsDefault;
		pCheckBox->SetChecked( bChecked );
		return pCheckBox;
	}

	UITextBox*	UIDialog::AddEditBox( BOOL bInit, const char* strText, int x, int y, int width, int height )
	{CPUTime(UIDialog);
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		UITextBox* pTextBox = new UITextBox(bInit, this );
		if( pTextBox == NULL )
			return 0;
		hr = OnAddControl( pTextBox );
		if( Failed(hr) )
			return 0;
		pTextBox->SetID( ID ); 
		pTextBox->SetRenderRect(Rect(x, y, x + width, y + height));
		return pTextBox;
	}
	UIComboBox*	UIDialog::AddComboBox( BOOL bInit, int x, int y, int width, int height, char bIsDefault )
	{CPUTime(UIDialog);
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		UIComboBox* pComboBox = new UIComboBox( bInit, this );
		if( pComboBox == NULL )
			return 0;
		hr = OnAddControl( pComboBox );
		if( Failed(hr) )
			return 0;
		pComboBox->SetID( ID ); 
		pComboBox->SetRenderRect(Rect(x, y, x + width, y + height));
		pComboBox->m_bIsDefault = bIsDefault;
		return pComboBox;
	}
	UISlider*	UIDialog::AddSlider( BOOL bInit, int x, int y, int width, int height, int _min, int _max, int value, char bIsDefault )
	{CPUTime(UIDialog);
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		UISlider* pSlider = new UISlider( bInit, this );
		if( pSlider == NULL )
			return 0;
		hr = OnAddControl( pSlider );
		if( Failed(hr) )
			return 0;
		pSlider->SetID( ID ); 
		pSlider->SetRenderRect(Rect(x, y, x + width, y + height));
		pSlider->m_bIsDefault = bIsDefault;
		pSlider->SetRange( _min, _max );
		pSlider->SetValue( value );
		pSlider->recalcRects();
		return pSlider;
	}
	UIStaticFormat*	UIDialog::AddFormatStatic( BOOL bInit, const char* strText, int x, int y, int width, int height, char bIsDefault )
	{CPUTime(UIDialog);
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		UIStaticFormat *pEditBox = new UIStaticFormat( bInit, this );
		if( pEditBox == NULL )
			return 0;
		hr = OnAddControl( pEditBox );
		if( Failed(hr) )
			return 0;
		pEditBox->SetID( ID ); 
		pEditBox->SetRenderRect(Rect(x, y, x + width, y + height));
		pEditBox->m_bIsDefault = bIsDefault;
		//if( strText )
			//pEditBox->SetText( strText );
		return pEditBox;
	}
	UIListBox*	UIDialog::AddListBox( BOOL bInit, int x, int y, int width, int height, BOOL bMultiselect )
	{CPUTime(UIDialog);
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		UIListBox *pListBox = new UIListBox( bInit, this );
		if( pListBox == NULL )
			return 0;
		hr = OnAddControl( pListBox );
		if( Failed(hr) )
			return 0;
		pListBox->SetID( ID );
		pListBox->SetRenderRect(Rect(x, y, x + width, y + height));
		pListBox->SetMultiSelect( bMultiselect );
		return pListBox;
	}

	UIControl*	UIDialog::FindControlByID(INT nID)	//根据ID查找控件
	{CPUTime(UIDialog);
		for(int i=0;i<this->m_controls.size();i++)
		{
			if(m_controls[i]->m_controlID == nID)
				return m_controls[i];
		}
		return 0;
	}
	VOID		UIDialog::OnChangeAlignMode()
	{CPUTime(UIDialog);
		//int cx = g_manager.GetUIDialogSizeW()/2;
		//int cy = g_manager.GetUIDialogSizeH()/2;
		//Phantom::float2 uvs(1,1);// = g_manager.GetUIScale();
		//float fMinS = getmin(uvs.x, uvs.y);
		//for(int i=0;i<this->m_controls.size();i++)
		//{
		//	if(this->m_controls[i]->m_nAlignMode==1)
		//	{
		//		Rect rc = m_controls[i]->GetRenderRect();
		//		Pixel c((rc.left+rc.right)/2, (rc.top+rc.bottom)/2);
		//		Phantom::float2 v(c.x-cx, c.y-cy);
		//		v *= uvs;
		//		Rect newRc((float)(rc.left-c.x)*fMinS+v.x+cx, (float)(rc.top-c.y)*fMinS+v.y+cy, (float)(rc.right-c.x)*fMinS+v.x+cx, (float)(rc.bottom-c.y)*fMinS+v.y+cy);
		//		m_controls[i]->SetRenderRect(newRc);
		//	}
		//}
	}
	UIListCtrl*	UIDialog::AddListCtrl( BOOL bInit, int x, int y, int width, int height)							//添加滚动框
	{CPUTime(UIDialog);
		int ID	=	m_nControlID++;
		Result hr = Result_OK;
		UIListCtrl *pListBox = new UIListCtrl( bInit, this );
		if( pListBox == NULL )
			return 0;
		hr = OnAddControl( pListBox );
		if( Failed(hr) )
			return 0;
		pListBox->SetID( ID );
		pListBox->SetRenderRect(Rect(x, y, x + width, y + height));
		return pListBox;
	}

	UIControl*		UIDialog::CreateFromType(BOOL bInit, int type)
	{CPUTime(UIDialog);
		int x = 0, y  = 0, w = 32, h = 32;
		if(GetCallbackPtr())
		{
			UIControl * c	=	GetCallbackPtr()->OnCreateUIControl(type);
			if(c)
			{
				c->SetID(m_nControlID++);
				OnAddControl( c );
				return c;
			}
		}
		switch(type)
		{
		case UIControlType_BUTTON:
			return AddButton(bInit, "button", x, y, w, h);
		case UIControlType_PROGRESS:
			return AddProgress(bInit, x, y, w, h);
		case UIControlType_STATIC:
			return AddStatic(bInit, "static", x, y, w, h);
		case UIControlType_ui2DEffect:
			return AddEffect(bInit, x, y, w, h);
		case UIControlType_CHECKBOX:
			return AddCheckBox(bInit, "checkbox", x, y, w, h);
		case UIControlType_EDITBOX:
		case UIControlType_IMEEDITBOX:
			return AddEditBox(bInit, "", x, y, w, h);
		case UIControlType_COMBOBOX:
			return AddComboBox(bInit, x, y, w, h);
		case UIControlType_SLIDER:
			return AddSlider(bInit, x, y, w, h);
		case UIControlType_LISTBOX:
			return AddListBox(bInit, x, y, w, h);
		case UIControlType_SCROLLBAR:
			return AddScrollBar(bInit, x,y,w,h);
		case UIControlType_VIEW_FORMAT_TEXT:
			return AddFormatStatic(bInit, "format static", x, y, w, h);
		case UIControlType_LISTCTRL3D:
			return AddListCtrl(bInit, x, y, w, h);
		}
		return 0;
	}

	int				UIDialog::GetControlCount()
	{CPUTime(UIDialog);
		return m_controls.size();
	}

	UIControl*		UIDialog::GetControlPtr(int index)
	{CPUTime(UIDialog);
		if(index < 0 || index >= m_controls.size())
			return 0;
		return m_controls.at(index);
	}

	UIControl*		UIDialog::GetControlPtrFromID(int id)
	{CPUTime(UIDialog);
		return GetControl(id);
	}

	int		SortControls(const void* p1, const void* p2)
	{
		UIControl* c1 = *(UIControl**)p1;
		UIControl* c2 = *(UIControl**)p2;
		int l1 = c1->m_nLevel;
		int l2 = c2->m_nLevel;
		if(c1->m_bIsGridObject)
			l1 = 20;
		if(c2->m_bIsGridObject)
			l2 = 20;
		if(l1 > l2)
			return -1;
		else if(l1 < l2)
			return 1;
		if(c1->m_bIsGridObject&&c2->m_bIsGridObject){
			Pixel p1 = c1->GetPos();//c1->m_dialogPtr->GetMapGridIndex(c1->GetPos());
			Pixel p2 = c2->GetPos();//c2->m_dialogPtr->GetMapGridIndex(c2->GetPos());
			if(p1.y<p2.y)
				return 1;
			else if(p1.y>p2.y)
				return -1;
			return 0;
		}
		//if(c1->GetType() > c2->GetType())
		//{
		//	return 1;
		//}
		//else if(c2->GetType() < c2->GetType())
		//{
		//	return -1;
		//}
		//int r = stricmp(c2->GetName(), c1->GetName());
		//if(r > 0)
		//	return -1;
		//else if(r < 0)
		//	return 1;
		return 0;
	}
	VOID		UIDialog::Add2DChild(UIDialog* dlg)
	{
		for(int i=0;i<m_childs.size();i++){
			if(m_childs[i]==dlg)return;
		}
		dlg->m_bIsChildRender = true;
		dlg->AddRef();
		dlg->m_dlgParent = this;
		m_childs.push_back(dlg);
		this->m_bRequestResort=true;
	}
	VOID		UIDialog::ClearChilds()
	{
		for(int i=0;i<m_childs.size();i++){
			m_childs[i]->m_dlgParent = 0;
			m_childs[i]->Release();
		}
		this->m_renders.clear();
		m_childs.clear();
		m_bRequestResort=true;
	}
	VOID		UIDialog::BeginAutoMove(float fSpeed, LPCSTR szLuaFunc)
	{
		m_waySpeed = getmax(fSpeed, 0.1f);
		m_luaFunc = szLuaFunc;
		m_wayMode=true;
		Pixel p = GetPosition();
		m_wayFrom.setxy(p.x, p.y);
		//if(m_wayPoints.size()>0){
		//	m_wayFrom = m_wayPoints[0];
		//	m_wayPoints.eraseByIndex(0);
		//}
		ToNextMove();
	}
	BOOL		UIDialog::ToNextMove()
	{
		if(m_wayPoints.size()==0)
		{
			if(m_wayMode&&m_luaFunc.t[0]){
				lua_UIDialog_s mgr;
				SetIPtr(&mgr.m_data, this, UIDialog, IDialogPtr);
				lua_tinker::call<void>(g_luamanager.GetLuaState(), m_luaFunc.t, mgr, true);
			}
			m_wayMode=false;
			return false;
		}
		Pixel p = GetPosition();
		float2 pos = m_wayPoints[0];
		m_wayPoints.eraseByIndex(0);
		m_wayDir.setxy(pos.x-p.x, pos.y-p.y);
		m_wayFrom.setxy(p.x, p.y);
		m_wayLength = m_wayDir.length();
		m_wayDir.normalize();
		m_wayCurr = 0;
		float fDegress = Degress(getRotateZ(float3(-m_wayDir.x, -m_wayDir.y, 0)));
		for(int i=0;i<GetControlCount();i++){
			GetControlPtr(i)->SetUnitAngle(fDegress);
		}
		return true;
	}
	VOID		UIDialog::MoveToFrame(float fElapsedTime)
	{
		if(!m_wayMode)
			return;
		m_wayCurr +=	fElapsedTime*m_waySpeed;
		float2 pos;
		if(m_wayCurr>=m_wayLength){
			m_wayCurr=m_wayLength;
			pos = m_wayFrom + m_wayDir * m_wayCurr;
			ToNextMove();
		}
		else{
			pos = m_wayFrom + m_wayDir * m_wayCurr;
		}
		this->SetPosition(Pixel(pos.x, pos.y));
		if(m_dlgParent)
			m_dlgParent->m_bRequestResort=true;
		if(m_wayMode&&m_luaFunc.t[0]){
			lua_UIDialog_s mgr;
			SetIPtr(&mgr.m_data, this, UIDialog, IDialogPtr);
			lua_tinker::call<void>(g_luamanager.GetLuaState(), m_luaFunc.t, mgr, false);
		}
	}
	VOID		UIDialog::AddMoveTo(WayPoints* fw)
	{
		for(int i=1;i<fw->m_points.size();i++){
			AddMoveTo(Pixel(fw->m_points[i].pos.x, fw->m_points[i].pos.y));
		}
	}
	VOID		UIDialog::AddMoveTo(const Pixel& fw)
	{
		m_wayPoints.push_back(float2(fw.x, fw.y));
	}
	VOID		UIDialog::StopAutoMove()
	{
		m_wayPoints.clear();
		m_wayMode = 0;
	}
	Pixel		UIDialog::GetPosition()
	{
		if(this->m_centerObj)
			return this->m_centerObj->GetLoadPos() + m_pos;
		return m_pos;
	}
	VOID		UIDialog::SetPosition(const Pixel& pos)
	{
		if(m_centerObj){
			m_pos = pos - m_centerObj->GetLoadPos();
		}
		else{
			m_pos = pos;
		}
		//for(int i=0;i<m_controls.size();i++)
		//{
		//	UIControl* c = m_controls[i];
		//	Pixel lp = c->GetLoadPos() + off;
		//	c->SetPos(lp);
		//}
	}
	int		SortRenderData(const void* pp1, const void* pp2)
	{
		UIDialog::RenderData* c1 = (UIDialog::RenderData*)pp1;
		UIDialog::RenderData* c2 = (UIDialog::RenderData*)pp2;
		int l1 = 20;
		int l2 = 20;
		if(c1->c&&!c1->c->m_bIsGridObject)
			l1 = c1->c->m_nLevel;
		if(c2->c&&!c2->c->m_bIsGridObject)
			l2 = c2->c->m_nLevel;
		if(l1 > l2)
			return -1;
		else if(l1 < l2)
			return 1;
		BOOL bG1 = (c1->c?c1->c->m_bIsGridObject:true);
		BOOL bG2 = (c2->c?c2->c->m_bIsGridObject:true);
		if(bG1&&bG2){
			Pixel p1,p2;
			if(c1->c)
				p1 = c1->c->GetPos();
			else
				p1 = c1->d->GetPosition();
			if(c2->c)
				p2 = c2->c->GetPos();
			else
				p2 = c2->d->GetPosition();
			//
			if(p1.y<p2.y)
				return 1;
			else if(p1.y>p2.y)
				return -1;
			return 0;
		}
		return 0;
	}
	void	UIDialog::ResortControls()
	{CPUTime(UIDialog);
		int count = m_controls.size()+m_childs.size();
		if(count>0){
			m_renders.resize(count);
			memset(m_renders.GetPtr(), 0, sizeof(RenderData)*count);
			int index = 0;
			for(int i=0;i<m_controls.size();i++){
				m_renders[index++].c = m_controls[i];
			}
			for(int i=0;i<m_childs.size();i++){
				m_renders[index++].d = m_childs[i];
			}
			if(m_renders.size()>1)
				qsort(m_renders.GetPtr(), m_renders.size(), sizeof(RenderData), SortRenderData);
		}
		else if(m_renders.size()>0)
		{
			m_renders.clear();
		}
		//if(m_controls.size() > 0)
		//{
		//	qsort(m_controls.GetPtr(), m_controls.size(), sizeof(UIControl*), SortControls);
		//}
	}
	Result UIDialog::OnAddControl( UIControl* pControl )
	{CPUTime(UIDialog);
		m_controls.push_back( pControl );
		ResortControls();
		//if(m_controls.size() > 0)
		//{
		//	qsort(m_controls.GetPtr(), m_controls.size(), sizeof(UIControl*), SortControls);
		//}
		return Result_OK;
	}

	UIControl*	UIDialog::GetControl(Phantom::PtrID& id)
	{CPUTime(UIDialog);
		if(id.id == 0)
			return 0;
		if(id.index < m_controls.size() && m_controls[id.index]->m_controlID == id.id)
			return m_controls[id.index];
		for( int i=0; i < (int)m_controls.size(); i++ )
		{
			UIControl* pControl = m_controls[i];
			if( pControl->GetID() == id.id )
			{
				id.index	=	i;
				id.id = pControl->GetID();
				return pControl;
			}
		}
		id.id	=	0;
		return 0;
	}

	UIControl* UIDialog::GetControl( int ID )
	{CPUTime(UIDialog);
		for( int i=0; i < (int)m_controls.size(); i++ )
		{
			UIControl* pControl = m_controls[i];
			if( pControl->GetID() == ID )
			{
				return pControl;
			}
		}
		return NULL;
	}
	UIControl* UIDialog::GetControl( int ID, unsigned int nControlType )
	{CPUTime(UIDialog);
		for( int i=0; i < (int)m_controls.size(); i++ )
		{
			UIControl* pControl = m_controls[i];
			if( pControl->GetID() == ID && pControl->GetType() == nControlType )
			{
				return pControl;
			}
		}
		return NULL;
	}

	void UIDialog::RequestFocus( UIControl* pControl, unsigned int touchIndex )
	{CPUTime(UIDialog);
		if( s_pControlFocus[touchIndex] == pControl )
			return;
		if(pControl)
			if(pControl->m_pChildDialog == 0 && !pControl->canHaveFocus() )
				return;
		if( s_pControlFocus[touchIndex] != pControl)
		{
			if(s_pControlFocus[touchIndex])
				s_pControlFocus[touchIndex]->onFocusOut();
			if(pControl)
				pControl->onFocusIn();
		}
		s_pControlFocus[touchIndex] = pControl;
	}

	Result UIDialog::CalcTextRect( const short_t* strText, UIElement* pElement, Rect* prcDest, int nCount )
	{CPUTime(UIDialog);
		Result hr = Result_OK;
		FontNode2D* pFontNode = GetDlgManager()->GetFontNode( pElement->fontIndex );
		if( pFontNode == NULL || pFontNode->fontPtr.t == 0)
			return Result_Fail;
		DWORD dwTextFormat = pElement->dwTextFormat | DrawTextAlign_CALCRECT;
		pFontNode->fontPtr.t->DrawTexts( strText, *prcDest, nCount, dwTextFormat );
		return Result_OK;
	}
	void	UIDialog::SetLeft(int left)
	{CPUTime(UIDialog);
		m_x = left;
		if(this->m_bFullScreen)
			SetSize(this->m_nFullscreenWidth, this->m_nFullscreenHeight);
	}
	void	UIDialog::SetTop(int top)
	{CPUTime(UIDialog);
		m_y = top;
		if(this->m_bFullScreen)
			SetSize(this->m_nFullscreenWidth, this->m_nFullscreenHeight);
	}
	void	UIDialog::GetLocation( int& x, int& y ) const
	{CPUTime(UIDialog);
		x = m_x;
		y = m_y;
		if(m_pParentControl)
		{
			x -= m_pParentControl->m_windowRect.left;
			y -= m_pParentControl->m_windowRect.top;
		}
	}
	void	UIDialog::SetLocation( int x, int y )
	{CPUTime(UIDialog);
		m_x = x;
		m_y = y;
		if(m_pParentControl)
		{
			m_x += m_pParentControl->m_windowRect.left;
			m_y += m_pParentControl->m_windowRect.top;
			m_pParentControl->RecalcScrollValue();
		}
		if(this->m_bFullScreen)
			SetSize(this->m_nFullscreenWidth, this->m_nFullscreenHeight);
	}
	int		UIDialog::GetWidth() {CPUTime(UIDialog); return m_width; }
	int		UIDialog::GetHeight() {CPUTime(UIDialog); return m_height; }

	void UIDialog::SetBackgroundColors( unsigned int colorTopLeft, unsigned int colorTopRight, unsigned int colorBottomLeft, unsigned int colorBottomRight )
	{CPUTime(UIDialog);
		m_colorTopLeft = colorTopLeft;
		m_colorTopRight = colorTopRight;
		m_colorBottomLeft = colorBottomLeft;
		m_colorBottomRight = colorBottomRight;
	}
	char	UIDialog::CalcTextRect( const short_t* strText, Rect* prcDest, int nFontID, int nCount, unsigned int textFormat)
	{CPUTime(UIDialog);
		FontNode2D* pFontNode = GetDlgManager()->GetFontNode( nFontID );
		if( pFontNode == NULL || pFontNode->fontPtr.t == 0 )
			return false;
		DWORD dwTextFormat = textFormat | DrawTextAlign_CALCRECT;
		pFontNode->fontPtr.t->DrawTexts( strText, *prcDest, nCount, dwTextFormat );
		return true;
	}
	BOOL	IsCodeSection(char c)
	{
		static char s_CodeSymbol[] = {' ','\t','+','-','*','/','\"','\\','*','%','^','#','=','~','!','<','>','(',')','{','}','[',']',';',':',',','.','\r','\n','\'', '\"', '\r', '\n', 0};
		int len	=	sizeof(s_CodeSymbol) / sizeof(char);
		for(int i=0;i<len;i++)
			if(c == s_CodeSymbol[i])
				return true;
		return false;
	}
	struct	ControlVarAry{
		int		id;
		short	aryindex;
	};
	struct	ControlVar{
		Name	name;
		int		id;
		short	isary;//-1
		CDynamicArray<ControlVarAry> arys;
	};
	struct	GroupVar{
		CDynamicArray<ControlVar*>	vars;
		Name	name;
		int		id;
		~GroupVar(){
			for(int i=0;i<vars.size();i++)
				delete vars[i];
		}
	};
	char UIDialog::LoadDialog(const char* szFile, ImportResult* result)
	{CPUTime(UIDialog);
		LogInfo("system->dlg_load(%s)", szFile);
		m_strFile	=	szFile;
		m_strFile.setIsFile();
		//
		int version = 0;
		{
			CSafeFileHelperR r;
			if(!r.open( m_strFile, true))
			{
				LogInfo("system->error:dlg_file not found.%s", szFile);
				return false;
			}
			if(PakDirectory::IsPakFile(r.pStream))//如果是压包类型则读取包内容
			{
				r.close();
				safe_release(m_pakdir);
				m_pakdir = new PakDirectory();
				CSafeFileHelperR temp;
				if(!m_pakdir->OpenPak(m_strFile, temp))
				{
					if(result)
						*result = Phantom::ImportResult_FailureAuth;
					LogInfo("system->error: package password error.(%s)", m_strFile.t);
					return false;
				}
				if(!this->LoadDialog(temp, result, &version))
				{
					LogInfo("system->error: dlg package data error.(%s)", m_strFile.t);
					return false;
				}
				m_pakdir->ClosePak();
			}
			else{
				if(!LoadDialog(r, result, &version))
				{
					LogInfo("system->error:dlg load error.%s", szFile);
					return false;
				}
			}
		}
		NameT<128> file;
		m_strFile.splitpath(0, &file, 0);
		strcpy(m_szName, file);
		RebuildRenderRect();
		OnChangeAlignMode();
		OnLoadLua();
		for(int i=0;i<m_controls.size();i++){
			if(m_controls[i]->m_bParentMove)
			{
				m_centerObj = m_controls[i];
				break;
			}
		}
		//FileName ftemp = m_strFile;
		//ftemp += ".lua";
		//CDynamicArray<char> str;
		//if(LoadUtf8(ftemp, str)){
		//	
		//}
		return true;
	}
	void	UIDialog::OnLoadLua()
	{
#ifndef EDITOR_OS
		NameT<128> file;
		m_strFile.splitpath(0, &file, 0);
		CDynamicArray<GroupVar*> groups;
		for(int i=0;i<m_controls.size();i++){
			if(strnicmp(m_controls[i]->GetName(), "var_", 4)==0){
				Name temp;
				temp = m_controls[i]->GetName() + 4;
				BOOL bIsArray = false;
				int arrayIndex = -1;
				const char* ab = strstr(temp.t, "[");
				if(ab){
					arrayIndex = atoi(ab+1);
					temp.t[ab-temp.t] = 0;
					bIsArray = true;
				}
				int len = temp.size();
				for(int k=0;k<len;k++){
					if(IsCodeSection(temp[k])){
						temp.t[k] = '_';
					}
				}
				GroupVar* gvar = 0;
				for(int g=0;g<groups.size();g++){
					if(groups[g]->id==m_controls[i]->m_groupID.id){
						gvar = groups[g];
						break;
					}
				}
				if(!gvar){
					gvar = new GroupVar();
					gvar->id = m_controls[i]->m_groupID.id;
					Name gtemp;
					gtemp = m_controls[i]->GetGroupPtr()->GetName();
					int len = gtemp.size();
					for(int k=0;k<len;k++){
						if(IsCodeSection(gtemp[k])){
							gtemp.t[k] = '_';
						}
					}
					gvar->name = gtemp;
					groups.push_back(gvar);
				}
				bool bFind = false;
				for(int k=0;k<gvar->vars.size();k++){
					if(gvar->vars[k]->name==temp.t){
						bFind = true;
						if(bIsArray&&arrayIndex>=0){
							ControlVarAry ary;
							ary.aryindex = arrayIndex;
							ary.id = m_controls[i]->GetID();
							gvar->vars[k]->arys.push_back(ary);
						}
						break;
					}
				}
				if(!bFind){
					ControlVar* cv = new ControlVar();
					cv->name =temp;
					cv->id = m_controls[i]->GetID();
					cv->isary=bIsArray;
					if(bIsArray&&arrayIndex>=0)
					{
						ControlVarAry ary;
						ary.aryindex = arrayIndex;
						ary.id = m_controls[i]->GetID();
						cv->arys.push_back(ary);
					}
					gvar->vars.push_back(cv);
				}
			}
		}
		char buf[256];
		std::string stemp;
		sprintf(buf, "function init_%s_vars(dlg)\n",file.t);
		stemp = buf;
		GroupVar* gvar0 = 0;
		for(int i=0;i<groups.size();i++){
			for(int k=0;k<groups[i]->vars.size();k++){
				if(groups[i]->vars[k]->isary){
					sprintf(buf, "local la_%s={}\n", groups[i]->vars[k]->name.t);
					stemp += buf;
					for(int m=0;m<groups[i]->vars[k]->arys.size();m++){
						sprintf(buf, "la_%s[%d]=dlg:searchbyid(%d);\n", groups[i]->vars[k]->name.t, groups[i]->vars[k]->arys[m].aryindex, groups[i]->vars[k]->arys[m].id);
						stemp += buf;
					}
				}
			}
			if(groups[i]->id==0){
				gvar0 = groups[i];
				continue;
			}
			sprintf(buf, "local l_%s={", groups[i]->name.t);
			stemp += buf;
			for(int k=0;k<groups[i]->vars.size();k++){
				if(groups[i]->vars[k]->isary){
					sprintf(buf, "%s=la_%s,", groups[i]->vars[k]->name.t, groups[i]->vars[k]->name.t);
				}
				else{
					sprintf(buf, "%s=dlg:searchbyid(%d),", groups[i]->vars[k]->name.t, groups[i]->vars[k]->id);
				}
				stemp += buf;
			}
			sprintf(buf, "self=dlg:gsearchbyid(%d)};\n", groups[i]->id);
			stemp += buf;
		}
		stemp += "\treturn {";
		if(gvar0){
			for(int i=0;i<gvar0->vars.size();i++){
				if(gvar0->vars[i]->isary){
					sprintf(buf, "%s=la_%s,", gvar0->vars[i]->name.t, gvar0->vars[i]->name.t);
				}
				else
					sprintf(buf, "%s=dlg:searchbyid(%d),", gvar0->vars[i]->name.t, gvar0->vars[i]->id);
				stemp += buf;
			}
		}
		for(int i=0;i<groups.size();i++){
			if(groups[i]->id==0){
				gvar0 = groups[i];
				continue;
			}
			sprintf(buf, "%s=l_%s,", groups[i]->name.t, groups[i]->name.t);
			stemp += buf;
		}
		sprintf(buf, "self=dlg};\nend\n");
		stemp += buf;
		g_luamanager.loadAndExecute(stemp.c_str(), m_strFile);
		for(int i=0;i<groups.size();i++)
			delete groups[i];
#endif
	}
#define CURRENT_UI_VERSION	0x0000088
	char UIDialog::LoadDialog(CSafeFileHelperR& r, ImportResult* result, INT* pVersion)
	{CPUTime(UIDialog);
		int head = _MakeFileHead('z','d','l','g');
		int headRead;
		r >> headRead;
		int version;
		r >> version;
		if(version<0x0000082){
			VersionWriter reads;
			reads.ReadAll(r.pStream);
		}
		if(pVersion)
			*pVersion	=	version;
		if(version < 0x00000075){
			if(result)*result = Phantom::ImportResult_ReadFailure;
			return false;
		}
		if(version>CURRENT_UI_VERSION)
		{
			if(result)
				*result = Phantom::ImportResult_VersionLow;
			return false;
		}
		int controlCount;
		r >> m_nControlID;
		r >> m_bIsBackground;
		for(int i=0;i<m_groups.size();i++)
			delete m_groups[i];
		m_groups.clear();
		int nGroup	=	0;
		this->m_defaultGroup.Load(r);
		r >> nGroup;
		for(int i=0;i<nGroup;i++)
		{
			UIGroup* g		=	new UIGroup();
			g->m_dialogPtr	=	this;
			g->Load(r);
			m_groups.push_back(g);
		}
		if(version >= 0x00000073&&version <= 0x00000080)
		{
			unsigned char auth[2048];
			int authLen = 0;
			BOOL bAuth = false;
			r >> bAuth >> authLen;
			r.pStream->read(auth, authLen);
			//if(!bAuth)
			//	return 0;
		}
		r >> m_nDefaultControlID >> m_x >> m_y >> m_width >> m_height;
		m_x=m_y=0;
		r >> m_colorTopLeft >> m_colorTopRight >> m_colorBottomLeft >> m_colorBottomRight >> controlCount;
		r.pStream->read( m_szName, sizeof(m_szName) );
		r >> m_bIsToolTipWindow;
		if(version <= 0x00000080){
			BOOL bEnableDlgAnimation;
			r >> bEnableDlgAnimation;
		}
		r >> m_nElementID;
		//
		r >> m_mapGridSize >> m_mapGridCount >> m_bMapEnable;
		int nSize = 0;
		r >> nSize;
		if(nSize > 0)
		{
			if(version>=0x00000084){
				m_mapGrids.resize(nSize);
				r.pStream->read(m_mapGrids.GetPtr(), m_mapGrids.size() * sizeof(Map2DGridInfo));
			}
			else{
				CDynamicArray<MapGridInfo> temp;
				temp.resize(nSize);
				r.pStream->read(temp.GetPtr(), temp.size() * sizeof(MapGridInfo));
			}
		}
		if(version>=0x00000084){
			r >> m_mapAstarGridCount;
			if(version<0x00000086||version>0x00000086){
				int nSize2 = 0;
				r >> nSize2;
				if(nSize2>0)
				{
					m_mapAstarGrids.resize(nSize2);
					r.pStream->read(m_mapAstarGrids.GetPtr(), m_mapAstarGrids.size() * sizeof(Map2DAstarInfo));
				}
			}
		}
		if(nSize>0)
		{
			RebuildAstar();
		}
		r >> m_nDialogID;
		INT nControlID = this->m_nControlID;
		for(int i=0;i<controlCount;i++)
		{
			int nType;
			int tell = 0;
			if(version >= 0x00000080)
				r >> tell;
			r >> nType;
			if(nType <= 0)
				nType	=	UIControlType_STATIC;
			UIControl* c	=	static_cast<UIControl*>(this->CreateFromType(false, nType));
			if(c)
			{
				c->LoadControl( r );
				c->recalcRects();
				nControlID	=	getmax(nControlID, c->m_controlID);
			}
			else if(version >= 0x00000080)
				r.pStream->seek(tell, SEEK_SET);
		}
		m_nControlID	=	(nControlID + 1);
		r >> m_showDlgSound;
		r >> m_screenSize;
		if(version < 0x0000081){
			char nL, nT;
			int nOffsetX, nOffsetY;
			r >> nL >> nT >> nOffsetX >> nOffsetY;
		}
		if(version>=0x00000083)
			r >> m_bMap25DMode;
		if(version>=0x00000088)
			r >> m_mapGridBegin;
		//if(nL == 2)
		//	this->m_nDlgAlign |= UIDialogAlign_LEFT;
		//else if(nL == 3)
		//	this->m_nDlgAlign |= UIDialogAlign_CENTER;
		//else if(nL == 4)
		//	this->m_nDlgAlign |= UIDialogAlign_RIGHT;

		//if(nT == 5)
		//	this->m_nDlgAlign |= UIDialogAlign_TOP;
		//else if(nT == 3)
		//	this->m_nDlgAlign |= UIDialogAlign_VCENTER;
		//else if(nT == 6)
		//	this->m_nDlgAlign |= UIDialogAlign_BOTTOM;
		//this->m_dlgAlignOffset.Set(nOffsetX, nOffsetY);
		for(int i=0;i<this->m_controls.size();i++)
		{
			int id = m_controls[i]->m_groupID.id;
			m_controls[i]->m_groupID.id = 0;
			m_controls[i]->SetGroupID(id);
		}
		std::vector<UIGroup*> groups	=	m_groups;
		m_groups.clear();
		for(int i=0;i<groups.size();i++)
		{
			if(groups[i]->m_controls.size() == 0)
				delete groups[i];
			else
				m_groups.push_back(groups[i]);
		}
		for(int i=0;i<m_controls.size();i++)
		{
			for(int j=0;j<m_controls.size();j++)
			{
				if(i == j)
					continue;
				if(m_controls[i]->m_controlID == m_controls[j]->m_controlID)
				{
					assert(false);
					m_controls[i]->m_controlID	=	(m_nControlID++);
					break;
				}
			}
		}
		//RebuildRenderRect();
		this->ResortControls();
		if(result)
			*result	=	ImportResult_OK;
#ifndef EXPORT_ENGINE
		if(version >= 0x00000075&&version<0x00000085)
		{
			int bversion = 1;
			int bm_id;
			BOOL bm_bEnabled;
			float2	bm_viewScale;	//((box2d.x / m_viewScale.x) + 0.5f) * screenWidth, height...
			int	bm_velocityIterations;	//
			int	bm_positionIterations;	//
			r >> bversion >> bm_viewScale >> bm_velocityIterations >> bm_positionIterations >> bm_bEnabled;	//
			r >> bm_id;
			int bbodys;
			int bjoints;
			r >> bbodys >> bjoints;
			//m_box2World.Load(r);
		}
#endif
		RecalcRect();
		m_dlgCenter.Set(this->m_width / 2, this->m_height / 2);
		for(int i=0;i<m_groups.size();i++)
		{
			m_groups[i]->RecalcRect();
			//if(m_groups[i]->m_controls.size() > 0)
			//{
			//	UIControl* c	=	GetControl(m_groups[i]->m_controls[0]);
			//	if(c)
			//	{
			//		Rect rc = c->GetRenderRect();
			//		m_groups[i]->m_movePos.Set(rc.left, rc.top);
			//	}
			//}
		}
		return true;
	}
	extern	BOOL	IsExistFile(CDynamicArray<PakFileData*>& files, const char* szFile);
	extern	BOOL	GetAuthCode(GenuineGUID* r);
	char	UIDialog::ExportPak(const char* szFile)
	{
		if(this->m_pakdir)return false;
		//GenuineGUID temp;
		//if(!GetAuthCode(&temp))return false;
		//int passlen = sizeof(Phantom::GenuineGUID);
		//unsigned char* pass = (unsigned char*)&temp;
		CDynamicArray<PakFileData*> files;
		PakFileData* data = LoadFileData(this->m_strFile.t, true);
		if(!data)
			return false;
		files.push_back(data);
		//
		//
		LFileName fileName, szTexturePath;
#ifdef WIN32_OS
		char buf[256];
		memset(buf, 0, 256);
		GetCurrentDirectory(256, buf);
		LFileName fn = buf;
		fn.setIsFile();
		if(strnicmp(fn.t, m_strFile.t, strlen(fn.t))==0){
			fn = m_strFile.t+strlen(fn.t)+1;
		}
		fn.splitpath(&szTexturePath, &fileName, 0);
#endif
		for(int i=0;i<this->m_controls.size();i++){
			UIControl* control = m_controls[i];
			for(int e=0;e<control->GetElementCount();e++){
				UIElement* element = control->GetElementPtr(e);
				if(!IsExistFile(files, element->GetTexture())){
					data = LoadTexturePak(element->GetTexture(), szTexturePath);
					if(data){
						data->bIsImage = true;
						files.push_back(data);
					}
				}
				for(int a=0;a<element->m_uvAnimations.size();a++){
					UVAnimation* ani = element->m_uvAnimations[a];
					if(!IsExistFile(files, ani->GetTexture())){
						data = LoadTexturePak(ani->GetTexture(), szTexturePath);
						if(data){
							data->bIsImage = true;
							files.push_back(data);
						}
					}
				}
			}
		}
		char ret = PakDirectory::ExportPak(szFile, files);
		for(int i=0;i<files.size();i++)
			delete files[i];
		return ret;
	}

	char				UIDialog::SaveDialog(CSafeFileHelperW& w)
	{CPUTime(UIDialog);
		int head = _MakeFileHead('z','d','l', 1);
		w << head;
		int version = CURRENT_UI_VERSION;
		w << version;
		//
		w << m_nControlID;
		w << m_bIsBackground;
		int nGroups	=	m_groups.size();
		m_defaultGroup.Save(w);
		w << nGroups;
		for(int i=0;i<nGroups;i++)
			this->m_groups[i]->Save(w);
		//
		int nCount	=	m_controls.size();
		w << m_nDefaultControlID << m_x << m_y << m_width << m_height << m_colorTopLeft << m_colorTopRight << m_colorBottomLeft << m_colorBottomRight << nCount;
		w.pStream->write( m_szName, sizeof(m_szName) );
		w << m_bIsToolTipWindow;
		w << m_nElementID;
		//
		w << m_mapGridSize << m_mapGridCount << m_bMapEnable;
		int nSize	=	this->m_mapGrids.size();
		w << nSize;
		if(nSize > 0)
			w.pStream->write(m_mapGrids.GetPtr(), m_mapGrids.size() * sizeof(Map2DGridInfo));
		w << m_mapAstarGridCount;
		int nSize2 = m_mapAstarGrids.size();
		w << nSize2;
		if(nSize2>0)
			w.pStream->write(m_mapAstarGrids.GetPtr(), m_mapAstarGrids.size() * sizeof(Map2DAstarInfo));
		//
		w << m_nDialogID;
		for(int i=0;i< nCount;i++)
		{
			int tell	=	w.pStream->tell();
			int nType	=	m_controls[i]->GetType();
			w << tell;
			w << nType;
			m_controls[i]->SaveControl( w );
			int newS	=	w.pStream->tell();
			w.pStream->seek(tell, SEEK_SET);
			w << tell;
			w.pStream->seek(newS, SEEK_SET);
		}
		w << m_showDlgSound.c_str();
		w << m_screenSize;
		w << m_bMap25DMode;
		w << m_mapGridBegin;
#ifndef EXPORT_ENGINE
		//m_box2World.Save(w);
#endif
#ifdef EDITOR_OS
		//CDynamicArray<Name>	vars;
		//for(int i=0;i<m_controls.size();i++){
		//	if(strnicmp(m_controls[i]->GetName(), "var_", 4)==0){
		//		Name temp;
		//		temp = m_controls[i]->GetName() + 4;
		//		int len = temp.size();
		//		for(int k=0;k<len;k++){
		//			if(IsCodeSection(temp[k])){
		//				temp.t[k] = '_';
		//			}
		//		}
		//		vars.push_back(temp);
		//	}
		//}
		//FileName ftemp = m_strFile;
		//ftemp += ".lua";
		//FileName fname;
		//ftemp.splitpath(0, &fname, 0);
		//char buf[256];
		//std::string stemp;
		//sprintf(buf, "function init_%s(dlg)\n{\n\treturn {", fname.t);
		//stemp = buf;
		//for(int i=0;i<vars.size();i++){
		//	sprintf(buf, "%s=dlg:search(\"var_%s\"),", vars[i].t, vars[i].t);
		//	stemp += buf;
		//}
		//sprintf(buf, "self=dlg};\n}\n");
		//stemp += buf;
		//SaveUtf8(ftemp, stemp.c_str());
#endif
		return true;
	}
	char				UIDialog::SaveDialog(const char* szFile)
	{CPUTime(UIDialog);
		if(this->m_pakdir)
			return 0;
		LogInfo("system->dlg_save(%s)", szFile);
		m_strFile	=	szFile;
		m_strFile.setIsFile();
		//
		int version = 0;
		{
			CSafeFileHelperW w;
			if(!w.create( m_strFile ))
			{
				LogInfo("system->error:dlg_file not found.%s", szFile);
				return false;
			}
			if(!SaveDialog(w))
			{
				LogInfo("system->error:dlg save error.%s", szFile);
				return false;
			}
		}
		return true;
	}
	VOID				UIDialog::SetSize(int dlgWidth, int dlgHeight)
	{CPUTime(UIDialog);
		m_bFullScreen	=	(this->m_width!=dlgWidth || this->m_height != dlgHeight);
		if(m_bFullScreen)
		{
			m_nFullscreenWidth	=	dlgWidth;
			m_nFullscreenHeight	=	dlgHeight;
			float fW	=	(float)g_manager.GetBufferWidth() / (float)m_nFullscreenWidth;
			float fH	=	(float)g_manager.GetBufferHeight() / (float)m_nFullscreenHeight;
			SetWorldScaling(Phantom::float2(fW, fH), Pixel(m_x, m_y));
		}
		else
		{
			SetWorldScaling(Phantom::float2(1, 1), Pixel(m_x, m_y));
			m_nFullscreenWidth	=	m_width;
			m_nFullscreenHeight	=	m_height;
		}
	}
	VOID				UIDialog::SetFullscreen(BOOL b, int dlgWidth, int dlgHeight, int align)
	{CPUTime(UIDialog);
		m_bFullScreen	=	b;
		if(m_bFullScreen)
		{
			m_nFullscreenWidth	=	dlgWidth;
			m_nFullscreenHeight	=	dlgHeight;
			this->m_dlgX	=	m_dlgY	=	0;
			//
			float fW	=	(float)g_manager.GetBufferWidth() / (float)m_nFullscreenWidth;
			float fH	=	(float)g_manager.GetBufferHeight() / (float)m_nFullscreenHeight;
			float fScale	=	getmin(fW, fH);
			float nWidth	=	(float)m_nFullscreenWidth * fScale;
			float nHeight	=	(float)m_nFullscreenHeight * fScale;
			Pixel offset(0, 0);
			if(align & DrawTextAlign_RIGHT)
				offset.x	=	g_manager.GetUIBufferWidth() - nWidth;
			else if(align & DrawTextAlign_CENTER)
				offset.x	=	(g_manager.GetUIBufferWidth() - nWidth) / 2;
			if(align & DrawTextAlign_BOTTOM)
				offset.y	=	g_manager.GetUIBufferHeight() - nHeight;
			else if(align & DrawTextAlign_VCENTER)
				offset.y	=	(g_manager.GetUIBufferHeight() - nHeight) / 2;
			SetWorldScaling(Phantom::float2(fScale, fScale), Pixel(offset.x, offset.y));
		}
		else
		{
			SetWorldScaling(Phantom::float2(1, 1), Pixel(0,0));
			m_nFullscreenWidth	=	m_width;
			m_nFullscreenHeight	=	m_height;
		}
	}
	Pixel	UIDialog::MapToAstarMap(int x, int y)//从地图坐标转换成寻路坐标
	{
		if(x<0||y<0||x>=m_mapGridCount.cx||y>=m_mapGridCount.cy)
			return Pixel(-1,-1);
		Map2DGridInfo& info = m_mapGrids[x+y*m_mapGridCount.cx];
		return Pixel(info.rx, info.ry);
	}
	Pixel	UIDialog::AstarMapToMap(int ax, int ay)//从寻路坐标转换成地图坐标
	{
		if(ax<0||ay<0||ax>=m_mapAstarGridCount.cx||ay>=m_mapAstarGridCount.cy||m_mapAstarGrids.size()==0)
			return Pixel(-1,-1);
		Map2DAstarInfo& info = m_mapAstarGrids[ax+ay*m_mapAstarGridCount.cx];
		return Pixel(info.mx, info.my);
	}
	VOID			UIDialog::SetMapGridCount(const Size& s)
	{CPUTime(UIDialog);
		Size oldSize	=	m_mapGridCount;
		int cnt = s.cx*s.cy;
		m_mapGridCount	=	Size(getmax(s.cx, 1), getmax(s.cy, 1));
		if(oldSize.cx != m_mapGridCount.cx || oldSize.cy != m_mapGridCount.cy || m_mapGrids.size() == 0||m_mapGrids.size() != cnt)
		{
			CDynamicArray<Map2DGridInfo,int,0> olds	=	m_mapGrids;
			int cx = m_mapGridCount.cx;
			int cy = m_mapGridCount.cy;
			m_mapGrids.resize(cx * cy);
			for(int x=0;x<cx;x++)
			{
				for(int y=0;y<cy;y++)
				{
					if(x < oldSize.cx && y < oldSize.cy && olds.size() > 0)
					{
						m_mapGrids[x + y * cx] = olds[x + y * oldSize.cx];
					}
					else
						m_mapGrids[x + y * cx] = Map2DGridInfo();
				}
			}
			if(this->m_bMap25DMode){
				int s = sqrtf(m_mapGridCount.cx*m_mapGridCount.cx*2+m_mapGridCount.cy*m_mapGridCount.cy*2)+1;
				m_mapAstarGridCount.cx = m_mapAstarGridCount.cy = s;
				int sx = m_mapGridSize.cx;
				int sy = m_mapGridSize.cy/2;
				m_mapAstarGrids.resize(m_mapAstarGridCount.cx*m_mapAstarGridCount.cy);
				for(int x=0;x<cx;x++){
					for(int y=0;y<cy;y++){
						m_mapGrids[x+y*cx].rx = 0;
						m_mapGrids[x+y*cx].ry = 0;
					}
				}
				for(int x=0;x<m_mapAstarGridCount.cx;x++)
				{
					int yy = (-x+m_mapGridCount.cy)*sy;
					int xx = (x - m_mapGridCount.cy)*(sx/2);
					for(int y=0;y<m_mapAstarGridCount.cy;y++){
						int by = yy + y*sy;
						int bx = xx + y*(sx/2);
						Rect rc(bx,by,bx+sx,by+m_mapGridSize.cy);
						Pixel vm = this->GetMapGridIndex(Pixel((rc.left+rc.right)/2, (rc.top+rc.bottom)/2));
						int mx = vm.x;
						int my = vm.y;
						this->m_mapAstarGrids[x+y*m_mapAstarGridCount.cx].mx = mx;
						this->m_mapAstarGrids[x+y*m_mapAstarGridCount.cx].my = my;
						if(mx>=0&&my>=0&&mx<m_mapGridCount.cx&&my<m_mapGridCount.cy){
							Map2DGridInfo& info = m_mapGrids[mx+my*m_mapGridCount.cx];
							info.rx=x;
							info.ry=y;
						}
					}
				}
			}
			else{
				m_mapAstarGridCount = m_mapGridCount;
				m_mapAstarGrids.resize(m_mapAstarGridCount.cx*m_mapAstarGridCount.cy);
				for(int x=0;x<cx;x++){
					for(int y=0;y<cy;y++){
						m_mapAstarGrids[x+y*cx].mx = x;
						m_mapAstarGrids[x+y*cx].my = y;
						m_mapGrids[x+y*cx].rx = x;
						m_mapGrids[x+y*cx].ry = y;
					}
				}
			}
		}
		RebuildAstar();
	}
	VOID	UIDialog::RebuildAstar()
	{
		//if(!this->m_bMapEnable||m_mapGridCount.cx==0||m_mapGridCount.cy==0)
		//	return;
		//if(this->m_bMap25DMode){
		//	m_mapAstarGrids.resize(m_mapAstarGridCount.cx*m_mapAstarGridCount.cy);
		//	for(int x=0;x<m_mapAstarGridCount.cx;x++){
		//		for(int y=0;y<m_mapAstarGridCount.cy;y++){
		//			Map2DAstarInfo& info = m_mapAstarGrids[x+y*m_mapAstarGridCount.cx];
		//			info.mx=-1;
		//			info.my=-1;
		//		}
		//	}
		//	for(int x=0;x<m_mapGridCount.cx;x++){
		//		for(int y=0;y<m_mapGridCount.cy;y++){
		//			Map2DGridInfo& gi = this->m_mapGrids[x+y*m_mapGridCount.cx];
		//			if(gi.rx>=0&&gi.ry>=0&&gi.rx<m_mapAstarGridCount.cx&&gi.ry<m_mapAstarGridCount.cy)
		//			{
		//				Map2DAstarInfo& info = m_mapAstarGrids[gi.rx+gi.ry*m_mapAstarGridCount.cx];
		//				info.mx=x;
		//				info.my=y;
		//			}
		//		}
		//	}
		//}
		//else{
		//	m_mapAstarGridCount = m_mapGridCount;
		//	m_mapAstarGrids.resize(m_mapAstarGridCount.cx*m_mapAstarGridCount.cy);
		//	for(int x=0;x<m_mapGridCount.cx;x++){
		//		for(int y=0;y<m_mapGridCount.cy;y++){
		//			m_mapAstarGrids[x+y*m_mapGridCount.cx].mx = x;
		//			m_mapAstarGrids[x+y*m_mapGridCount.cx].my = y;
		//			m_mapGrids[x+y*m_mapGridCount.cx].rx = x;
		//			m_mapGrids[x+y*m_mapGridCount.cx].ry = y;
		//		}
		//	}
		//}
	}
	VOID	UIDialog::OnRenderMap()//渲染地形图片
	{
		if(this->m_bMap25DMode){
			int cx = m_mapGridCount.cx;
			int cy = m_mapGridCount.cy;
			int sx = m_mapGridSize.cx;
			int sy = m_mapGridSize.cy/2;
			BOOL bRText = false;
#ifdef WIN32_OS
			if(HIBYTE(GetKeyState(VK_RBUTTON)))
				bRText = true;
#endif
			for(int y=0;y<cy;y++){
				int beginX = 0;
				if((y%2)==1)
					beginX += sx/2;
				int by = y * sy;
				for(int x=0;x<cx;x++)
				{
					int bx = x*sx + beginX;
					Rect rc(bx,by,bx+sx,by+m_mapGridSize.cy);
					rc.Offset(m_mapGridBegin.x, m_mapGridBegin.y);
					int index = y * cx+x;
					unsigned color = 0x6fffffff;
					Map2DGridInfo& info = m_mapGrids[index];
					switch(m_mapGrids[index].flags)
					{
					case Map2DGridInfo::MapGridInfoFlags_Disable:
						color = 0x6fff0000;
						break;
					}
					rc.inflate(-1,-1);
					this->DrawRect2D(&rc, color, true, true);
					char buf[128];
					sprintf(buf, "%d,%d", info.rx, info.ry);
					int lx,ly;
					GetLocation(lx,ly);
					rc.Offset(lx, ly);
					if(bRText)
						g_manager.DrawTexts(buf, rc, 0, 16, 0, 0, Phantom::DrawTextAlign_CENTER|Phantom::DrawTextAlign_VCENTER);
			//		//this->DrawRect2D(&rc, 0xff000000, false, true);
			//		//Pixel center = GetMapGridCenter(x, y);
			//		//this->DrawRect2D(&Rect(center.x-2, center.y-2,center.x+2,center.y+2), 0xff000000, true, 0);
				}
			}
				//for(int x=0;x<m_mapAstarGridCount.cx;x++)
				//{
				//	int yy = (-x+m_mapGridCount.cy)*sy;
				//	int xx = (x - m_mapGridCount.cy)*(sx/2);
				//	for(int y=0;y<m_mapAstarGridCount.cy;y++){
				//		int by = yy + y*sy;
				//		int bx = xx + y*(sx/2);
				//		unsigned color = 0xff000000;
				//		Rect rc(bx,by,bx+sx,by+m_mapGridSize.cy);
				//		this->DrawRect2D(&rc, color, false, true);
				//	}
				//}
		}
		else{
			for(int y=0;y<m_mapGridCount.cy;y++){
				for(int x=0;x<m_mapGridCount.cx;x++)
				{
					Rect rc(x*m_mapGridSize.cx, y*m_mapGridSize.cy, (x+1)*m_mapGridSize.cx, (y+1)*m_mapGridSize.cy);
					rc.Offset(m_mapGridBegin.x, m_mapGridBegin.y);
					int index = y * m_mapGridCount.cx+x;
					unsigned color = 0xafffffff;
					switch(m_mapGrids[index].flags)
					{
					case Map2DGridInfo::MapGridInfoFlags_Disable:
						color = 0xafff0000;
						break;
					}
					this->DrawRect2D(&rc, color, true, false);
					this->DrawRect2D(&rc, 0xff000000, false, false);
				}
			}
		}
	}
	const Map2DGridInfo*	UIDialog::GetMapGridInfo(INT x, INT y)
	{CPUTime(UIDialog);
		int cx = m_mapGridCount.cx;
		int cy = m_mapGridCount.cy;
		if(x < 0 || x >= cx)
			return 0;
		if(y < 0 || y >= cy)
			return 0;
		if(m_mapGrids.size() == 0)
			return 0;
		return &m_mapGrids[x + y * cx];
	}
	Pixel			UIDialog::GetMapGridIndex(const Pixel& pixel2)//返回像素对应的格子索引，对话框位置开始的像素
	{
		Pixel pixel(pixel2.x-m_mapGridBegin.x, pixel2.y-m_mapGridBegin.y);
		if(!m_bMap25DMode){
			return Pixel(pixel.x/this->m_mapGridSize.cx, pixel.y/this->m_mapGridSize.cy);
		}
		int xx = pixel.x / m_mapGridSize.cx;
		int y1 = pixel.y / m_mapGridSize.cy;
		int ox = pixel.x - xx*m_mapGridSize.cx-(m_mapGridSize.cx/2);
		int oy = pixel.y - y1*m_mapGridSize.cy-(m_mapGridSize.cy/2);
		int yy = y1 * 2;
		float mx = (float)(m_mapGridSize.cx/2);
		float my = (float)(m_mapGridSize.cy/2);
		float fx = mx - mx/my * fabsf(oy);
		if(fabsf(ox)>=fx){
			if(ox<0)
			{
				if(oy<0)
					return Pixel(xx-1,yy-1);
				else
					return Pixel(xx-1,yy+1);
			}
			else{
				if(oy<0)
					return Pixel(xx,yy-1);
				else
					return Pixel(xx,yy+1);
			}
		}
		return Pixel(xx,yy);
	}
	Pixel			UIDialog::GetMapGridCenter(int x, int y)//返回格子索引对应的中心点像素坐标
	{
		if(!m_bMap25DMode){
			return Pixel(x*m_mapGridSize.cx+m_mapGridSize.cx/2+m_mapGridBegin.x, y*this->m_mapGridSize.cy+this->m_mapGridSize.cy/2+m_mapGridBegin.y);
		}
		int sx = m_mapGridSize.cx;
		int sy = m_mapGridSize.cy/2;
		int beginX = 0;
		if((y%2)==1)
			beginX += sx/2;
		int by = y * sy;
		int bx = x*sx + beginX;
		Rect rc(bx,by,bx+sx,by+m_mapGridSize.cy);
		return Pixel((rc.left+rc.right)/2+m_mapGridBegin.x, (rc.top+rc.bottom)/2+m_mapGridBegin.y);
	}
	VOID			ResetCenter(UIDialog* dlg){
		if(!dlg->m_bMapEnable)
			return;
		for(int i=0;i<dlg->GetControlCount();i++){
			UIControl* c = dlg->GetControlPtr(i);
			if(c->m_bIsGridObject){
				Pixel ind = dlg->GetMapGridIndex(c->GetPos());
				Pixel p = dlg->GetMapGridCenter(ind.x, ind.y);
				p.y -= c->m_sGridOffsetY;
				c->SetPos(p);
			}
		}
	}
	VOID			UIDialog::SetMapGridInfo(INT x, INT y, const Map2DGridInfo& v)
	{CPUTime(UIDialog);
		int cx = m_mapGridCount.cx;
		int cy = m_mapGridCount.cy;
		if(x < 0 || x >= cx)
			return;
		if(y < 0 || y >= cy)
			return;
		m_mapGrids[x + y * cx] = v;
	}
	VOID			UIDialog::SetMapEnable(BOOL b)
	{CPUTime(UIDialog);
		m_bMapEnable = b;
		if(m_bMapEnable)
			SetMapGridCount(m_mapGridCount);
	}


	Map2DGridInfo*	GetGridInfo(CDynamicArray<Map2DGridInfo,int,0>& olds, Size& oldGridSize, Size& oldSize, Size& newGridSize, int x, int y)
	{
		int nX	=	x * newGridSize.cx / oldGridSize.cx;
		int nY	=	y * newGridSize.cy / oldGridSize.cy;
		static Map2DGridInfo s;
		if(nX < 0 || nX >= oldSize.cx || nY < 0 || nY >= oldSize.cy)
			return &s;
		return &olds[nX + nY * oldSize.cx];
	}

	VOID			UIDialog::SetMapGridSize(const Size& s)
	{CPUTime(UIDialog);
		//Size oldSize		=	m_mapGridCount;
		//Size oldGridSize	=	m_mapGridSize;
		////
		//int nWidth		=	m_mapGridCount.cx * m_mapGridSize.cx;
		//int nHeight		=	m_mapGridCount.cy * m_mapGridSize.cy;
		m_mapGridSize	=	Size(getmax(s.cx, 8), getmax(s.cy, 8));
		//m_mapGridCount.cx	=	nWidth / m_mapGridSize.cx;
		//m_mapGridCount.cy	=	nHeight / m_mapGridSize.cy;
		////
		//m_mapGridCount	=	Size(getmax(m_mapGridCount.cx, 1), getmax(m_mapGridCount.cy, 1));
		//if(oldSize.cx != m_mapGridCount.cx || oldSize.cy != m_mapGridCount.cy || m_mapGrids.size() == 0)
		//{
		//	CDynamicArray<Map2DGridInfo,int,0> olds	=	m_mapGrids;
		//	m_mapGrids.resize(m_mapGridCount.cx * m_mapGridCount.cy);
		//	for(int x=0;x<m_mapGridCount.cx;x++)
		//	{
		//		for(int y=0;y<m_mapGridCount.cy;y++)
		//		{
		//			m_mapGrids[x + y * m_mapGridCount.cx]	=	*GetGridInfo(olds, oldGridSize, oldSize, m_mapGridSize, x, y);
		//		}
		//	}
		//}
	}
	BOOL	UIDialog::CreateWay(WayPoints* way, int fx, int fy, int tx, int ty)
	{
		if(this->m_mapAstarGrids.size() == 0)
			return 0;
		Pixel afrom(fx,fy);
		Pixel ato(tx,ty);
		m_astar.setupMap(&this->m_astarCallback);
		if(!m_astar.find_linear(afrom.x, afrom.y, ato.x, ato.y, way->GetIs2DMode()))
			return 0;
		if(m_astar.getfindPath().getNumPath() <= 2)
		{
			way->SetWayCount(2);
			Pixel gfrom = this->AstarMapToMap(fx,fy);
			Pixel gto = this->AstarMapToMap(tx,ty);
			Pixel ff = this->GetMapGridCenter(gfrom.x, gfrom.y);
			Pixel tt = this->GetMapGridCenter(gto.x, gto.y);
			way->SetWayPoint(0, Phantom::float3((float)ff.x, (float)ff.y, 0));
			way->SetWayPoint(1, Phantom::float3((float)tt.x, (float)tt.y, 0));
			return TRUE;
		}
		int index = 0;
		way->SetWayCount(m_astar.getfindPath().getNumPath());
		for(int i= m_astar.getfindPath().getNumPath() - 1;i >= 0; i--)// - 1; i > 0; i--)
		{
			Pixel* p	=	m_astar.getfindPath().getPath(i);
			Pixel gp = this->AstarMapToMap(p->x, p->y);
			Pixel pixel = this->GetMapGridCenter(gp.x, gp.y);
			Phantom::float3 v( pixel.x, pixel.y, 0 );
			way->SetWayPoint(index++, v);
		}
		return true;
	}
	BOOL	UIDialog::CreateWay(WayPoints* way, const Pixel& from, const Pixel& to)
	{CPUTime(UIDialog);
		if(this->m_mapAstarGrids.size() == 0)
			return 0;
		Pixel gfrom = this->GetMapGridIndex(from);
		Pixel gto = this->GetMapGridIndex(to);
		Pixel afrom = this->MapToAstarMap(gfrom.x, gfrom.y);
		Pixel ato = this->MapToAstarMap(gto.x, gto.y);
		//INT nGridSX		=	(INT)((float)from.x / (float)m_mapGridSize.cx);
		//INT nGridSY		=	(INT)((float)from.y / (float)this->m_mapGridSize.cy);
		//INT nGridEX		=	(INT)((float)to.x / (float)this->m_mapGridSize.cx);
		//INT nGridEY		=	(INT)((float)to.y / (float)this->m_mapGridSize.cy);
		m_astar.setupMap(&this->m_astarCallback);
		if(!m_astar.find_linear(afrom.x, afrom.y, ato.x, ato.y, way->GetIs2DMode()))
		{
			return 0;
		}
		if(m_astar.getfindPath().getNumPath() <= 2)
		{
			way->SetWayCount(2);
			if(!way->GetIs2DMode()){
				way->SetWayPoint(0, Phantom::float3((float)from.x, (float)from.y, 0));
				way->SetWayPoint(1, Phantom::float3((float)to.x, (float)to.y, 0));
			}
			else{
				Pixel ff = this->GetMapGridCenter(gfrom.x, gfrom.y);
				Pixel tt = this->GetMapGridCenter(gto.x, gto.y);
				way->SetWayPoint(0, Phantom::float3((float)ff.x, (float)ff.y, 0));
				way->SetWayPoint(1, Phantom::float3((float)tt.x, (float)tt.y, 0));
			}
			return TRUE;
		}
		int index = 0;
		way->SetWayCount(m_astar.getfindPath().getNumPath());
		//way->SetWayPoint(index++, Phantom::float3((float)from.x, (float)from.y, 0));
		for(int i= m_astar.getfindPath().getNumPath() - 1;i >= 0; i--)// - 1; i > 0; i--)
		{
			Pixel* p	=	m_astar.getfindPath().getPath(i);
			Pixel gp = this->AstarMapToMap(p->x, p->y);
			Pixel pixel = this->GetMapGridCenter(gp.x, gp.y);
			Phantom::float3 v( pixel.x, pixel.y, 0 );//(float)p->x * (float)m_mapGridSize.cx + (float)m_mapGridSize.cx/2, (float)p->y * (float)m_mapGridSize.cy + (float)m_mapGridSize.cy/2, 0 );
			way->SetWayPoint(index++, v);
		}
		if(!way->GetIs2DMode()){
			way->SetWayPoint(0, Phantom::float3((float)from.x, (float)from.y, 0));
			way->SetWayPoint(m_astar.getfindPath().getNumPath() - 1, Phantom::float3((float)to.x, (float)to.y, 0));
		}
		//way->SetWayPoint(index++, Phantom::float3((float)to.x, (float)to.y, 0));
		return true;
	}

	VOID	UIDialog::GetFontData(INT nFontID, INT& nHeight, INT& nBold, INT& nUnderLine)
	{CPUTime(UIDialog);
		FontNode2D* pFontNode	=	GetDlgManager()->GetFontNode(nFontID);
		if(!pFontNode)return;
		nHeight	=	pFontNode->nFontHeight;
		nBold	=	pFontNode->nWeight;
		nUnderLine	=	false;
	}

	Size	UIDialog::GetImageSize(INT nImageIndex)
	{CPUTime(UIDialog);
		Size sz;
		sz.cx	=	32;
		sz.cy	=	32;
		if(nImageIndex < 0 || nImageIndex >= GetDlgManager()->m_textureMap.size())
			return sz;
		Texture* pTexture	=	GetDlgManager()->GetTextureNode(nImageIndex, this);
		sz.cx	=	pTexture->GetWidth();
		sz.cy	=	pTexture->GetHeight();
		return sz;
	}
	//--------------------------------------------------------------------------------------
	BOOL UIDialog::OnInputEvent( unsigned int uMsg, const Pixel& pos, unsigned int touchIndex )
	{CPUTime(UIDialog);
		if( !m_bVisible )
			return false;
		if(this->m_bIsToolTipWindow)
			return false;
		if(uMsg == InputEventID_ButtonDown || uMsg == InputEventID_ButtonDblClk)
		{
			for(size_t i=0;i<m_controls.size();i++)
			{
				if(!m_controls[i]->m_bVisible)
					continue;
				UIGroup* g = this->FindGroupPtr(m_controls[i]->m_groupID);
				if(g && !g->GetVisible())
					continue;
				if(m_controls[i]->m_pChildDialog == 0 && !m_controls[i]->canHaveFocus())
					continue;
				Pixel mousePoint = pos;
				g_manager.SetUIDialogSize(m_controls[i]->m_nAlignMode==0, m_screenSize.cx, m_screenSize.cy, &mousePoint, touchIndex);
				OnSetMouseMatrix(mousePoint);
				Pixel clientPos;
				clientPos	=	m_controls[i]->ScreenToClient(mousePoint);
				if(m_controls[i]->isPtIn(clientPos))
				{
					if(m_controls[i]->m_pChildDialog)
					{
						if(!m_controls[i]->m_pChildDialog->OnInputEvent(uMsg, pos, touchIndex))
							this->RequestFocus(m_controls[i], touchIndex);
					}
					else
						this->RequestFocus(m_controls[i], touchIndex);
					return true;
				}
			}
		}
		if(uMsg == Phantom::InputEventID_ButtonDown || uMsg == Phantom::InputEventID_ButtonDblClk)
			this->RequestFocus(0, touchIndex);
		return FALSE;
	}
	BOOL			UIDialog::ShowTipText(UIControl* pControl)
	{CPUTime(UIDialog);
		//if(GetDlgManager()->m_toolTipTextCB)
		//{
		//	int x = 0, y = 0;
		//	if(pControl)
		//	{
		//		Rect rc	=	pControl->GetWindowRect();
		//		ToolTipData d;
		//		d.c	=	pControl;
		//		GetDlgManager()->SetToolTip(&d, pControl->m_toolTipText.strW(), rc, (UIPopupType)pControl->GetToolTipPopupType());
		//	}
		//	else if(GetDlgManager())
		//		GetDlgManager()->SetToolTip(0, 0, Rect(), UIPopupType_LEFT);
		//}
		return FALSE;
	}
	INT		UIDialog::DrawImage(INT nImageIndex, const Rect& rcImage, BOOL bSelect, VOID* lpParam, INT color)
	{CPUTime(UIDialog);
		if(nImageIndex < 0 || nImageIndex >= GetDlgManager()->m_textureMap.size())
			return 0;
		TextRenderParam* p = (TextRenderParam*)lpParam;
		int saveIndex		=	p->e->textureIndexNew;
		p->e->textureIndexNew	=	nImageIndex;
		unsigned int c		=	Phantom::color4(color);
		if(bSelect)
			c	=	0xff3f3f3f;
		Rect rcScreen	=	rcImage;
		BOOL bSaveWrap	=	p->e->m_bWrap;
		BOOL bSaveScale	=	p->e->m_bRenderScale;
		p->e->m_bWrap	=	false;
		p->e->m_bRenderScale	=	true;
		int	nRet	=	DrawElement(p->c->IsEnabled(), p->e, NULL, &rcScreen, c, &p->c->m_currentAnim);
		p->e->m_bWrap			=	bSaveWrap;
		p->e->m_bRenderScale	=	bSaveScale;
		p->e->textureIndexNew	=	saveIndex;
		return nRet;
	}
	char	UIDialog::DrawRect2D( const Rect* pRect, unsigned int color, char bFill, BOOL bLingXing )
	{CPUTime(UIDialog);
		Rect rcScreen = *pRect;
		OffsetRect( &rcScreen, m_dlgX, m_dlgY );
		Phantom::color4 c =	Phantom::color4(color);
		c	=	m_dlgColor * c;
		g_manager.DrawRect2D(rcScreen, c, bFill, bLingXing);
		return true;
	}

	VOID	UIDialog::DrawSpriteNoOffsetE(Texture* texture, Rect* screenRc, Rect* prcTexture, Rect* prcDest, const Phantom::color4& c, Phantom::float3* pCenter, Phantom::matrix4x4* pMatRot, int nHighlight, BOOL bEnabled)
	{CPUTime(UIDialog);
		Rect rcTexture	=	*prcTexture;
		int maxWidth		=	prcDest->right - prcDest->left;
		int maxHeight		=	prcDest->bottom - prcDest->top;
		int textureWidth	=	rcTexture.right - rcTexture.left;
		int textureHeight	=	rcTexture.bottom - rcTexture.top;
		if(textureWidth <= 0 || textureHeight <= 0)
			return;
		if(textureWidth > maxWidth)
		{
			textureWidth	=	maxWidth;
			rcTexture.right	=	rcTexture.left + textureWidth;
		}
		if(textureHeight > maxHeight)
		{
			textureHeight	=	maxHeight;
			rcTexture.bottom=	rcTexture.top + textureHeight;
		}
		if(texture->GetWidth() == 0 || texture->GetHeight() == 0)
			return;
		float fScaleX		= 1.0f;
		float fScaleY		= 1.0f;
		Phantom::matrix4x4 matTransform;
		matrixScaling( &matTransform, fScaleX, fScaleY, 1.0f );
		Phantom::float3 center;
		if(pMatRot)
		{
			center	=	Phantom::float3(pCenter->x + screenRc->left - prcDest->left, pCenter->y + screenRc->top - prcDest->top, 0);
			pCenter	=	&center;
			//
			matTransform._41 = -(float)pCenter->x;
			matTransform._42 = -(float)pCenter->y;
			matrixMultiply(&matTransform, &matTransform, pMatRot);
			matTransform._41 = (float)prcDest->left + pCenter->x;
			matTransform._42 = (float)prcDest->top + pCenter->y;
		}
		else
		{
			matTransform._41 = (float)prcDest->left;
			matTransform._42 = (float)prcDest->top;
		}
		g_manager.DrawSprite2D( texture, rcTexture, pCenter, &c, &matTransform, 0, nHighlight, bEnabled, 0, 0, this->m_bEnableDlgMatrix);
	}
	BOOL	IsClipRender(const Rect& rc, const Phantom::float3* center){
		if(center){
			Rect temp = rc;
			Phantom::float2 c(center->x, center->y);
			Phantom::float2 v1(temp.left, temp.top);
			Phantom::float2 v2(temp.right, temp.top);
			Phantom::float2 v3(temp.left, temp.bottom);
			Phantom::float2 v4(temp.right, temp.bottom);
			v1 -= c;
			v2 -= c;
			v3 -= c;
			v4 -= c;
			float fv1 = v1.R();
			float fv2 = v2.R();
			float fv3 = v3.R();
			float fv4 = v4.R();
			fv1 = getmax(fv1, fv2);
			fv1 = getmax(fv1, fv3);
			fv1 = getmax(fv1, fv4);
			temp.Set(center->x - fv1, center->y - fv1, center->x + fv1, center->y + fv1);
			return g_manager.IsClipRender(temp);
		}
		return g_manager.IsClipRender(rc);
	}
	VOID	DisableColor(color4& c){
		float f = c.r*0.11 + c.g * 0.59 + c.b * 0.30;
		c.setRGBA(f,f,f,c.a);
	}
	BOOL UIDialog::DrawTexture( BOOL bEnabled, Texture* texture, Rect* textureRect, Rect* prcDest, const Phantom::color4& color, BOOL bScale, int colorop, int tileWidth, int tileHeight)
	{CPUTime(UIDialog);
		Rect rcScreen	=	*(Rect*)prcDest;
		OffsetRect( &rcScreen, m_dlgX, m_dlgY );
		if(!texture || color.a <= 0.001f || rcScreen.right <= rcScreen.left || rcScreen.bottom <= rcScreen.top)
			return false;
		if(!this->m_bEnableDlgMatrix && !IsClipRender(rcScreen, 0))
			return false;
		Rect rcTexture;
		if(textureRect)
			rcTexture		=	*(Rect*)textureRect;
		else
			rcTexture.Set(0, 0, texture->GetWidth(), texture->GetHeight());
		int maxWidth		=	rcScreen.right - rcScreen.left;
		int maxHeight		=	rcScreen.bottom - rcScreen.top;
		int textureWidth	=	rcTexture.right - rcTexture.left;
		int textureHeight	=	rcTexture.bottom - rcTexture.top;
		if(textureWidth <= 0 || textureHeight <= 0)
			return false;
		Phantom::color4 c			=	m_dlgColor * color;
		if(!bEnabled)
			DisableColor(c);
		int nHighlight		=	0;
		if(colorop == Phantom::TextureBlend_MODULATE2X)
			nHighlight		=	1;
		else if(colorop == Phantom::TextureBlend_MODULATE4X)
			nHighlight		=	2;
		Phantom::float3* pCenter	=	0;
		Phantom::matrix4x4 matRotation;
		Phantom::float3 center;
		Phantom::matrix4x4* pMatRot = 0;
		//if(bRotateEnable)
		//{
		//	pCenter		=	&center;
		//	if(!windowPos)
		//		windowPos	=	(Pixel*)&ani->rc;
		//	center		=	Phantom::float3(ani->center.x + windowPos->x - prcDest->left - pElement->m_imgOffset.x, ani->center.y + windowPos->y - prcDest->top - pElement->m_imgOffset.y, 0);
		//	matrixRotationZ(&matRotation, Radians(ani->rot));
		//	pMatRot		=	&matRotation;
		//}
		if(bScale)
		{
			float fScaleX 		= 1.0f;
			float fScaleY 		= 1.0f;
			fScaleX				*=	((float)maxWidth / (float)textureWidth);
			fScaleY				*=	((float)maxHeight / (float)textureHeight);
			if(pMatRot)
			{
				float left			=	(float)rcScreen.left;
				float top			=	(float)rcScreen.top;
				Phantom::matrix4x4 matTransform;
				matrixScaling( &matTransform, fScaleX, fScaleY, 1.0f );
				Phantom::float3 tempCenter(0,0,0);
				pCenter		=	&tempCenter;
				tempCenter	=	center;
				tempCenter.x	/=	fScaleX;
				tempCenter.y	/=	fScaleY;
				matTransform._41 = -(float)pCenter->x;
				matTransform._42 = -(float)pCenter->y;
				matrixMultiply(&matTransform, &matTransform, pMatRot);
				matTransform._41 = (float)left + center.x;
				matTransform._42 = (float)top + center.y;
				pCenter		=	&tempCenter;
				//matTransform._41 = (float)left;
				//matTransform._42 = (float)top;
				g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c , &matTransform, 0, nHighlight, bEnabled, 0, 0, m_bEnableDlgMatrix);
			}
			else
			{
				float left			=	(float)rcScreen.left;
				float top			=	(float)rcScreen.top;
				rect_f sr(fScaleX, fScaleY, left, top);
				g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c , 0, &sr, nHighlight, bEnabled, 0, 0, m_bEnableDlgMatrix);
			}
		}
		else
		{
			float fScaleX 		= 1.0f;
			float fScaleY 		= 1.0f;
			//if(pElement->m_bIsInvWTo)fScaleX=-fScaleX;
			//if(pElement->m_bIsInvHTo)fScaleY=-fScaleY;
			int countW			=	maxWidth / textureWidth + 1;
			int countH			=	maxHeight / textureHeight + 1;
			countW	=	getmin(countW, tileWidth);
			countH	=	getmin(countH, tileHeight);
			if(tileWidth > 0 && tileHeight > 0)
			{
				for(int x=0;x<countW;x++)
				{
					for(int y=0;y<countH;y++)
					{
						Rect rcTex(x * textureWidth, y * textureHeight, (x + 1) * textureWidth, (y + 1) * textureHeight);
						if(rcTex.right > maxWidth)
							rcTex.right = maxWidth;
						if(rcTex.bottom > maxHeight)
							rcTex.bottom = maxHeight;
						rcTexture.right		=	rcTexture.left + rcTex.GetWidth();
						rcTexture.bottom	=	rcTexture.top + rcTex.GetHeight();
						//
						rcTex.Offset(rcScreen.left, rcScreen.top);
						Phantom::float3 tempCenter;
						if(pMatRot)
						{
							Phantom::matrix4x4 matTransform = Phantom::matrix4x4::s_identity();
							//
							tempCenter.setxyz(center.x + rcScreen.left - rcTex.left, center.y + rcScreen.top - rcTex.top, 0);
							pCenter	=	&tempCenter;
							matTransform._41 = -(float)pCenter->x;
							matTransform._42 = -(float)pCenter->y;
							matrixMultiply(&matTransform, &matTransform, pMatRot);
							matTransform._41 = (float)rcTex.left + pCenter->x;
							matTransform._42 = (float)rcTex.top + pCenter->y;
							g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c , &matTransform, 0, nHighlight, bEnabled, 0, 0, m_bEnableDlgMatrix);
						}
						else
						{
							rect_f sr(fScaleX, fScaleY, rcTex.left, rcTex.top);
							g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c , 0, &sr, nHighlight, bEnabled, 0, 0, m_bEnableDlgMatrix);
						}
					}
				}
			}
			else if(tileWidth > 0)
			{
				for(int x=0;x<countW;x++)
				{
					int y = 0;
					{
						Rect rcTex(x * textureWidth, y * textureHeight, (x + 1) * textureWidth, (y + 1) * textureHeight);
						if(rcTex.right > maxWidth)
							rcTex.right = maxWidth;
						if(rcTex.bottom > maxHeight)
							rcTex.bottom = maxHeight;
						rcTexture.right		=	rcTexture.left + rcTex.GetWidth();
						rcTexture.bottom	=	rcTexture.top + rcTex.GetHeight();
						//
						rcTex.Offset(rcScreen.left, rcScreen.top);
						Phantom::float3 tempCenter;
						if(pMatRot)
						{
							tempCenter.setxyz(center.x + rcScreen.left - rcTex.left, center.y + rcScreen.top - rcTex.top, 0);
							pCenter	=	&tempCenter;
							Phantom::matrix4x4 matTransform = Phantom::matrix4x4::s_identity();
							matTransform._41 = -(float)pCenter->x;
							matTransform._42 = -(float)pCenter->y;
							matrixMultiply(&matTransform, &matTransform, pMatRot);
							matTransform._41 = (float)rcTex.left + pCenter->x;
							matTransform._42 = (float)rcTex.top + pCenter->y;
							g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, &matTransform, 0, nHighlight, bEnabled, 0, 0, m_bEnableDlgMatrix);
						}
						else
						{
							rect_f sr(fScaleX, fScaleY, rcTex.left, rcTex.top);
							g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, 0, &sr, nHighlight, bEnabled , 0, 0, m_bEnableDlgMatrix);
						}
					}
				}
			}
			else if(tileHeight > 0)
			{
				int x = 0;
				{
					for(int y=0;y<countH;y++)
					{
						Rect rcTex(x * textureWidth, y * textureHeight, (x + 1) * textureWidth, (y + 1) * textureHeight);
						if(rcTex.right > maxWidth)
							rcTex.right = maxWidth;
						if(rcTex.bottom > maxHeight)
							rcTex.bottom = maxHeight;
						rcTexture.right		=	rcTexture.left + rcTex.GetWidth();
						rcTexture.bottom	=	rcTexture.top + rcTex.GetHeight();
						//
						rcTex.Offset(rcScreen.left, rcScreen.top);
						Phantom::float3 tempCenter;
						if(pMatRot)
						{
							tempCenter.setxyz(center.x + rcScreen.left - rcTex.left, center.y + rcScreen.top - rcTex.top, 0);
							pCenter	=	&tempCenter;
							Phantom::matrix4x4 matTransform = Phantom::matrix4x4::s_identity();
							matTransform._41 = -(float)pCenter->x;
							matTransform._42 = -(float)pCenter->y;
							matrixMultiply(&matTransform, &matTransform, pMatRot);
							matTransform._41 = (float)rcTex.left + pCenter->x;
							matTransform._42 = (float)rcTex.top + pCenter->y;
							g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, &matTransform, 0, nHighlight, bEnabled , 0, 0, m_bEnableDlgMatrix);
						}
						else
						{
							rect_f sr(fScaleX, fScaleY, rcTex.left, rcTex.top);
							g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, 0, &sr, nHighlight, bEnabled, 0, 0, m_bEnableDlgMatrix);
						}
					}
				}
			}
			else
			{
				if((rcTexture.right - rcTexture.left) > maxWidth)
				{
					textureWidth	=	maxWidth;
					rcTexture.right	=	rcTexture.left + maxWidth;
				}
				if((rcTexture.bottom - rcTexture.top) > maxHeight)
				{
					textureHeight	=	maxHeight;
					rcTexture.bottom=	rcTexture.top + maxHeight;
				}
				if(pMatRot)
				{
					int left			=	rcScreen.left;
					int top				=	rcScreen.top;
					Phantom::matrix4x4 matTransform = Phantom::matrix4x4::s_identity();
					matTransform._41 = -(float)pCenter->x;
					matTransform._42 = -(float)pCenter->y;
					int www = rcScreen.GetWidth() - pCenter->x;
					int hhh = rcScreen.GetHeight() - pCenter->y;
					matrixMultiply(&matTransform, &matTransform, pMatRot);
					matTransform._41 = (float)left + pCenter->x;
					matTransform._42 = (float)top + pCenter->y;
					g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, &matTransform, 0, nHighlight, bEnabled, 0, 0 , m_bEnableDlgMatrix);
				}
				else
				{
					int left			=	rcScreen.left;
					int top				=	rcScreen.top;
					rect_f sr(fScaleX, fScaleY, left, top);
					g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, 0, &sr, nHighlight, bEnabled, 0, 0 , m_bEnableDlgMatrix);
				}
				
			}
		}
		return true;
	}
	BOOL UIDialog::DrawElementColor( BOOL bEnabled, UIElement* pElement, Rect* prcDest, const color4& color, ControlAnimation* ani, UIControl* ptr, const matrix4x4* renderMat)
	{CPUTimeT(UIDialog);
		Rect rcScreen	=	*(Rect*)prcDest;
		OffsetRect( &rcScreen, m_dlgX, m_dlgY );
		if(color.a <= 0.001f || rcScreen.right <= rcScreen.left || rcScreen.bottom <= rcScreen.top)
			return false;
		Phantom::float3* pCenter	=	0;
		Phantom::matrix4x4 matRotation;
		Phantom::float3 center;
		Phantom::matrix4x4* pMatRot = 0;
		if(ani && ani->bRotateEnable)
		{
			pCenter		=	&center;
			center		=	Phantom::float3(ani->center.x + ani->rc.left - prcDest->left - pElement->m_imgOffset.x, ani->center.y + ani->rc.top - prcDest->top - pElement->m_imgOffset.y, 0);
			matrixRotationZ(&matRotation, Radians(ani->rot));
			pMatRot		=	&matRotation;
		}
		if(!this->m_bEnableDlgMatrix && !IsClipRender(rcScreen, pCenter))
			return false;
		Phantom::color4 c			=	m_dlgColor * color;
		c.a *= ptr->GetGroupPtr()->m_fOpacity;
		int nHighlight		=	0;
		if(pElement->m_nColorOP == Phantom::TextureBlend_MODULATE2X)
			nHighlight		=	1;
		else if(pElement->m_nColorOP == Phantom::TextureBlend_MODULATE4X)
			nHighlight		=	2;
		//
		if(pMatRot)
		{
			int left			=	rcScreen.left;
			int top				=	rcScreen.top;
			if(pElement->m_imgOffset.x > 0 || pElement->m_imgOffset.y > 0)
			{
				Phantom::matrix4x4 matR;
				matR.identity();
				matR.SetPosition(pElement->m_imgOffset.x, pElement->m_imgOffset.y, 0);
				matrixMultiply(&matR, &matR, pMatRot);
				left	-=	matR._41;
				top		-=	matR._42;
			}
			Phantom::matrix4x4 matTransform = Phantom::matrix4x4::s_identity();
			matTransform._41 = -(float)pCenter->x;
			matTransform._42 = -(float)pCenter->y;
			int www = rcScreen.GetWidth() - pCenter->x;
			int hhh = rcScreen.GetHeight() - pCenter->y;
			matrixMultiply(&matTransform, &matTransform, pMatRot);
			matTransform._41 = (float)left + pCenter->x;
			matTransform._42 = (float)top + pCenter->y;
			g_manager.DrawSprite2D(rcScreen, pCenter, &c, &matTransform, 0, nHighlight, bEnabled, pElement->m_bIsInvWTo, pElement->m_bIsInvHTo , m_bEnableDlgMatrix);
		}
		else
		{
			int left			=	rcScreen.left + pElement->m_imgOffset.x;
			int top				=	rcScreen.top + pElement->m_imgOffset.y;
			rect_f sr(1, 1, left, top);
			g_manager.DrawSprite2D(rcScreen, pCenter, &c, 0, &sr, nHighlight, bEnabled, pElement->m_bIsInvWTo, pElement->m_bIsInvHTo , m_bEnableDlgMatrix);
		}
		return false;
		//Rect rcScreen	=	*(Rect*)prcDest;
		//OffsetRect( &rcScreen, m_dlgX, m_dlgY );
		//if(color.a <= 0.001f || rcScreen.right <= rcScreen.left || rcScreen.bottom <= rcScreen.top)
		//	return false;
		//Phantom::float3* pCenter	=	0;
		//Phantom::matrix4x4 mat;
		//Phantom::float3 center;
		//Phantom::matrix4x4* pMatRot = 0;
		//BOOL bRot = ani->bRotateEnable;
		//if(bRot&&(int)(ani->rot*1000.0f)==0)
		//	bRot = false;
		//if(ani && bRot)
		//{
		//	pCenter		=	&center;
		//	center		=	Phantom::float3(ani->center.x + ani->rc.left - prcDest->left - pElement->m_imgOffset.x, ani->center.y + ani->rc.top - prcDest->top - pElement->m_imgOffset.y, 0);
		//	if(!renderMat)
		//	{
		//		matrixRotationZ(&mat, Radians(ani->rot));
		//		pMatRot		=	&mat;
		//	}
		//}
		//if(!this->m_bEnableDlgMatrix && !IsClipRender(rcScreen, pCenter))
		//	return false;
		//Phantom::color4 c			=	m_dlgColor * color;
		//c.a *= ptr->GetGroupPtr()->m_fOpacity;
		//int nHighlight		=	0;
		//if(pElement->m_nColorOP == Phantom::TextureBlend_MODULATE2X)
		//	nHighlight		=	1;
		//else if(pElement->m_nColorOP == Phantom::TextureBlend_MODULATE4X)
		//	nHighlight		=	2;
		////
		//if(!renderMat){
		//	if(pMatRot)
		//	{
		//		Phantom::matrix4x4 matT;
		//		matrixScaling(&matT, rcScreen.GetWidth(), rcScreen.GetHeight(), 1);
		//		matrixMultiply(&mat, &matT, pMatRot);
		//	}
		//	else
		//		matrixScaling(&mat, rcScreen.GetWidth(), rcScreen.GetHeight(), 1);
		//	renderMat = &mat;
		//	float cx = (float)ani->center.x * ptr->m_scale.x;
		//	float cy = (float)ani->center.y * ptr->m_scale.y;
		//	mat._41 = (float)rcScreen.left + cx;
		//	mat._42 = (float)rcScreen.top + cy;
		//	mat._43 = cx / (float)rcScreen.GetWidth();
		//	mat._44 = cy / (float)rcScreen.GetHeight();
		//}
		////g_manager.DrawSprite2D(rcScreen, &center, &c, renderMat, 
		//g_manager.DrawSprite2D(bEnabled, c, renderMat, 0, m_bEnableDlgMatrix);
		return true;
	}
	BOOL UIDialog::DrawElement( BOOL bEnabled, UIElement* pElement, Rect* textureRect, Rect* prcDest, const Phantom::color4& color, ControlAnimation* ani, UIControl* ptr, const matrix4x4* renderMat)
	{CPUTime(UIDialog);
		g_manager->SetBlendMode((BlendMode)pElement->m_nSrcBlend,(BlendMode)pElement->m_nDestBlend);
		if(pElement->GetTextureNewID()<0){
			if(pElement->m_bNoTexture){
				return this->DrawElementColor(bEnabled, pElement, prcDest, color, ani, ptr, renderMat);
			}
			return false;
		}
		int dx = m_dlgX;
		int dy = m_dlgY;
		if(this->m_pParentControl)
		{
			dx += m_pParentControl->m_dialogPtr->m_x;
			dy += m_pParentControl->m_dialogPtr->m_y;
		}

		Texture* texture	=	GetDlgManager()->GetTextureNode(pElement->GetTextureNewID(), this);
		Rect rcScreen	=	*(Rect*)prcDest;
		OffsetRect( &rcScreen, dx, dy );
		if(!texture || color.a <= 0.001f || rcScreen.right <= rcScreen.left || rcScreen.bottom <= rcScreen.top)
		{
			return false;
		}
		renderMat = 0;
		Phantom::float3* pCenter	=	0;
		Phantom::matrix4x4 matRotation;
		Phantom::float3 center;
		Phantom::matrix4x4* pMatRot = 0;
		BOOL bRot = ani->bRotateEnable;
		if(bRot&&(int)(ani->rot*1000.0f)==0)
			bRot = false;
		if(bRot)
		{
			pCenter		=	&center;
			center		=	Phantom::float3(ani->center.x + ani->rc.left - prcDest->left - pElement->m_imgOffset.x, ani->center.y + ani->rc.top - prcDest->top - pElement->m_imgOffset.y, 0);
		}
		Rect rcClip = rcScreen;
		if(pElement->m_nTileWidth>0||pElement->m_nTileHeight>0){
			int xx = pElement->m_nTileWidth * pElement->m_nTileOffX+pElement->m_nTileHeight * pElement->m_nTileOffX;
			int yy = pElement->m_nTileWidth*pElement->m_nTileOffY+pElement->m_nTileHeight*pElement->m_nTileOffY;
			rcClip.inflate(abs(xx),abs(yy));
		}
		if(!this->m_bEnableDlgMatrix && !IsClipRender(rcClip, pCenter))
			return false;
		Rect rcTexture;
		if(textureRect)
			rcTexture		=	*(Rect*)textureRect;
		else
			rcTexture.Set(0, 0, texture->GetWidth(), texture->GetHeight());
		int maxWidth		=	rcScreen.right - rcScreen.left;
		int maxHeight		=	rcScreen.bottom - rcScreen.top;
		int textureWidth	=	pElement->m_imgSize.cx;//rcTexture.right - rcTexture.left;
		int textureHeight	=	pElement->m_imgSize.cy;//rcTexture.bottom - rcTexture.top;
		if(textureWidth <= 0 || textureHeight <= 0)
			return false;
		Phantom::color4 c			=	m_dlgColor * color;
		if(!bEnabled)
			DisableColor(c);
		c.a *= ptr->GetGroupPtr()->m_fOpacity;
		int nHighlight		=	0;
		if(pElement->m_nColorOP == Phantom::TextureBlend_MODULATE2X)
			nHighlight		=	1;
		else if(pElement->m_nColorOP == Phantom::TextureBlend_MODULATE4X)
			nHighlight		=	2;
		//
		BOOL bEqual = ((maxWidth == textureWidth) && (maxHeight == textureHeight));
		if(pElement->m_bRenderScale && !bEqual)
		{
			if(pElement->m_bWrap)
			{
				if(bRot){
					matrixRotationZ(&matRotation, Radians(ani->rot));
					pMatRot		=	&matRotation;
				}
				Pixel wrapSize	=	pElement->m_wrapWidth;
				if(wrapSize.x > (maxWidth / 2))
					wrapSize.x = maxWidth / 2;
				if(wrapSize.y > (maxHeight / 2))
					wrapSize.y = maxHeight / 2;
				//
				if(wrapSize.x == 0 && wrapSize.y > 0)
				{
					Rect rcTop, rcBottom, rcMiddle, rcM, rcDestM,rcDestT,rcDestB;
					SetRect(&rcTop, 0, 0, textureWidth, wrapSize.y);
					SetRect(&rcBottom, 0, textureHeight - wrapSize.y, textureWidth, textureHeight);
					SetRect(&rcMiddle, 0, wrapSize.y, textureWidth, textureHeight - wrapSize.y);
					rcTop.Offset(rcTexture.left, rcTexture.top);
					rcBottom.Offset(rcTexture.left, rcTexture.top);
					rcMiddle.Offset(rcTexture.left, rcTexture.top);
					SetRect(&rcDestM, rcScreen.left, rcScreen.top + wrapSize.y, rcScreen.right, rcScreen.bottom - wrapSize.y);
					int	nDestMiddleX	=	(rcScreen.right + rcScreen.left) / 2;
					int	nDestMiddleY	=	(rcScreen.top + rcScreen.bottom) / 2;
					//
					SetRect(&rcDestT, rcScreen.left, rcScreen.top, rcScreen.right, rcScreen.top + wrapSize.y);
					rcDestT.bottom = getmin(rcDestT.bottom, nDestMiddleY);
					//
					SetRect(&rcDestB, rcScreen.left, rcScreen.bottom - wrapSize.y, rcScreen.right, rcScreen.bottom);
					rcDestB.top = getmax(rcDestB.top, nDestMiddleY);
					//
					DrawSpriteNoOffsetE(texture, &rcScreen, &rcTop, &rcDestT, c, pCenter, pMatRot, nHighlight, bEnabled);
					DrawSpriteNoOffsetE(texture, &rcScreen, &rcBottom, &rcDestB, c, pCenter, pMatRot, nHighlight, bEnabled);
					//
					int numHeight = rcDestM.GetHeight() / wrapSize.y;
					if((rcDestM.GetHeight() % wrapSize.y) > 0)
						numHeight++;
					int top = rcDestM.top;
					for(int i=0;i<numHeight;i++)
					{
						rcM.Set(rcDestM.left, top, rcDestM.right, top+wrapSize.y);
						if(rcM.bottom > rcDestM.bottom)
						{
							rcMiddle.bottom -= (rcM.bottom-rcDestM.bottom);
							rcM.bottom = rcDestM.bottom;
						}
						DrawSpriteNoOffsetE(texture, &rcScreen, &rcMiddle, &rcM, c, pCenter, pMatRot, nHighlight, bEnabled);
						top += wrapSize.y;
					}
				}
				else if(wrapSize.y == 0 && wrapSize.x > 0)
				{
					Rect rcLeft, rcRight, rcMiddle, rcM, rcDestM,rcDestL,rcDestR;
					SetRect(&rcLeft, 0, 0, wrapSize.x, textureHeight);
					SetRect(&rcRight, textureWidth - wrapSize.x, 0, textureWidth, textureHeight);
					SetRect(&rcMiddle, wrapSize.x, 0, textureWidth - wrapSize.x, textureHeight);
					rcLeft.Offset(rcTexture.left, rcTexture.top);
					rcRight.Offset(rcTexture.left, rcTexture.top);
					rcMiddle.Offset(rcTexture.left, rcTexture.top);
					//
					SetRect(&rcDestM, rcScreen.left + wrapSize.x, rcScreen.top, rcScreen.right - wrapSize.x, rcScreen.bottom);
					SetRect(&rcDestL, rcScreen.left, rcScreen.top, rcScreen.left + wrapSize.x, rcScreen.bottom);
					SetRect(&rcDestR, rcScreen.right - wrapSize.x, rcScreen.top, rcScreen.right, rcScreen.bottom);
					DrawSpriteNoOffsetE(texture, &rcScreen, &rcLeft, &rcDestL, c, pCenter, pMatRot, nHighlight, bEnabled);
					DrawSpriteNoOffsetE(texture, &rcScreen, &rcRight, &rcDestR, c, pCenter, pMatRot, nHighlight, bEnabled);
					//
					int numWidth = rcDestM.GetWidth() / wrapSize.x;
					if((rcDestM.GetWidth() % wrapSize.x) > 0)
						numWidth++;
					int left = rcDestM.left;
					for(int i=0;i<numWidth;i++)
					{
						rcM.Set(left, rcDestM.top, left + wrapSize.x, rcDestM.bottom);
						if(rcM.right > rcDestM.right)
						{
							rcMiddle.right -= (rcM.right-rcDestM.right);
							rcM.right = rcDestM.right;
						}
						DrawSpriteNoOffsetE(texture, &rcScreen, &rcMiddle, &rcM, c, pCenter, pMatRot, nHighlight, bEnabled);
						left += wrapSize.x;
					}

				}
				else
				{
					Rect rcLeftTop, rcLeftBottom, rcRightTop, rcRightBottom, rcLeft, rcTop, rcRight, rcBottom, rcMiddle;
					//
					SetRect(&rcLeftTop, 0, 0, wrapSize.x, wrapSize.y);
					SetRect(&rcLeftBottom, 0, textureHeight - wrapSize.y, wrapSize.x, textureHeight);
					SetRect(&rcRightTop, textureWidth - wrapSize.x, 0, textureWidth, wrapSize.y);
					SetRect(&rcRightBottom, textureWidth - wrapSize.x, textureHeight - wrapSize.y, textureWidth, textureHeight);
					//
					SetRect(&rcLeft, 0, wrapSize.y, wrapSize.x, textureHeight - wrapSize.y);
					SetRect(&rcTop, wrapSize.x, 0, textureWidth - wrapSize.x, wrapSize.y);
					SetRect(&rcRight, textureWidth - wrapSize.x, wrapSize.y, textureWidth, textureHeight - wrapSize.y);
					SetRect(&rcBottom, wrapSize.x, textureHeight - wrapSize.y, textureWidth - wrapSize.x, textureHeight);
					SetRect(&rcMiddle, wrapSize.x, wrapSize.y, textureWidth - wrapSize.x, textureHeight - wrapSize.y);

					rcLeftTop.Offset(rcTexture.left, rcTexture.top);
					rcLeftBottom.Offset(rcTexture.left, rcTexture.top);
					rcRightTop.Offset(rcTexture.left, rcTexture.top);
					rcRightBottom.Offset(rcTexture.left, rcTexture.top);
					rcLeft.Offset(rcTexture.left, rcTexture.top);
					rcTop.Offset(rcTexture.left, rcTexture.top);
					rcRight.Offset(rcTexture.left, rcTexture.top);
					rcBottom.Offset(rcTexture.left, rcTexture.top);
					rcMiddle.Offset(rcTexture.left, rcTexture.top);

					Rect rcDestM, rcDestLT, rcDestRT, rcDestLB, rcDestRB, rcDestL, rcDestR, rcDestT, rcDestB;
					SetRect(&rcDestM, rcScreen.left + wrapSize.x, rcScreen.top + wrapSize.y, rcScreen.right - wrapSize.x, rcScreen.bottom - wrapSize.y);
					int	nDestMiddleX	=	(rcScreen.right + rcScreen.left) / 2;
					int	nDestMiddleY	=	(rcScreen.top + rcScreen.bottom) / 2;
					//
					SetRect(&rcDestLT, rcScreen.left, rcScreen.top, rcScreen.left + wrapSize.x, rcScreen.top + wrapSize.y);
					rcDestLT.right = getmin(rcDestLT.right, nDestMiddleX);		rcDestLT.bottom = getmin(rcDestLT.bottom, nDestMiddleY);
					//
					SetRect(&rcDestRT, rcScreen.right - wrapSize.x, rcScreen.top, rcScreen.right, rcScreen.top + wrapSize.y);
					rcDestRT.left = getmax(rcDestRT.left, nDestMiddleX);		rcDestRT.bottom = getmin(rcDestRT.bottom, nDestMiddleY);
					//
					SetRect(&rcDestLB, rcScreen.left, rcScreen.bottom - wrapSize.y, rcScreen.left + wrapSize.x, rcScreen.bottom);
					rcDestLB.right = getmin(rcDestLB.right, nDestMiddleX);		rcDestLB.top = getmax(rcDestLB.top, nDestMiddleY);
					//
					SetRect(&rcDestRB, rcScreen.right - wrapSize.x, rcScreen.bottom - wrapSize.y, rcScreen.right, rcScreen.bottom);
					rcDestRB.left = getmax(rcDestRB.left, nDestMiddleX);		rcDestLB.top = getmax(rcDestLB.top, nDestMiddleY);
					//
					SetRect(&rcDestL, rcScreen.left, rcScreen.top + wrapSize.y, rcScreen.left + wrapSize.x, rcScreen.bottom - wrapSize.y);
					rcDestL.right = getmin(rcDestL.right, nDestMiddleX);
					//
					SetRect(&rcDestR, rcScreen.right - wrapSize.x, rcScreen.top + wrapSize.y, rcScreen.right, rcScreen.bottom - wrapSize.y);
					rcDestR.left = getmax(rcDestR.left, nDestMiddleX);
					//
					SetRect(&rcDestT, rcScreen.left + wrapSize.x, rcScreen.top, rcScreen.right - wrapSize.x, rcScreen.top + wrapSize.y);
					rcDestT.bottom = getmin(rcDestT.bottom, nDestMiddleY);
					//
					SetRect(&rcDestB, rcScreen.left + wrapSize.x, rcScreen.bottom - wrapSize.y, rcScreen.right - wrapSize.x, rcScreen.bottom);
					rcDestB.top = getmax(rcDestB.top, nDestMiddleY);
					//
					if(rcDestM.left < rcDestM.right && rcDestM.top < rcDestM.bottom)
					{
						int nMiddleWidth	=	rcMiddle.right - rcMiddle.left;
						int nMiddleHeight	=	rcMiddle.bottom - rcMiddle.top;
						if(nMiddleWidth <= 0)
							nMiddleWidth	=	1;
						if(nMiddleHeight <= 0)
							nMiddleHeight	=	1;
						int numWidth	=	(rcDestM.right - rcDestM.left) / nMiddleWidth + 1;
						int numHeight	=	(rcDestM.bottom - rcDestM.top) / nMiddleHeight + 1;
						for(int x=0;x<numWidth;x++)
						{
							rcDestM.top		=	rcScreen.top + wrapSize.y;
							for(int y=0;y<numHeight;y++)
							{
								Rect rcRender;
								rcRender.Set(rcDestM.left, rcDestM.top, rcDestM.left + nMiddleWidth, rcDestM.top + nMiddleHeight);
								rcRender.right	=	getmin(rcRender.right, rcDestM.right);
								rcRender.bottom	=	getmin(rcRender.bottom, rcDestM.bottom);
								DrawSpriteNoOffsetE(texture, &rcScreen, &rcMiddle, &rcRender, c, pCenter, pMatRot, nHighlight, bEnabled);
								//
								rcDestM.top		+=	nMiddleHeight;
							}
							rcDestM.left	+=	nMiddleWidth;
						}
					}
					DrawSpriteNoOffsetE(texture, &rcScreen, &rcLeftTop, &rcDestLT, c, pCenter, pMatRot, nHighlight, bEnabled);
					DrawSpriteNoOffsetE(texture, &rcScreen, &rcRightTop, &rcDestRT, c, pCenter, pMatRot, nHighlight, bEnabled);
					DrawSpriteNoOffsetE(texture, &rcScreen, &rcLeftBottom, &rcDestLB, c, pCenter, pMatRot, nHighlight, bEnabled);
					DrawSpriteNoOffsetE(texture, &rcScreen, &rcRightBottom, &rcDestRB, c, pCenter, pMatRot, nHighlight, bEnabled);
					//
					if(rcDestL.top < rcDestL.bottom)
					{
						int nLeftHeight	=	rcLeft.bottom - rcLeft.top;
						if(nLeftHeight <= 0)nLeftHeight = 1;
						int numHeight	=	(rcDestL.bottom - rcDestL.top) / nLeftHeight + 1;
						for(int y=0;y<numHeight;y++)
						{
							Rect rcRender;
							SetRect(&rcRender, rcDestL.left, rcDestL.top, rcDestL.right, rcDestL.top + nLeftHeight);
							rcRender.bottom	=	getmin(rcRender.bottom, rcDestL.bottom);
							DrawSpriteNoOffsetE(texture, &rcScreen, &rcLeft, &rcRender, c, pCenter, pMatRot, nHighlight, bEnabled);
							rcDestL.top += nLeftHeight;
						}
					}
					//
					if(rcDestR.top < rcDestR.bottom)
					{
						int nLeftHeight	=	rcRight.bottom - rcRight.top;
						if(nLeftHeight <= 0)nLeftHeight = 1;
						int numHeight	=	(rcDestR.bottom - rcDestR.top) / nLeftHeight + 1;
						for(int y=0;y<numHeight;y++)
						{
							Rect rcRender;
							SetRect(&rcRender, rcDestR.left, rcDestR.top, rcDestR.right, rcDestR.top + nLeftHeight);
							rcRender.bottom	=	getmin(rcRender.bottom, rcDestR.bottom);
							DrawSpriteNoOffsetE(texture, &rcScreen, &rcRight, &rcRender, c, pCenter, pMatRot, nHighlight, bEnabled);
							rcDestR.top += nLeftHeight;
						}
					}
					//
					if(rcDestT.left < rcDestT.right)
					{
						int nLeftHeight	=	rcTop.right - rcTop.left;
						if(nLeftHeight <= 0)nLeftHeight = 1;
						int numWidth	=	(rcDestT.right - rcDestT.left) / nLeftHeight + 1;
						for(int x=0;x<numWidth;x++)
						{
							Rect rcRender;
							SetRect(&rcRender, rcDestT.left, rcDestT.top, rcDestT.left + nLeftHeight, rcDestT.bottom);
							rcRender.right	=	getmin(rcRender.right, rcDestT.right);
							DrawSpriteNoOffsetE(texture, &rcScreen, &rcTop, &rcRender, c, pCenter, pMatRot, nHighlight, bEnabled);
							rcDestT.left += nLeftHeight;
						}
					}
					//
					if(rcDestB.left < rcDestB.right)
					{
						int nLeftHeight	=	rcBottom.right - rcBottom.left;
						if(nLeftHeight <= 0)nLeftHeight = 1;
						int numWidth	=	(rcDestB.right - rcDestB.left) / nLeftHeight + 1;
						for(int x=0;x<numWidth;x++)
						{
							Rect rcRender;
							SetRect(&rcRender, rcDestB.left, rcDestB.top, rcDestB.left + nLeftHeight, rcDestB.bottom);
							rcRender.right	=	getmin(rcRender.right, rcDestB.right);
							DrawSpriteNoOffsetE(texture, &rcScreen, &rcBottom, &rcRender, c, pCenter, pMatRot, nHighlight, bEnabled);
							rcDestB.left += nLeftHeight;
						}
					}
				}
			}
			else
			{
				if(renderMat)
					g_manager.DrawSprite2D(texture, rcTexture, bEnabled, c, renderMat, nHighlight, m_bEnableDlgMatrix);
				else{
					if(bRot){
						matrixRotationZ(&matRotation, Radians(ani->rot));
						pMatRot		=	&matRotation;
					}
					float fScaleX 		= 1.0f;
					float fScaleY 		= 1.0f;
					//if(pElement->m_bIsInvWTo)fScaleX=-fScaleX;
					//if(pElement->m_bIsInvHTo)fScaleY=-fScaleY;
					fScaleX				*=	((float)maxWidth / (float)textureWidth);
					fScaleY				*=	((float)maxHeight / (float)textureHeight);
					if(pMatRot)
					{
						float left			=	(float)rcScreen.left;
						float top			=	(float)rcScreen.top;
						if(pElement->m_imgOffset.x > 0 || pElement->m_imgOffset.y > 0)
						{
							Phantom::matrix4x4 matR;
							matR.identity();
							matR.SetPosition(pElement->m_imgOffset.x, pElement->m_imgOffset.y, 0);
							matrixMultiply(&matR, &matR, pMatRot);
							left	-=	matR._41 * fScaleX;
							top		-=	matR._42 * fScaleY;
						}
						Phantom::matrix4x4 matTransform;
						matrixScaling( &matTransform, fScaleX, fScaleY, 1.0f );
						Phantom::float3 tempCenter(0,0,0);
						pCenter		=	&tempCenter;
						tempCenter	=	center;
						tempCenter.x	/=	fScaleX;
						tempCenter.y	/=	fScaleY;
						matTransform._41 = -(float)pCenter->x;
						matTransform._42 = -(float)pCenter->y;
						matrixMultiply(&matTransform, &matTransform, pMatRot);
						matTransform._41 = (float)left + center.x;
						matTransform._42 = (float)top + center.y;
						pCenter		=	&tempCenter;
						g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c , &matTransform, 0, nHighlight, bEnabled, pElement->m_bIsInvWTo, pElement->m_bIsInvHTo , m_bEnableDlgMatrix);
					}
					else
					{
						float left			=	(float)rcScreen.left + (float)pElement->m_imgOffset.x * fScaleX;
						if(pElement->m_bIsInvWTo){
							left			=	(float)rcScreen.right-(float)rcTexture.GetWidth()*fScaleX - (float)pElement->m_imgOffset.x * fScaleX;
						}
						float top			=	(float)rcScreen.top + (float)pElement->m_imgOffset.y * fScaleY;
						rect_f sr(fScaleX, fScaleY, left, top);
						g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c , 0, &sr, nHighlight, bEnabled, pElement->m_bIsInvWTo, pElement->m_bIsInvHTo, m_bEnableDlgMatrix);
					}
				}
			}
		}
		else
		{
			float fScaleX 		= 1.0f;
			float fScaleY 		= 1.0f;
			maxWidth = maxWidth * (1.0f / pElement->m_nTileScaleX);
			maxHeight = maxHeight * (1.0f / pElement->m_nTileScaleY);
			int countW			=	maxWidth / textureWidth + 1;
			int countH			=	maxHeight / textureHeight + 1;
			countW	=	getmin(countW, pElement->m_nTileWidth-1);
			countH	=	getmin(countH, pElement->m_nTileHeight-1);
			if(pElement->m_nTileWidth > 0 || pElement->m_nTileHeight > 0)
			{
				if(bRot){
					matrixRotationZ(&matRotation, Radians(ani->rot));
					pMatRot		=	&matRotation;
				}
				if(countW<0)countW=0;
				if(countH<0)countH=0;
				float tWidth = textureWidth*pElement->m_nTileScaleX;
				float tHeight = textureHeight*pElement->m_nTileScaleY;
				int sx = tWidth;
				int sy = tHeight/2;
				for(int x=0;x<=countW;x++)
				{
					for(int y=0;y<=countH;y++)
					{
						Rect rcTex(x * tWidth, y * tHeight, 0, 0);// (x + 1) * textureWidth, (y + 1) * textureHeight);
						rcTex.right = rcTex.left+textureWidth;
						rcTex.bottom = rcTex.top+textureHeight;
						if(pElement->m_tileXieJiao){//是否斜角渲染
							int yy = -x*sy;
							int xx = x*(sx/2);
							int by = yy + y*sy;
							int bx = xx + y*(sx/2);
							rcTex.Set(bx,by,bx+sx,by+tHeight);
						}
						if(rcTex.right > maxWidth)
							rcTex.right = maxWidth;
						if(rcTex.bottom > maxHeight)
							rcTex.bottom = maxHeight;
						rcTexture.right		=	rcTexture.left + rcTex.GetWidth();
						rcTexture.bottom	=	rcTexture.top + rcTex.GetHeight();
						if(rcTexture.left == rcTexture.right || rcTexture.top == rcTexture.bottom)
							continue;
						//
						rcTex.Offset(rcScreen.left, rcScreen.top);
						if(pElement->m_nTileWidth>0)
							rcTex.Offset(pElement->m_nTileOffX*x, pElement->m_nTileOffY*x);
						if(pElement->m_nTileHeight>0)
							rcTex.Offset(pElement->m_nTileOffX*y, pElement->m_nTileOffY*y);
						Phantom::float3 tempCenter;
						if(pMatRot)
						{
							Phantom::matrix4x4 matTransform = Phantom::matrix4x4::s_identity();
							//
							tempCenter.setxyz(center.x + rcScreen.left - rcTex.left, center.y + rcScreen.top - rcTex.top, 0);
							pCenter	=	&tempCenter;
							matTransform._41 = -(float)pCenter->x;
							matTransform._42 = -(float)pCenter->y;
							matrixMultiply(&matTransform, &matTransform, pMatRot);
							matTransform._41 = (float)rcTex.left + pCenter->x;
							matTransform._42 = (float)rcTex.top + pCenter->y;
							g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c , &matTransform, 0, nHighlight, bEnabled, pElement->m_bIsInvWTo, pElement->m_bIsInvHTo, m_bEnableDlgMatrix);
						}
						else
						{
							if(rcTex.right<0||rcTex.bottom<0||rcTex.left>g_manager.GetBufferWidth()||rcTex.top>g_manager.GetBufferHeight())
								continue;
							rect_f sr(fScaleX*pElement->m_nTileScaleX, fScaleY*pElement->m_nTileScaleY, rcTex.left, rcTex.top);
							g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c , 0, &sr, nHighlight, bEnabled, pElement->m_bIsInvWTo, pElement->m_bIsInvHTo, m_bEnableDlgMatrix);
						}
					}
				}
			}
			//else if(pElement->m_nTileWidth > 0)
			//{
			//	if(bRot){
			//		matrixRotationZ(&matRotation, Radians(ani->rot));
			//		pMatRot		=	&matRotation;
			//	}
			//	for(int x=0;x<countW;x++)
			//	{
			//		int y = 0;
			//		{
			//			Rect rcTex(x * textureWidth, y * textureHeight, (x + 1) * textureWidth, (y + 1) * textureHeight);
			//			if(rcTex.right > maxWidth)
			//				rcTex.right = maxWidth;
			//			if(rcTex.bottom > maxHeight)
			//				rcTex.bottom = maxHeight;
			//			rcTexture.right		=	rcTexture.left + rcTex.GetWidth();
			//			rcTexture.bottom	=	rcTexture.top + rcTex.GetHeight();
			//			if(rcTexture.left == rcTexture.right || rcTexture.top == rcTexture.bottom)
			//				continue;
			//			//
			//			rcTex.Offset(rcScreen.left, rcScreen.top);
			//			Phantom::float3 tempCenter;
			//			if(pMatRot)
			//			{
			//				tempCenter.setxyz(center.x + rcScreen.left - rcTex.left, center.y + rcScreen.top - rcTex.top, 0);
			//				pCenter	=	&tempCenter;
			//				Phantom::matrix4x4 matTransform = Phantom::matrix4x4::s_identity();
			//				matTransform._41 = -(float)pCenter->x;
			//				matTransform._42 = -(float)pCenter->y;
			//				matrixMultiply(&matTransform, &matTransform, pMatRot);
			//				matTransform._41 = (float)rcTex.left + pCenter->x;
			//				matTransform._42 = (float)rcTex.top + pCenter->y;
			//				g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, &matTransform, 0, nHighlight, bEnabled, pElement->m_bIsInvWTo, pElement->m_bIsInvHTo, m_bEnableDlgMatrix);
			//			}
			//			else
			//			{
			//				rect_f sr(fScaleX, fScaleY, rcTex.left, rcTex.top);
			//				g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, 0, &sr, nHighlight, bEnabled , pElement->m_bIsInvWTo, pElement->m_bIsInvHTo, m_bEnableDlgMatrix);
			//			}
			//			
			//		}
			//	}
			//}
			//else if(pElement->m_nTileHeight > 0)
			//{
			//	if(bRot){
			//		matrixRotationZ(&matRotation, Radians(ani->rot));
			//		pMatRot		=	&matRotation;
			//	}
			//	int x = 0;
			//	{
			//		for(int y=0;y<countH;y++)
			//		{
			//			Rect rcTex(x * textureWidth, y * textureHeight, (x + 1) * textureWidth, (y + 1) * textureHeight);
			//			if(rcTex.right > maxWidth)
			//				rcTex.right = maxWidth;
			//			if(rcTex.bottom > maxHeight)
			//				rcTex.bottom = maxHeight;
			//			rcTexture.right		=	rcTexture.left + rcTex.GetWidth();
			//			rcTexture.bottom	=	rcTexture.top + rcTex.GetHeight();
			//			if(rcTexture.left == rcTexture.right || rcTexture.top == rcTexture.bottom)
			//				continue;
			//			//
			//			rcTex.Offset(rcScreen.left, rcScreen.top);
			//			Phantom::float3 tempCenter;
			//			if(pMatRot)
			//			{
			//				tempCenter.setxyz(center.x + rcScreen.left - rcTex.left, center.y + rcScreen.top - rcTex.top, 0);
			//				pCenter	=	&tempCenter;
			//				Phantom::matrix4x4 matTransform = Phantom::matrix4x4::s_identity();
			//				matTransform._41 = -(float)pCenter->x;
			//				matTransform._42 = -(float)pCenter->y;
			//				matrixMultiply(&matTransform, &matTransform, pMatRot);
			//				matTransform._41 = (float)rcTex.left + pCenter->x;
			//				matTransform._42 = (float)rcTex.top + pCenter->y;
			//				g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, &matTransform, 0, nHighlight, bEnabled , pElement->m_bIsInvWTo, pElement->m_bIsInvHTo, m_bEnableDlgMatrix);
			//			}
			//			else
			//			{
			//				rect_f sr(fScaleX, fScaleY, rcTex.left, rcTex.top);
			//				g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, 0, &sr, nHighlight, bEnabled, pElement->m_bIsInvWTo, pElement->m_bIsInvHTo, m_bEnableDlgMatrix);
			//			}
			//		}
			//	}
			//}
			else
			{
				if((rcTexture.right - rcTexture.left) > maxWidth)
				{
					textureWidth	=	maxWidth;
					rcTexture.right	=	rcTexture.left + maxWidth;
				}
				if((rcTexture.bottom - rcTexture.top) > maxHeight)
				{
					textureHeight	=	maxHeight;
					rcTexture.bottom=	rcTexture.top + maxHeight;
				}
				if(renderMat)
					g_manager.DrawSprite2D(texture, rcTexture, bEnabled, c, renderMat, nHighlight, m_bEnableDlgMatrix);
				else{
					if(bRot){
						matrixRotationZ(&matRotation, Radians(ani->rot));
						pMatRot		=	&matRotation;
					}
					if(pMatRot)
					{
						int left			=	rcScreen.left;
						int top				=	rcScreen.top;
						if(pElement->m_imgOffset.x > 0 || pElement->m_imgOffset.y > 0)
						{
							Phantom::matrix4x4 matR;
							matR.identity();
							matR.SetPosition(pElement->m_imgOffset.x, pElement->m_imgOffset.y, 0);
							matrixMultiply(&matR, &matR, pMatRot);
							left	-=	matR._41;
							top		-=	matR._42;
						}
						Phantom::matrix4x4 matTransform = Phantom::matrix4x4::s_identity();
						matTransform._41 = -(float)pCenter->x;
						matTransform._42 = -(float)pCenter->y;
						int www = rcScreen.GetWidth() - pCenter->x;
						int hhh = rcScreen.GetHeight() - pCenter->y;
						matrixMultiply(&matTransform, &matTransform, pMatRot);
						matTransform._41 = (float)left + pCenter->x;
						matTransform._42 = (float)top + pCenter->y;
						g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, &matTransform, 0, nHighlight, bEnabled, pElement->m_bIsInvWTo, pElement->m_bIsInvHTo , m_bEnableDlgMatrix);
					}
					else
					{
						int left			=	rcScreen.left + pElement->m_imgOffset.x;
						int top				=	rcScreen.top + pElement->m_imgOffset.y;
						rect_f sr(fScaleX, fScaleY, left, top);
						g_manager.DrawSprite2D(texture, rcTexture, pCenter, &c, 0, &sr, nHighlight, bEnabled, pElement->m_bIsInvWTo, pElement->m_bIsInvHTo , m_bEnableDlgMatrix);
					}
				}
			}
		}
		return false;
	}
	VOID	RenderScroll(UIDialog* dlg, const Rect& dest, const Phantom::color4& color, Texture* scrollTex, const Rect* scrollRc)
	{
		Texture* tex = scrollTex;
		if(!tex)
			tex = GetDlgManager()->GetTexturePtr(0);
		Rect scroll(247, 2, 254, 126);
		if(!scrollRc)
			scrollRc = &scroll;
		int height = 40;
		int winHeight = dest.GetHeight();
		if((height*2)>winHeight)
			height = winHeight / 2;
		Rect rcsRect(scrollRc->left, scrollRc->top, scrollRc->right, scrollRc->top + height);
		Rect tcxRect(dest.left, dest.top, dest.right, dest.top + height);
		dlg->DrawTexture(true, tex, &rcsRect, &tcxRect, color, false);
		int exth = (winHeight - height*2);
		if(exth > 0)
		{
			int eh = scrollRc->GetHeight() - 24;
			int cnt = exth / eh;
			if((exth%eh)>0)
				cnt++;
			Rect rc(dest.left, dest.top + height, dest.right, dest.top + height + eh);
			for(int y=0;y<cnt;y++)
			{
				bool bexit = false;
				if(rc.bottom > (dest.bottom - height))
				{
					rc.bottom = dest.bottom - height;
					bexit = true;
					if(rc.top >= rc.bottom)
						break;
				}
				Rect scsRect(scrollRc->left, scrollRc->top + 12, scrollRc->right, scrollRc->bottom - 12);
				dlg->DrawTexture(true, tex, &scsRect, &rc, color, false);
				if(bexit)
					break;
				rc.top+=eh;
				rc.bottom+=eh;
			}
		}
		Rect rcs2Rect(scrollRc->left, scrollRc->bottom-height, scrollRc->right, scrollRc->bottom);
		Rect tcs2Rect(dest.left, dest.bottom-height, dest.right, dest.bottom);
		dlg->DrawTexture(true, tex, &rcs2Rect, &tcs2Rect, color, false);
	}

	BOOL			UIDialog::DrawElement( RenderText* textPtr, BOOL bEnabled, UIElement* pElement, Rect* textureRect, Rect* prcDest, const Phantom::color4& color, ControlAnimation* ani, UIControl* ptr, const matrix4x4* renderMat)
	{CPUTimeT(UIDialog);
		g_manager->SetBlendMode((BlendMode)pElement->m_nSrcBlend,(BlendMode)pElement->m_nDestBlend);
		Rect rcScreen	=	*(Rect*)prcDest;
		int dx = m_dlgX;
		int dy = m_dlgY;
		if(this->m_pParentControl)
		{
			dx += m_pParentControl->m_dialogPtr->m_x;
			dy += m_pParentControl->m_dialogPtr->m_y;
		}
		OffsetRect( &rcScreen, dx, dy );
		if(!textPtr || color.a <= 0.001f || rcScreen.right <= rcScreen.left || rcScreen.bottom <= rcScreen.top)
		{
			return false;
		}
		renderMat = 0;
		int nHighlight		=	0;
		if(pElement->m_nColorOP == Phantom::TextureBlend_MODULATE2X)
			nHighlight		=	1;
		else if(pElement->m_nColorOP == Phantom::TextureBlend_MODULATE4X)
			nHighlight		=	2;
		Rect rcTexture;
		if(textureRect)
			rcTexture		=	*(Rect*)textureRect;
		else
			rcTexture.Set(0, 0, textPtr->GetWidth(), textPtr->GetHeight());
		Phantom::color4 c			=	m_dlgColor * color;
		if(!bEnabled)
			DisableColor(c);
		c.a *= ptr->GetGroupPtr()->m_fOpacity;
		Rect temp = rcScreen;
		if(!textPtr->IsFigFont()){
			int dw = rcScreen.GetWidth();
			int dh = rcScreen.GetHeight();
			int tw = rcTexture.GetWidth()/2;
			int th = rcTexture.GetHeight()/2;
			int dtFormat = pElement->dwTextFormat;
			if(dw!=tw)
			{
				if(dtFormat&Phantom::DrawTextAlign_RIGHT)
					rcScreen.left = rcScreen.right - tw;
				else if(dtFormat&Phantom::DrawTextAlign_CENTER)
					rcScreen.left += (dw-tw)/2;
			}
			if(dh!=th)
			{
				if(dtFormat&Phantom::DrawTextAlign_BOTTOM)
					rcScreen.top = rcScreen.bottom - th;
				else if(dtFormat&Phantom::DrawTextAlign_VCENTER)
					rcScreen.top += (dh-th)/2;
			}
			rcScreen.right = rcScreen.left + tw;
			rcScreen.bottom = rcScreen.top + th;
			if(rcScreen.left < temp.left)
			{
				rcTexture.left += (temp.left - rcScreen.left);
				rcScreen.left = temp.left;
			}
			if(rcScreen.right > temp.right)
			{
				rcTexture.right -= (rcScreen.right - temp.right);
				rcScreen.right = temp.right;
			}
			if(rcScreen.top < temp.top)
			{
				rcTexture.top += (temp.top - rcScreen.top);
				rcScreen.top = temp.top;
			}
			if(rcScreen.bottom > temp.bottom)
			{
				rcTexture.bottom -= (rcScreen.bottom - temp.bottom);
				rcScreen.bottom = temp.bottom;
			}
		}
		int maxWidth		=	rcScreen.right - rcScreen.left;
		int maxHeight		=	rcScreen.bottom - rcScreen.top;
		int textureWidth	=	textPtr->GetWidth();
		int textureHeight	=	textPtr->GetHeight();
		if(textureWidth <= 0 || textureHeight <= 0)
			return false;
		Phantom::float3* pCenter	=	0;
		Phantom::matrix4x4 mat;
		Phantom::float3 center;
		Phantom::matrix4x4* pMatRot = 0;
		BOOL bRot = ani->bRotateEnable;
		if(bRot&&(int)(ani->rot*1000.0f)==0)
			bRot = false;
		if(ani && bRot)
		{
			pCenter		=	&center;
			center		=	Phantom::float3(ani->center.x + ani->rc.left - prcDest->left - pElement->m_imgOffset.x, ani->center.y + ani->rc.top - prcDest->top - pElement->m_imgOffset.y, 0);
			center.x	-=	(rcScreen.left - temp.left);
			center.y	-=	(rcScreen.top - temp.top);
			if(!textPtr->IsFigFont()){
				matrixRotationZ(&mat, Radians(ani->rot));
				pMatRot		=	&mat;
			}
		}
		if(!this->m_bEnableDlgMatrix && !IsClipRender(rcScreen, pCenter))
			return false;
		//
		g_manager->SetCullVisible(Phantom::CullMode_CW);
		g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		g_manager->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_INVSRCALPHA);
		//g_manager.DrawTexts(ptr->GetText(), rcScreen, 0, 16.0f, 0, &c, 0);
		float fSX = ptr->m_scale.x;
		float fSY = ptr->m_scale.y;
		if(textPtr->IsFigFont())
		{
			fSX	*=	(float)rcScreen.GetWidth();
			fSY *=	(float)rcScreen.GetHeight();
			static_cast<RenderTextFig*>(textPtr)->SetInsideSize(Size(rcScreen.GetWidth(), rcScreen.GetHeight()), ani->center, pElement->dwTextFormat);
			if(renderMat){
				static_cast<RenderTextFig*>(textPtr)->DrawSprite2D(bEnabled, c, renderMat, nHighlight, m_bEnableDlgMatrix);
				return true;
			}
		}
		if(pMatRot)
		{
			int left			=	prcDest->left+m_dlgX;
			int top				=	prcDest->top+m_dlgY;
			Phantom::matrix4x4 matTransform;
			matrixScaling(&matTransform, fSX, fSY, 1);
			matTransform._41 = -(float)pCenter->x;
			matTransform._42 = -(float)pCenter->y;
			matrixMultiply(&matTransform, &matTransform, pMatRot);
			matTransform._41 = (float)rcScreen.left + pCenter->x;
			matTransform._42 = (float)rcScreen.top + pCenter->y;
			if(textPtr->IsFigFont())
				static_cast<RenderTextFig*>(textPtr)->DrawSprite2D(bEnabled, c, &matTransform, nHighlight, m_bEnableDlgMatrix);
			else
				g_manager.DrawSprite2D(textPtr, rcTexture, pCenter, &c, &matTransform, 0, nHighlight, bEnabled, m_bEnableDlgMatrix);
		}
		else if(textPtr->IsFigFont())
		{
			matrixScaling(&mat, fSX, fSY, 1);
			mat._41 = (float)rcScreen.left + ani->center.x;
			mat._42 = (float)rcScreen.top + ani->center.y;
			static_cast<RenderTextFig*>(textPtr)->DrawSprite2D(bEnabled, c, &mat, nHighlight, m_bEnableDlgMatrix);
		}
		else
		{
			int left			=	rcScreen.left;
			int top				=	rcScreen.top;
			if((int)(ptr->m_scale.x*1000)==1000&&(int)(ptr->m_scale.y*1000)==1000)
			{
				rect_f sr(1, 1, left, top);
				g_manager.DrawSprite2D(textPtr, rcTexture, pCenter, &c, 0, &sr, nHighlight, bEnabled, m_bEnableDlgMatrix);
			}
			else
			{
				Pixel center(ani->center.x + left, ani->center.y + top);
				left = (int)((float)(left - center.x) * ptr->m_scale.x) + center.x;
				top = (int)((float)(top - center.y) * ptr->m_scale.y) + center.y;
				rect_f sr(ptr->m_scale.x, ptr->m_scale.y, left, top);
				g_manager.DrawSprite2D(textPtr, rcTexture, 0, &c, 0, &sr, nHighlight, bEnabled, m_bEnableDlgMatrix);
			}
		}
		return true;
	}

	VOID	UIDialog::OnSetMouseMatrix(Pixel& point)
	{CPUTime(UIDialog);
		if(this->m_bEnableDlgMatrix)
		{
			Phantom::float3 pos[4];
			const float fSize	=	10.0f;
			pos[0].setxyz(-fSize, -fSize, 0.0f);
			pos[1].setxyz(fSize, -fSize, 0.0f);
			pos[2].setxyz(fSize, fSize, 0.0f);
			pos[3].setxyz(-fSize, fSize, 0.0f);
			for(int i=0;i<4;i++)
				pos[i].transform(pos[i], m_dlgMatrix);
			Phantom::float3 orig, dir;
			g_manager.Get3DFrom2D_2DProj(orig, dir, point);
			PIntersectData data;
			Phantom::float3 ret(0, 0, 0);
			data.Pick = &ret;
			Phantom::IntersectTriangle(&data, orig, dir, pos[0], pos[1], pos[2], 0, false);
			Phantom::IntersectTriangle(&data, orig, dir, pos[2], pos[3], pos[0], 0, false);
			ret.transform(ret, this->m_dlgMatrixInv);

			//ret	+=	m_dlgMatrix.GetPosition();
			g_manager.Get2DFrom3D_2DProj(point, ret);
		}
		point.x -= m_dlgX;
		point.y -= m_dlgY;
	}
	BOOL	UIDialog::GetGroupVisibleByID(INT nID)
	{CPUTime(UIDialog);
		for(int i=0;i<m_groups.size();i++)
		{
			if(m_groups[i]->m_nGroupID == nID)
			{
				return m_groups[i]->m_bVisible;
			}
		}
		return false;
	}
	VOID	UIDialog::SetGroupVisibleByID(INT nID, BOOL bVisible)
	{CPUTime(UIDialog);
		for(int i=0;i<m_groups.size();i++)
		{
			if(m_groups[i]->m_nGroupID == nID)
			{
				m_groups[i]->SetVisible(bVisible);
				break;
			}
		}
	}
};
