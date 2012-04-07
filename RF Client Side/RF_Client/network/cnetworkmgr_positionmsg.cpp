////////////////////////////////////////////////////////////////////////////
//
// CNetworkMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../GameObject/Manager/CTargetMgr.h"
#include "../GameObject/Manager/CGroundItemManager.h"
#include "../GameObject/Board/Common/CBoardManager.h"

#include "../CLand.h"

extern BOOL  g_bWriteTestLog;		// 테스트 코드
extern DWORD g_bWriteTestLogCount;	// 테스트 코드

#define CURRENT_POSITION_INTERPOLATION_DISTANCE		100.0f
#define TARGET_POSITION_INTERPOLATION_DISTANCE		2.0f
#define CURRENT_POSITION_CHANGE_SPEED_DISTANCE		10.0f

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// character position flag
enum { CPF_MOVE, CPF_FIX_POSITION, CPF_REAL_MOVE, CPF_STOP };
char CPF_STRING[4][16] = { "Move", "Fix Position", "Real Move", "Stop" };

//------------------------------------------------------------------------------

// 캐릭터의 위치를 설정해주는 함수
BOOL
SetClientCurrentPosition( CCharacter * pi_pChar, Vector3f pi_vecCur, Vector3f pi_vecTar, BYTE pi_byMoveFlag )
{
	if( ( pi_vecCur[1] > 10000 ) || ( pi_vecCur[1] < -10000 ) ||
		( pi_vecTar[1] > 10000 ) || ( pi_vecTar[1] < -10000 ) )
	{
		_GetNetworkMgr()->NetLogWrite( "[ %s ] Cur => %0.f, %0.f, %0.f, Tar => %0.f, %0.f, %0.f",
										CPF_STRING[pi_byMoveFlag],
										pi_vecCur[0], pi_vecCur[1], pi_vecCur[2],
										pi_vecTar[0], pi_vecTar[1], pi_vecTar[2] );
		return FALSE;
	}

	//--------------------------------------------------------------------------
	// ROSF_OUT_OF_VISUAL_FIELD_SERVER 가 Enable 상태라면
	// 무조건 서버의 위치로 세팅을 해 주어야한다.
	if( pi_pChar->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) )
	{
		// ROSF_OUT_OF_VISUAL_FIELD_SERVER ==> Disable
		pi_pChar->Disable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );
	
		// 현재 위치
		pi_pChar->SetPosition( pi_vecCur );

		// 타겟 위치
		pi_pChar->CreatePath( pi_vecTar );

		// Action
		pi_pChar->SetFirstMoving( TRUE );
		pi_pChar->SetAction( CAI_MOVE_MOVING );

		return TRUE;
	}

	// 현재 위치와 타겟 위치가 같다면, 무조건 현재 위치로 세팅후에 멈춘다.
	if( ( pi_vecCur[0] == pi_vecTar[0] ) && ( pi_vecCur[1] == pi_vecTar[1] ) && ( pi_vecCur[2] == pi_vecTar[2] ) )
		return TRUE;

	//--------------------------------------------------------------------------
	// 현재 위치

	// 서버와 클라이언트의 위치 차이를 계산해서 속도의 배율을 정한다.
	// 차이가 심할 경우는 서버 위치로 변경시켜준다.
	float l_fDist = GetDist( pi_pChar->GetPosition(), pi_vecCur );
	if( l_fDist >= CURRENT_POSITION_INTERPOLATION_DISTANCE )
	{
		pi_pChar->SetPosition( pi_vecCur );
		pi_pChar->SetMagnificationOfSpeed( 1.0f );
	}

	/*
	// 차이가 CURRENT_POSITION_CHANGE_SPEED_DISTANCE 이상 일경우 속도를 조절해준다.
	else if( l_fDist >= CURRENT_POSITION_CHANGE_SPEED_DISTANCE )
	{
		float l_fDistOfServer, l_fDistOfClient;

		l_fDistOfServer = GetDist( pi_vecCur, pi_vecTar );

		if( pi_pChar->GetCharTypeID() == CTI_AVATAR )
		{
			l_fDistOfClient = GetDist( pi_pChar->GetPosition(), ((CAvatar *)pi_pChar)->GetCurrentTargetPos() );
		}
		else
		{
			Vector3f l_vecLastTarget;
			pi_pChar->GetLastPath( l_vecLastTarget );
			l_fDistOfClient = GetDist( pi_pChar->GetPosition(), l_vecLastTarget );
		}

		pi_pChar->SetMagnificationOfSpeed( l_fDistOfClient / l_fDistOfServer );
	}
	// 차이가 약간 난다면 아무것도 변경시켜주지 않는다.
	else
	{
		pi_pChar->SetMagnificationOfSpeed( 1.0f );
	}*/

	//--------------------------------------------------------------------------
	// 타겟 위치

	// 서버에서 온 타겟위치로 변경시켜준다.
	// 아바타일 경우에는 패치 시키지 않는다 => 서버에서의 응답이 늦게 오면 캐릭터가
	// 이상하게 움직이는 현상이 발생하기 때문에, 일단 아바타일 경우에는 타겟 위치를 맞춰주지 않는다.
	// => ex ) 아바타는 두/세번째 움직임을 하고 있을 경우 서버에서 응답이 늦어서 첫번째 움직임의 응답이
	//    올경우 타겟 위치를 서버 데이터로 맞출 경우 앞/뒤/옆으로 이상하게 움직인다.
	if( !pi_pChar->IsEqualCharType( CTI_AVATAR ) )
	{
		/*
		Vector3f l_vecLastTarget;
		pi_pChar->GetLastPath( l_vecLastTarget );

		if( GetDist( l_vecLastTarget, pi_vecTar ) > TARGET_POSITION_INTERPOLATION_DISTANCE )
			pi_pChar->CreatePath( pi_vecTar );*/

		pi_pChar->CreatePath( pi_vecTar );
	}

	return TRUE;
}

