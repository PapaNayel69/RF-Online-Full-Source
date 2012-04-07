#ifndef __LEVEL_H__
#define __LEVEL_H__

//#include "C_Vector.h"
#include "R3Bsp.h"
#include "SkyBox.h"
#include "R3Move.h"
#include "AniCamera.h"
#include "ExtDummy.h"

//----------------------------------타이머 클래스: 맵당 하나씩 넣을때..----------------------------
typedef unsigned __int64 UINT64;
class CTimer{
private:
	//------ 엔진 내부 타이머 
	float mLoopTime,mTime,mRealTime;
	float mMinFPS,mfLoopHop;
	DWORD mOldTime;
	DWORD mLoopCnt;
	DWORD mLoopHop;
	BOOL m_bUsingQPF;
	BOOL m_bTimerInitialized;
	float mFPS;
	DWORD mLoopFPSCnt;
	float mFPSTime;

	float m_fTicksPerSec;
	float m_fFramesPerSec;
	float m_fAverageFramesPerSec;
	float m_fSecsPerFrame;
	float m_fLamTime;

	UINT64 m_qwTicks;
	UINT64 m_qwStartTicks;
	UINT64 m_qwTicksPerSec;
	UINT64 m_qwTicksPerFrame;

	float GetDuration();
	UINT64 GetTicks();
public:
	CTimer();
	float GetLoopTime();	//한루프의 도는 시간을 건진다.
	float GetTime();				//현재 시간을 알아낸다.
	void SetTime(float time);	//타임을 재 설정...
	void SetMinFPS(float min_fps);	//최소 프래임 레이트를 정의한다.
	void CalculateTime();	//루프마다 해줘야된다.
	float GetFPS()		{	return mFPS;	};

};


class CLevel{
private:
	char mMapName[256];		//레벨의 맵이름...
	Vector3f mCamPos;			//카메라 위치좌표.
	D3DXMATRIX mMatView;	//실제 렌더링걸려는 카메라의 뷰 매트릭스.. 
	BOOL mIsLoadedBsp;
public:

	CBsp	*mBsp;	//해당 레벨의 맵 bsp
	CSkyBox	*mSkyBox;	//해당 레벨의 스카이박스
	CAniCamera mAutoAniCam;		//기본 자동 카메라이다.	신경쓰지 마시길..
	CTimer	mTimer;				//내부 타이머....
	CExtDummy mDummy;			//더미닷.

	//----레벨 관련 텍스쳐 메모리사이즈...
	DWORD mLightTexMemSize;
	DWORD mMapTexMemSize;
	DWORD mSkyTexMemSize;
	DWORD mEntityTexMemSize;

	CLevel();
	virtual ~CLevel();

	void LoadLevel(char *name=NULL);		//레벨을 파일로 읽는다 NULL일경우현재 mMapName을 참조해서 읽는다.
	void ReleaseLevel();

	BOOL IsLoadedBsp() const { return mIsLoadedBsp; } //맵파일이 로딩되었는가?
	void CalcR3Fog();						//포그를 계산한다.
	
	void FrameMove();						//맵 렌더링하기전에 반드시 호출하는것.. 맵렌더 사전준비이다.

	void DrawSkyBoxRender();				//하늘을 렌더링한다.
	void DrawMapRender();					//맵을 렌더링한다.
	void DrawMapEntitiesRender();			//맵안의 엔티티들을 렌더링한다.
	void DrawMapAlphaRender(Vector3f pos);	//맵에서 알파 부분을 렌더링한다.
	void DrawLeafBBox();	//리프의 바운딩 박스를 출력.
	void DrawMatBBox();		//머터리얼블록 바운딩 박스 출력.

	void PrepareShadowRender(Vector3f ch_pos,void *tex,float intensity,DWORD blur=3,float scale=1,float add_pos=0);	// 
	void DrawShadowRender(Vector3f ch_pos);

	void HearMapSound();	//맵 사운드를 듣자.(렌더링이 아니므로 endscene()이후에 넣자..)

