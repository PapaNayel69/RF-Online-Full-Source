#include "../GameObject/Manager/CCharacterMgr.h"
#include "../GameObject/Manager/CGroundItemManager.h"
#include "../GameObject/Manager/CTargetMgr.h"
#include "../Character/Avatar/CAvatar_Quest.h"

#include "../CLand.h"
#include "../CCamera.h"
#include "../DataTable/CResourceDataMgr.h"

#include "CGP_MainGame.h"
#include "../MainApp.h"

#ifdef _DEVELOPER_
#include "RTMovie.h"
#endif

#ifdef _NEW_UI_
#include "../Item/CItemData.h"
#include "../GameObject/Board/Common/CBoardManager.h"
#endif

#include "../GameObject/Board/General/CPartyBoard.h"

#include "../Network/NetMsgProcessor/Dungeon/CNetMsgProcessor_Dungeon.h"
//#include "../Network/NetMsgProcessor/Dungeon/CNetMsgProcessor_Dungeon_Dev.h"
#include "../Network/NetMsgProcessor/Unit/CNetMsgProcessor_Unit.h"

#include "CInputProcessMgr.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum KEY_MAP
{
	KM_WINDOW_INVENTORY, KM_WINDOW_FORCE, KM_WINDOW_PARTY, KM_ANIMUS_WINDOW,
	KM_WINDOW_SELECT_ATTACK_PART, KM_WINDOW_MINIMAP, KM_WINDOW_REBORN, KM_WINDOW_OPTION,
	KM_WINDOW_SKILL, KM_WINDOW_AVATAR_INFO,

	KM_COMBAT_CHANGE_COMBAT_MODE, KM_COMBAT_CHANGE_MOVE_MODE, KM_COMBAT_CHANGE_AUTO_ATTACK_MODE,
	KM_COMBAT_ATTACK, KM_COMBAT_CHANGE_FREE_TARGET_MODE,

	KM_BEHAVIOR_ITEM_LOOTING, KM_BEHAVIOR_CHANGE_TARGET_TO_LASTEST, KM_BEHAVIOR_CANCEL_TARGET,
	KM_MOVE_LEFT, KM_MOVE_UP, KM_MOVE_RIGHT, KM_MOVE_DOWN,

	KM_CHAT_PREV_SEND_CHAT_MSG, KM_CHAT_PREV_WHISPER_CHAR_NAME,

	KM_GAME_QUIT, KM_GAME_LOGOUT, KM_GAME_CHANGE_CHATING_MODE, KM_GAME_SCREENSHOT,
	KM_GAME_CHANGE_AUTO_MOUSE_MODE, KM_GAME_CHANGE_AUTO_TARGET_MODE, KM_GAME_RENDER_SHADOW,
	KM_GAME_HIDE_UI, KM_GAME_CHANGE_TO_OBSERVER, KM_GAME_CHANGE_OBSERVER_MODE,
	KM_GAME_PLAY_REALTIME_MOVIE, KM_GAME_CHANGE_REALTIME_WIDE_MODE, KM_GAME_CHANGE_REALTIME_CAMERA_MODE,
};

static const char KEY_MAP_NAME[MAX_KEY_MAP][32] =
{
	"INVENTORY", "FORCE_WINDOW", "PARTY_WINDOW", "ANIMUS_WINDOW",
	"ATTACK_PART_WINDOW", "MINIMAP", "REBORN_WINDOW", "OPTION_WINDOW",
	"SKILL_WINDOW", "AVATAR_INFO_WINDOW",

	"COMBAT_MODE", "MOVE_MODE", "AUTO_ATTACK_MODE",
	"ATTACK", "FREE_TARGET_MODE",

	"ITEM_LOOTING", "CHANGE_TARGET_TO_LASTEST", "CANCEL_TARGET",
	"MOVE_LEFT", "MOVE_UP", "MOVE_RIGHT", "MOVE_DOWN",

	"PREV_SEND_MSG", "PREV_WHISPER_CHAR",

	"QUIT", "LOGOUT", "CHATING_MODE", "SCREENSHOT",
	"AUTO_MOUSE_MODE", "AUTO_TARGET_MODE", "RENDER_SHADOW",
	"HIDE_UI", "CHANGE_TO_OBSERVER", "CHANGE_OBSERVER_MODE",
	"PLAY_MOVIE", "MOVIE_WIDE_MODE", "MOVIE_CAMERA_MODE"
};

extern int  g_nOpenedDlgBoxCount;
extern int	g_nOpenedTradeWin;

#ifdef _DEVELOPER_
extern BOOL	g_bWriteTestLog;
#endif

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CInputProcessMgr::CInputProcessMgr()
{
	ZeroMemory( this, sizeof( CInputProcessMgr ) );

	for( int i = 0; i < MAX_KEY_MAP; ++i )
	{
		m_listKey[i][0]	= ID_INVALID;
		m_listKey[i][1]	= ID_INVALID;
	}
}

CInputProcessMgr::~CInputProcessMgr()
{
}

BOOL
CInputProcessMgr::SplitOrgKeyString( char * po_pSystemKeyString,
									 char * po_pNormalKeyString, char * pi_pOrgString )
{
	char l_pBuf[32];
	ZeroMemory( l_pBuf, 32 );

	for( int i = 0, cnt = 0; i < strlen( pi_pOrgString ); ++i )
	{
		if( pi_pOrgString[i] == '+' )
		{
			memcpy( po_pSystemKeyString, l_pBuf, strlen( l_pBuf ) );
			ZeroMemory( l_pBuf, 32 );
			cnt = 0;
			continue;
		}
		else
		{
			l_pBuf[cnt] = pi_pOrgString[i];
			 ++cnt;
		}
	}

	memcpy( po_pNormalKeyString, l_pBuf, strlen( l_pBuf ) );

	return TRUE;
}

BOOL
CInputProcessMgr::GetHotKeyFromSplitedStringKey( DWORD * po_pKeyMap,
												 char * pi_pSystemKeyString, char * pi_pNormalKeyString )
{
	static	const	int		MAX_SYSTEM_KEY	= 3;
	static	const	int		MAX_NORMAL_KEY	= 10 + 26 + 8;

	static	const	char	SYSTEM_KEY_STRING[MAX_SYSTEM_KEY][12] =
							{ "SHIFT", "CTRL", "ALT" };
	static	const	int		SYSTEM_KEY_VALUE[MAX_SYSTEM_KEY] =
							{ VK_SHIFT, VK_CONTROL, VK_MENU };

	static	const	char	NORMAL_KEY_STRING[MAX_NORMAL_KEY][12] =
							{
								"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
								"A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
								"K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
								"U", "V", "W", "X", "Y", "Z",
								"ESC", "TAB", "LEFT", "UP", "RIGHT", "DOWN", "SPACE",
								"PRINT"
							};
	static	const	int		NORMAL_KEY_VALUE[MAX_NORMAL_KEY] =
							{
								VK_0, VK_1, VK_2, VK_3, VK_4, VK_5, VK_6, VK_7, VK_8, VK_9,
								VK_A, VK_B, VK_C, VK_D, VK_E, VK_F, VK_G, VK_H, VK_I, VK_J,
								VK_K, VK_L, VK_M, VK_N, VK_O, VK_P, VK_Q, VK_R, VK_S, VK_T,
								VK_U, VK_V, VK_W, VK_X, VK_Y, VK_Z,
								VK_ESCAPE, VK_TAB, VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN, VK_SPACE,
								VK_PRINT
							};

	int		i;
	BOOL	l_bHaveKey = FALSE;

	for( i = 0; i < MAX_SYSTEM_KEY; ++i )
	{
		if( strcmp( _strupr( pi_pSystemKeyString ), SYSTEM_KEY_STRING[i] ) == 0 )
		{
			po_pKeyMap[0] = SYSTEM_KEY_VALUE[i];
			break;
		}
	}

	for( i = 0; i < MAX_NORMAL_KEY; ++i )
	{
		if( strcmp( _strupr( pi_pNormalKeyString ), NORMAL_KEY_STRING[i] ) == 0 )
		{
			po_pKeyMap[1] = NORMAL_KEY_VALUE[i];
			l_bHaveKey = TRUE;
			break;
		}
	}

	return l_bHaveKey;
}

