////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_ItemArrangement Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#ifndef __NETWORK_MSG_ITEM_ARRANGE_PROCESSOR_H__
#define __NETWORK_MSG_ITEM_ARRANGE_PROCESSOR_H__

#include "../CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_ItemArrangement : public CNetMsgProcessor_Base
{
public :

	CNetMsgProcessor_ItemArrangement();
	virtual ~CNetMsgProcessor_ItemArrangement();

				BOOL	SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );
				bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	PotionsocketSeparationRequest_clzo( void );
	NMP_VIRTUAL	void	PotionsocketSeparationResult_zocl( char * pi_pMsg );	
				void	Item_Devide( WORD pi_wOrgIndex, BYTE pi_byOrgQuantity, WORD pi_wTarIndex, BYTE pi_byTarQuantity );
	NMP_VIRTUAL	BOOL	PotionsocketDivisionRequest_clzo( void );
	NMP_VIRTUAL	void	PotionsocketDivisionResult_zocl( char * pi_pMsg );
				void	Item_Combine( WORD pi_wOrgIndex, BYTE pi_byOrgQuantity, WORD pi_wTarIndex, BYTE pi_byTarQuantity );
	NMP_VIRTUAL	BOOL	PotionsocketChangeRequest_zocl( void );
	NMP_VIRTUAL	void	PotionsocketChangeResult_zocl( char * pi_pMsg );
				void	Item_SocketChange( void );

	NMP_VIRTUAL	BOOL	ResSeparationRequest_clzo( void );
	NMP_VIRTUAL	void	ResSeparationResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	ResDivisionRequest_clzo( void );
	NMP_VIRTUAL	void	ResDivisionResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	AnimusInvenChangeRequest_clzo( void );
	NMP_VIRTUAL	void	AnimusInvenChangeResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	ForceInvenChangeRequest_clzo( void );
	NMP_VIRTUAL	void	ForceInvenChangeResult_zocl( char * pi_pMsg );



				void	ShowServerErrorMessage( DWORD pi_dwErrorCode );
};

#endif // __NETWORK_MSG_ITEM_ARRANGE_PROCESSOR_H__

#endif // _NEW_UI_