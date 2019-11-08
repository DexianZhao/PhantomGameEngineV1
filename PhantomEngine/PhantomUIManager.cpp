//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomManager.h"
#include "PhantomFile.h"
#include "LuaManager.h"
#include "APILua.h"

namespace Phantom{
	WayPoints::WayPoints()
	{
		m_bPlaying	=	false;
		m_bPause	=	false;
		m_fSpeed	=	1.0f;
		m_nIndex	=	0;
		m_fLength	=	0;
		m_fLastTime	=	0;
		this->m_bIs2DMode = true;
	}
	WayPoints::~WayPoints()
	{
	}

	VOID					WayPoints::Pause(BOOL b){CPUTime(WayPoints);
		m_bPause = b;
		m_fLastTime	=	g_manager.GetAppTime();
	}
	VOID					WayPoints::Play()
	{CPUTime(WayPoints);
		m_bPlaying = true;
		m_fLength = 0;
		m_nIndex = 0;
		m_fLastTime	=	g_manager.GetAppTime();
		for(int i=1;i<m_points.size();i++)
		{
			WayPointInfo& pFrom	=	m_points[i - 1];
			WayPointInfo& pTo	=	m_points[i];
			pFrom.dir			=	pTo.pos - pFrom.pos;
			pFrom.length		=	pFrom.dir.length();
			if(pFrom.length > 0.01f)
				pFrom.fAngle	=	Degress(getRotateZ(pFrom.dir));
			pFrom.dir.normalize();
		}
	}

	BOOL					WayPoints::FrameMove(float3* ret, float* retRotation)	//返回最新位置
	{CPUTime(WayPoints);
		if(m_bPlaying)
		{
			if(!this->m_bPause)
			{
				int nCount	=	(int)m_points.size() - 1;
				double dTime = g_manager.GetAppTime();
				this->m_fLength	+=	this->m_fSpeed * (dTime - m_fLastTime);
				m_fLastTime = dTime;
				if(m_nIndex < nCount)
				{
					WayPointInfo* pos = &m_points[m_nIndex];
					while(m_fLength > pos->length)
					{
						m_fLength	-= pos->length;
						m_nIndex	++;
						if(m_nIndex >= nCount)
						{
							m_fLength	=	0;
							break;
						}
						pos = &m_points[m_nIndex];
					}
				}
				if(m_nIndex >= nCount)
				{
					m_bPlaying	=	false;
				}
			}
		}
		if(m_nIndex < m_points.size())
		{
			*retRotation	=	m_points[m_nIndex].fAngle;
			*ret	=	m_points[m_nIndex].pos + m_points[m_nIndex].dir * m_fLength;
		}
		else if(m_points.size() > 0)
		{
			*retRotation	=	m_points[m_points.size() - 2].fAngle;
			*ret	=	m_points[m_points.size() - 1].pos;
		}
		return m_bPlaying;
	}

	UIManager*	GetDlgManager()
	{
		return g_manager.m_uiManager;
	}
	UIDialog*					UIManager::FindDialogByID(INT nID)	//根据ID查找对话框
	{CPUTime(UIManager);
		for(int i=0;i< (int)m_dialogs.size();i++)
		{
			if(m_dialogs[i]->m_nDialogID == nID)
				return m_dialogs[i];
		}
		return 0;
	}

	WayPoints*				CreateWayPoints()
	{
		return new WayPoints();
	}

	VOID	UIManager::SetDefaultTexture()
	{CPUTime(UIManager);
		if(this->m_textureMap.size() == 0)
			this->AddTexture("default_init_zip/default.png");
		if(this->m_fontMap.size() == 0)
			AddFont("Arial", 12, 0);
	}

	const TextureRect*	UIManager::GetTextureRect(int textureID, const char* szName)										//返回图片指针
	{CPUTime(UIManager);
		CDynamicArray<TextureRect>* rects	=	GetTextureRectPtr(textureID);
		if(!rects || !szName)
			return 0;
		for(int i=0;i<rects->size();i++)
		{
			if(stricmp(rects->GetPtr()[i].name, szName) == 0)
			{
				return &rects->GetPtr()[i];
			}
		}
		return 0;
	}

	Result UIManager::CreateTexture( unsigned int textureIndex, const char* szDefaultPath, UIDialog *dlg )
	{CPUTime(UIManager);
		Result hr = Result_OK;
		TextureNode2D* pTextureNode = m_textureMap[textureIndex];
		if(!pTextureNode)return Result_Fail;
		if(pTextureNode->bLoadFail)return Result_Fail;
		if(pTextureNode->texturePtr)return Result_OK;

		if( pTextureNode->fileName.t[0] == 0 )
			return Result_OK;
		LFileName buf	=	pTextureNode->fileName.t;
		LFileName ext;
		LFileName drive, file;
		buf.splitpath(&drive, &file, &ext);
		BOOL bExist = true;
		if(!fexist(buf) && szDefaultPath)
			bExist = false;
		if(dlg&&dlg->m_pakdir&&dlg->m_pakdir->fexist(buf))
			bExist = true;
		if(!bExist)
		{
			LFileName def = szDefaultPath;
			def.setIsFile();
			LFileName defDrive;
			def.splitpath(&defDrive, 0, 0);
			LFileName path;
			path	=	defDrive;
			path	+=	file.t;
			path	+=	ext.t;
			buf		=	path;
		}
		TexturePtr tex;
		int filter = pTextureNode->filter;
		filter |= 0x00010000;
		if(!g_manager.LoadTexture(tex, buf,false,filter,g_manager.m_bUseThread, dlg?dlg->m_pakdir:0))
		{
			LogInfo("system->error: UI TexturePtr %s Load Failure", buf.t);
			pTextureNode->bLoadFail	=	true;
			return false;
		}
		pTextureNode->texturePtr	=	tex.GetPtr();
		if(!pTextureNode->texturePtr)
		{
			LogInfo("system->error: UI TexturePtr %s Load Failure", buf.t);
			pTextureNode->bLoadFail	=	true;
			return Result_Fail;
		}
		return Result_OK;
	}

