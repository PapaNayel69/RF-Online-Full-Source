#ifndef __R3LIGHTMAP_H__
#define __R3LIGHTMAP_H__

#include "c_vector.h"

typedef struct{
	WORD xl;
	WORD yl;
	WORD *buf;	//565포맷
}_LIGHTMAP;

void LoadLightMap(char *name);
void ReleaseLightMap();
void *GetLightMapSurface(int i);

DWORD GetLightMapTexSize();	//라이트맵 텍스쳐 사이즈를 알아낸다.

DWORD GetLightMapColor(Vector2f lgt_uv, int id);	//해당라이트맵아이디와 라이트맵 uv를 넣너주면 컬러값을 얻어온다.



#endif