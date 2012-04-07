////////////////////////////////////////////////////////////////////////////////
//
// CCharacter Relation Infomation Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"

#include "../../UserInterface/GameWindow/SkillForce/GUISkillForceQueue.h"
#include "../../UserInterface/GUIController/GUIMessagebox.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CHARACTER_CONDITION_INFO::CHARACTER_CONDITION_INFO()
{
	m_bEnable_Stealth	= FALSE;
	m_bEnable_Detect	= FALSE;
	m_bEnable_Stun		= FALSE;

	Init();
}

CHARACTER_CONDITION_INFO::~CHARACTER_CONDITION_INFO()
{
}

void
CHARACTER_CONDITION_INFO::Init( void )
{
	int i, j;

	m_pCharacter				= NULL;

	ZeroMemory( m_nAttackDistance, sizeof( int ) * CCI_MAX_ATTACK_TYPE );
	ZeroMemory( m_nAttackSpeed, sizeof( int ) * CCI_MAX_ATTACK_TYPE );
	for( i = 0; i < CCI_MAX_ATTACK_TYPE; ++i )
	{
		m_fStrikingPower[i]		= 1.0f;
		m_fHitProb[i]			= 1.0f;
		m_fAvoidProb[i]			= 1.0f;
	}

	m_fConsumeFP				= 1.0f;
	m_fMoveSpeed				= 0.0f;
	m_fVisualField				= 1.0f;
	for( i = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
		m_nResist[i]			= 0;
	m_fDefence_Exclude_Shield	= 1.0f;

	m_fDefencePower				= 1.0f;
	m_bySkill_Level				= 0;
	m_bEnable_MarkWeekPoint		= FALSE;

	m_bEnable_CounterAttack		= FALSE;
	m_bDisable_Run				= FALSE;
	m_bDisable_Move				= FALSE;
	m_bEnable_Stun				= FALSE;
	m_bDisable_Recover			= FALSE;
	m_dwStunStartTime			= 0;
	for( i = 0; i < 2; ++i )
		for( j = 0; j < 4; ++j )
			m_bDisable_UseSkillForce[i][j] = FALSE;

	for( i = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
	{
		m_nResist_Upgrade[i]	= 0;
		m_nResist_Accessory[i]	= 0;
	}

	m_dwTotalStrikingPower[0]	= 0;
	m_dwTotalStrikingPower[1]	= 0;
	m_dwTotalForcePower[0]		= 0;
	m_dwTotalForcePower[1]		= 0;
	m_dwTotalDefencePower		= 0;
	for( i = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
		m_nTotalResist[i]		= 0;
	m_dwTotalAttackDelay		= 0;
	m_fTotalHitProb				= 1.0f;
	m_byWeakPartID				= 0;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define SET_EFFECT_VALUE_ADD( pi_fVariable, pi_fValue ) \
if( pi_bIsAdd ) pi_fVariable += pi_fValue; \
else			pi_fVariable -= pi_fValue

#define SET_EFFECT_VALUE_INC( pi_fVariable ) \
if( pi_bIsAdd ) ++pi_fVariable; \
else			--pi_fVariable

//==============================================================================

void
CHARACTER_CONDITION_INFO::ApplySFEffect( CSFItem * pi_pSFItem, BOOL pi_bIsAdd )
{
	if( !pi_pSFItem )
		return;

	for( int i = 0; i < MAX_SF_EFFECT; ++i )
	{
		switch( pi_pSFItem->GetUsingType( i ) )
		{
			case SFAT_ATTACK :
				break;

			case SFAT_SUPPORT_ONCE :
				ApplySFEffect_Once( pi_pSFItem->GetEffectType( i ),
									pi_pSFItem->GetEffectValue( i ), pi_bIsAdd );
				break;

			case SFAT_SUPPORT_CONTINUANCE_LOSS :
			case SFAT_SUPPORT_CONTINUANCE_PROFIT :
				ApplySFEffect_Continuance( pi_pSFItem->GetEffectUnit( i ),
										   pi_pSFItem->GetEffectType( i ),
										   pi_pSFItem->GetEffectValue( i ), pi_bIsAdd );
				break;
		}
	}

	CombineAllEffect();
}

void
CHARACTER_CONDITION_INFO::ApplySFEffect_Once( BYTE pi_byEffectType, float pi_fEffectValue, BOOL pi_bIsAdd )
{
	switch( pi_byEffectType )
	{
		case 0 :	// #define 0 자신의 HP를 대상의 FP로 만든다
		case 1 :	// #define 1 지속 피해보조 남은 시간 늘이기
		case 2 :	// #define 2 죽은 유저를 재생한다.
		case 3 :	// #define 3 현재 HP증가
		case 4 :	// #define 4 현재 ST증가
		case 5 :	// #define 5 지속보조시간 증가
		case 6 :	// #define 6 overhealing..   MaxHP의 200%만큼만 가능하도록 한다.
		case 7 :	// #define 7 가장 최근 지속이득보조 스킬 해제
		case 8 :	// #define 8 가장 최근 지속이득보조 포스 해제
		case 9 :	// #define 9 가장 최근 지속피해보조 포스 해제
		case 10 :	// #define 10 대상에 걸려있는 지속이득보조 스킬 제거
		case 11 :	// #define 11 대상에 걸려있는 지속이득보조 포스 제거
		case 12 :	// #define 12 대상에 걸려있는 지속 피해 보조 포스 해제
		case 13 :	// #define 13 시전자 주위 다른 종족들의 지속 이득 보조 스킬 & 포스 해제
		case 14 :	// #define 14 skill에 의한 지속적인 이득 남은 시간 늘이기
			break;
	}
}

void
CHARACTER_CONDITION_INFO::ApplySFEffect_Continuance( BYTE pi_byEffectUnit, BYTE pi_byEffectType, float pi_fEffectValue, BOOL pi_bIsAdd )
{
	switch( pi_byEffectUnit )
	{
		case 0 :	// #define 0 Rate(×)
			ApplySFEffect_Continuance_Rate( pi_byEffectType, pi_fEffectValue, pi_bIsAdd );
			break;

		case 1 :	// #define 1 Value(＋)
			ApplySFEffect_Continuance_Add( pi_byEffectType, pi_fEffectValue, pi_bIsAdd );
			break;

		case 2 :	// // #define 2 Bool
			ApplySFEffect_Continuance_Bool( pi_byEffectType, pi_fEffectValue, pi_bIsAdd );
			break;
	}
}

void
CHARACTER_CONDITION_INFO::ApplySFEffect_Continuance_Rate( BYTE pi_byEffectType, float pi_fEffectValue, BOOL pi_bIsAdd )
{
	switch( pi_byEffectType )
	{
		case 0 :	// #define 0 근접 공격력 증가
			SET_EFFECT_VALUE_ADD( m_fStrikingPower[CCI_AT_NEAR], pi_fEffectValue );
			break;
		case 1 :	// #define 1 원거리 공격력 증가
			SET_EFFECT_VALUE_ADD( m_fStrikingPower[CCI_AT_FAR], pi_fEffectValue );
			break;
		case 2 :	// #define 2 런쳐 공격력 증가
			SET_EFFECT_VALUE_ADD( m_fStrikingPower[CCI_AT_LAUNCHER], pi_fEffectValue );
			break;
		case 3 :	// #define 3 스킬 공격력 증가
			SET_EFFECT_VALUE_ADD( m_fStrikingPower[CCI_AT_SKILL], pi_fEffectValue );
			break;
		case 4 :	// #define 4 포스 공격력 증가
			SET_EFFECT_VALUE_ADD( m_fStrikingPower[CCI_AT_FORCE], pi_fEffectValue );
			break;
		case 5 :	// #define 5 스턴 확률
			break;
		case 6 :	// #define 6 방어력 증가
			SET_EFFECT_VALUE_ADD( m_fDefencePower, pi_fEffectValue );
			break;
		case 7 :	// #define 7 FP소모율 감소
		case 8 :	// #define 8 지속피해보조 시간 증가
		case 9 :	// #define 9 최대 HP 증가
		case 10 :	// #define 10 최대 FP 증가
		case 11 :	// #define 11 최대 SP 증가
		case 12 :	// #define 12 데미지의 일부를 HP로 바꿈
		case 13 :	// #define 13 데미지의 일부를 FP로 바꿈
		case 14 :	// #define 14 HP 회복속도 상승
		case 15 :	// #define 15 FP 회복속도 상승
		case 16 :	// #define 16 SP 회복속도 상승
		case 17 :	// #define 17 방패방어력증가
		case 18 :	// #define 18 HP 회복제한(%)
		case 19 :	// #define 19 FP 회복제한(%)
		case 20 :	// #define 20 SP 회복제한(%)
			break;
	}
}

void
CHARACTER_CONDITION_INFO::ApplySFEffect_Continuance_Add( BYTE pi_byEffectType, float pi_fEffectValue, BOOL pi_bIsAdd )
{
	int i;

	switch( pi_byEffectType )
	{
		case 0 :	// #define 0 근접 공격 성공 확률 증가
			SET_EFFECT_VALUE_ADD( m_fHitProb[CCI_AT_NEAR], pi_fEffectValue );
			break;
		case 1 :	// #define 1 원거리 공격 성공 확률 증가
			SET_EFFECT_VALUE_ADD( m_fHitProb[CCI_AT_FAR], pi_fEffectValue );
			break;
		case 2 :	// #define 2 런쳐 공격 성공 확률 증가
			SET_EFFECT_VALUE_ADD( m_fHitProb[CCI_AT_LAUNCHER], pi_fEffectValue );
			break;
		case 3 :	// #define 3 회피율 증가
			for( i = CCI_AT_NEAR; i <= CCI_AT_FAR; ++i )
			{
				SET_EFFECT_VALUE_ADD( m_fAvoidProb[i], pi_fEffectValue );
			}
			break;
		case 4 :	// #define 4 근접 공격 사정거리 증가
			SET_EFFECT_VALUE_ADD( m_nAttackDistance[CCI_AT_NEAR], pi_fEffectValue );
			break;
		case 5 :	// #define 5 원거리 공격 사정거리 증가
			SET_EFFECT_VALUE_ADD( m_nAttackDistance[CCI_AT_FAR], pi_fEffectValue );
			break;
		case 6 :	// #define 6 런처 공격 사정거리 증가
			SET_EFFECT_VALUE_ADD( m_nAttackDistance[CCI_AT_LAUNCHER], pi_fEffectValue );
			break;
		case 7 :	// #define 7 스킬 사정거리 증가
			SET_EFFECT_VALUE_ADD( m_nAttackDistance[CCI_AT_SKILL], pi_fEffectValue );
			break;
		case 8 :	// #define 8 포스 사정거리 증가
			SET_EFFECT_VALUE_ADD( m_nAttackDistance[CCI_AT_FORCE], pi_fEffectValue );
			break;
		case 9 :	// #define 9 근접 공격 딜레이 감소
			SET_EFFECT_VALUE_ADD( m_nAttackSpeed[CCI_AT_NEAR], pi_fEffectValue );
			break;
		case 10 :	// #define 10 원거리 공격 딜레이 감소
			SET_EFFECT_VALUE_ADD( m_nAttackSpeed[CCI_AT_FAR], pi_fEffectValue );
			break;
		case 11 :	// #define 11 런처 공격 딜레이 감소
			SET_EFFECT_VALUE_ADD( m_nAttackSpeed[CCI_AT_LAUNCHER], pi_fEffectValue );
			break;
		case 12 :	// #define 12 스킬 사용 딜레이 감소
			SET_EFFECT_VALUE_ADD( m_nAttackSpeed[CCI_AT_SKILL], pi_fEffectValue );
			break;
		case 13 :	// #define 13 포스 사용 딜레이 감소
			SET_EFFECT_VALUE_ADD( m_nAttackSpeed[CCI_AT_FAR], pi_fEffectValue );
			break;
		case 14 :	// #define 14 크리티컬 구간 증가
			break;
		case 15 :	// #define 15 불 내성 증가
			SET_EFFECT_VALUE_ADD( m_nResist[IAT_FIRE], pi_fEffectValue );
			break;
		case 16 :	// #define 16 물 내성 증가
			SET_EFFECT_VALUE_ADD( m_nResist[IAT_WATER], pi_fEffectValue );
			break;
		case 17 :	// #define 17 흙 내성 증가
			SET_EFFECT_VALUE_ADD( m_nResist[IAT_EARTH], pi_fEffectValue );
			break;
		case 18 :	// #define 18 바람 내성 증가
			SET_EFFECT_VALUE_ADD( m_nResist[IAT_WIND], pi_fEffectValue );
			break;
		case 19 :	// #define 19 스킬 레벨 상승
			SET_EFFECT_VALUE_ADD( m_bySkill_Level, pi_fEffectValue );
			break;
		case 20 :	// #define 20 이동속도 상승
			SET_EFFECT_VALUE_ADD( m_fMoveSpeed, pi_fEffectValue );
			break;
		case 21 :	// #define 21 스텔스
			break;
		case 22 :	// #define 22 디텍트
		case 23 :	// #define 23 지속피해보조 걸리지 않음
			break;
		case 24 :	// #define 24 대상의 약한 부위 찾기
			SET_EFFECT_VALUE_INC( m_bEnable_MarkWeekPoint );
			break;
		case 25 :	// #define 25 FP 회복량 증가
		case 26 :	// #define 26 시야범위 감소
			SET_EFFECT_VALUE_ADD( m_fVisualField, pi_fEffectValue );
			break;
	}
}

void
CHARACTER_CONDITION_INFO::ApplySFEffect_Continuance_Bool( BYTE pi_byEffectType, float pi_fEffectValue, BOOL pi_bIsAdd )
{
	switch( pi_byEffectType )
	{
		case 0 :	// #define 0 대상의 공격 스킬 사용 제한
			SET_EFFECT_VALUE_INC( m_bDisable_UseSkillForce[CCI_AT_SKILL][SFAT_ATTACK] );
			break;
		case 1 :	// #define 1 대상의 공격 포스 사용 제한
			SET_EFFECT_VALUE_INC( m_bDisable_UseSkillForce[CCI_AT_FORCE][SFAT_ATTACK] );
			break;
		case 2 :	// #define 2 대상의 지속이득보조 스킬 사용 제한
			SET_EFFECT_VALUE_INC( m_bDisable_UseSkillForce[CCI_AT_SKILL][SFAT_SUPPORT_CONTINUANCE_PROFIT] );
			break;
		case 3 :	// #define 3 대상의 지속이득보조 포스 사용 제한
			SET_EFFECT_VALUE_INC( m_bDisable_UseSkillForce[CCI_AT_FORCE][SFAT_SUPPORT_CONTINUANCE_PROFIT] );
			break;
		case 4 :	// #define 4 대상의 지속피해보조 포스 사용 제한
			SET_EFFECT_VALUE_INC( m_bDisable_UseSkillForce[CCI_AT_FORCE][SFAT_SUPPORT_CONTINUANCE_LOSS] );
			break;
		case 5 :	// #define 5 대상 스텔스
			break;
		case 6 :	// #define 6 대상의 이동 제한
			SET_EFFECT_VALUE_INC( m_bDisable_Move );
			break;
		case 7 :	// #define 7 대상의 뛰기 제한
			SET_EFFECT_VALUE_INC( m_bDisable_Run );
			break;
		case 8 :	// #define 8 절대회피
		case 9 :	// #define 9 크리티컬 성공 확률 100%
			break;
		case 10 :	// #define 10 대상의 HP/FP/SP 자동 회복 불가
			break;
		case 11 :	// #define 11 대상의 방패 방어력 무시
		case 12 :	// #define 12 대상의 방어력 무시
		case 13 :	// #define 13 스턴
			break;
		case 14 :	// #define 14 반격기
			SET_EFFECT_VALUE_INC( m_bEnable_CounterAttack );
			break;
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CHARACTER_CONDITION_INFO::ApplyPotionEffect( POTION_DATA * pi_pPotionData, BOOL pi_bIsAdd )
{
	if( !pi_pPotionData )
		return;

	for( int i = 0; i < MAX_POTION_EFFECT_NUM; ++i )
	{
		if( pi_pPotionData->byEffectType[i] == PET_INC_ATTACK_POINT )
		{
			for( int j = CCI_AT_NEAR; j < CCI_AT_LAUNCHER; ++j )
			{
				if( pi_bIsAdd )	m_fStrikingPower[j] += pi_pPotionData->fEffectValue[i];
				else			m_fStrikingPower[j] -= pi_pPotionData->fEffectValue[i];
			}
		}
		else if( pi_pPotionData->GetEffectType( i ) == PET_INC_DEFENCE_POINT )
		{
			if( pi_bIsAdd )	m_fDefencePower += pi_pPotionData->fEffectValue[i];
			else			m_fDefencePower -= pi_pPotionData->fEffectValue[i];
		}
	}

	// 모든 효과를 합친다.
	CombineAllEffect();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

enum ITEM_SPECIAL_EFFECT_TYPE
{
	ISET_NONE, ISET_INC_SP, ISET_DEC_USING_FP, ISET_INC_HIT_PROB, ISET_INC_AVOID_PROB,
	ISET_INC_MAX_HPFP, ISET_INC_STRIKING_POWER, ISET_INC_DEFENCE_POWER, ISET_INC_SKILL_LEVEL, ISET_ENABLE_STEALTH,
	ISET_ENABLE_DETECT, ISET_CANCEL_LOSS_SUPPORT_SKILL, ISET_INC_SPEED, ISET_ENABLE_MARK_WEEK_POINT
};

void
CHARACTER_CONDITION_INFO::ApplyItemEffect( CNormalItem * pi_pNormalItem, BOOL pi_bIsAdd )
{
	if( !pi_pNormalItem )
		return;

	// 장착 아이템이나 반지, 목걸이 중의 하나여야 된다.
	if( !( pi_pNormalItem->IsEquipmentItem() ||
		   pi_pNormalItem->IsEqualItemKind( IEPT_RING ) ||
		   pi_pNormalItem->IsEqualItemKind( IEPT_AMULET ) ) )
		return;

	// 아이템의 특수 능력 적용
	ApplyItemEffect_Special( pi_pNormalItem, pi_bIsAdd );

	// 아이템의 업그레이드 능력 적용
	ApplyItemEffect_Upgrade( pi_pNormalItem, pi_bIsAdd );
	
	// 아이템의 일반 능력 적용
	ApplyItemEffect_Normal( pi_pNormalItem, pi_bIsAdd );

	// 모든 효과를 합친다.
	CombineAllEffect();

	// 캐릭터의 약한 부위 세팅
	SetWeakPart();
}

void
CHARACTER_CONDITION_INFO::ApplyItemEffect_Special( CNormalItem * pi_pNormalItem, BOOL pi_bIsAdd )
{
	if( !m_pCharacter )
		return;

	EQUIPMENT_DATA * pEquipData = static_cast< EQUIPMENT_DATA * >( pi_pNormalItem->GetData() );

	int		i, j;
	float	fEffectValue;

	for( i = 0; i < pEquipData->byMaxEffectNum; ++i )
	{
		fEffectValue = pEquipData->fEffectValue[i];

		if( fEffectValue == 0  )
			continue;

		switch( pEquipData->byEffectType[i] )
		{
			case ISET_NONE :
				break;

			case ISET_INC_SP :						// SP 증가 ( * )
				break;

			case ISET_DEC_USING_FP :				// 스킬/포스 사용시 FP 소모량 감소 ( * )
				if( pi_bIsAdd )	m_fConsumeFP += fEffectValue;
				else			m_fConsumeFP -= fEffectValue;
				break;

			case ISET_INC_HIT_PROB :				// 일반공격 명중률 증가 ( * )
				for( j = CCI_AT_NEAR; j <= CCI_AT_FAR; ++j )
				{
					if( pi_bIsAdd )	m_fHitProb[j] += fEffectValue;
					else			m_fHitProb[j] -= fEffectValue;
				}
				break;

			case ISET_INC_AVOID_PROB :				// 일반공격 회피율 증가 ( * )
				for( j = CCI_AT_NEAR; j <= CCI_AT_FAR; ++j )
				{
					if( pi_bIsAdd )	m_fAvoidProb[j] += fEffectValue;
					else			m_fAvoidProb[j] -= fEffectValue;
				}
				break;

			case ISET_INC_STRIKING_POWER :			// 모든 공격력 증가 ( * )
				for( j = CCI_AT_SKILL; j <= CCI_AT_FAR; ++j )
				{
					if( pi_bIsAdd )	m_fStrikingPower[j] += fEffectValue;
					else			m_fStrikingPower[j] -= fEffectValue;
				}
				break;

			case ISET_INC_DEFENCE_POWER :			// 모든 방어력 증가 ( * )
				if( pi_bIsAdd )	m_fDefencePower += fEffectValue;
				else			m_fDefencePower -= fEffectValue;
				break;

			case ISET_INC_SKILL_LEVEL :				// 스킬 레벨 상승 ( + )
				if( pi_bIsAdd )	m_bySkill_Level += fEffectValue;
				else			m_bySkill_Level -= fEffectValue;
				break;

			case ISET_ENABLE_STEALTH :				// 스텔스
				//if( pi_bIsAdd )	++m_bEnable_Stealth;
				//else				--m_bEnable_Stealth;
				break;

			case ISET_ENABLE_DETECT :				// 디텍트
				//if( pi_bIsAdd )	++m_bEnable_Detect;
				//else				--m_bEnable_Detect;
				break;

			case ISET_CANCEL_LOSS_SUPPORT_SKILL :	// 지속피해보조기술 해제
				break;

			case ISET_INC_SPEED :					// 이동 속도 증가 ( + )
				if( m_pCharacter->IsEqualCharType( CTI_AVATAR ) )
				{
					if( pi_bIsAdd )	m_fMoveSpeed += fEffectValue;
					else			m_fMoveSpeed -= fEffectValue;
				}
				break;

			case ISET_ENABLE_MARK_WEEK_POINT :		// 적 플레이어의 약한 부위를 표시
				if( pi_bIsAdd )	++m_bEnable_MarkWeekPoint;
				else			--m_bEnable_MarkWeekPoint;
				break;
		}
	}
}

// 아이템 업그레이드 효과 적용
// 아이템 하나가 아닌 전체에 전부 영향을 미치는 요소들만 세팅한다.
void
CHARACTER_CONDITION_INFO::ApplyItemEffect_Upgrade( CNormalItem * pi_pNormalItem, BOOL pi_bIsAdd )
{
	if( !pi_pNormalItem->IsUpgradable() )
		return;

	CItemUpgradeEffect * pUpgradeEffect = pi_pNormalItem->GetUpgradeEffectInfo();
	if( !pUpgradeEffect )
		return;

	if( pi_pNormalItem->IsEqualItemKind( IEPT_WEAPON ) )
	{
		BYTE byAttackType = CCI_AT_NEAR;
		switch( pi_pNormalItem->GetWeaponCategoryID() )
		{
			case IWCT_BOW		:
			case IWCT_FIREARM	:
			case IWCT_THROW		: byAttackType = CCI_AT_FAR; break;
			case IWCT_LUANCHER	: byAttackType = CCI_AT_LAUNCHER; break;
		}

		if( pi_bIsAdd )
		{
			m_fStrikingPower[byAttackType] += pUpgradeEffect->m_fStrikingPower;
			m_nAttackDistance[byAttackType]+= pUpgradeEffect->m_byAttackRange;	
		}
		else
		{
			m_fStrikingPower[byAttackType] -= pUpgradeEffect->m_fStrikingPower;
			m_nAttackDistance[byAttackType]-= pUpgradeEffect->m_byAttackRange;
		}
	}

	for( int i = 0; i < CCI_MAX_ATTACK_TYPE; ++i )
	{
		if( pi_bIsAdd )
			m_fAvoidProb[i] += pUpgradeEffect->m_fAvoidProb;
		else
			m_fAvoidProb[i] -= pUpgradeEffect->m_fAvoidProb;
	}

	for( i = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
	{
		if( pi_bIsAdd )
			m_nResist_Upgrade[i] += pUpgradeEffect->m_byResist[i];
		else
			m_nResist_Upgrade[i] -= pUpgradeEffect->m_byResist[i];
	}
}

void
CHARACTER_CONDITION_INFO::ApplyItemEffect_Normal( CNormalItem * pi_pNormalItem, BOOL pi_bIsAdd )
{
	if( !pi_pNormalItem || !m_pCharacter )
		return;

	// 아바타라면 장신구의 내성을 세팅한다.
	if( m_pCharacter->IsEqualCharType( CTI_AVATAR ) )
	{
		if( pi_pNormalItem->IsEqualItemKind( IEPT_RING ) || pi_pNormalItem->IsEqualItemKind( IEPT_AMULET ) )
		{
			RING_DATA * pRingData = static_cast< RING_DATA * >( pi_pNormalItem->GetData() );
			for( int i = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
			{
				if( pi_bIsAdd )
					m_nResist_Accessory[i]	+= pRingData->wResist[i];
				else
					m_nResist_Accessory[i]	-= pRingData->wResist[i];
			}
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 모든 이펙트의 효과를 합친다.
void
CHARACTER_CONDITION_INFO::CombineAllEffect( void )
{
	if( !m_pCharacter )
		return;

	// 플레이어
	int i, j;
	if( m_pCharacter->IsEqualCharType( CTI_AVATAR ) || m_pCharacter->IsEqualCharType( CTI_PLAYER ) )
	{
		CPlayer *				pPlayer	= ( CPlayer * )m_pCharacter;
		CNormalItem *			pNormalItem	= NULL;
		CItemUpgradeEffect *	pNormalItemUpInfo = NULL;

		// init
		m_dwTotalDefencePower		= 0;
		m_dwTotalAttackDelay		= 0;

		m_nTotalResist[0] = m_nTotalResist[1] = m_nTotalResist[2] = m_nTotalResist[3] = 0;
		m_dwTotalStrikingPower[0]	= m_dwTotalStrikingPower[1] = 0;
		m_dwTotalForcePower[0]		= m_dwTotalForcePower[1] = 0;
		m_fTotalHitProb				= m_fHitProb[CCI_AT_NEAR];

		// setting
		for( i = IEPT_UPPER_PART; i <= IEPT_CLOAK; ++i )
		{
			pNormalItem = pPlayer->GetNormalItem( i );
			if( !pNormalItem )
				continue;
			pNormalItemUpInfo = pNormalItem->GetUpgradeEffectInfo();
			if( !pNormalItemUpInfo )
				continue;

			// 방어구일 경우
			if( pNormalItem->IsArmor() )
			{
				static const float DEFENCE_CONST[5] = { 0.23f, 0.22f, 0.17f, 0.18f, 0.20f };

				float fDefence_Exclude_Shield = 1.0f;
				if( !pNormalItem->IsEqualItemKind( IEPT_SHIELD ) )
					fDefence_Exclude_Shield = m_fDefence_Exclude_Shield;

				float fDefenceConst = 1.0f;
				if( pNormalItem->GetItemKind() >= IEPT_UPPER_PART && pNormalItem->GetItemKind() <= IEPT_HELMET )
					fDefenceConst = DEFENCE_CONST[ pNormalItem->GetItemKind() - IEPT_UPPER_PART ];
				m_dwTotalDefencePower	+= (float)pNormalItem->GetDefencePoint() * m_fDefencePower * fDefence_Exclude_Shield * fDefenceConst;

				for( j = 0; j < MAX_ITEM_ATTRIBUTE; ++j )
					m_nTotalResist[j]	+= pNormalItem->GetResist( j );
				m_dwTotalAttackDelay	+= pNormalItem->GetAttackDelay();
			}
			// 무기일 경우
			else if( pNormalItem->IsEqualItemKind( IEPT_WEAPON ) )
			{
				BYTE byAttackType = CCI_AT_NEAR;	// default = near
				switch( pNormalItem->GetWeaponCategoryID() )
				{
					case IWCT_BOW		:
					case IWCT_FIREARM	:
					case IWCT_THROW		: byAttackType = CCI_AT_FAR; break;
					case IWCT_LUANCHER	: byAttackType = CCI_AT_LAUNCHER; break;
				}

				WEAPON_DATA * pWeaponData = static_cast< WEAPON_DATA * >( pNormalItem->GetData() );
				m_dwTotalStrikingPower[0]	= pWeaponData->GetMinStrikingPower() * m_fStrikingPower[byAttackType];
				m_dwTotalStrikingPower[1]	= pWeaponData->GetMaxStrikingPower() * m_fStrikingPower[byAttackType];
				m_dwTotalForcePower[0]		= pWeaponData->GetMinForcePower() * m_fStrikingPower[CCI_AT_FORCE];
				m_dwTotalForcePower[1]		= pWeaponData->GetMaxForcePower() * m_fStrikingPower[CCI_AT_FORCE];
				m_fTotalHitProb				= m_fHitProb[byAttackType];
			}
		}

		// 방어력의 마지막에 적용 ( 쉴드때문에 적용됐음 )
		m_dwTotalDefencePower /= 2.0f;

		// 내성은 캐릭터에 적용되므로 여기에서 처리
		for( j = 0; j < MAX_ITEM_ATTRIBUTE; ++j )
		{
			m_nTotalResist[j] += m_nResist_Upgrade[j] + m_nResist_Accessory[j] + m_nResist[j];

			if( m_nTotalResist[j] < 0 )
				m_nTotalResist[j] = 0;
			else if( m_nTotalResist[j] > MAX_RESIST )
				m_nTotalResist[j] = MAX_RESIST;
		}
	}

	//--------------------------------------------------------------------------
	// 몬스터 or NPC
	else
	{
	}

	// 아바타라면 UI 세팅
	if( m_pCharacter->IsEqualCharType( CTI_AVATAR ) )
	{
		_GetAvatar()->UI_RefreshCharInfoWindow();
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 캐릭터 정보를 받아서 약한 부위를 세팅한다.
void
CHARACTER_CONDITION_INFO::SetWeakPart( void )
{
	if( !m_pCharacter )
		return;
	if( !( m_pCharacter->IsEqualCharType( CTI_AVATAR ) || m_pCharacter->IsEqualCharType( CTI_PLAYER ) ) )
		return;

	CPlayer * pPlayer = static_cast< CPlayer * >( m_pCharacter );

	m_byWeakPartID = IEPT_UPPER_PART;						// default는 서버에서의 0번 파트임

	CNormalItem *	pEquipItem;
	DWORD			dwPrevDefencePoint = ID_INVALID;
	DWORD			dwCurDefencePoint = ID_INVALID;
	for( int i = IEPT_UPPER_PART; i <= IEPT_HELMET; ++i )	// 상의에서 헬멧까지 5부분만 체크하면 된다.
	{
		// get item
		pEquipItem = pPlayer->GetNormalItem( i );
		if( !pEquipItem )
			continue;

		// 방어력 체크
		dwCurDefencePoint = pEquipItem->GetDefencePoint();
		if( dwCurDefencePoint < dwPrevDefencePoint )
		{
			m_byWeakPartID = i;
			dwPrevDefencePoint = dwCurDefencePoint;
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CHARACTER_CONDITION_INFO::SetStun( BOOL pi_bIsStun )
{
	m_bEnable_Stun			= pi_bIsStun;

	if( pi_bIsStun )
	{
		m_dwStunStartTime	= timeGetTime();
		m_pCharacter->Disable( ROSF_ANIMATION | ROSF_MOVABLE );
	}
	else
	{
		m_dwStunStartTime	= 0;
		m_pCharacter->Enable( ROSF_ANIMATION | ROSF_MOVABLE );
	}
}

void
CHARACTER_CONDITION_INFO::ProcessStun( void )
{
	const static DWORD STUN_LIMIT_TIME = 1000;

	if( m_bEnable_Stun )
	{
		if( timeGetTime() - m_dwStunStartTime >= STUN_LIMIT_TIME )
			SetStun( FALSE );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#ifdef _DEVELOPER_

// 캐릭터의 상태 정보를 출력
void
CHARACTER_CONDITION_INFO::DisplayConditionInfo( void )
{
	char l_pBuf[2048], l_pTempBuf[128];
	int	 l_nCnt = 0;

	ZeroMemory( l_pBuf, 2048 );

	sprintf( l_pTempBuf, "01. 사정 거리 (+): <S>%d - <F>%d - <M>%d - <B>%d - <F>%d - <L>%d - <T>%d\n",
			 m_nAttackDistance[0], m_nAttackDistance[1], m_nAttackDistance[2], m_nAttackDistance[3],
			 m_nAttackDistance[4], m_nAttackDistance[5], m_nAttackDistance[6] );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "02. 공격 속도 (+) : <S>%d - <F>%d - <M>%d - <B>%d - <F>%d - <L>%d - <T>%d\n",
			 m_nAttackSpeed[0], m_nAttackSpeed[1], m_nAttackSpeed[2], m_nAttackSpeed[3],
			 m_nAttackSpeed[4], m_nAttackSpeed[5], m_nAttackSpeed[6] );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "03. 공격력 (*) : <S>%.2f - <F>%.2f - <M>%.2f - <B>%.2f - <F>%.2f - <L>%.2f - <T>%.2f\n",
			 m_fStrikingPower[0], m_fStrikingPower[1], m_fStrikingPower[2], m_fStrikingPower[3],
			 m_fStrikingPower[4], m_fStrikingPower[5], m_fStrikingPower[6] );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "04. 명중률 (*) : <S>%.2f - <F>%.2f - <M>%.2f - <B>%.2f - <F>%.2f - <L>%.2f - <T>%.2f\n",
			 m_fHitProb[0], m_fHitProb[1], m_fHitProb[2], m_fHitProb[3],
			 m_fHitProb[4], m_fHitProb[5], m_fHitProb[6] );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "05. 회피율 (*) : <S>%.2f - <F>%.2f - <M>%.2f - <B>%.2f - <F>%.2f - <L>%.2f - <T>%.2f\n",
			 m_fAvoidProb[0], m_fAvoidProb[1], m_fAvoidProb[2], m_fAvoidProb[3],
			 m_fAvoidProb[4], m_fAvoidProb[5], m_fAvoidProb[6] );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "06. FP 소모량 (*) : %.2f\n", m_fConsumeFP );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "07. 이동 속도 (+) : %.2f\n", m_fMoveSpeed );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "08. 시야 거리 (*) : %.2f\n", m_fVisualField );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "09. 내성 (+) : %d\n", m_nResist );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "10. 방어력 - 방패 제외 (*) : %.2f\n", m_fDefence_Exclude_Shield );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "================================================\n" );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	/*
	sprintf( l_pTempBuf, "11. 최대 HP & FP (*) : %.2f\n", m_fMaxHPnFP );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );*/

	sprintf( l_pTempBuf, "12. 방어력 (*) : %.2f\n", m_fDefencePower );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "13. 스킬 레벨 (+) : %d\n", m_bySkill_Level );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "14. 적 캐릭터의 약한 부위 표시 (+) : %d\n", m_bEnable_MarkWeekPoint );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "================================================\n" );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "15. 스텔스 가능 (+) : %d\n", m_bEnable_Stealth );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "16. 디텍트 가능 (+) : %d\n", m_bEnable_Detect );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "17. 반격 가능 (+) : %d\n", m_bEnable_CounterAttack );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "18. 달리기 불가능 (+) : %d\n", m_bDisable_Run );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "19. 움직임 불가능 (+) : %d\n", m_bDisable_Move );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "20. 스킬 사용 불가능 (+) : <A>%d - <SO>%d - <SCP>%d - <SCL>%d\n", m_bDisable_UseSkillForce[0][0],
			 m_bDisable_UseSkillForce[0][1], m_bDisable_UseSkillForce[0][2], m_bDisable_UseSkillForce[0][3] );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "21. 포스 사용 불가능 (+) : <A>%d - <SO>%d - <SCP>%d - <SCL>%d\n", m_bDisable_UseSkillForce[1][0],
			 m_bDisable_UseSkillForce[1][1], m_bDisable_UseSkillForce[1][2], m_bDisable_UseSkillForce[1][3] );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "================================================\n" );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "22. 일반 공격력 : <Min>%d - <Max>%d\n", m_dwTotalStrikingPower[0], m_dwTotalStrikingPower[1] );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "23. 포스 공격력 : <Min>%d - <Max>%d\n", m_dwTotalForcePower[0], m_dwTotalForcePower[1] );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "24. 방어력 : %d\n", m_dwTotalDefencePower );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "25. 내성 : <Fire>%d - <Water>%d - <Earth>%d - <Wind>%d\n", 
			 m_nTotalResist[0], m_nTotalResist[1], m_nTotalResist[2], m_nTotalResist[3] );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "26. 증가된 딜레이 : %d\n", m_dwTotalAttackDelay );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "27. 현재 무기의 명중률 증가치: %.2f\n", m_fTotalHitProb );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	sprintf( l_pTempBuf, "28. 약한 부위 : %d\n", m_byWeakPartID );
	sprintf( &l_pBuf[l_nCnt], "%s", l_pTempBuf );
	l_nCnt += strlen( l_pTempBuf );

	_GUIMessageWin( l_pBuf, 0xD0FFFFFF, MSGBOX_ID_CONDITION_INFO, 300, 200 );
}

