#include "GUIHotKeyBoard.h"
#include "../GUIController/GUIScreen.h"
#include "../SpriteMgr.h"

#include "GUIItemBeltWindow.h"
#include "GUIForceBeltWindow.h"

class CGUIItemBeltWindow;
class CGUIForceBeltWindow;

extern CGUIHotKeyBoard *		g_pUiFuncKeyWindow;
extern CGUIItemBeltWindow *		g_pUiItemBeltWindow;
extern CGUIForceBeltWindow *	g_pUiSFBeltWindow;

////////////////////////////////////////////////////////////////////////////
//
// CGUIUnitControlBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIUnitControlBoard::CGUIUnitControlBoard()
{
	m_bIsCreated = FALSE;
}

CGUIUnitControlBoard::~CGUIUnitControlBoard()
{

}

void
CGUIUnitControlBoard::Init()
{
	SetSize( gScreen.m_ptSize.x, 41 );

	m_uiActionKey.SetSize( 36, 34 );
	m_uiMoveKey.SetSize( 36, 34 );
	m_uiAttackKey.SetSize( 36, 34 );
	m_uiWeaponKey[Main_Weapon].SetSize( 40, 40 );
	m_uiWeaponKey[Sub_Weapon].SetSize( 40, 40 );
	m_uiSelectedWeaponAni.SetSize( 40, 40 );
	m_uiGetOffUnitKey.SetSize( 40, 40 );

	m_uiActionKey.SetPos( m_ptPos.x + 10, m_ptPos.y + 4 );
	m_uiMoveKey.SetPos( m_uiActionKey.m_ptPos.x + m_uiActionKey.m_ptSize.x , m_ptPos.y + 4 );
	m_uiAttackKey.SetPos( m_uiMoveKey.m_ptPos.x + m_uiMoveKey.m_ptSize.x , m_ptPos.y + 4 );

	m_uiWeaponKey[Main_Weapon].SetPos( m_ptPos.x + m_ptSize.x - 155, m_ptPos.y );		
	m_uiWeaponKey[Sub_Weapon].SetPos( m_uiWeaponKey[Main_Weapon].m_ptPos.x + m_uiWeaponKey[Main_Weapon].m_ptSize.x + 2, m_ptPos.y );	

	m_uiGetOffUnitKey.SetPos( m_ptPos.x + m_ptSize.x - m_uiGetOffUnitKey.m_ptSize.x - 10, m_ptPos.y );

	m_uiActionKey.AddMouseEventHandler( this );
	m_uiMoveKey.AddMouseEventHandler( this );
	m_uiAttackKey.AddMouseEventHandler( this );
	m_uiWeaponKey[Main_Weapon].AddMouseEventHandler( this );
	m_uiBulletNum[Main_Weapon].AddMouseEventHandler( this );
	m_uiWeaponKey[Sub_Weapon].AddMouseEventHandler( this );
	m_uiBulletNum[Sub_Weapon].AddMouseEventHandler( this );
	m_uiGetOffUnitKey.AddMouseEventHandler( this );	

	Add( &m_uiActionKey );
	Add( &m_uiMoveKey );
	Add( &m_uiAttackKey );

	Add( &m_uiWeaponKey[Main_Weapon] );
	Add( &m_uiBulletNum[Main_Weapon] );
	Add( &m_uiWeaponKey[Sub_Weapon] );
	Add( &m_uiBulletNum[Sub_Weapon] );
	Add( &m_uiGetOffUnitKey );

	SetSprite();

	m_uiSelectedWeaponAni.SetTotalFrameNum( 6 );
	m_uiSelectedWeaponAni.SetObjectNumInLine( 2 );
	m_uiSelectedWeaponAni.SetFPS( 10 );
	m_uiSelectedWeaponAni.SetStopInterval( 200 );	

	m_uiWeaponKey[Main_Weapon].SetName( "메인 무기" );
	m_uiWeaponKey[Sub_Weapon].SetName( "보조 무기" );
	m_uiGetOffUnitKey.SetName( "하차" );

	m_bIsCreated = TRUE;
}

void
CGUIUnitControlBoard::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );

		l_spriteInfo.wActionNo = 2;

		l_spriteInfo.wFrameNo = 0;
		m_uiActionKey.SetSpriteInfo( &l_spriteInfo );
		
		l_spriteInfo.wFrameNo = 5;
		m_uiAttackKey.SetSpriteInfo( &l_spriteInfo );

		// -------------------------------------------------------
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_UNIT );

		l_spriteInfo.wActionNo = 3;

		l_spriteInfo.wFrameNo = 0;
		m_uiWeaponKey[0].SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 2;
		m_uiWeaponKey[1].SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 5;
		m_uiGetOffUnitKey.SetSpriteInfo( &l_spriteInfo );	

		l_spriteInfo.wFrameNo = 6;
		m_uiMoveKey.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 4;
		m_uiSelectedWeaponAni.SetSpriteInfo( &l_spriteInfo );		

		// -------------------------------------------------------
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );	
		
		POINT l_ptNumSize;
		l_ptNumSize.x = 7;
		l_ptNumSize.y = 9;
		
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 6;
		m_uiBulletNum[0].SetNumberSprite( &l_spriteInfo, l_ptNumSize );
		m_uiBulletNum[1].SetNumberSprite( &l_spriteInfo, l_ptNumSize );		
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		if( !l_pSprite )
			return;
		
		m_uiActionKey.GetSpriteInfo()->pSprite = l_pSprite;		
		m_uiAttackKey.GetSpriteInfo()->pSprite = l_pSprite;

		// --------------------------------------------------------
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_UNIT );
		m_uiWeaponKey[0].GetSpriteInfo()->pSprite = l_pSprite;
		m_uiWeaponKey[1].GetSpriteInfo()->pSprite = l_pSprite;
		m_uiGetOffUnitKey.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiMoveKey.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiSelectedWeaponAni.GetSpriteInfo()->pSprite = l_pSprite;

		// --------------------------------------------------------
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		m_uiBulletNum[0].GetSpriteInfo()->pSprite = l_pSprite;
		m_uiBulletNum[1].GetSpriteInfo()->pSprite = l_pSprite;
	}
}

void
CGUIUnitControlBoard::SetActionMode( BYTE pi_byMode )
{
	if( pi_byMode == CGUIHotKeyBoard::Peace )
		m_uiActionKey.GetSpriteInfo()->wFrameNo = 0;
	else
		m_uiActionKey.GetSpriteInfo()->wFrameNo = 1;	
}

