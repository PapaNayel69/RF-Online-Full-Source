#ifndef __R3Move_H__
#define __R3Move_H__

#include "C_vector.h"
#include "R3D3d8.h"
//#define EXPORT extern "C" _declspec (dllexport)


class R3Camera{
private:
	Vector3f mPos,mPoint;	// 
	D3DXMATRIX mMatView;	//  이카메라의 뷰 매트릭스다..
	float mTh;				//	카메라 위아래 회전...	한바퀴가 360.0 각도 
	float mPh;				//  카메라가 포인트를 중심으로 좌우 회전...
	float mDist;			//  카메라의 포인트와 의 거리...

	//auto 카메라를 위한.
	float mTime;	//시간 가속을 위한.
	float mOldPh;				//  카메라가 포인트를 중심으로 좌우 회전...
	float mPhDist;				//Ph값 이동할 차이.
	float mPhSign;				//Ph값 보간을 위한 부호.
	float mCharYangle;			//캐릭터 yangle값을 이용한다.
	float mCharYangleForTime;	//캐릭터 yangle값 변한 시간을 저장한다.
	//smooth 카메라를 위한
	float mPointSmooth,mRotSmooth;
	Vector3f mTarPoint;
	float mTarTh;
	float mTarPh;
	float mTarDist;
	void *mBsp;	//bsp포인터...
	void MakeViewMatrix(D3DXMATRIX *mat);
public:
	//-----외부에서 접근할수있는 API
	R3Camera();
	~R3Camera();
	float GetTh();			//  카메라에서 필요할때 얻어올것 들...
	float GetPh();
	float GetDist();
	void GetPoint(Vector3f get);	//float 리턴값은 [3]배열이다.
	void GetPos(Vector3f get);		//float [3]배열이다.주의할점은 pos좌표는 dist,th,ph에의해 생성된다.
	D3DXMATRIX *GetViewMatrix();	//뷰매트릭스를 얻어낸다.

	void SetPos(float x,float y,float z);		//카메라 위치좌표는 계산으로 설정한다...(주의)
	void SetPoint(float x,float y,float z);		//포인트좌표는 지정할수있다...
	void SetThPhDist(float th,float ph,float dist);	//회전과 거리를 지정..

	//오토 카메라.
	void SetAutoCameraThPh(float *th,float *ph,float ch_y_angle);	//자동카메라 회전을 위한..
	void SetAutoSmoothThPhDistLoop(float *th,float *ph,float dist);	//회전과 거리를 지정..반드시 루프마다
	//------Smooth 카메라 회전,맵과의 충돌도 포함된다.
	void SetSmoothValue(float point,float rot);	// 0-1 사이... 숫자가 높을수록 카메라가 부드러워진다.0이면 완전히 끊어진다.  
	void SetSmoothPointLoop(float x,float y,float z);			//포인트좌표 반드시 루프마다 
	void SetSmoothThPhDistLoop(float th,float ph,float dist);	//회전과 거리를 지정..반드시 루프마다
	
	void SetSmoothTarPoint(float x,float y,float z); //카메라 이동할 타겟좌표 지정...(주의)
	void GetSmoothTarPoint(Vector3f get); //카메라 이동할 타겟좌표 얻기 ..(주의)
	void SetBspPtr(void *bsp);

	void MakeCameraAndViewMatrix();		//모든 값을 지정한후에 한번호출해줘해야한다.  내부에서 pos값과 뷰매트릭스를 생성한다.
	void SetViewMatrix();
};


/*
	카메라쓰는 두가지 방법
	1. 카메라를 바깥에서 제어하려면 R3Camera 클래스를 이용해서 접근한다.
	2. 아래 유틸리티를 이용한다. 단 인터페이스가 내장되어있어 바깥에서건드리지 못한다.
*/

//--- 카메라 유틸리티....평범한 움직임관련
void R3FlyMoveSetPos(Vector3f pos);
void R3FlyMove(float m_speed,Vector3f player);
D3DXMATRIX *R3MoveGetViewMatrix();	//카메라쓴것을 얻어온다.
void GetPointCamera(Vector3f *pos);


void R3SetCameraMatrix(Vector3f get_cam_pos,float cam_mat[4][4]);

#endif