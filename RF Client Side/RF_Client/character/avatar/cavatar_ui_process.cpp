#include "../../GameObject/Manager/CCharacterMgr.h"

#include "../../DataTable/ServerTable/ExpFld.h"

#include "../../UserInterface/GameWindow/GUIGaugeExpWindow.h"
#include "../../UserInterface/GameWindow/GUIHotKeyBoard.h"
#include "../../UserInterface/GameWindow/GUIForceBeltWindow.h"
#include "../../UserInterface/GameWindow/GUIChatWindow.h"
#include "../../UserInterface/GameWindow/SkillForce/GUIForceWindow.h"
#include "../../UserInterface/GameWindow/SkillForce/GUISkillWindow.h"
#include "../../UserInterface/GameWindow/GUICharInfoWindow.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// UI 채팅창에서 단축키 필터링.
BOOL
EP_ProcessFiltering_KeydownMsg( LPARAM pi_lParam )
{
	if( GetKeyState( VK_CONTROL ) & 0x8000 )
	{
		DWORD l_dwScanCode = ( pi_lParam & 0x00FF0000 ) >> 16;
		if( ( l_dwScanCode == 19 ) || ( l_dwScanCode == 30 ) )
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------

// 펑션키 윈도우에서 마우스로 클릭했을때의 처리
BOOL
EP_WINDOW_OPEN( DWORD pi_dwWindowID )
{
	static char RACE_TYPE[3][12] = { "벨라토", "코라", "아크레시아" };
	static char ZENDER_TYPE[2][12] = { "남자", "여자" };

/*
	if( pi_dwWindowID == UII_CHAR_INFO_WINDOW )
	{
		_GetCharInfoWindow()->SetCharName( _GetAvatar()->GetName() );
		_GetCharInfoWindow()->SetRace( RACE_TYPE[_GetAvatar()->GetRace()/2] );
		_GetCharInfoWindow()->SetSex( ZENDER_TYPE[_GetAvatar()->GetRace()%2] );
	}
*/
	return TRUE;
}

// 펑션키 윈도우에서 마우스로 클릭했을때의 처리
BOOL
EP_WINDOW_CLOSE( DWORD pi_dwWindowID )
{
	return TRUE;
}