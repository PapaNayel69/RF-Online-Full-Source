#include <math.h>
#include "CommonUtil.h"
#include "R3D3D8.h"
#include "R3D3Dtex.h"
#include "core.h"
#include "Jerror.h"
#include "Jmalloc.h"
#include "R3Render.h"
#include "R3Util.h"
#include "R3Math.h"
#include "R3Material.h"
#include "R3Lightmap.h"
#include "RenderFlag.h"
#include "Collision.h"
//#include "RevisitedRadix.h"
#include "MultiLayer.h"

#include "RenderParticle.h"



void MultiTexOn()
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
//	pd3dDevice->SetVertexShader( D3DFVF_R3VERTEX_TEX2 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT ); 
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
}

void MultiTexOff()
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
//	pd3dDevice->SetTexture( 1,NULL);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
}
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_BLENDFACTORALPHA );	

void SetLightMap(LONG lgt_id)
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	if( lgt_id == -1)
		pd3dDevice->SetTexture( 1, NULL );
	else
		pd3dDevice->SetTexture( 1,(LPDIRECT3DTEXTURE8)GetLightMapSurface(lgt_id));
}

void DetailTextureOnStage2(_R3MATERIAL *mat)	//2번텍스쳐 스테이지 유닛으로 디테일 텍스쳐를 킨다.
{
	if( GetR3State()->mTextureUnitNum < 3 || !GetR3State()->mDetailTexture )
		return;

	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	D3DXMATRIX tex_mat;		//텍스쳐 좌표..
	MatrixIdentity( tex_mat.m );
	tex_mat.m[0][0]=mat->m_fDetailScale*8;
	tex_mat.m[1][1]=mat->m_fDetailScale*8;

	pd3dDevice->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	pd3dDevice->SetTransform(D3DTS_TEXTURE2,&tex_mat );

	pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 0 );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT ); 
	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
//	pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
//	pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	pd3dDevice->SetTexture( 2,(LPDIRECT3DTEXTURE8)R3GetSurface(mat->m_iDetailSurface));

/*
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	D3DXMATRIX tex_mat;		//텍스쳐 좌표..
	MatrixIdentity( tex_mat.m );
	tex_mat.m[0][0]=mat->m_fDetailScale*8;
	tex_mat.m[1][1]=mat->m_fDetailScale*8;
extern void SetDetailTexVS(D3DXMATRIX *tex_mat);
	SetDetailTexVS(&tex_mat);

	static DWORD stDetailTexPixelShader=0;
	if( !stDetailTexPixelShader )
	{
extern HRESULT LoadCreateShader(char *strFilePath, const DWORD* pDeclaration, DWORD Usage, int ShaderType, DWORD* pHandle);
		HRESULT result = LoadCreateShader(".\\System\\shader\\DetailTex.pso"
			,0,0,2, &stDetailTexPixelShader);
	}
	pd3dDevice->SetPixelShader( stDetailTexPixelShader );
*/
}

void DetailTextureOffStage2()	//2번텍스쳐 스테이지 유닛으로 디테일 텍스쳐를 킨다.
{
	if( GetR3State()->mTextureUnitNum < 3 || !GetR3State()->mDetailTexture )
		return;
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

//	pd3dDevice->SetPixelShader( 0 );

//	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2 );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
}

void DrawPrimitiveUPPoint(int num,void *vbuffer)
{
	if( num == 0 )
		return;
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	pd3dDevice->SetVertexShader( D3DFVF_R3VERTEX_TEX0 );
	pd3dDevice->DrawPrimitiveUP(D3DPT_POINTLIST,num,vbuffer,sizeof(_D3DR3VERTEX_TEX0));
}
void DrawPrimitiveUPLine(int num,void *vbuffer)
{
	if( num == 0 )
		return;
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	pd3dDevice->SetVertexShader( D3DFVF_R3VERTEX_TEX0 );
	pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST,num,vbuffer,sizeof(_D3DR3VERTEX_TEX0));
}

void DrawPrimitiveUPTLLine(int num,void *vbuffer)
{
	if( num == 0 )
		return;
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	pd3dDevice->SetVertexShader( D3DFVF_R3TLVERTEX_TEX0 );
	pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST,num,vbuffer,sizeof(_D3DR3TLVERTEX_TEX0));
}

void DrawPrimitiveUPTex2(int num,void *vbuffer)
{
	if( num == 0 )
		return;
	GetD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,num,vbuffer,sizeof(_D3DR3VERTEX_TEX2));
}
void DrawPrimitiveUPTex1(int num,void *vbuffer)
{
	if( num == 0 )
		return;
	GetD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,num,vbuffer,sizeof(_D3DR3VERTEX_TEX1));
}
void DrawIndexedPrimitiveUPTex1(int num,int index_num,WORD *index,void *vbuffer)
{
	if( num == 0 )
		return;
	GetD3dDevice()->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,index_num,num,index,
		D3DFMT_INDEX16,vbuffer,sizeof(_D3DR3VERTEX_TEX1));
}
void DrawPrimitiveUPTex1Strip(int num,void *vbuffer)
{
	if( num == 0 )
		return;
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,num,vbuffer,sizeof(_D3DR3VERTEX_TEX1));
}


void DrawPrimitiveTLTex1(int num,void *vbuffer)
{
	if( num == 0 )
		return;

	GetD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,num,vbuffer,sizeof(_D3DR3TLVERTEX_TEX1));
}

void DrawIndexedPrimitiveTLTex1(int num,int index_num,WORD *index,void *vbuffer)
{
	if( num == 0 )
		return;
	GetD3dDevice()->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,index_num,num,index,
		D3DFMT_INDEX16,vbuffer,sizeof(_D3DR3TLVERTEX_TEX1));
}

void BlendOff()
{
	GetD3dDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    GetD3dDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}
/*
#include "r3input.h"
#include "r3text.h"

DWORD src_blend=1,dest_blend=1;

	if( gAsci[VK_R] )
	{
		gAsci[VK_R]=0;
		src_blend++;
		if( src_blend > 13 )
			src_blend--;
	}
	if( gAsci[VK_F] )
	{
		gAsci[VK_F]=0;
		src_blend--;
		if( src_blend < 1 )
			src_blend++;
	}
	if( gAsci[VK_T] )
	{
		gAsci[VK_T]=0;
		dest_blend++;
		if( dest_blend > 13 )
			dest_blend--;
	}
	if( gAsci[VK_G] )
	{
		gAsci[VK_G]=0;
		dest_blend--;
		if( dest_blend < 1 )
			dest_blend++;
	}

*/

#include "r3input.h"
#include "r3text.h"

DWORD src_blend=1,dest_blend=1;

void BlendOn(int type)
{
/*
//	pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
//	pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
버텍스 alpha를 하고 싶을때는
   pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
   pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
   pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
   또는 
   pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
단순 oppa만을 할경우는 
   pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
*/
	if( !type )	//아무 형태도 아니다...
		return;	
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	switch(type&0x0000000f)	//나중에 인버스한것도 넣자.
	{
		case _BLEND_OPPA:
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
//			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_BLENDFACTORALPHA );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//			GetRenderState();
//			GetTextureStage();

			break;
		case _BLEND_DEFAULT:
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			break;
		case _BLEND_BRIGHT:
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		case _BLEND_BACK_BRIGHT:
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			break;
		case _BLEND_INV_DEFAULT:
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			break;
		case _BLEND_INV_BRIGHT:
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_INVSRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
			break;

		case _BLEND_LIGHTMAP:
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
			break;
		case _BLEND_INV_LIGHTMAP:
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
//			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, src_blend );
//			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, dest_blend );
			break;
		case _BLEND_ONLY_TRANSPARENCY:
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case _BLEND_SHADOW:
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			break;
		default:
			return;
	}
}


//inline void 
#define	D3DRGBA(r,g,b,a)    (((DWORD)(r*255)<<16)|((DWORD)(g*255)<<8)|((DWORD)(b*255))|((DWORD)(a*255)<<24))

