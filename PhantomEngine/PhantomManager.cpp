//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   Manager.cpp
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "PhantomManager.h"
#include "PhantomFile.h"
#include "PhantomAction.h"
#include "PhantomHttp.h"
#include "LuaManager.h"
#include "PhantomMap2D.h"
#include "encrypt.h"
#include "zlib.h"

namespace Phantom{
	Manager	g_manager;	//
	extern	Device*			CreateDevice();
	extern	Device*			CreateDeviceV1();
	Device*					GetDevicePtr()
	{
		return g_manager.GetDevicePtr();
	}
	Callback*	GetCallbackPtr()
	{
		return g_manager.m_callback;
	}
	Manager::Manager()
	{
		m_bPause			=	false;
		m_fFrameSpeed		=	1;
		m_uiDialogSize.Set(768,512);
		m_bMute				=	false;
		m_callback			=	0;
        for(int i=0;i<MAX_TOUCH_COUNT;i++)
		{
			m_cursorDown[i] = 0;
			m_cursorPos[i].Set(0, 0);
		}
		m_bUseThread		=	false;
		m_bInIDE			=	false;
		m_shadowPCF			=	0;
		m_shadowDensity		=	0.5;
		m_gameWidth			=	960;
		m_gameHeight		=	640;
		m_renderIdx			=	0;
        m_deviceWidth       =   320;
        m_deviceHeight      =   480;
		m_nSetMVP			=	0;
		m_pSetMVP			=	0;
		m_shadowEpsilon		=	0.001f;
		m_bBrightness=m_bContrast = false;
		m_shadowMapViewMode	=	false;
		m_fBrightness=0;
		m_fContrast = 1;
		m_world.identity();
		m_bGameOverflow		=	true;
		m_textid			=	1;
		m_meshid			=	1;
		m_programType		=	0;
		m_fogEnable			=	false;
		m_fogNear			=	0;
		m_fogFar			=	100;
		m_fogMode			=	0;
		m_shadowViewSize	=	1;
		m_nGravitySmooth	=	0;
		m_lightDir.setxyz(1,1,1);
		m_lightEnable		=	true;
		m_lightColor.setxyz(1,1,1);
		//
        this->m_nOSType = 0;
		m_dFPS		=	0;
		m_dAppTime	=	0;
		m_dElapsedTime	=	0;
		m_dLastFPS	=	0;
		m_nFrameMoveCount	=	0;
		m_bufferWidth		=	0;
		m_bufferHeight		=	0;
        m_nOrientation      =   1;
        m_bMp3Enabled       =   true;
        m_bWavEnabled       =   true;
		m_currProg			=	0;
		m_bVR3DViewer		=	false;
		m_bVR3DSingleEye	=	false;
		m_bIsInitialized	=	false;
		m_shaderMode		=	RenderMode_Color;
		m_bShaderModeBlend	=	0;
		m_ambientColor.setxyz(0,0,0);
		//
		m_dRealElapsedTime	=	0;
		m_fRenderAlphaDistance	=	To3DValue(0.25f);
		m_renderAlphaCount		=	0;
		memset(m_renderAlpha, 0, sizeof(m_renderAlpha));
		m_renderDrawMgrCount		=	0;
		memset(m_renderDrawMgr, 0, sizeof(m_renderDrawMgr));
		m_renderNoZBufferCount		=	0;
		memset(m_renderNoZBuffer, 0, sizeof(m_renderNoZBuffer));
		m_bDeviceSetup			=	false;
		m_bRenderMeshChanged	=	true;
		m_fSleedTime			=	0.03f;
		m_dDoubleClickTime		=	0.5f;
		m_nTextureBaseDiv		=	1;
		m_bTextureDivAsFile		=	false;
        m_tempText				=	0;
		m_authText				=	0;
		m_camera				=	0;
		m_uiManager				=	0;
		//
		m_uiScaleSave.setxy(1,1);
		m_uiScale.setxy(1, 1);
		m_uiOffset.setxy(0, 0);
		//初始化矩阵信息
		m_f2DFov			=	PI / 4.0f;
		m_fTan2DFov			=	tanf(m_f2DFov / 2.0f) * 2.0f;
		m_bIsUtf8			=	false;
		m_bRenderUI			=	true;
		m_bIsPause			=	false;
		m_bExitGame			=	false;
		this->m_bShowFPS	=	true;
		m_dLastDownTime		=	0;
		m_numRenderTriangles	=	0;
		//
		m_allParticles			=	0;
		m_topParticle			=	0;
		m_particleCount			=	0;
		m_particleAssignIndex	=	0;
		memset(&m_sensordata, 0, sizeof(m_sensordata));
		m_device				=	0;
#ifdef EXPORT_ENGINE
		m_uiManager				=	new UIManager();
		m_camera				=	new Camera();
		m_device				=	CreateDevice();
#endif
	}
	const float3&			Manager::GetFogPos()
	{CPUTimeNone();
		if(m_fogMode < 5)
			return m_camera->GetEye();
		return m_camera->GetTarget();
	}
	VOID					Manager::SetBrightness(BOOL bEnable, float fBrightness)
	{CPUTimeNone();
		m_bBrightness = bEnable;
		m_fBrightness = fBrightness;
		ResetProgramType();
	}
	VOID					Manager::SetContrast(BOOL bEnable, float fContrast)
	{CPUTimeNone();
		m_bContrast = bEnable;
		m_fContrast = fContrast;
		ResetProgramType();
	}
	VOID					Manager::ResetProgramType()
	{CPUTimeNone();
		this->m_programType = 0;
		if(this->m_fogEnable){
			if(m_fogMode == FogMode_Fog3D)
				m_programType = Program::Type_Fog;
			else if(m_fogMode == FogMode_Fog3DGray)
				m_programType = Program::Type_Fog3DGray;
			else if(m_fogMode == FogMode_Fog3DSub)
				m_programType = Program::Type_Fog3DSub;
			else if(m_fogMode == FogMode_Fog2DSub)
				m_programType = Program::Type_Fog2DSub;
			else if(m_fogMode == FogMode_Fog2D)
				m_programType = Program::Type_Fog2D;
			else if(m_fogMode == FogMode_Fog2DGray)
				m_programType = Program::Type_Fog2DGray;
		}
		if(m_bBrightness)
			this->m_programType |= Program::Type_Brightness;
		if(m_bContrast)
			this->m_programType |= Program::Type_Contrast;
	}
	VOID					Manager::SetFog(BOOL bEnable, unsigned int color, float fnear, float ffar, FogMode type)
	{CPUTimeNone();
		this->m_fogEnable = bEnable;
		this->m_fogNear = fnear;
		this->m_fogFar = ffar;
		if(m_fogFar < (m_fogNear+0.1f))
			m_fogFar = m_fogNear + 0.1f;
		color4 c(color);
		m_fogColor.setxyz(c.r,c.g,c.b);
		this->m_device->SetClearColor(c);
		this->m_fogMode = type;
		ResetProgramType();
	}
	VOID					Manager::FramePause(BOOL bPause)//暂停整个游戏
	{CPUTimeNone();
		this->m_bPause = bPause;
		g_luamanager.PauseTime(bPause);
		if(bPause)
		{
			LogInfo("system->:pause:%s", utf8("游戏暂停：所有暂停之前所调用的interval和timeout都将暂停，后来创建的则不会被暂停，除非再次调用暂停函数"));
		}
		else
		{
			LogInfo("system->:pause:%s", utf8("游戏继续：所有暂停的计时器都继续运行"));
		}
	}
	VOID					Manager::SetFrameSpeed(float f)//设置整个游戏的播放速度,1是正常,不能小于0
	{CPUTimeNone();
		this->m_fFrameSpeed = clamp(f, 0, 10);
	}
	
	INT						Manager::AddText(const char* text, int x, int y)
	{CPUTimeNone();
		DrawTextItem item;
		item.text = text;
		item.id = m_textid++;
		item.x = x;
		item.y = y;
		item.c.setRGBA(1,1,1,1);
		item.size = 16;
		m_textItems.push_back(item);
		return item.id;
	}
	INT						Manager::AddText(const char* text, int x, int y, const color4& c, int size)
	{CPUTimeNone();
		DrawTextItem item;
		item.text = text;
		item.id = m_textid++;
		item.x = x;
		item.y = y;
		item.c = c;
		item.size = clamp(size, 6, 512);
		m_textItems.push_back(item);
		return item.id;
	}
	VOID					Manager::SetText(int nID, const char* text)
	{CPUTimeNone();
		for(int i=0;i<m_textItems.size();i++)
		{
			if(m_textItems[i].id == nID){
				m_textItems[i].text = text;
				break;
			}
		}
	}
	VOID					Manager::RemoveText(int nID)
	{CPUTimeNone();
		for(int i=0;i<m_textItems.size();i++)
		{
			if(m_textItems[i].id == nID){
				m_textItems.eraseByIndex(i);
				break;
			}
		}
	}

