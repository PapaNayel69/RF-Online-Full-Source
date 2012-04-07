////////////////////////////////////////////////////////////////////////////////
//
// CInventoryBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../../Manager/CCharacterMgr.h"
#include "../../Manager/CTargetMgr.h"
#include "../../Manager/CGroundItemManager.h"
#include "../Common/CBoardManager.h"
#include "../../../CLand.h"

#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_ItemArrangement.h"
#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_Item.h"
#include "../../../Network/NetMsgProcessor/Dungeon/CNetMsgProcessor_Dungeon.h"

#include "../../../GameProgress\CGP_MainGame.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BUILD_GUARD_TOWER::BUILD_GUARD_TOWER()
{
	m_dwSkillDTIndex			= ID_INVALID;
	m_dwGuardTowerIndex			= ID_INVALID;

	for( int i = 0; i < MAX_ITEM_MATERIAL_NUM; ++i )
		m_dwMaterialIndex[i]	= ID_INVALID;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CInventoryBoard::CInventoryBoard()
{
	m_pNetMsgProcessor_Arrange	= static_cast< CNetMsgProcessor_ItemArrangement * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_ITEM_ARRANGE ) );	

	m_byLootingPackNo			= ID_INVALID;
	m_byLootingSocketNo			= ID_INVALID;	
}

CInventoryBoard::~CInventoryBoard()
{
}

void
CInventoryBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiInventoryWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );

	m_uiWindow->SetPos( _GetUIScreen()->GetPosX() + _GetUIScreen()->GetSizeX() - m_uiWindow->GetSizeX(), _GetUIScreen()->GetPosY() );
}

void
CInventoryBoard::SetEquipWindow( CGUIEquipWindow * pi_pEquipWindow )
{
	m_uiInventoryWindow.SetEquipWindow( pi_pEquipWindow );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CInventoryBoard::AddPack( BYTE pi_byPackNo )
{
	if( !CObjectBoard::AddPack( pi_byPackNo ) )
		return FALSE;

	m_uiInventoryWindow.AddPack( pi_byPackNo );

	return TRUE;
}

// ------------------------------------------------------------------------------

BOOL			
CInventoryBoard::UI_AddPackRequest( void )
{
	BOOL	bIsEmpty = FALSE;
	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		if( !m_listItemPack[i].IsExist() )
		{
			bIsEmpty = TRUE;
			break;
		}				
	}

	if( bIsEmpty )
	{
		m_pNetMsgProcessor_Item->AddBagRequest_clzo();
	}
	else
	{
		_SetSystemMsg( "인벤토리에 가방을 더 이상 추가할 수 없습니다." );
		return FALSE;
	}
	
	return TRUE;
}

void
CInventoryBoard::UI_AddPackResult( void )
{
	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		if( !m_listItemPack[i].IsExist() )
		{
			AddPack( i );
			break;
		}
	}

	Remove( _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), 1 );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CInventoryBoard::UI_OpenPack( BYTE pi_byPackNo, BOOL pi_bOpen )
{	
	m_uiInventoryWindow.OpenPack( pi_byPackNo, pi_bOpen );
}

