// GUISelectCharacter.h: interface for the CGUISelectCharacter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUISELECTCHARACTER_H__FEC0DBFF_2534_4016_AF34_64A2C694A0A0__INCLUDED_)
#define AFX_GUISELECTCHARACTER_H__FEC0DBFF_2534_4016_AF34_64A2C694A0A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIButton.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUISelectCharacter ]

//////////////////////////////////////////////////////////////////////////*/
class CGUISelectCharacter : public CGUIWindow
{
private:
	// ----------------------------------------------
	// character info board
	CGUIContainer	m_cUpperBoard;
	CGUIObject		m_cUpperBase;
	CGUIObjectAni	m_cUpperBoardAni;

	CGUIObject		m_cLCharInfoBoard;
	CGUIObject		m_cRCharInfoBoard;

	CGUIText		m_cNameStr;
	CGUIText		m_cLevelStr;	
	CGUIText		m_cAttributeStr;
	CGUIText		m_cDalantStr;
	CGUIText		m_cGoldStr;

	// ----------------------------------------------
	// menu board
	CGUIContainer	m_cLowerBoard;
	CGUIObject		m_cLowerBase;
	CGUIObjectAni	m_cLowerBoardAni;

	CGUIButtonEx	m_cPrevButton;	
	CGUIButtonEx	m_cNextButton;		

	CGUIButtonEx	m_cCreateButton;
	CGUIButtonEx	m_cDeleteButton;

	CGUIButton		m_cConnectButton;
	CGUIButtonEx	m_cExitButton;


	BOOL			m_bIsCreated;	

public:
	CGUISelectCharacter();
	~CGUISelectCharacter();

	void	Init( void );
	void	SetSprite( void );			

	void	MouseEvent( _mouse_event & event );
	void	ClosingDialogBox( CGUIDialogBox * pi_pDlg );

	void	SetCharInfo_Name( char * pi_pName );	
	void	SetCharInfo_Level( char * pi_pLevel );
	void	SetCharInfo_Attribute( BYTE pi_byAttrType ); 
	void	SetCharInfo_Money( DWORD pi_dwDalant, DWORD pi_dwGold, BYTE pi_byRaceType );
	
	void	SetCharInfo_Race( char * pi_pRace ){;}
	void	SetCharInfo_Sex( char * pi_pSex ){;}
	
	void	EnableCreate( BOOL pi_bEnable );
	void	EnableDelete( BOOL pi_bEnable );

	void	SetEmptyInfoBoard( void );

	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	CGUIContainer * GetLowerBoard( void ) { return &m_cLowerBoard; }
	CGUIContainer * GetUpperBoard( void ) { return &m_cUpperBoard; }
};

#endif // !defined(AFX_GUISELECTCHARACTER_H__FEC0DBFF_2534_4016_AF34_64A2C694A0A0__INCLUDED_)
