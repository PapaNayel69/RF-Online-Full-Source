#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../GameObject/Manager/CTargetMgr.h"

#include "../../Item/CItemData.h"
#include "../../GameObject/Board/Common/CBoardManager.h"

#include "../../Network/NetMsgProcessor/Combat/CNetMsgProcessor_Combat.h"

#include "../../UserInterface/GameWindow/GUIAttackPartWindow.h"
#include "../../UserInterface/GameWindow/SkillForce/GUISkillForceQueue.h"

#include "../../CLand.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
EP_SelectAttackPart( BYTE pi_byAttackPart )
{
	switch( pi_byAttackPart )
	{
		case 0 :
			_GetAvatar()->SetAttackPart( CDPT_HELMET );
			break;

		case 1 :
			_GetAvatar()->SetAttackPart( CDPT_UPPER_PART );
			break;

		case 2 :
			_GetAvatar()->SetAttackPart( CDPT_LOWER_PART );
			break;

		case 3 :
			_GetAvatar()->SetAttackPart( CDPT_GLOVES );
			break;

		case 4 :
			_GetAvatar()->SetAttackPart( CDPT_SHOES );
			break;
	}
}

void
EP_DeselectAttackPart( void )
{
	_GetAvatar()->SetAttackPart( ID_INVALID );
}

//------------------------------------------------------------------------------

BOOL
EP_SummonMonster( DWORD pi_dwMonsterDTIndex )
{
	/*
	if( !_GetAvatar()->IsEnableMove() )
		return FALSE;

	_GetNetworkMgr()->FightMsg_RecallMonsterRequest_clzo( pi_dwMonsterDTIndex,
														  _GetAvatar()->GetRotY() );
*/
	return TRUE;
}

void
EP_OrderToMonster( BYTE pi_byOrderType )
{
	//_GetNetworkMgr()->FightMsg_RecalledMonsterModeRequest_clzo( pi_byOrderType );
}

void
EP_ReleaseMonster( void )
{
//	if( _GetAvatar()->GetSummonMonsterInfo()->m_dwSelectSummonMonsterServerIndex != ID_INVALID )
//		_GetNetworkMgr()->FightMsg_RecallRemoveRequest_clzo( _GetAvatar()->GetSummonMonsterInfo()->m_dwSelectSummonMonsterServerIndex );
}

void
EP_ReleaseAllMonster( void )
{
	/*
	if( _GetAvatar()->GetSummonMonsterInfo()->m_bySummonMonsterNum > 0 )
	{
		_GetAvatar()->GetSummonMonsterInfo()->SetSelectSummonMonster( NULL );
		_GetNetworkMgr()->FightMsg_RecallRemoveRequest_clzo( ID_INVALID );
	}*/
}

