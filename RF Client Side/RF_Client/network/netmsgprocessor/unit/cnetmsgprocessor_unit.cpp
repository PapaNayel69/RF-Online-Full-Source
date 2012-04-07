////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Unit Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../../../GameObject/Manager/CCharacterMgr.h"
#include "../../../DataFile.h"
#include "../../../Item/CItemData.h"
#include "../../../DataTable/CItemDataMgr.h"
#include "../../../GameObject/Board/Common/CBoardManager.h"

#include "CNetMsgProcessor_Unit.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_Unit::CNetMsgProcessor_Unit()
{
}

CNetMsgProcessor_Unit::~CNetMsgProcessor_Unit()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetMsgProcessor_Unit::SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	return CNetMsgProcessor_Base::SendNetMessage( unit_msg, pi_byMsgIndex, pi_pMsg, pi_nSize );
}

bool
CNetMsgProcessor_Unit::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	if( pMsgHeader->m_byType[0] != unit_msg )
		return false;

	switch( pMsgHeader->m_byType[1] )
	{
		case unit_frame_buy_result_zocl :
			UnitFrameBuyResult_zocl( pi_pMsg );
			break;

		case unit_sell_result_zocl :
			UnitSellResult_zocl( pi_pMsg );
			break;

		case unit_part_tuning_result_zocl :
			UnitPartTuningResult_zocl( pi_pMsg );
			break;

		case unit_bullet_fill_result_zocl :
			UnitBulletFillResult_zocl( pi_pMsg );
			break;

		case unit_pack_fill_result_zocl :
			UnitPackFillResult_zocl( pi_pMsg );
			break;

		case unit_frame_repair_result_zocl :
			UnitFrameRepairResult_zocl( pi_pMsg );
			break;

		case unit_delivery_result_zocl :
			UnitDeliveryResult_zocl( pi_pMsg );
			break;

		case unit_return_result_zocl :
			UnitReturnResult_zocl( pi_pMsg );
			break;

		case unit_take_result_zocl :
			UnitTakeResult_zocl( pi_pMsg );
			break;

		case unit_leave_result_zocl :
			UnitLeaveResult_zocl( pi_pMsg );
			break;

		case unit_alter_fee_inform_zocl :
			UnitAlterFeeInform_zocl( pi_pMsg );
			break;

		case unit_booster_charge_inform_zocl :
			UnitBoosterChargeInform_zocl( pi_pMsg );
			break;

		case unit_destroy_inform_zocl :
			UnitDestroyInform_zocl( pi_pMsg );
			break;

		case unit_force_return_inform_zocl :
			UnitForceReturnInform_zocl( pi_pMsg );
			break;

		default :
			m_pNetworkMgr->NetLogWrite( "Unknown Message : CNetMsgProcessor_Unit::AnalysisServerPacket()\n" );
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 유닛 프레임 구입 요청
BOOL
CNetMsgProcessor_Unit::UnitFrameBuyRequest_clzo( DWORD pi_dwUnitDTIndex )
{
	_unit_frame_buy_request_clzo stSendMsg;

	stSendMsg.byFrameCode	= pi_dwUnitDTIndex;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( unit_frame_buy_request_clzo,
						   &stSendMsg,
						   sizeof( _unit_frame_buy_request_clzo ) );

	/*
	#define unit_frame_buy_request_clzo 1
	struct _unit_frame_buy_request_clzo {	//add(2/9)
		BYTE byFrameCode;
	};*/
}

// 유닛 프레임 구입 결과
void
CNetMsgProcessor_Unit::UnitFrameBuyResult_zocl( char * pi_pMsg )
{
	_unit_frame_buy_result_zocl * pRecvMsg = reinterpret_cast< _unit_frame_buy_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byRetCode == 0 )
	{
		// create unit
		CUnitItem clNewUnitItem;
		if( !clNewUnitItem.CreateByFrameType( pRecvMsg->byFrameCode ) )
			return;

		clNewUnitItem.SetIndex( pRecvMsg->wKeySerial );
		clNewUnitItem.SetServerIndex( pRecvMsg->byKeyIndex );

		_GetUnitStorehouseBoard()->AddUnit( pRecvMsg->byAddUnitSlot, &clNewUnitItem );

		//----------------------------------------------------------------------

		// Set Shop board
		if( ( _GetShopTradeBoard()->GetDalant() != pRecvMsg->dwConsumMoney[IMT_DALANT] ) ||
			( _GetShopTradeBoard()->GetGold() != pRecvMsg->dwConsumMoney[IMT_GOLD] ) )
		{
		#ifdef _DEVELOPER_
			_SetSystemMsg( "서버와 클라이언트의 금액이 틀립니다. ( 서버 : %d Dalant - %d Gold ) ( 클라이언트 : %d Dalant - %d Gold )",
						   pRecvMsg->dwConsumMoney[IMT_DALANT], pRecvMsg->dwConsumMoney[IMT_GOLD], _GetShopTradeBoard()->GetDalant(), _GetShopTradeBoard()->GetGold() );
		#endif
		}

		// set item index
		CTradeInfoList* pTradeInfoList	= _GetShopTradeBoard()->GetTradeInfoList();
		TRADE_INFO *	pTradeItem		= NULL;

		if( !pTradeInfoList->empty() )
			pTradeItem = pTradeInfoList->front();

		if( pTradeItem )
			pTradeItem->m_clsItem.SetIndex( pRecvMsg->wKeySerial );

		// set money
		_GetShopTradeBoard()->SetDalant( pRecvMsg->dwConsumMoney[IMT_DALANT] );
		_GetShopTradeBoard()->SetGold( pRecvMsg->dwConsumMoney[IMT_GOLD] );

		// process result
		_GetShopTradeBoard()->BuyItem_Result( TRUE );

		//----------------------------------------------------------------------

		// Set Avatar
		_GetAvatar()->SetDalant( pRecvMsg->dwLeftMoney[IMT_DALANT] );
		_GetAvatar()->SetGold( pRecvMsg->dwLeftMoney[IMT_GOLD] );
	}
	else
	{
		ShowServerErrorMessage( pRecvMsg->byRetCode );

		_GetShopTradeBoard()->BuyItem_Result( FALSE );
	}

	/*
	#define unit_frame_buy_result_zocl 2
	struct _unit_frame_buy_result_zocl {	//add(2/9)
		BYTE    byRetCode;
		BYTE	byFrameCode;
		BYTE	byAddUnitSlot;
		BYTE	byKeyIndex;
		WORD	wKeySerial;

		DWORD	dwLeftMoney[MONEY_UNIT_NUM];
		DWORD	dwConsumMoney[MONEY_UNIT_NUM];
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 유닛 프레임 판매 요청
BOOL
CNetMsgProcessor_Unit::UnitSellRequest_clzo( BYTE pi_bySlotIndex )
{
	_unit_sell_request_clzo stSendMsg;

	stSendMsg.bySlotIndex	= pi_bySlotIndex;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( unit_sell_request_clzo,
						   &stSendMsg,
						   sizeof( _unit_sell_request_clzo ) );

	/*
	#define unit_sell_request_clzo 11
	struct _unit_sell_request_clzo {	//add(2/9)
		BYTE bySlotIndex;
	};*/
}

