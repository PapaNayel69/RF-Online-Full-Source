#ifndef _BSPRENDERPART_H__
#define _BSPRENDERPART_H__

#include "world.h"
#include "R3bsp.h"
#include "R3BlockMat.h"

typedef struct {
	int MatObjNum;
	int BMaxVertexNum;
	int WMaxVertexNum;
	int FMaxVertexNum;
//---------------- 
	int BVertexNum;
	int WVertexNum;
	int FVertexNum;
	int FaceNum;
//	int FaceIdNum;	//페이스 갯수랑 같으므로...
	int VertexIdNum;
	Vector3c *BVertex;			//	byte 점
	Vector3s *WVertex;			//	word 점
	Vector3f *FVertex;		//	float점
	Vector2f *UV;			//	float uv 텍스쳐
	Vector2s *LGTUV;		//	short 라이트맵 uv
	_BSP_FACE *Face;		//	면
	DWORD *FaceId;		// 면 인덱스리스트	임시로 사용.... 나중에 최적화
	DWORD *VertexId;		// 면 인덱스리스트	임시로 사용.... 나중에 최적화
	DWORD *VertexColor;	//버텍스 컬러.
}BSPRenderPart;


BSPRenderPart *GetBSPRenderPart(void);
void CreateBSPRenderPart(_BSP_M_GROUP *bsp_m_group,_MESH *m_mesh,R3BlockBsp *block_bsp);
void ReleaseBSPRenderPart(void);


#endif