////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_ItemTrade Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../../Character/Avatar/CAvatar.h"
#include "../../../DataFile.h"
#include "../../../DataTable/CItemDataMgr.h"

#include "../../../Item/CItemData.h"
#include "../../../GameObject/Board/Common/CBoardManager.h"
#include "../../../GameObject/Manager/CCharacterMgr.h"

#include "../../../Character/Npc/CNpc.h"
#include "../../../GameProgress/CGP_MainGame.h"

#include "CNetMsgProcessor_ItemTrade.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_ItemTrade::CNetMsgProcessor_ItemTrade()
{

}

CNetMsgProcessor_ItemTrade::~CNetMsgProcessor_ItemTrade()
{

}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetMsgProcessor_ItemTrade::SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	return CNetMsgProcessor_Base::SendNetMessage( trade_msg, pi_byMsgIndex, pi_pMsg, pi_nSize );
}
			
bool	
CNetMsgProcessor_ItemTrade::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	if( pMsgHeader->m_byType[0] != trade_msg )
		return false;

	switch( pMsgHeader->m_byType[1] )
	{		
		case store_list_result_zocl		:
			StoreListResult_clzo( pi_pMsg );
			break;

		case buy_store_success_zocl		:
			BuyStoreSuccess_zocl( pi_pMsg );
			break;

		case buy_store_failure_zocl		:
			BuyStoreFailure_zocl( pi_pMsg );
			break;

		case sell_store_result_zocl		:
			SellStoreResult_zocl( pi_pMsg );
			break;

		case repair_result_zocl			:
			RepairResult_zocl( pi_pMsg );
			break;

		case exchange_money_result_zocl	:
			ExchangeMoneyResult_zocl( pi_pMsg );
			break;

		case economy_history_inform_zocl	:
			EconomyHistoryInform_zocl( pi_pMsg );
			break;

		case economy_rate_inform_zocl	:
			EconomyRateInform_zocl( pi_pMsg );
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

// 맵내에 존재하는 상인의 리스트를 가져온다.
BOOL
CNetMsgProcessor_ItemTrade::StoreListRequest_clzo( void )
{
	return SendNetMessage( store_list_request_clzo,
						   NULL,
						   0 );
}

void
CNetMsgProcessor_ItemTrade::StoreListResult_clzo( char * pi_pMsg )
{
	_store_list_result_zocl * pRecvMsg =
		reinterpret_cast< _store_list_result_zocl * >( pi_pMsg );


	CNpc *				l_pNpc		= NULL;
	DT_CNMInfo *		l_pNpcInfo	= NULL;
	CMerchantItemMgr *	l_pItemList = NULL;
	CBoardItem *		l_pItem		= NULL;

	for( int i = 0; i < pRecvMsg->byStoreNum; ++i )
	{
		l_pNpc = _GetCharMgr()->AddNpc( pRecvMsg->StorePos[i].wStoreIndex );

		if( l_pNpc )
		{
			//------------------------------------------------------------------
			// ID 세팅
			l_pNpc->SetIndex( pRecvMsg->StorePos[i].wStoreIndex );
			l_pNpc->SetObjectTypeID( OTI_CHAR );
			l_pNpc->SetCharTypeID( CTI_NPC );
			l_pNpc->SetDTIndex( pRecvMsg->StorePos[i].wStoreIndex );	// 테이블 레코드 인덱스

			//------------------------------------------------------------------
			// 본과 메시 설정
			l_pNpcInfo = ( DT_CNMInfo * )_GetCharDataMgr()->GetCharacter( CTI_NPC, pRecvMsg->StorePos[i].wStoreIndex );
			if( !l_pNpcInfo )
			{
				_GetCharMgr()->DeleteCharacter( CTI_NPC, i );
				continue;
			}

			l_pNpc->SetData( l_pNpcInfo );
			l_pNpc->SetBone( l_pNpcInfo->dwDTCode );
			l_pNpc->SetMesh( l_pNpcInfo->dwDTCode );
			l_pNpc->SetAction( CAI_MOVE_STOP );

			//------------------------------------------------------------------
			// 기타
			if( l_pNpcInfo->pName[0] != '\0' )
				l_pNpc->SetName( l_pNpcInfo->pName );
			else
				l_pNpc->SetName( "Unknown NPC" );

			l_pNpc->SetPosition( pRecvMsg->StorePos[i].fPos );
			l_pNpc->SetRotY( l_pNpcInfo->fYAngle, FALSE );

			//------------------------------------------------------------------
			// 상점 아이템 세팅
			// 중고 상점은 아이템 세팅이 따로 필요하지 않다.
			l_pItemList = l_pNpc->GetShopItemList();
			if( !l_pItemList )
				continue;

			for( int j = 0; j < l_pNpcInfo->dwMaxItemNum; ++j )
			{
				l_pItem = l_pItemList->AddItem( j );

				int l_nDTCode = l_pNpcInfo->pItemList[j].dwDTCode;

				if( !_GetItemDataMgr()->GetDataByDTCode( l_pNpcInfo->pItemList[j].byItemType,
														 l_nDTCode, l_pItem, FALSE ) )
					continue;

				l_pItem->SetIndex( j );
				//l_pItem->SetMaxQuantity( l_pNpcInfo->pItemList[j].dwMaxNum );
				l_pItem->SetCurMaxDurability( l_pItem->GetOrgMaxDurability() );
				l_pItem->SetDurability( l_pItem->GetOrgMaxDurability() );
			}
		}
	}

	// 메인 게임 시작
	//m_bIsConnectedToServer[GST_ZONE] = TRUE;
	_GetMainGame()->SetCompleteDataDownload( TRUE );	
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 상점의 아이템 구입 요청
BOOL	
CNetMsgProcessor_ItemTrade::BuyStoreRequest_clzo( void )
{	
	_buy_store_request_clzo stSendMsg;

	stSendMsg.byStoreIndex	= _GetShopTradeBoard()->GetTradeCharIndex();	
	stSendMsg.byBuyNum		= 0;

	CTradeInfoList * pTradeInfoList = _GetShopTradeBoard()->GetTradeInfoList();
	CTradeInfoList_Iterator it;	
	for( it = pTradeInfoList->begin(); it != pTradeInfoList->end(); ++it )
	{
		stSendMsg.OfferList[stSendMsg.byBuyNum].byStorageCode	= GetStoragePos( (*it)->m_byBoardType, (*it)->m_clsItem.GetItemKind() );				
		stSendMsg.OfferList[stSendMsg.byBuyNum].dwGoodSerial	= (*it)->m_clsItem.GetIndex();
		stSendMsg.OfferList[stSendMsg.byBuyNum].byAmount		= (*it)->m_clsItem.GetQuantity();

		++stSendMsg.byBuyNum;
	}

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( buy_store_request_clzo,
						   &stSendMsg,
						   sizeof( _buy_store_request_clzo ) );
}

// 상점의 아이템 구입 요청에 대한 응답 ( 성공 )
void
CNetMsgProcessor_ItemTrade::BuyStoreSuccess_zocl( char * pi_pMsg )
{
	_buy_store_success_zocl * pRecvMsg = 
		reinterpret_cast< _buy_store_success_zocl * >( pi_pMsg );
	
	_GetGBM()->SetRequestProcessState( FALSE );


	CTradeInfoList * pTradeInfoList = _GetShopTradeBoard()->GetTradeInfoList();

	//--------------------------------------------------------------------------
	// 서버에 요청한 정보와 맞는지 검사한다.
	if( pTradeInfoList->size() != pRecvMsg->byBuyNum )
		return;

	if( ( _GetShopTradeBoard()->GetDalant() != pRecvMsg->dwConsumDanlant ) ||
		( _GetShopTradeBoard()->GetGold() != pRecvMsg->dwConsumGold ) )
	{
#ifdef _DEVELOPER_
		static char l_pBuf[256];
		sprintf( l_pBuf, "서버와 클라이언트의 금액이 틀립니다. ( 서버 : %d Dalant - %d Gold ) ( 클라이언트 : %d Dalant - %d Gold )",
				 pRecvMsg->dwConsumDanlant, pRecvMsg->dwConsumGold, _GetShopTradeBoard()->GetDalant(), _GetShopTradeBoard()->GetGold() );
		_SetSystemMsg( l_pBuf );
#endif // _DEVELOPER_

		_GetShopTradeBoard()->SetDalant( pRecvMsg->dwConsumDanlant );
		_GetShopTradeBoard()->SetGold( pRecvMsg->dwConsumGold );
	}

	//--------------------------------------------------------------------------
	// item 개개의 정보 확인 및 item index 설정
	int i;
	CTradeInfoList_Iterator it;	
	for( i=0, it = pTradeInfoList->begin(); it != pTradeInfoList->end(); ++it, ++i )
	{
		// setting item index
		(*it)->m_clsItem.SetIndex( pRecvMsg->OfferList[i].wSerial );
	}	

	_GetAvatar()->SetDalant( pRecvMsg->dwLeftDalant );
	_GetAvatar()->SetGold( pRecvMsg->dwLeftGold );

	//--------------------------------------------------------------------------

	_GetShopTradeBoard()->BuyItem_Result( TRUE );

}

// 상점의 아이템 구입 요청에 대한 응답 ( 실패 )
void
CNetMsgProcessor_ItemTrade::BuyStoreFailure_zocl( char * pi_pMsg )
{
	_buy_store_failure_zocl * pRecvMsg = 
		reinterpret_cast< _buy_store_failure_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	_GetAvatar()->SetDalant( pRecvMsg->dwDalant );
	_GetAvatar()->SetGold( pRecvMsg->dwGold );


	_GetShopTradeBoard()->BuyItem_Result( FALSE );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타의 아이템 판매 요청
BOOL
CNetMsgProcessor_ItemTrade::SellStoreRequest_clzo( void )
{
	_sell_store_request_clzo stSendMsg;	

	stSendMsg.wStoreIndex	= _GetShopTradeBoard()->GetTradeCharIndex();
	
	stSendMsg.bySellNum = 0;
	
	CTradeInfoList * pTradeInfoList = _GetShopTradeBoard()->GetTradeInfoList();
	CTradeInfoList_Iterator it;	
	for( it = pTradeInfoList->begin(); it != pTradeInfoList->end(); ++it )
	{
		stSendMsg.Item[stSendMsg.bySellNum].byStorageCode	= GetStoragePos( (*it)->m_byBoardType, (*it)->m_clsItem.GetItemKind() );
		stSendMsg.Item[stSendMsg.bySellNum].wSerial			= (*it)->m_clsItem.GetIndex();
		stSendMsg.Item[stSendMsg.bySellNum].byAmount		= (*it)->m_clsItem.GetQuantity();

		++stSendMsg.bySellNum;
	}	

	_GetGBM()->SetRequestProcessState( TRUE );


	return SendNetMessage( sell_store_request_clzo,
						   &stSendMsg,
						   sizeof( _sell_store_request_clzo ) );
	
}

// 아바타의 아이템 판매 요청에 대한 응답
void	
CNetMsgProcessor_ItemTrade::SellStoreResult_zocl( char * pi_pMsg )
{
	_sell_store_result_zocl * pRecvMsg = 
		reinterpret_cast< _sell_store_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->bSucc == TRUE )
	{
		//--------------------------------------------------------------------------
		// 서버에 요청한 정보와 맞는지 검사한다.		

		if( ( _GetShopTradeBoard()->GetDalant() != pRecvMsg->dwProfitDanlant ) ||
			( _GetShopTradeBoard()->GetGold() != pRecvMsg->dwProfitGold ) )
		{
#ifdef _DEVELOPER_
			static char l_pBuf[256];
			sprintf( l_pBuf, "서버와 클라이언트의 금액이 틀립니다. ( 서버 : %d Dalant - %d Gold ) ( 클라이언트 : %d Dalant - %d Gold )",
					 pRecvMsg->dwProfitDanlant, pRecvMsg->dwProfitGold, _GetShopTradeBoard()->GetDalant(), _GetShopTradeBoard()->GetGold() );
			_SetSystemMsg( l_pBuf );
#endif // _DEVELOPER_

			_GetShopTradeBoard()->SetDalant( pRecvMsg->dwProfitDanlant );
			_GetShopTradeBoard()->SetGold( pRecvMsg->dwProfitGold );
		}

		//--------------------------------------------------------------------------

		_GetAvatar()->SetGold( pRecvMsg->dwLeftGold );
		_GetAvatar()->SetDalant( pRecvMsg->dwLeftDalant );

		_GetShopTradeBoard()->SellItem_Result( TRUE );
	}
	else
	{
		_GetShopTradeBoard()->SellItem_Result( FALSE );

		pRecvMsg->szErrCode[pRecvMsg->byErrCodeLen] = 0;
		_GUIMessageBox( NULL, pRecvMsg->szErrCode, 0xAAFFFFFF );
	}
		
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타의 아이템 수리 요청
BOOL
CNetMsgProcessor_ItemTrade::RepairRequest_clzo( void )
{
	_repair_request_clzo stSendMsg;	


	stSendMsg.byStoreIndex	= _GetShopTradeBoard()->GetTradeCharIndex();	
	
	stSendMsg.byItemNum = 0;

	CTradeInfoList * pTradeInfoList = _GetShopTradeBoard()->GetTradeInfoList();
	CTradeInfoList_Iterator it;	
	for( it = pTradeInfoList->begin(); it != pTradeInfoList->end(); ++it )
	{
		stSendMsg.Item[stSendMsg.byItemNum].byPositon	= GetStoragePos( (*it)->m_byBoardType, (*it)->m_clsItem.GetItemKind() );			
		stSendMsg.Item[stSendMsg.byItemNum].wSerial		= (*it)->m_clsItem.GetIndex();

		++stSendMsg.byItemNum;
	}

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( repair_request_clzo,
						   &stSendMsg,
						   sizeof( _repair_request_clzo ) );	
}

// 아바타의 아이템 수리 요청에 대한 응답
void	
CNetMsgProcessor_ItemTrade::RepairResult_zocl( char * pi_pMsg )
{
	_repair_result_zocl * pRecvMsg = 
		reinterpret_cast< _repair_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );	

	if( pRecvMsg->bSucc )
	{
		//--------------------------------------------------------------------------
		// 서버에 요청한 정보와 맞는지 검사한다.
		if( ( _GetShopTradeBoard()->GetDalant() != pRecvMsg->dwConsumDanlant ) ||
			( _GetShopTradeBoard()->GetGold() != pRecvMsg->dwConsumGold ) )
		{
#ifdef _DEVELOPER_
			static char l_pBuf[256];
			sprintf( l_pBuf, "서버와 클라이언트의 금액이 틀립니다. ( 서버 : %d Dalant - %d Gold ) ( 클라이언트 : %d Dalant - %d Gold )",
					 pRecvMsg->dwConsumDanlant, pRecvMsg->dwConsumGold, _GetShopTradeBoard()->GetDalant(), _GetShopTradeBoard()->GetGold() );
			_SetSystemMsg( l_pBuf );
#endif // _DEVELOPER_

			_GetShopTradeBoard()->SetDalant( pRecvMsg->dwConsumDanlant );
			_GetShopTradeBoard()->SetGold( pRecvMsg->dwConsumGold );
		}

		//--------------------------------------------------------------------------
		// 기타 세팅
		_GetAvatar()->SetGold( pRecvMsg->dwLeftGold );
		_GetAvatar()->SetDalant( pRecvMsg->dwLeftDalant );

		_GetShopTradeBoard()->RepairItem_Result( TRUE );
	}
	else
	{
		_GetShopTradeBoard()->RepairItem_Result( FALSE );

		pRecvMsg->szErrCode[pRecvMsg->byErrCodeLen] = 0;
		_GUIMessageBox( _GetShopWindow(), pRecvMsg->szErrCode, 0xAAFFFFFF );
	}	
}

// 환전 요청 ( 달란트 => 골드 )
BOOL	
CNetMsgProcessor_ItemTrade::ExchangeDalantForGoldRequest_clzo( DWORD pi_dwDalant )
{
	_exchange_dalant_for_gold_request_clzo stSendMsg;

	stSendMsg.dwDalant = pi_dwDalant;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( exchange_dalant_for_gold_request_clzo,
						   &stSendMsg,
						   sizeof( _exchange_dalant_for_gold_request_clzo ) );			
}

// 환전 요청 ( 골드 => 달란트 )
BOOL
CNetMsgProcessor_ItemTrade::ExchangeGoldForDalantRequest_clzo( DWORD pi_dwGold )
{
	_exchange_gold_for_dalant_request_clzo stSendMsg;

	stSendMsg.dwGold = pi_dwGold;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( exchange_gold_for_dalant_request_clzo,
						   &stSendMsg,
						   sizeof( _exchange_gold_for_dalant_request_clzo ) );		
}

// 환전에 대한 결과
void
CNetMsgProcessor_ItemTrade::ExchangeMoneyResult_zocl( char * pi_pMsg )
{
	_exchange_money_result_zocl * pRecvMsg = 
		reinterpret_cast< _exchange_money_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_GetExchangeBoard()->ExchangeMoney_Result( pRecvMsg->dwLeftDalant, pRecvMsg->dwLeftGold );		
	}
	// 실패
	else
	{
		_GetExchangeBoard()->ExchangeMoney_Result( 0, 0 );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNetMsgProcessor_ItemTrade::EconomyHistoryInform_zocl( char * pi_pMsg )
{
	_economy_history_inform_zocl * pRecvMsg = reinterpret_cast< _economy_history_inform_zocl * >( pi_pMsg );

	for( int i = 0; i < RACE_NUM; ++i )
		_GetExchangeWindow()->InitEconomyRatePoint( i, MAX_ECONOMY_HISTORY, pRecvMsg->wEconomyGuide[i] );

	/*
	#define economy_history_inform_zocl 15	//	경제지표히스토리(지급환율) 접속시에 한번 보내줌
	struct _economy_history_inform_zocl {	//upt(4/19).. 
		WORD wEconomyGuide[RACE_NUM][MAX_ECONOMY_HISTORY];	//0~11.. 0: 과거..11: 최근	//
	};*/
}

void
CNetMsgProcessor_ItemTrade::EconomyRateInform_zocl( char * pi_pMsg )
{
	_economy_rate_inform_zocl * pRecvMsg = reinterpret_cast< _economy_rate_inform_zocl * >( pi_pMsg );

	_GetAvatar()->SetOrgTexRate( pRecvMsg->fTexRate );
	_GetAvatar()->SetExchangeRate( pRecvMsg->fPayExgRate );
	_GetAvatar()->SetExchangeManageValue( pRecvMsg->wMgrValue );

	if( !pRecvMsg->bStart )
	{
		for( int i = 0; i < RACE_NUM; ++i )
			_GetExchangeWindow()->InsertEconomyRatePoint( i, pRecvMsg->wEconomyGuide[i] );
	}

	/*
	#define economy_rate_inform_zocl 16	//	경제지표(지급환율) 접속시와 변경될때마다 3종족..
	struct _economy_rate_inform_zocl {	
		bool	bStart;	//true면 첨 게임 시작시 받는것으로 wEconomyGuide는 적용하지 않는다.. false일경우만 아래꺼 적용해서 history에 추가..
		float	fPayExgRate;
		float	fTexRate;
		WORD	wMgrValue;	//관리값	//add(4/22)
		WORD	wEconomyGuide[RACE_NUM];	//upt(4/19)

		int size()	{	if(bStart) return sizeof(*this)-sizeof(wEconomyGuide); return sizeof(*this);	}
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void	
CNetMsgProcessor_ItemTrade::ShowServerErrorMessage( DWORD pi_dwErrorCode )
{

}