#include "patch.h"
#include "rad.h"
#include "Jerror.h"
#include "Jmalloc.h"
#include "math.h"
#include "R3BspUtil.h"
#include <stdio.h>
#include <process.h>

CRITICAL_SECTION		stCrit;

#define _ON_EPSILON		0.01

class Rad{
protected:
	_MATERIAL *mMaterial;
	_LIGHTSOURCE *mLight;
	_LIGHTSOURCE *mNowLight;	//현재 계산중인 라이트
	_LFACE **mlf;				//계산 패치들

	_MESH *All_mesh;
	int *Lgt_face_table;
	Vector3uc *mWindingTemp;
	_POLYFACE **Lgt_face_ptr;
	int Start_Lgt_face_table;	//쉐도우계산을 할시작 페이스인덱스 //캐스트 쉐도우얘들은 빼야되니까..
	int Lgt_face_table_cnt;//=0;

	void GetUV(Vector2f getuv,_POLYFACE *face,Vector3f p,int obj_num);	//평면위의 한 점을 넘겨주면 uv를 구해준다.
	void GetPointRGBBuffer(Vector3f color,_POLYFACE *face,Vector3f p,_TEXTURE *tex,int obj_num);
	float IsCollisionLight(_LIGHTSOURCE *lgt_mesh, Vector3f v,int n_face_num);
	inline float GetIntensity(_LIGHTSOURCE *lgt_mesh, Vector3f n, Vector3f v,BOOL shadow_calc,int n_face_num,BOOL *shadow_collision);
	void GetPatchIntensity(_LIGHTSOURCE *lgt_mesh,_PATCH *patch,int n_face_num);
	void GetShadowPatchIntensity(_LIGHTSOURCE *lgt_mesh,_PATCH *patch,int n_face_num);
	void GetLightAttFaceTable( _LIGHTSOURCE *lgt_mesh);
public :
	void ThreadCalculateIntensity(int start,int end);
	void ThreadLightCalculate();
	void AddBlurShadowPatchIntensity(_LIGHTSOURCE *lgt_mesh,float range,_PATCH *patch);
	Rad(_MESH *mesh,_MATERIAL *mat,_LIGHTSOURCE *lgt_mesh,int num,int max_windind_length);
	~Rad();
};
Rad::Rad(_MESH *mesh,_MATERIAL *mat,_LIGHTSOURCE *lgt_mesh,int num,int max_windind_length)
{
	All_mesh = mesh;
	
	mMaterial=mat;
	mLight=lgt_mesh;

	Lgt_face_table_cnt = 0;
	Lgt_face_table = (int *)Dmalloc(sizeof(int)*num);
	Lgt_face_ptr = (_POLYFACE **)Dmalloc(sizeof(_POLYFACE *)*num);
	mWindingTemp=(Vector3uc*)Dmalloc(sizeof(Vector3uc)*max_windind_length);

}
Rad::~Rad()
{
	Dfree(mWindingTemp);
	Dfree(Lgt_face_table);
	Dfree(Lgt_face_ptr);
}

static Vector3f Stained_rgb;	//스테인드의 색.
static float Stained_rate;		//스테인드색의 비율.

float DotProduct(Vector3f s,Vector3f e)
{
	return ((s[0]*e[0]+s[1]*e[1]+s[2]*e[2])/(VectorLength(s)*VectorLength(e)));
}

static void CrossVector( Vector4f a, Vector3f b, Vector3f r )
{
    r[0] = (a[1] * b[2]) - (a[2] * b[1]);
    r[1] = (a[2] * b[0]) - (a[0] * b[2]);
    r[2] = (a[0] * b[1]) - (a[1] * b[0]);
}
static int CheckEdge(Vector3f a,Vector3f b,Vector3f v,Vector4f fv)
{

	Vector3f vv,n;
	float dp;
//	double dp;

	vv[0] = a[0] - b[0];
	vv[1] = a[1] - b[1];
	vv[2] = a[2] - b[2];
    CrossVector(fv, vv, n);
    dp = (n[0] * ( v[0] - b[0])	+ n[1] * ( v[1] - b[1])
		+ n[2] * ( v[2] - b[2]));
/*
    if( dp < 0 )
        return 0;
	return 1;	//  통과 
    if(  dp < -N_EPSILON)
        return 0;
*/
#define N_EPSILON 1/3.0
    if(  dp > 0)
        return 0;
	return 1;	//  통과 
}

inline static int CheckEdge(Vector4f edge_n,Vector3f p)
{
	if( edge_n[0]*p[0]+edge_n[1]*p[1]+edge_n[2]*p[2]-edge_n[3] > _ON_EPSILON)
		return 1;
	return 0;
}


