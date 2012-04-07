////////////////////////////////////////////////////////////////////////////////
//
// CGP_MainGame Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../GameObject/Manager/CGroundItemManager.h"
#include "../Character/Avatar/CAvatar_Quest.h"
#include "../GameObject/Manager/CTargetMgr.h"

#include "../CLand.h"
#include "../CCamera.h"
#include "../Item/CItemMgr.h"

#include "../Input/IMEEdit.h"

#include "../DataFile.h"
#include "../DataTable/CResourceDataMgr.h"

#include "../AddCodeByJang/ForEffect.h"
#include "../AddCodeByJang/effectutil.h"
#include "../AddCodeByJang/effectutilfont.h"

#include "CGP_MainGame.h"
#include "../MainApp.h"

#include "CInputProcessMgr.h"

#include "../GameObject/Board/General/CGameEventBoard.h"
#include "../GameObject/Board/General/CPartyBoard.h"
#include "../GameObject/Board/General/CHelpBoard.h"

#ifdef _NEW_UI_
	#include "../Item/CItemData.h"
	#include "../GameObject/Board/Common/CBoardManager.h"
#endif

#ifdef _DEVELOPER_
#include "RTMovie.h"
#endif
#include "R3Profiler.h"

//------------------------------------------------------------------------------

extern BOOL	g_bWriteTestLog;	// 테스트 코드

//------------------------------------------------------------------------------

#define	WM_MOUSEWHEEL	0x020A

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CGP_MainGame::CGP_MainGame()
{
	Init();
}

CGP_MainGame::~CGP_MainGame()
{
	Destroy();
}

void
CGP_MainGame::Init( void )
{
	m_bIsCompleteDataDownload	= FALSE;
	m_bIsCreated_UI				= FALSE;
	m_bIsCreated_Board			= FALSE;
	m_bIsCreated_Avatar			= FALSE;

	m_dwDisplayInfomationMode	= DISPLAY_INFO_NONE;
	m_bIsRenderCollisionMap		= FALSE;
	m_bIsRenderBoundBox			= FALSE;
	m_bIsDisplayHelpText		= FALSE;
	m_bIsDisplayNetworkMessage	= FALSE;

	m_bIsEnableInput			= TRUE;

	m_dwRequestDataUploadTime	= 0;
	m_dwRequestLogoutTime		= 0;

	m_bIsChatMode				= TRUE;
	m_bIsMoveByDrag				= FALSE;
	m_bIsAutoMouse				= FALSE;
	m_bIsAutoTargeting			= FALSE;
	m_bIsFreeTargetCombatMode	= FALSE;
	m_bIsPlayMovie				= FALSE;
	m_dwMovieFlag				= 0;
	m_bIsHideUI					= FALSE;

	m_bIsMove_Cursor			= FALSE;

	m_bIsRequestLogout			= FALSE;

	m_pInputProcessMgr			= new CInputProcessMgr;
	m_pHintMgr					= new CHintMgr;
	m_pUIHelpMgr				= new CUIHelpMgr;
	m_pGameEventBoard			= new CGameEventBoard;
	m_pPartyBoard				= new CPartyBoard;
	m_pHelpBoard				= new CHelpBoard;

	m_pTargetMgr				= new CTargetMgr;
	m_pNextActionMgr			= new CNextActionMgr;
}

BOOL
CGP_MainGame::Create( void )
{
	if( m_bIsCreated )
		return TRUE;

	if( !CGameProgress::Create() )
		return FALSE;

	//--------------------------------------------------------------------------

	Create_UI();
	Create_Board();

	if( !m_bIsCompleteDataDownload )
		return FALSE;

	LoadData();

	Create_Avatar();

	LoadGameSetting();

	//--------------------------------------------------------------------------

	_GetNetworkMgr()->InitMsg_NewPosStartRequest_clzo( GST_NEW_GAME );
	_GetNetworkMgr()->SecureMsg_ResponFreeRequest();

	g_pGameEventBoard	= m_pGameEventBoard;
	g_pPartyBoard		= m_pPartyBoard;
	g_pHelpBoard		= m_pHelpBoard;

	g_pTargetMgr		= m_pTargetMgr;
	g_pNextActionMgr	= m_pNextActionMgr;

	//--------------------------------------------------------------------------

	m_bIsCreated = TRUE;

	return TRUE;
}

