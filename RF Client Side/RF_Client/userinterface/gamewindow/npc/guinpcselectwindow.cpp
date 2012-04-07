////////////////////////////////////////////////////////////////////////////
//
// CGUINpcSelectWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUINpcSelectWindow.h"
#include "../../GUIController/GUIScreen.h"
#include "../../SpriteMgr.h"


#define	BASE_ARGB		0xCC161C1C
#define	BASE_LINE_ARGB	0xB0B4B4B4
#define TITLE_COLOR		0xFFC3C1BE
#define	TEXT_COLOR		0xFFE8E8E8

////////////////////////////////////////////////////////////////////////////

CGUINpcSelectWindow::CGUINpcSelectWindow()
{
	m_byButtonNum = 0;

	m_bCreated = FALSE;
}

CGUINpcSelectWindow::~CGUINpcSelectWindow()
{

}

void
CGUINpcSelectWindow::Init( void )
{
	// --------------------------------------------------------
	// size
	SetSize( 232, 253 );	

	m_uiTitleBoard.SetSize( 232, 26 );		
	
	m_uiDescBoard.SetSize( 208, 130 );

	POINT l_ptSize;
	l_ptSize.x = 15; 
	l_ptSize.y = 10; 
	m_uiScrollbar.SetTopArrowSize( l_ptSize );
	m_uiScrollbar.SetBottomArrowSize( l_ptSize );
	l_ptSize.y = 18;
	m_uiScrollbar.SetBarSize( l_ptSize );	
	l_ptSize.y = m_uiDescBoard.m_ptSize.y;
	m_uiScrollbar.SetSize( l_ptSize );	

	for( int i = 0; i < MAX_SELECT_BUTTON_NUM; ++i )	
		m_uiNpcSelectButton[i].SetSize( 164, 26 );
	

	m_uiCloseButton.SetSize( 16, 13 );

	// --------------------------------------------------------
	// pos
	m_uiDescBoard.SetPos( m_ptPos.x + (m_ptSize.x - m_uiDescBoard.m_ptSize.x)/2, m_uiTitleBoard.m_ptSize.y + 10 );		

	m_uiCloseButton.SetPos( 214, 4 );

	// --------------------------------------------------------
	// add controller
	for( i = 0; i < MAX_SELECT_BUTTON_NUM; ++i )	
		m_uiNpcSelectButton[i].AddMouseEventHandler( this );
	
	m_uiCloseButton.AddMouseEventHandler( this );	
	
	Add( &m_uiTitleBoard );	
	Add( &m_uiTitle );
	Add( &m_uiCloseButton );

	Add( &m_uiDescBoard );		


	// --------------------------------------------------------	
	// m_uiDescBoard setting
	m_uiDescBoard.SetMargin( 6, 8 );
	m_uiDescBoard.SetColumnGap( 4 );	
	m_uiDescBoard.AddScrollbar( &m_uiScrollbar );
	SetWheelScrollObject( &m_uiScrollbar );

	SetSprite();

	m_uiTitle.GetFont()->dwColor = TITLE_COLOR;	

	for( i = 0; i < MAX_SELECT_BUTTON_NUM; ++i )	
		m_uiNpcSelectButton[i].SetLabelColor( TEXT_COLOR );
	
	m_uiDescBoard.SetARGB( 0xA0121212 );	

	SetMoveObject( &m_uiTitleBoard );
	SetMoveObject( &m_uiTitle );
	SetMovable( TRUE );


	m_bCreated = TRUE;
}

