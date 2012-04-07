////////////////////////////////////////////////////////////////////////////
//
// CGUITextBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "GUITextBoard.h"
#include <malloc.h>

////////////////////////////////////////////////////////////////////////////
//
CGUITextBoard::CGUITextBoard()
{
	m_wLimitLineNum		= 2000;
	m_wColumnGap		= 0;
	m_ptMargin.x		= 0;
	m_ptMargin.y		= 0;

	m_bAutoScroll		= FALSE;
	
	m_dwViewedFirstTextIndex = 0;
	m_wViewedFirstCharIndex = 0;

	m_dwTotalLineNum	= 0;
	m_wLastCharNum		= 0;

	m_pOneLineBuffer	= NULL;

	m_pScrollbar		= NULL;
}

CGUITextBoard::~CGUITextBoard()
{
	if( !m_listText.empty() )
	{		
		for( GUITextList::iterator i=m_listText.begin(); i != m_listText.end(); ++i )
		{		
			delete (*i);
		}

		m_listText.clear();
	}

	if( m_pOneLineBuffer )
		delete[] m_pOneLineBuffer;
}

void
CGUITextBoard::SetColumnGap( int pi_nGap )
{
	m_wColumnGap = pi_nGap;

	if( m_pScrollbar )
		m_pScrollbar->SetAdjustmentUnit( TWOBYTE_FONTSIZE + m_wColumnGap );
}

void
CGUITextBoard::AddScrollbar( CGUIScrollbar	* pi_pScrollbar )
{
	m_pScrollbar = pi_pScrollbar;

	if( m_pScrollbar ) 
	{	
		m_pScrollbar->SetAdjustmentUnit( TWOBYTE_FONTSIZE + m_wColumnGap );

		m_pScrollbar->AddScrollHandler( this );
	}
}

void
CGUITextBoard::SetBound( RECT & pi_rtBound )
{
	int l_nResizeUnit = TWOBYTE_FONTSIZE + m_wColumnGap;	

	CGUIObject::SetBound( pi_rtBound );

	if( m_pScrollbar )
	{
		int l_nViewLineNum = m_ptSize.y / l_nResizeUnit;
		if( m_listText.size() > l_nViewLineNum )
		{
			int l_nTotalAmount =  ( m_listText.size() - l_nViewLineNum ) * l_nResizeUnit;
			int l_nCurAmount = m_pScrollbar->GetCurrentAmount();
			if( l_nCurAmount > l_nTotalAmount )
				l_nCurAmount = l_nTotalAmount;
			
			m_pScrollbar->SetAmount( l_nTotalAmount, l_nCurAmount );
		}
		else
		{
			m_pScrollbar->SetAmount( 0, 0 );
		}		
	}

	// reset one line buffer
	WORD l_wOneLineCharNum = ( m_ptSize.x - m_ptMargin.x * 2 ) / ONEBYTE_FONTSIZE;
	if( l_wOneLineCharNum > 0 )
	{
		if( !m_pOneLineBuffer )
		{
			m_pOneLineBuffer = new char[l_wOneLineCharNum + 1];
		}
		else
		{		
			if( _msize( m_pOneLineBuffer ) < l_wOneLineCharNum + 1 )
			{
				delete [] m_pOneLineBuffer;
				m_pOneLineBuffer = new char[l_wOneLineCharNum + 1];
			}
		}
	}
	
	SetViewedText();
}

void
CGUITextBoard::SetSize( LONG pi_nSizeX, LONG pi_nSizeY )
{
	RECT l_rtBound;

	l_rtBound.left = m_ptPos.x;
	l_rtBound.top = m_ptPos.y;
	l_rtBound.right = m_ptPos.x + pi_nSizeX;
	l_rtBound.bottom = m_ptPos.y + pi_nSizeY;

	SetBound( l_rtBound );
}