BOOL
SetClientCurrentPosition( CCharacter * pi_pChar, short * pi_pCur, short * pi_pTar, BYTE pi_byMoveFlag )
{
	Vector3f l_fCur, l_fTar;

	ShortPosToFloatPos( l_fCur, pi_pCur );
	ShortPosToFloatPos( l_fTar, pi_pTar );
	return SetClientCurrentPosition( pi_pChar, l_fCur, l_fTar, pi_byMoveFlag );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
SetPlayerPosition( char * pi_pMsg, BYTE pi_byFlag )
{
	_player_move_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _player_move_zocl ) );

	if( _GetAvatar()->GetIndex() == l_sRecv.dwSerial )
		return FALSE;

	cnm_create_player l_cnmPlayerInfo;

	l_cnmPlayerInfo.dwClientIndex		= l_sRecv.dwSerial;
	l_cnmPlayerInfo.dwServerIndex		= l_sRecv.wIndex;
	l_cnmPlayerInfo.dwRecIndex			= l_sRecv.byRaceCode;
	l_cnmPlayerInfo.dwEquipItemVersion	= l_sRecv.wEquipVer;

	CPlayer * l_pPlayer = _GetNetworkMgr()->CreatePlayer( &l_cnmPlayerInfo );
	if( !l_pPlayer )
		return FALSE;

	//--------------------------------------------------------------------------

	// 좌표가 같을 경우에는 멈춘다.
	if( ( l_sRecv.zCur[0] == l_sRecv.zTar[0] ) && ( l_sRecv.zCur[2] == l_sRecv.zTar[1] ) )
	{
		l_pPlayer->SetAction( CAI_MOVE_STOP );
	}
	if( pi_byFlag != CPF_FIX_POSITION )
	{
		// 위치 설정 => RealMove는 경계선에서 처음 보이는 것 이므로 무조건 서버위치로
		//				맞춰준후에 위치 보정을 한다.
		if( pi_byFlag == CPF_REAL_MOVE )
			l_pPlayer->SetPosition( l_sRecv.zCur[0], l_sRecv.zCur[1], l_sRecv.zCur[2] );

		short l_pTar[3] = { l_sRecv.zTar[0], l_sRecv.zCur[1], l_sRecv.zTar[1] };
		SetClientCurrentPosition( l_pPlayer, l_sRecv.zCur, l_pTar, pi_byFlag );

		Vector3f l_vecCurPath;
		l_pPlayer->GetCurPath( l_vecCurPath );

		l_pPlayer->SetRotY( l_pPlayer->GetPosition(), l_vecCurPath );

		l_pPlayer->SetFirstMoving( TRUE );
		l_pPlayer->SetAction( CAI_MOVE_MOVING );
	}
	else
	{
		// 위치 설정 => FixPosition은 경계선에서 처음 보이는 것 이므로
		//              좌표 보정을 하지 않고 무조건 서버위치로 맞춰주어야 한다.
		l_pPlayer->SetPosition( l_sRecv.zCur[0], l_sRecv.zCur[1], l_sRecv.zCur[2] );
		l_pPlayer->SetAction( CAI_MOVE_STOP );
	}

	//--------------------------------------------------------------------------

	// 상태 플래그 & 이펙트 코드 설정
	l_pPlayer->SetServerStatusFlag( l_sRecv.dwStateFlag );
	l_pPlayer->SetServerSFEffectCode( l_sRecv.wLastEffectCode, 1, TRUE );

	// RealMove도 FixPosition과 동일하게 플래그를 세팅시켜줘야 한다.
	if( pi_byFlag != CPF_MOVE )
	{
		l_pPlayer->SetReceiveFixPositionMsg( TRUE );

		if( l_pPlayer->IsEnable( ROSF_CHECK_Y_POS ) )
			l_pPlayer->CheckYPos();
	}

	// 이동 속도
	if( l_sRecv.byAddSpeed != 0xFF )
		l_pPlayer->GetConditionInfo()->m_fMoveSpeed	= ( float )l_sRecv.byAddSpeed / 10.0f;

	//--------------------------------------------------------------------------

	l_pPlayer->SetReceiveNetMsg( CPF_STRING[pi_byFlag] );

	return TRUE;

	/*
	#define player_move_zocl 4
	struct _player_move_zocl{	
		DWORD dwSerial;
		WORD wIndex;
		WORD wEquipVer;	
		BYTE byRaceCode;
		WORD wLastEffectCode;	//upt(4/12)
		DWORD dwStateFlag;
		short zCur[3];//xyz
		short zTar[2];	//xz
		BYTE byAddSpeed;	// add(10/23)
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
SetMonsterPosition( char * pi_pMsg, BYTE pi_byFlag )
{
	_monster_move_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _monster_move_zocl ) );

	cnm_create_monster l_cnmMonsterInfo;

	l_cnmMonsterInfo.dwServerIndex	= l_sRecv.wIndex;
	l_cnmMonsterInfo.dwClientIndex	= l_sRecv.dwSerial;
	l_cnmMonsterInfo.dwRecIndex		= l_sRecv.wRecIndex;
	l_cnmMonsterInfo.fScale			= (float)l_sRecv.byInfo[0] / 100.0f;

	CMonster * l_pMonster = _GetNetworkMgr()->CreateMonster( &l_cnmMonsterInfo );

	if( !l_pMonster )
		return FALSE;
	if( l_pMonster->IsDeath() )
		return FALSE;

	//--------------------------------------------------------------------------
	// SetAction() 전에 세팅 해야 함
	if( l_sRecv.byInfo[1] != AI_EM_NORMAL )
	{
		l_pMonster->SetCombatMode( CSI_COMBAT );
		l_pMonster->SetWalkMode( CMM_MOVE_RUN );
	}
	else
	{
		l_pMonster->SetCombatMode( CSI_PEACE );
		l_pMonster->SetWalkMode( CMM_MOVE_WALK );
	}

	//--------------------------------------------------------------------------

	// 좌표가 같을 경우에는 멈춘다.
	if( ( l_sRecv.zCur[0] == l_sRecv.zTar[0] ) && ( l_sRecv.zCur[2] == l_sRecv.zTar[1] ) )
	{
		l_pMonster->SetAction( CAI_MOVE_STOP );
	}
	if( pi_byFlag != CPF_FIX_POSITION )
	{
		// 위치 설정 => RealMove는 경계선에서 처음 보이는 것 이므로 무조건 서버위치로
		//				맞춰준후에 위치 보정을 한다.
		if( pi_byFlag == CPF_REAL_MOVE )
			l_pMonster->SetPosition( l_sRecv.zCur[0], l_sRecv.zCur[1], l_sRecv.zCur[2] );

		short l_pTar[3] = { l_sRecv.zTar[0], l_sRecv.zCur[1], l_sRecv.zTar[1] };
		SetClientCurrentPosition( l_pMonster, l_sRecv.zCur, l_pTar, pi_byFlag );

		Vector3f l_vecCurPath;
		l_pMonster->GetCurPath( l_vecCurPath );
		l_pMonster->SetRotY( l_pMonster->GetPosition(), l_vecCurPath );

		l_pMonster->SetFirstMoving( TRUE );
		l_pMonster->SetAction( CAI_MOVE_MOVING );
	}
	else
	{
		// 위치 설정 => FixPosition은 경계선에서 처음 보이는 것 이므로
		//              좌표 보정을 하지 않고 무조건 서버위치로 맞춰주어야 한다.
		l_pMonster->SetPosition( l_sRecv.zCur[0], l_sRecv.zCur[1], l_sRecv.zCur[2] );
		l_pMonster->SetAction( CAI_MOVE_STOP );
	}

	//--------------------------------------------------------------------------

	if( pi_byFlag == CPF_MOVE )
	{
		if( l_pMonster->GetCombatState()->GetRemainHP() == ID_INVALID )
			l_pMonster->SetHP( l_sRecv.wHPRate );
	}
	else
	{
		l_pMonster->SetHP( l_sRecv.wHPRate );
	}

	//--------------------------------------------------------------------------

	// 상태 플래그 & 이펙트 코드 설정
	l_pMonster->SetServerSFEffectCode( l_sRecv.wLastEffectCode, 1, TRUE );

	// RealMove도 FixPosition과 동일하게 플래그를 세팅시켜줘야 한다.
	if( pi_byFlag != CPF_MOVE )
	{
		l_pMonster->SetReceiveFixPositionMsg( TRUE );

		if( l_pMonster->IsEnable( ROSF_CHECK_Y_POS ) )
			l_pMonster->CheckYPos();
	}

	//--------------------------------------------------------------------------

	l_pMonster->SetReceiveNetMsg( CPF_STRING[pi_byFlag] );

	return TRUE;

	/*
	#define monster_move_zocl 5
	struct _monster_move_zocl{ 
		WORD wRecIndex;
		WORD wIndex;
		DWORD dwSerial;
		WORD wLastEffectCode;	//upt(4/12)
		short zCur[3];
		short zTar[2];	//x,z	//upt(6/3)
		WORD wHPRate;
		BYTE byInfo[4];
	};
	*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
SetAnimusPosition( char * pi_pMsg, BYTE pi_byFlag, BYTE pi_byLevel )
{
	_animus_move_zocl * pRecvMsg = reinterpret_cast< _animus_move_zocl * >( pi_pMsg );

	cnm_create_animus cnmAnimusInfo;

	cnmAnimusInfo.dwServerIndex		= pRecvMsg->wIndex;
	cnmAnimusInfo.dwClientIndex		= pRecvMsg->dwSerial;
	cnmAnimusInfo.dwRecIndex		= pRecvMsg->wRecIndex;
	cnmAnimusInfo.byLevel			= pi_byLevel;
	cnmAnimusInfo.fScale			= 1.0f;

	CAnimus * pAnimus = _GetNetworkMgr()->CreateAnimus( &cnmAnimusInfo );
	if( !pAnimus )
		return FALSE;

	//--------------------------------------------------------------------------

	// 좌표가 같을 경우에는 멈춘다.
	if( ( pRecvMsg->zCur[0] == pRecvMsg->zTar[0] ) && ( pRecvMsg->zCur[2] == pRecvMsg->zTar[1] ) )
	{
		pAnimus->SetAction( CAI_MOVE_STOP );
	}
	if( pi_byFlag != CPF_FIX_POSITION )
	{
		// 위치 설정 => RealMove는 경계선에서 처음 보이는 것 이므로 무조건 서버위치로
		//				맞춰준후에 위치 보정을 한다.
		if( pi_byFlag == CPF_REAL_MOVE )
			pAnimus->SetPosition( pRecvMsg->zCur[0], pRecvMsg->zCur[1], pRecvMsg->zCur[2] );

		short l_pTar[3] = { pRecvMsg->zTar[0], pRecvMsg->zCur[1], pRecvMsg->zTar[1] };
		SetClientCurrentPosition( pAnimus, pRecvMsg->zCur, l_pTar, pi_byFlag );

		Vector3f l_vecCurPath;
		pAnimus->GetCurPath( l_vecCurPath );
		pAnimus->SetRotY( pAnimus->GetPosition(), l_vecCurPath );

		pAnimus->SetFirstMoving( TRUE );
		pAnimus->SetAction( CAI_MOVE_MOVING );
	}
	else
	{
		// 위치 설정 => FixPosition은 경계선에서 처음 보이는 것 이므로
		//              좌표 보정을 하지 않고 무조건 서버위치로 맞춰주어야 한다.
		pAnimus->SetPosition( pRecvMsg->zCur[0], pRecvMsg->zCur[1], pRecvMsg->zCur[2] );
		pAnimus->SetAction( CAI_MOVE_STOP );
	}

	//--------------------------------------------------------------------------

	if( pi_byFlag == CPF_MOVE )
	{
		if( pAnimus->GetCombatState()->GetRemainHP() == ID_INVALID )
			pAnimus->SetHP( pRecvMsg->wHPRate );
	}
	else
	{
		pAnimus->SetHP( pRecvMsg->wHPRate );
	}

	//--------------------------------------------------------------------------

	// 상태 플래그 & 이펙트 코드 설정
	pAnimus->SetServerSFEffectCode( pRecvMsg->wLastEffectCode, 1, TRUE );

	// RealMove도 FixPosition과 동일하게 플래그를 세팅시켜줘야 한다.
	if( pi_byFlag != CPF_MOVE )
	{
		pAnimus->SetReceiveFixPositionMsg( TRUE );

		if( pAnimus->IsEnable( ROSF_CHECK_Y_POS ) )
			pAnimus->CheckYPos();
	}

	//--------------------------------------------------------------------------

	pAnimus->SetReceiveNetMsg( CPF_STRING[pi_byFlag] );

	return TRUE;

	/*
	#define animus_move_zocl 107
	struct _animus_move_zocl {		//add(2/2)
		WORD wRecIndex;
		WORD wIndex;
		DWORD dwSerial;
		short zCur[3];
		short zTar[2];
		WORD wHPRate;
		WORD wLastEffectCode;	//upt(4/12)
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타의 움직임 요청

// pi_vecTargetPos - 움직일 좌표
// pi_bMoveByUserInput - 사용자의 인풋에 의한 움직임인가?
// pi_bLimitMinDistance - 최소 거리 제한을 둘 것인가?
// pi_fChaseDistance - 아바타가 무언가를 쫓아갈 때 사용. 서버에는 실제 거리에서 pi_fChaseDistance 을 빼서 보낸다.

BOOL
CNetworkMgr::PositionMsg_MoveRequest_clzo( float * pi_vecTargetPos, BOOL pi_bMoveByUserInput,
										   BOOL pi_bLimitMinDistance, float pi_fChaseDistance )
{
	if( _GetAvatar()->IsDisable( ROSF_MOVABLE ) )
		return FALSE;

	//--------------------------------------------------------------------------

	const static float VALID_TARGET_DIST = 10.0f;	// 아바타가 이동할 수 있는 최소 유효 거리
	if( pi_bMoveByUserInput )
	{
		// 현재의 거리와 너무 가깝다면 return
		if( pi_bLimitMinDistance )
		{
			if( GetDist( _GetAvatar()->GetPosition(), pi_vecTargetPos ) < VALID_TARGET_DIST )
				return TRUE;
		}
	}

	//--------------------------------------------------------------------------

			static	BYTE	l_byPrevAvatarWalkMode = _GetAvatar()->GetWalkMode();

	const	static	DWORD	MSG_SEND_DELAY_SERVER	= 500;
	const	static	DWORD	MSG_SEND_DELAY_CLIENT	= 200;
			static	DWORD	l_dwPrevSendTime_Client	= 0;
			static	DWORD	l_dwPrevSendTime_Server	= 0;
					DWORD	l_dwCurSendTime			= timeGetTime();

	// 일정 딜레이를 두고 보낸다 - 유저에 의한 입력일 경우
	// 이전과 걷기 모드가 틀리다면 무조건 보낸다.
	if( ( pi_bMoveByUserInput ) && ( l_byPrevAvatarWalkMode == _GetAvatar()->GetWalkMode() ) )
	{
		if( ( l_dwCurSendTime - l_dwPrevSendTime_Client ) < 200 )
			return TRUE;

		l_dwPrevSendTime_Client = l_dwCurSendTime;
	}

	//--------------------------------------------------------------------------

	if( _GetAvatar()->GetConditionInfo()->m_bDisable_Move )
	{
		_SetSystemMsg( "현재 아바타가 이동 제한에 걸려있습니다." );
		return FALSE;
	}

	// 스킬/포스에 의한 이동 제한 체크
	if( _GetAvatar()->GetConditionInfo()->m_bDisable_Run && ( _GetAvatar()->GetWalkMode() == CMM_MOVE_RUN ) )
	{
		_SetSystemMsg( "현재 아바타가 뛰기 제한에 걸려있습니다." );
		return FALSE;
	}

	//--------------------------------------------------------------------------

	_move_request_clzo l_sSend;

	// 아바타의 모드 설정
	l_sSend.byMoveType = _GetAvatar()->GetWalkMode();
	l_sSend.byModeType = _GetAvatar()->GetCombatMode();

	//--------------------------------------------------------------------------

	// 움직이는 도중에 다른곳으로 움직일 수도 있으므로 TRUE로 세팅해 주어야 한다.
	if( pi_bMoveByUserInput )
		_GetAvatar()->SetFirstMoving( TRUE );

	//--------------------------------------------------------------------------

	Vector3f l_vecRealTargetPos;
	memcpy( l_vecRealTargetPos, pi_vecTargetPos, sizeof( float ) * 3 );

	// 현재 위치 설정
	FloatPosToShortPos( l_sSend.zCur, _GetAvatar()->GetPosition() );
	
	//--------------------------------------------------------------------------

	// 타겟 패쓰 생성
	_GetAvatar()->CreatePath( l_vecRealTargetPos );
	_GetAvatar()->GetLastPath( l_vecRealTargetPos );
	_GetAvatar()->SetLastTargetPos( l_vecRealTargetPos );
	if( ( _GetAvatar()->GetPosX() == l_vecRealTargetPos[0] ) && ( _GetAvatar()->GetPosZ() == l_vecRealTargetPos[2] ) )
		return FALSE;

	//--------------------------------------------------------------------------

	// 제한 길이가 있다면 마지막 패쓰부터 계산
	if( pi_fChaseDistance > 0 )
	{
		const int	MAX_PATH_LIST	= 10;
		Vector3f	l_vecPathList[MAX_PATH_LIST];	// 최대 10개라고 가정, [0] = 아바타의 현재 포지션
		int			l_nTotalPath = 1;				// 아바타의 현재 포지션이 기본으로 존재
		int			i;

		// 패쓰 리스트 생성
		memcpy( l_vecPathList[0], _GetAvatar()->GetPosition(), sizeof( Vector3f ) );
		for( i = 0; ( i < _GetAvatar()->GetTotalPathNo() ) && ( i < MAX_PATH_LIST - 1 ); ++i, ++l_nTotalPath )
		{
			if( !_GetAvatar()->GetPath( l_vecPathList[i+1], i ) )
				continue;
		}
	
		// 마지막 패쓰부터 길이 계산
		float		l_fChaseDistance = pi_fChaseDistance;
		for( i = l_nTotalPath - 1; i > 0; --i )
		{
			l_fChaseDistance -= GetDist( l_vecPathList[i], l_vecPathList[i-1] );
			if( l_fChaseDistance <= 0 )
				break;
		}

		// 최종 타겟 포지션 설정
		l_fChaseDistance = GetDist( l_vecPathList[i], l_vecPathList[i-1] ) + l_fChaseDistance;
		_GetWaypointOfAandB( l_vecRealTargetPos, l_vecPathList[i-1], l_vecPathList[i], pi_fChaseDistance );

		// 패쓰 다시 생성
		_GetAvatar()->CreatePath( l_vecRealTargetPos );
		_GetAvatar()->GetLastPath( l_vecRealTargetPos );
		if( ( _GetAvatar()->GetPosX() == l_vecRealTargetPos[0] ) && ( _GetAvatar()->GetPosZ() == l_vecRealTargetPos[2] ) )
			return FALSE;
	}

	// 계산된 값으로 타겟 위치 설정 ( Y값은 없음 )
	l_sSend.zTar[0] = l_vecRealTargetPos[0];
	l_sSend.zTar[1] = l_vecRealTargetPos[2];

	//--------------------------------------------------------------------------

	// 일정 딜레이를 두고 보낸다 - 유저에 의한 입력이 아닐 경우 ( 타겟을 Chase 해 가는 경우 )
	// 이 루틴은 일단 클라이언트에서는 이동을 해야 되기 때문에, 이동 좌표 계산이 끝난 후에 온다.
	// 이전과 걷기 모드가 틀리다면 무조건 보낸다.
	if( l_byPrevAvatarWalkMode == _GetAvatar()->GetWalkMode() )
	{
		if( ( l_dwCurSendTime - l_dwPrevSendTime_Server ) < MSG_SEND_DELAY_SERVER )
		{
			if( !pi_bMoveByUserInput )
			{
				_GetAvatar()->SetAction( CAI_MOVE_MOVING );
				_GetAvatar()->GetCurPath( l_vecRealTargetPos );
			}

			_GetAvatar()->SetRotY( _GetAvatar()->GetPosition(), l_vecRealTargetPos );
			return TRUE;
		}
	}

	l_dwPrevSendTime_Server	= l_dwCurSendTime;
	l_byPrevAvatarWalkMode	= _GetAvatar()->GetWalkMode();

	//--------------------------------------------------------------------------

	// 메세지를 보냄
	BYTE l_nType[] = { position_msg, move_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 sizeof( _move_request_clzo ) );

	// 결과 처리
	if( l_nResult )
	{
		_GetAvatar()->SetAction( CAI_MOVE_MOVING );
		_GetAvatar()->GetCurPath( l_vecRealTargetPos );
		_GetAvatar()->SetRotY( _GetAvatar()->GetPosition(), l_vecRealTargetPos );
	}
	else
	{
		NetLogWrite( "CNetworkMgr::PositionMsg_MoveRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );
	}

	/*
	#define move_request_clzo 1
	struct _move_request_clzo{
		BYTE byMoveType;
		BYTE byModeType;
		short zCur[3];	//xyz
		short zTar[2];	//xz
	};*/

	return l_nResult;
}

