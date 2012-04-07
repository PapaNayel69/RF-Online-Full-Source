#include "world.h"
#include "JError.h"
#include "R3BspUtil.h"
#include "jmalloc.h"
#include "d3dmath.h"
#include <stdio.h>


int GetCountMaterial(_MATERIAL *material)
{
	int matnum=0;
	if(material->type == _MAT_TYPE_COMPOSITE)
	{
		matnum++;	//라이트 맵이 하나존재하고.
		if(material->submaterial->sbnum==0)
			matnum++;
		else
			matnum+=material->submaterial->sbnum;
	}
	else
	{
        if(material->sbnum==0)
            matnum++;
        else
            matnum+=material->sbnum;
	}
	return matnum;
}


int GetMaterialID(_OBJECT *obj,_MATERIAL *material,int mtl_id)
{
	int id=-1;
	int k;
	int matnum;

    if(obj->material_ref !=-1) // 메인 머터리얼이 존재
    {
        if( material[obj->material_ref].tex!=NULL ) // 메인머터리얼에 텍스쳐가있슴
        {
            matnum=0;
			for(k=0;k<obj->material_ref;k++)
		        matnum+=GetCountMaterial(&material[k]);
			id=matnum;
        }
        else
        {
			if(material[obj->material_ref].type == _MAT_TYPE_COMPOSITE)
			{
				if(material[obj->material_ref].submaterial->sbnum==0)
				{
					matnum=0;
					for(k=0;k<obj->material_ref;k++)
						matnum+=GetCountMaterial(&material[k]);
					id=matnum+1;
				}
				else
				{
//					if( material[obj->material_ref].submaterial->submaterial[mtl_id%material[obj->material_ref].submaterial->sbnum].tex!=NULL)
					if( material[obj->material_ref].submaterial->submaterial[mtl_id%material[obj->material_ref].submaterial->sbnum].tex!=NULL)
					{
						matnum=1;
						for(k=0;k<obj->material_ref;k++)
							matnum+=GetCountMaterial(&material[k]);
						id=matnum+(mtl_id%material[obj->material_ref].submaterial->sbnum);
					}
				}
			}
			else
			{
				if(material[obj->material_ref].sbnum!=0)
				{
	//		if(obj->face[j].mtlid<mesh->material[obj->material_ref].sbnum
					if( material[obj->material_ref].submaterial[mtl_id%material[obj->material_ref].sbnum].tex!=NULL)
					{
						matnum=0;
						for(k=0;k<obj->material_ref;k++)
					        matnum+=GetCountMaterial(&material[k]);
						id=matnum+(mtl_id%material[obj->material_ref].sbnum);
					}
					else
						id=-1;
				}
				else
					id=-1;
			}
        }
    }
	else
	{
		id=-1;
	}
	return id;
}

void SetWorldMesh(_MESH *mesh,_MATERIAL *material)
{
	int i,j;
	int all_f_num=0;

    for(i=0;i<mesh->objectnum;i++)
    {
		all_f_num+=mesh->object[i].facenum;
		mesh->object[i].bb_min[0] = 1000000;		
		mesh->object[i].bb_min[1] = 1000000;
		mesh->object[i].bb_min[2] = 1000000;
		mesh->object[i].bb_max[0] = -1000000;	
		mesh->object[i].bb_max[1] = -1000000;	
		mesh->object[i].bb_max[2] = -1000000;

		for( j=0; j<mesh->object[i].vertexnum; j++ )
		{
			D3DVECTOR a;
			D3DMath_VectorMatrixMultiply( 
				a
//				*((D3DVECTOR*)&w_mesh->object[i].vertex[j].local)
				,*((D3DVECTOR*)&mesh->object[i].vertex[j].local)
				,mesh->object[i].d3d_trmatrix);
			mesh->object[i].vertex[j].local[0] = a.x;
			mesh->object[i].vertex[j].local[1] = a.y;
			mesh->object[i].vertex[j].local[2] = a.z;

			//바운딩 박스 생성.
			mesh->object[i].bb_min[0]=min(mesh->object[i].bb_min[0],a.x);
			mesh->object[i].bb_max[0]=max(mesh->object[i].bb_max[0],a.x);
			mesh->object[i].bb_min[1]=min(mesh->object[i].bb_min[1],a.y);
			mesh->object[i].bb_max[1]=max(mesh->object[i].bb_max[1],a.y);
			mesh->object[i].bb_min[2]=min(mesh->object[i].bb_min[2],a.z);
			mesh->object[i].bb_max[2]=max(mesh->object[i].bb_max[2],a.z);
		}
	    for(j=0; j<mesh->object[i].facenum; j++)
		{
			if( mesh->object[i].tvertex )
			{
				mesh->object[i].face[j].fu[0] = mesh->object[i].tvertex[mesh->object[i].face[j].tv[0]].uv[0];
				mesh->object[i].face[j].fv[0] = mesh->object[i].tvertex[mesh->object[i].face[j].tv[0]].uv[1];
				mesh->object[i].face[j].fu[1] = mesh->object[i].tvertex[mesh->object[i].face[j].tv[1]].uv[0];
				mesh->object[i].face[j].fv[1] = mesh->object[i].tvertex[mesh->object[i].face[j].tv[1]].uv[1];
				mesh->object[i].face[j].fu[2] = mesh->object[i].tvertex[mesh->object[i].face[j].tv[2]].uv[0];
				mesh->object[i].face[j].fv[2] = mesh->object[i].tvertex[mesh->object[i].face[j].tv[2]].uv[1];
			}
			//채널 2번째 좌표를 가지고있으면...	지금은 수동 라이트맵을 사용한다.
			if( mesh->object[i].tvertex_ch2 )
			{
				mesh->object[i].face[j].lu[0] = mesh->object[i].tvertex_ch2[mesh->object[i].face[j].tv2[0]].uv[0];
				mesh->object[i].face[j].lv[0] = mesh->object[i].tvertex_ch2[mesh->object[i].face[j].tv2[0]].uv[1];
				mesh->object[i].face[j].lu[1] = mesh->object[i].tvertex_ch2[mesh->object[i].face[j].tv2[1]].uv[0];
				mesh->object[i].face[j].lv[1] = mesh->object[i].tvertex_ch2[mesh->object[i].face[j].tv2[1]].uv[1];
				mesh->object[i].face[j].lu[2] = mesh->object[i].tvertex_ch2[mesh->object[i].face[j].tv2[2]].uv[0];
				mesh->object[i].face[j].lv[2] = mesh->object[i].tvertex_ch2[mesh->object[i].face[j].tv2[2]].uv[1];
			}

			mesh->object[i].face[j].mtlid= GetMaterialID(&mesh->object[i],material,mesh->object[i].face[j].mtlid);

		}
	}
}

static DWORD GetIndexFromCompositeMat(_MATERIAL *mat,int mat_num)
{
	int i;

	DWORD id=0;
	for(i=0; i<mat->all_num; i++)
	{
		if( mat_num == i )
			break;
		if( mat[i].type == _MAT_TYPE_COMPOSITE)
			id++;
	}
	return id;
}

void SetMeshCompositeMat(_MESH *mesh, _MATERIAL *org_mat)	//Composite오브젝트의 경우. 총 개수와 오브젝트에 속성을 넣어준다.
{
	int i;

    for(i=0;i<mesh->objectnum;i++)
    {
		if( mesh->object[i].material_ref != -1 )
		{
			if( org_mat[mesh->object[i].material_ref].type == _MAT_TYPE_COMPOSITE )
			{
				//콤포짓 라이트맵이다. 
				mesh->object[i].type |= _OBJECT_TYPE_COMPOSITE;
				mesh->object[i].composite_id = 	GetIndexFromCompositeMat(org_mat,mesh->object[i].material_ref);
			}
		}
	}
}

void GetCompositeMat(_MATERIAL *one_mat,_MATERIAL **composite_mat)	// 머터리얼에서 composite머터리얼과 그냥 머터리얼을  추출해낸다...
{
	int i;

	DWORD num=0;
	for(i=0; i<one_mat->all_num; i++)
	{
		if( one_mat[i].type == _MAT_TYPE_COMPOSITE)
			num++;
	}

	_MATERIAL *com_mat;
	com_mat = (_MATERIAL *)Dmalloc(sizeof(_MATERIAL)*num );
	memset(com_mat,0,sizeof(_MATERIAL)*num );
	if( com_mat )
		com_mat->all_num = num;

	num=0;
	for(i=0; i<one_mat->all_num; i++)
	{
		if( one_mat[i].type == _MAT_TYPE_COMPOSITE)
		{
			memcpy(&com_mat[num],&one_mat[i],sizeof(_MATERIAL));
			num++;
		}
	}
	if( com_mat )
		com_mat->all_num = num;
//	Dfree(one_mat);	//뭉쳐진 머터리얼 분리...
	*composite_mat = com_mat;	
}

_MESH *ConvOneMesh(_MESH *mesh,_MATERIAL *material)	//하나의 오브젝트로 컨버트
{
	_MESH *o_mesh;
	int i,j;

	o_mesh = (_MESH *)Dmalloc(sizeof(_MESH));
	memset(o_mesh,0,sizeof(_MESH));
	o_mesh->objectnum=1;

	o_mesh->object=(_OBJECT*)Dmalloc(sizeof(_OBJECT));	
	memset(o_mesh->object,0,sizeof(_OBJECT));	

    for( i=0;i<mesh->objectnum;i++)
    {
		o_mesh->object->vertexnum += mesh->object[i].vertexnum;
	}
    for( i=0;i<mesh->objectnum;i++)
    {
		o_mesh->object->facenum += mesh->object[i].facenum;
	}
	o_mesh->object->face=(_FACE*)Dmalloc(sizeof(_FACE)*o_mesh->object->facenum);	//면과 점을 할당.
	o_mesh->object->vertex=(_VERTEX*)Dmalloc(sizeof(_VERTEX)*o_mesh->object->vertexnum);	//면과 점을 할당.

	int face=0,vertex=0;
    for(i=0;i<mesh->objectnum;i++)
    {
		memcpy(&o_mesh->object->vertex[vertex],&mesh->object[i].vertex[0]
			,sizeof(_VERTEX)*mesh->object[i].vertexnum);

		memcpy(&o_mesh->object->face[face],&mesh->object[i].face[0]
			,sizeof(_FACE)*mesh->object[i].facenum);
	    for(j=0; j<mesh->object[i].facenum; j++)
		{
			o_mesh->object->face[face+j].v[0]+=vertex;	//vertex 인덱스 숫자를 증가시킨다.
			o_mesh->object->face[face+j].v[1]+=vertex;
			o_mesh->object->face[face+j].v[2]+=vertex;
			o_mesh->object->face[face+j].smooth_group = mesh->object[i].face[j].smooth_group;

//			if( mesh->object[i].tvertex )
			{
				o_mesh->object->face[face+j].fu[0] = mesh->object[i].face[j].fu[0];
				o_mesh->object->face[face+j].fv[0] = mesh->object[i].face[j].fv[0];
				o_mesh->object->face[face+j].fu[1] = mesh->object[i].face[j].fu[1];
				o_mesh->object->face[face+j].fv[1] = mesh->object[i].face[j].fv[1];
				o_mesh->object->face[face+j].fu[2] = mesh->object[i].face[j].fu[2];
				o_mesh->object->face[face+j].fv[2] = mesh->object[i].face[j].fv[2];
			}
			//쓰파 죽는줄 알았다... 이거 해결하느라고...
            o_mesh->object->face[face+j].mtlid=mesh->object[i].face[j].mtlid;
//            o_mesh->object->face[face+j].mtlid=GetMaterialID(&mesh->object[i],material,mesh->object[i].face[j].mtlid);
			/*
            o_mesh->object->face[face+j].mtlid=-1;
            if(mesh->object[i].material_ref !=-1) // 메인 머터리얼이 존재
            {
                if( material[mesh->object[i].material_ref].tex!=NULL ) // 메인머터리얼에 텍스쳐가있슴
                {
                    matnum=0;
                    for(k=0;k<mesh->object[i].material_ref;k++)
                    {
                        if(material[k].sbnum==0)
                            matnum++;
                        else
                            matnum+=material[k].sbnum;
                    }
                    o_mesh->object->face[face+j].mtlid=matnum;
                }
                else
                {
                    if(material[mesh->object[i].material_ref].sbnum!=0)
                    {
//                        if(mesh->object[i].face[j].mtlid<mesh->material[mesh->object[i].material_ref].sbnum
                        if( material[mesh->object[i].material_ref].submaterial[mesh->object[i].face[j].mtlid%material[mesh->object[i].material_ref].sbnum].tex!=NULL)
                        {
                            matnum=0;
                            for(k=0;k<mesh->object[i].material_ref;k++)
                            {
                                if(material[k].sbnum==0)
                                    matnum++;
                                else
                                    matnum+=material[k].sbnum;
                            }
                            o_mesh->object->face[face+j].mtlid=matnum+(mesh->object[i].face[j].mtlid%material[mesh->object[i].material_ref].sbnum);
                        }
                    }
                }
            }*/
		}
		vertex+=mesh->object[i].vertexnum;
		face+=mesh->object[i].facenum;
	}
	return o_mesh;
}


