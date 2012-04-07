#include<stdio.h>
#include"world.h"
#include"patch.h"
#include"rad.h"
#include"bmplib.h"
#include"jerror.h"
#include"Lightmap.h"
#include"R3BlockMat.h"
#include"jmalloc.h"
#include"R3BspUtil.h"
#include"RevisitedRadix.h"

//다이렉트 X
#include"ddraw.h"



#define _EXPORT_BMP_LIGHTMAP		//라이맵을 bmp로 출력한다.

static int AmbientRed,AmbientGreen,AmbientBlue;
static int L_edge[_MAX_TEXTURE_SIDE];	// 왼쪽 에지테이블
static int R_edge[_MAX_TEXTURE_SIDE];	// 오른쪽 에지테이블

static _LIGHT_MAP *GetLightmapFromPatch(_PATCH *patch,_LIGHT_MAP *lgt_map)
{
	int i,j;

/*
	for(i=0; i<patch->yl; i++)
	{
		for(j=0; j<patch->xl; j++)
		{
//			patch->winding[i*patch->xl + j].energy.a = min(1.0f,patch->winding[i*patch->xl + j].energy.a);
			patch->winding[i*patch->xl + j].energy[0] = min(1.0f,patch->winding[i*patch->xl + j].energy[0]);
			patch->winding[i*patch->xl + j].energy[1] = min(1.0f,patch->winding[i*patch->xl + j].energy[1]);
			patch->winding[i*patch->xl + j].energy[2] = min(1.0f,patch->winding[i*patch->xl + j].energy[2]);
		}
	}
*/
	for(i=0; i<patch->yl; i++)
	{
		for(j=0; j<patch->xl; j++)
		{
			lgt_map->bitmap[i*patch->xl+j].R = (BYTE)min(255,(patch->winding[i*patch->xl + j].energy[0])+AmbientRed);
			lgt_map->bitmap[i*patch->xl+j].G = (BYTE)min(255,(patch->winding[i*patch->xl + j].energy[1])+AmbientGreen);
			lgt_map->bitmap[i*patch->xl+j].B = (BYTE)min(255,(patch->winding[i*patch->xl + j].energy[2])+AmbientBlue);
		}
	}
	return lgt_map;
}

typedef struct{
	CGenLightMap *GenLightMap;
	int Lightmap_cnt;
	int Lightmap_16bitsize;
	int Lightmap_24bitsize;
}_LIGHTMAP_INFO;

_LIGHTMAP_INFO L_info;

_LIGHTMAP_INFO *GetLightmapInfo(void)
{
	return &L_info;
}

int GetLightmapCnt(void)
{
	return L_info.Lightmap_cnt;
}
CGenLightMap *GetLightmap(void)
{
	return L_info.GenLightMap;
}

static RadixSorter Radix;

void GetLightMapSize(float size,int maxxl,int maxyl,int *xsize,int *ysize)	//사이즈에 적당한 스퀘어를 리턴한다.
{
	*xsize = _MAX_TEXTURE_SIDE;
	*ysize = _MAX_TEXTURE_SIDE;

	int cnt=0;
	while( 1 )
	{
		if( *xsize==1 && *ysize==1 )
			break;
		if(cnt&1)
		{
			*ysize/=2;
			if(*ysize < maxyl)
			{
				*ysize*=2;
			}
			if( (*xsize)*(*ysize) < size )	//만만한 사이즈를 찾았다.
			{
				*ysize*=2;
				return;
			}
		}
		else
		{
			*xsize/=2;
			if(*xsize < maxxl)
			{
				*xsize*=2;
			}
			if( (*xsize)*(*ysize) < size )	//만만한 사이즈를 찾았다.
			{
				*xsize*=2;
				return;
			}
		}
		cnt++;

		if( cnt>30 )	//충분히 찾았다.
			return;
	}
}

