#include <windows.h>
#include "R3Util.h"
#include "commonutil.h"
#include "R3math.h"
#include "Core.h"
#include "MultiLayer.h"
#include "2dsprite.h"
#include "procedural.h"
#include "R3render.h"
#include "r3enginekernel.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static float stQuakeTime;
static float stQuakeTimeFirst;
static float stQuakeDensity;

void R3GetQuakeMatrix(float mat[4][4])	//Quake 매트릭스를 만들어주자.
{
	if( stQuakeTime <= 0)	//지진 쓸일이 없다.
		return;
	float loop_time = R3GetLoopTime();
	stQuakeTime-=R3GetLoopTime();
	if( stQuakeTime < 0 )
		stQuakeTime=0;
	float alpha = 1-(stQuakeTimeFirst-stQuakeTime)/stQuakeTimeFirst;
	float a=Noise1((DWORD)(loop_time*1000))*alpha;
	mat[3][0]+=a*stQuakeDensity;
	a=Noise1((DWORD)(loop_time*1000+200))*alpha;
	mat[3][1]+=a*stQuakeDensity;
	a=Noise1((DWORD)(loop_time*1000+400))*alpha;
	mat[3][2]+=a*stQuakeDensity;
}
void R3GetQuakeVector(Vector3f pos)	//Quake 매트릭스를 만들어주자.
{
	if( stQuakeTime <= 0)	//지진 쓸일이 없다.
		return;
	float loop_time = R3GetLoopTime();
/*	펄린 노이즈를 이용하는건데 흔들리는 느낌이 좀 약하다.
	float alpha = 1-(stQuakeTimeFirst-stQuakeTime)/stQuakeTimeFirst;
	float a=Noise1((DWORD)(loop_time*1000))*alpha;
	pos[0]+=a*stQuakeDensity;
	a=Noise1((DWORD)(loop_time*1000+200))*alpha;
	pos[1]+=a*stQuakeDensity;
	a=Noise1((DWORD)(loop_time*1000+400))*alpha;
	pos[2]+=a*stQuakeDensity;
*/

	_R3ENGINE_STATE *state = GetR3State();
	Vector3f view_vec;

	view_vec[0] = state->mMatView.m[2][2];	//쳐다보고있는 각도와 
	view_vec[1] = 0;
	view_vec[2] = state->mMatView.m[2][0];
	if( fabs(view_vec[0]) < 0.01f && fabs(view_vec[2]) < 0.01f )
	{
		view_vec[0]=1;
		view_vec[0]=0;
	}
	float dist = Vector3fDist(view_vec);
	float gac = (float)acos(view_vec[0]/dist);
	view_vec[0]/=dist;
	view_vec[1]/=dist;
	view_vec[2]/=dist;

	float dist_m= (float)R3GetToggle30fps();//(char)ch->GetDistortDamage();
	float x_add = (float)cos(gac)*dist_m;
	float y_add = (float)sin(gac)*dist_m;
	float alpha = 1-(stQuakeTimeFirst-stQuakeTime)/stQuakeTimeFirst;

	pos[0]+=x_add*alpha*stQuakeDensity;
	float a=Noise1((DWORD)(loop_time*1000+200))*alpha;
	pos[1]+=a*stQuakeDensity;
	pos[2]+=y_add*alpha*stQuakeDensity;

	stQuakeTime-=R3GetLoopTime();
	if( stQuakeTime < 0 )
		stQuakeTime=0;
}
//지진 만들자... 앗싸.
void R3EnvironmentQuake(float time,float density)
{
	stQuakeTimeFirst=time;
	stQuakeTime=time;
	stQuakeDensity=density;
}

//--------------------------다이나믹 라이트 부분...

#define _MAX_DLIGHT_NUM 10	//다이나믹 라이트 최대 10개...

typedef struct{
	Vector3f pos;
	float diameter;
	DWORD color;
	void *tex;
	BOOL bMLight;
	Vector3f BBMin;
	Vector3f BBMax;
	DWORD blend_type;
}_D_LIGHT;

static _D_LIGHT stDLight[_MAX_DLIGHT_NUM];
DWORD stDLightNum=0;
void ClearDynamicLight()
{
	stDLightNum=0;
}

DWORD GetDynamicLightNum()
{
	return stDLightNum;
}
void GetDynamicLightBBox(DWORD id,Vector3f *bbmin,Vector3f *bbmax)
{
	Vector3fCopy(stDLight[id].BBMin,bbmin[0]);
	Vector3fCopy(stDLight[id].BBMax,bbmax[0]);
}
BOOL IsMagicLight(DWORD id)	//매직라이트인가?
{
	return stDLight[id].bMLight;
}
void GetDynamicLight(DWORD id,Vector3f pos,float *diameter,DWORD *color,void **texture,DWORD *blend_type)
{
	if( stDLightNum > _MAX_DLIGHT_NUM || stDLightNum < 0 )
	{
		pos[0]=0;		pos[1]=0;		pos[2]=0;
		*diameter=1;
		*color=0;
		*texture=NULL;
		return;
	}
	*diameter = stDLight[id].diameter;
	*color = stDLight[id].color;
	*texture = stDLight[id].tex;
	*blend_type = stDLight[id].blend_type;
	Vector3fCopy(stDLight[id].pos,pos);
}

