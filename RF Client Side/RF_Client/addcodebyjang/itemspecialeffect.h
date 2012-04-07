#ifndef __ITEMSPECIALEFFECT_H__
#define __ITEMSPECIALEFFECT_H__

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../BaseHeader.h"
#include "../Global.h"
#include "DummyDefine.h"


void CItemSpecialEffectFrameMove(void* item,DWORD unique_id);	//아이템은 프래임무브만 있다.
void CItemSpecialEffectCreateDummy( void* item, CHARACTEROBJECT *co );	//장언일 추가 코드 무기의경우 궤적이 있기때문에 미리 준비를 해줘야된다.


#endif
