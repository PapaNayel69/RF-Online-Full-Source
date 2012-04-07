#ifndef __EFFECTUTIL_H__
#define __EFFECTUTIL_H__

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../BaseHeader.h"
#include "../Global.h"


//효과음 루트 숫자....
#define _ROOT_FORCE_SOUND			100	
#define _ROOT_SKILL_SOUND			70000	
#define _ROOT_DAMAGE_SOUND			90000-50
#define _ROOT_START_ATTACK_SOUND	260000

#define _MOB_BASE_SOUND		95000

#define	_POS_TYPE_BOUNDINGBOX			1	//바운딩 박스 끝
#define	_POS_TYPE_DAMAGE2				2	//damage2 더미위치
#define	_POS_TYPE_R_HAND				3	//오른손
#define	_POS_TYPE_L_HAND				4	//왼손
#define	_POS_TYPE_EFFECT1				5	//이펙트1
#define	_POS_TYPE_FIRE1					6	//총구
#define	_POS_TYPE_BOUNDINGBOX_PLUS		7	//바운딩박스 일정 위
#define	_POS_TYPE_R_TOE					8	//왼발 
#define	_POS_TYPE_L_TOE					9	//오른발
#define	_POS_TYPE_FLOOR					10	//그냥 캐릭터 포지션
#define	_POS_TYPE_EFFECT2				11	//이펙트2
#define	_POS_TYPE_FIRE2					12	//총구



//한 방에 궤적 표현하는 거.
class COneTrace{
	Vector3f mStart,mEnd,mNow,mRealStartPos,mNormal;
	float mLength,mSpeed,mTraceDist;
public:	
	void SetOneTraceStartEndPos(Vector3f start_pos,Vector3f end_pos,float length,float speed);
	void NowSetTracePos(Vector3f pos);
	BOOL DrawOneTrace();
};



float GetVolume(Vector3f pos);	//맵상의 위치로 효과음에 쓰일 볼륨을 구한다.
float GetPan(Vector3f pos);	//맵상의 위치하나로 효과음에 쓰일 팬값을 구한다.  -1은 왼쪽에치우침.1은 오른쪽에 치우침.

void SkipCheckSameSound();	//같은사운드 연속으로 들어오는거 허락하는거...
void Play3DSound(DWORD id,Vector3f pos);	//해당 웨이브아이디와 위치를 날려주면 3D 사운드가 생긴다.
void Play2DSound(DWORD id);					//볼륨 1로..
DWORD EFUTIL_GetSoundMat(DWORD attack,DWORD defence);	//재질별로 효과음 인덱스 리턴.


DWORD EFUtil_GetUniqueIDFromItem(void *ptr);
DWORD EFUtil_GetUniqueIDFromMonster(void *ptr);

//캐릭터 클래스에있는 더미 변환코드들이 여기에 있다.
BOOL EFUtil_GetTransformFromCharacter(Vector3f *get,void *char_ptr,int dummy_id,Vector3f pos);
BOOL EFUtil_GetCharTransformFromIndex(Vector3f *get_pos,CCharacter *c_char,CEquipItem *item,DWORD id);
BOOL EFUtil_GetCharTransformFromIndex(Vector3f *get_pos,CCharacter *c_char,DWORD id);

void EFUTIL_CheckContinuousSkillForce(CCharacter *ch);		//지속형 스킬,포스
void EFUtil_Set2ndLight(void *character);
void EFUtil_UnSet2ndLight();
DWORD EFUtil_GetRace(CCharacter *ch);	//종족을 알아낸다.
float EFUtil_GetAngle( CCharacter *ch_src, CCharacter *ch_tar);

//더미인덱스의 포지션을 구한다. 없으면 바운딩 박스의 2/3높이를 구한다.
void EFUtil_GetDummyPos( Vector3f pos, CCharacter *ch,DWORD d_index);
void EFUtil_GetDummyPos( Vector3f pos, CMeshItem *ch_tar,DWORD d_index);


//자연스럽게 사라지는 스트링 렌더링
void EFUtil_StartStringMoveOut(Vector3f pos,char *str,DWORD str_style,DWORD color);
void EFUtil_RenderStringMoveOut();

//void EFUtil_PreDistortDamage(CCharacter *ch,Vector3f pos);	//맞을때 흔들림.
//void EFUtil_BackUpDistortDamage(CCharacter *ch,Vector3f pos);


