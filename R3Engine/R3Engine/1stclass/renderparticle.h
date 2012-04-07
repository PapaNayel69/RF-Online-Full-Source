#ifndef __RENDERPARTICLE_H__
#define __RENDERPARTICLE_H__

#include "R3Particle.h"
#include "entity.h"

void DrawCParticle(CParticle *particle,CEntity *entity,float world_mat[4][4],DWORD argb=0xffffffff);	//미리 버텍스버퍼에 저장해야된다.
//void DrawCParticle(CParticle *particle,CEntity *entity,Vector3f pos,DWORD argb=0xffffffff);	//미리 버텍스버퍼에 저장해야된다.

#endif