////////////////////////////////////////////////////////////////////////////////
//
// CShopTradeBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../Manager/CCharacterMgr.h"
#include "../Common/CBoardManager.h"

#include "../../../Network/NetMsgProcessor/ItemTrade/CNetMsgProcessor_ItemTrade.h"
#include "../../../Network/NetMsgProcessor/Unit/CNetMsgProcessor_Unit.h"

#include "../../../UserInterface/GUIController/GUIMessageBox.h"
#include "../../../UserInterface/GameWindow/EtcWin/GUIMiniMapWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void DisplayItemInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode );

////////////////////////////////////////////////////////////////////////////////


// 거래 형태 - pi_byTradeType
// 0 : None, 1: 구입, 2 : 판매, 3 : 수리, 4 : 교환
void
UI_DisplayTradeResultString( BYTE pi_byTradeType, BOOL pi_bIsSuccess )
{
	static const char TRADE_TYPE[5][8] = { "None", "구입", "판매", "수리", "교환" };

	static char l_pBuf[64];
	static char l_pString[512];

	ZeroMemory( l_pString, 512 );	

	if( pi_bIsSuccess )
	{		
		sprintf( l_pString, "[ 아이템 %s 성공 ]\n\n", TRADE_TYPE[pi_byTradeType] );

		
		CTradeInfoList * pTradeInfoList = _GetShopTradeBoard()->GetTradeInfoList();		
		CTradeInfoList_Iterator it;	
		TRADE_INFO * pTradeInfo;
		int cnt;
		for( cnt = 0, it = pTradeInfoList->begin(); it != pTradeInfoList->end(); ++it, ++cnt )
		{		
			pTradeInfo = *it;			
			sprintf( l_pString, "%s%d . %s ( %d개 )\n", l_pString, cnt + 1, pTradeInfo->m_clsItem.GetName(), pTradeInfo->m_clsItem.GetQuantity() );

		}		

		sprintf( l_pString, "%s\n금액 : %s - %d\n       골드 - %d",
							l_pString, _GetAvatarMoneyName(), _GetShopTradeBoard()->GetDalant(), _GetShopTradeBoard()->GetGold() );
	}
	else
	{
		sprintf( l_pString, "[ 아이템 %s 실패 ]", TRADE_TYPE[pi_byTradeType] );
	}
	
	_GUIMessageBox( NULL, l_pString, 0xD0FFFFFF, 0, DLG_STYLE_OK, -1L, TRUE );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CShopTradeBoard::CShopTradeBoard()
{
	m_pNMP_ItemTrade	= static_cast< CNetMsgProcessor_ItemTrade * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_ITEM_TRADE ) );
	m_pNMP_Unit			= static_cast< CNetMsgProcessor_Unit * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_UNIT ) );

	m_pMerchant			= NULL;

	m_byTradeMode		= STM_BUY;
	
	m_dwNpcIndex		= ID_INVALID;

	ClearTradeList();
}

CShopTradeBoard::~CShopTradeBoard()
{
	for( int i = 0; i < m_byMaxPackNum; ++i )
		m_listItemPack[i].EmptyAll();

	RemoveAllTradeInfo();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CShopTradeBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiShopWindow;	

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );

	m_uiWindow->SetPos( _GetUIScreen()->GetSizeX() - m_uiWindow->GetSizeX() * 2 - 15, _GetUIScreen()->GetPosY() );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void		
CShopTradeBoard::OpenShopWindow( DWORD pi_dwNpcIndex )
{	
	CNpc * pMerchant = ( CNpc * )_GetCharMgr()->GetCharacter( CTI_NPC, pi_dwNpcIndex );
	if( !pMerchant )
		return;	

	m_uiShopWindow.SetShopType( ( CGUIShopWindow::ShopType )pMerchant->GetMerchantType() );

	SetTradeMode( STM_BUY );

	// 이전에 npc와 같으면 창만 연다
	if( pi_dwNpcIndex == m_dwNpcIndex )
	{
		OpenWindow();
		return;
	}

	m_dwNpcIndex	= pi_dwNpcIndex;
	m_pMerchant		= pMerchant;

	CMerchantItemMgr * pItemList = pMerchant->GetShopItemList();
	if( !pItemList )
		return;	

	ClearShopItemList();

	// 상점 아이템 목록 세팅
	for( int i = 0; i < pItemList->GetTotalItemNum(); ++i )
	{		
		Add( 0, i, pItemList->GetItem( i ) );
	}			
	
	
	OpenWindow();
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CShopTradeBoard::Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	CBoardItem * pExistBoardItem= static_cast< CBoardItem * >( GetObject( pi_byTarPackNo, pi_byTarSocketNo ) );
	
	if( pi_pOrgObject != pExistBoardItem )
		return FALSE;

	return TRUE;
}
	
BOOL
CShopTradeBoard::Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	CBoardItem * pOrgItem = static_cast< CBoardItem * >( pi_pOrgObject );	

	// 성점아이템의 포인터만 저장한다.
	if( !m_listItemPack[pi_byTarPackNo].Add( pi_byTarSocketNo, pOrgItem ) )
		return FALSE;

	UI_SHOP_ITEM	uiItem;
	uiItem.dwIndex			= pOrgItem->GetIndex();
	uiItem.dwMeshID			= pOrgItem->GetMeshID();
	uiItem.dwIconID			= pOrgItem->GetIconID();
	uiItem.byUsableLevel	= pOrgItem->GetUsableLevel();

	m_uiShopWindow.TakeInBuyItem( &uiItem );

	pOrgItem->SetIcon( m_uiShopWindow.GetBuyableItem( pOrgItem->GetIndex() ) ); 	

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CShopTradeBoard::Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity )
{
	CBoardItem * pOrgItem = static_cast< CBoardItem * >( pi_pOrgObject );

	if( pOrgItem->RemoveQuantity( pi_byQuantity ) > 0 )
		return TRUE;

	return FALSE; 
}
	
