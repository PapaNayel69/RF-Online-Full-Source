////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_ItemTrade Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../../GameObject/Manager/CCharacterMgr.h"
#include "../../../DataTable/CItemDataMgr.h"
#include "../../../GameObject/Board/General/CGameEventBoard.h"
#include "../../../GameObject/Board/Common/CBoardManager.h"

#include "CNetMsgProcessor_Event.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_Event::CNetMsgProcessor_Event()
{
}

CNetMsgProcessor_Event::~CNetMsgProcessor_Event()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetMsgProcessor_Event::SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	return CNetMsgProcessor_Base::SendNetMessage( event_msg, pi_byMsgIndex, pi_pMsg, pi_nSize );
}
			
bool	
CNetMsgProcessor_Event::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	if( pMsgHeader->m_byType[0] != event_msg )
		return false;

	switch( pMsgHeader->m_byType[1] )
	{
		case level_up_zocl	:
			LevelUp_zocl( pi_pMsg );
			break;

		case recover_zocl	:
			Recover_zocl( pi_pMsg );
			break;

		case max_hfsp_zocl	:
			MaxHfsp_zocl( pi_pMsg );
			break;

		case exp_alter_zocl	:
			ExpAlter_zocl( pi_pMsg );
			break;

		case change_class_command_zocl	:
			ChangeClassCommand_zocl( pi_pMsg );
			break;

		case select_class_result_zocl	:
			SelectClassResult_zocl( pi_pMsg );
			break;

		case change_class_other_zocl	:
			ChangeClassOther_zocl( pi_pMsg );
			break;

		case reward_add_item_zocl	:
			RewardAddItem_zocl( pi_pMsg );
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

// 플레이어의 레벨업
void
CNetMsgProcessor_Event::LevelUp_zocl( char * pi_pMsg )
{
	_level_up_zocl * pRecvMsg = reinterpret_cast< _level_up_zocl * >( pi_pMsg );

	CPlayer * pPlayer = _GetCharMgr()->GetPlayer( pRecvMsg->dwSerial );
	if( !pPlayer )
		return;

	pPlayer->SetLevel( pRecvMsg->byLevel );

	if( pPlayer->IsEqualCharType( CTI_AVATAR ) )
	{
		_GetAvatar()->SetExp( 0 );

		if( !_GetAvatar()->IsEqualUnitMountState( UMS_MOUNT ) )
		{
			_GetAvatar()->SetHP( _GetAvatar()->GetMaxHP() );
			_GetAvatar()->SetFP( _GetAvatar()->GetMaxFP() );
			_GetAvatar()->SetSP( _GetAvatar()->GetMaxSP() );
		}
	}

	pPlayer->SetEffectInfo( CAI_GESTURE_LEVEL_UP );
	pPlayer->SetReceiveNetMsg( "Level Up ^0^)/" );

	/*
	#define level_up_zocl	1
	struct _level_up_zocl{
		DWORD dwSerial;//플레이어의 시리얼..
		BYTE byLevel;
	};	*/
}

// 아바타의 현재 HP, FP, SP 양
void
CNetMsgProcessor_Event::Recover_zocl( char * pi_pMsg )
{
	_recover_zocl * pRecvMsg = reinterpret_cast< _recover_zocl * >( pi_pMsg );

	if( _GetAvatar()->IsEqualUnitMountState( UMS_DISMOUNT ) )
	{
		_GetAvatar()->SetHP( pRecvMsg->wHP );
		_GetAvatar()->SetFP( pRecvMsg->wFP );
		_GetAvatar()->SetSP( pRecvMsg->wST );

		if( pRecvMsg->wST <= 0 )
			_GetAvatar()->SetWalkMode( CMM_MOVE_WALK );
	}

	_GetAvatar()->SetReceiveNetMsg( "Recover" );

	/*
	#define recover_zocl	2
	struct _recover_zocl{
		WORD wHP;
		WORD wFP;
		WORD wST;
	};
	*/
}

// 아바타의 최대 HP, FP, SP 양
void
CNetMsgProcessor_Event::MaxHfsp_zocl( char * pi_pMsg )
{
	_max_hfsp_zocl * pRecvMsg = reinterpret_cast< _max_hfsp_zocl * >( pi_pMsg );

	_GetAvatar()->SetMaxHP( pRecvMsg->wMaxHP );
	_GetAvatar()->SetMaxFP( pRecvMsg->wMaxFP );
	_GetAvatar()->SetMaxSP( pRecvMsg->wMaxST );

	_GetAvatar()->SetReceiveNetMsg( "Max HP/FP/SP" );

	/*
	#define max_hfsp_zocl	3
	struct _max_hfsp_zocl{
		WORD wMaxHP;
		WORD wMaxFP;
		WORD wMaxST;
	};*/
}

// 아바타의 현재 경험치
void
CNetMsgProcessor_Event::ExpAlter_zocl( char * pi_pMsg )
{
	_exp_alter_zocl * pRecvMsg = reinterpret_cast< _exp_alter_zocl * >( pi_pMsg );

	_GetAvatar()->SetExp( pRecvMsg->wExpRate / 100.0f );

	_GetAvatar()->SetReceiveNetMsg( "EXP" );

	/*
	#define exp_alter_zocl	4	//add(3/17)
	struct _exp_alter_zocl	{
		WORD wExpRate;	//만분율..
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 서버에서 전달된 전직 명령
void
CNetMsgProcessor_Event::ChangeClassCommand_zocl( char * pi_pMsg )
{
	_change_class_command_zocl * pRecvMsg = reinterpret_cast< _change_class_command_zocl * >( pi_pMsg );

	CClassEvent clClassEvent;
	clClassEvent.SetType( GET_CLASS );
	clClassEvent.SetClassEventType( CClassEvent::RECEIVE_CLASS_CHANGE );
	_GetGameEventBoard()->PushEvent( clClassEvent );

	/*
	#define change_class_command_zocl 5	//add(4/10)
	struct _change_class_command_zocl {		//전직 명령
		WORD	wCurClassIndex;
	};*/
}

// 전직 요청
BOOL	
CNetMsgProcessor_Event::SelectClassRequest_clzo( DWORD pi_dwNewClassDTIndex )
{
	_select_class_request_clzo stSendMsg;

	stSendMsg.wSelClassIndex	= pi_dwNewClassDTIndex;

	_GetGBM()->SetRequestProcessState( TRUE );

	return SendNetMessage( select_class_request_clzo,
						   &stSendMsg,
						   sizeof( _select_class_request_clzo ) );

	/*
	#define select_class_request_clzo 6	//add(4/10)
	struct _select_class_request_clzo {		//class 선택
		WORD	wSelClassIndex;
	};*/
}

// 아바타의 전직 결과
void
CNetMsgProcessor_Event::SelectClassResult_zocl( char * pi_pMsg )
{
	_select_class_result_zocl * pRecvMsg = reinterpret_cast< _select_class_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byRetCode == 0 )
	{
		_GetClassChangeBoard()->UI_ClassChangeResult();
	}
	else
	{
		switch( pRecvMsg->byRetCode )
		{
			case 0	:
			default : _SetSystemMsg( "전직 관련 에러 : 알 수 없는 에러 ( %d )", pRecvMsg->byRetCode );
		}
	}

	/*
	#define select_class_result_zocl 7	//add(4/10)
	struct _select_class_result_zocl {		//자신에게 전직결과
		BYTE	byRetCode;
	};*/
}