#include "math.h"
void DrawLensFlare(CBsp *bsp)	//렌즈플레어를 뿌린다.
{
	_R3ENGINE_STATE *state= GetR3State();
#define _LENS_FLARE_TEX_NUM	16	//8개혹은 16개로 해야 한다.
	if( state->mLensFlareTex==-1 )	//렌즈플레어 텍스쳐가 없으면.
		return;
	_D3DR3TLVERTEX_TEX1 v_buf[1048];
	int cnt=0,i;

//----렌즈플레어 위치 트랜스폼.
	D3DXMATRIX mattemp = state->mMatView;
	Vector4f base,middle;
	mattemp.m[3][0]=0;
	mattemp.m[3][1]=0;
	mattemp.m[3][2]=0;
	mattemp=mattemp*state->mMatProj;
	int clip = TransformVertex( base, mattemp.m,state->mLensFlarePos);
	if( !clip )		//화면 밖의 영역에 있다...
		return;
	//----- 렌즈플레어와 맵과의 충돌첵크를 한다.
	Vector3f lens_far,eye;
	Vector3fCopy(state->mLensFlarePos,lens_far);
	Normalize(lens_far);
	Vector3fMul(lens_far,state->mFar,lens_far);
	eye[0] = state->mInvMatView.m[3][0];
	eye[1] = state->mInvMatView.m[3][1];
	eye[2] = state->mInvMatView.m[3][2];
	if( bsp->IsCollisionFace(eye,lens_far) )
		return;

	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	int tex_mod=(_LENS_FLARE_TEX_NUM/4);


	middle[0] = state->mScreenXsize/2;
	middle[1] = state->mScreenYsize/2;
	float texel_v=(1/(float)(_LENS_FLARE_TEX_NUM*2))*state->mScreenXsize;
	float texel_u=texel_v;
	Vector3f normal;
	normal[0] = middle[0]-base[0];
	normal[1] = middle[1]-base[1];
	normal[2] = 0;
	DWORD alpha;
	if( !(normal[0] == 0 && normal[1] == 0) )	//노멀벡터계산.
	{
		float temp = 1.0f - (Vector3fDist(normal)/(state->mScreenXsize/2.0f));
		temp = max(0,temp);
		alpha = (DWORD)(temp*255);
	}
	else
		alpha = 255;
	if( alpha ==0 )	//렌더링 할필요가없다.
		return;
	alpha = alpha<<24;
	DWORD color = 0x00ffffff|alpha;
//	Vector3fMul(normal,1/2.8f,normal);
	Vector3fMul(normal,(1/2.8f)*0.66f,normal);
	Vector3fMul(normal,(1/(float)(_LENS_FLARE_TEX_NUM*100))*state->mScreenXsize,normal);

	for(i=0; i<_LENS_FLARE_TEX_NUM; i++)
	{
		v_buf[cnt].x = base[0]+(i)*normal[0]-texel_u*state->mLensFlareScale[i];
		v_buf[cnt].y = base[1]+(i)*normal[1]-texel_v*state->mLensFlareScale[i];
		v_buf[cnt].z = base[2];
		v_buf[cnt].w = base[3];
		v_buf[cnt].u = ((i)%4)/4.0f;
		v_buf[cnt].v = ((i)/4)/(float)tex_mod;
		v_buf[cnt].color=color;
		cnt++;
		v_buf[cnt].x = base[0]+(i)*normal[0]-texel_u*state->mLensFlareScale[i];
		v_buf[cnt].y = base[1]+(i)*normal[1]+texel_v*state->mLensFlareScale[i];
		v_buf[cnt].z = base[2];
		v_buf[cnt].w = base[3];
		v_buf[cnt].u = ((i)%4)/4.0f;
		v_buf[cnt].v = ((i+4)/4)/(float)tex_mod;
		v_buf[cnt].color=color;
		cnt++;
		v_buf[cnt].x = base[0]+(i)*normal[0]+texel_u*state->mLensFlareScale[i];
		v_buf[cnt].y = base[1]+(i)*normal[1]+texel_v*state->mLensFlareScale[i];
		v_buf[cnt].z = base[2];
		v_buf[cnt].w = base[3];
		v_buf[cnt].u = ((i)%4+1)/4.0f;
		v_buf[cnt].v = ((i+4)/4)/(float)tex_mod;
		v_buf[cnt].color=color;
		cnt++;

		v_buf[cnt].x = base[0]+(i)*normal[0]-texel_u*state->mLensFlareScale[i];
		v_buf[cnt].y = base[1]+(i)*normal[1]-texel_v*state->mLensFlareScale[i];
		v_buf[cnt].z = base[2];
		v_buf[cnt].w = base[3];
		v_buf[cnt].u = ((i)%4)/4.0f;
		v_buf[cnt].v = ((i)/4)/(float)tex_mod;
		v_buf[cnt].color=color;
		cnt++;
		v_buf[cnt].x = base[0]+(i)*normal[0]+texel_u*state->mLensFlareScale[i];
		v_buf[cnt].y = base[1]+(i)*normal[1]+texel_v*state->mLensFlareScale[i];
		v_buf[cnt].z = base[2];
		v_buf[cnt].w = base[3];
		v_buf[cnt].u = ((i)%4+1)/4.0f;
		v_buf[cnt].v = ((i+4)/4)/(float)tex_mod;
		v_buf[cnt].color=color;
		cnt++;
		v_buf[cnt].x = base[0]+(i)*normal[0]+texel_u*state->mLensFlareScale[i];
		v_buf[cnt].y = base[1]+(i)*normal[1]-texel_v*state->mLensFlareScale[i];
		v_buf[cnt].z = base[2];
		v_buf[cnt].w = base[3];
		v_buf[cnt].u = ((i)%4+1)/4.0f;
		v_buf[cnt].v = ((i)/4)/(float)tex_mod;
		v_buf[cnt].color=color;
		cnt++;
	}
	GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
	BlendOn(_BLEND_BRIGHT);
	DrawTLVertex(R3GetSurface(state->mLensFlareTex),cnt/3,FALSE,(_LPD3DR3VERTEX)(&v_buf[0]) );	//미리 버텍스버퍼에 저장해야된다.
}



void DrawAlphaStack(CBsp *bsp,Vector3f camera)	//알파 스택에 쌓인것을 죄 렌더링하자...
{
	//코로나 부분 정의
#define _MAX_CORONA	256
	DWORD corona_cnt;
	WORD corona_id[_MAX_CORONA];		//머터리얼 아이디다..

	DWORD *alpha_face,alpha_face_cnt,i,*sorted;
	_R3MATERIAL*	mat		= GetMainMaterial();
	_BSP_MAT_GROUP *m_group = bsp->GetMatGroup();
	float l_time;
	_R3ENGINE_STATE *state; 
	D3DMATERIAL8 mtrl;
	DWORD old_type;
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

	alpha_face_cnt=bsp->mAlpha.GetAlphaFaceCnt();
	if( alpha_face_cnt == 0 )
		goto ALPHA_NEXT_PROCEDURE;		//뿌릴일 없다.

	//정렬부텀하고...
	sorted = bsp->mAlpha.SortAlphaStack(camera);
	if( sorted == NULL )	
		goto ALPHA_NEXT_PROCEDURE;		//뿌릴일 없다.
	
	alpha_face=bsp->mAlpha.GetAlphaFace();

	corona_cnt=0;

//	MakeLayerMatrix(&mat[now_mtlid]);	//머터리얼을 통한 UV Matrix를  만든다.

	old_type = -1;
	for(i=0; i<alpha_face_cnt; i++)
	{
//		DWORD alpha_buf = alpha_face[sorted[alpha_face_cnt-1-i]];
		DWORD alpha_buf = sorted[alpha_face_cnt-1-i];
	
//		if( (WORD)(alpha_buf&0x0000ffff) == 0)
//			continue;
		if( alpha_buf & _ALPHA_TYPE_CORONA )
		{
			corona_id[corona_cnt++] = (WORD)(alpha_buf&0x0000ffff);
		}
		else
		if( alpha_buf & _ALPHA_TYPE_M_GROUP )
		{
			if( old_type != _ALPHA_TYPE_M_GROUP )
			{
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				MultiTexOn();
			}
			old_type = _ALPHA_TYPE_M_GROUP;
			if( bsp->IsInViewFrustum((WORD)(alpha_buf&0x0000ffff)))
				bsp->RenderIndepentMatGroup((WORD)(alpha_buf&0x0000ffff));
		}
		else
		if( alpha_buf & _ALPHA_TYPE_ENTITY )
		{
			if(	old_type == _ALPHA_TYPE_M_GROUP )
				MultiTexOff();
			old_type = _ALPHA_TYPE_ENTITY;
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			bsp->RenderOneEntityRender((WORD)(alpha_buf&0x0000ffff));	//맵 엔티티하나 실레로 렌더링.
//			bsp->RenderIndepentMatGroup((WORD)(alpha_buf&0x0000ffff));
		}
	}
	//-----마지막으로 코로나를 뿌린다.

	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	Vector3f eye;
	state= GetR3State();
	eye[0] = state->mInvMatView.m[3][0];
	eye[1] = state->mInvMatView.m[3][1];
	eye[2] = state->mInvMatView.m[3][2];
	l_time=R3GetLoopTime();

	memset(&mtrl,0,sizeof(D3DMATERIAL8));
	mtrl.Diffuse.a=1;
	mtrl.Diffuse.r=1;
	mtrl.Diffuse.g=1;
	mtrl.Diffuse.b=1;
	mtrl.Emissive.a=1;
	mtrl.Emissive.r=1;
	mtrl.Emissive.g=1;
	mtrl.Emissive.b=1;
	pd3dDevice->SetMaterial( &mtrl );
	pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );  
	pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );

#define _CORONA_DIST 300	//80
	for(i=0; i<corona_cnt; i++)
	{
		int c_id=corona_id[i];
		Vector3f temp;
		Vector3fSub(eye,m_group[c_id].Origin,temp);
//		if(temp > 80*10) //80미터 넘는거는 꺼트린다.... 
		if( (Vector3fDist(temp)) > _CORONA_DIST*10 || bsp->IsCollisionFace(eye,m_group[c_id].Origin) )	//두벡터사이에 폴리곤이 충돌이되나? 코로나...
		{	//꺼져야 한다.
			m_group[c_id].CoronaAlpha-=l_time*2;
			if(	m_group[c_id].CoronaAlpha < 0 )
				m_group[c_id].CoronaAlpha=0;
		}
		else
		{	//켜지자.
			m_group[c_id].CoronaAlpha+=l_time*2;
			if(	m_group[c_id].CoronaAlpha > 1.0 )
				m_group[c_id].CoronaAlpha=1;
		}
		if( m_group[c_id].CoronaAlpha == 0)
			continue;
		mtrl.Diffuse.a=m_group[c_id].CoronaAlpha;
		pd3dDevice->SetMaterial( &mtrl );
		
		bsp->RenderIndepentMatGroup(c_id);
	}

ALPHA_NEXT_PROCEDURE:

	pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
	pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );  

	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	MultiTexOff();
	BlendOn(_BLEND_BRIGHT);
	DrawLensFlare(bsp);	//렌즈플레어를 뿌린다.

	_R3ENGINE_STATE *r3_state=GetR3State();
//	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	if( r3_state->mEnvID )	//환경이 존재하는 맵이다.
	{
		bsp->RenderEnvironment(r3_state->mEnvID);	
	}
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );

	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
	BlendOff();
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

}


void Draw2DSprite(float sx,float sy,float w,float h,float u[2],float v[2],void *tex,DWORD color)
{
#define D3D_EL_2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

	struct D3D_EL_2D_VERTEX 
	{ 
	 D3DXVECTOR4 p;   
	 DWORD color;     
	 FLOAT tu, tv; 
	};

	D3D_EL_2D_VERTEX g_vert [4];
	BOOL alpha;
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

//	if( (color&0xff000000) != 0xff000000 )
		alpha = TRUE;
//	else
//		alpha = FALSE;

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	if( alpha )
	{
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_INVSRCCOLOR );
//		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
//		BlendOn(_BLEND_BRIGHT);		
	}

	if(tex==NULL)
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, NULL);
	}
	else
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, (LPDIRECT3DTEXTURE8)tex);
	}
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
 
	g_vert[0].p = D3DXVECTOR4(sx + 0 -0.5f,sy + h -0.5f,0.1f,0.5f);
	g_vert[1].p = D3DXVECTOR4(sx + 0 -0.5f,sy - 0 -0.5f,0.1f,0.5f);
	g_vert[2].p = D3DXVECTOR4(sx + w -0.5f,sy + h -0.5f,0.1f,0.5f);
	g_vert[3].p = D3DXVECTOR4(sx + w -0.5f,sy - 0 -0.5f,0.1f,0.5f);
 
	g_vert[0].color = color;
	g_vert[1].color = color;
	g_vert[2].color = color;
	g_vert[3].color = color;
 
	g_vert[0].tu = u[0];
	g_vert[0].tv = v[1];
 
	g_vert[1].tu = u[0];
	g_vert[1].tv = v[0];
 
	g_vert[2].tu = u[1];
	g_vert[2].tv = v[1];
 
	g_vert[3].tu = u[1];
	g_vert[3].tv = v[0];

	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);

	pd3dDevice->SetVertexShader( D3D_EL_2D );
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (LPVOID) g_vert, sizeof(D3D_EL_2D_VERTEX));
//	DrawPrimitiveTex1(2,GetDynamicVertexBuffer());

	if(alpha)  
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}

void Draw2DSprite(float sx,float sy,float w,float h,void *tex,DWORD color)
{
	float u[2],v[2];
	u[0] = 0.0001f;
	u[1] = 0.9999f;
	v[0] = 0.0001f;
	v[1] = 0.9999f;

	Draw2DSprite(sx,sy,w,h,u,v,tex,color);
}


