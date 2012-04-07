#include "core.h"
#include "jmalloc.h"
#include "r3math.h"
#include "r3sound.h"

_R3ENGINE_STATE stState;
BOOL stIsInit=FALSE;

BYTE *stTemp;

void SetMainLight(Vector3f lgt_pos)
{
	float sat=lgt_pos[0]*lgt_pos[0]+lgt_pos[2]*lgt_pos[2];
	if( sat < 0.1f )
		sat=0.1f;
	sat = sqrt(sat);

	Vector3fCopy(lgt_pos,stState.mMainLightNomal);
	Vector3fCopy(lgt_pos,stState.mLightNomalForShadow);	
	stState.mLightNomalForShadow[1] = (sat*5.0f)/3.0f;	//지금 쉐도우하고 맞추기 위해서...
	Normalize(stState.mLightNomalForShadow);

	Vector3f temp;
	Vector3fCopy(stState.mMainLightNomal,temp);
	Normalize(stState.mMainLightNomal);
	temp[1]=0;
	Normalize(temp);
	stState.mMainLightYAngle = acos(temp[2]);
	if( temp[0] < 0 )
		stState.mMainLightYAngle=-stState.mMainLightYAngle;
//	stState.mMainLightYAngle=stState.mMainLightYAngle+3.141592f;
}

