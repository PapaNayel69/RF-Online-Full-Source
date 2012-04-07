//#include "..\\gaudio.h"
#include "jerror.h"
#include "CJCache.h"
#include "r3sound.h"
#include "core.h"
#include "Commonutil.h"


//#define NO_WAVE_PLAY

#ifdef	_SERVER_DEBUG_NO_SOUND	//서버 디버그의 경우 빈함수만을 호출한다.

BOOL InitR3SoundSystem(char *wav_spt_name){ return 1; }		//웨이브스크립트 이름을 입력한다.
void ReleaseR3SoundSystem(){}
void LoadWaveList(char *name){}	//효과음 관련리스트 기본적으로 사운드초기화및 릴리즈때 해준다....
void ReleaseWaveList(){}		//

BOOL SetGlobalWavVolume(float vol){ return 1; }	//0-1까지..	웨이브 전체볼륨.
float GetGlobalWavVolume(){ return 1.0f; }	//0-1까지..	웨이브 전체볼륨.
BOOL SetGlobalMusicVolume(float vol){ return 1; }	//0-1까지..	음악 전체볼륨.
float GetGlobalMusicVolume(){	return 1;	}	//0-1까지..	음악 전체볼륨.

//MP3 플레이에 관한것.
BOOL LoadR3MP3(char *name){ return 1; }
void ReleaesR3MP3(){}
void PlayR3MP3(){}
void StopR3MP3(){}
BOOL SetVolumeMP3(float vol){ return 1; }	//0-1까지..
void SetPlayWaveState( BOOL state ){}
void SetPlayMusicState( BOOL state ){}
void PlayWave(DWORD id,DWORD flag,float vol,float pan){}
void StopWave(DWORD id){}		//루프되어 플레이중인 웨이브를 정지시킨다.
void SetWaveVolume(DWORD id,float vol,float pan){}	// vol 0 ~ 1사이 pan -1 ~ 1사이  -1은 왼쪽에치우침.1은 오른쪽에 치우침.
void SetWaveVolAndPan(DWORD id,float vol,float pan){}
BOOL IsExistFromSoundSpt(DWORD id){return 1;}		//지정된아이디가 스크립트상에 있는가?


BOOL LoadStreamR3MP3(char *name,DWORD loop_cnt){ return TRUE; }
void ReleaesStreamR3MP3(){}

void PlayStreamR3MP3(){}
void StopStreamR3MP3(){}
BOOL SetVolumeStreamMP3(float vol){ return TRUE; }

//-------------- 웨이브 플레이관한거. 직접적으로 사용하므로 로딩및 프리를 해줘야한다.-------------//
DWORD IM_LoadWave(char *name,DWORD flag){ return 1; }	//인덱스가 리턴된다.하나씩 로딩하지만 안에서 웨이브 관리자가 생기게되므로 .. 마지막엔 IM_ReleaseAllWaves()호출 할것.
void IM_ReleaseWave(DWORD id){}				// 순수하게 웨이브 
void IM_ReleaseAllWaves(){}					//맵을 새로 로딩할경우에 이함수를 사용하면 깨끗하게 릴리즈된다.
void IM_PlayWave(DWORD id,float vol,float pan){}
void IM_StopWave(DWORD id){}		//루프되어 플레이중인 웨이브를 정지시킨다.
void IM_SetWaveVolumeAndPan(DWORD id,float vol,float pan){}	//볼륨 0 ~ 1사이 팬은 -1은 왼쪽에치우침.1은 오른쪽에 치우침.
void UpdateStreamMP3(){}  // mp3가 끊기지 않으려면 이함수를 1초에 10번은 호출하랜다...ㅡㅡ
void IM_SetLoopCntWave(DWORD id,DWORD loop_cnt){}

#else


#include "..\\MSS.H"


static CJcache stWaveCache;

void LoadWaveList(char *name);
void ReleaseWaveList();

HDIGDRIVER  stDigital = NULL;           // Digital playback device

