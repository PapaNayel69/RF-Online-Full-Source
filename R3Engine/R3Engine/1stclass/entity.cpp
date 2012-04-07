#include <windows.h>
#include "stdio.h"
#include "entity.h"
#include "jerror.h"
#include "jmalloc.h"
#include "commonutil.h"
#include "R3material.h"
#include "R3render.h"
#include "R3math.h"
#include "R3d3dtex.h"
#include "Core.h"
#include "R3Particle.h"

CEntity::CEntity()
{
	mVetexBufferSize=0;
	mIsEntityLoad=FALSE;
	mTextureSize=0;
	mMat=NULL;
	mFrame=0;	//내부 프래임.
	mFlag=0;
	mIndependencyTex=0;
}
CEntity::~CEntity()
{
}

int CEntity::GetUsedVertexBufferSize()
{
	return mVetexBufferSize;
}

void CEntity::GetAnimationMatrix(float get_ani_mat[4][4],_ANI_OBJECT *obj,float now_frame)
{
	::GetAniMatrix(get_ani_mat,obj,now_frame);
}
void ExtractVertex(WORD type,int num,Vector3f *vertex,void *comp_vertex,Vector3f pos,float scale)
{
	int i;
	Vector3c *b_buffer;
	Vector3s *s_buffer;
//	Vector3f *f_buffer;

	if( type & _VERTEX_ARC_TYPE_CHAR )
	{
		b_buffer = (Vector3c *)comp_vertex;

		for(i=0; i<num; i++)
		{
			vertex[i][0]=(b_buffer[i][0]/127.0f)*scale+pos[0];
			vertex[i][1]=(b_buffer[i][1]/127.0f)*scale+pos[1];
			vertex[i][2]=(b_buffer[i][2]/127.0f)*scale+pos[2];
		}
	}
	else
	if( type & _VERTEX_ARC_TYPE_SHORT )
	{
		s_buffer = (Vector3s *)comp_vertex;

		for(i=0; i<num; i++)
		{
			vertex[i][0]=(s_buffer[i][0]/32767.0f)*scale+pos[0];
			vertex[i][1]=(s_buffer[i][1]/32767.0f)*scale+pos[1];
			vertex[i][2]=(s_buffer[i][2]/32767.0f)*scale+pos[2];
		}
	}
	else
	{
		memcpy(vertex,comp_vertex,sizeof(Vector3f)*num);
	}
}

void CEntity::AddFlag(DWORD flag)	//상태 플래그를 추가한다.
{
	mFlag |= flag;
}