void Rad::GetUV(Vector2f getuv,_POLYFACE *face,Vector3f p,int obj_num)	//평면위의 한 점을 넘겨주면 uv를 구해준다.
{
	_VERTEX *v=All_mesh->object[obj_num].vertex;
	int v0,v1,v2,r0,r1,r2;
	int i;
	Vector3f temp;
	int uv0,uv1,uv2,ruv0,ruv1,ruv2;
	float dist1,dist2,dist0;
	
	for(i=0; i<face->vertex_num-2; i++)
	{
		v0 = face->vertex_id[0];
		v1 = face->vertex_id[i+1];
		v2 = face->vertex_id[i+2];
		uv0 = 0;
		uv1 = i+1;
		uv2 = i+2;
		// 삼각형 안이면 
		if( CheckEdge(v[v0].local,v[v1].local,p,face->f_normal) == 1
			&& CheckEdge(v[v1].local,v[v2].local,p,face->f_normal) == 1
			&& CheckEdge(v[v2].local,v[v0].local,p,face->f_normal) == 1  )
		{
			break;
		}
	}	
	Vector3fSub(v[v0].local,p,temp);
	dist0=Vector3fDist(temp);
	Vector3fSub(v[v1].local,p,temp);
	dist1=Vector3fDist(temp);
	Vector3fSub(v[v2].local,p,temp);
	dist2=Vector3fDist(temp);
	if( dist0 > dist1)	//오차문제땜에 거리가 가장 큰걸로하자..
	{
		if( dist0 > dist2)
		{
			r0=v0;
			r1=v1;
			r2=v2;
			ruv0=uv0;
			ruv1=uv1;
			ruv2=uv2;
		}
		else
		{
			r0=v2;
			r1=v0;
			r2=v1;
			ruv0=uv2;
			ruv1=uv0;
			ruv2=uv1;
		}
	}
	else
	{
		if( dist1 > dist2)
		{
			r0=v1;
			r1=v2;
			r2=v0;
			ruv0=uv1;
			ruv1=uv2;
			ruv2=uv0;
		}
		else
		{
			r0=v2;
			r1=v0;
			r2=v1;
			ruv0=uv2;
			ruv1=uv0;
			ruv2=uv1;
		}
	}
	Vector3f a,b;
	float leng0,leng1;

	Vector3fSub(v[r1].local,v[r0].local,a);
	leng0 = Vector3fDist(a);
	Vector3fSub(v[r2].local,v[r0].local,b);
	leng1 = Vector3fDist(b);
	Vector3fSub(p,v[r0].local,temp);
	dist2 = Vector3fDist(temp);

	Normalize(a);
	Normalize(b);
	Normalize(temp);
	dist0 = Vector3fDot(a,temp);	//a 각도.
	dist0 = sqrtf(1-dist0*dist0);		//사인 값
	dist1 = Vector3fDot(b,temp);	//b 각도.
	dist1 = sqrtf(1-dist1*dist1);		//사인 값
	dist0 *= leng0;
	dist1 *= leng1;

	//길이에 관한 a의 비율
	dist0 = dist0/(dist1+dist0);
	Vector3fSub(v[r2].local,v[r1].local,a);
	Vector3fScale(a,dist0,a);
	Vector3fAdd(a,v[r1].local,a);	//교점을 구했다.

	Vector3fSub(a,v[r0].local,b);
	dist1 = Vector3fDist(b);
	dist1 = dist2/dist1;			// 2번째 비례

	//---마무리계산...
	getuv[0] = (face->fu[ruv2] - face->fu[ruv1])*dist0 + face->fu[ruv1];
	getuv[1] = (face->fv[ruv2] - face->fv[ruv1])*dist0 + face->fv[ruv1];
	
	getuv[0] = (getuv[0] - face->fu[ruv0])*dist1 + face->fu[ruv0];
	getuv[1] = (getuv[1] - face->fv[ruv0])*dist1 + face->fv[ruv0];
}


void Rad::GetPointRGBBuffer(Vector3f color,_POLYFACE *face,Vector3f p,_TEXTURE *tex,int obj_num)
{
	Vector2f getuv;

	if( tex->buffer == NULL )
	{
		color[2] = 0;
		color[1] = 0;
		color[0] = 0;
		return;
	}
	GetUV(getuv,face,p,obj_num);	//평면위의 한 점을 넘겨주면 uv를 구해준다.

	int xp = (int)(((getuv[0]+1000.0f)*tex->mapxl))%tex->mapxl;
	int yp = (int)(((getuv[1]+1000.0f)*tex->mapyl))%tex->mapyl;

	yp = tex->mapyl - yp -1;

	color[2] = (float)(tex->buffer[(xp+yp*tex->mapxl)*3+0])/255.0f;
	color[1] = (float)(tex->buffer[(xp+yp*tex->mapxl)*3+1])/255.0f;
	color[0] = (float)(tex->buffer[(xp+yp*tex->mapxl)*3+2])/255.0f;
}
typedef struct{
	Vector3f m_point;
	Vector4f edge_normal[4];
}_FACE_UTIL;
static _FACE_UTIL **Futil;

#define _OBJECT_SHIFT 16	//6만폴리 6만오브젝트.

float Rad::IsCollisionLight(_LIGHTSOURCE *lgt_mesh, Vector3f v,int n_face_num)
{
	Vector3f vector;
	float inten=1.0f;
	int j;

	float front,back;

	float frontx = v[0];
	float fronty = v[1];
	float frontz = v[2];
	float backx = lgt_mesh->pos[0];
	float backy = lgt_mesh->pos[1];
	float backz = lgt_mesh->pos[2];

	for(int i=Start_Lgt_face_table; i<Lgt_face_table_cnt; i++)
	{
//		_POLYFACE *face = &All_mesh->object[Lgt_face_table[i]>>16].p_face[Lgt_face_table[i]&0x0000ffff];
		_POLYFACE *face = Lgt_face_ptr[i];

        front=face->f_normal[0]*frontx+face->f_normal[1]*fronty+face->f_normal[2]*frontz -face->f_normal[3];
		if( front > _ON_EPSILON )
			continue;
        back= face->f_normal[0]*backx +face->f_normal[1]*backy +face->f_normal[2]*backz  -face->f_normal[3];
		if( back < 0 )
			continue;
		if( Lgt_face_table[i] == n_face_num )
			continue;

		// 교점을 구하고
		front = front / (front-back);
	
		vector[0] = frontx + front*(backx-frontx);
		vector[1] = fronty + front*(backy-fronty);
		vector[2] = frontz + front*(backz-frontz);

//		_VERTEX *ver=All_mesh->object[Lgt_face_table[i]>>16].vertex;
		for(j=0; j<face->vertex_num; j++)
		{
/*
			Vector4fp edge_temp = Futil[Lgt_face_table[i]>>16][Lgt_face_table[i]&0x0000ffff].edge_normal[j];
			float d_tist = Vector3fDot(edge_temp,vector)-edge_temp[3];
			if(  d_tist <_ON_EPSILON)
				break;
*/

			if( CheckEdge(Futil[(Lgt_face_table[i]>>16)][(Lgt_face_table[i]&0x0000ffff)].edge_normal[j],vector) ==0)
				break;//			int ii=0;
/*
			if( CheckEdge(ver[face->vertex_id[j]].local
				,ver[face->vertex_id[(j+1)%face->vertex_num]].local
				,vector,face->f_normal) == 0 )
				break;	
*/
		}
		if( j != face->vertex_num )
			continue;
		if(face->mtlid != -1)
		{
			BOOL is_continue=0;
			
			if(mMaterial[face->mtlid].transparency!=1)	//반투명일경우..
			{
				is_continue=1;
				inten*=(1-mMaterial[face->mtlid].transparency);
			}
			if(mMaterial[face->mtlid].oppa)	//오파의 경우
			{
				is_continue=1;
				GetPointRGBBuffer(Stained_rgb,face,vector,mMaterial[face->mtlid].oppa,(Lgt_face_table[i]>>16));
				inten*=(1.0f-(Stained_rgb[0]+Stained_rgb[1]+Stained_rgb[2])/3.0f);
			}
			if(mMaterial[face->mtlid].self_illum != 0.0f)	//스테인드 글라스의 경우
			{
				is_continue=1;
				GetPointRGBBuffer(Stained_rgb,face,vector,mMaterial[face->mtlid].tex,(Lgt_face_table[i]>>16));
				Stained_rate=mMaterial[face->mtlid].self_illum;
				if(inten > 0)
					inten = -inten;
			}
			if( is_continue )
				continue;
			return 0;
		}
		else
			return 0.001f;
	}
	return inten;
}

