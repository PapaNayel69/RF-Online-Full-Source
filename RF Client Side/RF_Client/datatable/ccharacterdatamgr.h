////////////////////////////////////////////////////////////////////////////////
//
// CCharacterDataMgr Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CHARACTER_DATA_MGR_H__
#define __CHARACTER_DATA_MGR_H__

#include "../DataFile.h"
#include "./ServerTable/PlayerCharacterFld.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct SF_DATA;

class CPlayerClass
{
public :

	enum { MAX_CHILD_CLASS_NUM = 8 };							// 자식 클래스의 개수
	enum { MAX_SKILL_NUM = 3 };									// 클래스 스킬의 개수

protected :

	BYTE		m_byType;										// 클래스 타입 ( 0 = 전사형, 1 = 레인져형, 2 = 마법사형, 3 = 특수형 )
	DWORD		m_dwDTIndex;									// DT Index
	DWORD		m_dwDTCode;										// DT Code ( 숫자 )
	char		m_strDTCode[MAX_NAME_LENGTH];					// DT Code ( String )
	DWORD		m_dwChildClassDTCode[MAX_CHILD_CLASS_NUM];		// 하위 클래스 DT Code
	DWORD		m_dwIconID;										// Icon ID
	char		m_pClassTypeName[MAX_NAME_LENGTH];
	char		m_pClassName_kor[MAX_NAME_LENGTH];
	char		m_pClassName_Eng[MAX_NAME_LENGTH];
	BYTE		m_byUsableRace;

	DWORD		m_dwClassSkillDTCode[MAX_SKILL_NUM];			// 클래스 스킬의 DT Code

	DWORD		m_dwExpertBonus[MAX_EXPERTNESS_VALUE_TYPE];		// 숙련도 보너스
	DWORD		m_dwForceMasteryBonus[MAX_FORCE_MASTERY_NUM];	// 포스 보너스
	DWORD		m_dwSkillMasteryBonus[MAX_SKILL_MASTERY_NUM];	// 스킬 보너스 ( 실제 데이터는 없지만 호환성을 유해 사용한다. )

	char		m_pDescription[MAX_DESCRIPTION_LENGTH];			// 클래스 설명

public :

	CPlayerClass();

	inline	BYTE			GetType( void )							{ return m_byType; }
	inline	DWORD			GetDTIndex( void )						{ return m_dwDTIndex; }
	inline	DWORD			GetDTCode( void )						{ return m_dwDTCode; }
	inline	char *			GetClassName( void )					{ return m_pClassName_kor; }
	inline	DWORD			GetIconID( void )						{ return m_dwIconID; }

			DWORD			GetHP( void );
			DWORD			GetFP( BYTE pi_byLevel = 1 );
			DWORD			GetSP( void );
			BYTE			GetExpertness( BYTE pi_byType );
			BYTE			GetForceExpertness( BYTE pi_byClassType );
			BYTE			GetSkillExpertness( BYTE pi_byClassType );

			CPlayerClass *	GetChildClass( BYTE pi_byOrderNo );
			SF_DATA *		GetClassSkill( BYTE pi_byOrderNo );

	inline	BOOL			IsEqualDTIndex( DWORD pi_dwDTIndex )	{ return ( m_dwDTIndex == pi_dwDTIndex ); }
	inline	BOOL			IsEqualDTCode( DWORD pi_dwDTCode )		{ return ( m_dwDTCode == pi_dwDTCode ); }
	inline	BOOL			IsEqualDTString( char * pi_strDTCode );