	BOOL			UIManager::OnInputEvent( unsigned int uMsg, const Pixel& pos, unsigned int touchIndex )
	{CPUTime(UIManager);
		BOOL bRet = LocalInputEvent(uMsg, pos, touchIndex);
		if(UIDialog::s_pControlFocus[touchIndex] != 0){
			UIControl* pFocus = UIDialog::s_pControlFocus[touchIndex];
			UIDialog* pFocusDlg = pFocus->m_dialogPtr;
			pFocusDlg->AddRef();
			//
			Pixel mousePoint = pos;
			g_manager.SetUIDialogSize(pFocus->m_nAlignMode==0, pFocusDlg->m_screenSize.cx, pFocusDlg->m_screenSize.cy, &mousePoint, touchIndex);
			pFocusDlg->OnSetMouseMatrix(mousePoint);
			Pixel clientPos;
			clientPos	=	pFocus->ScreenToClient(mousePoint);
			//LogInfo("system->error:%d,%d:%s", pos.x, pos.y, pFocus->GetName());
			//
			BOOL bDoEvent = false;
			if(pFocusDlg->m_pParentControl){
				if(pFocusDlg->OnChildInput(uMsg, pos, touchIndex))
					bDoEvent = true;
			}
			else if(pFocus->m_pChildDialog)
				if(pFocus->m_pChildDialog->OnChildInput(uMsg, pos, touchIndex))
					bDoEvent = true;
			if(!bDoEvent)
				bRet	=	pFocus->onMouseMessage( uMsg, clientPos, touchIndex );
			else
				bRet	=	true;
			//
			pFocusDlg->Release();
		}
		return bRet;
	}
	BOOL			UIManager::LocalInputEvent( unsigned int uMsg, const Pixel& pos, unsigned int touchIndex )
	{CPUTime(UIManager);
		if(m_pModalDlg && m_pModalDlg->m_bVisible)
		{
			UIDialog* temp = m_pModalDlg;
			temp->AddRef();
			if(m_pModalDlg->OnInputEvent(uMsg, pos, touchIndex))
			{
				temp->Release();
				return TRUE;
			}
			temp->Release();
			switch(uMsg)
			{
			case InputEventID_MouseMove:
			case InputEventID_ButtonDown:
				return TRUE;
			}
			return FALSE;
		}
		//
		m_pModalDlg			=	NULL;
		//
		for(int i = (int)m_dialogs.size() - 1;i>=0;i--)
		{
			UIDialog* pDlg	=	m_dialogs[i];
			if(!pDlg->m_bVisible)
				continue;
			if(pDlg->m_bIsToolTipWindow)
			{
				pDlg->AddRef();
				pDlg->OnInputEvent(uMsg, pos, touchIndex);
				pDlg->Release();
			}
		}
		//
		//
		if(uMsg == InputEventID_MouseMove)
		{
//			UIControl*	toolTipCtrl	=	0;
//			for(int i = (int)m_dialogs.size() - 1;i>=0;i--)
//			{
//				UIDialog* pDlg	=	m_dialogs[i];
//				if(!pDlg->m_bVisible || pDlg->m_bIsToolTipWindow)
//					continue;
//				Pixel mousePoint	=	pos;
//				mousePoint.x -= pDlg->m_dlgX;
//				mousePoint.y -= pDlg->m_dlgY;
//				//
//				for(int cc=0;cc<pDlg->m_controls.size();cc++)
//				{
//					UIControl* c	=	pDlg->m_controls[cc];
//					if(!c->m_bVisible)
//						continue;
////					if(c->m_toolTipText.sizeW() == 0)
////						continue;
//					if(c->m_bEnableRotation)
//					{
//						if(!c->isPtIn(c->ScreenToClient(mousePoint)))
//							continue;
//					}
//					else
//					{
//						if(!c->isPtIn(mousePoint))
//							continue;
//					}
//					if(toolTipCtrl)
//						if(toolTipCtrl->m_nLevel > c->m_nLevel)
//							continue;
//					toolTipCtrl	=	c;
//				}
//			}
//			UIDialog::ShowTipText(toolTipCtrl);
		}
		//
		UIDialog* pOld		=	m_pSelectDlg;
		m_pSelectDlg		=	0;
		for(int i = (int)m_dialogs.size() - 1;i>=0;i--)
		{
			UIDialog* pDlg	=	m_dialogs[i];
			if(!pDlg->m_bVisible || pDlg->m_bIsToolTipWindow || pDlg->m_pParentControl)//如果是控件内则不进行自动点击
				continue;
			pDlg->AddRef();
			if(pDlg->OnInputEvent(uMsg, pos, touchIndex))
			{
				switch(uMsg)
				{
				case InputEventID_ButtonDown:
					{
						m_pSelectDlg	=	pDlg;
						m_pSelectDlg->m_nDlgOrder = m_nDlgOrder++;
						this->SetTopMost(0, 0);
					}
					break;
				}
				if(pOld != m_pSelectDlg && pOld)
				{
					if(pOld->m_bHideOnKillFocus)
						pOld->SetVisible(false);
				}
				if(pDlg->Release() == 0)
				{
				}
				return TRUE;
			}
			else
				pDlg->Release();
		}
		return FALSE;
	}

	UIControl*				UIManager::Pick(int x, int y)
	{CPUTime(UIManager);
		if(this->m_pSelectDlg && m_pSelectDlg->m_bVisible)
		{
			UIControl* p	=	m_pSelectDlg->Pick(x, y);
			if(p)
				return p;
		}
		for(int i=0;i< (int)m_dialogs.size();i++)
		{
			if(m_dialogs[i] == m_pSelectDlg || !m_dialogs[i]->m_bVisible || m_dialogs[i]->m_pParentControl)
				continue;
			UIControl* p	=	m_dialogs[i]->Pick( x, y );
			if(p)
				return p;
		}
		return 0;
	}

	INT	compareDlg(const void* v1, const void* v2)
	{
		UIDialog* dlg1 = *(UIDialog**)v1;
		UIDialog* dlg2 = *(UIDialog**)v2;
		if(dlg1 == GetDlgManager()->m_topDialog)
			return 1;
		if(dlg1->m_bIsBackground < dlg2->m_bIsBackground)
			return 1;
		else if(dlg1->m_bIsBackground > dlg2->m_bIsBackground)
			return -1;
		//
		if(dlg1->m_layer < dlg2->m_layer)
			return -1;
		else if(dlg1->m_layer > dlg2->m_layer)
			return 1;
		return 0;
	}

	VOID UIManager::SetTopMost(UIDialog* dlg, BOOL b)
	{CPUTime(UIManager);
		//if(dlg)
		//{
		//	for(int i=0;i<m_dialogs.size();i++)
		//	{
		//		if(m_dialogs[i] == dlg)
		//		{
		//			m_dialogs[i]->m_nDlgOrder	=	this->m_nDlgOrder++;
		//			break;
		//		}
		//	}
		//}
		//if(m_dialogs.size() > 1)
		//	qsort(m_dialogs.m_lists.GetPtr(), m_dialogs.size(), m_dialogs.m_lists.TypeSize, compareDlg);
	}

	int UIManager::AddFont( const char* strFaceName, int height, int weight, char italic, char underline )
	{CPUTime(UIManager);
		if(!strFaceName)
			strFaceName	=	"Arial";
		for( int i=0; i < (int)m_fontMap.size(); i++ )
		{
			FontNode2D* pFontNode = m_fontMap[i];
			if( pFontNode->nFontHeight == height && pFontNode->nWeight == weight && pFontNode->bItalic == italic && pFontNode->bUnderline == underline && 
				0 == stricmp( pFontNode->fontName, strFaceName  ))
				return i;
		}
		FontNode2D* pNewFontNode = new FontNode2D();
		if( pNewFontNode == NULL )
			return -1;
		pNewFontNode->fontName		=	strFaceName;
		pNewFontNode->nFontHeight	=	height;
		pNewFontNode->nWeight		=	weight;
		pNewFontNode->bItalic		=	italic;
		pNewFontNode->bUnderline	=	underline;
		m_fontMap.push_back( pNewFontNode );
		int fontIndex = (int)m_fontMap.size() - 1;
		return fontIndex;
	}

	FontNode2D*     		UIManager::GetFontNode( int iIndex )
	{CPUTime(UIManager);
		if(iIndex < 0 || iIndex >= m_fontMap.size())
			return m_fontMap[ 0 ];
		FontNode2D* pNode	=	m_fontMap[ iIndex ];
		if(!pNode)return m_fontMap[ 0 ];
		if(g_manager.GetDefaultUnicodeFont())
			pNode->fontPtr = g_manager.GetDefaultUnicodeFont();
		else
			pNode->fontPtr = g_manager.GetDefaultENFont();
		//if(pNode->fontPtr.t == 0)
		//{
		//	char szFontFile[256];
		//	if(fexist(pNode->fontName.t))
		//		strcpy(szFontFile, pNode->fontName.t);
		//	else
		//		sprintf(szFontFile, "default_init_zip/fonts/%s.fig", pNode->fontName.t);
		//	FontPtr f;
		//	if(g_manager.LoadFont(f, szFontFile))
		//		pNode->fontPtr	=	f.GetPtr();
		//	else
		//		pNode->fontPtr	=	g_manager.GetDefaultUnicodeFont();
		//}
		return m_fontMap[ iIndex ];
	}

	CDynamicArray<TextureRect>*	UIManager::GetTextureRectPtr(int iIndex)										//返回图片指针
	{CPUTime(UIManager);
		if(iIndex < 0 || iIndex >= (int)m_textureMap.size())
			return 0;
		TextureNode2D* pNode	=	m_textureMap[ iIndex ];
		if(!pNode || !pNode->texturePtr)
			return 0;
		return &pNode->texturePtr->m_rects;//rects;
	}
	TextureNode2D*  UIManager::GetTextureNode( int iIndex )
	{CPUTime(UIManager);
		if(iIndex < 0 || iIndex >= (int)m_textureMap.size())
			return 0;
		return m_textureMap[ iIndex ];
	}
	Texture*  		UIManager::GetTextureNode( int iIndex , UIDialog* dlg  )
	{CPUTime(UIManager);
		const char* szPath	=	0;
		if(dlg)
			szPath	=	dlg->m_strFile;
		if(iIndex < 0 || iIndex >= (int)m_textureMap.size())
			return 0;
		if(!m_textureMap[ iIndex ]->texturePtr.t)
			CreateTexture( iIndex, szPath, dlg );
		return m_textureMap[ iIndex ]->texturePtr;
	}

