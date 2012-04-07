
#ifndef __CHARACTEROBJ__
#define __CHARACTEROBJ__

#ifdef RTMOVIE

//#include "Stdafx.h"
//#include "Object.h"
#include "ParticleList.h"
#include "r3math.h"
#include "Character.h"
#include "Entity.h"
#include "RenderParticle.h"

void DrawEffect(CHARACTEROBJECT *obj,Vector3f pos,float g_Rot);
//void DrawEffect(CHARACTEROBJECT *obj,float mat[4][4]);
void DrawEffectMat(CHARACTEROBJECT *pObj,D3DXMATRIX *w_matrix,DWORD color);


#endif

#endif