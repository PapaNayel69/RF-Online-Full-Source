////////////////////////////////////////////////////////////////////////////
//
// CGUIRadioButton Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef ___GUIRADIOBUTTON_H__
#define ___GUIRADIOBUTTON_H__

#include "GUIContainer.h"
#include "GUIText.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUIRadioButton ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIRadioButton : public CGUIContainer						
{
friend class CGUIRadioButtonCtrl;
// < Data Member > ---------------------------------------------------------
private:
	CGUIObject	m_cButton;
	CGUIText	m_cText;

	WORD		m_wIndex;
	
	BOOL		m_bSelected;

	SPRITE_INFO	m_sSelectedSprite;
	
// < Member Function > -----------------------------------------------------
public:
	CGUIRadioButton();
	virtual ~CGUIRadioButton();
	
	// ---------------------------------------------------------------------
	void	SetSize( LONG pi_nWidth, LONG pi_nHeight );
	void	SetSize( POINT & pi_ptSize );

	void	SetButtonSize( int pi_nSizeX, int pi_nSizeY );
	void	SetString( char * pi_pStr );	
	
	void	SetButtonSprite( SPRITE_INFO	* pi_pSpriteInfo );	
	void	SetSelectedSprite( SPRITE_INFO	* pi_pSpriteInfo );	

	CGUIObject	*	GetButton( void ) { return &m_cButton; }
	CGUIText	*	GetText( void ) { return &m_cText; }

	// ---------------------------------------------------------------------
	void	Select( void ) { m_bSelected = true; }
	void	UnSelect( void ) { m_bSelected = false; }
	BOOL	IsSelected( void ) { return m_bSelected; }	

	// ---------------------------------------------------------------------
	BOOL	Draw( void );

private:
	void	Reset( void );
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUIRadioButtonCtrl ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIRadioButtonCtrl : public IMouseObserver
{
public:
	typedef list< CGUIRadioButton * >		CGUButtonList;
// < Data Member > ---------------------------------------------------------
private:
	CGUButtonList	m_listButton;
	
	BOOL			m_bMultiSelect;			// 다중선택
	BOOL			m_bEnableUnSelect;		// Unselect가 가능한가

	ISelectItemObserver	*	m_pSelectItemObserver;		

// < Member Function > -----------------------------------------------------
public:
	CGUIRadioButtonCtrl();
	~CGUIRadioButtonCtrl();

	// ---------------------------------------------------------------------
	void	AddButton( CGUIRadioButton * pi_pButton );

	void	SetMultiSelect( BOOL pi_bMultiSelect ) { m_bMultiSelect = pi_bMultiSelect; }
	void	SetEnableUnSelect( BOOL pi_bUnSelect ) { m_bEnableUnSelect = pi_bUnSelect; }

	void	AddSelectItemHandler( ISelectItemObserver * l_pObserver ) { m_pSelectItemObserver = l_pObserver; }

	// ---------------------------------------------------------------------
	BOOL	IsSelected( WORD pi_wIndex );
	void	Select( WORD pi_wIndex );
	void	UnSelect( WORD pi_wIndex );
	void	UnSelectAll( void );

	// ---------------------------------------------------------------------
	void	MouseEvent( _mouse_event & event );

private:
	CGUIRadioButton * GetSource( CGUIObject * pi_pSrc );
};

#endif // ___GUIRADIOBUTTON_H__
