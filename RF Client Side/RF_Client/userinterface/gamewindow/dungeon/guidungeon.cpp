////////////////////////////////////////////////////////////////////////////
//
// CGUIDungeon UI Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "GUIDungeon.h"
#include "../../SpriteMgr.h"
#include "../../GUIController/GUIScreen.h"

CGUIKillingCounter::CGUIKillingCounter()
{
	m_wMaxKillingNum	= 0;
	m_wCurKillingNum	= 0;

	m_byPolyNum			= 0;

	for( int i=0; i<POLY_NUM; ++i )
		for( int j=0; j<3; ++j )
		{
			m_listVertex[i][j].color = 0x7FFFFFFF;			
			m_listVertex[i][j].z = 0.5f;
			m_listVertex[i][j].w = 1.0f;
		}

	m_bIsCreated		= FALSE;
}

CGUIKillingCounter::~CGUIKillingCounter()
{

}

void
CGUIKillingCounter::Init( void )
{
	SetSize( 144, 126 );

	SetSprite();

	SetARGB( 0x60FFFFFF );

	SetName( "KillCounter" );

	m_bIsCreated = TRUE;
}

void
CGUIKillingCounter::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_DUNGEON );

		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );

		m_sGaugeSpriteInfo.pSprite = l_spriteInfo.pSprite;
		m_sGaugeSpriteInfo.wActionNo = 0;
		m_sGaugeSpriteInfo.wFrameNo = 1;

		m_fGaugeUV[0] = (float)m_ptSize.x / m_sGaugeSpriteInfo.pSprite->GetSpriteXL( m_sGaugeSpriteInfo.wActionNo, m_sGaugeSpriteInfo.wFrameNo );
		m_fGaugeUV[1] = (float)m_ptSize.y / m_sGaugeSpriteInfo.pSprite->GetSpriteYL( m_sGaugeSpriteInfo.wActionNo, m_sGaugeSpriteInfo.wFrameNo );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_DUNGEON );
		if( !l_pSprite )
			return;

		GetSpriteInfo()->pSprite = l_pSprite;

		m_sGaugeSpriteInfo.pSprite = l_pSprite;
	}
}

void
CGUIKillingCounter::SetMaxKillingNum( WORD pi_wMaxKillingNum )
{
	m_wMaxKillingNum = pi_wMaxKillingNum;

	if( m_wMaxKillingNum < m_wCurKillingNum )
		m_wMaxKillingNum = m_wCurKillingNum;

	// update current killing gauge
	if( m_wMaxKillingNum != pi_wMaxKillingNum )
		SetCurrentKillingNum( m_wCurKillingNum );
}

