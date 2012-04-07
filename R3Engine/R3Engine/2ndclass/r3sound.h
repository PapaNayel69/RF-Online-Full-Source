#ifndef __R3Sound_H__
#define __R3Sound_H__

//사운드 초기화및릴리즈
BOOL InitR3SoundSystem(char *wav_spt_name);		//웨이브스크립트 이름을 입력한다.
void ReleaseR3SoundSystem();

void LoadWaveList(char *name);	//효과음 관련리스트 기본적으로 사운드초기화및 릴리즈때 해준다....
void ReleaseWaveList();		//


//전체 볼륨에 관한 API
BOOL SetGlobalWavVolume(float vol);	//0-1까지..	웨이브 전체볼륨.
float GetGlobalWavVolume();	//0-1까지..	웨이브 전체볼륨.
BOOL SetGlobalMusicVolume(float vol);	//0-1까지..	음악 전체볼륨.
float GetGlobalMusicVolume();	//0-1까지..	음악 전체볼륨.


class CPlayMP3{
private:
	void *m_Mp3Handle;
	DWORD *m_ptr;	//메모리 블럭.
public:
	CPlayMP3::CPlayMP3();
	CPlayMP3::~CPlayMP3();

	//MP3 플레이에 관한것.
	//---다음은 파일을 메모리에 한번에 올리는경우고 
	BOOL LoadR3MP3(char *name);
	void ReleaesR3MP3();
	void PlayR3MP3();
	void StopR3MP3();
	BOOL SetVolumeMP3(float vol);	//0-1까지..
};

//스트림은 파일을 야금야금 읽어서 플레이하는경우이다. 
BOOL LoadStreamR3MP3(char *name,DWORD loop_cnt=0);
void ReleaesStreamR3MP3();
void PlayStreamR3MP3();
void StopStreamR3MP3();
BOOL SetVolumeStreamMP3(float vol);	//0-1까지..

void UpdateStreamMP3();  // mp3가 끊기지 않으려면 이함수를 1초에 10번은 호출하랜다...ㅡㅡ
BOOL IsEndStreamMP3();		//스트림 mp3가 끝났는가??

#define		SND_DEFAULT		-65536.0f


// 웨이브 플레이는 캐쉬를 사용하는 방법과 직접적으로 사용하는 두가지 방법이있따....
// 현재 엔진에서는 배경효과음의 경우 직접적으로 사용하고(IM모드 ) 게임상에서는 내부캐쉬를 쓰는 방법을 택한다.
//------------------- 웨이브 플레이관한거. 내부 캐쉬를 사용하는 경우의 API -----------------------//

#define _PLAY_WAVE_LOOP		1


void SetPlayWaveState( BOOL state );
void SetPlayMusicState( BOOL state );

void PlayWave(DWORD id,DWORD flag=0,float vol=1.0f,float pan=0);
void StopWave(DWORD id);		//루프되어 플레이중인 웨이브를 정지시킨다.
void SetWaveVolume(DWORD id,float vol,float pan);	// vol 0 ~ 1사이 pan -1 ~ 1사이  -1은 왼쪽에치우침.1은 오른쪽에 치우침.

BOOL IsExistFromSoundSpt(DWORD id);		//지정된아이디가 스크립트상에 있는가?


//-------------- 웨이브 플레이관한거. 직접적으로 사용하므로 로딩및 프리를 해줘야한다.-------------//

DWORD IM_LoadWave(char *name,DWORD flag=0);	//인덱스가 리턴된다.하나씩 로딩하지만 안에서 웨이브 관리자가 생기게되므로 .. 마지막엔 IM_ReleaseAllWaves()호출 할것.
void IM_ReleaseWave(DWORD id);				// 순수하게 웨이브 
void IM_ReleaseAllWaves();					//맵을 새로 로딩할경우에 이함수를 사용하면 깨끗하게 릴리즈된다.

void IM_PlayWave(DWORD id,float vol=1.0f,float pan=0);
void IM_StopWave(DWORD id);		//루프되어 플레이중인 웨이브를 정지시킨다.
//void IM_SetWaveVolume(DWORD id,float vol);	// 0 ~ 1사이 
//void IM_SetWavePan(DWORD id,float pan);	//-1 ~ 1사이  -1은 왼쪽에치우침.1은 오른쪽에 치우침.
void IM_SetWaveVolumeAndPan(DWORD id,float vol,float pan);	//볼륨 0 ~ 1사이 팬은 -1은 왼쪽에치우침.1은 오른쪽에 치우침.
void IM_SetLoopCntWave(DWORD id,DWORD loop_cnt);



#endif