void
CGUITextBoard::Clear( void )
{
	if( !m_listText.empty() )
	{
		for( GUITextList::iterator i=m_listText.begin(); i != m_listText.end(); ++i )
		{		
			delete (*i);		
		}
		m_listText.clear();		

		// init		
		m_dwViewedFirstTextIndex = 0;
		m_wViewedFirstCharIndex = 0;
		m_dwTotalLineNum = 0;
		m_wLastCharNum = 0;
		if( m_pScrollbar )		
			m_pScrollbar->SetAmount( 0, 0 );		
	}
}

void
CGUITextBoard::InsertText( char *pi_pText )
{
	CGUITextInfo  l_cNewText;

	l_cNewText.SetText( pi_pText );

	// default font setting
	FONT_INFO	l_sFontInfo;
	l_sFontInfo.bySize = 12;
	l_sFontInfo.dwStyle = 0;
	l_sFontInfo.dwColor = 0xffffffff;
	l_cNewText.SetFont( &l_sFontInfo );

	InsertText( &l_cNewText );
}

void
CGUITextBoard::InsertText( char * pi_pText, FONT_INFO * pi_pFontInfo )
{
	CGUITextInfo  l_cNewText;

	l_cNewText.SetText( pi_pText );	
	l_cNewText.SetFont( pi_pFontInfo );

	InsertText( &l_cNewText );
}

void
CGUITextBoard::InsertText( CGUITextInfo * pi_pTextInfo, BOOL pi_bResize/*= FALSE*/ )
{	
	CGUITextInfo * l_pNewText;
	char * l_pText;		
	int l_nCharIndex;		
	
	
	if( pi_bResize || !m_pOneLineBuffer )
	{
		// pi_pTextInfo의 string에 크기를 맞춘다.
		ResizeByText( pi_pTextInfo->GetText() );		
	}	
	
	WORD l_wOneLineCharNum = ( m_ptSize.x - m_ptMargin.x * 2 ) / ONEBYTE_FONTSIZE;
	

	l_pText = pi_pTextInfo->GetText();

	l_nCharIndex = 0;
	m_pOneLineBuffer[0] = '\0';

	for( int i=0; l_pText[i] != '\0'; ++i )
	{
		if( l_pText[i] == '\n' )
		{
			m_pOneLineBuffer[l_nCharIndex] = '\0';

			// insert msg
			l_pNewText = new CGUITextInfo;
			
			l_pNewText->SetText( m_pOneLineBuffer );
			l_pNewText->SetFont( pi_pTextInfo->GetFont() );	

			if( m_listText.size() >= m_wLimitLineNum )
				m_listText.pop_front();

			m_listText.push_back( l_pNewText );
			
			// init
			l_nCharIndex = 0;
			m_pOneLineBuffer[0] = '\0';
		}
		else
		{
			m_pOneLineBuffer[l_nCharIndex] = l_pText[i];

			if( l_nCharIndex + 1 < l_wOneLineCharNum )
			{
				++l_nCharIndex;
				continue;
			}

			m_pOneLineBuffer[l_nCharIndex+1] = '\0';

			if( m_pOneLineBuffer[l_nCharIndex] & 0x80 )
			{
				for( int ch=l_nCharIndex-1; ch>=0; --ch )
				{
					if( !(m_pOneLineBuffer[ch] & 0x80) )					
						break;					
				}

				if( (l_nCharIndex - ch) % 2 != 0 )
				{						
					m_pOneLineBuffer[l_nCharIndex] = '\0';
					--i; // 마지막 문자를 밑줄로 내린다.
				}			
			}			
			
			// 줄마지막에 붙은 newline은 넘어간다
			if( l_pText[i+1] == '\n' )
			{
				++i; // skip new line
			}

			// insert msg			
			l_pNewText = new CGUITextInfo;
			
			l_pNewText->SetText( m_pOneLineBuffer );
			l_pNewText->SetFont( pi_pTextInfo->GetFont() );				

			if( m_listText.size() >= m_wLimitLineNum )
				m_listText.pop_front();

			m_listText.push_back( l_pNewText );
			
			// init
			l_nCharIndex = 0;
			m_pOneLineBuffer[0] = '\0';
			
		}
	}

	// buf에 남아있는 것도 insert한다
	if( l_nCharIndex > 0 )
	{
		m_pOneLineBuffer[l_nCharIndex] = '\0';

		// insert msg
		l_pNewText = new CGUITextInfo;
		
		l_pNewText->SetText( m_pOneLineBuffer );
		l_pNewText->SetFont( pi_pTextInfo->GetFont() );			

		if( m_listText.size() >= m_wLimitLineNum )
			m_listText.pop_front();

		m_listText.push_back( l_pNewText );
	}


	// --------------------------------------------------------------------------

	if( !m_pScrollbar )
		return;

	WORD l_wViewedLine = ( m_ptSize.y - m_ptMargin.y * 2 ) / ( TWOBYTE_FONTSIZE + m_wColumnGap );
	if( m_listText.size() > l_wViewedLine )
	{
		m_pScrollbar->SetTotalAmount( ( m_listText.size() - l_wViewedLine ) * 
									  ( TWOBYTE_FONTSIZE + m_wColumnGap ) );

		if( m_bAutoScroll )
		{
			m_pScrollbar->SetCurrentAmount( m_pScrollbar->GetTotalAmount() );

			SetViewedText();
		}
	}

}

