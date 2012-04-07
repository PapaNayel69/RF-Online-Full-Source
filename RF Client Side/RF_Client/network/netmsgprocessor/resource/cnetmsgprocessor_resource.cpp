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

#include "CNetMsgProcessor_Resource.h"


////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_Resource::CNetMsgProcessor_Resource()
{

}

CNetMsgProcessor_Resource::~CNetMsgProcessor_Resource()
{

}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetMsgProcessor_Resource::SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	return CNetMsgProcessor_Base::SendNetMessage( resource_msg, pi_byMsgIndex, pi_pMsg, pi_nSize );
}
			
bool	
CNetMsgProcessor_Resource::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	if( pMsgHeader->m_byType[0] != resource_msg )
		return false;

	switch( pMsgHeader->m_byType[1] )
	{	
		case mine_start_result_zocl			:
			MineStartResult_zocl( pi_pMsg );
			break;

		case mine_cancle_result_zocl		:
			MineCancleResult_zocl( pi_pMsg );
			break;

		case mine_complete_result_zocl		:
			MineCompleteResult_zocl( pi_pMsg );
			break;

		case mine_start_result_other_zocl	:
			MineStartResultOther_zocl( pi_pMsg );
			break;

		case mine_end_result_other_zocl	:
			MineEndResultOther_zocl( pi_pMsg );
			break;

		case ore_cutting_result_zocl		:
			OreCuttingResult_zocl( pi_pMsg );
			break;

		case ore_into_bag_result_zocl		:
			OreIntoBagResult_zocl( pi_pMsg );
			break;

		case cutting_complete_result_zocl	:
			CuttingCompleteResult_zocl( pi_pMsg );
			break;

		default :
			m_pNetworkMgr->NetLogWrite( "Unknown Message : CNetMsgProcessor_Event::AnalysisServerPacket()\n" );
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 채굴 시작 요청
BOOL	
CNetMsgProcessor_Resource::MineStartRequest_clzo( void )
{
	_mine_start_request_clzo stSendMsg;			

	stSendMsg.byMineIndex	= _GetMiningBoard()->m_byDummyIndex;		// 자원 더미의 인덱스		

	BOOL bResult;

	bResult = SendNetMessage( mine_start_request_clzo,
						      &stSendMsg,
							  sizeof( _mine_start_request_clzo ) );	

	
	if( bResult )
	{
		_GetMiningBoard()->m_byMiningRequestType = MRT_START;
		_GetAvatar()->SetUsingItem( _GetEquipBoard()->GetItem( CEPT_CLIENT_TO_UI[IEPT_WEAPON] ) ); // => 여기서 아이템을 설정해줘야 포션 사용후 채굴할 경우 이펙트가 제대로 나온다.
	}

	/*
	#define mine_start_request_clzo 1
	struct _mine_start_request_clzo {
		BYTE byMineIndex;//광산 더미 인덱스 
	//	BYTE byBatteryBagIndex;//바떼리가든 가방번호
	//	WORD wBatterySerial;	//바떼리 시리얼
	};*/	

	return bResult;
}

// 채굴 시작 요청에 대한 응답
void	
CNetMsgProcessor_Resource::MineStartResult_zocl( char * pi_pMsg )
{
	_mine_start_result_zocl * pRecvMsg = 
		reinterpret_cast< _mine_start_result_zocl * >( pi_pMsg );

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		_GetAvatar()->SetAction( CAI_ITEM_USE_MINING );

		_GetMiningBoard()->m_dwMiningStartTime	= timeGetTime();
		_GetMiningBoard()->m_dwMiningEndTime	= _GetMiningBoard()->m_dwMiningStartTime + pRecvMsg->dwMineDelayTime;

		_GetMiningBoard()->SetMining( TRUE );

		_SetSystemMsg( "채굴 시작" );
	}
	// 실패
	else
	{
		_GetMiningBoard()->SetMining( FALSE );

#ifdef _DEVELOPER_
		switch( pRecvMsg->byErrCode )
		{
			case 2	: _SetSystemMsg( "채굴 시작 요청 실패 : 채굴 장소 아님" ); break;
			case 3	: _SetSystemMsg( "채굴 시작 요청 실패 : 장비 에러 )" ); break;
			case 4	: _SetSystemMsg( "채굴 시작 요청 실패 : 현재 채굴 모드임" ); break;
			default	: _SetSystemMsg( "채굴 시작 요청 실패" ); break;
		}
#else
		_SetSystemMsg( "채굴 시작 실패" );
#endif
	}

	_GetMiningBoard()->m_byMiningRequestType	= MRT_NONE;
	
	/*
	#define mine_start_result_zocl 2
	struct _mine_start_result_zocl {
		BYTE byErrCode;	//0: 성공, 2: 범위에러, 3: 채굴장비에러, 4: 현재 채굴모드임
		DWORD dwMineDelayTime;	//채굴시간	
	//	WORD wEquipLeftDur;		//남은 채굴장비 내구성//del(1/16)
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 채굴 취소
BOOL
CNetMsgProcessor_Resource::MineCancleRequest_clzo( void )
{
	_mine_cancle_request_clzo stSendMsg;

	stSendMsg.wBatterySerial = _GetMiningBoard()->m_dwBatteryIndex;	// 사용할 배터리의 시리얼		

	BOOL bResult = SendNetMessage( mine_cancle_request_clzo,
								   &stSendMsg,
								   sizeof( _mine_cancle_request_clzo ) );	

	// 결과 처리
	if( bResult )
	{
		_GetAvatar()->SetAction( CAI_MOVE_STOP );
		_GetAvatar()->SetEffectInfo( CAI_ITEM_USE_STOP );

		_GetMiningBoard()->SetMining( FALSE );

		//_GetMiningBoard()->m_byMiningRequestType	= MRT_CANCEL;
	}
	
	/*
	#define mine_cancle_request_clzo 3		//add(1/10)
	struct _mine_cancle_request_clzo {
		BYTE byBatteryBagIndex;//바떼리가든 가방번호
		WORD wBatterySerial;	//바떼리 시리얼
	};*/	

	return bResult;
}

