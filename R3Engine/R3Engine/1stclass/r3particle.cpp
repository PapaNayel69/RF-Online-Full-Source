#include "R3Bsp.h"
#include "R3math.h"
#include "R3Particle.h"
#include "JError.h"
#include "Jmalloc.h"
#include "Procedural.h"
#include "CommonUtil.h"
#include "Core.h"
#include <math.h>
#include <stdio.h>



#define _TIME_EPSILON 0.3

static _PRE_PARTICLE_LIST stPreParticleList[]={
	"사용자 셋팅",0,0,0,0		,1 ,2,2, 0,0,0, 1

	,255,203,152,101,50,0	,255,255,255,255,255,255	,255,255,255,255,255,255	,255,255,255,255,255,255	,1,1,1,1,1,1

	,"단순 추락"	 ,0,0,0,0		,4 ,2,2, 0,0,0, 1
	,255,203,152,101,50,0	,255,255,255,255,255,255	,255,255,255,255,255,255	,255,255,255,255,255,255	,1,1,1,1,1,1
 
	,"연기"		 ,0,0,0,0		,4 ,-2,-2, 0,0,0,1		
	,255,203,152,101,50,0	,255,255,255,255,255,255	,255,255,255,255,255,255	,255,255,255,255,255,255	,1,1,1,1,1,1

	,"폭발"		 ,0,0,0,0		,1 ,2,2, 0,0,0,	1
	,255,203,152,101,50,0	,255,255,255,255,255,255	,255,255,255,255,255,255	,255,255,255,255,255,255	,1,1,1,1,1,1

	,"용의 불"	 ,0,0,0,0		,4 ,2,-0.7f, 0,-5,-10, 2
	,255,203,152,101,50,0	,255,255,255,255,255,255	,255,255,255,255,255,255	,255,255,255,255,255,255	,0.5f,1,1,1,1,2

	,"용암지대 연기"	,0,100,0,100		,4 ,-2,-2, 0,0,0,1		
	,255,203,152,101,50,0	,255,255,255,255,255,255	,255,255,255,255,255,255	,255,255,255,255,255,255	,0.2f,1,1,1,1,4
};

int GetPreParticleListNum()//미리 수치가 들어있는 파티클 리스트 갯수 
{
	return sizeof(stPreParticleList)/sizeof(_PRE_PARTICLE_LIST);
}

_PRE_PARTICLE_LIST *GetPreParticleList(int id)//해당 아이디의 포인터 리턴
{
	return &stPreParticleList[id];
}


#define _AFC			0.0f // 공기저항계수

#define _GTIME ((80*250.0f)/4900.0)		// 시간*시간 = 즉 1/70 초*1/70

float GetGravity( float num )     // 0은 최 정점	-는 올라간다 +는 내려간다.
{
	if(num<0)
	    return((float)(_GTIME)*num*num);    // 여기서 250 은 떨어지는 속도
	else
		return((float)-(_GTIME)*num*num);    // 여기서 250 은 떨어지는 속도
}

float PowInterpolate(float a,float b, float alpha)
{
	float start = 15;
	float max_i=(float)pow(start,2);

	float p_pow= (float)pow(start, 2-alpha);
	alpha = (((p_pow-start)/(max_i-start)));

	return  a*(1-alpha) + b*alpha;
}


void InitR3Particle(void)
{
	float start = 15;
	float max_i=(float)pow(start,2);
	float div_f = (1)/256.0f;

	for(int i=0; i<256; i++)
	{
		float p_pow= (float)pow(start, 2-(float)i*div_f);
		BYTE gop=255-(BYTE)(((p_pow-start)/(max_i-start))*255);  
	}
}
void ReleaseR3Particle(void)
{
}
//움직일때 2가지 타잎이 있다. 
//자기가 움직여서 변하는 스타일
//실제는 움직이지만 파티클은 제자리에 남아서 처리되는 스타일.	<- 실은 메인파티클pos가 이동되는경우다.

CParticle::CParticle()
{
	memset(this,0,sizeof(CParticle));
	mFlag=0;
	mEntity=NULL;
/*
	memset(mATrack,0xff,_PATICLE_MAX_TRACK);
	memset(mRTrack,0xff,_PATICLE_MAX_TRACK*2);
	memset(mGTrack,0xff,_PATICLE_MAX_TRACK*2);
	memset(mBTrack,0xff,_PATICLE_MAX_TRACK*2);
*/
/*
	mATrack[0]=255;
	mATrack[5]=0;
	mRTrack[0]=255;
	mRTrack[5]=255;
	mGTrack[0]=255;
	mGTrack[5]=255;
	mBTrack[0]=255;
	mBTrack[5]=255;
	mScaleTrack[0][0]=1.0f;
	mScaleTrack[0][1]=1.0f;
	mScaleTrack[5][0]=1.0f;
	mScaleTrack[5][1]=1.0f;
*/
	mGravity[0]=0;
	mGravity[1]=-15;
	mGravity[2]=0;
	mStartPower[0][0]=0;
	mStartPower[0][1]=0;
	mStartPower[0][2]=0;
	mStartPower[1][0]=0;
	mStartPower[1][1]=0;
	mStartPower[1][2]=0;
	mCreatePos[0]=0;
	mCreatePos[1]=0;
	mCreatePos[2]=0;
}

CParticle::~CParticle()
{
	if( mElement )
		Dfree(mElement);
}

void CParticle::ReleaseEntity()	//특별 주의 : 클론 들은  릴리즈하지 않는다.
{
	if( !mEntity )
		return;
	mEntity->ReleaseEntity();
	delete mEntity;
}

static float GetRandNum(float a,float b)
{
	LONG d_a= (LONG)(a*32767);
	LONG d_b= (LONG)(b*32767);
	if( d_b-d_a == 0 )
		return a;
	DWORD count=GetTickCount();
	LONG gop = (rand()*count)%(d_b-d_a)+d_a;
	return gop/32767.0f;
}

//save파티클로의 이동.
void CParticle::CopyParticleToSaveParticle(_SAVE_PARTICLE *s_particle)
{
	memset(s_particle,0,sizeof(_SAVE_PARTICLE));
	strcpy(s_particle->mEntityName,mEntityName);
	s_particle->mNum=mNum;
	s_particle->mAlphaType=mAlphaType;
	s_particle->mFlag = mFlag;	
	s_particle->mLiveTime = mLiveTime;	
	Vector3fCopy(mGravity,s_particle->mGravity);	
	s_particle->mTimeSpeed = mTimeSpeed;	
//	Vector3fCopy(mPowerNormal,s_particle->mPowerNormal);	
	//구찮다 트랙전부를 카피.
	memcpy(s_particle->mATrack,mATrack,_PATICLE_MAX_TRACK*4*sizeof(BYTE)+sizeof(float)*_PATICLE_MAX_TRACK);
}

void CParticle::CopySaveParticleToParticle(_SAVE_PARTICLE *s_particle)
{
	memset(this,0,sizeof(CParticle));
	strcpy(mEntityName,s_particle->mEntityName);
//	Vector3lCopy(s_particle->mCreateArea,mCreateArea);
	mFlag = s_particle->mFlag;
	mNum = s_particle->mNum;
	mAlphaType=s_particle->mAlphaType;
	mLiveTime = s_particle->mLiveTime;	
	mTimeSpeed = s_particle->mTimeSpeed;	
//	Vector3fCopy(s_particle->mPowerNormal,mPowerNormal);
	//구찮다 트랙전부를 카피.
	memcpy(mATrack,s_particle->mATrack,_PATICLE_MAX_TRACK*4*sizeof(BYTE)+sizeof(float)*_PATICLE_MAX_TRACK);
}