void DrawRect(float sx,float sy,float w,float h,float u[2],float v[2],void *tex,DWORD color)
{
#define D3D_EL_2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

	struct D3D_EL_2D_VERTEX 
	{ 
	 D3DXVECTOR4 p;   
	 DWORD color;     
	 FLOAT tu, tv; 
	};

	D3D_EL_2D_VERTEX g_vert [4];
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	if(tex==NULL)
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, NULL);
	}
	else
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, (LPDIRECT3DTEXTURE8)tex);
	}
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
 
	g_vert[0].p = D3DXVECTOR4(sx + 0 -0.5f,sy + h -0.5f,0.1f,0.5f);
	g_vert[1].p = D3DXVECTOR4(sx + 0 -0.5f,sy - 0 -0.5f,0.1f,0.5f);
	g_vert[2].p = D3DXVECTOR4(sx + w -0.5f,sy + h -0.5f,0.1f,0.5f);
	g_vert[3].p = D3DXVECTOR4(sx + w -0.5f,sy - 0 -0.5f,0.1f,0.5f);
 
	g_vert[0].color = color;
	g_vert[1].color = color;
	g_vert[2].color = color;
	g_vert[3].color = color;
 
	g_vert[0].tu = u[0];
	g_vert[0].tv = v[1];
 
	g_vert[1].tu = u[0];
	g_vert[1].tv = v[0];
 
	g_vert[2].tu = u[1];
	g_vert[2].tv = v[1];
 
	g_vert[3].tu = u[1];
	g_vert[3].tv = v[0];

	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);

	pd3dDevice->SetVertexShader( D3D_EL_2D );
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (LPVOID) g_vert, sizeof(D3D_EL_2D_VERTEX));
//	DrawPrimitiveTex1(2,GetDynamicVertexBuffer());

	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}


void Draw2DSpriteRot(float sx,float sy,float w,float h,float angle,float u[2],float v[2],void *tex,DWORD color)
{
#define D3D_EL_2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

	struct D3D_EL_2D_VERTEX 
	{ 
	 D3DXVECTOR4 p;   
	 DWORD color;     
	 FLOAT tu, tv; 
	};

	D3D_EL_2D_VERTEX g_vert [4];
	BOOL alpha;
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

//	if( (color&0xff000000) != 0xff000000 )
		alpha = TRUE;
//	else
//		alpha = FALSE;

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	if( alpha )
	{
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	}

	if(tex==NULL)
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, NULL);
	}
	else
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, (LPDIRECT3DTEXTURE8)tex);
	}
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	Vector3f vptr[4];
	vptr[0][0] = sx + 0 -0.5f-w/2.0f;
	vptr[0][1] = sy + h -0.5f-h/2.0f;
	vptr[0][2] = 0;
	vptr[1][0] = sx + 0 -0.5f-w/2.0f;
	vptr[1][1] = sy - 0 -0.5f-h/2.0f;
	vptr[1][2] = 0;
	vptr[2][0] = sx + w -0.5f-w/2.0f;
	vptr[2][1] = sy + h -0.5f-h/2.0f;
	vptr[2][2] = 0;
	vptr[3][0] = sx + w -0.5f-w/2.0f;
	vptr[3][1] = sy - 0 -0.5f-h/2.0f;
	vptr[3][2] = 0;

	float f_mat[4][4],mat[4][4],y_mat[4][4],xyz[4][4];
	MatrixIdentity(f_mat);
	f_mat[3][0] = -sx;
	f_mat[3][1] = -sy;
	MatrixRotate(y_mat,0,0,angle);
	MatrixMultiply(mat,y_mat,f_mat);
	mat[3][0] += sx;
	mat[3][1] += sy;
	Vector3fTransform(xyz[0],vptr[0],mat);
	Vector3fTransform(xyz[1],vptr[1],mat);
	Vector3fTransform(xyz[2],vptr[2],mat);
	Vector3fTransform(xyz[3],vptr[3],mat);

	g_vert[0].p = D3DXVECTOR4(xyz[0][0],xyz[0][1],0.1f,0.5f);
	g_vert[1].p = D3DXVECTOR4(xyz[1][0],xyz[1][1],0.1f,0.5f);
	g_vert[2].p = D3DXVECTOR4(xyz[2][0],xyz[2][1],0.1f,0.5f);
	g_vert[3].p = D3DXVECTOR4(xyz[3][0],xyz[3][1],0.1f,0.5f);
 
	g_vert[0].color = color;
	g_vert[1].color = color;
	g_vert[2].color = color;
	g_vert[3].color = color;
 
	g_vert[0].tu = u[0];
	g_vert[0].tv = v[1];
 
	g_vert[1].tu = u[0];
	g_vert[1].tv = v[0];
 
	g_vert[2].tu = u[1];
	g_vert[2].tv = v[1];
 
	g_vert[3].tu = u[1];
	g_vert[3].tv = v[0];

	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);

	pd3dDevice->SetVertexShader( D3D_EL_2D );
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (LPVOID) g_vert, sizeof(D3D_EL_2D_VERTEX));

	if(alpha)  
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}
void Draw2DSpriteRot(float sx,float sy,float w,float h,float angle,void *tex,DWORD color)
{
	float u[2],v[2];
	u[0] = 0.0001f;
	u[1] = 0.9999f;
	v[0] = 0.0001f;
	v[1] = 0.9999f;

	Draw2DSpriteRot(sx,sy,w,h,angle,u,v,tex,color);
}
void Draw2DSprite( _D3DR3TLVERTEX_TEX1 * pVertex, int nPrimitiveNum, void *pTex )
{
#define D3D_EL_2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

	BOOL alpha;
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

//	if( (color&0xff000000) != 0xff000000 )
		alpha = TRUE;
//	else
//		alpha = FALSE;

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	if( alpha )
	{
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	}

	if(pTex==NULL)
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, NULL);
	}
	else
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, (LPDIRECT3DTEXTURE8)pTex);
	}
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
 
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);

	pd3dDevice->SetVertexShader( D3D_EL_2D );
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, nPrimitiveNum, (LPVOID) pVertex, sizeof(_D3DR3TLVERTEX_TEX1));

	if(alpha)  
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}


void Draw2DSpriteSub(float sx,float sy,float w,float h,int tex_id,DWORD color)
{
#define D3D_EL_2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

	struct D3D_EL_2D_VERTEX 
	{ 
	 D3DXVECTOR4 p;   
	 DWORD color;     
	 FLOAT tu, tv; 
	};

	D3D_EL_2D_VERTEX g_vert [4];
	BOOL alpha;
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

	if( (color&0xff000000) != 0xff000000 )
		alpha = TRUE;
	else
		alpha = FALSE;
 
	if( alpha )
	{
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
//		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	}
 
	if(tex_id==-1)
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, NULL);
	}
	else
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, R3GetSurface(tex_id));
	}
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
 
	g_vert[0].p = D3DXVECTOR4(sx + 0 -0.5f,sy + h -0.5f,1.0f,1.0f);
	g_vert[1].p = D3DXVECTOR4(sx + 0 -0.5f,sy - 0 -0.5f,1.0f,1.0f);
	g_vert[2].p = D3DXVECTOR4(sx + w -0.5f,sy + h -0.5f,1.0f,1.0f);
	g_vert[3].p = D3DXVECTOR4(sx + w -0.5f,sy - 0 -0.5f,1.0f,1.0f);
 
	g_vert[0].color = color;
	g_vert[1].color = color;
	g_vert[2].color = color;
	g_vert[3].color = color;
 
	g_vert[0].tu = 0.0001f;
	g_vert[0].tv = 0.9999f;
 
	g_vert[1].tu = 0.0001f;
	g_vert[1].tv = 0.0001f;
 
	g_vert[2].tu = 0.9999f;
	g_vert[2].tv = 0.9999f;
 
	g_vert[3].tu = 0.9999f;
	g_vert[3].tv = 0.0001f;
 
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
	pd3dDevice->SetVertexShader( D3D_EL_2D );
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (LPVOID) g_vert, sizeof(D3D_EL_2D_VERTEX));
//	DrawPrimitiveTex1(2,GetDynamicVertexBuffer());
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
 
	if(alpha)  
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
}

void Draw2DSprite(float sx,float sy,float w,float h,int tex_id,DWORD color)
{
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	Draw2DSpriteSub(sx,sy,w,h,tex_id,color);
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}

