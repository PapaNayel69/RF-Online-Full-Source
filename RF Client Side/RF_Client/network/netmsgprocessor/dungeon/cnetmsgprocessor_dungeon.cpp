////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Dungeon Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../../GameObject/Manager/CCharacterMgr.h"
#include "../../../GameObject/Manager/CGroundItemManager.h"
#include "../../../Character/Avatar/CAvatar_Quest.h"
#include "../../../GameObject/Board/Common/CBoardManager.h"
#include "../../../GameObject/Board/General/CGameEventBoard.h"
#include "../../../GameObject/Board/General/CPartyBoard.h"

#include "../../../CLand.h"

#include "CNetMsgProcessor_Dungeon.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_Dungeon::CNetMsgProcessor_Dungeon()
{
}

CNetMsgProcessor_Dungeon::~CNetMsgProcessor_Dungeon()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetMsgProcessor_Dungeon::SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	return CNetMsgProcessor_Base::SendNetMessage( dungeon_msg, pi_byMsgIndex, pi_pMsg, pi_nSize );
}

bool
CNetMsgProcessor_Dungeon::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	if( pMsgHeader->m_byType[0] != dungeon_msg )
		return false;

	switch( pMsgHeader->m_byType[1] )
	{
		case battledungeon_open_result_zocl :
			BattledungeonOpenResult_zocl( pi_pMsg );
			break;

		case dungeongate_create_zocl :
			DungeongateCreate_zocl( pi_pMsg );
			break;

		case dungeongate_destroy_zocl :
			DungeongateDestroy_zocl( pi_pMsg );
			break;

		case dungeongate_fixpositon_zocl :
			DungeongateFixpositon_zocl( pi_pMsg );
			break;

		case dungeongate_state_change_zocl :
			DungeongateStateChange_zocl( pi_pMsg );
			break;

		case goto_dungeon_result_zocl :
			GotoDungeonResult_zocl( pi_pMsg );
			break;

		case close_dungeon_inform_zocl :
			CloseDungeonInform_zocl( pi_pMsg );
			break;

		case outof_dungeon_result_zocl :
			OutofDungeonResult_zocl( pi_pMsg );
			break;

		case killmon_dungeon_inform_zocl :
			KillmonDungeonInform_zocl( pi_pMsg );
			break;

		default :
			m_pNetworkMgr->NetLogWrite( "Unknown Message : CNetMsgProcessor_Dungeon::AnalysisServerPacket()\n" );
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 던젼 생성 요청
BOOL
CNetMsgProcessor_Dungeon::BattledungeonOpenRequest_clzo( DWORD pi_dwItemIndex )
{
	_battledungeon_open_request_clzo stSendMsg;

	stSendMsg.dwItemSerial	= pi_dwItemIndex;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( battledungeon_open_request_clzo,
						   &stSendMsg,
						   sizeof( _battledungeon_open_request_clzo ) );
	/*
	#define battledungeon_open_request_clzo 1
	struct _battledungeon_open_request_clzo	{	//add(12/22)
		DWORD dwItemSerial;
	};
	*/
}

// 던젼 생성 요청에 대한 결과
void
CNetMsgProcessor_Dungeon::BattledungeonOpenResult_zocl( char * pi_pMsg )
{
	_battledungeon_open_result_zocl * pRecvMsg = reinterpret_cast< _battledungeon_open_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byRetCode == 0 )
	{
		CBoardItem * pUsingItem = _GetAvatar()->GetUsingItem();
		if( pUsingItem )
			_GetInventoryBoard()->UI_UseResult( pUsingItem->GetIndex() );
	}
	else
	{
		ShowServerErrorMessage( pRecvMsg->byRetCode );
	}

	/*
	#define battledungeon_open_result_zocl 2
	struct _battledungeon_open_result_zocl	{	//add(12/22)
		BYTE byRetCode;	
	};
	*/
}

