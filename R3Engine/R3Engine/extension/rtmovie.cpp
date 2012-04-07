#include "RTMovie.h"
#include "jmalloc.h"
#include "commonutil.h"
#include "effectcore.h"
#include "R3Sound.h"
#include "R3Math.h"
#include "CharacterObj.h"
#include "ParticleList.h"

#ifdef RTMOVIE

static ChInterface			*g_chIF	= NULL;	//메인인터페이스..
//static TextureManager		*g_texture		= NULL;
static AnimationManager		*g_animation	= NULL;
static CObjectManager		*g_om_body		= NULL;
static char TempPath[48];
static CParticleManager_		*g_particle = NULL;


static CLevel *stLevel;

/*
//--------텍스쳐 관련 유틸 애니..
static void LoadTextureInDir(char *name)
{
	int start = strlen(name);
	while(name[start]!='\\'){start--;}start++;

	g_texture->AddTexture(TempPath,&name[start]);
	
}*/

static void FileFindAndProcess( char *fdir,void (*func)( char * ))
{
	char buf[256];
	char dir[256];
	sprintf(buf,"%s\\*.*",fdir);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	//hFind = FindFirstFile(".\\HEAD\\*", &FindFileData);
	hFind = FindFirstFile(buf , &FindFileData);
	int bWorking = true;
	if (hFind!=INVALID_HANDLE_VALUE)
	{
		while (bWorking)
		{
			bWorking = FindNextFile(hFind,&FindFileData);
			if (!bWorking) break;

			if (strcmp(".",FindFileData.cFileName)==0||strcmp("..",FindFileData.cFileName)==0)
			{
				continue;
			}
			if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				sprintf(dir,"%s\\%s",fdir,FindFileData.cFileName);
				FileFindAndProcess(dir,func);
			}
			else
			{
				sprintf(dir,"%s\\%s",fdir,FindFileData.cFileName);
				func(dir);
			}
		}
		FindClose(hFind);
	}
}
#define _RENDER_DIST (340-40)
#define _MAX_HEAR_DIST (_RENDER_DIST/4.0f)

static float GetVolume(Vector3f pos)
{
	Vector3f cam_pos,temp;

	::GetCameraPos(&cam_pos);
	Vector3fSub(pos,cam_pos,temp);
	float dist = Vector3fDist(temp);	//몬스터와의 거리.
	float density = max((dist-_MAX_HEAR_DIST),0);
	density = 1-density/(_RENDER_DIST-_MAX_HEAR_DIST);

	return density;
}
static float GetPan(Vector3f src)	//맵상의 위치하나로 효과음에 쓰일 팬값을 구한다.  -1은 왼쪽에치우침.1은 오른쪽에 치우침.
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
static void Play3DSound(DWORD id,Vector3f pos)
{
	float vol = GetVolume(pos);
	if( vol <= 0 )
		return;
	float pan = GetPan(pos);	//맵상의 위치하나로 효과음에 쓰일 팬값을 구한다.  -1은 왼쪽에치우침.1은 오른쪽에 치우침.
//	::StopWave(id);
	::IM_PlayWave(id,vol,pan);
}



#define _MAX_ACTION	6

class RTMCharClass{
public:
	DWORD mFlag;					// 캐릭터들 특수 플래그들..
	DWORD mNowAction;
	DWORD mActionCnt;
	DWORD mColor;
	float mFrame;
	float mScale;					//캐릭터 스케일값.
	DWORD mMagicUniqueID;			// 매직 다루는 아이디.
	Vector3f mPos;					// 포지션(충돌 쳌크시 사용하는거..)
	DWORD mMeshID;					// 이펙트용 메쉬 아이디.
	Vector3f mBBmin,mBBmax;			// 바운딩 박스
	BOOL mIsInFrustum;				// 캐릭터가 뷰프러스텀에 들어 왓는가?
	
	//페이드 루틴.
	float mFadeTime;
	union {
	float mStartTick;
	DWORD mMagicID;
	};
	float mFadeAlpha;

	DWORD mDummyID;		//링크된 더미아이디.
	CHARACTEROBJECT			*g_bn;//*mBone;		//본이닷.
	CHARACTEROBJECT			*mMesh;		//메쉬

	char mCharName[32];		//캐릭터 이름.
	char mMeshName[48];		//메쉬 이름.
	char mBoneName[48];		//뼈 이름.
	char mTexPath[48];		//텍스쳐 패쓰 이름.
	char mAniName[_MAX_ACTION][48];
	float mMat[4][4];		//사용 매트릭스 

	RTMCharClass();
	~RTMCharClass();

	BOOL LoadMesh();
	void Release();

	void FrameMove();
	void Render();

	BOOL IsInViewFrustum()		{	return mIsInFrustum;	};	//캐릭터가 뷰프러스텀에 들어 왓는가?
	void CheckViewFrustum();		//뷰프러스텀 첵크
	void SetAnimation(DWORD id);
	void AddAnimation(char *ani_name);	//애니메이션 초기화할때...
	void SetFadeOut(float time);
	void SetFadeIn(float time);
	void UpdateFadeLoop();
};

RTMCharClass::RTMCharClass()
{
	memset(this,0,sizeof(RTMCharClass));
	mScale=1;
}
RTMCharClass::~RTMCharClass()
{

}

void RTMCharClass::CheckViewFrustum()
{
	if( !stLevel || mFlag & _RTM_CHAR_FLAG_MAGIC )
	{
		mIsInFrustum = FALSE;
		return;
	}
	Vector3f bbmin,bbmax;

	Vector3fCopy(mBBmin,bbmin);
	Vector3fCopy(mBBmax,bbmax);
	Vector3fScale(bbmin,mScale,bbmin);
	Vector3fScale(bbmax,mScale,bbmax);
	Vector3fAdd(bbmin,mMat[3],bbmin);
	Vector3fAdd(bbmax,mMat[3],bbmax);

	mIsInFrustum = IsBBoxInFrustum(bbmin,bbmax);
}

