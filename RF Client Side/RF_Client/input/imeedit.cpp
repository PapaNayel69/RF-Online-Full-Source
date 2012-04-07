/*------------------------------------------------------------
	File name   : IMEEdit.cpp
	Description : IME for TestClient Project
	Author      : Jingyu,Kim
	Update Date : 2002.1.11

	Copyright (c) 2001-2002 CCR Entertainment Inc.
	All rights reserved.
-------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include "R3Text.h"
#include "IMEEdit.h"

//#define LEFT_SPACE 20	// 에디트 박스에서 왼쪽으로 실제 글자가 찍히는곳 만큼 간격을 준다.
//#define TOP_SPACE 30    // 에디트 박스에서 아래로 실제 글자가 찍히는곳 만큼 간격을 준다.
#define LEFT_SPACE 0	// 에디트 박스에서 왼쪽으로 실제 글자가 찍히는곳 만큼 간격을 준다. // 임시코드
#define TOP_SPACE 0    // 에디트 박스에서 아래로 실제 글자가 찍히는곳 만큼 간격을 준다.

CIMEEdit::CIMEEdit()
{
	m_bEnableInput		= TRUE;

	m_bEnableBlinkCaret	= TRUE;
	m_bBlinkCaretState	= TRUE;
	m_dwBlinkGap		= 500;
	m_dwPrevBlinkTime	= 0;
}

CIMEEdit::~CIMEEdit()
{
	if ( GetCompLength() )
		szCompStr[0] = '\0'; 

	if ( GetResultLength() )
		szResultStr[0] = '\0'; 
}

/*------------------------------------------------------------
	Description  : IME Edit 초기화 
	Function     : IMEEditInit()
	parameter    : 
	Return Value : 
	Exception    :
	Author       : Jingyu,Kim
	Update Date  : 2002.3.19
-------------------------------------------------------------*/
void CIMEEdit::IMEEditInit(POINT nPos, int nLength, DWORD dwTextColor )
{
	nEditBoxPos		= nPos; 
	nEditBoxLength	= nLength;

	nCaretPos.x		= nEditBoxPos.x + LEFT_SPACE;
	nCaretPos.y		= nEditBoxPos.y + TOP_SPACE;

	m_dwTextColor	= dwTextColor;
	m_dwCaretColor	= dwTextColor;

	ZeroMemory( szCompStr, MAX_SIZE_IMEBUFFER );
	ZeroMemory( szResultStr, MAX_SIZE_TEXTBUFFER );
}

/*------------------------------------------------------------
	Description  : 
	Function     : OnChar()
	parameter    : 
	Return Value : 
	Exception    :
	Author       : Jingyu,Kim
	Update Date  : 2002.1.11
-------------------------------------------------------------*/
void CIMEEdit::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if( !m_bEnableInput ) return;

	// 테스트 중임
	if ( GetResultLength() > nEditBoxLength )
		return;	

	char ch = (char)wParam;

	if ( ch < 0x20 || ch > 0x7E || ch & 0x80 ) // 0x20은 스페이스 키
		return;

	// 키 입력을 받고 영문자와 특수문자를 표현한다.
	// 한글 IME 때문에 여러번 호출되는 경우가 있기 때문 
	if ( bKeyDown ) 
	{
		// 캐럿의 왼쪽에서부터 문자를 입력받는다.
		int l_nInsertStrIndex = ( nCaretPos.x - nEditBoxPos.x - LEFT_SPACE ) / ONEBYTE_FONTSIZE;

		// 중간에 삽입할 경우 한칸씩 뒤로 밀어낸다.
		int nStrSize = GetResultLength();
		if( nStrSize - l_nInsertStrIndex > 0 )
			memcpy( szResultStr + l_nInsertStrIndex + 1,
					szResultStr + l_nInsertStrIndex,
					nStrSize - l_nInsertStrIndex );

		szResultStr[l_nInsertStrIndex] = ch;

		szResultStr[nStrSize+1] = '\0';

		nCaretPos.x += ONEBYTE_FONTSIZE;

		bKeyDown = FALSE;
	}

	/*
	OutputDebugString(szResultStr);
	OutputDebugString("\n");
	*/
}	

