#include "windows.h"
#include "entity.h"
#include "multilayer.h"
#include "core.h"
#include "commonutil.h"
#include "r3math.h"
#include "r3VertexShader.h"

//엔티티에서 렌더부는 따로 둔다.
//-----------------------------------엔티티 관련...------------------------------------------
void CEntity::DrawOneMatGroup(_ENTITY_M_GROUP *m_group)
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	_R3MATERIAL* mat = &mMat[m_group->MtlId];
	_D3DR3VERTEX_TEX1 *vertexbp;

//	pd3dDevice->SetVertexShader( D3DFVF_R3VERTEX_TEX1 );
	if( m_group->MtlId == -1 )
	{
		pd3dDevice->SetTexture( 0,NULL);
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
			m_group->VBMinIndex, m_group->VCnt,
			m_group->IBMinIndex, m_group->TriNum );
		return;
	}

	DWORD flag=0,i;

	for(i=0; i<mat->m_dwLayerNum; i++)
		flag|=mat->m_Layer[i].m_dwFlag;

	for(i=0; i<mat->m_dwLayerNum; i++)
	{
		BOOL is_tex_mat=FALSE;
		
		D3DXMATRIX tex_mat;
		is_tex_mat=GetTextureMatrix(mat,i,&tex_mat,mStartTime);
		if( is_tex_mat )
		{
			pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			pd3dDevice->SetTransform(D3DTS_TEXTURE0,&tex_mat );
		}
		if( mat->m_Layer[i].m_dwFlag & _NEED_LOCK_FLAG )	//현재 레이어에 쓸일이 있으면
		{
			vertexbp=(_D3DR3VERTEX_TEX1 *)mStaticVertexBuffer.VPLock(m_group->VBMinIndex*sizeof(_D3DR3VERTEX_TEX1),m_group->VCnt*sizeof(_D3DR3VERTEX_TEX1),0);

			MakeUV(vertexbp,m_group->VCnt,m_group->MultiSourceUV,mat,i);

			mStaticVertexBuffer.VPUnLock();	//언락 해주자.
		}

		if( mFlag & _DO_NOT_MANAGE_TEXTURE )	//일단 텍스쳐 메니지를 쓰나 확인..
		{
			LPDIRECT3DTEXTURE8 *ppTex=(LPDIRECT3DTEXTURE8 *)mIndependencyTex;
			pd3dDevice->SetTexture( 0,ppTex[mat->m_Layer[i].m_iSurface-mStartTexID] );
		}
		else
		if( mat->m_Layer[i].m_dwFlag & _ANI_TILE_TEXTURE )	//애니 텍스쳐면...
		{
			int add_id=GetTileAniTextureAddId(mat,i,mStartTime);
			pd3dDevice->SetTexture( 0,R3GetSurface(mat->m_Layer[i].m_iSurface+add_id));
		}
		else
			pd3dDevice->SetTexture( 0,R3GetSurface(mat->m_Layer[i].m_iSurface));

		if( mat->m_Layer[i].m_dwAlphaType )
			BlendOn(mat->m_Layer[i].m_dwAlphaType);
/*
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
//	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xff80ff80);
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,mMapColor);

	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
*/
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
			m_group->VBMinIndex, m_group->VCnt,
			m_group->IBMinIndex, m_group->TriNum );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		if( is_tex_mat )
		{
			pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
		}
		if(mat->m_Layer[i].m_dwFlag &_ANI_ALPHA_FLICKER)//점멸
		{
			pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );  
			pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		}

		if( mat->m_Layer[i].m_dwAlphaType )
			BlendOff();
	}
}