// 유닛 프레임 판매 결과
void
CNetMsgProcessor_Unit::UnitSellResult_zocl( char * pi_pMsg )
{
	_unit_sell_result_zocl * pRecvMsg = reinterpret_cast< _unit_sell_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byRetCode == 0 )
	{
		// remove unit
		_GetUnitStorehouseBoard()->RemoveUnit( pRecvMsg->bySlotIndex );

		//----------------------------------------------------------------------

		// Set Shop board
		if( ( _GetShopTradeBoard()->GetDalant() != pRecvMsg->dwAddMoney[IMT_DALANT] ) ||
			( _GetShopTradeBoard()->GetGold() != pRecvMsg->dwAddMoney[IMT_GOLD] ) )
		{
		#ifdef _DEVELOPER_
			_SetSystemMsg( "서버와 클라이언트의 금액이 틀립니다. ( 서버 : %d Dalant - %d Gold ) ( 클라이언트 : %d Dalant - %d Gold )",
						   pRecvMsg->dwAddMoney[IMT_DALANT], pRecvMsg->dwAddMoney[IMT_GOLD], _GetShopTradeBoard()->GetDalant(), _GetShopTradeBoard()->GetGold() );
		#endif
		}

		// set money
		_GetShopTradeBoard()->SetDalant( pRecvMsg->dwAddMoney[IMT_DALANT] );
		_GetShopTradeBoard()->SetGold( pRecvMsg->dwAddMoney[IMT_GOLD] );

		// process result
		_GetShopTradeBoard()->SellItem_Result( TRUE );

		//----------------------------------------------------------------------

		// Set Avatar
		_GetAvatar()->SetDalant( pRecvMsg->dwLeftMoney[IMT_DALANT] );
		_GetAvatar()->SetGold( pRecvMsg->dwLeftMoney[IMT_GOLD] );
	}
	else
	{
		ShowServerErrorMessage( pRecvMsg->byRetCode );

		_GetShopTradeBoard()->SellItem_Result( FALSE );
	}

	/*
	#define unit_sell_result_zocl 22
	struct _unit_sell_result_zocl {	//add(2/9)
		BYTE    byRetCode;
		BYTE	bySlotIndex;
		WORD	wKeySerial;

		DWORD	dwNonPayDalant;
		DWORD	dwAddMoney[MONEY_UNIT_NUM];
		DWORD	dwLeftMoney[MONEY_UNIT_NUM];
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 파츠 튜닝 요청
BOOL
CNetMsgProcessor_Unit::UnitPartTuningRequest_clzo( void )
{
	_unit_part_tuning_request_clzo stSendMsg;

	CUnitItem *	pCurUnit	= _GetUnitStorehouseBoard()->GetCurUnit();
	CUnitItem *	pTuningUnit	= _GetUnitTuningBoard()->GetTuningUnit();
	if( !pCurUnit || !pTuningUnit )
		return FALSE;

	stSendMsg.bySlotIndex	= _GetUnitStorehouseBoard()->GetCurUnitSocketNo();
	stSendMsg.byTuningNum	= 0;

	CUnitPartsItem * pCurParts		= NULL;
	CUnitPartsItem * pTuningParts	= NULL;
	for( int i = 0; i < MAX_UNIT_PART_TYPE; ++i )
	{
		pCurParts		= pCurUnit->GetParts( IEPT_UNIT_HELMET + i );
		pTuningParts	= pTuningUnit->GetParts( IEPT_UNIT_HELMET + i );
		if( !pTuningParts || pTuningParts->IsEmpty() )
			continue;

		if( pTuningParts->IsSameKind( pCurParts ) &&
			pTuningParts->GetDurability() == pCurParts->GetDurability() )
			continue;

		stSendMsg.TuningList[stSendMsg.byTuningNum].byPartCode	= pTuningParts->GetUnitPartType();
		stSendMsg.TuningList[stSendMsg.byTuningNum].byPartIndex	= pTuningParts->GetDTIndex();
		++stSendMsg.byTuningNum;
	}

	if( stSendMsg.byTuningNum == 0 )
		return FALSE;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( unit_part_tuning_request_clzo,
						   &stSendMsg,
						   stSendMsg.size() );

	/*
	#define unit_part_tuning_request_clzo 3
	struct _unit_part_tuning_request_clzo {	//add(2/9)
		BYTE	bySlotIndex;

		BYTE    byTuningNum;
		_tuning_data	TuningList[UNIT_PART_NUM];

		_unit_part_tuning_request_clzo(){
			byTuningNum = 0;
		}

		int size(){
			return sizeof(*this)-sizeof(_tuning_data)*(UNIT_PART_NUM-byTuningNum);
		}
	};*/
}

