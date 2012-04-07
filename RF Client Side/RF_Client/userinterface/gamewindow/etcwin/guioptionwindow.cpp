////////////////////////////////////////////////////////////////////////////
//
// CGUIOptionWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIOptionWindow.h"
#include "../../SpriteMgr.h"
#include "../../GUIController/GUIScreen.h"
#include "../../../DefineMessage.h"
#include "Core.h"

void	EP_OpenDialogBox( CGUIWindow * pi_pOwnerWindow, CGUIDialogBox * pi_pDialogBox );
void	EP_CloseDialogBox( CGUIWindow * pi_pOwnerWindow );

void	EP_Logout( void );
void	EP_QuitProgram( void );
////////////////////////////////////////////////////////////////////////////

CGUIOptionWindow::CGUIOptionWindow()
{
	m_bIsCreated = FALSE;

	m_bySelectedCamera = 0xFF;
}

CGUIOptionWindow::~CGUIOptionWindow()
{

}

void
CGUIOptionWindow::Init( void )
{
	SetSize( 228, 571 );	

	m_uiTitle.SetSize( 80, 28 );
	m_uiAniTitleBar.SetSize( 228, 28 );	
	m_uiCloseButton.SetSize( 16, 13 );
	m_uiHelpButton.SetSize( 16, 13 );

	m_uiLogOutButton.SetSize( 100, 25 );
	m_uiExitButton.SetSize( 100, 25 );

	// scrollbar setting
	POINT l_ptSize;	
	for( int i = 0; i < SOUND_OPTION_NUM; ++i )
	{
		l_ptSize.x = 8;
		l_ptSize.y = 11;
		m_uiSoundCtrl[i].SetTopArrowSize( l_ptSize );
		m_uiSoundCtrl[i].SetBottomArrowSize( l_ptSize );				
		
		m_uiSoundCtrl[i].SetBarSize( l_ptSize );		

		l_ptSize.x = 184;		
		m_uiSoundCtrl[i].SetSize( l_ptSize );

		m_uiSoundCtrl[i].SetStyle( CGUIScrollbar::Horizontal );
	}

	for( i = 0; i < GRAPHIC_OPTION_NUM; ++i )
	{
		l_ptSize.x = 8;
		l_ptSize.y = 11;
		m_uiGraphicCtrl[i].SetTopArrowSize( l_ptSize );
		m_uiGraphicCtrl[i].SetBottomArrowSize( l_ptSize );
		
		m_uiGraphicCtrl[i].SetBarSize( l_ptSize );

		l_ptSize.x = 184;		
		m_uiGraphicCtrl[i].SetSize( l_ptSize );		

		m_uiGraphicCtrl[i].SetStyle( CGUIScrollbar::Horizontal );
	}	

	m_uiGraphicNum[Gamma].SetSize( 169, 9 );
	m_uiGraphicNum[DynamicLighting].SetSize( 170, 12 );
	m_uiGraphicNum[Shadow].SetSize( 170, 12 );
	m_uiGraphicNum[SeeDist].SetSize( 170, 12 );

	for( i = 0; i < ETC_OPTION_NUM; ++i )
	{
		m_uiEtcSwitch[i].SetSize( 12, 12 );
	}

	for( i = 0; i < TOTAL_CAMERA_NUM; ++i )
	{
		m_uiCamSelectObj[i].SetSize( 16, 21 );
	}

	m_uiTitle.SetString( "옵 션" );

	m_uiLogOutButton.SetLabel( "로그 아웃" );
	m_uiExitButton.SetLabel( "게임 종료" );

	m_uiSoundTitleStr.SetString( "사운드 설정" );
	m_uiSoundStr[Effect_Snd].SetString( "효 과 음" );
	m_uiSoundStr[BGM].SetString( "배 경 음" );

	m_uiGraphicTitleStr.SetString( "그래픽 설정" );
	m_uiGraphicStr[Gamma].SetString( "감 마" );
	m_uiGraphicStr[DynamicLighting].SetString( "다이나믹 라이팅" );
	m_uiGraphicStr[Shadow].SetString( "그 림 자" );
	m_uiGraphicStr[SeeDist].SetString( "시야 거리" );

	m_uiEtcTitleStr.SetString( "기타 설정" );
	m_uiEtcSwitchStr[Whisper].SetString( "귓말 거부" );
	m_uiEtcSwitchStr[Party].SetString( "파티요청 거부" );
	m_uiEtcSwitchStr[Guild].SetString( "길드요청 거부" );
	m_uiEtcSwitchStr[Trade].SetString( "거래요청 거부" );
	m_uiEtcSwitchStr[Hint].SetString( "도움말 켜기" );
	m_uiEtcSwitchStr[AutoSeeDist].SetString( "자동 시야거리" );
	m_uiEtcSwitchStr[DetailTexture].SetString( "디테일텍스쳐 켜기" );


	m_uiCamTitleStr.SetString( "카메라 시점" );
	

	// --------------------------------------------------------------	
	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 7 );
	m_uiAniTitleBar.SetPos( m_ptPos.x, m_ptPos.y );
	m_uiCloseButton.SetPos( m_ptPos.x + 211, m_ptPos.y + 4 );	
	m_uiHelpButton.SetPos( m_ptPos.x + 194, m_ptPos.y + 4 );

	m_uiLogOutButton.SetPos( m_ptPos.x + 10, m_ptPos.y + 38 );
	m_uiExitButton.SetPos( m_ptPos.x + 119, m_ptPos.y + 38 );

	m_uiSoundTitleStr.SetPos( m_ptPos.x + 11, m_ptPos.y + 80 );
	m_uiSoundStr[Effect_Snd].SetPos( m_ptPos.x + 156, m_ptPos.y + 97 );
	m_uiSoundCtrl[Effect_Snd].SetPos( m_ptPos.x + 25, m_ptPos.y + 113 );
	m_uiSoundStr[BGM].SetPos( m_ptPos.x + 156, m_ptPos.y + 141 );
	m_uiSoundCtrl[BGM].SetPos( m_ptPos.x + 24, m_ptPos.y + 157 );

	m_uiGraphicTitleStr.SetPos( m_ptPos.x + 11, m_ptPos.y + 188 );
	m_uiGraphicStr[Gamma].SetPos( m_ptPos.x + 164, m_ptPos.y + 205 );
	m_uiGraphicCtrl[Gamma].SetPos( m_ptPos.x + 24, m_ptPos.y + 221 );
	m_uiGraphicStr[DynamicLighting].SetPos( m_ptPos.x + 134, m_ptPos.y + 258 );
	m_uiGraphicCtrl[DynamicLighting].SetPos( m_ptPos.x + 24, m_ptPos.y + 274 );
	m_uiGraphicStr[Shadow].SetPos( m_ptPos.x + 158, m_ptPos.y + 311 );
	m_uiGraphicCtrl[Shadow].SetPos( m_ptPos.x + 24, m_ptPos.y + 327 );
	m_uiGraphicStr[SeeDist].SetPos( m_ptPos.x + 155, m_ptPos.y + 364 );
	m_uiGraphicCtrl[SeeDist].SetPos( m_ptPos.x + 24, m_ptPos.y + 382 );

	m_uiGraphicNum[Gamma].SetPos( m_ptPos.x + 31, m_ptPos.y + 233 );
	m_uiGraphicNum[DynamicLighting].SetPos( m_ptPos.x + 30, m_ptPos.y + 283 );
	m_uiGraphicNum[Shadow].SetPos( m_ptPos.x + 30, m_ptPos.y + 336 );
	m_uiGraphicNum[SeeDist].SetPos( m_ptPos.x + 30, m_ptPos.y + 390 );

	m_uiEtcTitleStr.SetPos( m_ptPos.x + 11, m_ptPos.y + 434 );
	for( i = 0; i < Trade; ++i )
	{
		m_uiEtcSwitch[i].SetPos( m_ptPos.x + 8, m_ptPos.y + 468 + 19 * i );
		m_uiEtcSwitchStr[i].SetPos( m_uiEtcSwitch[i].m_ptPos.x + m_uiEtcSwitch[i].m_ptSize.x + 5,
									   m_uiEtcSwitch[i].m_ptPos.y );
	}
	for( i = Trade; i < ETC_OPTION_NUM-1; ++i )
	{
		m_uiEtcSwitch[i].SetPos( m_ptPos.x + 116, m_ptPos.y + 468 + 19 * (i - 3) );
		m_uiEtcSwitchStr[i].SetPos( m_uiEtcSwitch[i].m_ptPos.x + m_uiEtcSwitch[i].m_ptSize.x + 5,
									   m_uiEtcSwitch[i].m_ptPos.y );
	}	
	m_uiEtcSwitch[DetailTexture].SetPos( m_ptPos.x + 8, m_ptPos.y + 412 );
	m_uiEtcSwitchStr[DetailTexture].SetPos( m_uiEtcSwitch[DetailTexture].m_ptPos.x + m_uiEtcSwitch[DetailTexture].m_ptSize.x + 5,
											m_uiEtcSwitch[DetailTexture].m_ptPos.y );


	m_uiCamTitleStr.SetPos( m_ptPos.x + 25, m_ptPos.y + 541 );
	for( i = 0; i < TOTAL_CAMERA_NUM; ++i )
	{
		m_uiCamSelectObj[i].SetPos( m_ptPos.x + 130 + (17 * i), m_ptPos.y + 536 );
	}	
	

	// --------------------------------------------------------------	
	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );

	m_uiLogOutButton.AddMouseEventHandler( this );
	m_uiExitButton.AddMouseEventHandler( this );

	for( i = 0; i < SOUND_OPTION_NUM; ++i )
	{
		m_uiSoundCtrl[i].AddScrollHandler( this );
	}

	for( i = 0; i < GRAPHIC_OPTION_NUM; ++i )
	{
		m_uiGraphicCtrl[i].AddScrollHandler( this );
	}

	for( i = 0; i < ETC_OPTION_NUM; ++i )
	{
		m_uiEtcSwitch[i].AddMouseEventHandler( this );
		m_uiEtcSwitchStr[i].AddMouseEventHandler( this );
	}

	
	Add( &m_uiAniTitleBar );
	Add( &m_uiTitle );
	Add( &m_uiCloseButton );
	Add( &m_uiHelpButton );

	
	Add( &m_uiLogOutButton );
	Add( &m_uiExitButton );

	Add( &m_uiSoundTitleStr );
	for( i = 0; i < SOUND_OPTION_NUM; ++i )
	{
		Add( &m_uiSoundStr[i] );
		Add( &m_uiSoundCtrl[i] );
	}
	
	Add( &m_uiGraphicTitleStr );
	for( i = 0; i < GRAPHIC_OPTION_NUM; ++i )
	{
		Add( &m_uiGraphicStr[i] );
		Add( &m_uiGraphicCtrl[i] );
		Add( &m_uiGraphicNum[i] );
	}

	Add( &m_uiEtcTitleStr );
	for( i = 0; i < ETC_OPTION_NUM; ++i )
	{
		Add( &m_uiEtcSwitch[i] );
		Add( &m_uiEtcSwitchStr[i] );
	}

	Add( &m_uiCamTitleStr );
	for( i = 0; i < TOTAL_CAMERA_NUM; ++i )
	{
		m_uiCamSelectObj[i].AddMouseEventHandler( this );
		Add( &m_uiCamSelectObj[i] );
	}
	
	// --------------------------------------------------------------
	
	SetSprite();

	SetARGB( 0xCCFFFFFF );

