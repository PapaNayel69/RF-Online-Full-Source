////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_ItemArrangement Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../../../Character/Avatar/CAvatar.h"
#include "../../../DataFile.h"
#include "../../../DataTable/CItemDataMgr.h"

#include "../../../Item/CItemData.h"
#include "../../../GameObject/Board/Common/CBoardManager.h"

#include "CNetMsgProcessor_ItemArrangement.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_ItemArrangement::CNetMsgProcessor_ItemArrangement()
{
}

CNetMsgProcessor_ItemArrangement::~CNetMsgProcessor_ItemArrangement()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetMsgProcessor_ItemArrangement::SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	return CNetMsgProcessor_Base::SendNetMessage( ui_msg, pi_byMsgIndex, pi_pMsg, pi_nSize );
}

bool
CNetMsgProcessor_ItemArrangement::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	if( pMsgHeader->m_byType[0] != ui_msg )
		return false;

	switch( pMsgHeader->m_byType[1] )
	{
		case potionsocket_separation_result_zocl:
			PotionsocketSeparationResult_zocl( pi_pMsg );
			break;

		case potionsocket_division_result_zocl	:
			PotionsocketDivisionResult_zocl( pi_pMsg );
			break;

		case res_separation_result_zocl			:
			ResSeparationResult_zocl( pi_pMsg );
			break;

		case res_division_result_zocl			:
			ResDivisionResult_zocl( pi_pMsg );
			break;

		case animus_inven_change_result_zocl	:
			AnimusInvenChangeResult_zocl( pi_pMsg );
			break;

		case potionsocket_change_result_zocl	:
			PotionsocketChangeResult_zocl( pi_pMsg );
			break;

		case force_inven_change_result_zocl		: 
			ForceInvenChangeResult_zocl( pi_pMsg );
			break;

		default :
			m_pNetworkMgr->NetLogWrite( "Unknown Message : CNetMsgProcessor_ItemArrangement::AnalysisServerPacket()\n" );
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 포션 나누기
BOOL
CNetMsgProcessor_ItemArrangement::PotionsocketSeparationRequest_clzo( void )
{
	_potionsocket_separation_request_clzo stSendMsg;

	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pHoverItem )
		return FALSE;

	stSendMsg.StartPos.byPositon	= _GetGBM()->GetOrgBoardType();
	stSendMsg.TargetPos.byPositon	= _GetGBM()->GetTarBoardType();

	stSendMsg.wStartSerial			= pHoverItem->GetIndex();
	stSendMsg.byMoveAmount			= pHoverItem->GetQuantity();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( potionsocket_separation_request_clzo,
						   &stSendMsg,
						   sizeof( _potionsocket_separation_request_clzo ) );

	/*
	#define potionsocket_separation_request_clzo 20	//inventory빈곳에 나눈다.
	struct _potionsocket_separation_request_clzo{
		struct _pos{
			BYTE byPositon;		//인벤토리: 0, 벨트: 1, 
			BYTE byBagIndex;	//인벤토리일 경우 가방번호
		};
		_pos StartPos;	
		_pos TargetPos;
		WORD wStartSerial;
		BYTE byMoveAmount;
	};*/
}

// 포션 나누기
void
CNetMsgProcessor_ItemArrangement::PotionsocketSeparationResult_zocl( char * pi_pMsg )
{
	_potionsocket_separation_result_zocl * pRecvMsg =
		reinterpret_cast< _potionsocket_separation_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->sErrorCode == 0 )
	{
		Item_Devide( pRecvMsg->wParentSerial, pRecvMsg->byParentAmount,
					 pRecvMsg->wChildSerial, pRecvMsg->byChildAmount );
	}
	else
	{
		if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )		
		{
			_GetInventoryBoard()->UI_AddResult( FALSE );
		}
		else if( _GetGBM()->GetTarBoardType() == IBT_ITEM_BELT )
		{
			_GetBeltBoard()->UI_AddResult( FALSE );
		}

		ShowServerErrorMessage( pRecvMsg->sErrorCode );
	}

	/*
	#define potionsocket_separation_result_zocl 21	
	struct _potionsocket_separation_result_zocl{
		char sErrorCode;	//0: 성공, -1: 갯수모자름 , -2: 전부이동함, -3: 포션이 아니다. -4: 남은 인벤토리가 없다, -5: 그런 시리얼은 없다
		WORD wParentSerial;
		BYTE byParentAmount;
		WORD wChildSerial;
		BYTE byChildAmount;
	};*/
}

