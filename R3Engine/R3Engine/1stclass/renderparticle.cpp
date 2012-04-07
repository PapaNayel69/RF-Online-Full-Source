#include "R3Particle.h"
#include "core.h"
#include "R3math.h"
#include "R3D3dTex.h"
#include "R3render.h"
#include "CommonUtil.h"
#include "d3dutil.h"
#include "specialParticle.h"

static float GetDivU(int frame)
{
	float ret;
	if( 4 >= frame )
		ret=2;
	else
	if( 16 >= frame )
		ret=4;
	else
	if( 64 >= frame )
		ret=8;
	else
		ret=16;
	return ret;
}
static float GetDivV(int frame)
{
	float ret;
	if( 2 >= frame )
		ret=1;
	else
	if( 8 >= frame )
		ret=2;
	else
	if( 32 >= frame )
		ret=4;
	else
		ret=8;
	return ret;
}


static void GetTrackScaleAndARGB(_PARTICLE_ELEMENT *element,DWORD *dw_argb,float *scale)
{
	float f_a=max(element->mARGB[0],0);
	f_a=min(f_a,255);
	float f_r=max(element->mARGB[1],0);
	f_r=min(f_r,255);
	float f_g=max(element->mARGB[2],0);
	f_g=min(f_g,255);
	float f_b=max(element->mARGB[3],0);
	f_b=min(f_b,255);

	DWORD a = (DWORD)f_a;
	DWORD r = (DWORD)f_r;
	DWORD g = (DWORD)f_g;
	DWORD b = (DWORD)f_b;
	*scale = element->mScale;
	*dw_argb = (a<<24)|(r<<16)|(g<<8)|b;
}

static void GetBillboard( D3DXMATRIX *mat ,CParticle *particle,float world_mat[4][4])
{
	float f_matrix[4][4];
	Vector3f w_pos;
	_R3ENGINE_STATE *state = GetR3State();


	if( particle->mFlag & _PARTICLE_ATTR_IS_NOT_BILLBOARD)
	{
		if( particle->mFlag & _PARTICLE_ATTR_FREE )	//월드 매트릭스에 의존하는 거라면.
		{
			Vector3fCopy(particle->mCreatePos,w_pos);
		}
		else
		{
			Vector3fCopy(world_mat[3],w_pos);
		}

		if( particle->mFlag & _PARTICLE_ATTR_NO_BILLBOARD)
		{
			if( particle->mFlag & _PARTICLE_ATTR_FREE )	//월드 매트릭스에 의존하는 거라면.
			{
				MatrixCopy(mat->m,particle->mRotMat);
				mat->m[3][0]=0;	mat->m[3][1]=0;	mat->m[3][2]=0;	mat->m[3][3]=1;
			}
			else
				MatrixIdentity(mat->m);
		}
		else
		if( particle->mFlag & _PARTICLE_ATTR_Y_BILLBOARD)
		{
//			GetYBillboardMatrix(mat,state->mInvMatView.m[3],world_mat[3]);//	y 빌보드
			MatrixInvert(f_matrix,particle->mRotMat);
			GetYBillboardMatrix(mat,state->mInvMatView.m[3],w_pos);//	y 빌보드
			MatrixMultiply( mat->m, mat->m, f_matrix  );
		}
		else
		if( particle->mFlag & _PARTICLE_ATTR_Z_BILLBOARD)
		{
			Vector3f fix_v,fix;
			MatrixInvert(f_matrix,particle->mRotMat);
			Vector3fSub(w_pos,state->mInvMatView.m[3],fix_v);
			Vector3fTransform(fix,fix_v,f_matrix);
			Vector3fAdd(w_pos,fix,w_pos);
			GetZBillboardMatrix(mat,state->mInvMatView.m[3],w_pos);//	z 빌보드
		}
//		pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
	}
	else
	{	//빌보드 구하기.
		/*
		if( particle->mFlag & _PARTICLE_ATTR_FREE )
			MatrixInvert(mat->m,state->mMatView.m);
		else
		{
			MatrixMultiply( f_matrix ,state->mMatView.m,particle->mRotMat );
			MatrixInvert(mat->m,f_matrix);
		}
		*/

		if( particle->mFlag & _PARTICLE_ATTR_FREE )
			MatrixInvert(mat->m,state->mMatView.m);
		else
		{
			MatrixMultiply( f_matrix ,state->mMatView.m,particle->mRotMat );
			MatrixInvert(mat->m,f_matrix);
		}
//		MatrixMultiply( f_matrix ,state->mMatView.m,particle->mRotMat );
//		MatrixInvert(mat->m,f_matrix);
		mat->m[3][0]=0;
		mat->m[3][1]=0;
		mat->m[3][2]=0;
	}
}
/*	엔티티 애니메이션이 필요할경우 
	_ENTITY_M_GROUP *mat_group=&entity->mMatGroup[j];
	entity->PrepareAnimation();
	GetObjectMatrix(w_matrix,mat_group->ObjectId,entity->mObject,(R3GetTime()+0)*30.0f);	//애니메이션 매트릭스가 있으면 가져온다.

	MatrixMultiply(w_matrix,billboard.m,w_matrix);
	w_matrix[3][0]+=particle->mElement[i].mPos[0];
	w_matrix[3][1]+=particle->mElement[i].mPos[1];
	w_matrix[3][2]+=particle->mElement[i].mPos[2];
	MatrixMultiply(f_matrix,view_proj,w_matrix);
*/
//엔티티 애니메이션이 필요없는경우 이걸루 한다.
//	MatrixMultiply(f_matrix,view_proj,billboard.m);

