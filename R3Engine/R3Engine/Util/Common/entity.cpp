#include "entity.h"
#include "jmalloc.h"
#include "jerror.h"
#include "patch.h"
#include "r3bsputil.h"
#include "meshutil.h"
#include <stdio.h>


_ENTITY_FILE_HEADER Header;
//-------------------------엔티티 출력(하늘)---------------------------------------------
static WORD GetDynamicObjectId(_MESH *mesh,int n_id)
{
	int i;

	WORD cnt=0;
	for(i=0 ;i<n_id+1; i++)
	{
		if( mesh->object[i].type & _DYNAMIC_OBJECT)
			cnt++;
		if( i==n_id+1)
			break;
	}
	return cnt;
}
static WORD GetObjectId(_MESH *mesh,int d_id)
{
	int i;

	WORD cnt=0;
	for(i=0 ;i<mesh->objectnum; i++)
	{
		if( mesh->object[i].type & _DYNAMIC_OBJECT)
			cnt++;
		if( cnt==d_id)
			break;
	}
	return i;
}


//---월드좌표로 되있는 애니메이션 오브젝트를 로컬좌표로 만든다음 다시 바꿔준다.
void RestoreDynamicObjectVertex(_OBJECT *object)
{
	SetLocalCoodinate(object);
}
void InvalidateDynamicObjectVertex(_OBJECT *object)
{
	SetWorldCoodinate(object);
}

#define _VERTEX_ARC_TYPE_CHAR		0x8000
#define _VERTEX_ARC_TYPE_SHORT		0x4000
#define _VERTEX_ARC_TYPE_FLOAT		0x2000

//일단 최소한 short형으로 압축을 한다.
#define _VERTEX_ARC_AREA_CHAR		18
#define _VERTEX_ARC_AREA_SHORT		30000
#define _VERTEX_ARC_AREA_FLOAT		1000000

WORD GetArcTypeNumber(_MESH *mesh,Vector3f get_pos,float *get_scale)
{
	int i,k;
	float min_x=60000,max_x=-60000;
	float min_y=60000,max_y=-60000;
	float min_z=60000,max_z=-60000;

	for(k=0; k<mesh->objectnum; k++)
	{
		for(i=0; i<mesh->object[k].vertexnum; i++)
		{
			if( min_x > mesh->object[k].vertex[i].x )
				min_x = mesh->object[k].vertex[i].x;
			if( min_y > mesh->object[k].vertex[i].y )
				min_y = mesh->object[k].vertex[i].y;
			if( min_z > mesh->object[k].vertex[i].z )
				min_z = mesh->object[k].vertex[i].z;
			if( max_x < mesh->object[k].vertex[i].x )
				max_x = mesh->object[k].vertex[i].x;
			if( max_y < mesh->object[k].vertex[i].y )
				max_y = mesh->object[k].vertex[i].y;
			if( max_z < mesh->object[k].vertex[i].z )
				max_z = mesh->object[k].vertex[i].z;
		}
	}
	get_pos[0] = (max_x - min_x)*.5f+min_x;
	get_pos[1] = (max_y - min_y)*.5f+min_y;
	get_pos[2] = (max_z - min_z)*.5f+min_z;

	float max_gop=-1;
	max_gop = max(max_gop,max_x-get_pos[0]);
	max_gop = max(max_gop,max_y-get_pos[1]);
	max_gop = max(max_gop,max_z-get_pos[2]);

	*get_scale = max_gop;	// 바로 그 값이 스케일값...

	if( max_gop < _VERTEX_ARC_AREA_CHAR )
		return _VERTEX_ARC_TYPE_CHAR;
	else
	if( max_gop < _VERTEX_ARC_AREA_SHORT )
		return _VERTEX_ARC_TYPE_SHORT;
	else
		return _VERTEX_ARC_TYPE_FLOAT;
}
void GetBBox(_MESH *mesh,Vector3s bb_min,Vector3s bb_max)
{
	int i,k;
	float min_x=60000,max_x=-60000;
	float min_y=60000,max_y=-60000;
	float min_z=60000,max_z=-60000;

	for(k=0; k<mesh->objectnum; k++)
	{
		for(i=0; i<mesh->object[k].vertexnum; i++)
		{
			if( min_x > mesh->object[k].vertex[i].x )
				min_x = mesh->object[k].vertex[i].x;
			if( min_y > mesh->object[k].vertex[i].y )
				min_y = mesh->object[k].vertex[i].y;
			if( min_z > mesh->object[k].vertex[i].z )
				min_z = mesh->object[k].vertex[i].z;
			if( max_x < mesh->object[k].vertex[i].x )
				max_x = mesh->object[k].vertex[i].x;
			if( max_y < mesh->object[k].vertex[i].y )
				max_y = mesh->object[k].vertex[i].y;
			if( max_z < mesh->object[k].vertex[i].z )
				max_z = mesh->object[k].vertex[i].z;
		}
	}
	bb_min[0] = (short)min_x;
	bb_max[0] = (short)max_x;
	bb_min[1] = (short)min_y;
	bb_max[1] = (short)max_y;
	bb_min[2] = (short)min_z;
	bb_max[2] = (short)max_z;
}

