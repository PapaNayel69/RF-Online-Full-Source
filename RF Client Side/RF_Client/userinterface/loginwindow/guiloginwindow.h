// GUILoginWindow.h: interface for the CGUILoginWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUILOGINWINDOW_H__E8C7FCA3_039E_47C0_A8A7_7102C83F8D82__INCLUDED_)
#define AFX_GUILOGINWINDOW_H__E8C7FCA3_039E_47C0_A8A7_7102C83F8D82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIButton.h"
#include "../GUIController/GUIInput.h"
#include "../GUIController/GUIDialogBox.h"

class CGUIIDCreateDlg;

/*//////////////////////////////////////////////////////////////////////////

[ CGUILoginWindow ]

//////////////////////////////////////////////////////////////////////////*/
class CGUILoginWindow : public CGUIWindow
{
friend class CGUIIDCreateDlg;
private:
	CGUIContainer		m_cBase;
	CGUIContainer		m_cBoard;

	CGUIInput			m_cIDInput;
	CGUIInput			m_cPasswordInput;

	CGUIButton			m_cCreateButton;
	CGUIButton			m_cJoinButton;

	CGUIIDCreateDlg	*	m_pIDCreateDlg;

	BOOL				m_bCreated;

public:
	CGUILoginWindow();
	~CGUILoginWindow();

			void	Init( void );

			void	SetSprite( void );

			void	MouseEvent( _mouse_event & event );	
			void	KeyEvent( _key_event & event );
			void	ClosingDialogBox( CGUIDialogBox * pi_pDlg );

	
	inline	char *	GetID( void ) { return m_cIDInput.GetText(); }
	inline	char *	GetPassword( void ) { return m_cPasswordInput.GetText(); }
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUIIDCreateDlg ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIIDCreateDlg : public CGUIDialogBox
{
public:
	CGUIContainer	m_cBoard;
	
	CGUIInput		m_cIDInput;
	CGUIInput		m_cPasswordInput;
	CGUIInput		m_cRePasswordInput;

	CGUILoginWindow * m_pLoginWindow;

	BOOL			m_bCreated;

public:
	CGUIIDCreateDlg( CGUILoginWindow * pi_pWin );
	~CGUIIDCreateDlg();	

			void	Init( void );
			void	SetSprite( void );

			void	Close( void );

			void	MouseEvent( _mouse_event & event );
			void	KeyEvent( _key_event & event );	
			LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	inline	char *	GetID( void ) { return m_cIDInput.GetText(); }
	inline	char *	GetPassword( void ) { return m_cPasswordInput.GetText(); }
	inline	char *	GetRePassword( void ) { return m_cRePasswordInput.GetText(); }
	inline	void	ClearID( void ) { m_cIDInput.Clear(); }
	inline	void	ClearPassword( void ) { m_cPasswordInput.Clear(); m_cRePasswordInput.Clear(); }
};

#endif // !defined(AFX_GUILOGINWINDOW_H__E8C7FCA3_039E_47C0_A8A7_7102C83F8D82__INCLUDED_)
