#include <windows.h>
#include "d3dx8.h"
#include "core.h"
#include "d3dutil.h"
#include "C_vector.h"
#include "R3input.h"
#include "CommonUtil.h"
#include "MoveCamera.h"
#include "AniCamera.h"
#include "R3Move.h"
#include "R3math.h"
#include "R3Bsp.h"

static DWORD fOldTimeKey;
static float Speed,Nutime=0,sp_cnt=0,rd_cnt=0;

static _MOVE_CAMERA MoveCamera;
static Vector3f PointCamera;
static void SetPointCamera(Vector3f pos)
{
	Vector3fCopy(pos,PointCamera);
}
void GetPointCamera(Vector3f *pos)
{
	Vector3fCopy(PointCamera,pos[0]);
}


void R3FlyMoveSetPos(Vector3f pos)
{
	Vector3fCopy(pos,MoveCamera.point);
	SetPointCamera(pos);
}

void R3FlyMove(float m_speed,Vector3f player)
{
	_R3ENGINE_STATE *state=GetR3State();
//	MoveCamera.point[0] =79;
//	MoveCamera.point[1] =-118;
//	MoveCamera.point[2] =-910;

	MoveCamera.rho=1;	//크기가 0이되면 안됨.

	DWORD nowf = timeGetTime();
	float time = (float)(nowf-fOldTimeKey)/1000.f;
	Nutime += time;
	sp_cnt++;
	fOldTimeKey = nowf;
    FLOAT fViewAngle = time*8.0f;
	Speed= time*m_speed;

    // Rotate the camera around the room
	if(gScan[VK_W])
	{
		MoveCamera.speed=Speed;
		ReMoveCamera(&MoveCamera);
		rd_cnt+=Speed;
	}
	if(gScan[VK_S])
	{
		MoveCamera.speed=-Speed;
		ReMoveCamera(&MoveCamera);
		rd_cnt+=Speed;
	}
	if(gScan[VK_D])
	{
		MoveCamera.step=Speed;
		ReMoveCamera(&MoveCamera);
		rd_cnt+=Speed;
	}
	if(gScan[VK_A])
	{
		MoveCamera.step=-Speed;
		ReMoveCamera(&MoveCamera);
		rd_cnt+=Speed;
	}

	static int startx, starty; // used for dragging
	static float viewanglex,viewangley, viewanglez, orgviewanglex,orgviewangley, orgviewanglez;

	if (gMouse.button & _RIGHT_BUTTON_DOWN) 
	{              
		if( startx!=gMouse.x || starty!=gMouse.y )
		{
			viewanglex=orgviewanglex-(starty-gMouse.y)/2.0f;
			viewanglez=orgviewanglez+(startx-gMouse.x)/2.0f;
		}
	}
	else
	{
		orgviewanglex=viewanglex; orgviewanglez=viewanglez;
		startx = gMouse.x; starty = gMouse.y;
	}

	if(viewanglex<-89)		
		viewanglex =-89;
	if(viewanglex> 89)		
		viewanglex = 89;

	MoveCamera.ph = -(viewanglez*3.141592f)/(360.0f/2.0f);
	MoveCamera.th =  (viewanglex*3.141592f)/(360.0f/2.0f);

	CalculateMoveCamera(&MoveCamera);

	D3DXVECTOR3 from,at;
    D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	from.x = MoveCamera.pos[0];
	from.y = MoveCamera.pos[1];
	from.z = MoveCamera.pos[2];

	at.x = MoveCamera.point[0];
	at.y = MoveCamera.point[1];
	at.z = MoveCamera.point[2];

	SetPointCamera(MoveCamera.point);

//    D3DCamera.SetViewParams( from, at, up );
//    GetD3dDevice()->SetTransform( D3DTS_VIEW, &D3DCamera.GetViewMatrix() );
//	D3DXMatrixLookAtRH( &state->mMatView, &from, &at, &up );
	D3DXMatrixLookAtLH( &state->mMatView, &from, &at, &up );
/*
	state->mMatView.m[0][0]= - state->mMatView.m[0][0];
	state->mMatView.m[1][0]= - state->mMatView.m[1][0];
	state->mMatView.m[2][0]= - state->mMatView.m[2][0];
	state->mMatView.m[3][0]= - state->mMatView.m[3][0];
*/
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
    GetD3dDevice()->SetTransform( D3DTS_VIEW, &state->mMatView );
    D3DXMatrixInverse( &state->mInvMatView, NULL, &state->mMatView );

	//---------잠깐
	player[0]=from.x;
	player[1]=from.y;
	player[2]=from.z;

}

