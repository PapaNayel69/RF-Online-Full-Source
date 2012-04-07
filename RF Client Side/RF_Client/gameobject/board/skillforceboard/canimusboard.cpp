////////////////////////////////////////////////////////////////////////////////
//
// CBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../../Manager/CCharacterMgr.h"
#include "../../Manager/CTargetMgr.h"
#include "../Common/CBoardManager.h"
#include "../General/CPartyBoard.h"

#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_ItemArrangement.h"
#include "../../../Network/NetMsgProcessor/Summon/CNetMsgProcessor_Summon.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void	DisplayAnimusInfo( CAnimusItem * pi_AnimusItem );

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CAnimusControlBoard::CAnimusControlBoard()
{
	m_pAnimusChar			= NULL;
	m_pAnimusItem			= NULL;
	m_bIsRequestResummon	= FALSE;

	m_pNMP_Summon			= static_cast< CNetMsgProcessor_Summon * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_SUMMON ) );
}

CAnimusControlBoard::~CAnimusControlBoard()
{
}

void
CAnimusControlBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiAnimusControlBoard;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAnimusControlBoard::FrameMove( void )
{
	CBaseBoard::FrameMove();

	// 재소환 요청 처리
	if( m_bIsRequestResummon )
	{
		// 맵 이동 중에는 소환 불가능
		if( !_GetAvatar()->IsMovingMap() )
		{
			m_pNMP_Summon->AnimusRecallRequest_clzo( m_pAnimusItem );
			m_bIsRequestResummon = FALSE;
		}
	}
}

LRESULT
CAnimusControlBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_ANIMUSCONTROL_BOARD_COMMAND :
			CommandToAnimus( lParam );
			break;
	}

	return CBaseBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAnimusControlBoard::CommandToAnimus( BYTE pi_byCommand )
{
	if( pi_byCommand == CGUIMobControl::Cmd_Target )
	{
		CCharacter * pTarget = _GetTargetMgr()->GetTarget();
		if( !pTarget )
		{
			_SetSystemMsg( "공격할 타겟을 먼저 선택해 주십시오." );
			return FALSE;
		}

		if( _GetAvatar()->IsMyCompanion( pTarget ) )
		{
			_SetSystemMsg( "우호적인 캐릭터는 공격할 수 없습니다. 적 캐릭터를 선택해 주십시오." );
			return FALSE;
		}

		return m_pNMP_Summon->AnimusTargetRequest_clzo( pTarget );
	}
	else
	{
		return m_pNMP_Summon->AnimusCommandRequest_clzo( pi_byCommand - CGUIMobControl::Cmd_Action );
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAnimusControlBoard::SetAnimusChar( CAnimus * pi_pAnimus )
{
	m_pAnimusChar = pi_pAnimus;

	if( m_pAnimusChar && m_pAnimusItem )
	{
		m_pAnimusChar->SetHP( m_pAnimusItem->GetHP() );
		m_pAnimusChar->SetFP( m_pAnimusItem->GetFP() );
	}
}

void
CAnimusControlBoard::SetAnimusItem( CAnimusItem * pi_pItem )
{
	m_pAnimusItem = pi_pItem;

	if( pi_pItem )
	{
		m_uiAnimusControlBoard.SetMobName( pi_pItem->GetName() );
		m_uiAnimusControlBoard.SetHP( pi_pItem->GetHP(), pi_pItem->GetMaxHP() );
		m_uiAnimusControlBoard.SetFP( pi_pItem->GetFP(), pi_pItem->GetMaxFP() );
		m_uiAnimusControlBoard.SetLevel( pi_pItem->GetLevel() );
	}
	else
	{
		m_uiAnimusControlBoard.SetMobName( "NONE" );
		m_uiAnimusControlBoard.SetHP( 0, 0 );
		m_uiAnimusControlBoard.SetFP( 0, 0 );
		m_uiAnimusControlBoard.SetLevel( 1 );
	}
}

void
CAnimusControlBoard::SetExp( DWORD pi_dwExp )
{
	if( m_pAnimusItem )
	{
		m_pAnimusItem->SetExp( pi_dwExp );
		m_uiAnimusControlBoard.SetExp( (float)m_pAnimusItem->GetExpByLevel() / m_pAnimusItem->GetMaxExpByLevel() );

		m_uiAnimusControlBoard.SetLevel( m_pAnimusItem->GetLevel() );
	}
}

void
CAnimusControlBoard::SetHP( DWORD pi_dwHP )
{
	if( m_pAnimusItem )
	{
		m_pAnimusItem->SetHP( pi_dwHP );
		m_uiAnimusControlBoard.SetHP( pi_dwHP, m_pAnimusItem->GetMaxHP() );
	}
}

void
CAnimusControlBoard::SetFP( DWORD pi_dwFP )
{
	if( m_pAnimusItem )
	{
		m_pAnimusItem->SetFP( pi_dwFP );
		m_uiAnimusControlBoard.SetFP( pi_dwFP, m_pAnimusItem->GetMaxFP() );
	}
}

BOOL
CAnimusControlBoard::IsEqualSummonAnimusItem( CAnimusItem * pi_pInputItem )
{
	if( !m_pAnimusItem )
		return FALSE;

	return m_pAnimusItem->IsEqual( pi_pInputItem );
}

BOOL
CAnimusControlBoard::IsEqualSummonAnimusItem( DWORD pi_dwIndex )
{
	if( !m_pAnimusItem )
		return FALSE;

	return m_pAnimusItem->IsEqual( pi_dwIndex );
}

BOOL
CAnimusControlBoard::IsEqualSummonAnimusChar( CAnimus * pi_pInputChar )
{
	if( !m_pAnimusChar )
		return FALSE;

	return m_pAnimusChar->IsEqual( pi_pInputChar );
}

BOOL
CAnimusControlBoard::IsEqualSummonAnimusChar( DWORD pi_dwIndex )
{
	if( !m_pAnimusChar )
		return FALSE;

	return m_pAnimusChar->IsEqual( CTI_ANIMUS, pi_dwIndex );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAnimusControlBoard::OpenWindow( void )
{
	CBaseBoard::OpenWindow();

	if( _GetPartyBoard()->IsOpen() )
		_GetPartyBoard()->SetWindowPos( 0, m_uiAnimusControlBoard.m_ptPos.y + m_uiAnimusControlBoard.m_ptSize.y );
}

void
CAnimusControlBoard::CloseWindow( void )
{
	CBaseBoard::CloseWindow();

	if( _GetPartyBoard()->IsOpen() )
		_GetPartyBoard()->SetWindowPos( 0, m_uiAnimusControlBoard.m_ptPos.y );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////      Class  Boundary       //////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CAnimusBoard::CAnimusBoard()
{
	m_pNMP_Arrange			= static_cast< CNetMsgProcessor_ItemArrangement * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_ITEM_ARRANGE ) );
	m_pNMP_Summon			= static_cast< CNetMsgProcessor_Summon * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_SUMMON ) );

	m_dwLatestSummonTime	= 0;
}

