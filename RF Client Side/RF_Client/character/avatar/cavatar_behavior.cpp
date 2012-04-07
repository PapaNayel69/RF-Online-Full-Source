#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../GameObject/Manager/CTargetMgr.h"
#include "../../GameObject/Board/Common/CBoardManager.h"
#include "../../GameObject/Board/General/CPartyBoard.h"

#include "../../Network/NetMsgProcessor/Unit/CNetMsgProcessor_Unit.h"

#include "../../CCamera.h"
#include "../../CLand.h"
#include "../../GameProgress/CGP_MainGame.h"

#include "../../UserInterface/GameWindow/GUIAttackPartWindow.h"
#include "../../UserInterface/GameWindow/GUISelectedCharMenu.h"
#include "../../UserInterface/GameWindow/GUIChatWindow.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
EP_ChaseTarget( void )
{
	CCharacter * l_pTarget = _GetTargetMgr()->GetTarget();
	if( l_pTarget )
		_GetTargetMgr()->SetPursuitInfo( l_pTarget->GetCharTypeID(), PPT_PURSUIT );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CAvatar::RequestMoveByKeyboard( DWORD pi_dwKeyType )
{
	if( !IsEnableMove() )
		return;

	/*
	float l_fRotY;
	if( pi_dwKeyType == CMK_UP )
		l_fRotY = 360 - _GetCamera()->GetPitch();
	else if( pi_dwKeyType == CMK_DOWN )
		l_fRotY = 180 - _GetCamera()->GetPitch();
	else if( pi_dwKeyType == CMK_LEFT )
		l_fRotY = 360 - _GetCamera()->GetPitch() - 90;
	else if( pi_dwKeyType == CMK_RIGHT )
		l_fRotY = 360 - _GetCamera()->GetPitch() + 90;
	else if( pi_dwKeyType == CMK_LEFT + CMK_UP )
		l_fRotY = 360 - _GetCamera()->GetPitch() - 45;
	else if( pi_dwKeyType == CMK_RIGHT + CMK_UP )
		l_fRotY = 360 - _GetCamera()->GetPitch() + 45;
	else if( pi_dwKeyType == CMK_LEFT + CMK_DOWN )
		l_fRotY = 360 - _GetCamera()->GetPitch() - 135;
	else if( pi_dwKeyType == CMK_RIGHT + CMK_DOWN )
		l_fRotY = 360 - _GetCamera()->GetPitch() + 135;
	else
		return;
	*/
	float l_fRotY;

	if( pi_dwKeyType == CMK_LEFT + CMK_UP )
		l_fRotY = 360 - _GetCamera()->GetPitch() - 45;
	else if( pi_dwKeyType == CMK_RIGHT + CMK_UP )
		l_fRotY = 360 - _GetCamera()->GetPitch() + 45;
	else if( pi_dwKeyType == CMK_LEFT + CMK_DOWN )
		l_fRotY = 360 - _GetCamera()->GetPitch() - 135;
	else if( pi_dwKeyType == CMK_RIGHT + CMK_DOWN )
		l_fRotY = 360 - _GetCamera()->GetPitch() + 135;
	else if( pi_dwKeyType == CMK_UP )
		l_fRotY = 360 - _GetCamera()->GetPitch();
	else if( pi_dwKeyType == CMK_DOWN )
		l_fRotY = 180 - _GetCamera()->GetPitch();
	else if( pi_dwKeyType == CMK_LEFT )
		l_fRotY = 360 - _GetCamera()->GetPitch() - 90;
	else if( pi_dwKeyType == CMK_RIGHT )
		l_fRotY = 360 - _GetCamera()->GetPitch() + 90;
	else
		return;

	//--------------------------------------------------------------------------

	static const float MAX_MOVE_DISTANCE = 60;
	Vector3f l_vecTargetPos;

	l_vecTargetPos[0] = m_vecPos[0] + ( (float)sin( AngleToPi( l_fRotY ) ) * MAX_MOVE_DISTANCE );
	l_vecTargetPos[2] = m_vecPos[2] + ( (float)cos( AngleToPi( l_fRotY ) ) * MAX_MOVE_DISTANCE );

	if( !m_pObserverInfo->m_bIsObserver )
	{
		l_vecTargetPos[1] = m_vecPos[1];

		_GetLand()->GetLevel()->mBsp->CanYouGoThere( m_vecPos, l_vecTargetPos, &l_vecTargetPos );
	}
	else
	{
		m_pObserverInfo->m_byKeyDownCode = pi_dwKeyType;

		if( pi_dwKeyType == CMK_UP )
			l_vecTargetPos[1] = m_vecPos[1] - ( AngleToPi( _GetCamera()->GetYaw() ) * MAX_MOVE_DISTANCE );
		else if( pi_dwKeyType == CMK_DOWN )
			l_vecTargetPos[1] = m_vecPos[1] + ( AngleToPi( _GetCamera()->GetYaw() ) * MAX_MOVE_DISTANCE );
		else
			l_vecTargetPos[1] = m_vecPos[1];	
	}

	//--------------------------------------------------------------------------

	if( GetDist( m_vecPos, l_vecTargetPos ) > 2 )
	{
		_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( l_vecTargetPos, TRUE, FALSE, 0 );
		if( !m_bMoveByKeyboard )
			memcpy( m_vecStartPos_MoveByKeyboard, m_vecPos, sizeof( float ) * 3 );

		_GetTargetMgr()->SetPursuitInfo( PTT_NONE, PPT_NONE );

		m_bMoveByKeyboard = TRUE;
	}
}

void
CAvatar::RequestStopByKeyboard( void )
{
	// 키보드 이동의 최소 이동 거리는 20
	static const float MIN_MOVE_DISTANCE = 20.0f;

	if( m_bMoveByKeyboard )
	{
		Vector3f l_vecTargetPos;
		GetPosByDistFromATOB( &l_vecTargetPos, m_vecPos, m_vecNextPath, MIN_MOVE_DISTANCE );

		_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( l_vecTargetPos,
														FALSE, FALSE, 0 );
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CAvatar::ProcessGroundItemForPickUp( CGroundItem * pi_pGroundItem )
{
	if( !pi_pGroundItem )
		return FALSE;

	// 아이템 획득 가능 상태인지를 체크한다. IsEnableMove() 함수 사용
	if( !IsEnableMove() )
		return FALSE;

	// 아바타가 채굴중이라면 그냥 나온다.
	if( IsEqualAction( CAI_ITEM_USE_MINING ) )
	{
		_SetSystemMsg( "채굴중에는 아이템 루팅이 불가능합니다." );
		return FALSE;
	}

	// 아이템을 획득할 수 있는 거리내에 있는지를 체크
	float fDist = GetDist( m_vecPos, pi_pGroundItem->GetPosition() );
	if( fDist > DISTANCE_ITEM_PICKUP )
	{
		_GetTargetMgr()->SetTargetItem( pi_pGroundItem );
		_GetTargetMgr()->SetPursuitInfo( PTT_GROUND_ITEM, PPT_LOOTING );

		MoveToTarget( pi_pGroundItem, fDist - ( DISTANCE_ITEM_PICKUP - 3 ) );
		return FALSE;
	}

	if( IsEqualAction( CAI_MOVE_MOVING ) )
		_GetNetworkMgr()->PositionMsg_PlayerStop_clzo();

	// 포탈 처리
	if( pi_pGroundItem->IsPortalGotoDungeon() )
		return EnterToDungeonPortal( pi_pGroundItem );

	// 아바타가 획득할 수 있는 아이템인지를 먼저 체크한다.
	if( !( ( pi_pGroundItem->GetOwnerIndex() == m_dwIndex ) || ( pi_pGroundItem->GetOwnerIndex() == ID_INVALID ) ) )
	{
		_SetSystemMsg( "획득 권한이 없는 아이템입니다." );
		return TRUE;
	}

	return TRUE;
}

BOOL
CAvatar::EnterToDungeonPortal( CGroundItem * pi_pGroundItem )
{
	DUNGEON_ITEM_DATA * pItemData = static_cast< DUNGEON_ITEM_DATA * >( pi_pGroundItem->GetData() );
	if( !pItemData )
		return FALSE;

	if( pItemData->m_byEnterLimitation == DUNGEON_ITEM_DATA::DEL_NONE )
	{
		m_pMoveMapInfo->MoveToDungeon_Request( pi_pGroundItem );
	}
	else if( pItemData->m_byEnterLimitation == DUNGEON_ITEM_DATA::DEL_PARTY )
	{
		if( ( pi_pGroundItem->GetOwnerIndex() == m_dwIndex ) || _GetPartyBoard()->IsPartyMember( m_dwIndex ) )
			m_pMoveMapInfo->MoveToDungeon_Request( pi_pGroundItem );
		else
			_SetSystemMsg( "포탈을 연 플레이어 혹은 파티원만이 사용할 수 있습니다." );
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CAvatar::MountUnitRequest( CPlayer * pi_pUnit )
{
	if( !pi_pUnit )
		return FALSE;

	if( !IsEnableMove() )
		return FALSE;
	if( IsEqualAction( CAI_MOVE_MOVING ) )
		return FALSE;

	if( !IsEqualUnitMountState( UMS_DISMOUNT ) )
		return FALSE;

	// 유닛은 뛰기가 아닌 부스터 이동이므로 무조건 걷기모드로 바꿔줘야 함.
	if( m_byWalkMode == CMM_MOVE_RUN )
		ChangeMoveMode( CMM_MOVE_WALK );
	// 전투 모드도 비전투 모드로 변경한다.
	if( m_byCombatMode == CSI_COMBAT )
		ChangeCombatMode( CSI_PEACE );

	// set position
	SetPosition( pi_pUnit->GetPosition() );
	SetRotY( pi_pUnit->GetRotY(), FALSE );

	// request
	return m_pNMP_Unit->UnitTakeRequest_clzo();
}

void
CAvatar::MountUnitResult( void )
{
	CUnitItem * pUnitItem = _GetUnitStorehouseBoard()->GetCurUnit();
	if( !pUnitItem )
		return;

	if( !CPlayer::MountUnit( pUnitItem, TRUE ) )
		return;

	UI_RefreshCharInfoWindow( TRUE );

	_GetFuncKeyWindow()->SetUnitControlBoard();
	_GetFuncKeyWindow()->SetUnitBulletNum( CGUIUnitControlBoard::Main_Weapon, pUnitItem->GetBulletNum( IEPT_UNIT_ARMS ) );
	_GetFuncKeyWindow()->SetUnitBulletNum( CGUIUnitControlBoard::Sub_Weapon, pUnitItem->GetBulletNum( IEPT_UNIT_SHOULDER ) );

	SetAction( CAI_GESTURE_MOUNT );
}

BOOL
CAvatar::DismountUnitRequest( void )
{
	if( !IsEnableMove() )
		return FALSE;
	if( IsEqualAction( CAI_MOVE_MOVING ) )
		return FALSE;

	if( !IsEqualUnitMountState( UMS_MOUNT ) )
		return FALSE;

	return m_pNMP_Unit->UnitLeaveRequest_clzo();
}

void
CAvatar::DismountUnitResult( void )
{
	CPlayer::DismountUnit( TRUE );

	UI_RefreshCharInfoWindow( TRUE );

	_GetFuncKeyWindow()->SetAvatarControlBoard();

	SetAction( CAI_MOVE_STOP );
}

void
CAvatar::SetUnitBulletNum( BYTE pi_byAttackParts, DWORD pi_dwBulletNum )
{
	CUnitItem * pUnitItem = m_clsUnitProc.GetUnitItem();
	if( pUnitItem )
	{
		pUnitItem->SetBulletNum( pi_byAttackParts, pi_dwBulletNum );
		_GetFuncKeyWindow()->SetUnitBulletNum( ( CGUIUnitControlBoard::WeaponType )
											   ( pi_byAttackParts - IEPT_UNIT_ARMS ), pi_dwBulletNum );
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 부활
void
EP_RebirthAvatar( void )
{
	if( _GetAvatar()->IsDeath() )
		_GetNetworkMgr()->InitMsg_PlayerRevivalRequest_clzo();
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
EP_WhisperToPlayer( void )
{
	char		l_pBuf[32];
	CGUIInput * l_pInput;
	CCharacter*	l_pChar = _GetTargetMgr()->GetTarget();

	if( !l_pChar )
		return FALSE;

	sprintf( l_pBuf, "/%s ", l_pChar->GetName() );
	
	l_pInput = _GetChatInputWindow()->GetInputField();
	if( l_pInput )
	{
		l_pInput->Clear();
		l_pInput->SetText( l_pBuf );
	}

	//--------------------------------------------------------------------------

	l_pInput = _GetChatWindow()->GetInputField();
	if( l_pInput )
	{
		l_pInput->Clear();
		l_pInput->SetText( l_pBuf );
	}

	//--------------------------------------------------------------------------

	_GetChatWindow()->SetWhisperCharName( l_pChar->GetName() );

	//_GetSelectedCharMenu()->Close();
	_GetTargetMgr()->CloseWindow( UII_PLAYER_POPUP_MENU );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
// 선택된 캐릭터의 메뉴 열기
BOOL
CAvatar::UI_OpenSelectedCharMenu( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return FALSE;

	// 플레이어 이름
	_GetSelectedCharMenu()->SetCharName( pi_pPlayer->GetName() );

	// 파티 관련
	static enum { INVITE, JOIN, NONE };
	if( GetPartyMemberType() == PMT_NONE )
	{
		if( pi_pPlayer->GetPartyMemberType() == PMT_NONE )
			_GetSelectedCharMenu()->SetPartyMenu( ( CGUISelectedCharMenu::PartyMenu )INVITE );
		else if( pi_pPlayer->GetPartyMemberType() == PMT_LEADER )
			_GetSelectedCharMenu()->SetPartyMenu( ( CGUISelectedCharMenu::PartyMenu )JOIN );
		else
			_GetSelectedCharMenu()->SetPartyMenu( ( CGUISelectedCharMenu::PartyMenu )NONE );
	}
	else if( GetPartyMemberType() == PMT_LEADER )
	{
		if( pi_pPlayer->GetPartyMemberType() == PMT_NONE )
			_GetSelectedCharMenu()->SetPartyMenu( ( CGUISelectedCharMenu::PartyMenu )INVITE );
		else
			_GetSelectedCharMenu()->SetPartyMenu( ( CGUISelectedCharMenu::PartyMenu )NONE );
	}
	else
	{
		_GetSelectedCharMenu()->SetPartyMenu( ( CGUISelectedCharMenu::PartyMenu )NONE );
	}

	// 좌표 세팅
	Vector4f l_vecPos;
	_GetRenderNamePos( pi_pPlayer, l_vecPos, strlen( pi_pPlayer->GetName() ) );
	_GetSelectedCharMenu()->SetPos( l_vecPos[0] + 50, l_vecPos[1] );

	// set target
	_GetTargetMgr()->SetTarget( pi_pPlayer );	// SetTarget에서 SetRenderCharMenu를 FALSE로 만들기 때문에 SetTarget은 SetRenderCharMenu전에 호출되어야 한다.

	// open
	pi_pPlayer->SetRenderCharMenu( TRUE );
	_GetSelectedCharMenu()->Open();

	return TRUE;
}

// 선택된 캐릭터의 메뉴 닫기
BOOL
CAvatar::UI_CloseSelectedCharMenu( void )
{
	//SetTarget( NULL );
	_GetSelectedCharMenu()->Close();

	return TRUE;
}*/