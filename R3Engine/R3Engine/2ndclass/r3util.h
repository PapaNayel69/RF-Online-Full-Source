#ifndef _R3UTIL_H_
#define _R3UTIL_H_

#include "C_vector.h"
#include "d3dx8math.h"
#include "d3d8.h"
#include "r3move.h"
#include "r3render.h"

#define _R3PI 3.1415926535

#define PiToAngle(a)  (((double)(a)*360.0)/(2.0*_R3PI))
#define AngleToPi(a)  ((2.0*_R3PI*(double)(a))/360.0)

float GetDist(Vector3f Pos,Vector3f Tar);	//두 벡터간의 거리를 구한다.
float GetYAngle(Vector3f pos,Vector3f tar);	// 현재위치(pos)와 다음위치(tar) 각도(360)을 리턴한다.
BOOL GetYAngle(float *get_yangle, Vector3f Pos,Vector3f Tar);	// 각도(360)을 리턴한다. 두위치가 너무 가까우면 FALSE다 
float GetXAngle(Vector3f Pos,Vector3f Tar);	// 각도(360)을 리턴한다.

// A 와 B를 잇는 선분의 A로부터 dist만큼 떨어진 좌표를 얻는 함수다.
void GetPosByDistFromATOB(Vector3f *get,Vector3f src,Vector3f tar,float dist);


BOOL GetTransformVertex(Vector4f *xyzw,Vector3f pos);	//pos값을 스크린좌표로 가져온다.
//역 트랜스폼 trans_pos[0]은 스크린x, trans[1]은 스크린y, trans[2] 희망 z좌표
void GetInverseTransformVertex( float get[3],float trans_pos[3]);

void GetViewMatrix(D3DXMATRIX *get_matrix);
void GetProjectMatrix(D3DXMATRIX *get_matrix);
//메인 다이렉션 라이트 벡터를 바꿔준다.
void GetMainDirLight(Vector3f *get_pos);
void SetMainDirLight(Vector3f pos);
void GetMatLightFromColor( D3DLIGHT8 *light,D3DMATERIAL8 *mtrl,DWORD color,float density=1.0f );

void GetBBoxRotate(Vector3f *get_bb_min,Vector3f *get_bb_max,Vector3f bb_min,Vector3f bb_max,float y_angle);
void GetHeadMatrix(D3DXMATRIX *get_mat,R3Camera *camera,float yangle,LONG mouse_x,LONG mouse_y);

//벡터를 매트릭스로 얻어주는 유틸.
//활경우 기준벡터는 {0,0,-1}맥스에서 프론트의경우.
//피라든지 그런거는 {0,1,0}맥스에서 top의경우 
void GetMatrixFromVector(float get_mat[4][4],Vector3f vec,Vector3f axis);
//void GetMatrixFromAtoB(Vector3f a,Vector3f b);	//a가 b를 향해 갈때의 매트릭스를 구한다.
void GetMatrixFromAtoB(float get_mat[4][4],Vector3f a,Vector3f b);	//a가 b를 향해 갈때의 매트릭스를 구한다.


//z버퍼 영역을 클리어.
void Clean2DRectangleZbuffer(LONG x,LONG y,LONG ex,LONG ey);
// 스크린 좌표 영역과바운딩 박스로 이동값을 얻어오는 함수. 2d영역에 3D오브젝트를 영역에 정확히 맞게뿌릴때 쓴다.
void Get2DTo3DTranslation(Vector3f *get,Vector3f bb_min,Vector3f bb_max
					  ,float sx,float sy,float ex,float ey,float scale=1.0f);

//------------------------ 특수 처리 함수들...
//지진 효과
void R3GetQuakeMatrix(float mat[4][4]);	//Quake 매트릭스를 만들어주자.
void R3GetQuakeVector(Vector3f pos);	//Quake 백터를 만들어주자.
void R3EnvironmentQuake(float time,float density);


//다이나믹라이트...매루프마다 넣어주어야 한다...
void ClearDynamicLight();
BOOL InsertDynamicLight(Vector3f pos,float diameter,DWORD color,void *texture=0,BOOL magic_light=FALSE,DWORD blend_type=_BLEND_BRIGHT);
DWORD GetDynamicLightNum();
void GetDynamicLight(DWORD id,Vector3f pos,float *diameter,DWORD *color,void **texture,DWORD *blend_type);
BOOL IsMagicLight(DWORD id);	//매직라이트는 마법진이다.
void GetDynamicLightBBox(DWORD id,Vector3f *bbmin,Vector3f *bbmax);


//쉐도우를 위한 라이트 위치 셋팅.
void SetLightForShadow(Vector3f pos);//쉐도우를 위한 라이트 위치...
void GetLightForShadow(Vector3f pos);//쉐도우를 위한 라이트 위치...
void PushLightForShadow();//쉐도우를 위한 라이트 위치 스택
void PopLightForShadow();//쉐도우를 위한 라이트 위치 스택

//파일 이름으로 파티클인가 확인하기..
BOOL IsParticle(char *name);


// 카메라 좌표를 얻어주는 유틸리티...
void GetCameraPos(Vector3f *get_pos);	//카메라 월드상의 위치백터.
float GetDensityFromPos(Vector3f pos);	//인자의 위치와 카메라와의 거리로 강도를 결정해준다. 효과음이나 퀘이크사용시 쓴다.

//페이드인 페이드 아웃
BOOL DrawFadeScreen();	//리턴값은 페이드인 된 상태일때 TRUE,진행중이나 페이드 아웃된상태는 FALSE
BOOL IsFadeOut();		//페이드아웃 된 상태인가?
BOOL IsFadeIn();		//페이드인된 상태인가?
BOOL IsFading();		//페이드가 진행중인가?
void SetFadeOut(DWORD color,float time,float density=1.0f);		//해당되는 컬러와 페이드되는 시간.	density는 마지막 해당색의 강도이다.
void SetFadeIn(DWORD color,float time,float density=1.0f);			//해당되는 컬러와 페이드되는 시간.
void SetFadeFactor(DWORD color,float alpha,void *tex=NULL);	//페이드 인아웃 직접 제어.알파가 1에 가까울수록 해당 페이드 색이 강하다.

// 하늘 페이드인 아웃.
void SetFadeSky(DWORD color,float fade_time);		//해당되는 컬러와 페이드되는 시간.
BOOL DrawFadeSKy();

//텍스쳐 페이드인 아웃.
void SetFadeTex(DWORD color,float fade_time,void *tex);		//해당되는 컬러와 페이드되는 시간.
BOOL DrawFadeTex(void);

//스크린샷
void FramebufferToBMP(char *name);
void FramebufferToJPG(char *name);
DWORD PixelFromFramebuffer(short cl_x,short cl_y);

//사각형박스를 그린다.
void DrawTestBox(float hor,float ver,float height,DWORD color);
void CleanZbuffer(float sx,float sy,float w,float h);	//사각영역의 zbuffer를 클리어한다.


//블로링을 위한 버텍스 버퍼들.
void CreateBlurVBuffer(DWORD width,DWORD height);
void DrawBlurVBuffer(DWORD level);
void ReleaseBlurVBuffer();


#endif


