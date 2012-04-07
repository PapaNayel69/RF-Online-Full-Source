#ifndef _CSWORDTRACE_H__
#define _CSWORDTRACE_H__

#define _MAX_TRACE_CNT	60
#define _MAX_REAL_TRACE_CNT	10

#include "r3d3dtex.h"
#include "r3math.h"
#include "entity.h"


class CSwordTrace{
	int mCnt;	//
	int mQueue;		//이건 큐다.	큐의 시작 포인트
	DWORD mColor;		//잔상의 컬러다.
	Vector3f mStartPos[_MAX_TRACE_CNT];
	Vector3f mEndPos[_MAX_TRACE_CNT];
	
	int mRealCnt;	//
	Vector3f mRealStartPos[_MAX_REAL_TRACE_CNT];	//실제지나간 궤적 버퍼..
	Vector3f mRealEndPos[_MAX_REAL_TRACE_CNT];		//실제 지나간 궤적 버퍼.

	BOOL mIsTrace;
	LONG mIsRealTrace;
public:
	CSwordTrace();
	void SetTraceState(BOOL trace);	//칼베는 동작 사용할때 TRUE 끝날때 FALSE..
	BOOL IsTrace();	//현재 궤적을 그리는 중인지...
	BOOL IsDraw();	//현재 궤적을 그리는 중인지...
	BOOL IsRealTrace() {	return (mIsRealTrace);	};	//내부적으로 완전 트레이스가 끝난상태인가?
	void InsertSwordTraceQueue(Vector3f s_pos,Vector3f e_pos);
	void UpdateSwordTrace();
	void SetTraceColor(DWORD color){ mColor = color; };
	void SetDisable()	{	mCnt=0;	};	//뭔가 문제가 있어서 트레이스를 완전 없앨 때..
	CEntity *mEntity;	// 텍스쳐를 위한 엔티티를 가지고 있어라.
};

#endif