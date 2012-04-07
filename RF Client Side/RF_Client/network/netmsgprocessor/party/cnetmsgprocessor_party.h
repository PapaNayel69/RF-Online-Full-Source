////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Party Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_MSG_PARTY_PROCESSOR_H__
#define __NETWORK_MSG_PARTY_PROCESSOR_H__

#include "../CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Party : public CNetMsgProcessor_Base
{
public :

	CNetMsgProcessor_Party();
	virtual ~CNetMsgProcessor_Party();

				BOOL	SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );
				bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	PartyJoinInvitation_clzo( CPlayer * pi_pPlayer );
	NMP_VIRTUAL	BOOL	PartyJoinInvitationAnswer_clzo( CPlayer * pi_pPlayer );
	NMP_VIRTUAL	BOOL	PartyJoinApplication_clzo( CPlayer * pi_pPlayer );
	NMP_VIRTUAL	BOOL	PartyJoinApplicationAnswer_clzo( CPlayer * pi_pPlayer );
	NMP_VIRTUAL	BOOL	PartyLeaveSelfRequest_clzo( void );
	NMP_VIRTUAL	BOOL	PartyLeaveCompulsionRequest_clzo( DWORD pi_dwIndex );
	NMP_VIRTUAL	BOOL	PartyDisjointRequest_clzo( void );
	NMP_VIRTUAL	BOOL	PartySuccessionRequest_clzo( DWORD pi_dwLeaderIndex );
	NMP_VIRTUAL	BOOL	PartyLockRequest_clzo( BOOL pi_bIsLock );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	PartyJoinInvitationQuestion_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyJoinApplicationQuestion_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyJoinJoinerResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyJoinMemberResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyLeaveSelfResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyLeaveCompulsionResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyDisjointResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartySuccessionResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyLockResult_zocl( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyExpDivision_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	PartyMemberInfo_upd( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyMemberHp_upd( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyMemberFp_upd( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyMemberSp_upd( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyMemberLv_upd( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyMemberPos_upd( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyMemberMaxHfsp_upd( char * pi_pMsg );
	NMP_VIRTUAL	BOOL	PartyMemberEffect_upd( char * pi_pMsg );
};

#endif // __NETWORK_MSG_PARTY_PROCESSOR_H__
