#include <windows.h>
#include <assert.h>

#include "R3D3d8.h"
#include "R3D3dtex.h"
#include "core.h"
#include "R3Text.h"
#include "R3Render.h"
#include "R3EngineKernel.h"
#include "Jmalloc.h"
#include "Console.h"
#include "MultiLayer.h"
#include "2dsprite.h"
#include "R3Sound.h"

static BOOL Is_server_mode	= FALSE;
static LONG *Temp			= NULL;		//루프상에서 쓰는 템프버퍼
static BOOL Is_init_R3engine= FALSE;

BOOL IsServerMode()
{
	return Is_server_mode;
}

void ForceFullScreen()	//강제적으로 풀스크린을 만든다.initR3engine이후에 할것.)
{
	GetR3State()->mbFullScreen = TRUE;
}

void InitR3Engine(int use)
{
//	assert( IsInitR3Engine() == FALSE );

	Is_init_R3engine=TRUE;
	if( use == USE_SERVER )
		Is_server_mode=TRUE;
	else
	if( use == USE_TOOL )
	{
//		SetToolMode();//툴용도일때 따로 처리할것이있다.
	}
	InitCore();

	//사운드나 뮤직 플래그 처리..
	SetPlayWaveState(GetR3State()->mSoundOn);
	SetPlayMusicState(GetR3State()->mMusicOn);

	if( !Is_server_mode )	//서버모드가 아니면...
	{
		InitFunctionKey();
		InitSpriteManager();	//스프라이트 메니저 엔진 초기화시 호출.
		InitR3Text();		//텍스트 관련 
	}
}

BOOL IsInitR3Engine(void)
{
	return Is_init_R3engine;
}

void ReleaseR3Engine(void)
{
	if( !Is_server_mode )	//서버모드가 아니면...
	{
		ReleaseFunctionKey();
		ReleaseSpriteManager();
		ReleaseR3Text();	//텍스트..
		ReleaseConsole();
	}
	Is_init_R3engine=FALSE;
	if ( Temp != NULL )
	{
		Dfree(Temp);
		Temp = NULL;
	}
	ReleaseCore();
}

//---엔진 디바이스관련
HRESULT R3InitDevice(  void *pDevice ,int sc_width,int sc_height )
{
	GetR3State()->mScreenXsize = (float)sc_width;
	GetR3State()->mScreenYsize = (float)sc_height;
//	SetViewPortArea(0,0,sc_width,sc_height);
	CleanViewPortStack();
//	PushViewPortArea();
	return D3D_R3InitDevice((LPDIRECT3DDEVICE8)pDevice);
}
HRESULT R3ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior,D3DFORMAT Format )
{
	return D3D_R3ConfirmDevice(pCaps,dwBehavior,Format);
}
HRESULT R3RestoreDevice()
{
//	R3LoadTextTexture();	//텍스트도 텍스쳐메모리를 쓴다. 올리고내려줘야된다.
	return D3D_R3RestoreDevice();
}
HRESULT R3InvalidateDevice()
{
	return D3D_R3InvalidateDevice();
}
HRESULT R3DeleteDevice()
{
	return D3D_R3DeleteDevice();
}

void R3SetViewPort(LONG sx,LONG sy,LONG xl,LONG yl)
{
	SetViewPortArea(sx,sy,xl,yl);
}
void R3GetViewPort(LONG *sx,LONG *sy,LONG *xl,LONG *yl)
{
	_R3ENGINE_STATE *state = GetR3State();
	*sx = (LONG)state->mViewSx;
	*sy = (LONG)state->mViewSy;
	*xl = (LONG)state->mViewXL;
	*yl = (LONG)state->mViewYL;
}


//----------엔진 메시지 관련.. 함수안에 키보드와 마우스관련 함수가 있다.
void R3MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,LPARAM lParam )
{
	D3D_R3MsgProc( hWnd,uMsg,wParam,lParam );
}

DWORD GetFogColor()
{
	return GetR3State()->mFogColor;
}



