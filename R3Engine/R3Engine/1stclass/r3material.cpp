#include "jerror.h"
#include "jmalloc.h"
#include "R3Material.h"
#include "SkyBox.h"
#include "R3d3dtex.h"
#include "core.h"
#include "commonutil.h"
#include <stdio.h>
#include <assert.h>

static _R3MATERIAL *R3material=NULL;
static DWORD stdwMaterialNum=0;
static char stTexturePath[256]=".\\Texture\\";

static void MatInit(_R3MATERIAL *mat)
{
	if(mat)
	{
		memset(mat,0,sizeof(_R3MATERIAL));//한머터리얼을 몽땅밀어버림
		for(int i=0; i<_MAX_LAYER_NUM; i++)
		{
			mat->m_Layer[i].m_iSurface	= 0;
			mat->m_Layer[i].m_dwAlphaType	= 0;
			mat->m_Layer[i].m_ARGB = 0xffffffff;
			mat->m_Layer[i].m_dwFlag		= 0;
		}
	}
	else
	{
		assert(!"mat_init error");
	}
}


_R3MATERIAL *GetMainMaterial()
{
	return R3material;
}

static BOOL stIsReLoad=FALSE;
void SetReLoadState(BOOL state)
{
	stIsReLoad=state;
}

static void LoadOneLayer(FILE *fp,_R3MATERIAL *material,int l_id)
{
	char name[256];
	char hole[256];

	material->m_Layer[l_id].m_ARGB=0xffffffff;

//	material->m_dwFlag=0;
//	material->m_sGradientAlpha=0;

    while(1)
    {
        fscanf(fp,"%s",&hole[0]);
        if (!strcmp(hole,"}"))
			break;

        if (!strcmp(hole,"map_name") && stIsReLoad == FALSE)	//맵이름.
		{
	        fscanf(fp,"%s",&hole[0]);
			//strcpy(material->m_tex_name,hole);
			strcpy(name,stTexturePath);
			strcat(name,hole);
			if( hole[0] != NULL && strcmp(hole,"NULL"))
			{
				if( IsAniTex(hole) == FALSE )	//애니메이션 텍스쳐가 아니다.
				{
					material->m_Layer[l_id].m_iSurface = R3GetPreTextureId(name);
				}
				else	//타일 애니메이션 텍스쳐다.
				{		//material->m_iSurface에 시작 아이디를 가지고 있는다.
					material->m_Layer[l_id].m_dwFlag|=_ANI_TILE_TEXTURE;			
					LONG get_tex_num;
					R3GetPreAniTextureId(stTexturePath,hole,&material->m_Layer[l_id].m_iSurface,&get_tex_num);
					material->m_Layer[l_id].m_iTileAniTexNum=(SHORT)get_tex_num;			//타일 애니 맵 갯수
				}
			}
			else
			{
				material->m_Layer[l_id].m_iSurface = 0;
			}
		}
		
        if (!strcmp(hole,"uv_scroll_u"))	
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sUVScrollU= FixedFloatToShort((float)atof(hole));
			material->m_Layer[l_id].m_dwFlag|=_UV_SCROLL_U;			
		}
        if (!strcmp(hole,"uv_scroll_v"))	
		{
			fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sUVScrollV=FixedFloatToShort((float)atof(hole));
			material->m_Layer[l_id].m_dwFlag|=_UV_SCROLL_V;			
		}
        if (!strcmp(hole,"uv_scale"))	
		{
			//un스케일의 시작과 끝을 미리 지정한다.
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sUVScaleStart=FixedFloatToShort((float)atof(hole));
			material->m_Layer[l_id].m_sUVScaleEnd =material->m_Layer[l_id].m_sUVScaleStart; 
			material->m_Layer[l_id].m_sUVScaleSpeed=256;
			material->m_Layer[l_id].m_dwFlag|=_UV_SCALE;
		}
        if (!strcmp(hole,"uv_scale_speed"))	
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sUVScaleSpeed=FixedFloatToShort((float)atof(hole));
		}
        if (!strcmp(hole,"uv_scale_end"))	
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sUVScaleEnd=FixedFloatToShort((float)atof(hole));
		}
        if (!strcmp(hole,"uv_rotate"))	
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sUVRotate=FixedFloatToShort((float)atof(hole));
			material->m_Layer[l_id].m_dwFlag|=_UV_ROTATE;
		}
        if (!strcmp(hole,"uv_lava"))	
		{
	        fscanf(fp,"%s",&hole[0]);
			if( material->m_Layer[l_id].m_sUVLavaWave==0)
				material->m_Layer[l_id].m_sUVLavaWave=FixedFloatToShort(1.0f);
			material->m_Layer[l_id].m_sUVLavaSpeed=FixedFloatToShort((float)atof(hole));
			material->m_Layer[l_id].m_dwFlag|=_UV_LAVA;
		}
        if (!strcmp(hole,"uv_lava_wave"))	
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sUVLavaWave=FixedFloatToShort((float)atof(hole));
		}
        if (!strcmp(hole,"uv_gradient_alpha_u"))	
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sGradientAlpha|=((WORD)((float)atof(hole)*25+100)<<8);
			material->m_Layer[l_id].m_dwFlag|=_UV_GRADIENT_ALPHA_U;
		}
        if (!strcmp(hole,"uv_gradient_alpha_v"))	
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sGradientAlpha|=((WORD)((float)atof(hole)*25+100));
			material->m_Layer[l_id].m_dwFlag|=_UV_GRADIENT_ALPHA_V;
		}
        if (!strcmp(hole,"uv_metal_floor"))	
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sUVMetal=FixedFloatToShort((float)atof(hole));
			material->m_Layer[l_id].m_dwFlag|=_UV_METAL_FLOOR;
		}
        if (!strcmp(hole,"uv_metal_wall"))	
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sUVMetal=FixedFloatToShort((float)atof(hole));
			material->m_Layer[l_id].m_dwFlag|=_UV_METAL_WALL;
		}
        if (!strcmp(hole,"ani_alpha_flicker"))
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sANIAlphaFlicker = FixedFloatToShort((float)atof(hole));
			material->m_Layer[l_id].m_dwFlag|=_ANI_ALPHA_FLICKER;
		}
        if (!strcmp(hole,"ani_alpha_flicker_start"))
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sANIAlphaFlickerAni |= ((WORD)(255*(float)atof(hole))<<8);
		}
        if (!strcmp(hole,"ani_alpha_flicker_end"))
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sANIAlphaFlickerAni |= (WORD)(255*(float)atof(hole));
		}
        if (!strcmp(hole,"ani_tex_speed"))
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_sANITexSpeed = FixedFloatToShort((float)atof(hole));
		}
        if (!strcmp(hole,"ani_tex_frame"))
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_dwFlag|=_ANI_TEXTURE;
			material->m_Layer[l_id].m_sANITexFrame = FixedFloatToShort((float)atof(hole));
			if( material->m_Layer[l_id].m_sANITexSpeed == 0)
				material->m_Layer[l_id].m_sANITexSpeed = FixedFloatToShort(1.0f);
		}
		
        if (!strcmp(hole,"uv_env"))
		{
	        fscanf(fp,"%s",&hole[0]);
//			material->m_sUVEnv=FixedFloatToShort((float)atof(hole));
			material->m_Layer[l_id].m_dwFlag|=_UV_ENV;
		}
		
        if (!strcmp(hole,"alpha_sort"))
		{
	        fscanf(fp,"%s",&hole[0]);
			char *boolgop = strupr(hole);
			if( !strcmp(boolgop,"TRUE"))
			{
				if( l_id == 0 )
			 		material->m_Layer[l_id].m_dwAlphaType |= _ALPHA_SORT_ON;
				else
					Warning("alpha_sort TRUE는 0번레이어에만..","");
			}
		}
        if (!strcmp(hole,"tex_clamp"))
		{
			material->m_Layer[l_id].m_dwFlag |= _MAT_TEX_CLAMP;
		}

        if (!strcmp(hole,"zwrite"))
		{
			//if( l_id == 0 )
			 	material->m_Layer[l_id].m_dwAlphaType |= _MAT_ZWRITE;
			//else
			//	Warning("zwrite는 0번레이어에만..","");
		}
        if (!strcmp(hole,"bump_env"))
		{
			material->m_Layer[l_id].m_dwFlag |= _MAT_ENV_BUMP;
		}

        if (!strcmp(hole,"type"))
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_dwAlphaType = atoi(hole);
		}
		
        if (!strcmp(hole,"alpha"))
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_Layer[l_id].m_ARGB &= 0x00ffffff;
			material->m_Layer[l_id].m_ARGB |= ((DWORD)(atoi(hole)))<<24;
		}
        if (!strcmp(hole,"color"))
		{
			DWORD r,g,b;
	        fscanf(fp,"%s",&hole[0]);
			r = (DWORD)(atoi(hole));
	        fscanf(fp,"%s",&hole[0]);
			g = (DWORD)(atoi(hole));
	        fscanf(fp,"%s",&hole[0]);
			b = (DWORD)(atoi(hole));
			material->m_Layer[l_id].m_ARGB &= 0xff000000;
			material->m_Layer[l_id].m_ARGB |= (r<<16)|(g<<8)|b;
		}
	}
	if( material->m_Layer[l_id].m_sANIAlphaFlickerAni == 0 )
		material->m_Layer[l_id].m_sANIAlphaFlickerAni=0x00ff;
}

