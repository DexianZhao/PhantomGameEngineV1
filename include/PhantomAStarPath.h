//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
文件       :   AStarPath.h
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
Copyright 2009-2016 Zhao Dexian
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___PHANTOM_ASTAR__PATH_H___
#define ___PHANTOM_ASTAR__PATH_H___
#include "PhantomBase.h"

#define SHIFT 0
#define TILESIZE 1

namespace Phantom{
	struct WayPoint2D{
		float2				pos;
		float2				dir;
		float				length;
		float				fAngle;
		VOID				Setup(const float2& _pos){pos = _pos;}
	};
	class AstarfindPath
	{
	public:
		void				set_numPath(int numPath);
		int					getNumPath(){return m_numPaths;}
		Pixel*				getPath(int index);
		//
		AstarfindPath();
		~AstarfindPath();
	protected:
		//
		friend class		AStarPath;
		Pixel*				m_paths;
		int					m_numPaths;
		int					m_numPathAlloc;
	};

	class	AstarCallback
	{
	public:
		virtual	int				IsAstarMoveEnable(int x, int y) = 0;	//是否可以移动到这里
		virtual	int				GetAstarWidth() = 0;					//地形宽度
		virtual	int				GetAstarHeight() = 0;				//地形高度
	};
	class AStarPath
	{
	public:
		AStarPath();
		~AStarPath();
		//
		void 					setupMap(AstarCallback* pMap);
		//
		void					destroy();
		//
		bool					find_linear(int startX, int startY,int targetX, int targetY, BOOL bIs2DMode = false);
		AstarfindPath&			getfindPath(){return m_findWay;}
		//
		bool					calc_linear(AstarfindPath& _out, AstarfindPath& _in);
		//
		inline	int				getFindDepth(){return m_findDepth;}
		inline	void			setFindDepth(int depth){m_findDepth = depth;}
		//格子单元设置
		bool					setGrid(int x, int y, int value);
		int						getGrid(int x, int y);
		bool					isLineCanGo(int startX, int startY, int targetX, int targetY);

		bool					findway(CDynamicArray<float2>& rets, const float2& from, const float2& to, float fGridSizeW, float fGridSizeH);
		bool					m_bCalcNearest;
	protected:
		BOOL 					NewPath(int sx, int sy, int dx, int dy);

		BOOL 					ReachedGoal(void);
		void 					PathNextNode(void) { m_pPathPtr=m_pPathPtr->parentNode; }
		int 					NodeGetX(void)      { return m_pPathPtr->x; }
		int 					NodeGetY(void)      { return m_pPathPtr->y; }
		//
		int						CalcNumPath();
		//
		int 					TileNum(int x, int y);
		int 					FreeTile(int x, int y);
		//
		AstarfindPath			m_tempFind;
		//
	protected:
		//
		bool					find(AstarfindPath& _out,int startX, int startY,int targetX, int targetY);
		//
		struct PosNode {
			int						f, h;
			int						g, tmpg, x, y, nodeCount;
			PosNode *				parentNode;
			PosNode *				childNode[8];
			PosNode *				NextNode;
		};
		PosNode *					m_pOpenPointPtr;
		PosNode *					m_pClosePtr;
		PosNode *					m_pPathPtr;
		struct AstarStack{
			PosNode *				pNodePtr;
			AstarStack *			pNextStackPtr;
		};
		AstarStack *				m_pStackPtr;

		BOOL						m_bIsPath;
		int 						m_width, m_height;
		AstarCallback*				m_mapPtr;

		int							m_findDepth;//最大查找深度
		AstarfindPath				m_findWay;

		//bz_linePoint2D				m_linePoint;
		int							m_startX, m_startY;
		bool						m_bFindIsFirst;
		BOOL						m_bIs2DMode;
	private:
		void 					freeModes(void);
		BOOL 					findPath(int sx, int sy, int dx, int dy);
		PosNode 		*		returnBestNode(void);
		void 					generateSuccessors(PosNode *BestNode, int dx, int dy);
		void 					generateSucc(PosNode *BestNode,int x, int y, int dx, int dy);
		PosNode 		*		checkOpen(int tilenum);
		PosNode 		*		checkClosed(int tilenum);
		void 					Insert(PosNode *Successor);

		void 					propagateDown(PosNode *Old);

		void					Push(PosNode *Node);
		PosNode 		*		Pop(void);
	};
};

#endif
