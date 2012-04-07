#include "../Network/CNetworkMgr.h"
#include "../GameObject/Manager/CCharacterMgr.h"
#include "../BaseHeader.h"
#include "../Global.h"
#include "../Cland.h"

#include "CSwordTrace.h"
#include "Character.h"
#include "CMagicScript.h"
#include "EffectEntityList.h"
#include "EffectCore.h"
#include "r3util.h"
#include "r3profiler.h"
#include "Commonutil.h"
#include "ItemSpecialEffect.h"
//#include "CWeaponEffect.h"
#include "EffectUtil.h"
#include "MonsterSpecialEffect.h"
#include "PlayerSpecialEffect.h"
#include "ViewEffectCode.h"
#include "..\\addcodebyyun\\particlelist.h"
#include "MoveEffectManager.h"

#define _F_ONLY_THROW					0	//걍 날라간다.		(A)
#define _F_ONLY_NONE_THROW				1	//날라가지 않는다. (자기자신 타겟)(b)
#define _F_THROW_FLASH					2	//날아가서 플래쉬형..	(c)
#define _F_THROW_EXTENT					3	//날아가서 폭발형	(d)
#define _F_NONE_THROW_FLASH				4	//안 날아가서 플래쉬형	(e)
#define _F_NONE_THROW_EXTENT			5	//안 날아가서 폭발형	(f)
#define _F_NONE_THROW_ONE_HIT			6	//안 날아가서 대상 하나만 발동을 줌. (g)
#define _F_NONE_THROW_MAP_ONE_HIT		7	//안 날아가서 맵 클릭해서 발동하는경우.(h)
#define _F_THROW_PASS_ONE_HIT			8	//날아가서 대상을 통과하는경우.(i)
#define _F_ONLY_ONE_HIT					9	//대상하나에 발동만있는경우.(j) 일반공격이다.
#define _F_MULTI_HIT					10	//멀티샷 유형.
#define _F_THREE_HIT					11	//삼연타.


// l_pChar->GetConditionInfo()->m_bEnable_Stealth 스텔스 처리 할것..


class CAllEffectInfo{
public:
	EffectInfo *mEInfo;
	CCharacter *mSrc;
	CCharacter *mTar;
	CEquipItem *mSrcItem;	//주체자가들고 있는 무기..
	CEquipItem *mTarItem;	//당분간 안씀.
	DWORD mSrcID;		//캐릭터나 몬스터의 경우 어떤 건지 아이디...
	DWORD mTarID;
	DWORD mSrcMat;		//캐릭터나 몬스터의 경우 머터리얼 종류...
	DWORD mTarMat;
	Vector3f mSrcPos,mTarPos;
	BBOX mSrcBBox,mTarBBox;
	float mSrcScale,mTarScale;	//소스와 타겟의 스케일...
	DWORD mBulletIDForDamage;	//총알 종류에 따라 데미지표현하기위해 원거리무기일때 어떤 종류인지...알수있는 아이디.

	void ExtractEffectInfo( EffectInfo *e_info );	//이펙트 정보를 풀어놔서 사용하기좋게 해놓는다.
	void ProcessSpecialFarAttack();	//원거리 특수 공격 처리...(원거리지만 근거리와 같은거다.)
	void SetDefaultMoveFunc(_MOVE_EF_FUNC *m_ef_func);	//이동하는 이펙트의 함수..
	BOOL ProcessFarAttack();	//원거리 공격(활,런처등 나가는것들이 보이는것.)
	void Make2ndLightTarget();	//2번째 타겟.
	void SetAttachMoveEffectManager(DWORD unique_id,Vector3f src,Vector3f tar,DWORD style,DWORD dummy_id);//움직이는 거에 붙인다.

	DWORD StartMagicEffectType(Vector3f pos,float y_angle, DWORD magic_id,void *e_Info=NULL,DWORD state=_EF_STATE_AUTO_DISAPEAR);
	DWORD StartMagicEffectType(float mat[4][4], DWORD magic_id,void *e_Info=NULL,DWORD state=_EF_STATE_AUTO_DISAPEAR);

	//공통 함수 
	void Miss();  //src가 미스로 들어오는것..

	//플레이어 주체 함수..
	void PlayerDamage1();
	void PlayerDamage2();
	void PlayerLevelUp();
	void PlayerCreate();
	void PlayerCritical();
	void PlayerFailure();
	void PlayerMoveMapStart();
	void PlayerMoveMapEnd();

	//몬스터 주체 함수..
	void MonsterDamage();
	void MonsterDeath();
	void MonsterCrazy();
	void MonsterCreate();
	void MonsterCritical();
	void MonsterMiss();
	void MonsterAttack();
	void MonsterForce();

	//아이템 주체함수.
	void ItemDefault();
	void ItemAttack();
	void ItemAttr();
	void ItemUse();
	void ItemSkill();
	void ItemForce();
	void OutputDamage(CCharacter *src);	//	데미지숫자포인트 띄우기

	void ForceSkillInvokeCommon();			//포스 스킬 발동기본.
	void DamageCommon(Vector3f pos);		//현재는 공격공통이다. 
	BOOL FarAttackCommon(DWORD ef_id,DWORD what_weapon);	//원거리 공격공통
	BOOL SpecialFarAttackCommon(DWORD weapon_id);			//특수 공격 처리...
	BOOL GetSpecialFarAttackFXID(DWORD weapon_id,DWORD value,DWORD *ef_id,DWORD *snd_id);	//특수 공격 처리...

	//어태치 속성이 있는 경우에만 붙인다.
	BOOL SetAttachMagicTypeAttr(DWORD unique_id);

	//스킬주체함수.
//	void Skill(int code1,int code2,int code3);
	void SkillPrepare();
	void SkillDamage();
	void SkillInvoke();

	//포스.
	void ForceDamage();		//데미지.
	void ForceInvoke();		//발동.
	void ForcePrepare();	//시전.

	void UnitAttack(DWORD code1,DWORD code2);
	void UnitDamage(DWORD code1,DWORD code2);
	void UnitEtc(); //유닛기타 처리.

	void Animus();	//소환수는 이렇게 해보자...깨끗해지는지..
};

//임시 코드
//COneTrace stOneTrace;

//#define _OUT_STRING_COLOR	0xffef2000
#define _OUT_STRING_COLOR	0xff6f2000


//데미지 순서테이블
DWORD stDamageIndexTable[]={ 
	1,1,1		//스워드
	,0,0,12		//나이프
	,2,2,12,2	// 도끼
	,3,3		//메이스
	,4,4		//스태프
	,5			//스피어
	,6			//활
	,6			//석궁
	,7			//총
	,8			//쌍손총
	,9			//라이플
	,13			//런처
	,14			//파우스트
	,10			//기관총
	,11			//화염방사기
	,15,15		//빔총
	,16			//양손빔총
	,17			//플라즈마
	,99999			//채굴기
};


#include "../ccamera.h"



//-----치트 관련 디파인,변수.
#define _MAX_KEY_STRING_CNT 7
#define _MAX_KEY_TIME_INTERVAL 0.5f

static char stKeyString[64];
static DWORD stKeyStringCnt=0;
static float stKeyStringTimer=0;
char stCmpSting[]={"odindev"};
BOOL st_bCmp=FALSE;

static BOOL stTarAttach=FALSE;
void SetTargetAttachMagicEffectType()	//디폴트는 소스다..
{
	stTarAttach = TRUE;
}


void CreateEffect()	//이펙트 초기화.
{
//	LoadEffectEntityList(".\\Entity\\Effect\\EffectEntityList.spt");	//스크립트에서 엔티티리스트를 읽는다.
	
	_R3ENGINE_STATE *state = GetR3State();
	char path[256];
	_strcpy(path,state->EffectPath,200);	//이펙트 패스 얻어오기...
	strcat(path,"EffectEntityList.spt");	//이펙트 패스 얻어오기...
	CreateEffectsList(path);

	_strcpy(path,state->EffectPath,200);	//이펙트 패스 얻어오기...
	strcat(path,"MagicSptList.spt");	//이펙트 패스 얻어오기...
	CreateMagicSptList(path);

	EFUtil_RestoreDeviceObjects();	//
}

void DestroyEffect() //이펙트 릴리즈
{
	DestroyEffectsList();
	DestroyMagicSptList();
	ResetEffectManager();	//이펙트 메니져들을 리셋한다.

	EFUtil_InvalidateDeviceObjects();	//
}


static BOOL do_not_trace=TRUE;

static int st_eff_code=0;
static int stMagicID=0;


float GetDistFromCamera(Vector3f pos)
{
	Vector3f cam_pos,temp;

	::GetCameraPos(&cam_pos);
	Vector3fSub(pos,cam_pos,temp);
	float dist = Vector3fDist(temp);	//몬스터와의 거리.
	return dist;
}




void CAllEffectInfo::SetAttachMoveEffectManager(DWORD unique_id,Vector3f src,Vector3f tar,DWORD style,DWORD type_id)
{
	_MOVE_EF_FUNC m_ef_func;
	SetDefaultMoveFunc(&m_ef_func);	//기본인자들 카피.
	m_ef_func.unique_id = unique_id;
	Vector3fCopy(src,m_ef_func.pos);
	Vector3fCopy(tar,m_ef_func.tar);
	m_ef_func.move_style = style;
	m_ef_func.flag = _MEM_MS_FLAG_AUTO_DISPEAR;		//자동으로 사라지게 한다.
	
	CProcessMagic *magic = GetMagic(unique_id);
	if( magic )
		m_ef_func.speed = magic->mSpeed;
	else
		m_ef_func.speed = 100;
	m_ef_func.attach_time=20;	//2초 동안 박혀있자.
	m_ef_func.attach_type_id = type_id;
	StartMoveEffectManager(&m_ef_func);
}

//value 
#define		_EF_FIRE				0	//불꽃 
#define		_EF_SMOKE				1	//연기
#define		_EF_DAMAGE				2	//데미지
#define		_EF_LINE_ATTACK			3	//무기라인긋기
#define		_EF_EMPTY_CARTRIDGE		4	//탄피
#define		_EF_INVOKE_FIRE			5	//발동타이밍 코드
		


BOOL IsSeeBullet(DWORD weapon_id)
{
	//날아가는 것이 보이느냐 안보이느냐로 구분. 
	switch( weapon_id )
	{
		case 0x11:	//권총..
		case 0x13:	//라이플..
		case 0x16:	//기관총..
		case 0x12:	//쌍총이면..
		case 0x17:	//화염 방사기..
		case 0x19:	//쌍손 빔총..
		case 0x18:	//양손 빔총..
		case 0x1a:	//양손 빔라이플..
		case 0x1b:	//양손프라즈마..
			return FALSE;
	}
	return TRUE;
}
BOOL IsTwinWeapon(DWORD weapon_id)
{
	//날아가는 것이 보이느냐 안보이느냐로 구분. 
	switch( weapon_id )
	{
		case 0x12:	//쌍총이면..
		case 0x19:	//쌍손 빔총..
			return TRUE;
	}
	return FALSE;
}

//스킬 일때만 
BOOL CAllEffectInfo::GetSpecialFarAttackFXID(DWORD weapon_id,DWORD value,DWORD *effect_id,DWORD *snd_id)	//특수 공격 처리...
{
	DWORD ef_id;
	*effect_id=-1;
	*snd_id=-1;
	BOOL ret=0;

	if( mEInfo->dwType == ETI_SKILL && value == _EF_INVOKE_FIRE) // 스킬중 발동 타이밍 코드가 필요할경우.
	{
		*snd_id =  0x3f000000|(weapon_id<<16)|(mEInfo->dwID&0x0000ffff);
//		*effect_id = 0x3f000000|(weapon_id<<16)|(mEInfo->dwID&0x0000ffff);
		*effect_id = 0x30000000|(weapon_id<<20)|(0x000d0000)|(mEInfo->dwID&0x000fffff);
		ret = TRUE;
	//	return 0x3f000000|(weapon_id<<16)|(mEInfo->dwID&0x0000ffff);
	}
	else	//일반 공격및 일반 스킬.
	{
		if( mEInfo->dwType == ETI_SKILL )
			ef_id = ( (weapon_id<<8)|0x50F10010) & 0xfffffff0 ;
		else
			ef_id = ( mEInfo->dwID|0x50F00000) & 0xfffffff0 ;

		switch( weapon_id ) 
		{
			case 0x11:	//권총..
			case 0x13:	//라이플..
			case 0x16:	//기관총..
			case 0x12:	//쌍총이면..
			case 0x17:	//화염 방사기..
			case 0x19:	//쌍손 빔총..
			case 0x18:	//양손 빔총..
			case 0x1a:	//양손 빔라이플..
			case 0x1b:	//양손프라즈마..
				if( value == _EF_FIRE )
					*effect_id = 0x100000|(ef_id & 0xff0fffff);
				else
				if( value == _EF_EMPTY_CARTRIDGE )
					*effect_id = 0x200000|(ef_id&0xff0fffff);
				else
				if( value == _EF_FIRE )
					*effect_id = 0x100000|(ef_id&0xff0fffff);
				else
				if( value == _EF_LINE_ATTACK )
					*effect_id = 0x50300000|(ef_id&0xff0fffff);
				break;
		}
		*snd_id = _ROOT_START_ATTACK_SOUND + weapon_id*50;
	}
	return ret;
}

