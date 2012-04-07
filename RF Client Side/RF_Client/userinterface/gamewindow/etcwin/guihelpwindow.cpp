////////////////////////////////////////////////////////////////////////////
//
// CGUIHelpWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIHelpWindow.h"
#include "../../GUIController/GUIScreen.h"
#include "../../SpriteMgr.h"
#include <malloc.h>

#include "../../../DefineMessage.h"

#define	MARGIN_X			20
#define	MARGIN_Y			20
#define OUTLINE_MARGIN		3
#define	COLUMN_GAP			5
#define	SECTION_LINE_NUM	1

CGUIHelpWindow::CGUIHelpWindow()
{
	m_ptStringArea.x		= 0;
	m_ptStringArea.y		= 0;
	
	m_pHelpInfoList			= NULL;
	
	m_bIsCreated			= FALSE;	
}

CGUIHelpWindow::~CGUIHelpWindow()
{
	ClearHelpInfo();	
}

void
CGUIHelpWindow::Init( void )
{	
	m_uiTitle.SetString( "도움말" );

	m_uiTitleBoard.SetSize( 232, 26 );

	m_uiCloseButton.SetSize( 16, 13 );

	POINT l_ptSize;
	l_ptSize.x = 15;	l_ptSize.y = 10;
	m_uiScrollbar.SetTopArrowSize( l_ptSize );
	m_uiScrollbar.SetBottomArrowSize( l_ptSize );	
	l_ptSize.y = 18;
	m_uiScrollbar.SetBarSize( l_ptSize );	
	l_ptSize.y = m_ptSize.y - MARGIN_Y * 2;
	m_uiScrollbar.SetSize( l_ptSize );

	m_uiTitleBoard.SetPos( m_ptPos );
	m_uiTitle.SetPos( m_ptPos.x + (m_uiTitleBoard.m_ptSize.x - m_uiTitle.m_ptSize.x)/2,
					  m_uiTitleBoard.m_ptPos.y + (m_uiTitleBoard.m_ptSize.y - m_uiTitle.m_ptSize.y)/2 );
	m_uiCloseButton.SetPos( m_ptPos.x + 214, m_ptPos.y + 4 );
	
	
	m_uiScrollbar.AddScrollHandler( this );	

	Add( &m_uiTitleBoard );
	Add( &m_uiTitle );
	m_uiCloseButton.AddMouseEventHandler( this );
	Add( &m_uiCloseButton );
	

	SetSprite();			

	m_uiTitle.GetFont()->dwColor = D3DCOLOR_XRGB( 195, 193, 190 );

	SetMoveObject( &m_uiTitleBoard );
	SetMoveObject( &m_uiTitle );
	SetMovable( TRUE );


	SetSize( 232, 300 );


	m_bIsCreated = TRUE;
}

void
CGUIHelpWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		// title board
		l_spriteInfo.wActionNo = 7;		
		l_spriteInfo.wFrameNo = 0;	
		m_uiTitleBoard.SetSpriteInfo( &l_spriteInfo );	

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

		m_uiTitleBoard.GetSpriteInfo()->pSprite = l_pSprite;

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );		

		m_uiScrollbar.SetSprite( l_pSprite );
		m_uiCloseButton.SetSprite( l_pSprite );
	}
}

void
CGUIHelpWindow::SetSize( LONG pi_nWidth, LONG pi_nHeight )
{
	CGUIWindow::SetSize( pi_nWidth, pi_nHeight );	

	m_ptStringArea.x = m_ptSize.x - MARGIN_X * 2;
	m_ptStringArea.y = m_ptSize.y - MARGIN_Y * 2 - m_uiTitleBoard.m_ptSize.y;

	ResetScrollbar();

	if( m_uiScrollbar.GetTotalAmount() > 0 )
	{
		RECT l_rtBoardBound;
		l_rtBoardBound.left = m_ptPos.x + m_ptSize.x - m_uiScrollbar.m_ptSize.x - OUTLINE_MARGIN - 3;
		l_rtBoardBound.right = l_rtBoardBound.left + m_uiScrollbar.m_ptSize.x;
		l_rtBoardBound.top = m_uiTitleBoard.m_ptPos.y + MARGIN_Y;
		l_rtBoardBound.bottom = l_rtBoardBound.top + m_ptStringArea.y;
		m_uiScrollbar.SetBound( l_rtBoardBound );

		Add( &m_uiScrollbar );
	}
	else
	{
		Remove( &m_uiScrollbar );
	}	

	SetViewedString();	
}

