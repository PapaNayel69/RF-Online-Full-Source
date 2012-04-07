////////////////////////////////////////////////////////////////////////////
//
// CGUIObject Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIOBJECT_H__
#define __GUIOBJECT_H__

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "R3Input.h"
#include "R3Text.h"
#include "R3Render.h"
#include "2DSprite.h"
#include "R3Sound.h"

#include "../UIGlobal.h"

#include "../MouseDev.h"
#include "../NDQueue.h"

#include "GUIObjectEvent.h"

//--------------------------------------------------------------------------
// object 상태
#define	HOVERED		1
#define	PRESSED		2
#define	DISABLED	4

#define	IS_HOVERED(state)	(BOOL)(state&HOVERED)
#define	IS_PRESSED(state)	(BOOL)(state&PRESSED)
#define	IS_DISABLED(state)	(BOOL)(state&DISABLED)

// --------------------------------------------------------------------------
// sprite info
struct _sprite_info {
	CSprite		*pSprite;	
	WORD		wActionNo;
	WORD		wFrameNo;
};

typedef _sprite_info		SPRITE_INFO;

/*//////////////////////////////////////////////////////////////////////////

[ CGUIObject ]

  - 모든 GUI Controller의 Base class이다.

  - object의 크기, 위치, 상태등의 기본적이 정보를 가지고 있고
    event를 감지한다.

//////////////////////////////////////////////////////////////////////////*/
class CGUIObject
{
friend class CGUIObjectEx;
friend class CGUITileObject;
friend class CGUIContainer;
friend class CGUIWindow;
friend class CGUIScreen;
friend class CGUIButton;

private:
	// 처음으로 mouse event에 잡힌 object.
	static	CGUIObject	*	s_pMouseDetectedObject;
	static	CGUIObject	*	s_pPrevMouseDetectedObject;

	// 어떤 object의 input인지 알기 위해..
	// object를 누른 순간부터 띌때까지 pressedObject로 정한다.
	static	CGUIObject	*	s_pPressedObject;
	// 보였던 Object가 안보이거나 제거된 상태에서도 계속 누르고 있을 때나
	// guiObject가 아닌 외부 object를 눌렀을 때
	static	BOOL			s_bIsPressedOtherObject;	

	// focus된 objec는 key event를 받을 수 있다.
	static	CGUIObject	*	s_pFocusedObject;

	// event queue	
	static	CNDQueue< _event_queue_data >	s_qEventList;	

	//---------------------------------------------------------------------------------
	static	void		CreateEventQueue( WORD pi_wSize=10 );	
	static	void		EventProcess( void );	
	static	void		ClearMouseEventBuffer( void );		

public:
	static	void		SendEvent( _event_queue_data & pi_EventInfo );	
	static	void		RemoveEvent(  CGUIObject * pi_pEventObj );

	static	CGUIObject * GetMouseDetectedObject( void ) { return s_pMouseDetectedObject; }
	static	CGUIObject * GetPressedObject( void ) { return s_pPressedObject; }	
	static	CGUIObject * GetFocusedObject( void ) { return s_pFocusedObject; }	

	static	void	SetPressedOtherObject( void ) { s_bIsPressedOtherObject = TRUE; }

// < Data Member > ---------------------------------------------------------
public:
	POINT			m_ptPos;				// position
	POINT			m_ptSize;				// size

private:
	char			m_pName[16];			// object name

	//--------------------------------------------------------------------------
	// object state
	WORD			m_wState;
	BOOL			m_bShow;
	BOOL			m_bActive;	
	
	//--------------------------------------------------------------------------
	// draw info
	SPRITE_INFO		m_spriteInfo;			// sprite	
	DWORD			m_dwARGB;
	
	//--------------------------------------------------------------------------
	/// event observer
	IMouseObserver			*	m_pMouseObserver;
	IMouseMotionObserver	*	m_pMouseMotionObserver;
	IKeyObserver			*	m_pKeyObserver;

protected:
	BOOL			m_bEnableFocus;			// focus가 가능한가

// < Member Function > -----------------------------------------------------
public:
	CGUIObject();
	virtual ~CGUIObject();
	
			void	SetName( char * pi_pName );
	inline	char *	GetName( void )									{ return m_pName; }	
	