#define DIGITALRATE		44100
#define DIGITALBITS		16		//
#define DIGITALCHANNELS	2	//스테레오란다.

#define _SOUND_CACHE_NUM	128
#define _MAX_PALY_CHANNELS 10

static BOOL stIsInit=FALSE;
//static BOOL GetR3State()->mSoundOn=TRUE;
//static BOOL GetR3State()->mMusicOn=TRUE;



static float stGlobalWavVolume=1.0f;	//웨이브 전체 볼륨을 담당한다.
static float stGlobalMusicVolume=1.0f;	//웨이브 전체 볼륨을 담당한다.

BOOL SetGlobalWavVolume(float vol)	//0-1까지..	웨이브 전체볼륨.
{
	stGlobalWavVolume=vol;
	return 1; 
}
float GetGlobalWavVolume()	//0-1까지..	웨이브 전체볼륨.
{
	return stGlobalWavVolume;
}
BOOL SetGlobalMusicVolume(float vol)	//0-1까지..	음악 전체볼륨.
{
	stGlobalMusicVolume=vol;
	return 1; 
}
float GetGlobalMusicVolume()	//0-1까지..	음악 전체볼륨.
{
	return stGlobalMusicVolume;
}


void SetPlayMusicState( BOOL state )
{
//	=state;
}

void SetPlayWaveState( BOOL state )
{
//	GetR3State()->mSoundOn=state;
}

//실제 웨이브 포인터...
typedef struct{
	U32 *ptr;	//메모리 포인터.
	HSAMPLE Handle;
	DWORD id;
}_WAVE_PTR_LIST;

BOOL InitR3SoundSystem(char *wav_spt_name)
{
	AIL_startup();

	AIL_set_preference(DIG_MIXER_CHANNELS,_SOUND_CACHE_NUM+16);

#ifndef	NO_WAVE_PLAY
	LoadWaveList(wav_spt_name);
#endif

	stDigital = AIL_open_digital_driver( DIGITALRATE, DIGITALBITS, DIGITALCHANNELS, 0 );
	if( !stDigital )
		return FALSE;

	stIsInit=TRUE;

	return TRUE;
}

//HSAMPLE stMp3Handle=0;
CPlayMP3::CPlayMP3()
{
	m_Mp3Handle=0;
}
CPlayMP3::~CPlayMP3()
{
	if( m_Mp3Handle )
		ReleaesR3MP3();
}
BOOL CPlayMP3::LoadR3MP3(char *name)
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return TRUE;	
	if( !GetR3State()->mMusicOn )
		return TRUE;

	U32        *ptr;
    ptr = (U32*)AIL_file_read( name, FILE_READ_WITH_SIZE );
	m_ptr = ptr;

	if (ptr == NULL)
	{
		Warning( name, "<- File not found\n" );
		m_Mp3Handle = 0;
		return FALSE;
	}

	// allocate an HSAMPLE to use
	m_Mp3Handle = AIL_allocate_sample_handle( stDigital );
	if (m_Mp3Handle == NULL)
	{
		AIL_mem_free_lock(ptr);
		m_ptr=NULL;
		Warning( AIL_last_error(),"" );
		return FALSE;
	}

	// initialize the sample
	AIL_init_sample( (HSAMPLE)m_Mp3Handle );

	// point the sample at the loaded file image (the first dword is the size)
	if (AIL_set_named_sample_file( (HSAMPLE)m_Mp3Handle, name, ptr+1, ptr[0], -1 ) == 0)
	{
		AIL_mem_free_lock(ptr);
		m_ptr=NULL;
		Warning( AIL_last_error(),"" );
		return FALSE;
	}

	AIL_set_sample_loop_count((HSAMPLE)m_Mp3Handle,0);

	return TRUE;
}

BOOL CPlayMP3::SetVolumeMP3(float vol)	//0-1까지..
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return TRUE;	
	if( !GetR3State()->mMusicOn )
		return TRUE;
	if( !m_Mp3Handle )
		return FALSE;

	AIL_set_sample_volume_pan( (HSAMPLE)m_Mp3Handle,vol*stGlobalMusicVolume,0.5f );
	return TRUE;
}

