////////////////////////////////////////////////////////////////////////////////
//
// CGameEventBoard Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __GAME_EVENT_BOARD_H__
#define __GAME_EVENT_BOARD_H__

#include "../../../DefineMessage.h"
#include "../../../UserInterface/GameWindow/EtcWin/GUIEventWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Game Event Type ( GET_ )
enum
{
	GET_TUTORIAL, GET_TRADE, GET_PARTY, GET_GUILD, GET_COMMAND, GET_QUEST,
	GET_CLASS,
	MAX_EVENT_TYPE
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CGameEvent
{
protected :

	BYTE	m_byType;
	DWORD	m_dwIndex;

public :

	CGameEvent();
	~CGameEvent();

	virtual	CGameEvent*	New( void ) = 0;
	virtual	CGameEvent*	Clone( void ) = 0;

	inline	BOOL		IsEqualType( BYTE pi_byType )	{ return ( m_byType == pi_byType ); }
	virtual	BOOL		IsEqualSubType( BYTE pi_byType ) = 0;
	inline	BOOL		IsEqual( DWORD pi_dwIndex )		{ return ( m_dwIndex == pi_dwIndex ); }
	virtual	BOOL		CheckWhetherShouldBeErase( CGameEvent & pi_clsOrgEvent );

	inline	void		SetType( BYTE pi_byType )		{ m_byType = pi_byType; }
	inline	BYTE		GetType( void )					{ return m_byType; }
	inline	void		SetIndex( DWORD pi_dwIndex )	{ m_dwIndex = pi_dwIndex; }
	inline	DWORD		GetIndex( void )				{ return m_dwIndex; }
};

//==============================================================================

class CPartyEvent : public CGameEvent
{
public :

	enum { INVITE_PLAYER_TO_MY_PARTY, AVATAR_JOIN_OTHER_PARTY,
		   INVITE_AVATAR_TO_OTHER_PARTY, PLAYER_JOIN_MY_PARTY };

protected :

	BYTE	m_byPartyEventType;

	char	m_pApplicantName[MAX_NAME_LENGTH];
	DWORD	m_dwApplicantIndex;

public :

	CPartyEvent();
	~CPartyEvent();

	virtual	CGameEvent*	New( void );
	virtual	CGameEvent*	Clone( void );

	virtual	BOOL		IsEqualSubType( BYTE pi_byType )		{ return IsEqualPartyEventType( pi_byType ); }
	virtual	BOOL		CheckWhetherShouldBeErase( CGameEvent & pi_clsOrgEvent );

	inline	void		SetPartyEventType( BYTE pi_byType )		{ m_byPartyEventType = pi_byType; }
	inline	BYTE		GetPartyEventType( void )				{ return m_byPartyEventType; }
	inline	BOOL		IsEqualPartyEventType( BYTE pi_byType )	{ return ( m_byPartyEventType == pi_byType ); }
			void		SetApplicant( CCharacter * pi_pChar );
	inline	void		SetApplicantName( char * pi_pName )		{ _strcpy( m_pApplicantName, pi_pName, MAX_NAME_LENGTH ); }
	inline	char *		GetApplicantName( void )				{ return m_pApplicantName; }
	inline	void		SetApplicantIndex( DWORD pi_dwIndex )	{ m_dwApplicantIndex = pi_dwIndex; }
	inline	DWORD		GetApplicantIndex( void )				{ return m_dwApplicantIndex; }
	inline	BOOL		IsEqualApplicant( DWORD pi_dwIndex )	{ return ( m_dwApplicantIndex == pi_dwIndex ); }
};

//==============================================================================

class CTradeEvent : public CPartyEvent
{
public :

	enum { SEND_TRADE_REQUEST, RECEIVE_TRADE_REQUEST };

protected :

public :

	CTradeEvent();
	~CTradeEvent();

	virtual	CGameEvent*	New( void );
	virtual	CGameEvent*	Clone( void );

	virtual	BOOL		IsEqualSubType( BYTE pi_byType )		{ return CPartyEvent::IsEqualPartyEventType( pi_byType ); }

