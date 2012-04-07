#include "collision.h"
#include "R3bsp.h"
#include "Core.h"
#include "jerror.h"
#include "R3lightmap.h"

#pragma optimize( "atp", on )


//void 
#define _ON_EPSILON	0.01

static void CrossVector( Vector3f a, Vector3f b, Vector3f r )
{
    r[0] = (a[1] * b[2]) - (a[2] * b[1]);
    r[1] = (a[2] * b[0]) - (a[0] * b[2]);
    r[2] = (a[0] * b[1]) - (a[1] * b[0]);
}
inline int CheckEdge(Vector3f a,Vector3f b,Vector3f v,Vector3f f_normal,float d)
{
	Vector3f vv,n;
	double dp;

	vv[0] = a[0] - b[0];
	vv[1] = a[1] - b[1];
	vv[2] = a[2] - b[2];
    CrossVector(f_normal, vv, n);
    dp = (n[0] * ( v[0] - b[0])	+ n[1] * ( v[1] - b[1])	+ n[2] * ( v[2] - b[2]));
    if( dp > 0 )
        return 0;
	return 1;	//  통과 
}

int CheckEdgeEpsilon(Vector3f a,Vector3f b,Vector3f v,Vector3f f_normal,float d)
{
	Vector3f vv,n;
	double dp;

	vv[0] = a[0] - b[0];
	vv[1] = a[1] - b[1];
	vv[2] = a[2] - b[2];
    CrossVector(f_normal, vv, n);
    dp = (n[0] * ( v[0] - b[0])	+ n[1] * ( v[1] - b[1])	+ n[2] * ( v[2] - b[2]));
    if( dp > _ON_EPSILON )
        return 0;
	return 1;	//  통과 
}

DWORD GetPlaneCrossPoint(Vector3f a,Vector3f b,Vector3f cross,Vector3f f_normal,float d)
{
    float front = f_normal[0]*a[0] + f_normal[1]*a[1] + f_normal[2]*a[2] -d;
    float back  = f_normal[0]*b[0] + f_normal[1]*b[1] + f_normal[2]*b[2] -d;

    if ( (front > 0 && back > 0) || (front < 0 && back < 0)) 
		return FALSE;

    front = front / (front-back);

    cross[0] = a[0] + front*(b[0]-a[0]);
    cross[1] = a[1] + front*(b[1]-a[1]);
    cross[2] = a[2] + front*(b[2]-a[2]);

	if( front <= 0 )	//앞으로 부딪힌경우.
		return 1;
	else
		return -1;		//뒤로 부딪힌경우.
}

BOOL GetPlaneCrossPoint(Vector3f a,Vector3f b,Vector3f cross,Vector3f f_normal,float d,float option)
{
    float front = f_normal[0]*a[0] + f_normal[1]*a[1] + f_normal[2]*a[2] -d;
    float back  = f_normal[0]*b[0] + f_normal[1]*b[1] + f_normal[2]*b[2] -d;

    if ( (front < 0 && back > 0)|| (front >= 0 && back >= 0) || (front < 0 && back < 0)) 
		return FALSE;

    front = front / (front-back);

    cross[0] = a[0] + front*(b[0]-a[0]);
    cross[1] = a[1] + front*(b[1]-a[1]);
    cross[2] = a[2] + front*(b[2]-a[2]);

	return TRUE;
}

//주의 : 라이트맵이 없는경우는 color 값으로 해야된다.
void CBsp::GetLightMapUVFromPoint(Vector2f getuv,int face_id,Vector3f p)	//평면위의 한 점을 넘겨주면 uv를 구해준다.
{
	_BSP_C_FACE *face=&mCFace[face_id];
	int v0,v1,v2,r0,r1,r2;
	int i;
	Vector3f temp;
	float dist1,dist2,dist0;

	LONG start_v = face->VStartId;
	for(i=0; i<face->VNum-2; i++)
	{
		v0 = start_v+0;
		v1 = start_v+i+1;
		v2 = start_v+i+2;
		// 삼각형 안이면 
		if( (CheckEdge( mCVertex[mCVertexId[v0]] , mCVertex[mCVertexId[v1]] , p ,  face->Normal,face->Normal[3]) == 1)
		&& (CheckEdge( mCVertex[mCVertexId[v1]] , mCVertex[mCVertexId[v2]] , p ,  face->Normal,face->Normal[3]) == 1)
		&& (CheckEdge( mCVertex[mCVertexId[v2]] , mCVertex[mCVertexId[v0]] , p ,  face->Normal,face->Normal[3]) == 1) )
		{
			break;
		}
	}	
	Vector3fSub(mCVertex[mCVertexId[v0]],p,temp);
	dist0=Vector3fDist(temp);
	Vector3fSub(mCVertex[mCVertexId[v1]],p,temp);
	dist1=Vector3fDist(temp);
	Vector3fSub(mCVertex[mCVertexId[v2]],p,temp);
	dist2=Vector3fDist(temp);
	if( dist0 > dist1)	//오차문제땜에 거리가 가장 큰걸로하자..
	{
		if( dist0 > dist2)
		{
			r0=v0;
			r1=v1;
			r2=v2;
		}
		else
		{
			r0=v2;
			r1=v0;
			r2=v1;
		}
	}
	else
	{
		if( dist1 > dist2)
		{
			r0=v1;
			r1=v2;
			r2=v0;
		}
		else
		{
			r0=v2;
			r1=v0;
			r2=v1;
		}
	}
	Vector3f a,b;
	float leng0,leng1;

	Vector3fSub(mCVertex[mCVertexId[r1]],mCVertex[mCVertexId[r0]],a);
	leng0 = Vector3fDist(a);
	Vector3fSub(mCVertex[mCVertexId[r2]],mCVertex[mCVertexId[r0]],b);
	leng1 = Vector3fDist(b);
	Vector3fSub(p,mCVertex[mCVertexId[r0]],temp);
	dist2 = Vector3fDist(temp);

	Normalize(a);
	Normalize(b);
	Normalize(temp);
	dist0 = Vector3fDot(a,temp);	//a 각도.
	dist0 = sqrtf(1-dist0*dist0);		//사인 값
	dist1 = Vector3fDot(b,temp);	//b 각도.
	dist1 = sqrtf(1-dist1*dist1);		//사인 값
	dist0 *= leng0;
	dist1 *= leng1;

	//길이에 관한 a의 비율
	dist0 = dist0/(dist1+dist0);
	Vector3fSub(mCVertex[mCVertexId[r2]],mCVertex[mCVertexId[r1]],a);
	Vector3fScale(a,dist0,a);
	Vector3fAdd(a,mCVertex[mCVertexId[r1]],a);	//교점을 구했다.

	Vector3fSub(a,mCVertex[mCVertexId[r0]],b);
	dist1 = Vector3fDist(b);
	dist1 = dist2/dist1;			// 2번째 비례

	Vector2f l_uv[3];	//라이트맵은 short으로 저장되어있으므로 float로 변환하자.
	l_uv[0][0] = mLgtUV[r0][0]/32767.0f;
	l_uv[0][1] = mLgtUV[r0][1]/32767.0f;
	l_uv[1][0] = mLgtUV[r1][0]/32767.0f;
	l_uv[1][1] = mLgtUV[r1][1]/32767.0f;
	l_uv[2][0] = mLgtUV[r2][0]/32767.0f;
	l_uv[2][1] = mLgtUV[r2][1]/32767.0f;

	//---마무리계산...
	getuv[0] = (l_uv[2][0] - l_uv[1][0])*dist0 + l_uv[1][0];
	getuv[1] = (l_uv[2][1] - l_uv[1][1])*dist0 + l_uv[1][1];

	getuv[0] = (getuv[0] - l_uv[0][0])*dist1 + l_uv[0][0];
	getuv[1] = (getuv[1] - l_uv[0][1])*dist1 + l_uv[0][1];
}
//주의 : 라이트맵이 없는경우는 color 값으로 해야된다.

