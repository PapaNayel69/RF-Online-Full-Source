////////////////////////////////////////////////////////////////////////////
//
// CGUINumInputDlg Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUINUMINPUTDLG_H__
#define __GUINUMINPUTDLG_H__

#include "GUIDialogBox.h"
#include "GUIInput.h"
#include "GUIButton.h"

void _GUINumInputDlg( CGUIWindow * pi_pOwnerWindow,
					  POINT pi_ptEventPos,
					  int pi_nMaxNumber,
					  WORD pi_wUserDefinedID = NOT_DEFINE_ID,
					  int pi_nInitNumber = 0
					  );

/*//////////////////////////////////////////////////////////////////////////

[ CGUINumInputDlg ]

//////////////////////////////////////////////////////////////////////////*/
class CGUINumInputDlg : public CGUIDialogBox
{
// < Data Member > ---------------------------------------------------------
private:	
	CGUIInput		m_cInput;

	CGUIObject		m_cInputStr;

	CGUIButton		m_cUpButton;
	CGUIButton		m_cDownButton;

	DWORD			m_dwMaxNumber;

	BOOL			m_bIsFirstInput;
	DWORD			m_dwInitNumber;

	DWORD			m_dwPressedTime;	// up, down button pressed time

// < Member Function > -----------------------------------------------------
public:
	CGUINumInputDlg();
	CGUINumInputDlg( CGUIWindow * pi_pOwnerWindow, BOOL pi_bModal, WORD pi_wUserDefinedID=NOT_DEFINE_ID );
	virtual ~CGUINumInputDlg();

	void	Init( void );
	void	SetSprite( void );

	char*	GetInput( void ) { return m_cInput.GetText(); }

	void	SetNumber( DWORD pi_dwNumber );
	void	SetMaxNumber( DWORD pi_dwMaxNumber );

	void	InitInputState( void );

	void	Clear( void );

	void	Active( void );	

	void	MouseEvent( _mouse_event & event );

	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif // __GUINUMINPUTDLG_H__