// 파츠 튜닝 결과
void
CNetMsgProcessor_Unit::UnitPartTuningResult_zocl( char * pi_pMsg )
{
	_unit_part_tuning_result_zocl * pRecvMsg = reinterpret_cast< _unit_part_tuning_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byRetCode == 0 )
	{
		_GetAvatar()->SetDalant( pRecvMsg->dwLeftMoney[IMT_DALANT] );
		_GetAvatar()->SetGold( pRecvMsg->dwLeftMoney[IMT_GOLD] );

		_GetUnitTuningBoard()->UI_TuningResult( TRUE );
	}
	else
	{
		_GetUnitTuningBoard()->UI_TuningResult( FALSE );

		ShowServerErrorMessage( pRecvMsg->byRetCode );
	}

	/*
	#define unit_part_tuning_result_zocl 4
	struct _unit_part_tuning_result_zocl {	//add(2/9)
		BYTE    byRetCode;
		BYTE	bySlotIndex;
		BYTE	byPart[UNIT_PART_NUM];

		DWORD	dwComsumMoney[MONEY_UNIT_NUM];
		DWORD	dwLeftMoney[MONEY_UNIT_NUM];
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CNetMsgProcessor_Unit::UnitBulletFillRequest_clzo( void )
{
	_unit_bullet_fill_request_clzo stSendMsg;

	CUnitItem *	pCurUnit	= _GetUnitStorehouseBoard()->GetCurUnit();
	CUnitItem *	pTuningUnit	= _GetUnitTuningBoard()->GetTuningUnit();
	if( !pCurUnit || !pTuningUnit )
		return FALSE;

	stSendMsg.bySlotIndex	= _GetUnitStorehouseBoard()->GetCurUnitSocketNo();

	CUnitPartsItem * pCurBullet		= NULL;
	CUnitPartsItem * pTuningBullet	= NULL;
	for( int i = 0; i < MAX_UNIT_BULLET_TYPE; ++i )
	{
		stSendMsg.byBulletIndex[i]	= 0xFF;

		pCurBullet		= pCurUnit->GetBullet( IEPT_UNIT_ARMS + i );
		pTuningBullet	= pTuningUnit->GetBullet( IEPT_UNIT_ARMS + i );
		if( !pTuningBullet || pTuningBullet->IsEmpty() )
			continue;

		if( pTuningBullet->IsSameKind( pCurBullet ) &&
			pTuningBullet->GetDurability() == pCurBullet->GetDurability() )
			continue;

		stSendMsg.byBulletIndex[i]	= pTuningBullet->GetDTIndex();
	}

	if( ( stSendMsg.byBulletIndex[0] == 0xFF ) && ( stSendMsg.byBulletIndex[1] == 0xFF ) )
		return FALSE;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( unit_bullet_fill_request_clzo,
						   &stSendMsg,
						   sizeof( _unit_bullet_fill_request_clzo ) );

	/*
	#define unit_bullet_fill_request_clzo 7
	struct _unit_bullet_fill_request_clzo {	//add(2/9)

		BYTE	bySlotIndex;
		
	//	#define unit_bullet_arms			0
	//	#define unit_bullet_shoulder		1
	//	#define UNIT_BULLET_NUM				2	

		BYTE	byBulletIndex[UNIT_BULLET_NUM];	//upt(3/16)	.. 채우지않은 파트는 0xFF
	};*/
}

