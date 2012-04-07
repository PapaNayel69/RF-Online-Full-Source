#ifndef _CMAGICSCRIPT_H__
#define _CMAGICSCRIPT_H__

#include <windows.h>
#include <stdio.h>
#include <jerror.h>
#include <jmalloc.h>
#include "Entity.h"

#define _MAX_MAGIC_TRACK 16
#define _SPECIAL_EFFECT_FLAG	0x8000000	//지진이나 폭풍등 기타 다른 효과들...

//MAGIC TRACK FLAG
#define _MT_FLAG_Y_RANDOM		0x400	//y회전 렌덤 적용.
#define _MT_FLAG_Z_DISABLE		0x200	//z버퍼 안쓸때.
#define _MT_FLAG_MAGIC_LIGHT	0x100	//마법진인가? 마법진의 경우 다이나믹 라이트 속성을 가진다.
#define _MT_FLAG_QUAKE			0x80
#define _MT_FLAG_BILLBOARD		0x40
#define _MT_FLAG_PARTICLE		0x20	//파티클을 가지고 있는 트랙이다.
#define _MT_FLAG_DYNAMIC_LIGHT	0x10	//다이나믹 라이트쥐..
#define _MT_FLAG_SKY_FADE		0x08	//하늘 페이드인..
#define _MT_FLAG_TEX_FADE		0x04	//텍스쳐 페이드인..
#define _MT_FLAG_ENTITY_FADE	0x02	//엔티티 페이드인..
#define _MT_FLAG_CHECK_DAMAGE	0x01	//충돌 첵크 해야될 타이밍시...이펙트를 반환하기위한..

#pragma	pack(push, 1)		//바이트 정렬.


typedef struct{
	WORD Flag;				// 플래그.
	float Time;
	Vector3f Pos;			// 그트랙의 위치.
	union{
		union{
		float Yangle;		//y각도.
		WORD Yrand[2];
		};
	float QuakeTime;		// 지진 시간...
	float SkyFadeTime;		// 이시간동안 하늘이 어두웠다 밝아진다.
	float TexFadeTime;		// 이시간동안 화면에 텍스쳐를 보여줬다 다시 원상태로된다.
	float Rand1[1];
	};
	union{
	float AlphaDisapear;	//인버스니까 조심하길.. 전체 프래임에서 알파 사라지는 시점을 정한다. 0-1까지. 0은 완전 끝에서 사라짐.
	float QuakeDensity;		//지진강도.
	};
	float AlphaApear;		//인버스니까 조심하길.. 전체 프래임에서 알파 나타나는 시점을 정한다. 0-1까지. 0은 완전 뿅하고 나타남.
	DWORD EntityID;
	union{
	float Scale;
	float Rand2[1];
	};
	DWORD Color;
	DWORD DynamicLight;		//동적라이팅...LRGB  Length,Red,Green,Blue  용량 땜에.^^
}_MAGIC_TRACK;

#pragma	pack(pop)


//마법진행 클래스 
class CProcessMagic{
public:
	WORD mFlag;			//마법 플래그...
	BYTE mState;		//상태
	BYTE mCreateType;	// 매직발생 형태.
	BOOL mIsLoaded;		//파일로딩 되었나?
	DWORD mTrackNum;	//트랙 갯수.
	float mSpeed;		//원거리 공격 일때 스피드다.
	Vector3f mCreatePos;		//시작 상대 위치닷.
	_MAGIC_TRACK mMagicTrack[_MAX_MAGIC_TRACK];	//실제 트랙.

	BOOL IsLoaded(){ return mIsLoaded; };
	void CreateMagicTrack(char *name);
	void DestoryMagicTrack();
	// 리턴값이 -1인경우 실행할 엔티티아이디가 없다. 
	// -2인경우 앞으로의 트랙이 없어서 마법을 멈춰야한다.
	// 그외에 엔티티의 갯수가 리턴된다. 
// 배열 값은 충분이 잡아서 넣어주길...
//	LONG GetEntityIDs(float old_frame,float now_frame,float *get_start_frame,_MAGIC_TRACK **get_magic_track);
	LONG GetEntityIDs(float old_frame,float now_frame,LONG *start_loop);
};

DWORD GetRealIDFromMagicSpt(DWORD id);	//지정된아이디의 실제 인덱스를 리턴.
DWORD GetRealIDFromMagicSptSkipWarning(DWORD id);	//지정된아이디의 실제 인덱스를 리턴.경고가 안뜬다.

DWORD GetIDFromRealID(DWORD id);		//실제 인덱스의 지정된아이디를 리턴.
DWORD GetIDFromMagicSpt(char *name);	//지정된 마법이름으로 아이디를 리턴.

//매직 아이디가 존재하는지 여부
BOOL IsExistFromMagicSpt(DWORD id);	//지정된아이디가 매직리스트에 있는가..

char *GetFilenameFromMagicSpt(DWORD id);	//지정된아이디의 실제 이름을 리턴.
void CreateMagicSptList(char *name);
void DestroyMagicSptList();
float GetRandNum(float a,float b);

DWORD GetMagicSptNum();
DWORD GetAllMemorySizeMagic();	//현재 대략 사용되는 매직 메모리 수. 


#endif