void
CGUIUnitControlBoard::SetMoveMode( BYTE pi_byMode )
{
	if( pi_byMode == CGUIHotKeyBoard::Walk )
		m_uiMoveKey.GetSpriteInfo()->wFrameNo = 7;
	else
		m_uiMoveKey.GetSpriteInfo()->wFrameNo = 6;
}

void	
CGUIUnitControlBoard::SetAttackMode( BYTE pi_byMode )
{
	if( pi_byMode == CGUIHotKeyBoard::Manual )
		m_uiAttackKey.GetSpriteInfo()->wFrameNo = 5;
	else
		m_uiAttackKey.GetSpriteInfo()->wFrameNo = 4;	
}

void
CGUIUnitControlBoard::SelectWeapon( WeaponType pi_eWeapon )
{
	if( pi_eWeapon == None_Weapon )
		Remove( &m_uiSelectedWeaponAni );

	if( pi_eWeapon >= 2 )
		return;

	m_uiSelectedWeaponAni.SetPos( m_uiWeaponKey[pi_eWeapon].m_ptPos );

	m_uiSelectedWeaponAni.Show( IsShow() );
	Add( &m_uiSelectedWeaponAni );
}

void	
CGUIUnitControlBoard::SetBulletNum( WeaponType pi_eWeapon, WORD pi_wBulletNum )
{
	if( pi_eWeapon >= 2 )
		return;

	m_uiBulletNum[pi_eWeapon].SetNumber( pi_wBulletNum );
	m_uiBulletNum[pi_eWeapon].SetPos( m_uiWeaponKey[pi_eWeapon].m_ptPos.x + m_uiWeaponKey[pi_eWeapon].m_ptSize.x - m_uiBulletNum[pi_eWeapon].m_ptSize.x - 2,
									  m_uiWeaponKey[pi_eWeapon].m_ptPos.y + m_uiWeaponKey[pi_eWeapon].m_ptSize.y - m_uiBulletNum[pi_eWeapon].m_ptSize.y - 2 );
	
	if( pi_wBulletNum > 0 )
	{
		m_uiWeaponKey[pi_eWeapon].GetSpriteInfo()->wFrameNo = pi_eWeapon * 2;
	}
	else
	{
		m_uiWeaponKey[pi_eWeapon].GetSpriteInfo()->wFrameNo = pi_eWeapon * 2 + 1;
	}
}