DWORD CBsp::GetColorFromPoint(int face_id,Vector3f p)	//평면위의 한 점을 넘겨주면 uv를 구해준다.
{
	_BSP_C_FACE *face=&mCFace[face_id];
	int v0,v1,v2,r0,r1,r2;
	int i;
	Vector3f temp;
	float dist1,dist2,dist0;

	LONG start_v = face->VStartId;
	for(i=0; i<face->VNum-2; i++)
	{
		v0 = start_v+0;
		v1 = start_v+i+1;
		v2 = start_v+i+2;
		// 삼각형 안이면 
		if( (CheckEdge( mCVertex[mCVertexId[v0]] , mCVertex[mCVertexId[v1]] , p ,  face->Normal,face->Normal[3]) == 1)
		&& (CheckEdge( mCVertex[mCVertexId[v1]] , mCVertex[mCVertexId[v2]] , p ,  face->Normal,face->Normal[3]) == 1)
		&& (CheckEdge( mCVertex[mCVertexId[v2]] , mCVertex[mCVertexId[v0]] , p ,  face->Normal,face->Normal[3]) == 1) )
		{
			break;
		}
	}	
	Vector3fSub(mCVertex[mCVertexId[v0]],p,temp);
	dist0=Vector3fDist(temp);
	Vector3fSub(mCVertex[mCVertexId[v1]],p,temp);
	dist1=Vector3fDist(temp);
	Vector3fSub(mCVertex[mCVertexId[v2]],p,temp);
	dist2=Vector3fDist(temp);
	if( dist0 > dist1)	//오차문제땜에 거리가 가장 큰걸로하자..
	{
		if( dist0 > dist2)
		{
			r0=v0;
			r1=v1;
			r2=v2;
		}
		else
		{
			r0=v2;
			r1=v0;
			r2=v1;
		}
	}
	else
	{
		if( dist1 > dist2)
		{
			r0=v1;
			r1=v2;
			r2=v0;
		}
		else
		{
			r0=v2;
			r1=v0;
			r2=v1;
		}
	}
	Vector3f a,b;
	float leng0,leng1;

	Vector3fSub(mCVertex[mCVertexId[r1]],mCVertex[mCVertexId[r0]],a);
	leng0 = Vector3fDist(a);
	Vector3fSub(mCVertex[mCVertexId[r2]],mCVertex[mCVertexId[r0]],b);
	leng1 = Vector3fDist(b);
	Vector3fSub(p,mCVertex[mCVertexId[r0]],temp);
	dist2 = Vector3fDist(temp);

	Normalize(a);
	Normalize(b);
	Normalize(temp);
	dist0 = Vector3fDot(a,temp);	//a 각도.
	dist0 = sqrtf(1-dist0*dist0);		//사인 값
	dist1 = Vector3fDot(b,temp);	//b 각도.
	dist1 = sqrtf(1-dist1*dist1);		//사인 값
	dist0 *= leng0;
	dist1 *= leng1;

	//길이에 관한 a의 비율
	dist0 = dist0/(dist1+dist0);
	Vector3fSub(mCVertex[mCVertexId[r2]],mCVertex[mCVertexId[r1]],a);
	Vector3fScale(a,dist0,a);
	Vector3fAdd(a,mCVertex[mCVertexId[r1]],a);	//교점을 구했다.

	Vector3fSub(a,mCVertex[mCVertexId[r0]],b);
	dist1 = Vector3fDist(b);
	dist1 = dist2/dist1;			// 2번째 비례

	float red[3],green[3],blue[3];
	float rr,gg,bb;
	red[0] = ((mVertexColor[mCVertexId[r0]]>>16)&255)/255.0f;
	red[1] = ((mVertexColor[mCVertexId[r1]]>>16)&255)/255.0f;
	red[2] = ((mVertexColor[mCVertexId[r2]]>>16)&255)/255.0f;
	green[0] = ((mVertexColor[mCVertexId[r0]]>>8)&255)/255.0f;
	green[1] = ((mVertexColor[mCVertexId[r1]]>>8)&255)/255.0f;
	green[2] = ((mVertexColor[mCVertexId[r2]]>>8)&255)/255.0f;
	blue[0] = ((mVertexColor[mCVertexId[r0]]>>0)&255)/255.0f;
	blue[1] = ((mVertexColor[mCVertexId[r1]]>>0)&255)/255.0f;
	blue[2] = ((mVertexColor[mCVertexId[r2]]>>0)&255)/255.0f;

	//---마무리계산...
	rr = (red[2] - red[1])*dist0 + red[1];
	rr = (rr - red[0])*dist1 + red[0];
	gg = (green[2] - green[1])*dist0 + green[1];
	gg = (gg - green[0])*dist1 + green[0];
	bb = (blue[2] - blue[1])*dist0 + blue[1];
	bb = (bb - blue[0])*dist1 + blue[0];
/*
	// 2x비율낮추자...
	float max_color=max(rr,gg);
	max_color=max(max_color,bb);
	if( max_color > 1.0f )
	{
		float div_rate = 1.0f/(float)max_color;
		rr = (rr*div_rate);
		gg = (gg*div_rate);
		bb = (bb*div_rate);
	}
*/
	//multiply 가 2X래서 
	return 0xff000000|((DWORD)(rr*255)<<16)|((DWORD)(gg*255)<<8)|((DWORD)(bb*255));
}


float CBsp::GetYposInLeaf(Vector3f a,Vector3f b,float able_height,float now_height,int leaf_id)// able은 전진 가능한 높이,now_현재 높이,leaf는 얻을 리프
{
	int i,j;

	float ret=-32000;

	int start_f = mLeaf[leaf_id].face_start_id;
	for(i=0; i<mLeaf[leaf_id].face_num; i++)//리프 페이스의 갯수만큼
	{
		LONG f = mCFaceId[start_f+i];

		if( mCFace[f].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
			continue;

//		if( !(mCFace[f].Attr & _CFACE_ATTR_FLOOR) )
//			continue;

		Vector3f cross;
		LONG start_v = mCFace[f].VStartId;
	
		//교차하는 면이라면
		if( GetPlaneCrossPoint(a, b, cross, mCFace[f].Normal,mCFace[f].Normal[3],0) == FALSE ) 
			continue;

		int cnt=0;

		for(j=0; j<mCFace[f].VNum; j++)
		{
			LONG v0 = mCVertexId[start_v+j];
			LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];

			if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross ,  mCFace[f].Normal,mCFace[f].Normal[3]) == 0)	
				continue;
			cnt++;
		}

		if(cnt == mCFace[f].VNum) //충돌이 된다.
		{
			if(ret < cross[1] && cross[1] <  able_height+now_height
				&& cross[1] >  -able_height+now_height)	
			{
				ret = cross[1];
				mNowCFaceId=f;
			}
		}
	}
	return ret;
}

static DWORD CollisionLevel(float org,float best_add_height,float col_height)	//0에 가까울수록 아주 좋은 단계다.
{
	float best_height = org + best_add_height;
	if( org <= col_height )
	{
		return (DWORD)((col_height-org)/(best_height - org))*2;
	}
	else
	{
		return (DWORD)((org-col_height)/(best_height - org))*2+1;
	}
}

float CBsp::GetBestYposInLeaf(Vector3f a,Vector3f b,float best_add_height,float now_height,int leaf_id)// able은 전진 가능한 높이,now_현재 높이,leaf는 얻을 리프
{
	int i,j;

	float ret=-32000;
	DWORD best_level=10000;

	int start_f = mLeaf[leaf_id].face_start_id;
	for(i=0; i<mLeaf[leaf_id].face_num; i++)//리프 페이스의 갯수만큼
	{
		LONG f = mCFaceId[start_f+i];

		if( mCFace[f].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
			continue;

		Vector3f cross;
		LONG start_v = mCFace[f].VStartId;
	
		//교차하는 면이라면
		if( GetPlaneCrossPoint(a, b, cross, mCFace[f].Normal,mCFace[f].Normal[3],0) == FALSE ) 
			continue;

		int cnt=0;

		for(j=0; j<mCFace[f].VNum; j++)
		{
			LONG v0 = mCVertexId[start_v+j];
			LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];

			if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross ,  mCFace[f].Normal,mCFace[f].Normal[3]) == 0)	
				continue;
			cnt++;
		}

		if(cnt == mCFace[f].VNum) //충돌이 된다.
		{
			DWORD h_level = CollisionLevel(now_height,best_add_height,cross[1]);
			if( best_level == h_level ) 
			{
				if( cross[1] > ret )
				{
					best_level = h_level;
					ret = cross[1];
					mNowCFaceId=f;
				}
			}
			else
			if( best_level > h_level ) 
			{
				best_level = h_level;
				ret = cross[1];
				mNowCFaceId=f;
			}
		}
	}
	return ret;
}




