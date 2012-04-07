#ifndef __R3BSPTOOL_H__
#define __R3BSPTOOL_H__

#include "world.h"
#include "r3blockMat.h"

#pragma	pack(push, 1)		//바이트 정렬.


#define	_MAX_ONE_FACE_VERTEX	16
typedef struct bspface{
	LONG v_num;			//버텍스 수
	LONG v_list[_MAX_ONE_FACE_VERTEX];
	LONG org_facenum;	//원래 메쉬의 페이스번호
	bspface *before;
	bspface *next;
}_TOOL_BSP_FACE;			//자를 면의 구조체...

typedef struct {
	Vector4f plane;		//노드를 가르는 평면 방정식
	DWORD f_normal_id;		//	노드를 가르는 평면 방정식의 id
	LONG front;
	LONG back;
	Vector3f bb_min,bb_max;	//노드의 바운딩 박스 최종에서는 10%정도 더크게...
	LONG face_num;			//노드의 폴리곤수
	_TOOL_BSP_FACE *bsp_face;	//노드에서 시작되는 face ; 나누어질때마다 다르다.
}_TOOL_NODE;

typedef struct {
	BYTE type;	//노드의 특성. water라든지...
	WORD face_num;			//bsp 페이스 갯수
	_TOOL_BSP_FACE *face;			//시작bsp face 번호
	Vector3f bb_min,bb_max;	//노드의 바운딩 박스
}_TOOL_LEAF;
#pragma	pack(pop)


void MakeBSPTree(_MESH *mesh);	//물론 통짜다각형 메쉬다.
void BspPrint(void);	//bsp프린팅
void DrawBspLeafBBox(void);		//bsp leaf 바운딩 박스 표시 
void DrawBspSplitPolygon(void);		//bsp leaf 폴리곤 표시 
void BspExport(_MESH *c_mesh,_MESH *m_mesh,char *name,R3BlockBsp *BspExport);

#endif