#define	TITLE_COLOR		D3DCOLOR_XRGB(195,193,190)
#define	SUBTITLE_COLOR	D3DCOLOR_XRGB(212,208,190)
#define	TEXT_COLOR		D3DCOLOR_XRGB(195,193,190)
	
	m_uiTitle.GetFont()->dwColor = TITLE_COLOR;
	
	m_uiSoundTitleStr.GetFont()->dwColor = SUBTITLE_COLOR;
	for( i = 0; i < SOUND_OPTION_NUM; ++i )
		m_uiSoundStr[i].GetFont()->dwColor = TEXT_COLOR;

	m_uiGraphicTitleStr.GetFont()->dwColor = SUBTITLE_COLOR;
	for( i = 0; i < GRAPHIC_OPTION_NUM; ++i )
		m_uiGraphicStr[i].GetFont()->dwColor = TEXT_COLOR;

	m_uiEtcTitleStr.GetFont()->dwColor = SUBTITLE_COLOR;
	for( i = 0; i < ETC_OPTION_NUM; ++i )
		m_uiEtcSwitchStr[i].GetFont()->dwColor = TEXT_COLOR;

	m_uiCamTitleStr.GetFont()->dwColor = TEXT_COLOR;

	m_uiLogOutButton.SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );
	m_uiExitButton.SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );

	m_uiLogOutButton.SetLabelStyle( R3_HAN_OUTLINE );
	m_uiExitButton.SetLabelStyle( R3_HAN_OUTLINE );


	// --------------------------------------------------------------
	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );

	SetMoveObject( &m_uiAniTitleBar );
	SetMoveObject( &m_uiTitle );
	SetMovable( TRUE );


