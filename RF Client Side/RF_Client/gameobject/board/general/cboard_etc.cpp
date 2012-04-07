
#include "../../Manager/CCharacterMgr.h"
#include "../../Manager/CTargetMgr.h"
#include "../Common/CBoardManager.h"

#include "../../../CLand.h"

#include "../../../Network/NetMsgProcessor/Resource/CNetMsgProcessor_Resource.h"
#include "../../../Network/NetMsgProcessor/ItemTrade/CNetMsgProcessor_ItemTrade.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CMiningBoard::CMiningBoard()
{
	m_byDummyIndex			= ID_INVALID;
	m_dwMiningToolIndex		= ID_INVALID;
	m_dwBatteryIndex		= ID_INVALID;
	m_byMiningRequestType	= MRT_NONE;	

	m_bIsMining				= FALSE;

	m_byOreIndex			= 0;	

	m_pNetMsgProcessor_Resource	= static_cast< CNetMsgProcessor_Resource * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_RESOURCE ) );
}

CMiningBoard::~CMiningBoard()
{

}

void	
CMiningBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiMiningWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );	
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CMiningBoard::SelectOre( BYTE pi_byOreIndex )
{
	m_byOreIndex = pi_byOreIndex;

	m_uiMiningWindow.SelectOre( m_byOreIndex );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 채굴 요청
BOOL
CMiningBoard::Mining_Request( BOOL pi_bCheckCondition )
{
	if( IsMining() )
		return FALSE;

	if( m_byMiningRequestType != MRT_NONE )
		return FALSE;

	
	if( pi_bCheckCondition && !CheckConditionForMining() )
		return FALSE;	

	if( !CheckItemForMining() )
		return FALSE;		
		
	if( !m_pNetMsgProcessor_Resource->MineStartRequest_clzo() )
		return FALSE;			

	return TRUE;
}

// 채굴 취소
BOOL
CMiningBoard::Mining_Cancel( void )
{
	if( !IsMining() )
		return FALSE;

	if( _GetAvatar()->IsEqualAction( CAI_ITEM_USE_MINING ) )
		m_pNetMsgProcessor_Resource->MineCancleRequest_clzo();

	return TRUE;
}

// 채굴 종료 요청
BOOL
CMiningBoard::Mining_Complete_Request( void )
{
	if( !IsMining() )
		return FALSE;

	if( timeGetTime() < m_dwMiningEndTime )
		return FALSE;

	if( m_byMiningRequestType != MRT_NONE )
		return FALSE;

	m_pNetMsgProcessor_Resource->MineCompleteRequest_clzo();
	
	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 채굴
BOOL
CMiningBoard::Mining_Request_By_TargetPos( Vector3f pi_vecTargetPos )
{
	if( m_byMiningRequestType != MRT_NONE )
		return FALSE;

	m_byDummyIndex = 0xFF;

	// 클릭한 위치 체크
	for( int i = 0; i < _GetLand()->GetMaxMineNum( _GetLand()->GetMapIndex() ); ++i )
	{
		MINE_INFO * l_pMineInfo = _GetLand()->GetMineInfo( _GetLand()->GetMapIndex(), i );
		if( !l_pMineInfo )
			continue;

		if( ( pi_vecTargetPos[0] >= l_pMineInfo->vecMin[0] ) &&
			( pi_vecTargetPos[2] >= l_pMineInfo->vecMin[2] ) &&
			( pi_vecTargetPos[0] <= l_pMineInfo->vecMax[0] ) &&
			( pi_vecTargetPos[2] <= l_pMineInfo->vecMax[2] ) )
		{
			m_byDummyIndex = i;
			break;
		}
	}

	if( m_byDummyIndex == 0xFF )
	{
		_SetSystemMsg( "채굴할 수 없는 곳입니다." );
		return FALSE;
	}

	// 움직일 수 있다면...
	if( _GetAvatar()->IsEnableMove() )
	{
		float l_fDist = GetDist( _GetAvatar()->GetPosition(), pi_vecTargetPos );

		// 거리가 너무 가깝다면 무시한다.
		if( l_fDist <= 20 )
			return TRUE;

		// 채굴중이라면 먼저 채굴을 취소한다.
		if( _GetAvatar()->IsEqualAction( CAI_ITEM_USE_MINING ) )
			m_pNetMsgProcessor_Resource->MineCancleRequest_clzo();			

		// 지정한 곳으로 이동
		//_GetAvatar()->SetChaseTarget( CHASE_ORE, CHASE_FOR_MINING );
		_GetTargetMgr()->SetPursuitInfo( PTT_ORE, PPT_MINING );
		memcpy( m_vecMiningPosition, pi_vecTargetPos, sizeof( float ) * 3 );
		_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( pi_vecTargetPos, TRUE );
		return TRUE;
	}
	// 움직일 수 없다면 상태 저장...
	else
	{
		_GetNextActionMgr()->SetNextAction( CAI_ITEM_USE_MINING );
		_GetNextActionMgr()->SetNextTargetPosition( pi_vecTargetPos );

		return FALSE;
	}

	return FALSE;
}

// 채굴전 체크 사항
BOOL
CMiningBoard::CheckConditionForMining( void )
{
	if( _GetAvatar()->IsEqualUnitMountState( UMS_MOUNT ) )
		return FALSE;

	if( _GetAvatar()->GetCombatMode() != CSI_COMBAT )
		return FALSE;

	if( _GetTradeWindow()->IsShow() )
	{
		_SetSystemMsg( "플레이어와 트레이드 중에는 채굴할 수 없습니다." );
		return FALSE;
	}

	CBoardItem * pWeapon = _GetEquipBoard()->GetItem( CEPT_CLIENT_TO_UI[IEPT_WEAPON] );
	if( !pWeapon )
		return FALSE;

	if( !pWeapon->IsEqualWeaponCategoryID( IWCT_MINE ) )
		return FALSE;

	m_dwMiningToolIndex = pWeapon->GetIndex();

	//--------------------------------------------------------------------------

	// 현재 위치가 광산 더미 위치인지를 체크
	Vector3f vecAvatarPos;
	_GetAvatar()->GetPosition( vecAvatarPos );
	for( int i = 0; i < _GetLand()->GetMaxMineNum( _GetLand()->GetMapIndex() ); ++i )
	{
		MINE_INFO * pMineInfo = _GetLand()->GetMineInfo( _GetLand()->GetMapIndex(), i );
		if( !pMineInfo )
			continue;

		if( ( vecAvatarPos[0] >= pMineInfo->vecMin[0] ) &&
			( vecAvatarPos[2] >= pMineInfo->vecMin[2] ) &&
			( vecAvatarPos[0] <= pMineInfo->vecMax[0] ) &&
			( vecAvatarPos[2] <= pMineInfo->vecMax[2] ) )
		{
			m_byDummyIndex = i;
			break;
		}
	}

	if( m_byDummyIndex == 0xFF )
	{
		_SetSystemMsg( "채굴할 수 없는 곳입니다." );
		return FALSE;
	}

	return TRUE;
}

// 채굴 준비
BOOL
CMiningBoard::CheckItemForMining( void )
{
	CBoardItem	clsBattery;
	BYTE		byPackNo, bySocketNo;

	// 인벤토리내의 빈 공간 체크
	BYTE byEmptySocketNum = _GetInventoryBoard()->GetEmptySocketNum();
	if( byEmptySocketNum == 0 )
	{
		_SetSystemMsg( "인벤토리에 빈 공간이 없습니다." );
		return FALSE;
	}
	else if( byEmptySocketNum == 1 )
	{
		if( _GetGBM()->HaveHoverObject() && ( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY ) )
		{
			_SetSystemMsg( "인벤토리에 빈 공간이 없습니다." );
			return FALSE;
		}
	}

	// 인벤토리내의 베터리 유무 체크
	if( _GetInventoryBoard()->GetItemByItemType( clsBattery, byPackNo, bySocketNo, IEPT_BATTERY ) )
	{
		// 배터리 용량 체크
		if( clsBattery.GetDurability() > 0 )
		{
			m_byBatteryPackNo	= byPackNo;
			m_byBatterySocketNo	= bySocketNo;
			m_dwBatteryIndex	= clsBattery.GetIndex();

			return TRUE;
		}
		else
		{
			_SetSystemMsg( "배터리의 용량이 부족합니다. 새로운 배터리를 사용하십시오." );
			return FALSE;
		}
	}
	else
	{
		_SetSystemMsg( "인벤토리에 배터리가 없습니다." );
		return FALSE;
	}

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void	
CMiningBoard::SetMining( BOOL pi_bIsMining )
{
	m_bIsMining = pi_bIsMining;

	if( m_bIsMining	)
		m_uiMiningWindow.Mining_On();
	else
		m_uiMiningWindow.Mining_Off();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

LRESULT
CMiningBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{	
	switch( uMsg )
	{
		case WM_MINING_BOARD_SELECT_ORE	:
			SelectOre( lParam );
			break;

		case WM_MINING_BOARD_MINING		:
			if( IsMining() )
				Mining_Cancel();
			else
				Mining_Request();

			break;		
	}

	return CBaseBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CExchangeBoard::CExchangeBoard()
{
	m_pNetMsgProcessor_ItemTrade = static_cast< CNetMsgProcessor_ItemTrade * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_ITEM_TRADE ) );
}

CExchangeBoard::~CExchangeBoard()
{

}

void	
CExchangeBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiExchangeWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );	

	m_uiWindow->SetPos( _GetUIScreen()->GetSizeX() - m_uiWindow->GetSizeX() * 2 - 15, _GetUIScreen()->GetPosY() );

	m_uiExchangeWindow.SetRaceType( _GetAvatar()->GetRace() );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CExchangeBoard::ExchangeMoney_Request( BYTE pi_byMoneyType, DWORD pi_dwMoney )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;

	if( pi_byMoneyType == IMT_GOLD )
	{
		if( _GetAvatar()->GetGold() >= pi_dwMoney )
		{
			m_pNetMsgProcessor_ItemTrade->ExchangeGoldForDalantRequest_clzo( pi_dwMoney );			
			return TRUE;
		}
	}
	else
	{
		if( _GetAvatar()->GetDalant() >= pi_dwMoney )
		{
			m_pNetMsgProcessor_ItemTrade->ExchangeDalantForGoldRequest_clzo( pi_dwMoney );
			return TRUE;
		}
	}

	return FALSE;
}

void
CExchangeBoard::ExchangeMoney_Result( DWORD pi_dwDalant, DWORD pi_dwGold )
{
	// 성공
	if( ( pi_dwDalant != 0 ) || ( pi_dwGold != 0 ) )
	{
		_GetAvatar()->SetDalant( pi_dwDalant );
		_GetAvatar()->SetGold( pi_dwGold );

		ClearExchange();
	}
	else
	// 실패
	{
		_GUIMessageBox( NULL, "환전에 실패 했습니다.", 0xAAFFFFFF );
	}
}

void
CExchangeBoard::ClearExchange( void )
{
	m_uiExchangeWindow.SetGold( _GetAvatar()->GetGold() );
	m_uiExchangeWindow.SetDalant( _GetAvatar()->GetDalant() );
	m_uiExchangeWindow.SetExchangeAdjustbar();

	m_uiExchangeWindow.EnableExchange( FALSE );
	m_uiExchangeWindow.EnableRecovery( FALSE );
}

LRESULT
CExchangeBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{	
	switch( uMsg )
	{
		case WM_EXCHANGE_BOARD_EXCHANGE_CHECK	:			
			{	
				char pStrBuf[256];			
				char pMoneyBuf[32];
				char pMoneyUnitBuf[64];

				if( m_uiExchangeWindow.GetGold() > _GetAvatar()->GetGold() )
				{
					_GetPriceStr( m_uiExchangeWindow.GetGold() - _GetAvatar()->GetGold(), pMoneyBuf );				
					sprintf( pMoneyUnitBuf, "[%s] 골드", pMoneyBuf );
				}
				else if( m_uiExchangeWindow.GetDalant() > _GetAvatar()->GetDalant() )
				{
					_GetPriceStr( m_uiExchangeWindow.GetDalant() - _GetAvatar()->GetDalant(), pMoneyBuf );
					sprintf( pMoneyUnitBuf, "[%s] %s", pMoneyBuf, _GetAvatarMoneyName() );
				}
				else
				{
					break;
				}

				/*
				sprintf( pStrBuf, "%s로 환전됩니다. 실시\n"
									"간 환율변동에 의해서 금액에 다소\n"
									"차이가 날수도 있습니다.\n"
									"승인하시겠습니까?\n", pStrBuf );*/
				sprintf( pStrBuf, "환전 금액의 5%% 수수료가 감안된\n"
					         		"%s로 환전됩니다.\n"
									"\n"
									"승인하시겠습니까?\n", pMoneyUnitBuf );

				_GUIMessageBox( &m_uiExchangeWindow , pStrBuf, 0xCCFFFFFF, MSGBOX_ID_EXCHANGE, DLG_STYLE_OK_CANCEL );
			}
			break;

		case WM_EXCHANGE_BOARD_EXCHANGE			:
			if( m_uiExchangeWindow.GetGold() < _GetAvatar()->GetGold() )
			{
				ExchangeMoney_Request( IMT_GOLD, _GetAvatar()->GetGold() - m_uiExchangeWindow.GetGold() );
			}
			else if( m_uiExchangeWindow.GetDalant() < _GetAvatar()->GetDalant() )
			{
				ExchangeMoney_Request( IMT_DALANT, ( m_uiExchangeWindow.GetGold() - _GetAvatar()->GetGold() ) * _GetAvatar()->GetExchangeRate() );
			}			
			break;

		case WM_EXCHANGE_BOARD_CANCEL			:
			ClearExchange();
			break;

		case WM_OPEN_WINDOW						:
			ClearExchange();
			m_uiExchangeWindow.SetExchangeCommission( _GetAvatar()->GetExchangeCommission() );
			break;
	}	

	return CBaseBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}