	int UIManager::AddTexture( const char* strFilename, int filter )
	{CPUTime(UIManager);
		FileName f = strFilename;
		f.setIsFile();
		for( int i=0; i < (int)m_textureMap.size(); i++ )
		{
			TextureNode2D* pTextureNode = m_textureMap[i];
			if(pTextureNode->fileName == f)
			{
				pTextureNode->filter = filter;
				pTextureNode->bLoadFail	=	false;
				pTextureNode->nRef++;
				return i;
			}
		}
		TextureNode2D* pTextureNode = new TextureNode2D();
		if( pTextureNode == NULL )
			return -1;
		pTextureNode->fileName	=	f;
		pTextureNode->nRef	=	1;
		pTextureNode->filter = filter;
		m_textureMap.push_back( pTextureNode );
		int textureIndex = (int)m_textureMap.size() - 1;
		//CreateTexture( textureIndex, 0 );
		return textureIndex;
	}
	int UIManager::AddTexture( const char* strFilename, UIDialog* dlg, int filter)
	{CPUTime(UIManager);
		FileName f = strFilename;
		f.setIsFile();
		if(fexist(f))
			return AddTexture(f, filter);
		FileName path, file, ext;
		dlg->m_strFile.splitpath(&path, 0, 0);
		f.splitpath(0, &file, &ext);
		path	+=	file;
		path	+=	ext;
		if(fexist(path))
			return AddTexture(path, filter);
		return AddTexture(f, filter);
	}

	int UIManager::AddTexture( const short_t* strFilename, UIDialog* dlg, int filter)
	{CPUTime(UIManager);
		char szFile[512];
		WToUtf8(strFilename, szFile, 512);
		return AddTexture(szFile, dlg, filter);
	}

	//--------------------------------------------------------------------------------------
	int UIManager::AddTexture( const short_t* strFilename, int filter )
	{CPUTime(UIManager);
		char szFile[512];
		WToUtf8(strFilename, szFile, 512);
		return AddTexture(szFile, filter);
	}

	int UIManager::AddTextureRef( int index )
	{CPUTime(UIManager);
		if(index < 0 || index >= (int)m_textureMap.size())
			return 0;
		return m_textureMap[ index ]->nRef ++;
	}

	int UIManager::DelTexture( int iIndex )
	{CPUTime(UIManager);
		if(iIndex <= 0 || iIndex >= (int)m_textureMap.size())
			return 0;
		m_textureMap[ iIndex ]->nRef --;
		if(m_textureMap[ iIndex ]->nRef <= 0)
		{
			m_textureMap[ iIndex ]->nRef	=	0;
			m_textureMap[ iIndex ]->texturePtr	=	0;
			//m_textureMap[ iIndex ]->rects.clear();
		}
		return 0;
	}

	UIManager::UIManager()
	{
		m_callback			=	0;
		m_fToolTipTime		=	100000.0f;
		m_nDlgOrder			=	1;
		m_toolTipTextCB		=	0;
		m_pModalDlg			=	0;
		this->m_pSelectDlg	=	0;
		m_allParticles			=	0;
		m_topParticle			=	0;
		m_particleAssignIndex	=	0;
		m_particleCount			=	0;
		m_bShowUI			=	true;
		m_bResortDialogs	=	true;
		m_topDialog			=	0;
		//
		SetupParticles(5000);
	}

	//--------------------------------------------------------------------------------------
	UIManager::~UIManager()
	{
		safe_delete(m_callback);
	}
	VOID						UIManager::Destroy()
	{CPUTime(UIManager);
		if(m_allParticles)
			free(m_allParticles);
		m_allParticles			=	0;
		m_topParticle			=	0;
		int i;
		for( i=0; i < (int)m_fontMap.size(); i++ )
		{
			FontNode2D* pFontNode = m_fontMap[ i ];
			pFontNode->fontPtr	=	0;
			safe_delete( pFontNode );
		}
		m_fontMap.clear();
		for( i=0; i < (int)m_textureMap.size(); i++ )
		{
			TextureNode2D* pTextureNode = m_textureMap[ i ];
			pTextureNode->texturePtr	=	0;
			safe_delete( pTextureNode );
		}
		m_textureMap.clear();
	}
	VOID						UIManager::SetupParticles(INT nCount)	//设置最大粒子数量
	{CPUTime(UIManager);
		if(m_allParticles)
			return;
		m_particleCount			=	nCount;
		m_allParticles			=	(Particle2D*)malloc_s(sizeof(Particle2D) * m_particleCount);
		memset(m_allParticles, 0, sizeof(Particle2D) * m_particleCount);
		this->m_topParticle		=	0;
		m_particleAssignIndex	=	0;
	}

	Particle2D*					UIManager::Alloc()			//分配粒子
	{
		if(m_topParticle)
		{
			Particle2D* pRet	=	m_topParticle;
			m_topParticle			=	m_topParticle->next;
			pRet->next				=	0;
			return pRet;
		}
		if(this->m_particleAssignIndex < this->m_particleCount)
		{
			Particle2D* pRet	=	&m_allParticles[m_particleAssignIndex];
			m_particleAssignIndex	++;
			pRet->next				=	0;
			return pRet;
		}
		return NULL;
	}
	VOID						UIManager::OnEditClose(int id, BOOL bIsCancel, const char* text)
	{
		for(int i=0;i<this->m_dialogs.size();i++){
			UIControl *c = m_dialogs[i]->FindControlByID(id);
			if(c){
				c->OnEditClose(bIsCancel, text);
			}
		}
	}
	VOID						UIManager::Free(Particle2D* p)	//释放粒子
	{CPUTime(UIManager);
		if(p->next)return;
		Particle2D* pLocal			=	(Particle2D*)p;
		pLocal->next				=	m_topParticle;
		m_topParticle				=	pLocal;
	}

	BOOL UIManager::OnResize()
	{CPUTime(UIManager);
		for(int i=0;i<this->m_dialogs.size();i++)
			m_dialogs[i]->AlignDialog();
		return true;
	}

	BOOL							UIManager::CreateDlg(Dialog& ret, classT<UIDialog, classMgr<UIDialog> >* myClass)
	{CPUTime(UIManager);
		if(this->m_textureMap.size() == 0)
			SetDefaultTexture();
		//
		int id;
		if(!myClass)
			myClass	=	new classT<UIDialog, classMgr<UIDialog> >();
		classT<UIDialog, classMgr<UIDialog> >* dlg		=	m_dialogs.addNew(&id, myClass);
		dlg->m_nDlgOrder	=	m_nDlgOrder++;
		this->SetTopMost(0, 0);
		ret.set(m_dialogs, id);
		return true;
	}
	BOOL							UIManager::LoadDialog(Dialog& ret, const char* szFile, ImportResult* result, classT<UIDialog, classMgr<UIDialog> >* myClass, BOOL bCreateNew)
	{CPUTime(UIManager);
		if(this->m_textureMap.size() == 0)
			SetDefaultTexture();
		//
		FileName file		=	szFile;
		file.setIsFile();
		if(!bCreateNew)
		{
			for(int i=0;i<m_dialogs.size();i++)
			{
				if(m_dialogs[i]->m_strFile == file)
				{
					ret.set(m_dialogs, m_dialogs[i]->GetID());
					if(m_dialogs[i]->m_bIsHold){
						m_dialogs[i]->OnHold(false);
						return 1;
					}
					return 2;
				}
			}
		}
		Dialog temp;
		temp = ret;
		int id = 0;
		if(!myClass)
			myClass	=	new classT<UIDialog, classMgr<UIDialog> >();
		classT<UIDialog, classMgr<UIDialog> >* dlg		=	m_dialogs.addNew(&id, myClass);
		if(!dlg->LoadDialog(file, result))
		{
			temp = 0;
			safe_release(dlg);
			return 0;
		}
		//加载所有的资源
		for(int i=0;i<dlg->GetControlCount();i++)
			dlg->GetControlPtr(i)->LoadAllResource();
		dlg->m_nDlgOrder = m_nDlgOrder++;
		this->SetTopMost(0, 0);
		ret.set(m_dialogs, id);
		temp = 0;
		this->m_bResortDialogs = true;
		return 1;
	}
	void					UIManager::OnDeleteDlg(UIDialog* dlg)
	{CPUTime(UIManager);
		if(m_pModalDlg	== dlg)
			m_pModalDlg		=	0;
		if(dlg == m_pSelectDlg)
			m_pSelectDlg	=	0;
		UIDialog::ShowTipText(NULL);
		for(int i=0;i<this->m_dialogs.size();i++)
		{
			if(m_dialogs[i] == dlg)
			{
				m_dialogs.m_lists.eraseByIndex(i);
				break;
			}
		}
	}

