#include "level.h"
#include "R3bsp.h"
#include "R3d3dtex.h"
#include "core.h"
#include "Skybox.h"
#include "R3input.h"

#include "Jmalloc.h"
#include "JError.h"
#include "Collision.h"

#include "CommonUtil.h"
#include "R3LightMap.h"

#include "R3move.h"
#include "AniCamera.h"
#include "R3render.h"
#include "R3util.h"
#include "R3Sound.h"
#include "MultiLayer.h"
#include "R3engineKernel.h"
#include "RTMovie.h"
#include <stdio.h>

//#define Y_ABLE_RATE			5.67// 80도 경사 3.27f=73도	  ; 2.74f=70도
#define Y_ABLE_RATE			1.73f	// 80도 경사 3.27f=73도	  ; 2.74f=70도
#define _ABLE_HEIGHT  23

CLevel::CLevel()
{
	mBsp = new CBsp;
	mSkyBox = new CSkyBox;
	mIsLoadedBsp=FALSE;
	mMapName[0]=NULL;

	//----레벨 관련 텍스쳐 메모리사이즈...
	mLightTexMemSize=0;
	mMapTexMemSize=0;
	mSkyTexMemSize=0;
	mEntityTexMemSize=0;
}

CLevel::~CLevel()
{
	delete mSkyBox;
	delete mBsp;
}

void CLevel::ReleaseLevel()
{
	if(mBsp)
		mBsp->ReleaseBsp();
	mIsLoadedBsp=FALSE;
	if( IsServerMode()== FALSE )	//서버모드가 아닐때..
	{
//		ReleaseConsole();
	    R3ReleaseAllTextures();	//맵,스프라이트,시스템 텍스쳐 싹 내린다.

		ReleaseMainMaterial();
		ReleaseLightMap();
		R3ReleasePreTextures();
		if(mSkyBox)
			mSkyBox->ReleaseSkyBox();
//		ReleaseAniCamera();
		_R3ENGINE_STATE *state = GetR3State();
		state->mLensFlareTex = -1;

		mLightTexMemSize=0;
		mMapTexMemSize=0;
		mSkyTexMemSize=0;
		mEntityTexMemSize=0;
		
		RTMovieRelease();	//리얼타임무비 릴리즈..
		mDummy.ReleaseExtDummy();
	}
	else
		ReleaseMainMaterial();
}
//#include "afterscript.h"

void CLevel::LoadLevel(char *name)
{
	char buf[256];
	char f_buf[256];
	char real_bsp_name[256];
	char tempname[256];

	_R3ENGINE_STATE *state = GetR3State();

	if( name==NULL )
	{
		if( mMapName[0]==NULL )	//초기화...
			return;
		//strcpy(f_buf,mMapName);
		sprintf(f_buf,"%s\\%s\\%s.bsp",state->MapPath,mMapName,mMapName);
		name = f_buf;
	}

	FILE *fp;
	fp = fopen(name,"rb");
	if(fp == NULL )
	{
		mBsp->SetIsLoaded(FALSE);
		if( name[0] == NULL )
			Error("존재하지않은 맵을 로딩하려 하고있습니다.","");
		else
			Error(name,"<-이 맵파일이 없습니다.");
		return;
	}
	fclose(fp);

	strcpy(buf,name);
	StripEXT(buf);
	StripPath(buf);
	strcpy(real_bsp_name,state->MapPath);
	strcat(real_bsp_name,buf);
	strcat(real_bsp_name,"\\");
	strcat(real_bsp_name,buf);
	strcat(real_bsp_name,".bsp");

	ResetTexMemSize();
	if( IsServerMode()== FALSE )	//머터리얼을 먼저알아야 버텍스버퍼를 생성할수있다.서버모드가 아닐때..
	{
//		InitConsole();

		char load_name[256];

		strcpy(load_name,real_bsp_name);
		StripEXT(load_name);
		strcat(load_name,".r3m");

		strcpy(buf,name);
		StripEXT(buf);
		StripPath(buf);
		strcpy(mMapName,buf);
		if( IsExistFile(load_name) )	//r3m파일이 존재하면..
		{
			LoadMainR3M(load_name);

			strcpy(load_name,real_bsp_name);
			StripEXT(load_name);
			strcat(load_name,".r3t");
			R3Texture *t_tex = R3GetTexInfoR3T(load_name,0);	//R3T텍스쳐 정보를 리턴한다.
			SetNoLodTextere();	//lod텍스쳐 안먹어야되는거..
			LoadR3T(t_tex);
		}
		else
		{
			strcpy(load_name,state->MapPath);
			strcat(load_name,"\\");
			strcat(load_name,buf);
			strcat(load_name,"\\Tex\\");
			LoadMainMaterial(load_name);
			SetNoLodTextere();	//lod텍스쳐 안먹어야되는거..
		}
		mMapTexMemSize += GetNowTexMemSize();
	}
	else	//서버모드일때 
	{
		char load_name[256];

		strcpy(load_name,real_bsp_name);
		StripEXT(load_name);
		strcat(load_name,".r3m");

		if( IsExistFile(load_name) )	//r3m파일이 존재하면..
			LoadMainR3M(load_name);
	}

	ResetTexMemSize();

	mBsp->LoadBsp(real_bsp_name);
	
	//ext bsp 로딩.
	strcpy(tempname,real_bsp_name);	
	StripEXT(tempname);
	strcat(tempname,".ebp");
	mBsp->LoadExtBsp(tempname);
	mEntityTexMemSize+= GetNowTexMemSize();

	if( IsServerMode()== FALSE )	//서버모드가 아닐때..
	{
		strcpy(tempname,real_bsp_name);
		StripEXT(tempname);
		strcat(tempname,"Lgt.R3T");

		ResetTexMemSize();
		LoadLightMap(tempname);
		mLightTexMemSize+= GetNowTexMemSize();
		mBsp->CalcEntitiesMainColor();	//엔티티들 바닥컬러계산.
		
		strcpy(tempname,state->MapPath);
		strcat(tempname,buf);
		strcat(tempname,"\\sky\\");
		strcat(tempname,mMapName);
		strcat(tempname,"sky.r3e");

		ResetTexMemSize();
		if(IsExistFile(tempname))
			mSkyBox->LoadSkyBox(tempname);
		mSkyTexMemSize+= GetNowTexMemSize();

		ResetTexMemSize();
	    R3RestoreAllTextures();	//맵,스프라이트,시스템 텍스쳐 싹 올린다.
		mMapTexMemSize += GetNowTexMemSize();

		mIsLoadedBsp=TRUE;
		strcpy(tempname,real_bsp_name);
		StripEXT(tempname);
		strcat(tempname,".rvp");
		RTMovieCreate(tempname,this);	//리얼타임 렌더링의 파일을 로딩한다.
		RTMovieSetState(RTM_STATE_NO_LOGO|RTM_STATE_NO_WIDE|RTM_STATE_NO_CAMERAMOVE);	//디폴트 셋팅.

		strcpy(tempname,real_bsp_name);
		StripEXT(tempname);
		strcat(tempname,"EXT.spt");
		//더미 정보를 로딩한다.
		mDummy.LoadExtDummy(tempname);
	}
	mIsLoadedBsp=TRUE;
	ClearDynamicLight();
	//키보드버퍼 클리어링.
	memset(gAsci,0,256);
	mBsp->SetIsLoaded(TRUE);
}


