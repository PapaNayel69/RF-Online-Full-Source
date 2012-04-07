#ifndef __R3BSP_H__
#define __R3BSP_H__

#include <windows.h>
#include <stdio.h>
#include "R3Alpha.h"
#include "C_vector.h"
#include "R3D3d8.h"
#include "ToolCollision.h"
#include "Animation.h"
#include "entity.h"
#include "SoundEntity.h"
#include "R3Particle.h"
#include "CPathFinder.h"

//#define EXPORT extern "C" _declspec (dllexport)

//bsp추가 세이브를 위한 구조체.
typedef struct{
	//충돌 폴리곤...
	_TOOL_COL_POINT *ColPoint;
	_TOOL_COL_LINE *ColLine;
	WORD *ColLineID;
	_TOOL_COL_LEAF *ColLeaf;
	//엔티티
	_ENTITY_LIST *EntityList;
	_LEAF_ENTITIES_LIST_INFO *LeafEntityList;
	_READ_MAP_ENTITIES_LIST *MapEntitiesList;
	WORD *EntityID;
	//사운드..(배경 효과음...)
	_READ_SOUND_ENTITY_LIST *SoundEntityList;
	_LEAF_SOUND_ENTITIES_LIST_INFO *LeafSoundEntityList;
	_READ_SOUND_ENTITIES_LIST *SoundEntitiesList;
	WORD *SoundEntityID;
}_ADD_BSP_SAVE;


#define _READ_BSP_SPARE_CNT	35	//26		//파일확장을 위한 여분카운트.
#define _FREE_BSP_SPARE_CNT	32  //29


#pragma	pack(push, 1)		//바이트 정렬.

#define _R3BSP_VERSION			39	// 000부터 출발
#define _R3_EXT_BSP_VERSION		20	// 19 부터 출발
//------------------- 실제 export되는 BSP 
typedef struct{
	DWORD offset;		//옵셋.
	DWORD size;			//엔트리 사이즈.
}_ENTRY;

typedef struct{
	DWORD version;		//_R3BSP_VERSION
//------------------- 데이터를 읽을때 메모리에 가지고 있는것들...
	_ENTRY CPlanes;		// 면 노멀	d값은 없다.
	_ENTRY CFaceId;		// 면 인덱스리스트	임시로 사용.... 나중에 최적화
	_ENTRY Node;		// 노드 
	_ENTRY Leaf;		// 리프

	_ENTRY MatListInLeaf;	//	리프에 있는 머터리얼그룹리스트.

	//---	애니메이션 관련.
	_ENTRY Object;	//현재는 다이나믹 오브젝트로 쓴다. 
	_ENTRY Track;	//애니메이션 트랙
	_ENTRY EventObjectID;		//	이벤트 오브젝트 아이디.	 다이나믹 오브젝트중 이벤트 오브젝트 애니메이션 아이디.
/*
	//----  충돌라인관련 -- 주의할점은 툴에서 추가할수 있다는 점이다.
	_ENTRY CFVertex;	//	충돌 페이스 점.
	_ENTRY CFLine;		//	충돌 페이스 라인
	_ENTRY CFLineId;	//	충돌 페이스 아이디 리스트.
	_ENTRY CFLeaf;		//	충돌 페이스 리프.
	_ENTRY EntityList;		//	엔티티 종류 리스트
	_ENTRY EntityID;		//	 map entities list 아이디를 좌악.
	_ENTRY LeafEntityList;		//	리프 엔티티 리스트. 리프당 시작 아이디와 갯수를 가지고 있음.

	_ENTRY SoundEntityID;		//	사운드 리프 엔티티 리스트 아이디
	_ENTRY LeafSoundEntityList;		//	리프 사운드 엔티티 리스트. 	리프당 시작 아이디와 갯수를 가지고 있음.
*/	

	_ENTRY ReadSpare[_READ_BSP_SPARE_CNT];

//------------------- 데이터를 읽고나서 Free하는 것들.
	//-------- 화면 디스플레이용...
	_ENTRY BVertex;		//	byte 점
	_ENTRY WVertex;		//	word 점
	_ENTRY FVertex;		//	float점
	_ENTRY VertexColor;	// 버텍스 컬러 
	_ENTRY UV;			// uv좌표
	_ENTRY LgtUV;		// 라이트맵 uv좌표	//WORD형이다.
	_ENTRY Face;		//	면
	_ENTRY FaceId;		// 면 인덱스리스트
	_ENTRY VertexId;	// 점 인덱스리스트
	_ENTRY ReadMatGroup;	//	머터리얼 그룹..	읽기용으로 쓸거다..
/*
	_ENTRY MapEntitiesList;	//	맵상에 뿌려지는 엔티티리스트.
	_ENTRY SoundEntityList;	//	사운드 엔티티리스트.	
	_ENTRY SoundEntitiesList;	//	사운드 엔티티즈 리스트.  차이점이 머냐구?물어봐..
*/
	_ENTRY FreeSpare[_FREE_BSP_SPARE_CNT];
}_BSP_FILE_HEADER;