	//---- 맵렌더 하기전에 아래2개를 셋팅해줘야한다.
	void SetCameraPos(Vector3f camera);	
	void SetViewMatrix(D3DXMATRIX *view_mat);	// 카메라를 안 이용하고 외부에서 직접접근하는 방법..

	//-------맵 유틸에 관한거... 
	BOOL GetPointFromScreenRay(LONG x,LONG y,Vector3f *get_point);	//마우스클릭된 폴리곤 포인트를 얻어낸다.
	float GetFirstYpos(Vector3f pos,int option);	// 처음 y좌료를 
	float GetFirstYpos(Vector3f pos,Vector3s min,Vector3s max);	//바운딩 박스 영역안에서 구한다.구할수 없는경우 65535을 리턴
	float GetFirstYpos(Vector3f pos,Vector3f min,Vector3f max);	//	바운딩 박스영역에서 y값을 얻어온다.
	BOOL GetNextYpos(Vector3f tpos,float *ypos);	//다음위치의 y좌표를..	
	BOOL GetNextYposNoAttr(Vector3f tpos,float *ypos);	//다음위치의 y좌표를..	

	BOOL GetNextYposFarProgress(Vector3f src_pos,Vector3f tar_pos,float *ypos);
	
	//타이머체크해서 
	//프래임이 안나올경우 플레이어가 공중에 뜨거나 박히지 않게 하기위한 함수.
	BOOL GetNextYposFar(Vector3f src_pos,Vector3f tar_pos,float *ypos);

	BOOL GetNextYposForServer(Vector3f tpos,float *ypos);	//다음위치의 y좌표를..	서버에서만 쓴다. 에지노멀을 가지고 있으므로 속도 좋다.
	BOOL GetNextYposForServerFar(Vector3f src_pos,Vector3f tar_pos,float *ypos);//서버에서 일정시간에 호출하는거다. 
/*
	GetNextYposForServerFar()함수에대해...
	서버에서의 Ypos는 속도문제로인해 매루프보다는 캐릭터 키(현재18)를 벗어나지 않을 거리에서 이함수로 
	사용하는 편이 빠르다. 
	예를 들어 몬스터가 1초에 40정도 움직인다면 18이란 거리를 0.45초만에 가므로 0.45초 전에 ypos를 측정하면 
	되겠다. 하지만 좀더 정확해야될 필요가 있으므로 절반인 약0.2초정도로 측정하자..
*/
	BOOL IsCollisionRayAABB(LONG x,LONG y,Vector3f BBmin,Vector3f BBmax, Vector3f *get_cross);	//바운딩 박스와의 충돌..

	void GetFrustumNormalPlane(Vector4f n_plane[6]);	//core에 view frustum 평면노멀을 리턴한다.

	//기타 유틸...
	void DrawBBox(Vector3f bb_min,Vector3f bb_max,DWORD color);	//바운딩 박스를 라인으로 그리는 함수.
	void DrawBBox(Vector3s bb_min,Vector3s bb_max,DWORD color);	//바운딩 박스를 라인으로 그리는 함수.
	void DrawTestBox(Vector3f box_size,Vector3f pos,DWORD color);		//사람테스트할 박스유틸...
	void ScreenShot();
	void ReLoadAllMaterial();	//머터리얼값만 재로드하는것..
	char *GetMapName();		//현재 맵이름을 알아낸다.


	DWORD GetPath(Vector3f from,Vector3f at,Vector3f get[16],DWORD *cnt,int depth=_MAX_PATH_POS);	//패스 파인더.
	BOOL GetPathFromDepth(Vector3f from,Vector3f at,int depth,Vector3f get[16],DWORD *cnt);	//패스 파인더.
	//리턴값으로 패스를 못찾을경우 FALSE를 리턴하지만 최대 갈수있는 좌표를 넣어준다.

	//------ 툴에서만 쓸 API 

	BOOL GetPointFromScreenRayFar(LONG x,LONG y,Vector3f *get_point);	//픽킹된 가장 멀리있는 폴리곤을 얻어낸다.
};


#endif