#ifndef __R3D3DTEX_H__
#define __R3D3DTEX_H__
#include "d3d8.h"
#include "d3dx8.h"
#include "r3lightmap.h"

#define _R3T_VERSION 1.2f	//R3T포맷 버전..

#define _R3TEXFLAG_NONE_R3T			0x8000	//텍스쳐 파일포맷이 r3t가 아니다.
#define _R3TEXFLAG_DO_NOT_MANAGE	0x4000	//텍스쳐가 독립적이다..
#define _R3TEXFLAG_MIPMAP			0x2000	//텍스쳐가 MIPMAP이 있다.

#define _R3TEX_MANAGE_FLAG_NO_SAME_CHECK	0x1


typedef struct{
	char mName[128];
	DWORD mStartID;
	DWORD mTexNum;
	DWORD mFlag;
	DWORD mSameCnt;		//같은 텍스쳐를 사용하는 카운트... 특정 텍스쳐를 내릴때 유용하다.
}R3Texture;


//   텍스쳐 구조 관련...
BOOL IsAniTex(char *hole);	//애니메이션 텍스쳐인지 알아낸다.
R3Texture *R3GetTexInfoR3T(char *name,DWORD flag);	//R3T텍스쳐 정보를 리턴한다.
_LIGHTMAP **LoadR3TLightMap(R3Texture *tex,D3DFORMAT desire_format);

char *R3GetTexName(int id);	//아이디를 넣어주면 텍스쳐 이름을 알려준다.
int R3GetPreTextureId(char *name);//처음에 로딩할 텍스쳐 아이디를 얻어낸다.실제로 텍스쳐 메모리에 얹지는 않는다.
void R3GetPreAniTextureId(char *path,char *name,LONG *start_id,LONG *num); //로딩할 애니 텍스쳐 시작 아이디및 개수를 얻어낸다.실제로 텍스쳐 메모리에 얹지는 않는다.
void R3ReleasePreTextures();	//로딩할 텍스쳐들을 저장하는 _R3Texture를 없앤다.
void R3ReleaseAllTextures();	//텍스쳐메모리를 전부 클리어한다.
void R3RestoreAllTextures();	//_R3Texture를 참조해서 로딩할 텍스쳐를 텍스쳐메모리에 얹는다.

LPDIRECT3DTEXTURE8 R3GetSurface(int id);	//id를 넣어주면 표면을 리턴해준다.
LPDIRECT3DTEXTURE8 R3GetSurface(char* name);	
int GetNowR3D3DTexCnt();			//현재 텍스쳐 카운트를 리턴
void SetNowR3D3DTexCnt(int cnt);	//무쟈게 주의% 모르고 쓰면 책임못짐...

int GetNowR3TexCnt();			//현재 r3텍스쳐 카운트
void SetNowR3TexCnt(int cnt);	//r3텍스쳐 주의, 모르고 쓰면 책임못짐...

void R3LoadTextureMem(int id);	//특정 텍스쳐를 텍스쳐메모리에 얹는다.
void R3ReleaseTextureMem(int id);	//특정 텍스쳐를 텍스쳐메모리에 내린다.
void LoadR3T(R3Texture *tex);	//r3t파일의 경우 

DWORD GetNowTexMemSize();	//방금 로딩했던 텍스쳐메모리용량을 얻어낸다.
void ResetTexMemSize();


void SetR3TexManageFlag(DWORD mFlag);
DWORD GetR3TexManageFlag();


LPDIRECT3DTEXTURE8 R3LoadDDSAndTextureMem(char *name);	//텍스쳐를 그냥 쓸경우.

//텍스쳐 플래그 관련.
#define _R3D3DTEXTURE_FLAG_NO_LOD	0x80000000	//lod적용하면 안되는 텍스쳐.
void SetR3D3DTexture(DWORD id,DWORD flag);


//--2d스프라이트 관련

void *GetDDSTexFromBuffer(WORD xl,WORD yl,DWORD format,BYTE *buf);
void InitSpriteManager();	//엔진 초기화시 호출.
void ReleaseSpriteManager();
class CSprite;
void RestoreSpriteManager(CSprite *spr);
void ReleaseSpriteManager(CSprite *spr);



//----------------------------- 텍스쳐 렌더 클래스 
class CTextureRender{
    LPD3DXRENDERTOSURFACE  m_pRenderToSurface;
    LPDIRECT3DTEXTURE8     m_pCausticTex;
    LPDIRECT3DSURFACE8     m_pCausticSurf;
    LPDIRECT3DSURFACE8     m_pZBuffer;

	BOOL m_IsLoaded;
	BOOL m_ClearAtOnce;
	BOOL m_bUseZbuffer;
	LPDIRECT3DDEVICE8 m_pd3dDevice;
	DWORD m_dwXsize,m_dwYsize;
public:
	CTextureRender();
	~CTextureRender();
	void ReleaseTexture();
	BOOL CreateTexture(LPDIRECT3DDEVICE8 pd3dDevice,DWORD x_size,DWORD y_size,BOOL use_zbuffer);	//z버퍼를 쓰고 안쓰고 차이가 잇다.
	void ClearFrameBuffer( DWORD color = 0);
	BOOL BeginScene(DWORD sub_view_port=0);	//뷰포트의 일부를 없애는지 확인.
	void EndScene();
	DWORD GetXSize()	{	return m_dwXsize;	};
	DWORD GetYSize()	{	return m_dwYsize;	};
	LPDIRECT3DTEXTURE8 GetD3DTexture()	{	return m_pCausticTex;	};
};

CTextureRender *GetShadowRenderTexture();

#endif