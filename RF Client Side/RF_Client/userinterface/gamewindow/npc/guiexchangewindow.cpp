////////////////////////////////////////////////////////////////////////////
//
// CGUIExchangeWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIExchangeWindow.h"
#include "../../SpriteMgr.h"
#include "../../UIGlobal.h"
#include "../../GameWindow/GUIInventoryWindow.h"
#include "../../GUIController/GUIMessagebox.h"
#include "../../../DefineMessage.h"
#include "r3Math.h"

////////////////////////////////////////////////////////////////////////////

class CGUIInventoryWindow;
extern CGUIInventoryWindow * g_pUiInventWindow;

////////////////////////////////////////////////////////////////////////////

CGUIEconomyRateGraph::CGUIEconomyRateGraph()
{
	m_byGraphPointNum	= 0;
	for( int i = 0; i < 2; ++i )
	{
		m_pGraphPointList[i]= NULL;
		m_pCtrlPointList[i] = NULL;
	}

	m_nPointRange[0]	= 0;
	m_nPointRange[1]	= 100;

	m_byCurveStep		= 0;

	m_nRenderPointNum	= 0;
	m_pRenderPointList	= NULL;

	m_dwColor			= -1L;
}

CGUIEconomyRateGraph::~CGUIEconomyRateGraph()
{
	ClearPointList();
	ClearRenderPointList();
}

void
CGUIEconomyRateGraph::ClearPointList( void )
{
	for( int i = 0; i < 2; ++i )
	{
		if( m_pGraphPointList[i] )
		{
			delete[] m_pGraphPointList[i];
			m_pGraphPointList[i] = NULL;
		}

		if( m_pCtrlPointList[i] )
		{
			delete[] m_pCtrlPointList[i];
			m_pCtrlPointList[i] = NULL;
		}
	}
}

void
CGUIEconomyRateGraph::ClearRenderPointList( void )
{
	if( m_pRenderPointList )
	{
		delete[] m_pRenderPointList;
		m_pRenderPointList = NULL;
	}
}

void	
CGUIEconomyRateGraph::SetPointRange( int pi_nMinPoint, int pi_nMaxPoint )
{
	m_nPointRange[0] = pi_nMinPoint;
	m_nPointRange[1] = pi_nMaxPoint;
}

void	
CGUIEconomyRateGraph::InitPoint( BYTE pi_byPointNum, WORD * pi_pPointList, int pi_nCurveStep )
{
	if( pi_byPointNum < 2 )
		return;
	
	m_byGraphPointNum = pi_byPointNum;

	ClearPointList();

	// set point width
	m_nPointWidth = m_ptSize.x / ( m_byGraphPointNum - 1 );
	if( !m_nPointWidth )
		return;

	for( int i = 0; i < 2; ++i )
	{
		m_pGraphPointList[i] = new float[m_byGraphPointNum];
		m_pCtrlPointList[i] = new Vector4f[m_byGraphPointNum];
	}

	// set point
	for( i = 0; i < m_byGraphPointNum; ++i )
	{
		SetPoint( i, m_nPointWidth * i, pi_pPointList[i] );
	}

	// set curve detail
	SetCurveStep( pi_nCurveStep );

	CalcGraph();
}

void
CGUIEconomyRateGraph::InsertPoint( WORD pi_wPoint )
{
	if( !m_byGraphPointNum )
		return;

	// x좌표는 새로, y좌표는 한칸씩 shift한다.
	for( int i = 0; i < m_byGraphPointNum - 1; ++i )
	{
		m_pGraphPointList[0][i] = m_ptPos.x + m_nPointWidth * i;
		m_pGraphPointList[1][i] = m_pGraphPointList[1][i+1];
	}

	// set point
	SetPoint( m_byGraphPointNum - 1, m_nPointWidth * ( m_byGraphPointNum - 1 ), pi_wPoint );

	CalcGraph();
}

