////////////////////////////////////////////////////////////////////////////////
//
// CTargetMgr Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "CCharacterMgr.h"
#include "CGroundItemManager.h"

#include "../Board/General/CPartyBoard.h"
#include "CTargetMgr.h"

CTargetMgr *	g_pTargetMgr		= NULL;
CNextActionMgr*	g_pNextActionMgr	= NULL;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CTarget::CTarget()
{
	m_dwTargetID[0] = m_dwTargetID[1] = ID_INVALID;
}

void
CTarget::SetTarget( CCharacter * pi_pTarget )
{
	if( pi_pTarget )
	{
		m_dwTargetID[0] = pi_pTarget->GetCharTypeID();
		m_dwTargetID[1] = pi_pTarget->GetIndex();
	}
	else
	{
		m_dwTargetID[0] = m_dwTargetID[1] = ID_INVALID;
	}
}

void
CTarget::SetTarget( BYTE pi_byCharTypeID, DWORD pi_dwTargetIndex )
{
	m_dwTargetID[0] = pi_byCharTypeID;
	m_dwTargetID[1] = pi_dwTargetIndex;
}

CCharacter *
CTarget::GetTarget( void )
{
	return _GetCharMgr()->GetCharacter( m_dwTargetID[0], m_dwTargetID[1] );
}

BOOL
CTarget::IsEqual( CCharacter * pi_pCharacter )
{
	if( pi_pCharacter )
		return pi_pCharacter->IsEqual( m_dwTargetID[0], m_dwTargetID[1] );

	return FALSE;
}

