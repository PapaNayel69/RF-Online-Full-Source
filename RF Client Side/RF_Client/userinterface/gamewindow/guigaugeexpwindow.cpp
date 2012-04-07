#include "GUIGaugeExpWindow.h"
#include "../GUIController/GUIScreen.h"
#include "../SpriteMgr.h"
#include <malloc.h>

#include "GUICharInfoWindow.h"

////////////////////////////////////////////////////////////////////////////
//
// CGUINumber Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#define	DECIMAL_POINT_SIZE 3

CGUINumber::CGUINumber()
{
	m_pNumberStr	= NULL;

	m_byNumberCnt	= 0;

	m_byDecimalPointPos = 0;

	m_dwNumberColor	= 0xFFFFFFFF;

	m_ptUnitSize.x	= 0;
	m_ptUnitSize.y	= 0;
}

CGUINumber::~CGUINumber()
{
	if( m_pNumberStr )
		delete[] m_pNumberStr;
}

void
CGUINumber::SetNumberSprite( SPRITE_INFO * pi_pSpriteInfo, POINT & pi_ptNumSize )
{
	if( pi_pSpriteInfo == NULL ||
		pi_pSpriteInfo->pSprite == NULL )
		return;

	SetSpriteInfo( pi_pSpriteInfo );

	SetSpriteU( 0.f, 0.1f );
	SetSpriteV( 0.f, 1.f );

	m_fSpriteUVUnit[0] = m_fSpriteUV[0][1] - m_fSpriteUV[0][0];
	m_fSpriteUVUnit[1] = m_fSpriteUV[1][1] - m_fSpriteUV[1][0];

	// set size
	m_ptUnitSize = pi_ptNumSize;	
	SetSize( m_ptSize.x, m_ptUnitSize.y );
}

void
CGUINumber::SetNumber( DWORD pi_nNumber )
{
	char buf[32];
	itoa( pi_nNumber, buf, 10 );
	
	SetNumberStr( buf );		

	// 정수 숫자갯수는 소수점자리와 같다.
	m_byDecimalPointPos = m_byNumberCnt;

	// resize
	SetSize( m_byNumberCnt * m_ptUnitSize.x, m_ptSize.y );
}

void
CGUINumber::SetNumber( double pi_fNumber, int pi_nDigitCntAfterPoint )
{
	char *	l_pBuf;		
	int		l_nDecimalPointPos;
	int		l_nSign;

	l_pBuf = _fcvt( pi_fNumber, pi_nDigitCntAfterPoint, &l_nDecimalPointPos, &l_nSign );	

	// 소숫점이 맨앞에 나오기 때문에 앞에 영을 붙여준다.
	if( l_nDecimalPointPos == 0 )
	{
		char l_pTempBuf[32];
		sprintf( l_pTempBuf, "0%s", l_pBuf );
		
		m_byDecimalPointPos = l_nDecimalPointPos + 1;

		SetNumberStr( l_pTempBuf );
	}
	else
	{
		m_byDecimalPointPos = l_nDecimalPointPos;

		SetNumberStr( l_pBuf );
	}

	// resize
	if( m_byDecimalPointPos < m_byNumberCnt ) // include decimal point
		SetSize( m_byNumberCnt * m_ptUnitSize.x + DECIMAL_POINT_SIZE, m_ptSize.y );
	else
		SetSize( m_byNumberCnt * m_ptUnitSize.x, m_ptSize.y );
}

void
CGUINumber::SetNumberStr( char * pi_pStr )
{
	m_byNumberCnt = strlen( pi_pStr );

	if( !m_pNumberStr )
	{
		m_pNumberStr = new char[m_byNumberCnt + 1];
	}
	else if( _msize( m_pNumberStr ) < m_byNumberCnt + 1 )
	{		
		delete[] m_pNumberStr;
		m_pNumberStr = new char[m_byNumberCnt + 1];
	}

	strcpy( m_pNumberStr, pi_pStr );
}

