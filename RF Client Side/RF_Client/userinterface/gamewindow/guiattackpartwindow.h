////////////////////////////////////////////////////////////////////////////
//
// CGUIAttackPartWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef ___GUIATTAKPARTWINDOW_H__
#define ___GUIATTAKPARTWINDOW_H__

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIButton.h"
#include "../GUIController/GUIText.h"
#include "../GUIController/GUIRadioButton.h"


/*//////////////////////////////////////////////////////////////////////////

[ CGUIAttackPartWindow ]
 
   - 타격 부위 선택창

//////////////////////////////////////////////////////////////////////////*/

class CGUIAttackPartWindow : public CGUIWindow,
							 public ISelectItemObserver
{
// < Data Member > ---------------------------------------------------------
public:

	CGUIContainer		m_cBoard;
	
	CGUIText			m_cAttackCharStr;	

	BYTE				m_byWeakPart;	

	CGUIRadioButtonCtrl	m_cButtonCtrl;
	CGUIRadioButton		m_cAttackPartButton[5];
	

	CGUIButton			m_cCloseButton;

	BOOL				m_bCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUIAttackPartWindow();
	virtual ~CGUIAttackPartWindow();

	void	Init( void );
	void	SetSprite( void );

	// ----------------------------------------------------------------------
	void	SetAttackCharName( char * pi_pAttChar );	// 공격 대상 설정	

	void	SetWeakPart( BYTE pi_byWeakPart );			// 취약부위 설정
	void	ClearWeakPart( void );						// 취약부위 설정 해제

	// ----------------------------------------------------------------------
	void	MouseEvent( _mouse_event & event );

	void	SelectItemEvent( _select_event & event );
};

#endif // ___GUIATTAKPARTWINDOW_H__
