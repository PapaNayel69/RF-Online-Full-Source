#ifndef __CJCACHE_H__
#define __CJCACHE_H__

#include <Windows.h>
#include <List>
#include "jmalloc.h"
#include "jerror.h"

/*
	Jcache ver 0.5
	사용빈도와는 상관없이 이 캐쉬는 가장 최근에 사용했던 것이 오래있을 우선권이 있는 
	방법으로 사용되는 캐쉬이다.
	빈번하게 매루프마다 계속 사용되는것 보다는 가끔사용하는 곳에 좋다.
*/

typedef struct{
	DWORD mID;	//아이디..
	DWORD mLoopID;	//루프 아이디..  i값이지..
//	DWORD mCnt;	//사용 카운트
}_CJCACHE_CONTENT;

using namespace std;
typedef list<_CJCACHE_CONTENT*>		JCache;



#define _HITCACHE_RET_NEW	1			//이전에 없어서 새로 생성해야된다.
#define _HITCACHE_RET_DEL_AND_NEW	0	//현재 넘겨주는거를 하나 지우로 거기에다 새로 생성해야된다.
#define _HITCACHE_RET_OLD	-1			//이전에 존재하므로 바로 쓴다.

class CJcache{
public:
	CJcache();
	~CJcache();

	void CreateJcache(DWORD cache_num,DWORD struct_size);		//제한된 캐쉬 갯수를 초기화해야함. hitcache()에서 id는 0 ~ cache_num-1까지 값이다.
	void DestroyJcache();	//지우기전에 주의할점은 사용하는 구조체안의 malloc들은 알아서 클리어 해야함.

	BOOL GetCache(DWORD id,void **get_cache_ptr);	//캐쉬된 버퍼를 얻는다. 없으면 FALSE를 리턴
	BOOL DelCache(DWORD id);						//캐쉬된 버퍼를 지운다. 구조체 안의 것들을 지우기위해 GetCache통해 사용한후 이함수 호출.
	virtual LONG HitCache(DWORD id,void **get_cache_ptr);	//캐쉬를 사용하는 히트, 리턴값은 _HITCACHE_RET 참조.

	DWORD GetCacheCnt();	//캐쉬카운트갯수

	void BeginCacheList();	//캐쉬의 리스트를 얻기위한 함수.
	DWORD GetNextCache();	//Begin이후의 순차적인 리턴값은 아이디...  
	void EndCacheList();	//캐쉬의 리스트를 얻기위한 함수.
private:
	BYTE *mCacheBuf;
	BYTE *mCacheBitBuf;	//비트 플래그캐쉬
	DWORD mCacheNum;	//캐쉬 제한 갯수.
	DWORD mStructSize;	//초기화시 받아주는 구조체사이즈
	JCache mCacheList;
	_CJCACHE_CONTENT *mContent;
	DWORD GetCacheLoopID();
};

#endif