#include <stdio.h>
#include "C_vector.h"
#include "R3D3d8.h"
#include "R3Math.h"
#include "JError.h"
#include "Jmalloc.h"
#include "AniCamera.h"
#include "CommonUtil.h"


CAniCamera::CAniCamera()
{
	memset(this,0,sizeof(CAniCamera));
}
CAniCamera::~CAniCamera()
{
}

static float stMakedViewMatrix[4][4];
/*
static int stAniCameraState;

void SetAniCameraState(int state)
{
	stAniCameraState = state;
}
int IsAniCameraState(void)
{
	return stAniCameraState;
}
*/
BOOL CAniCamera::IsLoadedAniCamera()
{
	if( mAniCameraNum )
		return TRUE;
	return FALSE;
}

char *CAniCamera::GetCameraName(DWORD id)	//카메라 이름을 리턴한다. 존재치않으면 NULL
{
	if( mAniCameraNum <= id)
		return NULL;
	return AniCamera[id].mName;
}

DWORD CAniCamera::GetCameraIndex(char *name)	//카메라 인덱스를 리턴한다. 존재치않으면 -1
{
	char cmp_name[256];

	strcpy(cmp_name,name);
	strlwr(cmp_name);

	for(DWORD i=0; i<mAniCameraNum; i++)
	{
		if( !strcmp(cmp_name,AniCamera[i].mName))
			return i;
	}
	return -1;
}

void CAniCamera::LoadAniCamera(char *name)
{
	DWORD i,j;
	float version;
	FILE *fp =fopen(name,"rb");

	if( fp == NULL)
		return;
	if( mAniCameraNum != 0)
		ReleaseAniCamera();
	fread(&version,4,1,fp);	//항상 버젼부터..
	if( version< _ANICAMERA_VERSION )
	{
		Error("애니메이션 카메라버젼이 낮습니다.","");
		return;
	}
	// 최상의 부모를 찾아서 먼저 세이브
	fread(&mAniCameraNum,4,1,fp);
	if(mAniCameraNum==0)
	{
		fclose(fp);
		return;
	}
	fread(&mStartFrame,4,1,fp);
	fread(&mEndFrame,4,1,fp);
	
	AniCamera=(_ANI_CAMERA*)Dmalloc(sizeof(_ANI_CAMERA)*mAniCameraNum);
	for(i=0; i<mAniCameraNum; i++)
	{
		fread(&AniCamera[i].mName,64,1,fp);	//카메라 이름.
		fread(&AniCamera[i].fov,4,1,fp);
		fread(&AniCamera[i].tdist,4,1,fp);
		fread(&AniCamera[i].h_num,4,1,fp);
		
		AniCamera[i].obj=(_ANI_OBJECT*)Dmalloc(sizeof(_ANI_OBJECT)*AniCamera[i].h_num);
		for( j=0; j<AniCamera[i].h_num; j++)
		{
			fread(AniCamera[i].obj[j].s_matrix,sizeof(D3DXMATRIX),1,fp);
			fread(AniCamera[i].obj[j].pos,sizeof(Vector3f),1,fp);
			fread(AniCamera[i].obj[j].quat,sizeof(Vector4f),1,fp);
			fread(&AniCamera[i].obj[j].Pos_cnt,4,1,fp);
			fread(&AniCamera[i].obj[j].Rot_cnt,4,1,fp);
			AniCamera[i].obj[j].Pos = (_POS_TRACK *)Dmalloc(sizeof(_POS_TRACK)*AniCamera[i].obj[j].Pos_cnt
				+sizeof(_ROT_TRACK)*AniCamera[i].obj[j].Rot_cnt );
			AniCamera[i].obj[j].Rot = (_ROT_TRACK*)((BYTE *)AniCamera[i].obj[j].Pos + sizeof(_POS_TRACK)*AniCamera[i].obj[j].Pos_cnt);
			fread(AniCamera[i].obj[j].Pos,sizeof(_POS_TRACK)*AniCamera[i].obj[j].Pos_cnt,1,fp);
			fread(AniCamera[i].obj[j].Rot,sizeof(_ROT_TRACK)*AniCamera[i].obj[j].Rot_cnt,1,fp);
			AniCamera[i].obj[j].Scale =NULL;
			AniCamera[i].obj[j].Scale_cnt =0;
		}
	}
	fread(&mDummyNum,4,1,fp);
	if( mDummyNum )
		mDummy=(_ANI_OBJECT*)Dmalloc(sizeof(_ANI_OBJECT)*mDummyNum);

	for( i=0; i<mDummyNum; i++)
	{
		fread(mDummy[i].ObjectName,64,1,fp);
		fread(mDummy[i].ParentName,64,1,fp);
		fread(mDummy[i].s_matrix,sizeof(D3DXMATRIX),1,fp);
		fread(mDummy[i].pos,sizeof(Vector3f),1,fp);
		fread(mDummy[i].quat,sizeof(Vector4f),1,fp);
		fread(&mDummy[i].Pos_cnt,4,1,fp);
		fread(&mDummy[i].Rot_cnt,4,1,fp);
		mDummy[i].Pos = (_POS_TRACK *)Dmalloc(sizeof(_POS_TRACK)*mDummy[i].Pos_cnt
			+sizeof(_ROT_TRACK)*mDummy[i].Rot_cnt );
		mDummy[i].Rot = (_ROT_TRACK*)((BYTE *)mDummy[i].Pos + sizeof(_POS_TRACK)*mDummy[i].Pos_cnt);
		fread(mDummy[i].Pos,sizeof(_POS_TRACK)*mDummy[i].Pos_cnt,1,fp);
		fread(mDummy[i].Rot,sizeof(_ROT_TRACK)*mDummy[i].Rot_cnt,1,fp);
		mDummy[i].Scale =NULL;
		mDummy[i].Scale_cnt =0;
	}
	
	fclose(fp);
}