	VOID					Manager::SetUIDialogSize(BOOL bScale, int width, int height, Pixel* ret, int touchIndex)
	{CPUTimeT(Manager);
		m_uiDialogSize.Set(width, height);
		if(m_bufferWidth > 0)
		{
			float fX = (float)m_bufferWidth / (float)GetUIDialogSizeW();
			float fY = (float)m_bufferHeight / (float)GetUIDialogSizeH();
			if(!bScale){
				if(m_bufferWidth<m_bufferHeight)//如果是纵向的则宽度为缩放基础
					fY = fX;
				else
					fX = fY;//横向的则高度为缩放基础
			}
			m_uiDialogSize.Set((float)m_bufferWidth/fX, (float)m_bufferHeight/fY);
			SetUIOffset(0, 0);
			SetUIScale(fX, fY);
			RecalcRenderRect();
			if(ret){
				ret->Set((int)((float)(m_cursorPos[touchIndex].x - m_uiOffset.x) / m_uiScale.x), (int)((float)(m_cursorPos[touchIndex].y-m_uiOffset.y) / m_uiScale.y));
			}
		}
	}
	VOID			Manager::SetUIScaleMode(BOOL b, Pixel* ret)
	{CPUTimeT(Manager);
		if(b)
		{
			this->m_uiScale = this->m_uiScaleSave;
			m_uiOffset = m_uiOffsetSave;
		}
		else
		{
			m_uiScale.x = m_uiScale.y = getmin(m_uiScaleSave.x, m_uiScaleSave.y);
			m_uiOffset.x = (float)this->GetBufferWidth()*(m_uiScaleSave.x-m_uiScale.x)/2;
			m_uiOffset.y = (float)this->GetBufferHeight()*(m_uiScaleSave.y-m_uiScale.y)/2;
		}
		RecalcRenderRect();
		if(ret){
			ret->Set((int)((float)(m_cursorPos[0].x - m_uiOffset.x) / m_uiScale.x), (int)((float)(m_cursorPos[0].y-m_uiOffset.y) / m_uiScale.y));
		}
	}
	VOID			Manager::PushHttp(HttpRequest* hr)
	{
		m_https.push_back(hr);
	}
	VOID			Manager::PopHttp(HttpRequest* hr)
	{
		for(int i=0;i<m_https.size();i++)
		{
			if(m_https[i] == hr)
				m_https.eraseByIndex(i);
		}
	}
	VOID			Manager::RemoveText(RenderText* text)
	{CPUTimeT(Manager);
		if(text->IsFigFont())
		{
			delete text;
			return;
		}
		for(int i=0;i<this->m_textPool.size();i++)
		{
			RenderTextPool& rt = m_textPool[i];
			if(rt.text == text)
			{
				rt.count--;
				if(rt.count==0)
				{
					delete rt.text;
					m_textPool.eraseByIndex(i);
				}
				return;
			}
		}
	}
	RenderText*		Manager::InsertText(const char* szText, int nFontSize, int nFontStyle, int nTextAlignment, const char* szFontName)
	{CPUTimeT(Manager);
		for(int i=0;i<this->m_textPool.size();i++)
		{
			RenderTextPool& rt = m_textPool[i];
			if(stricmp(rt.text->m_text.GetPtr(), szText) == 0 && rt.text->m_nFontSize == nFontSize && rt.text->m_nFontStyle==nFontStyle && rt.text->m_nTextAlignment==nTextAlignment)
			{
				if(szFontName&&stricmp(rt.text->m_name.t, szFontName)!=0)//如果不是相同字体则继续搜索
					continue;
				rt.count++;
				return rt.text;
			}
		}
		RenderTextPool rt;
		rt.text = m_device->CreateRenderText();
		rt.count = 1;
		rt.text->m_nTextAlignment = nTextAlignment;
		rt.text->SetFontSize(nFontSize);
		rt.text->SetFontStyle(nFontStyle);
		rt.text->SetFontName(szFontName);
		rt.text->SetText(szText);
		m_textPool.push_back(rt);
		return rt.text;
	}
	/*RenderText*		Manager::InsertText(const short_t* szText, int nFontSize, int nFontStyle)
	{
		for(int i=0;i<this->m_textPool.size();i++)
		{
			RenderTextPool& rt = m_textPool[i];
			if(u_stricmp(rt.text->m_text.GetPtr(), szText) == 0 && rt.text->m_nFontSize == nFontSize && rt.text->m_nFontStyle==nFontStyle)
			{
				rt.count++;
				return rt.text;
			}
		}
		RenderTextPool rt;
		rt.text = m_device->CreateRenderText();
		rt.count = 1;
		rt.text->SetFontSize(nFontSize);
		rt.text->SetFontStyle(nFontStyle);
		rt.text->SetText(szText);
		m_textPool.push_back(rt);
		return rt.text;
	}*/

	VOID			Manager::SetupParticles(INT nMaxParticles)	//设置最大粒子数量
	{CPUTimeNone();
		if(m_allParticles)
			return;
		m_particleCount			=	nMaxParticles;
		m_allParticles			=	(ParticleData*)malloc(sizeof(ParticleData) * m_particleCount);
		memset(m_allParticles, 0, sizeof(ParticleData) * m_particleCount);
		this->m_topParticle		=	0;
		m_particleAssignIndex	=	0;
	}

	ParticleData*			Manager::AllocParticle()			//分配粒子
	{CPUTimeNone();
		if(m_topParticle)
		{
			ParticleData* pRet	=	m_topParticle;
			m_topParticle			=	m_topParticle->next;
			pRet->next				=	0;
			return pRet;
		}
		if(this->m_particleAssignIndex < this->m_particleCount)
		{
			ParticleData* pRet	=	&m_allParticles[m_particleAssignIndex];
			m_particleAssignIndex	++;
			pRet->next				=	0;
			return pRet;
		}
		return NULL;
	}
	VOID						Manager::PlayMusic(const char* szFile)
	{CPUTimeNone();
		MultiMusic* music	=	GetMusicFilePtr(szFile);
		if(music)
			music->Play(1);
	}
	VOID						Manager::FreeParticle(ParticleData* p)	//释放粒子
	{CPUTimeNone();
		if(p->next)return;
		ParticleData* pLocal	=	(ParticleData*)p;
		pLocal->next				=	m_topParticle;
		m_topParticle				=	pLocal;
	}

	INT							Manager::GetFreeParticleCount()								//返回空闲的粒子数量
	{CPUTimeNone();
		INT numFree				=	m_particleCount - this->m_particleAssignIndex;
		ParticleData* next		=	m_topParticle;
		while(next)
		{
			numFree				++;
			next				=	next->next;
		}
		return numFree;
	}

	INT				Manager::CreateProgramFromFile(const char* szFile, BOOL bHasBlend)
	{CPUTimeT(Manager);
		FileName f = szFile;
		f.setIsFile();
		int nFree = -1;
		for(int v=RenderMode_Custom_Max;v<RenderMode_Max;v++)
		{
			if(m_progMode[v] == 0 && m_progModeB[v] == 0){
				if(nFree==-1)
					nFree = v;
			}
			else if(m_progMode[v]&&m_progMode[v]->m_fileName.size()>0&&!bHasBlend){
				if(stricmp(m_progMode[v]->m_fileName.GetPtr(), szFile) == 0)
					return v;
			}
			else if(m_progModeB[v]&&m_progModeB[v]->m_fileName.size()>0&&bHasBlend){
				if(stricmp(m_progModeB[v]->m_fileName.GetPtr(), szFile) == 0)
					return v;
			}
		}
		if(nFree < 0)return -1;
		LFileName ret;
		if(!fexist(f.t, &ret))
			return -1;
		if(!bHasBlend){
			m_progMode[nFree]	=	m_device->CreateProgram((int)nFree);
			m_progMode[nFree]->SetFileName(f.t, false);
		}
		else
		{
			m_progModeB[nFree]	=	m_device->CreateProgram((int)nFree);
			m_progModeB[nFree]->SetFileName(f.t, true);
		}
		if(this->m_bDeviceSetup)
		{
			if(m_progMode[nFree])
				m_progMode[nFree]->OnResetDevice();
			if(m_progModeB[nFree])
				m_progModeB[nFree]->OnResetDevice();
		}
		return nFree;
	}
	INT				Manager::CreateProgram(int nProgramType, BOOL bBlend)//如果不存在则自动创建shader方法
	{CPUTimeT(Manager);
		int nFree = -1;
		for(int v=1;v<RenderMode_Max;v++)
		{
			if(m_progMode[v] == 0&&m_progModeB[v] == 0){
				if(nFree==-1)
					nFree = v;
			}
			else if(m_progMode[v]&&m_progMode[v]->GetProgramType() == nProgramType&&!bBlend)
				return v;
			else if(m_progModeB[v]&&m_progModeB[v]->GetProgramType() == nProgramType&&bBlend)
				return v;
		}
		if(nFree < 0)
            return -1;
		if(!bBlend){
			m_progMode[nFree]	=	m_device->CreateProgram((int)nFree);
			m_progMode[nFree]->SetProgramType(nProgramType, false);
		}
		else
		{
			m_progModeB[nFree]	=	m_device->CreateProgram((int)nFree);
			m_progModeB[nFree]->SetProgramType(nProgramType, true);
		}
		if(this->m_bDeviceSetup)
		{
			if(m_progMode[nFree])
				m_progMode[nFree]->OnResetDevice();
			if(m_progModeB[nFree])
				m_progModeB[nFree]->OnResetDevice();
		}
		return nFree;
	}
	VOID			Manager::CreateProgramFromMode(int mode, const char* szFile, BOOL bHasBlend)
	{CPUTimeT(Manager);
		if(mode<0||mode>=RenderMode_Custom_Max)
			return;
		LFileName ret;
		if(!fexist(szFile, &ret))
			return;
		m_progMode[mode]	=	m_device->CreateProgram((int)mode);
		m_progMode[mode]->SetFileName(szFile, false);
		if(bHasBlend)
		{
			m_progModeB[mode]	=	m_device->CreateProgram((int)mode);
			m_progModeB[mode]->SetFileName(szFile, true);
		}
		if(this->m_bDeviceSetup)
		{
			m_progMode[mode]->OnResetDevice();
			if(m_progModeB[mode])
				m_progModeB[mode]->OnResetDevice();
		}
	}
	Manager::~Manager()
	{
		for(int i=0;i<m_allocs.size();i++)
			delete m_allocs[i];
        if(m_tempText)
            delete m_tempText;
        m_tempText = 0;
		if(m_authText)
			delete m_authText;
		m_authText = 0;
		for(int i=0;i<m_actions.size();i++)
			safe_release(m_actions[i]);
		if(m_allParticles)
			free(m_allParticles);
		m_allParticles			=	0;
		m_topParticle			=	0;
		for(int v=0;v<RenderMode_Max;v++)
		{
			safe_delete(m_progMode[v]);
			safe_delete(m_progModeB[v]);
		}
		delete m_uiManager;
		delete m_camera;
		delete m_device;
	}