BOOL
CInputProcessMgr::GetHotKeyFromString( DWORD * po_pKeyMap, char * pi_pOrgString )
{
	char l_pSystemKeyString[32], l_pNormalKeyString[32];

	ZeroMemory( l_pSystemKeyString, 32 );
	ZeroMemory( l_pNormalKeyString, 32 );

	SplitOrgKeyString( l_pSystemKeyString, l_pNormalKeyString, pi_pOrgString );

	return GetHotKeyFromSplitedStringKey( po_pKeyMap, l_pSystemKeyString, l_pNormalKeyString );
}

BOOL
CInputProcessMgr::LoadData( void )
{
	static	const	int		MAX_SECTION_NUM				= 5;
	static	const	int		MAX_ITEM_NUM_PER_SECTION	= 3;
	static	const	int		MAX_ITEM_NUM[MAX_SECTION_NUM] = { 10, 5, 7, 2, 13 };

	static	const	char	FILE_NAME[128] = { "./DataTable/KeyMap.dat" };
	static	const	char	SECTION_NAME[MAX_SECTION_NUM][32] =
							{ "WINDOW", "COMBAT", "BEHAVIOR", "CHAT", "GAME" };

	char l_pOrgString[32];
	m_dwTotalKeyNum = 0;

	for( int i = 0, l_nKeyCount = 0; i < MAX_SECTION_NUM; ++i )
	{
		for( int j = 0; j < MAX_ITEM_NUM[i]; ++j, ++l_nKeyCount )
		{
			GetPrivateProfileString( SECTION_NAME[i], KEY_MAP_NAME[l_nKeyCount], "NULL", l_pOrgString, 32, FILE_NAME );
			if( GetHotKeyFromString( m_listKey[l_nKeyCount], l_pOrgString ) )
				++m_dwTotalKeyNum;
		}
	}

	return TRUE;
}

void
CInputProcessMgr::SetMenuHotKeyString( void )
{
	if( !_GetFuncKeyWindow() )
		return;

	const int	HOT_KEY_LENGTH = 64;
	char		l_pHotKeyString[HOT_KEY_LENGTH];
	ZeroMemory( l_pHotKeyString, HOT_KEY_LENGTH );

	IntHotKeyToStrHotKey( l_pHotKeyString, m_listKey[KM_COMBAT_CHANGE_COMBAT_MODE] );
	_GetFuncKeyWindow()->SetHotKeyStr( HOTKEY_ID_ACTION, l_pHotKeyString );
	IntHotKeyToStrHotKey( l_pHotKeyString, m_listKey[KM_COMBAT_CHANGE_MOVE_MODE] );
	_GetFuncKeyWindow()->SetHotKeyStr( HOTKEY_ID_MOVE, l_pHotKeyString );
	IntHotKeyToStrHotKey( l_pHotKeyString, m_listKey[KM_COMBAT_CHANGE_AUTO_ATTACK_MODE] );
	_GetFuncKeyWindow()->SetHotKeyStr( HOTKEY_ID_ATTACK, l_pHotKeyString );
	IntHotKeyToStrHotKey( l_pHotKeyString, m_listKey[KM_WINDOW_INVENTORY] );
	_GetFuncKeyWindow()->SetHotKeyStr( HOTKEY_ID_INVENTORY, l_pHotKeyString );
	IntHotKeyToStrHotKey( l_pHotKeyString, m_listKey[KM_WINDOW_AVATAR_INFO] );
	_GetFuncKeyWindow()->SetHotKeyStr( HOTKEY_ID_CHARINFO, l_pHotKeyString );
	IntHotKeyToStrHotKey( l_pHotKeyString, m_listKey[KM_WINDOW_SKILL] );
	_GetFuncKeyWindow()->SetHotKeyStr( HOTKEY_ID_SKILL, l_pHotKeyString );
	IntHotKeyToStrHotKey( l_pHotKeyString, m_listKey[KM_WINDOW_FORCE] );
	_GetFuncKeyWindow()->SetHotKeyStr( HOTKEY_ID_FORCE, l_pHotKeyString );
	IntHotKeyToStrHotKey( l_pHotKeyString, m_listKey[KM_WINDOW_PARTY] );
	_GetFuncKeyWindow()->SetHotKeyStr( HOTKEY_ID_GROUP, l_pHotKeyString );
	_GetFuncKeyWindow()->SetHotKeyStr( HOTKEY_ID_QUEST, NULL );
	IntHotKeyToStrHotKey( l_pHotKeyString, m_listKey[KM_WINDOW_OPTION] );
	_GetFuncKeyWindow()->SetHotKeyStr( HOTKEY_ID_OPTION, l_pHotKeyString );
	IntHotKeyToStrHotKey( l_pHotKeyString, m_listKey[KM_ANIMUS_WINDOW] );
	_GetFuncKeyWindow()->SetHotKeyStr( HOTKEY_ID_SUMMONS, l_pHotKeyString );

}

void
CInputProcessMgr::IntHotKeyToStrHotKey( char * po_strHotKey, DWORD * pi_pKey )
{
	if( !pi_pKey )
		return;

	const int	KEY_LENGTH = 32;
	char		l_pKeyString[KEY_LENGTH];

	IntNormalKeyToStrNormalKey( l_pKeyString, pi_pKey[1] );

	switch( pi_pKey[0] )
	{
		case VK_SHIFT	: sprintf( po_strHotKey, "SHIFT+%s", l_pKeyString ); break;
		case VK_CONTROL	: sprintf( po_strHotKey, "CTRL+%s", l_pKeyString ); break;
		case VK_MENU	: sprintf( po_strHotKey, "ALT+%s", l_pKeyString ); break;
		default			: sprintf( po_strHotKey, "%s", l_pKeyString ); 
	}
}

