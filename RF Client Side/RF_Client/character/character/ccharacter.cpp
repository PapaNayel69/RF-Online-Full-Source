////////////////////////////////////////////////////////////////////////////////
//
// CCharacter Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"

#include "../../UserInterface/GameWindow/SkillForce/GUISkillWindow.h"
#include "../../UserInterface/GameWindow/SkillForce/GUIForceWindow.h"
#include "../../UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"

#include "../../GameObject/Board/General/CPartyBoard.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCharacter::CCharacter()
{
	m_byObjectTypeID		= OTI_CHAR;

	m_dwCharTypeID			= ID_INVALID;
	m_dwDTIndex				= ID_INVALID;

	m_dwActionID			= ID_INVALID;
	m_dwPrevActionID		= ID_INVALID;
	m_dwActionTime			= 0;
	m_dwDeadTime			= 0;
	m_dwIdleTime			= IDLE_TIME1;

	//--------------------------------------------------------------------------

	m_byCombatMode			= CSI_PEACE;
	m_byPrevCombatMode		= m_byCombatMode;

	m_byWalkMode			= CMM_MOVE_WALK;
	m_byPrevWalkMode		= m_byWalkMode;
	m_fSpeed[0]				= CHAR_SPEED_DEFAULT;
	m_fSpeed[1]				= CHAR_SPEED_DEFAULT;
	m_fMagnificationOfSpeed	= 1.0f;

	//--------------------------------------------------------------------------

	m_dwMaxHP				= 0;
	m_dwHP					= 0;
	m_dwMaxFP				= 0;
	m_dwFP					= 0;

	//--------------------------------------------------------------------------

	m_dwCurFrameMoveTime			= 0;
	m_dwPrevFrameMoveTime			= 0;
	m_bIsPassedFrameMoveLimitTime	= FALSE;

	//--------------------------------------------------------------------------

	m_fDistanceBetweenAvatar		= 0.0f;
	m_byAnimationSkipTotalFrame		= 0;
	m_byAnimationSkipCurFrame		= 0;
	m_dwAnimationSkipTime			= 0;

	//--------------------------------------------------------------------------

	m_stAppliedSF.m_pCharCondInfo	= &m_stConditionInfo;
	m_stConditionInfo.m_pCharacter	= this;
	m_stETCInfo.m_pCharacter		= this;
	m_pCombatState					= new CCombatState;
	m_pCombatState->SetOwner( this );
	m_pCharAniInfo					= new CCharAniInfo;
	m_pCharAniInfo->SetOwner( this );

	//--------------------------------------------------------------------------

	m_bIsFirstMoving		= TRUE;
	ZeroMemory( m_vecNextPath, sizeof( Vector3f ) );

	//--------------------------------------------------------------------------

	Enable( ROSF_CHECK_Y_POS | ROSF_MOVABLE | ROSF_ANIMATION_INTERPOLATION );

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//

	// by J.U.I
	m_fDamageTimeForShake	= 0;
	m_f2ndDensity			= 0;			//2nd 라이팅 관련.
	m_dw2ndColor			= 0xffffffff;
	m_bCheckExistEF			= FALSE;

	// by MAYUN
	m_bBlend				= FALSE;
	for (int i =0;i<MAX_PARTS_RESULT;i++)
	{
		m_pMatResMat[i][0]	= NULL;
		m_pMatResMat[i][1]	= NULL;
		m_dwMaxResult[i]	= 0;			//초기값 1
	}

	if( !m_pAniIPInfo )
		m_pAniIPInfo = new ANI_IP_INFO;
	ZeroMemory( m_pAniIPInfo, sizeof( ANI_IP_INFO ) );
}

CCharacter::~CCharacter()
{
	_DELETE( m_pCombatState );
	_DELETE( m_pCharAniInfo );

	m_stAppliedSF.m_pCharCondInfo	= NULL;
	m_stConditionInfo.m_pCharacter	= NULL;
	m_stETCInfo.m_pCharacter		= NULL;

	//----------------------------------------------------------------------	
	// MAYUN
	// 버텍스 블렌딩
	for (int i =0;i<MAX_PARTS_RESULT;i++)
	{
		SAFE_DELETE_ARRAY( m_pMatResMat[i][0] );
		SAFE_DELETE_ARRAY( m_pMatResMat[i][1] );
	}
	
	// 애니메이션 인터폴레이션
	if( m_pAniIPInfo )
	{
		delete m_pAniIPInfo;
		m_pAniIPInfo = NULL;
	}
	// MAYUN
	//----------------------------------------------------------------------
}