// 타겟보다 좀더 앞으로 땡긴다.
#define _PRE_DAMAGE_DIST 5
void EFUtil_GetDistFromSource(Vector3f get,Vector3f src,Vector3f tar,float ratio,float dist = _PRE_DAMAGE_DIST);
//스크린쪽으로 위치를 좀 땡긴다.
void EFUtil_GetDistFromScreen(Vector3f get,Vector3f pos,float ratio);


void EFUtil_RestoreDeviceObjects();
void EFUtil_InvalidateDeviceObjects();


//사운드 부분..
void EFUtil_MusicStart(DWORD map_index);	//맵 인덱스를 넣는다..
void EFUtil_MusicLoop();	//매루프마다 음악 첵크한다.
void EFUtil_MusicEnd();
void EFUTIL_SetBattleMusicState();	//아바타가 공격을 당하거나 공격을 할 경우호출해준다. 음악전환을 위해서.
void EFUtil_EffectLog(char *str);

void EFUTIL_DrawLineDummy(CCharacter *ch,DWORD dummy,DWORD color);
void EFUTIL_DrawLineDummyWeapon(CPlayer *pl,DWORD dummy,DWORD color);



// 원거리공격시 캐릭터쏘는위치와 맞는 위치를 파악해서 맵간 충돌 검사를 한다. 
// 충돌이 있는경우 TRUE 충돌이 없는경우 FALSE
BOOL EFUtil_IsCollisionFarAttack(CCharacter *ch_sou,CCharacter *ch_tar);	

// 죽었을때 카메라 워킹..
void EFUTIL_RunDeathCamera(Vector3f point);
void EFUTIL_RunEnterDungeonCamera(Vector3f point);
void EFUTIL_SpecialCamera();			//죽었을때,전투던젼카메라처럼 특수카메라제어시 

void EFUtil_CheckDeleteAttachMagicEffect();	//캐릭터에 붙은 매직이펙트들을 점검해서 안쓰는건 지운다.

BOOL EFUtil_IsBeamGun(CMeshItem *item);	//빔건인가? 

void EFUtil_DeleteSF(APPLIED_SF *sf);	//지울만한 스킬포스는 지운다.

//모션 블로 유틸 함수.
void EFUtil_SetMotionBlurOff();
void EFUtil_LoopMotionBlurTime();	//루프에서 한번씩 호출해준다.
void EFUtil_SetMotionBlurTime(float Time);

void EFUtil_SetLastestEfCode(EffectInfo *e_info);
void EFUtil_MessageStack();
void EFUtil_SetErrorMessage();	//에러메시지를 게임쪽과 연결한다.

DWORD EFUtil_GetMobIndex(void *character);	//몹이펙트에서 사용한는 인덱스.
DWORD EFUtil_GetAttackMaterial(void *character);	//몹이던 캐릭이던 무기 머터리얼을 구한다.
DWORD EFUtil_GetDefenceMaterial(void *character);	//몹이던 캐릭이던 갑옷 머터리얼을 구한다.

void EFUtil_CharacterHide(void *pl,float alpha);	//포탈이동.
void EFUtil_CharacterHideOff();


BOOL EFUTIL_GetPosCharFromBone(Vector3f *get, CCharacter *ch,char *name);	//본이나 오즈젝트 이름 원하는 위치를 얻어낸다.
BOOL EFUTIL_GetPosCharFromPos(Vector3f *get, CCharacter *ch,Vector3f pos);	//오즈젝트 위치를 얻어낸다.


DWORD StartMagicEffectType(CCharacter *ch,Vector3f pos,float y_angle, DWORD magic_id,void *e_Info,DWORD state);
DWORD StartMagicEffectType(CCharacter *ch,float mat[4][4], DWORD magic_id,void *e_Info,DWORD state);

BOOL EFUTIL_GetPosTypeFromChar(Vector3f pos,void *ch, DWORD unique_id);		//현재매직을 타잎별로위치 얻어내는거다.
BOOL EFUTIL_GetPosType(Vector3f pos, CCharacter *ch, BYTE type);			//타잎별 포지션을 챙긴다.

//쉐도우 유틸.
void EFUtil_PushShadow(CCharacter *ptr);	//일단 쉐도우 뿌릴거 리스트를 만든다.
void EFUtil_DrawShadowList(DWORD cnt);		//Push 해놓은 쉐도우 리스트를 필요에 따라 비운다.cnt=-1이면 다뿌린다.

void EFUtil_PushEffectValue(EffectInfo *info,BOOL state);
DWORD EFUTIL_GetRandomMat(DWORD id);	//머터리얼을 넣어주면 해당 머터리얼들에서 렌덤으로 가져온다.0~15

#endif
