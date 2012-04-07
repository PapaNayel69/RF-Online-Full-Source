////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Map Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../../GameObject/Manager/CCharacterMgr.h"
#include "../../../GameObject/Board/Common/CBoardManager.h"
#include "../../../GameObject/Board/General/CGameEventBoard.h"
#include "../../../GameObject/Board/General/CPartyBoard.h"

#include "CNetMsgProcessor_Map.h"

#include "../../../CLand.h"
#include "../../../UserInterface/GUIController/GUIMessagebox.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_Map::CNetMsgProcessor_Map()
{
}

CNetMsgProcessor_Map::~CNetMsgProcessor_Map()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetMsgProcessor_Map::SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	return CNetMsgProcessor_Base::SendNetMessage( map_msg, pi_byMsgIndex, pi_pMsg, pi_nSize );
}

bool
CNetMsgProcessor_Map::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	if( pMsgHeader->m_byType[0] != map_msg )
		return false;

	switch( pMsgHeader->m_byType[1] )
	{
		case move_potal_result_zocl :
			MovePotalResult_zocl( pi_pMsg );
			break;

		default :
			m_pNetworkMgr->NetLogWrite( "Unknown Message : CNetMsgProcessor_Map::AnalysisServerPacket()\n" );
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 맵 이동 요청
BOOL
CNetMsgProcessor_Map::MovePotalRequest_clzo( BYTE pi_byPortalNo )
{
	_move_potal_request_clzo stSendMsg;

	MAP_INFO * pMapInfo = _GetLand()->GetMapInfo( _GetLand()->GetMapIndex() );
	if( pi_byPortalNo >= pMapInfo->byPortalNum )
		return FALSE;

	stSendMsg.byPotalIndex = pi_byPortalNo;

	if( SendNetMessage( move_potal_request_clzo,
						&stSendMsg,
						sizeof( _move_potal_request_clzo ) ) )
	{
		_GetGBM()->SetRequestProcessState( TRUE );

		_GetAvatar()->GetMoveMapInfo()->m_bIsMoved = TRUE;

		_GetLand()->SetMapIndex( ID_INVALID );
		_GetLand()->SetPortalIndex( ID_INVALID );

		return TRUE;
	}

	return FALSE;

	/*
	#define move_potal_request_clzo 1
	struct _move_potal_request_clzo{
		BYTE byPotalIndex;	
	};
	*/
}

// 맵 이동 요청에 대한 결과
void
CNetMsgProcessor_Map::MovePotalResult_zocl( char * pi_pMsg )
{
	_move_potal_result_zocl * pRecvMsg = reinterpret_cast< _move_potal_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	_GetAvatar()->SetAction( CAI_MOVE_STOP );

	// 성공
	if( pRecvMsg->byRet == 0 )
	{
		MOVEMAP_INFO * pMoveMapInfo = _GetAvatar()->GetMoveMapInfo();
		if( !pMoveMapInfo )
			return;

		memcpy( pMoveMapInfo->m_vecPosition, pRecvMsg->fStartPos, sizeof( float ) * 3 );

		// 같은 존
		if( pRecvMsg->byZoneCode == 0 )
		{
			if( pRecvMsg->byMapIndex != pMoveMapInfo->m_pExitPortalInfo->byMapIndex )
			{
				_GUIMessageBox( NULL, "클라이언트와 서버의 맵 인덱스가 틀립니다.", 0xAAFFFFFF );
				return;
			}

			pMoveMapInfo->MoveToPortal_Result( GST_PORTAL );
		}
		// 다른 존
		else
		{
			if( pRecvMsg->byMapIndex != pMoveMapInfo->m_pExitPortalInfo->byMapIndex )
			{
				_GUIMessageBox( NULL, "클라이언트와 서버의 맵 인덱스가 틀립니다.", 0xAAFFFFFF );
				return;
			}

			pMoveMapInfo->MoveToPortal_Result_ToOtherServer();
		}
	}
	// 실패
	else
	{
		_GetLand()->SetMapIndex( _GetAvatar()->GetMoveMapInfo()->m_pEnterPortalInfo->byMapIndex );
		_GetAvatar()->GetMoveMapInfo()->m_bIsMoved			= FALSE;
		_GetAvatar()->GetMoveMapInfo()->m_pEnterPortalInfo	= NULL;
		_GetAvatar()->GetMoveMapInfo()->m_pExitPortalInfo	= NULL;

		switch( pRecvMsg->byRet )
		{
			case 1 : _SetSystemMsg( "맵 이동 실패 : 잘못된 포탈 번호" ); break;
			case 2 : _SetSystemMsg( "맵 이동 실패 : 포탈과의 거리가 너무 멈" ); break;
			case 3 : _SetSystemMsg( "맵 이동 실패 : 잘못된 맵 코드" ); break;
			case 4 : _SetSystemMsg( "맵 이동 실패 : 잘못된 포탈 코드" ); break;
			case 5 : _SetSystemMsg( "맵 이동 실패 : 연결 좌표 영역 에러" ); break;
			default : _SetSystemMsg( "맵 이동 실패 : 알 수 없는 에러" ); break;
		}
	}

	/*
	#define move_potal_result_zocl 2
	struct _move_potal_result_zocl{	//upt(3/5)
		BYTE byRet;	////1;//잘못됀포탈번호 2;//포탈과먼위치 3;//잘못됀맵코드 4;//잘못됀포탈코드 5;//연결좌표영역에러 
		BYTE byMapIndex;	//현재 맵과 다른경우 새로운 맵을 로드.. 로드후 로드완료 메시지를 보내야함
		float fStartPos[3];
		BYTE byZoneCode;	//0이면 같은 존.. 1이면 다른존.. => move_out을 보낸다.	//add(3/11)
	};	
	*/
}