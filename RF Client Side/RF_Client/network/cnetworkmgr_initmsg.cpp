////////////////////////////////////////////////////////////////////////////////
//
// CNetworkMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../GameObject/Manager/CTargetMgr.h"
#include "../GameObject/Manager/CGroundItemManager.h"
#include "../GameObject/Board/Common/CBoardManager.h"

#include "../CLand.h"
#include "../CCamera.h"

#include "../UserInterface/GameWindow/GUIAttackPartWindow.h"
#include "../UserInterface/GameWindow/Mining/GUIMiningWindow.h"

#include "../GameObject/Board/General/CPartyBoard.h"

extern BOOL  g_bWriteTestLog;		// 테스트 코드
extern DWORD g_bWriteTestLogCount;	// 테스트 코드

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////    Internal Function     ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 플레이어가 부활시 필요한 데이터들을 세팅해준다.
void
CNetworkMgr::SettingPlayerRebirth( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return;

	if( pi_pPlayer->IsEqualCharType( CTI_AVATAR ) )
		_GetGBM()->CloseUnnecessaryWindow();

	// 죽은 후 다시 부활하기 할 때는 애니메이션 보간을 사용하지 않음
	pi_pPlayer->Disable( ROSF_ANIMATION_INTERPOLATION );

	pi_pPlayer->SetWalkMode( CMM_MOVE_RUN );
	pi_pPlayer->SetCombatMode( CSI_PEACE );
	
	_GetCamera()->SetSpecialCameraMode(0);	//특수카메라 원상복귀
	SetStateFullScreenEffect(FALSE);
	pi_pPlayer->DismountUnit( FALSE );
	pi_pPlayer->SetAction( CAI_MOVE_STOP );
	pi_pPlayer->GetAppliedSF()->Clear();
	pi_pPlayer->GetCombatState()->Clear();
	pi_pPlayer->SetEffectInfo( CAI_CREATE_CREATE );
	pi_pPlayer->Enable( ROSF_ANIMATION_INTERPOLATION | ROSF_ANIMATION_LOOP );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////


// 현재 아바타의 영역안에서 몬스터가 생성되었음 ( 일반 몬스터 )
void
CNetworkMgr::InitMsg_MonsterCreate_zocl( char * pi_pMsg )
{
	_monster_create_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _monster_create_zocl ) );

	cnm_create_monster l_cnmMonsterInfo;

	l_cnmMonsterInfo.dwClientIndex	= l_sRecv.dwSerial;
	l_cnmMonsterInfo.dwServerIndex	= l_sRecv.wIndex;
	l_cnmMonsterInfo.dwRecIndex		= l_sRecv.wRecIndex;
	l_cnmMonsterInfo.fScale			= (float)l_sRecv.byInfo[0] / 100.0f;

	CMonster * l_pMonster = CreateMonster( &l_cnmMonsterInfo );

	if( l_pMonster )
	{
		l_pMonster->SetCreationType( CCT_NORMAL );
		l_pMonster->SetEffectInfo( CAI_CREATE_CREATE );

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

		l_pMonster->SetPosition( l_sRecv.zPos[0], l_sRecv.zPos[1], l_sRecv.zPos[2] );
		l_pMonster->CheckYPos();
		l_pMonster->CreatePath( l_pMonster->GetPosition() );
		l_pMonster->SetRotY( 0, FALSE );
	}

	/*
	#define monster_create_zocl 1
	struct _monster_create_zocl{
		WORD wRecIndex;
		WORD wIndex;
		DWORD dwSerial;
		short zPos[3];
		//BYTE byCreateMode;//0: respawn 1: recall  //del(6/22)
		BYTE byInfo[4];

	};*/
}

// 현재 아바타의 영역안에서 몬스터가 생성되었음 ( 소환 몬스터 )
void
CNetworkMgr::InitMsg_RecallMonCreate_zocl( char * pi_pMsg )
{
	_recallmon_create_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _recallmon_create_zocl ) );

	cnm_create_monster l_cnmMonsterInfo;

	l_cnmMonsterInfo.dwClientIndex	= l_sRecv.dwSerial;
	l_cnmMonsterInfo.dwServerIndex	= l_sRecv.wIndex;
	l_cnmMonsterInfo.dwRecIndex		= l_sRecv.wRecIndex;
	l_cnmMonsterInfo.fScale			= (float)l_sRecv.byInfo[0] / 100.0f;

	CMonster * l_pMonster = CreateMonster( &l_cnmMonsterInfo );

	if( l_pMonster )
	{
		l_pMonster->SetCreationType( CCT_SUMMON );
		l_pMonster->SetEffectInfo( CAI_GESTURE_SUMMON );

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

		l_pMonster->SetTextColor( CHAR_SUMMON_MONSTER_NAME_COLOR );

		l_pMonster->SetPosition( l_sRecv.zPos[0], l_sRecv.zPos[1], l_sRecv.zPos[2] );
		l_pMonster->CheckYPos();
		l_pMonster->CreatePath( l_pMonster->GetPosition() );
		l_pMonster->SetRotY( 0, FALSE );
	}

	/*
	#define recallmon_create_zocl 113
	struct _recallmon_create_zocl{	//add(6/22)
		WORD wRecIndex;
		WORD wIndex;
		DWORD dwSerial;
		short zPos[3];
		BYTE byInfo[4];
	};*/

	//if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_RecallMonCreate_zocl", ++g_bWriteTestLogCount ); // 테스트 코드
}

/*
// 현재 아바타의 영역안에서 NPC가 생성되었음
#define npc_create_zocl 114
struct _npc_create_zocl { // add(2/2)

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	short zPos[3];
};*/

void
CNetworkMgr::InitMsg_AnimusCreate_zocl( char * pi_pMsg )
{
	_animus_create_zocl * pRecvMsg = reinterpret_cast< _animus_create_zocl * >( pi_pMsg );

	cnm_create_animus cnmAnimusInfo;

	cnmAnimusInfo.dwClientIndex		= pRecvMsg->dwSerial;
	cnmAnimusInfo.dwServerIndex		= pRecvMsg->wIndex;
	cnmAnimusInfo.dwRecIndex		= pRecvMsg->wRecIndex;
	cnmAnimusInfo.byLevel			= pRecvMsg->byLv;
	cnmAnimusInfo.fScale			= 1.0f;

	CAnimus * pAnimus = CreateAnimus( &cnmAnimusInfo );

	if( pAnimus )
	{
		pAnimus->SetPosition( pRecvMsg->zPos[0], pRecvMsg->zPos[1], pRecvMsg->zPos[2] );
		pAnimus->CheckYPos();
		pAnimus->CreatePath( pAnimus->GetPosition() );
		pAnimus->SetRotY( 0, FALSE );
	}

	/*
	// 현재 아바타의 영역안에서 Animus가 생성되었음
	#define animus_create_zocl 115
	struct _animus_create_zocl {		//add(2/2)
		WORD wIndex;
		WORD wRecIndex;
		DWORD dwSerial;
		short zPos[3];
		BYTE byLv;	//add(2/13)
	};*/
}

void
CNetworkMgr::InitMsg_TowerCreate_zocl( char * pi_pMsg )
{
	_tower_create_zocl * pRecvMsg = reinterpret_cast< _tower_create_zocl * >( pi_pMsg );

	cnm_create_guardtower cnmGuardTowerInfo;

	cnmGuardTowerInfo.dwClientIndex		= pRecvMsg->dwSerial;
	cnmGuardTowerInfo.dwServerIndex		= pRecvMsg->wIndex;
	cnmGuardTowerInfo.dwRecIndex		= pRecvMsg->wRecIndex;

	CGuardTower * pGuardTower = CreateGuardTower( &cnmGuardTowerInfo );

	if( pGuardTower )
	{
		pGuardTower->SetAction( CAI_GESTURE_SUMMON );
		pGuardTower->SetPosition( pRecvMsg->zPos[0], pRecvMsg->zPos[1], pRecvMsg->zPos[2] );
		pGuardTower->CheckYPos();
		pGuardTower->CreatePath( pGuardTower->GetPosition() );
		pGuardTower->SetRotY( 0, FALSE );
	}

	/*
	#define tower_create_zocl 116
	struct _tower_create_zocl {	//add(3/17)
		WORD wIndex;
		WORD wRecIndex;
		DWORD dwSerial;
		short zPos[3];	
	};*/
}