float CBsp::GetYposInLeafNoAttr(Vector3f a,Vector3f b,float able_height,float now_height,int leaf_id)// able은 전진 가능한 높이,now_현재 높이,leaf는 얻을 리프
{
	int i,j;

	float ret=-32000;

	int start_f = mLeaf[leaf_id].face_start_id;
	for(i=0; i<mLeaf[leaf_id].face_num; i++)//리프 페이스의 갯수만큼
	{
		LONG f = mCFaceId[start_f+i];

		if( mCFace[f].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
			continue;
//		if( !(mCFace[f].Attr & _CFACE_ATTR_FLOOR) )
//			continue;

		Vector3f cross;
		LONG start_v = mCFace[f].VStartId;
	
		//교차하는 면이라면
		if( GetPlaneCrossPoint(a, b, cross, mCFace[f].Normal,mCFace[f].Normal[3],0) == FALSE ) 
			continue;

		int cnt=0;

		for(j=0; j<mCFace[f].VNum; j++)
		{
			LONG v0 = mCVertexId[start_v+j];
			LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];

			if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross ,  mCFace[f].Normal,mCFace[f].Normal[3]) == 0)	
				continue;
			cnt++;
		}

		if(cnt == mCFace[f].VNum) //충돌이 된다.
		{
			if(ret < cross[1] && cross[1] <  able_height+now_height
				&& cross[1] >  -able_height+now_height)	
			{
				ret = cross[1];
				mNowCFaceId=f;
			}
		}
	}
	return ret;
}

inline static int CheckEdgeNormal(Vector4f edge_n,Vector3f p)
{
	if( edge_n[0]*p[0]+edge_n[1]*p[1]+edge_n[2]*p[2]-edge_n[3] > -_ON_EPSILON)
		return 0;
	return 1;
}

float CBsp::GetYposInLeafUseEdgeNormal(Vector3f a,Vector3f b,float able_height,float now_height,int leaf_id)// able은 전진 가능한 높이,now_현재 높이,leaf는 얻을 리프
{
	int i,j;

	if( !mCNEdgeNormal )
	{
		Warning("GetYposInLeafUseEdgeNormal()함수는 서버에서 쓸겁니다.","");
		return 0;
	}
	float ret=-32000;

	int start_f = mLeaf[leaf_id].face_start_id;
	for(i=0; i<mLeaf[leaf_id].face_num; i++)//리프 페이스의 갯수만큼
	{
		LONG f = mCFaceId[start_f+i];

		if( mCFace[f].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
			continue;
//		if( !(mCFace[f].Attr & _CFACE_ATTR_FLOOR) )
//			continue;

		Vector3f cross;
		LONG start_v = mCFace[f].VStartId;
	
		//교차하는 면이라면
		if( GetPlaneCrossPoint(a, b, cross, mCFace[f].Normal,mCFace[f].Normal[3],0) == FALSE ) 
			continue;

		int cnt=0;

		for(j=0; j<mCFace[f].VNum; j++)
		{
			if( CheckEdgeNormal(mCNEdgeNormal[(f<<2)+j],cross) )
				break;
			cnt++;
		}

		if(cnt == mCFace[f].VNum) //충돌이 된다.
		{
			if(ret < cross[1] && cross[1] <  able_height+now_height
				&& cross[1] >  -able_height+now_height)	
			{
				ret = cross[1];
				mNowCFaceId=f;
			}
		}
	}
	return ret;
}

DWORD CBsp::GetLightFromPoint(Vector3f p,DWORD diff) //현재 밟고있는 평면위의 한 점을 넘겨주면 맵의 라이팀을 구해준다.
{
	Vector2f l_uv;
	DWORD color;

	Vector3f a,b;
	Vector3fCopy(p,a);
	a[1] += diff;
	Vector3fCopy(p,b);
	b[1] -= diff;

	LONG leaf_num=0,i,j,k;
	short leaf_id[1256];

	GetLeafList(a,b,&leaf_num,leaf_id,1255);

	DWORD face_id=-1;

	float dist = 100000000;
	BOOL is_col=FALSE;

	for(k=0; k<leaf_num; k++)
	{
		int start_f = mLeaf[leaf_id[k]].face_start_id;
		for(i=0; i<mLeaf[leaf_id[k]].face_num; i++)//리프 페이스의 갯수만큼
		{
			Vector3f cross;
			LONG f = mCFaceId[start_f+i];
			if( mCFace[f].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
				continue;

			//교차하는 면이라면
			if( GetPlaneCrossPoint(a, b, cross, mCFace[f].Normal,mCFace[f].Normal[3],0) == FALSE ) 
				continue;
			LONG start_v = mCFace[f].VStartId;
			int cnt=0;
			for(j=0; j<mCFace[f].VNum; j++)
			{
				LONG v0 = mCVertexId[start_v+j];
				LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];

				if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross , mCFace[f].Normal,mCFace[f].Normal[3]) == 0)	
					continue;
				cnt++;
			}

			if(cnt == mCFace[f].VNum) //충돌이 된다.
			{
				Vector3f vec_dist;
				Vector3fSub(cross,a,vec_dist);
				float temp_d = Vector3fDot(vec_dist,vec_dist);
				if(dist > temp_d)
				{
					is_col = TRUE;
					dist = temp_d;
					face_id = f;
				}
			}
		}
	}
/*
	for( i=0; i<leaf_num; i++)
	{
		GetYposInLeaf(a,b,10,p[1],leaf_id[i] );
//		GetBestYposInLeaf(a,b,10,p[1],leaf_id[i] );
		if( mNowCFaceId != -1)
		{
			face_id=;
		}
	}*/
	if( face_id == -1 )
		return 0xff808080;
//		return 0xffffffff;

	int lgt_id = mMatGroup[mCFace[face_id].MatGIndex].LgtId;
	if( lgt_id !=-1 )	//버텍스 라이트일때
	{
		GetLightMapUVFromPoint(l_uv,face_id,p);	//평면위의 한 점을 넘겨주면 uv를 구해준다.
		//해당라이트맵아이디와 라이트맵 uv를 넣너주면 컬러값을 얻어온다.
		color = GetLightMapColor( l_uv,lgt_id );	
	}
	else
		color = GetColorFromPoint(face_id,p);	//평면위의 한 점을 넘겨주면 컬러를 구해준다.

	DWORD r_color = (color>>16)&0xff;
	DWORD g_color = (color>>8)&0xff;
	DWORD b_color = (color>>0)&0xff;

#define TOTAL_PERCENT		0.65f
#define DOWN_PERCENT		0.1f

	r_color = (DWORD)(r_color*TOTAL_PERCENT + 255.0f*DOWN_PERCENT);
	g_color = (DWORD)(g_color*TOTAL_PERCENT + 255.0f*DOWN_PERCENT);
	b_color = (DWORD)(b_color*TOTAL_PERCENT + 255.0f*DOWN_PERCENT);

	return 0xff000000|(r_color<<16)|(g_color<<8)|(b_color);
}

