#include "EffectUtil.h"
#include "EffectCore.h"
#include "CMagicScript.h"
#include "EffectEntityList.h"
#include "MonsterSpecialEffect.h"
#include "EffectUtilFont.h"
#include "R3D3dTex.h"

#define _FONT_XLENG 20
#define _FONT_YLENG 32

#define _TIME_FONT_XLENG 28
#define _TIME_FONT_YLENG 38

#define _KILL_FONT_XLENG 44
#define _KILL_FONT_YLENG 66

#define _START_KILL_Y	166
#define _START_TIME_Y	128

extern LPDIRECT3DTEXTURE8 R3LoadDDSAndTextureMem(char *name);
static LPDIRECT3DTEXTURE8 st_FontTexture=NULL;
extern void DrawPrimitiveUPTex1(int num,void *vbuffer);


void EFUtilFontRestoreDeviceObjects()
{
	if( st_FontTexture )
		EFUtilFontInvalidateDeviceObjects();
	st_FontTexture = R3LoadDDSAndTextureMem(".\\Effect\\effectfont.dds");

}
void EFUtilFontInvalidateDeviceObjects()
{
	if( st_FontTexture )
	{
		st_FontTexture->Release();
		st_FontTexture=NULL;
	}
}

DWORD stVbufCnt;

void BeginDrawEffectFont()
{
	stVbufCnt=0;
    GetD3dDevice()->SetTexture(0,st_FontTexture);

	_R3ENGINE_STATE *state = GetR3State();
	float x_center = (float)(state->mViewXL/2+state->mViewSx);
	float y_center = (float)(state->mViewYL/2+state->mViewSy);
	SetTransformClipInfo((float)state->mViewXL,(float)state->mViewYL,x_center ,y_center );
}

void DrawEffectFont(_D3DR3TLVERTEX_TEX1 *v_buf)
{
	if( stVbufCnt == 0 )
		return;

	if( GetR3State()->mIsFog )
		GetD3dDevice()->SetRenderState( D3DRS_FOGENABLE, FALSE );
	MultiTexOff();
//	BlendOn(_BLEND_OPPA);
	BlendOn(_BLEND_BRIGHT);
//	BlendOn(_BLEND_OPPA);

	GetD3dDevice()->SetRenderState(D3DRS_ZWRITEENABLE ,FALSE);

	GetD3dDevice()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	GetD3dDevice()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	GetD3dDevice()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	GetD3dDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	GetD3dDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	GetD3dDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
//#define D3DFVF_R3TLVERTEX_TEX1 (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
	GetD3dDevice()->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1 );
	GetD3dDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,stVbufCnt/3,(void*)(&v_buf[0]),sizeof(_D3DR3TLVERTEX_TEX1));
	stVbufCnt=0;
	BlendOff();

	GetD3dDevice()->SetRenderState(D3DRS_ZWRITEENABLE ,TRUE);
	if( GetR3State()->mIsFog )
		GetD3dDevice()->SetRenderState( D3DRS_FOGENABLE, TRUE );
}