// 채굴 취소에 대한 응답
void
CNetMsgProcessor_Resource::MineCancleResult_zocl( char * pi_pMsg )
{
	_mine_cancle_result_zocl * pRecvMsg = 
		reinterpret_cast< _mine_cancle_result_zocl * >( pi_pMsg );
	
	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		CBoardItem * pBoardItem = NULL;

		/* 채굴기 내구성 없음 => 2003.10.29 by 권경만
		//----------------------------------------------------------------------
		// 채굴기 설정

		// 채굴중 무기 아이템 장착을 바꿨을수도 있으므로 채굴기의 인덱스를 가지고 찾아야 한다.
		BYTE l_byPackNo = 0xFF, l_bySocketNo = 0xFF;

		l_pItem	= _GetEquipBoard()->GetItemByIndex( _GetMiningBoard()->m_dwMiningToolIndex );
		if( !l_pItem )
		{
			if( _GetInventoryBoard()->GetItemLocationByIndex( l_byPackNo, l_bySocketNo,
															  _GetMiningBoard()->m_dwMiningToolIndex ) )
				l_pItem = _GetInventoryBoard()->GetItem( l_byPackNo, l_bySocketNo );
		}

		// 채굴기가 있다면 내구성 & UI 설정
		if( l_pItem )
		{
			if( pRecvMsg->wEquipLeftDur <= 0 )
			{
				if( l_byPackNo == 0xFF )
				{
					_GetEquipBoard()->UnsetItem( CEPT_CLIENT_TO_UI[CEPT_WEAPON] );
					_GetInventoryWindow()->TakeOutItemID( ( CGUIInventoryWindow::EquipItemBoard )CEPT_CLIENT_TO_UI[CEPT_WEAPON] );
				}
				else
				{
					_GetInventoryBoard()->UnsetItem( l_byPackNo, l_bySocketNo );
					_GetInventoryWindow()->TakeOutItemID( l_byPackNo, l_bySocketNo );
				}
			}
			else
			{
				l_pItem->SetDurability( pRecvMsg->wEquipLeftDur );
			}
		}*/

		//----------------------------------------------------------------------
		// 배터리 설정
		if( pRecvMsg->wBatteryLeftDur <= 0 )
		{
			_GetInventoryBoard()->Remove( _GetMiningBoard()->m_byBatteryPackNo,
										  _GetMiningBoard()->m_byBatterySocketNo );
		}
		else
		{
			pBoardItem = _GetInventoryBoard()->GetItem( _GetMiningBoard()->m_byBatteryPackNo,
														_GetMiningBoard()->m_byBatterySocketNo );
			if( pBoardItem )
				pBoardItem->SetDurability( pRecvMsg->wBatteryLeftDur );
		}

#ifdef _DEVELOPER_
		_SetSystemMsg( "채굴 취소 요청 성공" );
#else
		_SetSystemMsg( "채굴 취소" );
#endif
	}
	// 실패
	else
	{
		_GetMiningBoard()->SetMining( FALSE );
		

#ifdef _DEVELOPER_
		switch( pRecvMsg->byErrCode )
		{
			case 1	: _SetSystemMsg( "채굴 취소 요청 실패 : 배터리 에러" ); break;
			case 3	: _SetSystemMsg( "채굴 취소 요청 실패 : 지연시간 에러" ); break;
			case 4	: _SetSystemMsg( "채굴 취소 요청 실패 : 채굴장비에러" ); break;
			default	: _SetSystemMsg( "채굴 취소 요청 실패" ); break;
		}
#else
		_SetSystemMsg( "채굴 취소" );
#endif
	}

	//_GetMiningBoard()->m_byMiningRequestType = MRT_NONE;

	/*
	#define mine_cancle_result_zocl 4
	struct _mine_cancle_result_zocl {
		BYTE byErrCode;	//0: 성공, 1: 바떼리에러, 3: 지연시간 에러 4:채굴장비에러
		WORD wBatteryLeftDur;	//남은 바떼리 용량
		WORD wEquipLeftDur;		//남은 채굴장비 내구성//add(1/16)
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 채굴 종료 요청
BOOL
CNetMsgProcessor_Resource::MineCompleteRequest_clzo( void )
{
	_mine_complete_request_clzo stSendMsg;

	stSendMsg.wBatterySerial	= _GetMiningBoard()->m_dwBatteryIndex;	// 사용할 배터리의 시리얼
	stSendMsg.byOreIndex		= _GetMiningBoard()->m_byOreIndex * 3;	// 채취할 원석의 인덱스
					
	BOOL bResult = SendNetMessage( mine_complete_request_clzo,
								   &stSendMsg,
								   sizeof( _mine_complete_request_clzo ) );

	// 결과 처리
	if( bResult )
	{
		_GetMiningBoard()->m_byMiningRequestType = MRT_COMPLEATE;
		_GetAvatar()->SetEffectInfo( CAI_ITEM_USE_STOP );
	}	

	/*
	#define mine_complete_request_clzo 5
	struct _mine_complete_request_clzo {
		BYTE byBatteryBagIndex;//바떼리가든 가방번호	//add(1/10)
		WORD wBatterySerial;	//바떼리 시리얼			//add(1/10)
		BYTE byIntoBagIndex;	//넣을가방
		BYTE byOreIndex;	//원석광물 인덱스
	};*/

	return bResult;
}