	Program*			Manager::SetRenderMode(_shader_var& tech, BOOL bBlendMode, const char* szName)
	{CPUTimeT(Manager);
		if(tech.is_findFail())
			return 0;
		if(tech.handle >= 0 && tech.is_findTechniqueOK())
		{
			if((unsigned int)tech.handle < RenderMode_Max)
			{
				if(SetRenderMode((int)tech.handle, bBlendMode))
					return m_currProg;
				return 0;
			}
		}
		tech.resetID		=	0;
		tech.programID	=	0;
		if(bBlendMode)
		{
			for(int i=0;i<RenderMode_Max;i++)
			{
				if(!m_progModeB[i])
					continue;
				if(m_progModeB[i]->GetName() == szName)
				{
					tech.set_technique(i);
					if(SetRenderMode((int)tech.handle, bBlendMode))
						return m_currProg;
					return 0;
				}
			}
		}
		for(int i=0;i<RenderMode_Max;i++)
		{
			if(!m_progMode[i])
				continue;
			if(m_progMode[i]->GetName() == szName)
			{
				tech.set_technique(i);
				if(SetRenderMode((int)tech.handle, bBlendMode))
					return m_currProg;
				return 0;
			}
		}

		tech.set_null();
		return 0;
	}
	VOID					Manager::LoadAllResources()	//加载所有资源
	{CPUTimeT(Manager);
		for(int i=0;i<ZipFilePackage::s_zipFiles.size();i++)
		{
			ZipFilePackage* zipFile	=	ZipFilePackage::s_zipFiles[i];
			{
				std::string s[2];
				s[0]	=	".png";
				s[1]	=	".jpg";
				PFileList* f	=	zipFile->enumFiles("", true, s, 2);
				if(f)
				{
					for(int i=0;i<f->vFiles.size();i++)
					{
						TexturePtr tex;
						if(this->LoadTexture(tex, f->vFiles[i]))
						{
							Texture* t = tex.GetPtr();
							safe_addref(t);
							m_loadTextures.push_back(t);
						}
					}
					delete f;
				}
			}
			{
				std::string s[2];
				s[0]	=	".wav";
				s[1]	=	".mp3";
				PFileList* f	=	zipFile->enumFiles("", true, s, 2);
				if(f)
				{
					for(int i=0;i<f->vFiles.size();i++)
					{
						MultiMusic* music	=	this->GetMusicFilePtr(f->vFiles[i]);
						//if(music)
							//music->CreateAll();
					}
					delete f;
				}
			}
		}
	}
	VOID					Manager::OnLostDevice()
	{CPUTimeT(Manager);
		if(!this->m_bIsInitialized || !m_bDeviceSetup)
			return;
		m_device->OnLostDevice();
		m_bDeviceSetup	=	false;
		for(int i=0;i<this->m_musics.size();i++)
			m_musics[i]->PauseAll();
		for(int v=0;v<RenderMode_Max;v++)
		{
			if(m_progMode[v])
				m_progMode[v]->OnLostDevice();
			if(m_progModeB[v])
				m_progModeB[v]->OnLostDevice();
		}
		for(int i=0;i<m_RTs.size();i++)
			m_RTs[i]->OnLostDevice();
		for(int i=0;i<this->m_textures.size();i++)
			this->m_textures[i]->OnLostDevice();
		for(int i=0;i<this->m_deviceMesh.size();i++)
			this->m_deviceMesh[i]->OnLostDevice();
		for(int i=0;i<m_texts.size();i++)
			m_texts[i]->OnLostDevice();
	}
	VOID					Manager::OnPause()
	{CPUTimeT(Manager);
		if(!m_bIsPause)
		{
			m_bIsPause	=	true;
			for(int i=0;i<this->m_musics.size();i++)
				m_musics[i]->PauseAll();
		}
	}
	VOID					Manager::OnResume()
	{CPUTimeT(Manager);
		if(m_bIsPause)
		{
			m_bIsPause	=	false;
			for(int i=0;i<this->m_musics.size();i++)
				m_musics[i]->ResumeAll();
		}
	}
	VOID					Manager::OnResetDevice()
	{CPUTimeT(Manager);
		if(!m_bIsInitialized)
			return;
		m_device->OnResetDevice();
		m_bDeviceSetup	=	true;
		//
		for(int v=0;v<RenderMode_Max;v++)
		{
			if(m_progMode[v])
				m_progMode[v]->OnResetDevice();
			if(m_progModeB[v])
				m_progModeB[v]->OnResetDevice();
		}
		for(int i=0;i<this->m_textures.size();i++)
			this->m_textures[i]->OnResetDevice();
		for(int i=0;i<this->m_deviceMesh.size();i++)
			this->m_deviceMesh[i]->OnResetDevice();
		for(int i=0;i<m_texts.size();i++)
			m_texts[i]->OnResetDevice();
		for(int i=0;i<m_RTs.size();i++)
			m_RTs[i]->OnResetDevice();
		this->Set2DProjectionFov(m_f2DFov);
		m_camera->SetPerspectiveProjection(m_camera->GetFov(), m_bufferWidth, m_bufferHeight, m_camera->GetZNearest(), m_camera->GetZFar());
		m_device->ResetAllDefault();
		m_device->ClearError();
		m_device->SetAlphaMode(AlpahMode_Unknown);
		m_device->SetBlendMode(BlendMode_UNKNOWN, BlendMode_UNKNOWN);
		m_device->SetViewport(0, 0, m_bufferWidth, m_bufferHeight);
	}