_MATERIAL *ConvOneMaterial(_MATERIAL *material)
{
	int allmat,matnum,i,j;
	_MATERIAL *r_material=NULL;

	if( material == NULL )
		return NULL;
    allmat=0;
    for(i=0;i<material->all_num;i++)
    {
		if(material[i].type == _MAT_TYPE_COMPOSITE )	//컴퍼짓 머터리얼을 해결하자..
		{
	        allmat++;
			_MATERIAL *temp_mat = &material[i].submaterial[0];
			if(temp_mat->sbnum)
				allmat+=temp_mat->sbnum;
			else
				allmat++;
		}
		else
        if(material[i].sbnum==0)
            allmat++;
        else
            allmat+=material[i].sbnum;
    }
    r_material=(_MATERIAL*)Dmalloc(sizeof(_MATERIAL)*allmat);
    memset(r_material,0,sizeof(_MATERIAL)*allmat);
    
//	_MATERIAL *imsi_material=(_MATERIAL*)Dmalloc(sizeof(_MATERIAL)*allmat);
  //  memset(imsi_material,0,sizeof(_MATERIAL)*allmat);
    matnum=0;
//---------------------------- 통자 머터리얼로...
    for(i=0;i<material->all_num;i++)
    {
		r_material[matnum].sbnum=0;
        if(material[i].sbnum==0)
        {
            memcpy(&r_material[matnum],&material[i],sizeof(_MATERIAL));
            matnum++;
        }
        else
        {
			if(material[i].type == _MAT_TYPE_COMPOSITE )	//컴퍼짓 머터리얼을 해결하자..
			{
                memcpy(&r_material[matnum],&material[i].submaterial[1],sizeof(_MATERIAL));
				r_material[matnum].type = _MAT_TYPE_COMPOSITE;
	            matnum++;
				_MATERIAL *temp_mat = &material[i].submaterial[0];
				if(temp_mat->sbnum)
				{
					for(j=0; j<temp_mat->sbnum; j++)
					{
						memcpy(&r_material[matnum+j],&temp_mat->submaterial[j],sizeof(_MATERIAL));
						r_material[matnum+j].sbnum=0;
					}
					matnum+=temp_mat->sbnum;
				}else
				{
					memcpy(&r_material[matnum],&material[i].submaterial[0],sizeof(_MATERIAL));
					r_material[matnum].sbnum=0;
					matnum++;
				}
			}
			else
			{
				for(j=0; j<material[i].sbnum; j++)
				{
					memcpy(&r_material[matnum+j],&material[i].submaterial[j],sizeof(_MATERIAL));
					r_material[matnum+j].sbnum=0;
				}
				matnum+=material[i].sbnum;
			}
        }
    }
	if( allmat!=matnum )
		Error("One 머터리얼 생성에러","");
	r_material->all_num = matnum;
	return r_material;
}

void AdjustMeshMaterial(_MESH *mesh,_MATERIAL *material)
{
	int i,j,k,matnum;

	if(material==NULL)
		return;
	matnum = material->all_num;
	_MATERIAL *imsi_material=(_MATERIAL*)Dmalloc(sizeof(_MATERIAL)*matnum);
    memcpy(imsi_material,material,sizeof(_MATERIAL)*matnum);

	int real_mat=0;

/*
//---------------------------- 같은 텍스쳐를 가지고 있는것들 정리 
    for(i=0;i<matnum;i++)
	{
		imsi_material[i].same_mat=-1;
	}
	int real_mat=0;
   
	for(i=0;i<matnum;i++)
    {
		int is_exist=-1;

		for(j=0;j<real_mat;j++)
		{
			if( material[j].tex->mapname == NULL || imsi_material[i].tex->mapname == NULL)
				continue;
			if( !strcmp(imsi_material[i].tex->mapname,material[j].tex->mapname))
			{
				is_exist=j;
				break;
			}
		}
		if(is_exist!=-1)
		{
			imsi_material[i].same_mat=is_exist;
		}
		else
		{
			imsi_material[i].same_mat=real_mat;
			memcpy( &material[real_mat],&imsi_material[i],sizeof(_MATERIAL) );
			real_mat++;
		}
	}
	material->all_num=real_mat;
//--------------------- 같은 텍스쳐를 가지고있는 머터리얼  id를 정리한다.
	for(i=0;i<obj->facenum;i++)
	{
		if( obj->p_face[i].mtlid == -1 )
			continue;
		if( imsi_material[obj->p_face[i].mtlid].same_mat ==-1 )
			continue;
		obj->p_face[i].mtlid=imsi_material[obj->p_face[i].mtlid].same_mat;
	}
*/
//--------------------- 사용하지않는 텍스쳐를 가지고 있는 머터리얼  id를 정리한다.
    for(i=0;i<material->all_num;i++)
    {
		material[i].is_used=FALSE;
	}

	for(k=0; k<mesh->objectnum; k++)
	{
		_OBJECT *obj = &mesh->object[k];
		for(i=0;i<obj->facenum;i++)
		{
			if(obj->p_face[i].mtlid >=0 && obj->p_face[i].mtlid < material->all_num)
				material[obj->p_face[i].mtlid].is_used=TRUE;
		}
	}
	real_mat=material->all_num;
    for(i=0; i<real_mat; i++)
    {
		if(material[i].is_used==FALSE)
		{
			if(i==real_mat-1)
				break;
			memcpy(&material[i],&material[i+1],sizeof(_MATERIAL)*(real_mat-1-i));
			for(k=0; k<mesh->objectnum; k++)
			{
				_OBJECT *obj = &mesh->object[k];
				for(j=0;j<obj->facenum;j++)
				{
					if( obj->p_face[j].mtlid >=i )
						obj->p_face[j].mtlid--;
				}
			}
			i--;
			real_mat--;
			continue;
		}
	}
	material->all_num=real_mat;
}
#define _EPSILON	0.001f

//-----------------------------
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

	vv[0] = b[0] - a[0];
	vv[1] = b[1] - a[1];
	vv[2] = b[2] - a[2];
    CrossVector(fv, vv, n);
    dp = (n[0] * ( v[0] - b[0])	+ n[1] * ( v[1] - b[1])
		+ n[2] * ( v[2] - b[2]));

//#define N_EPSILON 1/3.0		//폴리곤 정확성 검사..
    if(  dp > _EPSILON)
        return 1;
	return 0;	//  통과 
}

//#define _EPSILON	0.1/3.0
int GetEqualFaceCount(_FACE *cf,_FACE *f)
{
	int cnt=0,i,j;

	//----개같은 경우 처리..
	for( i=0; i<3; i++ )
	{
		for( j=0; j<3; j++ )
		{
			if( cf->v[i] == f->v[j] )
				cnt++;
		}
	}
	if( cnt == 3 )
		return 0;

	cnt=0;
	if(cf->v[0]==f->v[0] && cf->fu[0] == f->fu[0] && cf->fv[0] == f->fv[0] 
		&& cf->lu[0] == f->lu[0] && cf->lv[0] == f->lv[0])
		cnt++;
	if(cf->v[0]==f->v[1] && cf->fu[0] == f->fu[1] && cf->fv[0] == f->fv[1] 
		&& cf->lu[0] == f->lu[1] && cf->lv[0] == f->lv[1])
		cnt++;
	if(cf->v[0]==f->v[2] && cf->fu[0] == f->fu[2] && cf->fv[0] == f->fv[2] 
		&& cf->lu[0] == f->lu[2] && cf->lv[0] == f->lv[2])
		cnt++;
	if(cf->v[1]==f->v[0] && cf->fu[1] == f->fu[0] && cf->fv[1] == f->fv[0] 
		&& cf->lu[1] == f->lu[0] && cf->lv[1] == f->lv[0])
		cnt++;
	if(cf->v[1]==f->v[1] && cf->fu[1] == f->fu[1] && cf->fv[1] == f->fv[1] 
		&& cf->lu[1] == f->lu[1] && cf->lv[1] == f->lv[1])
		cnt++;
	if(cf->v[1]==f->v[2] && cf->fu[1] == f->fu[2] && cf->fv[1] == f->fv[2] 
		&& cf->lu[1] == f->lu[2] && cf->lv[1] == f->lv[2])
		cnt++;
	if(cf->v[2]==f->v[0] && cf->fu[2] == f->fu[0] && cf->fv[2] == f->fv[0] 
		&& cf->lu[2] == f->lu[0] && cf->lv[2] == f->lv[0])
		cnt++;
	if(cf->v[2]==f->v[1] && cf->fu[2] == f->fu[1] && cf->fv[2] == f->fv[1] 
		&& cf->lu[2] == f->lu[1] && cf->lv[2] == f->lv[1])
		cnt++;
	if(cf->v[2]==f->v[2] && cf->fu[2] == f->fu[2] && cf->fv[2] == f->fv[2] 
		&& cf->lu[2] == f->lu[2] && cf->lv[2] == f->lv[2])
		cnt++;
	if(cnt ==3)
	{
		return 0;
		//Error("same polys");
	}
	return cnt;
}

int WhatAddVertex(LONG ev[],LONG sv[])	//어떤놈을 넣을까?
{
	if( sv[0] == ev[0] ||  sv[0] == ev[1] || sv[0] == ev[2] )
	{
	}
	else
		return 0;
	if( sv[1] == ev[0] || sv[1] == ev[1] || sv[1] == ev[2] )
	{
	}
	else
		return 1;
	if( sv[2] == ev[0] || sv[2] == ev[1] || sv[2] == ev[2] )
	{
	}
	else
		return 2;

	Error("이럴수가!","");
	return -1;
}
BOOL IsSameVertex(_VERTEX *s,_VERTEX *e)
{
	if( fabs(s->x - e->x) < _EPSILON && fabs(s->y - e->y) < _EPSILON && fabs(s->z - e->z) < _EPSILON )
		return TRUE;
	else
		return FALSE;
}