void
CGUIKillingCounter::SetCurrentKillingNum( WORD pi_wCurKillingNum )
{
	m_wCurKillingNum = pi_wCurKillingNum;

	if( m_wCurKillingNum > m_wCurKillingNum )
		m_wCurKillingNum = m_wMaxKillingNum;

	float l_fGauge = (float)m_wCurKillingNum / m_wMaxKillingNum;
	float l_fTempGauge;

	// set vertex
	m_byPolyNum = 2;

	m_listVertex[0][0].x = m_ptPos.x;
	m_listVertex[0][0].y = m_ptPos.y + m_ptSize.y;
	m_listVertex[0][0].u = 0.f;
	m_listVertex[0][0].v = m_fGaugeUV[1];

	m_listVertex[0][1].x = m_ptPos.x;
	m_listVertex[0][1].y = m_ptPos.y + m_ptSize.y/2;
	m_listVertex[0][1].u = 0.f;
	m_listVertex[0][1].v = m_fGaugeUV[1] * 0.5;

	l_fTempGauge = l_fGauge/0.3f;
	if( l_fTempGauge > 1.f )
		l_fTempGauge = 1.f;

	m_listVertex[0][2].x = m_ptPos.x + m_ptSize.x/2 * l_fTempGauge;
	m_listVertex[0][2].y = m_ptPos.y + m_ptSize.y/2;
	m_listVertex[0][2].u = m_fGaugeUV[0]*0.5f * l_fTempGauge;
	m_listVertex[0][2].v = m_fGaugeUV[1] * 0.5;		

	memcpy( &m_listVertex[1][0], &m_listVertex[0][0], sizeof(_D3DR3TLVERTEX_TEX1) );		
	memcpy( &m_listVertex[1][1], &m_listVertex[0][2], sizeof(_D3DR3TLVERTEX_TEX1) );

	l_fTempGauge = (l_fGauge - 0.3)/0.2f;
	if( l_fTempGauge < 0.f )
		l_fTempGauge = 0.f;
	else if( l_fTempGauge > 1.f )
		l_fTempGauge = 1.f;
	
	m_listVertex[1][2].x = m_listVertex[0][2].x + m_ptSize.x / 2 * l_fTempGauge;
	m_listVertex[1][2].y = m_ptPos.y + m_ptSize.y;
	m_listVertex[1][2].u = m_listVertex[0][2].u + m_fGaugeUV[0] * 0.5f * l_fTempGauge;
	m_listVertex[1][2].v = m_fGaugeUV[1];
	
	if( l_fGauge <= 0.5f )
		return;

	++m_byPolyNum;	
	
	memcpy( &m_listVertex[2][0], &m_listVertex[1][2], sizeof(_D3DR3TLVERTEX_TEX1) );		
	memcpy( &m_listVertex[2][1], &m_listVertex[1][1], sizeof(_D3DR3TLVERTEX_TEX1) );

	l_fTempGauge = (l_fGauge - 0.5)/0.4f;
	if( l_fTempGauge > 1.f )
		l_fTempGauge = 1.f;

	m_listVertex[2][2].x = m_ptPos.x + m_ptSize.x;
	m_listVertex[2][2].y = m_ptPos.y + m_ptSize.y * ( 1.f - l_fTempGauge );
	m_listVertex[2][2].u = m_fGaugeUV[0];
	m_listVertex[2][2].v = m_fGaugeUV[1] * ( 1.f - l_fTempGauge );

	if( l_fGauge <= 0.9f )
		return;

	++m_byPolyNum;	

	memcpy( &m_listVertex[3][0], &m_listVertex[2][2], sizeof(_D3DR3TLVERTEX_TEX1) );
	// 끝부분만 중심이 다르다.
	m_listVertex[3][1].x = m_ptPos.x + 66;
	m_listVertex[3][1].y = m_ptPos.y + 55;
	m_listVertex[3][1].u = m_fGaugeUV[0] * 66.f / m_ptSize.x;
	m_listVertex[3][1].v = m_fGaugeUV[1] * 55.f / m_ptSize.y;

	memcpy( &m_listVertex[3][1], &m_listVertex[0][2], sizeof(_D3DR3TLVERTEX_TEX1) );// 

	l_fTempGauge = (l_fGauge - 0.9)/0.1f;

	m_listVertex[3][2].x = m_ptPos.x + 112 + 32 * ( 1.f - l_fTempGauge );
	m_listVertex[3][2].y = m_ptPos.y;
	m_listVertex[3][2].u = m_fGaugeUV[0] * (float)(m_listVertex[3][2].x - m_ptPos.x) / m_ptSize.x;
	m_listVertex[3][2].v = 0.f;	
}

