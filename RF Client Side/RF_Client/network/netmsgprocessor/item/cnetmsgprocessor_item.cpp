////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Item Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

//#include "../../../Character/Avatar/CAvatar.h"
//#include "../../../DataFile.h"
//#include "../../../DataTable/CItemDataMgr.h"
#include "../../../GameObject/Manager/CCharacterMgr.h"
#include "../../../GameObject/Manager/CGroundItemManager.h"
#include "../../../GameObject/Board/Common/CBoardManager.h"

#include "CNetMsgProcessor_Item.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_Item::CNetMsgProcessor_Item()
{
}

CNetMsgProcessor_Item::~CNetMsgProcessor_Item()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetMsgProcessor_Item::SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	return CNetMsgProcessor_Base::SendNetMessage( item_msg, pi_byMsgIndex, pi_pMsg, pi_nSize );
}

bool
CNetMsgProcessor_Item::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	if( pMsgHeader->m_byType[0] != item_msg )
		return false;

	switch( pMsgHeader->m_byType[1] )
	{		
		case equip_part_result_zocl			:
			EquipPartResult_zocl( pi_pMsg );
			break;

		case embellish_result_zocl			:
			EmbellishResult_zocl( pi_pMsg );
			break;

		case off_part_result_zocl			:
			OffPartResult_zocl( pi_pMsg );
			break;

		case use_potion_result_zocl			:
			UsePotionResult_zocl( pi_pMsg );
			break;

		case add_bag_result_zocl			:
			AddBagResult_zocl( pi_pMsg );
			break;		

		case itembox_take_new_result_zocl	:
			ItemboxTakeNewResult_zocl( pi_pMsg );
			break;

		case itembox_take_add_result_zocl	:
			ItemboxTakeAddResult_zocl( pi_pMsg );
			break;

		case throw_storage_result_zocl		:
			ThrowStorageResult_zocl( pi_pMsg );
			break;

		case itembox_state_change_zocl		:
			ItemboxStateChange_zocl( pi_pMsg );
			break;

		case delete_storage_inform_zocl		:
			DeleteStorageInform_zocl( pi_pMsg );
			break;

		case make_item_result_zocl			:
			MakeItemResult_zocl( pi_pMsg );
			break;

		case upgrade_item_result_zocl		:
			UpgradeItemResult_zocl( pi_pMsg );
			break;

		case downgrade_item_result_zocl		:
			DowngradeItemResult_zocl( pi_pMsg );
			break;

		default :
			m_pNetworkMgr->NetLogWrite( "Unknown Message : CNetMsgProcessor_Item::AnalysisServerPacket()\n" );
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타의 아이템 장착을 서버에 요청한다.
BOOL
CNetMsgProcessor_Item::EquipPartRequest_clzo( DWORD pi_dwItemIndex )
{
	_equip_part_request_clzo stSendMsg;	

	stSendMsg.Item.byStorageCode	= IBT_INVENTORY;
	stSendMsg.Item.wItemSerial		= pi_dwItemIndex;
		
	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( equip_part_request_clzo,						
						   &stSendMsg,
						   sizeof( _equip_part_request_clzo ) );
}

void
CNetMsgProcessor_Item::EquipPartResult_zocl( char * pi_pMsg )
{
	_equip_part_result_zocl * pRecvMsg = 
		reinterpret_cast< _equip_part_result_zocl * >( pi_pMsg );		

	_GetGBM()->SetRequestProcessState( FALSE );
	
	if( pRecvMsg->byErrCode == 0 )
	{
		if( _GetGBM()->GetOperationType() == IOT_SWAP )
		{
			if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )	
				_GetInventoryBoard()->UI_SwapResult( TRUE );
			else
				_GetEquipBoard()->UI_SwapResult( TRUE );
		}
		else
		{
			_GetEquipBoard()->UI_AddResult( TRUE );
		}
	}	
	else
	{
		if( _GetGBM()->GetOperationType() == IOT_SWAP )
		{
			if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )	
				_GetInventoryBoard()->UI_SwapResult( FALSE );
			else
				_GetEquipBoard()->UI_SwapResult( FALSE );
		}
		else
		{
			_GetEquipBoard()->UI_AddResult( FALSE );
		}

		switch( pRecvMsg->byErrCode )
		{
			case 1	: _SetSystemMsg( "아이템 장착 실패 : 저장 위치 틀림" ); break;
			case 2	: _SetSystemMsg( "아이템 장착 실패 : 없는 아이템" ); break;
			case 3	: _SetSystemMsg( "아이템 장착 실패 : 장착 아이템이 아님" ); break;
			case 4	: _SetSystemMsg( "아이템 장착 실패 : 사용할 수 없는 종족임" ); break;
			case 5	: _SetSystemMsg( "아이템 장착 실패 : 방패를 들었음" ); break;
			case 6	: _SetSystemMsg( "아이템 장착 실패 : 양손 무기를 들었음" ); break;
			case 7	: _SetSystemMsg( "아이템 장착 실패 : 숙련도가 낮음" ); break;
			default	: _SetSystemMsg( "아이템 장착 실패 : 알 수 없는 에러 ( %d )", pRecvMsg->byErrCode );
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 장신구 / 탄환등의 장착 요청
BOOL
CNetMsgProcessor_Item::EmbellishRequest_clzo( BYTE pi_byTarBoardType, DWORD pi_dwTarItemIndex, DWORD pi_dwOrgItemIndex )
{
	_embellish_request_clzo stSendMsg;

	CBoardItem * pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );	

	stSendMsg.Item.byStorageCode	= pi_byTarBoardType;
	stSendMsg.Item.wItemSerial		= pi_dwTarItemIndex;
	stSendMsg.wChangeSerial			= pi_dwOrgItemIndex;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( embellish_request_clzo,						
						   &stSendMsg,
						   sizeof( _embellish_request_clzo ) );
}

void	
CNetMsgProcessor_Item::EmbellishResult_zocl( char * pi_pMsg )
{
	_embellish_result_zocl * pRecvMsg = 
		reinterpret_cast< _embellish_result_zocl * >( pi_pMsg );		

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byErrCode == 0 )
	{
		if( _GetGBM()->GetOperationType() == IOT_SWAP )
		{
			if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )				
				_GetInventoryBoard()->UI_SwapResult( TRUE );			
			else
				_GetEquipBoard()->UI_SwapResult( TRUE );
		}
		else
		{
			_GetEquipBoard()->UI_AddResult( TRUE );
		}
	}	
	else
	{
		if( _GetGBM()->GetOperationType() == IOT_SWAP )
		{
			if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )				
				_GetInventoryBoard()->UI_SwapResult( FALSE );			
			else
				_GetEquipBoard()->UI_SwapResult( FALSE );
		}
		else
		{
			_GetEquipBoard()->UI_AddResult( FALSE );
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타의 아이템 해제를 서버에 요청한다.
BOOL	
CNetMsgProcessor_Item::OffPartRequest_clzo( void )
{
	_off_part_request_clzo stSendMsg;

	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pHoverItem )
		return FALSE;

	if( pHoverItem->GetItemKind() < MAX_EQUIP_PART )
		stSendMsg.Item.byStorageCode	= IBT_EQUIP;
	else
		stSendMsg.Item.byStorageCode	= _STORAGE_POS::EMBELLISH;
	stSendMsg.Item.wItemSerial			= pHoverItem->GetIndex();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( off_part_request_clzo,
						   &stSendMsg,
						   sizeof( _off_part_request_clzo ) );
}

