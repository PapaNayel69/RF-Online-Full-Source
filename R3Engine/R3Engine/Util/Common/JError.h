
#ifndef __JERROR_H__
#define __JERROR_H__

#include <windows.h>
#include <windowsx.h>


extern void error_malloc(char *);
extern void error_fatal(char *,char *);
extern void error_fatal_printf(char *,char *,...);
extern void Error(char *mes,char *mes1);
extern void Warning(char *mes,char *mes1);
extern void PutMessage(char *mes,char *mes1);

extern int No_warning;


#endif