void *GetCompVertex(int *num,WORD type,_MESH *mesh,Vector3f pos,float scale)
{
	int v_size=0,i,k,cnt=0;
	Vector3c *b_buffer;
	Vector3s *s_buffer;
	Vector3f *f_buffer;

	for(i=0; i<mesh->objectnum; i++)
	{
		v_size += mesh->object[i].vertexnum;
	}

	if( type & _VERTEX_ARC_TYPE_CHAR )
	{
		*num = sizeof(Vector3c)*v_size;
		b_buffer=(Vector3c *)Dmalloc(*num);
		for(k=0; k<mesh->objectnum; k++)
		{
			for(i=0; i<mesh->object[k].vertexnum; i++)
			{
				b_buffer[cnt][0]=(char)(((mesh->object[k].vertex[i].local[0]-pos[0])/scale)*127);
				b_buffer[cnt][1]=(char)(((mesh->object[k].vertex[i].local[1]-pos[1])/scale)*127);
				b_buffer[cnt][2]=(char)(((mesh->object[k].vertex[i].local[2]-pos[2])/scale)*127);
				cnt++;
			}
		}
		return (void *)b_buffer;
	}
	else
	if( type &  _VERTEX_ARC_TYPE_SHORT )
	{
		*num = sizeof(Vector3s)*v_size;
		s_buffer=(Vector3s *)Dmalloc(*num);
		for(k=0; k<mesh->objectnum; k++)
		{
			for(i=0; i<mesh->object[k].vertexnum; i++)
			{
				s_buffer[cnt][0]=(short)(((mesh->object[k].vertex[i].local[0]-pos[0])/scale)*32767);
				s_buffer[cnt][1]=(short)(((mesh->object[k].vertex[i].local[1]-pos[1])/scale)*32767);
				s_buffer[cnt][2]=(short)(((mesh->object[k].vertex[i].local[2]-pos[2])/scale)*32767);
				cnt++;
			}
		}
		return (void *)s_buffer;
	}
	else
	{
		*num = sizeof(Vector3f)*v_size;
		f_buffer=(Vector3f *)Dmalloc(*num);
		v_size=0;
		for(k=0; k<mesh->objectnum; k++)
		{
			memcpy(&f_buffer[v_size],mesh->object[k].vertex,mesh->object[k].vertexnum*sizeof(Vector3f));
			v_size += mesh->object[i].vertexnum;
		}
		return (void *)f_buffer;
	}
}

void ReleaseCompVertex(void *v_buffer)
{
	Dfree(v_buffer);
}

