#include "Jmalloc.h"
#include "Jerror.h"
#include "BSPRenderPart.h"
#include "MeshUtil.h"


static BSPRenderPart RetMTemp;

#define _VERTEX_ARC_AREA_CHAR		0
#define _VERTEX_ARC_AREA_SHORT		00
#define _VERTEX_ARC_AREA_FLOAT		1000000


#define _MALLOC_BLOCK 10000 //현재는 만개다...
//#define _MALLOC_BLOCK 60000 //현재는 만개다...


BSPRenderPart *GetBSPRenderPart(void)
{
	return &RetMTemp;
}

//---월드좌표로 되있는 애니메이션 오브젝트를 로컬좌표로 만든다음 다시 바꿔준다.
void RestoreDynamicObjectVertex(_BSP_M_GROUP *bsp_m_group,_MESH *mesh)
{
	int i;
	float min_x=60000,max_x=-60000;
	float min_y=60000,max_y=-60000;
	float min_z=60000,max_z=-60000;

	if( bsp_m_group->object_id )
	{
		WORD id= GetObjectId(mesh,bsp_m_group->object_id);
		SetLocalCoodinate(&mesh->object[id]);
		for(i=0; i<mesh->object[id].vertexnum; i++)
		{
			if( min_x > mesh->object[id].vertex[i].x )
				min_x = mesh->object[id].vertex[i].x;
			if( min_y > mesh->object[id].vertex[i].y )
				min_y = mesh->object[id].vertex[i].y;
			if( min_z > mesh->object[id].vertex[i].z )
				min_z = mesh->object[id].vertex[i].z;
			if( max_x < mesh->object[id].vertex[i].x )
				max_x = mesh->object[id].vertex[i].x;
			if( max_y < mesh->object[id].vertex[i].y )
				max_y = mesh->object[id].vertex[i].y;
			if( max_z < mesh->object[id].vertex[i].z )
				max_z = mesh->object[id].vertex[i].z;
		}
		bsp_m_group->pos[0] = (max_x - min_x)*.5f+min_x;
		bsp_m_group->pos[1] = (max_y - min_y)*.5f+min_y;
		bsp_m_group->pos[2] = (max_z - min_z)*.5f+min_z;

		float max_gop=-1;
		max_gop = max(max_gop,max_x-bsp_m_group->pos[0]);
		max_gop = max(max_gop,max_y-bsp_m_group->pos[1]);
		max_gop = max(max_gop,max_z-bsp_m_group->pos[2]);

		bsp_m_group->scale = max_gop;	// 바로 그 값이 스케일값...
	}
}
void InvalidateDynamicObjectVertex(_BSP_M_GROUP *bsp_m_group,_MESH *mesh)
{
	if( bsp_m_group->object_id )
	{
		WORD id= GetObjectId(mesh,bsp_m_group->object_id);
		SetWorldCoodinate(&mesh->object[id]);
	}
}

//스케일 포지션 압축타입을 구한다.
static void CalculateMGroupScalePosType(_BSP_M_GROUP *bsp_m_group,_OptObject *op_bsp)
{
	//바운딩 박스의 위치를 구한다.
	bsp_m_group->pos[0] = (op_bsp->bb_max[0] - op_bsp->bb_min[0])*.5f+op_bsp->bb_min[0];
	bsp_m_group->pos[1] = (op_bsp->bb_max[1] - op_bsp->bb_min[1])*.5f+op_bsp->bb_min[1];
	bsp_m_group->pos[2] = (op_bsp->bb_max[2] - op_bsp->bb_min[2])*.5f+op_bsp->bb_min[2];

	//--------스케일링을 구한다.

	//제일 긴 한쪽길이를 구한다.
	float max_gop=-1;
	max_gop = max(max_gop,op_bsp->bb_max[0]-bsp_m_group->pos[0]);
	max_gop = max(max_gop,op_bsp->bb_max[1]-bsp_m_group->pos[1]);
	max_gop = max(max_gop,op_bsp->bb_max[2]-bsp_m_group->pos[2]);

	bsp_m_group->scale = max_gop;	// 바로 그 값이 스케일값...

	if( max_gop < _VERTEX_ARC_AREA_CHAR )
		bsp_m_group->attr|=_VERTEX_ARC_TYPE_CHAR;
	else
	if( max_gop < _VERTEX_ARC_AREA_SHORT )
		bsp_m_group->attr|=_VERTEX_ARC_TYPE_SHORT;
	else
	{
		bsp_m_group->attr|=_VERTEX_ARC_TYPE_FLOAT;
		bsp_m_group->scale = 1.0f;
	}
}
int stB_vertex_s=0;
int stW_vertex_s=0;
int stF_vertex_s=0;

