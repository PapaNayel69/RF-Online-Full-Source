#ifndef __R3BLOCKBSP_H__
#define __R3BLOCKBSP_H__

typedef struct OptObject{
	Vector3f bb_min,bb_max;	//바운딩 박스
	SHORT mtlid;
	SHORT lgtid;		//	//컴포짓 라이트맵 아이디도 된다.
	DWORD start_id;	//면 스타트인덱스
	DWORD f_cnt;	//면갯수 
	OptObject *next;
	WORD type;
//	bool no_merge;
//	bool loop_ani;
	WORD object_id;		//오브젝트 id= 실제 id+1 현재는 다이나믹 오브젝트를 위해 사용.
	int i;	//index번호 디버그용...
}_OptObject;

class R3BlockBsp{
private:
	void MergeOptObject();

public:
	_MESH *MakePMeshBlockBsp(_MESH *p_mesh,_MESH *r_p_mesh);

	_POLYFACE **mPolyFace;	// 면들의 포인터..
	DWORD *mPolyWhatObj;	// 면들이 어떤 오브젝트를 가지고 있나...
	int mPolys;				// 총 면수..
	_OptObject *mOptObject;
	int mOptObjNum;		//총 옵티마이즈 오브젝트수..
	R3BlockBsp(int face_num);
	~R3BlockBsp();
	void PrepareBlockBsp(_MESH *mesh,_MATERIAL *material);
	void TestFree();
};

WORD GetDynamicObjectId(_MESH *mesh,int n_id);
WORD GetObjectId(_MESH *mesh,int d_id);		//다이나믹 오브젝트 인덱스를 넣어서 몇번 오브젝트인가를 알아낸다.


#endif