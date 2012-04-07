#include "EffectUtil.h"
#include "EffectUtilFont.h"
#include "EffectCore.h"
#include "CMagicScript.h"
#include "EffectEntityList.h"
#include "MonsterSpecialEffect.h"
//#include "R3Util.h"
#include "Collision.h"
#include "b_sort.h"
#include "../gameprogress/cgp_maingame.h"
#include "../CCamera.h"
#include "../UserInterface/GUIController/GUIMessagebox.h"

extern bool SendLog();

void EFUtil_RestoreDeviceObjects()
{
	EFUtilFontRestoreDeviceObjects();
}
void EFUtil_InvalidateDeviceObjects()
{
	EFUtilFontInvalidateDeviceObjects();
}



//-----------------------------------------------------------------------------------------------------

#define _RENDER_DIST (340-40)
#define _MAX_HEAR_DIST (_RENDER_DIST/4.0f)


float GetVolume(Vector3f pos)
{
	Vector3f cam_pos,temp;

	::GetCameraPos(&cam_pos);
	Vector3fSub(pos,cam_pos,temp);
	float dist = Vector3fDist(temp);	//몬스터와의 거리.
	float density = max((dist-_MAX_HEAR_DIST),0);
	density = 1-density/(_RENDER_DIST-_MAX_HEAR_DIST);

	return density;
}
float GetPan(Vector3f src)	//맵상의 위치하나로 효과음에 쓰일 팬값을 구한다.  -1은 왼쪽에치우침.1은 오른쪽에 치우침.
{
	Vector3f tar_pos;
	Vector3f view_vec;
	_R3ENGINE_STATE *state = GetR3State();

	view_vec[0] = state->mMatView.m[2][2];	//쳐다보고있는 각도와 
	view_vec[1] = state->mMatView.m[2][1];
	view_vec[2] = state->mMatView.m[2][0];
	Normalize(view_vec);

	tar_pos[0] = state->mInvMatView.m[3][0];
	tar_pos[1] = state->mInvMatView.m[3][1];
	tar_pos[2] = state->mInvMatView.m[3][2];	//실제 로컬 위치와의 벡터로....
	Vector3fSub(tar_pos,src,tar_pos);
	Normalize(tar_pos);

	return 0.5f-Vector3fDot(tar_pos,view_vec)/2;	//Dot가 바로 팬이 된다.
}

#define SAME_SOUND_INTERVAL		0.2//같은 사운드가 연속으로 일정시간내 들어오는것을 막는다.
static float stOldTime;
static DWORD stOldSoundID;
void SkipCheckSameSound()	//같은사운드 연속으로 들어오는거 허락하는거...
{
	stOldSoundID = -1;
}
void Play3DSound(DWORD id,Vector3f pos)
{
	float vol = GetVolume(pos);
	if( vol <= 0 )
		return;
	if( stOldSoundID == id && R3GetTime()-stOldTime < SAME_SOUND_INTERVAL )
		return;

	stOldSoundID = id;
	stOldTime = R3GetTime();

	float pan = GetPan(pos);	//맵상의 위치하나로 효과음에 쓰일 팬값을 구한다.  -1은 왼쪽에치우침.1은 오른쪽에 치우침.
//	::StopWave(id);
	::PlayWave(id,0,vol,pan);
}

void Play2DSound(DWORD id)
{
	if( stOldSoundID == id && R3GetTime()-stOldTime < SAME_SOUND_INTERVAL )
		return;

	stOldSoundID = id;
	stOldTime = R3GetTime();
	::PlayWave(id,0,1,0);
}

/*
void PlayUseWeapon3DSound(DWORD id,Vector3f pos)	//무기 사용시 해당 무기아이디와 위치를 날려주면플레이...
{
#define	MAX_WEAPON_CLASS	20
	DWORD ef_code[MAX_WEAPON_CLASS] = {
	0x00a10010,0x00a10110,0x00a10210,0x00a10310,0x00a10410,
	0x00a10510,0x00a10610,0x00a10710,0x00a10810,0x00a10910,
	0x00a10a10,0x00a10b10,0x00a10c10,0x00a10d10,0x00a10e10,
	0x00a10f10,0x00a11010,0x00a11110,0x00411210,0x00411211
	};

	DWORD sf_code[MAX_WEAPON_CLASS] = {
	8002,8002,8002,8001,8001,
	8013,8003,8003,8013,8003,
	8004,8004,8005,8005,8006,
	8007,8007,8008,8008,8012
	};
	if( id>= MAX_WEAPON_CLASS )
		return;
	Play3DSound(sf_code[id],pos);
}

*/

DWORD EFUtil_GetUniqueIDFromItem(void *ptr)
{
	CGroundItem *item = (CGroundItem *)ptr;
	DWORD id=0,real_id,unique_id=0;
	Vector3f pos;

	if( ((CMeshItem *)item)->IsEqualItemType( BOT_GROUND_ITEM ) )	//그라운드 아이템일경우...
	{
		id =item->GetMeshID()&0xffffff00;
		id |=_GROUND_ITEM_FLAG;
	}

	if( id == 0 )
		return 0;

	real_id=GetRealIDFromMagicSptSkipWarning(id);
	pos[0] = item->GetPosX();
	pos[1] = item->GetPosY();
	pos[2] = item->GetPosZ();
	if( real_id )
	{
		if( item->IsPortalGotoDungeon() )
			unique_id = StartMagicEffect(pos,item->GetRotY(),id+1,0,_EF_STATE_ALWAYS_LIVE);			
		else
			unique_id = StartMagicEffect(pos,item->GetRotY(),id,0,_EF_STATE_ALWAYS_LIVE);

//-------------------------------------------디버그 코드 나중에 지울것.

		CMagicList*mag = GetMagicListFromID(unique_id);
		if( mag && id&_GROUND_ITEM_FLAG )
		{
			mag->mDebugPtr = item;
		}
//----------------------------------------------------------------------

		id =item->GetMeshID();
		DWORD main_snd_id = ((id & 0x000F0000)>>16)+273019;
		DWORD snd_id = ((id & 0x000FFF00)>>8)+0xf0000000;
		if( IsExistFromSoundSpt(main_snd_id))
			::Play3DSound(main_snd_id ,pos);
		else
			::Play3DSound(snd_id ,pos);
/*
		if( (id & 0x000F0000) = 0)		//방어구....
		{
		}
		else
		if( (id & 0x000F0000) = 1)		//무기....
		{
		}
		else
		if( (id & 0x000F0000) = 2)		//소모성....
		{
		}
		else
		if( (id & 0x000F0000) = 3)		//기타....
		{
		}
		else
		if( (id & 0x000F0000) = 4)		//포스....
		{
		}
	else
	{
		char buf[256];

		sprintf(buf,"0x%x <-이 그라운드 효과가 없습니다.",id);
		Warning(buf,"");
	}
*/
	}

	if( unique_id == 0 )
	{	//그라운드 아이템이 없으면 대표되는 걸로 띄운다.
		unique_id=StartMagicEffect(pos,item->GetRotY(),0xF0F00000,0,_EF_STATE_ALWAYS_LIVE);
	}

	return unique_id;
}
DWORD EFUtil_GetUniqueIDFromMonster(void *ptr)
{
	return 0;
}

//캐릭터 클래스에있는 더미 변환코드들이 여기에 있다.
BOOL EFUtil_GetTransformFromCharacter(Vector3f *get,void *char_ptr,int dummy_id,Vector3f pos)
{
	D3DXMATRIX *m_temp,yrot,ytemp;

	CCharacter *ch = (CCharacter *)char_ptr;

	MatrixRotate(yrot.m,0,0,360-ch->GetRotY());
	m_temp=ch->GetResultMat(0,ch->GetDummyIndex(dummy_id));
	if( m_temp == NULL )
		return FALSE;

	ytemp = *m_temp*yrot;

	D3DXVECTOR4 out;

	D3DXVec3Transform(&out,(D3DXVECTOR3 *)pos,&ytemp);
	Vector3fTransform( *get,pos,ytemp.m);
//	D3DXVec3Transform(&pOut,&ppp3,&ytemp);

	get[0][0] = -get[0][0];		//틀리니까....
	float y = -get[0][1];
	get[0][1] = get[0][2];
	get[0][2] = y;

	return TRUE;
}



// 레이져라든지 한줄로 그리는 궤적 클래스 
void COneTrace::SetOneTraceStartEndPos(Vector3f start_pos,Vector3f end_pos,float length,float speed)
{
	Vector3fCopy(start_pos,mNow);
	Vector3fCopy(start_pos,mStart);
	Vector3fCopy(end_pos,mEnd);
	Vector3fSub(end_pos,start_pos,mNormal);
	mLength = Vector3fDist(mNormal);	//궤적의 원래 길이.
	Normalize(mNormal);

	mSpeed = speed*1.0f/3.0f;		//초당 가는 거리.
	mTraceDist = 0;//쫒아가는 거리...

//	Vector3fSub(mEnd,mStart,mNormal);
//	Normalize(mNormal);
}

void COneTrace::NowSetTracePos(Vector3f pos)
{
	Vector3fCopy(pos,mNow);
}

BOOL COneTrace::DrawOneTrace()
{
	if( mStart[0] == mNow[0] && mStart[1] == mNow[1] && mStart[2] == mNow[2] )
		return TRUE;

	Vector3f f_temp;
	//실제 끝좌표를 구한다.
	Vector3fSub(mNow,mStart,f_temp);
	float leng = Vector3fDist(f_temp);
	/*
	if( leng > mLength )	//시작한곳에서 더 떨어진곳이면 총 궤적길이끝을 구한다.
	{
		//끝점 에서 정지인경우는 
		if( mEnd[0] == mNow[0] && mEnd[1] == mNow[1] && mEnd[2] == mNow[2] )
		{
			mTraceDist+=::R3GetLoopTime()*mSpeed;
		}
		if( mLength-mFinalSubDist <= 0 )	//완전히 끝났다.
			return FALSE;
		GetPosByDistFromATOB(&mRealStartPos,mNow,mStart,mLength-mFinalSubDist);
	}
	else
		Vector3fCopy(mStart,mRealStartPos);
	*/

	mTraceDist+=::R3GetLoopTime()*mSpeed;

//	Vector4f v_temp;
//	Vector3fMul(,,v_temp);
//	mTraceDist = leng*2.0f/3.0f;

	if( mTraceDist >=  mLength) 
		return FALSE;
	GetPosByDistFromATOB(&mRealStartPos,mStart,mNow,mTraceDist);
/*
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );	
	R3DrawLine(mRealStartPos,mNow,0xffffffff);
*/
	D3DXMATRIX matWorld;
	float axis[3]={0,0,1};

	GetMatrixFromVector(matWorld.m,mNormal,axis);
//	D3DXMatrixIdentity( &matWorld );
	matWorld.m[3][0] = mRealStartPos[0];
	matWorld.m[3][1] = mRealStartPos[1];
	matWorld.m[3][2] = mRealStartPos[2];
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );	

