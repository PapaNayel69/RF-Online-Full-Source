////////////////////////////////////////////////////////////////////////////////
//
// CCharacter Relation Infomation Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CHARACTER_INFO1_H__
#define __CHARACTER_INFO1_H__

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// CHARACTER_CONDITION_INFO & APPLIED_SF 구조체 안에서만 쓰인다. Define.h에 정의된 것과는 상관없음
//enum { CCI_MAX_ATTACK_TYPE = 7 };
//enum { CCI_AT_SKILL, CCI_AT_FORCE, CCI_AT_MELEE, CCI_AT_BOW, CCI_AT_FIREARM, CCI_AT_LAUNCHER, CCI_AT_THROW };	// <= MAX_ATTACK_TYPE
//enum { CCI_MAX_SUPPORT_SF_TYPE = 2 };
//enum { CCI_SFAT_SCP, CCI_SFAT_SCL };					// <= MAX_SUPPORT_SF_TYPE

// MAX_ATTACK_TYPE
enum { CCI_AT_SKILL, CCI_AT_FORCE, CCI_AT_NEAR, CCI_AT_FAR, CCI_AT_LAUNCHER, CCI_MAX_ATTACK_TYPE };
// MAX_SUPPORT_SF_TYPE
enum { CCI_SFAT_SCP, CCI_SFAT_SCL, CCI_MAX_SUPPORT_SF_TYPE };

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CNormalItem;
class CSFItem;

// 캐릭터에 영향을 미치는 요소들
struct CHARACTER_CONDITION_INFO
{
	CCharacter*	m_pCharacter;							// 캐릭터의 포인터

	//--------------------------------------------------------------------------
	// 캐릭터에 가감되는 능력치들
	int			m_nAttackDistance[CCI_MAX_ATTACK_TYPE];	// 공격 거리 ( + )
	int			m_nAttackSpeed[CCI_MAX_ATTACK_TYPE];	// 공격 속도 ( + )
	float		m_fStrikingPower[CCI_MAX_ATTACK_TYPE];	// 공격력 ( * )
	float		m_fHitProb[CCI_MAX_ATTACK_TYPE];		// 명중률 ( * )
	float		m_fAvoidProb[CCI_MAX_ATTACK_TYPE];		// 회피율 ( * )
	float		m_fConsumeFP;							// 소모되는 FP량 ( * )
	float		m_fMoveSpeed;							// 이동 속도 ( + )
	float		m_fVisualField;							// 시야 거리 ( + )
	int			m_nResist[MAX_ITEM_ATTRIBUTE];			// 내성 ( + )
	float		m_fDefence_Exclude_Shield;				// 방어력. 방패를 제외한 모든 방어력 상승 ( * )

	//--------------------------------------------------------------------------

	//float		m_fMaxHPnFP;							// 최대 HP & FP ( * )
	float		m_fDefencePower;						// 방어력 ( * )
	BYTE		m_bySkill_Level;						// 스킬 레벨 ( + )
	BOOL		m_bEnable_MarkWeekPoint;				// 적 플레이어의 약한 부위를 표시 ( default = FALSE )

	//--------------------------------------------------------------------------

	BOOL		m_bEnable_Stealth;						// 스텔스 가능 ( default = FALSE )
	BOOL		m_bEnable_Detect;						// 디텍트 가능 ( default = FALSE )
	BOOL		m_bEnable_CounterAttack;				// 반격 가능 ( default = FALSE )
	BOOL		m_bDisable_Run;							// 뛰기 불가능 ( default = FALSE )
	BOOL		m_bDisable_Move;						// 움직임 불가능 ( default = FALSE )
	BOOL		m_bEnable_Stun;							// 스턴 상태인가? ( default = FALSE )
	BOOL		m_bDisable_Recover;						// HP/SP/FP 회복 불가 ( default = FALSE )
	BOOL		m_bDisable_UseSkillForce[2][4];			// 스킬/포스를 사용 불가능한가? ( [0] = 스킬, [1] = 포스 / [0] = 공격, [1] = 일시 보조, [2] = 지속 보조 득, [3] = 지속 보조 실 ( default = FALSE )
	DWORD		m_dwStunStartTime;						// 스턴을 시작한 시간

	//--------------------------------------------------------------------------

	int			m_nResist_Upgrade[MAX_ITEM_ATTRIBUTE];	// 업그레이드된 아이템의 내성
	int			m_nResist_Accessory[MAX_ITEM_ATTRIBUTE];// 장신구의 내성

	//--------------------------------------------------------------------------
	// 각 능력치의 총합
	DWORD		m_dwTotalStrikingPower[2];				// 캐릭터의 일반 공격력 ( [0] = Min, [1] = Max )
	DWORD		m_dwTotalForcePower[2];					// 캐릭터의 포스 공격력 ( [0] = Min, [1] = Max )
	DWORD		m_dwTotalDefencePower;					// 캐릭터의 방어력
	int			m_nTotalResist[MAX_ITEM_ATTRIBUTE];		// 캐릭터의 총 내성
	DWORD		m_dwTotalAttackDelay;					// 아이템 착용으로 인해서 증가되는 어택 딜레이
	float		m_fTotalHitProb;						// 현재 장착한 무기의 명중률 증가치
	BYTE		m_byWeakPartID;							// 캐릭터의 약한 부위

