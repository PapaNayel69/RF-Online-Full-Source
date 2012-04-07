////////////////////////////////////////////////////////////////////////////
//
// CGUIDialogBox Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIDIALOGBOX_H__
#define __GUIDIALOGBOX_H__

#include "GUIWindow.h"
#include "GUIButton.h"

#define		MSG_OK			0x1
#define		MSG_CANCEL		0x2
#define		MSG_CLOSE		0x3
#define		MSG_NONE		0x4

#define		DLG_STYLE_OK_CANCEL	0x3
#define		DLG_STYLE_OK		0x4
#define		DLG_STYLE_NONE		0x5

#define		NOT_DEFINE_ID		0

void	_CloseDialogBox( CGUIWindow * pi_pOwnerWindow = NULL,
						 WORD pi_wUserDefinedID = NOT_DEFINE_ID );

void	_CloseAllDialogBox( CGUIWindow * pi_pOwnerWindow = NULL );

BOOL	_IsOpenedDialogBox( CGUIWindow * pi_pOwnerWindow = NULL,
						 WORD pi_wUserDefinedID = NOT_DEFINE_ID );


/*//////////////////////////////////////////////////////////////////////////

[ CGUIDialogBox ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIDialogBox : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
protected:	

	CGUIWindow	*	m_pOwnerWindow;

	WORD			m_wUserDefinedID;
	
	WORD			m_wDlgStyleID;	

	BOOL			m_bModal;
	BOOL			m_bLockOtherWin;	// dialogbox가 열려있는동안 다른 윈도우에 lock을 걸어놓는다.

	BOOL			m_bOpened;

	BOOL			m_bDestroy;			// close되었을 때 메모리를 해제할 것인지 결정한다.

	CGUIButtonEx	m_uiOKButton;
	CGUIButtonEx	m_uiCancelButton;
	CGUIButton		m_uiCloseButton;

	WORD			m_wResultMsg;
									

// < Member Function > -----------------------------------------------------
public:
	CGUIDialogBox();
	CGUIDialogBox( CGUIWindow * pi_pOwnerWindow, WORD pi_wType, BOOL pi_bModal, WORD pi_wUserDefinedID );
	virtual ~CGUIDialogBox();

			void	SetOwnerWindow( CGUIWindow * pi_pWin );
	inline	CGUIWindow *	GetOwnerWindow( void ) { return m_pOwnerWindow; }
			void	RemoveOwnerWindow( void );
			
	inline	void	SetUserDefinedID( WORD pi_wID ) { m_wUserDefinedID = pi_wID; }
	inline	WORD	GetUserDefinedID( void ) { return m_wUserDefinedID; }

	inline	WORD	GetStyleID( void ) { return m_wDlgStyleID; }
	
	inline	void	SetModal( BOOL pi_bModal ) { m_bModal = pi_bModal; }
	inline	BOOL	IsModal( void ) { return m_bModal; }

			void	LockOtherWin( BOOL pi_bLock );
	inline	BOOL	IsLockOtherWin( void ) { return m_bLockOtherWin; }
	
	// ---------------------------------------------------------------------------------
	virtual	void	Open( void );
	virtual	void	Close( void );

			void	ActiveWindow( CGUIWindow * pi_pWin );
			void	InActiveWindow( CGUIWindow * pi_pWin );

			BOOL	IsOpen( void ) { return m_bOpened; }

			WORD	GetResultMsg( void ) { return m_wResultMsg; }
	inline	void	SetResultMsg( WORD pi_ResultMsg ) { m_wResultMsg = pi_ResultMsg; }

			void	SetDestroy( BOOL pi_bDestroy ) { m_bDestroy = pi_bDestroy; }
			BOOL	IsDestroy( void ) { return m_bDestroy; }
			BOOL	IsOpened( void ) { return m_bOpened; }

	virtual	void	MouseEvent( _mouse_event & event );

	virtual	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

			void	SendDialogBoxEvent( WORD pi_wID );
};

#endif // __GUIDIALOGBOX_H__
