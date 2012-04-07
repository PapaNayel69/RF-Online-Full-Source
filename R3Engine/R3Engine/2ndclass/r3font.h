#ifndef __R3FONT_H__
#define __R3FONT_H__
///////////////////////////////////////////////////////////////////
//--라이징포스R3엔진용FONT라이브러리입니다.
//--오딘팀:남정수(프로그램파트).
//--완성형 한글입니다.
//--그러나 엔진과 상관없이도 돌아 갑니다.
//
//--1차 포팅 성공(CD3DFont를 그대로 포팅)
//--2차 한글을 텍스처에다가 다 그렸음
//--3차 한글 맵방식으로 성공 각종 자잘 버그 잡음.
//		그러나 현자 자음,모음,한문,특수 문자는 안됨
//--4차 캐시로 변환, 지금하는중
//(이방식은 한자, 자음, 모음 특히 각종 도배에서 퍼포먼스를 발휘함)
//--5차 캐시 저장 성공중
//--6차 캐시 꽉차면 비우기 모드 구현중. IME로 테스트 할려고 함
//--7차 캐시 일단 선형으로 구현해봄
//--8차 캐시 2차원 구현 시작
//--9차 캐시 2차원 512모드
//--10차 IME테스트(한글입력완벽)
//--10차 문제발생 Fullscreen모드에러
//--11차 Fullscreen모드 해결
//--12차 CacheLock API 구현
//--13차 문제 발생 IME에서 글자수가 일정범위 넘어서면 글자 지워짐
//	delete부분 문제 발생
//--14차 해결
//-- 정수캐쉬 루틴 싹 다바꿔 버림... 윈98에서 일부 다운현상해결. 속도 150-300% 향상.
//-- 캐쉬 동적으로 바꿈.
///////////////////////////////////////////////////////////////////
#include <d3d8.h>

//#define _CONSOLE
#ifdef _CONSOLE
#include "cconsoleex.h"
#endif

#define D3DFONT_CENTERED			    0x0100
#define D3DFONT_TWOSIDED				0x0200
#define D3DFONT_FILTERED				0x0400

#define D3DFONT_BOLD					0x0001
#define D3DFONT_ITALIC					0x0002
#define D3DFONT_ZENABLE					0x0004
//#define D3DFONT_UNDERLINE				0x0008
#define D3DFONT_SHADOW					0x0010
#define D3DFONT_OUTLINE					0x0020
//#define D3DFONT_BRIGHT_BLEND			0x0040		//알파 부분을 더욱 밝게하는 효과...
#define D3DFONT_CLIPPING				0x0080		//클리핑 수행. NADIA필요.

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)


//--2D버텍스 구조체 RHW벡터좌표를 사용한다. 변환을 사용하지 않으므로.
struct FONT2DVERTEX
{
	D3DXVECTOR4 p;
	DWORD color;
	float tu, tv;
};

class CR3Font
{
private:
	//--폰트부분
	LPDIRECT3DDEVICE8 m_pd3dDevice;				//--D3D디바이스(없어도 되는데)
	LPDIRECT3DTEXTURE8 m_pTexture;				//--텍스처

	char m_strFontName[85];						//--폰트이름
	float m_fTextScale;							//--스케일값(확대시)
	DWORD m_dwFontHeight;						//--폰트사이즈
	DWORD m_dwFontFlags;						//--플래그 이탤릭,볼드등 처리
	DWORD m_dwOutLineColor;						//--아웃라인 컬러.
	DWORD m_dwSavedStateBlock;					//--저장된 블록값	
	DWORD m_dwDrawTextStateBlock;				//--텍스트 그릴때 블록값

	HBITMAP m_hbmBitmap;
	HDC		m_hDC;
	HFONT	m_hFont;
	DWORD	*m_pBitmapBits;//--비트맵비트포인터

	DWORD mMaxTextureXSize,mMaxTextureYSize;
	DWORD mFontMaxLines,mFontMaxStringLength;
	DWORD mDeleteLine;
	BYTE *mMemPtr;		//대표메모리 얼로케이터.
	DWORD *m_Start;	//시작점.
	DWORD *m_Cnt;		//문자열 길이.
	DWORD *m_StrCnt;	//해당라인별루 카운팅.
	DWORD *m_LineCnt;	//해당 라인의 여분.
	char *m_CacheString;	//실제 비교 문자닷.


	DWORD m_dwFontVertexNum;	//뿌릴 폰트의 버텍스 갯수.
	float m_fClipping[2][2];
	FONT2DVERTEX m_Vertices[64];		//9번 동시 직는거땜시 최대 54개.

	void PrivateInit();
	void PrivateRelease();

	void MemAllocate();
	void MemFree();

	void PrepareDrawText();
	void DrawFullText(FONT2DVERTEX *pVertices);

	void ClearCache();
	//str_leng은 NULL을 제외한 숫자
	void GetBestPosCache(DWORD str_leng,DWORD *x_index,DWORD *y,DWORD *start);
	BOOL IsExistCache(char *ch,DWORD str_leng,DWORD *x_index,DWORD *y);
	void SetCache(char *ch,DWORD str_leng,DWORD x_index,DWORD y,DWORD start);

	HRESULT FillIt(FONT2DVERTEX *pVertices,float xyzw[4], DWORD dwColor, char* strText, DWORD dwFlags);

public:
	
	//--텍스트 그리기 함수(반드시 BeginScene(), EndScene()사이에 있어야 그려짐)
	HRESULT DrawText(float x, float y, DWORD dwColor, char* strText, DWORD dwFlags=0L);
	HRESULT DrawText(float xyzw[4], DWORD dwColor, char* strText, DWORD dwFlags=0L);
	
	// BY Nadia
	// clipping 범위 : 0.0 ~ 1.0
	HRESULT DrawText(float x, float y, DWORD dwColor, char* strText, float clipping[2][2], DWORD dwFlags=0L);

	//--D3D시스템 함수(D3D에서 이름이 같은 각각의 함수서 호출해야한다.)
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice,DWORD tex_xl,DWORD tex_yl );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

	void SetOutLineColor(DWORD color)	{	m_dwOutLineColor = color&0x00ffffff;		};
	DWORD GetOutLineColor()			{	return m_dwOutLineColor;	};
	
	//--생성,소멸(현재는 9포인트 밖에 되지 않는다)
	CR3Font();
	CR3Font( char* strFontName, DWORD dwHeight, DWORD dwFlags=0L);
	~CR3Font();
};
#endif