// 현재 아바타의 영역안에서 그라운드 아이템이 생성되었음
void
CNetworkMgr::InitMsg_ItemboxCreate_zocl( char * pi_pMsg )
{
	_itembox_create_zocl * pRecvMsg = reinterpret_cast< _itembox_create_zocl * >( pi_pMsg );

	cnm_create_item cnmItemInfo;

	cnmItemInfo.wIndex		= pRecvMsg->wBoxIndex;
	cnmItemInfo.wRecIndex	= pRecvMsg->wItemRecIndex;
	cnmItemInfo.byEquipPart	= pRecvMsg->byItemTableCode;
	cnmItemInfo.vecPos[0]	= ( float )pRecvMsg->zPos[0];
	cnmItemInfo.vecPos[1]	= ( float )pRecvMsg->zPos[1];
	cnmItemInfo.vecPos[2]	= ( float )pRecvMsg->zPos[2];

	CGroundItem * pNewItem = CreateGroundItem( &cnmItemInfo );

	if( pNewItem )
	{
		pNewItem->SetOwner( pRecvMsg->byState, pRecvMsg->dwOwerSerial );
		pNewItem->SetDroper( pRecvMsg->idDumber.byID, pRecvMsg->idDumber.dwSerial );
		pNewItem->SetQuantity( pRecvMsg->byAmount );

		// 몬스터일 경우에만 해당됨
		if( pRecvMsg->idDumber.byID == CTI_MONSTER )
			pNewItem->Disable( ROSF_RENDER | ROSF_ANIMATION );
	}

	/*#define itembox_create_zocl 2
	#define itembox_create_zocl 12
	struct _itembox_create_zocl{	
		BYTE byItemTableCode;
		WORD wItemRecIndex;
		BYTE byAmount;	//중첩아이템일경우 갯수..
		WORD wBoxIndex;		
		DWORD dwOwerSerial;	//1차로 줏을수잇는 권리자..
		_CHRID idDumber;	//add(3/24)..버린사람.. 버린사람정보없으면 0xFF..
		BYTE byState;
		short zPos[3];
	};*/

	//if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_ItemboxCreate_zocl : Index - %d, Pos - %d, %d, %d",
	//	++g_bWriteTestLogCount, l_sRecv.wItemIndex, l_sRecv.zPos[0] , l_sRecv.zPos[1] , l_sRecv.zPos[2] ); // 테스트 코드
}

// 현재 아바타의 영역안에서 기갑장비가 생성되었음
void
CNetworkMgr::InitMsg_ParkingUnitCreate_zocl( char * pi_pMsg )
{
	_parkingunit_create_zocl * pRecvMsg = reinterpret_cast< _parkingunit_create_zocl * >( pi_pMsg );

	CPlayer * pPlayer = _GetCharMgr()->GetPlayer( pRecvMsg->dwOwerSerial );
	if( !pPlayer )
		return;

	CPlayer * pUnit = _GetCharMgr()->AddUnit( pRecvMsg->dwObjSerial );
	if( !pUnit )
		return;

	// create unit
	pUnit->SetIndex( pRecvMsg->dwObjSerial );
	pUnit->SetServerIndex( pRecvMsg->wObjIndex );
	pUnit->SetCharTypeID( CTI_UNIT );

	pPlayer->SetLinkPlayer( pUnit );
	pUnit->SetLinkPlayer( pPlayer );

	// create unit item
	CUnitItem clNewUnit;
	if( !clNewUnit.AddFrame( pRecvMsg->byFrame ) )
		return;
	for( int i = 0; i < MAX_UNIT_PART_TYPE; ++i )
		clNewUnit.AddParts( IEPT_UNIT_HELMET + i, pRecvMsg->byPart[i] );

	// create mesh equipment
	pUnit->MountUnit( &clNewUnit, TRUE );

	// set property
	pUnit->SetPosition( pRecvMsg->zPos[0], pRecvMsg->zPos[1], pRecvMsg->zPos[2] );
	pUnit->Disable( ROSF_OUT_OF_VISUAL_FIELD_SERVER | ROSF_OUT_OF_VISUAL_FIELD_CLIENT );
	pUnit->SetReceiveFixPositionMsg( TRUE );

	if( pRecvMsg->byCreateType == unit_create_type_delivery )
		pUnit->SetAction( CAI_GESTURE_STANDBY );
	else
		pUnit->SetAction( CAI_GESTURE_DISMOUNT );

	/*
	#define parkingunit_create_zocl 124
	struct _parkingunit_create_zocl{	//add(2/12)
		WORD wObjIndex;
		DWORD dwObjSerial;
		BYTE byCreateType;	//#define unit_create_type_delivery	0	//출고	//#define unit_create_type_leave		1	//하차
		BYTE byFrame;
		BYTE byPart[UNIT_PART_NUM];
		DWORD dwOwerSerial;	
		short zPos[3];
		BYTE byTransDistCode;	//#define unit_trans_short_range		0	//유닛 근거리수송 #define unit_trans_long_range		1	//유닛 원거리수송
	};*/
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 현재 아바타의 영역안에서 플레어어 접속을 종료하거나 맵 이동을 하였음
void
CNetworkMgr::InitMsg_PlayerDestroy_zocl( char * pi_pMsg )
{
	_player_destroy_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _player_destroy_zocl ) );

	CPlayer * l_pPlayer = ( CPlayer * )_GetCharMgr()->GetCharacter( CTI_PLAYER, l_sRecv.dwSerial );
	if( !l_pPlayer )
		return;
	l_pPlayer->SetServerIndex( l_sRecv.wIndex );

	// 게임 종료
	if( l_sRecv.byState == 0 )
	{
		l_pPlayer->SetQuitGame( TRUE );
		l_pPlayer->SetEffectInfo( CAI_GESTURE_GAME_LOGOUT );

		// 자신의 파티원일 경우에는 지우지 않는다. => PartyMsg_PartyLeaveSelfResult_zocl 에서 지운다.
		if( !_GetPartyBoard()->IsPartyMember( l_sRecv.dwSerial ) )
			_GetCharMgr()->DeleteCharacter( CTI_PLAYER, l_sRecv.dwSerial );
			//_GetCharMgr()->SaveToCache( l_pPlayer );
	}
	// 존 이동
	else
	{
		// 자신의 파티원일 경우에는 지우지 않는다.
		if( !_GetPartyBoard()->IsPartyMember( l_sRecv.dwSerial ) )
		{
			_GetCharMgr()->DeleteCharacter( CTI_PLAYER, l_sRecv.dwSerial );
			//_GetCharMgr()->SaveToCache( l_pPlayer );
		}
		else
		{
			CPlayer * l_pPlayer = ( CPlayer * )_GetCharMgr()->GetCharacter( CTI_PLAYER, l_sRecv.dwSerial );
			if( !l_pPlayer )
				return;

			l_pPlayer->Enable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT );
			l_pPlayer->Enable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );
		}
	}

	l_sRecv.byState;	// 이펙트 처리할 것이 있으면 사용

	/*
	#define player_destroy_zocl 13
	struct _player_destroy_zocl{	//존서버종료//
		WORD wIndex;
		DWORD dwSerial;

		BYTE byState; //0: 게임종료, 1: 존이동	//upt(3/5)
	};*/

	//if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_PlayerDestroy_zocl : Server Index %d, State : %d",
	//	++g_bWriteTestLogCount, l_sRecv.wIndex, l_sRecv.byState ); // 테스트 코드
}