void	
CGUITextBoard::ResizeByText( char * pi_pStr )
{
	WORD	l_wMaxCharNum = 0;
	WORD	l_wCharNum = 0;		
	WORD	l_wLineNum = 0;

	for( int i=0; pi_pStr[i] != '\0'; ++i )
	{		
		if( pi_pStr[i] == '\n' )
		{
			++l_wLineNum;			

			if( l_wCharNum > l_wMaxCharNum )
			{
				l_wMaxCharNum = l_wCharNum;
			}
			
			l_wCharNum = 0;
		}
		else 
		{
			++l_wCharNum;
		}
	}

	if( l_wCharNum > 0 )
	{
		++l_wLineNum;		
		
		if( l_wCharNum > l_wMaxCharNum )
		{
			l_wMaxCharNum = l_wCharNum;
		}
	}

	
	RECT	l_rtBound;		
	
	l_rtBound.left = m_ptPos.x;
	l_rtBound.top = m_ptPos.y;
	l_rtBound.right = l_rtBound.left + l_wMaxCharNum * ONEBYTE_FONTSIZE + m_ptMargin.x * 2;
	l_rtBound.bottom = l_rtBound.top + l_wLineNum * (TWOBYTE_FONTSIZE + m_wColumnGap) + m_ptMargin.y * 2;

	SetBound( l_rtBound );
}

