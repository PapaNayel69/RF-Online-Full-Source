////////////////////////////////////////////////////////////////////////////////
//
// CCharacter Relation Infomation Header 2
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CHARACTER_INFO2_H__
#define __CHARACTER_INFO2_H__

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CEnemyInfo
{
protected :

	DWORD			m_dwEnemy[2];

public :

	CEnemyInfo();

			void			SetEnemy( CCharacter * pi_pEnemy );
			CCharacter *	GetEnemy( void );
	inline	DWORD			GetEnemyCharTypeID( void )	{ return m_dwEnemy[0]; }
	inline	DWORD			GetEnemyCharIndex( void )	{ return m_dwEnemy[1]; }

			BOOL			IsEqual( DWORD pi_dwCharTypeID, DWORD pi_dwIndex );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CAttackerInfo : public CEnemyInfo
{
	BYTE			m_byDamageType_Dist;			// 거리에 따른 데미지 타입 ( Dist = Distance )
	BYTE			m_byDamageType_AT;				// 공격 형태에 따른 데미지 타입 ( AT = Attack Type )
	BYTE			m_byDamageType_CT;				// 데미지가 크리티컬인가? ( CT = Critical )
	CSFItem			m_clSFItem;						// 스킬/포스일 경우 그에 대한 정보를 저장한다.

	BOOL			m_bDamaged;						// 캐릭터가 공격을 받았는지 서버로부터 메세지가 왔는가?
	BOOL			m_bProcessedDamage;				// m_bDamaged가 TRUE로 된 뒤로 데미지 처리를 했는가?

	DWORD			m_dwTotalDamagePoint;			// 현재 캐릭터가 받은 데미지 ( 총 데미지 )
	DWORD			m_dwCurDamagePoint;				// 현재 캐릭터가 받은 데미지 ( 공격자의 1 이펙트 프레임당 데미지 )

	BYTE			m_byEffectFrameNum;				// Attacker 애니메이션의 프레임 갯수

public :

	CAttackerInfo();

	inline	void	SetDamageType_Dist( BYTE pi_byType )		{ m_byDamageType_Dist = pi_byType; }
	inline	BYTE	GetDamageType_Dist( void )					{ return m_byDamageType_Dist; }
	inline	void	SetDamageType_AT( BYTE pi_byType )			{ m_byDamageType_AT = pi_byType; }
	inline	BYTE	GetDamageType_AT( void )					{ return m_byDamageType_AT; }
	inline	void	SetDamageType_CT( BYTE pi_byType )			{ m_byDamageType_CT = pi_byType; }
	inline	BYTE	GetDamageType_CT( void )					{ return m_byDamageType_CT; }
			void	SetSFItem( CSFItem * pi_pSFItem );
	inline	CSFItem*GetSFItem( void )							{ return &m_clSFItem; }

	inline	void	SetDamaged( BOOL pi_bFlag )					{ m_bDamaged = pi_bFlag; }
	inline	BOOL	IsDamaged( void )							{ return m_bDamaged; }
	inline	void	SetProcessedDamage( BOOL pi_bFlag )			{ m_bProcessedDamage = pi_bFlag; }
	inline	BOOL	IsProcessedDamage( void )					{ return m_bProcessedDamage; }

	inline	void	SetTotalDamagePoint( DWORD pi_dwDamage )	{ m_dwTotalDamagePoint = pi_dwDamage; }
	inline	DWORD	GetTotalDamagePoint( void )					{ return m_dwTotalDamagePoint; }
	inline	void	SetCurDamagePoint( DWORD pi_dwDamage )		{ m_dwCurDamagePoint = pi_dwDamage; }
	inline	DWORD	GetCurDamagePoint( void )					{ return m_dwCurDamagePoint; }

	inline	void	SetEffectFrameNum( BYTE pi_byFrameNum )		{ m_byEffectFrameNum = pi_byFrameNum; }
	inline	BYTE	GetEffectFrameNum( void )					{ return m_byEffectFrameNum; }
	inline	void	DecreaseEffectFrameNum( void )				{ --m_byEffectFrameNum; }
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CVictimInfo : public CEnemyInfo
{
	BOOL			m_bAttacked;					// 캐릭터가 공격을 한것에 대한 결과가 서버로부터 메세지가 왔는가?
	DWORD			m_dwAttackPoint;				// 현재 캐릭터가 가한 공격

public :

	CVictimInfo();

	inline	void	SetAttacked( BOOL pi_bFlag )		{ m_bAttacked = pi_bFlag; }
	inline	BOOL	GetAttacked( void )					{ return m_bAttacked; }
	inline	void	SetAttackPoint( DWORD pi_dwPoint )	{ m_dwAttackPoint = pi_dwPoint; }
	inline	DWORD	GetAttackPoint( void )				{ return m_dwAttackPoint; }
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// Enemy Type
enum { ET_ATTACKER, ET_VICTIM, MAX_ENEMY_TYPE };

class CCombatState
{
public :

	//friend	CAttackerInfo;
	//friend	CVictimInfo;

	typedef	list< CEnemyInfo * >			ENEMY_LIST;
	typedef	list< CEnemyInfo * >::iterator	ENEMY_LIST_ITER;

private :

	CCharacter *	m_pOwner;						// 이 구조체를 가지고 있는 캐릭터의 포인터

	BOOL			m_bDead;						// 캐릭터가 죽었는지 서버로부터 메세지가 왔는가? ( 내부상태처리에만 쓰는 것으로 CCharacter 클래스에 있는것과는 다른 것임 )
	DWORD			m_dwDeadTime;					// 캐릭터가 죽은 시간 ( 내부상태처리에만 쓰는 것으로 CCharacter 클래스에 있는것과는 다른 것임 )
	DWORD			m_dwRemainHP;					// 캐릭터가 데미지를 받은 후 남은 HP ( 내부상태처리에만 쓰는 것으로 CCharacter 클래스에 있는것과는 다른 것임 )

	BYTE			m_byAttackType_Dist;			// 공격 타입 - 거리 ( Dist = Distance )
	BYTE			m_byAttackType_AT;				// 공격 타입 - 공격 형태 ( AT = Attack Type )
	BOOL			m_bReceiveAttackResultMsg;		// 공격에 결과에 대한 네트웍 메세지를 받았는가?
	BOOL			m_bAttackError;					// 공격 실패

	ENEMY_LIST		m_listEnemy[MAX_ENEMY_TYPE];
	CEnemyInfo *	m_pCurEnemy[MAX_ENEMY_TYPE];

public :

	CCombatState();
	~CCombatState();

	void			Clear( void );

	CEnemyInfo *	AddEnemyInfo( BYTE pi_byEnemyType, CCharacter * pi_pEnemy, BOOL pi_bIsSettingToCurEnemy );
	void			DeleteEnemyInfo( BYTE pi_byEnemyType, DWORD pi_dwCharTypeID, DWORD pi_dwIndex );
	void			DeleteEnemyInfoByOrder( BYTE pi_byEnemyType, BYTE pi_byOrderNo );
	void			DeleteAllEnemyInfo( BYTE pi_byEnemyType );

	CCharacter *	GetEnemy( BYTE pi_byEnemyType, DWORD pi_dwCharTypeID, DWORD pi_dwIndex );
	CCharacter *	GetEnemyByOrder( BYTE pi_byEnemyType, BYTE pi_byOrderNo );
	CEnemyInfo *	GetEnemyInfo( BYTE pi_byEnemyType, DWORD pi_dwCharTypeID, DWORD pi_dwIndex );
	CEnemyInfo *	GetEnemyInfoByOrder( BYTE pi_byEnemyType, BYTE pi_byOrderNo );
	CCharacter *	GetCurEnemy( BYTE pi_byEnemyType );
	CEnemyInfo *	GetCurEnemyInfo( BYTE pi_byEnemyType );

	void			NotifyDeathToEnemy( BYTE pi_byEnemyType );
	void			ClearMyself( void );

protected :

	void			DeleteEnemyInfo_RealData( BYTE pi_byEnemyType, ENEMY_LIST_ITER & pio_iter );
	void			DeleteEnemyInfo_DeleteDummy( BYTE pi_byEnemyType, DWORD pi_dwCharTypeID, DWORD pi_dwIndex );

	//--------------------------------------------------------------------------

public :

	inline	void	SetOwner( CCharacter * pi_pCharacter )		{ m_pOwner = pi_pCharacter; }

	inline	void	SetDead( BOOL pi_bFlag )					{ m_bDead = pi_bFlag; }
	inline	BOOL	IsDead( void )								{ return m_bDead; }
	inline	void	SetDeadTime( void )							{ m_dwDeadTime = timeGetTime(); }
			BOOL	IsPassedLimitTimeFromDeadMsg( void );
	inline	void	SetRemainHP( DWORD pi_dwHP )				{ m_dwRemainHP = pi_dwHP; }
	inline	DWORD	GetRemainHP( void )							{ return m_dwRemainHP; }

	inline	void	SetAttackType_Dist( BYTE pi_byType )		{ m_byAttackType_Dist = pi_byType; }
	inline	BYTE	GetAttackType_Dist( void )					{ return m_byAttackType_Dist; }
	inline	void	SetAttackType_AT( BYTE pi_byType )			{ m_byAttackType_AT = pi_byType; }
	inline	BYTE	GetAttackType_AT( void )					{ return m_byAttackType_AT; }
	inline	void	SetReceiveAttackResultMsg( BOOL pi_bFlag )	{ m_bReceiveAttackResultMsg = pi_bFlag; }
	inline	BOOL	IsReceiveAttackResultMsg( void )			{ return m_bReceiveAttackResultMsg; }
	inline	void	SetAttackError( BOOL pi_bFlag )				{ m_bAttackError = pi_bFlag; }
	inline	BOOL	IsAttackError( void )						{ return m_bAttackError; }

	inline	int		GetTotalEnemyNum( BYTE pi_byEnemyType )		{ return m_listEnemy[pi_byEnemyType].size(); }

	//--------------------------------------------------------------------------

			void	ProcessInitDamage( void );
			void	ProcessDeath( void );
			void	ProcessDamage( void );

protected :

			void	ProcessDeath_ByMeleeNearDistanceAttack( CAttackerInfo * pi_pAttackerInfo );
			void	ProcessDeath_ByMeleeFarDistanceAttack( CAttackerInfo * pi_pAttackerInfo );

			BOOL	ProcessDamage_SetSupportSkillForceEffect( CAttackerInfo * pi_pAttackerInfo );
			void	ProcessDamage_SetDamage( CAttackerInfo * pi_pAttackerInfo );
			void	ProcessDamage_SetHP( CAttackerInfo * pi_pAttackerInfo );
			void	ProcessDamage_SetDamageEffect( CAttackerInfo * pi_pAttackerInfo );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CCharacterAnimation
{
	CCharacter *	m_pOwner;						// 이 구조체를 가지고 있는 캐릭터의 포인터

	BOOL			m_bPushOut;						// 캐릭터를 뒤로 민 것인가? 밀린 것인가?
	Vector3f		m_vecPushOutPos;				// 캐릭터가 밀려날 위치
	float			m_fPushOut_Distance;			// 캐릭터를 뒤로 밀 거리
	float			m_fPushOut_Angle;				// 캐릭터를 뒤로 밀 방향
	float			m_fPushOut_Speed;				// 캐릭터를 뒤로 밀 속도
	BYTE			m_byPushOutCount;				// push out 횟수

	BOOL			m_bIsRock;						// 캐릭터를 흔들리게 할 것인가?
	DWORD			m_dwStartRockTime;				// 캐릭터가 흔들리기 시작한 시간
	Vector3f		m_vecStartRockPos;				// 캐릭터가 흔들릴때의 위치

	BOOL			m_bStopAnimation;				// 캐릭터가 애니메이션을 멈춰있을 것인가?
	DWORD			m_dwStopAnimationTime;			// 캐릭터가 애니메이션을 멈춰있기 시작한 시간

public :

	CCharacterAnimation();

			void	SetStopAnimation( BOOL pi_fFlag );
			void	SetRockAnimation( BOOL pi_fFlag );

			BOOL	PositionSettingThatIsPushOut( float pi_fPushOutAngle );
			void	PositionCheckThatIsPushOut( void );

	//--------------------------------------------------------------------------

			void	ProcessStopAnimation( void );
			BOOL	ProcessPushOut( Vector3f pio_vecCharPos );
			BOOL	ProcessRock( Vector3f pio_vecCharPos );

protected :

			void	ProcessStopAnimation_Player( void );
			DWORD	ProcessStopAnimation_Player_GetStopAnimationTime( int pi_nMaxAniFrameNum );
			void	ProcessStopAnimation_Monster( void );

	//--------------------------------------------------------------------------

public :

	inline	void	SetOwner( CCharacter * pi_pCharacter )		{ m_pOwner = pi_pCharacter; }

	inline	void	SetPushOut( BOOL pi_bFlag )					{ m_bPushOut = pi_bFlag; }
	inline	BOOL	GetPushOut( void )							{ return m_bPushOut; }
	inline	void	SetPushOutPos( Vector3f pi_vecPos )			{ memcpy( m_vecPushOutPos, pi_vecPos, sizeof( Vector3f ) ); }
	inline	float *	GetPushOutPos( void )						{ return m_vecPushOutPos; }
	inline	void	SetPushOut_Distance( float pi_fDist )		{ m_fPushOut_Distance = pi_fDist; }
	inline	float 	GetPushOut_Distance( void )					{ return m_fPushOut_Distance; }
	inline	void	SetPushOut_Angle( float pi_fAngle )			{ m_fPushOut_Angle = pi_fAngle; }
	inline	float	GetPushOut_Angle( void )					{ return m_fPushOut_Angle; }
	inline	void	SetPushOut_Speed( float pi_fSpeed )			{ m_fPushOut_Speed = pi_fSpeed; }
	inline	float	GetPushOut_Speed( void )					{ return m_fPushOut_Speed; }
	inline	void	SetPushOutCount( BYTE pi_byCount )			{ m_byPushOutCount = pi_byCount; }
	inline	BYTE	GetPushOutCount( void )						{ return m_byPushOutCount; }
	inline	void	IncPushOutCount( void )						{ ++m_byPushOutCount; }
};

typedef CCharacterAnimation CCharAniInfo;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Character Infomation String Type ( CIST_ )
enum
{
	CIST_NAME, CIST_RACE, CIST_APDP, CIST_NETWORK, CIST_HPFP,
	CIST_EFFECT, CIST_ATTACKER_VICTIM,
	CIST_EFFECT_FRAME
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

struct CHARACTER_ETC_INFO
{
	CCharacter *	m_pCharacter;					// 이 구조체를 가지고 있는 캐릭터의 포인터

	DWORD			m_dwReceiveMsgTime;				// 네트웍 메세지를 받은 시간

#ifdef _DEVELOPER_
	char			m_pInfoString[10][256];			// 캐릭터에 대한 정보 - 테스트코드
	DWORD			m_pInfoTime[10];				// 캐릭터에 대한 정보 - 테스트코드
	BYTE			m_byMaxInfoStringWidth;
#endif

	char			m_pChatMsg[256];				// 캐릭터의 채팅 메세지
	DWORD			m_dwReceiveChatMsgTime;			// 캐릭터의 채팅 메세지를 받은 시간
	BYTE			m_byChatMsgType;				// 채팅 메세지 타입

	//--------------------------------------------------------------------------

	CHARACTER_ETC_INFO() { Init(); }
			void	Init( void );

			void	SetReceiveNetMsg( char * pi_pMsg );
	inline	DWORD	GetReceiveNetMsgTime( void )	{ return m_dwReceiveMsgTime; }

			void	SetInfo( int pi_nNo, char * pi_pInfo );
			void	RenderInfo( void );

			void	SetChatMsg( char * pi_pMsg, BYTE pi_byChatMsgType );
	inline	char *	GetChatMsg( void )				{ return m_pChatMsg; }
			void	RenderChatMsg( void );
};

typedef CHARACTER_ETC_INFO CHAR_ETC_INFO;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif __CHARACTER_INFO2_H__