	BOOL					Manager::AppCreate(INT nOSType)	//创建使用幻影引擎系统
	{CPUTimeT(Manager);
        m_nOSType = nOSType;
		if(m_bIsInitialized)
			return true;
		m_uiManager				=	new UIManager();
		m_camera				=	new Camera();
		m_device				=	CreateDevice();
		m_device->SetClearColor(color4(0,0,0,1));
		CreateProgramFromMode(RenderMode_Color, "default_init_zip/shader/tech_color.inl");
		CreateProgramFromMode(RenderMode_Tex1, "default_init_zip/shader/tech_tex1.inl");
		CreateProgramFromMode(RenderMode_ColorTex1, "default_init_zip/shader/tech_colortex1.inl");
		CreateProgramFromMode(RenderMode_ColorTex1Disable, "default_init_zip/shader/tech_colortex1_d.inl");
		CreateProgramFromMode(RenderMode_TextChannel, "default_init_zip/shader/tech_textchannel.inl");
		CreateProgramFromMode(RenderMode_ColorRect, "default_init_zip/shader/tech_colorrect.inl");
		CreateProgramFromMode(RenderMode_TextureRect, "default_init_zip/shader/tech_texturerect.inl");
		CreateProgramFromMode(RenderMode_TextureRectDisable, "default_init_zip/shader/tech_texturerectd.inl");
		CreateProgramFromMode(RenderMode_Default, "default_init_zip/shader/tech_default.inl", true);
		CreateProgramFromMode(RenderMode_PlaneShadow, "default_init_zip/shader/tech_planeshadow.inl");
		matrix4x4 proj;
		proj.PerspectiveFovRH(PI / 4.0f, 1, 0.01, 100.0f);//(this->m_bufferWidth,this->m_bufferHeight, 0.1f, 20.0f);
		matrix4x4 view;
		float3 eye(0, 10.0f, 1.0);
		view.LookAtRH(eye, float3(0, 0, 0), float3(0, 0, 1));
		SetViewMatrix(view);
		SetProjMatrix(proj);
		m_projection2D.identity();
		m_viewProjection2D.identity();
		m_matrixDef2D.identity();
		m_view2D.LookAtRH(float3(0,0,1),  float3(0, 0, 0), float3(0, 0, 1));
		matrixInverse(&m_view2DInv, NULL, &m_view2D);
		//
		m_bIsInitialized	=	true;
		m_bufferWidth		=	960;
		m_bufferHeight		=	640;
		m_bIsPause			=	false;
		//
		RandomStart();
		Set2DProjectionFov(this->m_f2DFov);
		SetDefaultENFont("default_init_zip/fonts/def_en.fig");
		this->m_camera->SetPerspectiveProjection(m_camera->GetFov(), m_bufferWidth, m_bufferHeight, m_camera->GetZNearest(), m_camera->GetZFar());
		m_device->AppCreate();
		//Start();
		//
		{
#ifdef DIRECT_X_9
			float off	=	0;
			float offT	=	0.5f;
#else
			float off	=	0;
			float offT	=	0;
#endif
			unsigned short ind[6];
			ind[0] = 3;
			ind[1] = 1;
			ind[2] = 0;
			ind[3] = 3;
			ind[4] = 2;
			ind[5] = 1;
			PosUV2 vert[4];
			rect_f drr(0,0,1,1);
			vert[0].p.setxyz(drr.left + off, drr.top + off, 0);
			vert[1].p.setxyz(drr.right + off, drr.top + off, 0);
			vert[2].p.setxyz(drr.right + off, drr.bottom + off, 0);
			vert[3].p.setxyz(drr.left + off, drr.bottom + off, 0);
			vert[0].uv[0]	=	float2(1, 1);
			vert[1].uv[0]	=	float2(0, 1);
			vert[2].uv[0]	=	float2(0, 0);
			vert[3].uv[0]	=	float2(1, 0);
			vert[0].uv[1]	=	float2(0, 0);
			vert[1].uv[1]	=	float2(1, 0);
			vert[2].uv[1]	=	float2(1, 1);
			vert[3].uv[1]	=	float2(0, 1);
			MeshPtr temp;
			CreateMesh(temp);
			m_textureMesh = temp;
			m_textureMesh->Create(4, 2, PosUV2::FVF);
			m_textureMesh->WriteVertexBuffer(vert, 4, sizeof(PosUV2));
			m_textureMesh->WriteIndexBuffer((unsigned short*)ind, 6, 0);
			m_textureMesh->Commit();
		}
		return OnCreate();
	}
	BOOL					Manager::SetDefaultENFont(const char* szFigFile)		//从fig文件创建
	{CPUTimeT(Manager);
		FontPtr font;
		if(LoadFont(font, szFigFile))
		{
			LogInfo("LoadFontOK(%s)", szFigFile);
			m_defaultENFont	=	font.GetPtr();
			return true;
		}
		LogInfo("LoadFontFail(%s)", szFigFile);
		return true;

	}
//	VOID					Manager::SetUIClipRect(const Rect* rect)//设置UI裁剪,如果是0则不做任何裁剪
//	{
//		m_bClipRect = (rect!=0);//是否裁剪
//		if(rect)
//			m_rcClipRect = *rect;//裁剪区域
//	}
	BOOL					Manager::SetDefaultUnicodeFont(const char* szFigFile)	//支持国际语言的其他字体型
	{CPUTimeT(Manager);
		m_defaultUnicodeFont = 0;
		if(!szFigFile)return false;
		FontPtr font;
		if(LoadFont(font, szFigFile))
		{
			m_defaultUnicodeFont	=	font.GetPtr();
			return true;
		}
		return true;
	}
    Program*				Manager::GetRenderMode(int shader, BOOL bBlendMode)
    {CPUTimeNone();
        if(bBlendMode)
		{
			if(m_progModeB[shader])
			{
				if(!m_progModeB[shader]->IsCreateFail())
				{
					if(m_progModeB[shader]->IsCreated())
						return m_progModeB[shader];
				}
			}
		}
		if(!m_progMode[shader])
			return 0;
		if(m_progMode[shader]->IsCreateFail())
			return 0;
		if(m_progMode[shader]->IsCreated())
            return m_progMode[shader];
		return 0;
    }
	Program*				Manager::SetRenderMode(int shader, BOOL bBlendMode)
	{CPUTime(Manager);
		if(m_shaderMode == shader && m_bShaderModeBlend == bBlendMode && m_currProg)
		{
			m_currProg->CommitMatrixToShaders();
			return m_currProg;
		}
		if(shader<0||shader>=RenderMode_Max)
			return 0;
		Program* curr	=	0;
		if(bBlendMode)
		{
			if(m_progModeB[shader])
			{
				if(!m_progModeB[shader]->IsCreateFail())
				{
					if(m_progModeB[shader]->IsCreated())
						curr	=	m_progModeB[shader];
				}
			}
		}
		if(!curr)
		{
			if(!m_progMode[shader])
				return 0;
			if(m_progMode[shader]->IsCreateFail())
				return 0;
			if(m_progMode[shader]->IsCreated())
				this->m_currProg	=	m_progMode[shader];
			else
				return 0;
		}
		else
			m_currProg		=	curr;
		m_shaderMode		=	shader;
		m_bShaderModeBlend	=	bBlendMode;
		if(m_currProg)
		{
			CPUTimeN(Manager, 1);
			m_currProg->Active();
			m_currProg->CommitMatrixToShaders();
		}
		return m_currProg;
	}
	VOID					Manager::SetActiveSDK(SDKBase* base)
	{CPUTimeT(Manager);
		AutoPtr<SDKBase>		oldPtr;	//当前运行的状态
		oldPtr = m_sdkPtr.operator Phantom::SDKBase *();
		m_sdkPtr = 0;
		m_sdkPtr = base;
		if(m_sdkPtr)
		{
			//LogInfo("system->Begin New SDK Create");
			m_sdkPtr->OnCreate();
			//LogInfo("system->End New SDK Create");
		}
		if(oldPtr)
		{
			//LogInfo("system->Begin Old SDK Destroy");
			oldPtr->OnDestroy();
			oldPtr = 0;
			//LogInfo("system->End Old SDK Destroy");
		}
	}
	extern	VOID	OnDownloadFrame();
	VOID					Manager::SetVR3DViewerSingle(BOOL b)
	{
		this->m_bVR3DSingleEye = b;
	}
	VOID					Manager::SetVR3DViewer(BOOL b)
	{CPUTimeNone();
		this->m_bVR3DViewer = b;
		GetCameraPtr()->SetViewMatrix(GetCameraPtr()->GetViewMatrix());
	}
	VOID					Manager::AddGravity(float x, float y, float z)
	{
		if(m_nGravitySmooth<=1){
			this->m_sensordata.gravity_x = x;
			this->m_sensordata.gravity_y = y;
			this->m_sensordata.gravity_z = z;
			return;
		}
		if(m_nGravitySmooth>50)m_nGravitySmooth = 50;
		static int num = 0;
		float3 temp(x,y,z);
		if(m_vGravitys.size()<m_nGravitySmooth)
			m_vGravitys.push_back(temp);
		else
			m_vGravitys[num%m_nGravitySmooth] = temp;
		num++;
		//
		temp.setxyz(0,0,0);
		float fInv = 1.0f / (float)m_vGravitys.size();
		for(int i=0;i<m_vGravitys.size();i++){
			temp += m_vGravitys[i];
		}
		this->m_sensordata.gravity_x = temp.x*fInv;
		this->m_sensordata.gravity_y = temp.y*fInv;
		this->m_sensordata.gravity_z = temp.z*fInv;
	}

