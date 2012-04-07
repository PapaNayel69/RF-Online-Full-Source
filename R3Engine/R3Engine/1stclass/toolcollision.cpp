#include <stdio.h>
#include "Jmalloc.h"
#include "Jerror.h"
#include "ToolCollision.h"
#include "r3bsp.h"
#include "r3math.h"
#include "collision.h"
#include "core.h"

//--------------------충돌 폴리곤 에디트 부분.
DWORD CF_VERSION=100;
static int stColFaceId;	//충돌 페이스 아이디

CToolCollisionFace::CToolCollisionFace()
{
	mColPoint=NULL;
	mColLine=NULL;
	mNormal=NULL;
	mVNormal=NULL;
	mCOLVNORMALMAXCNT=7000;
	mCOLPOINTMAXCNT=7000;
	mCOLLINEMAXCNT=7000;
	mColPointCnt=1;		//0번배열은 안쓴다.
	mColLineCnt=1;
	mBeforeVId=0;
	mOnePointCreate=FALSE;
	mbContinueLine = FALSE;
	mVNormalCnt=0;
}

CToolCollisionFace::~CToolCollisionFace()
{
	if( mColPoint )
		Dfree(mColPoint);
	if( mColLine )
		Dfree(mColLine);
	if( mNormal )
		Dfree(mNormal);
	if( mVNormal )
		Dfree(mVNormal);
	mColPoint=NULL;
	mColLine=NULL;
	mCOLVNORMALMAXCNT=7000;
	mCOLPOINTMAXCNT=7000;
	mCOLLINEMAXCNT=7000;
	mColPointCnt=1;
	mColLineCnt=1;
	mBeforeVId=0;
	mOnePointCreate=FALSE;
}

BOOL CToolCollisionFace::GetVertex(Vector3f *get,WORD id)
{
	if( id < mColPointCnt)
	{
		get[0][0] = mColPoint[id].v[0];
		get[0][1] = mColPoint[id].v[1];
		get[0][2] = mColPoint[id].v[2];
		return TRUE;
	}
	get[0][0] = 0;
	get[0][1] = 0;
	get[0][2] = 0;

	return FALSE;
}

WORD CToolCollisionFace::CreateVertex(Vector3f v)	//리턴값이 생성된 id값이다.
{
	if( mColPoint == NULL )	//초기 생성.
	{
		mColPoint=(_TOOL_COL_POINT *)Dmalloc(mCOLPOINTMAXCNT*sizeof(_TOOL_COL_POINT));
		mColPointCnt=1;
	}
	if( mColPointCnt == 65535)
	{
		Warning("충돌점 최대갯수 65536개를 넘었습니다.","");
		return 0;
	}
	mColPoint[mColPointCnt].v[0]=v[0];
	mColPoint[mColPointCnt].v[1]=v[1];
	mColPoint[mColPointCnt].v[2]=v[2];
	mColPointCnt++;
	
	if(mColPointCnt >= mCOLPOINTMAXCNT)	//용량이 커지면 재할당.
	{
		mColPoint=(_TOOL_COL_POINT *)ReAlloc(mColPoint,mCOLPOINTMAXCNT*sizeof(_TOOL_COL_POINT),(mCOLPOINTMAXCNT+1000)*sizeof(_TOOL_COL_POINT));
		mCOLPOINTMAXCNT+=1000;
	}
	return (WORD)mColPointCnt-1;
}


WORD CToolCollisionFace::GetSearchLineIndex(WORD v_id)
{
	int i;

	for(i=1; i<mColLineCnt; i++)
	{
		if(	mColLine[i].end_v == v_id )
			return (WORD)i;

	}
	return 0;
}

int stHeight = 50;

void SetHeight(int su)
{
	stHeight=su;
}
int GetHeight()
{
	return stHeight;
}

void CToolCollisionFace::SetBeforeId(WORD v_id)
{
	mBeforeVId = v_id;
}
int CToolCollisionFace::GetBeforeId()
{
	return mBeforeVId;
}

WORD CToolCollisionFace::CreateLine(WORD v_id,BOOL b_ontinue_line)	//리턴값이 생성된 id값이다.
{
	if( mColLine == NULL )	//초기 생성.
	{
		mColLine=(_TOOL_COL_LINE *)Dmalloc(mCOLLINEMAXCNT*sizeof(_TOOL_COL_LINE));
		memset(mColLine,0,mCOLLINEMAXCNT*sizeof(_TOOL_COL_LINE));
		mNormal=(Vector4f *)Dmalloc(mCOLLINEMAXCNT*sizeof(Vector4f));
		mColLineCnt=1;
	}
	if( mColLineCnt >= 65535)
	{
		Warning("충돌점 최대갯수 65536개를 넘었습니다.","");
		return 0;
	}
	if( b_ontinue_line )
	{
		if( mBeforeVId==0 )
		{
			Warning("b_ontinue_line 라인에디터 에러. 장언일에게 문의바람.","");
		}
		else
		{
			mColLine[mColLineCnt].start_v=mBeforeVId;
			mColLine[mColLineCnt].end_v = v_id;
			WORD l_index = GetSearchLineIndex(mBeforeVId);
			mColLine[mColLineCnt].back = l_index;
			mColLine[mColLineCnt].front = 0;
			mColLine[mColLineCnt].attr = 0;
			mColLine[mColLineCnt].height = (float)stHeight;
			if( l_index )	//앞쪽 연결
			{
				mColLine[l_index].front = mColLineCnt;
			}
			GetLineNormal(&mNormal[mColLineCnt],&mColLine[mColLineCnt]);
			mColLineCnt++;
			mBeforeVId = v_id;	//현재 에디팅인덱스를 넣어줘야 다음에 이어서 쓴다.
		}
		mOnePointCreate=FALSE;
	}
	else	//첫번째는 너어줘야 된다.
	{
		mBeforeVId=v_id;
		mColLine[mColLineCnt].start_v=v_id;
		mColLine[mColLineCnt].end_v = 0;
		mColLine[mColLineCnt].front=0;
		mColLine[mColLineCnt].back = 0;
		mColLine[mColLineCnt].height = (float)stHeight;
		mOnePointCreate=TRUE;
	}
	
	if(mColLineCnt >= mCOLLINEMAXCNT)	//용량이 커지면 재할당.
	{
		mColLine=(_TOOL_COL_LINE *)ReAlloc(mColLine,mCOLLINEMAXCNT*sizeof(_TOOL_COL_LINE),(mCOLLINEMAXCNT+1000)*sizeof(_TOOL_COL_LINE));
		mNormal=(Vector4f *)ReAlloc(mNormal,mCOLLINEMAXCNT*sizeof(Vector4f),(mCOLLINEMAXCNT+1000)*sizeof(Vector4f));

//		memset(&mColLine[mColLineCnt],0
//			,(mCOLLINEMAXCNT+200)*sizeof(_TOOL_COL_LINE)-mCOLLINEMAXCNT*sizeof(_TOOL_COL_LINE));

		mCOLLINEMAXCNT+=1000;
	}
	FixLinkFace();	//페이스 링크 에러 첵크해서 고친다.
	return (WORD)mColLineCnt-1;
}

void CToolCollisionFace::DeleteCurLine()	
{
	if( mColLine == NULL )
		return;
//	mColLine[mColLineCnt-1].start_v=mBeforeVId;
	mBeforeVId = mColLine[mColLineCnt-1].start_v;
	if( mColLineCnt > 1 )
		mColLineCnt--;
	AdjustVertex();

	if( mColPointCnt==1 )
		mbContinueLine = FALSE;
	else
		mbContinueLine = TRUE;
	if( mColPointCnt  <= mBeforeVId )	//버그 수정..
	{
		mbContinueLine=FALSE;
	}
}

float CToolCollisionFace::GetFaceHeght(WORD f_id)
{
	return mColLine[f_id].height;
}

BOOL CToolCollisionFace::GetContinueLine()
{
	return mbContinueLine;
}
void CToolCollisionFace::SetContinueLine(BOOL state)
{
	mbContinueLine = state;
}

void CToolCollisionFace::AdjustVertex()		//노는 버텍스를 없앤다.
{
	int i,j;

	BOOL is_del=FALSE;
	for(i=mColPointCnt-1; i>0; i--)
	{
		BOOL is_exist = FALSE;
		for(j=1; j<mColLineCnt; j++)
		{    
			if( mColLine[j].start_v == i || mColLine[j].end_v == i )
			{
				is_exist = TRUE;
				break;
			}
		}
		if( is_exist == FALSE )
		{
			DeleteVertex((WORD)i);
			is_del =TRUE;
			break;
		}
	}
//		mOnePointCreate=TRUE;
	if( is_del )	//재귀함수..
		AdjustVertex();
}