void EntityExport(char *name,_MESH *mesh,_MATERIAL *mat,R3BlockBsp *block_mat)
{
	int i,j,k,l,v_id_num,v_buffer_size;
	DWORD r,g,b,all_face_num,all_vertex_num;

//---------------세이브 사전 준비

	all_face_num=0;
	all_vertex_num=0;
	for(k=0; k<mesh->objectnum; k++)
	{
		all_face_num += mesh->object[k].facenum;
		all_vertex_num += mesh->object[k].vertexnum;
	}
//	DWORD *vertex=(Vector3f *)Dmalloc(sizeof(Vector3f)*all_vertex_num);
	WORD *v_id=(WORD *)Dmalloc(sizeof(WORD)*all_face_num*5);
	WORD *f_id=(WORD *)Dmalloc(sizeof(WORD)*all_face_num);
	Vector2s *uv=(Vector2s *)Dmalloc(sizeof(Vector2s)*all_face_num*5);
	_ENTITY_FACE *entity_face = (_ENTITY_FACE*)Dmalloc(all_face_num*sizeof(_ENTITY_FACE));
	DWORD *color = (DWORD *)Dmalloc(all_vertex_num*sizeof(DWORD));
	_ENTITY_COMP_HEADER comp_header;

	_LFACE **lf = GetLFace();

	//바운딩박스를 구한다.
	GetBBox(mesh,comp_header.bb_min,comp_header.bb_max);

	//다이나믹 오브텍트 로컬 버텍스로...
	for(k=0; k<mesh->objectnum; k++)	
	{
		if( mesh->object[k].type & _DYNAMIC_OBJECT)
			RestoreDynamicObjectVertex(&mesh->object[k]);
	}
	//압축 타입과 위치,스케일을 구한다.
	comp_header.type = GetArcTypeNumber(mesh,comp_header.pos,&comp_header.scale);
	void *comp_vertex = GetCompVertex(&v_buffer_size,comp_header.type,mesh,comp_header.pos,comp_header.scale);

	//---------버텍스 컬러를 계산한다.
	int nu_vertex=0;
	for(k=0; k<mesh->objectnum; k++)
	{
		for(i=0; i<mesh->object[k].facenum; i++)
		{
			for(j=0; j<mesh->object[k].p_face[i].vertex_num; j++)
			{
				_WINDING *winding=&lf[k][i].patch->winding[j];
	//			winding->energy[0] = min(1.0f,winding->energy[0]);
	//			winding->energy[1] = min(1.0f,winding->energy[1]);
	//			winding->energy[2] = min(1.0f,winding->energy[2]);
				r = (DWORD)(winding->energy[0]);
				g = (DWORD)(winding->energy[1]);
				b = (DWORD)(winding->energy[2]);
				color[mesh->object[k].p_face[i].vertex_id[j]+nu_vertex] = (0xff<<24)|(r<<16)|(g<<8)|(b<<0);
			}
		}
		nu_vertex+=mesh->object[k].vertexnum;
	}

	//uv 압축할거를 먼저 준비 한다.
	_ENTITY_M_GROUP *entity_m_group=(_ENTITY_M_GROUP *)Dmalloc(sizeof(_ENTITY_M_GROUP)*(block_mat->mOptObjNum+1));
	memset(entity_m_group,0,sizeof(_ENTITY_M_GROUP)*(block_mat->mOptObjNum+1));

	_OptObject *opt_object = block_mat->mOptObject;
	comp_header.uv_min=30000;
	comp_header.uv_max=-30000;
	for(i=0; i<block_mat->mOptObjNum; i++)
	{
		for(j=0; j<(int)opt_object->f_cnt; j++)
		{
			_POLYFACE *p_face = block_mat->mPolyFace[j+opt_object->start_id];
			for(l=0; l<p_face->vertex_num; l++)
			{
				comp_header.uv_min = min(p_face->fu[l],comp_header.uv_min);
				comp_header.uv_min = min(p_face->fv[l],comp_header.uv_min);
				comp_header.uv_max = max(p_face->fu[l],comp_header.uv_max);
				comp_header.uv_max = max(p_face->fv[l],comp_header.uv_max);
			}
		}
		opt_object = opt_object->next;
	}

	//--머터리얼그룹을 정리한다.
	float uv_scale = (comp_header.uv_max-comp_header.uv_min)/2.0f;
	float uv_pos = comp_header.uv_min+uv_scale;
	
	opt_object = block_mat->mOptObject;
	int face_id_num=0;
	int face_num=0;
	v_id_num=0;
	for(i=0; i<block_mat->mOptObjNum; i++)
	{
		entity_m_group[i].face_num=(WORD)opt_object->f_cnt;
		entity_m_group[i].mtl_id=opt_object->mtlid;
		entity_m_group[i].face_start_id = face_num;

		for(j=0; j<(int)opt_object->f_cnt; j++)
		{
			int k = block_mat->mPolyWhatObj[j+opt_object->start_id];	//오브젝트 번호
			f_id[face_num]=face_num;

			nu_vertex=0;
			for(l=0; l<k; l++)
				nu_vertex+=mesh->object[l].vertexnum;

			_POLYFACE *p_face = block_mat->mPolyFace[j+opt_object->start_id];
			entity_face[face_num].v_num=(WORD)p_face->vertex_num;
			entity_face[face_num].v_start_id=v_id_num;
			for(l=0; l<entity_face[face_num].v_num; l++)
			{
				v_id[v_id_num] = p_face->vertex_id[l]+nu_vertex;
				uv[v_id_num][0] = (short)(((p_face->fu[l]-uv_pos)/uv_scale)*32767);
				uv[v_id_num][1] = (short)(((p_face->fv[l]-uv_pos)/uv_scale)*32767);
				v_id_num++;
				if(v_id_num>=65535)
				{
					Error("엔티티는 65535이상의 버텍스를 수용 안합니다.","");
				}
			}
			face_num++;
			if(face_num>=65535)
			{
				Error("엔티티는 face가 65535이상이면 안됩니다.","");
			}
		}

		entity_m_group[i].object_id = opt_object->object_id;
		entity_m_group[i].bb_min[0] = (short)MinFixFloatToInt(opt_object->bb_min[0]);
		entity_m_group[i].bb_min[1] = (short)MinFixFloatToInt(opt_object->bb_min[1]);
		entity_m_group[i].bb_min[2] = (short)MinFixFloatToInt(opt_object->bb_min[2]);
		entity_m_group[i].bb_max[0] = (short)MaxFixFloatToInt(opt_object->bb_max[0]);
		entity_m_group[i].bb_max[1] = (short)MaxFixFloatToInt(opt_object->bb_max[1]);
		entity_m_group[i].bb_max[2] = (short)MaxFixFloatToInt(opt_object->bb_max[2]);

		opt_object = opt_object->next;
	}

//----다이나믹 오브젝트 설정.
	int d_obj_num,track_size;

	_D_OBJECT *object = AllocDObject(mesh);	//다이나믹 오브젝트 버퍼 설정.
	BYTE *ani_track = AllocTrack(mesh);			// 애니메이션 트랙설정.
	d_obj_num=GetDObjectNum(mesh);
	track_size=GetTrackSize(mesh);

	StoreDObjectAndTrack(object,ani_track,mesh);

	//미니 해더.
	struct{
		DWORD version;
		DWORD Identity;
	}MiniHeader;
	MiniHeader.version =_ENTITY_VERSION;
	MiniHeader.Identity = 0;	// 스카이박스나 엔티티는 항상 0이다. 파티클일때 틀리다.
//---------------- 헤더 준비.
	int offset=sizeof(_ENTITY_FILE_HEADER)+sizeof(DWORD)*2;

	Header.CompHeader.offset = offset;
	Header.CompHeader.size = sizeof(_ENTITY_COMP_HEADER);
	offset+=Header.CompHeader.size;

	Header.Vertex.offset = offset;
	Header.Vertex.size = v_buffer_size;//sizeof(Vector3f)*all_vertex_num;
	offset+=Header.Vertex.size;

	Header.VColor.offset = offset;
	Header.VColor.size = sizeof(DWORD)*all_vertex_num;
	offset+=Header.VColor.size;

	Header.UV.offset = offset;
	Header.UV.size = sizeof(Vector2s)*v_id_num;
	offset+=Header.UV.size;

	Header.Face.offset = offset;
	Header.Face.size = sizeof(_ENTITY_FACE)*all_face_num;
	offset+=Header.Face.size;

	Header.FaceId.offset = offset;
	Header.FaceId.size = sizeof(WORD)*all_face_num;
	offset+=Header.FaceId.size;

	Header.VertexId.offset = offset;
	Header.VertexId.size = sizeof(WORD)*v_id_num;
	offset+=Header.VertexId.size;

	Header.MatGroup.offset = offset;
	Header.MatGroup.size = sizeof(_ENTITY_M_GROUP)*block_mat->mOptObjNum;
	offset+=Header.MatGroup.size;

	//------------다이나믹 오브젝트 엔트리 
	Header.Object.size = sizeof(_D_OBJECT)*d_obj_num;
	Header.Object.offset=offset;
	offset += Header.Object.size;

	//------------트랙 엔트리 
	Header.Track.size = track_size;
	Header.Track.offset=offset;
	offset += Header.Track.size;

	FILE *fp = fopen(name,"wb");

	fwrite(&MiniHeader,sizeof(DWORD)*2,1,fp);
	fwrite(&Header,sizeof(_ENTITY_FILE_HEADER),1,fp);
	fwrite(&comp_header,Header.CompHeader.size,1,fp);

	fwrite(comp_vertex,Header.Vertex.size,1,fp);
//	for(k=0; k<mesh->objectnum; k++)
//		fwrite(mesh->object[k].vertex,mesh->object[k].vertexnum*sizeof(Vector3f),1,fp);

	fwrite(color,Header.VColor.size,1,fp);
	fwrite(uv,Header.UV.size,1,fp);
	fwrite(entity_face,Header.Face.size,1,fp);
	fwrite(f_id,Header.FaceId.size,1,fp);
	fwrite(v_id,Header.VertexId.size,1,fp);
	fwrite(entity_m_group,Header.MatGroup.size,1,fp);
	//-엔티티 애니메이션.
	fwrite(object,Header.Object.size,1,fp);
	fwrite(ani_track,Header.Track.size,1,fp);
	
	fclose(fp);

	FreeDObject( object );
	FreeTrack( ani_track );
	Dfree(color);
	Dfree(v_id);
	Dfree(f_id);
	Dfree(uv);
	Dfree(entity_face);
	Dfree(entity_m_group);

	for(k=0; k<mesh->objectnum; k++)	//다이나믹 오브텍트 버텍스 좌표 환원.
	{
		if( mesh->object[k].type & _DYNAMIC_OBJECT)
			InvalidateDynamicObjectVertex(&mesh->object[k]);
	}
}

