#include <stdio.h>
#include "Jerror.h"
#include "Jmalloc.h"
#include "CommonUtil.h"
#include "R3D3d8.h"
#include "R3Bsp.h"
#include "R3Math.h"
#include "R3Util.h"
#include "Core.h"

#include "R3Render.h"
#include "RenderParticle.h"
#include "R3Alpha.h"
#include "R3Material.h"
#include "MultiLayer.h"
#include "R3D3dTex.h"

#include "Collision.h"
#include "R3EngineKernel.h"
#include "R3Sound.h"

#pragma optimize( "atp", on )

#define _CHAR_HEIGHT		20.0f
#define _VIEW_DIST			((6000.f*1.8f)/_CHAR_HEIGHT)

CBsp::ClearVariable()	//변수 클리어
{
	mStaticAlloc=NULL;
	mStaticAllocSize=0;
	mExtBspStaticAlloc=NULL;
	mExtBspStaticAllocSize=0;

	mMatGroupCacheSize=0;
	mMatGroupCache=NULL;
	mMultiLayerUV=NULL;
	mStaticVBCnt=0;
	mCFVertexNum=0;			//	충돌 페이스 점.
	mCFLineNum=0;			//	충돌 페이스 라인
	mCFLineIdNum=0;			//	충돌 페이스 아이디 리스트개수.
	mEnvEntity=NULL;
	mEntityCache=NULL;		//엔티티 캐쉬 할 버퍼다.
	mEntityCacheSize=0;		//맵 엔티티 캐쉬 사이즈 
	mNowCFaceId=0;
	mCNEdgeNormal = NULL;
	mEntity=NULL;
	mParticle=NULL;
	mEntityListNum=0;
	mLeafEntityListNum=0;
	mEntityIDNum=0;
	mMapEntitiesListNum=0;
	mSoundEntityIDNum=0;
	mLeafSoundEntityListNum=0;
	mSoundEntityListNum=0;		//사운드 원본 갯수.
	mSoundEntitiesListNum=0;	//복사판들 갯수.
	mSoundEntityCache=NULL;		//사운드 엔티티 캐쉬 할 버퍼다.
	mSoundEntityCacheSize=0;		//사운드 엔티티 캐쉬 사이즈 
	mEventObjectID=NULL;

	mPickPoly=-1;
	mNowLeafNum=0;
	mEnvID=0;
	memset(&mBSPHeader,0,sizeof( _BSP_FILE_HEADER));
	memset(&mExtBSPHeader,0,sizeof( _EXT_BSP_FILE_HEADER));	
}


CBsp::CBsp()
{
//	memset(this,0,sizeof(CBsp));	//이거 못쓰는 이유는 bsp 안의 클래스가 null로 잡힌다. 
	ClearVariable();
}
CBsp::~CBsp()	//BSP 릴리즈
{
}

_BSP_MAT_GROUP *CBsp::GetMatGroup()
{
	return mMatGroup;
}

void *CBsp::GetDynamicVertexBuffer()
{
//	return mDynamicVertexBuffer;
	return NULL;
}

void CBsp::GetVertexNormal()
{
/*
	DWORD i,j;
	DWORD *f_cnt=(DWORD*)Dmalloc(bsp->vertex_num*sizeof(DWORD));
	memset(f_cnt,0,bsp->vertex_num*sizeof(DWORD));

	memset(bsp->v_normal,0,bsp->vertex_num*sizeof(Vector3f));

	for(i=0; i<bsp->face_num; i++)
	{
		DWORD v;
		for(j=0; j<bsp->face[i].vertex_num; j++)
		{
			v = bsp->vertex_id[bsp->face[i].vertex_start_id+j];
			bsp->v_normal[v][0] += bsp->f_normal[bsp->face[i].f_normal_id][0];
			bsp->v_normal[v][1] += bsp->f_normal[bsp->face[i].f_normal_id][1];
			bsp->v_normal[v][2] += bsp->f_normal[bsp->face[i].f_normal_id][2];
			f_cnt[v]++;
		}
	}
	for(i=0; i<bsp->vertex_num; i++)
	{
		if( f_cnt[i] == 0 )
		{
//			Warning("노는 vertex가 있군..","");
			bsp->v_normal[i][0] =0;
			bsp->v_normal[i][1] =0;
			bsp->v_normal[i][2] =1;
			continue;
		}
//		bsp->v_normal[i][0] /=(float)f_cnt[i];
//		bsp->v_normal[i][1] /=(float)f_cnt[i];
//		bsp->v_normal[i][2] /=(float)f_cnt[i];
		Normalize(bsp->v_normal[i]);	//노멀라이즈
	}
	Dfree(f_cnt);
*/
}

void GetVertexFromBVertex(Vector3f get,void *vertex,_BSP_READ_M_GROUP *readmatgroup)
{
	get[0] = ((float)((Vector3cp)vertex)[0]/127.0f)*readmatgroup->scale + readmatgroup->pos[0];
	get[1] = ((float)((Vector3cp)vertex)[1]/127.0f)*readmatgroup->scale + readmatgroup->pos[1];
	get[2] = ((float)((Vector3cp)vertex)[2]/127.0f)*readmatgroup->scale + readmatgroup->pos[2];
}

void GetVertexFromWVertex(Vector3f get,void *vertex,_BSP_READ_M_GROUP *readmatgroup)
{
	get[0] = ((float)((Vector3sp)vertex)[0]/32767.0f)*readmatgroup->scale + readmatgroup->pos[0];
	get[1] = ((float)((Vector3sp)vertex)[1]/32767.0f)*readmatgroup->scale + readmatgroup->pos[1];
	get[2] = ((float)((Vector3sp)vertex)[2]/32767.0f)*readmatgroup->scale + readmatgroup->pos[2];
}
void GetVertexFromFVertex(Vector3f get,void *vertex,_BSP_READ_M_GROUP *readmatgroup)
{
	get[0] = ((Vector3fp)vertex)[0];
	get[1] = ((Vector3fp)vertex)[1];
	get[2] = ((Vector3fp)vertex)[2];
}

//함수 포인터..
void (*GetVertexFromCompress[])(Vector3f get,void *bvertex,_BSP_READ_M_GROUP *readmatgroup)
	={NULL,GetVertexFromBVertex,GetVertexFromWVertex,NULL,GetVertexFromFVertex};

 DWORD stColorTable[] = {
	0xff00ff00,0xffff0000,0xff0000ff,0xffffff00,0xff00ffff,
	0xffff00ff,0xff1fff0f,0xffffffff,0xff4fff8f,0xff4f004f,
 };

void CBsp::OnlyStoreCollisionStructure(_BSP_READ_M_GROUP *readmatgroup,
	Vector3c *bvertex,Vector3s *wvertex,Vector3f *fvertex,DWORD *vertexid
	,_BSP_READ_FACE *face,DWORD *faceid )
{
	int i,j,k;

	int	function_id;
	void *vertex;
	float w_matrix[4][4];
	int base_char_index=mBSPHeader.BVertex.size/sizeof(Vector3c);
	int base_word_index=base_char_index+mBSPHeader.WVertex.size/sizeof(Vector3s);
	int static_v_cnt=0;
	_R3MATERIAL *mat=GetMainMaterial();

	for(i=0; i< (int)(mBSPHeader.ReadMatGroup.size/sizeof(_BSP_READ_M_GROUP)); i++)
	{
		if( _VERTEX_ARC_TYPE_CHAR & readmatgroup[i].attr )
		{
			vertex = (void*)bvertex;
			function_id=1;
		}
		else
		if( _VERTEX_ARC_TYPE_SHORT & readmatgroup[i].attr )
		{
			vertex = (void*)wvertex;
			function_id=2;
		}
		else
		{
			vertex = (void*)fvertex;
			function_id=4;
		}
		if( readmatgroup[i].object_id )	//애니메이션이 있을때
		{
			mObject[readmatgroup[i].object_id-1].now_frame=0;
			::GetObjectMatrix(w_matrix,readmatgroup[i].object_id,mObject,R3GetTime()*30.0f);
		}

		for(j=0; j<readmatgroup[i].face_num; j++)
		{
			DWORD f_id = faceid[readmatgroup[i].face_start_id+j];
			DWORD real_v_index;
			if( function_id == 2 )
				real_v_index = base_char_index;
			else
			if( function_id == 3 )
				real_v_index = base_word_index;
			else
				real_v_index = 0;
			mCFace[f_id].Attr = 0;
			mCFace[f_id].VNum = (BYTE)face[f_id].v_num;
			mCFace[f_id].VStartId = static_v_cnt;//face[f_id].v_start_id;
			mCFace[f_id].MatGIndex = (WORD)i;

			for(k=0; k<face[f_id].v_num; k++)
			{
				DWORD v_id = face[f_id].v_start_id+k;
				Vector3f t_v;
				
				//압축된 x ,y ,z를 풀어서 채운다.
				GetVertexFromCompress[function_id](t_v,(void*)((BYTE *)vertex+vertexid[v_id]*function_id*3),&readmatgroup[i]);

				LONG c_v_id = real_v_index+vertexid[v_id];
				//충돌 점들...
				if( readmatgroup[i].object_id )	//애니메이션이 있을때
				{
					mCVertex[c_v_id][0]=w_matrix[0][0]*t_v[0]+w_matrix[1][0]*t_v[1]+w_matrix[2][0]*t_v[2]+w_matrix[3][0];
					mCVertex[c_v_id][1]=w_matrix[0][1]*t_v[0]+w_matrix[1][1]*t_v[1]+w_matrix[2][1]*t_v[2]+w_matrix[3][1];
					mCVertex[c_v_id][2]=w_matrix[0][2]*t_v[0]+w_matrix[1][2]*t_v[1]+w_matrix[2][2]*t_v[2]+w_matrix[3][2];
				}
				else
					Vector3fCopy(t_v,mCVertex[c_v_id]);
				mCVertexId[static_v_cnt]=c_v_id;
				static_v_cnt++;
			}
			// 면 노멀을 구하자..
			GetNormal(mCFace[f_id].Normal
				,mCVertex[real_v_index+vertexid[face[f_id].v_start_id]]
				,mCVertex[real_v_index+vertexid[face[f_id].v_start_id+1]]
				,mCVertex[real_v_index+vertexid[face[f_id].v_start_id+2]]);
//			if( mCFace[f_id].Normal[1] >= 0)  //0.5 )	//약 30도 아랬것들은 죄
//				mCFace[f_id].Attr|=_CFACE_ATTR_FLOOR;
			if( readmatgroup[i].mtl_id != -1 && mat )
			{
				if( mat[readmatgroup[i].mtl_id].m_Layer[0].m_dwFlag & _MAT_NO_COLLISON )
					mCFace[f_id].Attr|=_CFACE_NO_COLLISION;
			}
		}
	}
}

void CBsp::MakeEdgeNormal()
{
	if( !mCNEdgeNormal )
	{
		Warning("MakeEdgeNormal()는 서버모드 일때만 써야됨.","");
		return;
	}
	DWORD i,j;
	for(i=0; i<mCFaceNum; i++)
	{
		Vector3f mid;
		mid[0]=0;		mid[1]=0;		mid[2]=0;
		for(j=0; j<mCFace[i].VNum; j++)
		{
			Vector3fp v = mCVertex[mCVertexId[mCFace[i].VStartId+j]];
			mid[0]+=v[0];
			mid[1]+=v[1];
			mid[2]+=v[2];
		}
		mid[0] /= mCFace[i].VNum;	//페이스의 중앙점을 구함.
		mid[1] /= mCFace[i].VNum;
		mid[2] /= mCFace[i].VNum;

		Vector4f edge_n;
		for(j=0; j<mCFace[i].VNum; j++)
		{
			Vector3fp v0 = mCVertex[mCVertexId[mCFace[i].VStartId+j]];
			Vector3fp v1 = mCVertex[mCVertexId[mCFace[i].VStartId+(j+1)%mCFace[i].VNum]];
			float l = v1[0] - v0[0];
			float m = v1[1] - v0[1];
			float n = v1[2] - v0[2];
			if( (l*l+m*m+n*n) == 0 )	//이런경우가 발생하면 안되지만..
			{
				mCNEdgeNormal[4*i+j][0] = 0;
				mCNEdgeNormal[4*i+j][1] = 0;
				mCNEdgeNormal[4*i+j][2] = 1;
				mCNEdgeNormal[4*i+j][3] = 0;
				continue;
			}
			float t = -((  l*(v0[0]-mid[0]) 
				+ m*(v0[1]-mid[1]) 
				+ n*(v0[2]-mid[2]) )/(l*l+m*m+n*n));
			
			edge_n[0] = mid[0] - (l*t + v0[0]);
			edge_n[1] = mid[1] - (m*t + v0[1]);
			edge_n[2] = mid[2] - (n*t + v0[2]);

#define _ON_EPSILON		0.01
			if( fabs(edge_n[0]) < _ON_EPSILON && fabs(edge_n[1]) < _ON_EPSILON&& fabs(edge_n[2]) < _ON_EPSILON )
			{	//물론 이런 경우도 생기면 안된다.
				mCNEdgeNormal[4*i+j][0] = 0;
				mCNEdgeNormal[4*i+j][1] = 0;
				mCNEdgeNormal[4*i+j][2] = 1;
				mCNEdgeNormal[4*i+j][3] = 0;
				continue;
			}
			
			//Vector3fNeg(edge_n,edge_n);
			Normalize(edge_n);

			mCNEdgeNormal[4*i+j][0]=edge_n[0];
			mCNEdgeNormal[4*i+j][1]=edge_n[1];
			mCNEdgeNormal[4*i+j][2]=edge_n[2];
			mCNEdgeNormal[4*i+j][3]=edge_n[0]*v1[0]+edge_n[1]*v1[1]+edge_n[2]*v1[2];
		}
//		mCNEdgeNormal[4*i+j];
	}
}

