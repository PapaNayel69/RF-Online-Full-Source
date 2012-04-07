////////////////////////////////////////////////////////////////////////////
//
// CGUIForceBeltWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIFORCEBELTWINDOW_H__
#define __GUIFORCEBELTWINDOW_H__

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIButton.h"
#include "GUIInventoryWindow.h"

#define MAX_FORCE_SOCKET_NO		8

/*//////////////////////////////////////////////////////////////////////////

[ CGUIForceBeltWindow ]

  - Skill도 포함된다.

//////////////////////////////////////////////////////////////////////////*/
class CGUIForceBeltWindow : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
private:

	CGUISFItem			m_uiItem[MAX_FORCE_SOCKET_NO];
	CGUIObject			m_uiSocket[MAX_FORCE_SOCKET_NO];

	CGUIContainer		m_uiBoard;

	BOOL				m_bCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUIForceBeltWindow();
	virtual ~CGUIForceBeltWindow();

			void		Init( void );
			void		SetSprite( void );

			void		Open( void ) {;}		// Hotkeyboard에 포함되기 때문에 open, close를 막는다.
			void		Close( void ) {;}

			BOOL		TakeInItem( BYTE pi_bySocketIndex, CGUIItem * pi_pSourceItem );
			BOOL		TakeOutItem( BYTE pi_bySocketIndex );

	inline CGUISFItem *	GetItem( BYTE pi_bySocketIndex )	{ return &m_uiItem[pi_bySocketIndex]; }

			void		UseItem( BYTE pi_bySocketIndex );

			void		MouseEvent( _mouse_event & event );
};


#endif // __GUIFORCEBELTWINDOW_H__
