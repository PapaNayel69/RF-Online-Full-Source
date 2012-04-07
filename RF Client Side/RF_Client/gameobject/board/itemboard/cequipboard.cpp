////////////////////////////////////////////////////////////////////////////////
//
// CEquipBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../../Manager/CCharacterMgr.h"
#include "../Common/CBoardManager.h"

#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_ItemArrangement.h"
#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_Item.h"

#include "../../../GameProgress\CGP_MainGame.h"


////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CEquipBoard::CEquipBoard()
{
	m_byReservedItemPackNo		= ID_INVALID;
	m_byReservedItemSocketNo	= ID_INVALID; 

	m_byUsingBulletSocketNo		= ID_INVALID;
}

CEquipBoard::~CEquipBoard()
{
}

void
CEquipBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiEquipWindow;

	m_uiWindow->Init();
	m_uiWindow->SetID( pi_dwID );
	m_uiWindow->SetPos( pi_nPosX, pi_nPosY );
	
	//CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CEquipBoard::Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	if( !CItemBoard::Add_ProcessNewObject( pi_byTarPackNo, pi_byTarSocketNo, pi_pOrgObject ) )
		return FALSE;

	CBoardItem * pOrgItem = static_cast< CBoardItem * >( pi_pOrgObject );

	// 악세사리의 경우에는 효과만 적용
	if( ( CEPT_UI_TO_CLIENT[pi_byTarSocketNo] == -1 ) || ( CEPT_UI_TO_CLIENT[pi_byTarSocketNo] >= MAX_EQUIP_PART ) )
	{
		_GetAvatar()->GetConditionInfo()->ApplyItemEffect( pOrgItem, TRUE );
	}
	// 실제 장착 메시 변경
	else
	{
		_GetAvatar()->PutOnEquipment( pOrgItem );
	}
	

	if( ( pOrgItem->GetItemKind() == IEPT_WEAPON ) &&	
		( pOrgItem->GetWeaponCategoryID() == IWCT_MINE ) &&
		( _GetAvatar()->GetCombatMode() == CSI_COMBAT ) )	
	{
		_GetMiningBoard()->OpenWindow();
	}

	return TRUE;
}

//------------------------------------------------------------------------------

BOOL
CEquipBoard::Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject )
{
	CBoardItem *	pOrgItem	= static_cast< CBoardItem * >( pi_pOrgObject );
	char			byEquipPart = CEPT_UI_TO_CLIENT[ pi_bySocketNo ];

	// 악세사리의 경우에는 효과만 적용
	if( ( byEquipPart == 0xFF ) || ( byEquipPart >= MAX_EQUIP_PART ) )
	{
		_GetAvatar()->GetConditionInfo()->ApplyItemEffect( pOrgItem , FALSE );
	}
	// 실제 장착 메시 변경
	else
	{
		_GetAvatar()->PutOffEquipment( byEquipPart );
	}
	
	// 무기일경우, 만약 채굴중이라면 채굴 취소
	if( byEquipPart == IEPT_WEAPON )
	{
		if( _GetAvatar()->IsEqualAction( CAI_ITEM_USE_MINING ) )
		{			
			_GetMiningBoard()->Mining_Cancel();			
		}

		if( _GetMiningBoard()->IsOpen() )
			_GetMiningBoard()->CloseWindow();
	}

	// 삭제
	CItemBoard::Remove_ProcessRemoveObject( pi_byPackNo, pi_bySocketNo, pi_pOrgObject );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CEquipBoard::UI_CheckBeforeAdd_CheckHoverObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeAdd_CheckHoverObject() )
		return FALSE;

	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );

	if( !IsEquipableItem( pHoverItem, _GetGBM()->GetTarSocketNo() ) )
		return FALSE;

	return CItemBoard::UI_CheckBeforeAdd_CheckHoverObject();
}

//------------------------------------------------------------------------------

BOOL
CEquipBoard::UI_AddRequest_ProcessObject( void )
{	
	if( _GetGBM()->GetOrgBoardType() == _GetGBM()->GetTarBoardType() )
	{
		UI_AddResult( TRUE );

		return TRUE;
	}

	CBoardItem * pHoverItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );	

	if( pHoverItem->GetItemKind() < MAX_EQUIP_PART )
		return m_pNetMsgProcessor_Item->EquipPartRequest_clzo( pHoverItem->GetIndex() );
	else
		return m_pNetMsgProcessor_Item->EmbellishRequest_clzo( _GetGBM()->GetOrgBoardType(), pHoverItem->GetIndex(), ID_INVALID );
}