//	m_uiSoundCtrl[Effect_Snd].SetTotalAmount( 10 );
	m_uiSoundCtrl[Effect_Snd].SetTotalAmount( 160 );
	m_uiSoundCtrl[Effect_Snd].SetAdjustmentUnit( 1 );
	m_uiSoundCtrl[Effect_Snd].SetArrowAdjustmentUnit( 8 );

//	m_uiSoundCtrl[BGM].SetTotalAmount( 10 );
	m_uiSoundCtrl[BGM].SetTotalAmount( 160 );
	m_uiSoundCtrl[BGM].SetAdjustmentUnit( 1 );
	m_uiSoundCtrl[BGM].SetArrowAdjustmentUnit( 8 );


	m_uiGraphicCtrl[Gamma].SetTotalAmount( 10 );
	m_uiGraphicCtrl[DynamicLighting].SetTotalAmount( 3 );	
	m_uiGraphicCtrl[Shadow].SetTotalAmount( 3 );
	m_uiGraphicCtrl[SeeDist].SetTotalAmount( 4 );
	
	for( i=0; i<ETC_OPTION_NUM; ++i )
		SwitchOn( (EtcOption)i, FALSE );


	SoundOn( TRUE );

	m_bIsCreated = TRUE;
}

void
CGUIOptionWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );

		l_spriteInfo.wActionNo = 1;
		
		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );		

		// scrollbar	
		SPRITE_INFO l_sScrollbarSprite[2];
		memcpy( &l_sScrollbarSprite[0], &l_spriteInfo, sizeof( SPRITE_INFO ) );
		memcpy( &l_sScrollbarSprite[1], &l_spriteInfo, sizeof( SPRITE_INFO ) );	

		for( int i = 0; i < SOUND_OPTION_NUM; ++i )
		{
			l_sScrollbarSprite[0].wFrameNo = 5;
			m_uiSoundCtrl[i].SetSpriteInfo( &l_sScrollbarSprite[0] );

			l_sScrollbarSprite[0].wFrameNo = 6;
			m_uiSoundCtrl[i].SetBarSpriteInfo( &l_sScrollbarSprite[0] );

			l_sScrollbarSprite[0].wFrameNo = 1;
			l_sScrollbarSprite[1].wFrameNo = 2;
			m_uiSoundCtrl[i].SetTopArrowSpriteInfo( l_sScrollbarSprite );

			l_sScrollbarSprite[0].wFrameNo = 3;
			l_sScrollbarSprite[1].wFrameNo = 4;
			m_uiSoundCtrl[i].SetBottomArrowSpriteInfo( l_sScrollbarSprite );			
		}	

		for( i = 0; i < GRAPHIC_OPTION_NUM; ++i )
		{
			l_sScrollbarSprite[0].wFrameNo = 5;
			m_uiGraphicCtrl[i].SetSpriteInfo( &l_sScrollbarSprite[0] );

			l_sScrollbarSprite[0].wFrameNo = 6;
			m_uiGraphicCtrl[i].SetBarSpriteInfo( &l_sScrollbarSprite[0] );

			l_sScrollbarSprite[0].wFrameNo = 1;
			l_sScrollbarSprite[1].wFrameNo = 2;
			m_uiGraphicCtrl[i].SetTopArrowSpriteInfo( l_sScrollbarSprite );

			l_sScrollbarSprite[0].wFrameNo = 3;
			l_sScrollbarSprite[1].wFrameNo = 4;
			m_uiGraphicCtrl[i].SetBottomArrowSpriteInfo( l_sScrollbarSprite );			
		}

		l_spriteInfo.wFrameNo = 9;
		m_uiGraphicNum[Gamma].SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 10;
		m_uiGraphicNum[DynamicLighting].SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 21;
		m_uiGraphicNum[Shadow].SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 22;
		m_uiGraphicNum[SeeDist].SetSpriteInfo( &l_spriteInfo );
		

		l_spriteInfo.wFrameNo = 8;
		for( i = 0; i < ETC_OPTION_NUM; ++i )
			m_uiEtcSwitch[i].SetSpriteInfo( &l_spriteInfo );

		
		for( i = 0; i < TOTAL_CAMERA_NUM; ++i )
		{
			l_spriteInfo.wFrameNo = 11 + i;
			m_uiCamSelectObj[i].SetSpriteInfo( &l_spriteInfo );
		}
		

		// logout, exit button
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );	
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 16;
		m_uiLogOutButton.SetSpriteInfoEx( &l_spriteInfo );
		m_uiExitButton.SetSpriteInfoEx( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 17;
		m_uiLogOutButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		m_uiExitButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );

		l_spriteInfo.wFrameNo = 18;
		m_uiLogOutButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );		
		m_uiExitButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );


		// anibar
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );	
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );	

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
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		if( !l_pSprite )
			return;

		GetSpriteInfo()->pSprite = l_pSprite;		
		
		for( int i = 0; i < SOUND_OPTION_NUM; ++i )
			m_uiSoundCtrl[i].SetSprite( l_pSprite );

		for( i = 0; i < GRAPHIC_OPTION_NUM; ++i )
		{
			m_uiGraphicCtrl[i].SetSprite( l_pSprite );		
			m_uiGraphicNum[i].GetSpriteInfo()->pSprite = l_pSprite;
		}		
		
		for( i = 0; i < ETC_OPTION_NUM; ++i )
			m_uiEtcSwitch[i].GetSpriteInfo()->pSprite = l_pSprite;

		for( i = 0; i < TOTAL_CAMERA_NUM; ++i )
		{			
			m_uiCamSelectObj[i].GetSpriteInfo()->pSprite = l_pSprite;
		}


		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		m_uiLogOutButton.SetSprite( l_pSprite );				
		m_uiExitButton.SetSprite( l_pSprite );
		m_uiCloseButton.SetSprite( l_pSprite );
		m_uiHelpButton.SetSprite( l_pSprite );

		// anibar
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;	
	}

}

