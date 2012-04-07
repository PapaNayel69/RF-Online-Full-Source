////////////////////////////////////////////////////////////////////////////////
//
// CNpc Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"

#include "../../UserInterface/GameWindow/Npc/GUINpcSelectWindow.h"
#include "../../UserInterface/GameWindow/GUIHotKeyBoard.h"
#include "../../UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"

#include "../../GameObject/Board/Common/CBoardManager.h"


////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// CMerchantItemMgr
//
////////////////////////////////////////////////////////////////////////////////

CMerchantItemMgr::CMerchantItemMgr()
{
	m_dwMaxNum		= 0;
	m_dwTotalNum	= 0;
	m_pItemList		= NULL;
}

CMerchantItemMgr::~CMerchantItemMgr()
{
	_DELETE_ARRAY( m_pItemList );
}

BOOL
CMerchantItemMgr::Create( DWORD pi_dwMaxItemNum )
{	
	if( !m_pItemList )
		m_pItemList = new CBoardItem[pi_dwMaxItemNum];

	m_dwMaxNum = pi_dwMaxItemNum;

	return TRUE;
}

//------------------------------------------------------------------------------

CBoardItem *
CMerchantItemMgr::AddItem( DWORD pi_dwOrderIndex )
{
	if( ( pi_dwOrderIndex < 0 ) || ( pi_dwOrderIndex > m_dwTotalNum ) )	// add 시에는 ( pi_dwOrderIndex >= m_dwTotalNum )가 아닌 ( pi_dwOrderIndex > m_dwTotalNum )로 비교해야 한다.
		return NULL;
	
	if( m_pItemList[pi_dwOrderIndex].IsEmpty() )
	{
		m_pItemList[pi_dwOrderIndex].SetIndex( pi_dwOrderIndex );
		++m_dwTotalNum;
	}

	return &m_pItemList[pi_dwOrderIndex];
}

CBoardItem *
CMerchantItemMgr::AddItem2( DWORD pi_dwOrderIndex )
{
	if( ( pi_dwOrderIndex < 0 ) || ( pi_dwOrderIndex > m_dwTotalNum ) )	// add 시에는 ( pi_dwOrderIndex >= m_dwTotalNum )가 아닌 ( pi_dwOrderIndex > m_dwTotalNum )로 비교해야 한다.
		return NULL;

	memcpy( &m_pItemList[pi_dwOrderIndex], &m_pItemList[pi_dwOrderIndex+1], sizeof( CBoardItem ) * ( m_dwTotalNum - pi_dwOrderIndex ) );
	m_pItemList[pi_dwOrderIndex].SetIndex( pi_dwOrderIndex );
	++m_dwTotalNum;

	return &m_pItemList[pi_dwOrderIndex];
}

void
CMerchantItemMgr::DeleteItem( DWORD pi_dwOrderIndex )
{
	if( pi_dwOrderIndex == ID_INVALID )
		return;

	if( !m_pItemList[pi_dwOrderIndex].IsEmpty() )
	{
		m_pItemList[pi_dwOrderIndex].SetEmpty();
		--m_dwTotalNum;
	}
}

void
CMerchantItemMgr::Clear( void )
{
	for( int i = 0; i < m_dwTotalNum; ++i )
		m_pItemList[i].SetEmpty();

	m_dwTotalNum = 0;
}

//------------------------------------------------------------------------------

CBoardItem *
CMerchantItemMgr::GetItem( DWORD pi_dwIndex )
{
	if( pi_dwIndex == ID_INVALID )
		return NULL;

	for( int i = 0; i < m_dwTotalNum; ++i )
	{
		if( m_pItemList[i].IsEqual( pi_dwIndex ) )		
			return &m_pItemList[i];
	}

	return NULL;
}

CBoardItem *
CMerchantItemMgr::GetItemByOrder( DWORD pi_dwOrderIndex )
{
	if( ( pi_dwOrderIndex < 0 ) || ( pi_dwOrderIndex >= m_dwTotalNum ) )
		return NULL;

	if( !m_pItemList[pi_dwOrderIndex].IsEmpty() )
		return &m_pItemList[pi_dwOrderIndex];

	return NULL;
}

CBoardItem *
CMerchantItemMgr::GetItemByOrder2( DWORD pi_dwOrderIndex )
{
	if( pi_dwOrderIndex < 0 )
		return NULL;

	int l_nValidItemNum = 0;

	for( int i = 0; i < m_dwMaxNum; ++i )
	{
		if( i >= m_dwTotalNum )
			return NULL;

		if( !m_pItemList[pi_dwOrderIndex].IsEmpty() )
		{
			if( l_nValidItemNum == pi_dwOrderIndex )
				return &m_pItemList[pi_dwOrderIndex];

			l_nValidItemNum++;
		}
	}

	return NULL;
}