void
CGUIHelpWindow::SetSize( POINT & pi_ptSize )
{
	SetSize( pi_ptSize.x, pi_ptSize.y );
}

void
CGUIHelpWindow::SetHelpInfo( HELP_INFO * pi_pHelpInfoList, BYTE pi_byNum )
{
	ClearHelpInfo();

	m_byHelpInfoNum = pi_byNum;
	if( m_byHelpInfoNum == 0 )
		return;

	m_pHelpInfoList = new HELP_INFO[m_byHelpInfoNum];	

	BYTE l_byNewLineNum;
	BYTE l_byLastStrIndex;

	for( int i=0; i<m_byHelpInfoNum; ++i )
	{
		m_pHelpInfoList[i].m_dwWindowID		= pi_pHelpInfoList[i].m_dwWindowID;
		m_pHelpInfoList[i].m_dwSectionID	= pi_pHelpInfoList[i].m_dwSectionID;	
		
	
		if( i < m_byHelpInfoNum - 1 )
		{
			l_byNewLineNum = 0;

			if( m_pHelpInfoList[i].m_dwSectionID != m_pHelpInfoList[i+1].m_dwSectionID )
			{
				l_byNewLineNum = SECTION_LINE_NUM;		
			}

			if( m_pHelpInfoList[i].m_byTotalStrNum > 0 )
			{
				l_byLastStrIndex = pi_pHelpInfoList[i].m_byTotalStrNum-1;	

				if( pi_pHelpInfoList[i].m_pStringInfoList[l_byLastStrIndex].m_pString[pi_pHelpInfoList[i].m_pStringInfoList[l_byLastStrIndex].m_wStrLen-1] != '\n' )
				{
					++l_byNewLineNum;
				}
			}
		}
		else
		{
			// 마지막 줄엔 newline을 추가하지 않는다.
			l_byNewLineNum = 0;
		}
		
		m_pHelpInfoList[i].m_byTotalStrNum	= pi_pHelpInfoList[i].m_byTotalStrNum + l_byNewLineNum;		

		m_pHelpInfoList[i].m_pStringInfoList = new HELP_STRING[m_pHelpInfoList[i].m_byTotalStrNum];
		for( int k=0; k<m_pHelpInfoList[i].m_byTotalStrNum - l_byNewLineNum; ++k )
		{
			memcpy( &m_pHelpInfoList[i].m_pStringInfoList[k].m_sFont,
					&pi_pHelpInfoList[i].m_pStringInfoList[k].m_sFont, sizeof( HELP_FONT ) );
			
			m_pHelpInfoList[i].m_pStringInfoList[k].m_wStrLen = strlen( pi_pHelpInfoList[i].m_pStringInfoList[k].m_pString );
			m_pHelpInfoList[i].m_pStringInfoList[k].m_pString = new char[pi_pHelpInfoList[i].m_pStringInfoList[k].m_wStrLen + 1];
			strcpy( m_pHelpInfoList[i].m_pStringInfoList[k].m_pString, pi_pHelpInfoList[i].m_pStringInfoList[k].m_pString );
		}	
		
		// section 구분을 위해 new line을 넣는다		
		for( ; k<m_pHelpInfoList[i].m_byTotalStrNum; ++k )
		{
			m_pHelpInfoList[i].m_pStringInfoList[k].m_wStrLen = 1;
			m_pHelpInfoList[i].m_pStringInfoList[k].m_pString = new char[3];
			strcpy( m_pHelpInfoList[i].m_pStringInfoList[k].m_pString, "\n" );
		}		
	}

	SetFitSize();

//	ResetScrollbar();
//	SetViewedString();

}

void
CGUIHelpWindow::ClearHelpInfo( void )
{
	if( m_pHelpInfoList )
	{
		for( int i=0; i<m_byHelpInfoNum; ++i )
		{
			for( int k=0; k<m_pHelpInfoList[i].m_byTotalStrNum; ++k )
				delete[] m_pHelpInfoList[i].m_pStringInfoList[k].m_pString;

			delete[] m_pHelpInfoList[i].m_pStringInfoList;			
		}

		delete[] m_pHelpInfoList;

		m_pHelpInfoList = NULL;
	}

	m_byHelpInfoNum = 0;	

	m_dwPrevLinkID = -1L;

	m_uiScrollbar.SetAmount( 0, 0 );

	m_wFirstViewListIndex = 0;
	m_wFirstViewStringIndex = 0;
	m_wFirstViewCharIndex = 0;
}