	TIDMessage*	Manager::alloc()
	{
		int oldSize = m_frees.size();
		if(oldSize>0){
			TIDMessage* msg = m_frees[oldSize-1];
			m_frees.SetArrayCount(oldSize-1);
			return msg;
		}
		TIDMessage* msg = new TIDMessage();
		memset(msg, 0, sizeof(TIDMessage));
		m_allocs.push_back(msg);
		return msg;
	}
	TIDMessage::TIDMessage(){}
	TIDMessage::~TIDMessage(){}
	void	Manager::recycle(TIDMessage* msg)//回收数据块
	{
		if(msg->ptr)
			free(msg->ptr);
		memset(msg, 0, sizeof(TIDMessage));
		m_frees.push_back(msg);
	}
	VOID	Manager::send(TIDMessage::TID messageID, const char* ptr, int length, int id)
	{CPUTimeT(Manager);
		TIDMessage* msg = alloc();
		msg->mid = messageID;
		msg->ptr = (unsigned char*)ptr;
		msg->length = length;
		msg->objectid = id;
		{
			Lock lock(m_lockMessage);
			m_trecvs.push_back(msg);
		}
	}
	VOID					Manager::OnThread()
	{
		while(true){
			if(m_trecvs.size()==0)
				Phantom::AppSleep(100);
			//
			{
				Lock lock(m_lockMessage);
				m_tempTRecvs = m_trecvs;
				m_trecvs.SetArrayCount(0);
			}
			for(int i=0;i<m_tempTRecvs.size();i++)
			{
				TIDMessage* msg = m_tempTRecvs[i];
				switch(msg->mid){
				case TIDMessage::TID_LoadTexture:
					{
						Texture::LoadTextureData((const char*)msg->ptr, msg->length, msg->buffer);
					}
					break;
				}
			}
			{
				Lock lock(m_lockMessage);
				for(int i=0;i<m_tempTRecvs.size();i++){
					m_recvs.push_back(m_tempTRecvs[i]);
				}
			}
		}
	}
	void					Manager::onMessage(TIDMessage* msg)//处理游戏消息接收
	{CPUTimeT(Manager);
		switch(msg->mid){
		case TIDMessage::TID_LoadTexture:
			{
				for(int i=0;i<m_textures.size();i++){
					if(m_textures[i]->GetPtrID().id == msg->objectid){
						m_textures[i]->OnLoadOk(msg->buffer);
						free(msg->ptr);
						msg->ptr = 0;
						LogInfo("system->thread:loadtexture(%s) ok", m_textures[i]->GetFileName().t);
						break;
					}
				}
			}
			break;
		}
	}
	BOOL					Manager::AppFrameMove()			//
	{CPUTimeT(Manager);
		if(!m_bIsInitialized)return false;
		//{
		//	Lock lock(m_lockMessage);
		//	m_tempRecvs = m_recvs;
		//	m_recvs.SetArrayCount(0);
		//}
		//{
		//	for(int i=0;i<m_tempRecvs.size();i++)
		//	{
		//		onMessage(m_tempRecvs[i]);
		//		recycle(m_tempRecvs[i]);
		//	}
		//}
		Lock lock(m_lockRender);
		ClearAlphaObjects();
		if(!m_bDeviceSetup)
		{
			return true;
		}
		if(!m_bIsInitialized)
		{
			return true;
		}
		if(this->m_bIsPause)
		{
			m_dElapsedTime	=	0;
			return true;
		}
		m_dElapsedTime	=	RequestIntervalTime();
		if(m_dElapsedTime > 0.1f)
			m_dElapsedTime	=	0.1f;
		m_nFrameMoveCount++;
		if((m_dAppTime - m_dLastFPS) > 1.0f)
		{
			m_dFPS				=	(double)m_nFrameMoveCount / (m_dAppTime - m_dLastFPS);
			m_nFrameMoveCount	=	0;
			m_dLastFPS			=	m_dAppTime;
		}
		//for(int i=0;i<m_https.size();i++)
		//{
		//	if(m_https[i]->IsUseThread())
		//		m_https[i]->DoMessageQueue();
		//}
		m_dElapsedTime	*= this->m_fFrameSpeed;
		double dTemp = m_dRealElapsedTime = m_dElapsedTime;
		if(m_bPause)
			m_dElapsedTime = 0;
		m_dAppTime		+=	m_dElapsedTime;
		//
		NetRecvMessage::DoMessageQueue();
		OnDownloadFrame();
		m_camera->OnFrameMove(m_dElapsedTime);
		int nMill	=	(int)(m_fSleedTime * 1000.0f);
		//float nDistrictFPS = 20;
		//if(GetFPS()>nDistrictFPS){
		//	float fSleep = 1000.0f / GetFPS();
		//	float fBaseSleep = 1000.0f / nDistrictFPS;
		//	AppSleep(fBaseSleep - fSleep);
		//}
		if(nMill > 0)
			AppSleep(nMill);
#ifndef EDITOR_OS
		g_luamanager.OnFrameMove(this->m_dElapsedTime, dTemp);
#endif
		if(m_sdkNextPtr.t)
		{
			SetActiveSDK(m_sdkNextPtr.t);
			m_sdkNextPtr	=	0;
		}
		if(GetActiveSDK())
		{
			if(!GetActiveSDK()->OnFrameMove(m_dElapsedTime))
				return false;
		}
		for(int i=0;i<m_scene3ds.size();i++)
		{
			if(m_scene3ds[i]->IsEnabled(Scene3D::Enable_ShowScene))
				m_scene3ds[i]->OnFrameMove();
		}
		m_uiManager->OnFrameMove(this->m_dAppTime, this->m_dElapsedTime);
        static double dLastMusic = 0;
        dLastMusic -= m_dElapsedTime;
        if(dLastMusic <= 0)
        {
            for(int i=0;i<this->m_musics.size();i++)
            {
                if(m_musics[i])
                {
                    m_musics[i]->FrameMove();
                }
            }
            dLastMusic = 0.5f;
        }
		ActionBase* removelist[1024];
		int removeindex = 0;
		for(int i=0;i<m_actions.size();i++)
		{
			if(!m_actions[i]->OnFrameMove(this->m_dElapsedTime))
			{
				if(removeindex<1024)//最多一次删除1024个动作
					removelist[removeindex++] = m_actions[i];
			}
		}
		for(int i=0;i<removeindex;i++)
		{
			removelist[i]->Release();
		}
		
		OnFrameMove(m_dElapsedTime);
		return (!m_bExitGame);
	}
	VOID					Manager::RenderAllToShadowMap()
	{CPUTimeT(Manager);
		float fLength = m_shadowViewSize;//GetCameraPtr()->GetDistance() * m_shadowViewSize;
		if(m_shadowMap.BeginShadow(m_lightPos, this->m_lightDir, fLength))
		{
			for(int i=0;i<m_scene3ds.size();i++)
			{
				if(m_scene3ds[i]->IsEnabled(Scene3D::Enable_ShowScene))
				{
					m_scene3ds[i]->RenderToShadowMap();
					break;
				}
			}
			m_shadowMap.EndShadow();
			m_programType |= Program::Type_WithShadowMap;
		}
		else{
			if(m_programType&Program::Type_WithShadowMap)
				m_programType ^= Program::Type_WithShadowMap;
		}
		//this->RenderAllAlphas();
	}
	VOID					Manager::RenderAll3DObjects()
	{CPUTimeT(Manager);
		for(int i=0;i<m_scene3ds.size();i++)
		{
			if(m_scene3ds[i]->IsEnabled(Scene3D::Enable_ShowScene))
			{
				m_scene3ds[i]->RenderScene();
				break;
			}
		}
		this->RenderAllAlphas();
	}
	VOID					Manager::SetViewport(const rect_f* rc)
	{CPUTimeNone();
		if(rc){
#ifdef IPHONE_OS
			float fWidth = m_bufferWidth*2;
			float fHeight = m_bufferHeight*2;
#else
			float fWidth = m_bufferWidth;
			float fHeight = m_bufferHeight;
#endif
			m_device->SetViewport(rc->left*fWidth, rc->top*fHeight, (rc->right-rc->left)*fWidth, (rc->bottom-rc->top)*fHeight);
		}
		else{
#ifdef IPHONE_OS
			m_device->SetViewport(0, 0, m_bufferWidth*2, m_bufferHeight*2);
#else
			m_device->SetViewport(0, 0, m_bufferWidth, m_bufferHeight);
#endif
		}
	}
	BOOL					Manager::AppRender()
	{CPUTimeT(Manager);
		Lock lock(m_lockRender);
		if(!m_bDeviceSetup)
			return true;
		if(!m_bIsInitialized)
			return true;
		//是否启用阴影图渲染
		if(m_programType&Program::Type_WithShadowMap)
			m_programType ^= Program::Type_WithShadowMap;
		if(m_programType&Program::Type_ShadowUsePCF3)
			m_programType ^= Program::Type_ShadowUsePCF3;
		if(m_programType&Program::Type_ShadowUsePCF5)
			m_programType ^= Program::Type_ShadowUsePCF5;
		//
//#ifdef WIN32_OS
//		float f[] = {0.035647273, -0.99935853, -0.0034419298, 0.0, 0.4825082, 0.020227015, -0.87565786, 0.0, 0.87516576, 0.029554069, 0.48291975, 0.0, 0.0, 0.0, 0.0, 1.0};
//		matrix4x4 mat;
//		for(int i=0;i<16;i++)
//			mat.f[i] = f[i];
//		matrix4x4 mat2;
//		float fRot = PI*0.5;
//		matrixRotationZ(&mat2, fRot);
//		matrixMultiply(&mat, &mat, &mat2);
//		GetCameraPtr()->Set3DVRViewMatrix(mat);
//#endif
		m_renderIdx++;
		//
		m_numRenderTriangles	=	m_device->GetRenderTriangles();
		m_device->ResetRenderTriangles();
		m_nSetMVP		=	0;
		m_pSetMVP		=	0;
		m_shaderMode		=	RenderMode_Max;
		m_bShaderModeBlend	=	0;
		//
		m_device->ResetAllDefault();
		m_device->SetEnableZ(true);
		m_device->SetAlphaMode(AlphaMode_None);
		m_device->SetCullMode(Phantom::CullMode_CW);
		//
		SetRenderMode(Phantom::RenderMode_Color);
		this->SetUIDialogSize(true,this->m_gameWidth, this->m_gameHeight, 0);
		//for(int i=0;i<m_map2ds.size();i++)
		//	m_map2ds[i]->OnRender(m_dElapsedTime);
        if(m_shadowMapViewMode)
            m_device->Clear(CLEAR_TARGET | CLEAR_ZBUFFER);
        //m_shadowMapViewMode = true;
		if(this->m_shadowMap.IsInit()&&IsShadowEnable())
		{
			if(m_shadowPCF==1)
				m_programType |= Program::Type_ShadowUsePCF3;
			else if(m_shadowPCF==2)
				m_programType |= Program::Type_ShadowUsePCF5;
			RenderAllToShadowMap();//渲染所有对象到阴影贴图中
		}
		if(!m_shadowMapViewMode)
			m_device->Clear(CLEAR_TARGET | CLEAR_ZBUFFER);
		if(m_bVR3DViewer){//左屏幕
			m_camera->SetPerspectiveProjection(m_camera->GetFov(), m_bufferWidth/2, m_bufferHeight, m_camera->GetZNearest(), m_camera->GetZFar());
#ifdef IPHONE_OS
			m_device->SetViewport(0, 0, m_bufferWidth, m_bufferHeight*2);
#else
			m_device->SetViewport(0, 0, m_bufferWidth/2, m_bufferHeight);
#endif
			m_camera->Set3DVRViewMatrixEye(true);
		}
		else if(m_bVR3DSingleEye){
			m_camera->Set3DVRViewMatrix();
		}
		if(GetActiveSDK())
			GetActiveSDK()->OnRender(m_dElapsedTime);
		g_luamanager.Render();
		if(!m_shadowMapViewMode)
			RenderAll3DObjects();
		g_luamanager.OnRender(m_dElapsedTime);
		if(m_bVR3DViewer){//右屏幕
			this->m_renderIdx++;
			SetRenderMode(Phantom::RenderMode_Color);
#ifdef IPHONE_OS
			m_device->SetViewport(m_bufferWidth, 0, m_bufferWidth, m_bufferHeight*2);
#else
			m_device->SetViewport(m_bufferWidth/2, 0, m_bufferWidth/2, m_bufferHeight);
#endif
			m_camera->Set3DVRViewMatrixEye(false);
			m_device->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_INVSRCALPHA);
			m_device->SetAlphaMode(AlphaMode_None);
			ClearAlphaObjects();
			this->m_renderIdx++;
			this->m_shadowMap.m_shadowUpdateIdx++;
			if(!m_shadowMapViewMode)
				RenderAll3DObjects();
			g_luamanager.OnRender(m_dElapsedTime);
			//
		}
		if(m_bVR3DViewer){//恢复viewport
			m_camera->SetPerspectiveProjection(m_camera->GetFov(), m_bufferWidth, m_bufferHeight, m_camera->GetZNearest(), m_camera->GetZFar());
#ifdef IPHONE_OS
			m_device->SetViewport(0, 0, m_bufferWidth*2, m_bufferHeight*2);
#else
			m_device->SetViewport(0, 0, m_bufferWidth, m_bufferHeight);
#endif
		}
		//
		ClearAlphaObjects();
		//渲染界面,也需要进行VR渲染
		if(m_bRenderUI)
		{
			m_device->Clear(CLEAR_ZBUFFER);
			this->RenderAll3DDrawMgr();
			m_uiManager->Render(m_dAppTime, m_dElapsedTime, false);
			if(GetActiveSDK())
			{
				this->SetUIDialogSize(true,this->m_gameWidth, this->m_gameHeight, 0);
				GetActiveSDK()->OnRenderUI();
			}
			for(int i=0;i<m_textItems.size();i++)
			{
				Rect screen(m_textItems[i].x, m_textItems[i].y, m_textItems[i].x+m_bufferWidth, m_textItems[i].y+20);
				DrawTexts(m_textItems[i].text.t, screen, 0, m_textItems[i].size, 0, &m_textItems[i].c, DrawTextAlign_LEFT | DrawTextAlign_TOP);
			}
			if(m_bShowFPS)
			{
				char system[128] = {0};
#ifdef WIN32_OS
				strcpy(system, "Windows");
#elif defined(IPHONE_OS)
				strcpy(system, "IOS");
#elif defined(ANDROID_OS)
				strcpy(system, "Android");
#endif
				char buf[256];
				sprintf(buf, "FPS:%.02f Tri:%d Size:%d,%d OS:%s MEM:%.02fM", GetFPS(), GetRenderTriangles(), GetBufferWidth(), GetBufferHeight(), system, (float)Texture::s_maxImageBuffer/(1024.0f*1024.0f));
				color4 r;
				r.setRGBA(1,1,0,1);
				Rect screen(0, 0, GetUIDialogSizeW(), GetUIDialogSizeH());
				DrawTexts(buf, screen, 0, 12, 0, &r, DrawTextAlign_CENTER | DrawTextAlign_TOP);
			}
		}
		OnRender(m_dElapsedTime);
		return true;
	}
	VOID					Manager::CreateShadowMap(int textureSize)//是否开启动态阴影
	{CPUTimeT(Manager);
		this->m_shadowMap.Create(textureSize);
	}
	VOID					Manager::SetGameOverflow(BOOL bBlackOnOverflow)
	{CPUTimeNone();
		m_bGameOverflow = bBlackOnOverflow;
	}
	//界面缩放大小
	VOID					Manager::SetUIScale(float x, float y){CPUTimeNone();
		m_uiScale.setxy(x, y);
		m_uiScaleSave = m_uiScale;
		RecalcRenderRect();
	}
	BOOL					Manager::IsClipRender(const Rect& clientRc)
	{CPUTimeNone();
		if(clientRc.left>m_renderRect.right||clientRc.right<m_renderRect.left||clientRc.top>m_renderRect.bottom||clientRc.bottom<m_renderRect.top)
			return false;
		return true;
	}
	VOID					Manager::RecalcRenderRect()
	{CPUTimeT(Manager);
		int bufferW = GetBufferWidth();
		int bufferH = GetBufferHeight();
		int gameW = GetUIDialogSizeW();
		int gameH = GetUIDialogSizeH();
		gameW = (float)gameW*m_uiScale.x;
		gameH = (float)gameH*m_uiScale.y;
		int offsetW = (float)(bufferW-gameW)/m_uiScale.x/2;
		int offsetH = (float)(bufferH-gameH)/m_uiScale.y/2;
		m_renderRect.Set(-offsetW,-offsetH,offsetW*2+getmax(GetUIDialogSizeW(),bufferW), offsetH*2+getmax(GetUIDialogSizeH(),bufferH));//gameW+offsetW,gameH+offsetH);
	}
	VOID					Manager::PushText(RenderText* hr)
	{
		m_texts.push_back(hr);
	}
	VOID					Manager::PopText(RenderText* hr)
	{
		for(int i=0;i<m_texts.size();i++)
		{
			if(m_texts[i] == hr)
			{
				m_texts.eraseByIndex(i, 1, true);
				break;
			}
		}
	}
    int                     Manager::GetDeviceWidth()
    {CPUTimeNone();
        if(this->m_bufferWidth>this->m_bufferHeight)
        {
            if(this->m_deviceWidth>this->m_deviceHeight)
                return this->m_deviceWidth;
            return m_deviceHeight;
        }
        if(this->m_deviceWidth>this->m_deviceHeight)
            return this->m_deviceHeight;
        return m_deviceWidth;
    }
    int                     Manager::GetDeviceHeight()
    {CPUTimeNone();
        if(this->m_bufferWidth>this->m_bufferHeight)
        {
            if(this->m_deviceWidth>this->m_deviceHeight)
                return this->m_deviceHeight;
            return m_deviceWidth;
        }
        if(this->m_deviceWidth>this->m_deviceHeight)
            return this->m_deviceWidth;
        return m_deviceHeight;
    }

	VOID					Manager::AppResize(int width, int height, int deviceWidth, int deviceHeight)
	{CPUTimeT(Manager);
		if(!m_bIsInitialized)return;
        if(m_bufferWidth == width && m_bufferHeight == height)
            return;
		m_bufferWidth	=	width;//, height);
		m_bufferHeight	=	height;//, width);
        m_deviceWidth   =   deviceWidth;
        m_deviceHeight  =   deviceHeight;
		if(m_bDeviceSetup)
			m_device->SetViewport(0, 0, m_bufferWidth, m_bufferHeight);
		//m_uiDialogSize.Set(width,height);
		//SetUIDialogSize(m_bufferWidth,m_bufferHeight);//m_uiDialogSize.cx, m_uiDialogSize.cy);
		//SetUIScale((float)m_bufferWidth / (float)m_uiDialogSize.cx, (float)m_bufferHeight / (float)m_uiDialogSize.cy);
		if(GetActiveSDK())
			GetActiveSDK()->OnResize(width, height);
		LogInfo("system->AppResize(%d,%d  %d,%d  %.02f, %.02f, %.02f,%.02f)", width, height, m_uiDialogSize.cx, m_uiDialogSize.cy, this->m_uiOffset.x, this->m_uiOffset.y, this->m_uiScale.x, this->m_uiScale.y);
		//
		this->Set2DProjectionFov(m_f2DFov);
		this->m_camera->SetPerspectiveProjection(m_camera->GetFov(), width, height, m_camera->GetZNearest(), m_camera->GetZFar());
		GetDlgManager()->OnResize();
	}
	extern	VOID								CloseAllZip();
	VOID					Manager::AppDestroy()	//删除幻影使用
	{CPUTimeT(Manager);
		this->Stop();
		if(!m_bIsInitialized)
			return;
		m_bExitGame		=	true;
		OnLostDevice();
		m_bDeviceSetup	=	false;
		OnDestroy();
		SetActiveSDK(0);
		for(int i=0;i<m_loadTextures.size();i++)
			safe_release(m_loadTextures[i]);
		m_loadTextures.clear();
		//
		m_uiManager->Destroy();
		m_defaultENFont			=	0;
		m_defaultUnicodeFont	=	0;	//中文字体
		//
		for(int i=0;i<m_musics.size();i++)
			m_musics[i]->StopAll();
		m_printTexts.clear();
		m_textures.freeAll();//释放所有的图片
		CloseAllZip();
		m_bIsInitialized	=	false;
		//
		for(int i=0;i<m_musics.size();i++)
			delete m_musics[i];
		m_musics.clear();
	}
	VOID							Manager::GetCursorPos2D(Pixel& ret, unsigned int index)
	{CPUTimeNone();
		if(index >= MAX_TOUCH_COUNT)
			index = MAX_TOUCH_COUNT - 1;
		ret.Set((int)((float)(m_cursorPos[index].x - m_uiOffset.x) / m_uiScale.x), (int)((float)(m_cursorPos[index].y - m_uiOffset.y) / m_uiScale.y));
	}
	VOID							Manager::AppInputEvent(Phantom::InputEventID btn, int x, int y, unsigned int touchIndex)
	{CPUTimeT(Manager);
		if(!this->m_bIsInitialized)return;
		Lock lock(m_lockRender);
		//
        if(touchIndex >= MAX_TOUCH_COUNT)
            touchIndex = MAX_TOUCH_COUNT - 1;
		if(btn == Phantom::InputEventID_MouseMove)
		{
			if(this->m_cursorPos[touchIndex].x == x && this->m_cursorPos[touchIndex].y == y)
				return;
		}
		this->m_cursorPos[touchIndex].Set(x, y);
		if(btn == Phantom::InputEventID_ButtonDown)
			m_cursorDown[touchIndex] = 1;
		else if(btn == Phantom::InputEventID_ButtonUp)
			m_cursorDown[touchIndex] = 0;
		//处理界面事件
        g_luamanager.OnInputEvent(btn, x, y, touchIndex);

		Pixel ui((int)((float)(x - m_uiOffset.x) / m_uiScale.x), (int)((float)(y-m_uiOffset.y) / m_uiScale.y));
		if(m_uiManager->OnInputEvent(btn, ui, touchIndex))
		{
		}
		else
		{
			for(int i=0;i<m_scene3ds.size();i++)
			{
				if(m_scene3ds[i]->IsEnabled(Scene3D::Enable_EnableScene)&&m_scene3ds[i]->IsEnabled(Scene3D::Enable_ShowScene))
					m_scene3ds[i]->OnInputEvent(btn,x,y,touchIndex);
			}
			this->m_camera->OnInputEvent(btn, x, y, touchIndex);
			for(int i=0;i<m_map2ds.size();i++)
			{
				if(m_map2ds[i]->OnInputEvent(btn, x, y, touchIndex))
					return;
			}
			if(this->GetActiveSDK())
				GetActiveSDK()->OnInputEvent(btn, x, y, touchIndex);
		}
	}
	BOOL	Manager::LoadFont(FontPtr& ret, const char* szFile)	//fonts directory, like "fonts/abc.fig"
	{CPUTimeT(Manager);
		if(!szFile)
			return false;
		FileName f = szFile;
		f.setIsFile();
		for(int i=0;i<m_fonts.size();i++)
		{
			if(m_fonts[i]->GetFileName().isFile(f))
			{
				ret.set(m_fonts, m_fonts[i]->GetID());
				return true;
			}
		}
		if(!fexist(f))
		{
			LogInfo("FileNotExist(%s)", f.t);
			return false;
		}
		int id = 0;
		Font* v	=	m_fonts.addNew(&id, new classT<Font, classMgr<Font> >());	//添加自动删减功能
		if(!v->LoadFont(f.t))
		{
			safe_release(v);
			return false;
		}
		ret.set(m_fonts, id);
		return true;
	}
	BOOL	Manager::LoadMesh(AutoPtr<Mesh>& ret, const char* szFile, ImportResult* result)
	{CPUTimeT(Manager);
		MeshDataPtr mesh;
		if(!LoadMesh(mesh, szFile, result))
			return false;
		Mesh* g	=	mesh->CreateRuntime();
		ret	=	g;
		safe_release(g);
		return true;
	}
	BOOL	Manager::LoadMesh(MeshDataPtr& ret, const char* szFile, ImportResult* result)
	{CPUTimeT(Manager);
		if(!szFile)
			return false;
		FileName f = szFile;
		f.setIsFile();
		for(int i=0;i<m_meshGroupDatas.size();i++)
		{
			if(m_meshGroupDatas[i]->m_filename.isFile(f))
			{
				ret.set(m_meshGroupDatas, m_meshGroupDatas[i]->GetID());
				return true;
			}
		}
		int id = 0;
		classT<MeshData, classMgr<MeshData> >* c	=	new classT<MeshData, classMgr<MeshData> >();
		MeshData* v	=	m_meshGroupDatas.addNew(&id, c);	//添加自动删减功能
		if(!v->LoadGroupData(f.t, result))
		{
			safe_release(v);
			return false;
		}
		ret.set(m_meshGroupDatas, id);
		return true;
	}
	BOOL	Manager::CreateScene3D(Scene3DPtr& ret, float fGridSize/*格子大小1=0.1米*/, int nSceneSize/*场景大小2,4,8,16,32*/)//创建一个新的场景
	{CPUTimeT(Manager);
		int id = 0;
		classT<Scene3D, classMgr<Scene3D> >* c	=	new classT<Scene3D, classMgr<Scene3D> >();
		Scene3D* v	=	m_scenes.addNew(&id, c);	//添加自动删减功能
		if(!v->CreateScene(fGridSize, nSceneSize))
		{
			safe_release(v);
			return false;
		}
		ret.set(m_scenes, id);
		return true;
	}
	BOOL	Manager::LoadScene3D(Scene3DPtr& ret, const char* szFile)
	{CPUTimeT(Manager);
		if(!szFile)
			return false;
		FileName f = szFile;
		f.setIsFile();
		for(int i=0;i<m_scenes.size();i++)
		{
			if(m_scenes[i]->GetFileName().isFile(f))
			{
				ret.set(m_scenes, m_scenes[i]->GetID());
				return true;
			}
		}
		int id = 0;
		classT<Scene3D, classMgr<Scene3D> >* c	=	new classT<Scene3D, classMgr<Scene3D> >();
		Scene3D* v	=	m_scenes.addNew(&id, c);	//添加自动删减功能
		if(!v->LoadScene(f.t))
		{
			safe_release(v);
			return false;
		}
		ret.set(m_scenes, id);
		return true;
	}
	VOID	Manager::PushMap3D(Scene3D* m)
	{
		m_scene3ds.push_back(m);
	}
	VOID	Manager::PopMap3D(Scene3D* m)
	{
		for(int i=0;i<m_scene3ds.size();i++)
			if(m_scene3ds[i] == m)
			{
				m_scene3ds.eraseByIndex(i);
				break;
			}
	}
	VOID	Manager::PushRT(RenderTarget* m)
	{
		m_RTs.push_back(m);
	}
	VOID	Manager::PopRT(RenderTarget* m)
	{
		for(int i=0;i<m_RTs.size();i++)
			if(m_RTs[i] == m)
			{
				m_RTs.eraseByIndex(i);
				break;
			}
	}
	BOOL	Manager::LoadTexture(Texture** ret, const char* szFile, BOOL bGenMipmap, int nFilter, BOOL bLoadThread, BaseDirectory* basedir)
	{CPUTimeT(Manager);
		if(!szFile)
			return false;
		FileName f = szFile;
		f.setIsFile();
		for(int i=0;i<m_textures.size();i++)
		{
			if(m_textures[i]->GetFileName().isFile(f)&&m_textures[i]->m_basedir == basedir)
			{
				safe_addref(m_textures[i]);
				*ret = m_textures[i];
				return true;
			}
		}
		if(!m_bUseThread)bLoadThread = false;
		int id = 0;
		Texture* v	=	m_textures.addNew(&id, m_device->CreateTexture());	//添加自动删减功能
		if(!v->LoadTextureFromFile(f.t, bGenMipmap, false, nFilter, bLoadThread, basedir))
		{
			safe_release(v);
			return false;
		}
		safe_addref(v);
		*ret = v;
		return true;
	}
	BOOL	Manager::LoadTexture(TexturePtr& ret, const char* szFile, BOOL bGenMipmap, int nFilter, BOOL bLoadThread, BaseDirectory* basedir)
	{CPUTimeT(Manager);
		if(!szFile)
			return false;
		FileName f = szFile;
		f.setIsFile();
		for(int i=0;i<m_textures.size();i++)
		{
			if(m_textures[i]->GetFileName().isFile(f)&&m_textures[i]->m_basedir == basedir)
			{
				ret.set(m_textures, m_textures[i]->GetID());
				return true;
			}
		}
		if(!m_bUseThread)bLoadThread = false;
		int id = 0;
		Texture* v	=	m_textures.addNew(&id, m_device->CreateTexture());	//添加自动删减功能
		if(!v->LoadTextureFromFile(f.t, bGenMipmap, false, nFilter, bLoadThread, basedir))
		{
			safe_release(v);
			return false;
		}
		ret.set(m_textures, id);
		return true;
	}
	extern	MultiMusic*	CreateMusicByFile(const char* szFile);
	int						Manager::GetMusicFileIndex(const char* szFile)
	{CPUTimeNone();
		FileName f = szFile;
		f.setIsFile();
		for(int i=0;i<this->m_musics.size();i++)
		{
			if(m_musics[i]->m_fileName == f)
			{
				return i;
			}
		}
#ifndef EXPORT_ENGINE
		if(fexist(szFile))
		{
			MultiMusic* m = CreateMusicByFile(szFile);//new MultiMusic();
			if(!m)
				return 0;
			m->m_fileName	=	f.t;
			m_musics.push_back(m);
			return (m_musics.size()-1);
		}
#endif
		return -1;
	}
	MultiMusic*		Manager::GetMusicFilePtrByIndex(int nIndex)
	{CPUTimeNone();
		if(nIndex < 0 || nIndex >= m_musics.size())
			return 0;
		return m_musics[nIndex];
	}
	VOID			Manager::StopAllMusic()
	{CPUTimeNone();
		for(int i=0;i<this->m_musics.size();i++)
		{
			m_musics[i]->StopAll();
		}
	}
	MultiMusic*		Manager::GetMusicFilePtr(const char* szFile)
	{CPUTimeNone();
		FileName f = szFile;
		f.setIsFile();
		for(int i=0;i<this->m_musics.size();i++)
		{
			if(m_musics[i]->m_fileName == f)
			{
				return m_musics[i];
			}
		}
#ifndef EXPORT_ENGINE
		if(fexist(szFile))
		{
			MultiMusic* m = CreateMusicByFile(szFile);//new MultiMusic();
			if(!m)
				return 0;
			m->m_fileName	=	f.t;
			m_musics.push_back(m);
			return m;
		}
#endif
		return 0;
	}
	//
	VOID			Manager::DeleteMusicFileIndex(const char* szFile)	//删除音乐文件和所有内存使用
	{CPUTimeNone();
		FileName f = szFile;
		f.setIsFile();
		for(int i=0;i<this->m_musics.size();i++)
		{
			if(m_musics[i]->m_fileName == f)
			{
				m_musics[i]->DestroyAll();
				break;
			}
		}
	}
	VOID			Manager::CreateMesh(MeshPtr& r)
	{CPUTimeNone();
		this->m_bRenderMeshChanged	=	true;
		int id = 0;
		DeviceMesh* mesh	=	m_deviceMesh.addNew(&id, m_device->CreateMesh());
		r.set(m_deviceMesh, id);
	}
	VOID			Manager::PushMusic(Music2D* m)
	{
		m_backMusics.push_back(m);
	}
	VOID			Manager::PopMusic(Music2D* m)
	{
		for(int i=0;i<m_backMusics.size();i++)
			if(m_backMusics[i]==m)
				m_backMusics.eraseByIndex(i, 1, true);
	}
	Mesh*			Manager::SearchMesh(GameID& id)
	{CPUTimeT(Manager);
		if(id.id==0)return 0;
		if(id.index<this->m_meshs.size()&&m_meshs[id.index]->m_gameid.id==id.id)
			return m_meshs[id.index];
		for(int i=0;i<m_meshs.size();i++){
			if(m_meshs[i]->m_gameid.id==id.id){
				id.index = i;
				id.id = m_meshs[i]->m_gameid.id;
				return m_meshs[i];
			}
		}
		id.id=0;
		return 0;
	}
	VOID			Manager::PushMesh(Mesh* m)
	{
		m->m_gameid.id = m_meshid++;
		m->m_gameid.index = m_meshs.size();
		m_meshs.push_back(m);
	}
	VOID			Manager::PopMesh(Mesh* m)
	{
		for(int i=0;i<m_meshs.size();i++)
			if(m_meshs[i]==m)
				m_meshs.eraseByIndex(i, 1, true);
	}
	VOID			Manager::SetTexture(Texture* tex, TextureIndex nIndex)
	{CPUTimeNone();
		if(tex && m_currProg)
			m_currProg->SetTexture(nIndex, tex);
	}
	VOID			Manager::ReleaseAllTextureState()
	{CPUTimeT(Manager);
		for(int i=0;i<RenderMode_Max;i++)
		{
			if(m_progMode[i])
			{
				m_progMode[i]->SetTexture((TextureIndex)0, 0);
				m_progMode[i]->SetTexture((TextureIndex)1, 0);
				m_progMode[i]->SetTexture((TextureIndex)2, 0);
				m_progMode[i]->SetTexture((TextureIndex)3, 0);
				m_progMode[i]->SetTexture((TextureIndex)4, 0);
				m_progMode[i]->SetTexture((TextureIndex)5, 0);
				m_progMode[i]->SetTexture(Phantom::TextureIndex_5, 0);
			}
			if(m_progModeB[i])
			{
				m_progModeB[i]->SetTexture((TextureIndex)0, 0);
				m_progModeB[i]->SetTexture((TextureIndex)1, 0);
				m_progModeB[i]->SetTexture((TextureIndex)2, 0);
				m_progModeB[i]->SetTexture((TextureIndex)3, 0);
				m_progModeB[i]->SetTexture((TextureIndex)4, 0);
				m_progModeB[i]->SetTexture((TextureIndex)5, 0);
				m_progModeB[i]->SetTexture(Phantom::TextureIndex_5, 0);
			}
		}
	}
	VOID			Manager::SetWorldMatrix(const matrix4x4* world)
	{CPUTimeNone();
		if(!m_currProg)
			return;
		m_currProg->SetWorldMatrix(world);
	}
	VOID			Manager::Set2DProjectionFov(float f2DFov)	//设置2D的FOV值
	{CPUTimeNone();
		m_projection2D.PerspectiveFovRH(m_f2DFov = f2DFov, (float)this->m_bufferWidth / (float)this->m_bufferHeight, 0.99f, 1.01f);
		m_fTan2DFov	=	tanf(this->m_f2DFov / 2.0f) * 2.0f;
		matrixMultiply(&m_viewProjection2D, &m_view2D, &m_projection2D);
		m_matrixDef2D.identity();
		matrixMultiply(&m_matrixDef2D, &m_matrixDef2D, &m_viewProjection2D);
	}

	BOOL			Manager::Get2DFrom3D_2DProj(Pixel& _out,const float3& vPos) const
	{CPUTimeNone();
		float3 p;
		vPos.transform(p, this->m_view2D);
		//
		if(p.z>-0.001f)
			return false;
		float finv = 1.0f/-p.z;
		p.x *= finv;
		p.y *= finv;
		//
		_out.x = (int)((p.x* this->m_projection2D._11/2.0f+0.5f)*(float)this->m_bufferWidth);
		_out.y = (int)((0.5f - p.y * m_projection2D._22/2.0f)*(float)this->m_bufferHeight);
		//
		if(_out.x<-m_bufferWidth||_out.y<-m_bufferHeight)
			return false;
		if(_out.x>m_bufferWidth||_out.y>m_bufferHeight)
			return false;
		return true;
	}
	void			Manager::Get3DFrom2D_2DProj(float3& _orig, float3& _dir, const Pixel& _in) const
	{CPUTimeNone();
		float3 v;
		v.x =  ( (float)_in.x / (float)m_bufferWidth-0.5f )*2.0f / m_projection2D._11;
		v.y =	 ( 0.5f-(float)_in.y / (float)m_bufferHeight )*2.0f / m_projection2D._22;
		v.z =  -1.0f;
		const matrix4x4& m = m_view2DInv;
		_dir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
		_dir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
		_dir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
		_orig.x = m._41;
		_orig.y = m._42;
		_orig.z = m._43;
		_dir.normalv(_dir);
	}
	unsigned char*	g_encrypt = 0;
	int				g_encrypt_len = 0;
	VOID	SetEncryptData(unsigned char* crypt, int len)
	{
		g_encrypt = crypt;
		g_encrypt_len = len;
	}
	extern	GenuineGUID g_phantomGUID;
	unsigned char*	ReadLock(streamRead& r, char* head, unsigned char* buf, int size, int& newsize, int& begin)
	{
#ifndef EXPORT_ENGINE
		Phantom::EncryptVal val;
		int len = sizeof(Phantom::EncryptVal);
		int version = 1;
		char head2[4];
		r.read(head2, 4);
		for(int i=0;i<4;i++)
		{
			if(head2[i] != head[i])
				return 0;
		}
		r >> version;
		unsigned int orig = 0;
		unsigned int compsize = 0;
		r.read(&orig, sizeof(unsigned int));
		r.read(&compsize, sizeof(unsigned int));
		unsigned char* newbuf = new unsigned char[orig+1];
		uLongf orig2 = orig;
		uncompress(newbuf, &orig2, buf + r.m_point, compsize);
		newbuf[orig] = 0;
		newsize = orig2 - sizeof(Phantom::RandomVal) - sizeof(Phantom::EncryptVal);
		Phantom::RandomVal rnd;
		int index = 0;
		memcpy(&rnd, newbuf, sizeof(Phantom::RandomVal));
		index += sizeof(Phantom::RandomVal);
		memcpy(&val, newbuf + index, sizeof(Phantom::EncryptVal));
		index += sizeof(Phantom::EncryptVal);
		if(!DecryptRandomVal(rnd, g_phantomGUID))
			return 0;
		if(!DecryptData((char*)newbuf + index, orig - index, rnd, val))
			return 0;
		//newsize = size - r.m_point;
		begin = index;
		return newbuf;
		//Phantom::EncryptVal val;
		//int len = sizeof(Phantom::EncryptVal);
		//int version = 1;
		//char head2[4];
		//r.read(head2, 4);
		//for(int i=0;i<4;i++)
		//{
		//	if(head2[i] != head[i])
		//		return 0;
		//}
		//r >> version;
		//Phantom::RandomVal rnd;
		//r >> rnd;
		//r >> val;
		//if(!DecryptRandomVal(rnd, g_phantomGUID))
		//	return 0;
		//if(!DecryptData((char*)buf + r.m_point, size - r.m_point, rnd, val))
		//	return 0;
		//newsize = size - r.m_point;
		//return buf+r.m_point;
#else
		return 0;
#endif
	}
	VOID			Uncompress(char* newbuf, int& origsize, const char* inbuf, int length)
	{
#ifndef EXPORT_ENGINE
		uLongf orig2 = origsize;
		uncompress((Bytef*)newbuf, &orig2, (Bytef*)inbuf, length);
		origsize = orig2;
#endif
	}
	unsigned char*	DecryptMemoryFile(const char* szExt, unsigned char* buf, int size, int& newsize, int& begin)
	{
#ifndef EXPORT_ENGINE
		newsize = size;
		streamRead r((char*)buf, size);
		if(stricmp(szExt, ".lui") == 0)
		{
			char head[4] = {'l','o','c','u'};
			return ReadLock(r, head, buf, size, newsize, begin);
		}
		else if(stricmp(szExt, ".lmesh") == 0)
		{
			char head[4] = {'l','o','c','m'};
			return ReadLock(r, head, buf, size, newsize, begin);
		}
		if(stricmp(szExt, ".pui") == 0)
		{
			char head[4] = {'p','o','c','u'};
			return ReadLock(r, head, buf, size, newsize, begin);
		}
		else if(stricmp(szExt, ".pmesh") == 0)
		{
			char head[4] = {'p','o','c','m'};
			return ReadLock(r, head, buf, size, newsize, begin);
		}
		else if(stricmp(szExt, ".lscene") == 0)
		{
			char head[4] = {'l','s','t','o'};
			return ReadLock(r, head, buf, size, newsize, begin);
		}
		else if(stricmp(szExt, ".lsc") == 0)
		{
			char head[4] = {'l','o','c','l'};
			return ReadLock(r, head, buf, size, newsize, begin);
		}
		else if(stricmp(szExt, ".lt") == 0)
		{
			char head[4] = {'l','o','c','t'};
			return ReadLock(r, head, buf, size, newsize, begin);
		}
		else if(size > 4 && strnicmp((char*)buf, "comp", 4) == 0)
		{
			char fhead[128];
			r.read(fhead, 4);
			unsigned int orig = 0;
			unsigned int compsize = 0;
			r.read(&orig, sizeof(unsigned int));
			r.read(&compsize, sizeof(unsigned int));
			unsigned char* newbuf = new unsigned char[orig+1];
			uLongf orig2 = orig;
			uncompress(newbuf, &orig2, buf + r.m_point, compsize);
			newbuf[orig] = 0;
			newsize = orig2;
			begin = 0;
			return newbuf;
		}
		return buf;
		//if(g_encrypt)
		//{
		//	if(stricmp(szExt, ".limg") == 0 || stricmp(szExt, ".lui") == 0)
		//	{
		//		int maxCrypt = 10000;
		//		if(size < maxCrypt)
		//			maxCrypt = size;
		//		for(int i=0;i<maxCrypt;i++)
		//		{
		//			buf[i] ^= g_encrypt[i % g_encrypt_len];
		//		}
		//	}
		//}
#else
		newsize = size;
		begin = 0;
		return buf;
#endif
	}
	VOID		Manager::PushMap(MyMap2D* m)
	{
		m_map2ds.push_back(m);
	}
	VOID		Manager::PopMap(MyMap2D* m)
	{
		for(int i=0;i<m_map2ds.size();i++)
			if(m_map2ds[i]==m)
			{
				m_map2ds.eraseByIndex(i);
				break;
			}
	}
	VOID		Manager::SetMute(BOOL b)
	{CPUTimeNone();
		m_bMute = b;
		if(!m_bMute){
			for(int i=0;i<m_backMusics.size();i++)
			{
				if(m_backMusics[i]->IsPlaying())
					m_backMusics[i]->SetVolume(m_backMusics[i]->GetVolume());
			}
			for(int i=0;i<m_musics.size();i++)
			{
				m_musics[i]->ResumeAll();
			}
		}
		else
		{
			for(int i=0;i<m_backMusics.size();i++)
			{
				if(m_backMusics[i]->IsPlaying())
					m_backMusics[i]->SetMute();
			}
			for(int i=0;i<m_musics.size();i++)
			{
				m_musics[i]->PauseAll();
			}
		}
	}

	VOID		Manager::AddAction(ActionBase* a)
	{CPUTimeNone();
		if(!a)return;
		safe_addref(a);
		m_actions.push_back(a);
	}
	VOID		Manager::RemoveAction(ActionBase* a)
	{CPUTimeNone();
		for(int i=0;i<m_actions.size();i++)
		{
			if(m_actions[i] == a)
			{
				safe_release(m_actions[i]);
				m_actions.eraseByIndex(i);
				break;
			}
		}
	}
};

using namespace Phantom;
