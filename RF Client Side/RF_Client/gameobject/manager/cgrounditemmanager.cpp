////////////////////////////////////////////////////////////////////////////////
//
// CGroundItemManager Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "CCharacterMgr.h"
#include "../Board/Common/CBoardManager.h"

#include "CGroundItemManager.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CGroundItemManager g_clsGroundItemManager;

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CGroundItemManager::CGroundItemManager()
{
	m_fVisualField		= MAX_VISUAL_FIELD;
	m_bIsDrawBBox		= FALSE;
	m_bIsRenderItemName	= FALSE;
}

CGroundItemManager::~CGroundItemManager()
{
	Clear();
}

void
CGroundItemManager::Clear( void )
{
	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
	{
		delete ( *itItem );
		( *itItem ) = NULL;
	}
	m_listItem.clear();
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CGroundItemManager::Render( void )
{
	_GetCharIF()->SetState();

	CGroundItem * pItem = NULL;

	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
	{
		pItem = ( *itItem );

		if( pItem->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) &&
			pItem->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
		{
			if( pItem->ProcessClipping() ) 
				continue;

			// is render bound box?
			( m_bIsDrawBBox ) ? pItem->Enable( ROSF_RENDER_BBOX ) : pItem->Disable( ROSF_RENDER_BBOX );

			// is render name?
			if( m_bIsRenderItemName )
				pItem->Enable( ROSF_RENDER_NAME );
			else if( pItem->IsEqualItemKind( m_clsHoverItem.GetItemKind() ) &&
					 pItem->IsEqualIndex( m_clsHoverItem.GetIndex() ) )
				pItem->Enable( ROSF_RENDER_NAME );
			else
				pItem->Disable( ROSF_RENDER_NAME );

			// render mesh
			pItem->Render();
			pItem->RenderName();
		}
	}

	_GetCharIF()->UnSetState();
}

void
CGroundItemManager::Animation( void )
{
	static const DWORD ITEM_DESTROY_TIME = 60000;

	DWORD			m_dwCurTime	= timeGetTime();
	CGroundItem *	pItem		= NULL;

	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
	{
		pItem = ( *itItem );

		if( ( m_dwCurTime - pItem->GetCreateTime() > ITEM_DESTROY_TIME ) || pItem->IsEnable( ROSF_DESTROY ) )
		{
			DeleteItem( itItem );
			continue;
		}

		pItem->ProcessTimeOverFromReceiveFixPositionMsg();

		if( pItem->IsDisable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) && pItem->IsEnable( ROSF_ANIMATION ) )
		{
			pItem->FrameMove();
			pItem->Animation();
		}
	}
}

void
CGroundItemManager::FrameMove( void )
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