/*------------------------------------------------------------
	Description  : 
	Function     : OnKeyDown()
	parameter    : 
	Return Value : 
	Exception    :
	Author       : Jingyu,Kim
	Update Date  : 2002.1.8
-------------------------------------------------------------*/
void CIMEEdit::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam) 
{
	if( !m_bEnableInput ) return;

	bKeyDown = TRUE;

	int nStrSize;

	switch( wParam ) 
	{
	case VK_BACK: // Backspace 키를 눌렀을때 글자가 뒤에서 지워지는 코딩 

		if ( nCaretPos.x > nEditBoxPos.x + LEFT_SPACE )
		{
			nStrSize = GetResultLength();

			int l_nDeleteStrIndex = ( nCaretPos.x - nEditBoxPos.x - LEFT_SPACE ) / ONEBYTE_FONTSIZE - 1;
			
			BOOL bIsHangul;

			if( szResultStr[l_nDeleteStrIndex] & 0x80 )
			{
				bIsHangul = TRUE;
			}
			else
			{				
				// 잘못된 조합 체크
				// l_nStrDeleteIndex에 문자가 한글이 아니라도 잘못된 조합의 한글일 수 있다.
				// 잘못된 조합의 경우 앞바이트는 한글이고 뒤바이트가 영문자이다.
				for( int i=l_nDeleteStrIndex-1; i>=0; --i )
				{
					if( !(szResultStr[i] & 0x80) )
						break;
				}

				if( (l_nDeleteStrIndex-i)%2 == 0 )
					bIsHangul = TRUE;
				else
					bIsHangul = FALSE;				
			}		


			if( bIsHangul )
			{
				// 스트링 중간에서 삭제하면 2byte 앞으로 당긴다.
				if( l_nDeleteStrIndex < nStrSize - 1 )
				{				
					memcpy( szResultStr + l_nDeleteStrIndex - 1,
							szResultStr + l_nDeleteStrIndex + 1, 
							nStrSize - (l_nDeleteStrIndex+1) );
				}

				szResultStr[nStrSize - 1] = '\0';
				szResultStr[nStrSize - 2] = '\0';

				nCaretPos.x -= ONEBYTE_FONTSIZE * 2;
			}
			else
			{
				// 1byte 앞으로 당긴다.
				if( l_nDeleteStrIndex < nStrSize - 1 )
				{
					memcpy( szResultStr + l_nDeleteStrIndex,
							szResultStr + l_nDeleteStrIndex + 1, 
							nStrSize - (l_nDeleteStrIndex+1) );
				}

				szResultStr[nStrSize - 1] = '\0';

				nCaretPos.x -= ONEBYTE_FONTSIZE;
			}
		}
		
		break;

	case VK_DELETE: // 캐럿의 오른쪽에 있는 글자를 지운다.

		nStrSize = GetResultLength();
		
		if( nCaretPos.x < nEditBoxPos.x + LEFT_SPACE + nStrSize * ONEBYTE_FONTSIZE )
		{
			int l_nDeleteStrIndex = ( nCaretPos.x - nEditBoxPos.x - LEFT_SPACE ) / ONEBYTE_FONTSIZE;

			if ( szResultStr[l_nDeleteStrIndex] & 0x80 )
			{
				// 한글이면 2byte 앞으로 당긴다.
				memcpy( szResultStr + l_nDeleteStrIndex,
						szResultStr + l_nDeleteStrIndex + 2, 
						nStrSize - l_nDeleteStrIndex - 2 );

				szResultStr[nStrSize - 1] = '\0';
				szResultStr[nStrSize - 2] = '\0';
			}
			else
			{			
				// 영문이면 1byte 앞으로 당긴다.
				memcpy( szResultStr + l_nDeleteStrIndex,
						szResultStr + l_nDeleteStrIndex + 1, 
						nStrSize - l_nDeleteStrIndex - 1 );

				szResultStr[nStrSize - 1] = '\0';
			}

			// 캐럿의 위치는 그대로다.
		}
		break;

	case VK_LEFT: // 캐럿을 왼쪽으로 옮긴다.

		if( nCaretPos.x > nEditBoxPos.x + LEFT_SPACE )
		{			
			nStrSize = GetResultLength();		

			int l_nLeftStrIndexOfCaret = ( nCaretPos.x - nEditBoxPos.x - LEFT_SPACE ) / ONEBYTE_FONTSIZE - 1;
			
			BOOL bIsHangul;
			if( szResultStr[l_nLeftStrIndexOfCaret] & 0x80 )
			{
				bIsHangul = TRUE;
			}
			else
			{				
				// 잘못된 조합 체크
				// l_nStrDeleteIndex에 문자가 한글이 아니라도 잘못된 조합의 한글을 수 있다.
				// 잘못된 조합의 경우 앞바이트는 한글이고 뒤바이트가 영문자이다.
				for( int i=l_nLeftStrIndexOfCaret-1; i>=0; --i )
				{
					if( !(szResultStr[i] & 0x80) )					
						break;		
				}

				if( (l_nLeftStrIndexOfCaret-i)%2 == 0 )
					bIsHangul = TRUE;
				else
					bIsHangul = FALSE;				
			}


			if( bIsHangul )
			{
				nCaretPos.x -= ONEBYTE_FONTSIZE * 2;
			}
			else
			{
				nCaretPos.x -= ONEBYTE_FONTSIZE;
			}
		}			
		
		break;

	case VK_RIGHT:	// 캐럿을 오른쪽으로 옮긴다.

		nStrSize = GetResultLength();

		if( nCaretPos.x < nEditBoxPos.x + LEFT_SPACE + nStrSize * ONEBYTE_FONTSIZE )
		{
			int l_nRightStrIndexOfCaret = ( nCaretPos.x - nEditBoxPos.x - LEFT_SPACE ) / ONEBYTE_FONTSIZE;

			if( szResultStr[l_nRightStrIndexOfCaret] & 0x80 )
			{
				nCaretPos.x += ONEBYTE_FONTSIZE * 2;
			}
			else
			{
				nCaretPos.x += ONEBYTE_FONTSIZE;
			}
		}
		
		break;

	case VK_HOME:	// 캐럿을 맨앞으로

		nCaretPos.x = nEditBoxPos.x + LEFT_SPACE;

		break;

	case VK_END:	// 캐럿을 맨뒤로

		nStrSize = GetResultLength();
		nCaretPos.x = nEditBoxPos.x + LEFT_SPACE + nStrSize * ONEBYTE_FONTSIZE;

		break;

	}

	return;
}	