BOOL
CGUINumber::Draw( void )
{
	if( !IsShow() )
		return FALSE;
	
	SPRITE_INFO * l_pSpriteInfo;
	l_pSpriteInfo = GetSpriteInfo();
	if( !l_pSpriteInfo->pSprite )
		return FALSE;

	if( !m_pNumberStr )
		return FALSE;


	int l_nNum;	
	int l_nNumPosX;	

	l_nNumPosX = m_ptPos.x;

	l_pSpriteInfo->pSprite->SetAction( l_pSpriteInfo->wActionNo );
	l_pSpriteInfo->pSprite->SetFrame( l_pSpriteInfo->wFrameNo );

	// 소수점 위의 정수를 출력한다.
	for( int i=0; i < m_byDecimalPointPos; ++i )
	{
		l_nNum = m_pNumberStr[i] - '0';
			
		m_fSpriteUV[0][0] = l_nNum * m_fSpriteUVUnit[0];
		m_fSpriteUV[0][1] = m_fSpriteUV[0][0] + m_fSpriteUVUnit[0];

		l_pSpriteInfo->pSprite->DrawSpriteUV( l_nNumPosX,
											  m_ptPos.y,
											  m_fSpriteUV,
											  m_ptPos.x + m_ptUnitSize.x * i + m_ptUnitSize.x,
											  m_ptPos.y + m_ptSize.y,
											  GetARGB() );
		l_nNumPosX += m_ptUnitSize.x;
	}

	// 소숫점 아래 수가 있으면 출력한다.
	if( m_byDecimalPointPos < m_byNumberCnt )
	{
		// draw decimal point		
		Draw2DRectangle( l_nNumPosX + 1,
						 m_ptPos.y + m_ptUnitSize.y - 2,
						 l_nNumPosX + 2,
						 m_ptPos.y + m_ptUnitSize.y - 1,
						 m_dwNumberColor );
		
		l_nNumPosX += DECIMAL_POINT_SIZE;

		// draw numbers after decimal point
		for( i=m_byDecimalPointPos; m_pNumberStr[i] != '\0'; ++i )
		{
			l_nNum = m_pNumberStr[i] - '0';
			
			m_fSpriteUV[0][0] = l_nNum * m_fSpriteUVUnit[0];
			m_fSpriteUV[0][1] = m_fSpriteUV[0][0] + m_fSpriteUVUnit[0];

			l_pSpriteInfo->pSprite->DrawSpriteUV( l_nNumPosX,
												  m_ptPos.y,
												  m_fSpriteUV,
												  m_ptPos.x + m_ptUnitSize.x * i + m_ptUnitSize.x,
												  m_ptPos.y + m_ptSize.y,
												  GetARGB() );
			l_nNumPosX += m_ptUnitSize.x;
		}
	}	

	return TRUE;
}