void	
CGUIEconomyRateGraph::SetPoint( BYTE pi_byListIndex, int pi_nPointX, int pi_nPointY )
{
	if( pi_byListIndex >= m_byGraphPointNum )
		return;

	if( pi_nPointY < m_nPointRange[0] )
		pi_nPointY = m_nPointRange[0];
	else if( pi_nPointY > m_nPointRange[1] )
		pi_nPointY = m_nPointRange[1];

	m_pGraphPointList[0][pi_byListIndex] = m_ptPos.x + pi_nPointX;
	m_pGraphPointList[1][pi_byListIndex] = m_ptPos.y + m_ptSize.y * ( 1 - (float)pi_nPointY / ( m_nPointRange[1] - m_nPointRange[0] ) );
}

void	
CGUIEconomyRateGraph::SetCurveStep( BYTE pi_byStep )
{
	m_byCurveStep = pi_byStep;

	ClearRenderPointList();

	// set total rendering point
	m_nRenderPointNum = ( m_byGraphPointNum - 1 ) * m_byCurveStep + 1;
	m_pRenderPointList = new Vector2f[m_nRenderPointNum];
}

void
CGUIEconomyRateGraph::CalcGraph( void )
{	
	// calc control point
	CalcCubicCurve( m_pCtrlPointList[0], m_byGraphPointNum-1 , m_pGraphPointList[0] );
	CalcCubicCurve( m_pCtrlPointList[1], m_byGraphPointNum-1 , m_pGraphPointList[1] );

	// calc render point
	int nCnt = ( m_byGraphPointNum - 1 ) * m_byCurveStep;
	m_pRenderPointList[nCnt][0] = CalcEvalCubicCurve( m_pCtrlPointList[0][0], 0 );
	m_pRenderPointList[nCnt][1] = CalcEvalCubicCurve( m_pCtrlPointList[1][0], 0 );

	--nCnt;

	float u;
	for( int j = 0; j < m_byGraphPointNum - 1; ++j )
	{
		for( int i = 1; i <= m_byCurveStep; ++i )
		{
			u = i / (float)m_byCurveStep;

			m_pRenderPointList[nCnt][0] = CalcEvalCubicCurve( m_pCtrlPointList[0][j], u );
			m_pRenderPointList[nCnt][1] = CalcEvalCubicCurve( m_pCtrlPointList[1][j], u );

			--nCnt;
		}
	}
}

void
CGUIEconomyRateGraph::SetPos( LONG pi_nPosX, LONG pi_nPosY )
{
	for( int i = 0; i < m_nRenderPointNum ; ++i )
	{
		m_pRenderPointList[i][0] = pi_nPosX + m_pRenderPointList[i][0] - m_ptPos.x;
		m_pRenderPointList[i][1] = pi_nPosY + m_pRenderPointList[i][1] - m_ptPos.y;
	}

	for( i = 0; i < m_byGraphPointNum; ++i )
	{
		m_pGraphPointList[0][i] = pi_nPosX + m_pGraphPointList[0][i] - m_ptPos.x;
		m_pGraphPointList[1][i] = pi_nPosY + m_pGraphPointList[1][i] - m_ptPos.y;
	}
	
	CGUIObject::SetPos( pi_nPosX, pi_nPosY );
}

void
CGUIEconomyRateGraph::SetPos( POINT pi_ptPoint )
{
	SetPos( pi_ptPoint.x, pi_ptPoint.y );
}