// 현재 아바타의 영역안에서 플레어어가 죽었음
void
CNetworkMgr::InitMsg_PlayerDie_zocl( char * pi_pMsg )
{
	_player_die_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _player_die_zocl ) );

	CPlayer * l_pPlayer = NULL;

	l_pPlayer = ( CPlayer * )_GetCharMgr()->GetCharacter( CTI_PLAYER, l_sRecv.dwSerial );
	if( !l_pPlayer ) return;
	l_pPlayer->SetServerIndex( l_sRecv.wIndex );

	if( l_pPlayer->IsEqualCharType( CTI_AVATAR ) )
	{
		_GetAvatar()->StopTargetPursuit( FALSE );	// SetAction( CAI_DEATH_READY ) 전에 호출해야한다.
		_GetAvatar()->SetHP( 0 );
		_GetAvatar()->SetAction( CAI_DEATH_READY );
	}
	else if( l_pPlayer->GetActionByCategory() != CAI_DEATH )
	{
		l_pPlayer->SetHP( 0 );
		l_pPlayer->SetAction( CAI_INTERNALSTATE_DEATH );
	}

	if( l_pPlayer )
		l_pPlayer->SetReceiveNetMsg( "Death" );

	/*
	#define player_die_zocl 14
	struct _player_die_zocl{	//add(3/5)	//시체로 죽음//
		WORD wIndex;
		DWORD dwSerial;
	};*/

	//if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_PlayerDie_zocl : Server Index %d, State : %d",
	//	++g_bWriteTestLogCount, l_sRecv.wIndex, l_sRecv.byState ); // 테스트 코드
}

// 현재 아바타의 영역안에서 몬스터가 파괴되었음 ( 혹은, 죽었음 ) ( 일반 몬스터 )
void
CNetworkMgr::InitMsg_MonsterDestroy_zocl( char * pi_pMsg )
{
	_monster_destroy_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _monster_destroy_zocl ) );

	CMonster * l_pMonster = _GetCharMgr()->GetMonster( l_sRecv.dwSerial );
	if( !l_pMonster )
	{
		l_pMonster = ( CMonster * )_GetCharMgr()->GetCharacterByServerIndex( CTI_MONSTER, l_sRecv.wIndex );
		if( !l_pMonster )
			return;
	}
	l_pMonster->SetServerIndex( l_sRecv.wIndex );
	l_pMonster->SetDeath( l_sRecv.byDestroyCode );
 
	l_pMonster->SetReceiveNetMsg( "Death" );

	/*
	#define monster_destroy_zocl 17
	struct _monster_destroy_zocl{
		WORD wIndex;
		DWORD dwSerial;	
		BYTE byDestroyCode;	//add(10/19).. 0.. die, 1.. respawn
	};*/
}

// 현재 아바타의 영역안에서 NPC가 파괴되었음 ( 혹은, 죽었음 )
/*
#define npc_destroy_zocl 118
struct _npc_destroy_zocl{		//add(2/2)

	WORD wIndex;
	DWORD dwSerial;	
};*/

// 현재 아바타의 영역안에서 애니머스가 파괴되었음 ( 혹은, 죽었음 )
void
CNetworkMgr::InitMsg_AnimusDestroy_zocl( char * pi_pMsg )
{
	_animus_destroy_zocl * pRecvMsg = reinterpret_cast< _animus_destroy_zocl * >( pi_pMsg );

	CAnimus * pAnimus = _GetCharMgr()->GetAnimus( pRecvMsg->dwSerial );
	if( !pAnimus )
		return;
	pAnimus->SetServerIndex( pRecvMsg->wIndex );

	// 소환 해제
	if( pRecvMsg->byDestroyCode == animus_des_type_back )
	{
		pAnimus->SetAction( CAI_GESTURE_REPATRIATE );
	}
	// 죽음
	else
	{
		if( !pAnimus->IsEqualActionCategory( CAI_DEATH ) )
		{
			pAnimus->GetCombatState()->SetRemainHP( 0 );
			pAnimus->SetAction( CAI_INTERNALSTATE_DEATH );
		}
	}

	pAnimus->SetReceiveNetMsg( "Death" );

	/*
	#define animus_destroy_zocl 119
	struct _animus_destroy_zocl {	//add(2/2)
		WORD wIndex;
		DWORD dwSerial;
		BYTE byDestroyCode;
	};*/
}

// 현재 아바타의 영역안에서 가드타워가 파괴되었음 ( 혹은, 죽었음 )
void
CNetworkMgr::InitMsg_TowerDestroy_zocl( char * pi_pMsg )
{
	_tower_destroy_zocl * pRecvMsg = reinterpret_cast< _tower_destroy_zocl * >( pi_pMsg );

	CGuardTower * pGuardTower = _GetCharMgr()->GetGuardTower( pRecvMsg->dwSerial );
	if( !pGuardTower )
		return;
	pGuardTower->SetServerIndex( pRecvMsg->wIndex );

	// 파괴
	if( pRecvMsg->byDestroyCode == tower_des_type_destroy )
	{
		if( !pGuardTower->IsEqualActionCategory( CAI_DEATH ) )
		{
			pGuardTower->GetCombatState()->SetRemainHP( 0 );
			pGuardTower->SetAction( CAI_INTERNALSTATE_DEATH );
		}
	}
	// 회수 or 설치자 게임 종료
	else
	{
		pGuardTower->SetAction( CAI_GESTURE_REPATRIATE );
	}

	pGuardTower->SetReceiveNetMsg( "Death" );

	/*
	#define tower_destroy_zocl 120
	struct _tower_destroy_zocl {	//add(3/17)
		WORD wIndex;
		DWORD dwSerial;

	//#define tower_des_type_back			0	//회수
	//#define tower_des_type_destroy		1	//파괴
	//#define tower_des_type_disconnect	2	//설치자 게임종료

		BYTE byDestroyCode;	//맞아죽나..회수냐..
	};*/
}

// 현재 아바타의 영역안에서 아이템이 파괴되었음
void
CNetworkMgr::InitMsg_ItemboxDestroy_zocl( char * pi_pMsg )
{
	_itembox_destroy_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _itembox_destroy_zocl ) );

	CGroundItem * pItem = _GetGroundItemMgr()->GetItem( l_sRecv.wIndex );// 서버 인덱스를 이용해 아이템을 얻어온다.
	if( pItem )
		pItem->DeleteItemEffect();	//아이템에 붙은 효과를 지운다.
		
	_GetGroundItemMgr()->DeleteItem( l_sRecv.wIndex );

	//WORD wIndex;

//	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_ItemboxDestroy_zocl : Index - %d", ++g_bWriteTestLogCount, l_sRecv.wIndex ); // 테스트 코드
}