static void LoadOneMaterial(char *name,_R3MATERIAL *material)
{
	FILE *fp;
	char hole[256];
	int l_id;

	bool sub = false;

	if( (fp = fopen(name,"rt")) == NULL )	
	{
		Warning(name,"<-이파일이 없다.");
		return;
	}

	if( stIsReLoad ==FALSE )
	{
		for(int q = 0;q < _MAX_LAYER_NUM;q++)
		{
			material->m_Layer[q].m_iSurface		= -1;
		}
		material->m_iDetailSurface=-1;
		material->m_dwFlag=0;
	}

	DWORD temp_texture_detail = GetR3State()->mSceneTextureDetail;
//	material->m_dwFlag |= _MAT_FLAG_NO_LOD_TEXTURE;

    while(1)
    {

		if( fscanf(fp,"%s",&hole[0])==EOF)	break;
        if (!strcmp(hole,"}"))
		{
			if(sub) sub = false;
			else	break;
		}
        if (!strcmp(hole,"light_map"))
		{
	        fscanf(fp,"%s",&hole[0]);
			char *boolgop = strupr(hole);
			if( !strcmp(boolgop,"TRUE"))
		 		material->m_dwFlag |= _MAT_FLAG_LIGHT_MAP;
		}
        if (!strcmp(hole,"layer_num"))
		{
	        fscanf(fp,"%s",&hole[0]);
		 	material->m_dwLayerNum = atoi(hole);
		}
        if (!strcmp(hole,"no_lod_texture"))
		{
	 		material->m_dwFlag |= _MAT_FLAG_NO_LOD_TEXTURE;
			GetR3State()->mSceneTextureDetail = 0;	//이럴경우 옵션을 무시하고 텍스쳐로딩을 최고로 한다.
		}
        if (!strcmp(hole,"detail_texture"))
		{
	        fscanf(fp,"%s",&hole[0]);
			strcpy(name,stTexturePath);
			strcat(name,hole);
	 		material->m_dwFlag |= _MAT_FLAG_NO_LOD_TEXTURE;
			GetR3State()->mSceneTextureDetail = 0;	//현재는 디테일경우 옵션을 무시하고 텍스쳐로딩을 최고로 한다.(나중에는 없앨것임.)
			if( hole[0] != NULL && strcmp(hole,"NULL"))
			{
				material->m_iDetailSurface = R3GetPreTextureId(name);
			}
			else
			{
				material->m_iDetailSurface = 0;
			}
			material->m_fDetailScale=1.0;
		}
        if (!strcmp(hole,"no_collision"))	//이 머터리얼을 쓰는 폴리곤들은 모든 충돌검사를 안한다.
		{
			material->m_Layer[0].m_dwFlag |= _MAT_NO_COLLISON;	//0번 레이어 플래그를 쓴다.
		}
        if (!strcmp(hole,"water"))	//이 머터리얼을 쓰는 폴리곤들은 모든 충돌검사를 안한다.
		{
			material->m_Layer[0].m_dwFlag |= _MAT_NO_COLLISON;	
			material->m_Layer[0].m_dwFlag |= _MAT_WATER;		//물효과닷.
		}
        if (!strcmp(hole,"detail_texture_scale"))
		{
	        fscanf(fp,"%s",&hole[0]);
			material->m_fDetailScale=(float)atof(hole);
		}
        if (!strcmp(hole,"layer"))
		{
	        fscanf(fp,"%s",&hole[0]);
			l_id = atoi(hole);
//			if(l_id != 1) 
			LoadOneLayer(fp,material,l_id);
			sub = true;
		}		
	}
	GetR3State()->mSceneTextureDetail = temp_texture_detail;

	fclose(fp);
}

