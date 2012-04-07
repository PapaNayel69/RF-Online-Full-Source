////////////////////////////////////////////////////////////////////////////////
//
// CItemWorkBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../Manager/CCharacterMgr.h"
#include "../Common/CBoardManager.h"

#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_Item.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CItemWorkBoard::CItemWorkBoard()
{
	m_dwWorkToolItemIndex	= ID_INVALID;	

	m_byWorkToolType		= 0xFF;

	m_dwWorkExp	= ID_INVALID;

	m_bIsSet[0] = FALSE;
	m_bIsSet[1] = FALSE;
	m_bIsSet[2] = FALSE;			

	ClearWork();
}

CItemWorkBoard::~CItemWorkBoard()
{

}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CItemWorkBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiItemWorkWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );

	m_uiWindow->SetPos( _GetUIScreen()->GetSizeX() - m_uiWindow->GetSizeX() * 2 - 15, _GetUIScreen()->GetPosY() );

	m_clsItemListBoard[WTT_WEAPON].SetWindow( m_uiItemWorkWindow.GetItemListWindow( WTT_WEAPON ) );
	m_clsItemListBoard[WTT_ARMOR].SetWindow( m_uiItemWorkWindow.GetItemListWindow( WTT_ARMOR ) );
	m_clsItemListBoard[WTT_BULLET].SetWindow( m_uiItemWorkWindow.GetItemListWindow( WTT_BULLET ) );	
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL		
CItemWorkBoard::OpenItemWorkWindow( CBoardItem * pi_pWorkTool )
{
	if( !pi_pWorkTool )
		return FALSE;

	if( IsOpen() )
		return FALSE;

	m_uiItemWorkWindow.SetMakeGroup( ( CGUIItemMakeWindow::MakeGroup ) pi_pWorkTool->GetDTIndex() );
	m_uiItemWorkWindow.SetActiveTab( 0 );
	

	DWORD dwWorkToolDTIndex = pi_pWorkTool->GetDTIndex();	
	DWORD dwSkillExp	= ( DWORD )( sqrt( ( (float)_GetAvatar()->GetSuccessCountOfExpertness( dwWorkToolDTIndex + EVT_MAKE_WEAPON ) / 1.1f ) * 3.0f ) );
	if( dwSkillExp < 1 )
		dwSkillExp = 1;
	if( dwSkillExp > 99 )
		dwSkillExp = 99;	

	//--------------------------------------------------------------------------

	if( ( m_dwWorkExp != dwSkillExp ) || ( !m_bIsSet[dwWorkToolDTIndex] ) )
	{
		m_dwWorkExp = dwSkillExp;
		ClearWorkItemList( dwWorkToolDTIndex );

		if( dwWorkToolDTIndex == WTT_WEAPON )
			SettingItem_Weapon();
		else if( dwWorkToolDTIndex == WTT_ARMOR )
			SettingItem_Armor();
		else if( dwWorkToolDTIndex == WTT_BULLET )
			SettingItem_Bullet();
		else
			return FALSE;
	}

	//--------------------------------------------------------------------------
	
	m_dwWorkToolItemIndex = pi_pWorkTool->GetIndex();

	m_byWorkToolType = dwWorkToolDTIndex;


	OpenWindow();

	return TRUE;
}