CAnimusBoard::~CAnimusBoard()
{
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAnimusBoard::CreateBoardObject( void )
{
	if( !_GetAvatar()->IsEqualRace( CRT_CORA_MALE ) )
		return;

	AddPack( 0 );

	CAnimusItem		stAnimusItem;
	ANIMUS_DATA *	pData;
	for( int i = 0; i < _GetItemDataMgr()->GetTotalDataNum( IEPT_ANIMUS ); ++i )
	{
		pData = ( ANIMUS_DATA * )_GetItemDataMgr()->GetData( IEPT_ANIMUS, i );
		if( !pData )
			continue;

		stAnimusItem.SetData( pData );

		stAnimusItem.SetIndex( pData->GetDTIndex() | 0xF0000000 );
		stAnimusItem.SetName( pData->GetName() );

		Add( 0, i, &stAnimusItem );
	}
}

void
CAnimusBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiAnimusWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CAnimusBoard::Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	CBoardItem *	pOrgBoardItem	= static_cast< CBoardItem * >( pi_pOrgObject );
	CAnimusItem *	pExistBoardItem	= static_cast< CAnimusItem * >( GetObject( pi_byTarPackNo, pi_byTarSocketNo ) );
	if( !pExistBoardItem )
		return FALSE;

	if( !pExistBoardItem->IsSameKind( pOrgBoardItem ) )
		return FALSE;

	return TRUE;
}