// 애니메이션 옵져버를 위한 함수
BOOL
CNetworkMgr::PositionMsg_MoveRequest_Simple_clzo( float * pi_vecTargetPos )
{
	const	static	DWORD MSG_SEND_DELAY_SERVER		= 500;
			static	DWORD l_dwPrevSendTime_Server	= 0;
					DWORD l_dwCurSendTime			= timeGetTime();

	_GetAvatar()->SetPosition( pi_vecTargetPos, TRUE, FALSE );

	if( ( l_dwCurSendTime - l_dwPrevSendTime_Server ) < MSG_SEND_DELAY_SERVER )
		return FALSE;

	l_dwPrevSendTime_Server = l_dwCurSendTime;

	//--------------------------------------------------------------------------

	_move_request_clzo l_sSend;

	l_sSend.byMoveType = _GetAvatar()->GetWalkMode();
	l_sSend.byModeType = _GetAvatar()->GetCombatMode();

	FloatPosToShortPos( l_sSend.zCur, _GetAvatar()->GetPosition() );
	FloatPosToShortPos( l_sSend.zTar, pi_vecTargetPos );

	//--------------------------------------------------------------------------

	// 메세지를 보냄
	BYTE l_nType[] = { position_msg, move_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 sizeof( _move_request_clzo ) );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::PositionMsg_MoveRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	#define move_request_clzo 1
	struct _move_request_clzo{
		BYTE byMoveType;
		BYTE byModeType;
		short zCur[3];	//xyz
		short zTar[2];	//xz
	};*/

	return l_nResult;
}