BOOL IsInsertVertex(_FACE *f,_VERTEX *s,_VERTEX *e,_VERTEX *sm)	//시작 과 끝 버텍스의 오른쪽에있으면 해당된면 OK
{
	Vector3f vector;

    vector[0] = sm->x;
    vector[1] = sm->y;
    vector[2] = sm->z;

	if( IsSameVertex(s,sm) )
		return FALSE;
	if( IsSameVertex(e,sm) )
		return FALSE;

	if( CheckEdge( s->local,e->local,vector,f->f_normal) == FALSE )
		return TRUE;
	return FALSE;
}

void GetInvScaleMatrix(_MESH *mesh)
{
	int i,j;

	for(i=0; i<mesh->objectnum; i++)
	{
		D3DMATRIX rot_m,inv_rot,temp,invmat;

		if(	mesh->object[i].parentname[0]==NULL )
		{
//			memset(&mesh->object[i].d3d_s_matrix,0,sizeof(D3DMATRIX));
//			mesh->object[i].d3d_s_matrix._11=1;
//			mesh->object[i].d3d_s_matrix._22=1;
//			mesh->object[i].d3d_s_matrix._33=1;
//			mesh->object[i].d3d_s_matrix._44=1;
			memcpy(&temp,&mesh->object[i].d3d_trmatrix,sizeof(D3DMATRIX));
		}
		else	//부모가 있으면.
		{
			for(j=0; j<mesh->objectnum; j++)
			{
				if( !strcmp(mesh->object[j].objectname,mesh->object[i].parentname))
					break;
			}
			if( j == mesh->objectnum )
			{
				char buf[256];
				strcpy(buf,mesh->object[i].parentname);
				buf[5]=NULL;
				if( !strcmp(buf,"Group"))
				{
					memcpy(&temp,&mesh->object[i].d3d_trmatrix,sizeof(D3DMATRIX));
				}
				else
					Error(mesh->object[i].parentname,"<-이 부모가 없음");
			}
			else
			{
				D3DMath_MatrixInvert( invmat, mesh->object[j].d3d_trmatrix );
				D3DMath_MatrixMultiply( temp,invmat, mesh->object[i].d3d_trmatrix );
			}
		}
		D3DMath_MatrixFromQuaternion(rot_m,mesh->object[i].quat[0],mesh->object[i].quat[1],mesh->object[i].quat[2],mesh->object[i].quat[3]);

		D3DMath_MatrixInvert( inv_rot, rot_m );
		D3DMath_MatrixMultiply( mesh->object[i].d3d_s_matrix ,inv_rot,temp );	//스케일 로컬 매트릭스
		mesh->object[i].d3d_s_matrix._41=0;
		mesh->object[i].d3d_s_matrix._42=0;
		mesh->object[i].d3d_s_matrix._43=0;
		mesh->object[i].d3d_s_matrix._44=1;
	}
}

DWORD GetFaceNumFromMesh(_MESH *mesh)
{
	DWORD face_num=0;
	for(int i=0; i<mesh->objectnum; i++)
	{
		face_num+=mesh->object[i].facenum;
	}
	return face_num;
}

void SetTempTexUV(int *t_v,float *t_fu,float *t_fv,_FACE *f,_FACE *f_face,int id_0,int id_w_num,int id_1,int id_2)
{
	t_v[0]=f->v[id_0];
	t_v[1]=f_face->v[id_w_num];
	t_v[2]=f->v[id_1];
	t_v[3]=f->v[id_2];
	
	t_fu[0] = f->fu[id_0];
	t_fu[1] = f_face->fu[id_w_num];
	t_fu[2] = f->fu[id_1];
	t_fu[3] = f->fu[id_2];

	t_fv[0] = f->fv[id_0];
	t_fv[1] = f_face->fv[id_w_num];
	t_fv[2] = f->fv[id_1];
	t_fv[3] = f->fv[id_2];
}
//	SetTempLgtUV(t_v,t_fu,t_fv,f,f_face,0,w_num,1,2);
void SetTempLgtUV(int *l_v,float *l_fu,float *l_fv,_FACE *f,_FACE *f_face,int id_0,int id_w_num,int id_1,int id_2)
{
	l_v[0]=f->v[id_0];
	l_v[1]=f_face->v[id_w_num];
	l_v[2]=f->v[id_1];
	l_v[3]=f->v[id_2];
	
	l_fu[0] = f->lu[id_0];
	l_fu[1] = f_face->lu[id_w_num];
	l_fu[2] = f->lu[id_1];
	l_fu[3] = f->lu[id_2];

	l_fv[0] = f->lv[id_0];
	l_fv[1] = f_face->lv[id_w_num];
	l_fv[2] = f->lv[id_1];
	l_fv[3] = f->lv[id_2];
}