char *CAniCamera::GetDummyParentID(DWORD id)
{
	if( id >= mDummyNum )
		return NULL;

	return mDummy[id].ParentName;
}
DWORD CAniCamera::GetDummyID(char *name,BOOL check_lwr)
{
	char buf[256];
	DWORD i;

	if( check_lwr )
	{
		for( i=0; i<mDummyNum; i++)
		{
			strcpy(buf,name);
			strlwr(buf);
			if( !strcmp(name,buf ))
				return i;
		}
	}
	else
	{
		for( i=0; i<mDummyNum; i++)
		{
			if( !strcmp(name,mDummy[i].ObjectName ))
				return i;
		}
	}
	return -1;
}

void CAniCamera::ReleaseAniCamera()
{
	if(mAniCameraNum==0)
		return;
	for(DWORD i=0; i<mAniCameraNum; i++)
	{
		for( DWORD j=0; j<AniCamera[i].h_num; j++)
		{
			if( AniCamera[i].obj[j].Pos_cnt+AniCamera[i].obj[j].Rot_cnt )
				Dfree(AniCamera[i].obj[j].Pos);
		}
		Dfree(AniCamera[i].obj);
	}
	Dfree(AniCamera);
	for( i=0; i<mDummyNum; i++)
	{
		if( mDummy[i].Pos_cnt + mDummy[i].Rot_cnt )
			Dfree(mDummy[i].Pos);
	}
	if( mDummy )
		Dfree(mDummy);
	memset(this,0,sizeof(CAniCamera));
}

void CAniCamera::GetDummyMatrixSub(float mat[4][4],DWORD id,float frame)
{
	float t_p[4][4],get_ani[4][4];

	DWORD p_id = GetDummyID(mDummy[id].ParentName);
	if( p_id !=-1 )
	{
		GetDummyMatrixSub(mat,p_id,frame);
	}
	GetAniMatrix(get_ani,&mDummy[id],frame);
	MatrixMultiply( t_p,mat, get_ani);
	MatrixCopy(mat,t_p);
}

void CAniCamera::GetDummyMatrix(float mat[4][4],DWORD id,float frame) //frmae에따른 매트릭스를 만들어 리턴한다.
{
	if(mDummyNum==0 || 	( id >= mDummyNum ))
	{
		MatrixIdentity(mat);
		return;
	}
	frame*=30.0f;	//초당 프래임..
	if( frame > mEndFrame-mStartFrame )
	{
		int spare = (int)frame%(int)(mEndFrame-mStartFrame);
		float f_spare = frame-(float)((int)frame);
		frame = f_spare+(float)spare;
	}
	
	frame = frame-mStartFrame;

	float t_p[4][4],get_ani[4][4];
	MatrixIdentity(mat);
	DWORD p_id;

	GetDummyMatrixSub(mat,id,frame);

	GetMatrixFrom3DSMAXMatrix(mat);
}