	inline	void		SetTradeEventType( BYTE pi_byType )		{ CPartyEvent::SetPartyEventType( pi_byType ); }
	inline	BYTE		GetTradeEventType( void )				{ return CPartyEvent::GetPartyEventType(); }
	inline	BOOL		IsEqualTradeEventType( BYTE pi_byType )	{ return CPartyEvent::IsEqualPartyEventType( pi_byType ); }
};

//==============================================================================

class CClassEvent : public CGameEvent
{
public :

	enum { RECEIVE_CLASS_CHANGE };

protected :

	BYTE	m_byClassEventType;

public :

	CClassEvent();
	~CClassEvent();

	virtual	CGameEvent*	New( void );
	virtual	CGameEvent*	Clone( void );

	virtual	BOOL		IsEqualSubType( BYTE pi_byType )		{ return IsEqualClassEventType( pi_byType ); }
			BOOL		CheckWhetherShouldBeErase( CGameEvent & pi_clsOrgEvent );

	inline	void		SetClassEventType( BYTE pi_byType )		{ m_byClassEventType = pi_byType; }
	inline	BYTE		GetClassEventType( void )				{ return m_byClassEventType; }
	inline	BOOL		IsEqualClassEventType( BYTE pi_byType )	{ return ( m_byClassEventType == pi_byType ); }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CGUIScreen;
typedef deque< CGameEvent * >			EVENT_DEQUE;
typedef deque< CGameEvent * >::iterator	EVENT_DEQUE_ITER;

class CGameEventBoard
{
public :

	enum { MAX_MESSAGE_LENGTH = 256 };

protected :

	EVENT_DEQUE		m_dequeEvent;
	CGUIEventWindow	m_uiEventWindow;
	FONT_INFO		m_uiFontInfo[MAX_EVENT_TYPE];

	DWORD			m_dwLastestEventPushTime;
	DWORD			m_dwCurrentEventIndex;

	DWORD			m_dwAllEventCount;		// 현재까지의 이벤트 카운트 ( 이벤트의 인덱스로 사용된다. )

	char			m_pMessage[MAX_MESSAGE_LENGTH];

public :

	CGameEventBoard();
	~CGameEventBoard();

	void		Clear( void );

	void		InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY, CGUIScreen * pi_pUiScreen );

	BOOL		PushEvent( CGameEvent & pi_clsOrgEvent );
	BOOL		PopEvent( void );

	void		ClearEvent( BYTE pi_byEventType, BYTE pi_bySubType );
	void		ClearAllEvent( BYTE pi_byEventType = 0xFF );

	void		FrameMove( void );
	LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private :

	BOOL		CheckEventBeforePush( CGameEvent & pi_clsOrgEvent );
	BOOL		CheckEventBeforePush_Party( CGameEvent & pi_clsOrgEvent );
	BOOL		CheckEventBeforePush_Trade( CGameEvent & pi_clsOrgEvent );
	BOOL		CheckEventBeforePush_Class( CGameEvent & pi_clsOrgEvent );

	BOOL		RemoveEvent( EVENT_DEQUE_ITER & pi_iterEvent );

	BOOL		MouseHoverOnEventIcon( DWORD pi_dwEventIndex );
	BOOL		MouseHoverOnEventIcon_Party( CGameEvent * pi_pHoverEvent );
	void		SetPartyInviteOrJoinMessage( CPartyEvent * pi_pHoverEvent );
	BOOL		MouseHoverOnEventIcon_Trade( CGameEvent * pi_pHoverEvent );
	BOOL		MouseHoverOnEventIcon_Class( CGameEvent * pi_pHoverEvent );

	BOOL		ClickOKButton( DWORD pi_dwEventIndex );
	BOOL		ClickOKButton_Party( CGameEvent * pi_pHoverEvent );
	BOOL		ClickOKButton_Trade( CGameEvent * pi_pHoverEvent );
	BOOL		ClickOKButton_Class( CGameEvent * pi_pHoverEvent );

	BOOL		ClickCancelButton( DWORD pi_dwEventIndex );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

extern CGameEventBoard * g_pGameEventBoard;
#define _GetGameEventBoard() g_pGameEventBoard

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __GAME_EVENT_BOARD_H__