BOOL RTMCharClass::LoadMesh()
{
	strcpy(TempPath,mTexPath);
//	FileFindAndProcess(mTexPath,LoadTextureInDir);

	g_chIF->SetTexPath(mTexPath);
	g_chIF->LoadBoneData(mBoneName,false);

	g_bn  = g_chIF->GetBoneData( mBoneName );
	g_chIF->LoadMeshData(g_om_body ,mMeshName,true,g_bn);
	mMesh  = g_chIF->GetMeshData(g_om_body, mMeshName );

	char temp[256];
	strncpy(temp,mBoneName,254);
	StripEXT(temp);
	strcat(temp,".BBX");
	if( !g_chIF->ImportBoundBox( temp, mBBmin, mBBmax ) )
	{
		mBBmin[0]=-10;		mBBmin[1]=-10;		mBBmin[2]=-10;
		mBBmax[0]=10;		mBBmax[1]=10;		mBBmax[2]=10;
	}
	return TRUE;
}

void RTMCharClass::SetAnimation(DWORD id)
{
	char buf[256];

	mFrame=0;
	if( !mActionCnt )
		return;
	if( mActionCnt < id-1 )
		id = mActionCnt-1;

	if( !g_bn )
		return;
	g_chIF->AnimationReset(g_bn);
	sprintf(buf,"%s",mAniName[id]);
	ChAnimation		*ani = g_chIF->GetAnimationData( g_animation, buf );
	g_chIF->MatchAnimationToMesh(ani,g_bn );
}

void RTMCharClass::AddAnimation(char *ani_name)
{
	if( strlen(ani_name) >= 48)
	{
		Warning(ani_name," <- 패스와 파일이름이 48자가 넘습니다.\n");
		return;
	}
	strcpy(mAniName[mActionCnt],ani_name);
	g_chIF->AddAnimationData(g_animation,mAniName[mActionCnt]);
}
void RTMCharClass::SetFadeIn(float time)
{
	mFadeTime = time+0.0001f;
	mStartTick = R3GetTime();
}

void RTMCharClass::UpdateFadeLoop()
{
	mColor &= 0x00ffffff;
	mColor |= ((DWORD)((1-mFadeAlpha)*255)<<24);
	if( mFadeAlpha== 0)	//페이드 인 상태다 
		return;

	if( mFadeTime > 0 )	//페이드인.
	{
		float tick = (R3GetTime()-mStartTick+0.0001f)/mFadeTime;
		if( tick > 1 )//1넘어가면 끝.
		{
			tick=1;
			mFadeAlpha=0;
			mFadeTime=0;
		}
		else
			mFadeAlpha = 1-tick;
	}
	else
	if( mFadeTime < 0 )	//페이드 아웃.
	{
		float tick = (R3GetTime()-mStartTick+0.0001f)/-mFadeTime;
		if( tick > 1 )//1넘어가면 끝.
		{
			tick=1;
			mFadeAlpha=1;
			mFadeTime=0;
		}
		else
			mFadeAlpha = tick;
	}
	else	//페이드 인이나 페이드 아웃된 상태...
	{
	}
}

void RTMCharClass::SetFadeOut(float time)
{
	mFadeTime = -time-0.0001f;
	mStartTick = R3GetTime();
	mFadeAlpha=0.0001f;
}

#define _MAX_TRACK	128

RTMCharClass *stCh=NULL;
static DWORD stRTCharNum=0;
DWORD stCharMaxCnt=64;



#define _TRACK_INDEPENDANT_CAMERA	0x80	//카메라와 상관없는 명령어 

#define _TRACK_CAMERA				1
#define _TRACK_FADE_IN				2
#define _TRACK_FADE_OUT				3
#define _TRACK_ANI					(4|_TRACK_INDEPENDANT_CAMERA)
#define _TRACK_QUAKE				5
#define _TRACK_CHAR_FADE_IN			(6|_TRACK_INDEPENDANT_CAMERA)
#define _TRACK_CHAR_FADE_OUT		(7|_TRACK_INDEPENDANT_CAMERA)
#define _TRACK_MAGIC				(8|_TRACK_INDEPENDANT_CAMERA)	//매직 스크립트를 공략한다.
#define _TRACK_WAVE					(9|_TRACK_INDEPENDANT_CAMERA)		//효과음.

typedef struct{ 
	union{
		DWORD mdwArgv[2];
		float mfArgv[2];
	};
	BYTE mWhat;	//어떤종류인지.	
	float mFrame;	//몇 프레임인지..
}CRTMovieTrack;

CRTMovieTrack *stTrack=NULL;
DWORD stTrackCnt=0;
DWORD stNowTrack=0;
DWORD stTrackMaxCnt=128;
DWORD stTotalFrame=0;

#define _RTM_FLAG_NO_CAMERA		0x1	//카메라 모드를 안쓸경우.
#define _RTM_FLAG_WIDE			0x2	//와이드 뷰이다.
#define _RTM_FLAG_LOGO			0x4	//로고닷.

static DWORD stRTMovieFlag=0;
static BOOL stIsLoadedRTMovie=FALSE;

BOOL IsLoadedRTMovie()	//rt동영상 로딩되었는가? 
{
	return stIsLoadedRTMovie;	
}
BOOL IsLoadedRTCamera()	//rt동영상 로딩되었는가? 
{
	if( !stLevel )
		return FALSE;

	return stLevel->mAutoAniCam.IsLoadedAniCamera();
}