BOOL CAllEffectInfo::SpecialFarAttackCommon(DWORD weapon_id)	//특수 공격 처리...
{
	if( mSrc == NULL || mTar == NULL || mSrcItem == NULL)	//대상과 자신이 있어야되는거다..
		return FALSE;

	CEquipItem *item = mSrcItem;

	DWORD snd_id,ef_id,unique_id;
	Vector3f pos;

	switch( weapon_id)
	{
	case 0x12:	//쌍총이면..
		GetSpecialFarAttackFXID(weapon_id,_EF_FIRE,&ef_id,&snd_id);
		::Play3DSound(snd_id,mSrcPos);
		if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE3) )
			return TRUE;
		StartMagicEffect(pos,mSrc->GetRotY()+180,ef_id);//+st_eff_code%4);

		if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE4) )
		{
			if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE3) )
				return TRUE;
		}

		if( GetSpecialFarAttackFXID(weapon_id,_EF_INVOKE_FIRE,&ef_id,&snd_id) )	//스킬발동타격 코드 일때만 
		{
			if( IsExistFromMagicSpt(ef_id))
				StartMagicEffectType(pos,mSrc->GetRotY()+180,ef_id);
			ef_id = ((ef_id&0xfff0fff0)|0x000e0000);
			if( IsExistFromMagicSpt(ef_id))
				StartMagicEffectType(pos,mSrc->GetRotY()+180,ef_id);
		}

		//탄피..
		GetSpecialFarAttackFXID(weapon_id,_EF_EMPTY_CARTRIDGE,&ef_id,&snd_id);
		StartMagicEffect(pos,0,ef_id);

		if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE1) )
			return TRUE;

		//총 불꽃
		GetSpecialFarAttackFXID(weapon_id,_EF_FIRE,&ef_id,&snd_id);
		StartMagicEffect(pos,mSrc->GetRotY()+180,ef_id);//+st_eff_code%4);
		if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE2) )
		{
			if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE1) )
				return TRUE;
		}
		//탄피..
		GetSpecialFarAttackFXID(weapon_id,_EF_EMPTY_CARTRIDGE,&ef_id,&snd_id);
		StartMagicEffect(pos,0,ef_id);
		break;
	case 0x11:	//권총..
	case 0x13:	//라이플..
	case 0x16:	//기관총..
		if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE1) )
			return TRUE;


		if( GetSpecialFarAttackFXID(weapon_id,_EF_INVOKE_FIRE,&ef_id,&snd_id) )	//스킬 일때만 
		{
			if( IsExistFromMagicSpt(ef_id))
				StartMagicEffectType(pos,mSrc->GetRotY()+180,ef_id);
		}

		GetSpecialFarAttackFXID(weapon_id,_EF_FIRE,&ef_id,&snd_id);
		::Play3DSound(snd_id,mSrcPos);
		//총 불꽃 
		StartMagicEffect(pos,mSrc->GetRotY()+180,ef_id);//+st_eff_code%4);
		if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE2) )
		{
			if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE1) )
				return TRUE;
		}
		//탄피..
		GetSpecialFarAttackFXID(weapon_id,_EF_EMPTY_CARTRIDGE,&ef_id,&snd_id);
		StartMagicEffect(pos,0,ef_id);//+st_eff_code%4);
		break;

	case 0x17:	//화염 방사기.
		if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE1) )
			return TRUE;

		if( GetSpecialFarAttackFXID(weapon_id,_EF_INVOKE_FIRE,&ef_id,&snd_id) )	//스킬 일때만 
		{
			if( IsExistFromMagicSpt(ef_id))
				StartMagicEffectType(pos,mSrc->GetRotY()+180,ef_id);
		}

		GetSpecialFarAttackFXID(weapon_id,_EF_FIRE,&ef_id,&snd_id);
		::Play3DSound(snd_id,mSrcPos);
		//화염 방사기 발사불꽃.
		Vector3f temp;
		EFUtil_GetDummyPos( temp,mTar,_POS_TYPE_DAMAGE2);
		
		float mat[4][4];
		GetMatrixFromAtoB(mat,pos,temp);	//a가 b를 향해 갈때의 매트릭스를 구한다.
		Vector3fCopy(pos,mat[3]);
		StartMagicEffect(mat,ef_id);
			 		
		//화염 방사기 타겟 불꽃.
		unique_id = StartMagicEffect(mat, 0X50F11710,0,_EF_STATE_ALWAYS_LIVE);
		if( unique_id )
		{
			_MOVE_EF_FUNC m_ef_func;
			SetDefaultMoveFunc(&m_ef_func);	//기본인자들 카피.
			m_ef_func.unique_id = unique_id;
			Vector3fCopy(mSrcPos,m_ef_func.pos);
			Vector3fCopy(mTarPos,m_ef_func.tar);
			m_ef_func.move_style = _MEM_MS_ATTACH_TARGET;
			m_ef_func.speed = 100;
			m_ef_func.attach_time=12;	//7초 동안 박혀있자.
			StartMoveEffectManager(&m_ef_func);
		}
		break;
	case 0x19:	//쌍손 빔총..
		Vector3f temp1;

		if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE3) )
			return TRUE;
		EFUtil_GetDummyPos( temp1,mTar,_POS_TYPE_DAMAGE2);

		if( GetSpecialFarAttackFXID(weapon_id,_EF_INVOKE_FIRE,&ef_id,&snd_id) )	//스킬 일때만 
		{
			if( IsExistFromMagicSpt(ef_id))
				StartMagicEffectType(pos,mSrc->GetRotY()+180,ef_id);
			ef_id = ((ef_id&0xf0ffffff)|0x08000000);
			if( IsExistFromMagicSpt(ef_id))
				StartMagicEffectType(pos,mSrc->GetRotY()+180,ef_id);
		}

		//총 불꽃 
		GetSpecialFarAttackFXID(weapon_id,_EF_FIRE,&ef_id,&snd_id);
		::Play3DSound(snd_id,mSrcPos);
		StartMagicEffect(pos,mSrc->GetRotY()+180,ef_id);

		GetSpecialFarAttackFXID(weapon_id,_EF_LINE_ATTACK,&ef_id,&snd_id);
		if( IsExistFromMagicSpt(ef_id))
		{
			DWORD unique_id = StartMagicEffect(pos,mSrc->GetRotY()+180,ef_id);
			SetAttachMoveEffectManager(unique_id,pos,temp1,_MEM_MS_ATTACH_TARGET,_POS_TYPE_DAMAGE2);//움직이는 거에 붙인다.
			SpecialARGV1(pos);
			SetSpecialIDFromMagic(unique_id,1);	//요건 1번.
		}
		GetSpecialFarAttackFXID(weapon_id,_EF_EMPTY_CARTRIDGE,&ef_id,&snd_id);
		if( IsExistFromMagicSpt(ef_id))
		{
			if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE4) )
			{
				if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE3) )
					return TRUE;
			}
			//탄피..
			StartMagicEffect(pos,0,ef_id);//+st_eff_code%4);
		}
	case 0x18:	//양손 빔총..
	case 0x1a:	//양손 빔라이플..
	case 0x1b:	//양손프라즈마..
		if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE1) )
			return TRUE;
		//총 불꽃 
		GetSpecialFarAttackFXID(weapon_id,_EF_FIRE,&ef_id,&snd_id);
		::Play3DSound(snd_id,mSrcPos);
		StartMagicEffect(pos,mSrc->GetRotY()+180,ef_id);//+st_eff_code%4);

		if( GetSpecialFarAttackFXID(weapon_id,_EF_INVOKE_FIRE,&ef_id,&snd_id) )	//스킬 일때만 
		{
			if( IsExistFromMagicSpt(ef_id))
				StartMagicEffectType(pos,mSrc->GetRotY()+180,ef_id);
		}

		EFUtil_GetDummyPos( temp1,mTar,_POS_TYPE_DAMAGE2);
		GetSpecialFarAttackFXID(weapon_id,_EF_LINE_ATTACK,&ef_id,&snd_id);
		if( IsExistFromMagicSpt(ef_id))
		{
			DWORD unique_id = StartMagicEffect(pos,mSrc->GetRotY()+180,ef_id);
			SetAttachMoveEffectManager(unique_id,pos,temp1,_MEM_MS_ATTACH_TARGET,_POS_TYPE_DAMAGE2);//움직이는 거에 붙인다.
			SpecialARGV1(pos);
			SetSpecialIDFromMagic(unique_id,1);	//요건 1번.
		}
		GetSpecialFarAttackFXID(weapon_id,_EF_EMPTY_CARTRIDGE,&ef_id,&snd_id);
		if( IsExistFromMagicSpt(ef_id))
		{
			if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE2) )
			{
				if( !EFUtil_GetCharTransformFromIndex(&pos,mSrc,item,_DUMMY_FIRE1) )
					 return TRUE;
			}
			//탄피..
			StartMagicEffect(pos,0,ef_id);
		}
		break;
	default: 
		return FALSE;
	}
	return TRUE;
}

void GetEffectString(DWORD type,DWORD id,char *name)
{
	DWORD code1;

	sprintf(name,"0x%x : ",type);
	switch(type)
	{
	case ETI_PLAYER	:
		strcat(name,"플레이어");
		break;
	case ETI_MONSTER:
		strcat(name,"몬스터");
		break;
	case ETI_ANIMUS	:
		strcat(name,"애니머스");
		break;
	case ETI_TOWER	:
		strcat(name,"타워");
		break;
	case ETI_SKILL	: 
		strcat(name,"스킬");
		code1 = ( id & 0x0000000F );
		if( code1 >= 6)
			strcat(name," 데미지");
		else
		if( code1 >= 3)
			strcat(name," 발동");
		else
			strcat(name," 시전");
		break;
	case ETI_FORCE	: 
		strcat(name,"포스 ");
		code1 = ( id & 0x000000FF ) >> 0;
		if( code1 >= 0xe )
			strcat(name," 데미지");
		else
		if( code1 >= 7 )
			strcat(name," 발동");
		else
			strcat(name," 시전");
		break;
	case ETI_ITEM	:
		strcat(name,"아이템");
		break;
	case ETI_UNIT	:
		strcat(name,"유닛");
		break;
	default	:
		strcat(name,"기타");
		break;
	}
}




void CAllEffectInfo::ProcessSpecialFarAttack()	//특수 공격 처리...
{
	if( SpecialFarAttackCommon( (mEInfo->dwID>>8)&0xff ))
	{
		//총은 즉석해서 돌려주자.목적지까지 맞았다. 수환씨한테 돌려주자....
		EFUtil_PushEffectValue(mEInfo,TRUE);
	}
}

void CheckKeyStringBuffer();	//연속키 체크하기..

extern CR3Profiler st_MainProfile;

