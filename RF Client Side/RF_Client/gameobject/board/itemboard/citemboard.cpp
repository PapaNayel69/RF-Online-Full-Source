////////////////////////////////////////////////////////////////////////////////
//
// CItemBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../../Manager/CCharacterMgr.h"
#include "../Common/CBoardManager.h"

#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_ItemArrangement.h"
#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_Item.h"

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

BYTE	
GetStoragePos( BYTE pi_byBoardType, BYTE pi_byItemKind )
{
	if( pi_byBoardType == IBT_EQUIP )
	{
		if( pi_byItemKind < MAX_EQUIP_PART )
			return IBT_EQUIP;
		else
			return _STORAGE_POS::EMBELLISH;
	}
	else	
	{
		return pi_byBoardType;
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CItemBoard::CItemBoard()
{
	m_pNetMsgProcessor_Arrange	= static_cast< CNetMsgProcessor_ItemArrangement * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_ITEM_ARRANGE ) );
	m_pNetMsgProcessor_Item		= static_cast< CNetMsgProcessor_Item * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_ITEM ) );
}

CItemBoard::~CItemBoard()
{

}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL	
CItemBoard::Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	CBoardItem * pOrgBoardItem	= static_cast< CBoardItem * >( pi_pOrgObject );
	CBoardItem * pExistBoardItem= static_cast< CBoardItem * >( GetObject( pi_byTarPackNo, pi_byTarSocketNo ) );
	if( !pExistBoardItem )
		return FALSE;

	if( !pExistBoardItem->IsSameKind( pOrgBoardItem ) ||
		!pExistBoardItem->IsSplitable() )
		return FALSE;

	if( !pExistBoardItem->AddQuantity( pOrgBoardItem->GetQuantity() ) )
		return FALSE;
	_GetAvatar()->ApplyItemEffect_Jade( pOrgBoardItem, TRUE );

	pOrgBoardItem->RemoveQuantity( pExistBoardItem->GetQuantity() );

	return TRUE;
}

BOOL
CItemBoard::Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	CBoardItem * pOrgItem = static_cast< CBoardItem * >( pi_pOrgObject );
	CBoardItem * pNewItem = static_cast< CBoardItem * >( pi_pOrgObject->Clone() );
	if( !pNewItem )
		return FALSE;	

	// data
	if( !m_listItemPack[pi_byTarPackNo].Add( pi_byTarSocketNo, pNewItem ) )
		return FALSE;
	_GetAvatar()->ApplyItemEffect_Jade( pOrgItem, TRUE );

	pOrgItem->SetQuantity( 0 );

	// icon
	CGUIItemWindow * pWindow = static_cast< CGUIItemWindow *>( m_uiWindow );
	pWindow->TakeInItem( pi_byTarPackNo, pi_byTarSocketNo, 
						 pNewItem->GetMeshID(), pNewItem->GetIconID(),
						 pNewItem->GetQuantity(),
						 pNewItem->GetUsableLevel() );		
	pNewItem->SetIcon( pWindow->GetItem( pi_byTarPackNo, pi_byTarSocketNo ) );

	return TRUE;
}

BOOL
CItemBoard::Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity )
{
	CBoardItem * pOrgItem = static_cast< CBoardItem * >( pi_pOrgObject );

	_GetAvatar()->ApplyItemEffect_Jade( pOrgItem, FALSE );
	if( pOrgItem->RemoveQuantity( pi_byQuantity ) > 0 )
		return TRUE;

	return FALSE;
}

