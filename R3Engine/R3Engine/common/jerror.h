
#ifndef __JERROR_H__
#define __JERROR_H__

#include <windows.h>
#include <windowsx.h>


void error_malloc(char *);
void FatalError(char *,char *);		//치명적인 에러일경우 사용해라.
void error_fatal_printf(char *,char *,...);
void Error(char *mes,char *mes1);
void Warning(char *mes,char *mes1);
void PutMessage(char *mes,char *mes1);

//에러시 실행할 메시지 함수 포인터를 셋팅해준다.
//워닝시 실행할 메시지 함수 포인터를 셋팅해준다.
void SetErrorMessageProc(void (*proc)(char *));
void SetWarningMessageProc(void (*proc)(char *));

extern int No_warning;


#endif