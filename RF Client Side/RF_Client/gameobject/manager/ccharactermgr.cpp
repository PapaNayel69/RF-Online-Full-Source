////////////////////////////////////////////////////////////////////////////////
//
// CCharacterMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "CCharacterMgr.h"
#include "../Manager/CTargetMgr.h"
#include "../Board/Common/CBoardManager.h"

#include "../../CCamera.h"
#include "../../AddCodeByJang/EffectUtil.h"
#include "../../AddCodeByJang/MonsterSpecialEffect.h"
#include "../../AddCodeByYun/ShadowList.h"	// by MAYUN
#include "../../UserInterface/GameWindow/GUISelectedCharWin.h"
#include "../../UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"
#include "../../UserInterface/GameWindow/EtcWin/GUIMiniMapWindow.h"
#include "../../UserInterface/GameWindow/EtcWin/GUIOptionWindow.h"

#include "../Board/General/CPartyBoard.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCharacterMgr::CCharacterMgr()
{
	m_pAvatar	= NULL;
	m_pDummy	= new CDummy[MAX_DUMMY];

	char pDummyName[MAX_NAME_LENGTH];
	for( int i = 0; i < MAX_DUMMY; ++i )
	{
		m_pDummy[i].SetCharTypeID( CTI_DUMMY );
		m_pDummy[i].SetIndex( ID_INVALID );
		m_pDummy[i].SetServerIndex( ID_INVALID );
		sprintf( pDummyName, "DUMMY %d", i );
		m_pDummy[i].SetName( pDummyName );
	}

	//--------------------------------------------------------------------------

	m_nCollisionObjectNum					= 0;

	m_fVisualField							= MAX_VISUAL_FIELD;
	m_bIsEnableAutoVisualFieldAdjustment	= FALSE;
	m_byVisualFieldAdjustmentLevel			= MAX_VISUALFIELD_ADJUSTMENT_LEVEL;

	m_bIsDrawBBox							= FALSE;
	m_bIsRenderShadow						= FALSE;
	m_bIsRenderInfo							= FALSE;
	m_bIsRenderPlayerName					= FALSE;

	m_dwMouseHoverCharID[0]					= ID_INVALID;
	m_dwMouseHoverCharID[1]					= ID_INVALID;
	m_dwSelectedCharID[0]					= ID_INVALID;
	m_dwSelectedCharID[1]					= ID_INVALID;

	m_fAverageFPS							= 0.0f;

	g_pCharacterMgr							= this;
}

CCharacterMgr::~CCharacterMgr()
{
	_DELETE( m_pAvatar );

	g_pCharacterMgr	= NULL;

	Clear();
}

void
CCharacterMgr::Clear( void )
{
	_DELETE_ARRAY( m_pDummy );

	CHAR_LIST_ITER iter;
	for( int i = 0; i < MAX_CHAR_TYPE; ++i )
	{
		for( iter = m_listActive[i].begin(); iter != m_listActive[i].end(); ++iter )
			_DELETE( ( *iter ) );
		m_listActive[i].clear();

		for( iter = m_listCache[i].begin(); iter != m_listCache[i].end(); ++iter )
			_DELETE( ( *iter ) );
		m_listCache[i].clear();
	}

	m_dequeFPSInfo.clear();
}

void
CCharacterMgr::CreateAvatar( void )
{
	if( m_pAvatar )
		delete m_pAvatar;

	m_pAvatar	= new CAvatar;
	g_pAvatar	= m_pAvatar;
}