BOOL
CInventoryBoard::UI_IsOpenedPack( BYTE pi_byPackNo )
{
	return m_uiInventoryWindow.IsOpenedPack( pi_byPackNo );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CInventoryBoard::UI_CheckBeforeAdd_CheckHoverObject( void )
{
	CBoardObject * pHoverObject = _GetGBM()->GetHoverObject();
	if( !pHoverObject )
		return FALSE;

	// force
	if( pHoverObject->IsEqualItemType( BOT_SF ) )
	{
		CSFItem * pSFItem = static_cast< CSFItem * >( pHoverObject );
		if( pSFItem->IsEqualSFType( CAT_FORCE ) )		
			return CItemBoard::UI_CheckBeforeAdd_CheckHoverObject();
		else 
			return FALSE;
	}
	// animus
	else if( pHoverObject->IsEqualItemType( BOT_ANIMUS ) )
	{
		if( _GetAnimusControlBoard()->IsEqualSummonAnimusItem( pHoverObject->GetIndex() ) )
		{
			_SetSystemMsg( "소환중인 애니머스 아이템은 인벤토리로 옮길 수 없습니다." );
			return FALSE;
		}

		return CItemBoard::UI_CheckBeforeAdd_CheckHoverObject();
	}

	// normal item
	if( !CItemBoard::UI_CheckBeforeAdd_CheckHoverObject() )
		return FALSE;

	// mining check
	if( !UI_CheckBeforeAdd_CheckMining() )
		return FALSE;
	
	return TRUE;

	/*
	// 트레이드시 체크 사항
	if( _GetTradeWindow()->IsShow() )
	{
		if( _UI_GetOrgBoardType() != pi_byBoradType )
		{
			_SetSystemMsg( "플레이어와 아이템을 트레이드 하는 중에는 아이템을 장착 / 해제할 수 없습니다." );
			return FALSE;
		}
		else
		{
			pi_byPackNo		= _UI_GetOrgPackNo();
			pi_bySocketNo	= _UI_GetOrgSocketNo();
		}
	}*/

	/*
	if( GetEmptySocketNum() <= 0 )
	{
		_SetSystemMsg( "현재는 채굴중이므로 인벤토리에 최소한 한칸은 남겨두어야 합니다." );
		return FALSE;
	}*/

	/*
	// 포스창에서 온 포스
	if( _UI_GetOrgBoardType() == IBT_FORCE )
	{
		if( !_GetHoverSF() )
			return FALSE;

		// 타겟 소켓 정보 저장
		_UI_SetOperationType( IOT_SET );
		_UI_SaveTarSocket( pi_byBoradType, pi_byPackNo, pi_bySocketNo );

		_GetForceBoard()->UI_MoveForceItem_Request( pi_byPackNo, -1 );
		return TRUE;
	}
	// 스킬/포스 벨트창에서 온 스킬/포스
	else if( _UI_GetOrgBoardType() == IBT_LINK )
		return FALSE;
	*/	
}

//------------------------------------------------------------------------------

BOOL
CInventoryBoard::UI_CheckBeforeAdd_CheckMining( void )
{	
	// 만약 채굴중이라면...
	if( _GetAvatar()->IsEqualAction( CAI_ITEM_USE_MINING ) )
	{
		if( GetEmptySocketNum() <= 0 )
		{
			_SetSystemMsg( "현재는 채굴중이므로 인벤토리에 최소한 한칸은 남겨두어야 합니다." );
			return FALSE;
		}
	}

	return TRUE;
}

//------------------------------------------------------------------------------

BOOL	
CInventoryBoard::UI_AddRequest_ProcessNonSplitableItem( void )
{
	if( _GetGBM()->GetOrgBoardType() == _GetGBM()->GetTarBoardType() )
	{
		UI_AddResult( TRUE );
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_EQUIP )
	{		
		m_pNetMsgProcessor_Item->OffPartRequest_clzo();
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_FORCE )
	{
		m_pNetMsgProcessor_Arrange->ForceInvenChangeRequest_clzo();		
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_ANIMUS )
	{
		m_pNetMsgProcessor_Arrange->AnimusInvenChangeRequest_clzo();
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_SF_BELT )
	{
		// orgboard를 force(or animus) board로 바꾼다
		CBoardObject * pHoverObject = _GetGBM()->GetHoverObject();
		BYTE byPackNo, bySocketNo;

		// force
		if( pHoverObject->IsEqualItemType( BOT_SF ) )
		{
			if( _GetForceBoard()->GetObjectPosition( byPackNo, bySocketNo, pHoverObject->GetIndex() ) )
			{
				_GetGBM()->SetOrgObject( IBT_FORCE, byPackNo, bySocketNo, _GetForceBoard()->GetObject( byPackNo, bySocketNo ) );
				m_pNetMsgProcessor_Arrange->ForceInvenChangeRequest_clzo();
			}

		}
		// animus
		else if( pHoverObject->IsEqualItemType( BOT_ANIMUS ) )
		{
			if( _GetAnimusBoard()->GetObjectPosition( byPackNo, bySocketNo, pHoverObject->GetIndex() ) )
			{
				_GetGBM()->SetOrgObject( IBT_ANIMUS, byPackNo, bySocketNo, _GetAnimusBoard()->GetObject( byPackNo, bySocketNo ) );
				m_pNetMsgProcessor_Arrange->AnimusInvenChangeRequest_clzo();
			}
		}
		else 
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------

void
CInventoryBoard::UI_AddResult_ProcessObject( void )
{
	// 포스 아이템
	if( _GetGBM()->GetOrgBoardType() == IBT_FORCE )
	{
		CBoardItem		clsTarItem;
		CBoardItem *	pHoverItem = _GetGBM()->GetHoverItem();

		// set force state
		CSFItem * pForceItem = _GetForceBoard()->GetSFByIndex( pHoverItem->GetIndex() );
		if( !pForceItem )
			return;
		pForceItem->SetState( CSFItem::SFS_NOT_EXIST );

		// force -> force item
		_GetGBM()->ConvertBoardObject( &clsTarItem, _GetGBM()->GetHoverObject() );
		
		Add( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo(), &clsTarItem );		
	}
	// 소환 아이템
	else if( _GetGBM()->GetOrgBoardType() == IBT_ANIMUS )
	{
		CBoardItem		clsTarItem;
		CBoardItem *	pHoverItem = _GetGBM()->GetHoverItem();

		// set force state
		CAnimusItem * pAnimusItem = _GetAnimusBoard()->GetAnimusByIndex( pHoverItem->GetIndex() );
		if( !pAnimusItem )
			return;
		pAnimusItem->Clear();

		// animus -> animus item
		_GetGBM()->ConvertBoardObject( &clsTarItem, _GetGBM()->GetHoverObject() );
		
		Add( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo(), &clsTarItem );
	}
	else
	{
		CItemBoard::UI_AddResult_ProcessObject();
	}

	PlayWave( SND_ID_ITEM_ADD_IN_PACK );
}

//==============================================================================

BOOL
CInventoryBoard::UI_CheckBeforeAdd_CheckEmptySocket_Trade( void )
{
	if( _GetGBM()->GetTarObject() )
		return FALSE;
	
	return FALSE;
}

BOOL
CInventoryBoard::UI_CheckBeforeRemove_CheckHoverObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeRemove_CheckHoverObject() )
		return FALSE;

	return TRUE;
}

BOOL
CInventoryBoard::UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity )
{
	if( !CItemBoard::UI_CheckBeforeRemove_CheckOriginalObject( pi_byQuantity ) )
		return FALSE;

	CBoardItem * pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );	

	//--------------------------------------------------------------------------
	// 현재 사용중인 유닛인지를 체크
