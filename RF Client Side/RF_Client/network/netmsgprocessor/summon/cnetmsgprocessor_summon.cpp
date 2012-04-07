////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Summon Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../../../GameObject/Manager/CCharacterMgr.h"
#include "../../../DataFile.h"
#include "../../../DataTable/CItemDataMgr.h"

#include "../../../Item/CItemData.h"
#include "../../../GameObject/Board/Common/CBoardManager.h"

#include "CNetMsgProcessor_Summon.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_Summon::CNetMsgProcessor_Summon()
{
}

CNetMsgProcessor_Summon::~CNetMsgProcessor_Summon()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetMsgProcessor_Summon::SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	return CNetMsgProcessor_Base::SendNetMessage( recall_msg, pi_byMsgIndex, pi_pMsg, pi_nSize );
}

bool
CNetMsgProcessor_Summon::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	if( pMsgHeader->m_byType[0] != recall_msg )
		return false;

	switch( pMsgHeader->m_byType[1] )
	{
		case animus_recall_result_zocl :
			AnimusRecallResult_zocl( pi_pMsg );
			break;

		case animus_return_result_zocl :
			AnimusReturnResult_zocl( pi_pMsg );
			break;

		case animus_command_inform_zocl :
			AnimusCommandIinform_zocl( pi_pMsg );
			break;

		case animus_target_result_zocl :
			AnimusTargetResult_zocl( pi_pMsg );
			break;

		case animus_hp_inform_zocl :
			AnimusHpInform_zocl( pi_pMsg );
			break;

		case animus_fp_inform_zocl :
			AnimusFpInform_zocl( pi_pMsg );
			break;

		case animus_exp_inform_zocl :
			AnimusExpInform_zocl( pi_pMsg );
			break;

		case animus_lvup_inform_zocl :
			AnimusLvupInform_zocl( pi_pMsg );
			break;

		default :
			m_pNetworkMgr->NetLogWrite( "Unknown Message : CNetMsgProcessor_Summon::AnalysisServerPacket()\n" );
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 소환 요청
BOOL
CNetMsgProcessor_Summon::AnimusRecallRequest_clzo( CAnimusItem * pi_pAnimusItem )
{
	if( !pi_pAnimusItem )
		return FALSE;

	_animus_recall_request_clzo stSendMsg;

	stSendMsg.dwAnimusItemSerial	= pi_pAnimusItem->GetIndex();
	stSendMsg.wAnimusClientHP		= pi_pAnimusItem->GetHP();
	stSendMsg.wAnimusClientFP		= pi_pAnimusItem->GetFP();

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( animus_recall_request_clzo,
						   &stSendMsg,
						   sizeof( _animus_recall_request_clzo ) );

	/*
	#define animus_recall_request_clzo 1	//소환
	struct _animus_recall_request_clzo {	//add(2/2)
		DWORD dwAnimusItemSerial;	//애니머스창에있는 아이템의 시리얼..
		WORD  wAnimusClientHP;		//클라이언트상의 회복된 HP
		WORD  wAnimusClientFP;		//클라이언트상의 회복된 FP
	};*/
}

// 소환 결과
void
CNetMsgProcessor_Summon::AnimusRecallResult_zocl( char * pi_pMsg )
{
	_animus_recall_result_zocl * pRecvMsg = reinterpret_cast< _animus_recall_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byResultCode == 0 )
	{
		_GetAnimusBoard()->UI_SummonResult( TRUE );

		CAnimus * pAnimus = _GetCharMgr()->GetAnimus( pRecvMsg->dwAnimusSerial );
		if( !pAnimus )
			return;

		_GetAnimusControlBoard()->SetAnimusChar( pAnimus );

		pAnimus->SetHP( pRecvMsg->wAnimusHP );
		pAnimus->SetFP( pRecvMsg->wAnimusFP );
		pAnimus->SetDataByExp( pRecvMsg->dwAnimusExp );

		_GetAvatar()->SetFP( pRecvMsg->wLeftFP );
	}
	else
	{
		_GetAnimusBoard()->UI_SummonResult( FALSE );
		ShowServerErrorMessage( pRecvMsg->byResultCode );
	}

	/*
	#define animus_recall_result_zocl 2
	struct _animus_recall_result_zocl {	
		BYTE byResultCode;
		DWORD dwAnimusSerial;		//애니머스 케릭터의 시리얼
		WORD  wAnimusHP;			//애니머스 HP
		WORD  wAnimusFP;			//애니머스 FP
		DWORD dwAnimusExp;			//애니머스 Exp //upt(5/6)
		WORD  wLeftFP;				//자신의 FP
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 귀환 요청
BOOL
CNetMsgProcessor_Summon::AnimusReturnRequest_clzo( void )
{
	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( animus_return_request_clzo,
						   NULL,
						   0 );

	/*
	#define animus_return_request_clzo 3	//귀환
	struct _animus_return_request_clzo {	//add(2/2)
	};*/
}