#include "character.h"
#include "..\\..\\..\\global.h"
BOOL
CGUIEconomyRateGraph::Draw( void )
{
	if( !IsShow() )
		return FALSE;

	if( !m_nRenderPointNum )
		return FALSE;
	
	SetDefaultFlags();
	ResetVertexBufferCache();
	_GetCharIF()->UnSetState();

	R3Draw2DLine( m_pRenderPointList[m_nRenderPointNum - 1],m_pRenderPointList[0], m_dwColor );

	for( int i = 0; i < m_nRenderPointNum - 1 ; ++i )
//		R3Draw2DLine( m_pRenderPointList[i], m_pRenderPointList[i+1], m_dwColor );

	_GetCharIF()->SetState();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CGUIExchangeWindow::CGUIExchangeWindow()
{	
	m_dwDalantPerGold		= 0;
	m_fExchangeCommission	= 0.f;

	m_bIsCreated			= FALSE;
}

CGUIExchangeWindow::~CGUIExchangeWindow()
{

}

void
CGUIExchangeWindow::Init( void )
{
	// --------------------------------------------------------
	// size
	SetSize( 228, 571 );	
	
	m_uiAniTitleBar.SetSize( 228, 28 );	
	m_uiCloseButton.SetSize( 16, 13 );	
	m_uiHelpButton.SetSize( 16, 13 );

	m_uiEcoBase.SetSize( 207, 31 );
	m_uiMoneyBase.SetSize( 207, 31 );
	m_uiEcoGraphBase.SetSize( 192, 18 );
	m_uiEcoRateBase.SetSize( 192, 18 );

	for( int i = 0; i < 3; ++i )
	{
		m_uiEconomyGraph[i].SetSize( 180, 45 );
		m_uiRaceColor[i].SetSize( 12, 12 );
	}

	m_uiTaxRateBase.SetSize( 192, 18 );
	m_uiExchangeRateBase.SetSize( 192, 18 );


	POINT l_ptSize;
	l_ptSize.x = 12;
	l_ptSize.y = 15;
	m_uiExchangeAdjScrollbar.SetTopArrowSize( l_ptSize );
	m_uiExchangeAdjScrollbar.SetBottomArrowSize( l_ptSize );
	l_ptSize.x = 8;
	m_uiExchangeAdjScrollbar.SetBarSize( l_ptSize );
	l_ptSize.x = 192;
	m_uiExchangeAdjScrollbar.SetSize( l_ptSize );
	m_uiExchangeAdjScrollbar.SetStyle( CGUIScrollbar::Horizontal );

	m_uiExchangeButton.SetSize( 77, 25 );
	m_uiRecoveryButton.SetSize( 77, 25 );

	m_uiTitle.SetString( "환 전 소" );
	m_uiEcoTitleStr.SetString( "경제지표" );
	m_uiRaceStr[0].SetString( "벨라토" );
	m_uiRaceStr[1].SetString( "코라" );
	m_uiRaceStr[2].SetString( "아크레시아" );
	m_uiEcoGraphHourStr.SetString( "-12D -10D -8D  -6D  -4D  -2D  현재" );

	m_uiTaxRateTitleStr.SetString( "세 율" );
	m_uiExchangeRateTitleStr.SetString( "환 율" );
	m_uiMoneyTitleStr.SetString( "보유금액" );
	m_uiGoldStr.SetString( "골  드" );
	m_uiDalantStr.SetString( _GetAvatarMoneyName() );
	m_uiExchangeAdjStr.SetString( "환전조절" );

	m_uiExchangeButton.SetLabel( "환 전" );
	m_uiRecoveryButton.SetLabel( "복 구" );

	m_uiTaxRateMsg[0].SetString( "세율은 아이템을 거래할 때 부가" );
	m_uiTaxRateMsg[1].SetString( "되는 금액입니다." );

	m_uiExchangeRateMsg[0].SetString( "환율은 1골드 당 환전할 수 있는" );
	char buf[32];
	sprintf( buf, "%s의 금액입니다.", _GetAvatarMoneyName() );
	m_uiExchangeRateMsg[1].SetString( buf );
	
	// --------------------------------------------------------
	// pos
	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 7 );

	m_uiAniTitleBar.SetPos( m_ptPos.x, m_ptPos.y );
	m_uiCloseButton.SetPos( m_ptPos.x + 211, m_ptPos.y + 4 );
	m_uiHelpButton.SetPos( m_ptPos.x + 194, m_ptPos.y + 4 );

	m_uiEcoTitleStr.SetPos( m_ptPos.x + 90, m_ptPos.y + 38 );
	m_uiEcoBase.SetPos( m_ptPos.x + 11, m_ptPos.y + 33 );

	m_uiEcoGraphBase.SetPos( m_ptPos.x + 18, m_ptPos.y + 69 );
	m_uiEcoGraphTitleStr.SetPos( m_ptPos.x + 39, m_ptPos.y + 74 );
	m_uiEcoGraphRaceStr.SetPos( m_ptPos.x + 44, m_ptPos.y + 74 );
	
	
	m_uiEcoGraphMsg[0].SetPos( m_ptPos.x + 18, m_ptPos.y + 96 );
	m_uiEcoGraphMsg[1].SetPos( m_ptPos.x + 18, m_ptPos.y + 110 );

	for( i = 0; i < 3; ++i )
		m_uiEconomyGraph[i].SetPos( m_ptPos.x + 25, m_ptPos.y + 152 );

	m_uiEcoGraphHourStr.SetPos( m_ptPos.x + 12, m_ptPos.y + 224 );

	m_uiRaceColor[0].SetPos( m_ptPos.x + 22, m_ptPos.y + 243 );
	m_uiRaceColor[1].SetPos( m_ptPos.x + 82, m_ptPos.y + 243 );
	m_uiRaceColor[2].SetPos( m_ptPos.x + 132, m_ptPos.y + 243 );

	m_uiRaceStr[0].SetPos( m_ptPos.x + 37, m_ptPos.y + 243 );
	m_uiRaceStr[1].SetPos( m_ptPos.x + 98, m_ptPos.y + 243 );
	m_uiRaceStr[2].SetPos( m_ptPos.x + 147, m_ptPos.y + 243 );

	m_uiEcoRateBase.SetPos( m_ptPos.x + 18, m_ptPos.y + 273 );
	m_uiEcoRateTitleStr.SetPos( m_ptPos.x + 42, m_ptPos.y + 277 );
	m_uiEcoRateRaceStr.SetPos( m_ptPos.x + 48, m_ptPos.y + 277 );

	m_uiTaxRateBase.SetPos( m_ptPos.x + 17, m_ptPos.y + 296 );
	m_uiTaxRateTitleStr.SetPos( m_ptPos.x + 27, m_ptPos.y + 299 );
	m_uiTaxRateStr.SetPos( m_ptPos.x + 117, m_ptPos.y + 300 );
	m_uiTaxRateMsg[0].SetPos( m_ptPos.x + 24, m_ptPos.y + 317 );
	m_uiTaxRateMsg[1].SetPos( m_ptPos.x + 24, m_ptPos.y + 331 );

	m_uiExchangeRateBase.SetPos( m_ptPos.x + 17, m_ptPos.y + 349 );
	m_uiExchangeRateTitleStr.SetPos( m_ptPos.x + 27, m_ptPos.y + 351 );
	m_uiExchangeRateStr.SetPos( m_ptPos.x + 75, m_ptPos.y + 353 );
	m_uiExchangeRateMsg[0].SetPos( m_ptPos.x + 24, m_ptPos.y + 370 );
	m_uiExchangeRateMsg[1].SetPos( m_ptPos.x + 24, m_ptPos.y + 384 );


	m_uiMoneyBase.SetPos( m_ptPos.x + 11, m_ptPos.y + 419 );
	m_uiMoneyTitleStr.SetPos( m_ptPos.x + 91, m_ptPos.y + 423 );
	
	
	m_uiGoldStr.SetPos( m_ptPos.x + 29, m_ptPos.y + 438 );
	m_uiGold.SetPos( m_ptPos.x, m_ptPos.y + 454 );
	m_uiDalantStr.SetPos( m_ptPos.x + 165, m_ptPos.y + 438 );	
	m_uiDalant.SetPos( m_ptPos.x, m_ptPos.y + 454 );	

	m_uiExchangeAdjStr.SetPos( m_ptPos.x + 91, m_ptPos.y + 488 );
	m_uiExchangeAdjScrollbar.SetPos( m_ptPos.x + 18, m_ptPos.y + 505 );

	m_uiExchangeButton.SetPos( m_ptPos.x + 13, m_ptPos.y + 537 );
	m_uiRecoveryButton.SetPos( m_ptPos.x + 139, m_ptPos.y + 537 );	
	

	// --------------------------------------------------------
	// add controller	
	m_uiExchangeButton.AddMouseEventHandler( this );
	m_uiRecoveryButton.AddMouseEventHandler( this );
	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );
	m_uiExchangeAdjScrollbar.AddScrollHandler( this );
	

	Add( &m_uiAniTitleBar );
	Add( &m_uiTitle );
	Add( &m_uiCloseButton );
	Add( &m_uiHelpButton );

	Add( &m_uiEcoBase );
	Add( &m_uiEcoTitleStr );
	
	Add( &m_uiEcoGraphBase );
	Add( &m_uiEcoGraphTitleStr );
	Add( &m_uiEcoGraphRaceStr );
	Add( &m_uiEcoGraphMsg[0] );
	Add( &m_uiEcoGraphMsg[1] );

	for( i = 0; i < 3; ++i )
	{
		Add( &m_uiEconomyGraph[i] );

		Add( &m_uiRaceColor[i] );
		Add( &m_uiRaceStr[i] );
	}
	Add( &m_uiEcoGraphHourStr );

	Add( &m_uiEcoRateBase );
	Add( &m_uiEcoRateTitleStr );
	Add( &m_uiEcoRateRaceStr );

	Add( &m_uiTaxRateBase );
	Add( &m_uiTaxRateTitleStr );
	Add( &m_uiTaxRateStr );
	Add( &m_uiTaxRateMsg[0] );
	Add( &m_uiTaxRateMsg[1] );

	Add( &m_uiExchangeRateBase );
	Add( &m_uiExchangeRateTitleStr );
	Add( &m_uiExchangeRateStr );
	Add( &m_uiExchangeRateMsg[0] );
	Add( &m_uiExchangeRateMsg[1] );


	Add( &m_uiMoneyBase );
	Add( &m_uiMoneyTitleStr );	

	Add( &m_uiDalantStr );
	Add( &m_uiDalant );
	Add( &m_uiGoldStr );
	Add( &m_uiGold );

	Add( &m_uiExchangeAdjStr );
	Add( &m_uiExchangeAdjScrollbar );

	Add( &m_uiExchangeButton );
	Add( &m_uiRecoveryButton );
	
	// --------------------------------------------------------
	SetSprite();

	SetARGB( 0xCCFFFFFF );

	m_uiRaceColor[0].SetARGB( D3DCOLOR_XRGB( 243, 192, 71 ) );
	m_uiRaceColor[1].SetARGB( D3DCOLOR_XRGB( 255, 18, 0 ) );
	m_uiRaceColor[2].SetARGB( D3DCOLOR_XRGB( 54, 54, 247 ) );
	for( i = 0; i < 3; ++i )
	{
		m_uiRaceStr[i].GetFont()->dwColor = D3DCOLOR_XRGB( 227, 224, 217);
		m_uiEconomyGraph[i].SetColor( m_uiRaceColor[i].GetARGB() );
	}

	m_uiTitle.GetFont()->dwColor = D3DCOLOR_XRGB( 195, 193, 190 );

	m_uiEcoTitleStr.GetFont()->dwColor = D3DCOLOR_XRGB( 227, 224, 217);
	m_uiEcoRateTitleStr.GetFont()->dwColor = D3DCOLOR_XRGB( 227, 224, 217);
	m_uiEcoGraphHourStr.GetFont()->dwColor = D3DCOLOR_XRGB( 227, 224, 217);
	m_uiTaxRateTitleStr.GetFont()->dwColor = D3DCOLOR_XRGB( 227, 224, 217);
	m_uiExchangeRateTitleStr.GetFont()->dwColor = D3DCOLOR_XRGB( 227, 224, 217);	
	m_uiMoneyTitleStr.GetFont()->dwColor = D3DCOLOR_XRGB( 227, 224, 217);
	m_uiExchangeAdjStr.GetFont()->dwColor = D3DCOLOR_XRGB( 227, 224, 217);

	m_uiEcoGraphRaceStr.GetFont()->dwColor = D3DCOLOR_XRGB( 182, 182, 255 );
	m_uiEcoRateRaceStr.GetFont()->dwColor = D3DCOLOR_XRGB( 182, 182, 255 );

	m_uiTaxRateStr.GetFont()->dwColor = D3DCOLOR_XRGB( 255, 204, 91 );
	m_uiExchangeRateStr.GetFont()->dwColor = D3DCOLOR_XRGB( 255, 204, 91 );

	for( i = 0; i < 2; ++i )
	{
		m_uiEcoGraphMsg[i].GetFont()->dwColor = D3DCOLOR_XRGB( 196, 182, 150 );
		m_uiTaxRateMsg[i].GetFont()->dwColor = D3DCOLOR_XRGB( 196, 182, 150 );
		m_uiExchangeRateMsg[i].GetFont()->dwColor = D3DCOLOR_XRGB( 196, 182, 150 );
	}	

	m_uiDalantStr.GetFont()->dwColor = D3DCOLOR_XRGB( 196, 167, 145);
	m_uiGoldStr.GetFont()->dwColor = D3DCOLOR_XRGB( 196, 167, 145);
	m_uiDalant.GetFont()->dwColor = D3DCOLOR_XRGB( 229, 220, 201);
	m_uiGold.GetFont()->dwColor = D3DCOLOR_XRGB( 229, 220, 201);	

	m_uiExchangeButton.SetLabelStyle( R3_HAN_OUTLINE );
	m_uiRecoveryButton.SetLabelStyle( R3_HAN_OUTLINE );

	m_uiTaxRateTitleStr.GetFont()->dwStyle = R3_HAN_OUTLINE;
	m_uiExchangeRateTitleStr.GetFont()->dwStyle = R3_HAN_OUTLINE;

	// ---------------------------------------------------------

	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );	

	SetMoveObject( &m_uiAniTitleBar );
	SetMoveObject( &m_uiTitle );
	SetMovable( true );	


	EnableExchange( FALSE ); 
	EnableRecovery( FALSE );


	SetExchangeRate( 1000 );

	// test
	SetRaceType( 0 );
	

	m_bIsCreated = TRUE;
}

