#include <malloc.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "jmalloc.h"
#include "JError.h"

#define _MAXMALLOCNUM 1024

unsigned char *Allmemory=NULL,*Startpointer=NULL,*Totalmemory=NULL,*Nowoffset=NULL;
int Jmalloccnt=0,Jmallocbuffer[_MAXMALLOCNUM];

int InitJmalloc(int size)
{
    Allmemory=(BYTE *)malloc(size);
    Totalmemory=(BYTE *)((int)Allmemory+size);
    if(Allmemory==NULL)
    {
        Error("can't alloc momory!","");
    }
    return((int)Allmemory);
}
int GetTotalFreeJmallocSize()		// 처음 잡았던 메모리의 크기를 알려준다.
{
	return((int)Totalmemory-(int)Allmemory);
}
int GetNowFreeJmallocSize()		// jmalloc된 크기에서 남아 있는 양을 알려준다.
{
	return ((int)Totalmemory-(int)Allmemory)-(int)Nowoffset;
}
int GetNowJmallocSize()		// 현재 차지한 메모리 크기를 알려준다..(byte 단위)
{
	return (int)Nowoffset;
}


void ReleaseJmalloc()
{
    if( Allmemory )
        free(Allmemory);
}

unsigned char *Jmalloc(int num)
{
//    if(Allmemory == NULL || Totalmemory < (BYTE *)((unsigned int)Allmemory+(unsigned int)Nowoffset+num))
    if(Allmemory == NULL || ((unsigned int)Totalmemory-(unsigned int)Allmemory) < ((unsigned int)Nowoffset+num))
    {
		char buf[256]="";
//		extern int debug_light;
//		sprintf(buf,"required %d,%d",num,debug_light);
		sprintf(buf,"required %d",num);
        Error("memory overflow!",buf);
    }
	Startpointer=(BYTE *)((unsigned int)Allmemory+(unsigned int)Nowoffset);
	Nowoffset+=num;

    return(Startpointer);
}

void SetFreePointer()		// 프리포인터를 셋팅해놓는다.
{
    Jmallocbuffer[Jmalloccnt]=((int)Allmemory+(int)Nowoffset);
	Jmalloccnt++;
	if(Jmalloccnt>= _MAXMALLOCNUM)
	    Error("_MAXMALLOCNUM overflow!","");
}

void FreePointer(void *buf)		// 셋팅된 프리포인터로 프리시킨다.
{
	int i;

	for(i=0;i<Jmalloccnt; i++)
	{
		if((int)buf == Jmallocbuffer[i])
		{
            buf=NULL;
            Jmalloccnt=i;
			Nowoffset = (BYTE *)(Jmallocbuffer[Jmalloccnt]-(int)Allmemory);
			return;
		}
	}
	char sbuf[256];
	sprintf(sbuf,"%h",buf);
	Error(sbuf,"<-error! can't jfree pointer");
}

//---------------------------------- 동적인 메모리 관리 -------------------------------
int Dmalloc_size = 0;
void *Dmalloc(int size)
{
	if(size == 0)
		return NULL;
	Dmalloc_size += size;
	BYTE *ptr=(BYTE *)malloc(size+4);
	((int *)ptr)[0]=size;
	return((void *)(ptr+4));
}
int GetDmallocSize()
{
	return Dmalloc_size;
}
void Dfree(void *fptr)
{
	if( fptr == NULL )
		return;
	Dmalloc_size-=((int *)fptr)[-1];
	free(((char *)fptr-4));
}

void *ReAlloc(void *buf,int old_size,int size)
{
	BYTE *temp = (BYTE *)Dmalloc(size);
	memcpy(temp,buf,old_size);
	Dfree(buf);
	return (void *)temp;
}

