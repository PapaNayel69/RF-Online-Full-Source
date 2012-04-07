#ifndef __R3MATERIAL_H__
#define __R3MATERIAL_H__

#define _MAX_MATERIALS		1024
#define _MAX_LAYER_NUM		4	//다이나믹,디테일,라이트맵을 제외하고 최대 4개까지..
#define _MAX_KEY_TRACK		32	//최대 키트랙값

#define _TEX_NAME_SIZE		128	//텍스쳐 최대 이름길이
#define _MAX_ANI_TEX_NUM	64	// 애니메이션 되는 최대 텍스쳐수...

#include "C_vector.h"

#define _R3M_VERSION 1.1f	//R3M포맷 버전..
#define _R3X_VERSION 1.1f	//익스텐드 머터리얼(메인)R3X포맷 버전..


//----layer type
#define _LAYER_TYPE_NORMAL_MAP				0
#define _LAYER_TYPE_LIGHT_MAP				1
#define _LAYER_TYPE_TRANSPARENCY_MAP		2
#define _LAYER_TYPE_OPPASITY_MAP			3


#define _ALPHA_SORT_ON		0x80000000


#define _UV_ENV					0x00000001	
//#define _UV_WATER				0x00000002
#define _UV_LAVA				0x00000004
#define _UV_METAL_FLOOR			0x00000002
#define _UV_METAL_WALL			0x00000008
#define _UV_METAL				0x0000000a	//0x2 or 0x8
#define _UV_SCROLL_U			0x00000010
#define _UV_SCROLL_V			0x00000020
#define _UV_ROTATE				0x00000040
#define _UV_SCALE				0x00000080	//스케일자체는 애니다.
#define _UV_GRADIENT_ALPHA_UV	0x00000300	//알파를 글라디에이션한다.
#define _UV_GRADIENT_ALPHA_U	0x00000100	//알파를 글라디에이션한다.
#define _UV_GRADIENT_ALPHA_V	0x00000200	//알파를 글라디에이션한다.

#define _ANI_ALPHA_FLICKER		0x00000400	//알파 점멸
#define _ANI_TEXTURE			0x00000800  //텍스쳐애니...
#define _ANI_TILE_TEXTURE		0x00001000  //타일 텍스쳐애니...

#define _MAT_ZWRITE				0x00002000	//해당 머터리얼을 z 라이트를 한다. 기본이 안하는거다.
#define _MAT_ENV_BUMP			0x00008000	//Env bump 매핑이다.
//#define _MAT_VOLUME_FOG			0x00008000	//머터리얼이 볼륨포그다.
#define _MAT_TEX_CLAMP			0x00010000	//텍스쳐 클램프 옵션.


#define _MAT_NO_COLLISON		0x00020000	//이 머터리얼을 쓰는 폴리곤들은 모든 충돌검사를 안한다.//0번 레이어 플래그를 쓴다.
#define _MAT_WATER				0x00040000	//이 머터리얼을 쓰는 폴리곤들은 워터효과를 낸다.
/////////////////////////////////////////////////

#define FixedFloatToShort(a)	((short)((a)*256.0f))
#define FixedShortToFloat(a)	((float)((a)/256.0f))


#pragma	pack(push, 1)		//바이트 정렬.

typedef struct
{
	SHORT	m_iTileAniTexNum;			//타일 애니 맵 갯수
	LONG	m_iSurface;
	DWORD	m_dwAlphaType;
	DWORD	m_ARGB;

	DWORD	m_dwFlag;	//값을 가지고 있는지의 bit flag.	//env는 값이 필요없다..
	SHORT	m_sUVLavaWave;	//용암웨이브의 크기

	SHORT	m_sUVLavaSpeed;	// 용암 스피드
	SHORT	m_sUVScrollU;	// U로 이동
	SHORT	m_sUVScrollV;	// V로 이동
	SHORT	m_sUVRotate;	// 회전스피드.
	SHORT	m_sUVScaleStart;	// 스케일 애니시작...
	SHORT	m_sUVScaleEnd;		// 스케일 애니끝...
	SHORT	m_sUVScaleSpeed;		// 스케일 애니끝...
	SHORT	m_sUVMetal;		// 스케일 
	SHORT	m_sANIAlphaFlicker;	//알파 애니
	WORD	m_sANIAlphaFlickerAni;	//알파 애니시작과 끝,값.
	SHORT	m_sANITexFrame;			//텍스쳐 애니의 프래임수
	SHORT	m_sANITexSpeed;		//애니스피드수.	
	SHORT	m_sGradientAlpha;		//	글라디에이션 알파 
}_ONE_LAYER;