BOOL
CAnimusBoard::Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	CAnimusItem * pNewItem = static_cast< CAnimusItem * >( pi_pOrgObject->Clone() );
	if( !pNewItem )
		return FALSE;

	// set ui - take in item
	_recall_info	uiItem;
	ZeroMemory( &uiItem, sizeof( _recall_info ) );
	uiItem.dwIndex		= pNewItem->GetIndex();
	uiItem.pName		= pNewItem->GetName();
	uiItem.dwItemID		= pNewItem->GetMeshID();
	uiItem.dwSpriteID	= pNewItem->GetIconID();
	uiItem.byLevel		= 1;
	m_uiAnimusWindow.TakeInItem( &uiItem );

	// set ui - get icon board
	pNewItem->SetIconBoard( m_uiAnimusWindow.GetRecallUnitByIndex( pNewItem->GetIndex() ) );
	pNewItem->SetState( CAnimusItem::AIS_NOT_EXIST );

	// set data
	m_listItemPack[pi_byTarPackNo].Add( pi_byTarSocketNo, pNewItem );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAnimusBoard::Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity )
{
	return FALSE;
}

BOOL
CAnimusBoard::Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject )
{
	m_uiAnimusWindow.TakeOutItem( pi_pOrgObject->GetIndex() );
	m_listItemPack[pi_byPackNo].Remove( pi_bySocketNo );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL	
CAnimusBoard::UI_CheckBeforeAdd_CheckHoverObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeAdd_CheckHoverObject() )
		return FALSE;

	CBoardObject * pHoverObject	= _GetGBM()->GetHoverObject();

	// 애니머스 
	if( pHoverObject->IsEqualItemType( BOT_ANIMUS ) )
	{
		return TRUE;
	}
	// 애니머스 리버
	else if( pHoverObject->IsEqualItemType( BOT_BOARD_ITEM ) )
	{
		CBoardItem * pHoverItem = _GetGBM()->GetHoverItem();
		if( pHoverItem->IsEqualItemKind( IEPT_ANIMUS ) )
			return TRUE;
	}

	return FALSE;
}

