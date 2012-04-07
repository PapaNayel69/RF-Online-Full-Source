#include "R3Render.h"
#include "R3Alpha.h"
#include "jmalloc.h"
#include "jerror.h"
#include "R3Bsp.h"
#include "b_sort.h"

CAlpha::CAlpha()
{
	mAlphaFace=(DWORD *)Dmalloc(_MAX_ALPHA_FACE*sizeof(DWORD));
//	mAlphaFaceZ=(float *)Dmalloc(_MAX_ALPHA_FACE*sizeof(float));
	mAlphaFaceZ=(SHORT *)Dmalloc(_MAX_ALPHA_FACE*sizeof(SHORT));

	mTempAlphaFace=(DWORD *)Dmalloc(_MAX_ALPHA_FACE*sizeof(DWORD));
	mTempAlphaFaceZ=(SHORT *)Dmalloc(_MAX_ALPHA_FACE*sizeof(SHORT));
	
	mAlphaFaceCnt = 0;
	mAlphaSize=_MAX_ALPHA_FACE;
}

CAlpha::~CAlpha()
{
	Dfree(mTempAlphaFaceZ);
	Dfree(mTempAlphaFace);
	Dfree(mAlphaFaceZ);
	Dfree(mAlphaFace);
}

void CAlpha::InitAlpha(void* bsp)
{
	mBsp = bsp;
}


void CAlpha::LoopInitAlphaStack(void)	//알파 리스트 스텍 초기화
{
	mAlphaFaceCnt = 0;
}

void CAlpha::CheckAlphaAlloc(void)	//알파 alloc
{
	if(mAlphaFaceCnt>=(DWORD)mAlphaSize)//_MAX_ALPHA_FACE)
	{
		int Size =mAlphaSize/2+mAlphaSize;
		mAlphaFace=(DWORD *)ReAlloc(mAlphaFace,mAlphaSize*sizeof(DWORD),Size*sizeof(DWORD));
		mAlphaFaceZ=(SHORT *)ReAlloc(mAlphaFaceZ,mAlphaSize*sizeof(SHORT),Size*sizeof(SHORT));
		mTempAlphaFace=(DWORD *)ReAlloc(mTempAlphaFace,mAlphaSize*sizeof(DWORD),Size*sizeof(DWORD));
		mTempAlphaFaceZ=(SHORT *)ReAlloc(mTempAlphaFaceZ,mAlphaSize*sizeof(SHORT),Size*sizeof(SHORT));
		mAlphaSize=Size;
	}
}


void CAlpha::SetAlphaStack(WORD group_id)	//맵에서의 스태틱알파 폴리곤.
{
	mAlphaFace[mAlphaFaceCnt++]=_ALPHA_TYPE_M_GROUP|group_id;
	CheckAlphaAlloc();
}

DWORD CAlpha::GetAlphaFaceCnt()
{
	return mAlphaFaceCnt;
}
DWORD *CAlpha::GetAlphaFace()
{
	return mAlphaFace;
}

DWORD *CAlpha::SortAlphaStack(Vector3f camera)	//여지껏 알파스택에 쌓인것을 정렬한뒤 정렬된버퍼를 넘겨준다.
{
	DWORD i;
	CBsp *bsp = (CBsp *)mBsp;
	Vector3f total;

	for(i=0; i<mAlphaFaceCnt; i++)
	{
		if( mAlphaFace[i]&_ALPHA_TYPE_ENTITY)
		{
			Vector3fCopy(bsp->mMapEntitiesList[(mAlphaFace[i]&0x0000ffff)].Pos,total);
		}
		else
		{
			float add_dist=bsp->GetMatGroupPoint((WORD)(mAlphaFace[i]&0x0000ffff),total);
		}
		Vector3fSub(total,camera,total);
		//Vector3fMul(total,3,total);
//		mAlphaFaceZ[i] = Vector3fDist(total)/(float)bsp->face[f].vertex_num;	//점들의 합한 거리로 비교하자..
		mAlphaFaceZ[i] = (SHORT)Vector3fDist(total);//+add_dist;
	}

//	udword *sorted=NULL;
//	if( mAlphaFaceCnt != 0 )
//		sorted = mSortBase.Sort(mAlphaFaceZ, mAlphaFaceCnt).GetIndices();
//	return (DWORD *)sorted;


	if( mAlphaFaceCnt != 0 )
		 ByteSortForShort(mAlphaFaceCnt,mAlphaFace,mAlphaFaceZ,mTempAlphaFace,mTempAlphaFaceZ);
	else
		return NULL;

	return mAlphaFace;
}

//----앗싸 코로나.. 코로나도 소팅을 해야쥐...
void CAlpha::SetCoronaStack(WORD group_id)
{
	mAlphaFace[mAlphaFaceCnt++]=_ALPHA_TYPE_CORONA|group_id;
	CheckAlphaAlloc();
}

//----앗싸 알파 엔티티.. 알파 엔티티도 소팅을 해야쥐...
void CAlpha::SetAlphaEntityStack(WORD entities_id)
{
	mAlphaFace[mAlphaFaceCnt++]=_ALPHA_TYPE_ENTITY|entities_id;
	CheckAlphaAlloc();
}
