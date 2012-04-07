#ifndef __BOARD_ETC_H__
#define __BOARD_ETC_H__

#include "../Common/CBoard.h"
#include "../../../UserInterface/GameWindow/Mining/GUIMiningWindow.h"
#include "../../../UserInterface/GameWindow/Npc/GUIExchangeWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Mining Request Type ( MRT_ )
#define	MRT_NONE		0
#define	MRT_START		1
#define	MRT_CANCEL		2
#define	MRT_COMPLEATE	3

// 원석 채취 정보
class CMiningBoard : public CBaseBoard
{
friend class CAvatar;
friend class CNetMsgProcessor_Resource;

protected :

	CGUIMiningWindow			m_uiMiningWindow;
	CNetMsgProcessor_Resource *	m_pNetMsgProcessor_Resource;


	Vector3f	m_vecMiningPosition;					// 채굴할 위치
	BYTE		m_byDummyIndex;							// 채굴할 자원 더미의 인덱스

	DWORD		m_dwMiningStartTime;					// 채굴 시작 시간
	DWORD		m_dwMiningEndTime;						// 채굴 종료 시간
	BOOL		m_bIsMining;							// 현재 채굴중인가? ( 내부적 상태 )

	DWORD		m_dwMiningToolIndex;					// 채굴기의 인덱스

	BYTE		m_byBatteryPackNo;						// 배터리의 Pack No
	BYTE		m_byBatterySocketNo;					// 배터리의 Socket No
	DWORD		m_dwBatteryIndex;						// 배터리의 Index

	//BYTE		m_byOrePackNo;							// 채굴한 광물을 넣을 Pack No
	//BYTE		m_byOreSocketNo;						// 채굴한 광물을 넣을 Socket No
	BYTE		m_byOreIndex;							// 채굴할 Ore의 인덱스 ( 사용자가 선택한 )

	BYTE		m_byMiningRequestType;					// 서버에 요청한 채굴 타입	
	

	//--------------------------------------------------------------------------

public :

	CMiningBoard();
	virtual ~CMiningBoard();

			void	InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );

public :
			void	SelectOre( BYTE pi_byOreIndex );
			BOOL	Mining_Request( BOOL pi_bCheckCondition = TRUE );				
			BOOL	Mining_Cancel( void );

			BOOL	Mining_Complete_Request( void );							// 채굴 종료 요청

			BOOL	Mining_Request_By_TargetPos( Vector3f pi_vecTargetPos );	// 채굴 요청
			BOOL	CheckConditionForMining( void );							// 채굴전 체크 사항
			BOOL	CheckItemForMining( void );									// 채굴에 필요한 아이템 체크


	inline	BOOL	IsMining( void )				{ return m_bIsMining; }
			void	SetMining( BOOL pi_bIsMining );

	inline	DWORD	GetBatteryIndex( void )			{ return m_dwBatteryIndex; }

public :
	
	virtual	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
class CNetMsgProcessor_ItemTrade;

class CExchangeBoard : public CBaseBoard
{
protected :

	CGUIExchangeWindow				m_uiExchangeWindow;
	CNetMsgProcessor_ItemTrade *	m_pNetMsgProcessor_ItemTrade;

public :
	CExchangeBoard();
	virtual	~CExchangeBoard();

	void		InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );

public :
	
	BOOL		ExchangeMoney_Request( BYTE pi_byMoneyType, DWORD pi_dwMoney );
	void		ExchangeMoney_Result( DWORD pi_dwDalant, DWORD pi_dwGold );	

	void		ClearExchange( void );

public :
	
	virtual	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};


////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


#endif // BOARD_ETC_H__