CBoardItem *
CMerchantItemMgr::GetItemByMeshID( DWORD pi_dwMeshID )
{
	if( pi_dwMeshID == ID_INVALID )
		return NULL;

	for( int i = 0; i < m_dwTotalNum; ++i )
	{
		if( !m_pItemList[i].IsEmpty() )
		{
			if( m_pItemList[i].GetMeshID() == pi_dwMeshID )
				return &m_pItemList[i];
		}
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// CNpc
//
////////////////////////////////////////////////////////////////////////////////

CNpc::CNpc()
{
	m_dwCharTypeID		= CTI_NPC;

	m_pData				= NULL;
	m_pMesh				= NULL;

	m_dwTotalItemNum	= ID_INVALID;
	m_dwItemListVersion	= 0;

	m_dwTextColor		= CHAR_NPC_NAME_COLOR;

	m_cItemList.Create();

	Enable( ROSF_RENDER_MESH );
}

CNpc::~CNpc()
{
}

CCharacter *
CNpc::New( void )
{
	return ( dynamic_cast< CCharacter * >( new CNpc ) );
}

CCharacter *
CNpc::Clone( void )
{
	return ( dynamic_cast< CCharacter * >( new CNpc( *this ) ) );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNpc::Render( void )
{
	if( !CCharacter::Render() )
		return FALSE;

	// 렌더링이 가능하고 클리핑 되지 않았어야 렌더링 함.
	if( ( IsEnable( ROSF_RENDER_MESH ) ) && ( IsDisable( ROSF_CLIPPED ) ) )
	{
		if( !m_pMesh )
			return FALSE;

		// 캐릭터가 죽어가는 경우라면 알파값을 점차 줄여준다.
		if( IsEqualAction( CAI_DEATH_DIE ) )
			m_fAlphaDensity -= 0.01f;

		// 알파 적용
		if( IsEnable( ROSF_RENDER_ALPHA ) )
			_GetCharIF()->SetAlpha( m_pMesh, m_fAlphaDensity );

		// 머터리얼 적용
		_GetCharIF()->SetMaterial( m_pMesh, m_d3dMaterial );

		// vertex blending
		if( !SetVertexBlending( 0, m_pMesh, FALSE ) ) 
		{
			if( IsEqualAction( CAI_DEATH_DIE ) || IsEnable( ROSF_RENDER_ALPHA ) )
				_GetCharIF()->SetAlpha( m_pMesh, 1.0f );
		}

		// draw mesh
		_GetCharIF()->DrawCharacter( m_pMesh, m_vecPos, m_vecRot[1] );

		// 메시를 그린후 다시 알파값을 복구시킨다.
		// 복구시키지 않으면 같은 메시를 사용하는 다른 오브젝트도 사라져 보인다.
		if( GetAction() == CAI_DEATH_DIE || IsEnable( ROSF_RENDER_ALPHA ) )
			_GetCharIF()->SetAlpha( m_pMesh, 1.0f );
	}

	return TRUE;
}

// 그림자 렌더링
void
CNpc::RenderShadow( void )
{
}

// 그림자 생성
void
CNpc::CreateShadow( void )
{
	_GetCharIF()->DrawCharacterShadow( GetMesh(),m_vecPos, m_vecRot[1],0.8f * GetAlphaDensity(),m_fScale );
}

BOOL
CNpc::Animation( DWORD pi_dwAniFrame )
{
	if( !CCharacter::Animation( pi_dwAniFrame ) )
		return FALSE;

	if( !m_pMesh )
		return FALSE;

	_GetCharIF()->FrameMove( m_pMesh );

	SetVertexBlending( 0, m_pMesh, TRUE );

	return TRUE;
}

void
CNpc::FrameMove( void )
{
	if( IsDisable( ROSF_MOVABLE ) )
		return;

	// 아바타를 보고 있는 방향으로 틀어준다. ( 거리제한 - 현재는 50 ) => 테스트 코드
	if( GetDist( m_vecPos, _GetAvatar()->GetPosition() ) > 50 )
		SetRotY( m_vecPos, _GetAvatar()->GetPosition() );

	CCharacter::FrameMove();
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// 현재 동작에 알맞은 이펙트로 설정한다.
// TRUE - 이펙트를 발생됐음. FALSE - 이펙트가 발생되지 않았음
BOOL
CNpc::SetEffectInfo( DWORD pi_dwActionID )
{
	if( CCharacter::SetEffectInfo( pi_dwActionID ) )
		return TRUE;

	return FALSE;
}

void
CNpc::SetAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication )
{
	if( pi_dwActionID == ID_INVALID )
		return;

	SetAnimationByAction( pi_dwActionID );

	CCharacter::SetAction( pi_dwActionID );
}

void
CNpc::SetAnimationByAction( DWORD pi_dwActionID )
{
	if( pi_dwActionID == ID_INVALID )
		return;

	DWORD l_dwAniID;
	switch( pi_dwActionID )
	{
		case CAI_MOVE_STOP			:
			if( GetCombatMode() == CSI_PEACE )
				l_dwAniID = NAI_PEACE_STOP;
			else
				l_dwAniID = NAI_COMBAT_STOP;
			break;

		case CAI_MOVE_MOVING		:
			if( GetCombatMode() == CSI_PEACE )
				l_dwAniID = NAI_PEACE_WALK;
			else
				l_dwAniID = NAI_COMBAT_WALK;
			break;

		case CAI_MOVE_IDLE			:
			if( GetCombatMode() == CSI_PEACE )
				l_dwAniID = NAI_PEACE_IDLE;
			else
				l_dwAniID = NAI_COMBAT_IDLE;
			break;

		case CAI_COMBAT_ATTACK		: l_dwAniID = NAI_COMBAT_ATTACK;	break;
		case CAI_DAMAGED_CRITICAL	:
			if( GetCombatMode() == CSI_PEACE )
				l_dwAniID = NAI_PEACE_CRITICAL;
			else
				l_dwAniID = NAI_COMBAT_CRITICAL;
			break;

		case CAI_DEATH_READY		: l_dwAniID = NAI_DEATH_READY;		break;
		case CAI_DEATH_DIE			: l_dwAniID = NAI_DEATH_DIE;		break;
		default						: return;
	}

	CAniData * l_pAniData = GetAnimationData( l_dwAniID );
	m_Animation.SetAniData( l_pAniData );

	if( l_pAniData )
	{
		if( pi_dwActionID == CAI_DEATH_DIE )
			SetAnimationInterpolation( l_pAniData, TRUE );
		else
			SetAnimationInterpolation( l_pAniData, FALSE );
	}
}

// 플레이어의 액션 ID 를 받아서 애니메이션 데이터를 가져온다.
ChAnimation *
CNpc::GetAnimationData( DWORD pi_dwActionID )
{
	if( pi_dwActionID == ID_INVALID )
		return NULL;

	CAniData *	l_pAni = NULL;
	DWORD		l_dwAniID = 0;

	// 현재의 NPC 애니메이션 ID는 메시(혹은 본)ID + 순번 + 액션 ID + 개수 + 0 ( ~ F ) 이다.
	// NPC 는 Mesh ID 의 제일 마지막 F에 갯수가 붙기 때문에 몬스터와는 틀리게 처리해야 한다.
	l_dwAniID |= ( ( GetMeshID() & 0x000FFFFF ) << 8 );
	l_dwAniID |= ( pi_dwActionID << 4 );
	l_dwAniID &= 0xFFFFFFF0;

	// 애니메이션을 랜덤하게 가져온다.
	return GetAnimationDataByRandom( RLI_NPC, l_dwAniID );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNpc::UI_OpenIntroduceWindow( void )
{
	if( !m_pData )
		return;

	// 아바타와 종족이 틀리다면 return
	if( !_GetAvatar()->IsEqualRace( m_pData->byRace * 2 ) )
	{
		_SetSystemMsg( "다른 종족의 상인과는 거래가 불가능합니다." );
		return;
	}

	// 아이템 제작, 교환, 거래창 등이 열려있을 경우에는 return
	if( _GetItemWorkWindow()->IsShow() || _GetItemUpgradeWindow()->IsShow() ||
		_GetTradeWindow()->IsShow() )
		return;

	if( !_GetNpcIntroWindow()->IsShow() )
	{
		BYTE byTotalButtonNum = 0;
		BYTE byButtonTypeList[MAX_NPC_BUTTON_NUM];		
		for( int i = 0; i < MAX_NPC_BUTTON_NUM; ++i )
		{
			if( m_pData->pButtonFunction[i] )
				++byTotalButtonNum;
								
			byButtonTypeList[i] = m_pData->pButtonFunction[i];
		}			
		_GetNpcIntroWindow()->SetButtonType( byTotalButtonNum, byButtonTypeList );
		

		_GetNpcIntroWindow()->SetTitle( m_pName );
		_GetNpcIntroWindow()->SetDescription( m_pData->pDescription );

		_GetNpcIntroWindow()->Open();

		_GetAvatar()->GetNpcTradeInfo()->m_dwCurrentIntroduceNpc = m_dwIndex;		
	}
}

BOOL
CNpc::OpenShopWindow( BYTE pi_byButtonOrder )
{
	if( !m_pData )
		return FALSE;

	switch( m_pData->pButtonFunction[pi_byButtonOrder] )
	{
		case NBFT_TRADE :				// 거래			
			_GetShopTradeBoard()->OpenShopWindow( m_dwIndex );			
			break;

		case NBFT_QUEST :				// 퀘스트
			break;

		case NMBT_ECONOMY :				// 경제지표 확인
			break;
		
		case NMBT_EXCHANGE :			// 환전
			_GetExchangeBoard()->OpenWindow();			
			break;

		case NMBT_PROCESS :				// 가공			
			_GetOreProcBoard()->OpenWindow();
			break;

		case NMBT_WAREHOUSE :			// 아이템/돈 창고 기능
			break;

		case NMBT_INFO :				// 안내
			break;

		case NMBT_ENTER :				// 입장
			break;

		case NMBT_VIEW_KILL_POINT :		// 킬포인트
			break;

		case NMBT_UNIT_TUNING :			// 기갑 장비 튜닝
			// 거래하는 상인을 세팅해야, 튜닝 가격이 올바로 나온다.
			_GetShopTradeBoard()->SetMerchant( this );
			_GetUnitTuningBoard()->OpenWindow();
			break;

		case NMBT_UNIT_STOREHOUSE :		// 기갑 장비 보관
			_GetUnitStorehouseBoard()->OpenWindow();
			break;

		case NMBT_CLASS_CHANGE :		// 전직
			_GetClassChangeBoard()->OpenWindow();
			break;

		default :
			return FALSE;
	}	

	NPC_TRADE_INFO * pNpcTradeInfo = _GetAvatar()->GetNpcTradeInfo();
	if( !pNpcTradeInfo )
		return FALSE;

	pNpcTradeInfo->m_dwPrevTradeCharType	= pNpcTradeInfo->m_dwTradeCharType;
	pNpcTradeInfo->m_dwPrevTradeCharIndex	= pNpcTradeInfo->m_dwTradeCharIndex;
	pNpcTradeInfo->m_dwPrevTradeButtonType	= pNpcTradeInfo->m_dwTradeButtonType;

	pNpcTradeInfo->m_dwTradeCharType		= m_dwCharTypeID;
	pNpcTradeInfo->m_dwTradeCharIndex		= m_dwIndex;
	pNpcTradeInfo->m_dwTradeButtonType		= m_pData->pButtonFunction[pi_byButtonOrder];

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNpc::SetBone( DWORD pi_dwBoneID )
{
	m_pBone = _GetBoneMgr()->GetBone( RLI_NPC, pi_dwBoneID );
	m_pBBox = _GetBBoxMgr()->GetBoundBox( RLI_NPC, pi_dwBoneID );
}

void
CNpc::SetMesh( DWORD pi_dwIndex )
{
	m_pMesh = _GetMeshMgr()->GetMesh( RLI_NPC, pi_dwIndex );
}

DWORD
CNpc::GetMeshID( void )
{
	if( m_pMesh )
		return m_pMesh->m_ID;

	return ID_INVALID;
}

//------------------------------------------------------------------------------

void
CNpc::RenderInfo( void )
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNpc::Enable( DWORD pi_dwFlag )
{
	CCharacter::Enable( pi_dwFlag );

	if( IsEnable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) && _GetRadarWindow() )
		_GetRadarWindow()->RemoveCharacter( m_dwCharTypeID, m_dwIndex );
}

void
CNpc::Disable( DWORD pi_dwFlag )
{
	CCharacter::Disable( pi_dwFlag );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CNpc::ProcessTimeOverFromReceiveFixPositionMsg( void )
{
	return FALSE;
}

BOOL
CNpc::IsPassedLimitTimeFromReceiveNetworkMsg( void )
{
	return FALSE;
}

BOOL
CNpc::IsPassedLimitTimeForDelete( void )
{
	return FALSE;
}

BOOL
CNpc::IsReceiveFixPositionMsg( void )
{
	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CNpc::UI_SetRadar( void )
{
	if( !_GetRadarWindow() || ( m_dwIndex == ID_INVALID ) )
		return;
	if( IsDeath() )
		return;

	CGUIRadarWindow::CharType l_byType = CGUIRadarWindow::NPC_Bel;

	_GetRadarWindow()->AddCharacter( m_dwCharTypeID, m_dwIndex, l_byType, m_vecPos[0], m_vecPos[1], m_vecPos[2] );
}