void CLevel::ReLoadAllMaterial()
{
	char material_name[256];

	SetReLoadState(TRUE);
	strcpy(material_name,mMapName);
	ReLoadMaterial(material_name,GetMainMaterial());

//	strcat(material_name,"sky");
//	ReLoadMaterial(material_name,GetSkyMaterial());
	SetReLoadState(FALSE);
}

void CLevel::SetViewMatrix(D3DXMATRIX *view_mat)
{
	Vector3f pos={0,0,0};
	memcpy(&mMatView,view_mat,sizeof(D3DXMATRIX));	
	GetR3State()->mMatView = mMatView;
	R3GetQuakeVector(pos);
	mMatView.m[3][0] +=pos[0];
	mMatView.m[3][1] +=pos[1];
	mMatView.m[3][2] +=pos[2];
	GetR3State()->mMatView.m[3][0] +=pos[0];
	GetR3State()->mMatView.m[3][1] +=pos[1];
	GetR3State()->mMatView.m[3][2] +=pos[2];
}
void CLevel::FrameMove()	//맵 렌더링하기전에 호출하는것..
{
//	R3QuakeMatrix(mMatView.m);
	//사전준비.

	RTMovieFrameMove();	//리얼타임렌더링 프레임무브 

    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
	
	GetD3dDevice()->SetTransform( D3DTS_VIEW, &mMatView );
	SetDefaultFlags();
	SetFrustumNormalPlane();	//core에 view frustum 평면을 
	SetMultiLayerTime(R3GetTime());


//	ClearShadows();	//쉐도우 클리어 ..
	ClearDynamicLight();	//다이나믹 라이트 클리어 해야쥐...

	//-----BSP의 사전준비.
	ResetVertexBufferCache();	//루프에서 시작 할때 반드시 호출하자.. 

	mBsp->ReadyBspRender(mCamPos);	//Bsp렌더링준비

	mBsp->CalcObjectLoop();		//오브젝트단위 루프로 갱신할 루틴이 있는 곳.

	UpdateStreamMP3();		//mp3를 위한거...

	CalcR3Fog();	//포그 미리 계산.
}
void CLevel::DrawSkyBoxRender()
{
	mSkyBox->DrawSkyBox((void *)&mMatView);
	DrawFadeSKy();	//하늘 페이드 인부분..
}
void CLevel::DrawMapEntitiesRender()	//맵 엔티티들 렌더링.
{
	mBsp->DrawMapEntitiesRender();		//맵안의 엔티티들을 렌더링한다.
}

void CLevel::PrepareShadowRender(Vector3f ch_pos,void *tex,float intensity,DWORD blur,float scale,float add_pos)	// 
{
	mBsp->PrepareShadowRender(ch_pos,tex,intensity,blur,scale,add_pos);
}
void CLevel::DrawShadowRender(Vector3f ch_pos)
{
	mBsp->DrawShadowRender(ch_pos);
}

void CLevel::SetCameraPos(Vector3f camera)
{
	mCamPos[0] = camera[0];
	mCamPos[1] = camera[1];
	mCamPos[2] = camera[2];
}

void CLevel::DrawMapRender()
{
	D3DXMATRIX final;
	final=mMatView;	//월드는 제로다..

//	mBsp->ReadyBspRender(mCamPos);	//Bsp렌더링준비
	mBsp->DrawBspRender();			//배경에 해당되는 부분렌더링.
	mBsp->DrawDynamicLights();
//	DrawFogTest();
//	extern void DrawEtcPrint();
//	DrawEtcPrint();	//리프와 자신의 위치 프래임 레이트를 화면에 출력
	RTMovieRender();	//리얼타임 렌더링.
}
void CLevel::DrawMapAlphaRender(Vector3f pos)
{	
	if( !IsLoadedBsp())
		return;

	ResetVertexBufferCache();	//렌더링 구조에서 시작 할때 반드시 호출하자.. 
	_R3ENGINE_STATE *state = GetR3State();
	pos[0] = state->mInvMatView.m[3][0];
	pos[1] = state->mInvMatView.m[3][1];
	pos[2] = state->mInvMatView.m[3][2];
	mBsp->DrawAlphaRender(pos);	//모든 알파폴리곤들을 렌더링. 카메라 위치가 있어야 된다.
}