_MESH *ConvMeshTriToPoly(_MESH *mesh)	//오브젝트의 삼각형 메쉬를 폴리곤 메쉬로 변환
{
	int i,j,k;

	//앞으로 여기에서 삼각형을 다각형화하는 작업을 해야겠쥐...
	if(mesh->poly_type != FALSE )	//삼각메쉬가 아니면.
	{
		return NULL;
	}

	_MESH *p_mesh=(_MESH *)Dmalloc(sizeof(_MESH));
	memset(p_mesh,0,sizeof(_MESH));
	p_mesh->poly_type = TRUE;
	p_mesh->object=(_OBJECT *)Dmalloc(sizeof(_OBJECT)*mesh->objectnum);
	memset(p_mesh->object,0,sizeof(_OBJECT)*mesh->objectnum);
	p_mesh->objectnum=mesh->objectnum;
	
	for(i=0; i<mesh->objectnum; i++)
	{
		memcpy(&p_mesh->object[i],&mesh->object[i],sizeof(_OBJECT));
		p_mesh->object[i].type = mesh->object[i].type;
		strcpy(p_mesh->object[i].objectname,mesh->object[i].objectname);
		p_mesh->object[i].vertexnum = mesh->object[i].vertexnum;
		p_mesh->object[i].vertex = (_VERTEX *)Dmalloc(sizeof(_VERTEX)*mesh->object[i].vertexnum);
		memcpy(p_mesh->object[i].vertex,mesh->object[i].vertex,sizeof(_VERTEX)*p_mesh->object[i].vertexnum);
	}

	int max_face_num=0,all_face_num=0;
	for(i=0; i<mesh->objectnum; i++)
	{
		if( max_face_num < mesh->object[i].facenum )
			max_face_num = mesh->object[i].facenum;
		all_face_num += mesh->object[i].facenum;
		p_mesh->object[i].facenum = mesh->object[i].facenum;
		p_mesh->object[i].p_face = (_POLYFACE *)Dmalloc(sizeof(_POLYFACE)*p_mesh->object[i].facenum);
		memset(p_mesh->object[i].p_face,0,sizeof(_POLYFACE)*p_mesh->object[i].facenum);
	}

	int *friend_num = (int *)Dmalloc(sizeof(int)*max_face_num);
	BYTE *is_friend = (BYTE *)Dmalloc(sizeof(BYTE)*max_face_num);

	LONG *v_num=(LONG *)Dmalloc(sizeof(LONG)*all_face_num*3);
	float *fv=(float *)Dmalloc(sizeof(float)*all_face_num*3);
	float *fu=(float *)Dmalloc(sizeof(float)*all_face_num*3);
	float *lv=(float *)Dmalloc(sizeof(float)*all_face_num*3);
	float *lu=(float *)Dmalloc(sizeof(float)*all_face_num*3);
//----------이웃 폴리곤 구하기...

	LONG nu_vnum =0;
	for(k=0; k<mesh->objectnum; k++)
	{
		for(j=0; j<max_face_num; j++)
			friend_num[j] = -1;
		for(i=0; i<mesh->object[k].facenum; i++)
		{
			int best=-1;
			int friend_on=0;

			_FACE *cf=&mesh->object[k].face[i];
			float *cn=mesh->object[k].face[i].f_normal;
			//최대 페이스는 2만개니까....
			int start_j = i-20000;
			int end_j = i+20000;
			start_j=max(0,start_j);
			end_j=min(mesh->object[k].facenum,end_j);

			for(j=start_j;j<end_j;j++)
			{
				double b_leng=999999999999.0,a;

				_VERTEX *v=mesh->object[k].vertex;

				_FACE *f=&mesh->object[k].face[j];
				float *n=mesh->object[k].face[j].f_normal;
				
				if( friend_num[j] ==-1 && friend_num[i] ==-1 && i!=j
					&& mesh->object[k].face[i].mtlid == mesh->object[k].face[j].mtlid)
				{
					int cnt=GetEqualFaceCount(cf,f);
					float b_min[3],b_max[3];

					if(cnt ==2 
					&& ( n[0]-cn[0]>=-_EPSILON && n[0]-cn[0]<=_EPSILON)
					&& ( n[1]-cn[1]>=-_EPSILON && n[1]-cn[1]<=_EPSILON)
					&& ( n[2]-cn[2]>=-_EPSILON && n[2]-cn[2]<=_EPSILON))
					{
						b_min[0]=min(v[cf->v[0]].x,(float)999999.0);
						b_min[0]=min(v[cf->v[1]].x,b_min[0]);
						b_min[0]=min(v[cf->v[2]].x,b_min[0]);
						b_min[1]=min(v[cf->v[0]].y,(float)999999.0);
						b_min[1]=min(v[cf->v[1]].y,b_min[1]);
						b_min[1]=min(v[cf->v[2]].y,b_min[1]);
						b_min[2]=min(v[cf->v[0]].z,(float)999999.0);
						b_min[2]=min(v[cf->v[1]].z,b_min[2]);
						b_min[2]=min(v[cf->v[2]].z,b_min[2]);

						b_max[0]=max(v[cf->v[0]].x,(float)-999999.0);
						b_max[0]=max(v[cf->v[1]].x,b_max[0]);
						b_max[0]=max(v[cf->v[2]].x,b_max[0]);
						b_max[1]=max(v[cf->v[0]].y,(float)-999999.0);
						b_max[1]=max(v[cf->v[1]].y,b_max[1]);
						b_max[1]=max(v[cf->v[2]].y,b_max[1]);
						b_max[2]=max(v[cf->v[0]].z,(float)-999999.0);
						b_max[2]=max(v[cf->v[1]].z,b_max[2]);
						b_max[2]=max(v[cf->v[2]].z,b_max[2]);

						a = sqrt((b_max[2]-b_min[2])*(b_max[2]-b_min[2])*(b_max[2]-b_min[2])*(b_max[2]-b_min[2])
								+(b_max[1]-b_min[1])*(b_max[1]-b_min[1])*(b_max[1]-b_min[1])*(b_max[1]-b_min[1])
								+(b_max[0]-b_min[0])*(b_max[0]-b_min[0])*(b_max[0]-b_min[0])*(b_max[0]-b_min[0]));
						if( b_leng > a )
						{
							int im_cnt=0;
							int w_num=WhatAddVertex(cf->v,f->v);	//어떤놈을 넣을까?
							_VERTEX *w_v=&v[f->v[w_num]];
							if( IsInsertVertex(cf,&v[cf->v[0]],&v[cf->v[1]],w_v))	//시작 과 끝 버텍스의 오른쪽에있으면 해당된면 OK
								im_cnt++;
							if( IsInsertVertex(cf,&v[cf->v[1]],&v[cf->v[2]],w_v))	//시작 과 끝 버텍스의 오른쪽에있으면 해당된면 OK
								im_cnt++;
							if( IsInsertVertex(cf,&v[cf->v[2]],&v[cf->v[0]],w_v))	//시작 과 끝 버텍스의 오른쪽에있으면 해당된면 OK
								im_cnt++;						

							if(im_cnt==1)	//convex 폴리곤만.. 오차가 생길수도 있으니...
							{
								friend_on=1;
								best=j;
								break;
							}
						}
					}
				}
			}
			if( friend_on )
			{
				friend_num[i]=best;			
				friend_num[best]=i;
			}
		}
//-------------------- 폴리곤 페이스를 생성..
		int cnt=0;
		for( i=0; i<mesh->object[k].facenum; i++)
			is_friend[i]=0;

		for(i=0; i<mesh->object[k].facenum; i++)
		{
			LONG vertex_num;
			if( is_friend[i] )
				continue;
			if(friend_num[i] != -1)
			{
				vertex_num=4;
				is_friend[friend_num[i]]=1;
			}
			else
				vertex_num=3;

			p_mesh->object[k].p_face[cnt].vertex_num=vertex_num;

			p_mesh->object[k].p_face[cnt].vertex_id=v_num+nu_vnum;
			p_mesh->object[k].p_face[cnt].fv=fv+nu_vnum;
			p_mesh->object[k].p_face[cnt].fu=fu+nu_vnum;
			p_mesh->object[k].p_face[cnt].lv=lv+nu_vnum;
			p_mesh->object[k].p_face[cnt].lu=lu+nu_vnum;
			p_mesh->object[k].p_face[cnt].v_normal=(Vector3f *)Dmalloc(sizeof(Vector3f)*vertex_num);
			p_mesh->object[k].p_face[cnt].smooth_group = mesh->object[k].face[i].smooth_group;
			p_mesh->object[k].p_face[cnt].mtlid=mesh->object[k].face[i].mtlid;
			p_mesh->object[k].p_face[cnt].temp=0;

			float t_fu[4],t_fv[4];
			int t_v[4];
			float l_fu[4],l_fv[4];
			int l_v[4];
	//		Vector3f v_normal[4];

			_FACE *f=&mesh->object[k].face[i];
			if( vertex_num == 4 )
			{
				int w_num=WhatAddVertex(mesh->object[k].face[i].v,mesh->object[k].face[friend_num[i]].v);	//어떤놈을 넣을까?
				_FACE *f_face= &mesh->object[k].face[friend_num[i]];
				_VERTEX *w_v=&mesh->object[k].vertex[f_face->v[w_num]];

				int is_one=0;
				if( IsInsertVertex(f,&mesh->object[k].vertex[f->v[0]],&mesh->object[k].vertex[f->v[1]],w_v))	//시작 과 끝 버텍스의 오른쪽에있으면 해당된면 OK
				{
					SetTempTexUV(t_v,t_fu,t_fv,f,f_face,0,w_num,1,2);
					SetTempLgtUV(l_v,l_fu,l_fv,f,f_face,0,w_num,1,2);
/*
					t_v[0]=f->v[0];
					t_v[1]=f_face->v[w_num];
					t_v[2]=f->v[1];
					t_v[3]=f->v[2];
					
					t_fu[0] = f->fu[0];
					t_fu[1] = f_face->fu[w_num];
					t_fu[2] = f->fu[1];
					t_fu[3] = f->fu[2];

					t_fv[0] = f->fv[0];
					t_fv[1] = f_face->fv[w_num];
					t_fv[2] = f->fv[1];
					t_fv[3] = f->fv[2];
*/
					is_one++;
				}
				if( IsInsertVertex(f,&mesh->object[k].vertex[f->v[1]],&mesh->object[k].vertex[f->v[2]],w_v))	//시작 과 끝 버텍스의 오른쪽에있으면 해당된면 OK
				{
					SetTempTexUV(t_v,t_fu,t_fv,f,f_face,1,w_num,2,0);
					SetTempLgtUV(l_v,l_fu,l_fv,f,f_face,1,w_num,2,0);
/*
					t_v[0]=f->v[1];
					t_v[1]=f_face->v[w_num];
					t_v[2]=f->v[2];
					t_v[3]=f->v[0];
					
					t_fu[0] = f->fu[1];
					t_fu[1] = f_face->fu[w_num];
					t_fu[2] = f->fu[2];
					t_fu[3] = f->fu[0];

					t_fv[0] = f->fv[1];
					t_fv[1] = f_face->fv[w_num];
					t_fv[2] = f->fv[2];
					t_fv[3] = f->fv[0];
*/
					is_one++;
				}
				if( IsInsertVertex(f,&mesh->object[k].vertex[f->v[2]],&mesh->object[k].vertex[f->v[0]],w_v))	//시작 과 끝 버텍스의 오른쪽에있으면 해당된면 OK
				{
					SetTempTexUV(t_v,t_fu,t_fv,f,f_face,2,w_num,0,1);
					SetTempLgtUV(l_v,l_fu,l_fv,f,f_face,2,w_num,0,1);
/*					t_v[0]=f->v[2];
					t_v[1]=f_face->v[w_num];
					t_v[2]=f->v[0];
					t_v[3]=f->v[1];
					
					t_fu[0] = f->fu[2];
					t_fu[1] = f_face->fu[w_num];
					t_fu[2] = f->fu[0];
					t_fu[3] = f->fu[1];

					t_fv[0] = f->fv[2];
					t_fv[1] = f_face->fv[w_num];
					t_fv[2] = f->fv[0];
					t_fv[3] = f->fv[1];
*/
					is_one++;
				}

				static int Friend_ship=-1;
				if( is_one != 1 )	//현재는 4각폴리곤만 지원.
				{
					if( Friend_ship == i )
					{
						char buf[256];
						sprintf(buf,"이상한데 %d poly가 3각,4각도 아닌 폴리곤이... ",i);
						Warning(buf,"");
					}
					if( Friend_ship == -1 )//친구 관점에서 다시한번 돌릴 기회를 준다.
					{
						Friend_ship = friend_num[i];
						is_friend[friend_num[i]]=0;
					}
					goto SKIP_FACE;
				}
				else
				{
					if( Friend_ship == i )	//됬군...
						Friend_ship=-1;
				}
			}
			else
			{
				t_v[0]=f->v[0];
				t_v[1]=f->v[1];
				t_v[2]=f->v[2];
				
				t_fu[0] = f->fu[0];
				t_fu[1] = f->fu[1];
				t_fu[2] = f->fu[2];

				t_fv[0] = f->fv[0];
				t_fv[1] = f->fv[1];
				t_fv[2] = f->fv[2];

				l_fu[0] = f->lu[0];
				l_fu[1] = f->lu[1];
				l_fu[2] = f->lu[2];

				l_fv[0] = f->lv[0];
				l_fv[1] = f->lv[1];
				l_fv[2] = f->lv[2];

	//			Vector3fCopy(f->v_normal[0],v_normal[0]);
	//			Vector3fCopy(f->v_normal[1],v_normal[1]);
	//			Vector3fCopy(f->v_normal[2],v_normal[2]);
			}
			Vector4fCopy(mesh->object[k].face[i].f_normal,p_mesh->object[k].p_face[cnt].f_normal);
			for(j=0; j<vertex_num; j++)
			{
				p_mesh->object[k].p_face[cnt].fu[j] = t_fu[j];
				p_mesh->object[k].p_face[cnt].fv[j] = t_fv[j];
				p_mesh->object[k].p_face[cnt].lu[j] = l_fu[j];
				p_mesh->object[k].p_face[cnt].lv[j] = l_fv[j];
				p_mesh->object[k].p_face[cnt].vertex_id[j] = t_v[j];
	//			Vector3fCopy(v_normal[j],p_mesh->object[k].p_face[cnt].v_normal[j]);
			}
			cnt++;
			nu_vnum+=vertex_num;
	SKIP_FACE: {};
			p_mesh->object[k].facenum = cnt;
		}
	}
//-----------
	LONG p_face_num=0;
	for(i=0; i<p_mesh->objectnum; i++)
		p_face_num += p_mesh->object[i].facenum;

	Dfree( friend_num );
	Dfree( is_friend );

	return p_mesh;
}

static _MESH *N_mesh;
static int N_face;

void SetFacetNum(int i)	//노멀 에러를 잡기위해.
{
	N_face = i;
}

static BOOL Normalized( Vector3d vec) //정규화 만들기
{
	double a= sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
	if(a==0)
	{
		int i,temp=0,temp1=0;
		
		for(i=0; i<GetIniFile()->w_mesh->objectnum;i++)
		{
			temp+=GetIniFile()->w_mesh->object[i].facenum;
			if( temp1 <= N_face && temp> N_face)
			{
				break;
			}
			temp1+=GetIniFile()->w_mesh->object[i].facenum;
		}

//		char out[256];
//		sprintf(out,"<-노멀생성에러! %d 폴리곤이 너무작거나 지오메트리생성 문제...",N_face-temp1);
//		Warning(GetIniFile()->w_mesh->object[i].objectname,out);
		vec[0]=0;		vec[1]=0;		vec[2]=1;
		return FALSE;	//노멀생성 불가 경우..
	}
	vec[0]/=a;
	vec[1]/=a;
	vec[2]/=a;
	return TRUE;
}

static _MESH *Org_mesh;

static void Normalized( Vector3f vec) //정규화 만들기
{
	double a= sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
	if(a==0)
	{
		int i,temp=0,temp1=0;
		
		for(i=0; i<Org_mesh->objectnum;i++)
		{
			temp+=Org_mesh->object[i].facenum;
			if( temp1 <= N_face && temp> N_face)
			{
				break;
			}
			temp1+=Org_mesh->object[i].facenum;
		}
		char out[256];
		sprintf(out,"<-노멀생성에러! %d 폴리곤이 너무작거나 지오메트리생성 문제...",N_face-temp1);
		Warning(Org_mesh->object[i].objectname,out);
		vec[0]=0;		vec[1]=0;		vec[2]=1;
		return;
	}
	vec[0]/=(float)a;
	vec[1]/=(float)a;
	vec[2]/=(float)a;
}


static BOOL GetNormal( Vector3f result,_VERTEX *vv0,_VERTEX *vv1,_VERTEX *vv2 )
{
//----------------------- 평면의 방정식 생성
	Vector3d v0,v1,v2,d_res;
/*
	v0[0] = vv0->x;	v0[1] = vv0->y;	v0[2] = vv0->z;
	v1[0] = vv1->x;	v1[1] = vv1->y;	v1[2] = vv1->z;
	v2[0] = vv2->x;	v2[1] = vv2->y;	v2[2] = vv2->z;
*/
	v0[0] = 0;	v0[1] = 0;	v0[2] = 0;
	v1[0] = vv1->x - vv0->x;	v1[1] = vv1->y - vv0->y;	v1[2] = vv1->z - vv0->z;
	v2[0] = vv2->x - vv0->x;	v2[1] = vv2->y - vv0->y;	v2[2] = vv2->z - vv0->z;

    d_res[0]=(v0[1]*(v1[2]-v2[2])+v1[1]*(v2[2]-v0[2])+v2[1]*(v0[2]-v1[2]));
    d_res[1]=(v0[2]*(v1[0]-v2[0])+v1[2]*(v2[0]-v0[0])+v2[2]*(v0[0]-v1[0]));
    d_res[2]=(v0[0]*(v1[1]-v2[1])+v1[0]*(v2[1]-v0[1])+v2[0]*(v0[1]-v1[1]));

	BOOL ret = Normalized(d_res);	//정규화 
	result[0] = (float)d_res[0];
	result[1] = (float)d_res[1];
	result[2] = (float)d_res[2];
	return ret;
}
static BOOL GetNormal( Vector3f result,Vector3f vv0,Vector3f vv1,Vector3f vv2 )
{
//----------------------- 평면의 방정식 생성
	Vector3d v0,v1,v2,d_res;

	v0[0] = 0;	v0[1] = 0;	v0[2] = 0;
	v1[0] = vv1[0] - vv0[0];	v1[1] = vv1[1] - vv0[1];	v1[2] = vv1[2] - vv0[2];
	v2[0] = vv2[0] - vv0[0];	v2[1] = vv2[1] - vv0[1];	v2[2] = vv2[2] - vv0[2];

    d_res[0]=(v0[1]*(v1[2]-v2[2])+v1[1]*(v2[2]-v0[2])+v2[1]*(v0[2]-v1[2]));
    d_res[1]=(v0[2]*(v1[0]-v2[0])+v1[2]*(v2[0]-v0[0])+v2[2]*(v0[0]-v1[0]));
    d_res[2]=(v0[0]*(v1[1]-v2[1])+v1[0]*(v2[1]-v0[1])+v2[0]*(v0[1]-v1[1]));

	BOOL ret = Normalized(d_res);	//정규화 
	result[0] = (float)d_res[0];
	result[1] = (float)d_res[1];
	result[2] = (float)d_res[2];
	return ret;
}