/*
BOOL CEntity::OnlyFrameMove(float world_mat[4][4],DWORD color,float ani_frame)	//
{
	for(i=0; i<mMatGroupNum; i++)
	{
		GetObjectMatrix(w_matrix,mMatGroup[i].ObjectId,mObject,ani_frame);	//애니메이션 매트릭스가 있으면 가져온다.
	}
}
*/
BOOL CEntity::DrawEntity(float world_mat[4][4],DWORD color,float ani_frame)	//
{
	PrepareAnimation();	//애니메이션 준비.

	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();
	_R3ENGINE_STATE *r3_state=GetR3State();

	if( mIsEntityLoad==FALSE )
		return FALSE;
	if(mStaticVertexBuffer.m_lpVertexBuffer==NULL)
		return FALSE;

	D3DMATERIAL8 mtrl;
	memset(&mtrl,0,sizeof(D3DMATERIAL8));

	float alpha = (color>>24)/255.0f;

	if( mFlag & _ENTITY_Z_DISABLE )
        pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	mtrl.Diffuse.a=alpha;
	mtrl.Diffuse.r=((color>>16)&0xff)/255.0f;
	mtrl.Diffuse.g=((color>>8)&0xff)/255.0f;
	mtrl.Diffuse.b=((color>>0)&0xff)/255.0f;
	mtrl.Diffuse.r/=mtrl.Diffuse.r;
	mtrl.Diffuse.g/=mtrl.Diffuse.g;
	mtrl.Diffuse.b/=mtrl.Diffuse.b;
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	if( (color&0x00ffffff) == 0x00ffffff )
	{
		pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1 );//D3DMCS_MATERIAL );
		pd3dDevice->LightEnable( 0,FALSE);
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE);//FALSE );  
	}
	else
	{
		mtrl.Emissive.a=alpha;
		mtrl.Emissive.r=((color>>16)&0xff)/255.0f;
		mtrl.Emissive.g=((color>>8)&0xff)/255.0f;
		mtrl.Emissive.b=((color>>0)&0xff)/255.0f;
		pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );  
	}
	pd3dDevice->SetMaterial( &mtrl );
	pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
	pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );

	SetVPIPTex1(mStaticVertexBuffer.m_lpVertexBuffer,mStaticIndexedBuffer.m_lpIndexBuffer);

	//multiply 2까지 지원.
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );

//	if( r3_state->mIsFog )
//		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
	float w_matrix[4][4];

	if( ani_frame == -1 )
		ani_frame=(R3GetTime()+mAddFrame)*30.0f;

	int i;

	if( mFlag & _ENTITY_MAP_COLOR )
	{
 		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
		pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,mMapColor);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	}
	else
	{
		MultiTexOff();
	}

	for(i=0; i<mMatGroupNum; i++)
	{
		GetObjectMatrix(w_matrix,mMatGroup[i].ObjectId,mObject,ani_frame);	//애니메이션 매트릭스가 있으면 가져온다.
		if( mFlag & _EF_BILLBOARD )
		{
			D3DXMATRIX cal_mat,billboard;
			billboard=r3_state->mInvMatView;
			billboard.m[3][0] = 0;			billboard.m[3][1] = 0;			billboard.m[3][2] = 0;
			MatrixMultiply(w_matrix,billboard.m,w_matrix);
		}
		MatrixMultiply(w_matrix,world_mat,w_matrix);
		GetD3dDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)w_matrix );

//		mEntityPrepareAnimation();	//애니메이션 준비.
		if( alpha != 1 )
			BlendOn(_BLEND_ONLY_TRANSPARENCY);
		if( mMat[mMatGroup[i].MtlId].m_Layer[0].m_dwAlphaType != 0 )
		{
			if( mMat[mMatGroup[i].MtlId].m_Layer[0].m_dwAlphaType & _MAT_ZWRITE )
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE ,TRUE );
			else
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE ,FALSE );
			if( GetR3State()->mIsFog )
				pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		}
		else
		{
			if( GetR3State()->mIsFog )
				pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE ,TRUE );
		}

		DrawOneMatGroup(&mMatGroup[i]);
//		if( mMat[mMatGroup[i].MtlId].m_dwAlphaType[0] != 0 )
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE ,TRUE );
	}
	if( mFlag & _ENTITY_MAP_COLOR )
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	BlendOff();
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );  
	pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
	if( (color&0x00ffffff) == 0x00ffffff )
		pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );

	if( mFlag & _ENTITY_Z_DISABLE )
        pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	return TRUE;
}
BOOL CEntity::OnceDrawEntity(float world_mat[4][4],DWORD color)	//월드매트릭스를 넘겨준다.
{
	if( mFrame > mObject->frames )
		return FALSE;
	DrawEntity(world_mat,color,mFrame);
	mFrame += R3GetLoopTime()*30;
	return TRUE;
}