//월드의 렌더링 할것들을 죄 버텍스 버퍼에 쌓는다.
void CBsp::ReadDynamicDataFillVertexBuffer(FILE *fp)	//bsp에서 파일로읽고 버려야할것들.
{
	int read_alloc_size=0;
	BYTE *read_alloc_buf;
	float w_matrix[4][4];

	read_alloc_size += mBSPHeader.BVertex.size;
	read_alloc_size += mBSPHeader.WVertex.size;
	read_alloc_size += mBSPHeader.FVertex.size;
	read_alloc_size += mBSPHeader.VertexColor.size;
	read_alloc_size += mBSPHeader.UV.size;
	read_alloc_size += mBSPHeader.LgtUV.size;
	read_alloc_size += mBSPHeader.Face.size;
	read_alloc_size += mBSPHeader.FaceId.size;
	read_alloc_size += mBSPHeader.VertexId.size;
	read_alloc_size += mBSPHeader.ReadMatGroup.size;
/*
	read_alloc_size += mBSPHeader.MapEntitiesList.size;
	read_alloc_size += mBSPHeader.SoundEntityList.size;
	read_alloc_size += mBSPHeader.SoundEntitiesList.size;
*/
	read_alloc_buf=(BYTE *)Dmalloc(read_alloc_size);

	//-----한번 읽고 정리해서 저장 해야될것들...
	Vector3c *bvertex;		//	byte 점
	Vector3s *wvertex;		//	word 점
	Vector3f *fvertex;		//	float점
	Vector2f *uv;			// uv좌표
	Vector2s *lgtuv;		// 라이트맵 uv좌표	//WORD형이다.
	_BSP_READ_FACE *face;		//	면
	DWORD *faceid;		// 면 인덱스리스트
	DWORD *vertexid;	// 점 인덱스리스트
	DWORD *v_color;		// 버텍스 컬러.
	_BSP_READ_M_GROUP *readmatgroup;	//	머터리얼 그룹..	읽기용으로 쓸거다..

	int size=0;
	bvertex = (Vector3c *)&read_alloc_buf[size];
	fread(bvertex,mBSPHeader.BVertex.size,1,fp);
	size += mBSPHeader.BVertex.size;

	wvertex = (Vector3s *)&read_alloc_buf[size];
	fread(wvertex,mBSPHeader.WVertex.size,1,fp);
	size += mBSPHeader.WVertex.size;

	fvertex = (Vector3f *)&read_alloc_buf[size];
	fread(fvertex,mBSPHeader.FVertex.size,1,fp);
	size += mBSPHeader.FVertex.size;

//-----------버텍스 노멀을 위한 버퍼용 좌표들.
	
	int v_normal_size=(mBSPHeader.FVertex.size/sizeof(Vector3f)
		+mBSPHeader.BVertex.size/sizeof(Vector3c) 
		+mBSPHeader.WVertex.size/sizeof(Vector3s))*sizeof(Vector3f);
	Vector3f *v_normal =(Vector3f *)Dmalloc( v_normal_size );
	memset(v_normal,0, v_normal_size );

	v_color = (DWORD *)&read_alloc_buf[size];
	fread(v_color,mBSPHeader.VertexColor.size,1,fp);
	size += mBSPHeader.VertexColor.size;

	uv = (Vector2f *)&read_alloc_buf[size];
	fread(uv,mBSPHeader.UV.size,1,fp);
	size += mBSPHeader.UV.size;

	lgtuv = (Vector2s *)&read_alloc_buf[size];
	fread(lgtuv,mBSPHeader.LgtUV.size,1,fp);
	size += mBSPHeader.LgtUV.size;

	face = (_BSP_READ_FACE *)&read_alloc_buf[size];
	fread(face,mBSPHeader.Face.size,1,fp);
	size += mBSPHeader.Face.size;

	faceid = (DWORD *)&read_alloc_buf[size];
	fread(faceid,mBSPHeader.FaceId.size,1,fp);
	size += mBSPHeader.FaceId.size;

	vertexid = (DWORD *)&read_alloc_buf[size];
	fread(vertexid,mBSPHeader.VertexId.size,1,fp);
	size += mBSPHeader.VertexId.size;

	readmatgroup = (_BSP_READ_M_GROUP *)&read_alloc_buf[size];
	fread(readmatgroup,mBSPHeader.ReadMatGroup.size,1,fp);
	size += mBSPHeader.ReadMatGroup.size;
/*
	readMapEntitiesList=(_READ_MAP_ENTITIES_LIST *)&read_alloc_buf[size];
	fread(readMapEntitiesList,mBSPHeader.MapEntitiesList.size,1,fp);
	size += mBSPHeader.MapEntitiesList.size;

	readSoundEntityList=(_READ_SOUND_ENTITY_LIST *)&read_alloc_buf[size];
	fread(readSoundEntityList,mBSPHeader.SoundEntityList.size,1,fp);
	size += mBSPHeader.SoundEntityList.size;

	readSoundEntitiesList=(_READ_SOUND_ENTITIES_LIST *)&read_alloc_buf[size];
	fread(readSoundEntitiesList,mBSPHeader.SoundEntitiesList.size,1,fp);
	size += mBSPHeader.SoundEntitiesList.size;
*/
	int i,j,k;

	if( IsServerMode() )	//서버모드가 때..
	{
		OnlyStoreCollisionStructure(readmatgroup,bvertex,wvertex
			,fvertex,vertexid,face,faceid );
		MakeEdgeNormal();//서버모드일때 
		Dfree(read_alloc_buf);
		mEntityListNum=0;		//서버에서는 엔티티나 사운드를 전부 꺼버리자..
		mLeafEntityListNum=0;
		mEntityIDNum=0;
		mMapEntitiesListNum=0;
		mSoundEntityIDNum=0;
		mLeafSoundEntityListNum=0;
		mSoundEntityListNum=0;		//사운드 원본 갯수.
		mSoundEntitiesListNum=0;	//복사판들 갯수.
		return;
	}

	int static_vertexnum = 0;
	int static_trinum = 0;

	_R3MATERIAL *mat=GetMainMaterial();
	int uv_num=0,st_num=0;
	DWORD vp_state=_USE_STATIC_VP;
	// 오파나 반투명 계산을 제외한 삼각형을 구한다.
	for(i=0; i< (int)(mBSPHeader.ReadMatGroup.size/sizeof(_BSP_READ_M_GROUP)); i++)
	{
		int old_v_num = static_vertexnum;
		for(j=0; j<readmatgroup[i].face_num; j++)
		{
			DWORD f_id = faceid[readmatgroup[i].face_start_id+j];
			static_trinum+=(face[f_id].v_num-2);
			static_vertexnum += face[f_id].v_num;
		}
		if( static_vertexnum > 65535)
		{
			static_vertexnum-=old_v_num;
			mVBVertexNum[mStaticVBCnt]=old_v_num;
			old_v_num=0;
			mStaticVBCnt++;
			if( mStaticVBCnt >= _MAX_VB_CNT )
				Error("_MAX_VB_CNT를 넘겼습니다.","");
		}	
		DWORD mflag=0;
		if( readmatgroup[i].mtl_id != -1 )
		{
			for(j=0; j<(int)mat[readmatgroup[i].mtl_id].m_dwLayerNum; j++)
			{
				mflag|=mat[readmatgroup[i].mtl_id].m_Layer[j].m_dwFlag;
			}
		}

		//-------------버텍스버퍼를 스태틱으로 잡느냐 다이나믹으로 잡느냐 결정...
//		if( ( mflag & _UV_METAL )&&( mflag & (_UV_SCROLL_U|_UV_SCROLL_V|_UV_ROTATE|_UV_SCALE|_UV_LAVA) ))
//			Error("uv계열과 metal은 같이 쓸수 없습니다.","");

		if( mflag & _UV_METAL )
		{
			st_num+=(static_vertexnum-old_v_num);
			uv_num+=(static_vertexnum-old_v_num);
			//mflag &= (~_UV_METAL);
			//vp_state=_USE_DYNAMIC_VP;
		}

		if( mflag & _UV_LAVA )	//나머지는 uv다
		{
			vp_state=_USE_DYNAMIC_VP;
		}
		if(mflag)
		{
			uv_num+=(static_vertexnum-old_v_num);
		}
	}
	mVBVertexNum[mStaticVBCnt]=static_vertexnum;
	mStaticVBCnt++;

	//주의! 멀티레이어 필요요소와 같이 alloc해 버린다.
	mMultiLayerUV = (WORD *)Dmalloc(uv_num*sizeof(Vector2f)+st_num*sizeof(Vector3f)*3);
	mMultiLayerST = (BYTE*)mMultiLayerUV+uv_num*sizeof(Vector2f);
	//--버텍스 버퍼 쫙...

	DWORD t_vsize=GetTotalVertexBufferSize();
	for(i=0; i<(int)mStaticVBCnt; i++)
	{
		mStaticVertexBuffer[i].InitVertexBuffer( mVBVertexNum[i] * sizeof(_D3DR3VERTEX_TEX2),vp_state,D3DFVF_R3VERTEX_TEX2 );
	}
	mStaticIndexedBuffer.InitIndexBuffer(static_trinum*3*sizeof(WORD));

	mVertexBufferSize = GetTotalVertexBufferSize()-t_vsize;

	_D3DR3VERTEX_TEX2 *static_vertexbp;
	WORD *static_indexbp;

	int static_vb_cnt=0;
	static_vertexbp=(_D3DR3VERTEX_TEX2 *)mStaticVertexBuffer[static_vb_cnt].VPLock();
	static_indexbp =(WORD *)mStaticIndexedBuffer.VPLock();
	int static_v_index_cnt=0;
	int static_v_cnt=0;
	int	function_id;
	void *vertex;
	uv_num=0;
	st_num=0;
	int all_static_v_cnt=0;

	static_vertexnum=0;
	int base_index=0;
	int base_char_index=mBSPHeader.BVertex.size/sizeof(Vector3c);
	int base_word_index=base_char_index+mBSPHeader.WVertex.size/sizeof(Vector3s);
	for(i=0; i< (int)(mBSPHeader.ReadMatGroup.size/sizeof(_BSP_READ_M_GROUP)); i++)
	{
		//--임시코드
		mMatGroup[i].BBMin[0] = readmatgroup[i].bb_min[0];
		mMatGroup[i].BBMin[1] = readmatgroup[i].bb_min[1];
		mMatGroup[i].BBMin[2] = readmatgroup[i].bb_min[2];
		mMatGroup[i].BBMax[0] = readmatgroup[i].bb_max[0];
		mMatGroup[i].BBMax[1] = readmatgroup[i].bb_max[1];
		mMatGroup[i].BBMax[2] = readmatgroup[i].bb_max[2];
		mMatGroup[i].MtlId = readmatgroup[i].mtl_id;
		mMatGroup[i].LgtId = readmatgroup[i].lgt_id;
		mMatGroup[i].ObjectId = readmatgroup[i].object_id;
		mMatGroup[i].CoronaAlpha=0;	//템프 클리어..
		Vector3f t_pos;	//바운딩 박스 중심을 구한다.
		Vector3fSub(mMatGroup[i].BBMax,mMatGroup[i].BBMin,t_pos);
		Vector3fDiv(t_pos,2,t_pos);
		Vector3fAdd(t_pos,mMatGroup[i].BBMin,mMatGroup[i].Origin);

		if( _VERTEX_ARC_TYPE_CHAR & readmatgroup[i].attr )
		{
			vertex = (void*)bvertex;
			function_id=1;
		}
		else
		if( _VERTEX_ARC_TYPE_SHORT & readmatgroup[i].attr )
		{
			vertex = (void*)wvertex;
			function_id=2;
		}
		else
		{
			vertex = (void*)fvertex;
			function_id=4;
		}
		if( mMatGroup[i].ObjectId )	//애니메이션이 있을때
		{
			mObject[mMatGroup[i].ObjectId-1].now_frame=0;
			::GetObjectMatrix(w_matrix,mMatGroup[i].ObjectId,mObject,0);
		}

		DWORD color;
		if( readmatgroup[i].mtl_id == -1 )
			color = 0xffffffff;
//			color = 0xff808080;
		else
//			color=0xff808080;
			color=mat[readmatgroup[i].mtl_id].m_Layer[0].m_ARGB;
		int temp_f_num;
		int temp_v_num;

		mMatGroup[i].Type = _MAT_GROUP_TYPE_STANDARD;	//현재는 스탠다드만...
		if( readmatgroup[i].mtl_id != -1 )
		{
			if( mat[readmatgroup[i].mtl_id].m_Layer[0].m_dwAlphaType != _BLEND_NONE )
				mMatGroup[i].Type |= _MAT_GROUP_TYPE_OPPA;	//현재는 스탠다드만...
			if( mat[readmatgroup[i].mtl_id].m_Layer[0].m_dwAlphaType & _MAT_GROUP_TYPE_REFLECT )
				mMatGroup[i].Type |= _MAT_GROUP_TYPE_REFLECT;	//반사다.
		}
		mMatGroup[i].Type |= (readmatgroup[i].attr&0x0f00);	//상위 속성플래그는 그대로 복사.. 

		mMatGroup[i].VertexBufferId = static_vb_cnt;
		mMatGroup[i].VBMinIndex = static_v_cnt;
		mMatGroup[i].IBMinIndex = static_v_index_cnt;
		mMatGroup[i].CFaceStartVId = static_v_cnt+all_static_v_cnt;

		temp_f_num = static_v_index_cnt/3;
		temp_v_num = static_v_cnt;

		//----버텍스 노멀을 일단 구하자.
		
		DWORD real_v_index;
		if( function_id == 2 )
			real_v_index = base_char_index;
		else
		if( function_id == 3 )
			real_v_index = base_word_index;
		else
			real_v_index = 0;
		int temp_static_v_cnt =static_v_cnt;
		for(j=0; j<readmatgroup[i].face_num; j++)
		{
			DWORD f_id = faceid[readmatgroup[i].face_start_id+j];
		
			mCFace[f_id].Attr=0;	//면 속성.
			// 면 노멀을 구하자..
			for(k=0; k<face[f_id].v_num; k++)
			{
				DWORD v_id = face[f_id].v_start_id+k;
				Vector3f t_v;
				LONG c_v_id = real_v_index+vertexid[v_id];

				GetVertexFromCompress[function_id](t_v,(void*)((BYTE *)vertex+vertexid[v_id]*function_id*3),&readmatgroup[i]);
				//충돌 점들...
				if( mMatGroup[i].ObjectId )	//애니메이션이 있을때
				{
					mCVertex[c_v_id][0]=w_matrix[0][0]*t_v[0]+w_matrix[1][0]*t_v[1]+w_matrix[2][0]*t_v[2]+w_matrix[3][0];
					mCVertex[c_v_id][1]=w_matrix[0][1]*t_v[0]+w_matrix[1][1]*t_v[1]+w_matrix[2][1]*t_v[2]+w_matrix[3][1];
					mCVertex[c_v_id][2]=w_matrix[0][2]*t_v[0]+w_matrix[1][2]*t_v[1]+w_matrix[2][2]*t_v[2]+w_matrix[3][2];
				}
				else
					Vector3fCopy(t_v,mCVertex[c_v_id]);
				mCVertexId[temp_static_v_cnt+all_static_v_cnt]=c_v_id;
				temp_static_v_cnt++;
			}
			GetNormal(mCFace[f_id].Normal
				,mCVertex[real_v_index+vertexid[face[f_id].v_start_id]]
				,mCVertex[real_v_index+vertexid[face[f_id].v_start_id+1]]
				,mCVertex[real_v_index+vertexid[face[f_id].v_start_id+2]]);
			if( mCFace[f_id].Normal[1] >= 0 )	//약 30도 아랬것들은 죄
			{
				//mCFace[f_id].Attr|=_CFACE_ATTR_FLOOR;
			}
			else
				mCFace[f_id].Attr|=_CFACE_ATTR_NOT_FLOOR;

			// 충돌을 완전 무시하는 폴리곤일경우.
			if( readmatgroup[i].mtl_id != -1 )
			{
				if( mat[readmatgroup[i].mtl_id].m_Layer[0].m_dwFlag & _MAT_NO_COLLISON )
					mCFace[f_id].Attr|=_CFACE_NO_COLLISION;
				if( mat[readmatgroup[i].mtl_id].m_Layer[0].m_dwFlag & _MAT_WATER )	//물속성 폴리곤을 지정한다.
					mCFace[f_id].Attr|=_CFACE_WATER;				
				if( mat[readmatgroup[i].mtl_id].m_Layer[1].m_dwFlag & _MAT_ENV_BUMP )	//물속성 폴리곤을 지정한다.
					mCFace[f_id].Attr|=_CFACE_WATER;
			}

			for(k=0; k<face[f_id].v_num; k++)
			{
				DWORD v_id = face[f_id].v_start_id+k;
				LONG c_v_id = real_v_index+vertexid[v_id];
				v_normal[c_v_id][0] += mCFace[f_id].Normal[0];
				v_normal[c_v_id][1] += mCFace[f_id].Normal[1];
				v_normal[c_v_id][2] += mCFace[f_id].Normal[2];
			}
		}

		for(j=0; j<readmatgroup[i].face_num; j++)
		{
			DWORD f_id = faceid[readmatgroup[i].face_start_id+j];
			for(k=0; k<face[f_id].v_num-2; k++)	//인덱스 포인터를 채운다.
			{
				static_indexbp[static_v_index_cnt++]=(WORD)(static_v_cnt);
				static_indexbp[static_v_index_cnt++]=(WORD)(static_v_cnt+k+1);
				static_indexbp[static_v_index_cnt++]=(WORD)(static_v_cnt+k+2);
			}
			mCFace[f_id].VNum = (BYTE)face[f_id].v_num;
			mCFace[f_id].VStartId = static_v_cnt+all_static_v_cnt;//face[f_id].v_start_id;
			mCFace[f_id].MatGIndex = (WORD)i;

			DWORD flag=0;
			if( readmatgroup[i].mtl_id != -1 )
			{
				for(k=0; k<(int)mat[readmatgroup[i].mtl_id].m_dwLayerNum; k++)
				{
					flag|=mat[readmatgroup[i].mtl_id].m_Layer[k].m_dwFlag;
				}
			}
			for(k=0; k<face[f_id].v_num; k++)
			{
				DWORD v_id = face[f_id].v_start_id+k;
				Vector3f t_v;
				DWORD gr_color=color;
				
				//압축된 x ,y ,z를 풀어서 채운다.
				GetVertexFromCompress[function_id](t_v,(void*)((BYTE *)vertex+vertexid[v_id]*function_id*3),&readmatgroup[i]);
				if( readmatgroup[i].mtl_id != -1 && flag & _UV_GRADIENT_ALPHA_UV )
				{
					float mul_alpha = (float)((gr_color&0xff000000)>>24)/255.0f;
					gr_color &= 0x00ffffff;
					float u_alpha=1.0f,v_alpha=1.0f,v_su;
					if( flag & _UV_GRADIENT_ALPHA_U )
					{
						v_su = (((0xff00&(WORD)(mat[readmatgroup[i].mtl_id].m_Layer[0].m_sGradientAlpha))>>8)-100.0f)/25.0f;
						u_alpha = u_alpha*(uv[v_id][0]/v_su);
						if( v_su < 0)
							u_alpha+=(1/-v_su);
					}
					u_alpha = min(u_alpha,1);
					u_alpha = max(u_alpha,0);
					if( flag & _UV_GRADIENT_ALPHA_V )
					{
						v_su = (((0x00ff&(WORD)(mat[readmatgroup[i].mtl_id].m_Layer[0].m_sGradientAlpha)))-100.0f)/25.0f;
						//if( v_su )
						v_alpha = v_alpha*(uv[v_id][1]/v_su);
						if( v_su < 0)
							v_alpha+=(1/-v_su);
					}
					v_alpha = min(v_alpha,1);
					v_alpha = max(v_alpha,0);
					gr_color|=((DWORD)(u_alpha*v_alpha*mul_alpha*255)<<24);
				}
				static_vertexbp[static_v_cnt].x = t_v[0];
				static_vertexbp[static_v_cnt].y = t_v[1];
				static_vertexbp[static_v_cnt].z = t_v[2];
				static_vertexbp[static_v_cnt].uv[0][0] = uv[v_id][0];	//텍스쳐uv
				static_vertexbp[static_v_cnt].uv[0][1] = uv[v_id][1];
				if( readmatgroup[i].mtl_id != -1 && mat[readmatgroup[i].mtl_id].m_iDetailSurface !=-1 
					&& (mat[readmatgroup[i].mtl_id].m_dwFlag &_MAT_FLAG_LIGHT_MAP) && readmatgroup[i].lgt_id == -1 )
				{
					static_vertexbp[static_v_cnt].uv[1][0] = uv[v_id][0]*mat[readmatgroup[i].mtl_id].m_fDetailScale*8;	//디테일 맵uv
					static_vertexbp[static_v_cnt].uv[1][1] = uv[v_id][1]*mat[readmatgroup[i].mtl_id].m_fDetailScale*8;
				}
				else
				{
					static_vertexbp[static_v_cnt].uv[1][0] = lgtuv[v_id][0]/32767.0f;	//라이트맵uv
					static_vertexbp[static_v_cnt].uv[1][1] = lgtuv[v_id][1]/32767.0f;
				}
				if( readmatgroup[i].lgt_id == -1 )
					static_vertexbp[static_v_cnt].color =(gr_color&0xff000000)|(v_color[vertexid[v_id]]&0x00ffffff);
				else
					static_vertexbp[static_v_cnt].color =gr_color;

				static_v_cnt++;
			}
//			GetNormal(mCFace[f_id].Normal,mCVertex[face[f_id].v_start_id]
//				,mCVertex[face[f_id].v_start_id+1],mCVertex[face[f_id].v_start_id+2]);

			for(k=static_v_cnt-face[f_id].v_num; k<static_v_cnt; k++)
			{//노멀을 넣는 경우.
				DWORD v_id = face[f_id].v_start_id+k-(static_v_cnt-face[f_id].v_num);
				LONG c_v_id = real_v_index+vertexid[v_id];
				Normalize(v_normal[c_v_id]);
				static_vertexbp[k].nx = v_normal[c_v_id][0];
				static_vertexbp[k].ny = v_normal[c_v_id][1];
				static_vertexbp[k].nz = v_normal[c_v_id][2];
			}
		}
		
			
		mMatGroup[i].VCnt = static_v_cnt - temp_v_num;	//요 머터리얼그룹의 버텍스 갯수..
		mMatGroup[i].TriNum = static_v_index_cnt/3-temp_f_num;	//요머터리얼의 tri수
		//------------------ 멀티레이어에 필요한 요소는 버퍼에 넣어둔다. --------------------
		DWORD mflag=0;
		if( readmatgroup[i].mtl_id != -1 )
			for(j=0; j<(int)mat[readmatgroup[i].mtl_id].m_dwLayerNum; j++)
				mflag|=mat[readmatgroup[i].mtl_id].m_Layer[j].m_dwFlag;

		mMatGroup[i].MultiSourceUV=NULL;
		if( mflag )
		{
			Vector3f tex2temp[8];
			Vector2f *m_uv=(Vector2f *)mMultiLayerUV;
			Vector3f *m_st=(Vector3f *)mMultiLayerST;

			if( mflag & _UV_METAL )
			{
				mMatGroup[i].MultiSourceST = (void*)&m_st[st_num];
			}

			mMatGroup[i].MultiSourceUV = (void*)&m_uv[uv_num];
			for(j=0; j<readmatgroup[i].face_num; j++)
			{
				DWORD f_id = faceid[readmatgroup[i].face_start_id+j];
				int temp_st_num=st_num;

				for(k=0; k<face[f_id].v_num; k++)
				{
					DWORD v_id = face[f_id].v_start_id+k;
					
					if( mflag & _UV_METAL )
					{
						//압축된 x ,y ,z를 풀어서 채운다.
						GetVertexFromCompress[function_id](tex2temp[k],(void*)((BYTE *)vertex+vertexid[v_id]*function_id*3),&readmatgroup[i]);
						m_st[temp_st_num*3][0]=tex2temp[k][0];
						m_st[temp_st_num*3][1]=tex2temp[k][1];
						m_st[temp_st_num*3][2]=tex2temp[k][2];
						temp_st_num++;
						m_uv[uv_num][0] = uv[v_id][0];	//텍스쳐uv
						m_uv[uv_num][1] = uv[v_id][1];
						uv_num++;
					}
					else//나머지는 uv다
					{
						m_uv[uv_num][0] = uv[v_id][0];	//텍스쳐uv
						m_uv[uv_num][1] = uv[v_id][1];
						uv_num++;
					}
				}
				if( mflag & _UV_METAL )
				{
					Vector3f normal,an,bn;
					Vector3f v_s,v_t;
					Vector3fSub(tex2temp[1],tex2temp[0],an);
					Vector3fSub(tex2temp[2],tex2temp[0],bn);
					Vector3fCross(an,bn,normal);
					Normalize(normal);

					if( fabs(normal[2]) > 0.98 )
					{
						v_s[0] = 1.0;	v_s[1]=0;	v_s[2]=0;
					}
					else
					{
						v_s[0] = -normal[1];	v_s[1]=normal[0];	v_s[2]=0;
						Normalize(v_s);
					}
					Vector3fCross(v_s,normal,v_t);

					for(k=0; k<face[f_id].v_num; k++)
					{
						m_st[st_num*3+1][0]=v_s[0];
						m_st[st_num*3+1][1]=v_s[1];
						m_st[st_num*3+1][2]=v_s[2];
						m_st[st_num*3+2][0]=v_t[0];
						m_st[st_num*3+2][1]=v_t[1];
						m_st[st_num*3+2][2]=v_t[2];
						st_num++;
					}
				}
			}
			//-----------------------
		}
		if( (DWORD)static_v_cnt == mVBVertexNum[static_vb_cnt] )  //버텍스 버퍼 체인지.
		{			
			all_static_v_cnt+=static_v_cnt;
			//static_v_index_cnt=0;
			static_v_cnt=0;
			mStaticVertexBuffer[static_vb_cnt].VPUnLock();
			static_vb_cnt++;
			static_vertexbp=(_D3DR3VERTEX_TEX2 *)mStaticVertexBuffer[static_vb_cnt].VPLock();
		}
	}
	mStaticVertexBuffer[static_vb_cnt].VPUnLock();
	mStaticIndexedBuffer.VPUnLock();

	mVertexNum=all_static_v_cnt;
	mFaceNum=static_v_index_cnt/3;

	memcpy(mLgtUV,lgtuv,mBSPHeader.LgtUV.size);//라이트맵 좌포를 넣어준다.
	memcpy(mVertexColor,v_color,mBSPHeader.VertexColor.size);//버텍스 컬러를 넣어준다.
/*
	LoadEntities(readMapEntitiesList);	//엔티티들을 로딩한다.
	LoadSoundEntities(readSoundEntityList,readSoundEntitiesList);	//엔티티들을 로딩한다.
*/
	Dfree(v_normal);
	Dfree(read_alloc_buf);
}