BOOL IsLoadedMaterial()	//현재 머터리얼이 로드되었는가?
{
	if( R3material )
		return TRUE;
	return FALSE;
}


//독립된 머터리얼 로딩..
static _R3MATERIAL* LoadIndependenceMaterial(char *matpath)	//스카이박스나 엔티티는 아래 addmaterial으로 추가해서 쓸것...
{
	FILE *fp;
	char hole[256],name[256];
	int m_num,i,m_id;

	_R3MATERIAL *mat;

	strcpy(stTexturePath,matpath);
	strcpy(name,matpath);
	strcat(name,"MainMaterial.mst");
	if( (fp = fopen(name,"rt")) == NULL )	
	{
		Warning(name,"<-이파일이 없다.");
		return NULL;
	}

	_R3ENGINE_STATE *r3_state=GetR3State();
    while(1)
    {
		if( fscanf(fp,"%s",&hole[0])==EOF)		
			break;
//		if (!strcmp(hole,"}"))					break;
		strupr(hole);
        if (!strcmp(hole,"*MATERIAL_NUM"))
		{
            fscanf(fp,"%s",&hole[0]);
			m_num=atoi(hole);

			if( m_num >= _MAX_MATERIALS	)	
				Error("최대 머터리얼 초과","");

			mat = (_R3MATERIAL *)Dmalloc(m_num*sizeof(_R3MATERIAL));
			memset(mat,0,m_num*sizeof(_R3MATERIAL));
            fscanf(fp,"%s",&hole[0]);

			for(i=0; i<m_num; i++)
				MatInit(&mat[i]);	//머터리얼 초기화.
			for(i=0; i<m_num; i++)
			{
				strcpy(name,matpath);
	            fscanf(fp,"%s",&hole[0]);
				strcat(name,hole);
				strcpy(mat[i].m_name,hole);// mtrl name set
				strcat(name,".mst");
	            fscanf(fp,"%s",&hole[0]);
				m_id=atoi(hole);
				LoadOneMaterial(name,&mat[m_id]);
			}
		}

        if (!strcmp(hole,"*FOG_START"))
		{
			r3_state->mIsFog=TRUE;
            fscanf(fp,"%s",&hole[0]);
			r3_state->mMainFogStart=(float)atof(hole);
			r3_state->mFogStart=(float)atof(hole);
		}
        if (!strcmp(hole,"*FOG_END"))
		{
			r3_state->mIsFog=TRUE;
            fscanf(fp,"%s",&hole[0]);
			r3_state->mMainFogEnd=(float)atof(hole);
			r3_state->mFogEnd=(float)atof(hole);
			r3_state->mFar=r3_state->mFogEnd;
		}
        if (!strcmp(hole,"*FOG_COLOR"))
		{
			r3_state->mIsFog=TRUE;
			if( r3_state->mFogEnd == 0 )
			{
				r3_state->mFogEnd=5000;
				r3_state->mMainFogEnd=5000;
			}
			if( r3_state->mFogStart == 0 )
			{
				r3_state->mFogStart=5;
				r3_state->mMainFogStart=5;
			}
            fscanf(fp,"%s",&hole[0]);
			DWORD r=(DWORD)(atoi(hole));
            fscanf(fp,"%s",&hole[0]);
			DWORD g=(DWORD)(atoi(hole));
            fscanf(fp,"%s",&hole[0]);
			DWORD b=(DWORD)(atoi(hole));
			r3_state->mFogColor = 0xff000000|(r<<16)|(g<<8)|(b<<0);
			r3_state->mMainFogColor = 0xff000000|(r<<16)|(g<<8)|(b<<0);
		}
        if (!strcmp(hole,"*NO_FOG_SKY"))
		{
			r3_state->mIsNoFogSky=TRUE;
		}
		
        if (!strcmp(hole,"*FOG_RANGE"))
		{
            fscanf(fp,"%s",&hole[0]);
	        if (!strcmp(hole,"TRUE"))
				r3_state->mFogRangeEnable =TRUE;
		}

		//-----렌즈플레어에 관한거지...
        if (!strcmp(hole,"*LENS_FLARE_SCALE"))
		{
			for(i=0; i<16; i++)
			{
	            fscanf(fp,"%s",hole);
				r3_state->mLensFlareScale[i]=(float)atof(hole);
			}
		}
        if (!strcmp(hole,"*LENS_FLARE"))
		{
            fscanf(fp,"%s",hole);
			strcpy(name,matpath);
			strcat(name,hole);
			r3_state->mLensFlareTex = R3GetPreTextureId(name);
			strcpy(r3_state->mLensTexName,name);
		}
        if (!strcmp(hole,"*LENS_FLARE_POS"))
		{
            fscanf(fp,"%s",hole);
			r3_state->mLensFlarePos[0] = (float)atof(hole);
            fscanf(fp,"%s",hole);
			r3_state->mLensFlarePos[2] = (float)atof(hole);
            fscanf(fp,"%s",hole);
			r3_state->mLensFlarePos[1] = (float)atof(hole);
		}
        if (!strcmp(hole,"*ENVIRONMENT"))//
		{	//---Environment 렌더 엔티티이름 관련.  // 비,눈,기타 폭풍등...
            fscanf(fp,"%s",hole);
			strcpy(r3_state->mEnvEntityName,hole);// 환경은 엔티티로 접근된다.
		}
        if (!strcmp(hole,"*ENVIRONMENT_ID"))//
		{
            fscanf(fp,"%s",hole);
			r3_state->mEnvID = (DWORD)atoi(hole);
		}
	}

	mat->m_iMatNum = (WORD)m_num;//해당 루트에 머터리얼갯수를..

	fclose(fp);
	return mat;
}