void CToolCollisionFace::AdjustFace()		//링크안된 면을 고친다.
{
	int i,j;

	for(i=1; i<mColLineCnt; i++)
	{
		BOOL is_exist = FALSE;
		for(j=1; j<mColLineCnt; j++)
		{    
			if( mColLine[i].start_v == mColLine[j].end_v )
			{
				if( mColLine[i].back != j )	//교정해주자..
					mColLine[i].back = j;
			}
			if( mColLine[j].start_v == mColLine[i].end_v )
			{
				if( mColLine[i].front != j )	//교정해주자..
					mColLine[i].front = j;
			}
		}
	}
}
void CToolCollisionFace::AdjustFixFaceLink()	//링크 잘못된 면들을 고친다.
{
	int i,j;

	for(i=1; i<mColLineCnt; i++)
	{
		BOOL is_right = TRUE;
		if( mColLine[i].front )
		{
			if( mColLine[mColLine[i].front].start_v	!= mColLine[i].end_v )//링크 잘못됨.
			{
				//올바른 링크를 찾는다.
				for(j=1; j<mColLineCnt; j++)
				{    
					if( mColLine[j].start_v == mColLine[i].end_v )
					{
						mColLine[i].front = j;	//교정해주자..
						break;
					}
				}
				if( j==mColLineCnt )	//음 없군.
				{
					//결국 끊긴거다.
					mColLine[i].front=0;
				}
			}
		}
		if( mColLine[i].back )
		{
			if( mColLine[mColLine[i].back].end_v	!= mColLine[i].start_v )//링크 잘못됨.
			{
				//올바른 링크를 찾는다.
				for(j=1; j<mColLineCnt; j++)
				{    
					if( mColLine[j].end_v == mColLine[i].start_v )
					{
						mColLine[i].back = j;	//교정해주자..
						break;
					}
				}
				if( j==mColLineCnt )	//음 없군.
				{
					//결국 끊긴거다.
					mColLine[i].back=0;
				}
			}
		}
	}
}

BOOL CToolCollisionFace::DeleteVertex(WORD id)		//지우기 성공시 true
{
	int i;

	for(i=1; i<mColLineCnt; i++)
	{
		if( mColLine[i].start_v > id )
			mColLine[i].start_v--;
		if( mColLine[i].end_v > id )
			mColLine[i].end_v--;
	}
	memcpy(&mColPoint[id],&mColPoint[id+1],(mColPointCnt-id-1)*sizeof(_TOOL_COL_POINT));
	mColPointCnt--;

	return TRUE;
}
BOOL CToolCollisionFace::DeleteLine(WORD id)		//지우기 성공시 true
{
	int i;

	for(i=1; i<mColLineCnt; i++)
	{
		if( mColLine[i].front == id )
			mColLine[i].front=0;
		if( mColLine[i].back == id )
			mColLine[i].back = 0;
		if( mColLine[i].front > id )
			mColLine[i].front--;
		if( mColLine[i].back > id )
			mColLine[i].back--;
	}
	memcpy(&mColLine[id],&mColLine[id+1],(mColPointCnt-id-1)*sizeof(_TOOL_COL_LINE));
	memcpy(&mNormal[id],&mNormal[id+1],(mColPointCnt-id-1)*sizeof(Vector4f));
	mColLineCnt--;
	return TRUE;
}

// R3 render에 있다.
//void CToolCollisionFace::DrawCollisionPoly()//충돌 폴리곤을 뿌린다.

#define _COL_EPSILON	2
#define CmpVEpsilon(a,b,ep)	(( a[0]+ep > b[0] && a[0]-ep < b[0] )&& ( a[1]+ep > b[1] && a[1]-ep < b[1] )&& ( a[2]+ep > b[2] && a[2]-ep < b[2] ))


WORD CToolCollisionFace::IsExistPoint(Vector3f pos)	//리턴값이 0이면 생성불가
{
	int i;
	Vector3f from,normal,cmp;

    D3DXMATRIX m;
    D3DXMatrixInverse( &m, NULL, &GetR3State()->mMatView );

    from[0] = m._41;
    from[1] = m._42;
    from[2] = m._43;
	Vector3fSub(pos,from,normal);
	Normalize(normal);

	for(i=1; i<mColPointCnt; i++)
	{
		Vector3fSub(mColPoint[i].v,from,cmp);
		Normalize(cmp);
		float gac=Vector3fDot(cmp,normal);
		if( gac > 0.9999)
			return (WORD)i;
		if( CmpVEpsilon(mColPoint[i].v,pos,_COL_EPSILON))
		{
			return (WORD)i;
		}
	}
	return 0;
}
int CToolCollisionFace::GetColLineCnt()
{
	return mColLineCnt;
}
int CToolCollisionFace::GetColPointCnt()
{
	return mColPointCnt;
}

BOOL CToolCollisionFace::SaveCF(char *name)
{
	if( mColPoint == NULL )
		return FALSE;
	FILE *fp = fopen(name,"wb");

	if( fp == NULL )
	{
		Warning(name,"<-저장할수 없습니다.");
		return FALSE;
	}
	AdjustFixFaceLink();	//잘못된 링크를 바로 잡는다.
	fwrite(&CF_VERSION,4,1,fp);	
	fwrite(&mColPointCnt,4,1,fp);	
	fwrite(mColPoint,mColPointCnt*sizeof(_TOOL_COL_POINT),1,fp);	

	fwrite(&mColLineCnt,4,1,fp);	
	fwrite(mColLine,mColLineCnt*sizeof(_TOOL_COL_LINE),1,fp);	
	fclose(fp);
	return TRUE;
}

BOOL CToolCollisionFace::LoadCF(char *name)
{
	FILE *fp = fopen(name,"rb");

	if( fp == NULL )
	{
		Warning(name,"<-열수가 없습니다.");
		return FALSE;
	}
	if( mColPoint )
		Dfree(mColPoint);
	if( mColLine )
		Dfree(mColLine);
	if( mNormal )
		Dfree(mNormal);
	if( mVNormal )
		Dfree(mVNormal);
	mNormal=NULL;
	mVNormal=NULL;
	mColPoint=NULL;
	mColLine=NULL;
	mBeforeVId=0;
	mVNormalCnt=0;
	mOnePointCreate=FALSE;

	fread(&CF_VERSION,4,1,fp);	
	fread(&mColPointCnt,4,1,fp);	

	mCOLVNORMALMAXCNT=mColPointCnt+1;
	mCOLPOINTMAXCNT=mColPointCnt+1;
	mColPoint=(_TOOL_COL_POINT *)Dmalloc(mCOLPOINTMAXCNT*sizeof(_TOOL_COL_POINT));
	fread(mColPoint,mColPointCnt*sizeof(_TOOL_COL_POINT),1,fp);	


	fread(&mColLineCnt,4,1,fp);	
	mCOLLINEMAXCNT=mColLineCnt+1;
	mColLine=(_TOOL_COL_LINE *)Dmalloc(mCOLLINEMAXCNT*sizeof(_TOOL_COL_LINE));
	fread(mColLine,mColLineCnt*sizeof(_TOOL_COL_LINE),1,fp);	
	fclose(fp);

	mNormal=(Vector4f *)Dmalloc(mCOLLINEMAXCNT*sizeof(Vector4f));
	for(int i=0; i<mColLineCnt; i++)
		GetLineNormal(&mNormal[i],&mColLine[i]);

	return TRUE;
}

BOOL CToolCollisionFace::LoadColFaceFromBSP(CBsp *bsp)
{
	if( mColPoint )
		Dfree(mColPoint);
	if( mColLine )
		Dfree(mColLine);
	if( mNormal )
		Dfree(mNormal);
	if( mVNormal )
		Dfree(mVNormal);
	mVNormal=NULL;
	mNormal=NULL;
	mColPoint=NULL;
	mColLine=NULL;
	mBeforeVId=0;
	mOnePointCreate=FALSE;
	mVNormalCnt=0;
	mColPointCnt = bsp->mCFVertexNum;

	mCOLPOINTMAXCNT=mColPointCnt+1;
	mCOLVNORMALMAXCNT=mColPointCnt+1;
	mColPoint=(_TOOL_COL_POINT *)Dmalloc(mCOLPOINTMAXCNT*sizeof(_TOOL_COL_POINT));
	memcpy(mColPoint,bsp->mCFVertex,mColPointCnt*sizeof(_TOOL_COL_POINT));	

	mColLineCnt = bsp->mCFLineNum;
	mCOLLINEMAXCNT=mColLineCnt+1;
	mColLine=(_TOOL_COL_LINE *)Dmalloc(mCOLLINEMAXCNT*sizeof(_TOOL_COL_LINE));
	memcpy(mColLine,bsp->mCFLine,mColLineCnt*sizeof(_TOOL_COL_LINE));	

	mNormal=(Vector4f *)Dmalloc(mCOLLINEMAXCNT*sizeof(Vector4f));
	for(int i=1; i<mColLineCnt; i++)
		GetLineNormal(&mNormal[i],&mColLine[i]);

	return TRUE;
}