#define TRACE_WIDTH	3.5f
	float dist = GetDist(mNow,mRealStartPos);
	_D3DR3VERTEX_TEX1 vbuf[4];

	vbuf[0].x = -TRACE_WIDTH;
	vbuf[0].y = 0;
	vbuf[0].z = dist;
	vbuf[0].color=0xffffffff;
	vbuf[0].u=0;
	vbuf[0].v=1;

	vbuf[1].x = TRACE_WIDTH;
	vbuf[1].y = 0;
	vbuf[1].z = dist;
	vbuf[1].color=0xffffffff;
	vbuf[1].u=0;
	vbuf[1].v=0;

	vbuf[2].x = -TRACE_WIDTH;
	vbuf[2].y = 0;
	vbuf[2].z = 0;
	vbuf[2].color=0xffffffff;
	vbuf[2].u=1;
	vbuf[2].v=1;

	vbuf[3].x = TRACE_WIDTH;
	vbuf[3].y = 0;
	vbuf[3].z = 0;
	vbuf[3].color=0xffffffff;
	vbuf[3].u=1;
	vbuf[3].v=0;

//	mRealStartPos,mPos;
//	mRealStartPos;

	CEntity *temp = GetEntityFromEffectEntityList( GetRealID(4));
	GetD3dDevice()->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
	GetD3dDevice()->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);
	DrawPrimitiveUPTex1(R3GetSurface(temp->mMat->m_Layer[0].m_iSurface),2,3,vbuf,D3DPT_TRIANGLESTRIP);
	GetD3dDevice()->SetTextureStageState( 0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
	GetD3dDevice()->SetTextureStageState( 0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);

	return TRUE;
}

//DWORD st2ndLightNum;	//2번째 다이나믹 라이트 갯수다.
void EFUtil_Set2ndLight(void *character)
{
	if( !character )
		return;
	CCharacter *ch = (CCharacter *)character;
	float density = ch->Get2ndDensity();
	if( density <=0)	//라이팅 킬일이 없다.
		return;

	Vector3f pos;
	D3DLIGHT8 light;
	LPDIRECT3DDEVICE8 pd3Ddevice = GetD3dDevice();
	float r,g,b;
	r = 0.7f;
	g = 0.8f;
	b = 0.5f;

	density -= R3GetLoopTime()*4.0f;
	density=max(density,0);
	ch->Set2ndDensity(density);

	float diameter = ch->Get2ndLightRadius();

	ZeroMemory( &light, sizeof(D3DLIGHT8) );

	ch->Get2ndLightPos(pos);
	density=1;

	light.Type       = D3DLIGHT_POINT;
	light.Attenuation0 = 0.01f;
	light.Attenuation1 = 0.1f;
	light.Ambient.a = 1.0f;
	light.Diffuse.a = 1.0f;

	light.Ambient.r = r*density;
	light.Ambient.g = g*density;
	light.Ambient.b = b*density;

	light.Range     = diameter*density;

	light.Diffuse.r = r*density;
	light.Diffuse.g = g*density;
	light.Diffuse.b = b*density;

	light.Position.x = pos[0];
	light.Position.y = pos[1];
	light.Position.z = pos[2];

	pd3Ddevice->SetLight(1,&light);
	pd3Ddevice->LightEnable(1,TRUE);
}

void EFUtil_UnSet2ndLight()
{
	LPDIRECT3DDEVICE8 pd3Ddevice = GetD3dDevice();
	pd3Ddevice->LightEnable(1,FALSE);
}

DWORD EFUtil_GetRace(CCharacter *ch)
{ 
	DWORD ch_type = ((CCharacter *)ch)->GetCharTypeID();
	if( ch_type == CTI_AVATAR ||  ch_type == CTI_PLAYER )
	{
		return ((CPlayer *)ch)->GetRace();
	}
	else
		return -1;	//종족을 알수가 없다. 플레이어나 아바타가 아님.
}




// 타겟보다 좀더 앞으로 땡긴다.
void EFUtil_GetDistFromSource(Vector3f get,Vector3f src,Vector3f tar,float ratio,float dist)
{
	_R3ENGINE_STATE *state = GetR3State();

	ratio/=10.0f;		//비율은 1:10기준..
	Vector3fSub(src,tar,get);
	Normalize(get);
	get[0]*=dist*ratio;
	get[1]*=dist*ratio;
	get[2]*=dist*ratio;
	Vector3fAdd(get,tar,get);
}
//스크린쪽으로 타격위치를 좀 땡긴다.
void EFUtil_GetDistFromScreen(Vector3f get,Vector3f pos,float ratio)
{
	_R3ENGINE_STATE *state = GetR3State();
	EFUtil_GetDistFromSource(get,state->mInvMatView.m[3],pos,ratio);
}

void EFUtil_GetDummyPos( Vector3f pos, CCharacter *ch_tar,DWORD d_index)
{
	int m;
	D3DXMATRIX yrot,*m_temp,ytemp;
	D3DXVECTOR3 ppp3;
	D3DXVECTOR4 pOut;
	float axis[3]={0,0,-1};

	if( !ch_tar )
		return;

	m = ch_tar->GetDummyIndex(d_index);
	MatrixRotate(yrot.m,0,0,360-ch_tar->GetRotY());
	m_temp=ch_tar->GetResultMat(0,m);

	float scale = ch_tar->GetScale();
	if( m_temp == NULL || m == -1 )
	{
		if( !EFUTIL_GetPosCharFromBone((Vector3f *)pos,ch_tar,"Bip01 Spine1") )
		{
			BBOX tar_bbox;
			ch_tar->GetBoundBox(tar_bbox);

			pos[0] = ch_tar->GetPosX();
			pos[1] = ch_tar->GetPosY()+(tar_bbox.pMax[1]-tar_bbox.pMin[1])*scale*(0.72f);
			pos[2] = ch_tar->GetPosZ();
		}
		return;
	}

	ytemp = *m_temp*yrot;
	ppp3.x=0;	ppp3.y=0;	ppp3.z=0;

	D3DXVec3Transform(&pOut,&ppp3,&ytemp);

	pos[0] = ch_tar->GetPosX()-(pOut.x)*scale;
	pos[1] = ch_tar->GetPosY()-(-pOut.z)*scale;
	pos[2] = ch_tar->GetPosZ()-(pOut.y)*scale;
}

/*
void EFUtil_GetDummyPos( Vector3f pos, CItem *ch_tar,DWORD d_index)
{
	int m;
	D3DXMATRIX yrot,*m_temp,ytemp;
	D3DXVECTOR3 ppp3;
	D3DXVECTOR4 pOut;
	float axis[3]={0,0,-1};

	m = ch_tar->GetDummyIndex(d_index);

	MatrixRotate(yrot.m,0,0,360-ch_tar->GetRotY());
	m_temp=ch_tar->GetResultMat(0,m);
	float scale = ch_tar->GetScale();

	if( m_temp == NULL || m == -1 )
	{
		BBOX tar_bbox;
		ch_tar->GetBoundBox(tar_bbox);

		pos[0] = ch_tar->GetPosX();
		pos[1] = ch_tar->GetPosY()+(tar_bbox.pMax[1]-tar_bbox.pMin[1])*scale*(0.66f);
		pos[2] = ch_tar->GetPosZ();
		return;
	}

	ytemp = *m_temp*yrot;
	ppp3.x=0;	ppp3.y=0;	ppp3.z=0;

	D3DXVec3Transform(&pOut,&ppp3,&ytemp);

	pos[0] = ch_tar->GetPosX()-(pOut.x)*scale;
	pos[1] = ch_tar->GetPosY()-(-pOut.z)*scale;
	pos[2] = ch_tar->GetPosZ()-(pOut.y)*scale;
}
*/
/*
static float stDamageTime;
static Vector3f stTempPos;

void EFUtil_PreDistortDamage(CCharacter *ch,Vector3f pos)	//맞을때 흔들림.
{
	stDamageTime = ch->GetDamageTimeForShake();	//맞을때 허리제끼기...
	if( stDamageTime <= 0 )
		return;
	ch->ToggleDistortDamage();

	float asdf=g_pCamera->GetPitch();
	float dist_m= R3GetToggle30fps();//(char)ch->GetDistortDamage();
	float x_add = cos(AngleToPi(g_pCamera->GetPitch()))*dist_m;
	float y_add = sin(AngleToPi(g_pCamera->GetPitch()))*dist_m;
	Vector3fCopy(pos,stTempPos);
	pos[0]=stTempPos[0]+x_add;
	pos[2]=stTempPos[2]+y_add;
	Vector3fCopy(pos,stTempPos);
}

void EFUtil_BackUpDistortDamage(CCharacter *ch,Vector3f pos)
{
	if( stDamageTime <= 0 )
		return;
	Vector3fCopy(stTempPos,pos);
}
*/


//---------------------------------- 음악 부분 -----------------------------------------------------------
#define _MUSIC_NO_BATTLE_INTERVAL			18.0f	//전투를 안하는 사이 초...
#define _MUSIC_BATTLE_FADE_INTERVAL			4.0f	//음악 페이드 아웃 초...
#define _MUSIC_FIELD_FADE_INTERVAL			0.1f	//음악 페이드 아웃 초...
#define _MAX_MAP_INDEX	5

char *MusicNameTable[] = {
".\\snd\\BGM\\Belato_Field01.mp3",		//벨라토 
".\\snd\\BGM\\Belato_battle01.mp3",		
".\\snd\\BGM\\Belato_base01.mp3",		

".\\snd\\BGM\\Belato_Field01.mp3",		//코라 
".\\snd\\BGM\\Belato_battle01.mp3",
".\\snd\\BGM\\Cora_Base01.mp3",

".\\snd\\BGM\\Belato_Field01.mp3",		//자원맵
".\\snd\\BGM\\Belato_battle01.mp3",
NULL,

".\\snd\\BGM\\Belato_Field01.mp3",		//아크레시아
".\\snd\\BGM\\Belato_battle01.mp3",
NULL,

".\\snd\\BGM\\BattleDungeon01.mp3",		//배틀던젼
NULL,
NULL,
};


//NULL이면 mp3플레이할게 없다.
static char *stPlayIndexName[3];

#define _MUSIC_FIELD	0
#define _MUSIC_BATTLE	1
#define _MUSIC_TOWN		2

static CPlayMP3 stPlayMP3[3];
static BOOL stLoadedPlayMP3[3];		

static BOOL stStartMusic=FALSE;
static BOOL stMusicToggle=0;	//true일때  배틀음악 중..
static float st_fNoBattleCnt=_MUSIC_NO_BATTLE_INTERVAL;
static float st_fMusicFadeCnt=0;

#define IsPlayBattle()	(stMusicToggle)
#define IsPlayField()	(!stMusicToggle)
#define SetPlayBattle()	 {	stMusicToggle=TRUE;		}	//배틀 음악 셋팅.
#define SetPlayBackGround()	 {	stMusicToggle=FALSE;	}	//백그라운드 음악 셋팅.

static BOOL stIsBaseTown;	//현재 있는곳이 마을인가?
static float st_fFadeDevide;
static DWORD stPlayID=0,stOldPlayID=0;	//플레이할거 이름.

static BOOL EFUTIL_IsBaseTown()		//마을 인가?
{
	DWORD cnt,list[1024];
	Vector3f pos;
	
	if( !g_pLevel )
		return FALSE;

	if( g_pAvatar )
		g_pAvatar->GetPosition(pos);
	else
		Vector3fCopy(GetR3State()->mInvMatView.m[3],pos);

	g_pLevel->mDummy.GetDummyList(_DUMMY_FLAG_MUSIC,&cnt,list);
	for(DWORD i=0; i<cnt; i++)
	{
		if( g_pLevel->mDummy.IsInBBox(list[i],pos) )
			return TRUE;
	}
	return FALSE;
}