BOOL
CGUINumber::Draw( RECT & pi_rtClippingArea )
{
	if( !IsShow() )
		return FALSE;

	SPRITE_INFO * l_pSpriteInfo;
	l_pSpriteInfo = GetSpriteInfo();
	if( !l_pSpriteInfo->pSprite )
		return FALSE;

	if( !m_pNumberStr )
		return FALSE;

	int l_nNum;		
	int l_ptNumClippingX[2];	

	l_pSpriteInfo->pSprite->SetAction( l_pSpriteInfo->wActionNo );
	l_pSpriteInfo->pSprite->SetFrame( l_pSpriteInfo->wFrameNo );	

	m_fSpriteUV[1][0] = m_fSpriteUVUnit[1] * ( pi_rtClippingArea.top - m_ptPos.y ) / m_ptSize.y;
	m_fSpriteUV[1][1] = m_fSpriteUVUnit[1] * ( pi_rtClippingArea.bottom - m_ptPos.y ) / m_ptSize.y;

	// 소수점 위의 정수를 출력한다.
	for( int i=0; i < m_byDecimalPointPos; ++i )
	{
		l_nNum = m_pNumberStr[i] - '0';
			
		m_fSpriteUV[0][0] = l_nNum * m_fSpriteUVUnit[0];
		m_fSpriteUV[0][1] = m_fSpriteUV[0][0] + m_fSpriteUVUnit[0];

		l_ptNumClippingX[0] = m_ptPos.x + m_ptUnitSize.x * i;
		l_ptNumClippingX[1] = l_ptNumClippingX[0] + m_ptUnitSize.x;
	
		if( pi_rtClippingArea.left > l_ptNumClippingX[0] )
		{
			if( pi_rtClippingArea.left > l_ptNumClippingX[1] )
				continue;

			m_fSpriteUV[0][0] += ( pi_rtClippingArea.left - l_ptNumClippingX[0] ) * m_fSpriteUVUnit[0];
			l_ptNumClippingX[0] = pi_rtClippingArea.left;
		}
		
		if( pi_rtClippingArea.right < l_ptNumClippingX[1] )
		{
			if( pi_rtClippingArea.right < l_ptNumClippingX[0] )
				continue;

			m_fSpriteUV[0][1] -= ( l_ptNumClippingX[0] - pi_rtClippingArea.right ) * m_fSpriteUVUnit[1];
			l_ptNumClippingX[1] = pi_rtClippingArea.right;
		}

		l_pSpriteInfo->pSprite->DrawSpriteUV( l_ptNumClippingX[0],
											  pi_rtClippingArea.top,
											  m_fSpriteUV,
											  l_ptNumClippingX[1],
											  pi_rtClippingArea.bottom,
											  GetARGB() );
	}

	// 소숫점 아래 수가 있으면 출력한다.
	if( m_byDecimalPointPos < m_byNumberCnt )
	{
		l_ptNumClippingX[0] = m_ptPos.x + m_ptUnitSize.x * m_byDecimalPointPos + 1;
		l_ptNumClippingX[1] = l_ptNumClippingX[0] + 1;

		if( pi_rtClippingArea.left < l_ptNumClippingX[0] &&
			pi_rtClippingArea.right > l_ptNumClippingX[1] &&
			pi_rtClippingArea.top < m_ptPos.y + m_ptUnitSize.y - 2 &&
			pi_rtClippingArea.bottom > m_ptPos.y + m_ptUnitSize.y - 1 )
		{
			// draw decimal point			
			Draw2DRectangle( l_ptNumClippingX[0],
							 m_ptPos.y + m_ptUnitSize.y - 2,
							 l_ptNumClippingX[1],
							 m_ptPos.y + m_ptUnitSize.y - 1,
							 m_dwNumberColor );					
		}

		// draw numbers after decimal point
		for( i=m_byDecimalPointPos; m_pNumberStr[i] != '\0'; ++i )
		{
			l_nNum = m_pNumberStr[i] - '0';
			
			m_fSpriteUV[0][0] = l_nNum * m_fSpriteUVUnit[0];
			m_fSpriteUV[0][1] = m_fSpriteUV[0][0] + m_fSpriteUVUnit[0];

			l_ptNumClippingX[0] = m_ptPos.x + m_ptUnitSize.x * i + DECIMAL_POINT_SIZE;
			l_ptNumClippingX[1] = l_ptNumClippingX[0] + m_ptUnitSize.x;
		
			if( pi_rtClippingArea.left > l_ptNumClippingX[0] )
			{
				if( pi_rtClippingArea.left > l_ptNumClippingX[1] )
					continue;

				m_fSpriteUV[0][0] += ( pi_rtClippingArea.left - l_ptNumClippingX[0] ) * m_fSpriteUVUnit[0];
				l_ptNumClippingX[0] = pi_rtClippingArea.left;
			}
			
			if( pi_rtClippingArea.right < l_ptNumClippingX[1] )
			{
				if( pi_rtClippingArea.right < l_ptNumClippingX[0] )
					continue;

				m_fSpriteUV[0][1] -= ( l_ptNumClippingX[0] - pi_rtClippingArea.right ) * m_fSpriteUVUnit[1];
				l_ptNumClippingX[1] = pi_rtClippingArea.right;
			}

			l_pSpriteInfo->pSprite->DrawSpriteUV( l_ptNumClippingX[0],
												  pi_rtClippingArea.top,
												  m_fSpriteUV,
												  l_ptNumClippingX[1],
												  pi_rtClippingArea.bottom,
												  GetARGB() );
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// CGUIUnitGaugeBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIUnitGaugeBoard::CGUIUnitGaugeBoard()
{
	m_bIsCreated = FALSE;
}

CGUIUnitGaugeBoard::~CGUIUnitGaugeBoard()
{

}

#define	TOTAL_GAUGE_LENGTH		130

void
CGUIUnitGaugeBoard::Init( void )
{
	SetSize( 185, 74 );

	m_uiHPGaugeSlash.SetSize( 7, 9 );


	m_uiHPGauge.SetPos( m_ptPos.x + 10, m_ptPos.y + 6 );
	
	m_uiHPGaugeSlash.SetPos( m_uiHPGauge.m_ptPos.x + ( TOTAL_GAUGE_LENGTH - m_uiHPGaugeSlash.m_ptSize.x ) / 2, m_ptPos.y + 7 );
	m_uiCurHPGaugeNum.SetPos( m_uiHPGaugeSlash.m_ptPos.x, m_uiHPGaugeSlash.m_ptPos.y );
	m_uiMaxHPGaugeNum.SetPos( m_uiHPGaugeSlash.m_ptPos.x, m_uiHPGaugeSlash.m_ptPos.y );	

	m_uiUnitName.SetPos( m_ptPos.x + 10, m_ptPos.y + 30 );
	

	Add( &m_uiHPGauge );
	Add( &m_uiCurHPGaugeNum );
	Add( &m_uiMaxHPGaugeNum );
	Add( &m_uiHPGaugeSlash );

	Add( &m_uiUnitName );

	Add( &m_uiWarningLight );


	SetSprite();

	m_uiCurHPGaugeNum.SetNumberColor( 0xFFFFFFC8 );
	m_uiMaxHPGaugeNum.SetNumberColor( 0xFFFFFFC8 );		
}

void
CGUIUnitGaugeBoard::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );			
		
		l_spriteInfo.wActionNo = 0;

		// gauge bar		
		l_spriteInfo.wFrameNo = 2;
		m_uiHPGauge.SetSpriteInfo( &l_spriteInfo );	

		m_uiHPGauge.SetSpriteV( 0.f, 1.f );
		

		// number
		POINT l_ptNumSize;
		l_ptNumSize.x = 7;
		l_ptNumSize.y = 9;
		
		l_spriteInfo.wFrameNo = 6;
		m_uiCurHPGaugeNum.SetNumberSprite( &l_spriteInfo, l_ptNumSize );
		m_uiMaxHPGaugeNum.SetNumberSprite( &l_spriteInfo, l_ptNumSize );

		l_spriteInfo.wFrameNo = 8;
		m_uiHPGaugeSlash.SetSpriteInfo( &l_spriteInfo );		
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		if( !l_pSprite )
			return;			
		
		m_uiHPGauge.GetSpriteInfo()->pSprite = l_pSprite;

		m_uiCurHPGaugeNum.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiMaxHPGaugeNum.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiHPGaugeSlash.GetSpriteInfo()->pSprite = l_pSprite;		
		
		//m_uiWarningLight.GetSpriteInfo()->pSprite = l_pSprite;		
	}
}
	