void CLevel::ScreenShot()
{
	char name[256];

	_R3ENGINE_STATE *state = GetR3State();
	for(int i=0; i<9999; i++)
	{
/*
		sprintf(name,"%s%s%04d.bmp",state->ScreenShotsPath,mMapName,i);
		if( IsExistFile(name) == FALSE )
		{
			FramebufferToBMP(name);
			break;
		}
*/
		sprintf(name,"%s%s%04d.jpg",state->ScreenShotsPath,mMapName,i);
		if( IsExistFile(name) == FALSE )
		{
			FramebufferToJPG(name);
			break;
		}
	}
}
void CLevel::DrawTestBox(Vector3f box_size,Vector3f pos,DWORD color)		//사람테스트할 박스유틸...
{
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
	matWorld.m[3][0] = pos[0];
	matWorld.m[3][1] = pos[1];
	matWorld.m[3][2] = pos[2];
    GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );

	::DrawTestBox(box_size[0],box_size[1],box_size[2],color);

}
BOOL CLevel::GetPointFromScreenRay(LONG x,LONG y,Vector3f *get_point)
{
	Vector3f src;
	_R3ENGINE_STATE *state = GetR3State();
	x -= state->mViewSx;
	y -= state->mViewSy;
	return mBsp->GetPointFromScreenRay((float)x,(float)y,*get_point,src);
}
BOOL CLevel::GetPointFromScreenRayFar(LONG x,LONG y,Vector3f *get_point)	//가장 먼곳에서 얻어온다.
{
	Vector3f src;
	_R3ENGINE_STATE *state = GetR3State();
	x -= state->mViewSx;
	y -= state->mViewSy;
	return mBsp->GetPointFromScreenRayFar((float)x,(float)y,*get_point,src);
}
/*
BOOL CLevel::GetPointFromScreenRay(LONG x,LONG y,Vector3f *tar_point,Vector3f *src_point)
{
	return mBsp->GetPointFromScreenRay((float)x,(float)y,*tar_point,*src_point);
}
*/
float CLevel::GetFirstYpos(Vector3f pos,int option)
{
	if( !IsLoadedBsp() )	//맵 로딩이 안됬다.
		return FALSE;
	return mBsp->GetFirstYpos(pos,option);
}
float CLevel::GetFirstYpos(Vector3f pos,Vector3f min,Vector3f max)
{
	if( !IsLoadedBsp() )	//맵 로딩이 안됬다.
		return FALSE;
	return mBsp->GetFirstYpos(pos,min,max);
}
float CLevel::GetFirstYpos(Vector3f pos,Vector3s min,Vector3s max)	//바운딩 박스 영역안에서 구한다.구할수 없는경우 65535을 리턴
{
	if( !IsLoadedBsp() )	//맵 로딩이 안됬다.
		return FALSE;
	return mBsp->GetFirstYpos(pos,min,max);
}
void CLevel::DrawLeafBBox()
{
	mBsp->DrawLeafBBox();
}
void CLevel::DrawMatBBox()
{
	mBsp->DrawMatBBox();
}

BOOL CLevel::GetNextYpos(Vector3f tpos,float *ypos)
{
	BOOL is_can=TRUE;
	int leaf_num=0,i;
	short leaf_id[256];
	float temp_ypos=-100000;


	if( !IsLoadedBsp() )	//맵 로딩이 안됬다.
		return FALSE;

	tpos[1]-=_ABLE_HEIGHT;	//내려갈걸 감안해서...
	leaf_id[0] = mBsp->GetLeafNum(tpos);
	tpos[1]+=_ABLE_HEIGHT*2;	//절반 높이를 올라갈수있으면 리프가또 있는지 첵크해본다.
	leaf_id[1] = mBsp->GetLeafNum(tpos);
	tpos[1]-=_ABLE_HEIGHT;	//도루 제자리

	if( leaf_id[0] == leaf_id[1] )
		leaf_num=1;
	else
		leaf_num=2;

	Vector3f b_camera,n_camera;
	Vector3fCopy(tpos,b_camera);
	Vector3fCopy(tpos,n_camera);

	b_camera[1] += 30000;
	n_camera[1] -= 30000;

	is_can = FALSE;
	for( i=0; i<leaf_num; i++)
	{
		float y_height = mBsp->GetYposInLeaf(b_camera,n_camera,_ABLE_HEIGHT,tpos[1],leaf_id[i] );

		if( y_height != -32000 )
		{
			if( y_height > temp_ypos )
			{
				is_can = TRUE;
				temp_ypos = y_height;
			}
		}
	}
	if( is_can )
		*ypos = temp_ypos;
	return is_can;
}


