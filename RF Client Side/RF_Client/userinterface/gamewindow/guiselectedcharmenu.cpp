////////////////////////////////////////////////////////////////////////////
//
// CGUISelectedCharMenu Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUISelectedCharMenu.h"
#include "../SpriteMgr.h"
#include "../GUIController/GUIScreen.h"

#include "../../DefineMessage.h"

BOOL EP_WhisperToPlayer( void );
BOOL EP_ChaseTarget( void );

////////////////////////////////////////////////////////////////////////////

#define	LABEL_COLOR				D3DCOLOR_XRGB(161,161,161) 
#define	LABEL_COLOR_HOVERED		D3DCOLOR_XRGB(255,255,255) 
#define	LABEL_COLOR_DISABLED	D3DCOLOR_XRGB(108,108,108) 

CGUISelectedCharMenu::CGUISelectedCharMenu()
{
	m_byMenuType	= 0xFF;

	m_bIsCreated	= FALSE;
}

CGUISelectedCharMenu::~CGUISelectedCharMenu()
{

}

void
CGUISelectedCharMenu::Init( void )
{
	SetSize( 106, 170 );	
	
	m_uiCharNameBase.SetSize( 94, 14 );
	for( int i=0; i<PLAYER_MENU_NUM; ++i )
		m_uiFuncMenu[i].SetSize( 94, 22 );

	// ----------------------------------------------------------	
	m_uiCharNameBase.SetPos( m_ptPos.x + 6, m_ptPos.y + 4 );
	m_uiCharName.SetPos( m_ptPos.x, m_ptPos.y + 5 );
	

	Add( &m_uiCharNameBase );
	Add( &m_uiCharName );

	for( i = 0; i < MAX_MENU_NUM; ++i )
	{
		m_uiFuncMenu[i].AddMouseEventHandler( this );		
	}

	// ----------------------------------------------------------
	SetSprite();
	
	SetARGB( 0xC0262626 );
	m_uiCharNameBase.SetARGB( 0xC0080808 );

	m_uiCharName.GetFont()->dwColor = D3DCOLOR_XRGB( 255, 255, 255 );		


	SetMenuType( PLAYER );	

	m_bIsCreated = TRUE;
}

void
CGUISelectedCharMenu::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );

		l_spriteInfo.wActionNo = 6;		
		
		for( int i = 0; i < MAX_MENU_NUM; ++i )
		{
			l_spriteInfo.wFrameNo = 0;
			m_uiFuncMenu[i].SetSpriteInfoEx( &l_spriteInfo );
			l_spriteInfo.wFrameNo = 1;
			m_uiFuncMenu[i].SetSpriteInfoEx( HOVERED, &l_spriteInfo );
			l_spriteInfo.wFrameNo = 2;
			m_uiFuncMenu[i].SetSpriteInfoEx( PRESSED, &l_spriteInfo );
			l_spriteInfo.wFrameNo = 3;
			m_uiFuncMenu[i].SetSpriteInfoEx( DISABLED, &l_spriteInfo );
		}
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		if( !l_pSprite )
			return;		

		for( int i=0; i<MAX_MENU_NUM; ++i )
			m_uiFuncMenu[i].SetSprite( l_pSprite );
	}

}