void
CGUIUnitGaugeBoard::SetUnitName( char * pi_pName )
{
	m_uiUnitName.SetString( pi_pName );
}

void
CGUIUnitGaugeBoard::SetHP( DWORD pi_dwCurHP, DWORD pi_dwMaxHP )
{	
	float l_fRate = (float)pi_dwCurHP / pi_dwMaxHP;
	if( l_fRate > 1.f )
		l_fRate = 1.f;

	m_uiHPGauge.SetSpriteU( 0.f, l_fRate );
	m_uiHPGauge.SetSize( l_fRate * TOTAL_GAUGE_LENGTH, 11 );


	m_uiCurHPGaugeNum.SetNumber( pi_dwCurHP );
	m_uiMaxHPGaugeNum.SetNumber( pi_dwMaxHP );
	
	m_uiCurHPGaugeNum.SetPos( m_uiHPGaugeSlash.m_ptPos.x - m_uiCurHPGaugeNum.m_ptSize.x - 2, m_uiCurHPGaugeNum.m_ptPos.y );
	m_uiMaxHPGaugeNum.SetPos( m_uiHPGaugeSlash.m_ptPos.x + m_uiHPGaugeSlash.m_ptSize.x + 2, m_uiMaxHPGaugeNum.m_ptPos.y );
}