BOOL CEntity::LoadEntity(char *filename,DWORD option)
{
	float version=_ENTITY_VERSION;
	int i,j;
	WORD k;
	int buf_size;
	BYTE *load_buf;
	Vector3f *vertex;
	Vector2s *uv;
	void *comp_vertex;
	DWORD *color;
	WORD *f_id,*v_id;
	_READ_ENTITY_FACE *entity_face;
	_READ_ENTITY_M_GROUP *read_entity_m_group;
	_READ_ANI_OBJECT *read_object;		//다이나믹 리드 오브젝트.
	BYTE *track;						//애니메이션 트랙.
	_ENTITY_COMP_HEADER *comp_header;
	struct{
		DWORD version;
		DWORD Identity;
	}MiniHeader;

	mMapColor = 0xff808080;
	mStartTime=0;
	mFlag=option;
	FILE *fp = fopen(filename,"rb");
	if( fp == NULL )
	{
		Warning(filename,"<-이 파일이 안보인다.");
		return FALSE;
	}
	fread(&MiniHeader,sizeof(DWORD)*2,1,fp);
/*
	if( MiniHeader.Identity == _IDENTITY_PARTICLE)	//이 엔티티는 파티클이다.
	{
		_R3ENGINE_STATE *state = GetR3State();
		char r_name[256];
		_SAVE_PARTICLE s_particle;

		strcpy(r_name,state->EntityPath);
		fread(&s_particle,sizeof(_SAVE_PARTICLE),1,fp);	//파티클 정보를 싹 가져온다.
		mParticle = new CParticle;
		mParticle->CopySaveParticleToParticle(&s_particle);
		mParticle->InitParticle();
		mParticle->SetParticleState(_PARTICLE_STATE_START);
		
		fclose(fp);
		strcat(r_name,s_particle.mEntityName);
		LoadEntity(r_name,_LOAD_POLY_IN_MEMORY);	//파티클이므로 메모리에 일단 얹는다.
		return TRUE;
	}
*/	
	if( version != MiniHeader.version )
	{
		Warning(filename,"파일 버젼이 이전버젼입니다.");
		fclose(fp);
		return FALSE;
	}
	fread(&mHeader,sizeof(_ENTITY_FILE_HEADER),1,fp);

	DWORD load_option;
	if( option & _LOAD_POLY_IN_MEMORY )	//메모리에 얹어야될경우..
	{
		load_option=_USE_MEMORY_VP;
		mIsUseMemory =TRUE;
	}
	else
		load_option=_USE_STATIC_VP;

	mStartTexID = GetNowR3D3DTexCnt();	//현재 텍스쳐 카운트를 리턴
	int org_r3tex_num = GetNowR3TexCnt();			//현재 r3텍스쳐 카운트

	if( mFlag & _DO_NOT_MANAGE_TEXTURE )	//텍스쳐 메니지를 안쓰는 경우.
	{
		SetR3TexManageFlag(_R3TEX_MANAGE_FLAG_NO_SAME_CHECK);	//같은이름텍스쳐를 쓰는경우를 무시하고 따로 올린다.
	}

	DWORD old_tex_size=GetNowTexMemSize();
	
	char r3m_path[256];
	strcpy(r3m_path,filename);
	StripEXT(r3m_path);
	strcat(r3m_path,".r3m");
	if( IsExistFile(r3m_path) )
	{
		mMat = LoadSubR3M(r3m_path);

		StripEXT(r3m_path);
		strcat(r3m_path,".r3t");
		R3Texture *r3tex = R3GetTexInfoR3T(r3m_path,0);	//R3T텍스쳐 정보를 리턴한다.
		if( r3tex != NULL )
		{
			//r3tex = R3GetTexInfoR3T(r3m_path,0);	//R3T텍스쳐 정보를 리턴한다.

			if( r3tex->mSameCnt == 0 )	//같은텍스쳐가 있으면 로딩하문 안된다.
			{
				LoadR3T(r3tex);
				mTexNum = GetNowR3D3DTexCnt()-mStartTexID;	//현재 텍스쳐 개수를 알아내자.
			}
			else
			{
				AdjustIndependenceR3M(mMat,mStartTexID,r3tex->mStartID);
				mStartTexID = r3tex->mStartID;
				mTexNum = r3tex->mTexNum;	//
			}
		}
	}
	else
	{
		//---머터리얼이 먼저 로딩되야...
		char pmat_path[256];
		strcpy(pmat_path,filename);
		StripName(pmat_path);

		mMat = LoadSubMaterial(pmat_path);
		if( mMat == NULL )
		{
			fclose(fp);
			return FALSE;
		}
		mTexNum = GetNowR3D3DTexCnt()-mStartTexID;	//현재 텍스쳐 개수를 알아내자.
	}

	if( mFlag & _DO_NOT_MANAGE_TEXTURE )	//일단 텍스쳐 메니지를 쓰나 확인..
	{
		//R3 텍스쳐 매니저를 안쓰는 방법은 일단 로딩시켜놓고 텍스쳐포인터를 가져온다..
		//그것으로 다룬뒤.. 나중에 따로 릴리즈하는 방법이다...
		//상당히 좋지않은데... 담에 구조 바꾸거나 새로운 엔진땐 반드시 참조할거다....
		for(i=0; i<mTexNum; i++)	//일단 텍스쳐를 올린다.
		{
			R3LoadTextureMem(mStartTexID+i);	//특정 텍스쳐를 텍스쳐메모리에 얹는다.
		}
		LPDIRECT3DTEXTURE8 *ppTex;
		ppTex=(LPDIRECT3DTEXTURE8 *)Dmalloc(mTexNum*sizeof(LPDIRECT3DTEXTURE8 *));
		for( i=0; i<mTexNum; i++)
		{
			ppTex[i]=R3GetSurface(mStartTexID+i);
		}
		mIndependencyTex = ppTex;
		SetNowR3D3DTexCnt(mStartTexID); //무쟈게 주의% 모르고 쓰면 책임못짐...
		SetNowR3TexCnt(org_r3tex_num);	//r3텍스쳐 주의, 모르고 쓰면 책임못짐...
		//mStartTexID=0;
		SetR3TexManageFlag(0);	//같은이름텍스쳐를 쓰는경우를 무시하고 따로 올린다.
	}
	mTextureSize += (GetNowTexMemSize()-old_tex_size);

	//---그리고 로딩...
	mIsEntityLoad=TRUE;

	buf_size=0;
	buf_size+=mHeader.CompHeader.size;
	buf_size+=mHeader.Vertex.size;
	buf_size+=mHeader.VColor.size;
	buf_size+=mHeader.UV.size;
	buf_size+=mHeader.Face.size;
	buf_size+=mHeader.FaceId.size;
	buf_size+=mHeader.VertexId.size;
	buf_size+=mHeader.MatGroup.size;
	buf_size+=mHeader.Object.size;
	buf_size+=mHeader.Track.size;
	
	load_buf=(BYTE *)Dmalloc(buf_size);

	buf_size=0;

	comp_header=(_ENTITY_COMP_HEADER *)&load_buf[buf_size];
	buf_size+=mHeader.CompHeader.size;

	comp_vertex = (void *)&load_buf[buf_size];
	buf_size+=mHeader.Vertex.size;

	color = (DWORD *)&load_buf[buf_size];
	buf_size+=mHeader.VColor.size;

	uv = (Vector2s *)&load_buf[buf_size];
	buf_size+=mHeader.UV.size;

	entity_face = (_READ_ENTITY_FACE *)&load_buf[buf_size];
	buf_size+=mHeader.Face.size;

	f_id = (WORD *)&load_buf[buf_size];
	buf_size+=mHeader.FaceId.size;

	v_id = (WORD *)&load_buf[buf_size];
	buf_size+=mHeader.VertexId.size;

	read_entity_m_group = (_READ_ENTITY_M_GROUP *)&load_buf[buf_size];
	buf_size+=mHeader.MatGroup.size;
	
	read_object = (_READ_ANI_OBJECT *)&load_buf[buf_size];
	buf_size+=mHeader.Object.size;

	track = (BYTE *)&load_buf[buf_size];
	buf_size+=mHeader.Track.size;

	fread(comp_header,mHeader.CompHeader.size,1,fp);
	fread(comp_vertex,mHeader.Vertex.size,1,fp);
	fread(color,mHeader.VColor.size,1,fp);
	fread(uv,mHeader.UV.size,1,fp);
	fread(entity_face,mHeader.Face.size,1,fp);
	fread(f_id,mHeader.FaceId.size,1,fp);
	fread(v_id,mHeader.VertexId.size,1,fp);
	fread(read_entity_m_group,mHeader.MatGroup.size,1,fp);
	fread(read_object,mHeader.Object.size,1,fp);
	fread(track,mHeader.Track.size,1,fp);
	fclose(fp);

	int divide,vertex_num;
	if( comp_header->type == _VERTEX_ARC_TYPE_CHAR )
	{
		divide = 4;
		vertex_num = mHeader.Vertex.size/sizeof(Vector3c);
	}
	else
	if( comp_header->type == _VERTEX_ARC_TYPE_SHORT )
	{
		divide = 2;
		vertex_num = mHeader.Vertex.size/sizeof(Vector3s);
	}
	else
	{
		divide = 1;
		vertex_num = mHeader.Vertex.size/sizeof(Vector3f);
	}

	vertex = (Vector3f *)Dmalloc(vertex_num*sizeof(Vector3f));//압축풀 버텍스 버퍼.

	ExtractVertex(comp_header->type,vertex_num,vertex,comp_vertex
		,comp_header->pos,comp_header->scale);

	int static_trinum=0;
	int static_vertexnum=0;
	// 오파나 반투명 계산을 제외한 삼각형을 구한다.
	for(i=0; i< (int)(mHeader.MatGroup.size/sizeof(_READ_ENTITY_M_GROUP)); i++)
	{
		for(j=0; j<read_entity_m_group[i].face_num; j++)
		{
			DWORD face_id = f_id[read_entity_m_group[i].face_start_id+j];
			static_trinum+=(entity_face[face_id].v_num-2);
			static_vertexnum += entity_face[face_id].v_num;
		}
	}
	mVetexBufferSize=static_vertexnum*sizeof(_D3DR3VERTEX_TEX1);
	mVetexBufferSize+=static_trinum*3*sizeof(WORD);

	mMatGroupNum=(int)(mHeader.MatGroup.size/sizeof(_READ_ENTITY_M_GROUP));
	mObjectNum = mHeader.Object.size/sizeof(_READ_ANI_OBJECT);
	//----------실제 쓸것들에 대한 메모리 할당.(멜록수를 간략히 줄여주자.)

//	int fsdfsdf = ((mMatGroupNum)*sizeof(_ENTITY_M_GROUP)
//		+mObjectNum*sizeof(_ANI_OBJECT)+mHeader.Track.size+static_vertexnum*sizeof(Vector2f));
	mMatGroup = (_ENTITY_M_GROUP*)Dmalloc((mMatGroupNum)*sizeof(_ENTITY_M_GROUP)
		+mObjectNum*sizeof(_ANI_OBJECT)+mHeader.Track.size+static_vertexnum*sizeof(Vector2f));
	mObject=(_ANI_OBJECT *)((BYTE*)mMatGroup+(mMatGroupNum)*sizeof(_ENTITY_M_GROUP));
	mTrack=(BYTE *)mObject+mObjectNum*sizeof(_ANI_OBJECT);
	mOrgUV=(Vector2f*)((BYTE*)mTrack+mHeader.Track.size);

	memcpy(mTrack,track,mHeader.Track.size);	//트랙은 그대로 복사
	ConvAniObject(mObjectNum,mTrack,read_object,mObject);		//트랙인덱스를 포인터로 바꿔주자. 리드오브젝트 저장하는곳으로..

	if( option & _USE_VERTEX_SHADER )	//버텍스 쉐이더를 사용하면.
		mStaticVertexBuffer.InitVertexBuffer( static_vertexnum*sizeof(_D3DR3VERTEX_TEX1),load_option,0 );
	else
		mStaticVertexBuffer.InitVertexBuffer( static_vertexnum*sizeof(_D3DR3VERTEX_TEX1),load_option,D3DFVF_R3VERTEX_TEX1 );
	mStaticIndexedBuffer.InitIndexBuffer(static_trinum*3*sizeof(WORD),load_option);

	_D3DR3VERTEX_TEX1 *static_vertexbp;
	WORD *static_indexbp;

	static_vertexbp=(_D3DR3VERTEX_TEX1 *)mStaticVertexBuffer.VPLock();
	static_indexbp =(WORD *)mStaticIndexedBuffer.VPLock();

	if( static_vertexbp==NULL || static_indexbp==NULL )	//버텍스 버퍼와 인덱스 버퍼를 생성못했다.
	{
		ReleaseEntity();
		Dfree(vertex);
		Dfree(load_buf);
		mIsEntityLoad = FALSE;
		return TRUE;
	}

	//버텍스 버퍼에 쌓자.....  머터리얼그룹과 UV 버퍼에 쌓자.
	int static_v_index_cnt=0;
	WORD static_v_cnt=0;

	float uv_scale = (comp_header->uv_max-comp_header->uv_min)/2.0f;
	float uv_pos = comp_header->uv_min+uv_scale;

	mBBMin[0] = 67000	,mBBMin[1] = 67000	,mBBMin[2] = 67000;
	mBBMax[0] = -67000	,mBBMax[1] = -67000	,mBBMax[2] = -67000;

	mIsAlpha=FALSE;
	BOOL IsLightEnable=0;
	for(i=0; i< mMatGroupNum; i++)
	{
		mMatGroup[i].BBMin[0] = read_entity_m_group[i].bb_min[0];
		mMatGroup[i].BBMin[1] = read_entity_m_group[i].bb_min[1];
		mMatGroup[i].BBMin[2] = read_entity_m_group[i].bb_min[2];
		mMatGroup[i].BBMax[0] = read_entity_m_group[i].bb_max[0];
		mMatGroup[i].BBMax[1] = read_entity_m_group[i].bb_max[1];
		mMatGroup[i].BBMax[2] = read_entity_m_group[i].bb_max[2];
		mMatGroup[i].MtlId = read_entity_m_group[i].mtl_id;
		mMatGroup[i].MultiSourceUV = &mOrgUV[static_v_cnt];
		mMatGroup[i].ObjectId = read_entity_m_group[i].object_id;
		mBBMin[0]=min(mBBMin[0],mMatGroup[i].BBMin[0]);
		mBBMin[1]=min(mBBMin[1],mMatGroup[i].BBMin[1]);
		mBBMin[2]=min(mBBMin[2],mMatGroup[i].BBMin[2]);
		mBBMax[0]=max(mBBMax[0],mMatGroup[i].BBMax[0]);
		mBBMax[1]=max(mBBMax[1],mMatGroup[i].BBMax[1]);
		mBBMax[2]=max(mBBMax[2],mMatGroup[i].BBMax[2]);

		int temp_f_num;
		int temp_v_num;
		mMatGroup[i].Type = _MAT_GROUP_TYPE_STANDARD;	//현재는 스탠다드만...
		if( mMat[read_entity_m_group[i].mtl_id].m_Layer[0].m_dwAlphaType != _BLEND_NONE )
		{
			mMatGroup[i].Type |= _MAT_GROUP_TYPE_OPPA;
			mIsAlpha=TRUE;
		}


		DWORD alpha;
		if( read_entity_m_group[i].mtl_id == -1 )
		{
			alpha = 0xff000000;
			IsLightEnable=0;
		}
		else
		{
			alpha=mMat[mMatGroup[i].MtlId].m_Layer[0].m_ARGB&0xff000000;
			if( mMat[mMatGroup[i].MtlId].m_dwFlag & _MAT_FLAG_LIGHT_MAP )
				IsLightEnable=TRUE;
			else
				IsLightEnable=FALSE;
			if( alpha&0xff000000 != 0xff000000 )
				mIsAlpha=TRUE;
		}
		mMatGroup[i].VBMinIndex = static_v_cnt;
		mMatGroup[i].IBMinIndex = static_v_index_cnt;

		temp_f_num = static_v_index_cnt/3;
		temp_v_num = static_v_cnt;
		for(j=0; j<read_entity_m_group[i].face_num; j++)
		{
			WORD face_id = f_id[read_entity_m_group[i].face_start_id+j];
			for(k=0; k<entity_face[face_id].v_num-2; k++)	//인덱스 포인터를 채운다.
			{
				static_indexbp[static_v_index_cnt++]=static_v_cnt;
				static_indexbp[static_v_index_cnt++]=static_v_cnt+k+1;
				static_indexbp[static_v_index_cnt++]=static_v_cnt+k+2;
			}

			for(k=0; k<entity_face[face_id].v_num; k++)
			{
				DWORD vr_id = entity_face[face_id].v_start_id+k;

				static_vertexbp[static_v_cnt].x = vertex[v_id[vr_id]][0];
				static_vertexbp[static_v_cnt].y = vertex[v_id[vr_id]][1];
				static_vertexbp[static_v_cnt].z = vertex[v_id[vr_id]][2];

				mOrgUV[static_v_cnt][0]=((uv[vr_id][0]/32767.0f)*uv_scale)+uv_pos;
				mOrgUV[static_v_cnt][1]=((uv[vr_id][1]/32767.0f)*uv_scale)+uv_pos;
				static_vertexbp[static_v_cnt].u = mOrgUV[static_v_cnt][0];	//텍스쳐uv
				static_vertexbp[static_v_cnt].v = mOrgUV[static_v_cnt][1];
//				static_vertexbp[static_v_cnt].color =0x00ffffff;
				if( IsLightEnable )
					static_vertexbp[static_v_cnt].color =0xff808080;
				else
					static_vertexbp[static_v_cnt].color =color[v_id[vr_id]];
				static_vertexbp[static_v_cnt].color |= alpha;
				static_v_cnt++;
			}
			mMatGroup[i].VCnt = static_v_cnt - temp_v_num;	//요 머터리얼그룹의 버텍스 갯수..
			mMatGroup[i].TriNum = static_v_index_cnt/3-temp_f_num;	//요머터리얼의 tri수
		}
	}

	mStaticVertexBuffer.VPUnLock();
	mStaticIndexedBuffer.VPUnLock();
	Dfree(vertex);
	Dfree(load_buf);

//	RestoreTexMem();	//텍스쳐 메모리에 올린다.
	return TRUE;
}