float CBsp::GetFirstYpos(Vector3f pos,int option)//leaf는 얻을 리프,
{
	int i,j,k;

	float ret;	
	if( option == -1 )	//가장 충돌이 낮은것을 리턴한다.
		ret=32000;
	else
		ret=-32000;


	Vector3f a,b;
	Vector3fCopy(pos,a);
	Vector3fCopy(pos,b);

	a[1] = 30000;
	b[1] = -30000;

	LONG leaf_num=0;
	SHORT leaf_id[32000];
	GetLeafList(a,b,&leaf_num,leaf_id,32000);

	for(k=0; k<leaf_num; k++)
	{
		int start_f = mLeaf[leaf_id[k]].face_start_id;
		for(i=0; i<mLeaf[leaf_id[k]].face_num; i++)//리프 페이스의 갯수만큼
		{
			Vector3f cross;

			LONG f = mCFaceId[start_f+i];
			if( mCFace[f].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
				continue;
//			if( !(mCFace[f].Attr & _CFACE_ATTR_FLOOR) )
//				continue;
		
			//교차하는 면이라면

			if( GetPlaneCrossPoint(a, b, cross, mCFace[f].Normal,mCFace[f].Normal[3],0) == FALSE ) 
				continue;

			LONG start_v = mCFace[f].VStartId;
			int cnt=0;

			for(j=0; j<mCFace[f].VNum; j++)
			{
				LONG v0 = mCVertexId[start_v+j];
				LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];

				if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross , mCFace[f].Normal,mCFace[f].Normal[3]) == 0)	
					continue;
				cnt++;
			}

			if(cnt == mCFace[f].VNum) //충돌이 된다.
			{
				if( option == -1 )	//가장 충돌위치가 낮은것을 리턴한다.
				{
					if(ret > cross[1])	
					{
						ret = cross[1];
						mNowCFaceId=f;
					}
				}
				else
				{
					if(ret < cross[1])
					{
						ret = cross[1];
						mNowCFaceId=f;
					}
				}
			}
		}
	}
	if( fabs(ret)==32000)
		return 65535;
	return ret;
}
float CBsp::GetFirstYpos(Vector3f pos,Vector3s min,Vector3s max)
{
	int i,j,k;

	float ret;	
	ret=-65535;

	Vector3f a,b;
	Vector3fCopy(pos,a);
	Vector3fCopy(pos,b);

	a[1] = max[1];
	b[1] = min[1];

	LONG leaf_num=0;
	SHORT leaf_id[32000];
	GetLeafList(a,b,&leaf_num,leaf_id,32000);

	for(k=0; k<leaf_num; k++)
	{
		int start_f = mLeaf[leaf_id[k]].face_start_id;
		for(i=0; i<mLeaf[leaf_id[k]].face_num; i++)//리프 페이스의 갯수만큼
		{
			Vector3f cross;

			LONG f = mCFaceId[start_f+i];
			if( mCFace[f].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
				continue;
//			if( !(mCFace[f].Attr & _CFACE_ATTR_FLOOR) )
//				continue;
		
			//교차하는 면이라면
			if( GetPlaneCrossPoint(a, b, cross, mCFace[f].Normal,mCFace[f].Normal[3],0) == FALSE ) 
				continue;

			LONG start_v = mCFace[f].VStartId;
			int cnt=0;

			for(j=0; j<mCFace[f].VNum; j++)
			{
				LONG v0 = mCVertexId[start_v+j];
				LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];

				if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross , mCFace[f].Normal,mCFace[f].Normal[3]) == 0)	
					continue;
				cnt++;
			}

			if(cnt == mCFace[f].VNum) //충돌이 된다.
			{
				if( a[1] > cross[1] && cross[1] > b[1] )	//min과 max값의 사이에 있어야한다.
				{
					//가장 충돌위치가 높은 것을 리턴한다.
					if(ret < cross[1])	
					{
						ret = cross[1];
						mNowCFaceId=f;
					}
				}
			}
		}
	}
	return ret;
}

BOOL CBsp::IsCollisionFace(Vector3f src,Vector3f tar)	//두벡터사이에 폴리곤이 충돌이되나? 코로나...
{
	LONG leaf_num=0,i,j,k;
	SHORT leaf_id[32000];
	GetLeafList(src,tar,&leaf_num,leaf_id,32000);

	for(k=0; k<leaf_num; k++)
	{
		int start_f = mLeaf[leaf_id[k]].face_start_id;
		for(i=0; i<mLeaf[leaf_id[k]].face_num; i++)//리프 페이스의 갯수만큼
		{
			Vector3f cross;

			LONG f = mCFaceId[start_f+i];

			if( mCFace[f].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
				continue;
		
			//교차하는 면이라면

			if( GetPlaneCrossPoint(src, tar, cross, mCFace[f].Normal,mCFace[f].Normal[3],0) == FALSE ) 
				continue;

			LONG start_v = mCFace[f].VStartId;
			int cnt=0;

			for(j=0; j<mCFace[f].VNum; j++)
			{
				LONG v0 = mCVertexId[start_v+j];
				LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];

				if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross , mCFace[f].Normal,mCFace[f].Normal[3]) == 0)	
					continue;
				cnt++;
			}
			/*
			if(cnt == mCFace[f].VNum) //충돌이 된다.
			{
				GetPlaneCrossPoint(src, tar, cross, mCFace[f].Normal,mCFace[f].Normal[3],0);
			}*/

			if(cnt == mCFace[f].VNum) //충돌이 된다.
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CBsp::IsCollisionFaceForServer(Vector3f src,Vector3f tar)	//두벡터사이에 폴리곤이 충돌이되나? 코로나...
{
	LONG leaf_num=0,i,j,k;
	SHORT leaf_id[32000];
	GetLeafList(src,tar,&leaf_num,leaf_id,32000);

	for(k=0; k<leaf_num; k++)
	{
		int start_f = mLeaf[leaf_id[k]].face_start_id;
		for(i=0; i<mLeaf[leaf_id[k]].face_num; i++)//리프 페이스의 갯수만큼
		{
			Vector3f cross;

			LONG f = mCFaceId[start_f+i];

			if( mCFace[f].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
				continue;
		
			//교차하는 면이라면

			if( GetPlaneCrossPoint(src, tar, cross, mCFace[f].Normal,mCFace[f].Normal[3],0) == FALSE ) 
				continue;

			LONG start_v = mCFace[f].VStartId;
			int cnt=0;

			for(j=0; j<mCFace[f].VNum; j++)
			{
				LONG v0 = mCVertexId[start_v+j];
				LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];

				if( CheckEdgeNormal(mCNEdgeNormal[(f<<2)+j],cross) )
					break;
//				if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross , mCFace[f].Normal,mCFace[f].Normal[3]) == 0)	
//					continue;
				cnt++;
			}
			/*
			if(cnt == mCFace[f].VNum) //충돌이 된다.
			{
				GetPlaneCrossPoint(src, tar, cross, mCFace[f].Normal,mCFace[f].Normal[3],0);
			}*/

			if(cnt == mCFace[f].VNum) //충돌이 된다.
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
BOOL CBsp::IsCollisionFace(Vector3f src,Vector3f tar,Vector3f *get_pos,Vector4f *get_normal)	//충돌된 위치와 평면의 법선벡터를 리턴 
{
	LONG leaf_num=0,i,j,k,p_index;
	SHORT leaf_id[32000];
	GetLeafList(src,tar,&leaf_num,leaf_id,32000);

	float dist = 100000000;
	BOOL is_col=FALSE;

	for(k=0; k<leaf_num; k++)
	{
		int start_f = mLeaf[leaf_id[k]].face_start_id;
		for(i=0; i<mLeaf[leaf_id[k]].face_num; i++)//리프 페이스의 갯수만큼
		{
			Vector3f cross;

			LONG f = mCFaceId[start_f+i];

			if( mCFace[f].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
				continue;
		
			//교차하는 면이라면

			if( GetPlaneCrossPoint(src, tar, cross, mCFace[f].Normal,mCFace[f].Normal[3],0) == FALSE ) 
				continue;

			LONG start_v = mCFace[f].VStartId;
			int cnt=0;

			for(j=0; j<mCFace[f].VNum; j++)
			{
				LONG v0 = mCVertexId[start_v+j];
				LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];

				if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross , mCFace[f].Normal,mCFace[f].Normal[3]) == 0)	
					continue;
				cnt++;
			}

			if(cnt == mCFace[f].VNum) //충돌이 된다.
			{
				Vector3f vec_dist;
				Vector3fSub(cross,src,vec_dist);
				float temp_d = Vector3fDot(vec_dist,vec_dist);
				if(dist > temp_d)
				{
					is_col = TRUE;
					dist = temp_d;
					p_index = f;
					get_pos[0][0]=cross[0];
					get_pos[0][1]=cross[1];
					get_pos[0][2]=cross[2];
				}
			}
		}
	}
	if( is_col )
	{
		Vector4fCopy(mCFace[p_index].Normal,get_normal[0]);
		return TRUE;
	}

	return FALSE;
}
BOOL CBsp::IsCollisionFace(Vector3f src,Vector3f tar,Vector3f *get,float plane_dist)	//평면과의 떨어진 거리를 리턴
{
	LONG leaf_num=0,i,j,k,p_index;
	SHORT leaf_id[32000];
	GetLeafList(src,tar,&leaf_num,leaf_id,32000);

	float dist = 100000000;
	BOOL is_col=FALSE;

	for(k=0; k<leaf_num; k++)
	{
		int start_f = mLeaf[leaf_id[k]].face_start_id;
		for(i=0; i<mLeaf[leaf_id[k]].face_num; i++)//리프 페이스의 갯수만큼
		{
			Vector3f cross;

			LONG f = mCFaceId[start_f+i];
			if( mCFace[f].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
				continue;
			
			//교차하는 면이라면
			DWORD is_cross = GetPlaneCrossPoint(src, tar, cross, mCFace[f].Normal,mCFace[f].Normal[3],0);
			if( !is_cross )
				continue;

			LONG start_v = mCFace[f].VStartId;
			int cnt=0;

			for(j=0; j<mCFace[f].VNum; j++)
			{
				LONG v0 = mCVertexId[start_v+j];
				LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];

				if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross , mCFace[f].Normal,mCFace[f].Normal[3]) == 0)	
					continue;
				cnt++;
			}

			if(cnt == mCFace[f].VNum) //충돌이 된다.
			{
				Vector3f vec_dist;
				Vector3fSub(cross,src,vec_dist);
				float temp_d = Vector3fDot(vec_dist,vec_dist);
				if(dist > temp_d)
				{
					is_col = TRUE;
					dist = temp_d;
					p_index = f;
					get[0][0]=cross[0];
					get[0][1]=cross[1];
					get[0][2]=cross[2];
				}
			}
		}
	}
	if( is_col )
	{
		Vector3f l_normal;
		Vector3fSub(src,tar,l_normal);
		Normalize(l_normal);
		float su=Vector3fDot(l_normal,mCFace[p_index].Normal);
		su=plane_dist/su;
		get[0][0] = l_normal[0]*su+get[0][0];
		get[0][1] = l_normal[1]*su+get[0][1];
		get[0][2] = l_normal[2]*su+get[0][2];
		return TRUE;
	}

	return FALSE;
}