////////////////////////////////////////////////////////////////////////////
//
// CGUIGaugeExpWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIGaugeExpWindow::CGUIGaugeExpWindow()
{
	for( int i=0; i<MAX_GAUGE_NO; ++i )
	{
		m_nMaxGauge[i] = 0;
		m_nCurrentGauge[i] = 0;		
	}	

	m_fTotalExp		= 0;
	m_fCurrentExp	= 0;

	m_pCharInfoWin	= NULL;

	m_pUnitGaugeBoard = NULL;
	
	m_bCreated		= FALSE;
}

CGUIGaugeExpWindow::~CGUIGaugeExpWindow()
{
	if( m_pUnitGaugeBoard )
	{
		delete m_pUnitGaugeBoard;
		m_pUnitGaugeBoard = NULL;
	}
}

void
CGUIGaugeExpWindow::Init()
{
	// -----------------------------------------------------------
	SetSize( 185, 74 );
	m_uiBoard.SetSize( 185, 74 );
	m_uiBoardAni.SetSize( 185, 74 );

	m_uiAvatarGaugeBoard.SetSize( m_ptSize );
	// gauge bar
	m_uiGauge[LIFE_BAR].SetSize( 130, 11 );
	m_uiGauge[FORCE_BAR].SetSize( 130, 11 );
	m_uiGauge[STAMINA_BAR].SetSize( 130, 11 );

	for( int i=0; i<MAX_GAUGE_NO; ++i )
	{
		m_uiGaugeBoard[i].SetSize( m_uiGauge[i].m_ptSize );
	
		m_uiGaugeSlash[i].SetSize( 7, 9 );
	}
	
	
	// exp
	m_uiExp.SetSize( 132, 5 );
	m_uiExpBoard.SetSize( m_uiExp.m_ptSize );	
	

	// -----------------------------------------------------------
	// gauge bar
	m_uiBoard.SetPos( m_ptPos.x, m_ptPos.y );
	
	m_uiGauge[LIFE_BAR].SetPos( m_ptPos.x + 10, m_ptPos.y + 6 );
	m_uiGauge[FORCE_BAR].SetPos( m_ptPos.x + 10, m_ptPos.y + 22 );
	m_uiGauge[STAMINA_BAR].SetPos( m_ptPos.x + 10, m_ptPos.y + 38 );

	for( i=0; i<MAX_GAUGE_NO; ++i )
	{
		m_uiGaugeBoard[i].SetPos( m_uiGauge[i].m_ptPos );

		m_uiCurGaugeNum[i].SetPos( 0, m_uiGaugeBoard[i].m_ptPos.y + 1 );
		m_uiMaxGaugeNum[i].SetPos( 0, m_uiGaugeBoard[i].m_ptPos.y + 1 );
		m_uiGaugeSlash[i].SetPos( 0, m_uiGaugeBoard[i].m_ptPos.y + 1 );
	}

	// Exp
	m_uiExp.SetPos( m_ptPos.x + 9, m_ptPos.y + 60 );
	m_uiExpBoard.SetPos( m_uiExp.m_ptPos );

	m_uiExpProb.SetPos( m_ptPos.x + 145, m_ptPos.y + 56 );
	


	// -----------------------------------------------------------
	
	Add( &m_uiBoardAni );
	Add( &m_uiBoard );
	

	for( i=0; i<MAX_GAUGE_NO; ++i )
	{
		m_uiAvatarGaugeBoard.Add( &m_uiGaugeBoard[i] );	
		
		m_uiAvatarGaugeBoard.Add( &m_uiGauge[i] );
	
		m_uiAvatarGaugeBoard.Add( &m_uiCurGaugeNum[i] );
		m_uiAvatarGaugeBoard.Add( &m_uiMaxGaugeNum[i] );
		m_uiAvatarGaugeBoard.Add( &m_uiGaugeSlash[i] );
	}
	
	m_uiAvatarGaugeBoard.Add( &m_uiExpBoard );
	m_uiAvatarGaugeBoard.Add( &m_uiExp );

	m_uiAvatarGaugeBoard.Add( &m_uiExpProb );

	m_uiAvatarGaugeBoard.Add( &m_uiLevel );		

	Add( &m_uiAvatarGaugeBoard );

	// -----------------------------------------------------------
	SetSprite();			

	SetARGB( 0xCCFFFFFF );

	for( i=0; i<MAX_GAUGE_NO; ++i )
	{
		m_uiCurGaugeNum[i].SetNumberColor( 0xFFFFFFC8 );
		m_uiMaxGaugeNum[i].SetNumberColor( 0xFFFFFFC8 );	
	}

	m_uiLevel.SetNumberColor( 0xFFFFFFC8 );


	// ------------------------------------------------------------

	m_uiBoardAni.SetTotalFrameNum( 16 );
	m_uiBoardAni.SetObjectNumInLine( 2 );
	m_uiBoardAni.SetFPS( 10 );
	m_uiBoardAni.SetStopInterval( 3500 );


	SetGauge( LIFE_BAR, 0, 0 );
	SetGauge( FORCE_BAR, 0, 0 );

	SetLevel( 0 );
	

	SetName( "GaugeExpWin" );

	m_bCreated = TRUE;	
}