	INT	compareAvatar(const void* v1, const void* v2)
	{
		UIDialogRT* dlg1 = *(UIDialogRT**)v1;
		UIDialogRT* dlg2 = *(UIDialogRT**)v2;
		if(dlg1->m_layer < dlg2->m_layer)
			return -1;
		else if(dlg1->m_layer > dlg2->m_layer)
			return 1;
		return 0;
	}
	INT	compareBodyPart(const void* v1, const void* v2)
	{
		UIControlRT* dlg1 = *(UIControlRT**)v1;
		UIControlRT* dlg2 = *(UIControlRT**)v2;
		if(dlg1->m_layer < dlg2->m_layer)
			return -1;
		else if(dlg1->m_layer > dlg2->m_layer)
			return 1;
		return 0;
	}

	void					UIManager::Render(float fTime, float fElapsedTime, BOOL bBackground)
	{CPUTime(UIManager);
		if(!m_bShowUI)
			return;
		if(m_dialogs.size() > 1 && m_bResortDialogs)
		{
			qsort(m_dialogs.m_lists.GetPtr(), m_dialogs.size(), m_dialogs.m_lists.TypeSize, compareDlg);
			m_bResortDialogs = false;
		}
		g_manager->SetEnableZ(false);
		if(bBackground)
		{
			for(int i=0;i< (int)m_dialogs.size();i++)
			{
				if(!m_dialogs[i]->m_bVisible)
					continue;
				if(m_dialogs[i]->m_bIsToolTipWindow)
					continue;
				if(this->m_pModalDlg == m_dialogs[i])
					continue;
				m_dialogs[i]->OnRender( fElapsedTime, bBackground );
			}
			g_manager->SetEnableZ(true);
			return;
		}
		if(this->m_requestSortAvatar)
		{
			qsort(m_avatars.GetPtr(), m_avatars.size(), sizeof(UIDialogRT*), compareAvatar);
			m_requestSortAvatar = false;
		}
		g_manager->SetAlphaMode(Phantom::AlphaMode_Blend);
		g_manager->SetEnableZ(false);
		g_manager->SetBlendMode(BlendMode_SRCALPHA,BlendMode_INVSRCALPHA);
		g_manager->SetCullVisible(CullMode_CW);
		g_manager.SetWorldDlgMatrix2D(0);
		g_manager.SetUIDialogSize(true, g_manager.GetBufferWidth(), g_manager.GetBufferHeight(), 0);
		for(int i=0;i<m_avatars.size();i++){
			m_avatars[i]->Render();
		}
		//if(m_dialogs.size() > 1)
		//	qsort(m_dialogs.m_lists.GetPtr(), m_dialogs.m_lists.size(), m_dialogs.m_lists.TypeSize, compareDlg);
		UIDialog* pToolTip	=	0;
		UIDialog* pModal	=	0;
		for(int i=0;i< (int)m_dialogs.size();i++)
		{
			if(!m_dialogs[i]->m_bVisible||m_dialogs[i]->m_bIsChildRender)
				continue;
			if(m_dialogs[i]->m_bIsToolTipWindow)
			{
				pToolTip	=	m_dialogs[i];
				continue;
			}
			if(this->m_pModalDlg == m_dialogs[i])
			{
				pModal		=	m_pModalDlg;
				continue;
			}
			if(m_dialogs[i]->m_pParentControl)//如果在控件内渲染则不进行自动渲染
				continue;
			m_dialogs[i]->OnRender( fElapsedTime );
		}
		if(pModal && pModal->m_bVisible)
			pModal->OnRender( fElapsedTime );
		if(pToolTip && pToolTip->m_bVisible)
			pToolTip->OnRender( fElapsedTime );
		g_manager->SetEnableZ(true);
	}
	VOID	UIManager::PushAvatar(UIDialogRT* a)
	{
		a->m_ptr->m_nAvatarRef++;
		m_avatars.push_back(a);
	}
	VOID	UIManager::PopAvatar(UIDialogRT* a)
	{
		for(int i=0;i<m_avatars.size();i++)
		{
			if(m_avatars[i] == a)
			{
				a->m_ptr->m_nAvatarRef--;
				if(a->m_ptr->m_nAvatarRef<=0)
				{
					for(int j=0;j<m_dlgAvatars.size();j++)
					{
						if(m_dlgAvatars[j] == a->m_ptr)
						{
							safe_release(m_dlgAvatars[j]);
							m_dlgAvatars.eraseByIndex(j);
							break;
						}
					}
				}
				m_avatars.eraseByIndex(i);
				break;
			}
		}
	}

	int		compareDlgControls(const void* p1, const void* p2)
	{
		UIControl* c1 = *(UIControl**)p1;
		UIControl* c2 = *(UIControl**)p2;
		if(c1->GetLayer() < c2->GetLayer())
			return -1;
		else if(c1->GetLayer() > c2->GetLayer())
			return 1;
		return 0;
	}
	UIDialog*						UIManager::LoadAvatar(const char* szFileName)
	{CPUTime(UIManager);
		if(!szFileName)return 0;
		FileName file = szFileName;
		file.setIsFile();
		for(int i=0;i<m_dlgAvatars.size();i++)
		{
			if(m_dlgAvatars[i]->m_strFile == file)
			{
				return m_dlgAvatars[i];
			}
		}
		UIDialog* dlg = new UIDialog();
		ImportResult* result = 0;
		if(!dlg->LoadDialog(file, result))
		{
			safe_release(dlg);
			return 0;
		}
		//加载所有的资源
		for(int i=0;i<dlg->GetControlCount();i++)
			dlg->GetControlPtr(i)->LoadAllResource();
		if(dlg->m_controls.size() > 1)
			qsort(dlg->m_controls.GetPtr(), dlg->m_controls.size(), sizeof(UIControl*), compareDlgControls);
		m_dlgAvatars.push_back(dlg);
		return dlg;
	}
	void	UIManager::OnFrameMove( double fTime, float fElapsedTime )
	{CPUTime(UIManager);
		if(m_toolTipInfo.id > 0)
		{
			m_toolTipInfo.fElapsedTime	-=	fElapsedTime;
			if(m_toolTipInfo.fElapsedTime < 0)
			{
				m_toolTipInfo.id		=	0;
				m_toolTipInfo.fElapsedTime	=	0;
				m_toolTipInfo.type		=	UIPopupType_LEFT;
				if(m_toolTipTextCB)
					m_toolTipTextCB->ShowToolTip(0, 0, m_toolTipInfo.rect, UIPopupType_LEFT);
			}
		}
	}

	VOID	UIManager::SetToolTip(ToolTipData* t, const short_t* text, const Rect& rcInWindow, UIPopupType type)
	{CPUTime(UIManager);
		if(m_toolTipTextCB)
		{
			if(t && text && text[0] && t->c)
			{
				if(m_toolTipInfo.id != (int)t->c->GetID())
				{
					m_toolTipInfo.id	=	(int)t->c->GetID();
					m_toolTipInfo.rect	=	rcInWindow;
					m_toolTipInfo.fElapsedTime	=	this->m_fToolTipTime;
					m_toolTipTextCB->ShowToolTip(text, t, m_toolTipInfo.rect, type);
				}
			}
			else if(m_toolTipInfo.id > 0)
			{
				m_toolTipInfo.id		=	0;
				m_toolTipInfo.fElapsedTime	=	0;
				m_toolTipInfo.type		=	UIPopupType_LEFT;
				m_toolTipTextCB->ShowToolTip(0, 0, m_toolTipInfo.rect, UIPopupType_LEFT);
			}
		}
	}
	dlg_base::dlg_base()
	{
		GetDlgManager()->AddDlgBase(this);
	}
	dlg_base::~dlg_base()
	{
		dlg = 0;
		GetDlgManager()->RemoveDlgBase(this);
	}
	VOID		UIManager::AddDlgBase(dlg_base* b)
	{CPUTime(UIManager);
		m_dlgBases.push_back(b);
	}
	VOID		UIManager::DestroyAllDialogs()
	{CPUTime(UIManager);
		std::vector<UIDialog*> temps;
		for(int i=0;i<m_dialogs.size();i++)
		{
			temps.push_back(m_dialogs[i]);
		}
		for(int i=0;i<temps.size();i++)
			delete temps[i];
	}
	VOID		UIManager::RemoveDlgBase(dlg_base* b)
	{CPUTime(UIManager);
		for(int i=0;i<m_dlgBases.size();i++)
		{
			if(m_dlgBases[i] == b)
			{
				m_dlgBases.eraseByIndex(i);
				break;
			}
		}
	}