/*
// 한줄에 두개이상 다른 font로 출력할 때 
void
CGUITextBoard::InsertText( CGUITextInfo * pi_pText )
{
	// 제한 줄수를 넘었으면 clear해준다.
//	if( m_listText.size() >= m_wLimitLineNum )
//		Clear();

	CGUITextInfo * l_pNewText = new CGUITextInfo;

	l_pNewText->SetText( pi_pText->GetText() );
	l_pNewText->SetFont( pi_pText->GetFont() );
	
	// insert text in list
	m_listText.push_back( l_pNewText );	

	if( !m_pScrollbar ) 
		return;

	// set total line num, scrollbar amount
	char * l_pText;	
	l_pText = pi_pText->GetText();

	WORD l_wOneLineCharNum = ( m_ptSize.x - m_ptMargin.x * 2 ) / ONEBYTE_FONTSIZE;

	for( int i=0; l_pText[i] != '\0'; ++i )
	{
		if( l_pText[i] == '\n' )
		{
			++m_dwTotalLineNum;
			m_wLastCharNum = 0;
		}
		else
		{
			++m_wLastCharNum;

			if( m_wLastCharNum == l_wOneLineCharNum )
			{
				++m_dwTotalLineNum;

				m_wLastCharNum = 0;

				// 한글이 짤리는지 체크한다.
				if( l_pText[i] & 0x80 )
				{				
					for( int j=i; j>=0; --j )
					{
						if( !( l_pText[j] & 0x80 ) )					
						{
							// 한글이 짤리므로 char를 하나 내려준다.
							if( ( i - j ) % 2 != 0 )
								++m_wLastCharNum;
							break;
						}
					}

					if( j < 0 )
					{
						if( ( i - j + 1 ) % 2 != 0 )
							++m_wLastCharNum;
					}
				}
				else
				{
					// line over됐을 때 줄에 첫 문자가 new line일 때 건너뛴다.
					if( l_pText[i+1] != '\0' &&
						l_pText[i+1] == '\n' )
					{					
						++i; // skip new line
					}
				}
			}
		}		
	}	

	WORD l_wRealTotalLineNum;
	WORD l_wViewedLine;

	if( m_wLastCharNum > 0 )
		l_wRealTotalLineNum = m_dwTotalLineNum + 1;
	else
		l_wRealTotalLineNum = m_dwTotalLineNum;

	l_wViewedLine = ( m_ptSize.y - m_ptMargin.y * 2 ) / ( TWOBYTE_FONTSIZE + m_wColumnGap );
	if( l_wRealTotalLineNum > l_wViewedLine )
	{
		m_pScrollbar->SetTotalAmount( ( l_wRealTotalLineNum - l_wViewedLine ) * 
									  ( TWOBYTE_FONTSIZE + m_wColumnGap ) );

		if( m_bAutoScroll )
		{			
			m_pScrollbar->SetCurrentAmount( m_pScrollbar->GetTotalAmount() );
			// set m_dwViewedFirstTextIndex
			SetViewedText();
		}
	}
}
*/

BOOL
CGUITextBoard::Draw( void )
{
	if( !CGUIContainer::Draw() )
		return FALSE;

	if( m_listText.empty() )
		return TRUE;

	WORD l_wShowLineNum = ( m_ptSize.y - m_ptMargin.y * 2 ) / ( TWOBYTE_FONTSIZE + m_wColumnGap );	

	WORD l_wScrollLineNum;
	if( l_wShowLineNum > m_listText.size() )
		l_wScrollLineNum = l_wShowLineNum - m_listText.size();
	else
		l_wScrollLineNum = 0;


	if( m_bAutoScroll )
	{
		for( int i=m_dwViewedFirstTextIndex; i<m_listText.size(); ++i )
		{
			if( i - m_dwViewedFirstTextIndex + 1 > l_wShowLineNum )
				break;

			DrawR3Hangul( m_ptPos.x + m_ptMargin.x,
						  m_ptPos.y + m_ptMargin.y +						
							( (i - m_dwViewedFirstTextIndex) + l_wScrollLineNum ) * ( TWOBYTE_FONTSIZE+m_wColumnGap ),
						  m_listText[i]->GetText(),
						  m_listText[i]->GetFont()->dwColor,
						  m_listText[i]->GetFont()->dwStyle );
		}	
	}
	else
	{
		for( int i=m_dwViewedFirstTextIndex; i<m_listText.size(); ++i )
		{
			if( i - m_dwViewedFirstTextIndex + 1 > l_wShowLineNum )
				break;

			DrawR3Hangul( m_ptPos.x + m_ptMargin.x,
						  m_ptPos.y + m_ptMargin.y +						
							( i - m_dwViewedFirstTextIndex ) * ( TWOBYTE_FONTSIZE+m_wColumnGap ),
						  m_listText[i]->GetText(),
						  m_listText[i]->GetFont()->dwColor,
						  m_listText[i]->GetFont()->dwStyle );
		}
	}

	return TRUE;
}