#endif

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

APPLIED_SF::APPLIED_SF()
{
	ZeroMemory( m_dwApplyTime, sizeof( DWORD ) * 2 * MAX_APPLIED_SF );
	ZeroMemory( m_dwRemainTime, sizeof( DWORD ) * 2 * MAX_APPLIED_SF );
	m_bySFNum[0]			= 0;
	m_bySFNum[1]			= 0;
	ZeroMemory( m_byLatestSF, 4 );

	m_bSetUI				= FALSE;
	m_dwPrevFrameMoveTime	= 0;

	m_pCharCondInfo			= NULL;

	m_dwEffectKey[0]		= 0;					// 사용중인 이펙트 아이디. JUI
	m_dwEffectKey[1]		= 0;
	m_dwUniqueID[0][0]		= 0;					// 중간에 바뀔경우 유닉크아이디로 지워주어야한다. JUI
	m_dwUniqueID[0][1]		= 0;
	m_dwUniqueID[1][0]		= 0;
	m_dwUniqueID[1][1]		= 0;
}

APPLIED_SF::~APPLIED_SF()
{
}

DWORD
APPLIED_SF::AddSFItem( CSFItem * pi_pSFItem )
{
	if( !pi_pSFItem || !m_pCharCondInfo )
		return FALSE;

	// 지속 보조 기술인지 검사
	BYTE byUsingType = pi_pSFItem->GetContinuanceUsingType();
	if( byUsingType == 0xFF )
		return FALSE;

	BYTE byProfitType = byUsingType - SFAT_SUPPORT_CONTINUANCE_LOSS;


	// 같은 스킬/포스가 있는지를 검사
	for( int i = 0; i < m_bySFNum[byProfitType]; ++i )
	{
		if( !m_listSF[byProfitType][i].IsEmpty() )
		{
			// 같은 스킬이 있다면 현재 스킬로 대체한다.
			if( m_listSF[byProfitType][i].IsSameKind( pi_pSFItem ) )
			{
				// 레벨이 같거나 높을 경우
				if( pi_pSFItem->GetLevel() >= m_listSF[byProfitType][i].GetLevel() )
				{
					m_pCharCondInfo->ApplySFEffect( &m_listSF[byProfitType][i], FALSE );

					m_listSF[byProfitType][i].SetLevel( pi_pSFItem->GetLevel() );
					m_dwApplyTime[byProfitType][i]	= timeGetTime();
					m_dwRemainTime[byProfitType][i]	= pi_pSFItem->GetEffectTime();

					m_pCharCondInfo->ApplySFEffect( &m_listSF[byProfitType][i], TRUE );

					return TRUE + 1;
				}
				// 레벨이 낮을 경우
				else
				{
					return FALSE;
				}
			}
		}
	}

	// 스킬/포스 갯수가 MAX_APPLIED_SF 을 넘는지 검사
	if( m_bySFNum[byProfitType] >= MAX_APPLIED_SF )
		DeleteSFItem( byProfitType + SFAT_SUPPORT_CONTINUANCE_LOSS, 0 );

	// 스킬/포스 추가
	memcpy( &m_listSF[byProfitType][m_bySFNum[byProfitType]], pi_pSFItem, sizeof( CSFItem ) );
	m_dwApplyTime[byProfitType][m_bySFNum[byProfitType]]	= timeGetTime();
	m_dwRemainTime[byProfitType][m_bySFNum[byProfitType]]	= pi_pSFItem->GetEffectTime();

	m_pCharCondInfo->ApplySFEffect( &m_listSF[byProfitType][m_bySFNum[byProfitType]], TRUE );

	++m_bySFNum[byProfitType];

	return TRUE;
}