void FrameMoveEffect()	//매루프마다...이펙트 관리.
{
	if( !_GetAvatar() )
		return;

	if( !_GetLevel()->IsLoadedBsp() )
		return;

	CheckKeyStringBuffer();	//연속키 체크하기..

/*
	if( gAsci[VK_K] )
	{
		gAsci[VK_K]=0;
		Vector3f e_pos;
		e_pos[0] = g_pAvatar->GetPosX();
		e_pos[1] = g_pAvatar->GetPosY()-rand()%5+17;
		e_pos[2] = g_pAvatar->GetPosZ();
		StartMagicEffect(e_pos,0,0x4ff10307);
	}*/
/*
	for(int g=0; g<256; g++)
	{
		if(gAsci[g])
		{
			gAsci[g]=0;
		}
	}
*/

#ifdef _DEVELOPER_
	if( gAsci[VK_LEFT_FACE] )
	{
		gAsci[VK_LEFT_FACE]=0;
		if( stMagicID>0)
			stMagicID--;
	}
	if( gAsci[VK_RIGHT_FACE] )
	{
		gAsci[VK_RIGHT_FACE]=0;
		stMagicID++;
		DWORD index = GetIDFromRealID(stMagicID);
		if(index == -1)
			stMagicID--;
	}

	if( gAsci[187] )	//+키
	{
		gAsci[187]=0;
		Vector3f e_pos;
		e_pos[0] = g_pAvatar->GetPosX();
		e_pos[1] = g_pAvatar->GetPosY()+10;
		e_pos[2] = g_pAvatar->GetPosZ()+30;
		DWORD index = GetIDFromRealID(stMagicID);
		if( index != -1 )
			StartMagicEffect(e_pos,0, index);//,_EF_STATE_ALWAYS_LIVE);
	}
	
	if( gAsci[189] )	//'-'키
	{
		gAsci[189]=0;
		DestroyEffect(); //이펙트 릴리즈
		CreateEffect();	//이펙트 초기화.
		g_Particle.ReleaseParticle();
		g_Particle.ReleaseEntity();
//		ResetEffectManager();	//이펙트 메니져들을 리셋한다.
	
		ReleaseWaveList();
		LoadWaveList(".\\snd\\WaveList.spt");
	}

	if( gAsci[VK_P] )
	{
		gAsci[VK_P]=0;
		st_eff_code++;
		st_MainProfile.CleanR3Profiler();
	}
	/*
	if( gAsci[33] ) //pg up
	{
		gAsci[33]=0;
		stState.mSoundVol+=0.1f;
		if( stState.mSoundVol > 1 )
			stState.mSoundVol=1;
		SetGlobalWavVolume(stState.mSoundVol);	//0-1까지..
	}
	if( gAsci[34] ) //pg dn
	{
		gAsci[34]=0;
		stState.mSoundVol-=0.1f;
		if( stState.mSoundVol < 0 )
			stState.mSoundVol=0;
		SetGlobalWavVolume(stState.mSoundVol);	//0-1까지..
	}
	if( gAsci[36] ) //home
	{
		gAsci[36]=0;
		stState.mMusicVol+=0.1f;
		if( stState.mMusicVol > 1 )
			stState.mMusicVol=1;
		SetGlobalMusicVolume(stState.mMusicVol);	//0-1까지..
		SetVolumeStreamMP3(1);	//0-1까지..
	}
	if( gAsci[35] ) //end
	{
		gAsci[35]=0;
		stState.mMusicVol-=0.1f;
		if( stState.mMusicVol < 0 )
			stState.mMusicVol=0;
		SetGlobalMusicVolume(stState.mMusicVol);	//0-1까지..
		SetVolumeStreamMP3(1);	//0-1까지..
	}
	*/
/*
	if( gAsci[VK_M] )
	{
		gAsci[VK_M]=0;
		Vector3f e_pos;
		e_pos[0] = g_pAvatar->GetPosX();
		e_pos[1] = g_pAvatar->GetPosY()+7;
		e_pos[2] = g_pAvatar->GetPosZ();
		StartMagicEffect(e_pos,0, 0xEFFF006C,0,_MEM_MS_CURVE);
	}
*/
	static float ffff=45;
	if( gScan[VK_U] )
	{
		ffff-=R3GetLoopTime()*30;
		_GetCamera()->SetDistance( ffff );
	}
	if( gScan[VK_J] )
	{
		ffff+=R3GetLoopTime()*30;
		_GetCamera()->SetDistance( ffff );
	}
	if( gScan[VK_Y] )
	{
		ffff-=R3GetLoopTime()*20;
		_GetCamera()->SetDistance( ffff );
	}
	if( gScan[VK_H] )
	{
		ffff+=R3GetLoopTime()*20;
		_GetCamera()->SetDistance( ffff );
	}/*
	if( gScan[VK_I] )
	{
		float temp=g_pCamera->GetPitch();
		temp+=.2f;
		g_pCamera->SetPitch(temp);
	}
	if( gScan[VK_K] )
	{
		float temp=g_pCamera->GetPitch();
		temp+=.5;
		g_pCamera->SetPitch(temp);
	}*/
#endif

	EFUtil_LoopMotionBlurTime();

	SetBspPtr(g_pLevel->mBsp);	//충돌을 위한 bsp포인터를 넣어준다.

	EFUtil_CheckDeleteAttachMagicEffect();	//캐릭터에 붙은 매직이펙트들을 점검해서 안쓰는건 지운다.

	FrameMoveEffectManager();	//이펙트 메니져 

	FrameMoveAllEffectsList();	//매직과 효과관련프래임 무브리스트

	for( EFFECT_INFO_LIST_ITER i = _GetEffectInfoList()->begin(); i != _GetEffectInfoList()->end(); )
	{
		EffectInfo *e_info = ( *i );
		CAllEffectInfo all_e_info;

		CCharacter	*ch_sou=_GetCharMgr()->GetCharacter( e_info->dwMyself[0],e_info->dwMyself[1]);
		CCharacter	*ch_tar=_GetCharMgr()->GetCharacter( e_info->dwTarget[0],e_info->dwTarget[1]);
/*
		char pBuf[128],temp[256];
		GetEffectString(e_info->dwType,e_info->dwID, temp )
		sprintf(pBuf, "!!받음 %s %x %x\n", ,e_info->dwTarget[0], e_info->dwTarget[1] );
		OutputDebugString( pBuf );
*/
//		if( e_info->dwType == ETI_FORCE )

#ifdef _DEVELOPER_
		SettingEffectInfo( e_info, ch_sou, ch_tar );		// by J.S.H
#endif
		
		all_e_info.ExtractEffectInfo( e_info );		// 이펙트 코드를 분해해서 각각 처리한다.

		i = _GetEffectInfoList()->erase( i );
	}
}

void DrawCheatDisplay();


void RenderEffect()	//매루프마다... 이펙트 뿌려주기.
{
	if( !_GetLevel()->IsLoadedBsp() )
		return;

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );	

//	stOneTrace.DrawOneTrace();
	EFUtil_RenderStringMoveOut();	// 스트링 아웃 렌더링...
	RenderAllEffectsList();
	DrawFadeTex();

	EFUtil_MessageStack();	//에러나 워닝 메시지 출력.
//	Sleep(300);
}




///효과음 새로 할것.
//몬스터 주체 함수..
void CAllEffectInfo::MonsterDamage()	//몬스터맞는 거..
{
	Vector3f out_pos;

	EFUtil_GetCharTransformFromIndex(&out_pos,mSrc,_POS_TYPE_DAMAGE2);
	//데미지 숫자 띄우기....
	DWORD su =  mEInfo->dwValue; //데미지 숫자 띄우기....
	if( su )
	{
		char buf[256];
//		sprintf(buf,"FAILURE",su);
		sprintf(buf,"%d",su);
		EFUtil_StartStringMoveOut(out_pos,buf
			,1,_OUT_STRING_COLOR);//0xffffc8c8);
		CMonsterSpecialEffectDamageAction((void *)mSrc);
		if( mSrc->GetCharTypeID() == CTI_MONSTER )
		{
			DWORD snd_id = _MOB_BASE_SOUND + (((CMonster*)mSrc)->GetEffectValue())*40 + 9;
//			DWORD snd_id = _MOB_BASE_SOUND + (((CMonster*)mSrc)->GetEffectValue()-1)*40 + 9;
			::Play3DSound(snd_id,out_pos);
		}
		DamageCommon(out_pos);
	}	
}

void CAllEffectInfo::MonsterDeath()
{
}

void CAllEffectInfo::MonsterCrazy()
{
}

void CAllEffectInfo::MonsterCreate()
{
	// 리젠		
	if( mEInfo->dwMyself[0] == CTI_MONSTER )	//현재 개무시
	{
		
		DWORD m_mat  = EFUtil_GetDefenceMaterial(mSrc);
		m_mat = EFUTIL_GetRandomMat(m_mat);
		DWORD ef_id=0x80000000|m_mat;		
		StartMagicEffect(mSrcPos,0,ef_id);
	//	::PlayWave(12045);	//사운드는.....나중에..
	}

}

static DWORD stCriticalTable[]={
	0,0,0	//스워드
	,0,0,0	//나이프
	,1,1,1,1  // 도끼
	,1,1		//메이스
	,1,1		//스태프
	,2			//스피어
	,3			//활
	,3			//석궁
	,4		//총
	,4		//쌍손총
	,4			//라이플
	,4			//런처
	,4			//파우스트
	,4			//기관총
	,4			//화염방사기
	,99999			//채굴기
};

void CAllEffectInfo::MonsterCritical()	//몬스터가 크리티컬을 당함.
{
	Vector3f out_pos;

	if( !mSrc )
		return;
	//---맞은 녀석 허리를 꺽어주자...
	CMonsterSpecialEffectDamageAction((void *)mSrc);
//	StartMagicEffect(out_pos,0,0X1FFF0048);

	if( mTarItem )
	{
		DWORD weapon_id = (mTarItem->GetMeshID() >> 8) & 0xff;
		EFUtil_GetDummyPos( out_pos, mSrc,_POS_TYPE_DAMAGE2);
		StartMagicEffect(out_pos,360 - mSrc->GetRotY(),stCriticalTable[weapon_id]|0X1FFF0040);	//크리티컬 효과는 테이블로 따라간다.
		::Play3DSound(260015+weapon_id*50,out_pos);		//사운드는 무기를 따라간다.
	}
}

void CAllEffectInfo::MonsterMiss()
{
	Vector3f pos;

	pos[0]=mSrcPos[0];
	pos[1]=mSrcPos[1]+5;
	pos[2]=mSrcPos[2];
	EFUtil_StartStringMoveOut(pos,"MISS"
//					,0|R3_HAN_BRIGHT_BLEND,0xffff0000);
		,0,_OUT_STRING_COLOR);
}

void CAllEffectInfo::MonsterAttack()
{
	//---임시
	Vector3f out_pos;

	if( !mTar )
		return;
	if( mEInfo->dwMyself[0] != CTI_MONSTER )
		return;

	if( !EFUtil_GetCharTransformFromIndex(&out_pos,mSrc,_DUMMY_FIRE1) )//	파이어 1더미가 있으면..
	{
		EFUtil_GetCharTransformFromIndex(&out_pos,mTar,_POS_TYPE_DAMAGE2);
	}
//	DWORD m_id= ((CMonster*)mSrc)->GetMeshID();
	DWORD m_id= EFUtil_GetMobIndex(mSrc);
	StartMagicEffect(out_pos,mSrc->GetRotY()+180,0x2FFF0006|(m_id<<4));


}

void CAllEffectInfo::MonsterForce()
{
}

void CAllEffectInfo::PlayerDamage1()
{
	Vector3f out_pos;
	EFUtil_GetCharTransformFromIndex(&out_pos,mSrc,_POS_TYPE_DAMAGE2);

	//데미지 숫자 띄우기....
	CMonsterSpecialEffectDamageAction((void *)mSrc);
	//DWORD su =  mSrc->GetCurDamagePoint(); //데미지 숫자 띄우기....
	DWORD su =  mEInfo->dwValue; //데미지 숫자 띄우기....
	if( su )
	{
		char buf[256];
		sprintf(buf,"%d",su);
		EFUtil_StartStringMoveOut(out_pos,buf
			,0,_OUT_STRING_COLOR);//0xffffc8c8);
	}
	DamageCommon(out_pos);
}

void CAllEffectInfo::PlayerDamage2()
{
}

void CAllEffectInfo::PlayerLevelUp()
{
	//레벨업.
	DWORD ef_id=mEInfo->dwID&0xfffffff0;
	DWORD unique_id;

	if( ef_id == 0x00010020 || ef_id == 0x00000020) //벨라토 .
	{
		unique_id = StartMagicEffectType(mSrcPos,0,0x1FFF0020);
//		SetAttachMoveEffectManager(unique_id,mSrcPos,mSrcPos,_MEM_MS_ATTACH_SOURCE,_POS_TYPE_FLOOR);//움직이는 거에 붙인다.
		::PlayWave(277015);
	}
	else
	if( ef_id == 0x00020020 || ef_id == 0x00030020) //코라.
	{
		unique_id =StartMagicEffectType(mSrcPos,0,0x1FFF0021);
//		SetAttachMoveEffectManager(unique_id,mSrcPos,mSrcPos,_MEM_MS_ATTACH_SOURCE,_POS_TYPE_FLOOR);//움직이는 거에 붙인다.
		::PlayWave(277018,0);
	}
	else
	if( ef_id == 0x00040020 ) //아크레시아.
	{
		unique_id =StartMagicEffectType(mSrcPos,0,0x1FFF0022);
//		SetAttachMoveEffectManager(unique_id,mSrcPos,mSrcPos,_MEM_MS_ATTACH_SOURCE,_POS_TYPE_FLOOR);//움직이는 거에 붙인다.
		::PlayWave(277021,0);
	}
}

void CAllEffectInfo::PlayerMoveMapStart()
{
	if( mEInfo->dwMyself[0] != CTI_AVATAR && mEInfo->dwMyself[0] != CTI_PLAYER)
		return;		//플레이어만 한다.
	CPlayer *pl=(CPlayer*)mSrc;
	pl->m_fMoveMap = 1;
	if( pl->m_dwMoveUniqueID )
		SetDisapearTimeMagicEffect(pl->m_dwMoveUniqueID,0);
	pl->m_dwMoveUniqueID=StartMagicEffect(mSrcPos,0,0X1FF700A0);
}

void CAllEffectInfo::PlayerMoveMapEnd()
{
	if( mEInfo->dwMyself[0] != CTI_AVATAR && mEInfo->dwMyself[0] != CTI_PLAYER)
		return;		//플레이어만 한다.
	CPlayer *pl=(CPlayer*)mSrc;
	pl->m_fMoveMap = -1;
	if( pl->m_dwMoveUniqueID )
		SetDisapearTimeMagicEffect(pl->m_dwMoveUniqueID,0);
	pl->m_dwMoveUniqueID=StartMagicEffect(mSrcPos,0,0X1FF700A0);
}

void CAllEffectInfo::PlayerCreate()
{
	if( mEInfo->dwMyself[0] != CTI_AVATAR )
		return;		//아바타만 리젠을 한다.

	// 리젠 
	DWORD ef_id=mEInfo->dwID&0xfffffff0;

	if( ef_id == 0x00010030 || ef_id == 0x00000030) //벨라토 .
	{
		ef_id = 0X1FFF0030;
		::PlayWave(277020);
	}
	else
	if( ef_id == 0x00020030 || ef_id == 0x00030030) //코라.
	{
		ef_id = 0X1FFF0031;
		::PlayWave(277030);
	}
	else
	if( ef_id == 0x00040030 ) //아크레시아.
	{
		ef_id = 0X1FFF0032;
		::PlayWave(277033);
	}
	DWORD unique_id = StartMagicEffectType(mSrcPos,0,ef_id);
//	SetAttachMoveEffectManager(unique_id,mSrcPos,mSrcPos,_MEM_MS_ATTACH_SOURCE,_POS_TYPE_FLOOR);//움직이는 거에 붙인다.
}