void PushEffectFont(Vector4f pos,char *ch,float scale,DWORD style,_D3DR3TLVERTEX_TEX1 v_buf[],DWORD color)
{
	if( ch==NULL || ch[0] == NULL )
		return;

	int i,st_length;
	_R3ENGINE_STATE *state = GetR3State();

	st_length = strlen(ch);
/*
	//트랜스폼 정보를 직접...
	float x_center = (float)(state->mViewXL/2+state->mViewSx);
	float y_center = (float)(state->mViewYL/2+state->mViewSy);
	SetTransformClipInfo((float)state->mViewXL,(float)state->mViewYL,x_center ,y_center );
*/
	//_FONT_XLENG
#define SetVBuf(cnt,a,b,c,d,e,f,g)	{v_buf[cnt].x = (a); 	v_buf[cnt].y = (b);	v_buf[cnt].z = (c);	v_buf[cnt].w = (d);	v_buf[cnt].u = (e);	v_buf[cnt].v = (f);	v_buf[cnt].color = (g);}

	float f_xdiff = _FONT_XLENG/512.f;
	float f_ydiff = _FONT_YLENG/256.f;
	if( atoi(ch) )
	{
		float base_v=f_ydiff*style;	
		for(i=0; i<st_length; i++)
		{
			DWORD base= ch[i]-'0';
	//		v_buf[cnt]._FONT_XLENG;
	//		SetVBuf(stVbufCnt++,pos[0]+_FONT_XLENG*scale*i,pos[1],pos[2],pos[3]);

			SetVBuf(stVbufCnt,pos[0]+_FONT_XLENG*scale*i,pos[1],pos[2],pos[3],base*f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_FONT_XLENG*scale*(i+1),pos[1],pos[2],pos[3],base*(f_xdiff)+f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_FONT_XLENG*scale*i,pos[1]+_FONT_YLENG*scale,pos[2],pos[3],base*f_xdiff,base_v+f_ydiff,color);
			stVbufCnt++;

			SetVBuf(stVbufCnt,pos[0]+_FONT_XLENG*scale*i,pos[1]+_FONT_YLENG*scale,pos[2],pos[3],base*f_xdiff,base_v+f_ydiff,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_FONT_XLENG*scale*(i+1),pos[1],pos[2],pos[3],base*(f_xdiff)+f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_FONT_XLENG*scale*(i+1),pos[1]+_FONT_YLENG*scale,pos[2],pos[3],base*(f_xdiff)+f_xdiff,base_v+f_ydiff,color);
			stVbufCnt++;

			if(stVbufCnt > 1024+512 )	//넘치는거 방지..
			{
				DrawEffectFont(v_buf);
				stVbufCnt=0;
			}
		}	
	}
	else
	{
		if( !strcmp("MISS",ch) )
		{
			#define _MISS_XL 72	
			f_xdiff = _MISS_XL/512.f;
			float base_v=f_ydiff*2;	//미스는 아래 두칸에 있따.
			SetVBuf(stVbufCnt,pos[0]+0,pos[1],pos[2],pos[3],0,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_MISS_XL*scale,pos[1],pos[2],pos[3],f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0],pos[1]+_FONT_YLENG*scale,pos[2],pos[3],0,base_v+f_ydiff,color);
			stVbufCnt++;

			SetVBuf(stVbufCnt,pos[0],pos[1]+_FONT_YLENG*scale,pos[2],pos[3],0,base_v+f_ydiff,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_MISS_XL*scale,pos[1],pos[2],pos[3],f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_MISS_XL*scale,pos[1]+_FONT_YLENG*scale,pos[2],pos[3],f_xdiff,base_v+f_ydiff,color);
			stVbufCnt++;
		}
		else
		if( !strcmp("FAILURE",ch) )
		{
			#define _FAILURE_XL 98
			f_xdiff = _FAILURE_XL/512.f;
			float base_v=f_ydiff*3;	//미스는 아래 세칸에 있따.
			SetVBuf(stVbufCnt,pos[0]+0,pos[1],pos[2],pos[3],0,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_FAILURE_XL*scale,pos[1],pos[2],pos[3],f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0],pos[1]+_FONT_YLENG*scale,pos[2],pos[3],0,base_v+f_ydiff,color);
			stVbufCnt++;

			SetVBuf(stVbufCnt,pos[0],pos[1]+_FONT_YLENG*scale,pos[2],pos[3],0,base_v+f_ydiff,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_FAILURE_XL*scale,pos[1],pos[2],pos[3],f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_FAILURE_XL*scale,pos[1]+_FONT_YLENG*scale,pos[2],pos[3],f_xdiff,base_v+f_ydiff,color);
			stVbufCnt++;
		}
		if(stVbufCnt > 1024+512 )	//넘치는거 방지..
		{
			DrawEffectFont(v_buf);
			stVbufCnt=0;
		}
	}
}


