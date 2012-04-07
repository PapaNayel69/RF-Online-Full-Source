#include <stdio.h>
#include "Skybox.h"
#include "R3Render.h"
#include "CommonUtil.h"
#include "Jerror.h"
#include "Jmalloc.h"
#include "R3d3d8.h"
#include "core.h"

int GetMaterialNameNum(_R3MATERIAL *mat)
{
	char buf[256]={'0','0','0','0','0',NULL,'0','0','0'};

	int cnt=0;
	for( int i=strlen(mat->m_name)-1; i>=0; i-- )
	{
		if( mat->m_name[i] == '_')
		{
			break;
		}
		else
		{
			buf[4-cnt] = mat->m_name[i];
			cnt++;
		}
	}
	return atoi(buf);
}
CSkyBox::CSkyBox()
{
	mIsSkyLoad=FALSE;
}
CSkyBox::~CSkyBox()
{
}

int CSkyBox::GetUsedVertexBufferSize()
{
	return mEntity.GetUsedVertexBufferSize();
}

void CSkyBox::LoadSkyBox(char *filename)
{

	if( mEntity.LoadEntity(filename) == FALSE)
		return;
	mIsSkyLoad=TRUE;
	mMatNum = mEntity.GetMatNum();
	mMatGroupNum = mEntity.GetMatGroupNum();
	mMatGroup = mEntity.GetMatGroup();
//--- 미리 소트테이블을 만든다.
	int temp_mat_sort[512];
	DWORD i,j;

	for(i=0; (int)i<mMatNum; i++)
	{
		DWORD m_num=GetMaterialNameNum(&mEntity.mMat[i]);
		temp_mat_sort[i]=(SHORT)m_num;
	}
	int cnt=0;
	for(j=0; j<(DWORD)mMatGroupNum; j++)
	memset(mMatGroupSort,0,mMatGroupNum*sizeof(short));	//혹시 몰라서 0으로 셋팅..
	for(i=0; i<(DWORD)mMatNum; i++)
	{
//		mMatGroupSort[i]=0;
		for(j=0; j<(DWORD)mMatGroupNum; j++)
		{
			if( temp_mat_sort[i] == mMatGroup[j].MtlId )
			{
				mMatGroupSort[cnt++]=(short)j;
			}
		}
	}
/*
	for(i=0; (int)i<mMatGroupNum; i++)
	{
		DWORD m_num=GetMaterialNameNum(&GetR3Material()[mMatGroup[i].MtlId]);
		mMatSort[i]=(SHORT)m_num;
	}
*/
/*
	char pmat_name[256];
	strcpy(pmat_name,filename);
	StripPath(pmat_name);
	StripEXT(pmat_name);
//	stSkyMaterial=AddMaterial(pmat_name);

	//----머터리얼쪽 포인터를 나눠주자..
	int mat_f_num[_MAX_MATERIALS];

	for(i=0; i< stSkyMaterialNum; i++)
		mat_f_num[i] = 0;

	//각각의 갯수 파악
	for(i=0; i< stSky_face_num; i++)
	{
		if( stFace[i].mtl_id < 0 )
			continue;
		mat_f_num[stFace[i].mtl_id]++;
	}
	int total_face=0;

	total_face=0;
	//한머터리얼이 가지는 최대 그룹을 셈한다.
	for(i=0; i<stSkyMaterialNum; i++)	
	{
		total_face += mat_f_num[i];
	}

	DWORD temp = (total_face+stSkyMaterialNum)*sizeof(DWORD) 
				+ stSkyMaterialNum*sizeof(DWORD*);

	stFaceMemoryBlock = (BYTE *)Dmalloc(temp);
	stMatSkyBoxCnt = (DWORD *)stFaceMemoryBlock;
	stMatSkyBoxTable = (DWORD **)(stFaceMemoryBlock+stSkyMaterialNum*sizeof(DWORD));
	DWORD nu_juc = (stSkyMaterialNum*(sizeof(DWORD*)+sizeof(DWORD)));
	for(i=0; i<stSkyMaterialNum; i++)
	{
		stMatSkyBoxTable[i] = (DWORD *)(stFaceMemoryBlock+nu_juc);
		nu_juc += mat_f_num[i]*sizeof(DWORD);
	}
	//----스카이박스의 머터리얼 순서 정렬...
	DWORD *mat_index = (DWORD *)Dmalloc(sizeof(DWORD)*stSky_face_num);
	for(j=0; j<stSky_face_num; j++)
		mat_index[j] = stFace[j].mtl_id;

	for(i=0; i<stSkyMaterialNum; i++)
	{
		DWORD m_num=GetMaterialNameNum(&stSkyMaterial[i]);
		if( m_num != i)
		{
			for(j=0; j<stSky_face_num; j++)
			{
				if( mat_index[j] == m_num)
					stFace[j].mtl_id=i;
//				if( mat_index[j] == i)
//					stFace[j].mtl_id=m_num;
			}
		}
	}
	Dfree(mat_index);
*/
}



