#ifndef __R3ALPHA_H__
#define __R3ALPHA_H__

#include <windows.h>
//#include "RevisitedRadix.h"
#include "C_vector.h"

#define _MAX_ALPHA_FACE		8192	//한화면에서 최대뿌릴수 있는 알파...

#define _ALPHA_TYPE_M_GROUP		0x80000000
#define _ALPHA_TYPE_CORONA		0x40000000
#define _ALPHA_TYPE_ENTITY		0x20000000

class CAlpha{
private:
//	RadixSorter mSortBase;	//서트클래스 생성.
	DWORD mAlphaFaceCnt,*mAlphaFace;
	SHORT *mAlphaFaceZ;
	DWORD *mTempAlphaFace;
	SHORT *mTempAlphaFaceZ;
	int mAlphaSize;
	void *mBsp;
	void CheckAlphaAlloc(void);	//알파 alloc
public:	
	CAlpha();
	~CAlpha();

	void InitAlpha(void* bsp);

	void LoopInitAlphaStack(void);	//알파 리스트 스텍 초기화
	DWORD GetAlphaFaceCnt();	//알파 페이스갯수를 리턴.
	DWORD *GetAlphaFace();		//알파 페이스버퍼를 리턴.
	DWORD *SortAlphaStack(Vector3f camera);	//여지껏 알파스택에 쌓인것을 정렬한뒤 정렬된버퍼를 넘겨준다.
	void SetAlphaStack(WORD group_id);
	
	void SetCoronaStack(WORD group_id);	//코로나다..
	void SetAlphaEntityStack(WORD entities_id);//엔티티다.
};




#endif