void
CNetMsgProcessor_Item::OffPartResult_zocl( char * pi_pMsg )
{
	_off_part_result_zocl * pRecvMsg = reinterpret_cast< _off_part_result_zocl * >( pi_pMsg );		

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byErrCode == 0 )
	{
		if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )
		{
			_GetInventoryBoard()->UI_AddResult( TRUE );

			// 단축키에 의한 장착 - 이 후의 액션이 있을 경우 실행
			// ex ) 방패 장착시, 인벤토리에서 양손 무기를 장착하려 했을 경우
			if( _GetEquipBoard()->ExistReservedEquipItem() )
			{
				_GetInventoryBoard()->UI_PutOnEquipment_By_HotKey( _GetEquipBoard()->GetReservedEquipItemPackNo(),
																   _GetEquipBoard()->GetReservedEquipItemSocketNo() );

				_GetEquipBoard()->ClearReservedEquipItem();
			}
		}
	}	
	else
	{
		if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )
			_GetInventoryBoard()->UI_AddResult( FALSE );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int g_count = 0;

// 포션 사용
BOOL
CNetMsgProcessor_Item::UsePotionRequest_clzo( void )
{
	_use_potion_request_clzo stSendMsg;	

	CBoardItem * pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );

	stSendMsg.Item.byStorageCode	= _GetGBM()->GetOrgBoardType();
	stSendMsg.Item.wItemSerial		= pOrgItem->GetIndex();
	stSendMsg.Item.byNum			= 1;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( use_potion_request_clzo,
						   &stSendMsg,
						   sizeof( _use_potion_request_clzo ) );
}