void CPlayMP3::ReleaesR3MP3()
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return;	
	if( !GetR3State()->mMusicOn )
		return;
	if( m_Mp3Handle )
	{
		AIL_release_sample_handle( (HSAMPLE)m_Mp3Handle );
		m_Mp3Handle=0;
	}
	if( m_ptr )
	{
		AIL_mem_free_lock(m_ptr);
		m_ptr=NULL;
	}
}
void CPlayMP3::PlayR3MP3()
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return;	
	if( !GetR3State()->mMusicOn )
		return;
	if( !m_Mp3Handle )
		return;
    AIL_start_sample( (HSAMPLE)m_Mp3Handle );
	SetVolumeMP3(1.0f);	//0-1까지..
}
void CPlayMP3::StopR3MP3()
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return;	
	if( !GetR3State()->mMusicOn )
		return;
	if( !m_Mp3Handle )
		return;

    AIL_stop_sample( (HSAMPLE)m_Mp3Handle );
}

void ReleaseR3SoundSystem()
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return;	
#ifndef	NO_WAVE_PLAY
	ReleaseWaveList();
#endif

	if (stDigital)
	{
		AIL_close_digital_driver( stDigital );
	}
	AIL_shutdown();
	stIsInit=FALSE;
}



HSTREAM  stStream=NULL;
S32 stStreamSize;

BOOL LoadStreamR3MP3(char *name,DWORD loop_cnt)
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return TRUE;	
	if( !GetR3State()->mMusicOn )
		return TRUE;
	if( stStream )
		ReleaesStreamR3MP3();

	stStream = AIL_open_stream(stDigital, name, 0  );

	// loop the stream forever
	AIL_set_stream_loop_count( stStream, loop_cnt );

	// get the info about the file
	//

	if (stStream == 0)
	{
		Warning(name," Could not open file \n");
		return FALSE;
	}
	S32 mem,rate;

	AIL_stream_info( stStream, &rate, 0, &stStreamSize, &mem );
	return TRUE;
}


void ReleaesStreamR3MP3()
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return;	
	AIL_close_stream( stStream );
	stStream=NULL;
}

void PlayStreamR3MP3()
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return;	
	AIL_start_stream( stStream );
	SetVolumeStreamMP3(1.0f);	//0-1까지..
}

void StopStreamR3MP3()
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return;	
	AIL_pause_stream(stStream,1);
}

BOOL SetVolumeStreamMP3(float vol)	//0-1까지..
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return TRUE;	
	AIL_set_stream_volume_pan( stStream,vol*stGlobalMusicVolume,0.5f );
	return TRUE;
}


void UpdateStreamMP3()  // mp3가 끊기지 않으려면 이함수를 1초에 10번은 호출하랜다...ㅡㅡ
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return;	
	if(!stStream)
		return;

    AIL_service_stream( stStream, 0 );
}

//사운드 %위치 질문..
BOOL IsEndStreamMP3()
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return FALSE;	
	if(!stStream)
		return FALSE;

	if( AIL_stream_status(stStream) == SMP_DONE )
		return TRUE;
	return FALSE;
/*  
	S32 per = AIL_stream_position( stStream );
	per = (per*100)/stStreamSize ;
	if(per >= 100)
		return TRUE;
	return FALSE;*/
}

//-------------------------------------효과음 관련...-------------------------------------------------
typedef struct{
	char name[59];
	BYTE flag;
	DWORD id;
//	WAVstream	*WAV;
}_WAVE_LIST;


static _WAVE_LIST *stWaveList=NULL;
static int stMaxWaveNum;	//최대 엔티티 갯수.. 동적 로딩을 위한 
static int stWaveNum;	//엔티티 갯수 