CCharacter *
CCharacter::New( void )
{
	return ( new CCharacter );
}

CCharacter *
CCharacter::Clone( void )
{
	return ( new CCharacter( *this ) );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 매 프레임마다 초기에 설정해 주어야할 데이터들
void
CCharacter::Init_EveryFrame( void )
{
	CResObject::Init_EveryFrame();

	m_pCombatState->ProcessInitDamage();

	m_pCharAniInfo->ProcessStopAnimation();
}

//------------------------------------------------------------------------------
// < Function >
//   o 캐릭터를 화면에 렌더링한다.
// < Parameter >
//   o None
// < Return Value >
//   o BOOL - 현재 반환값은 의미를 가지지 못한다.
//------------------------------------------------------------------------------
BOOL
CCharacter::Render( void )
{
	if( !CResObject::Render() )
		return FALSE;

	if( IsEnable( ROSF_RENDER_MESH ) && IsDisable( ROSF_CLIPPED ) )
		SetLightColorFromMap();

	return TRUE;
}

BOOL
CCharacter::RenderName( void )
{
	if( IsEnable( ROSF_RENDER_MESH ) && IsDisable( ROSF_CLIPPED ) )
		return CResObject::RenderName();

	return TRUE;
}

void
CCharacter::RenderShadow( void )
{
}

void
CCharacter::CreateShadow( void )
{
}

void
CCharacter::RenderText( void )
{
	RenderName();

	m_stETCInfo.RenderChatMsg();
}

void
CCharacter::RenderInformation( void )
{
#ifdef _DEVELOPER_
	m_stETCInfo.RenderInfo();
#endif
}

//------------------------------------------------------------------------------
// < Function >
//   o 오브젝트의 상태에 따라서 메시 혹은 본 데이터를 애니메이션 시킨다.
// < Parameter >
//   o None
// < Return Value >
//   o None
//------------------------------------------------------------------------------
BOOL
CCharacter::Animation( DWORD pi_dwAniFrame )
{
	if( !IsEnableAnimation( pi_dwAniFrame ) )
		return FALSE;

	return CResObject::Animation( pi_dwAniFrame );
}

BOOL
CCharacter::IsEnableAnimation( DWORD pi_dwAniFrame )
{
	if( IsDisable( ROSF_ANIMATION ) )
	{
		return FALSE;
	}

	if( IsEnable( ROSF_CLIPPED ) )
	{
		CalculateCurrentAnimationFrame( pi_dwAniFrame );
		return FALSE;
	}

	/*
	if( !IsEqualCharType( CTI_AVATAR ) )
	{
		DWORD l_dwSkipTime	= GetTotalFrameSkipTime();
		DWORD l_dwCurTime	= timeGetTime();
		if( l_dwCurTime - m_dwAnimationSkipTime > l_dwSkipTime )
		{
			Disable( ROSF_ANIMATION_STOP_ENGINE );
			m_dwAnimationSkipTime = l_dwCurTime;
		}
		else
		{
			Enable( ROSF_ANIMATION_STOP_ENGINE );
		}
	}*/

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CCharacter::FrameMove( void )
{
	if( IsDisable( ROSF_MOVABLE ) )
		return;

	Vector3fCopy( m_vecPos, m_vecPrevPos );		// by J.U.I -이전 좌표 보관

	CResObject::FrameMove();

	FrameMove_InitFrameMoveTime();

	if( IsEqualActionCategory( CAI_MOVE ) )
	{
		FrameMove_Move();
	}
	else if( IsEqualActionCategory( CAI_COMBAT ) )
	{
		FrameMove_Attack();
	}
	else if( IsEqualActionCategory( CAI_SKILL ) )
	{
		FrameMove_Skill();
	}
	else if( IsEqualActionCategory( CAI_FORCE ) )
	{
		FrameMove_Force();
	}
	else if( IsEqualActionCategory( CAI_DAMAGED ) )
	{
		if( IsEqualAction( CAI_DAMAGED_CRITICAL ) )
		{
			if( !m_Animation.IsAnimating() )
				SetAction( CAI_MOVE_STOP );
		}
	}
	else if( IsEqualActionCategory( CAI_DEATH ) )
	{
		if( IsEqualAction( CAI_DEATH_READY ) )
		{
			if( !m_Animation.IsAnimating() )
				SetAction( CAI_DEATH_DIE );
		}
	}
	else if( IsEqualActionCategory( CAI_CREATE ) )
	{
		if( IsEqualAction( CAI_CREATE_CREATE ) )
			SetAction( CAI_MOVE_STOP );
	}
	else if( IsEqualActionCategory( CAI_INTERNALSTATE ) )
	{
		FrameMove_InternalState();
	}

	FrameMove_EveryFrame();
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CCharacter::FrameMove_InitFrameMoveTime( void )
{
	static const DWORD FRAME_MOVE_LIMIT_TIME = 200;

	m_dwCurFrameMoveTime			= timeGetTime();
	m_bIsPassedFrameMoveLimitTime	= FALSE;
	if( m_dwCurFrameMoveTime - m_dwPrevFrameMoveTime >= FRAME_MOVE_LIMIT_TIME )
	{
		m_bIsPassedFrameMoveLimitTime	= TRUE;
		m_dwPrevFrameMoveTime			= m_dwCurFrameMoveTime;
	}
}

void
CCharacter::FrameMove_Move( void )
{
	if( IsEqualAction( CAI_MOVE_STOP ) )
		FrameMove_Move_Stop();
	else if( IsEqualAction( CAI_MOVE_MOVING ) )
		FrameMove_Move_Moving();
	else if( IsEqualAction( CAI_MOVE_IDLE ) )
		FrameMove_Move_Idle();
}

void
CCharacter::FrameMove_Move_Stop( void )
{
	/*
	// 캐릭터가 현재 멈춰있는 상태인데, 현재 위치와 타겟 포지션의 차이가 크다면
	// 이동중 다른 동작을 하는 메세지를 받은 상태이기 때문이다.
	// 목표 지점까지 이동시켜 준다. ( 현재는 2.0 )
	Vector3f l_vecTarget;
	GetLastPath( l_vecTarget );
	if( GetDist( m_vecPos, l_vecTarget ) >= 2.0f )
		SetAction( CAI_MOVE_MOVING );
	*/
}

#include "../../GameObject/Manager/CTargetMgr.h"
void
CCharacter::FrameMove_Move_Moving( void )
{
	GetCurPath( m_vecNextPath );	// 현재 패쓰의 값의 중간에 변경될 수도 있으므로 매 프레임마다 현재 패쓰의 값을 가져와야 된다.

	if( m_bIsFirstMoving )
	{
		SetRotY( m_vecPos, m_vecNextPath );
		m_bIsFirstMoving = FALSE;
	}

	if( PositionMove( m_vecNextPath ) == -1 )
	{
		BOOL l_bContinue = TRUE;

		m_vecPos[0] = m_vecNextPath[0];
		m_vecPos[2] = m_vecNextPath[2];
		if( l_bContinue &= _GetLevel()->GetNextYpos( m_vecPos, &m_vecNextPath[1] ) )
			m_vecPos[1] = m_vecNextPath[1];

		if( l_bContinue &= GetNextPath( m_vecNextPath ) )
		{
			CheckLongPoint( m_vecPos, m_vecNextPath );
			SetRotY( m_vecPos, m_vecNextPath );
		}

		if( !l_bContinue )
		{
			if( IsEqualCharType( CTI_AVATAR ) )	_GetNetworkMgr()->PositionMsg_PlayerStop_clzo();
			else				SetAction( CAI_MOVE_STOP );
		}
	}

	CheckYPos();
}

void
CCharacter::FrameMove_Move_Idle( void )
{
	if( !m_Animation.IsAnimating() )
	{
		SetAction( CAI_MOVE_STOP );
		m_dwIdleTime = IDLE_TIME2;
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CCharacter::FrameMove_Attack( void )
{
	if( IsEqualAction( CAI_COMBAT_ATTACK ) )
	{
		if( IsEffectFrame() )
			FrameMove_Attack_SetAttacker( CAI_COMBAT_ATTACK, FALSE, TRUE );

		if( !m_Animation.IsAnimating() )
			SetAction( CAI_MOVE_STOP );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 공격자의 정보 설정
// pi_dwEffectCode - 공격 이펙트 코드
// pi_bIsPushOutForSkill - 스킬 사용자 공격자가 밀고 나갈 것인가?
// pi_bIsPushedOut - 피해자를 밀 것인가?
void
CCharacter::FrameMove_Attack_SetAttacker( DWORD pi_dwEffectCode,
										  BOOL pi_bIsPushOutForSkill, BOOL pi_bIsPushedOut )
{
	if( !FrameMove_Attack_SetEffect( pi_dwEffectCode, pi_bIsPushOutForSkill ) )
		return;

	if( m_pCombatState->GetAttackType_Dist() == ADT_NEAR )
		FrameMove_Attack_ProcessNearTypeAttack( pi_dwEffectCode, pi_bIsPushOutForSkill, pi_bIsPushedOut );
	else
		FrameMove_Attack_SetEffectFrame();
}

BOOL
CCharacter::FrameMove_Attack_SetEffect( DWORD pi_dwEffectCode, BOOL pi_bIsPushOutForSkill )
{
	if( !m_Animation.IsInvokedEffect() )
	{
		m_Animation.InvokedEffect( TRUE );

		// attack error - result from server
		if( m_pCombatState->IsAttackError() )
		{
			SetEffectInfo( CAI_COMBAT_ATTACK_ERROR );
			m_pCombatState->IsAttackError();
			return FALSE;
		}

		// set effect
		SetEffectInfo( pi_dwEffectCode );

		if( pi_bIsPushOutForSkill )
			SetPushOut( CIPO_PUSH_OUT_FOR_SKILL, this );
	}

	return TRUE;
}

void
CCharacter::FrameMove_Attack_ProcessNearTypeAttack( DWORD pi_dwEffectCode,
													BOOL pi_bIsPushOutForSkill, BOOL pi_bIsPushedOut )
{
	if( !m_pCombatState->IsReceiveAttackResultMsg() )
		return;

	BOOL l_bIsLastEffectFrame = FrameMove_Attack_SetEffectFrame();

	FrameMove_Attack_ProcessStopAnimation();

	CCharacter * l_pTarget;
	for( int i = 0; i < m_pCombatState->GetTotalEnemyNum( ET_VICTIM ); ++i )
	{
		l_pTarget = m_pCombatState->GetEnemyByOrder( ET_VICTIM, i );
		if( !l_pTarget )
			continue;

		FrameMove_Attack_SetVictim( l_pTarget, l_bIsLastEffectFrame,
									pi_bIsPushOutForSkill, pi_bIsPushedOut );
	}

	if( l_bIsLastEffectFrame )
		FrameMove_Attack_ProcessLastEffectFrame();
}

BOOL
CCharacter::FrameMove_Attack_SetEffectFrame( void )
{
	m_Animation.InvokedEffect( FALSE );
	m_Animation.IncreaseEffectFrameNum();

	if( m_Animation.IsPassedLastEffectFrame( m_dwBoneFrame ) )
		return TRUE;

	return FALSE;
}

void
CCharacter::FrameMove_Attack_ProcessStopAnimation( void )
{
	if( !IsEqualCharType( CTI_AVATAR ) && !IsEqualCharType( CTI_PLAYER ) )
		return;

	if( static_cast< CPlayer * >( this )->IsEqualUnitMountState( UMS_MOUNT ) )
		return;

	CVictimInfo * l_pVictimInfo = GetCurVictimInfo();
	if( !l_pVictimInfo )
		return;

	if( l_pVictimInfo->GetEnemyCharTypeID() != CTI_DUMMY )
		m_pCharAniInfo->SetStopAnimation( TRUE );
}

void
CCharacter::FrameMove_Attack_ProcessLastEffectFrame( void )
{
	m_pCombatState->SetReceiveAttackResultMsg( FALSE );
	m_pCombatState->DeleteAllEnemyInfo( ET_VICTIM );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 피해자의 정보 설정
void
CCharacter::FrameMove_Attack_SetVictim( CCharacter * pi_pTarget, BOOL pi_bIsLastEffectFrame,
										BOOL pi_bIsPushOutForSkill, BOOL pi_bIsPushOut )
{
	if( !pi_pTarget )
		return;

	CAttackerInfo *	l_pAttackerInfo = static_cast< CAttackerInfo * >( 
									  pi_pTarget->GetCombatState()->GetEnemyInfo(
									  ET_ATTACKER, m_dwCharTypeID, m_dwIndex ) );
	if( l_pAttackerInfo )
	{
		// 타겟에 대한 설정
		l_pAttackerInfo->SetDamaged( TRUE );

		// 공격자가 피해자를 미는 스킬을 사용했다면...
		if( pi_bIsPushOutForSkill && pi_bIsPushOut )
		{
			SetPushOut( CIPO_PUSHED_OUT_FOR_SKILL, pi_pTarget );
		}
		// 공격자의 이펙트 프레임이 마지막이고, 타겟이 죽는 상태라면 타겟을 뒤로 약간 민다.
		else if( pi_bIsPushOut && pi_bIsLastEffectFrame &&
				 ( pi_pTarget->GetCombatState()->GetRemainHP() <= 0 ) )
		{
			if( !IsEqual( pi_pTarget ) )
				SetPushOut( CIPO_PUSHED_OUT_FOR_DEATH, pi_pTarget );
		}
		// 적 캐릭터와 겹쳐있을 경우 적 캐릭터를 밀어낸다. ( 아바타에게만 해당됨 )
		else if( IsEqualCharType( CTI_AVATAR ) && !pi_pTarget->IsEqualCharType( CTI_AVATAR ) )
		{
			if( !pi_pTarget->IsEqualAction( CAI_MOVE_MOVING ) )
				SetPushOut( CIPO_PUSHED_OUT_FOR_DAMAGED, pi_pTarget );
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCharacter::SetPushOut( int pi_nPushOutMode, CCharacter * pi_pTarget )
{
	if( !pi_pTarget )
		return;
	if( pi_pTarget->IsEqualCharType( CTI_AVATAR ) || pi_pTarget->IsEqualCharType( CTI_DUMMY ) )
		return;

	CCharAniInfo * l_pTargetAniInfo = pi_pTarget->GetCharAniInfo();

	static const MAX_PUSH_OUT_COUNT = 2;
	if( l_pTargetAniInfo->GetPushOutCount() >= MAX_PUSH_OUT_COUNT )
		return;

	if( !SetPushOut_SetSpeedAndDistance( pi_nPushOutMode, pi_pTarget ) )
		return;

	if( !l_pTargetAniInfo->PositionSettingThatIsPushOut( m_vecRot[1] ) )
		return;

	l_pTargetAniInfo->PositionCheckThatIsPushOut();

	pi_pTarget->SetRotY( pi_pTarget->GetPosition(), m_vecPos );
}

BOOL
CCharacter::SetPushOut_SetSpeedAndDistance( int pi_nPushOutMode, CCharacter * pi_pTarget )
{
	// 캐릭터가 스킬 공격시, 앞으로 전진
	if( pi_nPushOutMode == CIPO_PUSH_OUT_FOR_SKILL )
	{
		if( !SetPushOut_PushOutForSkill( pi_pTarget ) )
			return FALSE;
	}
	// 캐릭터가 죽을 경우 뒤로 밀려남
	else if( pi_nPushOutMode == CIPO_PUSHED_OUT_FOR_DEATH )
	{
		SetPushOut_PushedOutForDeath( pi_pTarget );
	}
	// 캐릭터가 일반 공격을 받아서 뒤로 밀려남
	else if( pi_nPushOutMode == CIPO_PUSHED_OUT_FOR_DAMAGED )
	{
		if( !SetPushOut_PushedOutForDamaged( pi_pTarget ) )
			return FALSE;
	}
	// 캐릭터가 스킬 공격을 받아서 뒤로 밀려남
	else if( pi_nPushOutMode == CIPO_PUSHED_OUT_FOR_SKILL )
	{
		if( !SetPushOut_PushedOutForSkill( pi_pTarget ) )
			return FALSE;
	}

	return TRUE;
}

BOOL
CCharacter::SetPushOut_PushOutForSkill( CCharacter * pi_pTarget )
{
	SKILL_DATA * pSkillData = reinterpret_cast< SKILL_DATA * >( m_clSFItem.GetData() );
	if( !pSkillData )
		return FALSE;

	pi_pTarget->GetCharAniInfo()->SetPushOut_Speed( CHAR_SPEED_DEFAULT * 5 );
	pi_pTarget->GetCharAniInfo()->SetPushOut_Distance( pSkillData->fMoveDistance[m_Animation.GetCurrentEffectFrameNo()] );

	return TRUE;
}

BOOL
CCharacter::SetPushOut_PushedOutForDeath( CCharacter * pi_pTarget )
{
	pi_pTarget->GetCharAniInfo()->SetPushOut_Speed( pi_pTarget->GetSpeed( CMM_MOVE_RUN ) * 1 );
	pi_pTarget->GetCharAniInfo()->SetPushOut_Distance( 10.0f );

	return TRUE;
}

BOOL
CCharacter::SetPushOut_PushedOutForDamaged( CCharacter * pi_pTarget )
{
	float l_fDist = GetDist( m_vecPos, pi_pTarget->GetPosition() );
	float l_fAttackRange = GetAttackRange( CAT_NORMAL, FALSE );

	if( GetAttackDistanceType( CAT_NORMAL ) == ADT_NEAR )
		l_fAttackRange *= 0.75f;	// 근거리는 무기 사정거리의 3/4
	else
		l_fAttackRange = 20.0f;		// 원거리는 20으로 고정

	if( l_fDist >= l_fAttackRange )
		return FALSE;

	pi_pTarget->GetCharAniInfo()->SetPushOut_Speed( pi_pTarget->GetSpeed( CMM_MOVE_RUN ) * 5 );
	pi_pTarget->GetCharAniInfo()->SetPushOut_Distance( ( l_fAttackRange - l_fDist ) + 2.0f );

	return TRUE;
}

BOOL
CCharacter::SetPushOut_PushedOutForSkill( CCharacter * pi_pTarget )
{
	float l_fDist = GetDist( m_vecPos, pi_pTarget->GetPosition() );
	float l_fAttackRange = GetAttackRange( CAT_SKILL, FALSE );

	if( l_fDist >= l_fAttackRange )
		return FALSE;
	if( GetAttackDistanceType( CAT_NORMAL ) == ADT_FAR ) // 원거리는 밀려나가지 않음
		return FALSE;

	pi_pTarget->GetCharAniInfo()->SetPushOut_Speed( CHAR_SPEED_DEFAULT * 5 );
	pi_pTarget->GetCharAniInfo()->SetPushOut_Distance( l_fAttackRange - l_fDist );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCharacter::FrameMove_Skill( void )
{
	if( IsEqualAction( CAI_SKILL_INVOKE ) )
	{
		if( !m_Animation.IsAnimating() )
			SetAction( CAI_MOVE_STOP );
	}
}

void
CCharacter::FrameMove_Force( void )
{
	if( IsEqualAction( CAI_FORCE_INVOKE ) )
	{
		if( !m_Animation.IsAnimating() )
			SetAction( CAI_MOVE_STOP );
	}
}

void
CCharacter::FrameMove_InternalState( void )
{
	if( IsEqualAction( CAI_INTERNALSTATE_DEATH ) )
		m_pCombatState->ProcessDeath();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 매 프레임마다 처리해야 할 사항들
void
CCharacter::FrameMove_EveryFrame( void )
{
	// push out
	if( m_pCharAniInfo->ProcessPushOut( m_vecPos ) )
		CheckYPos();

	// rock
	m_pCharAniInfo->ProcessRock( m_vecPos );

	// 데미지를 입었을 경우의 이펙트 처리
	m_pCombatState->ProcessDamage();

	m_stAppliedSF.FrameMove();

	// Y position 값을 계산
	if( IsEnable( ROSF_CHECK_Y_POS ) && IsEqualAction( CAI_MOVE_MOVING ) )
	{
		if( m_bIsPassedFrameMoveLimitTime )
			CheckYPos();
	}

	// Idle
	if( !( IsDeath() || IsEqualAction( CAI_ITEM_USE_MINING ) ) )
	{
		if( timeGetTime() - m_dwActionTime >= m_dwIdleTime )
			SetAction( CAI_MOVE_IDLE );
	}

	// 적용 시간이 지난 SF 삭제 => 서버에서 프로토콜이 오면 지우므로 주석 처리
	//m_stAppliedSF.DeleteSFAferAppliedTime();

	// 레이더 세팅
	if( IsEqualAction( CAI_MOVE_MOVING ) && m_bIsPassedFrameMoveLimitTime )
		UI_SetPositionOfRadar();
}

BYTE
CCharacter::GetDummyIndex( DWORD pi_dwArrayId )
{ 
	if( pi_dwArrayId >= MAX_EFFECT_DUMMYS )
		return 0xFF;
	return m_byDummy[pi_dwArrayId]; 
}
void
CCharacter::SetDummyIndex( DWORD pi_dwArrayId,DWORD pi_dwId )	
{ 
	if( pi_dwArrayId >= MAX_EFFECT_DUMMYS )
		return;
	m_byDummy[pi_dwArrayId]=pi_dwId; 
}
