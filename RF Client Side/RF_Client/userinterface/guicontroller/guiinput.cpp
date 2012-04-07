////////////////////////////////////////////////////////////////////////////
//
// CGUIInput Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "GUIInput.h"

BOOL EP_ProcessFiltering_KeydownMsg( LPARAM pi_lParam );

////////////////////////////////////////////////////////////////////////////
//
CGUIInput::CGUIInput()
{
	m_eAlignment = Left_Alignment;
	m_dwTextColor = 0xffffffff;
	m_bIsSecretText = false;
	m_eInputType = Free;

	m_pInputString[0] = '\0';

	m_wInputLenLimit = MAX_INPUT_STRING;

	m_wShowBeginCharIndex = 0;

	m_ptMargin.x = 0;
	m_ptMargin.y = 0;

	m_nStrPosX = 0;	

	m_nCaretPosX = 0;		
	m_dwCaretColor = D3DCOLOR_XRGB( 95, 78, 252 );
}

CGUIInput::~CGUIInput()
{
}

void
CGUIInput::SetAlignment( Alignment pi_eAlignment )
{ 
	m_eAlignment = pi_eAlignment;	

	// set input pos	
	ResetInputPos();
}

void
CGUIInput::SetShowText( BOOL pi_bShowText )
{
	m_bIsSecretText = !pi_bShowText;

	if( m_bIsSecretText )
	{
		if( IsFocused() &&
			gInputMgr.IsKoreanMode() )
			gInputMgr.SetEnglish();				
	}
}

void
CGUIInput::SetInputLimit( WORD pi_wLimit )
{
	if( pi_wLimit > MAX_INPUT_STRING )
		m_wInputLenLimit = MAX_INPUT_STRING;
	else
		m_wInputLenLimit = pi_wLimit;
}

void
CGUIInput::SetMargin( LONG pi_nPosX, LONG pi_nPosY )
{
	m_ptMargin.x = pi_nPosX;
	m_ptMargin.y = pi_nPosY;
}

BOOL
CGUIInput::IsFocused( void )
{
	if( CGUIObject::GetFocusedObject() == this &&
		IsActive() && 
		gInputMgr.GetInputObject() == this )
		return true;

	return false;
}

void
CGUIInput::SetFocus( void )
{
	if( IsFocused() )
		return;	

	CGUIObject::SetFocus();

	// set input object
	gInputMgr.SetInputObject( this );
	
	// set inputstring
	gInputMgr.SetText( m_pInputString );	

	// ready input
	ResetInputPos();

	gInputMgr.SetCaretColor( m_dwCaretColor );
	gInputMgr.SetTextColor( m_dwTextColor );


	// secret char는 영문자로만 입력할수 있다.
	if( m_bIsSecretText )
	{
		if( gInputMgr.IsKoreanMode() )
			gInputMgr.SetEnglish();
	}
}

void
CGUIInput::SetPos( POINT & pi_ptNewPos )
{
	CGUIObject::SetPos( pi_ptNewPos );

	if( IsFocused() )
		ResetInputPos();	
}

void
CGUIInput::SetPos( LONG pi_nPosX, LONG pi_nPosY )
{
	CGUIObject::SetPos( pi_nPosX, pi_nPosY );
	
	if( IsFocused() )
		ResetInputPos();
}