void
CNetMsgProcessor_ItemArrangement::Item_Devide( WORD pi_wOrgIndex, BYTE pi_byOrgQuantity,
											   WORD pi_wTarIndex, BYTE pi_byTarQuantity )
{
	CBoardItem * pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );
	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pHoverItem )
		return;

	if( pOrgItem )
	{
		pOrgItem->SetIndex( pi_wOrgIndex );
		pOrgItem->SetQuantity( pi_byOrgQuantity );
	}

	pHoverItem->SetIndex( pi_wTarIndex );
	pHoverItem->SetQuantity( pi_byTarQuantity );

	
	if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )
	{
		_GetInventoryBoard()->UI_AddResult( TRUE );
	}
	else if( _GetGBM()->GetTarBoardType() == IBT_ITEM_BELT )
	{
		_GetBeltBoard()->UI_AddResult( TRUE );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 포션 합치기
BOOL
CNetMsgProcessor_ItemArrangement::PotionsocketDivisionRequest_clzo( void )
{
	_potionsocket_division_request_clzo stSendMsg;

	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );
	if( !pHoverItem || ! pTarItem )
		return FALSE;

	stSendMsg.StartPos.byPositon	= _GetGBM()->GetOrgBoardType();
	stSendMsg.TargetPos.byPositon	= _GetGBM()->GetTarBoardType();

	stSendMsg.wStartSerial			= pHoverItem->GetIndex();
	stSendMsg.wTarSerial			= pTarItem->GetIndex();
	stSendMsg.byMoveAmount			= pHoverItem->GetQuantity();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( potionsocket_division_request_clzo,
						   &stSendMsg,
						   sizeof( _potionsocket_division_request_clzo ) );

	/*
	#define potionsocket_division_request_clzo 22	//다른 inventory와 나눠서 병합시킨다.
	struct _potionsocket_division_request_clzo{	
		struct _pos{
			BYTE byPositon;	//인벤토리: 0, 벨트: 1, 
			BYTE byBagIndex;	//인벤토리일 경우 가방번호
		};
		_pos StartPos;	
		_pos TargetPos;
		WORD wStartSerial;	
		WORD wTarSerial;
		BYTE byMoveAmount;
	};*/
}

// 포션 합치기
void
CNetMsgProcessor_ItemArrangement::PotionsocketDivisionResult_zocl( char * pi_pMsg )
{
	_potionsocket_division_result_zocl * pRecvMsg =
		reinterpret_cast< _potionsocket_division_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->sErrorCode == 0 )
	{
		Item_Combine( pRecvMsg->wSerial, pRecvMsg->byParentAmount,
					  pRecvMsg->wChildSerial, pRecvMsg->byChildAmount );
	}
	else
	{
		if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )		
		{
			_GetInventoryBoard()->UI_AddResult( FALSE );
		}
		else if( _GetGBM()->GetTarBoardType() == IBT_ITEM_BELT )
		{
			_GetBeltBoard()->UI_AddResult( FALSE );
		}

		ShowServerErrorMessage( pRecvMsg->sErrorCode );
	}

	/*
	#define potionsocket_division_result_zocl 23	
	struct _potionsocket_division_result_zocl{
		char sErrorCode;	//0: 성공, -1: 갯수 모자름, -3: 포션이 아니다, -4: 더 많은 양을 이동, -5: 그런 시리얼은 없다, -6: 같은 종류의 포션이 아님.
		WORD wSerial;	//실패면 byParentAmount 부터 0xffff로 옴
		BYTE byParentAmount;	//0이면 슬롯 삭제
		WORD wChildSerial;	
		BYTE byChildAmount;
	};*/
}

