////////////////////////////////////////////////////////////////////////////////
//
// CBeltBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#ifndef __BELT_BOARD_H__
#define __BELT_BOARD_H__

#include "CItemBoard.h"
#include "../../../UserInterface/GameWindow/GUIItemBeltWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CBeltBoard : public CItemBoard
{
protected :

	CGUIItemBeltWindow		m_uiBeltWindow;	

public:
	
	CBeltBoard();
	virtual ~CBeltBoard();

			void			InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );	

protected:
	
	virtual	BOOL			UI_CheckBeforeAdd_CheckHoverObject( void );	
	virtual	void			UI_AddResult_ProcessObject( void );			

	virtual	BOOL			UI_UseRequest_ProcessObject( void );
	virtual	void			UI_UseResult_ProcessObject( void );	

public :

	virtual	LRESULT			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	

public :

			BYTE			FindEmptySocket( void );
			CBoardItem *	GetItem( BYTE pi_bySocketNo );
};

#endif // __BELT_BOARD_H__

#endif // _NEW_UI_