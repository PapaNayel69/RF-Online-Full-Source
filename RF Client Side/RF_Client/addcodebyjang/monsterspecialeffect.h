#ifndef __MONSTERSPECIALEFFECT_H__
#define __MONSTERSPECIALEFFECT_H__

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../BaseHeader.h"
#include "../Global.h"
#include "DummyDefine.h"


/*

class CWeaponEffectInfo{
public:
	BOOL mIsLoaded;	//로딩 되었는가?
	LONG mDummyIndex[_MAX_DUMMY_NUM];	//-1인경우는 한가지 인덱스만 가지고잇는경우다.
	DWORD mID;
	CEntity *mEntity;	//궤적용 엔티티...
};
*/

#define _M_BIO_			1	//머터리얼 생체 
#define _M_METAL_		2	//머터리얼 금속
#define _M_MUTANT_		3	//머터리얼 유전자형.
#define _M_BIO_METAL	4	//머터리얼 생체금속.
#define _M_SHELL_		5	//머터리얼 갑각류.
#define _M_SKIN			6	//가죽.
#define _M_PLASTIC		7	//합성수지.
#define _M_BEAM			8	//빔.


void CMonsterSpecialEffectFrameMove(void *monster);	//몬스터 관련 특별한 효과 이벤트 처리함수.
void CMonsterSpecialEffectSetAction(void *monster);	//액션이 변할때 효과넣는함수.

void CMonsterSpecialEffectDestroy(void *monster);	//몬스터 릴리즈시 초기화 실행.
void CMonsterSpecialEffectDamageAction(void *monster);	//몬스터 타격당할때 허리 꺽어주기..

void CMonsterDamageBackup(void *character);	//몬스터 관련 특별한 효과 이벤트 처리함수 프래임무브.

void CMonsterSpecialEffectFrameMove(void* monster,DWORD unique_id);	//몬스터에 붙어있는 효과 처리.
void CMonsterSpecialEffectCreateDummy( void* monster, CHARACTEROBJECT *co );	//몬스터 에 있을 부분을 미리 준비 해줘야된다.
void CMonsterSpecialEffectRender(void* monster);	//몬스터에 붙어있는 효과 처리.


#endif