void RTMovieRelease()
{
	if( !stLevel )
		return;
	if( stLevel->mAutoAniCam.IsLoadedAniCamera() )	//카메라가 로딩 되었음.
		stLevel->mAutoAniCam.ReleaseAniCamera();	//카메라 아웃.

	if( !stIsLoadedRTMovie )
		return;

	/*
	DWORD i;
	for(i=0; i<stRTCharNum; i++)
	{
		g_chIF->FrameStep(stCh->g_bn);
		g_chIF->FrameMove(stCh->g_bn);
		g_chIF->FrameMove(stCh->mMesh);
	}*/
	SAFE_DELETE( g_chIF );
//	SAFE_DELETE( g_texture );
	SAFE_DELETE( g_animation );
	if(g_particle)
	{
		g_particle->ReleaseEntity();
		g_particle->ReleaseParticle();
	}
	SAFE_DELETE( g_particle );

	IM_ReleaseAllWaves();
	if( stTrack )
		Dfree(stTrack);
	stTrack=0;
	if( stCh )
		Dfree(stCh);
	stCh=0;
	stTrackCnt=0;
	stNowTrack=0;
	stTrackMaxCnt=128;
	stCharMaxCnt=64;
	stRTMovieFlag=0;
	stRTCharNum=0;
	stTotalFrame=0;
	stLevel=0;

	DestroyEffectsList();
	DestroyMagicSptList();
	ResetEffectManager();	//이펙트 메니져들을 리셋한다.
}

DWORD GetCameraID(char *name)
{
	DWORD i;
	char buf[256],*temp;

	for(i=0; i<stLevel->mAutoAniCam.GetCameraNum();i++)
	{
		temp = stLevel->mAutoAniCam.GetCameraName(i);	//카메라 이름을 리턴한다. 존재치않으면 NULL
		if(temp)
		{
			strcpy(buf,temp);
			strlwr(buf);
			if( !strcmp(buf,name ))
				return i;
		}
	}
	return -1;
}

void ExtractFrame(float frame);


void PrepareRTMovie()
{
	DWORD i;

	for( i=0; i<stRTCharNum; i++)
	{
		if( stCh[i].mFlag & _RTM_CHAR_FLAG_MAGIC )	//마법이라서 통과.
			continue;
		if( stCh[i].mFlag & _RTM_CHAR_FLAG_COLLISION )	//충돌이라서 처음좌표를 구하자.
		{			
			Vector3f fmin,fmax;
			stLevel->mAutoAniCam.GetDummyMatrix(stCh[i].mMat,stCh[i].mDummyID,0);
			Vector3fCopy(stCh[i].mMat[3],fmin);
			Vector3fCopy(stCh[i].mMat[3],fmax);
			Vector3fCopy(stCh[i].mMat[3],stCh[i].mPos);
			fmin[1] -= 150;
			fmax[1] += 150;
			stCh[i].mPos[1] = stLevel->GetFirstYpos(stCh[i].mPos,fmin,fmax);
		}
	}
	DestroyEffectsCache();	//효과캐쉬 없앤다.

	float cam_mat[4][4];
	stNowTrack=0;
	stLevel->mAutoAniCam.SetPrePlayCamera(0);
	ExtractFrame(0);
	stLevel->mAutoAniCam.PlayAniCamera(cam_mat,1.0f);
}
void OneTimeSceneRTMovie()	//캐릭터 로딩이라든지 첨 로딩하는거..
{
	//-----캐릭터들 정리..
	DWORD i,j;

	g_om_body  = g_chIF->GetPart("BODY");

	for( i=0; i<stRTCharNum; i++)
	{
		if( stCh[i].mFlag & _RTM_CHAR_FLAG_MAGIC )	//마법이라서 통과.
			continue;
		stCh[i].LoadMesh();
		for(j=0; j<stCh[i].mActionCnt; j++)
		{
			stCh[i].AddAnimation(stCh[i].mAniName[j]);
		}
	}
/*
	//-------시작 준비를 하자....	0프래임에 관련된것들은 모조리 셋팅..
	ExtractFrame(0);

	float cam_mat[4][4];
	stLevel->mAutoAniCam.PlayAniCamera(cam_mat,1.0f);
*/
	PrepareRTMovie();
}

static DWORD GetCharID(char *ch)
{
	DWORD i;

	for( i=0; i<stRTCharNum; i++ )
	{
		if( stCh[i].mFlag & _RTM_CHAR_FLAG_MAGIC )	//마법이라서 통과.
			continue;
		if( !strcmp( stCh[i].mCharName,ch ))
			return i;
	}
	return -1;
}
//-------------------RTM 외부 인터페이스 
static BOOL stRTMPlay=0;	//플레이 하는게 폴스다
static BOOL stRTMPause=0;	//잠시 멈춤.
static DWORD stRTMState=0;

void RTMovieStartPlay(BOOL is_beginning)	// TRUE는 처음부터 FALSE는 멈춘때부터..
{
	if( stLevel == NULL )
		return;

	if( stLevel->mAutoAniCam.IsLoadedAniCamera() == FALSE ) 
		return;

	stRTMPlay=TRUE;

	if( stIsLoadedRTMovie == FALSE)
		return;
	if( is_beginning )
	{
		PrepareRTMovie();
	}
}

BOOL IsRTMoviePlaying()	//무비가 플레이도중인가?
{
	return stRTMPlay;
}
void RTMoviePause(BOOL pause)	//멈출려면 TRUE 다시 작동시킬려면 FALSE
{
	stRTMPause=pause;
}