void
CGP_MainGame::Create_UI( void )
{
	if( m_bIsCreated_UI )
		return;

	// ui 종족별 세팅을 위해 필요함
	_SetUIAvatarRace( _GetAvatar()->GetRace() );

	UI_LoadSprite();

	_GetUIScreen()->RemoveAll();
	_GetUIScreen()->Add( &m_uiGaugeWindow );
	_GetUIScreen()->Add( &m_uiChatWindow );		
	_GetUIScreen()->Add( &m_uiCharInfoWindow );
	_GetUIScreen()->Add( &m_uiFunctionKeyWindow );
	_GetUIScreen()->Add( &m_uiChatInputWindow );
	_GetUIScreen()->Add( &m_uiSelectAttackPartWindow );
	_GetUIScreen()->Add( &m_uiSummonsSelectWindow );
	_GetUIScreen()->Add( &m_uiSummonsControlWindow );
	_GetUIScreen()->Add( &m_uiNpcIntroduceWindow );		
	//_GetUIScreen()->Add( &m_uiTargetInfoBar );
	//_GetUIScreen()->Add( &m_uiTargetInfoWindow );
	//_GetUIScreen()->Add( &m_uiPartyWindow );	
	//_GetUIScreen()->Add( &m_uiSelectedCharMenu );
	_GetUIScreen()->Add( &m_uiMoveMapWindow );
	_GetUIScreen()->Add( &m_uiAppliedSFWindow );	
	_GetUIScreen()->Add( &m_uiRadarWindow );
	_GetUIScreen()->Add( &m_uiMinimapWindow );
	_GetUIScreen()->Add( &m_uiMapLoadingWindow );
	_GetUIScreen()->Add( &m_uiOptionWindow );

	//--------------------------------------------------------------------------
	// initialize (layout)
	m_uiGaugeWindow.Init();		
	m_uiChatWindow.Init();		
	m_uiCharInfoWindow.Init();
	m_uiFunctionKeyWindow.Init();
	m_uiSelectAttackPartWindow.Init();
	m_uiSummonsSelectWindow.Init();
	m_uiSummonsControlWindow.Init();
	m_uiNpcIntroduceWindow.Init();		
	//m_uiTargetInfoBar.Init();
	//m_uiTargetInfoWindow.Init();
	//m_uiPartyWindow.Init();		
	//m_uiSelectedCharMenu.Init();
	m_uiMoveMapWindow.Init();	
	m_uiRadarWindow.Init();
	m_uiMapLoadingWindow.Init();
	m_uiOptionWindow.Init();

	//--------------------------------------------------------------------------
	// window pointer setting
	
	m_uiFunctionKeyWindow.SetRefWindow( CGUIHotKeyBoard::CharInfo, &m_uiCharInfoWindow );	
	m_uiFunctionKeyWindow.SetRefWindow( CGUIHotKeyBoard::Menu, &m_uiOptionWindow );
	

	//--------------------------------------------------------------------------
	// Set position
	m_uiChatWindow.SetPos( 0, _GetUIScreen()->m_ptSize.y - m_uiChatWindow.m_ptSize.y - m_uiFunctionKeyWindow.m_ptSize.y );
	m_uiChatInputWindow.SetPos( _GetUIScreen()->m_ptPos.x + 5,
								_GetUIScreen()->m_ptSize.y - m_uiChatInputWindow.m_ptSize.y - m_uiFunctionKeyWindow.m_ptSize.y - 5 );	
	m_uiCharInfoWindow.SetPos( ( _GetUIScreen()->m_ptSize.x - m_uiCharInfoWindow.m_ptSize.x ) * 0.4,
							   ( _GetUIScreen()->m_ptSize.y - m_uiCharInfoWindow.m_ptSize.y ) * 0.2 );		
	
	m_uiFunctionKeyWindow.SetPos( 0, _GetUIScreen()->m_ptSize.y - m_uiFunctionKeyWindow.m_ptSize.y );		
	m_uiSelectAttackPartWindow.SetPos( 0, ( _GetUIScreen()->m_ptSize.y - m_uiSelectAttackPartWindow.m_ptSize.y ) / 2 );
	

	m_uiSummonsSelectWindow.SetPos( 0, m_uiGaugeWindow.m_ptPos.y + m_uiGaugeWindow.m_ptSize.y );
	m_uiSummonsControlWindow.SetPos( 0, m_uiSummonsSelectWindow.m_ptPos.y + m_uiSummonsSelectWindow.m_ptSize.y );
	m_uiNpcIntroduceWindow.SetPos( ( _GetUIScreen()->m_ptSize.x - m_uiNpcIntroduceWindow.m_ptSize.x ) * 0.3,
								   ( _GetUIScreen()->m_ptSize.y - m_uiNpcIntroduceWindow.m_ptSize.y ) * 0.3 );
	

	//m_uiPartyWindow.SetPos( 0, m_uiGaugeWindow.m_ptPos.y + m_uiGaugeWindow.m_ptSize.y );	
	//m_uiSelectedCharMenu.SetPos( 600, 400 );
	m_uiMoveMapWindow.SetPos( ( _GetUIScreen()->m_ptSize.x - m_uiMoveMapWindow.m_ptSize.x ) / 2,
							  ( _GetUIScreen()->m_ptSize.y - m_uiMoveMapWindow.m_ptSize.y ) / 2 );
	m_uiAppliedSFWindow.SetPos( _GetUIScreen()->m_ptSize.x, ( _GetUIScreen()->m_ptSize.y - m_uiAppliedSFWindow.m_ptSize.y ) * 0.3 );	
	m_uiRadarWindow.SetPos( _GetUIScreen()->m_ptSize.x - m_uiRadarWindow.m_ptSize.x, 0 );	
	m_uiOptionWindow.SetPos( ( _GetUIScreen()->m_ptSize.x - m_uiOptionWindow.m_ptSize.x ) / 2, 0 );
	//m_uiTargetInfoWindow.SetPos( m_uiGaugeWindow.m_ptPos.x + m_uiGaugeWindow.m_ptSize.x, 0 );
	m_uiMinimapWindow.SetPos( ( _GetUIScreen()->m_ptSize.x - m_uiMinimapWindow.m_ptSize.x ) / 2, 
							  ( _GetUIScreen()->m_ptSize.y - m_uiMinimapWindow.m_ptSize.y ) / 2 );

	//--------------------------------------------------------------------------		
	
	m_uiFunctionKeyWindow.SetRaceType( _GetAvatar()->GetRace() );

	//--------------------------------------------------------------------------

	m_uiCharInfoWindow.SetGaugeExpWindow( &m_uiGaugeWindow );
	m_uiChatWindow.SetChatInputWindow( &m_uiChatInputWindow );
	

	m_uiGaugeWindow.EnableDocking( true );
	m_uiGaugeWindow.AddDockingCustomer( &m_uiSummonsControlWindow );
	//m_uiGaugeWindow.AddDockingCustomer( &m_uiTargetInfoWindow );
	//m_uiGaugeWindow.AddDockingCustomer( &m_uiMiningWindow );
		
	//m_uiTargetInfoWindow.EnableDockingBorder( CGUIWindow::RIGHT, TRUE );
	//m_uiMiningWindow.EnableDockingBorder( CGUIWindow::RIGHT, TRUE );
	//m_uiTargetInfoWindow.SetDockingBorder( CGUIWindow::RIGHT );
	//m_uiMiningWindow.SetDockingBorder( CGUIWindow::RIGHT );

	//--------------------------------------------------------------------------
	m_uiGaugeWindow.SetAlwaysTop( true );
	m_uiChatWindow.SetAlwaysActive( true );
	m_uiChatInputWindow.SetAlwaysActive( true );

	// open window
	m_uiChatWindow.Close(); // 채팅창을 닫으면 입력창이 열린다
	m_uiGaugeWindow.Open();
	m_uiFunctionKeyWindow.Open();
	m_uiRadarWindow.Open();
	m_uiRadarWindow.SetYAxisViewRange( 200.0f );

	//--------------------------------------------------------------------------
	// UI ID 설정
	m_uiGaugeWindow.SetID( UII_GAUGE_WINDOW );	
	m_uiChatWindow.SetID( UII_CHAT_WINDOW );			
	m_uiFunctionKeyWindow.SetID( UII_FUNC_KEY_WINDOW );
	m_uiCharInfoWindow.SetID( UII_CHAR_INFO_WINDOW );
	m_uiSelectAttackPartWindow.SetID( UII_SELECT_ATTACK_PART_WINDOW );		
	//m_uiSummonsSelectWindow.SetID( UII_SUMMONS_SELECT_WINDOW );
	//m_uiSummonsControlWindow.SetID( UII_SUMMONS_CONTROL_WINDOW );
	m_uiNpcIntroduceWindow.SetID( UII_NPC_INTRODUCE_WINDOW );		
	//m_uiTargetInfoBar.SetID( UII_TARGET_INFO_BAR );
	//m_uiTargetInfoWindow.SetID( UII_TARGET_INFO_WINDOW );
	//m_uiPartyWindow.SetID( UII_PARTY_WINDOW );
	
	//m_uiSelectedCharMenu.SetID( UII_SELECTED_CHAR_MENU );
	m_uiMoveMapWindow.SetID( UII_MOVE_MAP_WINDOW );
	m_uiAppliedSFWindow.SetID( UII_APPLIED_SF_WINDOW );	
	m_uiRadarWindow.SetID( UII_RADAR_WINDOW );
	m_uiMapLoadingWindow.SetID( UII_RADAR_WINDOW );
	m_uiOptionWindow.SetID( UII_OPTION_WINDOW );

	g_pUiGaugeWindow			= &m_uiGaugeWindow;	
	g_pUiChatWindow				= &m_uiChatWindow;		
	g_pUiCharInfoWindow			= &m_uiCharInfoWindow;
	g_pUiFuncKeyWindow			= &m_uiFunctionKeyWindow;
	g_pUiChatInputWindow		= &m_uiChatInputWindow;
	g_pUiSelectAttackPartWindow	= &m_uiSelectAttackPartWindow;	
	g_pUiSummonsSelectWindow	= &m_uiSummonsSelectWindow;
	g_pUiSummonsControlWindow	= &m_uiSummonsControlWindow;
	g_pUiNpcIntroduceWindow		= &m_uiNpcIntroduceWindow;		
	//g_pUiTargetInfoBar		= &m_uiTargetInfoBar;
	//g_pUiTargetInfoWindow		= &m_uiTargetInfoWindow;
	//g_pUiPartyWindow			= &m_uiPartyWindow;
	//g_pUiSelectedCharMenu		= &m_uiSelectedCharMenu;
	g_pUiMoveMapWindow			= &m_uiMoveMapWindow;
	g_pUiAppliedSFWindow		= &m_uiAppliedSFWindow;	
	g_pUiRadarWindow			= &m_uiRadarWindow;
	g_pUiMinimapWindow			= &m_uiMinimapWindow;
	g_pUiMapLoadingWindow		= &m_uiMapLoadingWindow;
	g_pUiOptionWindow			= &m_uiOptionWindow;

	//--------------------------------------------------------------------------
	// UI Proc 설정
	m_uiChatWindow.SetEventProc_Ex( EventProcChatWindow );
	m_uiChatInputWindow.SetEventProc_Ex( EventProcChatWindow );

	//--------------------------------------------------------------------------
	// UI 기타 설정
	_GetGBM()->SetHoverObject( NULL );

	m_dwHoverMouseUI = ID_INVALID;
	
	// avatar info setting
	m_uiCharInfoWindow.SetRaceType( _GetAvatar()->GetRace() );
	m_uiCharInfoWindow.SetLevel( _GetAvatar()->GetLevel() );
	_SetCharInfoWindow( &m_uiCharInfoWindow );

	//_GetPartyWindow()->SetWindowType( UI_PMT_NONE );

	_GetLand()->UI_SetRadar();
	//_GetLand()->UI_SetMinimap();	


	//--------------------------------------------------------------------------
	// 채팅 모드를 비채팅 모드로 설정
	m_bIsChatMode = FALSE;
	_GetChatWindow()->GetInputField()->DisableFocus();
	_GetChatWindow()->GetWhisperInputField()->DisableFocus();
	_GetChatInputWindow()->GetInputField()->DisableFocus();

	//--------------------------------------------------------------------------

	m_bIsCreated_UI = TRUE;
}