void CAllEffectInfo::PlayerCritical()	//플레이어가 크리티컬을 당함 msrc로 옴
{
	if( mSrc==NULL )//|| mTar == NULL ) 
		return;

	Vector3f out_pos;

	//---맞은 녀석 허리를 꺽어주자...
	CMonsterSpecialEffectDamageAction((void *)mSrc);

	DWORD race = EFUtil_GetRace(mSrc);
	if( race == -1 )
		return;

	EFUtil_GetDummyPos( out_pos, mSrc,_POS_TYPE_DAMAGE2);
	DWORD add_id;
	if( race == CRT_BELLATOR_MALE || race == CRT_BELLATOR_FEMALE )
		add_id = 0;
	else
	if( race == CRT_CORA_MALE || race == CRT_CORA_FEMALE )
		add_id = 1;
	else
		add_id = 2;

	StartMagicEffect(out_pos,0,0X1FFF0048 + add_id);
	::Play3DSound(277080 + add_id,out_pos);
}

void CAllEffectInfo::PlayerFailure()	//Failure
{
	Vector3f out_pos;

	EFUtil_GetCharTransformFromIndex(&out_pos,mSrc,_POS_TYPE_DAMAGE2);
	EFUtil_StartStringMoveOut(out_pos,"FAILURE"
		,1,_OUT_STRING_COLOR);//0xffffc8c8);
}

void CAllEffectInfo::Miss()
{
	Vector3f pos;

	EFUtil_GetDummyPos( pos, mSrc,_POS_TYPE_DAMAGE2);
	EFUtil_StartStringMoveOut(pos,"MISS"
//					,0|R3_HAN_BRIGHT_BLEND,0xffff0000);
		,0,_OUT_STRING_COLOR);
}	

void CAllEffectInfo::ItemDefault()
{
}