// 유닛을 탑승했을 경우
void
CGUIUnitControlBoard::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_STROKED )
	{
		if( event.source == &m_uiActionKey )
		{
			PostMessage( gScreen.GetHWND(), WM_AVATAR_TOGGLE_COMBAT_MODE, 0, 0 );
		}
		else if( event.source == &m_uiMoveKey )
		{
			PostMessage( gScreen.GetHWND(), WM_AVATAR_TOGGLE_MOVE_MODE, 0, 0 );
		}
		else if( event.source == &m_uiAttackKey )
		{
			PostMessage( gScreen.GetHWND(), WM_AVATAR_TOGGLE_AUTO_ATTACK, 0, 0 );
		}
		else if( event.source == &m_uiWeaponKey[Main_Weapon] ||
				 event.source == &m_uiBulletNum[Main_Weapon] )
		{
			PostMessage( gScreen.GetHWND(), WM_AVATAR_UNIT_SELECT_WEAPON, 0, 0 );
		}
		else if( event.source == &m_uiWeaponKey[Sub_Weapon] ||
				 event.source == &m_uiBulletNum[Sub_Weapon] )
		{
			PostMessage( gScreen.GetHWND(), WM_AVATAR_UNIT_SELECT_WEAPON, 1, 0 );
		}
		else if( event.source == &m_uiGetOffUnitKey )
		{
			PostMessage( gScreen.GetHWND(), WM_AVATAR_UNIT_DISMOUNT, 0, 0 );
		}
	}
	else if( event.id == MOUSE_HOVERED )
	{
		if( event.source == &m_uiWeaponKey[Main_Weapon] )
		{
			SetTooltip( &m_uiWeaponKey[Main_Weapon], m_uiWeaponKey[Main_Weapon].GetName() );
		}
		else if( event.source == &m_uiWeaponKey[Sub_Weapon] )
		{
			SetTooltip( &m_uiWeaponKey[Sub_Weapon], m_uiWeaponKey[Sub_Weapon].GetName() );
		}
		else if( event.source == &m_uiGetOffUnitKey )
		{
			SetTooltip( &m_uiGetOffUnitKey, m_uiGetOffUnitKey.GetName() );
		}
		else if( event.source == &m_uiActionKey )
		{
			if( g_pUiFuncKeyWindow )
				g_pUiFuncKeyWindow->TooltipSetting( &m_uiActionKey, HOTKEY_ID_ACTION );			
		}
		else if( event.source == &m_uiMoveKey )
		{
			if( g_pUiFuncKeyWindow )
				g_pUiFuncKeyWindow->TooltipSetting( &m_uiMoveKey, HOTKEY_ID_MOVE );			
		}
		else if( event.source == &m_uiAttackKey )
		{
			if( g_pUiFuncKeyWindow )
				g_pUiFuncKeyWindow->TooltipSetting( &m_uiAttackKey, HOTKEY_ID_ATTACK );			
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Boundary          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define	SUBKEY_MIN_SIZE		26
#define	SUBKEY_MAX_SIZE		40

static char s_pTooltipStr[HOTKEY_NUM+1][32] =
{
	"평화/전투", "달리기/걷기", "수동/자동공격", 
	"인벤토리", 
	"캐릭터 정보", "스킬", "포스", "소환", "커뮤니티", "저널", 
	"게임옵션",
	"게임시스템"
};

////////////////////////////////////////////////////////////////////////////
//
// CGUIHotKeyBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIHotKeyBoard::CGUIHotKeyBoard()
{
	m_eBoardType		= Avatar_Board;	

	for( int i=0; i<WIN_NUM; ++i )
		m_pRefWindow[i] = NULL;	

	m_nOnMouseSubkey	= -1;

	for( i=0; i<HOTKEY_NUM; ++i )
		m_pHotKeyStr[i][0] = '\0';
	

	m_pPotionWin		= NULL;
	m_pSFWin			= NULL;
	m_pUnitCtrlBoard	= NULL;

	m_bIsCreated			= FALSE;
}

CGUIHotKeyBoard::~CGUIHotKeyBoard()
{
	if( m_pUnitCtrlBoard )
	{
		delete m_pUnitCtrlBoard;
		m_pUnitCtrlBoard = NULL;
	}
}

void
CGUIHotKeyBoard::Init( void )
{		
	SetSize( gScreen.m_ptSize.x, 41 );
	m_uiBoard.SetSize( gScreen.m_ptSize.x, 41 );	
	m_uiBoardAni.SetSize( 263, 13 );
	m_uiAvatarCtrlBoard.SetSize( gScreen.m_ptSize.x, 41 );

	// char	
	m_uiActionHotkey.SetSize( 36, 34 );
	m_uiMoveHotkey.SetSize( 36, 34 );
	m_uiAttackHotkey.SetSize( 36, 34 );

	m_uiInventoryKey.SetSize( 36, 34 );
	m_uiCharInfoKey.SetSize( 36, 34 );
	m_uiSystemKey.SetSize( 36, 34 );	


	// --------------------------------------------------
	m_uiAvatarCtrlBoard.SetPos( m_ptPos );

	m_uiActionHotkey.SetPos( m_uiAvatarCtrlBoard.m_ptPos.x + 10, m_uiAvatarCtrlBoard.m_ptPos.y + 4 );
	m_uiMoveHotkey.SetPos( m_uiActionHotkey.m_ptPos.x + m_uiActionHotkey.m_ptSize.x , m_uiAvatarCtrlBoard.m_ptPos.y + 4 );
	m_uiAttackHotkey.SetPos( m_uiMoveHotkey.m_ptPos.x + m_uiMoveHotkey.m_ptSize.x , m_uiAvatarCtrlBoard.m_ptPos.y + 4 );

	m_uiInventoryKey.SetPos( m_uiAttackHotkey.m_ptPos.x + m_uiAttackHotkey.m_ptSize.x + 
									60 * (float)gScreen.m_ptSize.x/1024,
							 m_ptPos.y + 4 );
	m_uiCharInfoKey.SetPos( m_uiInventoryKey.m_ptPos.x + m_uiInventoryKey.m_ptSize.x, m_ptPos.y + 4 );
	m_uiSystemKey.SetPos( m_uiCharInfoKey.m_ptPos.x + m_uiCharInfoKey.m_ptSize.x, m_ptPos.y + 4 );

	m_uiBoardAni.SetPos( m_ptPos.x + 89, m_ptPos.y + 18 );


	// --------------------------------------------------
	m_uiActionHotkey.AddMouseEventHandler( this );
	m_uiMoveHotkey.AddMouseEventHandler( this );
	m_uiAttackHotkey.AddMouseEventHandler( this );

	m_uiInventoryKey.AddMouseEventHandler( this );
	m_uiSystemKey.AddMouseEventHandler( this );
	m_uiCharInfoKey.AddMouseEventHandler( this );

	m_uiSubkeyBoard.AddMouseEventHandler( this );	


	Add( &m_uiBoard );
	Add( &m_uiBoardAni );

	m_uiAvatarCtrlBoard.Add( &m_uiActionHotkey );
	m_uiAvatarCtrlBoard.Add( &m_uiMoveHotkey );
	m_uiAvatarCtrlBoard.Add( &m_uiAttackHotkey );

	Add( &m_uiInventoryKey );
	Add( &m_uiSystemKey );
	Add( &m_uiCharInfoKey );		

	SetSprite();

	m_uiBoardAni.SetTotalFrameNum( 16 );
	m_uiBoardAni.SetObjectNumInLine( 1 );
	m_uiBoardAni.SetFPS( 10 );
	m_uiBoardAni.SetStopInterval( 2000 );

	// --------------------------------------------------

	ChangeActionMode( Peace );
	ChangeMoveMode( Walk );
	ChangeAttackMode( Manual );

	SetName( "Hotkey" );

	// defalt control board
	SetAvatarControlBoard();
	
	SetRaceType( 0 );

	m_bIsCreated = TRUE;
}

void
CGUIHotKeyBoard::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );

		l_spriteInfo.wActionNo = 2;		

		l_spriteInfo.wFrameNo = 15;
		m_uiBoard.SetSpriteInfo( &l_spriteInfo );
		float * l_pfUV;
		l_pfUV = m_uiBoard.GetU();
		l_pfUV[1] = (float)m_uiBoard.m_ptSize.x /
					l_spriteInfo.pSprite->GetSpriteXL( l_spriteInfo.wActionNo, l_spriteInfo.wFrameNo );												
		l_pfUV = m_uiBoard.GetV();
		l_pfUV[1] = (float)m_uiBoard.m_ptSize.y /
					l_spriteInfo.pSprite->GetSpriteYL( l_spriteInfo.wActionNo, l_spriteInfo.wFrameNo );

		l_spriteInfo.wFrameNo = 16;
		m_uiBoardAni.SetSpriteInfo( &l_spriteInfo );


		// combat/peace
		l_spriteInfo.wFrameNo = 0;
		m_uiActionHotkey.SetSpriteInfo( &l_spriteInfo );
		
		// run/walk
		l_spriteInfo.wFrameNo = 2;
		m_uiMoveHotkey.SetSpriteInfo( &l_spriteInfo );

		// attack 
		l_spriteInfo.wFrameNo = 5;
		m_uiAttackHotkey.SetSpriteInfo( &l_spriteInfo );

		// window
		l_spriteInfo.wFrameNo = 6;
		m_uiInventoryKey.SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 7;
		m_uiCharInfoKey.SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 8;
		m_uiSystemKey.SetSpriteInfo( &l_spriteInfo );

		// sub window
		for( int i=0; i<MAX_SUB_KEY_NUM; ++i )
		{
			l_spriteInfo.wFrameNo = 9 + i;
			m_uiCharInfoSubKey[i].SetSpriteInfo( &l_spriteInfo );
		}
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		if( !l_pSprite )
			return;

		m_uiBoard.GetSpriteInfo()->pSprite = l_pSprite;		

		m_uiBoardAni.GetSpriteInfo()->pSprite = l_pSprite;		

		// combat/peace
		m_uiActionHotkey.GetSpriteInfo()->pSprite = l_pSprite;

		// run/walk
		m_uiMoveHotkey.GetSpriteInfo()->pSprite = l_pSprite;

		// attack 		
		m_uiAttackHotkey.GetSpriteInfo()->pSprite = l_pSprite;

		// window
		m_uiInventoryKey.GetSpriteInfo()->pSprite = l_pSprite;		
		m_uiCharInfoKey.GetSpriteInfo()->pSprite = l_pSprite;		
		m_uiSystemKey.GetSpriteInfo()->pSprite = l_pSprite;

		// sub window
		for( int i=0; i<MAX_SUB_KEY_NUM; ++i )					
			m_uiCharInfoSubKey[i].GetSpriteInfo()->pSprite = l_pSprite;	
		
		if( g_pUiItemBeltWindow )
			g_pUiItemBeltWindow->SetSprite();

		if( g_pUiSFBeltWindow )
			g_pUiSFBeltWindow->SetSprite();

		if( m_pUnitCtrlBoard )
			m_pUnitCtrlBoard->SetSprite();
	}
}

