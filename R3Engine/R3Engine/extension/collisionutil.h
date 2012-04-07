#ifndef _COLLISIONUTIL_H_
#define _COLLISIONUTIL_H_

#include "C_Vector.h"
#include <windows.h>

class CCollision{
private:
	Vector4f *mFNormalList;//페이스 노멀.
	Vector4f *mVList;//버텍스 리스트
	int mFaceNum;

public:
	CCollision();
	~CCollision();

	void InitColision(Vector4f *vlist,int face_num);
	void ReleaseCollision();
	BOOL CheckCollision(Vector3f char_pos,Vector3f old_pos,Vector3f new_pos);
};

#endif