BOOL CToolCollisionFace::GetCrossPoint(Vector3f from,Vector3f at,Vector3f *get)
{
	int i;
	Vector3f a,b,cross;
	BOOL is_col=FALSE;
	float dist = 100000;
	float ret=-65536;
	stColFaceId=-1;

	from[1]+=9;
	at[1]+=9;

	for(i=1; i<mColLineCnt; i++)
	{
		if( GetPlaneCrossPoint(from,at, cross, mNormal[i],mNormal[i][3],0) == FALSE ) 
			continue;

		a[0] = mColPoint[mColLine[i].start_v].v[0];
		a[1] = mColPoint[mColLine[i].start_v].v[1];
		a[2] = mColPoint[mColLine[i].start_v].v[2];
		b[0] = mColPoint[mColLine[i].start_v].v[0];
		b[1] = mColPoint[mColLine[i].start_v].v[1] + mColLine[i].height;
		b[2] = mColPoint[mColLine[i].start_v].v[2];
		if( CheckEdge( a , b , cross ,  mNormal[i],mNormal[i][3]) == 0)	
			continue;

		a[0] = mColPoint[mColLine[i].end_v].v[0];
		a[1] = mColPoint[mColLine[i].end_v].v[1] + mColLine[i].height;
		a[2] = mColPoint[mColLine[i].end_v].v[2];
		if( CheckEdge( b , a , cross ,  mNormal[i],mNormal[i][3]) == 0)	
			continue;

		b[0] = mColPoint[mColLine[i].end_v].v[0];
		b[1] = mColPoint[mColLine[i].end_v].v[1];
		b[2] = mColPoint[mColLine[i].end_v].v[2];
		if( CheckEdge( a , b , cross ,  mNormal[i],mNormal[i][3]) == 0)	
			continue;

		a[0] = mColPoint[mColLine[i].start_v].v[0];
		a[1] = mColPoint[mColLine[i].start_v].v[1];
		a[2] = mColPoint[mColLine[i].start_v].v[2];
		if( CheckEdge( b , a , cross ,  mNormal[i],mNormal[i][3]) == 0)	
			continue;

		Vector3f vec_dist;
		is_col = TRUE;
		Vector3fSub(cross,from,vec_dist);
		float temp_d = sqrtf(Vector3fDot(vec_dist,vec_dist));
		if(dist > temp_d)
		{
			stColFaceId=i;
			dist = temp_d;
			is_col=TRUE;
			get[0][0]=cross[0];
			get[0][1]=cross[1];
			get[0][2]=cross[2];
		}
	}
	from[1]-=9;
	at[1]-=9;
	return is_col;
}

BOOL CToolCollisionFace::EdgeTest(Vector3f cross,int id)
{
	Vector3f a,b;
	a[0] = mColPoint[mColLine[id].start_v].v[0];
	a[1] = mColPoint[mColLine[id].start_v].v[1];
	a[2] = mColPoint[mColLine[id].start_v].v[2];
	b[0] = mColPoint[mColLine[id].start_v].v[0];
	b[1] = mColPoint[mColLine[id].start_v].v[1] + mColLine[id].height;
	b[2] = mColPoint[mColLine[id].start_v].v[2];
	if( CheckEdgeEpsilon( a , b , cross ,  mNormal[id],mNormal[id][3]) == 0)	
		return FALSE;

	a[0] = mColPoint[mColLine[id].end_v].v[0];
	a[1] = mColPoint[mColLine[id].end_v].v[1] + mColLine[id].height;
	a[2] = mColPoint[mColLine[id].end_v].v[2];
	if( CheckEdgeEpsilon( b , a , cross ,  mNormal[id],mNormal[id][3]) == 0)	
		return FALSE;

	b[0] = mColPoint[mColLine[id].end_v].v[0];
	b[1] = mColPoint[mColLine[id].end_v].v[1];
	b[2] = mColPoint[mColLine[id].end_v].v[2];
	if( CheckEdgeEpsilon( a , b , cross ,  mNormal[id],mNormal[id][3]) == 0)	
		return FALSE;

	a[0] = mColPoint[mColLine[id].start_v].v[0];
	a[1] = mColPoint[mColLine[id].start_v].v[1];
	a[2] = mColPoint[mColLine[id].start_v].v[2];
	if( CheckEdgeEpsilon( b , a , cross ,  mNormal[id],mNormal[id][3]) == 0)	
		return FALSE;
	return TRUE;
}

#define _NO_COLLISION		0	//충돌이 안될경우.
#define _NORMAL_COLLISION	1	//평범한 폴리곤 앞의 충돌경우.
#define _CONTINUE_NODE		2	//한쪽 방향으로 점을 찾아가야될경우.
#define _ONE_NORMAL_COLLISION	3	//딱한번 앞으로 보는 면이 충돌되는 경우.
#define _CANT_GO			4	//갈수가 없다.

#define _ON_EPSILON	0.5

#define _ADD_PLANE_DIST		1.0f

void GetCrossPointToDistFromPlane(Vector3f *get,Vector3f from,Vector3f at,Vector4f f_normal)
{
	Vector3f l_normal;
	Vector3fSub(from,at,l_normal);
	Normalize(l_normal);
	float su=DotProduct(l_normal,f_normal);
	su=_ADD_PLANE_DIST/su;
	get[0][0] = l_normal[0]*su+get[0][0];
	get[0][1] = l_normal[1]*su+get[0][1];
	get[0][2] = l_normal[2]*su+get[0][2];
}


int CToolCollisionFace::GetPathCrossPoint(Vector3f from,Vector3f at,Vector3f *get,int now_f1,int now_f2)
{
	int i;
	Vector3f cross;
	BOOL is_col=FALSE,back_ok;
	int back_crash=0;
	int front_crash=0;
	float dist = 100000,front,back;
	float ret=-65536;
	stColFaceId=-1;
	int front_epsilon[8];
	int f_ep_cnt=0;

#define _EYE_POINT 16
	from[1]+=_EYE_POINT;
	at[1]+=_EYE_POINT;

	for( i=1; i<mColLineCnt; i++)
	{
		if( now_f1 == i || now_f2 == i )
			continue;

		front = mNormal[i][0]*from[0] + mNormal[i][1]*from[1] + mNormal[i][2]*from[2] -mNormal[i][3];
		back  = mNormal[i][0]*at[0] + mNormal[i][1]*at[1] + mNormal[i][2]*at[2] -mNormal[i][3];

		if( front > -_ON_EPSILON && front <_ON_EPSILON )	//문제가 되는 프론트
			front_epsilon[f_ep_cnt++]=i;

		if( (front > 0 && back > 0 ) || (front < -_ON_EPSILON && back < -_ON_EPSILON)) 
     			continue;

		if( front > -_ON_EPSILON && back <= 0 )
			back_ok = FALSE;
		else
		if( front < 0 && back > 0 )
			back_ok = TRUE;
		else
		if ( front > -_ON_EPSILON && back > -_ON_EPSILON )
			continue;
		else
		if ( front < -_ON_EPSILON && back > -_ON_EPSILON )
			back_ok = TRUE;
		else
			back_ok = FALSE;

		front = front / (front-back);	
/*
		//앞에 점으로 교점을 만든다.
		cross[0] = from[0] + front*(at[0]-from[0]) + _ADD_PLANE_DIST*mNormal[i][0];
		cross[1] = from[1] + front*(at[1]-from[1]) + _ADD_PLANE_DIST*mNormal[i][1];
		cross[2] = from[2] + front*(at[2]-from[2]) + _ADD_PLANE_DIST*mNormal[i][2];
*/
		cross[0] = from[0] + front*(at[0]-from[0]);
		cross[1] = from[1] + front*(at[1]-from[1]);
		cross[2] = from[2] + front*(at[2]-from[2]);

		if( EdgeTest(cross,i) == FALSE )
			continue;

		Vector3f vec_dist;
		Vector3fSub(cross,from,vec_dist);
		float temp_d = sqrtf(Vector3fDot(vec_dist,vec_dist));
		if(dist > temp_d)
		{
			if( back_ok )
			{
				is_col=FALSE;
				dist = temp_d;
				back_crash++;
				continue;
			}
			stColFaceId=i;
			dist = temp_d;
			is_col=TRUE;
			get[0][0]=cross[0];
			get[0][1]=cross[1];
			get[0][2]=cross[2];
			front_crash++;
		}
		else
		{
			if( back_ok )
				back_crash++;
			else
				front_crash++;
		}
	}
	from[1]-=_EYE_POINT;
	at[1]-=_EYE_POINT;

	if( is_col )
	{
		GetCrossPointToDistFromPlane(get,from,at,mNormal[stColFaceId]);

		if( front_crash - back_crash > 0)
			return _ONE_NORMAL_COLLISION;
		if( front_crash && back_crash == 0)
			return _ONE_NORMAL_COLLISION;
		if( now_f1 || now_f2 )
		{
			front = mNormal[now_f1][0]*get[0][0] + mNormal[now_f1][1]*get[0][1] 
				+ mNormal[now_f1][2]*get[0][2] -mNormal[now_f1][3];
			back = mNormal[now_f2][0]*get[0][0] + mNormal[now_f2][1]*get[0][1] 
				+ mNormal[now_f2][2]*get[0][2] -mNormal[now_f2][3];
			if( front < 0 && back < 0 )
			return _CONTINUE_NODE;
		}

		return _NORMAL_COLLISION;
	}
	else
	{
		if( (now_f1 || now_f2) && back_crash  )
			return _CONTINUE_NODE;
		return _NO_COLLISION;
	}
}
void CToolCollisionFace::GetLineNormal(Vector4f *get,_TOOL_COL_LINE *col_line)
{
	Vector3f a,b,c;

	a[0] = mColPoint[col_line->start_v].v[0];
	a[1] = mColPoint[col_line->start_v].v[1];
	a[2] = mColPoint[col_line->start_v].v[2];

	b[0] = mColPoint[col_line->end_v].v[0];
	b[1] = mColPoint[col_line->end_v].v[1]+col_line->height;
	b[2] = mColPoint[col_line->end_v].v[2];

	c[0] = mColPoint[col_line->end_v].v[0];
	c[1] = mColPoint[col_line->end_v].v[1];
	c[2] = mColPoint[col_line->end_v].v[2];

	::GetNormal(get[0], a, b, c);
}