void
CNetMsgProcessor_Unit::UnitBulletFillResult_zocl( char * pi_pMsg )
{
	_unit_bullet_fill_result_zocl * pRecvMsg = reinterpret_cast< _unit_bullet_fill_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byRetCode == 0 )
	{
		_GetAvatar()->SetDalant( pRecvMsg->dwLeftMoney[IMT_DALANT] );
		_GetAvatar()->SetGold( pRecvMsg->dwLeftMoney[IMT_GOLD] );

		_GetUnitTuningBoard()->UI_TuningResult( TRUE );
	}
	else
	{
		_GetUnitTuningBoard()->UI_TuningResult( FALSE );

		ShowServerErrorMessage( pRecvMsg->byRetCode );
	}

	/*
	#define unit_bullet_fill_result_zocl 8
	struct _unit_bullet_fill_result_zocl {	//add(2/9)
		BYTE    byRetCode;
		BYTE	bySlotIndex;
		bool	bFillBulletPart[UNIT_BULLET_NUM];	//upt(3/16)	채우지않은 파트는 0xFF

		DWORD	dwComsumMoney[MONEY_UNIT_NUM];
		DWORD	dwLeftMoney[MONEY_UNIT_NUM];*/
}

//==============================================================================]

BOOL
CNetMsgProcessor_Unit::UnitPackFillRequest_clzo( void )
{
	_unit_pack_fill_request_clzo stSendMsg;

	CUnitItem *	pCurUnit	= _GetUnitStorehouseBoard()->GetCurUnit();
	CUnitItem *	pTuningUnit	= _GetUnitTuningBoard()->GetTuningUnit();
	if( !pCurUnit || !pTuningUnit )
		return FALSE;

	stSendMsg.bySlotIndex	= _GetUnitStorehouseBoard()->GetCurUnitSocketNo();
	stSendMsg.byFillNum		= 0;

	CUnitPartsItem * pCurBullet		= NULL;
	CUnitPartsItem * pTuningBullet	= NULL;
	for( int i = 0; i < MAX_BACKPACK_BULLET_NUM; ++i )
	{
		pCurBullet		= pCurUnit->GetBackpackBullet( i );
		pTuningBullet	= pTuningUnit->GetBackpackBullet( i );
		if( !pTuningBullet || pTuningBullet->IsEmpty() )
			continue;

		if( pTuningBullet->IsSameKind( pCurBullet ) &&
			pTuningBullet->GetDurability() == pCurBullet->GetDurability() )
			continue;

		stSendMsg.List[stSendMsg.byFillNum].bySpareIndex	= i;
		stSendMsg.List[stSendMsg.byFillNum].byBulletIndex	= pTuningBullet->GetDTIndex();
		++stSendMsg.byFillNum;
	}

	if( stSendMsg.byFillNum <= 0 )
		return FALSE;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( unit_pack_fill_request_clzo,
						   &stSendMsg,
						   sizeof( _unit_pack_fill_request_clzo ) );

	/*
	#define unit_pack_fill_request_clzo 11
	struct _unit_pack_fill_request_clzo {	//add(2/9)
		BYTE	bySlotIndex;

		BYTE	byFillNum;

		struct __list{					//upt(3/16)
			BYTE	bySpareIndex;
			BYTE	byBulletIndex;
		};

		__list	List[max_unit_spare];

		_unit_pack_fill_request_clzo()	{	byFillNum = 0;	}

		int size()	{	return sizeof(*this) - sizeof(__list)*(max_unit_spare - byFillNum);	}
	};*/
}