void CParticle::SetPreCalcParticle(DWORD type)	//파티클 갯수 초기화 사전에 반드시 상태변수들을 셋팅해야된다.
{
	mLiveTime=stPreParticleList[type].mLiveTime;
//	Vector3fCopy(stPreParticleList[type].mPowerNormal,mPowerNormal);
	mTimeSpeed=stPreParticleList[type].mTimeSpeed;
	mFlag=stPreParticleList[type].mFlag;
//	Vector3fCopy(stPreParticleList[type].mCreateArea,mCreateArea);	//	구 영역의 경우..[0]번 배열에 반지름이...있다. 박스는 x,y,z 
	//구찮다 트랙전부를 카피.
	memcpy(mATrack,stPreParticleList[type].mATrack,_PATICLE_MAX_TRACK*4*sizeof(BYTE)+sizeof(float)*_PATICLE_MAX_TRACK);
//	mAlphaGradient=stPreParticleList[type].mAlphaGradient;
//	mOnePerTimeEpsilon=stPreParticleList[type].mOnePerTimeEpsilon;
//	mStartPos[0][0] = 0;	mStartPos[0][1] = 0;	mStartPos[0][2] = 0;
}

void CParticle::GetBBox(Vector3f get_min,Vector3f get_max)			//해당파티클 그룹의 바운딩 박스를 계산한다.
{//끌날때까지 돌려봐서 아에 구한다.

	get_min[0] = 65000;
	get_min[1] = 65000;
	get_min[2] = 65000;
	get_max[0] = -65000;
	get_max[1] = -65000;
	get_max[2] = -65000;

	float i;

	_PARTICLE_ELEMENT temp;
	memcpy(&temp,mElement,sizeof(_PARTICLE_ELEMENT));	//일단 백업.

	_ENTITY_M_GROUP *mat_group=&mEntity->mMatGroup[0];
	float bb_max;

	Vector3f temp_v[3];
	Vector3fCopy(mStartPos[0],temp_v[0]);
	Vector3fCopy(mStartPos[1],temp_v[1]);
	Vector3fCopy(mCreatePos,temp_v[2]);
	mCreatePos[0]=0;
	mCreatePos[1]=0;
	mCreatePos[2]=0;
	mStartPos[0][0]=0;
	mStartPos[0][1]=0;
	mStartPos[0][2]=0;
	mStartPos[1][0]=0;
	mStartPos[1][1]=0;
	mStartPos[1][2]=0;
	bb_max = -65000;
	for(int k=0; k<(int)mat_group->VCnt ;k++)
	{
		_D3DR3VERTEX_TEX1 *tex1=&mEntity->mStaticVertexBuffer.m_VertexBufferTex1[k+mat_group->VBMinIndex];
		if( fabs(tex1->x) > bb_max )
			bb_max=(float)fabs(tex1->x);
		if( fabs(tex1->y) > bb_max )
			bb_max=(float)fabs(tex1->y);
		if( fabs(tex1->z) > bb_max )
			bb_max=(float)fabs(tex1->z);
	}

	InitElement(0,0); //엘리먼트 초기화
	for(i=0; i<mLiveTime/mTimeSpeed; i+=0.1f)	//0.1초정도 만 계산하자.
	{
		mElement[0].mTime = i;
		GetPartcleStep(0,0.1f);//좌표를 구하고.
		if( mElement->mPos[0] < get_min[0] )
			get_min[0]=mElement->mPos[0];
		if( mElement->mPos[1] < get_min[1] )
			get_min[1]=mElement->mPos[1];
		if( mElement->mPos[2] < get_min[2] )
			get_min[2]=mElement->mPos[2];

		if( mElement->mPos[0] > get_max[0] )
			get_max[0]=mElement->mPos[0];
		if( mElement->mPos[1] > get_max[1] )
			get_max[1]=mElement->mPos[1];
		if( mElement->mPos[2] > get_max[2] )
			get_max[2]=mElement->mPos[2];
	}
	get_min[0]-=bb_max;
	get_min[1]-=bb_max;
	get_min[2]-=bb_max;
	get_max[0]+=bb_max;
	get_max[1]+=bb_max;
	get_max[2]+=bb_max;

	Vector3fCopy(temp_v[0],mStartPos[0]);
	Vector3fCopy(temp_v[1],mStartPos[1]);
	Vector3fCopy(temp_v[2],mCreatePos);

	get_min[0]+=mStartPos[0][0];
	get_min[1]+=mStartPos[0][1];
	get_min[2]+=mStartPos[0][2];
	get_max[0]+=mStartPos[1][0];
	get_max[1]+=mStartPos[1][1];
	get_max[2]+=mStartPos[1][2];

	memcpy(mElement,&temp,sizeof(_PARTICLE_ELEMENT));	//복구.
}

//파티클 초기화 함수들을 만들어라....
void CParticle::InitParticle()	//파티클 갯수 초기화 사전에 반드시 상태변수들을 셋팅해야된다.
{
	if(  mNum < 1 )
	{
		Warning("파티클 엘레먼트가 1개는 되야...","");
		return;
	}
	if(  mLiveTime == 0 )
	{
		Warning("파티클 상태변수들을 미리 셋팅하거나 이함수 전에 SetParticleAttr()을 먼저 호출하세요","");
		return;
	}
	mElement = (_PARTICLE_ELEMENT *)Dmalloc(sizeof(_PARTICLE_ELEMENT)*mNum );
	for(int i=0; i<mNum; i++)
		InitElement(i,0);
	mOnePerTime = (mLiveTime/mTimeSpeed)/mNum;
//	mNextCreatorTime=0;
}
void CParticle::ResetOnePerTime()
{
	mOnePerTime = (mLiveTime/mTimeSpeed)/mNum;
}

void CParticle::ReInitParticle(int num)	//파티클 개수를 재할당 시킨다.  매루프마다는 금지...(malloc이 있다.)
{
	int i;

	if( mNum == num )
		return;		//같으면 할필요 없이 리턴.
	mElement = (_PARTICLE_ELEMENT *)ReAlloc(mElement,sizeof(_PARTICLE_ELEMENT)*mNum
		,sizeof(_PARTICLE_ELEMENT)*num );
	if( num > mNum )
	{
		for(i=mNum; i<num; i++)
			InitElement(i,0);
	}
	mNum = num;
	mOnePerTime = (mLiveTime/mTimeSpeed)/mNum;
}
void CParticle::ReleaseParticle()	//파티클 릴리즈 
{
	if( mElement )
		Dfree(mElement);
	mElement=NULL;
}

/*
void CParticle::GetPartcleAbsoluteTime(int id,float time)	//파티클의 절대타임에 대한좌표 구한다.
{
	
	float AirFriction=0.1f;
	float fr[3]; //공기 저항및 중력 저항값에 따른 계산값. 

	time*=mTimeSpeed;
	time=min(mLiveTime,time);
	//입자의 중력 트위닝 값.	이값을 잘 조절해야 튀길경우 탄성체를 조절한다.
	float g_tweening=(mEndGravity-mStartGravity)*(time/mLiveTime)+mStartGravity;

	mElement[id].mGravityCnt=time*g_tweening;

	float attn=(1-time/mLiveTime-.0f);	//중력 감쇠량.
	attn = max(attn,0);

//	AirFriction=(1-mElement[id].mTime/mLiveTime);	//공기저항값.
	AirFriction=(time);	//공기저항값.

	fr[0] = mPowerNormal[0]*AirFriction;	//과연 단지 힘의 방향?
	fr[1] = mPowerNormal[1]*attn;			//y좌표는 중력 감쇠.
	fr[2] = mPowerNormal[2]*AirFriction;
	
	float length=GetGravity(mElement[id].mGravityCnt);//g_tweening);	//가속도를 구한다. 중력상황에 따라 1.0f을 바꾸고.
	//파티클의 힘의 방향에 가산한다.
	mElement[id].mPos[0]=fr[0]+mStartPos[0][0];
	mElement[id].mPos[1]=length+fr[1]+mStartPos[0][1]-mPowerNormal[1];	
	mElement[id].mPos[2]=fr[2]+mStartPos[0][2];

}
*/


//힘의 방향과 중력을 따로 계산하자...
//운동량 = - 운동량 * 충돌계수 (운동량 = 질량 * 속도)