BOOL InsertDynamicLight(Vector3f pos,float diameter,DWORD color,void *texture,BOOL magic_light,DWORD blend_type)
{
	if( stDLightNum >= _MAX_DLIGHT_NUM )
		return FALSE;
	_R3ENGINE_STATE *state= GetR3State();
	DWORD d_num = state->mDynamicLight;
	if( d_num == 0 )
		return FALSE;

	if( stDLightNum >= d_num )
	{
		stDLightNum=d_num-1;
		memcpy(&stDLight[0],&stDLight[1],sizeof(_D_LIGHT)*(d_num-1));
	}
	Vector3fCopy(pos,stDLight[stDLightNum].pos);
	stDLight[stDLightNum].diameter=diameter;
	stDLight[stDLightNum].color=color;
	stDLight[stDLightNum].tex=texture;
	stDLight[stDLightNum].bMLight = magic_light;
	stDLight[stDLightNum].BBMin[0]=-diameter;
	stDLight[stDLightNum].BBMin[1]=-diameter;
	stDLight[stDLightNum].BBMin[2]=-diameter;
	stDLight[stDLightNum].BBMax[0]=diameter;
	stDLight[stDLightNum].BBMax[1]=diameter;
	stDLight[stDLightNum].BBMax[2]=diameter;
	stDLight[stDLightNum].blend_type=blend_type;
	Vector3fAdd(pos,stDLight[stDLightNum].BBMin,stDLight[stDLightNum].BBMin);
	Vector3fAdd(pos,stDLight[stDLightNum].BBMax,stDLight[stDLightNum].BBMax);

	stDLightNum++;
	return TRUE;
}


void GetCameraPos(Vector3f *get_pos)	//카메라 월드상의 위치백터.
{
	_R3ENGINE_STATE *state = GetR3State();

	get_pos[0][0] = state->mInvMatView.m[3][0];
	get_pos[0][1] = state->mInvMatView.m[3][1];
	get_pos[0][2] = state->mInvMatView.m[3][2];
}


//-----------페이드 인 아웃 루틴.
static DWORD stFadeColor;
static float stFadeTime=0;
static float stFadeAlpha=0;	//1일때 완전 페이드인 0일때 완전 페이드 아웃상태.
static float stStartTick;
static float stFadeDensity=1.0f;
void SetFadeFactor(DWORD color,float alpha,void *tex)	//알파가 1에 가까울수록 해당 페이드 색이 강하다.
{
	_R3ENGINE_STATE *state=GetR3State();

	color = (color&0x00ffffff)|((DWORD)(alpha*255)<<24);
	if( tex )
		Draw2DTexture(0,0,(LONG)state->mScreenXsize,(LONG)state->mScreenYsize, color,tex);//뿌리자.
	else
		Draw2DRectangle(0,0,(LONG)state->mScreenXsize,(LONG)state->mScreenYsize, color);//뿌리자.
}

BOOL IsFading()
{
	if(stFadeTime==0)
		return FALSE;
	return TRUE;
}
BOOL IsFadeOut()		//페이드아웃 된 상태인가?
{
	if( stFadeAlpha== 1)	//페이드 아웃 상태다 
		return TRUE;
	return FALSE;
}
BOOL IsFadeIn()		//페이드인된 상태인가?
{
	if( stFadeAlpha== 0)	//페이드 인 상태다 
		return TRUE;
	return FALSE;
}

void SetFadeIn(DWORD color,float time,float density)		//해당되는 컬러와 페이드되는 시간.
{
	stFadeColor = color;
	stFadeTime = time+0.0001f;
	stStartTick = R3GetTime();
	stFadeDensity=density;
}

void SetFadeOut(DWORD color,float time,float density)		//해당되는 컬러와 페이드되는 시간.
{
	stFadeColor = color;
	stFadeTime = -time-0.0001f;
	stStartTick = R3GetTime();
	stFadeAlpha=0.0001f;
	stFadeDensity=density;
}

BOOL DrawFadeScreen()
{
	if( stFadeAlpha== 0)	//페이드 인 상태다 
	{
		DrawLogo();
		return TRUE;
	}

	if( stFadeTime > 0 )	//페이드인.
	{
		float tick = (R3GetTime()-stStartTick+0.0001f)/stFadeTime;
		if( tick > 1 )//1넘어가면 끝.
		{
			tick=1;
			stFadeAlpha=0;
			stFadeTime=0;
		}
		else
			stFadeAlpha = 1-tick;
	}
	else
	if( stFadeTime < 0 )	//페이드 아웃.
	{
		float tick = (R3GetTime()-stStartTick+0.0001f)/-stFadeTime;
		if( tick > 1 )//1넘어가면 끝.
		{
			tick=1;
			stFadeAlpha=1;
			stFadeTime=0;
		}
		else
			stFadeAlpha = tick;
	}
	else	//페이드 인이나 페이드 아웃된 상태...
	{
	}
	SetFadeFactor(stFadeColor,stFadeAlpha*stFadeDensity);	//알파가 1에 가까울수록 해당 페이드 색이 강하다.
	DrawLogo();
	return FALSE;
}



float GetDist(Vector3f Pos,Vector3f Tar)
{
	return sqrtf((Tar[0]-Pos[0])*(Tar[0]-Pos[0])+(Tar[2]-Pos[2])*(Tar[2]-Pos[2]));
}
float GetYAngle(Vector3f Pos,Vector3f Tar)	// 각도(360)을 리턴한다.
{
	double Sqrt = GetDist(Tar,Pos);
	double Acos=(Pos[2]-Tar[2])/ Sqrt;
	if( Acos >1.0f)
		Acos = 0;
	else
	if(Acos <-1.0f)
		Acos = 3.14f;
	else
		Acos = acos(Acos);
 
	if(Pos[0]-Tar[0] > 0)
		return((float)(PiToAngle( Acos )+180));
	else
		return((float)(PiToAngle( -Acos )+180));
}

float GetXAngle(Vector3f Pos,Vector3f Tar)	// 각도(360)을 리턴한다.
{
	double Sqrt = GetDist(Tar,Pos);
	double Acos=(Pos[1]-Tar[1])/ Sqrt;
	if( Acos >1.0f)
		Acos = 0;
	else
	if(Acos <-1.0f)
		Acos = 3.14f;
	else
		Acos = acos(Acos);
 
	return((float)(PiToAngle( -Acos )+90));
}