BOOL
CItemBoard::Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject )
{
	CGUIItemWindow * pWindow = static_cast< CGUIItemWindow *>( m_uiWindow );
	pWindow->TakeOutItem( pi_byPackNo, pi_bySocketNo );

	_GetAvatar()->ApplyItemEffect_Jade( static_cast< CBoardItem * >( pi_pOrgObject ), FALSE );
	m_listItemPack[pi_byPackNo].Remove( pi_bySocketNo );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CItemBoard::UI_CheckBeforeAdd_CheckEmptySocket( void )
{
	if( UI_CheckBeforeAdd_CheckEmptySocket_Trade() )
		return FALSE;

	if( !GetEmptySocketNum() )
	{		
		CBoardObject * pHoverObject = _GetGBM()->GetHoverObject();
		if( !pHoverObject->IsEqualItemType( BOT_BOARD_ITEM ) )
			return FALSE;

		CBoardItem * pHoverItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );	

		if( pHoverItem->IsSplitable() )
		{
			CBoardItem * pTarItem = static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );
			if( !pHoverItem->IsSameKind( pTarItem ) )
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL
CItemBoard::UI_CheckBeforeAdd_CheckHoverObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeAdd_CheckHoverObject() )
		return FALSE;
	
	// check trade item
	if( UI_CheckBeforeAdd_CheckHoverObject_Trade() )
		return FALSE;

	if( _GetPlayerTradeBoard()->IsTrading() )
	{
		if( _GetGBM()->GetOrgBoardType() != m_byBoardType )
		{
			_SetSystemMsg( "플레이어와 아이템을 트레이드 하는 중에는 아이템을 이동할 수 없습니다." );
			
			if( _GetGBM()->ReturnItemToItemBoard( _GetGBM()->GetOrgBoardType(),
												  _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), _GetGBM()->GetHoverItem() ) )
				_GetGBM()->SetHoverObject( NULL );

			return FALSE;
		}
		else
		{
			CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
			CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );
			CBoardItem * pSameItem;

			// 같은 보드내에서 아이템을 나눌려고 할 때
			pSameItem = static_cast< CBoardItem * >( _GetGBM()->GetObjectByIndex( _GetGBM()->GetOrgBoardType(), pHoverItem->GetIndex() ) );

			if( pSameItem && pSameItem != pTarItem )
			{
				_SetSystemMsg( "플레이어와 아이템을 트레이드 하는 중에는 아이템을 나눌 수 없습니다." );

				if( _GetGBM()->ReturnItemToItemBoard( _GetGBM()->GetOrgBoardType(),
													  _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), pHoverItem ) )
					_GetGBM()->SetHoverObject( NULL );
				
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL
CItemBoard::UI_CheckBeforeAdd_CheckTargetObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeAdd_CheckTargetObject() )
		return FALSE;

	if( UI_CheckBeforeAdd_CheckTargetObject_Trade() )
	{
		if( _GetGBM()->ReturnItemToItemBoard( _GetGBM()->GetOrgBoardType(),
											  _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), _GetGBM()->GetHoverItem() ) )
			_GetGBM()->SetHoverObject( NULL );
		return FALSE;
	}

	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );

	// check whether swap
	if( pHoverItem && pTarItem )
	{
		// force이면 swap할수없다
		CBoardObject * pHoverObject = _GetGBM()->GetHoverObject();
		if( pHoverObject->IsEqualItemType( BOT_SF ) )
			return FALSE;
		
		if( _GetAvatar()->IsEqualAction( CAI_ITEM_USE_MINING ) )
		{
			// 스왑할 아이템이 현재 사용중인 배터리인지를 체크
			if( pTarItem->GetIndex() == _GetMiningBoard()->GetBatteryIndex() )
			{
				_SetSystemMsg( "현재 채굴에 사용중인 배터리 입니다. 채굴이 끝날때까지 기다려 주십시오." );
				return FALSE;
			}
		}
		
		if( pTarItem->IsSameKind( pHoverItem ) )
		{
			if( pTarItem->IsSplitable() && ( pTarItem->GetQuantity() < MAX_ITEM_QUANTITY ) )
				return TRUE;
		}
		// hoveritem이 쪼개진 아이템이면 swap할수 없다.
		else if( _GetGBM()->GetObjectByIndex( _GetGBM()->GetOrgBoardType(), pHoverItem->GetIndex() ) )
		{
			if( _GetGBM()->ReturnItemToItemBoard( _GetGBM()->GetOrgBoardType(),
												  _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), pHoverItem ) )
				_GetGBM()->SetHoverObject( NULL );
				
			return FALSE;
		}
		
		
		UI_SwapRequest( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo() );

		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------


BOOL
CItemBoard::UI_AddRequest_ProcessObject( void )
{
	BOOL l_bResult = FALSE;

	CBoardObject * pHoverObject = _GetGBM()->GetHoverObject();	

	if( pHoverObject->IsEqualItemType( BOT_BOARD_ITEM ) )		
	{
		CBoardItem * pHoverItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );	

		if( pHoverItem->IsSplitable() )
		{
			if( _GetGBM()->GetTarObject() )
				l_bResult = UI_AddRequest_ProcessSplitableItem_Combine();
			else
				l_bResult = UI_AddRequest_ProcessSplitableItem_Devide();
		}			
		else 
		{
			l_bResult = UI_AddRequest_ProcessNonSplitableItem();
		}
	}
	else
	{
		l_bResult = UI_AddRequest_ProcessNonSplitableItem();
	}

	return l_bResult;
}

BOOL
CItemBoard::UI_AddRequest_ProcessSplitableItem_Devide( void )
{
	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	CBoardItem * pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );

	// pOrgItem이 모두 이동할 경우
	if( !pOrgItem )
	{
		// same board
		if( _GetGBM()->GetOrgBoardType() == _GetGBM()->GetTarBoardType() )
		{
			if( !Add( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo(), pHoverItem ) )
				return FALSE;

			_GetGBM()->SetHoverObject( NULL );

			if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
			{
				PlayWave( SND_ID_ITEM_ADD_IN_PACK );
			}
			else if( _GetGBM()->GetOrgBoardType() == IBT_ITEM_BELT )
			{
				PlayWave( SND_ID_BELT_EQUIP );
			}
		}
		// other board ( only Potion )
		else if( pHoverItem->IsEqualItemKind( IEPT_POTION ) )
		{
			m_pNetMsgProcessor_Arrange->PotionsocketChangeRequest_zocl();
		}
	}
	// pOrgItem의 일부만 이동할 경우
	else
	{
		if( pHoverItem->IsEqualItemKind( IEPT_POTION ) )
			m_pNetMsgProcessor_Arrange->PotionsocketSeparationRequest_clzo();
		else
			m_pNetMsgProcessor_Arrange->ResSeparationRequest_clzo();
	}

	return TRUE;
}

