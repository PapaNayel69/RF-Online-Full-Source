////////////////////////////////////////////////////////////////////////////////
//
// CAvatar Relation Infomation Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../GameObject/Manager/CTargetMgr.h"
#include "../../GameObject/Board/Common/CBoardManager.h"

#include "../../CLand.h"
#include "../../CCamera.h"
#include "../../GameProgress/CGP_MainGame.h"

#include "../../Network/NetMsgProcessor/Map/CNetMsgProcessor_Map.h"
#include "../../Network/NetMsgProcessor/Dungeon/CNetMsgProcessor_Dungeon.h"
#include "../../Network/NetMsgProcessor/ItemTrade/CNetMsgProcessor_ItemTrade.h"

#include "../../UserInterface/GameWindow/Recall/GUIRecallWindow.h"
#include "../../UserInterface/GameWindow/Recall/GUIMobControl.h"
#include "../../UserInterface/GameWindow/Mining/GUIMiningWindow.h"
#include "../../UserInterface/GameWindow/EtcWin/GUIMapLoadingWindow.h"
#include "../../UserInterface/GameWindow/EtcWin/GUIMapMoveWindow.h"
#include "../../UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"
#include "../../UserInterface/GameWindow/EtcWin/GUIMiniMapWindow.h"

#include "../../UserInterface/GameWindow/Player/GUITradeWindow.h"

#include "../../GameObject/Board/General/CPartyBoard.h"

#include "../../AddCodeByJang/EffectUtil.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 포탈로 이동
void
EP_MoveToPortal( BYTE pi_byPortalIndex )
{
	_GetAvatar()->GetMoveMapInfo()->MoveToPortal_Request( pi_byPortalIndex );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define DEFAULT_MOVEMAP_DELAY		( 500 )
#define DUNGEON_PORTAL_OPEN_DELAY	( 2500 )
#define DUNGEON_DESC_OPEN_DELAY		( 6000 )

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
MOVEMENT_BETWEEN_MAP_INFO::Init( void )
{
	ZeroMemory( this, sizeof( MOVEMENT_BETWEEN_MAP_INFO ) );
	m_dwMoveDelay	= 1000;
}

void
MOVEMENT_BETWEEN_MAP_INFO::FrameMove( void )
{
	switch( m_byMoveStep )
	{
		case MMS_PREV_LOAD_DATA		:
			MoveToPortal_Result_PrevLoadData();
			break;

		case MMS_LOAD_DATA			:
			MoveToPortal_Result_LoadData();
			break;

		case MMS_AFTER_LOAD_DATA	:
			MoveToPortal_Result_AfterLoadData();
			break;

		case MMS_BEGIN_NEW_MAP		:
			MoveToPortal_Result_BeginNewMap();
			break;
	}
}

//==============================================================================

// 맵 이동 요청
BOOL
MOVEMENT_BETWEEN_MAP_INFO::MoveToPortal_Request( BYTE pi_byPortalIndex )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;

	// 체크
	if( _GetAvatar()->IsDeath() )
	{
		_SetSystemMsg( "아바타가 죽은 상태에서는 맵 이동을 할 수 없습니다." );
		return FALSE;
	}
	if( !_GetAvatar()->IsEnableMove() )
	{
		_SetSystemMsg( "아바타가 움직일 수 없는 상태에서는 맵 이동을 할 수 없습니다." );
		return FALSE;
	}

	if( _GetAvatar()->IsEqualAction( CAI_ITEM_USE_MINING ) )
	{
		_SetSystemMsg( "채굴중에는 맵 이동을 할 수 없습니다." );
		return FALSE;
	}

	if( m_byMoveStep != MMS_NONE )
		return FALSE;

	// clear
	_GetNextActionMgr()->SetEmpty( TRUE );

	// 맵 이동
	PORTAL_INFO * l_pPortalInfo = _GetLand()->GetPortalInfo( _GetLand()->GetMapIndex(), pi_byPortalIndex );
	if( !l_pPortalInfo )
		return FALSE;

	//--------------------------------------------------------------------------

	CNetMsgProcessor_Map * l_pNetMsgProcessor = static_cast< CNetMsgProcessor_Map * >
												( _GetNetworkMgr()->GetNetMsgProcessor( NMP_MAP ) );
	if( !l_pNetMsgProcessor )
		return FALSE;

	l_pNetMsgProcessor->MovePotalRequest_clzo( pi_byPortalIndex );

	//--------------------------------------------------------------------------

	m_pEnterPortalInfo	= l_pPortalInfo;
	m_pExitPortalInfo	= _GetLand()->GetPortalInfo( l_pPortalInfo->byLinkedPortal[0][0],
													 l_pPortalInfo->byLinkedPortal[0][1] ); // 현재는 링크된 포탈이 무조건 1개이므로 무조건 0 번으로 세팅시켜 준다.

	m_byEnterMapIndex	= m_pEnterPortalInfo->byMapIndex;
	m_byExitMapIndex	= m_pExitPortalInfo->byMapIndex;

	_GetGBM()->CloseUnnecessaryWindow();

	return TRUE;
}

