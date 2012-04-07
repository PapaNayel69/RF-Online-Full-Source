#ifndef _R3D3DWRAPPER_H__
#define _R3D3DWRAPPER_H__

#include <d3Dx8.h>
#include "R3D3DApp.h"

#define _R3_USE_DINPUT 0x1
//-----------------------------------------------------------------------------
// Name: class R3D3dWrapper
//-----------------------------------------------------------------------------
class R3D3dWrapper : public CD3DApplication
{
protected:
	DWORD mR3Flag;	//엔진 관련 플래그...
    HRESULT ConfirmDevice( D3DCAPS8*, DWORD, D3DFORMAT );
    HRESULT InitDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render();
    HRESULT FrameMove();
public:
	//----- 외부에서 사용하는 인터페이스....
	LPDIRECT3DDEVICE8 GetD3dDevice8(){	return m_pd3dDevice;	};	//d3d8 디바이스 
    R3D3dWrapper();
	HRESULT PrepareLoop();	// 내부 디바이스의 준비...
	HRESULT EndLoop();	//화면 플리핑이 있다.
	HRESULT InitR3D3D(HINSTANCE hInstance,DWORD flag);	// 다이렉트 3D초기화 
	HRESULT ReleaseR3D3D();		//릴리즈..
    virtual HRESULT RestoreDeviceObjects();		//해상도 제셋팅 들어갈때 로딩부분
    virtual HRESULT InvalidateDeviceObjects();	//해상도 제셋팅 들어갈때 릴리즈부분
	virtual LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//----- 
	BOOL IsActive();	//창이 활성화 되었는가...
	BOOL IsReady();		//aPP준비가 되었는가..
};

#endif