void _PARTICLE_ELEMENT::UpDate(float time)//,float mTimeSpeed)
{
	mPos[0]+=mDir[0]*time;
	mPos[1]+=mDir[1]*time;
	mPos[2]+=mDir[2]*time;
	mDir[0]+=mDirStep[0]*time;		//현 파티클 방향.
	mDir[1]+=mDirStep[1]*time;		//현 파티클 방향.
	mDir[2]+=mDirStep[2]*time;		//현 파티클 방향.
	mARGB[0]+=mARGBStep[0]*time;
	mARGB[1]+=mARGBStep[1]*time;
	mARGB[2]+=mARGBStep[2]*time;
	mARGB[3]+=mARGBStep[3]*time;
	mScale+=mScaleStep*time;
	mNowFrame+=time;
	mZRot += mZRotStep*time;
	mYRot += mYRotStep*time;
}

void CParticle::CheckCollision(int id,float time)	//파티클의 다음 좌표에 해당하는 스텝..
{
	if( (mFlag & _PARTICLE_ATTR_COLLISION ) && mBsp )	//개개별 파티클 충돌첵크...
	{
		Vector3f next_pos,c_pos,now_pos;
		Vector4f c_normal;

		CBsp *bsp = (CBsp *)mBsp;
		//mElement[id].UpDate(time);
		//다음 좌표를 미리 구해본다.
		next_pos[0] = mElement[id].mPos[0]+mElement[id].mDir[0]*time;
		next_pos[1] = mElement[id].mPos[1]+mElement[id].mDir[1]*time;
		next_pos[2] = mElement[id].mPos[2]+mElement[id].mDir[2]*time;

		if( mFlag & _PARTICLE_ATTR_FREE )
		{
			Vector3fCopy(mElement[id].mPos,now_pos);
		}
		else
		{
			Vector3fAdd(mCreatePos,next_pos,next_pos);
			Vector3fAdd(mCreatePos,mElement[id].mPos,now_pos);
		}
		
		if( bsp->IsCollisionFace(now_pos,next_pos,&c_pos,&c_normal) )
		{
			//충돌이 생겼다...
			Vector3f r,t;
			CrossVector(c_normal,mElement[id].mDir,r);
			if( r[0] == 0 || r[1] == 0 || r[2] == 0 )
			{
				t[0] = 1;
				t[1] = 0;
				t[2] = 0;
			}
			else
			{
				CrossVector(c_normal,r,t);
				Normalize(r);
				Normalize(t);
			}
			D3DXMATRIX matMirror;
			D3DXMatrixIdentity( &matMirror );
			matMirror._11 -= 2*t[0]*t[0]; matMirror._12 -= 2*t[0]*t[1]; matMirror._13 -= 2*t[0]*t[2];
			matMirror._21 -= 2*t[1]*t[0]; matMirror._22 -= 2*t[1]*t[1]; matMirror._23 -= 2*t[1]*t[2];
			matMirror._31 -= 2*t[2]*t[0]; matMirror._32 -= 2*t[2]*t[1]; matMirror._33 -= 2*t[2]*t[2];
			matMirror._41 -= 2*t[3]*t[0];   matMirror._42 -= 2*t[3]*t[1];   matMirror._43 -= 2*t[3]*t[2];
//			Vector3fSub(mElement[id].mDir,c_pos,r);
			Vector3fCopy(mElement[id].mDir,r);
			Vector3fTransform(t,r,matMirror.m);

			mElement[id].mDir[0] = -t[0]*mElasticity;
			mElement[id].mDir[1] = -t[1]*mElasticity;
			mElement[id].mDir[2] = -t[2]*mElasticity;
			mElement[id].mDirStep[0] = 0;
			mElement[id].mDirStep[1] = 0;
			mElement[id].mDirStep[2] = 0;//15;

			if( !(mFlag & _PARTICLE_ATTR_FREE) )
			{
				Vector3fSub(c_pos,mCreatePos,c_pos);
//				Vector3fAdd(c_pos,mElement[id].mPos,mElement[id].mPos);
				Vector3fCopy(c_pos,mElement[id].mPos);
			}
			else
			{
				Vector3fCopy(c_pos,mElement[id].mPos);
			}
		}
/*
		//----평면에 반사각 구하기...  a는 부딪히는 법석벡터 
		Vector3f a,b,r,t;
		a[0]=0;		a[1]=1;		a[2]=0;
		b[0]=1;		b[1]=1;		b[2]=0;
		CrossVector(a,b,r);
		CrossVector(b,r,t);
		*/
	}

	mElement[id].UpDate(time);

	mElement[id].mDir[0]+=mGravity[0]*time;		// 파티클 중력 적용.
	mElement[id].mDir[1]+=mGravity[1]*time;	
	mElement[id].mDir[2]+=mGravity[2]*time;	
/*
	Vector3f temp;	//중력은 바뀌면 안될듯.
	if( mFlag & _PARTICLE_ATTR_FREE )
		Vector3fTransform( temp,mGravity,mRotMat);
	else
		Vector3fCopy( mGravity,temp);
	mElement[id].mDir[0]+=temp[0]*time;		// 파티클 중력 적용.
	mElement[id].mDir[1]+=temp[1]*time;	
	mElement[id].mDir[2]+=temp[2]*time;	
*/
}
void CParticle::GetPartcleStep(int id,float time)	//파티클의 다음 좌표에 해당하는 스텝..
{
	time*=mTimeSpeed;
	time=min(mLiveTime,time);

	CheckCollision(id,time);
//	if( mElement[id].mScale > 0.2)
//		int dfdf=0;

	if( mTrackCnt > mElement[id].mNowTrack )
	{
		BOOL is_calc=FALSE;
/*
		if( mElement[id].mTime > mLiveTime/mTimeSpeed )
		{
			mElement[id].mTime=0;
			mElement[id].mIsLive=FALSE;
			return;
		}
*/
		//현재 파티클 시간이 트랙시간보다 크면 
		while( mTimeTrack[mElement[id].mNowTrack] <=  mElement[id].mTime*mTimeSpeed )
		{
			int next_track = mElement[id].mNowTrack+1;
			if( mTrackCnt-1 < next_track )
			{
				next_track=mTrackCnt-1;
				if( is_calc==FALSE )
				{
					mElement[id].mDirStep[0]=0;
					mElement[id].mDirStep[1]=0;
					mElement[id].mDirStep[2]=0;
					mElement[id].mARGBStep[0] = 0;
					mElement[id].mARGBStep[1] = 0;
					mElement[id].mARGBStep[2] = 0;
					mElement[id].mARGBStep[3] = 0;
					mElement[id].mScaleStep=0;
				}
				break;
			}
			float div=mTimeTrack[next_track]-mElement[id].mTime*mTimeSpeed;
			if( div < 0 ) //_TIME_EPSILON )
				goto _NEXT_TRACK;
			is_calc=TRUE;

			if(fabs(div) < _TIME_EPSILON)
				div=1;
			if( mTrackFlag[mElement[id].mNowTrack] & _TRACK_FLAG_POWER )
			{/*
				mElement[id].mDirStep[0] = GetRandNum(mPowerTrack[mElement[id].mNowTrack][0][0]
					,mPowerTrack[mElement[id].mNowTrack][1][0]);
				mElement[id].mDirStep[1] = GetRandNum(mPowerTrack[mElement[id].mNowTrack][0][1]
					,mPowerTrack[mElement[id].mNowTrack][1][1]);
				mElement[id].mDirStep[2] = GetRandNum(mPowerTrack[mElement[id].mNowTrack][0][2]
					,mPowerTrack[mElement[id].mNowTrack][1][2]);*/
				Vector3f temp;
				temp[0] = GetRandNum(mPowerTrack[mElement[id].mNowTrack][0][0]
					,mPowerTrack[mElement[id].mNowTrack][1][0]);
				temp[1] = GetRandNum(mPowerTrack[mElement[id].mNowTrack][0][1]
					,mPowerTrack[mElement[id].mNowTrack][1][1]);
				temp[2] = GetRandNum(mPowerTrack[mElement[id].mNowTrack][0][2]
					,mPowerTrack[mElement[id].mNowTrack][1][2]);
				if( mFlag & _PARTICLE_ATTR_FREE )
					Vector3fTransform( mElement[id].mDirStep,temp,mRotMat);
				else
					Vector3fCopy( temp,mElement[id].mDirStep);

				mElement[id].mDirStep[0]= mElement[id].mDirStep[0] - mElement[id].mDir[0]/div;
				mElement[id].mDirStep[1]= mElement[id].mDirStep[1] - mElement[id].mDir[1]/div;
				mElement[id].mDirStep[2]= mElement[id].mDirStep[2] - mElement[id].mDir[2]/div;
//				mElement[id].mDir[0]=0;
//				mElement[id].mDir[1]=0;
//				mElement[id].mDir[2]=0;
			}
			else
			{//그렇지 않을경우 이전 파워를 써야한다.
//				mElement[id].mDirStep[0]=0;	
//				mElement[id].mDirStep[1]=0;
//				mElement[id].mDirStep[2]=0;
			}

			if( mTrackFlag[mElement[id].mNowTrack] & _TRACK_FLAG_FLICKER )
			{				
				if( mElement[id].mFlag & _ELEMENT_ATTR_FLICKER )
					mElement[id].mFlag = ~_ELEMENT_ATTR_FLICKER & mElement[id].mFlag;
				else
					mElement[id].mFlag |= _ELEMENT_ATTR_FLICKER;
			}

			if( mTrackFlag[next_track] & _TRACK_FLAG_ALPHA )
				mElement[id].mARGBStep[0] = ((float)mATrack[next_track]-mElement[id].mARGB[0])/div;
			else
				mElement[id].mARGBStep[0] = 0;
			if( mTrackFlag[next_track] & _TRACK_FLAG_COLOR )
			{
				mElement[id].mARGBStep[1] = ((float)GetRandNum(mRTrack[next_track][0],mRTrack[next_track][1])-mElement[id].mARGB[1])/div;
				mElement[id].mARGBStep[2] = ((float)GetRandNum(mGTrack[next_track][0],mGTrack[next_track][1])-mElement[id].mARGB[2])/div;
				mElement[id].mARGBStep[3] = ((float)GetRandNum(mBTrack[next_track][0],mBTrack[next_track][1])-mElement[id].mARGB[3])/div;
			}
			else
			{
				mElement[id].mARGBStep[1] = 0;
				mElement[id].mARGBStep[2] = 0;
				mElement[id].mARGBStep[3] = 0;
			}
			if( mTrackFlag[next_track] & _TRACK_FLAG_SCALE )
			{
				float temp = GetRandNum(mScaleTrack[next_track][0],mScaleTrack[next_track][1]);
				mElement[id].mScaleStep = (temp-mElement[id].mScale)/div;
			}
			else
				mElement[id].mScaleStep = 0;
			if( mTrackFlag[next_track] & _TRACK_FLAG_ZROT )
			{
				float temp = GetRandNum(mZRotTrack[next_track][0],mZRotTrack[next_track][1]);
				mElement[id].mZRotStep = (temp)/div;
			}
			else
				mElement[id].mZRotStep = 0;
			if( mTrackFlag[next_track] & _TRACK_FLAG_YROT )
			{
				float temp = GetRandNum(mYRotTrack[next_track][0],mYRotTrack[next_track][1]);
				mElement[id].mYRotStep = (temp)/div;
			}
			else
				mElement[id].mYRotStep = 0;

//			CheckCollision(id,time);
//			mElement[id].UpDate(time);
_NEXT_TRACK:
			mElement[id].mNowTrack++;
			if( mTrackCnt <= mElement[id].mNowTrack )
				break;
		}
		if( is_calc )
			return;
	}
}