inline float Rad::GetIntensity(_LIGHTSOURCE *lgt_mesh, Vector3f n, Vector3f v,BOOL shadow_calc,int n_face_num,BOOL *shadow_collision)
{
	float intensity=lgt_mesh->intens;	//기본부터 시작
	float att_inten=1.0f;	//기본 감쇠
	Vector3f lgt,lgt_to_face;
	lgt[0] = lgt_mesh->pos[0];
	lgt[1] = lgt_mesh->pos[1];
	lgt[2] = lgt_mesh->pos[2];

//--뒤집힌것 제거
	VectorSubtract(lgt,v,lgt_to_face);	//lgt_to_face는 라이트와 점과의 거리
//	VectorSubtract(v,lgt,lgt_to_face);	//lgt_to_face는 라이트와 점과의 거리
	float s_dot = DotProduct( lgt_to_face,n );
	if( s_dot <= 0)
	{
		return 0;	//각도가 벗어나면 별볼일 없다.
	}
	else
	{
		intensity *= s_dot;	//면과 라이트와의 각도의 강도를 구한다.
	}
//--Attenuation 강도를 구한다.

	if(	lgt_mesh->attn_faron )	//라이트 attenuation 이있을경우
	{
		float sq=VectorLength(lgt_to_face);
        if(sq>lgt_mesh->attnstart && sq < lgt_mesh->attnend)	//att 구역에있다.
        {
            att_inten=(1.0f-(sq-lgt_mesh->attnstart)
                /(lgt_mesh->attnend-lgt_mesh->attnstart));
        }
        else
        if( sq >= lgt_mesh->attnend)	//벗어난것 제거.
		{
            att_inten=0.0;		
		}
		else
            att_inten=1.0;
	}

	if( lgt_mesh->type == _SPOT )
	{
		Vector3f spot;
		float spot_leng;

		spot[0] = lgt_mesh->tpos[0];
		spot[1] = lgt_mesh->tpos[1];
		spot[2] = lgt_mesh->tpos[2];
//		VectorSubtract(spot,lgt,spot);
		VectorSubtract(lgt,spot,spot);
		float angle =DotProduct(spot,lgt_to_face);
        angle=(float)acos(angle);
        spot_leng=(float)((angle*180.0)/_PI);
        if(spot_leng < lgt_mesh->hotspot/2 )
        {
            intensity *=1.0;
        }
        else if(spot_leng < lgt_mesh->falloff/2 )
        {
			intensity *=(1.0f-(spot_leng-lgt_mesh->hotspot/2.0f)
                /(lgt_mesh->falloff/2.0f-lgt_mesh->hotspot/2.0f));
        }
        else
        {
            intensity = 0;
        }
	}

//-------- 자 마지막 강도를 구한다.
	intensity *= att_inten;	//최종 강도를 구한다.

	if( intensity == 0 )	//계산할 필요가없다.
	{
		return 0;
	}
//-------- 점과 라이트사이에 면이 가리는가를 알아낸다. 
	if( shadow_calc )
	{
		float s_inten = IsCollisionLight(lgt_mesh,v,n_face_num);
		if(s_inten == 0)
			*shadow_collision=TRUE;	//쉐도우 다..
		else
			return s_inten*intensity;
	}
//-------- 최종 강도를 리턴한다.
	return intensity;
}

void Rad::GetPatchIntensity(_LIGHTSOURCE *lgt_mesh,_PATCH *patch,int n_face_num)
{
	int i,j;
	Vector3f n;
	float form_factor;
	Vector3f color;
	
	n[0] = patch->fnormal[0];
	n[1] = patch->fnormal[1];
	n[2] = patch->fnormal[2];

	_MATERIAL *mat = &mMaterial[patch->mtlid];

	BOOL shadows_calc;
	_INIFILE *Ini=GetIniFile();
	if( Ini->RenderState ==_R_OPTIMIZE_FASTEST || !lgt_mesh->is_shadow 
	|| (All_mesh->object[(n_face_num>>_OBJECT_SHIFT)].type&_SHADOW_RECEIVE_OFF) )
		shadows_calc = FALSE;
	else
		shadows_calc = TRUE;

	for(i=0; i<patch->yl; i++)
	{
		for(j=0; j<patch->xl; j++)
		{
			_WINDING *winding=&patch->winding[i*patch->xl + j];
			BOOL shadow_collision=FALSE;
			form_factor = GetIntensity(lgt_mesh,winding->normal,winding->pos,shadows_calc,n_face_num,&shadow_collision)/_MAX_INTENSITY;	//강도 2배까지 지원한다.

			if( form_factor >= 0 )	
			{
				if( shadow_collision )	//뒤집히거나 영역 밖의 면이다.
				{
					color[0] = (BYTE)((lgt_mesh->shadow_color[0]/_MAX_INTENSITY)*255);
					color[1] = (BYTE)((lgt_mesh->shadow_color[1]/_MAX_INTENSITY)*255);
					color[2] = (BYTE)((lgt_mesh->shadow_color[2]/_MAX_INTENSITY)*255);
				}
				else
				{
					color[0] = form_factor*lgt_mesh->rgb[0]*255;
					color[1] = form_factor*lgt_mesh->rgb[1]*255;
					color[2] = form_factor*lgt_mesh->rgb[2]*255;
				}
			}
			else	//스테인드 글라스의 경우..
			{
				form_factor = -form_factor*Stained_rate;
				color[0] = form_factor*(Stained_rgb[0])*lgt_mesh->rgb[0]*255;
				color[1] = form_factor*(Stained_rgb[1])*lgt_mesh->rgb[1]*255;
				color[2] = form_factor*(Stained_rgb[2])*lgt_mesh->rgb[2]*255;
			}
			winding->energy[0]=(BYTE)min(255,color[0]+winding->energy[0]);
			winding->energy[1]=(BYTE)min(255,color[1]+winding->energy[1]);
			winding->energy[2]=(BYTE)min(255,color[2]+winding->energy[2]);
		}
	}
}