void LoadR3X(char *name)
{
	int i;
	float version;
	_EXT_MAT ext_mat;
	FILE *fp;
	_R3ENGINE_STATE *state = GetR3State();
	memset(&ext_mat,0,sizeof(_EXT_MAT));

	//--------해당 머터리얼 전체 헤더....
	if( (fp = fopen(name,"rb")) == NULL) 
		return;
	fread(&version,sizeof(DWORD),1,fp);
	if( version != _R3X_VERSION )
	{
		Warning(name,"<-이 파일이 버전이 예전 버젼입니다.");
		fclose(fp);
		return;
	}
	fread(&ext_mat,sizeof(_EXT_MAT),1,fp);


	if( ext_mat.mFlag & EXT_MAT_EXIST_1ST_FOG )
	{
		state->mIsFog=TRUE;
		state->mFogColor=ext_mat.mFogColor;
		state->mFogStart=ext_mat.mFogStart;
		state->mFogEnd=ext_mat.mFogEnd ;
		state->mFar=state->mFogEnd;
		state->mMainFogStart=ext_mat.mFogStart;
		state->mMainFogEnd=ext_mat.mFogEnd;
		state->mMainFogColor=ext_mat.mFogColor;
	}
	/*
	if( ext_mat.mFlag & EXT_MAT_EXIST_2ND_FOG )
	{
		state->mIsFog2 = TRUE;
		state->mFogColor2=ext_mat.mFogColor2;
		state->mFogStart2=ext_mat.mFogStart2 ;
		state->mFogEnd2=ext_mat.mFogEnd2 ;
		Vector3fCopy(ext_mat.BBMin2,state->FogBBMin2);
		Vector3fCopy(ext_mat.BBMax2,state->FogBBMax2);
	}*/
	if( ext_mat.mFlag & EXT_MAT_EXIST_LENS_FLARE )	//렌즈 플레어가 있따..
	{
		//state->mLensFlareTex;
		strcpy(state->mLensTexName,ext_mat.mLensTexName);	//풀 패스다...
		
		state->mLensFlareTex = R3GetPreTextureId(state->mLensTexName);

		Vector3fCopy(ext_mat.mLensFlarePos,state->mLensFlarePos);
		for(i=0; i<16; i++)
			state->mLensFlareScale[i]=ext_mat.mLensFlareScale[i];
	}
	if( ext_mat.mFlag & EXT_MAT_EXIST_ENV )
	{
		state->mEnvID = ext_mat.mEnvID;
		strcpy(state->mEnvEntityName,ext_mat.mEnvEntityName);
	}
	if( ext_mat.mFlag & EXT_MAT_NO_FOG_SKY )
		state->mIsNoFogSky = TRUE;
	if( ext_mat.mFlag & EXT_MAT_FOG_RANGE )
		state->mFogRangeEnable = TRUE;

	fclose(fp);
}