void InitCore()
{
	int i;

	memset(&stState,0,sizeof(_R3ENGINE_STATE));
	stIsInit=TRUE;
	stState.mAsfectRatio = 640/480.0f;
	stState.mNear = 2.0f;
	stState.mFar = 6000.0f;
//	stState.mFov =  (D3DX_PI/(2.8f));		//48.29도이다.
	stState.mFov = _DEFINE_FOV; 
	stState.mMipMapBias = 0;//-0.5f;//-0.5f;
	D3DXMatrixIdentity(&stState.mMatView);
	D3DXMatrixIdentity(&stState.mMatWorld);
	D3DXMatrixIdentity(&stState.mMatProj);
	stState.mFogColor=0;
	stState.mFogStart=0;
	stState.mFogEnd=0;
	stState.mIsFog=FALSE;
	stState.mFogRangeEnable=FALSE;	//포그 범위킬까?
	stState.mTempSize=1024*256;//	딱 256K
	stState.mGamma = 1.0f;
	stState.mFlySpeed=30;
	stState.mWideOn=FALSE;
	stState.mIsDrawLogo=FALSE;
	
//	stState.mVFogTexture=0;
	stState.mDLightTexture=0;
	stTemp = (BYTE *)Dmalloc(stState.mTempSize);

	//렌즈플레어관련...
	stState.mLensFlareTex=-1;	//텍스쳐 아이디를 클리어..
	for(i=0; i<16; i++)
		stState.mLensFlareScale[i]=1.0f;	//스케일 미리지정.

	//각종 패스 셋팅.
	GetPrivateProfileString("Path","MapPath",".\\Map\\",stState.MapPath, 128,".\\R3Engine.ini");
	GetPrivateProfileString("Path","SystemPath",".\\System\\",stState.SystemPath, 128,".\\R3Engine.ini");
	GetPrivateProfileString("Path","EntityPath",".\\Map\\Entity\\",stState.EntityPath, 128,".\\R3Engine.ini");
	GetPrivateProfileString("Path","EffectPath",".\\Effect\\",stState.EffectPath, 128,".\\R3Engine.ini");
	GetPrivateProfileString("Path","ScreenShotsPath",".\\ScreenShots\\",stState.ScreenShotsPath, 128,".\\R3Engine.ini");
	GetPrivateProfileString("Path","SoundPath",".\\Snd\\",stState.SoundPath, 128,".\\R3Engine.ini");

	char temp[256];
	GetPrivateProfileString("RenderState","ScreenXSize","1024",temp, 128,".\\R3Engine.ini");
	stState.mScreenXsize = (float)atoi(temp);
	stState.mViewSx=0;
	stState.mViewXL=(LONG)stState.mScreenXsize;
	GetPrivateProfileString("RenderState","ScreenYSize","768",temp, 128,".\\R3Engine.ini");
	stState.mScreenYsize = (float)atoi(temp);
	stState.mViewSy=0;
	stState.mViewYL=(LONG)stState.mScreenYsize;
	GetPrivateProfileString("RenderState","RenderBits","16",temp, 128,".\\R3Engine.ini");
	stState.mRenderBits = atoi(temp);
	GetPrivateProfileString("RenderState","ZbufferBits","24",temp, 128,".\\R3Engine.ini");
	stState.mZbufferBits = atoi(temp);
	GetPrivateProfileString("RenderState","bFullScreen","FALSE",temp, 128,".\\R3Engine.ini");
	strupr(temp);
	if( !strcmp(temp,"TRUE"))
		stState.mbFullScreen = TRUE;
	else
		stState.mbFullScreen = FALSE;

	//------------- CFG파일에 있는 항목들..
	GetPrivateProfileString("RenderState","TextureDetail","2",temp, 128,".\\R3Engine.ini");
	stState.mTextureDetail = (DWORD)atoi(temp);//텍스쳐 디테일 값...
	stState.mTextureDetail = max(stState.mTextureDetail,0);
	stState.mTextureDetail = min(stState.mTextureDetail,3);
	stState.mTextureDetail = 3 - stState.mTextureDetail;
	stState.mSceneTextureDetail = max(0,(LONG)stState.mTextureDetail-1);

	GetPrivateProfileString("RenderState","DynamicLight","1",temp, 128,".\\R3Engine.ini");
	stState.mDynamicLight = (DWORD)atoi(temp);	//동적라이팅수...
	stState.mDynamicLight = max(stState.mDynamicLight,0);
	stState.mDynamicLight = min(stState.mDynamicLight,3);
	if( stState.mDynamicLight == 1 )
		stState.mDynamicLight =3;
	else
	if( stState.mDynamicLight == 2 )
		stState.mDynamicLight =6;
	else
	if( stState.mDynamicLight == 3 )
		stState.mDynamicLight =10;

	GetPrivateProfileString("RenderState","ShadowDetail","1",temp, 128,".\\R3Engine.ini");
	stState.mShadowDetail = (DWORD)atoi(temp);	//그림자 갯수..
	stState.mShadowDetail = max(stState.mShadowDetail,0);
	stState.mShadowDetail = min(stState.mShadowDetail,3);

	if( stState.mShadowDetail == 1 )
		stState.mShadowDetail =1;
	else
	if( stState.mShadowDetail == 2 )
		stState.mShadowDetail =10;
	else
	if( stState.mShadowDetail == 3 )
		stState.mShadowDetail =-1;

	GetPrivateProfileString("RenderState","bDetailTexture","TRUE",temp, 128,".\\R3Engine.ini");
	if( !strcmp(temp,"TRUE"))	//디테일 텍스쳐.
		stState.mDetailTexture = TRUE;
	else
		stState.mDetailTexture = FALSE;

	GetPrivateProfileString("RenderState","bMouseAccelation","TRUE",temp, 128,".\\R3Engine.ini");
	strupr(temp);
	if( !strcmp(temp,"TRUE"))	//마우스 가속.
		stState.mMouseAccelation = TRUE;
	else
		stState.mMouseAccelation = FALSE;

	float see_table[4] = {0.6f, 0.8f, 1.0f};
	DWORD dw_temp;
	GetPrivateProfileString("RenderState","SeeDistance","2",temp, 128,".\\R3Engine.ini");
	dw_temp = (DWORD)atoi(temp);	//시야 거리..
	dw_temp = max(dw_temp,0);
	dw_temp = min(dw_temp,2);
	stState.mSeeDistance =see_table[dw_temp];

	GetPrivateProfileString("Sound","Sound","TRUE",temp, 128,".\\R3Engine.ini");
	strupr(temp);
	if( !strcmp(temp,"TRUE"))	//효과음..
		stState.mSoundOn = TRUE;
	else
		stState.mSoundOn = FALSE;

	GetPrivateProfileString("Sound","Music","TRUE",temp, 128,".\\R3Engine.ini");
	strupr(temp);
	if( !strcmp(temp,"TRUE"))	//음악끔..
		stState.mMusicOn = TRUE;
	else
		stState.mMusicOn = FALSE;

	GetPrivateProfileString("Sound","MusicVol","1.0f",temp, 128,".\\R3Engine.ini");
	stState.mMusicVol = (float)atof(temp);
	SetGlobalMusicVolume(stState.mMusicVol);	//0-1까지..


	GetPrivateProfileString("Sound","SoundVol","1.0f",temp, 128,".\\R3Engine.ini");
	stState.mSoundVol = (float)atof(temp);
	SetGlobalWavVolume(stState.mSoundVol);	//0-1까지..


	GetPrivateProfileString("RenderState","Gamma","1.0f",temp, 128,".\\R3Engine.ini");
	stState.mGamma = (float)atof(temp);
	SetGamma(stState.mGamma);

//-----------------------------------------------------------------------------------
	GetPrivateProfileString("RenderState","bStartCamera","FALSE",temp, 128,".\\R3Engine.ini");
	strupr(temp);
	if( !strcmp(temp,"TRUE"))
		stState.mbStartCamera = TRUE;
	else
		stState.mbStartCamera = FALSE;
	
	GetPrivateProfileString("SampleCharacter","CharPath",".\\Character\\",stState.CharPath, 128,".\\R3Engine.ini");
	GetPrivateProfileString("SampleCharacter","CharTexturePath",".\\Character\\tex\\",stState.CharTexturePath, 128,".\\R3Engine.ini");
	GetPrivateProfileString("SampleCharacter","MeshName","SNWG_001.msh",stState.MeshName, 128,".\\R3Engine.ini");
	GetPrivateProfileString("SampleCharacter","BoneName","bone_b.bn",stState.BoneName, 128,".\\R3Engine.ini");
	GetPrivateProfileString("SampleCharacter","StopAni","Stop.ani",stState.StopAniName, 128,".\\R3Engine.ini");
	GetPrivateProfileString("SampleCharacter","RunAni","Run_Loop.ani",stState.RunAniName, 128,".\\R3Engine.ini");
	GetPrivateProfileString("SampleCharacter","SwordAni","",stState.SwordAniName, 128,".\\R3Engine.ini");
	GetPrivateProfileString("SampleCharacter","SwordMesh","",stState.SwordMeshName, 128,".\\R3Engine.ini");
	GetPrivateProfileString("SampleCharacter","SwordTexturePath","",stState.SwordTexturePath, 128,".\\R3Engine.ini");

//	Vector3f l_pos={0,5,3};
	Vector3f stMainDirLight={1,1,1};
//	Vector3f l_pos={0.3,0.8f,0.5f};
	SetMainLight(stMainDirLight);
}


