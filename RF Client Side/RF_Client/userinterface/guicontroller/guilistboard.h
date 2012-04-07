////////////////////////////////////////////////////////////////////////////
//
// CGUIListBoard Class Header
//
////////////////////////////////////////////////////////////////////////////
#ifndef __GUILISTBOARD_H__
#define __GUILISTBOARD_H__

#include "GUIContainer.h"
#include "GUIScrollbar.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUIListBoard ] 

//////////////////////////////////////////////////////////////////////////*/
class CGUIListBoard : public CGUIContainerEx, IScrollObserver
{
// < Data Member > ---------------------------------------------------------
private:
	CGUIContainer		m_uiBoard;
	CGUIScrollbar		m_uiScrollbar;


// < Member Function > -----------------------------------------------------
public:
	CGUIListBoard();
	virtual ~CGUIListBoard();

	void	SetSize( POINT pi_ptSize );
	void	SetSize( LONG pi_nWidth, LONG pi_nHeight );

	int		InsertObject( CGUIObject * pi_pObj, WORD pi_wInsertIndex );
	void	RemoveObject( CGUIObject * pi_pObj );
	void	RemoveAllObject( void );

	int		GetListSize( void );
	CGUIObject * GetListObject( WORD pi_wListIndex );


	inline	CGUIScrollbar * GetScrollbar( void ) { return &m_uiScrollbar; }

	void	ScrollEvent( _scroll_event & event );		

	void	SetSprite( void );
	

private:
	void	UpdateScrollbar( void );
};

#endif // __GUILISTBOARD_H__