// 현재 아바타의 영역안에서 기갑장비가 파괴되었음
void
CNetworkMgr::InitMsg_ParkingUnitDestroy_zocl( char * pi_pMsg )
{
	_parkingunit_destroy_zocl * pRecvMsg = reinterpret_cast< _parkingunit_destroy_zocl * >( pi_pMsg );

	CPlayer * pUnit = _GetCharMgr()->GetUnit( pRecvMsg->dwObjSerial );
	if( !pUnit )
		return;

	CPlayer * pPlayer = pUnit->GetLinkPlayer();
	if( pPlayer )
		pPlayer->SetLinkPlayer( NULL );

	_GetCharMgr()->DeleteCharacter( CTI_UNIT, pRecvMsg->dwObjSerial );

	/*
	#define parkingunit_destroy_zocl 188
	struct _parkingunit_destroy_zocl{	//add(2/12)
		WORD wObjIndex;
		DWORD dwObjSerial;
		BYTE byDestroyCode;//#define unit_destory_type_return	0	//입고#define unit_destory_type_take		1	//승차
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 다른 플레이어의 외형을 요청
BOOL
CNetworkMgr::InitMsg_OtherShapeRequest_clzo( CPlayer * pi_pPlayer, BOOL pi_bIsRequestAllPart )
{
	if( !pi_pPlayer )
		return FALSE;

	// 플레이어마다 일정 시간 간격으로 전송
	static const EQUIP_ITEM_REQ_TIME = 5000;
	DWORD pi_dwCurTime = timeGetTime();
	if( pi_dwCurTime - pi_pPlayer->GetEquipmentRequestTime() < EQUIP_ITEM_REQ_TIME )
		return FALSE;

	pi_pPlayer->SetEquipmentRequestTime( pi_dwCurTime );

	//--------------------------------------------------------------------------

	// 메세지 정보
	_other_shape_request_clzo l_sSend;
	l_sSend.wIndex		= pi_pPlayer->GetServerIndex();
	l_sSend.byReqType	= !pi_bIsRequestAllPart;

	// 메세지를 보냄
	BYTE l_nType[] = { init_msg, other_shape_request_clzo };
	BOOL l_bResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 sizeof( _other_shape_request_clzo ) );

	/*
	#define other_shape_request_clzo 17	
	struct _other_shape_request_clzo	{
		WORD wIndex;	//대상캐릭터의 인덱스
	//	DWORD dwSerial;	//대상캐릭터의 시리얼 //del(6/3)
		
		enum { SHAPE_ALL = 0, SHAPE_PART };

		BYTE byReqType;	//요구유형(0이면 첨보는 상대(other_shape_all_zocl). 1이면 다시만난 버젼이 다른 상대)..
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_OtherShapeRequest_clzo : %d, %d", ++g_bWriteTestLogCount, pi_pPlayer->GetServerIndex(), pi_pPlayer->GetIndex() ); // 테스트 코드

	return l_bResult;
}

// 플레이어의 외형 정보 요청 성공 - _other_shape_request_clzo ( SHAPE_ALL ) 에 대한 응답
void
CNetworkMgr::InitMsg_OtherShapeAll_zocl( char * pi_pMsg )
{
	_other_shape_all_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _other_shape_all_zocl ) );

	cnm_create_player l_cnmPlayerInfo;

	l_cnmPlayerInfo.dwClientIndex		= l_sRecv.dwSerial;
	l_cnmPlayerInfo.dwServerIndex		= l_sRecv.wIndex;
	l_cnmPlayerInfo.dwRecIndex			= l_sRecv.byRecIndex;
	l_cnmPlayerInfo.dwEquipItemVersion	= l_sRecv.wEquipVer;

	CPlayer * l_pPlayer = CreatePlayer( &l_cnmPlayerInfo, FALSE );
	if( !l_pPlayer )
		return;

	//--------------------------------------------------------------------------

	// 장착 아이템 버젼
	//l_pPlayer->SetEquipItemVersion( l_sRecv.wEquipVer );

	// 아이템 세팅
	l_pPlayer->RefreshConditionInfo();
	SetPlayerItem( l_pPlayer, l_sRecv.wModelPerPart, l_sRecv.byFaceIndex );
	l_pPlayer->PutOnEquipment( CEPT_HAIR, l_sRecv.byHairIndex );

	// etc
	l_pPlayer->SetName( l_sRecv.szName );
	l_pPlayer->SetUserGrade( l_sRecv.byUserGrade );

	// 유닛 탑승 여부
	if( l_sRecv.byUnitFrameIndex == 0xFF )
	{
		l_pPlayer->DismountUnit( FALSE );
	}
	else
	{
		CUnitItem clUnitItem;

		clUnitItem.AddFrame( l_sRecv.byUnitFrameIndex );
		for( int i = 0; i < MAX_UNIT_PART_TYPE; ++i )
			clUnitItem.AddParts( i + IEPT_UNIT_HELMET, l_sRecv.byUnitPartIndex[i] );

		l_pPlayer->MountUnit( &clUnitItem, FALSE );
	}

	l_pPlayer->SetAction( l_pPlayer->GetAction(), FALSE );

	l_pPlayer->SetReceiveNetMsg( "Change All Part" );

	/*
	#define other_shape_all_zocl 20
	struct _other_shape_all_zocl	{	//_other_shape_request_clzo의 결과로 첨보는 상대일경우..	

		WORD wIndex;
		DWORD dwSerial;
		WORD wEquipVer;
			
		BYTE byRecIndex;
		BYTE byFaceIndex;	
		BYTE byHairIndex;
		WORD wModelPerPart[equip_fix_num];//상위 4비트.. 업그레이드레벨..  비착용 0xFFFF
	//	BYTE byUnitIndex;	//안타고있으면 0xff	//del(2/13)
		BYTE byUserGrade;	//유저등급 
	//	BYTE byIDSize;	//del(2/13)
		char szName[max_name_len];		//upt(2/13)

		BYTE byUnitFrameIndex;	//안타고있으면 0xFF..
		BYTE byUnitPartIndex[UNIT_PART_NUM];	//byUnitFrameIndex이 0xFF면 무시..

		_other_shape_all_zocl(){
			byUnitFrameIndex = 0xFF;
		}

		int size(){
			if(byUnitFrameIndex != 0xFF) return sizeof(*this);
			return sizeof(*this) - sizeof(byUnitPartIndex);
		}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_OtherShapeAll_zocl : %d", ++g_bWriteTestLogCount, l_sRecv.wIndex ); // 테스트 코드
}

// 플레이어의 외형 정보 요청 성공 - _other_shape_request_clzo ( SHAPE_PART ) 에 대한 응답
void
CNetworkMgr::InitMsg_OtherShapePart_zocl( char * pi_pMsg )
{
	_other_shape_part_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _other_shape_part_zocl ) );

	CPlayer * l_pPlayer = ( CPlayer * )_GetCharMgr()->GetCharacter( CTI_PLAYER, l_sRecv.dwSerial );
	if( !l_pPlayer )
		return;
	l_pPlayer->SetServerIndex( l_sRecv.wIndex );

	//--------------------------------------------------------------------------

	// 장착 아이템 버젼
	l_pPlayer->SetEquipmentVersion( l_sRecv.wEquipVer );

	// 아이템 세팅
	l_pPlayer->RefreshConditionInfo();
	SetPlayerItem( l_pPlayer, l_sRecv.wModelPerPart, -1 );

	// 유닛 탑승 여부
	if( l_sRecv.byUnitFrameIndex == 0xFF )
	{
		l_pPlayer->DismountUnit( FALSE );
	}
	else
	{
		CUnitItem clUnitItem;

		clUnitItem.AddFrame( l_sRecv.byUnitFrameIndex );
		for( int i = 0; i < MAX_UNIT_PART_TYPE; ++i )
			clUnitItem.AddParts( i + IEPT_UNIT_HELMET, l_sRecv.byUnitPartIndex[i] );

		l_pPlayer->MountUnit( &clUnitItem, FALSE );
	}

	l_pPlayer->SetAction( l_pPlayer->GetAction() );

	l_pPlayer->SetReceiveNetMsg( "Change One Part" );

	/*
	#define other_shape_part_zocl 21
	struct _other_shape_part_zocl	{		//_other_shape_request_clzo의 결과로 버젼이 바뀐 상대일경우..//upt(2/13)
		WORD wIndex;
		DWORD dwSerial;
		WORD wEquipVer;
			
		WORD wModelPerPart[equip_fix_num];	//상위 4비트.. 업그레이드레벨..  비착용 0xFFFF

		BYTE byUnitFrameIndex;	//안타고있으면 0xFF..
		BYTE byUnitPartIndex[UNIT_PART_NUM];	//byUnitFrameIndex이 0xFF면 무시..

		_other_shape_part_zocl(){
			byUnitFrameIndex = 0xFF;
		}

		int size(){
			if(byUnitFrameIndex != 0xFF) return sizeof(*this);
			return sizeof(*this) - sizeof(byUnitPartIndex);
		}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_OtherShapeAll_zocl : %d", ++g_bWriteTestLogCount, l_sRecv.wIndex ); // 테스트 코드
}

// 플레이어의 외형 정보 요청 실패 - _other_shape_request_clzo 에 대한 응답
void
CNetworkMgr::InitMsg_OtherShapeFailure_zocl( char * pi_pMsg )
{
	_other_shape_failure_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _other_shape_failure_zocl ) );

#ifdef _DEVELOPER_
	if( l_sRecv.byErrCode == _other_shape_failure_zocl::NOT_LIVE )
		_SetSystemMsg( "셰잎 요청 실패 - 요청한 플레이어의 접속이 끊어졌음( S : %d )", l_sRecv.wOtherIndex );
	else if( l_sRecv.byErrCode == _other_shape_failure_zocl::NOT_SERIAL )
		_SetSystemMsg( "셰잎 요청 실패 - 요청한 플레이어의 시리얼이 잘못되었음.( S : %d )", l_sRecv.wOtherIndex );
	else
		_SetSystemMsg( "셰잎 요청 실패 - 알 수 없는 에러 ( S : %d )",	l_sRecv.wOtherIndex );
#endif

	/*
	#define other_shape_failure_zocl 20	
	struct _other_shape_failure_zocl	{		//_other_shape_request_clzo의 결과로 에러리턴
		WORD wOtherIndex;
	//	DWORD dwOtherSerial;	//클라이언트에게 받은 시리얼	del(6/3)

		enum { NOT_LIVE = 0, NOT_SERIAL };

		BYTE byErrCode;
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_OtherShapeFailure_zocl : %d", ++g_bWriteTestLogCount, l_sRecv.wOtherIndex ); // 테스트 코드
}

// 다른 플레이어의 외형이 변경 되었음
void
CNetworkMgr::InitMsg_OtherShapeChange_zocl( char * pi_pMsg )
{
	_other_shape_change_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _other_shape_change_zocl ) );

	CPlayer * l_pPlayer = ( CPlayer * )_GetCharMgr()->GetCharacter( CTI_PLAYER, l_sRecv.dwSerial );
	if( !l_pPlayer )
		return;
	l_pPlayer->SetServerIndex( l_sRecv.wIndex );

	//--------------------------------------------------------------------------

	if( ( l_pPlayer->GetEquipmentVersion() + 1 ) != l_sRecv.wEquipVer )
		return;

	l_pPlayer->SetEquipmentVersion( l_sRecv.wEquipVer );

	if( l_sRecv.wItemIndex != 0xFFFF )
	{
		BYTE			byLevel			= l_sRecv.wItemIndex >> 12;
		WORD			wItemDTIndex	= l_sRecv.wItemIndex & 0x0FFF;
		CNormalItem *	pEquipItem		= NULL;

		l_pPlayer->PutOffEquipment( IEPT_SERVER_TO_CLIENT[l_sRecv.byPartIndex] );
		l_pPlayer->PutOnEquipment( IEPT_SERVER_TO_CLIENT[l_sRecv.byPartIndex], wItemDTIndex );
		pEquipItem = l_pPlayer->GetNormalItem( IEPT_SERVER_TO_CLIENT[l_sRecv.byPartIndex] );
		if( pEquipItem )
			pEquipItem->SetLevel( byLevel );
	}
	else
	{
		l_pPlayer->PutOffEquipment( IEPT_SERVER_TO_CLIENT[l_sRecv.byPartIndex] );
	}

	l_pPlayer->SetReceiveNetMsg( "Put On Equipment" );

	/*
	#define other_shape_change_zocl 21//upt(4/8)
	struct _other_shape_change_zocl	{	//주위 플레이어가 장착을 바꾼경우
		WORD wIndex;
		DWORD dwSerial;
		WORD wEquipVer;

		BYTE byPartIndex;
		WORD wItemIndex;	//상위 4비트.. 업그레이드레벨..  
	};*/

	//if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_OtherShapeChange_zocl", ++g_bWriteTestLogCount ); // 테스트 코드
}

