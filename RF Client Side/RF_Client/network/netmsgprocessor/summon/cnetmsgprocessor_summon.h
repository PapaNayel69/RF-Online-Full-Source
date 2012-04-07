////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Summon Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#ifndef __NETWORK_MSG_SUMMON_PROCESSOR_H__
#define __NETWORK_MSG_SUMMON_PROCESSOR_H__

#include "../CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Summon : public CNetMsgProcessor_Base
{
public :

	CNetMsgProcessor_Summon();
	virtual ~CNetMsgProcessor_Summon();

				BOOL	SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );
				bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	AnimusRecallRequest_clzo( CAnimusItem * pi_pAnimusItem );
	NMP_VIRTUAL	void	AnimusRecallResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	AnimusReturnRequest_clzo( void );
	NMP_VIRTUAL	void	AnimusReturnResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	AnimusCommandRequest_clzo( BYTE pi_byCommand );
	NMP_VIRTUAL	void	AnimusCommandIinform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	AnimusTargetRequest_clzo( CCharacter * pi_pTarget );
	NMP_VIRTUAL	void	AnimusTargetResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	void	AnimusHpInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	AnimusFpInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	AnimusExpInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	AnimusLvupInform_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

				void	ShowServerErrorMessage( DWORD pi_dwErrorCode );
};

#endif // __NETWORK_MSG_SUMMON_PROCESSOR_H__

#endif // _NEW_UI_