#define _READ_EXT_BSP_SPARE_CNT	18			//파일확장을 위한 여분카운트.
#define _FREE_EXT_BSP_SPARE_CNT	18  

typedef struct{		//툴에서 추가된 BSP 정보.
	DWORD version;		//_R3_EXT_BSP_VERSION
//---읽음과 동시에 메모리에 쓰여지는것.
	// 충돌라인관련 
	_ENTRY CFVertex;	//	충돌 페이스 점.
	_ENTRY CFLine;		//	충돌 페이스 라인
	_ENTRY CFLineId;	//	충돌 페이스 아이디 리스트.
	_ENTRY CFLeaf;		//	충돌 페이스 리프.
	//엔티티 
	_ENTRY EntityList;		//	엔티티 종류 리스트
	_ENTRY EntityID;		//	 map entities list 아이디를 좌악.
	_ENTRY LeafEntityList;		//	리프 엔티티 리스트. 리프당 시작 아이디와 갯수를 가지고 있음.
	//사운드
	_ENTRY SoundEntityID;		//	사운드 리프 엔티티 리스트 아이디
	_ENTRY LeafSoundEntityList;		//	리프 사운드 엔티티 리스트. 	리프당 시작 아이디와 갯수를 가지고 있음.
	//리딩 여분 
	_ENTRY ReadSpare[_READ_EXT_BSP_SPARE_CNT];

//-- 읽고 내부에서 사용되는 메모리로 조정하는것들.
	_ENTRY MapEntitiesList;	//	맵상에 뿌려지는 엔티티리스트.
	_ENTRY SoundEntityList;	//	사운드 엔티티리스트.	
	_ENTRY SoundEntitiesList;	//	사운드 엔티티즈 리스트.  차이점이 머냐구?물어봐..

	_ENTRY FreeSpare[_FREE_EXT_BSP_SPARE_CNT];
}_EXT_BSP_FILE_HEADER;


/*
typedef struct{
	WORD f_normal_id;		//	페이스 노멀 아이디
	float d;				//	ax+by+cz = d 의 d
	WORD vertex_num;			//	vertex갯수
	DWORD vertex_start_id;	//	시작 vertex id = uv맵 시작 id = light맵uv 시작 id(몽땅 같다.)
}_BSP_C_FACE;
*/

typedef struct{
	DWORD f_normal_id;		//	노드를 가르는 평면 방정식
	float d;				//	ax+by+cz = d 의 d
	SHORT front;
	SHORT back;
	Vector3s bb_min,bb_max;	//노드의 바운딩 박스 대략 5%정도 더크게...
}_BSP_NODE;

typedef struct {
	BYTE type;					//노드의 특성. water라든지...
	WORD face_num;				//bsp 페이스 갯수
	DWORD face_start_id;		//시작bsp face id번호

	WORD m_group_num;			//머터리얼 그룹  갯수
	DWORD m_group_start_id;		//시작 머터리얼 그룹 id번호.

	Vector3s bb_min,bb_max;		//노드의 바운딩 박스
}_BSP_LEAF;