//==============================================================================

/*
BOOL
MOVEMENT_BETWEEN_MAP_INFO::MoveToPortal_Result_CurrentMap( void )
{
	_GetAvatar()->SetPosition( m_vecPosition );
	_GetAvatar()->CheckYPos();

	_GetLand()->SetMapIndex( m_byExitMapIndex );
	_GetLand()->SetPortalIndex( m_pExitPortalInfo->byIndex );

	_GetPartyBoard()->ResetPartyMemberAfterMoveMap();

	m_byMoveStep		= MMS_NONE;

	m_bIsMoved			= FALSE;
	m_pEnterPortalInfo	= NULL;
	m_pExitPortalInfo	= NULL;

	_GetNetworkMgr()->InitMsg_NewPosStartRequest_clzo( GST_PORTAL );

	return TRUE;
}*/

//==============================================================================

void
MOVEMENT_BETWEEN_MAP_INFO::MoveToPortal_Result( BOOL pi_byMoveType )
{
	m_byMoveType			= pi_byMoveType;
	m_byMoveStep			= MMS_PREV_LOAD_DATA;
	m_dwMoveDelay			= DEFAULT_MOVEMAP_DELAY;
	m_dwMoveStepStartTime	= timeGetTime();

	_GetMapLoadingWindow()->SetMapMove( m_byEnterMapIndex, m_byExitMapIndex );
	_GetAvatar()->Disable( ROSF_MOVABLE );

	m_dwMoveDelay			= _GetDungeonInfo()->OpenPortal( m_byMoveType );

	SetEffectInfo();
}

void
MOVEMENT_BETWEEN_MAP_INFO::MoveToPortal_Result_PrevLoadData( void )
{
	if( timeGetTime() - m_dwMoveStepStartTime >= m_dwMoveDelay )
	{
		m_byMoveStep			= MMS_LOAD_DATA;
		m_dwMoveStepStartTime	= timeGetTime();

		_GetDungeonInfo()->ClosePortalOrExitDungeon( m_byMoveType );
		_GetMapLoadingWindow()->Open();
	}
}

