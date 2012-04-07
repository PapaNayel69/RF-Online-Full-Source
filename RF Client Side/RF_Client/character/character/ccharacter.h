////////////////////////////////////////////////////////////////////////////////
//
// CCharacter Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "../../CResObject.h"
#include "../../CPath.h"

#include "../../DataTable/CCharacterDataMgr.h"
#include "../../DataTable/CItemDataMgr.h"
#include "../../Item/CItemData.h"

#include "CCharacter_RelationInfo.h"
#include "CCharacter_RelationInfo2.h"

#include "../../AddCodeByJang/CSwordTrace.h"	// By J.U.I

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// push out Mode
#define CIPO_PUSHED_OUT_FOR_DEATH		( 1 )		// 캐릭터가 죽을 경우 뒤로 밀려남
#define CIPO_PUSHED_OUT_FOR_DAMAGED		( 2 )		// 캐릭터가 일반 공격을 받아서 뒤로 밀려남
#define CIPO_PUSHED_OUT_FOR_SKILL		( 3 )		// 캐릭터가 스킬 공격을 받아서 뒤로 밀려남
#define CIPO_PUSH_OUT_FOR_SKILL			( 4 )		// 캐릭터가 스킬 공격시, 앞으로 전진


//MAYUN
#define MAX_PARTS_RESULT	10
#define MAX_EFFECT_DUMMYS	( 10 )
//
////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*//////////////////////////////////////////////////////////////////////////////

[ CCharacter ]

//////////////////////////////////////////////////////////////////////////////*/

class CCharacter : public CResObject
{
	friend class CPlayerProc;
	friend class CUnitProc;

protected :

	DWORD			m_dwCharTypeID;					// 캐릭터 타입
	DWORD			m_dwDTIndex;					// 서버 테이터 테이블의 레코드 인덱스

	DWORD			m_dwActionID;					// 걷기, 뛰기, 공격하기, etc...
	DWORD			m_dwPrevActionID;				// 이전 액션 ID - by J.U.I
	DWORD			m_dwActionTime;					// 캐릭터가 어떠한 행동을 취한 시간
	DWORD			m_dwIdleTime;					// Idle
	DWORD			m_dwDeadTime;					// 캐릭터가 죽었을때의 시간

	//--------------------------------------------------------------------------

	BYTE			m_byCombatMode;					// 전투 모드 ( Peace = 0, Combat = 1 )
	BYTE			m_byPrevCombatMode;				// 이전 전투 모드

	BYTE			m_byWalkMode;					// 움직임 모드 : 걷기, 뛰기, 날기
	BYTE			m_byPrevWalkMode;				// 이전의 걷기 모드
	float			m_fSpeed[2];					// 캐릭터의 실제 움직임 속도 ( [0] = walk, [1] = run )
	float			m_fMagnificationOfSpeed;		// 캐릭터의 움직임 속도의 배율 (default = 1.0f) - 속도 보정을 위해 쓰임

	//--------------------------------------------------------------------------

	DWORD			m_dwMaxHP;
	DWORD			m_dwHP;
	DWORD			m_dwMaxFP;
	DWORD			m_dwFP;

	//--------------------------------------------------------------------------

	CPath			m_cPathStack;					// 캐릭터가 이동할 Path를 저장
	BOOL			m_bIsFirstMoving;				// 처음 움직임인가?
	Vector3f		m_vecNextPath;					// 캐릭터가 다음으로 이동할 패쓰의 위치

	//--------------------------------------------------------------------------

	DWORD			m_dwCurFrameMoveTime;
	DWORD			m_dwPrevFrameMoveTime;
	BOOL			m_bIsPassedFrameMoveLimitTime;

	//--------------------------------------------------------------------------

	float			m_fDistanceBetweenAvatar;		// 아바타와의 거리
	BYTE			m_byAnimationSkipTotalFrame;	// 스킵해야할 애니메이션의 총 프레임
	BYTE			m_byAnimationSkipCurFrame;		// 현재까지 스킵한 애니메이션의 프레임
	DWORD			m_dwAnimationSkipTime;

	//--------------------------------------------------------------------------

	CSFItem			m_clSFItem;						// 캐릭터가 현재 선택한 스킬/포스