void
CNetMsgProcessor_ItemArrangement::Item_Combine( WORD pi_wOrgIndex, BYTE pi_byOrgQuantity,
												WORD pi_wTarIndex, BYTE pi_byTarQuantity )
{
	CBoardItem * pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );
	CBoardItem * pHoverItem;
	if( _GetGBM()->GetHoverObject() )
		pHoverItem = _GetGBM()->GetHoverItem();
	else
		pHoverItem = _GetGBM()->GetDeletedHoverItem();

	if( !pTarItem || !pHoverItem )
		return;

	if( pOrgItem )
		pOrgItem->SetQuantity( pi_byOrgQuantity );
	
	pHoverItem->SetQuantity( pi_byTarQuantity - pTarItem->GetQuantity() );	

	if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )		
	{
		_GetInventoryBoard()->UI_AddResult( TRUE );
	}
	else if( _GetGBM()->GetTarBoardType() == IBT_ITEM_BELT )
	{
		_GetBeltBoard()->UI_AddResult( TRUE );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 벨트 -> 인벤토리, 인벤토리 -> 벨트 로 아이템 모두를 옮길 때 
BOOL	
CNetMsgProcessor_ItemArrangement::PotionsocketChangeRequest_zocl( void )
{
	_potionsocket_change_request_clzo stSendMsg;

	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pHoverItem )
		return FALSE;
	
	stSendMsg.Item.byStorageCode	= _GetGBM()->GetOrgBoardType();
	stSendMsg.Item.wItemSerial		= pHoverItem->GetIndex();	
	stSendMsg.byTargetStorageCode	= _GetGBM()->GetTarBoardType();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( potionsocket_change_request_clzo,
						   &stSendMsg,
						   sizeof( _potionsocket_change_request_clzo ) );

	/*
	#define potionsocket_change_request_clzo 15
	struct _potionsocket_change_request_clzo{
		_STORAGE_POS_INDIV Item;
	//	_STORAGE_POS_INDIV TarPos;//빈칸으로 이동은  _STORAGE_POS_INDIV::wItemSerial = 0xFFFF..
		BYTE byTargetStorageCode;	//upt(4/24).. 무조건 빈칸으로 이동..
	};*/
}

void
CNetMsgProcessor_ItemArrangement::PotionsocketChangeResult_zocl( char * pi_pMsg )
{
	_potionsocket_change_result_zocl * pRecvMsg =
		reinterpret_cast< _potionsocket_change_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byErrCode == 0 )
	{
		Item_SocketChange();
	}
	else
	{
		if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )		
		{
			_GetInventoryBoard()->UI_AddResult( FALSE );
		}
		else if( _GetGBM()->GetTarBoardType() == IBT_ITEM_BELT )
		{
			_GetBeltBoard()->UI_AddResult( FALSE );
		}

		ShowServerErrorMessage( pRecvMsg->byErrCode );
	}	
}