inline void GetFinalBilloardMatrix(float final[4][4],float now_bill[4][4],CParticle *particle,DWORD i) //최종 빌보드를 얻어낸다
{
	float t_mat[4][4],t_mat2[4][4];

	if( !(particle->mFlag & (_PARTICLE_ATTR_Y_ROT|_PARTICLE_ATTR_Z_ROT)) )
	{
		MatrixCopy(final,now_bill);
		return;
	}

	if( particle->mFlag &  _PARTICLE_ATTR_Y_ROT && particle->mFlag &  _PARTICLE_ATTR_Z_ROT )
	{
		MatrixRotateZ(t_mat,particle->mElement[i].mZRot );
		MatrixRotateY(t_mat2,particle->mElement[i].mYRot );
		MatrixMultiply(t_mat,t_mat2,t_mat);
		MatrixMultiply(final,now_bill,t_mat);
	}
	else
	if( particle->mFlag &  _PARTICLE_ATTR_Y_ROT )	//y회전이 있는경우.
	{
		MatrixRotateY(t_mat,particle->mElement[i].mYRot );
		MatrixMultiply(final,now_bill,t_mat);
	}
	else	//나머진 z회전이다.
//	if( particle->mFlag &  _PARTICLE_ATTR_Z_ROT )	//z회전이 있는경우.
	{
		MatrixRotateZ(t_mat,particle->mElement[i].mZRot );
		MatrixMultiply(final,now_bill,t_mat);
	}
}


