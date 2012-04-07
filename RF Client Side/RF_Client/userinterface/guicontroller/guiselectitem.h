////////////////////////////////////////////////////////////////////////////
//
// CGUISelectItem Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUISELECTITEM_H__
#define __GUISELECTITEM_H__

#pragma warning( disable : 4786 )

#include "GUIContainer.h"
#include "GUIButton.h"
#include "GUIText.h"
#include <list>

/*//////////////////////////////////////////////////////////////////////////

[ CGUISelectItem ]

//////////////////////////////////////////////////////////////////////////*/
class CGUISelectItem : public CGUIContainer,
					   public IMouseObserver
						
{
public:
	typedef	list<  char * >		CGUIItemStrList;
// < Data Member > ---------------------------------------------------------
private:
	CGUIButton		m_cUpButton;
	CGUIButton		m_cDownButton;
	CGUIObject		m_cItemBoard;

	CGUIItemStrList				m_listItemStr;
	CGUIItemStrList::iterator	m_itCurSelectedItem;

	COLORREF				m_rgbItemColor;

	ISelectItemObserver	*	m_pSelectItemObserver;
	// item이 바뀌었을 때만 event msg를 보낼 것인가..
	BOOL					m_bHandleItemChange;

// < Member Function > -----------------------------------------------------
public:
	CGUISelectItem();
	~CGUISelectItem();

	void	Init( POINT pi_ptSelectButtonSize, int ItemBoardWidth );

	void	SetSprite( CSprite * pi_pSprite );

	void	SetUpButtonSpriteInfo( SPRITE_INFO * pi_pSpriteInfo );
	void	SetDownButtonSpriteInfo( SPRITE_INFO * pi_pSpriteInfo );
	void	SetItemBoardSpriteInfo( SPRITE_INFO * pi_pSpriteInfo );

	void	SetItemBoardWidth( int pi_nWidth );
	void	SetBound( RECT & pi_bound );	

	char *	GetItem( WORD pi_wIndex );
	char *	GetCurrentItem( void );

	void	SetCurrentItem( WORD pi_wIndex );

	void	InsertItem( char * pi_pItemStr );
	void	DeleteItem( char * pi_pItemStr );

	void	Clear( void );

	void	SetItemStrColor( COLORREF pi_rgbColor ) { m_rgbItemColor = pi_rgbColor; }
	BOOL	Draw( void );

	void	MouseEvent( _mouse_event & event);

	void	AddSelectItemHandler( ISelectItemObserver * l_pObserver ) { m_pSelectItemObserver = l_pObserver; }
	void	SetHandleItemChange( BOOL pi_bHandle ) { m_bHandleItemChange = pi_bHandle; }

private:
	BOOL	SelectUp( void );
	BOOL	SelectDown( void );

	void	SendSelectedEvent( WORD pi_wSelecedIndex );
};

#endif // __GUISELECTITEM_H__