BOOL
CShopTradeBoard::Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject )
{	
	m_uiShopWindow.TakeOutBuyItem( pi_pOrgObject->GetIndex() );	

	m_listItemPack[pi_byPackNo].Empty( pi_bySocketNo );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CShopTradeBoard::UI_CheckBeforeAdd_CheckHoverObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeAdd_CheckHoverObject() )
		return FALSE;

	CBoardObject * pHoverObject	= _GetGBM()->GetHoverObject();

	if( !pHoverObject->IsEqualItemType( BOT_BOARD_ITEM ) )
		return FALSE;

	if( m_byTradeMode == STM_BUY )
	{
		if( _GetGBM()->GetOrgBoardType() != IBT_SHOP )
			return FALSE;		
	}

	return TRUE;
}

BOOL	
CShopTradeBoard::UI_AddRequest_ProcessObject( void )
{
	if( m_byTradeMode == STM_BUY )
	{
		// shop item
		_GetGBM()->SetHoverObject( NULL );
	}
	else if( m_byTradeMode == STM_SELL )
	{		
		// select sell item
		CBoardItem * pHoverItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
		if( !SelectSellItem( _GetGBM()->GetOrgBoardType(), _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), pHoverItem ) )
			return FALSE;

		_GetGBM()->SetHoverObject( NULL );		
	}
	else if( m_byTradeMode == STM_REPAIR )
	{
		// select repair item
		CBoardItem * pHoverItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
		if( !SelectRepairItem( _GetGBM()->GetOrgBoardType(), _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), pHoverItem ) )
			return FALSE;

		_GetGBM()->SetHoverObject( NULL );		
	}

	return TRUE;
}

void	
CShopTradeBoard::UI_AddResult_ProcessObject( void ) 
{
	
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CShopTradeBoard::UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity )
{
	if( m_byTradeMode == STM_BUY )
	{
		if( !CObjectBoard::UI_CheckBeforeRemove_CheckOriginalObject( pi_byQuantity ) )
			return FALSE;
	}
	else
	{
		CBoardItem * pOrgItem = GetTradeItem( _GetGBM()->GetOrgSocketNo() );

		if( !pOrgItem )
			return FALSE;
		if( pOrgItem->IsUsing() )
			return FALSE;
	}

	return TRUE;
}

BOOL
CShopTradeBoard::UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity )
{
	if( !pi_byQuantity )
		return FALSE;

	CBoardItem * pHoverItem	= NULL;
	CBoardItem * pOrgItem;

	if( m_byTradeMode == STM_BUY )
	{		
		pOrgItem = static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );					

		// set hover item
		pHoverItem = static_cast< CBoardItem * >( _GetGBM()->SetHoverObject( pOrgItem ) );		
		pHoverItem->SetQuantity( pi_byQuantity );		
	}
	else
	{
		pOrgItem = GetTradeItem( _GetGBM()->GetOrgSocketNo() );
		if( !pOrgItem )
			return FALSE;

		BOOL bResult = FALSE;

		// deselect trade item
		if( m_byTradeMode == STM_SELL )
			bResult = DeselectSellItem( pOrgItem->GetIndex(), pOrgItem->GetQuantity() );
		else if( m_byTradeMode == STM_REPAIR )
			bResult = DeselectRepairItem( pOrgItem->GetIndex() );

		if( !bResult )
			return FALSE;		
	}	

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CShopTradeBoard::SetTradeMode( BYTE pi_byTradeMode )
{
	m_byTradeMode = pi_byTradeMode;

	m_uiShopWindow.SetTradeMode( (CGUIShopWindow::TradeMode)m_byTradeMode );
	m_uiShopWindow.SetCurrentShopBoard( 0 );
}