BOOL
CTarget::IsEmpty( void )
{
	if( ( m_dwTargetID[0] == ID_INVALID ) || ( m_dwTargetID[1] == ID_INVALID ) )
		return TRUE;

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CTargetItem::CTargetItem()
{
	m_dwTargetItemIndex	= ID_INVALID;
}

void
CTargetItem::SetTargetItem( CGroundItem * pi_pTargetItem )
{
	if( pi_pTargetItem )
		m_dwTargetItemIndex = pi_pTargetItem->GetIndex();
}

CGroundItem *
CTargetItem::GetTargetItem( void )
{
	return _GetGroundItemMgr()->GetItem( m_dwTargetItemIndex );
}

BOOL
CTargetItem::IsEqual( CGroundItem * pi_pGroundItem )
{
	if( pi_pGroundItem )
		return pi_pGroundItem->IsEqual( OTI_ITEM, m_dwTargetItemIndex );

	return FALSE;
}

BOOL
CTargetItem::IsEmpty( void )
{
	if( m_dwTargetItemIndex == ID_INVALID )
		return TRUE;

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CTargetMgr::CTargetMgr()
{
	m_byTargetMode				= TMT_SELECT_TARGET;

	PursuitInfo.m_nTargetType	= PTT_NONE;
	PursuitInfo.m_nPurposeType	= PPT_NONE;
	PursuitInfo.m_dwPursuitTime	= 0;

	TargetList.m_listTarget.reserve( MAX_TARGET_NUM_IN_LIST );
	CTarget l_Target;
	for( int i = 0; i < MAX_TARGET_NUM_IN_LIST; ++i )
		TargetList.m_listTarget.push_back( l_Target );
	TargetList.m_nCurTargetNo	= 0;
}

CTargetMgr::~CTargetMgr()
{
	TargetList.m_listTarget.clear();
}

void
CTargetMgr::InitTargetInfoBar( DWORD pi_dwID, CGUIScreen * pi_pUiScreen )
{
	m_uiTargetInfoBar.Init();
	m_uiTargetInfoBar.SetID( pi_dwID );

	if( pi_pUiScreen ) pi_pUiScreen->Add( &m_uiTargetInfoBar );
}

void
CTargetMgr::InitTargetInfoWindow( DWORD pi_dwID, CGUIWindow * pi_pDockingWindow, CGUIScreen * pi_pUiScreen )
{
	m_uiTargetInfoWindow.Init();
	m_uiTargetInfoWindow.SetID( pi_dwID );
	if( pi_pDockingWindow )
	{
		m_uiTargetInfoWindow.SetPos( pi_pDockingWindow->m_ptPos.x + pi_pDockingWindow->m_ptSize.x, 0 );
		pi_pDockingWindow->AddDockingCustomer( &m_uiTargetInfoWindow );
	}
	m_uiTargetInfoWindow.EnableDockingBorder( CGUIWindow::RIGHT, TRUE );
	m_uiTargetInfoWindow.SetDockingBorder( CGUIWindow::RIGHT );

	if( pi_pUiScreen ) pi_pUiScreen->Add( &m_uiTargetInfoWindow );
}

void
CTargetMgr::InitPlayerPopupMenu( DWORD pi_dwID, CGUIScreen * pi_pUiScreen )
{
	m_uiPlayerPopupMenu.Init();
	m_uiPlayerPopupMenu.SetID( pi_dwID );

	if( pi_pUiScreen ) pi_pUiScreen->Add( &m_uiPlayerPopupMenu );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CTargetMgr::FrameMove( void )
{
}

void
CTargetMgr::Render( void )
{
	CCharacter * l_pTarget = m_Target.GetTarget();
	if( !l_pTarget )
		return;

	Vector4f	l_vecRenderPos;
	POINT		l_ptSize;

	m_uiTargetInfoBar.GetSize( l_ptSize );
	_GetRenderNamePos( l_pTarget, l_vecRenderPos, l_ptSize.x / FONT_WIDTH );
	m_uiTargetInfoBar.SetPos( l_vecRenderPos[0], l_vecRenderPos[1] - 30 );

	if( l_pTarget->IsEqualCharType( CTI_MONSTER ) )
	{
		m_uiTargetInfoBar.SetHPRate( l_pTarget->GetHPRate() );
		m_uiTargetInfoWindow.SetHP( l_pTarget->GetHPRate() );
	}
	else if( l_pTarget->IsEqualCharType( CTI_PLAYER ) )
	{
		if( _GetPartyBoard()->IsPartyMember( l_pTarget->GetIndex() ) )
		{
			m_uiTargetInfoBar.SetHPRate( l_pTarget->GetHPRate() );
			m_uiTargetInfoWindow.SetHP( l_pTarget->GetHPRate() );
		}
		else
		{
			m_uiTargetInfoBar.SetHPRate( 1.0f );
			m_uiTargetInfoWindow.SetHP( 0.0f );
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CTargetMgr::SetTarget( CCharacter * pi_pTarget )
{
	CloseAllWindow();

	m_Target.SetTarget( pi_pTarget );

	if( pi_pTarget )
	{
		m_LatestTarget.SetTarget( pi_pTarget );
		SetTarget_OpenTargetUI( pi_pTarget );
	}
}

void
CTargetMgr::SetTarget_OpenTargetUI( CCharacter * pi_pTarget )
{
	if( !pi_pTarget )
		return;

	if( pi_pTarget->IsEqualCharType( CTI_MONSTER ) || pi_pTarget->IsEqualCharType( CTI_ANIMUS ) ||
		pi_pTarget->IsEqualCharType( CTI_GUARD_TOWER ) )
		SetTarget_SetEnemyNpcUI( pi_pTarget );
	else if( pi_pTarget->IsEqualCharType( CTI_PLAYER ) )
		SetTarget_SetPlayerUI( static_cast< CPlayer * >( pi_pTarget ) );
	else if( pi_pTarget->IsEqualCharType( CTI_NPC ) )
		SetTarget_SetMerchantUI( static_cast< CNpc * >( pi_pTarget ) );
}

void
CTargetMgr::SetTarget_SetEnemyNpcUI( CCharacter * pi_pTarget )
{
	m_uiTargetInfoBar.Open();
	m_uiTargetInfoBar.SetCharName( pi_pTarget->GetName() );
	m_uiTargetInfoBar.SetHPRate( pi_pTarget->GetHPRate() );
	m_uiTargetInfoBar.SetCharNameColor( pi_pTarget->GetTextColor() );

	m_uiTargetInfoWindow.Open();
	m_uiTargetInfoWindow.SetMobName( pi_pTarget->GetName() );
	m_uiTargetInfoWindow.SetHP( pi_pTarget->GetHPRate() );
	m_uiTargetInfoWindow.SetPower( GetResultOfCombatPowerComparisonWithAvatar( pi_pTarget ) );
}

void
CTargetMgr::SetTarget_SetPlayerUI( CPlayer * pi_pTarget )
{
	m_uiTargetInfoBar.Open();
	m_uiTargetInfoBar.SetCharName( pi_pTarget->GetName() );
	m_uiTargetInfoBar.SetHPRate( 1.0f );
	m_uiTargetInfoBar.SetCharNameColor( pi_pTarget->GetTextColor() );
	m_uiTargetInfoBar.SetSelectState();

	m_uiTargetInfoWindow.Open();
	m_uiTargetInfoWindow.SetMobName( pi_pTarget->GetName() );
	m_uiTargetInfoWindow.SetHP( 0.0f );
	m_uiTargetInfoWindow.SetPower( GetResultOfCombatPowerComparisonWithAvatar( pi_pTarget ) );
}

void
CTargetMgr::SetTarget_SetMerchantUI( CNpc * pi_pTarget )
{
	m_uiTargetInfoWindow.Open();
	m_uiTargetInfoWindow.SetMobName( pi_pTarget->GetName() );
	m_uiTargetInfoWindow.SetHP( 0.0f );
	m_uiTargetInfoWindow.SetPower( 0xFF );
}

void
CTargetMgr::SetTargetTextColor( DWORD pi_dwColor )
{
	m_uiTargetInfoBar.SetCharNameColor( pi_dwColor );
}

CCharacter *
CTargetMgr::GetTarget( void )
{
	return m_Target.GetTarget();
}

BOOL
CTargetMgr::IsEqualTarget( CCharacter * pi_pCharacter )
{
	return m_Target.IsEqual( pi_pCharacter );
}

BOOL
CTargetMgr::IsEmptyTarget( void )
{
	return m_Target.IsEmpty();
}

void
CTargetMgr::ChangeTargetFromLatestTarget( void )
{
	CCharacter * l_pLatestTarget = m_LatestTarget.GetTarget();
	if( l_pLatestTarget )
		SetTarget( l_pLatestTarget );
}

void
CTargetMgr::ChangeTargetFromNextTarget( void )
{
	SetTarget( _GetNextActionMgr()->GetNextTarget() );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CTargetMgr::SetTargetItem( CGroundItem * pi_pTargetItem )
{
	m_TargetItem.SetTargetItem( pi_pTargetItem );
}

CGroundItem *
CTargetMgr::GetTargetItem( void )
{
	return m_TargetItem.GetTargetItem();
}

BOOL
CTargetMgr::IsEqualTargetItem( CGroundItem * pi_pGroundItem )
{
	return m_TargetItem.IsEqual( pi_pGroundItem );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BYTE
CTargetMgr::GetResultOfCombatPowerComparisonWithAvatar( CCharacter * pi_pTarget )
{
	if( !pi_pTarget )
		return 0xFF;

	if( pi_pTarget->IsEqualCharType( CTI_MONSTER ) )
	{
		int l_nLevelDifference = static_cast< CMonster * >( pi_pTarget )->GetLevel() - _GetAvatar()->GetLevel();
		if( l_nLevelDifference <= -6 )
			return 0;
		else if( l_nLevelDifference <= -1 )
			return 1;
		else if( l_nLevelDifference >= 5 )
			return 7;

		return ( l_nLevelDifference + 2 );
	}

	return 0xFF;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CTargetMgr::SetPursuitInfo( int pi_nTarget, int pi_nPurpose )
{
	PursuitInfo.m_nTargetType	= pi_nTarget;
	PursuitInfo.m_nPurposeType	= pi_nPurpose;
	PursuitInfo.m_dwPursuitTime	= timeGetTime();

	if( pi_nPurpose == PPT_NORMAL_ATTACK || pi_nPurpose == PPT_SKILL_FORCE )
		m_uiTargetInfoBar.SetAttackState();
	else
		m_uiTargetInfoBar.SetSelectState();
}

void
CTargetMgr::ResetPursuitInfo( void )
{
	SetPursuitInfo( PTT_NONE, PPT_NONE );
	SetTarget( NULL );
	SetTargetItem( NULL );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CTargetMgr::AddTargetToList( BYTE pi_bySocketNo, CCharacter * pi_pTarget )
{
	if( pi_bySocketNo >= MAX_TARGET_NUM_IN_LIST )
		return;

	TargetList.m_listTarget[pi_bySocketNo].SetTarget( pi_pTarget );

	if( pi_pTarget )
		TargetList.m_nCurTargetNo = pi_bySocketNo;
}

void
CTargetMgr::AddTargetToList( BYTE pi_bySocketNo, BYTE pi_byCharTypeID, DWORD pi_dwTargetIndex )
{
	if( pi_bySocketNo >= MAX_TARGET_NUM_IN_LIST )
		return;

	TargetList.m_listTarget[pi_bySocketNo].SetTarget( pi_byCharTypeID, pi_dwTargetIndex );

	if( pi_dwTargetIndex != ID_INVALID )
		TargetList.m_nCurTargetNo = pi_bySocketNo;
}

CCharacter *
CTargetMgr::GetTargetInList( BYTE pi_bySocketNo )
{
	if( pi_bySocketNo >= MAX_TARGET_NUM_IN_LIST )
		return NULL;

	return TargetList.m_listTarget[pi_bySocketNo].GetTarget();
}

CCharacter *
CTargetMgr::GetNextTargetInList( BOOL pi_bBackward )
{
	for( int i = 0; i < MAX_TARGET_NUM_IN_LIST; ++i )
	{
		if( pi_bBackward )
			TargetList.m_nCurTargetNo = ++TargetList.m_nCurTargetNo % MAX_TARGET_NUM_IN_LIST;
		else
			TargetList.m_nCurTargetNo = --TargetList.m_nCurTargetNo % MAX_TARGET_NUM_IN_LIST;

		if( TargetList.m_listTarget[TargetList.m_nCurTargetNo].IsEmpty() )
			continue;

		return TargetList.m_listTarget[TargetList.m_nCurTargetNo].GetTarget();
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CTargetMgr::OpenWindow( DWORD pi_dwWindowID )
{
	switch( pi_dwWindowID )
	{
		case UII_TARGET_INFO_BAR	: m_uiTargetInfoBar.Open(); break;
		case UII_TARGET_INFO_WINDOW	: m_uiTargetInfoWindow.Open(); break;
		case UII_PLAYER_POPUP_MENU	: SetPopupMenu(); break;
	}
}

void
CTargetMgr::CloseWindow( DWORD pi_dwWindowID )
{
	switch( pi_dwWindowID )
	{
		case UII_TARGET_INFO_BAR	: m_uiTargetInfoBar.Close(); break;
		case UII_TARGET_INFO_WINDOW	: m_uiTargetInfoWindow.Close(); break;
		case UII_PLAYER_POPUP_MENU	: m_uiPlayerPopupMenu.Close(); break;
	}
}

void
CTargetMgr::CloseAllWindow( void )
{
	m_uiTargetInfoBar.Close();
	m_uiTargetInfoWindow.Close();
	m_uiPlayerPopupMenu.Close();
}

BOOL
CTargetMgr::IsOpenableWindow( DWORD pi_dwWindowID )
{
	switch( pi_dwWindowID )
	{
		case UII_TARGET_INFO_BAR	: return m_uiTargetInfoBar.IsShow();
		case UII_TARGET_INFO_WINDOW	: return m_uiTargetInfoWindow.IsShow();
		case UII_PLAYER_POPUP_MENU	: return m_uiPlayerPopupMenu.IsShow();
	}

	return FALSE;
}

void
CTargetMgr::SetPopupMenu( void )
{
	CCharacter * pTarget = GetTarget();
	if( !pTarget )
		return;

	// 다른 종족의 플레이어면 메뉴를 열지 않는다.
	if( !_GetAvatar()->IsMyCompanion( pTarget ) )
		return;

	// set name
	m_uiPlayerPopupMenu.SetCharName( pTarget->GetName() );

	// set menu contents
	if( pTarget->IsEqualCharType( CTI_PLAYER ) )
	{
		SetPopupMenu_Player( static_cast< CPlayer * >( pTarget ) );
	}
	else if( pTarget->IsEqualCharType( CTI_UNIT ) )
	{
		SetPopupMenu_Unit( static_cast< CPlayer * >( pTarget ) );
	}
	else if( pTarget->IsEqualCharType( CTI_GUARD_TOWER ) )
	{
		SetPopupMenu_GuardTower( static_cast< CGuardTower * >( pTarget ) );
	}

	// set open
	m_uiPlayerPopupMenu.Open();

	// set position
	Vector4f vecRenderPos;
	_GetRenderNamePos( pTarget, vecRenderPos, strlen( pTarget->GetName() ) );
	m_uiPlayerPopupMenu.SetPos( vecRenderPos[0] + 50, vecRenderPos[1] );
}

void
CTargetMgr::SetPopupMenu_Unit( CPlayer * pi_pUnit )
{
	m_uiPlayerPopupMenu.SetMenuType( CGUISelectedCharMenu::UNIT );
		
	if( pi_pUnit->IsEqualLinkPlayer( _GetAvatar() ) )
	{
		m_uiPlayerPopupMenu.EnableButton( CGUISelectedCharMenu::Riding, TRUE );
		m_uiPlayerPopupMenu.EnableButton( CGUISelectedCharMenu::Traction, FALSE );
		m_uiPlayerPopupMenu.EnableButton( CGUISelectedCharMenu::Custody, TRUE );
	}
	else
	{
		m_uiPlayerPopupMenu.EnableButton( CGUISelectedCharMenu::Riding, FALSE );
		m_uiPlayerPopupMenu.EnableButton( CGUISelectedCharMenu::Traction, TRUE );
		m_uiPlayerPopupMenu.EnableButton( CGUISelectedCharMenu::Custody, FALSE );
	}
}

void
CTargetMgr::SetPopupMenu_Player( CPlayer * pi_pPlayer )
{
	m_uiPlayerPopupMenu.SetMenuType( CGUISelectedCharMenu::PLAYER );

	if( _GetAvatar()->IsEqualPartyMemberType( PMT_NONE ) )
	{
		if( pi_pPlayer->IsEqualPartyMemberType( PMT_NONE ) )
			m_uiPlayerPopupMenu.SetPartyMenu( CGUISelectedCharMenu::Invite );
		else if( pi_pPlayer->IsEqualPartyMemberType( PMT_LEADER ) )
			m_uiPlayerPopupMenu.SetPartyMenu( CGUISelectedCharMenu::Join );
		else
			m_uiPlayerPopupMenu.SetPartyMenu( CGUISelectedCharMenu::None );
	}
	else if( _GetAvatar()->IsEqualPartyMemberType( PMT_LEADER ) )
	{
		if( pi_pPlayer->IsEqualPartyMemberType( PMT_NONE ) )
			m_uiPlayerPopupMenu.SetPartyMenu( CGUISelectedCharMenu::Invite );
		else
			m_uiPlayerPopupMenu.SetPartyMenu( CGUISelectedCharMenu::None );
	}
	else
	{
		m_uiPlayerPopupMenu.SetPartyMenu( CGUISelectedCharMenu::None );
	}
}

void
CTargetMgr::SetPopupMenu_GuardTower( CGuardTower * pi_pGuardTower )
{
	m_uiPlayerPopupMenu.SetMenuType( CGUISelectedCharMenu::GUARD_TOWER );

	m_uiPlayerPopupMenu.EnableButton( CGUISelectedCharMenu::GetBack, TRUE );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNextActionMgr::CNextActionMgr()
{
	m_dwNextActionID		= ID_INVALID;
	m_bClearNextActionID	= TRUE;

	m_pNextSFItem			= NULL;
}

void
CNextActionMgr::SetEmpty( BOOL pi_bIgnoreClearFlag )
{
	if( !pi_bIgnoreClearFlag && !m_bClearNextActionID )
		return;

	m_dwNextActionID		= ID_INVALID;

	m_NextTarget.SetTarget( NULL );
	m_NextTargetItem.SetTargetItem( NULL );
	m_pNextSFItem			= NULL;
	m_dwNextExtraData		= ID_INVALID;
}