void
CGUIHelpWindow::SetHelpTooltip( HELP_INFO * pi_pHelpInfo )
{
	ClearHelpTooltip();

	if( !pi_pHelpInfo || pi_pHelpInfo->m_byTotalStrNum == 0 )
	{
		m_ptLinkStringArea.x = 0;
		m_ptLinkStringArea.y = 0;
		return;
	}

	m_sLinkInfo.m_byTotalStrNum = pi_pHelpInfo->m_byTotalStrNum;
	m_sLinkInfo.m_pStringInfoList = new HELP_STRING[m_sLinkInfo.m_byTotalStrNum];	
	for( int i=0; i<m_sLinkInfo.m_byTotalStrNum; ++i )
	{
		m_sLinkInfo.m_pStringInfoList[i].m_wStrLen = pi_pHelpInfo->m_pStringInfoList[i].m_wStrLen;
		m_sLinkInfo.m_pStringInfoList[i].m_pString = new char[m_sLinkInfo.m_pStringInfoList[i].m_wStrLen+1];

		strcpy( m_sLinkInfo.m_pStringInfoList[i].m_pString, pi_pHelpInfo->m_pStringInfoList[i].m_pString );

		memcpy( &m_sLinkInfo.m_pStringInfoList[i].m_sFont, &pi_pHelpInfo->m_pStringInfoList[i].m_sFont, sizeof( HELP_FONT ) );
	}

	// calc link string size	
	int l_nMaxCharNum = 0;
	int l_nTotalLineNum = 0;

	int l_nCurCharIndex = 0;	
	int l_nCurBoardCharNum = 0;

	char * l_pNewLine;
	int	l_nStrLen;

#define MAX_CHAR_NUM	40

	for( i=0; i<m_sLinkInfo.m_byTotalStrNum; )
	{
		// search new line
		l_pNewLine = strstr( m_sLinkInfo.m_pStringInfoList[i].m_pString + l_nCurCharIndex, "\n" );
		if( l_pNewLine )
		{
			l_nStrLen = l_pNewLine - m_sLinkInfo.m_pStringInfoList[i].m_pString - l_nCurCharIndex;
			if( !l_nStrLen ) // 맨앞에 newline이 있다.
			{
				if( l_nCurCharIndex + 1 >= m_sLinkInfo.m_pStringInfoList[i].m_wStrLen )
				{
					l_nCurCharIndex = 0;
					++i;					
				}
				else
				{
					++l_nCurCharIndex;
				}

				// to next line
				l_nCurBoardCharNum = 0;
				++l_nTotalLineNum;
				continue;
			}
			

			if( l_nCurBoardCharNum + l_nStrLen > l_nMaxCharNum )
			{
				if( l_nCurBoardCharNum + l_nStrLen > MAX_CHAR_NUM )
					l_nMaxCharNum = MAX_CHAR_NUM;
				else
					l_nMaxCharNum = l_nCurBoardCharNum + l_nStrLen;
			}
		}
		else
		{
			l_nStrLen = m_sLinkInfo.m_pStringInfoList[i].m_wStrLen - l_nCurCharIndex;			
		}

		if( l_nCurBoardCharNum + l_nStrLen < MAX_CHAR_NUM )
		{
			l_nCurBoardCharNum += l_nStrLen;
			
			// to next string
			l_nCurCharIndex = 0;
			++i;
		}
		else if( l_nCurBoardCharNum >= MAX_CHAR_NUM )
		{
			// to next line
			l_nCurBoardCharNum = 0;
			++l_nTotalLineNum;

			l_nMaxCharNum = MAX_CHAR_NUM;
		}
		else
		{
			int l_nLineEndCharIndex = l_nCurCharIndex + (MAX_CHAR_NUM - l_nCurBoardCharNum - 1);			

			if( m_sLinkInfo.m_pStringInfoList[i].m_pString[l_nLineEndCharIndex] & 0x80 )
			{
				for( int ch=l_nLineEndCharIndex-1; ch >=l_nCurCharIndex; --ch )
				{
					if( !(m_sLinkInfo.m_pStringInfoList[i].m_pString[ch] & 0x80) )
						break;
				}

				if( (l_nLineEndCharIndex - ch) % 2 != 0 )
				{
					--l_nLineEndCharIndex;
				}
			}

			l_nCurCharIndex = l_nLineEndCharIndex + 1;

			// to next line
			l_nCurBoardCharNum = 0;
			++l_nTotalLineNum;

			l_nMaxCharNum = MAX_CHAR_NUM;
		}		
		
	}

	if( l_nCurBoardCharNum > 0 )
	{
		++l_nTotalLineNum;
		if( l_nCurBoardCharNum > l_nMaxCharNum )
			l_nMaxCharNum = l_nCurBoardCharNum;
	}

	m_ptLinkStringArea.x = ONEBYTE_FONTSIZE * l_nMaxCharNum;
	m_ptLinkStringArea.y = ( TWOBYTE_FONTSIZE + COLUMN_GAP ) * l_nTotalLineNum;
}