BOOL
CItemBoard::UI_AddRequest_ProcessSplitableItem_Combine( void )
{
	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	CBoardItem * pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );

	// check whether same item
	if( pTarItem && !pHoverItem->IsSameKind( pTarItem ) )
		return FALSE;

	// check hover and target are same item.
	if( pHoverItem->IsEqual( pTarItem ) )
	{
		UI_AddResult( TRUE );
	}
	// not same
	else
	{
		if( !UI_AddRequest_ProcessSplitableItem_Combine_CheckQuantity() )
			return FALSE;

		if( pHoverItem->IsEqualItemKind( IEPT_POTION ) )
			m_pNetMsgProcessor_Arrange->PotionsocketDivisionRequest_clzo();
		else
			m_pNetMsgProcessor_Arrange->ResDivisionRequest_clzo();
	}

	return TRUE;
}

BOOL
CItemBoard::UI_AddRequest_ProcessSplitableItem_Combine_CheckQuantity( void )
{
	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	CBoardItem * pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );

	// check quantity
	int nCombineQuantity = pHoverItem->GetQuantity();
	if( ( pTarItem->GetQuantity() + pHoverItem->GetQuantity() ) > MAX_ITEM_QUANTITY )
		nCombineQuantity = MAX_ITEM_QUANTITY - pTarItem->GetQuantity();

	if( nCombineQuantity <= 0 )
		return FALSE;

	// return remained item to org board
	if( nCombineQuantity < pHoverItem->GetQuantity() )
	{
		pHoverItem->RemoveQuantity( nCombineQuantity );
		_GetGBM()->AddObject( _GetGBM()->GetOrgBoardType(), _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), pHoverItem );

		pHoverItem->SetQuantity( nCombineQuantity );
		_GetGBM()->SetOrgObject( _GetGBM()->GetObject( _GetGBM()->GetOrgBoardType(), _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo() ) );
	}

	return TRUE;
}

BOOL
CItemBoard::UI_AddRequest_ProcessNonSplitableItem( void )
{
	if( _GetGBM()->GetOrgBoardType() == _GetGBM()->GetTarBoardType() )
	{
		UI_AddResult( TRUE );
	}	
	else
	{
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------

void
CItemBoard::UI_AddResult_ProcessObject( void )
{
	if( _GetGBM()->HaveHoverObject() )
		Add( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo(), _GetGBM()->GetHoverObject() );	
	else
		Add( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo(), _GetGBM()->GetDeletedHoverItem() );	
}

//==============================================================================

BOOL
CItemBoard::UI_CheckBeforeRemove_CheckHoverObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeRemove_CheckHoverObject() )
		return FALSE;

	return TRUE;
}

BOOL
CItemBoard::UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity )
{
	// check trade item
	if( UI_CheckBeforRemove_CheckOriginalObject_Trade() )
		return FALSE;

	if( !CObjectBoard::UI_CheckBeforeRemove_CheckOriginalObject( pi_byQuantity ) )
		return FALSE;

	CBoardItem * pOrgItem = static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	if( pOrgItem->GetQuantity() < pi_byQuantity )
		return FALSE;

	return TRUE;
}

//------------------------------------------------------------------------------

BOOL
CItemBoard::UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity )
{
	if( !pi_byQuantity )
		return FALSE;

	CBoardItem * pHoverItem	= NULL;
	CBoardItem * pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );	

	// set hover item
	pHoverItem = static_cast< CBoardItem * >( _GetGBM()->SetHoverObject( pOrgItem ) );

	if( !Remove( _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), pi_byQuantity ) )
	{
		_GetGBM()->SetHoverObject( NULL );
		return FALSE;
	}

	// set hover quantity & reset org object
	pHoverItem->SetQuantity( pi_byQuantity );	
	_GetGBM()->SetOrgObject( GetObject( _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo() ) );	

	return TRUE;
}

//==============================================================================

BOOL
CItemBoard::UI_SwapRequest_ProcessObject( void )
{
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );
	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );

	if( _GetGBM()->GetOrgBoardType() == _GetGBM()->GetTarBoardType() )
	{
		UI_SwapResult( TRUE );
		return TRUE;
	}	
	else if( _GetGBM()->GetOrgBoardType() == IBT_EQUIP &&
			 _GetGBM()->GetTarBoardType() == IBT_INVENTORY )
	{
		if( pTarItem->GetItemKind() != pHoverItem->GetItemKind() )
		{
			_SetSystemMsg( "같은 종류의 아이템이 아닙니다.\n다른 종류의 아이템은 교체될 수 없습니다." );
			return FALSE;
		}

		if( !_GetEquipBoard()->IsEquipableItem( pTarItem, _GetGBM()->GetOrgSocketNo() ) )
			return FALSE;

		if( pTarItem->GetItemKind() < MAX_EQUIP_PART )
			return m_pNetMsgProcessor_Item->EquipPartRequest_clzo( pTarItem->GetIndex() );
		else
			return m_pNetMsgProcessor_Item->EmbellishRequest_clzo( _GetGBM()->GetTarBoardType(), pTarItem->GetIndex(), pHoverItem->GetIndex() );
	}	
	else if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY &&
			 _GetGBM()->GetTarBoardType() == IBT_EQUIP )
	{
		if( pHoverItem->GetItemKind() < MAX_EQUIP_PART )
			return m_pNetMsgProcessor_Item->EquipPartRequest_clzo( pHoverItem->GetIndex() );
		else
			return m_pNetMsgProcessor_Item->EmbellishRequest_clzo( _GetGBM()->GetOrgBoardType(), pHoverItem->GetIndex(), pTarItem->GetIndex() );
	}
	else if( ( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY && _GetGBM()->GetTarBoardType() == IBT_ITEM_BELT ) ||
			 ( _GetGBM()->GetOrgBoardType() == IBT_ITEM_BELT && _GetGBM()->GetTarBoardType() == IBT_INVENTORY ) )
	 {
		_SetSystemMsg( "인벤토리와 벨트간의 아이템 스와핑은 지원되지 않습니다." );
		return FALSE;
	}
	else
	{
		_SetSystemMsg( "스와핑할 수 없습니다." );	
		return FALSE;
	}

	return TRUE;

}

