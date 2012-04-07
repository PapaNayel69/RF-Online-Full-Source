#ifndef __CPATHFINDER_H__
#define __CPATHFINDER_H__

#include "C_Vector.h"
#include "windows.h"

//--패스 파인더.
#define _MAX_FIND_PATH	10	// 최대 가는 방법.
#define _MAX_PATH_POS	16	//최대 패스 꺾임


typedef struct{
	float TotalLeng;
	Vector3f Path[_MAX_PATH_POS+1];
	int WhatDirection;	//어느 방향.?
	WORD FrontLinetId;
	WORD BackLineId;
	int PathCnt;
	BOOL IsFind;	//찾았냐?
}_PATH_NODE;


class CPathFinder{
public:
	int mMaxDepth;
	_PATH_NODE mPathNode[_MAX_FIND_PATH+2];
	Vector3f mStart;
	_PATH_NODE mStackPathNode[_MAX_FIND_PATH+10];
	int mStackPoint;
//	int stColFaceId;	//충돌 페이스 아이디

	CPathFinder()					{	memset(this,0,sizeof(CPathFinder));	}

public:
	BOOL AddPath(Vector3f pos,int id);
	void CopyPath(int tar,int src)			{	memcpy(&mPathNode[tar],&mPathNode[src],sizeof(_PATH_NODE));	}
	void PushPathStack(int id);	//갈라질곳의 패스를 스택에 넣는다.
	void PopPathStack(int id);	//갈라질곳의 패스를 스택에 꺼낸다.

	void CompletePath(int id)				{		mPathNode[id].IsFind = TRUE;	}//앗싸 길찾았다.
	int GetPathCnt(int id)					{		return mPathNode[id].PathCnt;	}
	void SetPathDirection(int id,int dir)	{		mPathNode[id].WhatDirection = dir;		}
	int GetPathDirection(int id)			{		return mPathNode[id].WhatDirection;	}
	void SetFrontLineId(int id,int l_id)	{		mPathNode[id].FrontLinetId = l_id;	}
	void SetBackLineId(int id,int l_id)		{		mPathNode[id].BackLineId = l_id;	}
	WORD GetFrontLineId(int id)				{		return mPathNode[id].FrontLinetId;	}
	WORD GetBackLineId(int id)				{		return mPathNode[id].BackLineId;	}
};

#endif