float *CAniCamera::GetMatrixAniCamera(float frame) //대략 카운트 역활...카운트에따른 카메라 매트릭스를 만들어 리턴한다.
{
	DWORD i;

	_ANI_CAMERA *ani_camera = &AniCamera[mPlayIndex];

	if(mAniCameraNum==0)
	{
		MatrixIdentity(stMakedViewMatrix);
		return (float *)&stMakedViewMatrix[0][0];
	}
	if( mIsSetPerspect == 0 )
	{
		mIsSetPerspect=1;

		//카메라의 원근을 셋팅한다.//fov값을 설정 라디안..
		SetFov(ani_camera->fov);
	}
	frame*=30.0f;	//초당 프래임..
//	frame=0.0f;	//초당 프래임..
	if( frame > mEndFrame-mStartFrame )
	{
		int spare = (int)frame%(int)(mEndFrame-mStartFrame);
		float f_spare = frame-(float)((int)frame);
		frame = f_spare+(float)spare;
	}
	
	frame = frame-mStartFrame;
	float temp[4][4],t_p[4][4],get_ani[4][4];
	MatrixIdentity(temp);
	for(i=0; i<ani_camera->h_num; i++)
	{
		GetAniMatrix(get_ani,&ani_camera->obj[i],frame);
		MatrixMultiply( t_p,temp, get_ani);
		MatrixCopy(temp,t_p);
	}

	GetMatrixFrom3DSMAXMatrix(temp);
	MatrixInvert(stMakedViewMatrix,temp);

  //---- 오른손 좌표계를 왼손좌표계로 바꾼다.
	D3DXMatrixRotationY((D3DXMATRIX*)t_p, 3.141592f );
//	*(D3DXMATRIX*)temp = *(D3DXMATRIX*)stMakedViewMatrix**(D3DXMATRIX*)t_p;
	MatrixMultiply( temp,t_p,stMakedViewMatrix);	//d3dxmatrix와 곱순서가 다른다. 주의...
	MatrixCopy(stMakedViewMatrix,temp);

	stMakedViewMatrix[0][0]= - stMakedViewMatrix[0][0];
	stMakedViewMatrix[1][0]= - stMakedViewMatrix[1][0];
	stMakedViewMatrix[2][0]= - stMakedViewMatrix[2][0];
	stMakedViewMatrix[3][0]= - stMakedViewMatrix[3][0];
	//----

	return (float *)&stMakedViewMatrix[0][0];
}

void CAniCamera::SetPrePlayCamera(DWORD id,DWORD start_frame,DWORD end_frame,DWORD flag)
{
	if( mAniCameraNum == 0 )
	{
		Warning("카메라가 한대도 없습니다.","");
		return;
	}
	if( start_frame == _CAM_PLAY_FULL_FRAME && end_frame == _CAM_PLAY_FULL_FRAME )
	{
		start_frame=mStartFrame;
		end_frame=mEndFrame;
	}
//-----인자 잘못 넘겨줄때 자동 수정..
	if( start_frame < mStartFrame )
		start_frame=mStartFrame;
	if( end_frame < mStartFrame )
		end_frame=mStartFrame;

	if( start_frame > mEndFrame )
		start_frame=mEndFrame;
	if( end_frame > mEndFrame )
		end_frame=mEndFrame;
	if( id >= mAniCameraNum )
		id = mAniCameraNum-1; 

	mPlayStartFrame=start_frame;
	mPlayEndFrame=end_frame;
	if( mAniCameraNum <= id )
		id=mAniCameraNum-1;
	mPlayIndex=id;		//어떤카메라를 리턴하는가..
	mIsSetPerspect=FALSE;
	mStartTick = R3GetTime();	//시작 틱이다.
	mFlag=flag;
}

void CAniCamera::SetNowFrame( DWORD frame)
{
//	mNowFrame=frame;
//	mOldFrame=frame;
	mStartTick = R3GetTime()-mNowFrame-(mPlayStartFrame/30.0f);
//	mStartTick+=R3GetTime()-mStartTick;		//프래임강제 지정.
}

BOOL CAniCamera::PlayAniCamera(float matrix[4][4],float speed_scale,BOOL pause)		//지정된 끝 플래이가 끝날경우FALSE를 리턴한다.(loop일경우 항상 true가된다.)
{
	BOOL ret = TRUE;
	float frame;
	if( mAniCameraNum == 0 )
	{
		return ret;
	}
	if( pause )
	{
		frame = mNowFrame;
	}
	else
	{
		float diff = (R3GetTime() - mStartTick)*speed_scale;	//시작 틱과 차이가....
		if( mPlayEndFrame >= mPlayStartFrame )
			frame = (mPlayStartFrame/30.0f)+diff;	//순방향.
		else
			frame = (mPlayStartFrame/30.0f)-diff;	//역방향
	}


//	if( mFlag == _CAM_FLAG_FINAL_STOP )
	{
		if( mPlayEndFrame >= mPlayStartFrame )
		{
			if( frame >= mPlayEndFrame/30.0f )
			{
				frame = mPlayEndFrame/30.0f;
				ret= FALSE;
			}
		}
		else	//역방향.
		{
			if( frame <= mPlayEndFrame/30.0f )
			{
				frame = mPlayEndFrame/30.0f;
				ret= FALSE;
			}
		}
	}
	mOldFrame = mNowFrame;
	mNowFrame = frame;
	//mPlayIndex
	GetMatrixAniCamera( frame); //대략 카운트 역활...카운트에따른 카메라 매트릭스를 만들어 리턴한다.
	MatrixCopy(matrix,stMakedViewMatrix);

	return ret;
}