//---- 읽기위한 구조체...
#define _VERTEX_ARC_TYPE_CHAR		0x8000
#define _VERTEX_ARC_TYPE_SHORT		0x4000
#define _VERTEX_ARC_TYPE_FLOAT		0x2000

typedef struct {
	WORD attr;		//상위 바이트는 압축타입 0 보통폴리곤,1 지형커브?
	WORD face_num;	//해당 면 갯수
	DWORD face_start_id;
	SHORT mtl_id;	//머터리얼 아이디
	SHORT lgt_id;	//라이트 맵 아이디
	Vector3s bb_min,bb_max;		//머터리얼 바운딩 박스
	Vector3f pos;				//위치
	float scale;				//스케일값...
	WORD object_id;		//오브젝트 id= 실제 id+1 현재는 다이나믹 오브젝트를 위해 사용.
}_BSP_READ_M_GROUP;

typedef struct{
	WORD v_num;		//해당 점 갯수
	DWORD v_start_id;	//점수...
}_BSP_READ_FACE;

//-----수정되는 구조체.

#define _MAT_GROUP_TYPE_STANDARD	0x0001	//라이트맵이 들어가 있는 일반적인 버텍스 버퍼.
#define _MAT_GROUP_TYPE_NORMAL		0x0002	//노멀이 들어가있어야되는 버텍스 버퍼.
#define _MAT_GROUP_TYPE_OPPA		0x8000	//오파및반투명으로 된 그룹.
#define _MAT_GROUP_TYPE_REFLECT		0x4000	//반사 거울이다.

#define _MAT_Y_BILLBOARD				0x0800	//y빌보드 그룹.
#define _MAT_CORONA						0x0400	//코로나 다.

typedef struct {
	WORD Type;				//어떤타잎인가..
	WORD TriNum;			//해당 삼각 갯수
	SHORT MtlId;			//머터리얼 아이디
	SHORT LgtId;			//라이트 맵 아이디
	Vector3f BBMin,BBMax;	//머터리얼 바운딩 박스
	Vector3f Origin;		//바운딩박스의 중심점이다.
	DWORD VBMinIndex;		//이 머터리얼 블록이 시작되는 버텍스 버퍼 인덱스
	DWORD IBMinIndex;		//인덱스버퍼 최소 번호.
	DWORD VertexBufferId;	//버텍스 버퍼 아이디.(멀티 버텍스버퍼를 위한..)
	DWORD VCnt;				//요그룹의 버텍스 갯수..
	DWORD CFaceStartVId;		//충돌 폴리곤과의 연계를 위한 시작 버텍스버퍼아이디.
	void *MultiSourceUV;		//멀티레이어가 쓸 소스 포인터. uv벡터
	void *MultiSourceST;		//멀티레이어가 쓸 소스 포인터. st벡터
	WORD ObjectId;		//오브젝트 id= 실제 id+1 현재는 다이나믹 오브젝트를 위해 사용.
	union{				//공용체로 템프변수를 접근한다.
	float CoronaAlpha;
	};
}_BSP_MAT_GROUP;

#define _CFACE_ATTR_NOT_FLOOR	0x80	//충돌 폴리곤이 바닥이 아닌경우..(기울기로 계산한다.)
#define _CFACE_NO_COLLISION		0x40	//충돌 개무시 폴리곤.
#define _CFACE_WATER			0x20	//물충돌 폴리곤.

typedef struct{				// 다이나믹 라이트와 제대로 된 충돌을 위한.
	BYTE Attr;				// 평면의 속성,
	BYTE VNum;				//해당 점 갯수
	DWORD VStartId;			//점수...	//edge노멀도 같다.
	WORD MatGIndex;			//머터리얼 그룹 인덱스
	Vector4f Normal;		//	ax+by+cz = d 
}_BSP_C_FACE;

#pragma	pack(pop)