void *GetTempBuffer()
{
	return (void*)stTemp;
}
DWORD GetTempBufferSize()
{
	return stState.mTempSize;
}
/*
_R3ENGINE_STATE *GetR3State()
{
	return &stState;
}*/
BOOL IsInitCore()
{
	return stIsInit;
}
void ReleaseCore()
{
	SetGamma(1.0f);	//감마 복원.
	Dfree(stTemp);
	stIsInit=FALSE;
}
template <class A> inline A Between(const A a, const A b, const A c) 
{
	return b < a ? a : (b > c ? c : b); 
}

void SetGamma(float gamma)
{
	WORD g_gamma[768*2];
	int dest;
	HDC hdc=GetDC(0); // desktop dc
//	GetDeviceGammaRamp(hdc, (LPVOID) g_gamma);
	for (int t=0;t<256;t++)
	{
		// 0 과 255사이의 (256.f * pow((double)i/256.f, 1/gamma) 값
		BYTE a = Between(0, (int) (256.f * pow((double)t/256.f, 1/gamma)), 255);
		dest = t;
		g_gamma[dest]			= a*256;
		g_gamma[dest+(256*1)]	= a*256;
		g_gamma[dest+(256*2)]	= a*256;
	}

	SetDeviceGammaRamp(hdc, (LPVOID) g_gamma);
	ReleaseDC(0,hdc);
}
void SetFrustumNormalPlane()	//core에 view frustum 평면을 
{
	D3DXMATRIX view_proj;

	D3DXMatrixMultiply(&view_proj,&stState.mMatView,&stState.mMatProj);
	//left clip plane.
	stState.mFrustumPlane[0][0] = -(view_proj._14 + view_proj._11);
	stState.mFrustumPlane[0][1] = -(view_proj._24 + view_proj._21);
	stState.mFrustumPlane[0][2] = -(view_proj._34 + view_proj._31);
	stState.mFrustumPlane[0][3] = -(view_proj._44 + view_proj._41);

	//right clip plane.
	stState.mFrustumPlane[1][0] = -(view_proj._14 - view_proj._11);
	stState.mFrustumPlane[1][1] = -(view_proj._24 - view_proj._21);
	stState.mFrustumPlane[1][2] = -(view_proj._34 - view_proj._31);
	stState.mFrustumPlane[1][3] = -(view_proj._44 - view_proj._41);

	//top clip plane.
	stState.mFrustumPlane[2][0] = -(view_proj._14 - view_proj._12);
	stState.mFrustumPlane[2][1] = -(view_proj._24 - view_proj._22);
	stState.mFrustumPlane[2][2] = -(view_proj._34 - view_proj._32);
	stState.mFrustumPlane[2][3] = -(view_proj._44 - view_proj._42);

	//bottom clip plane.
	stState.mFrustumPlane[3][0] = -(view_proj._14 + view_proj._12);
	stState.mFrustumPlane[3][1] = -(view_proj._24 + view_proj._22);
	stState.mFrustumPlane[3][2] = -(view_proj._34 + view_proj._32);
	stState.mFrustumPlane[3][3] = -(view_proj._44 + view_proj._42);

	//near clip plane.
	stState.mFrustumPlane[4][0] = -(view_proj._14 + view_proj._13);
	stState.mFrustumPlane[4][1] = -(view_proj._24 + view_proj._23);
	stState.mFrustumPlane[4][2] = -(view_proj._34 + view_proj._33);
	stState.mFrustumPlane[4][3] = -(view_proj._44 + view_proj._43);

	//far clip plane.
	stState.mFrustumPlane[5][0] = -(view_proj._14 - view_proj._13);
	stState.mFrustumPlane[5][1] = -(view_proj._24 - view_proj._23);
	stState.mFrustumPlane[5][2] = -(view_proj._34 - view_proj._33);
	stState.mFrustumPlane[5][3] = -(view_proj._44 - view_proj._43);

	for(int i=0; i<6; i++)
	{
		float denom = 1.0f / Vector3fDist(stState.mFrustumPlane[i]);
		stState.mFrustumPlane[i][0] *=denom;
		stState.mFrustumPlane[i][1] *=denom;
		stState.mFrustumPlane[i][2] *=denom;
		stState.mFrustumPlane[i][3] *=denom;
	}
}

