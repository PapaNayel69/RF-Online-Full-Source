////////////////////////////////////////////////////////////////////////////////
//
// CInvenBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../Manager/CCharacterMgr.h"
#include "../Common/CBoardManager.h"

#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_Item.h"

#include "CItemUpgradeBoard.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CItemUpgradeBoard::CItemUpgradeBoard()
{
	m_dwUpgradeItemIndex = ID_INVALID;

	for( int i = 0; i < MAX_UIAT_TYPE; ++i )
		m_byItemPackNo[i] = m_byItemSocketNo[i] = ID_INVALID;
}

CItemUpgradeBoard::~CItemUpgradeBoard()
{

}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CItemUpgradeBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiItemUpgradeWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );

	m_uiWindow->SetPos( _GetUIScreen()->GetSizeX() - m_uiWindow->GetSizeX() * 2 - 15, _GetUIScreen()->GetPosY() );
}

void
CItemUpgradeBoard::OpenUpgradeWindow( DWORD pi_dwItemIndex )
{
	m_dwUpgradeItemIndex = pi_dwItemIndex;

	OpenWindow();
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CItemUpgradeBoard::UI_AddRequest_ProcessObject( void )
{
	return SelectUpgradeItem( _GetGBM()->GetOrgBoardType(), _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), FALSE );
}