DWORD CParticle::GetParticleState()			// 시작,끝,강제 상태를 알아봄
{
	return mState;
}
void CParticle::SetParticleState(DWORD state)	//파티클 시작끝을 지정.
{
	int i,temp_i;
	float oldest_element;

	mState = state;
	switch(state)
	{
	case _PARTICLE_STATE_START:	//강제로 한개 출력하는 루틴
		for( i=0; i<mNum; i++)
		{
			InitElement(i,0);
			mElement[i].mIsLive =FALSE;
		}
		break;
	case _PARTICLE_STATE_START_ONE_ELEMENT:	//강제로 한개 출력하는 루틴
		oldest_element = -1;
		temp_i=-1;	//혹시 몰라서..
		for(i=0 ;i<mNum; i++)
		{
			if( !mElement[i].mIsLive )	//죽어 있는 넘들중 하나를 키운다.
			{
				//mElement[i].mTime=0;
				//mElement[i].mIsLive=TRUE;
				InitElement(i,0); //엘리먼트 초기화
				break;
			}
			else	//만약의 경우 살아 있는 애들중의 가장오래된 거를 잡는다.
			{
				if( oldest_element < mElement[i].mTime )
				{
					oldest_element = mElement[i].mTime;
					temp_i=i;
				}
			}
		}
		if( temp_i >= 0 )
		{
			InitElement(temp_i,0); //엘리먼트 초기화
		}
		break;
	case _PARTICLE_STATE_ALWAYS_LIVE:
		for( i=0; i<mNum; i++)
		{			
			mOnePerTimeEpsilonTemp = (Noise1(GetTickCount())-0.5f)*mOnePerTimeEpsilon;	//노이즈 함수.
			InitElement(i,i*(mOnePerTime+mOnePerTimeEpsilonTemp));
		}
		break;
	default:
		break;
	}
}