void CBsp::LoadExtBsp(char *name)
{
	FILE *fp;
	int size;

	if( (fp=fopen(name,"rb")) == NULL)
	{
		return;		//없는 갑다.
	}

	fread(&mExtBSPHeader,sizeof(_EXT_BSP_FILE_HEADER),1,fp);

	if(mExtBSPHeader.version != _R3_EXT_BSP_VERSION)
	{
		Warning("ebp파일 버젼이 맞지않슴다.","장언일");
		return;
	}

	mCFVertexNum = mExtBSPHeader.CFVertex.size/sizeof(Vector3f);
	mCFLineNum = mExtBSPHeader.CFLine.size/sizeof(_TOOL_COL_LINE);
	mCFLineIdNum = mExtBSPHeader.CFLineId.size/sizeof(WORD);
	mEntityListNum=mExtBSPHeader.EntityList.size/sizeof(_ENTITY_LIST);
	mLeafEntityListNum=mExtBSPHeader.LeafEntityList.size/sizeof(_LEAF_ENTITIES_LIST_INFO);
	mEntityIDNum=mExtBSPHeader.EntityID.size/sizeof(WORD);
	mMapEntitiesListNum =mExtBSPHeader.MapEntitiesList.size/sizeof(_READ_MAP_ENTITIES_LIST);
	mSoundEntityIDNum = mExtBSPHeader.SoundEntityID.size/sizeof(WORD);
	mLeafSoundEntityListNum = mExtBSPHeader.LeafSoundEntityList.size/sizeof(_LEAF_SOUND_ENTITIES_LIST_INFO);
	mSoundEntityListNum=mExtBSPHeader.SoundEntityList.size/sizeof(_READ_SOUND_ENTITY_LIST);		//사운드 원본 갯수.
	mSoundEntitiesListNum=mExtBSPHeader.SoundEntitiesList.size/sizeof(_READ_SOUND_ENTITIES_LIST);	//복사판들 갯수.

	//잘못된 ebp파일 
	if( mLeafSoundEntityListNum &&  mLeafSoundEntityListNum != mLeafNum )
	{
		fclose(fp);
		Error(name,"<-ebp파일이 BSP와 맞지않슴다. ebp를 다른버전으로 작업한듯");
	}
	if( mLeafEntityListNum &&  mLeafEntityListNum != mLeafNum )
	{
		fclose(fp);
		Error(name,"<-ebp파일이 BSP와 맞지않슴다. ebp를 다른버전으로 작업한듯");
	}
	if( mLeafSoundEntityListNum &&  mLeafSoundEntityListNum != mLeafNum )
	{
		fclose(fp);
		Error(name,"<-ebp파일이 BSP와 맞지않슴다. ebp를 다른버전으로 작업한듯");
	}

	mExtBspStaticAllocSize=0;

	mExtBspStaticAllocSize += mExtBSPHeader.CFVertex.size;
	mExtBspStaticAllocSize += mExtBSPHeader.CFLine.size;
	mExtBspStaticAllocSize += mExtBSPHeader.CFLineId.size;
	mExtBspStaticAllocSize += mExtBSPHeader.CFLeaf.size;
	mExtBspStaticAllocSize += mCFVertexNum*sizeof(Vector3f);
	mExtBspStaticAllocSize += mCFLineNum*sizeof(Vector4f);
	mExtBspStaticAllocSize += mEntityListNum*sizeof(_ENTITY_LIST);
	mExtBspStaticAllocSize += mLeafEntityListNum*sizeof(_LEAF_ENTITIES_LIST_INFO);
	mExtBspStaticAllocSize += mEntityIDNum*sizeof(WORD);
	//사운드 엔티티..
	mExtBspStaticAllocSize += mMapEntitiesListNum*sizeof(_MAP_ENTITIES_LIST);
	mExtBspStaticAllocSize += mSoundEntityIDNum*sizeof(WORD);
	mExtBspStaticAllocSize += mLeafSoundEntityListNum*sizeof(_LEAF_SOUND_ENTITIES_LIST_INFO);
	mExtBspStaticAllocSize += mSoundEntityListNum*sizeof(_SOUND_ENTITY_LIST);
	mExtBspStaticAllocSize += mSoundEntitiesListNum*sizeof(_SOUND_ENTITIES_LIST);
	

	mTotalAllocSize += mExtBspStaticAllocSize;	//메모리 전체 용량추가.


	mExtBspStaticAlloc=(BYTE *)Dmalloc(mExtBspStaticAllocSize);
	size=0;

	mCFVertex = (Vector3f *)&mExtBspStaticAlloc[size];
	fread(mCFVertex,mExtBSPHeader.CFVertex.size,1,fp);
	size += mExtBSPHeader.CFVertex.size;

	mCFLine = (_TOOL_COL_LINE *)&mExtBspStaticAlloc[size];
	fread(mCFLine,mExtBSPHeader.CFLine.size,1,fp);
	size += mExtBSPHeader.CFLine.size;

	mCFLineId=(WORD*)&mExtBspStaticAlloc[size];
	fread(mCFLineId,mExtBSPHeader.CFLineId.size,1,fp);
	size += mExtBSPHeader.CFLineId.size;

	mCFLeaf = (_TOOL_COL_LEAF*)&mExtBspStaticAlloc[size];
	fread(mCFLeaf,mExtBSPHeader.CFLeaf.size,1,fp);
	size += mExtBSPHeader.CFLeaf.size;

	mCFVNormal = (Vector3f *)&mExtBspStaticAlloc[size];
	size += mCFVertexNum*sizeof(Vector3f);

	mCFNormal = (Vector4f*)&mExtBspStaticAlloc[size];
	size += mCFLineNum*sizeof(Vector4f);

	SetCFNormal();	//충돌페이스 점및 라인 노멀을 구한다.

	mEntityList = (_ENTITY_LIST*)&mExtBspStaticAlloc[size];
	fread(mEntityList,mExtBSPHeader.EntityList.size,1,fp);
	size += mExtBSPHeader.EntityList.size;

	mEntityID = (WORD*)&mExtBspStaticAlloc[size];
	fread(mEntityID,mExtBSPHeader.EntityID.size,1,fp);
	size += mExtBSPHeader.EntityID.size;

	mLeafEntityList = (_LEAF_ENTITIES_LIST_INFO*)&mExtBspStaticAlloc[size];
	fread(mLeafEntityList,mExtBSPHeader.LeafEntityList.size,1,fp);
	size += mExtBSPHeader.LeafEntityList.size;

	mSoundEntityID = (WORD *)&mExtBspStaticAlloc[size];
	fread(mSoundEntityID,mExtBSPHeader.SoundEntityID.size,1,fp);
	size += mExtBSPHeader.SoundEntityID.size;

	mLeafSoundEntityList = (_LEAF_SOUND_ENTITIES_LIST_INFO *)&mExtBspStaticAlloc[size];
	fread(mLeafSoundEntityList,mExtBSPHeader.LeafSoundEntityList.size,1,fp);
	size += mExtBSPHeader.LeafSoundEntityList.size;


	mMapEntitiesList = (_MAP_ENTITIES_LIST*)&mExtBspStaticAlloc[size];
	size += mMapEntitiesListNum*sizeof(_MAP_ENTITIES_LIST);
	memset(mMapEntitiesList,0,mMapEntitiesListNum*sizeof(_MAP_ENTITIES_LIST));//나중에 릴리즈시 에러남.

	mSoundEntityList = (_SOUND_ENTITY_LIST *)&mExtBspStaticAlloc[size];
	size += sizeof(_SOUND_ENTITY_LIST)*mSoundEntityListNum;	//사운드 원본 정보...

	mSoundEntitiesList = (_SOUND_ENTITIES_LIST *)&mExtBspStaticAlloc[size];
	size += mSoundEntitiesListNum*sizeof(_SOUND_ENTITIES_LIST);	//사운드복사체정보 .
	
	ReadDynamicDataExtBsp(fp);

	fclose(fp);

	mEntityCacheSize = ((mExtBSPHeader.MapEntitiesList.size)/sizeof(_READ_MAP_ENTITIES_LIST))/8+1;		//엔티티 캐쉬 할 버퍼다.
	mEntityCache=(BYTE *)Dmalloc(mEntityCacheSize);		//맵 엔티티 캐쉬 사이즈 
	mTotalAllocSize += mEntityCacheSize;	//메모리 전체 용량계산.

	mSoundEntityCacheSize = (mSoundEntitiesListNum)/8+1;		//사운드 엔티티 캐쉬 할 버퍼다.
	mSoundEntityCache=(BYTE *)Dmalloc(mSoundEntityCacheSize);		//사운드  엔티티 캐쉬 사이즈 
	memset(mSoundEntityCache,0,mSoundEntityCacheSize);		//사운드  엔티티 캐쉬 사이즈는 오픈할때 한번클리어한다.

	mTotalAllocSize += mSoundEntityCacheSize;	//메모리 전체 용량계산.


//	mCNEdgeNormal = (Vector4f *)Dmalloc(sizeof(Vector4f)*4*mCFaceNum);
//	MakeEdgeNormal();//서버모드일때 
//점 노멀생성 
//	mVNormal = (Vector3f *)&mExtBspStaticAlloc[size];//점 노멀을 만들기위한 공간...
//	size += mExtBSPHeader.CVertex.size;
//	GetVertexNormal();
}

void CBsp::ReadDynamicDataExtBsp(FILE *fp)	//ext bsp에서 파일로읽고 버려야할것들.
{
	if( IsServerMode() )	//서버모드면
		return;

	int read_alloc_size=0;
	BYTE *read_alloc_buf;

	read_alloc_size += mExtBSPHeader.MapEntitiesList.size;
	read_alloc_size += mExtBSPHeader.SoundEntityList.size;
	read_alloc_size += mExtBSPHeader.SoundEntitiesList.size;

	read_alloc_buf=(BYTE *)Dmalloc(read_alloc_size);

	//-----한번 읽고 정리해서 저장 해야될것들...
	_READ_MAP_ENTITIES_LIST *readMapEntitiesList;
	_READ_SOUND_ENTITY_LIST *readSoundEntityList;
	_READ_SOUND_ENTITIES_LIST *readSoundEntitiesList;

	int size=0;
	readMapEntitiesList=(_READ_MAP_ENTITIES_LIST *)&read_alloc_buf[size];
	fread(readMapEntitiesList,mExtBSPHeader.MapEntitiesList.size,1,fp);
	size += mExtBSPHeader.MapEntitiesList.size;

	readSoundEntityList=(_READ_SOUND_ENTITY_LIST *)&read_alloc_buf[size];
	fread(readSoundEntityList,mExtBSPHeader.SoundEntityList.size,1,fp);
	size += mExtBSPHeader.SoundEntityList.size;

	readSoundEntitiesList=(_READ_SOUND_ENTITIES_LIST *)&read_alloc_buf[size];
	fread(readSoundEntitiesList,mExtBSPHeader.SoundEntitiesList.size,1,fp);
	size += mExtBSPHeader.SoundEntitiesList.size;

	LoadEntities(readMapEntitiesList);	//엔티티들을 로딩한다.
	LoadSoundEntities(readSoundEntityList,readSoundEntitiesList);	//엔티티들을 로딩한다.

	Dfree(read_alloc_buf);
}


void CBsp::SetCFNormal()	//충돌페이스 점및 라인 노멀을 구한다.
{
	DWORD i;
	Vector3f a,b,c;

	for(i=0; i<mCFLineNum; i++)
	{
		a[0] = mCFVertex[mCFLine[i].start_v][0];
		a[1] = mCFVertex[mCFLine[i].start_v][1];
		a[2] = mCFVertex[mCFLine[i].start_v][2];

		b[0] = mCFVertex[mCFLine[i].end_v][0];
		b[1] = mCFVertex[mCFLine[i].end_v][1]+mCFLine[i].height;
		b[2] = mCFVertex[mCFLine[i].end_v][2];

		c[0] = mCFVertex[mCFLine[i].end_v][0];
		c[1] = mCFVertex[mCFLine[i].end_v][1];
		c[2] = mCFVertex[mCFLine[i].end_v][2];

		::GetNormal(mCFNormal[i], a, b, c);
	}
	memset(mCFVNormal,0,sizeof(Vector3f)*mCFVertexNum);

	
	for(i=1; i<mCFLineNum; i++)
	{
		int w=mCFLine[i].start_v;
		mCFVNormal[w][0]+=mCFNormal[i][0];
		mCFVNormal[w][1]+=mCFNormal[i][1];
		mCFVNormal[w][2]+=mCFNormal[i][2];
		w=mCFLine[i].end_v;
		mCFVNormal[w][0]+=mCFNormal[i][0];
		mCFVNormal[w][1]+=mCFNormal[i][1];
		mCFVNormal[w][2]+=mCFNormal[i][2];
	}
	for(i=1; i<mCFVertexNum; i++)
	{
		Normalize(mCFVNormal[i]);
	}
}