void
MOVEMENT_BETWEEN_MAP_INFO::MoveToPortal_Result_LoadData( void )
{
	// 같은 맵 내에서의 이동이 아니라면 데이터 재설정
	if( m_byEnterMapIndex != m_byExitMapIndex )
	{
		// 모든 캐릭터 데이터를 삭제한다.
		_GetRadarWindow()->RemoveAllCharacter();
		_GetMinimapWindow()->RemoveAllMiniMapObject();
		_GetMinimapWindow()->RemoveAllTooltipObject();
		_GetCharMgr()->ClearCharacter( CTI_PLAYER );
		_GetCharMgr()->ClearCharacter( CTI_NPC );
		_GetCharMgr()->ClearCharacter( CTI_MONSTER );

		// 맵 로딩
		_GetLand()->UnloadLevel();
		_GetLand()->LoadLevel( m_byExitMapIndex );
		_GetCamera()->SetSpecialCameraMode(0);	//특수 카메라는 0으로 셋팅.
		SetStateFullScreenEffect(FALSE);

		// 상인 리스트를 요청한다.
		CNetMsgProcessor_ItemTrade * pNMP = static_cast< CNetMsgProcessor_ItemTrade * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_ITEM_TRADE ) );
		pNMP->StoreListRequest_clzo();
	}

	// set avatar
	if( _GetAvatar()->IsDeath() && ( m_byMoveType == GST_REBIRTH ) )
		_GetNetworkMgr()->SettingPlayerRebirth( _GetAvatar() );
	_GetAvatar()->SetPosition( m_vecPosition );
	_GetAvatar()->CheckYPos();

	// set map
	_GetLand()->SetMapIndex( m_byExitMapIndex );
	if( m_byMoveType == GST_PORTAL )
		_GetLand()->SetPortalIndex( m_pExitPortalInfo->byIndex );

	// UI
	if( _GetMapLoadingWindow()->IsShow() )
		_GetMapLoadingWindow()->Close();

	//--------------------------------------------------------------------------

	m_byMoveStep			= MMS_AFTER_LOAD_DATA;
	m_dwMoveStepStartTime	= timeGetTime();
	m_dwMoveDelay			= DEFAULT_MOVEMAP_DELAY;

	_GetDungeonInfo()->EnterDungeon( m_byMoveType );

	SetEffectInfo();
}

void
MOVEMENT_BETWEEN_MAP_INFO::MoveToPortal_Result_AfterLoadData( void )
{
	if( timeGetTime() - m_dwMoveStepStartTime >= m_dwMoveDelay )
	{
		m_byMoveStep			= MMS_BEGIN_NEW_MAP;
		m_dwMoveStepStartTime	= timeGetTime();
		return;
	}
}

void
MOVEMENT_BETWEEN_MAP_INFO::MoveToPortal_Result_BeginNewMap( void )
{
	_GetAvatar()->Enable( ROSF_MOVABLE );
	_GetNetworkMgr()->InitMsg_NewPosStartRequest_clzo( m_byMoveType );
	m_byMoveStep		= MMS_NONE;

	m_bIsMoved			= FALSE;
	m_pEnterPortalInfo	= NULL;
	m_pExitPortalInfo	= NULL;

	_GetPartyBoard()->ResetPartyMemberAfterMoveMap();
}

//==============================================================================

