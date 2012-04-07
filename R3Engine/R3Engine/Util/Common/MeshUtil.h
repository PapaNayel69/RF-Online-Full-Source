#ifndef __MESHUTIL_H__
#define __MESHUTIL_H__


void SetWorldMesh(_MESH *mesh,_MATERIAL *material);
_MESH *ConvMeshDX8Optimize(_MESH *mesh,_MATERIAL *material);	//머터리얼 블럭형성을 위한 새로운 

_MESH *ConvMeshTriToPoly(_MESH *mesh);	//통짜 오브젝트의 삼각형 메쉬를 폴리곤 메쉬로 변환
void SetMeshFaceNormal(_MESH *mesh,_MESH *org_mesh);
_MATERIAL *ConvOneMaterial(_MATERIAL *material);	//오리지날 머터리얼을 넣으면 통짜 머터리얼이 나온다.
void AdjustMeshMaterial(_MESH *mesh,_MATERIAL *material);	//안쓰는 머터리얼과 같이쓰는 텍스쳐를 정리...
void CalculateMeshSmooth(_MESH *mesh); // 스무스그룹을 계산한다.
void GetInvScaleMatrix(_MESH *mesh);	//인버스 스케일 메트릭스를 구한다.
void SetLocalCoodinate(_OBJECT *object);	//로컬좌표로 바꾼다.
void SetWorldCoodinate(_OBJECT *object);	//월드좌표로 바꾼다.

DWORD GetFaceNumFromMesh(_MESH *mesh);	//메쉬로부터 면갯수를 얻어낸다.
_MESH *ConvOneMesh(_MESH *mesh,_MATERIAL *material);	//하나의 오브젝트로 컨버트,머터리얼은 오리지날...
_MATERIAL *MakeVolumeFogObject(_MESH *mesh,_MATERIAL *mat);	//볼륨 포그영역의 폴리곤들을 생성한다.

void GetCompositeMat(_MATERIAL *one_mat,_MATERIAL **composite_mat);	// 머터리얼에서 composite머터리얼과 그냥 머터리얼을  추출해낸다...
void SetMeshCompositeMat(_MESH *mesh, _MATERIAL *org_mat);	//Composite오브젝트의 경우. 총 개수와 오브젝트에 속성을 넣어준다.

#endif