void
CGUIOptionWindow::Open( void )
{
	CGUIWindow::Open();

	// 옵션창을 여는 동안 항상 활성화된 창을 끈다.
//	EP_OpenDialogBox( this, NULL );
}

void
CGUIOptionWindow::Close( void )
{
	CGUIWindow::Close();

//	EP_CloseDialogBox( this );
}

void	
CGUIOptionWindow::SetSoundOption( SoundOption pi_eSoundOption, float pi_fValue )
{
	if( pi_eSoundOption >= SOUND_OPTION_NUM )
		return;

	m_uiSoundCtrl[pi_eSoundOption].SetCurrentAmount( pi_fValue * m_uiSoundCtrl[pi_eSoundOption].GetTotalAmount() );	
}

float
CGUIOptionWindow::GetSoundOption( SoundOption pi_eSoundOption )
{
	if( pi_eSoundOption >= SOUND_OPTION_NUM )
		return 0;

	return (float)m_uiSoundCtrl[pi_eSoundOption].GetCurrentAmount() / m_uiSoundCtrl[pi_eSoundOption].GetTotalAmount();
}

void
CGUIOptionWindow::SetGraphicOption( GraphicOption pi_eGraphicOption, float pi_fValue )
{
	if( pi_eGraphicOption >= GRAPHIC_OPTION_NUM )
		return;

	if( pi_eGraphicOption == Gamma )
	{
		m_uiGraphicCtrl[pi_eGraphicOption].SetCurrentAmount( (pi_fValue - 0.8) * m_uiGraphicCtrl[pi_eGraphicOption].GetTotalAmount() );
	}
	else 
	{
		m_uiGraphicCtrl[pi_eGraphicOption].SetCurrentAmount( pi_fValue );
	}
}