void Rad::AddBlurShadowPatchIntensity(_LIGHTSOURCE *lgt_mesh,float range,_PATCH *patch)
{
	int i,j,k,m,f_leng,start;
	float r,g,b,tr,tg,tb,hop;
	_WINDING *winding;
	
	if( range >= 40 )
		f_leng=9;
	else
	if( range >= 32 )
		f_leng=7;
	else
	if( range >= 16 )
		f_leng=5;
	else
		f_leng=3;

	hop = (float)(f_leng*f_leng);
	start=-((f_leng-1)/2);

	for(i=0; i<patch->yl; i++)
	{
		for(j=0; j<patch->xl; j++)
		{
			mWindingTemp[i*patch->xl + j][0]=patch->winding[i*patch->xl + j].energy[0];
			mWindingTemp[i*patch->xl + j][1]=patch->winding[i*patch->xl + j].energy[1];
			mWindingTemp[i*patch->xl + j][2]=patch->winding[i*patch->xl + j].energy[2];
		}
	}
	//다된 블로우맵을 저장한다.
	for(i=0; i<patch->yl; i++)
	{
		for(j=0; j<patch->xl; j++)
		{
			r=0;	g=0;	b=0;
			tr=0;	tg=0;	tb=0;
			for(k=start; k<f_leng+start; k++)
			{
				for(m=start; m<f_leng+start; m++)
				{
					int x,y;
					x=max(j+m,0);
					y=max(i+k,0);
					x=min(x,patch->xl-1);
					y=min(y,patch->yl-1);
					winding=&patch->winding[y*patch->xl + x];
					r+=winding->temp_energy[0];
					g+=winding->temp_energy[1];
					b+=winding->temp_energy[2];
					tr+=mWindingTemp[y*patch->xl + x][0];
					tg+=mWindingTemp[y*patch->xl + x][1];
					tb+=mWindingTemp[y*patch->xl + x][2];
				}	
			}
			if ( r || g || b )	//쉐도우가 있는곳은 계산을 해준다.
			{
				winding=&patch->winding[i*patch->xl + j];
				r=((r*lgt_mesh->shadow_density)/hop);
				g=((g*lgt_mesh->shadow_density)/hop);
				b=((b*lgt_mesh->shadow_density)/hop);
				tr=(tr/hop);
				tg=(tg/hop);
				tb=(tb/hop);
//				tr=min(255,r*lgt_mesh->shadow_density+tr*(1-lgt_mesh->shadow_density));
//				tg=min(255,g*lgt_mesh->shadow_density+tg*(1-lgt_mesh->shadow_density));
//				tb=min(255,b*lgt_mesh->shadow_density+tb*(1-lgt_mesh->shadow_density));
				tr=min(255,tr+r);
				tg=min(255,tg+g);
				tb=min(255,tb+b);
				tr=max(0,tr);
				tg=max(0,tg);
				tb=max(0,tb);
				winding->energy[0]=(BYTE)tr;
				winding->energy[1]=(BYTE)tg;
				winding->energy[2]=(BYTE)tb;
			}
		}
	}
}