DWORD
APPLIED_SF::DeleteSFItem( CSFItem * pi_pSFItem )
{	
	if( !pi_pSFItem || !m_pCharCondInfo )
		return FALSE;

	BYTE byProfitType, byOrderNo = 0xFF;

	if( pi_pSFItem->GetContinuanceUsingType() == 0xFF )
		return FALSE;
	
	byProfitType = pi_pSFItem->GetContinuanceUsingType() - SFAT_SUPPORT_CONTINUANCE_LOSS;
	for( int i = 0; i < m_bySFNum[byProfitType]; ++i )
	{
		if( m_listSF[byProfitType][i].IsSameKind( pi_pSFItem ) )
		{
			byOrderNo = i;
			break;
		}
	}

	if( byOrderNo == 0xFF )
		return FALSE;

	return DeleteSFItem( byProfitType + SFAT_SUPPORT_CONTINUANCE_LOSS, byOrderNo );
}

DWORD
APPLIED_SF::DeleteSFItem( BYTE pi_byProfitType, BYTE pi_byOrderNo )
{	
	if( !m_pCharCondInfo )
		return ID_INVALID;

	if( ( pi_byProfitType > SFAT_SUPPORT_CONTINUANCE_PROFIT ) ||
		( pi_byOrderNo >= MAX_APPLIED_SF ) )
		return ID_INVALID;

	BYTE byProfitType = pi_byProfitType - SFAT_SUPPORT_CONTINUANCE_LOSS;
	if( pi_byOrderNo >= m_bySFNum[byProfitType] )
		return ID_INVALID;

	//--------------------------------------------------------------------------

	DWORD dwDTCode = m_listSF[byProfitType][pi_byOrderNo].GetDTCode();

	//--------------------------------------------------------------------------
	// 내부 데이터 설정
	m_pCharCondInfo->ApplySFEffect( &m_listSF[byProfitType][pi_byOrderNo], FALSE );

	for( int i = pi_byOrderNo; i < m_bySFNum[byProfitType]-1; ++i )
	{
		memcpy( &m_listSF[byProfitType][i], &m_listSF[byProfitType][i+1], sizeof( CSFItem ) );
		m_dwApplyTime[byProfitType][i]	= m_dwApplyTime[byProfitType][i+1];
		m_dwRemainTime[byProfitType][i]	= m_dwRemainTime[byProfitType][i+1];
	}

	m_listSF[byProfitType][i].SetEmpty();
	m_dwApplyTime[byProfitType][i]	= 0;
	m_dwRemainTime[byProfitType][i]	= 0;

	--m_bySFNum[byProfitType];

	return dwDTCode;
}