void
CItemWorkBoard::ClearWork( void )
{
	m_clsNewItem.SetEmpty();
	m_byNewItemPackNo		= ID_INVALID;
	m_byNewItemSocketNo		= ID_INVALID;

	for( int i = 0; i < MAX_ITEM_MATERIAL_NUM; ++i )
	{
		m_dwMaterialIndex[i]	= ID_INVALID;
		m_byMaterialPackNo[i]	= 0xFF;
		m_byMaterialSocketNo[i]	= 0xFF;
	}

	DeselectWorkItem();	
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CItemWorkBoard::ClearWorkItemList( BYTE pi_byItemWorkType )
{
	if( pi_byItemWorkType == 0xFF )
	{
		for( int i = 0; i < WTT_NUM; ++i )
		{
			m_clsItemListBoard[i].Clear();
			m_bIsSet[i] = FALSE;
		}
	}
	else
	{		
		if( pi_byItemWorkType >= WTT_NUM )
			return;

		m_clsItemListBoard[pi_byItemWorkType].Clear();
		m_bIsSet[pi_byItemWorkType] = FALSE;
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CItemWorkBoard::SettingItem_Weapon( void )
{
	static const int MAX_WEAPON_TYPE = 28;
	static const BYTE WEAPON_TYPE_TO_ITEM_WORKING_UI[MAX_WEAPON_TYPE] =
	{
		1, 1, 1, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
	};

	ITEM_WORK_DATA *	pItemWorkData;
	DWORD				i, cnt[8], dwMaxNum;
	BYTE				byItemType;
	CBoardItem			clsItem;

	// create booard
	if( !m_clsItemListBoard[WTT_WEAPON].GetMaxPackNum() )
	{
		m_clsItemListBoard[WTT_WEAPON].Create( IBT_ITEM_WORK, 9, MAX_ITEM_WORK_WINDOW_SLOT );	
		for( int i = 0; i < 9; ++i )
			m_clsItemListBoard[WTT_WEAPON].AddPack( i );
	}

	//--------------------------------------------------------------------------
	// 무기
	i			= _GetItemDataMgr()->GetItemWorkDataStartDTIndex( IEPT_WEAPON );
	dwMaxNum	= i + _GetItemDataMgr()->GetItemWorkDataNum( IEPT_WEAPON );
	ZeroMemory( cnt, sizeof( DWORD ) * 8 );

	for( ; i < dwMaxNum; ++i )
	{
		// check
		pItemWorkData = ( ITEM_WORK_DATA * )_GetItemDataMgr()->GetData( IEPT_ITEM_WORK, i );
		if( !pItemWorkData )
			continue;

		if( !pItemWorkData->IsUsableRace( _GetAvatar()->GetRace() ) )
			continue;

		//if( ( m_dwWorkLevel + 4 ) < l_pItemWorkData->GetWorkableExp() )
		if( m_dwWorkExp < pItemWorkData->GetWorkableExp() )
			continue;

		if( ( pItemWorkData->GetItemMeshID() == ID_INVALID ) ||
			( pItemWorkData->GetItemIconID() == ID_INVALID ) )
			continue;

		// set data
		if( !_GetItemDataMgr()->GetData( pItemWorkData->GetItemType(), pItemWorkData->GetItemDTIndex(),
										 clsItem, FALSE, 0 ) )
			continue;

		byItemType = ( pItemWorkData->GetItemMeshID() & 0x0000FF00 ) >> 8;
		if( byItemType >= MAX_WEAPON_TYPE )
			continue;

		AddItem( WTT_WEAPON, WEAPON_TYPE_TO_ITEM_WORKING_UI[byItemType], cnt[ WEAPON_TYPE_TO_ITEM_WORKING_UI[byItemType] ], &clsItem );

		++cnt[ WEAPON_TYPE_TO_ITEM_WORKING_UI[byItemType] ];
	}

	//--------------------------------------------------------------------------
	// 방패
	i			= _GetItemDataMgr()->GetItemWorkDataStartDTIndex( IEPT_SHIELD );
	dwMaxNum	= i + _GetItemDataMgr()->GetItemWorkDataNum( IEPT_SHIELD );
	ZeroMemory( cnt, sizeof( DWORD ) * 8 );

	for( ; i < dwMaxNum; ++i )
	{
		// check
		pItemWorkData = ( ITEM_WORK_DATA * )_GetItemDataMgr()->GetData( IEPT_ITEM_WORK, i );
		if( !pItemWorkData )
			continue;

		if( !pItemWorkData->IsUsableRace( _GetAvatar()->GetRace() ) )
			continue;

		//if( ( m_dwWorkLevel + 4 ) < l_pItemWorkData->GetWorkableExp() )
		if( m_dwWorkExp < pItemWorkData->GetWorkableExp() )
			continue;

		if( ( pItemWorkData->GetItemMeshID() == ID_INVALID ) ||
			( pItemWorkData->GetItemIconID() == ID_INVALID ) )
			continue;

		// set data
		if( !_GetItemDataMgr()->GetData( pItemWorkData->GetItemType(), pItemWorkData->GetItemDTIndex(),
										 clsItem, FALSE, 0 ) )
			continue;

		AddItem( WTT_WEAPON, 8, cnt[0], &clsItem );
		
		++cnt[0];
	}

	m_bIsSet[WTT_WEAPON] = TRUE;

	return TRUE;
}

BOOL
CItemWorkBoard::SettingItem_Armor( void )
{
	static const BYTE ARMOR_TYPE_TO_ITEM_WORKING_UI[5] =
	{
		IEPT_HELMET, IEPT_UPPER_PART, IEPT_LOWER_PART, IEPT_GLOVES, IEPT_SHOES
	};

	ITEM_WORK_DATA *	pItemWorkData;
	DWORD				i, cnt[8], dwMaxNum;
	CBoardItem			clsItem;

	// create booard
	if( !m_clsItemListBoard[WTT_ARMOR].GetMaxPackNum() )
	{
		m_clsItemListBoard[WTT_ARMOR].Create( IBT_ITEM_WORK, 5, MAX_ITEM_WORK_WINDOW_SLOT );
		for( int i = 0; i < 5; ++i )
			m_clsItemListBoard[WTT_ARMOR].AddPack( i );
	}

	//--------------------------------------------------------------------------
	// 방어구 제작
	for( int j = 0; j < 5; ++j )
	{
		i			= _GetItemDataMgr()->GetItemWorkDataStartDTIndex( ARMOR_TYPE_TO_ITEM_WORKING_UI[j] );
		dwMaxNum	= i + _GetItemDataMgr()->GetItemWorkDataNum( ARMOR_TYPE_TO_ITEM_WORKING_UI[j] );
		ZeroMemory( cnt, sizeof( DWORD ) * 8 );

		for( ; i < dwMaxNum; ++i )
		{
			// check
			pItemWorkData = ( ITEM_WORK_DATA * )_GetItemDataMgr()->GetData( IEPT_ITEM_WORK, i );
			if( !pItemWorkData )
				continue;

			if( !pItemWorkData->IsUsableRace( _GetAvatar()->GetRace() ) )
				continue;

			//if( ( m_dwWorkLevel + 4 ) < l_pItemWorkData->GetWorkableExp() )
			if( m_dwWorkExp < pItemWorkData->GetWorkableExp() )
				continue;

			if( ( pItemWorkData->GetItemMeshID() == ID_INVALID ) ||
				( pItemWorkData->GetItemIconID() == ID_INVALID ) )
				continue;

			// set data
			if( !_GetItemDataMgr()->GetData( pItemWorkData->GetItemType(), pItemWorkData->GetItemDTIndex(),
											 clsItem, FALSE, 0 ) )
				continue;

			AddItem( WTT_ARMOR, j, cnt[j], &clsItem );
			
			++cnt[j];
		}
	}

	m_bIsSet[WTT_ARMOR] = TRUE;

	return TRUE;
}

BOOL
CItemWorkBoard::SettingItem_Bullet( void )
{
	static const BYTE BULLET_TYPE_TO_ITEM_WORKING_UI[2] =
	{
		0, 1
	};

	ITEM_WORK_DATA *	pItemWorkData;
	DWORD				i, cnt[8], dwMaxNum;
	BYTE				byItemType;
	CBoardItem			clsItem;

	// create board
	if( !m_clsItemListBoard[WTT_BULLET].GetMaxPackNum() )
	{
		m_clsItemListBoard[WTT_BULLET].Create( IBT_ITEM_WORK, 2, MAX_ITEM_WORK_WINDOW_SLOT );
		m_clsItemListBoard[WTT_BULLET].AddPack( 0 );
		m_clsItemListBoard[WTT_BULLET].AddPack( 1 );
	}


	//--------------------------------------------------------------------------
	// 탄환 제작
	i			= _GetItemDataMgr()->GetItemWorkDataStartDTIndex( IEPT_BULLET );
	dwMaxNum	= i + _GetItemDataMgr()->GetItemWorkDataNum( IEPT_BULLET );
	ZeroMemory( cnt, sizeof( DWORD ) * 8 );

	for( ; i < dwMaxNum; ++i )
	{
		// check
		pItemWorkData = ( ITEM_WORK_DATA * )_GetItemDataMgr()->GetData( IEPT_ITEM_WORK, i );
		if( !pItemWorkData )
			continue;

		if( !pItemWorkData->IsUsableRace( _GetAvatar()->GetRace() ) )
			continue;

		//if( ( m_dwWorkLevel + 4 ) < l_pItemWorkData->GetWorkableExp() )
		if( m_dwWorkExp < pItemWorkData->GetWorkableExp() )
			continue;

		if( ( pItemWorkData->GetItemMeshID() == ID_INVALID ) ||
			( pItemWorkData->GetItemIconID() == ID_INVALID ) )
			continue;
		// set data
		if( !_GetItemDataMgr()->GetData( pItemWorkData->GetItemType(), pItemWorkData->GetItemDTIndex(),
										 clsItem, FALSE, 0 ) )
			continue;

		byItemType = ( pItemWorkData->GetItemMeshID() & 0x0000FF00 ) >> 8;

		AddItem( WTT_BULLET, BULLET_TYPE_TO_ITEM_WORKING_UI[byItemType-3], cnt[ BULLET_TYPE_TO_ITEM_WORKING_UI[byItemType-3] ], &clsItem );

		++cnt[ BULLET_TYPE_TO_ITEM_WORKING_UI[byItemType-3] ];
	}

	m_bIsSet[WTT_BULLET] = TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CItemWorkBoard::AddItem( BYTE pi_byItemWorkType, BYTE pi_byTabNo, BYTE pi_bySocketNo, CBoardItem * pi_pItem )
{
	if( pi_byItemWorkType >= WTT_NUM )
		return FALSE;

	// 제작 아이템 툴팁에 업그레이드정보를 안보여주기 위해
	pi_pItem->SetMaxSocketNum( 0 );

	return m_clsItemListBoard[pi_byItemWorkType].Add( pi_byTabNo, pi_bySocketNo, pi_pItem );
}

BOOL
CItemWorkBoard::RemoveItem( BYTE pi_byItemWorkType, BYTE pi_byTabNo, BYTE pi_bySocketNo, BYTE pi_byQuantity )
{
	if( pi_byItemWorkType >= WTT_NUM )
		return FALSE;

	return m_clsItemListBoard[pi_byItemWorkType].Remove( pi_byTabNo, pi_bySocketNo, pi_byQuantity );
}

CBoardItem	*
CItemWorkBoard::GetItem( BYTE pi_byItemWorkType, BYTE pi_byTabNo, BYTE pi_bySocketNo )
{
	if( pi_byItemWorkType >= WTT_NUM )
		return NULL;

	return m_clsItemListBoard[pi_byItemWorkType].GetItem( pi_byTabNo, pi_bySocketNo );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CItemWorkBoard::SelectWorkItem( BYTE pi_byTabNo, BYTE pi_bySocketNo )
{
	CBoardItem * pItem = GetItem( m_byWorkToolType, pi_byTabNo, pi_bySocketNo );
	if( !pItem )
		return;

	ITEM_WORK_DATA * pItemWorkData;
	pItemWorkData = ( ITEM_WORK_DATA * )_GetItemDataMgr()->GetItemWorkDataByMeshID( pItem->GetMeshID() );
	if( !pItemWorkData )
		return;

	if( m_pItemWorkData == pItemWorkData )
		return;

	if( m_pItemWorkData )
		DeselectWorkItem();
	
	// set work item data
	m_pItemWorkData = pItemWorkData;

	MaterialSetting();
	
	// set ui
	m_uiItemWorkWindow.SelectItem( pi_bySocketNo );
}

void
CItemWorkBoard::DeselectWorkItem( void )
{
	m_pItemWorkData	= NULL;	

	m_uiItemWorkWindow.DeselectItem();
	m_uiItemWorkWindow.ClearMaterial();
	m_uiItemWorkWindow.EnableMake( FALSE );
}

// ========================================================================================
// 제작에 필요한 자원 설정
BOOL				
CItemWorkBoard::MaterialSetting( void )
{
	if( !m_pItemWorkData )
		return FALSE;
	
	ITEM_DATA *		pItemData;
	BOOL			bExistMaterial = TRUE;
	CBoardItem *	pMaterial;
	for( int i = 0, cnt = 0; i < MAX_ITEM_MATERIAL_NUM; ++i )
	{
		pItemData = ( ITEM_DATA * )_GetItemDataMgr()->GetData( m_pItemWorkData->GetMaterialType( i ),
															   m_pItemWorkData->GetMaterialDTIndex( i ) );
		if( !pItemData )
			continue;
		
		if( !m_pItemWorkData->GetMaterialNum( i ) )
			continue;

		// 아이템 제작에 필요한 오어 세팅
		m_byMaterialPackNo[i] = 0xFF;
		pMaterial = _GetInventoryBoard()->GetRemainOfQuantityItemByDTIndex( m_byMaterialPackNo[i],
																			m_byMaterialSocketNo[i],
																			m_pItemWorkData->GetMaterialType( i ),
																			m_pItemWorkData->GetMaterialDTIndex( i ),
																			m_pItemWorkData->GetMaterialNum( i ) );
		if( pMaterial )
		{
			m_dwMaterialIndex[cnt] = pMaterial->GetIndex();
			++cnt;
		}

		// set ui
		m_uiItemWorkWindow.SetMaterialStr( i, pItemData->GetName(), m_pItemWorkData->GetMaterialNum( i ), (BOOL)pMaterial );

		if( bExistMaterial && !pMaterial )
			bExistMaterial = FALSE;
	}

	// button enable
	m_uiItemWorkWindow.EnableMake( bExistMaterial );

	return bExistMaterial;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아이템 제작 요청
BOOL
CItemWorkBoard::ItemWork_Request( void )
{
	if( !m_pItemWorkData )
		return FALSE;

	// 아이템의 빈 공간 체크
	if( !_GetInventoryBoard()->FindEmptySocket( m_byNewItemPackNo, m_byNewItemSocketNo ) )
	{
		_SetSystemMsg( "인벤토리에 빈 공간이 없습니다." );
		return FALSE;
	}
	
	// 제작에 필요한 재료 세팅
	if( !MaterialSetting() )
		_SetSystemMsg( "아이템을 제작하기 위해 필요한 자원이 없거나 수량이 부족합니다." );

	m_clsNewItem.SetEmpty();

	m_pNetMsgProcessor_Item->MakeItemRequest_clzo();	

	return TRUE;
}

// 아이템 제작 요청에 대한 응답
BOOL
CItemWorkBoard::ItemWork_Result( BOOL pi_bIsSuccessed )
{	
	// 기존 아이템 설정
	// 이 함수에서는 기존의 자원등만 설정된다.
	for( int i = 0; i < m_pItemWorkData->GetTotalMaterialNum(); ++i )
	{
		_GetInventoryBoard()->Remove( m_byMaterialPackNo[i], m_byMaterialSocketNo[i],
									  m_pItemWorkData->GetMaterialNum( i ) );
	}

	if( pi_bIsSuccessed )
	{		
		// 제작 성공으로 생성된 아이템은 _insert_item_inform_zocl 프로토콜에서 처리됨

		if( m_clsNewItem.IsEmpty() )
			return FALSE;

		BYTE l_byPackNo, l_bySocketNo;
		if( !_GetInventoryBoard()->FindEmptySocket( l_byPackNo, l_bySocketNo ) )
		{
			_SetSystemMsg( "인벤토리에 빈 공간이 없습니다." );
			return FALSE;
		}

		_GetInventoryBoard()->Add( l_byPackNo, l_bySocketNo, &m_clsNewItem );

		_SetSystemMsg( "%s 1개를 제작하였습니다. ( %d번째 가방 %d번째 칸 )", m_clsNewItem.GetName(),
						l_byPackNo + 1, l_bySocketNo + 1 );

		ClearWork();

		PlayWave( SND_ID_SUCC_MAKE );

		return TRUE;
	}
	else
	{
		_SetSystemMsg( "아이템 제작에 실패 하였습니다." );

		MaterialSetting();

		PlayWave( SND_ID_FAIL_MAKE );

		return FALSE;
	}	
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

LRESULT
CItemWorkBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UI_MOUSE_MSG	stRecvMsg( lParam );

	switch( uMsg )
	{
		case WM_BOARD_CLICK_OBJECT		:
			SelectWorkItem( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
			break;

		case WM_ITEM_WORK_BOARD_PROCESS	:
			ItemWork_Request();
			break;

		case WM_ITEM_WORK_BOARD_CANCEL	:
			DeselectWorkItem();
			break;

		case WM_BOARD_HOVER_OBJECT		:
			m_clsItemListBoard[m_byWorkToolType].MsgProc( hWnd, uMsg, wParam, lParam );
			break;

	}

	return CObjectBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}