void
CGUISelectedCharMenu::SetMenuType( BYTE pi_byMenuType )
{
//	if( m_byMenuType == pi_byMenuType )
//		return;	

	if( pi_byMenuType > GUARD_TOWER )
		return;

	m_byMenuType = pi_byMenuType;

	// clear
	for( int i = 0; i < MAX_MENU_NUM; ++i )	
		Remove( &m_uiFuncMenu[i] );
	

	if( m_byMenuType == PLAYER )
	{
		m_uiFuncMenu[Whisper].SetLabel	( "1:1 대화" );
		m_uiFuncMenu[Trade].SetLabel	( "거래하기" );
		m_uiFuncMenu[Party].SetLabel	( "파티권유" );
		m_uiFuncMenu[Guild].SetLabel	( "길드가입" );
		m_uiFuncMenu[Friend].SetLabel	( "친구등록" );
		m_uiFuncMenu[Follow].SetLabel	( "따라가기" );		

		SetSize( 106, 22 + 24 * PLAYER_MENU_NUM + 4 );

		for( i = 0; i < PLAYER_MENU_NUM; ++i )
		{
			m_uiFuncMenu[i].SetPos( m_ptPos.x + 6, m_ptPos.y + 22 + i * 24 );
			Add( &m_uiFuncMenu[i] );
		}

		EnableButton( Whisper, TRUE );
		EnableButton( Trade, TRUE );
		EnableButton( Party, TRUE );	
		EnableButton( Friend, FALSE );
		EnableButton( Follow, TRUE );	
		EnableButton( Guild, FALSE );

		// init party menu
		SetPartyMenu( None );
	}
	else if( m_byMenuType == UNIT )
	{
		for( i = 0; i < MAX_MENU_NUM; ++i )
			m_uiFuncMenu[i].SetLabel( NULL );

		m_uiFuncMenu[Riding].SetLabel	( "탑 승" );
		m_uiFuncMenu[Traction].SetLabel	( "견 인" );
		m_uiFuncMenu[Custody].SetLabel	("보 관" );		

		SetSize( 106, 22 + 24 * UNIT_MENU_NUM + 4 );

		for( i = 0; i < UNIT_MENU_NUM; ++i )
		{
			m_uiFuncMenu[i].SetPos( m_ptPos.x + 6, m_ptPos.y + 22 + i * 24 );
			Add( &m_uiFuncMenu[i] );
		}

		EnableButton( Riding, TRUE );
		EnableButton( Traction, FALSE );
		EnableButton( Custody, FALSE );		
	}
	else if( m_byMenuType == GUARD_TOWER )
	{
		for( i = 0; i < MAX_MENU_NUM; ++i )
			m_uiFuncMenu[i].SetLabel( NULL );

		m_uiFuncMenu[GetBack].SetLabel( "회 수" );		
		
		SetSize( 106, 22 + 24 * GUARD_TOWER_MENU_NUM + 4 );

		for( i = 0; i < GUARD_TOWER_MENU_NUM; ++i )
		{
			m_uiFuncMenu[i].SetPos( m_ptPos.x + 6, m_ptPos.y + 22 + i * 24 );
			Add( &m_uiFuncMenu[i] );
		}

		EnableButton( GetBack, TRUE );
	}
}

void
CGUISelectedCharMenu::SetCharName( char * pi_pName )
{
	m_uiCharName.SetString( pi_pName );

	// set repos
	m_uiCharName.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiCharName.m_ptSize.x ) / 2, m_uiCharName.m_ptPos.y );
}

void
CGUISelectedCharMenu::SetPartyMenu( PartyMenu pi_eMenu )
{
	if( m_byMenuType != PLAYER )
		return;

	m_ePartyMenu = pi_eMenu;	
	
	if( m_ePartyMenu == Invite )
	{	
		// 파티권유 메뉴		
		m_uiFuncMenu[Party].SetLabel( "파티권유" );		
		EnableButton( Party, TRUE );		
	}
	else if( m_ePartyMenu == Join )
	{
		// 파티가입 메뉴		
		m_uiFuncMenu[Party].SetLabel( "파티가입" );				
		EnableButton( Party, TRUE );
	}
	else // None
	{	
		m_uiFuncMenu[Party].SetLabelColor( D3DCOLOR_XRGB( 119, 119, 119 ) );
		EnableButton( Party, FALSE );
	}	
}


