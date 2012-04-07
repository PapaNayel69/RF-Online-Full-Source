#include "GUISelectRace.h"
#include "../GUIController/GUIScreen.h"
#include "../SpriteMgr.h"

////////////////////////////////////////////////////////////////////////////
//
// CGUISelectRace Class Implementation
//
////////////////////////////////////////////////////////////////////////////
////*MAYUN
void EventSelectRace();
void EventSelectRaceCancel();
void EventSelectACCRETIAN();
void EventSelectBELLATOR();
void EventSelectCORA();
//*/

CGUISelectRace::CGUISelectRace()
{
	m_eCurSelectedRace = None;
	m_bIsChanging = FALSE;
	m_bIsCreated = FALSE;
}

CGUISelectRace::~CGUISelectRace()
{

}

void
CGUISelectRace::Init( void )
{
	SetSize( gScreen.m_ptSize );
	
	
	// race select board
	m_cUpperBoard.SetSize( 464, 135 );
	m_cUpperBase.SetSize( 456, 109 );
	m_cUpperBoardAni.SetSize( 50, 50 );

	m_cRaceSelectButton[0].SetSize( 143, 38 );
	m_cRaceSelectButton[1].SetSize( 143, 38 );
	m_cRaceSelectButton[2].SetSize( 143, 38 );

	m_cRaceProfile.SetSize( 486, 309 );
	
	
	// menu board
	m_cLowerBoard.SetSize( 520, 73 );
	m_cLowerBase.SetSize( 467, 72 );	

	m_cCreateButton.SetSize( 220, 36 );
	m_cCancelButton.SetSize( 220, 36 );


	m_cRaceSelectButton[0].SetLabel( "벨 라 토" );
	m_cRaceSelectButton[1].SetLabel( "코   라" );
	m_cRaceSelectButton[2].SetLabel( "아크레시아" );

	m_cCreateButton.SetLabel( "확  인" );
	m_cCancelButton.SetLabel( "취  소" );


	// --------------------------------------------------------------

	// race select board
	m_cUpperBoard.SetPos( ( m_ptSize.x - m_cUpperBoard.m_ptSize.x ) / 2, 10 );
	m_cUpperBase.SetPos( m_cUpperBoard.m_ptPos.x + (m_cUpperBoard.m_ptSize.x - m_cUpperBase.m_ptSize.x)/2,
						 m_cUpperBoard.m_ptPos.y );
	m_cUpperBoardAni.SetPos( m_cUpperBase.m_ptPos.x + 203, m_cUpperBase.m_ptPos.y + 20 );

	m_cRaceSelectButton[0].SetPos( m_cUpperBase.m_ptPos.x + 2, m_cUpperBase.m_ptPos.y + 96 );
	m_cRaceSelectButton[1].SetPos( m_cUpperBase.m_ptPos.x + 157, m_cUpperBase.m_ptPos.y + 96 );
	m_cRaceSelectButton[2].SetPos( m_cUpperBase.m_ptPos.x + 312, m_cUpperBase.m_ptPos.y + 96 );

	m_cRaceProfile.SetPos( ( m_ptSize.x - m_cRaceProfile.m_ptSize.x ) / 2,
						   ( m_ptSize.y - m_cRaceProfile.m_ptSize.y ) / 2 );
	
	// menu board
	m_cLowerBoard.SetPos( ( m_ptSize.x - m_cLowerBoard.m_ptSize.x ) / 2, 
						  m_ptSize.y - m_cLowerBoard.m_ptSize.y - 10 );
	m_cLowerBase.SetPos( m_cLowerBoard.m_ptPos.x + (m_cLowerBoard.m_ptSize.x - m_cLowerBase.m_ptSize.x)/2,
						 m_cLowerBoard.m_ptPos.y );	

	m_cCreateButton.SetPos( m_cLowerBoard.m_ptPos.x, m_cLowerBoard.m_ptPos.y + 18 );
	m_cCancelButton.SetPos( m_cLowerBoard.m_ptPos.x + 305, m_cLowerBoard.m_ptPos.y + 18 );

	m_cCreateButton.SetLabelPos( 60, 12 );
	m_cCancelButton.SetLabelPos( 110, 12 );	


	// --------------------------------------------------------------	
	m_cRaceSelectButton[0].AddMouseEventHandler( this );
	m_cRaceSelectButton[1].AddMouseEventHandler( this );
	m_cRaceSelectButton[2].AddMouseEventHandler( this );
	m_cCreateButton.AddMouseEventHandler( this );
	m_cCancelButton.AddMouseEventHandler( this );

	m_cUpperBoard.Add( &m_cUpperBase );
	m_cUpperBoard.Add( &m_cUpperBoardAni );
	m_cUpperBoard.Add( &m_cRaceSelectButton[0] );
	m_cUpperBoard.Add( &m_cRaceSelectButton[1] );
	m_cUpperBoard.Add( &m_cRaceSelectButton[2] );
	Add( &m_cUpperBoard );
	Add( &m_cRaceProfile );

	m_cLowerBoard.Add( &m_cLowerBase );	
	m_cLowerBoard.Add( &m_cCreateButton );
	m_cLowerBoard.Add( &m_cCancelButton );
	Add( &m_cLowerBoard );
	
	
	// --------------------------------------------------------------
	SetSprite();	

#define BUTTON_COLOR	D3DCOLOR_XRGB(127,169,220)

	m_cRaceSelectButton[0].SetLabelColor( BUTTON_COLOR );
	m_cRaceSelectButton[1].SetLabelColor( BUTTON_COLOR );
	m_cRaceSelectButton[2].SetLabelColor( BUTTON_COLOR );

	m_cCreateButton.SetLabelColor( BUTTON_COLOR );
	m_cCancelButton.SetLabelColor( BUTTON_COLOR );


	// --------------------------------------------------------------
	m_cUpperBoardAni.SetTotalFrameNum( 5 );
	m_cUpperBoardAni.SetObjectNumInLine( 2 );
	m_cUpperBoardAni.SetStopInterval( 2000 );
	m_cUpperBoardAni.SetFPS( 10 );	


	m_cRaceSelectButton[0].SetSoundID( SND_ID_LOGIN_SELECT );
	m_cRaceSelectButton[1].SetSoundID( SND_ID_LOGIN_SELECT );
	m_cRaceSelectButton[2].SetSoundID( SND_ID_LOGIN_SELECT );

	m_cCreateButton.SetSoundID( SND_ID_LOGIN_OK );
	m_cCancelButton.SetSoundID( SND_ID_LOGIN_CANCEL );
	
	///*MAYUN
	EventSelectBELLATOR();
	//*/

	m_bIsCreated = TRUE;

/*
#ifndef _DEVELOPER_
	// 현재 아크레시아 캐릭터는 생성할수 없어서 disable한다.
	m_cRaceSelectButton[Accretia].SetLabelColor( 0xFF606060 );	
	m_cRaceSelectButton[Accretia].Disable();
#endif
*/

}

