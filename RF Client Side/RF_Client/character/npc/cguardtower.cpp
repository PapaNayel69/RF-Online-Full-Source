////////////////////////////////////////////////////////////////////////////
//
// CGuardTower Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../GameObject/Board/Common/CBoardManager.h"

#include "../../AddCodeByYun/ShadowList.h"
#include "../../AddCodeByYun/ParticleList.h"

#include "../../UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"

#include "CGuardTower.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CGuardTower::CGuardTower()
{
	m_dwCharTypeID		= CTI_GUARD_TOWER;

	m_dwOwerIndex		= ID_INVALID;
	m_dwRemainBuildTime	= 0;
	m_pGuardTowerData	= NULL;

	m_dwTextColor		= CHAR_MONSTER_NAME_COLOR;
}

CGuardTower::~CGuardTower()
{
}

CCharacter *
CGuardTower::New( void )
{
	return ( dynamic_cast< CCharacter * >( new CGuardTower ) );
}

CCharacter *
CGuardTower::Clone( void )
{
	return ( dynamic_cast< CCharacter * >( new CGuardTower( *this ) ) );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CGuardTower::Render( void )
{
	if( !CBaseNpc::Render() )
		return FALSE;

	return TRUE;
}

BOOL
CGuardTower::Animation( DWORD pi_dwAniFrame )
{
	return CBaseNpc::Animation( pi_dwAniFrame );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGuardTower::FrameMove( void )
{
	if( IsDisable( ROSF_MOVABLE ) )
		return;

	// 소환
	if( IsEqualAction( CAI_GESTURE_SUMMON ) )
	{
		if( !m_Animation.IsAnimating() )
			SetAction( CAI_MOVE_STOP );
	}
	// 소환 해제 => 애니메이션 후에 삭제
	else if( IsEqualAction( CAI_GESTURE_REPATRIATE ) )
	{
		if( !m_Animation.IsAnimating() )
			Enable( ROSF_DESTROY );
	}

	CBaseNpc::FrameMove();
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

DWORD
CGuardTower::GetEffectTypeID( DWORD pi_dwActionID )
{
	switch( pi_dwActionID )
	{
		case CAI_CREATE_CREATE		: return AET_CREATE_CREATE;
		case CAI_DEATH_READY		: return AET_CREATE_DESTROY;
		case CAI_COMBAT_ATTACK		: return AET_ATTACK_NORMAL;
		case CAI_FORCE_CAST			:
		case CAI_FORCE_INVOKE		: break;	// skill/force는 따로 처리된다.
		case CAI_DAMAGED_NORMAL		: return AET_DAMAGE_NORMAL;
		case CAI_DAMAGED_CRITICAL	: return AET_DAMAGE_CRITICAL;
		case CAI_DAMAGED_MISS		: return AET_DAMAGE_MISS;
		case CAI_GESTURE_SUMMON		: return AET_GESTURE_SUMMON;
		case CAI_GESTURE_REPATRIATE	: return AET_GESTURE_REPATRIATE;
	}

	return ID_INVALID;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CGuardTower::SetAction_ProcessActionID( DWORD pi_dwActionID )
{
	switch( pi_dwActionID )
	{
		case CAI_GESTURE_SUMMON		:
			SetEffectInfo( pi_dwActionID );
			break;

		case CAI_GESTURE_REPATRIATE	:
			Disable( ROSF_ANIMATION_LOOP );
			SetEffectInfo( pi_dwActionID );
			break;
	}
}

void
CGuardTower::SetAction_SetEffectAtferSetAnimation( DWORD pi_dwActionID )
{
	CBaseNpc::SetAction_SetEffectAtferSetAnimation( pi_dwActionID );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD
CGuardTower::GetAnimationTypeID( DWORD pi_dwActionID )
{
	switch( pi_dwActionID )
	{
		case CAI_INTERNALSTATE_DEATH:
		case CAI_MOVE_STOP			: return GAI_MOVE_STOP;
		case CAI_MOVE_MOVING		: return GAI_MOVE_WALK;
		case CAI_MOVE_IDLE			: return GAI_MOVE_IDLE;
		case CAI_COMBAT_ATTACK		: return GAI_COMBAT_ATTACK;
		case CAI_FORCE_CAST			:
		case CAI_FORCE_INVOKE		: return GAI_FORCE_ATTACK;
		case CAI_DAMAGED_CRITICAL	: return GAI_DAMAGE_CRITICAL;
		case CAI_CREATE_CREATE		: return GAI_CREATE_CREATE;
		case CAI_DEATH_READY		: return GAI_CREATE_DYING;
		case CAI_DEATH_DIE			: return GAI_CREATE_DEATH;
		case CAI_GESTURE_SUMMON		: return GAI_CREATE_SUMMON;
		case CAI_GESTURE_REPATRIATE	: return GAI_CREATE_RETURN;
	}

	return ID_INVALID;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CAniData *
CGuardTower::GetAnimationData_Normal( DWORD pi_dwAniTypeID )
{
	if( pi_dwAniTypeID == ID_INVALID )
		return NULL;

	DWORD dwAniID = 0;

	// 몬스터 일반 애니메이션 ID는 메시(혹은 본)ID + 액션 ID + 개수 + 0 ( ~ F ) 이다.
	dwAniID |= ( GetMeshID() << 8 );
	dwAniID |= ( pi_dwAniTypeID << 4 );

	return GetAnimationDataByRandom( RLI_GUARD_TOWER, dwAniID );
}

CAniData *
CGuardTower::GetAnimationData_Force( DWORD pi_dwAniTypeID )
{
	if( pi_dwAniTypeID == ID_INVALID )
		return NULL;

	DWORD dwAniID = 0;

	// 몬스터 포스 애니메이션 ID는 메시(혹은 본)ID + 액션 ID + 개수 + 몬스터 데이터상 포스의 순번 이다.
	dwAniID |= ( GetMeshID() << 8 );
	dwAniID |= ( pi_dwAniTypeID << 4 );
	dwAniID |= m_byForceNo;

	m_Animation.SetAnimationID( dwAniID );	// 애니메이션이 NULL 이라면 상위단계에서 다시 ID_INVALID 로 세팅해 주므로 미리 세팅해도 상관없다.

	return _GetAniMgr()->GetAni( RLI_GUARD_TOWER, dwAniID );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CGuardTower::SetData( BASE_DATA * pi_pData )
{
	m_pGuardTowerData = static_cast< GUARD_TOWER_DATA * >( pi_pData );

	CBaseNpc::SetData( pi_pData );
}

void
CGuardTower::SetAttribute( BASE_DATA * pi_pData )
{
	if( !pi_pData )
		return;

	SetData( pi_pData );

	SetName( m_pGuardTowerData->pName );
	SetBone( m_pGuardTowerData->dwCharMeshID );
	SetMesh( m_pGuardTowerData->dwCharMeshID );

	SetSpeed( CMM_MOVE_WALK, 0.0f );
	SetSpeed( CMM_MOVE_RUN, 0.0f );

	SetMaxHP( m_pGuardTowerData->dwMaxDP );
	if( m_pCombatState->GetRemainHP() == ID_INVALID )
		SetHP( m_pGuardTowerData->dwMaxDP );

	LoadAnimaitonData();
}

void
CGuardTower::SetBone( DWORD pi_dwBoneID )
{
	m_pBone = _GetBoneMgr()->GetBone( RLI_GUARD_TOWER, pi_dwBoneID );
	m_pBBox = _GetBBoxMgr()->GetBoundBox( RLI_GUARD_TOWER, pi_dwBoneID );
}

void
CGuardTower::SetMesh( DWORD pi_dwIndex )
{
	m_pMesh = _GetMeshMgr()->GetMesh( RLI_GUARD_TOWER, pi_dwIndex );
}

BYTE
CGuardTower::GetLevel( void )
{
	return ( m_pGuardTowerData ) ? m_pGuardTowerData->byLevel : 1;
}

BYTE
CGuardTower::GetWidth( void )
{
	return ( m_pGuardTowerData ) ? m_pGuardTowerData->byWidth : 1;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

float
CGuardTower::GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget )
{
	if( m_pGuardTowerData )
		return m_pGuardTowerData->fAttackDistance;

	return CBaseNpc::GetAttackRange( pi_byAttackType, pi_bCheckTarget );
}

BYTE
CGuardTower::GetRace( void )
{
	for( int i = 0; i <= CRT_ACCRETIA; ++i )
	{
		if( m_pGuardTowerData && m_pGuardTowerData->byUsableRace[i] )
			return i;
	}

	return 0;
}

void
CGuardTower::SetRemainBuildTime( DWORD pi_dwTime )
{
	m_dwRemainBuildTime = pi_dwTime;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CGuardTower::IsPassedLimitTimeFromReceiveNetworkMsg( void )
{
	return CBaseNpc::IsPassedLimitTimeFromReceiveNetworkMsg();
}

BOOL
CGuardTower::IsPassedLimitTimeForDelete( void )
{
	return CBaseNpc::IsPassedLimitTimeForDelete();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BYTE
CGuardTower::UI_GetRadarCharType( void )
{
	return CGUIRadarWindow::Other_Recall_Mob;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