// 일정 시간마다 아바타의 현재 좌표를 서버로 전송한다.
BOOL
CNetworkMgr::PositionMsg_RealMovposRequest_clzo( void )
{
	_real_movpos_request_clzo l_sSend;

	FloatPosToShortPos( l_sSend.zCur, _GetAvatar()->GetPosition() );

	BYTE l_nType[] = { position_msg, real_movpos_request_clzo };
	BOOL l_bResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 sizeof( _real_movpos_request_clzo ) );

	// 결과 처리
	if( !l_bResult )
		NetLogWrite( "CNetworkMgr::PositionMsg_RealMovposRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_bResult );

	/*
	#define real_movpos_request_clzo 123	//add(5/29)
	struct _real_movpos_request_clzo {
		short zCur[3];	//xyz
	};*/

	return l_bResult;
}

// 아바타의 이동 에러
void
CNetworkMgr::PositionMsg_MoveErrorResult_zocl( char * pi_pMsg )
{
	_move_error_result_zocl l_stRecv;
   	memcpy( &l_stRecv, pi_pMsg, sizeof( _move_error_result_zocl ) );

	_GetAvatar()->SetReceiveNetMsg( "Move Error" );

#ifdef _DEVELOPER_
	_SetSystemMsg( "이동 에러 ( %d )", l_stRecv.byErrCode );
#endif

	if( _GetAvatar()->GetActionByCategory() != CAI_MOVE )
		return;

	/*
	Vector3f l_vecCurPos, l_vecTarPos;
	_GetAvatar()->GetPosition( l_vecCurPos );
	l_vecTarPos[0] = l_stRecv.zCur[0];
	l_vecTarPos[1] = l_stRecv.zCur[1];
	l_vecTarPos[2] = l_stRecv.zCur[2];

	SetClientCurrentPosition( _GetAvatar(), l_vecCurPos, l_vecTarPos, CPF_STOP );
	*/

	_GetAvatar()->SetPosition( l_stRecv.zCur[0], l_stRecv.zCur[1], l_stRecv.zCur[2] );
	_GetAvatar()->SetAction( CAI_MOVE_STOP );

	//PositionMsg_PlayerStop_clzo();

	/*
	#define move_error_result_zocl 22
	struct _move_error_result_zocl {	//add(10/31)
		BYTE byErrCode;
		short zCur[3];	//xyz..서버상의 위치..
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 플레이어의 움직임
void
CNetworkMgr::PositionMsg_PlayerMove_zocl( char * pi_pMsg )
{
	//_player_move_zocl l_stRecv;
	//memcpy( &l_stRecv, pi_pMsg, sizeof( _player_move_zocl ) );

	SetPlayerPosition( pi_pMsg, CPF_MOVE );

	/*
	#define player_move_zocl 4
	struct _player_move_zocl{	
		DWORD dwSerial;
		WORD wIndex;
		WORD wEquipVer;	
		BYTE byRaceCode;
		BYTE byLastEffectCode;
		DWORD dwStateFlag;
		short zCur[3];//xyz
		short zTar[2];	//xz
		BYTE byAddSpeed;	// add(10/23)
	};*/
}

// 몬스터 움직임
void
CNetworkMgr::PositionMsg_MonsterMove_zocl( char * pi_pMsg )
{
	SetMonsterPosition( pi_pMsg, CPF_MOVE );

	/*
#ifdef _DEVELOPER_

	_monster_move_zocl l_Recv;
	memcpy( &l_Recv, pi_pMsg, sizeof( _monster_move_zocl ) );

	CCharacter * pi_pChar = _GetCharMgr()->GetMonster( l_Recv.dwSerial );
	if( !pi_pChar )
		return;

	if( _GetTargetMgr()->IsEqualTarget( pi_pChar ) )
	{
		Vector3f l_vecCur = { l_Recv.zCur[0], l_Recv.zCur[1], l_Recv.zCur[2] };
		Vector3f l_vecTar = { l_Recv.zTar[0], 0.0f, l_Recv.zTar[1] };

		char l_pBuf[128];
		sprintf( l_pBuf, "Server => Cur : %.f %.f %.f, Tar : %.f %.f %.f, Dist : %.f",
				 l_vecCur[0], l_vecCur[1], l_vecCur[2], l_vecTar[0], l_vecTar[1], l_vecTar[2],
				 GetDist( _GetAvatar()->GetPosition(), l_vecCur ) );
		_GetNetworkMgr()->NetLogWrite( l_pBuf );
		sprintf( l_pBuf, "Client => Cur : %.f %.f %.f, Dist : %.f",
				 pi_pChar->GetPosX(), pi_pChar->GetPosY(), pi_pChar->GetPosZ(),
				 GetDist( _GetAvatar()->GetPosition(), pi_pChar->GetPosition() ) );
		NetLogWrite( l_pBuf );
	}
#endif*/

	/*
	#define monster_move_zocl 5
	struct _monster_move_zocl{ 
		WORD wRecIndex;
		WORD wIndex;
		DWORD dwSerial;
		BYTE byLastEffectCode;	//add(8/20)
		short zCur[3];
		short zTar[2];	//x,z	//upt(6/3)
		WORD wHPRate;
		BYTE byInfo[4];
	};
	*/
}

/*
#define npc_move_zocl 106
struct _npc_move_zocl{ 
	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	BYTE byLastEffectCode;	//add(8/20)
	short zCur[3];
	short zTar[2];	//x,z
};*/