void
CGUIHelpWindow::ClearHelpTooltip( void )
{	
	if( m_sLinkInfo.m_pStringInfoList )
	{
		for( int i=0; i < m_sLinkInfo.m_byTotalStrNum; ++i )
		{
			delete[] m_sLinkInfo.m_pStringInfoList[i].m_pString;
		}

		delete[] m_sLinkInfo.m_pStringInfoList;

		m_sLinkInfo.m_pStringInfoList = NULL;
	}		

	m_sLinkInfo.m_dwWindowID	= 0;
	m_sLinkInfo.m_byTotalStrNum	= 0;	
}

void
CGUIHelpWindow::ResetScrollbar( void )
{		
	int l_nTotalLineNum = GetTotalLine( m_ptStringArea.x );

	int l_nViewLineNum = ( m_ptStringArea.y ) / ( TWOBYTE_FONTSIZE + COLUMN_GAP );
	if( l_nTotalLineNum > l_nViewLineNum )
		m_uiScrollbar.SetAmount( l_nTotalLineNum - l_nViewLineNum, 0 );
	else
		m_uiScrollbar.SetAmount( 0, 0 );
}

void
CGUIHelpWindow::SetViewedString( void )
{	
	int l_nOneLineTotalCharNum = ( m_ptStringArea.x ) / ONEBYTE_FONTSIZE;
	if( l_nOneLineTotalCharNum == 0 || !m_pHelpInfoList )
		return;

	int l_nSearchLineIndex = m_uiScrollbar.GetCurrentAmount();

	int l_nCurCharIndex = 0;
	int l_nCurLineIndex = 0;
	int l_nCurBoardCharNum = 0;

	char * l_pNewLine;
	int	l_nStrLen;

	HELP_INFO * l_pHelpInfo;

	for( int lt=0; lt<m_byHelpInfoNum; ++lt )
	{
		l_pHelpInfo = &m_pHelpInfoList[lt];		

		for( int i=0; i<l_pHelpInfo->m_byTotalStrNum; )
		{
			if( l_nCurLineIndex == l_nSearchLineIndex )
			{
				m_wFirstViewListIndex	= lt;
				m_wFirstViewStringIndex	= i;
				m_wFirstViewCharIndex	= l_nCurCharIndex;
				return;
			}

			// search new line
			l_pNewLine = strstr( l_pHelpInfo->m_pStringInfoList[i].m_pString + l_nCurCharIndex, "\n" );
			if( l_pNewLine )
			{
				l_nStrLen = l_pNewLine - l_pHelpInfo->m_pStringInfoList[i].m_pString - l_nCurCharIndex;
				if( !l_nStrLen ) // 맨앞에 newline이 있다.
				{
					if( l_nCurCharIndex + 1 >= l_pHelpInfo->m_pStringInfoList[i].m_wStrLen )
					{
						l_nCurCharIndex = 0;
						++i;					
					}
					else
					{
						++l_nCurCharIndex;
					}

					// to next line
					l_nCurBoardCharNum = 0;			
					++l_nCurLineIndex;
					continue;
				}				
			}
			else
			{
				l_nStrLen = l_pHelpInfo->m_pStringInfoList[i].m_wStrLen - l_nCurCharIndex;
			}

			if( l_nCurBoardCharNum + l_nStrLen < l_nOneLineTotalCharNum )
			{
				l_nCurBoardCharNum += l_nStrLen;
				
				// to next string
				l_nCurCharIndex = 0;
				++i;
			}
			else if( l_nCurBoardCharNum >= l_nOneLineTotalCharNum )
			{
				// to next line
				l_nCurBoardCharNum = 0;
				++l_nCurLineIndex;			
			}
			else
			{
				int l_nLineEndCharIndex = l_nCurCharIndex + (l_nOneLineTotalCharNum - l_nCurBoardCharNum - 1);			

				if( l_pHelpInfo->m_pStringInfoList[i].m_pString[l_nLineEndCharIndex] & 0x80 )
				{
					for( int ch=l_nLineEndCharIndex-1; ch >=l_nCurCharIndex; --ch )
					{
						if( !(l_pHelpInfo->m_pStringInfoList[i].m_pString[ch] & 0x80) )
							break;
					}

					if( (l_nLineEndCharIndex - ch) % 2 != 0 )
					{
						--l_nLineEndCharIndex;
					}
				}

				l_nCurCharIndex = l_nLineEndCharIndex + 1;

				// to next line
				l_nCurBoardCharNum = 0;
				++l_nCurLineIndex;
			}
			
		}	

		// 한 section이 끝나면 한줄 띈다.
//		l_nCurLineIndex += SECTION_LINE_NUM;
		
		// init
		l_nCurCharIndex = 0;
		l_nCurBoardCharNum = 0;
	}
}