void CParticle::InitElement(int id,float time) //엘리먼트 초기화
{
	Vector3f temp;
	//한번 계산을 해줘야 파워량에 따라 초기위치를 설정할수가 있다.
	mElement[id].mTime = 0;
	mElement[id].mFlag = 0;

	if( mFlag & _PARTICLE_ATTR_FLICKER )	//시작부터 플리커면.
		mElement[id].mFlag |= _ELEMENT_ATTR_FLICKER;
//	mElement[id].mDir[0] = GetRandNum(mStartPower[0][0],mStartPower[1][0]);
//	mElement[id].mDir[1] = GetRandNum(mStartPower[0][1],mStartPower[1][1]);
//	mElement[id].mDir[2] = GetRandNum(mStartPower[0][2],mStartPower[1][2]);

	temp[0] = GetRandNum(mStartPower[0][0],mStartPower[1][0]);
	temp[1] = GetRandNum(mStartPower[0][1],mStartPower[1][1]);
	temp[2] = GetRandNum(mStartPower[0][2],mStartPower[1][2]);

	if( mFlag & _PARTICLE_ATTR_FREE )
		Vector3fTransform( mElement[id].mDir,temp,mRotMat);
	else
		Vector3fCopy( temp,mElement[id].mDir);


	
	mElement[id].mDirStep[0] = 0;
	mElement[id].mDirStep[1] = 0;
	mElement[id].mDirStep[2] = 0;//15;

	Vector3f start_pos;

	start_pos[0] = GetRandNum(mStartPos[0][0],mStartPos[1][0]);
	start_pos[1] = GetRandNum(mStartPos[0][1],mStartPos[1][1]);
	start_pos[2] = GetRandNum(mStartPos[0][2],mStartPos[1][2]);

	mElement[id].mZRot=GetRandNum(mStartZRot[0],mStartZRot[1]);
	mElement[id].mYRot=GetRandNum(mStartYRot[0],mStartYRot[1]);

	if( _PARTICLE_ATTR_FREE & mFlag )	//프리파티클.
	{
		mElement[id].mPos[0]=start_pos[0] + mCreatePos[0];	//첫포지션 결정.
		mElement[id].mPos[1]=start_pos[1] + mCreatePos[1];
		mElement[id].mPos[2]=start_pos[2] + mCreatePos[2];
	}
	else
	{
		mElement[id].mPos[0]=start_pos[0];// + mCreatePos[0];	//첫포지션 결정.
		mElement[id].mPos[1]=start_pos[1];// + mCreatePos[1];
		mElement[id].mPos[2]=start_pos[2];// + mCreatePos[2];
	}

	mElement[id].mNowFrame=0;

	mElement[id].mNowTrack = 0;
	mElement[id].mIsLive = FALSE;	//살리는거는 따로 해야..
//	mElement[id].mIsLive = TRUE;	//살리고..
	mElement[id].mTime = time;

	if( mElement[id].mTime > mLiveTime/mTimeSpeed )
		mElement[id].mTime=0;

	GetPartcleStep(id,mElement[id].mTime);	//짜투리 시간 계산이쥐...

	if( mTrackCnt )
	{
		DWORD next_track = 0;
		float div=mTimeTrack[next_track]-mElement[id].mTime*mTimeSpeed;
		if(fabs(div) < _TIME_EPSILON)
			div=1;

		if( mTrackFlag[next_track] & _TRACK_FLAG_ALPHA )
			mElement[id].mARGBStep[0] = ((float)mATrack[next_track]-GetRandNum(mStartARGB[0][0],mStartARGB[0][1]))/div;
		else
			mElement[id].mARGBStep[0] = 0;

		if( mTrackFlag[next_track] & _TRACK_FLAG_COLOR )
		{
			mElement[id].mARGBStep[1] = ((float)GetRandNum(mRTrack[next_track][0],mRTrack[next_track][1])
				-GetRandNum(mStartARGB[1][0],mStartARGB[1][1]))/div;
			mElement[id].mARGBStep[2] = ((float)GetRandNum(mGTrack[next_track][0],mGTrack[next_track][1])
				-GetRandNum(mStartARGB[2][0],mStartARGB[2][1]))/div;
			mElement[id].mARGBStep[3] = ((float)GetRandNum(mBTrack[next_track][0],mBTrack[next_track][1])
				-GetRandNum(mStartARGB[3][0],mStartARGB[3][1]))/div;
		}
		else
		{
			mElement[id].mARGBStep[1] = 0;
			mElement[id].mARGBStep[2] = 0;
			mElement[id].mARGBStep[3] = 0;
		}
		if( mTrackFlag[next_track] & _TRACK_FLAG_SCALE )
		{
			float temp = GetRandNum(mScaleTrack[next_track][0],mScaleTrack[next_track][1]);
			float temp_s = GetRandNum(mStartScale[0],mStartScale[1]);
			mElement[id].mScaleStep = (temp-temp_s)/div;
		}
		else
			mElement[id].mScaleStep = 0;

		if( mTrackFlag[next_track] & _TRACK_FLAG_ZROT )
		{
			float temp = GetRandNum(mZRotTrack[next_track][0],mZRotTrack[next_track][1]);
			mElement[id].mZRotStep = (temp)/div;
		}
		else
			mElement[id].mZRotStep = 0;

		if( mTrackFlag[next_track] & _TRACK_FLAG_YROT )
		{
			float temp = GetRandNum(mYRotTrack[next_track][0],mYRotTrack[next_track][1]);
			mElement[id].mYRotStep = (temp)/div;
		}
		else
			mElement[id].mYRotStep = 0;
	}
	else
	{
		mElement[id].mARGBStep[0] = 0;
		mElement[id].mARGBStep[1] = 0;
		mElement[id].mARGBStep[2] = 0;
		mElement[id].mARGBStep[3] = 0;
		mElement[id].mScaleStep = 0;
	}
	mElement[id].mARGB[0]=GetRandNum(mStartARGB[0][0],mStartARGB[0][1]);
	mElement[id].mARGB[1]=GetRandNum(mStartARGB[1][0],mStartARGB[1][1]);
	mElement[id].mARGB[2]=GetRandNum(mStartARGB[2][0],mStartARGB[2][1]);
	mElement[id].mARGB[3]=GetRandNum(mStartARGB[3][0],mStartARGB[3][1]);
	mElement[id].mScale = GetRandNum(mStartScale[0],mStartScale[1]);
	mElement[id].mZRot = GetRandNum(mStartZRot[0],mStartZRot[1]);
	mElement[id].mYRot = GetRandNum(mStartYRot[0],mStartYRot[1]);

//	GetPartcleStep(id,0);//

}

#include "r3input.h"

/*
	if( gScan[VK_1] )
	{
		SetParticleState(_PARTICLE_STATE_START);
	}
	if( gScan[VK_2] )
	{
//		SetParticleState(_PARTICLE_STATE_RESET);
		SetParticleState(_PARTICLE_STATE_STOP);
	}
	if( gAsci[VK_3] )
	{
		gAsci[VK_3]=0;
		SetParticleState(_PARTICLE_STATE_START_ONE_ELEMENT);
	}
*/
LONG CParticle::RealLoop()	//파티클의 해당 루프...
{
	int i;
	int live_num=0;
	float loop_time = R3GetLoopTime();

	if( loop_time*mTimeSpeed > 1)		//파티클 튀는거 조정.
		loop_time=loop_time/mTimeSpeed;
		
	mParticleTimer += loop_time;
	if( mState != _PARTICLE_STATE_STOP )
		mNextCreatorTime -= loop_time;
	{
		for(i=0 ;i<mNum; i++)
		{
			if( mElement[i].mIsLive )	//살아 있는 넘들은 계산해준다.
			{			
				/*	//float를 모듈러스 연산을 하기위해서...
				if( mElement[i].mTime > mLiveTime*2000 )	//너무 커지면 문제가 생길수있다.
				{
					mElement[i].mTime -=mLiveTime*2000;
				}
				DWORD dwTime = (DWORD)(mElement[i].mTime*512);
				DWORD dwLiveTime = (DWORD)(mLiveTime*512);
				float f_rtime = (dwTime%dwLiveTime)/512.0f;
				GetPartcleStep(i,f_rtime);//좌표를 구하고.
				*/
				live_num++;
				mElement[i].mTime += loop_time;
				if( mElement[i].mTime > mLiveTime/mTimeSpeed )
				{
					live_num--;
					mElement[i].mIsLive =FALSE;
					if( mState == _PARTICLE_STATE_STOP )
					{
					//	mElement[i].mTime=0;
					//	memset(&mElement[i],0,sizeof(_PARTICLE_ELEMENT));
					}
					while( mElement[i].mTime > mLiveTime/mTimeSpeed )
					{
						mElement[i].mTime -= mLiveTime/mTimeSpeed;
					}
				}
				else
				{
					GetPartcleStep(i,loop_time);//좌표를 구하고.
				}
	//			GetPartcleStep(i,mElement[i].mTime);//좌표를 구하고.
			}
			if( mFlag & _PARTICLE_ATTR_END )
			{
				continue;
			}
			if( mFlag & _PARTICLE_ATTR_EMIT_TIME )
			{
				if( mParticleTimer > mEmitTime )
				{
					if( i==mNum-1 )
						 mFlag |= _PARTICLE_ATTR_END;
					continue;
				}
			}
//			if( mFlag & _PARTICLE_ATTR_NO_LOOP )
			if( mNextCreatorTime + mOnePerTimeEpsilonTemp < 0 )	//element들을 크리에이트 하는 부분..
			{
				if( mElement[i].mIsLive == FALSE )	//새로 생성되야 된다.
				{
					live_num++;
					if( mNextCreatorTime + loop_time < 0 ) //강제로 한개 출력하는 루틴으로 인해 보정해야된다.
					{
						int temp_i = (int )(mNextCreatorTime*65536.0f);
						int temp_mod = (int)(loop_time*65536.0f);
						temp_mod = temp_i%temp_mod;
						mNextCreatorTime=(temp_mod/65536.0f);
					}
					InitElement(i,-(mNextCreatorTime+mOnePerTimeEpsilonTemp)); //엘리먼트 초기화
					mElement[i].mIsLive =TRUE;	//새로 생성되야 된다.
					mNextCreatorTime += mOnePerTime*(1-mStartTimeRange);
					mOnePerTimeEpsilonTemp = (Noise1(GetTickCount())-0.5f)*mOnePerTimeEpsilon*(1-mStartTimeRange);	//노이즈 함수.
					if( mFlag & _PARTICLE_ATTR_NO_LOOP )
					{
						if( i==mNum-1 )
							 mFlag |= _PARTICLE_ATTR_END;
					}
				}
			}
		}
	}
	return live_num;
}