void
CShopTradeBoard::SetMerchant( CNpc * pi_pMerchant )
{
	if( pi_pMerchant )
	{
		m_dwNpcIndex	= pi_pMerchant->GetIndex();
		m_pMerchant		= pi_pMerchant;
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CShopTradeBoard::ClearTradeList( void )
{
	RemoveAllTradeInfo();

	SetGold( 0 );
	SetDalant( 0 );	

	m_uiShopWindow.ClearTradeItemList();

	m_uiShopWindow.SetEnableTrade( FALSE );	
}

void
CShopTradeBoard::CancelTrade( void )
{
	CItemBoard * pOrgItemBoard;
	CGUIItemWindow * pWindow;			

	TRADE_INFO * pTradeInfo;

	CTradeInfoList_Iterator it;
	for( it = m_listTradeInfo.begin(); it != m_listTradeInfo.end(); ++it )
	{
		pTradeInfo = *it;

		pOrgItemBoard = static_cast< CItemBoard * >( _GetBoard( pTradeInfo->m_byBoardType ) );			
		pWindow = static_cast< CGUIItemWindow *>( pOrgItemBoard->GetWindow() );

		if( m_byTradeMode == STM_BUY )
		{
			// set ui
			pWindow->TakeOutItem( pTradeInfo->m_byItemPackNo, pTradeInfo->m_byItemSocketNo );
		}
		else if( m_byTradeMode == STM_SELL || m_byTradeMode == STM_REPAIR )
		{
			// return item
			_GetGBM()->ReturnItemToItemBoard( pTradeInfo->m_byBoardType, 
											  pTradeInfo->m_byItemPackNo,
											  pTradeInfo->m_byItemSocketNo,
											  &pTradeInfo->m_clsItem );

			// set ui
			m_uiShopWindow.TakeOutTradeItem( pTradeInfo->m_clsItem.GetIndex() );		
		}
	}

	// 상점 아이템이면 제거
	if( _GetGBM()->GetHoverObject() && _GetGBM()->GetOrgBoardType() == IBT_SHOP )
		_GetGBM()->SetHoverObject( NULL );

	ClearTradeList();
}

void
CShopTradeBoard::ClearShopItemList( void )
{
	for( int i = 0; i < m_byMaxPackNum; ++i )
		m_listItemPack[i].EmptyAll();

	m_uiShopWindow.ClearBuyItemList();
}

BOOL
CShopTradeBoard::BuyItem_Request( void )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;		

	if( !m_pMerchant )
		return FALSE;

	// 유닛 상인은 유닛 메세지를 사용
	if( m_pMerchant->IsEqualMerchantType( NMT_UNIT ) )
	{
		TRADE_INFO * pBuyItemInfo = m_listTradeInfo.front();
		if( !pBuyItemInfo )
			return FALSE;

		return m_pNMP_Unit->UnitFrameBuyRequest_clzo( pBuyItemInfo->m_clsItem.GetDTIndex() );
	}
	// 일반 상인
	else
	{
		return m_pNMP_ItemTrade->BuyStoreRequest_clzo();
	}
}

void
CShopTradeBoard::BuyItem_Result( BOOL pi_bIsSuccessed )
{
	if( !m_pMerchant )
		return;

	//--------------------------------------------------------------------------
	// item 구입 성공
	if( pi_bIsSuccessed )
	{		
		CBoardItem			clsBuyItem;

		CTradeInfoList_Iterator it;	
		for( it = m_listTradeInfo.begin(); it != m_listTradeInfo.end(); ++it )
		{
			memcpy( &clsBuyItem, &(*it)->m_clsItem, sizeof( CBoardItem ) );			

			clsBuyItem.SetCurMaxDurability( clsBuyItem.GetOrgMaxDurability() );
			clsBuyItem.SetDurability( clsBuyItem.GetOrgMaxDurability() );

			// 상점에서 구입한 아이템의 최대 소켓수는 3개임
			if( clsBuyItem.IsEquipmentItem() )
			{
				if( clsBuyItem.GetMaxSocketNum() > 3 )
					clsBuyItem.SetMaxSocketNum( 3 );
			}

			// add item
			CItemBoard * pTarItemBoard = static_cast< CItemBoard * >( _GetBoard( (*it)->m_byBoardType ) );				
			pTarItemBoard->Add( (*it)->m_byItemPackNo, (*it)->m_byItemSocketNo, &clsBuyItem );			
		}				

		UI_DisplayTradeResultString( ITT_BUY, pi_bIsSuccessed );
	
		// clear data
		ClearTradeList();
	}
	//--------------------------------------------------------------------------
	// item 구입 실패
	else
	{
		UI_DisplayTradeResultString( ITT_BUY, pi_bIsSuccessed );
	}	
	
}

BOOL	
CShopTradeBoard::SelectBuyItem( BYTE pi_byListIndex, BYTE pi_byNum, BYTE pi_byTarBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	CBoardItem * pShopItem = GetShopItem( pi_byListIndex );
	if( !pShopItem )
		return FALSE;	

	BYTE byPackNo, bySocketNo;
	byPackNo	= pi_byPackNo;
	bySocketNo	= pi_bySocketNo;

	//--------------------------------------------------------------------------
	// 빈 공간 체크
	if( !( pi_byTarBoardType == IBT_INVENTORY ||
		   pi_byTarBoardType == IBT_ITEM_BELT ||
		   pi_byTarBoardType == IBT_EQUIP ) )
		return FALSE;

	CItemBoard * pTarItemBoard = static_cast< CItemBoard * >( _GetBoard( pi_byTarBoardType ) );

	if( pi_byTarBoardType == IBT_EQUIP )
	{
		if( pTarItemBoard->GetItem( byPackNo, bySocketNo ) )
			return FALSE;

		if( IsTradingItem( pi_byTarBoardType, byPackNo, bySocketNo ) )
			return FALSE;
	}
	else if( ( byPackNo == 0xFF || bySocketNo == 0xFF ) ||
			 ( pTarItemBoard->GetItem( byPackNo, bySocketNo ) ) ||
			 ( IsTradingItem( pi_byTarBoardType, byPackNo, bySocketNo ) ) )
	{
		if( !pTarItemBoard->FindEmptySocket_IncludeOtherBoard( byPackNo, bySocketNo ) )
		{
			_GUIMessageBox( NULL, "빈 소켓이 없습니다.", 0xAAFFFFFF );
			return FALSE;
		}
	}

	//--------------------------------------------------------------------------
	// 유닛 아이템일 경우 체크
	if( pShopItem->IsEqualItemKind( IEPT_UNIT ) )
	{
		if( !SelectBuyItem_CheckUnitItem( pShopItem ) )
			return FALSE;
	}
	// 유닛 아이템일 경우 체크 - 장착 가능한가?
	else if( pi_byTarBoardType == IBT_EQUIP )
	{
		if( !SelectBuyItem_CheckItem( pShopItem ) )
			return FALSE;
	}

	//--------------------------------------------------------------------------
	// 금액 계산
	float fBuyPrice = pShopItem->GetBuyPrice( pi_byNum ) * _GetAvatar()->GetBuyTradeProfitRate();

	if( pShopItem->IsEqualMoneyType( IMT_DALANT ) )
	{
		if( _GetAvatar()->GetDalant() < ( m_fDalant + fBuyPrice ) )
		{
			char buf[32];
			sprintf( buf, "%s가 부족합니다.", _GetAvatarMoneyName() );
			_GUIMessageBox( NULL, buf, 0xAAFFFFFF );
			return FALSE;
		}

		SetDalant( m_fDalant + fBuyPrice );		
	}
	else
	{
		if( _GetAvatar()->GetGold() < ( m_fGold + fBuyPrice ) )
		{
			_GUIMessageBox( NULL, "Gold가 부족합니다.", 0xAAFFFFFF );
			return FALSE;
		}
		
		SetGold( m_fGold + fBuyPrice );		
	}

	//--------------------------------------------------------------------------
	// 목록 저장
	TRADE_INFO * pTradeInfo;
	pTradeInfo = AddTradeInfo( pi_byTarBoardType, byPackNo, bySocketNo, pShopItem, pi_byNum );
	if( !pTradeInfo )
		return FALSE;

	// -------------------------------------------------------------------------
	// set ui
	CGUIItemWindow * pWindow = static_cast< CGUIItemWindow * >( pTarItemBoard->GetWindow() );
	if( !pWindow->TakeInItem( byPackNo, bySocketNo,
							  pShopItem->GetMeshID(), pShopItem->GetIconID(),
							  pi_byNum,
							  pShopItem->GetUsableLevel() ) )
		return FALSE;
	pWindow->SetTradeItem( byPackNo, bySocketNo, TRUE );
	// reset icon
	pTradeInfo->m_clsItem.SetIcon( pWindow->GetItem( byPackNo, bySocketNo ) );	

	
	if( !pWindow->IsShow() )
	{
		pWindow->Open();
	}
	if( pi_byTarBoardType == IBT_INVENTORY )
	{
		// 최근에 열린 창이 아니면 다시 연다.
		if( _GetInventoryBoard()->GetActivePackNo() != byPackNo )
			_GetInventoryBoard()->UI_OpenPack( byPackNo, TRUE );
	}

	m_uiShopWindow.SetEnableTrade( TRUE );


	return TRUE;
}

BOOL
CShopTradeBoard::SelectBuyItem_CheckItem( CBoardItem * pi_pShopItem )
{
	if( !pi_pShopItem )
		return FALSE;

	//----------------------------------------------------------------------
	// 아바타가 장착한 아이템과 검사

	// 무기나 방패일 경우 장착을 할 수 있는 상태인지 체크
	if( !_GetAvatar()->IsEquipable( pi_pShopItem ) )
		return FALSE;

	// 장착 가능한 아이템인지를 체크
	if( !_GetAvatar()->IsUsableItem( pi_pShopItem, TRUE ) )
		return FALSE;

	//----------------------------------------------------------------------
	// 아바타가 선택한 품목과 검사
	if( pi_pShopItem->IsEqualItemKind( IEPT_WEAPON ) )
	{
		if( pi_pShopItem->IsEqualHoldType( IHT_BOTH_HAND ) || pi_pShopItem->IsEqualHoldType( IHT_BOTH_HAND ) )
		{			
			CTradeInfoList_Iterator it;	
			for( it = m_listTradeInfo.begin(); it != m_listTradeInfo.end(); ++it )
			{
				if( (*it)->m_byBoardType != IBT_EQUIP )
					continue;

				if( (*it)->m_clsItem.IsEqualItemKind( IEPT_SHIELD ) )
				{
					_SetSystemMsg( "이미 방패를 구입하고 있으므로 양손/쌍손 무기는 장착/구입할 수 없습니다." );
					return FALSE;
				}
			}			
		}
	}
	else if( pi_pShopItem->IsEqualItemKind( IEPT_SHIELD ) )
	{
		CTradeInfoList_Iterator it;	
		for( it = m_listTradeInfo.begin(); it != m_listTradeInfo.end(); ++it )
		{
			if( (*it)->m_byBoardType != IBT_EQUIP )
				continue;

			if( (*it)->m_clsItem.IsEqualItemKind( IEPT_WEAPON ) )
			{
				if( (*it)->m_clsItem.IsEqualHoldType( IHT_BOTH_HAND ) || (*it)->m_clsItem.IsEqualHoldType( IHT_DUAL_HAND ) )
				{
					_SetSystemMsg( "이미 양손/쌍손 무기를 구입하고 있으므로 방패는 장착/구입할 수 없습니다." );
					return FALSE;
				}
			}
		}	
	}

	return TRUE;
}

// 유닛 아이템은 다른 아이템과 함께 거래가 불가능하며, 한 번에 1개 이상 거래할 수 없다.
BOOL
CShopTradeBoard::SelectBuyItem_CheckUnitItem( CBoardItem * pi_pShopItem )
{
	// 거래하려는 아이템이 없다면 TRUE
	if( m_listTradeInfo.size() > 0 )
	{
		_SetSystemMsg( "기갑 장비 아이템은 단일 수량으로만 거래할 수 있습니다." );
		return FALSE;
	}

	// 이미 유닛 아이템을 MAX_AVATAR_UNIT 만큼 소유하고 있다면 구입할 수 없다.
	if( _GetUnitStorehouseBoard()->GetEmptySocketNum() <= 0 )
	{
		_SetSystemMsg( "더 이상 기갑장비 아이템을 구입할 수 없습니다." );
		return FALSE;
	}

	return TRUE;
}

BOOL
CShopTradeBoard::DeselectBuyItem( BYTE pi_byOrgBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	CBoardItem * pShopItem = GetTradeItem( pi_byOrgBoardType, pi_byPackNo, pi_bySocketNo );
	if( !pShopItem )
		return FALSE;	

	// set hoveritem
	_GetGBM()->SetHoverObject( pShopItem );
	CBoardItem * pHoverItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pHoverItem )
		return FALSE;	
	

	// reset org object		
	_GetGBM()->SetOrgObject( IBT_SHOP, 0, pHoverItem->GetIndex(), NULL );

	
	//--------------------------------------------------------------------------
	// 목록 제거
	if( !RemoveTradeInfo( pi_byOrgBoardType, pi_byPackNo, pi_bySocketNo ) )
		return FALSE;


	//--------------------------------------------------------------------------
	// 금액 계산
	float fBuyPrice = pHoverItem->GetBuyPrice( pHoverItem->GetQuantity() ) * _GetAvatar()->GetBuyTradeProfitRate();

	if( pHoverItem->IsEqualMoneyType( IMT_DALANT ) )
	{
		SetDalant( m_fDalant - fBuyPrice );
	}
	else
	{
		SetGold( m_fGold - fBuyPrice );
	}

	//--------------------------------------------------------------------------
	// set ui
	CItemBoard * pTarItemBoard = static_cast< CItemBoard * >( _GetBoard( pi_byOrgBoardType ) );
	CGUIItemWindow * pWindow = static_cast< CGUIItemWindow *>( pTarItemBoard->GetWindow() );

	pWindow->TakeOutItem( pi_byPackNo, pi_bySocketNo );

	if( m_listTradeInfo.empty() )
		m_uiShopWindow.SetEnableTrade( FALSE );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CShopTradeBoard::SellItem_Request( void )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;

	if( !m_pMerchant )
		return FALSE;

	// 유닛 상인은 유닛 메세지를 사용
	if( m_pMerchant->IsEqualMerchantType( NMT_UNIT ) )
	{
		TRADE_INFO * pBuyItemInfo = m_listTradeInfo.front();
		if( !pBuyItemInfo )
			return FALSE;

		BYTE byUnitSocketNo = _GetUnitStorehouseBoard()->GetUnitSocketNo( pBuyItemInfo->m_clsItem.GetIndex() );
		if( byUnitSocketNo == 0xFF )
			return FALSE;

		return m_pNMP_Unit->UnitSellRequest_clzo( byUnitSocketNo );
	}
	// 일반 상인
	else
	{
		if( m_listTradeInfo.empty() )
			return FALSE;

		return m_pNMP_ItemTrade->SellStoreRequest_clzo();
	}
}