void
CNetMsgProcessor_Unit::UnitPackFillResult_zocl( char * pi_pMsg )
{
	_unit_pack_fill_result_zocl * pRecvMsg = reinterpret_cast< _unit_pack_fill_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byRetCode == 0 )
	{
		_GetAvatar()->SetDalant( pRecvMsg->dwLeftMoney[IMT_DALANT] );
		_GetAvatar()->SetGold( pRecvMsg->dwLeftMoney[IMT_GOLD] );

		_GetUnitTuningBoard()->UI_TuningResult( TRUE );
	}
	else
	{
		_GetUnitTuningBoard()->UI_TuningResult( FALSE );

		ShowServerErrorMessage( pRecvMsg->byRetCode );
	}

	/*
	#define unit_pack_fill_result_zocl 12
	struct _unit_pack_fill_result_zocl {	//add(2/9)
		BYTE    byRetCode;
		BYTE	bySlotIndex;

		DWORD	dwComsumMoney[MONEY_UNIT_NUM];
		DWORD	dwLeftMoney[MONEY_UNIT_NUM];

		BYTE	byFillNum;

		typedef _unit_pack_fill_request_clzo::__list __list;

		__list	List[max_unit_spare];

		_unit_pack_fill_result_zocl()	{	byFillNum = 0;	}

		int size()	{	return sizeof(*this) - sizeof(__list)*(max_unit_spare - byFillNum);	}
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 기갑 장비 수리
BOOL
CNetMsgProcessor_Unit::UnitFrameRepairRequest_clzo( void )
{
	_unit_frame_repair_request_clzo stSendMsg;

	stSendMsg.bySlotIndex = _GetUnitStorehouseBoard()->GetCurUnitSocketNo();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( unit_frame_repair_request_clzo,
						   &stSendMsg,
						   sizeof( _unit_frame_repair_request_clzo ) );

	/*
	#define unit_frame_repair_request_clzo 5
	struct _unit_frame_repair_request_clzo {	//add(2/9)
		BYTE	bySlotIndex;
	};*/
}