void
CGUIGaugeExpWindow::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );

		// background
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 0;	
		SetSpriteInfo( &l_spriteInfo );	

		l_spriteInfo.wFrameNo = 1;
		m_uiBoard.SetSpriteInfo( &l_spriteInfo );	

		l_spriteInfo.wFrameNo = 9;
		m_uiBoardAni.SetSpriteInfo( &l_spriteInfo );	

		
		// gauge bar
		l_spriteInfo.wFrameNo = 2;
		m_uiGauge[LIFE_BAR].SetSpriteInfo( &l_spriteInfo );		
		l_spriteInfo.wFrameNo = 3;
		m_uiGauge[FORCE_BAR].SetSpriteInfo( &l_spriteInfo );	
		l_spriteInfo.wFrameNo = 4;
		m_uiGauge[STAMINA_BAR].SetSpriteInfo( &l_spriteInfo );				
		

		POINT l_ptNumSize;
		l_ptNumSize.x = 7;
		l_ptNumSize.y = 9;
		for( int i=0; i<MAX_GAUGE_NO; ++i )
		{
			// number
			l_spriteInfo.wFrameNo = 6;
			m_uiCurGaugeNum[i].SetNumberSprite( &l_spriteInfo, l_ptNumSize );
			m_uiMaxGaugeNum[i].SetNumberSprite( &l_spriteInfo, l_ptNumSize );

			l_spriteInfo.wFrameNo = 8;
			m_uiGaugeSlash[i].SetSpriteInfo( &l_spriteInfo );
		}
	
		
		// Exp
		l_spriteInfo.wFrameNo = 5;
		m_uiExp.SetSpriteInfo( &l_spriteInfo );	
		

		// Level
		l_spriteInfo.wFrameNo = 7;
		l_ptNumSize.x = 8;
		l_ptNumSize.y = 11;
		m_uiLevel.SetNumberSprite( &l_spriteInfo, l_ptNumSize );	
		

		m_uiGauge[LIFE_BAR].SetSpriteV( 0.f, 1.f );
		m_uiGauge[FORCE_BAR].SetSpriteV( 0.f, 1.f );
		m_uiGauge[STAMINA_BAR].SetSpriteV( 0.f, 1.f );
		m_uiExp.SetSpriteV( 0.f, 1.f );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		if( !l_pSprite )
			return;
		
		// background
		GetSpriteInfo()->pSprite = l_pSprite;		
		
		m_uiBoard.GetSpriteInfo()->pSprite = l_pSprite;

		m_uiBoardAni.GetSpriteInfo()->pSprite = l_pSprite;
		
		// gauge bar	
		for( int i=0; i<MAX_GAUGE_NO; ++i )
		{
			m_uiGauge[i].GetSpriteInfo()->pSprite = l_pSprite;	
			
			m_uiCurGaugeNum[i].GetSpriteInfo()->pSprite = l_pSprite;
			m_uiMaxGaugeNum[i].GetSpriteInfo()->pSprite = l_pSprite;

			m_uiGaugeSlash[i].GetSpriteInfo()->pSprite = l_pSprite;
		}					
		
		// Exp		
		m_uiExp.GetSpriteInfo()->pSprite = l_pSprite;

		// Level
		m_uiLevel.GetSpriteInfo()->pSprite = l_pSprite;		


		if( m_pUnitGaugeBoard )
			m_pUnitGaugeBoard->SetSprite();
	}
}