BOOL
CGUIKillingCounter::Draw( void )
{
	if( !IsShow() )
		return FALSE;	

	// draw gauge
	if( m_byPolyNum )
	{
		m_sGaugeSpriteInfo.pSprite->SetAction( m_sGaugeSpriteInfo.wActionNo );
		m_sGaugeSpriteInfo.pSprite->SetFrame( m_sGaugeSpriteInfo.wFrameNo );
		
		for( int i=0; i<m_byPolyNum; ++i )
			m_sGaugeSpriteInfo.pSprite->DrawSpriteVertex( m_listVertex[i], 1 );
	}

	CGUIContainer::Draw();

	DrawDialogBox();

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

CGUIMissionStateWin::CGUIMissionStateWin()
{
	m_dwShowTime		= 8000;	// 8초동안 보여준다.
	m_dwFadeOutInterval	= 800;	// 0.8초동안 fadeout된다.

	m_dwShowStartTime	= 0;
	m_bIsStartedFadeOut	= FALSE;

	m_dwMsgBoardColor	= 0x00272824;
	m_dwMissionInfoColor= 0x00FFFFFF;

	m_eMissionState		= NoneState;

	m_bIsCreated		= FALSE;
}

CGUIMissionStateWin::~CGUIMissionStateWin()
{

}

void
CGUIMissionStateWin::Init( void )
{
//	SetSize( gScreen.m_ptSize ); // UI event가 안일어나게 하기 위해 size를 0으로한다.

	m_uiMsgBoard.SetSize( gScreen.m_ptSize.x, 130 );	

	m_uiMissionText.SetSize( 115, 20 );	

	m_uiMsgBoard.SetPos( 0, ( gScreen.m_ptSize.y - m_uiMsgBoard.m_ptSize.y ) / 2 );
	m_uiMissionText.SetPos( 0, m_uiMsgBoard.m_ptPos.y + 13 );
	m_uiStateText.SetPos( 0, m_uiMsgBoard.m_ptPos.y + 39 );
	m_uiMissionInfoStr[0].SetPos( 0, m_uiMsgBoard.m_ptPos.y + 91 );
	m_uiMissionInfoStr[1].SetPos( 0, m_uiMissionInfoStr[0].m_ptPos.y + 20 );

	Add( &m_uiMsgBoard );
	Add( &m_uiMissionText );
	Add( &m_uiStateText );
	Add( &m_uiMissionInfoStr[0] );
	Add( &m_uiMissionInfoStr[1] );

	Add( &m_uiMissionStr );


	SetSprite();	

	SetAlwaysTop( TRUE );

	SetName( "MissionWin" );
}

void
CGUIMissionStateWin::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_DUNGEON );

		l_spriteInfo.wActionNo = 1;
		l_spriteInfo.wFrameNo = 0;
		m_uiMissionText.SetSpriteInfo( &l_spriteInfo );		

		l_spriteInfo.wFrameNo = 1;
		m_uiStateText.SetSpriteInfo( &l_spriteInfo );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_DUNGEON );
		if( !l_pSprite )
			return;

		m_uiMissionText.GetSpriteInfo()->pSprite = l_pSprite;		

		m_uiStateText.GetSpriteInfo()->pSprite = l_pSprite;
	}
}

void
CGUIMissionStateWin::Open( void )
{
	CGUIWindow::Open();

	// 이전에 창을 닫지 않고 안보이게만 했기 때메 열때 true로 세팅한다.
	m_uiMsgBoard.Show( TRUE );
	m_uiMissionText.Show( TRUE );
	m_uiStateText.Show( TRUE );
	m_uiMissionInfoStr[0].Show( TRUE );
	m_uiMissionInfoStr[1].Show( TRUE );
}

void
CGUIMissionStateWin::Close( void )
{
	CGUIWindow::Close();

	m_eMissionState = NoneState;
}