static DWORD ConvertHexa(char *hole)
{
	int i;

	strlwr(hole);
	if( hole[0] != '0' || hole[1] != 'x' )
		return 0;
	DWORD su=0,temp,cnt=0;
	for(i=strlen(hole)-1; i>1; i--)
	{
		if( '0' <= hole[i] && hole[i] <= '9')
		{
			temp = hole[i]-'0';
		}
		else
		if( 'a' <= hole[i] && hole[i] <= 'f')
		{
			temp = hole[i]-'a'+10;
		}
		else
			return 0;
		su += (temp<<(cnt*4));
		cnt++;
	}
	return su;
}


void LoadWaveList(char *name)
{
	if( name==NULL )
		return;

//	if( stIsInit ==0 )	//사운드 초기화를 안했따.
//		return;	
	FILE *fp;
	if(( fp =fopen(name,"rt"))==NULL) 
		Error(name,"<-이 파일이없습니다.");

	stMaxWaveNum=256;
	stWaveList=(_WAVE_LIST * )Dmalloc( sizeof(_WAVE_LIST)*stMaxWaveNum );
	memset(stWaveList,0,sizeof(_WAVE_LIST)*stMaxWaveNum );
	stWaveCache.CreateJcache(_SOUND_CACHE_NUM,sizeof(_WAVE_PTR_LIST));

	char hole[256];

    while(1)	//앞부분 스킵.
    {
		if( fscanf(fp,"%s",hole)==EOF)	
		{
			fclose(fp);
			return;
		}
        if (!strcmp(hole,"[WaveList]"))
		{
			break;
		}
	}

	stWaveNum=0;
    while(1)
    {
		if( fscanf(fp,"%s",hole)==EOF)	
			break;
CONTINUE_PROSS:
		int id = atoi(hole);
		if( id == 0)
		{
			id = ConvertHexa(hole);
			if( id==0 )
				continue;
		}
		stWaveList[stWaveNum].id=id;
		fscanf(fp,"%s",hole);
		strlwr(hole);
		if( strlen(hole) > 58  )
		{
			Warning(hole,"<-사운드 패스가 너무깁니다.58자이내로 해주세요.");
			continue;
		}
		strcpy(stWaveList[stWaveNum].name,hole);
		stWaveNum++;
		if( stWaveNum >= stMaxWaveNum)	//리얼록.
		{
			stWaveList = (_WAVE_LIST *)ReAlloc(stWaveList
				,sizeof(_WAVE_LIST)*(stMaxWaveNum)
				,sizeof(_WAVE_LIST)*(stMaxWaveNum+64));
			memset(stWaveList+stMaxWaveNum,0,sizeof(_WAVE_LIST)*64 );
			stMaxWaveNum+=64;
		}

		if( fscanf(fp,"%s",hole)==EOF)	
			break;
		strlwr(hole);
        if (!strcmp(hole,"loop"))
			stWaveList[stWaveNum-1].flag = _PLAY_WAVE_LOOP;
		else
			goto CONTINUE_PROSS;
	}
	fclose(fp);
}

void ReleaseWaveList()
{
	DWORD i;

	if( stIsInit ==0 )	//사운드 초기화를 안했따.
	{
		if( stWaveList )
			Dfree(stWaveList);
		stWaveList=NULL;
		stWaveNum=0;
		return;	
	}
	for(i=0; i<stWaveNum; i++)	//캐쉬된 효과음들을 지운다.
	{
		_WAVE_PTR_LIST *get_wav;
		if( stWaveCache.GetCache(stWaveList[i].id,(void**)&get_wav) )	//캐쉬된 버퍼를 얻는다. 없으면 FALSE를 리턴
		{
			if( get_wav->Handle )
			{
				AIL_release_sample_handle( get_wav->Handle );
				get_wav->Handle=NULL;
			}
			if( get_wav->ptr )
			{
				AIL_mem_free_lock((void*)get_wav->ptr);
				get_wav->ptr=NULL;
			}
		}
	}
	stWaveCache.DestroyJcache();

	if( stWaveList )
		Dfree(stWaveList);
	stWaveList=NULL;
	stWaveNum=0;
}

