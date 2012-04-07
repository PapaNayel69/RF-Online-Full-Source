////////////////////////////////////////////////////////////////////////////////
//
// CItemUpgradeBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __ITEM_UPGRADE_BOARD_H__
#define __ITEM_UPGRADE_BOARD_H__

#include "CItemBoard.h"
#include "../../../UserInterface/GameWindow/ItemProc/GUIItemUpgradeWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_UIAT_TYPE	( 6 )
#define UIAT_ITEM		( 0 )						// 업그레이드 시킬 아이템
#define UIAT_TALIK		( 1 )						// 탈릭
#define UIAT_FIRE		( 2 )						// 보석들
#define UIAT_WATER		( 3 )
#define UIAT_TERRA		( 4 )
#define UIAT_WIND		( 5 )

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


class CItemUpgradeBoard : public CItemBoard
{
protected :

	CGUIItemUpgradeWindow		m_uiItemUpgradeWindow;

	DWORD						m_dwUpgradeItemIndex;					// 업그레이드 아이템 ( 도구 ) 

	BYTE						m_byItemPackNo[MAX_UIAT_TYPE];			// 업그레이드 하기 위해 필요한 자원이 들어있는 Pack No
	BYTE						m_byItemSocketNo[MAX_UIAT_TYPE];		// 업그레이드 하기 위해 필요한 자원이 들어있는 Socket No

public :

	CItemUpgradeBoard();
	virtual ~CItemUpgradeBoard();


			void			InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );

			void			OpenUpgradeWindow( DWORD pi_dwItemIndex );

protected :

	virtual	BOOL			UI_AddRequest_ProcessObject( void );

	virtual	BOOL			UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity );


	virtual	BOOL			UI_Trade_By_HotKey( BYTE pi_byPackNo, BYTE pi_bySocketNo );

public :

			BOOL			SelectUpgradeItem( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, BOOL pi_bTradeByHotKey );	// 업그레이드에 필요한 아이템 선택
			BOOL			DeselectUpgradeItem( BYTE pi_bySocketNo, BOOL pi_bTradeByHotKey );					// 업그레이드에 필요한 아이템 해제

			BOOL			ItemUpgrade_Request( void );								// 업그레이드 or 다운그레이드 요청
			BOOL			ItemUpgrade_Result( BOOL pi_bIsSuccess );					// 업그레이드 or 다운그레이드 요청에 대한 결과
			BOOL			ItemUpgrade_Result_DestroyItem( BYTE pi_byErrorCode );		// 업그레이드 실패시 아이템 파괴
			
			void			ReturnItemToItemBoard( void );

	inline	DWORD			GetUpgradeItemIndex( void )				{ return m_dwUpgradeItemIndex; }

			CBoardItem	*	GetTradeItem( BYTE pi_byPackNo, BYTE pi_bySocketNo );

protected :

			void			AddUpgradeItem( BYTE pi_byUpgradeSocketNo, BYTE pi_byOrgPackNo, BYTE pi_byOrgSocketNo, CBoardItem * pi_pOrgItem );
			void			RemoveUpgradeItem( BYTE pi_byUpgradeSocketNo );

			BOOL			ReturnHoverItemToInventory( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardItem * pi_pItemToReturn );
			BOOL			ReturnBoardItemToInventory( BYTE pi_byBoardSocketNo );
			float			CalculateSuccessProbabilityOfItemUpgrade( void );			// 업그레이드 성공 확률 계산

public :

			CBoardItem	*	GetItem( BYTE pi_bySocketNo );
	
	virtual	LRESULT			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	
};

#endif // __ITEM_UPGRADE_BOARD_H__