/*------------------------------------------------------------
	Description  : 한글 자소가 입력되면 호출된다. WM_IME_COMPOSITION
	Function     : OnIMEComposition()
	parameter    : 
	Return Value : 
	Exception    :
	Author       : Jingyu,Kim
	Update Date  : 2002.1.8
-------------------------------------------------------------*/
void CIMEEdit::OnIMEComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if( !m_bEnableInput ) return;

	if ( GetResultLength() > nEditBoxLength )
		return;

    DWORD dwBufLen; // Stogare for len. of composition str
    HIMC  hIMC;

	hIMC = ImmGetContext(hWnd);

	if ( !hIMC )
		return;

	if ( lParam & GCS_COMPSTR )
	{
		dwBufLen = ::ImmGetCompositionString(hIMC, GCS_COMPSTR, (void FAR*)NULL, 0l);

		// 백스페이스 키를 눌렀을때 조합중인 문자 완전히 없애기 위해 
		if ( dwBufLen <= 0 )    
			szCompStr[0] = '\0';
	}
	else if ( lParam & GCS_RESULTSTR )
	{
		dwBufLen = ::ImmGetCompositionString(hIMC, GCS_RESULTSTR, (void FAR*)NULL, 0l);
	}

	if ( dwBufLen )
	{
		if ( lParam & GCS_COMPSTR )
		{
			::ImmGetCompositionString(hIMC, GCS_COMPSTR, szCompStr, dwBufLen);

			// 잘못된 조합의 경우 이상한 문자가 삽입되는것을 막는다.
			bKeyDown = FALSE; 
		}
		else if ( lParam & GCS_RESULTSTR )
		{
			::ImmGetCompositionString(hIMC, GCS_RESULTSTR, szCompStr, dwBufLen);


			int nStrSize = GetResultLength();
			int nCompSize = GetCompLength();

			// 한글조합 후 바로 연이어서 특수문자가 들어갈 경우 
			if ( nCompSize > 2 )
			{
				szCompStr[nCompSize - 1] = '\0';
				--nCompSize;
			}

			int l_nInsertStrIndex = ( nCaretPos.x - nEditBoxPos.x - LEFT_SPACE ) / ONEBYTE_FONTSIZE;

			// 중간에 삽입하는 경우 삽입할 곳에서부터 뒤로 한칸씩 밀어낸다.
			if( nStrSize - l_nInsertStrIndex > 0 )			
				memcpy( szResultStr + l_nInsertStrIndex + nCompSize,
						szResultStr + l_nInsertStrIndex,
						nStrSize - l_nInsertStrIndex );			

			// 조합한 문자열 삽입
			memcpy( szResultStr + l_nInsertStrIndex,
					szCompStr,
					nCompSize );

			szResultStr[nStrSize + nCompSize] = '\0';


			nCaretPos.x += nCompSize * ONEBYTE_FONTSIZE;

			// 잘못된 조합인 경우 연이어 붙은 특수문자를 삽입하지 않는다.
			if( !( szCompStr[1] & 0x80 ) )
				bKeyDown = FALSE;


			szCompStr[0] = '\0'; // 조합중인 문자열을 제거한다.
		}

		/*
		OutputDebugString(szCompStr);
		OutputDebugString("\n");
		OutputDebugString(szResultStr);
		OutputDebugString("\n");
		*/
	}

    ImmReleaseContext(hWnd, hIMC);

    return;
}

