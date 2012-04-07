#include "MultiLayer.h"

/*
꼭 다시 작업해야될것....
|=처리 결국 버텍스버퍼를 읽어버린다...
안읽고 구조체를 유니온을 설정해서 덮어쓰게 하는 방법으로 할것...



  */


static float stTime;	//타이머..
static Vector3f stCamera;

float GetSwimU(float time)
{
	return ((float)sin(time) * 0.15f);
}
float GetSwimV(float time)
{
	return ((float)cos(time) * 0.14f);
}



/*
Vector2fp MakeUV(float *v,Vector2f uv,int layer_num,Vector3f v_normal)
{
//	stResult[0]=stLayerMat[layer_num]._11*uv[0]+stLayerMat[layer_num]._21*uv[1]+stLayerMat[layer_num]._41;
//	stResult[1]=stLayerMat[layer_num]._12*uv[0]+stLayerMat[layer_num]._22*uv[1]+stLayerMat[layer_num]._42;
	if( stNowMat->m_dwFlag[layer_num] & (_NONE_MATRIX_FLAG) )
	{
		Vector2f temp;
		float su,speed;
		if(stNowMat->m_dwFlag[layer_num] &_UV_LAVA)
		{
			su=stNowMat->m_fUVLavaWave[layer_num];
			speed=stNowMat->m_fUVLavaSpeed[layer_num]*3;
//			temp[0]=stResult[0]+GetSwimU(r_time*speed+stResult[1]*2*su)*(su/2.0f);	
//			temp[1]=stResult[1]+GetSwimU(r_time*speed+stResult[0]*2*su)*(su/2.0f);
			temp[0]=stResult[0]+GetSwimU(r_time*speed+stResult[1]*20.0f)*su/(8.0f);	
			temp[1]=stResult[1]+GetSwimV(r_time*speed+stResult[0]*20.0f)*su/(8.0f);
			stResult[0]=temp[0];
			stResult[1]=temp[1];
		}
		if(stNowMat->m_dwFlag[layer_num] &_UV_WATER)
		{
			su=stNowMat->m_fUVWater[layer_num]/2.0f;

			float mod=v[2]-(int)v[2];
			temp[0]=stResult[0]+GetSwimV(r_time*su+(mod*3.14f))/4.0f;
			mod=v[0]-(int)v[0];
			temp[1]=stResult[1]+GetSwimV(r_time*su+(mod*3.14f))/3.0f;

//			mod=v0->lz-(int)v0->lz;
//			face->fu[0]=face->org_uv[0]+GetSwimU(Swim_cnt+((int)(v0->lz+10000)%200+mod)*128/100.0f)*5;

			stResult[0]=temp[0];
			stResult[1]=temp[1];
		}
		if(stNowMat->m_dwFlag[layer_num] &_ANI_COLOR_FLICKER)//점멸
		{
			su = stNowMat->m_fANIColorFlicker[layer_num];
			DWORD *c=(DWORD*)&v[3],color;
			if( su < 0)
			{
				color = (DWORD)(r_time*-su*100.0f);
				color&=127;
			}
			else
			{
				color = (DWORD)(sin(r_time*su)*64+64);
			}
			color = (color<<16)|(color<<8)|(color);
			*c&=0xff000000;
			*c|=color;
		}
		if(stNowMat->m_dwFlag[layer_num] &_ANI_ALPHA_FLICKER)//점멸
		{
			su = stNowMat->m_fANIAlphaFlicker[layer_num];
			DWORD *c=(DWORD*)&v[3],color;
			if( su < 0)
			{
				color = (DWORD)(r_time*-su*200.0f);
				color&=255;
			}
			else
			{
				color = (DWORD)(sin(r_time*su)*127+128);
			}
			*c&=0x00ffffff;
			*c|=(color<<24);
		}
		if(stNowMat->m_dwFlag[layer_num] &_UV_ENV)
		{
//			float tx=stViewMatrix._11*v_normal[0]+stViewMatrix._21*v_normal[1]+stViewMatrix._31*v_normal[2];
//			float ty=stViewMatrix._12*v_normal[0]+stViewMatrix._22*v_normal[1]+stViewMatrix._32*v_normal[2];

//			stResult[0]=tx/2.0f+0.5f;
//			stResult[1]=ty/2.0f+0.5f;
		}
		if(stNowMat->m_dwFlag[layer_num] &_UV_METAL)
		{
			Vector3f v_s,v_t;
			if( fabs(v_normal[2]) > 0.98 )
			{
				v_s[0] = 1.0;	v_s[1]=0;	v_s[2]=0;
			}
			else
			{
				v_s[0] = -v_normal[1];	v_s[1]=v_normal[0];	v_s[2]=0;
				Normalize(v_s);
			}
			Vector3fCross(v_s,v_normal,v_t);
			
			
			extern Vector3f S_player;
			stResult[0]=(Vector3fDot(v_s,v)-(Vector3fDot(v_s,S_player)*0.8f))/128.0f;
			stResult[1]=(Vector3fDot(v_t,v)-(Vector3fDot(v_t,S_player)*0.8f))/128.0f;
			
		}
	}
	return stResult;
}
*/