//	if( ( GetMountedUnitPackNo() == pi_byPackNo ) &&
//		( GetMountedUnitSocketNo() == pi_bySocketNo ) )
//	{
//		_SetSystemMsg( "현재 탑승중인 유닛의 아이템은 이동할 수 없습니다." );
//		return FALSE;
//	}

	//--------------------------------------------------------------------------
	// 만약 채굴중이라면 unset할 아이템이 현재 사용중인 배터리인지를 체크
	if( _GetAvatar()->IsEqualAction( CAI_ITEM_USE_MINING ) )
	{
		if( pOrgItem->GetIndex() == _GetMiningBoard()->GetBatteryIndex() )
		{
			_SetSystemMsg( "현재 채굴에 사용중인 배터리 입니다. 채굴이 끝날때까지 기다려 주십시오." );
			return FALSE;
		}
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CInventoryBoard::UI_UseRequest_ProcessObject( void )
{
	CBoardItem * pOrgItem = static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );

	switch( pOrgItem->GetItemKind() )
	{
		case IEPT_POTION	:
			m_pNetMsgProcessor_Item->UsePotionRequest_clzo();
			break;			

		case IEPT_UNIT		:
			UI_UseRequest_ProcessObject_Unit();
			break;

		case IEPT_WORKTOOL	:
			{
				// 먼저 불필요한 모든 창을 닫는다.
				BOOL bIsOpendMiningWindow = _GetMiningWindow()->IsShow();

				_GetGBM()->CloseUnnecessaryWindow();

				if( bIsOpendMiningWindow )
					_GetMiningWindow()->Open();

				if( pOrgItem->GetDTIndex() < 3 )
				{
					_GetItemWorkBoard()->OpenItemWorkWindow( pOrgItem );
				}
				else if( pOrgItem->GetDTIndex() == 3 )
				{
					_GetItemUpgradeBoard()->OpenUpgradeWindow( pOrgItem->GetIndex() );										
				}
			}
			break;

		case IEPT_MAP		:
			{
				MAP_DATA * pMapData = ( MAP_DATA * )pOrgItem->GetData();
				if( pMapData )
					_GetLand()->OpenMinimapWindow( pMapData->GetMapName(), pMapData->GetName() );
			}
			break;

		case IEPT_RETURN	:
			{
				if( _GetAvatar()->IsEqualAction( CAI_ITEM_USE_MINING ) )
				{
					_SetSystemMsg( "채굴중에는 귀환 아이템을 사용할 수 없습니다." );
					return FALSE;
				}

				_GetNetworkMgr()->PositionMsg_GotoBaseportalRequest_clzo( pOrgItem->GetIndex() );
			}
			break;

		case IEPT_DUNGEON	:
			{
				if( _GetDungeonInfo()->IsInDungeon() )
				{
					_SetSystemMsg( "던전내에서는 사용이 불가능 합니다." );
					return FALSE;
				}

				CNetMsgProcessor_Dungeon * pNMP_Dungeon = static_cast< CNetMsgProcessor_Dungeon * >
														  ( _GetNetworkMgr()->GetNetMsgProcessor( NMP_DUNGEON ) );
				if( !pNMP_Dungeon )
					return FALSE;
				pNMP_Dungeon->BattledungeonOpenRequest_clzo( pOrgItem->GetIndex() );
			}
			break;

		case IEPT_GUARD_TOWER	:
			if( !UI_UseRequest_ProcessObject_GuardTower() )
				return FALSE;

		case IEPT_ETC			:	// ETC는 모두 bag임
			if( !UI_AddPackRequest() )
				return FALSE;
	}

	_GetAvatar()->SetUsingItem( pOrgItem );

	return TRUE;
}