static DWORD GetRealID(int id,BOOL no_warning=TRUE)
{
	DWORD i;
	for(i=0; i<stWaveNum; i++)
	{
		if(stWaveList[i].id == id )
			return i;
	}
	if( no_warning )
	{
		char buf[256];
		if( id & 0xf0000000 )
			sprintf(buf,"0x%x",id);
		else
			sprintf(buf,"%d",id);
		Warning(buf,"<-효과음 스크립트에서 실행하고자하는 이 아이디가 없네요.");
	}
	return -1;
}

static BOOL LoadWave(char *name,DWORD flag, HSAMPLE *Handle,U32 **mem_ptr )
{
	U32        *ptr;

	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return FALSE;	
    ptr = (U32*)AIL_file_read( name, FILE_READ_WITH_SIZE );
	*mem_ptr = ptr;

	if (ptr == NULL)
	{
		Warning( name, "<- File not found\n" );
		*Handle = 0;
		return FALSE;
	}

	//
	// allocate an HSAMPLE to use
	//

	*Handle = AIL_allocate_sample_handle( stDigital );

	if (*Handle == NULL)
	{
		if( ptr )
		{
			AIL_mem_free_lock(ptr);
			*mem_ptr=NULL;
		}
		Warning( name,AIL_last_error() );
		return FALSE;
	}

	//
	// initialize the sample
	//

	AIL_init_sample( *Handle );

	//
	// point the sample at the loaded file image (the first dword is the size)
	//

	if (AIL_set_named_sample_file( *Handle, name, ptr+1, ptr[0], -1 ) == 0)
	{
		if( ptr )
		{
			AIL_mem_free_lock(ptr);
			*mem_ptr=NULL;
		}
		Warning( name,AIL_last_error() );
		return FALSE;
	}
	if( flag == _PLAY_WAVE_LOOP )
		AIL_set_sample_loop_count(*Handle,0);

	return TRUE;
}

BOOL IsExistFromSoundSpt(DWORD id)		//지정된아이디가 스크립트상에 있는가?
{
	DWORD real_id = GetRealID(id,FALSE);
	if( real_id == -1 )
		return FALSE;
	return TRUE;
}

void PlayWave(DWORD id,DWORD flag,float vol,float pan)
{
	if( !GetR3State()->mSoundOn )
		return;
	if( stIsInit ==NULL )	//사운드 초기화를 안했따.
		return;	

#ifdef	NO_WAVE_PLAY
	return;
#endif
	if( AIL_active_sample_count(stDigital) >= _MAX_PALY_CHANNELS )
		return;


	_WAVE_PTR_LIST *get_wav;
	DWORD real_id = GetRealID(id);
	if( real_id == -1 )
		return;
	char *name;

	//캐쉬에서 긁어 온다.없으면 새로만들어서 쓰고...
	LONG result = stWaveCache.HitCache(id,(void**)&get_wav);
	if( result == _HITCACHE_RET_NEW )
	{
		name = stWaveList[real_id].name;
		if( !IsExistFile(name))
		{
			Warning(name,"이 파일이 없슴. 다운은 안시키주지..");
			return;
		}
		stWaveList[real_id].flag =flag;
		LoadWave(name,stWaveList[real_id].flag,&get_wav->Handle,&get_wav->ptr);
	}
	else
	if( result == _HITCACHE_RET_DEL_AND_NEW )	//지웠다 다시쓴다.
	{
		name = stWaveList[real_id].name;
		if( !IsExistFile(name))
		{
			Warning(name,"이 효파일이 없슴. 다운은 안시키주지..");
			return;
		}
		AIL_release_sample_handle( get_wav->Handle );
		stWaveList[real_id].flag =flag;
		LoadWave(name,stWaveList[real_id].flag,&get_wav->Handle,&get_wav->ptr);
	}
	else// _HITCACHE_RET_OLD 기존에 있으므로 바로 쓴다.
	{
	}
	if( get_wav->Handle )
	{
		get_wav->id = id;
		AIL_set_sample_volume_pan(get_wav->Handle,vol*stGlobalWavVolume,(pan+1)*0.5f);
	    AIL_start_sample( get_wav->Handle );
	}
}