void
EP_CloseSummonMonsterWindow()
{
	_GetAvatar()->UI_DeselectSF();
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

RESULT
CAvatar::Attack( BYTE pi_byAttackType, CCharacter * pi_pTarget )
{
	if( _GetGGI()->IsRequestProcess() || !IsEnable( ROSF_MOVABLE ) )
		return FALSE;

	if( pi_pTarget )
		_GetSelectAttackPartWindow()->SetAttackCharName( pi_pTarget->GetName() );

	RESULT hResult;

	if( pi_byAttackType == CAT_NORMAL )
	{
		hResult = NormalAttack( pi_pTarget );
	}
	else
	{
		hResult = UseSkillForce_Request( pi_pTarget );
		if( hResult )
			;//UseSkillForce_Request_SetTarget( pi_pTarget );
		else
			UI_DeselectSF();
	}

	return hResult;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

RESULT
CAvatar::NormalAttack( CCharacter * pi_pTarget )
{
	RESULT l_nResult;

	if( IsEqualAction( CAI_COMBAT_ATTACK ) )
		int k = 0;

	l_nResult = CheckTargetConditionForAttack( pi_pTarget );
	if( l_nResult != ARC_SUCCESS ) return l_nResult;

	l_nResult = CheckAvatarConditionForAttack();
	if( l_nResult != ARC_SUCCESS ) return l_nResult;

	_GetTargetMgr()->SetPursuitInfo( pi_pTarget->GetCharTypeID(), PPT_NORMAL_ATTACK );
	
	l_nResult = CheckDistanceAndDelayForAttack( pi_pTarget );
	if( l_nResult != ARC_SUCCESS ) return l_nResult;

	if( IsEqualAction( CAI_MOVE_MOVING ) )
		_GetNetworkMgr()->PositionMsg_PlayerStop_clzo( FALSE );

	if( CheckObstacle( pi_pTarget ) )
	{
		m_pNMP_Combat->AttackTestRequest_clzo( CAT_NORMAL, pi_pTarget );
	}
	else
	{
		if( m_byUnitMountState == UMS_MOUNT )
			m_pNMP_Combat->AttackUnitRequest_clzo( pi_pTarget );
		else
			m_pNMP_Combat->AttackGenRequest_clzo( pi_pTarget );
	}

	return ARC_SUCCESS;
}

RESULT
CAvatar::CheckTargetConditionForAttack( CCharacter * pi_pTarget )
{
	if( !pi_pTarget || pi_pTarget->IsEqualCharType( CTI_NPC ) )
		return ARC_FAIL_STOP;

	if( pi_pTarget->IsDeath() )
		return ARC_FAIL_MOVE_TO_TARGET;

	if( IsMyCompanion( pi_pTarget ) )
		return ARC_FAIL_STOP;

	return ARC_SUCCESS;
}

RESULT
CAvatar::CheckAvatarConditionForAttack( void )
{
	if( _GetPlayerTradeBoard()->IsTrading() )
		return ARC_FAIL_STOP;

	if( m_byCombatMode == CSI_PEACE )
		return ARC_FAIL_STOP;

	if( !IsEnableMove() )
		return ARC_FAIL_STOP;

	CNormalItem * pWeapon = GetNormalItem( IEPT_WEAPON );
	if( pWeapon )
	{
		if( pWeapon->IsEqualWeaponCategoryID( IWCT_MINE ) )
			return ARC_FAIL_STOP;
	}

	if( !CheckCorrespondenceOfWeaponWithBullet() )
	{
		_GetTargetMgr()->SetPursuitInfo( PTT_NONE, PPT_NONE );
		return ARC_FAIL_STOP;
	}

	return ARC_SUCCESS;
}

RESULT
CAvatar::CheckDistanceAndDelayForAttack( CCharacter * pi_pTarget )
{
	float l_fDist = GetDist( m_vecPos, pi_pTarget->GetPosition() );
	float l_fAttackRange = GetAttackRange( CAT_NORMAL );
	if( l_fDist >= l_fAttackRange )
	{
		MoveToTarget( pi_pTarget, l_fDist - l_fAttackRange + 2 );
		return ARC_FAIL_MOVE_TO_TARGET;
	}

	if( fabs( m_vecPos[1] - pi_pTarget->GetPosY() ) >= l_fAttackRange )
		return ARC_FAIL_STOP;

	if( !CheckAttackDelay( CAT_NORMAL, NULL ) )
		return ARC_FAIL_STOP;

	return ARC_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 스킬 / 포스 사용
// TRUE - 스킬 사용 성공, FALSE - 스킬 사용 실패
BOOL
CAvatar::UseSkillForce_Request( CCharacter * pi_pTarget )
{
	if( !IsEnableMove() )
		return TRUE;		// 반드시 true로 반환

	if( !UseSkillForce_Request_PreCheck( pi_pTarget ) )
		return FALSE;

	CSFItem *	pSelectSF = NULL;
	SF_DATA *	pSelectSFData = NULL;
	BYTE		bySelectSFType;

	if( _GetGBM()->GetSelectSF() )
		pSelectSF = _GetGBM()->GetSelectSF();
	else if( _GetNextActionMgr()->IsEqualNextAction( CAI_SKILL_INVOKE ) ||
			 _GetNextActionMgr()->IsEqualNextAction( CAI_FORCE_INVOKE ) )
		pSelectSF = _GetNextActionMgr()->GetNextSF();
	if( !pSelectSF )
		return FALSE;

	pSelectSFData = pSelectSF->GetData();
	if( !pSelectSFData )
		return FALSE;

	bySelectSFType = pSelectSF->GetSFType();

	//--------------------------------------------------------------------------

	if( !UseSkillForce_Request_CheckResurrectionSF( bySelectSFType, pSelectSFData, pi_pTarget ) )
		return FALSE;

	if( !CheckAvatarConditionForCast( pSelectSF ) )
		return FALSE;

	if( !CheckTargetConditionForCast( pSelectSFData, pi_pTarget ) )
		return FALSE;

	if( !CheckRemainTimeForCast( pSelectSF, pi_pTarget ) )
		return FALSE;

	if( !CheckDelayForCast( pSelectSF ) )
		return FALSE;

	UseSkillForce_Request_SetTarget( pi_pTarget );

	if( !CheckDistanceForCast( bySelectSFType, pSelectSF, pi_pTarget ) )
		return FALSE;

	UseSkillForce_Request_ProcessEtc( pSelectSF, pi_pTarget );
	UseSkillForce_Request_SendNetMessage( pSelectSF, pi_pTarget );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAvatar::UseSkillForce_Request_PreCheck( CCharacter * pi_pTarget )
{
	// NPC 인가?
	if( pi_pTarget && pi_pTarget->IsEqualCharType( CTI_NPC ) )
		return FALSE;

	// 트레이드
	if( _GetPlayerTradeBoard()->IsTrading() )
		return FALSE;

	return TRUE;
}

// 타겟에게 부활 포스를 사용할 수 있는지를 체크
BOOL
CAvatar::UseSkillForce_Request_CheckResurrectionSF( BYTE pi_bySFType, SF_DATA * pi_pSFData, CCharacter * pi_pTarget )
{
	if( pi_pTarget && pi_pTarget->IsDeath() )
	{
		if( !( ( pi_bySFType == CAT_FORCE ) && ( pi_pSFData->dwDTIndex == 12 ) ) )
			return FALSE;
	}

	return TRUE;
}

void
CAvatar::UseSkillForce_Request_SetTarget( CCharacter * pi_pTarget )
{
	// 타겟 설정
	if( pi_pTarget && !IsInstantAction() )
	{
		// 아바타가 아닐 경우에만 타겟 설정
		if( !pi_pTarget->IsEqualCharType( CTI_AVATAR ) )
		{
			_GetTargetMgr()->SetPursuitInfo( pi_pTarget->GetCharTypeID(), PPT_SKILL_FORCE );
			_GetTargetMgr()->SetTarget( pi_pTarget );
		}
	}

	InstantAction( FALSE );
}

//==============================================================================

void
CAvatar::UseSkillForce_Request_SendNetMessage( CSFItem * pi_pSFItem, CCharacter * pi_pTarget )
{
	if( !pi_pSFItem )
		return;

	if( IsEqualAction( CAI_MOVE_MOVING ) )
		_GetNetworkMgr()->PositionMsg_PlayerStop_clzo( FALSE );

	if( CheckObstacle( pi_pTarget ) )
	{
		m_pNMP_Combat->AttackTestRequest_clzo( pi_pSFItem->GetSFType(), pi_pTarget );
	}
	else
	{
		if( pi_pSFItem->IsEqualSFType( CAT_SKILL ) )
		{
			if( pi_pSFItem->IsAttackable() )
				m_pNMP_Combat->AttackSkillRequest_clzo( pi_pTarget );
			else
				_GetNetworkMgr()->EffectMsg_SkillRequest_clzo( pi_pTarget );
		}
		else if( pi_pSFItem->IsEqualSFType( CAT_CLASS_SKILL ) )
		{
			if( pi_pSFItem->IsAttackable() )
				m_pNMP_Combat->AttackSkillRequest_clzo( pi_pTarget );
			else
				_GetNetworkMgr()->EffectMsg_ClassSkillRequest_clzo( pi_pTarget );
		}
		else
		{
			if( pi_pSFItem->IsAttackable() )
				m_pNMP_Combat->AttackForceRequest_clzo( pi_pTarget );
			else
				_GetNetworkMgr()->EffectMsg_ForceRequest_clzo( pi_pTarget );
		}
	}
}

void
CAvatar::UseSkillForce_Request_ProcessEtc( CSFItem * pi_pSFItem, CCharacter * pi_pTarget )
{
	// 자동 공격이 가능하다면 CHASE_FOR_ATTACK 상태로 돌려놓는다.
	// 현재 공격 모드가 근거리일때만 해당됨
	if( IsEnableAutoAttack() && pi_pTarget && ( GetAttackDistanceType( pi_pSFItem->GetSFType() ) == ADT_NEAR ) )
	{
		if( _GetTargetMgr()->IsEqualTarget( pi_pTarget ) )
			_GetTargetMgr()->SetPursuitPurpose( PPT_NORMAL_ATTACK );
	}

	// UI 설정
	DWORD dwDelay = GetAttackDelay( pi_pSFItem );
	_GetLinkBoard()->BeginDelayAnimation( pi_pSFItem, dwDelay ); // 링크된 SF icon 처리

	// SF 설정
	SetSFItem( pi_pSFItem );
	_GetGBM()->DeselectSF();

	// next action
	if( _GetNextActionMgr()->IsEqualNextAction( CAI_SKILL_INVOKE ) ||
		_GetNextActionMgr()->IsEqualNextAction( CAI_FORCE_INVOKE ) )
	{
		_GetNextActionMgr()->ClearNextActionID( TRUE );
		_GetNextActionMgr()->SetEmpty( TRUE );
	}

	// etc
	SetAttackMode( pi_pSFItem->GetSFType() );
	_GetUIScreen()->SetMouseState( CGUIMouse::NORMAL );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAvatar::CheckTargetConditionForCast( SF_DATA * pi_pSFData, CCharacter * pi_pTarget )
{
	BOOL l_bIsEqualTargetType = FALSE;

	if( pi_pSFData->byTargetType[SFT_ANYWHERE] )
	{
		CheckTargetConditionForCast_Anywhere( pi_pTarget );
		l_bIsEqualTargetType = TRUE;
	}
	if( !l_bIsEqualTargetType && pi_pSFData->byTargetType[SFT_MAP] )
	{
		CheckTargetConditionForCast_Map( pi_pTarget );
		l_bIsEqualTargetType = TRUE;
	}
	else
	{
		if( !pi_pTarget )
			return FALSE;

		if( !CheckTargetConditionForCast_CharacterCommon( pi_pTarget ) )
			return FALSE;

		if( !l_bIsEqualTargetType && pi_pSFData->byTargetType[SFT_ENEMY_NPC] &&
			( pi_pTarget->IsEqualCharType( CTI_MONSTER ) || pi_pTarget->IsEqualCharType( CTI_ANIMUS ) || pi_pTarget->IsEqualCharType( CTI_GUARD_TOWER ) ) )
		{
			l_bIsEqualTargetType = CheckTargetConditionForCast_EnemyNpc( pi_pTarget );
		}
		if( !l_bIsEqualTargetType && ( pi_pTarget->IsEqualCharType( CTI_AVATAR ) || pi_pTarget->IsEqualCharType( CTI_PLAYER ) ) &&
			pi_pSFData->byTargetType[SFT_SAME_RACE] )
		{
			l_bIsEqualTargetType = CheckTargetConditionForCast_Player( pi_pTarget, TRUE );
		}
		if( !l_bIsEqualTargetType && ( pi_pTarget->IsEqualCharType( CTI_AVATAR ) || pi_pTarget->IsEqualCharType( CTI_PLAYER ) ) &&
			pi_pSFData->byTargetType[SFT_ENEMY_PLAYER] )
		{
			l_bIsEqualTargetType = CheckTargetConditionForCast_Player( pi_pTarget, FALSE );
		}
	}

	return l_bIsEqualTargetType;
}

void
CAvatar::CheckTargetConditionForCast_Anywhere( CCharacter * pi_pTarget )
{
	// SFT_ANYWHERE 는 무조건 아바타에게만 적용되므로 pi_pTarget 를 아바타로 변경시킨다.
	pi_pTarget = this;

	SetSFTargetPos( m_vecPos );
}

void
CAvatar::CheckTargetConditionForCast_Map( CCharacter * pi_pTarget )
{
	if( !_GetNextActionMgr()->IsEqualNextAction( CAI_SKILL_INVOKE ) &&
		!_GetNextActionMgr()->IsEqualNextAction( CAI_FORCE_INVOKE ) )
	{
		// SFT_MAP 는 pi_pTarget 가 NULL 이다.
		// 캐릭터를 클릭했을 경우에는 캐릭터의 위치를 저장
		if( pi_pTarget )
		{
			SetSFTargetPos( pi_pTarget->GetPosition() );
		}
		// 맵 위치 저장
		else
		{
			_GetLevel()->GetPointFromScreenRay( gMouse.x, gMouse.y, &m_vecSFTargetPos );
		}
	}

	// 맵에 사용하는 스킬/포스라면 pi_pTarget를 NULL로 만든다.
	pi_pTarget = NULL;
}

BOOL
CAvatar::CheckTargetConditionForCast_CharacterCommon( CCharacter * pi_pTarget )
{
	// 스킬은 캐릭터에 사용할 수 있는데 맵을 클릭했다면 ( pi_pTarget가 NULL 이라면 ) 아바타를 해당 지점으로 이동시켜준다.
	if( !pi_pTarget )
	{
		Vector3f l_vecTargetPos;
		if( _GetLevel()->GetPointFromScreenRay( gMouse.x, gMouse.y, &l_vecTargetPos ) )
			_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( l_vecTargetPos, TRUE, TRUE );
		return FALSE;
	}

	SetSFTargetPos( pi_pTarget->GetPosition() );

	return TRUE;
}

BOOL
CAvatar::CheckTargetConditionForCast_EnemyNpc( CCharacter * pi_pTarget )
{
	if( !( pi_pTarget->IsEqualCharType( CTI_MONSTER ) || pi_pTarget->IsEqualCharType( CTI_ANIMUS ) ||
		   pi_pTarget->IsEqualCharType( CTI_GUARD_TOWER ) ) )
	{
		if( !_GetTargetMgr()->IsEqualPursuitPurpose( PPT_SKILL_FORCE ) )
			_SetSystemMsg( "현재 선택한 스킬/포스는 적 캐릭터에게만 사용 가능합니다." );
		return FALSE;
	}

	return TRUE;
}

BOOL
CAvatar::CheckTargetConditionForCast_Player( CCharacter * pi_pTarget, BOOL pi_bIsCheckSameRace )
{
	BOOL l_bResult = TRUE;

	if( !( pi_pTarget->IsEqualCharType( CTI_AVATAR ) || pi_pTarget->IsEqualCharType( CTI_PLAYER ) ) )
	{
		l_bResult = FALSE;
	}
	else
	{
		CPlayer * l_pTargetPlayer = static_cast< CPlayer * >( pi_pTarget );

		if( IsEqualRace( l_pTargetPlayer->GetRace() ) && !pi_bIsCheckSameRace )
			l_bResult = FALSE;
		else if( !IsEqualRace( l_pTargetPlayer->GetRace() ) && pi_bIsCheckSameRace )
			l_bResult = FALSE;
	}

	if( !l_bResult )
	{
		if( !_GetTargetMgr()->IsEqualPursuitPurpose( PPT_SKILL_FORCE ) )
		{
			if( pi_bIsCheckSameRace )
				_SetSystemMsg( "현재 선택한 스킬/포스는 아바타나 같은 종족일 경우에만 사용 가능합니다." );
			else
				_SetSystemMsg( "현재 선택한 스킬/포스는 다른 종족일 경우에만 사용 가능합니다." );
		}
	}

	return l_bResult;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAvatar::CheckAvatarConditionForCast( CSFItem * pi_pSFItem )
{
	if( IsDeath() )
	{
		_SetSystemMsg( "아바타가 죽은 상태에서는 스킬/포스를 장착/해제/사용할 수 없습니다." );
		return FALSE;
	}

	if( IsEqualUnitMountState( UMS_MOUNT ) )
	{
		_SetSystemMsg( "유닛 탑승상태에서는 스킬/포스를 사용할 수 없습니다." );
		return FALSE;
	}

	if( IsEqualAction( CAI_ITEM_USE_MINING ) )
	{
		_SetSystemMsg( "채굴 상태에서는 스킬/포스를 사용할 수 없습니다." );
		return FALSE;
	}

	if( !CheckAvatarConditionForCast_CheckSkillForce( pi_pSFItem ) )
		return FALSE;

	if( !CheckAvatarConditionForCast_CheckHpFpSp( pi_pSFItem ) )
		return FALSE;

	if( !CheckAvatarConditionForCast_CheckWeaponAndShield( pi_pSFItem ) )
		return FALSE;

	return TRUE;
}

BOOL
CAvatar::CheckAvatarConditionForCast_CheckSkillForce( CSFItem * pi_pSFItem )
{
	// 공격형일 경우에는 전투모드에서만 선택 가능
	if( pi_pSFItem->IsAttackable() && ( m_byCombatMode == CSI_PEACE ) )
	{
		_SetSystemMsg( "공격형 스킬/포스는 전투 모드에서만 사용 가능합니다." );
		return FALSE;
	}

	// 스킬/포스에 의한 스킬/포스 사용 제한 체크 ( 클래스 스킬은 일반 스킬과 동일하게 취급 )
	BYTE bySFType = pi_pSFItem->GetSFType();
	if( bySFType == CAT_CLASS_SKILL )
		bySFType = CAT_SKILL;
	for( int i = 0; i <= SFAT_SUPPORT_CONTINUANCE_PROFIT; ++i )
	{
		if( m_stConditionInfo.m_bDisable_UseSkillForce[bySFType][i] )
		{
			if( pi_pSFItem->IsHaveUsingType( i ) )
			{
				_SetSystemMsg( "아바타가 해당 스킬/포스의 사용 제한에 걸려있습니다." );
				return FALSE;
			}
		}
	}

	// 스킬/포스 시전에 필요한 특수효과가 걸려있는지를 체크
	SF_DATA * pSFData = pi_pSFItem->GetData();
	if( pSFData->byNeedfulSpecialEffectType != 0xFF )
	{
		if( !m_stAppliedSF.GetSFItemBySpecialEffect( pSFData->byNeedfulSpecialEffectType ) )
		{
			_SetSystemMsg( "해당 스킬/포스를 시전하기 위해선 특수한 효과가 필요합니다." );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL
CAvatar::CheckAvatarConditionForCast_CheckHpFpSp( CSFItem * pi_pSFItem )
{
	SF_DATA * pSFData = pi_pSFItem->GetData();

	if( pSFData->byHP > m_dwHP )
	{
		_SetSystemMsg( "스킬/포스 시전에 필요한 HP가 부족합니다." );
		return FALSE;
	}

	float fNeedFP = ( float )pSFData->byFP * m_stConditionInfo.m_fConsumeFP;
	if( fNeedFP > m_dwFP )
	{
		_SetSystemMsg( "스킬/포스 시전에 필요한 FP가 부족합니다." );
		return FALSE;
	}

	if( pSFData->bySP > m_dwSP )
	{
		_SetSystemMsg( "스킬/포스 시전에 필요한 SP가 부족합니다." );
		return FALSE;
	}

	return TRUE;
}

BOOL
CAvatar::CheckAvatarConditionForCast_CheckWeaponAndShield( CSFItem * pi_pSFItem )
{
	SF_DATA * pSFData = pi_pSFItem->GetData();

	// 무기 타입에 상관없이 사용 가능한가를 체크
	CNormalItem * pWeapon = GetNormalItem( IEPT_WEAPON );
	for( int i = 0; i < 16; ++i )
	{
		if( pSFData->byNeedfulWeapon[i] )
		{
			// 만약 무기 타입이 정의 되어 있다면 현재 장착 아이템을 쓸 수 있는지 체크한다.
			if( pWeapon )
			{
				// 현재 장착 아이템을 쓸 수 없다면 FALSE 리턴
				if( !pSFData->byNeedfulWeapon[ pWeapon->GetWeaponCategoryID() ] )
				{
					_SetSystemMsg( "선택한 스킬/포스는 현재 아바타가 장착중인 무기로는 사용할 수 없습니다." );
					return FALSE;
				}
				else
					break;
			}
			else
			{
				_SetSystemMsg( "선택한 스킬/포스를 사용하려면 해당 무기를 장착해 주십시오." );
				return FALSE;
			}
		}
	}

	// 원거리 스킬일 경우에는 탄환을 체크해야 한다. ( 클래스 스킬 포함. 포스나 보조는 제외 )
	if( pi_pSFItem->IsEqualSFType( CAT_SKILL ) && pi_pSFItem->IsAttackable() )
	{
		if( !CheckCorrespondenceOfWeaponWithBullet() )
			return FALSE;
	}

	// 방패 체크가 필요한지를 체크
	if( pSFData->bIsNeedfulShield )
	{
		CNormalItem * pShield = GetNormalItem( IEPT_SHIELD );
		if( !pShield )
		{
			_SetSystemMsg( "선택한 스킬/포스를 사용하려면 방패를 장착해 주십시오." );
			return FALSE;
		}
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAvatar::CheckDistanceForCast( BYTE pi_bySFType, CSFItem * pi_pSFItem, CCharacter * pi_pTarget )
{
	// 사용 거리 체크 - 타겟 타입이 SFT_ANYWHERE 인 경우에는 거리체크를 하지 않는다.
	//                - 타겟이 아바타인 경우에도 하지 않는다.
	if( !pi_pSFItem->IsEqualTargetType( SFT_ANYWHERE ) ||
		( pi_pTarget && !pi_pTarget->IsEqualCharType( CTI_AVATAR ) ) )
	{
		float fDist = 0.0f;
		if( pi_pTarget )
			fDist = GetDist( m_vecPos, pi_pTarget->GetPosition() );
		else
			fDist = GetDist( m_vecPos, m_vecSFTargetPos );

		float fAttackRange = GetAttackRange( pi_bySFType );
		if( fDist >= fAttackRange )
		{
			if( pi_pTarget )
				MoveToTarget( pi_pTarget, fDist - fAttackRange + 2 );
			else
				_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( m_vecSFTargetPos, FALSE, FALSE,
																fDist - fAttackRange + 2 );

			_GetNextActionMgr()->SetNextAction( CAI_SKILL_INVOKE );
			_GetNextActionMgr()->SetNextSF( _GetGBM()->GetSelectSF() );
			_GetNextActionMgr()->ClearNextActionID( FALSE );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL
CAvatar::CheckDelayForCast( CSFItem * pi_pSFItem )
{
	// 딜레이 체크
	int nResult = CheckAttackDelay( pi_pSFItem->GetSFType(), pi_pSFItem );
	if( nResult < TRUE )
	{
		if( nResult == 0 )
			_GetNextActionMgr()->ClearNextActionID( FALSE );

		if( ( !IsEnableAutoAttack() && IsEnableMove() ) ||
			( IsEnableAutoAttack() && !m_bShowSFDelayMsg && ( nResult == -1 ) ) )
		{
			m_bShowSFDelayMsg = TRUE;

			if( nResult == -1 )
				_SetSystemMsg( "스킬/포스를 준비중입니다." );
			else
				_SetSystemMsg( "선택한 스킬/포스는 잠시 후 사용하실 수 있습니다." );
		}
		return FALSE;
	}

	return TRUE;
}

BOOL
CAvatar::CheckRemainTimeForCast( CSFItem * pi_pSFItem, CCharacter * pi_pTarget )
{
	if( !pi_pSFItem  )
		return FALSE;

	if( !( pi_pSFItem->IsHaveUsingType( SFAT_SUPPORT_CONTINUANCE_LOSS ) ||
		   pi_pSFItem->IsHaveUsingType( SFAT_SUPPORT_CONTINUANCE_PROFIT ) ) )
		return TRUE;
	if( !pi_pTarget )
		return FALSE;
	if( !pi_pTarget->IsEqualCharType( CTI_AVATAR ) )	// 클라이언트에서는 아바타일 경우에만 체크한다.
		return TRUE;

	// 보조 지속형 스킬/포스는 남은 시간이 10%이하로 남아있어야만 재시전이 가능하다.
	DWORD dwMaxTime		= pi_pSFItem->GetEffectTime();
	DWORD dwRemainTime	= pi_pTarget->GetAppliedSF()->GetOriginalRemainTime( pi_pSFItem );

	if( dwRemainTime >= ( dwMaxTime / 10 ) )
	{
		_SetSystemMsg( "지속형 스킬/포스는 남은 시간이 10%% 이하로 남아있어야만 재시전이 가능합니다." );
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

float
CAvatar::GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget )
{
	float fAttackRange = CPlayer::GetAttackRange( pi_byAttackType, pi_bCheckTarget );

	//--------------------------------------------------------------------------
	// 유닛을 탑승하지 않았을 경우에는 스킬/포스에 의해 가감되는 사정거리 적용
	if( !IsEqualUnitMountState( UMS_MOUNT ) )
	{
		CNormalItem * pWeapon = GetNormalItem( IEPT_WEAPON );

		switch( pi_byAttackType )
		{
			case CAT_NORMAL :
				if( pWeapon )
				{
					int nWeaponCategoryID = pWeapon->GetWeaponCategoryID();

					if( nWeaponCategoryID == IWCT_BOW )
						fAttackRange += m_stConditionInfo.m_nAttackDistance[3];
					else if( nWeaponCategoryID == IWCT_FIREARM )
						fAttackRange += m_stConditionInfo.m_nAttackDistance[4];
					else if( nWeaponCategoryID == IWCT_LUANCHER )
						fAttackRange += m_stConditionInfo.m_nAttackDistance[5];
					else if( nWeaponCategoryID == IWCT_THROW )
						fAttackRange += m_stConditionInfo.m_nAttackDistance[6];
					else
						fAttackRange += m_stConditionInfo.m_nAttackDistance[2];
				}
				else
				{
					fAttackRange += m_stConditionInfo.m_nAttackDistance[2];
				}
				break;

			case CAT_SKILL :
			case CAT_CLASS_SKILL :
				fAttackRange += m_stConditionInfo.m_nAttackDistance[0];
				break;

			case CAT_FORCE :
				fAttackRange += m_stConditionInfo.m_nAttackDistance[1];
				break;
		}
	}

	//--------------------------------------------------------------------------
	// 타겟의 크기 적용
	if( pi_bCheckTarget )		// + ( 적의 몸통크기 / 2 )
	{
		CCharacter * pTarget = _GetTargetMgr()->GetTarget();
		if( pTarget )
			fAttackRange += pTarget->GetWidth() / 2.0f;
	}

	return fAttackRange;
}

// 공격 딜레이 체크
int
CAvatar::CheckAttackDelay( BYTE pi_byAttackType, CSFItem * pi_pSFItem )
{
	//--------------------------------------------------------------------------
	// 처음 공격인 경우
	if(	m_byPrevAttackType[0] == 0xFF )
		return TRUE;

	//--------------------------------------------------------------------------
	// 일반 공격
	if( pi_byAttackType == CAT_NORMAL )
	{
		// 이전 공격이 일반 공격
		if( m_byPrevAttackType[0] == CAT_NORMAL )
		{
			if( ( timeGetTime() - m_dwPrevAttackTime ) < CalculateAttackDelayWithAllEffect( m_byPrevAttackType[0], m_dwPrevAttackDelay ) )
				return FALSE;
		}
		// 이전 공격이 스킬/포스
		else
		{
			DWORD l_dwAttackDealy = (float)m_dwLevel / (float)m_dwLevel * 1000.0f;
			if( ( timeGetTime() - m_dwPrevAttackTime ) < CalculateAttackDelayWithAllEffect ( m_byPrevAttackType[0], l_dwAttackDealy ) )
				return FALSE;
		}
	}
	//--------------------------------------------------------------------------
	// 스킬/포스
	else
	{
		if( !pi_pSFItem )
			return FALSE;

		// 처음 사용
		if(	m_byPrevAttackType[1] == 0xFF )
			return TRUE;

		// 현재 스킬/포스 자체의 딜레이 적용
		DWORD dwPrevCastTime;
		if( pi_byAttackType == CAT_FORCE )
			dwPrevCastTime = _GetForceBoard()->GetCastTime( pi_pSFItem->GetDTIndex() );
		else
			dwPrevCastTime = _GetSkillBoard()->GetCastTime( pi_pSFItem->GetDTIndex() );

		if( ( timeGetTime() - dwPrevCastTime ) < CalculateAttackDelayWithAllEffect( pi_byAttackType, GetAttackDelay( pi_pSFItem ) ) )
			return ( -1 );	// 메세지 출력을 위해서 -1을 리턴

		// 기본 딜레이 적용 => 이전 공격이 일반 공격일 경우
		if( m_byPrevAttackType[0] == CAT_NORMAL )
		{
			if( ( timeGetTime() - m_dwPrevAttackTime ) < CalculateAttackDelayWithAllEffect( m_byPrevAttackType[0], m_dwPrevAttackDelay ) )
				return FALSE;
		}
		// 기본 딜레이 적용 => 이전 공격이 스킬/포스인 경우
		else
		{
			// 이전과 다른 스킬/포스일 경우
			if( ( m_byPrevAttackType[0] != pi_byAttackType ) ||
				( m_byPrevAttackType[1] != pi_pSFItem->GetDTIndex() ) )
			{
				DWORD dwAttackDealy = (float)m_dwLevel / (float)m_dwLevel * 1000.0f;
				if( ( timeGetTime() - m_dwPrevAttackTime ) < CalculateAttackDelayWithAllEffect( m_byPrevAttackType[0], dwAttackDealy ) )
					return FALSE;
			}
		}
	}

	return TRUE;
}

DWORD
CAvatar::GetAttackDelay( CSFItem * pi_pSFItem )
{
	DWORD dwAttackDelay = 0;

	if( !IsEqualUnitMountState( UMS_MOUNT ) )
	{
		// normal
		if( !pi_pSFItem )
		{
			dwAttackDelay			= (float)m_dwLevel / (float)m_dwLevel * 1000.0f;

			CNormalItem * pWeapon	= GetNormalItem( IEPT_WEAPON );
			if( pWeapon )
				dwAttackDelay += pWeapon->GetAttackDelay();
		}
		// skill or force
		else
		{
			dwAttackDelay += pi_pSFItem->GetCastDelay();
		}
	}
	else
	{
		// unit
		//if( m_pUnitData )
		//	l_dwAttackDelay += m_pUnitData->GetAttackDelay();
	}

	return dwAttackDelay;
}

DWORD
CAvatar::CalculateAttackDelayWithAllEffect( BYTE pi_byAttackType, DWORD pi_dwBasicDelay )
{
	DWORD			dwAttackDelay	= pi_dwBasicDelay;
	CNormalItem *	pWeapon			= GetNormalItem( IEPT_WEAPON );

	if( !IsEqualUnitMountState( UMS_MOUNT ) )
	{
		switch( pi_byAttackType )
		{
			case CAT_NORMAL :
				if( pWeapon )
				{
					BYTE byWeaponCategoryID = pWeapon->GetWeaponCategoryID();

					if( ( byWeaponCategoryID == IWCT_BOW ) ||
						( byWeaponCategoryID == IWCT_FIREARM ) ||
						( byWeaponCategoryID == IWCT_THROW ) )
						dwAttackDelay += m_stConditionInfo.m_nAttackSpeed[CCI_AT_FAR];
					else if( byWeaponCategoryID == IWCT_LUANCHER )
						dwAttackDelay += m_stConditionInfo.m_nAttackSpeed[CCI_AT_LAUNCHER];
					else
						dwAttackDelay += m_stConditionInfo.m_nAttackSpeed[CCI_AT_NEAR];
				}
				else
				{
					dwAttackDelay += m_stConditionInfo.m_nAttackSpeed[2];
				}

				dwAttackDelay += m_stConditionInfo.m_dwTotalAttackDelay;
				break;

			case CAT_SKILL :
			case CAT_CLASS_SKILL :
				dwAttackDelay += m_stConditionInfo.m_nAttackSpeed[CCI_AT_SKILL];
				break;

			case CAT_FORCE :
				dwAttackDelay += m_stConditionInfo.m_nAttackSpeed[CCI_AT_FORCE];
				break;
		}
	}

	return dwAttackDelay;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CAvatar::ChangeCombatMode( BYTE pi_byMode )
{
	if( IsDeath() )
	{
		_SetSystemMsg( "아바타가 죽은 상태에서는 전투 모드를 변경할 수 없습니다." );
		return;
	}

	if( _GetAvatar()->IsEqualActionCategory( CAI_MOVE ) ||
		_GetAvatar()->IsEqualAction( CAI_ITEM_USE_MINING ) )
		m_pNMP_Combat->ModeChangeRequest_clzo( pi_byMode );
}

void
CAvatar::ToggleCombatMode( void )
{
	ChangeCombatMode( !m_byCombatMode );
}

void
CAvatar::ChangeMoveMode( BYTE pi_byMode )
{
	if( !IsEnableMove() )
	{
		_SetSystemMsg( "아바타가 움직일 수 없는 상태에서는 걷기/뛰기 모드를 변경할 수 없습니다." );
		return;
	}

	m_pResProc->ChangeMoveMode( pi_byMode );
}

void
CAvatar::ToggleMoveMode( void )
{
	ChangeMoveMode( !m_byWalkMode );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// Avatar가 움직일 수 있는가?
BOOL
CAvatar::IsEnableMove( void )
{
	// check action
	if(	IsEqualAction( CAI_COMBAT_ATTACK ) ||
		IsEqualAction( CAI_FORCE_INVOKE ) || IsEqualAction( CAI_SKILL_INVOKE ) ||
		IsEqualAction( CAI_DAMAGED_CRITICAL ) ||
		IsEqualAction( CAI_GESTURE_MOUNT ) || IsEqualAction( CAI_GESTURE_DISMOUNT ) ||
		IsDeath() )
		return FALSE;

	// check unit
	if( m_pLinkPlayer )
	{
		if( m_pLinkPlayer->IsEqualAction( CAI_GESTURE_MOUNT ) ||
			m_pLinkPlayer->IsEqualAction( CAI_GESTURE_DISMOUNT ) )
			return FALSE;
	}

	return TRUE;
}

// 아바타와 타겟 사이에 장애물이 있는지를 검사
BOOL
CAvatar::CheckObstacle( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return FALSE;

	if( !_GetLand()->GetLevel() )
		return FALSE;

	if( !_GetLand()->GetLevel()->IsLoadedBsp() )
		return FALSE;

	Vector3f l_vecOrgPos, l_vecTarPos;
	memcpy( l_vecOrgPos, m_vecPos, sizeof( Vector3f ) );
	memcpy( l_vecTarPos, pi_pChar->GetPosition(), sizeof( Vector3f ) );

	float l_fAddY = 10;
	if( m_pBBox )
		l_fAddY = m_pBBox->pMax[1] - m_pBBox->pMin[1];

	l_vecOrgPos[1] += l_fAddY;
	l_vecTarPos[1] += l_fAddY;

	return _GetLand()->GetLevel()->mBsp->IsCollisionFace( l_vecOrgPos, l_vecTarPos );
}

// 캐릭터가 나와 우호적인 관계인가?
BOOL
CAvatar::IsMyCompanion( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return 0xFF;

	if( pi_pChar->IsEqualCharType( CTI_MONSTER ) )
	{
		return FALSE;
	}
	else if( pi_pChar->IsEqualCharType( CTI_NPC ) )
	{
		return TRUE;
	}
	else if( pi_pChar->IsEqualCharType( CTI_PLAYER ) )
	{
		return IsEqualRace( static_cast< CPlayer * >( pi_pChar )->GetRace() );
	}
	else if( pi_pChar->IsEqualCharType( CTI_ANIMUS ) )
	{
		return IsCora();
	}
	else if( pi_pChar->IsEqualCharType( CTI_GUARD_TOWER ) )
	{
		return IsEqualRace( static_cast< CGuardTower * >( pi_pChar )->GetRace() );
	}
	else if( pi_pChar->IsEqualCharType( CTI_AVATAR ) )
	{
		return TRUE;
	}

	return 0xFF;
}

// 현재 장착되어 있는 원거리 무기와 탄환이 맞게 설정 되어 있는지를 검사
BOOL
CAvatar::CheckCorrespondenceOfWeaponWithBullet( void )
{
	// init
	SetBulletDTIndex( ID_INVALID );
	_GetEquipBoard()->SetUsingBulletSocketNo( ID_INVALID );

	// set
	if( IsEqualUnitMountState( UMS_MOUNT ) )
		return CheckCorrespondenceOfWeaponWithBullet_WhenMount();
	else
		return CheckCorrespondenceOfWeaponWithBullet_WhenDismount();
	
}

// 현재 장착되어 있는 원거리 무기와 탄환이 맞게 설정 되어 있는지를 검사 ( 유닛 탑승시 )
BOOL
CAvatar::CheckCorrespondenceOfWeaponWithBullet_WhenMount( void )
{
	if( m_clsUnitProc.GetBulletNumThatCurAttackParts() <= 0 )
	{
		_SetSystemMsg( "현재 선택한 공격 파츠의 잔탄수가 부족합니다. 발사물을 구입하거나 다른 공격 파츠를 이용하여 주십시오." );
		return FALSE;
	}

	return TRUE;
}

// 현재 장착되어 있는 원거리 무기와 탄환이 맞게 설정 되어 있는지를 검사 ( 유닛 미탑승시 )
BOOL
CAvatar::CheckCorrespondenceOfWeaponWithBullet_WhenDismount( void )
{
	CBoardItem * pWeapon = _GetEquipBoard()->GetItem( UIPT_WEAPON );

	// 무기 장착이 안 되어 있다면 TRUE
	if( !pWeapon )
		return TRUE;

	// 원거리 무기가 아니라면 TRUE => 던지기형 무기는 따로 검사할 필요가 없다.
	if( !( pWeapon->IsEqualWeaponCategoryID( IWCT_BOW ) ||
		   pWeapon->IsEqualWeaponCategoryID( IWCT_FIREARM ) ||
		   pWeapon->IsEqualWeaponCategoryID( IWCT_LUANCHER ) ) )
		return TRUE;

	// 탄환 검사
	CBoardItem*	pBullet = NULL;
	BOOL		bIsOk = FALSE;
	for( int i = UIPT_BULLET_LEFT; i <= UIPT_BULLET_RIGHT; ++i )
	{
		pBullet = _GetEquipBoard()->GetItem( i );
		if( !pBullet )
			continue;

		if( pBullet->CheckWeaponWithBullet( pWeapon->GetDTCode() ) )
		{
			bIsOk = TRUE;
			break;
		}
	}

	if( !bIsOk )
	{
		_SetSystemMsg( "현재 장착중인 무기가 사용할 수 있는 발사물을 장착 해 주십시요." );
		return FALSE;
	}

	SetBulletDTIndex( pBullet->GetDTIndex() );
	_GetEquipBoard()->SetUsingBulletSocketNo( i );
	return TRUE;
}

// Animus룰 소환할 수 있는지를 체크
BOOL
CAvatar::IsSummonableAnimus( CAnimusItem * pi_pAnimusItem )
{
	if( !pi_pAnimusItem )
		return FALSE;

	// check expertness
	if( !IsUsableAnimus( pi_pAnimusItem ) )
		return FALSE;                                                                                                                                                                                                                                                                                                         

	// check fp
	if( m_dwFP < pi_pAnimusItem->GetNeedfulFP() )
	{
		_SetSystemMsg( "소환에 필요한 FP가 부족합니다." );
		return FALSE;
	}

	// check time
	if( !_GetAnimusBoard()->IsPassedSummonLimitTime() )
		return FALSE;

	// check hp
	static const float SUMMON_NEEDED_HP_RATE = 0.3f;
	float fHPRate = ( float )pi_pAnimusItem->GetHP() / ( float )pi_pAnimusItem->GetMaxHP();
	if( fHPRate < SUMMON_NEEDED_HP_RATE )
	{
		_SetSystemMsg( "애니머스의 HP가 30 %%이하일 경우에는 소환할 수 없습니다." );
		return FALSE;
	}

	return TRUE;
}

// 아바타를 타겟 위치까지 이동시킨다.
BOOL
CAvatar::MoveToTarget( CResObject * pi_pResObject, float pi_fDistance )
{
	if( !pi_pResObject )
		return FALSE;

	if( pi_pResObject->GetObjectTypeID() == OTI_CHAR )
		_GetTargetMgr()->SetTarget( ( CCharacter * )pi_pResObject );
	else if( pi_pResObject->GetObjectTypeID() == OTI_ITEM )
		_GetTargetMgr()->SetTargetItem( ( CGroundItem * )pi_pResObject );

	return _GetNetworkMgr()->PositionMsg_MoveRequest_clzo( pi_pResObject->GetPosition(),
														   FALSE, FALSE, pi_fDistance );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 평화 모드에서 적 캐릭터를 클릭하면 자동으로 서버에 공격 모드로 전환을 요청한다.
// 서버에서 응답이 오면 캐릭터를 공격하도록 한다.
void
CAvatar::SetRequestChangeToCombatMode( BOOL pi_bFlag )
{
	if( pi_bFlag )
	{
		if( !m_bIsRequestChangeToCombatMode )
		{
			if( GetCombatMode() == CSI_PEACE )
				m_pNMP_Combat->ModeChangeRequest_clzo( CSI_COMBAT );
		}
	}

	m_bIsRequestChangeToCombatMode = pi_bFlag;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 타겟 더미 생성
CDummy *
CAvatar::CreateDummy( float pi_fX, float pi_fZ )
{
	if( !CheckCorrespondenceOfWeaponWithBullet() )
		return NULL;

	CDummy * l_pTarget = CPlayer::CreateDummy( pi_fX, pi_fZ );
	if( l_pTarget )
		_GetTargetMgr()->SetTarget( l_pTarget );

	return l_pTarget;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD
CAvatar::AddAppliedSF( CSFItem * pi_pSFItem )
{
	DWORD dwResult = CCharacter::AddAppliedSF( pi_pSFItem );

	if( !_GetAppliedSFWindow() )
		return TRUE;

	CONT_SF_INFO stGuiSFInfo;
	stGuiSFInfo.bySFType		= pi_pSFItem->GetSFType();
	stGuiSFInfo.byContType		= pi_pSFItem->GetContinuanceUsingType();
	stGuiSFInfo.dwSFID			= pi_pSFItem->GetDTCode();
	stGuiSFInfo.dwSpriteID		= pi_pSFItem->GetIconID();
	stGuiSFInfo.dwKeepingTime	= pi_pSFItem->GetEffectTime();

	if( dwResult == TRUE )
	{
		_GetAppliedSFWindow()->InsertSF( &stGuiSFInfo );

		if( !_GetAppliedSFWindow()->IsShow() )
			_GetAppliedSFWindow()->Open();
	}
	else if( dwResult == TRUE + 1 )
	{
		_GetAppliedSFWindow()->UpdateSF( &stGuiSFInfo );
	}

	return TRUE;
}

DWORD
CAvatar::DeleteAppliedSF( CSFItem * pi_pSFItem )
{
	DWORD dwResult = CCharacter::DeleteAppliedSF( pi_pSFItem );

	if( dwResult == ID_INVALID )
		return FALSE;

	if( _GetAppliedSFWindow() )
	{
		_GetAppliedSFWindow()->DeleteSF( pi_pSFItem->GetContinuanceUsingType(), dwResult );
		if( ( m_stAppliedSF.m_bySFNum[0] <= 0 ) && ( m_stAppliedSF.m_bySFNum[1] <= 0 ) )
		{
			if( _GetAppliedSFWindow()->IsShow() )
				_GetAppliedSFWindow()->Close();
		}
	}

	return TRUE;
}

void
CAvatar::ClearAppliedSF( void )
{
	CCharacter::ClearAppliedSF();
}

// 캐릭터에게 걸려있는 지속 스킬/포스의 시간 업데이트 ( pi_dwTime 단위 : 초 )
void
CAvatar::UpdateAppliedSFTime( WORD pi_wServerEffectCode, DWORD pi_dwTime )
{
	if( pi_wServerEffectCode == 0xFFFF )
		return;

	CSFItem * pUpdateSF = m_stAppliedSF.UpdateRemainTime( pi_wServerEffectCode >> 12,
														  pi_wServerEffectCode & 0x0FFF, pi_dwTime ); 
	if( !pUpdateSF )
		return;

	CONT_SF_INFO stGuiSFInfo;
	stGuiSFInfo.bySFType		= pUpdateSF->GetSFType();
	stGuiSFInfo.byContType		= pUpdateSF->GetContinuanceUsingType();
	stGuiSFInfo.dwSFID			= pUpdateSF->GetDTCode();
	stGuiSFInfo.dwSpriteID		= pUpdateSF->GetIconID();
	stGuiSFInfo.dwKeepingTime	= pi_dwTime * 1000;

	_GetAppliedSFWindow()->UpdateSF( &stGuiSFInfo );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::UI_DeselectSF( void )
{
	m_clSFItem.SetEmpty();
	_GetGBM()->DeselectSF();

	SetAttackMode( CAT_NORMAL );

	_GetUIScreen()->SetMouseState( CGUIMouse::NORMAL );
}