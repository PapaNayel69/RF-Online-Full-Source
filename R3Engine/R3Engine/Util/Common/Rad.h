#ifndef __RAD_H__
#define __RAD_H__

#include "world.h"

#define _MAX_INTENSITY	2.0f	//다시 2.0으로 간다..

void GetLightRadiosity(_MESH *mesh,_MATERIAL *mat, _LIGHTSOURCE *Lgt_mesh );	//라이트계산만
BOOL CalculateFaceRadiosity(_MESH *mesh);	//face Radiosity
float GetEnergyLimit( _LIGHTSOURCE *lgt_mesh );	//에너지 리미트를 구한다.

#endif