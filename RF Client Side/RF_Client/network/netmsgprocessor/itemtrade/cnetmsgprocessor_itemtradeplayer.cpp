////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_ItemTradePlayer Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../../Character/Avatar/CAvatar.h"
#include "../../../DataFile.h"
#include "../../../DataTable/CItemDataMgr.h"

#include "../../../Item/CItemData.h"
#include "../../../GameObject/Board/Common/CBoardManager.h"
#include "../../../GameObject/Manager/CCharacterMgr.h"

#include "../../../GameObject/Board/General/CGameEventBoard.h"

#include "CNetMsgProcessor_ItemTradePlayer.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_ItemTradePlayer::CNetMsgProcessor_ItemTradePlayer()
{

}

CNetMsgProcessor_ItemTradePlayer::~CNetMsgProcessor_ItemTradePlayer()
{

}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetMsgProcessor_ItemTradePlayer::SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	return CNetMsgProcessor_Base::SendNetMessage( dtrade_msg, pi_byMsgIndex, pi_pMsg, pi_nSize );
}
			
bool	
CNetMsgProcessor_ItemTradePlayer::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	if( pMsgHeader->m_byType[0] != dtrade_msg )
		return false;

	switch( pMsgHeader->m_byType[1] )
	{	
		case d_trade_ask_result_clzo :
			TradeAskResult_zocl( pi_pMsg );
			break;

		case d_trade_ask_inform_zocl :
			TradeAskInform_zocl( pi_pMsg );
			break;

		case d_trade_answer_result_zocl :
			TradeAnswerResult_zocl( pi_pMsg );
			break;

		case d_trade_start_inform_zocl :
			TradeStartInform_zocl( pi_pMsg );
			break;

		case d_trade_cancle_result_zocl :
			TradeCancleResult_zocl( pi_pMsg );
			break;

		case d_trade_cancle_inform_zocl :
			TradeCancleInform_zocl( pi_pMsg );
			break;

		case d_trade_lock_result_zocl :
			TradeLockResult_zocl( pi_pMsg );
			break;

		case d_trade_lock_inform_zocl :
			TradeLockInform_zocl( pi_pMsg );
			break;

		case d_trade_add_result_zocl :
			TradeAddResult_zocl( pi_pMsg );
			break;

		case d_trade_add_inform_zocl :
			TradeAddInform_zocl( pi_pMsg );
			break;

		case d_trade_del_result_zocl :
			TradeDelResult_zocl( pi_pMsg );
			break;

		case d_trade_del_inform_zocl :
			TradeDelInform_zocl( pi_pMsg );
			break;

		case d_trade_bet_result_zocl :
			TradeBetResult_zocl( pi_pMsg );
			break;

		case d_trade_bet_inform_zocl :
			TradeBetInform_zocl( pi_pMsg );
			break;

		case d_trade_ok_result_zocl :
			TradeOkResult_zocl( pi_pMsg );
			break;

		case d_trade_ok_inform_zocl :
			TradeOkInform_zocl( pi_pMsg );
			break;

		case d_trade_accomplish_inform_zocl :
			TradeAccomplishInform_zocl( pi_pMsg );
			break;

		case d_trade_close_inform_zocl :
			TradeCloseInform_zocl( pi_pMsg );
			break;

		default :
			m_pNetworkMgr->NetLogWrite( "Unknown Message : CNetMsgProcessor_ItemTrade::AnalysisServerPacket()\n" );
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타가 다른 플레이어에게 아이템 트레이드를 요청함
BOOL
CNetMsgProcessor_ItemTradePlayer::TradeAskRequest_clzo( DWORD pi_dwServerIndex )
{
	_d_trade_ask_request_clzo stSendMsg;

	stSendMsg.wDstIndex = pi_dwServerIndex;

	_GetPlayerTradeBoard()->SetRequestOfAvatar( TRUE );

	return SendNetMessage( d_trade_ask_request_clzo,
						   &stSendMsg,
						   sizeof( _d_trade_ask_request_clzo ) );
}

// 아이템 트레이드를 요청에 대한 결과
void
CNetMsgProcessor_ItemTradePlayer::TradeAskResult_zocl( char * pi_pMsg )
{
	_d_trade_ask_result_clzo * pRecvMsg = 
		reinterpret_cast< _d_trade_ask_result_clzo * >( pi_pMsg );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_SetSystemMsg( "아이템 트레이드 요청 성공 : 트레이드 수락을 대기중입니다." );
	}
	// 실패
	else
	{
		switch( pRecvMsg->byErrCode )
		{
			case 1	: _SetSystemMsg( "아이템 트레이드 요청 실패 : 이미 다른 사람과 트레이드 중입니다." ); break;
			case 2	: _SetSystemMsg( "아이템 트레이드 요청 실패 : 대상자와의 거리가 멉니다." ); break;
			case 3	: _SetSystemMsg( "아이템 트레이드 요청 실패 : 유효한 대상이 아닙니다." ); break;
			case 4	: _SetSystemMsg( "아이템 트레이드 요청 실패 : 대상자가 이미 다른 사람과 트레이드 중입니다." ); break;
			default : _SetSystemMsg( "아이템 트레이드 요청 실패 : 알 수 없는 에러" ); break;
		}
	}

}


