#ifndef __R3PARTICLE_H__
#define __R3PARTICLE_H__

#include "windows.h"
#include "c_vector.h"
#include "entity.h"

//------------파티클 생성요소들이 너무나 많아 기본셋팅값들이 존재 한다.

#define _PARTICLE_TYPE_FALL			1	//단순 추락부터 시작한다.
#define _PARTICLE_TYPE_SMOG			2	//연기.
#define _PARTICLE_TYPE_EXPLOSION	3	//폭발타잎에서 시작한다.
#define _PARTICLE_TYPE_DRAGON		4	//용의 불.

//이 파티클은 파일로 저장된다.
typedef struct {
	char name[64];	//파티클 숏컷 이름.
	DWORD mFlag;		//파티클의 속성... 
	LONG mCreateArea[3];	//	구 영역의 경우..[0]번 배열에 반지름이...있다. 박스는 x,y,z 
	float mLiveTime;	//?초간 살아있는다.
	float mStartGravity;
	float mEndGravity;
	Vector3f mPowerNormal;	//힘의 방향.
	float mTimeSpeed;
	BYTE mATrack[6];	//6개의 고정 트랙.
	BYTE mRTrack[6];	//6개의 고정 트랙.
	BYTE mGTrack[6];	//6개의 고정 트랙.
	BYTE mBTrack[6];	//6개의 고정 트랙.
	float mScaleTrack[6];	//6개의 고정 트랙.
}_PRE_PARTICLE_LIST;


//이 파티클은 파일로 저장된다.
#define _PARTICLE_SPARE_SIZE		127
typedef struct {
	char mEntityName[128];	//Attach된 엔티티 이름.	
	DWORD mFlag;		//파티클의 속성... 
	DWORD mNum;			//파티클의 갯수... 
	DWORD mAlphaType;		//알파타잎... 
	LONG mCreateArea[3];	//	구 영역의 경우..[0]번 배열에 반지름이...있다. 박스는 x,y,z 
	float mLiveTime;	//?초간 살아있는다.
	float mGravity[3];			//중력.
	Vector3f mPowerNormal;	//힘의 방향.
	float mTimeSpeed;
	BYTE mATrack[6];	//6개의 고정 트랙.
	BYTE mRTrack[6];	//6개의 고정 트랙.
	BYTE mGTrack[6];	//6개의 고정 트랙.
	BYTE mBTrack[6];	//6개의 고정 트랙.
	float mScaleTrack[6];	//6개의 고정 트랙.
	DWORD spare[_PARTICLE_SPARE_SIZE];
}_SAVE_PARTICLE;		

int GetPreParticleListNum();	//미리 수치가 들어있는 파티클 리스트 갯수 
_PRE_PARTICLE_LIST *GetPreParticleList(int id);	//해당 아이디의 포인터 리턴



//-------------------------------------------------------------------------------

