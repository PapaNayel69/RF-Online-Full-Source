#ifndef __PATCH_H__
#define __PATCH_H__

#include"world.h"

#define _NO_SPLIT	0
#define _SPLIT		1

#pragma	pack( push, 1 )	//사이즈가 많으니까...
typedef struct Winding{	//
	BYTE is_out_face;	//면밖의 와인딩인가.
	Vector3f normal;	//노멀
	Vector3f pos;		//위치
	Vector3uc energy;		//unshot의 총합(unshot은 한번 방사하고 나면 클리어한다.),실제 라이트맵
	Vector3uc temp_energy;	//다이나믹 라이트및쉐도우맵 블로를 위한...
//	float ff;			//폼 팩터 (면이 받은 강도:intensity)
//	Vector3f unshot;		//현재 방사하고있는(라이트,팻치)것 * 폼팩터 * reflectance.r ,g, b
}_WINDING;
#pragma pack(pop)

typedef struct{	//팻치는 3각 또는 4각이다. 다각형도 된다.
	int xl,yl;				//winding의 x,y 길이
	int mtlid;				//머터리얼 id
	Vector4f fnormal;	// face normal
	_WINDING *winding;

//--radiosity사용할경우 
//	float sqare;			//면적
//	Vector3f reflectance;		//물체(또는 팻치)의 반사하는 r,g,b 값
//	Vector3f emission;		//방사능력(라이트의 방사력) 라이트 R.G.B
}_PATCH;

typedef struct{
//	int friend_num;	//이웃폴리곤은 1개만, 사각형을 위해
//	int option;
	_PATCH *patch;
}_LFACE;


void MakePatchAndWinding(_MESH *mesh,int option=_NO_SPLIT);
_LFACE **GetLFace(void);	//계산된 라이트맵 페이스 구조체포인터를 리턴

void VectorScale(Vector3f s,Vector3f e,float t,Vector3f get);
void VectorSubtract(Vector3f sou1,Vector3f sou2,Vector3f tar);
float VectorLength(Vector3f sou1);
void VectorPlus(Vector3f sou1,Vector3f sou2,Vector3f tar);
void MakeVertexPatchAndWinding(_MESH *mesh);	// 통자메쉬를 인자로.... 스카이 박스를 위한것...

//int CheckEdge(_VERTEX *a,_VERTEX *b,_VECTOR *v,_FACE_NORMAL *fv);


#endif
