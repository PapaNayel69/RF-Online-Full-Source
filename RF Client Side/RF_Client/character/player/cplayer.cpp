////////////////////////////////////////////////////////////////////////////////
//
// CPlayer Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "CPlayer.h"
#include "CUnit.h"

#include "../../GameObject/Board/Common/CBoardManager.h"
#include "../../GameObject/Board/General/CPartyBoard.h"
#include "../../GameObject/Manager/CTargetMgr.h"
#include "../../CLand.h"

#include "../../UserInterface/GameWindow/GUISelectedCharWin.h"
#include "../../UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"
#include "../../UserInterface/GameWindow/EtcWin/GUIMiniMapWindow.h"

#include "../../AddCodeByJang/EffectUtil.h"
#include "../../AddCodeByJang/PlayerSpecialEffect.h"
#include "../../AddCodeByYun/ParticleList.h"
#include "../../AddCodeByYun/ShadowList.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CPlayer::CPlayer()
{
	m_pLinkPlayer				= NULL;

	m_pResProc					= &m_clsPlayerProc;
	m_clsPlayerProc.m_pPlayer	= this;
	m_clsUnitProc.m_pPlayer		= this;

	m_byUserGrade				= USER_DEGREE_STD;
	
	m_dwCharTypeID				= CTI_PLAYER;
	m_byRace					= CRT_DEFAULT;
	m_byPrevRace				= m_byRace;

	m_dwLevel					= 1;
	m_dwMaxSP					= 0;
	m_dwSP						= 0;

	m_byUnitMountState			= UMS_DISMOUNT;
	m_byPrevUnitMountState		= UMS_DISMOUNT;
	m_fMoveMap					= 0;
	m_fWaterFoot				= 0;
	m_byIsInWater				= 0;

	m_byPartyMemberType			= PMT_NONE;
	m_byMapIndex				= 0xFF;

	m_byAttackPart				= ID_INVALID;
	m_dwBulletDTIndex			= ID_INVALID;
	ZeroMemory( m_vecSFTargetPos, sizeof( Vector3f ) );

	m_bIsQuitGame				= FALSE;

	//--------------------------------------------------------------------------

	Enable( ROSF_RENDER_MESH | ROSF_RENDER_DEFAULT_MESH );
	m_dwTextColor				= CHAR_PLAYER_NAME_COLOR;
}