	inline	char *			GetDescription( void )					{ return m_pDescription; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_MERCHANT_ITEM_NUM	240		// 상인이 가진 아이템의 최대 갯수
#define MAX_NPC_BUTTON_NUM		5

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

struct STORE_ITEM_DATA : public BASE_DATA
{
	BYTE	byItemType;					// 아이템의 타입
	DWORD	dwDTCode;					// 아이템의 데이터 테이블상의 코드 [ ex) iw00001 ]
};

struct MERCHANT_DATA : public BASE_DATA
{
	DWORD	dwIndex;
	DWORD	dwDTCode;
	BYTE	byRace;
	char	pName[MAX_NAME_LENGTH];
	char	pDummyName[MAX_NAME_LENGTH];
	BYTE	byMerchantType;
	float	fYAngle;

	DWORD	dwMaxItemNum;

	STORE_ITEM_DATA pItemList[MAX_MERCHANT_ITEM_NUM];	// 아이템 리스트
	BYTE	pButtonFunction[MAX_NPC_BUTTON_NUM];		// 버튼의 기능 리스트

	char	pDescription[MAX_DESCRIPTION_LENGTH];		// NPC 설명

	MERCHANT_DATA()
	{
		ZeroMemory( this, sizeof( MERCHANT_DATA ) );
	}
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNpcBase;
class CMonster;
class CAnimus;

class CMonsterData : public BASE_DATA
{
	friend CNpcBase;
	friend CMonster;

	enum { MAX_FORCE_NUM = 3 };

	DWORD	m_dwMeshID;
	char 	m_pName[MAX_NAME_LENGTH];
	BYTE	m_byLevel;
	DWORD	m_dwMaxHP;

	DWORD	m_dwForceID[MAX_FORCE_NUM];	// 시전가능한 포스의 ID

	DWORD	m_dwAttackDelay;
	float	m_fAttackRange;
	BYTE	m_byMoveSpeed[2];

	BYTE	m_byWidth;
	float	m_fScale;

	DWORD	m_dwEffectValue;			// 이펙트에서만 사용되는 값
	DWORD	m_dwAttackEffectMaterial;	// 이펙트에서만 사용되는 값
	DWORD	m_dwDefenceEffectMaterial;	// 이펙트에서만 사용되는 값
};

class CAnimusData : public BASE_DATA
{
	friend class CNpcBase;
	friend class CAnimus;
	friend class CAnimusItem;
	friend class CCharacterDataMgr;

	enum { MAX_FORCE_NUM = 3 };

	BYTE	m_byLevel;					// 레벨
	DWORD	m_dwExp;					// 해당 레벨의 애너머스가 되기 위한 경험치
	DWORD	m_dwMaxHP;
	DWORD	m_dwMaxFP;

	DWORD	m_dwHPRecoveryPoint;		// HP 회복양
	DWORD	m_dwHPRecoveryDelay;		// HP 회복 딜레이
	DWORD	m_dwFPRecoveryPoint;		// FP 회복양
	DWORD	m_dwFPRecoveryDelay;		// FP 회복 딜레이

	DWORD	m_dwAttackPower;			// 평균 공격력
	DWORD	m_dwDefencePower;			// 평균 방어력
	DWORD	m_dwForcePower;				// 평균 포스 공격력
	BYTE	m_byExpertness[MAX_EXPERTNESS_VALUE_TYPE];
	BYTE	m_byForceLevel;				// 시전가능한 포스의 레벨
	DWORD	m_dwForceID[MAX_FORCE_NUM];	// 시전가능한 포스의 ID

	BYTE	m_byWeakPartID;
	BYTE	m_byResist[MAX_ITEM_ATTRIBUTE];

	DWORD	m_dwAttackDelay;
	float	m_fAttackRange;
	BYTE	m_byMoveSpeed[2];

	BYTE	m_byWidth;
	float	m_fScale;

	DWORD	m_dwAttackEffectMaterial;	// 이펙트에서만 사용되는 값
	DWORD	m_dwDefenceEffectMaterial;	// 이펙트에서만 사용되는 값

public :

	inline	BYTE	GetLevel( void )	{ return m_byLevel; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct SF_EFFECT
{
	BYTE		byUsingType;					// 사용 형태 ( 공격, 보조 등 )
	BYTE		byEffectType[MAX_SF_LEVEL];		// 사용시 대상에 미치는 실제 효과 ( byUsingType에 따라서 의미가 틀려짐, 스킬/포스 레벨에 따라서 define이 틀려질 수도 있음 )
	BYTE		byEffectUnit;					// fEffect[7]값의 단위 ( +, -, *, ... )
	float		fEffectValue[MAX_SF_LEVEL];		// 실제 효과 수치

	SF_EFFECT()
	{
		ZeroMemory( this, sizeof( SF_EFFECT ) );
		byUsingType	= 0xFF;
	}
};

struct SF_DATA : public BASE_DATA
{
	DWORD		dwDTIndex;						// DT Index
	DWORD		dwDTCode;						// DT Code ( == ID )
	DWORD		dwIconID;						// Icon ID
	BYTE		byType;							// 스킬 / 포스의 종류
	BYTE		byMasteryIndex;					// 스킬 / 포스의 종류

	char		pMasteryName_Eng[MAX_NAME_LENGTH];
	char		pMasteryName_Kor[MAX_NAME_LENGTH];
	char		pName_Eng[MAX_NAME_LENGTH];
	char		pName_Kor[MAX_NAME_LENGTH];

	BYTE		byStep;							// 스킬 / 포스의 단계 ( 저급, 중급, 고급, 최고급 )

	SF_EFFECT	stEffect[MAX_SF_EFFECT];		// 스킬/포스의 이펙트 정보
	DWORD		dwEffectTime[MAX_SF_LEVEL];		// 실제 영향을 미치는 시간 ( 지속 보조일 경우만 해당 )

	char		byUsableRace[8];				// 사용가능 종족
	BYTE		byUsableClassType;				// 사용가능 클래스 타입 ( 0 = 전사형, 1 = 레인져형, 2 = 마법사형, 3 = 특수형 )
	char		byTargetType[8];				// 스킬 / 포스를 사용할 수 있는 타겟의 형태

	BYTE		byNeedfulMasteryIndex;			// 시전시 필요한 마스터리의 인덱스 ( 0xFF면 필요없음 )
	char		byNeedfulWeapon[32];			// 시전시 장착해야할 무기
	BOOL		bIsNeedfulShield;				// 시전시 방패를 장착하고 있어야 하는가?
	BYTE		bySpecialEffectType;			// 특수 효과 타입
	BYTE		byNeedfulSpecialEffectType;		// 시전시 필요한 특수 효과 타입 ( 0xFF면 필요없음 )

	BYTE		byHP;							// 소모 HP
	BYTE		byFP;							// 소모 FP
	BYTE		bySP;							// 소모 SP
	float		fDelay;							// 시전 딜레이

	char		pDescription[128];				// 스킬 / 포스의 설명

	BYTE		byRangeType_For_Effect;			// 시전시 이펙트를 위한 Range Type ( 단순히 이펙트 코드와 같이 사용된다. 실제 게임 플레이와는 상관없음 )
};

struct SKILL_DATA : public SF_DATA
{
	BYTE		byNeedfulBulletNum;				// 필요한 탄환 수

	float		fMoveSpeed[3];					// 이동하는 스킬일 경우 움직이는 속도 [0] = 1-3, [1] = 4-6, [2] = 7
	float		fMoveDistance[3];				// 이동하는 스킬일 경우 움직이는 거리 [0] = 1-3, [1] = 4-6, [2] = 7
};

struct FORCE_DATA : public SF_DATA
{
	BYTE		byAttribute;					// 포스의 속성 ( 무속성, 물, 불, 바람, 흙 ) => 공격형에서만 사용됨.
	float		fDistance;						// 시전 거리
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef	BASE_DATA				DT_CharInfo;
typedef	_PlayerCharacter_fld	DT_CPInfo;
typedef MERCHANT_DATA			DT_CNMInfo;		// NPC Merchant;

typedef CDataRecord				DT_PlayerList;

class CSkillBoard;
class CForceBoard;

/*//////////////////////////////////////////////////////////////////////////////

[ CCharacterDataMgr ]

  - 캐릭터들의 바이너리 테이블에서 읽은 데이터를 관리한다.

//////////////////////////////////////////////////////////////////////////////*/

class CCharacterDataMgr
{
protected :

	DT_PlayerList 		m_listPlayer;
	CDataMgr 			m_listMonster;

	DWORD				m_dwMaxMerchant;
	DT_CNMInfo *		m_listMarchant;

	BYTE				m_byMaxAnimusTypeNum;
	CDataMgr			m_listAnimus;

	DWORD				m_dwMaxForceNum;
	DWORD				m_dwMaxSkillNum;
	DWORD				m_dwMaxClassSkillNum;
	FORCE_DATA *		m_listForce;
	SKILL_DATA *		m_listSkill;
	SKILL_DATA *		m_listClassSkill;

	CDataMgr			m_listPlayerClass;

	//--------------------------------------------------------------------------

	CDataRecord			m_tableExp;

	CSkillBoard *		m_pSkillBoard;		// 게임에 들어가기 전 FP 등을 구하기 위해 사용한다.
	CForceBoard * 		m_pForceBoard;		// 게임에 들어가기 전 FP 등을 구하기 위해 사용한다.

public :

	CCharacterDataMgr();
	~CCharacterDataMgr();

			void			Init( void );
			BOOL			Create( void );
			BOOL			Destroy( void );

	//--------------------------------------------------------------------------

			DT_CharInfo *	GetCharacter( DWORD pi_dwCharTypeID, DWORD pi_dwIndex );
			DT_CharInfo *	GetCharacterByOrder( DWORD pi_dwCharTypeID, DWORD pi_dwIndex );

			DT_CPInfo	*	GetPlayer( DWORD pi_dwIndex );
	inline	DT_CPInfo	*	GetPlayerByOrder( DWORD pi_dwIndex )
							{ return ( DT_CPInfo * )m_listPlayer.GetRecord( pi_dwIndex ); }

			CMonsterData *	GetMonster( DWORD pi_dwIndex );
	inline	CMonsterData *	GetMonsterByOrder( DWORD pi_dwIndex )
							{ return ( CMonsterData * )m_listMonster.GetData( pi_dwIndex ); }

			DT_CNMInfo	*	GetNpc( DWORD pi_dwIndex );
	inline	DT_CNMInfo	*	GetNpcByOrder( DWORD pi_dwIndex )
							{ return ( DT_CNMInfo * )&m_listMarchant[pi_dwIndex]; }

			CAnimusData	*	GetAnimus( BYTE pi_byAnimusType, DWORD pi_dwIndex );
			CAnimusData	*	GetAnimus( DWORD pi_dwIndex );
			CAnimusData	*	GetAnimusByOrder( BYTE pi_byAnimusType, DWORD pi_dwIndex );
			CAnimusData	*	GetAnimusByOrder( DWORD pi_dwIndex );

			CAnimusData	*	GetAnimusByExp( BYTE pi_byAnimusType, DWORD pi_dwExp );
			CAnimusData	*	GetAnimusByLevel( BYTE pi_byAnimusType, BYTE pi_byLevel );

	//--------------------------------------------------------------------------

			SF_DATA *		GetSF( BYTE pi_byType, DWORD pi_dwDTIndex );
			SF_DATA *		GetSFByDTCode( BYTE pi_byType, DWORD pi_dwDTCode );
			FORCE_DATA *	GetForce( DWORD pi_dwDTIndex );
			FORCE_DATA *	GetForceByDTCode( DWORD pi_dwDTCode );
			SKILL_DATA *	GetSkill( DWORD pi_dwDTIndex );
			SKILL_DATA *	GetSkillByDTCode( DWORD pi_dwDTCode );
			SKILL_DATA *	GetClassSkill( DWORD pi_dwDTIndex );
			SKILL_DATA *	GetClassSkillByDTCode( DWORD pi_dwDTCode );

			CPlayerClass *	GetClassData( char * pi_strDTCode );
			CPlayerClass *	GetClassDataByDTIndex( DWORD pi_dwDTIndex );
			CPlayerClass *	GetClassDataByDTCode( DWORD pi_dwDTCode );
			CPlayerClass *	GetClassDataByOrder( DWORD pi_dwOrderNo );

			BOOL			LoadData( void );

	//--------------------------------------------------------------------------

			int				GetTotalCharacterNum( DWORD pi_dwCharTypeID );

	inline	DWORD			GetTotalPlayerNum( void )			{ return m_listPlayer.m_Header.m_nRecordNum; }
	inline	DWORD			GetTotalMonsterNum( void )			{ return m_listMonster.GetTotalNum(); }
	inline	DWORD			GetTotalNpcNum( void )				{ return m_dwMaxMerchant; }

	inline	DWORD			GetTotalForceNum( void )			{ return m_dwMaxForceNum; }
	inline	DWORD			GetTotalSkillNum( void )			{ return m_dwMaxSkillNum; }

	inline	BYTE			GetMaxAnimusTypeNum( void )			{ return m_byMaxAnimusTypeNum; }

	//--------------------------------------------------------------------------

			double			GetNeedExpForLevelup( BYTE pi_byRaceType, BYTE pi_byLevel );

	//--------------------------------------------------------------------------

	inline	CSkillBoard *	GetSkillBoard( void )				{ return m_pSkillBoard; }
	inline	CForceBoard *	GetForceBoard( void )				{ return m_pForceBoard; }

private :

			BOOL			LoadSkillForceData( void );
			BOOL			LoadNpcData( void );
			BOOL			LoadCharacterData( void );			
};

#endif // __CHARACTER_DATA_MGR_H__