static float st_fStartInterval;
void EFUtil_MusicStart(DWORD map_index)	//맵 인덱스를 넣는다..
{
	stPlayIndexName[0]=NULL;
	stPlayIndexName[1]=NULL;
	stPlayIndexName[2]=NULL;

	memset(gAsci,0,sizeof(int)*256);
	if( stStartMusic == FALSE )
	{
		//stStartMusic = TRUE;
		st_fStartInterval=5;
		if( _MAX_MAP_INDEX <= map_index )
			map_index = _MAX_MAP_INDEX-1;

		for(DWORD i=0 ;i<3; i++)
		{
			stPlayIndexName[i] = MusicNameTable[map_index*3+i];
			if( stPlayIndexName[i] )
				stPlayMP3[i].LoadR3MP3(stPlayIndexName[i]);
		}
	}
}


void EFUTIL_SetBattleMusicState()	//아바타가 공격을 당하거나 공격을 할 경우호출해준다. 음악전환을 위해서.
{
	st_fNoBattleCnt = 0;
}


void EFUtil_MusicLoop()	//매루프마다 음악 첵크한다.
{
	if( GetR3State()->mMusicOn == FALSE )
		return;
	if( stStartMusic == FALSE )
	{
		st_fStartInterval -= R3GetLoopTime();
		if( st_fStartInterval < 0 )	//음악 시작하자.
		{
			if( EFUTIL_IsBaseTown() && stPlayIndexName[2] )
			{
				stIsBaseTown = TRUE;
				stOldPlayID = stPlayID;
				stPlayID = _MUSIC_TOWN;
			}
			else
			{
				stIsBaseTown = FALSE;
				stOldPlayID = stPlayID;
				stPlayID = _MUSIC_FIELD;
			}
			st_fMusicFadeCnt = 8;
			st_fFadeDevide = 8;
			stStartMusic = TRUE;
			SetPlayBackGround();
		}
		if( st_fNoBattleCnt == 0 )	//전투는 강제시작.
		{
			stStartMusic = TRUE;	
			SetPlayBackGround();	//전투 음악 틀기위해서.
		}
		return;
	}

	if( st_fNoBattleCnt == 0 )	//배틀카운트가 0이 된경우는 공격을 시작하거나 공격을 당할때다.
	{
		if( IsPlayField() )	//만약 필드 음악 플래이 중이면 배틀로...
		{
			SetPlayBattle();
			st_fMusicFadeCnt = _MUSIC_FIELD_FADE_INTERVAL;
			st_fFadeDevide = _MUSIC_FIELD_FADE_INTERVAL;
			stOldPlayID = stPlayID;
			stPlayID = _MUSIC_BATTLE;
		}
	}
	st_fNoBattleCnt += R3GetLoopTime();
	if( st_fNoBattleCnt >_MUSIC_NO_BATTLE_INTERVAL ) 
	{		
		if( IsPlayBattle() && stPlayIndexName[1])	//필드 음악으로 전환할때다. 배틀 음악이 있어야지한다.
		{
			SetPlayBackGround();
			st_fMusicFadeCnt = _MUSIC_BATTLE_FADE_INTERVAL;
			st_fFadeDevide = _MUSIC_BATTLE_FADE_INTERVAL;
			if( EFUTIL_IsBaseTown() && stPlayIndexName[2] )
			{
				stOldPlayID = stPlayID;
				stPlayID = _MUSIC_TOWN;
				stIsBaseTown=TRUE;
			}
			else
			{
				stOldPlayID = stPlayID;
				stPlayID = _MUSIC_FIELD;
				stIsBaseTown=FALSE;
			}
		}
	}

	if( !IsPlayBattle() )	//전투 음악이 아닐때는
	{
		BOOL b_is_base = EFUTIL_IsBaseTown();
		if( !stPlayIndexName[2] )	//마을 음악이 없으면.
			b_is_base = FALSE;
		if( b_is_base && !stIsBaseTown)	//마을인데 음악 베이스타운이 아니라면.
		{
			stIsBaseTown = TRUE;
			st_fMusicFadeCnt = _MUSIC_BATTLE_FADE_INTERVAL;
			st_fFadeDevide = _MUSIC_BATTLE_FADE_INTERVAL;
			stOldPlayID = stPlayID;
			stPlayID = _MUSIC_TOWN;
		}
		if( !b_is_base && stIsBaseTown)	//필드인데  음악이 필드가 아니라면.
		{
			stIsBaseTown = FALSE;
			st_fMusicFadeCnt = _MUSIC_BATTLE_FADE_INTERVAL;
			st_fFadeDevide = _MUSIC_BATTLE_FADE_INTERVAL;
			stOldPlayID = stPlayID;
			stPlayID = _MUSIC_FIELD;
		}
	}

	if( st_fMusicFadeCnt > 0 )	//음악이 페이드 인중이라면.
	{
		st_fMusicFadeCnt -= R3GetLoopTime();
		if( stPlayIndexName[stPlayID] && stPlayIndexName[stOldPlayID] )
		{
			float temp;
			temp = min(st_fMusicFadeCnt/st_fFadeDevide,1);
			stPlayMP3[stOldPlayID].SetVolumeMP3(temp);	//먼지모르니까 귀찮다. 다..
			if(	st_fMusicFadeCnt <= 0 )
			{
				stPlayMP3[stOldPlayID].StopR3MP3();
				stPlayMP3[stPlayID].PlayR3MP3();
			}
		}
	}
}


void EFUtil_MusicEnd()
{
	st_fNoBattleCnt=_MUSIC_NO_BATTLE_INTERVAL;
	stStartMusic = FALSE;
	stOldPlayID = 0;
	stPlayID=0;
	for(DWORD i=0 ;i<3; i++)
	{
		if( stPlayIndexName[i] )
			stPlayMP3[i].ReleaesR3MP3();
		stPlayIndexName[i]=NULL;
	}
}

BOOL EFUtil_GetCharTransformFromIndex(Vector3f *get_pos,CCharacter *c_char,DWORD id)
{
	int m;

	D3DXMATRIX yrot,ytemp,*m_temp;
	D3DXVECTOR3 ppp3;
	D3DXVECTOR4 pOut[2];
	float scale;
	scale = c_char->GetScale();

	m=c_char->GetDummyIndex(id);
	if( m == -1 || m == 0 )
		goto EFUtil_GetCharTransformFromIndexError2;
	ppp3.x=0;
	ppp3.y=0;
	ppp3.z=0;
	m_temp=c_char->GetResultMat(0,m);
	if( m_temp == NULL )
		goto EFUtil_GetCharTransformFromIndexError2;
	MatrixRotate(yrot.m,0,0,360-c_char->GetRotY());
	ytemp = *m_temp*yrot;
	D3DXVec3Transform(&pOut[0],&ppp3,&ytemp);
	get_pos[0][0] = c_char->GetPosX()-pOut[0].x*scale;
	get_pos[0][1] = c_char->GetPosY()+pOut[0].z*scale;
	get_pos[0][2] = c_char->GetPosZ()-pOut[0].y*scale;
	return TRUE;

EFUtil_GetCharTransformFromIndexError2:
	BBOX bbox;
	c_char->GetBoundBox(bbox);
	get_pos[0][0] = c_char->GetPosX();
	get_pos[0][1] = c_char->GetPosY()+(bbox.pMax[1]-bbox.pMin[1])*scale*(0.66f);
	get_pos[0][2] = c_char->GetPosZ();
	return FALSE;
}

BOOL EFUtil_GetCharTransformFromIndex(Vector3f *get_pos,CCharacter *c_char,CEquipItem *item,DWORD id)
{
	int m;

	D3DXMATRIX yrot,ytemp,*m_temp;
	D3DXVECTOR3 ppp3;
	D3DXVECTOR4 pOut[2];
	float scale;
	scale = c_char->GetScale();

	if( !item )
		goto EFUtil_GetCharTransformFromIndexError;

	m=item->GetDummyIndex(id);
	if( m == -1 || m == 0 )
		goto EFUtil_GetCharTransformFromIndexError;

	ppp3.x=0;
	ppp3.y=0;
	ppp3.z=0;
	m_temp=c_char->GetResultMat(CEPT_WEAPON,m);
	if( m_temp == NULL )
		goto EFUtil_GetCharTransformFromIndexError;

	MatrixRotate(yrot.m,0,0,360-c_char->GetRotY());
	ytemp = *m_temp*yrot;
	D3DXVec3Transform(&pOut[0],&ppp3,&ytemp);
	get_pos[0][0] = c_char->GetPosX()-pOut[0].x*scale;
	get_pos[0][1] = c_char->GetPosY()+pOut[0].z*scale;
	get_pos[0][2] = c_char->GetPosZ()-pOut[0].y*scale;
	return TRUE;

EFUtil_GetCharTransformFromIndexError:
	BBOX bbox;
	c_char->GetBoundBox(bbox);
	get_pos[0][0] = c_char->GetPosX();
	get_pos[0][1] = c_char->GetPosY()+(bbox.pMax[1]-bbox.pMin[1])*scale*(0.66f);
	get_pos[0][2] = c_char->GetPosZ();
	return FALSE;
}

BOOL EFUtil_IsCollisionFarAttack(CCharacter *ch_sou,CCharacter *ch_tar)
{
	Vector3f out_pos,sout_pos,src_pos,tar_pos,temp;
	CEquipItem *item;
	DWORD fire_id;

	ch_tar->GetPosition(src_pos);
	ch_tar->GetPosition(tar_pos);

	if( ch_sou->GetCharTypeID() == CTI_PLAYER || ch_sou->GetCharTypeID() == CTI_AVATAR )
	{
		item = ((CPlayer*)ch_sou)->GetEquipItem( CEPT_WEAPON );
		fire_id = item->GetDummyIndex(_DUMMY_FIRE1);
		if( fire_id != -1 )
		{
			EFUtil_GetCharTransformFromIndex(&sout_pos,ch_sou,item,_DUMMY_FIRE1);
		}
	}
	else
	{
		EFUtil_GetCharTransformFromIndex(&sout_pos,ch_sou,_DUMMY_FIRE1);
	}
	EFUtil_GetCharTransformFromIndex(&out_pos,ch_tar,_DUMMY_DAMAGE2);

	return g_pLevel->mBsp->IsCollisionFace(sout_pos,out_pos,&temp,0.0f);
}

#include "r3move.h"
#include "r3math.h"


void EFUTIL_RunDeathCamera(Vector3f point)
{
	Vector3f get_cam_pos;
	_R3ENGINE_STATE *state=GetR3State();
	float m_speed=R3GetTime();

	D3DXVECTOR3 from;
	D3DXVECTOR3 up    = D3DXVECTOR3( 0.0f, 0.8f, 0.0f );

//	EFUtil_GetDummyPos( point, _GetAvatar(),_DUMMY_DAMAGE2);
//----------------------------
	Vector3fCopy(point,((float*)from));
	
	from[0] += cos(m_speed*0.5f*0.3f)*60;
	from[1] += cos(m_speed*0.4f*0.3f)*20+30;
	from[2] += sin(m_speed*0.3f*0.3f)*30;

	Vector3f c_get;
	if( g_pLevel &&  g_pLevel->mBsp->IsCollisionFace(point,from,&c_get,3) )	//평면과의 떨어진 거리를 리턴
		Vector3fCopy(c_get,from);

	D3DXMatrixLookAtLH( &state->mMatView, &from, (D3DXVECTOR3*)point, &up );
    D3DXMatrixInverse( &state->mInvMatView, NULL, &state->mMatView );

    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_VIEW, &state->mMatView );
	
	get_cam_pos[0] = state->mInvMatView.m[3][0];
	get_cam_pos[1] = state->mInvMatView.m[3][1];
	get_cam_pos[2] = state->mInvMatView.m[3][2];