BOOL CBsp::IsInWater(Vector3f src,Vector3f tar,Vector3f *get,float plane_dist)	// 시작과 끝이 워터 속인가.주의 src는 물위좌표즉 위쪽이여야 한다.
{
	LONG leaf_num=0,i,j,k;
	SHORT leaf_id[32000];
	GetLeafList(src,tar,&leaf_num,leaf_id,32000);

	float dist = 100000000;
	BOOL is_col=FALSE;

	for(k=0; k<leaf_num; k++)
	{
		int start_f = mLeaf[leaf_id[k]].face_start_id;
		for(i=0; i<mLeaf[leaf_id[k]].face_num; i++)//리프 페이스의 갯수만큼
		{
			Vector3f cross;

			LONG f = mCFaceId[start_f+i];
			if( !(mCFace[f].Attr & _CFACE_WATER) )	//물속성이 아니면.
				continue;

			//교차하는 면이라면
			DWORD is_cross = GetPlaneCrossPoint(src, tar, cross, mCFace[f].Normal,mCFace[f].Normal[3]);
			
			if( is_cross != -1 )	//물하고는 거꾸로 충돌해야되므로.
				continue;
			
			LONG start_v = mCFace[f].VStartId;
			int cnt=0;

			for(j=0; j<mCFace[f].VNum; j++)
			{
				LONG v0 = mCVertexId[start_v+j];
				LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];

				if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross , mCFace[f].Normal,mCFace[f].Normal[3]) == 0)	
					continue;
				cnt++;
			}

			if(cnt == mCFace[f].VNum) //충돌이 된다. 물과 충돌된다.
			{
				if( get )
				{
					Vector3f l_normal;
					Vector3fSub(src,tar,l_normal);
					Normalize(l_normal);
					float su=Vector3fDot(l_normal,mCFace[f].Normal);
					su=plane_dist/su;
					get[0][0] = l_normal[0]*su+cross[0];
					get[0][1] = l_normal[1]*su+cross[1];
					get[0][2] = l_normal[2]*su+cross[2];
				}
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CBsp::IsCollisionFaceForServer(Vector3f src,Vector3f tar,Vector3f *get,float plane_dist)	//평면과의 떨어진 거리를 리턴
{
	LONG leaf_num=0,i,j,k,p_index;
	SHORT leaf_id[32000];
	GetLeafList(src,tar,&leaf_num,leaf_id,32000);

	float dist = 100000000;
	BOOL is_col=FALSE;

	for(k=0; k<leaf_num; k++)
	{
		int start_f = mLeaf[leaf_id[k]].face_start_id;
		for(i=0; i<mLeaf[leaf_id[k]].face_num; i++)//리프 페이스의 갯수만큼
		{
			Vector3f cross;

			LONG f = mCFaceId[start_f+i];

			if( mCFace[f].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
				continue;
		
			//교차하는 면이라면

			if( GetPlaneCrossPoint(src, tar, cross, mCFace[f].Normal,mCFace[f].Normal[3],0) == FALSE ) 
				continue;

			LONG start_v = mCFace[f].VStartId;
			int cnt=0;

			for(j=0; j<mCFace[f].VNum; j++)
			{
				LONG v0 = mCVertexId[start_v+j];
				LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];

				if( CheckEdgeNormal(mCNEdgeNormal[(f<<2)+j],cross) )
					break;
//				if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross , mCFace[f].Normal,mCFace[f].Normal[3]) == 0)	
//					continue;
				cnt++;
			}

			if(cnt == mCFace[f].VNum) //충돌이 된다.
			{
				Vector3f vec_dist;
				Vector3fSub(cross,src,vec_dist);
				float temp_d = Vector3fDot(vec_dist,vec_dist);
				if(dist > temp_d)
				{
					is_col = TRUE;
					dist = temp_d;
					p_index = f;
					get[0][0]=cross[0];
					get[0][1]=cross[1];
					get[0][2]=cross[2];
				}
			}
		}
	}
	if( is_col )
	{
		Vector3f l_normal;
		Vector3fSub(src,tar,l_normal);
		Normalize(l_normal);
		float su=Vector3fDot(l_normal,mCFace[p_index].Normal);
		su=plane_dist/su;
		get[0][0] = l_normal[0]*su+get[0][0];
		get[0][1] = l_normal[1]*su+get[0][1];
		get[0][2] = l_normal[2]*su+get[0][2];
		return TRUE;
	}

	return FALSE;
}

float CBsp::GetFirstYpos(Vector3f pos,Vector3f min,Vector3f max)
{
	Vector3s s_min,s_max;
	s_min[0] = (short)min[0];
	s_min[1] = (short)min[1];
	s_min[2] = (short)min[2];
	s_max[0] = (short)max[0];
	s_max[1] = (short)max[1];
	s_max[2] = (short)max[2];

	return GetFirstYpos(pos,s_min,s_max);
}
//저쪽에 갈수있니? 못가면 대신 갈수있는 위치까지만...
BOOL CBsp::CanYouGoThere(Vector3f now,Vector3f target,Vector3f *get_point)
{
	if( !mIsLoaded )
		return TRUE;

	if( GetPathCrossPoint(now,target,get_point,0,0) )
	{
		return FALSE;
	}
	else
		return TRUE;
}


//--충돌되는것이 없으면 FALSE를 리턴...
BOOL CBsp::GetPointFromScreenRay(float x,float y,Vector3f tar_point,Vector3f src_point)
{
	_R3ENGINE_STATE *state = GetR3State();
	BOOL is_col=FALSE;
	Vector3f from,at,cross;
	float v[3],n[3];
	int i,j,k,l;

//	x-=.5f;
//	y-=.5f;
	float half_x=state->mViewXL/2.0f-.5f;
	float half_y=state->mViewYL/2.0f-.5f;

    n[0] =  ( ( ( 2.0f * x ) / (state->mViewXL) ) - 1.0f ) / state->mMatProj._11;
    n[1] = -( ( ( 2.0f * y ) / (state->mViewYL) ) - 1.0f) / state->mMatProj._22;
    n[2] =  1.0f;	//현재 왼손 좌표계라...

//	v[0] = n[0]*3660000;		//숫자가 커야 오차가 없어진다????
//	v[1] = n[1]*3660000;
//	v[2] = n[2]*3660000;
	v[0] = n[0]*13660;		//숫자가 커야 오차가 없어진다????
	v[1] = n[1]*13660;
	v[2] = n[2]*13660;

    // Get the inverse view matrix
    D3DXMATRIX m;
    D3DXMatrixInverse( &m, NULL, &state->mMatView );

    // Transform the screen space pick ray into 3D space
    at[0]  = v[0]*m._11 + v[1]*m._21 + v[2]*m._31 + m._41;
    at[1]  = v[0]*m._12 + v[1]*m._22 + v[2]*m._32 + m._42;
    at[2]  = v[0]*m._13 + v[1]*m._23 + v[2]*m._33 + m._43;
    from[0] = m._41;
    from[1] = m._42;
    from[2] = m._43;

	
	src_point[0] = m._41;
	src_point[1] = m._42;
	src_point[2] = m._43;

	double dist = 1000000;
	//지금은 전 폴리곤을 검사....나중에 트리에다 넣자....
	SHORT leaf_id[32000];
	LONG leaf_num;
	GetLeafList(from,at,&leaf_num,leaf_id,32000);

//	for(i=0; i<(int)mCFaceNum; i++)
	for(l=0; l<leaf_num; l++)
	for(k=0; k<mLeaf[leaf_id[l]].face_num; k++)
	{

		i = mCFaceId[mLeaf[leaf_id[l]].face_start_id+k];
		
		if( mCFace[i].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
			continue;
//		LONG start_v = mCFace[i].VNum;
		LONG start_v = mCFace[i].VStartId;
		if( GetPlaneCrossPoint(from, at, cross, mCFace[i].Normal,mCFace[i].Normal[3],0) == FALSE ) 
			continue;

		int cnt=0;
		for(j=0; j<mCFace[i].VNum; j++)
		{
			LONG v0 = mCVertexId[start_v+j];
			LONG v1 = mCVertexId[start_v+(j+1)%mCFace[i].VNum];

			if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross , mCFace[i].Normal,mCFace[i].Normal[3]) == 0)	
				continue;
			cnt++;
		}
		if(cnt == mCFace[i].VNum) //충돌이 된다.
		{
			Vector3f vec_dist;
			Vector3fSub(cross,from,vec_dist);
			float temp_d = sqrtf(Vector3fDot(vec_dist,vec_dist));
			if(dist > temp_d)
			{
				is_col = TRUE;
				dist = temp_d;
				mPickPoly = i;
				tar_point[0]=cross[0];
				tar_point[1]=cross[1];
				tar_point[2]=cross[2];
			}
		}
	}

	if( !is_col )
		mPickPoly=-1;
//	return is_col;

	if( is_col == FALSE )
	{
/*		v[0] = n[0]*3660000;		//숫자가 커야 오차가 없어진다????
		v[1] = n[1]*3660000;
		v[2] = n[2]*3660000;
		tar_point[0]  = v[0]*m._11 + v[1]*m._21 + v[2]*m._31;
		tar_point[1]  = v[0]*m._12 + v[1]*m._22 + v[2]*m._32;
		tar_point[2]  = v[0]*m._13 + v[1]*m._23 + v[2]*m._33;
		tar_point[1] = src_point[1];
		Vector3fMul(tar_point,1/36600.0f,tar_point);
*/	
		Vector3fCopy(at,tar_point);
		tar_point[1] = src_point[1];
//		tar_point
	}
/*
	Vector3f temp;
	Vector3fSub(tar_point,from,temp);
	temp[1]=0;
	Normalize(temp);
	Vector3fMul(temp,200,temp);
	Vector3fAdd(tar_point,temp,tar_point);
*/
	return TRUE;
}

