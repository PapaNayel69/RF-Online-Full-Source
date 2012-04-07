////////////////////////////////////////////////////////////////////////////////
//
// COreProcBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __ITEM_WORK_BOARD_H__
#define __ITEM_WORK_BOARD_H__

#include "CItemBoard.h"
#include "../../../UserInterface/GameWindow/ItemProc/GUIItemMakeWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_ITEM_WORK_WINDOW_SLOT ( 100 )

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


class CItemWorkBoard : public CItemBoard
{
public :
	enum { WTT_WEAPON, WTT_ARMOR, WTT_BULLET, WTT_NUM };		// Work Tool Type ( WTT )

protected :

	CGUIItemMakeWindow		m_uiItemWorkWindow;


	DWORD					m_dwWorkToolItemIndex;							// 제작 도구의 인덱스	
	BYTE					m_byWorkToolType;								// ( WTT )
	
	ITEM_WORK_DATA	*		m_pItemWorkData;								// 현재 아바타가 제작하기로 선택한 아이템

	CBoardItem				m_clsNewItem;									// 제작된 아이템
	BYTE					m_byNewItemPackNo;								// 제작된 아이템이 들어갈 인벤토리의 Pack No
	BYTE					m_byNewItemSocketNo;							// 제작된 아이템이 들어갈 인벤토리의 Socket No

	DWORD					m_dwMaterialIndex[MAX_ITEM_MATERIAL_NUM];		// 아이템을 제작하기 위해 필요한 자원의 인덱스
	BYTE					m_byMaterialPackNo[MAX_ITEM_MATERIAL_NUM];		// 아이템을 제작하기 위해 필요한 자원이 들어있는 Pack No
	BYTE					m_byMaterialSocketNo[MAX_ITEM_MATERIAL_NUM];	// 아이템을 제작하기 위해 필요한 자원이 들어있는 Socket No

	// -----------------------------------------------------------------------

	CItemBoard				m_clsItemListBoard[WTT_NUM];					// 아이템 제작 리스트
	
	DWORD					m_dwWorkExp;									// 아이템 제작 숙련도
	BYTE					m_bIsSet[3];									// 리스트가 세팅이 됐는가?

public :

	CItemWorkBoard();
	virtual ~CItemWorkBoard();

			void				InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );

public :

			BOOL				OpenItemWorkWindow( CBoardItem * pi_pWorkTool );

			void				ClearWork( void );

	inline	ITEM_WORK_DATA *	GetItemWorkData( void )					{ return m_pItemWorkData; }
	inline	DWORD				GetWorkToolItemIndex( void )			{ return m_dwWorkToolItemIndex; }
	inline	DWORD				GetMaterialIndex( BYTE pi_byIndex )		{ return m_dwMaterialIndex[pi_byIndex]; }


protected :

			void				ClearWorkItemList( BYTE pi_byItemWorkType = 0xFF );

			BOOL				SettingItem_Weapon( void );
			BOOL				SettingItem_Armor( void );
			BOOL				SettingItem_Bullet( void );			

public :

			BOOL				AddItem( BYTE pi_byItemWorkType, BYTE pi_byTabNo, BYTE pi_bySocketNo, CBoardItem * pi_pItem );
			BOOL				RemoveItem( BYTE pi_byItemWorkType, BYTE pi_byTabNo, BYTE pi_bySocketNo, BYTE pi_byQuantity = 0xFF );
			CBoardItem	*		GetItem( BYTE pi_byItemWorkType, BYTE pi_byTabNo, BYTE pi_bySocketNo );

public :

			void				SelectWorkItem( BYTE pi_byTabNo, BYTE pi_bySocketNo );
			void				DeselectWorkItem( void );
			
			BOOL				MaterialSetting( void );						// 제작에 필요한 자원 설정

			BOOL				ItemWork_Request( void );						// 아이템 제작 요청
			BOOL				ItemWork_Result( BOOL pi_bIsSuccessed );		// 아이템 제작 요청에 대한 결과			

	inline	void				SetNewItem( CBoardItem * pi_pNewItem )	{ memcpy( &m_clsNewItem, pi_pNewItem, sizeof( CBoardItem ) ); }

public :

	virtual	LRESULT				MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	

};

#endif // __ITEM_WORK_BOARD_H__
