#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include <windows.h>
#include "R3material.h"
#include "entity.h"



class CSkyBox{
private:
	BOOL mIsSkyLoad;
/*
	_ENTITY_FILE_HEADER mHeader;
 	int mVetexBufferSize;	//총 버텍스버퍼 사이즈
	void* mStaticVertexBuffer;	//정적버텍스 버퍼 
	void* mStaticIndexedBuffer;	//정적인덱스 버퍼 
//	BYTE *mStaticAlloc;		//정적 할당 (파일상 그대로 메모리 올릴 때)
	_ENTITY_M_GROUP *mMatGroup;
	Vector2f *mOrgUV;
*/
	CEntity	mEntity;				//스카이박스 엔티티 
	_ENTITY_M_GROUP *mMatGroup;		//엔티티거를 받아온 포인터.
	int mMatNum;				//머터리얼 개수.
	int mMatGroupNum;
	short mMatGroupSort[512];		//머터리얼 그룹을 순서대로 뿌려주기 위한 버퍼.
public:
	CSkyBox::CSkyBox();
	CSkyBox::~CSkyBox();

	void LoadSkyBox(char *filename);
	void ReleaseSkyBox();
//d3dmatrix를 인자로 넘겨줘야한다. 현재 R3Render.cpp와  R3D3d8에서만 
//다이렉트 3D로 다루려고 하기때문에 인자가 필요하다.
	void DrawSkyBox(void *view_mat);	
	int GetUsedVertexBufferSize();
};


									


#endif