//다이나믹 오브젝트에 관한거....
static WORD GetDynamicObjectId(_MESH *mesh,char *name)
{
	int i;

	WORD cnt=0;
	if( name[0] == NULL )
		return 0;
	for(i=0 ;i<mesh->objectnum; i++)
	{
		if( mesh->object[i].type & _DYNAMIC_OBJECT)
			cnt++;
		if( !strcmp( mesh->object[i].objectname,name ) )
			break;
	}
	return cnt;
}

int	GetDObjectNum(_MESH *m_mesh)
{
	int d_obj_num=0,i;
	for(i=0; i<m_mesh->objectnum; i++)
	{
		if( m_mesh->object[i].type & _DYNAMIC_OBJECT )
			d_obj_num++;
	}
	return d_obj_num;
}


int GetTrackSize(_MESH *m_mesh)
{
	int i;
	int track_size=0;		// 옵셋

	for(i=0; i<m_mesh->objectnum; i++)
	{
		if( m_mesh->object[i].type & _DYNAMIC_OBJECT )
		{
			track_size+=sizeof(_SCALE_TRACK)*m_mesh->object[i].Scale_cnt;
			track_size+=sizeof(_POS_TRACK)*m_mesh->object[i].Pos_cnt;
			track_size+=sizeof(_ROT_TRACK)*m_mesh->object[i].Rot_cnt;
		}
	}
	return track_size;
}