DWORD
APPLIED_SF::DeleteLastestAppliedSFItem( void )
{
	DWORD	dwAppliedTime	= 0;
	BYTE	byProfitType	= ID_INVALID;
	BYTE	byOrderNo		= ID_INVALID;

	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < m_bySFNum[i]; ++j )
		{
			if( m_dwApplyTime[i][j] > dwAppliedTime )
			{
				dwAppliedTime	= m_dwApplyTime[i][j];
				byProfitType	= i;
				byOrderNo		= j;
			}
		}
	}

	return DeleteSFItem( byProfitType + SFAT_SUPPORT_CONTINUANCE_LOSS, byOrderNo );
}

CSFItem *
APPLIED_SF::GetSFItem( BYTE pi_byProfitType, BYTE pi_byOrderNo )
{
	if( ( pi_byProfitType > SFAT_SUPPORT_CONTINUANCE_PROFIT ) ||
		( pi_byOrderNo >= MAX_APPLIED_SF ) )
		return NULL;

	pi_byProfitType -= SFAT_SUPPORT_CONTINUANCE_LOSS;

	if( m_listSF[pi_byProfitType][pi_byOrderNo].IsEmpty() )
		return NULL;

	return &m_listSF[pi_byProfitType][pi_byOrderNo];
}

CSFItem *
APPLIED_SF::GetSFItemBySpecialEffect( BYTE pi_bySpecialEffect )
{
	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < m_bySFNum[i]; ++j )
		{
			if( m_listSF[i][j].GetSpecialEffect() == pi_bySpecialEffect )
				return &m_listSF[i][j];
		}
	}

	return NULL;
}