void
CNetMsgProcessor_Unit::UnitFrameRepairResult_zocl( char * pi_pMsg )
{
	_unit_frame_repair_result_zocl * pRecvMsg = reinterpret_cast< _unit_frame_repair_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byRetCode == 0 )
	{
		_GetAvatar()->SetDalant( pRecvMsg->dwLeftDalant );
		_GetUnitTuningBoard()->UI_RepairResult( pRecvMsg->dwNewGauge );

		_SetSystemMsg( "기갑 장비를 수리하였습니다." );
	}
	else
	{
		ShowServerErrorMessage( pRecvMsg->byRetCode );
	}

	/*
	#define unit_frame_repair_result_zocl 6
	struct _unit_frame_repair_result_zocl {	
		BYTE    byRetCode;
		BYTE	bySlotIndex;
		DWORD	dwNewGauge;	//upt(5/2)

		DWORD	dwConsumDalant;	//수리비
		DWORD	dwLeftDalant;	//남은 달란트
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 기갑 장비를 창고에서 찾아온다.
BOOL
CNetMsgProcessor_Unit::UnitDeliveryRequest_clzo( DWORD pi_dwMerchantIndex )
{
	_unit_delivery_request_clzo stSendMsg;

	stSendMsg.bySlotIndex		= _GetUnitStorehouseBoard()->GetCurUnitSocketNo();
	stSendMsg.byStoreIndex		= pi_dwMerchantIndex;
	stSendMsg.zUnitNewPos[0]	= _GetAvatar()->GetPosX();
	stSendMsg.zUnitNewPos[1]	= _GetAvatar()->GetPosY();
	stSendMsg.zUnitNewPos[2]	= _GetAvatar()->GetPosZ();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( unit_delivery_request_clzo,
						   &stSendMsg,
						   sizeof( _unit_delivery_request_clzo ) );

	/*
	#define unit_delivery_request_clzo 13
	struct _unit_delivery_request_clzo {	//add(2/9)
		BYTE	bySlotIndex;
		BYTE	byStoreIndex;	//0xFF면 원거리수송
		short	zUnitNewPos[3];	//현재위치에서 20이니..
	};*/
}

// 기갑 장비를 창고에서 찾아온다.
void
CNetMsgProcessor_Unit::UnitDeliveryResult_zocl( char * pi_pMsg )
{
	_unit_delivery_result_zocl * pRecvMsg = reinterpret_cast< _unit_delivery_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byRetCode == 0 )
	{
		_GetUnitStorehouseBoard()->UI_DeliveryUnitResult();
		_GetAvatar()->SetDalant( pRecvMsg->dwLeftDalant );
	}
	else
	{
		ShowServerErrorMessage( pRecvMsg->byRetCode );
	}

	/*
	#define unit_delivery_result_zocl 14
	struct _unit_delivery_result_zocl {	//add(2/9)
		BYTE    byRetCode;
		BYTE	bySlotIndex;
		BYTE	byTransDistCode;	//#define unit_trans_short_range		0	//유닛 근거리수송  #define unit_trans_long_range		1	//유닛 원거리수송
		DWORD	dwParkingUnitSerial;

		DWORD	dwPayDalant;
		DWORD	dwLeftDalant;
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 기갑 장비를 창고에 보관
BOOL
CNetMsgProcessor_Unit::UnitReturnRequest_clzo( void )
{
	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( unit_return_request_clzo,
						   NULL,
						   0 );

	/*
	#define unit_return_request_clzo 15
	struct _unit_return_request_clzo {	//add(2/9)
	};*/
}