void
CGUIHotKeyBoard::SetRaceType( BYTE pi_byRaceType )
{
	BYTE l_byRace = pi_byRaceType / 2;

	m_uiSubkeyBoard.RemoveAll();
	
	if( l_byRace == RT_BELLATO )
	{
		for( int i = 0; i < MAX_SUB_KEY_NUM; ++i )
		{
			// 소환키 skip
			if( i == 3 )
				continue;			

			m_uiCharInfoSubKey[i].AddMouseEventHandler( this );
			m_uiCharInfoSubKey[i].AddMouseMotionEventHandler( this );
			m_uiSubkeyBoard.Add( &m_uiCharInfoSubKey[i] );
		}
	}
	else if( l_byRace == RT_CORA )
	{
		for( int i = 0; i < MAX_SUB_KEY_NUM; ++i )
		{
			m_uiCharInfoSubKey[i].AddMouseEventHandler( this );
			m_uiCharInfoSubKey[i].AddMouseMotionEventHandler( this );
			m_uiSubkeyBoard.Add( &m_uiCharInfoSubKey[i] );
		}
	}
	else // accretia
	{
		for( int i = 0; i < MAX_SUB_KEY_NUM; ++i )
		{
			// 포스, 소환키 skip 
			if( i == 2 ||
				i == 3 )
				continue;			

			m_uiCharInfoSubKey[i].AddMouseEventHandler( this );
			m_uiCharInfoSubKey[i].AddMouseMotionEventHandler( this );
			m_uiSubkeyBoard.Add( &m_uiCharInfoSubKey[i] );
		}
	}	
}

// ============================================================================================
// Unit Control Board
void
CGUIHotKeyBoard::SetUnitControlBoard( void )
{
	m_eBoardType = Unit_Board;

	if( !m_pUnitCtrlBoard )
	{
		m_pUnitCtrlBoard = new CGUIUnitControlBoard;
		m_pUnitCtrlBoard->Init();
	}

	Remove( &m_uiAvatarCtrlBoard );
	// 포션벨트와 스킬포스창은 gscreen에 포함 되어있기 때문에 
	// m_uiAvatarCtrlBoard를 제거할때 안보이게 해야한다. 
	if( m_pPotionWin )
		m_pPotionWin->Show( FALSE );
	if( m_pSFWin )
		m_pSFWin->Show( FALSE );


	m_pUnitCtrlBoard->SetPos( m_ptPos );
	m_pUnitCtrlBoard->Show( IsShow() );	
	Add( m_pUnitCtrlBoard, 2 );	
}

void
CGUIHotKeyBoard::SetAvatarControlBoard( void )
{
	m_eBoardType = Avatar_Board;

	if( m_pUnitCtrlBoard )
		Remove( m_pUnitCtrlBoard );

	m_uiAvatarCtrlBoard.SetPos( m_ptPos );
	m_uiAvatarCtrlBoard.Show( IsShow() );	
	Add( &m_uiAvatarCtrlBoard, 2 );
}

// ============================================================================================

void
CGUIHotKeyBoard::ChangeActionMode( ActionMode pi_eMode )
{
	if( m_eBoardType == Avatar_Board )
	{
		if( pi_eMode == Peace )
			m_uiActionHotkey.GetSpriteInfo()->wFrameNo = 0;
		else
			m_uiActionHotkey.GetSpriteInfo()->wFrameNo = 1;						
	}
	else if( m_eBoardType == Unit_Board )
	{
		if( m_pUnitCtrlBoard )
			m_pUnitCtrlBoard->SetActionMode( (BYTE)pi_eMode );
	}	
}

void
CGUIHotKeyBoard::ChangeMoveMode( MoveMode pi_eMode )
{
	if( m_eBoardType == Avatar_Board )
	{
		if( pi_eMode == Walk )
			m_uiMoveHotkey.GetSpriteInfo()->wFrameNo = 2;
		else
			m_uiMoveHotkey.GetSpriteInfo()->wFrameNo = 3;				
	}
	else if( m_eBoardType == Unit_Board )
	{
		if( m_pUnitCtrlBoard )
			m_pUnitCtrlBoard->SetMoveMode( (BYTE)pi_eMode );
	}
}

void
CGUIHotKeyBoard::ChangeAttackMode( AttackMode pi_eMode )
{
	if( m_eBoardType == Avatar_Board )
	{
		if( pi_eMode == Manual )
			m_uiAttackHotkey.GetSpriteInfo()->wFrameNo = 5;
		else
			m_uiAttackHotkey.GetSpriteInfo()->wFrameNo = 4;					
	}
	else if( m_eBoardType == Unit_Board )
	{
		if( m_pUnitCtrlBoard )
			m_pUnitCtrlBoard->SetAttackMode( (BYTE)pi_eMode );
	}
}

// ============================================================================================
// Unit Control
void
CGUIHotKeyBoard::SelectUnitWeapon( CGUIUnitControlBoard::WeaponType pi_eWeapon )
{
	if( m_pUnitCtrlBoard )
		m_pUnitCtrlBoard->SelectWeapon( pi_eWeapon );
}

void
CGUIHotKeyBoard::SetUnitBulletNum( CGUIUnitControlBoard::WeaponType pi_eWeapon, WORD pi_wBulletNum )
{
	if( m_pUnitCtrlBoard )
		m_pUnitCtrlBoard->SetBulletNum( pi_eWeapon, pi_wBulletNum );
}

// ============================================================================================

