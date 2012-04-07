#include "GUIText.h"
#include <malloc.h>


////////////////////////////////////////////////////////////////////////////
//
// CGUITextInfo Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUITextInfo::CGUITextInfo()
{
	m_pText = NULL;
}

CGUITextInfo::~CGUITextInfo()
{
	if( m_pText )
		delete [] m_pText;
}

void 
CGUITextInfo::SetText( char * pi_pText )
{
	if( !m_pText )
	{
		if( pi_pText == NULL )
			m_pText = new char[1];
		else
			m_pText = new char[ strlen( pi_pText ) + 1 ];
	}
	else
	{		
		if( pi_pText != NULL &&
			_msize( m_pText ) < strlen( pi_pText ) + 1 )
		{
			delete[] m_pText;
			m_pText = new char[ strlen( pi_pText ) + 1 ];
		}
	}

	if( pi_pText == NULL )
		m_pText[0] = '\0';
	else
		strcpy( m_pText, pi_pText );
}

void
CGUITextInfo::Clear( void )
{
	if( m_pText )
	{ 
		delete [] m_pText;
		m_pText = NULL;
	}
}

void
CGUITextInfo::SetFont( FONT_INFO * pi_pFontInfo )
{
	memcpy( &m_sFontInfo, pi_pFontInfo, sizeof( FONT_INFO ) );
}

CGUITextInfo&
CGUITextInfo::operator=( CGUITextInfo & pi_TextInfo )
{
	SetText( pi_TextInfo.GetText() );
	SetFont( pi_TextInfo.GetFont() );

	return *this;
}

////////////////////////////////////////////////////////////////////////////
//
// CGUIText Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIText::CGUIText()
{
	SetSize( 0, TWOBYTE_FONTSIZE );
}

CGUIText::~CGUIText()
{

}

void
CGUIText::SetString( char * pi_pStr )
{
	m_sTextInfo.SetText( pi_pStr );

	if( pi_pStr == NULL )
		SetSize( 0, TWOBYTE_FONTSIZE );
	else
		SetSize( strlen(pi_pStr) * ONEBYTE_FONTSIZE, TWOBYTE_FONTSIZE );
}

void
CGUIText::SetFont( FONT_INFO * pi_pFont )
{
	m_sTextInfo.SetFont( pi_pFont );
}

void
CGUIText::SetTextInfo( CGUITextInfo * pi_pTextInfo )
{
	memcpy( &m_sTextInfo, pi_pTextInfo, sizeof( CGUITextInfo ) );
}

BOOL
CGUIText::Draw( void )
{
	if( !IsShow() )
		return FALSE;

	if( m_sTextInfo.GetText() )
	{
		DrawR3Hangul( m_ptPos.x,
					  m_ptPos.y,
					  m_sTextInfo.GetText(),
					  m_sTextInfo.GetFont()->dwColor,
					  m_sTextInfo.GetFont()->dwStyle );
	}

	return TRUE;
}

BOOL
CGUIText::Draw( RECT & pi_rtClipping )
{
	if( !IsShow() )
		return FALSE;

	if( m_sTextInfo.GetText() )
	{
		float clipping[2][2];
		clipping[0][0] = (float)( pi_rtClipping.left - m_ptPos.x ) / m_ptSize.x;
		clipping[0][1] = (float)( pi_rtClipping.right - m_ptPos.x ) / m_ptSize.x;
		clipping[1][0] = (float)( pi_rtClipping.top - m_ptPos.y ) / m_ptSize.y;
		clipping[1][1] = (float)( pi_rtClipping.bottom - m_ptPos.y ) / m_ptSize.y;

		DrawR3Hangul( pi_rtClipping.left,
					  pi_rtClipping.top,					  
					  m_sTextInfo.GetText(),
					  m_sTextInfo.GetFont()->dwColor,
					  clipping,
					  m_sTextInfo.GetFont()->dwStyle );
	}

	return TRUE;
}