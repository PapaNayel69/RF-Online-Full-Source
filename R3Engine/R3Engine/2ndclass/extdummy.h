#ifndef __EXTDUMMY_H__
#define __EXTDUMMY_H__

#define _DUMMY_FLAG_MUSIC	0x1	//음악용.
#define _DUMMY_FLAG_FOG		0x2	//포그용.

typedef struct{
	char mName[32];
	DWORD mFlag;
	Vector3f mBBmin,mBBmax;
	DWORD mID;		//특별한 부분을 위한 고정 아이디.
	union {
	DWORD mdwArgv[4];
	float mfArgv[1];
	};
	float mMat[4][4];
	float mInvMat[4][4];
}_EXT_DUMMY;

class CExtDummy{
private:
	DWORD mMaxNum;
	DWORD mNum;
	_EXT_DUMMY *mDummy;
public:
	CExtDummy();
	~CExtDummy();
	_EXT_DUMMY *GetDummy( DWORD index );
	BOOL LoadExtDummy(char *name);
	void ReleaseExtDummy();
	BOOL IsInBBox(DWORD index,Vector3f pos);				//pos 좌표가 해당더미 인덱스안에 있는가?
	//회전 되지 않은 더미안의 local_pos좌표와 해당 인덱스를 넘겨주면 월드상의 좌표 get_pos를 얻어낸다.
	BOOL GetWorldFromLocal(Vector3f *get_pos,DWORD i,Vector3f local_pos);	
	//월드 상의 좌표를 넘겨주면 더미안의 local_pos좌표를 얻어낸다.
	BOOL GetLocalFromWorld(Vector3f *get_pos,DWORD i,Vector3f world_pos);	

	//해당 플래그를 가지고 있는 더미들 개수와 인덱스 리스트를 얻어낸다.
	void GetDummyList(DWORD flag,DWORD *cnt,DWORD *index_list);	
	DWORD GetTotalNum()		{	return mNum;	};

	//더미박스 렌더링.
	void DrawDummyBBox(DWORD id);
	void DrawAllDummyBBox();
};

#endif