void
APPLIED_SF::Clear( void )
{
	for( int i = SFAT_SUPPORT_CONTINUANCE_LOSS; i <= SFAT_SUPPORT_CONTINUANCE_PROFIT; ++i )
	{
		while( m_bySFNum[i-SFAT_SUPPORT_CONTINUANCE_LOSS] > 0 )
			DeleteSFItem( i, 0 );

		if( m_bSetUI )
			_GetAppliedSFWindow()->ClearQueue( i );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
APPLIED_SF::FrameMove( void )
{
	DWORD dwCurTime = _GetTime();

	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < MAX_APPLIED_SF; ++j )
		{
			if( m_listSF[i][j].IsEmpty() )
				continue;

			if( dwCurTime - m_dwPrevFrameMoveTime > m_dwRemainTime[i][j] )
				m_dwRemainTime[i][j] = 0;
			else
				m_dwRemainTime[i][j] -= ( dwCurTime - m_dwPrevFrameMoveTime );

			// 시간이 지났는데도 서버에서 delete 메시지가 안오는 경우가 
			// 간혹 있기 때문에.. 일단 임시로 클라이언트에서 지워준다.
			if( !m_dwRemainTime[i][j] && m_bSetUI )
			{
				if( _GetAvatar()->DeleteAppliedSF( &m_listSF[i][j] ) )
					--j; // delete되면 한칸씩 shift하니깐 현재위치 다시 체크
			}
		}
	}

	m_dwPrevFrameMoveTime = dwCurTime;
}

