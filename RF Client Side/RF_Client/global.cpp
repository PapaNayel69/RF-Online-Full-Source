////////////////////////////////////////////////////////////////////////////////
//
// Global Class and Data
//
////////////////////////////////////////////////////////////////////////////////

#include "./GameObject/Manager/CCharacterMgr.h"
#include "./DataTable/CCharacterDataMgr.h"
#include "./DataTable/CItemDataMgr.h"

#include "./UserInterface/GameWindow/GUIChatWindow.h"

#include "CLand.h"
#include "CCamera.h"
#include "Global.h"
#include "DataFile.h"
#include "./DataTable/CResourceDataMgr.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////    Resoruce Data List    ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCharResDataMgr * g_pCharResDataMgr = NULL;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                       ///////////////////////////////
//////////////////////////     Game Progress     ///////////////////////////////
//////////////////////////                       ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CMainApp *		g_pMainApp	= NULL;

CGP_MainGame *	g_pMainGame	= NULL;

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////          Land            ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CLand * g_pLand	= NULL;				// 현재 랜드에 관한 포인터

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////          Level           ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CLevel * g_pLevel = NULL;			// 현재의 레벨에 대한 포인터
									// 레벨을 생성할 때를 포인터를 값을 넣는다.
									// 이후에는 무조건 읽기 속성으로 처리

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////     Network Manager      ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetworkMgr * g_pNetworkMgr = NULL;	// 네트웍 매니져에 대한 포인터

/*
BOOL
SendNetMessage( DWORD pi_dwClientIndex, BYTE * pi_pType, char * pi_pMsg, int pi_nLen )
{
	return ( g_pNetworkMgr->m_pProcess[0]->LoadSendMsg(	pi_dwClientIndex,
														pi_pType,
														pi_pMsg,
														pi_nLen ) );
}*/

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////    Character Manager     ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCharacterMgr * g_pCharacterMgr = NULL;

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////       Item Manager       ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////          Avatar          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CAvatar * g_pAvatar = NULL;

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////          Camera          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCamera * g_pCamera = NULL;

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////    Character Interface   ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

ChInterface *	g_ifCharIF			= NULL;
DWORD			g_dwCharIFRefCnt	= 0;

BOOL
_CreateCharIF( LPDIRECT3DDEVICE8 pi_pD3DDevice )
{
	if( g_ifCharIF != NULL ) return FALSE;
	if( pi_pD3DDevice == NULL ) return FALSE;

	// 캐릭터 인터페이스 객체 생성
	if( ( g_ifCharIF == NULL ) && ( g_dwCharIFRefCnt == 0 ) )
	{
		g_ifCharIF = new ChInterface;

		if( g_ifCharIF == NULL ) return FALSE;
	}

	// 캐릭터 인터페이스 초기화
	g_ifCharIF->InitCharacter( pi_pD3DDevice );

	return TRUE;
}