void RTMovieSetState(DWORD state)
{
	stRTMState = state;	
	if( state & RTM_STATE_NO_WIDE )
	{
		stRTMovieFlag &= (~_RTM_FLAG_WIDE);
		GetR3State()->mWideOn=FALSE;
	}
	else
	{
		stRTMovieFlag |= _RTM_FLAG_WIDE;
		GetR3State()->mWideOn=TRUE;
	}
	if( state & RTM_STATE_NO_LOGO )
	{
		stRTMovieFlag &= (~_RTM_FLAG_LOGO);
		GetR3State()->mIsDrawLogo=FALSE;
	}
	else
	{
		stRTMovieFlag |= _RTM_FLAG_LOGO;
		GetR3State()->mIsDrawLogo=TRUE;
	}
	if( state & RTM_STATE_NO_CAMERAMOVE )
	{
		stRTMovieFlag |=_RTM_FLAG_NO_CAMERA;
		SetDefaultFov();	//포브를 디폴트로..
	}
	else
	{
		stRTMovieFlag &=(~_RTM_FLAG_NO_CAMERA);
	}

}
DWORD RTMovieGetState()
{
	return stRTMState;
}

void RTMoiveGetCameraPos(Vector3f get_pos)
{
	D3DXMATRIX *d3mat = R3MoveGetViewMatrix();
	Vector3fCopy(d3mat->m[3],get_pos);
}

void RTMoiveGetCameraMatrix(float mat[4][4])
{
	D3DXMATRIX *d3mat = R3MoveGetViewMatrix();
	MatrixCopy(mat,d3mat->m);
}

static EffectList			g_EFFRTUnitList;

