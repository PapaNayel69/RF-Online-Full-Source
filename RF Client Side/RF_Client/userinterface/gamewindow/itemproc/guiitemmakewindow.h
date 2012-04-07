////////////////////////////////////////////////////////////////////////////
//
// CGUIRecallWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef ___GUIITEMMAKEWINDOW_H__
#define ___GUIITEMMAKEWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIItem.h"

#include <vector>

#define	TOTAL_ITEM_GROUP_NUM		9
#define	TOTAL_ITEM_SOCKET_NUM		20

#define	WEAPON_CLASS_NUM			9
#define	ARMOR_CLASS_NUM				5
#define	CONSUME_CLASS_NUM			2

struct ItemInfo
{
	DWORD	dwItemID;
	DWORD	dwSpriteID;
};

typedef vector<ItemInfo *>	MakeItemList;

////////////////////////////////////////////////////////////////////////////

class CGUIItemListWindow : public CGUIItemWindow
{
// < Data Member > ---------------------------------------------------------
private :

	BYTE				m_byMakeItemType;

	CGUIObject			m_uiTabBase;
	CGUIObject	*		m_pTab;
	BYTE				m_byTotalTabNum;
	BYTE				m_byCurTabIndex;

	CGUIItem	**		m_pItem;
	BYTE				m_byTotalItemNum;
	BYTE		*		m_pEndItemIndex;

	CGUISocket			m_uiSocket[TOTAL_ITEM_SOCKET_NUM];	

	BYTE				m_bySelectedItemIndex;
	
	
	CGUIObject			m_uiPageBoard;
	CGUIButton			m_uiPageUp;
	CGUIButton			m_uiPageDown;
	CGUIText			m_uiPageNoStr;
	BYTE				m_byTotalPageNo;
	BYTE				m_byCurPageIndex;	

	BOOL				m_bIsCreated;	

// < Member Function > -----------------------------------------------------
public :

	CGUIItemListWindow();
	virtual ~CGUIItemListWindow();

			void		Create( BYTE pi_byMakeItemType, BYTE pi_byTotalPackNum, BYTE pi_byTotalSocketNum );

	inline	BOOL		IsCreated( void ) { return m_bIsCreated; }

			void		Init( void );
			void		SetSprite( void );	

public :			

	virtual	BOOL		TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex,
								    DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel = NONE_LEVEL );
	virtual	BOOL		TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum =0 , CGUIItem * po_pOutItem = NULL );	
	
	virtual	CGUIItem *	GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex );

public :

			void		SetTab( BYTE pi_byTabIndex );			
			void		SetPage( BYTE pi_byPageIndex );		
			
			void		SelectItem( BYTE pi_byItemIndex );
			void		DeselectItem( void );

			void		MouseEvent( _mouse_event & event );
};


/*//////////////////////////////////////////////////////////////////////////

[ CGUIItemMakeWindow ] 
 - 아이템 제작 창

//////////////////////////////////////////////////////////////////////////*/
class CGUIItemMakeWindow : public CGUIWindow
{
public:
	enum MakeGroup	// 아이템 제작 항목
	{
		Weapon,		// 무기, 방패
		Armor,		// 방어구	
		Consume,	// 탄환, 화살 등.
		None
	};

// < Data Member > ---------------------------------------------------------
private:	
	CGUIText			m_uiTitle;
	CGUIObjectAni		m_uiAniTitleBar;	
	CGUIButton			m_uiHelpButton;
	CGUIButton			m_uiCloseButton;	

	CGUIButtonEx		m_uiMakeButton;	

	
	
	CGUIItemListWindow	m_uiMakeItemList[3];				// 제작 가능한 항목별 아이템 리스트
	
	
	MakeGroup			m_eCurMakeGroup;


	CGUIText			m_uiMaterialTitle[2];	
	CGUIText			m_uiMaterialStr[5];
	
	BOOL				m_bIsCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUIItemMakeWindow();
	virtual ~CGUIItemMakeWindow();

			void		Init( void );
			void		SetSprite( void );

	// ----------------------------------------------------------------------

	inline	CGUIWindow *	GetItemListWindow( BYTE pi_byMakeGroup  )	{ return &m_uiMakeItemList[pi_byMakeGroup]; }

			
			void			SetMakeGroup( MakeGroup pi_eMakeGroup );						// 제작 항목 설정
	inline	MakeGroup		GetMakeGroup( void ) { return m_eCurMakeGroup; }			
	

			void			SetActiveTab( BYTE pi_byTabIndex );								// 활성화 탭 설정	
			void			SetPage( BYTE pi_byPageIndex );			


			void			SelectItem( BYTE pi_byItemIndex );
			void			DeselectItem( void );	
				

			void			SetMaterialStr( BYTE pi_byIndex, char * pi_pStr, BYTE pi_byNum, BOOL pi_bIsHaving = FALSE );
			void			ClearMaterial( void );


			void			EnableMake( BOOL pi_bEnable );									// 제작 버튼 Enable/Disable

	// ----------------------------------------------------------------------

			void			MouseEvent( _mouse_event & event );
			void			ClosingDialogBox( CGUIDialogBox * pi_pDlg );
};

#endif // ___GUIITEMMAKEWINDOW_H__