//이건 시간 폰트.
void PushEffectTimeFont(Vector4f pos,char *ch,float scale,DWORD style,_D3DR3TLVERTEX_TEX1 v_buf[],DWORD color)
{
	if( ch==NULL || ch[0] == NULL )
		return;

	int i,st_length;
	_R3ENGINE_STATE *state = GetR3State();

	st_length = strlen(ch);
#define SetVBuf(cnt,a,b,c,d,e,f,g)	{v_buf[cnt].x = (a); 	v_buf[cnt].y = (b);	v_buf[cnt].z = (c);	v_buf[cnt].w = (d);	v_buf[cnt].u = (e);	v_buf[cnt].v = (f);	v_buf[cnt].color = (g);}

	float f_xdiff = _TIME_FONT_XLENG/512.f;
	float f_ydiff = _TIME_FONT_YLENG/256.f;
//	if( atoi(ch) )
	{
		float base_v=_START_TIME_Y/256.0f;	
		for(i=0; i<st_length; i++)
		{
			DWORD base= ch[i]-'0';
			if( ch[i]==':' )
				base = 10;

			SetVBuf(stVbufCnt,pos[0]+_TIME_FONT_XLENG*scale*i,pos[1],pos[2],pos[3],base*f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_TIME_FONT_XLENG*scale*(i+1),pos[1],pos[2],pos[3],base*(f_xdiff)+f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_TIME_FONT_XLENG*scale*i,pos[1]+_TIME_FONT_YLENG*scale,pos[2],pos[3],base*f_xdiff,base_v+f_ydiff,color);
			stVbufCnt++;

			SetVBuf(stVbufCnt,pos[0]+_TIME_FONT_XLENG*scale*i,pos[1]+_TIME_FONT_YLENG*scale,pos[2],pos[3],base*f_xdiff,base_v+f_ydiff,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_TIME_FONT_XLENG*scale*(i+1),pos[1],pos[2],pos[3],base*(f_xdiff)+f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_TIME_FONT_XLENG*scale*(i+1),pos[1]+_TIME_FONT_YLENG*scale,pos[2],pos[3],base*(f_xdiff)+f_xdiff,base_v+f_ydiff,color);
			stVbufCnt++;

			if(stVbufCnt > 1024+512 )	//넘치는거 방지..
			{
				DrawEffectFont(v_buf);
				stVbufCnt=0;
			}
		}	
	}
}

//이건  킬 폰트.
void PushEffectKillFont(Vector4f pos,char *ch,float scale,DWORD style,_D3DR3TLVERTEX_TEX1 v_buf[],DWORD color)
{
	if( ch==NULL || ch[0] == NULL )
		return;

	int i,st_length;
	_R3ENGINE_STATE *state = GetR3State();

	st_length = strlen(ch);
#define SetVBuf(cnt,a,b,c,d,e,f,g)	{v_buf[cnt].x = (a); 	v_buf[cnt].y = (b);	v_buf[cnt].z = (c);	v_buf[cnt].w = (d);	v_buf[cnt].u = (e);	v_buf[cnt].v = (f);	v_buf[cnt].color = (g);}

#define _KILL_FONT_NEXT_XL  13

	float f_xdiff = _KILL_FONT_XLENG/512.f;
	float f_ydiff = _KILL_FONT_YLENG/256.f;
//	if( atoi(ch) )
	{
		float base_v=_START_KILL_Y/256.0f;	
		for(i=0; i<st_length; i++)
		{
			if( ch[i]==' ' )
				continue;
			DWORD base= ch[i]-'0';

			SetVBuf(stVbufCnt,pos[0]+_KILL_FONT_XLENG*scale*i+i*-_KILL_FONT_NEXT_XL,pos[1],pos[2],pos[3],base*f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_KILL_FONT_XLENG*scale*(i+1)+i*-_KILL_FONT_NEXT_XL,pos[1],pos[2],pos[3],base*(f_xdiff)+f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_KILL_FONT_XLENG*scale*i+i*-_KILL_FONT_NEXT_XL,pos[1]+_KILL_FONT_YLENG*scale,pos[2],pos[3],base*f_xdiff,base_v+f_ydiff,color);
			stVbufCnt++;

			SetVBuf(stVbufCnt,pos[0]+_KILL_FONT_XLENG*scale*i+i*-_KILL_FONT_NEXT_XL,pos[1]+_KILL_FONT_YLENG*scale,pos[2],pos[3],base*f_xdiff,base_v+f_ydiff,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_KILL_FONT_XLENG*scale*(i+1)+i*-_KILL_FONT_NEXT_XL,pos[1],pos[2],pos[3],base*(f_xdiff)+f_xdiff,base_v,color);
			stVbufCnt++;
			SetVBuf(stVbufCnt,pos[0]+_KILL_FONT_XLENG*scale*(i+1)+i*-_KILL_FONT_NEXT_XL,pos[1]+_KILL_FONT_YLENG*scale,pos[2],pos[3],base*(f_xdiff)+f_xdiff,base_v+f_ydiff,color);
			stVbufCnt++;

			if(stVbufCnt > 1024+512 )	//넘치는거 방지..
			{
				DrawEffectFont(v_buf);
				stVbufCnt=0;
			}
		}	
	}
}