void CGenLightMap::AdjustLightMap(_POLYFACE *face,DWORD *index,DWORD *face_index,int cnt)
{
	int i,j;

	if(mLgtMap.yl==1)
		return;
	for(i=mLgtMap.yl/2; i<mLgtMap.yl; i++)
	{
		if(R_edge[i]-L_edge[i] != mLgtMap.xl)
			break;
	}
	if( i == mLgtMap.yl )	//오옷 절반이나 남았다.챙기자.
	{
		mLgtMap.yl/=2;
		for(j=0; j<cnt; j++)
		{
			for(i=0; i<face[face_index[index[j]]].vertex_num; i++)
				face[face_index[index[j]]].lv[i]*=2.0f;
		}
		//또 줄일수 있을지 모른다. 
		AdjustLightMap(face,index,face_index,cnt);
	}
}
//작은 소스 라이트 맵을 통짜 dest라이트맵에 저장하고 페이스의 lu,lv값을 조정한다.
BOOL CGenLightMap::FillLightMap(_LIGHT_MAP *src,_POLYFACE *face,float *lu,float *lv)
{
	int i,j,k;
	int ddd=0;
	for(i=0; i<mLgtMap.yl; i++)
	{
		if( R_edge[i]-L_edge[i] >= src->xl+2 )
		{
			int start_ledge = L_edge[i];
			for(j=0; j<src->yl+2; j++)
			{
				if( i+j >= mLgtMap.yl )
				{
					//Error("오오 라이트맵 버퍼를 채울수가 없다.","");
					return FALSE;
				}
				if( R_edge[i+j] - L_edge[i+j] < src->xl+2 || L_edge[i+j] != start_ledge)
					break;
			}
			if(j != src->yl+2)	//이곳에 못 넣는다.
				continue;
			//-----자 넣어보자.
			int xpos = L_edge[i];
			int ypos = i+1;

			//---테두리를 채운다.
			mLgtMap.bitmap[(ypos-1)*mLgtMap.xl+(xpos-1)]= src->bitmap[0*src->xl+0];//왼쪽위
			mLgtMap.bitmap[(ypos-1)*mLgtMap.xl+(xpos+src->xl)]= src->bitmap[0*src->xl+src->xl-1];//오른쪽위
			mLgtMap.bitmap[(ypos+src->yl)*mLgtMap.xl+(xpos-1)]= src->bitmap[(src->yl-1)*src->xl+0];//왼쪽아래
			mLgtMap.bitmap[(ypos+src->yl)*mLgtMap.xl+(xpos+src->xl)]= src->bitmap[(src->yl-1)*src->xl+src->xl-1];//오른 아래

			for(k=0; k<src->xl; k++)	//위아래 
			{
				mLgtMap.bitmap[(ypos-1)*mLgtMap.xl+(k+xpos)]= src->bitmap[0*src->xl+k];
				mLgtMap.bitmap[(src->yl+ypos)*mLgtMap.xl+(k+xpos)]= src->bitmap[(src->yl-1)*src->xl+k];
			}
			for(j=0; j<src->yl; j++)	//좌우
			{
				mLgtMap.bitmap[(j+ypos)*mLgtMap.xl+(xpos-1)]= src->bitmap[j*src->xl+0];
				mLgtMap.bitmap[(j+ypos)*mLgtMap.xl+(xpos+src->xl)]= src->bitmap[j*src->xl+src->xl-1];
			}
			
			//---버퍼에 채운다.
			for(j=0; j<src->yl; j++)
				for(k=0; k<src->xl; k++)
				{
					mLgtMap.bitmap[(j+ypos)*mLgtMap.xl+(k+xpos)]= src->bitmap[j*src->xl+k];
				}

			//lu,lv좌표를 조정한다.
			for(j=0; j<face->vertex_num; j++)
			{
				if(face->lu[j] <= 0.01f)
					face->lu[j]=0.01f;
				if(face->lv[j] <= 0.01f)
					face->lv[j]=0.01f;
				if(face->lu[j] >= 0.99f)
					face->lu[j]=0.99f;
				if(face->lv[j] >= 0.99f)
					face->lv[j]=0.99f;
				lu[j]=(face->lu[j]*((float)src->xl)+(float)xpos)/(float)mLgtMap.xl;
				lv[j]=(face->lv[j]*((float)src->yl)+(float)ypos)/(float)mLgtMap.yl;
			}
			for(j=0; j<src->yl+2; j++)	//L_edge에 채워준다.
				L_edge[i+j] = xpos+src->xl+2;
			return TRUE;	//성공이다.
		}
	}
	return FALSE;
}

DWORD GetAverageColor(_PATCH *patch,_LIGHT_MAP *lgtmap)
{
	int i,j,cnt=0;
	DWORD r=0,g=0,b=0;

	for(i=0 ;i<patch->yl; i++)
	{
		for(j=0 ;j<patch->xl; j++)
		{
			if(patch->winding[i*patch->xl+j].is_out_face)
				continue;
			r += lgtmap->bitmap[i*patch->xl+j].R;
			g += lgtmap->bitmap[i*patch->xl+j].G;
			b += lgtmap->bitmap[i*patch->xl+j].B;
			cnt++;
		}
	}
	if(cnt!=0)
	{
		r/=cnt;
		g/=cnt;
		b/=cnt;
	}

	return ( (r<<16)|(g<<8)|b );
}

#define _LIGHTMAP_EXT_SIZE 1.00f

CGenLightMap *GenLightMap=NULL;

CGenLightMap::CGenLightMap()
{
	mNext=NULL;
	memset(mLEdge,0,_MAX_TEXTURE_SIDE*sizeof(int));	// 왼쪽 에지테이블
	memset(mREdge,0,_MAX_TEXTURE_SIDE*sizeof(int));	// 왼쪽 에지테이블
	mLgtMap.xl=0;
	mLgtMap.yl=0;
	mLgtMap.bitmap=NULL;
	mCnt=0;
	mBufSize=0;
	mIndex=0;
}
CGenLightMap::~CGenLightMap()
{
}

int GetFaceIndex(_OBJECT *obj,_POLYFACE *p_face)
{
	int index =( p_face  - obj->p_face );/// sizeof(_POLYFACE *);
	return index;
}
int GetLeanearFaceIndex(_MESH *mesh,_POLYFACE *p_face)
{
	int index=0,i;
	for(i=0; i<mesh->objectnum;i++)
	{
		_OBJECT *obj = &mesh->object[i];

		if( obj->p_face <= p_face && obj->p_face+obj->facenum > p_face )
		{
			index +=( p_face  - obj->p_face );/// sizeof(_POLYFACE *);
			break;
		}
		if( mesh->object[i].type & _SHADOW_RECEIVE_OFF )	//라이트맵이 없으면 버텍스라이트
			continue;
		if( mesh->object[i].type & _OBJECT_TYPE_COMPOSITE )	//라이트맵이 없으면 버텍스라이트
			continue;
		index+=obj->facenum;
	}
	return index;
}