void
CNetMsgProcessor_ItemArrangement::Item_SocketChange( void )
{
	CBoardItem * pOrgItem	= static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );
	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pHoverItem )
		return;

	if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )
	{
		_GetInventoryBoard()->UI_AddResult( TRUE );
	}
	else if( _GetGBM()->GetTarBoardType() == IBT_ITEM_BELT )
	{
		_GetBeltBoard()->UI_AddResult( TRUE );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 포션을 제외한 splitable item 나누기
BOOL
CNetMsgProcessor_ItemArrangement::ResSeparationRequest_clzo( void )
{
	_res_separation_request_clzo stSendMsg;

	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pHoverItem )
		return FALSE;

	stSendMsg.wStartSerial	= pHoverItem->GetIndex();
	stSendMsg.byMoveAmount	= pHoverItem->GetQuantity();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( res_separation_request_clzo,
						   &stSendMsg,
						   sizeof( _res_separation_request_clzo ) );
	/*
	#define res_separation_request_clzo 14	//inventory빈곳에 나눈다.
	struct _res_separation_request_clzo{		//add(1/23)

		BYTE byStartBagIndex;	
		BYTE byTargetBagIndex;
		WORD wStartSerial;
		BYTE byMoveAmount;
	};*/
}

// 포션을 제외한 splitable item 나누기
void
CNetMsgProcessor_ItemArrangement::ResSeparationResult_zocl( char * pi_pMsg )
{
	_res_separation_result_zocl * pRecvMsg =
		reinterpret_cast< _res_separation_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byErrCode == 0 )
	{
		Item_Devide( pRecvMsg->wParentSerial, pRecvMsg->byParentAmount,
					 pRecvMsg->wChildSerial, pRecvMsg->byChildAmount );
	}
	else
	{
		_GetInventoryBoard()->UI_AddResult( FALSE );
		ShowServerErrorMessage( pRecvMsg->byErrCode );
	}

	/*
	#define res_separation_result_zocl 15	
	struct _res_separation_result_zocl{//add(1/23)
		BYTE byErrCode;	//0: 성공, 1: 갯수모자름 , 2: 전부이동함, 3: 자원,원석이아님 4: 남은인벤토리없음, 5: 그런시리얼없음 6: 사용하지않는가방
		WORD wParentSerial;
		BYTE byParentAmount;
		WORD wChildSerial;
		BYTE byChildAmount;
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 포션을 제외한 splitable item 합치기
BOOL
CNetMsgProcessor_ItemArrangement::ResDivisionRequest_clzo( void )
{
	_res_division_request_clzo stSendMsg;

	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );
	if( !pHoverItem || ! pTarItem )
		return FALSE;

	stSendMsg.wStartSerial	= pHoverItem->GetIndex();
	stSendMsg.wTarSerial	= pTarItem->GetIndex();
	stSendMsg.byMoveAmount	= pHoverItem->GetQuantity();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( res_division_request_clzo,
						   &stSendMsg,
						   sizeof( _res_division_request_clzo ) );

	/*
	#define res_division_request_clzo 13	//다른 inventory와 나눠서 병합시킨다.
	struct _res_division_request_clzo{
		WORD wStartSerial;	
		WORD wTarSerial;
		BYTE byMoveAmount;
	};*/
}

// 포션을 제외한 splitable item 합치기
void
CNetMsgProcessor_ItemArrangement::ResDivisionResult_zocl( char * pi_pMsg )
{
	_res_division_result_zocl * pRecvMsg =
		reinterpret_cast< _res_division_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byErrCode == 0 )
	{
		Item_Combine( pRecvMsg->wParentSerial, pRecvMsg->byParentAmount,
					  pRecvMsg->wChildSerial, pRecvMsg->byChildAmount );
	}
	else
	{
		_GetInventoryBoard()->UI_AddResult( FALSE );
		ShowServerErrorMessage( pRecvMsg->byErrCode );
	}

	/*
	#define res_division_result_zocl 14
	struct _res_division_result_zocl{
		BYTE byErrCode;	//0: 성공, 1: 갯수모자름, 3: 자원,원석이아님, 4: 더많은양을이동, 5: 그런시리얼없음, 6: 같은종류의자원,원석아님. 7: 사용하지않는가방, 8: 더많이이동함 9: 이동할곳이넘침
		WORD wParentSerial;		//0xFFFF이면 슬롯삭제
		BYTE byParentAmount;	
		WORD wChildSerial;		//new serial
		BYTE byChildAmount;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// animus window <=> inventory window 간의 이동
BOOL
CNetMsgProcessor_ItemArrangement::AnimusInvenChangeRequest_clzo( void )
{
	_animus_inven_change_request_clzo stSendMsg;

	stSendMsg.Item.byStorageCode	= _GetGBM()->GetOrgBoardType();
	stSendMsg.Item.wItemSerial		= _GetGBM()->GetHoverObject()->GetIndex();
	stSendMsg.wReplaceSerial		= 0xFFFF;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( animus_inven_change_request_clzo,
						   &stSendMsg,
						   sizeof( _animus_inven_change_request_clzo ) );

	/*
	#define animus_inven_change_request_clzo 113
	struct _animus_inven_change_request_clzo{		//add(2/2)
		_STORAGE_POS_INDIV Item;	
		WORD wReplaceSerial;	//빈곳이동은 0xffff
	};*/
}