// 던젼이 생성되었음
void
CNetMsgProcessor_Dungeon::DungeongateCreate_zocl( char * pi_pMsg )
{
	_dungeongate_create_zocl * pRecvMsg = reinterpret_cast< _dungeongate_create_zocl * >( pi_pMsg );

	cnm_create_item stItemInfo;

	stItemInfo.wIndex			= pRecvMsg->wGateIndex;
	stItemInfo.wRecIndex		= pRecvMsg->wItemRecIndex;
	stItemInfo.byEquipPart		= tbl_code_bdungeon;
	stItemInfo.vecPos[0]		= ( float )pRecvMsg->zPos[0];
	stItemInfo.vecPos[1]		= ( float )pRecvMsg->zPos[1];
	stItemInfo.vecPos[2]		= ( float )pRecvMsg->zPos[2];

	CGroundItem * pGroundItem = _GetNetworkMgr()->CreateGroundItem( &stItemInfo );
	if( !pGroundItem )
		return;

	pGroundItem->SetIndex( pRecvMsg->dwGateSerial );
	pGroundItem->SetServerIndex( pRecvMsg->wGateIndex );

	pGroundItem->SetPortalGotoDungeon( TRUE );
	pGroundItem->SetBone( pGroundItem->GetMeshID() );
	pGroundItem->SetMesh( pGroundItem->GetMeshID() );

	DUNGEON_ITEM_DATA * l_pItemData = ( DUNGEON_ITEM_DATA * )pGroundItem->GetData();
	if( l_pItemData->GetEnterLimitation() == DUNGEON_ITEM_DATA::DEL_NONE )
		pRecvMsg->dwOpenerSerial = ID_INVALID;
	
	pGroundItem->SetOwner( pRecvMsg->byState, pRecvMsg->dwOpenerSerial );

	/*
	#define dungeongate_create_zocl 3
	struct _dungeongate_create_zocl	{	//add(12/22)

		WORD wGateIndex;
		DWORD dwGateSerial;
		DWORD dwOpenerSerial;
		WORD wItemRecIndex;
		BYTE byState;
		short zPos[3];
	};
	*/
}

// 던젼이 파괴되었음
void
CNetMsgProcessor_Dungeon::DungeongateDestroy_zocl( char * pi_pMsg )
{
	_dungeongate_destroy_zocl * pRecvMsg = reinterpret_cast< _dungeongate_destroy_zocl * >( pi_pMsg );

	CGroundItem * pGroundItem = _GetGroundItemMgr()->GetItem( pRecvMsg->wGateIndex );
	if( pGroundItem )
		pGroundItem->DeleteItemEffect();
		
	_GetGroundItemMgr()->DeleteItem( pRecvMsg->wGateIndex );

	/*
	#define dungeongate_destroy_zocl 4
	struct _dungeongate_destroy_zocl {	//add(12/22)		
		WORD wGateIndex;
		DWORD dwGateSerial;
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNetMsgProcessor_Dungeon::DungeongateFixpositon_zocl( char * pi_pMsg )
{
	_dungeongate_fixpositon_zocl * pRecvMsg = reinterpret_cast< _dungeongate_fixpositon_zocl * >( pi_pMsg );

	cnm_create_item stItemInfo;

	stItemInfo.wIndex			= pRecvMsg->wGateIndex;
	stItemInfo.wRecIndex		= pRecvMsg->wItemRecIndex;
	stItemInfo.byEquipPart		= IEPT_DUNGEON;
	stItemInfo.vecPos[0]		= ( float )pRecvMsg->zPos[0];
	stItemInfo.vecPos[1]		= ( float )pRecvMsg->zPos[1];
	stItemInfo.vecPos[2]		= ( float )pRecvMsg->zPos[2];

	CGroundItem * pGroundItem = _GetNetworkMgr()->CreateGroundItem( &stItemInfo );
	if( !pGroundItem )
		return;

	pGroundItem->SetIndex( pRecvMsg->dwGateSerial );
	pGroundItem->SetServerIndex( pRecvMsg->wGateIndex );

	pGroundItem->SetPortalGotoDungeon( TRUE );
	pGroundItem->SetBone( pGroundItem->GetMeshID() );
	pGroundItem->SetMesh( pGroundItem->GetMeshID() );

	pGroundItem->SetOwner( pRecvMsg->byState, pRecvMsg->dwOpenerSerial );
	pGroundItem->SetPosition( pRecvMsg->zPos[0], pRecvMsg->zPos[1], pRecvMsg->zPos[2] );
	pGroundItem->SetReceiveFixPositionMsg( TRUE );

	pGroundItem->Disable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );

	/*
	#define dungeongate_fixpositon_zocl 5
	struct _dungeongate_fixpositon_zocl {
		WORD wGateIndex;
		DWORD dwGateSerial;
		DWORD dwOpenerSerial;
		WORD wItemRecIndex;
		BYTE byState;
		short zPos[3];
	};
	*/
}

