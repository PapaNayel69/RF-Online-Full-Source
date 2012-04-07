#ifndef _R3VERTEXSHADER_H
#define _R3VERTEXSHADER_H

//--------------------------------------------------------------------------------------
//버텍스 쉐이더 레지스터 inupt 구조.   쉐이더에서 안쓰면 넣지 않는게 기본...
// c0-c3 매트릭스 1 
// c4-c7 매트릭스 2
// c8	포그
// c12- 버텍스 쉐이더의 상황에 따른 셋팅.

BOOL LoadVertexShaderList();
void SetWorldViewMatrixVS(float world_mat[4][4]);	//월드뷰매트릭스는 엔티티블록을 뿌릴때마다 새줘야하므로.
void SetGrassVS(float add_frame,float factor[2],float alpha,DWORD map_color);
void ReleaseVertexShaderList();


#endif