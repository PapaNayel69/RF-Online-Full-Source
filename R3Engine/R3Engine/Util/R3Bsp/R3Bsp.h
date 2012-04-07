#ifndef __R3BSP_H__
#define __R3BSP_H__


#pragma	pack(push, 1)		//바이트 정렬.


/*
충돌용 bsp는 최대 32767의 리프갯수와 노드갯수를 넘을 수 없다.
버텍스 갯수나 폴리곤 개수는 65536개를 넘을 수가 없다.
*/

#define _READ_SPARE_CNT	26		//파일확장을 위한 여분카운트.
#define _FREE_SPARE_CNT	32


#define _R3BSP_VERSION	39	// 000부터 출발
//------------------- 실제 export되는 BSP 
typedef struct{
	DWORD offset;		//옵셋.
	DWORD size;			//엔트리 사이즈.
}_ENTRY;

typedef struct{
	DWORD version;		//_R3BSP_VERSION

//------------------- 엔진에서 데이터를 읽을때 메모리에 바로 옮길 것들...
	_ENTRY Cplanes;		// 면 노멀	d값은 없다.
	_ENTRY Cface_id;		// 면 인덱스리스트	임시로 사용.... 나중에 최적화
	_ENTRY node;		// 노드 
	_ENTRY leaf;		// 리프

	_ENTRY MatListInLeaf;	//	리프에 있는 머터리얼그룹리스트.

	//---	애니메이션 관련.
	_ENTRY Object;	//현재는 다이나믹 오브젝트로 쓴다. 
	_ENTRY Track;	//애니메이션 트랙

	//----  충돌라인관련 -- 주의할점은 툴에서만 추가할수 있다는 점이다.
	_ENTRY CFVertex;	//	충돌 페이스 점.
	_ENTRY CFLine;		//	충돌 페이스 라인
	_ENTRY CFLineId;	//	충돌 페이스 아이디 리스트.
	_ENTRY CFLeaf;		//	충돌 페이스 리프.

	_ENTRY EntityList;		//	엔티티 종류 리스트
	_ENTRY EntityID;		//	 map entities list 아이디를 좌악.
	_ENTRY LeafEntityList;		//	리프 엔티티 리스트. 리프당 시작 아이디와 갯수를 가지고 있음.

	_ENTRY SoundEntityID;		//	사운드 리프 엔티티 리스트 아이디
	_ENTRY LeafSoundEntityList;		//	리프 사운드 엔티티 리스트. 	리프당 시작 아이디와 갯수를 가지고 있음.
	
	_ENTRY EventObjectID;		//	이벤트 오브젝트 아이디.	 다이나믹 오브젝트중 이벤트 오브젝트 애니메이션 아이디.

	_ENTRY ReadSpare[_READ_SPARE_CNT];

//------------------- 엔진에서 데이터를 읽고나서 Free할 것 들.
	//-------- 화면 디스플레이용...읽고나서 free해버리는거.
	_ENTRY BVertex;		//	byte 점
	_ENTRY WVertex;		//	word 점
	_ENTRY FVertex;		//	float점
	_ENTRY VertexColor;	// 버텍스 컬러 
	_ENTRY UV;			// uv좌표
	_ENTRY LgtUV;		// 라이트맵 uv좌표	//WORD형이다.
	_ENTRY Face;		//	면
	_ENTRY FaceId;		// 면 인덱스리스트
	_ENTRY VertexId;	// 점 인덱스리스트
	_ENTRY MatGroup;	//	머터리얼 그룹..

	_ENTRY FreeSpare[_FREE_SPARE_CNT];
}_BSP_FILE_HEADER;

typedef struct{
	DWORD f_normal_id;		//	페이스 노멀 아이디
	float d;				//	ax+by+cz = d 의 d
	WORD vertex_num;			//	vertex갯수
	DWORD vertex_start_id;	//	시작 vertex id = uv맵 시작 id = light맵uv 시작 id(몽땅 같다.)
}_BSP_C_FACE;

typedef struct{
	DWORD f_normal_id;		//	노드를 가르는 평면 방정식
	float d;				//	ax+by+cz = d 의 d
	SHORT front;
	SHORT back;
	Vector3s bb_min,bb_max;	//노드의 바운딩 박스 대략 5%정도 더크게...
}_BSP_NODE;


typedef struct{
	WORD v_num;		//해당 점 갯수
	DWORD v_start_id;	//점수...
//	DWORD color;			//	ARGB 알파 폴리곤의 컬러값.
}_BSP_FACE;

#define _VERTEX_ARC_TYPE_CHAR		0x8000
#define _VERTEX_ARC_TYPE_SHORT		0x4000
#define _VERTEX_ARC_TYPE_FLOAT		0x2000
#define _Y_BILLBOARD		0x0800

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
}_BSP_M_GROUP;

typedef struct {
	BYTE type;					//노드의 특성. water라든지...
	WORD face_num;				//bsp 페이스 갯수
	DWORD face_start_id;		//시작bsp face id번호

	WORD m_group_num;			//머터리얼 그룹  갯수
	DWORD m_group_start_id;		//시작 머터리얼 그룹 id번호.

	Vector3s bb_min,bb_max;		//노드의 바운딩 박스
}_BSP_LEAF;




#pragma	pack(pop)

#endif