// 타 플레이어의 전직 결과
void
CNetMsgProcessor_Event::ChangeClassOther_zocl( char * pi_pMsg )
{
	_change_class_other_zocl * pRecvMsg = reinterpret_cast< _change_class_other_zocl * >( pi_pMsg );

	CPlayer * pPlayer = _GetCharMgr()->GetPlayer( pRecvMsg->dwPlayerSerial );
	if( pPlayer )
		pPlayer->SetEffectInfo( CAI_GESTURE_CLASS_CHANGE );

	/*
	#define change_class_other_zocl 8	//add(4/10)
	struct _change_class_other_zocl {		//주변에게 효과보여주기위함.
		DWORD	dwPlayerSerial;
		WORD	wClassIndex;
	};*/
}

// 전직이나 퀘스트의 보상 아이템
void
CNetMsgProcessor_Event::RewardAddItem_zocl( char * pi_pMsg )
{
	static const char const pRewardTypw[3][32] = { "일반", "전직", "퀘스트" };

	_reward_add_item_zocl * pRecvMsg = reinterpret_cast< _reward_add_item_zocl * >( pi_pMsg );

	CBoardItem clNewItem;

	// create
	if( !clNewItem.Create( IEPT_SERVER_TO_CLIENT[ pRecvMsg->byTableCode ], pRecvMsg->wItemIndex ) )
		return;

	// set property
	clNewItem.SetIndex( pRecvMsg->wItemSerial );
	clNewItem.SetServerIndex( pRecvMsg->wItemSerial );

	if( clNewItem.IsSplitable() )
		clNewItem.SetQuantity( pRecvMsg->dwDur );
	else
		clNewItem.SetDurability( pRecvMsg->dwDur );

	// upgrade info ( animus item 일 경우 hp/fp 가 있음 => CBoardItem 에서는 내구성으로 저장을 했다가 CAnimusItem 으로 이동시 HP/FP로 변경시킨다. )
	if( clNewItem.IsEqualItemKind( IEPT_ANIMUS ) )
		clNewItem.SetDurability( pRecvMsg->dwLv );
	else 
		clNewItem.InsertTalikFromServerFlag( pRecvMsg->dwLv );

	// Find Emtpy Socket
	BYTE byPackNo, bySocketNo;
	if( !_GetInventoryBoard()->FindEmptySocket( byPackNo, bySocketNo ) )
	{
		_SetSystemMsg( "보상 아이템 추가에 실패 했습니다. 인벤토리에 빈 공간이 없습니다." );
		return;
	}

	// Add to Inventory
	if( _GetInventoryBoard()->Add( byPackNo, bySocketNo, &clNewItem ) )
	{
		_SetSystemMsg( "보상 아이템을 인벤토리 %d번째 가방 %d번째 칸에 추가 하였습니다.", byPackNo + 1, bySocketNo + 1 );
	}

	/*
	#define reward_add_item_zocl 9	
	struct _reward_add_item_zocl {	//아이템 보상(전직, 퀘스트 보상)
		BYTE	byTableCode;
		WORD	wItemIndex;
		DWORD	dwDur;
		DWORD	dwLv;
		WORD	wItemSerial;

	//#define reward_case_std			0	//일반적인경우
	//#define reward_case_class			1	//전직
	//#define reward_case_quest			2	//퀘스트

		BYTE    byReason;	//add(5/6)
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////