/*------------------------------------------------------------
	Description  : 
	Function     : TextOutput()
	parameter    : 
	Return Value : 
	Exception    :
	Author       : Jingyu,Kim
	Update Date  : 2002.1.10
-------------------------------------------------------------*/
void CIMEEdit::TextOutput(HWND hWnd)
{
	if ( GetResultLength() )
	{
		DrawR3Hangul(nEditBoxPos.x + LEFT_SPACE, nEditBoxPos.y + TOP_SPACE, szResultStr, m_dwTextColor );
	}

	if ( GetCompLength() )
	{
		DrawR3Hangul(nCaretPos.x, nCaretPos.y, szCompStr, m_dwCaretColor );
	}
}

/*------------------------------------------------------------
	Description  : 
	Function     : ShowCaret()
	parameter    : 
	Return Value : 
	Exception    :
	Author       : Jingyu,Kim
	Update Date  : 2002.3.18
-------------------------------------------------------------*/
void CIMEEdit::ShowCaret(HWND hWnd)
{
	if( !m_bEnableInput ) return;

	if( m_bEnableBlinkCaret )
	{
		if( ( timeGetTime() - m_dwPrevBlinkTime ) > m_dwBlinkGap )
		{
			m_bBlinkCaretState = !m_bBlinkCaretState;
			m_dwPrevBlinkTime = timeGetTime();
		}
	}

	if( !m_bEnableBlinkCaret || ( m_bEnableBlinkCaret && m_bBlinkCaretState ) )
	{
		if ( IMEIsKoreanMode(hWnd) && GetCompLength() )
			::Draw2DRectangle(nCaretPos.x, nCaretPos.y, nCaretPos.x + TWOBYTE_FONTSIZE, nCaretPos.y + 12, m_dwCaretColor );
		else
			::Draw2DRectangle(nCaretPos.x, nCaretPos.y, nCaretPos.x + 2, nCaretPos.y + 12, m_dwCaretColor );
	}
}

