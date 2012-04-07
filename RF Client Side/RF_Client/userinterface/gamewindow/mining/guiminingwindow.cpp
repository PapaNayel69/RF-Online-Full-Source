////////////////////////////////////////////////////////////////////////////
//
// CGUIMiningWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIMiningWindow.h"
#include "../../SpriteMgr.h"

////////////////////////////////////////////////////////////////////////////

CGUIMiningWindow::CGUIMiningWindow()
{	
	m_bySelectedOreIndex	= 0xFF;

	m_bIsCreated			= FALSE;
}

CGUIMiningWindow::~CGUIMiningWindow()
{
	
}

void
CGUIMiningWindow::Init( void )
{
	if( m_bIsCreated )
		return;

	// -------------------------------------------------
	// size
	SetSize( 148, 74 );
	m_uiOreSelectBase.SetSize( 140, 32 );
	
	m_uiMiningButton.SetSize( 94, 22 );

	for( int i=0; i<5; ++i )	
	{
		m_uiOreSelectObject[i].SetSize( 22, 19 );
	}

	m_uiTitle.SetString( "오어 선택" );	
	
	// -------------------------------------------------
	// pos
	m_uiOreSelectBase.SetPos( m_ptPos.x+ 4, m_ptPos.y + 17 );
	for( i=0; i<5; ++i )
	{
		m_uiOreSelectObject[i].SetPos( m_ptPos.x + 9 + 27 * i, m_ptPos.y + 23 );		
	}

	m_uiMiningButton.SetPos( m_ptPos.x + 27, m_ptPos.y + 47 );
	
	m_uiTitle.SetPos( m_ptPos.x + 47, m_ptPos.y + 4 );

	
	// -------------------------------------------------
	// add 
	Add( &m_uiTitle );
	Add( &m_uiOreSelectBase );
	for( i=0; i<5; ++i )
	{
		m_uiOreSelectObject[i].AddMouseEventHandler( this );
		Add( &m_uiOreSelectObject[i] );
	}	

	m_uiMiningButton.AddMouseEventHandler( this );
	Add( &m_uiMiningButton );
	

	// -------------------------------------------------
	SetSprite();			

	SetARGB( 0xCCFFFFFF );
	
	for( i=0; i<5; ++i )
	{
		m_uiOreSelectObject[i].SetTotalFrameNum( 5 );
		m_uiOreSelectObject[i].SetObjectNumInLine( 1 );
		m_uiOreSelectObject[i].SetFPS( 10 );
		m_uiOreSelectObject[i].SetStopInterval( 500 );	
	}	

	SetMoveObject( &m_uiTitle );
	SetMovable( true );
	
	
	Mining_Off();
	EnableMiningButton( TRUE );

	SelectOre( 0 );

	SetName( "Mining" );	

	m_bIsCreated = TRUE;
}

void
CGUIMiningWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		// set sprite
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_MINING );

		l_spriteInfo.wActionNo = 0;

		// background
		l_spriteInfo.wFrameNo  = 0;
		SetSpriteInfo( &l_spriteInfo );

		// board
		l_spriteInfo.wFrameNo  = 1;
		m_uiOreSelectBase.SetSpriteInfo( &l_spriteInfo );				
		
		// select object		
		l_spriteInfo.wFrameNo  = 2;
		for( int i=0; i<5; ++i )
			m_uiOreSelectObject[i].SetSpriteInfo( &l_spriteInfo );

		for( i=0; i<5; ++i )
			m_uiOreSelectObject[i].GetSpriteInfo()->pSprite = NULL;

		// mining button
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		l_spriteInfo.wActionNo = 6;
		l_spriteInfo.wFrameNo = 2;
		m_uiMiningButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 3;
		m_uiMiningButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 4;
		m_uiMiningButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo  = 5;
		m_uiMiningButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );		
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_MINING );	
		if( !l_pSprite )
			return;

		GetSpriteInfo()->pSprite = l_pSprite;

		m_uiOreSelectBase.GetSpriteInfo()->pSprite = l_pSprite;
		
		for( int i=0; i<5; ++i )
		{
			if( m_uiOreSelectObject[i].GetSpriteInfo()->pSprite )
				m_uiOreSelectObject[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );	
		m_uiMiningButton.SetSprite( l_pSprite );
	}
}

void
CGUIMiningWindow::SelectOre( BYTE pi_byOreIndex )
{
	if( m_bySelectedOreIndex == pi_byOreIndex )
		return;

	BYTE l_byPrevOreIndex;
	l_byPrevOreIndex = m_bySelectedOreIndex;

	if( l_byPrevOreIndex < 5 )
	{
		// unselect prev selected ore
		m_uiOreSelectObject[l_byPrevOreIndex].GetSpriteInfo()->pSprite = NULL;
	}
	
	// select ore
	m_bySelectedOreIndex = pi_byOreIndex;
	m_uiOreSelectObject[m_bySelectedOreIndex].GetSpriteInfo()->pSprite = GetSpriteInfo()->pSprite;		
}

void
CGUIMiningWindow::Mining_On( void )
{		
	m_uiMiningButton.SetLabel( "채굴종료" );	
}

void
CGUIMiningWindow::Mining_Off( void )
{
	m_uiMiningButton.SetLabel( "채굴시작" );	
}

void
CGUIMiningWindow::EnableMiningButton( BOOL pi_bEnable )
{
	if( pi_bEnable )
	{
		m_uiMiningButton.Enable();
	}
	else
	{
		m_uiMiningButton.Disable();
	}
}

void
CGUIMiningWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiMiningButton )
		{
			// 채굴
			PostUIMessage( WM_MINING_BOARD_MINING, 0 );			
		}
	}
	else if( event.id == MOUSE_STROKED )
	{		
		for( int i = 0; i < 5; ++i )
		{
			if( event.source == &m_uiOreSelectObject[i] )
			{
				// 오어 선택
				PostUIMessage( WM_MINING_BOARD_SELECT_ORE, i );
				break;
			}
		}
	}
}