////////////////////////////////////////////////////////////////////////////////
//
// CBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../Manager/CCharacterMgr.h"
#include "../Common/CBoardManager.h"

#include "../../../Network/NetMsgProcessor/Unit/CNetMsgProcessor_Unit.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void	DisplayItemInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode );
void	DisplayUnitParts( CUnitPartsItem * pi_pUnitParts, BYTE pi_byShopMode );

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CUnitStorehouseBoard::CUnitStorehouseBoard()
{
	m_byCurUnitSocketNo		= 0;
	m_pDeliveryUnit			= NULL;
	m_pMerchant				= NULL;

	m_pNetMsgProcessor_Unit	= static_cast< CNetMsgProcessor_Unit * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_UNIT ) );
}

CUnitStorehouseBoard::~CUnitStorehouseBoard()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CUnitStorehouseBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiStorehouseWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );
}

void
CUnitStorehouseBoard::OpenWindow( void )
{
	// UI에 유닛의 가격 재설정
	CUnitItem * pUnitItem = NULL;
	for( int i = 0; i < MAX_AVATAR_UNIT; ++i )
	{
		pUnitItem = GetUnit( i );
		if( !pUnitItem )
			continue;

		// UI를 통한 출고는 무조건 Merchant를 통하므로 MerchantIndex 는 ID_INVALID가 아닌 값으로 설정한다.
		m_uiStorehouseWindow.UpdateCost( pUnitItem->GetIndex(), pUnitItem->GetDeliveryPrice( 0 ) );
	}

	// Merchat 설정
	m_pMerchant = _GetCharMgr()->GetNpc( _GetAvatar()->GetNpcTradeInfo()->m_dwTradeCharIndex );

	CObjectBoard::OpenWindow();
}

void
CUnitStorehouseBoard::CloseWindow( void )
{
	m_pMerchant = NULL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CUnitStorehouseBoard::Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	return FALSE;
}

BOOL
CUnitStorehouseBoard::Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	CUnitItem * pNewItem = static_cast< CUnitItem * >( pi_pOrgObject->Clone() );
	if( !pNewItem )
		return FALSE;

	// 출고료 세팅 ( Merchant가 설정되어 있다면 계산. default는 0 )
	DWORD dwDeliveryPrice = 0;
	if( m_pMerchant )
		dwDeliveryPrice = pNewItem->GetDeliveryPrice( m_pMerchant->GetIndex() );

	// add
	m_uiStorehouseWindow.InsertUnit( pi_byTarSocketNo, pNewItem->GetIndex(),
									 pNewItem->GetMeshID(), pNewItem->GetIconID(), pNewItem->GetName(),
									 dwDeliveryPrice );
	m_listItemPack[pi_byTarPackNo].Add( pi_byTarSocketNo, pNewItem );

	return TRUE;
}

BOOL
CUnitStorehouseBoard::Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity )
{
	return FALSE;
}

