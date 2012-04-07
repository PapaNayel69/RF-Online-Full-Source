////////////////////////////////////////////////////////////////////////////
//
// GUI Object Event
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIOBJECTEVENT_H__
#define __GUIOBJECTEVENT_H__

class CGUIObject;

#define		MOUSE_STROKED	1
#define		MOUSE_PRESSED	2
#define		MOUSE_RELEASED	3
#define		MOUSE_CLICKED	4
#define		MOUSE_HOVERED	5
#define		MOUSE_LEAVED	6
#define		LMOUSE_STROKED	7

#define		MOUSE_MOVE		10
#define		MOUSE_DRAG		11

#define		KEY_STROKED		20
#define		KEY_PRESSED		21
#define		KEY_RELEASED	22

#define		SCROLLED_UP		30
#define		SCROLLED_DOWN	31

#define		SELECTED		40
#define		UNSELECTED		41

#define		DLG_BOX_OPEND	50
#define		DLG_BOX_CLOSED	51

#define		WINDOW_OPENED	60
#define		WINDOW_CLOSED	61

// ------------------------------------------------------------
struct _mouse_event {
	CGUIObject	*source;
	WORD		id;
	LONG		xPos;
	LONG		yPos;
};

struct _mouse_motion_event {
	CGUIObject	*source;
	WORD		id;
	LONG		xPos;
	LONG		yPos;
	LONG		xMove;
	LONG		yMove;
};

struct _key_event {
	CGUIObject	*source;
	WORD		id;
	BYTE		key;
};

struct _scroll_event {
	CGUIObject	*source;
	WORD		id;
};

#define	UP_BUTTON	1
#define	DOWN_BUTTON	2

struct _select_event 
{
	CGUIObject	*source;
	WORD		id;
	WORD		select;
};

// ------------------------------------------------------------

class IMouseObserver
{
public:
	virtual	void	MouseEvent( _mouse_event & event )	= 0;
};

class IMouseMotionObserver
{
public:
	virtual void	MouseMotionEvent( _mouse_motion_event & event )	= 0;
};

class IKeyObserver
{
public:	
	virtual	void	KeyEvent( _key_event & event )	= 0;
};

class IScrollObserver
{
public:
	virtual	void	ScrollEvent( _scroll_event & event )	= 0;
};

class ISelectItemObserver
{
public:
	virtual	void	SelectItemEvent( _select_event & event ) = 0;
};

//--------------------------------------------------------------------------
// event queue에 들어갈 data
struct _event_queue_data
{
	void		*	pEventHandler;

	CGUIObject	*	pSource;
	WORD			wEventID;
	BYTE			byKeyCode;
	LONG			nPosX;
	LONG			nPosY;
	LONG			nMoveX;
	LONG			nMoveY;
	WORD			wSelectIndex;

	_event_queue_data()
	{
		ZeroMemory( this, sizeof(*this) );
	}
};

#endif // __GUIOBJECTEVENT_H__