#ifndef __CORE_H__
#define __CORE_H__

#include "R3d3d8.h"
#include "C_vector.h"

#define _FRUSTUM_NEAR	4
#define _FRUSTUM_FAR	5
#define _FRUSTUM_RIGHT	1
#define _FRUSTUM_LEFT	0
#define _FRUSTUM_TOP	2
#define _FRUSTUM_BOTTOM	3


//#define _DEFINE_FOV		(D3DX_PI/(2.8f));		//48.29도이다.	tan(각도)값
#define _DEFINE_FOV		1.12f //(0.9f);		//tan(42). 


typedef struct{
//---엔진 패스 부분.
	char MapPath[128];
	char SystemPath[128];
	char EntityPath[128];
	char EffectPath[128];
	char ScreenShotsPath[128];
	char SoundPath[128];	//효과음 패스

	char CharPath[128];
	char CharTexturePath[128];
	char MeshName[128];
	char BoneName[128];
	char StopAniName[128];
	char RunAniName[128];
	char SwordAniName[128];
	char SwordMeshName[128];
	char SwordTexturePath[128];

//---- 렌더링 환경부분.
	BOOL mbStartCamera;			//시작부터 강제진행 카메라인가??
	float mNear,mFar;	//니어와 파플레인...
	float mMipMapBias;		//밉맵 날카로움 조정..
	float mFov;			//카메라 각도...
	float mAsfectRatio;			//화면 종횡비..
	float mScreenXsize;			//화면 가로 해상도.
	float mScreenYsize;			//화면 세로 해상도.
	float mFlySpeed;			//날러 다니는거 속도
	DWORD mRenderBits;			//렌더링프래임버퍼 비트수..
	DWORD mZbufferBits;			//Z버퍼 비트수..
	BOOL mbFullScreen;			//풀스크린인가??
	DWORD mAvailableTextureMem;	//최초의 현재 사용가능 텍스쳐 사이즈
	BOOL mIsUsedVertexShader;	//버텍스쉐이더 사용 가능한가?
	BOOL mWideOn;				//16:9의 와이드 인가..
	BOOL mIsDrawLogo;			//로고를 뿌릴건가..
	DWORD mTextureUnitNum;		//하드웨어가 동시에 뿌리는 텍스쳐갯수.

	//---CFG파일로 부터 읽어 들이는거...
	float mGamma;				//내부 감마값.
	DWORD mTextureDetail;		//텍스쳐 디테일 값...	//캐릭터에해당.
	BOOL mDetailTexture;		//텍스쳐 디테일 값...	//캐릭터에해당.
	DWORD mSceneTextureDetail;		//텍스쳐 디테일 값...	//배경에 해당.
	DWORD mShadowDetail;		//그림자 갯수..
	DWORD mDynamicLight;		//동적라이팅수...
	float mSeeDistance;			//보는거리 비율... 원래=1.0   0.8=8원래의 0%거리..
	BOOL mSoundOn;				//효과음..
	BOOL mMusicOn;				//음악끔..
	float mSoundVol;			//표과음볼륨
	float mMusicVol;			//음악 볼륨
	BOOL mMouseAccelation;		//마우스 가속여부.
	DWORD mThisVidMemLevel;		//현재 사용가능한 텍스쳐사이즈 레벨 0,1,2	상,중,하


	DWORD mMaxTexSize;		//현카드의 최대 텍스쳐 가로및 세로 사이즈
	DWORD mTempSize;

	BOOL mShowLightMap;		//라이트맵을 보인다.
	//포그관련.
	BOOL mIsFog;			//포그가 있는가?
	BOOL mIsNoFogSky;		//스카이박스 포그가 없는가?
	BOOL mFogRangeEnable;	//포그 범위킬까?
	float mFogStart;			//하드웨어 포그 시작.
	float mFogEnd;				//끝
	DWORD mFogColor;

	float mMainFogStart;			//메인 포그 포그 시작.
	float mMainFogEnd;				//메인 포그 끝
	DWORD mMainFogColor;			//메인 포그 색

	//---렌즈플레어관련.
	char mLensTexName[128];
	int	 mLensFlareTex;			//렌즈플레어 텍스쳐 안에 16개에 해당되는 플레어 조각들이 있다.
	float mLensFlareScale[16];	//렌즈플레어들 스케일값.
	float mLensFlarePos[3];		//렌즈플레어들어갈 위치값. x,y,z

	//---Environment 렌더 엔티티이름 관련.  // 비,눈,기타 폭풍등...
	char mEnvEntityName[128];		// 환경은 엔티티로 접근된다.
	DWORD mEnvID;					// 환경 아이디 0 은 없다, 1번은 휴먼 모래먼지.

	//매트릭스 관련.
	D3DXMATRIX mMatWorld;	//월드 매트릭스다..
	D3DXMATRIX mMatView;	//뷰 매트릭스다..
	D3DXMATRIX mMatViewProj;	//뷰*프로젝션 매트릭스
	D3DXMATRIX mInvMatView;	//인버스 뷰 매트릭스다..
	D3DXMATRIX mMatProj;	//프로젝션 매트릭스다..

	Vector3f mLightNomalForShadow;	//쉐도우를 위한 라이트노멀이다..
	Vector3f mMainLightNomal;		//메인라이트노멀이다..
	float	 mMainLightYAngle;		//메인라이트 Yangle값이다.단위는 라디안.

	//텍스쳐 포맷관련.
	D3DFORMAT mBestTextureFormat;		//가장 쓸만한 기본 텍스쳐포맷
//	D3DFORMAT mBest2ndTextureFormat;		//가장 쓸만한 2번째 텍스쳐포맷 bmp의경우.32비트로 가니까 이것으로 태체한다.

	D3DFORMAT mBestSpriteTextureFormat;	//스프라이트로 쓸 가장 쓸만한 텍스쳐포맷
	D3DFORMAT mBestAlphaTextureFormat;	//알파가 들어가는 가장 쓸만한 텍스쳐포맷


	// 스크린 사이즈및 프러스텀.
	LONG mViewSx,mViewSy,mViewXL,mViewYL;
	Vector4f mFrustumPlane[6];//Vector4f;	//프러스텀 클리핑 준비.

	//시스템 텍스쳐..
//	LPDIRECT3DTEXTURE8 mVFogTexture;			//볼륨 포그텍스쳐
	LPDIRECT3DTEXTURE8 mLogoTexture;			//로고 텍스쳐
	LPDIRECT3DTEXTURE8 mDLightTexture;			//다이나믹 라이트 텍스쳐
}_R3ENGINE_STATE;

void InitCore();
//_R3ENGINE_STATE *GetR3State();
extern _R3ENGINE_STATE stState;
#define GetR3State() (&stState)

BOOL IsInitCore();
void ReleaseCore();
void SetFrustumNormalPlane();	//core에 view frustum 평면을 
BOOL IsBBoxInFrustum( Vector3s bb_min,Vector3s bb_max );	//바운딩 박스가 프러스텀안에 드는가?
BOOL IsBBoxInFrustum( Vector3f bb_min,Vector3f bb_max );	//바운딩 박스가 프러스텀안에 드는가?
void GetFrustumNormalPlane(Vector4f n_plane[6]);	//core에 view frustum 평면을 

void *GetTempBuffer();		//스택에 잡기에는 사이즈가 크고 잠깐 쓸 버퍼를 위한 
DWORD GetTempBufferSize();
void RestoreSystemTexture();	//시스템 텍스쳐를 올린다.
void ReleaseSystemTexture();	//시스템 텍스쳐를 내린다.

void SetGamma(float gamma);
void SetMainLight(Vector3f lgt_pos);	//메인 라이트 위치를 셋팅한다.(그림자에 영향있음.)

#endif