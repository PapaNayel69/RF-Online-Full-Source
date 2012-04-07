////////////////////////////////////////////////////////////////////////////////
//
// CAnimusBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#ifndef __ANIMUS_BOARD_H__
#define __ANIMUS_BOARD_H__

#include "../Common/CBoard.h"
#include "../../../UserInterface/GameWindow/Recall/GUIMobControl.h"
#include "../../../UserInterface/GameWindow/Recall/GUIRecallWindow.h"

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CNetMsgProcessor_ItemArrangement;
class CNetMsgProcessor_Summon;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CAnimusControlBoard : public CBaseBoard
{
protected :

	CAnimus *					m_pAnimusChar;
	CAnimusItem *				m_pAnimusItem;
	BOOL						m_bIsRequestResummon;	// 현재 소환중인 몬스터의 재소환 요청이 들어왔는가?

	CGUIMobControl				m_uiAnimusControlBoard;
	CNetMsgProcessor_Summon *	m_pNMP_Summon;

public :

	CAnimusControlBoard();
	virtual ~CAnimusControlBoard();

	virtual	void			InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );

public :

	virtual	void			FrameMove( void );
	virtual	LRESULT			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public :

			BOOL			CommandToAnimus( BYTE pi_byCommand );

			void			SetAnimusChar( CAnimus * pi_pAnimus );
	inline	CAnimus *		GetAnimusChar( void )		{ return m_pAnimusChar; }
			void			SetAnimusItem( CAnimusItem * pi_pItem );
	inline	CAnimusItem *	GetAnimusItem( void )		{ return m_pAnimusItem; }
			void			SetExp( DWORD pi_dwExp );
			void			SetHP( DWORD pi_dwHP );
			void			SetFP( DWORD pi_dwFP );

	inline	BOOL			IsSummoned( void )			{ return ( m_pAnimusItem ) ? TRUE : FALSE; }
			BOOL			IsEqualSummonAnimusItem( CAnimusItem * pi_pInputItem );
			BOOL			IsEqualSummonAnimusItem( DWORD pi_dwIndex );
			BOOL			IsEqualSummonAnimusChar( CAnimus * pi_pInputChar );
			BOOL			IsEqualSummonAnimusChar( DWORD pi_dwIndex );

	inline	void			SetRequestResummon( BOOL pi_bFlag )	{ m_bIsRequestResummon = pi_bFlag; }

public :

	virtual	void			OpenWindow( void );
	virtual	void			CloseWindow( void );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CAnimusBoard : public CObjectBoard
{
protected :

	DWORD								m_dwLatestSummonTime;

	CGUIRecallWindow					m_uiAnimusWindow;
	CNetMsgProcessor_ItemArrangement *	m_pNMP_Arrange;
	CNetMsgProcessor_Summon *			m_pNMP_Summon;

public :

	CAnimusBoard();
	virtual ~CAnimusBoard();

			void			CreateBoardObject( void );
			void			InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );

protected :

	virtual	BOOL			Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );
	virtual	BOOL			Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL			Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity );
	virtual	BOOL			Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL			UI_CheckBeforeAdd_CheckHoverObject( void );
	virtual	BOOL			UI_CheckBeforeAdd_CheckTargetObject( void );

	virtual	BOOL			UI_AddRequest_ProcessObject( void );
	virtual	void			UI_AddResult_ProcessObject( void );

	virtual	BOOL			UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity );

	virtual	BOOL			UI_SwapRequest_ProcessObject( void );
	virtual	void			UI_SwapResult_ProcessObject( void );

	virtual	BOOL			UI_CheckBeforeUse_CheckOriginalObject( void );
	virtual	BOOL			UI_UseRequest_ProcessObject( void );
	virtual	void			UI_UseResult_ProcessObject( void );

public :

			void			UI_SummonResult( BOOL pi_bIsSuccessed );
			void			UI_ReturnResult( BOOL pi_bIsSuccessed );

			void			UI_Link_By_Hotkey( DWORD pi_dwIndex );
			void			Unlink( DWORD pi_dwIndex );

	virtual	void			FrameMove( void );
	virtual	LRESULT			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public :

	inline	CAnimusItem *	GetAnimusByIndex( DWORD pi_dwIndex )	{ return static_cast< CAnimusItem * >( GetObjectByIndex( pi_dwIndex ) ); }
			CAnimusItem *	GetAnimusByDTIndex( DWORD pi_dwDTIndex );

public :

			BOOL			IsPassedSummonLimitTime( void );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __ANIMUS_BOARD_H__

#endif // _NEW_UI_