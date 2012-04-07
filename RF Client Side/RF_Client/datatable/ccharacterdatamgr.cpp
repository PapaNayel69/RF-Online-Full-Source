////////////////////////////////////////////////////////////////////////////////
//
// CCharacterDataMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../GameObject/Board/Common/CBoardManager.h"
#include "CCharacterDataMgr.h"

#include "../DataFile.h"
#include "../DataTable/ServerTable/ExpFld.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CPlayerClass::CPlayerClass()
{
	ZeroMemory( this, sizeof( CPlayerClass ) );

	m_dwDTIndex	= ID_INVALID;
	m_dwDTCode	= ID_INVALID;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD
CPlayerClass::GetHP( void )
{
	return 80 + ( DWORD )( sqrt( 1 * pow( GetExpertness( EVT_DEFENCE ), 2 ) ) * 10 );
}

DWORD
CPlayerClass::GetFP( BYTE pi_byLevel )
{
	float fExpertness = 0.0f;

	// 포스의 숙련도 ( accretia일 경우에는 무조건 0임. )
	if( m_byUsableRace != 2 )
	{
		( m_byUsableRace == 0 ) ?	fExpertness = ( GetForceExpertness( FCT_HOLY ) * 0.4f ) :
									fExpertness = ( GetForceExpertness( FCT_DARK ) * 0.4f );

		fExpertness +=	( GetForceExpertness( FCT_FIRE ) * 0.075f ) +
						( GetForceExpertness( FCT_AQUA ) * 0.1125f ) +
						( GetForceExpertness( FCT_TERRA ) * 0.1125f ) +
						( GetForceExpertness( FCT_WIND ) * 0.075f );
	}

	// 스킬의 숙련도
	const float CONSTANT[3] = { 0.1125f, 0.1125f, 0.5f };

	fExpertness +=	( GetSkillExpertness( SCT_UI_MELEE ) * CONSTANT[m_byUsableRace] ) +
					( GetSkillExpertness( SCT_UI_MISSILE ) * CONSTANT[m_byUsableRace] );

	// result
	return ( DWORD )( 40 + sqrt( pi_byLevel * pow( fExpertness, 2 ) ) * 5 );
}

DWORD
CPlayerClass::GetSP( void )
{
	float fExpertness = ( GetExpertness( EVT_NEAR ) * 0.2f ) + ( GetExpertness( EVT_FAR ) * 0.8f );

	return 160 + ( DWORD )( sqrt( 1 * pow( fExpertness, 2 ) ) * 2.5f );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BYTE
CPlayerClass::GetExpertness( BYTE pi_byType )
{
	if( pi_byType >= MAX_EXPERTNESS_VALUE_TYPE )
		return 1;

	if( pi_byType == EVT_FORCE )
	{
		if( _GetForceBoard() )
			return _GetForceBoard()->CalculateForceExpertness();
		else
			return _GetCharDataMgr()->GetForceBoard()->CalculateForceExpertness( m_dwForceMasteryBonus );
	}
	else
	{
		return _GetAvatar()->CalculateExpertness( pi_byType, m_dwExpertBonus[pi_byType] );
	}
}

// 포스의 클래스별 숙련도 ( 캐릭터 정보창의 포스 숙련도는 전체 포스의 숙련도임 )
BYTE
CPlayerClass::GetForceExpertness( BYTE pi_byClassType )
{
	if( pi_byClassType >= MAX_UI_FORCE_CLASS_TYPE )
		return 1;

	if( _GetForceBoard() )
		return _GetForceBoard()->GetSFExpertness( pi_byClassType );
	else
		return _GetCharDataMgr()->GetForceBoard()->GetSFExpertness( pi_byClassType, m_dwForceMasteryBonus );
}

// 스킬의 클래스별 숙련도 ( 캐릭터 정보창의 스킬 숙련도는 전체 스킬의 숙련도임 )
BYTE
CPlayerClass::GetSkillExpertness( BYTE pi_byClassType )
{
	if( pi_byClassType >= MAX_UI_SKILL_CLASS_TYPE )
		return 1;
	
	if( _GetSkillBoard() )
		return _GetSkillBoard()->GetSFExpertness( pi_byClassType );
	else
		return _GetCharDataMgr()->GetSkillBoard()->GetSFExpertness( pi_byClassType, m_dwSkillMasteryBonus );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPlayerClass::IsEqualDTString( char * pi_strDTCode )
{
	if( !pi_strDTCode )
		return FALSE;

	return ( strcmp( m_strDTCode, pi_strDTCode ) == 0 );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CPlayerClass *
CPlayerClass::GetChildClass( BYTE pi_byOrderNo )
{
	if( pi_byOrderNo >= MAX_CHILD_CLASS_NUM )
		return NULL;

	return _GetCharDataMgr()->GetClassDataByDTCode( m_dwChildClassDTCode[pi_byOrderNo] );
}

SF_DATA *
CPlayerClass::GetClassSkill( BYTE pi_byOrderNo )
{
	if( pi_byOrderNo >= MAX_SKILL_NUM )
		return NULL;

	return _GetCharDataMgr()->GetClassSkillByDTCode( m_dwClassSkillDTCode[pi_byOrderNo] );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCharacterDataMgr::CCharacterDataMgr()
{
	Init();
}

CCharacterDataMgr::~CCharacterDataMgr()
{
	Destroy();
}

void
CCharacterDataMgr::Init( void )
{
	m_dwMaxMerchant			= 0;
	m_listMarchant			= NULL;

	m_byMaxAnimusTypeNum	= 0;

	m_dwMaxForceNum			= 0;
	m_dwMaxSkillNum			= 0;
	m_listForce				= NULL;
	m_listSkill				= NULL;

	m_pSkillBoard			= NULL;
	m_pForceBoard			= NULL;
}

BOOL
CCharacterDataMgr::Create( void )
{
	return TRUE;
}

BOOL
CCharacterDataMgr::Destroy( void )
{
	_DELETE_ARRAY( m_listMarchant );
	_DELETE_ARRAY( m_listForce );
	_DELETE_ARRAY( m_listSkill );

	_DELETE( m_pSkillBoard );
	_DELETE( m_pForceBoard );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

DT_CharInfo *
CCharacterDataMgr::GetCharacter( DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	switch( pi_dwCharTypeID )
	{
		case CTI_PLAYER		: return ( DT_CharInfo * )GetPlayer( pi_dwIndex );
		case CTI_MONSTER	: return ( DT_CharInfo * )GetMonster( pi_dwIndex );
		case CTI_NPC		: return ( DT_CharInfo * )GetNpc( pi_dwIndex );
		case CTI_ANIMUS		: return ( BASE_DATA * )GetAnimus( pi_dwIndex );
	}

	return NULL;
}

DT_CharInfo *
CCharacterDataMgr::GetCharacterByOrder( DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	switch( pi_dwCharTypeID )
	{
		case CTI_PLAYER		: return ( DT_CharInfo * )GetPlayerByOrder( pi_dwIndex );
		case CTI_MONSTER	: return ( DT_CharInfo * )GetMonsterByOrder( pi_dwIndex );
		case CTI_NPC		: return ( DT_CharInfo * )GetNpcByOrder( pi_dwIndex );
		case CTI_ANIMUS		: return ( BASE_DATA * )GetAnimusByOrder( pi_dwIndex );
	}

	return NULL;
}

DT_CPInfo *
CCharacterDataMgr::GetPlayer( DWORD pi_dwIndex )
{
	return GetPlayerByOrder( pi_dwIndex );

	/*
	for( int i = 0; i < m_listPlayer.m_Header.m_nRecordNum; ++i )
	{
		DT_CPInfo * l_pChar = GetPlayerByOrder( i );

		if( l_pChar )
		{
			if( l_pChar->m_dwIndex == pi_dwIndex )
				return l_pChar;
		}
	}

	return NULL;
	*/
}

CMonsterData *
CCharacterDataMgr::GetMonster( DWORD pi_dwIndex )
{
	return GetMonsterByOrder( pi_dwIndex );
	/*
	for( int i = 0; i < m_listMonster.m_Header.m_nRecordNum; ++i )
	{
		DT_CMInfo * l_pChar = GetMonsterByOrder( i );

		if( l_pChar )
		{
			if( l_pChar->m_dwIndex == pi_dwIndex )
				return l_pChar;
		}
	}

	return NULL;
	*/
}

DT_CNMInfo *
CCharacterDataMgr::GetNpc( DWORD pi_dwIndex )
{
	for( int i = 0; i < m_dwMaxMerchant; ++i )
	{
		if( m_listMarchant[i].dwIndex == pi_dwIndex )
			return &m_listMarchant[i];
	}

	return NULL;
}

CAnimusData *
CCharacterDataMgr::GetAnimus( BYTE pi_byAnimusType, DWORD pi_dwIndex )
{
	return GetAnimusByOrder( pi_byAnimusType, pi_dwIndex );
}

CAnimusData *
CCharacterDataMgr::GetAnimus( DWORD pi_dwIndex )
{
	return GetAnimusByOrder( pi_dwIndex );
}

CAnimusData *
CCharacterDataMgr::GetAnimusByOrder( BYTE pi_byAnimusType, DWORD pi_dwIndex )
{
	return ( CAnimusData * )m_listAnimus.GetData( ( pi_byAnimusType * MAX_ANIMUS_LEVEL ) + pi_dwIndex );
}

CAnimusData *
CCharacterDataMgr::GetAnimusByOrder( DWORD pi_dwIndex )
{
	return ( CAnimusData * )m_listAnimus.GetData( pi_dwIndex );
}

CAnimusData	*
CCharacterDataMgr::GetAnimusByExp( BYTE pi_byAnimusType, DWORD pi_dwExp )
{
	CAnimusData * pAnimusData = NULL;

	int start = pi_byAnimusType * MAX_ANIMUS_LEVEL;
	for( int i = start; i < start + MAX_ANIMUS_LEVEL; ++i )
	{
		pAnimusData = ( CAnimusData * )m_listAnimus.GetData( i );
		if( !pAnimusData )
			continue;

		if( pi_dwExp < pAnimusData->m_dwExp )
			return pAnimusData;
	}

	return NULL;
}

CAnimusData	*
CCharacterDataMgr::GetAnimusByLevel( BYTE pi_byAnimusType, BYTE pi_byLevel )
{
	CAnimusData * pAnimusData = NULL;

	int start = pi_byAnimusType * MAX_ANIMUS_LEVEL;
	for( int i = start; i < start + MAX_ANIMUS_LEVEL; ++i )
	{
		pAnimusData = ( CAnimusData * )m_listAnimus.GetData( i );
		if( !pAnimusData )
			continue;

		if( pi_byLevel == pAnimusData->m_byLevel )
			return pAnimusData;
	}

	return NULL;
}

//------------------------------------------------------------------------------

int
CCharacterDataMgr::GetTotalCharacterNum( DWORD pi_dwCharTypeID )
{
	switch( pi_dwCharTypeID )
	{
		case CTI_PLAYER		: return GetTotalPlayerNum();
		case CTI_MONSTER	: return GetTotalMonsterNum();
		case CTI_NPC		: return GetTotalNpcNum();
	}

	return -1;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

SF_DATA *
CCharacterDataMgr::GetSF( BYTE pi_byType, DWORD pi_dwDTIndex )
{
	if( pi_byType == CAT_FORCE )
		return GetForce( pi_dwDTIndex );
	else
		return  GetSkill( pi_dwDTIndex );

	return NULL;
}

SF_DATA *
CCharacterDataMgr::GetSFByDTCode( BYTE pi_byType, DWORD pi_dwDTCode )
{
	if( pi_byType == CAT_FORCE )
		return GetForceByDTCode( pi_dwDTCode );
	else
		return  GetSkillByDTCode( pi_dwDTCode );

	return NULL;
}

FORCE_DATA *
CCharacterDataMgr::GetForce( DWORD pi_dwDTIndex )
{
	if( pi_dwDTIndex >= m_dwMaxForceNum )
		return NULL;

	return &m_listForce[ pi_dwDTIndex ];
}

FORCE_DATA *
CCharacterDataMgr::GetForceByDTCode( DWORD pi_dwDTCode )
{
	for( int i = 0; i < m_dwMaxForceNum; ++i )
	{
		if( m_listForce[i].dwDTCode == pi_dwDTCode )
			return &m_listForce[i];
	}

	return NULL;
}

SKILL_DATA *
CCharacterDataMgr::GetSkill( DWORD pi_dwDTIndex )
{
	if( pi_dwDTIndex >= m_dwMaxSkillNum )
		return NULL;

	return &m_listSkill[ pi_dwDTIndex ];
}

SKILL_DATA *
CCharacterDataMgr::GetSkillByDTCode( DWORD pi_dwDTCode )
{
	for( int i = 0; i < m_dwMaxSkillNum; ++i )
	{
		if( m_listSkill[i].dwDTCode == pi_dwDTCode )
			return &m_listSkill[i];
	}

	return NULL;
}

SKILL_DATA *
CCharacterDataMgr::GetClassSkill( DWORD pi_dwDTIndex )
{
	if( pi_dwDTIndex >= m_dwMaxClassSkillNum )
		return NULL;

	return &m_listClassSkill[ pi_dwDTIndex ];
}

SKILL_DATA *
CCharacterDataMgr::GetClassSkillByDTCode( DWORD pi_dwDTCode )
{
	for( int i = 0; i < m_dwMaxClassSkillNum; ++i )
	{
		if( m_listClassSkill[i].dwDTCode == pi_dwDTCode )
			return &m_listClassSkill[i];
	}

	return NULL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CPlayerClass *
CCharacterDataMgr::GetClassData( char * pi_strDTCode )
{
	CPlayerClass * pClass;
	for( int i = 0; i < m_listPlayerClass.GetTotalNum(); ++i )
	{
		pClass = static_cast< CPlayerClass * >( m_listPlayerClass.GetData_Void( i ) );
		if( !pClass )
			continue;

		if( pClass->IsEqualDTString( pi_strDTCode ) )
			return pClass;
	}

	return NULL;
}

CPlayerClass *
CCharacterDataMgr::GetClassDataByDTIndex( DWORD pi_dwDTIndex )
{
	CPlayerClass * pClass;
	for( int i = 0; i < m_listPlayerClass.GetTotalNum(); ++i )
	{
		pClass = static_cast< CPlayerClass * >( m_listPlayerClass.GetData_Void( i ) );
		if( !pClass )
			continue;

		if( pClass->IsEqualDTIndex( pi_dwDTIndex ) )
			return pClass;
	}

	return NULL;
}

CPlayerClass *
CCharacterDataMgr::GetClassDataByDTCode( DWORD pi_dwDTCode )
{
	CPlayerClass * pClass;
	for( int i = 0; i < m_listPlayerClass.GetTotalNum(); ++i )
	{
		pClass = static_cast< CPlayerClass * >( m_listPlayerClass.GetData_Void( i ) );
		if( !pClass )
			continue;

		if( pClass->IsEqualDTCode( pi_dwDTCode ) )
			return pClass;
	}

	return NULL;
}

CPlayerClass *
CCharacterDataMgr::GetClassDataByOrder( DWORD pi_dwOrderNo )
{
	if( pi_dwOrderNo >= m_listPlayerClass.GetTotalNum() )
		return NULL;

	return static_cast< CPlayerClass * >( m_listPlayerClass.GetData_Void( pi_dwOrderNo ) );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CCharacterDataMgr::LoadData( void )
{
	// load data
	m_listPlayer.LoadData( "./DataTable/Player.edf" );

	LoadSkillForceData();
	LoadNpcData();
	LoadCharacterData();

	m_tableExp.LoadData( "./DataTable/Exp.edf" );

	// make skill/force board
	m_pSkillBoard	= new CSkillBoard;
	m_pForceBoard	= new CForceBoard;

	m_pSkillBoard->Create( IBT_SKILL, MAX_UI_SKILL_CLASS_TYPE, MAX_SKILL / MAX_UI_SKILL_CLASS_TYPE );
	m_pSkillBoard->CreateBoardObject( MAX_UI_SKILL_CLASS_TYPE, MAX_SKILL_MASTERY_NUM, MAX_SKILL );

	m_pForceBoard->Create( IBT_FORCE, MAX_UI_FORCE_CLASS_TYPE, MAX_FORCE / MAX_UI_FORCE_CLASS_TYPE );
	m_pForceBoard->CreateBoardObject( MAX_UI_FORCE_CLASS_TYPE, MAX_FORCE_MASTERY_NUM, MAX_FORCE );

	return TRUE;
}

BOOL
CCharacterDataMgr::LoadSkillForceData( void )
{
	CDataFile fileSkillforce( "./DataTable/SkillForce.edf" );

	CDataString * pSourceData = fileSkillforce.GetSourceData();
	if( !pSourceData )
		return FALSE;
	if( !pSourceData->GetString() )
		return FALSE;

	// skill
	pSourceData->Read( &m_dwMaxSkillNum, sizeof( DWORD ), 1 );
	m_listSkill = new SKILL_DATA[m_dwMaxSkillNum];
	pSourceData->Read( m_listSkill, sizeof( SKILL_DATA ), m_dwMaxSkillNum );

	// class skill
	pSourceData->Read( &m_dwMaxClassSkillNum, sizeof( DWORD ), 1 );
	m_listClassSkill = new SKILL_DATA[m_dwMaxClassSkillNum];
	pSourceData->Read( m_listClassSkill, sizeof( SKILL_DATA ), m_dwMaxClassSkillNum );

	// force
	pSourceData->Read( &m_dwMaxForceNum, sizeof( DWORD ), 1 );
	m_listForce = new FORCE_DATA[m_dwMaxForceNum];
	pSourceData->Read( m_listForce, sizeof( FORCE_DATA ), m_dwMaxForceNum );

	return TRUE;
}

BOOL
CCharacterDataMgr::LoadNpcData( void )
{
	CDataFile fileNpcData( "./DataTable/Store.edf" );

	CDataString * pSourceData = fileNpcData.GetSourceData();
	if( !pSourceData )
		return FALSE;
	if( !pSourceData->GetString() )
		return FALSE;

	DWORD dwRecordSize;
	pSourceData->Read( &m_dwMaxMerchant, sizeof( DWORD ), 1 );
	pSourceData->Read( &dwRecordSize, sizeof( DWORD ), 1 );

	m_listMarchant = new DT_CNMInfo[m_dwMaxMerchant];
	pSourceData->Read( m_listMarchant, sizeof( DT_CNMInfo ), m_dwMaxMerchant );

	return TRUE;
}

BOOL
CCharacterDataMgr::LoadCharacterData( void )
{
	CDataFile  fileData( "./DataTable/Character.edf" );

	m_listPlayerClass.LoadData( fileData.GetSourceData() );
	m_listMonster.LoadData( fileData.GetSourceData() );
	m_listAnimus.LoadData( fileData.GetSourceData() );

	m_byMaxAnimusTypeNum = m_listAnimus.GetTotalNum() / MAX_ANIMUS_LEVEL;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

double
CCharacterDataMgr::GetNeedExpForLevelup( BYTE pi_byRaceType, BYTE pi_byLevel )
{
	double		l_dbLevelUpExp;
	_exp_fld *	l_pTableField;

	l_pTableField = ( _exp_fld * )m_tableExp.GetRecord( pi_byLevel );
	if( !l_pTableField )
		return 0;

	if( ( pi_byRaceType == CRT_BELLATOR_MALE ) || ( pi_byRaceType == CRT_BELLATOR_FEMALE ) )
		l_dbLevelUpExp = atof( l_pTableField->m_strBellato );
	else if( ( pi_byRaceType == CRT_CORA_MALE ) || ( pi_byRaceType == CRT_CORA_FEMALE ) )
		l_dbLevelUpExp = atof( l_pTableField->m_strCora );
	else
		l_dbLevelUpExp = atof( l_pTableField->m_strAccretia );

	return  l_dbLevelUpExp;
}