//순수한 쉐도우를 각각의 와인딩temp_energy에 저장한다.
void Rad::GetShadowPatchIntensity(_LIGHTSOURCE *lgt_mesh,_PATCH *patch,int n_face_num)
{
	int i,j;
	Vector3f n;
	float form_factor;
	Vector3f color;
	
	n[0] = patch->fnormal[0];
	n[1] = patch->fnormal[1];
	n[2] = patch->fnormal[2];

	BOOL shadows_calc;
	_INIFILE *Ini=GetIniFile();
	if( Ini->RenderState ==_R_OPTIMIZE_FASTEST || !lgt_mesh->is_shadow 
	|| (All_mesh->object[(n_face_num>>_OBJECT_SHIFT)].type&_SHADOW_RECEIVE_OFF) )
		shadows_calc = FALSE;
	else
		shadows_calc = TRUE;

	_MATERIAL *mat = &mMaterial[patch->mtlid];

	for(i=0; i<patch->yl; i++)
	{
		for(j=0; j<patch->xl; j++)
		{
			_WINDING *winding=&patch->winding[i*patch->xl + j];
			BOOL shadow_collision=FALSE;
			form_factor = GetIntensity(lgt_mesh,winding->normal,winding->pos,shadows_calc,n_face_num,&shadow_collision)/_MAX_INTENSITY;	//강도 2배까지 지원한다.

			winding->temp_energy[0]=0;
			winding->temp_energy[1]=0;
			winding->temp_energy[2]=0;
			if( form_factor >= 0 )	
			{
				if( shadow_collision )	
				{
					color[0] = (BYTE)((lgt_mesh->shadow_color[0]/_MAX_INTENSITY)*255);
					color[1] = (BYTE)((lgt_mesh->shadow_color[1]/_MAX_INTENSITY)*255);
					color[2] = (BYTE)((lgt_mesh->shadow_color[2]/_MAX_INTENSITY)*255);
					winding->temp_energy[0] = (BYTE)((lgt_mesh->shadow_color[0]/_MAX_INTENSITY)*255);
					winding->temp_energy[1] = (BYTE)((lgt_mesh->shadow_color[1]/_MAX_INTENSITY)*255);
					winding->temp_energy[2] = (BYTE)((lgt_mesh->shadow_color[2]/_MAX_INTENSITY)*255);
					if( winding->temp_energy[0] == 0)
						winding->temp_energy[0] =1;
				}
				else//뒤집히거나 영역 밖의 면이다.
				{
					color[0] = form_factor*lgt_mesh->rgb[0]*255;
					color[1] = form_factor*lgt_mesh->rgb[1]*255;
					color[2] = form_factor*lgt_mesh->rgb[2]*255;
				}
			}
			else	//스테인드 글라스의 경우..
			{
				form_factor = -form_factor*Stained_rate;
				color[0] = form_factor*(Stained_rgb[0])*lgt_mesh->rgb[0]*255;
				color[1] = form_factor*(Stained_rgb[1])*lgt_mesh->rgb[1]*255;
				color[2] = form_factor*(Stained_rgb[2])*lgt_mesh->rgb[2]*255;
			}
			winding->energy[0]=(BYTE)min(255,color[0]+winding->energy[0]);
			winding->energy[1]=(BYTE)min(255,color[1]+winding->energy[1]);
			winding->energy[2]=(BYTE)min(255,color[2]+winding->energy[2]);
		}
	}
}
void Rad::GetLightAttFaceTable(_LIGHTSOURCE *lgt_mesh)
{
	int i,k;
	Vector3f temp;
	_MESH *mesh = All_mesh;

	Start_Lgt_face_table=0;
	Lgt_face_table_cnt=0;
	//라이트 attenuation 이없을경우.
	if(	lgt_mesh->attn_faron == 0 )
	{
		for(k=0; k<mesh->objectnum; k++)
		{
			if( !(mesh->object[k].type & _SHADOW_CAST_OFF) )
				continue;
			for( i=0; i<mesh->object[k].facenum; i++)
			{
				Lgt_face_table[Lgt_face_table_cnt]=i|(k<<_OBJECT_SHIFT);
				Lgt_face_ptr[Lgt_face_table_cnt]=&mesh->object[k].p_face[i];
				Lgt_face_table_cnt++;
				Start_Lgt_face_table++;
			}
		}
		for(k=0; k<mesh->objectnum; k++)
		{
			if( mesh->object[k].type & _SHADOW_CAST_OFF )
				continue;
			for( i=0; i<mesh->object[k].facenum; i++)
			{
				Lgt_face_table[Lgt_face_table_cnt]=i|(k<<_OBJECT_SHIFT);
				Lgt_face_ptr[Lgt_face_table_cnt]=&mesh->object[k].p_face[i];
				Lgt_face_table_cnt++;
			}
		}
		return;
	}
	_LFACE **lf=GetLFace();

	float backx = lgt_mesh->pos[0];
	float backy = lgt_mesh->pos[1];
	float backz = lgt_mesh->pos[2];

	Vector3f bb_temp,bb_pos;
	for(k=0; k<mesh->objectnum; k++)
	{
		if( !(mesh->object[k].type & _SHADOW_CAST_OFF) )
			continue;
		//오브젝트 컬링.
		Vector3fSub(mesh->object[k].bb_max,mesh->object[k].bb_min,bb_temp);
		float h_dist = Vector3fDist(bb_temp)/2.0f;
		Vector3fScale(bb_temp,0.5f,bb_temp);
		Vector3fAdd(bb_temp,mesh->object[k].bb_min,bb_pos);
		Vector3fSub(bb_pos,lgt_mesh->pos,bb_temp);
		float l_dist = Vector3fDist(bb_temp);	//라이트와의 거리
		if( lgt_mesh->attnend+h_dist < l_dist )	//라이트영역에서 떨어졌다.
			continue;

		for( i=0; i<mesh->object[k].facenum; i++)
		{
			_POLYFACE *face = &mesh->object[k].p_face[i];

			float back= face->f_normal[0]*backx +face->f_normal[1]*backy +face->f_normal[2]*backz  -face->f_normal[3];
			if(fabs(back) > lgt_mesh->attnend)
				continue;

			//거리 첵크 
			Vector3fSub(lgt_mesh->pos,Futil[k][i].m_point,temp);
			float dist = Vector3fDist(temp);
			if( face->f_temp + lgt_mesh->attnend > dist)
			{
				Lgt_face_table[Lgt_face_table_cnt]=i|(k<<_OBJECT_SHIFT);
				Lgt_face_ptr[Lgt_face_table_cnt]=&mesh->object[k].p_face[i];
				Lgt_face_table_cnt++;
				Start_Lgt_face_table++;
			}
		}
	}
	for(k=0; k<mesh->objectnum; k++)
	{
		if( mesh->object[k].type & _SHADOW_CAST_OFF )
			continue;

		//오브젝트 컬링.
		Vector3fSub(mesh->object[k].bb_max,mesh->object[k].bb_min,bb_temp);
		float h_dist = Vector3fDist(bb_temp)/2.0f;
		Vector3fScale(bb_temp,0.5f,bb_temp);
		Vector3fAdd(bb_temp,mesh->object[k].bb_min,bb_pos);
		Vector3fSub(bb_pos,lgt_mesh->pos,bb_temp);
		float l_dist = Vector3fDist(bb_temp);	//라이트와의 거리
		if( lgt_mesh->attnend+h_dist < l_dist )	//라이트영역에서 떨어졌다.
			continue;

		for( i=0; i<mesh->object[k].facenum; i++)
		{
			_POLYFACE *face = &mesh->object[k].p_face[i];

			float back= face->f_normal[0]*backx +face->f_normal[1]*backy +face->f_normal[2]*backz  -face->f_normal[3];
			if(fabs(back) > lgt_mesh->attnend)
				continue;

			//거리 첵크 
			Vector3fSub(lgt_mesh->pos,Futil[k][i].m_point,temp);
			float dist = Vector3fDist(temp);
			if( face->f_temp + lgt_mesh->attnend > dist)
			{
				Lgt_face_table[Lgt_face_table_cnt]=i|(k<<_OBJECT_SHIFT);
				Lgt_face_ptr[Lgt_face_table_cnt]=&mesh->object[k].p_face[i];
				Lgt_face_table_cnt++;
			}
		}
	}
}