CSFItem *
APPLIED_SF::UpdateRemainTime( BYTE pi_bySFType, DWORD pi_dwDTIndex, DWORD pi_dwTime )
{
	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < MAX_APPLIED_SF; ++j )
		{
			if( m_listSF[i][j].IsEqualSFType( pi_bySFType ) &&
				( m_listSF[i][j].GetDTIndex() == pi_dwDTIndex ) )
			{
				m_dwRemainTime[i][j] = pi_dwTime * 1000;
				return &m_listSF[i][j];
			}
		}
	}

	return NULL;
}

DWORD
APPLIED_SF::GetRemainTime( BYTE pi_byProfitType, BYTE pi_byOrderNo, float pi_fRate )
{
	if( ( pi_byProfitType > SFAT_SUPPORT_CONTINUANCE_PROFIT ) ||
		( pi_byOrderNo >= MAX_APPLIED_SF ) )
		return 0;

	pi_byProfitType	-= SFAT_SUPPORT_CONTINUANCE_LOSS;

	if( pi_fRate != 0 )
		m_dwRemainTime[pi_byProfitType][pi_byOrderNo] += m_listSF[pi_byProfitType][pi_byOrderNo].GetEffectTime() * pi_fRate;

	return m_dwRemainTime[pi_byProfitType][pi_byOrderNo];
}

DWORD
APPLIED_SF::GetOriginalRemainTime( CSFItem * pi_pSFItem )
{
	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < MAX_APPLIED_SF; ++j )
		{
			if( m_listSF[i][j].IsSameKind( pi_pSFItem ) )
				return GetRemainTime( i + SFAT_SUPPORT_CONTINUANCE_LOSS, j, 0.0f );
		}
	}

	return 0;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
APPLIED_SF::ClearEffect( void )
{
	ZeroMemory( m_dwEffectKey, 8 );
	ZeroMemory( m_dwUniqueID, 16 );
}