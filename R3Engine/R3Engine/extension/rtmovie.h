#ifndef __RTMOVIE_H_
#define __RTMOVIE_H_

#include "core.h"
#include "AniCamera.h"
#include "jerror.h"
#include "R3util.h"
#include "Level.h"

#define	_RTM_CHAR_FLAG_COLOR		0x00000001
#define	_RTM_CHAR_FLAG_MAGIC		0x00000002		//매직의 경우 하나의 캐릭터를 차지한다.
#define	_RTM_CHAR_FLAG_COLLISION	0x00000004		//캐릭터가 바닥을 타고 다니는 옵션이다.
#define	_RTM_CHAR_FLAG_SHADOW		0x00000008		//캐릭터 쉐도우가 존재한다.

//해당레벨의 이름과 같은 rvp확장자가 있으면 자동으로 로딩된다.
BOOL IsLoadedRTMovie();		//rt동영상 로딩되었는가?
BOOL IsLoadedRTCamera();	//rt카메라가 로딩되었는가? 
void RTMovieCreate(char *name,CLevel *level);	//리얼타임 렌더링의 파일을 로딩한다.
void RTMovieRelease();
void RTMovieFrameMove();
void RTMovieRender();



#define RTM_STATE_NO_WIDE			0x1
#define RTM_STATE_NO_LOGO			0x2
#define RTM_STATE_NO_CAMERAMOVE		0x4

void RTMovieSetState(DWORD state);
DWORD RTMovieGetState();
void RTMovieStartPlay(BOOL is_beginning);	// TRUE는 처음부터 FALSE는 멈춘때부터..
void RTMoviePause(BOOL pause);	//멈출려면 TRUE 다시 작동시킬려면 FALSE
void RTMoiveGetCameraPos(Vector3f get_pos);
void RTMoiveGetCameraMatrix(float mat[4][4]);
BOOL IsRTMoviePlaying();	//무비가 플레이도중인가?


#endif
 