	//函数列表
	UIControlRT::UIControlRT(UIDialogRT* av, UIControl* c)
	{
		m_ptr			=	c;
		m_avatar		=	av;
		m_textureID		=	c->GetElementPtr(0)->GetTextureID();
		m_fontID		=	c->GetElementPtr(0)->GetFontID();
		m_textureRect	=	c->GetElementPtr(0)->textureRect;
		Rect rc			=	c->m_windowRect;
		m_center		=	c->m_rotCenter;
		m_renderSize.Set(rc.GetWidth(), rc.GetHeight());
		m_position.Set(rc.left+m_center.x, rc.top+m_center.y);
		m_fScale	=	1;
		//
		m_bUVPlaying = m_bActionPlaying = false;
		//
		m_textureOffset.Set(0,0);
		m_textureOrig.Set(0,0);
		m_unitAngle		=	0;
		m_uvIndex		=	-1;//当前UV动画播放索引
		m_uvCurrentTime	=	0;//当前UV动画播放时间
		m_uvspeed		=	c->GetElementPtr(0)->m_fUVPlaySpeed;
		for(int i=0;i<m_ptr->GetElementPtr(0)->m_uvAnimations.size();i++)
		{
			if(m_ptr->GetElementPtr(0)->m_uvAnimations[i]->m_bIsDefault)
			{
				m_uvIndex = i;
				this->m_uvspeed = m_ptr->GetElementPtr(0)->m_uvAnimations[i]->m_fPlaySpeed * m_ptr->GetElementPtr(0)->m_fUVPlaySpeed;
				break;
			}
		}
		m_actionForWaitWave	=	0;
		m_fProgressVal	=	0;//精确到10000的数值
		m_bEnabled		=	c->GetEnabled();
		m_bVisible		=	c->GetVisible();
		m_layer			=	c->m_nLevel;//显示层级
		m_textureColor	=	c->m_textureColor;
		m_fRotation		=	c->m_rotation;//转动值
		m_actionIndex	=	-1;//动作索引，非UV动画，是独立的控件动画
		m_actionNextType	=	-1;//下一个动作索引
		m_actionSpeed		=	c->m_fPlaySpeed;//当前动作播放速度
		m_fRot2DAngle		=	0;//2D转动值，不是控件转动值
		m_actionForWait	=	c->m_fSleepTime;//播放动作前等待时间计算，只用于计算
		//动态分配内存变量
		//m_eventClick	=	0;//按下事件,如果不初始化则不分配内存
		//m_customText	=	0;//自定义文本，只有在设置的时候才会被初始化内存
		//m_customRender	=	0;//自动分化渲染，分成大面积格子，NxN格格子，自动分配UV值等
		m_textPtr	=	0;
		m_firstParticle	=	0;//第一个粒子指针，如果是粒子控件则需要这个,由UIManager控制总体粒子数量
		this->SetPos(m_position);
	}
	UIControlRT::~UIControlRT()
	{
		if(m_textPtr)
			g_manager.RemoveText(m_textPtr);
		m_textPtr = 0;
		//safe_delete(m_eventClick);
		//safe_delete(m_customText);
		//safe_release(m_customRender);
		safe_delete(m_firstParticle);
	}
	VOID				UIControlRT::SetText(const char* t)
	{CPUTime(UIControlRT);
		int len = strlen(t);
		m_textVal.SetArrayCount(len);
		memcpy(m_textVal.GetPtr(), t, len);
	}
	void				UIControlRT::SetTextureColor(const color4& c)
	{CPUTime(UIControlRT);
		this->m_textureColor = c;
	}
	void				UIControlRT::SetOpacity(float f)
	{CPUTime(UIControlRT);
		m_textureColor.a = f;
	}
	void				UIControlRT::SetTextureID(int id)
	{CPUTime(UIControlRT);
		this->m_textureID = id;
	}
	VOID				UIControlRT::SetFontID(int id)
	{CPUTime(UIControlRT);
		this->m_fontID = id;
	}
	VOID				UIControlRT::SetTextureRect(const Rect& rc)
	{CPUTime(UIControlRT);
		this->m_textureRect = rc;
		m_textureOffset.Set(0,0);
		m_textureOrig.Set(m_textureRect.GetWidth(),m_textureRect.GetHeight());
		RecalcPosUV();
	}
	VOID				UIControlRT::SetTextureMode(const char* szMode)
	{CPUTime(UIControlRT);
		Name name = szMode;
		name.setIsFile();
		const TextureRect* rc = GetDlgManager()->GetTextureRect(this->m_textureID, name.t);
		if(rc)
		{
			m_textureRect	=	rc->rc;
			m_textureOffset =	rc->off;
			m_textureOrig	=	rc->orig;
		}
		RecalcPosUV();
	}
	const char*			UIControlRT::GetUVAnimation()
	{CPUTime(UIControlRT);
		if(this->m_uvIndex < 0)
			return "";
		UVAnimation* uv = m_ptr->GetElementPtr(0)->GetUVAnimationPtr(this->m_uvIndex);
		if(!uv)return "";
		return uv->GetName();
	}
	void				UIControlRT::SetUVAnimation(const char* ani)
	{CPUTime(UIControlRT);
		m_bUVPlaying = true;
		UIElement* ele	=	m_ptr->GetElementPtr(0);
		for(int i=0;i<ele->m_uvAnimations.size();i++)
		{
			if(ele->m_uvAnimations[i]->m_name == ani)
			{
				m_uvIndex = i;
				this->m_uvCurrentTime = 0;
				this->m_uvspeed = ele->m_uvAnimations[i]->m_fPlaySpeed * ele->m_fUVPlaySpeed;
				return;
			}
		}
		m_bUVPlaying	=	false;
		m_uvIndex = -1;
		RecalcPosUV();
	}
	ActionType			UIControlRT::GetUVAnimationType()
	{CPUTime(UIControlRT);
		if(this->m_uvIndex < 0)
			return ActionType_None;
		UVAnimation* uv = m_ptr->GetElementPtr(0)->GetUVAnimationPtr(this->m_uvIndex);
		if(!uv)return ActionType_None;
		return uv->GetActionType();
	}
	VOID				UIControlRT::SetUVAnimationType(ActionType t)
	{CPUTime(UIControlRT);
		m_bUVPlaying = true;
		UIElement* ele	=	m_ptr->GetElementPtr(0);
		for(int i=0;i<ele->m_uvAnimations.size();i++)
		{
			if(ele->m_uvAnimations[i]->m_nActionType == t)
			{
				m_uvIndex = i;
				this->m_uvCurrentTime = 0;
				this->m_uvspeed = ele->m_uvAnimations[i]->m_fPlaySpeed * ele->m_fUVPlaySpeed;
				return;
			}
		}
		m_bUVPlaying	=	false;
		m_uvIndex = -1;
		RecalcPosUV();
	}
	VOID				UIControlRT::SetUVSpeed(float f)
	{CPUTime(UIControlRT);
		m_uvspeed	=	f;
	}
	VOID				UIControlRT::SetCurrentTime(float f)
	{CPUTime(UIControlRT);
		this->m_uvCurrentTime = f;
		this->m_actionCurrentTime = f;
	}
	float				UIControlRT::GetTimeLength()
	{CPUTime(UIControlRT);
		if(this->m_actionIndex<0||this->m_actionIndex>=m_ptr->m_trackList.size()==0)return 0;
		return (float)m_ptr->m_trackList[m_actionIndex]->m_nFrameCount / 20.0f;

	}
	VOID				UIControlRT::SetProgressValue(float f)
	{CPUTime(UIControlRT);
		this->m_fProgressVal = f;
	}
	VOID				UIControlRT::SetLayer(int layer)
	{CPUTime(UIControlRT);
		this->m_layer = layer;
		this->m_avatar->m_requestSort = true;
	}
	//VOID				UIControlRT::SetText(const char* t)
	//{
	//	if(!this->m_customText)
	//		this->m_customText = new UICustomText();
	//	m_customText->text.setA(t);
	//}
	ActionType			UIControlRT::GetNextActionType()
	{CPUTime(UIControlRT);
		return (ActionType)this->m_actionNextType;
	}
	VOID				UIControlRT::SetNextActionType(ActionType t)
	{CPUTime(UIControlRT);
		this->m_actionNextType = t;
	}
	const char*			UIControlRT::GetAction()
	{CPUTime(UIControlRT);
		if(this->m_actionIndex < 0 || m_actionIndex >= m_ptr->m_trackList.size())
			return "";
		UIAnimationTrack* at = m_ptr->m_trackList[m_actionIndex];
		return at->GetName();
	}
	VOID				UIControlRT::SetAction(const char* t)
	{CPUTime(UIControlRT);
		m_bActionPlaying	=	true;
		for(int i=0;i<m_ptr->m_trackList.size();i++)
		{
			if(m_ptr->m_trackList[i]->m_name == t)
			{
				this->m_actionIndex = i;
				OnInitAction();
				return;
			}
		}
		m_bActionPlaying	=	false;
		m_actionIndex = -1;
		RecalcPosUV();
		OnInitAction();
	}
	ActionType			UIControlRT::GetActionType()
	{CPUTime(UIControlRT);
		if(this->m_actionIndex < 0 || m_actionIndex >= m_ptr->m_trackList.size())
			return ActionType_None;
		UIAnimationTrack* at = m_ptr->m_trackList[m_actionIndex];
		return at->m_nActionType;
	}
	VOID				UIControlRT::SetActionType(ActionType t)
	{CPUTime(UIControlRT);
		m_bActionPlaying = true;
		for(int i=0;i<m_ptr->m_trackList.size();i++)
		{
			if(m_ptr->m_trackList[i]->m_nActionType == t)
			{
				this->m_actionIndex = i;
				OnInitAction();
				return;
			}
		}
		m_bActionPlaying	=	false;
		m_actionIndex = -1;
		RecalcPosUV();
		OnInitAction();
	}
	VOID				UIControlRT::SetActionSpeed(float f)
	{CPUTime(UIControlRT);
		this->m_actionSpeed = f;
	}
	VOID				UIControlRT::SetRot2DAngle(float r)
	{CPUTime(UIControlRT);
		m_fRot2DAngle		=	(int)r % 360;
		if(m_fRot2DAngle < 0.0f)
			m_fRot2DAngle	+=	360.0f;
		//int nAngle		=	0;
		//Unit2DAngle* dir = 0;
		//if(m_ptr->m_bIs4Angle)
		//{
		//	nAngle		=	(int)((m_fRot2DAngle + 45.0f) / 90.0f) % 4;
		//	static Unit2DAngle s_dir[4] = {Unit2DAngle_Bottom, Unit2DAngle_Left, Unit2DAngle_Top, Unit2DAngle_Right};
		//	dir = s_dir;
		//}
		//else
		//{
		//	nAngle = (int)((m_fRot2DAngle + 22.5f) / 45.0f) % Unit2DAngle_Count;
		//	static Unit2DAngle s_dir[Unit2DAngle_Count] = {Unit2DAngle_Bottom, Unit2DAngle_LeftBottom, Unit2DAngle_Left, Unit2DAngle_LeftTop, Unit2DAngle_Top, Unit2DAngle_RightTop, Unit2DAngle_Right, Unit2DAngle_RightBottom};
		//	dir = s_dir;
		//}
		//if(m_unitAngle != dir[nAngle])
		//	m_unitAngle		=	dir[nAngle];
		RecalcUV();
	}
	VOID				UIControlRT::SetRot2DDir(const Pixel& dir)
	{CPUTime(UIControlRT);
		SetRot2DAngle(Degress(getRotateZ(float3(dir.x, dir.y, 0))));
	}
	VOID				UIControlRT::SetScale(float f)
	{CPUTime(UIControlRT);
		this->m_fScale	=	f;
		RecalcPosUV();
	}
	VOID				UIControlRT::SetTextureFile(const char* file)
	{CPUTime(UIControlRT);
		this->m_ptr->GetElementPtr(0)->SetTexture(file);
	}
	//VOID				UIControlRT::BindClick(const char* luaFunc)
	//{
	//	if(!this->m_eventClick)
	//		m_eventClick = new UIEvent();
	//	m_eventClick->func	=	luaFunc;
	//	m_eventClick->type	=	0;
	//}
	VOID				UIControlRT::SetPos(const Pixel& p)
	{CPUTime(UIControlRT);
		this->m_position = p;
		RecalcPosUV();
	}
	VOID				UIControlRT::StopEmit()
	{CPUTime(UIControlRT);
		//
	}
	VOID				UIControlRT::SetVisible(BOOL b)
	{CPUTime(UIControlRT);
		this->m_bVisible	=	b;
	}
	VOID				UIControlRT::SetEnabled(BOOL b)
	{CPUTime(UIControlRT);
		this->m_bEnabled	=	b;
	}
	VOID				UIControlRT::RecalcPosUV()
	{CPUTime(UIControlRT);
	}
	VOID				UIControlRT::RecalcUV()
	{CPUTime(UIControlRT);
	}
	VOID				UIControlRT::OnInitAction()
	{CPUTime(UIControlRT);
		m_textureColor = m_ptr->m_textureColor;
		if(m_actionIndex<0)return;
		UIAnimationTrack* track = m_ptr->m_trackList[m_actionIndex];
		this->m_actionForWait = track->m_fSleepTime;
		this->m_actionForWaitWave = track->m_fWaveSleepTime;
		this->m_actionCurrentTime = 0;
		this->m_actionSpeed = track->m_fSpeed;
		this->m_actionNextType = track->m_nNextAnimationID;
	}
	BOOL				UIControlRT::UpdateAction(PixelW& pos, PixelW& center, SizeW& size, color4& color, float& fRotation)
	{CPUTime(UIControlRT);
		if(m_actionIndex<0)return true;
		double fElapsedTime = g_manager.GetElapsedTime();
		UIAnimationTrack* track = m_ptr->m_trackList[m_actionIndex];
		m_actionForWait -= fElapsedTime;
		//计算插值
		if(m_actionForWait<=0 || !track->m_bWaitTimeEnable){
			m_actionCurrentTime += fElapsedTime * this->m_actionSpeed * this->m_avatar->m_actionSpeed;
			if(track->GetAnimationPtr(ControlTrackType_TextureColorR)->IsEnabled()){
				color.r = track->GetAnimationFloat(ControlTrackType_TextureColorR, m_actionCurrentTime);
				color.g = track->GetAnimationFloat(ControlTrackType_TextureColorG, m_actionCurrentTime);
				color.b = track->GetAnimationFloat(ControlTrackType_TextureColorB, m_actionCurrentTime);
			}
			if(track->GetAnimationPtr(ControlTrackType_TextureColorA)->IsEnabled())
				color.a = track->GetAnimationFloat(ControlTrackType_TextureColorA, m_actionCurrentTime);

			if(track->GetAnimationPtr(ControlTrackType_RectX)->IsEnabled())
				pos.x += track->GetAnimationFloat(ControlTrackType_RectX, m_actionCurrentTime);
			if(track->GetAnimationPtr(ControlTrackType_RectX)->IsEnabled())
				pos.y += track->GetAnimationFloat(ControlTrackType_RectY, m_actionCurrentTime);
			if(track->GetAnimationPtr(ControlTrackType_RectW)->IsEnabled())
				size.cx += track->GetAnimationFloat(ControlTrackType_RectW, m_actionCurrentTime);
			if(track->GetAnimationPtr(ControlTrackType_RectH)->IsEnabled())
				size.cy += track->GetAnimationFloat(ControlTrackType_RectH, m_actionCurrentTime);
			//
			if(track->GetAnimationPtr(ControlTrackType_CenterX)->IsEnabled()){
				center.x	+=	track->GetAnimationFloat(ControlTrackType_CenterX, m_actionCurrentTime);
				center.y	+=	track->GetAnimationFloat(ControlTrackType_CenterY, m_actionCurrentTime);
			}
			if(track->GetAnimationPtr(ControlTrackType_Rotation)->IsEnabled())
				fRotation		+=	track->GetAnimationFloat(ControlTrackType_Rotation, m_actionCurrentTime);
			//
			if(m_actionCurrentTime >= (float)track->m_nFrameCount)
			{
				this->m_bActionPlaying = false;
				if(this->m_actionNextType != ActionType_None){
					this->SetActionType((ActionType)m_actionNextType);
				}
			}
		}
		else
		{
			if(track->m_bHideOnSleep)
				return false;
		}
		//播放音效
		m_actionForWaitWave -= fElapsedTime;
		if(m_actionForWaitWave<=0){
			if(track->m_soundFile.t[0])
			{
				Global_playsnd(track->m_soundFile.t, track->m_soundVolume, 0, track->m_fSoundSpeed);
			}
		}
		return true;
	}
	VOID				UIControlRT::RenderTexture(const PixelW& pos, const PixelW& center, const SizeW& size, const color4& color, float fRotation)
	{CPUTime(UIControlRT);
		Texture* texture	=	GetDlgManager()->GetTextureNode(m_textureID, this->m_avatar->m_ptr);
		if(!texture || (int)(color.a*1000)==0)return;//如果图片透明值是0则不渲染
		Rect destRect;
		destRect.left = -center.x;//m_position.x - m_center.x - m_avatar->m_center.x;
		destRect.top = -center.y;//m_position.y - m_center.y - m_avatar->m_center.y;
		destRect.right = destRect.left + size.cx;//m_textureRect.GetWidth();
		destRect.bottom = destRect.top + size.cy;//m_textureRect.GetHeight();
		//
		//if(!g_manager.IsClipRender(destRect))
		//return;//如果溢出屏幕区域则不渲染
		//
		UIElement* element	=	m_ptr->GetElementPtr(0);
		int nHighlight		=	0;
		if(element->m_nColorOP == Phantom::TextureBlend_MODULATE2X)
			nHighlight		=	1;
		else if(element->m_nColorOP == Phantom::TextureBlend_MODULATE4X)
			nHighlight		=	2;
		//
		float fTexWidth = texture->GetWidth();
		float fTexHeight = texture->GetHeight();
		float ub = (float)m_textureRect.left / (float)fTexWidth;
		float vb = (float)m_textureRect.top / (float)fTexHeight;
		float ue = (float)m_textureRect.right / (float)fTexWidth;
		float ve = (float)m_textureRect.bottom / (float)fTexHeight;
		if(!element->m_bRenderScale){
			int tright = m_textureRect.left + size.cx;
			int tbottom = m_textureRect.top + size.cy;
			if(tright > m_textureRect.right)
			{
				destRect.right -= (tright - m_textureRect.right);
				tright = m_textureRect.right;
			}
			if(tbottom > m_textureRect.bottom)
			{
				destRect.bottom -= (tbottom - m_textureRect.bottom);
				tbottom = m_textureRect.bottom;
			}
			ue = (float)tright / (float)fTexWidth;
			ve = (float)tbottom / (float)fTexHeight;
		}
		//
		if(this->m_uvIndex>=0){
			m_uvCurrentTime += m_uvspeed * m_avatar->m_actionSpeed * g_manager.GetElapsedTime();
			int uvIndex = (int)m_uvCurrentTime;
			UVAnimation* uvAni = element->m_uvAnimations[m_uvIndex];
			int texIndex = 0;
			Pixel offset(0,0);
			Rect rcUV;
			if(!uvAni->SetCurrentIndex(rcUV, uvIndex, offset))
				this->m_bUVPlaying	=	false;
			//
			{
				destRect.Offset(offset.x, offset.y);
				ub = (float)rcUV.left / (float)fTexWidth;
				vb = (float)rcUV.top / (float)fTexHeight;
				ue = (float)rcUV.right / (float)fTexWidth;
				ve = (float)rcUV.bottom / (float)fTexHeight;
			}
		}
		float2		_points[4];//最多4个点
		float2		_uv[4];
		//
		_points[0].setxy(destRect.left, destRect.top);
		_points[1].setxy(destRect.right, destRect.top);
		_points[2].setxy(destRect.right, destRect.bottom);
		_points[3].setxy(destRect.left, destRect.bottom);
		//
		_uv[0].setxy(ub, vb);
		_uv[1].setxy(ue, vb);
		_uv[2].setxy(ue, ve);
		_uv[3].setxy(ub, ve);
		unsigned int		c[4];
		color4 temp = m_avatar->m_color * color;
		c[0]=c[1]=c[2]=c[3]=temp;
		//
		if((int)(m_fScale*1000)!=1000)//如果有缩放
		{
			for(int i=0;i<4;i++)
			{
				_points[i].x *= m_fScale;
				_points[i].y *= m_fScale;
			}
		}
		if(m_ptr->m_bEnableRotation)
		{
			fRotation = Radians(fRotation);
			for(int i=0;i<4;i++)
			{
				float3 temp(_points[i].x, _points[i].y, 0);
				temp.RotateZ(temp, fRotation);
				_points[i].setxy(temp.x, temp.y);
			}
		}
		int dlgX = pos.x - m_avatar->m_center.x;
		int dlgY = pos.y - m_avatar->m_center.y;
		for(int i=0;i<4;i++)
		{
			_points[i].x += dlgX;
			_points[i].y += dlgY;
		}
		if((int)(m_avatar->m_fScale*1000)!=1000)//如果有缩放
		{
			for(int i=0;i<4;i++)
			{
				_points[i].x *= m_avatar->m_fScale;
				_points[i].y *= m_avatar->m_fScale;
			}
		}
		if((int)(m_avatar->m_fRotation*3600)!=0)//如果avatar有转动则计算转动率
		{
			float fRot = Radians(m_avatar->m_fRotation);
			for(int i=0;i<4;i++)
			{
				float3 temp(_points[i].x, _points[i].y, 0);
				temp.RotateZ(temp, fRot);
				_points[i].setxy(temp.x, temp.y);
			}
		}
        float fScaleX = (float)g_manager.GetBufferWidth()/(float)m_avatar->m_ptr->m_screenSize.cx;
        float fScaleY = (float)g_manager.GetBufferHeight()/(float)m_avatar->m_ptr->m_screenSize.cy;
        float fScale = getmin(fScaleX,fScaleY);
		Pixel posl[4];
		for(int i=0;i<4;i++)
		{
			posl[i].x = (int)((float)(_points[i].x + m_avatar->m_pos.x)*fScale);
			posl[i].y = (int)((float)(_points[i].y + m_avatar->m_pos.y)*fScale);
		}
		g_manager.Draw2D(texture, posl, _uv, c, 4, nHighlight, 0, m_bEnabled&&m_avatar->m_bEnabled, (CullMode)Phantom::CullMode_CW);
	}
	VOID				UIControlRT::Render()//渲染部件
	{CPUTime(UIControlRT);
		PixelW pos = m_position;
		PixelW center = m_center;
		SizeW size = m_renderSize;
		color4 color = m_textureColor;
		float rot = m_fRotation;
		if(this->m_actionIndex>=0)
		{
			if(!UpdateAction(pos,center,size,color,rot))//如果是隐藏的话
				return;
		}
		if(m_textureID>0)//如果有图片存在
			RenderTexture(pos,center,size,color,rot);
	}
	UIDialogRT::UIDialogRT(UIDialog* dlg)
	{
		m_ptr			=	dlg;//对话框指针
		m_bEnabled		=	true;//设置是否有效
		m_bVisible		=	dlg->GetVisible();
		m_center.Set(0,0);//默认是所有控件的中心点平均值
		m_pos.Set(0,0);//默认是中心点就是初始位置
		m_fScale		=	1;//缩放值，从center点向外扩散缩放
		m_fRotation		=	0;//转动值
		m_actionType	=	-1;//当前动作类型
		m_actionSpeed	=	1;
		m_color.setRGBA(1,1,1,1);//整体图片颜色和透明
		//m_matAvatar.identity();
		//m_matAvatarInv.identity();//矩阵
		m_layer			=	0;//角色显示层级，和对话框的层级一并计算
		m_currentTime	=	0;//当前动作总的播放时间
		m_fRot2DAngle	=	0;
		this->m_requestSort=false;
		for(int i=0;i<m_ptr->m_controls.size();i++)
		{
			UIControlRT* body = new UIControlRT(this, m_ptr->m_controls[i]);
			m_bodys.push_back(body);
		}
		m_nextActions.setGrow(0);
		//m_luaBindSecond = 0;//时间点
		//m_luaBindTimeEvent = 0;//时间是否引发过
		GetDlgManager()->PushAvatar(this);
	}
	UIDialogRT::~UIDialogRT()
	{
		for(int i=0;i<m_bodys.size();i++)
		{
			delete m_bodys[i];
		}
		for(int i=0;i<m_drawMgrs.size();i++)
		{
			safe_release(m_drawMgrs[i]);
		}
		m_bodys.clear();
		GetDlgManager()->PopAvatar(this);
	}