//	SetPointCamera(get_cam_pos);

	MatrixMultiply( state->mMatViewProj.m ,state->mMatProj.m,state->mMatView.m);
}

void EFUTIL_DrawLineDummy(CCharacter *ch,DWORD dummy,DWORD color)
{
	Vector3f pos;
	ch->GetPosition(pos);	//몬스터 위치.
	Vector3f get,d_pos;
	d_pos[0]=0;			d_pos[1]=0;			d_pos[2]=0;

	float mon_scale = ch->GetScale();
	if( EFUtil_GetTransformFromCharacter(&get,ch,dummy,d_pos))
	{
		pos[0] += (get[0])*mon_scale;
		pos[1] += (get[1])*mon_scale;
		pos[2] += (get[2])*mon_scale;
	}

	Vector3f a,b;
	a[0]=pos[0]+10;
	a[1]=pos[1];
	a[2]=pos[2];
	b[0]=pos[0]-10;
	b[1]=pos[1];
	b[2]=pos[2];
	R3DrawLine(a,b,color);

	a[0]=pos[0];
	a[1]=pos[1];
	a[2]=pos[2]+10;
	b[0]=pos[0];
	b[1]=pos[1];
	b[2]=pos[2]-10;
	R3DrawLine(a,b,color);
}

void EFUTIL_DrawLineDummyWeapon(CPlayer *pl,DWORD dummy,DWORD color)
{
	int m;
	DWORD	weapon_mesh_id = ID_INVALID;

	CHARACTEROBJECT *co;
	DWORD		MaxObj;
	ObjectMesh	*pMesh;
	D3DXMATRIX yrot,ytemp,*m_temp;
	D3DXVECTOR3 ppp3;
	D3DXVECTOR4 pOut[2];

	CEquipItem *item = pl->GetEquipItem( IEPT_WEAPON );
	if( item )
		weapon_mesh_id = item->GetMeshID();
	else
		return ;
	co = _GetMeshMgr()->GetMesh( RLI_ITEM, weapon_mesh_id );
	if( co == NULL )
		return ;

	int id0= item->GetDummyIndex(dummy);

	MaxObj		= co->m_MaxObj;
	pMesh		= co->m_pMesh;

	MatrixRotate(yrot.m,0,0,360-pl->GetRotY());
	ppp3.x=0;
	ppp3.y=0;
	ppp3.z=0;
	m=id0;
	m_temp=pl->GetResultMat(CEPT_WEAPON,m);
	if( m_temp == NULL )
		return ;
	ytemp = *m_temp*yrot;
	D3DXVec3Transform(&pOut[0],&ppp3,&ytemp);

	Vector3f a,b,pos;
	pos[0] = pl->GetPosX()-(pOut[0].x);
	pos[1] = pl->GetPosY()-(-pOut[0].z);
	pos[2] = pl->GetPosZ()-(+pOut[0].y);

	a[0]=pos[0]+3;
	a[1]=pos[1];
	a[2]=pos[2];
	b[0]=pos[0]-3;
	b[1]=pos[1];
	b[2]=pos[2];
	R3DrawLine(a,b,color);

	a[0]=pos[0];
	a[1]=pos[1];
	a[2]=pos[2]+3;
	b[0]=pos[0];
	b[1]=pos[1];
	b[2]=pos[2]-3;
	R3DrawLine(a,b,color);
}




float EFUtil_GetAngle( CCharacter *ch_src, CCharacter *ch_tar)
{
	float gac;

	if( ch_src )
	{
		gac = ch_src->GetRotY()+180;
		if( ch_tar )
		{
			if( ch_tar == ch_src )
				return gac-180;
			Vector3f a_pos,b_pos;
			ch_src->GetPosition(a_pos);
			ch_tar->GetPosition(b_pos);
			gac = R3API::GetYAngle( a_pos, b_pos );
		}
	}
	else
	{
		gac=0;
	}
	return gac;
}

void EFUTIL_CheckContinuousSkillForce(CCharacter *ch)		//지속형(보조) 스킬,포스
{
	DWORD i;

	APPLIED_SF *sf =ch->GetAppliedSF();
	BYTE type;
	DWORD index[2],d_effect_id,effect_key;
	DWORD is_exist[2]={0,0};

	if( _GetAvatar() )
	{
		if( !_GetAvatar()->IsEnableDetect(ch) )	//히든일경우 이펙트를 안뿌린다.
			return;
	}

	if( ch->IsEnable(ROSF_OUT_OF_VISUAL_FIELD_CLIENT)) //캐릭터가 렌더 영역 바깥으로 빠져나간경우.
		return;

	for( i=0; i<MAX_APPLIED_SF; i++)
	{		
		type = sf->m_listSF[0][i].GetSFType();
		if( type != 255 )			// 스킬/포스의 정보 ( 0 = 득, 1 =실 )
		{
			if( sf->m_dwApplyTime[0][i] > is_exist[0] )
			{
				index[0] = i;
				is_exist[0] = sf->m_dwApplyTime[0][i];
			}
		}
		type = sf->m_listSF[1][i].GetSFType();
		if( type == 255 )			// 스킬/포스의 정보 ( 0 = 득, 1 =실 )
			continue;
		if( sf->m_dwApplyTime[1][i] > is_exist[1] )
		{
			index[1] = i;
			is_exist[1] = sf->m_dwApplyTime[1][i];
		}
	}

	for(i=0; i<2; i++)	//득과 실
	{
		if( is_exist[i] )	//득과 실이되는 스킬이나 포스가 존재.
		{
			type = sf->m_listSF[i][index[i]].GetSFType();
			SF_DATA *sf_data = sf->m_listSF[i][index[i]].GetData();
			if( !sf_data )
				continue;
			Vector3f pos;
			float yangle;
			//sf_data->dwID; //스킬이나 포스 종류다. 
			if( type == CAT_SKILL )	//스킬이다.
			{
				effect_key=0X3F000000|((sf_data->dwDTCode & 0x0000ffff)<<4);
//				f_effect_id=effect_key|0x00500000;	//바닥 
				d_effect_id=effect_key|0x00600000;	//데미지
			}
			else
			if( type == CAT_FORCE )	//포스이다.
			{
				effect_key=0x4f000000|((sf_data->dwDTCode & 0x00000fff)<<8);
//				f_effect_id=effect_key|0x00500000;	//바닥 
				d_effect_id=effect_key|0x00600000;	//데미지
			}
			else
			{	
				//생기면 안되.
			}
			if( effect_key != sf->m_dwEffectKey[i] )
			{
				yangle = ch->GetRotY();
				//자 뿌려보자.
				if( sf->m_dwEffectKey[i] )	//기존게 있다면 일단 지운다.
				{
					if( sf->m_dwUniqueID[i][0] )
						SetDisapearTimeMagicEffect(sf->m_dwUniqueID[i][0],0);	//0초동안 사라지게.
					if( sf->m_dwUniqueID[i][1] )
						SetDisapearTimeMagicEffect(sf->m_dwUniqueID[i][1],0);	//0초동안 사라지게.
				}
				/*
				if( IsExistFromMagicSpt(f_effect_id) )	//존재하면 바닥
				{
					ch->GetPosition(pos);
					sf->m_dwUniqueID[i][0] = StartMagicEffectType(ch,pos,yangle, f_effect_id,0,_EF_STATE_ALWAYS_LIVE);
					//SetAttachCharMagicEffect(sf->m_dwUniqueID[i][0],ch->GetCharTypeID(),ch->GetIndex());
					sf->m_dwEffectKey[i] = effect_key;
				}*/
				if( IsExistFromMagicSpt(d_effect_id) )	//존재하면 데미지2
				{
					EFUtil_GetDummyPos( pos, ch,_DUMMY_DAMAGE2);
					sf->m_dwUniqueID[i][1] = StartMagicEffectType(ch,pos,yangle, d_effect_id,0,_EF_STATE_ALWAYS_LIVE);
					//SetAttachCharMagicEffect(sf->m_dwUniqueID[i][1],ch->GetCharTypeID(),ch->GetIndex());
					sf->m_dwEffectKey[i] = effect_key;
				}
			}
			else	//있으니까 붙여놓아라...
			{
				if( sf->m_dwUniqueID[i][0] )
				{
					EFUTIL_GetPosTypeFromChar(pos,ch,sf->m_dwUniqueID[i][0]);
					MovePosMagicEffect(sf->m_dwUniqueID[i][0],pos,yangle);
				}
				if( sf->m_dwUniqueID[i][1] )
				{
					EFUTIL_GetPosTypeFromChar(pos,ch,sf->m_dwUniqueID[i][1]);
					MovePosMagicEffect(sf->m_dwUniqueID[i][1],pos,yangle);
				}
			}
		}
		else	//존재하지 않음
		{
			if( sf->m_dwUniqueID[i][0] )	// 끝났으니지운다.
			{
				SetDisapearTimeMagicEffect(sf->m_dwUniqueID[i][0],0);
				sf->m_dwUniqueID[i][0]=0;
			}
			if( sf->m_dwUniqueID[i][1] )	// 끝났으니지운다.
			{
				SetDisapearTimeMagicEffect(sf->m_dwUniqueID[i][1],0);
				sf->m_dwUniqueID[i][1]=0;
			}

			if( !sf->m_dwUniqueID[i][0] && !sf->m_dwUniqueID[i][1] )	// 끝났으니지운다.
				sf->m_dwEffectKey[i]=0;
		}
	}
}

void EFUtil_DeleteSF(APPLIED_SF *sf)	//지울만한 스킬포스는 지운다.
{
	for(DWORD j=0; j<2; j++)	//득과 실
	{
		if( sf->m_dwUniqueID[j][0] )	// 끝났으니지운다.
		{
			SetDisapearTimeMagicEffect(sf->m_dwUniqueID[j][0],0);
			sf->m_dwUniqueID[j][0]=0;
		}
		if( sf->m_dwUniqueID[j][1] )	// 끝났으니지운다.
		{
			SetDisapearTimeMagicEffect(sf->m_dwUniqueID[j][1],0);
			sf->m_dwUniqueID[j][1]=0;
		}

		if( !sf->m_dwUniqueID[j][0] && !sf->m_dwUniqueID[j][1] )	// 끝났으니지운다.
			sf->m_dwEffectKey[j]=0;
	}
}