BOOL
CInventoryBoard::UI_UseRequest_ProcessObject_Unit( void )
{
	if( !_GetAvatar()->IsEqualUnitMountState( UMS_DISMOUNT ) )
		return FALSE;

	CBoardItem*	pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	CUnitItem *	pUnitItem	= _GetUnitStorehouseBoard()->GetUnitByIndex( pOrgItem->GetIndex() );
	if( !pUnitItem )
		return FALSE;

	if( pUnitItem->GetDeliveryPrice( ID_INVALID ) > _GetAvatar()->GetDalant() )
	{
		_SetSystemMsg( "기갑 장비 출고를 위해서는 %d %s가 필요합니다.",
					   pUnitItem->GetDeliveryPrice( ID_INVALID ), _GetAvatarMoneyName() );
	}
	else
	{
		_GetUnitStorehouseBoard()->SetCurUnit( pOrgItem->GetIndex() );

		char pMessage[128];
		sprintf( pMessage, "기갑 장비 출고시 %d %s가 소모됩니다.\n출고 하시겠습니까?",
				 pUnitItem->GetDeliveryPrice( ID_INVALID ), _GetAvatarMoneyName() );
		_GUIMessageBox( NULL, pMessage, 0xb0ffffff, MSGBOX_ID_UNIT_DELIVERY, DLG_STYLE_OK_CANCEL );
	}

	return TRUE;
}