LONG CParticle::Loop()	//파티클의 해당 루프...
{
	int live_num;
	if( mFlag & _PARTICLE_ATTR_SPECIAL_ID )
	{
		live_num = SpecialLoop();		//특수한것들 루핑 시킨다.
		if( live_num != -1 )			//특수 루프를 쓰는것만 리턴시키고 안쓰는것은 그냥 loop함수를 사용한다.
			return live_num;
	}
	return RealLoop();	//파티클의 해당 루프...
}

DWORD GetTokenFloat(char *ch,float *get_f)	//
{
	int i;
	int cnt = strlen(ch);
	char buf[256];

	int start_cnt=0;
	for(i=0; i<cnt; i++)
	{
		if( ch[i] == '(' )
		{
			start_cnt=0;
			continue;
		}
		if( ch[i] == ',' ||  ch[i] == ')')
		{
			buf[start_cnt] = NULL;
			*get_f=(float)atof(buf);
			return i+1;
		}
		buf[start_cnt] = ch[i];
		start_cnt++;
	}
	*get_f=0;
	return cnt;
}

BOOL GetRandOrNum( FILE *fp,float *from,float *at )	//TRUE는 렌덤.
{
	char hole[256];
	char temp[256];

	fscanf(fp,"%s",hole);
	if( hole[0] == 'r' && hole[1] == 'a' && hole[2] == 'n'&& hole[3] == 'd' ) //렌덤지정일경우.
	{
		int i,j;
		j=strlen(hole);
		BOOL is_end=FALSE;
		while(1)	//앞부분 스킵.
		{
			for(i=0; i<j ;i++)
			{
				if( hole[i]== ')' )
				{
					is_end=TRUE;
				}
			}
			if( is_end )
				break;
			if( fscanf(fp,"%s",temp)==EOF)	
				Error("스크립트 read 에러.",")가없음.");
			strcat(hole,temp);
			j=strlen(hole);
		}
		DWORD cnt = GetTokenFloat(hole,from);	// 렌덤시작.
		cnt = GetTokenFloat(&hole[cnt],at);	//렌덤 끝.
		return TRUE;
	}

	*from = (float)atof(hole);
	*at = *from;
	return FALSE;
}