//콤포짓 머터리얼은 미리계산된 라이트맵이다.
void MakeLightmapsFromPatchs(_MESH *mesh,_MATERIAL *mat,R3BlockBsp *blockmat,_MATERIAL *com_mat)
{
	int i,j,k,lgt_cnt;
	int size=0;
	int cnt=0;
	_OptObject *now_obj;
	_OptObject *opt_obj = blockmat->mOptObject;

	_LFACE **lf = GetLFace();
	if( lf==NULL )
		Error("먼저 팻치,와인딩을 설정해라.","");


	//-------------- Ambient를  설정...
	_INIFILE *Ini=GetIniFile();
	AmbientRed=(int)((Ini->Ambient[0]/_MAX_INTENSITY)*255);
	AmbientGreen=(int)((Ini->Ambient[1]/_MAX_INTENSITY)*255);
	AmbientBlue=(int)((Ini->Ambient[2]/_MAX_INTENSITY)*255);

	PutR3BspInfoText("Lightmap Generating...");

	size=0;
	for(k=0; k<mesh->objectnum; k++)
		size +=mesh->object[k].facenum;

	unsigned int *face_index=(unsigned int *)Dmalloc(sizeof(int)*size);
	unsigned int *face_lsize=(unsigned int *)Dmalloc(sizeof(int)*size);

	size=0;
	for(k=0; k<mesh->objectnum; k++)
		for(i=0; i<mesh->object[k].facenum; i++)
			size +=mesh->object[k].p_face[i].vertex_num;

	float *face_lu =(float *)Dmalloc(sizeof(float)*size);
	float *face_lv =(float *)Dmalloc(sizeof(float)*size);

	//---잘게 쪼개진 라이트맵은 미리 계산해놓고....
/*
	size=0;
	for(k=0; k<mesh->objectnum; k++)
	{
		for(i=0; i<mesh->object[k].facenum; i++)
		{
			size+=lf[k][i].patch->xl*lf[k][i].patch->yl;
		}
	}
*/
	size=0;
	for(k=0; k<mesh->objectnum; k++)
	{
		for(i=0; i<mesh->object[k].facenum; i++)
		{
			size+=lf[k][i].patch->xl*lf[k][i].patch->yl;
		}
	}

	_LIGHT_MAP *divide_lgtmap = (_LIGHT_MAP *)Dmalloc(size*sizeof(_LIGHT_MAP));
	BYTE *divide_buf = (BYTE *)Dmalloc(size*sizeof(_LIGHTMAP_COLOR));


	int base_lightmap_cnt=0;
	if( com_mat )
		base_lightmap_cnt = com_mat->all_num;
		
	size=0;
	cnt=0;
	for(k=0; k<mesh->objectnum; k++)
	{
		if( mesh->object[k].type & _OBJECT_TYPE_COMPOSITE )	//라이트맵이 없으면 버텍스라이트
		{
			continue;
		}
		if( mesh->object[k].type & _SHADOW_RECEIVE_OFF )	//라이트맵이 없으면 버텍스라이트
		{
			mesh->object[k].vcolor=(DWORD *)Dmalloc(mesh->object[k].vertexnum*sizeof(DWORD));			//버텍스 컬러
			Vector4f *vn_color=(Vector4f *)Dmalloc(mesh->object[k].vertexnum*sizeof(Vector4f));
			memset(vn_color,0,mesh->object[k].vertexnum*sizeof(Vector4f));
			//for(i=0; i<mesh->object[k].vertexnum; i++)
			for(i=0; i<mesh->object[k].facenum; i++)
			{
				for(j=0; j<mesh->object[k].p_face[i].vertex_num; j++)
				{
					int vc_index = mesh->object[k].p_face[i].vertex_id[j];
					float temp_mm = (float)min(255,lf[k][i].patch->winding[j].energy[0]+AmbientRed);
					vn_color[vc_index][0]+= temp_mm;
					temp_mm = (float)min(255,lf[k][i].patch->winding[j].energy[1]+AmbientRed);
					vn_color[vc_index][1]+= temp_mm;
					temp_mm = (float)min(255,lf[k][i].patch->winding[j].energy[2]+AmbientRed);
					vn_color[vc_index][2]+= temp_mm;
					vn_color[vc_index][3]+= 1;
				}
			}
			for(j=0; j<mesh->object[k].vertexnum; j++)
			{
				if( vn_color[j][3] )
				{
					DWORD temp_r = (DWORD)( vn_color[j][0]/vn_color[j][3] );
					DWORD temp_g = (DWORD)( vn_color[j][1]/vn_color[j][3] );
					DWORD temp_b = (DWORD)( vn_color[j][2]/vn_color[j][3] );
					//DWORD temp_r = (DWORD)( min(vn_color[j][0],255) );
					//DWORD temp_g = (DWORD)( min(vn_color[j][1],255) );
					//DWORD temp_b = (DWORD)( min(vn_color[j][2],255) );
					mesh->object[k].vcolor[j] =0xff000000|(temp_r<<16)|(temp_g<<8)|(temp_b);
				}
				else
				{
//					DWORD temp_c = (DWORD)(255.0f/_MAX_INTENSITY);
					DWORD temp_c = (DWORD)(255);
					mesh->object[k].vcolor[j]=0xff000000|(temp_c<<16)|(temp_c<<8)|(temp_c);
				}
			}
			Dfree(vn_color);
			continue;
		}
		for(i=0; i<mesh->object[k].facenum; i++)
		{
			divide_lgtmap[cnt].xl = lf[k][i].patch->xl;
			divide_lgtmap[cnt].yl = lf[k][i].patch->yl;
			divide_lgtmap[cnt].bitmap = (_LIGHTMAP_COLOR*)&divide_buf[size];
			mesh->object[k].p_face[i].lgt_ptr = GetLightmapFromPatch(lf[k][i].patch, &divide_lgtmap[cnt]);
			size+=lf[k][i].patch->xl*lf[k][i].patch->yl*sizeof(_LIGHTMAP_COLOR);
			cnt++;
		}
	}
/*
	//--- 오파맵의 컬러값을 구한다.
	if( mat )
	{
		cnt=0;
		for(k=0; k<mesh->objectnum; k++)
		{
			for(i=0; i<mesh->object[k].facenum; i++)
			{
				DWORD color=0xffffffff;
				if( mesh->object[k].p_face[i].mtlid == -1)
					continue;
				if(mat[mesh->object[k].p_face[i].mtlid].oppa)
				{
					color = GetAverageColor(lf[k][i].patch,&divide_lgtmap[cnt]);
					color |= ((DWORD)(mat[mesh->object[k].p_face[i].mtlid].transparency*255)<<24);
				}
				if(mat[mesh->object[k].p_face[i].mtlid].transparency!=1.0)
				{
					color = 0x00ffffff;	//multiply이가 1까지 지원되니까..
					color |= ((DWORD)(mat[mesh->object[k].p_face[i].mtlid].transparency*255)<<24);
				}
				mesh->object[k].p_face[i].argb=color;
				cnt++;
			}
		}
	}
*/
	lgt_cnt=0;
	CGenLightMap *ngen_light_map=NULL;	//라이트맵 젠 malloc 템프 포인터.
	CGenLightMap *now_light_map=NULL;

/*
	int debug=0;
	_OptObject *debug_p;
	for(now_obj = opt_obj; now_obj != NULL; now_obj= now_obj->next)
	{
		debug++;
		if( debug == 75 )
			debug_p =now_obj;
	}
	debug=0;
*/

	//최적화된 오브젝트로 라이트맵을 접근한다.
	for(now_obj = opt_obj; now_obj != NULL; now_obj= now_obj->next)
	{
//		debug++;

		size=0;
		cnt=0;
		int maxxl=0;
		int maxyl=0;
		int xsize;
		int ysize;
		

		if( now_obj->type& _COMPOSITE_LIGHT_MAP )
		{
			//now_obj->lgtid;	//지꺼는 미리 넣어줬따..
			continue;
		}

		if( now_obj->type & _NO_LIGHT_MAP )	//라이트맵이 없으면 버텍스라이트
		{
			now_obj->lgtid=-1;
			continue;
		}


		//이 머터리얼 블럭의 라이트맵 사이즈를 알아본다.
		for(j=0; j<(int)now_obj->f_cnt; j++)
		{
			int obj_id = blockmat->mPolyWhatObj[j+now_obj->start_id];
			int f_id = GetFaceIndex(&mesh->object[obj_id],blockmat->mPolyFace[j+now_obj->start_id]);

			//mesh->object[obj_id].p_face[f_id].l_mtlid = l_cnt;
			maxxl = max(maxxl,lf[obj_id][f_id].patch->xl+2);
			maxyl = max(maxyl,lf[obj_id][f_id].patch->yl+2);
			size += (lf[obj_id][f_id].patch->xl+2)*(lf[obj_id][f_id].patch->yl+2);
			face_lsize[cnt] = lf[obj_id][f_id].patch->yl;	//y길이가 긴걸로부터 만들어본다.
			face_index[cnt] = j+now_obj->start_id;
			cnt++;
		}
		//--라이트맵 사이즈대로 서팅하자.
		udword *sorted=NULL;
		if(cnt!=0)
		{
			Radix.InitSort();
			sorted = Radix.Sort(face_lsize,cnt).GetIndices();
		}

		BOOL is_fill_ok=FALSE;
		if( ngen_light_map == NULL )
		{
			ngen_light_map = new CGenLightMap;
			GenLightMap = ngen_light_map;
			ngen_light_map->mIndex = lgt_cnt;
			lgt_cnt++;
			goto FIRST;
		}

		for( now_light_map=GenLightMap; now_light_map!=NULL;now_light_map = now_light_map->mNext)
		{
//			sprintf(error_buf,"%d\n",error_cnt++);
//			PutR3BspInfoText(error_buf);
			if( now_light_map->mBufSize+size >= now_light_map->mLgtMap.xl*now_light_map->mLgtMap.yl )
				continue;
FILL:			
			memcpy(L_edge,now_light_map->mLEdge,_MAX_TEXTURE_SIDE*sizeof(int));	// 왼쪽 에지테이블
			memcpy(R_edge,now_light_map->mREdge,_MAX_TEXTURE_SIDE*sizeof(int));	// 왼쪽 에지테이블
			//--yl가 가장 큰 맵대로 차근차근
			float *now_lu=face_lu;
			float *now_lv=face_lv;
			for(j=0; j<cnt; j++)
			{
				if( now_light_map->FillLightMap(&divide_lgtmap[GetLeanearFaceIndex(mesh,blockmat->mPolyFace[face_index[sorted[cnt-1-j]]])],blockmat->mPolyFace[face_index[sorted[cnt-1-j]]],now_lu,now_lv) == FALSE)
//				if( now_light_map->FillLightMap(&divide_lgtmap[face_index[sorted[cnt-1-j]]],blockmat->mPolyFace[face_index[sorted[cnt-1-j]]],now_lu,now_lv) == FALSE)
				{
					//현재 라이트맵에 다 못넣는 경우가 생겼다. -.- 새로운 라이트맵을 생성하자...
					if( now_light_map->mCnt == 0 ) //처음 넣는곳에 못넣으면...
					{
						Dfree(now_light_map->mLgtMap.bitmap);
						if(xsize < _MAX_TEXTURE_SIDE)
						{
							xsize *= 2;
							now_light_map->mLgtMap.xl*=2;
						}
						else
						if(ysize < _MAX_TEXTURE_SIDE)
						{
							ysize *= 2;
							now_light_map->mLgtMap.yl*=2;
						}
						else
						{
							_INIFILE *ini = GetIniFile();
							Error(ini->w_mesh->object[now_obj->i].objectname,"<-이 오브젝트의 라이트맵이 너무커서 버퍼를 채울수가 없다.");
						}
						goto REFILL;
					}
					//기존거에 넣어보다 실패해서 다음거에 넣는다.
					memcpy(L_edge,now_light_map->mLEdge,_MAX_TEXTURE_SIDE*sizeof(int));	// 왼쪽 에지테이블
					memcpy(R_edge,now_light_map->mREdge,_MAX_TEXTURE_SIDE*sizeof(int));	// 왼쪽 에지테이블
					goto NEWFILL;
				}
				now_lu+=blockmat->mPolyFace[face_index[sorted[cnt-1-j]]]->vertex_num;
				now_lv+=blockmat->mPolyFace[face_index[sorted[cnt-1-j]]]->vertex_num;
			}
			//--오케이 잘 채워졌다..  통짜로 바뀐 lu,lv를 넣는다.
			now_lu=face_lu;
			now_lv=face_lv;
			for(j=0; j<cnt; j++)
			{
				for(k=0; k<blockmat->mPolyFace[face_index[sorted[cnt-1-j]]]->vertex_num; k++)
				{
					blockmat->mPolyFace[face_index[sorted[cnt-1-j]]]->lu[k] = now_lu[k];
					blockmat->mPolyFace[face_index[sorted[cnt-1-j]]]->lv[k] = now_lv[k];
				}
				now_lu+=blockmat->mPolyFace[face_index[sorted[cnt-1-j]]]->vertex_num;
				now_lv+=blockmat->mPolyFace[face_index[sorted[cnt-1-j]]]->vertex_num;
				blockmat->mPolyFace[face_index[sorted[cnt-1-j]]]->l_mtlid = now_light_map->mIndex;
			}
			now_light_map->mCnt++;
			now_light_map->mBufSize += size;
			is_fill_ok = TRUE;
			now_obj->lgtid = now_light_map->mIndex+base_lightmap_cnt;
			memcpy(now_light_map->mLEdge,L_edge,_MAX_TEXTURE_SIDE*sizeof(int));	// 왼쪽 에지테이블
			memcpy(now_light_map->mREdge,R_edge,_MAX_TEXTURE_SIDE*sizeof(int));	// 왼쪽 에지테이블
			break;
NEWFILL:
			int test=0;
		}
		if( is_fill_ok == FALSE )  //못채웠으면..
		{		
			ngen_light_map->mNext = new CGenLightMap;
			ngen_light_map =ngen_light_map->mNext;
			ngen_light_map->mIndex = lgt_cnt;
			lgt_cnt++;
FIRST:
			now_light_map = ngen_light_map;
			if( (float)size*_LIGHTMAP_EXT_SIZE > _MAX_TEXTURE_SIDE*_MAX_TEXTURE_SIDE )
			{
				Error("라이트맵 사이즈가 너무크다."," PatchSize를 줄여보던지 오브젝트를 나눌필요가있다.");
			}
			xsize=_FIT_TEXTURE_SIDE;
			ysize=_FIT_TEXTURE_SIDE;
			if( (float)size*_LIGHTMAP_EXT_SIZE > _FIT_TEXTURE_SIDE*_FIT_TEXTURE_SIDE )
			{
				//기본사이즈보다 크다면 사이즈를 다시 정해본다.
				GetLightMapSize((float)size*_LIGHTMAP_EXT_SIZE,maxxl,maxyl,&xsize,&ysize);
			}
			//라이트맵 xl,yl 길이및 메모리 할당
			now_light_map->mLgtMap.xl = xsize;
			now_light_map->mLgtMap.yl = ysize;
REFILL:		
			now_light_map->mLgtMap.bitmap = (_LIGHTMAP_COLOR *)Dmalloc(xsize*ysize*sizeof(_LIGHTMAP_COLOR));
			//이거는 검정색...
			memset(now_light_map->mLgtMap.bitmap,0,xsize*ysize*sizeof(_LIGHTMAP_COLOR));
			//--통짜 라이트맵을 분홍색으로 채우자.
//			memset(now_light_map->mLgtMap.bitmap,255,xsize*ysize*sizeof(_LIGHTMAP_COLOR));
			for(j=0; j<xsize*ysize; j++)
				now_light_map->mLgtMap.bitmap[j].G=0;
			//---엣지 버퍼를 클리어해놓자.
			for(j=0; j<ysize; j++)
			{
				now_light_map->mREdge[j] = xsize;
				now_light_map->mLEdge[j] = 0;
			}
			goto FILL;
		}
	}

	cnt=0;
	size=0;
	for( now_light_map=GenLightMap; now_light_map!=NULL;now_light_map = now_light_map->mNext)
	{
		size+=now_light_map->mLgtMap.xl*now_light_map->mLgtMap.yl;
		cnt++;
	}

	L_info.GenLightMap = GenLightMap;
	L_info.Lightmap_16bitsize = size*2;	//라이트맵 사이즈
	L_info.Lightmap_24bitsize = size*3;
	L_info.Lightmap_cnt = cnt;		//라이트맵 갯수 


	Dfree(face_index);
	Dfree(face_lsize);
	Dfree(divide_lgtmap);
	Dfree(divide_buf);
}
/*
	//--라이트맵을 머터리얼 별로 갯수를 만든다.
	lgt_cnt = mat->all_num;
	L_info.Lightmap_cnt = lgt_cnt;
	
	//----라이트맵 생성
	_LIGHT_MAP *lgt_map=(_LIGHT_MAP *)Dmalloc(sizeof(_LIGHT_MAP)*lgt_cnt);
	L_info.Light_map=lgt_map;



	int l_cnt=0;
	int start_i=0;
	//라이트맵 크기를 생성해보자.
	for(i=0; i<mat->all_num; i++)
	{
		size=0;
		cnt=0;
		start_i = i;
		int maxxl=0;
		int maxyl=0;
RELOAD:
		mat[i].lgt_id = l_cnt;
		int old_size = size;
		int old_cnt = cnt;
		for(j=0; j<mesh->object->facenum; j++)
		{
			if( i == mesh->object->p_face[j].mtlid )
			{
				mesh->object->p_face[j].l_mtlid = l_cnt;
				maxxl = max(maxxl,lf[j].patch->xl+2);
				maxyl = max(maxyl,lf[j].patch->yl+2);
				size += (lf[j].patch->xl+2)*(lf[j].patch->yl+2);
//				face_lsize[cnt]= lf[j].patch->xl*lf[j].patch->yl;
				face_lsize[cnt]= lf[j].patch->yl;
				face_index[cnt]=j;
				cnt++;
			}
		}
		if( (float)size*_LIGHTMAP_EXT_SIZE > _MAX_TEXTURE_SIDE*_MAX_TEXTURE_SIDE )
		{
			if( start_i == i)
			{
				char buf[256];
				sprintf(buf,"%d ",(int)((float)size*1.33f));
				Error(buf," size 라이트맵 생성 에러!,1개의 머터리얼에 너무 많은 크기의 폴리곤들에 맵핑을 했다.");
			}
			else
			{
				cnt = old_cnt;
				size = old_size;
				i--;
				goto PASS;
			}
		}
		else
		if(i != mat->all_num-1)
		{
			i++;
			goto RELOAD;
		}
PASS:
		//라이트맵 xl,yl 길이를 알아내자
		int xsize,ysize;
		GetLightMapSize((float)size*_LIGHTMAP_EXT_SIZE,maxxl,maxyl,&xsize,&ysize);
		lgt_map[l_cnt].xl = xsize;
		lgt_map[l_cnt].yl = ysize;

		//--라이트맵 사이즈대로 서팅하자.
		udword *sorted=NULL;
		if(cnt!=0)
		{
			Radix.InitSort();
			sorted = Radix.Sort(face_lsize,cnt).GetIndices();
		}
REFILL:		
		//--통짜 라이트맵을 분홍색으로 채우자.
		lgt_map[l_cnt].bitmap = (_LIGHTMAP_COLOR *)Dmalloc(xsize*ysize*sizeof(_LIGHTMAP_COLOR));
//		memset(lgt_map[l_cnt].bitmap,255,xsize*ysize*sizeof(_LIGHTMAP_COLOR));
		memset(lgt_map[l_cnt].bitmap,0,xsize*ysize*sizeof(_LIGHTMAP_COLOR));
		for(j=0; j<xsize*ysize; j++)
			lgt_map[l_cnt].bitmap[j].G=0;

		//---엣지 버퍼를 클리어해놓자.
		//memset(mLEdge,0,sizeof(int)*ysize);
		for(j=0; j<ysize; j++)
		{
			mREdge[j] = xsize;
			mLEdge[j] = 0;
		}

		//--yl가 가장 큰 맵대로 차근차근
		float *now_lu=face_lu;
		float *now_lv=face_lv;
		for(j=0; j<cnt; j++)
		{
//			if( j == 19)
//				break;
//			if( j == 11)
//				break;
			if( FillLightMap(&lgt_map[l_cnt],&divide_lgtmap[face_index[sorted[cnt-1-j]]],&mesh->object->p_face[face_index[sorted[cnt-1-j]]],now_lu,now_lv) == FALSE)
			{
				//워쩐일인지 다 못넣는 경우가 생겼다.
				Dfree(lgt_map[l_cnt].bitmap);
				if(xsize < _MAX_TEXTURE_SIDE)
				{
					xsize *= 2;
					lgt_map[l_cnt].xl*=2;
				}
				else
				if(ysize < _MAX_TEXTURE_SIDE)
				{
					ysize *= 2;
					lgt_map[l_cnt].yl*=2;
				}
				else
					Error("라이트맵이 너무커서 버퍼를 채울수가 없다.","");
				goto REFILL;
			}
			now_lu+=mesh->object->p_face[face_index[sorted[cnt-1-j]]].vertex_num;
			now_lv+=mesh->object->p_face[face_index[sorted[cnt-1-j]]].vertex_num;
		}
		//--통짜로 바뀐 lu,lv를 넣는다.
		now_lu=face_lu;
		now_lv=face_lv;
		for(j=0; j<cnt; j++)
		{
			for(k=0; k<mesh->object->p_face[face_index[sorted[cnt-1-j]]].vertex_num; k++)
			{
				mesh->object->p_face[face_index[sorted[cnt-1-j]]].lu[k] = now_lu[k];
				mesh->object->p_face[face_index[sorted[cnt-1-j]]].lv[k] = now_lv[k];
			}
			now_lu+=mesh->object->p_face[face_index[sorted[cnt-1-j]]].vertex_num;
			now_lv+=mesh->object->p_face[face_index[sorted[cnt-1-j]]].vertex_num;
		}
		//만약 아래 절반이 남았다면 조정해준다.
		AdjustLightMap(&lgt_map[l_cnt],mesh->object->p_face,sorted,face_index,cnt);
		l_cnt++;
	}
	lgt_cnt = l_cnt;
	L_info.Lightmap_cnt = lgt_cnt;

	Dfree(face_index);
	Dfree(face_lsize);
	Dfree(divide_lgtmap);
	Dfree(divide_buf);
*/


