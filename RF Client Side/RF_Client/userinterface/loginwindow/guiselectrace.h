// GUISelectRace.h: interface for the CGUISelectRace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUISELECTRACE_H__7A2C05F6_6439_4300_BCE0_5A9CCF8848F4__INCLUDED_)
#define AFX_GUISELECTRACE_H__7A2C05F6_6439_4300_BCE0_5A9CCF8848F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIButton.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUISelectRace ]

//////////////////////////////////////////////////////////////////////////*/
class CGUISelectRace : public CGUIWindow
{
public:
	enum Race
	{
		Bellato,
		Cora,
		Accretia,		
		None
	};

private:	
	// -------------------------------------------------------
	// race select board
	CGUIContainer		m_cUpperBoard;
	CGUIObject			m_cUpperBase;
	CGUIObjectAni		m_cUpperBoardAni;

	CGUIButtonEx		m_cRaceSelectButton[3];
	CGUIObject			m_cRaceProfile;

	// -------------------------------------------------------
	// menu board
	CGUIContainer		m_cLowerBoard;
	CGUIObject			m_cLowerBase;	

	CGUIButtonEx		m_cCreateButton;	
	CGUIButtonEx		m_cCancelButton;
	

	Race				m_eCurSelectedRace;

	BOOL				m_bIsChanging;
	DWORD				m_dwChagingTime;

	BOOL				m_bIsCreated;	

public:
	CGUISelectRace();
	~CGUISelectRace();

	void	Init( void );
	void	SetSprite( void );

	void	ChangeRace( Race pi_eRace );

	void	MouseEvent( _mouse_event & event );

	void	Update( void );

	CGUIContainer *		GetUpperBoard( void ) { return &m_cUpperBoard; }
	CGUIContainer *		GetLowerBoard( void ) { return &m_cLowerBoard; }
};

#endif // !defined(AFX_GUISELECTRACE_H__7A2C05F6_6439_4300_BCE0_5A9CCF8848F4__INCLUDED_)