void
CShopTradeBoard::SellItem_Result( BOOL pi_bIsSuccessed )
{
	//--------------------------------------------------------------------------
	// item 판매 성공
	if( pi_bIsSuccessed )
	{		
		UI_DisplayTradeResultString( ITT_SELL, pi_bIsSuccessed );
		
		ClearTradeList();		
	}
	//--------------------------------------------------------------------------
	// item 판매 실패
	else
	{
		UI_DisplayTradeResultString( ITT_SELL, pi_bIsSuccessed );	
	}
}
			
BOOL	
CShopTradeBoard::SelectSellItem( BYTE pi_byOrgBoardType, BYTE pi_byOrgPackNo, BYTE pi_byOrgSocketNo, CBoardItem * pi_pItem )
{	
	if( !pi_pItem )
		return FALSE;

	// 팔 수 있는 아이템인지 체크
	if( !pi_pItem->IsSellable() )
	{
		_SetSystemMsg( "판매가 불가능한 아이템입니다." );
		return FALSE;
	}

	// 상점 타입 체크
	if( !IsTradableItem( pi_pItem->GetItemKind() ) )
	{
		_SetSystemMsg( "이 상점에는 팔 수 없는 아이템입니다." );
		return FALSE;
	}

	// 유닛 아이템일 경우 체크
	if( pi_pItem->IsEqualItemKind( IEPT_UNIT ) )
	{
		if( !SelectSellItem_CheckUnitItem( pi_pItem ) )
			return FALSE;
	}
	// 미니맵 아이템일 경우 체크
	else if( pi_pItem->IsEqualItemKind( IEPT_MAP ) )
	{
		_GetMinimapWindow()->Close();
	}

	//--------------------------------------------------------------------------

	// 동일한 아이템이 있는지 검사한다.
	TRADE_INFO * pTradeInfo = GetTradeInfo( pi_pItem->GetIndex() );

	// 아이템 정보 저장
	if( pTradeInfo )
	{		
		pTradeInfo->m_clsItem.SetQuantity( pTradeInfo->m_clsItem.GetQuantity() + pi_pItem->GetQuantity() );
	}
	else
	{
		TRADE_INFO * pTradeInfo;
		pTradeInfo = AddTradeInfo( pi_byOrgBoardType, pi_byOrgPackNo, pi_byOrgSocketNo, pi_pItem, pi_pItem->GetQuantity() );
		if( !pTradeInfo )
			return FALSE;

		// set ui
		m_uiShopWindow.TakeInTradeItem( pi_pItem->GetIndex(), pi_pItem->GetMeshID(), pi_pItem->GetIconID(), pi_pItem->GetQuantity() );

		// reset icon
		pTradeInfo->m_clsItem.SetIcon( m_uiShopWindow.GetTradeItemByIndex( pi_pItem->GetIndex() ) );
	}

	//--------------------------------------------------------------------------

	// 금액 계산
	float fSellPrice = pi_pItem->GetSellPrice( pi_pItem->GetQuantity() ) * _GetAvatar()->GetSellTradeProfitRate();

	if( pi_pItem->IsEqualMoneyType( IMT_DALANT ) )
	{
		SetDalant( m_fDalant + fSellPrice );		
	}
	else
	{
		SetGold( m_fGold + fSellPrice );
	}


	//--------------------------------------------------------------------------	

	m_uiShopWindow.SetEnableTrade( TRUE );


	return TRUE;
}