BOOL GetYAngle(float *get_yangle, Vector3f Pos,Vector3f Tar)	// 각도(360)을 리턴한다.
{
	if( Tar[0]==Pos[0] && Tar[1]==Pos[1] && Tar[2]==Pos[2] )
		return FALSE;

	double Sqrt = GetDist(Tar,Pos);
	if( Sqrt < 0.2f )
		return FALSE;

	double Acos=(Pos[2]-Tar[2])/ Sqrt;
	if( Acos >1.0f)
		Acos = 0;
	else
	if(Acos <-1.0f)
		Acos = 3.14f;
	else
		Acos = acos(Acos);
 
	if(Pos[0]-Tar[0] > 0)
		*get_yangle = (float)(PiToAngle( Acos )+180);
	else
		*get_yangle = (float)(PiToAngle( -Acos )+180);
	return TRUE;
}

void GetMatrixFromAtoB(float get_mat[4][4],Vector3f a,Vector3f b)	//a가 b를 향해 갈때의 매트릭스를 구한다.
{
	float temp_mat[4][4];
	float yangle = GetYAngle( a,b );
	float xangle = GetXAngle( a,b );

	MatrixRotateY(get_mat,yangle);
	MatrixRotateX(temp_mat,xangle);
	MatrixMultiply(get_mat,get_mat,temp_mat);
}

void Clean2DRectangleZbuffer(LONG x,LONG y,LONG ex,LONG ey)//z버퍼 영역을 클리어.
{
	CleanZbuffer((float)x,(float)y,(float)ex-(float)x,(float)ey-(float)y);
}

// 스크린 좌표를 얻는경우.
void Get2DTo3DTranslation(Vector3f *get,Vector3f bb_min,Vector3f bb_max,float sx,float sy,float ex,float ey,float scale)
{
	_R3ENGINE_STATE *state=GetR3State();

    float maxgop,mingop,maxbuf;
	float maxygop,minygop,maxybuf,temp;

    temp=max((float)fabs(bb_min[0]),(float)fabs(bb_min[2]));
    maxgop=max(bb_max[0],bb_max[2]);
    maxgop=max(maxgop,temp);	//최대 x,z 끝을 구한다.

    maxygop=(bb_max[1]-bb_min[1])/2;
    mingop=min(bb_min[0],bb_min[2]);
    minygop=bb_min[1];

	maxbuf=(float)fabs(maxgop);
	maxbuf=(float)max(fabs(mingop),maxbuf);
	maxybuf=(float)fabs(maxygop);
	maxybuf=(float)max(fabs(minygop),maxybuf);

	maxgop = (state->mMatProj.m[0][0]*maxbuf);
	mingop = (state->mMatProj.m[1][1]*maxybuf);
	
	//중심 포지션을 구한다.
	get[0][0]=0;	//x는 고려하지 않음...
	get[0][1]=-(bb_min[1] + (bb_max[1] - bb_min[1])/2);	//y는 보정해야...
	get[0][2]=max(maxgop,mingop)/scale;	//마지막 z값.
}

BOOL GetTransformVertex(Vector4f *xyzw,Vector3f pos)	//pos값을 스크린좌표로 가져온다.
{
	_R3ENGINE_STATE *state = GetR3State();
	float x_center = (float)(state->mViewXL/2+state->mViewSx);
	float y_center = (float)(state->mViewYL/2+state->mViewSy);
	
	SetTransformClipInfo((float)state->mViewXL,(float)state->mViewYL	,x_center ,y_center );
	if( TransformVertex( xyzw[0], state->mMatViewProj.m,pos) > 0)
		return TRUE;
	return FALSE;	//화면 뒤에 점이 있따...
}

void GetViewMatrix(D3DXMATRIX *get_matrix)
{
	_R3ENGINE_STATE *state = GetR3State();
	memcpy(get_matrix,state->mMatView,sizeof(D3DXMATRIX));
}

void GetProjectMatrix(D3DXMATRIX *get_matrix)
{
	_R3ENGINE_STATE *state = GetR3State();
	memcpy(get_matrix,state->mMatProj,sizeof(D3DXMATRIX));
}

void GetBBoxRotate(Vector3f *get_bb_min,Vector3f *get_bb_max,Vector3f bb_min,Vector3f bb_max,float y_angle)
{
	float mat[4][4];
	MatrixRotate(mat,0,y_angle,0);
	Vector3fTransform( *get_bb_min,bb_min,mat);
	Vector3fTransform( *get_bb_max,bb_max,mat);
}

//현재 컬러를 넣어주면 라이트와 머터리얼을 셋팅해준다.
void GetMatLightFromColor( D3DLIGHT8 *light,D3DMATERIAL8 *mtrl,DWORD color,float density )
{
	D3DXVECTOR3 vecDir;
	LPDIRECT3DDEVICE8 pd3dDevice = GetD3dDevice();

	ZeroMemory( light, sizeof(D3DLIGHT8) );
	light->Type       = D3DLIGHT_DIRECTIONAL;

	light->Diffuse.r  = 1.0f;
	light->Diffuse.g  = 1.0f;
	light->Diffuse.b  = 1.0f;

	light->Ambient.r = 0;
	light->Ambient.g = 0;
	light->Ambient.b = 0;

	light->Range       = 1000.0f;

	light->Diffuse.a = 1;
/*
	light->Diffuse.r = ((color>>16)&255)/255.0f;
	light->Diffuse.g = ((color>>8)&255)/255.0f;
	light->Diffuse.b = ((color>>0)&255)/255.0f;
*/

	light->Ambient.a = 1;
//	light->Ambient.r = ((color>>16)&255)/255.0f;
//	light->Ambient.g = ((color>>8)&255)/255.0f;
//	light->Ambient.b = ((color>>0)&255)/255.0f;


	memset(mtrl,0,sizeof(D3DMATERIAL8));
	mtrl->Diffuse.a=1;
	mtrl->Diffuse.r=1;
	mtrl->Diffuse.g=1;
	mtrl->Diffuse.b=1;
	mtrl->Diffuse.a=(((color>>24)&255)/255.0f)*density;
	mtrl->Diffuse.r=(((color>>16)&255)/255.0f)*density;
	mtrl->Diffuse.g=(((color>>8)&255)/255.0f)*density;
	mtrl->Diffuse.b=(((color>>0)&255)/255.0f)*density;

	mtrl->Ambient.a=(((color>>24)&255)/255.0f)*density;
	static float temp_f = 412;
	mtrl->Ambient.r=(((color>>16)&255)/temp_f)*density;
	mtrl->Ambient.g=(((color>>8)&255)/temp_f)*density;
	mtrl->Ambient.b=(((color>>0)&255)/temp_f)*density;

//	D3DXVec3Normalize( (D3DXVECTOR3*)&light->Direction, &vecDir );
//	static Vector3f rgb={0,0,0};
//	Vector3f temp;
//	Vector3fCopy(stMainDirLight,temp);
//	Normalize(temp);
//	memcpy(&light->Direction.x,temp,sizeof(D3DXVECTOR3)*3);
	memcpy(&light->Direction.x,GetR3State()->mMainLightNomal,sizeof(D3DXVECTOR3));
}


