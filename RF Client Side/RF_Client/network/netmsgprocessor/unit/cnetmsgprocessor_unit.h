////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Unit Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#ifndef __NETWORK_MSG_UNIT_PROCESSOR_H__
#define __NETWORK_MSG_UNIT_PROCESSOR_H__

#include "../CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Unit : public CNetMsgProcessor_Base
{
public :

	CNetMsgProcessor_Unit();
	virtual ~CNetMsgProcessor_Unit();

				BOOL	SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );
				bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	UnitFrameBuyRequest_clzo( DWORD pi_dwUnitDTIndex );
	NMP_VIRTUAL	void	UnitFrameBuyResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	UnitSellRequest_clzo( BYTE pi_bySlotIndex );
	NMP_VIRTUAL	void	UnitSellResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	UnitPartTuningRequest_clzo( void );
	NMP_VIRTUAL	void	UnitPartTuningResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	UnitBulletFillRequest_clzo( void );
	NMP_VIRTUAL	void	UnitBulletFillResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	UnitPackFillRequest_clzo( void );
	NMP_VIRTUAL	void	UnitPackFillResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	UnitFrameRepairRequest_clzo( void );
	NMP_VIRTUAL	void	UnitFrameRepairResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	UnitDeliveryRequest_clzo( DWORD pi_dwMerchantIndex );
	NMP_VIRTUAL	void	UnitDeliveryResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	UnitReturnRequest_clzo( void );
	NMP_VIRTUAL	void	UnitReturnResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	UnitTakeRequest_clzo( void );
	NMP_VIRTUAL	void	UnitTakeResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	UnitLeaveRequest_clzo( void );
	NMP_VIRTUAL	void	UnitLeaveResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	void	UnitAlterFeeInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	UnitBoosterChargeInform_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	void	UnitDestroyInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	UnitForceReturnInform_zocl( char * pi_pMsg );

				void	ShowServerErrorMessage( DWORD pi_dwErrorCode );
};

#endif // __NETWORK_MSG_UNIT_PROCESSOR_H__

#endif // _NEW_UI_