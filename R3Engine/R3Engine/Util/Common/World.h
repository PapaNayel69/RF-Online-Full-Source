#ifndef __WORLD_H__
#define __WORLD_H__

#include "d3d.h"
#include "d3dmath.h"
//#include <d3dutil.h>
#include <windows.h>
#include "C_vector.h"

#define _PI 3.1415926535
#define _R3T_FILE_VERSION	1.1

#pragma	pack(push, 1)		//바이트 정렬.

typedef struct{
	union{
	float local[1];
	float x;
	};
	float y,z;
}_VERTEX;

typedef struct{
	union{
	float local[1];
	float x;
	};
	float y,z;
	union{
	float lgt_uv[1];
	float lu;
	};
	float lv;
	float normal[3];	
}_VERTEX_UV;

typedef struct{
	Vector2f uv;
}_TVERTEX;

typedef struct{
    Vector3l tvnum;
}_TVFACE;

typedef struct{
    Vector3l v;		// 삼각형	인덱스
    Vector3l tv;	// 텍스쳐	인덱스 
    Vector3l tv2;	// 텍스쳐	2번채널.인덱스 
    
    Vector3f fu;
    Vector3f fv;
    int mtlid;      //material id
	Vector4f f_normal;	//평면 방정식
	DWORD smooth_group;			//0이면 스무스 그룹을 사용하지 않는다. 비트필드
//	Vector3f v_normal[3];

	//------------ 수작업 라이트맵좌표를 위해...
    Vector3f lu;	//라이트맵 u,v좌표 
	Vector3f lv;

/*
// 라이트맵을 위해 
	int l_mtlid;	//라이트맵 머터리얼 ID
	_LIGHT_MAP *lgt_ptr;
    Vector3f lu;	//라이트맵 u,v좌표 
	Vector3f lv;
*/
}_FACE;		//삼각구조체.

typedef struct{
	BYTE B,G,R;
}_LIGHTMAP_COLOR;

typedef struct{
	int xl,yl;
	_LIGHTMAP_COLOR *bitmap;	//ARGB 8888 포맷 
}_LIGHT_MAP;

//----------

typedef struct{
	LONG vertex_num;
	LONG *vertex_id;
	Vector3f *v_normal;
	float *fv,*fu;
	float *lv,*lu;
	DWORD smooth_group;			//0이면 스무스 그룹을 사용하지 않는다. 비트필드
	Vector4f f_normal;	//평면 방정식
	DWORD f_normal_id;

	union{
	LONG temp;		//BSP에서 사용하는 템프변수 
	float f_temp;
	};
	DWORD	argb;		//컬러및 알파값...
	SHORT mtlid;	//머터리얼 아이디
	SHORT l_mtlid;	//라이트맵 머터리얼 아이디
	_LIGHT_MAP *lgt_ptr;	//라이트맵 포인터
}_POLYFACE;		//다각형을 위한 구조체...
//----------


typedef float _MATRIX[4][4];  

typedef struct{
	float frame;		//프래임 번호 ( 예:12.1 프래임)
	float x,y,z;
}_POS_TRACK;			//위치트랙
typedef struct{
	float frame;		//프래임 번호 ( 예:12.1 프래임)
	float x,y,z,w;		//쿼터니온
}_ROT_TRACK;			//회전 트랙
typedef struct{
	float frame;		//프래임 번호 ( 예:12.1 프래임)
	float x,y,z;		
	float qx,qy,qz,qw;	//스케일 축.(쿼터니온.)
}_SCALE_TRACK;			//스케일 트랙

//-------오브젝트 타잎 속성.	word형으로 해놓은다.
// 주의할점은 앞의 1byte는 버텍스 압축 타잎으로 바뀐다.
#define _DYNAMIC_OBJECT		0x8000
#define _NO_MERGE_OBJECT	0x4000
#define _EVENT_ANI			0x2000		//이벤트 애니메이션일경우.
#define _DEFAULT_ANI		0x1000		//평범한 애니메이션일 경우.
#define _Y_BILLBOARD		0x0800		//평범한 애니메이션일 경우.
#define _CORONA				0x0400		//코로나의 경우
#define _V_FOG				0x0200		//볼륨 포그의 경우
#define _NO_LIGHT_MAP		0x0100		//라이트 맵을 필요 없게 하는 경우. = 버텍스 라이트다.
#define _COMPOSITE_LIGHT_MAP	0x0080		//컴포짓 라이트 맵

//오브젝트 타잎 
#define _SHADOW_CAST_OFF	0x08000000
#define _SHADOW_RECEIVE_OFF 0x04000000		// 리시브 오프되면 노 라이트 맵이된다.
#define _OBJECT_TYPE_COMPOSITE		0x02000000		// 콤포짓 타입.