// 애니머스 움직임
void
CNetworkMgr::PositionMsg_AnimusMove_zocl( char * pi_pMsg )
{
	SetAnimusPosition( pi_pMsg, CPF_MOVE, 1 );

	/*
	#define animus_move_zocl 107
	struct _animus_move_zocl {		//add(2/2)
		WORD wRecIndex;
		WORD wIndex;
		DWORD dwSerial;
		short zCur[3];
		short zTar[2];
		WORD wHPRate;
		BYTE byLastEffectCode;
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 몬스터가 플레이어를 쫓아옴
void
CNetworkMgr::PositionMsg_MonsterChase_zocl( char * pi_pMsg )
{
	_monster_chase_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _monster_chase_zocl ) );

	cnm_create_monster l_cnmMonsterInfo;

	l_cnmMonsterInfo.dwServerIndex	= l_sRecv.wIndex;
	l_cnmMonsterInfo.dwRecIndex		= l_sRecv.wRecIndex;

	CMonster * l_pMonster = CreateMonster( &l_cnmMonsterInfo );

	if( l_pMonster )
	{
		if( ( l_pMonster->GetActionByCategory() == CAI_DEATH ) || ( l_pMonster->GetAction() == CAI_INTERNALSTATE_DEATH ) )
			return;

		//----------------------------------------------------------------------

		/*
		// 몬스터가 쫓아가는 플레이어를 세팅해준다.
		if( _GetAvatar()->GetIndex() == l_sRecv.wDstIndex )
		{
			l_pMonster->SetTarget( ( CCharacter *)_GetAvatar() );
		}
		else
		{
			CPlayer * l_pPlayer = _GetCharMgr()->GetPlayer( l_sRecv.wDstIndex );
			if( l_pPlayer )
				l_pMonster->SetTarget( ( CCharacter *)l_pPlayer );
		}*/

		//----------------------------------------------------------------------

		short l_pTar[3] = { l_sRecv.zTar[0], l_sRecv.zCur[1], l_sRecv.zTar[1] };
		SetClientCurrentPosition( l_pMonster, l_sRecv.zCur, l_pTar, CPF_MOVE );

		//----------------------------------------------------------------------

		if( l_pMonster->GetCombatState()->GetRemainHP() == ID_INVALID )
			l_pMonster->SetHP( l_sRecv.wHPRate );
		l_pMonster->SetFirstMoving( TRUE );
		l_pMonster->SetAction( CAI_MOVE_MOVING );

		//----------------------------------------------------------------------

		Vector3f l_vecCurPath;
		l_pMonster->GetCurPath( l_vecCurPath );
		l_pMonster->SetRotY( l_pMonster->GetPosition(), l_vecCurPath );

		// 이펙트 코드 설정
		l_pMonster->SetServerSFEffectCode( l_sRecv.byLastEffectCode, 1, TRUE );
	}

	if( l_pMonster ) l_pMonster->SetReceiveNetMsg( "Chase" );

	/*
	#define monster_chase_zocl 6
	struct _monster_chase_zocl{
		WORD wRecIndex;
		WORD wIndex;
		WORD wDstIndex;
		BYTE byLastEffectCode;	//add(8/20)
		short zCur[3];
		short zTar[2]; //x,z	//upt(6/3)
		WORD wHPRate;
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 플레이어의 고정위치
void
CNetworkMgr::PositionMsg_PlayerFixPosition_zocl( char * pi_pMsg )
{
	_player_fixpositon_zocl * pRecvMsg = reinterpret_cast< _player_fixpositon_zocl * >( pi_pMsg );

	_player_move_zocl stSend;
	stSend.dwSerial			= pRecvMsg->dwSerial;
	stSend.wIndex			= pRecvMsg->wIndex;
	stSend.wEquipVer		= pRecvMsg->wEquipVer;
	stSend.byRaceCode		= pRecvMsg->byRaceCode;
	stSend.wLastEffectCode	= pRecvMsg->wLastEffectCode;
	stSend.dwStateFlag		= pRecvMsg->dwStateFlag;
	memcpy( stSend.zCur, pRecvMsg->zCur, sizeof( short ) * 3 );
	stSend.zTar[0]			= stSend.zTar[1] = 0;
	stSend.byAddSpeed		= 0xFF;

	SetPlayerPosition( ( char * )&stSend, CPF_FIX_POSITION );

	/*
	#define player_fixpositon_zocl 7
	struct _player_fixpositon_zocl{
		WORD wIndex;
		DWORD dwSerial;
		WORD wEquipVer;	
		BYTE byRaceCode;
		short zCur[3];
		WORD wLastEffectCode; //최근에 걸린 효과코드..(왼쪽 4비트.. 효과코드(effect_code_skill, effect_code_force, effect_code_class).. 다음부터는 인덱스.. upt(4/12)
		DWORD dwStateFlag;	//upt(8/12)
	};*/
}

// 몬스터의 고정위치
void
CNetworkMgr::PositionMsg_MonsterFixPosition_zocl( char * pi_pMsg )
{
	_monster_fixpositon_zocl l_stRecv;
	memcpy( &l_stRecv, pi_pMsg, sizeof( _monster_fixpositon_zocl ) );

	_monster_move_zocl l_stSend;
	l_stSend.wRecIndex			= l_stRecv.wRecIndex;
	l_stSend.wIndex				= l_stRecv.wIndex;
	l_stSend.dwSerial			= l_stRecv.dwSerial;
	l_stSend.wLastEffectCode	= l_stRecv.wLastEffectCode;
	memcpy( l_stSend.zCur, l_stRecv.zCur, sizeof( short ) * 3 );
	l_stSend.zTar[0]			= l_stSend.zTar[1] = 0;
	l_stSend.wHPRate			= l_stRecv.wHPRate;
	memcpy( l_stSend.byInfo, l_stRecv.byInfo, 4 );

	SetMonsterPosition( ( char * )&l_stSend, CPF_FIX_POSITION );

	/*
	#define monster_fixpositon_zocl 8
	struct _monster_fixpositon_zocl{
		WORD wRecIndex;
		WORD wIndex;
		DWORD dwSerial;
		BYTE wLastEffectCode;	//add(8/20)
		short zCur[3];
		WORD wHPRate;
		BYTE byInfo[4];	
	};
	*/
}

/*
#define npc_fixpositon_zocl 108
struct _npc_fixpositon_zocl{

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	BYTE byLastEffectCode;	//add(8/20)
	short zCur[3];
};*/

// 애니머스의 고정위치
void
CNetworkMgr::PositionMsg_AnimusFixpositon_zocl( char * pi_pMsg )
{
	_animus_fixpositon_zocl * pRecvMsg = reinterpret_cast< _animus_fixpositon_zocl *>( pi_pMsg );

	_animus_move_zocl stSendMsg;
	stSendMsg.wRecIndex			= pRecvMsg->wRecIndex;
	stSendMsg.wIndex			= pRecvMsg->wIndex;
	stSendMsg.dwSerial			= pRecvMsg->dwSerial;
	stSendMsg.wLastEffectCode	= pRecvMsg->wLastEffectCode;
	memcpy( stSendMsg.zCur, pRecvMsg->zCur, sizeof( short ) * 3 );
	stSendMsg.zTar[0]			= stSendMsg.zTar[1] = 0;
	stSendMsg.wHPRate			= pRecvMsg->wHPRate;

	SetAnimusPosition( ( char * )&stSendMsg, CPF_FIX_POSITION, pRecvMsg->byLv );

	/*
	#define animus_fixpositon_zocl 110
	struct _animus_fixpositon_zocl {		//add(2/2)
		WORD wRecIndex;
		WORD wIndex;
		DWORD dwSerial;
		short zCur[3];
		WORD wHPRate;
		BYTE byLv;	//add(2/13)
		BYTE wLastEffectCode;
	};
	*/
}

// 가드타워의 고정위치
void
CNetworkMgr::PositionMsg_TowerFixpositon_zocl( char * pi_pMsg )
{
	_tower_fixpositon_zocl * pRecvMsg = reinterpret_cast< _tower_fixpositon_zocl *>( pi_pMsg );

	cnm_create_guardtower cnmGuardTowerInfo;

	cnmGuardTowerInfo.dwClientIndex	= pRecvMsg->dwSerial;
	cnmGuardTowerInfo.dwServerIndex	= pRecvMsg->wIndex;
	cnmGuardTowerInfo.dwRecIndex	= pRecvMsg->wRecIndex;

	CGuardTower * pGuardTower = CreateGuardTower( &cnmGuardTowerInfo );
	if( !pGuardTower )
		return;

	if( pRecvMsg->wCompLeftSec <= 0 )
		pGuardTower->SetAction( CAI_MOVE_STOP );
	else
		pGuardTower->SetAction( CAI_GESTURE_SUMMON );

	pGuardTower->SetPosition( pRecvMsg->zCur[0], pRecvMsg->zCur[1], pRecvMsg->zCur[2], FALSE );
	pGuardTower->SetRemainBuildTime( pRecvMsg->wCompLeftSec * 1000 );

	pGuardTower->SetReceiveNetMsg( "Fix Position" );

	/*
	#define tower_fixpositon_zocl	111
	struct _tower_fixpositon_zocl {	
		WORD wRecIndex;
		WORD wIndex;
		DWORD dwSerial;
		short zCur[3];
		WORD wCompLeftSec;	//다 짓기위해 남은시간..(0이면 다 지은 타워)
	};
	*/
}