void
CGUIInput::ResetInputPos( void )
{
	// object의 위치나 string이 바뀌었을 때,
	// object가 focus 되었을 때 ime input position을 다시 세팅한다.
	POINT l_ptInputPos;

	WORD l_wCaretSize;
	if( IsFocused() )
	{
		if( gInputMgr.m_cIME.GetCompLength() )
			l_wCaretSize = TWOBYTE_FONTSIZE;
		else
			l_wCaretSize = 2;
	}
	else
	{
		l_wCaretSize = 0;
	}
	

	WORD l_wShowLen;
	WORD l_wTotalLen;	
	l_wShowLen = ( m_ptSize.x - m_ptMargin.x * 2 - l_wCaretSize ) / ONEBYTE_FONTSIZE;
	l_wTotalLen = strlen( m_pInputString );
	
	
	if( m_eAlignment == Left_Alignment )
	{			
		l_ptInputPos.x = m_ptPos.x + m_ptMargin.x - m_wShowBeginCharIndex * ONEBYTE_FONTSIZE;
		l_ptInputPos.y = m_ptPos.y + m_ptMargin.y;

		// set string pos		
		m_nStrPosX = l_ptInputPos.x + m_wShowBeginCharIndex * ONEBYTE_FONTSIZE - m_ptPos.x;
	}
	else
	{
		if( l_wTotalLen > 0 )
		{
			l_ptInputPos.x = m_ptPos.x + m_ptSize.x - m_ptMargin.x - 
							 (m_wShowEndCharIndex+1) * ONEBYTE_FONTSIZE - l_wCaretSize;

			m_nStrPosX = m_ptSize.x - m_ptMargin.x - 
						 (m_wShowEndCharIndex-m_wShowBeginCharIndex+1) * ONEBYTE_FONTSIZE;
		}
		else
		{
			l_ptInputPos.x = m_ptPos.x + m_ptSize.x - m_ptMargin.x - l_wCaretSize;
			m_nStrPosX = m_ptSize.x - m_ptMargin.x;
		}

		l_ptInputPos.y = m_ptPos.y + m_ptMargin.y;		
	}

	// set ime pos
	if( l_wCaretSize > 0 ) // if focused		
	{
		// 입력 위치를 reset할 때마다 현재 객체의 caret위치도 reset한다.
		gInputMgr.m_cIME.SetIMEPosition( l_ptInputPos.x, l_ptInputPos.y, m_nCaretPosX );
	}
}

void
CGUIInput::SetShowCharIndex( void )
{
	// 문자를 입력할 때마다 m_wShowBeginCharIndex를 update한다.	

	if( m_pInputString[0] == '\0' )
	{
		m_wShowBeginCharIndex = 0;
		m_nCaretPosX = 0;
		ResetInputPos();
		return;
	}	

	WORD	l_wCaretSize;
	if( IsFocused() )
	{
		if( gInputMgr.m_cIME.GetCompLength() )
			l_wCaretSize = TWOBYTE_FONTSIZE;
		else
			l_wCaretSize = 2;

		// update caret position
		m_nCaretPosX = gInputMgr.m_cIME.GetCaretPositionX();
	}
	else
	{
		l_wCaretSize = 0;
	}
	
	WORD	l_wShowCharLen = ( m_ptSize.x - m_ptMargin.x * 2 - l_wCaretSize ) / ONEBYTE_FONTSIZE;
	WORD	l_wTotalCharLen = strlen( m_pInputString );

	// set m_wShowBeginCharIndex
	if( l_wTotalCharLen > l_wShowCharLen )
	{
		// caret의 위치를 기준으로 처음 보이는 문자를 정한다.
		WORD l_wCaretIndex = m_nCaretPosX / ONEBYTE_FONTSIZE;

		// 1.처음 보이는 문자 설정
		if( l_wCaretIndex < m_wShowBeginCharIndex )
			m_wShowBeginCharIndex = l_wCaretIndex;		
		else if( l_wCaretIndex > m_wShowBeginCharIndex + l_wShowCharLen )			
			m_wShowBeginCharIndex = l_wCaretIndex - l_wShowCharLen;
		
		// 처음 보이는 문자가 한글이면 짤리는지 체크한다.
		// 한글조합이 뒤바이트가 영문자인경우도 있기 때문에 
		// 첫번째가 아니면 다 검사한다.
		if( m_wShowBeginCharIndex > 0 )
		{		
			for( int i=m_wShowBeginCharIndex-1; i>=0; --i )
			{
				if( !(m_pInputString[i] & 0x80) )
					break;
			}

			// 한글
			if( ( m_wShowBeginCharIndex - i )%2 == 0 )
				++m_wShowBeginCharIndex;
		}


		// 2.끝에 보이는 문자 설정
		m_wShowEndCharIndex = m_wShowBeginCharIndex + l_wShowCharLen - 1;

		if( m_wShowEndCharIndex >= l_wTotalCharLen - 1 )
		{			
			m_wShowEndCharIndex = l_wTotalCharLen - 1;
		}
		else if( m_pInputString[m_wShowEndCharIndex] & 0x80 )
		{
			//한글이면 짤리는지 체크한다.
			for( int i=m_wShowEndCharIndex+1; i<l_wTotalCharLen; ++i )
			{
				if( !(m_pInputString[i] & 0x80) )				
					break;
			}

			// 한글이 짤리면 showchar를 하나 뺀다
			if( ( i - m_wShowEndCharIndex )%2 == 0 )
				--m_wShowEndCharIndex;
		}

	}	
	else
	{
		m_wShowBeginCharIndex	= 0;
		m_wShowEndCharIndex		= l_wTotalCharLen - 1;
	}
	
	ResetInputPos();
}