void
CGUIExchangeWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_EXCHANGE );

		l_spriteInfo.wActionNo = 0;

		// background
		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );		
		
		l_spriteInfo.wFrameNo = 1;
		m_uiEcoBase.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 2;
		m_uiEcoGraphBase.SetSpriteInfo( &l_spriteInfo );
		m_uiEcoRateBase.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 3;
		m_uiTaxRateBase.SetSpriteInfo( &l_spriteInfo );
		m_uiExchangeRateBase.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 4;
		m_uiMoneyBase.SetSpriteInfo( &l_spriteInfo );


		l_spriteInfo.wActionNo = 1;

		// adjust bar
		SPRITE_INFO l_sScrollbarSprite[2];
		memcpy( &l_sScrollbarSprite[0], &l_spriteInfo, sizeof( SPRITE_INFO ) );
		memcpy( &l_sScrollbarSprite[1], &l_spriteInfo, sizeof( SPRITE_INFO ) );	

		l_sScrollbarSprite[0].wFrameNo = 0;
		m_uiExchangeAdjScrollbar.SetSpriteInfo( &l_sScrollbarSprite[0] );

		l_sScrollbarSprite[0].wFrameNo = 1;
		m_uiExchangeAdjScrollbar.SetBarSpriteInfo( &l_sScrollbarSprite[0] );

		l_sScrollbarSprite[0].wFrameNo = 2;
		l_sScrollbarSprite[1].wFrameNo = 3;
		m_uiExchangeAdjScrollbar.SetTopArrowSpriteInfo( l_sScrollbarSprite );

		l_sScrollbarSprite[0].wFrameNo = 4;
		l_sScrollbarSprite[1].wFrameNo = 5;
		m_uiExchangeAdjScrollbar.SetBottomArrowSpriteInfo( l_sScrollbarSprite );			
		

		// ani title
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );				

		// exchangebutton
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_spriteInfo.wActionNo = 0;		
		l_spriteInfo.wFrameNo = 12;
		m_uiExchangeButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_uiExchangeButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_uiExchangeButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 15;
		m_uiExchangeButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );

		// recoverybutton
		l_spriteInfo.wFrameNo = 12;
		m_uiRecoveryButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_uiRecoveryButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_uiRecoveryButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 15;
		m_uiRecoveryButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );
		

		// help button		
		l_spriteInfo.wActionNo = 5;
		l_spriteInfo.wFrameNo = 0;
		m_uiHelpButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 1;
		m_uiHelpButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiHelpButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );

		
		// close button	
		m_uiCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_uiCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_uiCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_EXCHANGE );	
		if( !l_pSprite )
			return;

		GetSpriteInfo()->pSprite = l_pSprite;		

		m_uiEcoBase.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiEcoGraphBase.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiEcoRateBase.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiTaxRateBase.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiExchangeRateBase.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiMoneyBase.GetSpriteInfo()->pSprite = l_pSprite;

		m_uiExchangeAdjScrollbar.SetSprite( l_pSprite );
	

		// ani title
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;		

		
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );

		// exchange button		
		m_uiExchangeButton.SetSprite( l_pSprite );
		m_uiRecoveryButton.SetSprite( l_pSprite );

		// help button
		m_uiHelpButton.SetSprite( l_pSprite );

		// close button	
		m_uiCloseButton.SetSprite( l_pSprite );
	}
}