typedef struct{
    char objectname[128];
    char parentname[128];	//부모 이름 없으면 [0]이 NULL/
	DWORD type;				
	DWORD composite_id;			//콤포짓 라이트맵 아이디 
//	int dynamic_object_num;		// 다이나믹 object 번호
//	int dynamic_object_index;		// 다이나믹 object index
    int vertexnum;
    int tvertexnum;
    int tvertexnum_ch2;	//채널2의 텍스쳐버텍스
    int facenum;
    int tvfacenum;
    int material_ref;       // use material number
	union{
	    float ani_matrix[4][4];  // 자식을 위한 애니 매트릭스 
		D3DMATRIX d3d_ani_matrix;	
	};
    
	union{
		float revmatrix[4][4];  // rotate normal matrix
		D3DMATRIX d3d_rev_matrix;	
	};

	union{
		float trmatrix[4][4];
		D3DMATRIX d3d_trmatrix;
	};
//    float tmscale[3];       // x,y,z scale
    _VERTEX *vertex;        // start vertex pointer
	union{
    _FACE *face;            // start tri polygon pointer
    _POLYFACE *p_face;      // start polygon pointer
	};
    _TVERTEX *tvertex;      // texture vertex pointer
	_TVERTEX *tvertex_ch2;
	Vector3f *fnormal;		// face normal
    Vector3f *vnormal;       // vertex normal

	Vector3f bb_min,bb_max;		// 오즈젝트 바운딩 박스
	DWORD *vcolor;			//버텍스 컬러
//-------------------------- new ani 구조
	WORD start_frame,end_frame;		//이 오브젝트의 애니메이션시작 끝프래임수.

	int Pos_cnt;			// 위치 트랙 갯수 
	_POS_TRACK *Pos;		// 위치 트랙		x,y,z					하위갯수[4]
	int Rot_cnt;			// 로테이트 트랙 갯수
	_ROT_TRACK *Rot;		// 로테이트 트랙	x,y,z,w 의 쿼터니온		하위갯수[5]
	int Scale_cnt;			// 스케일 트랙 갯수
	_SCALE_TRACK *Scale;	// 스캐일 트랙	x,y,z	하위갯수[4]
	Vector4f scale_quat;	// 스케일 축 쿼터니온 값 
	Vector3f scale;			// 스케일애니가 없을경우 스케일값.
	Vector4f quat;			//회전애니가 없을 경우 쿼터니온 값 
	Vector3f pos;			//이동 애니가 없을 경우 사용한다.
	union{
	    float s_matrix[4][4];     //scale local matrix
		D3DMATRIX d3d_s_matrix;
	};
	WORD event_id;	//이벤트애니의 지정 아이디.
}_OBJECT;

//-----다이나믹 오브젝트 구조체... 엔티티와 bsp안에서 같이 사용한다.
typedef struct {
	WORD	flag;			// 오브젝트 플래그?
	WORD	parent;			// 0은 부모가 없다. parent-1 = index
	int frames;				//해당 오브젝트의 프래임수
	int Pos_cnt;			// 위치 트랙 갯수 
	int Rot_cnt;			// 로테이트 트랙 갯수
	int Scale_cnt;			// 스케일 트랙 갯수 

	Vector3f scale;		//이동애니가 없을 경우 이동값 
	Vector4f scale_quat;		//이동애니가 없을 경우 이동값 
	Vector3f pos;		//이동애니가 없을 경우 이동값 
	Vector4f quat;		//회전애니가 없을 경우 쿼터니온 값 
	DWORD pos_offset;
	DWORD rot_offset;
	DWORD scale_offset;
/*
	union{
	    float s_matrix[4][4];     //scale local matrix
		D3DMATRIX d3d_s_matrix;
	};*/
//	_POS_TRACK *Pos;		// 위치 트랙		x,y,z					하위갯수[4]
//	_ROT_TRACK *Rot;		// 로테이트 트랙	x,y,z,w 의 쿼터니온		하위갯수[5]
}_D_OBJECT;


//머터리얼 attr
#define _MAT_NORMAL_ALPHA		0x80000000	
#define _MAT_REFLECT			0x40000000	
#define _MAT_VOLUME_FOG			0x20000000	

typedef struct{
    char mapname[256];      // to texture buffer
    int mapxl;
    int mapyl;
    BYTE *buffer;
}_TEXTURE;

#define _MAT_TYPE_MUTISUBOBJECT		0
#define _MAT_TYPE_STANDARD			1
#define _MAT_TYPE_COMPOSITE			2
#define _MAT_TYPE_UNKNOWN			255		//지원안하는거다.