// 채굴 종료 요청에 대한 응답
void
CNetMsgProcessor_Resource::MineCompleteResult_zocl( char * pi_pMsg )
{
	_mine_complete_result_zocl * pRecvMsg = 
		reinterpret_cast< _mine_complete_result_zocl * >( pi_pMsg );

//#ifdef _DEVELOPER_
//	m_ClientLogFile.Write( "원석 : %d, 수량 : %d, 시리얼 : %d", pRecvMsg->byOreIndex, pRecvMsg->byOreDur, pRecvMsg->wOreSerial );
//#endif

	char pOreName[32];
	BYTE byOutput;

	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{
		CBoardItem * pBoardItem = NULL;

		/* 채굴기 내구성 없음 => 2003.10.29 by 권경만
		//----------------------------------------------------------------------
		// 채굴기 설정

		// 채굴중 무기 아이템 장착을 바꿨을수도 있으므로 채굴기의 인덱스를 가지고 찾아야 한다.
		BYTE l_byPackNo = 0xFF, l_bySocketNo = 0xFF;

		l_pItem	= _GetEquipBoard()->GetItemByIndex( _GetMiningBoard()->m_dwMiningToolIndex );
		if( !l_pItem )
		{
			if( _GetInventoryBoard()->GetItemLocationByIndex( l_byPackNo, l_bySocketNo,
															  _GetMiningBoard()->m_dwMiningToolIndex ) )
				l_pItem = _GetInventoryBoard()->GetItem( l_byPackNo, l_bySocketNo );
		}

		// 채굴기가 있다면 내구성 & UI 설정
		if( l_pItem )
		{
			if( pRecvMsg->wEquipLeftDur <= 0 )
			{
				if( l_byPackNo == 0xFF )
				{
					_GetEquipBoard()->UnsetItem( CEPT_CLIENT_TO_UI[CEPT_WEAPON] );
					_GetInventoryWindow()->TakeOutItemID( ( CGUIInventoryWindow::EquipItemBoard )CEPT_CLIENT_TO_UI[CEPT_WEAPON] );
				}
				else
				{
					_GetInventoryBoard()->UnsetItem( l_byPackNo, l_bySocketNo );
					_GetInventoryWindow()->TakeOutItemID( l_byPackNo, l_bySocketNo );
				}
			}
			else
			{
				l_pItem->SetDurability( pRecvMsg->wEquipLeftDur );
			}
		}*/

		//----------------------------------------------------------------------
		// 배터리 설정
		if( pRecvMsg->wBatteryLeftDur <= 0 )
		{
			_GetInventoryBoard()->Remove( _GetMiningBoard()->m_byBatteryPackNo,
										  _GetMiningBoard()->m_byBatterySocketNo );
		}
		else
		{
			pBoardItem = _GetInventoryBoard()->GetItem( _GetMiningBoard()->m_byBatteryPackNo,
														_GetMiningBoard()->m_byBatterySocketNo );
			if( pBoardItem )
				pBoardItem->SetDurability( pRecvMsg->wBatteryLeftDur );
		}

		//----------------------------------------------------------------------
		// 채굴한 원석의 설정
		BOOL bIsHaveOre = FALSE;	// 원석을 가지고 있는가?
		if( pRecvMsg->byOreDur > 1 )
		{
			// 먼저 인벤토리에서 찾는다.
			CBoardItem *	pInvenItem = _GetInventoryBoard()->GetItemByIndex( pRecvMsg->wOreSerial );
			CBoardItem *	pHoverItem = _GetGBM()->GetHoverItem();

			if( pInvenItem )
			{
				// 들고 있는 아이템이 있다면 동일한지 검사
				BYTE byHoverQuantity = 0;
				if( pHoverItem )
				{
					if( pHoverItem->GetIndex() == pRecvMsg->wOreSerial )
						byHoverQuantity = pHoverItem->GetQuantity();
				}

				byOutput = pRecvMsg->byOreDur - ( pInvenItem->GetQuantity() + byHoverQuantity );
				pInvenItem->SetQuantity( pInvenItem->GetQuantity() + byOutput );

				_strcpy( pOreName, pInvenItem->GetName() );
				bIsHaveOre = TRUE;
			}
			// 인벤토리에 없다면 들고 있는지 찾는다.
			else if( pHoverItem && ( pHoverItem->GetIndex() == pRecvMsg->wOreSerial ) )
			{
				byOutput = pRecvMsg->byOreDur - pHoverItem->GetQuantity();
				pHoverItem->SetQuantity( pRecvMsg->byOreDur );

				_strcpy( pOreName, pHoverItem->GetName() );
				bIsHaveOre = TRUE;
			}
		}

		if( !bIsHaveOre )
		{
			CBoardItem clsNewOre;

			if( !clsNewOre.Create( IEPT_ORE, pRecvMsg->byOreIndex ) )
				return;

			clsNewOre.SetIndex( pRecvMsg->wOreSerial );
			clsNewOre.SetServerIndex( pRecvMsg->wOreSerial );
			clsNewOre.SetDurability( 1 );
			clsNewOre.SetQuantity( pRecvMsg->byOreDur );

			// 인벤토리내의 빈 공간 체크
			BYTE byPackNo, bySocketNo;
			if( !_GetInventoryBoard()->FindEmptySocket( byPackNo, bySocketNo ) )
				_SetSystemMsg( "인벤토리에 빈 공간이 없습니다." );

			// add
			_GetInventoryBoard()->Add( byPackNo, bySocketNo, &clsNewOre );

			byOutput = pRecvMsg->byOreDur;
			_strcpy( pOreName, clsNewOre.GetName() );
		}

		_SetSystemMsg( "%s %d개를 획득하였습니다.", pOreName, byOutput );
	}
	// 실패
	else
	{
#ifdef _DEVELOPER_
		switch( pRecvMsg->byErrCode )
		{
			case 1	: _SetSystemMsg( "채굴 실패 : 지연시간에러" ); break;
			case 2	: _SetSystemMsg( "채굴 실패 : 원석가방에러" ); break;
			case 3	: _SetSystemMsg( "채굴 실패 : 담을공간없음" ); break;
			case 4	: _SetSystemMsg( "채굴 실패 : 원석종류에러" ); break;
			case 5	: _SetSystemMsg( "채굴 실패 : 베터리 에러" ); break;
			case 6	: _SetSystemMsg( "채굴 실패 : 채굴 장비 에러" ); break;
			default	: _SetSystemMsg( "채굴 실패" ); break;
		}
#else
		_SetSystemMsg( "채굴 실패" );
#endif
	}

	_GetMiningBoard()->SetMining( FALSE );	
	_GetMiningWindow()->Mining_On();	// 내부적으로만 채굴이 종료된 것이기 때문에 UI는 채굴상태로 보여준다.
	_GetMiningBoard()->m_byMiningRequestType	= MRT_NONE;

	/*	
	#define mine_complete_result_zocl 6
	struct _mine_complete_result_zocl {
		BYTE byErrCode;//0: 성공, 1: 지연시간에러, 2: 원석가방에러, 3: 담을공간없음, 4:원석종류에러,5: 베터리 에러, 6;채굴장비 에러
		WORD wEquipLeftDur;		//남은 채굴장비 내구성//udt(1/16)
		WORD wBatteryLeftDur;	//남은 바떼리 용량

		BYTE byOreIndex;
		WORD wOreSerial;
		BYTE byOreDur;	//1이면 위 시리얼은 새로운 시리얼 1이상이면 기존 시리얼		
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 다른 플레이어가 채굴을 시작했음
void
CNetMsgProcessor_Resource::MineStartResultOther_zocl( char * pi_pMsg )
{
	_mine_start_result_other_zocl * pRecvMsg = 
		reinterpret_cast< _mine_start_result_other_zocl * >( pi_pMsg );

	CPlayer * pPlayer = _GetCharMgr()->GetPlayer( pRecvMsg->dwSerial );
	if( !pPlayer )
		return;

	pPlayer->SetServerIndex( pRecvMsg->wIndex );
	pPlayer->SetAction( CAI_ITEM_USE_MINING );
	pPlayer->SetReceiveNetMsg( "Start Mining" );

	/*
	#define mine_start_result_other_zocl 22
	struct _mine_start_result_other_zocl {		//add(11/05)
		DWORD	dwSerial;
		WORD	wIndex;
	};
	*/
}

