#ifndef _LIGHTMAP_H__
#define _LIGHTMAP_H__

#include "world.h"

#include "R3BlockMat.h"


//#define _MAX_TEXTURE_SIDE	256
#define _MAX_TEXTURE_SIDE	2048
#define _FIT_TEXTURE_SIDE	256		//라이트맵 기본 사이즈...

class CGenLightMap{	//라이트맵 1개에 들어갈것들...
private:

public:
	int mLEdge[_MAX_TEXTURE_SIDE];	// 왼쪽 에지테이블
	int mREdge[_MAX_TEXTURE_SIDE];	// 오른쪽 에지테이블
	int mIndex;
	int mCnt;		// 하나의 라이트맵에 몇개의 머터리얼 블록 라이트맵이 들어가는가...
	int mBufSize;	//현재라이트맵 버퍼사이즈 실제사이즈가 아니라 단순히 가로세로사이즈다.

	CGenLightMap *mNext;	//링크드 리스트
	_LIGHT_MAP mLgtMap;	//라이트맵 구조체..

	CGenLightMap();
	~CGenLightMap();
	void AdjustLightMap(_POLYFACE *face,DWORD *index,DWORD *face_index,int cnt);
	BOOL FillLightMap(_LIGHT_MAP *src,_POLYFACE *face,float *lu,float *lv);
};

CGenLightMap *GetLightmap(void);
int GetLightmapCnt(void);	//라이트맵 총개수 

//void MakeLightmapsFromPatchs(_MESH *mesh,_MATERIAL *mat);
void MakeLightmapsFromPatchs(_MESH *mesh,_MATERIAL *mat,R3BlockBsp *blockmat,_MATERIAL *com_mat);
void DeleteLightmapsFromPatchs(_MESH *mesh);
void SaveLightMap(_MESH *mesh,_MATERIAL *com_mat);	//컴포짓 머터리얼로 라이트맵을 같이 셋팅.

#endif