void
CGUIHelpWindow::SetFitSize( void )
{
	int l_nTotalLineNum = GetTotalLine( m_ptStringArea.x );		

	// x축은 고정
	m_ptSize.y = l_nTotalLineNum * ( TWOBYTE_FONTSIZE + COLUMN_GAP ) +
				 m_uiTitleBoard.m_ptSize.y + MARGIN_Y * 2 + ( OUTLINE_MARGIN + 1 ) * 2;
	
	m_ptStringArea.y = m_ptSize.y - MARGIN_Y * 2 - m_uiTitleBoard.m_ptSize.y;

	m_uiScrollbar.SetAmount( 0, 0 );

	m_wFirstViewListIndex	= 0;
	m_wFirstViewStringIndex	= 0;
	m_wFirstViewCharIndex	= 0;
}

int
CGUIHelpWindow::GetTotalLine( int pi_nWidth )
{
	int l_nOneLineTotalCharNum = pi_nWidth / ONEBYTE_FONTSIZE;
	if( l_nOneLineTotalCharNum == 0 || !m_pHelpInfoList )
	{	
		return 0;
	}

	int l_nCurCharIndex = 0;
	int l_nCurBoardCharNum = 0;

	int l_nTotalLineNum = 0;

	char * l_pNewLine;
	int	l_nStrLen;

	HELP_INFO * l_pHelpInfo;


	for( int lt=0; lt<m_byHelpInfoNum; ++lt )
	{
		l_pHelpInfo = &m_pHelpInfoList[lt];

		l_nCurCharIndex = 0;	
		l_nCurBoardCharNum = 0;


		for( int i=0; i< l_pHelpInfo->m_byTotalStrNum; )
		{
			// search new line
			l_pNewLine = strstr( l_pHelpInfo->m_pStringInfoList[i].m_pString + l_nCurCharIndex, "\n" );
			if( l_pNewLine )
			{
				l_nStrLen = l_pNewLine - l_pHelpInfo->m_pStringInfoList[i].m_pString - l_nCurCharIndex;
				if( !l_nStrLen ) // 맨앞에 newline이 있다.
				{
					if( l_nCurCharIndex + 1 >= l_pHelpInfo->m_pStringInfoList[i].m_wStrLen )
					{
						l_nCurCharIndex = 0;
						++i;					
					}
					else
					{
						++l_nCurCharIndex;
					}

					// to next line
					l_nCurBoardCharNum = 0;			
					++l_nTotalLineNum;
					continue;
				}				
			}
			else
			{
				l_nStrLen = l_pHelpInfo->m_pStringInfoList[i].m_wStrLen - l_nCurCharIndex;
			}

			if( l_nCurBoardCharNum + l_nStrLen < l_nOneLineTotalCharNum )
			{
				l_nCurBoardCharNum += l_nStrLen;
				
				// to next string
				l_nCurCharIndex = 0;
				++i;
			}
			else if( l_nCurBoardCharNum >= l_nOneLineTotalCharNum )
			{
				// to next line
				l_nCurBoardCharNum = 0;
				++l_nTotalLineNum;			
			}
			else
			{
				int l_nLineEndCharIndex = l_nCurCharIndex + (l_nOneLineTotalCharNum - l_nCurBoardCharNum - 1);			

				if( l_pHelpInfo->m_pStringInfoList[i].m_pString[l_nLineEndCharIndex] & 0x80 )
				{
					for( int ch=l_nLineEndCharIndex-1; ch >=l_nCurCharIndex; --ch )
					{
						if( !(l_pHelpInfo->m_pStringInfoList[i].m_pString[ch] & 0x80) )
							break;
					}

					if( (l_nLineEndCharIndex - ch) % 2 != 0 )
					{
						--l_nLineEndCharIndex;
					}
				}

				l_nCurCharIndex = l_nLineEndCharIndex + 1;

				// to next line
				l_nCurBoardCharNum = 0;
				++l_nTotalLineNum;
			}

		}

		if( l_nCurBoardCharNum > 0 )
			++l_nTotalLineNum;		
	}

	return l_nTotalLineNum;
}