void GetPreCalculateMiddlePointDistAndEdgeNormal(_MESH *mesh) //페이스의 중간점과 거리를 미리계산한다.
{
	int i,j,k;
	Vector3f temp;

	for(k=0; k<mesh->objectnum; k++)
	{
		for(i=0; i<mesh->object[k].facenum; i++)
		{
			Vector3f min_f,max_f;
			min_f[0] = 1000000;		min_f[1] = 1000000;		min_f[2] = 1000000;
			max_f[0] = -1000000;		max_f[1] = -1000000;		max_f[2] = -10000;

			temp[0]=0;		temp[1]=0;		temp[2]=0;
			for(j=0; j<mesh->object[k].p_face[i].vertex_num; j++)
			{
				temp[0] += mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].x;
				temp[1] += mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].y;
				temp[2] += mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].z;
				if( mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].x < min_f[0] )
					min_f[0]= mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].x;
				if( mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].y < min_f[1] )
					min_f[1]= mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].y;
				if( mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].z < min_f[2] )
					min_f[2]= mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].x;

				if( mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].x > max_f[0] )
					max_f[0]= mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].x;
				if( mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].y > max_f[1] )
					max_f[1]= mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].y;
				if( mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].z > max_f[2] )
					max_f[2]= mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].z;
			}
			Futil[k][i].m_point[0] = temp[0]/(float)mesh->object[k].p_face[i].vertex_num;
			Futil[k][i].m_point[1] = temp[1]/(float)mesh->object[k].p_face[i].vertex_num;
			Futil[k][i].m_point[2] = temp[2]/(float)mesh->object[k].p_face[i].vertex_num;

			Vector3fSub( min_f, Futil[k][i].m_point,temp );
			float dist1 = Vector3fDist(temp);
			Vector3fSub( max_f, Futil[k][i].m_point,temp );
			float dist2 = Vector3fDist(temp);
			
			if( dist1 > dist2 )
				mesh->object[k].p_face[i].f_temp = dist1;
			else
				mesh->object[k].p_face[i].f_temp = dist2;
		}
	}
	for(k=0; k<mesh->objectnum; k++)
	{
		for(i=0; i<mesh->object[k].facenum; i++)
		{
			BOOL is_epsilron=FALSE;
			Vector3f edge_n;
			for(j=0; j<mesh->object[k].p_face[i].vertex_num; j++)
			{
				Vector3fp v0 = mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[j]].local;
				Vector3fp v1 = mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[(j+1)%mesh->object[k].p_face[i].vertex_num]].local;
				float l = v1[0] - v0[0];
				float m = v1[1] - v0[1];
				float n = v1[2] - v0[2];
				float t = -((  l*(v0[0]-Futil[k][i].m_point[0]) 
					+ m*(v0[1]-Futil[k][i].m_point[1]) 
					+ n*(v0[2]-Futil[k][i].m_point[2]) )/(l*l+m*m+n*n));
				
				edge_n[0] = Futil[k][i].m_point[0] - (l*t + v0[0]);
				edge_n[1] = Futil[k][i].m_point[1] - (m*t + v0[1]);
				edge_n[2] = Futil[k][i].m_point[2] - (n*t + v0[2]);

				if( fabs(edge_n[0]) < _ON_EPSILON && fabs(edge_n[1]) < _ON_EPSILON&& fabs(edge_n[2]) < _ON_EPSILON)
				{
					BOOL is_epsilron=TRUE;
					break;
				}
				
				//Vector3fNeg(edge_n,edge_n);
				Normalize(edge_n);

				Futil[k][i].edge_normal[j][0]=edge_n[0];
				Futil[k][i].edge_normal[j][1]=edge_n[1];
				Futil[k][i].edge_normal[j][2]=edge_n[2];
				Futil[k][i].edge_normal[j][3]=edge_n[0]*v1[0]+edge_n[1]*v1[1]+edge_n[2]*v1[2];

			}

			if( is_epsilron )
			{
				for(j=0; j<mesh->object[k].p_face[i].vertex_num; j++)
				{
					Vector3fp v1 = mesh->object[k].vertex[mesh->object[k].p_face[i].vertex_id[(j+1)%mesh->object[k].p_face[i].vertex_num]].local;
					if( j%3==0 )
					{
						edge_n[0]=0;	edge_n[1]=0;	edge_n[2]=1;
					}
					else
					if( j == 1 )
					{
						edge_n[0]=0;	edge_n[1]=0;	edge_n[2]=-1;
					}
					else
					if( j == 2 )
					{
						edge_n[0]=1;	edge_n[1]=0;	edge_n[2]=0;
					}
					Futil[k][i].edge_normal[j][0]=edge_n[0];
					Futil[k][i].edge_normal[j][1]=edge_n[1];
					Futil[k][i].edge_normal[j][2]=edge_n[2];
					Futil[k][i].edge_normal[j][3]=edge_n[0]*v1[0]+edge_n[1]*v1[1]+edge_n[2]*v1[2];
				}
			}
		}
	}
}

#define _ONLY_BLUR_RENDER	0xff
#define MAX_THREADS	64

typedef struct{
	Rad *rad;
	int start;
	int end;
	int id;	//해당 쓰레드의 아이디.
}_THREAD_STRUCT;	

BOOL stIsOutThread[MAX_THREADS];
_THREAD_STRUCT stThread[MAX_THREADS]; 

void ThreadFunc(void *mm)
{
	_THREAD_STRUCT *thread = (_THREAD_STRUCT *)mm;

	thread->rad->ThreadCalculateIntensity(thread->start,thread->end);
	stIsOutThread[thread->id]=TRUE;
	ExitThread( 526+ thread->id);
}
void WaitThreadEnd()
{
	int CPUNum = GetIniFile()->CPUNum;
	while( 1 )	//쓰래드가 다끝났는지 확인...
	{
		for( int i=0; i<CPUNum; i++)
		{
			if( stIsOutThread[i] == FALSE )
				break;
		}
		if( i >= (CPUNum) )
			break;
		Sleep(10);
	}
}

void Rad::ThreadCalculateIntensity(int start,int end)
{
	int j;

	if( mNowLight->shadow_sample_range <= 4.0 )
	{
		for(j=start; j<end; j++)	//라이트테이블로 좌좍 계산한다...
		{
			_PATCH *patch = mlf[Lgt_face_table[j]>>16][Lgt_face_table[j]&0x0000ffff].patch;
			if( patch == NULL )	//콤포짓일때는 넘어간다.
				continue;
			GetPatchIntensity(mNowLight,patch,Lgt_face_table[j]);
		}
	}
	else	//쉐도우 블러를 계산한다.
	{
		for(j=start; j<end; j++)	//라이트테이블로 좌좍 계산한다...
		{
			_PATCH *patch = mlf[Lgt_face_table[j]>>16][Lgt_face_table[j]&0x0000ffff].patch;
			if( patch == NULL )	//콤포짓일때는 넘어간다.
				continue;
			GetShadowPatchIntensity(mNowLight,patch,Lgt_face_table[j]);
		}
		for(j=start; j<end; j++)	//라이트테이블로 좌좍 계산한다...
		{
			_PATCH *patch = mlf[Lgt_face_table[j]>>16][Lgt_face_table[j]&0x0000ffff].patch;
			if( patch == NULL )	//콤포짓일때는 넘어간다.
				continue;
			AddBlurShadowPatchIntensity(mNowLight,mNowLight->shadow_sample_range,patch);
			//AddShadowPatchIntensity(mLight->shadow_sample_range,lf[Now_cal_face>>16][Now_cal_face&0x0000ffff].patch);
		}
	}
}