#define _MAX_DYNAMIC_VB_FACE_NUM	1024	//한번 처리의 최대 다이나믹 버텍스버퍼 폴리곤갯수...
#define	_MAX_VB_CNT		80	//80개면 대략 백만 폴리곤을 처리한다.

#define _PATH_NOT_FOUND			0
#define _PATH_FOUND				1
#define _PATH_TOO_LONG_CLIP		2		//멀리찍을 경우 계속 갈수있게 하기위해서..


#define _EVENT_ANI_STATE_FRONT_STOP			0
#define _EVENT_ANI_STATE_FRONT_START		1
#define _EVENT_ANI_STATE_FRONT_RUN			2
#define _EVENT_ANI_STATE_BACK_START			3
#define _EVENT_ANI_STATE_BACK_RUN			4
#define _EVENT_ANI_STATE_BACK_STOP			5


class  CBsp{
private:
	//충돌구조 관련 
	Vector3f *mCVertex;		// 점 
	Vector3f *mCNNormal;	// 충돌 노드 노멀	d값은 없다.
	DWORD *mCVertexId;		// 점 아이디 리스트
	_BSP_C_FACE *mCFace;	// 충돌 면 엔트리
	Vector4f *mCNEdgeNormal;	// 충돌 노드 에지노멀, 메모리때문에 서버에서만 쓴다.
	DWORD *mCFaceId;			// 충돌 면 인덱스리스트	임시로 사용중.... 나중에 최적화

	DWORD *mVertexColor;	//버텍스 컬러 배열.
	Vector2s *mLgtUV;		// 라이트맵 uv좌표를 가지고있는거.. 
	LONG mNowCFaceId;		// nextypos에서 충돌된 가장 최근에 계산된 페이스 아이디. 라이팅을 구할때 쓴다.

	WORD *MatListInLeafId;		//리프에 있는 머터리얼 그룹 리스트 아이디
	DWORD mMatGroupNum;			//머터리얼 블록 총갯수.
	_BSP_MAT_GROUP *mMatGroup;		//머터리얼 블록 읽기.

	DWORD	mCVertexNum;	//충돌점갯수
	DWORD	mCFaceNum;		//충돌면갯수

	DWORD	mVertexNum;		//점갯수
	DWORD	mFaceNum;		//면갯수

	BOOL mIsLoaded;			//맵 로딩 되었는가?

	//다이나믹 오브젝트
	DWORD mObjectNum;		//다이나믹 오브젝트 갯수 
	_ANI_OBJECT *mObject;		//다이나믹 오브젝트 
	WORD *mEventObjectID;		//이벤트 오브젝트의 아이디..

	//---Environment 렌더관련.  // 비,눈,기타 폭풍등...
#define _ENV_PARTICLE			0x0001000
	DWORD mEnvID;				//환경 아이디..
	union{
	CEntity *mEnvEntity;		// 환경은 엔티티및 파티클 로 접근된다.
	CParticle *mEnvParticle;
	};
public:
	//엔티티 관련
	BYTE *mEntityCache;		//맵 엔티티 캐쉬
	DWORD mEntityCacheSize;	//맵 엔티티 캐쉬 사이즈 
	DWORD mNowRenderMatGroupNum;	// 현재 렌더링 되고 잇는 머터리얼 블록 갯수.
	DWORD mNowShadowMatGroupNum;	// 현재 렌더링 되고 잇는 쉐도우머터리얼 블록 갯수.
	inline DWORD GetMatGroupNum()	{	return mMatGroupNum;	};			//머터리얼 블록 총갯수.


	CEntity *mEntity;		//맵상의 실제 엔티티
	CParticle *mParticle;	//맵상의 실제 파티클