// 다른 플레이어가 아바타에게 아이템 트레이드를 요청해 왔음
void
CNetMsgProcessor_ItemTradePlayer::TradeAskInform_zocl( char * pi_pMsg )
{
	_d_trade_ask_inform_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_ask_inform_zocl * >( pi_pMsg );

	if( _GetPlayerTradeBoard()->IsTrading() )
		return;

	CPlayer * l_pPlayer = _GetCharMgr()->GetPlayer( pRecvMsg->idAsker.dwSerial );
	if( !l_pPlayer )
		return;

	_GetPlayerTradeBoard()->SetTradeChar( l_pPlayer );
	_GetPlayerTradeBoard()->SetRequestOfAvatar( FALSE );

	CTradeEvent l_clsTradeEvent;
	l_clsTradeEvent.SetType( GET_TRADE );
	l_clsTradeEvent.SetPartyEventType( CTradeEvent::RECEIVE_TRADE_REQUEST );
	l_clsTradeEvent.SetApplicant( l_pPlayer );
	_GetGameEventBoard()->PushEvent( l_clsTradeEvent );
	
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 다른 플레이어로부터의 아이템 트레이드 요청을 수락 - 수락하지 않으면 보내지 않음
BOOL	
CNetMsgProcessor_ItemTradePlayer::TradeAnswerRequest_clzo( void )
{
	_d_trade_answer_request_clzo stSendMsg;

	stSendMsg.idAsker.dwSerial	= _GetPlayerTradeBoard()->GetTradeCharIndex();
	stSendMsg.idAsker.wIndex	= _GetPlayerTradeBoard()->GetTradeCharServerIndex();


	return SendNetMessage( d_trade_answer_request_clzo,
						   &stSendMsg,
						   sizeof( _d_trade_answer_request_clzo ) );	

}

// 아이템 트레이드 요청 수락에 대한 응답
void
CNetMsgProcessor_ItemTradePlayer::TradeAnswerResult_zocl( char * pi_pMsg )
{
	_d_trade_answer_result_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_answer_result_zocl * >( pi_pMsg );

	_GetPlayerTradeBoard()->SetTradeRequestTime( 0 );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_SetSystemMsg( "아이템 트레이드 수락 성공 : 아이템 트레이드를 시작합니다." );
	}
	// 실패
	else
	{
		switch( pRecvMsg->byErrCode )
		{
			case 1	: _SetSystemMsg( "아이템 트레이드 수락 실패 : 유효한 요청이 아닙니다." ); break;
			case 2	: _SetSystemMsg( "아이템 트레이드 수락 실패 : 아바타가 이미 다른 사람과 트레이드 중입니다." ); break;
			case 3	: _SetSystemMsg( "아이템 트레이드 수락 실패 : 대상자가 이미 다른 사람과 트레이드 중입니다." ); break;
			case 4	: _SetSystemMsg( "아이템 트레이드 수락 실패 : asker가자신을요청중아님." ); break;
			case 5	: _SetSystemMsg( "아이템 트레이드 수락 실패 : 대상자와의 거리가 멉니다." ); break;
			case 6	: _SetSystemMsg( "아이템 트레이드 수락 실패 : 유효한 대상이 아닙니다." ); break;
			default : _SetSystemMsg( "아이템 트레이드 수락 실패 : 알 수 없는 에러" ); break;
		}
	}
	
}