// 다른 플레이어가 유닛을 타고 내릴때
void
CNetworkMgr::InitMsg_OtherUnitRideChange_zocl( char * pi_pMsg )
{
	_other_unit_ride_change_zocl * pRecvMsg = reinterpret_cast< _other_unit_ride_change_zocl * >( pi_pMsg );

	CPlayer * pPlayer	= _GetCharMgr()->GetPlayer( pRecvMsg->dwSerial );
	CPlayer * pUnit		= _GetCharMgr()->GetUnit( pRecvMsg->dwUnitSerial );

	if( pRecvMsg->bTake )
	{
		if( !pPlayer || !pUnit )
			return;

		CUnitItem * pUnitItem = pUnit->GetUnitItem();
		if( !pUnitItem )
			return;

		if( !pPlayer->MountUnit( pUnitItem, TRUE ) )
			return;

		pPlayer->SetAction( CAI_GESTURE_MOUNT );
	}
	else
	{
		if( !pPlayer )
			return;

		pPlayer->DismountUnit( TRUE );
		pPlayer->SetAction( CAI_MOVE_STOP );
	}

	pPlayer->SetEquipmentVersion( pRecvMsg->wEquipVer );
	pPlayer->SetPosition( pRecvMsg->zNewPos[0], pRecvMsg->zNewPos[1], pRecvMsg->zNewPos[2] );

	/*
	#define other_unit_ride_change_zocl 233	//add(2/13)
	struct _other_unit_ride_change_zocl	{	//주위 플레이어가 유닛 승하차를 바꾼경우
		WORD	wIndex;
		DWORD	dwSerial;
		WORD	wEquipVer;

		bool	bTake;				//승차는 true, 하차는 false
		DWORD	dwUnitSerial;		//모르는 시리얼의 경우.. 바깥 섹터에 있는 유닛.. 플레이어도 섹터밖으로 빼버리고.. 나중에 만나면 _other_shape_part_zocl로 새로받는다..
		short	zNewPos[3];			//승하차 뽀지숀..
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 플레이어가 처음으로 생성 되었음
void
CNetworkMgr::InitMsg_OtherNewView_zocl( char * pi_pMsg )
{
	_other_new_view_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _other_new_view_zocl ) );

	cnm_create_player l_cnmPlayerInfo;

	l_cnmPlayerInfo.dwClientIndex		= l_sRecv.dwSerial;
	l_cnmPlayerInfo.dwServerIndex		= l_sRecv.wIndex;
	l_cnmPlayerInfo.dwRecIndex			= l_sRecv.byRaceCode;
	l_cnmPlayerInfo.dwEquipItemVersion	= l_sRecv.wEquipVer;

	CPlayer * l_pPlayer = CreatePlayer( &l_cnmPlayerInfo );
	if( !l_pPlayer )
		return;

	//--------------------------------------------------------------------------

	// 플레이어의 상태 설정
	l_pPlayer->SetPosition( l_sRecv.zPos[0], l_sRecv.zPos[1], l_sRecv.zPos[2] );
	l_pPlayer->CheckYPos();
	l_pPlayer->CreatePath( l_pPlayer->GetPosition() );
	l_pPlayer->SetRotY( 0, FALSE );

	l_pPlayer->SetAction( CAI_MOVE_STOP );	// SetServerStatusFlag에서 시체 처리가 되므로 보다 먼저 호출되어야 한다.
	l_pPlayer->SetServerStatusFlag( l_sRecv.dwStateFlag );
	l_pPlayer->SetServerSFEffectCode( l_sRecv.wLastEffectCode, 1, TRUE );

	// 효과 처리를 다르게 하고 싶다면 l_sRecv.byViewType 을 사용한다.
	if( l_sRecv.byViewType == mapin_type_start )
		l_pPlayer->SetEffectInfo( CAI_CREATE_CREATE );

	/*
	//자신이 새로운곳이 뿅하고 나타날때.. 주위의 플레이어에게 뿌린다.
	#define other_new_view_zocl 24
	struct _other_new_view_zocl{	//upt(2/13)
		WORD wIndex;
		DWORD dwSerial;
		WORD wEquipVer;
		short zPos[3];
		BYTE byRaceCode;

		//byViewType----
		//#define mapin_type_start		0	//게임시작		
		//#define mapin_type_move		1	//포탈이동..
		//#define mapin_type_revival	2	//부활
		//#define mapin_type_goto		3	//순간이동
		//#define mapin_type_recall     4	//출동..

		BYTE byViewType;
		DWORD dwStateFlag;
		//BYTE byUnitIndex;
		WORD wLastEffectCode;	//upt(4/12)
	};*/

	//if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_OtherNewView_zocl", ++g_bWriteTestLogCount ); // 테스트 코드
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타가 부활을 서버에 요청함
BOOL
CNetworkMgr::InitMsg_PlayerRevivalRequest_clzo( void )
{
	// 메세지를 보냄
	BYTE l_nType[] = { init_msg, player_revival_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 NULL,
									 0 );

	// 결과 처리
	if( l_nResult )
		_GetAvatar()->SetShowRebirthMessageBox( FALSE );
	else
		NetLogWrite( "CNetworkMgr::InitMsg_PlayerRevivalRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	//if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_PlayerRevivalRequest_clzo", ++g_bWriteTestLogCount ); // 테스트 코드

	/*
	#define player_revival_request_clzo 26
	struct _player_revival_request_clzo{
	};
	*/

	return l_nResult;
}

// 아바타가 부활했음
void
CNetworkMgr::InitMsg_PlayerRevival_zocl( char * pi_pMsg )
{
	_player_revival_zocl * pRecvMsg = reinterpret_cast< _player_revival_zocl * >( pi_pMsg );

	if( pRecvMsg->byRet == 0 )
	{
		_GetAvatar()->SetLevel( pRecvMsg->byLevel );
		_GetAvatar()->SetHP( pRecvMsg->wCurHP );
		_GetAvatar()->SetFP( pRecvMsg->wCurFP );
		_GetAvatar()->SetSP( pRecvMsg->wCurST );

		_GetAvatar()->SetRequestChangeToCombatMode( FALSE );
		_GetAvatar()->StopTargetPursuit();

		if( _GetDungeonInfo()->IsInDungeon() )
		{
			_GetDungeonInfo()->SetDungeonExitType( DET_DEATH );
			_GetDungeonInfo()->ExitDungeon();
		}

		// 같은 존 내에서의 이동
		MOVEMENT_BETWEEN_MAP_INFO * pMoveMapInfo = _GetAvatar()->GetMoveMapInfo();
		if( pMoveMapInfo )
		{
			pMoveMapInfo->m_byEnterMapIndex	= _GetLand()->GetMapIndex();
			pMoveMapInfo->m_byExitMapIndex	= pRecvMsg->wStartMapIndex;

			pMoveMapInfo->m_vecPosition[0]	= pRecvMsg->zPos[0];
			pMoveMapInfo->m_vecPosition[1]	= pRecvMsg->zPos[1];
			pMoveMapInfo->m_vecPosition[2]	= pRecvMsg->zPos[2];

			_GetAvatar()->GetMoveMapInfo()->MoveToPortal_Result( GST_REBIRTH );
		}
	}
	else
	{
		_SetSystemMsg( "부활 관련 에러 ( %d )", pRecvMsg->byRet );
	}

	//----------------------------------------------------------------------

	//if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) InitMsg_PlayerRevival_zocl : Pos - %d, %d, %d",
	//	++g_bWriteTestLogCount, pRecvMsg->zPos[0], pRecvMsg->zPos[1], pRecvMsg->zPos[2] ); // 테스트 코드
	m_ClientLogFile.Write( "( %d ) InitMsg_PlayerRevival_zocl : Pos - %d, %d, %d",
		++g_bWriteTestLogCount, pRecvMsg->zPos[0], pRecvMsg->zPos[1], pRecvMsg->zPos[2] ); // 테스트 코드

	/*
	#define player_revival_zocl 27
	struct _player_revival_zocl{
		BYTE byRet;
		WORD wStartMapIndex;
		short zPos[3];
		BYTE byLevel;
		WORD wCurHP;
		WORD wCurFP;
		WORD wCurST;
		//double dExp; //del(6/17)
		BYTE byZoneCode;	//0이면 같은 존.. 1이면 다른존.. => move_out을 보낸다.//add(4/6)
	};*/
}

// 플레이어가 부활이나 포탈등으로 다른 장소로 이동했음
void
CNetworkMgr::InitMsg_OtherMapOut_zocl( char * pi_pMsg )
{
	_other_map_out_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _other_map_out_zocl ) );

	CPlayer * l_pPlayer = _GetCharMgr()->GetPlayer( l_sRecv.dwSerial );
	if( !l_pPlayer )
		return;

	if( _GetPartyBoard()->IsPartyMember( l_sRecv.dwSerial ) )
	{
		if( _GetLand()->GetMapIndex() == l_sRecv.byNextMap )
			l_pPlayer->Enable( ROSF_OUT_OF_VISUAL_FIELD_SERVER | ROSF_OUT_OF_VISUAL_FIELD_CLIENT );
		else
			_GetCharMgr()->SaveToCache( l_pPlayer );
	}
	else
	{
		_GetCharMgr()->SaveToCache( l_pPlayer );
	}

	/*
	l_pPlayer->Enable( ROSF_OUT_OF_VISUAL_FIELD_SERVER | ROSF_OUT_OF_VISUAL_FIELD_CLIENT );

	// 같은 맵이 아닐 경우
	if( _GetLand()->GetMapIndex() != l_sRecv.byNextMap )
	{
		// 자신의 파티원이 아닐 경우에만 삭제한다.
		if( !_GetPartyBoard()->IsPartyMember( l_sRecv.dwSerial ) )
		{
			_GetCharMgr()->SaveToCache( l_pPlayer );
			return;
		}
	}*/

	/*
	#define other_map_out_zocl 28
	struct _other_map_out_zocl{				// add(3/29)
		//#define mapin_type_move		1	//포탈이동..
		//#define mapin_type_revival	2	//부활
		BYTE byMapOutType;
		DWORD dwSerial;
	};*/
}