float
CGUIOptionWindow::GetGraphicOption( GraphicOption pi_eGraphicOption )
{
	if( pi_eGraphicOption >= GRAPHIC_OPTION_NUM )
		return 0;

	return m_uiGraphicCtrl[pi_eGraphicOption].GetCurrentAmount();
}

void
CGUIOptionWindow::SwitchOn( EtcOption pi_eOption, BOOL pi_bSwitchOn )
{
	if( pi_eOption >= ETC_OPTION_NUM )
		return;

	if( pi_bSwitchOn )
		m_uiEtcSwitch[pi_eOption].GetSpriteInfo()->wFrameNo = 7;
	else
		m_uiEtcSwitch[pi_eOption].GetSpriteInfo()->wFrameNo = 8;
}

BOOL
CGUIOptionWindow::IsSwitchOn( EtcOption pi_eOption )
{
	if( pi_eOption >= ETC_OPTION_NUM )
		return FALSE;

	if( m_uiEtcSwitch[pi_eOption].GetSpriteInfo()->wFrameNo == 7 )
		return TRUE;
	else
		return FALSE;
}

void
CGUIOptionWindow::SelectCamera( BYTE pi_byCamIndex )
{
	if( m_bySelectedCamera == pi_byCamIndex )
		return;

	if( pi_byCamIndex >= TOTAL_CAMERA_NUM )
		return;

	BYTE l_byPrevSelectedCam;
	
	l_byPrevSelectedCam = m_bySelectedCamera;	

	if( l_byPrevSelectedCam < TOTAL_CAMERA_NUM )
	{
		// unselect
		m_uiCamSelectObj[l_byPrevSelectedCam].GetSpriteInfo()->wFrameNo = 11 + l_byPrevSelectedCam;
	}


	m_bySelectedCamera = pi_byCamIndex;
	// select 
	m_uiCamSelectObj[m_bySelectedCamera].GetSpriteInfo()->wFrameNo = 16 + m_bySelectedCamera;
}