// 다른 플레이어가 채굴을 중단했음
void
CNetMsgProcessor_Resource::MineEndResultOther_zocl( char * pi_pMsg )
{
	_mine_end_result_other_zocl * pRecvMsg = 
		reinterpret_cast< _mine_end_result_other_zocl * >( pi_pMsg );

	CPlayer * pPlayer = _GetCharMgr()->GetPlayer( pRecvMsg->dwSerial );
	if( !pPlayer )
		return;

	pPlayer->SetServerIndex( pRecvMsg->wIndex );
	pPlayer->SetAction( CAI_MOVE_STOP );
	pPlayer->SetReceiveNetMsg( "Stop Mining" );

	/*
	#define mine_end_result_other_zocl 33
	struct _mine_end_result_other_zocl {		//add(11/05)
		DWORD	dwSerial;
		WORD	wIndex;
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 원석의 가공 요청
BOOL
CNetMsgProcessor_Resource::OreCuttingRequest_clzo( void )
{
	_ore_cutting_request_clzo stSendMsg;

	CBoardItem * pOre = _GetOreProcBoard()->GetOre();
	if( !pOre )
		return FALSE;

	stSendMsg.byCuttingHouse	= _GetAvatar()->GetNpcTradeInfo()->m_dwTradeCharIndex;	// 가공하는 NPC 인덱스
	stSendMsg.wOreSerial		= pOre->GetIndex();										// 가공할 원석의 인덱스
	stSendMsg.byProcessNum		= pOre->GetQuantity();									// 가공할 수량

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( ore_cutting_request_clzo,
						   &stSendMsg,
						   sizeof( _ore_cutting_request_clzo ) );			
	
	/*
	#define ore_cutting_request_clzo 7	//add(1/13)
	struct _ore_cutting_request_clzo {
		BYTE byCuttingHouse;
		BYTE byOreBagIndex;
		WORD wOreSerial;
		BYTE byProcessNum;
	};*/
}