void DrawTLVertex(int matID,int face_num,DWORD alpha_type,_LPD3DR3VERTEX *buf)	//미리 버텍스버퍼에 저장해야된다.
{
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

	if( buf==NULL)
		return;

	if(alpha_type)  
	{
		BlendOn(alpha_type);
//		BlendOn(_BLEND_BRIGHT);
	}
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	pd3dDevice->SetVertexShader( D3DFVF_R3TLVERTEX_TEX1 );
	pd3dDevice->SetTexture( 0, R3GetSurface(matID) );

	DrawPrimitiveTLTex1(face_num,buf);

	if(alpha_type)  
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}
void DrawTLVertex(void *matP,int face_num,DWORD alpha_type,_LPD3DR3VERTEX buf,DWORD vbuf_type)	//미리 버텍스버퍼에 저장해야된다.
{
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

	if( buf==NULL)
		return;

	if(alpha_type)  
	{
		BlendOn(alpha_type);
//		BlendOn(_BLEND_BRIGHT);
	}
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	pd3dDevice->SetVertexShader( D3DFVF_R3TLVERTEX_TEX1 );
	pd3dDevice->SetTexture( 0, (LPDIRECT3DTEXTURE8)matP );

//	if( vbuf_type == D3DPT_TRIANGLELIST )
		DrawPrimitiveTLTex1(face_num,buf);
//	else
//		DrawPrimitiveUPTex1Strip(face_num,buf);

	if(alpha_type)  
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}
void DrawPrimitiveUPTex1(void *matP,int face_num,DWORD alpha_type,_LPD3DR3VERTEX buf,DWORD vbuf_type)	//미리 버텍스버퍼에 저장해야된다.
{
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

	if( buf==NULL)
		return;

	if(alpha_type)  
	{
		BlendOn(alpha_type);
//		BlendOn(_BLEND_BRIGHT);
	}
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	pd3dDevice->SetVertexShader( D3DFVF_R3VERTEX_TEX1 );
	pd3dDevice->SetTexture( 0, (LPDIRECT3DTEXTURE8)matP );

	if( vbuf_type == D3DPT_TRIANGLELIST )
		DrawPrimitiveUPTex1(face_num,buf);
	else
		DrawPrimitiveUPTex1Strip(face_num,buf);

	if(alpha_type)  
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}
void DrawIndexedTex1VertexUP(void *matP,int face_num,int index_num,DWORD alpha_type,WORD *index,_LPD3DR3VERTEX buf)	//미리 버텍스버퍼에 저장해야된다.
{
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

	if( buf==NULL)
		return;

	if(alpha_type)  
	{
		BlendOn(alpha_type);
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

//	pd3dDevice->SetVertexShader( D3DFVF_R3TLVERTEX_TEX1 );
	SetVPIPTex1IndexPrimitive(NULL,NULL);	//TL들도 등록을 해야된다.
	pd3dDevice->SetTexture( 0, (LPDIRECT3DTEXTURE8)matP );

	DrawIndexedPrimitiveUPTex1(face_num,index_num,index,buf);

	if(alpha_type)  
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

}


void DrawIndexedTLVertex(void *matP,int face_num,int index_num,DWORD alpha_type,WORD *index,_LPD3DR3VERTEX buf)	//미리 버텍스버퍼에 저장해야된다.
{
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

	if( buf==NULL)
		return;

	if(alpha_type)  
	{
		BlendOn(alpha_type);
//		BlendOn(_BLEND_BRIGHT);
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

//	pd3dDevice->SetVertexShader( D3DFVF_R3TLVERTEX_TEX1 );
	SetVPIPTex1IndexPrimitiveTL(NULL,NULL);	//TL들도 등록을 해야된다.
	pd3dDevice->SetTexture( 0, (LPDIRECT3DTEXTURE8)matP );

	DrawIndexedPrimitiveTLTex1(face_num,index_num,index,buf);

	if(alpha_type)  
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

}


//-------------------------- 실제쓰는것들....



void DrawPrimitiveUPPoly(int num,void *vbuffer)
{
	if( num == 0 )
		return;

	MultiTexOff();
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	pd3dDevice->SetVertexShader( D3DFVF_R3VERTEX_TEX0 );
	pd3dDevice->SetTexture( 0, 0 );
	pd3dDevice->SetTexture( 1,NULL);

	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,num,vbuffer,sizeof(_D3DR3VERTEX_TEX0));
}

void DrawOneMatGroup(CVertexBuffer *vb,CIndexBuffer *ib,int min_vp,int base_index_v,int v_num,int face_num)
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	pd3dDevice->SetTexture( 0,NULL);
	pd3dDevice->SetTexture( 1,NULL);
	pd3dDevice->SetVertexShader( D3DFVF_R3VERTEX_TEX2 );
    pd3dDevice->SetStreamSource( 0, vb->m_lpVertexBuffer, sizeof(_D3DR3VERTEX_TEX2) );
    pd3dDevice->SetIndices( ib->m_lpIndexBuffer, 0 );
    pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
		min_vp, v_num,
        base_index_v, face_num );
  //  pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
//		0, v_num,
//        0, face_num );
}

static void DetailTextureOn()
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	MultiTexOff();
	BlendOn(_BLEND_LIGHTMAP);

//	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
}

static void DetailTextureOff()
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	BlendOff();
	MultiTexOn();
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); 
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
}
static void VertexDetailTextureOn()	//버텍스 디테일 텍스쳐 킨다.
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

//	BlendOn(_BLEND_LIGHTMAP);

//	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	pd3dDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_POINT );
			
}

static void VertexDetailTextureOff() //버텍스 디테일 텍스쳐 끈다.
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

//	BlendOff();
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); 
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_NONE );
}

void GetBumpFactor(Vector4f factor)
{
    // Setup the bump matrix
    // Min r is 0.04 if amplitude is 32 to miss temporal aliasing
    // Max r is 0.16 for amplitude is 8 to miss spatial aliasing
	/*
    FLOAT r = 0.01f;
    factor[0] =  r * cosf( R3GetTime() * 3.0f );
    factor[1] = -r * sinf( R3GetTime() * 3.0f );
    factor[2] =  r * sinf( R3GetTime() * 3.0f );
    factor[3] =  r * cosf( R3GetTime() * 3.0f );
	*/
    FLOAT r = 0.01f;
    factor[0] =  r * cosf( R3GetTime() * 3.0f );
    factor[1] = -r * sinf( R3GetTime() * 3.0f );
    factor[2] =  r * sinf( R3GetTime() * 3.0f );
    factor[3] =  r * cosf( R3GetTime() * 3.0f );
}

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }
static BOOL stIsTexMat;		//텍스쳐 메트릭스를 쓰는지를 알기위한 변수

void PreRenderOneLayer(CVertexBuffer *vbuffer,_BSP_MAT_GROUP *m_group)	//첫번째레이어에서 할것들.
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	_R3MATERIAL *mat = &GetMainMaterial()[m_group->MtlId];
	if( (mat->m_dwFlag & _MAT_FLAG_LIGHT_MAP) == FALSE )
	{
		MultiTexOff();
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	}
	else
	if( m_group->LgtId == -1)	//버텍스 라이트다...
	{		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
		if( mat->m_iDetailSurface != -1 )	//디테일 텍스쳐링.
		{
			VertexDetailTextureOn();	//디테일 텍스쳐를 킨다.
			pd3dDevice->SetTexture( 1,(LPDIRECT3DTEXTURE8)R3GetSurface(mat->m_iDetailSurface));
		}
		else
			MultiTexOff();	
	}
	else
	{
		SetLightMap(m_group->LgtId);	//1번 스테이지유닛을 라이트맵으로..
		if( mat->m_iDetailSurface != -1 )	//디테일 텍스쳐링.
			DetailTextureOnStage2(mat);	//2번텍스쳐 스테이지 유닛으로 디테일 텍스쳐를 킨다.
	}
	
	if( mat->m_Layer[1].m_dwFlag & _MAT_ENV_BUMP)	//범프매핑이면.
	{
		//env범프는 항상 0번을 차지한다.
		pd3dDevice->SetTexture( 0, R3GetSurface(mat->m_Layer[1].m_iSurface) );
		pd3dDevice->SetTexture( 1, R3GetSurface(mat->m_Layer[0].m_iSurface) );

		pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_BUMPENVMAP );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

		if( !(mat->m_Layer[0].m_dwFlag & _UV_METAL) )	//메탈 처럼 텍스쳐매트릭스 쓰게되면..
			pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

		Vector4f bump_f;
		GetBumpFactor(bump_f);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT00, F2DW(bump_f[0]) );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT01, F2DW(bump_f[1]) );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT10, F2DW(bump_f[2]) );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT11, F2DW(bump_f[3]) );

/*
		pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT00, F2DW(0.01f) );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT01, F2DW(0.00f) );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT10, F2DW(0.00f) );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT11, F2DW(0.01f) );
*/
		D3DXMATRIX tex_mat;
		BOOL is_tex_mat=GetTextureMatrix(mat,0,&tex_mat);
		if( is_tex_mat )
		{
			pd3dDevice->SetTransform(D3DTS_TEXTURE1,&tex_mat );
			pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		}
		is_tex_mat=GetTextureMatrix(mat,1,&tex_mat);
		if( is_tex_mat )
		{
			pd3dDevice->SetTransform(D3DTS_TEXTURE0,&tex_mat );
			pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		}
	}
	
	/*
	if( m_group->Type & _MAT_GROUP_TYPE_OPPA )
	{
		if( GetR3State()->mIsFog && ((mat->m_dwAlphaType[0]&0xff) == _BLEND_OPPA))
			pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
		//MultiTexOff();
		BlendOn(mat->m_dwAlphaType[0]);
//		GetRenderState(stRStateA);
	}*/
}

void AfterRenderOneLayer(CVertexBuffer *vbuffer,_BSP_MAT_GROUP *m_group)	//첫번째 레이어에서 반드시할일.
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	_R3MATERIAL *mat = &GetMainMaterial()[m_group->MtlId];
	if( (mat->m_dwFlag & _MAT_FLAG_LIGHT_MAP) == FALSE )
	{
		MultiTexOn();
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	}
	else
	if( m_group->LgtId == -1)	//
	{
		MultiTexOn();	//버텍스 라이트다...
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	}
	if( mat->m_iDetailSurface != -1 )	//디테일 텍스쳐링.
	{
		if( (mat->m_dwFlag & _MAT_FLAG_LIGHT_MAP) && m_group->LgtId == -1)	//	버텍스 라이트의 경우.
		{
			VertexDetailTextureOff();	//디테일 텍스쳐를 끈다.
		}
		else	//라이트 맵인경우.. 
		{
			DetailTextureOffStage2();	//2번텍스쳐 스테이지 유닛을 끈다.
			/*	디테일 텍스쳐는 3번 텍스쳐 유닛으로 셋팅하기위해서..
			DetailTextureOn();	//디테일 텍스쳐를 킨다.
			D3DXMATRIX tex_mat;		//텍스쳐 좌표..
			D3DXMatrixIdentity( &tex_mat );
			tex_mat.m[0][0]=mat->m_fDetailScale*8;
			tex_mat.m[1][1]=mat->m_fDetailScale*8;

			pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			pd3dDevice->SetTransform(D3DTS_TEXTURE0,&tex_mat );

			pd3dDevice->SetTexture( 0,(LPDIRECT3DTEXTURE8)R3GetSurface(mat->m_iDetailSurface));
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
				m_group->VBMinIndex, m_group->VCnt,
				m_group->IBMinIndex, m_group->TriNum );

			pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
			DetailTextureOff();	//디테일 텍스쳐를 끈다.
			*/
		}
	}
	if( mat->m_Layer[1].m_dwFlag & _MAT_ENV_BUMP)	//범프매핑이면.
	{
		//env범프는 항상 0번을 차지한다.
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
		pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); 
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );

		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
		//	SetDefaultFlags();
	}
}