void DrawParticlePolygon(CParticle *particle,CEntity *entity,float world_mat[4][4],DWORD argb=0xffffffff)	//이거전에 반드시 월드 매트릭스 정규화를 해야된다.
{
	int i,j,k;
	_D3DR3VERTEX_TEX1 buf[2048];
	WORD index[2048];
	float f_matrix[4][4],view_proj[4][4];	//최종 메트릭스 

	_R3ENGINE_STATE *state = GetR3State();
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	if( particle->mFlag & _PARTICLE_ATTR_FREE )	//월드 매트릭스에 의존하는 거라면.
	{
		MatrixIdentity(world_mat);
	}
	else
	{
		particle->mCreatePos[0] = 0;
		particle->mCreatePos[1] = 0;
		particle->mCreatePos[2] = 0;
	}

	//---빌보드 매트릭스 구하기..
	D3DXMATRIX billboard,final_billboard;
	GetBillboard( &billboard,particle,world_mat );

	//--매트릭스 준비.
	MatrixMultiply( view_proj ,state->mMatView.m,world_mat );
	MatrixMultiply( view_proj ,state->mMatProj.m,view_proj);

	float x_center = (float)(state->mViewXL/2+state->mViewSx);
	float y_center = (float)(state->mViewYL/2+state->mViewSy);

	float w_matrix[4][4];

	float live_time = particle->mLiveTime/particle->mTimeSpeed;
	int cnt=0;
	int index_cnt=0;
	DWORD dw_argb;

	int mat_id=0;
	if( entity->mMatGroup[0].MtlId > 0)
		mat_id=entity->mMatGroup[0].MtlId;
	_R3MATERIAL* mat = &entity->mMat[mat_id];
	//트랜스폼 정보를 미리...
	SetTransformClipInfo((float)state->mViewXL,(float)state->mViewYL	,x_center ,y_center );

	int root_index=0;
	float scale;

	LPDIRECT3DTEXTURE8 pTex;
	if( entity->mFlag & _DO_NOT_MANAGE_TEXTURE )	//맞나 확인..
	{
		LPDIRECT3DTEXTURE8 *ppTex=(LPDIRECT3DTEXTURE8 *)entity->mIndependencyTex;
		pTex = ppTex[mat->m_Layer[0].m_iSurface-entity->mStartTexID];
	}
	else
	{
		pTex = R3GetSurface(mat->m_Layer[0].m_iSurface);
	}

	//-----zfront 로 접근 된 파티클은 xyzw포맷이다.
	if( particle->mFlag &  _PARTICLE_ATTR_ZFRONT )
	{
		Vector3f z_front;
		z_front[0]=0;
		z_front[1]=0;
		z_front[2]=particle->mZFront;

		_D3DR3TLVERTEX_TEX1 *v_buf=(_D3DR3TLVERTEX_TEX1 *)buf;
		for(i=0; i<particle->mNum; i++)
		{
			if( !particle->mElement[i].mIsLive )
				continue;

			GetFinalBilloardMatrix(final_billboard.m,billboard.m,particle,i); //최종 빌보드를 얻어낸다.

			GetTrackScaleAndARGB(&particle->mElement[i],&dw_argb,&scale);
			if( (argb & 0x00ffffff) != 0x00ffffff )	//색상 강제지정이다.
				dw_argb = (0xff000000&dw_argb)|(argb&0x00ffffff);
			if( (argb & 0xff000000) != 0xff000000 )	//알파 혼합지정이다.
				dw_argb=(((dw_argb>>24)*(argb>>24)>>8)<<24)|(dw_argb&0x00ffffff);
			particle->GetFlickerARGB(i,&dw_argb);

			MatrixIdentity(w_matrix);
			
			w_matrix[0][0]*=scale;
			w_matrix[1][1]*=scale;
			w_matrix[2][2]*=scale;

			w_matrix[3][0]+=particle->mElement[i].mPos[0];
			w_matrix[3][1]+=particle->mElement[i].mPos[1];
			w_matrix[3][2]+=particle->mElement[i].mPos[2];

			MatrixMultiply(w_matrix,w_matrix,final_billboard.m);
			MatrixMultiply(f_matrix,view_proj,w_matrix);

			_ENTITY_M_GROUP *mat_group=entity->mMatGroup;
			float div_u=1;
			float div_v=1;
			float add_u=0;
			float add_v=0;

			if( mat_group->MtlId != -1 )
			{
				_R3MATERIAL *mat = &entity->mMat[mat_group->MtlId];
				if( _ANI_TEXTURE & mat->m_Layer[0].m_dwFlag )
				{
					int frame_num=(int)FixedShortToFloat(mat->m_Layer[0].m_sANITexFrame);
					float su=FixedShortToFloat(mat->m_Layer[0].m_sANITexSpeed);
					su*=particle->mElement[i].mTime;
					int frame = (int)(su)%frame_num;
					div_u=GetDivU(frame_num);
					div_v=GetDivV(frame_num);
					add_u=(frame%((int)div_u))/div_u;
					add_v=((frame+(int)div_u)/((int)div_u)-1)/div_v;
				}
			}

			for(j=0; j<entity->mMatGroupNum; j++)
			{
				mat_group=&entity->mMatGroup[j];
	
				int clip=0;
				for(k=0; k<(int)mat_group->VCnt ;k++)
				{
					_D3DR3VERTEX_TEX1 *tex1=&entity->mStaticVertexBuffer.m_VertexBufferTex1[k+mat_group->VBMinIndex];
					clip += TransformVertex( (float*)&v_buf[cnt+k].x, f_matrix
						,(float*)&tex1->x,z_front);
//					v_buf[cnt+k].color=dw_alpha|0x00ffffff;
					v_buf[cnt+k].color=dw_argb;
					v_buf[cnt+k].u = 1/div_u*tex1->u+add_u;
					v_buf[cnt+k].v = 1/div_v*tex1->v+add_v;
				}				
				if( clip < 0 )	//클리핑 수행.
					continue;
				cnt+=mat_group->VCnt;	//버퍼증가.
				for(k=0; k<(int)mat_group->TriNum*3 ;k++)	//인덱스 버퍼 셋팅.
				{
					index[index_cnt++] = entity->mStaticIndexedBuffer.m_IndexBuffer[k+mat_group->IBMinIndex]+root_index;
				}
				if(cnt > 1024 || index_cnt> 1024)	//넘치는거 방지..
				{
					DrawIndexedTLVertex(pTex
						,index_cnt/3,index_cnt,particle->mAlphaType,index,(_LPD3DR3VERTEX)(&v_buf[0]));	//미리 버텍스버퍼에 저장해야된다.
					cnt=0;
					index_cnt=0;
					root_index=0;
				}
				root_index+=mat_group->VCnt;
			}
		}
		
		DrawIndexedTLVertex(pTex
			,index_cnt/3,index_cnt,particle->mAlphaType,index,(_LPD3DR3VERTEX)(&v_buf[0]));	//미리 버텍스버퍼에 저장해야된다.
	}
	else
	{
		float max_dist,min_dist;
		if( particle->mFlag & _PARTICLE_ATTR_BOUNDED_BOX )
		{
			max_dist = (float)(fabs((particle->mStartPos[1][0]-particle->mStartPos[0][0])/2.0f));
			min_dist = max_dist-max_dist/4.0f;
		}
		_D3DR3VERTEX_TEX1 *v_buf=(_D3DR3VERTEX_TEX1 *)buf;
		//항상 살아 있는 경우...
		for(i=0; i<particle->mNum; i++)
		{
			if( !particle->mElement[i].mIsLive )
				continue;
			if( particle->mFlag & _PARTICLE_ATTR_BOUNDED_BOX )	//경계 바운딩 박스이다.
			{
				Vector3f temp_pos;
				float dist;

				Vector3fSub(particle->mElement[i].mPos,particle->mCreatePos,temp_pos);
				temp_pos[1]=0;	//y무시.
				dist = Vector3fDist(temp_pos);
				
				if( dist > max_dist )	//렌더 안해도 된다.
					continue;
				dist = max(dist,min_dist);
				DWORD e_alpha = (DWORD)((1-(dist - min_dist)/(max_dist - min_dist))*255);
//				argb=(((e_alpha)*(argb>>24)>>8)<<24)|(argb&0x00ffffff);
				argb=((e_alpha)<<24)|(argb&0x00ffffff);
			}

			GetFinalBilloardMatrix(final_billboard.m,billboard.m,particle,i); //최종 빌보드를 얻어낸다.
//			GetTrackScaleAndARGB(particle,particle->mElement[i].mTime/live_time,&dw_argb,&scale);
			GetTrackScaleAndARGB(&particle->mElement[i],&dw_argb,&scale);
			if( (argb & 0x00ffffff) != 0x00ffffff )	//색상 강제지정이다.
				dw_argb = (0xff000000&dw_argb)|(argb&0x00ffffff);
			if( (argb & 0xff000000) != 0xff000000 )	//알파 혼합지정이다.
				dw_argb=(((dw_argb>>24)*(argb>>24)>>8)<<24)|(dw_argb&0x00ffffff);
			particle->GetFlickerARGB(i,&dw_argb);

			MatrixIdentity(w_matrix);
			
			w_matrix[0][0]*=scale;
			w_matrix[1][1]*=scale;
			w_matrix[2][2]*=scale;

			w_matrix[3][0]+=particle->mElement[i].mPos[0];
			w_matrix[3][1]+=particle->mElement[i].mPos[1];
			w_matrix[3][2]+=particle->mElement[i].mPos[2];

			if( particle->mFlag & _PARTICLE_ATTR_FREE )	//free는 월드 매트릭스를 안쓴다.
			{
				MatrixMultiply(f_matrix,w_matrix,final_billboard.m);
			}
			else
			{
				MatrixMultiply(w_matrix,w_matrix,final_billboard.m);
				MatrixMultiply(f_matrix,world_mat,w_matrix);
			}

			_ENTITY_M_GROUP *mat_group=entity->mMatGroup;
			float div_u=1;
			float div_v=1;
			float add_u=0;
			float add_v=0;

			if( mat_group->MtlId != -1 )
			{
				_R3MATERIAL *mat = &entity->mMat[mat_group->MtlId];
				if( _ANI_TEXTURE & mat->m_Layer[0].m_dwFlag )
				{
					int frame_num=(int)FixedShortToFloat(mat->m_Layer[0].m_sANITexFrame);
					float su=FixedShortToFloat(mat->m_Layer[0].m_sANITexSpeed);
					su*=particle->mElement[i].mTime;
					int frame = (int)(su)%frame_num;
					div_u=GetDivU(frame_num);
					div_v=GetDivV(frame_num);
					add_u=(frame%((int)div_u))/div_u;
					add_v=((frame+(int)div_u)/((int)div_u)-1)/div_v;
				}
			}

			for(j=0; j<entity->mMatGroupNum; j++)
			{
				mat_group=&entity->mMatGroup[j];
	
				for(k=0; k<(int)mat_group->VCnt ;k++)
				{
					_D3DR3VERTEX_TEX1 *tex1=&entity->mStaticVertexBuffer.m_VertexBufferTex1[k+mat_group->VBMinIndex];
					Vector3fTransform( (float*)&v_buf[cnt+k].x,(float*)&tex1->x,f_matrix);

					v_buf[cnt+k].color=dw_argb;
					v_buf[cnt+k].u = 1/div_u*tex1->u+add_u;
					v_buf[cnt+k].v = 1/div_v*tex1->v+add_v;
				}
				cnt+=mat_group->VCnt;	//버퍼증가.
				for(k=0; k<(int)mat_group->TriNum*3 ;k++)	//인덱스 버퍼 셋팅.
				{
					index[index_cnt++] = entity->mStaticIndexedBuffer.m_IndexBuffer[k+mat_group->IBMinIndex]+root_index;
				}
				if(cnt > 1024 || index_cnt> 1024)	//넘치는거 방지..
				{
					DrawIndexedTex1VertexUP(pTex
						,index_cnt/3,index_cnt,particle->mAlphaType,index,(_LPD3DR3VERTEX)(&v_buf[0]));	//미리 버텍스버퍼에 저장해야된다.
					cnt=0;
					index_cnt=0;
					root_index=0;
				}
				root_index+=mat_group->VCnt;
			}
		}
		
		DrawIndexedTex1VertexUP(pTex
			,index_cnt/3,index_cnt,particle->mAlphaType,index,(_LPD3DR3VERTEX)(&v_buf[0]));	//미리 버텍스버퍼에 저장해야된다.
	}
}


inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

void DrawParticlePointSprite(CParticle *particle)	//미리 버텍스버퍼에 저장해야된다.
{
	int i;
	_D3DR3VERTEX_TEX1 v_buf[2048];
	_R3ENGINE_STATE *state = GetR3State();

	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

    pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
    pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(4.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(2.00f) );

//    pd3dDevice->SetStreamSource( 0, v_buf, sizeof(_D3DR3VERTEX_TEX1) );
	pd3dDevice->SetVertexShader( D3DFVF_R3VERTEX_TEX1 );
    pd3dDevice->SetTexture(0,NULL);
	
	float live_time = particle->mLiveTime/particle->mTimeSpeed;
	int cnt=0;
	DWORD dw_argb;
	float scale;

	for(i=0; i<particle->mNum; i++)
	{
		if( particle->mElement[i].mIsLive )
		{
//			GetTrackScaleAndARGB(particle,particle->mElement[i].mTime/live_time,&dw_argb,&scale);
			GetTrackScaleAndARGB(&particle->mElement[i],&dw_argb,&scale);
			particle->GetFlickerARGB(i,&dw_argb);

			memcpy(&v_buf[cnt].x,particle->mElement[i].mPos,sizeof(Vector3f));
			v_buf[cnt].color=dw_argb;
			cnt++;
			if(cnt >= 1024 )	//넘치는거 방지..
			{
				pd3dDevice->DrawPrimitiveUP(D3DPT_POINTLIST,cnt,v_buf,sizeof(_D3DR3VERTEX_TEX1));
				cnt=0;
			}
		}
	}
	if( cnt )
		pd3dDevice->DrawPrimitiveUP(D3DPT_POINTLIST,cnt,v_buf,sizeof(_D3DR3VERTEX_TEX1));

    pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
}

void DrawCParticle(CParticle *particle,CEntity *entity,float world_mat[4][4],DWORD argb)	//미리 버텍스버퍼에 저장해야된다.
{
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
//	BlendOn(_BLEND_BRIGHT);
	BlendOn(particle->mAlphaType);

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
//	pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
	if( particle->mFlag & _PARTICLE_ATTR_ZDISABLE )
        pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	if( !(particle->mFlag & _PARTICLE_ATTR_ZWRITE))
        pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	//multiply 2까지 지원.
	MultiTexOff();
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );


	if( particle->mFlag & _PARTICLE_ATTR_SPECIAL_ID )	//특별처리 파티클.		
	{
		DrawSpectialParticle(particle,entity,world_mat,argb);
	}
	else
	{
		if( entity )
		{
			DrawParticlePolygon(particle,entity,world_mat,argb);
		}
		else
			DrawParticlePointSprite(particle);
	}
	BlendOff();

	if( !(particle->mFlag & _PARTICLE_ATTR_ZWRITE))
        pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	if( particle->mFlag & _PARTICLE_ATTR_ZDISABLE )
        pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	if( GetR3State()->mIsFog )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
//	pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
}