void SetMeshFaceNormal(_MESH *mesh,_MESH *org_mesh)
{
	int i,j;

	Org_mesh = org_mesh;
	if(mesh->poly_type == FALSE )	//삼각메쉬일 경우.
	{
		for( i=0; i<mesh->objectnum; i++)
		{
			for( j=0; j<mesh->object[i].facenum; j++)
			{
				Vector3f normal;
				int v0 = mesh->object[i].face[j].v[0];
				int v1 = mesh->object[i].face[j].v[1];
				int v2 = mesh->object[i].face[j].v[2];
				SetFacetNum(j);	//노멀 에러를 잡기위해.

				if( GetNormal(normal,&mesh->object[i].vertex[v0],&mesh->object[i].vertex[v1]
					,&mesh->object[i].vertex[v2]) == FALSE)
				{
					char out[256];
					sprintf(out,"<-노멀생성에러! %d 폴리곤이 너무작거나 지오메트리생성 문제...",j);
					Warning(mesh->object[i].objectname,out);
				}			
				mesh->object[i].face[j].f_normal[0]=normal[0];
				mesh->object[i].face[j].f_normal[1]=normal[1];
				mesh->object[i].face[j].f_normal[2]=normal[2];
				mesh->object[i].face[j].f_normal[3]=normal[0]*mesh->object[i].vertex[v0].local[0]
					+normal[1]*mesh->object[i].vertex[v0].local[1]+normal[2]*mesh->object[i].vertex[v0].local[2];
			}
		}
		return;
	}

	for( i=0; i<mesh->objectnum; i++)
	{
		for( j=0; j<mesh->object[i].facenum; j++)
		{
			Vector3f normal;
			if( mesh->object[i].p_face[j].vertex_num < 3)
				Error("not a plygon!","");
			int v0 = mesh->object[i].p_face[j].vertex_id[0];
			int v1 = mesh->object[i].p_face[j].vertex_id[1];
			int v2 = mesh->object[i].p_face[j].vertex_id[2];
			SetFacetNum(j);	//노멀 에러를 잡기위해.

			if( GetNormal(normal,&mesh->object[i].vertex[v0],&mesh->object[i].vertex[v1]
				,&mesh->object[i].vertex[v2])==FALSE)
			{
				char out[256];
				sprintf(out,"<-노멀생성에러! %d 폴리곤이 너무작거나 지오메트리생성 문제...",j);
				Warning(mesh->object[i].objectname,out);
			}			
		
			mesh->object[i].p_face[j].f_normal[0]=normal[0];
			mesh->object[i].p_face[j].f_normal[1]=normal[1];
			mesh->object[i].p_face[j].f_normal[2]=normal[2];
			mesh->object[i].p_face[j].f_normal[3]=normal[0]*mesh->object[i].vertex[v0].local[0]
				+normal[1]*mesh->object[i].vertex[v0].local[1]+normal[2]*mesh->object[i].vertex[v0].local[2];
		}
	}
}

static void CalculateObjectSmooth(_OBJECT *obj)	//스무스 그룹의 노멀을 구하자.
{
	int i,j,k;
	//---스무스 그룹의 노멀과 카운트 32개 버퍼를 생성한다.
	Vector3f *n_buf= (Vector3f *)Dmalloc(sizeof(Vector3f)*obj->vertexnum*32);
	if( n_buf == NULL )
		return;
	memset(n_buf,0,sizeof(Vector3f)*obj->vertexnum*32);
	int *c_buf= (int *)Dmalloc(sizeof(int)*obj->vertexnum*32);
	memset(c_buf,0,sizeof(int)*obj->vertexnum*32);

	for(i=0; i<obj->facenum; i++)
	{
		for(j=0; j<obj->p_face[i].vertex_num; j++)
		{
			DWORD s_group = obj->p_face[i].smooth_group;
			DWORD v_id = obj->p_face[i].vertex_id[j];
			if( s_group == 0 )
			{
				obj->p_face[i].v_normal[j][0] = obj->p_face[i].f_normal[0];
				obj->p_face[i].v_normal[j][1] = obj->p_face[i].f_normal[1];
				obj->p_face[i].v_normal[j][2] = obj->p_face[i].f_normal[2];
			}
			else
			{
				for(k=0; k<32; k++)	//각각의 비트를 점검한다.
				{
					if( s_group &( 1<<k ) )
					{
						n_buf[obj->vertexnum*k + v_id][0]+=obj->p_face[i].f_normal[0];
						n_buf[obj->vertexnum*k + v_id][1]+=obj->p_face[i].f_normal[1];
						n_buf[obj->vertexnum*k + v_id][2]+=obj->p_face[i].f_normal[2];
						c_buf[obj->vertexnum*k + v_id]++;
					}
				}
			}
		}
	}
	for(i=0; i<32; i++)		//32가지의 스무스그룹을 정리한다.
	{
		for(j=0; j<obj->vertexnum; j++)
		{
			if( c_buf[obj->vertexnum*i + j] != 0)
			{
				if( c_buf[obj->vertexnum*i + j] != 1 )	//카운트가 1이 아니면 계산한다.
				{
					n_buf[obj->vertexnum*i + j][0]/=(float)c_buf[obj->vertexnum*i + j];
					n_buf[obj->vertexnum*i + j][1]/=(float)c_buf[obj->vertexnum*i + j];
					n_buf[obj->vertexnum*i + j][2]/=(float)c_buf[obj->vertexnum*i + j];
					Normalize(n_buf[obj->vertexnum*i + j]);//오차가 생긴것을 다시 노멀라이즈한다.
					c_buf[obj->vertexnum*i + j]=1;
				}
			}
		}
	}

//------------최종적으로 섞인 스무스그룹을 계산한다.
	for(i=0; i<obj->facenum; i++)
	{
		for(j=0; j<obj->p_face[i].vertex_num; j++)
		{
			DWORD s_group = obj->p_face[i].smooth_group;
			DWORD v_id = obj->p_face[i].vertex_id[j];
			if( s_group != 0 )
			{
				Vector3f vec;
				vec[0]=0;
				vec[1]=0;
				vec[2]=0;

				int cnt=0;
				for(k=0; k<32; k++)	//각각의 비트를 점검한다.
				{
					if( s_group &( 1<<k ) )
					{
						vec[0] += n_buf[obj->vertexnum*k + v_id][0];
						vec[1] += n_buf[obj->vertexnum*k + v_id][1];
						vec[2] += n_buf[obj->vertexnum*k + v_id][2];
						cnt++;
					}
				}
				vec[0]/=(float)cnt;
				vec[1]/=(float)cnt;
				vec[2]/=(float)cnt;
				Normalize(vec);	//다시한번 평준화를 한다.
				obj->p_face[i].v_normal[j][0] = vec[0];
				obj->p_face[i].v_normal[j][1] = vec[1];
				obj->p_face[i].v_normal[j][2] = vec[2];
			}
		}
	}

	Dfree(c_buf);
	Dfree(n_buf);
}

void CalculateMeshSmooth(_MESH *mesh)	//스무스 그룹의 노멀을 구하자.
{
	int i;

	if(mesh->poly_type == FALSE )//폴리메쉬를 작업해야지.
		return;
	for(i=0; i<mesh->objectnum;i++)
	{
		CalculateObjectSmooth(&mesh->object[i]);	//스무스 그룹의 노멀을 구하자.
	}
}

void SetLocalCoodinate(_OBJECT *object)
{
	float x,y,z;
	D3DMATRIX mat;
	int i;

	memcpy(&mat,object->revmatrix,sizeof(D3DMATRIX));
    for(i=0;i< object->vertexnum;i++)
    {                                                       
        x = object->vertex[i].local[0] * mat._11 +
            object->vertex[i].local[2] * mat._21 +
            object->vertex[i].local[1] * mat._31 + mat._41;
        y = object->vertex[i].local[0] * mat._12 +
            object->vertex[i].local[2] * mat._22 +
            object->vertex[i].local[1] * mat._32 + mat._42;
        z = object->vertex[i].local[0] * mat._13 +
            object->vertex[i].local[2] * mat._23 +
            object->vertex[i].local[1] * mat._33 + mat._43;

        object->vertex[i].local[0] = x;
        object->vertex[i].local[1] = y;
        object->vertex[i].local[2] = z;
//		FixVertexEpsilon( object->vertex[i].local);
    }
}
void SetWorldCoodinate(_OBJECT *object)
{
	float x,y,z;
	D3DMATRIX mat;
	int i;

	memcpy(&mat,object->trmatrix,sizeof(D3DMATRIX));
    for(i=0;i< object->vertexnum;i++)
    {                                                       
        x = object->vertex[i].local[0] * mat._11 +
            object->vertex[i].local[1] * mat._21 +
            object->vertex[i].local[2] * mat._31 + mat._41;
        y = object->vertex[i].local[0] * mat._12 +
            object->vertex[i].local[1] * mat._22 +
            object->vertex[i].local[2] * mat._32 + mat._42;
        z = object->vertex[i].local[0] * mat._13 +
            object->vertex[i].local[1] * mat._23 +
            object->vertex[i].local[2] * mat._33 + mat._43;

        object->vertex[i].local[0] = x;
        object->vertex[i].local[1] = z;
        object->vertex[i].local[2] = y;
//		FixVertexEpsilon( object->vertex[i].local);
    }
}
//두 바운딩 박스가 충돌 하는가?
BOOL IsCollisionBBbox(Vector3f a_min,Vector3f a_max,Vector3f b_min,Vector3f b_max)
{
	if( b_min[0] < a_max[0] && a_min[0] < b_max[0]
	&& b_min[1] < a_max[1] && a_min[1] < b_max[1]
	&& b_min[2] < a_max[2] && a_min[2] < b_max[2] )
		return TRUE;
	return FALSE;
}

//----------- 박스 영역의 클리핑....
typedef struct{
	Vector3f v;
	int ccodes;
	float tex_u,tex_v;
}_TEMP_VERETX;

#define CC_OFF_LEFT		0x00008000
#define CC_OFF_RIGHT	0x00004000
#define CC_OFF_TOP		0x00002000
#define CC_OFF_BOT		0x00001000
#define CC_OFF_FRONT	0x00000800
#define CC_OFF_BACK		0x00000400

#define _TOP_NORMAL		0
#define _BOT_NORMAL		1
#define _LEFT_NORMAL	2
#define _RIGHT_NORMAL	3
#define _FRONT_NORMAL	4
#define _BACK_NORMAL	5