void SetMultiLayerTime(float ftime)
{
	stTime=ftime;
}
float GetMultiLayerTime()
{
	return stTime;
}
void SetMultiLayerCamera(Vector3f camera)
{
	stCamera[0] = camera[0];
	stCamera[1] = camera[1];
	stCamera[2] = camera[2];
}

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


void MakeUV(_D3DR3VERTEX_TEX2 *tex2,int v_num,void *source_uv,void *source_st,_R3MATERIAL *mat,int l_id,float start_time)
{
	D3DXMATRIX temp;
//	DWORD color,alpha;

	int i;
	float su,speed;
	float r_time = stTime - start_time;
	if( _NONE_MATRIX_FLAG & mat->m_Layer[l_id].m_dwFlag)
	{
		if( _UV_METAL & mat->m_Layer[l_id].m_dwFlag )
		{
			Vector3f *s_st=(Vector3f *)source_st;
			su = FixedShortToFloat(mat->m_Layer[l_id].m_sUVMetal)*128.0f;
			for(i=0; i<v_num; i++)
			{
				tex2[i].uv[0][0]=(Vector3fDot(s_st[i*3+1],s_st[i*3])-(Vector3fDot(s_st[i*3+1],stCamera)*0.8f))/su;
				tex2[i].uv[0][1]=(Vector3fDot(s_st[i*3+2],s_st[i*3])-(Vector3fDot(s_st[i*3+2],stCamera)*0.8f))/su;
			}
		}
	}
	else
	{
		Vector2f *s_uv=(Vector2f *)source_uv;
		if(mat->m_Layer[l_id].m_dwFlag &_UV_LAVA)
		{
			su=FixedShortToFloat(mat->m_Layer[l_id].m_sUVLavaWave);
			speed=FixedShortToFloat(mat->m_Layer[l_id].m_sUVLavaSpeed*3);
			for(i=0; i<v_num; i++)
			{
//				float temp_1=uv_matrix._11*s_uv[i][0]+uv_matrix._21*s_uv[i][1]+uv_matrix._41;
//				float temp_2=uv_matrix._12*s_uv[i][0]+uv_matrix._22*s_uv[i][1]+uv_matrix._42;
//				tex2[i].uv[0][0]=temp_1+GetSwimU(r_time*speed+temp_2*20.0f)*su/(8.0f);	
//				tex2[i].uv[0][1]=temp_2+GetSwimV(r_time*speed+temp_1*20.0f)*su/(8.0f);
				tex2[i].uv[0][0]=s_uv[i][0]+GetSwimU(r_time*speed+s_uv[i][1]*20.0f)*su/(8.0f);	
				tex2[i].uv[0][1]=s_uv[i][1]+GetSwimV(r_time*speed+s_uv[i][0]*20.0f)*su/(8.0f);
			}
		}
		else
		{/*
			if( _MATRIX_FLAG & mat->m_dwFlag[l_id] )
			{
				for(i=0; i<v_num; i++)
				{
					tex2[i].uv[0][0]=uv_matrix._11*s_uv[i][0]+uv_matrix._21*s_uv[i][1]+uv_matrix._41;
					tex2[i].uv[0][1]=uv_matrix._12*s_uv[i][0]+uv_matrix._22*s_uv[i][1]+uv_matrix._42;
				}
			}
			else*/
			{
				for(i=0; i<v_num; i++)
				{
					tex2[i].uv[0][0]=s_uv[i][0];
					tex2[i].uv[0][1]=s_uv[i][1];
				}
			}
		}
	}
	/*
	if( _FLICKER_FLAG & mat->m_dwFlag[l_id] )
	{
		color=0;
		alpha=0;
		if(mat->m_dwFlag[l_id] &_ANI_COLOR_FLICKER)//점멸
		{
			su = FixedShortToFloat(mat->m_sANIColorFlicker[l_id]);
			if( su < 0)
			{
				color = (DWORD)(r_time*-su*100.0f);
				color&=127;
			}
			else
			{
				color = (DWORD)(sin(r_time*su)*64+64);
			}
			color = (color<<16)|(color<<8)|(color);
		}
		else
			color = mat->m_ARGB[l_id]&0x00ffffff;

		if(mat->m_dwFlag[l_id] &_ANI_ALPHA_FLICKER)//점멸
		{
			su = FixedShortToFloat(mat->m_sANIAlphaFlicker[l_id]);
			if( su < 0)
			{
				alpha = (DWORD)(r_time*-su*200.0f);
				alpha &=255;
			}
			else
			{
				alpha = (DWORD)(sin(r_time*su)*127+128);
			}
			if( alpha<=10)
				alpha=10;
			alpha<<=24;
		}
		else
			alpha = mat->m_ARGB[l_id]&0xff000000;
		color = color|alpha;
		for(i=0; i<v_num; i++)
		{
			tex2[i].color=color;
		}
	}*/
}
//#include "r3text.h"
//float al_pha=0;