// 귀환 결과
void
CNetMsgProcessor_Summon::AnimusReturnResult_zocl( char * pi_pMsg )
{
	_animus_return_result_zocl * pRecvMsg = reinterpret_cast< _animus_return_result_zocl * >( pi_pMsg );

	// 플레이어가 귀환시킨 경우
	if( pRecvMsg->byReturnType == animus_return_type_command )
	{
		_GetGBM()->SetRequestProcessState( FALSE );

		if( pRecvMsg->byResultCode == 0 )
		{
			_GetAnimusBoard()->UI_ReturnResult( TRUE );
		}
		else
		{
			_GetAnimusBoard()->UI_ReturnResult( FALSE );
			ShowServerErrorMessage( pRecvMsg->byResultCode );
		}
	}
	// 서버에서 재소환 요청이 온 경우
	else if( pRecvMsg->byReturnType == animus_return_type_path )
	{
		_GetAnimusControlBoard()->SetRequestResummon( TRUE );
	}
	// 서버에서 귀환시킨 경우
	else
	{
		_GetAnimusControlBoard()->SetAnimusItem( NULL );
		_GetAnimusControlBoard()->CloseWindow();

		_SetSystemMsg( "애니머스가 귀환되었습니다." );
	}

	/*
	#define animus_return_result_zocl 4	//_animus_return_request_clzo의 결과 또는 서버상의 강제귀환의 경우..
	struct _animus_return_result_zocl {	
		BYTE  byResultCode;
		WORD  wAnimusItemSerial;

	//#define animus_return_type_command		0	//쥔장명령
	//#define animus_return_type_ai				1	//애니머스 스스로
	//#define animus_return_type_rule			2	//룰적인 처리 (멥이동, 쥔장사망)
	//#define animus_return_type_path			3	//패스목적상.. 곧바로 다시 나옴..

		BYTE	byReturnType; //add(5/10)
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 애니머스에게 명령
BOOL
CNetMsgProcessor_Summon::AnimusCommandRequest_clzo( BYTE pi_byCommand )
{
	_animus_command_request_clzo stSendMsg;

	stSendMsg.byCommandCode	= pi_byCommand;

	if( SendNetMessage( animus_command_request_clzo,
					   &stSendMsg,
					   sizeof( _animus_command_request_clzo ) ) )
	{
		if( pi_byCommand == animus_act_type_action )
		{
			_SetSystemMsg( "애니머스에게 공격 명령을 전달하였습니다." );
		}
		else if( pi_byCommand == animus_act_type_follow )
		{
			_SetSystemMsg( "애니머스에게 이동 명령을 전달하였습니다." );
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}

	/*
	#define animus_command_request_clzo 5	//애니머스에 명령.. 
	struct _animus_command_request_clzo {	//add(2/2)
		//#define animus_act_type_action	0	//액쑌
		//#define animus_act_type_follow	1	//Follow me!
		BYTE byCommandCode;
	};*/
}