void
CNetMsgProcessor_Dungeon::DungeongateStateChange_zocl( char * pi_pMsg )
{
	_dungeongate_state_change_zocl * pRecvMsg = reinterpret_cast< _dungeongate_state_change_zocl * >( pi_pMsg );

	CGroundItem * pGroundItem = _GetGroundItemMgr()->GetItem( pRecvMsg->wGateIndex );
	if( !pGroundItem )
		return;

	pGroundItem->SetOwner( pRecvMsg->byState, pGroundItem->GetOwnerIndex() );

	/*
	#define dungeongate_state_change_zocl 6
	struct _dungeongate_state_change_zocl {
		BYTE byState;
		WORD wGateIndex;
		DWORD dwGateSerial;
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 배틀 던젼으로 이동 요청
BOOL
CNetMsgProcessor_Dungeon::GotoDungeonRequest_clzo( CGroundItem * pi_pDungeonItem )
{
	if( !pi_pDungeonItem )
		return FALSE;

	_goto_dungeon_request_clzo stSendMsg;

	stSendMsg.dwGateSerial	= pi_pDungeonItem->GetIndex();
	stSendMsg.wGateIndex	= pi_pDungeonItem->GetServerIndex();

	if( SendNetMessage( goto_dungeon_request_clzo,
						&stSendMsg,
						sizeof( _goto_dungeon_request_clzo ) ) )
	{
		_GetGBM()->SetRequestProcessState( TRUE );

		_GetDungeonInfo()->SetDungeonData( pi_pDungeonItem );

		_GetAvatar()->GetMoveMapInfo()->m_bIsMoved = TRUE;

		_GetLand()->SetMapIndex( ID_INVALID );
		_GetLand()->SetPortalIndex( ID_INVALID );

		return TRUE;
	}

	return FALSE;

	/*
	#define goto_dungeon_request_clzo 7
	struct _goto_dungeon_request_clzo {	//add(10/16)
		WORD	wGateIndex;
		DWORD	dwGateSerial;
	};
	*/
}

// 배틀 던젼 이동 요청에 대한 결과
void
CNetMsgProcessor_Dungeon::GotoDungeonResult_zocl( char * pi_pMsg )
{
	_goto_dungeon_result_zocl * pRecvMsg = reinterpret_cast< _goto_dungeon_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	_GetAvatar()->SetAction( CAI_MOVE_STOP );

	// 성공
	if( pRecvMsg->byRetCode == 0 )
	{
		MOVEMAP_INFO * pMoveMapInfo = _GetAvatar()->GetMoveMapInfo();
		if( !pMoveMapInfo )
			return;

		pMoveMapInfo->m_byExitMapIndex	= pRecvMsg->byMapCode;

		pMoveMapInfo->m_vecPosition[0]	= pRecvMsg->zPos[0];
		pMoveMapInfo->m_vecPosition[1]	= pRecvMsg->zPos[1];
		pMoveMapInfo->m_vecPosition[2]	= pRecvMsg->zPos[2];

		pMoveMapInfo->MoveToPortal_Result( GST_DUNGEON );

		_GetDungeonInfo()->SetEnterDungeonTime( pRecvMsg->wLeftSec );
	}
	// 실패
	else
	{
		ShowServerErrorMessage( pRecvMsg->byRetCode );
	}

	/*
	#define goto_dungeon_result_zocl 8
	struct _goto_dungeon_result_zocl {	//	==> new_pos_start_request_clzo로 연결..
		BYTE	byRetCode;	
		BYTE	byMapCode;
		short	zPos[3];
		WORD	wLeftSec;			//남은초 add(1/5)..
		WORD	wTotalKillNum;		//add(1/5)..
		WORD	wMissionKillNum;	//add(1/5)..
	};
	*/
}

// 배틀 던젼이 닫혔음 ( => _outof_dungeon_result_zocl 이 연속으로 온다. )
void
CNetMsgProcessor_Dungeon::CloseDungeonInform_zocl( char * pi_pMsg )
{
	_close_dungeon_inform_zocl * pRecvMsg = reinterpret_cast< _close_dungeon_inform_zocl * >( pi_pMsg );

	if( pRecvMsg->byCompleteCode == dungeon_complete_code_mission )
		_GetDungeonInfo()->SetDungeonExitType( DET_CLEAR );
	else if( pRecvMsg->byCompleteCode == dungeon_complete_code_timeout )
		_GetDungeonInfo()->SetDungeonExitType( DET_TIMEOVER );

	/*
	#define close_dungeon_inform_zocl 9
	struct _close_dungeon_inform_zocl {	//add(10/16)
		BYTE byCompleteCode;//	CharacterDB.h.. dungeon_complete_code_timeout 0 (던젼이 닫히는 이유..)
	};
	*/
}