void
CGP_MainGame::Create_Board( void )
{
	if( m_bIsCreated_Board )
		return;

	// create board
	_GetGBM()->Create();

	// init inventory
	_GetInventoryBoard()->Create( IBT_INVENTORY, MAX_INVENTORY_PACK, MAX_INVENTORY_SOCKET );
	_GetInventoryBoard()->InitWindow( UII_INVENTORY, 400, 0 );

	// init link board
	_GetLinkBoard()->Create( IBT_SF_BELT, 1, MAX_SF_BELT_SOCKET );
	_GetLinkBoard()->InitWindow( UII_SF_BELT_WINDOW, 400, 500 );
	_GetLinkBoard()->AddPack( 0 );
	_GetLinkBoard()->OpenWindow();

	// init item belt
	_GetBeltBoard()->Create( IBT_ITEM_BELT, 1, MAX_ITEM_BELT_SOCKET );
	_GetBeltBoard()->InitWindow( UII_ITEM_BELT_WINDOW, 250, 500 );
	_GetBeltBoard()->AddPack( 0 );
	_GetBeltBoard()->OpenWindow();

	// init unit tuning board
	_GetUnitTuningBoard()->Create( IBT_UNIT_TUNING, MAX_UNIT_PART_TYPE + MAX_UNIT_BULLET_TYPE, MAX_MERCHANT_ITEM_NUM );
	_GetUnitTuningBoard()->InitWindow( UII_UNIT_TUNING_WINDOW, 300, 0 );
	for( int i = 0; i < MAX_UNIT_PART_TYPE + MAX_UNIT_BULLET_TYPE; ++i )
		_GetUnitTuningBoard()->AddPack( i );

	// init unit storehouse board
	_GetUnitStorehouseBoard()->Create( IBT_UNIT_STOREHOUSE, 1, MAX_AVATAR_UNIT );
	_GetUnitStorehouseBoard()->InitWindow( UII_UNIT_STOREHOUSE_WINDOW, 300, 100 );
	_GetUnitStorehouseBoard()->AddPack( 0 );

	// init equip board
	_GetEquipBoard()->Create( IBT_EQUIP, 1, MAX_EQUIP_WINDOW_SLOT );
	_GetEquipBoard()->InitWindow( UII_EQUIP_WINDOW, 0, 0 );
	_GetEquipBoard()->AddPack( 0 );
	_GetInventoryBoard()->SetEquipWindow( static_cast< CGUIEquipWindow * >( _GetEquipBoard()->GetWindow() ) );

	// init skill board
	_GetSkillBoard()->Create( IBT_SKILL, MAX_UI_SKILL_CLASS_TYPE, MAX_SKILL / MAX_UI_SKILL_CLASS_TYPE );
	_GetSkillBoard()->InitWindow( UII_SKILL_WINDOW, 200, 60 );
	_GetSkillBoard()->CreateBoardObject( MAX_UI_SKILL_CLASS_TYPE, MAX_SKILL_MASTERY_NUM, MAX_SKILL );

	// init force board
	_GetForceBoard()->Create( IBT_FORCE, MAX_UI_FORCE_CLASS_TYPE, MAX_FORCE / MAX_UI_FORCE_CLASS_TYPE );
	_GetForceBoard()->InitWindow( UII_FORCE_WINDOW, 300, 60 );	
	_GetForceBoard()->CreateBoardObject( MAX_UI_FORCE_CLASS_TYPE, MAX_FORCE_MASTERY_NUM, MAX_FORCE );

	// init animus board
	_GetAnimusBoard()->Create( IBT_ANIMUS, 1, 10 );
	_GetAnimusBoard()->InitWindow( UII_ANIMUS_WINDOW, 400, 60 );
	_GetAnimusBoard()->CreateBoardObject();

	// init animus control board
	_GetAnimusControlBoard()->InitWindow( UII_ANIMUS_CONTROL_WINDOW, 0, m_uiGaugeWindow.m_ptPos.y + m_uiGaugeWindow.m_ptSize.y );

	// init shop board
	_GetShopTradeBoard()->Create( IBT_SHOP, 1, MAX_MERCHANT_ITEM_NUM );
	_GetShopTradeBoard()->AddPack( 0 );
	_GetShopTradeBoard()->InitWindow( UII_SHOP_WINDOW, 200, 0 );

	// init player trade board
	_GetPlayerTradeBoard()->Create( IBT_PLAYER_TRADE, 1, MAX_PLAYER_TRADE_ITEM_NUM );
	_GetPlayerTradeBoard()->InitWindow( UII_TRADE_WINDOW, 200, 0 );

	// init ore proc
	_GetOreProcBoard()->Create( IBT_ORE_PROCESSING, 1, MAX_ORE_PROC_SOCKET );
	_GetOreProcBoard()->AddPack( 0 );
	_GetOreProcBoard()->InitWindow( UII_ORE_PROC_WINDOW, 0, 0 );

	// init item work
	_GetItemWorkBoard()->Create( IBT_ITEM_WORK, 0, 0 );
	_GetItemWorkBoard()->InitWindow( UII_ITEM_WORKING_WINDOW, 0, 0 );

	// init upgrade board
	_GetItemUpgradeBoard()->Create( IBT_ITEM_UPGRADE, 1, MAX_UIAT_TYPE );
	_GetItemUpgradeBoard()->InitWindow( UII_ITEM_UPGRADE_WINDOW, 0, 0 );
	_GetItemUpgradeBoard()->AddPack( 0 );		

	// etc board
	_GetExchangeBoard()->InitWindow( UII_EXCHANGE_WINDOW, 0, 0 );
	_GetMiningBoard()->InitWindow( UII_MINING_WINDOW, 0, 0 );

	// class change board
	_GetClassChangeBoard()->InitWindow( UII_CLASS_CHANGE_WINDOW, 200, 100 );

	// function key
	m_uiFunctionKeyWindow.SetRefWindow( CGUIHotKeyBoard::Inventory, _GetInventoryBoard()->GetWindow() );
	m_uiFunctionKeyWindow.SetRefWindow( CGUIHotKeyBoard::Force, _GetForceBoard()->GetWindow() );
	m_uiFunctionKeyWindow.SetRefWindow( CGUIHotKeyBoard::Skill, _GetSkillBoard()->GetWindow() );
	m_uiFunctionKeyWindow.SetRefWindow( CGUIHotKeyBoard::Summons, (CGUIRecallWindow *)_GetAnimusBoard()->GetWindow() );
	m_uiFunctionKeyWindow.SetBeltWindow( (CGUIItemBeltWindow *)_GetBeltBoard()->GetWindow(), (CGUIForceBeltWindow *)_GetLinkBoard()->GetWindow() );


	// etc
	g_pUiInventWindow		= static_cast< CGUIInventoryWindow * >( _GetInventoryBoard()->GetWindow() );
	g_pUiItemBeltWindow		= static_cast< CGUIItemBeltWindow * >( _GetBeltBoard()->GetWindow() );
	g_pUiSFBeltWindow		= static_cast< CGUIForceBeltWindow * >( _GetLinkBoard()->GetWindow() );
	g_pUiSkillWindow		= static_cast< CGUISkillWindow * >( _GetSkillBoard()->GetWindow() );
	g_pUiForceWindow		= static_cast< CGUIForceWindow * >( _GetForceBoard()->GetWindow() );
	g_pUiShopWindow			= static_cast< CGUIShopWindow * >( _GetShopTradeBoard()->GetWindow() );
	g_pUiTradeWindow		= static_cast< CGUITradeWindow * >( _GetPlayerTradeBoard()->GetWindow() );
	g_pUiOreProcWindow		= static_cast< CGUIOreProcWindow * >( _GetOreProcBoard()->GetWindow() );
	g_pUiItemWorkWindow		= static_cast< CGUIItemMakeWindow * >( _GetItemWorkBoard()->GetWindow() );
	g_pUiItemUpgradeWindow	= static_cast< CGUIItemUpgradeWindow * >( _GetItemUpgradeBoard()->GetWindow() );
	g_pUiExchangeWindow		= static_cast< CGUIExchangeWindow * >( _GetExchangeBoard()->GetWindow() ); 
	g_pUiMiningWindow		= static_cast< CGUIMiningWindow * >( _GetMiningBoard()->GetWindow() );

	m_uiGaugeWindow.AddDockingCustomer( g_pUiMiningWindow );
	g_pUiMiningWindow->EnableDockingBorder( CGUIWindow::RIGHT, TRUE );
	g_pUiMiningWindow->SetDockingBorder( CGUIWindow::RIGHT );

	//--------------------------------------------------------------------------

	RECT rtWindowRect;

	m_pGameEventBoard->InitWindow( UII_EVENT_WINDOW,
								   GetR3State()->mScreenXsize - 40, GetR3State()->mScreenYsize - 70,
								   _GetUIScreen() );

	_GetAnimusControlBoard()->GetWindowRect( rtWindowRect );

	m_pPartyBoard->InitWindow( UII_PARTY_WINDOW, 0, rtWindowRect.bottom );
	m_pHelpBoard->InitWindow( UII_HELP_WINDOW,
							  0, m_uiGaugeWindow.m_ptPos.y + m_uiGaugeWindow.m_ptSize.y,
							  _GetUIScreen() );

	m_pTargetMgr->InitTargetInfoBar( UII_TARGET_INFO_BAR, _GetUIScreen() );
	m_pTargetMgr->InitTargetInfoWindow( UII_TARGET_INFO_WINDOW, &m_uiGaugeWindow, _GetUIScreen() );
	m_pTargetMgr->InitPlayerPopupMenu( UII_PLAYER_POPUP_MENU, _GetUIScreen() );

	_GetDungeonInfo()->InitKillCounterWindow( UII_DUNGEON_KILL_COUNTER,
											  860+ GetR3State()->mScreenXsize-1024,
											  585+ GetR3State()->mScreenYsize-768 );
	_GetDungeonInfo()->InitMissionDescWindow( UII_DUNGEON_MISSION_DESC );

	//--------------------------------------------------------------------------

	// board를 생성한 후에, 네트웍 메세지를 보냄.
	_GetNetworkMgr()->InitMsg_BaseDownloadRequest_clzo();
	g_pMainGame			= this;

	m_bIsCreated_Board	= TRUE;
}