// 원석의 가공 요청에 대한 결과
void
CNetMsgProcessor_Resource::OreCuttingResult_zocl( char * pi_pMsg )
{
	_ore_cutting_result_zocl * pRecvMsg = 
		reinterpret_cast< _ore_cutting_result_zocl * >( pi_pMsg );

	CBoardItem * pOre = _GetOreProcBoard()->GetOre();
	if( !pOre )
		return;

	_GetGBM()->SetRequestProcessState( FALSE );

	//--------------------------------------------------------------------------
	// 성공
	if( pRecvMsg->byErrCode == 0 )
	{	
		// 인벤토리에 원석이 남았음
		if( pRecvMsg->byLeftNum > 0 )
		{
			// 인벤토리의 남은 원석을 가져온다.
			CBoardItem * pInvenItem = _GetInventoryBoard()->GetItemByIndex( pOre->GetIndex() );

			if( pInvenItem )
				pInvenItem->SetQuantity( pRecvMsg->byLeftNum );
			else
				_SetSystemMsg( "원석 가공 에러 - 원석이 인벤토리에 없습니다." );
		}
		// 인벤토리에 남은 원석이 없음
		else
		{
			_GetInventoryBoard()->RemoveByIndex( pOre->GetIndex() );
		}

		_GetOreProcBoard()->RemoveOre();

		//----------------------------------------------------------------------

		// 가공된 잉갓의 설정
		CBoardItem	clsNewIngot;
		BYTE		bySocketNo;
		BYTE		byRemainNum;
		for( int i = 0; i < pRecvMsg->byCuttingNum; ++i )
		{
			// 잉갓의 남아 있는 수량 초기화
			byRemainNum = pRecvMsg->ResList[i].byAddAmount;

			// 같은 아이템이 있는지 검색
			CBoardItem * pInvenItem = _GetOreProcBoard()->GetLackOfQuantityItemByDTIndex( bySocketNo, pRecvMsg->ResList[i].wResIndex );

			// 같은 아이템이 있다면 합친다.
			if( pInvenItem )
			{
				byRemainNum = ( pInvenItem->GetQuantity() + pRecvMsg->ResList[i].byAddAmount ) - MAX_ITEM_QUANTITY;
				pInvenItem->SetQuantity( pInvenItem->GetQuantity() + ( pRecvMsg->ResList[i].byAddAmount - byRemainNum ) );
			}

			// 새로운 아이템을 넣는다.
			if( byRemainNum > 0 )
			{
				bySocketNo = _GetOreProcBoard()->FindEmptySocket();
				if( bySocketNo == 0xFF )
				{
					_SetSystemMsg( "원석 가공 에러 - 원석 임시 가공창에 빈 공간이 없습니다." );
					break;
				}

				if( !clsNewIngot.Create( IEPT_INGOT, pRecvMsg->ResList[i].wResIndex ) )
					continue;

				clsNewIngot.SetIndex( bySocketNo );
				clsNewIngot.SetServerIndex( bySocketNo );
				clsNewIngot.SetDurability( 1 );
				clsNewIngot.SetQuantity( pRecvMsg->ResList[i].byAddAmount );

				_GetOreProcBoard()->Add( 0, bySocketNo, &clsNewIngot );
			}
		}

		//----------------------------------------------------------------------

		// 기타 설정
		_GetAvatar()->SetDalant( pRecvMsg->dwLeftDalant );		

		_SetSystemMsg( "원석 가공에 성공 하였습니다." );
	}
	//--------------------------------------------------------------------------
	// 실패
	else
	{
#ifdef _DEVELOPER_
		switch( pRecvMsg->byErrCode )
		{
			case 1	: _SetSystemMsg( "원석 가공 에러 ( 원석 가방 or 시리얼실패 )" ); break;
			case 2	: _SetSystemMsg( "원석 가공 에러 ( 수량실패 )" ); break;
			case 3	: _SetSystemMsg( "원석 가공 에러 ( 소지금부족 )" ); break;
			case 4	: _SetSystemMsg( "원석 가공 에러 ( 가공임시공간 안비움 )" ); break;
			default	: _SetSystemMsg( "원석 가공 에러 ( None )" ); break;
		}
#else
		 _SetSystemMsg( "원석 가공에 실패 하였습니다." );
#endif
	}

	/*
	#define ore_cutting_result_zocl 8	//add(1/13)
	struct _ore_cutting_result_zocl {
		BYTE byErrCode;	//0:성공 1:원석 가방, 시리얼실패 2:수량실패 3:소지금부족 4:가공임시공간 안비움 
		BYTE byLeftNum;
		DWORD dwLeftGold;
		DWORD dwConsumGold;
		
		BYTE byCuttingNum;

		struct _list {
			WORD wResIndex;		//자원인댁스
			BYTE byAddAmount;	//추가되는 양
		};

		_list ResList[5];

		_ore_cutting_result_zocl(){
			byCuttingNum = 0;
		}

		int size(){
			return sizeof(*this)-sizeof(_list)*(5-byCuttingNum);
		}
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 가공된 잉갓을 인벤토리로 이동
BOOL
CNetMsgProcessor_Resource::OreIntoBagRequest_clzo( BYTE pi_byOreDTIndex, BYTE pi_byOreNum, DWORD pi_dwInvenOreSerial )
{
	_ore_into_bag_request_clzo stSendMsg;

	stSendMsg.wResIndex		= pi_byOreDTIndex;
	stSendMsg.byAddAmount	= pi_byOreNum;
	stSendMsg.wSerial		= pi_dwInvenOreSerial;

	
	_GetGBM()->SetRequestProcessState( TRUE );


	return SendNetMessage( ore_into_bag_request_clzo,
						   &stSendMsg,
						   sizeof( _ore_into_bag_request_clzo ) );
	/*
	#define ore_into_bag_request_clzo 9	//add(1/13)
	struct _ore_into_bag_request_clzo {
		WORD wResIndex;
		BYTE byBagIndex;
		WORD wSerial; //빈곳에 넣은거라면 0xFFFF
		BYTE byAddAmount;//추가되는 양
	};*/
}

