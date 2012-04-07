////////////////////////////////////////////////////////////////////////////////
//
// CLand Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "./GameObject/Manager/CCharacterMgr.h"
#include "./GameObject/Manager/CGroundItemManager.h"

#include "CLand.h"
#include "CCamera.h"
#include "DataFile.h"

#include "./AddCodebyJang/ForEffect.h"
#include "./AddCodebyJang/EffectUtil.h"

#include "./UserInterface/SpriteMgr.h"
#include "./UserInterface/GameWindow/EtcWin/GUIMapMoveWindow.h"
#include "./UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"
#include "./UserInterface/GameWindow/EtcWin/GUIMiniMapWindow.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CLand::CLand()
{
	Init();
}

CLand::~CLand()
{
	Destroy();
}

void
CLand::Init( void )
{
	m_pLevel = NULL;

	ZeroMemory( &m_pMapInfo, sizeof( MAP_INFO ) * MAX_MAP );

	m_byMaxMapNum	= 0;
	m_byMapIndex	= ID_INVALID;
	m_byPortalIndex	= ID_INVALID;
}

BOOL
CLand::Create( void )
{
	m_pLevel = new CLevel;

	g_pLevel = m_pLevel;
	g_pLand = this;

	return TRUE;
}

BOOL
CLand::Destroy( void )
{
	UnloadLevel();

	if( m_pLevel != NULL )
	{
		delete m_pLevel;
		m_pLevel = NULL;
		g_pLevel = NULL;
		g_pLand = NULL;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// 레벨을 불러온다.
// 현재 반환값은 의미를 가지지 못한다. (차후를 위해 틀만 있는 것임)
//--------------------------------------------------------------------------
BOOL
CLand::LoadLevel( char * pi_pLevelName )
{
	assert( pi_pLevelName != NULL );
	assert( m_pLevel != NULL );

	m_pLevel->LoadLevel( pi_pLevelName );

	return TRUE;
}

//--------------------------------------------------------------------------
// 레벨을 불러온다.
// 현재 반환값은 의미를 가지지 못한다. (차후를 위해 틀만 있는 것임)
// pi_byMapIndex 가 0xFF라면 현재 세팅해 놓은 맵 인덱스로 읽어들인다.
//--------------------------------------------------------------------------
BOOL
CLand::LoadLevel( BYTE pi_byMapIndex )
{
	assert( m_pLevel != NULL );

	if( pi_byMapIndex == 0xFF )
		pi_byMapIndex = m_byMapIndex;


	_GetCamera()->SetSpecialCameraMode(0);	//특수 카메라는 0으로 셋팅.
	SetStateFullScreenEffect(FALSE);
	::EFUtil_MusicStart(pi_byMapIndex);	//음악이 맵 인덱스별로 설정되므로 레벨로딩후 호출.

	m_pLevel->LoadLevel( m_pMapInfo[pi_byMapIndex].pFileName );
	SetFadeIn(0x00,0,1.0f);	//혹시화면꺼놓은게 있다면


	UI_SetRadar();
	//UI_SetMinimap( pi_byMapIndex );

	_GetCharMgr()->ClearCharacterEffect();
	_GetGroundItemMgr()->ClearItemEffect();

	CreateEffect();

	return TRUE;
}

//--------------------------------------------------------------------------
// 불러온 레벨을 해제한다.
// 현재 반환값은 의미를 가지지 못한다. (차후를 위해 틀만 있는 것임)
//--------------------------------------------------------------------------
BOOL
CLand::UnloadLevel( void )
{
	assert( m_pLevel != NULL );

	//음악내린다.
	::EFUtil_MusicEnd();

	m_pLevel->ReleaseLevel();

	DestroyEffect();

	return TRUE;
}

//--------------------------------------------------------------------------
// 레벨을 렌더링한다. (Alpha 부분 제외)
//--------------------------------------------------------------------------
BOOL
CLand::Render( Vector3f & pi_vecPos )
{
	assert( m_pLevel );

	if( !m_pLevel->IsLoadedBsp() ) return FALSE;

	// 아래의 2개 함수는 맵을 렌더링 하기전 반드시 호출해 줘야한다.
	m_pLevel->SetCameraPos( pi_vecPos );
	m_pLevel->SetViewMatrix( R3MoveGetViewMatrix() );

	m_pLevel->DrawSkyBoxRender();
	m_pLevel->DrawMapRender();
	m_pLevel->DrawMapEntitiesRender();	//맵 엔티티들 렌더링 스택에저장.

	return TRUE;
}

//--------------------------------------------------------------------------
// 레벨을 렌더링한다. (Alpha 부분만)
//--------------------------------------------------------------------------
BOOL
CLand::RenderAlpha( Vector3f & pi_vecPos )
{
	m_pLevel->DrawMapAlphaRender( pi_vecPos );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 미니맵 세팅
void
CLand::OpenMinimapWindow( char * pi_pMapName, char * pi_pItemName )
{
	if( !_GetMinimapWindow() || !pi_pMapName || !pi_pItemName )
		return;

	for( int i = 0; i < m_byMaxMapNum; ++i )
	{
		if( stricmp( m_pMapInfo[i].pName, pi_pMapName ) == 0 )
		{
			_strcpy( m_pMapInfo[i].pName_UI, pi_pItemName, MAX_NAME_LENGTH );
			UI_SetMinimap( m_pMapInfo[i].byIndex );
			_GetMinimapWindow()->Open();
			break;
		}
	}
}

// 미니맵 세팅
void
CLand::UI_SetMinimap( BYTE pi_byMapIndex )
{
	if( !_GetMinimapWindow() )
		return;

	if( pi_byMapIndex == 0xFF )
		pi_byMapIndex = m_byMapIndex;

	// 스프라이트 로딩
	_GetUISpriteMgr()->UnloadSprite( SP_ID_MINIMAP );
	_GetUISpriteMgr()->LoadSprite( SP_ID_MINIMAP, m_pMapInfo[pi_byMapIndex].pMinimapFileName );

	// 미니맵 UI 세팅
	_GetMinimapWindow()->Init();
	_GetMinimapWindow()->SetID( UII_MINIMAP_WINDOW );

	// 실제맵 크기 세팅
	RECT l_rectMap;
	l_rectMap.left	= m_pLevel->mBsp->mNode[1].bb_min[0];		
	l_rectMap.top	= m_pLevel->mBsp->mNode[1].bb_min[2];
	l_rectMap.right	= m_pLevel->mBsp->mNode[1].bb_max[0];		
	l_rectMap.bottom= m_pLevel->mBsp->mNode[1].bb_max[2];
	
	_GetMinimapWindow()->SetMapBound( l_rectMap );
	_GetMinimapWindow()->SetMapRotate( m_pMapInfo[pi_byMapIndex].wMinimapAngle );
	_GetMinimapWindow()->SetMapIndex( m_byMapIndex, pi_byMapIndex );
	_GetMinimapWindow()->SetMapName( m_pMapInfo[pi_byMapIndex].pName_UI );

	// 포탈 세팅
	if( m_byMapIndex == pi_byMapIndex )
	{
		POINT l_ptPos;
		for( int i = 0; i < m_pMapInfo[pi_byMapIndex].byPortalNum; ++i )
		{
			l_ptPos.x = m_pMapInfo[pi_byMapIndex].pPortalInfo[i].vecMin[0] + ( ( m_pMapInfo[pi_byMapIndex].pPortalInfo[i].vecMax[0] - m_pMapInfo[pi_byMapIndex].pPortalInfo[i].vecMin[0] ) / 2 );
			l_ptPos.y = m_pMapInfo[pi_byMapIndex].pPortalInfo[i].vecMin[2] + ( ( m_pMapInfo[pi_byMapIndex].pPortalInfo[i].vecMax[2] - m_pMapInfo[pi_byMapIndex].pPortalInfo[i].vecMin[2] ) / 2 );

			_GetMinimapWindow()->AddMiniMapObject( CGUIMiniMapWindow::Teleport,
												   m_pMapInfo[pi_byMapIndex].pPortalInfo[i].byIndex | ( CGUIMiniMapWindow::Teleport << 28 ),
												   l_ptPos.x, l_ptPos.y );															   
		}
	}

	// 툴팁 세팅
	if( m_byMapIndex == pi_byMapIndex )
	{
		for( int i = 0; i < m_pMapInfo[pi_byMapIndex].byAreaNum; ++i )
		{
			_GetMinimapWindow()->AddTooltipObject( m_pMapInfo[pi_byMapIndex].pAreaInfo[i].dwIndex,
												   m_pMapInfo[pi_byMapIndex].pAreaInfo[i].recArea.left,
												   m_pMapInfo[pi_byMapIndex].pAreaInfo[i].recArea.top,
												   m_pMapInfo[pi_byMapIndex].pAreaInfo[i].recArea.right - m_pMapInfo[pi_byMapIndex].pAreaInfo[i].recArea.left,
												   m_pMapInfo[pi_byMapIndex].pAreaInfo[i].recArea.bottom - m_pMapInfo[pi_byMapIndex].pAreaInfo[i].recArea.top,
												   m_pMapInfo[pi_byMapIndex].pAreaInfo[i].pName );									   
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 맵 이동 UI 를 연다.
void
CLand::UI_OpenMoveMapWindow( void )
{
	if( !_GetMoveMapWindow() )
		return;

	if( _GetAvatar()->GetMoveMapInfo()->m_bIsMoved )
		return;

	static	char *	l_pPortalName[MAX_PORTAL];
	static	BYTE	l_byPortalIndex[MAX_PORTAL];
			int		l_nUsablePortalCnt = 0;

	for( int i = 0; i < m_pMapInfo[m_byMapIndex].byPortalNum; ++i )
	{
		if( m_pMapInfo[m_byMapIndex].pPortalInfo[i].bIsUsable )
		{
			//for( int j = 0; j < m_pMapInfo[m_byMapIndex].pPortalInfo[i].byLinkedPortalNum; ++j )
			// => 모든 포탈은 하나씩만 이어진다고 생각하므로 이 부분은 일단 제외한다.
			// => 다방향 포탈은 한 곳에 여러개를 겹쳐 찍어서 해결한다.
			{
				l_pPortalName[l_nUsablePortalCnt] = m_pMapInfo[m_byMapIndex].pPortalInfo[i].pDesc;
				l_byPortalIndex[l_nUsablePortalCnt] = m_pMapInfo[m_byMapIndex].pPortalInfo[i].byIndex;
				++l_nUsablePortalCnt;
			}
		}
	}

	if( l_nUsablePortalCnt > 0 )
	{
		_GetMoveMapWindow()->AddMapName( l_pPortalName, l_byPortalIndex, l_nUsablePortalCnt );
		if( !_GetMoveMapWindow()->IsShow() )
			_GetMoveMapWindow()->Open();
	}
	else
	{
		if( _GetMoveMapWindow()->IsShow() )
			_GetMoveMapWindow()->Close();
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 레이더 세팅
void
CLand::UI_SetRadar( void )
{
	if( _GetRadarWindow() )
	{
		// 반경 340내에 캐릭터를 표시한다.
		POINT l_ptSize;
		l_ptSize.x = 680;
		l_ptSize.y = 680;

		// 레이다상에 보이는 실제 맵 크기
		_GetRadarWindow()->SetMapSize( l_ptSize );
		_GetRadarWindow()->SetMapRotate( 0 );
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CLand::RenderInfo( void )
{
	int l_nPosX = 10, l_nPosY = 270;
	int l_nUsablePortalCnt = -1;

	char l_pBuf[128];
	int i;

	static const char PORTAL_TYPE[2][16] = { "Road", "Way Point" };

	if( m_byMapIndex == 0xFF )
		return;

	//--------------------------------------------------------------------------
	// 포탈 정보

	// 사각형
	for( i = 0; i < m_pMapInfo[m_byMapIndex].byPortalNum; ++i )
		if( m_pMapInfo[m_byMapIndex].pPortalInfo[i].bIsUsable )
			++l_nUsablePortalCnt;

	if( l_nUsablePortalCnt > -1 )
		Draw2DRectangle( 5, 265, 350, l_nPosY + ( l_nUsablePortalCnt * 15 ) + 20, 0x70020250 );

	// 정보 출력
	l_nUsablePortalCnt = -1;
	for( i = 0; i < m_pMapInfo[m_byMapIndex].byPortalNum; ++i )
	{
		if( m_pMapInfo[m_byMapIndex].pPortalInfo[i].bIsUsable )
		{
			++l_nUsablePortalCnt;

			sprintf( l_pBuf, "< Ctrl + %d > : %s ( %s )",
					 l_nUsablePortalCnt+1, m_pMapInfo[m_byMapIndex].pPortalInfo[i].pDesc,
					 PORTAL_TYPE[m_pMapInfo[m_byMapIndex].pPortalInfo[i].byPortalType] );

			DrawR3Hangul( l_nPosX, l_nPosY + ( i * 15 ),
						  l_pBuf, D3DCOLOR_XRGB( 234, 140, 0 ) );
		}

		// 임시 코드 - 포탈의 영역 렌더링
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );
		GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
		GetD3dDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );

		_GetLevel()->DrawBBox( m_pMapInfo[m_byMapIndex].pPortalInfo[i].vecMin,
							   m_pMapInfo[m_byMapIndex].pPortalInfo[i].vecMax,
							   D3DCOLOR_XRGB( 100, 255, 100 ) );

		GetD3dDevice()->SetRenderState( D3DRS_LIGHTING, TRUE );
	}

	//if( l_nUsablePortalCnt > -1 )
	//	Draw2DRectangle( 5, 265, 305, l_nPosY + ( l_nUsablePortalCnt * 15 ) + 20, 0x70020250 );

	//--------------------------------------------------------------------------
	// 광산 정보
	
	for( i = 0; i < m_pMapInfo[m_byMapIndex].byMineNum; ++i )
	{
		// 임시 코드 - 광산의 영역 렌더링
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );
		GetD3dDevice()->SetTransform( D3DTS_WORLD, &matWorld );
		GetD3dDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );

		_GetLevel()->DrawBBox( m_pMapInfo[m_byMapIndex].pMineInfo[i].vecMin,
							   m_pMapInfo[m_byMapIndex].pMineInfo[i].vecMax,
							   D3DCOLOR_XRGB( 100, 255, 100 ) );

		GetD3dDevice()->SetRenderState( D3DRS_LIGHTING, TRUE );
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CLand::LoadData( void )
{
	CDataFile l_fileMapInfo( "./DataTable/Map.edf" );

	CDataString * l_pSourceData = l_fileMapInfo.GetSourceData();
	if( !l_pSourceData )
		return FALSE;
	if( !l_pSourceData->GetString() )
		return FALSE;

	BYTE l_byMaxMapNum;
	l_pSourceData->Read( &l_byMaxMapNum, 1, 1 );
	_GetLand()->SetMaxMapNum( l_byMaxMapNum );

	MAP_INFO *		l_pMapInfo		= NULL;
	PORTAL_INFO	*	l_pPortalInfo	= NULL;

	int i, j, k;
	for( i = 0; i < l_byMaxMapNum; ++i )
	{
		l_pMapInfo = _GetLand()->GetMapInfo( i );

		l_pSourceData->Read( &l_pMapInfo->byIndex, 1, 1 );
		l_pSourceData->Read( l_pMapInfo->pName, 32, 1 );
		_strcpy( l_pMapInfo->pName_UI, l_pMapInfo->pName, 32 );
		l_pSourceData->Read( l_pMapInfo->pFileName, 128, 1 );

		l_pSourceData->Read( l_pMapInfo->pMinimapFileName, 128, 1 );
		l_pSourceData->Read( &l_pMapInfo->wMinimapAngle, 2, 1 );

		l_pSourceData->Read( &l_pMapInfo->byPortalNum, 1, 1 );

		for( j = 0; j < l_pMapInfo->byPortalNum; ++j )
		{
			l_pPortalInfo = &l_pMapInfo->pPortalInfo[j];

			l_pSourceData->Read( &l_pPortalInfo->byMapIndex, 1, 1 );
			l_pSourceData->Read( &l_pPortalInfo->byIndex, 1, 1 );
			l_pSourceData->Read( &l_pPortalInfo->byPortalType, 1, 1 );

			l_pSourceData->Read( &l_pPortalInfo->bIsMovable, 1, 1 );
			l_pSourceData->Read(  l_pPortalInfo->byUsableRace, 5, 1 );

			l_pSourceData->Read( l_pPortalInfo->pName, 32, 1 );
			l_pSourceData->Read( l_pPortalInfo->pDesc, 64, 1 );

			l_pSourceData->Read( &l_pPortalInfo->vecMin[0], sizeof( float ), 1 );
			l_pSourceData->Read( &l_pPortalInfo->vecMin[1], sizeof( float ), 1 );
			l_pSourceData->Read( &l_pPortalInfo->vecMin[2], sizeof( float ), 1 );
			l_pSourceData->Read( &l_pPortalInfo->vecMax[0], sizeof( float ), 1 );
			l_pSourceData->Read( &l_pPortalInfo->vecMax[1], sizeof( float ), 1 );
			l_pSourceData->Read( &l_pPortalInfo->vecMax[2], sizeof( float ), 1 );

			l_pSourceData->Read( &l_pPortalInfo->byLinkedPortalNum, 1, 1 );

			for( k = 0; k < l_pPortalInfo->byLinkedPortalNum; ++k )
			{
				l_pSourceData->Read( &l_pPortalInfo->byLinkedPortal[k][0], 1, 1 );
				l_pSourceData->Read( &l_pPortalInfo->byLinkedPortal[k][1], 1, 1 );
			}
		}

		//------------------------------------------------------------------

		MINE_INFO * l_pMineInfo = NULL;

		l_pSourceData->Read( &l_pMapInfo->byMineNum, 1, 1 );

		for( j = 0; j < l_pMapInfo->byMineNum; ++j )
		{
			l_pMineInfo = &l_pMapInfo->pMineInfo[j];

			l_pSourceData->Read( &l_pMineInfo->byMapIndex, 1, 1 );
			l_pSourceData->Read( &l_pMineInfo->byIndex, 1, 1 );

			l_pSourceData->Read( &l_pMineInfo->vecMin[0], sizeof( float ), 1 );
			l_pSourceData->Read( &l_pMineInfo->vecMin[1], sizeof( float ), 1 );
			l_pSourceData->Read( &l_pMineInfo->vecMin[2], sizeof( float ), 1 );
			l_pSourceData->Read( &l_pMineInfo->vecMax[0], sizeof( float ), 1 );
			l_pSourceData->Read( &l_pMineInfo->vecMax[1], sizeof( float ), 1 );
			l_pSourceData->Read( &l_pMineInfo->vecMax[2], sizeof( float ), 1 );
		}

		//------------------------------------------------------------------

		l_pSourceData->Read( &l_pMapInfo->byAreaNum, 1, 1 );
		if( l_pMapInfo->byAreaNum > 0 )
			l_pSourceData->Read( l_pMapInfo->pAreaInfo, sizeof( MAP_AREA_INFO ), l_pMapInfo->byAreaNum );
	}

	return TRUE;
}