void
CNetMsgProcessor_Item::UsePotionResult_zocl( char * pi_pMsg )
{
	_use_potion_result_zocl	* pRecvMsg = reinterpret_cast< _use_potion_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byErrCode == 0 )
	{
		if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
			_GetInventoryBoard()->UI_UseResult( TRUE );
		else if( _GetGBM()->GetOrgBoardType() == IBT_ITEM_BELT )
			_GetBeltBoard()->UI_UseResult( TRUE );

		_GetAvatar()->SetHP( pRecvMsg->wHP );
		_GetAvatar()->SetFP( pRecvMsg->wFP );
		_GetAvatar()->SetSP( pRecvMsg->wSP );
	}
	else
	{
		if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
		{
			_GetInventoryBoard()->UI_UseResult( FALSE );
		}
		else if( _GetGBM()->GetOrgBoardType() == IBT_ITEM_BELT ) 
		{
			_GetBeltBoard()->UI_UseResult( FALSE );
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL	
CNetMsgProcessor_Item::AddBagRequest_clzo( void )
{
	_add_bag_request_clzo stSendMsg;

	CBoardItem * pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	if( !pOrgItem )
		return FALSE;

	stSendMsg.wBagItemSerial = pOrgItem->GetIndex();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( add_bag_request_clzo,
						   &stSendMsg,
						   sizeof( _add_bag_request_clzo ) );
}

void
CNetMsgProcessor_Item::AddBagResult_zocl( char * pi_pMsg )
{
	_add_bag_result_zocl * pRecvMsg = 
		reinterpret_cast< _add_bag_result_zocl * >( pi_pMsg );		

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byErrCode == 0 )
	{
		_GetInventoryBoard()->UI_UseResult( TRUE );
	}	
	else
	{
		_GetInventoryBoard()->UI_UseResult( FALSE );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아이템 루팅 요청
BOOL
CNetMsgProcessor_Item::ItemboxTakeRequest_clzo( DWORD pi_dwGroundItemIndex, DWORD pi_dwInventoryItemIndex )
{
	_itembox_take_request_clzo stSendMsg;

	stSendMsg.wItemBoxIndex	= pi_dwGroundItemIndex;
	stSendMsg.wAddSerial	= pi_dwInventoryItemIndex;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( itembox_take_request_clzo,
						   &stSendMsg,
						   sizeof( _itembox_take_request_clzo ) );
}

// 아이템 루팅 요청에 대한 응답 ( 새로운 아이템 )
void
CNetMsgProcessor_Item::ItemboxTakeNewResult_zocl( char * pi_pMsg )
{
	_itembox_take_new_result_zocl * pRecvMsg = 
		reinterpret_cast< _itembox_take_new_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );
	
	// 아이템 루팅 성공
	if( pRecvMsg->sErrorCode == 0 )
	{
		CBoardItem clsLootItem;

		if( !clsLootItem.Create( IEPT_SERVER_TO_CLIENT[pRecvMsg->byItemTableCode], pRecvMsg->wItemIndex ) )
			return;

		// 아이템을 획득하면 아이템의 Index는 Serial를 가지게 된다.
		clsLootItem.SetIndex( pRecvMsg->wItemSerial );

		// 소모성 아이템일 경우는 수량 설정
		if( clsLootItem.IsSplitable() )
		{
			clsLootItem.SetQuantity( pRecvMsg->dwCurDurPoint );
		}
		// 소모성 아이템이 아닐 내구도 설정
		else
		{
			clsLootItem.SetCurMaxDurability( clsLootItem.GetOrgMaxDurability() );
			clsLootItem.SetDurability( pRecvMsg->dwCurDurPoint );
			clsLootItem.SetQuantity( 1 );
		}

		// 아이템 업그레이드 설정
		clsLootItem.InsertTalikFromServerFlag( pRecvMsg->dwUptInfo );

		// pick up
		_GetInventoryBoard()->UI_PickUpItemResult_CreateItem( &clsLootItem );
	}
	// 아이템 루팅 실패
	else
	{
		switch( pRecvMsg->sErrorCode )
		{
			case 1 : _SetSystemMsg( "아이템 루팅 실패 - 빈 공간이 없음" ); break;
			case 2 : _SetSystemMsg( "아이템 루팅 실패 - 인벤토리에 기존의 아이템이 없음" ); break;
			case 3 : _SetSystemMsg( "아이템 루팅 실패 - 인벤토리에 빈공간이 없음" ); break;
			case 4 : _SetSystemMsg( "아이템 루팅 실패 - 루팅하려는 그라운드 아이템이 없음" ); break;
			case 5 : _SetSystemMsg( "아이템 루팅 실패 - 루팅 권한이 없음" ); break;
			case 6 : _SetSystemMsg( "아이템 루팅 실패 - 거리 제한" ); break;
			default: _SetSystemMsg( "아이템 루팅 실패 - 알 수 없는 에러" ); break;
		}
	}
}

// 아이템 루팅 요청에 대한 응답 ( 기존 아이템에 수량만 추가 )
void
CNetMsgProcessor_Item::ItemboxTakeAddResult_zocl( char * pi_pMsg )
{
	_itembox_take_add_result_zocl * pRecvMsg = 
		reinterpret_cast< _itembox_take_add_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	// 아이템 루팅 성공
	if( pRecvMsg->sErrorCode == 0 )
	{
		_GetInventoryBoard()->UI_PickUpItemResult_AddQuantity( pRecvMsg->wItemSerial, pRecvMsg->byAmount );

		_GetAvatar()->SetReceiveNetMsg( "루팅 성공" );
	}
	// 아이템 루팅 실패
	else
	{
		switch( pRecvMsg->sErrorCode )
		{
			case 1 : _SetSystemMsg( "아이템 루팅 실패 - 빈 공간이 없음" ); break;
			case 2 : _SetSystemMsg( "아이템 루팅 실패 - 인벤토리에 기존의 아이템이 없음" ); break;
			case 3 : _SetSystemMsg( "아이템 루팅 실패 - 인벤토리에 빈공간이 없음" ); break;
			case 4 : _SetSystemMsg( "아이템 루팅 실패 - 루팅하려는 그라운드 아이템이 없음" ); break;
			case 5 : _SetSystemMsg( "아이템 루팅 실패 - 루팅 권한이 없음" ); break;
			case 6 : _SetSystemMsg( "아이템 루팅 실패 - 거리 제한" ); break;
			default: _SetSystemMsg( "아이템 루팅 실패 - 알 수 없는 에러" ); break;
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아이템을 그라운드에 버린다.
BOOL
CNetMsgProcessor_Item::ThrowStorageRequest_clzo( void )
{
	_throw_storage_request_clzo stSendMsg;	

	CBoardItem * pHoverItem	= _GetGBM()->GetHoverItem();
	if( !pHoverItem )
		return FALSE;

	stSendMsg.Item.byStorageCode	= GetStoragePos( _GetGBM()->GetOrgBoardType(), pHoverItem->GetItemKind() );
	stSendMsg.Item.wItemSerial		= pHoverItem->GetIndex();
	stSendMsg.Item.byNum			= pHoverItem->GetQuantity();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( throw_storage_request_clzo,
						   &stSendMsg,
						   sizeof( _throw_storage_request_clzo ) );
}

void	
CNetMsgProcessor_Item::ThrowStorageResult_zocl( char * pi_pMsg )
{
	_throw_storage_result_zocl * pRecvMsg = 
		reinterpret_cast< _throw_storage_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_GetGBM()->PutDownItemResult();		
	}
	// 실패
	else
	{
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 그라운드 아이템의 상태가 변경되었음
void	
CNetMsgProcessor_Item::ItemboxStateChange_zocl( char * pi_pMsg )
{
	_itembox_state_change_zocl * pRecvMsg = 
		reinterpret_cast< _itembox_state_change_zocl * >( pi_pMsg );

	CGroundItem * l_pItem = _GetGroundItemMgr()->GetItem( pRecvMsg->wItemBoxIndex );
	if( !l_pItem )
		return;

	l_pItem->SetOwner( pRecvMsg->byState, pRecvMsg->dwOwerSerial );	
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타의 아이템이 삭제되었음 ( 모든 아이템에 해당 )
void
CNetMsgProcessor_Item::DeleteStorageInform_zocl( char * pi_pMsg )
{
	_delete_storage_inform_zocl * pRecvMsg = reinterpret_cast< _delete_storage_inform_zocl * >( pi_pMsg );
	
	CBoardItem *	pUsingItem		= _GetAvatar()->GetUsingItem();
	CObjectBoard *	pObjectBoard	= static_cast< CObjectBoard * >( _GetGBM()->GetBoard( pRecvMsg->byStorageCode ) );
	if( !pObjectBoard )
		return;

	// 사용한 아이템이면 UI_UseResult를 호출해야한다.
	if( pUsingItem && pUsingItem->IsEqual( pRecvMsg->wSerial ) )
	{	
		pObjectBoard->UI_UseResult( TRUE );
	}
	else
	{
		if( pRecvMsg->byStorageCode == IBT_INVENTORY && _GetAvatar()->IsEqualUnitMountState( UMS_MOUNT ) )
		{
			/*
			CBoardItem * pUnitItem = _GetInventoryBoard()->GetMountedUnit();
			if( l_pUnitItem )
			{
				if( l_pUnitItem->GetIndex() == pRecvMsg->wSerial )
				{
					_GetAvatar()->SetEffectInfo( CAI_DEATH_DIE );
					_GetAvatar()->DismountUnit();
				}
			}
			*/
		}
	}

	// 탄환 자동 장착을 위해서 아이템 백업
	DWORD	dwBackupItemDTIndex = ID_INVALID;
	BYTE	byBackupPackNo, byBackupSocketNo;
	if( pObjectBoard->IsEqual( IBT_EQUIP ) )
	{
		CBoardItem * pOrgItem = _GetEquipBoard()->GetItemByIndex( byBackupPackNo, byBackupSocketNo,
																  pRecvMsg->wSerial );
		if( pOrgItem && pOrgItem->IsEqualItemKind( IEPT_BULLET ) )
			dwBackupItemDTIndex = pOrgItem->GetDTIndex();
	}

	// 아이템 삭제
	pObjectBoard->RemoveByIndex( pRecvMsg->wSerial );

	// 탄환 자동 장착
	_GetEquipBoard()->BulletAutoReload( byBackupSocketNo, dwBackupItemDTIndex );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아이템 제작을 서버에 요청한다.
BOOL
CNetMsgProcessor_Item::MakeItemRequest_clzo( void )
{
	_make_item_request_clzo stSendMsg;

	stSendMsg.wManualIndex				= _GetItemWorkBoard()->GetItemWorkData()->GetDTIndex();		

	stSendMsg.ipMakeTool.byStorageCode	= IBT_INVENTORY;
	stSendMsg.ipMakeTool.wItemSerial	= _GetItemWorkBoard()->GetWorkToolItemIndex();


	stSendMsg.byMaterialNum				= _GetItemWorkBoard()->GetItemWorkData()->GetTotalMaterialNum();
	for( int i = 0; i < stSendMsg.byMaterialNum; ++i )
	{
		stSendMsg.ipMaterials[i].byStorageCode	= IBT_INVENTORY;
		stSendMsg.ipMaterials[i].wItemSerial	= _GetItemWorkBoard()->GetMaterialIndex( i );
		stSendMsg.ipMaterials[i].byNum			= _GetItemWorkBoard()->GetItemWorkData()->GetMaterialNum( i );
	}

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( make_item_request_clzo,
						   &stSendMsg,
						   sizeof( _make_item_request_clzo ) );	

	/*
	#define make_item_request_clzo 15
	struct _make_item_request_clzo{		//upt(9/24)
		_STORAGE_POS_INDIV ipMakeTool;	//제작툴이 있는 위치
		WORD wManualIndex;				//제작메뉴얼인덱스

		BYTE byMaterialNum;				//사용할 재료수
		_STORAGE_POS_INDIV ipMaterials[max_item_make_material_num];	//사용할 재료가 있는 위치

		_make_item_request_clzo(){
			byMaterialNum = 0;
		}

		int size(){
			return sizeof(*this)-sizeof(_STORAGE_POS_INDIV)*(max_item_make_material_num-byMaterialNum);
		}
	};*/
}

// 아이템 제작에 대한 결과
void
CNetMsgProcessor_Item::MakeItemResult_zocl( char * pi_pMsg )
{
	_make_item_result_zocl * pRecvMsg = 
		reinterpret_cast< _make_item_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_GetItemWorkBoard()->ItemWork_Result( TRUE );		
	}
	// 실패
	else
	{
		if( pRecvMsg->byErrCode == miss_make_random )
		{
			_GetItemWorkBoard()->ItemWork_Result( FALSE );			
		}
		else
		{
			switch( pRecvMsg->byErrCode )
			{
				case error_make_nothing_tool		: _SetSystemMsg( "아이템 제작 실패 : 제작 도구가 아닙니다." ); break;
				case error_make_not_tool			: _SetSystemMsg( "아이템 제작 실패 : 제작 도구가 없습니다." ); break;
				case error_make_full_inven			: _SetSystemMsg( "아이템 제작 실패 : 인벤토리에 빈 공간이 없습니다." ); break;
				case error_make_nothing_material	: _SetSystemMsg( "아이템 제작 실패 : 제작에 필요한 재료의 수량이 부족합니다." ); break;
				case error_make_mix_fail			: _SetSystemMsg( "아이템 제작 실패 : 제작에 필요한 재료가 없습니다." ); break;
				default : _SetSystemMsg( "아이템 제작 실패 : 알 수 없는 에러" ); break;
			}
		}		
	}

	/*
	#define make_item_result_zocl 16
	struct _make_item_result_zocl{	//upt(9/24)
		BYTE byErrCode;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아이템 업그레이드를 서버에 요청한다.
BOOL	
CNetMsgProcessor_Item::UpgradeItemRequest_clzo( void )
{	
	_upgrade_item_request_clzo stSendMsg;

	stSendMsg.m_posToolItem.wItemSerial		= _GetItemUpgradeBoard()->GetUpgradeItemIndex();
	stSendMsg.m_posToolItem.byStorageCode	= IBT_INVENTORY;

	stSendMsg.m_posUpgItem.wItemSerial		= _GetItemUpgradeBoard()->GetItem( UIAT_ITEM )->GetIndex();
	stSendMsg.m_posUpgItem.byStorageCode	= IBT_INVENTORY;	

	stSendMsg.m_posTalik.wItemSerial		= _GetItemUpgradeBoard()->GetItem( UIAT_TALIK )->GetIndex();
	stSendMsg.m_posTalik.byStorageCode		= IBT_INVENTORY;


	CBoardItem * pItem;
	stSendMsg.byJewelNum = 0;	
	for( int i = UIAT_FIRE; i < UIAT_FIRE + 4; ++i )
	{
		pItem = _GetItemUpgradeBoard()->GetItem( i );
		if( pItem )
		{
			stSendMsg.m_posUpgJewel[stSendMsg.byJewelNum].wItemSerial	= pItem->GetIndex();
			stSendMsg.m_posUpgJewel[stSendMsg.byJewelNum].byStorageCode	= IBT_INVENTORY;
			++stSendMsg.byJewelNum;
		}
	}


	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( upgrade_item_request_clzo,
						   &stSendMsg,
						   sizeof( _upgrade_item_request_clzo ) );

	/*
	#define upgrade_item_request_clzo 17
	struct _upgrade_item_request_clzo {	//add(2/3)
		_STORAGE_POS_INDIV m_posUpgItem;	
		_STORAGE_POS_INDIV m_posTalik;
		_STORAGE_POS_INDIV m_posToolItem;

		BYTE byJewelNum;
		_STORAGE_POS_INDIV m_posUpgJewel[upgrade_jewel_num];

		_upgrade_item_request_clzo(){
			byJewelNum = 0;
		}
		int size(){
			return sizeof(*this)-sizeof(_STORAGE_POS_INDIV)*(upgrade_jewel_num-byJewelNum);
		}
	};*/
}

// 아이템 업그레이드에 대한 결과
void
CNetMsgProcessor_Item::UpgradeItemResult_zocl( char * pi_pMsg )
{
	_upgrade_item_result_zocl * pRecvMsg = 
			reinterpret_cast< _upgrade_item_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_GetItemUpgradeBoard()->ItemUpgrade_Result( TRUE );
		_SetSystemMsg( "아이템 업그레이드에 성공 했습니다." );
	}
	// 실패
	else
	{
		if( pRecvMsg->byErrCode >= miss_upgrade_random )
		{
			// ItemUpgrade_Result_DestroyItem()을 ItemUpgrade_Result()보다 먼저 호출해야 함.
			_GetItemUpgradeBoard()->ItemUpgrade_Result_DestroyItem( pRecvMsg->byErrCode );
			_GetItemUpgradeBoard()->ItemUpgrade_Result( FALSE );

			switch( pRecvMsg->byErrCode )
			{
				case miss_upgrade_random			: _SetSystemMsg( "아이템 업그레이드에 실패 하였습니다." ); break;
				case miss_upgrade_destroy_talik		: _SetSystemMsg( "아이템 업그레이드 실패 : 기존의 탈릭이 모두 파괴되었습니다." ); break;
				case miss_upgrade_destroy_item		: _SetSystemMsg( "아이템 업그레이드 실패 : 업그레이드 하려던 아이템이 파괴되었습니다." ); break;
			}
		}
		else
		{
			switch( pRecvMsg->byErrCode )
			{
				case error_upgrade_nothing_talik	: _SetSystemMsg( "아이템 업그레이드 실패 : 소유하지 않은 탈릭입니다." ); break;
				case error_upgrade_not_talik		: _SetSystemMsg( "아이템 업그레이드 실패 : 탈릭이 아닙니다." ); break;
				case error_upgrade_nothing_tool		: _SetSystemMsg( "아이템 업그레이드 실패 : 업그레이드 툴이 아닙니다." ); break;
				case error_upgrade_nothing_item		: _SetSystemMsg( "아이템 업그레이드 실패 : 업그레이드 하려는 아이템이 없습니다." ); break;
				case error_upgrade_nothing_jewel	: _SetSystemMsg( "아이템 업그레이드 실패 : 소유하지 않은 보석입니다." ); break;
				case error_upgrade_not_jewel		: _SetSystemMsg( "아이템 업그레이드 실패 : 보석이 보석입니다." ); break;
				case error_upgrade_overlap_tol		: _SetSystemMsg( "아이템 업그레이드 실패 : 이미 내성탈릭을 이용해 업그레이드한 아이템입니다." ); break;
				case error_upgrade_not_able			: _SetSystemMsg( "아이템 업그레이드 실패 : 업그레이드가 불가능한 아이템입니다." ); break;
				case error_upgrade_full_upgrade		: _SetSystemMsg( "아이템 업그레이드 실패 : 이미 최고 단계까지 업그레이드 되어있는 아이템입니다." ); break;
				case error_upgrade_full_tailk		: _SetSystemMsg( "아이템 업그레이드 실패 : 업그레이드 하려는 아이템에 해당 탈릭을 추가할 수 없습니다." ); break;
				default : _SetSystemMsg( "아이템 업그레이드 실패 : 알 수 없는 에러." ); break;
			}
		}
	}

	/*
	#define upgrade_item_result_zocl 18
	struct _upgrade_item_result_zocl {
		BYTE byErrCode; //1;//없는아이템 2;//업글만땅했음 3;//업글안돼는 아이템 
						//5;//더이상해당탈릭을추가할수없음 6;//이아이템에는해당탈릭을추가할수없음
						//100;//랜덤실패, 101;//기존탈릭파괴 102;//아이템파괴
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아이템 다운그레이드를 서버에 요청한다.
BOOL	
CNetMsgProcessor_Item::DowngradeItemRequest_clzo( void )
{
	_downgrade_item_request_clzo stSendMsg;

	stSendMsg.m_posToolItem.wItemSerial		= _GetItemUpgradeBoard()->GetUpgradeItemIndex();
	stSendMsg.m_posToolItem.byStorageCode	= IBT_INVENTORY;

	stSendMsg.m_posUpgItem.wItemSerial		= _GetItemUpgradeBoard()->GetItem( UIAT_ITEM )->GetIndex();
	stSendMsg.m_posUpgItem.byStorageCode	= IBT_INVENTORY;	

	stSendMsg.m_posTalik.wItemSerial		= _GetItemUpgradeBoard()->GetItem( UIAT_TALIK )->GetIndex();
	stSendMsg.m_posTalik.byStorageCode		= IBT_INVENTORY;

	
	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( downgrade_item_request_clzo,
						   &stSendMsg,
						   sizeof( _downgrade_item_request_clzo ) );
	
	/*
	#define downgrade_item_request_clzo 19
	struct _downgrade_item_request_clzo {	
		_STORAGE_POS_INDIV m_posUpgItem;	
		_STORAGE_POS_INDIV m_posTalik;
		_STORAGE_POS_INDIV m_posToolItem;
	};*/
}

// 아이템 다운그레이드에 대한 결과
void
CNetMsgProcessor_Item::DowngradeItemResult_zocl( char * pi_pMsg )
{
	_downgrade_item_result_zocl * pRecvMsg = 
			reinterpret_cast< _downgrade_item_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_GetItemUpgradeBoard()->ItemUpgrade_Result( TRUE );
		_SetSystemMsg( "아이템 다운그레이드에 성공 했습니다." );
	}
	// 실패
	else
	{
		_GetItemUpgradeBoard()->ItemUpgrade_Result( FALSE );

		switch( pRecvMsg->byErrCode )
		{
			case error_upgrade_nothing_talik	: _SetSystemMsg( "아이템 다운그레이드 실패 : 소유하지 않은 탈릭입니다." ); break;
			case error_upgrade_not_talik		: _SetSystemMsg( "아이템 다운그레이드 실패 : 탈릭이 아닙니다." ); break;
			case error_upgrade_nothing_tool		: _SetSystemMsg( "아이템 다운그레이드 실패 : 다운그레이드 툴이 아닙니다." ); break;
			case error_upgrade_nothing_item		: _SetSystemMsg( "아이템 다운그레이드 실패 : 다운그레이드 하려는 아이템이 없습니다." ); break;
			case error_upgrade_nothing_jewel	: _SetSystemMsg( "아이템 다운그레이드 실패 : 소유하지 않은 보석입니다." ); break;
			case error_upgrade_not_jewel		: _SetSystemMsg( "아이템 다운그레이드 실패 : 보석이 보석입니다." ); break;
			case error_upgrade_overlap_tol		: _SetSystemMsg( "아이템 다운그레이드 실패 : 이미 내성탈릭을 이용해 다운그레이드한 아이템입니다." ); break;
			case error_upgrade_not_able			: _SetSystemMsg( "아이템 다운그레이드 실패 : 다운그레이드가 불가능한 아이템입니다." ); break;
			case error_upgrade_full_upgrade		: _SetSystemMsg( "아이템 다운그레이드 실패 : 이미 최고 단계까지 다운그레이드 되어있는 아이템입니다." ); break;
			case error_upgrade_full_tailk		: _SetSystemMsg( "아이템 다운그레이드 실패 : 다운그레이드 하려는 아이템에 해당 탈릭을 추가할 수 없습니다." ); break;
			case error_upgrade_none_upgrade		: _SetSystemMsg( "아이템 다운그레이드 실패 : 다운그레이드가 되어있지 않은 아이템입니다." ); break;
			default : _SetSystemMsg( "아이템 다운그레이드 실패 : 알 수 없는 에러." ); break;
		}
	}

	/*
	#define downgrade_item_result_zocl 20
	struct _downgrade_item_result_zocl {
		BYTE byErrCode;////1;//없는탈릭 2;//잘못됀탈릭 3;//없는툴 4;//없는아이템 5;//업글안돼는 아이템 6;//업글됀아템아님
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNetMsgProcessor_Item::ShowServerErrorMessage( DWORD pi_dwErrorCode )
{
	/*
	switch( pi_dwErrorCode )
	{
		default	: _SetSystemMsg( "아이템 관련 에러 : 알 수 없는 에러 ( %d )", pi_dwErrorCode ); break;
	}*/
}