void CToolCollisionFace::SelectAll()
{
	mbContinueLine = FALSE;
	int i;
	for(i=1 ;i<mColLineCnt; i++)
	{
		if( !(mColLine[i].attr&_LINE_ATTR_FREEZE))
			mColLine[i].attr|=_LINE_ATTR_SELECT;
	}
}

void CToolCollisionFace::AdjustSelectFaceHeight()	//선택 면 높이조정.
{
	int i;
	for(i=1 ;i<mColLineCnt; i++)
	{
		if( mColLine[i].attr & _LINE_ATTR_SELECT )
		{
			mColLine[i].height = (float)GetHeight();
		}
	}
}

void CToolCollisionFace::UnSelectAll()
{
	mbContinueLine = FALSE;
	int i;
	for(i=1 ;i<mColLineCnt; i++)
	{
		mColLine[i].attr&=(~_LINE_ATTR_SELECT);
	}
}
void CToolCollisionFace::DeleteSelectFace()
{
	BOOL is_del=FALSE;
	int i;

	for(i=1 ;i<mColLineCnt; i++)
	{
		if( mColLine[i].attr & _LINE_ATTR_SELECT )
		{ 
			is_del=TRUE;
			break;
		}
	}
	if( is_del )
	{
		DeleteLine((WORD)i);
		DeleteSelectFace(); //재귀 호출
	}
	AdjustVertex();
}
void CToolCollisionFace::SelectFace(int id)	//한 페이스만 선택한다.
{
	if( id )
	{
		if( !(mColLine[id].attr&_LINE_ATTR_FREEZE))
			mColLine[id].attr|=_LINE_ATTR_SELECT;
	}
}
BOOL CToolCollisionFace::SelectFaceGroup(int id)	//관련된 페이스 그룹을 선택한다.
{
	_TOOL_COL_LINE *line;
	int err_cnt=0;

	if( id == 0 )
		return TRUE;
	if( !(mColLine[id].attr&_LINE_ATTR_FREEZE))
		mColLine[id].attr|=_LINE_ATTR_SELECT;
	line = &mColLine[id];
	while( 1 )
	{
		if( !(line->attr&_LINE_ATTR_FREEZE))
			line->attr|=_LINE_ATTR_SELECT;
		if( line->front == 0)
			break;
		line = &mColLine[line->front];
		if( line->front == id )
			break;
//		if( line->start_v == line->end_v )
		err_cnt++;
		if( err_cnt > 65536)	//라인링크 에러다 
			return FALSE;
	}
	line = &mColLine[id];
	err_cnt=0;
	while( 1 )
	{
		if( !(line->attr&_LINE_ATTR_FREEZE))
			line->attr|=_LINE_ATTR_SELECT;
		if( line->back == 0)
			break;
		line = &mColLine[line->back];
		if( line->back == id )
			break;
		err_cnt++;
		if( err_cnt > 65536)	//라인링크 에러다 
			return FALSE;
	}
	return TRUE;
}
BOOL CToolCollisionFace::UnFreezeFaceGroup(int id)	//관련된 페이스 그룹을 선택한다.
{
	_TOOL_COL_LINE *line;
	int err_cnt=0;

	if( id == 0 )
		return TRUE;
	mColLine[id].attr &= ~_LINE_ATTR_FREEZE;
	line = &mColLine[id];
	while( 1 )
	{
		line->attr &= ~_LINE_ATTR_FREEZE;
		if( line->front == 0)
			break;
		line = &mColLine[line->front];
		if( line->front == id )
			break;
//		if( line->start_v == line->end_v )
		err_cnt++;
		if( err_cnt > 65536)	//라인링크 에러다 
			return FALSE;
	}
	line = &mColLine[id];
	err_cnt=0;
	while( 1 )
	{
		line->attr &= ~_LINE_ATTR_FREEZE;
		if( line->back == 0)
			break;
		line = &mColLine[line->back];
		if( line->back == id )
			break;
		err_cnt++;
		if( err_cnt > 65536)	//라인링크 에러다 
			return FALSE;
	}
	return TRUE;
}


void CToolCollisionFace::FreezeFromSelect()	//선택된 페이스를 프리즈 시키고 선택을 푼다.
{
	int i;

	for(i=1 ;i<mColLineCnt; i++)
	{
		if( mColLine[i].attr & _LINE_ATTR_SELECT )
		{ 
			mColLine[i].attr &= ~_LINE_ATTR_SELECT;	//선택은 끄자.
			mColLine[i].attr |= _LINE_ATTR_FREEZE;
		}
	}
}

void CToolCollisionFace::UnFreezeAll()	//모두 언프리즈 한다.
{
	int i;

	for(i=1 ;i<mColLineCnt; i++)
	{
		mColLine[i].attr &= ~_LINE_ATTR_FREEZE;
	}
}

void CToolCollisionFace::UnFreezeFace(int id)	//한 페이스만  언프리즈 한다.
{
	if( id )
	{
		mColLine[id].attr &= ~_LINE_ATTR_FREEZE;
	}
}


void CToolCollisionFace::FixLinkFace()	//페이스 링크 에러 첵크해서 고친다.
{
	BOOL is_ok,fix_ok=FALSE;
	_TOOL_COL_LINE *line;
	int i,j;

	for(i=1 ;i<mColLineCnt; i++)
	{
		line = &mColLine[i];
		is_ok=FALSE;
		for(j=1 ;j<mColLineCnt; j++)
		{ 
			if( mColLine[j].start_v == line->end_v )
			{
				is_ok=TRUE;
				break;
			}
		}
		if( is_ok )
		{
			if( mColLine[j].back != i || mColLine[i].front != j )
			{
				mColLine[j].back = i;
				mColLine[i].front = j;
				fix_ok=TRUE;
			}
		}
	}
	int err_face=0;
	for(i=1 ;i<mColLineCnt; i++)
	{
		if( mColLine[i].end_v == mColLine[i].start_v )
		{
			err_face=i;
			break;
		}
	}
	if( err_face )
	{
		DeleteLine(err_face);
		FixLinkFace();
	}
}

int CToolCollisionFace::GetCrossFace(Vector3f from,Vector3f at)
{
	Vector3f get;
	GetCrossPoint(from,at,&get);
	return stColFaceId;
}

