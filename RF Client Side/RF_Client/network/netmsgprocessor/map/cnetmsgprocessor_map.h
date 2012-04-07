////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Map Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_MSG_MAP_PROCESSOR_H__
#define __NETWORK_MSG_MAP_PROCESSOR_H__

#include "../CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Map : public CNetMsgProcessor_Base
{
public :

	CNetMsgProcessor_Map();
	virtual ~CNetMsgProcessor_Map();

				BOOL	SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );
				bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	MovePotalRequest_clzo( BYTE pi_byPortalNo );
	NMP_VIRTUAL	void	MovePotalResult_zocl( char * pi_pMsg );
};

#endif // __NETWORK_MSG_MAP_PROCESSOR_H__
