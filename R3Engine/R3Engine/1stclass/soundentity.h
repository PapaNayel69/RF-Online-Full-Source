#ifndef __SOUNDENTITY_H__
#define __SOUNDENTITY_H__

#include "c_vector.h"
#include <windows.h>

#pragma	pack(push, 1)		//바이트 정렬.

typedef struct{
	WORD start_id;			//절대로 60000이상 가지말자.
	WORD entities_num;
}_LEAF_SOUND_ENTITIES_LIST_INFO;

typedef struct{
	char name[64];
}_READ_SOUND_ENTITY_LIST;

typedef struct{
	WORD ID;			//플레이에 쓸 아이디...
	char Name[62];
}_SOUND_ENTITY_LIST;

typedef struct{
	WORD id;			// 사운드 아이디.
	WORD event_time;	// 이벤트 타임.
	DWORD flag;			//사운드 플래그 
	float scale;		// 중심거리...
	float attn;			// 감쇠.
	Vector3f	pos;	//물론 위치지..
	Vector3f box_scale;
	float box_attn;
	float box_rot_x,box_rot_y;
	DWORD spare;		//만약을 위한 여분...
}_READ_SOUND_ENTITIES_LIST;	//파일로 읽는 루틴


typedef struct{
	WORD ID;			// 사운드 아이디.
	WORD EventTime;		// 사운드 렌덤타임.
	DWORD Flag;			//사운드 플래그 
	float Scale;		// 중심거리...
	float Attn;			// 감쇠.
	Vector3f	Pos;	//물론 위치지..
	Vector3f BoxScale;
	float BoxAttn;
	float BoxRotX,BoxRotY;
	float NextPlayTime;
	Vector3f BBMin,BBMax;
	float mInvMat[4][4];
	float GetVolume(Vector3f pos);
	float GetPan(Vector3f pos);
	float GetBoxIntensity(Vector3f pos);		//박스 영역에서의 볼륨을 알려준다. 0-1사이
}_SOUND_ENTITIES_LIST;		// 실제 메모리에서 가지고있을거.



#pragma	pack(pop)

#endif