////////////////////////////////////////////////////////////////////////////
//
// CGUITradeWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUITRADEWINDOW_H__
#define __GUITRADEWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIItem.h"
#include "../../GUIController/GUIText.h"
#include "../../GUIController/GUIInput.h"

#include "../Npc/GUIShopWindow.h"

#define	MAX_TRADE_ITEM_NUM	15

class CGUIInventoryWindow;

/*//////////////////////////////////////////////////////////////////////////

[ CGUITradeWindow ]
 
   - player간 아이템 교환 거래

//////////////////////////////////////////////////////////////////////////*/

class CGUITradeWindow : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
private:

	CGUIText			m_uiTitle;
	CGUIObjectAni		m_uiAniTitleBar;
	CGUIButton			m_uiCloseButton;

	// avatar
	CGUIText			m_uiAvatarName;
	CGUIObject			m_uiAvatarUselessSocket[MAX_TRADE_ITEM_NUM];
	CGUIItem			m_uiAvatarItem[MAX_TRADE_ITEM_NUM];
	CGUIInput			m_uiAvatarDalantInput;
	CGUIInput			m_uiAvatarGoldInput;	
	CGUIButtonEx		m_uiAvatarLockButton;
	CGUIButtonEx		m_uiAvatarAcceptButton;	

	DWORD				m_dwAvatarDalant;
	DWORD				m_dwAvatarGold;


	// other player
	CGUIText			m_uiOtherName;	
	CGUIObject			m_uiOtherUselessSocket[MAX_TRADE_ITEM_NUM];
	CGUIItem			m_uiOtherItem[MAX_TRADE_ITEM_NUM];
	CGUIText			m_uiOtherDalant;
	CGUIText			m_uiOtherGold;	
	CGUIButtonEx		m_uiOtherLockButton;
	CGUIButtonEx		m_uiOtherAcceptButton;
	BOOL				m_bIsLock_Other;
	BOOL				m_bIsAccept_Other;


	CGUIText			m_uiAvatarDalantStr;
	CGUIText			m_uiAvatarGoldStr;
	CGUIText			m_uiOtherDalantStr;
	CGUIText			m_uiOtherGoldStr;	
	

	BOOL				m_bIsCreated;

// < Member Function > ----------------------------------------------------
public:
	CGUITradeWindow();
	virtual ~CGUITradeWindow();

			void		Init( void );
			void		SetSprite( void );
	
			void		Open( void );
			void		Close( void );


	// Avatar --------------------------------------------------------------
			void		SetAvatarName( char * pi_pName );	

			void		SetEnableSocketNum_Avatar( BYTE pi_byNum );	// 상대방의 빈 소켓 갯수. avatar가 보낼 아이템 갯수를 제한한다.		

			BOOL		TakeInItem_Avatar( BYTE pi_bySocketIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byQuantity );
			BOOL		TakeOutItem_Avatar( BYTE pi_bySocketIndex );	

	inline	DWORD		GetAvatarDalant( void ) { return m_dwAvatarDalant; }
	inline	DWORD		GetAvatarGold( void ) { return m_dwAvatarGold; }
			void		SetAvatarDalant( DWORD pi_dwDalant );
			void		SetAvatarGold( DWORD pi_dwGold );	
	
			void		SetLock_Avatar( BOOL pi_bLock );
			void		SetAccept_Avatar( BOOL pi_bAccept );	

			CGUIItem *	GetItem_Avatar( BYTE pi_bySocketIndex );


	// Other ---------------------------------------------------------------
			void		SetOtherName( char * pi_pName );

			void		SetEnableSocketNum_Other( BYTE pi_byNum );	// 자신이 받을 수 있는 아이템 갯수

			BOOL		TakeInItem_Other( BYTE pi_bySocketIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byQuantity );
			BOOL		TakeOutItem_Other( BYTE pi_bySocketIndex );

			CGUIItem *	GetItem_Other( BYTE pi_bySocketIndex );

			void		SetOtherDalant( DWORD pi_dwDalant );
			void		SetOtherGold( DWORD pi_dwGold );

			void		SetLock_Other( BOOL pi_bLock );
			void		SetAccept_Other( BOOL pi_bAccept );

	// ---------------------------------------------------------------------

			void		MouseEvent( _mouse_event & event );
			void		KeyEvent( _key_event & event );

			LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );			
};

#endif // __GUITRADEWINDOW_H__