	//--------------------------------------------------------------------------

	CHARACTER_CONDITION_INFO();
	~CHARACTER_CONDITION_INFO();

	void	Init( void );

	void	ApplyItemEffect( CNormalItem * pi_pNormalItem, BOOL pi_bIsAdd );	// 아이템의 의해 적용되는 능력
	void	ApplySFEffect( CSFItem * pi_pSFItem, BOOL pi_bIsAdd );				// 스킬/포스에 의해 적용되는 능력
	void	ApplyPotionEffect( POTION_DATA * pi_pPotionData, BOOL pi_bIsAdd );	// 포션에 의해 적용되는 능력

protected :

	void	ApplyItemEffect_Special( CNormalItem * pi_pNormalItem, BOOL pi_bIsAdd );
	void	ApplyItemEffect_Upgrade( CNormalItem * pi_pNormalItem, BOOL pi_bIsAdd );
	void	ApplyItemEffect_Normal( CNormalItem * pi_pNormalItem, BOOL pi_bIsAdd );

	void	ApplySFEffect_Once( BYTE pi_byEffectType, float pi_fEffectValue, BOOL pi_bIsAdd );
	void	ApplySFEffect_Continuance( BYTE pi_byEffectUnit, BYTE pi_byEffectType, float pi_fEffectValue, BOOL pi_bIsAdd );
	void	ApplySFEffect_Continuance_Rate( BYTE pi_byEffectType, float pi_fEffectValue, BOOL pi_bIsAdd );
	void	ApplySFEffect_Continuance_Add( BYTE pi_byEffectType, float pi_fEffectValue, BOOL pi_bIsAdd );
	void	ApplySFEffect_Continuance_Bool( BYTE pi_byEffectType, float pi_fEffectValue, BOOL pi_bIsAdd );

	void	CombineAllEffect( void );									// 모든 이펙트의 효과를 합친다.

	void	SetWeakPart( void );										// 캐릭터 약한 부위를 세팅한다.

public :

	void	SetStun( BOOL pi_bIsStun );
	void	ProcessStun( void );

#ifdef _DEVELOPER_
public :
	void	DisplayConditionInfo( void );
#endif
};

typedef CHARACTER_CONDITION_INFO CHAR_COND_INFO;

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

struct APPLIED_SF
{
	CSFItem			m_listSF[2][MAX_APPLIED_SF];		// 스킬/포스의 정보 ( 0 = 득, 1 =실 )
	DWORD			m_dwApplyTime[2][MAX_APPLIED_SF];	// 스킬/포스가 적용된 시간
	DWORD			m_dwRemainTime[2][MAX_APPLIED_SF];	// 스킬/포스의 남은 시간
	BYTE			m_bySFNum[2];						// 적용된 스킬/포스의 갯수
	BYTE			m_byLatestSF[2][2];					// 가장 최근에 적용된 스킬/포스의 Order No ( 0 = 스킬, 1 = 포스 / 0 = 득, 1 =실 )

	BOOL			m_bSetUI;							// UI 를 세팅해 줄 것인가?
	DWORD			m_dwPrevFrameMoveTime;

	CHAR_COND_INFO* m_pCharCondInfo;					// 캐릭터의 상태 정보

	DWORD			m_dwEffectKey[2];					// 사용중인 이펙트 아이디 key. JUI
	DWORD			m_dwUniqueID[2][2];					// 중간에 바뀔경우 유닉크아이디로 지워주어야한다. JUI

	//--------------------------------------------------------------------------

	APPLIED_SF();
	~APPLIED_SF();
	
	DWORD		AddSFItem( CSFItem * pi_pSFItem );
	DWORD		DeleteSFItem( CSFItem * pi_pSFItem );
	DWORD		DeleteSFItem( BYTE pi_byProfitType, BYTE pi_byOrderNo );
	DWORD		DeleteLastestAppliedSFItem( void );
	CSFItem *	GetSFItem( BYTE pi_byProfitType, BYTE pi_byOrderNo );
	CSFItem *	GetSFItemBySpecialEffect( BYTE pi_bySpecialEffect );

	void		FrameMove( void );

	CSFItem *	UpdateRemainTime( BYTE pi_bySFType, DWORD pi_dwDTIndex, DWORD pi_dwTime );
	DWORD		GetRemainTime( BYTE pi_byProfitType, BYTE pi_byOrderNo, float pi_fRate );
	DWORD		GetOriginalRemainTime( CSFItem * pi_pSFItem );

	void		Clear( void );
	void		ClearEffect( void );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __CHARACTER_INFO1_H__