BOOL
CGUIHelpWindow::Draw( void )
{
	if( !IsShow() )
		return FALSE;			

	// base
	Draw2DRectangle( m_ptPos.x, m_ptPos.y + 5, m_ptPos.x + m_ptSize.x, m_ptPos.y + m_ptSize.y, 0xCC1C1C1C );

	// left
	Draw2DRectangle( m_ptPos.x + OUTLINE_MARGIN, m_ptPos.y + m_uiTitleBoard.m_ptSize.y + OUTLINE_MARGIN - 5, 
					 m_ptPos.x + OUTLINE_MARGIN + 1, m_ptPos.y + m_ptSize.y - OUTLINE_MARGIN, 0xCCE8E8E8 );
	// right
	Draw2DRectangle( m_ptPos.x + m_ptSize.x - OUTLINE_MARGIN - 1, m_ptPos.y + m_uiTitleBoard.m_ptSize.y + OUTLINE_MARGIN - 5,
					 m_ptPos.x + m_ptSize.x - OUTLINE_MARGIN, m_ptPos.y + m_ptSize.y - OUTLINE_MARGIN, 0xCCE8E8E8 );
	// bottom
	Draw2DRectangle( m_ptPos.x + OUTLINE_MARGIN, m_ptPos.y + m_ptSize.y - OUTLINE_MARGIN - 1,
					 m_ptPos.x + m_ptSize.x - OUTLINE_MARGIN, m_ptPos.y + m_ptSize.y - OUTLINE_MARGIN, 0xCCE8E8E8 );

	CGUIContainer::Draw();

	POINT l_ptDrawPos;
	l_ptDrawPos.x = m_ptPos.x + MARGIN_X;
	l_ptDrawPos.y = m_ptPos.y + m_uiTitleBoard.m_ptSize.y + MARGIN_Y;
	if( m_uiScrollbar.GetTotalAmount() > 0 )
		l_ptDrawPos.x -= 5; // 스크롤바가 있어서 왼쪽으로 조금 당긴다.	
	
	DWORD l_dwLinkID = DrawHelpInfo( m_pHelpInfoList, m_byHelpInfoNum,
								     l_ptDrawPos, m_ptStringArea,
									 m_wFirstViewListIndex, m_wFirstViewStringIndex, m_wFirstViewCharIndex );

	if( l_dwLinkID != -1L )
	{
		// set new helpinfo tooltip
		if( m_dwPrevLinkID != l_dwLinkID )
		{
			// get Help tooltip string

			// SetHelpTooltip();
			PostMessage( gScreen.GetHWND(), WM_OPEN_WINDOW_HELP_TOOLTIP, l_dwLinkID, 0 );
		}

		POINT l_ptDrawArea;
		l_ptDrawArea.x = (OUTLINE_MARGIN + 1 + MARGIN_X) * 2 + m_ptLinkStringArea.x;
		l_ptDrawArea.y = (OUTLINE_MARGIN + 1 + MARGIN_Y) * 2 + m_ptLinkStringArea.y;

		l_ptDrawPos.x = gMouse.x + gScreen.GetMouse()->m_ptSize.x;
		l_ptDrawPos.y = gMouse.y;

		Draw2DRectangle( l_ptDrawPos.x, l_ptDrawPos.y,
						 l_ptDrawPos.x + l_ptDrawArea.x, l_ptDrawPos.y + l_ptDrawArea.y, 0xDC2C2C2C );
		// left
		Draw2DRectangle( l_ptDrawPos.x + OUTLINE_MARGIN, l_ptDrawPos.y + OUTLINE_MARGIN, 
						 l_ptDrawPos.x + OUTLINE_MARGIN + 1, l_ptDrawPos.y + l_ptDrawArea.y - OUTLINE_MARGIN, 0xCCE8E8E8 );
		// right
		Draw2DRectangle( l_ptDrawPos.x + l_ptDrawArea.x - OUTLINE_MARGIN - 1, l_ptDrawPos.y + OUTLINE_MARGIN,
						 l_ptDrawPos.x + l_ptDrawArea.x - OUTLINE_MARGIN, l_ptDrawPos.y + l_ptDrawArea.y - OUTLINE_MARGIN, 0xCCE8E8E8 );
		// top
		Draw2DRectangle( l_ptDrawPos.x + OUTLINE_MARGIN, l_ptDrawPos.y + OUTLINE_MARGIN,
						 l_ptDrawPos.x + l_ptDrawArea.x - OUTLINE_MARGIN, l_ptDrawPos.y + OUTLINE_MARGIN + 1, 0xCCE8E8E8 );
		// bottom
		Draw2DRectangle( l_ptDrawPos.x + OUTLINE_MARGIN, l_ptDrawPos.y + l_ptDrawArea.y - OUTLINE_MARGIN - 1,
						 l_ptDrawPos.x + l_ptDrawArea.x - OUTLINE_MARGIN, gMouse.y + l_ptDrawArea.y - OUTLINE_MARGIN, 0xCCE8E8E8 );

		l_ptDrawPos.x += OUTLINE_MARGIN + 1 + MARGIN_X;
		l_ptDrawPos.y += OUTLINE_MARGIN + 1 + MARGIN_Y;

		DrawHelpInfo( &m_sLinkInfo, 1, l_ptDrawPos, m_ptLinkStringArea, 0, 0, 0 );		
	}

	// set link id
	m_dwPrevLinkID = l_dwLinkID;
	
	DrawDialogBox();
	return TRUE;
}