//---------------------- 스트링 아웃...
#define _MAX_STRING 15
#define _MAX_SIMULTANEOUS_STRING_OUT	48	//최대 출력글자 가지수.


typedef struct{
	BYTE mIsScroll;	//현재 올리는 중인가?
	float mTime;		//진행 카운트
	char mStr[_MAX_STRING];
	DWORD mStyle;		//문자 스타일...
	DWORD mColor;		//색상...
	Vector3f mPos;
	void UpDate();
}EFUTIL_STRING_OUT;

static DWORD stNowCnt;
static EFUTIL_STRING_OUT stStringOut[_MAX_SIMULTANEOUS_STRING_OUT];

void EFUtil_StartStringMoveOut(Vector3f pos,char *str,DWORD str_style,DWORD color)
{
	int i;

	if( strlen(str) >= _MAX_STRING-1 )
		Warning(str,"<-문자 올리는거 제한수자보다 많습니다.");

	DWORD old_gab=0;
	int oldest=0;
	for(i=0; i< _MAX_SIMULTANEOUS_STRING_OUT; i++)
	{
		if( stStringOut[i].mIsScroll == FALSE )		//빈 슬롯이다.
		{
			break;
		}
		if( stStringOut[i].mTime > old_gab) //만약 다 찰경우를 대비해서 가장 오래된거를 만들어놓는다..
		{
			old_gab=stStringOut[i].mTime;
			oldest=i;
		}
	}

	if( i == _MAX_SIMULTANEOUS_STRING_OUT )	//꽉 찬경우
		i = oldest;

	_strcpy(stStringOut[i].mStr,str,15);
	Vector3fCopy(pos,stStringOut[i].mPos);
	stStringOut[i].mTime=0;
	stStringOut[i].mColor = 0xffffffff;//color;
	stStringOut[i].mIsScroll=TRUE;
	stStringOut[i].mStyle=str_style;
}


//스트링 렌더링 
void EFUtil_RenderStringMoveOut()
{
	int i;
	Vector4f pos;
	Vector3f temp;
	DWORD color,alpha;
	_D3DR3TLVERTEX_TEX1 v_buf[2048];

	alpha = 255;
	float loop_time = R3GetLoopTime();
	_R3ENGINE_STATE *state = GetR3State();

	Vector3f add_pos={0,0,-5};	//좀 앞으로 땡긴다.

	BeginDrawEffectFont();

	DWORD old_outline_color=GetOutLineColor();
	SetOutLineColor(0xffffc8c8);

	for(i=0; i< _MAX_SIMULTANEOUS_STRING_OUT; i++)
	{
		if( !stStringOut[i].mIsScroll )		//빈 슬롯이다.
			continue;
		Vector3fCopy(stStringOut[i].mPos,temp);
		temp[1]+=stStringOut[i].mTime*20;		//스크롤되는 속도...

		stStringOut[i].mTime+=loop_time;
//		if( stStringOut[i].mTime > 10 )	//넌 그만 출력해라...
		if( stStringOut[i].mTime > 0.7)	//넌 그만 출력해라...
		{
			stStringOut[i].mIsScroll=FALSE;
			continue;
		}


		if( !TransformVertex( pos, state->mMatViewProj.m,temp,add_pos))
			continue;
		alpha = (DWORD)(min( 1 - stStringOut[i].mTime,1 )*255);
		pos[0]= (float)((LONG)(pos[0]+.5f))-( ( strlen( stStringOut[i].mStr) / 2 ) * _FONT_XLENG );
		pos[1]= (float)((LONG)(pos[1]))-_FONT_YLENG/2;


		color = (alpha<<24)|(stStringOut[i].mColor&0x00ffffff);
//		PushEffectFont(pos,"0011223344556677889900",1,stStringOut[i].mStyle,v_buf,color);
		PushEffectFont(pos,stStringOut[i].mStr,1,stStringOut[i].mStyle,v_buf,color);

	}
	SetOutLineColor(old_outline_color);


	DrawEffectFont(v_buf);
}