_D_OBJECT *AllocDObject(_MESH *m_mesh)
{
	int d_obj_num=0,i;
	for(i=0; i<m_mesh->objectnum; i++)
	{
		if( m_mesh->object[i].type & _DYNAMIC_OBJECT )
			d_obj_num++;
	}
	_D_OBJECT *object = (_D_OBJECT *)Dmalloc(sizeof(_D_OBJECT)*d_obj_num);

	return object;
}

void FreeDObject(_D_OBJECT *object)
{
	if( object )
		Dfree(object);
}

BYTE *AllocTrack(_MESH *m_mesh)
{
	int i;

	int d_obj_num=0;
	int track_size=0;		// 옵셋
	for(i=0; i<m_mesh->objectnum; i++)
	{
		if( m_mesh->object[i].type & _DYNAMIC_OBJECT )
		{
			track_size+=sizeof(_SCALE_TRACK)*m_mesh->object[i].Scale_cnt;
			track_size+=sizeof(_POS_TRACK)*m_mesh->object[i].Pos_cnt;
			track_size+=sizeof(_ROT_TRACK)*m_mesh->object[i].Rot_cnt;
			d_obj_num++;
		}
	}
	BYTE *ani_track	 = (BYTE *)Dmalloc(track_size);
	return ani_track;
}