void PreRenderSetting(int l_id,CVertexBuffer *vbuffer,_BSP_MAT_GROUP *m_group) //DP호출전 할일.
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	_D3DR3VERTEX_TEX2 *vertexbp;
	D3DXMATRIX tex_mat;

	_R3MATERIAL *mat = &GetMainMaterial()[m_group->MtlId];

	stIsTexMat=GetTextureMatrix(mat,l_id,&tex_mat);
	if( stIsTexMat )
	{
		if( mat->m_Layer[l_id].m_dwFlag & _UV_METAL)
		{
			if( mat->m_Layer[1].m_dwFlag & _MAT_ENV_BUMP)	//범프매핑이면1번 채널로 간다.
			{
				pd3dDevice->SetTransform(D3DTS_TEXTURE1,&tex_mat );
				pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
				pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
			}
			else
			{
				pd3dDevice->SetTransform(D3DTS_TEXTURE0,&tex_mat );
				pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
				pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
			}
		}
		else
		{
			pd3dDevice->SetTransform(D3DTS_TEXTURE0,&tex_mat );
			pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		}
	}
	if( mat->m_Layer[l_id].m_dwFlag & _NEED_LOCK_FLAG )	//현재 레이어에 쓸일이 있으면
	{
		vertexbp=(_D3DR3VERTEX_TEX2 *)vbuffer->VPLock(m_group->VBMinIndex*sizeof(_D3DR3VERTEX_TEX2),m_group->VCnt*sizeof(_D3DR3VERTEX_TEX2),D3DLOCK_NOOVERWRITE);

		MakeUV(vertexbp,m_group->VCnt,m_group->MultiSourceUV,m_group->MultiSourceST,mat,l_id);

		vbuffer->VPUnLock();	//언락 해주자.
	}
	if( mat->m_Layer[l_id].m_dwFlag & _ANI_TILE_TEXTURE )	//애니 텍스쳐면...
	{
		int add_id=GetTileAniTextureAddId(mat,l_id);
		pd3dDevice->SetTexture( 0,R3GetSurface(mat->m_Layer[l_id].m_iSurface+add_id));
	}
	else
		pd3dDevice->SetTexture( 0,R3GetSurface(mat->m_Layer[l_id].m_iSurface));

	if( mat->m_Layer[l_id].m_dwFlag & _MAT_TEX_CLAMP )
	{
		pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
		pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);
	}		
	if( mat->m_Layer[l_id].m_dwAlphaType )
	{
//		if( GetR3State()->mIsFog && ((mat->m_Layer[l_id].m_dwAlphaType&0xff) == _BLEND_OPPA))
//			pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
		BlendOn(mat->m_Layer[l_id].m_dwAlphaType);
	}
	if( mat->m_Layer[l_id].m_dwFlag & _UV_ENV)	//env 매핑이면.
	{
		D3DXMatrixIdentity(&tex_mat);
		pd3dDevice->SetTransform(D3DTS_TEXTURE0,&tex_mat );
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );
//        pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL  );
//        pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION  );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
	}
}
void AfterRenderSetting(int l_id,CVertexBuffer *vbuffer,_BSP_MAT_GROUP *m_group) //DP호출후 할일.
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	_R3MATERIAL *mat = &GetMainMaterial()[m_group->MtlId];

	if( stIsTexMat )
	{
		if( mat->m_Layer[1].m_dwFlag & _MAT_ENV_BUMP)	//범프매핑이면1번 채널로 간다.
		{
			pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
			if( mat->m_Layer[l_id].m_dwFlag & _UV_METAL)
				pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
		}
		else
		{
			pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
			if( mat->m_Layer[l_id].m_dwFlag & _UV_METAL)
				pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
		}
	}
	if(mat->m_Layer[l_id].m_dwFlag &_ANI_ALPHA_FLICKER)//점멸
	{
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );  
		pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
		BlendOff();
//				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	}
	if( mat->m_Layer[l_id].m_dwFlag & _MAT_TEX_CLAMP )
	{
		pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
		pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
	}		
/*
	if( m_group->Type & _MAT_GROUP_TYPE_OPPA )
	{
		if( GetR3State()->mIsFog && ((mat->m_Layer[l_id].m_dwAlphaType&0xff) == _BLEND_OPPA))
			GetD3dDevice()->SetRenderState( D3DRS_FOGENABLE, FALSE );
		BlendOff();
	}*/
	if( mat->m_Layer[l_id].m_dwAlphaType )
	{
//		if( GetR3State()->mIsFog && ((mat->m_Layer[l_id].m_dwAlphaType&0xff) == _BLEND_OPPA))
//			pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		BlendOff();
	}
	if( mat->m_Layer[l_id].m_dwFlag & _UV_ENV)	//범프매핑이면.
	{
        pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	}
}
void SetMatAlphaColor(DWORD argb)
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	D3DMATERIAL8 mtrl;
	memset(&mtrl,0,sizeof(D3DMATERIAL8));
	mtrl.Diffuse.a=((argb>>24)/255.0f);
	mtrl.Diffuse.r=1;
	mtrl.Diffuse.g=1;
	mtrl.Diffuse.b=1;
	mtrl.Emissive.a=1;
	mtrl.Emissive.r=((argb>>16)&255)/255.0f;
	mtrl.Emissive.g=((argb>>8)&255)/255.0f;
	mtrl.Emissive.b=((argb)&255)/255.0f;
	pd3dDevice->SetMaterial( &mtrl );
	pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
//	pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );  
	pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
	pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1 );//D3DMCS_MATERIAL );
}
void UnSetMatAlphaColor()
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );  
	pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );

}


//다이나믹 라이트 인자.   위치 지름,강도(알파)...
void SetDynamicLight(Vector3f d_light_pos,float diameter,DWORD color,void *tex,BOOL multi_tex,DWORD blend_type)
{
	if( diameter <= 0)
		return;
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	//다이나믹 라이트 준비.
	BlendOn(blend_type);

	if( multi_tex )
		MultiTexOn();
	else
		MultiTexOff();

	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
	if( multi_tex )
	{
		pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
		pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
//		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
//		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	}

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE ,FALSE );

	DWORD a=(color>>24);	
	DWORD r=(((color>>16)&0xff)*a)>>8;
	DWORD g=(((color>>8)&0xff)*a)>>8;
	DWORD b=(((color>>0)&0xff)*a)>>8;
//	color = 0xff000000|(r<<16)|(g<<8)|(b);
	color = (a<<24)|(r<<16)|(g<<8)|(b);
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,color);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	_R3ENGINE_STATE *state= GetR3State();
	D3DXMATRIX temp = state->mInvMatView,uv_temp,fix_mat,uv_matrix;
	float scale = 1/diameter;//0.01f;//05f;		//밑의 숫자가 바로 지름이다....
	D3DXMatrixIdentity( &uv_matrix );
	uv_matrix.m[0][0]=scale;
	uv_matrix.m[1][1]=scale;
	uv_matrix.m[2][2]=scale;

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU , D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV , D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU , D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSV , D3DTADDRESS_CLAMP);
/*
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU , D3DTADDRESS_BORDER);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV , D3DTADDRESS_BORDER);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU , D3DTADDRESS_BORDER);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSV , D3DTADDRESS_BORDER);
*/
	MatrixRotateX(fix_mat.m,-90 );

	fix_mat.m[1][1] = 0;
	fix_mat.m[1][2] = -1;
	fix_mat.m[2][2] = 0;
	fix_mat.m[2][1] = 1;

	Vector3f c_pos,pos;
	GetPointCamera(&c_pos);
	Vector3fSub(state->mInvMatView.m[3],c_pos,pos);

	temp.m[3][0]=(pos[0]-d_light_pos[0]+c_pos[0])*scale;
	temp.m[3][1]=(pos[1]-d_light_pos[1]+c_pos[1])*scale;//-44;
	temp.m[3][2]=(pos[2]-d_light_pos[2]+c_pos[2])*scale;
	temp.m[3][0]+=0.5;	//나중에 다이나믹 라이트할때.
	temp.m[3][1]+=0.5;
	temp.m[3][2]+=0.5;

	if( multi_tex )
	{
		uv_temp = uv_matrix;
		uv_temp = uv_temp*temp;
	 	pd3dDevice->SetTransform(D3DTS_TEXTURE1,&uv_temp );
	}

	uv_temp = uv_matrix;
	uv_temp = uv_temp*temp;
	uv_matrix=uv_temp*fix_mat;


	pd3dDevice->SetTransform(D3DTS_TEXTURE0,&uv_matrix );

	if( tex == NULL )	//지정된 다이나믹 라이트가 없는경우..
	{
		pd3dDevice->SetTexture( 0,state->mDLightTexture);
		if( multi_tex )
			pd3dDevice->SetTexture( 1,state->mDLightTexture);
	}
	else	//지정된 다이나믹 라이트가 있는경우.
	{
		pd3dDevice->SetTexture( 0,(LPDIRECT3DTEXTURE8)tex);
		if( multi_tex )
			pd3dDevice->SetTexture( 1,(LPDIRECT3DTEXTURE8)tex);
	}
	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
}

void UnSetDynamicLight()
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);//D3DTSS_TCI_PASSTHRU );
	pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);//D3DTSS_TCI_PASSTHRU );

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU , D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV , D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU , D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSV , D3DTADDRESS_WRAP);

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE ,TRUE );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );

	BlendOff();

	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
}


void DrawBlurRect(DWORD level,float w,float h,void *tex)
{
#define D3D_EL_2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

	struct D3D_EL_2D_VERTEX 
	{ 
	 D3DXVECTOR4 p;   
	 DWORD color;     
	 FLOAT tu, tv; 
	};

	D3D_EL_2D_VERTEX g_vert [3000];
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	if(tex==NULL)
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, NULL);
	}
	else
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, (LPDIRECT3DTEXTURE8)tex);
	}
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	
	int i,j;
	int start = level/2;
	float u[2]={0.001f,0.999f};
	float v[2]={0.001f,0.999f};
	DWORD color = ((DWORD)((1/(float)(level*level))*255));
	color = 0xff000000|(color<<16)|(color<<8)|(color<<0);
	DWORD cnt=0;
	for( i=-start; i<start+1; i++)
	{
		for( j=-start; j<start+1; j++)
		{
			//DrawRect(i,j,stRTest.GetXSize(),stRTest.GetYSize(),u,v,tex,color);
			g_vert[cnt].p = D3DXVECTOR4(i + 0 -0.5f,j - 0 -0.5f,0.1f,0.5f);
			g_vert[cnt].color = color;
			g_vert[cnt].tu = u[0];
			g_vert[cnt].tv = v[0];
			cnt++;
 			g_vert[cnt].p = D3DXVECTOR4(i + w -0.5f,j - 0 -0.5f,0.1f,0.5f);
			g_vert[cnt].color = color;
			g_vert[cnt].tu = u[1];
			g_vert[cnt].tv = v[0];
			cnt++;
			g_vert[cnt].p = D3DXVECTOR4(i + w -0.5f,j + h -0.5f,0.1f,0.5f);
			g_vert[cnt].color = color;
			g_vert[cnt].tu = u[1];
			g_vert[cnt].tv = v[1];
			cnt++;

			g_vert[cnt].p = D3DXVECTOR4(i + 0 -0.5f,j - 0 -0.5f,0.1f,0.5f);
			g_vert[cnt].color = color;
			g_vert[cnt].tu = u[0];
			g_vert[cnt].tv = v[0];
			cnt++;
			g_vert[cnt].p = D3DXVECTOR4(i + w -0.5f,j + h -0.5f,0.1f,0.5f);
			g_vert[cnt].color = color;
			g_vert[cnt].tu = u[1];
			g_vert[cnt].tv = v[1];
			cnt++;
			g_vert[cnt].p = D3DXVECTOR4(i + 0 -0.5f,j + h -0.5f,0.1f,0.5f);
			g_vert[cnt].color = color;
			g_vert[cnt].tu = u[0];
			g_vert[cnt].tv = v[1];
			cnt++;
		}
	}

	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);

//	GetD3dDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	pd3dDevice->SetVertexShader( D3D_EL_2D );
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, cnt/3, (LPVOID) g_vert, sizeof(D3D_EL_2D_VERTEX));
//	DrawPrimitiveTex1(2,GetDynamicVertexBuffer());

	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}