// animus window <=> inventory window 간의 이동
void
CNetMsgProcessor_ItemArrangement::AnimusInvenChangeResult_zocl( char * pi_pMsg )
{
	_animus_inven_change_result_zocl * pRecvMsg =
		reinterpret_cast< _animus_inven_change_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byErrCode == 0 )
	{
		if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )
			_GetInventoryBoard()->UI_AddResult( TRUE );
		else if( _GetGBM()->GetTarBoardType() == IBT_ANIMUS )
			_GetAnimusBoard()->UI_AddResult( TRUE );
	}
	else
	{
		if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )
			_GetInventoryBoard()->UI_AddResult( FALSE );
		else if( _GetGBM()->GetTarBoardType() == IBT_ANIMUS )
			_GetAnimusBoard()->UI_AddResult( FALSE );

		ShowServerErrorMessage( pRecvMsg->byErrCode );
	}

	/*
	#define animus_inven_change_result_zocl 114
	struct _animus_inven_change_result_zocl{		//add(2/2)
		BYTE byErrCode;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 인벤토리창과 포스간의 이동
BOOL
CNetMsgProcessor_ItemArrangement::ForceInvenChangeRequest_clzo( void )
{
	_force_inven_change_request_clzo stSendMsg;

	stSendMsg.Item.byStorageCode = _GetGBM()->GetOrgBoardType();
	stSendMsg.Item.wItemSerial = _GetGBM()->GetHoverObject()->GetIndex();
	stSendMsg.wReplaceSerial = 0xFFFF;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( force_inven_change_request_clzo,
						   &stSendMsg,
						   sizeof( _force_inven_change_request_clzo ) );	
}

void
CNetMsgProcessor_ItemArrangement::ForceInvenChangeResult_zocl( char * pi_pMsg )
{
	_force_inven_change_result_zocl	* pRecvMsg =
		reinterpret_cast< _force_inven_change_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byErrCode == 0 )
	{
		if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )
			_GetInventoryBoard()->UI_AddResult( TRUE );
		else if( _GetGBM()->GetTarBoardType() == IBT_FORCE )
			_GetForceBoard()->UI_AddResult( TRUE );
	}
	else
	{
		if( _GetGBM()->GetTarBoardType() == IBT_INVENTORY )
			_GetInventoryBoard()->UI_AddResult( FALSE );
		else if( _GetGBM()->GetTarBoardType() == IBT_FORCE )
			_GetForceBoard()->UI_AddResult( FALSE );

		ShowServerErrorMessage( pRecvMsg->byErrCode );
	}

}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNetMsgProcessor_ItemArrangement::ShowServerErrorMessage( DWORD pi_dwErrorCode )
{
	switch( pi_dwErrorCode )
	{
		case 0 :
		default	: _SetSystemMsg( "아이템 정리 관련 에러 : 알 수 없는 에러 ( %d )", pi_dwErrorCode ); break;
	}
}

#endif // _NEW_UI_