void
CGUIMissionStateWin::SetMissionState( MissionState pi_eState )
{	
	if( m_eMissionState == pi_eState )
		return;

	m_eMissionState = pi_eState;

	// set Statetext
	m_uiStateText.GetSpriteInfo()->pSprite = m_uiMissionText.GetSpriteInfo()->pSprite;
	m_uiStateText.GetSpriteInfo()->wActionNo = 1;

	if( pi_eState == Start )
	{
		m_uiStateText.SetSize( 173, 32 );	
		m_uiStateText.GetSpriteInfo()->wFrameNo = 1;

		m_uiMsgBoard.SetSize( gScreen.m_ptSize.x, 130 );		

		// set showtime
		SetShowTime( 8000 );
	}
	else if( pi_eState == Clear )
	{
		m_uiStateText.SetSize( 237, 32 );	
		m_uiStateText.GetSpriteInfo()->wFrameNo = 2;	

		m_uiMsgBoard.SetSize( gScreen.m_ptSize.x, 100 );
		m_uiMissionInfoStr[0].SetString( NULL );
		m_uiMissionInfoStr[1].SetString( NULL );

		// set showtime
		SetShowTime( 5000 );
	}
	else if( pi_eState == Fail )
	{
		m_uiStateText.SetSize( 194, 32 );	
		m_uiStateText.GetSpriteInfo()->wFrameNo = 3;

		m_uiMsgBoard.SetSize( gScreen.m_ptSize.x, 100 );
		m_uiMissionInfoStr[0].SetString( NULL );
		m_uiMissionInfoStr[1].SetString( NULL );

		// set showtime
		SetShowTime( 5000 );
	}
	else 
	{
		return;
	}

	// --------------------------------------------------------------------	
	int l_nMoveVel;
	//l_nMoveVel = 1.1 * m_ptSize.x / 2;	
	l_nMoveVel = 500;

	
	g_cMoveObjectMgr.AddMoveObject( &m_uiMissionText, MOVE_AXIS_X,
									gScreen.m_ptSize.x + 10,
									( gScreen.m_ptSize.x - m_uiMissionText.m_ptSize.x ) / 2,
									l_nMoveVel );
	
	g_cMoveObjectMgr.AddMoveObject( &m_uiStateText, MOVE_AXIS_X,
									gScreen.m_ptPos.x - m_uiStateText.m_ptSize.x - 10,
									( gScreen.m_ptSize.x - m_uiStateText.m_ptSize.x ) / 2,
									l_nMoveVel );

	g_cMoveObjectMgr.StartMove( &m_uiMissionText );
	g_cMoveObjectMgr.StartMove( &m_uiStateText );	

	// init
	m_uiMsgBoard.SetARGB( m_dwMsgBoardColor );
//	m_uiMissionText.SetARGB( -1L );
//	m_uiStateText.SetARGB( -1L );
	m_uiMissionInfoStr[0].GetFont()->dwColor = m_dwMissionInfoColor;
	m_uiMissionInfoStr[1].GetFont()->dwColor = m_dwMissionInfoColor;

	m_dwShowMissionInfoStartTime = 0;
	m_bIsStartedFadeOut = FALSE;


	//	if( !IsShow() )
		Open();
	
	m_dwShowStartTime = timeGetTime();	
}

void
CGUIMissionStateWin::SetMissionInfoStr( char *pi_pInfoStr[2] )
{
	m_uiMissionInfoStr[0].SetString( pi_pInfoStr[0] );
	m_uiMissionInfoStr[1].SetString( pi_pInfoStr[1] );	

	m_uiMissionInfoStr[0].SetPos( ( gScreen.m_ptSize.x - m_uiMissionInfoStr[0].m_ptSize.x )/2, m_uiMissionInfoStr[0].m_ptPos.y );
	m_uiMissionInfoStr[1].SetPos( ( gScreen.m_ptSize.x - m_uiMissionInfoStr[1].m_ptSize.x )/2, m_uiMissionInfoStr[1].m_ptPos.y );


	m_uiMissionStr.SetString( pi_pInfoStr[0] );
	m_uiMissionStr.SetPos( ( gScreen.m_ptSize.x - m_uiMissionStr.m_ptSize.x )/2, 5 );
}

void
CGUIMissionStateWin::SetMissionInfoStrColor( DWORD pi_dwColor )
{
	m_dwMissionInfoColor = pi_dwColor & 0x00FFFFFF;

	m_uiMissionInfoStr[0].GetFont()->dwColor = m_dwMissionInfoColor & (m_uiMissionInfoStr[0].GetFont()->dwColor & 0xFF000000);
	m_uiMissionInfoStr[1].GetFont()->dwColor = m_uiMissionInfoStr[0].GetFont()->dwColor;
}

void
CGUIMissionStateWin::SetShowTime( DWORD pi_dwTime )
{
	if( pi_dwTime < m_dwFadeOutInterval )
		return;

	m_dwShowTime = pi_dwTime;
}

#define	BOARD_ALPHA	0x80

