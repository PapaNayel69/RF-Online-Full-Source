/////////////////////////////////////////////////////////////////////////////
//
// CRenderUnit Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "CPlayer.h"
#include "CUnit.h"
#include "../../AddCodeByJang/PlayerSpecialEffect.h"
#include "../../UserInterface/GameWindow/GUIHotKeyBoard.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CUnitProc::CUnitProc()
{
	m_pUnitItem		= NULL;
	m_pAttackParts	= NULL;
}

CUnitProc::~CUnitProc()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CUnitProc::Render( void )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		if( !m_pPlayer->SetVertexBlending( i, m_listEquipItem[i].GetMesh(), FALSE ) )
			continue;

		m_listEquipItem[i].SetPosition( m_pPlayer->m_vecPos );
		m_listEquipItem[i].SetRotY( m_pPlayer->m_vecRot[1] );

		m_listEquipItem[i].Render();
	}
}

// J.U.I < begin > -------------------------------------------------------------
void
CUnitProc::ProcessEffectAfterRender( void )
{
}
// J.U.I < end > ---------------------------------------------------------------

// MAYUN < begin > -------------------------------------------------------------
void
CUnitProc::CreateShadow( void )
{
	CMesh * pMesh = NULL;

	for( int i = 0; i < MAX_EQUIPMENT; i++ )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		pMesh = m_listEquipItem[i].GetMesh();
		m_pPlayer->SetVertexBlending( i, pMesh, FALSE );
		_GetCharIF()->DrawCharacterShadow( pMesh, m_pPlayer->m_vecPos, m_pPlayer->m_vecRot[1],
										   0.8f * m_pPlayer->m_fAlphaDensity, 1.0f );
	}
}
// MAYUN < end > ---------------------------------------------------------------

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CUnitProc::Animation( void )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		m_listEquipItem[i].FrameMove();
		m_listEquipItem[i].Animation();

		m_pPlayer->SetVertexBlending( i, m_listEquipItem[i].GetMesh(), TRUE );
	}
	CUnitSpecialEffectFrameMove(this);	//J.U.I
}