//전투던젼용 스태틱 변수
static DWORD stDungeonRemainTime=0,stOldDungeonRemainTime=0;
static BOOL stBattleStart=FALSE;
static int stOldKill=0;
static int stNewKill=0;
static float stkillScale=1.0f;


void EFUtil_BattleDungeonString()
{

	if( !_GetDungeonInfo()->IsInDungeon() )
	{
		stBattleStart=FALSE;
		return;
	}

	Vector4f pos;
	DWORD color,alpha;
	_D3DR3TLVERTEX_TEX1 v_buf[2048];

	alpha = 255;
	float loop_time = R3GetLoopTime();
	_R3ENGINE_STATE *state = GetR3State();

	BeginDrawEffectFont();
	Vector3f temp_pos;
	if( stBattleStart==FALSE )		//처음 시작할경우 배틀던젼.
	{
		stBattleStart=TRUE;		
		temp_pos[0] = 200;		temp_pos[1] = -15;		temp_pos[2] = 250;
		StartMagicEffect(temp_pos,0,0x50FFFF33);	//화살표다.
		temp_pos[0] = 82.7f;		temp_pos[1] = -15.0f;		temp_pos[2] = -239;
		StartMagicEffect(temp_pos,0,0x50FFFF33);
		temp_pos[0] = -182;		temp_pos[1] = -9;		temp_pos[2] = -181;
		StartMagicEffect(temp_pos,0,0x50FFFF33);
		temp_pos[0] = -250;		temp_pos[1] = 21;		temp_pos[2] = 207;
		StartMagicEffect(temp_pos,0,0x50FFFF33);
		g_pAvatar->GetPosition(temp_pos);
		StartMagicEffect(temp_pos,0,0x50FFFF32);	//처음시작 
		::Play3DSound(0x50FFFF32,temp_pos);				 
	}
	//----------전투던젼을 위한 출력
	//시간 출력 
	pos[0] = GetR3State()->mScreenXsize/2;
	pos[0]= (float)((LONG)(pos[0]+.5f))-( ( 2.5f ) * _TIME_FONT_XLENG );
	pos[1]= 16.0f;
	pos[2] = 0.1f;
	pos[3] = 0.5f;
	stDungeonRemainTime = _GetDungeonInfo()->GetRemainTime();
	if( stDungeonRemainTime > 60*60 )
		stDungeonRemainTime=0;


	if( stDungeonRemainTime != stOldDungeonRemainTime )	//남은시간에 대한 이펙트
	{
		::Play2DSound(0x50FFFF37);				 
		if( stDungeonRemainTime == 60 )
		{
			g_pAvatar->GetPosition(temp_pos);
			g_pLevel->mBsp->SetEventAnimationState(1000,_EVENT_ANI_STATE_FRONT_START);
			StartMagicEffect(temp_pos,0,0x50FFFF34);
			EFUtil_SetMotionBlurTime(6);
			::Play3DSound(0x50FFFF34,temp_pos);				 
		}
		else
		if( stDungeonRemainTime == 30 )
		{
			g_pAvatar->GetPosition(temp_pos);
			EFUtil_SetMotionBlurTime(6);
			StartMagicEffect(temp_pos,0,0x50FFFF35);
			::Play3DSound(0x50FFFF35,temp_pos);				 
		}/*
		DWORD id=0;
		if( stDungeonRemainTime == 240 + 300 )
			id = 0x50FFFF34;
		else
		if( stDungeonRemainTime == 180 + 300 )
			id = 0x50FFFF35;
		else
		if( stDungeonRemainTime == 60 + 300 )
			id = 0x50FFFF36;
		else
		if( stDungeonRemainTime == 45 + 300 )
			id = 0x50FFFF37;
		else
		if( stDungeonRemainTime == 30 + 300 )
			id = 0x50FFFF38;
		else
		if( stDungeonRemainTime == 15 + 300 )
			id = 0x50FFFF39;

		if( id )
		{
			g_pAvatar->GetPosition(temp_pos);
			StartMagicEffect(temp_pos,0,id);
			::Play3DSound(id,temp_pos);				 
		}*/
	}

	stOldDungeonRemainTime = stDungeonRemainTime;

	if( _GetDungeonInfo()->GetRemainTime() < 30 )
		color=0xff000000|(255<<16)|(0<<8)|(0<<0); 
	else
	if( _GetDungeonInfo()->GetRemainTime() < 60 )
		color=0xff000000|(255<<16)|(179<<8)|(138<<0); 
	else
		color=0xff000000|(195<<16)|(199<<8)|(194<<0); 

	char d_buf[256];
	sprintf(d_buf,"%02d:%02d",_GetDungeonInfo()->GetRemainTime()/60,_GetDungeonInfo()->GetRemainTime()%60);
	PushEffectTimeFont(pos,d_buf,1,0,v_buf,color);

	//마리수 출력 
	color=0xffffffff; 
	/*
	if( gScan[VK_L] )
	{
//			g_pLevel->mBsp->SetEventAnimationState(1000,_EVENT_ANI_STATE_FRONT_START);
		stNewKill++;
	}*/
	stNewKill=_GetDungeonInfo()->GetMonsterKillNum();
	sprintf(d_buf,"%d",stNewKill);

	if( stOldKill!=stNewKill )
	{
		stkillScale = 4.5f;
		Vector3f tr_pos,trans_pos;
		trans_pos[0] = 854 + GetR3State()->mScreenXsize-1024+1.2f*_KILL_FONT_XLENG*1.5f;
		trans_pos[1] = 608 + GetR3State()->mScreenYsize-768+1.2f*_KILL_FONT_YLENG/2;
		trans_pos[2]=60;
		GetInverseTransformVertex(tr_pos,trans_pos);
		StartMagicEffect(tr_pos,0,0x50FFFF31);
		::Play2DSound(0x50FFFF31);
	}
	if( stkillScale > 1.2f )
		stkillScale-=14.0f*R3GetLoopTime();

	if( stkillScale < 1.2f )
	{
		stkillScale=1.2f;
		/*
		Vector3f tr_pos,trans_pos;
		trans_pos[0] = 854 + GetR3State()->mScreenXsize-1024+1.2f*_KILL_FONT_XLENG*1.5f;
		trans_pos[1] = 608 + GetR3State()->mScreenYsize-768+1.2f*_KILL_FONT_YLENG/2;
		trans_pos[2]=60;
		GetInverseTransformVertex(tr_pos,trans_pos);
		StartMagicEffect(tr_pos,0,0x3F0F0017);
		*/
	}

	int jari = strlen(d_buf);
//		pos[0] = 854 + GetR3State()->mScreenXsize-1024-(1-stkillScale)*_KILL_FONT_XLENG*.5f/2;
	pos[0] = 854 + GetR3State()->mScreenXsize-1024+(1.0f-stkillScale)*_KILL_FONT_XLENG*1.5f/2+(3-jari)*_KILL_FONT_XLENG*0.5f;
	pos[1] = 608 + GetR3State()->mScreenYsize-768+(1-stkillScale)*_KILL_FONT_YLENG/2;

	PushEffectKillFont(pos,d_buf,stkillScale,0,v_buf,color);
	stOldKill=stNewKill;

	DrawEffectFont(v_buf);
}