/*
// 한줄에 두개이상 다른 font로 출력할 때
BOOL
CGUITextBoard::Draw( void )
{
	if( !CGUIContainer::Draw() )
		return false;

	if( m_listText.empty() )
		return true;

	char * l_pText;	
	LONG	l_nTextXPos;
	WORD	l_wLineCount;

	WORD	l_wShowLineNum;
		
	WORD	l_wCurrentCharIndex;
	WORD	l_wStrLength;	

	l_wShowLineNum = ( m_ptSize.y - m_ptMargin.y * 2 ) / ( TWOBYTE_FONTSIZE + m_wColumnGap );
	if( l_wShowLineNum == 0 )
		return true;

	// init text x, y pos
	l_nTextXPos = m_ptPos.x + m_ptMargin.x;	
	l_wLineCount = 0;
	
	l_wCurrentCharIndex = m_wViewedFirstCharIndex;

	for( DWORD ti=m_dwViewedFirstTextIndex; ti<m_listText.size(); ++ti )
	{
		// get text
		l_pText = m_listText[ti]->GetText();
		l_wStrLength = strlen( l_pText );
		
		while( true )
		{
			// new line이 있는지 체크한다.
			char * l_pNewLine = strstr( l_pText + l_wCurrentCharIndex, "\n" );			
			WORD l_wNewLineIndex;
			
			if( !l_pNewLine )			
			{
				l_wNewLineIndex = l_wStrLength;
			}			
			else
			{				
				l_wNewLineIndex = l_pNewLine - l_pText;				
			}

			WORD l_wDrawStartIndex;
			l_wDrawStartIndex = l_wCurrentCharIndex;
			
			// 한 라인만 끊어서 처리한다.
			while( l_wDrawStartIndex < l_wNewLineIndex )
			{		
				// x축 여백이 모자랄 때 여백 크기만큼만 출력해주고 다시 체크한다.
				if( (l_wNewLineIndex - l_wDrawStartIndex) * ONEBYTE_FONTSIZE > 
					m_ptPos.x + m_ptSize.x - l_nTextXPos - m_ptMargin.x )
				{					
					WORD l_wOneLineCharNum = ( m_ptPos.x + m_ptSize.x - l_nTextXPos - m_ptMargin.x ) / ONEBYTE_FONTSIZE;										

					memcpy( m_pOneLineBuffer, l_pText + l_wDrawStartIndex, l_wOneLineCharNum );
					m_pOneLineBuffer[l_wOneLineCharNum] = '\0';

					BOOL l_bCutChar = false;

					if( m_pOneLineBuffer[l_wOneLineCharNum-1] & 0x80 )
					{
						for( int j=l_wOneLineCharNum-2; j>=0; --j )
						{
							if( !( m_pOneLineBuffer[j] & 0x80 ) )
							{
								if( (l_wOneLineCharNum-1-j)%2 != 0 )
									l_bCutChar = true;
								break;
							}								
						}
						if( j < 0 )
						{							
							if( l_wOneLineCharNum % 2 != 0 )
								l_bCutChar = true;
						}
					}

					if( l_bCutChar )
					{
						m_pOneLineBuffer[l_wOneLineCharNum - 1] = '\0';

						// l_wDrawStartIndex incrase
						l_wDrawStartIndex += l_wOneLineCharNum - 1;
					}
					else
					{
						// l_wDrawStartIndex incrase
						l_wDrawStartIndex += l_wOneLineCharNum;
					}

					if( m_bAutoScroll &&
						m_dwTotalLineNum < l_wShowLineNum )
					{
						DrawR3Hangul( l_nTextXPos,
									  m_ptPos.y + ( l_wLineCount + l_wShowLineNum - m_dwTotalLineNum ) *
												  (TWOBYTE_FONTSIZE+m_wColumnGap) + m_ptMargin.y,
									  m_pOneLineBuffer,
									  m_listText[ti]->GetFont()->dwColor,
									  m_listText[ti]->GetFont()->dwStyle );
					}
					else
					{					
						DrawR3Hangul( l_nTextXPos,
									  m_ptPos.y + l_wLineCount * (TWOBYTE_FONTSIZE+m_wColumnGap) + m_ptMargin.y,
									  m_pOneLineBuffer,
									  m_listText[ti]->GetFont()->dwColor,
									  m_listText[ti]->GetFont()->dwStyle );
					}					

					// to next line
					l_nTextXPos = m_ptPos.x + m_ptMargin.x;
					++l_wLineCount;					
					
					if( l_wLineCount == l_wShowLineNum )
						return true;
				}
				// x축으로 여백이 모자라지 않을 때 남은 text를 출력하고 나온다.
				else
				{
					WORD l_wOneLineCharNum = l_wNewLineIndex - l_wDrawStartIndex;
					
					memcpy( m_pOneLineBuffer, l_pText + l_wDrawStartIndex, l_wOneLineCharNum );
					m_pOneLineBuffer[l_wOneLineCharNum] = '\0';					

					if( m_bAutoScroll &&
						m_dwTotalLineNum < l_wShowLineNum )
					{
						DrawR3Hangul( l_nTextXPos,
									  m_ptPos.y + ( l_wLineCount + l_wShowLineNum - m_dwTotalLineNum ) *
												  (TWOBYTE_FONTSIZE+m_wColumnGap) + m_ptMargin.y,
									  m_pOneLineBuffer,
									  m_listText[ti]->GetFont()->dwColor,
									  m_listText[ti]->GetFont()->dwStyle );
					}
					else
					{					
						DrawR3Hangul( l_nTextXPos,
									  m_ptPos.y + l_wLineCount * (TWOBYTE_FONTSIZE+m_wColumnGap) + m_ptMargin.y,
									  m_pOneLineBuffer,
									  m_listText[ti]->GetFont()->dwColor,
									  m_listText[ti]->GetFont()->dwStyle );
					}					

					// l_nTextXPos set
					l_nTextXPos += l_wOneLineCharNum * ONEBYTE_FONTSIZE;					

					break;
				}
				
			}								
			
			if( l_pNewLine )
			{
				// to next line
				l_nTextXPos = m_ptPos.x + m_ptMargin.x;
				++l_wLineCount;
			}							
			// next char
			l_wCurrentCharIndex = l_wNewLineIndex + 1;
			
			if( l_wLineCount == l_wShowLineNum )
				return true;			
			
			if( l_wCurrentCharIndex + 1 >= l_wStrLength ) // text를 다 출력했을 때
				break;	
		}

		if( l_wLineCount == l_wShowLineNum )
			return true;

		// init
		l_wCurrentCharIndex = 0;
	}	

	return true;
}
*/