BOOL CBsp::GetPointFromScreenRayFar(float x,float y,Vector3f tar_point,Vector3f src_point)
{
	_R3ENGINE_STATE *state = GetR3State();
	BOOL is_col=FALSE;
	Vector3f from,at,cross;
	float v[3],n[3];
	int i,j,k,l;

//	x-=.5f;
//	y-=.5f;
	float half_x=state->mViewXL/2.0f-.5f;
	float half_y=state->mViewYL/2.0f-.5f;

    n[0] =  ( ( ( 2.0f * x ) / (state->mViewXL) ) - 1.0f ) / state->mMatProj._11;
    n[1] = -( ( ( 2.0f * y ) / (state->mViewYL) ) - 1.0f) / state->mMatProj._22;
    n[2] =  1.0f;	//현재 왼손 좌표계라...

	v[0] = n[0]*3660000;		//숫자가 커야 오차가 없어진다????
	v[1] = n[1]*3660000;
	v[2] = n[2]*3660000;

    // Get the inverse view matrix
//    D3DXMATRIX m;
  //  D3DXMatrixInverse( &m, NULL, &state->mInvMatView );
    D3DXMATRIX m=state->mInvMatView;

    // Transform the screen space pick ray into 3D space
    at[0]  = v[0]*m._11 + v[1]*m._21 + v[2]*m._31;
    at[1]  = v[0]*m._12 + v[1]*m._22 + v[2]*m._32;
    at[2]  = v[0]*m._13 + v[1]*m._23 + v[2]*m._33;
    from[0] = m._41;
    from[1] = m._42;
    from[2] = m._43;
	
	src_point[0] = m._41;
	src_point[1] = m._42;
	src_point[2] = m._43;

	double dist = -1000;
	//지금은 전 폴리곤을 검사....나중에 트리에다 넣자....
	SHORT leaf_id[32000];
	LONG leaf_num;
	GetLeafList(from,at,&leaf_num,leaf_id,32000);

//	for(i=0; i<(int)mCFaceNum; i++)
	for(l=0; l<leaf_num; l++)
	for(k=0; k<mLeaf[leaf_id[l]].face_num; k++)
	{
		i = mCFaceId[mLeaf[leaf_id[l]].face_start_id+k];

		if( mCFace[i].Attr & _CFACE_NO_COLLISION )	//충돌 무시 면이다.
			continue;
//		LONG start_v = mCFace[i].VNum;
		LONG start_v = mCFace[i].VStartId;
		if( GetPlaneCrossPoint(from, at, cross, mCFace[i].Normal,mCFace[i].Normal[3],0) == FALSE ) 
			continue;

		int cnt=0;
		for(j=0; j<mCFace[i].VNum; j++)
		{
			LONG v0 = mCVertexId[start_v+j];
			LONG v1 = mCVertexId[start_v+(j+1)%mCFace[i].VNum];

			if( CheckEdge( mCVertex[v0] , mCVertex[v1] , cross , mCFace[i].Normal,mCFace[i].Normal[3]) == 0)	
				continue;
			cnt++;
		}
		if(cnt == mCFace[i].VNum) //충돌이 된다.
		{
			Vector3f vec_dist;
			Vector3fSub(cross,from,vec_dist);
			float temp_d = sqrtf(Vector3fDot(vec_dist,vec_dist));
			if(dist < temp_d)
			{
				is_col = TRUE;
				dist = temp_d;
				mPickPoly = i;
				tar_point[0]=cross[0];
				tar_point[1]=cross[1];
				tar_point[2]=cross[2];
			}
		}
	}

	if( !is_col )
		mPickPoly=-1;
//	return is_col;

	if( is_col == FALSE )
	{

		v[0] = n[0]*2000;		//숫자가 커야 오차가 없어진다????
		v[1] = n[1]*2000;
		v[2] = n[2]*2000;
		tar_point[0]  = v[0]*m._11 + v[1]*m._21 + v[2]*m._31;
		tar_point[1]  = v[0]*m._12 + v[1]*m._22 + v[2]*m._32;
		tar_point[2]  = v[0]*m._13 + v[1]*m._23 + v[2]*m._33;
		tar_point[1] = src_point[1];
	}

	return TRUE;
}


//---------------------------바운딩 박스 관련 API
#define IR(x)	((DWORD&)x)
#define RAYAABB_EPSILON 0.00001f

