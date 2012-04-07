#include "Collision.h"
#include "CollisionUtil.h"
#include "R3Math.h"
#include "Jmalloc.h"

CCollision::CCollision()
{
	mFNormalList=NULL;//페이스 노멀.
	mVList=NULL;//버텍스 리스트
	mFaceNum=0;
}

CCollision::~CCollision()
{
	if( mFNormalList )
	{
		Dfree(mFNormalList);
	}
}
void CCollision::InitColision(Vector4f *vlist,int face_num)
{
	mFNormalList=(Vector4f *)Dmalloc(face_num*sizeof(Vector4f));

	mVList = vlist;
	mFaceNum = face_num;
	for(int i=0; i<face_num; i++)
	{
		GetNormal(mFNormalList[i],vlist[i*3]
			,vlist[i*3+1],vlist[i*3+2]);
	}
}

BOOL CCollision::CheckCollision(Vector3f char_pos,Vector3f old_pos,Vector3f new_pos)
{
	Vector3f cross;
	BOOL ret=FALSE;
	int i;
/*
	for( i=0; i<mFaceNum; i++)
	{
		mVList[i*3+0][0] += char_pos[0];
		mVList[i*3+0][1] += char_pos[1];
		mVList[i*3+0][2] += char_pos[2];

		mVList[i*3+1][0] += char_pos[0];
		mVList[i*3+1][1] += char_pos[1];
		mVList[i*3+1][2] += char_pos[2];

		mVList[i*3+2][0] += char_pos[0];
		mVList[i*3+2][1] += char_pos[1];
		mVList[i*3+2][2] += char_pos[2];
	}
*/
	old_pos[0]-=char_pos[0];
	old_pos[1]-=char_pos[1];
	old_pos[2]-=char_pos[2];
	new_pos[0]-=char_pos[0];
	new_pos[1]-=char_pos[1];
	new_pos[2]-=char_pos[2];
	for( i=0; i<mFaceNum; i++)
	{
		//평면을 가로지르는가?
		if( GetPlaneCrossPoint(new_pos, old_pos, cross, mFNormalList[i],mFNormalList[i][3],0) == FALSE ) 
			continue;
		
		//에지 첵크
		if( CheckEdge( mVList[i*3+0] , mVList[i*3+1] , cross ,  mFNormalList[i],mFNormalList[i][3] ) == 0)	
			continue;
		if( CheckEdge( mVList[i*3+1] , mVList[i*3+2] , cross ,  mFNormalList[i],mFNormalList[i][3] ) == 0)	
			continue;
		if( CheckEdge( mVList[i*3+2] , mVList[i*3+0] , cross ,  mFNormalList[i],mFNormalList[i][3] ) == 0)	
			continue;
		ret=TRUE;
		break;
	}
	old_pos[0]+=char_pos[0];
	old_pos[1]+=char_pos[1];
	old_pos[2]+=char_pos[2];
	new_pos[0]+=char_pos[0];
	new_pos[1]+=char_pos[1];
	new_pos[2]+=char_pos[2];

	return ret;
}

void CCollision::ReleaseCollision()
{
	if( mFNormalList )
	{
		Dfree(mFNormalList);
	}
}