void CSkyBox::ReleaseSkyBox()
{
	if( mIsSkyLoad == FALSE )
		return;
	mIsSkyLoad=FALSE;
	mEntity.ReleaseEntity();
}
/*
static void LoopInitSkyBoxStack()
{
	memset(stMatSkyBoxCnt,0,stSkyMaterialNum*sizeof(DWORD));
}

static void SetSkyBoxStack(int matnum,int f_num)	//해당 머터리얼에 해당 페이스를 추가...
{
	stMatSkyBoxTable[matnum][stMatSkyBoxCnt[matnum]] = f_num;
	stMatSkyBoxCnt[matnum]++;
}

DWORD GetSkyMaterialNum()
{
	return stSkyMaterialNum;
}
_R3MATERIAL *GetSkyMaterial()
{
	return stSkyMaterial;
}
static void BuildSkyStack()
{
	DWORD i;
	
	for(i=0; i<stSky_face_num; i++)
	{
		SetSkyBoxStack(stFace[i].mtl_id,i);
	}
}
*/
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

void CSkyBox::DrawSkyBox(void *view_matrix)	//
{
	mEntity.PrepareAnimation();	//애니메이션 준비.

	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();
	_R3ENGINE_STATE *r3_state=GetR3State();
	if( r3_state->mIsFog )
	{
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  r3_state->mFogColor );

		pd3dDevice->SetRenderState( D3DRS_FOGSTART,   FtoDW(r3_state->mFogStart));
		pd3dDevice->SetRenderState( D3DRS_FOGEND,     FtoDW(r3_state->mFogEnd) );
		pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
		pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );
		pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, r3_state->mFogRangeEnable );
	}
	else
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	if( r3_state->mIsNoFogSky )
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	if( mIsSkyLoad==FALSE )
		return;
	if(mEntity.GetStaticIndexedBuffer()==NULL)
		return;
	D3DMATRIX *view_mat = (D3DMATRIX *)view_matrix;
	D3DMATRIX mat_stack;
	memcpy(&mat_stack,view_mat,sizeof(D3DMATRIX));
	view_mat->_41=0;	//이동값이 제로인것이 바로 스카이박스다.
	view_mat->_42=0;
	view_mat->_43=0;
    pd3dDevice->SetTransform( D3DTS_VIEW, view_mat );

    // Some cards do not disable writing to Z when 
    // D3DRS_ZENABLE is FALSE. So do it explicitly
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	SetVPIPTex1(mEntity.GetStaticVertexBuffer()->m_lpVertexBuffer
		,mEntity.GetStaticIndexedBuffer()->m_lpIndexBuffer);
	MultiTexOff();
	//스카이박스 뿌리자.. 스카이는 multiply 2까지 지원.
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );

	float w_matrix[4][4];
	_ANI_OBJECT *object = mEntity.GetObject();
	_ENTITY_M_GROUP *m_group = mEntity.GetMatGroup();	

	int i;
	for(i=0; i<mMatGroupNum; i++)
	{
		GetObjectMatrix(w_matrix,m_group[mMatGroupSort[i]].ObjectId,object,R3GetTime()*30.0f);	//애니메이션 매트릭스가 있으면 가져온다.
		GetD3dDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)w_matrix );

//		mEntityPrepareAnimation();	//애니메이션 준비.
		mEntity.DrawOneMatGroup(&mMatGroup[mMatGroupSort[i]]);
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    // Some cards do not disable writing to Z when 
    // D3DRS_ZENABLE is FALSE. So do it explicitly
    pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    pd3dDevice->SetTransform( D3DTS_VIEW, &mat_stack );
}
