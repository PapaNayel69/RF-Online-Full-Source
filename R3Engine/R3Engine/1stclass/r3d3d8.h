#ifndef __R3D3D8_H__
#define __R3D3D8_H__
#include "d3d8.h"
#include "d3dx8.h"


#define D3DFVF_R3VERTEX_TEX0  (D3DFVF_XYZ |D3DFVF_DIFFUSE)
#define D3DFVF_R3VERTEX_TEX1  (D3DFVF_XYZ |D3DFVF_DIFFUSE| D3DFVF_TEX1)
#define D3DFVF_R3VERTEX_TEX2  (D3DFVF_XYZ |D3DFVF_DIFFUSE| D3DFVF_TEX2 |D3DFVF_NORMAL )

#define D3DFVF_R3TLVERTEX_TEX0 (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
#define D3DFVF_R3TLVERTEX_TEX1 (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

#pragma	pack(push, 1)		//바이트 정렬.

typedef struct {
	union{
	float x;
	float pos[1];
	};
	float y,z;
	DWORD color;
}_D3DR3VERTEX_TEX0;

typedef struct {
	union{
	float x;
	float pos[1];
	};
	float y,z;
	DWORD color;
	float u,v;
}_D3DR3VERTEX_TEX1;

typedef struct {
	union{
	float x;
	float local[1];
	};
	float y,z;
	
	union{
	float nx;
	float normal[1];
	};
	float ny,nz;

	DWORD color;
	float uv[2][2];
}_D3DR3VERTEX_TEX2;


typedef struct {
	float x,y,z,w;
	DWORD color;
}_D3DR3TLVERTEX_TEX0;


typedef struct {
	float x,y,z,w;
	DWORD color;
	float u,v;
}_D3DR3TLVERTEX_TEX1;
#pragma	pack(pop)

#define _LPD3DR3VERTEX void*


#define _SHADER_TEX1	0
#define _SHADER_TEX2	1





//---디바이스관련
HRESULT D3D_R3InitDevice(  LPDIRECT3DDEVICE8 pd3dDevice );
HRESULT D3D_R3ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format );

HRESULT D3D_R3RestoreDevice();
HRESULT D3D_R3InvalidateDevice();
HRESULT D3D_R3DeleteDevice();

LPDIRECT3DDEVICE8 GetD3dDevice();

//----------메시지 관련.. 함수안에 키보드와 마우스관련 함수가 있다.
void D3D_R3MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,LPARAM lParam );


//---
void SetViewPortArea(LONG sx,LONG sy,LONG xl,LONG yl);
void SetFov(float pi);	//fov값을 설정 라디안..
void SetDefaultFov();	//원근 48.29도이다.


void SetTextureRenderTargetFrameBuffer(BOOL state);	//현재 렌더타겟이 프래임버퍼인지를 알려주는 함수.
BOOL IsRenderTargetFrameBuffer();	//현재 프래임버퍼가 렌더 타겟인가?



//유틸..
void SetDefaultFlags();
//y빌보드 메트릭스... 빌보드는 view의 인버스 매트릭스이다.
void GetYBillboardMatrix(D3DXMATRIX *get,FLOAT src[3],FLOAT tar[3]);//	빌보드
void GetZBillboardMatrix(D3DXMATRIX *get,FLOAT src[3],FLOAT tar[3]);//	빌보드
void GetBillboardMatrix(D3DXMATRIX *get,FLOAT src[3],FLOAT tar[3]);//	빌보드

void CleanViewPortStack();
void PushViewPortArea();
void PopViewPortArea();



//풀스크린이펙트 ( FSE ) 효과들.
#define _FSE_MOTION_BLUR		0x80000000		//모션 블로우는 덧씌우기가 가능하다.
#define _FSE_IN_WATER			0x40000000		//물속.


void InitFullScreenEffect();	//최초 디바이스 생성시..
void ReleaseFullScreenEffect();
void FullScreenEffect();
void SetStateFullScreenEffect(DWORD run_id);	//0은 진행중이 아니다. 1은 모션블로어
DWORD GetStateFullScreenEffect();


//쉐도우 텍스쳐
void ShadowBeginScene();
void ShadowEndScene();
LPDIRECT3DTEXTURE8 GetShadowTexture();


#endif