void GetFromAt(int x,int y,Vector3f from,Vector3f at)
{
	_R3ENGINE_STATE *state = GetR3State();
	BOOL is_col=FALSE;
	float v[3],n[3];

	float half_x=state->mScreenXsize/2.0f-.5f;
	float half_y=state->mScreenYsize/2.0f-.5f;

    n[0] =  ( ( ( 2.0f * x ) / (state->mScreenXsize) ) - 1.0f ) / state->mMatProj._11;
    n[1] = -( ( ( 2.0f * y ) / (state->mScreenYsize) ) - 1.0f) / state->mMatProj._22;
    n[2] =  1.0f;	//현재 왼손 좌표계라...

	v[0] = n[0]*3660000;		//숫자가 커야 오차가 없어진다????
	v[1] = n[1]*3660000;
	v[2] = n[2]*3660000;

    // Get the inverse view matrix
    D3DXMATRIX m;
    D3DXMatrixInverse( &m, NULL, &state->mMatView );

    // Transform the screen space pick ray into 3D space
    at[0]  = v[0]*m._11 + v[1]*m._21 + v[2]*m._31;
    at[1]  = v[0]*m._12 + v[1]*m._22 + v[2]*m._32-9;
    at[2]  = v[0]*m._13 + v[1]*m._23 + v[2]*m._33;
    from[0] = m._41;
    from[1] = m._42-9;
    from[2] = m._43;
}

int CToolCollisionFace::GetCrossFace(int x,int y)
{
	Vector3f from,at;
	
	GetFromAt(x,y,from,at);

	return GetCrossFace(from,at);
}

//-------------------------------- 길찾는 ------------------------------

#define _MAX_FIND_PATH	10	// 최대 가는 방법.
#define _MAX_PATH_POS	16	//최대 패스 꺾임

#define _LINE_FRONT	1		//라인 방향.
#define _LINE_BACK	-1

//--패스 파인더.
#define _MAX_FIND_PATH	10	// 최대 가는 방법.
#define _MAX_PATH_POS	16	//최대 패스 꺾임
/*
typedef struct{
	float TotalLeng;
	Vector3f Path[_MAX_PATH_POS+1];
	int WhatDirection;	//어느 방향.?
	WORD FrontLinetId;
	WORD BackLineId;
	int PathCnt;
	BOOL IsFind;	//찾았냐?
}_PATH_NODE;
*/
static _PATH_NODE stPathNode[_MAX_FIND_PATH+2];
static int stFindPathCnt;
static Vector3f stStart;

static BOOL AddPath(Vector3f pos,int id)
{
	if( stPathNode[id].PathCnt+1 >= _MAX_PATH_POS )
		return FALSE;

	Vector3fCopy(pos,stPathNode[id].Path[stPathNode[id].PathCnt]);

	Vector3f dist;
	//거리를 구한다.
	if( stPathNode[id].PathCnt == 0 )
	{
		Vector3fSub(pos,stStart,dist);
	}
	else
	{	//그 이전거하고 거리를 구한다.
		Vector3fSub(pos,stPathNode[id].Path[stPathNode[id].PathCnt-1],dist);
	}
	stPathNode[id].TotalLeng += Vector3fDist(dist);

	stPathNode[id].PathCnt++;
	return TRUE;
}

static int GetPathCnt(int id)
{
	return stPathNode[id].PathCnt;
}
static void CopyPath(int tar,int src)
{
	memcpy(&stPathNode[tar],&stPathNode[src],sizeof(_PATH_NODE));
}

static _PATH_NODE stStackPathNode[_MAX_FIND_PATH+10];
static int stStackPoint=0;

static void PushPathStack(int id)	//갈라질곳의 패스를 스택에 넣는다.
{
	memcpy(&stStackPathNode[stStackPoint],&stPathNode[id],sizeof(_PATH_NODE));
	stStackPoint++;
	if( stStackPoint >= _MAX_FIND_PATH+10 )
	{
		Error("pathfinder stack overflow","");
	}
}
static void PopPathStack(int id)	//갈라질곳의 패스를 스택에 꺼낸다.
{
	stStackPoint--;
	if( stStackPoint < 0)
	{
		Error("pathfinder stack 에러","");
	}
	memcpy(&stPathNode[id],&stStackPathNode[stStackPoint],sizeof(_PATH_NODE));
}

static void CompletePath(int id)	//앗싸 길찾았다.
{
	stPathNode[id].IsFind = TRUE;
}
static void SetPathDirection(int id,int dir)
{
	stPathNode[id].WhatDirection = dir;
}
static int GetPathDirection(int id)
{
	return stPathNode[id].WhatDirection;
}
static void SetFrontLineId(int id,int l_id)
{
	stPathNode[id].FrontLinetId = l_id;
}
static void SetBackLineId(int id,int l_id)
{
	stPathNode[id].BackLineId = l_id;
}
static WORD GetFrontLineId(int id)
{
	return stPathNode[id].FrontLinetId;
}
static WORD GetBackLineId(int id)
{
	return stPathNode[id].BackLineId;
}

BOOL CToolCollisionFace::IsExistSelfPoint(int id,int face)	//페스리스트에 충돌 페이스의 점들이 있나?
{
	if( GetPathDirection(id) == _LINE_BACK )
	{
		int st=mColLine[face].start_v;
		for(int i=0; i<stPathNode[id].PathCnt; i++)
		{
			if( CmpVEpsilon(mColPoint[st].v,stPathNode[id].Path[i],_ADD_PLANE_DIST))
				return TRUE;
		}
	}
	else
	{
		int st=mColLine[face].end_v;
		for(int i=0; i<stPathNode[id].PathCnt; i++)
		{
			if( CmpVEpsilon(mColPoint[st].v,stPathNode[id].Path[i],_ADD_PLANE_DIST))
				return TRUE;
		}
	}
	return FALSE;
}

void CToolCollisionFace::GetFaceFrontPoint(Vector3f *get_temp,int v_id)	//평면의 약간 앞으로 튀어나온 점을 얻어온다.
{
	Vector3f add_pos;
	Vector3fScale(mVNormal[v_id],_ADD_PLANE_DIST,add_pos);
	Vector3fAdd(add_pos,mColPoint[v_id].v,get_temp[0]);
}


