////////////////////////////////////////////////////////////////////////////////
//
// CPartyBoard Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __PARTY_BOARD_H__
#define __PARTY_BOARD_H__

#include "../Common/CBoard.h"
#include "../../../UserInterface/GameWindow/Group/GUIPartyWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Party Member Type ( PMT_ )
enum PARTY_MEMBER_TYPE
{
	PMT_NONE, PMT_MEMBER, PMT_LEADER,
	MAX_PARTY_MEMBER_TYPE
};

typedef PARTY_MEMBER_TYPE	PM_TYPE;

#define MAX_PLAYER_IN_PARTY	( 8 )

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CPartyMember
{
	DWORD	m_dwMemberIndex;

public :

	CPartyMember();
	CPartyMember( CPlayer * pi_pPlayer );

			void		SetMember( CPlayer * pi_pMember );
			CPlayer *	GetMember( void );

			BOOL		IsEqual( CPlayer * pi_pPlayer );
			BOOL		IsEqual( DWORD pi_dwPlayerIndex );
	inline	DWORD		GetIndex( void )	{ return m_dwMemberIndex; }
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

typedef list< CPartyMember >			PARTY_LIST;
typedef list< CPartyMember >::iterator	PARTY_LIST_ITER;

class CGUIScreen;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Party;

class CPartyBoard : public CBaseBoard
{
	PARTY_LIST		m_listParty;
	CGUIPartyWindow	m_uiPartyWindow;

	DWORD			m_dwApplicantIndex[2];

	BOOL			m_bIsLockOnAddMember;

	CNetMsgProcessor_Party *	m_pNetMsgProcessor;

public :

	CPartyBoard();
	~CPartyBoard();

			void		Clear( void );

	virtual	void		InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );

			BOOL		AddMember( CPlayer * pi_pPlayer );
			BOOL		AddMember( DWORD pi_dwIndex, char * pi_pName );
			BOOL		DeleteMember( DWORD pi_dwIndex );

			BOOL		ChangeLeader( DWORD pi_dwNewLeaderIndex );
			BOOL		ChangeLeaderToNextMember( void );

			void		SettingToLeader( CPlayer * pi_pPlayer );
			void		SettingToMember( CPlayer * pi_pPlayer );
			void		SettingToPlayer( CPlayer * pi_pMember );

protected :

			BOOL		AddMember_CheckCondition( CPlayer * pi_pPlayer );
			PM_TYPE		AddMember_GetMemberType( CPlayer * pi_pPlayer );
			void		AddMember_SetUI( CPlayer * pi_pPlayer );

	//--------------------------------------------------------------------------

public :

			CPlayer *	GetLeader( void );
			CPlayer *	GetMember( DWORD pi_dwIndex );
			BOOL		IsPartyLeader( DWORD pi_dwIndex );
			BOOL		IsPartyMember( DWORD pi_dwIndex );
			void		SetApplicantIndex( CPlayer * pi_pPlayer );
			int			GetTotalMemberNum( void );

			void		FrameMove( void );
			LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//--------------------------------------------------------------------------

			BOOL		InviteOrJoinToParty( void );

protected :

			BOOL		RequestInvitePlayerToMyParty( CPlayer * pi_pPlayer );
			BOOL		AcceptInviteFromOtherParty( DWORD pi_dwPartyLeaderIndex );
			BOOL		RequestJoinToOtherParty( CPlayer * pi_pPlayer );
			BOOL		AcceptJoinToMyParty( DWORD pi_dwPlayerIndex );
			BOOL		ChangeLeader_Request( DWORD pi_dwIndex );
			BOOL		BanishPlayerFromMyParty_Request( DWORD pi_dwIndex );
			BOOL		LeaveFromMyParty_Request( void );
			BOOL		LockOnMyParty_Request( BOOL pi_bIsLock );

			void		SelectMember( DWORD pi_dwMemberIndex );

public :

			BOOL		ChangeLeader_Result( DWORD pi_dwIndex );
			BOOL		BanishPlayerFromMyParty_Result( DWORD pi_dwIndex );
			BOOL		LeaveFromMyParty_Result( DWORD pi_dwIndex );
			BOOL		LockOnMyParty_Result( BOOL pi_bIsLock );

	//--------------------------------------------------------------------------

public :

			void		SetMemberLevel( DWORD pi_dwIndex, BYTE pi_byLevel );
			void		SetMemberMaxHP( DWORD pi_dwIndex, DWORD pi_dwMaxHP );
			void		SetMemberHP( DWORD pi_dwIndex, DWORD pi_dwHP );
			void		SetMemberMaxFP( DWORD pi_dwIndex, DWORD pi_dwMaxFP );
			void		SetMemberFP( DWORD pi_dwIndex, DWORD pi_dwFP );
			void		SetMemberMaxSP( DWORD pi_dwIndex, DWORD pi_dwMaxSP );
			void		SetMemberSP( DWORD pi_dwIndex, DWORD pi_dwSP );
			void		SetMemberPosition( DWORD pi_dwIndex, BYTE pi_byMapIndex, Vector3f pi_vecMemberPosition );
			void		SetMemberSF( DWORD pi_dwIndex, BYTE pi_bySFType, DWORD pi_dwSFIndex, BYTE pi_bySFLevel, BOOL pi_bAdd );
			void		ClearMemberSF( DWORD pi_dwIndex );
			void		ResetPartyMemberAfterMoveMap( void );

			inline		BOOL	IsLockOnAddMember( void )	{ return m_bIsLockOnAddMember; }

protected :

			void		SetMemberSF_ApplySF( CPlayer * pi_pMember, CSFItem * pi_pSFItem, BOOL pi_bAdd );

public :

	//--------------------------------------------------------------------------

	virtual	void		OpenWindow( void );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

extern CPartyBoard * g_pPartyBoard;
#define _GetPartyBoard() g_pPartyBoard

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __PARTY_BOARD_H__