// 아이템 트레이드를 수락하면 요청자와 수락자 모두에게 인벤토리의 빈 칸수가 전달된다.
void
CNetMsgProcessor_ItemTradePlayer::TradeStartInform_zocl( char * pi_pMsg )
{
	_d_trade_start_inform_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_start_inform_zocl * >( pi_pMsg );

	// 상대방에게 줄 수 있는 아이템의 갯수 세팅
	if( _GetPlayerTradeBoard()->IsRequestOfAvatar() )
	{
		_GetPlayerTradeBoard()->SetMaxSendNum( pRecvMsg->byAnswerEmptyNum );
		_GetPlayerTradeBoard()->SetMaxReceiveNum( pRecvMsg->byAskerEmptyNum );
	}
	else
	{
		_GetPlayerTradeBoard()->SetMaxSendNum( pRecvMsg->byAskerEmptyNum );
		_GetPlayerTradeBoard()->SetMaxReceiveNum( pRecvMsg->byAnswerEmptyNum );
	}

	// 이름 세팅
	CCharacter * l_pTradeChar = _GetPlayerTradeBoard()->GetTradeChar();
	if( l_pTradeChar )
	{
		char l_pBuf[64];
		if( _GetPlayerTradeBoard()->IsRequestOfAvatar() )
		{
			sprintf( l_pBuf, "%s ( %d개 )", l_pTradeChar->GetName(), pRecvMsg->byAnswerEmptyNum );			
		}
		else
		{
			sprintf( l_pBuf, "%s ( %d개 )", l_pTradeChar->GetName(), pRecvMsg->byAskerEmptyNum );
		}

		_GetTradeWindow()->SetAvatarName( _GetAvatar()->GetName() );
		_GetTradeWindow()->SetOtherName( l_pBuf );		
	}

	_GetPlayerTradeBoard()->SetTrading( TRUE );
	_GetPlayerTradeBoard()->OpenWindow();
	

	// 트레이드와 관련된 모든 이벤트를 지운다.
	_GetGameEventBoard()->ClearAllEvent( GET_TRADE );	
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아이템 트레이드 취소 요청
BOOL
CNetMsgProcessor_ItemTradePlayer::TradeCancleRequest_clzo( void )
{
	_d_trade_cancle_request_clzo stSendMsg;

	return SendNetMessage( d_trade_cancle_request_clzo,
						   &stSendMsg,
						   sizeof( _d_trade_cancle_request_clzo ) );		
}

// 아이템 트레이드 취소 요청에 대한 응답
void
CNetMsgProcessor_ItemTradePlayer::TradeCancleResult_zocl( char * pi_pMsg )
{
	_d_trade_cancle_result_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_cancle_result_zocl * >( pi_pMsg );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_GetPlayerTradeBoard()->CancelTrade();
		_SetSystemMsg( "아이템 트레이드를 취소하였습니다." );
	}
	// 실패
	else
	{
		switch( pRecvMsg->byErrCode )
		{
			case 1	: _SetSystemMsg( "아이템 트레이드 취소 실패 : 아바타가 트레이드중이 아닙니다." ); break;
			case 2	: _SetSystemMsg( "아이템 트레이드 취소 실패 : 트레이드 잠금 상태입니다." ); break;
			default : _SetSystemMsg( "아이템 트레이드 취소 실패 : 알 수 없는 에러" ); break;
		}
	}
	
}

// 아이템 트레이드를 하던 플레이어가 아이템 트레이드를 취소하였음
void
CNetMsgProcessor_ItemTradePlayer::TradeCancleInform_zocl( char * pi_pMsg )
{
	_GetPlayerTradeBoard()->CancelTrade();

	_SetSystemMsg( "상대방이 아이템 트레이드를 취소하였습니다." );

}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타의 아이템 트레이드창 잠금 요청
BOOL
CNetMsgProcessor_ItemTradePlayer::TradeLockRequest_clzo( void )
{
	_d_trade_lock_request_clzo stSendMsg;

	return SendNetMessage( d_trade_lock_request_clzo,
						   &stSendMsg,
						   sizeof( _d_trade_lock_request_clzo ) );	
}

