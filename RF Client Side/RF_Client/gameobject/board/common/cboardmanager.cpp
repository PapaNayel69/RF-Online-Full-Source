////////////////////////////////////////////////////////////////////////////////
//
// CBoardManager Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../../Character/Avatar/CAvatar.h"
#include "../../../DataTable/CItemDataMgr.h"
#include "../../Manager/CCharacterMgr.h"

#include "../../manager/CTargetMgr.h"

#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_Item.h"

#include "../../../UserInterface/GameWindow/Npc/GUINpcSelectWindow.h"
#include "../../../UserInterface/GameWindow/EtcWin/GUIMapMoveWindow.h"
#include "../../../UserInterface/GameWindow/EtcWin/GUIMiniMapWindow.h"
#include "../../../UserInterface/GameWindow/GUIAttackPartWindow.h"

#include "../../../GameProgress/CGP_MainGame.h"

#include "CBoardManager.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CBoardManager g_clsBoardManager;

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CBoardManager::CBoardManager()
{
	ZeroMemory( this, sizeof( CBoardManager ) );

	m_byOrgBoardType		= ID_INVALID;
	m_byTarBoardType		= ID_INVALID;
	
	m_pDeletedHoverItem = new CBoardItem;
}

CBoardManager::~CBoardManager()
{
	for( int i = 0; i < MAX_BOARD_TYPE; ++i )
		_DELETE( m_listBoard[i] );

	_DELETE( m_pHoverObject );
	_DELETE( m_pDeletedHoverItem );
}

void
CBoardManager::Create( void )
{
	m_listBoard[IBT_INVENTORY]			= dynamic_cast< CBaseBoard * >( new CInventoryBoard );
	m_listBoard[IBT_ANIMUS]				= dynamic_cast< CBaseBoard * >( new CAnimusBoard );
	m_listBoard[IBT_ANIMUS_CONTROL]		= dynamic_cast< CBaseBoard * >( new CAnimusControlBoard );
	m_listBoard[IBT_LINK]				= dynamic_cast< CBaseBoard * >( new CLinkBoard );
	m_listBoard[IBT_UNIT_TUNING]		= dynamic_cast< CBaseBoard * >( new CUnitTuningBoard );
	m_listBoard[IBT_UNIT_STOREHOUSE]	= dynamic_cast< CBaseBoard * >( new CUnitStorehouseBoard );
	m_listBoard[IBT_ITEM_BELT]			= dynamic_cast< CBaseBoard * >( new CBeltBoard );
	m_listBoard[IBT_EQUIP]				= dynamic_cast< CBaseBoard * >( new CEquipBoard );
	m_listBoard[IBT_SKILL]				= dynamic_cast< CBaseBoard * >( new CSkillBoard );
	m_listBoard[IBT_FORCE]				= dynamic_cast< CBaseBoard * >( new CForceBoard );
	m_listBoard[IBT_SHOP]				= dynamic_cast< CBaseBoard * >( new CShopTradeBoard );	
	m_listBoard[IBT_PLAYER_TRADE]		= dynamic_cast< CBaseBoard * >( new CPlayerTradeBoard );	
	m_listBoard[IBT_ORE_PROCESSING]		= dynamic_cast< CBaseBoard * >( new COreProcBoard );	
	m_listBoard[IBT_ITEM_WORK]			= dynamic_cast< CBaseBoard * >( new CItemWorkBoard );
	m_listBoard[IBT_ITEM_UPGRADE]		= dynamic_cast< CBaseBoard * >( new CItemUpgradeBoard );

	m_listBoard[IBT_EXCHANGE]			= dynamic_cast< CBaseBoard * >( new CExchangeBoard );
	m_listBoard[IBT_MINING]				= dynamic_cast< CBaseBoard * >( new CMiningBoard );

	m_listBoard[IBT_CLASS_CHANGE]		= dynamic_cast< CBaseBoard * >( new CClassChangeBoard );
}