D3DXMATRIX *R3MoveGetViewMatrix()
{
//	D3DXMATRIX state->mMatView; 
//	state->mMatView = D3DCamera.GetViewMatrix(); 
	_R3ENGINE_STATE *state=GetR3State();
	return &state->mMatView;
}


void R3SetCameraMatrix(Vector3f get_cam_pos,float cam_mat[4][4])
{
	_R3ENGINE_STATE *state=GetR3State();
	float m_speed=R3GetTime();

//	cam_mat = PlayAniCamera(m_speed); //대략 카운트 역활...카운트에따른 카메라 매트릭스를 만들어 리턴한다.
	memcpy(state->mMatView,cam_mat,sizeof(D3DXMATRIX));
	MatrixMultiply( state->mMatViewProj.m ,state->mMatProj.m,state->mMatView.m);

//----------------------------
	
	static int startx, starty; // used for dragging
	static float viewanglex,viewangley, viewanglez, orgviewanglex,orgviewangley, orgviewanglez;
/*
	if (gMouse.button & _LEFT_BUTTON_DOWN) 
	{              
		if( startx!=gMouse.x || starty!=gMouse.y )
		{
			viewanglex=orgviewanglex-(starty-gMouse.y);
			viewanglez=orgviewanglez+(startx-gMouse.x);
		}
	}
	else
	{
		orgviewanglex=viewanglex; orgviewanglez=viewanglez;
		startx = gMouse.x; starty = gMouse.y;
	}
*/
	if(viewanglex<-89)		viewanglex =-89;
	if(viewanglex> 89)		viewanglex = 89;

	MoveCamera.ph = (viewanglez*3.141592f)/(360.0f/2.0f);
	MoveCamera.th =  -(viewanglex*3.141592f)/(360.0f/2.0f);

	D3DXMATRIX pxOut,pyOut;

	D3DXMatrixRotationY(&pxOut, MoveCamera.ph );
	D3DXMatrixRotationX(&pyOut, MoveCamera.th );
	pxOut = pxOut*pyOut;
	state->mMatView = state->mMatView*pxOut;
	

    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_VIEW, &state->mMatView );
//	player[0] = -state->mMatView.m[3][0];
//	player[1] = -state->mMatView.m[3][1];
//	player[2] = -state->mMatView.m[3][2];
    D3DXMatrixInverse( &state->mInvMatView, NULL, &state->mMatView );
	get_cam_pos[0] = state->mInvMatView.m[3][0];
	get_cam_pos[1] = state->mInvMatView.m[3][1];
	get_cam_pos[2] = state->mInvMatView.m[3][2];

	SetPointCamera(get_cam_pos);
}


R3Camera::R3Camera()
{
	_R3ENGINE_STATE *state=GetR3State();
	memset(state->mMatView,0,sizeof(D3DXMATRIX));

	::D3DXMatrixIdentity( &mMatView );
	
	mTh				= 0.f;
	mPh				= 0.f;
	mDist			= 45.f;

	//smooth 카메라를 위한
	mPointSmooth	= 0.4f;
	mRotSmooth		= 0.4f;

	mTarPoint[0] = mTarPoint[1] = mTarPoint[2] = 0.f;
	
	mTarTh			= 0.f;
	mTarPh			= 0.f;
	mTarDist		= 45.f;
	mBsp			= NULL;
}

R3Camera::~R3Camera()
{
}
float R3Camera::GetTh()			//  카메라에서 필요할때 얻어올것 들...
{
	return mTh;
}

float R3Camera::GetPh()
{
	return mPh;
}
float R3Camera::GetDist()
{
	return mDist;
}

void R3Camera::GetPoint(Vector3f get)	//float 리턴값은 [3]배열이다.
{
	Vector3fCopy(mPoint,get);
}

