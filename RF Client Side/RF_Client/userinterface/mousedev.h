////////////////////////////////////////////////////////////////////////////
//
// CInputDev Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __MOUSEDEV_H__
#define __MOUSEDEV_H__

#include "R3Input.h"
#include "Singleton.h"

#define		L_BUTTON		1
#define		R_BUTTON		2

class CMouseDev : public CSingleton<CMouseDev>
{
public:
	enum {
		STROKED	= 0x0001,
		PRESSED = 0x0002,
		RELEASED = 0x0004,
		DRAGGED = 0x0008		
	};
// < Data Member > ---------------------------------------------------------
private:
	WORD	m_wKey;
	DWORD	m_wState;
	POINT	m_lPrevPos;
	POINT	m_lMovePos;
	BOOL	m_bDrag;
	BOOL	m_bPressed;

// < Member Function > -----------------------------------------------------
public:
	CMouseDev();
	~CMouseDev();

	void	Update( void );
	void	Clear( void );

	BOOL	IsStroked( WORD	pi_wKey );
	BOOL	IsPressed( WORD pi_wKey );
	BOOL	IsReleased( WORD pi_wKey );
	BOOL	IsDragged( void );
	BOOL	IsMoved( void );
	BOOL	IsWheelScrolled( void );

	inline	LONG	GetMoveX( void ) { return m_lMovePos.x; }
	inline	LONG	GetMoveY( void ) { return m_lMovePos.y; }

	inline	LONG	GetX( void ) { return gMouse.x; }
	inline	LONG	GetY( void ) { return gMouse.y; }
	inline	LONG	GetZ( void ) { return gMouse.z; }
};

#define	gMouseDev	CMouseDev::GetSingleton()

#endif // __MOUSEDEV_H__
