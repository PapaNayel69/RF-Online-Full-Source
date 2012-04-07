#ifndef __EFFEFTCORRE_H__
#define __EFFEFTCORRE_H__

#include <windows.h>
#include "C_vector.h"
#include "entity.h"
#include "CMagicScript.h"
#include "../global.h"

#define _MAX_MAGIC_ID_STACK 16	//화면 디스플레이용. 매직아이디스택갯수.





//이펙트 리스트
class CEffectList{
public:
	DWORD mState;		//상태다. _EF_STATE_AUTO_DISAPEAR일경우 자동으로 애니메이션에 따라 사라지게된다.
	DWORD mColor;		//컬러다.
	float mScale;		//스케일이다.
	float mMat[4][4];	//해당되는 회전과 포지션이 들어간 매트릭스다.
	float mDisapearFrame;	//사라지기 시작할때 프래임.
	float mFrame;
	union{
	float mAlphaDisapear;	// 전체 프래임에서 알파 사라지는 시점을 정한다. 0-1까지. 1은 완전 끝에서 사라짐.
	float mDisapearTime;
	};
	float mAlphaApear;		// 전체 프래임에서 알파 나타나는 시점을 정한다. 0-1까지. 1은 완전 끝에서 사라짐.
	DWORD mEffectID;	//이펙트 아이디다..
	DWORD mUniqueID;	//유니크 아이디다..
	DWORD mFlag;		//파티클인가?또는 빌보드인가.? 플레그는 매직스크립트를 따라간다. _MT_FLAG_PARTICLE

	DWORD mSpecialID;			//스페셜한 경우에 사용한다.
	Vector3f mVectorArgv1,mVectorArgv2;

	union{
	CEntity *mEntity;	//이거는 포인터로 해결하자...
	CParticle *mParticle;	//파티클이다.
	};
};
//마법 클래스는 
class CMagicList{
public:
	DWORD mState;		//상태다. _EF_STATE_AUTO_DISAPEAR일경우 자동으로 애니메이션에 따라 사라지게된다.
	DWORD mEntityUniqueIDCnt;	//마법 트랙이 가지고 있는 엔티티를 콘트롤하기위해.
	DWORD mEntityUniqueID[_MAX_MAGIC_TRACK];	//마법 트랙이 가지고 있는 엔티티를 콘트롤하기위해.
	float mMat[4][4];	//마법 시행 위치와 회전...
	float mOldFrame;	//이전 프레임을 기억하고 있어야 해당 프래임을 얻어온다.
	float mFrame;		//진행 프래임.
	DWORD mMagicID;		//이펙트 아이디다..
	DWORD mUniqueID;		//유닉 아이디다..
	void *mMagicPtr;	//마법 포인터..

	DWORD mCharTypeID;			// 캐릭터 타입
	DWORD mCharIndex;			// 서버 테이터 테이블의 레코드 인덱스

	DWORD mSpecialID;			//스페셜한 경우에 사용한다.
	Vector3f mVectorArgv1,mVectorArgv2;

	EffectInfo mEInfo;	//이펙트 정보 리턴할경우를 대비해서.
	// 그라운드 아이템때문에 일단 갔고있는거다.	나중에 없앨것.
	float mYOldAngle;		//y 각도 ..
	float mYAngle;		//y 각도 ..
	DWORD mYAngleCnt;		//y 각도 ..	
};

//현재 뿌리는 이펙트 상황을 알려구 한다면.
CEffectList* GetEffectList();
DWORD GetEffectListCnt();
CMagicList* GetMagicList();
DWORD GetMagicListCnt();
void DeleteMagicList(DWORD id);	//이거좀 주의 해야 한다.

void SpecialARGV1(Vector3f pos);		//특별한 인자를 내부로 전달할때호출한다.
void GetSpecialARGV1(Vector3f pos);	
void SpecialARGV2(Vector3f pos);		//특별한 인자를 내부로 전달할때호출한다.
void GetSpecialARGV2(Vector3f pos);	
BOOL SetSpecialIDFromMagic(DWORD unique_id,DWORD special_id);
DWORD GetSpecialIDFromMagic(DWORD unique_id);
BOOL CopySpecialIDFromMagic(DWORD unique_id);



//--------------------------------------------------------------------------
//현재 미완성된 부분은 엔티티부분의 캐쉬와 수동조작캐쉬수정 
//그리고 MoveMatrixMagicEffect()함수부분이 필요하다.
//--------------------------------------------------------------------------


//이펙트나 마법의 속성.
#define _EF_STATE_AUTO_DISAPEAR		0	//자동으로 사라지기 시작한다.(애니메이션에 비례한다.) 모든 이펙트는 디폴트로 이것을 쓴다.
#define _EF_STATE_ALWAYS_LIVE		1	//항상 살아 있는다.
#define _EF_STATE_START_DISAPEAR	2	//항상 살아있다가 이거쓰면 그때부터 사라지기 시작한다. 
#define _EF_STATE_DEATH				3	//잠시 죽여놓는거다. 
#define _EF_STATE_MYSELF_LIVE		4	//스스로 살아 있는경우. (스스로 살아 있으면서 움직여야되는경우.)

#define _GROUND_ITEM_FLAG	0xf0000000
//#define _TRACE_FLAG			0x10000000

//------------------------ 이펙트 기본조정 관련 함수.
/* 모든 이펙트와 매직은 함수한번 호출하기만 하면 소멸될때까지 자동으로 작동되는 원리다.
하지만 프래임마다 움직이게 되거나 위치가 바뀌는 경우( 화살등) 불특정으로 소멸이되는 경우는
리턴값인 DWORD값인 unique_id 를 리턴받아 수정된 좌표를 MovePosEffect함수를 사용해서 옮겨준다.
*/

