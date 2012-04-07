////////////////////////////////////////////////////////////////////////////
//
// CMouseDev Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "MouseDev.h"

////////////////////////////////////////////////////////////////////////////
//
CMouseDev::CMouseDev()
{
	m_wKey = 0;
	m_wState = 0x0000;
	m_bPressed = false;
	m_bDrag = false;		
}

CMouseDev::~CMouseDev()
{

}

void
CMouseDev::Update( void )
{
	BOOL	l_bPress = false;
	m_wState = 0x0000;

	m_lMovePos.x = gMouse.x - m_lPrevPos.x;
	m_lMovePos.y = gMouse.y - m_lPrevPos.y;

	if( gMouse.button & _LEFT_BUTTON_DOWN )
	{
		m_wKey = L_BUTTON;
		l_bPress = true;
	}
	else if( gMouse.button & _RIGHT_BUTTON_DOWN )
	{
		m_wKey = R_BUTTON;
		l_bPress = true;
	}

	if( l_bPress )
	{
		if( !m_bPressed )
		{
			m_wState |= STROKED;
			m_bPressed = true;
		}
		else
		{
			m_wState &= ~STROKED;
		}

		m_wState |= PRESSED;

		if( m_wKey == L_BUTTON )
		{
			m_bDrag = true;
		}
	}
	else
	{
		if( m_bPressed )
		{
			m_wState |= RELEASED;
			m_bPressed = false;
		}
		else 
		{
			m_wKey = 0;
		}

		m_bDrag = false;
	}

	m_lPrevPos.x = gMouse.x;
	m_lPrevPos.y = gMouse.y;
}

BOOL
CMouseDev::IsStroked( WORD	pi_wKey )
{
	if( m_wKey != pi_wKey )
		return false;

	if( m_wState & STROKED )
		return true;

	return false;
}

BOOL
CMouseDev::IsPressed( WORD pi_wKey )
{
	if( m_wKey != pi_wKey )
		return false;

	if( m_wState & PRESSED )
		return true;

	return false;
}

BOOL
CMouseDev::IsReleased( WORD pi_wKey )
{
	if( m_wKey != pi_wKey )
		return false;

	if( m_wState & RELEASED )
		return true;

	return false;
}

BOOL
CMouseDev::IsDragged( void )
{
	if( m_bDrag &&
		( m_lMovePos.x != 0 || m_lMovePos.y != 0 ) )
	{
		return true;
	}

	return false;
}

BOOL
CMouseDev::IsMoved( void )
{
	if( m_bDrag )
		return false;

	if( m_lMovePos.x != 0 || m_lMovePos.y != 0 )
		return true;

	return false;
}

BOOL
CMouseDev::IsWheelScrolled( void )
{
	if( gMouse.z != 0 )
		return true;

	return false;
}