void	
CGUIExchangeWindow::SetRaceType( BYTE pi_byRaceType )
{
	BYTE	byRaceType;
	byRaceType = pi_byRaceType / 2;

	if( byRaceType >= 3 )
		return;

	char RACE_TYPE[3][16] = { "벨라토", "코라", "아크레시아" };

	m_uiEcoGraphRaceStr.SetString( RACE_TYPE[byRaceType] );
	m_uiEcoRateRaceStr.SetString( RACE_TYPE[byRaceType] );	

	char buf[32];

	strcpy( buf, "[" );
	int nStrLen = strlen( RACE_TYPE[byRaceType] );
	for( int i = 0; i < nStrLen; ++i )
		strcat( buf, " " );
	strcat( buf, "]경제지표 변동" );
	m_uiEcoGraphTitleStr.SetString( buf );


	strcpy( buf, "[" );	
	for( i = 0; i < nStrLen; ++i )
		strcat( buf, " " );	
	strcat( buf, "]세율 및 환율" );
	m_uiEcoRateTitleStr.SetString( buf );

	// repos
	m_uiEcoGraphTitleStr.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiEcoGraphTitleStr.m_ptSize.x ) / 2, m_uiEcoGraphTitleStr.m_ptPos.y );
	m_uiEcoGraphRaceStr.SetPos( m_uiEcoGraphTitleStr.m_ptPos.x + 7, m_uiEcoGraphTitleStr.m_ptPos.y );

	m_uiEcoRateTitleStr.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiEcoRateTitleStr.m_ptSize.x ) / 2, m_uiEcoRateTitleStr.m_ptPos.y );
	m_uiEcoRateRaceStr.SetPos( m_uiEcoRateTitleStr.m_ptPos.x + 7, m_uiEcoRateTitleStr.m_ptPos.y );

	// 그래프에 자기종족을 맨위에 보이게 한다
	Add( &m_uiEconomyGraph[byRaceType], GetAddedObjectNum() );
}

