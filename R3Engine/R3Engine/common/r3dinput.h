#ifndef __R3DINPUT_H__
#define __R3DINPUT_H__

#include "windows.h"

#define _KEYBOARD_EXCLUSIVE				0x00000001
#define _KEYBOARD_FOREGROUND			0x00000002
#define _KEYBOARD_IMMEDIATE				0x00000004
#define _KEYBOARD_DISABLE_WINDOWSKEY	0x00000008

#define _MOUSE_EXCLUSIVE				0x00000010
#define _MOUSE_FOREGROUND				0x00000020
#define _MOUSE_IMMEDIATE				0x00000040


HRESULT InitR3DInput(HWND hWnd,DWORD flag);
void ReleaseR3DInput();
HRESULT ReadR3ImmediateDataMouse();
HRESULT ReadR3ImmediateDataKeyboard();
BOOL IsUseDirectInput();	//현재 다이렉트 인풋을 쓰고있는가.


#endif