void
CGUISelectedCharMenu::EnableButton( BYTE pi_byMenuIndex, BOOL pi_bEnable )
{
	if( pi_byMenuIndex >= MAX_MENU_NUM )
		return;

	SPRITE_INFO l_spriteInfo;
	memcpy( &l_spriteInfo, m_uiFuncMenu[pi_byMenuIndex].GetSpriteInfo(), sizeof(SPRITE_INFO) );

	if( pi_bEnable )
	{	
		m_uiFuncMenu[pi_byMenuIndex].Enable();	


		m_uiFuncMenu[pi_byMenuIndex].SetLabelColor( LABEL_COLOR );
		m_uiFuncMenu[pi_byMenuIndex].SetLabelHoveredColor( LABEL_COLOR_HOVERED );		
	}
	else
	{	
		m_uiFuncMenu[pi_byMenuIndex].Disable();		

		m_uiFuncMenu[pi_byMenuIndex].SetLabelColor( LABEL_COLOR_DISABLED );
	}	
}

void
CGUISelectedCharMenu::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );	

	if( event.id == MOUSE_CLICKED )
	{
		if( m_byMenuType == PLAYER )
		{

			if( event.source == &m_uiFuncMenu[Whisper] )
			{
				// ################## 코드 추가 시킬 곳 ###########################
				// 1:1 대화
				EP_WhisperToPlayer();
				Close();
			}
			if( event.source == &m_uiFuncMenu[Trade] )
			{		
				// 거래 요청
				PostUIMessage( WM_PLAYER_TRADE_REQUEST, 0 );			
			}
			else if( event.source == &m_uiFuncMenu[Party] )
			{			
				// 파티 권유
				if( m_ePartyMenu == Invite )
				{				
					PostUIMessage( WM_PARTY_INVITE_OTHER_PLAYER, 0 );
					Close();
				}	
				// 파티 가입			
				else if( m_ePartyMenu == Join )
				{			
					PostUIMessage( WM_PARTY_JOIN_OTHER_PARTY, 0 );
					Close();
				}
			}
			else if( event.source == &m_uiFuncMenu[Follow] )
			{
				EP_ChaseTarget();
				Close();
			}
		}
		else if( m_byMenuType == UNIT )
		{
			if( event.source == &m_uiFuncMenu[Riding] )
			{
				// 탑승
				PostUIMessage( WM_AVATAR_UNIT_MOUNT, 0 );
				Close();
			}
			else if( event.source == &m_uiFuncMenu[Traction] )
			{
				// 견인
				PostUIMessage( WM_AVATAR_UNIT_TRACTION, 0 );
				Close();
			}
			else if( event.source == &m_uiFuncMenu[Custody] )
			{
				// 보관
				PostUIMessage( WM_AVATAR_UNIT_KEEPING, 0 );
				Close();
			}
		}
		else if( m_byMenuType == GUARD_TOWER )
		{
			if( event.source == &m_uiFuncMenu[GetBack] )
			{
				// 회수
				PostUIMessage( WM_AVATAR_GUARDTOWER_TAKEIN, 0 );
				Close();
			}
		}
	}
}

BOOL	
CGUISelectedCharMenu::Draw( void )
{
	if( !IsShow() )
		return FALSE;

#define	LINE_COLOR	0xC0080808

	CGUIContainer::Draw();

	// top line
	Draw2DRectangle( m_ptPos.x + 1, m_ptPos.y + 1,
					 m_ptPos.x + m_ptSize.x - 2, m_ptPos.y + 3,
					 LINE_COLOR );
	// left line
	Draw2DRectangle( m_ptPos.x + 1, m_ptPos.y + 2,
					 m_ptPos.x + 3, m_ptPos.y + m_ptSize.y - 2,
					 LINE_COLOR );					 
	// right line
	Draw2DRectangle( m_ptPos.x + m_ptSize.x - 3, m_ptPos.y + 2,
					 m_ptPos.x + m_ptSize.x - 1, m_ptPos.y + m_ptSize.y - 2,
					 LINE_COLOR );					
	// bottom line
	Draw2DRectangle( m_ptPos.x + 1, m_ptPos.y + m_ptSize.y - 3,
					 m_ptPos.x + m_ptSize.x - 1, m_ptPos.y + m_ptSize.y - 1,
					 LINE_COLOR );		

	DrawDialogBox();
	return TRUE;
}