/*
//---라이트맵의 총갯수를 구한다.
	lgt_cnt = mesh->object->facenum;
	L_info.Lightmap_cnt = mesh->object->facenum;
//----라이트맵 생성
	_LIGHT_MAP *lgt_map=(_LIGHT_MAP *)Dmalloc(sizeof(_LIGHT_MAP)*lgt_cnt);
	L_info.Light_map=lgt_map;


	lgt_cnt=0;
	int size=0;
	for(i=0; i<mesh->object->facenum; i++)
	{
		mesh->object->p_face[i].lgt_ptr = GetLightmapFromPatch(lf[i].patch, &lgt_map[lgt_cnt]);
		mesh->object->p_face[i].l_mtlid = lgt_cnt;
		size+=lf[i].patch->xl*lf[i].patch->yl;
		lgt_cnt++;
	}
	L_info.Lightmap_16bitsize=2*size;
	L_info.Lightmap_24bitsize=3*size;
	PutR3BspInfoText(" Ok!\n");
//	_LIGHTMAP_INFO *GetLightmapInfo();
	PutR3BspInfoText("Lightmap num = %5d , 16bit size = %5dk , 24bit size = %5dk\n",L_info.Lightmap_cnt
		,L_info.Lightmap_16bitsize/1000,L_info.Lightmap_24bitsize/1000);
*/