// 아바타의 아이템 트레이드창 잠금 요청에 대한 응답
void
CNetMsgProcessor_ItemTradePlayer::TradeLockResult_zocl( char * pi_pMsg )
{
	_d_trade_lock_result_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_lock_result_zocl * >( pi_pMsg );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_GetPlayerTradeBoard()->SetLockSendList( TRUE );
	}
	// 실패
	else
	{
		;
	}
	
}

// 아이템 트레이드를 하던 플레이어가 잠금을 하였음
void
CNetMsgProcessor_ItemTradePlayer::TradeLockInform_zocl( char * pi_pMsg )
{

	_GetPlayerTradeBoard()->SetLockReceiveList( TRUE );	
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타가 트레이드할 아이템의 추가를 요청하였음
BOOL
CNetMsgProcessor_ItemTradePlayer::TradeAddRequest_clzo( void )
{
	_d_trade_add_request_clzo stSendMsg;

	SEND_ITEM *	pSendItem = _GetPlayerTradeBoard()->GetCurSendItem();
	if( !pSendItem )
		return FALSE;

	stSendMsg.byStorageCode	= GetStoragePos( pSendItem->m_byBoardType, pSendItem->m_clsItem.GetItemKind() );
	stSendMsg.dwSerial		= pSendItem->m_clsItem.GetIndex();
	stSendMsg.byAmount		= pSendItem->m_clsItem.GetQuantity();
	stSendMsg.bySlotIndex	= _GetPlayerTradeBoard()->GetSelectSocketNo();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( d_trade_add_request_clzo,
						   &stSendMsg,
						   sizeof( _d_trade_add_request_clzo ) );	
}

// 트레이드할 아이템의 추가를 요청에 대한 응답
void
CNetMsgProcessor_ItemTradePlayer::TradeAddResult_zocl( char * pi_pMsg )
{
	_d_trade_add_result_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_add_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_GetPlayerTradeBoard()->SelectItem_Result( TRUE );
	}
	// 실패
	else
	{
		_GetPlayerTradeBoard()->SelectItem_Result( FALSE );
	}
	
}

// 아이템 트레이드를 하는 플레이어가 아이템을 추가 하였음
void
CNetMsgProcessor_ItemTradePlayer::TradeAddInform_zocl( char * pi_pMsg )
{
	_d_trade_add_inform_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_add_inform_zocl * >( pi_pMsg );

	CBoardItem clsItem;
	if( !_GetItemDataMgr()->GetData( IEPT_SERVER_TO_CLIENT[pRecvMsg->byTableCode],
									 pRecvMsg->wItemIndex, &clsItem, FALSE, 0xFF ) )
		return;

	clsItem.SetIndex( pRecvMsg->bySlotIndex );
	clsItem.SetDurability( pRecvMsg->dwDurPoint );
	clsItem.SetQuantity( pRecvMsg->byAmount );
	clsItem.InsertTalikFromServerFlag( pRecvMsg->dwUdtInfo );

	_GetPlayerTradeBoard()->AddReceiveItem( &clsItem, pRecvMsg->bySlotIndex );
	
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타가 트레이드할 아이템의 삭제를 요청하였음
BOOL
CNetMsgProcessor_ItemTradePlayer::TradeDelRequest_clzo( void )
{
	_d_trade_del_request_clzo stSendMsg;

	stSendMsg.bySlotIndex = _GetPlayerTradeBoard()->GetSelectSocketNo();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( d_trade_del_request_clzo,
						   &stSendMsg,
						   sizeof( _d_trade_del_request_clzo ) );	
}

// 트레이드할 아이템의 추가를 삭제에 대한 응답
void
CNetMsgProcessor_ItemTradePlayer::TradeDelResult_zocl( char * pi_pMsg )
{
	_d_trade_del_result_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_del_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_GetPlayerTradeBoard()->DeselectItem_Result( TRUE );
	}
	// 실패
	else
	{
		_GetPlayerTradeBoard()->DeselectItem_Result( FALSE );
	}
	
}