void CToolCollisionFace::GetPath(Vector3f from,Vector3f at)
{
	if( stFindPathCnt >= _MAX_FIND_PATH )	//오바다 그만 찾자..
		return;
	if( GetPathCnt(stFindPathCnt)+1 >= _MAX_PATH_POS )	//패스넣는게 오바됬다.
		return;

	Vector3f get,get_temp,get2;

	int type = GetPathCrossPoint(from,at,&get,GetFrontLineId(stFindPathCnt),GetBackLineId(stFindPathCnt));
	int backup_face = stColFaceId;

	if( type == _NORMAL_COLLISION )
	{
		if( IsExistSelfPoint(stFindPathCnt,backup_face) )
		{
			type = _CONTINUE_NODE;
		}
	}

	switch( type )
	{
		case _NORMAL_COLLISION:
			AddPath(get,stFindPathCnt);	//충돌된 점의 복사.
			PushPathStack(stFindPathCnt);	//갈라질곳의 패스를 스택에 넣는다.

			//백라인 쪽으로 정보를 넣는다.
			SetFrontLineId(stFindPathCnt,backup_face);//백과 프론트의 정보를 넣는다.
			SetBackLineId(stFindPathCnt,mColLine[backup_face].back);
			SetPathDirection(stFindPathCnt,_LINE_BACK);
			GetFaceFrontPoint(&get_temp,mColLine[backup_face].start_v);	//평면의 약간 앞으로 튀어나온 점을 얻어온다.
			get_temp[1] = from[1];

			type = GetPathCrossPoint(get,get_temp,&get2,GetFrontLineId(stFindPathCnt),GetBackLineId(stFindPathCnt));
			if( type && stColFaceId !=-1 )
			{
				AddPath(get2,stFindPathCnt);	//충돌된 점의 복사.
				SetFrontLineId( stFindPathCnt, stColFaceId );	//백과 프론트의 정보를 다시 넣는다.
				SetBackLineId( stFindPathCnt, mColLine[stColFaceId].back);
//				FixBeforePoint(stColFaceId,mColLine[stColFaceId].end_v,stFindPathCnt);	//이전 점과의 중간 위치를 얻어서 계산 해 넣는다.
				GetFaceFrontPoint(&get_temp,mColLine[stColFaceId].start_v);	//평면의 약간 앞으로 튀어나온 점을 얻어온다.
				get_temp[1] = from[1];
			}
			AddPath(get_temp,stFindPathCnt);	//점추가 
			GetPath(get_temp,at);

			stFindPathCnt++;	//패스하나더 생성..
			PopPathStack(stFindPathCnt);//갈라질곳의 패스를 스택에 꺼낸다.
			if( stFindPathCnt >= _MAX_FIND_PATH )
			{
				stFindPathCnt--;
				return;
			}
			//프론트 라인쪽으로 정보를 넣는다.
			SetFrontLineId(stFindPathCnt,mColLine[backup_face].front);//백과 프론트의 정보를 넣는다.
			SetBackLineId(stFindPathCnt,backup_face);
			SetPathDirection(stFindPathCnt,_LINE_FRONT);
			GetFaceFrontPoint(&get_temp,mColLine[backup_face].end_v);	//평면의 약간 앞으로 튀어나온 점을 얻어온다.
			get_temp[1] = from[1];

			type = GetPathCrossPoint(get,get_temp,&get2,GetFrontLineId(stFindPathCnt),GetBackLineId(stFindPathCnt));
			if( type && stColFaceId !=-1 )
			{
				AddPath(get2,stFindPathCnt);	//충돌된 점의 복사.
				SetBackLineId( stFindPathCnt, stColFaceId );	//백과 프론트의 정보를 넣는다.
				SetFrontLineId( stFindPathCnt, mColLine[stColFaceId].front);
//				FixBeforePoint(stColFaceId,mColLine[stColFaceId].start_v,stFindPathCnt);	//이전 점과의 중간 위치를 얻어서 계산 해 넣는다.
				GetFaceFrontPoint(&get_temp,mColLine[stColFaceId].end_v);	//평면의 약간 앞으로 튀어나온 점을 얻어온다.
				get_temp[1] = from[1];
			}
			AddPath(get_temp,stFindPathCnt);	//점추가 
			GetPath(get_temp,at);
			break;
		case _CONTINUE_NODE:
			if( GetPathDirection(stFindPathCnt) == _LINE_BACK)
			{
				int bk = GetBackLineId(stFindPathCnt);	//바뀌기 전에 백업.
				SetFrontLineId( stFindPathCnt, bk );	//백과 프론트의 정보를 넣는다.
				SetBackLineId( stFindPathCnt, mColLine[bk].back);
				//FixBeforePoint(bk,mColLine[bk].end_v,stFindPathCnt);	//이전 점과의 중간 위치를 얻어서 계산 해 넣는다.
				GetFaceFrontPoint(&get_temp,mColLine[bk].start_v);	//평면의 약간 앞으로 튀어나온 점을 얻어온다.
				get_temp[1] = from[1];

				type = GetPathCrossPoint(from,get_temp,&get,GetFrontLineId(stFindPathCnt),GetBackLineId(stFindPathCnt));
				if( type  && stColFaceId !=-1  )
				{
					AddPath(get,stFindPathCnt);	//충돌된 점의 복사.
					SetFrontLineId( stFindPathCnt, stColFaceId );	//백과 프론트의 정보를 다시 넣는다.
					SetBackLineId( stFindPathCnt, mColLine[stColFaceId].back);
//					FixBeforePoint(stColFaceId,mColLine[stColFaceId].end_v,stFindPathCnt);	//이전 점과의 중간 위치를 얻어서 계산 해 넣는다.
					GetFaceFrontPoint(&get_temp,mColLine[stColFaceId].start_v);	//평면의 약간 앞으로 튀어나온 점을 얻어온다.
					get_temp[1] = from[1];					
				}
				AddPath(get_temp,stFindPathCnt);	//점 추가 
				GetPath(get_temp,at);	//또 가자..
			}
			else
			{
				int bk = GetFrontLineId(stFindPathCnt);	//바뀌기 전에 백업.
				SetBackLineId( stFindPathCnt, bk );	//백과 프론트의 정보를 넣는다.
				SetFrontLineId( stFindPathCnt, mColLine[bk].front);
//				FixBeforePoint(bk,mColLine[bk].start_v,stFindPathCnt);	//이전 점과의 중간 위치를 얻어서 계산 해 넣는다.
				GetFaceFrontPoint(&get_temp,mColLine[bk].end_v);	//평면의 약간 앞으로 튀어나온 점을 얻어온다.
				get_temp[1] = from[1];

				type = GetPathCrossPoint(from,get_temp,&get,GetFrontLineId(stFindPathCnt),GetBackLineId(stFindPathCnt));
				if( type && stColFaceId !=-1 )
				{
					AddPath(get,stFindPathCnt);	//충돌된 점의 복사.
					SetBackLineId( stFindPathCnt, stColFaceId );	//백과 프론트의 정보를 넣는다.
					SetFrontLineId( stFindPathCnt, mColLine[stColFaceId].front);
//					FixBeforePoint(stColFaceId,mColLine[stColFaceId].start_v,stFindPathCnt);	//이전 점과의 중간 위치를 얻어서 계산 해 넣는다.
					GetFaceFrontPoint(&get_temp,mColLine[stColFaceId].end_v);	//평면의 약간 앞으로 튀어나온 점을 얻어온다.
					get_temp[1] = from[1];
				}
				AddPath(get_temp,stFindPathCnt);	//점추가 
				GetPath(get_temp,at);	//또가자..
			}
			break;
		case _NO_COLLISION:
			if( AddPath(at,stFindPathCnt) == FALSE )	//더이상못 넣는다.
				break;
			CompletePath(stFindPathCnt);//앗싸 길찾았다.
			break;
		case _ONE_NORMAL_COLLISION:	//딱 front면과의 충돌이 있는경우..
			if( AddPath(get,stFindPathCnt) == FALSE )	//더이상못 넣는다.
				break;
			CompletePath(stFindPathCnt);//앗싸 길찾았다.
			break;
	}
}

int CToolCollisionFace::GetFinalPath(void *node_p,Vector3f from,Vector3f get[16])
{
	int i,j,id;
	_PATH_NODE *node=(_PATH_NODE *)node_p;
/*
	Vector3f sub;	//처음 찍은곳이 너무 근처에 있다면..
	node->Path[0][1]=from[1];
	Vector3fSub(from,node->Path[0],sub);
	
	float dist = Vector3fDist(sub);
	int ss=0;
	if( dist < 1 )
		ss=1;
*/
	Vector3f temp;
	if( node->PathCnt == 1)
	{
		Vector3fCopy(node->Path[0],get[0]);
		return 1;
	}
	int index[16],index_cnt=0;

	index[index_cnt++] = node->PathCnt-1;
	for(i=node->PathCnt-2; i>0; i--)
	{
		id=-2;
		if( !GetCrossPoint(from,node->Path[i],&temp) )
		{
			id=-1;
			index[index_cnt++]=i;
			break;
		}
		for(j=i-2; j>0 ;j-- )
		{
			if( !GetCrossPoint(node->Path[j],node->Path[i],&temp) )
			{
				id=j;
			}
		}
		if( id == -2 )
		{
			index[index_cnt++]=i;
			continue;
		}
		
		index[index_cnt++]=i;
		index[index_cnt++]=id;
		i=id+1;
	}
	if( id!=-1 )
		index[index_cnt++]=0;

	for(i=0; i<index_cnt; i++)
		Vector3fCopy(node->Path[index[index_cnt-i-1]],get[i]);


	return index_cnt;
	/*	최적화가 아닐때
	for(i=ss; i<node->PathCnt; i++)
	{
		Vector3fCopy(node->Path[i],get[i-ss]);
	}
	return ( node->PathCnt-ss );
	*/
}

void CToolCollisionFace::GetPathFinder(Vector3f from,Vector3f at,Vector3f get[16],DWORD *cnt)
{
	int i,min_i=-1;
	float min_dist=1000000;

	at[1]=from[1];
	Vector3f temp_f;
	Vector3fSub(at,from,temp_f);
	float dist = Vector3fDist(temp_f);
	if( dist > 200 )
	{
		Normalize(temp_f);
		Vector3fScale(temp_f,200,temp_f);	//지금은 최대 20미터까지만 간다.
		Vector3fAdd(temp_f,from,at);
	}
/*
	if( dist > 800 )
	{
		Normalize(temp_f);
		Vector3fScale(temp_f,800,temp_f);	//지금은 최대 80미터까지만 간다.
		Vector3fAdd(temp_f,from,at);
	}
*/
	memset(stPathNode,0,sizeof(_PATH_NODE)*_MAX_FIND_PATH);
	stFindPathCnt=0;	//패스도 비우고..
	stStackPoint=0;	//스택도 비우고..

	Vector3fCopy(from,stStart);
	GetPath(from,at);
	stFindPathCnt++;	//개수이므로...
	for(i=0; i<	stFindPathCnt; i++)
	{
		if( stPathNode[i].IsFind == FALSE )
			continue;
		if( min_dist > stPathNode[i].TotalLeng )
		{
			min_dist = stPathNode[i].TotalLeng;
			min_i = i;
		}
	}
	/*
	if( stFindPathCnt == 0 )	//이런 개같은 경우가 생기면 안됨..
	{
		Vector3fCopy(at,get[i]);
		*cnt = 1;
		return;
	}*/
	if( min_i == -1 )	//길을 못찾음.
	{
		Vector3f can_go;
		if( GetPathCrossPoint(from,at,&can_go,0,0) )
		{
			Vector3fCopy(can_go,get[0]);
//			Vector3fCopy(from,get[0]);
		}
		else
		{
			Vector3fCopy(at,get[0]);
		}
		*cnt = 1;
		return;
	}
	
	*cnt=GetFinalPath(&stPathNode[min_i],from,get);	//중간에 갈수 있는 길을 찾아서 최적화 해준다. 

}