BOOL
CGUITextBoard::Draw( RECT & pi_rtClipping )
{
	return CGUITextBoard::Draw();
}

void
CGUITextBoard::ScrollEvent( _scroll_event & event )
{
	SetViewedText();	
}

void
CGUITextBoard::SetViewedText( void )
{
	if( m_pScrollbar )
		m_dwViewedFirstTextIndex = m_pScrollbar->GetCurrentAmount() / ( TWOBYTE_FONTSIZE + m_wColumnGap );
}

/*
// 한줄에 두개이상 다른 font로 출력할 때 
void
CGUITextBoard::SetViewedText( void )
{
	// set m_dwViewedFirstTextIndex by currnet amount
	char * l_pText;	
	WORD	l_wCurLineIndex;
	WORD	l_wCurCharCount;;	
	
	DWORD	l_dwSearchLineIndex;// 시작 라인	
	WORD	l_wOneLineCharNum;	
	WORD	i;
	WORD	l_wStrLen;	
	WORD	l_wPrevTextIndex;	// 이전 라인 backup
	WORD	l_wPrevCharIndex;

	 // get search line
	if( m_pScrollbar )
		l_dwSearchLineIndex = m_pScrollbar->GetCurrentAmount() / ( TWOBYTE_FONTSIZE + m_wColumnGap );
	else
		l_dwSearchLineIndex = 0;
	if( l_dwSearchLineIndex == 0 )
	{
		m_dwViewedFirstTextIndex = 0;
		m_wViewedFirstCharIndex = 0;
		return;
	}

	
	// init 
	l_wOneLineCharNum = ( m_ptSize.x - m_ptMargin.x * 2 ) / ONEBYTE_FONTSIZE;
	if( !l_wOneLineCharNum )
	{
		m_dwViewedFirstTextIndex = 0;
		m_wViewedFirstCharIndex = 0;
		return;
	}
	
	l_wCurLineIndex = 0;
	l_wCurCharCount = 0;	
	l_wPrevTextIndex = 0;
	l_wPrevCharIndex = 0;	

	// search m_dwViewedFirstTextIndex
	for( WORD ti=0; ti<m_listText.size(); ++ti )
	{
		// get text
		l_pText = m_listText[ti]->GetText();	
		l_wStrLen = strlen( l_pText );		

		for( i=0; i < l_wStrLen; ++i )
		{
			if( l_pText[i] == '\n' ) // new line char
			{
				// next line
				++l_wCurLineIndex;								
				// backup previous line
				l_wPrevTextIndex = ti;
				l_wPrevCharIndex = i+1; // skip new line index

				// check search line
				if( l_wCurLineIndex == l_dwSearchLineIndex )
					break;

				// init char count 
				l_wCurCharCount = 0;
			}
			else
			{
				// next char
				++l_wCurCharCount;

				// over one line
				if( l_wCurCharCount == l_wOneLineCharNum ) 
				{					
					// next line
					++l_wCurLineIndex;					

					// 한글이 짤리는지 체크한다.
					BOOL l_bCutChar = false;								
					if( l_pText[i] & 0x80 )
					{
						WORD l_wStartCharIndex;
						if( l_wPrevTextIndex == ti )
							l_wStartCharIndex = l_wPrevCharIndex;
						else
							l_wStartCharIndex = 0;

						for( int j=i; j>=l_wStartCharIndex; --j )
						{						
							if( !( l_pText[j] & 0x80 ) )
							{
								// 한글이 짤리므로 char를 하나 내려준다.
								if( ( i - j ) % 2 != 0 )
									l_bCutChar = true;
								break;							
							}
						}					

						if( j < l_wStartCharIndex )
						{
							if( ( i - l_wStartCharIndex + 1 ) % 2 != 0 )
								l_bCutChar = true;
						}
					}

					// backup prev line
					l_wPrevTextIndex = ti;

					if( l_bCutChar ) 
					{
						l_wPrevCharIndex = i;
					}
					else
					{						
						// line over됐을 때 줄에 첫 문자가 new line일 때 건너뛴다.
						if( i + 1 < l_wStrLen &&
							l_pText[i + 1] == '\n' )
						{
							l_wPrevCharIndex = i + 2;

							++i; // skip new line
						}
						else
						{
							l_wPrevCharIndex = i + 1;
						}
					}

					// check search line
					if( l_wCurLineIndex == l_dwSearchLineIndex )				
						break;
					
					// init char count index
					l_wCurCharCount = 0;					
				}
			}
		}	// end for loop

		if( i < l_wStrLen )
		{
			if( l_wPrevCharIndex > strlen( m_listText[l_wPrevTextIndex]->GetText() ) - 1 ) 
			{
				// next text
				if( l_wPrevTextIndex + 1 < m_listText.size() )
					m_dwViewedFirstTextIndex = l_wPrevTextIndex + 1;

				m_wViewedFirstCharIndex = 0;
			}
			else
			{
				m_dwViewedFirstTextIndex = l_wPrevTextIndex;
				m_wViewedFirstCharIndex = l_wPrevCharIndex;
			}

			return;
		}
		
	}
		
}
*/