CPlayer::~CPlayer()
{
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CPlayer::Render( void )
{
	if( IsDisable( ROSF_RENDER_MESH ) || IsEnable( ROSF_CLIPPED ) )
		return FALSE;

	if( !CCharacter::Render() )
		return FALSE;

	EFUtil_Set2ndLight( this );			// turn on damage light
	if( m_fMoveMap != 0 )				// 포탈이동클립처리.
		EFUtil_CharacterHide(this,m_fMoveMap);

	m_pResProc->Render();

	if( m_fMoveMap != 0 )				// 포탈이동클립복구.
		EFUtil_CharacterHideOff();

	EFUtil_UnSet2ndLight();				// turn off damage light

	m_pResProc->ProcessEffectAfterRender();

	return TRUE;
}

BOOL
CPlayer::RenderName( void )
{
	if( IsEnable( ROSF_RENDER_NAME ) && IsEnable( ROSF_RENDER_MESH ) && IsDisable( ROSF_CLIPPED ) )
	{
		static char pDisplayName[64];
		ZeroMemory( pDisplayName, 64 );

		if( _GetNetworkMgr()->GetAvatarGrade() <= USER_DEGREE_MGR )
		{
			switch( m_byUserGrade )
			{
				case USER_DEGREE_STD	:
				case USER_DEGREE_GID	: memcpy( pDisplayName, m_pName, MAX_NAME_LENGTH ); break;
				case USER_DEGREE_MGR	: sprintf( pDisplayName, "[ %s ] %s", "GM", m_pName ); break;
				case USER_DEGREE_DEV	: memcpy( pDisplayName, m_pName, MAX_NAME_LENGTH ); break;
			}
		}
		else if( _GetNetworkMgr()->GetAvatarGrade() >= USER_DEGREE_DEV )
		{
			switch( m_byUserGrade )
			{
				case USER_DEGREE_STD	:
				case USER_DEGREE_GID	: memcpy( pDisplayName, m_pName, MAX_NAME_LENGTH ); break;
				case USER_DEGREE_MGR	: sprintf( pDisplayName, "[ %s ] %s", "GM", m_pName ); break;
				case USER_DEGREE_DEV	: sprintf( pDisplayName, "[ %s ] %s", "DEV", m_pName ); break;
			}
		}
		else
		{
			memcpy( pDisplayName, m_pName, MAX_NAME_LENGTH );
		}

		Vector4f vecPos;
		_GetRenderNamePos( this, vecPos, strlen( pDisplayName ) );

		DrawR3Hangul3D( vecPos, pDisplayName, m_dwTextColor, R3_HAN_OUTLINE );
	}

	return TRUE;
}

void
CPlayer::CreateShadow( void )
{
	m_pResProc->CreateShadow();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPlayer::Animation( DWORD pi_dwAniFrame )
{
	m_pBone = m_pResProc->m_pBone;
	m_pBBox = m_pResProc->m_pBBox;

	m_pResProc->ProcessEffectBeforeAnimation();

	if( !CCharacter::Animation( pi_dwAniFrame ) )
		return FALSE;

	if( IsDisable( ROSF_ANIMATION_STOP_ENGINE ) )
		m_pResProc->Animation();

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayer::FrameMove( void )
{
	if( IsDisable( ROSF_MOVABLE ) )
		return;

	CCharacter::FrameMove();

	if( IsEqualActionCategory( CAI_MOVE ) )
		FrameMove_Move();
	else if( IsEqualActionCategory( CAI_SKILL ) )
		FrameMove_Skill();
	else if( IsEqualActionCategory( CAI_FORCE ) )
		FrameMove_Force();
	else if( IsEqualActionCategory( CAI_GESTURE ) )
		FrameMove_Gesture();

	FrameMove_EveryFrame();
}

void
CPlayer::FrameMove_Move( void )
{
	if( IsEqualAction( CAI_MOVE_STOP ) )
	{
		if( m_byUnitMountState == UMS_DISMOUNTING )
		{
			Enable( ROSF_ANIMATION_INTERPOLATION | ROSF_ANIMATION_LOOP );
			m_byUnitMountState = UMS_DISMOUNT;
		}
	}
}

void
CPlayer::FrameMove_Skill( void )
{
	if( m_clSFItem.IsEmpty() )
		return;

	if( IsEqualAction( CAI_SKILL_INVOKE ) )
	{
		if( IsEffectFrame() )
		{
			BOOL bIsEnablePushTarget = ( m_clSFItem.GetRangeType() < SFRT_COMBO_HIT );

			FrameMove_Attack_SetAttacker( CAI_SKILL_INVOKE, bIsEnablePushTarget, TRUE );
		}
	}
}

void
CPlayer::FrameMove_Force( void )
{
	if( m_clSFItem.IsEmpty() )
		return;

	if( IsEqualAction( CAI_FORCE_INVOKE ) )
	{
		if( IsEffectFrame() )
			FrameMove_Attack_SetAttacker( CAI_FORCE_INVOKE, FALSE, FALSE );
	}
}

void
CPlayer::FrameMove_Gesture( void )
{
	if( IsEqualAction( CAI_GESTURE_MOUNT ) )
	{
		if( !m_Animation.IsAnimating() )
		{
			SetAction( CAI_MOVE_STOP );
			Enable( ROSF_ANIMATION_INTERPOLATION | ROSF_ANIMATION_LOOP );
		}
	}
	else if( IsEqualAction( CAI_GESTURE_DISMOUNT ) )
	{
		if( !m_Animation.IsAnimating() )
		{
			SetAction( CAI_GESTURE_STANDBY );
			Enable( ROSF_ANIMATION_INTERPOLATION | ROSF_ANIMATION_LOOP );
		}
	}	
}

void
CPlayer::FrameMove_EveryFrame( void )
{
	if( _GetPartyBoard()->IsPartyMember( m_dwIndex ) && m_bIsPassedFrameMoveLimitTime )
		UI_SetPositionOfMinimap();
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// TRUE - 이펙트를 발생됐음. FALSE - 이펙트가 발생되지 않았음
BOOL
CPlayer::SetEffectInfo( DWORD pi_dwActionID )
{
	if( IsEnable( ROSF_AFTER_IMAGE_OF_OBJECT ) )
		return FALSE;

	if( CCharacter::SetEffectInfo( pi_dwActionID ) )
		return TRUE;

	EffectInfo stEffectInfo;

	m_pResProc->SetEffectInfo( stEffectInfo, pi_dwActionID );

	return PushEffectInfoToQueue( stEffectInfo );
}

DWORD
CPlayer::GetSkillForceEffectID( DWORD pi_dwActionID, CSFItem * pi_pSFItem )
{
	return m_clsPlayerProc.GetSkillForceEffectID( pi_dwActionID, pi_pSFItem );
}

BOOL
CPlayer::PushEffectInfoToQueue( EffectInfo & pi_stEffectInfo )
{
	if( pi_stEffectInfo.dwID == ID_INVALID )
		return FALSE;

	pi_stEffectInfo.SetMyself( this );

	pi_stEffectInfo.dwTargetPos[0]	= m_vecSFTargetPos[0];
	pi_stEffectInfo.dwTargetPos[1]	= m_vecSFTargetPos[1];
	pi_stEffectInfo.dwTargetPos[2]	= m_vecSFTargetPos[2];

	_GetEffectInfoList()->push_back( &pi_stEffectInfo );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPlayer::SetAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication )
{
	if( pi_dwActionID == ID_INVALID )
		return;

	if( SetAction_IsDuplicateAction( pi_dwActionID, pi_bCheckActionDuplication ) )
		return;

	SetAction_SwitchingCombatMode();
	SetAction_SaveCurrentState();

	ProcessActionID( pi_dwActionID );

	CAniData * pNewAniData = m_pResProc->GetAnimationByAction( pi_dwActionID );
	if( pNewAniData )
		SetAction_SetAnimationInterpolation( pi_dwActionID, pNewAniData );

	CCharacter::SetAction( pi_dwActionID, pi_bCheckActionDuplication );

	m_pResProc->ProcessEffectAfterSetAction();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPlayer::SetAction_IsDuplicateAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication )
{
	if( !pi_bCheckActionDuplication )
		return FALSE;

	// IDLE이나 공격의 경우에는 조건 검사를 하지 않는다.
	if( ( pi_dwActionID != CAI_MOVE_IDLE ) || ( pi_dwActionID != CAI_COMBAT_ATTACK ) ||
		( pi_dwActionID != CAI_SKILL_INVOKE ) )
	{
		if( ( m_dwActionID == pi_dwActionID ) &&
			( m_byPrevWalkMode == m_byWalkMode ) &&
			( m_byPrevCombatMode == m_byCombatMode ) &&
			( m_byPrevUnitMountState == m_byUnitMountState ) &&
			( m_byPrevRace == m_byRace ) )
			return TRUE;
	}

	return FALSE;
}

void
CPlayer::SetAction_SwitchingCombatMode( void )
{
	static const BYTE EQUIP_MODE[2]	= { IAI_UNEQUIP, IAI_EQUIP };
	static const BYTE FADE_MODE[2]	= { FRM_IN, FRM_OUT };

	if( IsEqualUnitMountState( UMS_MOUNT ) )
		return;

	if( m_byPrevCombatMode != m_byCombatMode )
	{
		CEquipItem * pWeapon = m_clsPlayerProc.GetEquipItem( IEPT_WEAPON );
		CEquipItem * pShield = m_clsPlayerProc.GetEquipItem( IEPT_SHIELD );

		if( pWeapon )
		{
			pWeapon->SetAnimation( EQUIP_MODE[m_byCombatMode] );
			pWeapon->SetFadeMode( FADE_MODE[m_byCombatMode], _GetFPS() );
		}
		if( pShield )
		{
			pShield->SetAnimation( EQUIP_MODE[m_byCombatMode] );
			pShield->SetFadeMode( FADE_MODE[m_byCombatMode], _GetFPS() );
		}
	}
}

void
CPlayer::SetAction_SaveCurrentState( void )
{
	m_byPrevWalkMode		= m_byWalkMode;
	m_byPrevCombatMode		= m_byCombatMode;
	m_byPrevRace			= m_byRace;
	m_byPrevUnitMountState	= m_byUnitMountState;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayer::ProcessActionID( DWORD pi_dwActionID )
{
	m_pResProc->ProcessActionID( pi_dwActionID );

	switch( pi_dwActionID >> 28 )
	{
		case CAI_MOVE		: ProcessActionID_Move( pi_dwActionID ); break;
		case CAI_COMBAT		: ProcessActionID_Combat( pi_dwActionID ); break;
		case CAI_SKILL		: ProcessActionID_Skill( pi_dwActionID ); break;
		case CAI_FORCE		: ProcessActionID_Force( pi_dwActionID ); break;
		case CAI_ITEM		: ProcessActionID_Item( pi_dwActionID ); break;
		case CAI_GESTURE	: ProcessActionID_Gesture( pi_dwActionID ); break;
	}
}

void
CPlayer::ProcessActionID_Move( DWORD pi_dwActionID )
{
}

void
CPlayer::ProcessActionID_Combat( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_COMBAT_ATTACK )
	{
		m_pCombatState->SetAttackType_Dist( GetAttackDistanceType( CAT_NORMAL ) );
		m_pCombatState->SetAttackType_AT( CAT_NORMAL );
	}
}

void
CPlayer::ProcessActionID_Skill( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_SKILL_INVOKE )
	{
		SetEffectInfo( CAI_SKILL_CAST );		// 처음 사용시 애니는 CAI_SKILL_INVOKE, 이펙트는 CAI_SKILL_CAST 로 나간다.

		m_pCombatState->SetAttackType_Dist( GetAttackDistanceType( CAT_SKILL ) );
		m_pCombatState->SetAttackType_AT( CAT_SKILL );
	}
}

void
CPlayer::ProcessActionID_Force( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_FORCE_INVOKE )
	{
		SetEffectInfo( CAI_FORCE_CAST );		// 처음 사용시 애니는 CAI_FORCE_INVOKE, 이펙트는 CAI_FORCE_CAST 로 나간다.

		m_pCombatState->SetAttackType_Dist( GetAttackDistanceType( CAT_FORCE ) );
		m_pCombatState->SetAttackType_AT( CAT_FORCE );
	}
}

void
CPlayer::ProcessActionID_Item( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_ITEM_USE_MINING )
	{
		SetEffectInfo( CAI_ITEM_USE_MINING );
	}
}

void
CPlayer::ProcessActionID_Gesture( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_GESTURE_MOUNT || pi_dwActionID == CAI_GESTURE_DISMOUNT )
	{
		SetEffectInfo( pi_dwActionID );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayer::SetAction_SetAnimationInterpolation( DWORD pi_dwActionID, CAniData * pi_pNewAniData )
{
	if( pi_pNewAniData && !m_Animation.IsEqual( pi_pNewAniData ) )
	{
		if( ( pi_dwActionID != CAI_MOVE_MOVING ) &&
			( pi_dwActionID != CAI_DEATH_READY ) &&
			( pi_dwActionID != CAI_DEATH_DIE ) )
			SetAnimationInterpolation( pi_pNewAniData, TRUE );
		else
			SetAnimationInterpolation( pi_pNewAniData, FALSE );

		m_Animation.SetAniData( pi_pNewAniData );
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPlayer::SetBone( DWORD pi_dwBoneID )
{
	m_pResProc->SetBone( pi_dwBoneID );

	m_pBone = m_pResProc->m_pBone;
	m_pBBox = m_pResProc->m_pBBox;
}

BOOL
CPlayer::PutOnEquipment( BYTE pi_byPartType, DWORD pi_dwDTIndex )
{
	return m_pResProc->PutOnEquipment( pi_byPartType, pi_dwDTIndex );
}

BOOL
CPlayer::PutOnEquipment( CBoardItem * pi_pBoardItem )
{
	return m_pResProc->PutOnEquipment( pi_pBoardItem );
}

BOOL
CPlayer::PutOnEquipment( CEquipItem * pi_pEquipItem )
{
	return m_pResProc->PutOnEquipment( pi_pEquipItem );
}

BOOL
CPlayer::PutOffEquipment( BYTE pi_byPartType )
{
	return m_pResProc->PutOffEquipment( pi_byPartType );
}

CEquipItem *
CPlayer::GetEquipItem( BYTE pi_byPartType )
{
	return m_pResProc->GetEquipItem( pi_byPartType );
}

CNormalItem *
CPlayer::GetNormalItem( BYTE pi_byPartType )
{
	return m_pResProc->GetNormalItem( pi_byPartType );
}

BOOL
CPlayer::CreateUnitEquipment( CUnitItem * pi_pUnitItem )
{
	return m_clsUnitProc.PutOnEquipment( pi_pUnitItem );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 유닛 탑승
// => 타 플레이어의 경우, 이미 유닛을 타고 있는 경우에도 이 함수가 호출될 수 있으므로 주의 필요.
//    이 상황을 고려하지 않으면 애니메이션이 잘못 나올 수 있음
// pi_bIsRiding => 유닛을 타거나 내리는 중인가?
BOOL
CPlayer::MountUnit( CUnitItem * pi_pUnitItem, BOOL pi_bIsRiding )
{
	if( !pi_pUnitItem || ( m_byUnitMountState != UMS_DISMOUNT ) )
		return FALSE;

	m_byUnitMountState	= UMS_MOUNT;
	m_pResProc			= &m_clsUnitProc;

	m_clsUnitProc.MountUnit();

	// 기갑장비의 Mount/Dismount 애니메이션에서는 메시자체가 틀려지므로
	// ROSF_ANIMATION_INTERPOLATION | ROSF_ANIMATION_LOOP 을 꺼줘야 함.
	// Mount/Dismount 애니메이션이 끝난 후 다시 킴
	if( pi_bIsRiding )
		Disable( ROSF_ANIMATION_INTERPOLATION | ROSF_ANIMATION_LOOP );

	// set equipment
	if( !CreateUnitEquipment( pi_pUnitItem ) )
		return FALSE;

	return TRUE;
}

// 유닛 해제
// => 타 플레이어의 경우, 유닛을 타고 있지 않은 경우에도 이 함수가 호출될 수 있으므로 주의 필요.
//    이 상황을 고려하지 않으면 애니메이션이 잘못 나올 수 있음
// pi_bIsRiding => 유닛을 타거나 내리는 중인가?
BOOL
CPlayer::DismountUnit( BOOL pi_bIsRiding )
{
	if( m_byUnitMountState != UMS_MOUNT )
		return FALSE;

	m_byUnitMountState	= UMS_DISMOUNTING;	// UMS_DISMOUNTING 상태에서 UMS_DISMOUNT 상태로 전환됨.
	m_pResProc			= &m_clsPlayerProc;

	if( pi_bIsRiding )
		Disable( ROSF_ANIMATION_INTERPOLATION | ROSF_ANIMATION_LOOP );

	return TRUE;
}

BYTE
CPlayer::GetUnitAttackPartType( void )
{
	if( ( m_byUnitMountState == UMS_MOUNT ) && ( m_clsUnitProc.m_pAttackParts ) )
		return m_clsUnitProc.m_pAttackParts->GetItemKind();

	return 0xFF;
}

BOOL
CPlayer::IsEqualLinkPlayer( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer || !m_pLinkPlayer )
		return FALSE;

	return m_pLinkPlayer->IsEqual( pi_pPlayer );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

float
CPlayer::GetRealSpeed( void )
{
	if( IsEqualUnitMountState( UMS_MOUNT ) )
		return ( R3API::R3GetLoopTime() * GetSpeed() * m_fMagnificationOfSpeed );

	return CCharacter::GetRealSpeed();
}

BYTE
CPlayer::GetAttackDistanceType( BYTE pi_byAttackType )
{
	// unit
	if( IsEqualUnitMountState( UMS_MOUNT ) )
	{
		return ADT_FAR;
	}
	// force
	else if( pi_byAttackType == CAT_FORCE )
	{
		return ADT_FAR;
	}
	// skill
	else if( pi_byAttackType == CAT_SKILL )
	{
		CSFItem * pSkillItem = &m_clSFItem;

		if( IsEqualCharType( CTI_AVATAR ) && _GetGBM()->GetSelectSF() )
			pSkillItem = _GetGBM()->GetSelectSF();

		// 보조 계열 스킬은 모두 자신에게만 사용되므로 근거리로 취급한다.
		if( !pSkillItem->IsAttackable() )
			return ADT_NEAR;
	}

	// normal attack or attack skill
	CEquipItem * pEquipItem = GetEquipItem( IEPT_WEAPON );
	if( pEquipItem )
		return pEquipItem->GetAttackDistanceType();

	return CCharacter::GetAttackDistanceType( pi_byAttackType );
}

float
CPlayer::GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget )
{
	return m_pResProc->GetAttackRange( pi_byAttackType );
}

BYTE
CPlayer::GetWidth( void )
{
	return 0;
	//return m_pResProc->GetAttackRange( pi_byAttackType );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayer::SetLightColor( DWORD pi_dwColor, BYTE pi_bFlag )
{
	m_pResProc->SetLightColor( pi_dwColor, pi_bFlag );
}

void
CPlayer::SetLightColorFromMap( void )
{
	CResObject::SetLightColorFromMap();

	m_pResProc->SetLightColorFromMap();
}

void
CPlayer::SetAlphaDensity( float pi_fAlpha )
{
	CResObject::SetAlphaDensity( pi_fAlpha );

	m_pResProc->SetAlphaDensity( pi_fAlpha );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayer::Enable( DWORD pi_dwFlag )
{
	CCharacter::Enable( pi_dwFlag );

	if( IsEnable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
	{
		if( !( _GetPartyBoard()->IsPartyMember( m_dwIndex ) && ( m_byMapIndex == _GetLand()->GetMapIndex() ) ) )
		{
			if( _GetRadarWindow() )
				_GetRadarWindow()->RemoveCharacter( m_dwCharTypeID, m_dwIndex );
		}

		if( !IsEqualUnitMountState( UMS_MOUNT ) )
			m_cSwordTrace.SetTraceState( FALSE ); // by J.U.I - 플레이어면 궤적 끈다.
	}

	//if( IsEnable( ROSF_RENDER_CLIPPED | ROSF_RENDER_ALPHA | ROSF_AFTER_IMAGE_OF_OBJECT | ROSF_RENDER_EFFECT ) )
		m_pResProc->Enable( pi_dwFlag );
}

void
CPlayer::Disable( DWORD pi_dwFlag )
{
	CCharacter::Disable( pi_dwFlag );

	//if( IsDisable( ROSF_RENDER_CLIPPED | ROSF_RENDER_ALPHA | ROSF_AFTER_IMAGE_OF_OBJECT | ROSF_RENDER_EFFECT ) )
		m_pResProc->Disable( pi_dwFlag );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPlayer::SetUserGrade( BYTE pi_byUserGrade )
{
	m_byUserGrade	= pi_byUserGrade;

	SetTextColor( ID_INVALID );
}

void
CPlayer::SetLevel( DWORD pi_dwLevel )
{
	m_dwLevel = pi_dwLevel;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayer::SetMaxHP( DWORD pi_dwMaxHP )
{
	if( IsEqualUnitMountState( UMS_MOUNT ) )
		m_clsUnitProc.SetMaxHP( pi_dwMaxHP );
	else
		CCharacter::SetMaxHP( pi_dwMaxHP );
}

DWORD
CPlayer::GetMaxHP( void )
{
	if( IsEqualUnitMountState( UMS_MOUNT ) )
		return m_clsUnitProc.GetMaxHP();
	else
		return CCharacter::GetMaxHP();
}

void
CPlayer::SetHP( DWORD pi_dwHP )
{
	if( IsEqualUnitMountState( UMS_MOUNT ) )
		m_clsUnitProc.SetHP( pi_dwHP );
	else
		CCharacter::SetHP( pi_dwHP );
}

DWORD
CPlayer::GetHP( void )
{
	if( IsEqualUnitMountState( UMS_MOUNT ) )
		return m_clsUnitProc.GetHP();
	else
		return CCharacter::GetHP();
}

float
CPlayer::GetHPRate( void )
{
	if( IsEqualUnitMountState( UMS_MOUNT ) )
		return m_clsUnitProc.GetHPRate();
	else
		return CCharacter::GetHPRate();
}

//==============================================================================

void
CPlayer::SetMaxFP( DWORD pi_dwMaxFP )
{
	if( !IsEqualUnitMountState( UMS_MOUNT ) )
		CCharacter::SetMaxFP( pi_dwMaxFP );
}

DWORD
CPlayer::GetMaxFP( void )
{
	if( !IsEqualUnitMountState( UMS_MOUNT ) )
		return CCharacter::GetMaxFP();
	else
		return 0;
}

void
CPlayer::SetFP( DWORD pi_dwFP )
{
	if( !IsEqualUnitMountState( UMS_MOUNT ) )
		CCharacter::SetFP( pi_dwFP );
}

DWORD
CPlayer::GetFP( void )
{
	if( !IsEqualUnitMountState( UMS_MOUNT ) )
		return CCharacter::GetFP();
	else
		return 0;
}

//==============================================================================

void
CPlayer::SetMaxSP( DWORD pi_dwMaxSP )
{
	if( IsEqualUnitMountState( UMS_MOUNT ) )
		m_clsUnitProc.SetMaxSP( pi_dwMaxSP );
	else
		m_dwMaxSP = pi_dwMaxSP;
}

DWORD
CPlayer::GetMaxSP( void )
{
	if( IsEqualUnitMountState( UMS_MOUNT ) )
		return m_clsUnitProc.GetMaxSP();
	else
		return m_dwMaxSP;
}

void
CPlayer::SetSP( DWORD pi_dwSP )
{
	if( IsEqualUnitMountState( UMS_MOUNT ) )
	{
		m_clsUnitProc.SetSP( pi_dwSP );
	}
	else
	{
		if( pi_dwSP > m_dwMaxSP )
			m_dwSP = m_dwMaxSP;
		else
			m_dwSP = pi_dwSP;
	}
}

DWORD
CPlayer::GetSP( void )
{
	if( IsEqualUnitMountState( UMS_MOUNT ) )
		return m_clsUnitProc.GetSP();
	else
		return m_dwSP;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayer::SetTextColor( DWORD pi_dwColor )
{
	m_dwTextColor = CHAR_PLAYER_NAME_COLOR;

	if( _GetNetworkMgr()->GetAvatarGrade() <= USER_DEGREE_MGR )
	{
		switch( m_byUserGrade )
		{
			case USER_DEGREE_STD :
			case USER_DEGREE_GID : break;
			case USER_DEGREE_MGR : m_dwTextColor = CHAR_GM_NAME_COLOR; break;
			case USER_DEGREE_DEV : break;
		}
	}
	else if( _GetNetworkMgr()->GetAvatarGrade() >= USER_DEGREE_DEV )
	{
		switch( m_byUserGrade )
		{
			case USER_DEGREE_STD :
			case USER_DEGREE_GID : break;
			case USER_DEGREE_MGR : m_dwTextColor = CHAR_GM_NAME_COLOR; break;
			case USER_DEGREE_DEV : m_dwTextColor = CHAR_DEV_NAME_COLOR; break;
		}
	}

	if( ( m_dwTextColor == CHAR_PLAYER_NAME_COLOR ) && ( pi_dwColor != ID_INVALID ) )
		m_dwTextColor = pi_dwColor;

	if( _GetTargetMgr() )
	{
		if( _GetTargetMgr()->IsEqualTarget( this ) )
			_GetTargetMgr()->SetTargetTextColor( m_dwTextColor );
	}
}

void
CPlayer::SetPartyMemberType( BYTE pi_byType )
{
	m_byPartyMemberType = pi_byType;

	if( pi_byType == PMT_NONE )
	{
		SetTextColor( CHAR_PLAYER_NAME_COLOR );
	}
	else if( _GetPartyBoard()->IsPartyMember( m_dwIndex ) )
	{
		if( pi_byType == PMT_LEADER )
			SetTextColor( UI_PARTY_LEADER_COLOR );
		else
			SetTextColor( UI_PARTY_MEMBER_COLOR );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayer::SetServerStatusFlag( DWORD pi_dwFlag )
{
	// 이동 & 전투
	SetWalkMode( ( pi_dwFlag >> FLAG_MOVE_TYPE ) & 0x1 );
	SetCombatMode( ( pi_dwFlag >> FLAG_MODE_TYPE ) & 0x1 );

	// 스텔스 & 디텍트
	m_stConditionInfo.m_bEnable_Stealth = ( pi_dwFlag >> FLAG_STEALTH_STATE ) & 0x1;
	m_stConditionInfo.m_bEnable_Detect = ( pi_dwFlag >> FLAG_DETECT_STATE ) & 0x1;

	// 스턴
	m_stConditionInfo.m_bEnable_Stun = ( pi_dwFlag >> FLAG_STUN_STATE ) & 0x1;

	// 파티
	if( ( pi_dwFlag >> FLAG_PARTY_STATE ) & 0x1 )
		SetPartyMemberType( ( ( pi_dwFlag >> FLAG_LEADER_STATE ) & 0x1 ) + 1 );	// 파티멤버인가? 리더인가?
	else
		SetPartyMemberType( PMT_NONE );

	// 죽음
	if( ( pi_dwFlag >> FLAG_CORPSE_STATE ) & 0x1 )
	{
		if( GetActionByCategory() != CAI_DEATH )
			SetAction( CAI_DEATH_DIE, FALSE );
	}

	// 옵져버
	if( ( pi_dwFlag >> FLAG_OBSERVER_STATE ) & 0x1 )
		Enable( ROSF_OBSERVER );
	else
		Disable( ROSF_OBSERVER );

	// 채굴
	if( ( pi_dwFlag >> FLAG_MINING_STATE ) & 0x1 )
	{
		if( IsEqualActionCategory( CAI_DEATH ) )
			SetAction( CAI_ITEM_USE_MINING );
	}

	UI_SetRadar();
	UI_SetMinimap();

	/*
	#define FLAG_MOVE_TYPE			0	//걷기, 뛰기 상태
	#define FLAG_MODE_TYPE			1	//전투, 비전투상태
	#define FLAG_STEALTH_STATE		2	//스텔스상태
	#define FLAG_STUN_STATE			3	//스턴상태
	#define FLAG_PARTY_STATE		4	//파티상태
	#define FLAG_CORPSE_STATE		5	//시체상태
	#define FLAG_LEADER_STATE		6	//파티리더	
	#define FLAG_DTRADE_STATE		7	//직거래중
	#define FLAG_DETECT_STATE		8	//detect모드
	#define FLAG_OBSERVER_STATE		9	//옵저버모드
	#define FLAG_PARTYLOCK_STATE	10	//파티잠금모드
	#define FLAG_MINING_STATE		11	//채굴모드
	*/
}

void
CPlayer::SetWalkMode( BYTE pi_byWalkMode )
{
	CCharacter::SetWalkMode( pi_byWalkMode );

	// 유닛과 플레이어일 때는 뛰기 속도가 틀리다.
	if( pi_byWalkMode == CMM_MOVE_RUN )
	{
		if( IsEqualUnitMountState( UMS_MOUNT ) )
			m_fSpeed[pi_byWalkMode] = CHAR_SPEED_DEFAULT * ( m_clsPlayerProc.m_pData->m_fMoveRunRate + m_clsUnitProc.GetBoosterSpeed() );
		else
			m_fSpeed[pi_byWalkMode] = CHAR_SPEED_DEFAULT * m_clsPlayerProc.m_pData->m_fMoveRunRate;
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 타겟 더미 생성 => 원거리 공격일 경우에만 더미를 만든다.
CDummy *
CPlayer::CreateDummy( float pi_fX, float pi_fZ )
{
	CDummy * pTarget = _GetCharMgr()->CreateDummy();
	if( !pTarget )
		return NULL;

	float		fYAngle			= m_vecRot[1];
	Vector3f	vecTargetPos	= { m_vecPos[0], m_vecPos[1], m_vecPos[2] };
	if( ( pi_fX != ID_INVALID ) || ( pi_fX != ID_INVALID ) )
	{
		vecTargetPos[0]	= pi_fX;
		vecTargetPos[2]	= pi_fZ;
		fYAngle			= GetYAngle( m_vecPos, vecTargetPos );
	}

	float fAttackRange = GetAttackRange( CAT_NORMAL );
	float fAddY = 10;
	if( m_pBBox )
		fAddY = m_pBBox->pMax[1] - m_pBBox->pMin[1];

	pTarget->SetPosX( m_vecPos[0] + ( (float)sin( AngleToPi( fYAngle ) ) * fAttackRange ) );
	pTarget->SetPosY( m_vecPos[1] + fAddY, FALSE );
	pTarget->SetPosZ( m_vecPos[2] + ( (float)cos( AngleToPi( fYAngle ) ) * fAttackRange ) );

	return pTarget;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPlayer::IsPassedLimitTimeFromReceiveNetworkMsg( void )
{
	const static DWORD PLAYER_AUTO_DESTROY_TIME = 300000;	// default 5분

	if( !IsRemovableWhenWasNotReciveNetworkMsg() )
		return FALSE;
	
	// 아바타의 파티원일 경우 제외
	if( !_GetPartyBoard()->IsPartyMember( m_dwIndex ) )
	{
		if( m_stETCInfo.m_dwReceiveMsgTime != ID_INVALID )
		{
			if( timeGetTime() - m_stETCInfo.m_dwReceiveMsgTime > PLAYER_AUTO_DESTROY_TIME )
				return TRUE;
		}
	}

	return FALSE;
}

BOOL
CPlayer::IsPassedLimitTimeForDelete( void )
{
	const static DWORD PLAYER_DELETE_TIME = 1800000;		// default 30분
	
	// 아바타의 파티원일 경우 제외
	if( !_GetPartyBoard()->IsPartyMember( m_dwIndex ) )
	{
		if( m_stETCInfo.m_dwReceiveMsgTime != ID_INVALID )
		{
			if( timeGetTime() - m_stETCInfo.m_dwReceiveMsgTime > PLAYER_DELETE_TIME )
				return TRUE;
		}
	}

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayer::SetTraceOfWeapon( BOOL pi_bFlag )
{
	if( pi_bFlag && IsEnable( ROSF_RENDER_EFFECT ) )
		m_cSwordTrace.SetTraceState( TRUE );
	else
		m_cSwordTrace.SetTraceState( FALSE );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CPlayer::UI_SetRadar_CheckMe( void )
{
	if( !_GetRadarWindow() || IsEqualCharType( CTI_AVATAR ) || ( m_dwIndex == ID_INVALID ) )
		return FALSE;

	BOOL bIsPartyMember = _GetPartyBoard()->IsPartyMember( m_dwIndex );

	if( !bIsPartyMember && IsEnable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
		return FALSE;

	if( bIsPartyMember && ( m_byMapIndex != _GetLand()->GetMapIndex() ) )
		return FALSE;

	return TRUE;
}

void
CPlayer::UI_SetRadar( void )
{
	if( !UI_SetRadar_CheckMe() )
		return;

	CGUIRadarWindow::CharType byType = CGUIRadarWindow::Same_Race;

	if( _GetPartyBoard()->IsPartyLeader( m_dwIndex ) )
		byType = CGUIRadarWindow::PartyLeader;
	else if( _GetPartyBoard()->IsPartyMember( m_dwIndex ) )
		byType = CGUIRadarWindow::PartyMember;
	else if( m_stConditionInfo.m_bEnable_Stealth && !_GetAvatar()->IsEnableDetect( this ) )
		byType = CGUIRadarWindow::Stealth;
	else if( IsEqualRace( _GetAvatar()->GetRace() ) )
		byType = CGUIRadarWindow::Same_Race;
	else if( IsBellato() )
		byType = CGUIRadarWindow::Other_Bel;
	else if( IsCora() )
		byType = CGUIRadarWindow::Other_Cor;
	else
		byType = CGUIRadarWindow::Other_Acc;

	BOOL l_bShowDirection = FALSE;
	if( _GetPartyBoard()->IsPartyMember( m_dwIndex ) )
		l_bShowDirection = TRUE;

	_GetRadarWindow()->AddCharacter( m_dwCharTypeID, m_dwIndex, byType,
									 m_vecPos[0], m_vecPos[1], m_vecPos[2], l_bShowDirection );
}

void
CPlayer::UI_SetPositionOfRadar( void )
{
	if( !UI_SetRadar_CheckMe() )
		return;

	CCharacter::UI_SetPositionOfRadar();
}

void
CPlayer::UI_SetMinimap( void )
{
	if( !_GetMinimapWindow() || IsEqualCharType( CTI_AVATAR ) )
		return;

	BOOL								bAdd = FALSE;
	CGUIMiniMapWindow::MiniMapObjType	guiType;

	if( _GetPartyBoard()->IsPartyLeader( m_dwIndex ) )
	{
		if( m_byMapIndex == _GetLand()->GetMapIndex() )
		{
			bAdd		= TRUE;
			guiType	= CGUIMiniMapWindow::PartyLeader;
		}
	}
	else if( _GetPartyBoard()->IsPartyMember( m_dwIndex ) )
	{
		if( m_byMapIndex == _GetLand()->GetMapIndex() )
		{
			bAdd		= TRUE;
			guiType	= CGUIMiniMapWindow::PartyMember;
		}
	}

	if( bAdd )
	{
		_GetMinimapWindow()->AddMiniMapObject( guiType,
											   m_dwIndex | ( CGUIMiniMapWindow::PartyMember << 28 ),
											   m_vecPos[0], m_vecPos[2] );
	}
	else
	{
		_GetMinimapWindow()->RemoveMiniMapObject( m_dwIndex | ( CGUIMiniMapWindow::PartyMember << 28 ) );
	}
}

void
CPlayer::UI_SetPositionOfMinimap( void )
{
	if( _GetMinimapWindow() && ( m_byMapIndex == _GetLand()->GetMapIndex() ) )
		_GetMinimapWindow()->UpdateMiniMapObject( m_dwIndex | ( CGUIMiniMapWindow::PartyMember << 28 ),
												  m_vecPos[0], m_vecPos[2] );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayer::LoadAnimaitonData( void )
{
	m_pResProc->LoadAnimaitonData();
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////