#define _PARTICLE_ATTR_NO_LOOP			0x00000001	//루프가 안되는 경우. 
#define _PARTICLE_ATTR_ZWRITE			0x00000002	// z write를 하는거.. no zwrite가 기본이다.
//#define _PARTICLE_ATTR_ZSORT			0x00000004	//파티클이 소팅이 반드시 필요한경우.
#define _PARTICLE_ATTR_ZDISABLE			0x00000004	//Z 비교를 아에 안한다.
#define _PARTICLE_ATTR_NO_BILLBOARD		0x00000008	//파티클이 빌보드 스타일이 아니다.
#define _PARTICLE_ATTR_COLLISION		0x00000010	//파티클이 충돌검사가 필요한경우.
#define _PARTICLE_ATTR_ALWAYS_LIVE		0x00000020	//항상 살아있음.
#define _PARTICLE_ATTR_ENTITY_ANI		0x00000040	//엔티티의 애니메이션을 사용.
#define _PARTICLE_ATTR_NO_AIR			0x00000080	//공기감쇠 무시.
#define _PARTICLE_ATTR_END				0x00000800	//출력 끝.
#define _PARTICLE_ATTR_Y_BILLBOARD		0x00000400	//파티클이 Y 빌보드.
#define _PARTICLE_ATTR_Z_BILLBOARD		0x00000200	//파티클이 Z 빌보드.
#define _PARTICLE_ATTR_Z_ROT			0x00000100	//파티클이 Z회전이 있는경우.
#define _PARTICLE_ATTR_FREE				0x00008000	//파티클 메트릭스 종속이 아닌 독립인경우.
#define _PARTICLE_ATTR_ZFRONT			0x00004000	//파티클 z값을 조정할필요가 있는경우.
#define _PARTICLE_ATTR_EMIT_TIME		0x00002000	//실재 방사할 시간 조정.
#define _PARTICLE_ATTR_Y_ROT			0x00001000	//파티클이 y회전이 있는경우.
#define _PARTICLE_ATTR_BOUNDED_BOX		0x00080000	//파티클을 박스영역에 가둔다.
#define _PARTICLE_ATTR_SPECIAL_ID		0x00040000	//특별한 지정 파티클이다. 모든 파티클은 특별파티클 아이디에 따라 움직인다.
#define _PARTICLE_ATTR_FLICKER			0x00020000	//시작부터 깜박인다.

#define _PARTICLE_ATTR_IS_NOT_BILLBOARD	0x00000608	// 빌보드가 아닌경우..


#define _PARTICLE_ATTR_SPHERE			0x80000000	//구 스타일의 시작점
#define _PARTICLE_ATTR_SPHERE_EDGE		0x40000000	//구 테두리 시작점.


#define _ELEMENT_ATTR_FLICKER			0x01

#pragma	pack(push, 1)		//바이트 정렬.
typedef struct{
//	float mVelosity[3];	//현 파티클의 각각 속도.
	float mDirStep[3];	//현 파티클 스텝.
	float mDir[3];		//현 파티클 방향.
	float mPos[3];
	float mTime;		//개개인의 현재시간.

	float mARGBStep[4];	//필요한것들.
	float mARGB[4];
	float mScaleStep;
	float mScale;
	float mZRotStep;
	float mZRot;
	float mYRotStep;
	float mYRot;

	BYTE mFlag;			//요소 플래그.
	BYTE mIsLive;
	WORD mNowTrack;		//다음 진행될 트랙아이디 처음은 0을 카르킨다.
	float mNowFrame;	//애니메이션되는거...

	void UpDate(float time);//,float mTimeSpeed);
}_PARTICLE_ELEMENT;
#pragma	pack(pop)

#define _PARTICLE_STATE_STOP				0	//파티클 정지 단 현재있는 파티클은 사라질때까지 존재한다.
#define _PARTICLE_STATE_START				1	//파티클의 시작을 알린다.
#define _PARTICLE_STATE_START_ONE_ELEMENT	2	//강제로 요소하나를 내보낸다. 단 현재 돌리고있는 전체 숫자보다 적을경우 가장오랜된 요소를 없애고 생성한다.
#define _PARTICLE_STATE_ALWAYS_LIVE			3	//파티클이 항상 살아있게하려면 타임값들을 재설정해줘야 된다.

#define _PATICLE_MAX_TRACK	12

#define _TRACK_FLAG_ALPHA		0x80
#define _TRACK_FLAG_COLOR		0x40
#define _TRACK_FLAG_SCALE		0x20
#define _TRACK_FLAG_POWER		0x10
#define _TRACK_FLAG_ZROT		0x08
#define _TRACK_FLAG_YROT		0x04
#define _TRACK_FLAG_FLICKER		0x02


class CParticle{
public:
	char mEntityName[128];		//이 파티클이 어떤 엔티티를 소유하는가...	//
	LONG mNum;					//파티클 엘러먼트 총 갯수
	_PARTICLE_ELEMENT *mElement;		//파티클 하나하나 요소

	CParticle();
	~CParticle();