void
CBoardManager::FrameMove( void )
{
	for( int i = 0; i < MAX_BOARD_TYPE; ++i )
	{
		if( m_listBoard[i] )
			m_listBoard[i]->FrameMove();
	}

	// 아이템 이동을 요청한지 일정시간이 지나도 응답이 오지 않으면 플래그를 FALSE로 세팅한다.
	if( m_bIsRequestProcess )
	{
		const DWORD REQUEST_FOR_ITEM_MOVE_LIMIT_TIME = 10000;
		if( timeGetTime() - m_bIsRequestProcessTime >= REQUEST_FOR_ITEM_MOVE_LIMIT_TIME  )
		{
			m_bIsRequestProcess = FALSE;
		}
	}
}

LRESULT
CBoardManager::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_BOARD_DROP_OBJECT		:
			return PutDownItemRequest();

		case WM_PLAYER_TRADE_REQUEST	:
			return m_listBoard[IBT_PLAYER_TRADE]->MsgProc( hWnd, uMsg, wParam, lParam );

		case WM_BOARD_OPEN_SHOP_WINDOW	:
			{
				CNpc * pMerchant = ( CNpc * )_GetCharMgr()->GetCharacter( CTI_NPC,
																		  _GetAvatar()->GetNpcTradeInfo()->m_dwCurrentIntroduceNpc );
				if( pMerchant )
				{
					_GetNpcIntroWindow()->Close();

					pMerchant->OpenShopWindow( lParam );
				}
			}
			return 1;			

		case WM_OPEN_WINDOW				:
			// 거래할 때 인벤토리를 열어준다
			if( wParam == UII_SHOP_WINDOW || 
				wParam == UII_TRADE_WINDOW ||	
				wParam == UII_ORE_PROC_WINDOW ||
				wParam == UII_EXCHANGE_WINDOW ||
				wParam == UII_UNIT_TUNING_WINDOW )
			{
				m_bIsOpenedEquipWindow	= _GetEquipBoard()->IsOpen();
				m_bIsOpenedPackWindow	= _GetInventoryBoard()->UI_IsOpenedPack( _GetInventoryBoard()->GetActivePackNo() );

				if( !m_bIsOpenedEquipWindow )
					_GetEquipBoard()->OpenWindow();	
				if( !m_bIsOpenedPackWindow ) 
					_GetInventoryBoard()->UI_OpenPack( _GetInventoryBoard()->GetActivePackNo(), TRUE );								
			}
			// 채팅창
			else if( wParam == UII_CHAT_WINDOW )
			{
				if( !_GetMainGame()->IsChatMode() )			
				{
					_GetMainGame()->SetChatMode( TRUE );
					
					_GetChatWindow()->GetInputField()->EnableFocus();
					_GetChatWindow()->GetWhisperInputField()->EnableFocus();
					_GetChatInputWindow()->GetInputField()->EnableFocus();

					if( _GetChatWindow()->IsShow() )
						_GetChatWindow()->GetInputField()->SetFocus();
					else
						_GetChatInputWindow()->GetInputField()->SetFocus();
				}
			}
			break;

		case WM_CLOSE_WINDOW			:	
			// 거래전에 인벤토리가 닫혀있었으면 닫는다
			if( wParam == UII_SHOP_WINDOW || 
				wParam == UII_TRADE_WINDOW ||	
				wParam == UII_ORE_PROC_WINDOW ||
				wParam == UII_EXCHANGE_WINDOW ||
				wParam == UII_UNIT_TUNING_WINDOW )
			{
				if( !m_bIsOpenedEquipWindow &&
					_GetEquipBoard()->IsOpen() )
					_GetEquipBoard()->CloseWindow();
				
				if( !m_bIsOpenedPackWindow &&
					_GetInventoryBoard()->UI_IsOpenedPack( _GetInventoryBoard()->GetActivePackNo() ) )
					_GetInventoryBoard()->UI_OpenPack( _GetInventoryBoard()->GetActivePackNo(), FALSE );								
			}
			// 채팅창
			else if( wParam == UII_CHAT_WINDOW )
			{
				if( _GetMainGame()->IsChatMode() )			
				{
					_GetMainGame()->SetChatMode( FALSE );

					_GetChatInputWindow()->GetInputField()->DisableFocus();
				}
			}
			break;
	}

	for( int i = 0; i < MAX_BOARD_TYPE; ++i )
	{
		if( m_listBoard[i] && m_listBoard[i]->IsEqualWindow( wParam ) )
			return m_listBoard[i]->MsgProc( hWnd, uMsg, wParam, lParam );
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CBoardManager::AddObject( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pTarObject )
{
	if( pi_byBoardType >= IBT_EXCHANGE )
		return FALSE;

	if( m_listBoard[ pi_byBoardType ] )
	{
		CObjectBoard * pObjectBoard = static_cast< CObjectBoard * >( m_listBoard[ pi_byBoardType ] );

		pObjectBoard->Add( pi_byPackNo, pi_bySocketNo, pi_pTarObject );		
	}

	return FALSE;
}

BOOL
CBoardManager::RemoveObject( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, BYTE pi_byQuantity )
{
	if( pi_byBoardType >= IBT_EXCHANGE )
		return FALSE;

	if( m_listBoard[ pi_byBoardType ] )
	{
		CObjectBoard * pObjectBoard = static_cast< CObjectBoard * >( m_listBoard[ pi_byBoardType ] );

		pObjectBoard->Remove( pi_byPackNo, pi_bySocketNo, pi_byQuantity );		
	}

	return FALSE;
}

BOOL
CBoardManager::RemoveObjectByIndex( DWORD pi_dwIndex, BYTE pi_byQuantity )
{	
	for( int i = 0; i < IBT_EXCHANGE; ++i )
	{
		if( m_listBoard[i] )
		{
			CObjectBoard * pObjectBoard = static_cast< CObjectBoard * >( m_listBoard[i] );
			
			if( pObjectBoard->RemoveByIndex( pi_dwIndex, pi_byQuantity ) )
				return TRUE;
		}
	}

	return FALSE;
}

// return value - 실제로 삭제한 양
BYTE
CBoardManager::RemoveHoverObjectQuantity( BYTE pi_byQuantity )
{
	if( m_pHoverObject && m_pHoverObject->IsEqualItemType( BOT_BOARD_ITEM ) )
	{
		CBoardItem *	pBoardItem		= static_cast< CBoardItem * >( m_pHoverObject );
		BYTE			byItemQuantity	= pBoardItem->GetQuantity();

		if( ( pi_byQuantity == 0xFF ) ||
			( byItemQuantity - pi_byQuantity <= 0 ) )
		{
			SetHoverObject( NULL );
			return byItemQuantity;
		}
		else
		{
			pBoardItem->SetQuantity( byItemQuantity - pi_byQuantity );

			return pi_byQuantity;
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CBaseBoard *
CBoardManager::GetBoard( BYTE pi_byBoardType )
{
	if( pi_byBoardType >= MAX_BOARD_TYPE )
		return NULL;

	if( pi_byBoardType == _STORAGE_POS::EMBELLISH )
		return m_listBoard[IBT_EQUIP];

	return m_listBoard[pi_byBoardType];
}

CBoardObject *
CBoardManager::GetObject( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	if( pi_byBoardType >= IBT_EXCHANGE )
		return NULL;

	if( m_listBoard[ pi_byBoardType ] )
	{
		CObjectBoard * pObjectBoard = static_cast< CObjectBoard * >( m_listBoard[ pi_byBoardType ] );
		
		return pObjectBoard->GetObject( pi_byPackNo, pi_bySocketNo );		
	}

	return NULL;	
}

CBoardObject *
CBoardManager::GetObjectByIndex( DWORD pi_dwIndex )
{
	CBoardObject * pObject = NULL;
	CObjectBoard * pObjectBoard;

	for( int i = 0; i < IBT_EXCHANGE; ++i )
	{
		if( !m_listBoard[i] )
			continue;
		
		pObjectBoard = static_cast< CObjectBoard * >( m_listBoard[i] );
		pObject = pObjectBoard->GetObjectByIndex( pi_dwIndex );

		if( pObject )
			return pObject;
	}

	return pObject;
}

CBoardObject *
CBoardManager::GetObjectByIndex( BYTE pi_byBoardType, DWORD pi_dwIndex )
{
	if( pi_byBoardType >= IBT_EXCHANGE )
		return NULL;

	if( m_listBoard[ pi_byBoardType ] )
	{
		CObjectBoard * pObjectBoard = static_cast< CObjectBoard * >( m_listBoard[ pi_byBoardType ] );

		return pObjectBoard->GetObjectByIndex( pi_dwIndex );		
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CBoardManager::ConvertBoardObject( CBoardObject * po_pTarObject, CBoardObject * pi_pOrgObject )
{
	if( !po_pTarObject || !pi_pOrgObject )
		return FALSE;

	// board item
	if( pi_pOrgObject->IsEqualItemType( BOT_BOARD_ITEM ) )
	{
		CBoardItem * pOrgItem = static_cast< CBoardItem * >( pi_pOrgObject );

		if( po_pTarObject->IsEqualItemType( BOT_ANIMUS ) )
		{
			CAnimusItem * pTarItem = static_cast< CAnimusItem * >( po_pTarObject );

			pTarItem->SetIndex( pOrgItem->GetIndex() );
			pTarItem->SetData( pOrgItem->GetData() );

			DWORD			dwDurability = pOrgItem->GetDurability();
			_animus_param	stAnimusHpFp;
			memcpy( &stAnimusHpFp, &dwDurability, sizeof( DWORD ) );

			pTarItem->SetExp( pOrgItem->GetCurMaxDurability() );
			pTarItem->SetHP( stAnimusHpFp.wHP );
			pTarItem->SetFP( stAnimusHpFp.wFP );
		}
		else if( po_pTarObject->IsEqualItemType( BOT_SF ) )
		{
			CSFItem * pTarItem = static_cast< CSFItem * >( po_pTarObject );
			if( !pTarItem->IsEqualSFType( CAT_FORCE ) )
				return FALSE;

			pTarItem->SetIndex( pOrgItem->GetIndex() );
			pTarItem->SetItemDTIndex( pOrgItem->GetDTIndex() );
			pTarItem->CalculateSuccessCount( pOrgItem->GetDurability() );
		}
	}
	// animus item
	else if( pi_pOrgObject->IsEqualItemType( BOT_ANIMUS ) )
	{
		CAnimusItem * pOrgItem = static_cast< CAnimusItem * >( pi_pOrgObject );

		if( po_pTarObject->IsEqualItemType( BOT_BOARD_ITEM ) )
		{
			CBoardItem * pTarItem = static_cast< CBoardItem * >( po_pTarObject );

			pTarItem->SetIndex( pOrgItem->GetIndex() );
			pTarItem->SetData( pOrgItem->GetData() );
			pTarItem->SetIcon( pOrgItem->GetIcon() );

			DWORD			dwDurability;
			_animus_param	stAnimusHpFp = { pOrgItem->GetHP(), pOrgItem->GetFP() };
			memcpy( &dwDurability, &stAnimusHpFp, sizeof( DWORD ) );

			pTarItem->SetCurMaxDurability( pOrgItem->GetExp() );
			pTarItem->SetDurability( dwDurability );
		}		
	}
	// skill / force item
	else if( pi_pOrgObject->IsEqualItemType( BOT_SF ) )
	{
		CSFItem * pOrgItem = static_cast< CSFItem * >( pi_pOrgObject );
		if( !pOrgItem->IsEqualSFType( CAT_FORCE ) )
			return FALSE;

		if( po_pTarObject->IsEqualItemType( BOT_BOARD_ITEM ) )
		{
			CBoardItem*	pTarItem = static_cast< CBoardItem * >( po_pTarObject );
			CSFItem *	pOrgItem = static_cast< CSFItem * >( pi_pOrgObject );

			pTarItem->SetIndex( pOrgItem->GetIndex() );
			pTarItem->SetData( static_cast< ITEM_DATA * >( _GetItemDataMgr()->GetData( IEPT_FORCE, pOrgItem->GetItemDTIndex() ) ) );
			pTarItem->SetDurability( pOrgItem->GetSuccessCount() );
			pTarItem->SetQuantity( 1 );			
		}
	}
	// unit item
	else if( pi_pOrgObject->IsEqualItemType( BOT_UNIT_FRAME ) )
	{
		CUnitItem * pOrgItem = static_cast< CUnitItem * >( pi_pOrgObject );

		if( po_pTarObject->IsEqualItemType( BOT_BOARD_ITEM ) )
		{
			CBoardItem * pTarItem = static_cast< CBoardItem * >( po_pTarObject );

			if( !pTarItem->Create( IEPT_UNIT, pOrgItem->GetDTIndex() ) )
				return FALSE;

			po_pTarObject->SetIndex( pOrgItem->GetIndex() );
			po_pTarObject->SetServerIndex( pOrgItem->GetServerIndex() );
			po_pTarObject->SetIcon( pOrgItem->GetIcon() );
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CBoardManager::SetOrgObject( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo,
							 CBoardObject * pi_pObject )
{
	m_byOrgBoardType	= pi_byBoardType;
	m_byOrgPackNo		= pi_byPackNo;
	m_byOrgSocketNo		= pi_bySocketNo;
	m_pOrgObject		= pi_pObject;
}

void
CBoardManager::SetTarObject( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo,
							 CBoardObject * pi_pObject )
{
	m_byTarBoardType	= pi_byBoardType;
	m_byTarPackNo		= pi_byPackNo;
	m_byTarSocketNo		= pi_bySocketNo;
	m_pTarObject		= pi_pObject;
}

CBoardObject *
CBoardManager::SetHoverObject( CBoardObject * pi_pObject )
{
	if( pi_pObject )
	{
		if( m_pHoverObject )
			_DELETE( m_pHoverObject );

		m_pHoverObject	= pi_pObject->Clone();
		_GetUIScreen()->AddObject( pi_pObject->GetIcon() );
		m_pHoverObject->SetIcon( _GetUIScreen()->GetObject() );

		if( pi_pObject->IsEqualItemType( BOT_BOARD_ITEM ) ||
			pi_pObject->IsEqualItemType( BOT_UNIT_FRAME ) ||
			pi_pObject->IsEqualItemType( BOT_UNIT_PARTS ) )
			m_pHoverObject->GetIcon()->SetSize( 64, 64 );
		else
			m_pHoverObject->GetIcon()->SetSize( 40, 40 );
	}
	else
	{
		// 제거된 아이템 backup
		if( ( m_pHoverObject ) && ( m_pHoverObject->IsEqualItemType( BOT_BOARD_ITEM ) ) )
			m_pDeletedHoverItem->Copy( m_pHoverObject );

		_DELETE( m_pHoverObject );
		_GetUIScreen()->RemoveObject();
	}

	return m_pHoverObject;
}

void
CBoardManager::ClearOperation( void )
{
	SetHoverObject( NULL );

	m_byTarBoardType	= m_byOrgBoardType	= ID_INVALID;
	m_byTarPackNo		= m_byOrgPackNo		= ID_INVALID;
	m_byTarSocketNo		= m_byOrgSocketNo	= ID_INVALID;
	m_pTarObject		= m_pOrgObject		= NULL;
}

void
CBoardManager::ReturnHoverObject( void )
{
	AddObject( m_byOrgBoardType, m_byOrgPackNo, m_byOrgSocketNo, m_pHoverObject );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CBoardManager::SetRequestProcessState( BOOL pi_bFlag )
{
	m_bIsRequestProcess = pi_bFlag;

	if( m_bIsRequestProcess )
		m_bIsRequestProcessTime = timeGetTime();
	else
		m_bIsRequestProcessTime = 0;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CBoardManager::PutDownItemRequest( void )
{
	if( IsRequestProcess() )
		return FALSE;
	if( !m_pHoverObject )
		return FALSE;
	if( !m_pHoverObject->IsEqualItemType( BOT_BOARD_ITEM ) )
		return FALSE;

	CBoardItem * pHoverItem = static_cast< CBoardItem * >( m_pHoverObject );

	if( !pHoverItem->IsDumpable() )
	{
		_SetSystemMsg( "버리기가 불가능한 아이템 입니다." );
		return FALSE;
	}

	if( _GetPlayerTradeBoard()->IsTrading() )
	{
		_SetSystemMsg( "플레이어와 트레이드 중에는 아이템을 버릴 수 없습니다." );
		return FALSE;
	}

	CNetMsgProcessor_Item *	pNetMsgProcessor_Item = static_cast< CNetMsgProcessor_Item * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_ITEM ) );
	return pNetMsgProcessor_Item->ThrowStorageRequest_clzo();
}

void
CBoardManager::PutDownItemResult( void )
{
	CBoardItem * pHoverItem = NULL;
	if( !m_pHoverObject )
	{
		if( m_pDeletedHoverItem->IsEmpty() )
			return;

		pHoverItem = m_pDeletedHoverItem;
	}
	else
	{
		pHoverItem = GetHoverItem();
	}

	_SetSystemMsg( "%s %d개를 버렸습니다.", pHoverItem->GetName(), pHoverItem->GetQuantity() );
	
	pHoverItem->SetEmpty();
	SetHoverObject( NULL );
}

void
CBoardManager::CloseUnnecessaryWindow( void )
{
	if( _GetShopWindow()->IsShow() ) _GetShopWindow()->Close();
	if( _GetMiningWindow()->IsShow() ) _GetMiningWindow()->Close();
	if( _GetOreProcWindow()->IsShow() ) _GetOreProcWindow()->Close();
	if( _GetExchangeWindow()->IsShow() ) _GetExchangeWindow()->Close();
	if( _GetSummonsSelectWindow()->IsShow() ) _GetSummonsSelectWindow()->Close();
	if( _GetSummonsControlWindow()->IsShow() ) _GetSummonsControlWindow()->Close();
	if( _GetNpcIntroWindow()->IsShow() ) _GetNpcIntroWindow()->Close();
	if( _GetItemWorkWindow()->IsShow() ) _GetItemWorkWindow()->Close();
	if( _GetItemUpgradeWindow()->IsShow() ) _GetItemUpgradeWindow()->Close();	
	_GetTargetMgr()->CloseAllWindow();
	if( _GetMoveMapWindow()->IsShow() ) _GetMoveMapWindow()->Close();
	if( _GetMinimapWindow()->IsShow() ) _GetMinimapWindow()->Close();
	if( _GetSelectAttackPartWindow()->IsShow() ) _GetSelectAttackPartWindow()->Close();
	if( m_listBoard[IBT_CLASS_CHANGE]->IsOpen() ) m_listBoard[IBT_CLASS_CHANGE]->CloseWindow();
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CBoardManager::ReturnItemToItemBoard( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardItem * pi_pItem )
{
	if( !( pi_byBoardType == IBT_INVENTORY || pi_byBoardType == IBT_EQUIP || pi_byBoardType == IBT_ITEM_BELT ) )
		return FALSE;
	if( !pi_pItem )
		return FALSE;

	CItemBoard * pItemBoard = static_cast< CItemBoard * >( GetBoard( pi_byBoardType ) );
	if( !pItemBoard )
		return FALSE;

	BYTE byPackNo, bySocketNo;
	CBoardItem * pItem;

	pItem = pItemBoard->GetItemByIndex( byPackNo, bySocketNo, pi_pItem->GetIndex() );

	if( pItem )
	{
		if( pi_byBoardType == IBT_EQUIP )
			return FALSE;

		pItem->SetQuantity( pItem->GetQuantity() + pi_pItem->GetQuantity() );
	}
	else
	{
		byPackNo = pi_byPackNo;
		bySocketNo = pi_bySocketNo;
		
		if( pItemBoard->GetItem( byPackNo, bySocketNo ) )
		{
			if( pi_byBoardType == IBT_EQUIP )
				return FALSE;

			if( !pItemBoard->FindEmptySocket( byPackNo, bySocketNo ) )
				return FALSE;
		}

		pItemBoard->Add( byPackNo, bySocketNo, pi_pItem );
	}

	return TRUE;
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

CGlobalGameInformation g_clGameInformation;

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CGlobalGameInformation::CGlobalGameInformation()
{
	Init();
}

CGlobalGameInformation::~CGlobalGameInformation()
{
}

void
CGlobalGameInformation::Init( void )
{
	ZeroMemory( this, sizeof( CGlobalGameInformation ) );
}

void
CGlobalGameInformation::SetRequestProcessState( BOOL pi_bFlag )
{
	m_bIsRequestProcess = pi_bFlag;

	if( m_bIsRequestProcess )
		m_dwRequestProcessTime = timeGetTime();
	else
		m_dwRequestProcessTime = 0;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////