#define _MAT_FLAG_LIGHT_MAP			0x00000001
#define _MAT_FLAG_NO_LOD_TEXTURE	0x00000002

typedef struct		//머터리얼구조가 아직 확정되지않음. 대부분이 들어갈경우 키트랙방식으로 바꿀예정.
{
	////////////////////
	// tex
	int m_iMatNum;	//머터리얼 총갯수 머터리얼의 루트에만 숫자를 가지고 있는다.

	char	m_name[_TEX_NAME_SIZE];					//mtrl name
	DWORD	m_dwFlag;		//해당머터리얼의 플그.

	LONG	m_iDetailSurface;		//디테일 맵 
	float	m_fDetailScale;

	DWORD	m_dwLayerNum;
	_ONE_LAYER m_Layer[_MAX_LAYER_NUM];
}_R3MATERIAL;


//ext 머터리얼 맵에관련된 확장 머터리얼.
#define EXT_MAT_NO_FOG_SKY			0x00000001
#define EXT_MAT_FOG_RANGE			0x00000002
#define EXT_MAT_EXIST_1ST_FOG		0x00000004	//1차포그 존재함.
#define EXT_MAT_EXIST_2ND_FOG		0x00000008	//2차포그 존재함.
#define EXT_MAT_EXIST_LENS_FLARE	0x00000010	//렌즈 플레어 존재함.
#define EXT_MAT_EXIST_ENV			0x00000020	//환경 엔티티...존재함.

typedef struct{
	DWORD mFlag;		//

	float mFogStart;
	float mFogEnd;
	DWORD mFogColor;

	float mFogStart2;
	float mFogEnd2;
	DWORD mFogColor2;
	Vector3f BBMin2,BBMax2;	//2차 맵 포그영역.

	//렌즈 플레어
	float mLensFlareScale[16];
	char mLensTexName[128];	//렌즈플레어로쓸 텍스쳐 이름
	Vector3f mLensFlarePos;

	char mEnvEntityName[128];
	DWORD mEnvID;			//환경 아이디 지정.. 이걸루 지정 한다.

	BYTE spare[252+384];		//확장성을 위한 여분.1024k로 맞춘다.
}_EXT_MAT;

#pragma	pack(pop)

//외부에서 쓸함수들...

//맵머터리얼을 로딩할때와 리턴값은 신경 안써도 됨..
_R3MATERIAL *LoadMainMaterial(char *matpath);	//맵에관련된 머터리얼은 이것으로 
void ReleaseMainMaterial();
BOOL IsLoadedMainMaterial();	//현재 메인 머터리얼이 로드되었는가?
DWORD	GetMainMaterialNum();
_R3MATERIAL* GetMainMaterial();


_R3MATERIAL* LoadMainR3M(char *name);	//r3m 바이너리 파일 읽는거...
_R3MATERIAL* LoadSubR3M(char *mat_name);	//메인머터리얼 이외는 이거를.

void SetNoLodTextere();					//lod텍스쳐가 아닌경우 플래그 셋팅을 해놓는다.

//같은 텍스쳐 아이디 잘못된거 수정.
void AdjustIndependenceR3M(_R3MATERIAL *mat,LONG old_base_tex_cnt,LONG new_base_tex_cnt);


//서브 머터리얼들.
_R3MATERIAL *LoadSubMaterial(char *matpath);	// 스카이박스나 엔티티는 이것으로 추가해서 쓸것.
void ReleaseSubMaterial(_R3MATERIAL *mat);	//머터리얼을 릴리즈하면 텍스쳐도 릴리즈 된다.중간에 add한 머터리얼을 제거한다.

//레이어상의 수치를 조정하고싶을때 맵을 나가지않고 머터리얼을 리로드시키는 함수 리로드시 True 끝나면 반드시 false로
void SetReLoadState(BOOL state);	
void ReLoadMaterial(char *filename,_R3MATERIAL *now_material);



#endif