void GetHeadMatrix(D3DXMATRIX *get_mat,R3Camera *camera,float yangle,LONG mouse_x,LONG mouse_y)
{
	D3DXMATRIX eye2;
	_R3ENGINE_STATE *state = GetR3State();

	float posx = min(state->mScreenXsize,mouse_x );
	float posy = min(state->mScreenYsize,mouse_y );
	float halfw = state->mScreenXsize/2;
	float halfh = state->mScreenYsize/2;
//	halfw = halfw;
//	halfh = halfh;
	float rotx= ((posx - halfw)/halfw)*40.f;
	float roty= ((posy - halfh)/halfh)*20.f;
	D3DXMatrixIdentity(&eye2);
	static float x = 0;
	static bool  inc = true;

	if (x>90) inc = false;
	else if(x<=-90) inc = true;
	if (inc) x+=0.1f;
	else x-= 0.1f;

	rotx = (float)cos(D3DXToRadian( camera->GetPh()+yangle ))*(rotx);
	roty = (float)cos(D3DXToRadian( camera->GetTh() ))*roty;
	D3DXMatrixRotationX(get_mat,D3DXToRadian(-rotx));
	D3DXMatrixRotationZ(&eye2,D3DXToRadian(roty));
//				D3DXMatrixRotationX(&EyeRot,D3DXToRadian(CameraTest.GetPh()+stPlayer.mYangle));
//				D3DXMatrixRotationZ(&eye2,D3DXToRadian(CameraTest.GetTh()));
	D3DXMatrixMultiply(get_mat,&eye2,get_mat);
}

//활경우 기준벡터는 {0,0,-1}맥스에서 프론트의경우.
//피라든지 그런거는 {0,1,0}맥스에서 top의경우 
void GetMatrixFromVector(float get_mat[4][4],Vector3f vec,Vector3f axis)
{
	Vector4f quat;

	GetQuaternionFromVector(quat,vec,axis);
	MatrixFromQuaternion(get_mat,quat[0],quat[1],quat[2],quat[3]);
	
	//워쩐일인지 뒤집혔다.
//	float t_mat[4][4];
//	MatrixRotate(t_mat,0,180,0);
//	MatrixMultiply(get_mat,get_mat,t_mat);
}


/*
void GetMatrixFromVector(float get_mat[4][4],Vector3f vec,Vector3f axis)
{
	Vector3f temp;
	
	//워쩐일인지 뒤집혔다.
//	float t_mat[4][4];
//	MatrixRotate(t_mat,0,180,0);
//	MatrixMultiply(get_mat,get_mat,t_mat);
	Vector3fCopy(vec,temp);
	temp[1]=0;
	Normalize(temp);
	float yangle = acos(temp[2]);

	Vector3fCopy(vec,temp);
	temp[0]=0;
	Normalize(temp);
	float xangle = acos(temp[1]);

	MatrixRotate(get_mat,xangle,yangle,0);
}
*/
// A 와 B를 잇는 선분의 A로부터 dist만큼 떨어진 좌표를 얻는 함수다.
void GetPosByDistFromATOB(Vector3f *get,Vector3f src,Vector3f tar,float dist)
{
	Vector3f temp;

	Vector3fSub(tar,src,temp);
	Normalize(temp);
	Vector3fMul(temp,dist,temp);
	Vector3fAdd(src,temp,get[0]);
}

BOOL IsParticle(char *name)
{
	strlwr(name);
	int leng = strlen(name);
	if( name[leng-1] == 't'
	&& name[leng-2] == 'p' 
	&& name[leng-3] == 's' )
		return TRUE;
	return FALSE;
}

//--------------------------------스카이 페이드관련.

DWORD GetColorToColorAlpha(DWORD src_color,DWORD dest_color,float alpha)
{
	DWORD r,g,b;
	float s_r = (float)((src_color>>16)&0xff);
	float d_r = (float)((dest_color>>16)&0xff);
	float s_g = (float)((src_color>>8)&0xff);
	float d_g = (float)((dest_color>>8)&0xff);
	float s_b = (float)((src_color)&0xff);
	float d_b = (float)((dest_color)&0xff);
	r = (DWORD)((d_r-s_r)*alpha+s_r);
	g = (DWORD)((d_g-s_g)*alpha+s_g);
	b = (DWORD)((d_b-s_b)*alpha+s_b);

	return 0xff000000|(r<<16)|(g<<8)|(b);
}


//#define _FADE_IN_OUT_TIME	0.3f
#define _FADE_IN_OUT_TIME	0.6f
DWORD stSkyFadeColor;
float stSkyFadeTime,stSkyStartTick,stAllSkyFadeTime;
float stSkyFadeAlpha=0;
BOOL stIsFadeInStart=0;