static void SetVertexIndexSearchPoint()	//찾고자하는 포인트를 만들어 놓는다.
{
	stB_vertex_s = RetMTemp.BVertexNum;
	stW_vertex_s = RetMTemp.WVertexNum;
	stF_vertex_s = RetMTemp.FVertexNum;
}

//버텍스 인덱스를 얻어오고 버텍스를 쌓는다.
static int GetVertexIndex(_BSP_M_GROUP *bsp_m_group,_VERTEX *v,BOOL be_create=FALSE)
{
	int index,i;
/*
	//타잎별로 버텍스 인덱스와 버텍스를 쌓는다.
	if( bsp_m_group->attr & _VERTEX_ARC_TYPE_CHAR )
	{
		char x = (char)(((v->x-bsp_m_group->pos[0])/bsp_m_group->scale)*127);
		char y = (char)(((v->y-bsp_m_group->pos[1])/bsp_m_group->scale)*127);
		char z = (char)(((v->z-bsp_m_group->pos[2])/bsp_m_group->scale)*127);

		for(i=stB_vertex_s; i<RetMTemp.BVertexNum; i++)
		{
			if( RetMTemp.BVertex[i][0] == x 
			&& RetMTemp.BVertex[i][1] == y 
			&& RetMTemp.BVertex[i][2] == z )
			{
				return i;
			}
		}
		RetMTemp.BVertex[RetMTemp.BVertexNum][0] = x;
		RetMTemp.BVertex[RetMTemp.BVertexNum][1] = y;
		RetMTemp.BVertex[RetMTemp.BVertexNum][2] = z;
		index = RetMTemp.BVertexNum;
		RetMTemp.BVertexNum++;
		if(RetMTemp.BVertexNum >= RetMTemp.BMaxVertexNum )
		{
			 RetMTemp.BVertex = (Vector3c *)ReAlloc(RetMTemp.BVertex,RetMTemp.BMaxVertexNum*sizeof(Vector3c)
				 ,(RetMTemp.BMaxVertexNum+_MALLOC_BLOCK)*sizeof(Vector3c));
			 RetMTemp.BMaxVertexNum+=_MALLOC_BLOCK;
		}
	}
	else
	if( bsp_m_group->attr & _VERTEX_ARC_TYPE_SHORT )
	{
		short x = (short)(((v->x-bsp_m_group->pos[0])/bsp_m_group->scale)*32767);
		short y = (short)(((v->y-bsp_m_group->pos[1])/bsp_m_group->scale)*32767);
		short z = (short)(((v->z-bsp_m_group->pos[2])/bsp_m_group->scale)*32767);

		for(i=stW_vertex_s; i<RetMTemp.WVertexNum; i++)
		{
			if( RetMTemp.WVertex[i][0] == x 
			&& RetMTemp.WVertex[i][1] == y 
			&& RetMTemp.WVertex[i][2] == z )
			{
				return i;
			}
		}
		RetMTemp.WVertex[RetMTemp.WVertexNum][0] = x;
		RetMTemp.WVertex[RetMTemp.WVertexNum][1] = y;
		RetMTemp.WVertex[RetMTemp.WVertexNum][2] = z;
		index = RetMTemp.WVertexNum;
		RetMTemp.WVertexNum++;
		if(RetMTemp.WVertexNum >= RetMTemp.WMaxVertexNum )
		{
			 RetMTemp.WVertex = (Vector3s *)ReAlloc(RetMTemp.WVertex,RetMTemp.BMaxVertexNum*sizeof(Vector3s)
				 ,(RetMTemp.WMaxVertexNum+_MALLOC_BLOCK)*sizeof(Vector3s));
			 RetMTemp.WMaxVertexNum+=_MALLOC_BLOCK;
		}
	}
	else
*/
	{
		/*
		float x = (float)(((v->x-bsp_m_group->pos[0])/bsp_m_group->scale)*1.0f);
		float y = (float)(((v->y-bsp_m_group->pos[1])/bsp_m_group->scale)*1.0f);
		float z = (float)(((v->z-bsp_m_group->pos[2])/bsp_m_group->scale)*1.0f);
		*/
		float x = v->x;
		float y = v->y;
		float z = v->z;

		if( be_create == FALSE )
		{
			for(i=stF_vertex_s; i<RetMTemp.FVertexNum; i++)
			{
				if( RetMTemp.FVertex[i][0] == x 
				&& RetMTemp.FVertex[i][1] == y 
				&& RetMTemp.FVertex[i][2] == z )
				{
					return i;
				}
			}
		}
		RetMTemp.FVertex[RetMTemp.FVertexNum][0] = x;
		RetMTemp.FVertex[RetMTemp.FVertexNum][1] = y;
		RetMTemp.FVertex[RetMTemp.FVertexNum][2] = z;
		index = RetMTemp.FVertexNum;
		RetMTemp.FVertexNum++;
		if(RetMTemp.FVertexNum >= RetMTemp.FMaxVertexNum )
		{
			RetMTemp.VertexColor = (DWORD *)ReAlloc(RetMTemp.VertexColor,RetMTemp.FMaxVertexNum*sizeof(DWORD)
				 ,(RetMTemp.FMaxVertexNum+_MALLOC_BLOCK)*sizeof(DWORD));
			memset(RetMTemp.VertexColor+RetMTemp.FMaxVertexNum,0xff,sizeof(DWORD)*_MALLOC_BLOCK);
			RetMTemp.FVertex = (Vector3f *)ReAlloc(RetMTemp.FVertex,RetMTemp.FMaxVertexNum*sizeof(Vector3f)
				 ,(RetMTemp.FMaxVertexNum+_MALLOC_BLOCK)*sizeof(Vector3f));
			RetMTemp.FMaxVertexNum+=_MALLOC_BLOCK;
		}
		RetMTemp.FVertex;
	}

	return index;
}
//#define WhatAddSu(a)		( a < 0 ? -0.999f: 0.999f )
#define MinWhatAddSu(a)		( -0.999f )
#define MaxWhatAddSu(a)		( 0.999f )