void RTMovieCreate(char *name,CLevel *level)	//리얼타임 렌더링의 파일을 로딩한다.
{
	char tempname[256];
	stLevel=level;

//	RTMovieRelease();	//이전에 로딩된게 있다면.

	strcpy(tempname,name);
	StripEXT(tempname);
	strcat(tempname,".cam");
	stLevel->mAutoAniCam.LoadAniCamera(tempname);	//카메라 부터 로딩.

	if( stLevel->mAutoAniCam.IsLoadedAniCamera()==FALSE )	//카메라가 없음 리얼타임 못읽는다.
		return;

	FILE *fp=fopen(name,"rt");
	if( !fp )
	{
		stIsLoadedRTMovie=FALSE;
		return;
	}
	stTrack = (CRTMovieTrack *)Dmalloc(sizeof(CRTMovieTrack)*stTrackMaxCnt);
	memset( stTrack, 0, sizeof(CRTMovieTrack)*stTrackMaxCnt );

	//---캐릭터 매니저 관련.
	g_chIF = new ChInterface;
	g_chIF->InitCharacter(GetD3dDevice());

//	g_texture   = new TextureManager;
//	g_texture->SetTextureDevice(GetD3dDevice());

	g_animation = new AnimationManager;


	g_particle = new CParticleManager_;
	g_particle->LoadEntityIni(".\\chef\\real\\entity.ini");
	g_particle->LoadParticleIni(".\\chef\\real\\particle.ini");

	g_EFFRTUnitList.LoadEffectFileList(".\\chef\\real\\unit.ini");	//실제 메쉬아이디와 매치되는 스크립트

	//	PartManager추가 
	g_chIF->AddPartToMM("BODY");

	//캐릭터
	stCh = (RTMCharClass *)Dmalloc(sizeof(RTMCharClass)*stCharMaxCnt);
	memset( stCh, 0, sizeof(RTMCharClass)*stCharMaxCnt );


	//---------스크립트를 읽는다.
	char hole[256];

	DWORD mode = 0;
	DWORD dw_temp = 0;
	float frame=0;
	DWORD id;

    while(1)
    {
		if( fscanf(fp,"%s",hole)==EOF)	
			break;
		strlwr(hole);
		if( !strcmp(hole,"[preparetrack]"))
		{
			mode=2;	//어태치 오브젝트 모드
		}
		else
		if( !strcmp(hole,"[object]"))
		{
			mode=1;	//오브젝트 모드
		}
		else
		if( !strcmp(hole,"[track]"))
		{
			mode=3;	//트랙 모드
		}
		if( mode == 1 )		//오브젝트 로딩이다..
		{
			if( hole[0] == '*' )
			{
				strcpy(stCh[stRTCharNum].mCharName,hole);
				fscanf(fp,"%s",hole);	//메쉬이름..

				strcpy(stCh[stRTCharNum].mMeshName,hole);

				stRTCharNum++;
				if( stRTCharNum >= stCharMaxCnt)	//리얼록.
				{
					stCh=(RTMCharClass *)ReAlloc(stCh,sizeof(RTMCharClass)*stCharMaxCnt
						,sizeof(RTMCharClass)*(stCharMaxCnt+32));
					memset( (stCh+stCharMaxCnt), 0, sizeof(RTMCharClass)*32 );
					stCharMaxCnt += 32;
				}
			}
			if( stRTCharNum==0 )
				continue;
			if( !strcmp(hole,"texpath"))
			{
				fscanf(fp,"%s",hole);	
				strcpy(stCh[stRTCharNum-1].mTexPath,hole);
			}
			if( !strcmp(hole,"bone"))
			{
				fscanf(fp,"%s",hole);	
				strcpy(stCh[stRTCharNum-1].mBoneName,hole);
			}
			if( !strcmp(hole,"ani"))
			{
				fscanf(fp,"%s",hole);	
				dw_temp = stCh[stRTCharNum-1].mActionCnt;
				strcpy(stCh[stRTCharNum-1].mAniName[dw_temp],hole);
     				stCh[stRTCharNum-1].AddAnimation(hole);
				stCh[stRTCharNum-1].mActionCnt++;
			}
			if( !strcmp(hole,"color"))
			{
				DWORD r,g,b;
				fscanf(fp,"%s",hole);
				r=atoi(hole)/2;
				fscanf(fp,"%s",hole);
				g=atoi(hole)/2;
				fscanf(fp,"%s",hole);
				b=atoi(hole)/2;
				stCh[stRTCharNum-1].mColor = 0xff000000|(r<<16)|(g<<8)|b;
				stCh[stRTCharNum-1].mFlag |= _RTM_CHAR_FLAG_COLOR;
			}
			if( !strcmp(hole,"collision"))
			{
				stCh[stRTCharNum-1].mFlag |= _RTM_CHAR_FLAG_COLLISION;
			}
			if( !strcmp(hole,"shadow"))
			{
				stCh[stRTCharNum-1].mFlag |= _RTM_CHAR_FLAG_SHADOW;
			}

			if( !strcmp(hole,"mesh_id"))
			{
				fscanf(fp,"%s",hole);	
				stCh[stRTCharNum-1].mMeshID = atoi(hole);
			}
			if( !strcmp(hole,"scale"))
			{
				fscanf(fp,"%s",hole);	
				stCh[stRTCharNum-1].mScale = (float)atof(hole);
			}			
		}
		else
		{
			if( !strcmp(hole,"camera"))
			{
				fscanf(fp,"%s",hole);
				strlwr(hole);
				id = GetCameraID(&hole[1]);
				if( id==-1 )
				{
					Warning(hole,"<-이 카메라가 cam파일에 없습니다. 맥스에서 카메라 파일이름이 맞나 확인해보시길.");
					continue;				
				}
				stTrack[stTrackCnt].mWhat = _TRACK_CAMERA;
				stTrack[stTrackCnt].mFrame = frame;
				stTrack[stTrackCnt].mdwArgv[0]=id;
				stTrackCnt++;
				continue;				
			}
			if( !strcmp(hole,"fade_in"))
			{
				fscanf(fp,"%s",hole);
				stTrack[stTrackCnt].mWhat = _TRACK_FADE_IN;
				stTrack[stTrackCnt].mFrame = frame;
				stTrack[stTrackCnt].mdwArgv[0] = atoi(hole);
				stTrackCnt++;
				continue;				
			}

			if( !strcmp(hole,"magic"))	//마법 하나가 캐릭터 하나를 차지한다.
			{
				fscanf(fp,"%s",hole);
				id = stLevel->mAutoAniCam.GetDummyID(&hole[1]);
				fscanf(fp,"%s",hole);
				stTrack[stTrackCnt].mWhat = _TRACK_MAGIC;
				stTrack[stTrackCnt].mFrame = frame;
				stTrack[stTrackCnt].mdwArgv[0] = stRTCharNum;	//캐릭터 아이디.
				dw_temp = atoi(hole);
				stTrack[stTrackCnt].mdwArgv[1] = dw_temp;	//매직 아이디.
				stTrackCnt++;

				stCh[stRTCharNum].mDummyID=id;	//더미아이디..
				stCh[stRTCharNum].mMagicID = dw_temp;
				stCh[stRTCharNum].mFlag = _RTM_CHAR_FLAG_MAGIC;
				stRTCharNum++;		//매직 하나에 캐릭터 하나가 늘어난다.
				if( stRTCharNum >= stCharMaxCnt)	//리얼록.
				{
					stCh=(RTMCharClass *)ReAlloc(stCh,sizeof(RTMCharClass)*stCharMaxCnt
						,sizeof(RTMCharClass)*(stCharMaxCnt+32));
					memset( (stCh+stCharMaxCnt), 0, sizeof(RTMCharClass)*32 );
					stCharMaxCnt += 32;
				}
			}

			if( !strcmp(hole,"ani"))
			{
				fscanf(fp,"%s",hole);
				id = GetCharID(hole);
				if( id == -1 )
					continue;
				fscanf(fp,"%s",hole);
				stTrack[stTrackCnt].mWhat = _TRACK_ANI;
				stTrack[stTrackCnt].mFrame = frame;
				stTrack[stTrackCnt].mdwArgv[0] = id;
				stTrack[stTrackCnt].mdwArgv[1] = atoi(hole);	//액션 아이디.
				stTrackCnt++;
				continue;				
			}			
			if( !strcmp(hole,"char_fade_in"))
			{
				fscanf(fp,"%s",hole);
				id = GetCharID(hole);
				if( id == -1 )
					continue;
				fscanf(fp,"%s",hole);
				stTrack[stTrackCnt].mWhat = _TRACK_CHAR_FADE_IN;
				stTrack[stTrackCnt].mFrame = frame;
				stTrack[stTrackCnt].mdwArgv[0] = id;
				stTrack[stTrackCnt].mdwArgv[1] = atoi(hole);	//
				stTrackCnt++;
				continue;				
			}		
			if( !strcmp(hole,"char_fade_out"))
			{
				fscanf(fp,"%s",hole);
				id = GetCharID(hole);
				if( id == -1 )
					continue;
				fscanf(fp,"%s",hole);
				stTrack[stTrackCnt].mWhat = _TRACK_CHAR_FADE_OUT;
				stTrack[stTrackCnt].mFrame = frame;
				stTrack[stTrackCnt].mdwArgv[0] = id;
				stTrack[stTrackCnt].mdwArgv[1] = atoi(hole);	//
				stTrackCnt++;
				continue;
			}			
			if( !strcmp(hole,"fade_out"))
			{
				fscanf(fp,"%s",hole);
				stTrack[stTrackCnt].mWhat = _TRACK_FADE_OUT;
				stTrack[stTrackCnt].mFrame = frame;
				stTrack[stTrackCnt].mdwArgv[0] = atoi(hole);
				stTrackCnt++;
				continue;				
			}
			if( !strcmp(hole,"quake"))
			{
				stTrack[stTrackCnt].mWhat = _TRACK_QUAKE;
				stTrack[stTrackCnt].mFrame = frame;
				fscanf(fp,"%s",hole);	//time 
				stTrack[stTrackCnt].mfArgv[0] = (float)atof(hole); 
				fscanf(fp,"%s",hole);	//density 
				stTrack[stTrackCnt].mfArgv[1] = (float)atof(hole); 
				stTrackCnt++;
				continue;				
			}
			if( !strcmp(hole,"wav"))
			{
				stTrack[stTrackCnt].mWhat = _TRACK_WAVE;
				stTrack[stTrackCnt].mFrame = frame;
				fscanf(fp,"%s",hole);
				id = stLevel->mAutoAniCam.GetDummyID(&hole[1]);
				stTrack[stTrackCnt].mdwArgv[0] = id;

				fscanf(fp,"%s",hole);	//time 
				stTrack[stTrackCnt].mdwArgv[1] = IM_LoadWave(hole);	//인덱스가 리턴된다.하나씩 로딩하지만 안에서 웨이브 관리자가 생기게되므로 .. 마지막엔 IM_ReleaseAllWaves()호출 할것.
				stTrackCnt++;
				continue;				
			}



			if( mode == 3 )//트랙 모드
			{
				if( !strcmp(hole,"*frame"))
				{
					fscanf(fp,"%s",hole);
					frame=(float)atof(hole);
				}
			}
			else
			if( mode == 2 )	//prepare 모드
			{
				if( hole[0] == '*' )
				{
					id = GetCharID(hole);
					if( id==-1 )
					{
						Warning(hole,"<-이 메쉬 이름이 스크립트에서 지정되지 않았습니다.");
						continue;				
					}
					fscanf(fp,"%s",hole);
					DWORD d_id = stLevel->mAutoAniCam.GetDummyID(&hole[1]);
					stCh[id].mDummyID = d_id;
				}

				if( !strcmp(hole,"no_camera"))	//카메라 안쓰는 모드이다.
					stRTMovieFlag|= _RTM_FLAG_NO_CAMERA;
				if( !strcmp(hole,"wide"))
				{
					stRTMovieFlag|= _RTM_FLAG_WIDE;
					GetR3State()->mWideOn=TRUE;
				}
				if( !strcmp(hole,"logo"))
				{
					stRTMovieFlag|= _RTM_FLAG_LOGO;
					GetR3State()->mIsDrawLogo=TRUE;
				}
				
				if( !strcmp(hole,"total_frame"))	//전체 프래임수
				{
					fscanf(fp,"%s",hole);
					stTotalFrame=(float)atof(hole);
					stLevel->mAutoAniCam.SetPrePlayCamera(0);
//					stLevel->mAutoAniCam.SetPrePlayCamera(0,_CAM_PLAY_FULL_FRAME
//						,_CAM_PLAY_FULL_FRAME,_CAM_FLAG_FINAL_STOP);
				}

			}
		}
		if( stTrackCnt >= stTrackMaxCnt)	//리얼록.
		{
			stTrack=(CRTMovieTrack *)ReAlloc(stTrack,sizeof(CRTMovieTrack)*stTrackMaxCnt,sizeof(CRTMovieTrack)*(stTrackMaxCnt+64));
			memset( (stTrack+stTrackMaxCnt), 0, sizeof(CRTMovieTrack)*64 );
			stTrackMaxCnt += 64;
		}
	}
	fclose(fp);
	stTrack[stTrackCnt].mFrame = -1;	//마지막 프래임에 -1을 두어서 판독한다.

	DestroyEffectsList();
	DestroyMagicSptList();
	ResetEffectManager();	//이펙트 메니져들을 리셋한다.
	CreateEffectsList(".\\effect\\Effectentitylist.spt");		//이펙트들 초기화.
	CreateMagicSptList(".\\effect\\Magicsptlist.spt");		//매직 초기화.

	//기본적으로 항상 노카메라모드로 해놓는다.
	stRTMovieFlag|= _RTM_FLAG_NO_CAMERA;

	OneTimeSceneRTMovie();	//캐릭터 로딩이라든지 첨 로딩하는거..
	PrepareRTMovie();		//첫프래임.
	stIsLoadedRTMovie=TRUE;
}

