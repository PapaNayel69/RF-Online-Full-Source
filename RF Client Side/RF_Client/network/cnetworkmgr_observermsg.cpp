////////////////////////////////////////////////////////////////////////////
//
// CNetworkMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../CCamera.h"

extern BOOL  g_bWriteTestLog;		// 테스트 코드
extern DWORD g_bWriteTestLogCount;	// 테스트 코드

////////////////////////////////////////////////////////////////////////////
//

// 옵져버 위치 요청
BOOL
CNetworkMgr::ObserverMsg_MoveRequest_clzo( float * pi_vecTargetPos )
{
	/*
	_observer_move_request_clzo l_observer_move_request_clzo;

	if( _GetAvatar()->GetObserverMode() == OMI_NORMAL )
	{
		// 옵져버의 속도
		//l_observer_move_request_clzo.fMoveSpeed = _GetAvatar()->GetSpeed() / 15.0f;
		l_observer_move_request_clzo.fMoveSpeed = 10; //CHAR_SPEED_RUN;

		// 현재 위치
		l_observer_move_request_clzo.zCurPos[0] = (short)_GetAvatar()->GetPosX();
		l_observer_move_request_clzo.zCurPos[1] = (short)_GetAvatar()->GetPosY();
		l_observer_move_request_clzo.zCurPos[2] = (short)_GetAvatar()->GetPosZ();

		// 타겟 위치
		l_observer_move_request_clzo.zTarPos[0] = (short)pi_vecTargetPos[0];
		l_observer_move_request_clzo.zTarPos[1] = (short)pi_vecTargetPos[1];
		l_observer_move_request_clzo.zTarPos[2] = (short)pi_vecTargetPos[2];
	}
	else if( _GetAvatar()->GetObserverMode() == OMI_FLY )
	{
		// Fly 모드일 경우에 넘어오는 pi_vecTargetPos 이전의 위치이다.
		// Fly 모드일 경우에는 pi_vecTargetPos 를 현재 위치로, _GetCamera()->GetPosition() 을
		// 목표 위치로 해서 보낸다.
		l_observer_move_request_clzo.fMoveSpeed = 10; //CAM_SPEED_FLY;

		// 현재 위치
		l_observer_move_request_clzo.zCurPos[0] = (short)pi_vecTargetPos[0];
		l_observer_move_request_clzo.zCurPos[1] = (short)pi_vecTargetPos[1];
		l_observer_move_request_clzo.zCurPos[2] = (short)pi_vecTargetPos[2];

		// 타겟 위치
		Vector3f l_vecTarget;
		_GetCamera()->GetPosition( l_vecTarget );
		l_observer_move_request_clzo.zTarPos[0] = (short)l_vecTarget[0];
		l_observer_move_request_clzo.zTarPos[1] = (short)l_vecTarget[1];
		l_observer_move_request_clzo.zTarPos[2] = (short)l_vecTarget[2];
	}
	else if( _GetAvatar()->GetObserverMode() == OMI_ANIMATION )
	{
		// 애니메이션 모드일 경우에 넘어오는 pi_vecTargetPos 이전의 위치이다.
		// 애니메이션 모드일 경우에는 pi_vecTargetPos 를 현재 위치로, _GetCamera()->GetPosition() 을
		// 목표 위치로 해서 보낸다.
		l_observer_move_request_clzo.fMoveSpeed = 10; //CAM_SPEED_FLY;

		// 현재 위치
		l_observer_move_request_clzo.zCurPos[0] = (short)pi_vecTargetPos[0];
		l_observer_move_request_clzo.zCurPos[1] = (short)pi_vecTargetPos[1];
		l_observer_move_request_clzo.zCurPos[2] = (short)pi_vecTargetPos[2];

		// 타겟 위치
		Vector3f l_vecTarget;
		_GetCamera()->GetPosition( l_vecTarget );
		l_observer_move_request_clzo.zTarPos[0] = (short)l_vecTarget[0];
		l_observer_move_request_clzo.zTarPos[1] = (short)l_vecTarget[1];
		l_observer_move_request_clzo.zTarPos[2] = (short)l_vecTarget[2];
	}

	// 메세지를 보냄
	BYTE l_nType[] = { observer_msg, observer_move_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_observer_move_request_clzo,
									 sizeof( _observer_move_request_clzo ) );


	// 결과 처리
	if( l_nResult )
	{
		// 먼저 아바타를 움직인 후... 서버에서 결과 메세지가 오면 재조정한다.
		if( _GetAvatar()->GetObserverMode() == OMI_NORMAL )
		{
			_GetAvatar()->SetAction( CAI_MOVE_MOVING );
			
			Vector3f l_vecTargetPos;
			l_vecTargetPos[0] = pi_vecTargetPos[0];
			l_vecTargetPos[1] = pi_vecTargetPos[1];
			l_vecTargetPos[2] = pi_vecTargetPos[2];
			_GetAvatar()->CreatePath( l_vecTargetPos );

			_GetAvatar()->SetRotY( R3API::GetYAngle( _GetAvatar()->GetPosition(), l_vecTargetPos ) );
		}
	}
	else
		NetLogWrite( "CNetworkMgr::observerMsg_MoveRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) observerMsg_MoveRequest_clzo", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
	*/

	return TRUE;
}

// 옵져버의 움직임
void
CNetworkMgr::ObserverMsg_MoveResult_zocl( char * pi_pMsg )
{
	/*
	_observer_move_result_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _observer_move_result_zocl ) );

	if( _GetAvatar()->GetObserverMode() == OMI_NORMAL )
	{
		_GetAvatar()->SetSpeed( l_sRecv.fMoveSpeed * CHAR_SPEED_DEFAULT );

		// 클라이언트의 캐릭터 위치와 서버의 캐릭터 위치 차이가 많이
		// 날 경우 서버상의 위치로 맞춰준다. ( 현재는 15.0 )
		Vector3f l_vecPos;
		ShortPosToFloatPos( l_vecPos, l_sRecv.zCurPos );
		if( GetDist( _GetAvatar()->GetPosition(), l_vecPos ) >= 15.0f )
			_GetAvatar()->SetPosition( l_vecPos );

		// 서버에서 온 타겟위치로 변경시켜준다.
		//ShortPosToFloatPos( l_vecPos, l_sRecv.zTarPos );
		//_GetAvatar()->SetCurPath( l_vecPos );

		// Y 회전값 설정
		_GetAvatar()->SetRotY( R3API::GetYAngle( _GetAvatar()->GetPosition(), l_vecPos ) );
	}
	// Fly모드 or Animation모드일 경우에는 위치 설정을 하지 않는다.
	else if( ( _GetAvatar()->GetObserverMode() == OMI_FLY ) ||
			 ( _GetAvatar()->GetObserverMode() == OMI_ANIMATION ) )
	{
		_GetCamera()->SetSpeed( l_sRecv.fMoveSpeed * CHAR_SPEED_DEFAULT * 10.0f );

		_GetCamera()->SetState( CAM_MOVE );
	}

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) ObjserverMsg_MoveResult_zocl", ++g_bWriteTestLogCount ); // 테스트 코드
	*/
}