void EFUtil_CheckDeleteAttachMagicEffect()	//캐릭터에 붙은 매직이펙트들을 점검해서 안쓰는건 지운다.
{
	CMagicList *magic = GetMagicList();
	DWORD m_cnt = GetMagicListCnt();
	DWORD i;
	CCharacter	*ch;

	for(i=0; i<m_cnt; i++)
	{
		if( magic->mCharIndex == -1 || magic->mCharTypeID == -1 )
			continue;
		ch = _GetCharMgr()->GetCharacter( magic->mCharTypeID,magic->mCharIndex );
		if( ch == NULL )	//접속을 끊었거나 캐릭터 클래스가 사라졌다.
		{
			SetDisapearTimeMagicEffect(magic->mUniqueID,0);
		}
		else
		if( ch->IsEnable(ROSF_OUT_OF_VISUAL_FIELD_CLIENT)) //캐릭터가 렌더 영역 바깥으로 빠져나간경우.
		{
			APPLIED_SF *sf =ch->GetAppliedSF();
			EFUtil_DeleteSF(sf);
//			if( ch->GetCharTypeID() == CTI_PLAYER )	
//				((CPlayer*)ch)->m_cSwordTrace.SetTraceState( FALSE );//플레이어면 궤적 끈다.
		}
	}
}

static DWORD stBeamGun[6] = {0xa11212,0xa11116,0xa11330,0xa11136,0xa1133a,0xa1134e};

BOOL EFUtil_IsBeamGun(CMeshItem *item)	//빔건인가? 
{
	if( !item )
		return FALSE;
	DWORD id = item->GetMeshID();
	for( DWORD i=0; i<6; i++)
	{
		if( stBeamGun[i] == id )
			return TRUE;
	}
	return FALSE;
}


static float stMotionBlurTime=0;

void EFUtil_SetMotionBlurOff()
{
	SetStateFullScreenEffect(FALSE);
	stMotionBlurTime=0;
}
void EFUtil_LoopMotionBlurTime()
{
	if( stMotionBlurTime > 0 )
	{
		stMotionBlurTime -= R3GetLoopTime();
		if( stMotionBlurTime == 0)
			stMotionBlurTime = -1;	//개같은경우 방지.
	}
	if( stMotionBlurTime < 0 )
	{
		SetStateFullScreenEffect(FALSE);
		stMotionBlurTime=0;
	}
}

void EFUtil_SetMotionBlurTime(float Time)
{
	stMotionBlurTime = Time;
	SetStateFullScreenEffect(_FSE_MOTION_BLUR);
}


//------------------------------------ 에러 출력 --------------------------------------
static EffectInfo stLastestEfCode;		//앤 최신 코드기록용
static EffectInfo stLastestEfCodeMessage;	//앤 디스플레이용.



void EFUtil_SetLastestEfCode(EffectInfo *e_info)
{
	memcpy(&stLastestEfCode,e_info,sizeof(EffectInfo));
}

void EFUtil_EffectLog(char *str)
{
	Warning(str,"");
}
void EffectLog(char *buf)	//이펙트 로그파일 만든다.
{
	FILE *fp;
	int end,start;

	fp = fopen(".\\NetLog\\EffectLog.log","rt+");
	if(fp == NULL )
	{
		fp = fopen(".\\NetLog\\EffectLog.log","wt");
		if(fp == NULL )
			return;
	}
	start=ftell(fp);	//파일 사이즈를 알아낸다.
	fseek(fp,0,SEEK_END);
	end=ftell(fp);
	fputs(buf,fp);
	fclose(fp);

	if( end-start > 5000000 )
	{	// 로그가 5메가 이상 쌓였을시..
		SendLog();	// 로그파일 전송
		
		fp = fopen(".\\NetLog\\EffectLog.log","wt");		// 용량 큰 로그 파일 삭제
		fclose(fp);		
	}

/*
//	if( end-start > 1000 )	//5메가 넘으면 자동전송하고 파일지운다.
	if( end-start > 5000000 )	//5메가 넘으면 자동전송하고 파일지운다.
	{
	MoveFile(".\\NetLog\\EffectLog.log",".\\NetLog\\EffectLog5M.log");

		STARTUPINFO         siStartupInfo;
        PROCESS_INFORMATION piProcessInfo;
        memset(&siStartupInfo, 0, sizeof(siStartupInfo));
        memset(&piProcessInfo, 0, sizeof(piProcessInfo));
        siStartupInfo.cb = sizeof(siStartupInfo);
        CreateProcess(".\\SendLog.exe",     // 실행 경로에 유의
                                                                          " -a",                                // 자동 전송용 파라미터
                                                                          0,
                                                                          0,
                                                                          FALSE,
                                                                          CREATE_DEFAULT_ERROR_MODE,
                                                                          0,
                                                                          ".\\",                  // 작업 디렉토리 
                                                                          &siStartupInfo,
                                                                          &piProcessInfo);

//		system(".\\SendLog.exe -a");
	}
*/
}

extern BOOL st_bCmp;
static char JMessage[256];
static float JMTimer=0;

void EFUtil_MessageStack()
{
	if( JMTimer <= 0 )
		return;
	JMTimer-=R3GetLoopTime();

	if( !st_bCmp )	//치트키성공시에만 
		return;

	char buf[256];
	::DrawR3Hangul( 180,26*5+40, JMessage,0xffffffff,R3_HAN_OUTLINE);
	sprintf(buf,"ef ID = 0x%x,myself %d/%d,target %d/%d",stLastestEfCodeMessage.dwID
		,stLastestEfCodeMessage.dwMyself[0],stLastestEfCodeMessage.dwMyself[1]
		,stLastestEfCodeMessage.dwTarget[0],stLastestEfCodeMessage.dwTarget[1]);
	::DrawR3Hangul( 180,26*5+40+13,buf,0xffffffff,R3_HAN_OUTLINE);
}


void DrawR3Message(char *message)
{
	JMTimer=5;	//기본 5초.
	_strcpy(JMessage,message,100);
	memcpy(&stLastestEfCodeMessage,&stLastestEfCode,sizeof(EffectInfo));

	char buf[256];
	SYSTEMTIME sysTime;   // Win32 time information
	GetSystemTime(&sysTime);
	sprintf(buf,"%d/%d/%d %d:%d:%d:\n\t%s\n",sysTime.wYear,sysTime.wMonth,sysTime.wDay
		,sysTime.wHour,sysTime.wMinute,sysTime.wSecond,message);
	EffectLog(buf);
	sprintf(buf,"\tef ID = 0x%x,myself %d/%d,target %d/%d\n"
		,stLastestEfCodeMessage.dwID
		,stLastestEfCodeMessage.dwMyself[0],stLastestEfCodeMessage.dwMyself[1]
		,stLastestEfCodeMessage.dwTarget[0],stLastestEfCodeMessage.dwTarget[1]);
	EffectLog(buf);
}
#include "conio.h"
void DrawR3ErroeMessage(char *message)
{
	JMTimer=5;	//기본 5초.
	_strcpy(JMessage,message,100);
	memcpy(&stLastestEfCodeMessage,&stLastestEfCode,sizeof(EffectInfo));

	char buf[256];
	SYSTEMTIME sysTime;   // Win32 time information
	GetSystemTime(&sysTime);
	sprintf(buf,"%d/%d/%d %d:%d:%d:\n\t%s\n",sysTime.wYear,sysTime.wMonth,sysTime.wDay
		,sysTime.wHour,sysTime.wMinute,sysTime.wSecond,message);
	EffectLog(buf);
	sprintf(buf,"\tef ID = 0x%x,myself %d/%d,target %d/%d\n"
		,stLastestEfCodeMessage.dwID
		,stLastestEfCodeMessage.dwMyself[0],stLastestEfCodeMessage.dwMyself[1]
		,stLastestEfCodeMessage.dwTarget[0],stLastestEfCodeMessage.dwTarget[1]);
	EffectLog(buf);

	_GUIErrorMessageBox( message);
/*
	R3BeginScene();
	_GetUIScreen()->Draw();
	R3EndScene();
	GetD3dDevice()->Present( NULL, NULL, NULL, NULL );
	R3BeginScene();
	_GetUIScreen()->Draw();
	R3EndScene();
	GetD3dDevice()->Present( NULL, NULL, NULL, NULL );
	getch();
*/
//	exit(0);
}


void EFUtil_SetErrorMessage()	//에러메시지를 게임쪽과 연결한다.
{
	::SetErrorMessageProc(DrawR3ErroeMessage);		//워닝이나 에러메시지를 화면상에 뿌리는 방법이다.
	::SetWarningMessageProc(DrawR3Message);
}


//-----------------------쉐도우 리스트 ---------------------
#define _MAX_CALC_SHADOW	64		//최대 아바타 + 1
static DWORD stShadowCnt=0;
typedef struct{
	CCharacter *ptr;
}_SHADOW_LIST;
_SHADOW_LIST stShadowList[_MAX_CALC_SHADOW];	


static void PrepareShadow(float scale,float add_pos)
{
	LPDIRECT3DDEVICE8 pDevice = GetD3dDevice();
	D3DXMATRIX  Proj;
	D3DXMATRIX  View;
	D3DXMATRIX	rot;
	float dg=66.0f*32.0f*scale;

	D3DXVECTOR3 vEyePt    = D3DXVECTOR3( -0.5f, dg*5,dg*3-add_pos+0.5f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( -0.5f, 0.0f, -add_pos+0.5f );
	D3DXVECTOR4 out,out1;
	D3DXMatrixRotationY(&rot,GetR3State()->mMainLightYAngle);	
		
	D3DXVec3Transform(&out1,&vLookatPt,&rot);
	D3DXVec3Transform(&out,&vEyePt,&rot);
	D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &View, (D3DXVECTOR3*)&out, (D3DXVECTOR3*)&out1, &vUpVec );
	pDevice->SetTransform( D3DTS_VIEW, &View );

//	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//	pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0);
	D3DXMatrixPerspectiveFovLH( &Proj, D3DX_PI/1024.0f, 1, 4.0f, 400000 );
	pDevice->SetTransform( D3DTS_PROJECTION, &Proj );
	pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_LIGHTING,FALSE );
}

void EFUtil_PushShadow(CCharacter *ptr)//일단 쉐도우 뿌릴거 리스트를 만든다.
{
	if( ptr->GetConditionInfo()->m_bEnable_Stealth )	//안보일경우 쉐도우는 끈다.
		return;

	if( CTI_AVATAR == ptr->GetCharTypeID() )	//아바타야 뻔하니까 퉁.
		return;
	if( stShadowCnt >= _MAX_CALC_SHADOW-1 )	//넘친다.
		return;
	if( !GetR3State()->mShadowDetail )
		return;

	stShadowList[stShadowCnt].ptr = ptr;
	stShadowCnt++;
}


//EFUtil_PushShadow(*ptr,DWORD char_type_id);	//일단 쉐도우 뿌릴거 리스트를 만든다.

static void GetScaleAddPos(BBOX *bbox,float *scale,float *add_pos )
{
	*scale=(bbox->pMax[1]/18.0f);
	float diff = max(((bbox->pMax[1]*3)/5),bbox->pMax[0]);
	*add_pos = (diff+bbox->pMax[0]-bbox->pMax[0]*2.0f)/2.0f;
}