void SetWaveVolAndPan(DWORD id,float vol,float pan)
{
	if( stIsInit ==NULL )	//사운드 초기화를 안했따.
		return;	
#ifdef	NO_WAVE_PLAY
	return;
#endif
	_WAVE_PTR_LIST *get_wav;

	if( stWaveCache.GetCache(id,(void**)&get_wav) )
		AIL_set_sample_volume_pan(get_wav->Handle,vol*stGlobalWavVolume,(pan+1)*0.5f);
}

void StopWave(DWORD id)		//루프되어 플레이중인 웨이브를 정지시킨다.
{
	if( stIsInit ==NULL )	//사운드 초기화를 안했따.
		return;	
#ifdef	NO_WAVE_PLAY
	return;
#endif
	_WAVE_PTR_LIST *get_wav;

	if( stWaveCache.GetCache(id,(void**)&get_wav) )
	{
		if( get_wav->Handle )
			AIL_stop_sample( get_wav->Handle );
	}
}

//-----------------------------IM모드 API ---------------------------------------------//



typedef struct{
	BYTE flag;
	HSAMPLE Handle;
	U32 *ptr;
	DWORD mLoopCnt;
}_IM_WAVE_LIST;


static _IM_WAVE_LIST *stIM_WaveList=NULL;
static int stIM_MaxWaveNum=0;	//최대 엔티티 갯수.. 동적 로딩을 위한 
static int stIM_WaveNum=0;	//엔티티 갯수 

DWORD IM_LoadWave(char *name,DWORD flag)	//인덱스가 리턴된다.하나씩 로딩하지만 안에서 웨이브 관리자가 생기게되므로 .. 마지막엔 IM_ReleaseAllWaves()호출 할것.
{
	int i;
	DWORD slut_id=0;

	if( stIsInit ==NULL )	//사운드 초기화를 안했따.
		return 0;	
	if( stIM_MaxWaveNum == 0 )	//처음 일경우..
	{
		stIM_MaxWaveNum=128;
		stIM_WaveList=(_IM_WAVE_LIST * )Dmalloc( sizeof(_IM_WAVE_LIST)*stIM_MaxWaveNum );
		memset(stIM_WaveList,0,sizeof(_IM_WAVE_LIST)*stIM_MaxWaveNum );
	}

	if( !IsExistFile(name))
		return 0;

	for( i=0; i<stIM_WaveNum; i++)
	{
		if( stIM_WaveList[i].Handle == NULL )
			break;
	}
	if( stIM_WaveNum == i )
		slut_id = stIM_WaveNum;
	else
		slut_id = i;

	if( flag == _PLAY_WAVE_LOOP )
		stIM_WaveList[slut_id].mLoopCnt = 0;
	else
		stIM_WaveList[slut_id].mLoopCnt = 1;
	LoadWave(name,flag, &stIM_WaveList[slut_id].Handle, &stIM_WaveList[slut_id].ptr);

	if( stIM_WaveNum == i )
	{
		stIM_WaveNum++;
		if( stIM_WaveNum >= stIM_MaxWaveNum)	//리얼록.
		{
			stIM_WaveList = (_IM_WAVE_LIST *)ReAlloc(stIM_WaveList
				,sizeof(_IM_WAVE_LIST)*(stIM_MaxWaveNum)
				,sizeof(_IM_WAVE_LIST)*(stIM_MaxWaveNum+32));
			memset(stIM_WaveList+stIM_MaxWaveNum,0,sizeof(_IM_WAVE_LIST)*32 );
			stIM_MaxWaveNum+=32;
		}
	}
	return slut_id+1;
}

