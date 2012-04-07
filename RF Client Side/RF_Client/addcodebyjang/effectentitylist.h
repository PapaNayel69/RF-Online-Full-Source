#ifndef __EFFECT_ENTITY_LIST_H__
#define __EFFECT_ENTITY_LIST_H__

#include <windows.h>
#include <stdio.h>
#include <jerror.h>
#include <jmalloc.h>
#include "R3Particle.h"
#include "Entity.h"

void LoadEffectEntityList(char *name);	//스크립트에서 엔티티리스트를 읽는다.
void ReleaseEffectEntityList();

DWORD GetRealID(DWORD id);	//지정된아이디의 실제 인덱스를 리턴.
CEntity *GetEntityFromEffectEntityList(DWORD real_id);
CParticle *GetParticleFromEffectEntityList(DWORD real_id,float mat[4][4]);
BOOL IsParticle(DWORD real_id);

void *GetFirstTexureFromEffectEntityList(DWORD entity_id);//엔티티 아이디를 인자로 넣으면..첫번째 텍스쳐를 얻어낸다.



DWORD GetEffectEntityNum();				//이펙트 엔티티 수.
DWORD GetAllEffectEntityTextureSize();	//현재 이펙트 엔티티에 사용되는 모든 텍스쳐수.
DWORD GetAllMemorySizeEffectEntity();	//현재 대략 사용되는 effectentitylist 메모리량	텍스쳐 제외
DWORD GetLoadedEffectEntityNum();		//로딩된 이펙트 엔티티 수.

#endif