static const float shadow_intensity = 0.8f;
void EFUtil_DrawShadowList(DWORD cnt)	//Push 해놓은 쉐도우 리스트를 필요에 따라 비운다.cnt=-1이면 다뿌린다.
{
	LPDIRECT3DDEVICE8 pDevice = GetD3dDevice();
	float scale=1.0f;
	float add_pos=10;

	if( cnt == 0 )
		goto EFUtil_DrawShadowList_Out;
	if( !g_pAvatar || !g_pLevel)	
		goto EFUtil_DrawShadowList_Out;
	Vector3f avatar_pos;
	DWORD i;
	DWORD id[_MAX_CALC_SHADOW+1];
	DWORD id_temp[_MAX_CALC_SHADOW+1];	//소트된 아이디리스트
	SHORT dist[_MAX_CALC_SHADOW+1];
	SHORT temp[_MAX_CALC_SHADOW+1];	//소트를 위한 템프변수
	BBOX bbox;

	g_pAvatar->GetPosition(avatar_pos);

	DWORD loop_cnt;
	//-쉐도 뿌릴것들 준비.
	if( cnt == -1 )
	{
		loop_cnt = stShadowCnt;
		for(i=0; i<loop_cnt; i++)	//어차피 다뿌릴거.
			id[i]=i;
	}
	else
	if( cnt == 1 )	//아바타만.
	{
		loop_cnt = 0;
	}
	else	//개수에 따라 아바타를 기준으로 가까운순서로 소팅. 당빠가장빠른 바이트 소팅을 쓴다.
	{
		loop_cnt = min(stShadowCnt,cnt);

		Vector3f v_temp,v_pos;
		for(i=0 ;i<stShadowCnt; i++)
		{
			stShadowList[i].ptr->GetPosition(v_pos);
			Vector3fSub(v_pos,avatar_pos,v_temp)
			dist[i]=(SHORT)Vector3fDist(v_temp);
			id[i]=i;
		}
		ByteSortForShort(stShadowCnt,id,dist,id_temp,temp);	//자 소팅.
	}

	MultiTexOff();
	if( !g_pAvatar->GetConditionInfo()->m_bEnable_Stealth )	//안보일경우 쉐도우는 끈다.
	{
		R3EndScene();
		//아바타 먼저 뿌리고
		ShadowBeginScene();
		_GetCharIF()->SetState();

		g_pAvatar->GetBoundBox(bbox,TRUE);
		GetScaleAddPos(&bbox,&scale,&add_pos);
		PrepareShadow(scale,add_pos);	
		//PrepareShadow(1,10);
		g_pAvatar->CreateShadow();
		_GetCharIF()->UnSetState();
		ShadowEndScene();
	
		g_pLevel->PrepareShadowRender(avatar_pos,GetShadowTexture(),0.8f,3,scale,add_pos);
	//	g_pLevel->PrepareShadowRender(avatar_pos,GetShadowTexture(),0.7f,5,1,10);
		R3BeginScene();
		g_pLevel->DrawShadowRender(avatar_pos);
	}

	DWORD char_id;
	//나머지 잡것들을 뿌리자.
	for( i=0; i<loop_cnt; i++)
	{
		char_id=stShadowList[id[i]].ptr->GetCharTypeID();
		float alpha=0.7f;
		if( char_id == CTI_PLAYER )
		{
			CPlayer *pl = ((CPlayer *)stShadowList[id[i]].ptr);
			R3EndScene();
			_GetCharIF()->SetState();
			ShadowBeginScene();
			pl->GetBoundBox(bbox,TRUE);
			GetScaleAddPos(&bbox,&scale,&add_pos);
			PrepareShadow(scale,add_pos);	
			pl->CreateShadow();
		}
		else
		if( char_id == CTI_MONSTER )
		{
			CMonster *mon = (CMonster *)stShadowList[id[i]].ptr;
			if( mon->IsEqualAction( CAI_DEATH_DIE ) )
				continue;

			R3EndScene();
			_GetCharIF()->SetState();
			ShadowBeginScene();
			Vector3f pos;
//			((CMonster *)stShadowList[id[i]].ptr)->CreateShadow();

			mon->GetBoundBox(bbox,TRUE);
			mon->GetPosition(pos);
			Vector3fMul(bbox.pMax,mon->GetScale(),bbox.pMax);
			scale=(bbox.pMax[1]/10.0f);
			float diff = max(((bbox.pMax[1]*3)/5),bbox.pMax[0]);
			add_pos = (diff+bbox.pMax[0]-bbox.pMax[0]*2.0f)/2.0f;
//				min(scale*10,bbox.pMax[0]*2.0f);
			PrepareShadow(scale,add_pos);

			mon->PSetVertexBlending( 0, mon->GetMesh(), FALSE ); 
			_GetCharIF()->DrawCharacterShadow( mon->GetMesh(), pos, mon->GetRotY(),0.8f *  mon->GetAlphaDensity(),  mon->GetScale() );
			alpha=0.6f;
		}
		else
		if( char_id == CTI_NPC )
		{
			CNpc *npc = ((CNpc *)stShadowList[id[i]].ptr);

			R3EndScene();
			_GetCharIF()->SetState();
			ShadowBeginScene();
			npc->GetBoundBox(bbox,TRUE);
			GetScaleAddPos(&bbox,&scale,&add_pos);
			PrepareShadow(scale,add_pos);
			npc->PSetVertexBlending( 0, npc->GetMesh(), FALSE ); 
			npc->CreateShadow();
		}
		else
		if( char_id == CTI_ANIMUS)
		{
			R3EndScene();
			_GetCharIF()->SetState();
			ShadowBeginScene();
			((CBaseNpc *)stShadowList[id[i]].ptr)->GetBoundBox(bbox,TRUE);
			GetScaleAddPos(&bbox,&scale,&add_pos);
			scale*=((CBaseNpc *)stShadowList[id[i]].ptr)->GetScale();
			PrepareShadow(scale,add_pos);
			((CBaseNpc *)stShadowList[id[i]].ptr)->CreateShadow();
			alpha=0.5f;
		}
		else
			continue;
		_GetCharIF()->UnSetState();
		ShadowEndScene();

		Vector3f v_temp;
		stShadowList[id[i]].ptr->GetPosition(v_temp);
		g_pLevel->PrepareShadowRender(v_temp,GetShadowTexture(),alpha,3,scale,add_pos);
		R3BeginScene();
		g_pLevel->DrawShadowRender(v_temp);
	}

	pDevice->SetTransform( D3DTS_VIEW, &GetR3State()->mMatView );
	pDevice->SetTransform( D3DTS_PROJECTION, &GetR3State()->mMatProj );
	if( GetR3State()->mIsFog )
		pDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
	
EFUtil_DrawShadowList_Out:
	stShadowCnt=0;
}

void EFUtil_CharacterHide(void *pl,float alpha)
{
	CPlayer *player = (CPlayer *)pl;
	BBOX bbox;

	player->GetBoundBox(bbox,TRUE);
	float dist = bbox.pMax[1]-bbox.pMin[1]+2;
	Vector3f pos;
	Vector4f c_plane;
	c_plane[0]=0;	c_plane[2]=0;
	player->GetPosition(pos);
	if( alpha > 0 )
	{
		c_plane[1]=-1;
		c_plane[3] = +(pos[1]+dist*alpha);
		pos[1] = pos[1]+dist*alpha;
	}
	else
	{
		c_plane[1]=1;
		c_plane[3] = -(pos[1]-dist*alpha);
		pos[1] = pos[1]-dist*alpha;
	}
	MovePosMagicEffect(player->m_dwMoveUniqueID,pos,player->GetRotY());
	GetD3dDevice()->SetClipPlane( 0, c_plane );
	GetD3dDevice()->SetRenderState( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );
}

void EFUtil_CharacterHideOff()
{
    GetD3dDevice()->SetRenderState( D3DRS_CLIPPLANEENABLE, 0x00 );
}
/*
DWORD EFUtil_GetMaterial(void *character)	//몹이던 캐릭이던 머터리얼을 구한다.
{
	if( !character )
		return _MM_BIO_;	//예외처리.

	CCharacter *ch = (CCharacter *)character;
	if( ch->GetCharTypeID() == CTI_MONSTER )
	{
		CMonster *mob = (CMonster*)ch;
		return mob->GetEffectValue();		//재질로 변환...
	}
	else
	if( ch->GetCharTypeID() == CTI_AVATAR || ch->GetCharTypeID() == CTI_PLAYER )
	{
		CPlayer *pl = (CPlayer *)ch;

		if( pl->GetRace() == CRT_ACCRETIA )
			return _MM_METAL_;
		return _MM_BIO_;
	}
	else
	if( ch->GetCharTypeID() == CTI_UNIT || ch->GetCharTypeID() == CTI_GUARD_TOWER )
	{
		return _MM_METAL_;
	}

	return _MM_BIO_;
}
*/


DWORD EFUtil_GetMobIndex(void *character)	//몹이펙트에서 사용한는 인덱스.
{
	if( !character )
		return 0;	//예외처리.

	CCharacter *ch = (CCharacter *)character;
	if( ch->GetCharTypeID() == CTI_MONSTER )
		return ((CMonster*)ch)->GetEffectValue();		//몹 인덱스..

	return 0;
}


DWORD EFUtil_GetAttackMaterial(void *character)	//몹이던 캐릭이던 무기 머터리얼을 구한다.
{
	if( !character )
		return _M_METAL_;	//예외처리.

	CCharacter *ch = (CCharacter *)character;
	if( ch->GetCharTypeID() == CTI_MONSTER )
	{
		CMonster *mob = (CMonster*)ch;
		return mob->GetAttackEffectMaterial();		//재질로 변환...
	}
	else
	if( ch->GetCharTypeID() == CTI_AVATAR || ch->GetCharTypeID() == CTI_PLAYER )
	{
		CPlayer *pl = (CPlayer *)ch;

		CEquipItem *pio_pItem = pl->GetEquipItem( CEPT_WEAPON );
		if( pio_pItem )
		{
			ITEM_DATA *l_pData= pio_pItem->GetData();
//			if( pio_pItem->IsEquipmentItem() )
//				DWORD temepe = (( EQUIPMENT_DATA * )l_pData )->dwEffectMaterial;
			return (( EQUIPMENT_DATA * )l_pData )->dwEffectMaterial;
		}
		return _M_METAL_;
	}
	else
	if( ch->GetCharTypeID() == CTI_ANIMUS )	//이건 나중에 추가하자.
	{
//		CAnimus *pl = (CAnimus *)ch;
//		return pl->GetAttackEffectMaterial();
	}
	else
	if( ch->GetCharTypeID() == CTI_GUARD_TOWER )
	{
//		return ch->GetAttackEffectMaterial();
	}
	else
	if( ch->GetCharTypeID() == CTI_UNIT )	//이건 나중에 추가하자.
	{
//		return ch->GetAttackEffectMaterial();
	}

	return _M_METAL_;
}

DWORD EFUtil_GetDefenceMaterial(void *character)	//몹이던 캐릭이던 갑옷 머터리얼을 구한다.
{
	if( !character )
		return _M_BIO_;	//예외처리.

	CCharacter *ch = (CCharacter *)character;
	if( ch->GetCharTypeID() == CTI_MONSTER )
	{
		CMonster *mob = (CMonster*)ch;
		return mob->GetDefenceEffectMaterial();		//재질로 변환...
	}
	else
	if( ch->GetCharTypeID() == CTI_AVATAR || ch->GetCharTypeID() == CTI_PLAYER )
	{
		CPlayer *pl = (CPlayer *)ch;

		CEquipItem *pio_pItem = pl->GetEquipItem( CEPT_UPPER_PART );
		if( pio_pItem )
		{
			ITEM_DATA *l_pData= pio_pItem->GetData();
//			if( pio_pItem->IsEquipmentItem() )
//				DWORD temepe = (( EQUIPMENT_DATA * )l_pData )->dwEffectMaterial;
			return (( EQUIPMENT_DATA * )l_pData )->dwEffectMaterial;
		}
		return _M_METAL_;
	}
	else
	if( ch->GetCharTypeID() == CTI_UNIT || ch->GetCharTypeID() == CTI_GUARD_TOWER )
	{
		return _M_METAL_;
	}

	return _M_BIO_;
}