BOOL
_DestroyCharIF( void )
{
	if( g_ifCharIF == NULL ) return FALSE;

	// 캐릭터 인터페이스 파괴
	if( ( g_ifCharIF != NULL ) && ( g_dwCharIFRefCnt == 0 ) )
	{
		delete g_ifCharIF;
		g_ifCharIF = NULL;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////           Bone           ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CUnityBoneMgr::CUnityBoneMgr()
{
	Init();
}

CUnityBoneMgr::~CUnityBoneMgr()
{
	Destroy();
}

void
CUnityBoneMgr::Init( void )
{
	for( int i = 0; i < MAX_BONE_MGR; i++ )
		m_pBoneMgr[i] = NULL;
}

BOOL
CUnityBoneMgr::Create( void )
{
	if( m_pBoneMgr[0] )	return FALSE;
	if( !g_ifCharIF )	return FALSE;

	for( int i = 0; i < MAX_BONE_MGR; i++ )
		m_pBoneMgr[i] = new CObjectManager;

	return TRUE;
}

BOOL
CUnityBoneMgr::Destroy( void )
{
	if( !m_pBoneMgr[0] )
		return FALSE;

	for( int i = 0; i < MAX_BONE_MGR; i++ )
	{
		delete m_pBoneMgr[i];
		m_pBoneMgr[i] = NULL;
	}

	return TRUE;
}

CBone *
CUnityBoneMgr::GetBoneByObject( CObject * pi_pObject, DWORD pi_dwBoneIndex )
{/*
	if( !pi_pObject )
		return NULL;

	if( pi_pObject->GetObjectTypeID() == OTI_CHAR )
	{
		CCharacter * pChar = static_cast< CCharacter * >( pi_pObject );
		if( pChar->IsEqualCharType( CTI_AVATAR ) || pChar->IsEqualCharType( CTI_PLAYER ) )
		{
			// 유닛을 타고 있는지를 검사해야 한다.
			CPlayer * l_pPlayer = ( CPlayer * )pi_pObject;
			if( l_pPlayer->IsMountedUnit() )
				return GetBone( RLI_UNIT, pi_dwBoneIndex );
			else
				return GetBone( RLI_PLAYER, pi_dwBoneIndex );
		}
		else if( pChar->IsEqualCharType( CTI_MONSTER ) )
		{
			return GetBone( RLI_MONSTER, pi_dwBoneIndex );
		}
		else if( pChar->IsEqualCharType( CTI_ANIMUS ) )
		{
			return GetBone( RLI_ANIMUS, pi_dwBoneIndex );
		}
		else if( pChar->IsEqualCharType( CTI_NPC ) )
		{
			return GetBone( RLI_NPC, pi_dwBoneIndex );
		}
	}
	else if( pi_pObject->GetObjectTypeID() == OTI_ITEM )
	{
		// 아이템은 메시가 곧 본이다.
		if( ((CBaseItem *)pi_pObject)->IsEqualItemType( ITI_GROUND ) )
			return _GetMeshMgr()->GetMesh( RLI_ITEM, pi_dwBoneIndex | 0x000000FF );
		else
			return _GetMeshMgr()->GetMesh( RLI_ITEM, pi_dwBoneIndex );
	}
*/
	return NULL;
}

CBone *
CUnityBoneMgr::GetBone( DWORD pi_dwBoneMgrIndex, DWORD pi_dwBoneIndex )
{
	CBone* l_pBone = m_pBoneMgr[pi_dwBoneMgrIndex]->GetCharacter( pi_dwBoneIndex );
	if( l_pBone )
	{
		if( !l_pBone->m_Load )
			_GetCharIF()->LoadRealData( l_pBone, &_GetCharIF()->m_TM, true );
	}

	return l_pBone;
}

//------------------------------------------------------------------------------

CUnityBoneMgr * g_pBoneMgr = NULL;

BOOL
_CreateBoneMgr( void )
{
	if( g_pBoneMgr != NULL ) return FALSE;

	g_pBoneMgr = new CUnityBoneMgr;

	g_pBoneMgr->Create();

	return TRUE;
}

BOOL
_DestroyBoneMgr( void )
{
	if( g_pBoneMgr == NULL ) return FALSE;

	g_pBoneMgr->Destroy();

	delete g_pBoneMgr;
	g_pBoneMgr = NULL;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////           Mesh           ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CUnityMeshMgr::CUnityMeshMgr()
{
	Init();
}

CUnityMeshMgr::~CUnityMeshMgr()
{
	Destroy();
}

void
CUnityMeshMgr::Init( void )
{
	for( int i = 0; i < MAX_MESH_MGR; i++ )
		m_pMeshMgr[i] = NULL;
}

BOOL
CUnityMeshMgr::Create( void )
{
	if( m_pMeshMgr[0] )	return FALSE;
	if( !g_ifCharIF )	return FALSE;

	for( int i = 0; i < MAX_MESH_MGR; i++ )
		m_pMeshMgr[i] = new CObjectManager;

	return TRUE;
}

BOOL
CUnityMeshMgr::Destroy( void )
{
	if( !m_pMeshMgr[0] )
		return FALSE;

	for( int i = 0; i < MAX_MESH_MGR; i++ )
	{
		delete m_pMeshMgr[i];
		m_pMeshMgr[i] = NULL;
	}

	return TRUE;
}

CMesh *
CUnityMeshMgr::GetMesh( DWORD pi_dwMeshMgrIndex, DWORD pi_dwObjectIndex )
{
	CMesh * l_pMesh = m_pMeshMgr[pi_dwMeshMgrIndex]->GetCharacter( pi_dwObjectIndex );
	if( l_pMesh )
	{
		if( !l_pMesh->m_Load )
			_GetCharIF()->LoadRealData( l_pMesh, &_GetCharIF()->m_TM, true );
	}

	return l_pMesh;
}

//------------------------------------------------------------------------------

CUnityMeshMgr *	g_pMeshMgr = NULL;

BOOL
_CreateMeshMgr( void )
{
	if( g_pMeshMgr != NULL ) return FALSE;
	if( g_ifCharIF == NULL ) return FALSE;

	g_pMeshMgr = new CUnityMeshMgr;

	g_pMeshMgr->Create();

	return TRUE;
}

BOOL
_DestroyMeshMgr( void )
{
	if( g_pMeshMgr == NULL ) return FALSE;

	g_pMeshMgr->Destroy();

	delete g_pMeshMgr;
	g_pMeshMgr = NULL;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Animation         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CUnityAniMgr::CUnityAniMgr()
{
	Init();
}

CUnityAniMgr::~CUnityAniMgr()
{
	Destroy();
}

void
CUnityAniMgr::Init( void )
{
	for( int i = 0; i < MAX_ANI_MGR; i++ )
		m_pAniMgr[i] = NULL;
}

BOOL
CUnityAniMgr::Create( void )
{
	if( m_pAniMgr[0] )	return FALSE;
	if( !g_ifCharIF )	return FALSE;

	for( int i = 0; i < MAX_ANI_MGR; i++ )
		m_pAniMgr[i] = new AnimationManager;

	return TRUE;
}

BOOL
CUnityAniMgr::Destroy( void )
{
	if( !m_pAniMgr[0] )
		return FALSE;

	for( int i = 0; i < MAX_ANI_MGR; i++ )
	{
		delete m_pAniMgr[i];
		m_pAniMgr[i] = NULL;
	}

	return TRUE;
}

CAniData *
CUnityAniMgr::GetAni( DWORD pi_dwAniMgrIndex, DWORD pi_dwObjectIndex )
{
	CAniData * l_pAni = m_pAniMgr[pi_dwAniMgrIndex]->GetAnimationData( pi_dwObjectIndex );
	if( l_pAni )
	{
		if( !l_pAni->m_Load )
			l_pAni->LoadRealAnimation( TRUE );
	}

	return l_pAni;
}

//------------------------------------------------------------------------------

CUnityAniMgr * g_pAnimationMgr = NULL;

BOOL
_CreateAnimationMgr( void )
{
	if( g_pAnimationMgr != NULL ) return FALSE;
	if( g_ifCharIF == NULL ) return FALSE;

	g_pAnimationMgr = new CUnityAniMgr;

	g_pAnimationMgr->Create();

	return TRUE;
}

BOOL
_DestroyAnimationMgr( void )
{
	if( g_pAnimationMgr == NULL ) return FALSE;

	g_pAnimationMgr->Destroy();

	delete g_pAnimationMgr;
	g_pAnimationMgr = NULL;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////   Character Data Table   ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCharacterDataMgr * g_pCharDataMgr = NULL;

//------------------------------------------------------------------------------

BOOL
_CreateCharDataMgr( void )
{
	if( g_pCharDataMgr != NULL ) return FALSE;

	g_pCharDataMgr = new CCharacterDataMgr;

	return TRUE;
}

BOOL
_DestroyCharDataMgr( void )
{
	if( g_pCharDataMgr == NULL ) return FALSE;

	delete g_pCharDataMgr;
	g_pCharDataMgr = NULL;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////     Item Data Table      ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CItemDataMgr * g_pItemDataMgr = NULL;

//------------------------------------------------------------------------------

BOOL
_CreateItemDataMgr( void )
{
	if( g_pItemDataMgr != NULL ) return FALSE;

	g_pItemDataMgr = new CItemDataMgr;

	return TRUE;
}

BOOL
_DestroyItemDataMgr( void )
{
	if( g_pItemDataMgr == NULL ) return FALSE;

	delete g_pItemDataMgr;
	g_pItemDataMgr = NULL;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Bound Box         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CBoundBoxManager::CBoundBoxManager()
{
}

CBoundBoxManager::~CBoundBoxManager()
{
	BBOX  * l_pBBox = NULL;

	for( int j = 0; j < MAX_BBOX_MGR; ++j )
	{
		for( BBoxList::iterator i = m_stdBBoxList[j].begin(); i != m_stdBBoxList[j].end(); i++ )
		{
			l_pBBox = *i;

			delete l_pBBox;

			i = m_stdBBoxList[j].erase( i );
			i--;
		}

		m_stdBBoxList[j].clear();
	}
}

BBOX *
CBoundBoxManager::LoadBoundBox( DWORD pi_dwBBoxMgrIndex, char * pi_pName, int pi_nBoneID )
{
	assert( pi_pName != NULL );
	assert( _GetCharIF() != NULL );
	assert( pi_nBoneID != -1 );

	float	l_dwTemp;
	BBOX *	l_pBBox = new BBOX;

	// 바운드 박스를 불러온다.
	if( !_GetCharIF()->ImportBoundBox( pi_pName, l_pBBox->pMin, l_pBBox->pMax ) )
	{
		delete l_pBBox;
		return NULL;
	}

	l_dwTemp			= l_pBBox->pMin[1];
	l_pBBox->pMin[1]	= l_pBBox->pMin[2];
	l_pBBox->pMin[2]	= l_dwTemp;

	l_dwTemp			= l_pBBox->pMax[1];
	l_pBBox->pMax[1]	= l_pBBox->pMax[2];
	l_pBBox->pMax[2]	= l_dwTemp;

	l_pBBox->dwBoneID	= pi_nBoneID;

	// 리스트에 삽입
	m_stdBBoxList[pi_dwBBoxMgrIndex].push_back( l_pBBox );

	return l_pBBox;
}

BBOX *
CBoundBoxManager::GetBoundBox( DWORD pi_dwBBoxMgrIndex, int pi_nBoneID )
{
	if( pi_nBoneID == ID_INVALID )
		return NULL;

	BBOX * l_pBBox = NULL;

	for( BBoxList::iterator i = m_stdBBoxList[pi_dwBBoxMgrIndex].begin(); i != m_stdBBoxList[pi_dwBBoxMgrIndex].end(); i++ )
	{
		l_pBBox = *i;
		if( l_pBBox->dwBoneID == pi_nBoneID )
			return l_pBBox;
	}

	return NULL;
}

BBOX *
CBoundBoxManager::GetBoundBoxByObject( CObject * pi_pObject, int nBoneID )
{
	/*
	if( !pi_pObject )
		NULL;

	if( pi_pObject->GetObjectTypeID() == OTI_CHAR )
	{
		CCharacter * pChar = static_cast< CCharacter * >( pi_pObject );
		if( pChar->IsEqualCharType( CTI_AVATAR ) || pChar->IsEqualCharType( CTI_PLAYER ) )
		{
			// 유닛을 타고 있는지를 검사해야 한다.
			CPlayer * l_pPlayer = ( CPlayer * )pi_pObject;
			if( l_pPlayer->IsMountedUnit() )
				return GetBoundBox( RLI_UNIT, nBoneID );
			else
				return GetBoundBox( RLI_PLAYER, nBoneID );
		}
		else if( pChar->IsEqualCharType( CTI_MONSTER ) )
		{
			return GetBoundBox( RLI_MONSTER, nBoneID );
		}
		else if( pChar->IsEqualCharType( CTI_ANIMUS ) )
		{
			return GetBoundBox( RLI_ANIMUS, nBoneID );
		}
		else if( pChar->IsEqualCharType( CTI_NPC ) )
		{
			return GetBoundBox( RLI_NPC, nBoneID );
		}
	}
	else if( pi_pObject->GetObjectTypeID() == OTI_ITEM )
	{
		// 아이템은 메시가 곧 본이다.
		return GetBoundBox( RLI_ITEM, nBoneID );
	}*/

	return NULL;
}

//------------------------------------------------------------------------------

CBoundBoxManager *	g_pBoundBoxMgr		= NULL;
int					g_nTotalBoundBoxNum	= 0;

BOOL
_CreateBoundBoxMgr( void )
{
	if( g_pBoundBoxMgr != NULL ) return FALSE;

	g_pBoundBoxMgr = new CBoundBoxManager;

	return TRUE;
}

BOOL
_DestroyBoundBoxMgr( void )
{
	if( g_pBoundBoxMgr == NULL ) return FALSE;

	delete g_pBoundBoxMgr;
	g_pBoundBoxMgr = NULL;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////    Effect Queue 관련     ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

EffectInfo::EffectInfo()
{
	ZeroMemory( this, sizeof( EffectInfo ) );

	dwType					= ID_INVALID;
	dwID					= ID_INVALID;
	dwMyself[0]				= ID_INVALID;
	dwMyself[1]				= ID_INVALID;
	dwTarget[0]				= ID_INVALID;
	dwTarget[1]				= ID_INVALID;
	byRangeType				= SFRT_ONE_HIT;
	byRangeType_For_Effect	= ID_INVALID;
}

void
EffectInfo::SetMyself( CCharacter * pi_pMyself )
{
	if( pi_pMyself )
	{
		dwMyself[0]	= pi_pMyself->GetCharTypeID();
		dwMyself[1]	= pi_pMyself->GetIndex();
	}
	else
	{
		dwMyself[0]	= ID_INVALID;
		dwMyself[1]	= ID_INVALID;
	}
}

void
EffectInfo::SetTarget( CCharacter * pi_pTarget )
{
	if( pi_pTarget )
	{
		dwTarget[0]	= pi_pTarget->GetCharTypeID();
		dwTarget[1]	= pi_pTarget->GetIndex();
	}
	else
	{
		dwTarget[0]	= ID_INVALID;
		dwTarget[1]	= ID_INVALID;
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CEffectInfoList::CEffectInfoList()
{
}

CEffectInfoList::~CEffectInfoList()
{
	Clear();
}

void
CEffectInfoList::Clear( void )
{
	for( EFFECT_INFO_LIST_ITER iter = m_listEffectInfo.begin(); iter != m_listEffectInfo.end(); ++iter )
		delete ( *iter );

	m_listEffectInfo.clear();
}

void
CEffectInfoList::push_back( EffectInfo * pi_pEffectInfo )
{
	EffectInfo * l_pNewEffectInfo = new EffectInfo;
	memcpy( l_pNewEffectInfo, pi_pEffectInfo, sizeof( EffectInfo ) );
	m_listEffectInfo.push_back( l_pNewEffectInfo );
}

EFFECT_INFO_LIST_ITER
CEffectInfoList::erase( EFFECT_INFO_LIST_ITER & pi_iter )
{
	delete ( *pi_iter );
	( *pi_iter ) = NULL;
	return m_listEffectInfo.erase( pi_iter );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CReturnEffectInfoList::CReturnEffectInfoList()
{
}

CReturnEffectInfoList::~CReturnEffectInfoList()
{
	Clear();
}

void
CReturnEffectInfoList::Clear( void )
{
	for( EFFECT_INFO_LIST_ITER iter = m_listEffectInfo.begin(); iter != m_listEffectInfo.end(); ++iter )
		delete ( *iter );

	m_listEffectInfo.clear();
}

void
CReturnEffectInfoList::ResultProcessing( void )
{
	if( m_listEffectInfo.empty() )
		return;

	EffectInfo* l_pEffectInfo	= NULL;
	CCharacter* l_pChar			= NULL;
	BOOL		l_bIsExecution	= FALSE;

	for( EFFECT_INFO_LIST_ITER iter = m_listEffectInfo.begin(); iter != m_listEffectInfo.end(); ++iter )
	{
		if( !IsAttackEffect( iter ) )
		{
			iter = erase( iter );
			--iter;
			continue;
		}

		if( SetDamage( iter ) )
		{
			iter = erase( iter );
			--iter;
			continue;
		}
	}
}

//------------------------------------------------------------------------------

BOOL
CReturnEffectInfoList::IsAttackEffect( EFFECT_INFO_LIST_ITER & pi_iter )
{
	if( ( ( *pi_iter )->dwType == ETI_SKILL ) || ( ( *pi_iter )->dwType == ETI_FORCE ) )
		return IsAttackEffect_SkillForce( pi_iter );
	else
		return IsAttackEffect_Normal( pi_iter );	
}

BOOL
CReturnEffectInfoList::IsAttackEffect_SkillForce( EFFECT_INFO_LIST_ITER & pi_iter )
{
	EffectInfo * l_pEffectInfo = ( *pi_iter );

	// 시전 코드라면 무조건 pop한 후 return
	static const BYTE	CAST_CONST[2]	= { 2, 6 };
	static const DWORD	LEVEL_CONST[2]	= { 0x0000000F, 0x000000FF };
	BYTE l_byType = l_pEffectInfo->dwType - ETI_SKILL;
	if( ( l_pEffectInfo->dwID & LEVEL_CONST[l_byType] ) <= CAST_CONST[l_byType] )
		return FALSE;

	// 데미지 코드라면 무조건 pop한 후 return
	static const BYTE	DAMAGE_CONST[2]	= { 6, 14 };
	if( ( l_pEffectInfo->dwID & LEVEL_CONST[l_byType] ) >= DAMAGE_CONST[l_byType] )
		return FALSE;

	// 스킬일 경우 보조 스킬이라면 처리하지 않는다.
	if( l_pEffectInfo->dwType == ETI_SKILL )
	{
		if( ( l_pEffectInfo->dwID & 0x00F0000 ) == 0x00F0000 )
			return FALSE;
	}
	// 포스일 경우 캐릭터에 맞고 돌아온 이펙트인지를 검사해야한다.
	// 캐릭터에 맞지 않고 돌아온 이펙트는 처리하지 않는다.
	else if( l_pEffectInfo->dwType == ETI_FORCE )
	{
		if( !l_pEffectInfo->dwValue && ( l_pEffectInfo->dwTarget[0] != ID_INVALID ) )
			return FALSE;
	}

	return TRUE;
}

BOOL
CReturnEffectInfoList::IsAttackEffect_Normal( EFFECT_INFO_LIST_ITER & pi_iter )
{
	EffectInfo * l_pEffectInfo = ( *pi_iter );

	// 공격 관련 코드가 아니라면 무조건 pop한 후 return
	if( l_pEffectInfo->dwType == ETI_PLAYER )
	{
		return FALSE;
	}
	else if( l_pEffectInfo->dwType == ETI_MONSTER )
	{
		BYTE l_byCode = ( l_pEffectInfo->dwID >> 4 ) & 0x0000000F;
		if( !( l_byCode == MET_ATTACK_NORMAL || l_byCode == MET_ATTACK_FORCE ) )
			return FALSE;
	}
	else if( l_pEffectInfo->dwType == ETI_UNIT )
	{
		BYTE l_byCode = l_pEffectInfo->dwID & 0x0000000F;
		if( l_byCode != UET_ATTACK )
			return FALSE;
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CReturnEffectInfoList::SetDamage( EFFECT_INFO_LIST_ITER & pi_iter )
{
	EffectInfo * pEffectInfo = ( *pi_iter );

	CCharacter * pAttacker = _GetCharMgr()->GetCharacter( pEffectInfo->dwMyself[0], pEffectInfo->dwMyself[1] );
	if( !pAttacker )
		return TRUE;

	// 캐릭터가 네트웍 메세지를 받았을 경우에만 설정
	// 더미일 경우에도 설정
	if( ( pAttacker->GetCombatState()->IsReceiveAttackResultMsg() ) ||
		( pAttacker->IsEqualCharType( CTI_DUMMY ) ) )
	{
		/*
		// 만약 attacker와 victim이 동일한데 CAttackerInfo가 NULL 이라면 지워버린다.
		// 지역 공격형 스킬/포스를 사용할 경우, 아바타도 맞았다고 돌아올 때 사용된다.
		CCharacter * pVictim = _GetCharMgr()->GetCharacter( pEffectInfo->dwTarget[0], pEffectInfo->dwTarget[1] );
		if( pAttacker->IsEqual( pVictim ) )
		{
			CAttackerInfo *pAttackerInfo = static_cast< CAttackerInfo * >(
											pVictim->GetCombatState()->GetEnemyInfo( ET_ATTACKER,
																		 pAttacker->GetCharTypeID(),
																		 pAttacker->GetIndex() ) );
			if( !pAttackerInfo )
				return TRUE;
		}*/

		// 다수를 공격하는 경우
		if( ( pEffectInfo->byRangeType == SFRT_SELF_AREA ) ||
			( pEffectInfo->byRangeType == SFRT_FLASH_DAMAGE ) ||
			( pEffectInfo->byRangeType == SFRT_EXTENT ) )
		{
			SetDamage_ManyCharacter( pAttacker );
		}
		// 한 캐릭터만 공격하는 경우
		else
		{
			SetDamage_OneCharacter( pAttacker );
		}

		// pAttacker의 이펙트 프레임이 마지막 프레임이어야 한다.
		if( pAttacker->IsPassedLastEffectFrame() )
		{
			pAttacker->GetCombatState()->SetReceiveAttackResultMsg( FALSE );
			pAttacker->GetCombatState()->DeleteAllEnemyInfo( ET_VICTIM );
		}

		return TRUE;
	}

	return FALSE;
}

void
CReturnEffectInfoList::SetDamage_OneCharacter( CCharacter * pi_pChar )
{
	CCharacter *	l_pVictim;
	CAttackerInfo *	l_pAttackerInfo;

	for( int i = 0; i < pi_pChar->GetCombatState()->GetTotalEnemyNum( ET_VICTIM ); ++i )
	{
		l_pVictim = pi_pChar->GetCombatState()->GetEnemyByOrder( ET_VICTIM, i );
		if( !l_pVictim )
			continue;

		l_pAttackerInfo = static_cast< CAttackerInfo * >(
						  l_pVictim->GetCombatState()->GetEnemyInfo( ET_ATTACKER,
																	 pi_pChar->GetCharTypeID(),
																	 pi_pChar->GetIndex() )
						  );
		if( l_pAttackerInfo )
			l_pAttackerInfo->SetDamaged( TRUE );
	}
}

void
CReturnEffectInfoList::SetDamage_ManyCharacter( CCharacter * pi_pChar )
{
	_GetCharMgr()->ProcessDamageFromReturnEffectMgr( pi_pChar );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CEffectInfoList			g_pEffectInfoList;
CReturnEffectInfoList 	g_pReturnEffectInfoList;

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////    네트웍 메세지 관련    ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL		g_bIsSaveNetworkMessage;		// 네트웍 메세지를 저장할 것인가?
int			g_nTotalNetworkMessage;			// 현재 저장중인 네트웍 메세지의 총 갯수

NetMsgQueue	g_stdNetMsgQueue;				// 네트웍 메세지 저장

CLogFile	g_logClientNetMsg;

void
_CreateNetLog( void )
{
	g_logClientNetMsg.SetWriteLogFile( ".\\NetLog\\Client-NetMsg.log", TRUE );
}

void
_SaveNetMsg( char * pi_pMsg )
{
	if( /* !g_bIsSaveNetworkMessage ||*/ !pi_pMsg )
		return;

	NetMsgStr l_netMsg;
	memcpy( l_netMsg.msg, pi_pMsg, strlen( pi_pMsg ) );

	g_stdNetMsgQueue.push( l_netMsg );

	if( g_stdNetMsgQueue.size() >= NETMSG_MAX_NUM - 1 )
		g_stdNetMsgQueue.pop();

	//--------------------------------------------------------------------------

	g_logClientNetMsg.Write( pi_pMsg );

	/*
	FILE * l_pFile = fopen( "./NetLog/Client-NetMsg.log", "at+" );
		
	if( l_pFile )
	{
		fwrite( l_netMsg.msg, strlen( pi_pMsg ), 1, l_pFile );
	}
	fclose( l_pFile );
	*/
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////   User Interface 관련    ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CSpriteMgr *			g_pUiSpriteMgr				= NULL;
CInputMgr *				g_pUiInputMgr				= NULL;
CMouseDev *				g_pUiMouseDev				= NULL;
CGUIScreen *			g_pUiScreen					= NULL;
CGUIMouse *				g_pUiMouse					= NULL;

CGUIGaugeExpWindow *	g_pUiGaugeWindow			= NULL;
CGUIItemBeltWindow *	g_pUiItemBeltWindow			= NULL;
CGUIForceBeltWindow *	g_pUiSFBeltWindow			= NULL;
CGUIChatWindow *		g_pUiChatWindow				= NULL;
CGUIChatInputWindow *	g_pUiChatInputWindow		= NULL;
CGUIInventoryWindow *	g_pUiInventWindow			= NULL;
CGUIShopWindow *		g_pUiShopWindow				= NULL;
CGUIForceWindow *		g_pUiForceWindow			= NULL;
CGUISkillWindow *		g_pUiSkillWindow			= NULL;
CGUICharInfoWindow *	g_pUiCharInfoWindow			= NULL;
CGUIHotKeyBoard *		g_pUiFuncKeyWindow			= NULL;
CGUIAttackPartWindow *	g_pUiSelectAttackPartWindow	= NULL;
CGUIMiningWindow *		g_pUiMiningWindow			= NULL;
CGUIOreProcWindow *		g_pUiOreProcWindow			= NULL;
CGUIExchangeWindow *	g_pUiExchangeWindow			= NULL;
CGUIRecallWindow *		g_pUiSummonsSelectWindow	= NULL;
CGUIMobControl *		g_pUiSummonsControlWindow	= NULL;
CGUINpcSelectWindow *	g_pUiNpcIntroduceWindow		= NULL;
CGUIItemMakeWindow *	g_pUiItemWorkWindow			= NULL;
CGUIItemUpgradeWindow *	g_pUiItemUpgradeWindow		= NULL;
//CGUISelectedCharWin *	g_pUiTargetInfoBar			= NULL;
//CGUISelectedMobInfoWin*	g_pUiTargetInfoWindow		= NULL;
CGUIPartyWindow *		g_pUiPartyWindow			= NULL;
//CGUISelectedCharMenu *	g_pUiSelectedCharMenu		= NULL;
CGUIMapMoveWindow	*	g_pUiMoveMapWindow			= NULL;
CGUISkillForceQueue *	g_pUiAppliedSFWindow		= NULL;
CGUITradeWindow *		g_pUiTradeWindow			= NULL;
CGUIRadarWindow *		g_pUiRadarWindow			= NULL;
CGUIMiniMapWindow *		g_pUiMinimapWindow			= NULL;
CGUIMapLoadingWindow *	g_pUiMapLoadingWindow		= NULL;
CGUIOptionWindow *		g_pUiOptionWindow			= NULL;

// BY B.H.K
// 서버로부터 UI event에 대한 결과를 받기 전까지 
// 다른 UI event가 일어나지 못하게 하기 위한 flag이다.
BOOL	g_bUiWaitServerMsg	=	FALSE;

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////       Avatar Dummy       ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CAvatar * g_pAvatarDummy = NULL;

void
_CreateAvatarDummy( void )
{
	if( !g_pAvatarDummy )
	{
		g_pAvatarDummy = new CAvatar[MAX_AVATAR_DUMMY];
	}
}

void
_DestroyAvatarDummy( void )
{
	if( g_pAvatarDummy )
	{
		delete [] g_pAvatarDummy;
		g_pAvatarDummy = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////        Message        /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

/*
void
_SetSystemMsg( char * pi_pMessage, BOOL pi_bIsNotify )
{
	static DWORD l_dwSystemCount = 0;
	static char	 l_pBuf[32];

	if( !_GetChatWindow() )
		return;

	ZeroMemory( l_pBuf, 32 );
	if( pi_bIsNotify )
	{
		for( int i = 0; i < strlen( pi_pMessage ); ++i )
		{
			if( pi_pMessage[i] == ' ' )
				break;

			l_pBuf[i] = pi_pMessage[i];
		}

		if( strcmp( l_pBuf, "팁" ) == 0 )
			_GetChatWindow()->InsertText( NULL, pi_pMessage, CGUIChatWindow::FS_TIP );
		else
			_GetChatWindow()->InsertText( NULL, pi_pMessage, CGUIChatWindow::FS_NOTIFY );
	}
	else
	{
		sprintf( l_pBuf, "[%d] SYSTEM", ++l_dwSystemCount );
		_GetChatWindow()->InsertText( l_pBuf, pi_pMessage, CGUIChatWindow::FS_SYSTEM );
	}
}

void
_SetSystemMsg( DWORD pi_dwID, BOOL pi_bIsNotify )
{
	if( !_GetChatWindow() )
		return;

	char * l_pMsg = _GetMessageMgr()->GetMessage( pi_dwID );
	if( l_pMsg )
	{
		_GetChatWindow()->InsertText( "SYSTEM", l_pMsg, CGUIChatWindow::FS_SYSTEM );
	}
}*/

void
_SetSystemMsg( const char * l_pFormatString, ... )
{
	static DWORD	l_dwSystemCount = 0;
	static char		l_pHeader[32];
	static char		l_pDisplayString[256];
	static va_list	l_pArgumentList;

	if( !_GetChatWindow() )
		return;

	va_start( l_pArgumentList, l_pFormatString );
	vsprintf( l_pDisplayString, l_pFormatString, l_pArgumentList );
	va_end( l_pArgumentList );

	sprintf( l_pHeader, "[%d]", ++l_dwSystemCount );
	_GetChatWindow()->InsertText( l_pHeader, l_pDisplayString, CGUIChatWindow::FS_SYSTEM );
}

void
_SetSystemMsg( DWORD pi_dwID )
{
	if( !_GetChatWindow() )
		return;

	/*
	char * l_pMsg = _GetMessageMgr()->GetMessage( pi_dwID );
	if( l_pMsg )
	{
		_GetChatWindow()->InsertText( "SYSTEM", l_pMsg, CGUIChatWindow::FS_SYSTEM );
	}*/
}

void
_SetNotifyMsg( const char * l_pFormatString, ... )
{
	static DWORD	l_dwSystemCount = 0;
	static char		l_pHeader[32];
	static char		l_pDisplayString[256];
	static va_list	l_pArgumentList;		

	if( !_GetChatWindow() )
		return;

	va_start( l_pArgumentList, l_pFormatString );
	vsprintf( l_pDisplayString, l_pFormatString, l_pArgumentList );
	va_end( l_pArgumentList );

	for( int i = 0; i < strlen( l_pDisplayString ); ++i )
	{
		if( l_pDisplayString[i] == ' ' )
			break;

		l_pHeader[i] = l_pDisplayString[i];
	}

	if( strcmp( l_pHeader, "팁" ) == 0 )
		_GetChatWindow()->InsertText( NULL, l_pDisplayString, CGUIChatWindow::FS_TIP );
	else
		_GetChatWindow()->InsertText( NULL, l_pDisplayString, CGUIChatWindow::FS_NOTIFY );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////           Etc            ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

HWND		g_hWnd = 0;
HINSTANCE	g_hInstance = 0;
float		g_fFPS = 0.0f;
DWORD		g_dwCurTime = 0;

void
_DrawLine( float pi_fX1, float pi_fY1, float pi_fZ1,
		   float pi_fX2, float pi_fY2, float pi_fZ2,
		   DWORD pi_dwColor )
{
	/*
	_D3DR3VERTEX_TEX0 l_vPoint[2];

	l_vPoint[0].x		= pi_fX1;
	l_vPoint[0].y		= pi_fY1;
	l_vPoint[0].z		= pi_fZ1;
	l_vPoint[0].color	= pi_dwColor;

	l_vPoint[1].x		= pi_fX2;
	l_vPoint[1].y		= pi_fY2;
	l_vPoint[1].z		= pi_fZ2;
	l_vPoint[1].color	= pi_dwColor;

	DrawPrimitiveUPLine( 1, l_vPoint );
	*/

	Vector3f l_vecPoint1, l_vecPoint2;

	l_vecPoint1[0] = pi_fX1;
	l_vecPoint1[1] = pi_fY1;
	l_vecPoint1[2] = pi_fZ1;

	l_vecPoint2[0] = pi_fX2;
	l_vecPoint2[1] = pi_fY2;
	l_vecPoint2[2] = pi_fZ2;

	R3DrawLine( l_vecPoint1, l_vecPoint2, pi_dwColor );
}

void
_DrawLine( Vector3f pi_vecPoint1, Vector3f pi_vecPoint2, DWORD pi_dwColor )
{
	/*
	_D3DR3VERTEX_TEX0 l_vPoint[2];

	l_vPoint[0].x		= pi_vecPoint1[0];
	l_vPoint[0].y		= pi_vecPoint1[0];
	l_vPoint[0].z		= pi_vecPoint1[0];
	l_vPoint[0].color	= pi_dwColor;

	l_vPoint[1].x		= pi_vecPoint2[1];
	l_vPoint[1].y		= pi_vecPoint2[1];
	l_vPoint[1].z		= pi_vecPoint2[1];
	l_vPoint[1].color	= pi_dwColor;

	DrawPrimitiveUPLine( 1, l_vPoint );
	*/

	R3DrawLine( pi_vecPoint1, pi_vecPoint2, pi_dwColor );
}

RECT g_rectWindow;
RECT g_rectClient;
BOOL g_bIsWindowed;

//------------------------------------------------------------------------------

char g_pBuf[MAX_GBUF_LENGTH];

//--------------------------------------------------------------------------

void
_strcpy( char * po_pDestBuffer, char * pi_pOrgString, DWORD pi_dwDestBufferSize )
{
	if( po_pDestBuffer == NULL || pi_pOrgString == NULL )
		return;

	DWORD l_dwLimitSize = strlen( pi_pOrgString ) + 1;
	if( l_dwLimitSize >= pi_dwDestBufferSize )
		l_dwLimitSize = pi_dwDestBufferSize - 1;

	ZeroMemory( po_pDestBuffer, pi_dwDestBufferSize );
	memcpy( po_pDestBuffer, pi_pOrgString, l_dwLimitSize );
}

//------------------------------------------------------------------------------

float g_pFrustumPlane[6][4];

void
SetCharacterClipping( void )
{
	assert( _GetCharIF() != NULL );

	D3DXMATRIX	l_matTemp, l_matView, l_matProject;

	GetViewMatrix( &l_matView );
	GetProjectMatrix( &l_matProject );

	D3DXMatrixMultiply( &l_matTemp, &l_matView, &l_matProject);

	SetFustumNormalPlane( g_pFrustumPlane, l_matTemp );
}

//------------------------------------------------------------------------------

void
_GetRenderNamePos( CResObject * pi_pResObject, Vector4f pio_vecPos, BYTE pi_bySizeOfName )
{
	if( !pi_pResObject )
		return;

	Vector3f	l_vecNamePos;
	BBOX		l_sBBox;

	memcpy( l_vecNamePos, pi_pResObject->GetPosition(), sizeof( float ) * 3 );

	/*
	// for rock character
	if( pi_pResObject->GetObjectTypeID() == OTI_CHAR )
	{
		CCharacter * l_pChar = ( CCharacter * )pi_pResObject;
		if( l_pChar->ROCK_IsRock() )
		{
			if( l_pChar->ROCK_IsOdd() )
			{
				l_vecNamePos[0] += 0.5f;
				l_vecNamePos[2] += 0.5f;
			}
			else
			{
				l_vecNamePos[0] -= 0.5f;
				l_vecNamePos[2] -= 0.5f;
			}
		}
	}*/

	/*
	// for rock character
	if( pi_pResObject->GetObjectTypeID() == OTI_CHAR )
	{
		CCharacter * l_pChar = ( CCharacter * )pi_pResObject;
		if( l_pChar->ROCK_IsRock() )
			memcpy( l_vecNamePos, l_pChar->ROCK_StartRockPos(), sizeof( float ) * 3 );
	}*/

	// calculate
	if( pi_pResObject->GetBoundBox( l_sBBox ) )
		l_vecNamePos[1] = l_sBBox.pMax[1];
	else
		l_vecNamePos[1] += 19.0f;

	GetTransformVertex( ( Vector4f * )pio_vecPos, l_vecNamePos );
	pio_vecPos[0] = (float)( (LONG)( pio_vecPos[0] + 0.5f ) ) - ( ( pi_bySizeOfName / 2 ) * FONT_WIDTH );
	pio_vecPos[1] = (float)( (LONG)( pio_vecPos[1] ) ) - FONT_HEIGHT;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
_GetWaypointOfAandB( Vector3f po_vecWaypoint, Vector3f pi_vecCurPos, Vector3f pi_vecTarPos, float pi_fDist )
{
	float l_fYAngle = GetYAngle( pi_vecCurPos, pi_vecTarPos );

	po_vecWaypoint[0] = pi_vecCurPos[0] + ( ( float )sin( AngleToPi( l_fYAngle ) ) * pi_fDist );
	po_vecWaypoint[1] = pi_vecTarPos[1];
	po_vecWaypoint[2] = pi_vecCurPos[2] + ( ( float )cos( AngleToPi( l_fYAngle ) ) * pi_fDist );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

FILE *
_OpenLogFile( void )
{
	FILE * l_pFile = fopen( "./NetLog/rfclient.log", "a" );
	if( l_pFile )
	{
		fseek( l_pFile, 0, SEEK_END );
		fprintf( l_pFile, "" );
		int l_nFileSize = ftell( l_pFile );

		if( l_nFileSize >= 1048576 )
		{
			fclose( l_pFile );
			l_pFile = fopen( "./NetLog/rfclient.log", "w" );
		}
	}

	return l_pFile;
}

void
_WriteLog( const char * pi_pFormat, ... )
{
	char		l_pBuffer[1024];
	char		l_pData[32], l_pTime[32];
	static DWORD	l_dwCount = 0;

	va_list l_pArgumentList;

	va_start( l_pArgumentList, pi_pFormat );
	vsprintf( l_pBuffer, pi_pFormat, l_pArgumentList );
	va_end( l_pArgumentList );

	_strdate( l_pData );
	_strtime( l_pTime );

	char		l_pLog[1024];
	wsprintf( l_pLog, "[%d] %s %s: %s\n", ++l_dwCount, l_pData, l_pTime, l_pBuffer );

	FILE * l_pFile = _OpenLogFile();
	if( l_pFile )
	{
		fprintf( l_pFile, l_pLog );
		fclose( l_pFile );
	}
}