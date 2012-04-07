////////////////////////////////////////////////////////////////////////////
//
// CGUIItemUpgradeWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef ___GUIITEMUPGRADEWINDOW_H__
#define ___GUIITEMUPGRADEWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GameWindow/GUIInventoryWindow.h"
#include "../../GUIController/GUIMessageBox.h"

#define	TOTAL_UPGRADE_SOCKET_NO		6

/*//////////////////////////////////////////////////////////////////////////

[ CGUIItemUpgradeWindow ] 
 - 아이템 업그레이드 창

//////////////////////////////////////////////////////////////////////////*/
class CGUIItemUpgradeWindow : public CGUIItemWindow
{
public:
	enum UpgradeSocket {
		UpgradeItem,
		Crystal,
		Fire,
		Water,
		Terra,
		Wind,
		None
	};

// < Data Member > ---------------------------------------------------------
private:

	CGUIText			m_uiTitle;
	CGUIObjectAni		m_uiAniTitleBar;	
	CGUIButton			m_uiHelpButton;
	CGUIButton			m_uiCloseButton;
	

	// upgrade item
	CGUIItem			m_uiUpgradeItem[TOTAL_UPGRADE_SOCKET_NO];
	CGUIObject			m_uiEmptySocketMark[TOTAL_UPGRADE_SOCKET_NO];

	CGUIButtonEx		m_uiUpgradeButton;	

	BOOL				m_bIsCreated;	

// < Member Function > -----------------------------------------------------
public:
	CGUIItemUpgradeWindow();
	virtual ~CGUIItemUpgradeWindow();

			void		Init( void );
			void		SetSprite( void );
			

	// 업그레이드 아이템 추가, 삭제
	virtual	BOOL		TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex,
								    DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel = NONE_LEVEL );
	virtual	BOOL		TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum =0 , CGUIItem * po_pOutItem = NULL );	
	
	virtual	CGUIItem *	GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex );


			void		EnableUpgrade( BOOL pi_bEnable );	// 업그레이드 버튼 enable, disable

			void		MouseEvent( _mouse_event & event );	


	
	BOOL	TakeInItemID( UpgradeSocket	pi_eSocket,
						  DWORD			pi_dwItemID,
						  DWORD			pi_dwSpriteID,
						  WORD			pi_wPackIndex,
						  WORD			pi_wSocketIndex ){ return FALSE;}
	BOOL	TakeOutItemID( UpgradeSocket pi_eSocket, CGUIItem * pi_pOutItem = NULL ){ return FALSE;}		
	void	SetSuccessProb( float pi_fProb ){;}					// 업그레이드 성공확률 설정. 범위( 0.f ~ 100.f )	

private:

	void	SetEmptySocket( UpgradeSocket pi_eSocket, BOOL pi_bIsEmpty );
	void	SetNonEmptySocket( UpgradeSocket pi_eSocket );
	
};

#endif // ___GUIITEMUPGRADEWINDOW_H__