void CBsp::LoadBsp(char *name)
{
	FILE *fp;
	int size;

	if(IsInitR3Engine()==FALSE)
		Error("R3engine 초기화 부터하세요.","");

	if( (fp=fopen(name,"rb")) == NULL)
	{
		Error(name,"<- 파일이 없슴다.");
	}
	mNowCFaceId=0;
	mTotalAllocSize=0;
	ResetTotalVertexBufferInfo();

	fread(&mBSPHeader,sizeof(_BSP_FILE_HEADER),1,fp);

	if(mBSPHeader.version != _R3BSP_VERSION)
		Error("BSP파일 버젼이 맞지않슴다.","장언일");
	//메모리 할당.

	mLeafNum=mBSPHeader.Leaf.size/sizeof(_BSP_LEAF);
	mNodeNum=mBSPHeader.Node.size/sizeof(_BSP_NODE);


	mCVertexNum=(mBSPHeader.BVertex.size/sizeof(Vector3c)+mBSPHeader.WVertex.size/sizeof(Vector3s)
		+mBSPHeader.FVertex.size/sizeof(Vector3f));
	mCFaceNum=mBSPHeader.Face.size/sizeof(_BSP_READ_FACE);
/*
	mCFVertexNum = mBSPHeader.CFVertex.size/sizeof(Vector3f);
	mCFLineNum = mBSPHeader.CFLine.size/sizeof(_TOOL_COL_LINE);
	mCFLineIdNum = mBSPHeader.CFLineId.size/sizeof(WORD);
*/
	mObjectNum = (mBSPHeader.Object.size/sizeof(_READ_ANI_OBJECT));
/*
	mEntityListNum=mBSPHeader.EntityList.size/sizeof(_ENTITY_LIST);
	mLeafEntityListNum=mBSPHeader.LeafEntityList.size/sizeof(_LEAF_ENTITIES_LIST_INFO);
	mEntityIDNum=mBSPHeader.EntityID.size/sizeof(WORD);
	mMapEntitiesListNum =mBSPHeader.MapEntitiesList.size/sizeof(_READ_MAP_ENTITIES_LIST);
*/
	mMatGroupNum = (mBSPHeader.ReadMatGroup.size/sizeof(_BSP_READ_M_GROUP));
/*
	//사운드 엔티티...
	mSoundEntityIDNum = mBSPHeader.SoundEntityID.size/sizeof(WORD);
	mLeafSoundEntityListNum = mBSPHeader.LeafSoundEntityList.size/sizeof(_LEAF_SOUND_ENTITIES_LIST_INFO);
	mSoundEntityListNum=mBSPHeader.SoundEntityList.size/sizeof(_READ_SOUND_ENTITY_LIST);		//사운드 원본 갯수.
	mSoundEntitiesListNum=mBSPHeader.SoundEntitiesList.size/sizeof(_READ_SOUND_ENTITIES_LIST);	//복사판들 갯수.
*/
	int c_vertex_size = mCVertexNum*sizeof(Vector3f);
	int c_vertex_id_size = mBSPHeader.VertexId.size;
	int c_face_size = sizeof(_BSP_C_FACE)*mCFaceNum;

	if( IsServerMode() )	//서버모드일때 에지노멀을 사용한다.
	{
		mCNEdgeNormal = (Vector4f *)Dmalloc(sizeof(Vector4f)*4*mCFaceNum);
		mMapEntitiesListNum=0;
		mEntityIDNum=0;
		mEntityListNum=0;
		mLeafEntityListNum=0;
	}

	mStaticAllocSize=0;
	mStaticAllocSize += c_vertex_size;
	mStaticAllocSize += c_vertex_id_size;
	mStaticAllocSize += c_face_size;
	mStaticAllocSize += mBSPHeader.CPlanes.size;
	mStaticAllocSize += mBSPHeader.CFaceId.size;
	mStaticAllocSize += mBSPHeader.Node.size;
	mStaticAllocSize += mBSPHeader.Leaf.size;
	mStaticAllocSize += mBSPHeader.MatListInLeaf.size;
	mStaticAllocSize += sizeof(_ANI_OBJECT)*(mBSPHeader.Object.size/sizeof(_READ_ANI_OBJECT));//읽는것과 저장하는것이 틀리다.
	mStaticAllocSize += mBSPHeader.Track.size;
/*
	mStaticAllocSize += mBSPHeader.CFVertex.size;
	mStaticAllocSize += mBSPHeader.CFLine.size;
	mStaticAllocSize += mBSPHeader.CFLineId.size;
	mStaticAllocSize += mBSPHeader.CFLeaf.size;
	mStaticAllocSize += mCFVertexNum*sizeof(Vector3f);
	mStaticAllocSize += mCFLineNum*sizeof(Vector4f);
	mStaticAllocSize += mEntityListNum*sizeof(_ENTITY_LIST);
	mStaticAllocSize += mLeafEntityListNum*sizeof(_LEAF_ENTITIES_LIST_INFO);
	mStaticAllocSize += mEntityIDNum*sizeof(WORD);
*/
	mStaticAllocSize += sizeof(_BSP_MAT_GROUP)*(mBSPHeader.ReadMatGroup.size/sizeof(_BSP_READ_M_GROUP));
	mStaticAllocSize += sizeof(_MAP_ENTITIES_LIST)*mMapEntitiesListNum;
	mStaticAllocSize += mBSPHeader.LgtUV.size;	//라이트맵 uv좌표가지고 있는거 ...
	mStaticAllocSize += mBSPHeader.VertexColor.size;	//버텍스 컬러 가지고 있는거 ...
/*	//사운드 엔티티..
	mStaticAllocSize += mSoundEntityIDNum*sizeof(WORD);
	mStaticAllocSize += mLeafSoundEntityListNum*sizeof(_LEAF_SOUND_ENTITIES_LIST_INFO);
	mStaticAllocSize += mSoundEntityListNum*sizeof(_SOUND_ENTITY_LIST);
	mStaticAllocSize += mSoundEntitiesListNum*sizeof(_SOUND_ENTITIES_LIST);
*/
	mStaticAllocSize += mObjectNum*sizeof(WORD);
	
	mTotalAllocSize += mStaticAllocSize;	//메모리 전체 용량.


	mStaticAlloc=(BYTE *)Dmalloc(mStaticAllocSize);
	size=0;

	mCVertex = (Vector3f *)&mStaticAlloc[size];
	size += c_vertex_size;

	mCVertexId = (DWORD *)&mStaticAlloc[size];
	size += c_vertex_id_size;

	mCFace = (_BSP_C_FACE *)&mStaticAlloc[size];
	size += c_face_size;

	mCNNormal = (Vector3f *)&mStaticAlloc[size];
	fread(mCNNormal,mBSPHeader.CPlanes.size,1,fp);
	size += mBSPHeader.CPlanes.size;

	mCFaceId = (DWORD *)&mStaticAlloc[size];
	fread(mCFaceId,mBSPHeader.CFaceId.size,1,fp);
	size += mBSPHeader.CFaceId.size;

	mNode = (_BSP_NODE *)&mStaticAlloc[size];
	fread(mNode,mBSPHeader.Node.size,1,fp);
	size += mBSPHeader.Node.size;

	mLeaf = (_BSP_LEAF *)&mStaticAlloc[size];
	fread(mLeaf,mBSPHeader.Leaf.size,1,fp);
	size += mBSPHeader.Leaf.size;

	MatListInLeafId = (WORD *)&mStaticAlloc[size];
	fread(MatListInLeafId,mBSPHeader.MatListInLeaf.size,1,fp);
	size += mBSPHeader.MatListInLeaf.size;

	_READ_ANI_OBJECT *r_object=(_READ_ANI_OBJECT *)Dmalloc(mBSPHeader.Object.size);
	mObject = (_ANI_OBJECT *)&mStaticAlloc[size];

	fread(r_object,mBSPHeader.Object.size,1,fp);
	size +=sizeof(_ANI_OBJECT)*(mBSPHeader.Object.size/sizeof(_READ_ANI_OBJECT));

	BYTE *track = (BYTE *)&mStaticAlloc[size];
	fread(track,mBSPHeader.Track.size,1,fp);
	size += mBSPHeader.Track.size;

	mEventObjectID = (WORD *)&mStaticAlloc[size];
	fread(mEventObjectID,mBSPHeader.EventObjectID.size,1,fp);
	size += mObjectNum*sizeof(WORD);	//사운드복사체정보 .
/*
	mCFVertex = (Vector3f *)&mStaticAlloc[size];
	fread(mCFVertex,mBSPHeader.CFVertex.size,1,fp);
	size += mBSPHeader.CFVertex.size;

	mCFLine = (_TOOL_COL_LINE *)&mStaticAlloc[size];
	fread(mCFLine,mBSPHeader.CFLine.size,1,fp);
	size += mBSPHeader.CFLine.size;

	mCFLineId=(WORD*)&mStaticAlloc[size];
	fread(mCFLineId,mBSPHeader.CFLineId.size,1,fp);
	size += mBSPHeader.CFLineId.size;

	mCFLeaf = (_TOOL_COL_LEAF*)&mStaticAlloc[size];
	fread(mCFLeaf,mBSPHeader.CFLeaf.size,1,fp);
	size += mBSPHeader.CFLeaf.size;

	mCFVNormal = (Vector3f *)&mStaticAlloc[size];
	size += mCFVertexNum*sizeof(Vector3f);

	mCFNormal = (Vector4f*)&mStaticAlloc[size];
	size += mCFLineNum*sizeof(Vector4f);

	SetCFNormal();	//충돌페이스 점및 라인 노멀을 구한다.

	mEntityList = (_ENTITY_LIST*)&mStaticAlloc[size];
	fread(mEntityList,mBSPHeader.EntityList.size,1,fp);
	size += mBSPHeader.EntityList.size;

	mEntityID = (WORD*)&mStaticAlloc[size];
	fread(mEntityID,mBSPHeader.EntityID.size,1,fp);
	size += mBSPHeader.EntityID.size;

	mLeafEntityList = (_LEAF_ENTITIES_LIST_INFO*)&mStaticAlloc[size];
	fread(mLeafEntityList,mBSPHeader.LeafEntityList.size,1,fp);
	size += mBSPHeader.LeafEntityList.size;

	mSoundEntityID = (WORD *)&mStaticAlloc[size];
	fread(mSoundEntityID,mBSPHeader.SoundEntityID.size,1,fp);
	size += mBSPHeader.SoundEntityID.size;

	mLeafSoundEntityList = (_LEAF_SOUND_ENTITIES_LIST_INFO *)&mStaticAlloc[size];
	fread(mLeafSoundEntityList,mBSPHeader.LeafSoundEntityList.size,1,fp);
	size += mBSPHeader.LeafSoundEntityList.size;
*/
	DWORD skip_file_size = 0;
	for(DWORD i=0; i<_READ_BSP_SPARE_CNT; i++)
		skip_file_size += mBSPHeader.ReadSpare[i].size;
	fseek(fp,skip_file_size,SEEK_CUR);	//엔티티파일 분리로 인한...

	ConvAniObject(mBSPHeader.Object.size/sizeof(_READ_ANI_OBJECT),track,r_object,mObject);		//애니메이션 구조체이동.
	Dfree(r_object);	//불필요한거 날려라..

//----------------다이나믹안에 스태틱 잡을 거는 미리 잡아놓는다.
	//머터리얼  그룹을 할당해서 호출하자....
	mMatGroup = (_BSP_MAT_GROUP *)&mStaticAlloc[size];
	size += sizeof(_BSP_MAT_GROUP)*(mBSPHeader.ReadMatGroup.size/sizeof(_BSP_READ_M_GROUP));
/*
	mMapEntitiesList = (_MAP_ENTITIES_LIST*)&mStaticAlloc[size];
	size += mMapEntitiesListNum*sizeof(_MAP_ENTITIES_LIST);
	memset(mMapEntitiesList,0,mMapEntitiesListNum*sizeof(_MAP_ENTITIES_LIST));//나중에 릴리즈시 에러남.
*/
	mLgtUV = (Vector2s *)&mStaticAlloc[size];
	size += mBSPHeader.LgtUV.size;	//라이트맵 uv좌표가지고 있는거 ...

	mVertexColor = (DWORD *)&mStaticAlloc[size];
	size += mBSPHeader.VertexColor.size;	//버텍스 컬러 가지고 있는거 ...
/*
	mSoundEntityList = (_SOUND_ENTITY_LIST *)&mStaticAlloc[size];
	size += sizeof(_SOUND_ENTITY_LIST)*mSoundEntityListNum;	//사운드 원본 정보...

	mSoundEntitiesList = (_SOUND_ENTITIES_LIST *)&mStaticAlloc[size];
	size += mSoundEntitiesListNum*sizeof(_SOUND_ENTITIES_LIST);	//사운드복사체정보 .
*/	
	ReadDynamicDataFillVertexBuffer(fp);

	fclose(fp);

	mMatGroupCacheSize=((mBSPHeader.ReadMatGroup.size)/sizeof(_BSP_READ_M_GROUP))/8+1;
	mMatGroupCache=(BYTE *)Dmalloc(mMatGroupCacheSize);
	mTotalAllocSize += mMatGroupCacheSize;	//메모리 전체 용량계산.

	mAlpha.InitAlpha(this);

/*
	mEntityCacheSize = ((mBSPHeader.MapEntitiesList.size)/sizeof(_READ_MAP_ENTITIES_LIST))/8+1;		//엔티티 캐쉬 할 버퍼다.
	mEntityCache=(BYTE *)Dmalloc(mEntityCacheSize);		//맵 엔티티 캐쉬 사이즈 
	mTotalAllocSize += mEntityCacheSize;	//메모리 전체 용량계산.

	mSoundEntityCacheSize = (mSoundEntitiesListNum)/8+1;		//사운드 엔티티 캐쉬 할 버퍼다.
	mSoundEntityCache=(BYTE *)Dmalloc(mSoundEntityCacheSize);		//사운드  엔티티 캐쉬 사이즈 
	memset(mSoundEntityCache,0,mSoundEntityCacheSize);		//사운드  엔티티 캐쉬 사이즈는 오픈할때 한번클리어한다.

	mTotalAllocSize += mSoundEntityCacheSize;	//메모리 전체 용량계산.
*/


//	mCNEdgeNormal = (Vector4f *)Dmalloc(sizeof(Vector4f)*4*mCFaceNum);
//	MakeEdgeNormal();//서버모드일때 
//점 노멀생성 
//	mVNormal = (Vector3f *)&mStaticAlloc[size];//점 노멀을 만들기위한 공간...
//	size += mBSPHeader.CVertex.size;
//	GetVertexNormal();

	_R3ENGINE_STATE *r3_state=GetR3State();
	mEnvID = r3_state->mEnvID;
	if( mEnvID )
	{
		if( IsParticle(r3_state->mEnvEntityName) )
			mEnvID|=_ENV_PARTICLE;
		if( mEnvID & _ENV_PARTICLE )
		{
			mEnvParticle=new CParticle;
			mEnvParticle->LoadParticleSPT(r3_state->mEnvEntityName);
			mEnvParticle->InitParticle();
			mEnvParticle->SetParticleState(_PARTICLE_STATE_START);
		}
		else
		{
			mEnvEntity=new CEntity;
			mEnvEntity->LoadEntity(r3_state->mEnvEntityName);
		}
	}
	//max에서 그룹으로 만들경우 부모가 간혹 잘못들어오는경우가 있다.
	for( i=0; i<mObjectNum; i++)
	{
		if( mObject[i].parent >= mObjectNum )
			mObject[i].parent=0;
	}
/*
//쉐도우를 위한 충돌 버텍스 버퍼를 생성하자.
	DWORD temp_p=0;	
	for( i=1; i<mLeafNum; i++)
	{
		temp_p += mLeaf[i].face_num;
		for(j=0; j<mCFace[f].VNum; j++)
		{
			LONG v0 = mCVertexId[start_v+j];
			LONG v1 = mCVertexId[start_v+(j+1)%mCFace[f].VNum];
		}
	}
*/
}

//실제 쓰는 구조체로 로딩한다.
void CBsp::LoadSoundEntities(_READ_SOUND_ENTITY_LIST *sound_entity_list ,_READ_SOUND_ENTITIES_LIST *sound_entities_list)	//엔티티들을 로딩한다.
{
	if( mSoundEntityListNum==0 || mSoundEntitiesListNum==0 )	//로딩할 일이 없다.
		return;
	DWORD i;
	char full_name[256];

	_R3ENGINE_STATE *r3_state=GetR3State();
	r3_state->SoundPath;
	
	mTotalWaveSize=0;
	for(i=0; i<mSoundEntityListNum; i++)
	{
		memcpy(mSoundEntityList[i].Name,sound_entity_list[i].name,62);
		strcpy(full_name,r3_state->SoundPath);
		strcat(full_name,sound_entity_list[i].name);
		mSoundEntityList[i].ID = (WORD)IM_LoadWave(full_name,_PLAY_WAVE_LOOP);
		mTotalWaveSize+=GetFileSize(full_name);
	}
	for(i=0; i<mSoundEntitiesListNum; i++)
	{
		mSoundEntitiesList[i].ID = sound_entities_list[i].id;
		mSoundEntitiesList[i].Flag = sound_entities_list[i].flag;
		mSoundEntitiesList[i].Scale = sound_entities_list[i].scale;
		mSoundEntitiesList[i].Attn = sound_entities_list[i].attn;
		Vector3fCopy(sound_entities_list[i].pos,mSoundEntitiesList[i].Pos);
		mSoundEntitiesList[i].BBMin[0] = mSoundEntitiesList[i].Pos[0] - mSoundEntitiesList[i].Attn;
		mSoundEntitiesList[i].BBMin[1] = mSoundEntitiesList[i].Pos[1] - mSoundEntitiesList[i].Attn;
		mSoundEntitiesList[i].BBMin[2] = mSoundEntitiesList[i].Pos[2] - mSoundEntitiesList[i].Attn;
		mSoundEntitiesList[i].BBMax[0] = mSoundEntitiesList[i].Pos[0] + mSoundEntitiesList[i].Attn;
		mSoundEntitiesList[i].BBMax[1] = mSoundEntitiesList[i].Pos[1] + mSoundEntitiesList[i].Attn;
		mSoundEntitiesList[i].BBMax[2] = mSoundEntitiesList[i].Pos[2] + mSoundEntitiesList[i].Attn;
		mSoundEntitiesList[i].BoxAttn=sound_entities_list[i].box_attn;
		mSoundEntitiesList[i].BoxRotX=sound_entities_list[i].box_rot_x;
		mSoundEntitiesList[i].BoxRotY=sound_entities_list[i].box_rot_y;
		mSoundEntitiesList[i].EventTime=sound_entities_list[i].event_time;
		Vector3fCopy(sound_entities_list[i].box_scale,mSoundEntitiesList[i].BoxScale);
		mSoundEntitiesList[i].NextPlayTime=0;

		float mat[4][4],temp[4][4];		//매트릭스를 미리 구해놓는다.
		MatrixScale(mat,mSoundEntitiesList[i].BoxScale[0],mSoundEntitiesList[i].BoxScale[1],mSoundEntitiesList[i].BoxScale[2]);
		MatrixRotate(temp,mSoundEntitiesList[i].BoxRotX,mSoundEntitiesList[i].BoxRotY,0);
		MatrixMultiply(mat,temp,mat);
		Vector3fCopy(mSoundEntitiesList[i].Pos,mat[3]);
		MatrixInvert(mSoundEntitiesList[i].mInvMat,mat);
	}
}

void CBsp::ReleaseSoundEntities()	//엔티티들을 릴리즈한다.
{
	for(DWORD i=0; i<mSoundEntityListNum; i++)
	{
		if( mSoundEntityList[i].ID )
		{
			IM_StopWave(mSoundEntityList[i].ID);	//행여 플레이되고있는거 끈다.
			IM_ReleaseWave(mSoundEntityList[i].ID);
		}
	}
	mSoundEntityListNum=0;
	IM_ReleaseAllWaves();	//강제 사운드 출력 전부를 제거
}

void CBsp::LoadEntities( _READ_MAP_ENTITIES_LIST *readMapEntitiesList)	//엔티티들을 로딩한다.
{
	if( mEntityListNum==0 )	//로딩할 일이 없다.
		return;
	//용량이 얼마안되 엔티티,파티클 갯수를 따로따로 정확하게는 할필요가 없다.
	mEntity=(CEntity *)Dmalloc(mEntityListNum*sizeof(CEntity));	
	memset(mEntity,0,mEntityListNum*sizeof(CEntity));	
	mParticle=(CParticle *)Dmalloc(mEntityListNum*sizeof(CParticle));	
	memset(mParticle,0,mEntityListNum*sizeof(CParticle));	

	mTotalAllocSize += mEntityListNum*sizeof(CEntity);	//메모리 전체 용량계산.
	mTotalAllocSize += mEntityListNum*sizeof(CParticle);	//메모리 전체 용량계산.

	char name[256];
	_R3ENGINE_STATE *r3_state=GetR3State();

	//원소스 엔티티 로딩...
	for(DWORD i=0; i<mEntityListNum; i++)
	{
//		mEntityList[i].ShaderID=1;	//쉐이더 테스트 닷.
//		mEntityList[i].Factor[0]=1;
//		mEntityList[i].Factor[1]=1;

		strcpy(name,r3_state->EntityPath);
		if( mEntityList[i].Name[0] == '\\' )
			strcat(name,&mEntityList[i].Name[1]);
		else
			strcat(name,&mEntityList[i].Name[0]);
		mEntityList[i].IsFileExist=FALSE;
		if( !r3_state->mIsUsedVertexShader ) //버텍스 쉐이더를 사용 못하는 경우.
			mEntityList[i].ShaderID=0;	//쉐이더를 강제로 셋팅한다.
		
		if( mEntityList[i].IsParticle )	//파티클일경우.
		{
			if( !mParticle[i].LoadParticleSPT(name) )
			{
				Warning(name," <- 파일이 없거나, 파티클 spt가 아니다.");
				continue;	//파티클 spt가 아니다.
			}
			mEntityList[i].IsFileExist=TRUE;
			mParticle[i].InitParticle();
			mParticle[i].SetParticleState(_PARTICLE_STATE_START);
		}		
		else
		{
			DWORD flag=0;
			if( mEntityList[i].ShaderID )
				flag = _USE_VERTEX_SHADER;
			flag |=_ENTITY_MAP_COLOR;		//바닥맵 컬러적용.
			if( mEntity[i].LoadEntity(name,flag) ==FALSE )
				continue;
			mEntity[i].RestoreTexMem();
			mEntityList[i].IsFileExist=TRUE;
		}
	}

	//엔티티 클론 체들....
	//읽는 엔티티리스트를 넣어준다.
	for(i=0; i<(int)mMapEntitiesListNum;i++)
	{
		if( !mEntityList[readMapEntitiesList[i].ID].IsFileExist )	//엔티티 파일이없다.
		{
			memset(&mMapEntitiesList[i],0,sizeof(_MAP_ENTITIES_LIST));
			continue;
		}
		mMapEntitiesList[i].ID=readMapEntitiesList[i].ID;

		//-------------------------
//		if( mMapEntitiesList[i].ID >= mEntityListNum-4 )
//			mMapEntitiesList[i].ID=mEntityListNum-4-1;

		Vector3fCopy(readMapEntitiesList[i].Pos,mMapEntitiesList[i].Pos);
		mMapEntitiesList[i].RotX=readMapEntitiesList[i].RotX;
		mMapEntitiesList[i].RotY=readMapEntitiesList[i].RotY;
		mMapEntitiesList[i].Scale=readMapEntitiesList[i].Scale;
		mMapEntitiesList[i].BBMin[0]=readMapEntitiesList[i].BBmin[0];
		mMapEntitiesList[i].BBMin[1]=readMapEntitiesList[i].BBmin[1];
		mMapEntitiesList[i].BBMin[2]=readMapEntitiesList[i].BBmin[2];
		mMapEntitiesList[i].BBMax[0]=readMapEntitiesList[i].BBmax[0];
		mMapEntitiesList[i].BBMax[1]=readMapEntitiesList[i].BBmax[1];
		mMapEntitiesList[i].BBMax[2]=readMapEntitiesList[i].BBmax[2];
		mMapEntitiesList[i].AddFrame = (rand()%256)/4.0f;
		mMapEntitiesList[i].Particle=NULL;
		if( mEntityList[mMapEntitiesList[i].ID].IsParticle )	//파티클이면.
		{
//			if( mMapEntitiesList[i].ID == 19 )
			CParticle *particle=new CParticle;
			mMapEntitiesList[i].Particle = particle;
			memcpy(particle,&mParticle[mMapEntitiesList[i].ID],sizeof(CParticle));
			particle->InitParticle();
		}
	}
}

void CBsp::CalcEntitiesMainColor()	//엔티티들 바닥컬러계산.
{
	for(DWORD i=0; i<(int)mMapEntitiesListNum;i++)
	{
		if( !mEntityList[mMapEntitiesList[i].ID].IsFileExist )	//엔티티 파일이없다.
			continue;

		if( mEntityList[mMapEntitiesList[i].ID].IsParticle )	//파티클이면.
			mMapEntitiesList[i].mMapColor=0xff808080;
		else
		{
			mMapEntitiesList[i].mMapColor = GetLightFromPoint(mMapEntitiesList[i].Pos,30);
			if( mMapEntitiesList[i].mMapColor == 0xff808080 )
			{
				Vector3f temp;
				Vector3fCopy(mMapEntitiesList[i].Pos,temp);
				temp[0]+=0.5f;
				mMapEntitiesList[i].mMapColor = GetLightFromPoint(mMapEntitiesList[i].Pos,30);
			}
		}
	}
}

void CBsp::ReleaseEntities()	//엔티티들을 릴리즈한다.
{
	DWORD i;

	for(i=0; i<mMapEntitiesListNum;i++)
	{
		if( !mEntityList[mMapEntitiesList[i].ID].IsFileExist )	//엔티티가 실제로 없으므로
			continue;
		if(  mMapEntitiesList[i].Particle ==NULL )
			continue;
		if( mEntityList[mMapEntitiesList[i].ID].IsParticle )	//파티클이면.
		{
			mMapEntitiesList[i].Particle->ReleaseParticle();
			delete mMapEntitiesList[i].Particle;
		}
	}

	if( mEntity )
	{
		for(i=0; i<mEntityListNum; i++)
		{
			if( !mEntityList[i].IsFileExist )	//엔티티가 실제로 없으므로
				continue;
			if( mEntityList[i].IsParticle )	//파티클일경우.
			{
				mParticle[i].ReleaseEntity();
				mParticle[i].ReleaseParticle();
			}
			else
				mEntity[i].ReleaseEntity();
		}
	}
	if( mEntity )
	{
		Dfree(mEntity);
		mEntity=NULL;
	}
	if( mParticle )
	{
		Dfree(mParticle);
		mParticle=NULL;
	}
	if( mEntityCache )
	{
		Dfree(mEntityCache);
		mEntityCache=NULL;
	}
	if( mSoundEntityCache )
	{
		Dfree(mSoundEntityCache);
		mSoundEntityCache=NULL;
	}
	mEntityListNum=0;
	mLeafEntityListNum=0;
	mEntityIDNum=0;
	mMapEntitiesListNum=0;

	mSoundEntityIDNum=0;
	mLeafSoundEntityListNum=0;
	mSoundEntityListNum=0;		//사운드 원본 갯수.
	mSoundEntitiesListNum=0;	//복사판들 갯수.
}