static void ExtractFrame(float frame)
{
	float temp;
	DWORD dw_temp;
	float mat[4][4];

	if( !stTrack )
		return;
	while( 1 )
	{
		if( stTrack[stNowTrack].mFrame == -1)	//더이상 트랙이 없다.
			break;
		if( frame < stTrack[stNowTrack].mFrame )
			break;
		if( ( stRTMovieFlag & _RTM_FLAG_NO_CAMERA ) && !(stTrack[stNowTrack].mWhat & _TRACK_INDEPENDANT_CAMERA) )	
		{
			stNowTrack++;//카메라가 자동진행이 아니고 카메라와 구속받는 종류라면 스킵한다.
			continue;	
		}
		
//		switch( (stTrack[stNowTrack].mWhat&~_TRACK_INDEPENDANT_CAMERA) )
		switch( stTrack[stNowTrack].mWhat )
		{
		case _TRACK_CAMERA:
			temp = frame -stTrack[stNowTrack].mFrame;
			stLevel->mAutoAniCam.SetPrePlayCamera(stTrack[stNowTrack].mdwArgv[0]
				,frame,_CAM_PLAY_FULL_FRAME,_CAM_FLAG_FINAL_STOP);
			break;
		case _TRACK_FADE_IN:
			SetFadeIn(0,stTrack[stNowTrack].mdwArgv[0]/30.0f);		//해당되는 컬러와 페이드되는 시간.	density는 마지막 해당색의 강도이다.
			break;
		case _TRACK_FADE_OUT:
			SetFadeOut(0,stTrack[stNowTrack].mdwArgv[0]/30.0f);		//해당되는 컬러와 페이드되는 시간.	density는 마지막 해당색의 강도이다.
			break;
		case _TRACK_MAGIC://마법 뿌려라.
			dw_temp = stTrack[stNowTrack].mdwArgv[0]; //캐릭터 id
			temp = stLevel->mAutoAniCam.GetNowFrame();
			stLevel->mAutoAniCam.GetDummyMatrix(mat,stCh[dw_temp].mDummyID,temp);
			stCh[dw_temp].mMagicUniqueID	= StartMagicEffect(mat,stTrack[stNowTrack].mdwArgv[1],0,_EF_STATE_MYSELF_LIVE);
			FrameMoveAllEffectsList();
//			stCh[dw_temp].mMagicUniqueID	= StartMagicEffect(mat[3],0,stTrack[stNowTrack].mdwArgv[1]);
			break;
		case _TRACK_ANI:
			dw_temp=stTrack[stNowTrack].mdwArgv[0];	//액션 바꾸기.
			stCh[dw_temp].SetAnimation(stTrack[stNowTrack].mdwArgv[1]);
			break;
		case _TRACK_CHAR_FADE_IN:
			dw_temp=stTrack[stNowTrack].mdwArgv[0];	//캐릭터 아이디.
			stCh[dw_temp].SetFadeIn(stTrack[stNowTrack].mdwArgv[1]/30.0f);
			break;
		case _TRACK_CHAR_FADE_OUT:
			dw_temp=stTrack[stNowTrack].mdwArgv[0];	//캐릭터 아이디.
			stCh[dw_temp].SetFadeOut(stTrack[stNowTrack].mdwArgv[1]/30.0f);
			break;
		case _TRACK_QUAKE:
			R3EnvironmentQuake(stTrack[stNowTrack].mfArgv[0],stTrack[stNowTrack].mfArgv[1]);
			break;
		case _TRACK_WAVE:
			dw_temp = stTrack[stNowTrack].mdwArgv[0]; //캐릭터 id
			temp = stLevel->mAutoAniCam.GetNowFrame();
			stLevel->mAutoAniCam.GetDummyMatrix(mat,stCh[dw_temp].mDummyID,temp);
			Play3DSound(stTrack[stNowTrack].mdwArgv[1],mat[3]);
			break;
		}
		stNowTrack++;
	}
}