// ==========================================================================================

void
CGUIExchangeWindow::InitEconomyRatePoint( BYTE pi_byRaceType, BYTE pi_byPointNum, WORD * pi_pEconomyPointList )
{
	if( pi_byRaceType >= 3 )
		return;

	m_uiEconomyGraph[pi_byRaceType].SetPointRange( 0, 300 );
	m_uiEconomyGraph[pi_byRaceType].InitPoint( pi_byPointNum, pi_pEconomyPointList, 5 );
}

void
CGUIExchangeWindow::InsertEconomyRatePoint( BYTE pi_byRaceType, int pi_nEconomyPoint )
{
	if( pi_byRaceType >= 3 )
		return;

	m_uiEconomyGraph[pi_byRaceType].InsertPoint( pi_nEconomyPoint );
}

// ==========================================================================================

void
CGUIExchangeWindow::SetExchangeRate( DWORD pi_dwDalantPerGold )
{
	m_dwDalantPerGold = pi_dwDalantPerGold;

	SetExchangeAdjustbar();

	char buf[32];
	sprintf( buf, "1 골드 당 %d %s", m_dwDalantPerGold, _GetAvatarMoneyName() );
	m_uiExchangeRateStr.SetString( buf );
}

void
CGUIExchangeWindow::SetExchangeCommission( float pi_fExchangeCommission )
{
	m_fExchangeCommission = pi_fExchangeCommission;

	SetExchangeAdjustbar();
}