BOOL
CInventoryBoard::UI_UseRequest_ProcessObject_GuardTower( void )
{
	CBoardItem *		pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	GUARD_TOWER_DATA *	pTowerData	= static_cast< GUARD_TOWER_DATA * >( pOrgItem->GetData() );
	if( !pTowerData )
		return FALSE;

	// check skill
	CSFItem * pGuardTowerSkill = _GetSkillBoard()->GetGuardTowerSkill();
	if( !pGuardTowerSkill )
	{
		_SetSystemMsg( "가드 타워 설치는 해당 스킬을 가진 사람만이 사용할 수 있습니다." );
		return FALSE;
	}

	m_stGuardTowerInfo.m_dwSkillDTIndex		= pGuardTowerSkill->GetDTIndex();
	m_stGuardTowerInfo.m_dwGuardTowerIndex	= pOrgItem->GetIndex();

	// set material
	CBoardItem *	pMaterial = NULL;
	ITEM_DATA *		pMaterialData = NULL;
	BYTE			byPackNo = 0xFF, SocketNo = 0xFF;

	for( int i = 0; i < MAX_ITEM_MATERIAL_NUM; ++i )
	{
		if( pTowerData->byMaterialType[i] == 0xFF )
			continue;

		pMaterial = GetRemainOfQuantityItemByDTIndex( byPackNo, SocketNo,
													  pTowerData->byMaterialType[i],
													  pTowerData->dwMaterialDTIndex[i],
													  pTowerData->dwMaterialNum[i] );
		if( !pMaterial )
		{
			pMaterialData = static_cast< ITEM_DATA * >( _GetItemDataMgr()->GetData( pTowerData->byMaterialType[i], pTowerData->dwMaterialDTIndex[i] ) );
			_SetSystemMsg( "가드 타워 제작에 필요한 재료가 부족합니다. %s %d개가 필요합니다.",
						   pMaterialData->pName, pTowerData->dwMaterialNum[i] );
			return FALSE;
		}

		m_stGuardTowerInfo.m_dwMaterialIndex[i]	= pMaterial->GetIndex();
		m_stGuardTowerInfo.m_dwMaterialNum[i]	= pTowerData->dwMaterialNum[i];
	}

	// send message
	return _GetNetworkMgr()->EffectMsg_MakeTowerRequest_clzo( m_stGuardTowerInfo );
}

//==============================================================================

void
CInventoryBoard::UI_UseResult_ProcessObject( void )
{
	// 수량이 1개인 아이템은 사용후 제거될때 한번 호출되기 때문에 아이템을 다시 확인해야한다.
	CBoardItem * pOrgItem = static_cast< CBoardItem * >( _GetGBM()->GetObject( _GetGBM()->GetOrgBoardType(),
																			   _GetGBM()->GetOrgPackNo(),
																			   _GetGBM()->GetOrgSocketNo() ) );
	if( !pOrgItem )
		return;	

	switch( pOrgItem->GetItemKind() )
	{
		case IEPT_POTION		:
		case IEPT_RETURN		:
		case IEPT_DUNGEON		:
			_GetAvatar()->UseItem( pOrgItem );
			Remove( _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), 1 );
			break;

		case IEPT_UNIT			:
		case IEPT_WORKTOOL		:
			break;

		case IEPT_GUARD_TOWER	:
			UI_UseResult_ProcessObject_GuardTower();
			break;

		case IEPT_ETC			:
			UI_AddPackResult();
			break;
	}

	_GetAvatar()->RemoveUsingItem();
}