BOOL CParticle::LoadParticleSPT(char *name,DWORD option)
{
	FILE *fp;
	float temp_f;
//	int i;

	if(( fp =fopen(name,"rt"))==NULL) 
	{
		Warning(name,"<-이 파일이없습니다.");
		return FALSE;
	}

	char hole[256];

	BOOL is_par=FALSE;
	mFlag=0;

    while(1)	//앞부분 스킵.
    {
		if( fscanf(fp,"%s",hole)==EOF)	
			break;
        if (!strcmp(hole,"[Particle]"))
		{
			is_par=TRUE;
			break;
		}
	}
	if( !is_par )
	{
		Warning(name,"<-이파일은 파티클스크립트가 아닙니다.");
		fclose(fp);
		return FALSE;
	}
	int is_track_ok=-1,track_id;
	memset(this,0,sizeof(CParticle));
	mStartScale[0]=1;	//디폴트 값 셋팅.
	mStartScale[1]=1;	//디폴트 값 셋팅.
	mStartARGB[0][0]=255;
	mStartARGB[1][0]=255;
	mStartARGB[2][0]=255;
	mStartARGB[3][0]=255;
	mStartARGB[0][1]=255;
	mStartARGB[1][1]=255;
	mStartARGB[2][1]=255;
	mStartARGB[3][1]=255;
	mTimeSpeed=1;
	mAlphaType=3;
	float old_time=0;
	track_id=0;
    while(1)
    {
		if( fscanf(fp,"%s",hole)==EOF)	
			break;
		strlwr(hole);
		if( !strcmp(hole,"entity_file"))
		{
			fscanf(fp,"%s",hole);
			strcpy(mEntityName,hole);
			//여기에서 엔티티를 로딩 
			mEntity = new CEntity;
			if( !mEntity->LoadEntity(mEntityName,option|_LOAD_POLY_IN_MEMORY) )
			{
				fclose(fp);
				delete mEntity;
				mEntity=NULL;
				return FALSE;
			}
			mEntity->RestoreTexMem();
		}
		if( !strcmp(hole,"num"))
		{
			fscanf(fp,"%s",hole);
			mNum=atoi(hole);	//파티클 총 갯수.
		}
		if( !strcmp(hole,"start_time_range"))
		{
			fscanf(fp,"%s",hole);
			mStartTimeRange = (float)atof(hole);
		}
		if( !strcmp(hole,"always_live"))
		{
//			SetParticleState(_PARTICLE_STATE_ALWAYS_LIVE);
//			mNum=atoi(hole);	//파티클 총 갯수.
		}
		if( !strcmp(hole,"pos"))
		{
			fscanf(fp,"%s",hole);
			if( !strcmp(hole,"box"))
			{
			}
			else
			if( !strcmp(hole,"sphere"))
			{
				mFlag |= _PARTICLE_ATTR_SPHERE;
			}
			else
			if( !strcmp(hole,"sphere_edge"))
			{
				mFlag |= _PARTICLE_ATTR_SPHERE_EDGE;
			}
			else
			{
				Warning(hole,"시작위치 지정 명령어가 아닙니다.");
				continue;
			}
			if( !GetRandOrNum(fp,&mStartPos[0][0],&mStartPos[1][0]) )	//렌덤이 아니면.
				mStartPos[1][0] = mStartPos[0][0];
			if( !GetRandOrNum(fp,&mStartPos[0][1],&mStartPos[1][1]) )	//렌덤이 아니면.
				mStartPos[1][1] = mStartPos[0][1];
			if( !GetRandOrNum(fp,&mStartPos[0][2],&mStartPos[1][2]) )	//렌덤이 아니면.
				mStartPos[1][2] = mStartPos[0][2];
		}
		if( !strcmp(hole,"no_billboard"))
			mFlag |= _PARTICLE_ATTR_NO_BILLBOARD;
		if( !strcmp(hole,"z_disable"))
			mFlag |= _PARTICLE_ATTR_ZDISABLE;
		if( !strcmp(hole,"live_time"))
		{
			GetRandOrNum(fp,&mLiveTime,&temp_f);	// 만약 렌덤으로 읽혀도..
		}
		if( !strcmp(hole,"alpha_type"))
		{
			fscanf(fp,"%s",hole);
			mAlphaType = atoi(hole);
		}
		if( !strcmp(hole,"always_live"))
			mFlag |= _PARTICLE_ATTR_ALWAYS_LIVE;
		if( !strcmp(hole,"time_speed"))
		{
			GetRandOrNum(fp,&mTimeSpeed,&temp_f);	// 만약 렌덤으로 읽혀도..
		}
		if( !strcmp(hole,"gravity"))
		{
			GetRandOrNum(fp,&mGravity[0],&temp_f);	// 만약 렌덤으로 읽혀도..
			GetRandOrNum(fp,&mGravity[1],&temp_f);	// 만약 렌덤으로 읽혀도..
			GetRandOrNum(fp,&mGravity[2],&temp_f);	// 만약 렌덤으로 읽혀도..
		}
		if( !strcmp(hole,"start_power"))
		{
			if( !GetRandOrNum(fp,&mStartPower[0][0],&mStartPower[1][0]) )	//렌덤이 아니면.
				mStartPower[1][0] = mStartPower[0][0];
			if( !GetRandOrNum(fp,&mStartPower[0][1],&mStartPower[1][1]) )	//렌덤이 아니면.
				mStartPower[1][1] = mStartPower[0][1];
			if( !GetRandOrNum(fp,&mStartPower[0][2],&mStartPower[1][2]) )	//렌덤이 아니면.
				mStartPower[1][2] = mStartPower[0][2];
		}
		if( !strcmp(hole,"start_scale"))
		{
			if( !GetRandOrNum(fp,&mStartScale[0],&mStartScale[1]) )	//렌덤이 아니면.
				mStartScale[1] = mStartScale[0];
		}
		if( !strcmp(hole,"start_zrot"))
		{
			mFlag |= _PARTICLE_ATTR_Z_ROT;
			if( !GetRandOrNum(fp,&mStartZRot[0],&mStartZRot[1]) )	//렌덤이 아니면.
				mStartZRot[1] = mStartZRot[0];
		}
		if( !strcmp(hole,"start_yrot"))
		{
			mFlag |= _PARTICLE_ATTR_Y_ROT;
			if( !GetRandOrNum(fp,&mStartYRot[0],&mStartYRot[1]) )	//렌덤이 아니면.
				mStartYRot[1] = mStartYRot[0];
		}

		if( !strcmp(hole,"y_billboard"))
			mFlag |= _PARTICLE_ATTR_Y_BILLBOARD;
		if( !strcmp(hole,"z_billboard"))
			mFlag |= _PARTICLE_ATTR_Z_BILLBOARD;
		if( !strcmp(hole,"free"))
			mFlag |= _PARTICLE_ATTR_FREE;
		if( !strcmp(hole,"check_collision"))
			mFlag |= _PARTICLE_ATTR_COLLISION;

		if( !strcmp(hole,"z_front"))
		{
			mFlag |= _PARTICLE_ATTR_ZFRONT;
			GetRandOrNum(fp,&mZFront,&temp_f);	// 만약 렌덤으로 읽혀도..
		}
		if( !strcmp(hole,"emit_time"))
		{
			mFlag |= _PARTICLE_ATTR_EMIT_TIME;
			GetRandOrNum(fp,&mEmitTime,&temp_f);	// 만약 렌덤으로 읽혀도..
		}

		if( !strcmp(hole,"start_alpha"))
			GetRandOrNum(fp,&mStartARGB[0][0],&mStartARGB[0][1]);
		if( !strcmp(hole,"start_color"))
		{
			GetRandOrNum(fp,&mStartARGB[1][0],&mStartARGB[1][1]);	// 만약 렌덤으로 읽혀도..
			GetRandOrNum(fp,&mStartARGB[2][0],&mStartARGB[2][1]);	// 만약 렌덤으로 읽혀도..
			GetRandOrNum(fp,&mStartARGB[3][0],&mStartARGB[3][1]);	// 만약 렌덤으로 읽혀도..
		}
		if( !strcmp(hole,"flicker_alpha"))
		{		
			fscanf(fp,"%s",hole);
			mFlickerAlpha = (BYTE)atoi(hole);
		}
		if( !strcmp(hole,"flicker_time"))
		{		
			fscanf(fp,"%s",hole);
			mFlickerTime = (float)atof(hole);
			if( mTimeSpeed != 0 )
				mFlickerTime /= mTimeSpeed; 
		}
		if( !strcmp(hole,"flicker"))
		{		
			mFlag |= _PARTICLE_ATTR_FLICKER;
		}
		if( !strcmp(hole,"create_time_epsilon"))
		{
			GetRandOrNum(fp,&mOnePerTimeEpsilon,&temp_f);	// 만약 렌덤으로 읽혀도..
		}
		if( !strcmp(hole,"elasticity"))
		{
			GetRandOrNum(fp,&mElasticity,&temp_f);	// 만약 렌덤으로 읽혀도..
		}

		if( !strcmp(hole,"special_id"))
		{
			fscanf(fp,"%s",hole);
			mSpecialID = atoi(hole);
			mFlag |= _PARTICLE_ATTR_SPECIAL_ID;
		}
		if( !strcmp(hole,"tex_repeat"))
		{
			fscanf(fp,"%s",hole);
			mTexRepeat = (float)atof(hole);
		}
		
		if( !strcmp(hole,"time"))
		{
			is_track_ok=0;
			fscanf(fp,"%s",hole);
			mTimeTrack[mTrackCnt] = (float)atof(hole);
			/*
			if( mTimeTrack[mTrackCnt]-old_time <= _TIME_EPSILON )
			{
				Warning("이전시간보다 0.3초이상 차이가 나야됩니다.","<-스크립트 read 에러");
				//continue;
			}*/
			old_time = mTimeTrack[mTrackCnt];
			track_id = mTrackCnt;
			mTrackCnt++;
			if( mTrackCnt >= _PATICLE_MAX_TRACK )
				Error("최대 트랙개수를 넘었습니다.","");
		}

		if( !strcmp(hole,"power"))
		{
			if( is_track_ok == -1 )
			{
				Warning(hole,"이 명령어는 time을 먼저 지정한뒤 써주세요.");
				continue;
			}
			if( !GetRandOrNum(fp,&mPowerTrack[track_id][0][0],&mPowerTrack[track_id][1][0]) )	//렌덤이 아니면.
				mPowerTrack[track_id][1][0] = mPowerTrack[track_id][0][0];
			if( !GetRandOrNum(fp,&mPowerTrack[track_id][0][1],&mPowerTrack[track_id][1][1]) )	//렌덤이 아니면.
				mPowerTrack[track_id][1][1] = mPowerTrack[track_id][0][1];
			if( !GetRandOrNum(fp,&mPowerTrack[track_id][0][2],&mPowerTrack[track_id][1][2]) )	//렌덤이 아니면.
				mPowerTrack[track_id][1][2] = mPowerTrack[track_id][0][2];
			mTrackFlag[track_id]|=_TRACK_FLAG_POWER;
		}
		if( !strcmp(hole,"alpha"))
		{
			if( is_track_ok == -1 )
			{
				Warning(hole,"이 명령어는 time을 먼저 지정한뒤 써주세요.");
				continue;
			}
			GetRandOrNum(fp,&temp_f,&temp_f);	// 만약 렌덤으로 읽혀도..
			mATrack[track_id]=(BYTE)temp_f;
			mTrackFlag[track_id]|=_TRACK_FLAG_ALPHA;
		}
		if( !strcmp(hole,"zrot"))
		{
			mTrackFlag[track_id]|=_TRACK_FLAG_ZROT;
			mFlag |= _PARTICLE_ATTR_Z_ROT;
			if( is_track_ok == -1 )
			{
				Warning(hole,"이 명령어는 time을 먼저 지정한뒤 써주세요.");
				continue;
			}
			if( !GetRandOrNum(fp,&mZRotTrack[track_id][0],&mZRotTrack[track_id][1]) )	//렌덤이 아니면.
				mZRotTrack[track_id][1] = mZRotTrack[track_id][0];
		}
		if( !strcmp(hole,"yrot"))
		{
			mTrackFlag[track_id]|=_TRACK_FLAG_YROT;
			mFlag |= _PARTICLE_ATTR_Y_ROT;
			if( is_track_ok == -1 )
			{
				Warning(hole,"이 명령어는 time을 먼저 지정한뒤 써주세요.");
				continue;
			}
			if( !GetRandOrNum(fp,&mYRotTrack[track_id][0],&mYRotTrack[track_id][1]) )	//렌덤이 아니면.
				mYRotTrack[track_id][1] = mYRotTrack[track_id][0];
		}
		if( !strcmp(hole,"flicker"))
			mTrackFlag[track_id]|=_TRACK_FLAG_FLICKER;
		
		if( !strcmp(hole,"color"))
		{
			if( is_track_ok == -1 )
			{
				Warning(hole,"이 명령어는 time을 먼저 지정한뒤 써주세요.");
				continue;
			}
			/*
			GetRandOrNum(fp,&temp_f,&temp_f);	// 만약 렌덤으로 읽혀도..
			mRTrack[track_id]=(BYTE)temp_f;
			GetRandOrNum(fp,&temp_f,&temp_f);	// 만약 렌덤으로 읽혀도..
			mGTrack[track_id]=(BYTE)temp_f;
			GetRandOrNum(fp,&temp_f,&temp_f);	// 만약 렌덤으로 읽혀도..
			mBTrack[track_id]=(BYTE)temp_f;
			*/
			float temp_f2;
			GetRandOrNum(fp,&temp_f,&temp_f2);
			mRTrack[track_id][0]=(BYTE)temp_f;
			mRTrack[track_id][1]=(BYTE)temp_f2;
			GetRandOrNum(fp,&temp_f,&temp_f2);
			mGTrack[track_id][0]=(BYTE)temp_f;
			mGTrack[track_id][1]=(BYTE)temp_f2;
			GetRandOrNum(fp,&temp_f,&temp_f2);
			mBTrack[track_id][0]=(BYTE)temp_f;
			mBTrack[track_id][1]=(BYTE)temp_f2;
			mTrackFlag[track_id]|=_TRACK_FLAG_COLOR;
		}
		if( !strcmp(hole,"scale"))
		{
			if( is_track_ok == -1 )
			{
				Warning(hole,"이 명령어는 time을 먼저 지정한뒤 써주세요.");
				continue;
			}
			if( !GetRandOrNum(fp,&mScaleTrack[track_id][0],&mScaleTrack[track_id][1]) )	//렌덤이 아니면.
				mScaleTrack[track_id][1] = mScaleTrack[track_id][0];
			mTrackFlag[track_id]|=_TRACK_FLAG_SCALE;
		}
//		if( !strcmp(hole,"time"))
//			fscanf(fp,"%s",hole);
	}
	fclose(fp);

	return TRUE;
}
void CParticle::GetFlickerARGB(int i,DWORD *dw_argb)	//플리커가 있다면 컬러값을 구한다.
{
	if( ! (mElement[i].mFlag & _ELEMENT_ATTR_FLICKER) )	//플리커가 아닌다.
		return;
	DWORD dw_frame = (DWORD)(mElement[i].mNowFrame*32768);
	DWORD dw_flicker = (DWORD)(mFlickerTime*65536);
	if( dw_flicker )
	{
		if( dw_frame % dw_flicker < dw_flicker/2 )	//플리커 적용해야된다.
		{
			if( (*dw_argb>>24) >  mFlickerAlpha )
				*dw_argb = (*dw_argb&0x00ffffff)|(mFlickerAlpha<<24);
		}
	}
}