//머터리얼 아이디 잘못된거 수정.
void AdjustIndependenceR3M(_R3MATERIAL *mat,LONG old_base_tex_cnt,LONG new_base_tex_cnt)
{	//안은 똑같다..
	int i,j;
	
	for(i=0; i<mat->m_iMatNum; i++)
	{
		if( mat[i].m_iDetailSurface != -1 )
			mat[i].m_iDetailSurface = mat[i].m_iDetailSurface - old_base_tex_cnt + new_base_tex_cnt;
		for(j=0; j<(int)mat[i].m_dwLayerNum; j++)	//좀 맘에 안들지만 용량때문에 이방법을...
		{
			mat[i].m_Layer[j].m_iSurface = mat[i].m_Layer[j].m_iSurface - old_base_tex_cnt + new_base_tex_cnt;	//실제 베이스가 될 
		}
	}
}

_R3MATERIAL* LoadIndependenceR3M(char *name)	//안은 똑같다..
{
	FILE *fp;
	int i,j;
	DWORD m_num;
	float version;
	
	if( (fp = fopen(name,"rb")) == NULL) 
	{
		Warning(name,"<-이 파일이 없습니다.");
		return NULL;
	}

	//------파일 저장.
//	tex_id=(BYTE *)Dmalloc(sizeof(BYTE)*texcnt);
//	memset(tex_id,0,sizeof(BYTE)*texcnt);

	//머터리얼 부분 ..
	fread(&version,sizeof(float),1,fp);

	if( version != _R3M_VERSION )
	{
		Warning(name,"<-이 파일이 없다...");
		return NULL;
	}

	fread(&m_num,sizeof(DWORD),1,fp);

	_R3MATERIAL *mat = (_R3MATERIAL *)Dmalloc(m_num*sizeof(_R3MATERIAL));
	memset(mat,0,m_num*sizeof(_R3MATERIAL));
	
	int base_tex_cnt = GetNowR3D3DTexCnt()-1;
	mat->m_iMatNum = m_num;
	for(i=0; i<mat->m_iMatNum; i++)
	{
		fread(&mat[i].m_dwLayerNum,sizeof(DWORD),1,fp);		//레이어갯수.
		fread(&mat[i].m_dwFlag,sizeof(BOOL),1,fp);			//플래그.
		fread(&mat[i].m_iDetailSurface,sizeof(LONG),1,fp);		//라이트맵 유무.
		fread(&mat[i].m_fDetailScale,sizeof(float),1,fp);	//디테일 스케일.
		fread(mat[i].m_name,sizeof(char)*_TEX_NAME_SIZE,1,fp);	//머터리얼 이름 

		if( mat[i].m_iDetailSurface != -1 )
			mat[i].m_iDetailSurface += base_tex_cnt;
		for(j=0; j<(int)mat[i].m_dwLayerNum; j++)	//좀 맘에 안들지만 용량때문에 이방법을...
		{
			fread(&mat[i].m_Layer[j],sizeof(_ONE_LAYER),1,fp);	//머터리얼 이름 
			mat[i].m_Layer[j].m_iSurface += base_tex_cnt;	//실제 베이스가 될 
		}
//		if( mat[i].m_dwFlag )	//현재는 디테일텍스쳐만 되있다.
		if( mat[i].m_iDetailSurface != -1 )	//현재는 디테일텍스쳐만 되있다.
			mat[i].m_dwFlag |= _MAT_FLAG_NO_LOD_TEXTURE;
	}

	fclose(fp);
	return mat;
}