void Rad::ThreadLightCalculate(void)
{
	int i,j;
	_LFACE **lf=GetLFace();
	mlf = lf;
	int CPUNum = GetIniFile()->CPUNum;
	DWORD threadid[MAX_THREADS];

//	InitializeCriticalSection (&stCrit);	//크리티컬 섹션..
		//---thread lock
//		EnterCriticalSection (&stCrit);
		//---thread unlock
//		LeaveCriticalSection (&stCrit);
//	DeleteCriticalSection (&stCrit);
	for( i=0; i<mLight->light_num; i++)
	{
		if(	mLight[i].is_calculated == FALSE )	//쓰레드를 위한 계산...
			mLight[i].is_calculated=TRUE;
		else
			continue;

		PutR3BspInfoText("Now Lighting  %4d/%4d\n",i+1,mLight->light_num);
		GetLightAttFaceTable(&mLight[i]);	//라이트에 접근된 테이블
		
		mNowLight=&mLight[i];
		int start=0;

		if( CPUNum == 1 )
		{
			ThreadCalculateIntensity(start,Lgt_face_table_cnt);
		}
		else
		{
			for( j=0; j<CPUNum; j++)	//쓰레드 플래그 
				stIsOutThread[j]=1;

			int ttemp=Lgt_face_table_cnt/625;
			ttemp = max(CPUNum,ttemp);
			ttemp=Lgt_face_table_cnt/ttemp;
			start=0;

			int sleep_cnt=100;
			while( 1 )
			{
				for( j=0; j<CPUNum; j++)
				{
					if( stIsOutThread[j] == 1 )
					{
						stIsOutThread[j]=0;
						stThread[j].rad = this;
						stThread[j].start = start;
						stThread[j].end = start+ttemp;
						stThread[j].end = min( stThread[j].end, Lgt_face_table_cnt );
						stThread[j].id = j;
						CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadFunc,&stThread[j],0, &threadid[j]);
						start += ttemp;
						sleep_cnt=100;
						if( stThread[j].end == Lgt_face_table_cnt )
						{
							goto END_RENDER;
						}
					}
				}
				Sleep(sleep_cnt);
				sleep_cnt=2*sleep_cnt;
				sleep_cnt=min(sleep_cnt,30000);	//최대 30초..
			}
END_RENDER:
			//ThreadCalculateIntensity(start,Lgt_face_table_cnt);
			WaitThreadEnd();	//쓰레드가 끝날때까정 기둘린다.	
		}
	}
}

void FixPatchWindingPos(_POLYFACE *face,_PATCH *patch,_FACE_UTIL *f_util)
{
	int i,j,k;

	Vector3fp m_point= f_util->m_point;
	float front,back;

	for(i=0; i<patch->yl; i++)
	{
		for(j=0; j<patch->xl; j++)
		{
			for(k=0; k<face->vertex_num; k++)
			{
				patch->winding[i*patch->xl+j].is_out_face=FALSE;
				Vector4fp edge_n= f_util->edge_normal[k];
				Vector3fp p = patch->winding[i*patch->xl+j].pos;
				back = edge_n[0]*p[0]+edge_n[1]*p[1]+edge_n[2]*p[2]-edge_n[3];
				if( back < 0 )	//와인딩이 삼각형의 바깥에있다.
				{
					front = edge_n[0]*m_point[0]+edge_n[1]*m_point[1]+edge_n[2]*m_point[2]-edge_n[3] -1.0f;//약간의 오차문제해결
					front = front / (front-back);
				
					p[0] = m_point[0] + front*(p[0]-m_point[0]);
					p[1] = m_point[1] + front*(p[1]-m_point[1]);
					p[2] = m_point[2] + front*(p[2]-m_point[2]);
					break;
				}
			}
		}
	}
}

void GetLightRadiosity(_MESH *mesh,_MATERIAL *material, _LIGHTSOURCE *Lgt_mesh )
{
	int i,k,all_face_num;

	if( Lgt_mesh == NULL )
		return;

	_LFACE **lf=GetLFace();
	if( lf==NULL )
		Error("먼저 팻치,와인딩을 설정해라.","");

//-------------쓰레드에 들어가기전 준비...
	Futil = (_FACE_UTIL **)Dmalloc(sizeof(_FACE_UTIL*)*mesh->objectnum);
	all_face_num=0;
	for(k=0; k<mesh->objectnum; k++)
	{
		all_face_num+=mesh->object[k].facenum;
		Futil[k] = (_FACE_UTIL *)Dmalloc(sizeof(_FACE_UTIL)*mesh->object[k].facenum);
	}
	//---가운데 중심점과 에지노멀을 만든다.
	GetPreCalculateMiddlePointDistAndEdgeNormal(mesh);
	for( i=0; i<Lgt_mesh->light_num; i++)
	{
		Lgt_mesh->is_calculated = FALSE;
	}
	
	//------ 면구역을 넘어가는 패치의 위치를 수정한다.
	int max_windind_length = 0;
	for(k=0; k<mesh->objectnum; k++)
	{
		for( i=0; i<mesh->object[k].facenum; i++)
		{
			FixPatchWindingPos(&mesh->object[k].p_face[i],lf[k][i].patch,&Futil[k][i]);
			if( max_windind_length < lf[k][i].patch->xl*lf[k][i].patch->yl)
				max_windind_length=lf[k][i].patch->xl*lf[k][i].patch->yl;
		}
	}


//-------------------------- 쓰레드쓸라고 클래스를 만들었는데 젠장 불필요하네..
	Rad *rad;	

	rad = new Rad(mesh,material,Lgt_mesh,all_face_num,max_windind_length);

	rad->ThreadLightCalculate();

	delete rad;

	for(k=0; k<mesh->objectnum; k++)
		Dfree(Futil[k]);
	Dfree(Futil);
}