// 애니머스의 상태 변경
void
CNetMsgProcessor_Summon::AnimusCommandIinform_zocl( char * pi_pMsg )
{
	_animus_command_inform_zocl * pRecvMsg = reinterpret_cast< _animus_command_inform_zocl * >( pi_pMsg );

	/*
	#define animus_command_inform_zocl 6	//애니머스의 상태변경.. 내부적으로 모드가 바뀔적마다..
	struct _animus_command_inform_zocl {	//add(2/2)
		BYTE  byStateCode;
	};*/
}

// 애니머스의 타겟 설정
BOOL
CNetMsgProcessor_Summon::AnimusTargetRequest_clzo( CCharacter * pi_pTarget )
{
	if( !pi_pTarget )
		return FALSE;

	_animus_target_request_clzo stSendMsg;

	stSendMsg.byObjectID		= pi_pTarget->GetCharTypeID();
	stSendMsg.wObjectIndex		= pi_pTarget->GetServerIndex();
	stSendMsg.dwObjectSerial	= pi_pTarget->GetIndex();

	return SendNetMessage( animus_target_request_clzo,
						   &stSendMsg,
						   sizeof( _animus_target_request_clzo ) );
	/*
	#define animus_target_request_clzo 7	//목표물
	struct _animus_target_request_clzo {	//add(2/2)
		BYTE	byObjectID;
		WORD	wObjectIndex;
		DWORD	dwObjectSerial;
	};*/
}

// 애니머스의 타겟 설정
void
CNetMsgProcessor_Summon::AnimusTargetResult_zocl( char * pi_pMsg )
{
	_animus_target_result_zocl * pRecvMsg = reinterpret_cast< _animus_target_result_zocl * >( pi_pMsg );

	if( pRecvMsg->byResultCode == 0 )
	{
		_SetSystemMsg( "애니머스의 타겟을 설정하였습니다." );
	}
	else
	{
		_SetSystemMsg( "애니머스 타겟 설정 에러 ( %d )",  pRecvMsg->byResultCode );
	}

	/*
	#define animus_target_result_zocl 8
	struct _animus_target_result_zocl {		//add(2/2)
		BYTE  byResultCode;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNetMsgProcessor_Summon::AnimusHpInform_zocl( char * pi_pMsg )
{
	_animus_hp_inform_zocl * pRecvMsg =
		reinterpret_cast< _animus_hp_inform_zocl * >( pi_pMsg );

	if( _GetAnimusControlBoard()->IsEqualSummonAnimusItem( pRecvMsg->wAnimusItemSerial ) )
	{
		_GetAnimusControlBoard()->SetHP( pRecvMsg->wLeftHP );
	}
	else
	{
		CAnimusItem * pAnimusItem = _GetAnimusBoard()->GetAnimusByIndex( pRecvMsg->wAnimusItemSerial );
		if( pAnimusItem )
			pAnimusItem->SetHP( pRecvMsg->wLeftHP );
	}
	

	/*
	#define animus_hp_inform_zocl 9
	struct _animus_hp_inform_zocl {			//add(2/2)
		WORD wAnimusItemSerial;
		WORD wLeftHP;
	};*/
}

void
CNetMsgProcessor_Summon::AnimusFpInform_zocl( char * pi_pMsg )
{
	_animus_fp_inform_zocl * pRecvMsg =
		reinterpret_cast< _animus_fp_inform_zocl * >( pi_pMsg );

	if( _GetAnimusControlBoard()->IsEqualSummonAnimusItem( pRecvMsg->wAnimusItemSerial ) )
	{
		_GetAnimusControlBoard()->SetFP( pRecvMsg->wLeftFP );
	}
	else
	{
		CAnimusItem * pAnimusItem = _GetAnimusBoard()->GetAnimusByIndex( pRecvMsg->wAnimusItemSerial );
		if( pAnimusItem )
			pAnimusItem->SetFP( pRecvMsg->wLeftFP );
	}

	/*
	#define animus_fp_inform_zocl 10
	struct _animus_fp_inform_zocl {			//add(2/2)
		WORD wAnimusItemSerial;
		WORD wLeftFP;
	};*/
}

