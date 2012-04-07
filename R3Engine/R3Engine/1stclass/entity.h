#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "c_vector.h"
#include "animation.h"
#include "R3Material.h"
#include "Renderflag.h"
#include "R3VertexBuffer.h"
#include "r3d3dtex.h"
#include "commonutil.h"

#define _ENTITY_VERSION		113

#pragma	pack(push, 1)		//바이트 정렬.

typedef struct{
	DWORD offset;		//옵셋.
	DWORD size;			//엔트리 사이즈.
}_E_ENTRY;

#define _IDENTITY_ENTITY	0
#define _IDENTITY_PARTICLE	1

//	파일 8byte를 읽으면 알수있다.
//	DWORD version;		//_R3BSP_VERSION
//	DWORD Identity;		//이 엔티티가 파티클인가.	
typedef struct{
	_E_ENTRY CompHeader;		//	float점
	_E_ENTRY Vertex;		//	float점
	_E_ENTRY VColor;		// vertex 컬러
	_E_ENTRY UV;			// uv좌표
	_E_ENTRY Face;		//	면
	_E_ENTRY FaceId;		// 면 인덱스리스트
	_E_ENTRY VertexId;	// 점 인덱스리스트
	_E_ENTRY MatGroup;	//	머터리얼 그룹..
	_E_ENTRY Object;	//	오브젝트.
	_E_ENTRY Track;	//	트랙.
}_ENTITY_FILE_HEADER;

//엔티티 압축 헤더
typedef struct {
	WORD type;		//버텍스 압축타입 
	Vector3s bb_min,bb_max;		//머터리얼 바운딩 박스
	Vector3f pos;				//위치
	float scale;				//스케일값...
	//매핑좌표
	float uv_min,uv_max;
}_ENTITY_COMP_HEADER;

typedef struct {
	WORD face_num;	//해당 면 갯수
	DWORD face_start_id;
	SHORT mtl_id;	//머터리얼 아이디
	WORD object_id;	//오브젝트 아이디... 다이나믹 오브젝트를 위한...
	Vector3s bb_min,bb_max;		//머터리얼 바운딩 박스
}_READ_ENTITY_M_GROUP;

typedef struct{
	WORD v_num;		//해당 점 갯수
	DWORD v_start_id;	//점수...
}_READ_ENTITY_FACE;

typedef struct {
	WORD Type;
	WORD TriNum;			//해당 삼각 갯수
	SHORT MtlId;			//머터리얼 아이디
	Vector3f BBMin,BBMax;	//머터리얼 바운딩 박스
	DWORD VBMinIndex;		//이 머터리얼 블록이 시작되는 버텍스 버퍼 인덱스
	DWORD IBMinIndex;		//인덱스버퍼 최소 번호.
	DWORD VCnt;				//요그룹의 버텍스 갯수..
	WORD ObjectId;			//다이나믹 오브젝트 아이디.
	void *MultiSourceUV;	//멀티레이어가 쓸 소스 포인터. uv벡터
}_ENTITY_M_GROUP;

//----------------------------------------------------




typedef struct{
	DWORD start_id;
	WORD entities_num;
}_LEAF_ENTITIES_LIST_INFO;

typedef struct{
	BYTE IsParticle;
	BYTE IsFileExist;		//로딩하고자하는 파일이 존재하는가?
	char Name[62];
	float FadeStart,FadeEnd;
	WORD Flag;			//엔티티 플래그..
	WORD ShaderID;		//쉐이더 아이디	//0번은 걍 디폴트..
	float Factor[2];
}_ENTITY_LIST;

typedef struct{
	WORD ID;		//엔티티 아이디.
	float Scale;
	Vector3f	Pos;
	float RotX,RotY;	//x,y회전값 
	Vector3s BBmin,BBmax;	// 바운딩 박스
}_READ_MAP_ENTITIES_LIST;	//파일로 읽는 루틴


class CParticle;
typedef struct{
	WORD ID;		//엔티티 아이디.
	float Scale;
	Vector3f	Pos;
	float RotX,RotY;	//x,y회전값 
	Vector3s BBMin,BBMax;	// 바운딩 박스
	float AddFrame;			//엔티티마다 애니를 틀리게 해주기 위해서.
	CParticle *Particle;	//파티클 포인터
	DWORD Color;			//알파및 색상.
	DWORD mMapColor;		//컬러값. 맵 바닥의 컬러값.	//주의할점은 2x이다.
}_MAP_ENTITIES_LIST;		// 실제 메모리에서 가지고있을거.


//---------------------------------------------------





#define _LOAD_POLY_IN_MEMORY		0x1		//엔티티 로딩시 버텍스나맵핑,폴리곤정보들을 메모리에 얹힌다.
#define _USE_VERTEX_SHADER			0x2		//버텍스 쉐이더를 사용하는 경우.
#define _DO_NOT_MANAGE_TEXTURE		0x8		//엔티티 텍스쳐 독립메니징.
#define _ENTITY_Z_DISABLE			0x10	//엔티티 텍스쳐 독립메니징.
#define _ENTITY_MAP_COLOR			0x20	//엔티티 맵 바닥 컬러 적용하기

//엔티티 플래그 
#define _EF_BILLBOARD				0x80