// 아이템의 고정위치
void
CNetworkMgr::PositionMsg_ItemboxFixPosition_zocl( char * pi_pMsg )
{
	_itembox_fixpositon_zocl * pRecvMsg = reinterpret_cast< _itembox_fixpositon_zocl * >( pi_pMsg );

	cnm_create_item cnmItemInfo;

	cnmItemInfo.wIndex		= pRecvMsg->wItemBoxIndex;
	cnmItemInfo.wRecIndex	= pRecvMsg->wItemRecIndex;
	cnmItemInfo.byEquipPart	= pRecvMsg->byItemTableCode;
	cnmItemInfo.vecPos[0]	= ( float )pRecvMsg->zPos[0];
	cnmItemInfo.vecPos[1]	= ( float )pRecvMsg->zPos[1];
	cnmItemInfo.vecPos[2]	= ( float )pRecvMsg->zPos[2];

	CGroundItem * pGroundItem = CreateGroundItem( &cnmItemInfo );

	if( pGroundItem )
	{
		// 상태 처리
		pGroundItem->SetOwner( pRecvMsg->byState, pRecvMsg->dwOwerSerial );
		pGroundItem->SetQuantity( pRecvMsg->byAmount );

		// 위치 설정 => FixPosition은 경계선에서 처음 보이는 것 이므로
		//              좌표 보정을 하지 않고 무조건 서버위치로 맞춰주어야 한다.
		pGroundItem->SetPosition( pRecvMsg->zPos[0], pRecvMsg->zPos[1], pRecvMsg->zPos[2] );

		pGroundItem->SetReceiveFixPositionMsg( TRUE );
		pGroundItem->Disable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );
	}
	
	/*
	#define itembox_fixpositon_zocl 9	
	struct _itembox_fixpositon_zocl{
		BYTE byItemTableCode;
		WORD wItemRecIndex;
		BYTE byAmount;	//중첩아이템일경우 수량
		WORD wItemBoxIndex;
		DWORD dwOwerSerial;	//누가 버린건 0xffff
		short zPos[3];
		BYTE byState;	//0: normal(지정자만이 먹을수잇는상태) 
						//1: open(누구든지 먹을수 있는 상태) 
						//2: hurry(소멸임박,깜빡임)
	};*/
}

// 기갑장비 고정위치
void
CNetworkMgr::PositionMsg_ParkingunitFixpositon_zocl( char * pi_pMsg )
{
	_parkingunit_fixpositon_zocl * pRecvMsg = reinterpret_cast< _parkingunit_fixpositon_zocl * >( pi_pMsg );

	CPlayer * pUnit = _GetCharMgr()->AddUnit( pRecvMsg->dwObjSerial );
	if( !pUnit )
		return;

	// create unit
	pUnit->SetIndex( pRecvMsg->dwObjSerial );
	pUnit->SetServerIndex( pRecvMsg->wObjIndex );
	pUnit->SetCharTypeID( CTI_UNIT );

	// create unit item
	CUnitItem clNewUnit;
	if( !clNewUnit.AddFrame( pRecvMsg->byFrame ) )
		return;
	for( int i = 0; i < MAX_UNIT_PART_TYPE; ++i )
		clNewUnit.AddParts( IEPT_UNIT_HELMET + i, pRecvMsg->byPart[i] );

	// create mesh equipment
	pUnit->MountUnit( &clNewUnit, FALSE );

	// set property
	pUnit->SetPosition( pRecvMsg->zPos[0], pRecvMsg->zPos[1], pRecvMsg->zPos[2] );
	pUnit->SetReceiveFixPositionMsg( TRUE );

	pUnit->SetAction( CAI_GESTURE_STANDBY );
	
	/*
	#define parkingunit_fixpositon_zocl 99	
	struct _parkingunit_fixpositon_zocl{
		WORD wObjIndex;
		DWORD dwObjSerial;
		BYTE byFrame;
		BYTE byPart[UNIT_PART_NUM];
		short zPos[3];
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 몬스터가 멈췄음
void
CNetworkMgr::PositionMsg_MonsterStop_zocl( char * pi_pMsg )
{
	_monster_stop_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _monster_stop_zocl ) );

	CMonster * l_pMonster = ( CMonster * )_GetCharMgr()->GetCharacterByServerIndex( CTI_MONSTER, l_sRecv.wIndex );
	if( !l_pMonster )
		return;
	if( l_pMonster->IsDeath() )
		return;

	Vector3f l_vecCurPos, l_vecTarPos = { l_sRecv.zCur[0], l_sRecv.zCur[1], l_sRecv.zCur[2] };
	l_pMonster->GetPosition( l_vecCurPos );
	SetClientCurrentPosition( l_pMonster, l_vecCurPos, l_vecTarPos, CPF_STOP );

	l_pMonster->SetAction( CAI_MOVE_MOVING );

	l_pMonster->SetReceiveNetMsg( "Stop" );

	/*
	#define monster_stop_zocl 11
	struct _monster_stop_zocl{
		WORD wIndex;
		short zCur[3];
	};*/
}

// 아바타의 멈춤 요청
BOOL
CNetworkMgr::PositionMsg_PlayerStop_clzo( BOOL pi_bSetAction )
{
	_player_stop_clzo l_sSend;

	FloatPosToShortPos( l_sSend.zCur, _GetAvatar()->GetPosition() );

	// 메세지를 보냄
	BYTE l_nType[] = { position_msg, player_stop_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 sizeof( _player_stop_clzo ) );
	// 결과 처리
	if( l_nResult )
	{
		if( pi_bSetAction )
			_GetAvatar()->SetAction( CAI_MOVE_STOP );
	}
	else
	{
		NetLogWrite( "CNetworkMgr::PositionMsg_PlayerStop_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );
	}

	/*
	#define player_stop_clzo 12
	struct _player_stop_clzo{
		short zCur[3];//add(5/29)
	};*/

	return l_nResult;
}

// 아바타의 멈춤 요청에 대한 응답
void
CNetworkMgr::PositionMsg_PlayerStop_zocl( char * pi_pMsg )
{
	_player_stop_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _player_stop_zocl ) );

	if( _GetAvatar()->GetIndex() != l_sRecv.dwSerial )
	{
		CPlayer * l_pPlayer = ( CPlayer * )_GetCharMgr()->GetCharacter( CTI_PLAYER, l_sRecv.dwSerial );
		if( !l_pPlayer )
			return;

		Vector3f l_vecCurPos, l_vecTarPos;
		l_pPlayer->GetPosition( l_vecCurPos );
		ShortPosToFloatPos( l_vecTarPos, l_sRecv.zCur );

		SetClientCurrentPosition( l_pPlayer, l_vecCurPos, l_vecTarPos, CPF_STOP );
	}

	/*
	#define player_stop_zocl 13
	struct _player_stop_zocl{	// upt(8/19)
	//	WORD wIndex;
		DWORD dwSerial;
	//	WORD dwEquipVer;
	//	BYTE byRaceCode;
		short zCur[3];
	};*/

	//if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) PositionMsg_PlayerStop_zocl", ++g_bWriteTestLogCount ); // 테스트 코드
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 플레이어의 움직임 ( real )
void
CNetworkMgr::PositionMsg_PlayerRealMove_zocl( char * pi_pMsg )
{
	_player_real_move_zocl * pRecvMsg = reinterpret_cast< _player_real_move_zocl * >( pi_pMsg );

	_player_move_zocl stSend;
	stSend.dwSerial			= pRecvMsg->dwSerial;
	stSend.wIndex			= pRecvMsg->wIndex;
	stSend.wEquipVer		= pRecvMsg->dwEquipVer;
	stSend.byRaceCode		= pRecvMsg->byRaceCode;
	stSend.wLastEffectCode	= pRecvMsg->wLastEffectCode;
	stSend.dwStateFlag		= pRecvMsg->dwStateFlag;
	memcpy( stSend.zCur, pRecvMsg->zCur, sizeof( short ) * 3 );
	memcpy( stSend.zTar, pRecvMsg->zTar, sizeof( short ) * 2 );
	stSend.byAddSpeed		= pRecvMsg->byAddSpeed;

#ifdef _DEVELOPER_
	if( pRecvMsg->dwSerial == 19264 )
	{
		_WriteLog( "( %d ) Player Real Position - Cur : %d, %d, %d", g_bWriteTestLogCount,
				   pRecvMsg->zCur[0], pRecvMsg->zCur[1], pRecvMsg->zCur[2] );
	}
#endif

	SetPlayerPosition( ( char * )&stSend, CPF_REAL_MOVE );

	/*
	#define player_real_move_zocl 14
	struct _player_real_move_zocl{	
		WORD wIndex;
		DWORD dwSerial;
		WORD dwEquipVer;

		BYTE byRaceCode;
		short zCur[3];
		short zTar[2];	
		BYTE wLastEffectCode; //최근에 걸린 효과코드..(왼쪽 1비트.. 효과코드(effect_code_skill, effect_code_force..).. 다음부터는 인덱스..
		DWORD dwStateFlag;
		BYTE byAddSpeed;	// add(10/23)
	};*/
}