BOOL
CUnitStorehouseBoard::Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject )
{
	CUnitItem * pUnitItem = static_cast< CUnitItem * >( pi_pOrgObject );
	if( !pUnitItem )
		return FALSE;

	m_uiStorehouseWindow.RemoveUnit( pUnitItem->GetIndex() );
	m_listItemPack[pi_byPackNo].Remove( pi_bySocketNo );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CUnitStorehouseBoard::AddUnit( BYTE pi_bySocketNo, CUnitItem * pi_pUnitItem )
{
	return Add( 0, pi_bySocketNo, pi_pUnitItem );
}

CUnitItem *
CUnitStorehouseBoard::AddUnitByIndex( BYTE pi_bySocketNo, DWORD pi_dwIndex )
{
	CUnitItem clsNewUnitItem;

	clsNewUnitItem.SetIndex( pi_dwIndex );

	if( !Add( 0, pi_bySocketNo, &clsNewUnitItem ) )
		return NULL;

	return static_cast< CUnitItem * >( GetObjectByIndex( pi_dwIndex ) );
}

BOOL
CUnitStorehouseBoard::RemoveUnit( BYTE pi_bySocketNo )
{
	return Remove( 0, pi_bySocketNo );
}

BOOL
CUnitStorehouseBoard::RemoveUnitByIndex( DWORD pi_dwIndex )
{
	return RemoveByIndex( pi_dwIndex );
}

BOOL
CUnitStorehouseBoard::ReplaceUnit( BYTE pi_bySocketNo, CUnitItem * pi_pNewUnitItem )
{
	if( !Remove( 0, pi_bySocketNo ) )
		return FALSE;

	return Add( 0, pi_bySocketNo, pi_pNewUnitItem );
}

CUnitItem *
CUnitStorehouseBoard::SetCurUnit( DWORD pi_dwUnitIndex )
{
	BYTE byPackNo;

	if( !GetObjectPosition( byPackNo, m_byCurUnitSocketNo, pi_dwUnitIndex ) )
		return NULL;

	return GetUnit( m_byCurUnitSocketNo );
}

CUnitItem *
CUnitStorehouseBoard::GetUnit( BYTE pi_bySocketNo )
{
	return static_cast< CUnitItem * >( GetObject( 0, pi_bySocketNo ) );
}

CUnitItem *
CUnitStorehouseBoard::GetUnitByIndex( DWORD pi_dwUnitIndex )
{
	return static_cast< CUnitItem * >( GetObjectByIndex( pi_dwUnitIndex ) );
}

CUnitItem *
CUnitStorehouseBoard::GetCurUnit( void )
{
	return static_cast< CUnitItem * >( GetObject( 0, m_byCurUnitSocketNo ) );
}

BYTE
CUnitStorehouseBoard::GetUnitSocketNo( DWORD pi_dwUnitIndex )
{
	BYTE byPackNo, bySocketNo;

	if( !GetObjectPosition( byPackNo, bySocketNo, pi_dwUnitIndex ) )
		return 0xFF;

	return bySocketNo;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 유닛 출고 요청
BOOL
CUnitStorehouseBoard::UI_DeliveryUnitRequest( DWORD pi_dwUnitIndex )
{
	if( m_pDeliveryUnit )
	{
		_SetSystemMsg( "이미 다른 유닛을 출고 하였습니다." );
		return FALSE;
	}

	BYTE byPackNo;
	if( !GetObjectPosition( byPackNo, m_byCurUnitSocketNo, pi_dwUnitIndex ) )
		return FALSE;

	if( m_pMerchant )
		return m_pNetMsgProcessor_Unit->UnitDeliveryRequest_clzo( m_pMerchant->GetIndex() );
	else
		return m_pNetMsgProcessor_Unit->UnitDeliveryRequest_clzo( ID_INVALID );
}

// 유닛 출고 결과
void
CUnitStorehouseBoard::UI_DeliveryUnitResult( void )
{
	// 유닛 키의 Using 상태를 TRUE로 만든다.
	m_pDeliveryUnit = GetCurUnit();
	if( !m_pDeliveryUnit )
		return;

	CBoardItem * pUnitBoardItem = _GetInventoryBoard()->GetItemByIndex( m_pDeliveryUnit->GetIndex() );
	if( !pUnitBoardItem )
		return;

	pUnitBoardItem->SetUsingState( TRUE );
}

// 유닛 입고 요청
BOOL
CUnitStorehouseBoard::UI_KeepingUnitRequest( void )
{
	if( !m_pDeliveryUnit )
	{
		_SetSystemMsg( "현재 출고한 유닛이 없습니다." );
		return FALSE;
	}

	return m_pNetMsgProcessor_Unit->UnitReturnRequest_clzo();
}

// 유닛 입고 결과
void
CUnitStorehouseBoard::UI_KeepingUnitResult( void )
{
	// 유닛 키의 Using 상태를 FALSE로 만든다.
	if( !m_pDeliveryUnit )
		return;

	CBoardItem * pUnitBoardItem = _GetInventoryBoard()->GetItemByIndex( m_pDeliveryUnit->GetIndex() );
	if( !pUnitBoardItem )
		return;

	pUnitBoardItem->SetUsingState( FALSE );

	m_pDeliveryUnit = NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CUnitStorehouseBoard::FrameMove( void )
{
}

LRESULT
CUnitStorehouseBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_BOARD_TRADE	:
			UI_DeliveryUnitRequest( lParam );
			break;
	}

	return CObjectBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CUnitTuningBoard::CUnitTuningBoard()
{
	m_dwPrevUnitFrame		= ID_INVALID;
	m_byPrevLevel			= 0;
	m_byPrevExpertness		= 0;

	m_byTuningStage			= UTS_NONE;
	m_byTuningBulletSocket	= 0;

	m_pNetMsgProcessor_Unit	= static_cast< CNetMsgProcessor_Unit * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_UNIT ) );
}