// 유닛 아이템은 다른 아이템과 함께 거래가 불가능하며, 한 번에 1개 이상 거래할 수 없다.
BOOL
CShopTradeBoard::SelectSellItem_CheckUnitItem( CBoardItem * pi_pItem )
{
	// 거래하려는 아이템이 없다면 TRUE
	if( m_listTradeInfo.size() > 0 )
	{
		_SetSystemMsg( "기갑 장비 아이템은 단일 수량으로만 거래할 수 있습니다." );
		return FALSE;
	}

	return TRUE;
}

BOOL
CShopTradeBoard::DeselectSellItem( DWORD pi_dwIndex, BYTE pi_byQuantity )
{	
	TRADE_INFO	* pTradeInfo = GetTradeInfo( pi_dwIndex );
	if( !pTradeInfo )
		return FALSE;

	// return item
	_GetGBM()->ReturnItemToItemBoard( pTradeInfo->m_byBoardType, 
									  pTradeInfo->m_byItemPackNo,
									  pTradeInfo->m_byItemSocketNo,
									  &pTradeInfo->m_clsItem );
	
	// 금액 계산
	float fSellPrice = pTradeInfo->m_clsItem.GetSellPrice( pi_byQuantity ) * _GetAvatar()->GetSellTradeProfitRate();

	if( pTradeInfo->m_clsItem.IsEqualMoneyType( IMT_DALANT ) )
	{
		SetDalant( m_fDalant - fSellPrice );
	}
	else
	{
		SetGold( m_fGold - fSellPrice );
	}

	// set ui
	m_uiShopWindow.TakeOutTradeItem( pTradeInfo->m_clsItem.GetIndex() );

	if( m_listTradeInfo.empty() )
		m_uiShopWindow.SetEnableTrade( FALSE );
	
	// 아이템 정보 제거
	RemoveTradeInfoByIndex( pi_dwIndex );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CShopTradeBoard::RepairItem_Request( void )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;	

	// send request
	m_pNMP_ItemTrade->RepairRequest_clzo();

	return TRUE;
}