// 플레이어의 스킬/포스에 의한 부활
void
CNetworkMgr::InitMsg_PlayerResurrect_zocl( char * pi_pMsg )
{
	_player_resurrect_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _player_resurrect_zocl ) );

	if( l_sRecv.byRet == 0 )
	{
		_GetAvatar()->SetLevel( l_sRecv.byLevel );
		_GetAvatar()->SetHP( l_sRecv.wCurHP );
		_GetAvatar()->SetFP( l_sRecv.wCurFP );
		_GetAvatar()->SetSP( l_sRecv.wCurST );

		_GetAvatar()->SetRequestChangeToCombatMode( FALSE );
		_GetAvatar()->StopTargetPursuit();

		SettingPlayerRebirth( _GetAvatar() );

		InitMsg_NewPosStartRequest_clzo( GST_REBIRTH );

		_SetSystemMsg( "부활 성공" );
	}
	else
	{
		_SetSystemMsg( "부활 실패" );
	}

	/*
	#define player_resurrect_zocl 28
	struct _player_resurrect_zocl{	//add(3/24)
		BYTE byRet;
		BYTE byLevel;
		WORD wCurHP;
		WORD wCurFP;
		WORD wCurST;
		//double dExp; //del(6/17)
	};*/

	m_ClientLogFile.Write( "( %d ) InitMsg_PlayerResurrect_zocl", ++g_bWriteTestLogCount ); // 테스트 코드
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CPlayer *
CNetworkMgr::CreatePlayer( cnm_create_player * pi_pRecv, BOOL pi_bRequestExterior )
{
	CPlayer * l_pPlayer = _GetCharMgr()->AddPlayer( pi_pRecv->dwClientIndex );
	if( !l_pPlayer )
		return NULL;

	if( l_pPlayer->GetIndex() == pi_pRecv->dwClientIndex )
	{
		// 플레이어 설정
		l_pPlayer->SetIndex( pi_pRecv->dwClientIndex );
		if( pi_pRecv->dwServerIndex != ID_INVALID )
			l_pPlayer->SetServerIndex( pi_pRecv->dwServerIndex );
		l_pPlayer->SetDTIndex( pi_pRecv->dwRecIndex );

		// 종족이 틀리다면 설정
		if( l_pPlayer->GetRace() != pi_pRecv->dwRecIndex )
		{
			l_pPlayer->SetRace( pi_pRecv->dwRecIndex );
			l_pPlayer->SetBone( pi_pRecv->dwRecIndex );

			DT_CPInfo * l_pData = ( DT_CPInfo * )_GetCharDataMgr()->GetCharacter( CTI_PLAYER,
																		  l_pPlayer->GetDTIndex() );
			if( !l_pData )
				return FALSE;

			l_pPlayer->SetData( l_pData );

			//l_pPlayer->SetSpeed( CMM_MOVE_WALK, CHAR_SPEED_DEFAULT * ( l_pData->m_fMoveWalkRate + 0.1f ) ); // 서버와의 싱크를 위해서 0.1 정도 빠르게 해준다.
			//l_pPlayer->SetSpeed( CMM_MOVE_RUN, CHAR_SPEED_DEFAULT * ( l_pData->m_fMoveRunRate + 0.1f ) );
			l_pPlayer->SetSpeed( CMM_MOVE_WALK, CHAR_SPEED_DEFAULT * l_pData->m_fMoveWalkRate );
			l_pPlayer->SetSpeed( CMM_MOVE_RUN, CHAR_SPEED_DEFAULT * l_pData->m_fMoveRunRate );

			// 외형 요청
			InitMsg_OtherShapeRequest_clzo( l_pPlayer, TRUE );
		}
		// 플레이어의 외형 요청
		else if( pi_bRequestExterior )
		{
			// 만약 서버or클라이언트의 인덱스가 틀리다면 외형을 전부 요청한다.
			if( ( l_pPlayer->GetIndex() != pi_pRecv->dwClientIndex ) ||
				( l_pPlayer->GetServerIndex() != pi_pRecv->dwServerIndex ) )
				InitMsg_OtherShapeRequest_clzo( l_pPlayer, TRUE );
			// 만약 장착 버전이 틀리다면 외형을 일부만 요청한다.
			else if( l_pPlayer->GetEquipmentVersion() != pi_pRecv->dwEquipItemVersion )
				InitMsg_OtherShapeRequest_clzo( l_pPlayer, FALSE );
		}

		l_pPlayer->SetEquipmentVersion( pi_pRecv->dwEquipItemVersion );

		// 기타 설정
		if( l_pPlayer->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) )
		{
			l_pPlayer->Enable( ROSF_ANIMATION_LOOP );
			l_pPlayer->Disable( ROSF_RENDER_ALPHA );
			l_pPlayer->SetAlphaDensity( 1.0f );
		}

		l_pPlayer->Disable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );
		l_pPlayer->SetReceiveFixPositionMsg( TRUE );
		_GetCharMgr()->CheckOutPositionOfVisualField_Character( l_pPlayer, FALSE );

		if( l_pPlayer->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
			l_pPlayer->UI_SetRadar();
		l_pPlayer->UI_SetMinimap();

		l_pPlayer->SetReceiveNetMsg( "Create" );

		return l_pPlayer;
	}

	//--------------------------------------------------------------------------
	// ID 세팅
	l_pPlayer->SetIndex( pi_pRecv->dwClientIndex );
	if( pi_pRecv->dwServerIndex != ID_INVALID )
		l_pPlayer->SetServerIndex( pi_pRecv->dwServerIndex );
	l_pPlayer->SetObjectTypeID( OTI_CHAR );
	l_pPlayer->SetCharTypeID( CTI_PLAYER );
	l_pPlayer->SetDTIndex( pi_pRecv->dwRecIndex );
	l_pPlayer->SetEquipmentVersion( pi_pRecv->dwEquipItemVersion );

	l_pPlayer->SetBone( pi_pRecv->dwRecIndex );
	l_pPlayer->SetRace( pi_pRecv->dwRecIndex );

	l_pPlayer->SetAction( CAI_MOVE_STOP );

	//--------------------------------------------------------------------------
	// 기타
	l_pPlayer->LoadAnimaitonData();

	DT_CPInfo * l_pData = ( DT_CPInfo * )_GetCharDataMgr()->GetCharacter( CTI_PLAYER,
																		  l_pPlayer->GetDTIndex() );
	if( !l_pData )
		return FALSE;

	l_pPlayer->SetData( l_pData );

	//l_pPlayer->SetSpeed( CMM_MOVE_WALK, CHAR_SPEED_DEFAULT * ( l_pData->m_fMoveWalkRate + 0.1f ) ); // 서버와의 싱크를 위해서 0.1 정도 빠르게 해준다.
	//l_pPlayer->SetSpeed( CMM_MOVE_RUN, CHAR_SPEED_DEFAULT * ( l_pData->m_fMoveRunRate + 0.1f ) );
	l_pPlayer->SetSpeed( CMM_MOVE_WALK, CHAR_SPEED_DEFAULT * l_pData->m_fMoveWalkRate );
	l_pPlayer->SetSpeed( CMM_MOVE_RUN, CHAR_SPEED_DEFAULT * l_pData->m_fMoveRunRate  );

	// 플레이어의 외형을 전부 요청한다.
	if( pi_bRequestExterior )
		InitMsg_OtherShapeRequest_clzo( l_pPlayer, TRUE );

	//--------------------------------------------------------------------------

	l_pPlayer->SetReceiveFixPositionMsg( TRUE );

	_GetCharMgr()->CheckOutPositionOfVisualField_Character( l_pPlayer, FALSE );

	if( l_pPlayer->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
		l_pPlayer->UI_SetRadar();
	l_pPlayer->UI_SetMinimap();

	l_pPlayer->SetReceiveNetMsg( "Create" );

	return l_pPlayer;
}