DWORD
CGUIHelpWindow::DrawHelpInfo( HELP_INFO * pi_pHelpInfoList, BYTE pi_byNum,
							  POINT pi_ptDrawPos, POINT pi_ptDrawArea,
							  WORD pi_wStartListIndex, WORD pi_wStartStringIndex, WORD pi_wStartCharIndex )
{
	int l_nOneLineTotalCharNum = ( pi_ptDrawArea.x ) / ONEBYTE_FONTSIZE;
	int l_nViewedTotalLine = ( pi_ptDrawArea.y ) / ( TWOBYTE_FONTSIZE + COLUMN_GAP );		
	
	int l_nCurBoardCharNum = 0;
	int l_nCurLineIndex = 0;
	int l_nCurCharIndex = pi_wStartCharIndex;
	
	char * l_pNewLine;
	int	l_nStrLen;

	int l_nDrawPosX = pi_ptDrawPos.x;	

	// link backup
	DWORD	l_dwLinkID = -1L;

	HELP_INFO * l_pHelpInfo;

	int i = pi_wStartStringIndex;

	for( int lt=pi_wStartListIndex; lt<pi_byNum; ++lt )
	{
		l_pHelpInfo = &pi_pHelpInfoList[lt];

		for( ; i<l_pHelpInfo->m_byTotalStrNum; )
		{
			if( l_nCurLineIndex >= l_nViewedTotalLine )
				break;

			// search new line
			l_pNewLine = strstr( l_pHelpInfo->m_pStringInfoList[i].m_pString + l_nCurCharIndex, "\n" );
			if( l_pNewLine )
			{
				l_nStrLen = l_pNewLine - l_pHelpInfo->m_pStringInfoList[i].m_pString - l_nCurCharIndex;
				if( !l_nStrLen ) // 맨앞에 newline이 있다.
				{
					if( l_nCurCharIndex + 1 >= l_pHelpInfo->m_pStringInfoList[i].m_wStrLen )
					{
						l_nCurCharIndex = 0;
						++i;					
					}
					else
					{
						++l_nCurCharIndex;
					}

					// to next line
					l_nCurBoardCharNum = 0;			
					++l_nCurLineIndex;
					continue;
				}
			}
			else
			{
				l_nStrLen = l_pHelpInfo->m_pStringInfoList[i].m_wStrLen - l_nCurCharIndex;
			}


			if( l_nCurBoardCharNum + l_nStrLen < l_nOneLineTotalCharNum )
			{
				memcpy( m_pOneLineBuffer, l_pHelpInfo->m_pStringInfoList[i].m_pString + l_nCurCharIndex, l_nStrLen );
				m_pOneLineBuffer[l_nStrLen] = '\0';

				DrawR3Hangul( l_nDrawPosX + l_nCurBoardCharNum * ONEBYTE_FONTSIZE,
							  pi_ptDrawPos.y + l_nCurLineIndex * (TWOBYTE_FONTSIZE+COLUMN_GAP),
							  m_pOneLineBuffer,
							  l_pHelpInfo->m_pStringInfoList[i].m_sFont.m_dwColor,
							  l_pHelpInfo->m_pStringInfoList[i].m_sFont.m_byStyle );

				// check link 
				if( l_pHelpInfo->m_pStringInfoList[i].m_sFont.m_dwLinkID != -1L &&
					gMouse.x >= m_ptPos.x + MARGIN_X + l_nCurBoardCharNum * ONEBYTE_FONTSIZE &&
					gMouse.x <= m_ptPos.x + MARGIN_X + ( l_nCurBoardCharNum + l_nStrLen ) * ONEBYTE_FONTSIZE &&
					gMouse.y >= m_ptPos.y + MARGIN_Y + l_nCurLineIndex * (TWOBYTE_FONTSIZE+COLUMN_GAP) &&
					gMouse.y <= m_ptPos.y + MARGIN_Y + l_nCurLineIndex * (TWOBYTE_FONTSIZE+COLUMN_GAP)+TWOBYTE_FONTSIZE )
				{
					l_dwLinkID = l_pHelpInfo->m_pStringInfoList[i].m_sFont.m_dwLinkID;
				}

				l_nCurBoardCharNum += l_nStrLen;

				
				if( l_nCurCharIndex + l_nStrLen < l_pHelpInfo->m_pStringInfoList[i].m_wStrLen )
				{
					l_nCurCharIndex += l_nStrLen;
				}			
				else
				{
					// to next string
					l_nCurCharIndex = 0;
					++i;
				}			
			}
			else if( l_nCurBoardCharNum >= l_nOneLineTotalCharNum )
			{
				// to next line
				l_nCurBoardCharNum = 0;			
				++l_nCurLineIndex;	
			}
			else
			{
				int l_nLineEndCharIndex = l_nCurCharIndex + (l_nOneLineTotalCharNum - l_nCurBoardCharNum - 1);		

				if( l_pHelpInfo->m_pStringInfoList[i].m_pString[l_nLineEndCharIndex] & 0x80 )
				{
					for( int ch=l_nLineEndCharIndex-1; ch >=l_nCurCharIndex; --ch )
					{
						if( !(l_pHelpInfo->m_pStringInfoList[i].m_pString[ch] & 0x80) )
							break;
					}

					if( (l_nLineEndCharIndex - ch) % 2 != 0 )
					{
						--l_nLineEndCharIndex;
					}
				}

				memcpy( m_pOneLineBuffer, l_pHelpInfo->m_pStringInfoList[i].m_pString + l_nCurCharIndex, l_nLineEndCharIndex - l_nCurCharIndex + 1 );
				m_pOneLineBuffer[l_nLineEndCharIndex - l_nCurCharIndex + 1] = '\0';

				DrawR3Hangul( l_nDrawPosX + l_nCurBoardCharNum * ONEBYTE_FONTSIZE,
							  pi_ptDrawPos.y + l_nCurLineIndex * (TWOBYTE_FONTSIZE+COLUMN_GAP),
							  m_pOneLineBuffer,
							  l_pHelpInfo->m_pStringInfoList[i].m_sFont.m_dwColor,
							  l_pHelpInfo->m_pStringInfoList[i].m_sFont.m_byStyle );

				// check link 
				if( l_pHelpInfo->m_pStringInfoList[i].m_sFont.m_dwLinkID != -1L &&
					gMouse.x >= m_ptPos.x + MARGIN_X + l_nCurBoardCharNum * ONEBYTE_FONTSIZE &&
					gMouse.x <= m_ptPos.x + MARGIN_X + (l_nCurBoardCharNum + l_nLineEndCharIndex - l_nCurCharIndex + 1) * ONEBYTE_FONTSIZE &&
					gMouse.y >= m_ptPos.y + MARGIN_Y + l_nCurLineIndex * (TWOBYTE_FONTSIZE+COLUMN_GAP) &&
					gMouse.y <= m_ptPos.y + MARGIN_Y + l_nCurLineIndex * (TWOBYTE_FONTSIZE+COLUMN_GAP)+TWOBYTE_FONTSIZE )
				{
					l_dwLinkID = l_pHelpInfo->m_pStringInfoList[i].m_sFont.m_dwLinkID;
				}

				l_nCurCharIndex = l_nLineEndCharIndex + 1;

				// to next line
				l_nCurBoardCharNum = 0;
				++l_nCurLineIndex;
			}
		}	

		if( l_nCurBoardCharNum > 0 )
			++l_nCurLineIndex;

		l_nCurBoardCharNum = 0;

		i = 0;

		// 한 section이 끝나면 한줄 띈다.
//		l_nCurLineIndex += SECTION_LINE_NUM;		
	}

	return l_dwLinkID;	
}

void
CGUIHelpWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiCloseButton )		
		{
			Close();
		}		
	}
}

void
CGUIHelpWindow::ScrollEvent( _scroll_event & event )
{
	SetViewedString();
}