void CBsp::ReleaseBsp()					//bsp파일 클리어...
{
	DWORD i;

	ReleaseSoundEntities();	//사운드 엔티티들을 릴리즈한다.
	ReleaseEntities();	//엔티티들을 릴리즈한다.
	if(mStaticAlloc)
	{
		Dfree(mStaticAlloc);
		mStaticAlloc=NULL;
	}
	if(mExtBspStaticAlloc)
	{
		Dfree(mExtBspStaticAlloc);
		mExtBspStaticAlloc=NULL;
	}
	for(i=0; i<mStaticVBCnt; i++)
	{
		mStaticVertexBuffer[i].ReleaseVertexBuffer();
	}
	mStaticIndexedBuffer.ReleaseIndexBuffer();
	if( mMatGroupCache )
	{
		Dfree(mMatGroupCache);
		mMatGroupCache=NULL;
	}
	if( mMultiLayerUV )
	{
		Dfree(mMultiLayerUV);
		mMultiLayerUV=NULL;	
	}

	if( mEnvID )
	{
		if( mEnvID & _ENV_PARTICLE )
		{
			mEnvParticle->ReleaseParticle();
			mEnvParticle->ReleaseEntity();
			delete mEnvParticle;
			mEnvParticle=NULL;
		}
		else
		{
			mEnvEntity->ReleaseEntity();
			delete mEnvEntity;
			mEnvEntity=NULL;
		}
		mEnvID=0;
	}
	if( mCNEdgeNormal )
		Dfree(mCNEdgeNormal);
/*
	if( mDynamicVertexBuffer )
	{
		ReleaseDynamicVertexBuffer(mDynamicVertexBuffer);
		mDynamicVertexBuffer=NULL;
	}*/
	ClearVariable();	//변수들 깨끗이 클리어.
}


//#define RenderedMatGroup(num)	((mMatGroupCache)[num>>5]|=(1<<(num&31)))
//#define IsRenderedMatGroup(num) ((mMatGroupCache)[num>>5]&(1<<(num&31)))
#define RenderedMatGroup(num)	(((BYTE*)mMatGroupCache)[num>>3]|=(1<<(num&7)))
#define IsRenderedMatGroup(num) (((BYTE*)mMatGroupCache)[num>>3]&(1<<(num&7)))

void CBsp::LoopInitRenderedMatGroup()	//머터리얼 캐쉬 초기화..
{
	if( mMatGroupCache == NULL )
		return;
	memset(mMatGroupCache,0,sizeof(BYTE)*mMatGroupCacheSize);
}

Vector3f S_camera;
BOOL IsFarDistance(_BSP_LEAF *node)
{
	int dist = 
	((int)node->bb_min[0]-(int)S_camera[0])*((int)node->bb_min[0]-(int)S_camera[0])
	+ ((int)node->bb_min[1]-(int)S_camera[1])*((int)node->bb_min[1]-(int)S_camera[1])
	+ ((int)node->bb_min[2]-(int)S_camera[2])*((int)node->bb_min[2]-(int)S_camera[2]);

	if( dist > 6000*6000)
		return TRUE;

	dist = 
	((int)node->bb_max[0]-(int)S_camera[0])*((int)node->bb_max[0]-(int)S_camera[0])
		+ ((int)node->bb_max[1]-(int)S_camera[1])*((int)node->bb_max[1]-(int)S_camera[1])
		+ ((int)node->bb_max[2]-(int)S_camera[2])*((int)node->bb_max[2]-(int)S_camera[2]);
	if( dist > 6000*6000)
		return TRUE;
	return FALSE;
}

void CBsp::RenderShadowMatGroup(WORD mat_g_id)	//머터리얼에 그림자를 그린다.
{
	if( mMatGroup[mat_g_id].ObjectId )
		return;

	SetVPIPTex2(mStaticVertexBuffer[mMatGroup[mat_g_id].VertexBufferId].m_lpVertexBuffer,mStaticIndexedBuffer.m_lpIndexBuffer);

	float w_matrix[4][4];
	::GetObjectMatrix(w_matrix,mMatGroup[mat_g_id].ObjectId,mObject,R3GetTime()*30.0f);
	GetD3dDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)w_matrix );

	DrawStripOneMatGroup(&mMatGroup[mat_g_id]);
}

BOOL CBsp::SetEventAnimationState(WORD id,DWORD flag)
{
	DWORD i;
	BOOL is_change = FALSE;

	for( i=0; i<mObjectNum; i++)
	{
		if( mEventObjectID[i] != id )
			continue;
		
		is_change = TRUE;
		//원하는 이벤트 아이디다..
		mObject[i].flag &= 0xfff0;
		if( flag == _EVENT_ANI_STATE_FRONT_START )
		{
			mObject[i].now_frame=0;
			mObject[i].flag |= _EVENT_ANI_STATE_FRONT_RUN; 
		}
		else
		if( flag == _EVENT_ANI_STATE_BACK_START )
		{
			mObject[i].now_frame=(float)mObject[i].frames-0.001f;
			mObject[i].flag |= _EVENT_ANI_STATE_BACK_RUN;
		}
	}
	return is_change;
}

BOOL CBsp::SetAllAnimationState(DWORD flag)
{
	DWORD i;
	BOOL is_change = FALSE;

	for( i=0; i<mObjectNum; i++)
	{
		if( !(mObject[i].flag & _EVENT_ANI) )
			continue;

		is_change = TRUE;
		//원하는 이벤트 아이디다..
		mObject[i].flag &= 0xfff0;
		if( flag == _EVENT_ANI_STATE_FRONT_START )
		{
			mObject[i].now_frame=0;
			mObject[i].flag |= _EVENT_ANI_STATE_FRONT_RUN; 
		}
		else
		if( flag == _EVENT_ANI_STATE_BACK_START )
		{
			mObject[i].now_frame=(float)mObject[i].frames-0.001f;
			mObject[i].flag |= _EVENT_ANI_STATE_BACK_RUN;
		}
	}
	return is_change;
}


DWORD CBsp::GetEventAnimationState(WORD id)
{
	DWORD i;

	for( i=0; i<mObjectNum; i++)
	{
		if( mEventObjectID[i] != id )
			continue;
		
		return (mObject[i].flag & 0x000f);
	}
	return 0;
}

//-- 이벤트 애니메이션과 
void CBsp::GetBspObjectMatrix(float w_matrix[4][4],WORD id)
{
	float time=0;

	if( id  )
	{
		id-=1;	//인덱스 맞추려구..
		if( mObject[id].flag & _EVENT_ANI )
		{
			DWORD state = (mObject[id].flag&0xf);
			if( state == _EVENT_ANI_STATE_FRONT_RUN )
			{
				if( mObject[id].AniFrameCache == 0 )
					mObject[id].now_frame+=R3GetLoopTime()*30;
				if( mObject[id].now_frame >= mObject[id].frames )
				{
					mObject[id].flag &= 0xfff0;
					mObject[id].flag |= _EVENT_ANI_STATE_BACK_STOP;
					mObject[id].now_frame = (float)mObject[id].frames-0.001f;
				}
			}
			else
			if( state == _EVENT_ANI_STATE_BACK_RUN )
			{
				if( mObject[id].AniFrameCache == 0 )
					mObject[id].now_frame-=R3GetLoopTime()*30;
				if( mObject[id].now_frame < 0 )
				{
					mObject[id].flag &= 0xfff0;
					mObject[id].flag |= _EVENT_ANI_STATE_FRONT_STOP;
					mObject[id].now_frame = 0;
				}
			}
			else
			if( state == _EVENT_ANI_STATE_FRONT_STOP )
				mObject[id].now_frame=0;
			else
			if( state == _EVENT_ANI_STATE_BACK_STOP )
				mObject[id].now_frame=(float)mObject[id].frames-0.001f;

			time = mObject[id].now_frame;
		}
		else
		if( mObject[id].flag & _DYNAMIC_OBJECT )
		{
			time = R3GetTime()*30.0f;
		}
		id+=1;
	}
	::GetObjectMatrix(w_matrix,id,mObject,time);
}

void CBsp::RenderMatGroup(WORD mat_g_id)
{
	if( mMatGroup[mat_g_id].Type & _MAT_CORONA )	//코로나는 제일로 나중에.
	{
		mAlpha.SetCoronaStack(mat_g_id);	//그룹채 셋팅
		return;
	}

	if( (mMatGroup[mat_g_id].Type & (_MAT_GROUP_TYPE_OPPA)) )
	{
		mAlpha.SetAlphaStack(mat_g_id);	//그룹채 셋팅
		return;
	}

	
	SetVPIPTex2(mStaticVertexBuffer[mMatGroup[mat_g_id].VertexBufferId].m_lpVertexBuffer,mStaticIndexedBuffer.m_lpIndexBuffer);

//	if( mMatGroup[mat_g_id].ObjectId )
//		GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );

	float w_matrix[4][4];
	
	GetBspObjectMatrix(w_matrix,mMatGroup[mat_g_id].ObjectId);
//	GetObjectMatrix(w_matrix,mMatGroup[mat_g_id].ObjectId,mObject,R3GetTime()*30.0f);

	GetD3dDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)w_matrix );
	if( mMatGroup[mat_g_id].Type & _MAT_Y_BILLBOARD	)
	{
		_R3ENGINE_STATE *state = GetR3State();
		//중점 찾고 
		Vector3f pos,src;
		Vector3fSub(mMatGroup[mat_g_id].BBMax,mMatGroup[mat_g_id].BBMin,pos);
		Vector3fDiv(pos,2,pos);
		Vector3fAdd(pos,mMatGroup[mat_g_id].BBMin,pos);

		src[0]=state->mInvMatView.m[3][0];
		src[1]=state->mInvMatView.m[3][1];
		src[2]=state->mInvMatView.m[3][2];
		D3DXMATRIX cal_mat,billboard;
		GetYBillboardMatrix(&billboard,src,pos);//	빌보드
//		GetYBillboardMatrix(&billboard,pos,src);//	빌보드
	    D3DXMatrixIdentity( &cal_mat );
		cal_mat.m[3][0] = -pos[0];
		cal_mat.m[3][1] = -pos[1];
		cal_mat.m[3][2] = -pos[2];
//		D3DXMATRIX temp_mat1 = billboard*cal_mat;
		D3DXMATRIX temp_mat1 = cal_mat*billboard;
		cal_mat.m[3][0] = pos[0];
		cal_mat.m[3][1] = pos[1];
		cal_mat.m[3][2] = pos[2];
		D3DXMATRIX temp_mat2 = temp_mat1*cal_mat;
		temp_mat1 = temp_mat2*state->mMatView;
		GetD3dDevice()->SetTransform( D3DTS_VIEW, &temp_mat1 );
	}

	_R3ENGINE_STATE *state = GetR3State();

	if( state->mShowLightMap )
		DrawLightMapGroup(&mStaticVertexBuffer[mMatGroup[mat_g_id].VertexBufferId],&mMatGroup[mat_g_id]);
	else
		DrawOneMatGroup(&mStaticVertexBuffer[mMatGroup[mat_g_id].VertexBufferId],&mMatGroup[mat_g_id]);

	if( mMatGroup[mat_g_id].Type & _MAT_Y_BILLBOARD	)
		GetD3dDevice()->SetTransform( D3DTS_VIEW, &state->mMatView );

//	if( mMatGroup[mat_g_id].ObjectId )
//		GetD3dDevice()->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
}
//#include "math.h"
void CBsp::RenderIndepentMatGroup(WORD mat_g_id)
{

	float w_matrix[4][4];
	GetBspObjectMatrix(w_matrix,mMatGroup[mat_g_id].ObjectId);
//	GetObjectMatrix(w_matrix,mMatGroup[mat_g_id].ObjectId,mObject,R3GetTime()*30.0f);
	GetD3dDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)w_matrix );

	if( mMatGroup[mat_g_id].Type & _MAT_CORONA	)
	{
		_R3ENGINE_STATE *state = GetR3State();
		//중점 찾고 
		Vector3f src,temp_v;

		src[0]=state->mInvMatView.m[3][0];
		src[1]=state->mInvMatView.m[3][1];
		src[2]=state->mInvMatView.m[3][2];
		Vector3fSub(src,mMatGroup[mat_g_id].Origin,temp_v);
/*
 		float scale=-10;
		float dist=Vector3fDist(temp_v);
		scale =  ( ( ( 2.0f * scale ) / (state->mViewXL) ) - 1.0f ) / state->mMatProj._11;
		scale = scale*100/sqrtf(dist);
*/
		float scale=Vector3fDist(temp_v);
		scale=sqrtf(scale)-state->mNear;
		scale=(0.13f*scale);	//스케일 팩터와 곱을 한다.

		D3DXMATRIX cal_mat,billboard;
		billboard=state->mInvMatView;
		billboard.m[3][0] = 0;
		billboard.m[3][1] = 0;
		billboard.m[3][2] = 0;
//		GetYBillboardMatrix(&billboard,src,pos);//	빌보드
	    D3DXMatrixIdentity( &cal_mat );
		cal_mat.m[3][0] = -mMatGroup[mat_g_id].Origin[0];
		cal_mat.m[3][1] = -mMatGroup[mat_g_id].Origin[1];
		cal_mat.m[3][2] = -mMatGroup[mat_g_id].Origin[2];
//		D3DXMATRIX temp_mat1 = billboard*cal_mat;
		D3DXMATRIX temp_mat1 = cal_mat*billboard;
		cal_mat.m[3][0] = mMatGroup[mat_g_id].Origin[0];
		cal_mat.m[3][1] = mMatGroup[mat_g_id].Origin[1];
		cal_mat.m[3][2] = mMatGroup[mat_g_id].Origin[2];
		cal_mat.m[0][0] *=scale;
		cal_mat.m[1][1] *=scale;
		cal_mat.m[2][2] *=scale;
		D3DXMATRIX temp_mat2 = temp_mat1*cal_mat;
		temp_mat1 = temp_mat2*state->mMatView;
		GetD3dDevice()->SetTransform( D3DTS_VIEW, &temp_mat1 );
	}
	else
	if( mMatGroup[mat_g_id].Type & _MAT_Y_BILLBOARD	)
	{
		_R3ENGINE_STATE *state = GetR3State();
		//중점 찾고 
		Vector3f src;

		src[0]=state->mInvMatView.m[3][0];
		src[1]=state->mInvMatView.m[3][1];
		src[2]=state->mInvMatView.m[3][2];
		D3DXMATRIX cal_mat,billboard;
		GetYBillboardMatrix(&billboard,src,mMatGroup[mat_g_id].Origin);//	빌보드
//		GetYBillboardMatrix(&billboard,pos,src);//	빌보드
	    D3DXMatrixIdentity( &cal_mat );
		cal_mat.m[3][0] = -mMatGroup[mat_g_id].Origin[0];
		cal_mat.m[3][1] = -mMatGroup[mat_g_id].Origin[1];
		cal_mat.m[3][2] = -mMatGroup[mat_g_id].Origin[2];
//		D3DXMATRIX temp_mat1 = billboard*cal_mat;
		D3DXMATRIX temp_mat1 = cal_mat*billboard;
		cal_mat.m[3][0] = mMatGroup[mat_g_id].Origin[0];
		cal_mat.m[3][1] = mMatGroup[mat_g_id].Origin[1];
		cal_mat.m[3][2] = mMatGroup[mat_g_id].Origin[2];
		D3DXMATRIX temp_mat2 = temp_mat1*cal_mat;
		temp_mat1 = temp_mat2*state->mMatView;
		GetD3dDevice()->SetTransform( D3DTS_VIEW, &temp_mat1 );
	}
/*		//볼륨 포그에서 어떻게 되는건지확인 할려고......
extern DWORD CFaceStartVId;
extern DWORD *CVertexId;
extern Vector3f *CVertex;
	CVertex=mCVertex;
	CVertexId=mCVertexId;
	CFaceStartVId=mMatGroup[mat_g_id].CFaceStartVId;
*/


	_R3MATERIAL* mat = &GetMainMaterial()[mMatGroup[mat_g_id].MtlId];

//언리얼2의 나무처럼 하고싶니?  그럼 이렇게 해.... 이게 아에 디폴트다..
	if( !(mat->m_Layer[0].m_dwAlphaType & _MAT_ZWRITE) )
		GetD3dDevice()->SetRenderState( D3DRS_ZWRITEENABLE , FALSE );
	
	SetVPIPTex2(mStaticVertexBuffer[mMatGroup[mat_g_id].VertexBufferId].m_lpVertexBuffer,mStaticIndexedBuffer.m_lpIndexBuffer);
	DrawOneMatGroup(&mStaticVertexBuffer[mMatGroup[mat_g_id].VertexBufferId],&mMatGroup[mat_g_id]);

	if( !(mat->m_Layer[0].m_dwAlphaType & _MAT_ZWRITE) )
		GetD3dDevice()->SetRenderState( D3DRS_ZWRITEENABLE , TRUE );

	if( mMatGroup[mat_g_id].Type & _MAT_Y_BILLBOARD	)
		GetD3dDevice()->SetTransform( D3DTS_VIEW, &GetR3State()->mMatView );
}


void CBsp::RenderLeaf(short leaf_id)
{
	int i;

	for(i=0; i<mLeaf[leaf_id].m_group_num; i++)
	{
		DWORD mat_group_id= (DWORD)MatListInLeafId[mLeaf[leaf_id].m_group_start_id+i];

//		if( IsRenderedMatGroup(mat_group_id) )
//			continue;
//		RenderedMatGroup(mat_group_id);

		DrawOneMatGroup(mStaticVertexBuffer,&mMatGroup[mat_group_id]);
//-----그냥 폴리곤...
//		DrawOneMatGroup(mStaticVertexBuffer,mStaticIndexeBuffer,mMatGroup[mat_group_id].VBMinIndex
//			,mMatGroup[mat_group_id].IBMinIndex,mMatGroup[mat_group_id].VCnt,mMatGroup[mat_group_id].TriNum);
	}
}


#define RenderedEntitiesID(num)	(((BYTE*)mEntityCache)[num>>3]|=(1<<(num&7)))
#define IsRenderedEntitiesID(num) (((BYTE*)mEntityCache)[num>>3]&(1<<(num&7)))


void CBsp::WalkLeaf(short leaf_id)
{
	int i;

	if(leaf_id==0)	//빈 leaf면 리턴
		return;
	if( !IsBBoxInFrustum( mLeaf[leaf_id].bb_min,mLeaf[leaf_id].bb_max))
		return;		//보는 영역에서 벗어났다.

	if( mEntityListNum )	//엔티티가 하나라도 있으면.
	{
		for( i=0; i<mLeafEntityList[leaf_id].entities_num; i++)
		{
			RenderedEntitiesID(mEntityID[mLeafEntityList[leaf_id].start_id+i]);
		}
	}


	for(i=0; i<mLeaf[leaf_id].m_group_num; i++)
	{
		DWORD mat_group_id= (DWORD)MatListInLeafId[mLeaf[leaf_id].m_group_start_id+i];

		RenderedMatGroup(mat_group_id);		//플래그만 셋팅해놓는다.
	}

/*
	D3DMATRIX temp;
	GetD3dDevice()->GetTransform(D3DTS_VIEW,&temp);
	GetD3dDevice()->GetTransform(D3DTS_WORLD,&temp);
	GetD3dDevice()->GetTransform(D3DTS_PROJECTION,&temp);
*/

	
}
void CBsp::RenderCollisionLeaf(short leaf_id)
{
	int i;

	if(leaf_id==0)	//빈 leaf면 리턴
		return;

	D3DXMATRIX mat;
	D3DXMatrixIdentity( &mat );
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &mat );