int GetTileAniTextureAddId(_R3MATERIAL *mat,int l_id,float start_time)	//타일애니텍스쳐추가 아이디를 얻어온다.
{
	int frame_num=(int)mat->m_Layer[l_id].m_iTileAniTexNum;
	if( frame_num == 0)
		return 0;
	float su=FixedShortToFloat(mat->m_Layer[l_id].m_sANITexSpeed);
	su *= (stTime-start_time);
	int frame = (int)(su)%frame_num;
	return frame;
}
#include "core.h"
BOOL GetTextureMatrix(_R3MATERIAL *mat,int l_id,D3DXMATRIX *uv_matrix,float start_time)
{
	BOOL is_true=FALSE;
	float scale=1.0f;
	D3DXMATRIX temp;
	float r_time = stTime - start_time;
	
	D3DXMatrixIdentity( uv_matrix );

	if( _UV_ROTATE & mat->m_Layer[l_id].m_dwFlag )
	{
		D3DXMatrixTranslation (&temp, -0.5f,-0.5f, 0 );
		D3DXMatrixMultiply( uv_matrix, uv_matrix, &temp );
		D3DXMatrixRotationZ( &temp, FixedShortToFloat(mat->m_Layer[l_id].m_sUVRotate)*r_time );
		D3DXMatrixMultiply( uv_matrix, uv_matrix, &temp );
		D3DXMatrixTranslation (&temp, 0.5f,0.5f, 0 );
		D3DXMatrixMultiply( uv_matrix, uv_matrix, &temp );
		is_true=TRUE;
	}
	if( _UV_SCALE & mat->m_Layer[l_id].m_dwFlag )
	{

		float start = FixedShortToFloat(mat->m_Layer[l_id].m_sUVScaleStart);
		float end   = FixedShortToFloat(mat->m_Layer[l_id].m_sUVScaleEnd);
		DWORD gop=(DWORD)(1.0f/FixedShortToFloat(mat->m_Layer[l_id].m_sUVScaleSpeed)*6000.0f);
		float su=((GetTickCount()%gop)/(float)gop)*2*3.141592f;
		scale=(end-start)*((float)sin(su)+1)*.5f+start;
		D3DXMatrixTranslation (&temp, -0.5f,-0.5f, 0 );
		D3DXMatrixMultiply( uv_matrix, uv_matrix, &temp );
		D3DXMatrixScaling( &temp, scale, scale, 1.0f );
		D3DXMatrixMultiply( uv_matrix, uv_matrix, &temp );
		D3DXMatrixTranslation (&temp, 0.5f,0.5f, 0 );
		D3DXMatrixMultiply( uv_matrix, uv_matrix, &temp );
		is_true=TRUE;
	}
	if( _UV_SCROLL_U & mat->m_Layer[l_id].m_dwFlag || _UV_SCROLL_V & mat->m_Layer[l_id].m_dwFlag )
	{
		D3DXMatrixTranslation (&temp, FixedShortToFloat(mat->m_Layer[l_id].m_sUVScrollU)*r_time/4.0f
			, FixedShortToFloat(mat->m_Layer[l_id].m_sUVScrollV)*r_time/4.0f, 0 );
		D3DXMatrixMultiply( uv_matrix, uv_matrix, &temp );
		is_true=TRUE;

	}
	if( _ANI_TEXTURE & mat->m_Layer[l_id].m_dwFlag )
	{
		is_true=TRUE;
		int frame_num=(int)FixedShortToFloat(mat->m_Layer[l_id].m_sANITexFrame);
		float su=FixedShortToFloat(mat->m_Layer[l_id].m_sANITexSpeed);
		su*=r_time;
		int frame = (int)(su)%frame_num;
		float div_u=GetDivU(frame_num);
		float div_v=GetDivV(frame_num);
		float add_u=(frame%((int)div_u))/div_u;
		float add_v=((frame+(int)div_u)/((int)div_u)-1)/div_v;
		uv_matrix->m[0][0] = 1/div_u;
		uv_matrix->m[1][1] = 1/div_v;
		uv_matrix->m[3][0] = add_u;
		uv_matrix->m[3][1] = add_v;
	}
	if(mat->m_Layer[l_id].m_dwFlag &_ANI_ALPHA_FLICKER)//점멸
	{
		LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();
		float su = FixedShortToFloat(mat->m_Layer[l_id].m_sANIAlphaFlicker);

		DWORD start = mat->m_Layer[l_id].m_sANIAlphaFlickerAni>>8;
		DWORD end = (mat->m_Layer[l_id].m_sANIAlphaFlickerAni&0xff);
		DWORD se_sub = end-start;

		DWORD alpha;
		if( su < 0)
		{
			alpha = (DWORD)(r_time*-su*200.0f);
			//alpha &=255;
			alpha %= se_sub;
			alpha +=start;
		}
		else
		{
			se_sub/=2;
			alpha = (DWORD)(sin(r_time*su)*se_sub+se_sub+start);
		}
//	pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
		D3DMATERIAL8 mtrl;
		memset(&mtrl,0,sizeof(D3DMATERIAL8));
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		alpha*=(mat->m_Layer[l_id].m_ARGB>>24);
		mtrl.Diffuse.a=((alpha)/(255*255.0f));

		mtrl.Diffuse.r=1;
		mtrl.Diffuse.g=1;
		mtrl.Diffuse.b=1;
		mtrl.Emissive.a=1;
		mtrl.Emissive.r=1;
		mtrl.Emissive.g=1;
		mtrl.Emissive.b=1;
		pd3dDevice->SetMaterial( &mtrl );
//	pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );  
		pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
		pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	}
	/*	// 엔티티 컬러 조정하기.....
	{
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
		pd3dDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1 );//D3DMCS_MATERIAL );
	}*/
	if( _UV_METAL & mat->m_Layer[l_id].m_dwFlag )
	{
		_R3ENGINE_STATE *state= GetR3State();
//		float scale = 0.1f;//0.003f;
		float scale = 0.003f;
		uv_matrix->m[0][0]=scale*FixedShortToFloat(mat->m_Layer[l_id].m_sUVMetal);//*(1-state->mMatView.m[1][1]);
		uv_matrix->m[1][1]=scale*FixedShortToFloat(mat->m_Layer[l_id].m_sUVMetal);//*(state->mMatView.m[1][1]*10);
		uv_matrix->m[2][2]=scale*FixedShortToFloat(mat->m_Layer[l_id].m_sUVMetal);//*(state->mMatView.m[1][1]*10);
		is_true=TRUE;
		D3DXMATRIX temp = state->mInvMatView,uv_temp,fix_mat;


		D3DXMatrixIdentity( &fix_mat );
		LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

		if( _UV_METAL_FLOOR & mat->m_Layer[l_id].m_dwFlag )
		{
			fix_mat.m[1][1] = 0;
			fix_mat.m[1][2] = -1;
			fix_mat.m[2][2] = 0;
			fix_mat.m[2][1] = 1;
		}
		temp.m[3][0]=0;
		temp.m[3][1]=0;
		temp.m[3][2]=0;

		uv_temp = *uv_matrix;
		*uv_matrix = uv_temp*temp;
		*uv_matrix=*uv_matrix*fix_mat;

//		uv_matrix->m[3][0]=0.5;	//나중에 다이나믹 라이트할때.
//		uv_matrix->m[3][1]=0.5;

		return is_true;
	}
	if( is_true )	//이동 부분 전환..
	{
		uv_matrix->m[2][0]=uv_matrix->m[3][0];
		uv_matrix->m[2][1]=uv_matrix->m[3][1];
		uv_matrix->m[3][0]=0;
		uv_matrix->m[3][1]=0;
	}
	return is_true;
}

