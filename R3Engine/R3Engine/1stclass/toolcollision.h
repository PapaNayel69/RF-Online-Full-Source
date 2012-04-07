#ifndef __TOOLCOLLISION_H__
#define __TOOLCOLLISION_H__

#include "windows.h"
#include "c_vector.h"

//-----충돌 라인(폴리곤) 속성 관련 나중에 이 속성으로 동적인 충돌라인 첵크 여부를 결정한다.
#define _LINE_ATTR_SELECT	0x80000000
#define _LINE_ATTR_FREEZE	0x40000000

#pragma	pack(push, 1)		//바이트 정렬.
typedef struct{
	float v[3];
}_TOOL_COL_POINT;

typedef struct{
	DWORD attr;		//라인속성		//하위 바이트는 이벤트 아이디로 쓸 예정이다. (나중에 동적인 출동첵크일때..사용해라..)
	WORD start_v,end_v;	//점 인덱스
	float height;
	WORD front,back;	//라인 인덱스
}_TOOL_COL_LINE;		//툴라인이 충돌 폴리곤과 같다.

typedef struct{
	DWORD start_id;
	WORD line_num;
}_TOOL_COL_LEAF;
#pragma	pack(pop)

class  CBsp;

class CToolCollisionFace{
private:
public:
	_TOOL_COL_POINT *mColPoint;
	_TOOL_COL_LINE *mColLine;
	Vector4f *mNormal;
	Vector3f *mVNormal;
//	_TOOL_COL_LEAF *mColLeaf;
//	WORD *mColLineId;
	int mCOLPOINTMAXCNT;
	int mCOLLINEMAXCNT;
	int mCOLVNORMALMAXCNT;
	int mColPointCnt;
	int mColLineCnt;
	int mVNormalCnt;
	int mSelectVertex;		//선택된 점...
	BOOL mOnePointCreate;
	WORD GetSearchLineIndex(WORD v_id);
	BOOL mbContinueLine;
public:
	WORD mBeforeVId;	//선택된 점인덱스나 에디트중인 

	CToolCollisionFace();
	~CToolCollisionFace();
// 입출력부분
	BOOL SaveCF(char *name);
	BOOL LoadCF(char *name);


	BOOL LoadColFaceFromBSP(CBsp *bsp);

// 충돌 페이스와의 충돌점을  얻어낸다.
	BOOL GetCrossPoint(Vector3f from,Vector3f at,Vector3f *get);

	int GetCrossFace(Vector3f from,Vector3f at);	//충돌 폴리곤 인덱스를 알아낸다.
	int GetCrossFace(int x,int y);					//같은건데 마우스 좌표를 넘기는 인자.
// 에디트 Tool에 관한것.	
	BOOL GetContinueLine();
	void SetContinueLine(BOOL state);
	void SelectAll();	//전부다 선택한다.
	void UnSelectAll();	//전부다 Un선택한다.
	void SelectFace(int id);	//한 페이스만 선택한다.
	BOOL SelectFaceGroup(int id);	//관련된 페이스 그룹을 선택한다.

	//---프리즈,언프리즈 관련 
	void FreezeFromSelect();	//선택된 페이스를 프리즈 시키고 선택을 푼다.
	void UnFreezeAll();	//모두 언프리즈 한다.
	void UnFreezeFace(int id);	//한 페이스만 언프리즈 한다.
	BOOL UnFreezeFaceGroup(int id);	//관련된 페이스 그룹을 선택한다.

	
	WORD CreateVertex(Vector3f v);	//리턴값이 생성된 id값이다.
	BOOL DeleteVertex(WORD id);		//지우기 성공시 true
	void DeleteSelectFace();		//선택되 면들을 제거한다.
	BOOL GetVertex(Vector3f *get,WORD id);
	int GetColLineCnt();
	int GetColPointCnt();
	int GetBeforeId();
	void SetBeforeId(WORD v_id);	//이전 점을 유저가 등록한다.
	float GetFaceHeght(WORD f_id);	//높이를 구해낸다.

	void AdjustVertex();		//노는 버텍스를 없앤다.
	void AdjustFace();			//링크안된 면을 고친다.
	void AdjustSelectFaceHeight();	//선택 면 높이조정.
	void AdjustFixFaceLink();		//링크 잘못된 면들을 고친다.

	WORD CreateLine(WORD v_id,BOOL b_ontinue_line);	//리턴값이 생성된 id값이다.
	BOOL DeleteLine(WORD id);		//지우기 성공시 true
	void DeleteCurLine();	
	void DrawCollisionPoly();	//충돌 면을 뿌린다.
	void DrawCollisionPoint();	//충돌 점을 뿌린다.
	WORD IsExistPoint(Vector3f pos);	//리턴값이 0이면 없음

	void GetLineNormal(Vector4f *get,_TOOL_COL_LINE *col_line);

	void GetPathFinder(Vector3f from,Vector3f at,Vector3f get[16],DWORD *cnt);
	void GetPath(Vector3f from,Vector3f at);
	int GetPathCrossPoint(Vector3f from,Vector3f at,Vector3f *get,int now_f1,int now_f2);
	BOOL EdgeTest(Vector3f cross,int id);

	void GetFaceFrontPoint(Vector3f *get_temp,int v_id);
	BOOL IsExistSelfPoint(int id,int face);
	void FixLinkFace();	//페이스 링크 에러 첵크해서 고친다.
	void MoveSelectFace(BOOL yaxis,BOOL xzaxis,LONG xdiff, LONG ydiff);
	void MoveSelectVertex(BOOL yaxis,BOOL xzaxis,LONG xdiff, LONG ydiff);
	void SetSelectVertex(WORD id);
	WORD GetSelectVertex();
	void UnSetSelectVertex();

	int GetFinalPath(void *node,Vector3f from,Vector3f get[16]);//최종 최적화 패스를 리턴한다.

	void MakeVnormal();	//현재 셋팅되있는 페이스들에서 버텍스노멀을 구한다.
	void MakeFnormal();	//현재 셋팅되있는 페이스노멀을 구한다.

	//bsp 세이브를 위한 정보가져오기.
	void LoadCollisonInfoInBSP(CBsp *bsp,void *v_add_bsp);
	void ReleaseCollisonInfoInBSP(void *v_add_bsp);	//릴리즈도 할것.
};

void SetHeight(int su);
int GetHeight();


#endif