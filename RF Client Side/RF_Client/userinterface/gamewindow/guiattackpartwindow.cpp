////////////////////////////////////////////////////////////////////////////
//
// CGUIAttackPartWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIAttackPartWindow.h"
#include "../SpriteMgr.h"

void EP_SelectAttackPart( BYTE pi_byAttackPart );
void EP_DeselectAttackPart( void );

////////////////////////////////////////////////////////////////////////////

#define	NONE_PART	0xff

#define ATTACK_PART_RGB		D3DCOLOR_XRGB(176,191,255)
#define WEAK_PART_RGB		D3DCOLOR_XRGB(255,172,172)

CGUIAttackPartWindow::CGUIAttackPartWindow()
{
	m_bCreated = FALSE;
}

CGUIAttackPartWindow::~CGUIAttackPartWindow()
{

}

void
CGUIAttackPartWindow::Init( void )
{
	// size
	SetSize( 120, 131 );
	m_cBoard.SetSize( 120, 131 );		

	for( int i=0; i<5; ++i )
	{
		m_cAttackPartButton[i].SetSize( 116, 22 );
		m_cAttackPartButton[i].SetButtonSize( 30, 7 );				
	}	

	m_cCloseButton.SetSize( 16, 13 );	

	m_cAttackPartButton[0].SetString( "머리 공격" );
	m_cAttackPartButton[1].SetString( "상체 공격" );
	m_cAttackPartButton[2].SetString( "하체 공격" );
	m_cAttackPartButton[3].SetString( "손   공격" );
	m_cAttackPartButton[4].SetString( "발   공격" );

	for( i=0; i<5; ++i )
	{
		m_cAttackPartButton[i].SetPos( 4, 21 + i * m_cAttackPartButton[i].m_ptSize.y );
		m_cAttackPartButton[i].GetButton()->SetPos( m_cAttackPartButton[i].m_ptPos.x + 76,
													m_cAttackPartButton[i].m_ptPos.y + 11 );
		m_cAttackPartButton[i].GetText()->SetPos( m_cAttackPartButton[i].m_ptPos.x + 6,
												  m_cAttackPartButton[i].m_ptPos.y + 6 );		
	}
	
	m_cAttackCharStr.SetPos( 5, 3 );

	m_cCloseButton.SetPos( 101, 1 );	
	

	// add button to button controller
	for( i=0; i<5; ++i )
		m_cButtonCtrl.AddButton( &m_cAttackPartButton[i] );	

	// add event handler
	m_cCloseButton.AddMouseEventHandler( this );
	m_cButtonCtrl.AddSelectItemHandler( this );

	// add	
	m_cBoard.Add( &m_cAttackCharStr );

	for( i=0; i<5; ++i )
		m_cBoard.Add( &m_cAttackPartButton[i] );		


	m_cBoard.Add( &m_cCloseButton );
	Add( &m_cBoard );

	// unselect 가능하게
	m_cButtonCtrl.SetEnableUnSelect( true );

	SetSprite();

	SetARGB( 0xd0ffffff );

	m_cAttackCharStr.GetFont()->dwColor = D3DCOLOR_XRGB( 168, 185, 255 );

	for( i=0; i<5; ++i )	
		m_cAttackPartButton[i].GetText()->GetFont()->dwColor = ATTACK_PART_RGB;

	m_byWeakPart = NONE_PART;

	SetMoveObject( &m_cBoard );
	SetMoveObject( &m_cAttackCharStr );	
	SetMovable( true );

	SetName( "attackPart" );

	m_bCreated = TRUE;	
}

void
CGUIAttackPartWindow::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );

		l_spriteInfo.wActionNo = 4;

		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 1;
		m_cBoard.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 2;
		for( int i=0; i<5; ++i )
			m_cAttackPartButton[i].SetSelectedSprite( &l_spriteInfo );	
		
		m_cCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_cCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_cCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		if( !l_pSprite )
			return;
		
		// background
		GetSpriteInfo()->pSprite = l_pSprite;

		m_cBoard.GetSpriteInfo()->pSprite = l_pSprite;

		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = l_pSprite;
		l_spriteInfo.wActionNo = 4;
		l_spriteInfo.wFrameNo = 2;
		for( int i=0; i<5; ++i )
			m_cAttackPartButton[i].SetSelectedSprite( &l_spriteInfo );				

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		m_cCloseButton.SetSprite( l_pSprite );
	}
}

void
CGUIAttackPartWindow::SetAttackCharName( char * pi_pAttChar )
{
	m_cAttackCharStr.SetString( pi_pAttChar );

	// repos
	m_cAttackCharStr.SetPos( m_ptPos.x + 2 + ( 99 - m_cAttackCharStr.m_ptSize.x ) / 2,
							  m_cAttackCharStr.m_ptPos.y );
}

void
CGUIAttackPartWindow::SetWeakPart( BYTE pi_byWeakPart )
{
	// 취약부위에 text color를 바꾼다.
	if( pi_byWeakPart > 4 )
		return;

	if( m_byWeakPart != NONE_PART )
		m_cAttackPartButton[m_byWeakPart].GetText()->GetFont()->dwColor = ATTACK_PART_RGB;	

	m_byWeakPart = pi_byWeakPart;

	m_cAttackPartButton[m_byWeakPart].GetText()->GetFont()->dwColor = WEAK_PART_RGB;	
}

void
CGUIAttackPartWindow::ClearWeakPart( void )
{
	if( m_byWeakPart == NONE_PART )
		return;

	m_cAttackPartButton[m_byWeakPart].GetText()->GetFont()->dwColor = ATTACK_PART_RGB;

	m_byWeakPart = NONE_PART;	
}

void
CGUIAttackPartWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );	

	if( event.id == MOUSE_CLICKED )
	{	
		if( event.source == &m_cCloseButton )
			Close();
	}
}

void
CGUIAttackPartWindow::SelectItemEvent( _select_event & event )
{
	if( event.id == SELECTED )
	{
		for( int i=0; i<5; ++i )
		{
			if( event.source == &m_cAttackPartButton[i] )
			{
				// ################## 코드 추가 시킬 곳 ###########################
				// 공격 부위 선택
				// ################################################################
				EP_SelectAttackPart( i );			
				return; 
			}
		}
	}
	else if( event.id == UNSELECTED )
	{
		for( int i=0; i<5; ++i )
		{
			if( event.source == &m_cAttackPartButton[i] )
			{
				// ################## 코드 추가 시킬 곳 ###########################
				// 공격 부위 선택 해제
				// ################################################################		
				EP_DeselectAttackPart();				
				return;
			}
		}
	}
}