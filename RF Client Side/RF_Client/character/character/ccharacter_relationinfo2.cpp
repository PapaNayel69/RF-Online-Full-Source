////////////////////////////////////////////////////////////////////////////////
//
// CCharacter Relation Infomation Implementation 2
//
////////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../CCamera.h"
#include "../../CLand.h"
#include "../../GameObject/Manager/CTargetMgr.h"

#include "../../UserInterface/GameWindow/GUIChatWindow.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CEnemyInfo::CEnemyInfo()
{
	m_dwEnemy[0]	= ID_INVALID;
	m_dwEnemy[1]	= ID_INVALID;
}

void
CEnemyInfo::SetEnemy( CCharacter * pi_pEnemy )
{
	if( pi_pEnemy )
	{
		m_dwEnemy[0]	= pi_pEnemy->GetCharTypeID();
		m_dwEnemy[1]	= pi_pEnemy->GetIndex();
	}
	else
	{
		m_dwEnemy[0]	= ID_INVALID;
		m_dwEnemy[1]	= ID_INVALID;
	}
}

CCharacter *
CEnemyInfo::GetEnemy( void )
{
	return _GetCharMgr()->GetCharacter( m_dwEnemy[0], m_dwEnemy[1] );
}

BOOL
CEnemyInfo::IsEqual( DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	if( ( m_dwEnemy[0] == pi_dwCharTypeID ) &&
		( m_dwEnemy[1] == pi_dwIndex ) )
		return TRUE;

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CAttackerInfo::CAttackerInfo()
{
	ZeroMemory( this, sizeof( CAttackerInfo ) );

	m_byDamageType_Dist		= ID_INVALID;
	m_byDamageType_AT		= ID_INVALID;
	m_byDamageType_CT		= FALSE;
}

void
CAttackerInfo::SetSFItem( CSFItem * pi_pSFItem )
{
	if( pi_pSFItem )
		memcpy( &m_clSFItem, pi_pSFItem, sizeof( CSFItem ) );
	else
		m_clSFItem.SetEmpty();
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CVictimInfo::CVictimInfo()
{
	ZeroMemory( this, sizeof( CVictimInfo ) );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCombatState::CCombatState()
{
	m_pOwner					= NULL;

	m_bDead						= FALSE;
	m_dwDeadTime				= 0;
	m_dwRemainHP				= ID_INVALID;

	m_byAttackType_Dist			= ADT_NEAR;
	m_byAttackType_AT			= CAT_NORMAL;
	m_bReceiveAttackResultMsg	= FALSE;
	m_bAttackError				= FALSE;

	m_pCurEnemy[0]				= NULL;
	m_pCurEnemy[1]				= NULL;
}

CCombatState::~CCombatState()
{
	Clear();
}

void
CCombatState::Clear( void )
{
	for( int i = 0; i < MAX_ENEMY_TYPE; ++i )
	{
		for( ENEMY_LIST_ITER iter = m_listEnemy[i].begin(); iter != m_listEnemy[i].end(); ++iter )
		{
			delete ( *iter );
			( *iter ) = NULL;
		}
		m_listEnemy[i].clear();
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CEnemyInfo *
CCombatState::AddEnemyInfo( BYTE pi_byEnemyType, CCharacter * pi_pEnemy, BOOL pi_bIsSettingToCurEnemy )
{
	if( !pi_pEnemy )
		return NULL;

	CEnemyInfo * l_pEnemyInfo = NULL;

	//l_pEnemyInfo = GetEnemyInfo( pi_byEnemyType, pi_pEnemy->GetCharTypeID(), pi_pEnemy->GetIndex() );
	//if( !l_pEnemyInfo )
	{
		if( pi_byEnemyType == ET_ATTACKER )
			l_pEnemyInfo = new CAttackerInfo;
		else
			l_pEnemyInfo = new CVictimInfo;
		l_pEnemyInfo->SetEnemy( pi_pEnemy );
		m_listEnemy[pi_byEnemyType].push_back( l_pEnemyInfo );
	}

	if( pi_bIsSettingToCurEnemy )
		m_pCurEnemy[pi_byEnemyType] = l_pEnemyInfo;

	return l_pEnemyInfo;
}

void
CCombatState::DeleteEnemyInfo( BYTE pi_byEnemyType, DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	for( ENEMY_LIST_ITER iter = m_listEnemy[pi_byEnemyType].begin(); iter != m_listEnemy[pi_byEnemyType].end(); ++iter )
	{
		if( ( *iter )->IsEqual( pi_dwCharTypeID, pi_dwIndex ) )
		{
			DeleteEnemyInfo_RealData( pi_byEnemyType, iter );
			break;
		}
	}
}

void
CCombatState::DeleteEnemyInfoByOrder( BYTE pi_byEnemyType, BYTE pi_byOrderNo )
{
	if( pi_byOrderNo >= m_listEnemy[pi_byEnemyType].size() )
		return;

	for( ENEMY_LIST_ITER iter = m_listEnemy[pi_byEnemyType].begin(); iter != m_listEnemy[pi_byEnemyType].end(); ++iter, --pi_byOrderNo )
	{
		if( pi_byOrderNo == 0 )
		{
			DeleteEnemyInfo_RealData( pi_byEnemyType, iter );
			break;
		}
	}
}

void
CCombatState::DeleteEnemyInfo_RealData( BYTE pi_byEnemyType, ENEMY_LIST_ITER & pio_iter )
{
	DeleteEnemyInfo_DeleteDummy( pi_byEnemyType,
								 ( *pio_iter )->GetEnemyCharTypeID(), ( *pio_iter )->GetEnemyCharIndex() );

	if( m_pCurEnemy[pi_byEnemyType] )
	{
		if( ( *pio_iter )->IsEqual( m_pCurEnemy[pi_byEnemyType]->GetEnemyCharTypeID(),
									m_pCurEnemy[pi_byEnemyType]->GetEnemyCharIndex() ) )
			m_pCurEnemy[pi_byEnemyType] = NULL;
	}

	delete ( *pio_iter );
	pio_iter = m_listEnemy[pi_byEnemyType].erase( pio_iter );
	--pio_iter;
}

void
CCombatState::DeleteEnemyInfo_DeleteDummy( BYTE pi_byEnemyType, DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	if( pi_dwCharTypeID == CTI_DUMMY )
	{
		_GetCharMgr()->DeleteDummy( pi_dwIndex );

		if( m_pOwner->IsEqualCharType( CTI_AVATAR ) )
			_GetTargetMgr()->SetTarget( NULL );
	}
}

void
CCombatState::DeleteAllEnemyInfo( BYTE pi_byEnemyType )
{
	for( ENEMY_LIST_ITER iter = m_listEnemy[pi_byEnemyType].begin();
		 iter != m_listEnemy[pi_byEnemyType].end(); ++iter )
		DeleteEnemyInfo_RealData( pi_byEnemyType, iter );

	m_pCurEnemy[pi_byEnemyType] = NULL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CCharacter *
CCombatState::GetEnemy( BYTE pi_byEnemyType, DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	for( ENEMY_LIST_ITER iter = m_listEnemy[pi_byEnemyType].begin(); iter != m_listEnemy[pi_byEnemyType].end(); ++iter )
	{
		if( ( *iter )->IsEqual( pi_dwCharTypeID, pi_dwIndex ) )
			return ( *iter )->GetEnemy();
	}

	return NULL;
}

CCharacter *
CCombatState::GetEnemyByOrder( BYTE pi_byEnemyType, BYTE pi_byOrderNo )
{
	if( pi_byOrderNo >= m_listEnemy[pi_byEnemyType].size() )
		return NULL;

	for( ENEMY_LIST_ITER iter = m_listEnemy[pi_byEnemyType].begin(); iter != m_listEnemy[pi_byEnemyType].end(); ++iter, --pi_byOrderNo )
	{
		if( pi_byOrderNo == 0 )
			return ( *iter )->GetEnemy();
	}

	return NULL;
}

CEnemyInfo *
CCombatState::GetEnemyInfo( BYTE pi_byEnemyType, DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	for( ENEMY_LIST_ITER iter = m_listEnemy[pi_byEnemyType].begin(); iter != m_listEnemy[pi_byEnemyType].end(); ++iter )
	{
		if( ( *iter )->IsEqual( pi_dwCharTypeID, pi_dwIndex ) )
			return ( *iter );
	}

	return NULL;
}

CEnemyInfo *
CCombatState::GetEnemyInfoByOrder( BYTE pi_byEnemyType, BYTE pi_byOrderNo )
{
	if( pi_byOrderNo >= m_listEnemy[pi_byEnemyType].size() )
		return NULL;

	for( ENEMY_LIST_ITER iter = m_listEnemy[pi_byEnemyType].begin(); iter != m_listEnemy[pi_byEnemyType].end(); ++iter, --pi_byOrderNo )
	{
		if( pi_byOrderNo == 0 )
			return ( *iter );
	}

	return NULL;
}

CCharacter *
CCombatState::GetCurEnemy( BYTE pi_byEnemyType )
{
	if( m_pCurEnemy[pi_byEnemyType] )
		return m_pCurEnemy[pi_byEnemyType]->GetEnemy();

	return NULL;
}

CEnemyInfo *
CCombatState::GetCurEnemyInfo( BYTE pi_byEnemyType )
{
	return m_pCurEnemy[pi_byEnemyType];
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCombatState::NotifyDeathToEnemy( BYTE pi_byEnemyType )
{
	if( !m_pOwner )
		return;

	CCharacter *	l_pEnemy;
	CEnemyInfo *	l_pMyInfo;

	for( ENEMY_LIST_ITER iter = m_listEnemy[pi_byEnemyType].begin(); iter != m_listEnemy[pi_byEnemyType].end(); ++iter )
	{
		l_pEnemy = ( *iter )->GetEnemy();
		if( !l_pEnemy )
			continue;

		l_pMyInfo = l_pEnemy->GetCombatState()->GetEnemyInfo( !pi_byEnemyType,
															  m_pOwner->GetCharTypeID(),
															  m_pOwner->GetIndex() );
		if( !l_pMyInfo )
			continue;

		if( pi_byEnemyType == ET_ATTACKER )
		{
			l_pEnemy->GetCombatState()->DeleteEnemyInfo( !pi_byEnemyType,
														 l_pMyInfo->GetEnemyCharTypeID(),
														 l_pMyInfo->GetEnemyCharIndex() );
		}
		else
		{
			static_cast< CAttackerInfo * >( l_pMyInfo )->SetDamaged( TRUE );
		}
	}
}

void
CCombatState::ClearMyself( void )
{
	for( int i = 0; i < MAX_ENEMY_TYPE; ++i )
	{
		if( m_listEnemy[i].empty() )
			continue;

		for( ENEMY_LIST_ITER iter = m_listEnemy[i].begin(); iter != m_listEnemy[i].end(); ++iter )
		{
			if( ( *iter )->IsEqual( m_pOwner->GetCharTypeID(), m_pOwner->GetIndex() ) )
				DeleteEnemyInfo_RealData( i, iter );
		}
	}
}

BOOL
CCombatState::IsPassedLimitTimeFromDeadMsg( void )
{
	static const LIMIT_DEAD_TIME = 2000;

	if( ( timeGetTime() - m_dwDeadTime ) > LIMIT_DEAD_TIME )
		return TRUE;

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCombatState::ProcessInitDamage( void )
{
	if( m_listEnemy[ET_ATTACKER].empty() )
		return;

	CAttackerInfo * l_pAttackerInfo = NULL;
	for( ENEMY_LIST_ITER iter = m_listEnemy[ET_ATTACKER].begin(); iter != m_listEnemy[ET_ATTACKER].end(); ++iter )
	{
		l_pAttackerInfo = static_cast< CAttackerInfo * >( *iter );
		if( l_pAttackerInfo->IsProcessedDamage() )
		{
			l_pAttackerInfo->SetProcessedDamage( FALSE );
			l_pAttackerInfo->SetCurDamagePoint( 0 );
			if( l_pAttackerInfo->GetTotalDamagePoint() <= 0 )
				DeleteEnemyInfo_RealData( ET_ATTACKER, iter );
		}
	}
}

void
CCombatState::ProcessDeath( void )
{
	if( GetTotalEnemyNum( ET_ATTACKER ) <= 0 )
	{
		m_pOwner->SetAction( CAI_DEATH_READY );
		return;
	}

	CAttackerInfo * l_pAttackerInfo	= NULL;

	for( ENEMY_LIST_ITER iter = m_listEnemy[ET_ATTACKER].begin(); iter != m_listEnemy[ET_ATTACKER].end(); ++iter )
	{
		l_pAttackerInfo = static_cast< CAttackerInfo * >( *iter );

		if( l_pAttackerInfo->GetDamageType_Dist() == ADT_NEAR )
			ProcessDeath_ByMeleeNearDistanceAttack( l_pAttackerInfo );
		else if( l_pAttackerInfo->GetDamageType_Dist() == ADT_FAR )
			ProcessDeath_ByMeleeFarDistanceAttack( l_pAttackerInfo );
		else
			m_pOwner->SetAction( CAI_DEATH_READY );
	}
}

void
CCombatState::ProcessDeath_ByMeleeNearDistanceAttack( CAttackerInfo * pi_pAttackerInfo )
{
	if( !pi_pAttackerInfo )
		return;

	CCharacter * l_pAttacker = pi_pAttackerInfo->GetEnemy();
	if( !l_pAttacker )
		return;

	if( !l_pAttacker->IsCombatAction() ||												// => 이미 공격 모션이 끝나있는 경우
		( l_pAttacker->IsPassedCurrentEffectFrame() && ( m_pOwner->GetHP() <= 0 ) ) ||	// => 공격 모션의 프레임이 이펙트 프레임보다 큰 경우
		IsPassedLimitTimeFromDeadMsg() )												// => CAI_INTERNALSTATE_DEATH 가 된지 일정 시간이 지났어도 죽지 않았으면 강제로 죽임
		m_pOwner->SetAction( CAI_DEATH_READY );
}

void
CCombatState::ProcessDeath_ByMeleeFarDistanceAttack( CAttackerInfo * pi_pAttackerInfo )
{
	if( !pi_pAttackerInfo )
		return;

	// 데미지를 입었다는 신호가 오면 CAI_DEATH_READY 로
	// CAI_INTERNALSTATE_DEATH 가 된지 일정 시간이 지났어도 죽지 않았으면 강제로 죽임
	if( pi_pAttackerInfo->IsDamaged() || IsPassedLimitTimeFromDeadMsg() )
		m_pOwner->SetAction( CAI_DEATH_READY );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCombatState::ProcessDamage( void )
{
	CEnemyInfo *	l_byPrevAttacker = m_pCurEnemy[ET_ATTACKER];
	CAttackerInfo *	l_pAttackerInfo;

	for( ENEMY_LIST_ITER iter = m_listEnemy[ET_ATTACKER].begin(); iter != m_listEnemy[ET_ATTACKER].end(); ++iter )
	{
		m_pCurEnemy[ET_ATTACKER] = ( *iter );
		l_pAttackerInfo = static_cast< CAttackerInfo * >( *iter );

		if( l_pAttackerInfo->IsDamaged() )
		{
			l_pAttackerInfo->SetDamaged( FALSE );
			l_pAttackerInfo->SetProcessedDamage( TRUE );

			// 스킬/포스일 경우 공격형이 아니라면 이펙트만 뿌려주면 된다.
			if( ProcessDamage_SetSupportSkillForceEffect( l_pAttackerInfo ) )
				continue;

			// 공격 성공
			if( l_pAttackerInfo->GetTotalDamagePoint() )
			{
				ProcessDamage_SetDamage( l_pAttackerInfo );
				ProcessDamage_SetHP( l_pAttackerInfo );
				ProcessDamage_SetDamageEffect( l_pAttackerInfo );

				m_pOwner->GetCharAniInfo()->SetRockAnimation( TRUE );
			}
			// 공격 실패
			else
			{
				m_pOwner->SetEffectInfo( CAI_DAMAGED_MISS );
			}
		}
	}

	m_pCurEnemy[ET_ATTACKER] = l_byPrevAttacker;
}

BOOL
CCombatState::ProcessDamage_SetSupportSkillForceEffect( CAttackerInfo * pi_pAttackerInfo )
{
	BYTE byDamageType_AT = pi_pAttackerInfo->GetDamageType_AT();

	// 스킬/포스일 경우 공격형이 아니라면 이펙트만 뿌려주면 된다.
	if( byDamageType_AT == CAT_SKILL || byDamageType_AT == CAT_CLASS_SKILL || byDamageType_AT == CAT_FORCE )
	{
		CSFItem * pSFItem = pi_pAttackerInfo->GetSFItem();
		if( !pSFItem )
			return FALSE;

		if( !pSFItem->IsAttackable() )
		{
			if( byDamageType_AT == CAT_FORCE )
				m_pOwner->SetEffectInfo( CAI_DAMAGED_BY_FORCE );
			else
				m_pOwner->SetEffectInfo( CAI_DAMAGED_BY_SKILL );

			return TRUE;
		}
	}

	return FALSE;
}

void
CCombatState::ProcessDamage_SetDamage( CAttackerInfo * pi_pAttackerInfo )
{
	if( !pi_pAttackerInfo )
		return;

	// 공격자의 애니메이션을 가져와서 이펙트 프레임수에 따라서 데미지를 깍는다.
	BYTE	l_byRemainFrame = pi_pAttackerInfo->GetEffectFrameNum();
	DWORD	l_dwTotalDamage = pi_pAttackerInfo->GetTotalDamagePoint();

	if( l_byRemainFrame > 1 )
	{
		DWORD l_dwDamage = ( ( float )l_dwTotalDamage * 0.8f ) / ( float )l_byRemainFrame;
		l_dwDamage += rand() % ( ( int )( ( float )l_dwTotalDamage * 0.2f ) + 1 );
		pi_pAttackerInfo->SetCurDamagePoint( l_dwDamage );
		pi_pAttackerInfo->SetTotalDamagePoint( l_dwTotalDamage - l_dwDamage );
	}
	else
	{
		pi_pAttackerInfo->SetCurDamagePoint( l_dwTotalDamage );
		pi_pAttackerInfo->SetTotalDamagePoint( 0 );
	}

	pi_pAttackerInfo->DecreaseEffectFrameNum();
}

void
CCombatState::ProcessDamage_SetHP( CAttackerInfo * pi_pAttackerInfo )
{
	// 아바타가 아니라면 HP를 깍는다.
	if( !m_pOwner->IsEqualCharType( CTI_AVATAR ) )
	{
		if( pi_pAttackerInfo->GetCurDamagePoint() >= m_pOwner->GetHP() )
			m_pOwner->SetHP( 0 );
		else
			m_pOwner->SetHP( m_pOwner->GetHP() - pi_pAttackerInfo->GetCurDamagePoint() );
	}
}

void
CCombatState::ProcessDamage_SetDamageEffect( CAttackerInfo * pi_pAttackerInfo )
{
	// 크리티컬 이펙트
	if( pi_pAttackerInfo->GetDamageType_CT() )
	{
		if( !m_pOwner->IsDeath() &&
			!( m_pOwner->IsEqualCharType( CTI_PLAYER ) || m_pOwner->IsEqualCharType( CTI_AVATAR ) ) )
			m_pOwner->SetAction( CAI_DAMAGED_CRITICAL );
		m_pOwner->SetEffectInfo( CAI_DAMAGED_CRITICAL );
	}

	// 실제 데미지 이펙트
	if( pi_pAttackerInfo->GetDamageType_AT() == CAT_NORMAL )
		m_pOwner->SetEffectInfo( CAI_DAMAGED_NORMAL );
	else if( pi_pAttackerInfo->GetDamageType_AT() == CAT_SKILL )
		m_pOwner->SetEffectInfo( CAI_DAMAGED_BY_SKILL );
	else if( pi_pAttackerInfo->GetDamageType_AT() == CAT_FORCE )
		m_pOwner->SetEffectInfo( CAI_DAMAGED_BY_FORCE );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCharacterAnimation::CCharacterAnimation()
{
	ZeroMemory( this, sizeof( CCharacterAnimation ) );
}

void
CCharacterAnimation::SetStopAnimation( BOOL pi_fFlag )
{
	m_bStopAnimation = pi_fFlag;

	if( pi_fFlag )
		m_dwStopAnimationTime = timeGetTime();
}

void
CCharacterAnimation::SetRockAnimation( BOOL pi_fFlag )
{
	if( m_pOwner->IsEqualCharType( CTI_AVATAR ) || m_pOwner->IsEqualCharType( CTI_DUMMY ) )
		return;

	if( pi_fFlag )
	{
		if( !m_bPushOut && !m_bIsRock )
		{
			m_dwStartRockTime = timeGetTime();
			memcpy( m_vecStartRockPos, m_pOwner->GetPosition(), sizeof( float ) * 3 );
			m_bIsRock = TRUE;
			return;
		}
	}

	m_bIsRock = FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CCharacterAnimation::PositionSettingThatIsPushOut( float pi_fPushOutAngle )
{
	if( m_fPushOut_Distance <= 0 )
		return FALSE;

	m_bPushOut			= TRUE;
	m_fPushOut_Angle	= pi_fPushOutAngle;

	m_vecPushOutPos[0]	= m_pOwner->GetPosX() + ( ( float )sin( AngleToPi( pi_fPushOutAngle ) ) * m_fPushOut_Distance );
	m_vecPushOutPos[1]	= m_pOwner->GetPosY();
	m_vecPushOutPos[2]	= m_pOwner->GetPosZ() + ( ( float )cos( AngleToPi( pi_fPushOutAngle ) ) * m_fPushOut_Distance );

	return TRUE;
}

void
CCharacterAnimation::PositionCheckThatIsPushOut( void )
{
	_GetLand()->GetLevel()->mBsp->CanYouGoThere( m_pOwner->GetPosition(),
												 m_vecPushOutPos, &m_vecPushOutPos );

	float l_fDistBetweenWallAndTarget = GetDist( m_pOwner->GetPosition(), m_vecPushOutPos );
	if( m_fPushOut_Distance > l_fDistBetweenWallAndTarget )
		m_fPushOut_Distance = l_fDistBetweenWallAndTarget;

	if( m_fPushOut_Distance < 0.5f )
		m_bPushOut = FALSE;
	else
		++m_byPushOutCount;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCharacterAnimation::ProcessStopAnimation( void )
{
	if( m_bStopAnimation )
	{
		if( m_pOwner->IsEqualCharType( CTI_AVATAR ) || m_pOwner->IsEqualCharType( CTI_PLAYER ) )
			ProcessStopAnimation_Player();
		else if( m_pOwner->IsEqualCharType( CTI_MONSTER ) )
			ProcessStopAnimation_Monster();
	}
}

void
CCharacterAnimation::ProcessStopAnimation_Player( void )
{
	DWORD l_dwStopAnimationTime = ProcessStopAnimation_Player_GetStopAnimationTime( m_pOwner->GetMaxEffectFrameNum() );

	if( ( timeGetTime() - m_dwStopAnimationTime ) > l_dwStopAnimationTime )
	{
		m_bStopAnimation	= FALSE;

		m_pOwner->Enable( ROSF_MOVABLE );
		m_pOwner->Disable( ROSF_ANIMATION_STOP );

		m_pOwner->SetAnimationSpeedByAction();
	}
	else
	{
		if( ( GetFPS() <= 40 ) || ( m_pOwner->GetMaxEffectFrameNum() > 1 ) )
		{
			m_pOwner->Disable( ROSF_MOVABLE );
			m_pOwner->Enable( ROSF_ANIMATION_STOP );
		}
		else
		{
			m_pOwner->Enable( ROSF_MOVABLE );
			m_pOwner->Disable( ROSF_ANIMATION_STOP );

			m_pOwner->SetAnimationSpeed( m_pOwner->GetAnimationSpeed() * 0.5f );
			if( m_pOwner->GetAnimationSpeed() < 0.2f )
				m_pOwner->SetAnimationSpeed( 0.0f );
		}
	}
}

DWORD
CCharacterAnimation::ProcessStopAnimation_Player_GetStopAnimationTime( int pi_nMaxAniFrameNum )
{
	static const DWORD STOP_ANIMATION_TIME_FOR_ONE_FRAME	= 230;
	static const DWORD STOP_ANIMATION_TIME_FOR_MANY_FRAME	= 130;

	DWORD l_dwStopAnimationTime;
	if( pi_nMaxAniFrameNum > 1 )
		l_dwStopAnimationTime = STOP_ANIMATION_TIME_FOR_MANY_FRAME;
	else
		l_dwStopAnimationTime = STOP_ANIMATION_TIME_FOR_ONE_FRAME;

	return l_dwStopAnimationTime;
}

void
CCharacterAnimation::ProcessStopAnimation_Monster( void )
{
	static const DWORD STOP_ANIMATION_TIME_FOR_MONSTER	= 100;

	if( ( timeGetTime() - m_dwStopAnimationTime ) > STOP_ANIMATION_TIME_FOR_MONSTER )
	{
		m_bStopAnimation	= FALSE;
		m_pOwner->Enable( ROSF_MOVABLE );
		m_pOwner->Disable( ROSF_ANIMATION_STOP );

		m_pOwner->SetAnimationSpeedByAction();
	}
	else
	{
		m_pOwner->Disable( ROSF_MOVABLE );
		m_pOwner->Enable( ROSF_ANIMATION_STOP );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CCharacterAnimation::ProcessPushOut( Vector3f pio_vecCharPos )
{
	return FALSE;

	/*
	if( m_bPushOut )
	{
		float l_fSpeed = R3GetLoopTime() * m_fPushOut_Speed;

		// 좌표 이동
		Vector3f l_vecTargetPos;
		l_vecTargetPos[0] = pio_vecCharPos[0] + ( ( float )sin( AngleToPi( m_fPushOut_Angle ) ) * l_fSpeed );
		l_vecTargetPos[1] = pio_vecCharPos[1];
		l_vecTargetPos[2] = pio_vecCharPos[2] + ( ( float )cos( AngleToPi( m_fPushOut_Angle ) ) * l_fSpeed );

		// 타겟 위치에 도착 했는지를 검사
		m_fPushOut_Distance -= GetDist( pio_vecCharPos, l_vecTargetPos );
		if( m_fPushOut_Distance <= 0.0f )
		{
			memcpy( l_vecTargetPos, m_vecPushOutPos, sizeof( float ) * 3 );
			m_bPushOut = FALSE;
		}
		else
		{
			pio_vecCharPos[0] = l_vecTargetPos[0];
			pio_vecCharPos[2] = l_vecTargetPos[2];
		}

		return TRUE;
	}

	return FALSE;*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CCharacterAnimation::ProcessRock( Vector3f pio_vecCharPos )
{
	if( m_bIsRock )
	{
		static const DWORD CHAR_ROCK_TIME = 150;

		if( ( timeGetTime() - m_dwStartRockTime ) > CHAR_ROCK_TIME )
		{
			m_bIsRock = FALSE;

			if( !( m_pOwner->GetActionByCategory() == CAI_DEATH ||
				   m_pOwner->GetAction() == CAI_INTERNALSTATE_DEATH ) )
			{
				memcpy( pio_vecCharPos, m_vecStartRockPos, sizeof( float ) * 3 );
				return TRUE;
			}
		}

		float l_fDist_m = R3GetToggle30fps();
		pio_vecCharPos[0] += cos( AngleToPi( _GetCamera()->GetPitch() ) ) * l_fDist_m;
		pio_vecCharPos[2] += sin( AngleToPi( _GetCamera()->GetPitch() ) ) * l_fDist_m;

		return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CHARACTER_ETC_INFO::Init( void )
{
	ZeroMemory( this, sizeof( CHARACTER_ETC_INFO ) );

	m_dwReceiveMsgTime	= ID_INVALID;

	m_byChatMsgType		= CGUIChatWindow::FS_NORMAL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CHARACTER_ETC_INFO::SetInfo( int pi_nNo, char * pi_pInfo )
{
#ifdef _DEVELOPER_
	sprintf( m_pInfoString[pi_nNo], "[%d] : %s", pi_nNo, pi_pInfo );
	m_pInfoTime[pi_nNo] = timeGetTime();

	BYTE l_byCurStrLen;
	m_byMaxInfoStringWidth = 0;
	for( int i = 0; i < 10; ++i )
	{
		l_byCurStrLen = strlen( m_pInfoString[i] );
		if( l_byCurStrLen > m_byMaxInfoStringWidth )
			m_byMaxInfoStringWidth = l_byCurStrLen;
	}
#endif
}

void
CHARACTER_ETC_INFO::SetReceiveNetMsg( char * pi_pMsg )
{
#ifdef _DEVELOPER_
	if( !m_pCharacter || !pi_pMsg )
		return;

	SetInfo( CIST_NETWORK, pi_pMsg );
#endif

	if( m_pCharacter )
		m_pCharacter->SetReceiveFixPositionMsg( TRUE );

	m_dwReceiveMsgTime = timeGetTime();
}

void
CHARACTER_ETC_INFO::RenderInfo( void )
{
#ifdef _DEVELOPER_
	if( !m_pCharacter )
		return;

	// 아바타와 아바타의 타겟만 렌더링
	if( m_pCharacter->GetCharTypeID() != CTI_AVATAR )
	{
		CCharacter * l_pTarget = _GetTargetMgr()->GetTarget();
		if( l_pTarget )
		{
			if( !m_pCharacter->IsEqual( l_pTarget ) )
				return;
		}
		else
		{
			return;
		}
	}

	if( m_pCharacter->IsEnable( ROSF_RENDER_INFO ) )
	{
		char l_pBuf[64];

		sprintf( l_pBuf, "C:%d - S:%d - %s", m_pCharacter->GetIndex(), m_pCharacter->GetServerIndex(),
											 m_pCharacter->GetName() );
		SetInfo( CIST_NAME, l_pBuf );

		//----------------------------------------------------------------------

		CVictimInfo *	l_pVictim	= m_pCharacter->GetCurVictimInfo();
		CAttackerInfo *	l_pAttacker	= m_pCharacter->GetCurAttackerInfo();

		if( l_pVictim && l_pAttacker )
		{
			sprintf( l_pBuf, "AP - %d, DP - %d", l_pVictim->GetAttackPoint(), l_pAttacker->GetTotalDamagePoint() );
			SetInfo( CIST_APDP, l_pBuf );
		}

		sprintf( l_pBuf, "HP - %d / %d", m_pCharacter->GetHP(), m_pCharacter->GetMaxHP() );
		SetInfo( CIST_HPFP, l_pBuf );

		sprintf( l_pBuf, "Attacker - %d / Victim - %d",
				 m_pCharacter->GetCombatState()->GetTotalEnemyNum( ET_ATTACKER ),
				 m_pCharacter->GetCombatState()->GetTotalEnemyNum( ET_VICTIM ) );
		SetInfo( CIST_ATTACKER_VICTIM, l_pBuf );

		//----------------------------------------------------------------------

		if( timeGetTime() - m_dwReceiveMsgTime > 5000 )
			SetInfo( CIST_NETWORK, "No Message" );

		//----------------------------------------------------------------------

		Vector4f l_vecTextPos;
		if( m_pCharacter->GetCharTypeID() == CTI_AVATAR )
		{
			//l_vecTextPos[0] = _GetClientRect()->right - ( m_byMaxInfoStringWidth * FONT_WIDTH ) - 20;
			l_vecTextPos[0] = GetR3State()->mScreenXsize - ( m_byMaxInfoStringWidth * FONT_WIDTH ) - 20;
			l_vecTextPos[1] = 300;
		}
		else
		{
			//l_vecTextPos[0] = _GetClientRect()->right - ( m_byMaxInfoStringWidth * FONT_WIDTH ) - 20;
			l_vecTextPos[0] = GetR3State()->mScreenXsize - ( m_byMaxInfoStringWidth * FONT_WIDTH ) - 20;
			l_vecTextPos[1] = 500;
		}

		//----------------------------------------------------------------------

		int i, l_nCnt;
		for( i = 0, l_nCnt = 0; i < 10; ++i )
		{
			if( m_pInfoString[i][0] != NULL )
				++l_nCnt;
		}
		Draw2DRectangle( l_vecTextPos[0] - 10,
						 l_vecTextPos[1] - ( l_nCnt * FONT_HEIGHT ) + FONT_HEIGHT - 5,
						 l_vecTextPos[0] + ( m_byMaxInfoStringWidth * FONT_WIDTH ) + 10,
						 l_vecTextPos[1] + FONT_HEIGHT,
						 0x70FFFFFF );

		//----------------------------------------------------------------------

		static DWORD STRING_COLOR[10] = { 0xFFA0FFA0, 0xFFA0FFA0, 0xFFFFA0A0, 0xFFB0B0B0, 0xFFA0A0FF,
										  0xFFA0FFA0, 0xFFA0FFA0, 0xFFA0FFA0, 0xFFA0FFA0, 0xFFA0FFA0 };

		for( i = 0, l_nCnt = 0; i < 10; ++i )
		{
			if( m_pInfoString[i][0] != NULL )
			{
				DrawR3Hangul( l_vecTextPos[0], l_vecTextPos[1] - ( l_nCnt * FONT_HEIGHT ),
							  m_pInfoString[i], STRING_COLOR[i], R3_HAN_OUTLINE );
				++l_nCnt;
			}
		}

		//----------------------------------------------------------------------

		CAnimation * pAni = m_pCharacter->GetAnimation();
		if( pAni )
		{
			sprintf( l_pBuf, "%d / %d ( %X )", pAni->GetCurrentEffectFrameNo(), pAni->GetMaxEffectFrameNum(),
											   pAni->GetAnimationID() );
			SetInfo( CIST_EFFECT_FRAME, l_pBuf );
		}
	}
#endif
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CHARACTER_ETC_INFO::SetChatMsg( char * pi_pMsg, BYTE pi_byChatMsgType )
{
	for( int i = 0; ( i < strlen( pi_pMsg ) ) && ( i < 256 ); ++i )
	{
		if( pi_pMsg[i] == ':' )
		{
			_strcpy( m_pChatMsg, &pi_pMsg[i+1], 256 );
			break;
		}
	}

	m_dwReceiveChatMsgTime = timeGetTime();
	m_byChatMsgType = pi_byChatMsgType;
}

void
CHARACTER_ETC_INFO::RenderChatMsg( void )
{
	if( !m_pCharacter )
		return;

	//if( !_GetChatWindow()->IsShow() )
	{
		if( m_pChatMsg[0] != NULL )
		{
			Vector4f l_vecTextPos;
			_GetRenderNamePos( m_pCharacter, l_vecTextPos, strlen( m_pChatMsg ) );

			DWORD l_dwTextColor, l_dwStyle;
			switch( m_byChatMsgType )
			{
				case CGUIChatWindow::FS_AVATAR :
					l_dwTextColor	= 0xFFA0FFA0;
					l_dwStyle		= R3_HAN_OUTLINE;
					break;

				case CGUIChatWindow::FS_SYSTEM :
					l_dwTextColor	= 0xFFFF80A0;
					l_dwStyle		= R3_HAN_OUTLINE;
					break;

				case CGUIChatWindow::FS_PARTY_NORMAL :
					l_dwTextColor	= 0xFFC0E0E0;
					l_dwStyle		= R3_HAN_OUTLINE;
					break;

				case CGUIChatWindow::FS_PARTY_AVATAR :
					l_dwTextColor	= 0xFFC0E0E0;
					l_dwStyle		= R3_HAN_OUTLINE;
					break;

				case CGUIChatWindow::FS_WHISPER_NORMAL :
					l_dwTextColor	= 0xFFC0C0C0;
					l_dwStyle		= R3_HAN_OUTLINE;
					break;

				case CGUIChatWindow::FS_WHISPER_AVATAR :
					l_dwTextColor	= 0xFFC0C0C0;
					l_dwStyle		= R3_HAN_OUTLINE;
					break;

				case CGUIChatWindow::FS_NOTIFY :
					l_dwTextColor	= 0xFFFFD0A0;
					l_dwStyle		= R3_HAN_OUTLINE;
					break;

				case CGUIChatWindow::FS_NORMAL :
				default :
					l_dwTextColor	= 0xFFFFFFFF;
					l_dwStyle		= R3_HAN_OUTLINE;
					break;
			}

			l_vecTextPos[1] -= 20;
			DrawR3Hangul3D( l_vecTextPos, m_pChatMsg, l_dwTextColor, l_dwStyle );

			if( timeGetTime() - m_dwReceiveChatMsgTime >= 3000 )
				ZeroMemory( m_pChatMsg, 256 );
		}
	}
}