CMonster *
CNetworkMgr::CreateMonster( cnm_create_monster * pi_pRecv )
{
	CMonster * l_pMonster = _GetCharMgr()->AddMonster( pi_pRecv->dwClientIndex );
	if( !l_pMonster )
		return NULL;

	if( l_pMonster->GetIndex() == pi_pRecv->dwClientIndex )
	{
		if( l_pMonster->IsDeath() )
			return NULL;

		if( l_pMonster->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) )
		{
			l_pMonster->Enable( ROSF_ANIMATION_LOOP );
			l_pMonster->Disable( ROSF_RENDER_ALPHA );
			l_pMonster->SetAlphaDensity( 1.0f );
		}

		l_pMonster->Disable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );

		l_pMonster->SetIndex( pi_pRecv->dwClientIndex );
		l_pMonster->SetServerIndex( pi_pRecv->dwServerIndex );
		l_pMonster->SetDTIndex( pi_pRecv->dwRecIndex );

		CMonsterData * l_pData= ( CMonsterData * )_GetCharDataMgr()->GetCharacter( CTI_MONSTER, pi_pRecv->dwRecIndex );
		if( !l_pData )
		{
			_GetCharMgr()->DeleteCharacter( CTI_MONSTER, pi_pRecv->dwClientIndex );
			return NULL;
		}

		l_pMonster->SetAttribute( l_pData );

		l_pMonster->SetReceiveFixPositionMsg( TRUE );
		_GetCharMgr()->CheckOutPositionOfVisualField_Character( l_pMonster, FALSE );

		if( l_pMonster->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
			l_pMonster->UI_SetRadar();

		l_pMonster->SetReceiveNetMsg( "Create" );

		return l_pMonster;
	}

	//--------------------------------------------------------------------------
	// ID 세팅
	l_pMonster->SetIndex( pi_pRecv->dwClientIndex );
	l_pMonster->SetServerIndex( pi_pRecv->dwServerIndex );
	l_pMonster->SetObjectTypeID( OTI_CHAR );
	l_pMonster->SetCharTypeID( CTI_MONSTER );
	l_pMonster->SetDTIndex( pi_pRecv->dwRecIndex );	// 테이블 레코드 인덱스

	//--------------------------------------------------------------------------
	// 몬스터의 본과 메시 설정
	CMonsterData * l_pData = ( CMonsterData * )_GetCharDataMgr()->GetCharacter( CTI_MONSTER, pi_pRecv->dwRecIndex );
	if( !l_pData )
	{
		_GetCharMgr()->DeleteCharacter( CTI_MONSTER, pi_pRecv->dwClientIndex );
		return NULL;
	}

	l_pMonster->SetAttribute( l_pData );

	l_pMonster->SetAction( CAI_MOVE_STOP );

	l_pMonster->SetReceiveFixPositionMsg( TRUE );

	_GetCharMgr()->CheckOutPositionOfVisualField_Character( l_pMonster, FALSE );

	if( l_pMonster->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
		l_pMonster->UI_SetRadar();

	l_pMonster->SetReceiveNetMsg( "Create" );

	return l_pMonster;
}