// 가공된 잉갓을 인벤토리로 이동
void
CNetMsgProcessor_Resource::OreIntoBagResult_zocl( char * pi_pMsg )
{
	_ore_into_bag_result_zocl * pRecvMsg = reinterpret_cast< _ore_into_bag_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byErrCode == 0 )
	{
		_GetOreProcBoard()->UI_Ingot_To_Inventory_Result( pRecvMsg->wNewSerial );
	}
	else
	{
#ifdef _DEVELOPER_
		switch( pRecvMsg->byErrCode )
		{
			case 1	: _SetSystemMsg( "잉갓 이동 ( 가공창 => 인벤토리 ) 실패 ( 가공모드아님 )" ); break;
			case 2	: _SetSystemMsg( "잉갓 이동 ( 가공창 => 인벤토리 ) 실패 ( 자원없거나부족 )" ); break;
			case 3	: _SetSystemMsg( "잉갓 이동 ( 가공창 => 인벤토리 ) 실패 ( 사용가방아님 )" ); break;
			case 4	: _SetSystemMsg( "잉갓 이동 ( 가공창 => 인벤토리 ) 실패 ( 가방공간부족 )" ); break;
			case 5	: _SetSystemMsg( "잉갓 이동 ( 가공창 => 인벤토리 ) 실패 ( 가방에없는슬롯 )" ); break;
			case 6	: _SetSystemMsg( "잉갓 이동 ( 가공창 => 인벤토리 ) 실패 ( 슬롯당용량초과 )" ); break;
			case 7	: _SetSystemMsg( "잉갓 이동 ( 가공창 => 인벤토리 ) 실패 ( 가방과내용이다름 )" ); break;
			default	: _SetSystemMsg( "잉갓 이동 ( 가공창 => 인벤토리 ) 실패 ( None )" ); break;
		}
#else
		_SetSystemMsg( "자원 이동 실패" );
#endif			
	}	

	/*
	#define ore_into_bag_result_zocl 10	//add(1/13)
	struct _ore_into_bag_result_zocl {
		BYTE byErrCode;	//0:성공 1:가공모드아님 2:자원없거나부족 3:사용가방아님 4:가방공간부족 5:가방에없는슬롯 6:슬롯당용량초과, 7:가방과내용이다름
		WORD wNewSerial; //새로운 시리얼.. 기존꺼에다 쌓은경우는 기존의시리얼
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 원석 가공 종료 - 처분시에 사용
BOOL
CNetMsgProcessor_Resource::CuttingCompleteRequest_clzo( void )
{
	_cutting_complete_request_clzo stSendMsg;

	CNpc * pMerchant = ( CNpc * )_GetCharMgr()->GetCharacter( CTI_NPC,
															  _GetAvatar()->GetNpcTradeInfo()->m_dwTradeCharIndex );
	if( !pMerchant )
		return FALSE;

	stSendMsg.byNpcRace = pMerchant->GetRace();

	if( stSendMsg.byNpcRace > 2 )	// 0 = 벨라토, 1 = 코라, 2 = 아크레시아, 2이상 = 중립, 일단 중립은 벨라토로 보낸다.
		stSendMsg.byNpcRace = 0;
	

	_GetGBM()->SetRequestProcessState( TRUE );


	return SendNetMessage( cutting_complete_request_clzo,
						   &stSendMsg,
						   sizeof( _cutting_complete_request_clzo ) );		

	/*
	#define cutting_complete_request_clzo 11
	struct _cutting_complete_request_clzo {
		BYTE byNpcRace;	//npc종족.. 0:벨리토, 1:코라 2:아크레시아 
	};*/

	//if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) ResMsg_CuttingCompleteRequest_clzo", ++g_bWriteTestLogCount ); // 테스트 코드	
}

// 원석 가공 종료 - 처분시에 사용
void
CNetMsgProcessor_Resource::CuttingCompleteResult_zocl( char * pi_pMsg )
{
	_cutting_complete_result_zocl * pRecvMsg = 
			reinterpret_cast< _cutting_complete_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );	


	_GetAvatar()->SetGold( pRecvMsg->dwLeftGold );

	_GetOreProcBoard()->ClearIngot();	

	_SetSystemMsg( "가공된 원석을 모두 처분하였습니다." );

	/*
	#define cutting_complete_result_zocl 12	
	struct _cutting_complete_result_zocl {
		DWORD dwLeftGold;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////