//------------------------------------------------------------------------------

void
CEquipBoard::UI_AddResult_ProcessObject( void )
{
	CItemBoard::UI_AddResult_ProcessObject();

	PlayWave( SND_ID_ITEM_EQUIP );
}

//==============================================================================

BOOL
CEquipBoard::UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity )
{
	// 아바타가 유닛을 타고 있는 상태일 경우는 아이템의 장착/해제 불가능
	if( _GetAvatar()->IsEqualUnitMountState( UMS_MOUNT ) )
	{
		_SetSystemMsg( "유닛 탑승중에는 아이템을 장착 / 해제할 수 없습니다." );
		return FALSE;
	}

	// 아바타가 전투중에는 무기의 장착/해제 불가능
	if( ( CEPT_UI_TO_CLIENT[_GetGBM()->GetOrgSocketNo()] == IEPT_WEAPON ) ||
		( CEPT_UI_TO_CLIENT[_GetGBM()->GetOrgSocketNo()] == IEPT_SHIELD ) )
	{
		if( !_GetAvatar()->IsEnableMove() )
		{
			_SetSystemMsg( "아바타가 전투중이거나 스킬/포스 등의 사용중에는 무기/방패를 장착/해제할 수 없습니다." );
			return FALSE;
		}
	}

	if( !CItemBoard::UI_RemoveRequest_ProcessObject( pi_byQuantity ) )
		return FALSE;		

	PlayWave( SND_ID_ITEM_PICKUP );
	
	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CEquipBoard::UI_PutOffEquipment_By_HotKey( BYTE pi_bySocketNo )
{
	if( !_GetGBM()->IsEnableProcess() )
		return FALSE;

	if( _GetMainGame()->IsRequestLogout() )
		return FALSE;

	if( ( CEPT_UI_TO_CLIENT[pi_bySocketNo] == IEPT_WEAPON ) ||
		( CEPT_UI_TO_CLIENT[pi_bySocketNo] == IEPT_SHIELD ) )
	{
		if( !_GetAvatar()->IsEnableMove() )
		{
			_SetSystemMsg( "아바타가 전투중이거나 스킬/포스 등의 사용중에는 무기/방패를 장착/해제할 수 없습니다." );
			return FALSE;
		}
	}

	// 인벤토리 빈 공간을 체크. 최근 열린 배낭에 넣는다
	BYTE byInvenPackNo, byInvenSocketNo;	
	byInvenPackNo = _GetInventoryBoard()->GetActivePackNo();
	byInvenSocketNo = _GetInventoryBoard()->FindEmptySocket( byInvenPackNo );
	if( byInvenSocketNo == 0xFF )
	{
		if( !_GetInventoryBoard()->FindEmptySocket( byInvenPackNo, byInvenSocketNo ) )
		{
			_SetSystemMsg( "인벤토리 빈 공간이 없습니다." );
			return FALSE;
		}
	}

	if( !UI_RemoveRequest( 0, pi_bySocketNo, 1 ) )
		return FALSE;	

	if( !_GetInventoryBoard()->UI_AddRequest( byInvenPackNo, byInvenSocketNo ) )			
		return FALSE;

	// 장착 해제한 아이템을 넣은 배낭을 열어준다
	if( !_GetInventoryBoard()->UI_IsOpenedPack( byInvenPackNo ) ||
		_GetInventoryBoard()->GetActivePackNo() != byInvenPackNo )
		_GetInventoryBoard()->UI_OpenPack( byInvenPackNo, TRUE );

	return TRUE;
}

BYTE
CEquipBoard::GetEmptySocketByEquipPart( BYTE pi_byEquipPart )
{
	// 일반 아이템
	if( pi_byEquipPart < IEPT_RING )
	{
		if( !GetItem( CEPT_CLIENT_TO_UI[pi_byEquipPart] ) )
			return CEPT_CLIENT_TO_UI[pi_byEquipPart];
	}
	// 장신구
	else
	{
		if( pi_byEquipPart == IEPT_AMULET )
		{
			if( !GetItem( UIPT_AMULET_LEFT ) )
				return UIPT_AMULET_LEFT;
			else if( !GetItem( UIPT_AMULET_RIGHT ) )
				return UIPT_AMULET_RIGHT;
		}
		else if( pi_byEquipPart == IEPT_RING )
		{
			if( !GetItem( UIPT_RING_LEFT ) )
				return UIPT_RING_LEFT;
			else if( !GetItem( UIPT_RING_RIGHT ) )
				return UIPT_RING_RIGHT;
		}
		else if( pi_byEquipPart == IEPT_BULLET )
		{
			if( !GetItem( UIPT_BULLET_LEFT ) )
				return UIPT_BULLET_LEFT;
			else if( !GetItem( UIPT_BULLET_RIGHT ) )
				return UIPT_BULLET_RIGHT;
		}
	}

	return ID_INVALID;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

LRESULT		
CEquipBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UI_MOUSE_MSG	stRecvMsg( lParam );

	switch( uMsg )
	{
		case WM_BOARD_EQUIP_HOT_KEY	:
			UI_PutOffEquipment_By_HotKey( stRecvMsg.m_bySocketNo );
			break;
	}

	return CItemBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD
CEquipBoard::GetUsingBulletIndex( void )
{
	CBoardItem * pWeaponItem = GetItem( UIPT_WEAPON );

	// 무기 장착이 안 되어 있다면 리턴
	if( !pWeaponItem )
		return ID_INVALID;

	// 원거리 무기가 아니라면 리턴
	if( !( pWeaponItem->IsEqualWeaponCategoryID( IWCT_BOW ) ||
		   pWeaponItem->IsEqualWeaponCategoryID( IWCT_FIREARM ) ||
		   pWeaponItem->IsEqualWeaponCategoryID( IWCT_LUANCHER ) ) )
		return ID_INVALID;

	// 탄환 인덱스 반환
	CBoardItem * pBulletItem = GetItem( m_byUsingBulletSocketNo );
	if( pBulletItem )
		return pBulletItem->GetIndex();

	return ID_INVALID;
}

//==============================================================================

BOOL
CEquipBoard::IsEquipableItem( CBoardItem * pi_pItem, BYTE pi_byTarSocketNo )
{	
	if( CEPT_UI_TO_CLIENT[pi_byTarSocketNo] != pi_pItem->GetItemKind() )
		return FALSE;

	// 장착아이템인가
	if( !( pi_pItem->IsEquipmentItem() ||
		 ( pi_pItem->GetItemKind() >= IEPT_RING && pi_pItem->GetItemKind() <= IEPT_BULLET ) ) )
		return FALSE;
	
	// 레벨, 숙련도등 체크
	if( !_GetAvatar()->IsUsableItem( pi_pItem, TRUE ) )
		return FALSE;	

	// 장착 가능한가
	if( !_GetAvatar()->IsEquipable( pi_pItem ) )
		return FALSE;

	return TRUE;
}

void
CEquipBoard::BulletAutoReload( BYTE pi_bySocketNo, DWORD pi_dwDTIndex )
{
	if( pi_dwDTIndex == ID_INVALID )
		return;

	if( pi_bySocketNo == UIPT_BULLET_LEFT || pi_bySocketNo == UIPT_BULLET_RIGHT )
	{
		// 장착창에 같은 종류의 아이템이 있다면 return
		CBoardItem* pExistItem = NULL;
		BYTE		byPackNo = 0xFF, bySocketNo = 0xFF;

		pExistItem = GetRemainOfQuantityItemByDTIndex( byPackNo, bySocketNo,
										IEPT_BULLET, pi_dwDTIndex, 1 );
		if( pExistItem )
			return;

		// 장착창에 없다면 인벤토리에서 찾는다.
		pExistItem = _GetInventoryBoard()->GetRemainOfQuantityItemByDTIndex( byPackNo, bySocketNo,
										IEPT_BULLET, pi_dwDTIndex, 1 );
		if( pExistItem )
			_GetInventoryBoard()->UI_PutOnEquipment_By_HotKey( byPackNo, bySocketNo );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CBoardItem *
CEquipBoard::GetItem( BYTE pi_bySocketNo )
{
	return static_cast< CBoardItem * >( GetObject( 0, pi_bySocketNo ) );
}

#endif