void DeleteLightmapsFromPatchs(_MESH *mesh)
{
//	int i,k;

	_LFACE **lf = GetLFace();
	if( lf==NULL )
		Error("먼저 팻치,와인딩을 설정해라.","");
/*
	int cnt=0;
	for(k=0; k<mesh->objectnum; k++)
		for(i=0; i<mesh->object[k].facenum; i++)
		{
			Dfree(L_info.Light_map[cnt++].bitmap);
		}
	Dfree(L_info.Light_map);
*/
}
/*
void ConvertOppasityMap(_MATERIAL *mat)
{
	int i,j,r,g,b,alpha;


	if( mat->opacity == NULL )
		Error("모야 오파한다며...",mat->mapname);
	
	for(i=0; i<mat->mapyl; i++)
	{
		for(j=0; j<mat->mapxl; j++)
		{
			if( mat->buffer[i*mat->mapxl+j] > 0 )
				j=j+i-i;
			r = (mat->buffer[i*mat->mapxl+j]>>11)&0xf;
			g = (mat->buffer[i*mat->mapxl+j]>>6)&0xf;
			b = (mat->buffer[i*mat->mapxl+j]>>1)&0xf;
			alpha = ((mat->opacity[i*mat->mapxl+j]>>10)
				+((mat->opacity[i*mat->mapxl+j]>>5)&0x1f)
				+((mat->opacity[i*mat->mapxl+j])&0x1f))/6;
			mat->buffer[i*mat->mapxl+j] = (alpha<<12)|(r<<8)|(g<<4)|(b);
		}
	}
}
*/