void
CNetMsgProcessor_Summon::AnimusExpInform_zocl( char * pi_pMsg )
{
	_animus_exp_inform_zocl * pRecvMsg =
		reinterpret_cast< _animus_exp_inform_zocl * >( pi_pMsg );

	if( _GetAnimusControlBoard()->IsEqualSummonAnimusItem( pRecvMsg->wAnimusItemSerial ) )
	{
		_GetAnimusControlBoard()->SetExp( pRecvMsg->dwExp );
	}
	else
	{
		CAnimusItem * pAnimusItem = _GetAnimusBoard()->GetAnimusByIndex( pRecvMsg->wAnimusItemSerial );
		if( pAnimusItem )
			pAnimusItem->SetExp( pRecvMsg->dwExp );
	}

	/*
	#define animus_exp_inform_zocl 11		//애니머스의 경험치.. 케릭터 스킬은 따로 옴..
	struct _animus_exp_inform_zocl {		//add(2/2)
		WORD wAnimusItemSerial;
		DWORD dwExp;
	};*/
}

void
CNetMsgProcessor_Summon::AnimusLvupInform_zocl( char * pi_pMsg )
{
	_animus_lvup_inform_zocl * pRecvMsg =
		reinterpret_cast< _animus_lvup_inform_zocl * >( pi_pMsg );

	CAnimus * pAnimus = _GetCharMgr()->GetAnimus( pRecvMsg->dwSerial );
	if( pAnimus )
		pAnimus->SetEffectInfo( CAI_GESTURE_LEVEL_UP );

	/*
	#define animus_lvup_inform_zocl 12		//애니머스의 레벨업.. (주변..)
	struct _animus_lvup_inform_zocl {		//add(2/13)
		WORD wIndex;
		DWORD dwSerial;
		BYTE byLv;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNetMsgProcessor_Summon::ShowServerErrorMessage( DWORD pi_dwErrorCode )
{
	switch( pi_dwErrorCode )
	{
		case error_recall_already_recalled	: _SetSystemMsg( "소환 관련 에러 : 이미 소환중입니다. ( %d )", pi_dwErrorCode ); break;
		case error_recall_nothing_item		: _SetSystemMsg( "소환 관련 에러 : 애니머스 리버가 없습니다. ( %d )", pi_dwErrorCode ); break;
		case error_recall_hp_under_30		: _SetSystemMsg( "소환 관련 에러 : 애니머스의 HP가 30 %% 이하입니다. ( %d )", pi_dwErrorCode ); break;
		case error_recall_fp_lack			: _SetSystemMsg( "소환 관련 에러 : FP가 부족합니다. ( %d )", pi_dwErrorCode ); break;
		case error_recall_max_animus		: _SetSystemMsg( "소환 관련 에러 : 지금은 소환할 수 없습니다. 잠시후 다시 시도해 주십시오. ( %d )", pi_dwErrorCode ); break;
		case error_recall_map_move			: _SetSystemMsg( "소환 관련 에러 : 맵이동 중입니다. ( %d )", pi_dwErrorCode ); break;
		case error_recall_not_recalled		: _SetSystemMsg( "소환 관련 에러 : 소환 상태가 아닙니다. ( %d )", pi_dwErrorCode ); break;
		case error_recall_target_invalid	: _SetSystemMsg( "소환 관련 에러 : 타겟이 올바르지 않습니다. ( %d )", pi_dwErrorCode ); break;
		case error_recall_delay				: _SetSystemMsg( "소환 관련 에러 : 지금은 소환할 수 없습니다. 잠시후 다시 시도해 주십시오. ( %d )", pi_dwErrorCode ); break;
		case error_recall_not_actable_race	: _SetSystemMsg( "소환 관련 에러 : 코라 종족만이 소환할 수 있습니다. ( %d )", pi_dwErrorCode ); break;
		case error_recal_few_mastery		: _SetSystemMsg( "소환 관련 에러 : 소환 마스터리가 부족합니다. ( %d )", pi_dwErrorCode ); break;
		default	: _SetSystemMsg( "소환 관련 에러 : 알 수 없는 에러 ( %d )", pi_dwErrorCode ); break;
	}
}

#endif // _NEW_UI_