BOOL IsRayAABB(Vector3f BBmin,Vector3f BBmax,Vector3f source, Vector3f target, Vector3f *get)
{

#define NUMDIM	3
#define RIGHT	0
#define LEFT	1
#define MIDDLE	2

	BOOL inside1 = TRUE;
	char quadrant1[NUMDIM];
	register int i;
	int whichPlane1;
	float maxT1[NUMDIM];
	float candidatePlane1[NUMDIM];

	// Find candidate planes; this loop can be avoided if
   	//rays cast all from the eye(assume perpsective view) 
	for (i=0; i<NUMDIM; i++)
		if(source[i] < BBmin[i]) {
			quadrant1[i] = LEFT;
			candidatePlane1[i] = BBmin[i];
			inside1 = FALSE;
		}else if (source[i] > BBmax[i]) {
			quadrant1[i] = RIGHT;
			candidatePlane1[i] = BBmax[i];
			inside1 = FALSE;
		}else	{
			quadrant1[i] = MIDDLE;
		}

	// Ray source inside1 bounding box 
	if(inside1)	{
		get[0][0] = source[0];
		get[0][1] = source[1];
		get[0][2] = source[2];
		return (TRUE);
	}


	// Calculate T distances to candidate planes 
	for (i = 0; i < NUMDIM; i++)
	{
		if (quadrant1[i] != MIDDLE && (target[i]-source[i]) !=0.)
			maxT1[i] = (candidatePlane1[i]-source[i]) / (target[i]-source[i]);
		else
			maxT1[i] = -1.;
	}

	// Get largest of the maxT's for final choice of intersection 
	whichPlane1 = 0;
	for (i = 1; i < NUMDIM; i++)
		if (maxT1[whichPlane1] < maxT1[i])
			whichPlane1 = i;

	// Check final candidate actually inside1 box 
	if (maxT1[whichPlane1] < 0.) 
		return (FALSE);
	for (i = 0; i < NUMDIM; i++)
	{
		if (whichPlane1 != i) 
		{
			get[0][i] = source[i] + maxT1[whichPlane1] *(target[i]-source[i]);
			if (get[0][i] < BBmin[i] || get[0][i] > BBmax[i])
				return (FALSE);
		} 
		else 
		{
			get[0][i] = candidatePlane1[i];
		}
	}
	return (TRUE);				// ray hits box 
	
}

BOOL IsRayAABB(Vector3s BBmin,Vector3s BBmax,Vector3f source, Vector3f target, Vector3f *get)
{
	Vector3f bb_min,bb_max;
	bb_min[0] = (float)BBmin[0];	
	bb_min[1] = (float)BBmin[1];	
	bb_min[2] = (float)BBmin[2];	

	bb_max[0] = (float)BBmax[0];	
	bb_max[1] = (float)BBmax[1];	
	bb_max[2] = (float)BBmax[2];	

	return IsRayAABB(bb_min,bb_max,source,target,get);
}


BOOL IsBothRayAABB(Vector3f BBmin,Vector3f BBmax,Vector3f source, Vector3f target)
{
	Vector3f get;
	BOOL temp1= IsRayAABB(BBmin,BBmax,source,target,&get);
	BOOL temp2= IsRayAABB(BBmin,BBmax,target,source,&get);

	if( temp1 && temp2 )
		return TRUE;
	else
		return FALSE;
}

BOOL IsBothRayAABB(Vector3s BBmin,Vector3s BBmax,Vector3f source, Vector3f target)
{
	Vector3f bb_min,bb_max,get;
	bb_min[0] = (float)BBmin[0];	
	bb_min[1] = (float)BBmin[1];	
	bb_min[2] = (float)BBmin[2];	

	bb_max[0] = (float)BBmax[0];	
	bb_max[1] = (float)BBmax[1];	
	bb_max[2] = (float)BBmax[2];	

	BOOL temp1= IsRayAABB(bb_min,bb_max,source,target,&get);
	BOOL temp2= IsRayAABB(bb_min,bb_max,target,source,&get);

	if( temp1 && temp2 )
		return TRUE;
	else
		return FALSE;
}






//----------------------------바운딩 박스와 폴리곤과의 충돌 검사.--------------------------
//----------- 박스 영역의 클리핑....
typedef struct{
	Vector3f v;
	int ccodes;
}_TEMP_VERETX;

#define CC_OFF_LEFT		0x00008000
#define CC_OFF_RIGHT	0x00004000
#define CC_OFF_TOP		0x00002000
#define CC_OFF_BOT		0x00001000
#define CC_OFF_FRONT	0x00000800
#define CC_OFF_BACK		0x00000400

#define _TOP_NORMAL		0
#define _BOT_NORMAL		1
#define _LEFT_NORMAL	2
#define _RIGHT_NORMAL	3
#define _FRONT_NORMAL	4
#define _BACK_NORMAL	5

static int GetCodeVertex(Vector3f v,Vector4f f_normal[6])
{
	int ret_code=0;
	if( f_normal[_TOP_NORMAL][0]*v[0]+f_normal[_TOP_NORMAL][1]*v[1]
		+f_normal[_TOP_NORMAL][2]*v[2]- f_normal[_TOP_NORMAL][3] <0 )
		ret_code|=CC_OFF_TOP;
	if( f_normal[_BOT_NORMAL][0]*v[0]+f_normal[_BOT_NORMAL][1]*v[1]
		+f_normal[_BOT_NORMAL][2]*v[2]- f_normal[_BOT_NORMAL][3] <0 )
		ret_code|=CC_OFF_BOT;
	if( f_normal[_LEFT_NORMAL][0]*v[0]+f_normal[_LEFT_NORMAL][1]*v[1]
		+f_normal[_LEFT_NORMAL][2]*v[2]- f_normal[_LEFT_NORMAL][3] <0 )
		ret_code|=CC_OFF_LEFT;
	if( f_normal[_RIGHT_NORMAL][0]*v[0]+f_normal[_RIGHT_NORMAL][1]*v[1]
		+f_normal[_RIGHT_NORMAL][2]*v[2]- f_normal[_RIGHT_NORMAL][3] <0 )
		ret_code|=CC_OFF_RIGHT;
	if( f_normal[_FRONT_NORMAL][0]*v[0]+f_normal[_FRONT_NORMAL][1]*v[1]
		+f_normal[_FRONT_NORMAL][2]*v[2]- f_normal[_FRONT_NORMAL][3] <0 )
		ret_code|=CC_OFF_FRONT;
	if( f_normal[_BACK_NORMAL][0]*v[0]+f_normal[_BACK_NORMAL][1]*v[1]
		+f_normal[_BACK_NORMAL][2]*v[2]- f_normal[_BACK_NORMAL][3] <0 )
		ret_code|=CC_OFF_BACK;
	return ret_code;
}

static int GetPlaneCrossPointClipCode(_TEMP_VERETX *cross,_TEMP_VERETX *a,_TEMP_VERETX *b,Vector4f f_normal,Vector4f p_normal[6])
{
    float front = f_normal[0]*a->v[0] + f_normal[1]*a->v[1] + f_normal[2]*a->v[2] -f_normal[3];
    float back  = f_normal[0]*b->v[0] + f_normal[1]*b->v[1] + f_normal[2]*b->v[2] -f_normal[3];

    if ( (front > 0 && back > 0) || (front < 0 && back < 0)) 
		return FALSE;

    front = front / (front-back);

    cross->v[0] = a->v[0] + front*(b->v[0]-a->v[0]);
    cross->v[1] = a->v[1] + front*(b->v[1]-a->v[1]);
    cross->v[2] = a->v[2] + front*(b->v[2]-a->v[2]);

	return GetCodeVertex(cross->v,p_normal);
}



_TEMP_VERETX stVpts[64], *stDefaultVlist[64],pts[64],*clip_list1[64],*clip_list2[64];
_TEMP_VERETX vpts[64];
_TEMP_VERETX *default_vlist[64]={
	&vpts[0],&vpts[1],&vpts[2],&vpts[3],&vpts[4],&vpts[5],&vpts[6],&vpts[7],&vpts[8],&vpts[9],
	&vpts[10],&vpts[11],&vpts[12],&vpts[13],&vpts[14],&vpts[15],&vpts[16],&vpts[17],&vpts[18],&vpts[19],
	&vpts[20],&vpts[21],&vpts[22],&vpts[23],&vpts[24],&vpts[25],&vpts[26],&vpts[27],&vpts[28],&vpts[29],
	&vpts[30],&vpts[31],&vpts[32],&vpts[33],&vpts[34],&vpts[35],&vpts[36],&vpts[37],&vpts[38],&vpts[39],
	&vpts[40],&vpts[41],&vpts[42],&vpts[43],&vpts[44],&vpts[45],&vpts[46],&vpts[47],&vpts[48],&vpts[49],
	&vpts[50],&vpts[51],&vpts[52],&vpts[53],&vpts[54],&vpts[55],&vpts[56],&vpts[57],&vpts[58],&vpts[59],
};