BOOL		
CShopTradeBoard::RepairItem_Result( BOOL pi_bIsSuccess )
{
	//--------------------------------------------------------------------------
	// item 수리 성공
	if( pi_bIsSuccess )
	{	
		CItemBoard * pTarItemBoard;

		CTradeInfoList_Iterator it;	
		for( it = m_listTradeInfo.begin(); it != m_listTradeInfo.end(); ++it )
		{
			// 아이템의 내구성 설정
			(*it)->m_clsItem.SetCurMaxDurability( (*it)->m_clsItem.GetRepairableMaxDP() );
			(*it)->m_clsItem.SetDurability( (*it)->m_clsItem.GetCurMaxDurability() );			

			// add item
			pTarItemBoard = static_cast< CItemBoard * >( _GetBoard( (*it)->m_byBoardType ) );				
			pTarItemBoard->Add( (*it)->m_byItemPackNo, (*it)->m_byItemSocketNo, &(*it)->m_clsItem );
		}			
		
		UI_DisplayTradeResultString( ITT_REPAIR, pi_bIsSuccess );		

		ClearTradeList();
	}
	//--------------------------------------------------------------------------
	// item 수리 실패
	else
	{
		UI_DisplayTradeResultString( ITT_REPAIR, pi_bIsSuccess );
	}	

	return pi_bIsSuccess;
}

BOOL
CShopTradeBoard::SelectRepairItem( BYTE pi_byOrgBoardType, BYTE pi_byOrgPackNo, BYTE pi_byOrgSocketNo, CBoardItem * pi_pItem )
{
	if( !pi_pItem )
		return FALSE;

	// 수리가능한 아이템인지 검사한다.
	if( !pi_pItem->IsRepairable() )
	{
		_GUIMessageBox( NULL, "수리가 불가능한 아이템입니다.", 0xAAFFFFFF );
		return FALSE;
	}
	else if( pi_pItem->GetDurability() >= pi_pItem->GetCurMaxDurability() )
	{
		_GUIMessageBox( NULL, "수리할 필요가 없는 아이템입니다.", 0xAAFFFFFF );
		return FALSE;
	}

	// -------------------------------------------------------------------------
	// 아이템 정보 저장
	if( !AddTradeInfo( pi_byOrgBoardType, pi_byOrgPackNo, pi_byOrgSocketNo, pi_pItem, pi_pItem->GetQuantity() ) )
		return FALSE;

	CBoardItem * pTradeItem = GetTradeItemByIndex( pi_pItem->GetIndex() ); 
	if( !pTradeItem )
		return FALSE;


	//--------------------------------------------------------------------------
	// 금액 계산
	DWORD fRepairPrice = pi_pItem->GetRepairPrice();

	if( pi_pItem->GetMoneyType() == IMT_DALANT )
	{
		if( _GetAvatar()->GetDalant() < ( m_fDalant + fRepairPrice ) )
		{
			char buf[32];
			sprintf( buf, "%s가 부족합니다.", _GetAvatarMoneyName() );
			_GUIMessageBox( NULL, buf, 0xAAFFFFFF );
			return FALSE;
		}

		SetDalant( m_fDalant + fRepairPrice );		
	}
	else
	{
		if( _GetAvatar()->GetGold() < ( m_fGold + fRepairPrice ) )
		{
			_GUIMessageBox( NULL, "Gold가 부족합니다.", 0xAAFFFFFF );
			return FALSE;
		}

		SetGold( m_fGold + fRepairPrice );		
	}

	//--------------------------------------------------------------------------
	// set ui	
	m_uiShopWindow.TakeInTradeItem( pi_pItem->GetIndex(), pi_pItem->GetMeshID(), pi_pItem->GetIconID(), pi_pItem->GetQuantity() );

	// reset icon
	pTradeItem->SetIcon( m_uiShopWindow.GetTradeItemByIndex( pi_pItem->GetIndex() ) );

	m_uiShopWindow.SetEnableTrade( TRUE );

	return TRUE;
}

