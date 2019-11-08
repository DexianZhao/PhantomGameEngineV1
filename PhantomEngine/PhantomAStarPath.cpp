//#include "stdafx.h"
#include "PhantomAStarPath.h"

namespace Phantom{

AstarfindPath::AstarfindPath()
{
	m_numPaths = 0;
	m_numPathAlloc = 0;
	m_paths = 0;
}
AstarfindPath::~AstarfindPath()
{
	if(this->m_paths)
		free(m_paths);
	m_paths = 0;
}
void		AstarfindPath::set_numPath(int numPath)
{
	if(m_numPathAlloc<numPath)
	{
		m_numPathAlloc = numPath;
		if(m_paths)
			m_paths = (Pixel*)realloc(m_paths,m_numPathAlloc*sizeof(Pixel));
		else
			m_paths = (Pixel*)malloc(m_numPathAlloc*sizeof(Pixel));
		memset(m_paths,0,m_numPathAlloc*sizeof(Pixel));
	}
	this->m_numPaths = numPath;
}
Pixel*	AstarfindPath::getPath(int index)
{
	if(index<0 || index>=m_numPaths)
		return NULL;
	return &m_paths[index];
}

AStarPath::AStarPath()//i_astar_cb* pMap, int forbiddenTiles)
{
	m_pStackPtr = ( AstarStack* )malloc(sizeof( AstarStack ));
	memset(m_pStackPtr,0,sizeof(AstarStack));
	m_bIsPath		= FALSE;
	m_pOpenPointPtr		= NULL;
	m_pClosePtr		= NULL;
	m_pPathPtr		= NULL;
	m_mapPtr	= 0;
	m_findDepth = 10240;
	this->m_width	=	this->m_height	=	0;
	m_bCalcNearest	=	true;
	m_startX		=	m_startY	=	0;
	m_bFindIsFirst	=	false;
	m_bIs2DMode		=	false;
}

////////////////////////////////////////////////////////////////////////////////

void	AStarPath::destroy()
{
	freeModes();
	m_bIsPath = FALSE;
	if(m_pOpenPointPtr)
		free(m_pOpenPointPtr);
	if(m_pClosePtr)
		free(m_pClosePtr);
	m_pOpenPointPtr = NULL;
	m_pClosePtr = NULL;
	m_pPathPtr = NULL;
}

AStarPath::~AStarPath()
{
	free(m_pStackPtr);
	destroy();
}

void AStarPath::setupMap(AstarCallback* pMap)
{
	m_mapPtr	=	pMap;
	//m_width		= pMap->getPlanetGridCount().cx;
	//m_height	= pMap->getPlanetGridCount().cy;
	if(m_mapPtr)
	{
		m_width		=	m_mapPtr->GetAstarWidth();
		m_height	=	m_mapPtr->GetAstarHeight();
	}
}

////////////////////////////////////////////////////////////////////////////////

BOOL AStarPath::NewPath(int sx,int sy, int dx,int dy)
{
	m_bFindIsFirst	=	false;
	if ( FreeTile(dx,dy) && /*FreeTile(sx,sy) && */(TileNum(sx,sy)!=TileNum(dx,dy)) )
	{
		m_bFindIsFirst	=	true;
		freeModes();
		if(findPath(sx,sy,dx,dy))
			return (m_bIsPath=TRUE);
	}
	return (m_bIsPath=FALSE);
}

////////////////////////////////////////////////////////////////////////////////

BOOL AStarPath::ReachedGoal(void)
{
	if ( !m_bIsPath ) return TRUE;
	if ( m_pPathPtr->parentNode != NULL )
		return FALSE;
	else
		return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

int AStarPath::TileNum(int x, int y)
{
	return (y*m_width + x);
}

////////////////////////////////////////////////////////////////////////////////

int AStarPath::FreeTile(int x, int y)
{
	if(x<0||y<0||x>=m_width||y>=m_height || !this->m_mapPtr)return 0;
	if(m_startX == x && m_startY == y)return 1;
	int nMove = m_mapPtr->IsAstarMoveEnable(x, y);
	if(!this->m_bIs2DMode){
		if(nMove == 1)
		{
			m_bFindIsFirst	=	false;
			return nMove;
		}
		if(m_bFindIsFirst)
			return 1;
	}
	return nMove;
}

int		AStarPath::getGrid(int x, int y)
{
	if(x<0||y<0||x>=m_width||y>=m_height)return 0;
	if(m_startX == x && m_startY == y)return 1;
	return m_mapPtr->IsAstarMoveEnable(x, y);
}

void AStarPath::freeModes(void)
{
	PosNode *Node, *OldNode;

	if ( m_pOpenPointPtr != NULL )
	{
		Node = m_pOpenPointPtr->NextNode;
		while ( Node != NULL )
		{
			OldNode = Node;
			Node = Node->NextNode;
			free(OldNode);
		}
		m_pOpenPointPtr->NextNode = 0;
	}

	if ( m_pClosePtr != NULL )
	{
		Node = m_pClosePtr->NextNode;
		while ( Node != NULL )
		{
			OldNode = Node;
			Node = Node->NextNode;
			free(OldNode);
		}
		m_pClosePtr->NextNode = 0;
	}
}

BOOL AStarPath::findPath(int sx, int sy, int dx, int dy)
{
	PosNode *Node, *BestNode;
	int TileNumDest;

	TileNumDest = TileNum(sx, sy);

	if(!m_pOpenPointPtr)
	{
		m_pOpenPointPtr=( PosNode* )malloc(sizeof( PosNode ));
		memset(m_pOpenPointPtr,0,sizeof(PosNode));
	}
	if(!m_pClosePtr)
	{
		m_pClosePtr=( PosNode* )malloc(sizeof( PosNode ));
		memset(m_pClosePtr,0,sizeof(PosNode));
	}

	Node=( PosNode* )malloc(sizeof( PosNode ));
	memset(Node,0,sizeof(PosNode));
	Node->g = 0;
	Node->h = (dx-sx)*(dx-sx) + (dy-sy)*(dy-sy);
	Node->f = Node->g+Node->h;
	Node->nodeCount = TileNum(dx, dy);
	Node->x = dx;
	Node->y = dy;

	m_pOpenPointPtr->NextNode=Node;
	int iDepth=0;
	for (iDepth=0;iDepth<=m_findDepth;iDepth++)
	{
		BestNode=returnBestNode();
		if(!BestNode)
			return FALSE;
		if (BestNode->nodeCount == TileNumDest)
			break;
		generateSuccessors(BestNode,sx,sy);
	}
	if(iDepth<m_findDepth)
	{
		m_pPathPtr = BestNode;
		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////

AStarPath::PosNode
*AStarPath::returnBestNode(void)
{
	PosNode *tmp;
	if ( m_pOpenPointPtr->NextNode == NULL )
	{
		return NULL;
	}
	tmp = m_pOpenPointPtr->NextNode;
	m_pOpenPointPtr->NextNode = tmp->NextNode;
	tmp->NextNode = m_pClosePtr->NextNode;
	m_pClosePtr->NextNode = tmp;

	return(tmp);
}

////////////////////////////////////////////////////////////////////////////////

void AStarPath::generateSuccessors(PosNode *BestNode, int dx, int dy)
{
	int x, y;

	// Upper-Left
	if ( m_bIs2DMode==false && FreeTile(x=BestNode->x-TILESIZE, y=BestNode->y-TILESIZE) )
		generateSucc(BestNode,x,y,dx,dy);
	// Upper
	if ( FreeTile(x=BestNode->x, y=BestNode->y-TILESIZE) )
		generateSucc(BestNode,x,y,dx,dy);
	// Upper-Right
	if ( m_bIs2DMode==false && FreeTile(x=BestNode->x+TILESIZE, y=BestNode->y-TILESIZE) )
		generateSucc(BestNode,x,y,dx,dy);
	// Right
	if ( FreeTile(x=BestNode->x+TILESIZE, y=BestNode->y) )
		generateSucc(BestNode,x,y,dx,dy);
	// Lower-Right
	if ( m_bIs2DMode==false && FreeTile(x=BestNode->x+TILESIZE, y=BestNode->y+TILESIZE) )
		generateSucc(BestNode,x,y,dx,dy);
	// Lower
	if ( FreeTile(x=BestNode->x, y=BestNode->y+TILESIZE) )
		generateSucc(BestNode,x,y,dx,dy);
	// Lower-Left
	if ( m_bIs2DMode==false && FreeTile(x=BestNode->x-TILESIZE, y=BestNode->y+TILESIZE) )
		generateSucc(BestNode,x,y,dx,dy);
	// Left
	if ( FreeTile(x=BestNode->x-TILESIZE, y=BestNode->y) )
		generateSucc(BestNode,x,y,dx,dy);
}

////////////////////////////////////////////////////////////////////////////////

void AStarPath::generateSucc(PosNode *BestNode,int x, int y, int dx, int dy)
{
	int g, TileNumS, c = 0;
	PosNode *Old, *Successor;

	g = BestNode->g+1;
	TileNumS = TileNum(x,y);
	if ( (Old=checkOpen(TileNumS)) != NULL )
	{
		for( c = 0; c < 8; c++)
			if( BestNode->childNode[c] == NULL )
				break;
		BestNode->childNode[c] = Old;

		if ( g < Old->g )
		{
			Old->parentNode = BestNode;
			Old->g = g;
			Old->f = g + Old->h;
		}
	}
	else if ( (Old=checkClosed(TileNumS)) != NULL )
	{
		for( c = 0; c< 8; c++)
			if ( BestNode->childNode[c] == NULL )
				break;
		BestNode->childNode[c] = Old;

		if ( g < Old->g )
		{
			Old->parentNode = BestNode;
			Old->g = g;
			Old->f = g + Old->h;
			propagateDown(Old);
		}
	}
	else
	{
		Successor = ( PosNode* )malloc(sizeof( PosNode ));
		memset(Successor,0,sizeof(PosNode));
		Successor->parentNode = BestNode;
		Successor->g = g;
		Successor->h = (x-dx)*(x-dx) + (y-dy)*(y-dy);
		Successor->f = g+Successor->h;
		Successor->x = x;
		Successor->y = y;
		Successor->nodeCount = TileNumS;
		Insert(Successor);
		for( c =0; c < 8; c++)
			if ( BestNode->childNode[c] == NULL )
				break;
		BestNode->childNode[c] = Successor;
	}
}

////////////////////////////////////////////////////////////////////////////////

AStarPath::PosNode
*AStarPath::checkOpen(int tilenum)
{
	PosNode *tmp;

	tmp = m_pOpenPointPtr->NextNode;
	while ( tmp != NULL )
	{
		if ( tmp->nodeCount == tilenum )
			return (tmp);
		else
			tmp = tmp->NextNode;
	}
	return(NULL);
}

////////////////////////////////////////////////////////////////////////////////

AStarPath::PosNode
*AStarPath::checkClosed(int tilenum)
{
	PosNode *tmp;

	tmp = m_pClosePtr->NextNode;

	while ( tmp != NULL )
	{
		if ( tmp->nodeCount == tilenum )
			return(tmp);
		else
			tmp = tmp->NextNode;
	}
	return(NULL);
}

////////////////////////////////////////////////////////////////////////////////

void AStarPath::Insert(PosNode *Successor)
{
	PosNode *tmp1, *tmp2;
	int f;

	if ( m_pOpenPointPtr->NextNode == NULL )
	{
		m_pOpenPointPtr->NextNode = Successor;
		return;
	}
	f = Successor->f;
	tmp1 = m_pOpenPointPtr;
	tmp2 = m_pOpenPointPtr->NextNode;

	while ( (tmp2 != NULL) && (tmp2->f < f) )
	{
		tmp1 = tmp2;
		tmp2 = tmp2->NextNode;
	}

	Successor->NextNode = tmp2;
	tmp1->NextNode = Successor;
}

////////////////////////////////////////////////////////////////////////////////

void AStarPath::propagateDown(PosNode *Old)
{
	int c, g;
	PosNode *childNode, *Father;

	g = Old->g;
	for ( c = 0; c < 8; c++)
	{
		if ( (childNode=Old->childNode[c]) == NULL )
			break;
		if ( g+1 < childNode->g)
		{
			childNode->g = g+1;
			childNode->f = childNode->g + childNode->h;
			childNode->parentNode = Old;
			Push(childNode);
		}
	}

	while ( m_pStackPtr->pNextStackPtr != NULL )
	{
		Father = Pop();
		for (c = 0; c<8; c++)
		{
			if ( (childNode=Father->childNode[c]) == NULL )
				break;
			if ( Father->g+1 < childNode->g )
			{
				childNode->g = Father->g+1;
				childNode->f = childNode->g+childNode->h;
				childNode->parentNode = Father;
				Push(childNode);
			}
		}
	}
}

void AStarPath::Push(PosNode *Node)
{
	AstarStack *tmp;

	tmp =( AstarStack* )malloc(sizeof( AstarStack ));
	tmp->pNodePtr = Node;
	tmp->pNextStackPtr = m_pStackPtr->pNextStackPtr;
	m_pStackPtr->pNextStackPtr = tmp;
}

////////////////////////////////////////////////////////////////////////////////

AStarPath::PosNode
*AStarPath::Pop(void)
{
	PosNode *tmp;
	AstarStack *tmpSTK;

	tmpSTK = m_pStackPtr->pNextStackPtr;
	tmp = tmpSTK->pNodePtr;

	m_pStackPtr->pNextStackPtr = tmpSTK->pNextStackPtr;
	free(tmpSTK);
	return(tmp);
}

int		AStarPath::CalcNumPath()
{
	PosNode* pTemp = m_pPathPtr;
	int numPath = 0;
	PosNode* pparentNodeTemp = 0;
	while(pTemp)
	{
		numPath++;
		pTemp = pTemp->parentNode;
		if(!pTemp)
			break;
	}
	return numPath;
}


bool	AStarPath::find(AstarfindPath& _out,int startX, int startY,int targetX, int targetY)
{
	bool bAddStart = false;
	int	beginOffsetX = 0;
	int beginOffsetY = 0;
	if(FreeTile(startX, startY) == 0)
	{
		bAddStart	=	true;
		int minValue[4] = {1000, 1000, 1000, 1000};
		for(int x=0;x<=1000;x++)
		{
			if(FreeTile(x + startX, startY) == 0)
				continue;
			minValue[0]	=	x;
			break;
		}
		for(int x=0;x>=-1000;x--)
		{
			if(FreeTile(x + startX, startY) == 0)
				continue;
			minValue[1]	=	x;
			break;
		}
		for(int y=0;y<=1000;y++)
		{
			if(FreeTile(startX, y + startY) == 0)
				continue;
			minValue[2]	=	y;
			break;
		}
		for(int y=0;y>=-1000;y--)
		{
			if(FreeTile(startX, y + startY) == 0)
				continue;
			minValue[3]	=	y;
			break;
		}
		int minIndex = 0;
		int maxValue = 1000;
		for(int i=0;i<4;i++)
		{
			if(abs(minValue[i]) < maxValue)
			{
				maxValue = abs(minValue[i]);
				minIndex	=	i;
			}
		}
		switch(minIndex)
		{
		case 0:
			beginOffsetX	=	startX + maxValue;
			beginOffsetY	=	startY;
			break;
		case 1:
			beginOffsetX	=	startX - maxValue;
			beginOffsetY	=	startY;
			break;
		case 2:
			beginOffsetX	=	startX;
			beginOffsetY	=	startY + maxValue;
			break;
		case 3:
			beginOffsetX	=	startX;
			beginOffsetY	=	startY - maxValue;
			break;
		}
	}
	BOOL bPath = FALSE;
	if(!bAddStart)
		bPath = NewPath(startX,startY,targetX,targetY);
	else
		bPath = NewPath(beginOffsetX,beginOffsetY,targetX,targetY);
	if(bPath)
	//if(this->NewPath(startX,startY,targetX,targetY))
	{
		int num = CalcNumPath();
		_out.set_numPath(num);
		PosNode* pTemp = m_pPathPtr;
		int index = 0;
		PosNode* pparentNodeTemp = 0;
		while(pTemp)
		{
			_out.getPath(num - index - 1)->x = pTemp->x;
			_out.getPath(num - index - 1)->y = pTemp->y;
			//
			//if(this->m_bIs2DMode&&!this->m_mapPtr->IsAstarMoveEnable(pTemp->x, pTemp->y))
			//	return false;
			index++;
			pTemp = pTemp->parentNode;
			if(!pTemp)
				break;
		}
		//
		//
		return true;
	}
	return false;
}

bool	AStarPath::find_linear(int startX, int startY,int targetX, int targetY, BOOL bIs2DMode)
{
	m_bIs2DMode	=	bIs2DMode;
	//if(isLineCanGo(startX,startY,targetX,targetY))
	//{
	//	m_findWay.set_numPath(1);
	//	m_findWay.getPath(0)->x = targetX;
	//	m_findWay.getPath(0)->y = targetY;
	//	return true;
	//}
	m_startX	=	startX;
	m_startY	=	startY;
	if(bIs2DMode)
	{
		return find(m_findWay, startX, startY, targetX, targetY);
	}
	else if(find(this->m_tempFind, startX, startY, targetX, targetY))
	{
		m_findWay.m_numPaths	=	0;
		return calc_linear(m_findWay, m_tempFind);
	}
	return false;
}

bool	AStarPath::isLineCanGo(int startX, int startY,int targetX, int targetY)
{
	if(!m_bCalcNearest)
		return false;
	if(m_bIs2DMode)
	{
		if(startX == targetX || startY == targetY)
			return true;
		return false;
	}
	else
	{
		float fSX = (float)startX;
		float fSY = (float)startY;
		float fTX = (float)targetX;
		float fTY = (float)targetY;
		//
		float fDX = fTX - fSX;
		float fDY = fTY - fSY;
		//
		float fRadius = sqrtf(fDX*fDX + fDY*fDY)*2.0f;
		//
		int	num = (int)(fRadius);
		//
		float fDDX = fDX/fRadius;
		float fDDY = fDY/fRadius;
		//
		for(int i=0;i<num;i++)
		{
			int x = (int)(((float)i*fDDX + fSX));
			int y = (int)(((float)i*fDDY + fSY));
			//
			if(!this->FreeTile(x,y))
				return false;
			//
		}
	}
	//
	return true;
	//
}
bool	AStarPath::calc_linear(AstarfindPath& _out,AstarfindPath& _in)
{
	int numPath = _in.getNumPath();
	_out.set_numPath(numPath);
	int index = 0;
	int dx = 0;
	int dy = 0;
	//
	//
	for(int i=0;i<numPath;i++)
	{
		if(_in.getPath(i)->x == 500000)
			continue;
		for(int j=numPath-1;j>i+1;j--)
		{
			if(_in.getPath(j)->x == 500000)
				continue;
			//
			Pixel* pfrom = _in.getPath(i);
			Pixel* pto = _in.getPath(j);
			//
			if(isLineCanGo(pto->x,pto->y,pfrom->x,pfrom->y))
			{
				for(int idel = i+1;idel<j;idel++)
				{
					_in.getPath(idel)->x = _in.getPath(idel)->y = 500000;
				}
			}
			//
		}
	}
	Pixel* p = _out.getPath(0);
	Pixel* p1 = _in.getPath(index);
	int nowPush = 0;
	for(int i=0;i<numPath-1;i++)
	{
		if(p1[i].x!=500000)
		{
			p[nowPush] = p1[i];
			nowPush++;
		}
	}
	//
	//for(int i=0;i<numPath;i++)
	//{
	//	//
	//	*_out.getPath(pushIndex) = *_in.getPath(i);
	//	pushIndex++;
	//	//
	//	//
	//	//
	//}
	//
	_out.set_numPath(nowPush);
	//
	if(nowPush>0)
	{
	}
	//
	return (nowPush!=0);
}

bool					AStarPath::findway(CDynamicArray<float2>& rets, const float2& from, const float2& to, float fGridSizeW, float fGridSizeH)
{
	INT nGridSX		=	(INT)(from.x / fGridSizeW);
	INT nGridSY		=	(INT)(from.y / fGridSizeH);
	INT nGridEX		=	(INT)(to.x / fGridSizeW);
	INT nGridEY		=	(INT)(to.y / fGridSizeH);
	if(!find_linear(nGridSX, nGridSY, nGridEX, nGridEY))
		return false;
	if(this->getfindPath().getNumPath() == 1)
	{
		//rets.push_back( &to );
		return true;
	}
	for(int i=getfindPath().getNumPath() - 1; i > 0; i--)
	{
		Pixel* p	=	getfindPath().getPath(i);
		float2 v( (float)p->x * fGridSizeW + fGridSizeW/2, (float)p->y * fGridSizeH + fGridSizeH/2 );
		rets.push_back( &v );
	}
	rets.push_back( &to );
	return true;
}

};
