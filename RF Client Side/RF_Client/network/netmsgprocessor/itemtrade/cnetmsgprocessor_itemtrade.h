////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_ItemTrade Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_MSG_ITEM_TRADE_PROCESSOR_H__
#define __NETWORK_MSG_ITEM_TRADE_PROCESSOR_H__

#include "../CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_ItemTrade : public CNetMsgProcessor_Base
{
public:
	CNetMsgProcessor_ItemTrade();
	virtual ~CNetMsgProcessor_ItemTrade();

				BOOL	SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );
				bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	StoreListRequest_clzo( void );
	NMP_VIRTUAL	void	StoreListResult_clzo( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	BuyStoreRequest_clzo( void );
	NMP_VIRTUAL	void	BuyStoreSuccess_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	BuyStoreFailure_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	SellStoreRequest_clzo( void );
	NMP_VIRTUAL	void	SellStoreResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	RepairRequest_clzo( void );
	NMP_VIRTUAL	void	RepairResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	ExchangeDalantForGoldRequest_clzo( DWORD pi_dwDalant );
	NMP_VIRTUAL	BOOL	ExchangeGoldForDalantRequest_clzo( DWORD pi_dwGold );
	NMP_VIRTUAL	void	ExchangeMoneyResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	void	EconomyHistoryInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	EconomyRateInform_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

				void	ShowServerErrorMessage( DWORD pi_dwErrorCode );
};

#endif // __NETWORK_MSG_ITEM_TRADE_PROCESSOR_H__