	_ENTITY_LIST *mEntityList;		//	엔티티 종류 리스트
	WORD		*mEntityID;		//	 map entities list 아이디를 좌악.
	_LEAF_ENTITIES_LIST_INFO *mLeafEntityList;		//	리프 엔티티 리스트. 리프당 시작 아이디와 갯수를 가지고 있음.
	_MAP_ENTITIES_LIST *mMapEntitiesList;	//맵상의 엔티티들 리스 
	DWORD mEntityListNum;
	DWORD mLeafEntityListNum;
	DWORD mEntityIDNum;
	DWORD mMapEntitiesListNum;
	void LoadEntities( _READ_MAP_ENTITIES_LIST *readMapEntitiesList);	//엔티티들을 로딩한다.
	void ReleaseEntities();												//엔티티들을 릴리즈한다.
	void CalcEntitiesMainColor();										//엔티티들 바닥컬러계산.
	void RenderOneEntityRender(WORD entities_id);	//맵 엔티티하나 실레로 렌더링.

	BOOL IsInWater(Vector3f src,Vector3f tar,Vector3f *get=NULL,float plane_dist=0);	// 시작과 끝이 워터 속인가.주의 src는 물위좌표즉 위쪽이여야 한다.

	//사운드 엔티티 관련...
	BYTE *mSoundEntityCache;		//사운드 엔티티 캐쉬 할 버퍼다.
	DWORD mSoundEntityCacheSize;	//사운드 엔티티 캐쉬 사이즈 

	DWORD mSoundEntityIDNum;
	DWORD mLeafSoundEntityListNum;
	DWORD mSoundEntityListNum;		//사운드 원본 갯수.
	DWORD mSoundEntitiesListNum;	//복사판들 갯수.
	_SOUND_ENTITY_LIST *mSoundEntityList;
	_SOUND_ENTITIES_LIST *mSoundEntitiesList;
	WORD	*mSoundEntityID;		//	 사운드 entities list 아이디를 좌악.
	_LEAF_SOUND_ENTITIES_LIST_INFO *mLeafSoundEntityList;
	void LoadSoundEntities(_READ_SOUND_ENTITY_LIST *sound_entity_list ,_READ_SOUND_ENTITIES_LIST *sound_entities_list);	//엔티티들을 로딩한다.
	void ReleaseSoundEntities();	//엔티티들을 릴리즈한다.
	void HearMapSound();	//맵 사운드를 듣자.(렌더링이 아니므로 endscene()이후에 넣자..)

	DWORD mTotalAllocSize;	//bsp메모리 전체(배경효과음만 일단 제외)
	DWORD mTotalWaveSize;	//배경효과음만
	DWORD mVertexBufferSize;	//맵 버텍스버퍼 사이즈 전체.

	BOOL IsLoaded()								{	return mIsLoaded;	};
	void SetIsLoaded(BOOL load_ok)				{	mIsLoaded=load_ok;	};

public:
	_BSP_NODE *mNode;		// 노드 
	_BSP_LEAF *mLeaf;		// 리프

	DWORD	mLeafNum;		//리프갯수
	DWORD	mNodeNum;		//노드갯수

	//--충돌 페이스(라인) 관련.
	DWORD mCFVertexNum;			//	충돌 페이스 점.
	DWORD mCFLineNum;			//	충돌 페이스 라인
	DWORD mCFLineIdNum;			//	충돌 페이스 아이디 리스트개수.

	Vector3f *mCFVertex;			//	충돌 페이스 점.
	_TOOL_COL_LINE *mCFLine;	//	충돌 페이스 라인
	WORD *mCFLineId;			//	충돌 페이스 아이디 리스트.
	_TOOL_COL_LEAF *mCFLeaf;	//	충돌 페이스 리프. 갯수는 물론 리프수와 같다.

	Vector3f *mCFVNormal;		//충돌 버텍스 노멀.
	Vector4f *mCFNormal;		//충돌 노멀.

	void RenderCollisionLeaf(short leaf_id);//주의 충돌 폴리곤을 렌더링한다.

private:
	void MakeEdgeNormal();	//서버구조에서는 빠른 충돌 첵크가 필요하다.Edge normal을 가지고 있는다.