void
CGP_MainGame::Create_Avatar( void )
{
	if( m_bIsCreated_Avatar )
		return;

	// 아바타 설정
	_GetAvatar()->SetIndex( _GetNetworkMgr()->GetAvatarIndex() );

	float * l_vecTarget = _GetNetworkMgr()->GetAvatarStartPos();
	_GetAvatar()->SetPosition( l_vecTarget );
	//_GetAvatar()->SetPrevPosition( l_vecTarget[0]+0.5f,l_vecTarget[1],l_vecTarget[2] );//서버는 정수값으로 계산하므로 정확한 위치로 하기위해.JUI
	_GetAvatar()->CheckYPos();

	//_GetLevel()->GetFirstYpos( l_vecTarget, -1 );	// GetNextYpos 를 위해서 호출
	_GetAvatar()->CreatePath( l_vecTarget );

	// UI 를 위한 재설정
	_GetAvatar()->SetUserGrade( _GetNetworkMgr()->GetAvatarGrade() );

	_GetAvatar()->SetExp( _GetAvatar()->GetExp() );
	_GetAvatar()->SetLevel( _GetAvatar()->GetLevel() );
	_GetAvatar()->SetHP( _GetAvatar()->GetHP() );
	_GetAvatar()->SetFP( _GetAvatar()->GetFP() );
	_GetAvatar()->SetSP( _GetAvatar()->GetSP() );

	_GetAvatar()->SetDalant( _GetAvatar()->GetDalant() );
	_GetAvatar()->SetGold( _GetAvatar()->GetGold() );

	// 캐릭터 정보창 설정
	_GetCharInfoWindow()->SetCharName( _GetAvatar()->GetName() );
	_GetCharInfoWindow()->SetRaceType( _GetAvatar()->GetRace() );
	_GetCharInfoWindow()->SetAttribute( _GetAvatar()->GetClassName() );
	for( int i = 0; i < MAX_EXPERTNESS_VALUE_TYPE; ++i )
		_GetAvatar()->SetExpertness( i, _GetAvatar()->GetSuccessCountOfExpertness( i ) );

	// 스킬창 재설정
	_GetAvatar()->RefreshClassInfo();

	// 상태 정보 재설정
	_GetAvatar()->RefreshConditionInfo();

	// 애니메이션 로드
	_GetAvatar()->LoadAnimaitonData();

	// 모드 설정
	_GetAvatar()->SetCombatMode( CSI_PEACE );
	_GetAvatar()->SetWalkMode( CMM_MOVE_RUN );

	// 액션 설정
	if( _GetAvatar()->GetHP() <= 0 )
		_GetAvatar()->SetAction( CAI_DEATH_DIE );
	else
		_GetAvatar()->SetAction( CAI_MOVE_STOP );

	_GetAvatar()->Disable( ROSF_RENDER_ALPHA );
	_GetAvatar()->SetAlphaDensity( 1.0f );

	//--------------------------------------------------------------------------
	// 아바타의 무기와 방패 설정
	CEquipItem * pEquipItem = NULL;
	pEquipItem = _GetAvatar()->GetEquipItem( IEPT_WEAPON );
	if( pEquipItem )
	{
		if( _GetAvatar()->GetCombatMode() == CSI_PEACE )
			pEquipItem->SetFadeMode( FRM_IN, 1 );
		else
			pEquipItem->SetFadeMode( FRM_OUT, 1 );
	}

	pEquipItem = _GetAvatar()->GetEquipItem( IEPT_SHIELD );
	if( pEquipItem )
	{
		if( _GetAvatar()->GetCombatMode() == CSI_PEACE )
			pEquipItem->SetFadeMode( FRM_IN, 1 );
		else
			pEquipItem->SetFadeMode( FRM_OUT, 1 );
	}

	//--------------------------------------------------------------------------
	// UI 세팅
	/*
	_GetSkillBoard()->SetUI();
	_GetForceBoard()->SetUI();

	_GetEquipBoard()->SetUI();
	_GetItemBeltBoard()->SetUI();
	_GetInventoryBoard()->SetUI();

	_GetSFBeltBoard()->SetUI();

	_GetOreProcBoard()->SetUI();*/

	//--------------------------------------------------------------------------
	// etc
	_GetAvatar()->CheckYPos();	

	// NPC의 Y좌표 설정
	CNpc * l_pNpc;
	for( i = 0; i < _GetCharMgr()->GetTotalNpcNum(); ++i )
	{
		l_pNpc = ( CNpc * )_GetCharMgr()->GetCharacterByOrder( CTI_NPC, i );
		if( l_pNpc )
			l_pNpc->CheckYPos();
	}

	_GetDungeonInfo()->SetInDungeon( FALSE );

	/*
	//--------------------------------------------------------------------------
	// 테스트 코드
	cnm_create_player l_sInfo;
	l_sInfo.wIndex = 100;
	l_sInfo.wRecIndex = 0;
	_GetAvatar()->GetPosition( l_sInfo.vecPos );

	CPlayer * l_pPlayer = _GetNetworkMgr()->CreatePlayer( &l_sInfo );

	WORD l_pModelPerPart[view_fix_num];
	ZeroMemory( l_pModelPerPart, sizeof( WORD ) * view_fix_num );

	_GetNetworkMgr()->SetPlayerItem( l_pPlayer, l_pModelPerPart, 0 );

	l_pPlayer->SetName( "My Test Dummy" );

	if( _GetAvatar()->GetRace() == 0 )
		l_pPlayer->SetRace( 1 );
	else
		l_pPlayer->SetRace( 0 );
	// 테스트 코드
	//--------------------------------------------------------------------------
	*/

	m_bIsCreated_Avatar = TRUE;
}

void
CGP_MainGame::LoadGameSetting( void )
{
	const char l_pFileName[] = "./DataTable/GameSetting.ini";

	int l_nIntValue;

	l_nIntValue = GetPrivateProfileInt( "Performance", "AutoVisualFieldAdjustment", 1, l_pFileName );
	_GetCharMgr()->EnableAutoVisualFieldAdjustment( l_nIntValue );
	l_nIntValue = GetPrivateProfileInt( "Performance", "VisualFieldAdjustmentLevel", 5, l_pFileName );
	_GetCharMgr()->SetVisualFieldAdjustmentLevel( l_nIntValue );

	l_nIntValue = GetPrivateProfileInt( "Camera", "Mode", 1, l_pFileName );
	if( l_nIntValue >= 5 )
		l_nIntValue = 1;
	_GetCamera()->SetCameraMode( l_nIntValue );

	l_nIntValue = GetPrivateProfileInt( "UI", "ShowHint", TRUE, l_pFileName );
	m_pHintMgr->EnableRenderHint( l_nIntValue );

	// sound
	_GetOptionWindow()->SetSoundOption( CGUIOptionWindow::BGM, GetGlobalMusicVolume() );
	_GetOptionWindow()->SetSoundOption( CGUIOptionWindow::Effect_Snd, GetGlobalWavVolume() );	

	// gamma
	_GetOptionWindow()->SetGraphicOption( CGUIOptionWindow::Gamma, GetR3State()->mGamma );

	//d_light
	LONG l_dwTemp;
	l_dwTemp = GetR3State()->mDynamicLight;
	if( l_dwTemp )
	{
		if( l_dwTemp < 5 )
			l_dwTemp =1;
		if( l_dwTemp == 6 )
			l_dwTemp =2;
		if( l_dwTemp >= 10 )
			l_dwTemp =3;
	}
	_GetOptionWindow()->SetGraphicOption( CGUIOptionWindow::DynamicLighting, l_dwTemp );
	l_dwTemp = GetR3State()->mShadowDetail;
	if( l_dwTemp == 10 )
		l_dwTemp =2;
	if( l_dwTemp < 0 || l_dwTemp > 9 )
		l_dwTemp =3;
	_GetOptionWindow()->SetGraphicOption( CGUIOptionWindow::Shadow, l_dwTemp );
}

void
CGP_MainGame::SaveGameSetting( void )
{
	const char l_pFileName[] = "./DataTable/GameSetting.ini";

	char l_pBuf[32];

	sprintf( l_pBuf, "%d", _GetCharMgr()->IsEnableAutoVisualFieldAdjustment() );
	WritePrivateProfileString( "Performance", "AutoVisualFieldAdjustment", l_pBuf, l_pFileName );
	sprintf( l_pBuf, "%d", _GetCharMgr()->GetVisualFieldAdjustmentLevel() );
	WritePrivateProfileString( "Performance", "VisualFieldAdjustmentLevel", l_pBuf, l_pFileName );

	sprintf( l_pBuf, "%d", _GetCamera()->GetCameraMode() );
	WritePrivateProfileString( "Camera", "Mode", l_pBuf, l_pFileName );

	sprintf( l_pBuf, "%d", m_pHintMgr->IsRenderHint() );
	WritePrivateProfileString( "UI", "ShowHint", l_pBuf, l_pFileName );
	
	const char l_pFileName2[] = "./R3Engine.ini";

	sprintf( l_pBuf, "%.1f", GetGlobalMusicVolume() );
	WritePrivateProfileString( "Sound", "MusicVol", l_pBuf, l_pFileName2 );
	sprintf( l_pBuf, "%.1f", GetGlobalWavVolume() );
	WritePrivateProfileString( "Sound", "SoundVol", l_pBuf, l_pFileName2 );

	sprintf( l_pBuf, "%.1f", GetR3State()->mGamma );
	WritePrivateProfileString( "RenderState", "Gamma", l_pBuf, l_pFileName2 );

	DWORD l_dwTemp;
	l_dwTemp = GetR3State()->mDynamicLight;
	if( l_dwTemp )
	{
		if( l_dwTemp < 5 )
			l_dwTemp =1;
		if( l_dwTemp == 6 )
			l_dwTemp =2;
		if( l_dwTemp >= 10 )
			l_dwTemp =3;
	}
	sprintf( l_pBuf, "%d", l_dwTemp );
	WritePrivateProfileString( "RenderState", "DynamicLight", l_pBuf, l_pFileName2 );

	l_dwTemp = GetR3State()->mShadowDetail;
	if( l_dwTemp )
	{
		if( l_dwTemp < 5 )
			l_dwTemp =1;
		if( l_dwTemp == 6 )
			l_dwTemp =2;
		if( l_dwTemp >= 10 )
			l_dwTemp =3;
	}
	l_dwTemp = GetR3State()->mShadowDetail;
	if( l_dwTemp == 10 )
		l_dwTemp =2;
	if( l_dwTemp < 0 || l_dwTemp > 9 )
		l_dwTemp =3;

	sprintf( l_pBuf, "%d", l_dwTemp );
	WritePrivateProfileString( "RenderState", "ShadowDetail", l_pBuf, l_pFileName2 );
}