void SetFadeSky(DWORD color,float fade_time)		//해당되는 컬러와 페이드되는 시간.
{
	stSkyFadeColor = color;
	stSkyFadeTime = -_FADE_IN_OUT_TIME-0.0001f;
	stSkyStartTick = R3GetTime();
	stAllSkyFadeTime = fade_time;
	stSkyFadeAlpha=0.0000001f;
	stIsFadeInStart = 0;
}
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

//::GetFogColor()
BOOL DrawFadeSKy()
{
	if( stSkyFadeAlpha== 0)	//페이드 아웃된 상태다 
		return TRUE;

	stAllSkyFadeTime -= R3GetLoopTime();
	if( stSkyFadeTime > 0 )	//페이드인.
	{
		float tick = (R3GetTime()-stSkyStartTick+0.0001f)/stSkyFadeTime;
		if( tick > 1 )//1넘어가면 끝.
		{
			tick=1;
			stSkyFadeAlpha=0;
			stSkyFadeTime=0;
		}
		else
			stSkyFadeAlpha = 1-tick;
	}
	else
	if( stSkyFadeTime < 0 )	//페이드 아웃.
	{
		float tick = (R3GetTime()-stSkyStartTick+0.0001f)/-stSkyFadeTime;
		if( tick > 1 )//1넘어가면 끝.
		{
			tick=1;
			stSkyFadeAlpha=1;
			stSkyFadeTime=0;
		}
		else
			stSkyFadeAlpha = tick;
	}
	else	//페이드 인이나 페이드 아웃된 상태...
	{
	}
	if( !stIsFadeInStart && stAllSkyFadeTime < _FADE_IN_OUT_TIME ) //페이드 아웃 시작.밝아지기
	{
		stIsFadeInStart = TRUE;//여기에 안들어오게 조작하자..
		stSkyFadeTime = _FADE_IN_OUT_TIME+0.0001f;
		stSkyStartTick = R3GetTime();
	}
	_R3ENGINE_STATE *state= GetR3State();

	SetFadeFactor(stSkyFadeColor,stSkyFadeAlpha);	//알파가 1에 가까울수록 해당 페이드 색이 강하다.
	DWORD color=GetColorToColorAlpha(GetFogColor(),stSkyFadeColor,stSkyFadeAlpha);
	GetD3dDevice()->SetRenderState( D3DRS_FOGCOLOR, color );

//	float f_start = (state->mFogStart/3.0f-state->mFogStart)*stSkyFadeAlpha + state->mFogStart;
//	float f_end = (state->mFogEnd/3.0f-state->mFogEnd)*stSkyFadeAlpha + state->mFogEnd;
	float f_start = (6-state->mFogStart)*stSkyFadeAlpha + state->mFogStart;
	float f_end = (600-state->mFogEnd)*stSkyFadeAlpha + state->mFogEnd;
	GetD3dDevice()->SetRenderState( D3DRS_FOGSTART,   FtoDW(f_start));
	GetD3dDevice()->SetRenderState( D3DRS_FOGEND,     FtoDW(f_end) );

	return FALSE;
}
//------------------- 텍스쳐 페이드
DWORD stTexFadeColor;
float stTexFadeTime,stTexStartTick,stAllTexFadeTime;
float stTexFadeAlpha=0;
BOOL stIsTexFadeInStart=0;
void *pTexFace;

void SetFadeTex(DWORD color,float fade_time,void *tex)		//해당되는 컬러와 페이드되는 시간.
{
	stTexFadeColor = color;
	stTexFadeTime = -_FADE_IN_OUT_TIME-0.0001f;
	stTexStartTick = R3GetTime();
	stAllTexFadeTime = fade_time;
	stTexFadeAlpha=0.0000001f;
	stIsTexFadeInStart = 0;
	pTexFace=tex;
}

//::GetFogColor()
BOOL DrawFadeTex(void)
{
	if( stTexFadeAlpha== 0)	//페이드 아웃된 상태다 
		return TRUE;

	stAllTexFadeTime -= R3GetLoopTime();
	if( stTexFadeTime > 0 )	//페이드인.
	{
		float tick = (R3GetTime()-stTexStartTick+0.0001f)/stTexFadeTime;
		if( tick > 1 )//1넘어가면 끝.
		{
			tick=1;
			stTexFadeAlpha=0;
			stTexFadeTime=0;
		}
		else
			stTexFadeAlpha = 1-tick;
	}
	else
	if( stTexFadeTime < 0 )	//페이드 아웃.
	{
		float tick = (R3GetTime()-stTexStartTick+0.0001f)/-stTexFadeTime;
		if( tick > 1 )//1넘어가면 끝.
		{
			tick=1;
			stTexFadeAlpha=1;
			stTexFadeTime=0;
		}
		else
			stTexFadeAlpha = tick;
	}
	else	//페이드 인이나 페이드 아웃된 상태...
	{
	}
	if( !stIsTexFadeInStart && stAllTexFadeTime < _FADE_IN_OUT_TIME ) //페이드 아웃 시작.밝아지기
	{
		stIsTexFadeInStart = TRUE;//여기에 안들어오게 조작하자..
		stTexFadeTime = _FADE_IN_OUT_TIME+0.0001f;
		stTexStartTick = R3GetTime();
	}
	SetFadeFactor(stTexFadeColor,stTexFadeAlpha*.5f ,pTexFace);	//알파가 1에 가까울수록 해당 페이드 색이 강하다.

	return FALSE;
}



#define _RENDER_DIST (840-40)
#define _MAX_HEAR_DIST (_RENDER_DIST/4.0f)