void IM_ReleaseWave(DWORD id)				// 순수하게 웨이브 
{
	if( stIsInit ==NULL )	//사운드 초기화를 안했따.
		return;	
	if( !id )
		return;
	if( id > stIM_WaveNum )
		return;
	if(!stIM_WaveList)
		return;

	if( stIM_WaveList[id-1].Handle)
	{
		AIL_release_sample_handle( stIM_WaveList[id-1].Handle );
		stIM_WaveList[id-1].Handle=NULL;
	}
	if( stIM_WaveList[id-1].ptr)
	{
		AIL_mem_free_lock( stIM_WaveList[id-1].ptr );
		stIM_WaveList[id-1].ptr=NULL;
	}
	/*
	if( stIM_WaveList )	//아웃.
	{
		Dfree(stIM_WaveList);
		stIM_WaveList=0;
		stIM_MaxWaveNum=0;
	}*/
}


void IM_ReleaseAllWaves()					//맵을 새로 로딩할경우에 이함수를 사용하면 깨끗하게 릴리즈된다.
{
	if( stIsInit ==NULL )	//사운드 초기화를 안했따.
		return;	
	for(int i=0; i<stIM_WaveNum; i++)
	{
		if( stIM_WaveList[i].Handle )
		{
			AIL_release_sample_handle( stIM_WaveList[i].Handle );
			stIM_WaveList[i].Handle=NULL;
		}
		if( stIM_WaveList[i].ptr)
		{
			AIL_mem_free_lock( stIM_WaveList[i].ptr );
			stIM_WaveList[i].ptr=NULL;
		}
	}
	stIM_WaveNum=0;
	if( stIM_WaveList )	//아웃.
	{
		Dfree(stIM_WaveList);
		stIM_WaveList=0;
		stIM_MaxWaveNum=0;
	}
}


void IM_PlayWave(DWORD id,float vol,float pan)
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return;	
	if( !GetR3State()->mSoundOn ) //플레이 하지마라...
		return;
	if( stIM_WaveList == NULL )
		return;
	if( !id )	//0번아이디는 안된다.
		return;
	if( stIM_WaveList[id-1].Handle==NULL )
		return;
	IM_SetWaveVolumeAndPan(id,vol*stGlobalWavVolume,pan);
    AIL_start_sample( stIM_WaveList[id-1].Handle );
}

void IM_SetLoopCntWave(DWORD id,DWORD loop_cnt)
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return;	
	if( !GetR3State()->mSoundOn ) //플레이 하지마라...
		return;
	if( stIM_WaveList == NULL )
		return;
	if( !id )	//0번아이디는 안된다.
		return;
	if( stIM_WaveList[id-1].Handle==NULL )
		return;
	if( loop_cnt !=  stIM_WaveList[id-1].mLoopCnt )
	{
		stIM_WaveList[id-1].mLoopCnt=loop_cnt;
		AIL_set_sample_loop_count(stIM_WaveList[id-1].Handle,loop_cnt);
	}
}

void IM_StopWave(DWORD id)		//루프되어 플레이중인 웨이브를 정지시킨다.
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return;	
	if( stIM_WaveList == NULL )
		return;
	if( !id )	//0번아이디는 안된다.
		return;
	if( stIM_WaveList[id-1].Handle==NULL )
		return;
    AIL_stop_sample( stIM_WaveList[id-1].Handle );
}


void IM_SetWaveVolumeAndPan(DWORD id,float vol,float pan)	//볼륨 0 ~ 1사이 팬은 -1은 왼쪽에치우침.1은 오른쪽에 치우침.
{
	if( stIsInit ==0 )	//사운드 초기화를 안했따.
		return;	
	if( stIM_WaveList == NULL )
		return;
	if( !id )	//0번아이디는 안된다.
		return;
	if( stIM_WaveList[id-1].Handle==NULL )
		return;
	AIL_set_sample_volume_pan(stIM_WaveList[id-1].Handle,vol*stGlobalWavVolume,(pan+1)*0.5f);
}
#endif