	virtual	void	SetPos( POINT & pi_ptPos );
	virtual	void	SetPos( LONG pi_nPosX, LONG pi_nPosY );
	inline	void	GetPos( POINT & po_ptPos )						{ po_ptPos.x = m_ptPos.x; po_ptPos.y = m_ptPos.y; }
	inline	LONG	GetPosX( void ) { return m_ptPos.x; }
	inline	LONG	GetPosY( void ) { return m_ptPos.y; }

			void	SetSize( POINT & pi_ptSize );
			void	SetSize( LONG pi_nWidth, LONG pi_nHeight );
	inline	void	GetSize( POINT & po_size )						{ po_size.x = m_ptSize.x; po_size.y = m_ptSize.y; }
	inline	LONG	GetSizeX( void )								{ return m_ptSize.x; }
	inline	LONG	GetSizeY( void )								{ return m_ptSize.y; }
	
	virtual	void	SetBound( RECT & pi_rtBound );
			void	GetBound( RECT & po_rtBound );

	//---------------------------------------------------------------------------------
	virtual	void	SetSpriteInfo( SPRITE_INFO * pi_pSpriteInfo );
	inline	SPRITE_INFO *	GetSpriteInfo( void )					{ return &m_spriteInfo; }
			void	RemoveSpriteInfo( void );

	virtual	void	SetSprite()										{;}

	virtual	void	Update( void );

	virtual BOOL	Draw( void );
	virtual	BOOL	Draw( RECT & pi_rtClippingArea );

	inline	void	SetARGB( DWORD pi_dwARGB )						{ m_dwARGB = pi_dwARGB; }
	inline	DWORD	GetARGB( void )									{ return m_dwARGB; }
	
	virtual	void	Show( BOOL pi_bShow )							{ m_bShow = pi_bShow; }
	inline	BOOL	IsShow( void )									{ return m_bShow; }	

	//---------------------------------------------------------------------------------
	// event 관련 함수
	virtual	BOOL	InputProc( void );	// mouse event
	virtual	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // key event	
	
	virtual	BOOL	IsCrossed( LONG pi_nXPos, LONG pi_nYPos );

	virtual	void	Enable( void );		
	virtual	void	Disable( void );

	virtual	void	Active( void );
	virtual	void	InActive( void );
	inline	BOOL	IsActive( void )								{ return m_bActive; }

	virtual	void	SetFocus( void )								{ if( m_bEnableFocus && !IS_DISABLED( m_wState ) ) s_pFocusedObject = this; }

	inline	void	AddMouseEventHandler( IMouseObserver *pi_pObj ) { m_pMouseObserver = pi_pObj; }
	inline	void	AddMouseMotionEventHandler( IMouseMotionObserver *pi_pObj ) { m_pMouseMotionObserver = pi_pObj; }
	inline	void	AddKeyEventHandler( IKeyObserver *pi_pObj )		{ m_pKeyObserver = pi_pObj; }

	inline	void	RemoveMouseEventHandler( void )					{ m_pMouseObserver = NULL; }
	inline	void	RemoveMouseMotionEventHandler( void )			{ m_pMouseMotionObserver = NULL; }
	inline	void	RemoveKeyEventHandler( void )					{ m_pKeyObserver = NULL; }

	virtual	void	EnableFocus( void );
	virtual	void	DisableFocus( void );

	//---------------------------------------------------------------------------------
	inline	WORD	GetState( void ) { return m_wState; }
	
protected:
	virtual	void	SetState( WORD pi_wState )						{ m_wState = ( m_wState & DISABLED ) | pi_wState; }
			
			void	SendMouseEvent( WORD pi_wID );
			void	SendMouseMotionEvent( WORD pi_wID );

	//---------------------------------------------------------------------------------
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUIObjectEx ]

  - CGUIObject의 확장 클래스
  - Draw방식만 다르다. 이미지를 clipping해서 사용.

//////////////////////////////////////////////////////////////////////////*/
class CGUIObjectEx : public CGUIObject
{
protected:
// < Data Member > ---------------------------------------------------------
	float		m_fSpriteUV[2][2];

	float		m_fRotate;

// < Member Function > -----------------------------------------------------
public:
	CGUIObjectEx();
	virtual ~CGUIObjectEx();

			void	SetSpriteUV( float pi_fUV[2][2] );			
			void	SetSpriteU( float pi_fU0, float pi_fU1 );	// range : 0.0f ~ 1.0f
			void	SetSpriteV( float pi_fV0, float pi_fV1 );

