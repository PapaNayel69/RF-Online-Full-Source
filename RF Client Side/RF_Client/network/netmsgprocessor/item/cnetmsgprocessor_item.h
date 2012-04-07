////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Item Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_MSG_ITEM_PROCESSOR_H__
#define __NETWORK_MSG_ITEM_PROCESSOR_H__

#include "../CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Item : public CNetMsgProcessor_Base
{
public :

	CNetMsgProcessor_Item();
	virtual ~CNetMsgProcessor_Item();

				BOOL	SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );
				bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg );

	//--------------------------------------------------------------------------
	
	NMP_VIRTUAL	BOOL	EquipPartRequest_clzo( DWORD pi_dwItemIndex );
	NMP_VIRTUAL	void	EquipPartResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	EmbellishRequest_clzo( BYTE pi_byTarBoardType, DWORD pi_dwTarItemIndex, DWORD pi_dwOrgItemIndex );
	NMP_VIRTUAL	void	EmbellishResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	OffPartRequest_clzo( void );
	NMP_VIRTUAL	void	OffPartResult_zocl( char * pi_pMsg );
	
	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	UsePotionRequest_clzo( void );
	NMP_VIRTUAL	void	UsePotionResult_zocl( char * pi_pMsg );	

	NMP_VIRTUAL	BOOL	AddBagRequest_clzo( void );
	NMP_VIRTUAL	void	AddBagResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------	

	NMP_VIRTUAL	BOOL	ItemboxTakeRequest_clzo( DWORD pi_dwGroundItemIndex, DWORD pi_dwInventoryItemIndex );
	NMP_VIRTUAL	void	ItemboxTakeNewResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	ItemboxTakeAddResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	ThrowStorageRequest_clzo( void );
	NMP_VIRTUAL	void	ThrowStorageResult_zocl( char * pi_pMsg );	

	NMP_VIRTUAL	void	ItemboxStateChange_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	DeleteStorageInform_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	MakeItemRequest_clzo( void );
	NMP_VIRTUAL	void	MakeItemResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	
	NMP_VIRTUAL	BOOL	UpgradeItemRequest_clzo( void );
	NMP_VIRTUAL	void	UpgradeItemResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	DowngradeItemRequest_clzo( void );
	NMP_VIRTUAL	void	DowngradeItemResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

				void	ShowServerErrorMessage( DWORD pi_dwErrorCode );
};

#endif // __NETWORK_MSG_ITEM_PROCESSOR_H__