	//VOID				UIDialogRT::SetActionFunc(LPCSTR szLuaBegin, LPCSTR szLuaEnd)
	//{
	//	if(szLuaBegin)
	//		m_luaBegin = szLuaBegin;
	//	else
	//		m_luaBegin.t[0] = 0;
	//	if(szLuaEnd)
	//		m_luaEnd = szLuaEnd;
	//	else
	//		m_luaEnd.t[0] = 0;
	//}
	//VOID				UIDialogRT::PushAction(const char* actionName)
	//{
	//	NextAction next;
	//	next.mode = 2;
	//	next.name = actionName;
	//	this->m_nextActions.push_back(next);
	//}
	VOID				UIDialogRT::PushAction(ActionType type)
	{
		this->m_nextActions.push_back(type);
	}

	//VOID				UIDialogRT::SetBindTime(float fSecond, const char* szLuaFunc)
	//{
	//	if(!szLuaFunc || !szLuaFunc[0])
	//	{
	//		m_luaBindTime.t[0] = 0;
	//		return;
	//	}
	//	m_luaBindTime = szLuaFunc;
	//	this->m_luaBindSecond = fSecond;
	//	this->m_luaBindTimeEvent = false;
	//}
	VOID				UIDialogRT::Render()//渲染avatar
	{CPUTime(UIDialogRT);
		if(!m_bVisible)
			return;
		double fLast = m_currentTime;
		this->m_currentTime += g_manager.GetElapsedTime();
		//if(m_luaBindTime.t[0] && !m_luaBindTimeEvent)
		//{
		//	if(m_luaBindSecond>=fLast && m_luaBindSecond<m_currentTime)
		//	{
		//		m_luaBindTimeEvent = true;
		//		lua_UIAvatar_s mgr;
		//		SetIPtr(&mgr.m_data, this, UIDialogRT, IAvatarPtr);
		//		lua_tinker::call<void>(g_luamanager.GetLuaState(), m_luaBindTime.t, mgr);
		//	}
		//}
		if(this->m_requestSort)
		{
			qsort(m_bodys.GetPtr(), m_bodys.size(), sizeof(UIDialogRT*), compareBodyPart);
			m_requestSort = false;
		}
		BOOL bAnimation = false;
		for(int ctrl = 0;ctrl<m_bodys.size();ctrl++)//-1); ctrl >= 0 ; ctrl--)
		{
			UIControlRT* c	=	m_bodys[ctrl];
			if(!c->m_bVisible)
				continue;
			c->Render();
			if(!c->IsAnimationOver())
				bAnimation=true;
		}
		if(!bAnimation)//如果所有的控件动作都播放完毕了则进行下一个动作的播放
		{
			if(this->m_actionType != ActionType_None && !CheckNextAction())
			{
				this->SetActionType(ActionType_None);
			}
		}
	}
	VOID				UIDialogRT::SetCenter(const Pixel& v)
	{CPUTime(UIDialogRT);
		this->m_center	=	v;
	}
	UIControlRT*			UIDialogRT::Pick(int x, int y)
	{CPUTime(UIDialogRT);
		return 0;
	}
	UIControlRT*			UIDialogRT::FromName(const char* f)
	{CPUTime(UIDialogRT);
		for(int i=0;i<m_bodys.size();i++)
		{
			if(m_bodys[i]->m_ptr->m_name == f)
				return m_bodys[i];
		}
		return 0;
	}
	VOID				UIDialogRT::SetVisible(BOOL b)
	{CPUTime(UIDialogRT);
		this->m_bVisible	=	b;
	}
	VOID				UIDialogRT::SetOpacity(float f)
	{CPUTime(UIDialogRT);
		this->m_color.a = f;
	}
	VOID				UIDialogRT::SetScale(float f)
	{CPUTime(UIDialogRT);
		this->m_fScale	=	f;
	}
	//VOID				UIDialogRT::SetTransform(const matrix4x4& mat)
	//{
	//	this->m_matAvatar	=	mat;
	//	Phantom::matrixInverse(&this->m_matAvatarInv, 0, &m_matAvatar);
	//}
	VOID				UIDialogRT::SetRotation(float f)
	{CPUTime(UIDialogRT);
		this->m_fRotation	=	f;
	}
	VOID				UIDialogRT::SetPos(const Pixel& p)
	{CPUTime(UIDialogRT);
		this->m_pos	=	p;
	}
	VOID				UIDialogRT::BindDraw(UIDrawMgr* mgr)
	{CPUTime(UIDialogRT);
		safe_addref(mgr);
		m_drawMgrs.push_back(mgr);
	}
	VOID				UIDialogRT::SetLayer(int layer)
	{CPUTime(UIDialogRT);
		this->m_layer	=	layer;
		GetDlgManager()->m_requestSortAvatar = true;
	}
	VOID				UIDialogRT::SetActionType(ActionType t)
	{CPUTime(UIDialogRT);
		this->m_nextActions.SetArrayCount(0);
		OnSetActionType(t);
	}
	//VOID				UIDialogRT::SetAction(const char* szAction)
	//{
	//	this->m_nextActions.SetArrayCount(0);
	//	OnSetAction(szAction);
	//}
	BOOL				UIDialogRT::CheckNextAction()
	{CPUTime(UIDialogRT);
		if(m_nextActions.size()>0)
		{
			short next = m_nextActions[0];
			m_nextActions.eraseByIndex(0);
			OnSetActionType((ActionType)next);
			return true;
		}
		return false;
	}