// 스크린 좌표와 모든 아이템 오브젝트와의 충돌을 처리함
// 반환값 - 충돌된 객체의 포인터
CObject *
CGroundItemManager::CollisionCheckWithBoundBox( int pi_nPosX, int pi_nPosY,
											   Vector3f & pi_vecCamPos, Vector3f & po_vecCollidedPoint )
{
	COLLISION_LIST	listCollision;
	CCollisionItem	clsCollisionItem;

	CGroundItem *	pItem = NULL;
	BOOL			bIsCollided = FALSE;

	// find collision item
	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
	{
		pItem = ( *itItem );

		if( pItem->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) ||
			pItem->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
			continue;

		if( !pItem->ProcessClipping() && pItem->IsEnable( ROSF_RENDER ) )
		{
			bIsCollided = pItem->CollisionCheckWithBoundBox( pi_nPosX, pi_nPosY, clsCollisionItem.m_vecCollidedPoint );
			if( bIsCollided )
			{
				clsCollisionItem.m_pGroundItem = pItem;
				listCollision.push_back( clsCollisionItem );
			}
		}
	}

	//--------------------------------------------------------------------------

	if( listCollision.empty() )
		return NULL;

	float			fMaxDist, fMinDist = 1000000000.0f;
	CGroundItem *	pSelectItem = NULL;

	// find nearest item
	for( COLLISION_LIST_ITER itCollision = listCollision.begin(); itCollision != listCollision.end(); ++itCollision )
	{
		fMaxDist = GetDist( pi_vecCamPos, ( *itCollision ).m_pGroundItem->GetPosition() );

		if( fMaxDist < fMinDist )
		{
			fMinDist		= fMaxDist;
			pSelectItem		= ( *itCollision ).m_pGroundItem;
			memcpy( po_vecCollidedPoint, ( *itCollision ).m_vecCollidedPoint, sizeof( Vector3f ) );
		}
	}

	return pSelectItem;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGroundItemManager::CheckOutPositionOfVisualField( void )
{
	CGroundItem *	pItem = NULL;
	float			fVisualField_Client = m_fVisualField - 20.0f;

	Vector3f		vecItemPos, vecAvatarPos;

	memcpy( vecAvatarPos, _GetAvatar()->GetPosition(), sizeof( Vector3f ) );

	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
	{
		pItem = ( *itItem );
		memcpy( vecItemPos, pItem->GetPosition(), sizeof( Vector3f ) );

		//----------------------------------------------------------------------
		// ROSF_OUT_OF_VISUAL_FIELD_SERVER 체크
		// ROSF_OUT_OF_VISUAL_FIELD_SERVER 상태에서는 캐릭터는 아무것도 수행하지 않는다.
		// 포지션 관련 네트웍 메세지를 받아야만 Disable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) 상태로 복구된다.
		if( !(
			( ( vecItemPos[0] < vecAvatarPos[0] + m_fVisualField ) &&
			( vecItemPos[0] > vecAvatarPos[0] - m_fVisualField ) ) &&
			( ( vecItemPos[2] < vecAvatarPos[2] + m_fVisualField ) &&
			( vecItemPos[2] > vecAvatarPos[2] - m_fVisualField ) )
			) )
		{
			// FixPosition 메세지를 받았다면 시야거리에 들어갈때 까지 무조건
			// ROSF_OUT_OF_VISUAL_FIELD_SERVER 를 Disable 시킨다.
			if( pItem->IsReceiveFixPositionMsg() )
				pItem->Disable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );
			else
				pItem->Enable( ROSF_OUT_OF_VISUAL_FIELD_SERVER );
		}
		//----------------------------------------------------------------------
		// ROSF_OUT_OF_VISUAL_FIELD_CLIENT 체크
		// ROSF_OUT_OF_VISUAL_FIELD_CLIENT => Enable
		// ROSF_OUT_OF_VISUAL_FIELD_CLIENT 상태에서는 캐릭터는 렌더링과 피킹만 되지 않을뿐 나머지는 모두 수행한다.
		else if( !(
			( ( vecItemPos[0] < vecAvatarPos[0] + fVisualField_Client ) &&
			( vecItemPos[0] > vecAvatarPos[0] - fVisualField_Client ) ) &&
			( ( vecItemPos[2] < vecAvatarPos[2] + fVisualField_Client ) &&
			( vecItemPos[2] > vecAvatarPos[2] - fVisualField_Client ) )
			) )
		{
			// FixPosition 메세지를 받았다면 시야거리에 들어갈때 까지 무조건
			// ROSF_OUT_OF_VISUAL_FIELD_CLIENT 를 Disable 시킨다.
			if( pItem->IsReceiveFixPositionMsg() )
				pItem->Disable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT );
			else
				pItem->Enable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT );
		}
		//----------------------------------------------------------------------
		// ROSF_OUT_OF_VISUAL_FIELD_CLIENT => Disable
		else
		{
			pItem->Disable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT );
			pItem->SetReceiveFixPositionMsg( FALSE );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아이템 생성을 위한 체크 - 몬스터가 죽었을 경우를 위해
