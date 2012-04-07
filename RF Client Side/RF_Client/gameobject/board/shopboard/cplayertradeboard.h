////////////////////////////////////////////////////////////////////////////////
//
// CPlayerTradeBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __PLAYER_TRADE_BOARD_H__
#define __PLAYER_TRADE_BOARD_H__

#include "../Common/CBoard.h"
#include "../../../UserInterface/GameWindow/Player/GUITradeWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define	MAX_PLAYER_TRADE_ITEM_NUM	( 15 )
#define	TRADE_REQUEST_WAIT_TIME		( 10000 )			// 거래 요청 대기 시간


struct SEND_ITEM
{
	BYTE		m_byBoardType;							// 거래할 Item이 속한 Board
	BYTE		m_byItemPackNo;							// 거래할 Item의 pack number
	BYTE		m_byItemSocketNo;						// 거래할 Item의 socket number
	CBoardItem	m_clsItem;

	SEND_ITEM()	{ Init(); }
	void		Init( void );
};

struct SEND_LIST
{
	BYTE		m_byMaxItemNum;							// 아바타가 상대방에게 줄 수 있는 아이템의 최대 갯수
	BYTE		m_byTotalItemNum;						// 거래하는 아이템의 총 갯수

	SEND_ITEM	m_listItem[MAX_PLAYER_TRADE_ITEM_NUM];

	DWORD		m_dwDalant;								// 거래할 Dalant
	DWORD		m_dwGold;								// 거래할 Gold

	BOOL		m_bIsLock;
	BOOL		m_bIsAccept;

	SEND_LIST() { Init(); }
	void		Init( void );
};

struct RECEIVE_LIST
{
	BYTE		m_byMaxItemNum;							// 아바타가 상대방에게 받을 수 있는 아이템의 최대 갯수
	BYTE		m_byTotalItemNum;						// 거래하는 아이템의 총 갯수

	CBoardItem	m_listItem[MAX_PLAYER_TRADE_ITEM_NUM];	// 거래할 아이템의 정보
	DWORD		m_dwStartIndex;							// 받은 아이템의 시작 인덱스 값.

	DWORD		m_dwDalant;								// 거래할 Dalant
	DWORD		m_dwGold;								// 거래할 Gold

	BOOL		m_bIsLock;
	BOOL		m_bIsAccept;

	RECEIVE_LIST() { Init(); }
	void		Init( void );
};


////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
class CNetMsgProcessor_ItemTradePlayer;


class CPlayerTradeBoard : public CObjectBoard
{
protected :

	CGUITradeWindow		m_uiTradeWindow;
	CNetMsgProcessor_ItemTradePlayer *	m_pNetMsgProcessor_ItemTradePlayer;	


	DWORD				m_dwTradeCharType;					// 거래할 사람
	DWORD				m_dwTradeCharIndex;
	DWORD				m_dwTradeCharServerIndex;			// 거래할 사람의 Server Index

	BOOL				m_bIsTrading;						// 거래중인가?
	BOOL				m_bIsRequestOfAvatar;				// 아이템 트레이드를 요청한 사람이 아바타인가?
	DWORD				m_dwTradeRequestTime;				// 거래를 요청한 시간

	SEND_LIST			m_pSendList;						// 아바타가 상대방에게 주는 아이템
	RECEIVE_LIST		m_pReceiveList;						// 상대방이 아바타에게 주는 아이템

	SEND_ITEM			m_stCurSendItem;					// 현재 상대방에게 주려는 아이템의 정보

	BYTE				m_bySelectSocketNo;					// 트레이드 윈도우에서 아바타가 선택한 번호 or 빈 소켓 번호

	BOOL				m_bTradeByHotKey;					// 핫키에 의한 아이템 선택	

public :

	CPlayerTradeBoard();
	virtual ~CPlayerTradeBoard();

			void		InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );	

			void		Init( void );
			void		ClearTrade( void );

protected :

	virtual	BOOL		Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );
	virtual	BOOL		Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL		Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity );
	virtual	BOOL		Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL		UI_CheckBeforeAdd_CheckEmptySocket( void );
	virtual	BOOL		UI_CheckBeforeAdd_CheckHoverObject( void );
	virtual	BOOL		UI_AddRequest_ProcessObject( void );
	virtual	void		UI_AddResult_ProcessObject( void );

	virtual	BOOL		UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity );
	virtual	BOOL		UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity );

	virtual	BOOL		UI_SwapRequest_ProcessObject( void )		{return FALSE; }
	virtual	void		UI_SwapResult_ProcessObject( void )			{;}

	virtual	BOOL		UI_UseRequest_ProcessObject( void )			{return FALSE; }
	virtual	void		UI_UseResult_ProcessObject( void )			{;}

		