void
CGUIHotKeyBoard::SetRefWindow( Window_Hotkey pi_eHotkey, CGUIWindow * pi_pWindow )
{
	if( pi_eHotkey >= WIN_NUM )
		return;

	m_pRefWindow[pi_eHotkey] = pi_pWindow;
}

void
CGUIHotKeyBoard::SetBeltWindow( CGUIItemBeltWindow * pi_pPotionWin, CGUIForceBeltWindow * pi_pSFWin )
{
	m_pPotionWin = pi_pPotionWin;
	m_pSFWin = pi_pSFWin;
	

	m_pPotionWin->SetPos( m_ptPos.x + m_ptSize.x - m_pPotionWin->m_ptSize.x - 
									m_pSFWin->m_ptSize.x - 50 * (float)gScreen.m_ptSize.x/1024,
							  m_ptPos.y + m_ptSize.y - m_pPotionWin->m_ptSize.y );
	m_pSFWin->SetPos( m_ptPos.x + m_ptSize.x - m_pSFWin->m_ptSize.x - 5,
						  m_ptPos.y + m_ptSize.y - m_pSFWin->m_ptSize.y );

	
	// 벨트창이 hotkeyboard에 포함돼서 더이상 일반 윈도우가 아니다. 그래서 gscreen에서 제거한다.
	gScreen.Remove( m_pPotionWin );
	gScreen.Remove( m_pSFWin );
	

	m_pPotionWin->Show( m_uiAvatarCtrlBoard.IsShow() );
	m_pSFWin->Show( m_uiAvatarCtrlBoard.IsShow() );
	m_uiAvatarCtrlBoard.Add( m_pPotionWin );	
	m_uiAvatarCtrlBoard.Add( m_pSFWin );
}

void
CGUIHotKeyBoard::SetHotKeyStr( BYTE pi_byIndex, char * pi_pStr )
{
	if( pi_byIndex >= HOTKEY_NUM || !pi_pStr )
		return;

	strncpy( m_pHotKeyStr[pi_byIndex], pi_pStr, 31 );
	m_pHotKeyStr[pi_byIndex][31] = '\0';
}

void
CGUIHotKeyBoard::TooltipSetting( CGUIObject * pi_pObject, BYTE pi_byIndex )
{
	if( pi_byIndex >= HOTKEY_NUM )
		return;
	
	if( m_pHotKeyStr[pi_byIndex][0] == '\0' )
	{
		SetTooltip( pi_pObject, s_pTooltipStr[pi_byIndex] );
	}
	else
	{
		char buf[64];
		sprintf( buf, "%s\n단축 '%s'", s_pTooltipStr[pi_byIndex], m_pHotKeyStr[pi_byIndex] );
		SetTooltip( pi_pObject, buf );
	}
	
}

// ----------------------------------------------------------------------------------------------
// test code
/*
static BOOL s_bIsInited = FALSE;

#include "GUIChatWindow.h"
class CGUIChatWindow;
class CGUIChatInputWindow;
class CGUIItemBeltWindow;
class CGUIForceBeltWindow;

extern CGUIChatWindow * g_pUiChatWindow;
extern CGUIChatInputWindow *	g_pUiChatInputWindow;
extern CGUIItemBeltWindow * g_pUiItemBeltWindow;
extern CGUIForceBeltWindow *	g_pUiSFBeltWindow;

void
CGUIHotKeyBoard::Update( void )
{
	if( !s_bIsInited )
	{
		g_pUiChatWindow->SetPos( gScreen.m_ptPos.x, gScreen.m_ptSize.y - g_pUiChatWindow->m_ptSize.y - m_ptSize.y );
		g_pUiChatInputWindow->SetPos( gScreen.m_ptPos.x + 5, gScreen.m_ptSize.y - g_pUiChatInputWindow->m_ptSize.y - m_ptSize.y - 5 );

		g_pUiItemBeltWindow->Close();
		g_pUiSFBeltWindow->Close();
		SetBeltWindow( g_pUiItemBeltWindow, g_pUiSFBeltWindow );

		SetPos( gScreen.m_ptPos.x, gScreen.m_ptSize.y - m_ptSize.y );
		
		char buf[128];		
		for( int i=0; i<100000; ++i )
		{
			sprintf( buf, "[%d] this is a test", i );
			g_pUiChatWindow->InsertText( NULL, buf, CGUIChatWindow::FS_SYSTEM, CGUIChatWindow::ALL );
		}

		s_bIsInited = TRUE;
	}

	CGUIWindow::Update();
}

static float s_fScale = 0.f;

BOOL
CGUIHotKeyBoard::Draw( void )
{
	if( !IsShow() )
		return false;

	CGUIWindow::Draw();

//	char buf[32];
//	sprintf( buf, "scale = %f", s_fScale );
//	DrawR3Hangul( 100, 600, buf, -1L, R3_HAN_OUTLINE );

	return true;
}
*/
// ----------------------------------------------------------------------------------------------

