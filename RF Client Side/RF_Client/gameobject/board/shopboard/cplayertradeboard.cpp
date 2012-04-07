////////////////////////////////////////////////////////////////////////////////
//
// CPlayerTradeBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../Manager/CCharacterMgr.h"
#include "../../Manager/CTargetMgr.h"
#include "../Common/CBoardManager.h"

#include "../../../Network/NetMsgProcessor/ItemTrade/CNetMsgProcessor_ItemTradePlayer.h"

#include "../../../UserInterface/GUIController/GUIMessageBox.h"
#include "../../../UserInterface/GameWindow/EtcWin/GUIMiniMapWindow.h"


////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void DisplayItemInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode );

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
SEND_ITEM::Init( void )
{
	m_byBoardType		= ID_INVALID;
	m_byItemPackNo		= ID_INVALID;
	m_byItemSocketNo	= ID_INVALID;
}

void
SEND_LIST::Init( void )
{
	m_byMaxItemNum		= 0;
	m_byTotalItemNum	= 0;

	m_dwDalant			= 0;
	m_dwGold			= 0;

	m_bIsLock			= FALSE;
	m_bIsAccept			= FALSE;
}

void
RECEIVE_LIST::Init( void )
{
	m_byMaxItemNum		= 0;
	m_byTotalItemNum	= 0;

	m_dwStartIndex		= 0;

	m_dwDalant			= 0;
	m_dwGold			= 0;

	m_bIsLock			= 0;
	m_bIsAccept			= 0;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CPlayerTradeBoard::CPlayerTradeBoard()
{
	m_pNetMsgProcessor_ItemTradePlayer	= 
			static_cast< CNetMsgProcessor_ItemTradePlayer * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_ITEM_TRADE_PLAYER ) );	

	Init();
}

CPlayerTradeBoard::~CPlayerTradeBoard()
{

}

void
CPlayerTradeBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiTradeWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );	

	m_uiWindow->SetPos( _GetUIScreen()->GetSizeX() - m_uiWindow->GetSizeX() * 2 - 15, _GetUIScreen()->GetPosY() );
}


void
CPlayerTradeBoard::Init( void )
{
	m_dwTradeCharType			= ID_INVALID;
	m_dwTradeCharIndex			= ID_INVALID;
	m_dwTradeCharServerIndex	= ID_INVALID;

	m_bIsTrading				= FALSE;

	m_bIsRequestOfAvatar		= FALSE;
	m_dwTradeRequestTime		= 0;

	m_bTradeByHotKey			= FALSE;
}

void		
CPlayerTradeBoard::ClearTrade( void )
{
	Init();
	m_pSendList.Init();
	m_pReceiveList.Init();

	SetLockSendList( FALSE );
	SetLockReceiveList( FALSE );
	SetAcceptSendList( FALSE );
	SetAcceptReceiveList( FALSE );

	m_uiTradeWindow.SetAvatarDalant( 0 );
	m_uiTradeWindow.SetAvatarGold( 0 );		
	m_uiTradeWindow.SetOtherDalant( 0 );
	m_uiTradeWindow.SetOtherGold( 0 );		


	SetTrading( FALSE );

	for( int i = 0; i < MAX_PLAYER_TRADE_ITEM_NUM; ++i )
	{
		if( !m_pSendList.m_listItem[i].m_clsItem.IsEmpty() )
		{
			// set ui
			CItemBoard * pItemBoard = static_cast< CItemBoard * >( _GetGBM()->GetBoard( m_pSendList.m_listItem[i].m_byBoardType ) );
			pItemBoard->UI_SetUselessSocket( m_pSendList.m_listItem[i].m_byItemPackNo, m_pSendList.m_listItem[i].m_byItemSocketNo, FALSE );
		}

		m_pSendList.m_listItem[i].m_clsItem.SetEmpty();
		m_pReceiveList.m_listItem[i].SetEmpty();

		m_uiTradeWindow.TakeOutItem_Avatar( i );
		m_uiTradeWindow.TakeOutItem_Other( i );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL	
CPlayerTradeBoard::Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	return TRUE;
}

BOOL		
CPlayerTradeBoard::Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	return TRUE;
}

BOOL
CPlayerTradeBoard::Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity )
{
	return FALSE;
}