BOOL IsBBoxInFrustum( Vector3f bb_min,Vector3f bb_max )
{
	Vector3f near_p;

	for( int i=0; i<6; i++ )
	{
		if( stState.mFrustumPlane[i][0] > 0 )
		{
			if( stState.mFrustumPlane[i][1] > 0 )
			{
				if( stState.mFrustumPlane[i][2] > 0 )
				{
					near_p[0] = bb_min[0];	near_p[1] = bb_min[1];	near_p[2] = bb_min[2];
				}
				else
				{
					near_p[0] = bb_min[0];	near_p[1] = bb_min[1];	near_p[2] = bb_max[2];
				}
			}
			else
			{
				if( stState.mFrustumPlane[i][2] > 0 )
				{
					near_p[0] = bb_min[0];	near_p[1] = bb_max[1];	near_p[2] = bb_min[2];
				}
				else
				{
					near_p[0] = bb_min[0];	near_p[1] = bb_max[1];	near_p[2] = bb_max[2];
				}
			}
		}
		else
		{
			if( stState.mFrustumPlane[i][1] > 0 )
			{
				if( stState.mFrustumPlane[i][2] > 0 )
				{
					near_p[0] = bb_max[0];	near_p[1] = bb_min[1];	near_p[2] = bb_min[2];
				}
				else
				{
					near_p[0] = bb_max[0];	near_p[1] = bb_min[1];	near_p[2] = bb_max[2];
				}
			}
			else
			{
				if( stState.mFrustumPlane[i][2] > 0 )
				{
					near_p[0] = bb_max[0];	near_p[1] = bb_max[1];	near_p[2] = bb_min[2];
				}
				else
				{
					near_p[0] = bb_max[0];	near_p[1] = bb_max[1];	near_p[2] = bb_max[2];
				}
			}
		}
		if( Vector3fDot(stState.mFrustumPlane[i],near_p) + stState.mFrustumPlane[i][3] > 0 )
			return FALSE;
	}
	return TRUE;
}
BOOL IsBBoxInFrustum( Vector3s bb_min,Vector3s bb_max )	//바운딩 박스가 프러스텀안에 드는가?
{
	Vector3f bbmin,bbmax;
	bbmin[0] = (float)bb_min[0];
	bbmin[1] = (float)bb_min[1];
	bbmin[2] = (float)bb_min[2];
	bbmax[0] = (float)bb_max[0];
	bbmax[1] = (float)bb_max[1];
	bbmax[2] = (float)bb_max[2];
	return IsBBoxInFrustum( bbmin,bbmax );
}
void GetFrustumNormalPlane(Vector4f n_plane[6])	//core에 view frustum 평면을 
{
	memcpy(n_plane,stState.mFrustumPlane,sizeof(Vector4f)*6);
}