void
CGUIGaugeExpWindow::SetGauge( Bar pi_eBar, DWORD pi_dwMaxGauge, DWORD pi_dwCurGauge )
{
	if( pi_dwMaxGauge < 0 )		
		m_nMaxGauge[pi_eBar] = 0;
	else 
		m_nMaxGauge[pi_eBar] = pi_dwMaxGauge;

	SetGauge( pi_eBar, pi_dwCurGauge );	
}

void
CGUIGaugeExpWindow::SetGauge( Bar pi_eBar, DWORD pi_dwCurGauge )
{
	// set current gauge
	if( pi_dwCurGauge < 0 )
		m_nCurrentGauge[pi_eBar] = 0;
	else if( pi_dwCurGauge > m_nMaxGauge[pi_eBar] )
		m_nCurrentGauge[pi_eBar] = m_nMaxGauge[pi_eBar];
	else
		m_nCurrentGauge[pi_eBar] = pi_dwCurGauge;


	float l_fGaugeRate = (float)m_nCurrentGauge[pi_eBar]/m_nMaxGauge[pi_eBar];


	// set size
	if( m_nMaxGauge[pi_eBar] == 0 )
	{
		m_uiGauge[pi_eBar].SetSize( 0, m_uiGauge[pi_eBar].m_ptSize.y );
	}
	else
	{
		m_uiGauge[pi_eBar].SetSize( l_fGaugeRate * m_uiGaugeBoard[pi_eBar].m_ptSize.x,
								m_uiGauge[pi_eBar].m_ptSize.y );
	}


	// sprite setting
	m_uiGauge[pi_eBar].SetSpriteU( 0.f, l_fGaugeRate );	
	

	// gauge number ui setting	
	m_uiCurGaugeNum[pi_eBar].SetNumber( m_nCurrentGauge[pi_eBar] );
	m_uiMaxGaugeNum[pi_eBar].SetNumber( m_nMaxGauge[pi_eBar] );

	// repos
	int l_nPosX = m_uiGaugeBoard[pi_eBar].m_ptPos.x +
				  ( m_uiGaugeBoard[pi_eBar].m_ptSize.x - m_uiGaugeSlash[pi_eBar].m_ptSize.x - 4 ) / 2 - m_uiCurGaugeNum[pi_eBar].m_ptSize.x;
				  
	m_uiCurGaugeNum[pi_eBar].SetPos( l_nPosX, m_uiCurGaugeNum[pi_eBar].m_ptPos.y );
	m_uiGaugeSlash[pi_eBar].SetPos( m_uiCurGaugeNum[pi_eBar].m_ptPos.x + m_uiCurGaugeNum[pi_eBar].m_ptSize.x + 2,
									m_uiGaugeSlash[pi_eBar].m_ptPos.y );
	m_uiMaxGaugeNum[pi_eBar].SetPos( m_uiGaugeSlash[pi_eBar].m_ptPos.x + m_uiGaugeSlash[pi_eBar].m_ptSize.x + 2,
									m_uiMaxGaugeNum[pi_eBar].m_ptPos.y );
	

	if( m_pCharInfoWin )
	{
		if( pi_eBar == LIFE_BAR )
			m_pCharInfoWin->SetHP( m_nCurrentGauge[LIFE_BAR], m_nMaxGauge[LIFE_BAR] );
		else if( pi_eBar == FORCE_BAR )
			m_pCharInfoWin->SetFP( m_nCurrentGauge[FORCE_BAR], m_nMaxGauge[FORCE_BAR] );
		else
			m_pCharInfoWin->SetSP( m_nCurrentGauge[STAMINA_BAR], m_nMaxGauge[STAMINA_BAR] );		
	}
}