BOOL
CPlayerTradeBoard::Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject )
{	
	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPlayerTradeBoard::UI_CheckBeforeAdd_CheckEmptySocket( void )
{
	return TRUE;
}

BOOL
CPlayerTradeBoard::UI_CheckBeforeAdd_CheckHoverObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeAdd_CheckHoverObject() )
		return FALSE;

	CBoardItem * pHoverItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pHoverItem->IsEqualItemType( BOT_BOARD_ITEM ) )
		return FALSE;

	return TRUE;
}

BOOL
CPlayerTradeBoard::UI_AddRequest_ProcessObject( void )
{	
	// select request	
	CBoardItem * pHoverItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );

	if( !SelectItem_Request( _GetGBM()->GetOrgBoardType(), _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), pHoverItem, FALSE ) )
	{
		_GetGBM()->ReturnItemToItemBoard( _GetGBM()->GetOrgBoardType(),
										  _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), pHoverItem );
		_GetGBM()->SetHoverObject( NULL );

		return FALSE;
	}

	return TRUE;
}
	
void	
CPlayerTradeBoard::UI_AddResult_ProcessObject( void )
{
	// add selected item to board
	SelectItem_Result( TRUE );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPlayerTradeBoard::UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity )
{
	CBoardItem * pOrgItem = &m_pSendList.m_listItem[_GetGBM()->GetOrgSocketNo()].m_clsItem;

	if( pOrgItem->IsEmpty() )
		return FALSE;

	return TRUE;
}