void DrawPrimitivePoly(int num,void *vbuffer);
static _D3DR3VERTEX_TEX0 stIMvertex[1000*3];
	//..............충돌 폴리곤..............

	int start_f = mLeaf[leaf_id].face_start_id;

	int cnt=0,j;

	for( i=0; i<mLeaf[leaf_id].face_num; i++)
	{
		DWORD f_id = mCFaceId[start_f+i];

		for( j=0; j<mCFace[f_id].VNum-2; j++)
		{
			DWORD tc = ((i*20+250)%256),v;
			tc=(0xff000000)|(tc<<16)|(tc<<8)|(tc<<0);
			v=mCVertexId[0+mCFace[f_id].VStartId];
			stIMvertex[cnt].x = mCVertex[v][0];
			stIMvertex[cnt].y = mCVertex[v][1];
			stIMvertex[cnt].z = mCVertex[v][2];
			stIMvertex[cnt].color = tc;
			cnt++;
			v=mCVertexId[j+1+mCFace[f_id].VStartId];
			stIMvertex[cnt].x = mCVertex[v][0];
			stIMvertex[cnt].y = mCVertex[v][1];
			stIMvertex[cnt].z = mCVertex[v][2];
			stIMvertex[cnt].color = tc;
			cnt++;
			v=mCVertexId[j+2+mCFace[f_id].VStartId];
			stIMvertex[cnt].x = mCVertex[v][0];
			stIMvertex[cnt].y = mCVertex[v][1];
			stIMvertex[cnt].z = mCVertex[v][2];
			stIMvertex[cnt].color = tc;
			cnt++;
			if( cnt>=3000)
			{
				DrawPrimitiveUPPoly(cnt/3,stIMvertex);
				cnt=0;
			}
		}
	}
	DrawPrimitiveUPPoly(cnt/3,stIMvertex);
}

//static int delval=0;
void CBsp::WalkNode(short node_id)
{
	if(node_id==0)	//빈 노드면 리턴
		return;
	_BSP_NODE *node=&mNode[node_id];

	//노드타는게 장난이 아니다... 그래서 뷰영역 벗어나는거면 제거한다. 이 방법이 훨낫다.
	if( !IsBBoxInFrustum( node->bb_min,node->bb_max))
		return;		//벗어났다.

	float result= DotProduct(mCNNormal[node->f_normal_id],mTempCamera)-node->d;

	if(result >= 0)	//front
	{
		if( node->front<0 )
			WalkLeaf(-node->front-1);
		else
			WalkNode(node->front);

		if( node->back<0 )
			WalkLeaf(-node->back-1);
		else
			WalkNode(node->back);
	}
	else
	{
		if( node->back<0 )
			WalkLeaf(-node->back-1);
		else
			WalkNode(node->back);

		if( node->front<0 )
			WalkLeaf(-node->front-1);
		else
			WalkNode(node->front);
	}
}


void CBsp::SearchNode(short node_id)
{
	if(node_id==0)	//빈 노드면 리턴
		return ;
	_BSP_NODE *node=&mNode[node_id];
	float result= DotProduct(mCNNormal[node->f_normal_id],mTempCamera)-node->d;

	if(result >= 0)	//front
	{
		if( node->front<0 )
			mTempSearchOk = (-node->front-1);
		else
			SearchNode(node->front);
		
		if( mTempSearchOk >0)
			return;

		if( node->back<0 )
			mTempSearchOk = (-node->back-1);
		else
			SearchNode(node->back);

		if( mTempSearchOk >0)
			return;
	}
	else
	{
		if( node->back<0 )
			mTempSearchOk = (-node->back-1);
		else
			SearchNode(node->back);
		if( mTempSearchOk >0)
			return;

		if( node->front<0 )
			mTempSearchOk = (-node->front-1);
		else
			SearchNode(node->front);
		if( mTempSearchOk >0)
			return;
	}
}

short CBsp::GetLeafNum(Vector3f camera)
{
	Vector3fCopy(camera,mTempCamera);
	mTempSearchOk=-1;
	SearchNode(1);
	return mTempSearchOk;
}
void CBsp::PrepareAnimation()	//애니메이션 하기전에 준비할것들..
{
	DWORD i;
	for(i=0; i<mObjectNum; i++)
	{
		mObject[i].AniFrameCache = 0;
	}
}

void CBsp::ReadyBspRender(Vector3f camera)
{
	Vector3fCopy(camera,mTempCamera);
	SetMultiLayerCamera(camera);

	//노드를 돌아다니기전 할것들
	LoopInitRenderedMatGroup();		//그룹 리스트 캐쉬 초기화
	mAlpha.LoopInitAlphaStack();			//알파 리스트 스텍 초기화

	if( mEntityCache )	//엔티티들 캐쉬클리어링...
		memset(mEntityCache,0,sizeof(BYTE)*mEntityCacheSize);

	PrepareAnimation();	//애니메이션 초기화..
	WalkNode(1);		//자 가볼까...


//	short jj = GetLeafNum( camera);
//	RenderLeaf(jj);
//	WalkLeaf(jj);

//	extern void TestAllGroup(void *vb,void *ib,int v_num,int face_num);
//	TestAllGroup(mStaticVertexBuffer,mStaticIndexeBuffer,mVertexNum,mFaceNum);

	mNowLeafNum=GetLeafNum(camera);	//현재 플레이어의 리프를 알아낸다.
}

void CBsp::RenderOneEntityRender(WORD entities_id)	//맵 엔티티하나 실레로 렌더링.
{
	int id = mMapEntitiesList[entities_id].ID;

	if( !mEntityList[id].IsFileExist )	//엔티티가 로딩이 되지 않았따.
		return;

	float w_matrix[4][4];
	float w_t_matrix[4][4];
	float s_matrix[4][4];//스케일 메트릭스
	float r_matrix[4][4];//로테이트 메트릭스

	MatrixIdentity(w_matrix);
	MatrixScale(s_matrix,mMapEntitiesList[entities_id].Scale
		,mMapEntitiesList[entities_id].Scale,mMapEntitiesList[entities_id].Scale);
	MatrixRotate(r_matrix,mMapEntitiesList[entities_id].RotX,mMapEntitiesList[entities_id].RotY,0);
	MatrixMultiply(w_t_matrix,s_matrix,w_matrix);
	MatrixMultiply(w_matrix,r_matrix,w_t_matrix);
	w_matrix[3][0] = mMapEntitiesList[entities_id].Pos[0];
	w_matrix[3][1] = mMapEntitiesList[entities_id].Pos[1];
	w_matrix[3][2] = mMapEntitiesList[entities_id].Pos[2];

	mEntity[id].mAddFrame = mMapEntitiesList[entities_id].AddFrame;
	DWORD color = mMapEntitiesList[entities_id].Color;
	//애니메이션을 틀리게 하기위한 작업....
	if( mEntityList[id].IsParticle )	//파티클인가?
	{
 		CParticle *particle = mMapEntitiesList[entities_id].Particle;
		if( particle == NULL )
			return;
		if( particle->Loop() ==0 )
			return;
		MatrixCopy(particle->mRotMat,r_matrix);
		particle->SetCreatePos(w_matrix[3]);	//충돌 할땐 필요하다...
		DrawCParticle(particle,particle->mEntity,w_matrix,color);	//파티클일경우 파티클을 뿌려주자.
	}
	else
	{
		mEntity[id].SetMapColor(mMapEntitiesList[entities_id].mMapColor);
		if( mEntityList[id].ShaderID )//쉐이더가 있으면...
		{
			mEntity[id].DrawEntityVS(&mEntityList[id],w_matrix,color);	//월드매트릭스,버텍스쉐이더 쓰는 엔티티출력루틴... 
		}
		else
		{
			mEntity[id].DrawEntity(w_matrix,color);	
		}
	}
}

void CBsp::DrawMapEntitiesRender()	//맵 엔티티들 알파 스택에 렌더링푸쉬.
{
	BYTE *cache = (BYTE *)mEntityCache;
	int i,j,bit,entities_id;

	_R3ENGINE_STATE *state = GetR3State();
	Vector3f cam_pos;
	cam_pos[0] = state->mInvMatView.m[3][0];
	cam_pos[1] = state->mInvMatView.m[3][1];
	cam_pos[2] = state->mInvMatView.m[3][2];

	//파티클을 나중에 돌리게.........*******주의 
	for(i=mEntityCacheSize-1; i>=0; i--)
	{
		if( !cache[i] )	
			continue;
			
		//해당 비트가 켜졌으면...	렌더링을 해주자.
		for(j=0,bit=0x1; j<8; j++,bit<<=1)
		{
			if( cache[i]&bit )
			{
				entities_id = i*8+j;

				DWORD alpha;
				float dist;
				Vector3f pos;

				float min_dist = 400;
				float max_dist = 700;
				int id = mMapEntitiesList[entities_id].ID;

				Vector3fSub( cam_pos,mMapEntitiesList[entities_id].Pos,pos );
				dist = Vector3fDist( pos );
				max_dist=mEntityList[id].FadeEnd;
				if( dist > max_dist )
					continue;
				min_dist=mEntityList[id].FadeStart;

				if( !IsBBoxInFrustum( mMapEntitiesList[entities_id].BBMin 
					,mMapEntitiesList[entities_id].BBMax ))
					continue;

				dist = max(dist,min_dist);
				alpha = (DWORD)((1-(dist - min_dist)/(max_dist - min_dist))*255);
				mMapEntitiesList[entities_id].Color=(0x00ffffff|(alpha<<24));

				if( mEntity[id].IsAlpha() || mEntityList[id].IsParticle )
					mAlpha.SetAlphaEntityStack(entities_id);	//알파와 파티클은 알파소팅.
				else
					RenderOneEntityRender(entities_id);		//다른거는 그냥 뿌리자.

//				if( IsInViewFrustum(entities_id) )
			}
		}
	}
	//엔티티들이 소팅이 필요한 경우는 따로 해주자.
}

BOOL CBsp::IsInViewFrustum(WORD mat_id)
{
	if( mMatGroup[mat_id].ObjectId )	//애니메이션은 일단 참.
		return TRUE;
	return IsBBoxInFrustum( mMatGroup[mat_id].BBMin ,mMatGroup[mat_id].BBMax );
}
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

void CBsp::DrawBspRender()
{
	BYTE *cache = (BYTE *)mMatGroupCache;
	int i,j,mat_id;
	BYTE bit;

	if( GetR3State()->mIsFog )
	{
		GetD3dDevice()->SetRenderState( D3DRS_FOGENABLE, TRUE );
//		GetD3dDevice()->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_EXP2 );
//		GetD3dDevice()->SetRenderState( D3DRS_FOGDENSITY, FtoDW(0.5f) );
	}

	//	SetVPIPTex2(mStaticVertexBuffer,mStaticIndexedBuffer);

	if( !cache )
		return;

	MultiTexOn();

	mNowRenderMatGroupNum=0;	// 현재 렌더링 되고 잇는 머터리얼 블록 갯수.
	for(i=mMatGroupCacheSize-1; i>=0; i--)
	{
		if( !cache[i] )	
			continue;
		
		//해당 비트가 켜졌으면...
		for(j=0,bit=0x1; j<8; j++)
		{
			if( cache[i]&bit )
			{
				mat_id = i*8+j;

				if( IsInViewFrustum(mat_id) )//확실하게 뷰영역에 있는 블록만 계산한다.
				{
					mNowRenderMatGroupNum++;
					RenderMatGroup(mat_id);
				}
				else	
				{
					//영역에 벗어난것은 꺼준다. 나중에 쉐도우 계산을 위해서.
					cache[i]&=(~bit);
				}

			}
			bit<<=1;
		}
	}
	MultiTexOff();

	// 잠시 테스트.
//	if( mNowPlayerNum >0 )
//	WalkLeaf(mNowPlayerNum);
/*
//----쉐도우 테스트.
	SetProjectShadow(lgt_pos,lgt_pos);	//project 쉐도우 들어가기전에 호출할것.
	for(i=mMatGroupCacheSize-1; i>=0; i--)
	{
		if( !cache[i] )	
			continue;
			
		//해당 비트가 켜졌으면...
		for(j=0,bit=0x1; j<8; j++)
		{
			if( cache[i]&bit )
			{
				mat_id = i*8+j;
				if( IsInViewFrustum(mat_id) )
				{
					RenderShadowMatGroup(mat_id);	//쉐도우 테스트.
				}
			}
			bit<<=1;
		}
	}
	UnSetProjectedShadow();	//proejct 쉐도우 실행하고 나서 호출할것.
*/


/*	
	//-----마우스로 픽킹된 폴리곤처리...
	void DrawPrimitivePoly(int num,void *vbuffer);
	_D3DR3VERTEX_TEX0 stIMvertex[1000*3];
	if( mPickPoly != -1)
	{
		int cnt=0;
		for( j=0; j<mCFace[mPickPoly].VNum-2; j++)
		{
			DWORD tc = ((i*20+250)%256),v;
			tc=(0x80ff4f00);
			v=mCVertexId[0+mCFace[mPickPoly].VStartId];
			stIMvertex[cnt].x = mCVertex[v][0];
			stIMvertex[cnt].y = mCVertex[v][1];
			stIMvertex[cnt].z = mCVertex[v][2];
			stIMvertex[cnt].color = tc;
			cnt++;
			v=mCVertexId[j+1+mCFace[mPickPoly].VStartId];
			stIMvertex[cnt].x = mCVertex[v][0];
			stIMvertex[cnt].y = mCVertex[v][1];
			stIMvertex[cnt].z = mCVertex[v][2];
			stIMvertex[cnt].color = tc;
			cnt++;
			v=mCVertexId[j+2+mCFace[mPickPoly].VStartId];
			stIMvertex[cnt].x = mCVertex[v][0];
			stIMvertex[cnt].y = mCVertex[v][1];
			stIMvertex[cnt].z = mCVertex[v][2];
			stIMvertex[cnt].color = tc;
			cnt++;
		}
		GetD3dDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
		GetD3dDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		GetD3dDevice()->SetRenderState( D3DRS_ZENABLE, FALSE );
		GetD3dDevice()->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		GetD3dDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		DrawPrimitiveUPPoly(cnt/3,stIMvertex);
		GetD3dDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );
		GetD3dDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	}
*/

//	DrawGroupStack();//스택에 있는 그룹들을 렌더링한다.

//	if( GetR3State()->mIsFog )
//		GetD3dDevice()->SetRenderState( D3DRS_FOGENABLE, FALSE );
}

void CBsp::PrepareShadowRender(Vector3f ch_pos,void *tex,float intensity,DWORD blur,float scale,float add_pos)	// 
{
	ResetVertexBufferCache();	//버텍스버퍼 셋팅을 위한.렌더링 구조에서 시작 할때 반드시 호출하자.. 
	SetProjectShadow(ch_pos,tex,intensity,blur,scale,add_pos);
}

void CBsp::DrawShadowRender(Vector3f ch_pos)	// 
{
	/*
	ResetVertexBufferCache();	//버텍스버퍼 셋팅을 위한.렌더링 구조에서 시작 할때 반드시 호출하자.. 
	SetProjectShadow(ch_pos,tex,intensity,blur,scale,add_pos);
*/
	int i,j;
	Vector3f lgt;
	Vector3fCopy(GetR3State()->mLightNomalForShadow,lgt);

	Vector3f ch_uppos;
	Vector3fCopy( ch_pos, ch_uppos );
	ch_uppos[1]+=10;
	ch_pos[1]-=10;
	LONG leaf_num=0;
	SHORT leaf_list[256];
	GetLeafList(ch_pos,ch_uppos,&leaf_num,leaf_list,256);
	ch_pos[1]+=22;

void DrawPrimitivePoly(int num,void *vbuffer);
static _D3DR3VERTEX_TEX0 stIMvertex[1000*3];
	//..............충돌 폴리곤..............

	int cnt=0,k;
	DWORD f_list[3000];
	DWORD f_cnt=0;
	for(k=0; k<leaf_num; k++)
	{
		DWORD leaf_id= leaf_list[k];
		int start_f = mLeaf[leaf_id].face_start_id;
		for( i=0; i<mLeaf[leaf_id].face_num; i++)
		{
			DWORD f_id = mCFaceId[start_f+i];

			if( mCFace[f_id].Attr & (_CFACE_ATTR_NOT_FLOOR|_CFACE_WATER) )
				continue;
			float dist = Vector3fDot(lgt,mCFace[f_id].Normal);
			if( dist < 0.1 )	//뒤집힌면들은 제외하자.
				continue;
			
			dist = Vector3fDot(ch_pos,mCFace[f_id].Normal)-mCFace[f_id].Normal[3];
			if( dist < 0.1 )	//캐릭터점의 앞에있는 면은 제외하자.
				continue;			

			for(DWORD m=0; m<f_cnt; m++)	//겹친면 제외
				if( f_list[m]==f_id )
					goto SKIP_FACE;
			f_list[f_cnt++] = f_id;

			for( j=0; j<mCFace[f_id].VNum-2; j++)
			{
				DWORD v;
				v=mCVertexId[0+mCFace[f_id].VStartId];
				stIMvertex[cnt].x = mCVertex[v][0];
				stIMvertex[cnt].y = mCVertex[v][1];
				stIMvertex[cnt].z = mCVertex[v][2];
				cnt++;
				v=mCVertexId[j+1+mCFace[f_id].VStartId];
				stIMvertex[cnt].x = mCVertex[v][0];
				stIMvertex[cnt].y = mCVertex[v][1];
				stIMvertex[cnt].z = mCVertex[v][2];
				cnt++;
				v=mCVertexId[j+2+mCFace[f_id].VStartId];
				stIMvertex[cnt].x = mCVertex[v][0];
				stIMvertex[cnt].y = mCVertex[v][1];
				stIMvertex[cnt].z = mCVertex[v][2];
				cnt++;
				if( cnt>=3000)
				{
					GetD3dDevice()->SetVertexShader( D3DFVF_R3VERTEX_TEX0 );
					float w_matrix[4][4];
					MatrixIdentity(w_matrix);
					GetD3dDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)w_matrix );
					GetD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,cnt/3,stIMvertex,sizeof(_D3DR3VERTEX_TEX0));
					cnt=0;
				}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
			}
SKIP_FACE:
			int kl=0;
		}
	}
ch_pos[1]-=12;
	if( cnt )
	{
		GetD3dDevice()->SetVertexShader( D3DFVF_R3VERTEX_TEX0 );
		float w_matrix[4][4];
		MatrixIdentity(w_matrix);
		GetD3dDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)w_matrix );
		GetD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,cnt/3,stIMvertex,sizeof(_D3DR3VERTEX_TEX0));
	}

	UnSetProjectedShadow();
	ResetVertexBufferCache();	
}

void CBsp::DrawDynamicLightSub(Vector3f bb_min,Vector3f bb_max)
{
	int i,j;
	LONG leaf_num=0;
	SHORT leaf_list[256];
	GetFastLeafListFromBBox(bb_min,bb_max,&leaf_num,leaf_list,256);

void DrawPrimitivePoly(int num,void *vbuffer);
static _D3DR3VERTEX_TEX0 stIMvertex[1000*3];
	//..............충돌 폴리곤..............

	int cnt=0,k;
	DWORD f_list[3000];
	DWORD f_cnt=0;
	for(k=0; k<leaf_num; k++)
	{
		DWORD leaf_id= leaf_list[k];
		int start_f = mLeaf[leaf_id].face_start_id;
		for( i=0; i<mLeaf[leaf_id].face_num; i++)
		{
			DWORD f_id = mCFaceId[start_f+i];
/*
			if( mCFace[f_id].Attr & (_CFACE_ATTR_NOT_FLOOR|_CFACE_WATER) )
				continue;
			float dist = Vector3fDot(lgt,mCFace[f_id].Normal);
			if( dist < 0.1 )	//뒤집힌면들은 제외하자.
				continue;
*/
			for(DWORD m=0; m<f_cnt; m++)
				if( f_list[m]==f_id )
					goto D_SKIP_FACE;
			f_list[f_cnt++] = f_id;

			for( j=0; j<mCFace[f_id].VNum-2; j++)
			{
				DWORD v;
				v=mCVertexId[0+mCFace[f_id].VStartId];
				stIMvertex[cnt].x = mCVertex[v][0];
				stIMvertex[cnt].y = mCVertex[v][1];
				stIMvertex[cnt].z = mCVertex[v][2];
				cnt++;
				v=mCVertexId[j+1+mCFace[f_id].VStartId];
				stIMvertex[cnt].x = mCVertex[v][0];
				stIMvertex[cnt].y = mCVertex[v][1];
				stIMvertex[cnt].z = mCVertex[v][2];
				cnt++;
				v=mCVertexId[j+2+mCFace[f_id].VStartId];
				stIMvertex[cnt].x = mCVertex[v][0];
				stIMvertex[cnt].y = mCVertex[v][1];
				stIMvertex[cnt].z = mCVertex[v][2];
				cnt++;
				if( cnt>=3000)
				{
					GetD3dDevice()->SetVertexShader( D3DFVF_R3VERTEX_TEX0 );
					float w_matrix[4][4];
					MatrixIdentity(w_matrix);
					GetD3dDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)w_matrix );
					GetD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,cnt/3,stIMvertex,sizeof(_D3DR3VERTEX_TEX0));
					cnt=0;
				}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
			}