class CEntity{
private:
	BOOL mIsUseMemory;		//메모리 버텍스 프로세싱일 경우. 파티클의 경우 이렇게 접근...
	BOOL mIsEntityLoad;		//버텍스 버퍼를 생성못할때도 로딩이 안됬다고 나온다.
	BOOL mIsAlpha;			//하나의 머터리얼그룹이 알파인경우 소팅을 위해 셋팅된다.
	_ENTITY_FILE_HEADER mHeader;
 	DWORD mVetexBufferSize;	//총 버텍스버퍼 사이즈
//	BYTE *mStaticAlloc;		//정적 할당 (파일상 그대로 메모리 올릴 때)
	Vector2f *mOrgUV;
	DWORD mMapColor;		//컬러값. 맵 바닥의 컬러값.	//주의할점은 2x이다.
public:
	DWORD mFlag;			//엔티티 플래그다. 
	_R3MATERIAL *mMat;		//머터리얼..
//	R3Texture *mR3Tex;		//R3 텍스쳐포인터.
	
	CVertexBuffer mStaticVertexBuffer;	//정적버텍스 버퍼 
	CIndexBuffer mStaticIndexedBuffer;	//정적인덱스 버퍼 
 	DWORD mTextureSize;			//텍스쳐 사이즈
	int mMatGroupNum;
	_ENTITY_M_GROUP *mMatGroup;
	int mObjectNum;				//다이나믹 오브젝트 개수.
	_ANI_OBJECT *mObject;		//다이나믹 오브젝트 인덱스.
	BYTE *mTrack;				//다이나믹 오브젝트 트랙.
	int mStartTexID;			//시작 텍스쳐 아이디.
	int mTexNum;				//현재 엔티티가 가지고 있는 텍스쳐 갯수 
	void *mIndependencyTex;		//_DO_NOT_MANAGE_TEXTURE 옵션시.. 독립텍스쳐 포인터를 위한 공간... 주의해서 써라..
	float mStartTime;			//시작 타이머 변수.	텍스쳐 애니라든지 특별히 시작하는 부분 부터 보여주기위한 애니..
	void SetStartTimer()		{	mStartTime = R3GetTime();	};
public:
	Vector3f mBBMin,mBBMax;		//전체 바운딩 박스
	float	mAddFrame;			//엔티티마다 틀린 애니메이션을 주기위한... 

	CEntity::CEntity();
	CEntity::~CEntity();

//----- 폴리곤 정보(메모리에 포함하는경우.. 충돌이나 파티클에 쓸경우 하드웨어 버텍스버퍼가 아니라 메모리에 얹혀야된다.)
	BOOL LoadEntity(char *filename,DWORD option=0);
	void ReleaseEntity();

	BOOL IsLoadedEntity() {		return mIsEntityLoad;	};
	BOOL IsUseMemory() {		return mIsUseMemory;	};	

	BOOL IsFirstAlpha();	//첫번째머터리얼그룹이 알파인가? 엔티티 소팅이 필요.
	BOOL IsAlpha()	{	return mIsAlpha;	};			//한개라도 알파인가? 

//이거는 한번 뿌려주고 없애야되는 엔티티...  FALSE를 돌려주면 애니메이션이 끝나는 경우다.
	float mFrame;		//내부 프래임.한번뿌리고 없애기위해.
	BOOL OnceDrawEntity(float world_mat[4][4],DWORD color=0xffffffff);	//월드매트릭스를 넘겨준다.
//d3dmatrix를 인자로 넘겨줘야한다. 현재 R3Render.cpp와  R3D3d8에서만 
//다이렉트 3D로 다루려고 하기때문에 인자가 필요하다.
	void AddFlag(DWORD flag);	//상태 플래그를 추가한다.
	BOOL DrawEntity(float world_mat[4][4],DWORD color=0xffffffff,float ani_frame=-1);	//월드매트릭스를 넘겨준다. //평범한 엔티티출력루틴... //false일경우 뿌릴일이 없다(파티클)
	BOOL DrawEntityVS(_ENTITY_LIST *e_list,float world_mat[4][4],DWORD color=0xffffffff);	//월드매트릭스,버텍스쉐이더 쓰는 엔티티출력루틴... 

	void SetVertexShaderID(_ENTITY_LIST *e_list,DWORD color);	//버택스 아이디.

	int GetUsedVertexBufferSize();
	void GetAnimationMatrix(float get_ani_mat[4][4],_ANI_OBJECT *obj,float now_frame);
	void DrawOneMatGroup(_ENTITY_M_GROUP *m_group);	//하나의 머터리얼 그룹을 뿌린다.
	void DrawOneMatGroupVS(_ENTITY_M_GROUP *m_group);	//버텍스 쉐이더 하나의 머터리얼 그룹을 뿌린다.

	void RestoreTexMem();	//텍스쳐 메모리에 올린다.
	void ReleaseTexMem();	//텍스쳐 메모리에 내린다.


	//-----스카이박스 소팅해서 뿌리기위해 접근한다.
	_ANI_OBJECT *GetObject()	{	return mObject;	}
//	int GetObjectNum()			{	return mObjectNum;	}
	void PrepareAnimation();	//애니메이션 하기전에 준비할것들..
	int GetMatGroupNum()		{	return mMatGroupNum;	}
	int GetMatNum()				{	return mMat->m_iMatNum;	}	//루트가 갯수를 가지고있따.
	_ENTITY_M_GROUP *GetMatGroup(){	return mMatGroup;}

	void SetMapColor(DWORD color)	{	mMapColor=color;	}
	DWORD GetMapColor()				{	return mMapColor;	}

	CVertexBuffer* GetStaticVertexBuffer(){	return &mStaticVertexBuffer;	}
	CIndexBuffer* GetStaticIndexedBuffer(){	return &mStaticIndexedBuffer;	}
};


#pragma	pack(pop)


#endif