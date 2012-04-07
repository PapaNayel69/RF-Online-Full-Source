////////////////////////////////////////////////////////////////////////////
//
// CMonster Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../CLand.h"

#include "../../AddCodeByJang/MonsterSpecialEffect.h"
#include "../../AddCodeByJang/EffectUtil.h"

#include "../../AddCodeByYun/ShadowList.h"
#include "../../AddCodeByYun/ParticleList.h"

#include "../../UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CMonster::CMonster()
{
	m_pMonsterData		= NULL;

	m_dwCharTypeID		= CTI_MONSTER;
	m_byCreationType	= CCT_NORMAL;

	m_dwPrevMoveTime	= 0;

	m_dwTextColor		= CHAR_MONSTER_NAME_COLOR;
}

CMonster::~CMonster()
{
}

CCharacter *
CMonster::New( void )
{
	return ( dynamic_cast< CCharacter * >( new CMonster ) );
}

CCharacter *
CMonster::Clone( void )
{
	return ( dynamic_cast< CCharacter * >( new CMonster( *this ) ) );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CMonster::Render( void )
{
	if( !CBaseNpc::Render() )
		return FALSE;

	::CMonsterSpecialEffectRender((void*)this);			// by J.U.I 특별한 몬스터의 렌더링부분..

	return TRUE;
}

BOOL
CMonster::Animation( DWORD pi_dwAniFrame )
{
	return CBaseNpc::Animation( pi_dwAniFrame );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CMonster::FrameMove( void )
{
	if( IsDisable( ROSF_MOVABLE ) )
		return;

	if( IsEqualActionCategory( CAI_DEATH ) )
	{
		if( IsEqualAction( CAI_DEATH_DIE ) )
		{
			if( m_byDestroyType == mon_destroy_code_die )
				m_vecPos[1]	-= 0.05f;

			if( _GetTime() - m_dwDeadTime >= 2000 )
				Enable( ROSF_DESTROY );
		}
	}

	CBaseNpc::FrameMove();
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

DWORD
CMonster::GetEffectTypeID( DWORD pi_dwActionID )
{
	switch( pi_dwActionID )
	{
		case CAI_CREATE_CREATE		: return MET_CREATE_CREATE;
		case CAI_DEATH_READY		: return MET_CREATE_DESTROY;
		case CAI_COMBAT_ATTACK		: return MET_ATTACK_NORMAL;
		case CAI_FORCE_CAST			:
		case CAI_FORCE_INVOKE		: break;	// skill/force는 따로 처리된다.
		case CAI_DAMAGED_NORMAL		: return MET_DAMAGE_NORMAL;
		case CAI_DAMAGED_CRITICAL	: return MET_DAMAGE_CRITICAL;
		case CAI_DAMAGED_MISS		: return MET_DAMAGE_MISS;
	}

	return ID_INVALID;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CMonster::SetAction_ProcessActionID( DWORD pi_dwActionID )
{
	switch( pi_dwActionID )
	{
		case CAI_DEATH_DIE :
			Enable( ROSF_RENDER_ALPHA );
			break;
	}
}

BOOL
CMonster::SetAction_WillSetAnimation( DWORD pi_dwActionID )
{
	return ( !( ( pi_dwActionID == CAI_DEATH_DIE ) && ( m_byDestroyType != mon_destroy_code_die ) ) );
}

void
CMonster::SetAction_SetEffectAtferSetAnimation( DWORD pi_dwActionID )
{
	CMonsterSpecialEffectSetAction( this );		// by J.U.I

	CBaseNpc::SetAction_SetEffectAtferSetAnimation( pi_dwActionID );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

DWORD
CMonster::GetAnimationTypeID( DWORD pi_dwActionID )
{
	switch( pi_dwActionID )
	{
		case CAI_INTERNALSTATE_DEATH:
		case CAI_MOVE_STOP			: return ( m_byCombatMode == CSI_PEACE ) ? MAI_PEACE_STOP : MAI_COMBAT_STOP;
		case CAI_MOVE_MOVING		: return ( m_byCombatMode == CSI_PEACE ) ? MAI_PEACE_WALK : MAI_COMBAT_WALK;
		case CAI_MOVE_IDLE			: return ( m_byCombatMode == CSI_PEACE ) ? MAI_PEACE_IDLE : MAI_COMBAT_IDLE;
		case CAI_COMBAT_ATTACK		: return MAI_COMBAT_ATTACK;
		case CAI_FORCE_CAST			:
		case CAI_FORCE_INVOKE		: return MAI_FORCE_ATTACK;
		case CAI_DAMAGED_CRITICAL	: return ( m_byCombatMode == CSI_PEACE ) ? MAI_PEACE_CRITICAL : MAI_COMBAT_CRITICAL;
		case CAI_CREATE_CREATE		: return MAI_CREATE_CREATE;
		case CAI_DEATH_READY		: return MAI_DEATH_READY;
		case CAI_DEATH_DIE			: return MAI_DEATH_DIE;
	}

	return ID_INVALID;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CAniData *
CMonster::GetAnimationData_Normal( DWORD pi_dwAniTypeID )
{
	if( pi_dwAniTypeID == ID_INVALID )
		return NULL;

	DWORD dwAniID = 0;

	// 몬스터 일반 애니메이션 ID는 메시(혹은 본)ID + 액션 ID + 개수 + 0 ( ~ F ) 이다.
	dwAniID |= ( GetMeshID() << 8 );
	dwAniID |= ( pi_dwAniTypeID << 4 );

	return GetAnimationDataByRandom( RLI_MONSTER, dwAniID );
}

CAniData *
CMonster::GetAnimationData_Force( DWORD pi_dwAniTypeID )
{
	if( pi_dwAniTypeID == ID_INVALID )
		return NULL;

	DWORD dwAniID = 0;

	// 몬스터 포스 애니메이션 ID는 메시(혹은 본)ID + 액션 ID + 개수 + 몬스터 데이터상 포스의 순번 이다.
	dwAniID |= ( GetMeshID() << 8 );
	dwAniID |= ( pi_dwAniTypeID << 4 );
	dwAniID |= m_byForceNo;

	m_Animation.SetAnimationID( dwAniID );	// 애니메이션이 NULL 이라면 상위단계에서 다시 ID_INVALID 로 세팅해 주므로 미리 세팅해도 상관없다.

	return _GetAniMgr()->GetAni( RLI_MONSTER, dwAniID );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CMonster::SetData( BASE_DATA * pi_pData )
{
	m_pMonsterData = static_cast< CMonsterData * >( pi_pData );

	CBaseNpc::SetData( pi_pData );
}

void
CMonster::SetAttribute( BASE_DATA * pi_pData )
{
	if( !pi_pData )
		return;

	SetData( pi_pData );

	SetName( m_pMonsterData->m_pName );
	SetBone( m_pMonsterData->m_dwMeshID >> 8 );
	SetMesh( m_pMonsterData->m_dwMeshID >> 8 );

	SetSpeed( CMM_MOVE_WALK, CHAR_SPEED_DEFAULT * m_pMonsterData->m_byMoveSpeed[0] );
	SetSpeed( CMM_MOVE_RUN, CHAR_SPEED_DEFAULT * m_pMonsterData->m_byMoveSpeed[1] );

	SetMaxHP( m_pMonsterData->m_dwMaxHP );
	if( m_pCombatState->GetRemainHP() == ID_INVALID )
		SetHP( m_pMonsterData->m_dwMaxHP );

	SetScale( m_pMonsterData->m_fScale );

	LoadAnimaitonData();
}

void
CMonster::SetBone( DWORD pi_dwBoneID )
{
	m_pBone = _GetBoneMgr()->GetBone( RLI_MONSTER, pi_dwBoneID );
	m_pBBox = _GetBBoxMgr()->GetBoundBox( RLI_MONSTER, pi_dwBoneID );
}

void
CMonster::SetMesh( DWORD pi_dwIndex )
{
	m_pMesh = _GetMeshMgr()->GetMesh( RLI_MONSTER, pi_dwIndex );
}

void
CMonster::SetDeath( BYTE pi_byDestroyType )
{
	if( IsDeath() )
		return;

	m_byDestroyType = pi_byDestroyType;

	if( pi_byDestroyType == mon_destroy_code_die )
	{
		m_pCombatState->SetRemainHP( 0 );
		SetAction( CAI_INTERNALSTATE_DEATH );
	}
	else if( pi_byDestroyType == mon_destroy_code_respawn )
	{
		SetAction( CAI_DEATH_DIE );
	}
}

BYTE
CMonster::GetLevel( void )
{
	return ( m_pMonsterData ) ? m_pMonsterData->m_byLevel : 1;
}

BYTE
CMonster::GetWidth( void )
{
	return ( m_pMonsterData ) ? m_pMonsterData->m_byWidth : 1;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

float
CMonster::GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget )
{
	if( m_pMonsterData )
		return m_pMonsterData->m_fAttackRange;

	return CBaseNpc::GetAttackRange( pi_byAttackType, pi_bCheckTarget );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CMonster::IsPassedLimitTimeFromReceiveNetworkMsg( void )
{
	return CBaseNpc::IsPassedLimitTimeFromReceiveNetworkMsg();
}

BOOL
CMonster::IsPassedLimitTimeForDelete( void )
{
	return CBaseNpc::IsPassedLimitTimeForDelete();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BYTE
CMonster::UI_GetRadarCharType( void )
{
	if( m_byCreationType == CCT_SUMMON )
		return CGUIRadarWindow::Other_Recall_Mob;

	return CGUIRadarWindow::Mob;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