void
CInventoryBoard::UI_UseResult_ProcessObject_GuardTower( void )
{
	CBoardItem * pOrgItem = static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );

	pOrgItem->SetUsingState( TRUE );

	for( int i = 0; i < MAX_ITEM_MATERIAL_NUM; ++i )
		RemoveByIndex( m_stGuardTowerInfo.m_dwMaterialIndex[i], m_stGuardTowerInfo.m_dwMaterialNum[i] );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CInventoryBoard::UI_PutOnEquipment_By_HotKey( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	if( !_GetGBM()->IsEnableProcess() )
		return FALSE;

	if( _GetMainGame()->IsRequestLogout() )
		return FALSE;

	CBoardItem * pOrgItem = GetItem( pi_byPackNo, pi_bySocketNo );
	if( !pOrgItem )
		return FALSE;
	
	if( ( pOrgItem->GetItemKind() == IEPT_WEAPON ) ||
		( pOrgItem->GetItemKind() == IEPT_SHIELD ) )
	{
		if( !_GetAvatar()->IsEnableMove() )
		{
			_SetSystemMsg( "아바타가 전투중이거나 스킬/포스 등의 사용중에는 무기/방패를 장착/해제할 수 없습니다." );
			return FALSE;
		}
	}

	//--------------------------------------------------------------------------
	// remove item

	// 갑옷
	if( ( pOrgItem->GetItemKind() < IEPT_WEAPON ) || ( pOrgItem->GetItemKind() == IEPT_CLOAK ) )
	{
		if( !UI_RemoveRequest( pi_byPackNo, pi_bySocketNo, 1 ) )
			return FALSE;
	}
	// 무기 / 방패
	else if( pOrgItem->GetItemKind() == IEPT_WEAPON )
	{
		// 양손 무기
		if( pOrgItem->GetHoldType() == IHT_BOTH_HAND || pOrgItem->GetHoldType() == IHT_DUAL_HAND )
		{
			// 방패가 착용되어 있으면 먼저 unset한다.
			if( _GetEquipBoard()->GetEmptySocketByEquipPart( IEPT_SHIELD ) == 0xFF )
			{
				if( !_GetEquipBoard()->UI_PutOffEquipment_By_HotKey( CEPT_CLIENT_TO_UI[IEPT_SHIELD] ) )
					return FALSE;

				// 방패 해제 후 장착하기 위해 아이템의 위치를 저장한다
				_GetEquipBoard()->SetReservedEquipItem( pi_byPackNo, pi_bySocketNo );
				
				return TRUE;
			}
		}

		if( !UI_RemoveRequest( pi_byPackNo, pi_bySocketNo, 1 ) )
			return FALSE;
	}
	// 방패
	else if( pOrgItem->GetItemKind() == IEPT_SHIELD )
	{
		// 양손 무기가 착용되어 있으면 먼저 remove한다.
		CBoardItem * pWeapon = _GetEquipBoard()->GetItem( CEPT_CLIENT_TO_UI[IEPT_WEAPON] );
		if( pWeapon )
		{
			if( pWeapon->GetHoldType() == IHT_BOTH_HAND || pWeapon->GetHoldType() == IHT_DUAL_HAND )
			{
				if( !_GetEquipBoard()->UI_PutOffEquipment_By_HotKey( CEPT_CLIENT_TO_UI[IEPT_WEAPON] ) )
					return FALSE;

				_GetEquipBoard()->SetReservedEquipItem( pi_byPackNo, pi_bySocketNo );

				return TRUE;
			}
		}

		if( !UI_RemoveRequest( pi_byPackNo, pi_bySocketNo, 1 ) )
			return FALSE;
	}
	// 장신구
	else if( pOrgItem->GetItemKind() == IEPT_RING ||
			 pOrgItem->GetItemKind() == IEPT_AMULET ||
			 pOrgItem->GetItemKind() == IEPT_BULLET )
	{
		if( !UI_RemoveRequest( pi_byPackNo, pi_bySocketNo, 1 ) )
			return FALSE;
	}
	// 기타 
	else
	{
		return FALSE;
	}

	//--------------------------------------------------------------------------

	// add item
	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	BYTE byEquipSocketNo;

	// 장신구일 경우 두 소켓중 빈소켓에 넣는다.
	if( pHoverItem->GetItemKind() == IEPT_RING ||
		pHoverItem->GetItemKind() == IEPT_AMULET ||
		pHoverItem->GetItemKind() == IEPT_BULLET )
	{
		if( ( _GetEquipBoard()->GetItem( CEPT_CLIENT_TO_UI[ pHoverItem->GetItemKind() ] ) ) &&
			( !_GetEquipBoard()->GetItem( CEPT_CLIENT_TO_UI[ pHoverItem->GetItemKind() ] + 1 ) ) )
			byEquipSocketNo = CEPT_CLIENT_TO_UI[ pHoverItem->GetItemKind() ] + 1;
		else
			byEquipSocketNo = CEPT_CLIENT_TO_UI[ pHoverItem->GetItemKind() ];
	}
	else
	{
		byEquipSocketNo = CEPT_CLIENT_TO_UI[ pHoverItem->GetItemKind() ];
	}
	
	if( !_GetEquipBoard()->UI_AddRequest( 0, byEquipSocketNo ) )
			return FALSE;

	return TRUE;
}

