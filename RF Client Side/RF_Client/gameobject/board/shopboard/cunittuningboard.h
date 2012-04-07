////////////////////////////////////////////////////////////////////////////////
//
// CUnitTuningBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __UNIT_TUNING_BOARD_H__
#define __UNIT_TUNING_BOARD_H__

#include "../Common/CBoard.h"
#include "../../../UserInterface/GameWindow/Unit/GUIUnitTuningWindow.h"
#include "../../../UserInterface/GameWindow/Unit/GUIUnitStoreWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Unit;

#define MAX_AVATAR_UNIT	( unit_storage_num )		// 아바타가 소지할 수 있는 유닛의 총 갯수

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CUnitStorehouseBoard : public CObjectBoard
{
protected :

	BYTE		m_byCurUnitSocketNo;				// 현재 아바타가 출고하거나 튜닝중인 기갑장비의 소켓 번호
	CUnitItem *	m_pDeliveryUnit;					// 현재 아바타가 출고한 유닛
	CNpc *		m_pMerchant;						// 현재 아바타가 거래하고 있는 NPC

	CGUIUnitStoreWindow		m_uiStorehouseWindow;
	CNetMsgProcessor_Unit *	m_pNetMsgProcessor_Unit;

public :

	CUnitStorehouseBoard();
	virtual ~CUnitStorehouseBoard();

	virtual	void		InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );
	virtual	void		OpenWindow( void );
	virtual	void		CloseWindow( void );

protected :

	virtual	BOOL		Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );
	virtual	BOOL		Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL		Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity );
	virtual	BOOL		Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject );

protected :

	virtual	BOOL		UI_AddRequest_ProcessObject( void ) { return FALSE; }
	virtual	void		UI_AddResult_ProcessObject( void ) { ; }

	virtual	BOOL		UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity ) { return FALSE; }

	virtual	BOOL		UI_SwapRequest_ProcessObject( void ) { return FALSE; }
	virtual	void		UI_SwapResult_ProcessObject( void ) { ; }

	virtual	BOOL		UI_UseRequest_ProcessObject( void ) { return FALSE; }
	virtual	void		UI_UseResult_ProcessObject( void ) { ; }

public :

			BOOL		AddUnit( BYTE pi_bySocketNo, CUnitItem * pi_pUnitItem );
			CUnitItem *	AddUnitByIndex( BYTE pi_bySocketNo, DWORD pi_dwIndex );			// 서버로부터 유닛을 사거나 인벤토리에 있는 유닛 아이템에서 생성시킬 경우 사용
			BOOL		RemoveUnit( BYTE pi_bySocketNo );
			BOOL		RemoveUnitByIndex( DWORD pi_dwIndex );
			BOOL		ReplaceUnit( BYTE pi_bySocketNo, CUnitItem * pi_pNewUnitItem );	// 기존의 아이템을 pi_pUnitItem 으로 교체

			CUnitItem *	SetCurUnit( DWORD pi_dwUnitIndex );
			CUnitItem *	GetUnit( BYTE pi_bySocketNo );
			CUnitItem *	GetUnitByIndex( DWORD pi_dwUnitIndex );
			CUnitItem *	GetCurUnit( void );
			BYTE		GetUnitSocketNo( DWORD pi_dwUnitIndex );
	inline	BYTE		GetCurUnitSocketNo( void )					{ return m_byCurUnitSocketNo; }

	inline	CUnitItem * GetDeliveryUnit( void )						{ return m_pDeliveryUnit; }

			BOOL		UI_DeliveryUnitRequest( DWORD pi_dwUnitIndex );
			void		UI_DeliveryUnitResult( void );
			BOOL		UI_KeepingUnitRequest( void );
			void		UI_KeepingUnitResult( void );

public :

	virtual	void		FrameMove( void );
	virtual	LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CUnitTuningBoard : public CObjectBoard
{
	enum { UTS_NONE, UTS_BEGIN, UTS_PARTS, UTS_BULLET, UTS_BACKPACK_BULLET, UTS_END };

protected :

	CGUIUnitTuningWindow	m_uiTuningWindow;
	CNetMsgProcessor_Unit *	m_pNetMsgProcessor_Unit;

	BYTE		m_byPackNo;							// 튜닝하는 유닛의 인벤토리 위치
	BYTE		m_bySocketNo;

	CUnitItem 	m_clsTuningUnit;					// 최종적으로 튜닝된 유닛 정보
	DWORD		m_dwTuningCost[2];					// 튜닝 비용

	DWORD		m_dwPrevUnitFrame;					// 이전에 아이템 목록을 세팅했던 유닛의 메인프레임 인덱스
	BYTE		m_byPrevLevel;						// 이전에 아이템 목록을 세팅했던 아바타의 레벨
	BYTE		m_byPrevExpertness;					// 이전에 아이템 목록을 세팅했던 아바타의 기갑장비 숙련도

	BYTE		m_byTuningStage;					// 튜닝 단계
	BYTE		m_byTuningBulletSocket;				// 튜닝하려고 선택한 탄환의 소켓 번호

public :

	CUnitTuningBoard();
	virtual ~CUnitTuningBoard();

			void		InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );

protected :

	virtual	BOOL		Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );
	virtual	BOOL		Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL		Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity );
	virtual	BOOL		Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL		UI_AddRequest_ProcessObject( void )		{ return FALSE; }
	virtual	void		UI_AddResult_ProcessObject( void )		{ ; }

	virtual	BOOL		UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity )	{ return FALSE; }

	virtual	BOOL		UI_SwapRequest_ProcessObject( void )	{ return FALSE; }
	virtual	void		UI_SwapResult_ProcessObject( void )		{ ; }

	virtual	BOOL		UI_UseRequest_ProcessObject( void )		{ return FALSE; }
	virtual	void		UI_UseResult_ProcessObject( void )		{ ; }

protected :

			void		ReturnItemToItemBoard( void );

			BOOL		UI_AddMainFrame( void );
			BOOL		UI_RemoveMainFrame( void );
			
			BOOL		UI_SelectPart( BYTE pi_byPackNo, BYTE pi_bySocketNo );
			void		UI_SelectTuningBulletSocket( BYTE pi_bySocketNo );
			BOOL		UI_DeselectPart( BYTE pi_byPackNo, BYTE pi_bySocketNo );

public :

			BOOL		UI_RepairRequest( void );
			void		UI_RepairResult( DWORD pi_dwDurability );
			BOOL		UI_TuningRequest( void );
			void		UI_TuningResult( BOOL pi_bIsSuccessed );

public :

	virtual	void		FrameMove( void );
	virtual	LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	virtual	void		OpenWindow( void );

protected :

			BOOL		SetBuyablePartList( void );
			BOOL		IsEquableUnitPart( BYTE pi_byPackNo, BYTE pi_bySocketNo );
			BOOL		IsEquableUnitPart( UNIT_PART_DATA * pi_pUnitPartData );

			BOOL		IsHaveTuningParts( void );
			BOOL		IsHaveTuningParts_CheckSameItem( CUnitPartsItem * pi_pCurUnitParts, CUnitPartsItem * pi_pTuningParts );

public :

	inline	CUnitItem *	GetTuningUnit( void )					{ return &m_clsTuningUnit; }

	inline	void		SetTuningStage( BYTE pi_byStage )		{ m_byTuningStage = pi_byStage; }
	inline	BYTE		GetTuningStage( void )					{ return m_byTuningStage; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __UNIT_TUNING_BOARD_H__