BOOL CLevel::GetNextYposNoAttr(Vector3f tpos,float *ypos)
{
	BOOL is_can=TRUE;
	int leaf_num=0,i;
	short leaf_id[256];
	float temp_ypos=-100000;

	if( !IsLoadedBsp() )	//맵 로딩이 안됬다.
		return FALSE;

	tpos[1]-=_ABLE_HEIGHT;	//내려갈걸 감안해서...
	leaf_id[0] = mBsp->GetLeafNum(tpos);
	tpos[1]+=_ABLE_HEIGHT*2;	//절반 높이를 올라갈수있으면 리프가또 있는지 첵크해본다.
	leaf_id[1] = mBsp->GetLeafNum(tpos);
	tpos[1]-=_ABLE_HEIGHT;	//도루 제자리

	if( leaf_id[0] == leaf_id[1] )
		leaf_num=1;
	else
		leaf_num=2;

	Vector3f b_camera,n_camera;
	Vector3fCopy(tpos,b_camera);
	Vector3fCopy(tpos,n_camera);

	b_camera[1] += 9999;
	n_camera[1] -= 9999;

	is_can = FALSE;
	for( i=0; i<leaf_num; i++)
	{
		float y_height = mBsp->GetYposInLeafNoAttr(b_camera,n_camera,_ABLE_HEIGHT,tpos[1],leaf_id[i] );

		if( y_height != -32000 )
		{
			if( y_height > temp_ypos )
			{
				is_can = TRUE;
				temp_ypos = y_height;
			}
		}/*
		else
		{
			b_camera[0] += _ABLE_HEIGHT/2;
			n_camera[1] += _ABLE_HEIGHT/2;
			float y_height = mBsp->GetYposInLeafNoAttr( b_camera,n_camera,_ABLE_HEIGHT,tpos[1],leaf_id[i] );
			if( y_height != -32000 )
			{
				is_can = TRUE;
				temp_ypos = y_height;
			}
			else	//갈수가 없다.
			{
			}			
		}*/
	}
	if( is_can )
		*ypos = temp_ypos;
	return is_can;
}

BOOL CLevel::GetNextYposForServer(Vector3f tpos,float *ypos)	//다음위치의 y좌표를..	서버에서만 쓴다. 에지노멀을 가지고 있으므로 속도 좋다.
{
	BOOL is_can=TRUE;
	int leaf_num=0,i;
	short leaf_id[256];
	float temp_ypos=-100000;

	tpos[1]-=_ABLE_HEIGHT;	//내려갈걸 감안해서...
	leaf_id[0] = mBsp->GetLeafNum(tpos);
	tpos[1]+=_ABLE_HEIGHT*2;	//절반 높이를 올라갈수있으면 리프가또 있는지 첵크해본다.
	leaf_id[1] = mBsp->GetLeafNum(tpos);
	tpos[1]-=_ABLE_HEIGHT;	//도루 제자리

	if( leaf_id[0] == leaf_id[1] )
		leaf_num=1;
	else
		leaf_num=2;

	Vector3f b_camera,n_camera;
	Vector3fCopy(tpos,b_camera);
	Vector3fCopy(tpos,n_camera);

	b_camera[1] += 9999;
	n_camera[1] -= 9999;

	temp_ypos = -100000;
	is_can = FALSE;
	for( i=0; i<leaf_num; i++)
	{
		float y_height = mBsp->GetYposInLeafUseEdgeNormal(b_camera,n_camera,_ABLE_HEIGHT,tpos[1],leaf_id[i] );

		if( y_height != -32000 )
		{
			if( temp_ypos < y_height )
			{
				if( y_height > temp_ypos )
				{
					is_can = TRUE;
					temp_ypos = y_height;
				}
			}
		}/*
		else
		{
			b_camera[0] += _ABLE_HEIGHT/2;
			n_camera[1] += _ABLE_HEIGHT/2;
			float y_height = mBsp->GetYposInLeafUseEdgeNormal( b_camera,n_camera,_ABLE_HEIGHT,tpos[1],leaf_id[i] );
			if( y_height != -32000 )
			{
				is_can = TRUE;
				temp_ypos = y_height;
			}
			else	//갈수가 없다.
			{
			}			
		}*/
	}
	if( is_can )
		*ypos = temp_ypos;
	return is_can;
}
BOOL CLevel::GetNextYposFar(Vector3f src_pos,Vector3f tar_pos,float *ypos)
{
	BOOL is_can=TRUE;
	LONG leaf_num=0,i;
	SHORT leaf_id[32000];
	Vector3f temp_pos;
	float temp_ypos;

	if( !IsLoadedBsp() )	//맵 로딩이 안됬다.
		return FALSE;
	if( src_pos[0]== tar_pos[0] && src_pos[1]== tar_pos[1] && src_pos[2]== tar_pos[2])
	{
		*ypos=src_pos[1];
		return TRUE;
	}

#define _CHECK_MIN_DIST	25.0f
	Vector3f t_src_pos,t_tar_pos;

	Vector3fCopy(src_pos,t_src_pos);
	Vector3fCopy(tar_pos,t_tar_pos);


	while( 1 )
	{
		temp_ypos=-100000;
		float dist = GetDist(t_src_pos,t_tar_pos);
		if( dist > _CHECK_MIN_DIST )
		{
			Vector3fSub(t_tar_pos,t_src_pos,temp_pos);
			Normalize(temp_pos);
			Vector3fMul(temp_pos,_CHECK_MIN_DIST,temp_pos);
			Vector3fAdd(t_src_pos,temp_pos,t_tar_pos);
			dist = _CHECK_MIN_DIST;
		}
		float able_height=dist*Y_ABLE_RATE;	//Y_ABLE_RATE배로 측정하자.
		if( able_height < _ABLE_HEIGHT )
			able_height = _ABLE_HEIGHT;

		Vector3fCopy(t_tar_pos,temp_pos);

		//갈수 있는 위아래 높이를 계산해 줘야된다.
		temp_pos[1] = t_tar_pos[1]+able_height;
		t_tar_pos[1]-=able_height;
		mBsp->GetLeafList(temp_pos,t_tar_pos,&leaf_num,leaf_id,32000);
		t_tar_pos[1]+=able_height;

		Vector3f b_camera,n_camera;
		Vector3fCopy(t_tar_pos,b_camera);
		Vector3fCopy(t_tar_pos,n_camera);

		b_camera[1] += 9999;
		n_camera[1] -= 9999;

		is_can = FALSE;
		for( i=0; i<leaf_num; i++)
		{
			float y_height = mBsp->GetYposInLeaf(b_camera,n_camera,able_height,t_tar_pos[1],leaf_id[i] );

			if( y_height != -32000 )
			{
				if( temp_ypos < y_height )
				{
					is_can = TRUE;
					temp_ypos = y_height;
				}
			}
		}
		if( !is_can )
			return FALSE;
		if( t_tar_pos[0] == tar_pos[0] && t_tar_pos[2] == tar_pos[2] )	//첵크 다햇따.
			break;
		Vector3fCopy(t_tar_pos,t_src_pos);
		Vector3fCopy(tar_pos,t_tar_pos);
		t_src_pos[1]=temp_ypos;
		t_tar_pos[1]=temp_ypos;
	}
	*ypos = temp_ypos;
	return TRUE;
}