BOOL
CGroundItemManager::CheckGroundItemForCreating( CCharacter * pi_pChar )
{
	if( !pi_pChar )
		return FALSE;

	CGroundItem * pItem = NULL;

	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
	{
		pItem = ( *itItem );

		if( pi_pChar->IsEqual( pItem->GetDroperTypeID(), pItem->GetDroperIndex() ) )
		{
			pItem->Enable( ROSF_RENDER | ROSF_ANIMATION );
			pItem->SetDroper( ID_INVALID, ID_INVALID );
		}
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타가 루팅가능한 아이템을 가져온다. ( 아바타에서 가장 가까운 아이템 )
CGroundItem *
CGroundItemManager::GetLootableItem( void )
{
	CGroundItem *	pItem			= NULL;
	CGroundItem *	pLootableItem	= NULL;
	float			fFinalDist		= 100.0f;
	float			fCurDist		= 0.0f;

	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
	{
		pItem = ( *itItem );

		if( pItem->IsDisable( ROSF_RENDER ) )
			continue;
		if( pItem->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) || pItem->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
			continue;

		// 아바타가 획득할 수 있는 아이템인지를 먼저 체크한다.
		if( pItem->GetOwnerIndex() == _GetAvatar()->GetIndex() ||
			pItem->GetOwnerIndex() == ID_INVALID )
		{
			fCurDist = GetDist( _GetAvatar()->GetPosition(), pItem->GetPosition() );
			if( fCurDist < fFinalDist )
			{
				fFinalDist		= fCurDist;
				pLootableItem	= pItem;
			}
		}
	}

	return pLootableItem;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CGroundItem *
CGroundItemManager::AddItem( void )
{
	CGroundItem * pNewItem = new CGroundItem;
	
	m_listItem.push_back( pNewItem );

	return pNewItem;
}

void
CGroundItemManager::DeleteItem( DWORD pi_dwIndex )
{
	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
	{
		if( ( *itItem )->IsEqual( OTI_ITEM, pi_dwIndex ) )
		{
			DeleteItem( itItem );
			return;
		}
	}
}

void
CGroundItemManager::DeleteItem( ITEM_LIST_ITER & pio_itItem )
{
	( *pio_itItem )->DeleteItemEffect();

	delete ( *pio_itItem );
	( *pio_itItem ) = NULL;

	pio_itItem = m_listItem.erase( pio_itItem );
	--pio_itItem;
}

CGroundItem *
CGroundItemManager::GetItem( DWORD pi_dwIndex )
{
	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
	{
		if( ( *itItem )->IsEqual( OTI_ITEM, pi_dwIndex ) )
			return ( *itItem );
	}

	return NULL;
}

CGroundItem *
CGroundItemManager::GetItemByMeshID( DWORD pi_dwMeshID )
{
	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
	{
		if( ( *itItem )->GetMeshID() == pi_dwMeshID )
			return ( *itItem );
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CGroundItemManager::ClearItemEffect( void )
{
	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
		( *itItem )->ClearItemEffect();
}

void
CGroundItemManager::SetMouseHoverItem( CGroundItem * pi_pGroundItem )
{
	if( pi_pGroundItem )
		memcpy( &m_clsHoverItem, pi_pGroundItem, sizeof( CGroundItem ) );
	else
		m_clsHoverItem.SetEmpty();
}

CGroundItem *
CGroundItemManager::GetMouseHoverItem( void )
{
	return &m_clsHoverItem;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// MAYUN < begin > -------------------------------------------------------------

CItemEffectRenderer g_clsItemEffectRenderer;

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CItemEffectRenderer::CItemEffectRenderer()
{
	Clear();
}

CItemEffectRenderer::~CItemEffectRenderer()
{
	Clear();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CItemEffectRenderer::AddItem( CMeshItem * pi_pOrgItem )
{
	if( !pi_pOrgItem )
		return;

	CMeshItem * pNewItem = new CMeshItem;
	memcpy( pNewItem, pi_pOrgItem, sizeof( CMeshItem ) );

	CMesh * pMesh = pi_pOrgItem->GetMesh();
	m_MaxObj[m_Max] = pMesh->m_MaxObj;

	if(m_MaxObj[m_Max]>=64) m_MaxObj[m_Max] = 63;
	for( int i = 0; i< m_MaxObj[m_Max]; i++)
	{
		if ( !pMesh->m_pMesh[i].m_BlendNum)
			memcpy( &m_Matrix[m_Max][i][0],&pMesh->m_pMesh[i].m_MatResult,sizeof(D3DXMATRIX));
		else
		{
			for (int j = 0;j<pMesh->m_pMesh[i].m_BlendNum;j++)
			{
				memcpy( &m_Matrix[m_Max][i][j],&pMesh->m_pMesh[i].m_pMatBlend[j],sizeof(D3DXMATRIX));
			}
		}
	}
	m_Max++;

	m_listItem.push_back( pNewItem );
}

void
CItemEffectRenderer::Clear( void )
{
	m_Max	= 0;
	m_Start	= 0;
	for (int i = 0; i<MAX_EFFECT_OBJ;i++)
	{
		m_MaxObj[i] = 0;
		m_BlendNum[i] = 0;
	}

	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
	{
		delete ( *itItem );
		( *itItem ) = NULL;
	}

	m_listItem.clear();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CItemEffectRenderer::Render( void )
{
	//_GetCharIF()->SetState();

	CMeshItem * pItem = NULL;
	CMesh *		pMesh = NULL;

	for( ITEM_LIST_ITER itItem = m_listItem.begin(); itItem != m_listItem.end(); ++itItem )
	{
		pItem = ( *itItem );

		m_Start++;
		if( !pItem )
		{
			continue;
		}

		//
		if (m_Start>m_Max)
		{
			continue;
		}

		pMesh = pItem->GetMesh();
		for( int i = 0; i< m_MaxObj[m_Start-1]; i++)
		{
			if ( !pMesh->m_pMesh[i].m_BlendNum)
				memcpy( &pMesh->m_pMesh[i].m_MatResult,&m_Matrix[m_Start-1][i][0],sizeof(D3DXMATRIX));
			else
			{
				for (int j = 0;j<pMesh->m_pMesh[i].m_BlendNum;j++)
				{
					memcpy( &pMesh->m_pMesh[i].m_pMatBlend[j],&m_Matrix[m_Start-1][i][j],sizeof(D3DXMATRIX));
				}
			}
		}

		//
		pItem->RenderWithShadow();
	}

	_GetCharIF()->UnSetState();

	return TRUE;
}

// MAYUN < end > ---------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////