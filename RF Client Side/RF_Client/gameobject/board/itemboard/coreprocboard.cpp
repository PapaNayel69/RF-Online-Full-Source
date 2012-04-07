////////////////////////////////////////////////////////////////////////////////
//
// CInvenBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../Manager/CCharacterMgr.h"
#include "../Common/CBoardManager.h"

#include "../../../Network/NetMsgProcessor/Resource/CNetMsgProcessor_Resource.h"

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

COreProcBoard::COreProcBoard()
{
	m_byOrePackNo	= ID_INVALID;
	m_byOreSocketNo	= ID_INVALID;

	m_m_pNetMsgProcessor_Resource	= static_cast< CNetMsgProcessor_Resource * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_RESOURCE ) );
}

COreProcBoard::~COreProcBoard()
{
}

void
COreProcBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiOreProcWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );

	m_uiWindow->SetPos( _GetUIScreen()->GetSizeX() - m_uiWindow->GetSizeX() * 2 - 15, _GetUIScreen()->GetPosY() );
}

void
COreProcBoard::ClearIngot( void )
{
	Clear();	
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


BOOL
COreProcBoard::UI_CheckBeforeAdd_CheckHoverObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeAdd_CheckHoverObject() )
		return FALSE;


	if( _GetGBM()->GetOrgBoardType() != IBT_INVENTORY )
		return FALSE;

	// check if ore 
	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pHoverItem->IsEqualItemKind( IEPT_ORE ) )
		return FALSE;

	return TRUE;
}

BOOL
COreProcBoard::UI_CheckBeforeAdd_CheckTargetObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeAdd_CheckTargetObject() )
		return FALSE;

	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );

	// check whether swap
	if( pHoverItem && !m_clsOre.IsEmpty() )
	{
		if( m_clsOre.IsEqual( pHoverItem->GetIndex() ) &&
			_GetGBM()->GetOrgPackNo() == m_byOrePackNo &&
			_GetGBM()->GetOrgSocketNo() == m_byOreSocketNo )
		{
			// append
			m_clsOre.SetQuantity( m_clsOre.GetQuantity() + pHoverItem->GetQuantity() );

			_GetGBM()->SetHoverObject( NULL );
		}
		else 
		{
			// swap
			UI_SwapRequest( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo() );			
		}

		return FALSE;
	}

	return TRUE;	
}

// =============================================================================

BOOL
COreProcBoard::UI_AddRequest_ProcessObject( void )
{
	UI_AddResult( TRUE );	
	
	return TRUE;
}

void
COreProcBoard::UI_AddResult_ProcessObject( void )
{	
	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );

	// set ore
	UI_SetOre( _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), pHoverItem->GetQuantity(), FALSE );		
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
COreProcBoard::UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity )
{
	if( !CObjectBoard::UI_CheckBeforeRemove_CheckOriginalObject( pi_byQuantity ) )		
	{
		// Ore
		if( ( _GetGBM()->GetOrgSocketNo() == 0xFF ) && ( !m_clsOre.IsEmpty() ) )		
			return TRUE;
		

		return FALSE;
	}

	CBoardItem * pOrgItem = static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	if( pOrgItem->GetQuantity() < pi_byQuantity )
		return FALSE;

	return TRUE;
}

// =============================================================================