/*
void DrawBlurRect256(DWORD level,void *tex)
{
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	if(tex==NULL)
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, NULL);
	}
	else
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, (LPDIRECT3DTEXTURE8)tex);
	}
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);


	SetVPIPTex1IndexPrimitive(NULL,NULL);	//TL들도 등록을 해야된다.

	GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
	DrawBlurVBuffer(level);

	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}
*/
#define r_xmax 1.01f
#define r_ymax 1.01f
//#include "R3input.h"

#include "r3enginekernel.h"

static LPDIRECT3DSURFACE8		m_pBackBuffer, m_pZBuffer;

void Filter(CTextureRender *tex_tender,DWORD level,void *tex)		//1,3,5,7,9 
{
	int start = level/2;
	float u[2]={0,1};
	float v[2]={0,1};
//	DWORD color = ((DWORD)((1/(float)(level*level))*255)<<24)|0x00ffffff;
	DWORD color = ((DWORD)((1/(float)(level*level))*255));
	color = 0xff000000|(color<<16)|(color<<8)|(color<<0);
/*
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	pd3dDevice->GetRenderTarget( &m_pBackBuffer );
	pd3dDevice->GetDepthStencilSurface( &m_pZBuffer );
*/
//	R3EndScene();
	
	tex_tender->BeginScene(1);	//필터링 끝을 막는다.
	tex_tender->ClearFrameBuffer(0xff000000);
//	tex_tender->ClearFrameBuffer(0xffff00ff);
	
	BlendOn(_BLEND_BRIGHT);		
//	BlendOff();		

	DrawBlurRect(level,256,256,tex);
//	DrawBlurRect(1,256,256,tex);
	BlendOff();		
	
	tex_tender->EndScene();
/*
	pd3dDevice->SetRenderTarget( m_pBackBuffer, m_pZBuffer);
	m_pBackBuffer->Release();
    m_pZBuffer->Release();
*/
}

void SetProjectShadow(Vector3f char_pos,void *tex,float intensity,DWORD blur,float scale,float add_pos)//project 쉐도우 들어가기전에 호출할것.
{
	D3DXMATRIX mat;
/*
#define s_map_width 512
#define s_map_height 512
#define z_near 5.0f
#define z_far  6000.0f
	memset(&mat,0,sizeof(D3DXMATRIX));

//	mat.m[0][0] = (0.98f/r_xmax)*s_map_width;
//	mat.m[1][1] = (0.98f/r_ymax)*s_map_height;
//	mat.m[2][2] = (z_far+z_near)/(z_near-z_far);
//	mat.m[2][3] = -1;
//	mat.m[3][2] = (2*z_far*z_near)/(z_near-z_far);
/*
	if( gAsci[VK_A] )
	{
		gAsci[VK_A]=0;
		src_blend++;
		if( src_blend > 13 )
			src_blend--;
	}
	if( gAsci[VK_Z] )
	{
		gAsci[VK_Z]=0;
		src_blend--;
		if( src_blend < 1 )
			src_blend++;
	}
	if( gAsci[VK_S] )
	{
		gAsci[VK_S]=0;
		dest_blend++;
		if( dest_blend > 13 )
			dest_blend--;
	}
	if( gAsci[VK_X] )
	{
		gAsci[VK_X]=0;
		dest_blend--;
		if( dest_blend < 1 )
			dest_blend++;
	}


	mat.m[0][0] = (1.0f/r_xmax)*s_map_width;
	mat.m[1][1] = (1.0f/r_ymax)*s_map_height;
	mat.m[2][2] = (z_far)/(z_far-z_near);
	mat.m[2][3] = 1;
	mat.m[3][2] = (z_far*z_near)/(z_near-z_far);
*/
#define _TEX_FOV (D3DX_PI/(1024.0f))
	D3DXMatrixPerspectiveFovLH( &mat, _TEX_FOV,1,5,6000 );

	static float gop_x=0,gop_y=-636.5*32.0f,gop_z=0;
/*
	if(gScan[VK_R])
		gop_the+=.1f;
	if(gScan[VK_F])
		gop_the-=.1f;
	if(gScan[VK_Y])
		gop_x+=.01;
	if(gScan[VK_H])
		gop_x-=.01;
	if(gScan[VK_U])
		gop_y+=1;
	if(gScan[VK_J])
		gop_y-=1;
	if(gScan[VK_I])
		gop_z+=.01;
	if(gScan[VK_K])
		gop_z-=.01;
	if(gScan[VK_O])
		gop+=1;
	if(gScan[VK_L])
		gop-=1;
*/
	Vector3f c_pos;
	GetPointCamera(&c_pos);

	D3DXMATRIX lgt_view,temp;
	D3DXVECTOR3 up;
	D3DXVECTOR3 eye,at,sub_cam;
	up.x=0;
	up.y=-1;
	up.z=0;
	float angle = GetR3State()->mMainLightYAngle+3.141592f;

	float y_rate=gop_y*scale;
	eye.x = (float)(char_pos[0]+(add_pos-y_rate*3/5)*sin(angle));
	eye.y = (float)(y_rate+char_pos[1]);
	eye.z = (float)(char_pos[2]+(add_pos-y_rate*3/5)*cos(angle));
	at.x = char_pos[0]+add_pos*(float)sin(angle);//+0.37f;
	at.y = char_pos[1];
	at.z = char_pos[2]+add_pos*(float)cos(angle);//-0.22f;

	sub_cam = eye - at;
	float c_dist = D3DXVec3Length(&sub_cam);
	c_dist = c_dist*(float)tan(_TEX_FOV);

	D3DXMatrixLookAtLH( &lgt_view, &eye, &at,&up );
	lgt_view.m[3][0] += c_dist/4-0.32f;
	lgt_view.m[3][1] += c_dist/4-0.52f;
	lgt_view.m[3][2] += 0;

	_R3ENGINE_STATE *state= GetR3State();


	temp=state->mInvMatView;
	temp.m[3][0] = state->mInvMatView.m[3][0];
	temp.m[3][1] = state->mInvMatView.m[3][1];
	temp.m[3][2] = state->mInvMatView.m[3][2];

	temp = temp*lgt_view;
	mat=temp*mat;

	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
/*	//---------------------------나중에 지워라....
	static LPDIRECT3DTEXTURE8 stptest=NULL;
	if( stptest == 0 )
	{
		LPDIRECT3DTEXTURE8 R3LoadDDSAndTextureMem(char *name);
		stptest=R3LoadDDSAndTextureMem(".\\Map\\작업중\\tex\\shadow.dds");
	
	}
	pd3dDevice->SetTexture( 0,(LPDIRECT3DTEXTURE8)stptest);
*/
	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	Filter(GetShadowRenderTexture(),blur,tex);		//1,3,5,7,9 
	pd3dDevice->SetTexture( 0,GetShadowRenderTexture()->GetD3DTexture());

	BlendOn(_BLEND_SHADOW);//INV_LIGHTMAP);
//	BlendOff();


	DWORD color = (DWORD)(intensity*255);
	color = 0xff000000|(color<<16)|(color<<8)|(color);
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,color);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

//	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_BORDER          );
//	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_BORDER          );
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP          );
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP          );
	 
	MultiTexOff();
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3|D3DTTFF_PROJECTED);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );//| 1);
	pd3dDevice->SetTransform(D3DTS_TEXTURE0,&mat );

	//매트릭스당빠 셋팅해야지.
	pd3dDevice->SetTransform( D3DTS_VIEW, &GetR3State()->mMatView );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &GetR3State()->mMatProj );

//	SetMatAlphaColor(0x1fffffff);
/*
	if( gScan[VK_G] )
	{
		SaveRenderState();
		SaveTextureStage();
	}
	*/
}


void UnSetProjectedShadow()	//proejct 쉐도우 실행하고 나서 호출할것.
{
//	UnSetMatAlphaColor();
	BlendOff();
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//	MultiTexOn();
	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
}



void DrawShadow(_BSP_MAT_GROUP *m_group)
{
	GetD3dDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
		m_group->VBMinIndex, m_group->VCnt,
		m_group->IBMinIndex, m_group->TriNum );
}
void DrawStripOneMatGroup(_BSP_MAT_GROUP *m_group)
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	GetD3dDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
		m_group->VBMinIndex, m_group->VCnt,
		m_group->IBMinIndex, m_group->TriNum );

	return;
}

void DrawOneMatGroup(CVertexBuffer *vbuffer,_BSP_MAT_GROUP *m_group)
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	D3DXMATRIX tex_mat;

	if(m_group->MtlId==-1 )
	{
		pd3dDevice->SetTexture( 0,NULL);
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
			m_group->VBMinIndex, m_group->VCnt,
			m_group->IBMinIndex, m_group->TriNum );
		return;
	}

	_R3MATERIAL *mat = &GetMainMaterial()[m_group->MtlId];


	//SetMaterial
	PreRenderSetting(0,vbuffer,m_group);
	PreRenderOneLayer(vbuffer,m_group);	//첫번째레이어에서 할것들.

	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
		m_group->VBMinIndex, m_group->VCnt,
		m_group->IBMinIndex, m_group->TriNum );

	AfterRenderOneLayer(vbuffer,m_group);	//첫번째레이어에서 할것들.
	AfterRenderSetting(0,vbuffer,m_group);

	if( mat->m_dwLayerNum == 1 || (mat->m_Layer[1].m_dwFlag & _MAT_ENV_BUMP) )//레이어 1이 범프면 넘어가고...
	{
		return;
	}
//---------------------------------------------여기서부터는 멀티레이어다...

	MultiTexOff();
	DWORD i;

	//컬러가 먹으면 안된다.
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

//	for(i=0; i<mat->m_dwLayerNum; i++)
	for(i=1; i<mat->m_dwLayerNum; i++)
	{
		if( mat->m_Layer[i].m_ARGB != mat->m_Layer[0].m_ARGB )	//머터리얼 알파.
			SetMatAlphaColor(mat->m_Layer[i].m_ARGB);

		PreRenderSetting(i,vbuffer,m_group);

		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
			m_group->VBMinIndex, m_group->VCnt,
			m_group->IBMinIndex, m_group->TriNum );

		AfterRenderSetting(i,vbuffer,m_group);

		if( mat->m_Layer[i].m_ARGB != mat->m_Layer[0].m_ARGB || (mat->m_Layer[i].m_dwFlag &_ANI_ALPHA_FLICKER))//점멸
			UnSetMatAlphaColor();
	}
	MultiTexOn();

}