BOOL CAllEffectInfo::FarAttackCommon(DWORD ef_id,DWORD what_weapon)	//원거리 공격
{
	Vector3f ch_src_pos;
	Vector3f normal,axis={0,0,-1};
	float mat[4][4];

	Vector3f out_pos;
	EFUtil_GetCharTransformFromIndex(&out_pos,mTar,_POS_TYPE_DAMAGE2);

	if( what_weapon == 0x5 || what_weapon == 0x08 )	//던지기...
	{
		Vector3fCopy(mSrcPos,ch_src_pos);
		ch_src_pos[1]+=12;
		Vector3fSub(out_pos,ch_src_pos,normal);
		Normalize(normal);
		GetMatrixFromVector(mat,normal,axis);
		mat[3][0] = ch_src_pos[0];
		mat[3][1] = ch_src_pos[1];
		mat[3][2] = ch_src_pos[2];

//		던지는 소리는 없다...
//		DWORD d_index=(mEInfo->dwID & 0x0000ff00)>>8;
//		Play3DSound(stWeponWavTable[d_index],mat[3]);
		float dist = GetDist(ch_src_pos,out_pos);
		DWORD unique_id = StartMagicEffect(mat,ef_id,0,_EF_STATE_ALWAYS_LIVE);
		if( unique_id )
		{
			_MOVE_EF_FUNC m_ef_func;

			SetDefaultMoveFunc(&m_ef_func);	//기본인자들 카피.
			m_ef_func.unique_id = unique_id;
			EFUtil_GetDummyPos( m_ef_func.pos, mSrc,_DUMMY_FIRE1);
			EFUtil_GetDummyPos( m_ef_func.tar, mTar,_POS_TYPE_DAMAGE2);
			m_ef_func.move_style = _MEM_MS_TRACE_TARGET;
			m_ef_func.speed = 145;
			m_ef_func.attach_time=0;	//7초 동안 박혀있자.
			StartMoveEffectManager(&m_ef_func);
		}
		return TRUE;
	}
	else
	if( what_weapon == 0x14 ||  what_weapon == 0x15 )//0x50F11410://런쳐,파우스트 ..
	{
		if( mSrc == NULL || mTar == NULL || mSrcItem == NULL )	//대상과 자신 들고 있는 아이템 있어야되는거다..
			return TRUE;
		if( mEInfo->dwType == ETI_SKILL )	//스킬일경우는 그대로 
			::Play3DSound(ef_id,mSrcPos);
		else
			::Play3DSound(261000 + (rand()%2)*3,mSrcPos);
		Vector3f temp;
		float mat[4][4];

		if( !EFUtil_GetCharTransformFromIndex(&temp,mSrc,mSrcItem,_DUMMY_FIRE1) )
			return TRUE;
		GetMatrixFromAtoB(mat,mSrcPos,mTarPos);	//a가 b를 향해 갈때의 매트릭스를 구한다.

		//런처 불꽃 
		Vector3fCopy(temp,mat[3]);
		StartMagicEffect(mat,0X50111410);//런쳐 불꽃꼬.
		//런쳐 알...
		DWORD unique_id = StartMagicEffect(mat, ef_id,0,_EF_STATE_ALWAYS_LIVE);
		if( unique_id )	//자 발사해볼까...
		{
			_MOVE_EF_FUNC m_ef_func;
			SetDefaultMoveFunc(&m_ef_func);	//기본인자들 카피.
			m_ef_func.bullet_ID_for_damage=mBulletIDForDamage;
			m_ef_func.unique_id = unique_id;
			Vector3fCopy(mat[3],m_ef_func.pos);
			EFUtil_GetDummyPos( m_ef_func.tar, mTar,_POS_TYPE_DAMAGE2);
			m_ef_func.move_style = _MEM_MS_TRACE_TARGET;//_MEM_MS_DIRECTION;
			m_ef_func.speed = 248;
			m_ef_func.attach_time=0;//런쳐는 박히지 않는다.
			StartMoveEffectManager(&m_ef_func);
		}
		//런쳐 후폭풍.
		if( !EFUtil_GetCharTransformFromIndex(&temp,mSrc,mSrcItem,_DUMMY_FIRE2) )
			return TRUE;
		Vector3fCopy(temp,mat[3]);
		StartMagicEffect(mat,0X50111420);//런쳐 후폭풍.
		return TRUE;
	}
	else	//보우나 석궁.. 던지기...
	if( what_weapon == 0xf ||  what_weapon == 0x10 )	//활과 석궁 기냥..
	{
//		DWORD bullet_id = mSrc->GetBulletDTIndex();
		if( !EFUtil_GetCharTransformFromIndex(&ch_src_pos,mSrc,mSrcItem,_DUMMY_FIRE1) )
		{
			Vector3fCopy(mSrcPos,ch_src_pos);
			ch_src_pos[1]+=12;
		}

		Vector3fSub(out_pos,ch_src_pos,normal);
		Normalize(normal);
		GetMatrixFromVector(mat,normal,axis);

//		화살은 나가는 것만 처리한다.
		mat[3][0] = ch_src_pos[0];
		mat[3][1] = ch_src_pos[1];
		mat[3][2] = ch_src_pos[2];

		DWORD temp_ef_id;
		DWORD snd_id;
		if( GetSpecialFarAttackFXID(what_weapon,_EF_INVOKE_FIRE,&temp_ef_id,&snd_id) )	//스킬 일때만 
		{
			if( IsExistFromMagicSpt(temp_ef_id))
				StartMagicEffectType(mat[3],mSrc->GetRotY()+180,temp_ef_id);
		}

		//화살 나가는 소리..
//		Play3DSound(stWeponWavTable[d_index],mat[3]);
		if( mEInfo->dwType == ETI_SKILL )	//스킬일경우는 그대로 
			::Play3DSound(ef_id,mSrcPos);
		else
		{
			DWORD d_index=(mEInfo->dwID & 0x0000ff00)>>8;
			Play3DSound(_ROOT_START_ATTACK_SOUND + d_index*50 + (rand()%2)*3,mat[3]);	//무기 사용시 해당 무기아이디와 위치를 날려주면플레이...
		}

		float dist = GetDist(ch_src_pos,out_pos);
		DWORD unique_id = StartMagicEffect(mat, ef_id,0,_EF_STATE_ALWAYS_LIVE);
		if( unique_id )
		{
			_MOVE_EF_FUNC m_ef_func;

			SetDefaultMoveFunc(&m_ef_func);	//기본인자들 카피.

			m_ef_func.unique_id = unique_id;
			Vector3fCopy( ch_src_pos,m_ef_func.pos);
			EFUtil_GetDummyPos( m_ef_func.tar, mTar,_POS_TYPE_DAMAGE2);
			m_ef_func.move_style = _MEM_MS_DIRECTION;
//			m_ef_func.flag = _EF_STATE_ALWAYS_LIVE;
			m_ef_func.speed = dist*1.8f+145;
			m_ef_func.attach_time=9;//;//12;	//7초 동안 박혀있자.
			StartMoveEffectManager(&m_ef_func);
//			StartMoveEffectManager(mEInfo,unique_id,ch_src_pos,out_pos,_MEM_MS_DIRECTION,dist*1.8f+145);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CAllEffectInfo::ProcessFarAttack()	//원거리 공격
{
	DWORD what_weapon = (mEInfo->dwID & 0x0000ff00)>>8;
	BOOL ret = FALSE;

	if( what_weapon == 0x5 || what_weapon == 0x08 )	//던지기...
	{
		if( mEInfo->dwMyself[0] == CTI_AVATAR ||  mEInfo->dwMyself[0] == CTI_PLAYER )
		{
			DWORD l_dwWeaponMeshID = ((CPlayer *)mSrc)->GetEquipItem( CEPT_WEAPON )->GetMeshID();
			ret = FarAttackCommon(l_dwWeaponMeshID|0xef000000,what_weapon);
		}
	}
	else
	if( what_weapon == 0x14 ||  what_weapon == 0x15 )//0x50F11410://런쳐,파우스트 ..
	{
		if( mEInfo->dwMyself[0] == CTI_AVATAR ||  mEInfo->dwMyself[0] == CTI_PLAYER )
		{
			mBulletIDForDamage = ((CPlayer *)mSrc)->GetBulletDTIndex();	//바닥 데미지를 위한 변수
			ret = FarAttackCommon(mBulletIDForDamage|0xefff0000,what_weapon);
		}
	}
	else
	//보우나 석궁.. 던지기...
	if( what_weapon == 0xf ||  what_weapon == 0x10 )	//활과 석궁 기냥..
	{
		if( mEInfo->dwMyself[0] == CTI_AVATAR ||  mEInfo->dwMyself[0] == CTI_PLAYER )
		{
			DWORD bow_effect_id = ((CPlayer *)mSrc)->GetBulletDTIndex();
			bow_effect_id = (bow_effect_id)|0xefff0000;
			ret = FarAttackCommon(bow_effect_id,what_weapon);
		}
	}
	return ret;
}



void CAllEffectInfo::Make2ndLightTarget()
{
	Vector3f pos;

	EFUtil_GetDummyPos( pos, mTar,_POS_TYPE_DAMAGE2);

	mTar->Set2ndLightPos(pos);
	mTar->Set2ndDensity(1.0f);	//타격시 맞는 놈의 라이팅....
}



void CAllEffectInfo::DamageCommon(Vector3f pos)		//현재는 공격공통이다. 
{
	DWORD m_mat = EFUtil_GetDefenceMaterial(mSrc);
	DWORD a_mat = EFUtil_GetAttackMaterial(mTar);
	DWORD snd_id = EFUTIL_GetSoundMat(a_mat,mSrcMat) + _ROOT_DAMAGE_SOUND;

	Play3DSound(snd_id,pos);

	Vector3f normal;
	float mat[4][4],s_mat[4][4];

	Vector3f out_pos;
	EFUtil_GetCharTransformFromIndex(&out_pos,mSrc,_POS_TYPE_DAMAGE2);

	float scale = (mSrcBBox.pMax[1]-mSrcBBox.pMin[1]);
	scale = sqrt(scale)*mSrc->GetScale()*0.2f;
	scale=min(scale,2);
	scale=max(scale,0.5f);
	MatrixScale(s_mat,scale,scale,scale);
	if( mTar )
	{
		Vector3fSub(mTarPos,mSrcPos,normal);	//맞는 쪽의 이펙트가 주체므로.
		Normalize(normal);
		GetMatrixFromAtoB(mat,mTarPos,mSrcPos);	//a가 b를 향해 갈때의 매트릭스를 구한다.
	}
	else
	{
		MatrixRotate(mat,0,mSrc->GetRotY(),0); //단위는 360도
	}


	MatrixMultiply(mat,s_mat,mat);
//	MatrixMultiply(mat,mat,s_mat);

	mat[3][0] = out_pos[0];
	mat[3][1] = out_pos[1];
	mat[3][2] = out_pos[2];

	DWORD ef_id;
	m_mat = EFUTIL_GetRandomMat(m_mat);
	// 통합 데미지 효과.
	ef_id = ( m_mat|0X80000010);
	DWORD d_index=(mEInfo->dwID & 0x0000ff00)>>8;
	StartMagicEffect(mat,ef_id);
}

void CAllEffectInfo::ItemAttack()
{
	if( mTar == 0)	//맞는 주체가 반드시 있어야된다.....
		return;

	if( ProcessFarAttack() )	//원거리 공격이면...
		return;

	Vector3f normal,axis={0,0,-1};
	float mat[4][4],s_mat[4][4];

	Vector3f out_pos;
	EFUtil_GetCharTransformFromIndex(&out_pos,mTar,_POS_TYPE_DAMAGE2);

	float scale = (mTarBBox.pMax[1]-mTarBBox.pMin[1]);
	scale = sqrt(scale)*mTar->GetScale()*0.2f;
	scale=min(scale,2);
	scale=max(scale,0.5f);
	MatrixScale(s_mat,scale,scale,scale);

	Vector3fSub(mSrcPos,mTarPos,normal);	//맞는 쪽의 이펙트가 주체므로.
//	Vector3fSub(mTarPos,mSrcPos,normal);
	Normalize(normal);
	GetMatrixFromAtoB(mat,mSrcPos,mTarPos);	//a가 b를 향해 갈때의 매트릭스를 구한다.

	MatrixMultiply(mat,s_mat,mat);
//	MatrixMultiply(mat,mat,s_mat);

	mat[3][0] = out_pos[0];
	mat[3][1] = out_pos[1];
	mat[3][2] = out_pos[2];
/*
	DWORD m_mat;
	DWORD ef_id;
	m_mat  = EFUtil_GetDefenceMaterial(mTar);
	m_mat = EFUTIL_GetRandomMat(m_mat);
	// 통합 데미지 효과.
	ef_id = ( m_mat|0X80000010);
	DWORD d_index=(mEInfo->dwID & 0x0000ff00)>>8;
	StartMagicEffect(mat,ef_id);
*/
	DWORD ef_id;

	if( mEInfo->dwMyself[0] == CTI_AVATAR ||  mEInfo->dwMyself[0] == CTI_PLAYER )
	{
		((CPlayer *)mSrc)->SetTraceOfWeapon(FALSE);		//궤적은 동작 끝날때보단 타격이 끝날경우 없어져야 간결하고 보기좋다.
	}

	// 타격 효과.
	ef_id = ( mEInfo->dwID|0x50F00000) & 0xfffffff0 ;
	StartMagicEffect(mat,ef_id);

	ProcessSpecialFarAttack();	//특수 공격 처리...총계열
}

void CAllEffectInfo::ItemAttr()
{
}
void CAllEffectInfo::ItemUse()
{
	DWORD race=(mEInfo->dwID>>20);
	DWORD attr=(mEInfo->dwID&0x0000ff00);
	DWORD unique_id;
	if( attr == 0x0d00 )		//베틀던젼.
	{
		if( !mSrcItem )
			return;
		Vector3f pos;
		EFUtil_GetCharTransformFromIndex(&pos,mSrc,mSrcItem,_DUMMY_FIRE1);
		StartMagicEffectType(pos,0,0x50F20030|attr);
		::Play3DSound(0x50F20030|attr,pos);				 
		return;
	}

	if( race == 7 )	//벨라토
	{
		unique_id = StartMagicEffectType(mSrcPos,0,0x50F20030|attr);
		if( (attr>>8) <3 )	//hp,force,스테미너 포션
			::Play3DSound(278000,mSrcPos);	// 위치를 날려주면플레이...
		if( attr&0x0000ff00 == 0x0500)
			::Play3DSound(278009,mSrcPos);	// 아드레날린
	}
	else
	if( race == 8 )	//코라
	{
		unique_id = StartMagicEffectType(mSrcPos,0,0x50F20031|attr);
		if( (attr>>8) <3 )	//hp,force,스테미너 포션
			::Play3DSound(278003,mSrcPos);	// 위치를 날려주면플레이...
		if( attr&0x0000ff00 == 0x0500)
			::Play3DSound(278012,mSrcPos);	// 아드레날린
	}
	else
//	if( race == 4 )	//아크레시아..
	{
		unique_id = StartMagicEffectType(mSrcPos,0,0x50F20032|attr);
		if( (attr>>8) <3 )	//hp,force,스테미너 포션
			::Play3DSound(278006,mSrcPos);	// 위치를 날려주면플레이...
		if( attr&0x0000ff00 == 0x0500 )
			::Play3DSound(278015,mSrcPos);	// 아드레날린
	}
}
 
void CAllEffectInfo::ItemSkill()  //
{
	//스킬 기본 데미지...
	if(!mTar )
		return;
	Vector3f pos;
	EFUtil_GetDummyPos( pos, mTar,_POS_TYPE_DAMAGE2);
	//DWORD su =  mTar->GetCurDamagePoint(); //데미지 숫자 띄우기....
	DWORD su =  mEInfo->dwValue; //데미지 숫자 띄우기....
	if( su )
	{
		char buf[256];
		sprintf(buf,"%d",su);
		EFUtil_StartStringMoveOut(pos,buf
			,1,_OUT_STRING_COLOR);//0xffffc8c8);
	}
}

void CAllEffectInfo::ItemForce()
{
}


void CAllEffectInfo::OutputDamage(CCharacter *src)		//	데미지숫자포인트 띄우기
{
	if(!src )
		return;
	Vector3f pos;
	EFUtil_GetDummyPos( pos, src,_POS_TYPE_DAMAGE2);
	DWORD su =  mEInfo->dwValue; //데미지 숫자 띄우기....
	if( su )
	{
		char buf[256];
		sprintf(buf,"%d",su);
		EFUtil_StartStringMoveOut(pos,buf
			,1,_OUT_STRING_COLOR);//0xffffc8c8);
	}
}

void CAllEffectInfo::SkillPrepare()
{
	float yangle;
	if( mTar && mSrc != mTar )
		yangle = GetYAngle( mSrcPos,mTarPos );
	else
		yangle = mSrc->GetRotY();

	DWORD ef_id = ( (mEInfo->dwID&0xf00fffff) |0x30000000);
	DWORD weapon_id;

	DWORD type= (mEInfo->dwID&0x000f0000)>>16;

	if( mSrcItem && type == 0) //근접일때만...
	{
		weapon_id = (mSrcItem->GetMeshID() >> 8) & 0xff;
		ef_id |= (weapon_id<<20);
	}
	if( type == 1 )	//원거리 일경우 
	{
		if( mSrcItem && ( mEInfo->dwMyself[0] == CTI_AVATAR ||  mEInfo->dwMyself[0] == CTI_PLAYER ))
		{
			weapon_id = (mSrcItem->GetMeshID() >> 8) & 0xff;
			ef_id|=(weapon_id<<20);		//무기별로 들어가니까..
			if( IsTwinWeapon(weapon_id) )
				StartMagicEffectType(mSrcPos,180+yangle,(ef_id&0xfff0ffff)|0x000e0000);
		}
	}

	DWORD unique_id=StartMagicEffectType(mSrcPos,180+yangle,ef_id);
/*
	DWORD what_a = (mEInfo->dwID&0x000f0000)>>16;	//어떤 스킬 대분류인지....
	DWORD what_b = (mEInfo->dwID&0x0000fff0)>>4;	//어떤 스킬인지....
	DWORD what_c = (mEInfo->dwID&0x000000f);
	DWORD snd_id = _ROOT_SKILL_SOUND + what_a*4608 + what_b*18 + what_c*6;
*/
	DWORD snd_id;
	if( IsExistFromSoundSpt(ef_id))
		snd_id = ef_id;
	else
		snd_id = ef_id&0xf00fffff;

	::Play3DSound(snd_id,mSrcPos);
	
	if( (ef_id & 0x000f0000) != 0x000f0000 ) //공격스킬일경우 
	{
		if( mSrc->GetCharTypeID() == CTI_AVATAR || mSrc->GetCharTypeID() ==CTI_PLAYER )
			((CPlayer *)mSrc)->SetTraceOfWeapon(TRUE);
		if( mSrc->GetCharTypeID() == CTI_AVATAR )
			EFUTIL_SetBattleMusicState();	//아바타라면 배틀 사운드를 위한.
	}
}

void CAllEffectInfo::SkillDamage()
{
	float yangle;
	if( mTar )
	{
		if( mSrc == mTar )
			yangle = mSrc->GetRotY();
		else
			yangle = GetYAngle( mSrcPos,mTarPos );
	}
	else
		yangle = mSrc->GetRotY();

	DWORD ef_id = ( (mEInfo->dwID&0xf00fffff) |0x30000000);

	ef_id|=0x0F100000;
	if( IsExistFromMagicSpt(ef_id) )	//존재하면 바닥데미지
	{
		StartMagicEffectType(mSrcPos,180+yangle,ef_id);
	}
/*
	ef_id=0x00F40000|(ef_id&0xFFF0FFFF);
	if( IsExistFromMagicSpt(ef_id) )	//존재하면 붙어다니는 바닥데미지
	{
		DWORD unique_id = StartMagicEffectType(mSrcPos,180+yangle,ef_id);
		SetAttachMoveEffectManager(unique_id,mSrcPos,mTarPos,_MEM_MS_ATTACH_SOURCE,_POS_TYPE_FLOOR);//움직이는 거에 붙인다.
	}
*/
	ef_id|=0x0FF00000;
	Vector3f pos;
	EFUtil_GetDummyPos( pos, mSrc,_POS_TYPE_DAMAGE2);
	if( IsExistFromMagicSpt(ef_id))		//일반스킬데미지 
		StartMagicEffectType(pos,180+yangle,ef_id);
	if( mTarItem )
	{
		DWORD weapon_id = (mTarItem->GetMeshID() >> 8) & 0xff;
		if( IsExistFromMagicSpt((ef_id&0xf00fffff)<<weapon_id<<20))		//특수 스킬 데미지 무기 따라갈것.
			StartMagicEffectType(pos,180+yangle,ef_id);
	}
/*
	DWORD what_a = (mEInfo->dwID&0x000f0000)>>16;	//어떤 스킬 대분류인지....
	DWORD what_b = (mEInfo->dwID&0x0000fff0)>>4;	//어떤 스킬인지....
	DWORD what_c = (mEInfo->dwID&0x0000000f)-6;
	*/
	DWORD snd_id;
	if( mTarItem && ( mEInfo->dwTarget[0] == CTI_AVATAR ||  mEInfo->dwTarget[0] == CTI_PLAYER ))
	{
		DWORD weapon_id = (mTarItem->GetMeshID() >> 8) & 0xff;
	//	DWORD snd_id = _ROOT_SKILL_SOUND + what_a*4608 + what_b*18 + what_c*6 + 2;
		if( IsExistFromSoundSpt( (ef_id&0xf00fffff)|weapon_id<<20 ) )
			snd_id = ((ef_id&0xf00fffff)|weapon_id<<20 );
		else
			snd_id = ef_id&0xf00fffff;
	}
	else
		snd_id = ef_id&0xf00fffff;
	::Play3DSound(snd_id,pos);

	ef_id &= 0xf0ffffff;
	if( mTar )
	{
		ef_id |= ((mTar->GetSkillAtackCnt()&0xf)<<20);
		if( IsExistFromMagicSpt(ef_id))		//	카운팅 스킬 이펙트가 존재하면
			StartMagicEffectType(pos,180+yangle,ef_id);
		mTar->AddSkillAtackCnt();	//스킬때리는 카운트를 알기위한 증가.
	}

	if( (ef_id & 0x000f0000) != 0x000f0000 )	//보조스킬이 아닌경우 사운드와 스킬 데미지를 안뿌린다.
	{
//		ef_id = ( mSrcMat|0x20000000);	//재질 데미지 추가.
//		StartMagicEffectType(pos,180+yangle,ef_id);
		OutputDamage(mSrc);	//스킬 데미지 표시
//		::Play3DSound(base_sound + add_sound,pos);
	}
}

void CAllEffectInfo::SkillInvoke()
{
	ForceSkillInvokeCommon();
}


void CAllEffectInfo::ForceDamage()		//데미지.
{
	DWORD what_a = (mEInfo->dwID&0x000f0000)>>16;	//어떤 포스 대분류인지....
	DWORD what_b = (mEInfo->dwID&0x0000ff00)>>8;	//어떤 포스인지....
	DWORD what_c = (mEInfo->dwID&0x000000ff)-7;
	if( what_c == 6 )
		what_c = 12;
	else
	if( what_c >= 3 )
		what_c = 6;
	else
		what_c = 0;
	DWORD snd_id = _ROOT_FORCE_SOUND + what_a*4608 + what_b*18 + what_c + 2;

//	if( mSrc == NULL )
//		return;
	//DWORD su = mSrc->GetCurDamagePoint(); //데미지 숫자 띄우기....
	DWORD su =  mEInfo->dwValue; //데미지 숫자 띄우기....
	if( su )
	{
		char buf[256];
		Vector3f temp_pos;
		sprintf(buf,"%d",su);
		EFUtil_GetDummyPos( temp_pos, mSrc,_POS_TYPE_DAMAGE2);
		EFUtil_StartStringMoveOut(temp_pos,buf
			,1,_OUT_STRING_COLOR);//0xffffc8c8);
	}

	Vector3f temp_pos;
	if( mTar == NULL )
		return;
	DWORD effect_id = ((mEInfo->dwID&0xff0fffff)|0x4F200000);

	EFUtil_GetDummyPos( temp_pos, mSrc,_POS_TYPE_DAMAGE2);
	float goc = R3API::GetYAngle( mTarPos,mSrcPos );

	BYTE type = mEInfo->byRangeType_For_Effect;
	if( type == _F_ONLY_THROW )	//A형은 캐릭터 데미지를 붙이지 않는다.
	{
		::Play3DSound(snd_id,temp_pos);
		StartMagicEffectType(temp_pos,goc, effect_id);//포스 데미지 캐릭터 위치.
		return;
	}
	DWORD unique_id=StartMagicEffectType(temp_pos,goc, effect_id,mEInfo,_EF_STATE_MYSELF_LIVE);//데미지 캐릭터 위치.
	::Play3DSound(snd_id,temp_pos);

	effect_id=0x0F400000|(effect_id&0xFF0FFFFF);
	if( IsExistFromMagicSpt(effect_id) )	//존재하면 붙어다니는 바닥데미지
	{
		unique_id = StartMagicEffectType(mSrcPos,goc,effect_id);
	}
}

void CAllEffectInfo::ForceSkillInvokeCommon()			//포스 스킬 발동기본.
{
	Vector3f ch_src_pos,ch_tar_pos;
	float mat[4][4];
	DWORD main_ef_id,floor_ef_id;
	DWORD what_a,what_b,what_c;
	DWORD snd_id;
	DWORD weapon_id;

	if( mEInfo->dwType == ETI_FORCE )
	{
		what_a = (mEInfo->dwID&0x000f0000)>>16;	//어떤 포스 대분류인지....
		what_b = (mEInfo->dwID&0x0000ff00)>>8;	//어떤 포스인지....
		what_c = (mEInfo->dwID&0x000000ff)-7;
		if( what_c == 6 )
			what_c = 12;
		else
		if( what_c >= 3 )
			what_c = 6;
		else
			what_c = 0;

		main_ef_id = mEInfo->dwID|0x4FF00000;
		floor_ef_id = 0x0F400000|(main_ef_id&0xFF0FFFFF);
		snd_id = _ROOT_FORCE_SOUND + what_a*4608 + what_b*18 + what_c + 1;
	}
	else
	if( mEInfo->dwType == ETI_SKILL )
	{
		/*
		what_a = (mEInfo->dwID&0x000f0000)>>16;	//어떤 스킬 대분류인지....
		what_b = (mEInfo->dwID&0x0000fff0)>>4;	//어떤 스킬인지....
		what_c = (mEInfo->dwID&0x0000000f)-3;
		snd_id = _ROOT_SKILL_SOUND + what_a*4608 + what_b*18 + what_c*6 + 1;
		*/
		if( mSrcItem )
			weapon_id = (mSrcItem->GetMeshID() >> 8) & 0xff;
		main_ef_id = mEInfo->dwID&0xf00fffff;
		main_ef_id = main_ef_id|0x30000000|(weapon_id<<20) ;
		floor_ef_id = -1;

		if( IsExistFromSoundSpt(main_ef_id))
			snd_id = main_ef_id;
		else
			snd_id = main_ef_id&0xf00fffff;
	}

	BYTE type = mEInfo->byRangeType_For_Effect;	//던지기 유형.
	if( IsExistFromMagicSpt(floor_ef_id) )	//존재하면 붙어다니는 바닥데미지
	{
		float yangle = EFUtil_GetAngle(mSrc,mTar);
		SetTargetAttachMagicEffectType();	//타겟에 붙이기위해서
		DWORD unique_id = StartMagicEffectType(mTarPos,180+yangle,floor_ef_id);
//		SetAttachMoveEffectManager(unique_id,mSrcPos,mTarPos,_MEM_MS_ATTACH_TARGET,_POS_TYPE_FLOOR);//움직이는 거에 붙인다.
	}

	if( type == _F_ONLY_NONE_THROW )
	{
//		EFUtil_PushEffectValue(mEInfo,TRUE);
		::Play3DSound(snd_id,mSrcPos);
		StartMagicEffectType(mSrcPos,0, main_ef_id,mEInfo);
		return;
	}

	if( mTar == NULL )	//대상이 없는경우 땅을 찍은경우다.
	{
		if( type == _F_NONE_THROW_MAP_ONE_HIT )	//땅을 찍어서 발동 하는 경우. 이건 수환씨한테 보냄 안된다.
		{
//			_GetReturnEffectInfoList()->push_back( mEInfo );
			::Play3DSound(snd_id, mEInfo->dwTargetPos);
			SetTargetAttachMagicEffectType();	//타겟에 붙이기위해서
			StartMagicEffectType(mEInfo->dwTargetPos,0, main_ef_id,mEInfo);
		}
		return;
	}
	float yangle = GetYAngle( mSrcPos,mTarPos )+180;
	if( type == _F_NONE_THROW_ONE_HIT )
	{
		EFUtil_PushEffectValue(mEInfo,TRUE);
		::Play3DSound(snd_id,mTarPos);
		
		SetTargetAttachMagicEffectType();	//타겟에 붙이기위해서
		DWORD unique_id=StartMagicEffectType(mTarPos,yangle, main_ef_id,mEInfo,_EF_STATE_MYSELF_LIVE);//포스 데미지 캐릭터 위치.
//		SetAttachMoveEffectManager(unique_id,mSrcPos,mTarPos,_MEM_MS_ATTACH_TARGET,_POS_TYPE_DAMAGE2);//움직이는 거에 붙인다.
		return;
	}
	if( type == _F_NONE_THROW_FLASH || type == _F_NONE_THROW_EXTENT )
	{
		//이건 보내면 안된다.
//		_GetReturnEffectInfoList()->push_back( mEInfo );
		::Play3DSound(snd_id,mTarPos);
		SetTargetAttachMagicEffectType();	//타겟에 붙이기위해서
		StartMagicEffectType(mTarPos,yangle, main_ef_id,mEInfo);
		return;
	}

	Vector3fCopy(mTarPos,ch_tar_pos);
	Vector3fCopy(mSrcPos,ch_src_pos);
	ch_src_pos[1]+=12;
	ch_tar_pos[1]+=12;

//	GetMatrixFromAtoB(mat,ch_src_pos,ch_tar_pos);	//a가 b를 향해 갈때의 매트릭스를 구한다.
	GetMatrixFromAtoB(mat,ch_tar_pos,ch_src_pos);	//a가 b를 향해 갈때의 매트릭스를 구한다.
	float dist = GetDist(ch_src_pos,ch_tar_pos);

	mat[3][0] = ch_src_pos[0];
	mat[3][1] = ch_src_pos[1];
	mat[3][2] = ch_src_pos[2];

	if(  type == _F_ONLY_THROW ||  type == _F_THROW_PASS_ONE_HIT 
	||  type == _F_THROW_FLASH ||  type == _F_THROW_EXTENT || type == _F_MULTI_HIT)	//던지기 종류닷..
	{
		BOOL no_collison_check = FALSE;
		::Play3DSound(snd_id,mSrcPos);
		if( mEInfo->dwType == ETI_SKILL )	//스킬 던지기류처리...
		{
			if( mSrcItem && ( mEInfo->dwMyself[0] == CTI_AVATAR ||  mEInfo->dwMyself[0] == CTI_PLAYER ))
			{
				DWORD bullet_id = ((CPlayer *)mSrc)->GetBulletDTIndex();	//바닥 데미지를 위한 변수
				weapon_id = (mSrcItem->GetMeshID() >> 8) & 0xff;

				if( IsSeeBullet(weapon_id) )	//날아가는 것이 보이느냐 안보이느냐로 구분. 
				{
					no_collison_check = TRUE;
					FarAttackCommon((bullet_id)|0xefff0000,weapon_id);	//원거리 공격
					DWORD temp_id = 0x30000000|(weapon_id<<20)|(mEInfo->dwID&0x000fffff);
					temp_id = (temp_id&0xfffffff0)|0x0000000f;
					if( IsExistFromMagicSpt(temp_id))
						StartMagicEffectType(mat,temp_id);
				}
				else
				{
//					FarAttackCommon(0x3f000000|(bullet_id<<16)|(mEInfo->dwID&0x0000ffff),weapon_id);	//원거리 공격
					if( SpecialFarAttackCommon( weapon_id ))
					{
						//총은 즉석해서 돌려주자.목적지까지 맞았다. 수환씨한테 돌려주자....
						EFUtil_PushEffectValue(mEInfo,TRUE);
					}
					return;
				}
			}
		}
		else	//포스 발동 타이밍 이펙트
		{
			DWORD temp_id = (main_ef_id&0xff0fffff)|0x00700000;
			if( IsExistFromMagicSpt(temp_id))
				StartMagicEffectType(mat,temp_id);
		}

		DWORD unique_id = StartMagicEffect(mat, main_ef_id,0,_EF_STATE_ALWAYS_LIVE);
		_MOVE_EF_FUNC m_ef_func;
		SetDefaultMoveFunc(&m_ef_func);
		m_ef_func.unique_id = unique_id;
		m_ef_func.move_style = _MEM_MS_TRACE_TARGET;
		EFUtil_GetDummyPos( m_ef_func.pos, mSrc,_DUMMY_FIRE1);
		EFUtil_GetDummyPos( m_ef_func.tar, mTar,_POS_TYPE_DAMAGE2);
		m_ef_func.speed = dist*1.0f+45;
		m_ef_func.move_style = _MEM_MS_TRACE_TARGET;
		if( no_collison_check )
			m_ef_func.flag |= _MEM_MS_FLAG_COLLISION_NO_RETURN;		//원거리 스킬 디폴트 공격이 충돌첵크를 했기때문에 추가로 날기는건 뺀다.

		//-----시작 위치특이하게 되는경우.
		CMagicList *magic = GetMagicListFromID(unique_id);
		if( magic )
		{
			Vector3f temp;
			CProcessMagic *pMagic = (CProcessMagic *)magic->mMagicPtr;
			if( pMagic )
			{
				if( EFUTIL_GetPosType(temp, mSrc, pMagic->mCreateType))	//타잎별 포지션이 있으면 변경한다.
				{
					Vector3fCopy(temp,magic->mMat[3]);
					Vector3fCopy(temp,m_ef_func.pos);
				}
				else
				{
					EFUTIL_GetPosCharFromPos(&m_ef_func.pos,mSrc,pMagic->mCreatePos);
					Vector3fCopy(m_ef_func.pos,magic->mMat[3]);
				}
				m_ef_func.speed = pMagic->mSpeed;
			}
		}

		if( mEInfo->dwType == ETI_SKILL )	//스킬 던지기류처리...
		{
			if( mSrcItem && ( mEInfo->dwMyself[0] == CTI_AVATAR ||  mEInfo->dwMyself[0] == CTI_PLAYER ))
			{
				weapon_id = (mSrcItem->GetMeshID() >> 8) & 0xff;
				if( IsSeeBullet(weapon_id) )	//날아가는 것이 보이느냐 안보이느냐로 구분. 
				{
					m_ef_func.move_style = _MEM_MS_DIRECTION;
					m_ef_func.speed = dist*1.8f+145;
				}
			}
		}

		m_ef_func.attach_time=0;	//스킬및 포스는 ...
		StartMoveEffectManager(&m_ef_func);
	}
	if( type == _F_THREE_HIT )	//땅을 찍어서 발동 하는 경우.
	{
		::Play3DSound(snd_id,mSrcPos);
	}
}

void CAllEffectInfo::ForceInvoke()		//발동.
{
	ForceSkillInvokeCommon();		//원거리 어택기본에 다있다.
}

// 생성 type별로 작업되어지는것.
DWORD CAllEffectInfo::StartMagicEffectType(Vector3f pos,float y_angle, DWORD magic_id,void *e_Info,DWORD state)
{
	DWORD unique_id = StartMagicEffect(pos,y_angle,magic_id,e_Info,state);
//	if( (magic_id>>28) == 4)
//		int dfsdf=0;
	CMagicList *magic = GetMagicListFromID(unique_id);
	if( magic )
	{
		Vector3f temp;
		CProcessMagic *pMagic = (CProcessMagic *)magic->mMagicPtr;
		if( pMagic )
		{
			CCharacter *ch;
			if( stTarAttach )
				ch = mTar;
			else
				ch = mSrc;
			if( EFUTIL_GetPosType(temp, ch, pMagic->mCreateType))	//타잎별 포지션이 있으면 변경한다.
			{
				Vector3fCopy(temp,magic->mMat[3]);				
			}
			if( pMagic->mFlag & _PM_FLAG_ATTACH )
			{
				if( stTarAttach )
					SetAttachMoveEffectManager(unique_id,mSrcPos,mTarPos,_MEM_MS_ATTACH_TARGET,pMagic->mCreateType);//움직이는 거에 붙인다.
				else
					SetAttachMoveEffectManager(unique_id,mSrcPos,mTarPos,_MEM_MS_ATTACH_SOURCE,pMagic->mCreateType);//움직이는 거에 붙인다.
			}
		}
	}
	stTarAttach=FALSE; //디폴트는 소스라서.
	return unique_id;
}
DWORD CAllEffectInfo::StartMagicEffectType(float mat[4][4], DWORD magic_id,void *e_Info,DWORD state)
{
	DWORD unique_id = StartMagicEffect(mat,magic_id,e_Info,state);
	CMagicList *magic = GetMagicListFromID(unique_id);
	if( magic )
	{
		Vector3f temp;
		CProcessMagic *pMagic = (CProcessMagic *)magic->mMagicPtr;
		if( pMagic )
		{
			CCharacter *ch;
			if( stTarAttach )
				ch = mTar;
			else
				ch = mSrc;
			if( EFUTIL_GetPosType(temp, ch, pMagic->mCreateType))	//타잎별 포지션이 있으면 변경한다.
			{
				Vector3fCopy(temp,magic->mMat[3]);
			}
			if( pMagic->mFlag & _PM_FLAG_ATTACH )
			{
				if( stTarAttach )
					SetAttachMoveEffectManager(unique_id,mSrcPos,mTarPos,_MEM_MS_ATTACH_TARGET,pMagic->mCreateType);//움직이는 거에 붙인다.
				else
					SetAttachMoveEffectManager(unique_id,mSrcPos,mTarPos,_MEM_MS_ATTACH_SOURCE,pMagic->mCreateType);//움직이는 거에 붙인다.
			}
		}
	}
	stTarAttach=FALSE; //디폴트는 소스라서.
	return unique_id;
}

//어태치 속성이 있는 경우 붙인다.
BOOL CAllEffectInfo::SetAttachMagicTypeAttr(DWORD unique_id)
{
	BOOL ret= FALSE;
	CMagicList *magic = GetMagicListFromID(unique_id);
	if( magic )
	{
		CProcessMagic *pMagic = (CProcessMagic *)magic->mMagicPtr;
		if( pMagic )
		{
			if( pMagic->mFlag & _PM_FLAG_ATTACH )
			{
				ret = TRUE;
				SetAttachMoveEffectManager(unique_id,mSrcPos,mTarPos,_MEM_MS_ATTACH_SOURCE,pMagic->mCreateType);//움직이는 거에 붙인다.
			}
		}
	}
	return ret;
}


void CAllEffectInfo::ForcePrepare()		//시전.
{
	float dist = GetDist(mSrcPos,mTarPos);
	DWORD unique_id;

	DWORD what_a = (mEInfo->dwID&0x000f0000)>>16;	//어떤 포스 대분류인지....
	DWORD what_b = (mEInfo->dwID&0x0000ff00)>>8;	//어떤 포스인지....
	DWORD what_c = (mEInfo->dwID&0x000000ff)-7;
	if( what_c == 6 )
		what_c = 12;
	else
	if( what_c >= 3 )
		what_c = 6;
	else
		what_c = 0;
	DWORD snd_id = _ROOT_FORCE_SOUND + what_a*4608 + what_b*18 + what_c;
	::Play3DSound(snd_id,mSrcPos);
	
	float yangle;
	DWORD ef_id = mEInfo->dwID|0x4FF00000;

	CSFItem * pSFItem = mSrc->GetSFItem();
	if( pSFItem )
	{
		if( pSFItem->IsAttackable() )
		{
			if( mSrc->IsEqualCharType( CTI_AVATAR ) || mSrc->IsEqualCharType( CTI_PLAYER ) )
				//((CPlayer *)mSrc)->m_cSwordTrace.SetTraceState(TRUE);
				((CPlayer *)mSrc)->SetTraceOfWeapon(TRUE);
			if( mSrc->IsEqualCharType( CTI_AVATAR ) )
				EFUTIL_SetBattleMusicState();	//아바타라면 배틀 사운드를 위한.
		}
	}

	if( dist == 0 || mTar == NULL )//대상이 자기자신인경우....
	{
		yangle = mSrc->GetRotY();
		unique_id = StartMagicEffectType(mSrcPos,mSrc->GetRotY(),ef_id);
		Vector3fCopy(mSrcPos,mTarPos);
	}
	else
	{
		yangle = mSrc->GetRotY();
		yangle = GetYAngle( mSrcPos,mTarPos );
		unique_id = StartMagicEffectType(mSrcPos,180+yangle,ef_id);
	}

	ef_id=0x0F400000|(ef_id&0xFF0FFFFF);
	if( IsExistFromMagicSpt(ef_id) )	//존재하면 붙어다니는 바닥데미지
	{
		DWORD unique_id = StartMagicEffectType(mSrcPos,180+yangle,ef_id);
//		SetAttachMoveEffectManager(unique_id,mSrcPos,mTarPos,_MEM_MS_ATTACH_SOURCE,_POS_TYPE_FLOOR);//움직이는 거에 붙인다.
	}
}

void CAllEffectInfo::ExtractEffectInfo( EffectInfo *e_info )
{
	if( !e_info )
		return;
	memset(this,0,sizeof(CAllEffectInfo));

	mSrc = _GetCharMgr()->GetCharacter( e_info->dwMyself[0],e_info->dwMyself[1]);
	mTar = _GetCharMgr()->GetCharacter( e_info->dwTarget[0],e_info->dwTarget[1]);
	
	if( !mSrc )	//주체가 없으니까 가라~  이런경우가 생긴적이 있었던거 같던데...
		return;
//----디버그를 위한 코드..	
	EFUtil_SetLastestEfCode(e_info);
//-------------------------------- 구조체에 필요한거 넣어준다..
	mEInfo = e_info;
	mSrc->GetPosition(mSrcPos);		//위치와 바운딩 박스얻어내자.
	mSrc->GetBoundBox(mSrcBBox);

	mSrcScale = mSrc->GetScale();
	mTarScale = 1;

	mSrcMat = EFUtil_GetDefenceMaterial(mSrc);		//캐릭터나 몬스터의 경우 머터리얼 종류...
	if( e_info->dwMyself[0] == CTI_MONSTER )	//몬스터 
	{
		mSrcID = ((CMonster*)mSrc)->GetMeshID();
	}
	else
	if( e_info->dwMyself[0] == CTI_PLAYER || mEInfo->dwMyself[0] == CTI_AVATAR  )	//플레이어. 
	{
		mSrcID = ((CPlayer*)mSrc)->GetRace();			//어떤 캐릭터인가..
		mSrcItem = ((CPlayer*)mSrc)->GetEquipItem( CEPT_WEAPON );	// 들고 있는 무기 
	}
	else
	{
		mSrcID = 0;		//다른것들..
	}

	if(mTar)	//타겟이 없을수도 있다. 주체만 있는경우....
	{
		mTarScale = mTar->GetScale();
		mTar->GetPosition(mTarPos);
		mTar->GetBoundBox(mTarBBox);
		mTarMat = EFUtil_GetDefenceMaterial(mTar);		//캐릭터나 몬스터의 경우 머터리얼 종류...
		if( e_info->dwTarget[0] == CTI_MONSTER )	//몬스터 
		{
			mTarID = ((CMonster*)mTar)->GetMeshID();
		}
		else
		if( e_info->dwTarget[0] == CTI_AVATAR || e_info->dwTarget[0] == CTI_PLAYER )
		{
			mTarID = ((CPlayer*)mTar)->GetRace();			//어떤 캐릭터인가..
			mTarItem = ((CPlayer*)mTar)->GetEquipItem( CEPT_WEAPON );	// 들고 있는 무기 
		}
		else
		{
			mTarID = 0;
		}
	}
	else
	{
		Vector3fCopy(mSrcPos,mTarPos);	//타겟이 없는 경우 소스와 타겟을 공유한다.
	}


	// 플래이 
//	SetPlayWaveState( FALSE );

//-------------------------------------------------------------------------------------------

	int code1,code2,code3;

	switch(e_info->dwType)
	{
		case ETI_MONSTER:
			code1 = ( e_info->dwID & 0x000000F0 ) >> 4;
			if( code1 == MET_DAMAGE_NORMAL )
				MonsterDamage();
			else
			if( code1 == MET_CREATE_DESTROY )
				MonsterDeath();
			else
			if( code1 == MET_GESTURE_CRAZY )
				MonsterCrazy();
			else
			if( code1 == MET_CREATE_CREATE )
				MonsterCreate();
			else
			if( code1 == MET_DAMAGE_CRITICAL )
				MonsterCritical();
			else
			if( code1 == MET_DAMAGE_MISS )
				MonsterMiss();
			else
			if( code1 == MET_ATTACK_NORMAL )
				MonsterAttack();
			else
			if( code1 == MET_ATTACK_FORCE )
				MonsterForce();
			break;
		case ETI_PLAYER:
			code1 = ( e_info->dwID & 0x00000FF0 ) >> 4;
			if( code1 == PET_DAMAGE_NORMAL1 )
				PlayerDamage1();
			else
			if( code1 == PET_DAMAGE_NORMAL2 )
				PlayerDamage1();
			else
			if( code1 == PET_GESTURE_LEVELUP )
				PlayerLevelUp();
			else
			if( code1 == PET_CREATE_CREATE )
				PlayerCreate();
			else
			if( code1 == PET_DAMAGE_CRITICAL )
				PlayerCritical();
			else
			if( code1 == PET_DAMAGE_MISS )
				Miss();
			else
			if( code1 == PET_ATTACK_ERROR )
				PlayerFailure();
			else
			if( code1 == PET_MOVEMAP_ENTER )
				PlayerMoveMapStart();
			else
			if( code1 == PET_MOVEMAP_EXIT )
				PlayerMoveMapEnd();

			break;
		case ETI_SKILL:
			code1 = ( e_info->dwID & 0x0000000F );
			if( code1 >= 6)
				SkillDamage();
			else
			if( code1 >= 3)
				SkillInvoke();
			else
				SkillPrepare();
			break;
		case ETI_FORCE:
			code1 = ( e_info->dwID & 0x000000FF ) >> 0;
			if( code1 >= 0xe )
				ForceDamage();	//데미지.
			else
			if( code1 >= 7 )
				ForceInvoke();	//발동.
			else
				ForcePrepare();	//시전.
			break;
		case ETI_ITEM:
			code1 = ( e_info->dwID & 0x000000F0 ) >> 4;
			if( code1 == IET_DEFAULT )
				ItemDefault();
			else
			if( code1 == IET_ATTACK_NORMAL )
				ItemAttack();
			else
			if( code1 == IET_ATTRIBUTE )
				ItemAttr();
			else
			if( code1 == IET_USE )
				ItemUse();
			else
			if( code1 == IET_ATTACK_SKILL )
				ItemSkill();
			else
			if( code1 == IET_ATTACK_FORCE )
				ItemForce();
			break;
		case ETI_UNIT:
			code1 = ( e_info->dwID & 0x0000FF00 ) >> 8;
			code2 = ( e_info->dwID & 0x000000F0 ) >> 4;
			code3 = ( e_info->dwID & 0x0000000F );
//			if( code3  == UET_DAMAGE )
//				UnitDamage(code1,code3);
//			else
//			if( code3  == UET_DAMAGE_MISS )
//				Miss();
//			else
//			if( code3  == _UNIT_ATTACK1 )
//				UnitAttack(code1,code3);
//			else
				UnitEtc();
			break;
		case ETI_ANIMUS:
			Animus();
			break;
	}
}


void CAllEffectInfo::Animus()
{
	float mat[4][4];
	float smat[4][4];

	DWORD ef_id = (mEInfo->dwID&0x00fffff0)|0x7F000000;
	switch( ((mEInfo->dwID&0xf0)>>4) )
	{
	default :
/*
		case AET_CREATE_DESTROY:			
		case AET_CREATE_CREATE:
		case AET_ATTACK_NORMAL:
		case AET_ATTACK_FORCE:
		case AET_ATTACK_ERROR:
		case AET_GESTURE_SUMMON:
		case AET_GESTURE_REPATRIATE:
		case AET_GESTURE_LEVELUP:
		case AET_GESTURE_CRAZY:
*/
			StartMagicEffectType(mSrcPos,mSrc->GetRotY(),ef_id);
			::Play3DSound(ef_id,mSrcPos);
			break;
		case AET_ATTACK_NORMAL:
		case AET_ATTACK_FORCE:
			if( mTar )
			{
				SetTargetAttachMagicEffectType();	//타겟에 붙이기위해서
				StartMagicEffectType(mTarPos,mTar->GetRotY(),ef_id);
				::Play3DSound(ef_id,mTarPos);
			}
			break;
		case AET_GESTURE_REPATRIATE:
			MatrixRotate(mat,0,mSrc->GetRotY(),0); //단위는 360도
			MatrixScale(smat,mSrc->GetScale(),mSrc->GetScale(),mSrc->GetScale());
			MatrixMultiply(mat,smat,mat);
			mat[3][0] = mSrcPos[0];
			mat[3][1] = mSrcPos[1];
			mat[3][2] = mSrcPos[2];
			StartMagicEffect(mat,ef_id);
			::Play3DSound(ef_id,mSrcPos);
			break;
		case AET_DAMAGE_MISS:
			Miss();
			break;
		case AET_DAMAGE_NORMAL:
			CMonsterSpecialEffectDamageAction(mSrc);
			OutputDamage(mSrc);
			break;
	}
}

void CAllEffectInfo::UnitEtc()
{
	DWORD ef_id = (mEInfo->dwID&0x00FFFFFF)|0x6F000000;
	switch( (mEInfo->dwID&0xf) )
	{
	default :
/*
#define UET_CREATE					0x00
#define UET_DESTROY					0x01
#define UET_DAMAGE					0x02
#define UET_DAMAGE_MISS				0x03
#define UET_CRITICAL				0x04
#define UET_ATTACK					0x05
#define UET_REPAIR					0x06
#define UET_GESTURE_MOUNT			0x07
#define UET_GESTURE_DISMOUNT		0x08
*/
			StartMagicEffectType(mSrcPos,mSrc->GetRotY(),ef_id);
			::Play3DSound(ef_id,mSrcPos);
			break;
		case UET_ATTACK:
			EFUtil_PushEffectValue(mEInfo,TRUE);
			if( mTar )
			{
				SetTargetAttachMagicEffectType();	//타겟에 붙이기위해서
				StartMagicEffectType(mTarPos,mTar->GetRotY(),ef_id);
				::Play3DSound(ef_id,mTarPos);
			}
			break;
		case UET_DAMAGE_MISS:
			Miss();
			break;
		case UET_DAMAGE:
			CMonsterSpecialEffectDamageAction(mSrc);
			OutputDamage(mSrc);
			break;
	}
}

void CAllEffectInfo::UnitDamage(DWORD code1,DWORD code3)
{
	if( code1 != 6 )	//현재는 6번만.
		return;

	Vector3f pos;
	DWORD effect_id=0x60102,yangle;

	EFUtil_GetDummyPos( pos, mSrc,_POS_TYPE_DAMAGE2);

	if( mTar == NULL )
		yangle = mSrc->GetRotY();
	else
		yangle = GetYAngle( mTarPos,mSrcPos );

	StartMagicEffect(pos,yangle,effect_id);	//미사일 공격
}
/*
void CAllEffectInfo::UnitAttack(DWORD code1,DWORD code3)
{
	if( mTar == NULL )	//유닛 공격이 타겟이 없을수는없다.
	{
		Warning("타겟 널.","");
		return;
	}
	mEInfo->dwValue = TRUE;
	_GetReturnEffectInfoList()->push_back( mEInfo );

	if( code1 != 6 )	//현재는 6번만.
		return;

	Vector3f ch_src_pos,ch_tar_pos;
	Vector3f normal,axis={0,0,-1};
	float mat[4][4];
	DWORD effect_id=(code1<<8)|0x60000;
	DWORD add_effect_id;

	Vector3fCopy(mTarPos,ch_tar_pos);
	Vector3fCopy(mSrcPos,ch_src_pos);
	ch_src_pos[1]+=12;
	ch_tar_pos[1]+=12;

	Vector3fSub(ch_tar_pos,ch_src_pos,normal);
	Normalize(normal);
	GetMatrixFromVector(mat,normal,axis);
	float dist = GetDist(ch_src_pos,ch_tar_pos);

	mat[3][0] = ch_src_pos[0];
	mat[3][1] = ch_src_pos[1];
	mat[3][2] = ch_src_pos[2];

//	DWORD unique_id = StartMagicEffect(mat, 0xEFFF006C,0,_EF_STATE_ALWAYS_LIVE);

	if( code3 == 0 )
	{
		effect_id = 0x60106;
		DWORD unique_id = StartMagicEffect(mat, effect_id,0,_EF_STATE_ALWAYS_LIVE);	//미사일 공격
		_MOVE_EF_FUNC m_ef_func;
		SetDefaultMoveFunc(&m_ef_func);
		m_ef_func.unique_id = unique_id;
		EFUtil_GetDummyPos( m_ef_func.pos, mSrc,_DUMMY_SPECIAL1);
		EFUtil_GetDummyPos( m_ef_func.tar, mTar,_POS_TYPE_DAMAGE2);
			
		m_ef_func.speed = 0.2f;
		m_ef_func.move_style = _MEM_MS_CURVE;
		_MAGIC_TRACK *track = GetMagicTrack(unique_id);
		if( track )
			SetCurveMagicTrackRandom(track);	//현재 매직에서 쓰는 커브를 랜덤으로 사용한다.
		
		m_ef_func.attach_time=0;	//포스는 ...
		StartMoveEffectManager(&m_ef_func);
		::Play3DSound(33208,mSrcPos);
	}
	else
	{	//유닛 쌍기관총.
		mEInfo->dwValue = TRUE;
		_GetReturnEffectInfoList()->push_back( mEInfo );
		add_effect_id = 1;
//		mEInfo->dwValue = TRUE;
//		_GetReturnEffectInfoList()->push_back( mEInfo );
		Vector3f pos;
		EFUtil_GetDummyPos( pos, mSrc,_DUMMY_FIRE1);
		StartMagicEffect(pos,mSrc->GetRotY()+180,0x60108);//불꽂
		EFUtil_GetDummyPos( pos, mSrc,_DUMMY_FIRE3);
		StartMagicEffect(pos,mSrc->GetRotY()+180,0x60108);//불꽂
		EFUtil_GetDummyPos( pos, mSrc,_DUMMY_FIRE2);
		StartMagicEffect(pos,0,0x60109);//탄피
		EFUtil_GetDummyPos( pos, mSrc,_DUMMY_FIRE4);
		StartMagicEffect(pos,0,0x60109);//탄피
		::Play3DSound(33209,mSrcPos);
		SkipCheckSameSound();

		effect_id = 0x6010A;	//기관총. 발동 데미지.
		EFUtil_GetDummyPos( pos, mTar,_POS_TYPE_DAMAGE2);
		StartMagicEffect(pos,0,effect_id);//
		::Play3DSound(33212,pos);
		SkipCheckSameSound();

		effect_id = 0x6010B;	//기관총. 바닥 데미지.
		if( IsExistFromMagicSpt(effect_id) )
		{
			StartMagicEffect(mTarPos,0,effect_id);//
		}		
	}
}
*/
void CAllEffectInfo::SetDefaultMoveFunc(_MOVE_EF_FUNC *m_ef_func)
{
	m_ef_func->e_info = mEInfo;
	m_ef_func->src_ID = mSrcID;
	m_ef_func->tar_ID = mTarID;
	m_ef_func->src_mat = mSrcMat;
	m_ef_func->tar_mat = mTarMat;
	m_ef_func->bullet_ID_for_damage=0;
	m_ef_func->flag=0;
	m_ef_func->attach_type_id = _POS_TYPE_DAMAGE2;
}


void CheckKeyStringBuffer()	//연속키 체크하기..
{
	//------- 특정 연속키누를때 사용하는거.
	int key = GetCharFromKey();
	stKeyStringTimer += R3GetLoopTime();
	if( key )
	{
		if( stKeyStringTimer > _MAX_KEY_TIME_INTERVAL )	//키입력 사이가 오래 걸린경우 첨부터 다시.
			stKeyStringCnt=0;

		stKeyString[stKeyStringCnt] = key;
		stKeyStringCnt++;
		if( stKeyStringCnt >= _MAX_KEY_STRING_CNT )
		{
			stKeyString[stKeyStringCnt] = NULL;
			if( !strcmp(stCmpSting,stKeyString) )	//같다면.
			{
				st_MainProfile.CleanR3Profiler();
				st_bCmp = !st_bCmp;
				stKeyStringCnt=0;
			}
			else	//같지 않다면 한칸씩 민다.
			{
				memcpy(stKeyString,&stKeyString[1],_MAX_KEY_STRING_CNT);
				stKeyStringCnt--;
			}
		}
		stKeyStringTimer=0;
	}
}

void DrawCheatDisplay()
{
	if( !st_bCmp )	//치트키성공시
		return;

	R3BeginScene();

	DWORD *id_list = GetMagicIDList();	
	char buf[256];

	::DrawR3Hangul( 200,240, "-magic stack-",0xffffcfaf,R3_HAN_OUTLINE);
	for(DWORD i=0; i<_MAX_MAGIC_ID_STACK; i++)
	{
		sprintf(buf,"0x%x",id_list[i]);
		::DrawR3Hangul( 200,12+240+i*12, buf,0xffffcf8f,R3_HAN_OUTLINE);
	}


	CEffectList* ef_list = GetEffectList();
	CMagicList* magic_list = GetMagicList();
	i=0;
	DWORD j;

	::DrawR3Hangul( 300,240+i*12, "-magic code-",0xffffcfaf,R3_HAN_OUTLINE);
	i++;
	//현재의 매직및 이펙트 상황을 알아볼수있는 코드닷.
	for( j=0; j<GetMagicListCnt(); j++,i++)
	{
		sprintf(buf,"0x%x",magic_list[j].mMagicID);
		::DrawR3Hangul( 300,240+i*12, buf,0xffffbfaf,R3_HAN_OUTLINE);
	}
	i=0;
	::DrawR3Hangul( 400,240+i*12, "-effect code-",0xffffcfaf,R3_HAN_OUTLINE);
	i++;
	for( j=0; j<GetEffectListCnt(); j++,i++)
	{
		sprintf(buf,"0x%x",ef_list[j].mEffectID);
		::DrawR3Hangul( 400,240+i*12, buf,0xffffcfaf,R3_HAN_OUTLINE);
	}

	sprintf(buf,"총magic수=%d,총Entity수=%d,로딩된Entity수=%d",GetMagicSptNum(),GetEffectEntityNum(),GetLoadedEffectEntityNum());
	::DrawR3Hangul( 200,62, buf,0xffffffff,R3_HAN_OUTLINE);

	sprintf(buf,"사용 캐쉬 %d / 총 캐쉬 %d,",GetEffectCacheNum(),GetEffectCacheTotalNum());
	::DrawR3Hangul( 200,64+12, buf,0xffffffff,R3_HAN_OUTLINE);

	DWORD al_en_tex_mem = GetAllEffectEntityTextureSize();	//현재 이펙트 엔티티에 사용되는 모든 텍스쳐수.
	DWORD al_en_mem = GetAllMemorySizeEffectEntity();	//현재 대략 사용되는 effectentitylist 메모리량	텍스쳐 제외
	DWORD al_ma_mem = GetAllMemorySizeMagic();	//현재 대략 사용되는 매직 메모리 수. 
	DWORD al_ca_mem = GetEffectCacheSize();			//현재 사용되고잇는 이펙트 캐쉬 사이즈를 알아낸다.

	float total_mem = (al_en_tex_mem + al_en_mem + al_ma_mem + al_ca_mem)/1000000.0f;
	sprintf(buf,"이펙트: 전체MEM = %.2fM,tex = %.2fM,entity = %.2fM,magic = %.2fM,cache = %.2fM"
		,total_mem,(float)al_en_tex_mem/1000000.0f,(float)al_en_mem/1000000.0f
		,(float)al_ma_mem/1000000.0f,(float)al_ca_mem/1000000.0f);
	::DrawR3Hangul( 200,64+24, buf,0xffffff00,R3_HAN_OUTLINE);

#define GetMega(a) ((float)(a)/1000000.0f)
	sprintf(buf,"캐릭: 전체MEM=%.2fM,텍스쳐:수=%d,사이즈=%.2fM 버텍스버퍼:수=%d,사이즈=%.2fM"
		,GetMega(g_ChMemoryInfo.m_TextureSize+g_ChMemoryInfo.m_VertexSize
		+g_ChMemoryInfo.m_ObjectSize+g_ChMemoryInfo.m_AniSize)
		,g_ChMemoryInfo.m_TextureNum,GetMega(g_ChMemoryInfo.m_TextureSize)
		,g_ChMemoryInfo.m_VetexNum,GetMega(g_ChMemoryInfo.m_VertexSize));
	::DrawR3Hangul( 200,64+26+13, buf,0xff00ffff,R3_HAN_OUTLINE);

	if( !g_pLevel )
		return;
	al_en_tex_mem = g_pLevel->mLightTexMemSize+g_pLevel->mLightTexMemSize/4
		+g_pLevel->mBsp->mVertexBufferSize
		+g_pLevel->mMapTexMemSize
		+g_pLevel->mSkyTexMemSize,g_pLevel->mEntityTexMemSize
		+g_pLevel->mBsp->mTotalAllocSize+g_pLevel->mBsp->mTotalWaveSize;

	sprintf(buf,"맵: 전체MEM=%.2fM 텍스쳐=%.2fM,버텍스버퍼: 수=%d,사이즈=%.2fM,사운드=%.2fM"
		,GetMega(al_en_tex_mem+GetTotalVertexBufferSize()),GetMega(al_en_tex_mem)
		,GetTotalVertexBufferCnt(),GetMega(GetTotalVertexBufferSize())
		,GetMega(g_pLevel->mBsp->mTotalWaveSize));
	::DrawR3Hangul( 200,64+26+26, buf,0xffff00ff,R3_HAN_OUTLINE);

	//프로파일 디스플레이.
	for(i=0; i<st_MainProfile.GetR3ProfilerTotalNum(); i++)
	{
		sprintf(buf,"%31s:%6.1f,%5.2f%%"
			,st_MainProfile.GetR3ProfilerIDName(i)
			,st_MainProfile.GetR3ProfilerIDLoopTime(i)
			,st_MainProfile.GetR3ProfilerIDPercent(i));
		::DrawR3Hangul( 550,148+16*i,buf,0xffffffff,R3_HAN_OUTLINE);
	}
	R3EndScene();
	//CleanR3Profiler();
}