			void	SetSpriteTileByIndex( DWORD pi_dwIndex );

			float*	GetU( void )								{ return (float *)m_fSpriteUV[0]; }
			float*	GetV( void )								{ return (float *)m_fSpriteUV[1]; }			

	inline	void	SetRotate( float pi_fAngle )				{ m_fRotate = pi_fAngle; }
	inline	float	GetRotate( void )							{ return m_fRotate; }

	virtual	BOOL	Draw( void );
	virtual	BOOL	Draw( RECT & pi_rtClipping );
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUIObjectAni ]

  - CGUIObjectEx의 확장 클래스
  - animation할 때 사용

//////////////////////////////////////////////////////////////////////////*/
class CGUIObjectAni : public CGUIObjectEx
{
private:
// < Data Member > ---------------------------------------------------------
	float		m_fCurFrameNo;		// 현재 frame
	WORD		m_wTotalFrameNo;	// total frame

	WORD		m_wFPS;				// 초당 프레임수

	WORD		m_wObjNumInLine;	// sprite에서 가로로 한줄에 object의 갯수

	//----------------------------------------------------------------------
	// animation 멈출 때 
	BOOL		m_bStop;
	DWORD		m_dwStopInterval;
	DWORD		m_dwStopTime;
	
	float		m_fUVSize[2];		// 텍스쳐상에서 guiobject의 크기	
	
// < Member Function > -----------------------------------------------------
public:
	CGUIObjectAni();
	virtual ~CGUIObjectAni();

	inline	void	SetObjectNumInLine( WORD pi_wNum )		{ m_wObjNumInLine = pi_wNum; }
	inline	void	SetTotalFrameNum( WORD pi_wNum )		{ m_wTotalFrameNo = pi_wNum; }
	inline	void	SetFPS( WORD pi_wFPS )					{ m_wFPS = pi_wFPS; }
	inline	void	SetStopInterval( DWORD pi_dwInterval )	{ m_dwStopInterval = pi_dwInterval; }

			void	SetSpriteInfo( SPRITE_INFO * pi_pSpriteInfo );

	virtual	void	Update( void );
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUITileObject ]
 
   - 타일 방식으로 그린다.

//////////////////////////////////////////////////////////////////////////*/
class CGUITileObject : public CGUIObject
{
// < Data Member > ---------------------------------------------------------
private:
	WORD		m_wTileNum[2];
	WORD		m_wGap[2];
	
	POINT		m_ptTileSize;

// < Member Function > -----------------------------------------------------
public:
	CGUITileObject();
	~CGUITileObject();

			void	SetSpriteInfo( SPRITE_INFO * pi_pSpriteInfo );
			
			void	SetGap( int pi_nWGap, int pi_nHGap );
	inline	WORD	GetGapW( void )										{ return m_wGap[0]; }
	inline	WORD	GetGapH( void )										{ return m_wGap[1]; }

			void	SetTileSize( LONG pi_nWidth, LONG pi_nHeight );
	inline	void	GetTileSize( POINT & po_ptSize )					{ po_ptSize = m_ptTileSize; }

			void	SetSize( POINT & pi_ptSize );
			void	SetSize( LONG pi_nWidth, LONG pi_nHeight );		

	virtual	BOOL	Draw( void );
	virtual	BOOL	Draw( RECT & pi_rtClipping );

private:
			void	ResetTileNum();
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUITimer ]
 
   - 정해진 시간동안 타이머 애니메이션 한다.

//////////////////////////////////////////////////////////////////////////*/
class CGUITimer : public CGUIObject
{
private :
	DWORD		m_dwDelay;
	DWORD		m_dwStartTime;

	BOOL		m_bIsStarted;

	int			m_nDelayAngle;

	_D3DR3TLVERTEX_TEX1	m_vVertex[3];

public :

	CGUITimer();
	virtual ~CGUITimer();

	inline	void	SetDelay( DWORD pi_dwDelay )	{ m_dwDelay = pi_dwDelay; }
	inline	DWORD	GetDelay( void )				{ return m_dwDelay; }
			
			void	SetColor( DWORD pi_dwColor );

			void	Start( void );
	inline	BOOL	IsStarted( void )				{ return m_bIsStarted; }

	virtual	BOOL	Draw( void );
	virtual	BOOL	Draw( RECT & pi_rtClipping );
};

#endif // __GUIOBJECT_H__