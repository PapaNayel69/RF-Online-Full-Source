////////////////////////////////////////////////////////////////////////////
//
// CAnimus Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../GameObject/Board/Common/CBoardManager.h"

#include "../../AddCodeByYun/ShadowList.h"
#include "../../AddCodeByYun/ParticleList.h"

#include "../../UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CAnimus::CAnimus()
{
	m_dwCharTypeID		= CTI_ANIMUS;

	m_pAnimusData		= NULL;
	m_pItemData			= NULL;

	m_dwTextColor		= CHAR_MONSTER_NAME_COLOR;
}

CAnimus::~CAnimus()
{
}

CCharacter *
CAnimus::New( void )
{
	return ( dynamic_cast< CCharacter * >( new CAnimus ) );
}

CCharacter *
CAnimus::Clone( void )
{
	return ( dynamic_cast< CCharacter * >( new CAnimus( *this ) ) );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CAnimus::Render( void )
{
	if( !CBaseNpc::Render() )
		return FALSE;

	return TRUE;
}

BOOL
CAnimus::Animation( DWORD pi_dwAniFrame )
{
	return CBaseNpc::Animation( pi_dwAniFrame );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAnimus::FrameMove( void )
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
CAnimus::GetEffectTypeID( DWORD pi_dwActionID )
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
CAnimus::SetAction_ProcessActionID( DWORD pi_dwActionID )
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
CAnimus::SetAction_SetEffectAtferSetAnimation( DWORD pi_dwActionID )
{
	CBaseNpc::SetAction_SetEffectAtferSetAnimation( pi_dwActionID );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD
CAnimus::GetAnimationTypeID( DWORD pi_dwActionID )
{
	switch( pi_dwActionID )
	{
		case CAI_INTERNALSTATE_DEATH:
		case CAI_MOVE_STOP			: return AAI_MOVE_STOP;
		case CAI_MOVE_MOVING		: return AAI_MOVE_WALK;
		case CAI_MOVE_IDLE			: return AAI_MOVE_IDLE;
		case CAI_COMBAT_ATTACK		: return AAI_COMBAT_ATTACK;
		case CAI_FORCE_CAST			:
		case CAI_FORCE_INVOKE		: return AAI_FORCE_ATTACK;
		case CAI_DAMAGED_CRITICAL	: return AAI_DAMAGE_CRITICAL;
		case CAI_CREATE_CREATE		: return AAI_CREATE_CREATE;
		case CAI_DEATH_READY		: return AAI_CREATE_DYING;
		case CAI_DEATH_DIE			: return AAI_CREATE_DEATH;
		case CAI_GESTURE_SUMMON		: return AAI_CREATE_SUMMON;
		case CAI_GESTURE_REPATRIATE	: return AAI_CREATE_RETURN;
	}

	return ID_INVALID;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CAniData *
CAnimus::GetAnimationData_Normal( DWORD pi_dwAniTypeID )
{
	if( pi_dwAniTypeID == ID_INVALID )
		return NULL;

	DWORD dwAniID = 0;

	// 몬스터 일반 애니메이션 ID는 메시(혹은 본)ID + 액션 ID + 개수 + 0 ( ~ F ) 이다.
	dwAniID |= ( GetMeshID() << 8 );
	dwAniID |= ( pi_dwAniTypeID << 4 );

	return GetAnimationDataByRandom( RLI_ANIMUS, dwAniID );
}

CAniData *
CAnimus::GetAnimationData_Force( DWORD pi_dwAniTypeID )
{
	if( pi_dwAniTypeID == ID_INVALID )
		return NULL;

	DWORD dwAniID = 0;

	// 몬스터 포스 애니메이션 ID는 메시(혹은 본)ID + 액션 ID + 개수 + 몬스터 데이터상 포스의 순번 이다.
	dwAniID |= ( GetMeshID() << 8 );
	dwAniID |= ( pi_dwAniTypeID << 4 );
	dwAniID |= m_byForceNo;

	m_Animation.SetAnimationID( dwAniID );	// 애니메이션이 NULL 이라면 상위단계에서 다시 ID_INVALID 로 세팅해 주므로 미리 세팅해도 상관없다.

	return _GetAniMgr()->GetAni( RLI_ANIMUS, dwAniID );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CAnimus::SetData( BASE_DATA * pi_pData )
{
	m_pAnimusData = static_cast< CAnimusData * >( pi_pData );

	CBaseNpc::SetData( pi_pData );
}

void
CAnimus::SetDataByExp( DWORD pi_dwExp )
{
	m_pAnimusData = _GetCharDataMgr()->GetAnimusByExp( m_pItemData->GetDTIndex(), pi_dwExp );

	if( _GetAnimusControlBoard()->IsEqualSummonAnimusChar( m_dwIndex ) )
		_GetAnimusControlBoard()->SetExp( pi_dwExp );
}

void
CAnimus::SetAttribute( BASE_DATA * pi_pData )
{
	if( !pi_pData )
		return;

	SetData( pi_pData );

	SetName( m_pItemData->pName );
	SetBone( GetMeshID() );
	SetMesh( GetMeshID() );

	SetSpeed( CMM_MOVE_WALK, CHAR_SPEED_DEFAULT * m_pAnimusData->m_byMoveSpeed[0] );
	SetSpeed( CMM_MOVE_RUN, CHAR_SPEED_DEFAULT * m_pAnimusData->m_byMoveSpeed[1] );

	SetMaxHP( m_pAnimusData->m_dwMaxHP );
	if( m_pCombatState->GetRemainHP() == ID_INVALID )
		SetHP( m_pAnimusData->m_dwMaxHP );

	SetScale( m_pAnimusData->m_fScale );

	LoadAnimaitonData();
}

void
CAnimus::SetBone( DWORD pi_dwBoneID )
{
	m_pBone = _GetBoneMgr()->GetBone( RLI_ANIMUS, pi_dwBoneID );
	m_pBBox = _GetBBoxMgr()->GetBoundBox( RLI_ANIMUS, pi_dwBoneID );
}

void
CAnimus::SetMesh( DWORD pi_dwIndex )
{
	m_pMesh = _GetMeshMgr()->GetMesh( RLI_ANIMUS, pi_dwIndex );
}

BYTE
CAnimus::GetLevel( void )
{
	return ( m_pAnimusData ) ? m_pAnimusData->m_byLevel : 1;
}

BYTE
CAnimus::GetGrade( void )
{
	BYTE byLevel = GetLevel();

	if( byLevel <= 20 )
		return AGT_RECRUIT;
	else if( byLevel <= 40 )
		return AGT_VETERAN;
	else if( byLevel <= 50 )
		return AGT_MASTER;

	return AGT_AMI;
}

BYTE
CAnimus::GetWidth( void )
{
	return ( m_pAnimusData ) ? m_pAnimusData->m_byWidth : 1;
}

DWORD
CAnimus::GetMeshID( void )
{
	if( !m_pAnimusData || !m_pItemData )
		return ID_INVALID;

	return ( ( m_pItemData->GetDTIndex() << 8 ) | GetGrade() );
}

void
CAnimus::SetHP( DWORD pi_dwHP )
{
	CCharacter::SetHP( pi_dwHP );

	if( _GetAnimusControlBoard()->IsEqualSummonAnimusChar( m_dwIndex ) )
		_GetAnimusControlBoard()->SetHP( pi_dwHP );
}

void
CAnimus::SetFP( DWORD pi_dwFP )
{
	CCharacter::SetFP( pi_dwFP );

	if( _GetAnimusControlBoard()->IsEqualSummonAnimusChar( m_dwIndex ) )
		_GetAnimusControlBoard()->SetFP( pi_dwFP );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

float
CAnimus::GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget )
{
	if( m_pAnimusData )
		return m_pAnimusData->m_fAttackRange;

	return CBaseNpc::GetAttackRange( pi_byAttackType, pi_bCheckTarget );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAnimus::IsPassedLimitTimeFromReceiveNetworkMsg( void )
{
	return CBaseNpc::IsPassedLimitTimeFromReceiveNetworkMsg();
}

BOOL
CAnimus::IsPassedLimitTimeForDelete( void )
{
	return CBaseNpc::IsPassedLimitTimeForDelete();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BYTE
CAnimus::UI_GetRadarCharType( void )
{
	return CGUIRadarWindow::Other_Recall_Mob;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