void
CGUIHotKeyBoard::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );
	
	if( event.id == MOUSE_STROKED )
	{	
		if( event.source == &m_uiActionHotkey )
		{
			PostMessage( gScreen.GetHWND(), WM_AVATAR_TOGGLE_COMBAT_MODE, 0, 0 );			
		}
		else if( event.source == &m_uiMoveHotkey )
		{
			PostMessage( gScreen.GetHWND(), WM_AVATAR_TOGGLE_MOVE_MODE, 0, 0 );			
		}
		else if( event.source == &m_uiAttackHotkey )
		{
			PostMessage( gScreen.GetHWND(), WM_AVATAR_TOGGLE_AUTO_ATTACK, 0, 0 );
		}
		else if( event.source == &m_uiInventoryKey )
		{
			if( !m_pRefWindow[Inventory]->IsShow() )
				m_pRefWindow[Inventory]->Open();
			else
				m_pRefWindow[Inventory]->Close();				
		}
		else if( event.source == &m_uiSystemKey )
		{
			if( !m_pRefWindow[Menu]->IsShow() )
				m_pRefWindow[Menu]->Open();
			else
				m_pRefWindow[Menu]->Close();
		}
		else if( event.source == &m_uiCharInfoKey )
		{
			if( !IsOpenedCharInfoSubkey() )						
				OpenCharInfoSubkey();
		}
		else
		{			
			//// char info sub window hotkey
			for( int i=0; i<MAX_SUB_KEY_NUM; ++i )
			{
				if( event.source == &m_uiCharInfoSubKey[i] )
				{
					// i+1 부터 subkey다.
					if( !m_pRefWindow[i+1] ) 
						return;

					if( m_pRefWindow[i+1]->IsShow() )					
						m_pRefWindow[i+1]->Close();						
					
					else
						m_pRefWindow[i+1]->Open();					

					return;
				}
			}			
			
		}
	}
	else if( event.id == MOUSE_HOVERED )
	{
		if( event.source == &m_uiActionHotkey )
		{			
			// set tooltip
			TooltipSetting( &m_uiActionHotkey, HOTKEY_ID_ACTION );			
		}
		else if( event.source == &m_uiMoveHotkey )
		{
			TooltipSetting( &m_uiMoveHotkey, HOTKEY_ID_MOVE );			
		}
		else if( event.source == &m_uiAttackHotkey )
		{
			TooltipSetting( &m_uiAttackHotkey, HOTKEY_ID_ATTACK );				
		}		
		else if( event.source == &m_uiInventoryKey )
		{
			TooltipSetting( &m_uiInventoryKey, HOTKEY_ID_INVENTORY );			
		}
		else if( event.source == &m_uiSystemKey )
		{
			TooltipSetting( &m_uiSystemKey, HOTKEY_ID_OPTION );			
		}	
		else if( event.source == &m_uiCharInfoKey )
		{
			SetTooltip( &m_uiCharInfoKey, s_pTooltipStr[HOTKEY_NUM] );			
		}
		else
		{
			// sub key
			for( int i=0; i<MAX_SUB_KEY_NUM; ++i )
			{
				if( event.source == &m_uiCharInfoSubKey[i] )
				{
					m_nOnMouseSubkey = m_uiSubkeyBoard.GetAddedObjectIndex( &m_uiCharInfoSubKey[i] );

					ScaleSubkey();

					// set tooltip
					TooltipSetting( &m_uiCharInfoSubKey[i], HOTKEY_ID_CHARINFO + i );

					return;
				}
			}
		}

		m_nOnMouseSubkey = -1;
	}
	else if( event.id == MOUSE_LEAVED )
	{
		if( m_uiSubkeyBoard.IsShow() &&
			!m_uiSubkeyBoard.IsCrossed( event.xPos, event.yPos ) )
		{
			// 열었을 때의 초기 상태로 만든다
			//OpenCharInfoSubkey();

			CloseCharInfoSubkey();

			m_nOnMouseSubkey = -1;			
		}
	}
	else if( event.id == MOUSE_RELEASED )
	{
		// 캐릭터정보창을 누르고 나서 뗄때 마우스가 
		// 서브키위에 없을수 있다.
		if( event.source == &m_uiCharInfoKey )
		{
			if( m_uiSubkeyBoard.IsShow() &&
				!m_uiSubkeyBoard.IsCrossed( event.xPos, event.yPos ) )	
			{
				CloseCharInfoSubkey();

				m_nOnMouseSubkey = -1;				
			}
		}
	}

}


void
CGUIHotKeyBoard::MouseMotionEvent( _mouse_motion_event & event )
{
	if( event.id == MOUSE_MOVE )
	{
		if( m_nOnMouseSubkey == -1 )
			return;

		if( event.xMove == 0 )
			return;

		ScaleSubkey();		
	}
}

void
CGUIHotKeyBoard::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{
	// 포션벨트창이 단축키창에 포함되어서 입력창 이벤트가 여기로 온다.
	if( pi_pDlg->GetUserDefinedID() == INPUT_DLG_ID &&
		pi_pDlg->GetResultMsg() == MSG_OK )
	{
		if( !g_pUiItemBeltWindow->ProcNumInputDlg( (CGUINumInputDlg *)pi_pDlg ) )
			return;
	}	

	CGUIWindow::ClosingDialogBox( pi_pDlg );
}

BOOL
CGUIHotKeyBoard::IsCrossed( LONG pi_nXPos, LONG pi_nYPos )
{
	if( CGUIObject::IsCrossed( pi_nXPos, pi_nYPos ) )
		return TRUE;

	if( m_uiSubkeyBoard.IsShow() )
	{
		for( int i=0; i<MAX_SUB_KEY_NUM; ++i )
		{
			if( m_uiCharInfoSubKey[i].IsCrossed( pi_nXPos, pi_nYPos ) )
				return TRUE;
		}
	}	
		
	return FALSE;
}

BOOL
CGUIHotKeyBoard::IsOpenedCharInfoSubkey( void )
{
	return IsAddedObject( &m_uiSubkeyBoard );	
}

void
CGUIHotKeyBoard::OpenCharInfoSubkey( void )
{
	// 메인메뉴를 안보이게한다.
	m_uiInventoryKey.Show( FALSE );
	m_uiSystemKey.Show( FALSE );
	m_uiCharInfoKey.Show( FALSE );

	m_uiSubkeyBoard.Show( IsShow() );
	Add( &m_uiSubkeyBoard );
	
//	m_uiSubkeyBoard.SetSize( SUBKEY_MIN_SIZE * 5, SUBKEY_MIN_SIZE );
	m_uiSubkeyBoard.SetSize( SUBKEY_MIN_SIZE * m_uiSubkeyBoard.GetAddedObjectNum(), SUBKEY_MAX_SIZE );

	m_uiSubkeyBoard.SetPos( m_uiInventoryKey.m_ptPos.x + ( m_uiInventoryKey.m_ptSize.x * 3 - m_uiSubkeyBoard.m_ptSize.x ) / 2,
							m_ptPos.y + m_ptSize.y - m_uiSubkeyBoard.m_ptSize.y - 5 );

	CGUIObject * l_pSubKeyObj;
	for( int i=0; i < m_uiSubkeyBoard.GetAddedObjectNum(); ++i )
	{
		l_pSubKeyObj = m_uiSubkeyBoard.GetAddedObject( i );

		l_pSubKeyObj->SetSize( SUBKEY_MIN_SIZE, SUBKEY_MIN_SIZE );
		l_pSubKeyObj->SetPos( m_uiSubkeyBoard.m_ptPos.x + i * SUBKEY_MIN_SIZE, 
							  m_uiSubkeyBoard.m_ptPos.y + ( m_uiSubkeyBoard.m_ptSize.y - l_pSubKeyObj->m_ptSize.y ) / 2 );
	}
}