void MakeUVExt(_D3DR3VERTEX_TEX2 *tex2,int v_num,void *source_uv,void *source_st,_R3MATERIAL *mat,int l_id,float start_time)
{
	DWORD color;


	int i;
	float su,speed;
	color=mat->m_Layer[l_id].m_ARGB;
	float r_time = stTime - start_time;

//	color=0x5500ff00;
	if( _NONE_MATRIX_FLAG & mat->m_Layer[l_id].m_dwFlag)
	{
		if( _UV_METAL & mat->m_Layer[l_id].m_dwFlag )
		{
			Vector3f *s_st=(Vector3f *)source_st;
			su = FixedShortToFloat(mat->m_Layer[l_id].m_sUVMetal)*128.0f;
			for(i=0; i<v_num; i++)
			{
				tex2[i].uv[0][0]=(Vector3fDot(s_st[i*3+1],s_st[i*3])-(Vector3fDot(s_st[i*3+1],stCamera)*0.8f))/su;
				tex2[i].uv[0][1]=(Vector3fDot(s_st[i*3+2],s_st[i*3])-(Vector3fDot(s_st[i*3+2],stCamera)*0.8f))/su;
				tex2[i].color = color;
			}
		}
	}
	else
	{
		Vector2f *s_uv=(Vector2f *)source_uv;
		if(mat->m_Layer[l_id].m_dwFlag &_UV_LAVA)
		{
			su=FixedShortToFloat(mat->m_Layer[l_id].m_sUVLavaWave);
			speed=FixedShortToFloat(mat->m_Layer[l_id].m_sUVLavaSpeed*3);
			for(i=0; i<v_num; i++)
			{
//				float temp_1=uv_matrix._11*s_uv[i][0]+uv_matrix._21*s_uv[i][1]+uv_matrix._41;
//				float temp_2=uv_matrix._12*s_uv[i][0]+uv_matrix._22*s_uv[i][1]+uv_matrix._42;
//				tex2[i].uv[0][0]=temp_1+GetSwimU(r_time*speed+temp_2*20.0f)*su/(8.0f);	
//				tex2[i].uv[0][1]=temp_2+GetSwimV(r_time*speed+temp_1*20.0f)*su/(8.0f);
				tex2[i].uv[0][0]=GetSwimU(r_time*speed+s_uv[i][1]*20.0f)*su/(8.0f);	
				tex2[i].uv[0][1]=GetSwimV(r_time*speed+s_uv[i][0]*20.0f)*su/(8.0f);
				tex2[i].color = color;
			}
		}
		else
/*
		if(mat->m_Layer[l_id].m_dwFlag &_MATRIX_FLAG)
		{
			for(i=0; i<v_num; i++)
			{
				tex2[i].uv[0][0]=uv_matrix._11*s_uv[i][0]+uv_matrix._21*s_uv[i][1]+uv_matrix._41;
				tex2[i].uv[0][1]=uv_matrix._12*s_uv[i][0]+uv_matrix._22*s_uv[i][1]+uv_matrix._42;
				tex2[i].color = color;
			}
		}
		else*/
		if(mat->m_Layer[l_id].m_dwFlag &_UV_METAL)
		{
			for(i=0; i<v_num; i++)
			{
				tex2[i].uv[0][0]=s_uv[i][0];
				tex2[i].uv[0][1]=s_uv[i][1];
				tex2[i].color = color;
			}
		}
		else
		{
			if( source_uv )
			{
				for(i=0; i<v_num; i++)
				{
					tex2[i].uv[0][0]=s_uv[i][0];
					tex2[i].uv[0][1]=s_uv[i][1];
					tex2[i].color = color;
				}
			}
		}
	}
	/*
	if( _FLICKER_FLAG & mat->m_Layer[l_id].m_dwFlag )
	{
		color=0;
		alpha=0;
		if(mat->m_Layer[l_id].m_dwFlag &_ANI_COLOR_FLICKER)//점멸
		{
			su = FixedShortToFloat(mat->m_Layer[l_id].m_sANIColorFlicker);
			if( su < 0)
			{
				color = (DWORD)(r_time*-su*100.0f);
				color&=127;
			}
			else
			{
				color = (DWORD)(sin(r_time*su)*64+64);
			}
			color = (color<<16)|(color<<8)|(color);
		}
		else
			color = mat->m_Layer[l_id].m_ARGB&0x00ffffff;

		if(mat->m_Layer[l_id].m_dwFlag &_ANI_ALPHA_FLICKER)//점멸
		{
			su = FixedShortToFloat(mat->m_Layer[l_id].m_sANIAlphaFlicker);
			if( su < 0)
			{
				alpha = (DWORD)(r_time*-su*200.0f);
				alpha &=255;
			}
			else
			{
				alpha = (DWORD)(sin(r_time*su)*127+128);
			}
			if( alpha<=10)
				alpha=10;
			alpha<<=24;
		}
		else
			alpha = mat->m_Layer[l_id].m_ARGB&0xff000000;
		color = color|alpha;
		for(i=0; i<v_num; i++)
		{
			tex2[i].color=color;
		}
	}
	*/
}
//--------------스카이박스용