BOOL	
CItemUpgradeBoard::UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity )
{
	return DeselectUpgradeItem( _GetGBM()->GetOrgSocketNo(), FALSE );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL	
CItemUpgradeBoard::UI_Trade_By_HotKey( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	DeselectUpgradeItem( pi_bySocketNo, TRUE );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 업그레이드에 필요한 아이템 선택
BOOL		
CItemUpgradeBoard::SelectUpgradeItem( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, BOOL pi_bTradeByHotKey )
{
	CBoardItem * pItem = NULL;
	if( pi_bTradeByHotKey )
		pItem = _GetInventoryBoard()->GetItem( pi_byPackNo, pi_bySocketNo );
	else
		pItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pItem )
		return FALSE;

	//--------------------------------------------------------------------------

	// 업그레이드 시킬 아이템
	BYTE byItemKind = pItem->GetItemKind();
	if( ( byItemKind == IEPT_WEAPON ) || ( byItemKind == IEPT_UPPER_PART ) ||
		( byItemKind == IEPT_LOWER_PART ) || ( byItemKind == IEPT_GLOVES ) ||
		( byItemKind == IEPT_SHOES ) || ( byItemKind == IEPT_HELMET ) ||
		( byItemKind == IEPT_SHIELD ) )
	{
		if( pi_byBoardType == IBT_EQUIP )
		{
			_SetSystemMsg( "장착중인 아이템은 업그레이드가 불가능합니다." );
			return FALSE;
		}

		if( !pItem->IsUpgradable() )
		{
			_SetSystemMsg( "업그레이드가 불가능한 아이템 입니다." );
			return FALSE;
		}

		// 이미 아이템이 있다면 먼저 인벤토리로 돌려놓는다.
		ReturnBoardItemToInventory( UIAT_ITEM );

		// 아이템 세팅
		AddUpgradeItem( UIAT_ITEM, pi_byPackNo, pi_bySocketNo, pItem );


		// 기존 아이템과 swap할 경우 기존에 있던 탈릭/보석이 현재의 아이템과 맞지 않을 경우 제외시킨다.
		CBoardItem * pOtherItem;
		for( int i = UIAT_TALIK; i < MAX_UIAT_TYPE; ++i )
		{
			pOtherItem = GetItem( i );
			if( !pOtherItem )
				continue;

			if( !pOtherItem->IsEnableInsertJewel( pOtherItem->GetDTIndex() ) )
			{				
				ReturnBoardItemToInventory( i );			
			}
		}
	}
	// 탈릭 or 보석
	else if( byItemKind == IEPT_INGOT )
	{
		if( GetItem( UIAT_ITEM ) )
		{
			INGOT_DATA *	pTalikData = ( INGOT_DATA * )pItem->GetData();
			DWORD			dwUpgradeAttributeType = pTalikData->GetUpgradeAttributeType();
			if( pTalikData->IsUpgradeItem() )			
			{
				// 동일한 아이템인지 체크
				if( GetItem( dwUpgradeAttributeType ) &&
					GetItem( dwUpgradeAttributeType )->IsEqual( pItem ) )
				{
					_SetSystemMsg( "이미 동일한 아이템이 있습니다." );
					return FALSE;
				}

				// 업그레이드할 아이템에 해당 탈릭 or 보석을 사용할 수 있는지 체크한다.
				switch( GetItem( UIAT_ITEM )->IsEnableInsertJewel( pItem->GetDTIndex() ) )
				{
					case 0 : return FALSE;
					case 1 : _SetSystemMsg( "더 이상 같은 종류의 탈릭을 사용할 수 없습니다."); return FALSE;
					case 2 : _SetSystemMsg( "무기에는 광휘, 정화, 수호, 신념의 탈릭 중 오직 하나만을 사용할 수 있습니다."); return FALSE;
				}

				// 이미 아이템이 있다면 먼저 인벤토리로 돌려놓는다.
				ReturnBoardItemToInventory( dwUpgradeAttributeType );

				// 아이템 세팅
				BYTE byRestQuantity = pItem->GetQuantity() - 1;
				pItem->SetQuantity( 1 );

				AddUpgradeItem( dwUpgradeAttributeType, pi_byPackNo, pi_bySocketNo, pItem );

				pItem->SetQuantity( byRestQuantity );
		
	
				// 만약 부활의 탈릭이라면 모든 보석은 빠져야한다.
				if( pTalikData->GetDTIndex() == 33 )
				{
					for( int i = UIAT_FIRE; i < MAX_UIAT_TYPE; ++i )
						ReturnBoardItemToInventory( i );
				}
			}
			else
			{
				_SetSystemMsg( "업그레이드에는 탈릭 혹은 보석만이 사용됩니다." );
				return FALSE;
			}
		}
		else
		{
			_SetSystemMsg( "먼저 업그레이드할 아이템을 선택해 주십시오." );
			return FALSE;
		}
	}
	// 다른 아이템
	else
	{
		return FALSE;
	}

	m_uiItemUpgradeWindow.SetSuccessProb( CalculateSuccessProbabilityOfItemUpgrade() );

	//--------------------------------------------------------------------------
	
	if( pi_bTradeByHotKey )
	{
		if( pItem->GetQuantity() == 0 )
			_GetInventoryBoard()->Remove( pi_byPackNo, pi_bySocketNo );
	}
	else
	{
		ReturnHoverItemToInventory( pi_byPackNo, pi_bySocketNo, pItem );

		_GetGBM()->SetHoverObject( NULL );
	}

	if( GetItem( UIAT_ITEM ) && GetItem( UIAT_TALIK ) )
		m_uiItemUpgradeWindow.EnableUpgrade( TRUE );


	return TRUE;
}

// 업그레이드에 필요한 아이템 해제
BOOL
CItemUpgradeBoard::DeselectUpgradeItem( BYTE pi_bySocketNo, BOOL pi_bTradeByHotKey )
{
	// 아이템이 해제되면 나머지 탈릭 & 보석들도  해제 되어야 한다.
	if( pi_bySocketNo == UIAT_ITEM )
	{
		for( int i = 0; i < MAX_UIAT_TYPE; ++i )
		{
			if( !GetItem( i ) )			
				continue;
			
			ReturnBoardItemToInventory( i );
		}
	}
	// 탈릭 or 보석 해제
	else
	{
		ReturnBoardItemToInventory( pi_bySocketNo );
	}

	m_uiItemUpgradeWindow.SetSuccessProb( CalculateSuccessProbabilityOfItemUpgrade() );

	if( !( GetItem( UIAT_ITEM ) && GetItem( UIAT_TALIK ) ) )
		m_uiItemUpgradeWindow.EnableUpgrade( FALSE );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아이템의 업그레이드 요청
BOOL	
CItemUpgradeBoard::ItemUpgrade_Request( void )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;

	if( !GetItem( UIAT_ITEM ) )
	{
		_SetSystemMsg( "업그레이드할 아이템이 없습니다." );
		return FALSE;
	}

	if( !GetItem( UIAT_TALIK ) )
	{
		_SetSystemMsg( "업그레이드시에는 반드시 탈릭이 있어야 합니다." );
		return FALSE;
	}

	if( GetItem( UIAT_TALIK )->GetDTIndex() != 33 )
	{
		if( !GetItem( UIAT_ITEM )->IsUpgradable() )
		{
			_SetSystemMsg( "업그레이드가 불가능한 아이템 입니다." );
			return FALSE;
		}
		if( GetItem( UIAT_ITEM )->GetLevel() >= GetItem( UIAT_ITEM )->GetMaxSocketNum() )
		{
			_SetSystemMsg( "더 이상 업그레이드가 불가능한 아이템 입니다." );
			return FALSE;
		}

		m_pNetMsgProcessor_Item->UpgradeItemRequest_clzo();		
	}
	else
	{
		if( GetItem( UIAT_ITEM )->GetLevel() <= 0 )
		{
			_SetSystemMsg( "더 이상 다운그레이드 할 수 없는 아이템 입니다." );
			return FALSE;
		}
		m_pNetMsgProcessor_Item->DowngradeItemRequest_clzo();
	}

	return TRUE;	
}

// 아이템의 업그레이드/다운그레이드 요청에 대한 결과
BOOL	
CItemUpgradeBoard::ItemUpgrade_Result( BOOL pi_bIsSuccess )
{
	for( int i = 0; i < MAX_UIAT_TYPE; ++i )
	{
		// 데이터 설정
		if( GetItem( i ) )			
		{
			// 업그레이드된 아이템
			if( i == UIAT_ITEM )
			{
				if( pi_bIsSuccess )
				{
					if( GetItem( UIAT_TALIK )->GetDTIndex() != 33 )
						GetItem( i )->InsertTalik( ( INGOT_DATA * )GetItem( UIAT_TALIK )->GetData() );
					else
						GetItem( i )->ExtractTalik();
				}
			}
			// 탈릭 or 보석
			else
			{
				RemoveUpgradeItem( i );
			}
		}
	}

	m_uiItemUpgradeWindow.SetSuccessProb( 0.0f );
	m_uiItemUpgradeWindow.EnableUpgrade( FALSE );

	if( pi_bIsSuccess )
		PlayWave( SND_ID_SUCC_UPGRADE );
	else
		PlayWave( SND_ID_FAIL_UPGRADE );	

	return TRUE;
}

// 아이템 업그레이드 실패시 아이템이 파괴되는 것을 처리
BOOL
CItemUpgradeBoard::ItemUpgrade_Result_DestroyItem( BYTE pi_byErrorCode )
{
	if( !GetItem( UIAT_ITEM ) )
		return FALSE;

	// 탈릭 파괴
	if( pi_byErrorCode == 101 )
	{
		GetItem( UIAT_ITEM )->ExtractAllTalik();
	}
	// 아이템 파괴
	else if( pi_byErrorCode == 102 )
	{
		RemoveUpgradeItem( UIAT_ITEM );
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CItemUpgradeBoard::AddUpgradeItem( BYTE pi_byUpgradeSocketNo, BYTE pi_byOrgPackNo, BYTE pi_byOrgSocketNo, CBoardItem * pi_pOrgItem )
{
	if( pi_byUpgradeSocketNo >= MAX_UIAT_TYPE )
		return;

	Add( 0, pi_byUpgradeSocketNo, pi_pOrgItem );

	m_byItemPackNo[pi_byUpgradeSocketNo]	= pi_byOrgPackNo;
	m_byItemSocketNo[pi_byUpgradeSocketNo]	= pi_byOrgSocketNo;

	// set ui
	_GetInventoryBoard()->UI_SetUselessSocket( pi_byOrgPackNo, pi_byOrgSocketNo, TRUE );
}

void
CItemUpgradeBoard::RemoveUpgradeItem( BYTE pi_byUpgradeSocketNo )
{
	if( pi_byUpgradeSocketNo >= MAX_UIAT_TYPE )
		return;

	Remove( 0, pi_byUpgradeSocketNo );

	// set ui
	_GetInventoryBoard()->UI_SetUselessSocket( m_byItemPackNo[pi_byUpgradeSocketNo], m_byItemSocketNo[pi_byUpgradeSocketNo], FALSE );

	m_byItemPackNo[pi_byUpgradeSocketNo]	= ID_INVALID;
	m_byItemSocketNo[pi_byUpgradeSocketNo]	= ID_INVALID;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CItemUpgradeBoard::ReturnItemToItemBoard( void )
{
	for( int i = 0; i < MAX_UIAT_TYPE; ++i )
		ReturnBoardItemToInventory( i );
}

BOOL	
CItemUpgradeBoard::ReturnHoverItemToInventory( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardItem * pi_pItemToReturn )
{
	if( !pi_pItemToReturn )
		return FALSE;
	if( pi_pItemToReturn->IsEmpty() )
		return FALSE;
	if( pi_pItemToReturn->GetQuantity() <= 0 )
		return FALSE;

	CBoardItem * l_pInvenItem = _GetInventoryBoard()->GetItem( pi_byPackNo, pi_bySocketNo );
	
	if( l_pInvenItem )
	{
		l_pInvenItem->SetQuantity( l_pInvenItem->GetQuantity() + pi_pItemToReturn->GetQuantity() );		
	}
	else
	{		
		_GetInventoryBoard()->Add( pi_byPackNo, pi_bySocketNo, pi_pItemToReturn );		
	}

	return TRUE;
}

BOOL
CItemUpgradeBoard::ReturnBoardItemToInventory( BYTE pi_byBoardSocketNo )
{
	if( pi_byBoardSocketNo >= MAX_UIAT_TYPE )
		return FALSE;
	
	CBoardItem * pItemToReturn = GetItem( pi_byBoardSocketNo );
	if( !pItemToReturn )
		return FALSE;

	// return item
	if( !_GetGBM()->ReturnItemToItemBoard( IBT_INVENTORY, m_byItemPackNo[pi_byBoardSocketNo], m_byItemSocketNo[pi_byBoardSocketNo], pItemToReturn ) )
		return FALSE;

	// remove
	RemoveUpgradeItem( pi_byBoardSocketNo );

	return TRUE;
}

// ======================================================================================

CBoardItem	*
CItemUpgradeBoard::GetTradeItem( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	CBoardItem * pItem;

	for( int i = 0; i < MAX_UIAT_TYPE; ++i )
	{
		pItem = GetItem( i );
		if( !pItem )
			continue;

		if( m_byItemPackNo[i] == pi_byPackNo &&
			m_byItemSocketNo[i] == pi_bySocketNo )
			return pItem;
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아이템 업그레이드의 성공 확률 계산
float
CItemUpgradeBoard::CalculateSuccessProbabilityOfItemUpgrade( void )
{
	// 확률 계산
	static const float fDefaultJewelConstant = 0.125f;
	static const float fSuccessProbabilityByUpgradeLevel[8] =
	{ 1.0f, 1.0f, 0.75f, 0.5f, 0.25f, 0.1f, 0.05f, 0.01f };

	float fSuccessProbability	= 0.0f;
	float fJewelConstant		= 0.0f;

	if( !GetItem( UIAT_ITEM ) )
		return fSuccessProbability;

	fSuccessProbability = fSuccessProbabilityByUpgradeLevel[GetItem( UIAT_ITEM )->GetLevel()];

	CBoardItem * pItem;
	for( int i = UIAT_FIRE; i < UIAT_FIRE + 4; ++i )
	{
		pItem = GetItem( i );
		if( pItem )		
			fJewelConstant += ( ( INGOT_DATA * )pItem->GetData() )->GetUpgradeJewelConstant();
		else
			fJewelConstant += fDefaultJewelConstant;
	}

	return ( fSuccessProbability * fJewelConstant / 4.0f * 100.0f );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CBoardItem	*
CItemUpgradeBoard::GetItem( BYTE pi_bySocketNo )
{
	return CItemBoard::GetItem( 0, pi_bySocketNo );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

LRESULT
CItemUpgradeBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UI_MOUSE_MSG	stRecvMsg( lParam );

	switch( uMsg )
	{
	case WM_ITEM_UPGRADE_BOARD_PROCESS	:
		ItemUpgrade_Request();
		break;

	case WM_CLOSE_WINDOW				:
		ReturnItemToItemBoard();
		break;
	}

	return CItemBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}