void CEntity::ReleaseEntity()
{
	if( mIsEntityLoad == FALSE )
		return;
/*
	if( mParticle )	//이 엔티티는 파티클이다.
	{
		mParticle->ReleaseParticle();
		delete mParticle;
		mParticle=NULL;
	}*/
	mIsEntityLoad=FALSE;
	mVetexBufferSize=0;
	mTextureSize=0;
	mObjectNum=0;

	mStaticVertexBuffer.ReleaseVertexBuffer();
	mStaticIndexedBuffer.ReleaseIndexBuffer();
//	ReleaseTexMem();	//텍스쳐 메모리에 내린다.
	if( mMatGroup )
	{
		Dfree(mMatGroup);
		mMatGroup=0;
	}
	if( mMat )
	{
		ReleaseSubMaterial(mMat);
		mMat=NULL;
	}

	if( mIndependencyTex )	//텍스쳐가 남아있네..
	{
		if( mFlag & _DO_NOT_MANAGE_TEXTURE )	//일단 맞나 확인..
		{
			ReleaseTexMem();	//텍스쳐 메모리에 내린다.
		}
		mIndependencyTex=NULL;
	}
}

void CEntity::PrepareAnimation()	//애니메이션 하기전에 준비할것들..
{
	int i;
	if( !mObject )
		return;
	for(i=0; i<mObjectNum; i++)
	{
		mObject[i].AniFrameCache = 0;
	}
}
void CEntity::RestoreTexMem()	//텍스쳐 메모리에 올린다.
{
	if( mFlag & _DO_NOT_MANAGE_TEXTURE )	//일단 텍스쳐 메니지를 쓰나 확인..
		return;
	DWORD old_tex_size=GetNowTexMemSize();
	for(int i=0; i<mTexNum; i++)
	{
		R3LoadTextureMem(mStartTexID+i);	//특정 텍스쳐를 텍스쳐메모리에 얹는다.
	}
	mTextureSize += (GetNowTexMemSize()-old_tex_size);
}

void CEntity::ReleaseTexMem()	//텍스쳐 메모리에 내린다.
{
	int i;

	if( mFlag & _DO_NOT_MANAGE_TEXTURE )	//일단 텍스쳐 메니지를 쓰나 확인..
	{
		LPDIRECT3DTEXTURE8 *ppTex;
		ppTex=(LPDIRECT3DTEXTURE8 *)mIndependencyTex;
		for( i=0; i<mTexNum; i++)
		{
			if( ppTex[i] )
				ppTex[i]->Release();
		}
		Dfree(mIndependencyTex);
		mIndependencyTex=NULL;
		return;
	}
	for( i=0; i<mTexNum; i++)
	{
		R3ReleaseTextureMem(mStartTexID+i);	//특정 텍스쳐를 텍스쳐메모리에 얹는다.
	}
}

BOOL CEntity::IsFirstAlpha()  //첫번째머터리얼그룹이 알파인가? 엔티티 소팅이 필요.
{
	if ( mMatGroupNum <= 0 || mMat == NULL )
		return FALSE;
	if( mMat[0].m_Layer[0].m_dwAlphaType != _BLEND_NONE )
		return TRUE;
	return FALSE;
}