static DWORD CollisionLevel(float org,float best_add_height,float col_height)	//0에 가까울수록 아주 좋은 단계다.
{
	float best_height = org + best_add_height;
	if( org <= col_height )
	{
		return (DWORD)((col_height-org)/(best_height - org))*2;
	}
	else
	{
		return (DWORD)((org-col_height)/(best_height - org))*2+1;
	}
}


BOOL CLevel::GetNextYposFarProgress(Vector3f src_pos,Vector3f tar_pos,float *ypos)
{
	if( !IsLoadedBsp() )	//맵 로딩이 안됬다.
		return FALSE;
	if( src_pos[0]== tar_pos[0] && src_pos[1]== tar_pos[1] && src_pos[2]== tar_pos[2])
	{
		*ypos=src_pos[1];
		return TRUE;
	}
	tar_pos[1] = src_pos[1];

	BOOL is_can=TRUE;
	LONG leaf_num=0,i,j;
	SHORT leaf_id[10512];
	Vector3f temp_pos;
	float temp_ypos;

#define _CHECK_MIN_DIST	25.0f
	Vector3f t_src_pos,t_tar_pos;

	Vector3fCopy(src_pos,t_src_pos);
	Vector3fCopy(tar_pos,t_tar_pos);


	while( 1 )
	{
		temp_ypos=-100000;
		float dist = GetDist(t_src_pos,t_tar_pos);
		if( dist > _CHECK_MIN_DIST )
		{
			Vector3fSub(t_tar_pos,t_src_pos,temp_pos);
			Normalize(temp_pos);
			Vector3fMul(temp_pos,_CHECK_MIN_DIST,temp_pos);
			Vector3fAdd(t_src_pos,temp_pos,t_tar_pos);
			dist = _CHECK_MIN_DIST;
		}
		float able_height=dist*Y_ABLE_RATE;	//Y_ABLE_RATE배로 측정하자.
		if( able_height < _ABLE_HEIGHT )
			able_height = _ABLE_HEIGHT;

		Vector3fCopy(t_tar_pos,temp_pos);

		//갈수 있는 위아래 높이를 계산해 줘야된다.
		temp_pos[1] = t_tar_pos[1]+30000;
		t_tar_pos[1]-=30000;
		mBsp->GetLeafList(temp_pos,t_tar_pos,&leaf_num,leaf_id,10000);
		t_tar_pos[1]+=30000;

		Vector3f b_camera,n_camera;
		Vector3fCopy(t_tar_pos,b_camera);
		Vector3fCopy(t_tar_pos,n_camera);

		b_camera[1] += 30000;
		n_camera[1] -= 30000;

		is_can = FALSE;
		DWORD best_level=10000;
		for( i=0; i<leaf_num; i++)
		{
			for(j=0; j<2; j++ )		//오차를 위해 2번을 돌린다.
			{
				float y_height;
				y_height = mBsp->GetBestYposInLeaf(b_camera,n_camera,able_height,t_tar_pos[1],leaf_id[i] );
				
				if( y_height != -32000 )
				{
					DWORD h_level = CollisionLevel(t_tar_pos[1],able_height,y_height);
					if( best_level == h_level ) 
					{
						if( temp_ypos < y_height )
						{
							is_can = TRUE;
							temp_ypos = y_height;
						}
					}
					else
					if( best_level > h_level ) 
					{
						best_level=h_level;
						is_can = TRUE;
						temp_ypos = y_height;
					}
				}
				b_camera[0]+=0.5f;
				n_camera[0]+=0.5f;
			}
			b_camera[0]-=1;
			n_camera[0]-=1;
		}
		if( !is_can )
			return FALSE;
		if( t_tar_pos[0] == tar_pos[0] && t_tar_pos[2] == tar_pos[2] )	//첵크 다햇따.
			break;
		Vector3fCopy(t_tar_pos,t_src_pos);
		Vector3fCopy(tar_pos,t_tar_pos);
		t_src_pos[1]=temp_ypos;
		t_tar_pos[1]=temp_ypos;
	}
	*ypos = temp_ypos;
	return TRUE;

}