// 기갑 장비를 창고에 보관
void
CNetMsgProcessor_Unit::UnitReturnResult_zocl( char * pi_pMsg )
{
	_unit_return_result_zocl * pRecvMsg = reinterpret_cast< _unit_return_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byRetCode == 0 )
	{
		_GetUnitStorehouseBoard()->UI_KeepingUnitResult();
		_GetAvatar()->SetDalant( pRecvMsg->dwNewDalant );
	}
	else
	{
		ShowServerErrorMessage( pRecvMsg->byRetCode );
	}

	/*
	#define unit_return_result_zocl 16
	struct _unit_return_result_zocl {	//add(2/9)
		BYTE    byRetCode;
		DWORD	dwPayDalant;
		DWORD   dwNewDalant;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 유닛 탑승 요청
BOOL
CNetMsgProcessor_Unit::UnitTakeRequest_clzo( void )
{
	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( unit_take_request_clzo,
						   NULL,
						   0 );

	/*
	#define unit_take_request_clzo 17
	struct _unit_take_request_clzo {	//add(2/9)
	};*/
}

// 유닛 탑승 결과
void
CNetMsgProcessor_Unit::UnitTakeResult_zocl( char * pi_pMsg )
{
	_unit_take_result_zocl * pRecvMsg = reinterpret_cast< _unit_take_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byRetCode == 0 )
	{
		_GetAvatar()->MountUnitResult();
	}
	else
	{
		ShowServerErrorMessage( pRecvMsg->byRetCode );
	}

	/*
	#define unit_take_result_zocl 18
	struct _unit_take_result_zocl {	//add(2/9)
		BYTE    byRetCode;
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 유닛 해제 요청
BOOL
CNetMsgProcessor_Unit::UnitLeaveRequest_clzo( void )
{
	_unit_leave_request_clzo stSendMsg;

	stSendMsg.zPlayerNewPos[0]	= _GetAvatar()->GetPosX();
	stSendMsg.zPlayerNewPos[1]	= _GetAvatar()->GetPosY();
	stSendMsg.zPlayerNewPos[2]	= _GetAvatar()->GetPosZ();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( unit_leave_request_clzo,
						   &stSendMsg,
						   sizeof( _unit_leave_request_clzo ) );

	/*
	#define unit_leave_request_clzo 19
	struct _unit_leave_request_clzo {	//add(2/9)
		short	zPlayerNewPos[3];	//현재위치에서 20이니..
	};*/
}

// 유닛 해제 결과
void
CNetMsgProcessor_Unit::UnitLeaveResult_zocl( char * pi_pMsg )
{
	_unit_leave_result_zocl * pRecvMsg = reinterpret_cast< _unit_leave_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byRetCode == 0 )
	{
		_GetAvatar()->DismountUnitResult();
	}
	else
	{
		ShowServerErrorMessage( pRecvMsg->byRetCode );
	}

	/*
	#define unit_leave_result_zocl 20
	struct _unit_leave_result_zocl {	//add(2/9)
		BYTE    byRetCode;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 유닛의 부채 변경
void
CNetMsgProcessor_Unit::UnitAlterFeeInform_zocl( char * pi_pMsg )
{
	_unit_alter_fee_inform_zocl * pRecvMsg = reinterpret_cast< _unit_alter_fee_inform_zocl * >( pi_pMsg );

	CUnitItem * pUnitItem = _GetUnitStorehouseBoard()->GetUnit( pRecvMsg->bySlotIndex );
	if( pUnitItem )
		pUnitItem->SetTractionDebt( pRecvMsg->dwPullingFee );

	/*
	#define unit_alter_fee_inform_zocl 30
	struct _unit_alter_fee_inform_zocl {
		BYTE	bySlotIndex;
	//	DWORD	dwKeepingFee;	del(4/22)
		DWORD	dwPullingFee;
	};*/
}