//---버텍스 쉐이더용.
void CEntity::DrawOneMatGroupVS(_ENTITY_M_GROUP *m_group)
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	_R3MATERIAL* mat = &mMat[m_group->MtlId];

	if( m_group->MtlId == -1 )
	{
		pd3dDevice->SetTexture( 0,NULL);
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
			m_group->VBMinIndex, m_group->VCnt,
			m_group->IBMinIndex, m_group->TriNum );
		return;
	}

	DWORD i;

	for(i=0; i<mat->m_dwLayerNum; i++)
	{
		BOOL is_tex_mat=FALSE;
		
		D3DXMATRIX tex_mat;
		if( mFlag & _DO_NOT_MANAGE_TEXTURE )	//일단 텍스쳐 메니지를 쓰나 확인..
		{
			LPDIRECT3DTEXTURE8 *ppTex=(LPDIRECT3DTEXTURE8 *)mIndependencyTex;
			pd3dDevice->SetTexture( 0,ppTex[mat->m_Layer[i].m_iSurface-mStartTexID] );
		}
		else
		if( mat->m_Layer[i].m_dwFlag & _ANI_TILE_TEXTURE )	//애니 텍스쳐면...
		{
			int add_id=GetTileAniTextureAddId(mat,i);
			pd3dDevice->SetTexture( 0,R3GetSurface(mat->m_Layer[i].m_iSurface+add_id));
		}
		else
			pd3dDevice->SetTexture( 0,R3GetSurface(mat->m_Layer[i].m_iSurface));

		if( mat->m_Layer[i].m_dwAlphaType )
			BlendOn(mat->m_Layer[i].m_dwAlphaType);

		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
			m_group->VBMinIndex, m_group->VCnt,
			m_group->IBMinIndex, m_group->TriNum );

		if( mat->m_Layer[i].m_dwAlphaType )
			BlendOff();
	}
}



void CEntity::SetVertexShaderID(_ENTITY_LIST *e_list,DWORD color)
{
	float alpha = (color>>24)/255.0f;

	switch(e_list->ShaderID)
	{
	case 1 :	
		SetGrassVS(mAddFrame,e_list->Factor,alpha,mMapColor);
		break;	//풀움직임 1.
	case 2 :	//작업 해야된다..
		SetGrassVS(mAddFrame,e_list->Factor,alpha,mMapColor);
		break;	//풀움직임 2.
	}
}

BOOL CEntity::DrawEntityVS(_ENTITY_LIST *e_list,float world_mat[4][4],DWORD color)	//월드매트릭스,버텍스쉐이더 쓰는 엔티티출력루틴... 
{
	PrepareAnimation();	//애니메이션 준비.

	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();
	_R3ENGINE_STATE *r3_state=GetR3State();

	if( mIsEntityLoad==FALSE )
		return FALSE;
	if(mStaticVertexBuffer.m_lpVertexBuffer==NULL)
		return FALSE;

	float alpha = (color>>24)/255.0f;

	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	MultiTexOff();
	
	SetVPIPTex1(mStaticVertexBuffer.m_lpVertexBuffer,mStaticIndexedBuffer.m_lpIndexBuffer);

	//각각의 쉐이더에 맞는 함수를 호출한다.
	SetVertexShaderID(e_list,color);


	//multiply 2까지 지원.
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	if( GetR3State()->mIsFog )
	{
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
	}
	/*
	if( mFlag & _ENTITY_MAP_COLOR )
	{
 		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
		pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,mMapColor);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	}
	else
	{
		MultiTexOff();
	}
*/
	float w_matrix[4][4];

	int i;
	for(i=0; i<mMatGroupNum; i++)
	{
		GetObjectMatrix(w_matrix,mMatGroup[i].ObjectId,mObject,0);	//애니메이션 매트릭스가 있으면 가져온다.
		if( mFlag & _EF_BILLBOARD )
		{
			D3DXMATRIX cal_mat,billboard;
			billboard=r3_state->mInvMatView;
			billboard.m[3][0] = 0;			billboard.m[3][1] = 0;			billboard.m[3][2] = 0;
			MatrixMultiply(w_matrix,billboard.m,w_matrix);
		}
		MatrixMultiply(w_matrix,world_mat,w_matrix);

		if( alpha != 1 )
			BlendOn(_BLEND_ONLY_TRANSPARENCY);
		if( mMat[mMatGroup[i].MtlId].m_Layer[0].m_dwAlphaType != 0 )
		{
			if( mMat[mMatGroup[i].MtlId].m_Layer[0].m_dwAlphaType & _MAT_ZWRITE )
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE ,TRUE );
			else
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE ,FALSE );
		}
		else
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE ,TRUE );


//extern void SetWorldViewMatrixVS(float world_mat[4][4]);
		SetWorldViewMatrixVS(w_matrix);
		DrawOneMatGroupVS(&mMatGroup[i]);
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE ,TRUE );
	}

	if( GetR3State()->mIsFog )
	{
		//pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );
	}
	BlendOff();
	return TRUE;
}