void R3Camera::GetPos(Vector3f get)		//float [3]배열이다.주의할점은 pos좌표는 dist,th,ph에의해 생성된다.
{
	Vector3fCopy(mPos,get);
}

D3DXMATRIX *R3Camera::GetViewMatrix()	//뷰매트릭스를 얻어낸다.
{
	_R3ENGINE_STATE *state=GetR3State();
	return &state->mMatView;
}

void R3Camera::SetPoint(float x,float y,float z)		//포인트좌표는 지정할수있다...
{
	mPoint[0] = x;
	mPoint[1] = y;
	mPoint[2] = z;
	SetPointCamera(mPoint);
}
void R3Camera::SetPos(float x,float y,float z)		//카메라 위치좌표는 계산으로 설정한다...(주의)
{
	mPos[0] = x;
	mPos[1] = y;
	mPos[2] = z;
}


void R3Camera::SetThPhDist(float th,float ph,float dist)	//회전과 거리를 지정..
{
	mTh = th;
	mPh = ph;
	mDist = dist;
}
void R3Camera::SetSmoothTarPoint(float x,float y,float z) //카메라 이동할 타겟좌표 지정...(주의)
{
	mTarPoint[0] = x;
	mTarPoint[1] = y;
	mTarPoint[2] = z;
}
void R3Camera::GetSmoothTarPoint(Vector3f get) //카메라 이동할 타겟좌표 얻기 ..(주의)
{
	get[0]=mTarPoint[0];
	get[1]=mTarPoint[1];
	get[2]=mTarPoint[2];
}
void R3Camera::SetSmoothPointLoop(float x,float y,float z)		//카메라 위치좌표는 계산으로 설정한다...(주의)
{
	mTarPoint[0] = x;
	mTarPoint[1] = y;
	mTarPoint[2] = z;

	float l_time=R3GetLoopTime();
	float g_time = 40.0f*l_time*mPointSmooth;
	g_time = min(1.0f,g_time);
/*
	mPoint[0] = (mTarPoint[0] - mPoint[0])*(g_time) + mPoint[0];
	mPoint[1] = (mTarPoint[1] - mPoint[1])*(g_time) + mPoint[1];
	mPoint[2] = (mTarPoint[2] - mPoint[2])*(g_time) + mPoint[2];
	mPoint[0] = mTarPoint[0];
	mPoint[1] = mTarPoint[1];
	mPoint[2] = mTarPoint[2];
*/
	//y만 부드럽게.
	mPoint[0] = mTarPoint[0];
	mPoint[1] = mTarPoint[1];
//	mPoint[1] = (mTarPoint[1] - mPoint[1])*(g_time) + mPoint[1];
	mPoint[2] = mTarPoint[2];
}