void MakeUV(_D3DR3VERTEX_TEX1 *tex1,int v_num,void *source_uv,_R3MATERIAL *mat,int l_id)
{
	D3DXMATRIX temp;

	DWORD color=mat->m_Layer[l_id].m_ARGB&0xff000000;
//	DWORD color=mat->m_Layer[l_id].m_ARGB;

	int i;
	float su,speed;

	Vector2f *s_uv=(Vector2f *)source_uv;
	float r_time = stTime;

	if(mat->m_Layer[l_id].m_dwFlag &_UV_LAVA)
	{
		su=FixedShortToFloat(mat->m_Layer[l_id].m_sUVLavaWave);
		speed=FixedShortToFloat(mat->m_Layer[l_id].m_sUVLavaSpeed*3);
		for(i=0; i<v_num; i++)
		{
			tex1[i].u=s_uv[i][0]+GetSwimU(r_time*speed+s_uv[i][1]*20.0f)*su/(8.0f);	
			tex1[i].v=s_uv[i][1]+GetSwimV(r_time*speed+s_uv[i][0]*20.0f)*su/(8.0f);
			tex1[i].color = color|(tex1[i].color&0x00ffffff);
		}
	}
	/*
	else
	if(mat->m_Layer[l_id].m_dwFlag &_MATRIX_FLAG)
	{
		for(i=0; i<v_num; i++)
		{
			tex1[i].u=uv_matrix._11*s_uv[i][0]+uv_matrix._21*s_uv[i][1]+uv_matrix._41;
			tex1[i].v=uv_matrix._12*s_uv[i][0]+uv_matrix._22*s_uv[i][1]+uv_matrix._42;
			tex1[i].color &=0x00ffffff;
			tex1[i].color |= color;
//			tex1[i].color = color|(tex1[i].color&0x00ffffff);
		}
	}*/
	else
	{
		if( _ANI_TEXTURE & mat->m_Layer[l_id].m_dwFlag )
		{
			Vector2f *s_uv=(Vector2f *)source_uv;
			int frame_num=(int)FixedShortToFloat(mat->m_Layer[l_id].m_sANITexFrame);
			su=FixedShortToFloat(mat->m_Layer[l_id].m_sANITexSpeed);
			su*=r_time;
			int frame = (int)(su)%frame_num;
			float div_u=GetDivU(frame_num);
			float div_v=GetDivV(frame_num);
			float add_u=(frame%((int)div_u))/div_u;
			float add_v=((frame+(int)div_u)/((int)div_u)-1)/div_v;
			for(i=0; i<v_num; i++)
			{
				tex1[i].u=s_uv[i][0]/div_u+add_u;
				tex1[i].v=s_uv[i][1]/div_v+add_v;
				tex1[i].color = color|(tex1[i].color&0x00ffffff);
			}
		}
		else
		{
			for(i=0; i<v_num; i++)
			{
				tex1[i].u=s_uv[i][0];
				tex1[i].v=s_uv[i][1];
				tex1[i].color = color|(tex1[i].color&0x00ffffff);
			}
		}
	}
}