void CToolCollisionFace::MakeFnormal()	//현재 셋팅되있는 페이스노멀을 구한다.
{
	if( mNormal == NULL )
		return;

	for( int i=0; i<mColLineCnt; i++)
	{
		GetLineNormal(&mNormal[i],&mColLine[i]);
	}
}
void CToolCollisionFace::MakeVnormal()	//현재 셋팅되있는 페이스들에서 버텍스노멀을 구한다.
{
	if( mVNormal == NULL )	//초기 생성.
	{
		mVNormal=(Vector3f *)Dmalloc(mCOLVNORMALMAXCNT*sizeof(Vector3f));
	}
	if(mColPointCnt >= mCOLVNORMALMAXCNT)	//용량이 커지면 재할당.
	{
		mVNormal=(Vector3f *)ReAlloc(mVNormal,mCOLVNORMALMAXCNT*sizeof(Vector3f),(mCOLVNORMALMAXCNT+1000)*sizeof(Vector3f));
		mCOLVNORMALMAXCNT+=1000;
	}
	for(int i=1; i<mColPointCnt; i++)
	{
		int face_list[20];
		int face_cnt=0;
		Vector3f a;
		a[0]=0;		a[1]=0;		a[2]=0;

		for(int j=1; j<mColLineCnt; j++)
		{
			if( mColLine[j].start_v == i )
			{
				a[0]+=mNormal[j][0];
				a[1]+=mNormal[j][1];
				a[2]+=mNormal[j][2];
				face_list[face_cnt++]=j;
			}
			else
			if( mColLine[j].end_v == i )
			{
				a[0]+=mNormal[j][0];
				a[1]+=mNormal[j][1];
				a[2]+=mNormal[j][2];
				face_list[face_cnt++]=j;
			}
		}
		if( face_cnt > 2 )
		{
			Warning("노멀 생성 에러","");
		}
		Normalize(a);
		Vector3fCopy(a,mVNormal[i]);
	}
	mVNormalCnt = i;
}


void CToolCollisionFace::MoveSelectFace(BOOL yaxis,BOOL xzaxis,LONG xdiff, LONG ydiff)
{
#define SetVertex(num)	(((BYTE*)select_vertex)[num>>3]|=(1<<(num&7)))
#define IsSetVertex(num) (((BYTE*)select_vertex)[num>>3]&(1<<(num&7)))
#define _SXSY_TO_DIST	0.5f

	BYTE select_vertex[65536/8];
	int i;
	float y=(float)ydiff*_SXSY_TO_DIST;
	float x=(float)xdiff*_SXSY_TO_DIST;

	if( x == 0 && y == 0 )
		return;

	memset(select_vertex,0,sizeof(BYTE)*(65536/8));
	for(i=1 ;i<mColLineCnt; i++)
	{
		if( mColLine[i].attr==_LINE_ATTR_SELECT )
		{
			SetVertex(mColLine[i].start_v);
			SetVertex(mColLine[i].end_v);
		}
	}
	if( yaxis )
	{
		for(i=1 ;i<mColPointCnt; i++)
		{
			if( IsSetVertex(i) )
			{
				mColPoint[i].v[1]-=y;
			}
		}
	}
	else
	if( xzaxis )
	{
		Vector3f src,tar,temp;
		_R3ENGINE_STATE *state = GetR3State();
		temp[0] = 0;
		temp[1] = 0;
		temp[2] = 100;

		tar[0]=state->mMatView.m[0][2]*temp[2];
		tar[1]=state->mMatView.m[1][2]*temp[2];
		tar[2]=state->mMatView.m[2][2]*temp[2];

		src[0]=0;
		src[1]=0;
		src[2]=0;
		D3DXMATRIX billboard,temp_b;
//		GetYBillboardMatrix(&billboard,src,tar);//	빌보드
		GetYBillboardMatrix(&temp_b,src,tar);//	빌보드
		D3DXMatrixInverse( &billboard, NULL, &temp_b );
		for(i=1 ;i<mColPointCnt; i++)
		{
			if( !IsSetVertex(i) )
				continue;
			mColPoint[i].v[0]+=(billboard.m[0][0]*x+billboard.m[0][2]*-y);
			mColPoint[i].v[2]+=(billboard.m[2][0]*x+billboard.m[2][2]*-y);
		}
	}
}
void CToolCollisionFace::MoveSelectVertex(BOOL yaxis,BOOL xzaxis,LONG xdiff, LONG ydiff)
{
#define _SXSY_TO_DIST	0.5f
	float y=(float)ydiff*_SXSY_TO_DIST;
	float x=(float)xdiff*_SXSY_TO_DIST;

	if( x == 0 && y == 0 )
		return;
	if( GetSelectVertex() >= mColPointCnt || mSelectVertex==0)
	{
		UnSetSelectVertex();
		return;
	}

	if( yaxis )
	{
		mColPoint[mSelectVertex].v[1]-=y;
	}
	else
	if( xzaxis )
	{
		Vector3f src,tar,temp;
		_R3ENGINE_STATE *state = GetR3State();
		temp[0] = 0;
		temp[1] = 0;
		temp[2] = 100;

		tar[0]=state->mMatView.m[0][2]*temp[2];
		tar[1]=state->mMatView.m[1][2]*temp[2];
		tar[2]=state->mMatView.m[2][2]*temp[2];

		src[0]=0;
		src[1]=0;
		src[2]=0;
		D3DXMATRIX billboard,temp_b;
//		GetYBillboardMatrix(&billboard,src,tar);//	빌보드
		GetYBillboardMatrix(&temp_b,src,tar);//	빌보드
		D3DXMatrixInverse( &billboard, NULL, &temp_b );

		mColPoint[mSelectVertex].v[0]+=(billboard.m[0][0]*x+billboard.m[0][2]*-y);
		mColPoint[mSelectVertex].v[2]+=(billboard.m[2][0]*x+billboard.m[2][2]*-y);
	}
}


void CToolCollisionFace::SetSelectVertex(WORD id)	//버택스는 오직 하나만 선택가능 
{
	mSelectVertex=id;
}

WORD CToolCollisionFace::GetSelectVertex()	//버택스는 오직 하나만 선택가능 
{
	return (WORD) mSelectVertex;
}

void CToolCollisionFace::UnSetSelectVertex()
{
	mSelectVertex=0;
}

#include "Collision.h"

void GetBBox(Vector3f bbmin,Vector3f bbmax,DWORD cnt,Vector3f *v_list)
{
	bbmin[0] = 65000;
	bbmin[1] = 65000;
	bbmin[2] = 65000;
	bbmax[0] = -65000;
	bbmax[1] = -65000;
	bbmax[2] = -65000;

	for(DWORD i=0; i<cnt; i++ )
	{
		for(DWORD j=0; j<3; j++ )
		{
			if( bbmin[j] > v_list[i][j] )
				bbmin[j]=v_list[i][j];
			if( bbmax[j] < v_list[i][j] )
				bbmax[j]=v_list[i][j];
		}
	}
}