static void InitDefaultPointList(void)
{
   int i;
   for (i=0; i < 64; ++i)
      default_vlist[i] = &vpts[i];
}

static int ClipVertexListInBox(int n,_TEMP_VERETX **vl,Vector4f p_normal[6], int codes_or, _TEMP_VERETX ***out_vl)
{
	int i,j,k,p=0; // p = index into temporary point pool
	_TEMP_VERETX **cur;

	if (codes_or & CC_OFF_LEFT) 
	{
		cur = clip_list1;
		k = 0;
		j = n-1;
		for (i=0; i < n; ++i) 
		{
	        if (!(vl[j]->ccodes & CC_OFF_LEFT))
				cur[k++] = vl[j];

			 // if it crosses, add the intersection point
			if ((vl[j]->ccodes ^ vl[i]->ccodes) & CC_OFF_LEFT) 
			{
				pts[p].ccodes = GetPlaneCrossPointClipCode(&pts[p],vl[i], vl[j],p_normal[_LEFT_NORMAL],p_normal);
				//외부 폴리곤을 위해서 
				cur[k++] = &pts[p++];
			}
			j = i;
		}
	    // move output list to be input
		n = k;
		vl = cur;
	}
	if (codes_or & CC_OFF_RIGHT) 
	{
		cur = (vl == clip_list1) ? clip_list2 : clip_list1;
		k = 0;
		j = n-1;
		for (i=0; i < n; ++i) 
		{
			if (!(vl[j]->ccodes & CC_OFF_RIGHT))
				cur[k++] = vl[j];
			if ((vl[j]->ccodes ^ vl[i]->ccodes) & CC_OFF_RIGHT) 
			{
				pts[p].ccodes = GetPlaneCrossPointClipCode(&pts[p],vl[i], vl[j],p_normal[_RIGHT_NORMAL],p_normal);
				//외부 폴리곤을 위해서 
				cur[k++] = &pts[p++];
			}
			j = i;
		}
		n = k;
		vl = cur;
	}
	if (codes_or & CC_OFF_TOP) 
	{
		cur = (vl == clip_list1) ? clip_list2 : clip_list1;
		k = 0;
		j = n-1;
		for (i=0; i < n; ++i) 
		{
			if (!(vl[j]->ccodes & CC_OFF_TOP))
				cur[k++] = vl[j];
			if ((vl[j]->ccodes ^ vl[i]->ccodes) & CC_OFF_TOP) 
			{
				pts[p].ccodes = GetPlaneCrossPointClipCode(&pts[p],vl[i], vl[j],p_normal[_TOP_NORMAL],p_normal);
				//외부 폴리곤을 위해서 
				cur[k++] = &pts[p++];
			}
			j = i;
		}
		n = k;
		vl = cur;
	}
	if (codes_or & CC_OFF_BOT) 
	{	
		cur = (vl == clip_list1) ? clip_list2 : clip_list1;
		k = 0;
		j = n-1;
		for (i=0; i < n; ++i) 
		{
			if (!(vl[j]->ccodes & CC_OFF_BOT))
				cur[k++] = vl[j];
			if ((vl[j]->ccodes ^ vl[i]->ccodes) & CC_OFF_BOT) 
			{
				pts[p].ccodes = GetPlaneCrossPointClipCode(&pts[p],vl[i], vl[j],p_normal[_BOT_NORMAL],p_normal);
				cur[k++] = &pts[p++];
			}
			j = i;
		}
		n = k;
		vl = cur;
	}
	if (codes_or & CC_OFF_FRONT) 
	{
		cur = (vl == clip_list1) ? clip_list2 : clip_list1;
		k = 0;
		j = n-1;
		for (i=0; i < n; ++i) 
		{
			if (!(vl[j]->ccodes & CC_OFF_FRONT))
				cur[k++] = vl[j];
			if ((vl[j]->ccodes ^ vl[i]->ccodes) & CC_OFF_FRONT) 
			{
				pts[p].ccodes = GetPlaneCrossPointClipCode(&pts[p],vl[i], vl[j],p_normal[_FRONT_NORMAL],p_normal);
				cur[k++] = &pts[p++];
			}
			j = i;
		}
		n = k;
		vl = cur;
	}
	if (codes_or & CC_OFF_BACK) 
	{	
		cur = (vl == clip_list1) ? clip_list2 : clip_list1;
		k = 0;
		j = n-1;
		for (i=0; i < n; ++i) 
		{
			if (!(vl[j]->ccodes & CC_OFF_BACK))
				cur[k++] = vl[j];
			if ((vl[j]->ccodes ^ vl[i]->ccodes) & CC_OFF_BACK) 
			{
				pts[p].ccodes = GetPlaneCrossPointClipCode(&pts[p],vl[i], vl[j],p_normal[_BACK_NORMAL],p_normal);
				cur[k++] = &pts[p++];
			}
			j = i;
		}
		n = k;
		vl = cur;
	}

	*out_vl = vl;
	return n;
}

BOOL IsCollisionBBoxToFace(Vector4f p_normal[6],Vector3f a,Vector3f b,Vector3f c)
{
	DWORD codes_or=0;
    _TEMP_VERETX **vlist;

	Vector3fCopy(a,vpts[0].v);
	Vector3fCopy(b,vpts[1].v);
	Vector3fCopy(c,vpts[2].v);
	vpts[0].ccodes =GetCodeVertex(vpts[0].v,p_normal);
	vpts[1].ccodes =GetCodeVertex(vpts[1].v,p_normal);
	vpts[2].ccodes =GetCodeVertex(vpts[2].v,p_normal);

	codes_or|=vpts[0].ccodes;
	codes_or|=vpts[1].ccodes;
	codes_or|=vpts[2].ccodes;
	int clip_v_cnt= ClipVertexListInBox(3,default_vlist,p_normal,codes_or, &vlist);
	if( clip_v_cnt >= 3 )
		return TRUE;
	return FALSE;
}
BOOL IsCollisionBBoxPoint(Vector3f a_min,Vector3f a_max,Vector3f pos)
{
	if( a_min[0] <= pos[0] && pos[0] <= a_max[0] 
	&& a_min[1] <= pos[1] && pos[1] <= a_max[1] 
	&& a_min[2] <= pos[2] && pos[2] <= a_max[2] )
		return TRUE;
	return FALSE;
}

//바운딩 박스끼리의 충돌 여부.....
BOOL IsCollisionBBox(Vector3s a_min,Vector3s a_max,Vector3f b_min,Vector3f b_max)
{
	if( a_min[0] <= b_max[0] && b_min[0] <= a_max[0] 
	&& a_min[1] <= b_max[1] && b_min[1] <= a_max[1] 
	&& a_min[2] <= b_max[2] && b_min[2] <= a_max[2] )
		return TRUE;
	return FALSE;
}
BOOL IsCollisionBBox(Vector3f a_min,Vector3f a_max,Vector3f b_min,Vector3f b_max)
{
	if( a_min[0] <= b_max[0] && b_min[0] <= a_max[0] 
	&& a_min[1] <= b_max[1] && b_min[1] <= a_max[1] 
	&& a_min[2] <= b_max[2] && b_min[2] <= a_max[2] )
		return TRUE;
	return FALSE;
}
BOOL IsCollisionBBox(Vector3s a_min,Vector3s a_max,Vector3s b_min,Vector3s b_max)
{
	if( a_min[0] <= b_max[0] && b_min[0] <= a_max[0] 
	&& a_min[1] <= b_max[1] && b_min[1] <= a_max[1] 
	&& a_min[2] <= b_max[2] && b_min[2] <= a_max[2] )
		return TRUE;
	return FALSE;
}
