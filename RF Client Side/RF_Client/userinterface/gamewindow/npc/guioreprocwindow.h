////////////////////////////////////////////////////////////////////////////
//
// CGUIOreProcWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIOREPROCWINDOW_H__
#define __GUIOREPROCWINDOW_H__

#include "../../GUIController/GUIItem.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIScrollbar.h"
#include "../../GUIController/GUIText.h"

#include "../GUIInventoryWindow.h"
#include "GUIShopWindow.h"

#define	MAX_RESULT_ORE_ITEM		40
#define	MAX_RESULT_ORE_SOCKET	20

/*//////////////////////////////////////////////////////////////////////////

[ CGUIOreProcWindow ] 
 - 원석 가공 창

//////////////////////////////////////////////////////////////////////////*/
class CGUIOreProcWindow : public CGUIItemWindow,
						  public IScrollObserver
{
// < Data Member > ---------------------------------------------------------
private:

	CGUIText			m_uiTitle;
	CGUIObjectAni		m_uiAniTitleBar;		
	CGUIButton			m_uiCloseButton;
	CGUIButton			m_uiHelpButton;

	
	CGUIItem			m_uiInputOreItem;							// 가공할 원석

	
	CGUIItem			m_uiOutputOreItem[MAX_RESULT_ORE_ITEM];

	CGUISocket			m_uiOutputOreSocket[MAX_RESULT_ORE_SOCKET];	// 가공된 Ore가 들어갈 socket
	CGUIScrollbar		m_uiOutputScrollBar;
	WORD				m_wOutputViewedIndex;
	BYTE				m_byOutputOreNum;							// 가공된 ore 갯수	
	

	CGUIButtonEx		m_uiProcButton;								// 가공 버튼
	CGUIButtonEx		m_uiDisposalButton;							// 처분 버튼	
	
	CGUIText			m_uiDalant;	
	CGUIText			m_uiProcPrice;


	BOOL				m_bIsCreated;
	

// < Member Function > -----------------------------------------------------
public:
	CGUIOreProcWindow();
	virtual ~CGUIOreProcWindow();

	
			void		Init( void );
			void		SetSprite( void );	
	

	//----------------------------------------------------------------------
	// 가공할 원석
			BOOL		TakeInInputOre( DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byQuantity );
			BOOL		TakeOutInputOre( void );

	inline	CGUIItem *	GetOre( void )						{ return &m_uiInputOreItem; }
	
			void		SetProcPrice( DWORD pi_dwPrice );					// 처리 비용

	//----------------------------------------------------------------------
	// 가공된 잉갓
			BOOL		TakeInResultOre( BYTE pi_bySocketIndex,
								   DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BOOL pi_bResizeScrollbar = TRUE );
			BOOL		TakeOutResultOre( BYTE pi_bySocketIndex, BYTE pi_byNum );
			void		ClearResultOreSocket( void );
			

	virtual	BOOL		TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel=NONE_LEVEL );
	virtual	BOOL		TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum=0, CGUIItem * po_pOutItem=NULL );	
	
	virtual	CGUIItem *	GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex );
	

	//----------------------------------------------------------------------

	void		EnableProc( BOOL pi_bEnable );
	void		EnableDisposal( BOOL pi_bEnable );	

	
	//----------------------------------------------------------------------

	void		MouseEvent( _mouse_event & event );
	void		ScrollEvent( _scroll_event & event );

	void		ClosingDialogBox( CGUIDialogBox * pi_pDlg );
	
	BOOL		Draw( void );
	

private:
	
	void		UpdateResultSocket( void );	
	
};

#endif // __GUIOREPROCWINDOW_H__