	BOOL IsExistSelfPoint(int id,int face);	//페스리스트에 충돌 페이스의 점들이 있나?
	void GetFaceFrontPoint(Vector3f *get_temp,int v_id);	//평면의 약간 앞으로 튀어나온 점을 얻어온다.
	void GetPath(Vector3f from,Vector3f at);
	int GetFinalPath(void *node_p,Vector3f from,Vector3f get[16]);
	int GetPathCrossPoint(Vector3f from,Vector3f at,Vector3f *get,int now_f1,int now_f2);
	BOOL EdgeTest(Vector3f cross,int id);
	void GetLightMapUVFromPoint(Vector2f getuv,int face_id,Vector3f p);	//평면위의 한 점을 넘겨주면 uv를 구해준다.
	DWORD GetColorFromPoint(int face_id,Vector3f p);	//버텍스 컬러면의 평면위의 한 점을 넘겨주면 컬러를 구해준다.

public:
	DWORD GetLightFromPoint(Vector3f p,DWORD diff=2); //현재 밟고있는 평면위의 한 점을 넘겨주면 맵의 라이팀을 구해준다.

	CPathFinder mPathFinder;	//패스파인더 
	int mColFaceId;	//충돌 페이스 아이디
	DWORD mFindPathCnt;			//패스 찾은 갯수.
	void SetCFNormal();			//충돌페이스 점및 라인 노멀을 구한다.
	DWORD GetPathFind(Vector3f from,Vector3f at,Vector3f get[16],DWORD *cnt,int depth=_MAX_PATH_POS);	//패스 파인더.못찾으면 false를 리턴하지만 최대로 갈수있는 좌표를 넣어준다.
	void DrawCollisionPoly(); //충돌 폴리곤을 뿌린다.
	void DrawLeafCollisionPoly(short leaf_id);//리프안의 충돌 폴리곤을 뿌린다.
	short mNowPlayerNum;		//현재 플레이어의 리프번호를 가지고 있다...

	void RenderEnvironment(DWORD id);	//환경이 존재하는... 비,눈,모래폭풍.
private:

	//버텍스 버퍼 관련 버텍스 버퍼는 총 3가지가 있다. 
	DWORD mStaticVBCnt;		//스태틱 버텍스버퍼의 카운트.
	CVertexBuffer mStaticVertexBuffer[_MAX_VB_CNT];	//정적버텍스 버퍼 (월드의 대부분이 여기에 있다)
	DWORD mVBVertexNum[_MAX_VB_CNT];		//버텍스버퍼의 버텍스 갯수를...
	CIndexBuffer mStaticIndexedBuffer;	//정적인덱스 버퍼 (월드의 대부분이 여기에 있다)
//	void* mNormalVertexBuffer;	//노멀이 포함된 정적 버텍스 버퍼 (메탈이나 env맵핑..)
//	void* mNormalIndexedBuffer;	//노멀이 포함된 정적 인덱스 버텍스 버퍼 (메탈이나 env맵핑..)
//	void* mDynamicVertexBuffer;	// 다이나믹 버텍스 버퍼.

	void* mMultiLayerUV;	//멀티레이어에서 쓸 uv
	void* mMultiLayerST;	//멀티레이어에서 쓸 st벡터

	//다이나믹 오브젝트 관련.
	void PrepareAnimation();		//애니메이션 하기전에 준비할것들..
	void GetBspObjectMatrix(float w_matrix[4][4],WORD id);
public:
	BOOL SetEventAnimationState(WORD id,DWORD flag);
	DWORD GetEventAnimationState(WORD id);
	BOOL SetAllAnimationState(DWORD flag);


	//----맵 관련 
	BYTE *mStaticAlloc;		//정적 할당 (파일상 그대로 메모리 올릴 때)
	DWORD mStaticAllocSize;	//정적 할당크기...
	
	BYTE *mExtBspStaticAlloc;		//정적 할당 (파일상 그대로 메모리 올릴 때)
	DWORD mExtBspStaticAllocSize;	//정적 할당크기...