BOOL
CInventoryBoard::UI_Trade_By_HotKey( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	if( _GetOreProcBoard()->IsTrading() )
	{
		CBoardItem * pOrgItem = GetItem( pi_byPackNo, pi_bySocketNo );
		if( pOrgItem )
			_GetOreProcBoard()->UI_SetOre( pi_byPackNo, pi_bySocketNo, pOrgItem->GetQuantity(), TRUE );

		return TRUE;
	}
	else if( _GetItemUpgradeBoard()->IsOpen() )
	{
		_GetItemUpgradeBoard()->SelectUpgradeItem( m_byBoardType, pi_byPackNo, pi_bySocketNo, TRUE );

		return TRUE;
	}

	return CItemBoard::UI_Trade_By_HotKey( pi_byPackNo, pi_bySocketNo );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CInventoryBoard::UI_AutoPickUpItemRequest( void )
{
	return UI_PickUpItemRequest( _GetGroundItemMgr()->GetLootableItem() );
}

BOOL
CInventoryBoard::UI_PickUpItemRequest( CGroundItem * pi_pGroundItem )
{
	if( !pi_pGroundItem )
		return FALSE;
	if( pi_pGroundItem->IsDisable( ROSF_RENDER ) )
		return FALSE;

	if( !UI_PickUpItemRequest_CheckBoard() )
		return FALSE;

	if( !_GetAvatar()->ProcessGroundItemForPickUp( pi_pGroundItem ) )
		return FALSE;

	//--------------------------------------------------------------------------

	BOOL		bIsAddToNewPosition = TRUE;
	CBoardItem* pExistItem = NULL;
	DWORD		dwExistItemIndex = ID_INVALID;

	// 인벤토리에 해당 아이템이 있는지 찾는다.
	if( pi_pGroundItem->IsSplitable() )
	{
		m_byLootingPackNo = 0xFF;
		pExistItem = GetLackOfQuantityItemByDTIndex( m_byLootingPackNo, m_byLootingSocketNo,
													 pi_pGroundItem->GetItemKind(),
													 pi_pGroundItem->GetDTIndex(),
													 pi_pGroundItem->GetQuantity(),
													 TRUE );
		if( pExistItem )
			dwExistItemIndex = pExistItem->GetIndex();
	}

	// 빈 공간을 찾는다.
	if( !pExistItem )
	{
		if( !FindEmptySocket_IncludeOtherBoard( m_byLootingPackNo, m_byLootingSocketNo ) )
		{
			_SetSystemMsg( "인벤토리에 빈 공간이 없습니다." );
			return FALSE;
		}
	}

	//--------------------------------------------------------------------------

	_GetTargetMgr()->SetTargetItem( pi_pGroundItem );
	m_pNetMsgProcessor_Item->ItemboxTakeRequest_clzo( pi_pGroundItem->GetIndex(), dwExistItemIndex );

	return TRUE;
}

BOOL
CInventoryBoard::UI_PickUpItemRequest_CheckBoard( void )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;
	if( _GetGBM()->HaveHoverObject() )
		return FALSE;

	if( _GetPlayerTradeBoard()->IsTrading() )
	{
		_SetSystemMsg( "플레이어와 트레이드 중에는 아이템을 루팅할 수 없습니다." );
		return FALSE;
	}

	return TRUE;
}

//==============================================================================

BOOL
CInventoryBoard::UI_PickUpItemResult_CreateItem( CBoardItem * pi_pLootingItem )
{
	if( !pi_pLootingItem )
		return FALSE;

	BYTE byQuantity = pi_pLootingItem->GetQuantity();

	Add( m_byLootingPackNo, m_byLootingSocketNo, pi_pLootingItem );

	PlayWave( SND_ID_ITEM_ADD_IN_PACK );

	_SetSystemMsg( "%s %d개를 획득하였습니다. ( %d번째 가방 %d번째 칸에 삽입 되었습니다. )",
				   pi_pLootingItem->GetName(), byQuantity, m_byLootingPackNo + 1, m_byLootingSocketNo + 1);

	return TRUE;
}

