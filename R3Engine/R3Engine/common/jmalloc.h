#ifndef __JMALLOC_H__
#define __JMALLOC_H__




/*
int InitJmalloc(int size);
void ReleaseJmalloc(void);
unsigned char *Jmalloc(int num);	//단순히 메모리만 할당해 준다.

void SetFreePointer();				// 프리포인터를 셋팅해놓는다.
void FreePointer(void *buf);		// 셋팅된 프리포인터로 프리시킨다.
int GetTotalFreeJmallocSize();		// 처음 잡았던 메모리의 크기를 알려준다.(byte 단위)
int GetNowFreeJmallocSize();		// jmalloc된 크기에서 남아 있는 양을 알려준다.(byte 단위)
int GetNowJmallocSize();			// 현재 차지한 메모리 크기를 알려준다..(byte 단위)
*/	//현재 안쓴다..


//---------동적 메모리 관리를 위한 것.
void *Dmalloc(int size);
void Dfree(void *fptr);
int GetDmallocSize();
void *ReAlloc(void *buf,int old_size,int size);

#endif