void Get6DOFNormal(Vector4f *p_normal,_BSP_LEAF *leaf)
{
	//바운딩 박스 노멀을 먼저 구하자..
	p_normal[0][0] = 0;		p_normal[0][1] = -1;		p_normal[0][2] = 0;
	p_normal[0][3] = (float)-leaf->bb_max[1];

	p_normal[1][0] = 0;		p_normal[1][1] = 1;		p_normal[1][2] = 0;
	p_normal[1][3] = (float)leaf->bb_min[1];

	p_normal[2][0] = 1;		p_normal[2][1] = 0;		p_normal[2][2] = 0;
	p_normal[2][3] = (float)leaf->bb_min[0];

	p_normal[3][0] = -1;		p_normal[3][1] = 0;		p_normal[3][2] = 0;
	p_normal[3][3] = -(float)leaf->bb_max[0];

	p_normal[4][0] = 0;		p_normal[4][1] = 0;		p_normal[4][2] = 1;
	p_normal[4][3] = (float)leaf->bb_min[2];

	p_normal[5][0] = 0;		p_normal[5][1] = 0;		p_normal[5][2] = -1;
	p_normal[5][3] = -(float)leaf->bb_max[2];
}
void CToolCollisionFace::LoadCollisonInfoInBSP(CBsp *bsp,void *v_add_bsp)
{
/*	//기존 방식인데 속도가 느리다.
	_ADD_BSP_SAVE *add_bsp = (_ADD_BSP_SAVE *)v_add_bsp;
	//리프 정보를 넣어놓는다.
	DWORD max_l_id=40000;
	_TOOL_COL_LEAF *col_leaf = (_TOOL_COL_LEAF *)Dmalloc(bsp->mLeafNum*sizeof(_TOOL_COL_LEAF));
	memset(col_leaf,0,bsp->mLeafNum*sizeof(_TOOL_COL_LEAF));
	WORD *col_line_id=(WORD *)Dmalloc(sizeof(WORD)*max_l_id);
	DWORD col_id_num=0;

	DWORD i,j;

	Vector3f v[4];
	_TOOL_COL_LINE *c_line = mColLine;
	_TOOL_COL_POINT *c_point = mColPoint;

	Vector4f p_normal[6];
	for(i=0; i<bsp->mLeafNum; i++)
	{
		BOOL is_in_leaf = FALSE;
		col_leaf[i].start_id = col_id_num;

		//바운딩 박스 노멀을 먼저 구하자..
		p_normal[0][0] = 0;		p_normal[0][1] = -1;		p_normal[0][2] = 0;
		p_normal[0][3] = (float)-bsp->mLeaf[i].bb_max[1];

		p_normal[1][0] = 0;		p_normal[1][1] = 1;		p_normal[1][2] = 0;
		p_normal[1][3] = (float)bsp->mLeaf[i].bb_min[1];

		p_normal[2][0] = 1;		p_normal[2][1] = 0;		p_normal[2][2] = 0;
		p_normal[2][3] = (float)bsp->mLeaf[i].bb_min[0];

		p_normal[3][0] = -1;		p_normal[3][1] = 0;		p_normal[3][2] = 0;
		p_normal[3][3] = -(float)bsp->mLeaf[i].bb_max[0];

		p_normal[4][0] = 0;		p_normal[4][1] = 0;		p_normal[4][2] = 1;
		p_normal[4][3] = (float)bsp->mLeaf[i].bb_min[2];

		p_normal[5][0] = 0;		p_normal[5][1] = 0;		p_normal[5][2] = -1;
		p_normal[5][3] = -(float)bsp->mLeaf[i].bb_max[2];


		for(j=1; j<(DWORD)mColLineCnt; j++)
		{

			//-----충돌방법을 충돌 폴리곤과 바운딩 박스 라인과의 충돌도 추가해야됨...
			v[0][0] = c_point[c_line[j].start_v].v[0];
			v[0][1] = c_point[c_line[j].start_v].v[1];
			v[0][2] = c_point[c_line[j].start_v].v[2];
			v[1][0] = v[0][0];
			v[1][1] = v[0][1] + c_line[j].height;
			v[1][2] = v[0][2];
			v[2][0] = c_point[c_line[j].end_v].v[0];
			v[2][1] = c_point[c_line[j].end_v].v[1] + c_line[j].height;
			v[2][2] = c_point[c_line[j].end_v].v[2];
			v[3][0] = v[2][0];
			v[3][1] = v[2][1] - c_line[j].height;
			v[3][2] = v[2][2];

			if( IsCollisionBBoxToFace(p_normal,v[0],v[1],v[2]) 
			|| IsCollisionBBoxToFace(p_normal,v[0],v[2],v[3]) )
				is_in_leaf=TRUE;

			if( is_in_leaf )
			{
				is_in_leaf=FALSE;
				col_leaf[i].line_num++;
				col_line_id[col_id_num++]=(WORD)j;
				if( col_id_num >= max_l_id )
				{
					col_line_id=(WORD *)ReAlloc(col_line_id,sizeof(WORD)*max_l_id,sizeof(WORD)*(max_l_id+4000));
					max_l_id+=4000;
				}
			}
		}
	}
*/

	//-----빠른 세이브 방법으로 바꾼다.
	DWORD max_l_list=40000;
	WORD *l_list=(WORD *)Dmalloc(sizeof(WORD)*max_l_list);
	WORD *coll_index=(WORD *)Dmalloc(sizeof(WORD)*max_l_list);
	DWORD l_list_num=0;

	LONG leaf_num;
	short leaf_list[10000];
	DWORD col_leaf_num=0;

	DWORD i,j;

	Vector3f v[4],bbmin,bbmax;
	_TOOL_COL_LINE *c_line = mColLine;
	_TOOL_COL_POINT *c_point = mColPoint;

	Vector4f p_normal[6];
	for(j=1; j<(DWORD)mColLineCnt; j++)
	{
		v[0][0] = c_point[c_line[j].start_v].v[0];
		v[0][1] = c_point[c_line[j].start_v].v[1];
		v[0][2] = c_point[c_line[j].start_v].v[2];
		v[1][0] = v[0][0];
		v[1][1] = v[0][1] + c_line[j].height;
		v[1][2] = v[0][2];
		v[2][0] = c_point[c_line[j].end_v].v[0];
		v[2][1] = c_point[c_line[j].end_v].v[1] + c_line[j].height;
		v[2][2] = c_point[c_line[j].end_v].v[2];
		v[3][0] = v[2][0];
		v[3][1] = v[2][1] - c_line[j].height;
		v[3][2] = v[2][2];

		GetBBox(bbmin,bbmax,4,v);

		bsp->GetFastLeafListFromBBox(bbmin,bbmax,&leaf_num,leaf_list,9999);// 바운딩 박스를 넣어주면 리프리스트를 준다.
		for(i=0; i<leaf_num; i++)
		{
			short leaf_id = leaf_list[i];
			Get6DOFNormal(p_normal,&bsp->mLeaf[leaf_id]);

			if( IsCollisionBBoxToFace(p_normal,v[0],v[1],v[2]) 
			|| IsCollisionBBoxToFace(p_normal,v[0],v[2],v[3]) )
			{
				l_list[col_leaf_num]=leaf_id;
				coll_index[col_leaf_num]=(WORD)j;
				col_leaf_num++;
			}
		}
	}

	_ADD_BSP_SAVE *add_bsp = (_ADD_BSP_SAVE *)v_add_bsp;
	//리프 정보를 넣어놓는다.
	_TOOL_COL_LEAF *col_leaf = (_TOOL_COL_LEAF *)Dmalloc(bsp->mLeafNum*sizeof(_TOOL_COL_LEAF));
	memset(col_leaf,0,bsp->mLeafNum*sizeof(_TOOL_COL_LEAF));

	DWORD max_l_id=40000;
	WORD *col_line_id=(WORD *)Dmalloc(sizeof(WORD)*max_l_id);
	DWORD col_id_num=0;

	for(i=1; i<bsp->mLeafNum; i++)
	{
		BOOL is_in_leaf = FALSE;
		col_leaf[i].start_id = col_id_num;

		for(j=1; j<col_leaf_num; j++)
		{
			if( l_list[j] == i  )
			{
				col_leaf[i].line_num++;
				col_line_id[col_id_num++]=coll_index[j];
				if( col_id_num >= max_l_id )
				{
					col_line_id=(WORD *)ReAlloc(col_line_id,sizeof(WORD)*max_l_id,sizeof(WORD)*(max_l_id+4000));
					max_l_id+=4000;
				}
			}
		}
	}

	Dfree(l_list);
	Dfree(coll_index);


	add_bsp->ColLeaf = col_leaf;
	add_bsp->ColLineID = col_line_id;
	add_bsp->ColPoint = c_point;
	add_bsp->ColLine = c_line;

	if( mColPointCnt != 1 )
	{
		bsp->mExtBSPHeader.CFVertex.size = sizeof(_TOOL_COL_POINT)*mColPointCnt;
		bsp->mExtBSPHeader.CFLine.size = sizeof(_TOOL_COL_LINE)*mColLineCnt;
		bsp->mExtBSPHeader.CFLineId.size = sizeof(WORD)*col_id_num;
		bsp->mExtBSPHeader.CFLeaf.size = sizeof(_TOOL_COL_LEAF)*bsp->mLeafNum;
	}
	else
	{
		bsp->mExtBSPHeader.CFVertex.size = 0;
		bsp->mExtBSPHeader.CFLine.size = 0;
		bsp->mExtBSPHeader.CFLineId.size = 0;
		bsp->mExtBSPHeader.CFLeaf.size = 0;
	}
}


void CToolCollisionFace::ReleaseCollisonInfoInBSP(void *v_add_bsp)
{
	_ADD_BSP_SAVE *add_bsp = (_ADD_BSP_SAVE *)v_add_bsp;
	if( add_bsp->ColLeaf )
		Dfree( add_bsp->ColLeaf );
	if( add_bsp->ColLineID )
		Dfree( add_bsp->ColLineID );
	add_bsp->ColLeaf = NULL; 
	add_bsp->ColLineID = NULL; 
}