// 아이템 트레이드를 하는 플레이어가 아이템을 삭제 하였음
void
CNetMsgProcessor_ItemTradePlayer::TradeDelInform_zocl( char * pi_pMsg )
{
	_d_trade_del_inform_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_del_inform_zocl * >( pi_pMsg );

	_GetPlayerTradeBoard()->RemoveReceiveItem( pRecvMsg->bySlotIndex );	
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타가 골드나 달란트의 트레이드를 요청
BOOL
CNetMsgProcessor_ItemTradePlayer::TradeBetRequest_clzo( BYTE pi_byMoneyType, DWORD pi_dwAmount )
{
	_d_trade_bet_request_clzo stSendMsg;

	stSendMsg.byMoneyUnit	= pi_byMoneyType;
	stSendMsg.dwBetAmount	= pi_dwAmount;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( d_trade_bet_request_clzo,
						   &stSendMsg,
						   sizeof( _d_trade_bet_request_clzo ) );	
}

// 골드나 달란트의 트레이드 요청에 대한 응답
void
CNetMsgProcessor_ItemTradePlayer::TradeBetResult_zocl( char * pi_pMsg )
{
	_d_trade_bet_result_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_bet_result_zocl * >( pi_pMsg );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		;
	}
	// 실패
	else
	{
		;
	}	
}

// 아이템 트레이드를 하는 플레이어가 골드나 달란트의 트레이드를 요청했음
void
CNetMsgProcessor_ItemTradePlayer::TradeBetInform_zocl( char * pi_pMsg )
{
	_d_trade_bet_inform_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_bet_inform_zocl * >( pi_pMsg );

	if( pRecvMsg->byMoneyUnit == IMT_DALANT )
	{
		_GetPlayerTradeBoard()->SetReceiveDalant( pRecvMsg->dwBetAmount );
	}
	else
	{
		_GetPlayerTradeBoard()->SetReceiveGold( pRecvMsg->dwBetAmount );
	}
	
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타가 트레이드 종료를 요청하였음
BOOL
CNetMsgProcessor_ItemTradePlayer::TradeOkRequest_clzo( void )
{
	_d_trade_ok_request_clzo	stSendMsg;		

	return SendNetMessage( d_trade_ok_request_clzo,
						   &stSendMsg,
						   sizeof( _d_trade_ok_request_clzo ) );
}

// 트레이드 종료를 요청에 대한 응답
void
CNetMsgProcessor_ItemTradePlayer::TradeOkResult_zocl( char * pi_pMsg )
{
	_d_trade_ok_result_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_ok_result_zocl * >( pi_pMsg );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_GetPlayerTradeBoard()->SetAcceptSendList( TRUE );
	}
	// 실패
	else
	{
		if( pRecvMsg->byErrCode == 100 )
			_SetSystemMsg( "트레이드 종료 요청 실패 : 상대방의 인벤토리 공간이 부족합니다." );
		else
			_SetSystemMsg( "트레이드 종료 요청 실패" );
	}
	
}

// 아이템 트레이드를 하는 플레이어가 트레이드 종료를 요청했음
void
CNetMsgProcessor_ItemTradePlayer::TradeOkInform_zocl( char * pi_pMsg )
{
	_GetPlayerTradeBoard()->SetAcceptReceiveList( TRUE );	
}

// 아이템 트레이드에 대한 최종 결과
void
CNetMsgProcessor_ItemTradePlayer::TradeAccomplishInform_zocl( char * pi_pMsg )
{
	_d_trade_accomplish_inform_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_accomplish_inform_zocl * >( pi_pMsg );

	if( pRecvMsg->bSucc )
	{
		_GetPlayerTradeBoard()->CompleteTrade( TRUE, pRecvMsg->wStartSerial );

		_GetAvatar()->SetDalant( pRecvMsg->dwDalant );
		_GetAvatar()->SetGold( pRecvMsg->dwGold );

		_SetSystemMsg( "아이템 트레이드를 종료하였습니다." );
	}
	else
	{
		_GetPlayerTradeBoard()->CompleteTrade( FALSE, 0 );
		_SetSystemMsg( "아이템 트레이드를 실패하였습니다." );
	}	
}

// 거래 오류로 인한 종료
void
CNetMsgProcessor_ItemTradePlayer::TradeCloseInform_zocl( char * pi_pMsg )
{
	_d_trade_close_inform_zocl * pRecvMsg = 
		reinterpret_cast< _d_trade_close_inform_zocl * >( pi_pMsg );

	_GetPlayerTradeBoard()->CancelTrade();
	_SetSystemMsg( "거래에 오류가 있습니다. 거래를 종료합니다." );	
}