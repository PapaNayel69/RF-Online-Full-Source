////////////////////////////////////////////////////////////////////////////////
//
// CClassChangeBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CLASS_CHANGE_BOARD_H__
#define __CLASS_CHANGE_BOARD_H__

#include "../../../UserInterface/GameWindow/Npc/GUIClassChangeWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Event;

class CClassChangeBoard : public CBaseBoard
{
	BOOL			m_bIsEnableClassChange;				// 클래스 전직이 가능한가?

	CPlayerClass *	m_listChildClass[CPlayerClass::MAX_CHILD_CLASS_NUM];
	CPlayerClass *	m_pSelectClass;						// 아바타가 선택한 클래스

	CGUIClassChangeWindow	m_uiClassChangeWindow;	
	CNetMsgProcessor_Event*	m_pNMP_Event;

public :

	CClassChangeBoard();
	virtual ~CClassChangeBoard();

	virtual	void		InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );
	virtual	void		OpenWindow( void );

			void		UI_SelectClass( BYTE pi_bySocketNo );
			void		UI_ClassChangeRequest( void );
			void		UI_ClassChangeResult( void );

	virtual	void		FrameMove( void );
	virtual	LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public :

	inline	void		SetEnableClassChange( BOOL pi_bFlag )	{ m_bIsEnableClassChange = pi_bFlag; }
	inline	BOOL		IsEnableClassChange( void )				{ return m_bIsEnableClassChange; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __CLASS_CHANGE_BOARD_H__