void
CItemBoard::UI_SwapResult_ProcessObject( void )
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


	if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )
	{
		PlayWave( SND_ID_ITEM_ADD_IN_PACK );
	}
	else if( _GetGBM()->GetTarBoardType() == IBT_EQUIP )
	{
		PlayWave( SND_ID_ITEM_EQUIP );
	}
	else if( _GetGBM()->GetTarBoardType() == IBT_ITEM_BELT )
	{
		PlayWave( SND_ID_BELT_EQUIP );
	}
}

//==============================================================================

BOOL
CItemBoard::UI_CheckBeforeUse_CheckOriginalObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeUse_CheckOriginalObject() )
		return FALSE;

	CBoardItem * pOrgItem = static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );

	if( pOrgItem->IsEqualItemKind( IEPT_POTION ) )
	{
		if( _GetAvatar()->IsEqualUnitMountState( UMS_MOUNT ) )
		{
			_SetSystemMsg( "기갑 장비 탑승중에는 포션을 사용할 수 없습니다." );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL	
CItemBoard::UI_UseRequest_ProcessObject( void )
{
	return FALSE;
}

void
CItemBoard::UI_UseResult_ProcessObject( void )
{
}

//==============================================================================

BOOL
CItemBoard::UI_CheckBeforeAdd_CheckEmptySocket_Trade( void )
{
	// no check empty socket
	return FALSE;
}

BOOL
CItemBoard::UI_CheckBeforeAdd_CheckHoverObject_Trade( void )
{
	// 상점에서 구입할 아이템인 경우
	if( _GetGBM()->GetOrgBoardType() == IBT_SHOP )
	{
		UI_AddRequest_Trade();
		return TRUE;
	}

	return FALSE;
}

BOOL
CItemBoard::UI_CheckBeforeAdd_CheckTargetObject_Trade( void )
{
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );

	if( GetTradeItemByIndex( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo() ) )
	{
		_SetSystemMsg( "현재 거래중인 소켓입니다." );
		return TRUE;
	}

	if( !pTarItem )
	{		
		if( _GetShopTradeBoard()->IsTrading() )
		{
			// target socket에 구매할 아이템이 들어있는지 체크
			if( _GetShopTradeBoard()->GetTradeMode() == CShopTradeBoard::STM_BUY &&
				_GetShopTradeBoard()->GetTradeItem( _GetGBM()->GetTarBoardType(), _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo() ) )
				return TRUE;
		}
	}

	return FALSE;
}

BOOL
CItemBoard::UI_AddRequest_Trade( void )
{	
	if( _GetGBM()->GetOrgBoardType() == IBT_SHOP )
	{
		CBoardItem * pHoverItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );		

		// select buy item
		BOOL bResult;
		bResult = _GetShopTradeBoard()->SelectBuyItem( pHoverItem->GetIndex(),
													   pHoverItem->GetQuantity(),
													   _GetGBM()->GetTarBoardType(),
													   _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo() );
		_GetGBM()->SetHoverObject( NULL );

		return bResult;
	}
	else
	{

	}

	return TRUE;
}

//==============================================================================

BOOL		
CItemBoard::UI_CheckBeforRemove_CheckOriginalObject_Trade( void )
{
	CBoardItem * pOrgItem = static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );

	if( pOrgItem )
	{
		// 거래중인 아이템인지 체크
		if( GetTradeItemByIndex( pOrgItem->GetIndex() ) )
		{
			_SetSystemMsg( "현재 거래중인 아이템은 이동할 수 없습니다." );
			return TRUE;
		}
	}
	else
	{
		if( _GetShopTradeBoard()->IsTrading() )
		{
			// deselect buy item
			if( _GetShopTradeBoard()->IsTradingItem( _GetGBM()->GetOrgBoardType(), _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo() ) )
			{
				UI_RemoveRequest_Trade();
				return TRUE;
			}
		}
	}

	return FALSE;
}