BOOL
CShopTradeBoard::DeselectRepairItem( DWORD pi_dwIndex )
{
	TRADE_INFO	* pTradeInfo = GetTradeInfo( pi_dwIndex );
	if( !pTradeInfo )
		return FALSE;

	// return item
	_GetGBM()->ReturnItemToItemBoard( pTradeInfo->m_byBoardType, 
									  pTradeInfo->m_byItemPackNo,
									  pTradeInfo->m_byItemSocketNo,
									  &pTradeInfo->m_clsItem );
	
	// 금액 계산
	float fRepairPrice = pTradeInfo->m_clsItem.GetRepairPrice();
	if( pTradeInfo->m_clsItem.GetMoneyType() == IMT_DALANT )
	{
		SetDalant( m_fDalant - fRepairPrice );		
	}
	else
	{
		SetGold( m_fGold - fRepairPrice );
	}

	// set ui
	m_uiShopWindow.TakeOutTradeItem( pTradeInfo->m_clsItem.GetIndex() );

	if( m_listTradeInfo.empty() )
		m_uiShopWindow.SetEnableTrade( FALSE );

	// 아이템 정보 제거	
	RemoveTradeInfoByIndex( pi_dwIndex );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CShopTradeBoard::UI_SelectItem_By_HotKey( BYTE pi_bySocketNo, BYTE pi_byQuantity )
{
	if( !UI_RemoveRequest( 0, pi_bySocketNo, pi_byQuantity ) )
		return;

	if( m_byTradeMode == STM_BUY )
	{
		_GetInventoryBoard()->UI_AddRequest( _GetInventoryBoard()->GetActivePackNo(), 0 );		
	}				
	else
	{
		CObjectBoard * pBoard = static_cast< CObjectBoard * >( _GetGBM()->GetBoard( _GetGBM()->GetOrgBoardType() ) );
		pBoard->UI_AddRequest( _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo() );
	}							
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

TRADE_INFO	*
CShopTradeBoard::AddTradeInfo( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardItem * pi_pItem, BYTE pi_byQuantity )
{
	if( !pi_pItem )
		return NULL;

	TRADE_INFO	*	pNewTradeInfo = NULL;
	_NEW( pNewTradeInfo, TRADE_INFO );
	
	pNewTradeInfo->m_byBoardType		= pi_byBoardType;
	pNewTradeInfo->m_byItemPackNo		= pi_byPackNo;
	pNewTradeInfo->m_byItemSocketNo		= pi_bySocketNo;
	memcpy( &pNewTradeInfo->m_clsItem, pi_pItem, sizeof( CBoardItem ) );
	pNewTradeInfo->m_clsItem.SetQuantity( pi_byQuantity );

	// add
	m_listTradeInfo.push_back( pNewTradeInfo );

	// set ui -  set useless socket
	if( m_byTradeMode == STM_SELL )
	{
		CItemBoard * pItemBoard = static_cast< CItemBoard * >( _GetGBM()->GetBoard( pi_byBoardType ) );
		if( pItemBoard )
			pItemBoard->UI_SetUselessSocket( pi_byPackNo, pi_bySocketNo, TRUE );
	}

	return pNewTradeInfo;
}

BOOL
CShopTradeBoard::RemoveTradeInfo( TRADE_INFO * pi_pTradeInfo )
{
	if( !pi_pTradeInfo )
		return FALSE;

	// set ui - set usable socket
	if( m_byTradeMode == STM_SELL )
	{
		CItemBoard * pItemBoard = static_cast< CItemBoard * >( _GetGBM()->GetBoard( pi_pTradeInfo->m_byBoardType ) );
		if( pItemBoard )
			pItemBoard->UI_SetUselessSocket( pi_pTradeInfo->m_byItemPackNo, pi_pTradeInfo->m_byItemSocketNo, FALSE );
	}

	m_listTradeInfo.remove( pi_pTradeInfo );

	_DELETE( pi_pTradeInfo );

	return TRUE;
}

BOOL
CShopTradeBoard::RemoveTradeInfoByIndex( DWORD pi_dwItemIndex )
{
	CTradeInfoList_Iterator it;	
	for( it = m_listTradeInfo.begin(); it != m_listTradeInfo.end(); ++it )
	{
		if( (*it)->m_clsItem.GetIndex() == pi_dwItemIndex )
		{
			RemoveTradeInfo( *it );
			return TRUE;
		}
	}

	return FALSE;
}

BOOL
CShopTradeBoard::RemoveTradeInfo( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	TRADE_INFO * pTradeInfo;

	CTradeInfoList_Iterator it;
	for( it = m_listTradeInfo.begin(); it != m_listTradeInfo.end(); ++it )
	{
		pTradeInfo = *it;

		if( pTradeInfo->m_byBoardType == pi_byBoardType &&
			pTradeInfo->m_byItemPackNo == pi_byPackNo &&
			pTradeInfo->m_byItemSocketNo == pi_bySocketNo )
		{						
			RemoveTradeInfo( *it );
			return TRUE;
		}
	}

	return FALSE;
}

void		
CShopTradeBoard::RemoveAllTradeInfo( void )
{
	CTradeInfoList_Iterator it;
	it = m_listTradeInfo.begin();
	while( it != m_listTradeInfo.end() )
	{
		RemoveTradeInfo( *it );

		it = m_listTradeInfo.begin();
	}
}

//==============================================================================

TRADE_INFO	*	
CShopTradeBoard::GetTradeInfo( DWORD pi_dwItemIndex )
{
	CTradeInfoList_Iterator it;
	for( it = m_listTradeInfo.begin(); it != m_listTradeInfo.end(); ++it )
	{
		if( (*it)->m_clsItem.GetIndex() == pi_dwItemIndex )
			return (*it);
	}

	return NULL;
}

CBoardItem *	
CShopTradeBoard::GetTradeItem( BYTE pi_bySocketNo )
{
	int nCount = 0;
	CTradeInfoList_Iterator it;
	for( it = m_listTradeInfo.begin(); it != m_listTradeInfo.end(); ++it )
	{
		if( nCount == pi_bySocketNo )
			return &(*it)->m_clsItem;

		++nCount;
	}

	return NULL;
}

CBoardItem *
CShopTradeBoard::GetTradeItemByIndex( DWORD pi_dwIndex )
{
	TRADE_INFO * pTradeInfo = GetTradeInfo( pi_dwIndex );
	if( pTradeInfo )
		return &pTradeInfo->m_clsItem;	

	return NULL;
}

CBoardItem *	
CShopTradeBoard::GetTradeItem( BYTE pi_byOrgBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	CTradeInfoList_Iterator it;
	for( it = m_listTradeInfo.begin(); it != m_listTradeInfo.end(); ++it )
	{
		if( (*it)->m_byBoardType == pi_byOrgBoardType &&
			(*it)->m_byItemPackNo == pi_byPackNo &&
			(*it)->m_byItemSocketNo == pi_bySocketNo )
		{
			return &(*it)->m_clsItem;
		}
	}
	
	return NULL;
}

BOOL			
CShopTradeBoard::IsTradingItem( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	if( GetTradeItem( pi_byBoardType, pi_byPackNo, pi_bySocketNo ) )
		return TRUE;

	return FALSE;	
}

BOOL			
CShopTradeBoard::IsTradableItem( BYTE pi_byItemTypeID )
{
	// 기본적으로 모든 상점에서 모든 물품을 판매 가능
	// 예외 => 유닛은 유닛 상점에서만 팔 수 있음. 유닛 상점에서는 아이템 판매 불가능.

	if( m_pMerchant->IsEqualMerchantType( NMT_UNIT ) )
	{
		if( pi_byItemTypeID == IEPT_UNIT )
			return TRUE;
		else
			return FALSE;
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CBoardItem *	
CShopTradeBoard::GetShopItem( BYTE pi_bySocketNo )
{
	return static_cast< CBoardItem * >( GetObject( 0, pi_bySocketNo ) );	
}

CBoardItem *	
CShopTradeBoard::GetShopItemByIndex( DWORD pi_dwIndex )
{
	return static_cast< CBoardItem * >( GetObjectByIndex( pi_dwIndex ) );
}

CBoardItem *
CShopTradeBoard::GetShopItemByMeshID( DWORD pi_dwMeshID )
{
	CBoardItem * pOrgItem;

	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[i].GetMaxSocketNum(); ++j )
		{
			if( !m_listItemPack[i].GetObject( j ) )
				continue;

			pOrgItem = static_cast< CBoardItem * >( m_listItemPack[i].GetObject( j ) );
			if( pOrgItem->GetMeshID() == pi_dwMeshID )
				return pOrgItem;
		}
	}

	return NULL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CShopTradeBoard::SetGold( float pi_fGold )
{
	m_fGold = pi_fGold;

	if( m_fGold < 0.f )
		m_fGold = 0.f;

	m_uiShopWindow.SetGold( ( DWORD )m_fGold );
}

void
CShopTradeBoard::SetDalant( float pi_fDalant )
{
	m_fDalant = pi_fDalant;

	if( m_fDalant < 0.f )
		m_fDalant = 0.f;

	m_uiShopWindow.SetDalant( ( DWORD )m_fDalant );
}

//==============================================================================

DWORD		
CShopTradeBoard::GetMaxNumToBuy( CBoardItem * pi_pItem )
{
	int nTotalMoney = 0;
	if( pi_pItem->GetMoneyType() == IMT_DALANT )
		nTotalMoney = _GetAvatar()->GetDalant() - m_fDalant;
	else
		nTotalMoney = _GetAvatar()->GetGold() - m_fGold;

	if( nTotalMoney <= 0 )
		return 0;

	int nPrice = pi_pItem->GetBuyPrice();

	return ( nTotalMoney / nPrice );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CShopTradeBoard::StoreListRequest( void )
{
	m_pNMP_ItemTrade->StoreListRequest_clzo();	
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

LRESULT	
CShopTradeBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UI_MOUSE_MSG	stRecvMsg( lParam );

	static char s_pTradeModeStr[3][8] = { "구입", "판매", "수리" };

	switch( uMsg )
	{
		case WM_BOARD_ADD_OBJECT			:
			UI_AddRequest( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
			break;

		case WM_BOARD_CHECK_REMOVE_OBJECT	:
			{		
				CBoardItem * pOrgItem;
				if( m_byTradeMode == STM_BUY )
				{
					pOrgItem = GetShopItem( stRecvMsg.m_bySocketNo );		
					if( !pOrgItem )
						break;
					
					if( pOrgItem->IsSplitable() )
					{
						POINT pDlgPos = { gMouse.x, gMouse.y };
						DWORD dwMaxNum = GetMaxNumToBuy( pOrgItem );
						if( dwMaxNum > MAX_ITEM_QUANTITY )
							dwMaxNum = MAX_ITEM_QUANTITY;

						_GUINumInputDlg( m_uiWindow, pDlgPos, dwMaxNum, INPUT_DLG_ID, dwMaxNum);
					}
					else
					{
						UI_RemoveRequest( 0, stRecvMsg.m_bySocketNo, 1 );
					}
				}
				// sell, repair
				else
				{
					UI_RemoveRequest( 0, stRecvMsg.m_bySocketNo, 1 );
				}						
			}
			break;

		case WM_BOARD_REMOVE_OBJECT			:					
			UI_RemoveRequest( 0, stRecvMsg.m_bySocketNo, stRecvMsg.m_byQuantity );
			
			break;

		case WM_BOARD_TRADE_HOT_KEY			:				
			if( stRecvMsg.m_byQuantity )
			{
				UI_SelectItem_By_HotKey( stRecvMsg.m_bySocketNo, stRecvMsg.m_byQuantity );
			}
			else
			{
				CBoardItem * pOrgItem;
				if( m_byTradeMode == STM_BUY )
				{
					pOrgItem = GetShopItem( stRecvMsg.m_bySocketNo );		
					if( !pOrgItem )
						break;
					
					if( pOrgItem->IsSplitable() )
					{
						POINT pDlgPos = { gMouse.x, gMouse.y };
						DWORD dwMaxNum = GetMaxNumToBuy( pOrgItem );
						if( dwMaxNum > MAX_ITEM_QUANTITY )
							dwMaxNum = MAX_ITEM_QUANTITY;

						_GUINumInputDlg( m_uiWindow, pDlgPos, dwMaxNum, INPUT_DLG_ID, dwMaxNum );										 
					}
					else
					{
						UI_SelectItem_By_HotKey( stRecvMsg.m_bySocketNo, 1 );
					}
				}
				// sell, repair
				else
				{
					UI_SelectItem_By_HotKey( stRecvMsg.m_bySocketNo, 1 );
				}						
			}									
			break;

		case WM_BOARD_TRADE					:
			if( m_byTradeMode == STM_BUY )
			{
				BuyItem_Request();
			}
			else if( m_byTradeMode == STM_SELL )
			{
				SellItem_Request();
			}
			else if( m_byTradeMode == STM_REPAIR )
			{
				RepairItem_Request();
			}

			break;

		case WM_BOARD_CANCEL_TRADE			:
			CancelTrade();

			break;

		case WM_BOARD_CHANGE_TRADE_MODE		:
			if( !m_listTradeInfo.empty() )
			{
				char pBuf[128];					
				sprintf( pBuf, "선택된 아이템을 %s하시겠습니까?", s_pTradeModeStr[m_byTradeMode] );
				_GUIMessageBox( &m_uiShopWindow, pBuf, 0xF0FFFFFF, MSGBOX_ID_CHANGE_TRADE, DLG_STYLE_OK_CANCEL ); 					
			}
			else
			{
				SetTradeMode( stRecvMsg.m_byPackNo );
			}
			break;	

		case WM_CLOSING_WINDOW				:
			if( !m_listTradeInfo.empty() )
			{
				char pBuf[128];					
				sprintf( pBuf, "선택된 아이템을 %s하시겠습니까?", s_pTradeModeStr[m_byTradeMode] );
				_GUIMessageBox( &m_uiShopWindow, pBuf, 0xF0FFFFFF, MSGBOX_ID_CLOSE, DLG_STYLE_OK_CANCEL ); 	
				return 1;
			}			
			break;

		case WM_CLOSE_WINDOW				:
			CancelTrade();
			break;


		case WM_BOARD_HOVER_OBJECT			:
			{
				CBoardItem * pItem;
				if( m_byTradeMode == STM_BUY )
				{
					pItem = GetShopItem( stRecvMsg.m_bySocketNo );
				}
				else
				{
					pItem = GetTradeItem( stRecvMsg.m_bySocketNo );
				}				

				DisplayItemInfo( pItem, m_byTradeMode );
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