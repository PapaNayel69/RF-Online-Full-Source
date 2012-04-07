#ifndef _COMMONUTIL_H_
#define _COMMONUTIL_H_

#include "windows.h"

void StripEXT(char *name);			//확장자를 없앤다.
void StripPath(char *name);			//패스를 없앤다.
void StripName(char *name);			//이름을 없앤다.
BOOL IsExistFile(char *name);		//지금 파일이 존재하는가?
BOOL GetNowPath(char *get_path);	//현재 실행하고 있는 패스를 얻어온다.
void GetFinalPath(char *get,char *path);	//패스중에 마지막 패쓰를 얻어온다.
void StripFinalPath(char *name);			// 마지막 패스를 없앤다.
void GetEXT(char *get,char *name);		//확장자를 얻어낸다.

int GetFileSize(char *name);		//파일 사이즈를 알아낸다.

int MaxFixFloatToInt(float su);
int MinFixFloatToInt(float su);

//--- 타이머 관련	단위는 1.0이 1초 
void R3CalculateTime();			//루프마다 넣줘야 시간이 측정 된다.
float R3GetLoopTime();			//이전루프와 한루프의 도는  시간을 건진다.
void R3SetMinFPS(float min_fps);	//최소 프래임 레이트를 정의한다.
void R3SetTime(float time);			//시간을 재 설정...
float R3GetTime();					//현재 시간을 알아낸다.
void R3SetLoopTime(float time);	//만약을 위해 쓴다. 그렇지만 쓸일이 없어야한다.

LONG R3GetToggle30fps();		//30fps로 토글 해준변수다.
LONG R3GetToggle15fps();		//30fps로 토글 해준변수다.
LONG R3GetToggle7fps();			//7fps로 토글을 해준다.


float GetFPS();		//프래임레이트를 리턴한다.
void SaveBMP(char *name,int xl,int yl,int pitch,BYTE *argb);
void SaveJPG(char *name,int xl,int yl,int pitch,BYTE *argb);


#endif