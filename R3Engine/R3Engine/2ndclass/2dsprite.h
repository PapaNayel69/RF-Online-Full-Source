#ifndef __2DSPRITE_H__
#define __2DSPRITE_H__

#include "windows.h"
#include "R3D3D8.h"
#include "C_vector.h"

#define _MAX_ACTIONS 16
#define _SELF_SIZE	65535
#define _NOW_STATE	65535

typedef struct{
	DWORD Format;
	DWORD Size;
	WORD OrgXL,OrgYL;		//스프라이트의 원래사이즈 
	WORD XL, YL;			//스프라이트 텍스쳐 사이즈
	void *Tex;	//텍스쳐 포인터
}_FRAME_INFO;

class CSprite{
private:
	BOOL mIsLoadedTexMem;
	DWORD mActions;
	BOOL mPageSPrite;	//페이지 스프라이트인가?
	DWORD mPage;		//하나의 텍스쳐로 여러가지 스프라이트를 제어할경우.
	WORD mPageElementXL;	//페이지 안의 조그마한 텍스쳐의 가로길이..
	WORD mPageElementYL;	//페이지 안의 조그마한 텍스쳐의 세로..
	WORD mFrames[_MAX_ACTIONS];
	DWORD mNowAction;
	DWORD mNowFrame;
	float mAniFrame;	//단위는 1/1000초다
	float mFPS;	//초당 프래임률
	_FRAME_INFO *mFrameInfo[_MAX_ACTIONS];
	char mFileName[128];
public:
	CSprite();
	~CSprite();
	void LoadSprite(char *name);	
	void ReleaseSprite();

	//텍스쳐 큰곳에 여러개의 스프라이트를 사용하는경우. 추가 API
	void InitPageSprite(WORD xl,WORD yl);	//하나의 텍스쳐로 여러가지 스프라이트를 적용할경우.요소요소 사이즈를 지정한다.
	void SetPage(DWORD page);				//해당 텍스쳐의 페이지를 지정한다.
	DWORD GetPageNum(){		return mFrames[0];	}	//페이지 스프라이트 페이지개수
	DWORD GetNowPage(){		return mPage;	}		//현재 지정된 페이지 인덱스를리턴...
	BOOL IsPageSprite(){	return mPageSPrite;	}	//페이지 스프라이트인가?
	WORD GetPageElementXL() { return mPageElementXL; }
	WORD GetPageElementYL() { return mPageElementYL; }
	
	char *GetFileName();
	void RestoreTexMemSprite();	//스프라이트를 텍스쳐 메모리에 올린다.
	void ReleaseTexMemSprite();	//스프라이트를 텍스쳐 메모리에 내린다.

	void SetAction(DWORD action);		//액션을 바꾼다.
	DWORD GetNowActions();				//현재 지정되있는 액션을 알아낸다.
	DWORD GetMaxActions();				//현 스프라이트의 총 액션을 알아낸다.

	void SetFrame(DWORD frame);			//현재 프래임을 지정해준다.
	DWORD GetNowFrames(DWORD action);	// 현재 셋팅되있는 프래임을 알아낸다.
	DWORD GetMaxFrames(DWORD action);	//인자 액션의 총 프래임수를 알아낸다.

	void SetAniAnimationSpeed(DWORD speed);		//초당 프래임수를 넣어준다. 1초 30frame 일때 30
	void RunAnimation();						//애니메이션이 들어갈 경우.

	WORD GetSpriteOrgXL(DWORD action=_NOW_STATE,DWORD frame=_NOW_STATE); //인자가 없으면 현재 액션과 현재 프래임의 사이즈가 리턴된다.
	WORD GetSpriteOrgYL(DWORD action=_NOW_STATE,DWORD frame=_NOW_STATE); //인자가 없으면 현재 액션과 현재 프래임의 사이즈가 리턴된다.

	WORD GetSpriteXL(DWORD action=_NOW_STATE,DWORD frame=_NOW_STATE);
	WORD GetSpriteYL(DWORD action=_NOW_STATE,DWORD frame=_NOW_STATE);

	// 1:1픽셀로 뿌릴때는 앞의 x,y만 넣어준다.DrawSprite(20,40);
	void DrawSprite(LONG x,LONG y,LONG ex=_SELF_SIZE,LONG ey=_SELF_SIZE,DWORD argb=0xffffffff);//반드시 beginscene(),endscene()사이에 있어야됨.
	//uv를 지정하는 스프라이트 API 
	void DrawSpriteUV(LONG x,LONG y,float uv[2][2],LONG ex=_SELF_SIZE,LONG ey=_SELF_SIZE,DWORD argb=0xffffffff);//반드시 beginscene(),endscene()사이에 있어야됨.
	//회전 스프라이트API angle값은 각도 0-360도
	void DrawSpriteRot(LONG x,LONG y,float angle,LONG ex=_SELF_SIZE,LONG ey=_SELF_SIZE,DWORD argb=0xffffffff);//반드시 beginscene(),endscene()사이에 있어야됨.
	//uv를 지정하는 회전 스프라이트 ( by Nadia )
	void DrawSpriteRotUV( LONG x, LONG y, float angle, float uv[2][2], LONG ex=_SELF_SIZE, LONG ey=_SELF_SIZE, DWORD argb=0xffffffff );

	// vertex 지정
	void DrawSpriteVertex( _D3DR3TLVERTEX_TEX1 * pVertex, int nPrimitiveNum );

	void *GetD3D8TexPtr(DWORD action=_NOW_STATE,DWORD frame=_NOW_STATE);//인자가 없으면 현재 액션과 현재 프래임의 사이즈가 리턴된다.
};

//텍스쳐 없는 사각형을 뿌릴경우.. 반투명을 넣고싶으면 ARGB color의 알파값을 조정하자.
void Draw2DRectangle(LONG x,LONG y,LONG ex,LONG ey,DWORD color);//뿌리자.
void Draw2DTexture(LONG x,LONG y,LONG ex,LONG ey,DWORD color,void *tex);//뿌리자.

void Draw2DLineList(Vector2f *line,DWORD line_num,DWORD color);//라인들 한꺼번에 뿌리자.
void Draw2DLine(Vector2f start,Vector2f end,DWORD color);//라인 하나 뿌리자.


#endif