	//--마우스 클릭시 클릭되는 폴리곤...
	int mPickPoly;	//-1일경우는 클릭이 안되었다..
private:
	BYTE *mMatGroupCache;	//머터리얼 그룹이 작동되는 캐쉬
	int mMatGroupCacheSize;	//머터리얼 그룹 캐쉬 사이즈 

//	Vector3f mPointCamera;	//내부에서 임시로 쓰는 포인트 카메라..
	Vector3f mTempCamera;	//내부에서 임시로 쓰는 플레이어 위치..
	short mTempSearchOk;
	short mNowLeafNum;		//현재 카메라의 리프번호를 가지고 있다...

	void GetVertexNormal();

	void RenderShadowMatGroup(WORD mat_g_id);	//머터리얼에 그림자를 그린다.
	void RenderMatGroup(WORD mat_g_id);
	void RenderLeaf(short leaf_id);
	void WalkLeaf(short leaf_id);
	void WalkNode(short node_id);
	void SearchNode(short node_id);
	void GetHeight(Vector3f camera);
	void LoopInitRenderedMatGroup();	//머터리얼 캐쉬 초기화..

	void SubLeafList(float su,_BSP_NODE *node,Vector3f src,Vector3f tar,SHORT *leaf_list,int *leaf_ids);
	void WalkNodeForLeafList(short node_id,Vector3f src,Vector3f tar,SHORT *leaf_list,int *leaf_ids);
	void SubLeafListFromBBox(float su,_BSP_NODE *node,Vector3f *bb_list,SHORT *leaf_list,int *leaf_ids);
	void FastWalkNodeForLeafListFromBBox(short node_id,Vector3f *bb_list,SHORT *leaf_list,int *leaf_ids);
	
	void ReadDynamicDataFillVertexBuffer(FILE *fp);						//bsp에서 파일로읽고 버려야할것들.
	void OnlyStoreCollisionStructure(_BSP_READ_M_GROUP *readmatgroup,	//서버에서 쓸 충돌부분만 읽는다.
		Vector3c *bvertex,Vector3s *wvertex,Vector3f *fvertex,DWORD *vertexid
		,_BSP_READ_FACE *face,DWORD *faceid );


	void ReadDynamicDataExtBsp(FILE *fp);	//ext bsp에서 파일로읽고 버려야할것들.

	//외부에서 접근하는 bsp관련
public:
	_BSP_FILE_HEADER		mBSPHeader;		//bsp파일 헤더... 파일 정보가 있다..
	_EXT_BSP_FILE_HEADER	mExtBSPHeader;	//ext bsp파일 헤더... 파일 정보가 있다..

	CBsp();
	~CBsp();
	ClearVariable();					//bsp 변수 클리어
	void LoadBsp(char *name);			//bsp 파일 로딩...
	void LoadExtBsp(char *name);		//ext bsp를 로딩하자.릴리즈는 ReleaseBsp()에서 해준다.
	void ReleaseBsp();					//bsp파일 클리어...

	void ReadyBspRender(Vector3f camera);
	void DrawBspRender();
	void DrawMapEntitiesRender();	//맵 엔티티들 렌더링.
	void DrawAlphaRender(Vector3f camera);
	void DrawLeafBBox();	//리프의 바운딩 박스를 출력.
	void DrawMatBBox();		//머터리얼 바운딩 박스 출력.

	short GetLeafNum(Vector3f player);	//현재의 리프번호를 얻어낸다.
	void GetLeafList(Vector3f src,Vector3f tar,LONG *num,SHORT *leaf_list,DWORD max_num);	//a,b점에 존재하는 리프 리스트를 얻어낸다.
	void GetFastLeafListFromBBox(Vector3f bb_min,Vector3f bb_max,LONG *num,SHORT *leaf_list,DWORD max_num);// 바운딩 박스를 넣어주면 리프리스트를 준다.

	//충돌 관련 함수..
	float GetFirstYpos(Vector3f pos,int option);	//구할수 없는경우 65535을 리턴
	float GetFirstYpos(Vector3f pos,Vector3s min,Vector3s max);	//바운딩 박스 영역안에서 구한다.구할수 없는경우 65535을 리턴
	float GetFirstYpos(Vector3f pos,Vector3f min,Vector3f max);	//바운딩 박스 영역안에서 구한다.구할수 없는경우 65535을 리턴