/*------------------------------------------------------------
	Description  : 
	Function     : IMEToggleLanguage()
	parameter    : 
	Return Value : 
	Exception    :
	Author       : Jingyu,Kim
	Update Date  : 2002.1.11
-------------------------------------------------------------*/
BOOL CIMEEdit::IMEToggleLanguage(HWND hWnd)
{
    HIMC    hIMC		= NULL;
    BOOL    bRet		= FALSE;
    DWORD   fdwConvMode	= 0;
	DWORD	fdwSentence	= 0;

    hIMC = ::ImmGetContext(hWnd);

    if ( hIMC != NULL )
    {
        bRet = ::ImmGetConversionStatus(hIMC, &fdwConvMode, &fdwSentence);

        if ( bRet == TRUE )
        {
			if ( fdwConvMode & IME_CMODE_NATIVE ) 
			{
				// 한글모드이면 영문모드로 바꾼다.
				fdwConvMode &= ( ~IME_CMODE_NATIVE );
			}
			else
			{
				// 영문모드이면 한글모드로 바꾼다. 
				fdwConvMode = IME_CMODE_NATIVE;
			}
		
            bRet = ::ImmSetConversionStatus(hIMC, fdwConvMode, fdwSentence);
        }
    }

	return bRet;
}

/*------------------------------------------------------------
	Description  : 
	Function     : IMESetKorean()
	parameter    : 
	Return Value : 
	Exception    :
	Author       : Jingyu,Kim
	Update Date  : 2002.3.21
-------------------------------------------------------------*/
BOOL CIMEEdit::IMESetKorean(HWND hWnd)
{
    HIMC    hIMC		= NULL;
    BOOL    bRet		= FALSE;
    DWORD   fdwConvMode	= 0;
	DWORD	fdwSentence	= 0;

    hIMC = ::ImmGetContext(hWnd);

    if ( hIMC != NULL )
    {
        bRet = ::ImmGetConversionStatus(hIMC, &fdwConvMode, &fdwSentence);

        if ( bRet == TRUE )
        {
            fdwConvMode = IME_CMODE_NATIVE;
            bRet = ::ImmSetConversionStatus(hIMC, fdwConvMode, fdwSentence);
        }
    }

	return bRet;
}	

/*------------------------------------------------------------
	Description  : 
	Function     : IMESetEnglish()
	parameter    : 
	Return Value : 
	Exception    :
	Author       : Jingyu,Kim
	Update Date  : 2002.3.21
-------------------------------------------------------------*/
BOOL CIMEEdit::IMESetEnglish(HWND hWnd)
{
    HIMC    hIMC		= NULL;
    BOOL    bRet		= FALSE;
    DWORD   fdwConvMode	= 0;
	DWORD	fdwSentence	= 0;

    hIMC = ::ImmGetContext(hWnd);

    if ( hIMC != NULL )
    {
        bRet = ::ImmGetConversionStatus(hIMC, &fdwConvMode, &fdwSentence);

        if ( bRet == TRUE )
        {
            fdwConvMode &= ( ~IME_CMODE_NATIVE );
            bRet = ::ImmSetConversionStatus(hIMC, fdwConvMode, fdwSentence);
        }
    }

	return bRet;
}	

/*------------------------------------------------------------
	Description  : 
	Function     : IMEIsKoreanMode()
	parameter    : 
	Return Value : 
	Exception    :
	Author       : Jingyu,Kim
	Update Date  : 2002.1.11
-------------------------------------------------------------*/
BOOL CIMEEdit::IMEIsKoreanMode(HWND hWnd)
{
    HIMC    hIMC		= NULL;
    BOOL    bRet		= FALSE;
    DWORD   fdwConvMode	= 0;
	DWORD	fdwSentence	= 0;

    hIMC = ::ImmGetContext(hWnd);

    if ( hIMC != NULL )
    {
        bRet = ::ImmGetConversionStatus(hIMC, &fdwConvMode, &fdwSentence);

        if ( bRet == TRUE )
		{
            if ( fdwConvMode & IME_CMODE_NATIVE )
				return TRUE;
		}
    }

	return FALSE;
}