	void SetCreatePos(Vector3f pos)		{	Vector3fCopy(pos,mCreatePos);	};
	LONG Loop();			//파티클의 해당 루프... 리턴값은 파티클이 살아있는 갯수다.
	LONG RealLoop();		//실제 파티클의 계산루프.리턴값은 파티클이 살아있는 갯수다.
	LONG SpecialLoop();		//특수 파티클의 해당 루프... 리턴값이 -1인경우 원래루프를 쓴다.
	LONG SpecialLoop2();		//특수 파티클의 해당 루프... 리턴값이 -1인경우 원래루프를 쓴다.
	void SetParticleState(DWORD state);	// 시작,끝,강제 출력을 지정.
	DWORD GetParticleState();			// 시작,끝,강제 상태를 알아봄
	
	void SetStartBoxArea();	//시작박스안에서 벗어난 파티클을 안으로 옮긴다. //비나 눈처럼 환경에서 쓰여질때

	//저장 파티클과 상태 파티클과의 복사.
	void CopyParticleToSaveParticle(_SAVE_PARTICLE *s_particle);
	void CopySaveParticleToParticle(_SAVE_PARTICLE *s_particle);

	void SetParticleAttr(DWORD attr)	{	mFlag=attr;		};			//파티클 플래그다.
	DWORD GetParticleAttr()				{	return mFlag;	};			//파티클 플래그다.
	void SetPreCalcParticle(DWORD type = _PARTICLE_TYPE_FALL);	//사전에 반드시 상태변수들을 셋팅해야된다.

	BOOL LoadParticleSPT(char *name,DWORD option=0);
	void InitParticle();//파티클 요소의 갯수 초기화 mNum값으로 초기화 된다.
	void ReInitParticle(int num);//최초  개수를 재할당 시킨다.  매루프마다는 금지...(malloc이 있다.)
	void ReleaseParticle();//파티클 릴리즈 

	CEntity *mEntity;	//파티클이 소유하고있는 엔티티
	void ReleaseEntity();	//클론들은 릴리즈하면 큰일 난다.  클론 들은  릴리즈하지 않는다.
	void GetBBox(Vector3f get_min,Vector3f get_max);			//해당파티클 그룹의 바운딩 박스를 계산한다.
	void GetFlickerARGB(int i,DWORD *dw_argb);	//플리커가 있다면 컬러값을 구한다.

public:	//직접 건들어 버리자... 
	float mLiveTime;			// 살아있는 시간 초단위
	float mStartTimeRange;		// 0일때 라이브타임에 맞게 나오고 1에 가까울수록 파티클이 한꺼번에 나온다.
	float mTimeSpeed;			//시간 가속.
	float mGravity[3];			//중력.
	float mStartPower[2][3];			//시작점에서 파티클이 움직이는 힘의 벡터.
	float mStartScale[2];		//시작점 스케일 실제 랜덤은 
	float mStartZRot[2];		//시작 Zrot값 
	float mStartYRot[2];		//시작 Yrot값 
	BYTE  mFlickerAlpha;		//플리커할때 최저값.
	float mFlickerTime;			//플리커 깜박임시간.
	float mStartARGB[4][2];		//시작점 ARGB
	float mOnePerTimeEpsilon;	//1개 생성 될때 오차범위... 값이 0이면 오차가없다.  초단위 초기 백업용
	float mRotMat[4][4];		//이파티클 전체 회전 매트릭스다.
	union{
	float mElasticity;			// 탄성력.... 충돌시 부딪힐경우 반발력이다.
	float mTexRepeat;			// 텍스쳐 리피트수치... 충돌때나 보므로..
	};
	float mZFront;				//화면앞으로 뿌려줘야할때 z 값의 앞섬테스트... 충돌시 부딪힐경우 반발력이다.
	float mEmitTime;			//방사하는 실제 시간.

//	float mAlphaGradient;		//알파클라디언트 0-1까지 0이면 시작부터 사라지기 시작...
	WORD mSpecialID;			//특수아이디다.