//텍스트로 된 메인 머터리얼로딩.
_R3MATERIAL* LoadMainMaterial(char *matpath)	//스카이박스나 엔티티는 아래 addmaterial으로 추가해서 쓸것...
{
	_R3ENGINE_STATE *state=GetR3State();
	state->mFogColor=0xff000000;
	state->mIsFog=FALSE;			//포그일단 끈다.
	state->mIsNoFogSky=FALSE;		//스카이박스 포그가 없는가?
	state->mFogRangeEnable=FALSE;	//포그 범위킬까?
	state->mEnvID=0;				//env 초기화.
	state->mLensFlareTex=-1;	//렌즈플레어 초기화.

	if( R3material )
		ReleaseMainMaterial();
	R3material = LoadIndependenceMaterial(matpath);	//안은 똑같다..
	stdwMaterialNum = R3material->m_iMatNum;	//머터리얼 개수 올려주자..
	return R3material;	//루트를 
}

_R3MATERIAL* LoadSubR3M(char *mat_name)	//메인머터리얼 이외는 이거를.
{
	_R3MATERIAL*ret = LoadIndependenceR3M(mat_name);	//안은 똑같다..
	return ret;	
}

//R3M은 머터리얼 바이너리 파일이다. 
_R3MATERIAL* LoadMainR3M(char *name)	//스카이박스나 엔티티는 아래 addmaterial으로 추가해서 쓸것...
{
	_R3ENGINE_STATE *state=GetR3State();
	state->mFogColor=0xff000000;
	state->mIsFog=FALSE;			//포그일단 끈다.
	state->mIsNoFogSky=FALSE;		//스카이박스 포그가 없는가?
	state->mFogRangeEnable=FALSE;	//포그 범위킬까?
	state->mEnvID=0;				//env 초기화.
	state->mLensFlareTex=-1;	//렌즈플레어 초기화.

	char buf[256];

	strcpy(buf,name);
	StripEXT(buf);
	strcat(buf,".r3x");

	LoadR3X( buf );		//확장 머터리얼이 우선이다.

	if( R3material )
		ReleaseMainMaterial();
	R3material = LoadIndependenceR3M(name);	//안은 똑같다..
	stdwMaterialNum = R3material->m_iMatNum;	//머터리얼 개수 올려주자..
	return R3material;	//루트를 
}

