#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "Level.h"


//---콘솔 명령어---	op code
#define OP_NON_OP				-1
#define OP_LIST					1
#define OP_OPEN					2
#define OP_FOV					3
#define OP_OPEN_CAM				4
#define OP_ALL_EVENT_ID			5
#define OP_PLAY_CAM				6
#define OP_SCREEN_SHOT			7
#define OP_VIEW_MAP_INFO		8
#define OP_CAPTURE_CAM			9
#define OP_FLY					10
#define OP_SHOW_CFACE			11
#define OP_SHOW_LEAF_BOX		12
#define OP_GAMMA				13
#define OP_FLY_SPEED			14
#define OP_SHOW_LIGHTMAP		15
#define OP_SHOW_MAT_BOX			16

void InitConsole();
void ReleaseConsole();
void ProgressConsole(CLevel *level);

BOOL GetConsoleStateB(int op);				// 콘솔 bool 타잎 op의 참거짓을 판정...
void SetConsoleStateB(int op,BOOL b_value);	// 콘솔 bool 타잎 op의 참거짓 넣는다.
void InsertConsoleStringQ(char *buf);		// 콘솔창에 문자를 띄운다.
void SetOpStack(int op);		//스택 명령강제 명령이다.


//--------functinon키 
void InitFunctionKey(char *name=NULL);
void ReleaseFunctionKey();
void ProgressFunctionKey();

#endif