D_SKIP_FACE:
			int kl=0;
		}
	}
	if( cnt )
	{
		GetD3dDevice()->SetVertexShader( D3DFVF_R3VERTEX_TEX0 );
		float w_matrix[4][4];
		MatrixIdentity(w_matrix);
		GetD3dDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)w_matrix );
		GetD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,cnt/3,stIMvertex,sizeof(_D3DR3VERTEX_TEX0));
	}
}

void CBsp::DrawMagicLightSub(Vector3f bb_min,Vector3f bb_max)
{
	int i,j;
	LONG leaf_num=0;
	SHORT leaf_list[256];
	GetFastLeafListFromBBox(bb_min,bb_max,&leaf_num,leaf_list,256);

void DrawPrimitivePoly(int num,void *vbuffer);
static _D3DR3VERTEX_TEX0 stIMvertex[1000*3];
	//..............충돌 폴리곤..............

	int cnt=0,k;
	DWORD f_list[3000];
	DWORD f_cnt=0;
	for(k=0; k<leaf_num; k++)
	{
		DWORD leaf_id= leaf_list[k];
		int start_f = mLeaf[leaf_id].face_start_id;
		for( i=0; i<mLeaf[leaf_id].face_num; i++)
		{
			DWORD f_id = mCFaceId[start_f+i];

			if( mCFace[f_id].Attr & _CFACE_WATER )
				continue;
			if( mCFace[f_id].Normal[1] <=0 )	//뒤집힌면들은 제외하자.
				continue;

			for(DWORD m=0; m<f_cnt; m++)
				if( f_list[m]==f_id )
					goto D_SKIP_FACE;
			f_list[f_cnt++] = f_id;

			for( j=0; j<mCFace[f_id].VNum-2; j++)
			{
				DWORD v;
				v=mCVertexId[0+mCFace[f_id].VStartId];
				stIMvertex[cnt].x = mCVertex[v][0];
				stIMvertex[cnt].y = mCVertex[v][1];
				stIMvertex[cnt].z = mCVertex[v][2];
				cnt++;
				v=mCVertexId[j+1+mCFace[f_id].VStartId];
				stIMvertex[cnt].x = mCVertex[v][0];
				stIMvertex[cnt].y = mCVertex[v][1];
				stIMvertex[cnt].z = mCVertex[v][2];
				cnt++;
				v=mCVertexId[j+2+mCFace[f_id].VStartId];
				stIMvertex[cnt].x = mCVertex[v][0];
				stIMvertex[cnt].y = mCVertex[v][1];
				stIMvertex[cnt].z = mCVertex[v][2];
				cnt++;
				if( cnt>=3000)
				{
					GetD3dDevice()->SetVertexShader( D3DFVF_R3VERTEX_TEX0 );
					float w_matrix[4][4];
					MatrixIdentity(w_matrix);
					GetD3dDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)w_matrix );
					GetD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,cnt/3,stIMvertex,sizeof(_D3DR3VERTEX_TEX0));
					cnt=0;
				}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
			}
D_SKIP_FACE:
			int kl=0;
		}
	}
	if( cnt )
	{
		GetD3dDevice()->SetVertexShader( D3DFVF_R3VERTEX_TEX0 );
		float w_matrix[4][4];
		MatrixIdentity(w_matrix);
		GetD3dDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)w_matrix );
		GetD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,cnt/3,stIMvertex,sizeof(_D3DR3VERTEX_TEX0));
	}
}



void CBsp::DrawDynamicLights()	// 
{
	_R3ENGINE_STATE *state= GetR3State();
	DWORD d_num = min(GetDynamicLightNum(),state->mDynamicLight);
	if( !d_num )
		return;
//	ResetVertexBufferCache();	//버텍스버퍼 셋팅을 위한.렌더링 구조에서 시작 할때 반드시 호출하자.. 

	DWORD i=0;
	Vector3f bb_min,bb_max,pos;
	float diameter,radious;
	DWORD color;
	void *tex;
	BOOL multi_tex;
	DWORD blend_type;

	for( i=0; i< d_num; i++ )
	{
		GetDynamicLight(i,pos,&diameter,&color,&tex,&blend_type);

		radious=diameter/7.0f;

		bb_min[0] = pos[0]-radious;
		bb_min[1] = pos[1]-radious;
		bb_min[2] = pos[2]-radious;

		bb_max[0] = pos[0]+radious;
		bb_max[1] = pos[1]+radious;
		bb_max[2] = pos[2]+radious;
		multi_tex = !IsMagicLight(i);		//매직 라이트는 멀티텍스쳐가 아니다.

		SetDynamicLight(pos,diameter,color,tex,multi_tex,blend_type);	
		if( !multi_tex )
			DrawMagicLightSub(bb_min,bb_max);
		else
			DrawDynamicLightSub(bb_min,bb_max);
	}
	UnSetDynamicLight();
	UnSetProjectedShadow();
	ResetVertexBufferCache();	
}


void CBsp::DrawAlphaRender(Vector3f camera)
{
/*
	DWORD alpha_face_cnt,*sorted;

	alpha_face_cnt=mAlpha.GetAlphaFaceCnt();
	if( alpha_face_cnt == 0 )
		return;		//뿌릴일 없다.

	//정렬부텀하고...
	sorted = mAlpha.SortAlphaStack(camera);
	if( sorted == NULL )	
		return;
*/

    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );

	DrawAlphaStack(this,camera);//스택에 있는 알파 페이스들을 렌더링한다.
	MultiTexOff();
//	if( GetR3State()->mIsFog )
//		GetD3dDevice()->SetRenderState( D3DRS_FOGENABLE, FALSE );
}

float CBsp::GetMatGroupPoint(WORD group_id,Vector3f get)
{
//	Vector3fSub(mMatGroup[group_id].BBMax,mMatGroup[group_id].BBMin,temp);
//	Vector3fDiv(get,2,get);
//	Vector3fAdd(get,mMatGroup[group_id].BBMin,get);
	Vector3f temp;
	Vector3fSub(mMatGroup[group_id].Origin,mMatGroup[group_id].BBMin,temp);
	Vector3fCopy(mMatGroup[group_id].Origin,get);
	return Vector3fDist(temp);
}


void CBsp::GetHeight(Vector3f camera)
{
//	return;
	mNowLeafNum=GetLeafNum(camera);

	Vector3f b_camera,n_camera;
	Vector3fCopy(camera,b_camera);
	Vector3fCopy(camera,n_camera);

//	b_camera[1] += _CHAR_HEIGHT*2;
//	n_camera[1] -= _CHAR_HEIGHT;
	b_camera[1] += 9999;
	n_camera[1] -= 9999;


	float y_height = GetYposInLeaf( b_camera,n_camera,0,camera[1],mNowLeafNum );
	if( y_height != -32000 )
		camera[1] = y_height + _CHAR_HEIGHT*0.9f;
	else
	{
		b_camera[0] += 5;
		n_camera[1] += 5;
		float y_height = GetYposInLeaf( b_camera,n_camera,0,camera[1],mNowLeafNum );
		if( y_height != -32000 )
			camera[1] = y_height + _CHAR_HEIGHT*0.9f;
		else
			camera[1] -= (_CHAR_HEIGHT);
/*		n_camera[1] -= 10000;
		float y_height = GetYposInLeaf( b_camera,n_camera,0,camera[1],mNowLeafNum );
		if( y_height == -32000 )
		{
			camera[1] -= (_CHAR_HEIGHT/2);
		}
*/
	}
}

//static int mLeafIDs;
static LONG stMaxLeafNum;

void CBsp::SubLeafList(float su,_BSP_NODE *node,Vector3f src,Vector3f tar,SHORT *leaf_list,int *leaf_ids)
{
	if(su >= 0)	//front
	{
		if( node->front<0 )
		{
			if( stMaxLeafNum > *leaf_ids )
			{
				leaf_list[*leaf_ids]=-node->front-1;
				*leaf_ids=*leaf_ids+1;
			}
		}
		else
			WalkNodeForLeafList(node->front,src,tar,leaf_list,leaf_ids);
	}
	else
	{
		if( node->back<0 )
		{
			if( stMaxLeafNum > *leaf_ids )
			{
				leaf_list[*leaf_ids]=-node->back-1;
				*leaf_ids=*leaf_ids+1;
			}
		}
		else
			WalkNodeForLeafList(node->back,src,tar,leaf_list,leaf_ids);
	}
}

void CBsp::WalkNodeForLeafList(short node_id,Vector3f src,Vector3f tar,SHORT *leaf_list,int *leaf_ids)
{
	if(node_id==0)	//빈 노드면 리턴
		return;
	_BSP_NODE *node=&mNode[node_id];
	float *normal = mCNNormal[node->f_normal_id];
	Vector3f cross;

	float result1= DotProduct(normal,src)-node->d;
	float result2= DotProduct(normal,tar)-node->d;

	if( result1 <= 0 && result2 > 0 || result2 <= 0 && result1 > 0)	// 갈라지는 경우.
	{
	    float rate = result1 / (result1-result2);
		cross[0] = src[0] + rate*(tar[0]-src[0]);
		cross[1] = src[1] + rate*(tar[1]-src[1]);
		cross[2] = src[2] + rate*(tar[2]-src[2]);
		SubLeafList(result1,node,src,cross,leaf_list,leaf_ids);
		SubLeafList(result2,node,cross,tar,leaf_list,leaf_ids);
	}
	else
		SubLeafList(result1,node,src,tar,leaf_list,leaf_ids);
}

void CBsp::GetLeafList(Vector3f src,Vector3f tar,LONG *num,SHORT *leaf_list,DWORD max_num)
{
	stMaxLeafNum = max_num;	//최대치를 정하고 들어간다.
	int leaf_ids=0;
	WalkNodeForLeafList(1,src,tar,leaf_list,&leaf_ids);
	*num=leaf_ids;
}

void CBsp::SubLeafListFromBBox(float su,_BSP_NODE *node,Vector3f *bb_list,SHORT *leaf_list,int *leaf_ids)
{
	if(su >= 0)	//front
	{
		if( node->front<0 )
		{
			if( stMaxLeafNum > *leaf_ids )
			{
				leaf_list[*leaf_ids]=-node->front-1;
				*leaf_ids=*leaf_ids+1;
			}
		}
		else
			FastWalkNodeForLeafListFromBBox(node->front,bb_list,leaf_list,leaf_ids);
	}
	else
	{
		if( node->back<0 )
		{
			if( stMaxLeafNum > *leaf_ids )
			{
				leaf_list[*leaf_ids]=-node->back-1;
				*leaf_ids=*leaf_ids+1;
			}
		}
		else
			FastWalkNodeForLeafListFromBBox(node->back,bb_list,leaf_list,leaf_ids);
	}
}

void CBsp::FastWalkNodeForLeafListFromBBox(short node_id,Vector3f *bb_list,SHORT *leaf_list,int *leaf_ids)
{
	if(node_id==0)	//빈 노드면 리턴
		return;
	_BSP_NODE *node=&mNode[node_id];
	float *normal = mCNNormal[node->f_normal_id];
	Vector3f bb_list_a[8],bb_list_b[8],cross;
	BOOL is_split=FALSE;
	float r1,r2,rate;
	DWORD i;

#define GetCross(src,tar)	\
	{\
		r1= DotProduct(normal,src)-node->d;\
		r2= DotProduct(normal,tar)-node->d;\
		if( r1 <= 0 && r2 > 0 || r2 <= 0 && r1 > 0){rate = r1 / (r1-r2); is_split=TRUE;	cross[0] = src[0] + rate*(tar[0]-src[0]);	cross[1] = src[1] + rate*(tar[1]-src[1]);	cross[2] = src[2] + rate*(tar[2]-src[2]);	}\
	}

	GetCross(bb_list[0],bb_list[1]);
	if( is_split )	//z평면
	{
		for( i =0; i<8 ;i++)
		{
			Vector3fCopy(bb_list[i],bb_list_a[i]);
			Vector3fCopy(bb_list[i],bb_list_b[i]);
		}
		bb_list_a[1][2] = cross[2];
		bb_list_a[2][2] = cross[2];
		bb_list_a[5][2] = cross[2];
		bb_list_a[6][2] = cross[2];

		bb_list_b[0][2] = cross[2];
		bb_list_b[3][2] = cross[2];
		bb_list_b[4][2] = cross[2];
		bb_list_b[7][2] = cross[2];
	}
	if( !is_split )	// x평면 
	{
		GetCross(bb_list[1],bb_list[2]);
		if( is_split )
		{
			for( i =0; i<8 ;i++)
			{
				Vector3fCopy(bb_list[i],bb_list_a[i]);
				Vector3fCopy(bb_list[i],bb_list_b[i]);
			}
			bb_list_a[2][0] = cross[0];
			bb_list_a[3][0] = cross[0];
			bb_list_a[6][0] = cross[0];
			bb_list_a[7][0] = cross[0];

			bb_list_b[0][0] = cross[0];
			bb_list_b[1][0] = cross[0];
			bb_list_b[4][0] = cross[0];
			bb_list_b[5][0] = cross[0];
		}
	}
	if( !is_split )	// y평면 
	{
		GetCross(bb_list[1],bb_list[5]);
		if( is_split )
		{
			for( i =0; i<8 ;i++)
			{
				Vector3fCopy(bb_list[i],bb_list_a[i]);
				Vector3fCopy(bb_list[i],bb_list_b[i]);
			}
			bb_list_a[4][1] = cross[1];
			bb_list_a[5][1] = cross[1];
			bb_list_a[6][1] = cross[1];
			bb_list_a[7][1] = cross[1];

			bb_list_b[0][1] = cross[1];
			bb_list_b[1][1] = cross[1];
			bb_list_b[2][1] = cross[1];
			bb_list_b[3][1] = cross[1];
		}
	}

	if( is_split )	// 갈라지는 경우.
	{
		SubLeafListFromBBox(r1,node,bb_list_a,leaf_list,leaf_ids);	//front
		SubLeafListFromBBox(r2,node,bb_list_b,leaf_list,leaf_ids);	//back
	}
	else
		SubLeafListFromBBox(r1,node,bb_list,leaf_list,leaf_ids);
}


// 바운딩 박스를 넣어주면 리프리스트를 준다.
void CBsp::GetFastLeafListFromBBox(Vector3f bb_min,Vector3f bb_max,LONG *num,SHORT *leaf_list,DWORD max_num)
{
	Vector3f bb_list[8];
	stMaxLeafNum = max_num;	//최대치를 정하고 들어간다.
	int leaf_ids=0;
	bb_list[0][0] = bb_max[0];	bb_list[0][1] = bb_max[1];  bb_list[0][2] = bb_max[2];
	bb_list[1][0] = bb_max[0];	bb_list[1][1] = bb_max[1];	bb_list[1][2] = bb_min[2];
	bb_list[2][0] = bb_min[0];	bb_list[2][1] = bb_max[1];	bb_list[2][2] = bb_min[2];
	bb_list[3][0] = bb_min[0];	bb_list[3][1] = bb_max[1];	bb_list[3][2] = bb_max[2];

	bb_list[4][0] = bb_max[0];	bb_list[4][1] = bb_min[1];	bb_list[4][2] = bb_max[2];
	bb_list[5][0] = bb_max[0];	bb_list[5][1] = bb_min[1];	bb_list[5][2] = bb_min[2];
	bb_list[6][0] = bb_min[0];	bb_list[6][1] = bb_min[1];	bb_list[6][2] = bb_min[2];
	bb_list[7][0] = bb_min[0];	bb_list[7][1] = bb_min[1];	bb_list[7][2] = bb_max[2];

	FastWalkNodeForLeafListFromBBox(1,bb_list,leaf_list,&leaf_ids);
	*num=leaf_ids;
}

