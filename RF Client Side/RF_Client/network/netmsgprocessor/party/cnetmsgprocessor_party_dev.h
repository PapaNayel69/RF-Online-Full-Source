////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Party_Dev Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __NET_MSG_PARTY_PROCESSOR_DEV_H__
#define __NET_MSG_PARTY_PROCESSOR_DEV_H__

#include "CNetMsgProcessor_Party.h"

#ifdef	_DEVELOPER_

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Party_Dev : public CNetMsgProcessor_Party
{
public :

	CNetMsgProcessor_Party_Dev();
	virtual ~CNetMsgProcessor_Party_Dev();

	//--------------------------------------------------------------------------

	BOOL	PartyJoinInvitation_clzo( CPlayer * pi_pPlayer );
	BOOL	PartyJoinInvitationAnswer_clzo( CPlayer * pi_pPlayer );
	BOOL	PartyJoinApplication_clzo( CPlayer * pi_pPlayer );
	BOOL	PartyJoinApplicationAnswer_clzo( CPlayer * pi_pPlayer );
	BOOL	PartyLeaveSelfRequest_clzo( void );
	BOOL	PartyLeaveCompulsionRequest_clzo( DWORD pi_dwIndex );
	BOOL	PartyDisjointRequest_clzo( void );
	BOOL	PartySuccessionRequest_clzo( DWORD pi_dwLeaderIndex );
	BOOL	PartyLockRequest_clzo( BOOL pi_bIsLock );

	//--------------------------------------------------------------------------

	BOOL	PartyJoinInvitationQuestion_zocl( char * pi_pMsg );
	BOOL	PartyJoinApplicationQuestion_zocl( char * pi_pMsg );
	BOOL	PartyJoinJoinerResult_zocl( char * pi_pMsg );
	BOOL	PartyJoinMemberResult_zocl( char * pi_pMsg );
	BOOL	PartyLeaveSelfResult_zocl( char * pi_pMsg );
	BOOL	PartyLeaveCompulsionResult_zocl( char * pi_pMsg );
	BOOL	PartyDisjointResult_zocl( char * pi_pMsg );
	BOOL	PartySuccessionResult_zocl( char * pi_pMsg );
	BOOL	PartyLockResult_zocl( char * pi_pMsg );
	BOOL	PartyExpDivision_zocl( char * pi_pMsg );

	BOOL	PartyMemberInfo_upd( char * pi_pMsg );
	BOOL	PartyMemberHp_upd( char * pi_pMsg );
	BOOL	PartyMemberFp_upd( char * pi_pMsg );
	BOOL	PartyMemberSp_upd( char * pi_pMsg );
	BOOL	PartyMemberLv_upd( char * pi_pMsg );
	BOOL	PartyMemberPos_upd( char * pi_pMsg );
	BOOL	PartyMemberMaxHfsp_upd( char * pi_pMsg );
	BOOL	PartyMemberEffect_upd( char * pi_pMsg );
};

#endif // __NET_MSG_PARTY_PROCESSOR_DEV_H__

#endif // _DEVELOPER_