void
CGUISelectRace::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_LOGIN );		

		
		l_spriteInfo.wActionNo = 3;

		// -------------------------------------------------------
		// race select board
		l_spriteInfo.wFrameNo = 0;
		m_cUpperBase.SetSpriteInfo( &l_spriteInfo );

		for( int i=0; i<3; ++i )
		{
			l_spriteInfo.wFrameNo = 1;
			m_cRaceSelectButton[i].SetSpriteInfoEx( &l_spriteInfo );
			l_spriteInfo.wFrameNo = 2;
			m_cRaceSelectButton[i].SetSpriteInfoEx( HOVERED, &l_spriteInfo );
			l_spriteInfo.wFrameNo = 3;
			m_cRaceSelectButton[i].SetSpriteInfoEx( PRESSED, &l_spriteInfo );		
		}		
		
		// -------------------------------------------------------
		// menu board
		l_spriteInfo.wFrameNo = 4;
		m_cLowerBase.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 5;
		m_cCreateButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 6;
		m_cCreateButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 7;
		m_cCreateButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );

		l_spriteInfo.wFrameNo = 9;
		m_cCancelButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 10;
		m_cCancelButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 11;
		m_cCancelButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );	
			

		
		// ani
		l_spriteInfo.wActionNo = 5;
		l_spriteInfo.wFrameNo = 0;
		m_cUpperBoardAni.SetSpriteInfo( &l_spriteInfo );				

		// 
		l_spriteInfo.pSprite = NULL;
		l_spriteInfo.wActionNo = 3;
		l_spriteInfo.wFrameNo  = 13;
		m_cRaceProfile.SetSpriteInfo( &l_spriteInfo );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_LOGIN );
		if( !l_pSprite )
			return;			
		
		// -------------------------------------------------------
		// race select board		
		m_cUpperBase.GetSpriteInfo()->pSprite = l_pSprite;

		for( int i=0; i<3; ++i )		
			m_cRaceSelectButton[i].SetSprite( l_pSprite );							
		
		// -------------------------------------------------------
		// menu board
		m_cLowerBase.GetSpriteInfo()->pSprite = l_pSprite;

		m_cCancelButton.SetSprite( l_pSprite );				
		m_cCreateButton.SetSprite( l_pSprite );
		
		// ani
		m_cUpperBoardAni.GetSpriteInfo()->pSprite = l_pSprite;		

		if( m_cRaceProfile.GetSpriteInfo()->pSprite != NULL )
			m_cRaceProfile.GetSpriteInfo()->pSprite = l_pSprite;
	}
}