/*
서버에서의 Ypos는 속도문제로인해 매루프보다는 캐릭터 키(현재18)를 벗어나지 않을 거리에서 이함수로 
사용하는 편이 빠르다. 
예를 들어 몬스터가 1초에 40정도 움직인다면 18이란 거리를 0.45초만에 가므로 0.45초 전에 ypos를 측정하면 
되겠다. 하지만 좀더 정확해야될 필요가 있으므로 절반인 약0.2초정도로 측정하자..
*/
BOOL CLevel::GetNextYposForServerFar(Vector3f src_pos,Vector3f tar_pos,float *ypos)
{
	BOOL is_can=TRUE;
	LONG leaf_num=0,i;
	SHORT leaf_id[32000];
	Vector3f temp_pos;
	float temp_ypos=-100000;

	if( src_pos[0]== tar_pos[0] && src_pos[1]== tar_pos[1] && src_pos[2]== tar_pos[2])
	{
		*ypos=src_pos[1];
		return TRUE;
	}

#define _CHECK_MIN_DIST_FOR_SERVER	30.0f
	Vector3f t_src_pos,t_tar_pos;

	Vector3fCopy(src_pos,t_src_pos);
	Vector3fCopy(tar_pos,t_tar_pos);


	while( 1 )
	{
		temp_ypos=-100000;
		float dist = GetDist(t_src_pos,t_tar_pos);
		if( dist > _CHECK_MIN_DIST_FOR_SERVER )
		{
			Vector3fSub(t_tar_pos,t_src_pos,temp_pos);
			Normalize(temp_pos);
			Vector3fMul(temp_pos,_CHECK_MIN_DIST,temp_pos);
			Vector3fAdd(t_src_pos,temp_pos,t_tar_pos);
			dist = _CHECK_MIN_DIST_FOR_SERVER;
		}
		float able_height=dist*Y_ABLE_RATE;	//Y_ABLE_RATE배로 측정하자.
		if( able_height < _ABLE_HEIGHT )
			able_height = _ABLE_HEIGHT;

		Vector3fCopy(t_tar_pos,temp_pos);

		//갈수 있는 위아래 높이를 계산해 줘야된다.
		temp_pos[1] = t_tar_pos[1]+able_height;
		t_tar_pos[1]-=able_height;
		mBsp->GetLeafList(temp_pos,t_tar_pos,&leaf_num,leaf_id,32000);
		t_tar_pos[1]+=able_height;

		Vector3f b_camera,n_camera;
		Vector3fCopy(t_tar_pos,b_camera);
		Vector3fCopy(t_tar_pos,n_camera);

		b_camera[1] += 9999;
		n_camera[1] -= 9999;

		is_can = FALSE;
		for( i=0; i<leaf_num; i++)
		{
			float y_height = mBsp->GetYposInLeaf(b_camera,n_camera,able_height,t_tar_pos[1],leaf_id[i] );

			if( y_height != -32000 )
			{
				if( temp_ypos < y_height )
				{
					is_can = TRUE;
					temp_ypos = y_height;
				}
			}
		}
		if( !is_can )
			return FALSE;
		if( t_tar_pos[0] == tar_pos[0] && t_tar_pos[2] == tar_pos[2] )	//첵크 다햇따.
			break;
		Vector3fCopy(t_tar_pos,t_src_pos);
		Vector3fCopy(tar_pos,t_tar_pos);
		t_src_pos[1]=temp_ypos;
		t_tar_pos[1]=temp_ypos;
	}
	*ypos = temp_ypos;
	return TRUE;
}

BOOL CLevel::IsCollisionRayAABB(LONG x,LONG y,Vector3f BBmin,Vector3f BBmax, Vector3f *get_cross)
{
	_R3ENGINE_STATE *state = GetR3State();
	BOOL is_col=FALSE;
	Vector3f from,at;
	float v[3],n[3];

	x -= state->mViewSx;
	y -= state->mViewSy;

    n[0] =  ( ( ( 2.0f * x ) / (state->mViewXL) ) - 1.0f ) / state->mMatProj._11;
    n[1] = -( ( ( 2.0f * y ) / (state->mViewYL) ) - 1.0f) / state->mMatProj._22;
    n[2] =  1.0f;	//현재 왼손 좌표계라...

	v[0] = n[0]*3660000;		//숫자가 커야 오차가 없어진다????
	v[1] = n[1]*3660000;
	v[2] = n[2]*3660000;

    // Get the inverse view matrix
    D3DXMATRIX m;
    D3DXMatrixInverse( &m, NULL, &state->mMatView );

    // Transform the screen space pick ray into 3D space
    at[0]  = v[0]*m._11 + v[1]*m._21 + v[2]*m._31;
    at[1]  = v[0]*m._12 + v[1]*m._22 + v[2]*m._32;
    at[2]  = v[0]*m._13 + v[1]*m._23 + v[2]*m._33;
    from[0] = m._41;
    from[1] = m._42;
    from[2] = m._43;

	if( IsRayAABB(BBmin,BBmax,from,at,get_cross) )
		return TRUE;
	else
		return FALSE;
}

DWORD CLevel::GetPath(Vector3f from,Vector3f at,Vector3f get[16],DWORD *cnt,int depth)	//패스 파인더.
{
	return mBsp->GetPathFind(from,at,get,cnt,depth);	//패스 파인더.
}

BOOL CLevel::GetPathFromDepth(Vector3f from,Vector3f at,int depth,Vector3f get[16],DWORD *cnt)	//일정 깊이의 패스 파인더.
{
	return mBsp->GetPathFind( from, at, get,cnt, depth );
}