void CParticle::SetStartBoxArea()	//시작박스안에서 벗어난 파티클을 안으로 옮긴다. //비나 눈처럼 환경에서 쓰여질때
{
	Vector3f pos;
	int i;


	for(i=0 ;i<mNum; i++)
	{
		if( !mElement[i].mIsLive )	//살아 있는 넘들만 계산.
			continue;
		
		float x_length = mStartPos[1][0]-mStartPos[0][0];
		float z_length = mStartPos[1][2]-mStartPos[0][2];

		pos[0] = mElement[i].mPos[0] - mCreatePos[0];
		if(pos[0] < mStartPos[0][0])
			pos[0]+=x_length;
		if(pos[0] > mStartPos[1][0])
			pos[0]-=x_length;
		mElement[i].mPos[0] = pos[0] + mCreatePos[0];

		pos[2] = mElement[i].mPos[2] - mCreatePos[2];
		if(pos[2] < mStartPos[0][2])
			pos[2]+=z_length;
		if(pos[2] > mStartPos[1][2])
			pos[2]-=z_length;
		mElement[i].mPos[2] = pos[2] + mCreatePos[2];
	}
}

static void CrossVector( Vector3f a, Vector3f b, Vector3f r )
{
    r[0] = (a[1] * b[2]) - (a[2] * b[1]);
    r[1] = (a[2] * b[0]) - (a[0] * b[2]);
    r[2] = (a[0] * b[1]) - (a[1] * b[0]);
}






void GetViewVector(Vector3f v)
{
	Vector3f org = {0,0,1};

	Vector3fTransform(v,org,GetR3State()->mMatView.m);
	v[0] -= stState.mMatView.m[3][0];
	v[1] -= stState.mMatView.m[3][1];
	v[2] -= stState.mMatView.m[3][2];
	Normalize(v);
}


//Get Snake Vertex list	얻어지는 값은 strip 버텍스 리스트 이다.
DWORD CalcSnakeVertexList(_D3DR3VERTEX_TEX1 *get_v,Vector3f *org_v,DWORD cnt,float f_length,DWORD color)
{
	if( cnt < 2 || cnt > 99)	//2개는 있어야지..
		return 0;

	DWORD v_cnt=0,i;
	Vector4f plane;	//기본이 되는 평면의 방정식.	ax+by+cz=d
	Vector3f v_temp,cross_v;
	Vector3f l_snake[100][2];	//왼쪽 점들.
	Vector3f r_snake[100][2];	//오른쪽 점들.
	float f_temp;

	Vector3f org = {0,0,1};
/*
	GetR3State()->mViewVector[0]=-GetR3State()->mViewVector[0];
	GetR3State()->mViewVector[1]=-GetR3State()->mViewVector[1];
	Vector3fCopy( GetR3State()->mViewVector,plane);
*/
//	plane[1]=0;
//	Normalize(plane);

	Vector3fCopy(GetR3State()->mInvMatView.m[3],org);

	Vector3f old_cross_v={0,1,0};
	for(i=0; i<cnt-1; i++)
	{
		org_v[i];
		org_v[i+1];
		Vector3fSub(org_v[i],org_v[i+1],v_temp);
		f_temp = Vector3fDot(v_temp,v_temp);
		if( f_temp < 0.01f )	//거의 같다 위치가.. 그래서 안뿌린다.
			continue;
		f_temp = sqrtf(f_temp);
		v_temp[0] /= f_temp;
		v_temp[1] /= f_temp;
		v_temp[2] /= f_temp;
		Vector3fSub(org_v[i],org_v[i+1],v_temp);
		
		Vector3fSub(org_v[i],org,plane);
		Normalize(plane);


//		Vector3fCross(v_temp,plane,cross_v);  //노멀 벡터 완성.
//		Vector3fCross(plane,v_temp,cross_v);  //노멀 벡터 완성.
		CrossVector(plane,v_temp,cross_v);  //노멀 벡터 완성.

		f_temp = Vector3fDot(cross_v,cross_v);
/*		if( f_temp < 1.1f )	//뷰 벡터와 거의 같다 
		{
			cross_v[0] = 0;
			cross_v[1] = 1;
			cross_v[2] = 0;
//			Vector3fCopy(old_cross_v,cross_v);
		}
		else*/
		{
			f_temp = sqrtf(f_temp);
			cross_v[0] /= f_temp;
			cross_v[1] /= f_temp;
			cross_v[2] /= f_temp;
			Vector3fMul(cross_v,f_length,cross_v);// 스케일추가해서 나왔다. 이걸루..각각의 버텍스를 구하자.
		}
		Vector3fAdd(org_v[i],cross_v,l_snake[v_cnt][0]);
		Vector3fSub(org_v[i],cross_v,r_snake[v_cnt][0]);
		Vector3fAdd(org_v[i+1],cross_v,l_snake[v_cnt][1]);
		Vector3fSub(org_v[i+1],cross_v,r_snake[v_cnt][1]);
		Vector3fCopy(cross_v,old_cross_v);
		v_cnt++;
	}
	if( v_cnt < 1 )
		return 0;

#define SetF(aa,bb,cc)	{	get_v[f_cnt].u=aa;	get_v[f_cnt].v=bb;	get_v[f_cnt].color=cc; };
	//최종 라인들을 구성 해서 넣자 냐하하....
	DWORD f_cnt=0;
	Vector3fCopy( l_snake[0][0],get_v[f_cnt].pos );
	SetF(0,0,color);
	f_cnt++;
	Vector3fCopy( r_snake[0][0],get_v[f_cnt].pos );
	SetF(1,0,color);
	f_cnt++;
	float fv_cnt = (float)v_cnt;
	for(i=0; i<v_cnt-1; i++)
	{
		float v = (i+1)/(fv_cnt);
		Vector3fAdd(l_snake[i][1],l_snake[i+1][0],v_temp);
		Vector3fMul(v_temp,0.5f,get_v[f_cnt].pos);
		SetF(0.0f,v,color);
		f_cnt++;
		Vector3fAdd(r_snake[i][1],r_snake[i+1][0],v_temp);
		Vector3fMul(v_temp,0.5f,get_v[f_cnt].pos);
		SetF(1,v,color);
		f_cnt++;
	}
	Vector3fCopy( l_snake[i][1],get_v[f_cnt].pos );
	SetF(0,1,color);
	f_cnt++;
	Vector3fCopy( r_snake[i][1],get_v[f_cnt].pos );
	SetF(1,1,color);
	f_cnt++;
	return f_cnt;
}