_R3MATERIAL* LoadSubMaterial(char *mat_path)	//메인머터리얼 이외는 이거를.
{
	_R3MATERIAL*ret = LoadIndependenceMaterial(mat_path);	//안은 똑같다..
	return ret;	
}

//단순 수치만을 로딩할때
void ReLoadMaterial(char *matpath,_R3MATERIAL *now_material)
{
	FILE *fp;
	char hole[256],name[256];
	int m_num,i,m_id;

	strcpy(stTexturePath,matpath);
	strcpy(name,matpath);
	strcat(name,"MainMaterial.mst");
	if( (fp = fopen(name,"rt")) == NULL )
		return;

    while(1)
    {
		if( fscanf(fp,"%s",&hole[0])==EOF)		break;

        if (!strcmp(hole,"*MATERIAL_NUM"))
		{
            fscanf(fp,"%s",&hole[0]);
			m_num=atoi(hole);
            fscanf(fp,"%s",&hole[0]);

			if( m_num >= _MAX_MATERIALS	)	
				Error("최대 머터리얼 초과","");

			for(i=0; i<m_num; i++)
			{
				strcpy(name,stTexturePath);
	            fscanf(fp,"%s",&hole[0]);
				strcat(name,hole);
				strcpy(now_material[i].m_name,hole);// mtrl name set
				strcat(name,".mst");
	            fscanf(fp,"%s",&hole[0]);
				m_id=atoi(hole);
				LoadOneMaterial(name,&now_material[m_id]);
			}
		}
	}

	fclose(fp);
}

void ReleaseSubMaterial(_R3MATERIAL *mat)
{
	if( mat )
	{
		Dfree(mat);
	}
}

void ReleaseMainMaterial()
{
	if( R3material )
	{
		Dfree(R3material);
	}
	R3material=NULL;
	stdwMaterialNum=0;
}

DWORD GetMainMaterialNum()
{
	return stdwMaterialNum;
}

void SetNoLodTextere()	//lod텍스쳐가 아닌경우.
{
	DWORD i,j;

	for(i=0; i<stdwMaterialNum; i++)
	{
		if( R3material[i].m_dwFlag & _MAT_FLAG_NO_LOD_TEXTURE )
		{
			SetR3D3DTexture( R3material[i].m_iDetailSurface,_R3D3DTEXTURE_FLAG_NO_LOD );
			for(j=0; j<R3material[i].m_dwLayerNum; j++)
			{
				SetR3D3DTexture( R3material[i].m_Layer[j].m_iSurface,_R3D3DTEXTURE_FLAG_NO_LOD );				
			}
		}
	}
}