	APPLIED_SF		m_stAppliedSF;					// 현재 캐릭터에 적용된 스킬과 포스에 대한 정보
	CHAR_COND_INFO	m_stConditionInfo;				// 캐릭터의 상태 정보
	CCombatState *	m_pCombatState;					// 캐릭터의 전투 상태 정보
	CCharAniInfo *	m_pCharAniInfo;					// 캐릭터의 애니메이션 정보
	CHAR_ETC_INFO	m_stETCInfo;					// 캐릭터의 기타 정보

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//

	// by J.U.I
	BYTE			m_bCheckExistEF;					// 리소스에 붙는 효과가 있는지 체크
	DWORD			m_dwEFUniqueID;						// 리소스에 붙는 효과 unique코드
	BYTE			m_byDummy[MAX_EFFECT_DUMMYS];		// 효과 더미 오브젝트 인덱스들... 이걸로 특수 이펙트 위치를 잡는다.
	BYTE			m_bySkillAtackCnt;					// 스킬 몇번째 타격인지 알아내기.
	float			m_fDamageTimeForShake;				// 장언일 추가 맞을때 허리제끼기 타임.

	DWORD			m_dw2ndColor;						//2번째 라이팅 컬러.
	float			m_f2ndDensity;						//2번째 라이팅 강도 0일경우는 라이팅처리가 없다.
	Vector3f		m_vec2ndLgtPos;						//2번째 라이팅 포지션.
	float			m_f2ndRadius;						//2번째 라이팅 반지름.

	// by MAYUN
	BOOL			m_bBlend;							// 버텍스 블렌드 오브젝트인가?
	D3DXMATRIX *	m_pMatResMat[MAX_PARTS_RESULT][2];	// Matrix Result List,15개의 파트는 충분히 크다고 가정한다. 
	DWORD			m_dwMaxResult[MAX_PARTS_RESULT];	// Max Result

public :

	CCharacter();
	virtual ~CCharacter();

	virtual	CCharacter *	New( void );
	virtual	CCharacter *	Clone( void );

	//--------------------------------------------------------------------------

			void			Init_EveryFrame( void );				// 매 프레임마다 초기에 설정해 주어야할 데이터들

			BOOL			Render( void );							// 렌더링
	virtual void			RenderShadow( void );					// 그림자 렌더링
	virtual void			CreateShadow( void );					// 그림자 생성
	virtual	void			RenderText( void );
	virtual void			RenderInformation( void );
	virtual	BOOL			Animation( DWORD pi_dwAniFrame = -1 );	// 애니메이션 ( 본 or 메시 )
			BOOL			RenderName( void );						// 캐릭터명 렌더링
			void			FrameMove( void );						// 캐릭터의 움직임

	virtual BOOL			SetEffectInfo( DWORD pi_dwActionID );	// 이펙트의 정보를 설정한다.

protected :

			BOOL			IsEnableAnimation( DWORD pi_dwAniFrame );

			void			FrameMove_InitFrameMoveTime( void );

			void			FrameMove_Move( void );
			void			FrameMove_Move_Stop( void );
			void			FrameMove_Move_Moving( void );
			void			FrameMove_Move_Idle( void );

			void			FrameMove_Attack( void );
			void			FrameMove_Attack_SetAttacker( DWORD pi_dwEffectCode, BOOL pi_bIsPushOutForSkill, BOOL pi_bIsPushedOut );
			BOOL			FrameMove_Attack_SetEffect( DWORD pi_dwEffectCode, BOOL pi_bIsPushOutForSkill );
			void			FrameMove_Attack_ProcessNearTypeAttack( DWORD pi_dwEffectCode, BOOL pi_bIsPushOutForSkill, BOOL pi_bIsPushedOut );
			BOOL			FrameMove_Attack_SetEffectFrame( void );
			void			FrameMove_Attack_ProcessStopAnimation( void );
			void			FrameMove_Attack_ProcessLastEffectFrame( void );

			void			FrameMove_Attack_SetVictim( CCharacter * pi_pTarget, BOOL pi_bIsLastEffectFrame, BOOL pi_bIsPushOutForSkill, BOOL pi_bIsPushOut );

			void			FrameMove_Skill( void );
			void			FrameMove_Force( void );
			void			FrameMove_InternalState( void );
			void			FrameMove_EveryFrame( void );

