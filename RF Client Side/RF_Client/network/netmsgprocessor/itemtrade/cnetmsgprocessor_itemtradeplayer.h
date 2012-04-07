////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_ItemTradePlayer Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_MSG_ITEM_TRADE_PLAYER_PROCESSOR_H__
#define __NETWORK_MSG_ITEM_TRADE_PLAYER_PROCESSOR_H__

#include "../CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_ItemTradePlayer : public CNetMsgProcessor_Base
{
public:
	CNetMsgProcessor_ItemTradePlayer();
	virtual ~CNetMsgProcessor_ItemTradePlayer();

				BOOL	SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );
				bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	TradeAskRequest_clzo( DWORD pi_dwServerIndex );
	NMP_VIRTUAL	void	TradeAskResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	TradeAskInform_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	TradeAnswerRequest_clzo( void );
	NMP_VIRTUAL	void	TradeAnswerResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	TradeStartInform_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	TradeCancleRequest_clzo( void );
	NMP_VIRTUAL	void	TradeCancleResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	TradeCancleInform_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	TradeLockRequest_clzo( void );
	NMP_VIRTUAL	void	TradeLockResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	TradeLockInform_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	TradeAddRequest_clzo( void );
	NMP_VIRTUAL	void	TradeAddResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	TradeAddInform_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	TradeDelRequest_clzo( void );
	NMP_VIRTUAL	void	TradeDelResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	TradeDelInform_zocl( char * pi_pMsg );
	
	NMP_VIRTUAL	BOOL	TradeBetRequest_clzo( BYTE pi_byMoneyType, DWORD pi_dwAmount );
	NMP_VIRTUAL	void	TradeBetResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	TradeBetInform_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	TradeOkRequest_clzo( void );
	NMP_VIRTUAL	void	TradeOkResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	TradeOkInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	TradeAccomplishInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	TradeCloseInform_zocl( char * pi_pMsg );
	

	//--------------------------------------------------------------------------

};

#endif // __NETWORK_MSG_ITEM_TRADE_PLAYER_PROCESSOR_H__
