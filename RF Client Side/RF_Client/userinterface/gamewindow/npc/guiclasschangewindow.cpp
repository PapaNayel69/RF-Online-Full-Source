#include "GUIClassChangeWindow.h"
#include "../../GUIController/GUIScreen.h"
#include "../../SpriteMgr.h"

////////////////////////////////////////////////////////////////////////////
//
// CGUIClassChangeWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUIClassChangeWindow::CGUIClassChangeWindow()
{
	m_byTotalClassButtonNum	= 0;

	m_bIsCreated		= FALSE;
}

CGUIClassChangeWindow::~CGUIClassChangeWindow()
{

}

void
CGUIClassChangeWindow::Init( void )
{
	SetSize( 228, 412 );

	m_uiAniTitleBar.SetSize( 228, 28 );		
	m_uiCloseButton.SetSize( 16, 13 );
	m_uiHelpButton.SetSize( 16, 13 );	

	m_uiClassDescription.SetSize( 170, 105 );

	POINT l_ptSize;
	l_ptSize.x = 15; 
	l_ptSize.y = 10; 
	m_uiScrollbar.SetTopArrowSize( l_ptSize );
	m_uiScrollbar.SetBottomArrowSize( l_ptSize );
	l_ptSize.y = 18;
	m_uiScrollbar.SetBarSize( l_ptSize );	
	l_ptSize.y = m_uiClassDescription.m_ptSize.y;
	m_uiScrollbar.SetSize( l_ptSize );

	for( int i = 0; i < MAX_CLASS_BUTTON_NUM; ++i )
		m_uiClassSelectButton[i].SetSize( 45, 45 );

	for( i = 0; i < MAX_CLASS_SKILL_ICON_NUM; ++i )
	{
		m_uiClassSkillIcon[i].SetSize( 32, 32 );
	}

	m_uiSelectedButtonMark.SetSize( 47, 47 );

	m_uiOKButton.SetSize( 77, 25 );
	m_uiCancelButton.SetSize( 77, 25 );

	m_uiTitle.SetString( "전 직" );
	m_uiClassListStr.SetString( "전직 리스트" );
	m_uiOKButton.SetLabel( "확 인" );
	m_uiCancelButton.SetLabel( "취 소" );

	// ------------------------------------------------------------------
	
	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 7 );			
	m_uiCloseButton.SetPos( m_ptPos.x + 211, m_ptPos.y + 4 );	
	m_uiHelpButton.SetPos( m_ptPos.x + 194, m_ptPos.y + 4 );

	m_uiClassNameStr.SetPos( m_ptPos.x + 97, m_ptPos.y + 44 );
	m_uiClassListStr.SetPos( m_ptPos.x + 81, m_ptPos.y + 247 );
	m_uiClassDescription.SetPos( m_ptPos.x + 26, m_ptPos.y + 79 );

	m_uiOKButton.SetPos( m_ptPos.x + 12, m_ptPos.y + 381 );
	m_uiCancelButton.SetPos( m_ptPos.x + 139, m_ptPos.y + 381 );

	// ------------------------------------------------------------------

	Add( &m_uiAniTitleBar );
	Add( &m_uiTitle );		
	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );
	Add( &m_uiCloseButton );	
	Add( &m_uiHelpButton );
	
	Add( &m_uiClassNameStr );
	Add( &m_uiClassDescription );

	Add( &m_uiClassListStr );

	for( i = 0; i < MAX_CLASS_BUTTON_NUM; ++i )
		m_uiClassSelectButton[i].AddMouseEventHandler( this );

	for( i = 0; i < MAX_CLASS_SKILL_ICON_NUM; ++i )
		m_uiClassSkillIcon[i].AddMouseEventHandler( this );

	Add( &m_uiSelectedButtonMark );

	m_uiOKButton.AddMouseEventHandler( this );
	m_uiCancelButton.AddMouseEventHandler( this );
	Add( &m_uiOKButton );
	Add( &m_uiCancelButton );

	// ------------------------------------------------------------------
	SetSprite();

	m_uiTitle.GetFont()->dwColor = 0xFFC3C1BE;
	m_uiClassNameStr.GetFont()->dwColor = 0xFFC3C1BE;
	m_uiClassListStr.GetFont()->dwColor = 0xFFC3C1BE;

	m_uiOKButton.SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );	
	m_uiCancelButton.SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );	

	m_uiOKButton.SetLabelStyle( R3_HAN_OUTLINE );
	m_uiCancelButton.SetLabelStyle( R3_HAN_OUTLINE );

	m_uiClassDescription.SetMargin( 0, 0 );
	m_uiClassDescription.SetColumnGap( 3 );
	m_uiClassDescription.AddScrollbar( &m_uiScrollbar );
	SetWheelScrollObject( &m_uiScrollbar );

	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );

	SetMoveObject( &m_uiAniTitleBar );	
	SetMoveObject( &m_uiTitle );	
	SetMovable( TRUE );

	SoundOn( TRUE );	

	SetName( "classchange" );

	m_bIsCreated = TRUE;
}