void
CGUIInput::Clear( void )
{
	m_pInputString[0] = '\0';

	m_wShowBeginCharIndex = 0;

	if( IsFocused() )
		gInputMgr.SetText( m_pInputString );		

	SetShowCharIndex();
}

void
CGUIInput::SetText( char * pi_pText )
{
	if( pi_pText == NULL )
		m_pInputString[0] = '\0';
	else
		strcpy( m_pInputString, pi_pText );	

	if( IsFocused() )	
		gInputMgr.SetText( m_pInputString );

	SetShowCharIndex();

	// 오른쪽 정렬이면 캐럿을 항상 오른쪽끝으로 한다.
	if( m_eAlignment == Right_Alignment )	
		SetCaretPos( strlen( pi_pText ) );
}

void
CGUIInput::SetCaretPos( int pi_nLeftStrIndexOfCaret )
{	
	if(	pi_nLeftStrIndexOfCaret < 0 )
		return;	

	int l_nLeftStrIndexOfCaret = pi_nLeftStrIndexOfCaret;

	if( l_nLeftStrIndexOfCaret > (int)strlen( m_pInputString ) )
	{
		l_nLeftStrIndexOfCaret = strlen( m_pInputString );		
	}
	else if( l_nLeftStrIndexOfCaret > 0 )
	{
		// 한글이 짤리면 다음 index를 caret의 위치로 정한다.
		for( int i=l_nLeftStrIndexOfCaret-1; i>=0; --i )
		{
			if( !(m_pInputString[i] & 0x80) )
				break;
		}

		// 한글
		if( ( l_nLeftStrIndexOfCaret - i )%2 == 0 )
			++l_nLeftStrIndexOfCaret;	
	}

	if( l_nLeftStrIndexOfCaret < m_wShowBeginCharIndex )
		l_nLeftStrIndexOfCaret = m_wShowBeginCharIndex;
	else if( l_nLeftStrIndexOfCaret > m_wShowEndCharIndex + 1 )
		l_nLeftStrIndexOfCaret = m_wShowEndCharIndex + 1;

	// update caret pos x
	m_nCaretPosX = l_nLeftStrIndexOfCaret * ONEBYTE_FONTSIZE;

	if( IsFocused() )
	{		
		gInputMgr.m_cIME.SetCaretPosition( m_nCaretPosX, 0 );
	}
}

BOOL
CGUIInput::Draw( void )
{
	if( !CGUIObject::Draw() )
		return false;

	// draw text
	if( m_bIsSecretText )					
		DrawSecretText();	// text 대신 특수문자를 출력한다.
	else			
		DrawText();


	if( IsFocused() )
	{		
		// draw caret
		gInputMgr.DrawCaret();

		// draw composition string
		char * l_pCompStr;
		l_pCompStr = gInputMgr.m_cIME.GetCompString();
		if( l_pCompStr[0] != '\0' )
		{
			DrawR3Hangul( gInputMgr.m_cIME.nCaretPos.x,
						  gInputMgr.m_cIME.nCaretPos.y,
						  l_pCompStr,
						  m_dwTextColor );
		}		
	}

	return true;
}