void
CCharacterMgr::InitCharacter_EveryFrame( void )
{
	if( m_pAvatar )
		m_pAvatar->Init_EveryFrame();

	for( int i = 0; i < MAX_CHAR_TYPE; ++i )
	{
		for( CHAR_LIST_ITER iter = m_listActive[i].begin(); iter != m_listActive[i].end(); ++iter )
			( *iter )->Init_EveryFrame();
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCharacter *
CCharacterMgr::AddCharacter( DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	if( pi_dwCharTypeID >= MAX_CHAR_TYPE )
		return NULL;

	CCharacter * pChar = GetCharacter( pi_dwCharTypeID, pi_dwIndex );
	if( !pChar )
	{
		switch( pi_dwCharTypeID )
		{
			case CTI_PLAYER		: pChar = new CPlayer; break;
			case CTI_MONSTER	: pChar = new CMonster; break;
			case CTI_ANIMUS		: pChar = new CAnimus; break;
			case CTI_GUARD_TOWER: pChar = new CGuardTower; break;
			case CTI_NPC		: pChar = new CNpc; break;
			case CTI_UNIT		: pChar = new CPlayer; break;
			default				: return NULL;
		}

		m_listActive[pi_dwCharTypeID].push_back( pChar );
	}

	return pChar;
}

void
CCharacterMgr::AddCharacter( DWORD pi_dwCharTypeID, CCharacter * pi_pCharacter )
{
	if( ( pi_dwCharTypeID >= MAX_CHAR_TYPE ) || !pi_pCharacter )
		return;

	CCharacter * pExistChar = GetCharacter( pi_dwCharTypeID, pi_pCharacter->GetIndex() );
	if( pExistChar )
	{
		if( !pExistChar->IsEqual( pi_pCharacter ) )
			m_listActive[pi_dwCharTypeID].push_back( pi_pCharacter );
	}
	else
	{
		m_listActive[pi_dwCharTypeID].push_back( pi_pCharacter );
	}
}

CPlayer *
CCharacterMgr::AddPlayer( DWORD pi_dwIndex )
{
	return static_cast< CPlayer * >( AddCharacter( CTI_PLAYER, pi_dwIndex ) );
}

CMonster *
CCharacterMgr::AddMonster( DWORD pi_dwIndex )
{
	return static_cast< CMonster * >( AddCharacter( CTI_MONSTER, pi_dwIndex ) );
}

CNpc *
CCharacterMgr::AddNpc( DWORD pi_dwIndex )
{
	return static_cast< CNpc * >( AddCharacter( CTI_NPC, pi_dwIndex ) );
}

CAnimus *
CCharacterMgr::AddAnimus( DWORD pi_dwIndex )
{
	return static_cast< CAnimus * >( AddCharacter( CTI_ANIMUS, pi_dwIndex ) );
}

CGuardTower *
CCharacterMgr::AddGuardTower( DWORD pi_dwIndex )
{
	return static_cast< CGuardTower * >( AddCharacter( CTI_GUARD_TOWER, pi_dwIndex ) );
}

CPlayer	*
CCharacterMgr::AddUnit( DWORD pi_dwIndex )
{
	return static_cast< CPlayer * >( AddCharacter( CTI_UNIT, pi_dwIndex ) );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCharacterMgr::DeleteCharacter( DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	if( pi_dwCharTypeID >= MAX_CHAR_TYPE )
		return;

	for( CHAR_LIST_ITER iter = m_listActive[pi_dwCharTypeID].begin();
		 iter != m_listActive[pi_dwCharTypeID].end(); ++iter )
	{
		if( ( *iter )->GetIndex() == pi_dwIndex )
		{
			DeleteCharacterFromList( iter );
			break;
		}
	}
}

void
CCharacterMgr::DeleteCharacterFromList( CHAR_LIST_ITER & pio_iter )
{
	DeleteCharacterFromList_UI( pio_iter );
	DeleteCharacterFromList_Data( pio_iter );
}

void
CCharacterMgr::DeleteCharacterFromList_Data( CHAR_LIST_ITER & pio_iter )
{
	DWORD dwCharTypeID = ( *pio_iter )->GetCharTypeID();

	delete ( *pio_iter );
	( *pio_iter ) = NULL;

	pio_iter = ( m_listActive[dwCharTypeID].erase( pio_iter ) );
	--pio_iter;
}

void
CCharacterMgr::DeleteCharacterFromList_UI( CHAR_LIST_ITER pi_iter )
{
	EFUtil_DeleteSF( ( *pi_iter )->GetAppliedSF() );

	if( _GetTargetMgr()->IsEqualTarget( *pi_iter ) )
		m_pAvatar->StopTargetPursuit();

	if( _GetRadarWindow() )
		_GetRadarWindow()->RemoveCharacter( ( *pi_iter )->GetCharTypeID(), ( *pi_iter )->GetIndex() );

	if( ( *pi_iter )->IsEqualCharType( CTI_PLAYER ) )
	{
		if( _GetMinimapWindow() )
			_GetMinimapWindow()->RemoveMiniMapObject( ( *pi_iter )->GetIndex() |
													  ( CGUIMiniMapWindow::PartyMember << 28 ) );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCharacterMgr::ClearCharacter( DWORD pi_dwCharTypeID, BOOL pi_bDeletePartyMember )
{
	if( pi_dwCharTypeID >= MAX_CHAR_TYPE )
		return;

	for( CHAR_LIST_ITER iter = m_listActive[pi_dwCharTypeID].begin();
		 iter != m_listActive[pi_dwCharTypeID].end(); ++iter )
	{
		if( ( *iter )->IsEqualCharType( CTI_PLAYER ) )
		{
			if( _GetPartyBoard()->IsPartyMember( ( *iter )->GetIndex() ) )
				continue;
		}

		SaveToCache( iter );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CCharacterMgr::SaveToCache( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return;

	for( CHAR_LIST_ITER iter = m_listActive[pi_pChar->GetCharTypeID()].begin();
		 iter != m_listActive[pi_pChar->GetCharTypeID()].end(); ++iter )
	{
		if( ( *iter )->GetIndex() == pi_pChar->GetIndex() )
		{
			SaveToCache( iter );
			break;
		}
	}
}

void
CCharacterMgr::SaveToCache( CHAR_LIST_ITER & pio_iter )
{
	CCharacter * pChar = ( *pio_iter );
	if( !pChar )
		return;

	DWORD dwCharType = pChar->GetCharTypeID();

	pChar->Enable( ROSF_OUT_OF_VISUAL_FIELD_SERVER | ROSF_OUT_OF_VISUAL_FIELD_CLIENT );
	m_listCache[dwCharType].push_back( pChar );

	DeleteCharacterFromList_UI( pio_iter );
	pio_iter = m_listActive[dwCharType].erase( pio_iter );
	--pio_iter;
}

CCharacter *
CCharacterMgr::LoadFromCache( DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	if( m_listCache[pi_dwCharTypeID].empty() )
		return NULL;

	CCharacter * pChar = NULL;

	for( CHAR_LIST_ITER iter = m_listCache[pi_dwCharTypeID].begin();
		 iter != m_listCache[pi_dwCharTypeID].end(); ++iter )
	{
		pChar = ( *iter );
		if( pChar->IsEqual( pi_dwCharTypeID, pi_dwIndex ) )
		{
			m_listActive[pi_dwCharTypeID].push_back( pChar );

			iter = m_listCache[pi_dwCharTypeID].erase( iter );
			return pChar;
		}
	}

	return NULL;
}

void
CCharacterMgr::CacheArrangementByTime( void )
{
	static	DWORD dwPrevArrangementTime = 0;
			DWORD dwCurTime = timeGetTime();

	if( dwCurTime - dwPrevArrangementTime <= 60000 )	// ( default 1분에 한 번씩 검사 )
		return;
	dwPrevArrangementTime = dwCurTime;

	CCharacter * pChar = NULL;

	for( int i = 0; i < MAX_CHAR_TYPE; ++i )
	{
		if( m_listCache[i].empty() )
			continue;

		for( CHAR_LIST_ITER iter = m_listCache[i].begin(); iter != m_listCache[i].end(); ++iter )
		{
			if( ( *iter )->IsPassedLimitTimeForDelete() )
			{
				_DELETE( ( *iter ) );

				iter = m_listCache[i].erase( iter );
				--iter;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCharacter *
CCharacterMgr::GetCharacter( DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	if( ( pi_dwCharTypeID == CTI_AVATAR ) || ( pi_dwCharTypeID == CTI_PLAYER ) )
	{
		if( m_pAvatar->IsEqual( CTI_AVATAR, pi_dwIndex ) )
			return dynamic_cast< CCharacter * >( m_pAvatar );
	}

	if( ( pi_dwCharTypeID >= MAX_CHAR_TYPE ) || ( pi_dwIndex == ID_INVALID ) )
		return NULL;

	for( CHAR_LIST_ITER iter = m_listActive[pi_dwCharTypeID].begin();
		 iter != m_listActive[pi_dwCharTypeID].end(); ++iter )
	{
		if( ( *iter )->IsEqual( pi_dwCharTypeID, pi_dwIndex ) )
			return ( *iter );
	}

	return LoadFromCache( pi_dwCharTypeID, pi_dwIndex );
}

CPlayer *
CCharacterMgr::GetPlayer( DWORD pi_dwIndex )
{
	return static_cast< CPlayer * >( GetCharacter( CTI_PLAYER, pi_dwIndex ) );
}

CMonster *
CCharacterMgr::GetMonster( DWORD pi_dwIndex )
{
	return static_cast< CMonster * >( GetCharacter( CTI_MONSTER, pi_dwIndex ) );
}

CNpc *
CCharacterMgr::GetNpc( DWORD pi_dwIndex )
{
	return static_cast< CNpc * >( GetCharacter( CTI_NPC, pi_dwIndex ) );
}

CAnimus *
CCharacterMgr::GetAnimus( DWORD pi_dwIndex )
{
	return static_cast< CAnimus * >( GetCharacter( CTI_ANIMUS, pi_dwIndex ) );
}

CGuardTower *
CCharacterMgr::GetGuardTower( DWORD pi_dwIndex )
{
	return static_cast< CGuardTower * >( GetCharacter( CTI_GUARD_TOWER, pi_dwIndex ) );
}

CPlayer *
CCharacterMgr::GetUnit( DWORD pi_dwIndex )
{
	return static_cast< CPlayer * >( GetCharacter( CTI_UNIT, pi_dwIndex ) );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CCharacter *
CCharacterMgr::GetCharacterByServerIndex( DWORD pi_dwCharTypeID, DWORD pi_dwServerIndex )
{
	if( ( pi_dwCharTypeID >= MAX_CHAR_TYPE ) || ( pi_dwServerIndex == ID_INVALID ) )
		return NULL;

	for( CHAR_LIST_ITER iter = m_listActive[pi_dwCharTypeID].begin();
		 iter != m_listActive[pi_dwCharTypeID].end(); ++iter )
	{
		if( ( *iter )->GetServerIndex() == pi_dwServerIndex )
			return ( *iter );
	}

	return NULL;
}

CCharacter *
CCharacterMgr::GetCharacterByName( DWORD pi_dwCharTypeID, char * pi_pName )
{
	if( ( pi_dwCharTypeID >= MAX_CHAR_TYPE ) || ( !pi_pName ) )
		return NULL;

	for( CHAR_LIST_ITER iter = m_listActive[pi_dwCharTypeID].begin();
		 iter != m_listActive[pi_dwCharTypeID].end(); ++iter )
	{
		if( strcmp( ( *iter )->GetName(), pi_pName ) == 0 )
			return ( *iter );
	}

	return NULL;
}

CCharacter *
CCharacterMgr::GetCharacterByDTIndex( DWORD pi_dwCharTypeID, DWORD pi_dwDTIndex )
{
	if( ( pi_dwCharTypeID >= MAX_CHAR_TYPE ) || ( pi_dwDTIndex == ID_INVALID ) )
		return NULL;

	for( CHAR_LIST_ITER iter = m_listActive[pi_dwCharTypeID].begin();
		 iter != m_listActive[pi_dwCharTypeID].end(); ++iter )
	{
		if( ( *iter )->GetDTIndex() == pi_dwDTIndex )
			return ( *iter );
		 }

	return NULL;
}

CCharacter *
CCharacterMgr::GetCharacterByOrder( DWORD pi_dwCharTypeID, DWORD pi_dwOrderIndex )
{
	if( pi_dwCharTypeID >= MAX_CHAR_TYPE )
		return NULL;

	if( pi_dwOrderIndex >= m_listActive[pi_dwCharTypeID].size() )
		return NULL;

	for( CHAR_LIST_ITER iter = m_listActive[pi_dwCharTypeID].begin();
		 iter != m_listActive[pi_dwCharTypeID].end(); ++iter, --pi_dwOrderIndex )
	{
		if( pi_dwOrderIndex == 0 )
			return ( *iter );
	}

	return NULL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CDummy *
CCharacterMgr::GetDummy( DWORD pi_dwIndex )
{
	DWORD l_dwIndex = pi_dwIndex - INDEX_DUMMY;
	if( l_dwIndex >= MAX_DUMMY )
		return NULL;

	if( m_pDummy[l_dwIndex].GetIndex() != ID_INVALID )
		return &m_pDummy[l_dwIndex];

	return NULL;
}

CDummy *
CCharacterMgr::CreateDummy( void )
{
	for( int i = 0; i < MAX_DUMMY; ++i )
	{
		if( m_pDummy[i].GetIndex() == ID_INVALID )
		{
			m_pDummy[i].SetIndex( INDEX_DUMMY + i );
			m_pDummy[i].SetServerIndex( INDEX_DUMMY + i );

			return &m_pDummy[i];
		}
	}

	return NULL;
}

void
CCharacterMgr::DeleteDummy( DWORD pi_dwIndex )
{
	DWORD dwIndex = pi_dwIndex - INDEX_DUMMY;
	if( dwIndex >= MAX_DUMMY )
		return;

	m_pDummy[dwIndex].SetIndex( ID_INVALID );
	m_pDummy[dwIndex].SetServerIndex( ID_INVALID );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CCharacterMgr::FrameMove( void )
{
	FrameMove_ProcessStealthAndDetect( m_pAvatar );
	m_pAvatar->FrameMove();

	for( int i = 0; i < MAX_CHAR_TYPE; ++i )
	{
		for( CHAR_LIST_ITER iter = m_listActive[i].begin(); iter != m_listActive[i].end(); ++iter )
			FrameMove_Character( i, iter );
	}

	CacheArrangementByTime();

	return TRUE;
}

BOOL
CCharacterMgr::FrameMove_Character( BYTE pi_dwCharTypeID, CHAR_LIST_ITER & pio_iter )
{
	CCharacter * pChar = ( *pio_iter );
	if( !pChar )
		return FALSE;

	if( pChar->IsEnable( ROSF_DESTROY ) )
	{
		DeleteCharacterFromList( pio_iter );
		return TRUE;
	}

	if( pChar->ProcessTimeOverFromReceiveFixPositionMsg() )
	{
		pChar->Enable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );
		if( pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
		{
			pChar->Enable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT );
			DeleteCharacterFromList_UI( pio_iter );

			if( pChar->IsEqualActionCategory( CAI_DEATH ) )
			{
				if( pChar->IsEqualCharType( CTI_PLAYER ) )
				{
					if( !_GetPartyBoard()->IsPartyMember( pChar->GetIndex() ) )
					{
						SaveToCache( pio_iter );
						return TRUE;
					}
				}
				else
				{
					DeleteCharacterFromList_Data( pio_iter );
					return TRUE;
				}
			}
		}
	}

	if( pChar->IsPassedLimitTimeFromReceiveNetworkMsg() )
	{
		if( pChar->IsEqualCharType( CTI_MONSTER ) )
		{
			if( pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) )
				pChar->SetAction( CAI_DEATH_READY );
			DeleteCharacterFromList( pio_iter );
		}
		else if( pChar->IsEqualCharType( CTI_PLAYER ) )
		{
			if( !_GetPartyBoard()->IsPartyMember( pChar->GetIndex() ) )
				SaveToCache( pio_iter );
		}

		return TRUE;
	}

	if( pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
	{
		FrameMove_ProcessStealthAndDetect( pChar );
		pChar->ProcessClipping();
	}

	if( pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) )
	{
		pChar->GetConditionInfo()->ProcessStun();
		pChar->FrameMove();
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCharacterMgr::FrameMove_ProcessStealthAndDetect( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return;

	// 캐릭터가 스텔스 상태일 경우
	BOOL l_bEnableStealth = pi_pChar->GetConditionInfo()->m_bEnable_Stealth;
	if( l_bEnableStealth )
	{
		// 캐릭터가 유닛을 타고 있다면 스텔스 불가능
		if( pi_pChar->IsEqualCharType( CTI_PLAYER ) )
		{
			if( static_cast< CPlayer * >( pi_pChar )->IsEqualUnitMountState( UMS_MOUNT ) )
				l_bEnableStealth = FALSE;
		}

		if( l_bEnableStealth )
		{
			// 디텍트 가능
			if( m_pAvatar->IsEnableDetect( pi_pChar ) )
			{
				pi_pChar->Enable( ROSF_RENDER_MESH | ROSF_RENDER_ALPHA_STEALTH );
				pi_pChar->Disable( ROSF_DO_NOT_PICK );
			}
			// 디텍트 불가능
			else
			{
				pi_pChar->Disable( ROSF_RENDER_MESH | ROSF_RENDER_ALPHA_STEALTH );
				pi_pChar->Enable( ROSF_DO_NOT_PICK );

				if( _GetTargetMgr()->IsEqualTarget( pi_pChar ) )
					_GetTargetMgr()->SetTarget( NULL );
			}
		}
	}

	// 캐릭터가 스텔스 상태가 아닌경우
	if( !l_bEnableStealth )
	{
		pi_pChar->Enable( ROSF_RENDER_MESH );
		pi_pChar->Disable( ROSF_RENDER_ALPHA_STEALTH | ROSF_DO_NOT_PICK );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CCharacterMgr::Animation( void )
{	
	Animation_Avatar();

	for( int i = 0; i < MAX_CHAR_TYPE; ++i )
	{
		for( CHAR_LIST_ITER iter = m_listActive[i].begin();
			 iter != m_listActive[i].end(); ++iter )
			Animation_Character( *iter );
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CCharacterMgr::Animation_Avatar( void )
{
	if( !m_pAvatar )
		return FALSE;

	Animation_Avatar_SetAvatarHead();
	m_pAvatar->Animation();
	Animation_Avatar_UnetAvatarHead();

	Animation_Avatar_MakeShadow();

	Animation_Avatar_SetUI();

	return TRUE;
}

void
CCharacterMgr::Animation_Avatar_SetAvatarHead( void )
{
	// by J.U.I - 마우스에 따른 머리회전...
	D3DXMATRIX EyeRot;
	::GetHeadMatrix( &EyeRot, _GetCamera()->GetR3Camera(),
					 m_pAvatar->GetRotY(), gMouse.x, gMouse.y);
	CHARACTEROBJECT *bn = NULL;
	bn = m_pAvatar->GetBone();

	if( bn )
	{
		if( !m_pAvatar->IsDeath() && m_pAvatar->IsEqualUnitMountState( UMS_DISMOUNT ) )
 			_GetCharIF()->SetEyeVec("Bip01 Head",bn,EyeRot);
		else
		{
			D3DXMatrixIdentity(&EyeRot);
 			_GetCharIF()->SetEyeVec("Bip01 Head",bn,EyeRot);
		}
	}
}

void
CCharacterMgr::Animation_Avatar_UnetAvatarHead( void )
{
	// by J.U.I - 딴넘들은 안돌아가게..
	D3DXMATRIX EyeRot;
	if( m_pAvatar->GetBone() )
	{
		D3DXMatrixIdentity(&EyeRot);
 		_GetCharIF()->SetEyeVec("Bip01 Head",m_pAvatar->GetBone(),EyeRot);
	}
}

void
CCharacterMgr::Animation_Avatar_MakeShadow( void )
{
	// by MAYUN
	if( m_bIsRenderShadow )
	{		
		if(_GetCharIF()->BeginShadow(0))
		{
			_GetCharIF()->SetState();
			m_pAvatar->CreateShadow();
			_GetCharIF()->EndShadow();
			_GetCharIF()->UnSetState();
		}
	}
}

void
CCharacterMgr::Animation_Avatar_SetUI( void )
{
	if( _GetRadarWindow() )
	{
		_GetRadarWindow()->SetAvatarPosition( m_pAvatar->GetPosX(), m_pAvatar->GetPosY(), m_pAvatar->GetPosZ() );
		_GetRadarWindow()->SetCameraRotate( _GetCamera()->GetPitch() );
		_GetRadarWindow()->SetAvatarRotate( m_pAvatar->GetRotY() );
	}

	if( _GetMinimapWindow() )
	{
		//if( _GetLand()->GetMapIndex() == _GetMinimapWindow()->GetMapIndex() )
		{
			_GetMinimapWindow()->SetAvatarPosition( m_pAvatar->GetPosX(), m_pAvatar->GetPosZ() );
			_GetMinimapWindow()->SetAvatarRotate( m_pAvatar->GetRotY() );
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CCharacterMgr::Animation_Character( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return FALSE;

	if( pi_pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) )
	{
		pi_pChar->Animation();

		Animation_Character_ProcessEffect( pi_pChar );
		Animation_Character_ProcessShadow( pi_pChar );
	}

	return TRUE;
}

void
CCharacterMgr::Animation_Character_ProcessEffect( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return;

	if( pi_pChar->IsEqualCharType( CTI_MONSTER ) )	// J.U.I
		::CMonsterDamageBackup( pi_pChar );
}

void
CCharacterMgr::Animation_Character_ProcessShadow( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return;

	// MAYUN
	if( !m_bIsRenderShadow )
		return;
	if( pi_pChar->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
		return;
	if( pi_pChar->IsEnable( ROSF_CLIPPED ) )
		return;

	DWORD n = g_ShadowList.GetShadowNum();
	if ( n<CMAX_SHADOW)
	{
		if (_GetCharIF()->BeginShadow(n+1))
		{
			g_ShadowList.SetShadow( pi_pChar->GetPosition() );
			_GetCharIF()->SetState();	
			
			pi_pChar->CreateShadow();
							
			_GetCharIF()->EndShadow();
			_GetCharIF()->UnSetState();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef _DEVELOPER_
//char g_pBuff[128];
#endif

BOOL
CCharacterMgr::Render( void )
{
	_GetCharIF()->SetState();

	SetCharacterClipping();

	Render_Avatar();

	for( int i = 0; i < MAX_CHAR_TYPE; ++i )
	{
		for( CHAR_LIST_ITER iter = m_listActive[i].begin(); iter != m_listActive[i].end(); ++iter )
			Render_Character( *iter );
	}

	_GetCharIF()->UnSetState();

	//RenderText();

	return TRUE;
}

void
CCharacterMgr::Render_Avatar( void )
{
	if( !m_pAvatar )
		return;

	if( m_pAvatar->IsDisable( ROSF_OBSERVER ) )
		Render_Character( dynamic_cast< CCharacter * >( m_pAvatar ) );
}

void
CCharacterMgr::Render_Character( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return;

	if( pi_pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) &&
		pi_pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
	{
		if( pi_pChar->IsDisable( ROSF_CLIPPED ) )
		{
			( m_bIsDrawBBox )	? pi_pChar->Enable( ROSF_RENDER_BBOX )
								: pi_pChar->Disable( ROSF_RENDER_BBOX );

			float l_fPrevAlphaDensity = pi_pChar->GetAlphaDensity();
			if( pi_pChar->IsEnable( ROSF_RENDER_ALPHA_STEALTH ) )
			{
				pi_pChar->Enable( ROSF_RENDER_ALPHA );
				pi_pChar->SetAlphaDensity( 0.5f );
			}

			pi_pChar->Render();

			/* 쉐도우 렌더 방식을 바꿨다. - by J.U.I
			if( m_bIsRenderShadow )		
				pi_pChar->RenderShadow();*/

			if( pi_pChar->IsEnable( ROSF_RENDER_ALPHA_STEALTH ) )
			{
				pi_pChar->Disable( ROSF_RENDER_ALPHA );
				pi_pChar->SetAlphaDensity( l_fPrevAlphaDensity );
			}
			else	//스텔스가 아닐때 쉐도우 리스트를 일단 만들어놓는다. by - J.U.I
			{
				EFUtil_PushShadow(pi_pChar);	//일단 쉐도우 뿌릴거 리스트를 만든다.함수 안에서 쉐도옵션에따라 작동.
			}
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CCharacterMgr::RenderText( void )
{
	if( m_pAvatar->IsDisable( ROSF_CLIPPED ) )
	{
		m_pAvatar->Enable( ROSF_RENDER_NAME );
		m_pAvatar->RenderText();
	}

	for( int i = 0; i < MAX_CHAR_TYPE; ++i )
	{
		for( CHAR_LIST_ITER iter = m_listActive[i].begin();
			 iter != m_listActive[i].end(); ++iter )
		{
			RenderText_Character( *iter );
		}
	}

#ifdef _DEVELOPER_
	static char l_pBuff[128];
	if( m_bIsRenderInfo )
	{
		sprintf( l_pBuff, "< Active : %d, %d, %d >   < Cache : %d, %d, %d >",
				 m_listActive[0].size(), m_listActive[1].size(), m_listActive[2].size(),
				 m_listCache[0].size(), m_listCache[1].size(), m_listCache[2].size() );
		DrawR3Hangul( 400, 520, l_pBuff, 0xFFFFFFFF, R3_HAN_OUTLINE );
	}
#endif

	return TRUE;
}

void
CCharacterMgr::RenderText_Character( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return;

	if( pi_pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) &&
		pi_pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) &&
		pi_pChar->IsDisable( ROSF_CLIPPED ) )
	{
		if( pi_pChar->IsDisable( ROSF_RENDER_MESH ) )			// 메시가 렌더링 되지 않는다면 이름도 출력하지 않는다.
			pi_pChar->Disable( ROSF_RENDER_NAME );
		else if( pi_pChar->GetCharTypeID() == CTI_NPC )			// NPC는 무조건 이름 출력
			pi_pChar->Enable( ROSF_RENDER_NAME );
		else if( _GetTargetMgr()->IsEqualTarget( pi_pChar ) )	// 타겟은 UI에서 이름이 나오므로 출력을 막는다.
			pi_pChar->Disable( ROSF_RENDER_NAME );
		else if( pi_pChar->GetCharTypeID() == CTI_PLAYER )		// Player는 무조건 이름 출력
			pi_pChar->Enable( ROSF_RENDER_NAME );
		//else if( m_bIsRenderPlayerName )
		//	if( pi_pChar->GetCharTypeID() == CTI_PLAYER )
		//		pi_pChar->Enable( ROSF_RENDER_NAME );
		else if( ( pi_pChar->GetCharTypeID() == m_dwMouseHoverCharID[0] ) &&
				 ( pi_pChar->GetIndex() == m_dwMouseHoverCharID[1] ) )
			pi_pChar->Enable( ROSF_RENDER_NAME );
		else if( ( pi_pChar->GetCharTypeID() == m_dwSelectedCharID[0] ) &&
				 ( pi_pChar->GetIndex() == m_dwSelectedCharID[1] ) )
			pi_pChar->Enable( ROSF_RENDER_NAME );
		else
			pi_pChar->Disable( ROSF_RENDER_NAME );

		( m_bIsRenderInfo ) ? pi_pChar->Enable( ROSF_RENDER_INFO )
							: pi_pChar->Disable( ROSF_RENDER_INFO );

		pi_pChar->RenderText();
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CCharacterMgr::RenderInformation( void )
{
#ifdef _DEVELOPER_

	if( m_pAvatar->IsDisable( ROSF_CLIPPED ) )
	{
		( m_bIsRenderInfo ) ? m_pAvatar->Enable( ROSF_RENDER_INFO )
							: m_pAvatar->Disable( ROSF_RENDER_INFO );
		m_pAvatar->RenderInformation();
	}

	for( int i = 0; i < MAX_CHAR_TYPE; ++i )
	{
		for( CHAR_LIST_ITER iter = m_listActive[i].begin();
			 iter != m_listActive[i].end(); ++iter )
		{
			RenderInformation_Character( *iter );
		}
	}

#endif

	return TRUE;
}

void
CCharacterMgr::RenderInformation_Character( CCharacter * pi_pChar )
{
#ifdef _DEVELOPER_

	if( !pi_pChar )
		return;

	if( pi_pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) &&
		pi_pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) &&
		pi_pChar->IsDisable( ROSF_CLIPPED ) )
	{
		( m_bIsRenderInfo ) ? pi_pChar->Enable( ROSF_RENDER_INFO )
							: pi_pChar->Disable( ROSF_RENDER_INFO );

		pi_pChar->RenderInformation();
	}

#endif
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CObject *
CCharacterMgr::CollisionCheckWithBoundBox( long pi_nPosX, long pi_nPosY,
										   Vector3f & pi_vecCamPos,
										   Vector3f & po_vecCollidedPoint,
										   BOOL pi_bIsPicking )
{
	m_nCollisionObjectNum = 0;
	m_nPosX = pi_nPosX;
	m_nPosY = pi_nPosY;

	CollisionCheckWithBoundBox_Avatar( pi_bIsPicking );

	for( int i = 0; i < MAX_CHAR_TYPE; ++i )
	{
		for( CHAR_LIST_ITER iter = m_listActive[i].begin();
			 iter != m_listActive[i].end(); ++iter )
		{
			CollisionCheckWithBoundBox_Character( *iter );
		}
	}

	return CollisionCheckWithBoundBox_GetNearestCharacter( pi_vecCamPos, po_vecCollidedPoint );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타 체크 - 아바타가 스킬/포스를 선택했을 경우에만 체크한다.
//             - 스킬/포스 타입이 SFT_ANYWHERE, SFT_SAME_RACE 일때만 해당된다.
void
CCharacterMgr::CollisionCheckWithBoundBox_Avatar( BOOL pi_bIsPicking )
{
	if( !m_pAvatar )
		return;

	// 카메라 모드가 FPS 모드일 경우라면 아바타는 아예 피킹이 되지 않는다.
	if( _GetCamera()->GetCameraMode() == 2 )
		return;

	if( pi_bIsPicking )
	{
		if( _GetGBM()->GetSelectSF() )
		{
			CSFItem * pSelectSF = _GetGBM()->GetSelectSF();
			if( !pSelectSF )
				return;

			if( pSelectSF->IsEqualTargetType( SFT_ANYWHERE ) ||
				pSelectSF->IsEqualTargetType( SFT_SAME_RACE ) )
				CollisionCheckWithBoundBox_Character( m_pAvatar );
		}
	}
}

void
CCharacterMgr::CollisionCheckWithBoundBox_Character( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return;

	if( pi_pChar->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) ||
		pi_pChar->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) ||
		pi_pChar->IsEnable( ROSF_CLIPPED ) ||
		pi_pChar->IsEnable( ROSF_DO_NOT_PICK ) ||
		pi_pChar->IsDeath() )
		return;

	BOOL l_bIsCollided = pi_pChar->CollisionCheckWithBoundBox( m_nPosX, m_nPosY,
						 m_vecCollidedPoint[ m_nCollisionObjectNum ] );

	if( l_bIsCollided )
	{
		m_pCollisionObject[ m_nCollisionObjectNum ] = dynamic_cast< CObject * >( pi_pChar );

		pi_pChar->GetPosition( m_vecCollisoinObjectPos[ m_nCollisionObjectNum ] );
		++m_nCollisionObjectNum;
	}
}

CObject *
CCharacterMgr::CollisionCheckWithBoundBox_GetNearestCharacter( Vector3f & pi_vecCamPos,
															   Vector3f & po_vecCollidedPoint )
{
	if( m_nCollisionObjectNum > 0 )
	{
		float	l_fMaxDist, l_fMinDist = 1000000000.0f;
		int		l_nSelectObject = 0;

		for( int i = 0; i < m_nCollisionObjectNum; ++i )
		{
			l_fMaxDist = GetDist( pi_vecCamPos, m_vecCollisoinObjectPos[i] );

			if( l_fMaxDist < l_fMinDist )
			{
				l_fMinDist = l_fMaxDist;
				l_nSelectObject = i;
			}
		}

		po_vecCollidedPoint[0] = m_vecCollidedPoint[l_nSelectObject][0];
		po_vecCollidedPoint[1] = m_vecCollidedPoint[l_nSelectObject][1];
		po_vecCollidedPoint[2] = m_vecCollidedPoint[l_nSelectObject][2];

		return m_pCollisionObject[l_nSelectObject];
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 플레이어 시야를 체크해서 각 캐릭터들을 렌더링 시킬지 말지를 결정한다.
void
CCharacterMgr::CheckOutPositionOfVisualField( void )
{
	VisualFieldAdjustment();

	if( !CheckOutPositionOfVisualField_CheckTime() )
		return;

	if( !m_pAvatar )
		return;

	for( int i = 0; i < MAX_CHAR_TYPE; ++i )
	{
		for( CHAR_LIST_ITER iter = m_listActive[i].begin();
			 iter != m_listActive[i].end(); ++iter )
		{
			int a = m_listActive[i].size();
			CheckOutPositionOfVisualField_Character( *iter, TRUE );
		}
	}
}

BOOL
CCharacterMgr::CheckOutPositionOfVisualField_CheckTime( void )
{
	static	DWORD l_dwPrevTime = 0;
			DWORD l_dwCurTime = timeGetTime();

	if( l_dwCurTime - l_dwPrevTime < 500 )
		return FALSE;

	l_dwPrevTime = l_dwCurTime;

	return TRUE;
}

// pi_bCheckRemovableFlag => IsRemovableWhenWasNotReciveNetworkMsg() 를 호출할 것인지 체크
void
CCharacterMgr::CheckOutPositionOfVisualField_Character( CCharacter * pi_pChar, BOOL pi_bCheckRemovableFlag )
{
	if( !pi_pChar )
		return;

	// 현재 캐릭터가 어떠한 행동을 하는 중이어서 화면상에서 지울수가 없다면 return
	if( pi_bCheckRemovableFlag &&
		!pi_pChar->IsRemovableWhenWasNotReciveNetworkMsg() )
		return;

	// 영역 체크
	SetOutsideCharOfServerRegion( pi_pChar );
	SetOutsideCharOfClientRegion( pi_pChar );

	// 아바타의 타겟 세팅
	if( pi_pChar->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) ||
		pi_pChar->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
	{
		if( _GetTargetMgr()->IsEqualTarget( pi_pChar ) )
			m_pAvatar->StopTargetPursuit();
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 캐릭터가 아바타를 중심으로 주어진 영역 안에 있는지를 검사한다.
BOOL
CCharacterMgr::IsLocatedInSectorOfAvatar( CCharacter * pi_pChar, float pi_fDistance )
{
	if( !pi_pChar )
		return FALSE;

	if( (
		( ( pi_pChar->GetPosX() < _GetAvatar()->GetPosX() + pi_fDistance ) &&
		( pi_pChar->GetPosX() > _GetAvatar()->GetPosX() - pi_fDistance ) ) &&
		( ( pi_pChar->GetPosZ() < _GetAvatar()->GetPosZ() + pi_fDistance ) &&
		( pi_pChar->GetPosZ() > _GetAvatar()->GetPosZ() - pi_fDistance ) )
		) )
		return TRUE;

	return FALSE;
}

void
CCharacterMgr::SetOutsideCharOfServerRegion( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return;

	float fVisualField_Server = m_fVisualField;

	// NPC는 시야의 10배까지 처리
	if( pi_pChar->IsEqualCharType( CTI_NPC ) )
		fVisualField_Server = m_fVisualField * 10;

	// 서버 경계선 바깥쪽
	if( !IsLocatedInSectorOfAvatar( pi_pChar, fVisualField_Server ) )
	{
		pi_pChar->Enable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );
		pi_pChar->Enable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT );
	}
	// 서버 경계선 안쪽
	else
	{
		if( pi_pChar->IsReceiveFixPositionMsg() )
			pi_pChar->Disable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );
	}
}

void
CCharacterMgr::SetOutsideCharOfClientRegion( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return;

	const	float fClientMargin = 0.0f;// 20.0f;
			float fVisualField_Client = m_fVisualField - fClientMargin;

	// 스킬/포스에 의해 가감되는 시야거리
	fVisualField_Client = ( m_fVisualField - fClientMargin ) * _GetAvatar()->GetConditionInfo()->m_fVisualField;

	// NPC는 시야의 2배까지 처리
	if( pi_pChar->IsEqualCharType( CTI_NPC ) )
		fVisualField_Client = ( m_fVisualField - fClientMargin ) * 2;

	// 클라이언트 경계선 바깥쪽
	if( !IsLocatedInSectorOfAvatar( pi_pChar, fVisualField_Client ) )
	{
		if( !_GetTargetMgr()->IsEqualTarget( pi_pChar ) )		// 아바타의 타겟은 제외
			pi_pChar->Enable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT );
	}
	// 클라이언트 경계선 안쪽
	else
	{
		if( pi_pChar->IsReceiveFixPositionMsg() )
			pi_pChar->Disable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT );

		if( pi_pChar->IsEqualCharType( CTI_NPC ) )
			pi_pChar->UI_SetRadar();
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 주어진 캐릭터와 가장 가까운 캐릭터를 찾는다.
// CCharacter * pi_pCenterChar		- 중심이 되는 캐릭터
// float		pi_fLimitDist		- 캐릭터를 찾을 제한 거리
// float		pi_fLimitYAngleDist	- 캐릭터를 찾을 제한 Y 각도 차이
CCharacter *
CCharacterMgr::FindNearestCharacter( CCharacter * pi_pCenterChar,
									 float pi_fLimitDist, float pi_fLimitYAngleDist )
{
	if( !pi_pCenterChar )
		return NULL;

	CCharacter * l_pNearestChar = NULL;
	for( int i = 0; i <= CTI_MONSTER; ++i )
	{
		for( CHAR_LIST_ITER iter = m_listActive[i].begin();
			 iter != m_listActive[i].end(); ++iter )
		{
			if( FindNearestCharacter_Character( pi_pCenterChar, *iter, pi_fLimitDist, pi_fLimitYAngleDist ) )
				l_pNearestChar = ( *iter );
		}
	}

	return l_pNearestChar;
}

CCharacter *
CCharacterMgr::FindNearestCharacter_Character( CCharacter * pi_pCenterChar, CCharacter * pi_pChar,
											   float pi_fLimitDist, float pi_fLimitYAngleDist )
{
	if( !pi_pCenterChar || !pi_pChar )
		return NULL;

	if( pi_pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) &&
		pi_pChar->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
	{
		if( ( pi_pChar->GetActionByCategory() == CAI_DEATH ) ||
			( pi_pChar->GetAction() == CAI_INTERNALSTATE_DEATH ) )
			return NULL;

		// 제한 거리내에 있는지 검사
		float l_fDist = GetDist( pi_pCenterChar->GetPosition(), pi_pChar->GetPosition() );
		if( l_fDist <= pi_fLimitDist )
		{
			FindNearestCharacter_Character_GetLimitYAngleByDistance( l_fDist, pi_fLimitYAngleDist );

			return FindNearestCharacter_Character_GetCharacterInLimitYAngle( pi_pCenterChar, pi_pChar, l_fDist, pi_fLimitYAngleDist );
		}
	}

	return NULL;
}

void
CCharacterMgr::FindNearestCharacter_Character_GetLimitYAngleByDistance( float pi_fDist, float & pio_fLimitYAngleDist )
{
	// pio_fLimitYAngleDist값이 0 이라면 거리에 맞춰서 pio_fLimitYAngleDist값을 설정한다.
	if( pio_fLimitYAngleDist == 0 )
	{
		const float MAX_ANGLE	= 90.0f;
		const float MIN_ANGLE	= 5.0f;

		// 거리 10당 3/4으로 줄어든다.
		pio_fLimitYAngleDist = 1.0f;
		int l_nCnt = ( int )( pi_fDist / 5.0f );
		for( int i = 0; i < l_nCnt; ++i )
			pio_fLimitYAngleDist *= 0.75;
		pio_fLimitYAngleDist *= MAX_ANGLE;
		if( pio_fLimitYAngleDist < MIN_ANGLE )
			pio_fLimitYAngleDist = MIN_ANGLE;
	}
}

CCharacter *
CCharacterMgr::FindNearestCharacter_Character_GetCharacterInLimitYAngle( CCharacter * pi_pCenterChar,
																		 CCharacter * pi_pChar,
																		 float pi_fDist,
																		 float pi_fLimitYAngleDist )
{
	static float l_fPrevLimitYAngleDist = 0;

	// 제한 Y각도 내에 있는지를 검사
	float l_fYAngleDist = GetYAngle( pi_pCenterChar->GetPosition(), pi_pChar->GetPosition() );
	l_fYAngleDist = fabs( pi_pCenterChar->GetRotY() - l_fYAngleDist );
	if( l_fYAngleDist < pi_fLimitYAngleDist )
	{
		if( l_fYAngleDist < pi_fLimitYAngleDist )
		{
			l_fPrevLimitYAngleDist = l_fYAngleDist;

			// 이전 캐릭터보다 Y각도차가 작다면, 거리가 더 가까운지를 체크한다.
			if( pi_fDist < l_fPrevLimitYAngleDist )
			{
				l_fPrevLimitYAngleDist = pi_fDist;
				return pi_pChar;
			}
		}
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CCharacterMgr::ProcessDamageFromReturnEffectMgr( CCharacter * pi_pAttacker )
{
	BOOL		bIsProcessAvatar = FALSE;
	CCharacter* pVictim = NULL;

	for( int i = 0; i <= CTI_UNIT; ++i )
	{
		for( CCharacterMgr::CHAR_LIST_ITER iter = m_listActive[i].begin();
			 iter != m_listActive[i].end(); )
		{
			if( !bIsProcessAvatar )
			{
				pVictim = m_pAvatar;
				bIsProcessAvatar = TRUE;
			}
			else
			{
				pVictim = ( *iter );
				++iter;
			}

			if( pVictim->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
			{
				CAttackerInfo * pAttackerInfo = static_cast< CAttackerInfo * > (
													pVictim->GetCombatState()->GetEnemyInfo(
																ET_ATTACKER,
																pi_pAttacker->GetCharTypeID(),
																pi_pAttacker->GetIndex() ) );
				if( pAttackerInfo )	
					pAttackerInfo->SetDamaged( TRUE );
			}
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCharacterMgr::ClearCharacterEffect( void )
{
	m_pAvatar->GetAppliedSF()->ClearEffect();

	for( int i = 0; i < MAX_CHAR_TYPE; ++i )
	{
		for( CHAR_LIST_ITER iter = m_listActive[i].begin();
			 iter != m_listActive[i].end(); ++iter )
		{
			( *iter )->GetAppliedSF()->ClearEffect();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CCharacterMgr::VisualFieldAdjustment( void )
{
	static	DWORD l_dwPrevTime = timeGetTime();

	FPS_INFO l_FpsInfo;
	l_FpsInfo.m_dwTime	= timeGetTime();
	l_FpsInfo.m_fFPS	= _GetFPS();

	if( l_FpsInfo.m_dwTime - l_dwPrevTime < 1000 )
		return;

	l_dwPrevTime = l_FpsInfo.m_dwTime;

	m_dequeFPSInfo.push_front( l_FpsInfo );

	//----------------------------------------------------------------------

	m_fAverageFPS = 0.0f;

	if( m_dequeFPSInfo.size() > 5 )
		m_dequeFPSInfo.pop_back();

	if( m_bIsEnableAutoVisualFieldAdjustment )
	{
		for( FPS_INFO_LIST_ITER iter = m_dequeFPSInfo.begin(); iter != m_dequeFPSInfo.end(); ++iter )
		{
			m_fAverageFPS += ( *iter ).m_fFPS;
		}

		if( !m_dequeFPSInfo.empty() )
			m_fAverageFPS /= ( float )m_dequeFPSInfo.size();

		//----------------------------------------------------------------------

		if( ( l_FpsInfo.m_fFPS - 3.0f >= m_fAverageFPS ) && ( l_FpsInfo.m_fFPS + 3.0f <= m_fAverageFPS ) )
		{
			return;
		}

		//----------------------------------------------------------------------

		const static float LIMIT_FRAME_DECREASE	= 30.0f;
		const static float LIMIT_FRAME_INCREASE	= 40.0f;

		if( m_fAverageFPS < LIMIT_FRAME_DECREASE )
		{
			m_fVisualField -= 10.0f;
			if( m_fVisualField < MIN_VISUAL_FIELD )
				m_fVisualField = MIN_VISUAL_FIELD;
		}
		else if( m_fAverageFPS > LIMIT_FRAME_INCREASE )
		{
			m_fVisualField += 10.0f;
			if( m_fVisualField > MAX_VISUAL_FIELD )
				m_fVisualField = MAX_VISUAL_FIELD;
		}

#ifdef _DEVELOPER_
//		sprintf( g_pBuff, "FPS : %f, num : %d, Visual : %f", m_fAverageFPS, m_dequeFPSInfo.size(), m_fVisualField );
#endif
	}
}

void
CCharacterMgr::EnableAutoVisualFieldAdjustment( BOOL pi_bFlag )
{
	m_bIsEnableAutoVisualFieldAdjustment = pi_bFlag;
	_GetOptionWindow()->SwitchOn( CGUIOptionWindow::AutoSeeDist, pi_bFlag );

	if( !pi_bFlag )
	{
		SetVisualFieldAdjustmentLevel( m_byVisualFieldAdjustmentLevel );
	}
}

void
CCharacterMgr::SetVisualFieldAdjustmentLevel( BYTE pi_byLevel )
{
	m_byVisualFieldAdjustmentLevel = pi_byLevel;
	_GetOptionWindow()->SetGraphicOption( CGUIOptionWindow::SeeDist, pi_byLevel );

	if( !m_bIsEnableAutoVisualFieldAdjustment )
	{
		static const float DIST_PER_LEVEL = ( MAX_VISUAL_FIELD - MIN_VISUAL_FIELD ) / 3.0f;
		m_fVisualField = ( MIN_VISUAL_FIELD + ( DIST_PER_LEVEL * pi_byLevel ) - DIST_PER_LEVEL );

#ifdef _DEVELOPER_
//		sprintf( g_pBuff, "Visual : %f", m_fVisualField );
#endif
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CCharacterMgr::SetMouseHoverChar( CCharacter * pi_pChar )
{
	if( pi_pChar )
	{
		m_dwMouseHoverCharID[0]	= pi_pChar->GetCharTypeID();
		m_dwMouseHoverCharID[1]	= pi_pChar->GetIndex();
	}
	else
	{
		m_dwMouseHoverCharID[0]	= ID_INVALID;
		m_dwMouseHoverCharID[1]	= ID_INVALID;
	}
}

CCharacter *
CCharacterMgr::GetMouseHoverChar( void )
{
	return GetCharacter( m_dwMouseHoverCharID[0], m_dwMouseHoverCharID[1] );
}

void
CCharacterMgr::SetSelectedChar( CCharacter * pi_pChar )
{
	if( pi_pChar )
	{
		m_dwSelectedCharID[0]	= pi_pChar->GetCharTypeID();
		m_dwSelectedCharID[1]	= pi_pChar->GetIndex();
	}
	else
	{
		m_dwSelectedCharID[0]	= ID_INVALID;
		m_dwSelectedCharID[1]	= ID_INVALID;
	}
}

CCharacter *
CCharacterMgr::GetSelectedChar( void )
{
	return GetCharacter( m_dwSelectedCharID[0], m_dwSelectedCharID[1] );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

LRESULT
CCharacterMgr::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return m_pAvatar->MsgProc( hWnd, uMsg, wParam, lParam );
}