BOOL
CAnimusBoard::UI_CheckBeforeAdd_CheckTargetObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeAdd_CheckTargetObject() )
		return FALSE;

	if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
	{
		CAnimusItem * pTarItem	= static_cast< CAnimusItem * >( _GetGBM()->GetTarObject() );	

		// 이미 애니머스가 있는지를 체크
		if( !pTarItem->IsEqualState( CAnimusItem::AIS_NOT_EXIST ) )
		{
			_GUIMessageBox( NULL, "소환창에 이미 등록되어 있는 애니머스입니다.", 0xAAFFFFFF );
			return FALSE;
		}
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAnimusBoard::UI_AddRequest_ProcessObject( void )
{
	CAnimusItem *	pTarItem	= static_cast< CAnimusItem * >( _GetGBM()->GetTarObject() );
	CBoardItem *	pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pHoverItem->IsEqualItemKind( IEPT_ANIMUS ) )
		return FALSE;

	if( _GetGBM()->GetOrgBoardType() == IBT_ANIMUS )
	{
		UI_AddResult( TRUE );
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
	{
		if( pTarItem->IsEqualState( CAnimusItem::AIS_NOT_EXIST ) )
			m_pNMP_Arrange->AnimusInvenChangeRequest_clzo();
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_LINK )
	{
		if( pTarItem->IsEqualState( CAnimusItem::AIS_LINK ) )
			UI_AddResult( TRUE );
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void
CAnimusBoard::UI_AddResult_ProcessObject( void )
{
	CAnimusItem *	pTarItem	= static_cast< CAnimusItem * >( _GetGBM()->GetTarObject() );
	CBoardItem *	pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );

	BOOL bIsSetState = FALSE;

	if( _GetGBM()->GetOrgBoardType() == IBT_ANIMUS )
	{
		bIsSetState = TRUE;
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
	{
		_GetGBM()->ConvertBoardObject( pTarItem, pHoverItem );
		PlayWave( SND_ID_ITEM_ADD_IN_PACK );
		bIsSetState = TRUE;
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_LINK )
	{
		bIsSetState = TRUE;
	}

	if( bIsSetState )
		pTarItem->SetState( CAnimusItem::AIS_ACTIVE );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAnimusBoard::UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity )
{
	CAnimusItem *	pOrgItem = static_cast< CAnimusItem * >( _GetGBM()->GetOrgObject() );

	if( pOrgItem->IsEqualState( CAnimusItem::AIS_NOT_EXIST ) ||
		pOrgItem->IsEqualState( CAnimusItem::AIS_LINK ) )
		return FALSE;

	pOrgItem->SetState( CAnimusItem::AIS_LINK );

	_GetGBM()->SetHoverObject( pOrgItem );
	
	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAnimusBoard::UI_SwapRequest_ProcessObject( void )
{
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );
	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );

	if( _GetGBM()->GetOrgBoardType() == IBT_ANIMUS )
	{
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
	{
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_LINK )
	{
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void
CAnimusBoard::UI_SwapResult_ProcessObject( void )
{
	CBoardItem *	pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );
	CBoardItem *	pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	CBoardItem		clsSwapItem( *pTarItem );

	if( !Remove( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo(), pTarItem->GetQuantity() ) )
		return;

	if( !Add( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo(), pHoverItem ) )
		return;

	_GetGBM()->SetHoverObject( &clsSwapItem );

	_GetGBM()->AddObject( _GetGBM()->GetOrgBoardType(),
						  _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(),
						  _GetGBM()->GetHoverObject() );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAnimusBoard::UI_CheckBeforeUse_CheckOriginalObject( void )
{
	CAnimusItem * pAnimusItem = static_cast< CAnimusItem * >( _GetGBM()->GetOrgObject() );

	if( !pAnimusItem )
		return FALSE;
	if( pAnimusItem->IsUsing() )
		return FALSE;

	if( _GetAnimusControlBoard()->IsSummoned() )		// return
	{
		if( !_GetAnimusControlBoard()->IsEqualSummonAnimusItem( pAnimusItem ) )
			return FALSE;
	}
	else												// summon
	{
		if( !_GetAvatar()->IsSummonableAnimus( pAnimusItem ) )
			return FALSE;
	}

	return TRUE;
}

BOOL
CAnimusBoard::UI_UseRequest_ProcessObject( void )
{
	if( _GetAnimusControlBoard()->IsSummoned() )		// return
	{
		return m_pNMP_Summon->AnimusReturnRequest_clzo();
	}
	else												// summon
	{
		CAnimusItem * pAnimusItem = static_cast< CAnimusItem * >( _GetGBM()->GetOrgObject() );

		return m_pNMP_Summon->AnimusRecallRequest_clzo( pAnimusItem );
	}
}

// 재소환 문제때문에 UI_UseResult_ProcessObject() 에서는 처리를 하지 않고,
// UI_SummonResult() 와 UI_ReturnResult() 에서 처리한다.
void
CAnimusBoard::UI_UseResult_ProcessObject( void )
{
}

void
CAnimusBoard::UI_SummonResult( BOOL pi_bIsSuccessed )
{
	if( pi_bIsSuccessed )
	{
		// 재소환 - 현재는 처리할 것이 없슴
		if( _GetAnimusControlBoard()->IsSummoned() )
		{
		}
		// 소환
		else
		{
			CAnimusItem * pAnimusItem = static_cast< CAnimusItem * >( _GetGBM()->GetOrgObject() );
			if( !pAnimusItem )
				return;

			_GetAnimusControlBoard()->SetAnimusItem( pAnimusItem );
			_GetAnimusControlBoard()->OpenWindow();

			_SetSystemMsg( "애니머스를 소환하였습니다." );
		}

		m_dwLatestSummonTime = timeGetTime();
	}

	_GetGBM()->ClearOperation();
}

void
CAnimusBoard::UI_ReturnResult( BOOL pi_bIsSuccessed )
{
	if( pi_bIsSuccessed )
	{
		_GetAnimusControlBoard()->SetAnimusItem( NULL );
		_GetAnimusControlBoard()->CloseWindow();

		_SetSystemMsg( "애니머스를 귀환시켰습니다." );

		m_dwLatestSummonTime = timeGetTime();
	}

	_GetGBM()->ClearOperation();
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CAnimusBoard::UI_Link_By_Hotkey( DWORD pi_dwIndex )
{
	BYTE byOrgPackNo, byOrgSocketNo;

	if( !GetObjectPosition( byOrgPackNo, byOrgSocketNo, pi_dwIndex ) )
		return;	
	
	BYTE byTarSocketNo;
	if( ( byTarSocketNo = _GetLinkBoard()->FindEmptySocket() ) == 0xFF )
	{
		_SetSystemMsg( "빈 소켓이 없습니다." );
		return;
	}

	if( !UI_RemoveRequest( byOrgPackNo, byOrgSocketNo, 1 ) )
		return;

	_GetLinkBoard()->UI_AddRequest( 0, byTarSocketNo );
}

void
CAnimusBoard::Unlink( DWORD pi_dwIndex )
{
	CAnimusItem * pAnimusItem = GetAnimusByIndex( pi_dwIndex );
	if( !pAnimusItem )
		return;
	if( pAnimusItem->IsEqualState( CAnimusItem::AIS_NOT_EXIST ) )
		return;
	
	// remove link
	BYTE byPackNo, bySocketNo;
	if( !_GetLinkBoard()->GetObjectPosition( byPackNo, bySocketNo, pAnimusItem->GetIndex() ) )
		return;
	_GetLinkBoard()->Remove( byPackNo, bySocketNo );

	// set state
	pAnimusItem->SetState( CAnimusItem::AIS_ACTIVE );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CAnimusBoard::FrameMove( void )
{
	CObjectBoard::FrameMove();

	if( !m_listItemPack[0].m_bIsExist )
		return;

	// check time
			DWORD dwCurRecoveryTime		= timeGetTime();
	static	DWORD dwPrevHPRecoveryTime	= dwCurRecoveryTime;
	static	DWORD dwPrevFPRecoveryTime	= dwCurRecoveryTime;

	// recover hp & fp
	CAnimusItem *	pAnimusItem = NULL;
	int				nMaxAnimusNum = m_listItemPack[0].GetMaxSocketNum();
	for( int i = 0; i < nMaxAnimusNum; ++i )
	{
		pAnimusItem = static_cast< CAnimusItem * >( GetObject( 0, i ) );
		if( !pAnimusItem )
			continue;
		if( pAnimusItem->IsEqualState( CAnimusItem::AIS_NOT_EXIST ) ||
			pAnimusItem->IsEqualState( CAnimusItem::AIS_DEACTIVE ) )
			continue;

		// don't summoned animus
		if( !_GetAnimusControlBoard()->IsEqualSummonAnimusItem( pAnimusItem ) )
		{
			// hp
			if( ( dwCurRecoveryTime - dwPrevHPRecoveryTime ) >= pAnimusItem->GetHPRecoveryDelay() )
			{
				pAnimusItem->RecoverHP();
				dwPrevHPRecoveryTime = dwCurRecoveryTime;
			}

			// fp
			if( ( dwCurRecoveryTime - dwPrevFPRecoveryTime ) >= pAnimusItem->GetFPRecoveryDelay() )
			{
				pAnimusItem->RecoverFP();
				dwPrevFPRecoveryTime = dwCurRecoveryTime;
			}
		}
	}
}

LRESULT
CAnimusBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	BYTE byPackNo, bySocketNo;

	switch( uMsg )
	{
		case WM_BOARD_ADD_OBJECT	:
			if( GetObjectPosition( byPackNo, bySocketNo, lParam ) )
				UI_AddRequest( byPackNo, bySocketNo );
			break;

		case WM_BOARD_REMOVE_OBJECT	:
			if( GetObjectPosition( byPackNo, bySocketNo, lParam ) )
				UI_RemoveRequest( byPackNo, bySocketNo, lParam );
			break;

		case WM_BOARD_EQUIP_HOT_KEY	:
			UI_Link_By_Hotkey( lParam );
			break;

		case WM_BOARD_USE_OBJECT	:
			if( GetObjectPosition( byPackNo, bySocketNo, lParam ) )
				UI_UseRequest( byPackNo, bySocketNo );
			break;

		case WM_BOARD_HOVER_OBJECT	:
			{
				CAnimusItem * pItem = GetAnimusByIndex( lParam );
				if( !pItem )
					break;

				DisplayAnimusInfo( pItem );
			}
			break;
	}

	return CObjectBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CAnimusItem *
CAnimusBoard::GetAnimusByDTIndex( DWORD pi_dwDTIndex )
{
	CAnimusItem * pAnimusItem = NULL;

	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[i].m_byMaxSocketNum; ++j )
		{
			if( !m_listItemPack[i].m_listSocket[j] )
				continue;

			pAnimusItem = static_cast< CAnimusItem * >( m_listItemPack[i].m_listSocket[j] );
			if( pAnimusItem->GetDTIndex() == pi_dwDTIndex )
				return pAnimusItem;
		}
	}

	return NULL;
}

BOOL
CAnimusBoard::IsPassedSummonLimitTime( void )
{
#ifdef _DEVELOPER_

	if( _GetNetworkMgr()->GetAvatarGrade() >= USER_DEGREE_MGR )
		return TRUE;

#endif

	static const DWORD SUMMON_LIMIT_TIME = 30000;	// 30초 제한

	DWORD dwRemainTime = timeGetTime() - m_dwLatestSummonTime;

	if( dwRemainTime < SUMMON_LIMIT_TIME )
	{
		_SetSystemMsg( "아직 소환 기술을 사용할 수 없습니다. 잠시 기다려 주십시오 ( 대기시간 %d초 ).",
					   ( SUMMON_LIMIT_TIME - dwRemainTime ) / 1000 );
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // _NEW_UI_