void	
CGUIExchangeWindow::SetExchangeAdjustbar( void )
{
	if( g_pUiInventWindow )
	{
		if( !m_dwDalantPerGold )
			return;

		// reset adjustbar
		DWORD l_dwDalantToGold = g_pUiInventWindow->GetOwnDalant() / ( m_dwDalantPerGold * ( 1.f + m_fExchangeCommission ) );

		m_uiExchangeAdjScrollbar.SetAmount( l_dwDalantToGold + g_pUiInventWindow->GetOwnGold(), l_dwDalantToGold );
	}
}

// ==========================================================================================

void	
CGUIExchangeWindow::SetTaxRate( float pi_fTaxRate )
{
	char buf[32];
	sprintf( buf, "%.2f%%", pi_fTaxRate );
	m_uiTaxRateStr.SetString( buf );
}

// ==========================================================================================

void
CGUIExchangeWindow::SetGold( DWORD pi_dwGold )
{
	m_dwGold = pi_dwGold;

	char buf[32];
	_GetPriceStr( pi_dwGold, buf );

	m_uiGold.SetString( buf );


	m_uiGold.SetPos( m_ptPos.x + 102 - m_uiGold.m_ptSize.x, m_uiGold.m_ptPos.y );
}

void
CGUIExchangeWindow::SetDalant( DWORD pi_dwDalant )
{
	m_dwDalant = pi_dwDalant;

	char buf[32];
	_GetPriceStr( pi_dwDalant, buf );

	m_uiDalant.SetString( buf );


	m_uiDalant.SetPos( m_ptPos.x + 210 - m_uiDalant.m_ptSize.x , m_uiDalant.m_ptPos.y );
}

