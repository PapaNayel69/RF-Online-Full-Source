////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Combat Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#ifndef __NETWORK_MSG_COMBAT_PROCESSOR_H__
#define __NETWORK_MSG_COMBAT_PROCESSOR_H__

#include "../CNetMsgProcessor_Base.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum CR_CODE { CRC_SUCCESS, CRC_FAIL, CRC_ERROR };

class CCombatResult
{
	friend class CNetMsgProcessor_Combat;

	typedef _CHRID ATTACKER;
	typedef _attack_gen_result_zocl::_dam_list VICTIM;

	CR_CODE		m_crcResult;
	char		m_nServerErrorCode;

	BYTE		m_byAttackType;
	BYTE		m_byAttackPart;
	DWORD		m_dwSFIndex;
	BYTE		m_bySFLevel;
	Vector3f	m_vecSFPosition;
	BYTE		m_byBulletDTIndex;

	ATTACKER	m_stAttacker;

	BYTE		m_byTotalVictim;
	VICTIM		m_listVictim[32];

	BOOL		m_bIsCritical;
	BOOL		m_bIsCounterAttack;

public :

	CCombatResult();

	BOOL	ProcessCombatResult( void );

private :

	BOOL	ProcessAttacker( CCharacter * pi_pAttacker );
	BOOL	ProcessAttacker_ProcessAvatar( CCharacter * pi_pAttacker );
	BOOL	ProcessAttacker_ProcessOther( CCharacter * pi_pAttacker );
	void	ProcessAttacker_ProcessBullet( CCharacter * pi_pAttacker );
	BOOL	ProcessAttacker_ProcessETC( CCharacter * pi_pAttacker );

	void	SetVictimOfAttacker( CCharacter * pi_pAttacker );
	void	SetAttackerOfVictim( CCharacter * pi_pAttacker );

	void	ProcessETC( CCharacter * pi_pAttacker );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_Combat : public CNetMsgProcessor_Base
{
public :

	CNetMsgProcessor_Combat();
	virtual ~CNetMsgProcessor_Combat();

				BOOL	SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize );
				bool	AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg );

	//--------------------------------------------------------------------------

				void	SetAvatarAttackState( BYTE pi_byAttackType, CCharacter * pi_pVictim );

	NMP_VIRTUAL	BOOL	AttackGenRequest_clzo( CCharacter * pi_pVictim );
	NMP_VIRTUAL	void	AttackGenResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	AttackSkillRequest_clzo( CCharacter * pi_pVictim );
	NMP_VIRTUAL	void	AttackSkillResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	AttackForceRequest_clzo( CCharacter * pi_pVictim );
	NMP_VIRTUAL	void	AttackForceResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	AttackUnitRequest_clzo( CCharacter * pi_pVictim );
	NMP_VIRTUAL	void	AttackUnitResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	BOOL	AttackTestRequest_clzo( BYTE pi_byAttackType, CCharacter * pi_pVictim );
	NMP_VIRTUAL	void	AttackTestResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	void	AttackCountResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	void	AttackErrorResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	void	AttackMonsterInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	AttackAnimusInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	AttackTowerInform_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

	NMP_VIRTUAL	BOOL	ModeChangeRequest_clzo( BYTE pi_byCombatMode );
	NMP_VIRTUAL	void	ModeChangeResult_zocl( char * pi_pMsg );

	NMP_VIRTUAL	void	BreakdownEquipItem_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	ShieldEquipDurDec_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	AlterWeaponBulletInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	AlterUnitBulletInform_zocl( char * pi_pMsg );
	NMP_VIRTUAL	void	AlterUnitHpInform_zocl( char * pi_pMsg );

	NMP_VIRTUAL	void	MonsterChaseChange_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------

				void	ShowServerErrorMessage( DWORD pi_dwErrorCode );
};

#endif // __NETWORK_MSG_COMBAT_PROCESSOR_H__

#endif // _NEW_UI_