BOOL
COreProcBoard::UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity )
{
	CBoardItem * pHoverItem	= NULL;	

	// Ore
	if( _GetGBM()->GetOrgSocketNo() == 0xFF )
	{
		// set hover item
		_GetGBM()->SetHoverObject( &m_clsOre );				
		
		_GetGBM()->SetOrgObject( IBT_INVENTORY, m_byOrePackNo, m_byOreSocketNo, NULL );

		RemoveOre();
	}
	// Ingot
	else
	{
		UI_Ingot_To_Inventory_Request( _GetGBM()->GetOrgSocketNo() );

		/*
		CBoardItem * pOrgItem = static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );

		// set hover item
		_GetGBM()->SetHoverObject( pOrgItem );

		if( !Remove( _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), pOrgItem->GetQuantity() ) )
		{
			_GetGBM()->SetHoverObject( NULL );
			return FALSE;
		}

		// reset org object
		_GetGBM()->SetOrgObject( IBT_ORE_PROCESSING, _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(),
								 GetObject( _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo() ) );	
		*/
	}	

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
COreProcBoard::UI_SwapRequest_ProcessObject( void )
{
	UI_SwapResult( TRUE );

	return TRUE;
}

void
COreProcBoard::UI_SwapResult_ProcessObject( void )
{	
	// swap ore
	UI_UnsetOre( TRUE );
	
	CBoardItem *	pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );	

	UI_SetOre( _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), pHoverItem->GetQuantity(), FALSE );	
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 원석 가공
BOOL			
COreProcBoard::UI_OreProcessing_Request( void )
{
	if( _GetGBM()->IsRequestProcess() )
	{
		_SetSystemMsg( "이미 원석 가공을 요청하였습니다. 잠시만 기다려 주십시오." );
		return FALSE;
	}

	// check board - 원석을 가공하기 위해서는 임시 저장창이 모두 비워져 있어야만한다.
	if( GetEmptySocketNum() < MAX_ORE_PROC_SOCKET )
	{
		_SetSystemMsg( "가공을 시작하려면 먼저 임시 저장창을 모두 비워 주십시오." );
		return FALSE;
	}	

	// check ore
	if( m_clsOre.IsEmpty() )
		return FALSE;

	// check dalant
	if( _GetAvatar()->GetDalant() < ( m_clsOre.GetOreProcPrice( m_clsOre.GetQuantity() ) ) )
	{
		_SetSystemMsg( "원석 가공에 필요한 %s가 부족합니다.", _GetAvatarMoneyName() );
		return FALSE;
	}

	// 가공 시작 메세지를 보낸다.
	m_m_pNetMsgProcessor_Resource->OreCuttingRequest_clzo();	

	return TRUE;
}

// 원석 가공 결과
BOOL
COreProcBoard::UI_OreProcessing_Result( void )
{
	RemoveOre();

	return TRUE;
}

// 원석 가공 취소
BOOL	
COreProcBoard::UI_OreProcessing_Cancel( void )
{
	ReturnItemToItemBoard();

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 잉갓 처분 요청
BOOL
COreProcBoard::UI_SellIngot_Request( void )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;

	m_m_pNetMsgProcessor_Resource->CuttingCompleteRequest_clzo();	

	return TRUE;
}

BOOL	
COreProcBoard::UI_SellIngot_Result( void )
{
	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 잉갓 이동 요청 ( to Inventory Board )
BOOL			
COreProcBoard::UI_Ingot_To_Inventory_Request( BYTE pi_bySocketNo )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;

	CBoardItem * pOrgIngot = GetItem( pi_bySocketNo );
	if( !pOrgIngot )
		return FALSE;

	DWORD	dwInvenItemIndex = ID_INVALID;
	BYTE	byPackNo	= 0xFF;
	BYTE	bySocketNo	= 0xFF;
	CBoardItem *	pInvenItem;

	// 수량 체크
	BYTE	byMoveQuantity = pOrgIngot->GetQuantity();
	if( byMoveQuantity > MAX_ITEM_QUANTITY )
		byMoveQuantity = MAX_ITEM_QUANTITY;

	// 동일한 아이템을 찾는다. ( 수량이 1개라도 비어있는 아이템 )
	pInvenItem = _GetInventoryBoard()->GetLackOfQuantityItemByDTIndex( byPackNo, bySocketNo,
																	   pOrgIngot->GetItemKind(),
																	   pOrgIngot->GetDTIndex(),
																	   1 );
	if( pInvenItem )
	{
		m_byInvenPackNo		= byPackNo;
		m_byInvenSocketNo	= bySocketNo;
		dwInvenItemIndex	= pInvenItem->GetIndex();

		if( byMoveQuantity > MAX_ITEM_QUANTITY - pInvenItem->GetQuantity() )
			byMoveQuantity = MAX_ITEM_QUANTITY - pInvenItem->GetQuantity();
	}
	// 동일한 아이템이 없다면 빈공간을 찾는다.
	else
	{
		m_byInvenPackNo		= ID_INVALID;
		m_byInvenSocketNo	= ID_INVALID;

		if( !_GetInventoryBoard()->FindEmptySocket_IncludeOtherBoard( byPackNo, bySocketNo ) )
		{
			_SetSystemMsg( "인벤토리에 빈 공간이 없습니다." );
			return FALSE;
		}
	}

	//--------------------------------------------------------------------------	
	
	_GetGBM()->SetTarObject( IBT_INVENTORY, byPackNo, bySocketNo, pInvenItem );

	// 네트웍 메세지 전송
	m_m_pNetMsgProcessor_Resource->OreIntoBagRequest_clzo( pOrgIngot->GetDTIndex(),
														   byMoveQuantity,
														   dwInvenItemIndex );

	return TRUE;
}

