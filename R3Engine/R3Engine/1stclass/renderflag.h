#ifndef __RenderFlag_H__
#define __RenderFlag_H__

#include "r3d3d8.h"

//플래그 셋팅 함수..
//HRESULT R3SetRenderState(D3DRENDERSTATETYPE type, DWORD state);
//HRESULT R3SetTextureStageState(DWORD stage,D3DTEXTURESTAGESTATETYPE type, DWORD state);

void SaveRenderState();
void SaveTextureStage();




#endif