	VOID				UIDialogRT::OnSetActionType(ActionType t)
	{CPUTime(UIDialogRT);
		OnEndAction(m_actionType);//, m_actionName.t);
		//m_luaBindTimeEvent=false;
		m_currentTime = 0;
		m_actionType	=	t;
		//m_actionName.t[0] = 0;
		for(int i=0;i<m_bodys.size();i++)
		{
			m_bodys[i]->SetActionType((ActionType)m_actionType);
			m_bodys[i]->SetUVAnimationType((ActionType)m_actionType);
			//if(m_actionName.t[0] == 0){
			//	const char* aname = m_bodys[i]->GetAction();
			//	if(aname[0])
			//		m_actionName = aname;
			//	aname = m_bodys[i]->GetUVAnimation();
			//	if(aname[0])
			//		m_actionName = aname;
			//}
		}
		OnBeginAction(m_actionType);
	}
	/*VOID				UIDialogRT::OnSetAction(const char* szAction)
	{
		OnEndAction(m_actionType, m_actionName.t);
		m_luaBindTimeEvent=false;
		m_currentTime = 0;
		m_actionType = ActionType_None;
		m_actionName = szAction;
		for(int i=0;i<this->m_bodys.size();i++){
			m_bodys[i]->SetAction(szAction);
			m_bodys[i]->SetUVAnimation(szAction);
			if(m_actionType==ActionType_None){
				ActionType temp = m_bodys[i]->GetActionType();
				if(temp != ActionType_None)
					m_actionType = temp;
				temp = m_bodys[i]->GetUVAnimationType();
				if(temp != ActionType_None)
					m_actionType = temp;
			}
		}
		OnBeginAction(m_actionType, m_actionName.t);
	}*/
	VOID				UIDialogRT::OnEndAction(int actiontype)
	{CPUTime(UIDialogRT);
		//if(m_luaEnd.t[0])
		//{
		//	lua_UIAvatar_s mgr;
		//	SetIPtr(&mgr.m_data, this, UIDialogRT, IAvatarPtr);
		//	lua_tinker::call<void>(g_luamanager.GetLuaState(), m_luaEnd.t, actiontype, mgr);
		//}
	}
	VOID				UIDialogRT::OnBeginAction(int actiontype)//, const char* actionname)
	{CPUTime(UIDialogRT);
		//if(m_luaBegin.t[0])
		//{
		//	lua_UIAvatar_s mgr;
		//	SetIPtr(&mgr.m_data, this, UIDialogRT, IAvatarPtr);
		//	lua_tinker::call<void>(g_luamanager.GetLuaState(), m_luaBegin.t, actiontype, actionname, mgr);
		//}
	}
	//VOID				UIDialogRT::BindClick(const char* fnLua)
	//{
	//	m_clickFunc	=	fnLua;
	//}
	VOID				UIDialogRT::SetActionSpeed(float f)
	{CPUTime(UIDialogRT);
		this->m_actionSpeed	=	clamp(f, 0, 100);
	}
	VOID				UIDialogRT::SetEnabled(BOOL b)
	{CPUTime(UIDialogRT);
		this->m_bEnabled	=	b;
	}
	VOID				UIDialogRT::SetRot2DAngle(float fAngle)
	{CPUTime(UIDialogRT);
		this->m_fRot2DAngle	=	fAngle;
		for(int i=0;i<m_bodys.size();i++)
		{
			m_bodys[i]->SetRot2DAngle(m_fRot2DAngle);
		}
	}
	VOID				UIDialogRT::SetRot2DDir(const Pixel& dir)
	{CPUTime(UIDialogRT);
		SetRot2DAngle(Degress(getRotateZ(float3(dir.x, dir.y, 0))));
	}

};