void
CInputProcessMgr::IntNormalKeyToStrNormalKey( char * po_strKey, DWORD pi_pKey )
{
	if( ( pi_pKey >= VK_A ) && ( pi_pKey <= VK_Z ) )
	{
		sprintf( po_strKey, "%c", pi_pKey );
		return;
	}

	switch( pi_pKey )
	{
		case VK_ESCAPE	: sprintf( po_strKey, "ESC" ); break;
		case VK_TAB		: sprintf( po_strKey, "TAB" ); break;
		case VK_LEFT	: sprintf( po_strKey, "LEFT" ); break;
		case VK_UP		: sprintf( po_strKey, "UP" ); break;
		case VK_RIGHT	: sprintf( po_strKey, "RIGHT" ); break;
		case VK_DOWN	: sprintf( po_strKey, "DOWN" ); break;
		case VK_SPACE	: sprintf( po_strKey, "SPACE" ); break;
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CInputProcessMgr::InputProcess_Keyboard( void )
{
	// move key => 이동 관련키가 눌려지고 있다면 m_bIsPressedKey 플래그와 상관없이 처리한다.
	if( IsPressedMoveKey() )
	{
		if( InputProcess_Keyboard_AvatarMoveByKeyboard() )
			return TRUE;
	}

	//--------------------------------------------------------------------------

	_GetCharMgr()->SetRenderPlayerName( FALSE );
	_GetGroundItemMgr()->SetRenderItemName( FALSE );

	m_bShiftKeyDown		= GetKeyState( VK_SHIFT ) & 0x8000;
	m_bControlKeyDown	= GetKeyState( VK_CONTROL ) & 0x8000;
	m_bAltKeyDown		= GetKeyState( VK_MENU ) & 0x8000;

	if( !m_bIsPressedKey && !m_bAltKeyDown )
		return FALSE;
	m_bIsPressedKey = FALSE;

	// check chat mode
	if( _GetMainGame()->IsChatMode() &&
		!( GetKeyState( VK_RETURN ) & 0x8000 ) &&
		!( GetKeyState( m_listKey[KM_BEHAVIOR_CANCEL_TARGET][1] ) & 0x8000 ) &&
		!( m_bShiftKeyDown || m_bControlKeyDown || m_bAltKeyDown ) )
		return FALSE;

	//--------------------------------------------------------------------------

	if( InputProcess_Keyboard_CancelTarget() ) return TRUE;

	if( InputProcess_Keyboard_OpenInventoryWindow() ) return TRUE;
	if( InputProcess_Keyboard_OpenForceWindow() ) return TRUE;
	if( InputProcess_Keyboard_OpenSkillWindow() ) return TRUE;
	if( InputProcess_Keyboard_OpenAvatarInfoWindow() ) return TRUE;
	if( InputProcess_Keyboard_OpenPartyWindow() ) return TRUE;
	if( InputProcess_Keyboard_OpenAnimusWindow() ) return TRUE;
	if( InputProcess_Keyboard_OpenSelectAttackPartWindow() ) return TRUE;
	if( InputProcess_Keyboard_OpenMinimapWindow() ) return TRUE;
	if( InputProcess_Keyboard_OpenRebornWindow() ) return TRUE;

	if( InputProcess_Keyboard_ChangeCombatMode() ) return TRUE;
	if( InputProcess_Keyboard_ChangeMoveMode() ) return TRUE;
	if( InputProcess_Keyboard_ChangeAutoAttackMode() ) return TRUE;
	if( InputProcess_Keyboard_ItemLooting() ) return TRUE;
	if( InputProcess_Keyboard_Attack() ) return TRUE;
	if( InputProcess_Keyboard_ChangeTargetFromLatestTarget() ) return TRUE;
	//if( InputProcess_Keyboard_AvatarMoveByKeyboard() ) return TRUE;

	if( InputProcess_Keyboard_DisplayPrevSendChatMsg() ) return TRUE;
	if( InputProcess_Keyboard_DisplayPrevWhisperCharName() ) return TRUE;

	if( InputProcess_Keyboard_ScreenShot() ) return TRUE;

	if( InputProcess_Keyboard_Logout() ) return TRUE;
	if( InputProcess_Keyboard_ChangeChatMode() ) return TRUE;
	if( InputProcess_Keyboard_ChangeToFreeTargetMode() ) return TRUE;
	if( InputProcess_Keyboard_ChangeToAutoMouseMode() ) return TRUE;
	if( InputProcess_Keyboard_ChangeToAutoTargetMode() ) return TRUE;
	if( InputProcess_Keyboard_RenderShadow() ) return TRUE;
	if( InputProcess_Keyboard_OpenOptionWindow() ) return TRUE;
	if( InputProcess_Keyboard_HideUI() ) return TRUE;

#ifdef _DEVELOPER_

	if( InputProcess_Keyboard_ChangeToObserver() ) return TRUE;
	if( InputProcess_Keyboard_ChangeObserverMode() ) return TRUE;

	if( InputProcess_Keyboard_PlayRealtimeMovie() ) return TRUE;
	if( InputProcess_Keyboard_ChangeRealtimeWideMode() ) return TRUE;
	if( InputProcess_Keyboard_ChangeRealtimeCameraMode() ) return TRUE;

#endif

	if( InputProcess_Keyboard_Etc() ) return TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CInputProcessMgr::IsPressedSystemKey( DWORD pi_dwSystemKey )
{
	if( pi_dwSystemKey == VK_SHIFT )
	{
		return m_bShiftKeyDown;
	}
	else if( pi_dwSystemKey == VK_CONTROL )
	{
		if( !_GetMainGame()->IsChatMode() )
			return TRUE;

		return m_bControlKeyDown;
	}
	else if( pi_dwSystemKey == VK_MENU )
	{
		return m_bAltKeyDown;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::IsPressedKey( DWORD pi_dwKey )
{
	if( m_listKey[pi_dwKey][1] == ID_INVALID )
		return FALSE;

	if( m_listKey[pi_dwKey][0] != ID_INVALID )
	{
		if( !IsPressedSystemKey( m_listKey[pi_dwKey][0] ) )
			return FALSE;
	}

	if( m_listKey[pi_dwKey][0] == VK_MENU )
	{
		static const DWORD ALT_INPUT_LIMIT_TIME = 500;
					 DWORD l_dwCurTime = timeGetTime();

		if( ( l_dwCurTime - m_dwPressedAltKeyTime ) > ALT_INPUT_LIMIT_TIME )
		{
			if( ( GetAsyncKeyState( m_listKey[pi_dwKey][1] ) & 0xFFFF0000 ) == 0xFFFF0000 )
			{
				m_dwPressedAltKeyTime = l_dwCurTime;
				return TRUE;
			}
		}
	}
	else
	{
		if( gScan[ m_listKey[pi_dwKey][1] ] )
		{
			gScan[ m_listKey[pi_dwKey][1] ] = FALSE;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_CancelTarget( void )
{
	// Cancel 순서 => Active Window => Selected SF => Selected Target
	if( IsPressedKey( KM_BEHAVIOR_CANCEL_TARGET ) )
	{
		if( _GetTargetMgr()->IsOpenableWindow( UII_PLAYER_POPUP_MENU ) )
		{
			_GetTargetMgr()->CloseWindow( UII_PLAYER_POPUP_MENU );
			return TRUE;
		}

		if( _GetGBM()->GetSelectSF() )
		{
			_GetAvatar()->UI_DeselectSF();
			return TRUE;
		}

		_GetTargetMgr()->SetTarget( NULL );
		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ChangeChatMode( void )
{
	/*
	if( IsPressedKey( KM_GAME_CHANGE_CHATING_MODE ) )
	{
		if( _GetMainGame()->IsChatMode() )
		{
			_GetMainGame()->SetChatMode( FALSE );

			_GetChatWindow()->GetInputField()->DisableFocus();
			_GetChatWindow()->GetWhisperInputField()->DisableFocus();
			_GetChatInputWindow()->GetInputField()->DisableFocus();
		}
		else
		{
			_GetMainGame()->SetChatMode( TRUE );
			
			_GetChatWindow()->GetInputField()->EnableFocus();
			_GetChatWindow()->GetWhisperInputField()->EnableFocus();
			_GetChatInputWindow()->GetInputField()->EnableFocus();

			if( _GetChatWindow()->IsShow() )
				_GetChatWindow()->GetInputField()->SetFocus();
			else
				_GetChatInputWindow()->GetInputField()->SetFocus();
		}

		return TRUE;
	}*/

	if( IsPressedKey( KM_GAME_CHANGE_CHATING_MODE ) )
	{
		if( _GetChatWindow()->IsShow() )
			_GetChatWindow()->Close();
		else
			_GetChatWindow()->Open();

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ScreenShot( void )
{
	if( IsPressedKey( KM_GAME_SCREENSHOT ) )
	{
		_GetLevel()->ScreenShot();
		_SetSystemMsg( "스크린샷을 저장하였습니다." );
		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_OpenInventoryWindow( void )
{
	if( IsPressedKey( KM_WINDOW_INVENTORY ) )
	{
		if( g_nOpenedDlgBoxCount <= 0 )
		{
			_GetInventoryBoard()->ToggleWindow();
		}

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_OpenForceWindow( void )
{
	if( IsPressedKey( KM_WINDOW_FORCE ) )
	{
		if( g_nOpenedDlgBoxCount > 0 ) {;}

		_GetForceBoard()->ToggleWindow();

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_OpenSkillWindow( void )
{
	if( IsPressedKey( KM_WINDOW_SKILL ) )
	{
		if( g_nOpenedDlgBoxCount > 0 ) {;}

		_GetSkillBoard()->ToggleWindow();

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_OpenAvatarInfoWindow( void )
{
	if( IsPressedKey( KM_WINDOW_AVATAR_INFO ) )
	{
		if( g_nOpenedDlgBoxCount > 0 ) {;}
		else if( _GetCharInfoWindow()->IsShow() )
			_GetCharInfoWindow()->Close();
		else
			_GetCharInfoWindow()->Open();

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_OpenPartyWindow( void )
{
#ifdef _NEW_UI_
	if( IsPressedKey( KM_WINDOW_PARTY ) )
	{
		RECT rtWindowRect;
		_GetAnimusControlBoard()->GetWindowRect( rtWindowRect );

		if( _GetAnimusControlBoard()->IsOpen() )
			_GetPartyBoard()->SetWindowPos( 0, rtWindowRect.bottom );
		else
			_GetPartyBoard()->SetWindowPos( 0, rtWindowRect.top );


		_GetPartyBoard()->ToggleWindow();

		return TRUE;
	}
#endif // _NEW_UI_

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_OpenAnimusWindow( void )
{
	if( IsPressedKey( KM_ANIMUS_WINDOW ) )
	{
#ifdef _NEW_UI_
		if( _GetAvatar()->IsCora() )
		{
			_GetAnimusBoard()->ToggleWindow();
		}
#else
		if( ( _GetAvatar()->GetRace() == CRT_CORA_MALE ) || ( _GetAvatar()->GetRace() == CRT_CORA_FEMALE ) )
		{
			if( _GetSummonsSelectWindow()->IsShow() )
				_GetSummonsSelectWindow()->Close();
			else
				_GetSummonsSelectWindow()->Open();
		}
#endif

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_OpenSelectAttackPartWindow( void )
{
	if( IsPressedKey( KM_WINDOW_SELECT_ATTACK_PART ) )
	{
		if( _GetSelectAttackPartWindow()->IsShow() )
			_GetSelectAttackPartWindow()->Close();
		else
			_GetSelectAttackPartWindow()->Open();

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_OpenMinimapWindow( void )
{
	if( IsPressedKey( KM_WINDOW_MINIMAP ) )
	{	
		if( _GetMinimapWindow()->IsShow() )
		{
			_GetMinimapWindow()->Close();
		}
		else
		{
			MAP_INFO * pMapInfo = _GetLand()->GetCurMapInfo();
			BYTE byPackNo, bySocketNo;

			if( _GetInventoryBoard()->FindMapItemPosition( pMapInfo->pName, byPackNo, bySocketNo ) )
				_GetInventoryBoard()->UI_UseRequest( byPackNo, bySocketNo );
			else
				_SetSystemMsg( "맵 아이템이 없습니다." );
		}

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_OpenRebornWindow( void )
{
	if( IsPressedKey( KM_WINDOW_REBORN ) )
	{
		if( ( _GetAvatar()->GetAction() == CAI_DEATH_DIE ) ||
			( _GetAvatar()->GetAction() == CAI_INTERNALSTATE_DEATH ) )
		{
			//NetLogWrite( "F12 : InitMsg_PlayerRevivalRequest_clzo" );
			_GetNetworkMgr()->InitMsg_PlayerRevivalRequest_clzo();
		}

		if( _IsOpenedDialogBox( NULL, MSGBOX_ID_REBIRTH ) )
			_CloseDialogBox( NULL, MSGBOX_ID_REBIRTH );

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ChangeCombatMode( void )
{
	if( IsPressedKey( KM_COMBAT_CHANGE_COMBAT_MODE ) )
	{
		_GetAvatar()->ToggleCombatMode();
		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ChangeMoveMode( void )
{
	if( IsPressedKey( KM_COMBAT_CHANGE_MOVE_MODE ) )
	{
		_GetAvatar()->ToggleMoveMode();
		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ChangeAutoAttackMode( void )
{
	if( IsPressedKey( KM_COMBAT_CHANGE_AUTO_ATTACK_MODE ) )
	{
		_GetAvatar()->EnableAutoAttack( !_GetAvatar()->IsEnableAutoAttack() );

		if( _GetAvatar()->IsEnableAutoAttack() )
			_GetFuncKeyWindow()->ChangeAttackMode( CGUIHotKeyBoard::Auto );
		else
			_GetFuncKeyWindow()->ChangeAttackMode( CGUIHotKeyBoard::Manual );

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ItemLooting( void )
{
	if( IsPressedKey( KM_BEHAVIOR_ITEM_LOOTING ) )
	{
		_GetInventoryBoard()->UI_AutoPickUpItemRequest();
		return TRUE;
	}

	if( IsPressedKey( KM_COMBAT_ATTACK ) )
	{
		if( _GetTargetMgr()->IsEmptyTarget() && !_GetGBM()->GetSelectSF() )
		{
			_GetInventoryBoard()->UI_AutoPickUpItemRequest();
			return TRUE;
		}

		gScan[ m_listKey[KM_COMBAT_ATTACK][1] ] = TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_Attack( void )
{
	if( IsPressedKey( KM_COMBAT_ATTACK ) )
	{
		CCharacter * l_pTarget = _GetTargetMgr()->GetTarget();
		if( l_pTarget )
		{
			if( l_pTarget->GetConditionInfo()->m_bEnable_Stealth )
			{
				if( !_GetAvatar()->IsEnableDetect( l_pTarget ) )
					return TRUE;
			}

			_GetMainGame()->ProcessMousePicking_Character( l_pTarget, TRUE );
		}
		else
		{
			_GetMainGame()->ProcessMousePicking_Character_None( CAT_NORMAL );
		}

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ChangeTargetFromLatestTarget( void )
{
	if( IsPressedKey( KM_BEHAVIOR_CHANGE_TARGET_TO_LASTEST ) )
	{
		_GetTargetMgr()->ChangeTargetFromLatestTarget();

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_AvatarMoveByKeyboard( void )
{
	if( !_GetMainGame()->IsChatMode() )
	{
		BOOL l_bIsPressed_Left	= FALSE;//IsPressedKey( KM_MOVE_LEFT );
		BOOL l_bIsPressed_Up	= FALSE;//IsPressedKey( KM_MOVE_UP );
		BOOL l_bIsPressed_Right = FALSE;//IsPressedKey( KM_MOVE_RIGHT );
		BOOL l_bIsPressed_Down	= FALSE;//IsPressedKey( KM_MOVE_DOWN );

		IsPressedMoveKey( l_bIsPressed_Left, l_bIsPressed_Up, l_bIsPressed_Right, l_bIsPressed_Down );

		if( l_bIsPressed_Left && l_bIsPressed_Up )
			_GetAvatar()->RequestMoveByKeyboard( CMK_LEFT + CMK_UP );
		else if( l_bIsPressed_Right && l_bIsPressed_Up )
			_GetAvatar()->RequestMoveByKeyboard( CMK_RIGHT + CMK_UP );
		else if( l_bIsPressed_Left && l_bIsPressed_Down )
			_GetAvatar()->RequestMoveByKeyboard( CMK_LEFT + CMK_DOWN );
		else if( l_bIsPressed_Right && l_bIsPressed_Down )
			_GetAvatar()->RequestMoveByKeyboard( CMK_RIGHT + CMK_DOWN );
		else if( l_bIsPressed_Up )
			_GetAvatar()->RequestMoveByKeyboard( CMK_UP );
		else if( l_bIsPressed_Down )
			_GetAvatar()->RequestMoveByKeyboard( CMK_DOWN );
		else if( l_bIsPressed_Left )
			_GetAvatar()->RequestMoveByKeyboard( CMK_LEFT );
		else if( l_bIsPressed_Right )
			_GetAvatar()->RequestMoveByKeyboard( CMK_RIGHT );
		/*
		if( GetKeyState( VK_Q ) & 0x80000000 )
			_GetAvatar()->RequestMoveByKeyboard( CMK_LEFT_ROT );
		if( GetKeyState( VK_E ) & 0x80000000 )
			_GetAvatar()->RequestMoveByKeyboard( CMK_RIGHT_ROT );
		*/

		if( l_bIsPressed_Left || l_bIsPressed_Up || l_bIsPressed_Right || l_bIsPressed_Down )
			return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_DisplayPrevSendChatMsg( void )
{
	if( IsPressedKey( KM_CHAT_PREV_SEND_CHAT_MSG ) )
	{
		_GetAvatar()->GetChatInfo()->DisplaySavedSendMessage();

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_DisplayPrevWhisperCharName( void )
{
	if( IsPressedKey( KM_CHAT_PREV_WHISPER_CHAR_NAME ) )
	{
		_GetAvatar()->GetChatInfo()->DisplaySavedWhisperList();

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_Logout( void )
{
	if( IsPressedKey( KM_GAME_LOGOUT ) )
	{
		_GetMainGame()->RequestLogout();
		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ChangeToFreeTargetMode( void )
{
	if( IsPressedKey( KM_COMBAT_CHANGE_FREE_TARGET_MODE ) )
	{
		_GetMainGame()->SetFreeTargetCombatMode( !_GetMainGame()->IsFreeTargetCombatMode() );

		if( _GetMainGame()->IsFreeTargetCombatMode() )
		{
			float l_fCameraRotY = ( int )_GetCamera()->GetPitch() % 360;
			_GetAvatar()->SetRotY( 360 - l_fCameraRotY, FALSE );

			_GetUIScreen()->GetMouse()->Show( FALSE );
		}
		else
		{
			_GetUIScreen()->GetMouse()->Show( TRUE );
		}

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ChangeToAutoMouseMode( void )
{
	if( IsPressedKey( KM_GAME_CHANGE_AUTO_MOUSE_MODE ) )
	{
		_GetMainGame()->SetAutoMouse( !_GetMainGame()->IsAutoMouse() );
		_GetMainGame()->SetMoveByDrag( FALSE );
		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ChangeToAutoTargetMode( void )
{
	if( IsPressedKey( KM_GAME_CHANGE_AUTO_TARGET_MODE ) )
	{
		_GetMainGame()->SetAutoTargeting( !_GetMainGame()->IsAutoTargeting() );
		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_RenderShadow( void )
{
	if( IsPressedKey( KM_GAME_RENDER_SHADOW ) )
	{
		_GetCharMgr()->Switching_RenderShadow();
		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_OpenOptionWindow( void )
{
	if( IsPressedKey( KM_WINDOW_OPTION ) )
	{
		if( _GetOptionWindow()->IsShow() )
			_GetOptionWindow()->Close();
		else
			_GetOptionWindow()->Open();

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_HideUI( void )
{
	if( IsPressedKey( KM_GAME_HIDE_UI ) )
	{
		if( _GetUIScreen()->IsShow() )
		{
			_GetMainGame()->SetHideUI( TRUE );
			_GetUIScreen()->Show( FALSE );
		}
		else
		{
			_GetMainGame()->SetHideUI( FALSE );
			_GetUIScreen()->Show( TRUE );
		}

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ChangeToObserver( void )
{
	if( IsPressedKey( KM_GAME_CHANGE_TO_OBSERVER ) )
	{
		if( _GetAvatar()->GetObserverInfo()->m_bIsObserver )
			_GetAvatar()->GetObserverInfo()->SetObserver( FALSE );
		else
			_GetAvatar()->GetObserverInfo()->SetObserver( TRUE );

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ChangeObserverMode( void )
{
	if( IsPressedKey( KM_GAME_CHANGE_OBSERVER_MODE ) )
	{
		BYTE l_byMode = ( ++_GetAvatar()->GetObserverInfo()->m_byObserverMode ) % ( MAX_OBSERVER_MODE - 1 );
		_GetAvatar()->GetObserverInfo()->SetObserverMode( l_byMode );

		return TRUE;
	}

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_PlayRealtimeMovie( void )
{
#ifdef _DEVELOPER_
	static Vector3f l_vecPrevAvatarPos;

	if( IsPressedKey( KM_GAME_PLAY_REALTIME_MOVIE ) )
	{
		_GetMainGame()->SetPlayMovie( !_GetMainGame()->IsPlayMovie() );

		if( _GetMainGame()->IsPlayMovie() )
		{
			RTMovieSetState( _GetMainGame()->GetMovieFlag() );
			RTMovieStartPlay( TRUE );
			_GetAvatar()->GetObserverInfo()->SetObserver( TRUE );
			_GetAvatar()->GetObserverInfo()->SetObserverMode( OMI_FLY );
			_GetUIScreen()->Show( FALSE );
			_GetMainGame()->SetHideUI( TRUE );

			memcpy( l_vecPrevAvatarPos, _GetAvatar()->GetPosition(), sizeof( Vector3f ) );
		}
		else
		{
			RTMovieSetState( _GetMainGame()->GetMovieFlag() );
			_GetAvatar()->GetObserverInfo()->SetObserverMode( OMI_NORMAL );
			_GetAvatar()->GetObserverInfo()->SetObserver( FALSE );
			_GetUIScreen()->Show( TRUE );
			_GetMainGame()->SetHideUI( FALSE );

			_GetAvatar()->SetPosition( l_vecPrevAvatarPos );
		}

		return TRUE;
	}
#endif

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ChangeRealtimeWideMode( void )
{
#ifdef _DEVELOPER_
	if( IsPressedKey( KM_GAME_CHANGE_REALTIME_WIDE_MODE ) )
	{
		DWORD l_dwMovieFlag = _GetMainGame()->GetMovieFlag();

		if( l_dwMovieFlag & RTM_STATE_NO_WIDE )
		{
			l_dwMovieFlag &= ( ~RTM_STATE_NO_WIDE );
			l_dwMovieFlag &= ( ~RTM_STATE_NO_LOGO );
		}
		else
		{
			l_dwMovieFlag |= RTM_STATE_NO_WIDE;
			l_dwMovieFlag |= RTM_STATE_NO_LOGO;
		}

		RTMovieSetState( l_dwMovieFlag );

		_GetMainGame()->SetMovieFlag( l_dwMovieFlag );

		return TRUE;
	}
#endif

	return FALSE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_ChangeRealtimeCameraMode( void )
{
#ifdef _DEVELOPER_
	if( IsPressedKey( KM_GAME_CHANGE_REALTIME_CAMERA_MODE ) )
	{
		DWORD l_dwMovieFlag = _GetMainGame()->GetMovieFlag();

		if( l_dwMovieFlag & RTM_STATE_NO_CAMERAMOVE )
		{
			l_dwMovieFlag &= ( ~RTM_STATE_NO_LOGO );
			l_dwMovieFlag &= ( ~RTM_STATE_NO_CAMERAMOVE );
		}
		else
		{
			l_dwMovieFlag |= RTM_STATE_NO_CAMERAMOVE;
			l_dwMovieFlag |= RTM_STATE_NO_LOGO;
		}

		RTMovieSetState( l_dwMovieFlag );

		_GetMainGame()->SetMovieFlag( l_dwMovieFlag );

		return TRUE;
	}
#endif

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CInputProcessMgr::InputProcess_Keyboard_Etc( void )
{
	if( m_bShiftKeyDown )
		InputProcess_Keyboard_Etc_WithShift();
	else if( m_bAltKeyDown )
		InputProcess_Keyboard_Etc_WithAlt();
	else if( m_bControlKeyDown )
		InputProcess_Keyboard_Etc_WithControl();
	else
		InputProcess_Keyboard_Etc_Normal();

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CInputProcessMgr::InputProcess_Keyboard_Etc_WithShift()
{
	if( !_GetMainGame()->IsChatMode() )
		InputProcess_Keyboard_Etc_SelectTargetByRotate();

	if( _GetMainGame()->IsFreeTargetCombatMode() )
		_GetUIScreen()->GetMouse()->Show( TRUE );

	return TRUE;
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_Etc_SelectTargetByRotate( void )
{
	// 탭 키로 타겟 셀렉트
	if( gScan[VK_TAB] )
	{
		//CCharacter * l_pTarget = _GetAvatar()->GetNextTargetInList( FALSE );
		CCharacter * l_pTarget = _GetTargetMgr()->GetNextTargetInList( FALSE );
		if( l_pTarget )
			//_GetAvatar()->SetTarget( l_pTarget );
			_GetTargetMgr()->SetTarget( l_pTarget );

		gScan[VK_TAB] = FALSE;
		return TRUE;
	}

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CInputProcessMgr::InputProcess_Keyboard_Etc_WithAlt( void )
{
	static	DWORD l_dwInputTime	= 0;
			DWORD l_dwCurTime	= timeGetTime();

	if( ( l_dwCurTime - l_dwInputTime ) < 500 )
		return FALSE;

	if( ( GetAsyncKeyState( VK_J ) & 0xFFFF0000 ) == 0xFFFF0000 )
	{
		_GetMainGame()->ToggleHint();
		l_dwInputTime = l_dwCurTime;
	}

	//--------------------------------------------------------------------------

#ifdef _DEVELOPER_

	// 임시처리 - 전투 던전에서 나가는 키
	if( ( GetAsyncKeyState( VK_M ) & 0xFFFF0000 ) == 0xFFFF0000 )
	{
		CNetMsgProcessor_Dungeon * l_pNetMsgProcessor = static_cast< CNetMsgProcessor_Dungeon * >
														( _GetNetworkMgr()->GetNetMsgProcessor( NMP_DUNGEON ) );
		if( !l_pNetMsgProcessor )
			return FALSE;

		l_pNetMsgProcessor->OutofDungeonRequest_clzo();

		l_dwInputTime = l_dwCurTime;
	}

#endif

	//--------------------------------------------------------------------------

	_GetCharMgr()->SetRenderPlayerName( TRUE );
	_GetGroundItemMgr()->SetRenderItemName( TRUE );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CInputProcessMgr::InputProcess_Keyboard_Etc_WithControl( void )
{
	InputProcess_Keyboard_Etc_SetTarget();

	InputProcess_Keyboard_Etc_DisplayInfo();

	// Ctrl키의 조합이나 비채팅 모드시에 동작
	InputProcess_Keyboard_Etc_WithControlAndNormal();

	//--------------------------------------------------------------------------

#ifdef _DEVELOPER_

	// 퀘스트 처리 => 임시로 키보를 이용해 퀘스트를 시작한다.
	if( gScan[VK_NUMPAD1] )
	{
		_GetAvatar()->GetTutorialInfo()->m_dwCurQuestNum = 0;
		_GetAvatar()->GetTutorialInfo()->RefreshQuest();

		gScan[VK_NUMPAD1] = FALSE;
	}

	if( gScan[VK_NUMPAD2] )
	{
		_GetAvatar()->GetTutorialInfo()->m_dwCurQuestNum = 1;
		_GetAvatar()->GetTutorialInfo()->RefreshQuest();

		gScan[VK_NUMPAD2] = FALSE;
	}

	if( gScan[VK_NUMPAD3] )
	{
		_GetAvatar()->GetTutorialInfo()->m_dwCurQuestNum = 2;
		_GetAvatar()->GetTutorialInfo()->RefreshQuest();

		gScan[VK_NUMPAD3] = FALSE;
	}

	if( gScan[VK_NUMPAD9] )
	{
		_GetAvatar()->GetTutorialInfo()->UnLoadQuestInfoFile();
		_GetAvatar()->GetTutorialInfo()->LoadQuestInfoFile( "./DataTable/Tutorial.scp" );

		gScan[VK_NUMPAD9] = FALSE;
	}

	if( gScan[VK_NUMPAD0] )
	{
		LoadData();

		gScan[VK_NUMPAD0] = FALSE;
	}

	//--------------------------------------------------------------------------

	// 유닛 관련 임시 기능
	if( gScan[VK_NUMPAD4] )
	{
		CNetMsgProcessor_Unit * pNMP = static_cast< CNetMsgProcessor_Unit * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_UNIT ) );
		pNMP->UnitFrameBuyRequest_clzo( 0 );

		gScan[VK_NUMPAD4] = FALSE;
	}

	if( gScan[VK_NUMPAD5] )
	{
		_GetUnitTuningBoard()->ToggleWindow();

		gScan[VK_NUMPAD5] = FALSE;
	}

	if( gScan[VK_NUMPAD6] )
	{
		CNetMsgProcessor_Unit * pNMP = static_cast< CNetMsgProcessor_Unit * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_UNIT ) );
		pNMP->UnitSellRequest_clzo( 0 );

		gScan[VK_NUMPAD6] = FALSE;
	}

	if( gScan[VK_ADD] )
	{
		CNetMsgProcessor_Unit * pNMP = static_cast< CNetMsgProcessor_Unit * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_UNIT ) );

		if( _GetAvatar()->GetLinkPlayer() )
			_GetAvatar()->MountUnitRequest( _GetAvatar()->GetLinkPlayer() );
		else
			pNMP->UnitDeliveryRequest_clzo( ID_INVALID );

		gScan[VK_ADD] = FALSE;
	}

	if( gScan[VK_SUBTRACT] )
	{
		CNetMsgProcessor_Unit * pNMP = static_cast< CNetMsgProcessor_Unit * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_UNIT ) );

		if( _GetAvatar()->IsEqualUnitMountState( UMS_MOUNT ) )
			_GetAvatar()->DismountUnitRequest();
		else
			pNMP->UnitReturnRequest_clzo();

		gScan[VK_SUBTRACT] = FALSE;
	}

	// 전직 관련 임시 기능
	if( gScan[VK_MULTIPLY] )
	{
		_GetClassChangeBoard()->OpenWindow();

		gScan[VK_MULTIPLY] = FALSE;
	}


#endif // _DEVELOPER_

	return TRUE;
}

void
CInputProcessMgr::InputProcess_Keyboard_Etc_SetTarget( void )
{
	if( _GetMainGame()->IsChatMode() )
		return;

	//if( !( ( _GetAvatar()->GetTargetTypeID() != ID_INVALID ) &&
	//	 ( _GetAvatar()->GetTargetIndex() != ID_INVALID ) ) )
	//	return;
	CCharacter * l_pTarget = _GetTargetMgr()->GetTarget();
	if( !l_pTarget )
		return;

	for( int i = VK_1; i < VK_1 + 10; ++i )
	{
		if( gScan[i] )
		{
			_GetTargetMgr()->AddTargetToList( i - VK_1, l_pTarget );
			gScan[i] = FALSE;
			break;
		}
	}
}

void
CInputProcessMgr::InputProcess_Keyboard_Etc_DisplayInfo( void )
{
#ifdef _DEVELOPER_

	// 도움말
	if( gScan[VK_F1] )
	{
		_GetMainGame()->SetDisplayHelpText( !_GetMainGame()->IsDisplayHelpText() );
		gScan[VK_F1] = FALSE;
	}

	// 장착 제한 설정
	if( gScan[VK_F2] )
	{
		//g_bEnableCheckEquipItem = !g_bEnableCheckEquipItem;
		gScan[VK_F2] = FALSE;
	}

	// 맵 이동 제한 설정
	if( gScan[VK_F3] )
	{
		_GetAvatar()->SetEnableTeleport( !_GetAvatar()->IsEnableTeleport() );
		gScan[VK_F3] = FALSE;
	}

	// 충돌맵 렌더링
	if( gScan[VK_F4] )
	{
		_GetMainGame()->SetRenderCollisionMap( !_GetMainGame()->IsRenderCollisionMap() );
		gScan[VK_F4] = FALSE;
	}

	// 게임 정보 디스플레이
	if( gScan[VK_F5] )
	{
		_GetMainGame()->SetDisplayInfomationMode( ( _GetMainGame()->GetDisplayInfomationMode() + 1 ) %
												  MAX_DISPLAY_INFO_NUM );
		gScan[VK_F5] = FALSE;
	}

	// 렌더링 정보
	if( gScan[VK_F6] )
	{
		_GetCharMgr()->SetRenderInfo( !_GetCharMgr()->IsRenderInfo() );
		gScan[VK_F6] = FALSE;
	}

	// 바운딩 박스 렌더링 설정
	if( gScan[VK_F7] )
	{
		_GetMainGame()->SetRenderBoundBox( !_GetMainGame()->IsRenderBoundBox() );
		_GetCharMgr()->SetDrawBBox( _GetMainGame()->IsRenderBoundBox() );
		_GetGroundItemMgr()->SetDrawBBox( _GetMainGame()->IsRenderBoundBox() );
		gScan[VK_F7] = FALSE;
	}

	// 네트웍 테스트 로그 설정	( 테스트 코드 )
	if( gScan[VK_F8] )
	{
		g_bWriteTestLog = !g_bWriteTestLog;
		gScan[VK_F8] = FALSE;
	}

#endif
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CInputProcessMgr::InputProcess_Keyboard_Etc_Normal( void )
{
	InputProcess_Keyboard_Etc_SelectTarget();
	InputProcess_Keyboard_Etc_ChangeChatModeByEnter();

	// Ctrl키의 조합이나 비채팅 모드시에 동작
	if( !_GetMainGame()->IsChatMode() )
		InputProcess_Keyboard_Etc_WithControlAndNormal();

	//--------------------------------------------------------------------------

	// 퀘스트 처리 => 임시로 입력 버튼을 키보드로 대신한다.
	if( gScan[VK_K] )
	{
		_GetAvatar()->GetTutorialInfo()->QSV.m_byInputButton = QBI_WAIT_DATA::WAIT_INPUT_PREV;
		gScan[VK_K] = FALSE;
	}

	if( gScan[VK_L] )
	{
		_GetAvatar()->GetTutorialInfo()->QSV.m_byInputButton = QBI_WAIT_DATA::WAIT_INPUT_NEXT;
		gScan[VK_L] = FALSE;
	}

	return TRUE;
}

void
CInputProcessMgr::InputProcess_Keyboard_Etc_SelectTarget( void )
{
	if( _GetMainGame()->IsChatMode() )
		return;

	CCharacter * l_pCurTarget	 = NULL;
	CCharacter * l_pTargetInList = NULL;

	// 번호로 타겟 셀렉트
	for( int i = VK_1; i < VK_1 + 10; ++i )
	{
		if( gScan[i] )
		{
			l_pTargetInList = _GetTargetMgr()->GetTargetInList( i - VK_1 );
			if( l_pTargetInList )
			{
				_GetTargetMgr()->SetTarget( l_pTargetInList );
				_GetTargetMgr()->SetCurrentTargetSocketNo( i - VK_1 );

				if( l_pTargetInList->IsEqualCharType( CTI_PLAYER ) &&
					_GetTargetMgr()->IsEqualTarget( l_pTargetInList ) )
				{
					if( _GetTargetMgr()->IsOpenableWindow( UII_PLAYER_POPUP_MENU ) )
						_GetTargetMgr()->CloseWindow( UII_PLAYER_POPUP_MENU );
					else
						_GetTargetMgr()->OpenWindow( UII_PLAYER_POPUP_MENU );
				}
			}

			gScan[i] = FALSE;
			break;
		}
	}
}

BOOL
CInputProcessMgr::InputProcess_Keyboard_Etc_ChangeChatModeByEnter( void )
{
	if( gScan[VK_RETURN] )
	{		
		// dialog box가 떠있을 때는 채팅모드로 바꿀수 없다
		if( _GetMainGame()->GetHoverMouseUI() == DEFAULT_ID_DIALOG_BOX )
		{
			gScan[VK_RETURN] = FALSE;
			return TRUE;
		}
		// 거래창이 열였을 때
		else if( g_nOpenedTradeWin )
		{
			if( _GetChatWindow()->IsShow() )
			{		
				// 비채팅모드로 전환하기 위해 현재 상태를 채팅모드로.
				if( _GetChatWindow()->GetInputField()->IsFocused() )
				{					
					_GetMainGame()->SetChatMode( TRUE );
				}
				//  채팅모드로 전환하기 위해 active상태로 만든다.
				else 
				{					
					_GetMainGame()->SetChatMode( FALSE );
					_GetChatWindow()->Open();
				}				
			}
			else if( _GetChatInputWindow()->IsShow() )
			{
				if( _GetChatInputWindow()->GetInputField()->IsFocused() )
				{
					_GetMainGame()->SetChatMode( TRUE );
				}
				else
				{
					_GetMainGame()->SetChatMode( FALSE );
					_GetChatInputWindow()->Open();
				}					
			}
		}
		else if( _GetChatWindow()->IsShow() )
		{
			gScan[VK_RETURN] = FALSE;
			return TRUE;
		}
		
		// 비채팅모드이면 채팅모드로 전환
		if( !_GetMainGame()->IsChatMode() )			
		{
			_GetMainGame()->SetChatMode( TRUE );
			
			_GetChatWindow()->GetInputField()->EnableFocus();
			_GetChatWindow()->GetWhisperInputField()->EnableFocus();
			_GetChatInputWindow()->GetInputField()->EnableFocus();

			if( _GetChatWindow()->IsShow() )
				_GetChatWindow()->GetInputField()->SetFocus();
			else
				_GetChatInputWindow()->GetInputField()->SetFocus();
		}
		// 입력창이 열려있고 채팅모드이면 비채팅모드로 전환
		else if( _GetChatInputWindow()->IsShow() )				 
		{
			_GetMainGame()->SetChatMode( FALSE );

			_GetChatInputWindow()->GetInputField()->DisableFocus();
		}

		gScan[VK_RETURN] = FALSE;
		return TRUE;
	}

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CInputProcessMgr::InputProcess_Keyboard_Etc_WithControlAndNormal( void )
{
#ifdef _DEVELOPER_

	// 아바타의 상태 정보
	if( gScan[VK_NUMPAD7] )
	{
		if( _IsOpenedDialogBox( NULL, MSGBOX_ID_CONDITION_INFO ) )
			_CloseDialogBox( NULL, MSGBOX_ID_CONDITION_INFO );
		else
			_GetAvatar()->GetConditionInfo()->DisplayConditionInfo();

		gScan[VK_NUMPAD7] = FALSE;
	}

	// 타겟의 상태 정보
	if( gScan[VK_NUMPAD8] )
	{
		if( _IsOpenedDialogBox( NULL, MSGBOX_ID_CONDITION_INFO ) )
		{
			_CloseDialogBox( NULL, MSGBOX_ID_CONDITION_INFO );
		}
		else
		{
			//CCharacter * l_pTarget = _GetAvatar()->GetTarget();
			CCharacter * l_pTarget = _GetTargetMgr()->GetTarget();
			if( l_pTarget )
				l_pTarget->GetConditionInfo()->DisplayConditionInfo();
		}

		gScan[VK_NUMPAD8] = FALSE;
	}

#endif

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CInputProcessMgr::IsMoveKey( DWORD pi_lParam )
{
	DWORD l_dwScanCode = ( pi_lParam & 0x00FF0000 ) >> 16;

	if( ( l_dwScanCode == CMK_UP ) || ( l_dwScanCode == CMK_DOWN ) ||
		( l_dwScanCode == CMK_LEFT ) || ( l_dwScanCode == CMK_RIGHT ) )
		return TRUE;

	return FALSE;
}

BOOL
CInputProcessMgr::IsPressedMoveKey( void )
{
	//if( ( GetAsyncKeyState( m_listKey[KM_MOVE_LEFT][1] ) & 0xFFFF0000 ) == 0xFFFF0000 )
	if( GetKeyState( m_listKey[KM_MOVE_LEFT][1] ) & 0x8000 )
		return TRUE;
	//if( ( GetAsyncKeyState( m_listKey[KM_MOVE_UP][1] ) & 0xFFFF0000 ) == 0xFFFF0000 )
	if( GetKeyState( m_listKey[KM_MOVE_UP][1] ) & 0x8000 )
		return TRUE;
	//if( ( GetAsyncKeyState( m_listKey[KM_MOVE_RIGHT][1] ) & 0xFFFF0000 ) == 0xFFFF0000 )
	if( GetKeyState( m_listKey[KM_MOVE_RIGHT][1] ) & 0x8000 )
		return TRUE;
	//if( ( GetAsyncKeyState( m_listKey[KM_MOVE_DOWN][1] ) & 0xFFFF0000 ) == 0xFFFF0000 )
	if( GetKeyState( m_listKey[KM_MOVE_DOWN][1] ) & 0x8000 )
		return TRUE;

	return FALSE;
}

void
CInputProcessMgr::IsPressedMoveKey( BOOL & pi_bLeftKey, BOOL & pi_bUpKey, BOOL & pi_bRightKey, BOOL & pi_bDownKey )
{
	//if( ( GetAsyncKeyState( m_listKey[KM_MOVE_LEFT][1] ) & 0xFFFF0000 ) == 0xFFFF0000 )
	if( GetKeyState( m_listKey[KM_MOVE_LEFT][1] ) & 0x8000 )
		pi_bLeftKey = TRUE;
	//if( ( GetAsyncKeyState( m_listKey[KM_MOVE_UP][1] ) & 0xFFFF0000 ) == 0xFFFF0000 )
	if( GetKeyState( m_listKey[KM_MOVE_UP][1] ) & 0x8000 )
		pi_bUpKey = TRUE;
	//if( ( GetAsyncKeyState( m_listKey[KM_MOVE_RIGHT][1] ) & 0xFFFF0000 ) == 0xFFFF0000 )
	if( GetKeyState( m_listKey[KM_MOVE_RIGHT][1] ) & 0x8000 )
		pi_bRightKey = TRUE;
	//if( ( GetAsyncKeyState( m_listKey[KM_MOVE_DOWN][1] ) & 0xFFFF0000 ) == 0xFFFF0000 )
	if( GetKeyState( m_listKey[KM_MOVE_DOWN][1] ) & 0x8000 )
		pi_bDownKey = TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

LRESULT
CInputProcessMgr::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_KEYDOWN :
			m_bIsPressedKey = TRUE;
			break;

		case WM_KEYUP :
			if( _GetMainGame() )	//NULL이될때 드러올때 있어서 JUI
			{
				if( !_GetMainGame()->IsEnableInput() )
					break;
			}

			if( _GetAvatar()->IsEqualAction( CAI_MOVE_MOVING ) )
			{
				if( IsMoveKey( lParam ) )
				{
					if( !IsPressedMoveKey() )
						_GetAvatar()->RequestStopByKeyboard();
				}
			}
			break;

		case WM_SYSKEYDOWN :
			m_bIsPressedKey = TRUE;
			break;
	}

	return 0;
}