void R3Camera::SetSmoothThPhDistLoop(float th,float ph,float dist)	//회전과 거리를 지정..
{
	mTarTh = th;
	mTarPh = ph;

	float l_time=R3GetLoopTime();
	if( l_time == 0 )
		l_time=1;
	float g_time = 20.6f*l_time*mRotSmooth;
	g_time = min(1.0f,g_time);

	//if(mTarTh - mTh != 0)
	mTh = (mTarTh - mTh)*(g_time) + mTh;
	mPh = (mTarPh - mPh)*(g_time) + mPh;

//	_R3ENGINE_STATE *state= GetR3State();
	Vector3f src,tar,get;

	D3DXMATRIX view_mat,inv_view_mat;
	MakeViewMatrix(&view_mat);	//카메라에 맞는 viewmat를 구한다.
    D3DXMatrixInverse( &inv_view_mat, NULL, &view_mat );

	src[0] = inv_view_mat.m[3][0];
	src[1] = inv_view_mat.m[3][1];
	src[2] = inv_view_mat.m[3][2];

	tar[0] = mTarPoint[0];
	tar[1] = mTarPoint[1];
	tar[2] = mTarPoint[2];

	SetPointCamera(mTarPoint);

	Vector3fSub(src,tar,get);
	if( !(get[0] == 0 && get[1] == 0 && get[2] == 0) )
	{
		if( dist < 2 )	//거리가 아에 작게 나오면 예를들어 1인칭..
		{
			mTarDist = dist;
		}
		else
		{
			if( mTh < 20 )	//아래로 꺽을때다.
			{
				if( mTh < -40 )
					dist = 7;
				else
					dist = dist - ( (dist-7) * (-mTh+20))/60.0f;
			}
			Normalize(get);
			Vector3fMul(get,(dist+10),src);
			Vector3fAdd(src,tar,src);
			//충돌 첵크  이거는 내가 구해야쥐..
			mTarDist = dist;//이거는 내가 구해야쥐..
			if( ((CBsp *)mBsp)->IsLoaded() && ((CBsp *)mBsp)->IsCollisionFace( tar, src,&get,2) )
			{
				Vector3fSub(tar,get,tar);
				mTarDist = Vector3fDist(tar);
				mDist=mTarDist;	//충돌 첵크는 바로 해줘야 안뚫려보인다.
				if( mTarDist > dist )
				{
					mDist = dist;
					mTarDist = dist;
				}				
			}
			else
			{
				mTarDist = dist;//이거는 내가 구해야쥐..
				if( mTarDist < 7 )
					mTarDist = 7;
				if( mTarDist > dist )
					mTarDist = dist;
			}
		}
	}
	mDist = (mTarDist - mDist)*(g_time) + mDist;
}

void R3Camera::SetSmoothValue(float point,float rot)	// 0-1 사이... 숫자가 높을수록 카메라가 부드러워진다.0이면 완전히 끊어진다.  
{
	mPointSmooth = point;
	mRotSmooth = rot;
}
void R3Camera::SetBspPtr(void *bsp)
{
	mBsp = bsp;
}

void R3Camera::MakeViewMatrix(D3DXMATRIX *mat)		//모든 값을 지정한후에 한번호출해줘해야한다.  내부에서 pos값과 뷰매트릭스를 생성한다.
{
	float ph,th;

	if(mTh<-89)
		mTh =-89;
	if(mTh> 89)
		mTh = 89;

	ph = (mPh*3.141592f)/(360.0f/2.0f);
	th =  -(mTh*3.141592f)/(360.0f/2.0f);

	D3DXMATRIX temp1,temp2,temp3;

    D3DXMatrixIdentity( &temp3 );
    D3DXMatrixIdentity( &temp2 );
	temp2.m[3][0]=-mPoint[0];
	temp2.m[3][1]=-mPoint[1];
	temp2.m[3][2]=-mPoint[2];
	temp3 = temp3*temp2;
	D3DXMatrixRotationY(&temp1, ph );
	D3DXMatrixRotationX(&temp2, th );
	temp3 = temp3*temp1;
	temp3 = temp3*temp2;
	
    D3DXMatrixIdentity( &temp2 );
	temp2.m[3][2]=mDist;

	*mat = temp3*temp2;
}

	
void R3Camera::MakeCameraAndViewMatrix()		//모든 값을 지정한후에 한번호출해줘해야한다.  내부에서 pos값과 뷰매트릭스를 생성한다.
{
	_R3ENGINE_STATE *state=GetR3State();
	D3DXMATRIX view_mat;

	MakeViewMatrix(&state->mMatView);	//카메라에 맞는 viewmat를 구한다.

	MatrixMultiply( state->mMatViewProj.m ,state->mMatProj.m,state->mMatView.m);

    D3DXMatrixInverse( &state->mInvMatView, NULL, &state->mMatView );
	mPos[0] = state->mInvMatView.m[3][0];
	mPos[1] = state->mInvMatView.m[3][1];
	mPos[2] = state->mInvMatView.m[3][2];

	SetPointCamera(mTarPoint);
}