void
CGUIOptionWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiLogOutButton )
		{			
			if( gScreen.ExistObject() )
				return;

			// ################## 코드 추가 시킬 곳 ###########################
			// 로그 아웃
			EP_Logout();
		}
		else if( event.source == &m_uiExitButton )
		{
			if( gScreen.ExistObject() )
				return;

			// ################## 코드 추가 시킬 곳 ###########################
			// 게임 종료
			EP_QuitProgram();
		}
		else if( event.source == &m_uiCloseButton )
		{
			Close();
		}
		else if( event.source == &m_uiHelpButton )
		{
			// 도움말 창 열기
			PostMessage( gScreen.GetHWND(), WM_OPEN_WINDOW_HELP, m_dwID, 0 );
		}
	}	
	else if( event.id == MOUSE_STROKED )
	{
		for( int i = 0; i < ETC_OPTION_NUM; ++i )
		{
			if( event.source == &m_uiEtcSwitch[i] ||
				event.source == &m_uiEtcSwitchStr[i] )
			{
				//SwitchOn( (EtcOption)i, !IsSwitchOn( (EtcOption)i ) );
				if( i == Hint )
					PostMessage( gScreen.GetHWND(), WM_OPTION_TOGGLE_HINT, !IsSwitchOn( (EtcOption)i ), 0 );
				else if( i == AutoSeeDist )
					PostMessage( gScreen.GetHWND(), WM_OPTION_TOGGLE_AUTO_VF_ADJUST, !IsSwitchOn( (EtcOption)i ), 0 );
				else if( i == DetailTexture )
					PostMessage( gScreen.GetHWND(), WM_OPTION_DETAIL_TEXTURE, !IsSwitchOn( (EtcOption)i ), 0 );

				return;
			}
		}	

		for( i = 0; i < TOTAL_CAMERA_NUM; ++i )
		{
			if( event.source == &m_uiCamSelectObj[i] )
			{
				if( m_bySelectedCamera != i )
				{
					// ################## 코드 추가 시킬 곳 ###########################
					// 카메라 변경
					PostMessage( gScreen.GetHWND(), WM_OPTION_CHANGE_CAMERA_MODE, i, 0 );
				}

				return;
			}
		}

	}

}

