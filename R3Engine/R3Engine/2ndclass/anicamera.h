#ifndef __ANICAMERA_H__
#define __ANICAMERA_H__

#include "Animation.h"
#define _ANICAMERA_VERSION 1.2

typedef struct{
	char mName[64];		//카메라 이름.
	DWORD h_num;	//	hirerachy 갯수
	float fov;
	float tdist;
	_ANI_OBJECT *obj;
}_ANI_CAMERA;

#define _CAM_FLAG_LOOP			0x0
#define _CAM_FLAG_FINAL_STOP	0x1	//지정된 끝프래임에 정지한다.

#define _CAM_PLAY_FULL_FRAME 	-1

class CAniCamera{
private:
	_ANI_CAMERA *AniCamera;
	_ANI_OBJECT *mDummy;		//더미오브젝트들..
	DWORD mDummyNum;
	DWORD mAniCameraNum;
	DWORD mStartFrame,mEndFrame;
	DWORD mPlayStartFrame,mPlayEndFrame;
	int mPlayIndex;		//어떤카메라를 리턴하는가..
	BOOL mIsSetPerspect;
	float mStartTick;	//시작 틱이다.
	DWORD mFlag;	//플래그다.

	float mOldFrame;	//이전 프래임값.
	float mNowFrame;		//현재 진행중인 프래임.

	float *GetMatrixAniCamera(float frame); //대략 카운트 역활...카운트에따른 카메라 매트릭스를 만들어 리턴한다.
	void GetDummyMatrixSub(float mat[4][4],DWORD id,float frame);

public:
	CAniCamera();
	~CAniCamera();

	void GetDummyMatrix(float mat[4][4],DWORD id,float frame); //frmae에따른 매트릭스를 만들어 리턴한다.
	char *GetDummyParentID(DWORD id);
	DWORD GetDummyID(char *name,BOOL check_lwr=FALSE);	//이름과 소문자 첵크여부인자. 

	DWORD GetStartFrame()	{	return mStartFrame;	};	//전체 카메라 시작 프래임.
	DWORD GetEndFrame()		{	return mEndFrame;	};	//전체 카메라 끝 프래임 

	void SetNowFrame( DWORD frame);
	float GetNowFrame()					{	return mNowFrame;	};		//현재 진행중인 프래임.
	float GetOldFrame()					{	return mOldFrame;	};		//과거 프래임.

	DWORD GetCameraNum()	{	return mAniCameraNum;	};	//하나의 파일에 여러개의 카메라파일이 가능하다.

	char *GetCameraName(DWORD id);	//카메라 이름을 리턴한다. 존재치않으면 NULL
	DWORD GetCameraIndex(char *name);	//카메라 인덱스를 리턴한다. 존재치않으면 -1

	BOOL IsLoadedAniCamera();
	void LoadAniCamera(char *name);
	void ReleaseAniCamera();

	void SetPrePlayCamera(DWORD id,DWORD start_frame=_CAM_PLAY_FULL_FRAME
		,DWORD end_frame=_CAM_PLAY_FULL_FRAME,DWORD flag=_CAM_FLAG_LOOP);
	BOOL PlayAniCamera(float matrix[4][4],float speed_scale=1.0f,BOOL pause=FALSE);		//지정된 끝 플래이가 끝날경우FALSE를 리턴한다.(loop일경우 항상 true가된다.)
};

#endif