void
CGUINpcSelectWindow::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );

		l_spriteInfo.wActionNo = 7;

		// title board
		l_spriteInfo.wFrameNo = 0;	
		m_uiTitleBoard.SetSpriteInfo( &l_spriteInfo );			

		// select button		
		for( int i = 0; i < MAX_SELECT_BUTTON_NUM; ++i )
		{
			l_spriteInfo.wFrameNo = 1;
			m_uiNpcSelectButton[i].SetSpriteInfoEx( &l_spriteInfo );	
			l_spriteInfo.wFrameNo = 2;
			m_uiNpcSelectButton[i].SetSpriteInfoEx( PRESSED, &l_spriteInfo );
			l_spriteInfo.wFrameNo = 3;
			m_uiNpcSelectButton[i].SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		}

		// descboard scrollbar
		SPRITE_INFO l_sScrollbarSprite[2];
		l_sScrollbarSprite[0].pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_sScrollbarSprite[0].wActionNo = 4;
		memcpy( &l_sScrollbarSprite[1], &l_sScrollbarSprite[0], sizeof( SPRITE_INFO ) );
		
		l_sScrollbarSprite[0].wFrameNo = 0;
		m_uiScrollbar.SetSpriteInfo( &l_sScrollbarSprite[0] );
		l_sScrollbarSprite[0].wFrameNo = 1;
		m_uiScrollbar.SetBarSpriteInfo( &l_sScrollbarSprite[0] );

		l_sScrollbarSprite[0].wFrameNo = 2;
		l_sScrollbarSprite[1].wFrameNo = 3;
		m_uiScrollbar.SetTopArrowSpriteInfo( l_sScrollbarSprite );

		l_sScrollbarSprite[0].wFrameNo = 4;
		l_sScrollbarSprite[1].wFrameNo = 5;
		m_uiScrollbar.SetBottomArrowSpriteInfo( l_sScrollbarSprite );	
		
		// close button	
		m_uiCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_uiCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_uiCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );	
		if( !l_pSprite )
			return;		

		// title board		
		m_uiTitleBoard.GetSpriteInfo()->pSprite = l_pSprite;


		// select button	
		for( int i = 0; i < MAX_SELECT_BUTTON_NUM; ++i )
		{
			m_uiNpcSelectButton[i].SetSprite( l_pSprite );
		}
		

		// descboard scrollbar
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );		
		m_uiScrollbar.SetSprite( l_pSprite );		
		
		// close button	
		m_uiCloseButton.SetSprite( l_pSprite );
	}
}

void
CGUINpcSelectWindow::Open( void )
{
	CGUIWindow::Open();

	// NPC 캐릭터 클릭 이벤트는 띌때까지
	// UI에서 사용하지 못하게 한다.
	CGUIObject::SetPressedOtherObject();
}

void
CGUINpcSelectWindow::SetButtonType( BYTE pi_byTotalButtonNum, BYTE * pi_pButtonTypeList )
{
	if( pi_byTotalButtonNum >= 5 )
		return;

	static char BUTTON_TYPE_STR[13][32] =
	{
		"없음", "아이템 거래", "퀘스트", "경제지표 확인", "환 전", "가 공",
		"아이템/돈 창고 기능", "안 내", "입 장", "킬포인트", "기갑 장비 튜닝", "기갑 장비 출고",
		"클래스 전직"
	};

	m_byButtonNum = pi_byTotalButtonNum;
	if( m_byButtonNum > MAX_SELECT_BUTTON_NUM )
		m_byButtonNum = MAX_SELECT_BUTTON_NUM;	
		
	// set pos
	// set label
	for( int i = 0; i < m_byButtonNum; ++i )	
	{
		m_uiNpcSelectButton[i].SetPos( m_ptPos.x + ( m_ptSize.x - m_uiNpcSelectButton[i].m_ptSize.x ) / 2,
				m_uiDescBoard.m_ptPos.y + m_uiDescBoard.m_ptSize.y + 10 + ( m_uiNpcSelectButton[i].m_ptSize.y + 10 ) * i );

		if( pi_pButtonTypeList[i] < 13 )						
			m_uiNpcSelectButton[i].SetLabel( BUTTON_TYPE_STR[ pi_pButtonTypeList[i] ] );
		else
			m_uiNpcSelectButton[i].SetLabel( NULL );
	}

	// set bound
	SetSize( m_ptSize.x, 185 + m_byButtonNum * ( m_uiNpcSelectButton[0].m_ptSize.y + 10 ) );	

	// add button
	for( i = 0; i < m_byButtonNum; ++i )
	{
		m_uiNpcSelectButton[i].Show( IsShow() );
		Add( &m_uiNpcSelectButton[i] );
	}
	for( ; i < MAX_SELECT_BUTTON_NUM; ++i )
	{
		Remove( &m_uiNpcSelectButton[i] );
	}
}