CAnimus *
CNetworkMgr::CreateAnimus( cnm_create_animus * pi_pRecv )
{
	CAnimus * pAnimus = _GetCharMgr()->AddAnimus( pi_pRecv->dwClientIndex );
	if( !pAnimus )
		return NULL;

	if( pAnimus->IsEqual( CTI_ANIMUS, pi_pRecv->dwClientIndex ) )
	{
		if( pAnimus->IsDeath() )
			return NULL;

		// set view
		if( pAnimus->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) )
		{
			pAnimus->Enable( ROSF_ANIMATION_LOOP );
			pAnimus->Disable( ROSF_RENDER_ALPHA );
			pAnimus->SetAlphaDensity( 1.0f );
		}
		pAnimus->Disable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );

		// set etc
		pAnimus->SetReceiveFixPositionMsg( TRUE );
		_GetCharMgr()->CheckOutPositionOfVisualField_Character( pAnimus, FALSE );

		if( pAnimus->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
			pAnimus->UI_SetRadar();

		pAnimus->SetReceiveNetMsg( "Create" );
	}
	else
	{
		// set basic property
		pAnimus->SetIndex( pi_pRecv->dwClientIndex );
		pAnimus->SetServerIndex( pi_pRecv->dwServerIndex );
		pAnimus->SetDTIndex( pi_pRecv->dwRecIndex );

		// set data
		CAnimusData * pAnimusData	= _GetCharDataMgr()->GetAnimus( pi_pRecv->dwRecIndex, pi_pRecv->byLevel );
		ANIMUS_DATA * pItemData		= static_cast< ANIMUS_DATA * >( _GetItemDataMgr()->GetData( IEPT_ANIMUS, pi_pRecv->dwRecIndex ) );
		if( !pAnimusData || !pItemData )
		{
			_GetCharMgr()->DeleteCharacter( CTI_ANIMUS, pi_pRecv->dwClientIndex );
			return NULL;
		}
		pAnimus->SetItemData( pItemData );
		pAnimus->SetAttribute( pAnimusData );

		// set etc
		pAnimus->SetAction( CAI_GESTURE_SUMMON );
		pAnimus->SetReceiveFixPositionMsg( TRUE );
		_GetCharMgr()->CheckOutPositionOfVisualField_Character( pAnimus, FALSE );

		if( pAnimus->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
			pAnimus->UI_SetRadar();

		pAnimus->SetReceiveNetMsg( "Create" );
	}

	return pAnimus;
}

CGuardTower *
CNetworkMgr::CreateGuardTower( cnm_create_guardtower * pi_pRecv )
{
	CGuardTower * pGuardTower = _GetCharMgr()->AddGuardTower( pi_pRecv->dwClientIndex );
	if( !pGuardTower )
		return NULL;

	if( pGuardTower->IsEqual( CTI_GUARD_TOWER, pi_pRecv->dwClientIndex ) )
	{
		if( pGuardTower->IsDeath() )
			return NULL;

		// set view
		if( pGuardTower->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) )
		{
			pGuardTower->Enable( ROSF_ANIMATION_LOOP );
			pGuardTower->Disable( ROSF_RENDER_ALPHA );
			pGuardTower->SetAlphaDensity( 1.0f );
		}
		pGuardTower->Disable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );

		// set etc
		pGuardTower->SetReceiveFixPositionMsg( TRUE );
		_GetCharMgr()->CheckOutPositionOfVisualField_Character( pGuardTower, FALSE );

		if( pGuardTower->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
			pGuardTower->UI_SetRadar();

		pGuardTower->SetReceiveNetMsg( "Create" );
	}
	else
	{
		// set basic property
		pGuardTower->SetIndex( pi_pRecv->dwClientIndex );
		pGuardTower->SetServerIndex( pi_pRecv->dwServerIndex );
		pGuardTower->SetDTIndex( pi_pRecv->dwRecIndex );

		// set data
		GUARD_TOWER_DATA * pItemData	= static_cast< GUARD_TOWER_DATA * >( _GetItemDataMgr()->GetData( IEPT_GUARD_TOWER, pi_pRecv->dwRecIndex ) );
		if( !pItemData )
		{
			_GetCharMgr()->DeleteCharacter( IEPT_GUARD_TOWER, pi_pRecv->dwClientIndex );
			return NULL;
		}
		pGuardTower->SetAttribute( pItemData );

		// set etc
		pGuardTower->SetReceiveFixPositionMsg( TRUE );
		_GetCharMgr()->CheckOutPositionOfVisualField_Character( pGuardTower, FALSE );

		if( pGuardTower->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
			pGuardTower->UI_SetRadar();

		pGuardTower->SetReceiveNetMsg( "Create" );
	}

	return pGuardTower;
}

CGroundItem *
CNetworkMgr::CreateGroundItem( cnm_create_item * pi_pRecv )
{
	CGroundItem * pGroundItem = _GetGroundItemMgr()->GetItem( pi_pRecv->wIndex );

	if( !pGroundItem )
	{
		pGroundItem = _GetGroundItemMgr()->AddItem();
		if( !pGroundItem )
			return NULL;
	}

	if( !pGroundItem->Create( IEPT_SERVER_TO_CLIENT[pi_pRecv->byEquipPart], pi_pRecv->wRecIndex ) )
		return FALSE;

	//--------------------------------------------------------------------------

	pGroundItem->SetIndex( pi_pRecv->wIndex );

	pGroundItem->SetPosition( pi_pRecv->vecPos );
	pGroundItem->CheckYPos();

	pGroundItem->SetAnimation( IAI_DEFAULT );

	//--------------------------------------------------------------------------

	pGroundItem->SetCreateTime( timeGetTime() );
	pGroundItem->SetReceiveFixPositionMsg( TRUE );

	pGroundItem->SetLightColor( D3DCOLOR_XRGB( 128, 128, 128 ) );
	pGroundItem->Enable( ROSF_ANIMATION_LOOP );
	pGroundItem->Disable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );

	return pGroundItem;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNetworkMgr::SetPlayerItem( CPlayer * pi_pPlayer, WORD * pi_pItemIndex, WORD pi_wFaceIndex )
{
	if( !pi_pItemIndex || !pi_pPlayer )
		return;

	BYTE			byLevel = 0;
	WORD			dwItemDTIndex = 0;
	CNormalItem *	pEquipItem = NULL;

	// 플레이어 아이템 설정
	for( int i = 0; i < equip_fix_num; ++i )
	{
		if( pi_pItemIndex[i] != 0xFFFF )
		{
			byLevel			= pi_pItemIndex[i] >> 12;
			dwItemDTIndex	= pi_pItemIndex[i] & 0x0FFF;

			pi_pPlayer->PutOnEquipment( IEPT_SERVER_TO_CLIENT[i], dwItemDTIndex );
			pEquipItem = pi_pPlayer->GetNormalItem( IEPT_SERVER_TO_CLIENT[i] );
			if( pEquipItem )
				pEquipItem->SetLevel( byLevel );
		}
		else
		{
			pi_pPlayer->PutOffEquipment( IEPT_SERVER_TO_CLIENT[i] );
		}
	}

	if( pi_wFaceIndex != 0xFFFF )
		pi_pPlayer->PutOnEquipment( CEPT_FACE, pi_wFaceIndex );
}