BOOL
CInventoryBoard::UI_PickUpItemResult_AddQuantity( DWORD pi_dwItemIndex, DWORD pi_dwQuantity )
{
	CBoardItem * pBoardItem = static_cast< CBoardItem * >( GetObject( m_byLootingPackNo, m_byLootingSocketNo ) );
	if( !pBoardItem )
		return FALSE;

	BYTE byLootingNum = pi_dwQuantity - pBoardItem->GetQuantity();
	pBoardItem->SetQuantity( pi_dwQuantity );

	PlayWave( SND_ID_ITEM_ADD_IN_PACK );

	_SetSystemMsg( "%s %d개를 획득하였습니다. ( %d번째 가방 %d번째 칸에 합쳐졌습니다. )",
				   pBoardItem->GetName(), byLootingNum, m_byLootingPackNo + 1, m_byLootingSocketNo + 1 );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 맵이름으로 맵 아이템 찾기
BOOL
CInventoryBoard::FindMapItemPosition( char * pi_pMapName, BYTE & po_byPackNo, BYTE & po_bySocketNo )
{
	CBoardItem * pItem;
	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[i].GetMaxSocketNum(); ++j )
		{
			pItem = GetItem( i, j );
			if( !pItem )
				continue;

			if( pItem->IsEqualItemKind( IEPT_MAP ) )
			{
				MAP_DATA * pMapData = ( MAP_DATA * )pItem->GetData();
				if( stricmp( pMapData->GetMapName(), pi_pMapName ) == 0 )
				{
					po_byPackNo = i;
					po_bySocketNo = j;
				
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
BOOL
CInventoryBoard::FindEmptySocket_IncludeShopTradeList( BYTE & po_byPackNo, BYTE & po_bySocketNo )
{
	char l_byPackNo		= -1;
	char l_bySocketNo	= -1;
	
	l_byPackNo = m_uiInventoryWindow.GetLastOpenedPack();

	// current pack
	l_bySocketNo = CItemBoard::FindEmptySocket_IncludeShopTradeList( l_byPackNo );

	// other pack
	if( l_bySocketNo == -1 )
	{
		for( int i = 0; i < MAX_INVENTORY_PACK; ++i )
		{
			l_byPackNo = i;

			if( m_listItemPack[i].IsExist() && ( i != m_uiInventoryWindow.GetLastOpenedPack() ) )
			{
				l_bySocketNo = CItemBoard::FindEmptySocket_IncludeShopTradeList( l_byPackNo );
				if( l_bySocketNo != ID_INVALID )
					break;
			}
		}
	}

	// if founded
	if( l_bySocketNo != ID_INVALID )
	{
		po_byPackNo		= l_byPackNo;
		po_bySocketNo	= l_bySocketNo;

		return TRUE;
	}

	return FALSE;
}
*/

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
BOOL
CInventoryBoard::IsOpen( BYTE pi_byPackNo )
{
	if( pi_byPackNo >= m_byMaxPackNum )
		return FALSE;

	CGUIInventoryBoard * pPackWindow = m_uiInventoryWindow.GetPackWindow( pi_byPackNo );
	if( !pPackWin )
		return FALSE;

	return pPackWindow->IsShow();
}
*/

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

LRESULT
CInventoryBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UI_MOUSE_MSG	stRecvMsg( lParam );

	switch( uMsg )
	{
		case WM_BOARD_OPEN_PACK		:
			if( ( stRecvMsg.m_byPackNo < m_byMaxPackNum ) && ( m_uiInventoryWindow.IsOpenedPack( stRecvMsg.m_byPackNo ) ) )
				m_byActivePackNo = stRecvMsg.m_byPackNo;
			break;

		case WM_BOARD_EQUIP_HOT_KEY	:
			UI_PutOnEquipment_By_HotKey( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
			break;
	}

	return CItemBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // _NEW_UI_