float GetDensityFromPos(Vector3f pos)
{
	Vector3f cam_pos,temp;

	::GetCameraPos(&cam_pos);
	Vector3fSub(pos,cam_pos,temp);
	float dist = Vector3fDist(temp);	//몬스터와의 거리.
	float density = max((dist-_MAX_HEAR_DIST),0);
	density = 1-density/(_RENDER_DIST-_MAX_HEAR_DIST);
	density = max(density,0);

	return density;
}




// 테스트할 박스 가로 세로 높이
void DrawTestBox(float hor,float ver,float height,DWORD color)
{
	_D3DR3VERTEX_TEX0 vertex[100];

	Vector3f v[8];	//박스의 8개좌표...
	v[0][0] = -hor/2;	v[0][1] = height;	v[0][2] = -ver/2;
	v[1][0] = -hor/2;	v[1][1] = height;	v[1][2] = ver/2;
	v[2][0] = hor/2;	v[2][1] = height;	v[2][2] = ver/2;
	v[3][0] = hor/2;	v[3][1] = height;	v[3][2] = -ver/2;
	v[4][0] = -hor/2;	v[4][1] = 0;	v[4][2] = -ver/2;
	v[5][0] = -hor/2;	v[5][1] = 0;	v[5][2] = ver/2;
	v[6][0] = hor/2;	v[6][1] = 0;	v[6][2] = ver/2;
	v[7][0] = hor/2;	v[7][1] = 0;	v[7][2] = -ver/2;

	int cnt=0;
#define color_dan(dan) ( (((r*dan)/8)<<16)|(((g*dan)/8)<<8)|(((b*dan)/8)<<0)|0xff000000);
	int r=(color&0x00ff0000)>>16;
	int g=(color&0x0000ff00)>>8;
	int b=(color&0x000000ff)>>0;

	//위 
	color = color_dan(8);
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[0],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[1],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[2],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[0],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[2],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[3],sizeof(Vector3f));

	//아래
	color = color_dan(2);
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[4],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[6],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[5],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[4],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[7],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[6],sizeof(Vector3f));

	// 주변..
	color = color_dan(4);
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[1],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[5],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[2],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[5],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[6],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[2],sizeof(Vector3f));

	color = color_dan(5);
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[2],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[6],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[7],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[2],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[7],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[3],sizeof(Vector3f));

	color = color_dan(6);
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[3],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[7],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[4],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[3],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[4],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[0],sizeof(Vector3f));

	color = color_dan(7);
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[0],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[4],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[5],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[0],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[5],sizeof(Vector3f));
	vertex[cnt].color = color;
	memcpy(&vertex[cnt++],v[1],sizeof(Vector3f));

	DrawPrimitiveUPPoly(cnt/3,vertex);
}


void CleanZbuffer(float sx,float sy,float w,float h)
{
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_GREATEREQUAL);
	Draw2DSpriteSub(sx,sy,w,h,NULL,0x00000000);
	pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
}

