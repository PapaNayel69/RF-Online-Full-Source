#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "C_Vector.h"
#include "windows.h"

float GetYposInLeaf(Vector3f a,Vector3f b,float able_height,float now_height,int leaf_id);
// source와 target을 정확히 해줘야된다.(BBOX의 뒤집힌면은 첵크안한다.)
BOOL IsRayAABB(Vector3f BBmin,Vector3f BBmax,Vector3f source, Vector3f target, Vector3f *get);
BOOL IsRayAABB(Vector3s BBmin,Vector3s BBmax,Vector3f source, Vector3f target, Vector3f *get);

//양쪽으로 체크할수있는경우
BOOL IsBothRayAABB(Vector3f BBmin,Vector3f BBmax,Vector3f source, Vector3f target);
BOOL IsBothRayAABB(Vector3s BBmin,Vector3s BBmax,Vector3f source, Vector3f target);

//면과 바운딩 박스와의 충돌 여부..TURE면 충돌 ...
BOOL IsCollisionBBoxToFace(Vector4f p_normal[6],Vector3f a,Vector3f b,Vector3f c);
//바운딩 박스 끼리의 충돌 여부...
BOOL IsCollisionBBox(Vector3s a_min,Vector3s a_max,Vector3f b_min,Vector3f b_max);
BOOL IsCollisionBBox(Vector3f a_min,Vector3f a_max,Vector3f b_min,Vector3f b_max);
BOOL IsCollisionBBox(Vector3s a_min,Vector3s a_max,Vector3s b_min,Vector3s b_max);

//바운딩 박스안에 점이 있는가?
BOOL IsCollisionBBoxPoint(Vector3f a_min,Vector3f a_max,Vector3f pos);

//---low level 함수
DWORD GetPlaneCrossPoint(Vector3f a,Vector3f b,Vector3f cross,Vector3f f_normal,float d);	//-1이면 뒤집힌충돌 1이면 앞으로충돌.
BOOL GetPlaneCrossPoint(Vector3f a,Vector3f b,Vector3f cross,Vector3f f_normal,float d,float option);
inline int CheckEdge(Vector3f a,Vector3f b,Vector3f v,Vector3f f_normal,float d);
int CheckEdgeEpsilon(Vector3f a,Vector3f b,Vector3f v,Vector3f f_normal,float d);

#endif
