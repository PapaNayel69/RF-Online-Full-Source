#ifndef __PLAYERSPECIALEFFECT_H__
#define __PLAYERSPECIALEFFECT_H__

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../BaseHeader.h"
#include "../Global.h"
#include "DummyDefine.h"

#define _CM_BIO_	0	//캐릭터 머터리얼 생체 
#define _CM_METAL_	1	//캐릭터 머터리얼 금속


void CPlayerSpecialEffectCreateDummy(void *player,CHARACTEROBJECT *co);
void CPlayerSpecialEffectFrameMove(void *player);	//플레이어 특별한 효과 이벤트 처리함수. 궤적이라든지..
void CPlayerSpecialEffectRender(void *player);	//플레이어 특별한 효과 이벤트 처리함수렌더링 부분 궤적이라든지..
void CPlayerSpecialEffectSecAction(void *player);	//플레이어 액션처리 특별한 함수.

void CUnitSpecialEffectSecAction(CUnitProc *unit);	//유닛 액션처리 특별한 함수.
void CUnitSpecialEffectFrameMove(CUnitProc *unit);

#endif