BOOL
CItemBoard::UI_RemoveRequest_Trade( void )
{
	if( _GetShopTradeBoard()->IsTrading() )
	{
		CBoardItem * pShopItem = _GetShopTradeBoard()->GetTradeItem( _GetGBM()->GetOrgBoardType(),
																	 _GetGBM()->GetOrgPackNo(),
																	 _GetGBM()->GetOrgSocketNo() );
		if( !pShopItem )
			return FALSE;

		// set hover object
		CBoardItem * pHoverItem = static_cast< CBoardItem * >( _GetGBM()->SetHoverObject( pShopItem ) );			

		
		// deselect buy item
		_GetShopTradeBoard()->DeselectBuyItem( _GetGBM()->GetOrgBoardType(),
											   _GetGBM()->GetOrgPackNo(),
											   _GetGBM()->GetOrgSocketNo() );

		// reset org object
		pShopItem = _GetShopTradeBoard()->GetShopItem( pHoverItem->GetIndex() );

		_GetGBM()->SetOrgObject( IBT_SHOP, 0, 0, pShopItem );
	}
	else if( _GetPlayerTradeBoard()->IsTrading() )
	{

	}

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CItemBoard::UI_Trade_By_HotKey( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{		
	CBoardItem * pOrgItem = GetItem( pi_byPackNo, pi_bySocketNo );

	if( _GetShopTradeBoard()->IsTrading() )
	{		
		if( !pOrgItem ) // 거래중인 아이템
			UI_RemoveRequest( pi_byPackNo, pi_bySocketNo, 1 );
		else
			UI_RemoveRequest( pi_byPackNo, pi_bySocketNo, pOrgItem->GetQuantity() );

		if( !_GetGBM()->HaveHoverObject() )
			return FALSE;

		_GetShopTradeBoard()->UI_AddRequest( 0, 0 );
	}
	else if( _GetPlayerTradeBoard()->IsTrading() )
	{
		if( pOrgItem )
			_GetPlayerTradeBoard()->SelectItem_Request( m_byBoardType, pi_byPackNo, pi_bySocketNo, pOrgItem, TRUE );
	}	

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 상점이나 기타 거래창에 올려진 아이템을 가져온다.
CBoardItem *
CItemBoard::GetTradeItemByIndex( DWORD pi_dwIndex )
{
	if( m_byBoardType > IBT_ITEM_BELT )
		return NULL;

	CBoardItem * pTradeItem = NULL;

	if( _GetShopTradeBoard()->IsOpen() )
	{
		if( _GetShopTradeBoard()->GetTradeMode() == CShopTradeBoard::STM_SELL )
			pTradeItem = _GetShopTradeBoard()->GetTradeItemByIndex( pi_dwIndex );
	}
	else if( _GetPlayerTradeBoard()->IsOpen() )
	{
		pTradeItem = _GetPlayerTradeBoard()->GetTradeItemByIndex( pi_dwIndex );
	}
	else if( _GetOreProcBoard()->IsOpen() )
	{
		CBoardItem * pOreItem = _GetOreProcBoard()->GetOre();
		if( pOreItem && pOreItem->IsEqual( pi_dwIndex ) )
			pTradeItem = pOreItem;
	}
	else if( _GetItemUpgradeBoard()->IsOpen() )
	{
		pTradeItem = _GetItemUpgradeBoard()->GetItemByIndex( pi_dwIndex );
	}

	return pTradeItem;
}

CBoardItem *
CItemBoard::GetTradeItemByIndex( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	if( m_byBoardType > IBT_ITEM_BELT )
		return NULL;

	CBoardItem * pTradeItem = NULL;

	if( _GetShopTradeBoard()->IsOpen() )
	{
		if( _GetShopTradeBoard()->GetTradeMode() == CShopTradeBoard::STM_SELL )
			pTradeItem = _GetShopTradeBoard()->GetTradeItem( m_byBoardType, pi_byPackNo, pi_bySocketNo );
	}
	else if( _GetPlayerTradeBoard()->IsOpen() )
	{
		pTradeItem = _GetPlayerTradeBoard()->GetTradeItem( m_byBoardType, pi_byPackNo, pi_bySocketNo );
	}
	else if( _GetOreProcBoard()->IsOpen() )
	{
		CBoardItem * pOreItem = _GetOreProcBoard()->GetOre();
		if( !pOreItem )
			return NULL;

		if( _GetOreProcBoard()->GetOrePackNo() == pi_byPackNo &&
			_GetOreProcBoard()->GetOreSocketNo() == pi_bySocketNo )
			pTradeItem = pOreItem;
	}
	else if( _GetItemUpgradeBoard()->IsOpen() )
	{
		pTradeItem = _GetItemUpgradeBoard()->GetTradeItem( pi_byPackNo, pi_bySocketNo );
	}

	return pTradeItem;
}

// ================================================================================

void
CItemBoard::UI_SetUselessSocket( BYTE pi_byPackNo, BYTE pi_bySocketNo, BOOL pi_bFlag )
{
	// set ui
	CGUIItemWindow * pItemWindow = static_cast< CGUIItemWindow * >( m_uiWindow );
	if( pItemWindow )
		pItemWindow->SetUselessSocket( pi_byPackNo, pi_bySocketNo, pi_bFlag );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CItemBoard::FrameMove( void )
{
	CObjectBoard::FrameMove();
}

LRESULT
CItemBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UI_MOUSE_MSG	stRecvMsg( lParam );

	switch( uMsg )
	{
		case WM_BOARD_ADD_OBJECT	:
			UI_AddRequest( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
			break;

		case WM_BOARD_REMOVE_OBJECT	:
			UI_RemoveRequest( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo, stRecvMsg.m_byQuantity );
			break;

		case WM_BOARD_CHECK_REMOVE_OBJECT :
			{
				CBoardItem * pOrgItem = static_cast< CBoardItem * >( GetObject( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo ) );
				if( !pOrgItem ) // -> 아이템이 없으면 거래중인 아이템일 수 있다.
				{
					UI_RemoveRequest( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo, 1 );
				}
				else
				{
					// 숫자 입력창을 띄우기 전에 거래중인 아이템인지 체크
					if( GetTradeItemByIndex( pOrgItem->GetIndex() ) )
						break;

					if( ( pOrgItem->IsSplitable() ) && ( pOrgItem->GetQuantity() > 1 ) )
					{
						POINT pDlgPos = { gMouse.x, gMouse.y };
						_GUINumInputDlg( m_uiWindow, pDlgPos, pOrgItem->GetQuantity(),
										 INPUT_DLG_ID, pOrgItem->GetQuantity() );
					}
					else
					{
						UI_RemoveRequest( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo, 1 );
					}				
				}
			}
			break;

		case WM_BOARD_USE_OBJECT	:
			UI_UseRequest( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
			break;

		case WM_BOARD_TRADE_HOT_KEY	:
			UI_Trade_By_HotKey( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
			break;

		case WM_BOARD_HOVER_OBJECT	:
			{		
				CBoardItem * pItem;
				BYTE byShopMode = 0xFF;

				// get item
				pItem = GetItem( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );

				if( _GetShopTradeBoard()->IsTrading() )
				{
					if( _GetShopTradeBoard()->GetTradeMode() == CShopTradeBoard::STM_BUY )
					{	
						// 거래중인 아이템인 경우
						if( !pItem )
						{
							pItem = _GetShopTradeBoard()->GetTradeItem( m_byBoardType, stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
							byShopMode = CShopTradeBoard::STM_BUY;
						}
					}
					else if( _GetShopTradeBoard()->GetTradeMode() == CShopTradeBoard::STM_SELL )
					{
						if( pItem->IsSellable() )
							byShopMode = CShopTradeBoard::STM_SELL;
					}
					else if( _GetShopTradeBoard()->GetTradeMode() == CShopTradeBoard::STM_REPAIR )
					{
						if( pItem->IsRepairable() )
							byShopMode = CShopTradeBoard::STM_REPAIR;
					}
				}				

				DisplayItemInfo( pItem, byShopMode );
			}
			break;
	}

	return CObjectBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CBoardItem *	
CItemBoard::GetItem( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	return static_cast< CBoardItem * >( GetObject( pi_byPackNo, pi_bySocketNo ) );
}

CBoardItem *
CItemBoard::GetItemByIndex( DWORD pi_dwIndex )
{
	return static_cast< CBoardItem * >( GetObjectByIndex( pi_dwIndex ) );
}

CBoardItem *
CItemBoard::GetItemByIndex( BYTE & po_byPackNo, BYTE & po_bySocketNo, DWORD pi_dwIndex )
{
	CBoardItem * pOrgItem = static_cast< CBoardItem * >( GetObjectByIndex( pi_dwIndex ) );
	if( pOrgItem )
		GetObjectPosition( po_byPackNo, po_bySocketNo, pi_dwIndex );

	return pOrgItem;
}

//------------------------------------------------------------------------------

CBoardItem *
CItemBoard::GetItemByItemType( BYTE pi_byItemKind )
{
	if( pi_byItemKind >= MAX_ITEM_TYPE )
		return NULL;

	CBoardItem * pItem;
	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[i].GetMaxSocketNum(); ++j )
		{
			pItem = GetItem( i, j );
			if( pItem &&
				pItem->GetItemKind() == pi_byItemKind )
			{
				return pItem;
			}
		}		
	}

	return NULL;
}

BOOL
CItemBoard::GetItemByItemType( CBoardItem & po_clsItem, BYTE & po_byPackNo, BYTE & po_bySocketNo, BYTE pi_byItemKind )
{
	if( pi_byItemKind >= MAX_ITEM_TYPE )
		return FALSE;

	CBoardItem * pItem;
	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[i].GetMaxSocketNum(); ++j )
		{
			pItem = GetItem( i, j );
			if( pItem &&
				pItem->GetItemKind() == pi_byItemKind )
			{
				memcpy( &po_clsItem, pItem, sizeof( CBoardItem ) );
				po_byPackNo = i;
				po_bySocketNo = j;
				
				return TRUE;
			}
		}		
	}

	return FALSE;
}
			
// pi_byItemType 과 pi_dwDTIndex을 갖는 아이템 중에서 pi_dwItemNum 이상을 합칠 수 있는 아이템을 찾는다.
// pi_bIncludeOtherBoard가 TRUE이면 다른보드에 올려진 아이템은 제외한다.
CBoardItem *
CItemBoard::GetLackOfQuantityItemByDTIndex( BYTE & pio_byPackNo, BYTE & po_bySocketNo,
										    BYTE pi_byItemKind, DWORD pi_dwDTIndex, DWORD pi_dwItemNum,
											BOOL pi_bIncludeOtherBoard )
{
	// pack no list
	BYTE	i;
	BYTE	byPackNum = 1;
	BYTE	byPackNoList[] = { m_byActivePackNo, 0xFF, 0xFF, 0xFF, 0xFF };
	
	// pio_byPackNo가 0xFF라면 모든 가방을 다 찾는다.
	if( pio_byPackNo == 0xFF )
	{
		BYTE byMaxPackNum = m_byMaxPackNum;
		if( byMaxPackNum > 5 )
			byMaxPackNum = 5;
			
		for( i = 0; i < byMaxPackNum; ++i )
		{
			if( m_listItemPack[i].IsExist() && ( i != m_byActivePackNo ) )
				byPackNoList[byPackNum++] = i;
		}
	}
	else
	{
		byPackNoList[0] = pio_byPackNo;
	}

	// get item
	CBoardItem * pItem = NULL;
	for( i = 0; i < byPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[ byPackNoList[i] ].GetMaxSocketNum(); ++j )
		{
			pItem = GetItem( byPackNoList[i], j );

			if( pItem )
			{
				if( ( pItem->GetItemKind() == pi_byItemKind ) &&
					( pItem->GetDTIndex() == pi_dwDTIndex ) )
				{
					if( pi_bIncludeOtherBoard && GetTradeItemByIndex( pItem->GetIndex() ) )
						continue;

					if( ( pItem->GetQuantity() + pi_dwItemNum ) <= MAX_ITEM_QUANTITY )
					{
						pio_byPackNo	= byPackNoList[i];
						po_bySocketNo	= j;
						return pItem;
					}
				}
			}
		}
	}

	return NULL;
}
		
// pi_byItemType 과 pi_dwDTIndex을 갖는 아이템 중에서 pi_dwItemNum 이상 남아있는 아이템을 찾는다.
CBoardItem *	
CItemBoard::GetRemainOfQuantityItemByDTIndex( BYTE & pio_byPackNo, BYTE & po_bySocketNo, BYTE pi_byItemKind, DWORD pi_dwDTIndex, DWORD pi_dwItemNum )
{
	// pack no list
	BYTE	i;
	BYTE	byPackNum = 1;
	BYTE	byPackNoList[] = { m_byActivePackNo, 0xFF, 0xFF, 0xFF, 0xFF };

	// pio_byPackNo가 0xFF라면 모든 가방을 다 찾는다.
	if( pio_byPackNo == 0xFF )
	{
		BYTE byMaxPackNum = m_byMaxPackNum;
		if( byMaxPackNum > 5 )
			byMaxPackNum = 5;

		for( i = 0; i < byMaxPackNum; ++i )
		{
			if( m_listItemPack[i].IsExist() && ( i != m_byActivePackNo ) )
				byPackNoList[byPackNum++] = i;
		}
	}
	else
	{
		byPackNoList[0] = pio_byPackNo;
	}

	// get item
	CBoardItem * pBoardItem = NULL;
	for( i = 0; i < byPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[ byPackNoList[i] ].GetMaxSocketNum(); ++j )
		{
			pBoardItem = GetItem( byPackNoList[i], j );
			if( pBoardItem && pBoardItem->IsSameKind( pi_byItemKind, pi_dwDTIndex ) )
			{
				if( pBoardItem->GetQuantity() >= pi_dwItemNum )
				{
					pio_byPackNo	= byPackNoList[i];
					po_bySocketNo	= j;
					return pBoardItem;
				}
			}
		}
	}

	return NULL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 인벤토리에서 제일 먼저 검색되는 장착가능한 아이템의 위치를 반환	
BOOL
CItemBoard::FindEquipmentPosition( BYTE & po_byPackNo, BYTE & po_bySocketNo )
{
	CBoardItem * pItem;
	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[i].GetMaxSocketNum(); ++j )
		{
			pItem = GetItem( i, j );
			if( !pItem )
				continue;
			
			if( pItem->IsEquipmentItem() &&
				_GetAvatar()->IsUsableItem( pItem ) )
			{
				po_byPackNo = i;
				po_bySocketNo = j;

				return TRUE;
			}
		}		
	}

	return FALSE;	
}

//------------------------------------------------------------------------------

// 거래할 아이템이 들어갈 위치를 찾는다.
BOOL	
CItemBoard::FindTradeItemPosition( BYTE & po_byPackNo, BYTE & po_bySocketNo )
{
	return FindEmptySocket_IncludeOtherBoard( po_byPackNo, po_bySocketNo );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

char
CItemBoard::FindEmptySocket_IncludeOtherBoard( BYTE pi_byPackNo )
{
	if( _GetShopTradeBoard()->IsOpen() )
	{
		return FindEmptySocket_IncludeShopTradeList( pi_byPackNo );
	}
	else if( _GetOreProcBoard()->IsOpen() )
	{
		return FindEmptySocket_IncludeOreProcList( pi_byPackNo );
	}
	else if( _GetItemUpgradeBoard()->IsOpen() )
	{
		return FindEmptySocket_IncludeUpgradeList( pi_byPackNo );
	}

	return FindEmptySocket( pi_byPackNo );
}

char
CItemBoard::FindEmptySocket_IncludeShopTradeList( BYTE pi_byPackNo )
{
	if( pi_byPackNo >= m_byMaxPackNum )
		return ID_INVALID;
	
	for( int i = 0; i < m_listItemPack[pi_byPackNo].GetMaxSocketNum(); ++i )
	{
		if( !m_listItemPack[pi_byPackNo].GetObject( i ) )
		{			
			if( !_GetShopTradeBoard()->IsTradingItem( m_byBoardType, pi_byPackNo, i ) )		
				return i;
		}
	}

	return ID_INVALID;
}

char
CItemBoard::FindEmptySocket_IncludeOreProcList( BYTE pi_byPackNo )
{
	if( pi_byPackNo >= m_byMaxPackNum )
		return ID_INVALID;

	for( int i = 0; i < m_listItemPack[pi_byPackNo].GetMaxSocketNum(); ++i )
	{
		if( !m_listItemPack[pi_byPackNo].GetObject( i ) )
		{
			if( !( ( _GetOreProcBoard()->GetOrePackNo() == pi_byPackNo ) &&
				   ( _GetOreProcBoard()->GetOreSocketNo() == i ) ) )
				return i;
		}
	}

	return ID_INVALID;
}

char
CItemBoard::FindEmptySocket_IncludeUpgradeList( BYTE pi_byPackNo )
{
	if( pi_byPackNo >= m_byMaxPackNum )
		return ID_INVALID;

	for( int i = 0; i < m_listItemPack[pi_byPackNo].GetMaxSocketNum(); ++i )
	{
		if( !m_listItemPack[pi_byPackNo].GetObject( i ) )
		{
			if( !_GetItemUpgradeBoard()->GetTradeItem( pi_byPackNo, i ) )
				return i;			
		}
	}

	return ID_INVALID;
}

//=====================================================================================

BOOL
CItemBoard::FindEmptySocket_IncludeOtherBoard( BYTE & po_byPackNo, BYTE & po_bySocketNo )
{
	if( _GetShopTradeBoard()->IsOpen() )
	{
		return FindEmptySocket_IncludeShopTradeList( po_byPackNo, po_bySocketNo );
	}	
	else if( _GetItemUpgradeBoard()->IsOpen() )
	{
		return FindEmptySocket_IncludeUpgradeList( po_byPackNo, po_bySocketNo );
	}
	else if( _GetOreProcBoard()->IsOpen() )
	{
		return FindEmptySocket_IncludeOreProcList( po_byPackNo, po_bySocketNo );
	}

	return FindEmptySocket( po_byPackNo, po_bySocketNo );
}

BOOL
CItemBoard::FindEmptySocket_IncludeShopTradeList( BYTE & po_byPackNo, BYTE & po_bySocketNo )
{
	char byPackNo	= -1;
	char bySocketNo	= -1;

	byPackNo = GetActivePackNo();

	// current pack
	bySocketNo = FindEmptySocket_IncludeShopTradeList( byPackNo );
	
	// other pack
	if( bySocketNo == -1 )
	{
		for( int i = 0; i < m_byMaxPackNum; ++i )
		{
			byPackNo = i;

			if( ( m_listItemPack[i].IsExist() ) && ( i != GetActivePackNo() ) )
			{
				bySocketNo = FindEmptySocket_IncludeShopTradeList( byPackNo );
				if( bySocketNo != ID_INVALID )
					break;
			}
		}
	}

	// if founded
	if( bySocketNo != ID_INVALID )
	{
		po_byPackNo		= byPackNo;
		po_bySocketNo	= bySocketNo;

		return TRUE;
	}

	return FALSE;
}

BOOL
CItemBoard::FindEmptySocket_IncludeOreProcList( BYTE & po_byPackNo, BYTE & po_bySocketNo )
{
	char byPackNo		= -1;
	char bySocketNo	= -1;
	
	byPackNo = GetActivePackNo();

	// current pack
	bySocketNo = FindEmptySocket_IncludeOreProcList( byPackNo );

	// other pack
	if( bySocketNo == -1 )
	{
		for( int i = 0; i < m_byMaxPackNum; ++i )
		{
			byPackNo = i;

			if( ( m_listItemPack[i].IsExist() ) && ( i != GetActivePackNo() ) )
			{
				bySocketNo = FindEmptySocket_IncludeOreProcList( byPackNo );
				if( bySocketNo != ID_INVALID )
					break;
			}
		}
	}

	// if founded
	if( bySocketNo != ID_INVALID )
	{
		po_byPackNo		= byPackNo;
		po_bySocketNo	= bySocketNo;

		return TRUE;
	}

	return FALSE;
}

BOOL
CItemBoard::FindEmptySocket_IncludeUpgradeList( BYTE & po_byPackNo, BYTE & po_bySocketNo )
{
	char byPackNo		= -1;
	char bySocketNo	= -1;
	
	byPackNo = GetActivePackNo();

	// current pack
	bySocketNo = FindEmptySocket_IncludeUpgradeList( byPackNo );

	// other pack
	if( bySocketNo == -1 )
	{
		for( int i = 0; i < m_byMaxPackNum; ++i )
		{
			byPackNo = i;

			if( ( m_listItemPack[i].IsExist() ) && ( i != GetActivePackNo() ) )
			{
				bySocketNo = FindEmptySocket_IncludeUpgradeList( byPackNo );
				if( bySocketNo != ID_INVALID )
					break;
			}
		}
	}

	// if founded
	if( bySocketNo != ID_INVALID )
	{
		po_byPackNo		= byPackNo;
		po_bySocketNo	= bySocketNo;

		return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // _NEW_UI_