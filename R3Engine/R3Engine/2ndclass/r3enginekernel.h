#ifndef __R3ENGINEKERNEL_H__
#define __R3ENGINEKERNEL_H__

#include "core.h"


#define USE_CLIENT	0
#define USE_SERVER	1
#define USE_TOOL	2


//-- 초기화 및 끝내기 관련
void InitR3Engine(int use = USE_CLIENT);	//엔진을 서버용으로 사용하려면 Use_server를 넣는다.
void ReleaseR3Engine();
BOOL IsInitR3Engine();
void ForceFullScreen();	//강제적으로 풀스크린을 만든다.InitR3engine이후에 할것.)

//---프래임버퍼 클리어및 렌더링 시작과 끝
void R3ClearFrameBuffer();
void DrawLogo();	//로고를 뿌린다.
BOOL R3BeginScene();
void R3EndScene();

//_R3ENGINE_STATE *GetR3State();
extern _R3ENGINE_STATE stState;
#define GetR3State() (&stState)

DWORD GetFogColor();

//-- 서버모드
BOOL IsServerMode();	//현재 서버모드인가?

//---엔진 디바이스관련
HRESULT R3InitDevice(  void *pDevice ,int sc_width,int sc_height );
HRESULT R3ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior,D3DFORMAT Format );

HRESULT R3RestoreDevice();
HRESULT R3InvalidateDevice();
HRESULT R3DeleteDevice();

//뷰포트 설정함수 ....
void R3SetViewPort(LONG sx,LONG sy,LONG xl,LONG yl);
void R3GetViewPort(LONG *sx,LONG *sy,LONG *xl,LONG *yl);
void PushViewPortArea();
void PopViewPortArea();

//----------엔진 메시지 관련.. 함수안에 기본 키보드와 마우스관련 함수가 있다.
void R3MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,LPARAM lParam );


#endif