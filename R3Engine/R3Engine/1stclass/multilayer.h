#ifndef __MULTILAYER_H__
#define __MULTILAYER_H__

#include "windows.h"
#include "R3Material.h"
#include "R3Render.h"

#define _NONE_MATRIX_FLAG (_UV_ENV|_ANI_TEXTURE)	//매트릭스로 계산못하는 것들
#define _FLICKER_FLAG (_ANI_ALPHA_FLICKER)
#define _MATRIX_FLAG (_UV_METAL|_UV_SCROLL_U|_UV_SCROLL_V|_UV_ROTATE|_UV_SCALE)	//매트릭스로 계산들
//#define _NONE_TEXTURE_MATRIX_FLAG (_MATRIX_FLAG | _ANI_TEXTURE|_ANI_ALPHA_FLICKER|_UV_GRADIENT_ALPHA_UV|_MAT_VOLUME_FOG)
#define _NEED_LOCK_FLAG (_UV_LAVA)	//버텍스 버퍼 락이 필요한 경우.

void SetMultiLayerTime(float ftime);
void SetMultiLayerCamera(Vector3f camera);
float GetMultiLayerTime();		//멀티레이어에 쓰이는 타임.

int GetTileAniTextureAddId(_R3MATERIAL *mat,int l_id,float start_time=0);	//타일애니텍스쳐추가 아이디를 얻어온다.

//-맵용
void MakeUV(_D3DR3VERTEX_TEX2 *tex2,int v_num,void *source_uv,void *source_st,_R3MATERIAL *mat,int l_id,float start_time=0);
void MakeUVExt(_D3DR3VERTEX_TEX2 *tex2,int v_num,void *source_uv,void *source_st,_R3MATERIAL *mat,int l_id,float start_time=0);
//-스카이박스용
void MakeUV(_D3DR3VERTEX_TEX1 *tex1,int v_num,void *source_uv,_R3MATERIAL *mat,int l_id);
BOOL GetTextureMatrix(_R3MATERIAL *mat,int l_id,D3DXMATRIX *uv_matrix,float start_time=0);


#endif