////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Event Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_MSG_PROCESSOR_EVENT_H__
#define __NETWORK_MSG_PROCESSOR_EVENT_H__

#include "../CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Event : public CNetMsgProcessor_Base
{
public:

	CNetMsgProcessor_Event();
	virtual ~CNetMsgProcessor_Event();

				BOOL	SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );
				bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	void	LevelUp_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	Recover_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	MaxHfsp_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	ExpAlter_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	void	ChangeClassCommand_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	SelectClassRequest_clzo( DWORD pi_dwNewClassDTIndex );
	NMP_VIRTUAL	void	SelectClassResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	ChangeClassOther_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	RewardAddItem_zocl( char * pi_pMsg );

};

#endif // __NETWORK_MSG_PROCESSOR_EVENT_H__
