////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Dungeon Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_MSG_DUNGEON_PROCESSOR_H__
#define __NETWORK_MSG_DUNGEON_PROCESSOR_H__

#include "../CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Dungeon : public CNetMsgProcessor_Base
{
public :

	CNetMsgProcessor_Dungeon();
	virtual ~CNetMsgProcessor_Dungeon();

				BOOL	SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );
				bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	BattledungeonOpenRequest_clzo( DWORD pi_dwItemIndex );
	NMP_VIRTUAL	void	BattledungeonOpenResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	DungeongateCreate_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	DungeongateDestroy_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	void	DungeongateFixpositon_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	DungeongateStateChange_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	GotoDungeonRequest_clzo( CGroundItem * pi_pDungeonItem );
	NMP_VIRTUAL	void	GotoDungeonResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	CloseDungeonInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	OutofDungeonRequest_clzo( void );
	NMP_VIRTUAL	void	OutofDungeonResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	void	KillmonDungeonInform_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

				void	ShowServerErrorMessage( DWORD pi_dwErrorCode );
};

#endif // __NETWORK_MSG_DUNGEON_PROCESSOR_H__