void FreeTrack(BYTE *track)
{
	Dfree(track);
}

void StoreDObjectAndTrack(_D_OBJECT *object,BYTE *ani_track,_MESH *m_mesh)
{
	int i;
	int d_obj_num=0;
	int track_size=0;		// 옵셋

	for(i=0; i<m_mesh->objectnum; i++)
	{
		if( m_mesh->object[i].type & _DYNAMIC_OBJECT )
		{
//			memcpy(&object[d_obj_num].d3d_s_matrix,&m_mesh->object[i].d3d_s_matrix,sizeof(D3DMATRIX));
//			object[d_obj_num].is_loop = m_mesh->object[i].
			object[d_obj_num].parent = GetDynamicObjectId(m_mesh,m_mesh->object[i].parentname);
			Vector4fCopy(m_mesh->object[i].quat,object[d_obj_num].quat);
			Vector3fCopy(m_mesh->object[i].pos,object[d_obj_num].pos);
			Vector3fCopy(m_mesh->object[i].scale,object[d_obj_num].scale);
			Vector4fCopy(m_mesh->object[i].scale_quat,object[d_obj_num].scale_quat);

			object[d_obj_num].flag = (WORD)m_mesh->object[i].type;
			object[d_obj_num].frames = m_mesh->object[i].end_frame;
			object[d_obj_num].Pos_cnt = m_mesh->object[i].Pos_cnt;
			object[d_obj_num].pos_offset = track_size;
			memcpy(&ani_track[track_size],m_mesh->object[i].Pos,sizeof(_POS_TRACK)*object[d_obj_num].Pos_cnt);
			track_size+=sizeof(_POS_TRACK)*object[d_obj_num].Pos_cnt;

			object[d_obj_num].Rot_cnt = m_mesh->object[i].Rot_cnt;
			object[d_obj_num].rot_offset  = track_size;
			memcpy(&ani_track[track_size],m_mesh->object[i].Rot,sizeof(_ROT_TRACK)*object[d_obj_num].Rot_cnt);
			track_size+=sizeof(_ROT_TRACK)*object[d_obj_num].Rot_cnt;

			object[d_obj_num].Scale_cnt = m_mesh->object[i].Scale_cnt;
			object[d_obj_num].scale_offset  = track_size;
			memcpy(&ani_track[track_size],m_mesh->object[i].Scale,sizeof(_SCALE_TRACK)*object[d_obj_num].Scale_cnt);
			track_size+=sizeof(_SCALE_TRACK)*object[d_obj_num].Scale_cnt;
			d_obj_num++;
		}
	}
}