BOOL
CPlayerTradeBoard::UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity )
{	
	// deselect request
	DeselectItem_Request( _GetGBM()->GetOrgSocketNo() );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CPlayerTradeBoard::TradeRequest( void )
{
	if( IsTrading() )
		return FALSE;
	
	CCharacter * pTarget = _GetTargetMgr()->GetTarget();
	if( !pTarget )
		return FALSE;

	DWORD dwCurTime = timeGetTime();
	if( ( pTarget->GetCharTypeID() == m_dwTradeCharType ) &&
		( pTarget->GetIndex() == m_dwTradeCharIndex ) )
	{
		if( ( dwCurTime - m_dwTradeRequestTime ) < TRADE_REQUEST_WAIT_TIME )
		{
			_SetSystemMsg( "%s님에게 이미 거래를 요청하였습니다. 잠시만 기다려 주십시오.", pTarget->GetName() );
			return FALSE;
		}
	}

	SetTradeChar( pTarget );

	m_pNetMsgProcessor_ItemTradePlayer->TradeAskRequest_clzo( pTarget->GetServerIndex() );
	
	_GetTargetMgr()->CloseWindow( UII_PLAYER_POPUP_MENU );
	
	m_dwTradeRequestTime = dwCurTime;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL		
CPlayerTradeBoard::SelectItem_Request( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardItem * pi_pItem, BOOL pi_bTradeByHotKey )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;	

	if( !pi_pItem )
		return FALSE;

	if( m_pSendList.m_bIsLock )
		return FALSE;	

	if( !SelectItem_Request_CheckCondition( pi_pItem ) )
		return FALSE;

	if( !SelectItem_Request_CheckEmptySocket() )
		return FALSE;

	if( !SelectItem_Request_AddCurSendItem( pi_byBoardType, pi_byPackNo, pi_bySocketNo, pi_pItem ) )
		return FALSE;	

	m_bTradeByHotKey = pi_bTradeByHotKey;

	return TRUE;
}

BOOL
CPlayerTradeBoard::SelectItem_Request_CheckCondition( CBoardItem * pi_pItem )
{
	if( !pi_pItem )
		return FALSE;

	if( !pi_pItem->IsExchangable() )
	{
		_SetSystemMsg( "아이템 트레이드가 불가능한 아이템 입니다." );
		return FALSE;
	}
	else if( pi_pItem->IsEqualItemKind( IEPT_UNIT ) )
	{
		_SetSystemMsg( "현재 기갑 장비 아이템은 트레이드할 수 없습니다." );
		return FALSE;
	}

	if( pi_pItem->GetItemKind() == IEPT_MAP )
		_GetMinimapWindow()->Close();

	// 포션등의 경우에는 이미 동일한 아이템이 있는지를 검사
	for( int i = 0; i < MAX_PLAYER_TRADE_ITEM_NUM; ++i )
	{
		if( m_pSendList.m_listItem[i].m_clsItem.GetIndex() == pi_pItem->GetIndex() )
		{
			_SetSystemMsg( "거래 리스트에 이미 동일한 아이템이 있습니다." );
			return FALSE;
		}
	}

	return TRUE;	
}

BOOL
CPlayerTradeBoard::SelectItem_Request_CheckEmptySocket( void )
{
	// 상대방에게 줄 수 있는 최대 아이템 갯수 체크
	if( ( m_pSendList.m_byTotalItemNum + 1 ) > m_pSendList.m_byMaxItemNum )
	{
		_SetSystemMsg( "거래를 하는데 필요한 공간이 확보되지 않았습니다." );
		return FALSE;
	}

	// 빈 공간 체크
	m_bySelectSocketNo = 0xFF;
	for( int i = 0; i < MAX_PLAYER_TRADE_ITEM_NUM; ++i )
	{
		if( m_pSendList.m_listItem[i].m_clsItem.IsEmpty() )
		{
			m_bySelectSocketNo = i;
			break;
		}
	}

	if( m_bySelectSocketNo == 0xFF )
	{
		_SetSystemMsg( "거래를 하는데 필요한 공간이 확보되지 않았습니다." );
		return FALSE;
	}

	return TRUE;
}

BOOL
CPlayerTradeBoard::SelectItem_Request_AddCurSendItem( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardItem * pi_pItem )
{
	// set data
	m_stCurSendItem.m_byBoardType		= pi_byBoardType;
	m_stCurSendItem.m_byItemPackNo		= pi_byPackNo;
	m_stCurSendItem.m_byItemSocketNo	= pi_bySocketNo;
	memcpy( &m_stCurSendItem.m_clsItem, pi_pItem, sizeof( CBoardItem ) );

	m_pNetMsgProcessor_ItemTradePlayer->TradeAddRequest_clzo();

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPlayerTradeBoard::SelectItem_Result( BOOL pi_bIsSuccess )
{
	if( pi_bIsSuccess )
	{
		if( m_stCurSendItem.m_clsItem.IsEmpty() )
			return FALSE;

		SelectItem_Result_UnsetOrgItem();
		SelectItem_Result_AddItem();		
	}

	return TRUE;
}

void
CPlayerTradeBoard::SelectItem_Result_AddItem( void )
{
	// set data
	memcpy( &m_pSendList.m_listItem[m_bySelectSocketNo], &m_stCurSendItem, sizeof( SEND_ITEM ) );
	
	// set ui
	m_uiTradeWindow.TakeInItem_Avatar( m_bySelectSocketNo,
									   m_stCurSendItem.m_clsItem.GetMeshID(), 
									   m_stCurSendItem.m_clsItem.GetIconID(),
									   m_stCurSendItem.m_clsItem.GetQuantity() );
	m_pSendList.m_listItem[m_bySelectSocketNo].m_clsItem.SetIcon( m_uiTradeWindow.GetItem_Avatar( m_bySelectSocketNo ) );

	CItemBoard * pItemBoard = static_cast< CItemBoard * >( _GetGBM()->GetBoard( m_stCurSendItem.m_byBoardType ) );
	pItemBoard->UI_SetUselessSocket( m_stCurSendItem.m_byItemPackNo, m_stCurSendItem.m_byItemSocketNo, TRUE );


	m_stCurSendItem.m_clsItem.SetEmpty();

	++m_pSendList.m_byTotalItemNum;	
}

void	
CPlayerTradeBoard::SelectItem_Result_UnsetOrgItem( void )
{
	if( m_bTradeByHotKey )
	{
		CObjectBoard * pOrgBoard = static_cast< CObjectBoard * >( _GetGBM()->GetBoard( m_stCurSendItem.m_byBoardType ) );
		pOrgBoard->Remove( m_stCurSendItem.m_byItemPackNo, m_stCurSendItem.m_byItemSocketNo, m_stCurSendItem.m_clsItem.GetQuantity() );
	}
	else
	{
		_GetGBM()->SetHoverObject( NULL );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPlayerTradeBoard::DeselectItem_Request( BYTE pi_bySocketNo )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;

	if( m_pSendList.m_bIsLock )
		return FALSE;

	if( m_pSendList.m_listItem[pi_bySocketNo].m_clsItem.IsEmpty() )
		return FALSE;

	m_bySelectSocketNo	= pi_bySocketNo;
	
	m_pNetMsgProcessor_ItemTradePlayer->TradeDelRequest_clzo();

	return TRUE;
}

BOOL
CPlayerTradeBoard::DeselectItem_Result( BOOL pi_bIsSuccess )
{
	if( pi_bIsSuccess )
	{
		ReturnItemToItemBoard( m_bySelectSocketNo );

		m_pSendList.m_listItem[m_bySelectSocketNo].m_clsItem.SetEmpty();		

		--m_pSendList.m_byTotalItemNum;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CPlayerTradeBoard::AddReceiveItem( CBoardItem * pi_pItem, BYTE pi_bySocketNo )
{
	if( !pi_pItem )
		return FALSE;

	memcpy( &m_pReceiveList.m_listItem[pi_bySocketNo], pi_pItem, sizeof( CBoardItem ) );
	m_bySelectSocketNo = pi_bySocketNo;
	++m_pReceiveList.m_byTotalItemNum;

	// set ui
	m_uiTradeWindow.TakeInItem_Other( pi_bySocketNo,
									  m_pReceiveList.m_listItem[pi_bySocketNo].GetMeshID(),
									  m_pReceiveList.m_listItem[pi_bySocketNo].GetIconID(),
									  m_pReceiveList.m_listItem[pi_bySocketNo].GetQuantity() );

	m_pReceiveList.m_listItem[pi_bySocketNo].SetIcon( m_uiTradeWindow.GetItem_Other( pi_bySocketNo ) );


	return TRUE;
}

BOOL
CPlayerTradeBoard::RemoveReceiveItem( BYTE pi_bySocketNo )
{
	m_pReceiveList.m_listItem[pi_bySocketNo].SetEmpty();
	m_bySelectSocketNo = pi_bySocketNo;
	--m_pReceiveList.m_byTotalItemNum;

	m_uiTradeWindow.TakeOutItem_Other( pi_bySocketNo );

	return TRUE;
}

void
CPlayerTradeBoard::SetReceiveDalant( DWORD pi_dwDalant )
{
	m_pReceiveList.m_dwDalant = pi_dwDalant;
	m_uiTradeWindow.SetOtherDalant( pi_dwDalant );
}

void
CPlayerTradeBoard::SetReceiveGold( DWORD pi_dwGold )
{
	m_pReceiveList.m_dwGold = pi_dwGold;
	m_uiTradeWindow.SetOtherGold( pi_dwGold );
}


////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CPlayerTradeBoard::RecognizeLock( void )
{
	if( m_pSendList.m_bIsLock )
		return FALSE;

	m_pSendList.m_dwDalant	= m_uiTradeWindow.GetAvatarDalant();
	m_pSendList.m_dwGold	= m_uiTradeWindow.GetAvatarGold();

	if( m_pSendList.m_dwDalant > 0 )
		m_pNetMsgProcessor_ItemTradePlayer->TradeBetRequest_clzo( IMT_DALANT, m_pSendList.m_dwDalant );
	if( m_pSendList.m_dwGold > 0 )
		m_pNetMsgProcessor_ItemTradePlayer->TradeBetRequest_clzo( IMT_GOLD, m_pSendList.m_dwGold );

	m_pNetMsgProcessor_ItemTradePlayer->TradeLockRequest_clzo();

	m_pSendList.m_bIsLock = TRUE;

	return TRUE;
}

BOOL
CPlayerTradeBoard::RecognizeTrade( void )
{
	if( m_pSendList.m_bIsAccept )
		return FALSE;

	if( m_pSendList.m_bIsLock && m_pReceiveList.m_bIsLock )
	{
		m_pNetMsgProcessor_ItemTradePlayer->TradeOkRequest_clzo();

		m_pSendList.m_bIsAccept = TRUE;
	}
	else
	{
		_SetSystemMsg( "두 사람 모두 거래 리스트를 잠궈야만 승인할 수 있습니다." );
		return FALSE;
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPlayerTradeBoard::CompleteTrade( BOOL pi_bIsSuccess, DWORD pi_dwStartIndex )
{
	if( pi_bIsSuccess )
	{
		CompleteTrade_SetSendList();

		CompleteTrade_SetReceiveList( pi_dwStartIndex );		
	}

	ClearTrade();
	m_uiTradeWindow.Close();

	return pi_bIsSuccess;
}

void
CPlayerTradeBoard::CompleteTrade_SetSendList( void )
{
	_GetAvatar()->SetDalant( _GetAvatar()->GetDalant() - m_pSendList.m_dwDalant );
	_GetAvatar()->SetGold( _GetAvatar()->GetGold() - m_pSendList.m_dwGold );
}

void
CPlayerTradeBoard::CompleteTrade_SetReceiveList( DWORD pi_dwStartIndex )
{
	// Receive Item
	BYTE	l_byEmptyPackNo, l_byEmptySocketNo;
	DWORD	l_dwItemIndex = pi_dwStartIndex;
	for( int i = 0; i < MAX_PLAYER_TRADE_ITEM_NUM; ++i, ++l_dwItemIndex )
	{
		if( m_pReceiveList.m_listItem[i].IsEmpty() )
			continue;

		m_pReceiveList.m_listItem[i].SetIndex( l_dwItemIndex );
		m_pReceiveList.m_listItem[i].SetServerIndex( l_dwItemIndex );

		// set inventory
		if( !_GetInventoryBoard()->FindEmptySocket( l_byEmptyPackNo, l_byEmptySocketNo ) )
			continue;

		_GetInventoryBoard()->Add( l_byEmptyPackNo, l_byEmptySocketNo, &m_pReceiveList.m_listItem[i] );
	}

	_GetAvatar()->SetDalant( _GetAvatar()->GetDalant() + m_pReceiveList.m_dwDalant );
	_GetAvatar()->SetGold( _GetAvatar()->GetGold() + m_pReceiveList.m_dwGold );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPlayerTradeBoard::CancelTrade( void )
{
	ReturnItemToItemBoard();

	CloseWindow();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPlayerTradeBoard::ReturnItemToItemBoard( void )
{
	for( int i = 0; i < MAX_PLAYER_TRADE_ITEM_NUM; ++i )
		ReturnItemToItemBoard( i );

	ClearTrade();
}

void
CPlayerTradeBoard::ReturnItemToItemBoard( BYTE pi_bySocketNo )
{
	if( m_pSendList.m_listItem[pi_bySocketNo].m_clsItem.IsEmpty() )
		return;

	_GetGBM()->ReturnItemToItemBoard( m_pSendList.m_listItem[pi_bySocketNo].m_byBoardType,
									  m_pSendList.m_listItem[pi_bySocketNo].m_byItemPackNo,
									  m_pSendList.m_listItem[pi_bySocketNo].m_byItemSocketNo,
									  &m_pSendList.m_listItem[pi_bySocketNo].m_clsItem );

	// set ui
	m_uiTradeWindow.TakeOutItem_Avatar( pi_bySocketNo );
	
	CItemBoard * pItemBoard = static_cast< CItemBoard * >( _GetGBM()->GetBoard( m_pSendList.m_listItem[pi_bySocketNo].m_byBoardType ) );
	pItemBoard->UI_SetUselessSocket( m_pSendList.m_listItem[pi_bySocketNo].m_byItemPackNo,
									 m_pSendList.m_listItem[pi_bySocketNo].m_byItemSocketNo,
									 FALSE );
}

BOOL
CPlayerTradeBoard::IsTradingItem( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	if( GetTradeItem( pi_byBoardType, pi_byPackNo, pi_bySocketNo ) )
		return TRUE;

	return FALSE;
}

CBoardItem *
CPlayerTradeBoard::GetTradeItem( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	for( int i = 0; i < MAX_PLAYER_TRADE_ITEM_NUM; ++i )
	{
		if( m_pSendList.m_listItem[i].m_clsItem.IsEmpty() )
			continue;

		if( ( m_pSendList.m_listItem[i].m_byBoardType == pi_byBoardType ) &&
			( m_pSendList.m_listItem[i].m_byItemPackNo == pi_byPackNo ) &&
			( m_pSendList.m_listItem[i].m_byItemSocketNo == pi_bySocketNo ) )
			return &m_pSendList.m_listItem[i].m_clsItem;
	}

	return NULL;
}

CBoardItem	*
CPlayerTradeBoard::GetTradeItemByIndex( DWORD pi_dwIndex )
{
	for( int i = 0; i < MAX_PLAYER_TRADE_ITEM_NUM; ++i )
	{
		if( m_pSendList.m_listItem[i].m_clsItem.IsEmpty() )
			continue;

		if( m_pSendList.m_listItem[i].m_clsItem.IsEqual( pi_dwIndex ) )
			return &m_pSendList.m_listItem[i].m_clsItem;
	}

	return NULL;
}

// =============================================================================

BYTE
CPlayerTradeBoard::GetEmptySocketForTrade( void )
{
	BYTE byEmptySocketNum = m_pReceiveList.m_byMaxItemNum;

	for( int i = 0; i < MAX_PLAYER_TRADE_ITEM_NUM; ++i )
	{
		if( m_pSendList.m_listItem[i].m_clsItem.IsEmpty() )
			continue;

		if( !_GetGBM()->GetObjectByIndex( m_pSendList.m_listItem[i].m_byBoardType,
										  m_pSendList.m_listItem[i].m_clsItem.GetIndex() ) )
			++byEmptySocketNum;
	}

	return byEmptySocketNum;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPlayerTradeBoard::SetTradeChar( CCharacter * pi_pCharacter )
{
	if( pi_pCharacter )
	{
		m_dwTradeCharType			= pi_pCharacter->GetCharTypeID();
		m_dwTradeCharIndex			= pi_pCharacter->GetIndex();
		m_dwTradeCharServerIndex	= pi_pCharacter->GetServerIndex();
	}
}

CCharacter *
CPlayerTradeBoard::GetTradeChar( void )
{
	return _GetCharMgr()->GetCharacter( m_dwTradeCharType, m_dwTradeCharIndex );
}

void
CPlayerTradeBoard::SetTrading( BOOL pi_bFlag )
{
	m_bIsTrading = pi_bFlag;

	if( pi_bFlag )
		_GetAvatar()->Disable( ROSF_MOVABLE );
	else
		_GetAvatar()->Enable( ROSF_MOVABLE );
}

// =============================================================================

void		
CPlayerTradeBoard::SetMaxSendNum( BYTE pi_byMaxNum )
{
	m_pSendList.m_byMaxItemNum = pi_byMaxNum;

	m_uiTradeWindow.SetEnableSocketNum_Avatar( pi_byMaxNum );
}

void
CPlayerTradeBoard::SetMaxReceiveNum( BYTE pi_byMaxNum )
{
	m_pReceiveList.m_byMaxItemNum = pi_byMaxNum;

	m_uiTradeWindow.SetEnableSocketNum_Other( pi_byMaxNum );
}

// =============================================================================

void
CPlayerTradeBoard::SetLockSendList( BOOL pi_bFlag )
{
	m_pSendList.m_bIsLock = pi_bFlag;

	m_uiTradeWindow.SetLock_Avatar( pi_bFlag );
}

void
CPlayerTradeBoard::SetLockReceiveList( BOOL pi_bFlag )
{
	m_pReceiveList.m_bIsLock = pi_bFlag;

	m_uiTradeWindow.SetLock_Other( pi_bFlag );
}

void
CPlayerTradeBoard::SetAcceptSendList( BOOL pi_bFlag )
{
	m_pSendList.m_bIsAccept = pi_bFlag;

	m_uiTradeWindow.SetAccept_Avatar( pi_bFlag );
}

void
CPlayerTradeBoard::SetAcceptReceiveList( BOOL pi_bFlag )
{
	m_pReceiveList.m_bIsAccept = pi_bFlag;

	m_uiTradeWindow.SetAccept_Other( pi_bFlag );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

LRESULT
CPlayerTradeBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UI_MOUSE_MSG	stRecvMsg( lParam );

	switch( uMsg )
	{
		case WM_PLAYER_TRADE_REQUEST		:
			TradeRequest();
			break;

		case WM_ACCEPT_TRADE_REQUEST		:
			m_pNetMsgProcessor_ItemTradePlayer->TradeAnswerRequest_clzo();
			break;

		case WM_BOARD_ADD_OBJECT			:
			UI_AddRequest( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
			break;

		case WM_BOARD_REMOVE_OBJECT			:
			UI_RemoveRequest( 0, stRecvMsg.m_bySocketNo, stRecvMsg.m_byQuantity );			
			break;

		case WM_BOARD_TRADE_HOT_KEY			:
			DeselectItem_Request( stRecvMsg.m_bySocketNo );
			break;

		case WM_PLAYER_TRADE_BOARD_LOCK		:
			RecognizeLock();
			break;

		case WM_PLAYER_TRADE_BOARD_ACCEPT	:
			RecognizeTrade();
			break;

		case WM_CLOSING_WINDOW				:
			// trade cancel
			m_pNetMsgProcessor_ItemTradePlayer->TradeCancleRequest_clzo();
			return 1;

		case WM_BOARD_HOVER_OBJECT			:
			{
				if( stRecvMsg.m_bySocketNo >= MAX_PLAYER_TRADE_ITEM_NUM )
					break;

				CBoardItem * pItem = NULL;
				if( stRecvMsg.m_byPackNo == 0 )
				{
					pItem = GetSendItem( stRecvMsg.m_bySocketNo );
				}
				else if( stRecvMsg.m_byPackNo == 1 )
				{
					pItem = GetReceiveItem( stRecvMsg.m_bySocketNo );
				}
				
				if( pItem && !pItem->IsEmpty() )
					DisplayItemInfo( pItem, 0xFF );
			}
			break;
	}

	return CObjectBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}