BOOL EFUTIL_GetPosCharFromBone(Vector3f *get, CCharacter *ch,char *name)	//본이나 오즈젝트 이름 원하는 위치를 얻어낸다.
{
	CHARACTEROBJECT *pObj = ch->GetBone();
	if( !pObj )
		return FALSE;

	DWORD		MaxObj		= pObj->m_MaxObj;
	ObjectMesh	*pMesh		= pObj->m_pMesh;
	D3DXMATRIX yrot,ytemp;
	D3DXVECTOR3 ppp3;
	D3DXVECTOR4 pOut[2];
	float scale;

	if( !pMesh )
		return FALSE;
	for (int i=0;i<MaxObj;i++)
	{
		if(!strcmp(name,pMesh[i].m_Name))
		{
/*
			get[0][0] = -pMesh[i].m_MatResult.m[3][0];
			get[0][1] = pMesh[i].m_MatResult.m[3][2];
			get[0][2] = -pMesh[i].m_MatResult.m[3][1];
*/
			scale = ch->GetScale();
			ppp3.x=0;
			ppp3.y=0;
			ppp3.z=0;
			MatrixRotate(yrot.m,0,0,360-ch->GetRotY());
			ytemp = pMesh[i].m_MatResult*yrot;
			D3DXVec3Transform(&pOut[0],&ppp3,&ytemp);
			get[0][0] = ch->GetPosX()-pOut[0].x*scale;
			get[0][1] = ch->GetPosY()+pOut[0].z*scale;
			get[0][2] = ch->GetPosZ()-pOut[0].y*scale;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL EFUTIL_GetPosCharFromPos(Vector3f *get, CCharacter *ch,Vector3f pos)	//본이나 오즈젝트 이름 원하는 위치를 얻어낸다.
{
	Vector3f temp;
	float mat[4][4];

	MatrixRotateY(mat,ch->GetRotY());
	Vector3fTransform(temp,pos,mat);
	get[0][0]=ch->GetPosX()+temp[0];
	get[0][1]=ch->GetPosY()+temp[1];
	get[0][2]=ch->GetPosZ()+temp[2];
	return TRUE;
}

static CEquipItem *GetEquipItem(CCharacter *ch)
{
	CEquipItem *item;
	if( ch->GetCharTypeID() == CTI_AVATAR || ch->GetCharTypeID() ==CTI_PLAYER )
	{
		item = ((CPlayer*)ch)->GetEquipItem( IEPT_WEAPON );
		if( !item )
			return NULL;
	}
	else
		return NULL;
	return item;
}

BOOL EFUTIL_GetPosType(Vector3f pos, CCharacter *ch, BYTE type)	//타잎별 포지션을 챙긴다.
{
	if( ch == NULL )
		return FALSE;
	CEquipItem *item;
	BBOX bbox;
	Vector3f temp;

	ch->GetPosition(pos);
	switch(type)
	{
	case _POS_TYPE_BOUNDINGBOX:		//바운드 박스 일정끝.
		ch->GetBoundBox(bbox);
		pos[1] += (bbox.pMax[1]-bbox.pMin[1])*ch->GetScale();
		break;
	case _POS_TYPE_DAMAGE2:		//damage2
		EFUtil_GetDummyPos( pos, ch,_DUMMY_DAMAGE2);
		break;
	case _POS_TYPE_R_HAND:		// 오른손.
		EFUTIL_GetPosCharFromBone(&temp,ch,"Bip01 R Hand");
		Vector3fCopy(temp,pos);
		break;
	case _POS_TYPE_L_HAND:		// 왼손.
		EFUTIL_GetPosCharFromBone(&temp,ch,"Bip01 L Hand");
		Vector3fCopy(temp,pos);
		break;
	case _POS_TYPE_EFFECT1:		//무기의 effect1 =_DUMMY_TRACE1
		item = GetEquipItem(ch);
		if( item )
			EFUtil_GetCharTransformFromIndex(&temp,ch,item,_DUMMY_TRACE1);
		Vector3fCopy(temp,pos);
		break;
	case _POS_TYPE_EFFECT2:		//무기의 effect1 =_DUMMY_TRACE1
		item = GetEquipItem(ch);
		if( item )
			EFUtil_GetCharTransformFromIndex(&temp,ch,item,_DUMMY_TRACE2);
		Vector3fCopy(temp,pos);
		break;
	case _POS_TYPE_FIRE1:		//무기의 fire1 = _DUMMY_FIRE1
		item = GetEquipItem(ch);
		if( item )
			EFUtil_GetCharTransformFromIndex(&temp,ch,item,_DUMMY_FIRE1);
		Vector3fCopy(temp,pos);
		break;
	case _POS_TYPE_FIRE2:		//무기의 fire2
		item = GetEquipItem(ch);
		if( item )
			EFUtil_GetCharTransformFromIndex(&temp,ch,item,_DUMMY_FIRE2);
		Vector3fCopy(temp,pos);
		break;
	case _POS_TYPE_BOUNDINGBOX_PLUS:
		ch->GetBoundBox(bbox);
		pos[1] += (bbox.pMax[1]-bbox.pMin[1])*ch->GetScale()*1.3f;
		break;
	case _POS_TYPE_R_TOE:		// 오른발.
		EFUTIL_GetPosCharFromBone(&temp,ch,"Bip01 R Toe0");
		Vector3fCopy(temp,pos);
		break;
	case _POS_TYPE_L_TOE:		// 왼발.
		EFUTIL_GetPosCharFromBone(&temp,ch,"Bip01 L Toe0");
		Vector3fCopy(temp,pos);
		break;
	}
	return TRUE;
}


void EFUTIL_RunEnterDungeonCamera(Vector3f point)
{
	Vector3f get_cam_pos;
	_R3ENGINE_STATE *state=GetR3State();
	float m_speed=550 + (2.5-stMotionBlurTime)*4;

	D3DXVECTOR3 from;
	D3DXVECTOR3 up    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

//	EFUtil_GetDummyPos( point, _GetAvatar(),_DUMMY_DAMAGE2);
//----------------------------
	point[1] += 15;
	Vector3fCopy(point,((float*)from));
	
	from[0] += cos(m_speed*0.5f*0.3f)*60;
	from[1] += cos(m_speed*0.4f*0.3f)*20+10;
	from[2] += sin(m_speed*0.3f*0.3f)*30;
	
	from.y = point[1]-(from.y-point[1]);

	Vector3f c_get;
	if( g_pLevel &&  g_pLevel->mBsp->IsCollisionFace(point,from,&c_get,3) )	//평면과의 떨어진 거리를 리턴
		Vector3fCopy(c_get,from);

	D3DXMatrixLookAtLH( &state->mMatView, &from, (D3DXVECTOR3*)point, &up );
    D3DXMatrixInverse( &state->mInvMatView, NULL, &state->mMatView );

    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_VIEW, &state->mMatView );
	
	get_cam_pos[0] = state->mInvMatView.m[3][0];
	get_cam_pos[1] = state->mInvMatView.m[3][1];
	get_cam_pos[2] = state->mInvMatView.m[3][2];
//	SetPointCamera(get_cam_pos);

	MatrixMultiply( state->mMatViewProj.m ,state->mMatProj.m,state->mMatView.m);
}



static void EFUtil_MakeViewMatrix(D3DXMATRIX *mat,Vector3f pos,float th,float ph,float dist)		//모든 값을 지정한후에 한번호출해줘해야한다.  내부에서 pos값과 뷰매트릭스를 생성한다.
{
	/*
	if(th<-89)
		th =-89;
	if(th> 89)
		th = 89;
*/
	ph = (ph*3.141592f)/(360.0f/2.0f);
	th =  -(th*3.141592f)/(360.0f/2.0f);

	D3DXMATRIX temp1,temp2,temp3;

    D3DXMatrixIdentity( &temp3 );
    D3DXMatrixIdentity( &temp2 );
	temp2.m[3][0]=-pos[0];
	temp2.m[3][1]=-pos[1];
	temp2.m[3][2]=-pos[2];
	temp3 = temp3*temp2;
	D3DXMatrixRotationY(&temp1, ph );
	D3DXMatrixRotationX(&temp2, th );
	temp3 = temp3*temp1;
	temp3 = temp3*temp2;
	
    D3DXMatrixIdentity( &temp2 );
	temp2.m[3][2]=dist;

	*mat = temp3*temp2;
}

typedef struct{
	Vector3f pos;
	float th,ph,dist,d_th;
}_TEMP_CAMERA;
static _TEMP_CAMERA stTempCamera;
static _TEMP_CAMERA stOrgCamera,stOrgCamera2;
static float stTempCameraStep=0;

void EFUTIL_ViewTopCamera()
{
	_R3ENGINE_STATE *state=GetR3State();
	R3Camera *cam = g_pCamera->GetR3Camera();
	EFUtil_MakeViewMatrix( &state->mMatView, stTempCamera.pos,cam->GetTh(),cam->GetPh(),stTempCamera.dist );
//	EFUtil_MakeViewMatrix( &state->mMatView, stTempCamera.pos,69,0,stTempCamera.dist );
//	g_pCamera->SetYaw(69+180);
//	g_pCamera->SetPitch(0+180);

    D3DXMatrixInverse( &state->mInvMatView, NULL, &state->mMatView );

    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_VIEW, &state->mMatView );
	MatrixMultiply( state->mMatViewProj.m ,state->mMatProj.m,state->mMatView.m);
}

#define TOP_VIEW_HEIGHT		600	//60미터 위로 올라간다.

static float TOP_HEIGHT; 
//#define TOP_HEIGHT	700	//70미터 위로 올라간다.
#define TOTAL_TIME	2.0f	//총이동 시간.

BOOL EFUTIL_CommonTopCamera(Vector3f s_pos,float s_th,float s_ph,float s_dist, Vector3f d_pos,float d_th,float d_ph,float d_dist,float ab_time)
{
	Vector3f pos;
	float rot_time,mov_time;
	BOOL ret = TRUE;
	float ph,th,dist;

	mov_time = sin((ab_time*(3.141592f/2.0f)));
//	mov_time = ab_time;
	rot_time = 1-cos((ab_time*(3.141592f/2.0f)));
	//스텝에 따른 중간 값을 구한다.
	pos[0] = (d_pos[0] - s_pos[0]) * mov_time + s_pos[0];
	pos[1] = (d_pos[1] - s_pos[1]) * mov_time + s_pos[1];
	pos[2] = (d_pos[2] - s_pos[2]) * mov_time + s_pos[2];
	th = (d_th - s_th) * mov_time + s_th;
	ph = (d_ph - s_ph) * mov_time + s_ph;
	dist = (d_dist - s_dist) * mov_time + s_dist;

	g_pCamera->SetYaw(th);
	g_pCamera->SetPitch(ph);

	stTempCamera.ph = ph;
	stTempCamera.th = th;
	stTempCamera.dist = dist;
	Vector3fCopy( pos,stTempCamera.pos );

	EFUTIL_ViewTopCamera();
	return ret;
}

BOOL EFUTIL_GotoTopCamera()
{
	Vector3f d_pos,s_pos;
	float s_ph,s_th,s_dist;
	float d_ph,d_th,d_dist;
	BOOL ret = TRUE;

	//원래 위치 
	Vector3fCopy(stOrgCamera.pos,s_pos);
	s_pos[1] += g_pCamera->GetAddY();
	s_th = stOrgCamera.th;
	s_ph = stOrgCamera.ph;
	s_dist = stOrgCamera.dist;

	//가고자하는곳.
	Vector3fCopy(stOrgCamera.pos,d_pos);
	d_pos[1] += TOP_HEIGHT;		
//	d_th = g_pCamera->GetYaw();
//	d_ph = g_pCamera->GetPitch();
	d_th = 69;
	d_ph = s_ph-110;
	d_dist = 0.1f + stOrgCamera2.dist*(1-stTempCameraStep/TOTAL_TIME);
/*
	stTempCameraStep += R3GetLoopTime();
	ab_time = stTempCameraStep/TOTAL_TIME;
	if( ab_time > 1 )
	{
		ab_time=1;
		ret = FALSE;
	}
	mov_time = sin((ab_time*(3.141592f/2.0f)));
	rot_time = 1-cos((ab_time*(3.141592f/2.0f)));
	//스텝에 따른 중간 값을 구한다.
	pos[0] = (d_pos[0] - s_pos[0]) * mov_time + s_pos[0];
	pos[1] = (d_pos[1] - s_pos[1]) * mov_time + s_pos[1];
	pos[2] = (d_pos[2] - s_pos[2]) * mov_time + s_pos[2];
	th = (d_th - s_th) * mov_time + s_th;
	ph = (d_ph - s_ph) * mov_time + s_ph;
	dist = (d_dist - s_dist) * mov_time + s_dist;

	g_pCamera->SetYaw(th);
	g_pCamera->SetPitch(ph);

	stTempCamera.ph = ph;
	stTempCamera.th = th;
	stTempCamera.dist = dist;
	Vector3fCopy( pos,stTempCamera.pos );

	EFUTIL_ViewTopCamera();
*/
	stTempCameraStep += R3GetLoopTime();
	float ab_time = stTempCameraStep/TOTAL_TIME;
	if( ab_time > 1 )
	{
		ab_time=1;
		ret = FALSE;
	}

	EFUTIL_CommonTopCamera(s_pos,s_th, s_ph, s_dist,d_pos,d_th, d_ph, d_dist,ab_time);
	return ret;
}

BOOL EFUTIL_ReturnTopCamera()
{
	Vector3f d_pos,s_pos;
	float s_ph,s_th,s_dist;
	float d_ph,d_th,d_dist;
	BOOL ret = TRUE;

	//가고자하는곳 
	Vector3fCopy(stOrgCamera2.pos,d_pos);
	d_pos[1] += +g_pCamera->GetAddY();		
	d_th = stOrgCamera.th;
	d_ph = stOrgCamera.ph;
	d_dist = stOrgCamera.dist;

	//지금 위치.
	Vector3fCopy(stOrgCamera.pos,s_pos);
	s_pos[1] += TOP_HEIGHT;		
	s_th = stOrgCamera2.th;
	s_ph = stOrgCamera2.ph;
	s_dist = 0.1f+stOrgCamera2.dist*(1-stTempCameraStep/TOTAL_TIME);

	stTempCameraStep -= R3GetLoopTime();
	float ab_time = stTempCameraStep/TOTAL_TIME;
	if( ab_time < 0 )
	{
		ab_time=0;
		ret = FALSE;
	}
	EFUTIL_CommonTopCamera(s_pos,s_th, s_ph, s_dist,d_pos,d_th, d_ph, d_dist,1-ab_time);
	return ret;
}

void EFUTIL_SpecialCamera()			//죽었을때,전투던젼카메라처럼 특수카메라제어시 
{

	if( gAsci[VK_K] )
	{
		gAsci[VK_K]=0;
#ifdef _DEVELOPER_
		if( !_GetMainGame()->IsChatMode() )			
#else
		if( !_GetMainGame()->IsChatMode() && !_GetDungeonInfo()->IsInDungeon())
#endif
		{
			if( _GetCamera()->GetSpecialCameraMode() == 9 || _GetCamera()->GetSpecialCameraMode() == 7)	// 꼭대기에서 돌아보는 카메라워크...
			{
				if( _GetCamera()->GetSpecialCameraMode() == 9 )
				{
					stTempCameraStep=1*TOTAL_TIME;
				}
					g_pCamera->GetPosition(stOrgCamera2.pos);
					stOrgCamera2.th = g_pCamera->GetYaw();
					stOrgCamera2.ph = g_pCamera->GetPitch();
					stOrgCamera2.dist = g_pCamera->GetDistance();
				//}
				_GetCamera()->SetSpecialCameraMode(8);
			}
			else
			{
				if( _GetCamera()->GetSpecialCameraMode() <= 6 )
				{
					stTempCameraStep=0;
					g_pCamera->GetPosition(stOrgCamera.pos);
					stOrgCamera.th = g_pCamera->GetYaw();
					stOrgCamera.ph = g_pCamera->GetPitch();
					stOrgCamera.dist = g_pCamera->GetDistance();
				}
				TOP_HEIGHT=TOP_VIEW_HEIGHT;
				//충돌 검사 
				if( g_pLevel && g_pLevel->mBsp->IsLoaded() )
				{
					Vector3f src,tar,get;
					g_pCamera->GetPosition(src);
					Vector3fCopy(src,tar);
					tar[1]+=700;
					if( g_pLevel->mBsp->IsCollisionFace( src,tar,&get,15) )
					{
						TOP_HEIGHT=get[1]-src[1];
					}
				}				
				_GetCamera()->SetSpecialCameraMode(7);
			}
		}
	}

	if( _GetCamera()->GetSpecialCameraMode() == 5 )	// 죽었을 때 카메라워크...
	{
		Vector3f pos;
		_GetAvatar()->GetPosition(pos);
		pos[1]+=3;
		EFUTIL_RunDeathCamera(pos);
	}
	else
	if( _GetCamera()->GetSpecialCameraMode() == 6 )	// 전투 던젼 들어갈때 카메라워크...
	{
		Vector3f pos;
		_GetAvatar()->GetPosition(pos);
		EFUTIL_RunEnterDungeonCamera(pos);
	}
	else
	if( _GetCamera()->GetSpecialCameraMode() == 7 )	// 꼭대기로 올라가는 카메라워크...
	{
		SetStateFullScreenEffect(_FSE_MOTION_BLUR);
		if( !EFUTIL_GotoTopCamera() )
		{
			_GetCamera()->SetSpecialCameraMode(9);
			SetStateFullScreenEffect(FALSE);
			_GetCamera()->SetRotatedByKeyboard(TRUE);
		}
	}
	else
	if( _GetCamera()->GetSpecialCameraMode() == 8 )	// 다시 되돌아 오는 카메라워크...
	{
		SetStateFullScreenEffect(_FSE_MOTION_BLUR);
		if( !EFUTIL_ReturnTopCamera() )
		{
			_GetCamera()->SetSpecialCameraMode(0);
			SetStateFullScreenEffect(FALSE);
			_GetCamera()->SetRotatedByKeyboard(TRUE);
		}
	}
	else
	if( _GetCamera()->GetSpecialCameraMode() == 9 )	// 꼭대기에서 돌아보는 카메라워크...
	{
//		stTempCamera.pos[1]-=g_pCamera->GetAddY();
		EFUTIL_ViewTopCamera();
//		stTempCamera.pos[1]+=g_pCamera->GetAddY();
	}
	else
	{
		stTempCameraStep=0;
		TOP_HEIGHT=TOP_VIEW_HEIGHT;
	}
}

DWORD EFUTIL_GetSoundMat(DWORD attack,DWORD defence)
{
	if( attack == 2 || attack == 5 )	//베기와 때르기는 합치려구 
		attack--;
	DWORD i,bit=1;
	DWORD defence_add;
	DWORD attr[16];
	DWORD rand_cnt=0;

	for(i=0; i<16; i++)
	{
		if( defence & bit )
			attr[rand_cnt++] = i;
		bit<<=1;
	}
	if( rand_cnt==0 )
	{
		attr[0] = 0;
		rand_cnt = 1;
	}
	defence_add = attr[rand()%rand_cnt]*3;	//각각의 재질에 대한 렌덤 적용...
	defence_add += rand()%2;		//렌덤 적용
	return attack*50+defence_add;
}

DWORD EFUTIL_GetRandomMat(DWORD id)
{
	DWORD i,bit=1;
	DWORD defence_add;
	DWORD attr[16];
	DWORD rand_cnt=0;

	for(i=0; i<16; i++)
	{
		if( id & bit )
			attr[rand_cnt++] = i;
		bit<<=1;
	}
	if( rand_cnt==0 )
	{
		attr[0] = 0;
		rand_cnt = 1;
	}
	defence_add = attr[rand()%rand_cnt];	//각각의 재질에 대한 렌덤 적용...
	return defence_add;
}

BOOL EFUTIL_GetPosTypeFromChar(Vector3f pos,void *ch, DWORD unique_id)
{
	CMagicList *magic = GetMagicListFromID(unique_id);
	BOOL ret=FALSE;
	((CCharacter *)ch)->GetPosition(pos);
	if( magic )
	{
		Vector3f temp;
		CProcessMagic *pMagic = (CProcessMagic *)magic->mMagicPtr;
		if( pMagic )
		{
			if( EFUTIL_GetPosType(temp, (CCharacter *)ch, pMagic->mCreateType))	//타잎별 포지션이 있으면 변경한다.
			{
				ret=TRUE;
				Vector3fCopy(temp,magic->mMat[3]);
				Vector3fCopy(temp,pos);
			}
		}
	}
	return ret;
}

DWORD StartMagicEffectType(CCharacter *ch,Vector3f pos,float y_angle, DWORD magic_id,void *e_Info,DWORD state)
{
	DWORD unique_id = StartMagicEffect(pos,y_angle,magic_id,e_Info,state);
	CMagicList *magic = GetMagicListFromID(unique_id);
	if( magic )
	{
		Vector3f temp;
		CProcessMagic *pMagic = (CProcessMagic *)magic->mMagicPtr;
		if( pMagic )
		{
			if( EFUTIL_GetPosType(temp, ch, pMagic->mCreateType))	//타잎별 포지션이 있으면 변경한다.
			{
				Vector3fCopy(temp,magic->mMat[3]);
			}
		}
	}
	return unique_id;
}

DWORD StartMagicEffectType(CCharacter *ch,float mat[4][4], DWORD magic_id,void *e_Info,DWORD state)
{
	DWORD unique_id = StartMagicEffect(mat,magic_id,e_Info,state);
	CMagicList *magic = GetMagicListFromID(unique_id);
	if( magic )
	{
		Vector3f temp;
		CProcessMagic *pMagic = (CProcessMagic *)magic->mMagicPtr;
		if( pMagic )
		{
			if( EFUTIL_GetPosType(temp, ch, pMagic->mCreateType))	//타잎별 포지션이 있으면 변경한다.
			{
				Vector3fCopy(temp,magic->mMat[3]);
			}
		}
	}
	return unique_id;
}




void EFUtil_PushEffectValue(EffectInfo *info,BOOL state)
{
	info->dwValue = state;
	_GetReturnEffectInfoList()->push_back( info );


}