CUnitTuningBoard::~CUnitTuningBoard()
{
}

void
CUnitTuningBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiTuningWindow;
	m_clsTuningUnit.m_uiTuningWindow = &m_uiTuningWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CUnitTuningBoard::Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	return FALSE;
}

BOOL
CUnitTuningBoard::Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	CBoardItem * pNewItem = static_cast< CBoardItem * >( pi_pOrgObject->Clone() );
	if( !pNewItem )
		return FALSE;

	m_uiTuningWindow.InsertBuyTuningParts( pi_byTarPackNo, pNewItem->GetDTIndex(), pNewItem->GetIconID() );
	pNewItem->SetIcon( m_uiTuningWindow.GetBuyParts( pi_byTarPackNo, pNewItem->GetDTIndex() ) );

	m_listItemPack[pi_byTarPackNo].Add( pi_byTarSocketNo, pNewItem );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CUnitTuningBoard::Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity )
{
	return FALSE;
}

BOOL
CUnitTuningBoard::Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject )
{
	CBoardItem * pBoardItem = static_cast< CBoardItem * >( pi_pOrgObject );
	if( !pBoardItem )
		return FALSE;

	m_uiTuningWindow.RemoveBuyTuningParts( pi_byPackNo, pBoardItem->GetDTIndex() );
	m_listItemPack[pi_byPackNo].Remove( pi_bySocketNo );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CUnitTuningBoard::ReturnItemToItemBoard( void )
{
	UI_RemoveMainFrame();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CUnitTuningBoard::UI_AddMainFrame( void )
{
	// check
	if( _GetGBM()->GetOrgBoardType() != IBT_INVENTORY )
		return FALSE;

	CBoardItem * pBoardItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pBoardItem )
		return FALSE;
	if( !pBoardItem->IsEqualItemKind( IEPT_UNIT ) )
		return FALSE;
	if( pBoardItem->IsUsing() )
		return FALSE;

	if( !m_clsTuningUnit.IsEmpty() )
	{
		_SetSystemMsg( "이미 다른 기갑 장비를 튜닝하고 있습니다." );
		return FALSE;
	}

	// set item list
	Clear();
	SetBuyablePartList();

	// set unit
	CUnitItem * pUnitItem = _GetUnitStorehouseBoard()->SetCurUnit( pBoardItem->GetIndex() );
	if( !pUnitItem )
		return FALSE;

	// copy currnet unit
	m_clsTuningUnit.Clear();
	if( !m_clsTuningUnit.CreateByUnitItem( pUnitItem ) )
	{
		m_clsTuningUnit.Clear();
		return FALSE;
	}

	// enable button
	if( pUnitItem->GetDurabilityRate() < 1 )
		m_uiTuningWindow.SetRepairButtonState( TRUE );
	else
		m_uiTuningWindow.SetRepairButtonState( FALSE );
	m_uiTuningWindow.SetTuningButtonState( FALSE );
	m_uiTuningWindow.SetCancelButtonState( TRUE );

	// set shoulder & backpack
	m_uiTuningWindow.Enable_Shouler_Bullet( m_clsTuningUnit.GetBulletSocketNum( IEPT_UNIT_SHOULDER ) );
	m_uiTuningWindow.Enable_BackPack_Bullet( m_clsTuningUnit.GetBulletSocketNum( IEPT_UNIT_BACKPACK ) );

	// set cost
	m_dwTuningCost[0] = m_dwTuningCost[1] = 0;
	m_uiTuningWindow.SetTuningCost( m_dwTuningCost[IMT_DALANT] );

	// save position
	m_byPackNo		= _GetGBM()->GetOrgPackNo();
	m_bySocketNo	= _GetGBM()->GetOrgSocketNo();
	_GetGBM()->ClearOperation();

	return TRUE;
}

BOOL
CUnitTuningBoard::UI_RemoveMainFrame( void )
{
	// check
	if( _GetGBM()->GetHoverObject() )
		return FALSE;

	// set inventory
	CBoardItem	clBoardItem;
	_GetGBM()->ConvertBoardObject( &clBoardItem, &m_clsTuningUnit );
	_GetInventoryBoard()->Add( m_byPackNo, m_bySocketNo, &clBoardItem );

	// Initialize
	Clear();
	m_clsTuningUnit.Clear();
	m_dwTuningCost[0] = m_dwTuningCost[1] = 0;
	m_uiTuningWindow.SetTuningCost( m_dwTuningCost[IMT_DALANT] );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CUnitTuningBoard::UI_SelectPart( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	// check item
	CBoardItem * pSelectParts = static_cast< CBoardItem * >( GetObject( pi_byPackNo, pi_bySocketNo ) );
	if( !pSelectParts )
		return FALSE;
	
	// 현재 장착한 파츠와 동일한 파츠를 선택했다면 return
	// 탄환은 pi_byPackNo 에 튜닝을 하려 선택한 탄환의 소켓번호를 더해서 보낸다.
	CUnitPartsItem * pCurEquipParts = NULL;
	if( pi_byPackNo < MAX_UNIT_PART_TYPE )
		pCurEquipParts = m_clsTuningUnit.GetPartsByTuningBoard( pi_byPackNo, pSelectParts );
	else
		pCurEquipParts = m_clsTuningUnit.GetPartsByTuningBoard( pi_byPackNo + m_byTuningBulletSocket, pSelectParts );
	if( pCurEquipParts )
	{
		if( pCurEquipParts->IsSameKind( pSelectParts ) &&
			pCurEquipParts->GetDurability() == pSelectParts->GetDurability() )
			return FALSE;
	}

	if( !IsEquableUnitPart( static_cast< UNIT_PART_DATA * >( pSelectParts->GetData() ) ) )
		return FALSE;

	// calculate cost
	DWORD dwTuningCost = m_dwTuningCost[IMT_DALANT];
	dwTuningCost += (DWORD)pSelectParts->GetBuyPrice();

	// 원래 장착한 파츠와 동일한 파츠라면 pSelectParts->GetBuyPrice()를 도로 뺀다.
	BOOL			bSelectPartsEqualOrgEquipParts = FALSE;
	CUnitItem *		pCurUnit = _GetUnitStorehouseBoard()->GetCurUnit();
	CUnitPartsItem* pOrgEquipParts = pCurUnit->GetPartsByTuningBoard( pi_byPackNo, pSelectParts );
	if( pOrgEquipParts && pOrgEquipParts->IsSameKind( pSelectParts ) &&
		pCurEquipParts->GetDurability() == pSelectParts->GetDurability() )
	{
		//dwTuningCost += (DWORD)pSelectParts->GetSellPrice(); // 장착한 파츠의 가격을 받으려면 여길 푼다.
		dwTuningCost -= (DWORD)pSelectParts->GetBuyPrice();
		bSelectPartsEqualOrgEquipParts = TRUE;
	}

	if( pCurEquipParts )
	{
		if( pCurEquipParts->IsHave() )
			;//dwTuningCost -= (DWORD)pCurEquipParts->GetSellPrice(); // 장착한 파츠의 가격을 받으려면 여길 푼다.
		else
			dwTuningCost -= (DWORD)pCurEquipParts->GetBuyPrice();
	}

	// add parts
	// 탄환은 pi_byPackNo 에 튜닝을 하려 선택한 탄환의 소켓번호를 더해서 보낸다.
	if( pi_byPackNo < MAX_UNIT_PART_TYPE )
	{
		if( !m_clsTuningUnit.AddByTuningBoard( pi_byPackNo, pSelectParts, bSelectPartsEqualOrgEquipParts ) )
			return FALSE;
	}
	else
	{
		if( !m_clsTuningUnit.AddByTuningBoard( pi_byPackNo + m_byTuningBulletSocket, pSelectParts, bSelectPartsEqualOrgEquipParts ) )
			return FALSE;
	}

	// set cost
	m_dwTuningCost[IMT_DALANT] = dwTuningCost;
	m_uiTuningWindow.SetTuningCost( m_dwTuningCost[IMT_DALANT] );

	// turn on button
	if( IsHaveTuningParts() )
		m_uiTuningWindow.SetTuningButtonState( TRUE );

	// set shoulder & backpack
	m_uiTuningWindow.Enable_Shouler_Bullet( m_clsTuningUnit.GetBulletSocketNum( IEPT_UNIT_SHOULDER ) );
	m_uiTuningWindow.Enable_BackPack_Bullet( m_clsTuningUnit.GetBulletSocketNum( IEPT_UNIT_BACKPACK ) );

	return TRUE;
}

// 튜닝할 탄환의 소켓을 설정한다.
void
CUnitTuningBoard::UI_SelectTuningBulletSocket( BYTE pi_bySocketNo )
{
	if( pi_bySocketNo >= ( MAX_UNIT_BULLET_TYPE + MAX_BACKPACK_BULLET_NUM ) )
		return;

	m_byTuningBulletSocket = pi_bySocketNo;
	m_uiTuningWindow.Select_Bullet( pi_bySocketNo );
}

//==============================================================================

BOOL
CUnitTuningBoard::UI_DeselectPart( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	// calculate cost
	CUnitItem *			pCurUnit	= _GetUnitStorehouseBoard()->GetCurUnit();
	if( !pCurUnit )
		return FALSE;

	CUnitPartsItem *	pOrgParts	= NULL;
	CUnitPartsItem *	pEquipParts	= m_clsTuningUnit.GetPartsByTuningBoard( pi_byPackNo, pi_bySocketNo );
	if( pEquipParts )
	{
		if( pEquipParts->IsHave() )
			return FALSE;

		pOrgParts = pCurUnit->GetPartsByTuningBoard( pi_byPackNo, pi_bySocketNo );
		if( pOrgParts )
			;//m_dwTuningCost[IMT_DALANT] += (DWORD)pOrgParts->GetSellPrice(); // 장착한 파츠의 가격을 받으려면 여길 푼다.

		m_dwTuningCost[IMT_DALANT] -= (DWORD)pEquipParts->GetBuyPrice();
	}

	m_uiTuningWindow.SetTuningCost( m_dwTuningCost[IMT_DALANT] );

	// remove parts
	m_clsTuningUnit.RemoveByTuningBoard( pi_byPackNo, pi_bySocketNo );

	// add original parts
	m_clsTuningUnit.AddByTuningBoard( pi_byPackNo, pi_bySocketNo, pOrgParts );

	// turn off button
	if( !IsHaveTuningParts() )
		m_uiTuningWindow.SetTuningButtonState( FALSE );

	// set shoulder & backpack
	m_uiTuningWindow.Enable_Shouler_Bullet( m_clsTuningUnit.GetBulletSocketNum( IEPT_UNIT_SHOULDER ) );
	m_uiTuningWindow.Enable_BackPack_Bullet( m_clsTuningUnit.GetBulletSocketNum( IEPT_UNIT_BACKPACK ) );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CUnitTuningBoard::UI_RepairRequest( void )
{
	return m_pNetMsgProcessor_Unit->UnitFrameRepairRequest_clzo();
}

void
CUnitTuningBoard::UI_RepairResult( DWORD pi_dwDurability )
{
	CUnitItem *	pCurUnit = _GetUnitStorehouseBoard()->GetCurUnit();
	if( !pCurUnit )
		return;

	// set durability
	pCurUnit->SetDurability( pi_dwDurability );
	m_clsTuningUnit.SetDurability( pi_dwDurability );

	// disable button
	m_uiTuningWindow.SetRepairButtonState( FALSE );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CUnitTuningBoard::UI_TuningRequest( void )
{
	if( _GetAvatar()->GetDalant() < m_dwTuningCost[IMT_DALANT] )
	{
		_SetSystemMsg( "기갑장비를 튜닝하기 위한 %s가 부족합니다.", _GetAvatarMoneyName() );
		return FALSE;
	}

	m_byTuningStage = UTS_BEGIN;

	return TRUE;
}

void
CUnitTuningBoard::UI_TuningResult( BOOL pi_bIsSuccessed )
{
	CUnitItem * pCurUnit = _GetUnitStorehouseBoard()->GetCurUnit();
	if( !pCurUnit )
		return;

	m_clsTuningUnit.SetHaveAllPartAndBullets();

	switch( m_byTuningStage )
	{
		case UTS_PARTS	:
			if( pi_bIsSuccessed )
				pCurUnit->CreateAllPartsByUnitItem( &m_clsTuningUnit );
			break;
		case UTS_BULLET	: 
			if( pi_bIsSuccessed )
				pCurUnit->CreateAllBulletsByUnitItem( &m_clsTuningUnit );
			break;
		case UTS_BACKPACK_BULLET	:
			if( pi_bIsSuccessed )
				pCurUnit->CreateAllBackpackBulletsByUnitItem( &m_clsTuningUnit );

			m_clsTuningUnit.Clear();
			m_dwTuningCost[0] = m_dwTuningCost[1] = 0;
			m_uiTuningWindow.SetTuningCost( 0 );
			m_uiTuningWindow.SetTuningButtonState( FALSE );

			m_clsTuningUnit.CreateByUnitItem( pCurUnit );

			_SetSystemMsg( "기갑 장비 튜닝을 마쳤습니다.");
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CUnitTuningBoard::FrameMove( void )
{
	CObjectBoard::FrameMove();

	if( !_GetGBM()->IsRequestProcess() && ( m_byTuningStage != UTS_NONE ) )
	{
		++m_byTuningStage;
		if( m_byTuningStage == UTS_END )
			m_byTuningStage = UTS_NONE;

		BOOL bResult = TRUE;

		switch( m_byTuningStage )
		{
			case UTS_PARTS	:
				if( !m_pNetMsgProcessor_Unit->UnitPartTuningRequest_clzo() )
					UI_TuningResult( FALSE );
				break;

			case UTS_BULLET	:
				if( !m_pNetMsgProcessor_Unit->UnitBulletFillRequest_clzo() )
					UI_TuningResult( FALSE );
				break;

			case UTS_BACKPACK_BULLET	:
				if( !m_pNetMsgProcessor_Unit->UnitPackFillRequest_clzo() )
					UI_TuningResult( FALSE );
				break;
		}

		/*
		if( !bResult )
		{
			_SetSystemMsg( "기갑 장비 튜닝에 실패하였습니다." );
			m_byTuningStage = UTS_NONE;
		}*/
	}
}

LRESULT
CUnitTuningBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UI_MOUSE_MSG	stRecvMsg( lParam );

	switch( uMsg )
	{
		case WM_TUNING_BOARD_MAINFRAME	:
			if( m_clsTuningUnit.IsEmpty() )
				UI_AddMainFrame();
			else
				UI_RemoveMainFrame();
			break;

		case WM_BOARD_ADD_OBJECT		:
			UI_SelectPart( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
			break;

		case WM_BOARD_REMOVE_OBJECT		:
			if( stRecvMsg.m_byPackNo == 0 )
			{
				UI_DeselectPart( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
			}
			else
			{
				if( m_byTuningBulletSocket == stRecvMsg.m_bySocketNo )
					UI_DeselectPart( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
				else
					UI_SelectTuningBulletSocket( stRecvMsg.m_bySocketNo );
			}
			break;

		case WM_TUNING_BOARD_REPAIR		:
			UI_RepairRequest();
			break;

		case WM_TUNING_BOARD_TUNING		:
			UI_TuningRequest();
			break;

		case WM_TUNING_BOARD_CANCEL		:
			CloseWindow();
			break;

		case WM_BOARD_HOVER_OBJECT		:
			{
				// buyable parts
				if( stRecvMsg.m_byExtra == 0 )
				{
					CBoardItem * pOrgItem;
					pOrgItem = static_cast< CBoardItem * >( GetObject( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo ) );
					if( !pOrgItem )
						break;

					static CUnitPartsItem s_clsUnitPart;
					s_clsUnitPart.Create( pOrgItem->GetItemKind(), pOrgItem->GetDTIndex() );
					s_clsUnitPart.SetIcon( pOrgItem->GetIcon() );

					DisplayUnitParts( &s_clsUnitPart, 0xFF );
				}
				// tuning parts
				else if( stRecvMsg.m_byExtra == 1 )
				{
					CUnitPartsItem * pUnitParts = m_clsTuningUnit.GetPartsByTuningBoard( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );

					DisplayUnitParts( pUnitParts, 0xFF );
				}
			}
			break;

		case WM_CLOSE_WINDOW			:
			ReturnItemToItemBoard();
			break;
	}

	return CObjectBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CUnitTuningBoard::OpenWindow( void )
{
	// initialize
	m_clsTuningUnit.Clear();
	m_dwTuningCost[0] = m_dwTuningCost[1] = 0;
	m_uiTuningWindow.SetTuningCost( m_dwTuningCost[IMT_DALANT] );

	// open
	CObjectBoard::OpenWindow();
}

BOOL
CUnitTuningBoard::SetBuyablePartList( void )
{
	//UNIT_PART_DATA *	pUnitPartData;
	int					nUnitPartNum;
	CBoardItem			clsNewPartItem;

	BYTE				byPartType = IEPT_UNIT_HELMET;

	for( int i = 0; i < MAX_UNIT_PART_TYPE + MAX_UNIT_BULLET_TYPE; ++i, ++byPartType )
	{
		if( byPartType >= IEPT_UNIT_BULLET )
			byPartType = IEPT_UNIT_BULLET;

		nUnitPartNum = _GetItemDataMgr()->GetTotalDataNum( byPartType );
		for( int j = 0; j < nUnitPartNum; ++j )
		{
			if( !clsNewPartItem.Create( byPartType, j ) )
				continue;

			if( !IsEquableUnitPart( static_cast< UNIT_PART_DATA * >( clsNewPartItem.GetData() ) ) )
				continue;

			Add( i, j, &clsNewPartItem );
		}
	}

	return TRUE;
}

BOOL
CUnitTuningBoard::IsEquableUnitPart( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	CBoardItem * pUnitPart = static_cast< CBoardItem * >( GetObject( pi_byPackNo, pi_bySocketNo ) );

	return IsEquableUnitPart( static_cast< UNIT_PART_DATA * >( pUnitPart->GetData() ) );
}

BOOL
CUnitTuningBoard::IsEquableUnitPart( UNIT_PART_DATA * pi_pUnitPartData )
{
	if( !pi_pUnitPartData )
		return FALSE;

	CUnitItem * pCurUnit = _GetUnitStorehouseBoard()->GetCurUnit();
	if( !pCurUnit )
		return FALSE;

	// check unit frame
	DWORD byCurUnitFrame = pCurUnit->GetDTIndex();
	if( !pi_pUnitPartData->byEquipableFrame[byCurUnitFrame] )
		return FALSE;

	// check level
	if( _GetAvatar()->GetLevel() < pi_pUnitPartData->byEquableLevel )
		return FALSE;

	// check expertness
	BYTE byExpertness;
	for( int i = 0; i < UNIT_PART_DATA::MAX_UNIT_EQUABLE_EXPERTNESS; ++i )
	{
		if( pi_pUnitPartData->byEquableExpertnessType[i] == 0xFF )
			continue;

		byExpertness = _GetAvatar()->GetExpertness( pi_pUnitPartData->byEquableExpertnessType[i] );
		if( byExpertness < pi_pUnitPartData->byEquableExpertnessValue[i] )
			return FALSE;
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 튜닝할 파츠가 있는지를 검사한다.
BOOL
CUnitTuningBoard::IsHaveTuningParts( void )
{
	CUnitItem * pCurUnit = _GetUnitStorehouseBoard()->GetCurUnit();
	if( !pCurUnit )
		return FALSE;

	CUnitPartsItem * pCurUnitParts	= NULL;
	CUnitPartsItem * pTuningParts	= NULL;

	for( int i = IEPT_UNIT_HELMET; i <= IEPT_UNIT_BACKPACK; ++i )
	{
		if( !IsHaveTuningParts_CheckSameItem( pCurUnit->GetParts( i ), m_clsTuningUnit.GetParts( i ) ) )
			return TRUE;
	}

	for( i = IEPT_UNIT_ARMS; i <= IEPT_UNIT_SHOULDER; ++i )
	{
		if( !IsHaveTuningParts_CheckSameItem( pCurUnit->GetBullet( i ), m_clsTuningUnit.GetBullet( i ) ) )
			return TRUE;
	}

	for( i = 0; i < MAX_BACKPACK_BULLET_NUM; ++i )
	{
		if( !IsHaveTuningParts_CheckSameItem( pCurUnit->GetBackpackBullet( i ), m_clsTuningUnit.GetBackpackBullet( i ) ) )
			return TRUE;
	}

	return FALSE;
}

// 두 파츠가 동일한지를 체크한다.
BOOL
CUnitTuningBoard::IsHaveTuningParts_CheckSameItem( CUnitPartsItem * pi_pCurUnitParts, CUnitPartsItem * pi_pTuningParts )
{
	// 양 쪽 파츠가 모두 있는 경우
	if( pi_pCurUnitParts && pi_pTuningParts )
	{
		if( pi_pCurUnitParts->IsSameKind( pi_pTuningParts ) )
		{
			if( pi_pCurUnitParts->GetDurability() == pi_pTuningParts->GetDurability() )
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}
	// 양 쪽 파츠가 모두 없는 경우
	else if( !pi_pCurUnitParts && !pi_pTuningParts )
	{
		return TRUE;
	}
	// 한 쪽 파츠만 있는 경우
	else
	{
		return FALSE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////