void
CGUIHotKeyBoard::CloseCharInfoSubkey( void )
{	
	m_uiInventoryKey.Show( IsShow() );
	m_uiSystemKey.Show( IsShow() );
	m_uiCharInfoKey.Show( IsShow() );

	m_uiSubkeyBoard.Show( FALSE );
	Remove( &m_uiSubkeyBoard );	
}

/*
void
CGUIHotKeyBoard::ScaleSubkey( void )
{
	float	l_fSizeRate;
	int		l_nPrevCenterPosX;	
	
	for( int i=0; i<5; ++i )
	{
		//l_nPrevCenterPosX = m_uiSubkeyBoard.m_ptPos.x + SUBKEY_MIN_SIZE * i + (float)SUBKEY_MIN_SIZE / 2;
		l_nPrevCenterPosX = m_uiCharInfoSubKey[i].m_ptPos.x + m_uiCharInfoSubKey[i].m_ptSize.x / 2;
		l_fSizeRate = 1.f - (float)abs( gMouseDev.GetX() - l_nPrevCenterPosX ) / 63;		

		if( l_fSizeRate > 1.f )
			l_fSizeRate = 1.f;
		else if( l_fSizeRate < 0.f )
			l_fSizeRate = 0.f;	

		int l_nNewSize = SUBKEY_MIN_SIZE + ( SUBKEY_MAX_SIZE - SUBKEY_MIN_SIZE ) * l_fSizeRate;
		m_uiCharInfoSubKey[i].SetSize( SUBKEY_MIN_SIZE + ( SUBKEY_MAX_SIZE - SUBKEY_MIN_SIZE ) * l_fSizeRate,
									   SUBKEY_MIN_SIZE + ( SUBKEY_MAX_SIZE - SUBKEY_MIN_SIZE ) * l_fSizeRate );			

		if( i == 0 )
			m_uiCharInfoSubKey[i].SetPos( m_uiSubkeyBoard.m_ptPos.x,
										  m_uiSubkeyBoard.m_ptPos.y + m_uiSubkeyBoard.m_ptSize.y - m_uiCharInfoSubKey[i].m_ptSize.y );		
		else
			m_uiCharInfoSubKey[i].SetPos( m_uiCharInfoSubKey[i-1].m_ptPos.x + m_uiCharInfoSubKey[i-1].m_ptSize.x,
										  m_uiSubkeyBoard.m_ptPos.y + m_uiSubkeyBoard.m_ptSize.y - m_uiCharInfoSubKey[i].m_ptSize.y );
	}		

	// reset m_uiSubkeyBoard
	// m_uiSubkeyBoard에 포함된 object에 영향을 주면 안되기 때문에 setpos()함수를 쓰면 안된다.
	m_uiSubkeyBoard.m_ptPos.x = m_uiCharInfoSubKey[0].m_ptPos.x;
	m_uiSubkeyBoard.m_ptPos.y = m_uiCharInfoSubKey[m_nOnMouseSubkey].m_ptPos.y;

	m_uiSubkeyBoard.SetSize( m_uiCharInfoSubKey[4].m_ptPos.x + m_uiCharInfoSubKey[4].m_ptSize.x - m_uiCharInfoSubKey[0].m_ptPos.x,
							 m_uiCharInfoSubKey[m_nOnMouseSubkey].m_ptSize.y );
}
*/

/*
void
CGUIHotKeyBoard::ScaleSubkey( void )
{
	float l_fSizeRate;
	int	l_nPrevCenterPosX;	

	int l_ptPrevOnObjPosX;
	l_ptPrevOnObjPosX = m_uiCharInfoSubKey[m_nOnMouseSubkey].m_ptPos.x + 
						m_uiCharInfoSubKey[m_nOnMouseSubkey].m_ptSize.x / 2;

	int l_nNewSize;
	float l_fTemp;
	
	for( int i=0; i<5; ++i )
	{		
		l_nPrevCenterPosX = m_uiCharInfoSubKey[i].m_ptPos.x + m_uiCharInfoSubKey[i].m_ptSize.x / 2;
		l_fTemp = (int)(( abs( gMouseDev.GetX() - l_nPrevCenterPosX ) / 63.f ) * 10) / 10.f;
		l_fSizeRate = 1.f - l_fTemp;

		if( l_fSizeRate > 1.f )
			l_fSizeRate = 1.f;
		else if( l_fSizeRate < 0.f )
			l_fSizeRate = 0.f;		
		

		l_nNewSize = SUBKEY_MIN_SIZE + ( SUBKEY_MAX_SIZE - SUBKEY_MIN_SIZE ) * l_fSizeRate;
		m_uiCharInfoSubKey[i].SetSize( l_nNewSize, l_nNewSize );
	}	

	m_uiCharInfoSubKey[m_nOnMouseSubkey].SetPos( l_ptPrevOnObjPosX - m_uiCharInfoSubKey[m_nOnMouseSubkey].m_ptSize.x / 2,													
												 m_uiSubkeyBoard.m_ptPos.y + m_uiSubkeyBoard.m_ptSize.y - 
													m_uiCharInfoSubKey[m_nOnMouseSubkey].m_ptSize.y );		

	if( m_nOnMouseSubkey > 0 )
	{		
		for( int j=m_nOnMouseSubkey-1; j>=0; --j )					
			m_uiCharInfoSubKey[j].SetPos( m_uiCharInfoSubKey[j+1].m_ptPos.x - m_uiCharInfoSubKey[j].m_ptSize.x,
										  m_uiSubkeyBoard.m_ptPos.y + m_uiSubkeyBoard.m_ptSize.y - m_uiCharInfoSubKey[j].m_ptSize.y );
	}

	if( m_nOnMouseSubkey < 4 )
	{	
		for( int j=m_nOnMouseSubkey+1; j<5; ++j )	
			m_uiCharInfoSubKey[j].SetPos( m_uiCharInfoSubKey[j-1].m_ptPos.x + m_uiCharInfoSubKey[j-1].m_ptSize.x,
										  m_uiSubkeyBoard.m_ptPos.y + m_uiSubkeyBoard.m_ptSize.y - m_uiCharInfoSubKey[j].m_ptSize.y );
	}	

	// reset m_uiSubkeyBoard
	// m_uiSubkeyBoard에 포함된 object에 영향을 주면 안되기 때문에 setpos()함수를 쓰면 안된다.
	m_uiSubkeyBoard.m_ptPos.x = m_uiCharInfoSubKey[0].m_ptPos.x - 10; // m_uiSubkeyBoard 여분으로 양쪽에 10정도 남겨둔다
	m_uiSubkeyBoard.m_ptPos.y = m_uiCharInfoSubKey[m_nOnMouseSubkey].m_ptPos.y;

	m_uiSubkeyBoard.SetSize( m_uiCharInfoSubKey[4].m_ptPos.x + m_uiCharInfoSubKey[4].m_ptSize.x - m_uiCharInfoSubKey[0].m_ptPos.x + 20,
							 m_uiCharInfoSubKey[m_nOnMouseSubkey].m_ptSize.y );
}
*/