DWORD GetNextUniqueID();	//  StartEffect의 unique_id를 넣을때 이함수 리턴값을 넣어준다.

//리턴값은  unique id값이다. 매프래임마다 좌표를 바꿀경우 unique id를 저장해놓고 좌표를 바꾼다.
void StartEffect(Vector3f pos,float y_angle, DWORD effect_id,DWORD state=_EF_STATE_AUTO_DISAPEAR
	,DWORD color=0xffffffff,float scale=1.0f,DWORD flag=0,float frame=0,DWORD unique_id=0);//단순 이펙트도 시작위치와 아이디면 이것도 끝.
void StartEffect(float mat[4][4], DWORD effect_id,DWORD state=_EF_STATE_AUTO_DISAPEAR
	,DWORD color=0xffffffff,float scale=1.0f,DWORD flag=0,float frame=0,DWORD unique_id=0);//단순 이펙트도 시작위치와 아이디면 이것도 끝.

//마법은 시작위치와 마법아이디로 시작을 알리면 모든게 끝. 매프래임마다 좌표를 바꿀경우 리턴값인 unique id를 저장
DWORD StartMagicEffect(Vector3f pos,float y_angle, DWORD magic_id,void *e_Info=NULL,DWORD state=_EF_STATE_AUTO_DISAPEAR);
DWORD StartMagicEffect(float mat[4][4], DWORD magic_id,void *e_Info=NULL,DWORD state=_EF_STATE_AUTO_DISAPEAR);

//캐릭터 아이디를 등록한다. 특별한 상황이면 나중에 없애기 위해.
BOOL SetAttachCharMagicEffect(DWORD unique_id,DWORD char_type,DWORD char_index);

BOOL DelMagicEffect(DWORD magic_id);//캐쉬상에 있는 마법을 지운다. Tool에서나 쓰는거..


void CreateEffectsList(char *filename);		//이펙트들 초기화.
void DestroyEffectsList();		//이펙트들 릴리즈
void CreateMagicSptList(char *name);	//마법스크립트 리스트초기화 
void DestroyMagicSptList();		//마법 스크립트 리스트 없애기

void FrameMoveAllEffectsList();	//프래임무브 파트
void RenderAllEffectsList();		//렌더파트

void DestroyEffectsCache();		//캐쉬상의 모든 이펙트들을 릴리즈 시킨다. 새로운 레벨로딩때 필요.
void ResetEffectManager();	//이펙트 메니져들을 리셋한다.


//--------------------- 이펙트 수동조정 관련 함수 ----------------
/*
BOOL SetMagicEffectState(DWORD magic_id,DWORD state);	//마법이펙트 상태 셋팅한다.
DWORD GetMagicEffectState(DWORD magic_id);	//마법이펙트 상태를 얻어온다.
*/

//BOOL SetMagicEffectState(DWORD magic_id,DWORD state);	//마법이펙트 상태 셋팅한다.

//마법 상태 조정 함수. 리턴값은 해당 unique_id가 없을 경우.
BOOL SetMagicState(DWORD unique_id,DWORD state);

//이펙트 상태 조정 함수. 리턴값은 해당 unique_id가 없을 경우.
BOOL SetEffectState(DWORD unique_id,DWORD state);

//수동으로 사라지기 시작할때 사용한다. 시간단위는 초...
BOOL SetDisapearTimeEffect(DWORD unique_id,float time);
BOOL SetDisapearTimeMagicEffect(DWORD unique_id,float time);

//특정 이펙트들을 움직일때 사용한다.  화살이나 기타 그런것들...
BOOL MoveMatrixEffect(DWORD unique_id,float mat[4][4]);	//지정된 유닉아이디의 이펙트를 수정한다.
BOOL MovePosEffect(DWORD unique_id,Vector3f pos,float y_angle=-1);	//지정된 유닉아이디의 이펙트를 수정한다.

BOOL MovePosMagicEffect(DWORD unique_id,Vector3f pos,float y_angle=-1);	//지정된 유닉아이디의 이펙트를 수정한다.
BOOL MoveMatrixMagicEffect(DWORD unique_id,float matrix[4][4]);	//지정된 유닉아이디의 이펙트를 수정한다.

BOOL GetMagicCTRLPoint012(DWORD unique_id,Vector3f *get_pos);	//베이어 커브 4점중 마지막(타겟) 점을 제외하고 넣어준다. 
_MAGIC_TRACK *GetMagicTrack(DWORD unique_id);
CProcessMagic *GetMagic(DWORD unique_id);
CMagicList* GetMagicListFromID(DWORD unique_id);
void SetCurveMagicTrackRandom(_MAGIC_TRACK *mtrack);	//현재 매직에서 쓰는 커브를 랜덤으로 사용한다.

DWORD *GetMagicIDList();

void SetBspPtr(void *bsp);

DWORD GetEffectCacheSize();			//현재 사용되고잇는 이펙트 캐쉬 사이즈를 알아낸다.
DWORD GetEffectCacheTotalNum();		//현재 사용되고잇는 이펙트 캐쉬 갯수를 알아낸다.
DWORD GetEffectCacheNum();			//현재 이펙트 캐쉬 총 갯수를 알아낸다.

DWORD GetMagicSptNum();
DWORD GetAllMemorySizeMagic();	//현재 대략 사용되는 매직 메모리 수. 


#endif