void R3Camera::SetViewMatrix()	//다이렉트 3D ViewMat에 셋팅해준다. 
{
	GetD3dDevice()->SetTransform( D3DTS_VIEW, &GetR3State()->mMatView );
}
extern float GetGravity( float num );     // 0은 최 정점	-는 올라간다 +는 내려간다.
/*
//--------------------자동 카메라..
#define AutoStartDelayTime	0.4	//초단위 
#define AutoAccelTime		1	//가속도 량.

void R3Camera::SetAutoSmoothThPhDistLoop(float *th,float *ph,float dist)	//회전과 거리를 지정..
{
	mTarTh = *th;
	mTarPh = *ph;

	float l_time=R3GetLoopTime();
	if( l_time == 0 )
		l_time=1;
	float g_time = 8.6f*l_time*mRotSmooth;
	g_time = min(1.0f,g_time);

	float n_time=R3GetTime();
	float time = n_time - mCharYangleForTime - AutoStartDelayTime;

	if( mPh==mTarPh)
		return;

	float ph_temp = mPh-mOldPh;

	if( fabs(ph_temp) > mPhDist/2)
		mTime -= l_time ;
	else
		mTime += l_time ;

	if( mTime < 0 )	//끝에 다와서 거꾸로가는것을 막자.
		mPh += mPhSign*-GetGravity(l_time)/2.0f;
	else
	{
		float temp = min(mTime,0.3);
		mPh += mPhSign*-GetGravity(mTime)/2.0f;
	}
	if( mPhSign > 0 )
	{
		if(mTarPh - mPh <= 0)
		{
			mPh=mTarPh;
			if( mPh<0 )
				mPh+=360;
			if( mPh>360 )
				mPh-=360;
			mTarPh = mPh;
			*ph = mPh;
		}
	}
	else
	{
		if(mTarPh - mPh >= 0)
		{
			mPh=mTarPh;
			if( mPh<0 )
				mPh+=360;
			if( mPh>360 )
				mPh-=360;
			mTarPh = mPh;
			*ph = mPh;
		}
	}
	_R3ENGINE_STATE *state= GetR3State();
	Vector3f src,tar,get;
	src[0] = state->mInvMatView.m[3][0];
	src[1] = state->mInvMatView.m[3][1];
	src[2] = state->mInvMatView.m[3][2];

	tar[0] = mTarPoint[0];
	tar[1] = mTarPoint[1];
	tar[2] = mTarPoint[2];

	SetPointCamera(mTarPoint);

	Vector3fSub(src,tar,get);
	if( !(get[0] == 0 && get[1] == 0 && get[2] == 0) )
	{
		if( mTh < 20 )	//아래로 꺽을때다.
		{
			if( mTh < -40 )
				dist = 7;
			else
				dist = dist - ( (dist-7) * (-mTh+20))/60.0f;
		}
		Normalize(get);
		Vector3fMul(get,(dist+10),src);
		Vector3fAdd(src,tar,src);
		//충돌 첵크  이거는 내가 구해야쥐..
		mTarDist = dist;//이거는 내가 구해야쥐..
		if( ((CBsp *)mBsp)->IsCollisionFace( tar, src,&get,2) )
		{
			Vector3fSub(tar,get,tar);
			mTarDist = Vector3fDist(tar);
		}
		else
			mTarDist = dist;//이거는 내가 구해야쥐..

		if( mTarDist < 7 )
			mTarDist = 7;
		if( mTarDist > dist )
			mTarDist = dist;
	}
	mDist = (mTarDist - mDist)*(g_time) + mDist;
}



void R3Camera::SetAutoCameraThPh(float *th,float *ph,float ch_y_angle)	//자동카메라 회전을 위한..
{
	float n_time=R3GetTime();

	if( ch_y_angle == mCharYangle )
	{
		mCharYangleForTime = n_time;
		return;
	}
	if( n_time - mCharYangleForTime < AutoStartDelayTime )
	{
		return;
	}
	mCharYangle = ch_y_angle;
	mTime=0;

	*th = 15;
	*ph = (360 - ch_y_angle);
	float temp_ph = *ph;
	if( mPh > temp_ph )
	{
		temp_ph+=360;
	}
	mOldPh = mPh;
	if( (temp_ph - mPh) > 180 )
	{
		mPhSign = -1.0f;
		mPhDist = 360-(temp_ph - mPh);
		*ph = temp_ph-360;
	}
	else
	{
		mPhSign = +1.0f;
		mPhDist = (temp_ph - mPh);
		*ph = temp_ph;
	}
}
*/