void DrawLightMapGroup(CVertexBuffer *vbuffer,_BSP_MAT_GROUP *m_group)
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	D3DXMATRIX tex_mat;

	if(m_group->MtlId==-1 )
	{
		pd3dDevice->SetTexture( 0,NULL);
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
			m_group->VBMinIndex, m_group->VCnt,
			m_group->IBMinIndex, m_group->TriNum );
		return;
	}

	//SetMaterial

	_R3MATERIAL *mat = &GetMainMaterial()[m_group->MtlId];
	pd3dDevice->SetTexture( 0,NULL);
	if( m_group->LgtId == -1)	//버텍스 라이트다...
	{		
		MultiTexOff();	
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	}
	else
	{
		MultiTexOn();	
		SetLightMap(m_group->LgtId);
	}
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		
//	PreRenderSetting(0,vbuffer,m_group);
//	PreRenderOneLayer(vbuffer,m_group);	//첫번째레이어에서 할것들.

	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
		m_group->VBMinIndex, m_group->VCnt,
		m_group->IBMinIndex, m_group->TriNum );
}

void R3DrawLine(Vector3f a,Vector3f b,DWORD color)
{
	_D3DR3VERTEX_TEX0 buf[2];

	buf[0].x=a[0];
	buf[0].y=a[1];
	buf[0].z=a[2];
	buf[0].color = color;
	buf[1].x=b[0];
	buf[1].y=b[1];
	buf[1].z=b[2];
	buf[1].color = color;

	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );  
	pd3dDevice->SetTexture( 0,NULL);
//	pd3dDevice->SetTexture( 1, NULL );
	DrawPrimitiveUPLine(1,buf);
}

void R3Draw2DLine(Vector2f a,Vector2f b,DWORD color)
{
	_D3DR3TLVERTEX_TEX0 buf[2];

	buf[0].x=a[0];
	buf[0].y=a[1];
	buf[0].z=0.1f;
	buf[0].w=0.5f;
	buf[0].color = color;
	buf[1].x=b[0];
	buf[1].y=b[1];
	buf[1].z=0.1f;
	buf[1].w=0.5f;
	buf[1].color = color;

	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND ,D3DVBF_DISABLE );
	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE ,FALSE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );  
	pd3dDevice->SetTexture( 0,NULL);
//	pd3dDevice->SetTexture( 1, NULL );
	DrawPrimitiveUPTLLine(1,buf);
	pd3dDevice->SetRenderState( D3DRS_ZENABLE ,TRUE );
	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
}

void R3Draw2DLineList(Vector2f *line,DWORD line_num,DWORD color)//라인들 한꺼번에 뿌리자.
{
	_D3DR3TLVERTEX_TEX0 buf[1024*2];

	line_num=min(line_num,1024);

	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	pd3dDevice->SetRenderState( D3DRS_ZENABLE ,FALSE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	pd3dDevice->SetTexture( 0,NULL);
	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	for(DWORD i=0; i< line_num*2; i++)
	{
		buf[i].x=line[i][0];
		buf[i].y=line[i][1];
		buf[i].z=0.1f;
		buf[i].z=0.5f;
		buf[i].color = color;
	}
	DrawPrimitiveUPTLLine(line_num,buf);

	pd3dDevice->SetRenderState( D3DRS_ZENABLE ,TRUE );
	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
}


void TestAllGroup(void *vb,void *ib,int v_num,int face_num)
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	pd3dDevice->SetTexture( 0,NULL);
	pd3dDevice->SetTexture( 1,NULL);
	pd3dDevice->SetVertexShader( D3DFVF_R3VERTEX_TEX2 );
    pd3dDevice->SetStreamSource( 0, ((LPDIRECT3DVERTEXBUFFER8)vb), sizeof(_D3DR3VERTEX_TEX2) );
	pd3dDevice->SetIndices( ((LPDIRECT3DINDEXBUFFER8)ib), 0 );
//	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
//		0, v_num,
  //      0, face_num );
	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
		0, 4,
        3, 1 );
}




#include "ToolCollision.h"
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }
void CToolCollisionFace::DrawCollisionPoint()//충돌 점을 뿌린다.
{
	int i;
	_D3DR3VERTEX_TEX0 vertex[1000*3];
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

	if( mColPointCnt == 1 )
		return;
	D3DXMATRIX matWorld;

	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	
	BlendOn(_BLEND_BRIGHT);
    pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
    pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(0.01f) );


	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	int cnt=0;
	for( i=1; i< mColPointCnt; i++)
	{
		DWORD tc;
		if( mSelectVertex == i )
			tc=0xfff71f4f;
		else
			tc=0xff7ffccf;
		float *sv=(float *)&mColPoint[i].v;

		vertex[cnt].x = sv[0];
		vertex[cnt].y = sv[1];
		vertex[cnt].z = sv[2];
		vertex[cnt].color = tc;
		cnt++;

		if( cnt>=2000)
		{
			DrawPrimitiveUPPoint(cnt,vertex);
			cnt=0;
		}
	}
	DrawPrimitiveUPPoint(cnt,vertex);
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
    pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );

	BlendOff();
}