// ==========================================================================================

void
CGUIExchangeWindow::EnableExchange( BOOL pi_bEnable )
{
	if( pi_bEnable )
	{	
		m_uiExchangeButton.Enable();
	}
	else
	{
		m_uiExchangeButton.Disable();
	}
}

void
CGUIExchangeWindow::EnableRecovery( BOOL pi_bEnable )
{
	if( pi_bEnable )
	{
		m_uiRecoveryButton.Enable();
	}
	else
	{
		m_uiRecoveryButton.Disable();
	}
}

void
CGUIExchangeWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );	

	if( event.id == MOUSE_CLICKED )
	{	
		if( event.source == &m_uiExchangeButton )
		{
			PostUIMessage( WM_EXCHANGE_BOARD_EXCHANGE_CHECK, 0 );			
		}		
		else if( event.source == &m_uiRecoveryButton )
		{
			PostUIMessage( WM_EXCHANGE_BOARD_CANCEL, 0 );			
		}
		else if( event.source == &m_uiCloseButton )
		{
			Close();
		}
		else if( event.source == &m_uiHelpButton )
		{
			// 도움말 창 열기
			PostUIMessage( WM_OPEN_WINDOW_HELP, 0 );			
		}	
	}
}

void
CGUIExchangeWindow::ScrollEvent( _scroll_event & event )
{
	// set gold, dalant	
	SetGold( m_uiExchangeAdjScrollbar.GetTotalAmount() - m_uiExchangeAdjScrollbar.GetCurrentAmount() );

	// dalant -> gold
	if( m_dwGold > g_pUiInventWindow->GetOwnGold() )
	{
		SetDalant( g_pUiInventWindow->GetOwnDalant() - ( m_dwGold - g_pUiInventWindow->GetOwnGold() ) *
				   m_dwDalantPerGold * ( 1.f + m_fExchangeCommission ) );
	}
	//  gold -> dalant
	else if( m_dwGold < g_pUiInventWindow->GetOwnGold() )
	{
		SetDalant( g_pUiInventWindow->GetOwnDalant() + ( g_pUiInventWindow->GetOwnGold() - m_dwGold ) *
				   m_dwDalantPerGold * ( 1.f - m_fExchangeCommission ) );
	}
	else
	{
		SetDalant( g_pUiInventWindow->GetOwnDalant() );
	}
	

	if( m_dwGold != g_pUiInventWindow->GetOwnGold() )
	{
		EnableExchange( TRUE );
		EnableRecovery( TRUE );
	}
	else
	{
		EnableExchange( FALSE );
		EnableRecovery( FALSE );
	}
}

void
CGUIExchangeWindow::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{
	if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_EXCHANGE )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
		{
			PostUIMessage( WM_EXCHANGE_BOARD_EXCHANGE, 0 );			
		}
		else 
		{
			PostUIMessage( WM_EXCHANGE_BOARD_CANCEL, 0 );
		}
	}
	
	CGUIWindow::ClosingDialogBox( pi_pDlg );	
}

