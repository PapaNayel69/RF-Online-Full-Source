#ifndef __ANIMATION_H__
#define __ANIMATION_H__

//#include "R3math.h"

#pragma	pack(push, 1)		//바이트 정렬.

typedef struct{
	float frame;		//프래임 번호 ( 예:12.1 프래임)
	Vector3f pos;
}_POS_TRACK;			//위치트랙

typedef struct{
	float frame;		//프래임 번호 ( 예:12.1 프래임)
	Vector4f quat;		//쿼터니온
}_ROT_TRACK;			//회전 트랙

typedef struct{
	float frame;		//프래임 번호 ( 예:12.1 프래임)
	Vector3f scale;
	Vector4f scale_axis;	//스케일축 쿼터니온.
}_SCALE_TRACK;			//스케일트랙

//-------오브젝트 타잎 속성.	word형으로 해놓은다.
//		0x000f는 이벤트 애니에서 스테이트 값으로 쓴다.bsp클래스에 있다.
#define _DYNAMIC_OBJECT		0x8000
#define _NO_MERGE_OBJECT	0x4000
#define _EVENT_ANI			0x2000		//이벤트 애니메이션일경우.
#define _DEFAULT_ANI		0x1000		//평범한 애니메이션일 경우.

typedef struct {
	WORD	flag;			// 오브젝트 플래그?
	WORD	parent;			// 0은 부모가 없다. parent-1 = index
	int frames;				//해당 오브젝트의 프래임수
	int Pos_cnt;			// 위치 트랙 갯수 
	int Rot_cnt;			// 로테이트 트랙 갯수
	int Scale_cnt;			// 스케일 트랙 갯수 

	Vector3f scale;		//스케일 애니가 없을 경우 이동값 
	Vector4f scale_quat;		//스케일 축 쿼터니온.
	Vector3f pos;		//이동애니가 없을 경우 이동값 
	Vector4f quat;		//회전애니가 없을 경우 쿼터니온 값 
	DWORD pos_offset;
	DWORD rot_offset;
	DWORD scale_offset;
}_READ_ANI_OBJECT;

typedef struct{
	char ObjectName[64];	//오브젝트 이름
	char ParentName[64];	//부모 이름 
	WORD flag;				// 루프되는가?
	WORD parent;			// 0은 부모가 없다. parent-1 = index
	int frames;				//해당 오브젝트의 프래임수
	int Pos_cnt;			// 위치 트랙 갯수 
	int Rot_cnt;			// 로테이트 트랙 갯수
	int Scale_cnt;			// 스케일 트랙 갯수 

	Vector3f scale;			//스케일 애니가 없을 경우 이동값 
	Vector4f scale_quat;	//스케일 축 쿼터니온.
	Vector3f pos;			//이동애니가 없을 경우 이동값 
	Vector4f quat;			//회전애니가 없을 경우 쿼터니온 값 
	_SCALE_TRACK *Scale;	// 위치 트랙		x,y,z					하위갯수[4]
	_POS_TRACK *Pos;		// 위치 트랙		x,y,z					하위갯수[4]
	_ROT_TRACK *Rot;		// 로테이트 트랙	x,y,z,w 의 쿼터니온		하위갯수[5]

	//이것들은 실제 안에서 동적으로 해주어야할것들이다.
	BYTE AniFrameCache;		//애니 프래임 연산은 한번만....
	float s_matrix[4][4];   //scale local matrix
	float now_frame;		//현재 프래임
    float f_matrix[4][4];   //최종 매트릭스
}_ANI_OBJECT;

#pragma	pack(pop)


void GetObjectMatrix(float w_matrix[4][4],WORD id,_ANI_OBJECT *ani_object,float time);
void GetAniMatrix(float ani_mat[4][4],_ANI_OBJECT *obj,float now_frame);
void ConvAniObject(int num,BYTE *track,_READ_ANI_OBJECT *r_object,_ANI_OBJECT *object);		//트랙인덱스를 포인터로 바꿔주자.

#endif
