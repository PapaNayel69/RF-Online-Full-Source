////////////////////////////////////////////////////////////////////////////
//
// CGUIMapLoadingWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIEVENTWINDOW_H__
#define __GUIEVENTWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUITextBoard.h"
#include <deque>


/*//////////////////////////////////////////////////////////////////////////

[ CGUIEventWindow ]   

//////////////////////////////////////////////////////////////////////////*/

#define		VIEW_EVENT_NUM			3

#define		EVENT_MSG_BUFSIZE		128


struct EVENT_INFO
{
	DWORD	dwEventID;
	BYTE	byEventType;
	BOOL	bIsBlinked;		// ±Ù∫˝¿Ã¥¬∞°?
	DWORD	dwBlinkedTime;
};

typedef	deque< EVENT_INFO * >	CGUIEventList;

class CGUIEventWindow : public CGUIWindow
{
public:
	enum EventType
	{
		Tutorial,
		Trade,
		Party,
		Guild,
		Order,
		Quest,
		Class,
		Bulletin,

		Event_Num
	};

	enum ButtonType
	{
		OK,
		YesOrNo,

		Button_Num
	};

// < Data Member > ---------------------------------------------------------
private:

	CGUIObject		m_uiEventButton[VIEW_EVENT_NUM];

	CGUIButton		m_uiUpButton;
	CGUIButton		m_uiDownButton;

	CGUIContainer	m_uiEventInfoBoard;		
	CGUIButtonEx	m_uiYESButton;
	CGUIButtonEx	m_uiNOButton;	
	char			m_pEventInfoMsgStr[EVENT_MSG_BUFSIZE];
	FONT_INFO		m_sEventInfoFont;
	BYTE			m_byOpenedInfoIndex;

	CGUIObject		m_uiEventBlock;


	CGUIEventList	m_listEvent;

	int				m_nFirstViewIndex;	

	BOOL			m_bIsCreated;

	DWORD			m_dwSendEventID;

// < Member Function > -----------------------------------------------------
public:
	CGUIEventWindow();
	virtual ~CGUIEventWindow();

	void	Init( void );
	void	SetSprite( void );

	// --------------------------------------------------------------------
	// ¿Ã∫•∆Æ ª¿‘, ªË¡¶
	void	InsertEvent( WORD pi_wInsertIndex, DWORD pi_dwEventID, EventType pi_eEventType );
	void	RemoveEvent( DWORD pi_dwEventID );	

	// ¿Ã∫•∆Æ ±Ùπ⁄¿”
	void	SetBlink( DWORD pi_dwEventID );
	void	SetUnBlink( DWORD pi_dwEventID );
	
	void	SetPage( int pi_nPageIndex, BOOL pi_bIsUpdate = FALSE );


	// --------------------------------------------------------------------
	// event info board	
	void	OpenEventInfoBoard( DWORD pi_dwEventID, CGUITextInfo * pi_pTextInfo, ButtonType pi_btButtonType );
	void	CloseEventInfoBoard( void );
	

	// --------------------------------------------------------------------
	void	MouseEvent( _mouse_event & event );

	void	Update( void );	

	BOOL	IsCrossed( LONG pi_nXPos, LONG pi_nYPos );

	BOOL	Draw( void );

private:
	void	SetEventButtonState( EventType pi_eEventType, CGUIObject * po_pEventObj, BOOL pi_bIsBlinked, BYTE pi_byIndex );
	BOOL	IsBlinked( EventType pi_eEventType, CGUIObject * pi_pEventObj );
	void	UpdateEventButton( void );
};

#endif // __GUIEVENTWINDOW_H__