// MAYUN < begin > -------------------------------------------------------------
void
CUnitProc::ProcessEffectBeforeAnimation( void )
{
}
// MAYUN < end > ---------------------------------------------------------------

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CUnitProc::FrameMove( void )
{
	if( m_pPlayer->IsDisable( ROSF_MOVABLE ) )
		return;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CUnitProc::SetEffectInfo( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	switch( pi_dwActionID >> 28 )
	{
		case CAI_COMBAT		: SetEffectInfo_Combat( po_stEffectInfo, pi_dwActionID ); break;
		case CAI_DAMAGED	: SetEffectInfo_Damaged( po_stEffectInfo, pi_dwActionID ); break;
		case CAI_ITEM		: SetEffectInfo_Item( po_stEffectInfo, pi_dwActionID ); break;
		case CAI_CREATE		: SetEffectInfo_Create( po_stEffectInfo, pi_dwActionID ); break;
		case CAI_DEATH		: SetEffectInfo_Death( po_stEffectInfo, pi_dwActionID ); break;
		case CAI_GESTURE	: SetEffectInfo_Gesture( po_stEffectInfo, pi_dwActionID ); break;
	}
}

void
CUnitProc::SetEffectInfo_Combat( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType = ETI_UNIT;

	if( pi_dwActionID == CAI_COMBAT_ATTACK )
		po_stEffectInfo.dwID = GetUnitEffectData( pi_dwActionID, UET_ATTACK );

	po_stEffectInfo.SetTarget( m_pPlayer->m_pCombatState->GetCurEnemy( ET_VICTIM ) );
}

void
CUnitProc::SetEffectInfo_Damaged( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType = ETI_UNIT;

	if( pi_dwActionID == CAI_DAMAGED_NORMAL )
		po_stEffectInfo.dwID	= GetUnitEffectData( pi_dwActionID, UET_DAMAGE );
	else if( pi_dwActionID == CAI_DAMAGED_CRITICAL )
		po_stEffectInfo.dwID	= GetUnitEffectData( pi_dwActionID, UET_CRITICAL );
	else if( pi_dwActionID == CAI_DAMAGED_MISS )
		po_stEffectInfo.dwID	= GetUnitEffectData( pi_dwActionID, UET_DAMAGE_MISS );

	CAttackerInfo * pAttackerInfo = m_pPlayer->GetCurAttackerInfo();
	if( !pAttackerInfo )
		return;
	po_stEffectInfo.dwValue = pAttackerInfo->GetCurDamagePoint();
	po_stEffectInfo.SetTarget( pAttackerInfo->GetEnemy() );		// 타겟에 공격자를 넣어서 보낸다.
}

void
CUnitProc::SetEffectInfo_Item( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType = ETI_UNIT;
}

void
CUnitProc::SetEffectInfo_Create( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType = ETI_UNIT;

	if( pi_dwActionID == CAI_CREATE_CREATE )
		po_stEffectInfo.dwID	= GetUnitEffectData( pi_dwActionID, UET_CREATE );
}

void
CUnitProc::SetEffectInfo_Death( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType = ETI_UNIT;

	if( pi_dwActionID == CAI_DEATH_READY )
		po_stEffectInfo.dwID	= GetUnitEffectData( pi_dwActionID, UET_DESTROY );
}

void
CUnitProc::SetEffectInfo_Gesture( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType = ETI_UNIT;

	if( pi_dwActionID == CAI_GESTURE_MOUNT )
		po_stEffectInfo.dwID = GetUnitEffectData( pi_dwActionID, UET_GESTURE_MOUNT );
	else if( pi_dwActionID == CAI_GESTURE_DISMOUNT )
		po_stEffectInfo.dwID = GetUnitEffectData( pi_dwActionID, UET_GESTURE_DISMOUNT );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD
CUnitProc::GetUnitEffectData( DWORD pi_dwActionID, DWORD pi_dwEffectTypeID )
{
	DWORD dwEffectID = 0;

	if( pi_dwActionID == CAI_COMBAT_ATTACK )
	{
		if( !m_pAttackParts )
			return ID_INVALID;

		dwEffectID |= m_pAttackParts->GetMeshID() << 4;
	}
	else
	{
		dwEffectID |= m_pUnitItem->GetDTIndex() << 20;
	}

	dwEffectID |= pi_dwEffectTypeID;

	return dwEffectID;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CUnitProc::ProcessActionID( DWORD pi_dwActionID )
{
	switch( pi_dwActionID )
	{
		case CAI_COMBAT_ATTACK		: SetEquipItemAnimation( IEPT_UNIT_ARMS, IAI_ATTACK ); break;
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CUnitProc::ProcessEffectAfterSetAction( void )
{
	CUnitSpecialEffectSecAction( this );		// by J.U.I
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CAniData *
CUnitProc::GetAnimationByAction( DWORD pi_dwActionID )
{
	if( pi_dwActionID == ID_INVALID )
		return FALSE;

	DWORD dwAniTypeID = ID_INVALID;

	switch( pi_dwActionID )
	{
		case CAI_MOVE_STOP			: dwAniTypeID = PAI_STOP; break;
		case CAI_MOVE_MOVING		: dwAniTypeID = ( m_pPlayer->m_byWalkMode == CMM_MOVE_WALK ) ? PAI_WALK : PAI_RUN; break;
		case CAI_MOVE_IDLE			: dwAniTypeID = PAI_IDLE; break;
		case CAI_COMBAT_ATTACK		: dwAniTypeID = PAI_ATTACK; break;
		case CAI_CREATE_CREATE		: dwAniTypeID = PAI_CREATE_CREATE; break;
		case CAI_DAMAGED_CRITICAL	: dwAniTypeID = PAI_DAMAGE_CRITICAL; break;
		case CAI_DEATH_READY		: dwAniTypeID = PAI_DEATH_READY; break;
		case CAI_DEATH_DIE			: dwAniTypeID = PAI_DEATH_DIE; break;
		case CAI_GESTURE_STANDBY	: dwAniTypeID = PAI_UNIT_STANDBY; break;
		case CAI_GESTURE_MOUNT		: dwAniTypeID = PAI_UNIT_MOUNT; break;
		case CAI_GESTURE_DISMOUNT	: dwAniTypeID = PAI_UNIT_DISMOUNT; break;
	}

	return GetAnimationData( pi_dwActionID, dwAniTypeID );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CAniData *
CUnitProc::GetAnimationData( DWORD pi_dwActionID, DWORD pi_dwAniTypeID )
{
	static const BYTE COMBAT_MODE[2][2] = { { 0x2, 0x0 }, { 0x2, 0x1 } };

	CAniData *	pAnimationData = NULL;
	DWORD		dwAnimationID = 0;
	CEquipItem*	pUnitFrame = GetEquipItem( IEPT_UNIT );
	if( !pUnitFrame )
		return NULL;

	dwAnimationID = pUnitFrame->GetDTIndex() << 24;

	for( int i = 0; i < 2; ++i )
	{
		dwAnimationID	&= 0xF0000000;

		dwAnimationID	|= COMBAT_MODE[m_pPlayer->m_byCombatMode][i] << 20;
		dwAnimationID	|= pi_dwAniTypeID << 12;

		if( pi_dwActionID == CAI_COMBAT_ATTACK )
		{
			if( !m_pAttackParts )
				return NULL;

			if( m_pAttackParts->IsEqualItemKind( IEPT_UNIT_ARMS ) )
				dwAnimationID	|= ( IEPT_UNIT_ARMS - IEPT_UNIT_ARMS ) << 8;
			else
				dwAnimationID	|= ( IEPT_UNIT_SHOULDER - IEPT_UNIT_ARMS ) << 8;

			dwAnimationID	|= m_pAttackParts->GetDTIndex() << 4;
		}
		else
		{
			dwAnimationID	|= 0xFF << 4;
		}

		pAnimationData = m_pPlayer->GetAnimationDataByRandom( RLI_UNIT, dwAnimationID );
		if( pAnimationData )
			return pAnimationData;
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CUnitProc::SetEquipItemAnimation( BYTE pi_byPartType, BYTE pi_byAniTypeID )
{
	CMeshItem * pMeshItem = GetEquipItem( pi_byPartType );
	if( pMeshItem )
		pMeshItem->SetAnimation( pi_byAniTypeID );
}

void
CUnitProc::SetBone( DWORD pi_dwBoneID )
{
	m_pBone = _GetBoneMgr()->GetBone( RLI_UNIT, pi_dwBoneID );

	m_pBBox = _GetBBoxMgr()->GetBoundBox( RLI_UNIT, pi_dwBoneID );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CUnitProc::PutOnEquipment( BYTE pi_byPartType, DWORD pi_dwDTIndex )
{
	return CreateEquipment( pi_byPartType, pi_dwDTIndex );
}

BOOL
CUnitProc::PutOnEquipment( CBoardItem * pi_pBoardItem )
{
	if( !pi_pBoardItem )
		return FALSE;

	if( !CreateEquipment( pi_pBoardItem->GetItemKind(), pi_pBoardItem->GetDTIndex() ) )
		return FALSE;

	// set property
	BYTE byOrederNo = ConvertPartTypeToOrderNo( pi_pBoardItem->GetItemKind() );
	m_listEquipItem[byOrederNo].SetIndex( pi_pBoardItem->GetIndex() );
	m_listEquipItem[byOrederNo].SetServerIndex( pi_pBoardItem->GetServerIndex() );

	return TRUE;
}

BOOL
CUnitProc::PutOnEquipment( CEquipItem * pi_pEquipItem )
{
	if( !pi_pEquipItem )
		return FALSE;

	// create item
	if( !CreateEquipment( pi_pEquipItem->GetItemKind(), pi_pEquipItem->GetDTIndex() ) )
		return FALSE;

	// set property
	BYTE byOrederNo = ConvertPartTypeToOrderNo( pi_pEquipItem->GetItemKind() );
	m_listEquipItem[byOrederNo].SetIndex( pi_pEquipItem->GetIndex() );
	m_listEquipItem[byOrederNo].SetServerIndex( pi_pEquipItem->GetServerIndex() );

	return TRUE;
}

BOOL
CUnitProc::PutOnEquipment( CUnitPartsItem * pi_pUnitPartsItem )
{
	if( !pi_pUnitPartsItem )
		return FALSE;

	// create item
	if( !CreateEquipment( pi_pUnitPartsItem->GetItemKind(), pi_pUnitPartsItem->GetDTIndex() ) )
		return FALSE;

	// set property
	BYTE byOrederNo = ConvertPartTypeToOrderNo( pi_pUnitPartsItem->GetItemKind() );
	m_listEquipItem[byOrederNo].SetIndex( pi_pUnitPartsItem->GetIndex() );
	m_listEquipItem[byOrederNo].SetServerIndex( pi_pUnitPartsItem->GetServerIndex() );

	return TRUE;
}

BOOL
CUnitProc::PutOnEquipment( CUnitItem * pi_pUnitItem )
{
	if( !pi_pUnitItem )
		return FALSE;

	PutOnEquipment( dynamic_cast< CUnitPartsItem * >( pi_pUnitItem ) );

	for( int i = 0; i < MAX_UNIT_PART_TYPE; ++i )
		PutOnEquipment( &pi_pUnitItem->m_listPart[i] );

	// 아바타일 경우에는 CUnitStorehouseBoard()->GetCurUnit() 을 넘겨주므로 
	// pi_pUnitItem 을 그대로 사용하고, 일반 플레이어는 m_clUnitItem에 저장해두어 사용한다.
	if( m_pPlayer->IsEqualCharType( CTI_AVATAR ) )
	{
		m_pUnitItem		= pi_pUnitItem;
	}
	else
	{
		memcpy( &m_clUnitItem, pi_pUnitItem, sizeof( CUnitItem ) );
		m_pUnitItem = &m_clUnitItem;
	}

	m_pUnitItem->SetAttackParts( IEPT_UNIT_ARMS );
	m_pAttackParts	= GetEquipItem( IEPT_UNIT_ARMS );

	return TRUE;
}

BOOL
CUnitProc::PutOffEquipment( BYTE pi_byPartType )
{
	BYTE byOrederNo = ConvertPartTypeToOrderNo( pi_byPartType );
	if( byOrederNo == 0xFF )
		return FALSE;

	// delete item
	m_listEquipItem[pi_byPartType].SetEmpty();
	m_listEquipItem[pi_byPartType].SetLinkItem( NULL );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 유닛은 CNormalItem 대신 CUnitItem 으로 대처하므로 CNormalItem 생성 부분이 빠짐.
BOOL
CUnitProc::CreateEquipment( BYTE pi_byPartType, DWORD pi_dwDTIndex )
{
	BYTE byOrederNo = ConvertPartTypeToOrderNo( pi_byPartType );
	if( byOrederNo == 0xFF )
		return FALSE;

	// is empty?
	if( !m_listEquipItem[byOrederNo].IsEmpty() )
		return FALSE;

	// create item ( pi_dwDTIndex의 최상위 바이트를 0xE로 세팅해야 함. )
	if( !m_listEquipItem[byOrederNo].Create( pi_byPartType, pi_dwDTIndex | ( 0xE << 28 ) ) )
	{
		m_listEquipItem[byOrederNo].SetEmpty();
		return FALSE;
	}
	m_listEquipItem[byOrederNo].SetAnimation( IAI_EQUIP );

	// 유닛 프레임이라면 bone을 세팅한다.
	if( byOrederNo == 0 )
		SetBone( pi_dwDTIndex );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CEquipItem *
CUnitProc::GetEquipItem( BYTE pi_byPartType )
{
	BYTE byOrederNo = ConvertPartTypeToOrderNo( pi_byPartType );
	if( byOrederNo == 0xFF )
		return NULL;

	if( m_listEquipItem[byOrederNo].IsEmpty() )
		return NULL;

	return &m_listEquipItem[byOrederNo];
}

CNormalItem *
CUnitProc::GetNormalItem( BYTE pi_byPartType )
{
	return NULL;
}

CUnitPartsItem *
CUnitProc::GetBooster( void )
{
	if( !m_pUnitItem )
		return NULL;

	CUnitPartsItem * pBackpack = m_pUnitItem->GetParts( IEPT_UNIT_BACKPACK );
	if( !pBackpack )
		return NULL;

	// 부스터인지 체크 ( 부스터는 GetOrgMaxDurability( 가 최대 부스터량임 )
	if( pBackpack->GetOrgMaxDurability() > 0 )
		return pBackpack;

	return NULL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BYTE
CUnitProc::ConvertPartTypeToOrderNo( BYTE pi_byPartType )
{
	if( pi_byPartType == IEPT_UNIT )
		return 0;
	else if( ( pi_byPartType >= IEPT_UNIT_HELMET ) && ( pi_byPartType <= IEPT_UNIT_BACKPACK ) )
		return pi_byPartType - IEPT_UNIT_HELMET + 1;

	return ID_INVALID;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CUnitProc::SetMaxHP( DWORD pi_dwMaxHP )
{
}

DWORD
CUnitProc::GetMaxHP( void )
{
	if( m_pUnitItem )
		return m_pUnitItem->GetOrgMaxDurability();
	else
		return 0;
}

void
CUnitProc::SetHP( DWORD pi_dwHP )
{
	if( m_pUnitItem )
		m_pUnitItem->SetDurability( pi_dwHP );
}

DWORD
CUnitProc::GetHP( void )
{
	if( m_pUnitItem )
		return m_pUnitItem->GetDurability();
	else
		return 0;
}

float
CUnitProc::GetHPRate( void )
{
	return ( (float)GetHP() / (float)GetMaxHP() );
}

//==============================================================================

void
CUnitProc::SetMaxSP( DWORD pi_dwMaxSP )
{
}

DWORD
CUnitProc::GetMaxSP( void )
{
	CUnitPartsItem * pBooster = GetBooster();
	if( pBooster )
		return pBooster->GetOrgMaxDurability();

	return 0;
}

void
CUnitProc::SetSP( DWORD pi_dwSP )
{
	CUnitPartsItem * pBooster = GetBooster();
	if( pBooster )
		pBooster->SetDurability( pi_dwSP );
}

DWORD
CUnitProc::GetSP( void )
{
	CUnitPartsItem * pBooster = GetBooster();
	if( pBooster )
		return pBooster->GetDurability();

	return 0;
}

//==============================================================================

void
CUnitProc::MountUnit( void )
{
	SetAttackParts( IEPT_UNIT_ARMS );
}

void
CUnitProc::SetAttackParts( BYTE pi_byItemKind )
{
	// Set Item
	if( !m_pUnitItem )
		return;

	m_pUnitItem->SetAttackParts( pi_byItemKind );

	// Set Mesh
	BYTE byOrederNo = ConvertPartTypeToOrderNo( pi_byItemKind );
	if( byOrederNo != 0xFF )
		m_pAttackParts = &m_listEquipItem[byOrederNo];

	// UI
	if( m_pPlayer->IsEqualCharType( CTI_AVATAR ) )
	{
		_GetFuncKeyWindow()->SelectUnitWeapon( ( CGUIUnitControlBoard::WeaponType )
											   ( pi_byItemKind - IEPT_UNIT_ARMS ) );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CUnitProc::ChangeMoveMode( BYTE pi_byMoveMode )
{
	// 뛰기 모드로 부스터를 장착했는지 체크해야 한다.
	if( pi_byMoveMode == CMM_MOVE_RUN )
	{
		CUnitPartsItem * pBooster = m_pUnitItem->GetParts( IEPT_UNIT_BACKPACK );
		if( !pBooster )
		{
			_SetSystemMsg( "부스터를 장착하지 않았거나 모두 사용하였습니다." );
			return;
		}

		if( pBooster->GetDurability() > 0 )
			_GetNetworkMgr()->PositionMsg_MoveTypeChangeRequest_clzo( pi_byMoveMode );
	}
	else
	{
		_GetNetworkMgr()->PositionMsg_MoveTypeChangeRequest_clzo( pi_byMoveMode );
	}
}

float
CUnitProc::GetAttackRange( BYTE pi_byAttackType )
{
	if( !m_pAttackParts )
		return FALSE;

	return m_pAttackParts->GetAttackRange();
}

float
CUnitProc::GetBoosterSpeed( void )
{
	CUnitPartsItem * pBooster = m_pUnitItem->GetParts( IEPT_UNIT_BACKPACK );
	if( pBooster )
		return pBooster->GetBoosterSpeed();

	return 0.0f;
}

void
CUnitProc::SetBulletNumThatCurAttackParts( DWORD pi_dwBulletNum )
{
	if( m_pUnitItem )
		m_pUnitItem->SetBulletNumThatCurAttackParts( pi_dwBulletNum );
}

DWORD
CUnitProc::GetBulletNumThatCurAttackParts( void )
{
	if( m_pUnitItem )
		return m_pUnitItem->GetBulletNumThatCurAttackParts();

	return 0;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CUnitProc::SetLightColor( DWORD pi_dwColor, BYTE pi_bFlag )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		m_listEquipItem[i].SetLightColor( pi_dwColor, pi_bFlag );
	}
}

void
CUnitProc::SetLightColorFromMap( void )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		m_listEquipItem[i].SetMaterial( &m_pPlayer->m_d3dMaterial );
	}
}

void
CUnitProc::SetAlphaDensity( float pi_fAlpha )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		m_listEquipItem[i].SetAlphaDensity( pi_fAlpha );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CUnitProc::Enable( DWORD pi_dwFlag )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		m_listEquipItem[i].Enable( pi_dwFlag );
	}
}

void
CUnitProc::Disable( DWORD pi_dwFlag )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		m_listEquipItem[i].Disable( pi_dwFlag );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CUnitProc::LoadAnimaitonData( void )
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////