void
CGUIGaugeExpWindow::SetTotalExp( double pi_fTotalExp )
{
	m_fTotalExp = pi_fTotalExp;	

	SetCurrentExp( m_fCurrentExp );
}

void
CGUIGaugeExpWindow::SetCurrentExp( double pi_fCurrentExp )
{
	m_fCurrentExp = pi_fCurrentExp;

	float l_fExpRate;

	if( m_fTotalExp == 0 )
		l_fExpRate = 0.f;
	else
		l_fExpRate = m_fCurrentExp / m_fTotalExp;

	if( l_fExpRate >= 1 )
		l_fExpRate = 0.9999f;


	// set exp ui size
	m_uiExp.SetSize( l_fExpRate * m_uiExpBoard.m_ptSize.x, m_uiExp.m_ptSize.y );


	// sprite setting
	m_uiExp.SetSpriteU( 0.f, l_fExpRate );
	
	
	// set exp prob	
	char buf[8];
	sprintf( buf, "%.2f%%", l_fExpRate * 100 );
	m_uiExpProb.SetString( buf );

	m_uiExpProb.SetPos( m_ptPos.x + m_ptSize.x - m_uiExpProb.m_ptSize.x - 3, m_uiExpProb.m_ptPos.y );

	// update char info win
	if( m_pCharInfoWin )
		m_pCharInfoWin->SetExp( l_fExpRate * 100 );
}

void
CGUIGaugeExpWindow::SetLevel( WORD pi_wLevel )
{
	m_wLevel = pi_wLevel;			

	m_uiLevel.SetNumber( m_wLevel );

	if( m_wLevel < 10 )
		m_uiLevel.SetPos( m_ptPos.x + 159, m_ptPos.y + 22 );
	else
		m_uiLevel.SetPos( m_ptPos.x + 155, m_ptPos.y + 22 );


	if( m_pCharInfoWin )
		m_pCharInfoWin->SetLevel( m_wLevel );
}

void
CGUIGaugeExpWindow::SetUnitGaugeBoard( void )
{
	if( !m_pUnitGaugeBoard )
	{
		m_pUnitGaugeBoard = new CGUIUnitGaugeBoard;
		m_pUnitGaugeBoard->Init();
	}

	Remove( &m_uiAvatarGaugeBoard );

	m_pUnitGaugeBoard->SetPos( m_ptPos );
	m_pUnitGaugeBoard->Show( IsShow() );
	Add( m_pUnitGaugeBoard, 2 );
}

void
CGUIGaugeExpWindow::SetAvatarGaugeBoard( void )
{
	if( m_pUnitGaugeBoard )
		Remove( m_pUnitGaugeBoard );

	m_uiAvatarGaugeBoard.SetPos( m_ptPos );
	m_uiAvatarGaugeBoard.Show( IsShow() );
	Add( &m_uiAvatarGaugeBoard, 2 );
}

void
CGUIGaugeExpWindow::SetUnitName( char * pi_pName )
{
	if( m_pUnitGaugeBoard )
		m_pUnitGaugeBoard->SetUnitName( pi_pName );
}

void
CGUIGaugeExpWindow::SetUnitHP( DWORD pi_dwCurHP, DWORD pi_dwMaxHP )
{
	if( m_pUnitGaugeBoard )
		m_pUnitGaugeBoard->SetHP( pi_dwCurHP, pi_dwMaxHP );
}