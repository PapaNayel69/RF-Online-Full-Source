#ifndef __MOVEEFFECTMANAGER_H__
#define __MOVEEFFECTMANAGER_H__

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../BaseHeader.h"
#include "../global.h"


#define _MEM_FLAG_STOP

#define _MEM_MS_STOP				0//멈춘상태 
#define _MEM_MS_TARGET_STOP			1// 타겟에 박혀 있는 상태에서 멈춤.(캐릭터에 화살을 박는다든지..)
#define _MEM_MS_TARGET_STOP_HIDE	2// (캐릭터에 화살을 박힌다음 사라진다.)
#define _MEM_MS_DIRECTION			3//한쪽으로 가는 단 방향성으로....
#define _MEM_MS_TRACE_TARGET		4//계속적으로 타겟을 바꾸어준다...
#define _MEM_MS_ATTACH_TARGET		5//타겟에 들러 붙어있다. 
#define _MEM_MS_ATTACH_SOURCE		6//소스에 들러 붙어있다. 
#define _MEM_MS_SOURCE_STOP			7//소스에 들러 붙어있다. 
#define _MEM_MS_SOURCE_STOP_HIDE	8//소스에 들러 붙어있다가 사라진다. 
#define _MEM_MS_FREE_DIRECTION		9//그냥 맵에 부딪힐때까지 멀리 날아간다. 
#define _MEM_MS_CURVE				10//커브로 날이가는 경우닷. 

#define _MEM_MS_FLAG_AUTO_DISPEAR				0x1//속성을 말한다.
#define _MEM_MS_FLAG_COLLISION_NO_RETURN		0x2//충돌시 리턴을 하지 않을때.


typedef struct{
	EffectInfo *e_info;
	DWORD unique_id;
	Vector3f pos;
	Vector3f tar;
	BYTE move_style;
	WORD flag;
	float speed;
	float attach_time;			//	다 도착하구나서  박혀있는 시간.
	DWORD attach_type_id;		//	부착 위치 아이디.
	DWORD src_mat;		//소스 머터리얼.. 생체..메탈 그런거.
	DWORD src_ID;		//소스 아이디 몹이나 캐릭일때 종족.
	DWORD tar_mat;		//타겟 머터리얼.. 생체..메탈 그런거.
	DWORD tar_ID;		//타겟 아이디 몹이나 캐릭일때 종족.
	DWORD bullet_ID_for_damage;		//원거리 공격시 데미지때 종류를 구분할필요가 있을때 
}_MOVE_EF_FUNC;		//함수 인자가 너무 많아서 구조체에다 넣는다...


void StartMoveEffectManager(_MOVE_EF_FUNC *move_ef_func);
void FrameMoveEffectManager();

#endif
