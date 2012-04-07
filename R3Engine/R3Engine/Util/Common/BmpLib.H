#ifndef __BMPLIB_H__
#define __BMPLIB_H__

#include <windows.h>


extern BYTE *LoadBmp(char *name,int *xl,int *yl);
extern BYTE *LoadBmpInverse(char *name,int *xl,int *yl);
void SaveBmp16(char *name,int xl,int yl,WORD *hibuf);
void SaveBmp24(char *name,int xl,int yl,BYTE *buf);

#endif