extern HWND GetNowHWnd();
void FramebufferToBMP(char *name)
{
	HRESULT hr;
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();
    LPDIRECT3DSURFACE8 psurface;
    LPDIRECT3DSURFACE8 backbuf;

	_R3ENGINE_STATE *state = GetR3State();

	HWND hwnd = GetNowHWnd();

	RECT rcWindowBounds,rcWindowClient,rect;

    GetWindowRect( hwnd, &rcWindowBounds );
    GetClientRect( hwnd, &rcWindowClient );

//    WINDOWPLACEMENT wndpl;
//	GetWindowPlacement( hwnd, &wndpl);

    DWORD BackBufferWidth  = rcWindowClient.right - rcWindowClient.left;
    DWORD BackBufferHeight = rcWindowClient.bottom - rcWindowClient.top;
	D3DDISPLAYMODE mode;
	pd3dDevice->GetDisplayMode(&mode);

    if( FAILED( hr = pd3dDevice->CreateImageSurface( mode.Width, mode.Height, 
        D3DFMT_A8R8G8B8, &psurface ) ) )
    {
        return;
    }

	pd3dDevice->GetRenderTarget(&backbuf);
	if( D3D_OK !=  pd3dDevice->GetFrontBuffer(psurface) )
		return;
//	pd3dDevice->Present( NULL, NULL, NULL, NULL ) ;

	D3DSURFACE_DESC Desc;
	backbuf->GetDesc(&Desc);
	backbuf->Release();
	DWORD slice = (rcWindowBounds.right-rcWindowBounds.left-BackBufferWidth)/2;
	rect.left = slice+rcWindowBounds.left;
	rect.right = slice+rcWindowBounds.left+BackBufferWidth;
	rect.bottom = rcWindowBounds.bottom-slice;
	rect.top = rcWindowBounds.bottom-slice-BackBufferHeight;
	//--lock
	D3DLOCKED_RECT lockedRect;
	if( D3D_OK == psurface->LockRect(&lockedRect,&rect,0) )
	{
		SaveBMP(name,Desc.Width,Desc.Height,lockedRect.Pitch,(BYTE*)lockedRect.pBits);
		psurface->UnlockRect();
	}
	psurface->Release();
}
void FramebufferToJPG(char *name)
{
	HRESULT hr;
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();
    LPDIRECT3DSURFACE8 psurface;
    LPDIRECT3DSURFACE8 backbuf;

	_R3ENGINE_STATE *state = GetR3State();

	HWND hwnd = GetNowHWnd();

	RECT rcWindowBounds,rcWindowClient,rect;

    GetWindowRect( hwnd, &rcWindowBounds );
    GetClientRect( hwnd, &rcWindowClient );

//    WINDOWPLACEMENT wndpl;
//	GetWindowPlacement( hwnd, &wndpl);

    DWORD BackBufferWidth  = rcWindowClient.right - rcWindowClient.left;
    DWORD BackBufferHeight = rcWindowClient.bottom - rcWindowClient.top;
	D3DDISPLAYMODE mode;
	pd3dDevice->GetDisplayMode(&mode);

    if( FAILED( hr = pd3dDevice->CreateImageSurface( mode.Width, mode.Height, 
        D3DFMT_A8R8G8B8, &psurface ) ) )
    {
        return;
    }

	pd3dDevice->GetRenderTarget(&backbuf);
	if( D3D_OK !=  pd3dDevice->GetFrontBuffer(psurface) )
		return;
//	pd3dDevice->Present( NULL, NULL, NULL, NULL ) ;

	D3DSURFACE_DESC Desc;
	backbuf->GetDesc(&Desc);
	backbuf->Release();
	DWORD slice = (rcWindowBounds.right-rcWindowBounds.left-BackBufferWidth)/2;
	rect.left = slice+rcWindowBounds.left;
	rect.right = slice+rcWindowBounds.left+BackBufferWidth;
	rect.bottom = rcWindowBounds.bottom-slice;
	rect.top = rcWindowBounds.bottom-slice-BackBufferHeight;
	//--lock
	D3DLOCKED_RECT lockedRect;
	if( D3D_OK == psurface->LockRect(&lockedRect,&rect,0) )
	{
//		SaveBMP(name,Desc.Width,Desc.Height,lockedRect.Pitch,(BYTE*)lockedRect.pBits);
		SaveJPG(name,Desc.Width,Desc.Height,lockedRect.Pitch,(BYTE*)lockedRect.pBits);
		psurface->UnlockRect();
	}
	psurface->Release();
}
DWORD PixelFromFramebuffer(short cl_x,short cl_y)
{
	HRESULT hr;
	LPDIRECT3DDEVICE8 pd3dDevice=GetD3dDevice();
    LPDIRECT3DSURFACE8 psurface;
	DWORD color;

	_R3ENGINE_STATE *state = GetR3State();

	HWND hwnd = GetNowHWnd();

	RECT rcWindowBounds,rcWindowClient,rect;

    GetWindowRect( hwnd, &rcWindowBounds );
    GetClientRect( hwnd, &rcWindowClient );
	DWORD slice = (rcWindowBounds.right-rcWindowBounds.left-(rcWindowClient.right - rcWindowClient.left))/2;
	rect.left = slice+rcWindowBounds.left;
	rect.top = rcWindowBounds.bottom-slice-rcWindowClient.bottom - rcWindowClient.top;
	cl_x += (short)rect.left;
	cl_y += (short)rect.top;

    DWORD BackBufferWidth =GetSystemMetrics(SM_CXSCREEN);
    DWORD BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);

	D3DDISPLAYMODE mode;
	pd3dDevice->GetDisplayMode(&mode);

    if( FAILED( hr = pd3dDevice->CreateImageSurface( BackBufferWidth, BackBufferHeight, 
        D3DFMT_A8R8G8B8, &psurface ) ) )
    {
        return 0;
    }
	if( D3D_OK !=  pd3dDevice->GetFrontBuffer(psurface) )
		return 0;

	rect.left = 0;
	rect.right = BackBufferWidth;
	rect.bottom = BackBufferHeight;
	rect.top = 0;
	D3DLOCKED_RECT lockedRect;
	cl_x--;
	cl_y--;
	if( D3D_OK == psurface->LockRect(&lockedRect,&rect,0) )
	{
		color = ((DWORD *)lockedRect.pBits)[(lockedRect.Pitch/4)*cl_y+cl_x];

//		SaveBMP(name,Desc.Width,Desc.Height,lockedRect.Pitch,(BYTE*)lockedRect.pBits);
		psurface->UnlockRect();
	}
	psurface->Release();

	return color;
}


/*
//------------------- 엔티티 페이드
DWORD stEntityFadeColor;
float stEntityFadeTime,stEntityStartTick,stAllEntityFadeTime;
float stEntityFadeAlpha=0;
BOOL stIsEntityFadeInStart=0;
void *pEntityFace;

void SetFadeEntity(DWORD color,float fade_time,void *tex)		//해당되는 컬러와 페이드되는 시간.
{
	stEntityFadeColor = color;
	stEntityFadeTime = -_FADE_IN_OUT_TIME-0.0001f;
	stEntityStartTick = R3GetTime();
	stAllEntityFadeTime = fade_time;
	stEntityFadeAlpha=0.0000001f;
	stIsEntityFadeInStart = 0;
	pEntityFace=Entity;
}

//::GetFogColor()
DWORD CalcFadeEntity(void)
{
	if( stEntityFadeAlpha== 0)	//페이드 아웃된 상태다 
		return 0;

	stAllEntityFadeTime -= R3GetLoopTime();
	if( stEntityFadeTime > 0 )	//페이드인.
	{
		float tick = (R3GetTime()-stEntityStartTick+0.0001f)/stEntityFadeTime;
		if( tick > 1 )//1넘어가면 끝.
		{
			tick=1;
			stEntityFadeAlpha=0;
			stEntityFadeTime=0;
		}
		else
			stEntityFadeAlpha = 1-tick;
	}
	else
	if( stEntityFadeTime < 0 )	//페이드 아웃.
	{
		float tick = (R3GetTime()-stEntityStartTick+0.0001f)/-stEntityFadeTime;
		if( tick > 1 )//1넘어가면 끝.
		{
			tick=1;
			stEntityFadeAlpha=1;
			stEntityFadeTime=0;
		}
		else
			stEntityFadeAlpha = tick;
	}
	else	//페이드 인이나 페이드 아웃된 상태...
	{
	}
	if( !stIsEntityFadeInStart && stAllEntityFadeTime < _FADE_IN_OUT_TIME ) //페이드 아웃 시작.밝아지기
	{
		stIsEntityFadeInStart = TRUE;//여기에 안들어오게 조작하자..
		stEntityFadeTime = _FADE_IN_OUT_TIME+0.0001f;
		stEntityStartTick = R3GetTime();
	}
//	SetFadeFactor(stEntityFadeColor,stEntityFadeAlpha*.5f ,pEntityFace);	//알파가 1에 가까울수록 해당 페이드 색이 강하다.
	DWORD color = (color&0x00ffffff)|((DWORD)(alpha*255)<<24);

	return color;
}
*/


