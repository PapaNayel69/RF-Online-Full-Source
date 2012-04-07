////////////////////////////////////////////////////////////////////////////////
//
// CBoardManager Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __BOARD_MANAGER_H__
#define __BOARD_MANAGER_H__

#include "../../../DefineMessage.h"

#include "CBoard.h"
#include "CLinkBoard.h"

#include "../ItemBoard/CInventoryBoard.h"
#include "../ItemBoard/CBeltBoard.h"
#include "../ItemBoard/CEquipBoard.h"
#include "../ItemBoard/COreProcBoard.h"
#include "../ItemBoard/CItemWorkBoard.h"
#include "../ItemBoard/CItemUpgradeBoard.h"
#include "../ShopBoard/CUnitTuningBoard.h"
#include "../ShopBoard/CPlayerTradeBoard.h"
#include "../ShopBoard/CShopTradeBoard.h"
#include "../SkillForceBoard/CSkillBoard.h"
#include "../SkillForceBoard/CForceBoard.h"
#include "../SkillForceBoard/CAnimusBoard.h"
#include "../General/CBoard_Etc.h"
#include "../EtcBoard/CClassChangeBoard.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Item Operation Type
enum { IOT_SET, IOT_UNSET, IOT_SWAP, IOT_USE, IOT_DROP };

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CBoardManager
{
	friend CObjectBoard;

private :
	
	CBaseBoard *	m_listBoard[MAX_BOARD_TYPE];

	BYTE			m_byOperationType;

	BYTE			m_byOrgBoardType;
	BYTE			m_byOrgPackNo;
	BYTE			m_byOrgSocketNo;
	CBoardObject *	m_pOrgObject;

	BYTE			m_byTarBoardType;
	BYTE			m_byTarPackNo;
	BYTE			m_byTarSocketNo;
	CBoardObject *	m_pTarObject;

	CBoardObject *	m_pHoverObject;

	//CBoardItem *	m_pPutDownItem;					// 버릴 아이템
	CBoardItem *	m_pDeletedHoverItem;			// 제거될 hover item일 경우 제거후 결과처리를 위해 backup해둔다


	BOOL			m_bIsHold;						// 모든 Board 사용 불가
	BOOL			m_bIsRequestProcess;			// 서버에 처리를 요청했는가?
	BOOL			m_bIsRequestProcessTime;		// 서버에 처리를 요청한 시간
	BOOL			m_bMoveByHotKey;				// 단축키를 통한 오브젝트 이동인가?

	CSFItem *		m_pSelectSF;					// 현재 UI에서 선택한 스킬/포스
	int				m_nSelectSFCountInUI;			// 현재 UI에서 동일한 스킬/포스를 선택한 횟수
	CCharacter *	m_pSFTarget;					// 시전 대상
	BOOL			m_bIsSFEventInLinkBoard;

	BOOL			m_bIsOpenedEquipWindow;			// 거래창을 열기전에 인벤토리가 열려 있었는가?
	BOOL			m_bIsOpenedPackWindow;

public :

	CBoardManager();
	virtual ~CBoardManager();

			void			Create( void );
			void			FrameMove( void );
			LRESULT			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

			CBaseBoard *	GetBoard( BYTE pi_byBoardType );

			BOOL			AddObject( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pTarObject );
			BOOL			RemoveObject( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, BYTE pi_byQuantity = 0xFF );
			BOOL			RemoveObjectByIndex( DWORD pi_dwIndex, BYTE pi_byQuantity = 0xFF );
			BYTE			RemoveHoverObjectQuantity( BYTE pi_byQuantity );
			CBoardObject *	GetObject( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo );
			CBoardObject *	GetObjectByIndex( DWORD pi_dwIndex );
			CBoardObject *	GetObjectByIndex( BYTE pi_byBoardType, DWORD pi_dwIndex );
			
			BOOL			AddObjectByUIPackNo( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pObject ) { return FALSE; }
			BOOL			RemoveObjectByUIPackNo( BYTE pi_byPackNo, BYTE pi_bySocketNo, BYTE pi_byQuantity = 0xFF ) { return FALSE; }
			CBoardObject *	GetObjectByUIPackNo( BYTE pi_byPackNo, BYTE pi_bySocketNo ) { return NULL; }
	inline	CBoardItem *	GetItemByUIPackNo( BYTE pi_byPackNo, BYTE pi_bySocketNo ) { return static_cast< CBoardItem * >( GetObjectByUIPackNo( pi_byPackNo, pi_bySocketNo ) ); }
			BYTE			GetItemBoardTypeByUIPackNo( BYTE pi_byPackNo, BYTE pi_bySocketNo ) { return 0xFF; }			

			BOOL			ConvertBoardObject( CBoardObject * po_pTarObject, CBoardObject * pi_pOrgObject );

public :

			void			ClearOperation( void );
	inline	void			SetOperationType( BYTE pi_byType )			{ m_byOperationType = pi_byType; }
	inline	BYTE			GetOperationType( void )					{ return m_byOperationType; }
	inline	BOOL			IsEqualOperationType( BYTE pi_byType )		{ return ( m_byOperationType == pi_byType ); }

	inline	BYTE			GetOrgBoardType( void )						{ return m_byOrgBoardType; }
	inline	BYTE			GetOrgPackNo( void )						{ return m_byOrgPackNo; }
	inline	BYTE			GetOrgSocketNo( void )						{ return m_byOrgSocketNo; }
			void			SetOrgObject( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pObject );
	inline	void			SetOrgObject( CBoardObject * pi_pObject )	{ m_pOrgObject = pi_pObject; }
	inline	CBoardObject *	GetOrgObject( void )						{ return m_pOrgObject; }

	inline	BYTE			GetTarBoardType( void )						{ return m_byTarBoardType; }
	inline	BYTE			GetTarPackNo( void )						{ return m_byTarPackNo; }
	inline	BYTE			GetTarSocketNo( void )						{ return m_byTarSocketNo; }
			void			SetTarObject( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pObject );
	inline	void			SetTarObject( CBoardObject * pi_pObject )	{ m_pTarObject = pi_pObject; }
	inline	CBoardObject *	GetTarObject( void )						{ return m_pTarObject; }

	inline	BOOL			HaveHoverObject( void )						{ return ( ( m_pHoverObject ) ? TRUE : FALSE ); }
			CBoardObject *	SetHoverObject( CBoardObject * pi_pObject );
	inline	CBoardObject *	GetHoverObject( void )						{ return m_pHoverObject; }
	inline	CBoardItem *	GetHoverItem( void )						{ return static_cast< CBoardItem * >( m_pHoverObject ); }
			void			ReturnHoverObject( void );

	inline	CBoardItem *	GetDeletedHoverItem( void )					{ return ( m_pDeletedHoverItem->IsEmpty() ? NULL : m_pDeletedHoverItem ); }

public :

	inline	BOOL			IsEnableProcess( void )						{ return ( !m_bIsHold && !m_bIsRequestProcess ); }

	inline	void			SetHoldState( BOOL pi_bFlag )				{ m_bIsHold = pi_bFlag; }
	inline	BOOL			IsHold( void )								{ return m_bIsHold; }
			void			SetRequestProcessState( BOOL pi_bFlag );
	inline	BOOL			IsRequestProcess( void )					{ return m_bIsRequestProcess; }
	inline	void			SetMoveByHotKeyState( BOOL pi_bFlag )		{ m_bMoveByHotKey = pi_bFlag; }
	inline	BOOL			IsMoveByHotKey( void )						{ return m_bMoveByHotKey; }

	inline	void			SelectSF( CSFItem * pi_pSFItem )			{ m_pSelectSF = pi_pSFItem; }
	inline	void			DeselectSF( void )							{ m_pSelectSF = NULL; m_nSelectSFCountInUI = 0; }
	inline	CSFItem *		GetSelectSF( void )							{ return m_pSelectSF; }
	inline	void			SetSelectSFCountInUI( int pi_nCnt )			{ m_nSelectSFCountInUI = pi_nCnt; }
	inline	int				GetSelectSFCountInUI( void )				{ return m_nSelectSFCountInUI; }
	inline	BOOL			IsSFEventInLinkBoard( void )				{ return m_bIsSFEventInLinkBoard; }
	inline	void			SetSFEventInLinkBoard( BOOL pi_bFlag )		{ m_bIsSFEventInLinkBoard = pi_bFlag; }
	inline	CCharacter *	GetSFTarget( void )							{ return m_pSFTarget; }
	inline	void			SetSFTarget( CCharacter * pi_pTarget )		{ m_pSFTarget = pi_pTarget; }

public :

			BOOL			PutDownItemRequest( void );
			void			PutDownItemResult( void );

			void			CloseUnnecessaryWindow( void );
			void			OpenWindow( DWORD pi_dwWindowID ) { ; }
			void			CloseWindow( DWORD pi_dwWindowID ) { ; }
			BOOL			GetWindowPos( POINT & po_ptWindowPos, DWORD pi_dwWindowID, BYTE pi_bySocketNo, BOOL pi_bIsCenterPos )  { return TRUE; }
			BOOL			GetWindowPosByItem( POINT & po_ptWindowPos, DWORD pi_dwWindowID, BYTE pi_byItemType, BOOL pi_bIsCenterPos ) { return TRUE; }

			BOOL			ReturnItemToItemBoard( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardItem * pi_pItem );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

extern CBoardManager g_clsBoardManager;
#define _GetGBM() ( &g_clsBoardManager )	// GBM = global board manager

//------------------------------------------------------------------------------

#define _GetBoard( x )				_GetGBM()->GetBoard( x )
#define _GetInventoryBoard()		( static_cast< CInventoryBoard * >( _GetGBM()->GetBoard( IBT_INVENTORY ) ) )
#define _GetAnimusBoard()			( static_cast< CAnimusBoard * >( _GetGBM()->GetBoard( IBT_ANIMUS ) ) )
#define _GetLinkBoard()				( static_cast< CLinkBoard * >( _GetGBM()->GetBoard( IBT_LINK ) ) )
#define _GetAnimusControlBoard()	( static_cast< CAnimusControlBoard * >( _GetGBM()->GetBoard( IBT_ANIMUS_CONTROL ) ) )
#define _GetUnitTuningBoard()		( static_cast< CUnitTuningBoard * >( _GetGBM()->GetBoard( IBT_UNIT_TUNING ) ) )
#define _GetUnitStorehouseBoard()	( static_cast< CUnitStorehouseBoard * >( _GetGBM()->GetBoard( IBT_UNIT_STOREHOUSE ) ) )
#define	_GetBeltBoard()				( static_cast< CBeltBoard * >( _GetGBM()->GetBoard( IBT_ITEM_BELT ) ) )
#define	_GetEquipBoard()			( static_cast< CEquipBoard * >( _GetGBM()->GetBoard( IBT_EQUIP ) ) )
#define	_GetSkillBoard()			( static_cast< CSkillBoard * >( _GetGBM()->GetBoard( IBT_SKILL ) ) )
#define	_GetForceBoard()			( static_cast< CForceBoard * >( _GetGBM()->GetBoard( IBT_FORCE ) ) )
#define	_GetShopTradeBoard()		( static_cast< CShopTradeBoard * >( _GetGBM()->GetBoard( IBT_SHOP ) ) )
#define	_GetPlayerTradeBoard()		( static_cast< CPlayerTradeBoard * >( _GetGBM()->GetBoard( IBT_PLAYER_TRADE ) ) )
#define	_GetOreProcBoard()			( static_cast< COreProcBoard * >( _GetGBM()->GetBoard( IBT_ORE_PROCESSING ) ) )
#define	_GetItemWorkBoard()			( static_cast< CItemWorkBoard * >( _GetGBM()->GetBoard( IBT_ITEM_WORK ) ) )
#define	_GetItemUpgradeBoard()		( static_cast< CItemUpgradeBoard * >( _GetGBM()->GetBoard( IBT_ITEM_UPGRADE ) ) )
#define	_GetExchangeBoard()			( static_cast< CExchangeBoard * >( _GetGBM()->GetBoard( IBT_EXCHANGE ) ) )
#define	_GetMiningBoard()			( static_cast< CMiningBoard * >( _GetGBM()->GetBoard( IBT_MINING ) ) )
#define	_GetClassChangeBoard()		( static_cast< CClassChangeBoard * >( _GetGBM()->GetBoard( IBT_CLASS_CHANGE ) ) )

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CGlobalGameInformation
{
	BOOL	m_bIsRequestProcess;
	BOOL	m_dwRequestProcessTime;

public :

	CGlobalGameInformation();
	virtual ~CGlobalGameInformation();

			void	Init( void );

			void	SetRequestProcessState( BOOL pi_bFlag );
	inline	BOOL	IsRequestProcess( void )			{ return m_bIsRequestProcess; }
};

extern CGlobalGameInformation g_clGameInformation;
#define _GetGGI() ( &g_clGameInformation )

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __BOARD_MANAGER_H__