// 몬스터 움직임 ( real )
void
CNetworkMgr::PositionMsg_MonsterRealMove_zocl( char * pi_pMsg )
{
	_monster_real_move_zocl l_stRecv;
	memcpy( &l_stRecv, pi_pMsg, sizeof( _monster_real_move_zocl ) );

	_monster_move_zocl l_stSend;
	l_stSend.wRecIndex			= l_stRecv.wRecIndex;
	l_stSend.wIndex				= l_stRecv.wIndex;
	l_stSend.dwSerial			= l_stRecv.dwSerial;
	l_stSend.wLastEffectCode	= l_stRecv.wLastEffectCode;
	l_stSend.wHPRate			= l_stRecv.wHPRate;
	memcpy( l_stSend.zCur, l_stRecv.zCur, sizeof( short ) * 3 );
	memcpy( l_stSend.zTar, l_stRecv.zTar, sizeof( short ) * 2 );
	memcpy( l_stSend.byInfo, l_stRecv.byInfo, 4 );

	SetMonsterPosition( ( char * )&l_stSend, CPF_REAL_MOVE );

	/*
	#define monster_real_move_zocl 15
	struct _monster_real_move_zocl{
		WORD wRecIndex;
		WORD wIndex;
		DWORD dwSerial;
		BYTE wLastEffectCode;	//add(8/20)
		short zCur[3];
		short zTar[2]; //upt(6/3)
		WORD wHPRate;
		BYTE byInfo[4];
	};
	*/
}

/*
#define npc_real_move_zocl 116
struct _npc_real_move_zocl{		

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	BYTE byLastEffectCode;	//add(8/20)
	short zCur[3];
	short zTar[2];
};*/

// 애니머스 움직임 ( real )
void
CNetworkMgr::PositionMsg_AnimusRealMove_zocl( char * pi_pMsg )
{
	_animus_real_move_zocl * pRecvMsg = reinterpret_cast< _animus_real_move_zocl * >( pi_pMsg );

	_animus_real_move_zocl stSendMsg;
	stSendMsg.wRecIndex			= pRecvMsg->wRecIndex;
	stSendMsg.wIndex			= pRecvMsg->wIndex;
	stSendMsg.dwSerial			= pRecvMsg->dwSerial;
	stSendMsg.wLastEffectCode	= pRecvMsg->wLastEffectCode;
	stSendMsg.wHPRate			= pRecvMsg->wHPRate;
	memcpy( stSendMsg.zCur, pRecvMsg->zCur, sizeof( short ) * 3 );
	memcpy( stSendMsg.zTar, pRecvMsg->zTar, sizeof( short ) * 2 );

	SetAnimusPosition( ( char * )&stSendMsg, CPF_REAL_MOVE, pRecvMsg->byLv );

	/*
	#define animus_real_move_zocl 117
	struct _animus_real_move_zocl {		//add(2/2)
		WORD wRecIndex;
		WORD wIndex;
		DWORD dwSerial;
		short zCur[3];
		short zTar[2];
		WORD wHPRate;
		BYTE byLv;	//add(2/13)
		BYTE wLastEffectCode;
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 플레이어의 상태 플래그 세팅
void
CNetworkMgr::PositionMsg_StateInform_zocl( char * pi_pMsg )
{
	_state_inform_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _state_inform_zocl ) );

	CPlayer * l_pPlayer = ( CPlayer * )_GetCharMgr()->GetCharacter( CTI_PLAYER, l_sRecv.dwSerial );
	if( !l_pPlayer )
		return;

	l_pPlayer->SetServerStatusFlag( l_sRecv.dwState );

	/*
	#define state_inform_zocl 16	
	struct _state_inform_zocl {		
		DWORD dwSerial;
		DWORD dwState; //upt(8/12)
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 해당맵의 베이스 포탈(0번 포탈)로 이동한다.
BOOL
CNetworkMgr::PositionMsg_GotoBaseportalRequest_clzo( DWORD pi_dwIndex )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;

	_goto_baseportal_request_clzo l_sSend;

#ifdef _DEVELOPER_

	/* => 개발자 버젼에서는 베이스 맵에 상관없이 무조건 이동
	if( pi_dwIndex == ID_INVALID )
	{
		BOOL l_bIsMovable = TRUE;

		if( ( _GetAvatar()->GetRace() == CRT_BELLATOR_MALE ) || ( _GetAvatar()->GetRace() == CRT_BELLATOR_FEMALE ) )
		{
			if( !( ( _GetLand()->GetMapIndex() == 0 ) || ( _GetLand()->GetMapIndex() == 3 ) ) )
				l_bIsMovable = FALSE;
		}
		else if( ( _GetAvatar()->GetRace() == CRT_CORA_MALE ) || ( _GetAvatar()->GetRace() == CRT_CORA_FEMALE ) )
		{
			if( !( ( _GetLand()->GetMapIndex() == 1 ) || ( _GetLand()->GetMapIndex() == 4 ) ) )
				l_bIsMovable = FALSE;
		}
		else
		{
			if( !( ( _GetLand()->GetMapIndex() == 2 ) || ( _GetLand()->GetMapIndex() == 5 ) ) )
				l_bIsMovable = FALSE;
		}

		if( !l_bIsMovable )
		{
			_SetSystemMsg( "/마을 명령어는 아바타 종족의 베이스 맵이나 킵에서만 사용할 수 있습니다." );
			return FALSE;
		}
	}*/

#endif

	_GetGBM()->CloseUnnecessaryWindow();

	l_sSend.wItemSerial	= pi_dwIndex;

	_GetGBM()->SetRequestProcessState( TRUE );

	BYTE l_nType[] = { position_msg, goto_baseportal_request_clzo };
	BOOL l_bResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 &l_sSend,
									 sizeof( _goto_baseportal_request_clzo ) );

	// 결과 처리
	if( l_bResult )
	{
		MOVEMAP_INFO * pMoveMapInfo = _GetAvatar()->GetMoveMapInfo();
		if( !pMoveMapInfo )
			return FALSE;

		pMoveMapInfo->m_bIsMoved = TRUE;
		pMoveMapInfo->m_byEnterMapIndex = _GetLand()->GetMapIndex();

		_GetLand()->SetMapIndex( ID_INVALID );
		_GetLand()->SetPortalIndex( ID_INVALID );
	}
	else
	{
		NetLogWrite( "CNetworkMgr::PositionMsg_GotoBaseportalRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_bResult );
	}

	/*
	#define goto_baseportal_request_clzo 17
	struct _goto_baseportal_request_clzo {
		WORD wItemSerial;	//town item 시리얼 //add(12/07).. /마을 => 0xffff
	};*/

	return l_bResult;
}