BOOL
CGP_MainGame::Destroy( void )
{
	if( !m_bIsCreated )
		return TRUE;

	//if( !CGameProgress::Destroy() )
	//	return FALSE;

	//if( !UnloadData() )
	//	return FALSE;

	SaveGameSetting();

	CGameProgress::Destroy();

	UnloadData();

	Destroy_UI();
	Destroy_Board();

	//--------------------------------------------------------------------------

	delete	m_pInputProcessMgr;
	delete	m_pUIHelpMgr;
	delete	m_pGameEventBoard;
	delete	m_pPartyBoard;
	delete	m_pHelpBoard;
	delete	m_pTargetMgr;
	delete	m_pNextActionMgr;

	g_pMainGame			= NULL;
	g_pGameEventBoard	= NULL;
	g_pPartyBoard		= NULL;
	g_pHelpBoard		= NULL;
	g_pTargetMgr		= NULL;
	g_pNextActionMgr	= NULL;

	//--------------------------------------------------------------------------

	m_bIsCreated = FALSE;

	return TRUE;
}

void
CGP_MainGame::Destroy_UI( void )
{
	// remove all window
	_GetUIScreen()->RemoveAll();

	g_pUiGaugeWindow			= NULL;
	g_pUiItemBeltWindow			= NULL;
	g_pUiSFBeltWindow			= NULL;
	g_pUiChatWindow				= NULL;
	g_pUiInventWindow			= NULL;
	g_pUiShopWindow				= NULL;
	g_pUiForceWindow			= NULL;
	g_pUiCharInfoWindow			= NULL;
	g_pUiFuncKeyWindow			= NULL;
	g_pUiChatInputWindow		= NULL;
	g_pUiSelectAttackPartWindow	= NULL;
	g_pUiMiningWindow			= NULL;
	g_pUiOreProcWindow			= NULL;
	g_pUiExchangeWindow			= NULL;
	g_pUiSummonsSelectWindow	= NULL;
	g_pUiSummonsControlWindow	= NULL;
	g_pUiNpcIntroduceWindow		= NULL;
	g_pUiItemWorkWindow			= NULL;
	g_pUiItemUpgradeWindow		= NULL;
	//g_pUiTargetInfoBar			= NULL;
	//g_pUiTargetInfoWindow		= NULL;
	g_pUiPartyWindow			= NULL;
	g_pUiSkillWindow			= NULL;
	//g_pUiSelectedCharMenu		= NULL;
	g_pUiMoveMapWindow			= NULL;
	g_pUiAppliedSFWindow		= NULL;
	g_pUiTradeWindow			= NULL;
	g_pUiRadarWindow			= NULL;
	g_pUiMinimapWindow			= NULL;
	g_pUiMapLoadingWindow		= NULL;
	g_pUiOptionWindow			= NULL;
}