void
CGUIMissionStateWin::Update( void )
{
	if( !IsShow() )
		return;

	if( m_eMissionState == NoneState )
		return;

	if( timeGetTime() - m_dwShowStartTime > m_dwShowTime )
	{
//		CGUIWindow::SendWindowEvent( WINDOW_CLOSED );

		// 미션정보를 보여줘야하기 때메 창을 안닫고 안보이게만 한다.
		m_eMissionState = NoneState;		

		m_uiMsgBoard.Show( FALSE );
		m_uiMissionText.Show( FALSE );
		m_uiStateText.Show( FALSE );
		m_uiMissionInfoStr[0].Show( FALSE );
		m_uiMissionInfoStr[1].Show( FALSE );		

		return;
	}

	int l_nAlpha;

	if( timeGetTime() - m_dwShowStartTime < m_dwShowTime - m_dwFadeOutInterval )
	{
		// fade in 

		// ----------------------------------------------------------------
		// message board
		l_nAlpha = (float)( timeGetTime() - m_dwShowStartTime )/1500 * BOARD_ALPHA;
		if( l_nAlpha > BOARD_ALPHA )
			l_nAlpha = BOARD_ALPHA;
		
		m_uiMsgBoard.SetARGB( (l_nAlpha << 24) | m_dwMsgBoardColor );


		// ----------------------------------------------------------------
		// mission info string
		if( m_dwShowMissionInfoStartTime == 0 )		
		{
			// start
			if( //g_cMoveObjectMgr.IsFinishedMove( &m_uiMissionText ) )
				timeGetTime() - m_dwShowStartTime > 1000 )
			{
				m_dwShowMissionInfoStartTime = timeGetTime();	
			}
		}
		else
		{
			// update
			l_nAlpha = (float)( timeGetTime() - m_dwShowMissionInfoStartTime )/900 * 0xFF;
			if( l_nAlpha > 0xFF )
				l_nAlpha = 0xFF;
			
			m_uiMissionInfoStr[0].GetFont()->dwColor = (l_nAlpha << 24) | m_dwMissionInfoColor;
			m_uiMissionInfoStr[1].GetFont()->dwColor = m_uiMissionInfoStr[0].GetFont()->dwColor;
		}
	}
	else
	{
		// fade out

		// ----------------------------------------------------------------
		// message board
		l_nAlpha = (float)( m_dwShowTime - ( timeGetTime() - m_dwShowStartTime ) ) / m_dwFadeOutInterval * BOARD_ALPHA;
		if( l_nAlpha < 0 )
			l_nAlpha = 0;
		else if( l_nAlpha > BOARD_ALPHA )
			l_nAlpha = BOARD_ALPHA;
		
		m_uiMsgBoard.SetARGB( (l_nAlpha << 24) | m_dwMsgBoardColor );

		
		// ----------------------------------------------------------------
		// message board보다 text를 먼저 fadeout한다.
		l_nAlpha = (float)( m_dwShowTime - ( timeGetTime() - m_dwShowStartTime + (m_dwFadeOutInterval*0.4) ) ) / m_dwFadeOutInterval * BOARD_ALPHA;
		if( l_nAlpha < 0  )
			l_nAlpha = 0;
		else if( l_nAlpha > BOARD_ALPHA )
			l_nAlpha = BOARD_ALPHA;
		
		m_uiMissionInfoStr[0].GetFont()->dwColor = (l_nAlpha << 24) | m_dwMissionInfoColor;
		m_uiMissionInfoStr[1].GetFont()->dwColor = m_uiMissionInfoStr[0].GetFont()->dwColor;
		
		if( !m_bIsStartedFadeOut )
		{
			g_cMoveObjectMgr.AddMoveObject( &m_uiMissionText, MOVE_AXIS_X,
											m_uiMissionText.m_ptPos.x,
											gScreen.m_ptPos.x - m_uiMissionText.m_ptSize.x - 10,										
											500 );
		
			g_cMoveObjectMgr.AddMoveObject( &m_uiStateText, MOVE_AXIS_X,
											m_uiStateText.m_ptPos.x,
											gScreen.m_ptSize.x + 10,
											500 );

			g_cMoveObjectMgr.StartMove( &m_uiMissionText );
			g_cMoveObjectMgr.StartMove( &m_uiStateText );

			m_bIsStartedFadeOut = TRUE;
		}		

//		m_uiMissionText.SetARGB( l_nAlpha << 24 | 0x00FFFFFF );		
//		m_uiStateText.SetARGB( m_uiMissionText.GetARGB() );
	}
	
}