// 열라 복잡하다..  조심하자...
void CreateBSPRenderPart(_BSP_M_GROUP *bsp_m_group,_MESH *m_mesh,R3BlockBsp *block_bsp)
{
#define _MAX_BASE_FACE 132768
	int i,j,k;
	LONG base_face[_MAX_BASE_FACE];

	for(i=0; i<_MAX_BASE_FACE; i++)
	{
		base_face[i] = -1;
	}

	memset(&RetMTemp,0,sizeof(BSPRenderPart));
	
//	RetMTemp.BVertex=(Vector3c *)Dmalloc(sizeof(Vector3c)*_MALLOC_BLOCK);
//	RetMTemp.WVertex=(Vector3s *)Dmalloc(sizeof(Vector3s)*_MALLOC_BLOCK);
	RetMTemp.FVertex=(Vector3f *)Dmalloc(sizeof(Vector3f)*_MALLOC_BLOCK);
	RetMTemp.VertexColor=(DWORD *)Dmalloc(sizeof(DWORD)*_MALLOC_BLOCK);
	memset(RetMTemp.VertexColor,0xff,sizeof(DWORD)*_MALLOC_BLOCK);
	RetMTemp.Face=(_BSP_FACE *)Dmalloc(sizeof(_BSP_FACE)*_MALLOC_BLOCK);
	RetMTemp.FaceId=(DWORD *)Dmalloc(sizeof(DWORD)*_MALLOC_BLOCK);
	RetMTemp.VertexId=(DWORD *)Dmalloc(sizeof(DWORD)*_MALLOC_BLOCK);
	RetMTemp.UV=(Vector2f *)Dmalloc(sizeof(Vector2f)*_MALLOC_BLOCK);
	RetMTemp.LGTUV=(Vector2s *)Dmalloc(sizeof(Vector2s)*_MALLOC_BLOCK);

	//넘치는 검사용...
	int max_vertexid_num = _MALLOC_BLOCK;
	int max_face_num = _MALLOC_BLOCK;
	RetMTemp.BMaxVertexNum = _MALLOC_BLOCK;
	RetMTemp.WMaxVertexNum = _MALLOC_BLOCK;
	RetMTemp.FMaxVertexNum = _MALLOC_BLOCK;

	_OptObject *op_bsp = block_bsp->mOptObject;
	RetMTemp.MatObjNum = block_bsp->mOptObjNum;

	for(i=0; i<block_bsp->mOptObjNum; i++)	//개수가 정확이 있다.
	{
		if( op_bsp->type & _Y_BILLBOARD )
			bsp_m_group[i].attr |= _Y_BILLBOARD;
		if( op_bsp->type & _CORONA )
			bsp_m_group[i].attr |= _CORONA;
		//스케일 포지션 압축타입을 구한다.
		CalculateMGroupScalePosType(&bsp_m_group[i],op_bsp);
		SetVertexIndexSearchPoint();
		
		//머터리얼과 라이트맵
		bsp_m_group[i].lgt_id = op_bsp->lgtid;
		bsp_m_group[i].mtl_id = op_bsp->mtlid;
		bsp_m_group[i].object_id = op_bsp->object_id;

		//바운딩박스...
		bsp_m_group[i].bb_min[0] = (short)((short)op_bsp->bb_min[0]+MinWhatAddSu(op_bsp->bb_min[0]));
		bsp_m_group[i].bb_min[1] = (short)((short)op_bsp->bb_min[1]+MinWhatAddSu(op_bsp->bb_min[1]));
		bsp_m_group[i].bb_min[2] = (short)((short)op_bsp->bb_min[2]+MinWhatAddSu(op_bsp->bb_min[2]));
		bsp_m_group[i].bb_max[0] = (short)((short)op_bsp->bb_max[0]+MaxWhatAddSu(op_bsp->bb_max[0]));
		bsp_m_group[i].bb_max[1] = (short)((short)op_bsp->bb_max[1]+MaxWhatAddSu(op_bsp->bb_max[1]));
		bsp_m_group[i].bb_max[2] = (short)((short)op_bsp->bb_max[2]+MaxWhatAddSu(op_bsp->bb_max[2]));
		bsp_m_group[i].face_start_id = RetMTemp.FaceNum; 
		bsp_m_group[i].face_num = (WORD)op_bsp->f_cnt;

		RestoreDynamicObjectVertex(&bsp_m_group[i],m_mesh);	//애니메이션 되는거는 로컬좌표로 바꾼다.
		for(j=0; j<(int)op_bsp->f_cnt; j++)
		{
			int obj_num = block_bsp->mPolyWhatObj[j+op_bsp->start_id];	//오브젝트 번호
		    _VERTEX *vertex=m_mesh->object[obj_num].vertex;				//버텍스포인터를 얻은후
			
			RetMTemp.FaceId[RetMTemp.FaceNum] = RetMTemp.FaceNum;

			RetMTemp.Face[RetMTemp.FaceNum].v_start_id = RetMTemp.VertexIdNum;	//시작 버텍스 아이디.
			RetMTemp.Face[RetMTemp.FaceNum].v_num = (WORD)block_bsp->mPolyFace[j+op_bsp->start_id]->vertex_num;
			int base_v_num;
			if( op_bsp->type & _NO_LIGHT_MAP )
			{
				if( op_bsp->i >= _MAX_BASE_FACE )
					Error("_MAX_BASE_FACE 가 넘쳤습니다.","");
				if( base_face[op_bsp->i] == -1 )
				{
					base_v_num = RetMTemp.FVertexNum;
					base_face[op_bsp->i] = base_v_num;
					RetMTemp.FVertexNum+=m_mesh->object[obj_num].vertexnum;
					
					if(RetMTemp.FVertexNum >= RetMTemp.FMaxVertexNum )
					{
						RetMTemp.VertexColor = (DWORD *)ReAlloc(RetMTemp.VertexColor,RetMTemp.FMaxVertexNum*sizeof(DWORD)
							 ,(RetMTemp.FMaxVertexNum+_MALLOC_BLOCK)*sizeof(DWORD));
						memset(RetMTemp.VertexColor+RetMTemp.FMaxVertexNum,0xff,sizeof(DWORD)*_MALLOC_BLOCK);
						RetMTemp.FVertex = (Vector3f *)ReAlloc(RetMTemp.FVertex,RetMTemp.FMaxVertexNum*sizeof(Vector3f)
							 ,(RetMTemp.FMaxVertexNum+_MALLOC_BLOCK)*sizeof(Vector3f));
						RetMTemp.FMaxVertexNum+=_MALLOC_BLOCK;
					}
					for(k=0; k<m_mesh->object[obj_num].vertexnum; k++)
					{
						RetMTemp.FVertex[k+base_v_num][0] = m_mesh->object[obj_num].vertex[k].x;
						RetMTemp.FVertex[k+base_v_num][1] = m_mesh->object[obj_num].vertex[k].y;
						RetMTemp.FVertex[k+base_v_num][2] = m_mesh->object[obj_num].vertex[k].z;
						if( m_mesh->object[obj_num].vcolor )
							RetMTemp.VertexColor[k+base_v_num] = m_mesh->object[obj_num].vcolor[k];
					}
				}
				else
					base_v_num = base_face[op_bsp->i];
			}

			//버텍스 숫자만큼 집어넣고...
			for(k=0; k<block_bsp->mPolyFace[j+op_bsp->start_id]->vertex_num; k++)
			{
				int v_id = block_bsp->mPolyFace[j+op_bsp->start_id]->vertex_id[k];
				
				if( op_bsp->type & _NO_LIGHT_MAP )
				{
					RetMTemp.VertexId[RetMTemp.VertexIdNum] = base_v_num+block_bsp->mPolyFace[j+op_bsp->start_id]->vertex_id[k];
				}
				else
					RetMTemp.VertexId[RetMTemp.VertexIdNum] = GetVertexIndex(&bsp_m_group[i],&vertex[v_id]);
				RetMTemp.UV[RetMTemp.VertexIdNum][0] = block_bsp->mPolyFace[j+op_bsp->start_id]->fu[k];
				RetMTemp.UV[RetMTemp.VertexIdNum][1] = block_bsp->mPolyFace[j+op_bsp->start_id]->fv[k];
				RetMTemp.LGTUV[RetMTemp.VertexIdNum][0] = (short)(block_bsp->mPolyFace[j+op_bsp->start_id]->lu[k]*32767);
				RetMTemp.LGTUV[RetMTemp.VertexIdNum][1] = (short)(block_bsp->mPolyFace[j+op_bsp->start_id]->lv[k]*32767);
				RetMTemp.VertexIdNum++;

				if( RetMTemp.VertexIdNum >= max_vertexid_num )	//버텍스인덱스 버퍼가 넘치면..
				{
					 RetMTemp.VertexId= (DWORD*)ReAlloc(RetMTemp.VertexId,max_vertexid_num*sizeof(DWORD),(max_vertexid_num+_MALLOC_BLOCK)*sizeof(DWORD));
					 RetMTemp.UV= (Vector2f *)ReAlloc(RetMTemp.UV,max_vertexid_num*sizeof(Vector2f),(max_vertexid_num+_MALLOC_BLOCK)*sizeof(Vector2f));
					 RetMTemp.LGTUV= (Vector2s *)ReAlloc(RetMTemp.LGTUV,max_vertexid_num*sizeof(Vector2s),(max_vertexid_num+_MALLOC_BLOCK)*sizeof(Vector2s));
					 max_vertexid_num+=_MALLOC_BLOCK;
				}
			}
			RetMTemp.FaceNum++;
			if( RetMTemp.FaceNum >= max_face_num )		//면 버퍼가 넘치면..
			{
				 RetMTemp.Face = (_BSP_FACE*)ReAlloc(RetMTemp.Face,max_face_num*sizeof(_BSP_FACE),(max_face_num+_MALLOC_BLOCK)*sizeof(_BSP_FACE));
				 RetMTemp.FaceId = (DWORD*)ReAlloc(RetMTemp.FaceId,max_face_num*sizeof(DWORD),(max_face_num+_MALLOC_BLOCK)*sizeof(DWORD));
				 max_face_num += _MALLOC_BLOCK;
			}
		}
		InvalidateDynamicObjectVertex(&bsp_m_group[i],m_mesh);//애니메이션 되는거는 다시 월드좌표로 바꾼다.
		op_bsp = op_bsp->next;
	}

}

void ReleaseBSPRenderPart(void)
{
	Dfree(RetMTemp.LGTUV);
	Dfree(RetMTemp.UV);
	Dfree(RetMTemp.BVertex);
	Dfree(RetMTemp.WVertex);
	Dfree(RetMTemp.FVertex);
	Dfree(RetMTemp.Face);
	Dfree(RetMTemp.FaceId);
	Dfree(RetMTemp.VertexId);
}