void
CGP_MainGame::Destroy_Board( void )
{
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
#define CRT_BELLATOR_MALE			0x0
#define CRT_BELLATOR_FEMALE			0x1
#define CRT_CORA_MALE				0x2
#define CRT_CORA_FEMALE				0x3
#define CRT_ACCRETIA				0x4
*/
//static const float st_ADD_Y[5]={12,12,14,14,15};
static float st_ADD_Y[5]={11,11,13,13,16.5f};

//임시코드 1인칭뷰일경우 좀 높아야한다.
static float st_FPS_ADD_Y[5]={15.5,15.5,17,17,18};
CR3Profiler st_MainProfile;

HRESULT
CGP_MainGame::FrameMove( void )
{
	if( !m_bIsCompleteDataDownload )
	{
		return 0;
	}

	//--------------------------------------------------------------------------
	st_MainProfile.BeginR3Profiler();
	st_MainProfile.StartR3ProfilerName(",퀘스트,네트웍,FrameMove+캐릭시야처리");

	if( m_bIsMove_Cursor )
	{
		BOOL	l_bIsReach = FALSE;
		float	l_fSpeed = R3GetLoopTime() * 300.0f;

		float l_fDist = GetDist( m_vecCursorCurPos, m_vecCursorTarPos );
		if( ( l_fDist <= l_fSpeed ) || ( m_vecCursorTarPos[0] < 0 ) || ( m_vecCursorTarPos[2] < 0 ) )
		{
			l_bIsReach = TRUE;
			m_vecCursorCurPos[0] = m_vecCursorTarPos[0];
			m_vecCursorCurPos[2] = m_vecCursorTarPos[2];

			// button up
			if( m_dwMouseButton != 0xFF )
			{
				DWORD l_lParam = ( ( WORD )m_vecCursorCurPos[2] << 16 ) | ( WORD )m_vecCursorCurPos[0];
				SendMessage( _GetHWND(), MOUSE_BU_MSG[m_dwMouseButton], MOUSE_BUTTON[m_dwMouseButton], l_lParam );	
			}

			// x, y 중 -1이 있으면 위치 세팅을 시키지 않는다.
			if( !( ( m_vecCursorTarPos[0] < 0 ) || ( m_vecCursorTarPos[2] < 0 ) ) )
			{
				// cursor pos
				if(	_IsWindowed() )
					SetCursorPos( _GetWindowRect()->left + m_vecCursorCurPos[0], _GetWindowRect()->top + 21 + m_vecCursorCurPos[2] );
				else
					SetCursorPos( m_vecCursorCurPos[0], m_vecCursorCurPos[2] );
			}
		}
		else
		{
			m_vecCursorCurPos[0] += ( ( float )sin( m_fCursorAngle_PI ) * l_fSpeed );
			m_vecCursorCurPos[2] += ( ( float )cos( m_fCursorAngle_PI ) * l_fSpeed );
		}

		if( l_bIsReach )
		{
			m_bIsMove_Cursor = FALSE;

			// 퀘스트 처리
			QBI_CURSOR_DATA *	l_pQBI = NULL;
			int					l_nIndex = 0;
			while( l_pQBI = ( QBI_CURSOR_DATA * )_GetAvatar()->GetCurrentProcessingQBI( l_nIndex ) )
			{
				if( ( l_pQBI->m_byType == QBI_MOVE_CURSOR1 ) || ( l_pQBI->m_byType == QBI_MOVE_CURSOR2 ) ||
					( l_pQBI->m_byType == QBI_MOVE_CURSOR3 ) || ( l_pQBI->m_byType == QBI_MOVE_CURSOR2UI ) )
				{
					if( ( l_pQBI->nTarX == m_vecCursorTarPos[0] ) &&
						( l_pQBI->nTarY == m_vecCursorTarPos[2] ) )
						l_pQBI->m_byFlag |= QBIF_FINISH;
				}
			}
		}
		else
		{
			/*
			if( _IsWindowed() )
			{
				DWORD l_lParam = ( ( WORD )m_vecCursorCurPos[2] << 16 ) | ( WORD )m_vecCursorCurPos[0];
				SendMessage( _GetHWND(), WM_MOUSEMOVE, m_dwMouseButton, l_lParam );
			}
			else
			{
				_GetMainApp()->SetSystemMousePos( m_vecCursorCurPos[0], m_vecCursorCurPos[2] );
				_GetUIScreen()->GetMouse()->SetPos( m_vecCursorCurPos[0], m_vecCursorCurPos[2] );
			}*/
			DWORD l_lParam = ( ( WORD )m_vecCursorCurPos[2] << 16 ) | ( WORD )m_vecCursorCurPos[0];
			SendMessage( _GetHWND(), WM_MOUSEMOVE, MOUSE_BUTTON[m_dwMouseButton], l_lParam );
		}
	}

	//--------------------------------------------------------------------------
	_GetNetworkMgr()->FrameMove();

	//--------------------------------------------------------------------------
	// 캐릭터 매니져 - 렌더링 이외의 부분은 BeginScene 밖에서...

	// 데이터 초기화
	_GetCharMgr()->InitCharacter_EveryFrame();

	// 시야에 따른 처리
	_GetCharMgr()->CheckOutPositionOfVisualField();

	// 애니메이션
	st_MainProfile.StartR3ProfilerName("캐릭터FrameMove+Ani");
	_GetReturnEffectInfoList()->ResultProcessing();
	_GetCharMgr()->FrameMove();

	_GetCharMgr()->Animation();

	//--------------------------------------------------------------------------
	// 아이템
	st_MainProfile.StartR3ProfilerName("아이템FrameMove+Ani");
	if( !_GetAvatar()->GetObserverInfo()->m_bIsObserver )
		_GetGroundItemMgr()->CheckOutPositionOfVisualField();
	_GetGroundItemMgr()->Animation();

	//--------------------------------------------------------------------------
	// 보드
	_GetGBM()->FrameMove();

	//--------------------------------------------------------------------------
	// 카메라 & 캐릭터의 움직임 처리

	// 리얼 타임 무비 플레이
	if( m_bIsPlayMovie )
	{
#ifdef _DEVELOPER_
		Vector3f l_vecMovieCameraPos;
		RTMoiveGetCameraPos( l_vecMovieCameraPos );

		_GetNetworkMgr()->PositionMsg_MoveRequest_Simple_clzo( l_vecMovieCameraPos );

		// 무비가 끝까지 플레이 된 경우
		if( !IsRTMoviePlaying() )
		{
			RTMovieSetState( m_dwMovieFlag );
			RTMovieStartPlay( TRUE );
		}
#endif
	}
	// 애니메이션 옵져버
	else if( ( _GetAvatar()->GetObserverInfo()->m_bIsObserver ) &&
			 ( _GetAvatar()->GetObserverInfo()->m_byObserverMode == OMI_ANIMATION ) )
	{
		float l_matAniCamera[4][4];		// 애니메이션 카메라 매트릭스
		if( _GetCamera()->GetAniCamera()->PlayAniCamera( l_matAniCamera, 1.0f ) )
		{
			Vector3f l_vecAniCamera;
			R3SetCameraMatrix( l_vecAniCamera, l_matAniCamera );

			_GetNetworkMgr()->PositionMsg_MoveRequest_Simple_clzo( l_vecAniCamera );
		}
		else
		{
			DWORD l_dwAniCameraIndex = _GetCamera()->GetAniCamera()->GetCameraIndex( "C01-01" );
			_GetCamera()->GetAniCamera()->SetPrePlayCamera( l_dwAniCameraIndex, 0, 64, _CAM_FLAG_LOOP );

			float l_matAniCamera[4][4];		// 애니메이션 카메라 매트릭스
			if( _GetCamera()->GetAniCamera()->PlayAniCamera( l_matAniCamera, 1.0f ) )
			{
				Vector3f l_vecAniCamera;
				R3SetCameraMatrix( l_vecAniCamera, l_matAniCamera );

				_GetAvatar()->SetPosition( l_vecAniCamera );
			}
		}
	}

#ifdef _DEVELOPER_
	if( !m_bIsPlayMovie )
#endif
	{
		float l_fYAddY=0;
		// 카메라 위치 세팅
		_GetCamera()->SetPosition( _GetAvatar()->GetPosition() );

		if( _GetAvatar()->IsEqualUnitMountState( UMS_MOUNT ) )	//유닛 탐승이면높이를 더 올린다.
			l_fYAddY=15;

		if( _GetCamera()->GetCameraMode() == 2 )				// 종족별 카메라 바라보는 위치를 맞춘다 - by J.U.I
			_GetCamera()->SetAddY( st_FPS_ADD_Y[ _GetAvatar()->GetRace() ]+l_fYAddY );
		else
			_GetCamera()->SetAddY( st_ADD_Y[ _GetAvatar()->GetRace() ]+l_fYAddY );

		_GetCamera()->Animation();
		_GetCamera()->Render();
	}

	/*
	// 카메라를 아바타에 붙인다.
	if( !_GetAvatar()->GetObserverInfo()->m_bIsObserver || ( _GetAvatar()->GetObserverInfo()->m_byObserverMode == OMI_NORMAL ) )
	{
		_GetCamera()->SetPosition( _GetAvatar()->GetPosition() );

		//종족별 카메라 바라보는 위치를 맞춘다.	JUI
		if( _GetCamera()->GetCameraMode() == 2 )
			_GetCamera()->SetAddY(st_FPS_ADD_Y[_GetAvatar()->GetRace()]);
		else
			_GetCamera()->SetAddY(st_ADD_Y[_GetAvatar()->GetRace()]);
		// camera rendering
		_GetCamera()->Render();
	}
	// 옵져버 & FLY 모드일 경우에는 카메라를 직접 움직여준다.
	else if( _GetAvatar()->GetObserverInfo()->m_bIsObserver && ( _GetAvatar()->GetObserverInfo()->m_byObserverMode == OMI_FLY ) )
	{
		static DWORD	l_dwPrevMoveTime = 0;
		static Vector3f	l_vecPrevCamPos = { 0, 0, 0 };

		Vector3f l_vecPos;
		//R3FlyMove( CHAR_SPEED_FLY, l_vecPos );								// 카메라의 위치 계산
		R3FlyMove( CHAR_SPEED_DEFAULT * 5.0f, l_vecPos );						// 카메라의 위치 계산
		_GetCamera()->SetPosition( l_vecPos[0], l_vecPos[1], l_vecPos[2] );		// 카메라의 위치 설정
		_GetAvatar()->SetPosition( l_vecPos[0], l_vecPos[1], l_vecPos[2] );		// 아바타의 위치 설정

		if( timeGetTime() - l_dwPrevMoveTime > 500 )							// 0.5 초마다 한 번씩 서버에 메세지를 보냄
		{
			_GetNetworkMgr()->ObserverMsg_MoveRequest_clzo( l_vecPrevCamPos );

			_GetCamera()->GetPosition( l_vecPos );
			l_vecPrevCamPos[0] = l_vecPos[0];
			l_vecPrevCamPos[1] = l_vecPos[1];
			l_vecPrevCamPos[2] = l_vecPos[2];

			l_dwPrevMoveTime = timeGetTime();
		}
	}
	// 옵져버 & 애니메이션 모드일 경우...
	else if( _GetAvatar()->IsObserver() && ( _GetAvatar()->GetObserverMode() == OMI_ANIMATION ) && g_bPlayCamera )
	{
		static DWORD	l_dwPrevMoveTime = 0;
		static Vector3f	l_vecPrevCamPos = { 0, 0, 0 };

		Vector3f l_vecPos;
		R3AniCameraMove( l_vecPos );
		_GetCamera()->SetPosition( l_vecPos[0], l_vecPos[1], l_vecPos[2] );		// 카메라의 위치 설정
		_GetAvatar()->SetPosition( l_vecPos[0], l_vecPos[1], l_vecPos[2] );	// 아바타의 위치 설정

		//g_pLand->_GetLevel()->SetViewMatrix(R3MoveGetViewMatrix());

		//_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( _GetAvatar()->GetPosition() );

		if( timeGetTime() - l_dwPrevMoveTime > 500 )// 0.5 초마다 한 번씩 서버에 메세지를 보냄
		{
			_GetNetworkMgr()->ObserverMsg_MoveRequest_clzo( l_vecPrevCamPos );

			_GetCamera()->GetPosition( l_vecPos );
			l_vecPrevCamPos[0] = l_vecPos[0];
			l_vecPrevCamPos[1] = l_vecPos[1];
			l_vecPrevCamPos[2] = l_vecPos[2];

			l_dwPrevMoveTime = timeGetTime();
		}
	}*/

	//--------------------------------------------------------------------------

	/*if( m_cCamera.GetIsChaseView() )
	{
		Vector3f	l_vecTarget;
		float		l_fYAngle;

		_GetAvatar()->GetCurPath( l_vecTarget );
		l_fYAngle = GetYAngle( _GetAvatar()->GetPosition(), l_vecTarget );
		m_cCamera.SetPitch( l_fYAngle );
	}*/


	EFUTIL_SpecialCamera();			//죽었을때,전투던젼카메라처럼 특수카메라제어시 

	//--------------------------------------------------------------------------

	// 퀘스트 처리
	QUEST_INFO * l_pTutorial = _GetAvatar()->GetTutorialInfo();
	if( l_pTutorial )
	{
		//static	Vector3f l_fPrevCameraRot	= { _GetCamera()->GetR3Camera()->GetTh(), _GetCamera()->GetR3Camera()->GetPh(), _GetCamera()->GetR3Camera()->GetDist() };
		//		Vector3f l_fCurCameraRot	= { _GetCamera()->GetR3Camera()->GetTh(), _GetCamera()->GetR3Camera()->GetPh(), _GetCamera()->GetR3Camera()->GetDist() };
		static	Vector3f l_fPrevCameraRot	= { _GetCamera()->GetYaw(), _GetCamera()->GetPitch(), _GetCamera()->GetDistance() };
				Vector3f l_fCurCameraRot	= { _GetCamera()->GetYaw(), _GetCamera()->GetPitch(), _GetCamera()->GetDistance() };

		for( int i = 0; i < 3; ++i )
		{
			if( l_pTutorial->QSV.m_fCameraRotate[i] > 0 )
				l_pTutorial->QSV.m_fCameraRotate[i] -= fabs( l_fCurCameraRot[i] - l_fPrevCameraRot[i] );

			l_fPrevCameraRot[i] = l_fCurCameraRot[i];
		}
	}

	//--------------------------------------------------------------------------
	st_MainProfile.StartR3ProfilerName("맵 이펙트FrameMove");

	// 맵 렌더링전에 준비할것들...
	if( _GetLevel()->IsLoadedBsp() )
	{
		_GetLevel()->FrameMove();

		// 물속인지 확인.
		if( _GetCamera() )
		{
			Vector3f ch_pos,ch_point;
			R3Camera *cam = _GetCamera()->GetR3Camera();
			if( cam )
				cam->GetPos(ch_point);
			if( _GetAvatar() )
				_GetAvatar()->GetPosition(ch_pos);
			ch_pos[1] += 20;	//머리끝높이에 맞춘다.
			DWORD FSEstate=GetStateFullScreenEffect();
			if( _GetLevel()->mBsp->IsInWater(ch_pos,ch_point) )	//물속 효과 
				SetStateFullScreenEffect(_FSE_IN_WATER);
			else
			{
				if( FSEstate & _FSE_IN_WATER )
					SetStateFullScreenEffect(FALSE);
			}
		}
	}
	

	::FrameMoveEffect();	// 매루프마다...이펙트 관리. by J.U.I
	::EFUtil_MusicLoop();	// 게임 음악봐가면서 틀어주느곳 by J.U.I

	st_MainProfile.StartR3ProfilerName("UI FrameMove");

	_GetUIScreen()->Update();

	//--------------------------------------------------------------------------

	m_pGameEventBoard->FrameMove();
	m_pPartyBoard->FrameMove();
	m_pHelpBoard->FrameMove();
	m_pTargetMgr->FrameMove();

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

HRESULT
CGP_MainGame::Render( void )
{
	if( !m_bIsCompleteDataDownload )
		return S_OK;

	//--------------------------------------------------------------------------
	// scene rendering
	if( R3BeginScene() )
	{
		R3ClearFrameBuffer();
		st_MainProfile.StartR3ProfilerName("맵 Render");
		// Land
		Vector3f l_vecCamPos;
		_GetCamera()->GetPosition( l_vecCamPos );
		
		_GetLand()->Render( l_vecCamPos );
		if( m_bIsRenderCollisionMap )
			_GetLevel()->mBsp->DrawCollisionPoly();	// 임시코드 - 충돌맵 렌더링

		
		// Second Render Item
		_GetItemEffectRenderer()->Clear();

		st_MainProfile.StartR3ProfilerName("캐릭터 Render");
		// Character
		_GetCharMgr()->Render();

		// Item
		_GetGroundItemMgr()->Render();

		st_MainProfile.StartR3ProfilerName("Shadow Render");

		EFUtil_DrawShadowList(GetR3State()->mShadowDetail);	//Push 해놓은 쉐도우 리스트를 필요에 따라 비운다.cnt=-1이면 다뿌린다.

		st_MainProfile.StartR3ProfilerName("맵알파,알파 엔티티 Render");
		
		// Land ( Alpha ) - 알파 렌더링은 제일 마지막에 처리
		_GetLand()->RenderAlpha( l_vecCamPos );

		st_MainProfile.StartR3ProfilerName("이펙트 Render");
		//-----장언일 추가 코드 ----//
		::RenderEffect();	//매루프마다... 이펙트 뿌려주기.

		st_MainProfile.StartR3ProfilerName("캐릭알파 기타 Render");
		// Alpha Mesh - by MAYUN
		g_AMeshManager.DrawAlpahMesh();

		// Second Render Item
		_GetItemEffectRenderer()->Render();

		m_pTargetMgr->Render();


		FullScreenEffect();	//모션 블로우나 풀화면 이펙트처리.

		// Avatar's Path Line
		//DrawAvatarPath();

		st_MainProfile.StartR3ProfilerName("UI,flip Render");

		// character & item text
		if( !m_bIsHideUI )
			_GetCharMgr()->RenderText();

		// Hint & Info
		if( !m_bIsHideUI )
		{
			_GetCharMgr()->RenderInformation();
			m_pHintMgr->RenderHint();
			_GetAvatar()->GetTutorialInfo()->RenderCurMsg();
		}

		// Etc
		DisplayInformation( 20, 50 );

		if( m_bIsDisplayHelpText )
			DisplayHelpText();

		::DrawFadeScreen();	//페이드 관련 이펙트도 있음.
		EFUtil_BattleDungeonString();
		// UI
		_GetUIScreen()->Draw();
	}

	R3EndScene();

	_GetLevel()->HearMapSound();	//맵사운드를 가동한다...

	st_MainProfile.EndR3Profiler();

	DrawCheatDisplay();		//프로파일 디스플레이..
	g_fFPS = GetFPS();

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

LRESULT
CGP_MainGame::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	const	int			l_nMaxMeshNum[5] = { 2, 3, 3, 2, 2 };
			DWORD		l_dwTemp = 0;
	static	bool		l_bWriteNetLog = true;

	static	DWORD		l_dwPrevChangeCombatTime	= timeGetTime();
	static	DWORD		l_dwPrevChangeMoveTime		= timeGetTime();
			DWORD		l_dwCurTime					= timeGetTime();

	static	int			l_nForceID = 0;
	static	int			l_nSkillID = 0;
	static	int			l_nMaxForce;

	CPath				l_pathTest;//테스트코드

	switch ( uMsg ) 
	{
		case WM_ACTIVATEAPP :
			if( !wParam )
			{
				if( _GetAvatar()->GetAction() == CAI_MOVE_MOVING )
					_GetNetworkMgr()->PositionMsg_PlayerStop_clzo();
			}
			break;

		case WM_KEYDOWN :
			if( !IsEnableInput() )
				break;

			if( wParam >= VK_F1 && wParam <= VK_F4 )
			{
				_GetItemBeltWindow()->UseItem( wParam % VK_F1 );
			}
			else if( wParam >= VK_F5 && wParam <= VK_F12 )
			{
				_GetSFBeltWindow()->UseItem( wParam % VK_F5 );
			}
			else if( wParam == VK_PROCESSKEY )
			{
				if( m_bIsChatMode )
				{
					if( gInputMgr.IsKoreanMode() )
						m_uiChatWindow.SetLangMode( CGUIChatWindow::Korean );
					else
						m_uiChatWindow.SetLangMode( CGUIChatWindow::English );
				}
			}
			break;

		case WM_KEYUP :
			/*
			if( !IsEnableInput() )
				break;

			{
				DWORD l_dwScanCode = ( lParam & 0x00FF0000 ) >> 16;

				if( ( l_dwScanCode == CMK_UP ) || ( l_dwScanCode == CMK_DOWN ) ||
					( l_dwScanCode == CMK_LEFT ) || ( l_dwScanCode == CMK_RIGHT ) )
				{
					if( !( ( GetKeyState( VK_UP ) & 0x80000000 ) || ( GetKeyState( VK_DOWN ) & 0x80000000 ) ||
						   ( GetKeyState( VK_LEFT ) & 0x80000000 ) || ( GetKeyState( VK_RIGHT ) & 0x80000000 ) ) )
						_GetAvatar()->RequestStopByKeyboard();
				}
				else if( l_dwScanCode == CMK_SHIFT )
				{
					if( m_bIsFreeTargetCombatMode )
						_GetUIScreen()->GetMouse()->Show( FALSE );
				}
			}*/
			break;

		case WM_MOUSEWHEEL:
			if( !IsEnableInput() )
				break;

			break;

		case WM_SYSKEYDOWN :		// System 키는 윈도우즈로 들어가지 않게 막는다.
			if( !IsEnableInput() )
				break;

			// F10번 처리
			if( ( ( lParam & 0x00FF0000 ) >> 16 ) == 0x44 )
				_GetSFBeltWindow()->UseItem( 5 );

			_GetUIScreen()->MsgProc( hWnd, uMsg, wParam, lParam );
			return 0xFFFFFFFF;

		case WM_SYSKEYUP :
		case WM_MOUSEMOVE :
		case WM_LBUTTONDOWN :
		case WM_LBUTTONUP :
		case WM_RBUTTONDOWN :
		case WM_RBUTTONUP :
			if( !IsEnableInput() )
				break;
			break;

		case WM_OPTION_TOGGLE_HINT :
			if( m_pHintMgr )
				m_pHintMgr->EnableRenderHint( wParam );
			break;

		case WM_OPTION_TOGGLE_AUTO_VF_ADJUST :
			_GetCharMgr()->EnableAutoVisualFieldAdjustment( wParam );
			break;

		case WM_OPTION_CHANGE_VF_DIST :
			_GetCharMgr()->SetVisualFieldAdjustmentLevel( wParam );
			break;

		case WM_OPTION_CHANGE_CAMERA_MODE :
			_GetCamera()->SetCameraMode( wParam );
			break;

		case WM_OPTION_ADJUST_EFFECT_SOUND	:
			SetGlobalWavVolume( _GetOptionWindow()->GetSoundOption( CGUIOptionWindow::Effect_Snd ) );
			break;
		
		case WM_OPTION_ADJUST_BGM_SOUND	:
			SetGlobalMusicVolume( _GetOptionWindow()->GetSoundOption( CGUIOptionWindow::BGM ) );
			break;

		case WM_OPTION_ADJUST_GAMMA	:
			SetGamma( 0.8 + (float)wParam / 10 );
			break;

		case WM_OPTION_ADJUST_DYNAMIC_LIGHTING	:
			//  wParam 0 ~ 3
			if( wParam == 0 )
				GetR3State()->mDynamicLight = 0;
			if( wParam == 1 )
				GetR3State()->mDynamicLight = 3;
			if( wParam == 2 )
				GetR3State()->mDynamicLight = 6;
			if( wParam == 3 )
				GetR3State()->mDynamicLight = 10;
			break;

		case WM_OPTION_ADJUST_SHADOW	:
			//  wParam 0 ~ 3
			GetR3State()->mShadowDetail = wParam;
			if( wParam == 2 )
				GetR3State()->mShadowDetail = 10;
			if( wParam == 3 )
				GetR3State()->mShadowDetail = -1;
			break;

		case WM_OPTION_DETAIL_TEXTURE	:
			GetR3State()->mDetailTexture = wParam;
			_GetOptionWindow()->SwitchOn( CGUIOptionWindow::DetailTexture, wParam );
			break;
	}

	//--------------------------------------------------------------------------

	if( m_pInputProcessMgr )
		m_pInputProcessMgr->MsgProc( hWnd, uMsg, wParam, lParam );

	_GetCharMgr()->MsgProc( hWnd, uMsg, wParam, lParam );

	_GetGBM()->MsgProc( hWnd, uMsg, wParam, lParam );

	if( m_pGameEventBoard )
		m_pGameEventBoard->MsgProc( hWnd, uMsg, wParam, lParam );
	if( m_pPartyBoard )
		m_pPartyBoard->MsgProc( hWnd, uMsg, wParam, lParam );
	if( m_pHelpBoard )
		m_pHelpBoard->MsgProc( hWnd, uMsg, wParam, lParam );

	// User Interface
	if( IsEnableInput() )
		return _GetUIScreen()->MsgProc( hWnd, uMsg, wParam, lParam );

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CGP_MainGame::LoadData( void )
{
	if( m_bIsLoadedData )
		return TRUE;

	_GetLand()->LoadLevel();

	_GetAvatar()->GetTutorialInfo()->LoadQuestInfoFile( "./DataTable/Tutorial.scp" );

	m_pInputProcessMgr->LoadData();
	m_pInputProcessMgr->SetMenuHotKeyString();

	m_pHelpBoard->LoadData();
	m_pHintMgr->LoadData();
	m_pUIHelpMgr->LoadData();

	CreateEffect();

	m_bIsLoadedData = TRUE;

	return TRUE;
}

BOOL
CGP_MainGame::UnloadData( void )
{
	if( !m_bIsLoadedData ) return TRUE;

	_GetLand()->UnloadLevel();

	// 캐릭터 클리어 => 모두 캐쉬로 이동
	_GetCharMgr()->ClearCharacter( CTI_PLAYER );
	_GetCharMgr()->ClearCharacter( CTI_NPC );
	_GetCharMgr()->ClearCharacter( CTI_MONSTER );
	_GetCharMgr()->ClearCharacter( CTI_ANIMUS );
	_GetCharMgr()->ClearCharacter( CTI_GUARD_TOWER );

	// 이펙트 큐
	_GetEffectInfoList()->Clear();
	_GetReturnEffectInfoList()->Clear();

	// ground item
	_GetGroundItemMgr()->Clear();
	DestroyEffect();

	UI_UnloadSprite();

	m_bIsLoadedData = FALSE;

	return TRUE;
}

void
CGP_MainGame::UI_LoadSprite( void )
{
	_GetUISpriteMgr()->UnloadSpriteAll();

	_GetUISpriteMgr()->LoadSprite( SP_ID_MOUSE );
	_GetUISpriteMgr()->LoadSprite( SP_ID_COMMON );
	_GetUISpriteMgr()->LoadSprite( SP_ID_GAUGE_EXP );
	_GetUISpriteMgr()->LoadSprite( SP_ID_BELT );
	_GetUISpriteMgr()->LoadSprite( SP_ID_INVENTORY );
	_GetUISpriteMgr()->LoadSprite( SP_ID_ITEM );
	_GetUISpriteMgr()->LoadSprite( SP_ID_CHAT );
	_GetUISpriteMgr()->LoadSprite( SP_ID_SHOP );
	_GetUISpriteMgr()->LoadSprite( SP_ID_MINING );
	_GetUISpriteMgr()->LoadSprite( SP_ID_ORE_PROC );
	_GetUISpriteMgr()->LoadSprite( SP_ID_EXCHANGE );	
	_GetUISpriteMgr()->LoadSprite( SP_ID_RECALL );
	_GetUISpriteMgr()->LoadSprite( SP_ID_ITEM_MAKE );
	_GetUISpriteMgr()->LoadSprite( SP_ID_ITEM_UPGRADE );
	_GetUISpriteMgr()->LoadSprite( SP_ID_PARTY );
	_GetUISpriteMgr()->LoadSprite( SP_ID_TRADE );
	_GetUISpriteMgr()->LoadSprite( SP_ID_MAPLOADING );
	_GetUISpriteMgr()->LoadSprite( SP_ID_DUNGEON );
	_GetUISpriteMgr()->LoadSprite( SP_ID_UNIT );
	_GetUISpriteMgr()->LoadSprite( SP_ID_UNIT_PARTS );
	_GetUISpriteMgr()->LoadSprite( SP_ID_CLASS );
	if( _GetMinimapWindow() && _GetMinimapWindow()->GetMiniMapIndex() != 0xFF )
		_GetUISpriteMgr()->LoadSprite( SP_ID_MINIMAP, _GetLand()->GetMapInfo( _GetMinimapWindow()->GetMiniMapIndex() )->pMinimapFileName );

	_GetUIScreen()->RestoreSprite();
}

void
CGP_MainGame::UI_UnloadSprite( void )
{
	_GetUISpriteMgr()->UnloadSpriteAll();
}

HRESULT
CGP_MainGame::RestoreDeviceObjects( void )
{
	_GetLand()->LoadLevel();
	UI_LoadSprite();

	return 0;
}

HRESULT
CGP_MainGame::InvalidateDeviceObjects( void )
{
	_GetLand()->UnloadLevel();
	UI_UnloadSprite();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CGP_MainGame::RequestLogout( void )
{
	if( _GetPlayerTradeBoard()->IsTrading() )
	{
		_SetSystemMsg( "플레이어와 트레이드 중에는 로그아웃 할 수 없습니다." );
		return;
	}

	_GetGBM()->ReturnHoverObject();

	if( !m_bIsRequestLogout )
	{
		_GetNetworkMgr()->InitMsg_ExitSaveDataRequest_clzo();
		m_dwRequestDataUploadTime	= timeGetTime();
		m_bIsRequestLogout			= TRUE;

		_GetMainApp()->SetRequestLogout( TRUE );
	}
}

DWORD
CGP_MainGame::EventProcChatWindow( void * pi_pObject )
{
	if( !pi_pObject )
		return FALSE;

	CGUIInput *	l_pInput = (CGUIInput * )pi_pObject;
	
	if( strlen( l_pInput->GetText() ) > 0 )
	{
		// 채팅명령어가 포함되어있으면 명령어를 제외한 나머지 문자열만 필터링한다.
		char * l_pFilteringMsg;
		l_pFilteringMsg = l_pInput->GetText();		
		if( l_pFilteringMsg[0] == '/' )
		{
			l_pFilteringMsg = strstr( l_pFilteringMsg, " " );
		}
		

		// 욕 필터링
		if( _IsAbuseStr( l_pFilteringMsg ) )
		{
			_SetSystemMsg( "바른말 고운말을 사용합시다." );			
		}
		// 광고 필터링
		else if( _IsAdvStr( l_pFilteringMsg ) )
		{
			_SetSystemMsg( "광고성 문구나 비속한 문장을 포함였습니다." );	
		}
		else
		{
			// send chat message
			_GetAvatar()->GetChatInfo()->SendMessage( l_pInput->GetText() );
		}

		l_pInput->Clear();

		l_pInput->SetText( _GetAvatar()->GetChatInfo()->m_pMessageType );
		l_pInput->SetCaretPos( strlen( _GetAvatar()->GetChatInfo()->m_pMessageType ) );
	}

	return TRUE;
}

void
CGP_MainGame::ToggleHint( void )
{
	if( m_pHintMgr )
		m_pHintMgr->EnableRenderHint( !m_pHintMgr->IsRenderHint() );
}

void
CGP_MainGame::EnableHint( BOOL pi_bFlag )
{
	if( m_pHintMgr )
		m_pHintMgr->EnableRenderHint( pi_bFlag );
}

BOOL
CGP_MainGame::IsEnableHint( void )
{
	if( m_pHintMgr )
		return m_pHintMgr->IsRenderHint();

	return FALSE;
}