// 잉갓 이동 요청에 대한 결과 ( to Inventory Board )
BOOL
COreProcBoard::UI_Ingot_To_Inventory_Result( DWORD pi_dwItemIndex )
{
	// get item
	CBoardItem * pOrgItem = GetItem( _GetGBM()->GetOrgSocketNo() );
	if( !pOrgItem )
		return FALSE;

	BYTE byMoveQuantity = pOrgItem->GetQuantity();

	// 인벤토리에 동일한 아이템이 있을 경우
	if( ( m_byInvenPackNo != 0xFF ) && ( m_byInvenSocketNo != 0xFF ) )
	{
		CBoardItem * pInvenItem = _GetInventoryBoard()->GetItem( m_byInvenPackNo, m_byInvenSocketNo );
		if( pInvenItem )
		{
			if( byMoveQuantity > MAX_ITEM_QUANTITY - pInvenItem->GetQuantity() )
				byMoveQuantity = MAX_ITEM_QUANTITY - pInvenItem->GetQuantity();

			pInvenItem->SetIndex( pi_dwItemIndex );
			pInvenItem->SetServerIndex( pi_dwItemIndex );
			pInvenItem->SetQuantity( pInvenItem->GetQuantity() + byMoveQuantity );
		}
		
		PlayWave( SND_ID_ITEM_ADD_IN_PACK );

		_SetSystemMsg( "%s %d개가 %d번째 가방 %d번째 칸에 합쳐졌습니다.", pOrgItem->GetName(),
					   byMoveQuantity, m_byInvenPackNo + 1, m_byInvenSocketNo + 1 );
	}
	// 인벤토리에 동일한 아이템이 없을 경우
	else
	{
		if( byMoveQuantity > MAX_ITEM_QUANTITY )
			byMoveQuantity = MAX_ITEM_QUANTITY;

		CBoardItem clsInvenItem;
		memcpy( &clsInvenItem, pOrgItem, sizeof( CBoardItem ) );
		clsInvenItem.SetIndex( pi_dwItemIndex );
		clsInvenItem.SetServerIndex( pi_dwItemIndex );
		clsInvenItem.SetQuantity( byMoveQuantity );

		_GetInventoryBoard()->Add( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo(), &clsInvenItem );

		PlayWave( SND_ID_ITEM_ADD_IN_PACK );

		_SetSystemMsg( "%s %d개가 %d번째 가방 %d번째 칸으로 이동되었습니다.", pOrgItem->GetName(),
					   byMoveQuantity, _GetGBM()->GetTarPackNo() + 1, _GetGBM()->GetTarSocketNo() + 1 );
	}


	Remove( 0, _GetGBM()->GetOrgSocketNo(), byMoveQuantity );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
COreProcBoard::ReturnItemToItemBoard( void )
{
	UI_UnsetOre( TRUE );	
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
COreProcBoard::UI_SetOre( BYTE pi_byPackNo, BYTE pi_bySocketNo, BYTE pi_byOreNum, BOOL pi_bTradeByHotKey )
{
	CBoardItem * pItem = NULL;
	if( pi_bTradeByHotKey )
		pItem = _GetInventoryBoard()->GetItem( pi_byPackNo, pi_bySocketNo );
	else
		pItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );	

	if( !pItem )
		return FALSE;
	if( !pItem->IsEqualItemKind( IEPT_ORE ) )
		return FALSE;


	if( !m_clsOre.IsEmpty() )
	{
		UI_UnsetOre( pi_bTradeByHotKey );
	}
	
	memcpy( &m_clsOre, pItem, sizeof( CBoardItem ) );
	m_byOrePackNo	= pi_byPackNo;
	m_byOreSocketNo	= pi_bySocketNo;	

	// set ui
	m_uiOreProcWindow.TakeInInputOre( m_clsOre.GetMeshID(), m_clsOre.GetIconID(), m_clsOre.GetQuantity() );
	m_clsOre.SetIcon( m_uiOreProcWindow.GetOre() );	
	_GetInventoryBoard()->UI_SetUselessSocket( pi_byPackNo, pi_bySocketNo, TRUE );

	m_uiOreProcWindow.SetProcPrice( m_clsOre.GetOreProcPrice() * m_clsOre.GetQuantity() );	


	if( pi_bTradeByHotKey )
		_GetInventoryBoard()->Remove( pi_byPackNo, pi_bySocketNo );
	else
		_GetGBM()->SetHoverObject( NULL );	

	return TRUE;	
}

BOOL
COreProcBoard::UI_UnsetOre( BOOL pi_bTradeByHotKey )
{
	if( m_clsOre.IsEmpty() )
		return FALSE;

	if( !_GetGBM()->ReturnItemToItemBoard( IBT_INVENTORY, m_byOrePackNo, m_byOreSocketNo, &m_clsOre ) )
		return FALSE;

	RemoveOre();

	/*
	if( pi_bTradeByHotKey )
	{
		CBoardItem * pInvenItem = _GetInventoryBoard()->GetItem( m_byOrePackNo, m_byOreSocketNo );
		if( !pInvenItem )
			_GetInventoryBoard()->Add( m_byOrePackNo, m_byOreSocketNo, &m_clsOre );
		else
			pInvenItem->SetQuantity( pInvenItem->GetQuantity() + m_clsOre.GetQuantity() );
	}
	else
	{
		_GetGBM()->SetHoverObject( &m_clsOre );
		_GetGBM()->SetOrgObject( IBT_INVENTORY, m_byOrePackNo, m_byOreSocketNo, NULL );
	}

	RemoveOre();
	*/

	return TRUE;
}

void			
COreProcBoard::RemoveOre( void )
{
	// set ui
	m_uiOreProcWindow.TakeOutInputOre();
	_GetInventoryBoard()->UI_SetUselessSocket( m_byOrePackNo, m_byOreSocketNo, FALSE );

	m_uiOreProcWindow.SetProcPrice( 0 );	

	m_clsOre.SetEmpty();
	m_byOrePackNo = 0xFF;
	m_byOreSocketNo = 0xFF;
}

CBoardItem *
COreProcBoard::GetOreInInventory( void )
{
	return _GetInventoryBoard()->GetItem( m_byOrePackNo, m_byOreSocketNo );
}

CBoardItem *
COreProcBoard::GetLackOfQuantityItemByDTIndex( BYTE & po_bySocketNo, DWORD pi_dwDTIndex )
{
	BYTE byPackNo;

	return CItemBoard::GetLackOfQuantityItemByDTIndex( byPackNo, po_bySocketNo, IEPT_INGOT, pi_dwDTIndex, 0 );
}

CBoardItem *
COreProcBoard::GetItem( BYTE pi_bySocketNo )
{
	return CItemBoard::GetItem( 0, pi_bySocketNo );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

LRESULT
COreProcBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UI_MOUSE_MSG	stRecvMsg( lParam );

	switch( uMsg )
	{
	case WM_BOARD_REMOVE_OBJECT		:
	case WM_BOARD_TRADE_HOT_KEY		:
		if( stRecvMsg.m_bySocketNo == 0xFF )
		{
			ReturnItemToItemBoard();			
		}
		else
		{
			UI_RemoveRequest( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo, stRecvMsg.m_byQuantity );
		}
		return 1;

	case WM_ORE_PROC_BOARD_PROCESS	:
		UI_OreProcessing_Request();
		break;

	case WM_ORE_PROC_BOARD_SELL		:
		UI_SellIngot_Request();
		break;

	case WM_CLOSE_WINDOW			:
		ReturnItemToItemBoard();
		break;

	case WM_BOARD_HOVER_OBJECT		:
		if( stRecvMsg.m_bySocketNo == 0xFF )
		{
			if( !m_clsOre.IsEmpty() )
				DisplayItemInfo( &m_clsOre, 0xFF );
		}
		else
		{
			return CItemBoard::MsgProc( hWnd, uMsg, wParam, lParam );
		}
		break;
	}

	return CItemBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}