BOOL
MOVEMENT_BETWEEN_MAP_INFO::MoveToPortal_Result_ToOtherServer( void )
{
	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
MOVEMENT_BETWEEN_MAP_INFO::MoveToDungeon_Request( CGroundItem * pi_pDungeonItem )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;

	if( m_byMoveStep != MMS_NONE )
		return FALSE;

	if( !pi_pDungeonItem )
		return FALSE;

	CNetMsgProcessor_Dungeon * pNetMsgProcessor = static_cast< CNetMsgProcessor_Dungeon * >
												  ( _GetNetworkMgr()->GetNetMsgProcessor( NMP_DUNGEON ) );
	if( !pNetMsgProcessor )
		return FALSE;

	pNetMsgProcessor->GotoDungeonRequest_clzo( pi_pDungeonItem );

	m_pEnterPortalInfo	= NULL;
	m_pExitPortalInfo	= NULL;

	m_byEnterMapIndex	= _GetLand()->GetMapIndex();
	m_byExitMapIndex	= ID_INVALID;

	_GetGBM()->CloseUnnecessaryWindow();

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
MOVEMENT_BETWEEN_MAP_INFO::SetEffectInfo( void )
{
	// 일반 맵 이동
	if( m_byMoveType == GST_PORTAL )
	{
		if( m_byMoveStep == MMS_PREV_LOAD_DATA )
			_GetAvatar()->SetEffectInfo( CAI_GESTURE_MOVEMAP_ENTER );
		else if( m_byMoveStep == MMS_AFTER_LOAD_DATA )
			_GetAvatar()->SetEffectInfo( CAI_GESTURE_MOVEMAP_EXIT );
	}
	// 전투 던젼 이동
	else if( m_byMoveType == GST_DUNGEON )
	{
		if( m_byMoveStep == MMS_PREV_LOAD_DATA )
			_GetAvatar()->SetEffectInfo( CAI_GESTURE_DUNGEON_ENTER );
		else if( m_byMoveStep == MMS_AFTER_LOAD_DATA )
			_GetAvatar()->SetEffectInfo( CAI_GESTURE_DUNGEON_EXIT );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CDungeonInfo g_stDungeonInfo;

//------------------------------------------------------------------------------

CDungeonInfo::CDungeonInfo()
{
	m_bIsInDungeon				= FALSE;

	m_dwMonsterKillNum			= 0;
	m_dwTargetMonsterTotalNum	= 0;
	m_dwTargetMonsterKillNum	= 0;

	m_bIsEnableHint				= FALSE;

	m_pDungeonData				= NULL;
};

void
CDungeonInfo::InitKillCounterWindow( DWORD pi_dwWindowID, int pi_nPosX, int pi_nPosY )
{
	m_uiKillCounterWindow.Init();
	m_uiKillCounterWindow.SetID( pi_dwWindowID );
	m_uiKillCounterWindow.SetPos( pi_nPosX, pi_nPosY );

	if( _GetUIScreen() )
		_GetUIScreen()->Add( &m_uiKillCounterWindow );
}

void
CDungeonInfo::InitMissionDescWindow( DWORD pi_dwWindowID )
{
	m_uiMissionDescWindow.Init();
	m_uiMissionDescWindow.SetID( pi_dwWindowID );

	if( _GetUIScreen() )
		_GetUIScreen()->Add( &m_uiMissionDescWindow );
}

void
CDungeonInfo::Open( DWORD pi_dwWindowID )
{
	if( pi_dwWindowID == UII_DUNGEON_KILL_COUNTER )
		m_uiKillCounterWindow.Open();
	else if( pi_dwWindowID == UII_DUNGEON_MISSION_DESC )
		m_uiMissionDescWindow.Open();
}

void
CDungeonInfo::Close( DWORD pi_dwWindowID )
{
	if( pi_dwWindowID == UII_DUNGEON_KILL_COUNTER )
		m_uiKillCounterWindow.Close();
	else if( pi_dwWindowID == UII_DUNGEON_MISSION_DESC )
		m_uiMissionDescWindow.Close();
}

BOOL
CDungeonInfo::IsOpen( DWORD pi_dwWindowID )
{
	if( pi_dwWindowID == UII_DUNGEON_KILL_COUNTER )
		return m_uiKillCounterWindow.IsShow();
	else if( pi_dwWindowID == UII_DUNGEON_MISSION_DESC )
		return m_uiMissionDescWindow.IsShow();

	return FALSE;
}

void
CDungeonInfo::OpenMissionDescWindow( BYTE pi_byType )
{
	char *	l_pDesc[2] = { NULL, NULL };
	char	l_pPrizeMsg[128];

	l_pDesc[0] = _GetItemDataMgr()->GetDungeonDescription( m_pDungeonData->GetDungeonDescDTIndex(),
														   CDungeonDescription::DDT_BRIEFING );
	switch( pi_byType )
	{
		case CGUIMissionStateWin::Start :
			if( m_pDungeonData )
			{
				ZeroMemory( l_pPrizeMsg, 128 );
				if( m_pDungeonData->GetPrizeDalant() > 0 )
					sprintf( l_pPrizeMsg, "보상 - %s : %d", _GetAvatarMoneyName(), m_pDungeonData->GetPrizeDalant() );
				if( m_pDungeonData->GetPrizeGold() > 0 )
					sprintf( l_pPrizeMsg, "%s  골드 : %d ", l_pPrizeMsg, m_pDungeonData->GetPrizeGold() );
				if( m_pDungeonData->GetPrizeExp() > 0 )
					sprintf( l_pPrizeMsg, "%s  경험치 : %d ", l_pPrizeMsg, m_pDungeonData->GetPrizeExp() );

				l_pDesc[1] = l_pPrizeMsg;

				m_uiMissionDescWindow.SetMissionInfoStr( l_pDesc );
			}
			break;

		case CGUIMissionStateWin::Clear :
			break;

		case CGUIMissionStateWin::Fail :
			l_pDesc[1] = NULL;
			m_uiMissionDescWindow.SetMissionInfoStr( l_pDesc );
			break;
	};

	m_uiMissionDescWindow.SetMissionState( ( CGUIMissionStateWin::MissionState )pi_byType );
	m_uiMissionDescWindow.Open();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD
CDungeonInfo::OpenPortal( BYTE pi_byMapMoveType )
{
	// enter dungeon
	if( !m_bIsInDungeon && pi_byMapMoveType == GST_DUNGEON )
	{
		EFUtil_SetMotionBlurTime( DUNGEON_PORTAL_OPEN_DELAY / 1000 );
		::Play2DSound( 0x50FFFF36 );
		_GetCamera()->SetSpecialCameraMode( 6 );
		return ( DUNGEON_PORTAL_OPEN_DELAY - 500 );
	}
	// exit dungeon
	else if( m_bIsInDungeon )
	{
		if( ( m_byDungeonExitType == DET_CHEAT ) || ( m_byDungeonExitType == DET_PORTAL ) )
			return DEFAULT_MOVEMAP_DELAY;
		else if( m_byDungeonExitType == DET_CLEAR )
			OpenMissionDescWindow( CGUIMissionStateWin::Clear );
		else
			OpenMissionDescWindow( CGUIMissionStateWin::Fail );
		return DUNGEON_DESC_OPEN_DELAY;
	}

	return DEFAULT_MOVEMAP_DELAY;
}

void
CDungeonInfo::ClosePortalOrExitDungeon( BYTE pi_byMapMoveType )
{
	// enter dungeon
	if( !m_bIsInDungeon && pi_byMapMoveType == GST_DUNGEON )
	{
		EFUtil_SetMotionBlurOff();
		_GetCamera()->SetCameraMode( _GetOptionWindow()->GetSelectedCamera() );
	}
	// exit dungeon
	else if( m_bIsInDungeon )
	{
		ExitDungeon();
	}
}

void
CDungeonInfo::EnterDungeon( BYTE pi_byMapMoveType )
{
	if( !m_bIsInDungeon && pi_byMapMoveType == GST_DUNGEON )
	{
		m_dwMonsterKillNum			= 0;
		m_dwTargetMonsterKillNum	= 0;

		if( m_pDungeonData )
			m_dwTargetMonsterTotalNum	= m_pDungeonData->GetTargetMonsterTotalNum();

		m_bIsEnableHint	= _GetMainGame()->IsEnableHint();
		_GetMainGame()->EnableHint( FALSE );

		m_uiKillCounterWindow.Open();
		m_uiKillCounterWindow.SetMaxKillingNum( m_dwTargetMonsterTotalNum );
		m_uiKillCounterWindow.SetCurrentKillingNum( m_dwTargetMonsterKillNum );

		OpenMissionDescWindow( CGUIMissionStateWin::Start );
	}
}

void
CDungeonInfo::ExitDungeon( void )
{
	if( m_bIsInDungeon )
	{
		_GetMainGame()->EnableHint( m_bIsEnableHint );
		m_uiKillCounterWindow.Close();
		m_uiMissionDescWindow.Close();

		m_dwEnterTime	= 0;
		m_dwExitTime	= 0;
	}
}

void
CDungeonInfo::SetEnterDungeonTime( DWORD pi_dwRemainTime )
{
	m_dwEnterTime	= timeGetTime();
	m_dwExitTime	= m_dwEnterTime + ( pi_dwRemainTime * 1000 );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD
CDungeonInfo::GetRemainTime( void )
{
	DWORD l_dwCurTime = timeGetTime();

	if( l_dwCurTime >= m_dwExitTime )
		return 0;

	return ( ( m_dwExitTime - l_dwCurTime ) / 1000 );
}

void
CDungeonInfo::SetDungeonData( CGroundItem * pi_pDungeonItem )
{
	if( pi_pDungeonItem )
		m_pDungeonData = static_cast< DUNGEON_ITEM_DATA * >( pi_pDungeonItem->GetData() );
}

void
CDungeonInfo::SetTargetMonsterKillNum( DWORD pi_dwNum )
{
	m_dwTargetMonsterKillNum = pi_dwNum;
	m_uiKillCounterWindow.SetCurrentKillingNum( pi_dwNum );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

