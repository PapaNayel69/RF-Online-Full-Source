#ifndef __R3INPUT_H__
#define __R3INPUT_H__

#include  "windows.h"
#include  "winuser.h"

#define VK_0	0x30
#define VK_1	0x31
#define VK_2	0x32
#define VK_3	0x33
#define VK_4	0x34
#define VK_5	0x35
#define VK_6	0x36
#define VK_7	0x37
#define VK_8	0x38
#define VK_9	0x39

#define VK_A	0x41
#define VK_B	0x42
#define VK_C	0x43
#define VK_D	0x44
#define VK_E	0x45
#define VK_F	0x46
#define VK_G	0x47
#define VK_H	0x48
#define VK_I	0x49
#define VK_J	0x4A
#define VK_K	0x4B
#define VK_L	0x4C
#define VK_M	0x4D
#define VK_N	0x4E
#define VK_O	0x4F
#define VK_P	0x50
#define VK_Q	0x51
#define VK_R	0x52
#define VK_S	0x53
#define VK_T	0x54
#define VK_U	0x55
#define VK_V	0x56
#define VK_W	0x57
#define VK_X	0x58
#define VK_Y	0x59
#define VK_Z	0x5A


#define VK_WAVE		0xC0

#define VK_LEFT_FACE		219
#define VK_RIGHT_FACE		221


typedef struct{
	int x,y,z;		//x,y는 화면의 절대값이고 z는 휠을 사용하기때문에 상대값이다.
	int oldx,oldy,oldz;
	DWORD button;
	short extX,extY;	//확장용x,y좌표 용도는 장언일에게.
	BOOL LDBClick;		//왼쪽 더블클릭... TRUE 확인하구 꺼줘야한다.
	BOOL RDBClick;		//오른쪽 더블클릭... TRUE 확인하구 꺼줘야한다.
}_MOUSE;

#define _LEFT_BUTTON_DOWN		0x1		//마우스 버튼 0 
#define _MIDDLE_BUTTON_DOWN		0x2		//마우스 버튼 2
#define _RIGHT_BUTTON_DOWN		0x4		//마우스 버튼 1


#define _LEFT_BUTTON_UP			0x8			//마우스 버튼 0 
#define _MIDDLE_BUTTON_UP		0x10		//마우스 버튼 2
#define _RIGHT_BUTTON_UP		0x20		//마우스 버튼 1

#define _R3_KEY_UP			0x0800
//마우스 확장 버튼.
#define _MOUSE_BUTTON3_DOWN		0x1000	
#define _MOUSE_BUTTON4_DOWN		0x4000	
#define _MOUSE_BUTTON5_DOWN		0x0200

#endif

//class CR3Input:public CD3DApplication//

extern _MOUSE gMouse;
extern int gScan[256];
extern int gAsci[256];
extern int gGetKey[256];	//키값을 받을경우....
extern int gScanExt[256];

int GetCharFromKey();