//역 트랜스폼 trans_pos[0]은 스크린x, trans[1]은 스크린y, trans[2] 희망 z좌표
void GetInverseTransformVertex( float get[3],float trans_pos[3])
{
	Vector3f n,v;
	_R3ENGINE_STATE *state = GetR3State();

    n[0] =  ( ( ( 2.0f * trans_pos[0] ) / (state->mViewXL) ) - 1.0f ) / state->mMatProj._11;
    n[1] = -( ( ( 2.0f * trans_pos[1] ) / (state->mViewYL) ) - 1.0f) / state->mMatProj._22;
    n[2] =  1.0f;	//현재 왼손 좌표계라...

	v[0] = n[0]*trans_pos[2];		//숫자가 커야 오차가 없어진다????
	v[1] = n[1]*trans_pos[2];
	v[2] = n[2]*trans_pos[2];

    // Get the inverse view matrix
    D3DXMATRIX *m = &state->mInvMatView;

    // Transform the screen space pick ray into 3D space
    get[0]  = v[0]*m->_11 + v[1]*m->_21 + v[2]*m->_31 + m->_41;
    get[1]  = v[0]*m->_12 + v[1]*m->_22 + v[2]*m->_32 + m->_42;
    get[2]  = v[0]*m->_13 + v[1]*m->_23 + v[2]*m->_33 + m->_43;
}


/*
//---------------------블로링을 위한 버텍스 버퍼들.
CVertexBuffer stBlurVertexBuffer[5];		//1,3,5,7,9
CIndexBuffer stBlurIndexBuffer;			//이거 가장큰걸루 같이 써도된다.
void CreateBlurVBuffer(DWORD width,DWORD height)
{
	DWORD level[5]={1,3,5,7,9};
	LONG i,j,k;

	float u[2]={0,1};
	float v[2]={0,1};
	DWORD w=width;
	DWORD h=height;
	for(k=0; k<5; k++)
	{
		stBlurVertexBuffer[k].InitVertexBuffer( 4*level[k]*level[k] * sizeof(_D3DR3TLVERTEX_TEX1)
			,_USE_STATIC_VP,D3DFVF_R3TLVERTEX_TEX1 );
		BYTE *vbuf = stBlurVertexBuffer[k].VPLock();
		_D3DR3TLVERTEX_TEX1 *vert=(_D3DR3TLVERTEX_TEX1 *)vbuf;

		int start = level[k]/2;
		DWORD color = ((DWORD)((1/(float)(level[k]*level[k]))*255));
		color = 0xff000000|(color<<16)|(color<<8)|(color<<0);
		DWORD cnt=0;

#define SetVert(aa,bb,cc,dd,color,uu,vv)	{	vert[cnt].x=aa;	vert[cnt].y=bb;	vert[cnt].z=cc; vert[cnt].w=dd; vert[cnt].color=color; vert[cnt].u=uu; vert[cnt].u=vv; };
		for( i=-start; i<start+1; i++)
		{
			for( j=-start; j<start+1; j++)
			{
				//DrawRect(i,j,stRTest.GetXSize(),stRTest.GetYSize(),u,v,tex,color);
				SetVert(i + 0 -0.5f,j - 0 -0.5f,0.1f,0.5f,color,u[0],v[0]);
				cnt++;
				SetVert(i + w -0.5f,j - 0 -0.5f,0.1f,0.5f,color,u[1],v[0]);
				cnt++;
				SetVert(i + w -0.5f,j + h -0.5f,0.1f,0.5f,color,u[1],v[1]);
				cnt++;
				SetVert(i + 0 -0.5f,j + h -0.5f,0.1f,0.5f,color,u[0],v[1]);
				cnt++;
			}
		}

		stBlurVertexBuffer[k].VPUnLock();
	}
	stBlurIndexBuffer.InitIndexBuffer(6*level[4]*level[4]*sizeof(WORD));
	WORD *index = (WORD *)stBlurIndexBuffer.VPLock();
	DWORD cnt=0;
	WORD s_cnt=0;
	for( i=0; i<level[4]; i++)
	{
		for( j=0; j<level[4]; j++)
		{
			index[cnt++]=s_cnt;
			index[cnt++]=s_cnt+1;
			index[cnt++]=s_cnt+2;
			index[cnt++]=s_cnt;
			index[cnt++]=s_cnt+2;
			index[cnt++]=s_cnt+3;
			s_cnt+=4;
		}
	}
	stBlurIndexBuffer.VPUnLock();
}

void DrawBlurVBuffer(DWORD level)
{
	DWORD level_table[5]={1,3,5,7,9};
	DWORD id = level/2;
	GetD3dDevice()->SetVertexShader( D3DFVF_R3TLVERTEX_TEX1 );
    GetD3dDevice()->SetStreamSource( 0, stBlurVertexBuffer[id].m_lpVertexBuffer, sizeof(_D3DR3TLVERTEX_TEX1) );
    GetD3dDevice()->SetIndices( stBlurIndexBuffer.m_lpIndexBuffer, 0 );
    GetD3dDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
		0, 4*level_table[id]*level_table[id],
        0, 2*level_table[id]*level_table[id] );
}

void ReleaseBlurVBuffer()
{
	DWORD k;

	for(k=0; k<5; k++)
		stBlurVertexBuffer[k].ReleaseVertexBuffer();
	stBlurIndexBuffer.ReleaseIndexBuffer();
}
*/
void CreateBlurVBuffer(DWORD width,DWORD height)
{
}
void DrawBlurVBuffer(DWORD level)
{
}
void ReleaseBlurVBuffer()
{
}