static BYTE stHeader565[128] = {
	68,68,83,32,124,0,0,0
	,7,16,10,0,0,1,0,0
	,0,1,0,0,0,0,2,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,32,0,0,0
	,64,0,0,0,0,0,0,0
	,16,0,0,0,0,248,0,0
	,224,7,0,0,31,0,0,0
	,0,0,0,0,8,16,64,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0
};
static BYTE stHeader8888[128] = {
	68,68,83,32,124,0,0,0
	,7,16,10,0,0,1,0,0
	,0,1,0,0,0,0,4,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,32,0,0,0
	,65,0,0,0,0,0,0,0
	,32,0,0,0,0,0,255,0
	,0,255,0,0,255,0,0,0
	,0,0,0,255,8,16,64,0
	,0,0,0,0,0,0,0,0
	,0,0,0,0,0,0,0,0
};

void SaveLightMap(_MESH *mesh,_MATERIAL *com_mat)	//컴포짓 머터리얼로 라이트맵을 같이 셋팅.
{
	int i;
	char filename[256];
	char tempname[256]="md ";
	char lgtfilename[256]=".\\Map\\";
	FILE *fp;
	int lightmap_cnt;
	_LFACE **lf;
	CGenLightMap *gen_map;
	typedef struct{
		DWORD dwMagic;// (0x20534444, or "DDS")	//dds파일 포맷의 앞의 128바이트.... ^^
		DDSURFACEDESC2 ddsd;
	}_DDS_Header;
	_DDS_Header dds_header;
	dds_header.dwMagic = 0x20534444;

	_INIFILE *Ini=GetIniFile();
//	MakeDir("LightMap");
//------------------------------라이트맵 세이브
	PutR3BspInfoText("Creating light map!\n");
	strcpy(filename,Ini->In_file);
	StripPath(filename);
	StripEXT(filename);

	strcat(lgtfilename,filename);
	strcat(tempname,lgtfilename);
//	MakeDir(lgtfilename);
	strcpy(tempname,lgtfilename);
	strcat(tempname,"\\");
	strcat(tempname,filename);
	strcpy(lgtfilename,tempname);

	if( Ini->IsExportLightmapBMP )	// 라이트맵을 bmp로...
	{
		char lgt_name[256];
		strcpy(lgt_name,tempname);
		strcat(lgt_name,".txt");
		if( (fp=fopen(lgt_name,"wt"))==NULL)
			Error("세이브를 못함.. 파일이름에 스페이스(공백)이있으면 안됩니다. ",lgt_name);
		lightmap_cnt = GetLightmapCnt();
		fprintf(fp,"Lightmap_cnt %d",lightmap_cnt);
		fclose(fp);

		lf = GetLFace();
		if( lf==NULL )
			Error("먼저 팻치,와인딩을 설정해라.","");

		gen_map = GetLightmap();
		for(i=0; i<lightmap_cnt; i++)
		{
			_LIGHT_MAP *lgt_map = &gen_map->mLgtMap;
			strcpy(lgt_name,lgtfilename);
			sprintf(filename,"%s%06d.bmp",lgt_name,i);
			SaveBmp24(filename,lgt_map->xl,lgt_map->yl
				,(BYTE *)lgt_map->bitmap);
			gen_map = gen_map->mNext;
		}
	}

	strcat(tempname,"Lgt.R3T");
	if( (fp=fopen(tempname,"wb"))==NULL)
		Error("세이브를 못함.. 파일이름에 스페이스(공백)이있으면 안됩니다. ",tempname);
	lightmap_cnt = GetLightmapCnt();
	int all_lightmap_cnt = GetLightmapCnt();
	if( com_mat )
		all_lightmap_cnt+=com_mat->all_num;

	lf = GetLFace();
	if( lf==NULL )
	{
		Warning("먼저 팻치,와인딩을 설정해라. 또는 라이트맵이 하나도 없는경우","");
		fclose(fp);
		return;
	}

#define D3DFMT_A8R8G8B8			21
#define D3DFMT_R5G6B5			23

	gen_map = GetLightmap();
	float version = _R3T_FILE_VERSION;

	DWORD format;
	if( Ini->LightMapBits == 16 )	//라이트맵	비트수..
		format = D3DFMT_R5G6B5;	//rgb 888포맷
	else
//	if( Ini->LightMapBits == 24 )	//라이트맵	비트수..
		format = D3DFMT_A8R8G8B8;	//rgb 888포맷

	DWORD size,flag=0;

	fwrite(&version,4,1,fp);	//버전
//	fwrite(&format,4,1,fp);		//포맷
	fwrite(&all_lightmap_cnt,4,1,fp);	//텍스쳐 개수 

	if( com_mat )
	{
		for(i=0; i<com_mat->all_num; i++)
		{
			int xl,yl;
			if( !com_mat[i].tex )
				Error("컴포짓 라이트맵텍스쳐가 없습니다.","");

			char *full_path=GetFullTexPath(com_mat[i].tex->mapname);
			if( IsExistFile(full_path) == NULL )
				Error(full_path,"이 라이트맵 파일이 없습니다. 치명적오류..");
			BYTE *tex_buffer=LoadBmp(full_path,&xl,&yl);
			xl++;
			yl++;

			size = xl*yl*4;
			BYTE *temp_b = (BYTE *)Dmalloc(size);
			if( format == D3DFMT_A8R8G8B8 )
			{
				memcpy( &dds_header,stHeader8888,sizeof(_DDS_Header));
				dds_header.ddsd.dwWidth = xl;
				dds_header.ddsd.dwHeight = yl;
				size = xl*yl*4;
//				fwrite(&size,4,1,fp);	//텍스쳐 사이즈 
				for(int k=0; k<yl; k++)
				{
					for(int j=0; j<xl; j++)
					{
						temp_b[(k*xl+j)*4+3]=0xff;	//A
						temp_b[(k*xl+j)*4+0]=tex_buffer[(k*xl+j)*3+0];
						temp_b[(k*xl+j)*4+1]=tex_buffer[(k*xl+j)*3+1];
						temp_b[(k*xl+j)*4+2]=tex_buffer[(k*xl+j)*3+2];
					}
				}
			}
			else
			if( format == D3DFMT_R5G6B5 )
			{
				memcpy( &dds_header,stHeader565,sizeof(_DDS_Header));
				dds_header.ddsd.dwWidth = xl;
				dds_header.ddsd.dwHeight = yl;
				size = xl*yl*2;
//				fwrite(&size,4,1,fp);	//텍스쳐  사이즈
				WORD *temp_w=(WORD *)temp_b;
				DWORD r,g,b;
				for(int k=0; k<yl; k++)
				{
					for(int j=0; j<xl; j++)
					{
						b = (tex_buffer[(k*xl+j)*3+0]>>3)&31;
						g = (tex_buffer[(k*xl+j)*3+1]>>2)&63;
						r = (tex_buffer[(k*xl+j)*3+2]>>3)&31;
						temp_w[(k*xl+j)] = (WORD)((r<<11)|(g<<5)|b);
					}
				}
			}
			DWORD total_size = sizeof(_DDS_Header)+size;

			fwrite(&total_size,sizeof(DWORD),1,fp);	//총사이즈.
			fwrite(&dds_header,sizeof(_DDS_Header),1,fp);	//텍스쳐 헤더  
			fwrite(temp_b,size,1,fp);	//텍스쳐 저장. 
			Dfree(temp_b);
		}
	}

	for(i=0; i<lightmap_cnt; i++)
	{
		_LIGHT_MAP *lgt_map = &gen_map->mLgtMap;
		size = lgt_map->xl*lgt_map->yl*4;
		BYTE *temp_b = (BYTE *)Dmalloc(size);

		if( format == D3DFMT_A8R8G8B8 )
		{
			memcpy( &dds_header,stHeader8888,sizeof(_DDS_Header));
			dds_header.ddsd.dwWidth = lgt_map->xl;
			dds_header.ddsd.dwHeight = lgt_map->yl;
			size = lgt_map->xl*lgt_map->yl*4;
			for(int k=0; k<lgt_map->yl; k++)
			{
				for(int j=0; j<lgt_map->xl; j++)
				{
					temp_b[(k*lgt_map->xl+j)*4+3]=0xff;
					temp_b[(k*lgt_map->xl+j)*4+0]=lgt_map->bitmap[k*lgt_map->xl+j].R;
					temp_b[(k*lgt_map->xl+j)*4+1]=lgt_map->bitmap[k*lgt_map->xl+j].G;
					temp_b[(k*lgt_map->xl+j)*4+2]=lgt_map->bitmap[k*lgt_map->xl+j].B;
				}
			}
		}
		else
		if( format == D3DFMT_R5G6B5 )
		{
			memcpy( &dds_header,stHeader565,sizeof(_DDS_Header));
			dds_header.ddsd.dwWidth = lgt_map->xl;
			dds_header.ddsd.dwHeight = lgt_map->yl;
			size = lgt_map->xl*lgt_map->yl*2;
			WORD *temp_w=(WORD *)temp_b;
			DWORD r,g,b;
			for(int k=0; k<lgt_map->yl; k++)
			{
				for(int j=0; j<lgt_map->xl; j++)
				{
					r = (lgt_map->bitmap[(k*lgt_map->xl+j)].R>>3)&31;
					g = (lgt_map->bitmap[(k*lgt_map->xl+j)].G>>2)&63;
					b = (lgt_map->bitmap[(k*lgt_map->xl+j)].B>>3)&31;
					temp_w[(k*lgt_map->xl+j)] = (WORD)((r<<11)|(g<<5)|b);
				}
			}
		}
		DWORD total_size = sizeof(_DDS_Header)+size;

		fwrite(&total_size,sizeof(DWORD),1,fp);	//총사이즈.
		fwrite(&dds_header,sizeof(_DDS_Header),1,fp);	//텍스쳐 헤더  
		fwrite(temp_b,size,1,fp);	//텍스쳐 x길이 
		Dfree(temp_b);

		gen_map = gen_map->mNext;
	}
	fclose(fp);
}