void RTMovieFrameMove()
{
	if( stLevel == NULL )
		return;

	if( stLevel->mAutoAniCam.IsLoadedAniCamera() == FALSE ) 
		return;

	float mat[4][4];
	float frame = stLevel->mAutoAniCam.GetNowFrame();
	BOOL b_end;
	float cam_mat[4][4];

	frame*=30;	//초당 30프래임이니까..

	if( stRTMPause || !stRTMPlay )	//정지 상태의 경우는 원래 프래임 세팅.
	{
		ExtractFrame(frame);
		stLevel->mAutoAniCam.SetNowFrame(frame/30.0f);
//		SetDefaultFov();	//포브를 디폴트로..
		b_end = 0;
	}
	else
	{
		ExtractFrame(frame);
		b_end = stLevel->mAutoAniCam.PlayAniCamera(cam_mat,1.0f,stRTMPause);
	}

	if( !(stRTMovieFlag & _RTM_FLAG_NO_CAMERA))	//자동카메라를 사용할 경우만.
	{
		Vector3f pos,add_pos={0,0,0};
//		R3GetQuakeVector(add_pos);
//		cam_mat[3][0] +=add_pos[0];
//		cam_mat[3][1] +=add_pos[1];
//		cam_mat[3][2] +=add_pos[2];
		::R3SetCameraMatrix(pos,cam_mat);
		stLevel->SetCameraPos(pos);
		stLevel->SetViewMatrix(R3MoveGetViewMatrix());
	}
	else
		SetDefaultFov();	//포브를 디폴트로..
	if( !b_end )
	{
		stRTMPlay=FALSE;
		stRTMovieFlag|=_RTM_FLAG_NO_CAMERA;
	}


	if( !g_chIF )
		return;
	DWORD i;
	for(i=0; i<stRTCharNum; i++)
	{
		if( stCh[i].mFlag & _RTM_CHAR_FLAG_MAGIC )
		{
			if( stCh[i].mMagicUniqueID )	//현재 매직 아이디가 존재하면.
			{
				stLevel->mAutoAniCam.GetDummyMatrix(mat,stCh[i].mDummyID,frame/30.0f);
				BOOL is_exist = MoveMatrixMagicEffect(stCh[i].mMagicUniqueID,mat);
				if( !is_exist )	//사라졌다.
					stCh[i].mMagicUniqueID=0;
			}
			continue;
		}
		if( stCh[i].g_bn==NULL )
		{
			stCh[i].g_bn  = g_chIF->GetBoneData( stCh[i].mBoneName );
			if( stCh[i].g_bn==NULL )
				continue;
		}
		if( stCh[i].mMesh==NULL )
		{
			stCh[i].mMesh  = g_chIF->GetMeshData(g_om_body, stCh[i].mMeshName );
			if( stCh[i].mMesh==NULL )
				continue;
		}
		stCh[i].mFrame += R3GetLoopTime()*30*160;
		stCh[i].g_bn->m_Frame = stCh[i].mFrame;

		if( stCh[i].g_bn->m_OMaxFrame < stCh[i].g_bn->m_Frame )
		{
			stCh[i].mFrame=160;
		}
//		g_chIF->FrameStep(stCh[i].g_bn);


		float temp_mat[4][4];
		MatrixScale(temp_mat,stCh[i].mScale,stCh[i].mScale,stCh[i].mScale);
		//더미 움직임에 붙이기...
		stLevel->mAutoAniCam.GetDummyMatrix(stCh[i].mMat,stCh[i].mDummyID
			,stLevel->mAutoAniCam.GetNowFrame());//,frame/30.0f);
		MatrixMultiply(stCh[i].mMat,stCh[i].mMat,temp_mat);

		//프러스텀계산
		stCh[i].CheckViewFrustum();
		if( !stCh[i].IsInViewFrustum() )
			continue;

		g_chIF->FrameMove(stCh[i].g_bn);
		g_chIF->FrameMove(stCh[i].mMesh);
		
		if( stCh[i].mFlag & _RTM_CHAR_FLAG_COLLISION )
		{
			float get_ypos;
			stCh[i].mMat[3][1] = stCh[i].mPos[1];	//이전 y좌표를 넣어준다.
			BOOL can_go = stLevel->GetNextYpos(stCh[i].mMat[3],&get_ypos);
			if( can_go )
				stCh[i].mMat[3][1] = get_ypos;
			Vector3fCopy(stCh[i].mMat[3],stCh[i].mPos);
		}
	}
}