void CLevel::DrawBBox(Vector3f bb_min,Vector3f bb_max,DWORD color)	//바운딩 박스를 라인으로 그리는 함수.
{
	int cnt=0;
	_D3DR3VERTEX_TEX0 vertex[1000*3];
	DWORD tc;


    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
    GetD3dDevice()->SetTexture( 0,NULL);

#define LineSet(xx,yy,zz,cc) {	vertex[cnt].x = xx;	vertex[cnt].y = yy;	vertex[cnt].z = zz;		vertex[cnt].color = cc; cnt++;};


	tc=(color|0xff000000);
	LineSet(bb_min[0],bb_min[1],bb_min[2],tc);
	LineSet(bb_max[0],bb_min[1],bb_min[2],tc);
	LineSet(bb_min[0],bb_min[1],bb_min[2],tc);
	LineSet(bb_min[0],bb_min[1],bb_max[2],tc);
	LineSet(bb_max[0],bb_min[1],bb_min[2],tc);
	LineSet(bb_max[0],bb_min[1],bb_max[2],tc);
	LineSet(bb_min[0],bb_min[1],bb_max[2],tc);
	LineSet(bb_max[0],bb_min[1],bb_max[2],tc);

	LineSet(bb_min[0],bb_max[1],bb_min[2],tc);
	LineSet(bb_max[0],bb_max[1],bb_min[2],tc);
	LineSet(bb_min[0],bb_max[1],bb_min[2],tc);
	LineSet(bb_min[0],bb_max[1],bb_max[2],tc);
	LineSet(bb_max[0],bb_max[1],bb_min[2],tc);
	LineSet(bb_max[0],bb_max[1],bb_max[2],tc);
	LineSet(bb_min[0],bb_max[1],bb_max[2],tc);
	LineSet(bb_max[0],bb_max[1],bb_max[2],tc);

	LineSet(bb_min[0],bb_min[1],bb_min[2],tc);
	LineSet(bb_min[0],bb_max[1],bb_min[2],tc);
	LineSet(bb_max[0],bb_min[1],bb_min[2],tc);
	LineSet(bb_max[0],bb_max[1],bb_min[2],tc);
	LineSet(bb_min[0],bb_min[1],bb_max[2],tc);
	LineSet(bb_min[0],bb_max[1],bb_max[2],tc);
	LineSet(bb_max[0],bb_min[1],bb_max[2],tc);
	LineSet(bb_max[0],bb_max[1],bb_max[2],tc);
	DrawPrimitiveUPLine(cnt/2,vertex);
}
void CLevel::DrawBBox(Vector3s bb_min,Vector3s bb_max,DWORD color)	//바운딩 박스를 라인으로 그리는 함수.
{
	Vector3f bb_min_f,bb_max_f;

	bb_min_f[0] = bb_min[0];
	bb_min_f[1] = bb_min[1];
	bb_min_f[2] = bb_min[2];

	DrawBBox(bb_min_f,bb_max_f,color);	//바운딩 박스를 라인으로 그리는 함수.
}

/*
void CLevel::LoadMemoryBSP(char *name)		//bsp를 그대로 메모리에 읽는다.
{
	FILE *fp;
	char filename[256]=".\\map\\";

	strcat(filename,mMapName);
	strcat(filename,".bsp");

	if( (fp=fopen(filename,"rb")) == NULL)
	{
		Warning(filename,"<- 파일이 없슴다.");
		return;
	}
	fread(&mBspMemoryHeader,sizeof(_BSP_FILE_HEADER),1,fp);

	if(mBspMemoryHeader.version != _R3BSP_VERSION)
	{
		Warning("BSP파일 버젼이 맞지않슴다.","장언일");
		fclose(fp);
		return;
	}

	int size=0;
	size+=mBspMemoryHeader.CPlanes.size;
	size+=mBspMemoryHeader.CFaceId.size;
	size+=mBspMemoryHeader.Node.size;
	size+=mBspMemoryHeader.Leaf.size;
	size+=mBspMemoryHeader.MatListInLeaf.size;
	size+=mBspMemoryHeader.Object.size;
	size+=mBspMemoryHeader.Track.size;
	size+=mBspMemoryHeader.CFVertex.size;
	size+=mBspMemoryHeader.CFLine.size;
	size+=mBspMemoryHeader.CFLineId.size;
	size+=mBspMemoryHeader.CFLeaf.size;
	size+=mBspMemoryHeader.BVertex.size;
	size+=mBspMemoryHeader.WVertex.size;
	size+=mBspMemoryHeader.FVertex.size;
	size+=mBspMemoryHeader.VertexColor.size;
	size+=mBspMemoryHeader.UV.size;
	size+=mBspMemoryHeader.LgtUV.size;
	size+=mBspMemoryHeader.Face.size;
	size+=mBspMemoryHeader.FaceId.size;
	size+=mBspMemoryHeader.VertexId.size;
	size+=mBspMemoryHeader.ReadMatGroup.size;

	mBspMemory=(BYTE *)Dmalloc(size);
	fread(mBspMemory,size,1,fp);
	fclose(fp);
}
*/
char *CLevel::GetMapName()		
{
	return mMapName;
}
/*
void CLevel::ReleaseMemoryBSP()		//bsp를 그대로 메모리에 읽는다.
{
	if( mBspMemory ) 
		Dfree(mBspMemory);
	mBspMemory=NULL;
}
*/
void CLevel::GetFrustumNormalPlane(Vector4f n_plane[6])	//core에 view frustum 평면을 
{
	GetFrustumNormalPlane(n_plane);	//core에 view frustum 평면을 
}

void CLevel::HearMapSound()	//맵 사운드를 듣자.(렌더링이 아니므로 endscene()이후에 넣자..)
{
	mBsp->HearMapSound();
}

