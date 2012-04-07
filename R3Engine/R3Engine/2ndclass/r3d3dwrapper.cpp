#define STRICT

#define D3D_OVERLOADS
#include <math.h>
#include <stdio.h>
#include "R3D3DWRAPPER.H"
#include "DXUtil.h"
#include "R3EngineKernel.h"
#include "R3Text.h"
#include "R3DInput.h"
#include "console.h"

//#include "resource.h"
/*

//#include "R3D3d8.h"
#include "CommonUtil.h"
#include "R3Move.h"
#include "R3Util.h"
#include "Level.h"
#include "Console.h"
#include "r3render.h"	//임시 연결.
#include "core.h" 
*/

//-----------------------------------------------------------------------------
// Name: R3D3dWrapper()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
R3D3dWrapper::R3D3dWrapper()
{
    m_strWindowTitle  = "R3BSP Viewer ver 1.00";
    m_bUseDepthBuffer   = TRUE;
	mR3Flag=NULL;
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------

HRESULT R3D3dWrapper::FrameMove()
{
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT R3D3dWrapper::Render()
{
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Restore device-memory objects and state after a device is created or
//       resized.
//-----------------------------------------------------------------------------
HRESULT R3D3dWrapper::RestoreDeviceObjects()
{

//	R3LoadTextTexture();	//텍스트도 텍스쳐메모리를 쓴다. 올리고내려줘야된다.
    if( R3RestoreDevice() == E_FAIL )
 		return E_FAIL;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the device-dependent objects are about to be lost.
//-----------------------------------------------------------------------------
HRESULT R3D3dWrapper::InvalidateDeviceObjects()
{
	
//	R3ReleaseTextTexture();
    if( R3InvalidateDevice() == E_FAIL )
		return E_FAIL;
	return S_OK;

}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------

HRESULT R3D3dWrapper::InitDeviceObjects()
{
/*	
	//--------R3와 d3d를 연결 초기화...
	if( R3InitDevice(m_pd3dDevice,m_d3dpp.BackBufferWidth,m_d3dpp.BackBufferHeight) == E_FAIL )
		return E_FAIL;
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
		,D3DXCOLOR(0,0,0,0), 1.0f, 0L );
	LONG wide_yl = (LONG)(m_d3dpp.BackBufferWidth*(352.0f/640.0f));
//	R3SetViewPort(0,(m_d3dpp.BackBufferHeight-wide_yl)/2,m_d3dpp.BackBufferWidth,wide_yl);
*/
	//--------R3와 d3d를 연결 초기화...
	if( R3InitDevice(m_pd3dDevice,m_d3dpp.BackBufferWidth,m_d3dpp.BackBufferHeight) == E_FAIL )
		return E_FAIL;
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
		,D3DXCOLOR(0,0,0,0), 1.0f, 0L );
	LONG wide_yl = (LONG)(m_d3dpp.BackBufferWidth*(352.0f/640.0f));
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
HRESULT R3D3dWrapper::DeleteDeviceObjects()
{
	/*
	if( R3DeleteDevice() == E_FAIL)
		return E_FAIL;*/
	if( R3DeleteDevice() == E_FAIL)
		return E_FAIL;
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------

HRESULT R3D3dWrapper::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
	/*
    if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
        if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,0) )
            return E_FAIL;
    }*/
	if( R3ConfirmDevice(pCaps,dwBehavior,Format)==E_FAIL )
		E_FAIL;
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message 
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT R3D3dWrapper::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,LPARAM lParam )
{

	//------키보드와 마우스 메시지를 받기위해...
	R3MsgProc( hWnd, uMsg, wParam, lParam );

    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}

HRESULT R3D3dWrapper::PrepareLoop()	// 내부 디바이스의 준비...
{
	if( mR3Flag & _R3_USE_DINPUT )
	{
		ReadR3ImmediateDataMouse();
		ReadR3ImmediateDataKeyboard();
	}
    return CD3DApplication::PrepareLoop();
}

#include "r3input.h"

HRESULT R3D3dWrapper::EndLoop()	//화면 플리핑이 있다.
{
	gMouse.oldx = gMouse.x;	//마우스 좌표 백업..
	gMouse.oldy = gMouse.y;
	gMouse.z = 0;			//휠을 끄자.

	if( gMouse.button & _LEFT_BUTTON_UP )	//마우스 버튼 관련 리작업.
	{
		gMouse.button &= ( ~_LEFT_BUTTON_UP );
		gMouse.button &= ( ~_LEFT_BUTTON_DOWN );
	}
	if( gMouse.button & _RIGHT_BUTTON_UP )
	{
		gMouse.button &= ( ~_RIGHT_BUTTON_UP );
		gMouse.button &= ( ~_RIGHT_BUTTON_DOWN );
	}
	if( gMouse.button & _MIDDLE_BUTTON_UP )
	{
		gMouse.button &= ( ~_MIDDLE_BUTTON_UP );
		gMouse.button &= ( ~_MIDDLE_BUTTON_DOWN );
	}
	for(int i=0; i<256; i++)
	{
		if( gScan[i] & _R3_KEY_UP )
		{
			gScan[i]=0;
		}
	}

    return CD3DApplication::EndLoop();
}

#pragma	comment( lib, "Dinput8.lib" )
#pragma	comment( lib, "dxguid.lib" )

HRESULT R3D3dWrapper::InitR3D3D(HINSTANCE hInstance,DWORD flag)	// 다이렉트 3D초기화 
{
	_R3ENGINE_STATE *state = GetR3State();

    m_dwCreationWidth   = (DWORD)state->mScreenXsize;
    m_dwCreationHeight  = (DWORD)state->mScreenYsize;
    m_bUseDepthBuffer   = TRUE;
    m_dwMinDepthBits    = state->mZbufferBits;
	m_bIsFullScreen = state->mbFullScreen;
	if( state->mbStartCamera )			//시작부터 강제진행 카메라인가??
	{
		SetConsoleStateB(OP_PLAY_CAM,TRUE);
		SetConsoleStateB(OP_VIEW_MAP_INFO,FALSE);
	}
	Create(hInstance);
	mR3Flag = flag;
	if( flag & _R3_USE_DINPUT )
	{
		InitR3DInput(m_hWnd,_KEYBOARD_EXCLUSIVE|_KEYBOARD_IMMEDIATE
			|_MOUSE_EXCLUSIVE|_MOUSE_IMMEDIATE);
//			|_MOUSE_FOREGROUND|_MOUSE_EXCLUSIVE);
	}


    return S_OK;
}
HRESULT R3D3dWrapper::ReleaseR3D3D()		//릴리즈..
{
    Release();	//3D완전 제거.
	ReleaseR3DInput();
    return S_OK;
}

BOOL R3D3dWrapper::IsActive()
{
	return m_bActive;
}
BOOL R3D3dWrapper::IsReady()
{
	return m_bReady;
}
