////////////////////////////////////////////////////////////////////////////
//
// CGUIItemBeltWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIITEMBELTWINDOW_H__
#define __GUIITEMBELTWINDOW_H__

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIItem.h"
#include "../GUIController/GUIButton.h"


#define MAX_ITEM_SOCKET_NO		4


class CGUINumInputDlg;

/*//////////////////////////////////////////////////////////////////////////

[ CGUIItemBeltWindow ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIItemBeltWindow : public CGUIItemWindow
{
// < Data Member > ---------------------------------------------------------
private :	

	CGUIItem			m_uiItem[MAX_ITEM_SOCKET_NO];
	CGUIObject			m_uiSocket[MAX_ITEM_SOCKET_NO];

	CGUIObject			m_uiUselessSocketMark[MAX_ITEM_SOCKET_NO];
	CGUIObject			m_uiTradeItemMark[MAX_ITEM_SOCKET_NO];


	CGUIContainer		m_uiBoard;	
	
	
	BYTE				m_byInputSocketIndex;		// item number input

	BOOL				m_bIsCreated;


// < Member Function > -----------------------------------------------------
public :

	CGUIItemBeltWindow();
	virtual ~CGUIItemBeltWindow();

			void		Init( void );
			void		SetSprite( void );

			void		Open( void ) {;}	// Hotkeyboard에 포함되기 때문에 open, close를 막는다.
			void		Close( void ) {;}
	
	
	virtual	BOOL		TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex,	DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel = NONE_LEVEL );
	virtual	void		SetTradeItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bIsTradeItem );	// 거래할 아이템 표시

	virtual	BOOL		TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum = 0, CGUIItem * po_pOutItem = NULL );

	virtual	CGUIItem *	GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex );	
	inline	CGUIItem *	GetItem( BYTE pi_bySocketIndex )	{ return &m_uiItem[pi_bySocketIndex]; }	

	virtual	void		SetUselessSocket( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bFlag );

	inline	BOOL		IsEmpty( BYTE pi_bySocketIndex )	{ return ( m_uiItem[pi_bySocketIndex].GetItemID() == NONE_ITEM_ID ); }
			void		SetEmpty( BYTE pi_bySocketIndex );

			void		UseItem( BYTE pi_bySocketIndex );

			void		MouseEvent( _mouse_event & event );
			void		ClosingDialogBox( CGUIDialogBox * pi_pDlg );
			BOOL		ProcNumInputDlg( CGUINumInputDlg * pi_pInputDlg );
};

#endif // __GUIITEMBELTWINDOW_H__
