#ifndef ___GUIUNITTUNINGWINDOW_H__
#define ___GUIUNITTUNINGWINDOW_H__


#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIItem.h"
#include "../../GUIController/GUIText.h"
#include "../../GUIController/GUIButton.h"
#include <vector>


#define	BUY_ITEM_TAB_NUM		6
#define	ITEM_BOARD_NUM			5
#define	BACK_PACK_BULLET_NUM	8	

struct PARTS_INFO
{
	DWORD		dwPartsID;

	CGUIItem 	uiPartsIcon;
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUIUnitTuningWindow ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIUnitTuningWindow : public CGUIWindow
{
public :
	typedef	vector< PARTS_INFO * >			CPartsList;

	enum PartsType
	{
		Parts_Head,
		Parts_Upper,
		Parts_Lower,
		Parts_Arms,
		Parts_Shoulder,
		Parts_Back,	

		TOTAL_PARTS_TYPE_NUM
	};

	enum BulletType
	{
		Bullet_Arms,
		Bullet_Shoulder,

		BULLET_TYPE_NUM
	};
	enum { BULLET_ICON_TYPE = TOTAL_PARTS_TYPE_NUM };

	enum { PARTS_TAB, BULLET_TAB, TOTAL_TAB_NUM };

// < Data Member > ---------------------------------------------------------
private :

	CGUIObject			m_uiUpperBase;

	CGUIText			m_uiTitle;
	CGUIObjectAni		m_uiAniTitleBar;
	CGUIButton			m_uiCloseButton;
	CGUIButton			m_uiHelpButton;


	CGUIItem			m_uiMainFrameKey;
	CGUIButtonEx		m_uiRepairButton;
	

	CGUIText			m_uiMainFrameInfoTitle[4];	
	CGUIText			m_uiMainFrameInfo[4];	

	CGUIText			m_uiWeaponTitle;
	CGUIText			m_uiWeaponInfoTitle[3];
	CGUIText			m_uiWeaponInfo[3];
	
	CGUIText			m_uiAddWeaponTitle;
	CGUIText			m_uiAddWeaponInfoTitle[3];
	CGUIText			m_uiAddWeaponInfo[3];	
	
	// -------------------------------------------------------------------
	CGUIContainer		m_uiLowerBase;
	
	CGUIButtonEx		m_uiTab[TOTAL_TAB_NUM];
	BYTE				m_byCurTabIndex;

	CGUIObject			m_uiActiveTabBoard;
	CGUIText			m_uiActiveTabName;
	CGUIObject			m_uiBuyItemTab[BUY_ITEM_TAB_NUM];
	CGUISocket			m_uiBuyItemBoard[ITEM_BOARD_NUM];
	CGUIButton			m_uiPageLeftButton;
	CGUIButton			m_uiPageRightButton;				
	
	BYTE				m_byTempBuyTabIndex[2];	
	BYTE				m_byCurBuyTabIndex;

	// ---------------------------------------------------------------------
	// parts tab board
	CGUIContainer		m_uiPartsBoard;
	CGUIItem			m_uiTuningParts[TOTAL_PARTS_TYPE_NUM];
	CGUIObject			m_uiEmptyPartsFlag[TOTAL_PARTS_TYPE_NUM];
	CGUIObject			m_uiBuyingPartsFlag[TOTAL_PARTS_TYPE_NUM];
	CGUIObject			m_uiSelectedParts;

	CPartsList			m_listBuyParts[TOTAL_PARTS_TYPE_NUM];
	BYTE				m_byBuyPartsPageStartIndex[TOTAL_PARTS_TYPE_NUM];


	// ---------------------------------------------------------------------
	// bullet tab board
	CGUIContainer		m_uiBulletBoard;
	
	// bullet
	CGUIItem			m_uiTuningBullet[BULLET_TYPE_NUM];
	CGUIObject			m_uiEmptyBulletFlag[BULLET_TYPE_NUM];
	CGUIObject			m_uiBuyingBulletFlag[BULLET_TYPE_NUM];
	CGUIObject			m_uiSelectedBullet;
	
	// back pack bullet
	CGUIItem			m_uiTuningBackPackBullet[BACK_PACK_BULLET_NUM];	
	CGUIObject			m_uiEmptyBackPackBulletFlag[BACK_PACK_BULLET_NUM];
	CGUIObject			m_uiBuyingBackPackBulletFlag[BACK_PACK_BULLET_NUM];	
	CGUIText			m_uiBackPackBulletTitle;

	CPartsList			m_listBuyBullet[BULLET_TYPE_NUM];
	BYTE				m_byBuyBulletPageStartIndex[BULLET_TYPE_NUM];

	// ---------------------------------------------------------------------

	CGUIText			m_uiTuningCostTitle;
	CGUIText			m_uiTuningCost;

	CGUIButtonEx		m_uiTuningButton;
	CGUIButtonEx		m_uiCancelButton;
	

	BOOL				m_bIsCreated;


// < Member Function > -----------------------------------------------------
public :
	CGUIUnitTuningWindow();
	virtual ~CGUIUnitTuningWindow();	

	void	Init( void );
	void	InitPartsBoard();
	void	InitBulletBoard();

	void	SetSprite( void );

	// ---------------------------------------------------------------------------
	// 메인프레임키
			void		TakeInMainFrameKey( DWORD pi_dwItemID, DWORD pi_dwSpriteID );
			void		TakeOutMainFrameKey( void );
			
			void		SetMainFrameName( char * pi_pMainFrameName );						// 메인프레임 설정
			void		SetDefPower( int pi_nDefPower );
			void		SetDurability( int pi_nDurability );
			//void		SetMovePower( int pi_nMovePower );
			void		SetMovePower( char * pi_pSpeedStirng );

			void		SetWeaponName( char * pi_pWeaponName );								// 기본 무기 설정
			void		SetWeaponAttPower( int pi_nMinPower, int pi_nMaxPower );
			void		SetBulletNum( int pi_nRestBulletNum, int pi_nTotalBulletNum );

	
			void		SetAddWeaponName( char * pi_pWeaponName );							// 추가 무기 설정
			void		SetAddWeaponAttPower( int pi_nMinPower, int pi_nMaxPower );
			void		SetAddBulletNum( int pi_nRestBulletNum, int pi_nTotalBulletNum );

	// ---------------------------------------------------------------------------
	// 장착할 소켓 선택
			void		Select_Parts( BYTE pi_bySocketNo );
			void		Select_Bullet( BYTE pi_bySocketNo ); // backpack socket은 2부터

	// parts 장착
			void		Equip_Parts( BYTE pi_bySocketNo, DWORD pi_dwSpriteID, BOOL pi_bIsForBuying );
			void		UnEquip_Parts( BYTE pi_bySocketNo );
			void		SetPartsBuyingState( BYTE pi_bySocketNo, BOOL pi_bState );

	// bullet 장착
			void		Equip_Bullet( BYTE pi_bySocketNo, DWORD pi_dwSpriteID, BOOL pi_bIsForBuying );
			void		UnEquip_Bullet( BYTE pi_bySocketNo );
			void		Enable_Shouler_Bullet( BOOL pi_bIsEnabled );
			void		SetBulletBuyingState( BYTE pi_bySocketNo, BOOL pi_bState );
			
	// backpack bullet 장착
			void		Equip_BackPack_Bullet( BYTE pi_bySocketNo, DWORD pi_dwSpriteID, BOOL pi_bIsForBuying );
			void		UnEquip_BackPack_Bullet( BYTE pi_bySocketNo );
			void		Enable_BackPack_Bullet( BYTE pi_byEnableNum );
			void		SetBackpackBulletBuyingState( BYTE pi_bySocketNo, BOOL pi_bState );

	// ---------------------------------------------------------------------------

	inline	CGUIItem *	GetMainFrameKey( void )					{ return &m_uiMainFrameKey; }
	inline	CGUIItem *	GetParts( BYTE pi_byPartsType )			{ if( pi_byPartsType >= TOTAL_PARTS_TYPE_NUM ) return NULL; else return &m_uiTuningParts[pi_byPartsType]; }
	inline	CGUIItem *	GetBullet( BYTE pi_byBulletType )		{ if( pi_byBulletType >= BULLET_TYPE_NUM ) return NULL; else return &m_uiTuningBullet[pi_byBulletType]; }
	inline	CGUIItem *	GetBackPackBullet( BYTE pi_bySocketNo )	{ if( pi_bySocketNo >= BACK_PACK_BULLET_NUM ) return NULL; else return &m_uiTuningBackPackBullet[pi_bySocketNo]; }
			CGUIItem *	GetBuyParts( BYTE pi_byPartsTabIndex, DWORD pi_dwPartsID ); // bullet도 포함

	// ---------------------------------------------------------------------------

			void		SetTab( BYTE pi_byTabIndex );

	// 튜닝 목록 추가
			void		InsertBuyTuningParts( BYTE pi_byPartsTabIndex, DWORD pi_dwPartsID, DWORD pi_dwSpriteID );
			void		RemoveBuyTuningParts( BYTE pi_byPartsTabIndex, DWORD pi_dwPartsID );

			void		ClearBuyTuningPartsList( BYTE pi_byPartsTabIndex );
			void		ClearBuyBulletList( BYTE pi_byBulletTabIndex );
	
			void		SetBuyItemTab( BYTE pi_byTabIndex, BOOL pi_bUpdate = FALSE );

			void		SetBuyPage( BYTE pi_byPageIndex );

	
			void		SetTuningCost( DWORD pi_dwCost );		// 튜닝 비용

	// ---------------------------------------------------------------------------

			void		MouseEvent( _mouse_event &event );

			void		SetRepairButtonState( BOOL pi_bFlag );
			void		SetTuningButtonState( BOOL pi_bFlag );
			void		SetCancelButtonState( BOOL pi_bFlag );

			BOOL		Draw( void );
};

#endif // ___GUIUNITTUNINGWINDOW_H__