BOOL
CGUIInput::Draw( RECT & pi_rtClipping )
{
	return CGUIInput::Draw();
}

void
CGUIInput::DrawText( void )
{	
	if( m_pInputString[0] == '\0' )
		return;	
	
	char	buf[MAX_INPUT_STRING+1];		

	WORD l_wCaretSize;
	if( IsFocused() )
	{
		if( gInputMgr.m_cIME.GetCompLength() )
			l_wCaretSize = TWOBYTE_FONTSIZE;
		else
			l_wCaretSize = 2;
	}
	else { l_wCaretSize = 0; }
	

	WORD l_wLeftStrOfCaretLen = m_nCaretPosX / ONEBYTE_FONTSIZE;	

	// copy showed left string of caret	
	memcpy( buf, m_pInputString + m_wShowBeginCharIndex,
			l_wLeftStrOfCaretLen - m_wShowBeginCharIndex );
	buf[l_wLeftStrOfCaretLen - m_wShowBeginCharIndex] = '\0';

	// draw text
	if( m_eAlignment == Left_Alignment )
	{
		// left string of caret 
		DrawR3Hangul( m_ptPos.x + m_nStrPosX,
					  m_ptPos.y + m_ptMargin.y,
					  buf,
					  m_dwTextColor );

		// right string of caret		
		if( l_wLeftStrOfCaretLen <= m_wShowEndCharIndex )
		{
			memcpy( buf, m_pInputString + l_wLeftStrOfCaretLen,
					m_wShowEndCharIndex - l_wLeftStrOfCaretLen + 1 );
			buf[m_wShowEndCharIndex - l_wLeftStrOfCaretLen + 1] = '\0';

			// 조합중일 때만 캐럿크기만큼 띄운다.
			if( l_wCaretSize > 2 ) 
				DrawR3Hangul( m_ptPos.x + m_nStrPosX +
								(l_wLeftStrOfCaretLen - m_wShowBeginCharIndex) * ONEBYTE_FONTSIZE + l_wCaretSize,
							  m_ptPos.y + m_ptMargin.y,
							  buf,
							  m_dwTextColor );
			else
				DrawR3Hangul( m_ptPos.x + m_nStrPosX +
								(l_wLeftStrOfCaretLen - m_wShowBeginCharIndex) * ONEBYTE_FONTSIZE,
							  m_ptPos.y + m_ptMargin.y,
							  buf,
							  m_dwTextColor );

		}		
	}
	else
	{		
		// 오른쪽 정렬은 커서크기만큼 string을 왼쪽으로 옮겨줘야한다.
		DrawR3Hangul( m_ptPos.x + m_nStrPosX - l_wCaretSize,
					  m_ptPos.y + m_ptMargin.y,
					  buf,
					  m_dwTextColor );

		// right string of caret
		if( l_wLeftStrOfCaretLen <= m_wShowEndCharIndex )
		{
			memcpy( buf, m_pInputString + l_wLeftStrOfCaretLen,
					m_wShowEndCharIndex - l_wLeftStrOfCaretLen + 1 );
			buf[m_wShowEndCharIndex - l_wLeftStrOfCaretLen + 1] = '\0';

			// 캐럿크기만큼 띄운다.
			if( l_wCaretSize > 0 )
				DrawR3Hangul( m_ptPos.x + m_nStrPosX +
								(l_wLeftStrOfCaretLen - m_wShowBeginCharIndex) * ONEBYTE_FONTSIZE + l_wCaretSize,
							  m_ptPos.y + m_ptMargin.y,
							  buf,
							  m_dwTextColor );
			else
				DrawR3Hangul( m_ptPos.x + m_nStrPosX +
								(l_wLeftStrOfCaretLen - m_wShowBeginCharIndex) * ONEBYTE_FONTSIZE,
							  m_ptPos.y + m_ptMargin.y,
							  buf,
							  m_dwTextColor );

		}

	}
}

