////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Resource Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_MSG_PROCESSOR_RESOURCE_H__
#define __NETWORK_MSG_PROCESSOR_RESOURCE_H__

#include "../CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Resource : public CNetMsgProcessor_Base
{
public:

	CNetMsgProcessor_Resource();
	virtual ~CNetMsgProcessor_Resource();

				BOOL	SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );
				bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	MineStartRequest_clzo( void );
	NMP_VIRTUAL	void	MineStartResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	MineCancleRequest_clzo( void );
	NMP_VIRTUAL	void	MineCancleResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	MineCompleteRequest_clzo( void );
	NMP_VIRTUAL	void	MineCompleteResult_zocl( char * pi_pMsg );
	
	NMP_VIRTUAL	void	MineStartResultOther_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	MineEndResultOther_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	
	NMP_VIRTUAL	BOOL	OreCuttingRequest_clzo( void );
	NMP_VIRTUAL	void	OreCuttingResult_zocl( char * pi_pMsg );	
	
	NMP_VIRTUAL	BOOL	OreIntoBagRequest_clzo( BYTE pi_byOreDTIndex, BYTE pi_byOreNum, DWORD pi_dwInvenOreSerial );
	NMP_VIRTUAL	void	OreIntoBagResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	CuttingCompleteRequest_clzo( void );
	NMP_VIRTUAL	void	CuttingCompleteResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

};

#endif // __NETWORK_MSG_PROCESSOR_RESOURCE_H__