void
CGUIClassChangeWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_CLASS );

		// background
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );

		// class button
		l_spriteInfo.wFrameNo = 2;
		for( int i = 0; i < MAX_CLASS_BUTTON_NUM; ++i )
			m_uiClassSelectButton[i].SetSpriteInfo( &l_spriteInfo );

		// selected mark
		l_spriteInfo.pSprite = NULL;
		l_spriteInfo.wFrameNo = 1;
		m_uiSelectedButtonMark.SetSpriteInfo( &l_spriteInfo );

		// ani title bar	
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );

		// scrollbar
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

		// help button
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_spriteInfo.wActionNo = 5;
		l_spriteInfo.wFrameNo = 0;
		m_uiHelpButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 1;
		m_uiHelpButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiHelpButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		
		// m_uiOKButton button
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 12;
		m_uiOKButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_uiOKButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_uiOKButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 15;
		m_uiOKButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );	
		
		// cancel button
		l_spriteInfo.wFrameNo = 12;
		m_uiCancelButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_uiCancelButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_uiCancelButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 15;
		m_uiCancelButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_CLASS );
		if( !l_pSprite )
			return;
		
		// background
		GetSpriteInfo()->pSprite = l_pSprite;		

		// class button
		for( int i = 0; i < MAX_CLASS_BUTTON_NUM; ++i )
			m_uiClassSelectButton[i].GetSpriteInfo()->pSprite = l_pSprite;

		// selected mark
		if( m_uiSelectedButtonMark.GetSpriteInfo()->pSprite )
			m_uiSelectedButtonMark.GetSpriteInfo()->pSprite = l_pSprite;

		// ani title bar
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		m_uiScrollbar.SetSprite( l_pSprite );

		// close button
		m_uiCloseButton.SetSprite( l_pSprite );
		// help button
		m_uiHelpButton.SetSprite( l_pSprite );

		m_uiOKButton.SetSprite( l_pSprite );
		m_uiCancelButton.SetSprite( l_pSprite );

		// skill icon
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
		for( i = 0; i < MAX_CLASS_SKILL_ICON_NUM; ++i )
			m_uiClassSkillIcon[i].GetSpriteInfo()->pSprite = l_pSprite;
	}
}

void	
CGUIClassChangeWindow::SetTotalClassNum( BYTE pi_byClassNum, BYTE * pi_pIconIDList )
{
	m_byTotalClassButtonNum = pi_byClassNum;

	if( m_byTotalClassButtonNum > MAX_CLASS_BUTTON_NUM )
		m_byTotalClassButtonNum = MAX_CLASS_BUTTON_NUM;	

	int l_nPosX = 0;
	int l_nButtonNumInLine;
	int l_nRestLine;

	for( int i = 0; i <	m_byTotalClassButtonNum; ++i )
	{
		// set sprite
		m_uiClassSelectButton[i].SetSpriteTileByIndex( pi_pIconIDList[i] );

		// setpos
		if( !( i % 4 ) )
		{
			l_nRestLine = ( m_byTotalClassButtonNum - i ) / 4;
			if( l_nRestLine )
				l_nButtonNumInLine = 4;
			else
				l_nButtonNumInLine = ( m_byTotalClassButtonNum - i ) % 4;
			
			l_nPosX = m_ptPos.x + ( m_ptSize.x - ( m_uiClassSelectButton[0].m_ptSize.x + 3 ) * l_nButtonNumInLine ) / 2;
		}

		m_uiClassSelectButton[i].SetPos( l_nPosX, m_ptPos.y + 271 + ( i / 4 ) * ( m_uiClassSelectButton[0].m_ptSize.y + 3 ) );				
		
		l_nPosX += m_uiClassSelectButton[i].m_ptSize.x + 3;

		// add
		m_uiClassSelectButton[i].Show( IsShow() );
		Add( &m_uiClassSelectButton[i] );
	}
	// remove rest
	for( ; i < MAX_CLASS_BUTTON_NUM; ++i )
	{
		Remove( &m_uiClassSelectButton[i] );
	}		
}