int GetCodeVertex(Vector3f v,Vector4f f_normal[6])
{
	int ret_code=0;
	if( f_normal[_TOP_NORMAL][0]*v[0]+f_normal[_TOP_NORMAL][1]*v[1]
		+f_normal[_TOP_NORMAL][2]*v[2]- f_normal[_TOP_NORMAL][3] <0 )
		ret_code|=CC_OFF_TOP;
	if( f_normal[_BOT_NORMAL][0]*v[0]+f_normal[_BOT_NORMAL][1]*v[1]
		+f_normal[_BOT_NORMAL][2]*v[2]- f_normal[_BOT_NORMAL][3] <0 )
		ret_code|=CC_OFF_BOT;
	if( f_normal[_LEFT_NORMAL][0]*v[0]+f_normal[_LEFT_NORMAL][1]*v[1]
		+f_normal[_LEFT_NORMAL][2]*v[2]- f_normal[_LEFT_NORMAL][3] <0 )
		ret_code|=CC_OFF_LEFT;
	if( f_normal[_RIGHT_NORMAL][0]*v[0]+f_normal[_RIGHT_NORMAL][1]*v[1]
		+f_normal[_RIGHT_NORMAL][2]*v[2]- f_normal[_RIGHT_NORMAL][3] <0 )
		ret_code|=CC_OFF_RIGHT;
	if( f_normal[_FRONT_NORMAL][0]*v[0]+f_normal[_FRONT_NORMAL][1]*v[1]
		+f_normal[_FRONT_NORMAL][2]*v[2]- f_normal[_FRONT_NORMAL][3] <0 )
		ret_code|=CC_OFF_FRONT;
	if( f_normal[_BACK_NORMAL][0]*v[0]+f_normal[_BACK_NORMAL][1]*v[1]
		+f_normal[_BACK_NORMAL][2]*v[2]- f_normal[_BACK_NORMAL][3] <0 )
		ret_code|=CC_OFF_BACK;
	return ret_code;
}

int GetPlaneCrossPointClipCode(_TEMP_VERETX *cross,_TEMP_VERETX *a,_TEMP_VERETX *b,Vector4f f_normal,Vector4f p_normal[6])
{
    float front = f_normal[0]*a->v[0] + f_normal[1]*a->v[1] + f_normal[2]*a->v[2] -f_normal[3];
    float back  = f_normal[0]*b->v[0] + f_normal[1]*b->v[1] + f_normal[2]*b->v[2] -f_normal[3];

    if ( (front > 0 && back > 0) || (front < 0 && back < 0)) 
		return FALSE;

    front = front / (front-back);

    cross->v[0] = a->v[0] + front*(b->v[0]-a->v[0]);
    cross->v[1] = a->v[1] + front*(b->v[1]-a->v[1]);
    cross->v[2] = a->v[2] + front*(b->v[2]-a->v[2]);
    cross->tex_u = a->tex_u + front*(b->tex_u-a->tex_u);
    cross->tex_v = a->tex_v + front*(b->tex_v-a->tex_v);

	return GetCodeVertex(cross->v,p_normal);
}

_TEMP_VERETX stVpts[64], *stDefaultVlist[64],pts[64],*clip_list1[64],*clip_list2[64];
_TEMP_VERETX vpts[64], *default_vlist[64];

int left_ext_vertex_cnt,right_ext_vertex_cnt,top_ext_vertex_cnt;
int bot_ext_vertex_cnt,front_ext_vertex_cnt,back_ext_vertex_cnt;

_TEMP_VERETX left_ext_vertex[32];
_TEMP_VERETX right_ext_vertex[32];
_TEMP_VERETX top_ext_vertex[32];
_TEMP_VERETX bot_ext_vertex[32];
_TEMP_VERETX front_ext_vertex[32];
_TEMP_VERETX back_ext_vertex[32];

void InitDefaultPointList(void)
{
   int i;
   for (i=0; i < 64; ++i)
      default_vlist[i] = &vpts[i];
}

int ClipVertexListInBox(int n,_TEMP_VERETX **vl,Vector4f p_normal[6], int codes_or, _TEMP_VERETX ***out_vl)
{
	int i,j,k,p=0; // p = index into temporary point pool
	_TEMP_VERETX **cur;

	left_ext_vertex_cnt=0;
	if (codes_or & CC_OFF_LEFT) 
	{
		cur = clip_list1;
		k = 0;
		j = n-1;
		for (i=0; i < n; ++i) 
		{
	        if (!(vl[j]->ccodes & CC_OFF_LEFT))
				cur[k++] = vl[j];
			else//외부 폴리곤을 위해서 
				memcpy(&left_ext_vertex[left_ext_vertex_cnt++],vl[j],sizeof(_TEMP_VERETX));

			 // if it crosses, add the intersection point
			if ((vl[j]->ccodes ^ vl[i]->ccodes) & CC_OFF_LEFT) 
			{
				pts[p].ccodes = GetPlaneCrossPointClipCode(&pts[p],vl[i], vl[j],p_normal[_LEFT_NORMAL],p_normal);
				//외부 폴리곤을 위해서 
				memcpy(&left_ext_vertex[left_ext_vertex_cnt++],&pts[p],sizeof(_TEMP_VERETX));
				cur[k++] = &pts[p++];
			}
			j = i;
		}
	    // move output list to be input
		n = k;
		vl = cur;
	}
	right_ext_vertex_cnt=0;
	if (codes_or & CC_OFF_RIGHT) 
	{
		cur = (vl == clip_list1) ? clip_list2 : clip_list1;
		k = 0;
		j = n-1;
		for (i=0; i < n; ++i) 
		{
			if (!(vl[j]->ccodes & CC_OFF_RIGHT))
				cur[k++] = vl[j];
			else//외부 폴리곤을 위해서 
				memcpy(&right_ext_vertex[right_ext_vertex_cnt++],vl[j],sizeof(_TEMP_VERETX));
			if ((vl[j]->ccodes ^ vl[i]->ccodes) & CC_OFF_RIGHT) 
			{
				pts[p].ccodes = GetPlaneCrossPointClipCode(&pts[p],vl[i], vl[j],p_normal[_RIGHT_NORMAL],p_normal);
				//외부 폴리곤을 위해서 
				memcpy(&right_ext_vertex[right_ext_vertex_cnt++],&pts[p],sizeof(_TEMP_VERETX));
				cur[k++] = &pts[p++];
			}
			j = i;
		}
		n = k;
		vl = cur;
	}
	top_ext_vertex_cnt=0;
	if (codes_or & CC_OFF_TOP) 
	{
		cur = (vl == clip_list1) ? clip_list2 : clip_list1;
		k = 0;
		j = n-1;
		for (i=0; i < n; ++i) 
		{
			if (!(vl[j]->ccodes & CC_OFF_TOP))
				cur[k++] = vl[j];
			else//외부 폴리곤을 위해서 
				memcpy(&top_ext_vertex[top_ext_vertex_cnt++],vl[j],sizeof(_TEMP_VERETX));
			if ((vl[j]->ccodes ^ vl[i]->ccodes) & CC_OFF_TOP) 
			{
				pts[p].ccodes = GetPlaneCrossPointClipCode(&pts[p],vl[i], vl[j],p_normal[_TOP_NORMAL],p_normal);
				//외부 폴리곤을 위해서 
				memcpy(&top_ext_vertex[top_ext_vertex_cnt++],&pts[p],sizeof(_TEMP_VERETX));
				cur[k++] = &pts[p++];
			}
			j = i;
		}
		n = k;
		vl = cur;
	}
	bot_ext_vertex_cnt=0;
	if (codes_or & CC_OFF_BOT) 
	{	
		cur = (vl == clip_list1) ? clip_list2 : clip_list1;
		k = 0;
		j = n-1;
		for (i=0; i < n; ++i) 
		{
			if (!(vl[j]->ccodes & CC_OFF_BOT))
				cur[k++] = vl[j];
			else//외부 폴리곤을 위해서 
				memcpy(&bot_ext_vertex[bot_ext_vertex_cnt++],vl[j],sizeof(_TEMP_VERETX));
			if ((vl[j]->ccodes ^ vl[i]->ccodes) & CC_OFF_BOT) 
			{
				pts[p].ccodes = GetPlaneCrossPointClipCode(&pts[p],vl[i], vl[j],p_normal[_BOT_NORMAL],p_normal);
				//외부 폴리곤을 위해서 
				memcpy(&bot_ext_vertex[bot_ext_vertex_cnt++],&pts[p],sizeof(_TEMP_VERETX));
				cur[k++] = &pts[p++];
			}
			j = i;
		}
		n = k;
		vl = cur;
	}
	front_ext_vertex_cnt=0;
	if (codes_or & CC_OFF_FRONT) 
	{
		cur = (vl == clip_list1) ? clip_list2 : clip_list1;
		k = 0;
		j = n-1;
		for (i=0; i < n; ++i) 
		{
			if (!(vl[j]->ccodes & CC_OFF_FRONT))
				cur[k++] = vl[j];
			else//외부 폴리곤을 위해서 
				memcpy(&front_ext_vertex[front_ext_vertex_cnt++],vl[j],sizeof(_TEMP_VERETX));
			if ((vl[j]->ccodes ^ vl[i]->ccodes) & CC_OFF_FRONT) 
			{
				pts[p].ccodes = GetPlaneCrossPointClipCode(&pts[p],vl[i], vl[j],p_normal[_FRONT_NORMAL],p_normal);
				//외부 폴리곤을 위해서 
				memcpy(&front_ext_vertex[front_ext_vertex_cnt++],&pts[p],sizeof(_TEMP_VERETX));
				cur[k++] = &pts[p++];
			}
			j = i;
		}
		n = k;
		vl = cur;
	}
	back_ext_vertex_cnt=0;
	if (codes_or & CC_OFF_BACK) 
	{	
		cur = (vl == clip_list1) ? clip_list2 : clip_list1;
		k = 0;
		j = n-1;
		for (i=0; i < n; ++i) 
		{
			if (!(vl[j]->ccodes & CC_OFF_BACK))
				cur[k++] = vl[j];
			else//외부 폴리곤을 위해서 
				memcpy(&back_ext_vertex[back_ext_vertex_cnt++],vl[j],sizeof(_TEMP_VERETX));
			if ((vl[j]->ccodes ^ vl[i]->ccodes) & CC_OFF_BACK) 
			{
				pts[p].ccodes = GetPlaneCrossPointClipCode(&pts[p],vl[i], vl[j],p_normal[_BACK_NORMAL],p_normal);
				//외부 폴리곤을 위해서 
				memcpy(&back_ext_vertex[back_ext_vertex_cnt++],&pts[p],sizeof(_TEMP_VERETX));
				cur[k++] = &pts[p++];
			}
			j = i;
		}
		n = k;
		vl = cur;
	}

	*out_vl = vl;
	return n;
}
int GetVertexIndex( Vector3f v,_VERTEX *vertex,int vertexnum)
{
	for(int i=0; i<vertexnum; i++)
	{
		if( vertex[i].local[0] == v[0] && vertex[i].local[1] == v[1]
		&& vertex[i].local[2] == v[2] )
		{
			return i;
		}
	}
	return -1;
}

#define _SetVertexIndex(ver,k,m) \
{\
	int v_index=GetVertexIndex(ver,obj_vertex,obj_vertex_num);\
	if( v_index == -1)\
	{\
		obj_face[k].v[m] = obj_vertex_num;\
		memcpy(&obj_vertex[obj_vertex_num],ver,sizeof(Vector3f));\
		obj_vertex_num++;\
	}\
	else\
		obj_face[k].v[m] = v_index;\
}\