			void			SetEffectInfo_Damage( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
			BOOL			PushEffectInfoToQueue( EffectInfo & pi_stEffectInfo );

			void			SetPushOut( int pi_nPushOutMode, CCharacter * pi_pTarget );
			BOOL			SetPushOut_SetSpeedAndDistance( int pi_nPushOutMode, CCharacter * pi_pTarget );
			BOOL			SetPushOut_PushOutForSkill( CCharacter * pi_pTarget );
			BOOL			SetPushOut_PushedOutForDeath( CCharacter * pi_pTarget );
			BOOL			SetPushOut_PushedOutForDamaged( CCharacter * pi_pTarget );
			BOOL			SetPushOut_PushedOutForSkill( CCharacter * pi_pTarget );

	virtual	float			PositionMove( Vector3f pi_vecTargetPos );		// 타겟 포지션으로 이동
			void			SetAnimationInterpolation( CAniData * pi_pAni, BOOL pi_bIsEnable );				// 애니메이션 보간 설정
			BOOL			SetVertexBlending( int pi_nIndex, CMesh * pi_pMesh, BOOL pi_bForAnimation );	// 버텍스 블렌딩 설정

			CAniData *		GetAnimationDataByRandom( BYTE pi_byAniMgrType, DWORD & pio_dwAniID );

public :

	//--------------------------------------------------------------------------

			void			SetRotY( float pi_fRotY, BOOL pi_bEnableRotYInterpolation = TRUE );
			void			SetRotY( Vector3f pi_vecOrgPos, Vector3f pi_vecTarPos, BOOL pi_bEnableRotYInterpolation = TRUE );
			void			SetPosY( float pi_fPosY, BOOL pi_bEnablePosYInterpolation = TRUE );
			void			SetPosition( float pi_fPosX, float pi_fPosY, float pi_fPosZ, BOOL pi_bSetPrevPosition = TRUE, BOOL pi_bEnablePosYInterpolation = TRUE );
			void			SetPosition( Vector3f pi_vecPos, BOOL pi_bSetPrevPosition = TRUE, BOOL pi_bEnablePosYInterpolation = TRUE );
			BOOL			CheckYPos( void );

	virtual	BYTE			GetWidth( void )						{ return 0; }

			void			Enable( DWORD pi_dwFlag );
			void			Disable( DWORD pi_dwFlag );

	//--------------------------------------------------------------------------

	inline	void			SetCharTypeID( DWORD pi_dwCharTypeID )	{ m_dwCharTypeID = pi_dwCharTypeID; }
	inline	DWORD			GetCharTypeID( void )					{ return m_dwCharTypeID; }
	inline	BOOL			IsEqualCharType( DWORD pi_dwCharTypeID ){ return ( m_dwCharTypeID == pi_dwCharTypeID ); }
			BOOL			IsEqual( CCharacter * pi_pCharacter );
			BOOL			IsEqual( DWORD pi_dwCharTypeID, DWORD pi_dwIndex );

	//--------------------------------------------------------------------------

	inline	void			SetDTIndex( DWORD pi_dwDTIndex )		{ m_dwDTIndex = pi_dwDTIndex; }
	inline	DWORD			GetDTIndex( void )						{ return m_dwDTIndex; }
	virtual	void			SetAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication = TRUE );
	inline	DWORD			GetAction( void )						{ return m_dwActionID; }
	inline	DWORD			GetActionByCategory( void )				{ return ( m_dwActionID >> 28 ); }
	inline	DWORD			IsEqualAction( DWORD pi_dwActionID )	{ return ( m_dwActionID == pi_dwActionID ); }
	inline	DWORD			IsEqualActionCategory( DWORD pi_dwActionID ){ return ( ( m_dwActionID >> 28 ) == pi_dwActionID ); }
			BOOL			IsDeath( void );
			BOOL			IsCombatAction( void );

	inline	void			SetPrevAction( DWORD pi_dwActionID )	{ m_dwPrevActionID = pi_dwActionID; }
	inline	DWORD			GetPrevAction( void )					{ return m_dwPrevActionID; }

private :

			void			SetAction_Move( DWORD pi_dwActionID );
			void			SetAction_Combat( DWORD pi_dwActionID );
			void			SetAction_Damaged( DWORD pi_dwActionID );
			void			SetAction_Death( DWORD pi_dwActionID );
			void			SetAction_InternalState( DWORD pi_dwActionID );

public :

			void			SetAnimationSpeedByAction( void );

	//--------------------------------------------------------------------------

