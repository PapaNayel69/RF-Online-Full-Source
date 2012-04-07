#include "CJcache.h"

#define SetCache(num)	(mCacheBitBuf[num>>3]|=(1<<(num&7)))
#define UnSetCache(num)	(mCacheBitBuf[num>>3]&=(~(1<<(num&7))))
#define IsCached(num)	(mCacheBitBuf[num>>3]&(1<<(num&7)))

void CJcache::CreateJcache(DWORD cache_num,DWORD struct_size)		//제한된 캐쉬 갯수를 초기화해야함.
{
	mCacheNum = cache_num;
	mStructSize = struct_size;
	mCacheBitBuf=(BYTE *)Dmalloc((cache_num/8+1)*sizeof(BYTE));
	memset(mCacheBitBuf,0,(cache_num/8+1)*sizeof(BYTE));

	mCacheBuf = (BYTE *)Dmalloc(cache_num*struct_size);
	memset(mCacheBuf,0,cache_num*struct_size);
	mContent = (_CJCACHE_CONTENT*)Dmalloc(sizeof(_CJCACHE_CONTENT)*cache_num);
	memset(mContent,0,sizeof(_CJCACHE_CONTENT)*cache_num);
}

DWORD CJcache::GetCacheLoopID()
{
	DWORD i,j;
	for(i=0; i<(mCacheNum/8+1); i++)
	{
		if( mCacheBitBuf[i] != 0xff )	//빈곳이 있다.
		{
			for(j=0; j<8; j++)
			{
				if( !(mCacheBitBuf[i] & ( 1<<j )) ) //비었다.
				{
					return (i*8+j);
					//return mCacheBuf[mStructSize*(i*8+j)];
				}
			}
		}
	}
	return -1;
}

void CJcache::DestroyJcache()
{
	if( mCacheBitBuf )
	{
		Dfree(mCacheBitBuf);
		mCacheBitBuf=NULL;
	}
	if( mCacheBuf )
	{
		Dfree(mCacheBuf);
		mCacheBuf=NULL;
	}
	if( mContent )
	{
		Dfree(mContent);
		mContent=NULL;
	}

	if( mCacheList.size() )
	{
		/*
		for( JCache::iterator i = mCacheList.begin(); i != mCacheList.end(); i++ )
		{
			i = mCacheList.erase( i );
		}*/
		mCacheList.clear();
	}
}

CJcache::CJcache()
{
	mCacheNum=0;
	mCacheBuf=NULL;
}
CJcache::~CJcache()
{
	if( mCacheBuf )
		Dfree(mCacheBuf);
}

LONG CJcache::HitCache(DWORD id,void **get_cache_ptr)	//캐쉬를 사용하는 히트, 리턴값이 TRUE일경우 이전에 캐쉬된 거다.
{
	_CJCACHE_CONTENT *content;
	int j=0;

	for( JCache::iterator i = mCacheList.begin(); i != mCacheList.end(); i++,j++ )
	{
		content = *i;
		
		if( content->mID == id )
		{
			*get_cache_ptr = &mCacheBuf[mStructSize*content->mLoopID];
			mCacheList.erase(i);		//지우고 가장 앞에다 둔다.
			mCacheList.push_front( content );
			return _HITCACHE_RET_OLD;
		}
	}
	if( j >= mCacheNum )	//만약 넘친다면 가장 오래된거를 리턴한다. false일경우는 반드시 지우고 사용한다.
	{
		i=mCacheList.end();
		i--;
		content=*i;
		*get_cache_ptr = &mCacheBuf[mStructSize*content->mLoopID];

		content->mID = id;
		mCacheList.erase(i);		//지우고 젤 앞으로 해놓자.
//		mCacheList.push_back( content );
		mCacheList.push_front( content );
		return _HITCACHE_RET_DEL_AND_NEW;  //지운다음 새로 삽입해야함.
	}
	
	// 새로운 버퍼를 보낸다. 삽입해야된다.
	DWORD l_id = GetCacheLoopID();
	if( l_id ==-1 )
		Error("캐쉬 에러!","치명적임...");

	mContent[l_id].mID = id;
	mContent[l_id].mLoopID = l_id;
	SetCache(l_id);

	mCacheList.push_front( &mContent[l_id] );

	*get_cache_ptr = &mCacheBuf[mStructSize*l_id];
	memset(*get_cache_ptr,0,mStructSize);
	return _HITCACHE_RET_NEW;
}

BOOL CJcache::DelCache(DWORD id)						//캐쉬된 버퍼를 지운다. 구조체 안의 것들을 지우기위해 GetCache통해 사용한후 이함수 호출.
{
	_CJCACHE_CONTENT *content;

	for( JCache::iterator i = mCacheList.begin(); i != mCacheList.end(); i++)
	{
		content = *i;
		
		if( content->mID == id )
		{
//			get_cache_ptr = &mCacheBuf[mStructSize*content->mLoopID];
			i = mCacheList.erase( i );
			UnSetCache(content->mLoopID);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CJcache::GetCache(DWORD id,void **get_cache_ptr)	//캐쉬된 버퍼를 얻는다. 없으면 FALSE를 리턴
{
	_CJCACHE_CONTENT *content;

	for( JCache::iterator i = mCacheList.begin(); i != mCacheList.end(); i++)
	{
		content = *i;
		
		if( content->mID == id )
		{
			*get_cache_ptr = &mCacheBuf[mStructSize*content->mLoopID];
			return TRUE;
		}
	}
	return FALSE;
}


DWORD CJcache::GetCacheCnt()  //캐쉬카운트변수
{
	_CJCACHE_CONTENT *content;

	DWORD j=0;
	for( JCache::iterator i = mCacheList.begin(); i != mCacheList.end(); i++)
		j++;

	return j;
}