//Vector3f a={-40,2,-1},b={-10,60,40};
void CBsp::DrawLeafBBox()
{
	int cnt=0,i;
	_D3DR3VERTEX_TEX0 vertex[1000*3];
	DWORD tc;


    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
    GetD3dDevice()->SetTexture( 0,NULL);
//    GetD3dDevice()->SetRenderState( D3DRS_ZENABLE, FALSE );

	tc=(0x80ff4f00);
//	LineSet(a[0],a[1],a[2],tc);
//	LineSet(b[0],b[1],b[2],tc);

#define LineSet(xx,yy,zz,cc) {	vertex[cnt].x = xx;	vertex[cnt].y = yy;	vertex[cnt].z = zz;		vertex[cnt].color = cc; cnt++;};


	for( i=1; i<(int)mLeafNum; i++)
	{
//		if( IsBothRayAABB(mLeaf[i].bb_min,mLeaf[i].bb_max,a,b) == FALSE )
//			continue;

//		if( i!= 140 && i!=138 && i!= 162 && i!=157 )
//			continue;
		if( i==mNowPlayerNum)//3574 )
			tc=(0x80ff4f00);
		else
		{
			tc=(0x8000ff4f);
		continue;
		}

//	static int lef_id = 14;
//		if( i == lef_id )
//			tc=(0x804fff00);
//		else

//		tc=(0x80ff4f00);

		LineSet(mLeaf[i].bb_min[0],mLeaf[i].bb_min[1],mLeaf[i].bb_min[2],tc);
		LineSet(mLeaf[i].bb_max[0],mLeaf[i].bb_min[1],mLeaf[i].bb_min[2],tc);
		LineSet(mLeaf[i].bb_min[0],mLeaf[i].bb_min[1],mLeaf[i].bb_min[2],tc);
		LineSet(mLeaf[i].bb_min[0],mLeaf[i].bb_min[1],mLeaf[i].bb_max[2],tc);
		LineSet(mLeaf[i].bb_max[0],mLeaf[i].bb_min[1],mLeaf[i].bb_min[2],tc);
		LineSet(mLeaf[i].bb_max[0],mLeaf[i].bb_min[1],mLeaf[i].bb_max[2],tc);
		LineSet(mLeaf[i].bb_min[0],mLeaf[i].bb_min[1],mLeaf[i].bb_max[2],tc);
		LineSet(mLeaf[i].bb_max[0],mLeaf[i].bb_min[1],mLeaf[i].bb_max[2],tc);

		LineSet(mLeaf[i].bb_min[0],mLeaf[i].bb_max[1],mLeaf[i].bb_min[2],tc);
		LineSet(mLeaf[i].bb_max[0],mLeaf[i].bb_max[1],mLeaf[i].bb_min[2],tc);
		LineSet(mLeaf[i].bb_min[0],mLeaf[i].bb_max[1],mLeaf[i].bb_min[2],tc);
		LineSet(mLeaf[i].bb_min[0],mLeaf[i].bb_max[1],mLeaf[i].bb_max[2],tc);
		LineSet(mLeaf[i].bb_max[0],mLeaf[i].bb_max[1],mLeaf[i].bb_min[2],tc);
		LineSet(mLeaf[i].bb_max[0],mLeaf[i].bb_max[1],mLeaf[i].bb_max[2],tc);
		LineSet(mLeaf[i].bb_min[0],mLeaf[i].bb_max[1],mLeaf[i].bb_max[2],tc);
		LineSet(mLeaf[i].bb_max[0],mLeaf[i].bb_max[1],mLeaf[i].bb_max[2],tc);

		LineSet(mLeaf[i].bb_min[0],mLeaf[i].bb_min[1],mLeaf[i].bb_min[2],tc);
		LineSet(mLeaf[i].bb_min[0],mLeaf[i].bb_max[1],mLeaf[i].bb_min[2],tc);
		LineSet(mLeaf[i].bb_max[0],mLeaf[i].bb_min[1],mLeaf[i].bb_min[2],tc);
		LineSet(mLeaf[i].bb_max[0],mLeaf[i].bb_max[1],mLeaf[i].bb_min[2],tc);
		LineSet(mLeaf[i].bb_min[0],mLeaf[i].bb_min[1],mLeaf[i].bb_max[2],tc);
		LineSet(mLeaf[i].bb_min[0],mLeaf[i].bb_max[1],mLeaf[i].bb_max[2],tc);
		LineSet(mLeaf[i].bb_max[0],mLeaf[i].bb_min[1],mLeaf[i].bb_max[2],tc);
		LineSet(mLeaf[i].bb_max[0],mLeaf[i].bb_max[1],mLeaf[i].bb_max[2],tc);

		if( cnt > 1000)
		{
			DrawPrimitiveUPLine(cnt/2,vertex);
			cnt=0;
		}
	}
	if( cnt )
		DrawPrimitiveUPLine(cnt/2,vertex);
//    GetD3dDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );
}
void CBsp::DrawMatBBox()
{
	int cnt=0,i;
	_D3DR3VERTEX_TEX0 vertex[1000*3];
	DWORD tc;


    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
    GetD3dDevice()->SetTexture( 0,NULL);
//    GetD3dDevice()->SetRenderState( D3DRS_ZENABLE, FALSE );

	tc=(0x80ff4f00);
//	LineSet(a[0],a[1],a[2],tc);
//	LineSet(b[0],b[1],b[2],tc);

#define LineSet(xx,yy,zz,cc) {	vertex[cnt].x = xx;	vertex[cnt].y = yy;	vertex[cnt].z = zz;		vertex[cnt].color = cc; cnt++;};

	for( i=1; i<(int)mMatGroupNum; i++)
	{

//		if( i!= 140 && i!=138 && i!= 162 && i!=157 )
//			continue;
//		if( i== 140 || i==138 )
//			tc=(0x80ff4f00);
//		else
//			tc=(0x8000ff4f);

		LineSet(mMatGroup[i].BBMin[0],mMatGroup[i].BBMin[1],mMatGroup[i].BBMin[2],tc);
		LineSet(mMatGroup[i].BBMax[0],mMatGroup[i].BBMin[1],mMatGroup[i].BBMin[2],tc);
		LineSet(mMatGroup[i].BBMin[0],mMatGroup[i].BBMin[1],mMatGroup[i].BBMin[2],tc);
		LineSet(mMatGroup[i].BBMin[0],mMatGroup[i].BBMin[1],mMatGroup[i].BBMax[2],tc);
		LineSet(mMatGroup[i].BBMax[0],mMatGroup[i].BBMin[1],mMatGroup[i].BBMin[2],tc);
		LineSet(mMatGroup[i].BBMax[0],mMatGroup[i].BBMin[1],mMatGroup[i].BBMax[2],tc);
		LineSet(mMatGroup[i].BBMin[0],mMatGroup[i].BBMin[1],mMatGroup[i].BBMax[2],tc);
		LineSet(mMatGroup[i].BBMax[0],mMatGroup[i].BBMin[1],mMatGroup[i].BBMax[2],tc);

		LineSet(mMatGroup[i].BBMin[0],mMatGroup[i].BBMax[1],mMatGroup[i].BBMin[2],tc);
		LineSet(mMatGroup[i].BBMax[0],mMatGroup[i].BBMax[1],mMatGroup[i].BBMin[2],tc);
		LineSet(mMatGroup[i].BBMin[0],mMatGroup[i].BBMax[1],mMatGroup[i].BBMin[2],tc);
		LineSet(mMatGroup[i].BBMin[0],mMatGroup[i].BBMax[1],mMatGroup[i].BBMax[2],tc);
		LineSet(mMatGroup[i].BBMax[0],mMatGroup[i].BBMax[1],mMatGroup[i].BBMin[2],tc);
		LineSet(mMatGroup[i].BBMax[0],mMatGroup[i].BBMax[1],mMatGroup[i].BBMax[2],tc);
		LineSet(mMatGroup[i].BBMin[0],mMatGroup[i].BBMax[1],mMatGroup[i].BBMax[2],tc);
		LineSet(mMatGroup[i].BBMax[0],mMatGroup[i].BBMax[1],mMatGroup[i].BBMax[2],tc);

		LineSet(mMatGroup[i].BBMin[0],mMatGroup[i].BBMin[1],mMatGroup[i].BBMin[2],tc);
		LineSet(mMatGroup[i].BBMin[0],mMatGroup[i].BBMax[1],mMatGroup[i].BBMin[2],tc);
		LineSet(mMatGroup[i].BBMax[0],mMatGroup[i].BBMin[1],mMatGroup[i].BBMin[2],tc);
		LineSet(mMatGroup[i].BBMax[0],mMatGroup[i].BBMax[1],mMatGroup[i].BBMin[2],tc);
		LineSet(mMatGroup[i].BBMin[0],mMatGroup[i].BBMin[1],mMatGroup[i].BBMax[2],tc);
		LineSet(mMatGroup[i].BBMin[0],mMatGroup[i].BBMax[1],mMatGroup[i].BBMax[2],tc);
		LineSet(mMatGroup[i].BBMax[0],mMatGroup[i].BBMin[1],mMatGroup[i].BBMax[2],tc);
		LineSet(mMatGroup[i].BBMax[0],mMatGroup[i].BBMax[1],mMatGroup[i].BBMax[2],tc);

		if( cnt > 1000)
		{
			DrawPrimitiveUPLine(cnt/2,vertex);
			cnt=0;
		}
	}
	if( cnt )
		DrawPrimitiveUPLine(cnt/2,vertex);
//    GetD3dDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );
}

#define PlaySoundFlag(num)			(((BYTE*)mSoundEntityCache)[num>>3]|=(1<<(num&7)))
#define PlaySoundUnFlag(num)		(((BYTE*)mSoundEntityCache)[num>>3]&=(~(1<<(num&7))) )	//끈다.
#define IsPlayedSoundFlag(num)		(((BYTE*)mSoundEntityCache)[num>>3]&(1<<(num&7)))

#define _SOUND_FLAG_PAN_OFF	0x10
#define _SOUND_FLAG_SPHERE	0x1
#define _SOUND_FLAG_BOX		0x2
#define _SOUND_FLAG_LOOP	0x4
#define _SOUND_FLAG_EVENT	0x8

void CBsp::HearMapSound()	//맵 사운드를 듣자.(렌더링이 아니므로 endscene()이후에 넣자..)
{
	DWORD j,id;
	Vector3f pos;
	float vol,pan;

	if( mSoundEntityListNum==0 || mSoundEntitiesListNum ==0 )	//사운드 없다...
		return;

	GetCameraPos(&pos);
	for( j=0; j<mLeafSoundEntityList[mNowLeafNum].entities_num; j++)
	{
		id = mSoundEntityID[mLeafSoundEntityList[mNowLeafNum].start_id+j];

		if( pos[0] > mSoundEntitiesList[id].BBMin[0] && pos[0] < mSoundEntitiesList[id].BBMax[0]
		&& pos[1] > mSoundEntitiesList[id].BBMin[1] && pos[1] < mSoundEntitiesList[id].BBMax[1]
		&& pos[2] > mSoundEntitiesList[id].BBMin[2] && pos[2] < mSoundEntitiesList[id].BBMax[2] )
		{	//듣는 영역에 들어왔다..

			if( mSoundEntitiesList[id].Flag & _SOUND_FLAG_SPHERE ) //구형태닷
			{
				float vol = mSoundEntitiesList[id].GetVolume(mSoundEntitiesList[id].Pos);
				float pan;
				if( mSoundEntitiesList[id].Flag & _SOUND_FLAG_PAN_OFF )
					pan = 0;
				else
					pan = mSoundEntitiesList[id].GetPan(mSoundEntitiesList[id].Pos);

				if( mSoundEntitiesList[id].Flag & _SOUND_FLAG_LOOP ) //루프되는.
				{
					if( !IsPlayedSoundFlag(id) ) //플레이중이 아니라면..
					{
						PlaySoundFlag(id);
						IM_SetLoopCntWave(mSoundEntityList[mSoundEntitiesList[id].ID].ID,0);
						IM_PlayWave(mSoundEntityList[mSoundEntitiesList[id].ID].ID,vol,pan);
					}
					else	//플레이중이면 pan과 볼륨을 셋팅해준다.
					{
						IM_SetWaveVolumeAndPan(mSoundEntityList[mSoundEntitiesList[id].ID].ID,vol,pan);
		//				IM_SetWavePan(mSoundEntityList[mSoundEntitiesList[id].ID].ID,pan);
					}
				}
				else
				{
					mSoundEntitiesList[id].NextPlayTime += R3GetLoopTime();
					if( mSoundEntitiesList[id].NextPlayTime > mSoundEntitiesList[id].EventTime )
					{
						mSoundEntitiesList[id].NextPlayTime = (float)(rand()%(mSoundEntitiesList[id].EventTime+1));	//다음 시간을 잡는다.
						mSoundEntitiesList[id].NextPlayTime = mSoundEntitiesList[id].EventTime - mSoundEntitiesList[id].EventTime/4.0f+mSoundEntitiesList[id].NextPlayTime/2.0f;
						mSoundEntitiesList[id].NextPlayTime = mSoundEntitiesList[id].EventTime - mSoundEntitiesList[id].NextPlayTime;
						vol = mSoundEntitiesList[id].GetVolume(mSoundEntitiesList[id].Pos);
						pan = mSoundEntitiesList[id].GetPan(mSoundEntitiesList[id].Pos);
						IM_SetLoopCntWave(mSoundEntityList[mSoundEntitiesList[id].ID].ID,1);
						IM_PlayWave(mSoundEntityList[mSoundEntitiesList[id].ID].ID,vol,pan);
					}
				}
			}
			else		//요기는 박스형태.
			{
				float intensity = mSoundEntitiesList[id].GetBoxIntensity(pos);

				if( intensity > 0 )
				{
					if( mSoundEntitiesList[id].Flag & _SOUND_FLAG_PAN_OFF )
						pan = 0;
					else
						pan = mSoundEntitiesList[id].GetPan(mSoundEntitiesList[id].Pos);
					if( !(mSoundEntitiesList[id].Flag & _SOUND_FLAG_LOOP) ) //루프되는 것이 아니라면.
					{
						mSoundEntitiesList[id].NextPlayTime+=R3GetLoopTime();
						if( mSoundEntitiesList[id].NextPlayTime > mSoundEntitiesList[id].EventTime )
						{
							mSoundEntitiesList[id].NextPlayTime = (float)(rand()%(mSoundEntitiesList[id].EventTime+1));	//다음 시간을 잡는다.
							mSoundEntitiesList[id].NextPlayTime = mSoundEntitiesList[id].EventTime - mSoundEntitiesList[id].EventTime/4.0f+mSoundEntitiesList[id].NextPlayTime/2.0f;
							mSoundEntitiesList[id].NextPlayTime = mSoundEntitiesList[id].EventTime - mSoundEntitiesList[id].NextPlayTime;
							IM_SetLoopCntWave(mSoundEntityList[mSoundEntitiesList[id].ID].ID,1);
							IM_PlayWave(mSoundEntityList[mSoundEntitiesList[id].ID].ID,intensity,pan);
						}
					}
					else
					{
						vol = intensity;//mSoundEntitiesList[id].GetVolume(mSoundEntitiesList[id].Pos);
						if( !IsPlayedSoundFlag(id) ) //플레이중이 아니라면..
						{
							PlaySoundFlag(id);
							IM_SetLoopCntWave(mSoundEntityList[mSoundEntitiesList[id].ID].ID,0);
							IM_PlayWave(mSoundEntityList[mSoundEntitiesList[id].ID].ID,vol,pan);
						}
						else	//플레이중이면 pan과 볼륨을 셋팅해준다.
						{
							IM_SetWaveVolumeAndPan(mSoundEntityList[mSoundEntitiesList[id].ID].ID,vol,pan);
						}
					}
				}
			}
		}
	}
	DWORD i,bit;


	// 플레이중인데 벗어난경우... 
	for(i=0 ;i< mSoundEntityCacheSize; i++)
	{
		if( !mSoundEntityCache[i] )
			continue;
		for(j=0,bit=0x1; j<8; j++,bit<<=1)
		{
			if( mSoundEntityCache[i]&bit )
			{
				int entities_id = (i<<3)+j;

				if( pos[0] > mSoundEntitiesList[entities_id].BBMin[0] 
				&& pos[0] < mSoundEntitiesList[entities_id].BBMax[0]
				&& pos[1] > mSoundEntitiesList[entities_id].BBMin[1] 
				&& pos[1] < mSoundEntitiesList[entities_id].BBMax[1]
				&& pos[2] > mSoundEntitiesList[entities_id].BBMin[2] 
				&& pos[2] < mSoundEntitiesList[entities_id].BBMax[2] )
				{
				}
				else
				{
					// 플레이중인데 벗어나 있다... 그래서 끈다...
					IM_StopWave(mSoundEntityList[mSoundEntitiesList[entities_id].ID].ID);
					PlaySoundUnFlag(entities_id);
				}
			}
		}
	}
}

void CBsp::CalcObjectLoop()		//오브젝트단위 루프로 갱신할 루틴이 있는 곳.
{
}

//--------------------------땜빵

void DrawEtcPrint()
{
/*
	char buf[256];
    static FLOAT fFPS      = 0.0f;
    static FLOAT fLastTime = 0.0f;
    static DWORD dwFrames  = 0L;

    // Keep track of the time lapse and frame count
    FLOAT fTime = timeGetTime() * 0.001f; // Get current time in seconds
    ++dwFrames;

    // Update the frame rate once per second
    if( fTime - fLastTime > 1.0f )
    {
        fFPS      = dwFrames / (fTime - fLastTime);
        fLastTime = fTime;
        dwFrames  = 0L;
    }
    sprintf( buf, "%7.02f fps", fFPS);
    OutputText( 0, 0, buf );

	sprintf(buf,"now leaf num = %5d",mNowLeafNum);
	OutputText( 0, 15, buf );
	sprintf(buf,"Viewdist %4dm, now pos = %5.2f,%5.2f,%5.2f",(int)_VIEW_DIST,mTempCamera[0],mTempCamera[1],mTempCamera[2]);
	OutputText( 0, 30, buf );
*/
}

/*
void CBsp::LoadMemoryFromBspFile(char *name,_BSP_FILE_HEADER *m_header)			//bsp파일을 그대로 메모리블럭으로 로딩한다....
{
	FILE *fp;
	char filename[256];

	_R3ENGINE_STATE *state = GetR3State();
	strcpy(filename,state->MapPath);
	strcat(filename,name);
	strcat(filename,"\\");
	strcat(filename,name);
	strcat(filename,".bsp");

	if( (fp=fopen(filename,"rb")) == NULL)
	{
		Warning(filename,"<- 파일이 없슴다.");
		return;
	}
	fread(m_header,sizeof(_BSP_FILE_HEADER),1,fp);

	if(m_header->version != _R3BSP_VERSION)
	{
		Warning("BSP파일 버젼이 맞지않슴다.","장언일");
		fclose(fp);
		return;
	}

	int size=0;
	size+=m_header->CPlanes.size;
	size+=m_header->CFaceId.size;
	size+=m_header->Node.size;
	size+=m_header->Leaf.size;
	size+=m_header->MatListInLeaf.size;
	size+=m_header->Object.size;
	size+=m_header->Track.size;
	size+=m_header->EventObjectID.size;

	size+=m_header->CFVertex.size;
	size+=m_header->CFLine.size;
	size+=m_header->CFLineId.size;
	size+=m_header->CFLeaf.size;
	size+=m_header->EntityList.size;
	size+=m_header->EntityID.size;
	size+=m_header->LeafEntityList.size;
	size+=m_header->SoundEntityID.size;
	size+=m_header->LeafSoundEntityList.size;

	size+=m_header->BVertex.size;
	size+=m_header->WVertex.size;
	size+=m_header->FVertex.size;
	size+=m_header->VertexColor.size;
	size+=m_header->UV.size;
	size+=m_header->LgtUV.size;
	size+=m_header->Face.size;
	size+=m_header->FaceId.size;
	size+=m_header->VertexId.size;
	size+=m_header->ReadMatGroup.size;

	size+=m_header->MapEntitiesList.size;
	size+=m_header->SoundEntityList.size;
	size+=m_header->SoundEntitiesList.size;

	mBspMemory=(BYTE *)Dmalloc(size);
	fread(mBspMemory,size,1,fp);
	fclose(fp);
}
void CBsp::ReleaseMemoryBsp()			//bsp메보리 블럭 해제 init을 실행하면 반드시 릴리즈를...
{
	if( mBspMemory )
	{
		Dfree(mBspMemory);
		mBspMemory=0;
	}
}

*/
void CBsp::SaveExtBsp(char *name,_ADD_BSP_SAVE *add_bsp)	//ext bsp를 저장.
{
	FILE *fp=fopen(name,"wb");
	if( fp == NULL )
		return;
	//옵셋 정리.
	DWORD i;
	DWORD size=sizeof(_EXT_BSP_FILE_HEADER);
	mExtBSPHeader.CFVertex.offset=size;
	size += mExtBSPHeader.CFVertex.size;

	mExtBSPHeader.CFLine.offset=size;
	size += mExtBSPHeader.CFLine.size;
	mExtBSPHeader.CFLineId.offset=size;
	size += mExtBSPHeader.CFLineId.size;
	mExtBSPHeader.CFLeaf.offset=size;
	size += mExtBSPHeader.CFLeaf.size;
	mExtBSPHeader.EntityList.offset=size;
	size += mExtBSPHeader.EntityList.size;
	mExtBSPHeader.EntityID.offset=size;
	size += mExtBSPHeader.EntityID.size;
	mExtBSPHeader.LeafEntityList.offset=size;
	size += mExtBSPHeader.LeafEntityList.size;
	mExtBSPHeader.SoundEntityID.offset=size;
	size += mExtBSPHeader.SoundEntityID.size;
	mExtBSPHeader.LeafSoundEntityList.offset=size;
	size += mExtBSPHeader.LeafSoundEntityList.size;
	mExtBSPHeader.MapEntitiesList.offset=size;
	size += mExtBSPHeader.MapEntitiesList.size;
	mExtBSPHeader.SoundEntityList.offset=size;
	size += mExtBSPHeader.SoundEntityList.size;
	mExtBSPHeader.SoundEntitiesList.offset=size;
	size += mExtBSPHeader.SoundEntitiesList.size;

	//---스페어 엔트리.
	for(i=0; i<_READ_EXT_BSP_SPARE_CNT; i++ )
	{
		mExtBSPHeader.ReadSpare[i].offset=0;
		mExtBSPHeader.ReadSpare[i].size=0;
	}
	for(i=0; i<_FREE_EXT_BSP_SPARE_CNT; i++ )
	{
		mExtBSPHeader.FreeSpare[i].offset=0;
		mExtBSPHeader.FreeSpare[i].size=0;
	}
	//--------------bsp 세이브

	mExtBSPHeader.version = _R3_EXT_BSP_VERSION;
	fwrite(&mExtBSPHeader,sizeof(_EXT_BSP_FILE_HEADER),1,fp);

	//충돌 부분.
	fwrite(add_bsp->ColPoint,mExtBSPHeader.CFVertex.size,1,fp);
	fwrite(add_bsp->ColLine,mExtBSPHeader.CFLine.size,1,fp);
	fwrite(add_bsp->ColLineID,mExtBSPHeader.CFLineId.size,1,fp);
	fwrite(add_bsp->ColLeaf,mExtBSPHeader.CFLeaf.size,1,fp);

	//엔티티 부분.
	fwrite(add_bsp->EntityList,mExtBSPHeader.EntityList.size,1,fp);
	fwrite(add_bsp->EntityID,mExtBSPHeader.EntityID.size,1,fp);
	fwrite(add_bsp->LeafEntityList,mExtBSPHeader.LeafEntityList.size,1,fp);

	//사운드 엔티티부분.
	fwrite(add_bsp->SoundEntityID,mExtBSPHeader.SoundEntityID.size,1,fp);
	fwrite(add_bsp->LeafSoundEntityList,mExtBSPHeader.LeafSoundEntityList.size,1,fp);

	//---------------------------------------
	// 엔티티 추가 부분.
	fwrite(add_bsp->MapEntitiesList,mExtBSPHeader.MapEntitiesList.size,1,fp);

	//사운드 엔티티부분.
	fwrite(add_bsp->SoundEntityList,mExtBSPHeader.SoundEntityList.size,1,fp);
	fwrite(add_bsp->SoundEntitiesList,mExtBSPHeader.SoundEntitiesList.size,1,fp);

	fclose(fp);
}