void
CGUIHotKeyBoard::ScaleSubkey( void )
{
	float l_fSizeRate;
	int	l_nPrevCenterPosX;	

	int l_nNewSize;
	float l_fTemp;

	CGUIObject * l_pMouseOnSubKeyObj;
	l_pMouseOnSubKeyObj = m_uiSubkeyBoard.GetAddedObject( m_nOnMouseSubkey );

	int l_ptPrevOnObjPosX;	
	l_ptPrevOnObjPosX = l_pMouseOnSubKeyObj->m_ptPos.x + 
						l_pMouseOnSubKeyObj->m_ptSize.x / 2;
	int l_nPrevOnObjSizeX;
	l_nPrevOnObjSizeX = l_pMouseOnSubKeyObj->m_ptSize.x;

	// ---------------------------------------------------------------------
	l_nPrevCenterPosX = l_pMouseOnSubKeyObj->m_ptPos.x + l_pMouseOnSubKeyObj->m_ptSize.x / 2;
	l_fTemp = (int)(( abs( gMouseDev.GetX() - l_nPrevCenterPosX ) / 63.f ) * 10) / 10.f;
	l_fSizeRate = 1.f - l_fTemp;

	if( l_fSizeRate > 1.f )
		l_fSizeRate = 1.f;
	else if( l_fSizeRate < 0.f )
		l_fSizeRate = 0.f;	

	l_nNewSize = SUBKEY_MIN_SIZE + ( SUBKEY_MAX_SIZE - SUBKEY_MIN_SIZE ) * l_fSizeRate;
	l_pMouseOnSubKeyObj->SetSize( l_nNewSize, l_nNewSize );

	// ---------------------------------------------------------------------
	if( l_nPrevOnObjSizeX == l_pMouseOnSubKeyObj->m_ptSize.x )
		return;	
	

	CGUIObject * l_pSubKeyObj;

	for( int i=0; i<m_uiSubkeyBoard.GetAddedObjectNum(); ++i )
	{
		if( i == m_nOnMouseSubkey )
			continue;

		l_pSubKeyObj = m_uiSubkeyBoard.GetAddedObject( i );

		l_nPrevCenterPosX = l_pSubKeyObj->m_ptPos.x + l_pSubKeyObj->m_ptSize.x / 2;
		l_fTemp = (int)(( abs( gMouseDev.GetX() - l_nPrevCenterPosX ) / 63.f ) * 10) / 10.f;
		l_fSizeRate = 1.f - l_fTemp;

		if( l_fSizeRate > 1.f )
			l_fSizeRate = 1.f;
		else if( l_fSizeRate < 0.f )
			l_fSizeRate = 0.f;				

		l_nNewSize = SUBKEY_MIN_SIZE + ( SUBKEY_MAX_SIZE - SUBKEY_MIN_SIZE ) * l_fSizeRate;
		l_pSubKeyObj->SetSize( l_nNewSize, l_nNewSize );
	}	


	// -----------------------------------------------------------------------

	l_pMouseOnSubKeyObj->SetPos( l_ptPrevOnObjPosX - l_pMouseOnSubKeyObj->m_ptSize.x / 2,													
												 m_uiSubkeyBoard.m_ptPos.y + m_uiSubkeyBoard.m_ptSize.y - 
													l_pMouseOnSubKeyObj->m_ptSize.y );		

	CGUIObject * l_pNextSubKeyObj;

	if( m_nOnMouseSubkey > 0 )
	{		
		for( int j=m_nOnMouseSubkey-1; j>=0; --j )	
		{
			l_pSubKeyObj = m_uiSubkeyBoard.GetAddedObject( j );
			l_pNextSubKeyObj = m_uiSubkeyBoard.GetAddedObject( j + 1 );

			l_pSubKeyObj->SetPos( l_pNextSubKeyObj->m_ptPos.x - l_pSubKeyObj->m_ptSize.x,
										  m_uiSubkeyBoard.m_ptPos.y + m_uiSubkeyBoard.m_ptSize.y - l_pSubKeyObj->m_ptSize.y );
		}
	}

	if( m_nOnMouseSubkey < m_uiSubkeyBoard.GetAddedObjectNum() - 1 )
	{	
		for( int j=m_nOnMouseSubkey+1; j<m_uiSubkeyBoard.GetAddedObjectNum(); ++j )
		{
			l_pSubKeyObj = m_uiSubkeyBoard.GetAddedObject( j );
			l_pNextSubKeyObj = m_uiSubkeyBoard.GetAddedObject( j - 1 );

			l_pSubKeyObj->SetPos( l_pNextSubKeyObj->m_ptPos.x + l_pNextSubKeyObj->m_ptSize.x,
								  m_uiSubkeyBoard.m_ptPos.y + m_uiSubkeyBoard.m_ptSize.y - l_pSubKeyObj->m_ptSize.y );
		}
	}	

	// reset m_uiSubkeyBoard
	// m_uiSubkeyBoard에 포함된 object에 영향을 주면 안되기 때문에 setpos()함수를 쓰면 안된다.
	m_uiSubkeyBoard.m_ptPos.x = m_uiCharInfoSubKey[0].m_ptPos.x - 10; // m_uiSubkeyBoard 여분으로 양쪽에 10정도 남겨둔다
	m_uiSubkeyBoard.m_ptPos.y = l_pMouseOnSubKeyObj->m_ptPos.y;

	l_pSubKeyObj = m_uiSubkeyBoard.GetAddedObject( m_uiSubkeyBoard.GetAddedObjectNum() - 1 );
	m_uiSubkeyBoard.SetSize( l_pSubKeyObj->m_ptPos.x + l_pSubKeyObj->m_ptSize.x - m_uiCharInfoSubKey[0].m_ptPos.x + 20,
							 l_pMouseOnSubKeyObj->m_ptSize.y );
}