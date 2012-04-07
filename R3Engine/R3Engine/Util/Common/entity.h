#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "world.h"
#include "r3blockmat.h"

#pragma	pack(push, 1)		//바이트 정렬.

#define _ENTITY_VERSION 113	//110과 111의 차이는 라이트 2배 밝기.

typedef struct{
	DWORD offset;		//옵셋.
	DWORD size;			//엔트리 사이즈.
}_ENTITY_ENTRY;

//파일 앞부분 8byte는 이것이 있다.
//	DWORD version;		//_R3BSP_VERSION
//	DWORD identity;		//스카이박스나 엔티티들은 항상 0이다.

typedef struct{
	_ENTITY_ENTRY CompHeader;	//	압축정보 헤더.
	_ENTITY_ENTRY Vertex;		//	float점
	_ENTITY_ENTRY VColor;		// vertex 컬러
	_ENTITY_ENTRY UV;			// uv좌표
	_ENTITY_ENTRY Face;		//	면
	_ENTITY_ENTRY FaceId;		// 면 인덱스리스트
	_ENTITY_ENTRY VertexId;	// 점 인덱스리스트
	_ENTITY_ENTRY MatGroup;	//	머터리얼 그룹..
	_ENTITY_ENTRY Object;	//	오브젝트.
	_ENTITY_ENTRY Track;	//	트랙.
}_ENTITY_FILE_HEADER;

typedef struct {
	WORD face_num;	//해당 면 갯수
	DWORD face_start_id;
	SHORT mtl_id;	//머터리얼 아이디
	WORD object_id;	//오브젝트 아이디... 다이나믹 오브젝트를 위한...
	Vector3s bb_min,bb_max;		//머터리얼 바운딩 박스
}_ENTITY_M_GROUP;

typedef struct{
	WORD v_num;		//해당 점 갯수
	DWORD v_start_id;	//점수...
}_ENTITY_FACE;

//엔티티 압축 헤더
typedef struct {
	WORD type;		//버텍스 압축타입 
	Vector3s bb_min,bb_max;		//머터리얼 바운딩 박스
	Vector3f pos;				//위치
	float scale;				//스케일값...
	//매핑좌표
	float uv_min,uv_max;
}_ENTITY_COMP_HEADER;

//------- 엔티티용 리스트



typedef struct {
}_ENTITY_LIST;


#pragma	pack(pop)

void EntityExport(char *name,_MESH *mesh,_MATERIAL *mat,R3BlockBsp *block_mat);
_D_OBJECT *AllocDObject(_MESH *m_mesh);
void FreeDObject(_D_OBJECT *object);
BYTE *AllocTrack(_MESH *m_mesh);
void FreeTrack(BYTE *track);
void StoreDObjectAndTrack(_D_OBJECT *object,BYTE *ani_track,_MESH *m_mesh);
int	GetDObjectNum(_MESH *m_mesh);
int GetTrackSize(_MESH *m_mesh);


#endif