void
CGUIClassChangeWindow::SelectClass( BYTE pi_byIndex )
{
	if( pi_byIndex >= m_byTotalClassButtonNum )
		return;

	m_uiSelectedButtonMark.SetPos( m_uiClassSelectButton[pi_byIndex].m_ptPos.x - 1,
											  m_uiClassSelectButton[pi_byIndex].m_ptPos.y - 1 );

	m_uiSelectedButtonMark.GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_CLASS );
}

void
CGUIClassChangeWindow::SetSkillIcon( BYTE pi_byIconNum, DWORD * pi_pIconIDList )
{
	int l_nIconNum = pi_byIconNum;

	if( l_nIconNum > MAX_CLASS_SKILL_ICON_NUM )
		l_nIconNum = MAX_CLASS_SKILL_ICON_NUM;

	int l_nPosX = m_ptPos.x + ( m_ptSize.x - ( m_uiClassSkillIcon[0].m_ptSize.x + 5 ) * l_nIconNum ) / 2;
	for( int i = 0; i <	l_nIconNum; ++i )
	{
		// set sprite
		m_uiClassSkillIcon[i].SetIconType( CLASS_SKILL_ICON );
		m_uiClassSkillIcon[i].SetSpriteID( pi_pIconIDList[i] );
		m_uiClassSkillIcon[i].SetSprite();

		m_uiClassSkillIcon[i].SetPos( l_nPosX, m_ptPos.y + 190 );
		l_nPosX += ( 5 + m_uiClassSkillIcon[i].m_ptSize.x );

		m_uiClassSkillIcon[i].Show( IsShow() );
		Add( &m_uiClassSkillIcon[i] );
	}
	// remove rest
	for( ; i < MAX_CLASS_SKILL_ICON_NUM; ++i )
	{
		Remove( &m_uiClassSkillIcon[i] );
	}
}

void	
CGUIClassChangeWindow::SetClassName( char * pi_pStr )
{
	m_uiClassNameStr.SetString( pi_pStr );

	m_uiClassNameStr.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiClassNameStr.m_ptSize.x ) / 2, m_ptPos.y + 44 );
}

void	
CGUIClassChangeWindow::SetDescription( char * pi_pStr )
{
	m_uiClassDescription.Clear();
	m_uiClassDescription.InsertText( pi_pStr );

	// show scrollbar
	if( m_uiScrollbar.GetTotalAmount() > 0 )
	{
		m_uiClassDescription.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiClassDescription.m_ptSize.x - m_uiScrollbar.m_ptSize.x )/2,
									 m_uiClassDescription.m_ptPos.y );

		m_uiScrollbar.SetPos( m_uiClassDescription.m_ptPos.x + m_uiClassDescription.m_ptSize.x,
							  m_uiClassDescription.m_ptPos.y );
		m_uiScrollbar.Show( IsShow() );
		Add( &m_uiScrollbar );
	}
	else
	{
		m_uiClassDescription.SetPos( m_ptPos.x + (m_ptSize.x - m_uiClassDescription.m_ptSize.x)/2, m_uiClassDescription.m_ptPos.y );
	
		Remove( &m_uiScrollbar );
	}
}

void
CGUIClassChangeWindow::MouseEvent( _mouse_event &event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_STROKED )
	{
		for( int i = 0; i < MAX_CLASS_BUTTON_NUM; ++i )
		{
			if( event.source == &m_uiClassSelectButton[i] )
			{
				DWORD dwInfo = MAKELONG( i, 0 );		// 클래스는 분류가 0
				PostUIMessage( WM_BOARD_CLICK_OBJECT, dwInfo );
				return;
			}
		}
	}
	else if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiOKButton )
		{
			PostUIMessage( WM_BOARD_OK, 0 );
		}
		else if( event.source == &m_uiCancelButton )
		{
			PostUIMessage( WM_BOARD_CANCEL, 0 );
		}
		else if( event.source == &m_uiCloseButton )
		{
			PostUIMessage( WM_BOARD_CANCEL, 0 );
		}
		else if( event.source == &m_uiHelpButton )
		{
//			PostUIMessage( WM_OPEN_WINDOW_HELP, 0 );
		}
	}
	else if( event.id == MOUSE_HOVERED )
	{
		for( int i = 0; i < MAX_CLASS_SKILL_ICON_NUM; ++i )
		{
			if( event.source == &m_uiClassSkillIcon[i] )
			{
				DWORD dwInfo = MAKELONG( i, 1 );		// 스킬은 분류가 1
				PostUIMessage( WM_BOARD_HOVER_OBJECT, dwInfo );
				return;
			}
		}
	}
}