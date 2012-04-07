#ifndef __R3RENDER_H__
#define __R3RENDER_H__

#include "C_Vector.h"
#include "R3D3D8.h"
#include "R3Bsp.h"
#include "entity.h"
#include "SkyBox.h"


//---------블렌딩 type 방식...
#define _BLEND_NONE			0
#define _BLEND_OPPA			1
#define _BLEND_DEFAULT		2
#define _BLEND_BRIGHT		3

#define _BLEND_INV_DEFAULT	5
#define _BLEND_INV_BRIGHT	6

//#define _BLEND_BACK_BRIGHT	7
#define _BLEND_BACK_BRIGHT	8

#define _BLEND_LIGHTMAP		10
#define _BLEND_INV_LIGHTMAP		11

#define _BLEND_ONLY_TRANSPARENCY		13		//단지 전체 투명..
#define _BLEND_SHADOW	14		//쉐도우에 쓸거.


void DrawOneMatGroup(CVertexBuffer *vb,CIndexBuffer *ib,int min_vp,int base_index_v,int v_num,int face_num);
void DrawOneMatGroup(CVertexBuffer *vbuffer,_BSP_MAT_GROUP *m_group);
void DrawLightMapGroup(CVertexBuffer *vbuffer,_BSP_MAT_GROUP *m_group);


void DrawPrimitiveUPPoly(int num,void *vbuffer);
void DrawPrimitiveUPLine(int num,void *vbuffer);
void DrawPrimitiveUPPoint(int num,void *vbuffer);

void DrawPrimitiveUPTex1(void *matP,int face_num,DWORD alpha_type
						 ,_LPD3DR3VERTEX buf,DWORD vbuf_type=D3DPT_TRIANGLELIST);	//미리 버텍스버퍼에 저장해야된다.

void R3DrawLine(Vector3f a,Vector3f b,DWORD color);	//직접 라인그리기..
void R3Draw2DLine(Vector2f a,Vector2f b,DWORD color);
void R3Draw2DLineList(Vector2f *line,DWORD line_num,DWORD color);//라인들 한꺼번에 뿌리자.


//----폴리곤 뿌리기에 관련된것
//void DrawGroupStack();					//그룹 스택에 쌓인것을 죄 렌더링하자...
void DrawSkyBoxStack(void *view_mat);				//스카이박스 스택에 쌓인것을 죄 렌더링하자... view매트릭스를 인자로..
void DrawAlphaStack(CBsp *bsp,Vector3f camera);	//알파 스택에 쌓인것을 죄 렌더링하자...

//////////////////////////////////////////////
void MultiTexOn();
void MultiTexOff();

void BlendOn(int type);
void BlendOff();

void DrawRect(float sx,float sy,float w,float h,float u[2],float v[2],void *tex,DWORD color);

void Draw2DSpriteSub(float sx,float sy,float w,float h,int tex_id,DWORD color);
void Draw2DSprite(float sx,float sy,float w,float h,int tex_id,DWORD color);
void Draw2DSprite(float sx,float sy,float w,float h,void *tex,DWORD color);
void Draw2DSprite(float sx,float sy,float w,float h,float u[2],float v[2],void *tex,DWORD color);
void Draw2DSprite( _D3DR3TLVERTEX_TEX1 * pVertex, int nPrimitiveNum, void *pTex );

void Draw2DSpriteRot(float sx,float sy,float w,float h,float angle,void *tex,DWORD color);
void Draw2DSpriteRot(float sx,float sy,float w,float h,float angle,float u[2],float v[2],void *tex,DWORD color);

void DrawIndexedTex1VertexUP(void *matP,int face_num,int index_num,DWORD alpha_type,WORD *index,_LPD3DR3VERTEX buf);	//미리 버텍스버퍼에 저장해야된다.

void DrawTLVertex(int matID,int face_num,DWORD alpha_type,_LPD3DR3VERTEX *buf=NULL);	//미리 버텍스버퍼에 저장해야된다.
void DrawTLVertex(void *matP,int face_num,DWORD alpha_type,_LPD3DR3VERTEX buf,DWORD vbuf_type=D3DPT_TRIANGLELIST);	//미리 버텍스버퍼에 저장해야된다.
void DrawIndexedTLVertex(void *matP,int face_num,int index_num,DWORD alpha_type,WORD *index,_LPD3DR3VERTEX buf);	//미리 버텍스버퍼에 저장해야된다.


void DrawFullScreenLava(DWORD xsize,DWORD ysize,void *tex,DWORD color,float amplitude=0.5f,float speed=1.0f);

//프로젝트 쉐도우 관련..
void SetProjectShadow(Vector3f char_pos,void *tex,float intensity,DWORD blur,float scale,float add_pos);//project 쉐도우 들어가기전에 호출할것.
void UnSetProjectedShadow();	//proejct 쉐도우 실행하고 나서 호출할것.
void DrawStripOneMatGroup(_BSP_MAT_GROUP *m_group);
void DrawShadow(_BSP_MAT_GROUP *m_group);

//다이나믹 라이팅..
void SetDynamicLight(Vector3f d_light_pos,float diameter,DWORD color,void *tex,BOOL multi_tex=TRUE,DWORD blend_type = _BLEND_BRIGHT);
void UnSetDynamicLight();


#endif