// 배틀 던젼에서 나감
BOOL
CNetMsgProcessor_Dungeon::OutofDungeonRequest_clzo( void )
{
	_GetDungeonInfo()->SetDungeonExitType( DET_CHEAT );

	return SendNetMessage( outof_dungeon_request_clzo,
						   NULL,
						   0 );
	/*
	#define outof_dungeon_request_clzo 6
	struct _outof_dungeon_request_clzo {	//add(10/16)
	};
	*/
}

// 배틀 던젼에서 나감
void
CNetMsgProcessor_Dungeon::OutofDungeonResult_zocl( char * pi_pMsg )
{
	_outof_dungeon_result_zocl * pRecvMsg = reinterpret_cast< _outof_dungeon_result_zocl * >( pi_pMsg );

	_GetAvatar()->SetAction( CAI_MOVE_STOP );

	// 성공
	if( pRecvMsg->byRetCode == 0 )
	{
		MOVEMAP_INFO * pMoveMapInfo = _GetAvatar()->GetMoveMapInfo();
		if( !pMoveMapInfo )
			return;

		pMoveMapInfo->m_byEnterMapIndex	= _GetLand()->GetMapIndex();
		pMoveMapInfo->m_byExitMapIndex	= pRecvMsg->byMapCode;

		pMoveMapInfo->m_vecPosition[0]	= pRecvMsg->zPos[0];
		pMoveMapInfo->m_vecPosition[1]	= pRecvMsg->zPos[1];
		pMoveMapInfo->m_vecPosition[2]	= pRecvMsg->zPos[2];

		_GetDungeonInfo()->ExitDungeon();

		pMoveMapInfo->MoveToPortal_Result( GST_DUNGEON );
	}
	// 실패
	else
	{
		ShowServerErrorMessage( pRecvMsg->byRetCode );
	}

	/*
	#define outof_dungeon_result_zocl 7
	struct _outof_dungeon_result_zocl {	//add(10/16)	==> new_pos_start_request_clzo로 연결..
		BYTE	byRetCode;	
		BYTE	byMapCode;
		short	zPos[3];
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타가 사냥한 몬스터의 수
void
CNetMsgProcessor_Dungeon::KillmonDungeonInform_zocl( char * pi_pMsg )
{
	_killmon_dungeon_inform_zocl * pRecvMsg = reinterpret_cast< _killmon_dungeon_inform_zocl * >( pi_pMsg );

	_GetDungeonInfo()->SetMonsterKillNum( pRecvMsg->wTotalKillNum );
	_GetDungeonInfo()->SetTargetMonsterKillNum( pRecvMsg->wMissionKillNum );

	/*
	#define killmon_dungeon_inform_zocl 12
	struct _killmon_dungeon_inform_zocl {		//add(1/5)
		WORD	wTotalKillNum;
		WORD	wMissionKillNum;
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNetMsgProcessor_Dungeon::ShowServerErrorMessage( DWORD pi_dwErrorCode )
{
	switch( pi_dwErrorCode )
	{
		case error_dungeon_nothing_ditem	: _SetSystemMsg( "던젼 관련 에러 : 던젼 아이템이 없습니다." ); break;
		case error_dungeon_not_ditem		: _SetSystemMsg( "던젼 관련 에러 : 던젼 아이템이 아닙니다." ); break;
		case error_dungeon_full_gate		: _SetSystemMsg( "던젼 관련 에러 : 현재 던젼을 생성시킬 수 없습니다. 잠시후 다시 시도해 주십시오." ); break;
		case error_dungeon_create_fail		: _SetSystemMsg( "던젼 관련 에러 : 던젼 생성을 실패하였습니다." ); break;
		case error_dungeon_none_exist		: _SetSystemMsg( "던젼 관련 에러 : 존재하지 않는 던젼입니다." ); break;
		case error_dungeon_refuse_enter		: _SetSystemMsg( "던젼 관련 에러 : 던젼에 들어갈 수 있는 권한이 없습니다." ); break;
		case error_dungeon_pos_setting		: _SetSystemMsg( "던젼 관련 에러 : 위치 설정에 실패 하였습니다." ); break;
		case error_dungeon_trans_map		: _SetSystemMsg( "던젼 관련 에러 : 현재 맵 이동 중입니다." ); break;
		case error_dungeon_not_here			: _SetSystemMsg( "던젼 관련 에러 : 현재 던젼안에 있지 않습니다." ); break;
		case error_dungeon_not_near_gate	: _SetSystemMsg( "던젼 관련 에러 : 던젼과 거리가 멉니다." ); break;
		case error_dungeon_data_failure		:
		default								: _SetSystemMsg( "던젼 관련 에러 : 알 수 없는 에러 ( %d )", pi_dwErrorCode ); break;
	}
}