int GetFaceLight(_MESH *mesh,_LIGHTSOURCE *lgt)
{
	
	int emit_j=0;
	/*
	int i,j,emit_i,emit_j;

	_LFACE *lf=GetLFace();
	if( lf==NULL )
		Error("먼저 팻치,와인딩을 설정해라.","");

//--unshot 클리어.
	float max_ff=0.0f;
	for(j=0; j<mesh->object[k].facenum; j++)	//지금은 몽땅 계산하지만 라이트테이블을 만들거다...
	{
		for(i=0; i<lf[j].patch->xl*lf[j].patch->yl; i++)
		{
			if( lf[j].patch->winding[i].is_out_face == 1)
				continue;
			if( lf[j].patch->winding[i].unshot[0]+lf[j].patch->winding[i].unshot[1]+lf[j].patch->winding[i].unshot[2] > max_ff)	//방사할 와인딩을 찾는다.
			{
				max_ff = lf[j].patch->winding[i].unshot[0]+lf[j].patch->winding[i].unshot[1]+lf[j].patch->winding[i].unshot[2];
				emit_i=i;
				emit_j=j;
			}
		}
	}
	if( max_ff != 0)
	{
		lgt->type = _SPOT;
		lgt->attn_faron=1;
		lgt->attnstart=0;
		lgt->attnend = ((lf[emit_j].patch->reflectance[0] + lf[emit_j].patch->reflectance[1] + lf[emit_j].patch->reflectance[2])/3.0f)
					*lf[emit_j].patch->sqare*max_ff;	//거리를 적당히 조절

		lgt->pos[0] = lf[emit_j].patch->winding[emit_i].pos[0];
		lgt->pos[1] = lf[emit_j].patch->winding[emit_i].pos[1];
		lgt->pos[2] = lf[emit_j].patch->winding[emit_i].pos[2];

		// 타겟 좌표를 구한다.
		lgt->tpos[0] = lgt->attnend*lf[emit_j].patch->fnormal[0];
		lgt->tpos[1] = lgt->attnend*lf[emit_j].patch->fnormal[1];
		lgt->tpos[2] = lgt->attnend*lf[emit_j].patch->fnormal[2];
		
		lgt->tpos[0] += lgt->pos[0];		// 타겟 좌표를 구한다.
		lgt->tpos[1] += lgt->pos[1];
		lgt->tpos[2] += lgt->pos[2];
	
		lgt->intens=1.0f;

		lgt->reflection[0]=lf[emit_j].patch->reflectance[0];
		lgt->reflection[1]=lf[emit_j].patch->reflectance[1];
		lgt->reflection[2]=lf[emit_j].patch->reflectance[2];

		float square = lf[emit_j].patch->sqare/6.0f;	//적당한 크기의 계수로 나눈다.
//		float square = 50;
		lgt->rgb[0]=lf[emit_j].patch->winding[emit_i].unshot[0]/square;
		lgt->rgb[1]=lf[emit_j].patch->winding[emit_i].unshot[1]/square;
		lgt->rgb[2]=lf[emit_j].patch->winding[emit_i].unshot[2]/square;
		lgt->hotspot=178;	//180도 짜리를 만들자.
		lgt->falloff=178;

		lf[emit_j].patch->winding[emit_i].ff=0;
		lf[emit_j].patch->winding[emit_i].unshot[0]=0;
		lf[emit_j].patch->winding[emit_i].unshot[1]=0;
		lf[emit_j].patch->winding[emit_i].unshot[2]=0;
	}
	else
		return -1;
		*/
	return emit_j;
}


BOOL CalculateFaceRadiosity(_MESH *mesh,_MATERIAL *material)
{
/*
	int j;
	_LFACE *lf=GetLFace();
	if( lf==NULL )
		Error("먼저 팻치,와인딩을 설정해라.","");

	_INIFILE *Ini=GetIniFile();
	_LIGHTSOURCE lgt_sour;
	int ret=GetFaceLight(mesh,&lgt_sour);
	if(ret == -1)	//더이상 방사할것이 없다.
		return FALSE;
	float convergence=(lgt_sour.rgb[0]+lgt_sour.rgb[1]+lgt_sour.rgb[2])/Ini->EnergyLimit;
	PutR3BspInfoText("Now convergence %3f\n",convergence);
	if(convergence<0.0001)
		return FALSE;

	for(j=0; j<mesh->object[k].facenum; j++)	//지금은 몽땅 계산하지만 라이트테이블을 만들거다...
	{
		if(ret == j )	//자기 소속은 뺀다.
			continue;
		GetPatchIntensity(&lgt_sour,material,lf[j].patch);
	}
	*/
	return TRUE;
}

float GetEnergyLimit( _LIGHTSOURCE *lgt_mesh )	//에너지 리미트를 구한다.
{
	int i;
	float hop=0;

	for( i=0; i<lgt_mesh->light_num; i++)
	{
		hop += lgt_mesh[i].rgb[0];
		hop += lgt_mesh[i].rgb[1];
		hop += lgt_mesh[i].rgb[2];
	}
/*
	_LFACE *lf=GetLFace();
	if( lf==NULL )
		Error("먼저 팻치,와인딩을 설정해라.","");
	for(j=0;j<mesh->object[k].facenum;j++)
		lf[j].option=0;
	for(j=0; j<mesh->object[k].facenum; j++)	//지금은 몽땅 계산하지만 라이트테이블을 만들거다...
	{
		if(lf[j].option==1)
			continue;
		lf[j].option=1;
		if(lf[j].friend_num>=0)	//이웃 폴리곤이 있다면
			lf[lf[j].friend_num].option=1;
		float cnt=0;
		for( i=0; i<lf[j].patch[0]l*lf[j].patch[1]l ;i++)
		{
			if(lf[j].patch->winding[i].is_out_face==0)
				cnt++;
		}
		hop+=lf[j].patch->emission.r;
		hop+=lf[j].patch->emission.g;
		hop+=lf[j].patch->emission.b;
	}*/
	return hop;
}