void
CNetworkMgr::PositionMsg_GotoBaseportalResult_zocl( char * pi_pMsg )
{
	_goto_baseportal_result_zocl * pRecvMsg = reinterpret_cast< _goto_baseportal_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );
	_GetAvatar()->SetAction( CAI_MOVE_STOP );

	if( pRecvMsg->byRet == 0 )
	{
		if( _GetDungeonInfo()->IsInDungeon() )
		{
			_GetDungeonInfo()->SetDungeonExitType( DET_PORTAL );
			_GetDungeonInfo()->ExitDungeon();
		}

		MOVEMAP_INFO * pMoveMapInfo = _GetAvatar()->GetMoveMapInfo();
		if( !pMoveMapInfo )
			return;

		pMoveMapInfo->m_vecPosition[0] = pRecvMsg->zPos[0];
		pMoveMapInfo->m_vecPosition[1] = pRecvMsg->zPos[1];
		pMoveMapInfo->m_vecPosition[2] = pRecvMsg->zPos[2];

		pMoveMapInfo->m_byExitMapIndex = pRecvMsg->byMapCode;

		pMoveMapInfo->MoveToPortal_Result( GST_BASE_PORTAL );

		/*
		if( _GetLand()->GetMapIndex() == pRecvMsg->byMapCode )
		{
			_GetAvatar()->SetPosition( pRecvMsg->zPos[0], pRecvMsg->zPos[1], pRecvMsg->zPos[2] );
			InitMsg_NewPosStartRequest_clzo( GST_BASE_PORTAL );
		}
		else
		{
			MOVEMAP_INFO * pMoveMapInfo = _GetAvatar()->GetMoveMapInfo();
			if( !pMoveMapInfo )
				return;

			pMoveMapInfo->m_byExitMapIndex	= pRecvMsg->byMapCode;

			pMoveMapInfo->m_vecPosition[0]	= pRecvMsg->zPos[0];
			pMoveMapInfo->m_vecPosition[1]	= pRecvMsg->zPos[1];
			pMoveMapInfo->m_vecPosition[2]	= pRecvMsg->zPos[2];

			pMoveMapInfo->MoveToPortal_Result( GST_BASE_PORTAL );
		}*/

		CBoardItem * pUsingItem = _GetAvatar()->GetUsingItem();
		if( pUsingItem )
			_GetInventoryBoard()->UI_UseResult( pUsingItem->GetIndex() );
	}
	else
	{
		_SetSystemMsg( "정해진 장소로 이동할 수 없습니다." );
	}

	/*
	#define goto_baseportal_result_zocl 18
	struct _goto_baseportal_result_zocl {
		BYTE byRet;//0;//성공//1;//시체상태임 2;//포탈을못읽음 3;//좌표 못얻음 4;//다른종족맴
		BYTE byMapCode;	
		short zPos[3];
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 입력한 이름을 가진 유저가 있는 장소로 이동 ( 같은 존에서만 해당 됨 )
BOOL
CNetworkMgr::PositionMsg_GotoAvatorRequest_clzo( char * pi_pPlayerName )
{
	_goto_avator_request_clzo l_sSend;

	_strcpy( l_sSend.szAvatorName, pi_pPlayerName, max_name_len + 1 );

	BYTE l_nType[] = { position_msg, goto_avator_request_clzo };
	BOOL l_bResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 sizeof( _goto_avator_request_clzo ) );

	// 결과 처리
	if( !l_bResult )
		NetLogWrite( "CNetworkMgr::PositionMsg_GotoAvatorRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_bResult );

	/*
	#define goto_avator_request_clzo 19
	struct _goto_avator_request_clzo {		//add(4/24)
		char szAvatorName[max_name_len+1];
	};*/

	return l_bResult;
}

void
CNetworkMgr::PositionMsg_GotoAvatorResult_zocl( char * pi_pMsg )
{
	_goto_avator_result_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _goto_avator_result_zocl ) );

	if( l_sRecv.byRet == 0 )
	{
		// 다른 맵일 경우
		if( l_sRecv.byMapCode != _GetLand()->GetMapIndex() )
		{
			_GetLand()->UnloadLevel();
			_GetLand()->LoadLevel( l_sRecv.byMapCode );
			_GetLand()->SetMapIndex( l_sRecv.byMapCode );

			// 모든 캐릭터 데이터를 삭제한다.
			_GetCharMgr()->ClearCharacter( CTI_PLAYER );
			_GetCharMgr()->ClearCharacter( CTI_NPC );
			_GetCharMgr()->ClearCharacter( CTI_MONSTER );

			// 상인 리스트를 요청한다.			
			_GetShopTradeBoard()->StoreListRequest();
		}

		_GetAvatar()->SetPosition( l_sRecv.fStartPos[0], l_sRecv.fStartPos[1], l_sRecv.fStartPos[2] );
		//_GetAvatar()->SetPrevPosition( l_sRecv.fStartPos[0]+.5f, l_sRecv.fStartPos[1], l_sRecv.fStartPos[2] );

		_GetAvatar()->SetAction( CAI_MOVE_STOP );

		InitMsg_NewPosStartRequest_clzo( GST_TELEPORT );
	}
	else
	{
		switch( l_sRecv.byRet )
		{
			case 1	: _SetSystemMsg( "순간 이동 실패 : 해당 플레이어가 다른 존에 있습니다." ); break;
			case 2	: _SetSystemMsg( "순간 이동 실패 : 해당 플레이어가 없습니다." ); break;
			default	: _SetSystemMsg( "순간 이동 실패 : 알 수 없는 에러." );
		}
	}

	/*
	#define goto_avator_result_zocl 20	
	struct _goto_avator_result_zocl {	//add(4/24)
		BYTE byRet;//0;//성공//1;//댜른존에 있음 2;//없음
		BYTE byMapCode;
		float fStartPos[3];
	};*/
}

// 모든 캐릭터의 멈춤 메세지
void
CNetworkMgr::PositionMsg_BreakStopResult_zocl( char * pi_pMsg )
{
	_break_stop_result_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _break_stop_result_zocl ) );

	CCharacter * l_pChar = _GetCharMgr()->GetCharacter( l_sRecv.byObjID, l_sRecv.dwObjSerial );
	if( l_pChar )
	{
		Vector3f l_vecTar = { l_sRecv.zCur[0], l_sRecv.zCur[1], l_sRecv.zCur[2] };
		SetClientCurrentPosition( l_pChar, l_pChar->GetPosition(), l_vecTar, CPF_STOP );

		l_pChar->GetCurPath( l_vecTar );
		l_pChar->SetRotY( l_pChar->GetPosition(), l_vecTar );

		l_pChar->SetFirstMoving( TRUE );
		l_pChar->SetAction( CAI_MOVE_MOVING );

		l_pChar->SetReceiveNetMsg( "Break Stop" );
	}

	/*
	#define break_stop_result_zocl 21
	struct _break_stop_result_zocl {	//add(10/18)	..높이등의 문제땜에 중간에 멈춰야하는경우..
		BYTE byObjID;	
		DWORD dwObjSerial;
		short zCur[3];
	};*/
}

// 일정 시간마다 오브젝트의 위치가 날라옴
void
CNetworkMgr::PositionMsg_ObjectRealFixpositon_zocl( char * pi_pMsg )
{
	_object_real_fixpositon_zocl * pRecvMsg = reinterpret_cast< _object_real_fixpositon_zocl * >( pi_pMsg );

	if( pRecvMsg->byObjKind == obj_kind_char )
	{
		CCharacter * pChar = _GetCharMgr()->GetCharacter( pRecvMsg->byObjID, pRecvMsg->dwSerial );
		if( pChar )
		{
			pChar->SetReceiveFixPositionMsg( TRUE );
			pChar->SetReceiveNetMsg( "Real Fix" );
		}
	}
	else if( pRecvMsg->byObjKind == obj_kind_item )
	{
		if( pRecvMsg->byObjID == obj_id_parkingunit )
		{
			CPlayer * pUnit = _GetCharMgr()->GetUnit( pRecvMsg->dwSerial );
			if( pUnit )
				pUnit->SetReceiveFixPositionMsg( TRUE );
		}
		else
		{
			CGroundItem * pItem = _GetGroundItemMgr()->GetItem( pRecvMsg->wIndex );
			if( pItem )
				pItem->SetReceiveFixPositionMsg( TRUE );
		}
	}

	/*
	#define object_real_fixpositon_zocl 77
	struct _object_real_fixpositon_zocl{	//add(10/08).. 2초마다..
		BYTE byObjKind;
		BYTE byObjID;
		WORD wIndex;
		DWORD dwSerial;
	};

	#define obj_kind_num				2	//오브젝트 종류수
	#define obj_kind_char				0	//케릭터	(이동, 전투 가능한 오브젝트)	
	#define obj_kind_item				1	//아이템	(이동, 전투가 불가능한 오브젝트)

	#define obj_id_player				0	//플레이어 코드
	#define obj_id_monster				1	//몬스트 코드
	#define obj_id_npc					2	//NPC
	#define obj_id_animus				3	//애니머스	
	#define OBJ_CHAR_ID_NUM				4

	#define obj_id_itembox				0	//아이템박스
	#define obj_id_dungeongate			1	//던젼구멍
	#define obj_id_parkingunit			2	//정차중인 기갑장비
	#define OBJ_ITEM_ID_NUM				3
	*/
}

// 걷기 모드 변경 요청 => 결과로는 server flaga가 날라옴
BOOL
CNetworkMgr::PositionMsg_MoveTypeChangeRequest_clzo( BYTE pi_byMoveMode )
{
	_move_type_change_request_clzo l_stSend;

	static	DWORD l_dwPrevSendTime = 0;
			DWORD l_dwCurTime = timeGetTime();

	if( ( l_dwCurTime - l_dwPrevSendTime ) < 1000 )
		return FALSE;

	l_dwPrevSendTime = l_dwCurTime;
	
	l_stSend.byMoveType = pi_byMoveMode;

	// 메세지를 보냄
	BYTE l_nType[] = { position_msg, move_type_change_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_stSend,
									 sizeof( _move_type_change_request_clzo ) );

	// 결과 처리
	if( l_nResult )
		_GetAvatar()->SetWalkMode( pi_byMoveMode );
	else
		NetLogWrite( "CNetworkMgr::PositionMsg_MoveTypeChangeRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	return l_nResult;

	/*
	#define move_type_change_request_clzo 222
	struct _move_type_change_request_clzo{	//add(10/23)
		BYTE byMoveType;	//	move_type_walk:0, move_type_run:1 
	};
	*/
}