typedef struct _SUBMATERIAL{
//   Vector3f ambient;   // r,g,b
//    Vector3f diffuse;   // r,g,b
//    Vector3f specular;   // r,g,b
//    float shine;
	DWORD type;			//머터리얼 타입 
    float reflect;		//반사값
    float self_illum;	//스테인드를 쓰기위한 거...
    float transparency;	//머터리얼의 투명도 0은 완전투명 1은 불투명
    int all_num;		//0번에만 있다. 전체 머터리얼 갯수 
	int same_mat;		//같은 텍스쳐를 쓰는 경우 그 머터리얼 번호, 없으면 -1
    int sbnum;
	int new_matnum;		//새로운 머터리얼 번호 
    int attr;   // 그 머터리얼의 속성 ,반사 오파,투명,워터 애니등
	int lgt_id;
	BOOL is_used;		//사용하지않는 머터리얼이라면 FALSE 가....
    char name[256];
	_TEXTURE *tex;		// 텍스쳐맵...
	_TEXTURE *oppa;		//오파맵이 있는경우.
    struct _SUBMATERIAL *submaterial;
}_MATERIAL;


typedef struct{
	BOOL poly_type;			//삼각형인지 FALSE 다각형 TRUE
	int objectnum;
	_OBJECT *object;
//    int material_num;
//    _MATERIAL *material;
//------------------------- new ani
}_MESH;	

typedef struct{
	int is_loop;			// 루프되는 애니일경우 1 아닐경우 0
	int StartFrame;		//시작 프래임과 끝 프래임.
	int EndFrame;
	int FrameSpeed;			// 초당 몇 프래임인가...
	int TickPerFrame;		// 프래임당 틱 
}_SCENE;
//--------------------------라이트및 카메라.
#define _OMNI 1
#define _SPOT 2
#define _DIRECTIONAL 3

typedef struct{
	BOOL is_calculated;
    int light_num;   // only light[0].light_num uses;
    int type;   // omni=1 spot=2;
//------------ to spotlight
    float hotspot,falloff;
    int attn_nearon,attn_faron;

    float near_attnstart,near_attnend;	//attenuation값들 
    float attnstart,attnend;
    Vector3f tpos;
    float is_shadow;
    float shadow_color[3];
    float shadow_density;
    float shadow_size;
    float shadow_sample_range;
//-----------------------
    Vector3f rgb;
    Vector3f pos;
    float intens;		//0.0 - 2.0까지 지원
}_LIGHTSOURCE;

typedef struct{
	int type;	//type 0 is target 1 is free
    int num;    // camera numbers   it's only 'Camera[0].num' uses
	union{
	    float m_matrix[4][4];     //main matrix
		D3DMATRIX d3d_trmatrix;
	};
	union{
	    float s_matrix[4][4];     //scale local matrix
		D3DMATRIX d3d_s_matrix;
	};
    float t_matrix[4][4];     //target matrix
    Vector3f pos,target;
    float fov;              // float
    float tdist;            // screen scale; if X 640

//---------애니메이션을 위한 구조체...
    char objectname[128];
    char parentname[128];	//부모 이름 없으면 [0]이 NULL/
	int Pos_cnt;			// 위치 트랙 갯수 
	_POS_TRACK *Pos;		// 위치 트랙		x,y,z					하위갯수[4]
	int Rot_cnt;			// 로테이트 트랙 갯수
	_ROT_TRACK *Rot;		// 로테이트 트랙	x,y,z,w 의 쿼터니온		하위갯수[5]
//	int Scale_cnt;			// 스케일 트랙 갯수 
//	_SCALE_TRACK *Scale;	// 스켈일  트랙		x,y,z				
	Vector4f quat;		//회전애니가 없을 경우 쿼터니온 값 
//	Vector3f pos;			//이동 애니가 없을 경우 사용한다.
}_CAMERA;

typedef struct{
	int num;		//0번 구조체에만 있다.
	Vector3f bb_min;	//바운딩 박스
	Vector3f bb_max;	//바운딩 박스

    char objectname[256];
    char parentname[128];	//부모 이름 없으면 [0]이 NULL/
	int Pos_cnt;			// 위치 트랙 갯수 
	_POS_TRACK *Pos;		// 위치 트랙		x,y,z					하위갯수[4]
	int Rot_cnt;			// 로테이트 트랙 갯수
	_ROT_TRACK *Rot;		// 로테이트 트랙	x,y,z,w 의 쿼터니온		하위갯수[5]
//	int Scale_cnt;			// 스케일 트랙 갯수 
//	_SCALE_TRACK *Scale;	// 스켈일  트랙		x,y,z				
	Vector4f quat;		//회전애니가 없을 경우 쿼터니온 값 
	Vector3f pos;			//이동 애니가 없을 경우 사용한다.
	union{
		float trmatrix[4][4];
		D3DMATRIX d3d_trmatrix;
	};
	union{
	    float s_matrix[4][4];     //scale local matrix
		D3DMATRIX d3d_s_matrix;
	};
}_HELPER;

#pragma	pack(pop)


#endif