	float GetYposInLeafNoAttr(Vector3f a,Vector3f b,float able_height,float now_height,int leaf_id);// able은 전진 가능한 높이,now_현재 높이,leaf는 얻을 리프
	float GetYposInLeaf(Vector3f a,Vector3f b,float able_height,float now_height,int leaf_id);// able은 전진 가능한 높이,now_현재 높이,leaf는 얻을 리프
	float GetYposInLeafUseEdgeNormal(Vector3f a,Vector3f b,float able_height,float now_height,int leaf_id);// able은 전진 가능한 높이,now_현재 높이,leaf는 얻을 리프
	float GetBestYposInLeaf(Vector3f a,Vector3f b,float best_height,float now_height,int leaf_id);	// best는 올라갈수있는 적당한 높이,now_현재 높이,leaf는 얻을 리프


	BOOL CanYouGoThere(Vector3f now,Vector3f target,Vector3f *get_point); //저쪽까지 갈수 있니? 못가면 갈수있는 곳까지 get_point에 넣어준다.
	BOOL GetPointFromScreenRay(float x,float y,Vector3f tar_point,Vector3f src_point);//마우스 스크린좌표를 넘긴다.
	BOOL GetPointFromScreenRayFar(float x,float y,Vector3f tar_point,Vector3f src_point);//마우스 스크린좌표를 넘긴다.
	BOOL IsCollisionFace(Vector3f src,Vector3f tar);		//두벡터사이에 폴리곤이 충돌이되나? 코로나...
	BOOL IsCollisionFace(Vector3f src,Vector3f tar,Vector3f *get,float plane_dist);	//평면과의 떨어진 거리를 리턴
	BOOL IsCollisionFace(Vector3f src,Vector3f tar,Vector3f *get_pos,Vector4f *get_normal);	//충돌된 위치와 평면의 법선벡터를 리턴 

	BOOL IsCollisionFaceForServer(Vector3f src,Vector3f tar);	//두벡터사이에 폴리곤이 충돌이되나? 코로나...
	BOOL IsCollisionFaceForServer(Vector3f src,Vector3f tar,Vector3f *get,float plane_dist);	//평면과의 떨어진 거리를 리턴

//	Vector3f *mVNormal;		//버텍스 노멀인데 파일안에 없고 로딩시 생성한다.

	//	알파관련 API및 클래스 생성.
	CAlpha mAlpha;
	void *GetDynamicVertexBuffer();
	_BSP_MAT_GROUP *GetMatGroup();

	float GetMatGroupPoint(WORD group_id,Vector3f get);	// get은 바운딩박스의 중심.바운딩박스 절반의길이를 리턴한다.
	void RenderIndepentMatGroup(WORD mat_g_id);
	void CalcObjectLoop();		//오브젝트단위 루프로 갱신할 루틴이 있는 곳.

	BOOL IsInViewFrustum(WORD mat_id);  //해당 머터리얼 블록이 가시영역에 있는가?

	void PrepareShadowRender(Vector3f ch_pos,void *tex,float intensity,DWORD blur=3,float scale=1,float add_pos=0);	// 
//	void DrawShadowRender(Vector3f ch_pos,void *tex,float intensity,DWORD blur=3,float scale=1,float add_pos=0);	
	void DrawShadowRender(Vector3f ch_pos);	// 
	//쉐도우렌더시 블루어링값, 캐릭터 크기, 적당한 이동으로 텍스쳐 안에 적절한 크기로 만든다.

	void DrawDynamicLightSub(Vector3f bb_min,Vector3f bb_max);
	void DrawMagicLightSub(Vector3f bb_min,Vector3f bb_max);
	void DrawDynamicLights();

public:
	void SaveExtBsp(char *name,_ADD_BSP_SAVE *add_bsp);	//ebp를 저장.
};



#endif