void
CGUISelectRace::ChangeRace( Race pi_eRace )
{	
	if( m_eCurSelectedRace == pi_eRace )
		return;

	m_eCurSelectedRace = pi_eRace;
	
	// set profile image
	m_cRaceProfile.GetSpriteInfo()->pSprite = m_cUpperBase.GetSpriteInfo()->pSprite;
	m_cRaceProfile.GetSpriteInfo()->wFrameNo = 13 + pi_eRace;	

	m_bIsChanging = TRUE;
	m_dwChagingTime = timeGetTime();


	///*MAYUN
	switch ( pi_eRace )
	{
	case	Bellato:
		EventSelectBELLATOR();
		break;
	case	Cora:
		EventSelectCORA();
		break;
	case	Accretia:
		EventSelectACCRETIAN();
		break;
	}
	//*/
	// ################## 코드 추가 시킬 곳 ###########################
	// 종족을 바꿨다.
	// ################################################################	
}

void
CGUISelectRace::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_CLICKED )
	{		
		if( event.source == &m_cRaceSelectButton[0] )
		{			
			ChangeRace( Bellato );
		}
		else if( event.source == &m_cRaceSelectButton[1] )
		{
			ChangeRace( Cora );
		}
		else if( event.source == &m_cRaceSelectButton[2] )
		{
			ChangeRace( Accretia );
		}		
		else if( event.source == &m_cCreateButton )
		{
			EventSelectRace();
			// ################## 코드 추가 시킬 곳 ###########################
			// 현재 선택한 종족을 가지고 캐릭터를 생성할 것이다.
			// ################################################################
		}
		else if( event.source == &m_cCancelButton )
		{
			EventSelectRaceCancel();
			// ################## 코드 추가 시킬 곳 ###########################
			// 취소
			// ################################################################
		}
	}
}

void
CGUISelectRace::Update( void )
{
	if( !IsShow() )
		return;

	CGUIContainer::Update();

	if( m_bIsChanging )
	{
		if( m_eCurSelectedRace == None )
			return;

		if( timeGetTime() - m_dwChagingTime >= 1000 )
		{
			m_cRaceProfile.SetARGB( 0xFFFFFFFF );
			m_bIsChanging = FALSE;
		}
		else 
		{
			m_cRaceProfile.SetARGB( (0xFF * ( timeGetTime() - m_dwChagingTime ) / 1000 << 24) | 0x00FFFFFF );
		}
	}
}