#define _INTER_SPEED 1.0f
void CLevel::CalcR3Fog()	//포그를 계산한다.
{
	DWORD cnt;
	DWORD list[10000];
	mDummy.GetDummyList(_DUMMY_FLAG_FOG, &cnt, list);	
	if( !cnt )	//포그계산할게 없다.
		return;
	DWORD i,is_in=-1,now_fog;
	float now_start,now_end;
	_R3ENGINE_STATE *state=GetR3State();

	Vector3f pos;
	Vector3fCopy( state->mInvMatView.m[3],pos);
	now_fog = state->mMainFogColor;
	now_start = state->mMainFogStart;
	now_end = state->mMainFogEnd;

	for(i=0; i<cnt; i++)
	{
		if( mDummy.IsInBBox(list[i],pos) )
		{
			_EXT_DUMMY *dummy=mDummy.GetDummy(list[i]);
			now_fog = dummy->mdwArgv[0];
			now_start = dummy->mfArgv[1];
			now_end = dummy->mfArgv[2];
			break;
		}
	}
	
	if( state->mFogColor != now_fog )
	{
		float nr,ng,nb;
		float fr,fg,fb;
		DWORD r,g,b;

		fr = (float)((state->mFogColor>>16)&0xff);
		fg = (float)((state->mFogColor>>8)&0xff);
		fb = (float)((state->mFogColor>>0)&0xff);
		nr = (float)((now_fog>>16)&0xff);
		ng = (float)((now_fog>>8)&0xff);
		nb = (float)((now_fog>>0)&0xff);
		r = (DWORD)(( nr - fr )*_INTER_SPEED*R3GetLoopTime() + fr);
		g = (DWORD)(( ng - fg )*_INTER_SPEED*R3GetLoopTime() + fg);
		b = (DWORD)(( nb - fb )*_INTER_SPEED*R3GetLoopTime() + fb);

		state->mFogColor = 0xff000000|(r<<16)|(g<<8)|(b);
	}

	if( state->mFogStart != now_start )
	{
		state->mFogStart = (now_start - state->mFogStart)*_INTER_SPEED*R3GetLoopTime() + state->mFogStart;
		if( state->mFogStart - now_start < 2 )
			state->mFogStart = now_start;
	}

	if( state->mFogEnd != now_end )
	{
		state->mFogEnd = ( now_end - state->mFogEnd )*_INTER_SPEED*R3GetLoopTime() + state->mFogEnd;
		if( state->mFogEnd - now_end < 2 )
			state->mFogEnd = now_end;
		state->mFar = state->mFogEnd;
		D3DXMatrixPerspectiveFovLH( &state->mMatProj, state->mFov
			, state->mAsfectRatio, state->mNear, state->mFar );
		GetD3dDevice()->SetTransform( D3DTS_PROJECTION, &state->mMatProj );
	}
}




CTimer::CTimer()
{
	memset(this,0,sizeof(CTimer));
	mMinFPS=1.0f;
}

UINT64 CTimer::GetTicks()
{
	UINT64 qw;

	if(QueryPerformanceCounter((LARGE_INTEGER *) &qw))
	{
		while(!qw)
			QueryPerformanceCounter((LARGE_INTEGER *) &qw);
	} 
	else
	{
		qw = (UINT64) timeGetTime();
	}
	return qw;
}
float CTimer::GetDuration()
{
	INT64 qwDuration = (INT64)(GetTicks() - m_qwStartTicks);
	float fDuration = (float)qwDuration / (float)(INT64)m_qwTicksPerSec;
	return fDuration;
}

//#include "r3input.h"

void CTimer::CalculateTime()	//루프마다 해줘야된다.
{
	float fFramesPerSec =1000;

//	if (gScan[VK_L])
  //      m_bTimerInitialized = FALSE;

    if( FALSE == m_bTimerInitialized )
    {
        m_bTimerInitialized = TRUE;
		if(!QueryPerformanceFrequency((LARGE_INTEGER *) &m_qwTicksPerSec))
			m_qwTicksPerSec = 1000;
		m_fTicksPerSec  = (float) (__int64) m_qwTicksPerSec;
		m_fSecsPerFrame = 1.0f / fFramesPerSec;
		m_qwTicksPerFrame = m_qwTicksPerSec / (__int64) fFramesPerSec;
		m_qwStartTicks = m_qwTicks = GetTicks();
 	}

	if( 0 )
	{
		UINT64 qw;
		qw = GetTicks();
		if(qw != m_qwTicks)
		{
			m_qwTicksPerFrame = qw - m_qwTicks;
			m_qwTicks = qw;
	//		m_fFramesPerSec = 0.75f * m_fFramesPerSec 
	//			+ 0.25f * (m_fTicksPerSec / (float) (__int64) m_qwTicksPerFrame);
			m_fFramesPerSec = 0.00f * m_fFramesPerSec 
				+ 1.00f * (m_fTicksPerSec / (float) (__int64) m_qwTicksPerFrame);
			if(m_fFramesPerSec < 1.0f)
				m_fFramesPerSec = 1.0f;
			m_fSecsPerFrame = 1.0f / m_fFramesPerSec;
			mLoopTime=m_fSecsPerFrame;
		}
	}
	else
	{
		DWORD gop=timeGetTime();
		DWORD sub=gop-mOldTime;
		
		mLoopCnt++;
		mLoopHop+=sub;
	//	if( mLoopHop > 200 )
		if( mLoopHop > 10 )
		{
			mLoopTime = (mLoopHop/(float)mLoopCnt)/1000.0f;
			mLoopCnt=0;
			mLoopHop=0;
		}
		mOldTime = gop;

		if( mLoopTime > mMinFPS || mLoopTime < 0 )
		{
			mLoopTime=mMinFPS;
		}

//		SetMultiLayerTime(mRealTime);
	}


	mRealTime+=mLoopTime;
	mFPSTime+=mLoopTime;
	mLoopFPSCnt++;
	if( mFPSTime > 1 )
	{
		mFPS = mLoopFPSCnt/mFPSTime;
		mLoopFPSCnt=0;
		mFPSTime=0;
	}
}
float CTimer::GetLoopTime()	//한루프의 도는 시간을 건진다.
{
	return mLoopTime;
}

float CTimer::GetTime()				//현재 시간을 알아낸다.
{
	return mRealTime;
}
void CTimer::SetTime(float time)	//타임을 재 설정...
{
	mRealTime=time;
}
void CTimer::SetMinFPS(float min_fps)	//최소 프래임 레이트를 정의한다.
{
	mMinFPS=1.0f/min_fps;
}