public :	
			BOOL		TradeRequest( void );


			BOOL		SelectItem_Request( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardItem * pi_pItem, BOOL pi_bTradeByHotKey );
			BOOL		SelectItem_Result( BOOL pi_bIsSuccess );
			BOOL		DeselectItem_Request( BYTE pi_bySocketNo );
			BOOL		DeselectItem_Result( BOOL pi_bIsSuccess );

			BOOL		AddReceiveItem( CBoardItem * pi_pItem, BYTE pi_bySocketNo );
			BOOL		RemoveReceiveItem( BYTE pi_bySocketNo );
			void		SetReceiveDalant( DWORD pi_dwDalant );
			void		SetReceiveGold( DWORD pi_dwGold );

			BOOL		RecognizeLock( void );
			BOOL		RecognizeTrade( void );
			BOOL		CompleteTrade( BOOL pi_bIsSuccess, DWORD pi_dwStartIndex );
			BOOL		CancelTrade( void );

			void		ReturnItemToItemBoard( void );
			void		ReturnItemToItemBoard( BYTE pi_bySocketNo );
			BOOL		IsTradingItem( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo );
			CBoardItem*	GetTradeItem( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo );
			CBoardItem*	GetTradeItemByIndex( DWORD pi_dwIndex );

			BYTE		GetEmptySocketForTrade( void );

protected :

			BOOL		SelectItem_Request_CheckCondition( CBoardItem * pi_pItem );
			BOOL		SelectItem_Request_CheckEmptySocket( void );
			BOOL		SelectItem_Request_AddCurSendItem( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardItem * pi_pItem );

			void		SelectItem_Result_AddItem( void );
			void		SelectItem_Result_UnsetOrgItem( void );
			
			void		CompleteTrade_SetSendList( void );
			void		CompleteTrade_SetReceiveList( DWORD pi_dwStartIndex );

public :

			void		SetTradeChar( CCharacter * pi_pCharacter );
			CCharacter*	GetTradeChar( void );
	inline	DWORD		GetTradeCharType( void )				{ return m_dwTradeCharType; }
	inline	DWORD		GetTradeCharIndex( void )				{ return m_dwTradeCharIndex; }
	inline	DWORD		GetTradeCharServerIndex( void )			{ return m_dwTradeCharServerIndex; }

			void		SetTrading( BOOL pi_bFlag );
	inline	BOOL		IsTrading( void )						{ return m_bIsTrading; }

	inline	void		SetRequestOfAvatar( BOOL pi_bFlag )		{ m_bIsRequestOfAvatar = pi_bFlag; }
	inline	BOOL		IsRequestOfAvatar( void )				{ return m_bIsRequestOfAvatar; }
	inline	void		SetTradeRequestTime( DWORD pi_dwTime )	{ m_dwTradeRequestTime = pi_dwTime; }
	inline	DWORD		GetTradeRequestTime( void )				{ return m_dwTradeRequestTime; }

			void		SetMaxSendNum( BYTE pi_byMaxNum );
			void		SetMaxReceiveNum( BYTE pi_byMaxNum );
			void		SetLockSendList( BOOL pi_bFlag );
			void		SetLockReceiveList( BOOL pi_bFlag );
			void		SetAcceptSendList( BOOL pi_bFlag );
			void		SetAcceptReceiveList( BOOL pi_bFlag );

	inline	CBoardItem*	GetSendItem( BYTE pi_bySocketNo )		{ return &m_pSendList.m_listItem[pi_bySocketNo].m_clsItem; }
	inline	CBoardItem*	GetReceiveItem( BYTE pi_bySocketNo )	{ return &m_pReceiveList.m_listItem[pi_bySocketNo]; }
	
	inline	SEND_ITEM *	GetCurSendItem( void )					{ return &m_stCurSendItem; }
	inline	BYTE		GetSelectSocketNo( void )				{ return m_bySelectSocketNo; }	

	inline	BYTE		GetTotalSendItemNum( void )				{ return m_pSendList.m_byTotalItemNum; }
	inline	BYTE		GetMaxRecieveItemNum( void )			{ return m_pReceiveList.m_byMaxItemNum; }

public :

	virtual	LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __PLAYER_TRADE_BOARD_H__