void
CGUIOptionWindow::ScrollEvent( _scroll_event & event )
{
	// sound
	if( event.source == &m_uiSoundCtrl[SoundOption::Effect_Snd] )
	{
		PostMessage( gScreen.GetHWND(), WM_OPTION_ADJUST_EFFECT_SOUND, 0, 0 );
	}
	else if( event.source == &m_uiSoundCtrl[SoundOption::BGM] )
	{
		PostMessage( gScreen.GetHWND(), WM_OPTION_ADJUST_BGM_SOUND, 0, 0 );
	}
	// graphic
	else if( event.source == &m_uiGraphicCtrl[GraphicOption::Gamma] )
	{		
		PostMessage( gScreen.GetHWND(), WM_OPTION_ADJUST_GAMMA, m_uiGraphicCtrl[Gamma].GetCurrentAmount(), 0 );
	}
	else if( event.source == &m_uiGraphicCtrl[GraphicOption::DynamicLighting] )
	{
		PostMessage( gScreen.GetHWND(), WM_OPTION_ADJUST_DYNAMIC_LIGHTING, m_uiGraphicCtrl[DynamicLighting].GetCurrentAmount(), 0 );
	}
	else if( event.source == &m_uiGraphicCtrl[GraphicOption::Shadow] )
	{
		PostMessage( gScreen.GetHWND(), WM_OPTION_ADJUST_SHADOW, m_uiGraphicCtrl[Shadow].GetCurrentAmount(), 0 );
	}
	else if( event.source == &m_uiGraphicCtrl[GraphicOption::SeeDist] )
	{
		PostMessage( gScreen.GetHWND(), WM_OPTION_CHANGE_VF_DIST, m_uiGraphicCtrl[SeeDist].GetCurrentAmount(), 0 );
	}
}