void
CGUIInput::DrawSecretText( void )
{	
	WORD	len = strlen( m_pInputString );
	if( len == 0 || m_wShowBeginCharIndex >= len )
		return;
	
	char	buf[MAX_INPUT_STRING+1];

	// showed text copy	
	memset( buf, '*', len - m_wShowBeginCharIndex );
	buf[len - m_wShowBeginCharIndex] = '\0';	

	DrawR3Hangul( m_ptPos.x + m_nStrPosX,
				  m_ptPos.y + m_ptMargin.y,
				  buf,
				  m_dwTextColor );
}

LRESULT
CGUIInput::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsFocused() )
		return 0;
	
	switch( uMsg )
	{
	case WM_IME_COMPOSITION:
		{
			// 글자수 제한
			WORD inputLength = gInputMgr.m_cIME.GetResultLength();
			if( inputLength >= MAX_INPUT_STRING ||
				inputLength >= m_wInputLenLimit )
				return 1;

			if( m_eInputType == Only_Number )
				return 1;

			gInputMgr.m_cIME.OnIMEComposition( hWnd, wParam, lParam );

			char * l_pText = gInputMgr.m_cIME.GetText();
			memcpy( m_pInputString, l_pText, MAX_INPUT_STRING );
			m_pInputString[ strlen( l_pText ) ] = '\0';			

			SetShowCharIndex();

			return DefWindowProc( hWnd, uMsg, wParam, lParam );
		}
		break;
	case WM_KEYDOWN:
		{
			if( EP_ProcessFiltering_KeydownMsg( lParam ) )	// JSH
				return CGUIObject::MsgProc( hWnd, uMsg, wParam, lParam );

			if( wParam == VK_RETURN )
			{
				return CGUIObject::MsgProc( hWnd, uMsg, wParam, lParam );
			}
			// scret text이면 영문으로만 입력할수 있다.
			else if( wParam == VK_HANGUL && m_bIsSecretText )
			{				
				gInputMgr.SetEnglish();

				return CGUIObject::MsgProc( hWnd, uMsg, wParam, lParam );									
			}
			// 오른쪽 정렬인 경우 캐럿 이동 못하게..
			else if( m_eAlignment == Right_Alignment &&
					 ( wParam == VK_LEFT || wParam == VK_RIGHT ) )
			{			
				return CGUIObject::MsgProc( hWnd, uMsg, wParam, lParam );
			}			

			gInputMgr.m_cIME.OnKeyDown( hWnd, wParam, lParam );

			char * l_pText = gInputMgr.m_cIME.GetText();
			memcpy( m_pInputString, l_pText, MAX_INPUT_STRING );
			m_pInputString[ strlen( l_pText ) ] = '\0';			
			
			SetShowCharIndex();

			return CGUIObject::MsgProc( hWnd, uMsg, wParam, lParam );
		}
		break;
	case WM_CHAR:
		{
			// 글자수 제한
			WORD inputLength = strlen( m_pInputString );
			if( inputLength >= MAX_INPUT_STRING ||
				inputLength >= m_wInputLenLimit )
				return 1;

			if( m_eInputType == Only_Number &&
				!isdigit( wParam ) )
				return 1;
			else if( m_eInputType == Only_Char &&
				isdigit( wParam ) )
				return 1;

			gInputMgr.m_cIME.OnChar( hWnd, wParam, lParam );

			char * l_pText = gInputMgr.m_cIME.GetText();
			memcpy( m_pInputString, l_pText, MAX_INPUT_STRING );
			m_pInputString[ strlen( l_pText ) ] = '\0';
			
			SetShowCharIndex();
			
			return 1;
		}
		break;
	}

	return 1;
}