void RTMovieRender()
{
	if( !g_chIF )
		return;

	if( stLevel->mAutoAniCam.IsLoadedAniCamera() == FALSE ) 
		return;

	D3DLIGHT8 light;
	D3DMATERIAL8 mtrl;
	DWORD color;

	g_chIF->SetState();
	DWORD i;

	float frame = stLevel->mAutoAniCam.GetNowFrame();

	if( GetR3State()->mIsFog )
		GetD3dDevice()->SetRenderState( D3DRS_FOGENABLE, TRUE );
	for(i=0; i<stRTCharNum; i++)
	{
//		DWORD color = stLevel->mBsp->GetLightFromPoint(stPlayer.mPos);
		if( stCh[i].mFlag & _RTM_CHAR_FLAG_MAGIC )
			continue;
		if( !stCh[i].IsInViewFrustum() )
			continue;
		
		if( stCh[i].g_bn==NULL )
		{
			stCh[i].g_bn  = g_chIF->GetBoneData( stCh[i].mBoneName );
			if( stCh[i].g_bn==NULL )
				continue;
		}
		if( stCh[i].mMesh==NULL )
		{
			stCh[i].mMesh  = g_chIF->GetMeshData(g_om_body, stCh[i].mMeshName );
			if( stCh[i].mMesh==NULL )
				continue;
		}

		if( stCh[i].mFlag & _RTM_CHAR_FLAG_COLOR )
			color = stCh[i].mColor;
		else
			color = stLevel->mBsp->GetLightFromPoint(stCh[i].mMat[3]);

		::GetMatLightFromColor( &light,&mtrl, color );	//컬러로 부터 라이트 머터리얼을 얻어온다. 

		float alpha = (stCh[i].mColor>>24)/255.0f;
		if( alpha == 0 )
			continue;
		g_chIF->SetAlpha(stCh[i].mMesh,alpha);
		g_chIF->SetLight(light);
		g_chIF->SetMaterial(stCh[i].mMesh,mtrl);

		//memcpy(&temp,stCh[i].mMat,sizeof(D3DXMATRIX));
		//스케일 테스트 해바..
		g_EFFRTUnitList.SetEffect(stCh[i].mMesh,stCh[i].mMeshID);

		g_chIF->DrawCharacterMat(stCh[i].mMesh,(D3DXMATRIX*)stCh[i].mMat, 0);
	}

	if( stRTCharNum )
		g_AMeshManager.DrawAlpahMesh();

	if( GetR3State()->mIsFog )
		GetD3dDevice()->SetRenderState( D3DRS_FOGENABLE, FALSE );
	g_chIF->UnSetState();
	
	for(i=0; i<stRTCharNum; i++)
	{
//		DWORD color = stLevel->mBsp->GetLightFromPoint(stPlayer.mPos);
		if( stCh[i].mFlag & _RTM_CHAR_FLAG_MAGIC )
			continue;
		if( !stCh[i].IsInViewFrustum() )
			continue;
		
		if( stCh[i].g_bn==NULL )
		{
			stCh[i].g_bn  = g_chIF->GetBoneData( stCh[i].mBoneName );
			if( stCh[i].g_bn==NULL )
				continue;
		}
		if( stCh[i].mMesh==NULL )
		{
			stCh[i].mMesh  = g_chIF->GetMeshData(g_om_body, stCh[i].mMeshName );
			if( stCh[i].mMesh==NULL )
				continue;
		}
		DrawEffectMat(stCh[i].mMesh,(D3DXMATRIX*)stCh[i].mMat,stCh[i].mColor);	//캐릭터에 붙는이펙트
	}
	if( GetR3State()->mIsFog )
		GetD3dDevice()->SetRenderState( D3DRS_FOGENABLE, TRUE );
}


#else
BOOL IsLoadedRTMovie(){		return FALSE;	}		//rt동영상 로딩되었는가?
BOOL IsLoadedRTCamera(){	return FALSE;	}	//rt카메라가 로딩되었는가? 
void RTMovieCreate(char *name,CLevel *level){}	//리얼타임 렌더링의 파일을 로딩한다.
void RTMovieRelease(){}
void RTMovieFrameMove(){}
void RTMovieRender(){}


void RTMovieSetState(DWORD state){}
DWORD RTMovieGetState(){return 0;}
void RTMovieStartPlay(BOOL is_beginning){}	// TRUE는 처음부터 FALSE는 멈춘때부터..
void RTMoiveGetCameraPos(Vector3f get_pos){}
void RTMoiveGetCameraMatrix(float mat[4][4]){}
void RTMoviePause(BOOL pause){}	//멈출려면 TRUE 다시 작동시킬려면 FALSE
BOOL IsRTMoviePlaying(){	return FALSE; }	//무비가 플레이도중인가?

#endif