_MATERIAL *MakeVolumeFogObject(_MESH *mesh,_MATERIAL *mat)	//볼륨 포그영역의 폴리곤들을 재 생성한다.
{
	int i,j,k,l,m;
    _TEMP_VERETX **vlist;
	Vector3f temp_v[32];
	_VERTEX *obj_vertex;
    Vector3f *obj_fnormal;
	_FACE *obj_face;

	int v_fog_cnt=0;
	InitDefaultPointList();	//클립 버텍스 리스트를 초기화.
	for(i=0 ;i<mesh->objectnum; i++)
	{
		if( !(mesh->object[i].type&_V_FOG) )	//오오 볼륨 포그닷...
			continue;
		if( mesh->object[i].vertexnum != 8 )
			Error("볼륨포그를 하시려면 box로 지정해야됩니다. ","");
		_VERTEX *org_vertex = mesh->object[i].vertex;
		_FACE *org_face = mesh->object[i].face;

		int vertex_num = 8;
		int face_num = 12;
		int max_vertex_num = 8;
		int max_face_num = 12;

		_VERTEX *n_vertex=(_VERTEX *)Dmalloc(sizeof(_VERTEX)*max_vertex_num);
		_FACE *n_face=(_FACE *)Dmalloc(sizeof(_FACE)*max_face_num);
		memcpy(n_vertex,mesh->object[i].vertex,sizeof(_VERTEX)*vertex_num);
		memcpy(n_face,mesh->object[i].face,sizeof(_FACE)*face_num);
		for(j=0 ;j<mesh->object[i].facenum; j++)
		{
			n_face[j].f_normal[3] = -n_face[j].f_normal[3];
			int temp = n_face[j].v[2];
			n_face[j].v[2]=n_face[j].v[1];
			n_face[j].v[1]=temp;
		}

		for(j=0 ;j<mesh->objectnum; j++)
		{
			if( mesh->object[j].type&_V_FOG )	// 볼륨 포그는 빼야쥐..
				continue;
			if(!IsCollisionBBbox(mesh->object[i].bb_min,mesh->object[i].bb_max
				,mesh->object[j].bb_min,mesh->object[j].bb_max) ) //포그영역에 오브젝트가 벗어나있다.
				continue;
/*
#define _TOP_NORMAL		0
#define _BOT_NORMAL		1
#define _LEFT_NORMAL	2
#define _RIGHT_NORMAL	3
#define _FRONT_NORMAL	4
#define _BACK_NORMAL	5
*/
			Vector4f p_normal[6];	//포그 영역 평면 노멀
			GetNormal(p_normal[_TOP_NORMAL],&org_vertex[0],&org_vertex[1],&org_vertex[2] );
			p_normal[_TOP_NORMAL][3] = p_normal[_TOP_NORMAL][0]*org_vertex[0].x+
				p_normal[_TOP_NORMAL][1]*org_vertex[0].y+p_normal[_TOP_NORMAL][2]*org_vertex[0].z;

			GetNormal(p_normal[_BOT_NORMAL],&org_vertex[7],&org_vertex[5],&org_vertex[4] );
			p_normal[_BOT_NORMAL][3] = p_normal[_BOT_NORMAL][0]*org_vertex[7].x+
				p_normal[_BOT_NORMAL][1]*org_vertex[7].y+p_normal[_BOT_NORMAL][2]*org_vertex[7].z;

			GetNormal(p_normal[_LEFT_NORMAL],&org_vertex[0],&org_vertex[6],&org_vertex[4] );
			p_normal[_LEFT_NORMAL][3] = p_normal[_LEFT_NORMAL][0]*org_vertex[0].x+
				p_normal[_LEFT_NORMAL][1]*org_vertex[0].y+p_normal[_LEFT_NORMAL][2]*org_vertex[0].z;

			GetNormal(p_normal[_RIGHT_NORMAL],&org_vertex[3],&org_vertex[1],&org_vertex[5] );
			p_normal[_RIGHT_NORMAL][3] = p_normal[_RIGHT_NORMAL][0]*org_vertex[3].x+
				p_normal[_RIGHT_NORMAL][1]*org_vertex[3].y+p_normal[_RIGHT_NORMAL][2]*org_vertex[3].z;

			GetNormal(p_normal[_FRONT_NORMAL],&org_vertex[0],&org_vertex[5],&org_vertex[1] );
			p_normal[_FRONT_NORMAL][3] = p_normal[_FRONT_NORMAL][0]*org_vertex[0].x+
				p_normal[_FRONT_NORMAL][1]*org_vertex[0].y+p_normal[_FRONT_NORMAL][2]*org_vertex[0].z;

			GetNormal(p_normal[_BACK_NORMAL],&org_vertex[7],&org_vertex[2],&org_vertex[3] );
			p_normal[_BACK_NORMAL][3] = p_normal[_BACK_NORMAL][0]*org_vertex[7].x+
				p_normal[_BACK_NORMAL][1]*org_vertex[7].y+p_normal[_BACK_NORMAL][2]*org_vertex[7].z;

			//-------새로 생성되는 포그경계선의 좌표들을 좌악 
			int obj_vertex_num = mesh->object[j].vertexnum;
			int obj_face_num = mesh->object[j].facenum;
			obj_vertex=(_VERTEX *)Dmalloc(sizeof(_VERTEX)*mesh->object[j].vertexnum);
			obj_face=(_FACE *)Dmalloc(sizeof(_FACE)*mesh->object[j].facenum);
			obj_fnormal=(Vector3f *)Dmalloc(sizeof(Vector3f)*mesh->object[j].facenum);
			//노멀은 난중에 계산되니까 복사하지 말구...
			memcpy(obj_vertex,mesh->object[j].vertex,sizeof(_VERTEX)*mesh->object[j].vertexnum);
			memcpy(obj_face,mesh->object[j].face,sizeof(_FACE)*mesh->object[j].facenum);
			
			for(k=0; k<mesh->object[j].facenum; k++)
			{
				int index0=mesh->object[j].face[k].v[0];
				int index1=mesh->object[j].face[k].v[1];
				int index2=mesh->object[j].face[k].v[2];
				memcpy(vpts[0].v,mesh->object[j].vertex[index0].local,sizeof(Vector3f));
				memcpy(vpts[1].v,mesh->object[j].vertex[index1].local,sizeof(Vector3f));
				memcpy(vpts[2].v,mesh->object[j].vertex[index2].local,sizeof(Vector3f));
				vpts[0].ccodes = GetCodeVertex(vpts[0].v,p_normal);
				vpts[1].ccodes = GetCodeVertex(vpts[1].v,p_normal);
				vpts[2].ccodes = GetCodeVertex(vpts[2].v,p_normal);
				vpts[0].tex_u = mesh->object[j].face[k].fu[0];
				vpts[0].tex_v = mesh->object[j].face[k].fv[0];
				vpts[1].tex_u = mesh->object[j].face[k].fu[1];
				vpts[1].tex_v = mesh->object[j].face[k].fv[1];
				vpts[2].tex_u = mesh->object[j].face[k].fu[2];
				vpts[2].tex_v = mesh->object[j].face[k].fv[2];
				int codes_or=0;
				codes_or|=vpts[0].ccodes;
				codes_or|=vpts[1].ccodes;
				codes_or|=vpts[2].ccodes;
				int clip_v_cnt= ClipVertexListInBox(3,default_vlist,p_normal,codes_or, &vlist);
				if( clip_v_cnt >= 3 )	//
				{
					for( l=0; l<clip_v_cnt; l++)
						memcpy(temp_v[l],vlist[l]->v,sizeof(Vector3f));
					//넘칠때 리얼록
					int pre_vertexnum = clip_v_cnt + vertex_num;
					int pre_facenum = clip_v_cnt-2 + face_num;
					if( pre_vertexnum >= max_vertex_num )
					{
						pre_vertexnum = max_vertex_num + 50;
						n_vertex=(_VERTEX *)ReAlloc(n_vertex,max_vertex_num*sizeof(_VERTEX),pre_vertexnum*sizeof(_VERTEX));
						max_vertex_num = pre_vertexnum;
					}
					if( pre_facenum >= max_face_num )
					{
						pre_facenum = max_face_num + 50;
						n_face=(_FACE *)ReAlloc(n_face,max_face_num*sizeof(_FACE),pre_facenum*sizeof(_FACE));
						max_face_num=pre_facenum;
					}
					for( l=0; l<clip_v_cnt; l++)
						memcpy(n_vertex[vertex_num+l].local,temp_v[l],sizeof(Vector3f));
					for( l=0; l<clip_v_cnt-2; l++)
					{
						memset(&n_face[face_num+l],0,sizeof(_FACE));
						memcpy(n_face[face_num+l].f_normal,mesh->object[j].face[k].f_normal,sizeof(Vector4f));
						n_face[face_num+l].mtlid=-1;
						n_face[face_num+l].v[0]=vertex_num;
						n_face[face_num+l].v[1]=vertex_num+l+1;
						n_face[face_num+l].v[2]=vertex_num+l+2;
					}
					vertex_num+=clip_v_cnt;
					face_num+=(clip_v_cnt-2);

					//-- 원래폴리곤을 포그영역으로 걸쳐진 폴리곤으로 죄 잘라내야 한다.
					if( left_ext_vertex_cnt || right_ext_vertex_cnt || top_ext_vertex_cnt
						|| bot_ext_vertex_cnt || front_ext_vertex_cnt || back_ext_vertex_cnt )
					{	//포그 외부 영역을 폴리곤으로 만들자...
						int add_vertex_num = 40;
						//clip_v_cnt + left_ext_vertex_cnt + right_ext_vertex_cnt + top_ext_vertex_cnt
						//	+ bot_ext_vertex_cnt + front_ext_vertex_cnt + back_ext_vertex_cnt;
						int add_face_num = 40;
						//일단 충분히 추가될 면과 점을 설정한다.
						obj_vertex=(_VERTEX *)ReAlloc(obj_vertex,obj_vertex_num*sizeof(_VERTEX),(obj_vertex_num+add_vertex_num)*sizeof(_VERTEX));
						obj_face=(_FACE *)ReAlloc(obj_face,obj_face_num*sizeof(_FACE),(obj_face_num+add_face_num)*sizeof(_FACE));
						obj_fnormal=(Vector3f *)ReAlloc(obj_fnormal,obj_face_num*sizeof(Vector3f),(obj_face_num+add_face_num)*sizeof(Vector3f));
						
						//현재 지정된 페이스를 먼저 바꾸어 준다.mesh->object[j].face[k]를 넣는다.
						for( l=0; l<3; l++)
						{
							_SetVertexIndex(vlist[l]->v,k,l);
							obj_face[k].fu[l] = vlist[l]->tex_u;
							obj_face[k].fv[l] = vlist[l]->tex_v;
						}
						
						int f_index;
						Vector3f temp;
						for( l=3; l<clip_v_cnt ;l++)
						{
							f_index = obj_face_num;
							memset(&obj_face[f_index],0,sizeof(_FACE));
							_SetVertexIndex(vlist[0]->v,f_index,0);
							_SetVertexIndex(vlist[l-1]->v,f_index,1);
							_SetVertexIndex(vlist[l]->v,f_index,2);
							if( GetNormal(temp,vlist[0]->v,vlist[l-1]->v,vlist[l]->v) == FALSE)
								continue;
							obj_face[f_index].mtlid=obj_face[k].mtlid;
							obj_face[f_index].fu[0] = vlist[0]->tex_u;
							obj_face[f_index].fv[0] = vlist[0]->tex_v;
							obj_face[f_index].fu[1] = vlist[l-1]->tex_u;
							obj_face[f_index].fv[1] = vlist[l-1]->tex_v;
							obj_face[f_index].fu[2] = vlist[l]->tex_u;
							obj_face[f_index].fv[2] = vlist[l]->tex_v;
							obj_face_num++;
						}
						for( l=2; l<back_ext_vertex_cnt ;l++)
						{							
							f_index = obj_face_num;
							memset(&obj_face[f_index],0,sizeof(_FACE));
							_SetVertexIndex(back_ext_vertex[0].v,f_index,0);
							_SetVertexIndex(back_ext_vertex[l-1].v,f_index,1);
							_SetVertexIndex(back_ext_vertex[l].v,f_index,2);
							if( GetNormal(temp,back_ext_vertex[0].v,back_ext_vertex[l-1].v,back_ext_vertex[l].v) == FALSE)
								continue;
							obj_face[f_index].fu[0] = back_ext_vertex[0].tex_u;
							obj_face[f_index].fv[0] = back_ext_vertex[0].tex_v;
							obj_face[f_index].fu[1] = back_ext_vertex[l-1].tex_u;
							obj_face[f_index].fv[1] = back_ext_vertex[l-1].tex_v;
							obj_face[f_index].fu[2] = back_ext_vertex[l].tex_u;
							obj_face[f_index].fv[2] = back_ext_vertex[l].tex_v;
							obj_face[f_index].mtlid=obj_face[k].mtlid;
							obj_face_num++;
						}
						for( l=2; l<right_ext_vertex_cnt ;l++)
						{							
							f_index = obj_face_num;
							memset(&obj_face[f_index],0,sizeof(_FACE));
							_SetVertexIndex(right_ext_vertex[0].v,f_index,0);
							_SetVertexIndex(right_ext_vertex[l-1].v,f_index,1);
							_SetVertexIndex(right_ext_vertex[l].v,f_index,2);
							if( GetNormal(temp,right_ext_vertex[0].v,right_ext_vertex[l-1].v,right_ext_vertex[l].v) == FALSE)
								continue;
							obj_face[f_index].fu[0] = right_ext_vertex[0].tex_u;
							obj_face[f_index].fv[0] = right_ext_vertex[0].tex_v;
							obj_face[f_index].fu[1] = right_ext_vertex[l-1].tex_u;
							obj_face[f_index].fv[1] = right_ext_vertex[l-1].tex_v;
							obj_face[f_index].fu[2] = right_ext_vertex[l].tex_u;
							obj_face[f_index].fv[2] = right_ext_vertex[l].tex_v;
							obj_face[f_index].mtlid=obj_face[k].mtlid;
							obj_face_num++;
						}
						for( l=2; l<top_ext_vertex_cnt ;l++)
						{							
							f_index = obj_face_num;
							memset(&obj_face[f_index],0,sizeof(_FACE));
							_SetVertexIndex(top_ext_vertex[0].v,f_index,0);
							_SetVertexIndex(top_ext_vertex[l-1].v,f_index,1);
							_SetVertexIndex(top_ext_vertex[l].v,f_index,2);
							if( GetNormal(temp,top_ext_vertex[0].v,top_ext_vertex[l-1].v,top_ext_vertex[l].v) == FALSE)
								continue;
							obj_face[f_index].fu[0] = top_ext_vertex[0].tex_u;
							obj_face[f_index].fv[0] = top_ext_vertex[0].tex_v;
							obj_face[f_index].fu[1] = top_ext_vertex[l-1].tex_u;
							obj_face[f_index].fv[1] = top_ext_vertex[l-1].tex_v;
							obj_face[f_index].fu[2] = top_ext_vertex[l].tex_u;
							obj_face[f_index].fv[2] = top_ext_vertex[l].tex_v;
							obj_face[f_index].mtlid=obj_face[k].mtlid;
							obj_face_num++;
						}
						for( l=2; l<bot_ext_vertex_cnt ;l++)
						{							
							f_index = obj_face_num;
							memset(&obj_face[f_index],0,sizeof(_FACE));
							_SetVertexIndex(bot_ext_vertex[0].v,f_index,0);
							_SetVertexIndex(bot_ext_vertex[l-1].v,f_index,1);
							_SetVertexIndex(bot_ext_vertex[l].v,f_index,2);
							if( GetNormal(temp,bot_ext_vertex[0].v,bot_ext_vertex[l-1].v,bot_ext_vertex[l].v) == FALSE)
								continue;
							obj_face[f_index].mtlid=obj_face[k].mtlid;
							obj_face[f_index].fu[0] = bot_ext_vertex[0].tex_u;
							obj_face[f_index].fv[0] = bot_ext_vertex[0].tex_v;
							obj_face[f_index].fu[1] = bot_ext_vertex[l-1].tex_u;
							obj_face[f_index].fv[1] = bot_ext_vertex[l-1].tex_v;
							obj_face[f_index].fu[2] = bot_ext_vertex[l].tex_u;
							obj_face[f_index].fv[2] = bot_ext_vertex[l].tex_v;
							obj_face_num++;
						}
						for( l=2; l<front_ext_vertex_cnt ;l++)
						{							
							f_index = obj_face_num;
							memset(&obj_face[f_index],0,sizeof(_FACE));
							_SetVertexIndex(front_ext_vertex[0].v,f_index,0);
							_SetVertexIndex(front_ext_vertex[l-1].v,f_index,1);
							_SetVertexIndex(front_ext_vertex[l].v,f_index,2);
							if( GetNormal(temp,front_ext_vertex[0].v,front_ext_vertex[l-1].v,front_ext_vertex[l].v) == FALSE)
								continue;
							obj_face[f_index].mtlid=obj_face[k].mtlid;
							obj_face[f_index].fu[0] = front_ext_vertex[0].tex_u;
							obj_face[f_index].fv[0] = front_ext_vertex[0].tex_v;
							obj_face[f_index].fu[1] = front_ext_vertex[l-1].tex_u;
							obj_face[f_index].fv[1] = front_ext_vertex[l-1].tex_v;
							obj_face[f_index].fu[2] = front_ext_vertex[l].tex_u;
							obj_face[f_index].fv[2] = front_ext_vertex[l].tex_v;
							obj_face_num++;
						}
						for( l=2; l<left_ext_vertex_cnt ;l++)
						{							
							f_index = obj_face_num;
							memset(&obj_face[f_index],0,sizeof(_FACE));
							_SetVertexIndex(left_ext_vertex[0].v,f_index,0);
							_SetVertexIndex(left_ext_vertex[l-1].v,f_index,1);
							_SetVertexIndex(left_ext_vertex[l].v,f_index,2);
							if( GetNormal(temp,left_ext_vertex[0].v,left_ext_vertex[l-1].v,left_ext_vertex[l].v) == FALSE)
								continue;
							obj_face[f_index].mtlid=obj_face[k].mtlid;
							obj_face[f_index].fu[0] = left_ext_vertex[0].tex_u;
							obj_face[f_index].fv[0] = left_ext_vertex[0].tex_v;
							obj_face[f_index].fu[1] = left_ext_vertex[l-1].tex_u;
							obj_face[f_index].fv[1] = left_ext_vertex[l-1].tex_v;
							obj_face[f_index].fu[2] = left_ext_vertex[l].tex_u;
							obj_face[f_index].fv[2] = left_ext_vertex[l].tex_v;
							obj_face_num++;
						}
					}
				}
			}
			Dfree(mesh->object[j].face);
			Dfree(mesh->object[j].vertex);
			Dfree(mesh->object[j].fnormal);
			mesh->object[j].vertex = obj_vertex;
			mesh->object[j].vertexnum = obj_vertex_num;
			mesh->object[j].face = obj_face;
			mesh->object[j].facenum = obj_face_num;
			mesh->object[j].fnormal =obj_fnormal;
		}
		//버텍스 정리...
		mesh->object[i].vertex = n_vertex;
		mesh->object[i].face=n_face;
		mesh->object[i].facenum = face_num;
		mesh->object[i].vertexnum = vertex_num;
		int vol_mat_id = mat->all_num;	//볼륨포그의 머터리얼을 늘린다.
		mat = (_MATERIAL*)ReAlloc(mat,sizeof(_MATERIAL)*mat->all_num,sizeof(_MATERIAL)*(mat->all_num+1));
		mat->all_num++;

		//머터리얼 기본값 셋팅.
		memset(&mat[vol_mat_id],0,sizeof(_MATERIAL)); 
		mat[vol_mat_id].attr |=_MAT_VOLUME_FOG;
		mat[vol_mat_id].same_mat=-1;
		mat[vol_mat_id].is_used=TRUE;
		mat[vol_mat_id].lgt_id=-1;
		mat[vol_mat_id].transparency=1;
		sprintf(mat[vol_mat_id].name,"vfog%02d",v_fog_cnt);
		for(k=0; k<vertex_num; k++)
		{
			for(l=k+1; l<vertex_num; l++)
			{
				if( fabs(n_vertex[k].local[0] - n_vertex[l].local[0]) < 0.01f
				&& fabs(n_vertex[k].local[1] - n_vertex[l].local[1]) < 0.01f 
				&& fabs(n_vertex[k].local[2] - n_vertex[l].local[2]) < 0.01f )
				{
					for(m=0; m<face_num; m++)
					{
						if( n_face[m].v[0] == l )
							n_face[m].v[0] = k;
						if( n_face[m].v[1] == l )
							n_face[m].v[1] = k;
						if( n_face[m].v[2] == l )
							n_face[m].v[2] = k;
					}
				}
			}
		}
		for(k=0; k<face_num; k++)
		{
			n_face[k].mtlid = vol_mat_id;	//볼륨 fog 머터리얼 아이디..
		}
		/*
		Vector3f *v_normal=(Vector3f *)Dmalloc(sizeof(Vector3f)*vertex_num);
		memset(v_normal,0,sizeof(Vector3f)*vertex_num);
		for(k=0; k<face_num; k++)
		{
			GetNormal( n_face[k].f_normal,&n_vertex[n_face[k].v[0]],&n_vertex[n_face[k].v[1]],&n_vertex[n_face[k].v[2]] );
			v_normal[n_face[k].v[0]][0] += n_face[k].f_normal[0];
			v_normal[n_face[k].v[0]][1] += n_face[k].f_normal[1];
			v_normal[n_face[k].v[0]][2] += n_face[k].f_normal[2];
			v_normal[n_face[k].v[1]][0] += n_face[k].f_normal[0];
			v_normal[n_face[k].v[1]][1] += n_face[k].f_normal[1];
			v_normal[n_face[k].v[1]][2] += n_face[k].f_normal[2];
			v_normal[n_face[k].v[2]][0] += n_face[k].f_normal[0];
			v_normal[n_face[k].v[2]][1] += n_face[k].f_normal[1];
			v_normal[n_face[k].v[2]][2] += n_face[k].f_normal[2];
			n_face[k].mtlid = vol_mat_id;	//볼륨 fog 머터리얼 아이디..
		}
		for(k=0; k<vertex_num; k++)
		{
			if( v_normal[k][0] == 0 && v_normal[k][1] == 0 && v_normal[k][2] == 0 )
				continue;
			Normalize(v_normal[k]);
			Vector3fScale(v_normal[k],0.3f,v_normal[k]);
			n_vertex[k].local[0]+=v_normal[k][0];
			n_vertex[k].local[1]+=v_normal[k][1];
			n_vertex[k].local[2]+=v_normal[k][2];
		}
		Dfree(v_normal);
		*/
		v_fog_cnt++;
	}
	return mat;
}