/*------------------------------------------------------------
	Description  : 
	Function     : IsHangulMiddleByte()
	parameter    : 
	Return Value : 
	Exception    :
	Author       : Jingyu,Kim
	Update Date  : 2002.3.19
-------------------------------------------------------------*/
BOOL CIMEEdit::IsHangulMiddleByte(LPCSTR lpszStr, BYTE pos)
{
    if ( !lpszStr ) 
		return FALSE;

    if ( pos <= 0 ) 
		return FALSE;

    int nLength = strlen(lpszStr);

    if ( pos >= nLength ) 
		return FALSE;

    if ( !(lpszStr[pos] & 0x80) )
        return FALSE;

    register BOOL bMiddle = FALSE;
    register int i;

    for ( i = 0; i<pos && i < nLength; i++ )
    {
        if ( lpszStr[i] & 0x80 )
            bMiddle = !bMiddle;
    }

    return bMiddle;
}

void
CIMEEdit::SetIMEPosition( int x, int y )
{
	POINT l_ptTemp;

	l_ptTemp.x		= nEditBoxPos.x - x;
	l_ptTemp.y		= nEditBoxPos.y - y;

	nEditBoxPos.x	-= l_ptTemp.x;
	nEditBoxPos.y	-= l_ptTemp.y;

	nCaretPos.x		= nEditBoxPos.x + ( strlen( szResultStr ) * ONEBYTE_FONTSIZE );
	nCaretPos.y		-= l_ptTemp.y;
}

void
CIMEEdit::SetIMEPosition( POINT &pi_ptIME )
{
	POINT l_ptTemp;

	l_ptTemp.x		= nEditBoxPos.x - pi_ptIME.x;
	l_ptTemp.y		= nEditBoxPos.y - pi_ptIME.y;

	nEditBoxPos.x	-= l_ptTemp.x;
	nEditBoxPos.y	-= l_ptTemp.y;

	nCaretPos.x		= nEditBoxPos.x + ( strlen( szResultStr ) * ONEBYTE_FONTSIZE );
	nCaretPos.y		-= l_ptTemp.y;
}

void
CIMEEdit::SetIMEPosition( int x, int y, int caretX )
{
	POINT l_ptTemp;

	l_ptTemp.x		= nEditBoxPos.x - x;
	l_ptTemp.y		= nEditBoxPos.y - y;

	nEditBoxPos.x	-= l_ptTemp.x;
	nEditBoxPos.y	-= l_ptTemp.y;

	nCaretPos.x		= nEditBoxPos.x + caretX;
	nCaretPos.y		-= l_ptTemp.y;
}

void
CIMEEdit::GetIMEPosition( int &x, int &y )
{
	x = nEditBoxPos.x; 
	y = nEditBoxPos.y;
}

void
CIMEEdit::GetIMEPosition( POINT &po_ptIME )
{
	po_ptIME.x = nEditBoxPos.x; 
	po_ptIME.y = nEditBoxPos.y;
}

void
CIMEEdit::SetCaretPosition( int x, int y )
{
	nCaretPos.x = nEditBoxPos.x + x;
	nCaretPos.y = nEditBoxPos.y + y;
}

void
CIMEEdit::GetCaretPosition( int &x, int &y )
{
	x = nCaretPos.x - nEditBoxPos.x;
	y = nCaretPos.y - nEditBoxPos.y;
}

void
CIMEEdit::SetText( char * pi_pText )
{
	ZeroMemory( szCompStr, MAX_SIZE_IMEBUFFER );
	ZeroMemory( szResultStr, MAX_SIZE_TEXTBUFFER );

	memcpy( szResultStr, pi_pText, strlen( pi_pText ) );
}