void CToolCollisionFace::DrawCollisionPoly()//충돌 폴리곤을 뿌린다.
{
	int i,j;
	_D3DR3VERTEX_TEX0 vertex[1000*3];

	GetD3dDevice()->SetRenderState( D3DRS_ZWRITEENABLE ,FALSE );
	D3DXMATRIX matWorld;
	if( mOnePointCreate )
	{
		i=mColLineCnt;
		if( mColLine[i].end_v == 0)
		{
			D3DXMatrixIdentity( &matWorld );
			matWorld.m[3][0] = mColPoint[mColLine[i].start_v].v[0];
			matWorld.m[3][1] = mColPoint[mColLine[i].start_v].v[1];
			matWorld.m[3][2] = mColPoint[mColLine[i].start_v].v[2];
			GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
			::DrawTestBox(1,1,50,0xf0ffff00);

			D3DXMatrixIdentity( &matWorld );
			GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
		}
	}
	D3DXMatrixIdentity( &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
	if( mColLineCnt == 1)
		return;
	
	int cnt=0;

#define _BACK_C  0x7f000000
//	DWORD _FRONT_C[3]={0x5fff009f,0x7ffc6fbf,0x5f8f027f};
	DWORD _FRONT_C[3]={0x5fffef9f,0x7cffefbf,0x5fefaf9f};
	DWORD _S_FRONT_C[3]={0x5fff009f,0x7cff6fbf,0x5f8f027f};
	DWORD _F_FRONT_C[3]={0x7f00ffff,0x7f00ffff,0x7f00ffff};

	BlendOn(_BLEND_BRIGHT);

	int r_cnt=0;
	for( i=1; i< mColLineCnt; i++)
	{
		DWORD tc=_BACK_C;
		float *sv=(float *)&mColPoint[mColLine[i].start_v].v;
		float *ev=(float *)&mColPoint[mColLine[i].end_v].v;

		vertex[cnt].x = sv[0];
		vertex[cnt].y = sv[1];
		vertex[cnt].z = sv[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = ev[0];
		vertex[cnt].y = ev[1]+mColLine[i].height;
		vertex[cnt].z = ev[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = ev[0];
		vertex[cnt].y = ev[1];
		vertex[cnt].z = ev[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = sv[0];
		vertex[cnt].y = sv[1];
		vertex[cnt].z = sv[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = sv[0];
		vertex[cnt].y = sv[1]+mColLine[i].height;
		vertex[cnt].z = sv[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = ev[0];
		vertex[cnt].y = ev[1]+mColLine[i].height;
		vertex[cnt].z = ev[2];
		vertex[cnt].color = tc;
		cnt++;

		if( cnt>=2000)
		{
			BlendOn(_BLEND_OPPA);
			GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
			DrawPrimitiveUPPoly(cnt/3,vertex);

			BlendOn(_BLEND_BRIGHT);

			for(j=cnt-1; j>=0; j--)
			{
				if( mColLine[(r_cnt+j)/6+1].attr & _LINE_ATTR_SELECT)
				{
					vertex[j].color = _S_FRONT_C[(j/6)%3];
				}
				else
				if( mColLine[(r_cnt+j)/6+1].attr & _LINE_ATTR_FREEZE)
				{
					vertex[j].color = _F_FRONT_C[(j/6)%3];
				}
				else
				{
					vertex[j].color = _FRONT_C[(j/6)%3];
				}
			}
			GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
			DrawPrimitiveUPPoly(cnt/3,vertex);
			r_cnt+=cnt;
			cnt=0;

		}
	}
	BlendOn(_BLEND_OPPA);
	GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
	DrawPrimitiveUPPoly(cnt/3,vertex);
	for(j=cnt-1; j>=0; j--)
	{
		if( mColLine[(r_cnt+j)/6+1].attr & _LINE_ATTR_SELECT)
		{
			vertex[j].color = _S_FRONT_C[(j/6)%3];
		}
		else
		if( mColLine[(r_cnt+j)/6+1].attr & _LINE_ATTR_FREEZE)
		{
			vertex[j].color = _F_FRONT_C[(j/6)%3];
		}
		else
		{
			vertex[j].color = _FRONT_C[(j/6)%3];
		}
	}
//	for(j=0 ;j<cnt; j++)
//		vertex[j].color = _FRONT_C[(j/6)%3];
	BlendOn(_BLEND_BRIGHT);
	GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	DrawPrimitiveUPPoly(cnt/3,vertex);

	GetD3dDevice()->SetRenderState( D3DRS_ZWRITEENABLE ,TRUE );
	BlendOff();
}

void CBsp::DrawCollisionPoly()//충돌 폴리곤을 뿌린다.
{
	int i,j;
	_D3DR3VERTEX_TEX0 vertex[1000*3];

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
	
	int cnt=0;

#define _BACK_C  0x7f000000
//	DWORD _FRONT_C[3]={0x5fff009f,0x7ffc6fbf,0x5f8f027f};
	DWORD _FRONT_C[3]={0x5f00ff9f,0x7c6fffbf,0x5f028f7f};
	DWORD _S_FRONT_C[3]={0x5fff009f,0x7cff6fbf,0x5f8f027f};
	DWORD _F_FRONT_C[3]={0x5f00ffff,0x7c00ffff,0x5f00ffff};

	BlendOn(_BLEND_BRIGHT);

	int r_cnt=0;
	for( i=1; i< (int)mCFLineNum; i++)
	{
		DWORD tc=_BACK_C;
		float *sv=(float *)&mCFVertex[mCFLine[i].start_v];
		float *ev=(float *)&mCFVertex[mCFLine[i].end_v];

		vertex[cnt].x = sv[0];
		vertex[cnt].y = sv[1];
		vertex[cnt].z = sv[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = ev[0];
		vertex[cnt].y = ev[1]+mCFLine[i].height;
		vertex[cnt].z = ev[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = ev[0];
		vertex[cnt].y = ev[1];
		vertex[cnt].z = ev[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = sv[0];
		vertex[cnt].y = sv[1];
		vertex[cnt].z = sv[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = sv[0];
		vertex[cnt].y = sv[1]+mCFLine[i].height;
		vertex[cnt].z = sv[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = ev[0];
		vertex[cnt].y = ev[1]+mCFLine[i].height;
		vertex[cnt].z = ev[2];
		vertex[cnt].color = tc;
		cnt++;

		if( cnt>=2000)
		{
			BlendOn(_BLEND_OPPA);
			GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
			DrawPrimitiveUPPoly(cnt/3,vertex);

			BlendOn(_BLEND_BRIGHT);

			for(j=cnt-1; j>=0; j--)
			{
				if( mCFLine[j/6+1].attr & _LINE_ATTR_SELECT)
				{
					vertex[j].color = _S_FRONT_C[(j/6)%3];
				}
				else
				if( mCFLine[j/6+1].attr & _LINE_ATTR_FREEZE)
				{
					vertex[j].color = _F_FRONT_C[(j/6)%3];
				}
				else
				{
					vertex[j].color = _FRONT_C[(j/6)%3];
				}
			}
			GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
			DrawPrimitiveUPPoly(cnt/3,vertex);
			r_cnt+=cnt;
			cnt=0;

		}
	}
	BlendOn(_BLEND_OPPA);
	GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
	DrawPrimitiveUPPoly(cnt/3,vertex);
	for(j=cnt-1; j>=0; j--)
	{
		if( mCFLine[(r_cnt+j)/6+1].attr & _LINE_ATTR_SELECT)
		{
			vertex[j].color = _S_FRONT_C[(j/6)%3];
		}
		else
		if( mCFLine[(r_cnt+j)/6+1].attr & _LINE_ATTR_FREEZE)
		{
			vertex[j].color = _F_FRONT_C[(j/6)%3];
		}
		else
		{
			vertex[j].color = _FRONT_C[(j/6)%3];
		}
	}
//	for(j=0 ;j<cnt; j++)
//		vertex[j].color = _FRONT_C[(j/6)%3];
	BlendOn(_BLEND_BRIGHT);
	GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	DrawPrimitiveUPPoly(cnt/3,vertex);

	BlendOff();
}

void CBsp::DrawLeafCollisionPoly(short leaf_id)//리프안의 충돌 폴리곤을 뿌린다.
{
	int i,j;
	_D3DR3VERTEX_TEX0 vertex[1000*3];

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
	
	int cnt=0;

#define _BACK_C  0x7f000000
//	DWORD _FRONT_C[3]={0x5fff009f,0x7ffc6fbf,0x5f8f027f};
	DWORD _FRONT_C[3]={0x5f00ff9f,0x7c6fffbf,0x5f028f7f};
	DWORD _S_FRONT_C[3]={0x5fff009f,0x7cff6fbf,0x5f8f027f};

	BlendOn(_BLEND_BRIGHT);

	int r_cnt=0,k;
	for( k=0; k< mCFLeaf[leaf_id].line_num; k++)
	{
		i = mCFLineId[mCFLeaf[leaf_id].start_id+k];//-1;
		DWORD tc=_BACK_C;
		float *sv=(float *)&mCFVertex[mCFLine[i].start_v];
		float *ev=(float *)&mCFVertex[mCFLine[i].end_v];

		vertex[cnt].x = sv[0];
		vertex[cnt].y = sv[1];
		vertex[cnt].z = sv[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = ev[0];
		vertex[cnt].y = ev[1]+mCFLine[i].height;
		vertex[cnt].z = ev[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = ev[0];
		vertex[cnt].y = ev[1];
		vertex[cnt].z = ev[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = sv[0];
		vertex[cnt].y = sv[1];
		vertex[cnt].z = sv[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = sv[0];
		vertex[cnt].y = sv[1]+mCFLine[i].height;
		vertex[cnt].z = sv[2];
		vertex[cnt].color = tc;
		cnt++;

		vertex[cnt].x = ev[0];
		vertex[cnt].y = ev[1]+mCFLine[i].height;
		vertex[cnt].z = ev[2];
		vertex[cnt].color = tc;
		cnt++;

		if( cnt>=2000)
		{
			BlendOn(_BLEND_OPPA);
			GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
			DrawPrimitiveUPPoly(cnt/3,vertex);

			BlendOn(_BLEND_BRIGHT);

			for(j=cnt-1; j>=0; j--)
			{
				if( mCFLine[j/6+1].attr & _LINE_ATTR_SELECT)
				{
					vertex[j].color = _S_FRONT_C[(j/6)%3];
				}
				else
				{
					vertex[j].color = _FRONT_C[(j/6)%3];
				}
			}
			GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
			DrawPrimitiveUPPoly(cnt/3,vertex);
			r_cnt+=cnt;
			cnt=0;

		}
	}
	BlendOn(_BLEND_OPPA);
	GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
	DrawPrimitiveUPPoly(cnt/3,vertex);
	for(j=cnt-1; j>=0; j--)
	{
		if( mCFLine[(r_cnt+j)/6+1].attr & _LINE_ATTR_SELECT)
		{
			vertex[j].color = _S_FRONT_C[(j/6)%3];
		}
		else
		{
			vertex[j].color = _FRONT_C[(j/6)%3];
		}
	}
//	for(j=0 ;j<cnt; j++)
//		vertex[j].color = _FRONT_C[(j/6)%3];
	BlendOn(_BLEND_BRIGHT);
	GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	DrawPrimitiveUPPoly(cnt/3,vertex);

	BlendOff();
}
void CBsp::RenderEnvironment(DWORD id)	//환경이 존재하는... 비,눈,모래폭풍.
{
	float mat[4][4],r_matrix[4][4];
	_R3ENGINE_STATE *state= GetR3State();

	MatrixIdentity(mat);
	MatrixCopy(r_matrix,mat);
	mat[3][0]=-100+state->mInvMatView.m[3][0];
	mat[3][1]=state->mInvMatView.m[3][1];
	mat[3][2]=-100+state->mInvMatView.m[3][2];
	switch( id )
	{
/*
	case 1:	//모래먼지다...
		mEnvEntity->DrawEntity(mat,0xffffffff);
		break;
*/
	case 1:	//일반적인 파티클 사용하는거..
		mat[3][0]=state->mInvMatView.m[3][0];
		mat[3][1]=state->mInvMatView.m[3][1];
		mat[3][2]=state->mInvMatView.m[3][2];
		mEnvParticle->SetCreatePos(mat[3]);
		mEnvParticle->mFlag|=_PARTICLE_ATTR_BOUNDED_BOX;	//경계속성을 집어넣는다.
		if( mEnvParticle->Loop()==0)
			break;
		mEnvParticle->SetStartBoxArea();
		MatrixCopy(mEnvParticle->mRotMat,r_matrix);
		DrawCParticle(mEnvParticle,mEnvParticle->mEntity,mat);	//미리 버텍스버퍼에 저장해야된다.
//		mEnvParticle->R
		break;
	case 2:	//코라 파티클 올라가는거다...
		mat[3][0]=state->mInvMatView.m[3][0];
		mat[3][1]=state->mInvMatView.m[3][1];
		mat[3][2]=state->mInvMatView.m[3][2];
		mEnvParticle->SetCreatePos(mat[3]);
		mEnvParticle->mFlag|=_PARTICLE_ATTR_BOUNDED_BOX;	//경계속성을 집어넣는다.
		Vector3f bb_min,bb_max;
		bb_min[0] = -4256;
		bb_min[1] = -5779;
		bb_min[2] = -4591;
		bb_max[0] = 4385;
		bb_max[1] = 3139;
		bb_max[2] = 4943;
		DWORD state = mEnvParticle->GetParticleState();
		if( IsCollisionBBoxPoint(bb_min,bb_max,mat[3]) )
			mEnvParticle->SetParticleState(_PARTICLE_STATE_STOP);		//실내 영역에서는 정지..
		else
		{
			if( state == _PARTICLE_STATE_STOP)
			{
				mEnvParticle->SetParticleState(_PARTICLE_STATE_START);
			}
		}

		if( mEnvParticle->Loop()==0)
			break;
		mEnvParticle->SetStartBoxArea();
		MatrixCopy(mEnvParticle->mRotMat,r_matrix);
		DrawCParticle(mEnvParticle,mEnvParticle->mEntity,mat);	//미리 버텍스버퍼에 저장해야된다.
//		mEnvParticle->R
		break;
	}
//	GetRenderState();
}

static float GetSwimU(float time)
{
	return ((float)sin(time) * 0.15f);
}
static float GetSwimV(float time)
{
	return ((float)cos(time) * 0.14f);
}


void DrawFullScreenLava(DWORD xsize,DWORD ysize,void *tex,DWORD color,float amplitude,float speed)
{
	_D3DR3TLVERTEX_TEX1 v_buf[2048];
	int cnt=0;

	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();

#define _XLENG_ 8
#define _YLENG_ 8
	DWORD i,j;
	float u_fix = 1/(float)_XLENG_;
	float v_fix = 1/(float)_YLENG_;
	float r_time = R3GetTime();
	_D3DR3TLVERTEX_TEX1 src_buf[_XLENG_+1+4][_YLENG_+1+4];

//	float uv[_XLENG_+1][_YLENG_+1][2];
	for(i=0; i<_YLENG_+1; i++)
	{
		for(j=0; j<_XLENG_+1; j++)
		{
			src_buf[j][i].u=u_fix*j+GetSwimU(r_time*speed+v_fix*i*20.0f)*amplitude/(8.0f);
			src_buf[j][i].v=v_fix*i+GetSwimV(r_time*speed+u_fix*j*20.0f)*amplitude/(8.0f);
			src_buf[j][i].color=color;
			src_buf[j][i].z=0.1f;
			src_buf[j][i].w=0.5f;
			src_buf[j][i].x=j*(xsize/(float)_XLENG_);
			src_buf[j][i].y=i*(ysize/(float)_YLENG_);
		}
	}

	for(i=0; i<_YLENG_; i++)
	{
		for(j=0; j<_XLENG_; j++)
		{
			memcpy(&v_buf[cnt++],&src_buf[j][i],sizeof(_D3DR3TLVERTEX_TEX1));
			memcpy(&v_buf[cnt++],&src_buf[j][i+1],sizeof(_D3DR3TLVERTEX_TEX1));
			memcpy(&v_buf[cnt++],&src_buf[j+1][i+1],sizeof(_D3DR3TLVERTEX_TEX1));

			memcpy(&v_buf[cnt++],&src_buf[j][i],sizeof(_D3DR3TLVERTEX_TEX1));
			memcpy(&v_buf[cnt++],&src_buf[j+1][i+1],sizeof(_D3DR3TLVERTEX_TEX1));
			memcpy(&v_buf[cnt++],&src_buf[j+1][i],sizeof(_D3DR3TLVERTEX_TEX1));
		}
	}


	pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTexture(0, (LPDIRECT3DTEXTURE8)tex);
	}
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	DrawTLVertex(tex,cnt/3,FALSE,(_LPD3DR3VERTEX)(&v_buf[0]) );	//미리 버텍스버퍼에 저장해야된다.

	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}
