////////////////////////////////////////////////////////////////////////////
//
// CGUIWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIWINDOW_H__
#define __GUIWINDOW_H__

#include "GUIContainer.h"
#include "../../DefineMessage.h"
#include "GUIItem.h"

#define	NONE_MOTION		0
#define	RESIZE_MOTION	1
#define	MOVE_MOTION		2

class CGUIDialogBox;
class CGUIScrollbar;

/*//////////////////////////////////////////////////////////////////////////

[ CGUIWindow ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIWindow : public CGUIContainer,
				   public IMouseObserver,
				   public IMouseMotionObserver,
				   public IKeyObserver
{
public:
	typedef list< CGUIDialogBox * >		CGUIDialogBoxList;
	typedef list< CGUIWindow * >		CGUIWindowList;

	enum Border {
		LEFT,
		TOP,
		RIGHT,
		BOTTOM,
		NONE_BORDER
	};

// < Data Member > ---------------------------------------------------------
protected:
	DWORD			m_dwID;	

	CGUIWindow	*	m_pParent;	

	CGUIObject	*	m_pResizeObject[4];	
	BOOL			m_bExistResizeObj;	
	
	CGUIObjectList	m_listMoveObject;

	BYTE			m_byCurMotion;	

	// input을 따로 처리하
	CGUIDialogBoxList	m_listDialogBox;
	
	POINT			m_ptPressPos;

private:	
	BOOL			m_bAlwaysTop;				// 항상 최상위 layer에 있게..
	BOOL			m_bAlwaysActive;			// 항상 active되어야할 때 사용

	BOOL			m_bReSizable;				// size 조절이 가능한가..
	BOOL			m_bMovable;					// 이동이 가능한가..
	
	CGUIObject	*	m_pFocusObject;				// window가 활성화 되었을 때 focus될 object

	
	CGUIScrollbar *	m_pWheelScrollObject;		// wheel mouse를 사용했을 때 scroll event를 보내줄 object


	// docking
	BOOL			m_bEnableDocking;
	CGUIWindowList	m_listDockingCustomer;		// 도킹된 윈도우 리스트
	BOOL			m_bEnableDockingBorder[4];	// 도킹 가능한 border는 true로 세팅

	CGUIWindow	*	m_pDockingHost;				// 자신이 도킹된 상태일 때의 호스트 윈도우
	Border			m_eDockingBorder;			// 도킹된 호스트의 경계
	RECT			m_rtDockingBound;			// 도킹된 host와 customer를 합한 영역

	BOOL			m_bHostToCustomerDocking;


	// sound flag
	BOOL			m_bSoundOn;

// < Member Function > -----------------------------------------------------
public:
	CGUIWindow();
	virtual ~CGUIWindow();

	virtual	void	Init( void ) { ; }

	//--------------------------------------------------------------------------
	// restore sprite
	virtual	void	RestoreSprite( void );
	

	//--------------------------------------------------------------------------
	inline	void		SetParent( CGUIWindow * pi_pParent ) { m_pParent = pi_pParent; }
	inline	CGUIWindow* GetParent( void ) { return m_pParent; }
	
	inline	void	SetID( DWORD pi_dwID ) { m_dwID = pi_dwID; }
	inline	DWORD	GetID( void ) { return m_dwID; }

	//--------------------------------------------------------------------------
	inline	BOOL	IsAlwaysTop() { return m_bAlwaysTop; }
			void	SetAlwaysTop( BOOL pi_bAlwaysTop );
	
	inline	BOOL	IsAlwaysActive( void ) { return m_bAlwaysActive; }
			void	SetAlwaysActive( BOOL pi_bFlag );


	//--------------------------------------------------------------------------
	// 생성하고 삭제하는게 아니라 화면에 보여주고 활성화시키는 것만 한다.
	virtual	void	Open( void );
	virtual	void	Close( void );

	virtual	void	Active( void );
	virtual	void	InActive( void );			

			BOOL	IsActiveWindow( void );
			BOOL	IsCrossedWindow( LONG pi_nXPos, LONG pi_nYPos );

	virtual	void	ActiveWindow( CGUIWindow * pi_pWin );
	virtual	void	InActiveWindow( CGUIWindow * pi_pWin );	
	
	virtual	void	SetPos( LONG pi_nPosX, LONG pi_nPosY );
	virtual	void	SetPos( POINT pi_ptPos );

			void	SetSize(  LONG pi_nWidth, LONG pi_nHeight );
			void	SetSize( POINT & pi_ptSize );


	inline	void	SoundOn( BOOL pi_bSoundOnFlag ) { m_bSoundOn = pi_bSoundOnFlag; }

	//--------------------------------------------------------------------------	
	inline	void			SetFocusObject( CGUIObject * pi_pObj ) { m_pFocusObject = pi_pObj; }
	inline	CGUIObject *	GetFocusObject( void ) { return m_pFocusObject; }

	//--------------------------------------------------------------------------
	inline	void	SetWheelScrollObject( CGUIScrollbar * pi_pScrollbar ) { m_pWheelScrollObject = pi_pScrollbar; }
	inline	CGUIScrollbar * GetWheelScrollObject( void ) { return m_pWheelScrollObject; }


	//--------------------------------------------------------------------------
	// dialog box
	virtual	void	AddDialogBox( CGUIDialogBox * pi_pDialogBox, BOOL pi_bModal );
	virtual	void	RemoveDialogBox( CGUIDialogBox * pi_pDialogBox );
	CGUIDialogBox * GetLastDialogBox( void );
	inline	CGUIDialogBoxList * GetDialogBoxList( void ) { return &m_listDialogBox; }
	inline	BOOL	ExistDialogBox( void ) { return !m_listDialogBox.empty(); }
	
	
	//--------------------------------------------------------------------------
	// Window Move, Resize
			void	SetReSizable( BOOL pi_bResizable );
			void	SetMovable( BOOL pi_bMovable );

			void	SetReSizeObject( Border pi_eBorder, CGUIObject * pi_pResizeObj );
			void	SetMoveObject( CGUIObject * pi_pMoveObj );			

	// docking window
			void	AddDockingCustomer( CGUIWindow * pi_pWindow );
			void	RemoveDockingCustomer( CGUIWindow * pi_pWindow );

			void	EnableDockingBorder( Border pi_eBorder, BOOL pi_bEnable );
	inline	BOOL	IsEnabledDockingBorder( Border pi_eBorder ) { return m_bEnableDockingBorder[pi_eBorder]; }
			void	EnableDocking( BOOL pi_bEnable );

	inline	void			SetDockingHost( CGUIWindow * pi_pWindow ) { m_pDockingHost = pi_pWindow; }
	inline	CGUIWindow *	GetDockingHost( void ) { return m_pDockingHost; }			
	
			void	SetDockingBorder( Border pi_eBorder );
	inline	Border	GetDockingBorder( void ) { return m_eDockingBorder; }
	
	inline	CGUIWindowList*	GetDockingList( void ) { return &m_listDockingCustomer; }
	inline	RECT*			GetDockingBound( void ) { return &m_rtDockingBound; }

private:
			BOOL	DetectedDocking( POINT & pi_ptPos );
			Border	DetectedDockingBorder( CGUIWindow * pi_pHostWin, CGUIWindow * pi_pCustomerWin, POINT & pi_ptCustMovePos );
			void	ResetDockingBound( void );


public:
	//--------------------------------------------------------------------------
	// event
	virtual	void	MouseEvent( _mouse_event & event );
	virtual	void	MouseMotionEvent( _mouse_motion_event & event );
	virtual	void	KeyEvent( _key_event & event );	

	// mouse로 object를 잡거나 떨어뜨릴 때 일어나는 event
	virtual	void	PickingObjectEvent( BOOL pi_bPick );

	virtual	void	Update( void );
			
			BOOL	Draw( void );
			void	DrawDialogBox( void );		

	virtual	BOOL	InputProc( void );
	virtual	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	virtual	void	ClosingDialogBox( CGUIDialogBox * pi_pDlg );

			void	SendWindowEvent( WORD pi_wID );

protected :

			BOOL	PostUIMessage( UINT pi_uMsg, UI_MOUSE_MSG & pi_stMouseMsg );
			BOOL	PostUIMessage( UINT pi_uMsg, LPARAM pi_lParam );
};



/*//////////////////////////////////////////////////////////////////////////

[ CGUIItemWindow ]
 
   - 아이템이 들어가는 윈도우에 아이템 관리하는 인터페이스를 위해 만들었다.

//////////////////////////////////////////////////////////////////////////*/

class CGUIItemWindow : public CGUIWindow
{

// < Member Function > -----------------------------------------------------
public:
	CGUIItemWindow() {;}
	virtual	~CGUIItemWindow() {;}


	virtual	BOOL		TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex,
								    DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel = NONE_LEVEL ) = 0;

	virtual	BOOL		TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum =0 , CGUIItem * po_pOutItem = NULL ) = 0;

	virtual	void		SetTradeItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bIsTradeItem ){;}
	
	virtual	CGUIItem *	GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex ) = 0;

	virtual	void		SetUselessSocket( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bFlag ){;}
};

#endif // __GUIWINDOW_H__