	virtual void			SetCombatMode( BYTE pi_byCombatMode );
	inline	BYTE			GetCombatMode( void )					{ return m_byCombatMode; }
	virtual	BYTE			GetAttackDistanceType( BYTE pi_byAttackType );
	virtual	float			GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget = TRUE );

	virtual	void			SetWalkMode( BYTE pi_byWalkMode );
	inline	BYTE			GetWalkMode( void )						{ return m_byWalkMode; }
			void			SetSpeed( BYTE pi_bMoveMode, float pi_fSpeed );
			void			SetSpeed( float pi_fSpeed );
			float			GetSpeed( BYTE pi_bMoveMode );
			float			GetSpeed( void );
	virtual	float			GetRealSpeed( void );					// 모든 요소를 포함한 캐릭터의 실제 속도
	inline	void			SetMagnificationOfSpeed( float pi_fMagnification ){ m_fMagnificationOfSpeed = pi_fMagnification; }
	inline	float			GetMagnificationOfSpeed( void )			{ return m_fMagnificationOfSpeed; }

	//--------------------------------------------------------------------------

	virtual	void			SetMaxHP( DWORD pi_dwMaxHP = ID_INVALID );
	virtual	DWORD			GetMaxHP( void );
	virtual	void			SetHP( DWORD pi_dwHP )					{ m_dwHP = pi_dwHP; }
	virtual	DWORD			GetHP( void );
	virtual	float			GetHPRate( void );

	virtual	void			SetMaxFP( DWORD pi_dwMaxFP = ID_INVALID );
	virtual	DWORD			GetMaxFP( void )						{ return m_dwMaxFP; }
	virtual	void			SetFP( DWORD pi_dwFP )					{ m_dwFP = pi_dwFP; }
	virtual	DWORD			GetFP( void )							{ return m_dwFP; }

	//--------------------------------------------------------------------------

	virtual	void			SetSFItem( CSFItem * pi_pSFItem );
	inline	CSFItem *		GetSFItem( void )						{ if( m_clSFItem.IsEmpty() ) return NULL; else return &m_clSFItem; }
	inline	APPLIED_SF *	GetAppliedSF( void )					{ return &m_stAppliedSF; }
	inline	CHAR_COND_INFO* GetConditionInfo( void )				{ return &m_stConditionInfo; }
	inline	CCombatState *	GetCombatState( void )					{ return m_pCombatState; }
	inline	CCharAniInfo *	GetCharAniInfo( void )					{ return m_pCharAniInfo; }
	inline	CHAR_ETC_INFO *	GetEtcInfo( void )						{ return &m_stETCInfo; }
	virtual	void			RefreshConditionInfo( void );

	//--------------------------------------------------------------------------

	virtual	DWORD			AddAppliedSF( CSFItem * pi_pSFItem );
	virtual	DWORD			DeleteAppliedSF( CSFItem * pi_pSFItem );
	virtual	void			ClearAppliedSF( void );
			void			SetServerSFEffectCode( WORD pi_wServerEffectCode, BYTE pi_bySFLevel, BOOL pi_bIsAdd );

			CAttackerInfo *	AddAttackerInfo( CCharacter * pi_pEnemy, BOOL pi_bIsSettingToCurEnemy );
			CVictimInfo *	AddVictimInfo( CCharacter * pi_pEnemy, BOOL pi_bIsSettingToCurEnemy );
			CAttackerInfo *	GetAttackerInfo( CCharacter * pi_pEnemy );
			CVictimInfo *	GetVictimInfo( CCharacter * pi_pEnemy );
			CAttackerInfo *	GetCurAttackerInfo( void );
			CVictimInfo *	GetCurVictimInfo( void );
			CCharacter *	GetCurAttacker( void );
			CCharacter *	GetCurVictim( void );

	//--------------------------------------------------------------------------

			void			EnableStun( void );

	//--------------------------------------------------------------------------

	virtual	void			UI_SetRadar( void )		{ ; }
	virtual	void			UI_SetMinimap( void )	{ ; }
	virtual	void			UI_SetPositionOfRadar( void );

	//--------------------------------------------------------------------------

	inline	CPath *			GetPathStack( void )					{ return &m_cPathStack; }
			void			CreatePath( Vector3f pi_vecTargetPos );
			BOOL			GetNextPath( Vector3f po_vecPos );
			void			GetCurPath( Vector3f po_vecPos );
			BOOL			GetLastPath( Vector3f po_vecPath );
			BOOL			GetPath( Vector3f po_vecPath, int pi_nPathNo );
	inline	int				GetCurPathNo( void )					{ return m_cPathStack.GetCurPathNo(); }
	inline	int				GetTotalPathNo( void )					{ return m_cPathStack.GetTotalPathNo(); }
			void			CheckLongPoint( Vector3f pi_vecPos, Vector3f pi_vecTargetPos );	// by J.U.I - 길게 피킹할경우. 다시 패스를 얻어낸다.

	inline	void			SetFirstMoving( BOOL pi_bFlag )			{ m_bIsFirstMoving = pi_bFlag; }
	inline	BOOL			IsFirstMoving( void )					{ return m_bIsFirstMoving; }

	inline	float			GetNextPathX( void )					{ return m_vecNextPath[0]; }
	inline	float			GetNextPathY( void )					{ return m_vecNextPath[1]; }
	inline	float			GetNextPathZ( void )					{ return m_vecNextPath[2]; }

	//--------------------------------------------------------------------------

	virtual	BOOL			IsRemovableWhenWasNotReciveNetworkMsg( void );
	virtual	BOOL			IsPassedLimitTimeFromReceiveNetworkMsg( void );
	virtual	BOOL			IsPassedLimitTimeForDelete( void );
	inline	void			SetReceiveNetMsg( char * pi_pMessage )	{ m_stETCInfo.SetReceiveNetMsg( pi_pMessage ); }
			DWORD			GetTotalFrameSkipTime( void );

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//

	// by J.U.I
	inline	void			Set2ndLightRadius( float pi_f2ndRadius ){ m_f2ndRadius=pi_f2ndRadius; };	//2번째 라이팅 반지름.
	inline	float			Get2ndLightRadius( void  )				{ return m_f2ndRadius; };			//2번째 라이팅 반지름.
	inline	void			Set2ndLightPos( Vector3f pi_vec2ndLgtPos )	{ Vector3fCopy(pi_vec2ndLgtPos,m_vec2ndLgtPos); };		//2번째 라이팅 위치.
	inline	void			Get2ndLightPos( Vector3f pi_vec2ndLgtPos )	{ Vector3fCopy(m_vec2ndLgtPos,pi_vec2ndLgtPos); };		//2번째 라이팅 위치.
	inline	void			Set2ndColor( DWORD pi_dw2Color )		{ m_dw2ndColor= pi_dw2Color; };		//2번째 라이팅 컬러.
	inline	DWORD			Get2ndColor( void )						{ return m_dw2ndColor; };			//2번째 라이팅 컬러.
	inline	void 			Set2ndDensity(float pi_fDen)			{ m_f2ndDensity=pi_fDen;	};		//2번째 라이팅 강도 0일경우는 라이팅처리가 없다.
	inline	float			Get2ndDensity()							{ return m_f2ndDensity;	};			//2번째 라이팅 강도 0일경우는 라이팅처리가 없다.
	inline	void 			AddSkillAtackCnt( void )				{ m_bySkillAtackCnt++;	};			//스킬 타격 카운트를 올린다.
	inline	BYTE 			GetSkillAtackCnt( void )				{ return m_bySkillAtackCnt;	};		//스킬 타격 카운트를 올린다.
	inline	void 			CleanSkillAtackCnt( void )				{ m_bySkillAtackCnt=0;	};			//스킬 타격 카운트를 비운다.

	inline	float			GetDamageTimeForShake( void )			{ return m_fDamageTimeForShake; }	// J.U.I - 맞을때 허리제끼기 타임.
	inline	void			SetDamageTimeForShake( float time )		{ m_fDamageTimeForShake=time; }		// J.U.I - 맞을때 허리제끼기 타임.

	// by MAYUN
	inline	BOOL			PSetVertexBlending( int pi_nIndex, CMesh * pi_pMesh, BOOL pi_bForAnimation )
																	{ return SetVertexBlending( pi_nIndex, pi_pMesh, pi_bForAnimation ); }
	
	D3DXMATRIX	*			GetResultMat( DWORD pi_dwPart, DWORD pi_dwMeshNum );
			BYTE			GetDummyIndex( DWORD pi_dwArrayId );
			void			SetDummyIndex( DWORD pi_dwArrayId,DWORD pi_dwId );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef CCharacter CDummy;

#endif // __CHARACTER_H__
