////////////////////////////////////////////////////////////////////////////////
//
// CLinkBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __LINK_BOARD_H__
#define __LINK_BOARD_H__

#include "CBoard.h"
#include "../../../UserInterface/GameWindow/GUIForceBeltWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CLinkBoard : public CObjectBoard
{
protected :

	CGUIForceBeltWindow		m_uiLinkWindow;

public :

	CLinkBoard();
	virtual	~CLinkBoard();

			void			InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );

			void			UI_UseSkillForceRequest( CSFItem * pi_pSFItem, CCharacter * pi_pTarget );
			void			UI_UseSkillForceRequest( BYTE pi_bySocketNo, CCharacter * pi_pTarget, BOOL pi_bEventInSFBeltBoard );

			
			void			UI_Unlink_By_Hotkey( BYTE pi_bySocketNo );

protected :
	
	virtual	BOOL			Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );
	virtual	BOOL			Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL			Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity );
	virtual	BOOL			Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL			UI_CheckBeforeAdd_CheckEmptySocket( void );
	virtual	BOOL			UI_CheckBeforeAdd_CheckTargetObject( void );
	virtual	BOOL			UI_AddRequest_ProcessObject( void );
	virtual	void			UI_AddResult_ProcessObject( void );

	virtual	BOOL			UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity );
	virtual	BOOL			UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity );

	virtual	BOOL			UI_SwapRequest_ProcessObject( void );
	virtual	void			UI_SwapResult_ProcessObject( void );

	virtual	BOOL			UI_UseRequest_ProcessObject( void );
	virtual	void			UI_UseResult_ProcessObject( void );

public :

	virtual	void			FrameMove( void );
	virtual	LRESULT			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public :

			BYTE			FindEmptySocket( void );

			void			BeginDelayAnimation( CBoardObject * pi_pOrgObject, DWORD pi_dwTime );

protected :

			CBoardObject *	GetOrgBoardObject( CBoardObject * pi_pObject );
			CBoardObject *	GetEqualObject( CBoardObject * pi_pOrgObject );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __LINK_BOARD_H__