void
CGUINpcSelectWindow::SetTitle( char * pi_pTitle )
{
	m_uiTitle.SetString( pi_pTitle );	

	m_uiTitle.SetPos( m_ptPos.x + (m_ptSize.x - m_uiTitle.m_ptSize.x)/2,
					  m_uiTitleBoard.m_ptPos.y + (m_uiTitleBoard.m_ptSize.y - m_uiTitle.m_ptSize.y)/2 );
}

void
CGUINpcSelectWindow::SetDescription( char * pi_pDescStr )
{
	m_uiDescBoard.Clear();

	m_uiDescBoard.InsertText( pi_pDescStr );		

	// show scrollbar
	if( m_uiScrollbar.GetTotalAmount() > 0 )
	{
		m_uiDescBoard.SetPos( m_ptPos.x + 5, m_uiDescBoard.m_ptPos.y );

		m_uiScrollbar.SetPos( m_uiDescBoard.m_ptPos.x + m_uiDescBoard.m_ptSize.x,
							  m_uiDescBoard.m_ptPos.y );
		m_uiScrollbar.Show( IsShow() );
		Add( &m_uiScrollbar );
	}
	else
	{
		m_uiDescBoard.SetPos( m_ptPos.x + (m_ptSize.x - m_uiDescBoard.m_ptSize.x)/2, m_uiDescBoard.m_ptPos.y);
	
		Remove( &m_uiScrollbar );
	}
}

void
CGUINpcSelectWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_CLICKED )
	{
		if( gScreen.ExistObject() )
			return;

		if( event.source == &m_uiCloseButton )
		{
			Close();
		}
		else
		{
			for( int i = 0; i < MAX_SELECT_BUTTON_NUM; ++i )
			{
				if( event.source == &m_uiNpcSelectButton[i] )
				{
					PostUIMessage( WM_BOARD_OPEN_SHOP_WINDOW, i );
					return;
				}
			}
		}		
	}
}

BOOL
CGUINpcSelectWindow::Draw( void )
{
	if( !IsShow() )
		return FALSE;	

	// base
	Draw2DRectangle( m_ptPos.x, m_ptPos.y + m_uiTitleBoard.m_ptSize.y - 10,
					 m_ptPos.x + m_ptSize.x, m_ptPos.y + m_ptSize.y,
					 BASE_ARGB );

	// left line
	Draw2DRectangle( m_ptPos.x + 2, m_ptPos.y + m_uiTitleBoard.m_ptSize.y - 10,
					 m_ptPos.x + 3, m_ptPos.y + m_ptSize.y - 3,
					 BASE_LINE_ARGB );					 
	// right line
	Draw2DRectangle( m_ptPos.x + m_ptSize.x - 3, m_ptPos.y + m_uiTitleBoard.m_ptSize.y - 10,
					 m_ptPos.x + m_ptSize.x - 2, m_ptPos.y + m_ptSize.y - 3,
					 BASE_LINE_ARGB );					
	// bottom line
	Draw2DRectangle( m_ptPos.x + 2, m_ptPos.y + m_ptSize.y - 3,
					 m_ptPos.x + m_ptSize.x - 2, m_ptPos.y + m_ptSize.y - 2,
					 BASE_LINE_ARGB );					

	CGUIContainer::Draw();

	DrawDialogBox();

	return TRUE;
}