	WORD mTrackCnt;	//트랙이 있는지.
	float mTimeTrack[_PATICLE_MAX_TRACK];	//x개의 고정 트랙.
	BYTE mTrackFlag[_PATICLE_MAX_TRACK];	//각각 트랙의 어떤종류들이 있는가 플래그.
	BYTE mATrack[_PATICLE_MAX_TRACK];		//x개의 고정 트랙.
	BYTE mRTrack[_PATICLE_MAX_TRACK][2];	//x개의 고정 트랙.
	BYTE mGTrack[_PATICLE_MAX_TRACK][2];	//x개의 고정 트랙.
	BYTE mBTrack[_PATICLE_MAX_TRACK][2];	//x개의 고정 트랙.
	float mScaleTrack[_PATICLE_MAX_TRACK][2];	//x개의 고정 트랙.
	float mZRotTrack[_PATICLE_MAX_TRACK][2];			// x개의 고정 트랙. Zrot값 
	float mYRotTrack[_PATICLE_MAX_TRACK][2];			// x개의 고정 트랙. Zrot값 
	Vector3f mPowerTrack[_PATICLE_MAX_TRACK][2];	//x개의 고정 트랙. 힘의 방향.
	Vector3f mSpecialARGV[2];						//특별 인자.

	void *mBsp;						//bsp포인터..
	void SetBspPtr(void *bsp)	{		mBsp = bsp;	};	//충돌검사에쓸 bsp 포인터를 셋팅.
	DWORD mFlag;					//파티클의 속성... 
	DWORD mAlphaType;				//파티클 알파 타잎.. 
//	LONG mCreateArea[3];			//	구 영역의 경우..[0]번 배열에 반지름이...있다. 박스는 x,y,z 지금은 박스만 사용.
	float mStartPos[2][3];			// Vector3f  파티클 시작 위치범위 렌덤범위는 항상[2]배열을쓴다.
	float mCreatePos[3];			// Vector3f  파티클 시작 위치 mCreatePos+mStartPos = world위치.
	void ResetOnePerTime();			//하나당 생기는 시간 갱신...

private:
	BOOL mState;					//파티클이 시작및 기타 상태?
	
	float mOnePerTime;				//파티클 1개 생성되는 시간...
	float mOnePerTimeEpsilonTemp;	//1개 생성 될때 오차범위...mOnePerTimeEpsilon값을 내부에서 쓰는 수치..
	float mParticleTimer;			//파티클 타이머.
	float mNextCreatorTime;			//다음 생성 타임.

//	float mMass;					// 파티클 질량 

	void CheckCollision(int id,float time);	//파티클의 다음 좌표에 해당하는 스텝..
	void GetPartcleStep(int id,float time);	//파티클의 다음 좌표에 해당하는 스텝..
//	void GetPartcleAbsoluteTime(int id,float time);	//파티클의 절대타임에 대한좌표 구한다.
	void InitElement(int id,float time); //엘리먼트 초기화

};

float GetGravity( float num );     // 0은 최 정점	-는 올라간다 +는 내려간다.

//Get Snake Vertex list	얻어지는 값은 strip 버텍스 리스트 이다.  꼬이는 궤적라인 그릴때 딱이다..
DWORD CalcSnakeVertexList(_D3DR3VERTEX_TEX1 *get_v,Vector3f *org_v,DWORD cnt,float f_length,DWORD color);


/*
	%주의    파티클 함수 호출 순서및 사용법...
	파티클 클래스는 뿌리는 루틴은 없다... 단지 좌표생성및 갯수등을 나타낸다.

	CParticle	test;
	test.SetParticleAttr(_PARTICLE_TYPE_FALL);
	test.InitParticle();
	test.SetParticleState(_PARTICLE_STATE_START);
	while(1)
	{
		-beginRender-
		test.Loop();	
		renderParticle();
		-endRender-
	}
	test.ReleaseParticle();
  */



#endif