// 부스터 차지량 변경
void
CNetMsgProcessor_Unit::UnitBoosterChargeInform_zocl( char * pi_pMsg )
{
	_unit_booster_charge_inform_zocl * pRecvMsg = reinterpret_cast< _unit_booster_charge_inform_zocl * >( pi_pMsg );

	// 아바타가 유닛에 탑승하고 있을 경우...
	if( _GetUnitStorehouseBoard()->GetCurUnitSocketNo() == pRecvMsg->bySlotIndex &&
		_GetAvatar()->IsEqualUnitMountState( UMS_MOUNT ) )
	{
		_GetAvatar()->SetSP( pRecvMsg->wBoosterGauge );
	}
	// 아바타가 유닛에 탑승하지 않았을 경우...
	else
	{
		CUnitItem * pUnitItem = _GetUnitStorehouseBoard()->GetUnit( pRecvMsg->bySlotIndex );
		if( pUnitItem )
			pUnitItem->SetCurrentBoosterPoint( pRecvMsg->wBoosterGauge );
	}

	// 부스터가 다 됐다면 걷기 모드로 변경
	if( pRecvMsg->wBoosterGauge <= 0 )
		_GetAvatar()->SetWalkMode( CMM_MOVE_WALK );

	/*
	#define unit_booster_charge_inform_zocl 34	//부스터 변경
	struct _unit_booster_charge_inform_zocl {
		BYTE	bySlotIndex;
		WORD	wBoosterGauge;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 기갑 장비 파괴
void
CNetMsgProcessor_Unit::UnitDestroyInform_zocl( char * pi_pMsg )
{
	_unit_destroy_inform_zocl * pRecvMsg = reinterpret_cast< _unit_destroy_inform_zocl * >( pi_pMsg );

	/*
	// 아바타의 클론 처리
	// 아바타를 복사해 character manager에 add 한다.
	CAvatar * pClone = static_cast< CAvatar * >( _GetAvatar()->Clone() );
	pClone->SetIndex( pClone->GetIndex() | 0xF0000000 );
	_GetCharMgr()->AddCharacter( CTI_PLAYER, pClone );

	pClone->SetAction( CAI_DEATH_READY );
	*/

	// 아바타 처리
	_GetAvatar()->DismountUnitResult();
	_GetUnitStorehouseBoard()->RemoveUnit( pRecvMsg->bySlotIndex );

	_SetSystemMsg( "기갑 장비가 파괴되었습니다." );

	/*
	#define unit_destroy_inform_zocl 35	//유닛파괴
	struct _unit_destroy_inform_zocl {		//add(4/24)	
		BYTE	bySlotIndex;
	};*/
}

// 기갑 장비 강제 견인
void
CNetMsgProcessor_Unit::UnitForceReturnInform_zocl( char * pi_pMsg )
{
	_unit_force_return_inform_zocl * pRecvMsg = reinterpret_cast< _unit_force_return_inform_zocl * >( pi_pMsg );

	_GetUnitStorehouseBoard()->UI_KeepingUnitResult();

	if( _GetAvatar()->GetDalant() >= pRecvMsg->dwDebt )
	{
		_GetAvatar()->SetDalant( _GetAvatar()->GetDalant() - pRecvMsg->dwDebt );
	}
	else
	{
		CUnitItem * pCurUnit = _GetUnitStorehouseBoard()->GetCurUnit();
		if( pCurUnit )
			pCurUnit->SetTractionDebt( pCurUnit->GetTractionDebt() + pRecvMsg->dwDebt );
	}

	_SetSystemMsg( "기갑 장비가 자동 견인 되었습니다. 견인료 %d 달란트가 지출되거나 채무로 남게됩니다.",
					pRecvMsg->dwDebt );

	/*
	#define unit_force_return_inform_zocl 36	//유닛강제 견인
	struct _unit_force_return_inform_zocl {		//add(4/26)	
		BYTE	bySlotIndex;
		DWORD	dwDebt;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNetMsgProcessor_Unit::ShowServerErrorMessage( DWORD pi_dwErrorCode )
{
	switch( pi_dwErrorCode )
	{
		case 0	:
		default	: _SetSystemMsg( "기갑장비 관련 에러 : 알 수 없는 에러 ( %d )", pi_dwErrorCode ); break;
	}
}

#endif // _NEW_UI_