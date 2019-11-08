//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   PhantomGame.h
软件名称   :   幻影游戏引擎

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___PHANTOMGAME__H___
#define ___PHANTOMGAME__H___
#include "PhantomGameObject.h"

namespace Phantom{
	class Scene3D;
	class	GamePower{
	public:
		GamePower();
	public:
		CDynamicArray<GameObject*>	m_objects;
		int							m_powerID;
		int							m_enemyID;
		GamePower*					m_enemeyPower;//敌对方势力
		ConfigLine*					m_data;
	protected:
		friend class				GameMgr;
	};
	class	GameMgr: public ConfigMgr{
	public:
		GameMgr();
		virtual ~GameMgr();
		inline	Scene3D*			GetScenePtr(){return m_scenePtr;}
		GameObject*					Search(GameID& id, BOOL* bReset = 0);
		GameObject*					CreateObject(int lineid);//创建角色，从roles表格中的行id
		VOID						DeleteObject(GameObject* obj);
		VOID						Reborn(GameObject* obj, float fRebornTime);//重新复活等待
		//VOID						CreateAllObjects();//创建所有角色
		int							AddFileIndex(const FileName& f);//文件名
		VOID						OnFrameMove(double dElapsedTime);//计算所有游戏对象
		//外部调用发送消息
		enum RecvType{
			RecvType_ToGame = 0,//发送到游戏对象,GameObject
			RecvType_ToScene,//发送到场景对象,SceneObject
		};
		VOID						send(GameMessage::MID messageID, const GameSceneID& id, const void* buff, unsigned short length);
		BOOL						CreateScene(int sceneLineID, LPCSTR szLuaFunc);//创建游戏系统
		VOID						ResetAllObjects();
		VOID						DestroyScene();
		inline	ConfigLine*			GetLeaderLine(){return m_leaderLine;}
		inline	Config*				GetLeaderTable(){return m_leaderTable;}
		inline	GameLeader*			GetLeaderPtr(){return m_leader;}
		inline	int					GetReloadCount(){return m_reloadCount;}
		VOID						GotoPlot(int plotid);//-1下一个，-2结束
		VOID						SetPlotEnable(BOOL b);
		short						m_plotid;
		char						m_bPlotInit;
		char						m_bPlotEnable;
		VOID						RebuildInScenes();//重新构造在场对象列表
		//
		VOID						AddLoadTexture(Texture* tex);//要载入的图片列表
		void						OnLoadFrameMove();
	public:
		CDynamicArray<GameObject*>				m_objects;//游戏对象列表
		CDynamicArray<GameObject*>				m_inobjects;//可选游戏对象列表
		CDynamicArray<ScriptBase*>				m_scripts;//脚本列表
		CDynamicArray<GamePower*>				m_powerList;//势力列表
		CDynamicArray<FileName>					m_filelist;
		GameID						m_cameraTarget;//跟随镜头的角色
		BOOL						m_cameraFollow;//跟随镜头的角色
		CDynamicArray<Texture*>					m_loadlist;
		int										m_loadCount;
	protected:
		Config*						m_leaderTable;//主角属性表数据
		ConfigLine*					m_leaderLine;//主角属性表行
		GameLeader*					m_leader;
		ConfigLine*					m_sceneLine;//当前激活的场景行ID
		int							m_reloadCount;
		Name						m_funcLoading;
		//
		VOID						OnCheckEnemyIn();//
		VOID						AddToPower(GameObject* obj);
		VOID						RemoveFromPower(GameObject* obj);
		//
		bool						AddScripts(ScriptBase::eScript type, CDynamicArray<ScriptBase*>& scripts, const char* config, const char* func, int loadID=-1);
		//
		BOOL						LoadScene(int nLineID);//从表格进入场景
		BOOL						ResetAllDatas(int sceneID);//载入所有脚本
		BOOL						CreateLeader();//创建主角数据
		//
		GameMessage*				alloc();//分配数据块
		void						recycle(GameMessage* msg);//回收数据块
		CDynamicArrayS<GameMessage*,100>	m_frees;//空闲消息数据
		CDynamicArrayS<GameMessage*,100>	m_allocs;//空闲消息数据
		CDynamicArrayS<GameMessage*,100>	m_recvs;//接收的消息数据
		CDynamicArrayS<GameMessage*,100>	m_tempRecvs;//接收的消息数据,当前处理
		void						onMessage(GameMessage* msg);//处理游戏消息接收
		//
		AutoPtr<Scene3D>			m_scenePtr;//游戏场景指针
		BOOL						m_bCreated;//游戏系统是否有效
		BOOL						m_bInitLua;//游戏脚本是否已经初始化
		friend class				GameObject;
	};
	GameMgr*	GetGameMgr();
};

#endif
