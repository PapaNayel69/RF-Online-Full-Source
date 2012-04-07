////////////////////////////////////////////////////////////////////////////
//
// CGUIMiniMapWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIMiniMapWindow.h"
#include "../../SpriteMgr.h"
#include "GUIRadarWindow.h"
#include "../../GUIController/GUIScreen.h"

extern CGUIRadarWindow * g_pUiRadarWindow;

#define MAX_MAP_ZOOM_OUT_RATE	2.0

CGUIMiniMapWindow::CGUIMiniMapWindow()
{	
	ZeroMemory( &m_rtMapBound, sizeof( RECT ) );

	m_nMapRotate = 0;

	m_ptAvatarPos.x = 0;
	m_ptAvatarPos.y = 0;

	m_ptRealMinimapSize.x = 0;
	m_ptRealMinimapSize.y = 0;

	m_ptCutSize.x = 0;
	m_ptCutSize.y = 0;

	m_fPixelPerMeter[0] = 0.f;
	m_fPixelPerMeter[1] = 0.f;

	m_byMapIndex		= 0xFF;
	m_byMiniMapIndex	= 0xFF;
	

	m_bCreated = FALSE;

	SetSize( 402, 427 );
}

CGUIMiniMapWindow::~CGUIMiniMapWindow()
{
	RemoveAllMiniMapObject();

	RemoveAllTooltipObject();
}

void
CGUIMiniMapWindow::Init( void )
{
	SetSize( 402, 427 );
	
	m_uiTitleBoard.SetSize( 402, 26 );	
	m_uiCloseButton.SetSize( 16, 13 );

	m_uiMiniMap.SetSize( 400, 400 );

	m_uiObjectBoard.SetSize( m_uiMiniMap.m_ptSize );
	m_uiTooltipObjBoard.SetSize( m_uiMiniMap.m_ptSize );

	m_uiAvatarObject.SetSize( 16, 16 );	
	

	// ---------------------------------------------------------------------	
	m_uiTitleBoard.SetPos( m_ptPos );
	m_uiCloseButton.SetPos( m_ptPos.x + 385, m_ptPos.y + 4 );

	m_uiMiniMap.SetPos( m_ptPos.x + 1, m_ptPos.y + m_uiTitleBoard.m_ptSize.y );
	m_uiObjectBoard.SetPos( m_uiMiniMap.m_ptPos );
	m_uiTooltipObjBoard.SetPos( m_uiMiniMap.m_ptPos );

	m_uiAvatarObject.SetPos( m_uiMiniMap.m_ptPos.x + ( m_uiMiniMap.m_ptSize.x - m_uiAvatarObject.m_ptSize.x ) / 2,
							 m_uiMiniMap.m_ptPos.y + ( m_uiMiniMap.m_ptSize.y - m_uiAvatarObject.m_ptSize.y ) / 2 );
	

	m_uiAvatarPosition.SetPos( m_ptPos.x + 16, m_ptPos.y + 6 );	
	

	// ---------------------------------------------------------------------	
	Add( &m_uiTitleBoard );
	Add( &m_uiTitle );
	m_uiCloseButton.AddMouseEventHandler( this );
	Add( &m_uiCloseButton );

//	Add( &m_uiAvatarPosition );	

	Add( &m_uiMiniMap );
//	Add( &m_uiObjectBoard );
//	Add( &m_uiAvatarObject );

	// ---------------------------------------------------------------------
	m_bCreated = FALSE; // init()할때마다 sprite를 다시 세팅한다

	SetSprite();

	m_uiAvatarObject.SetTotalFrameNum( 4 );
	m_uiAvatarObject.SetObjectNumInLine( 1 );
	m_uiAvatarObject.SetFPS( 10 );
	m_uiAvatarObject.SetStopInterval( 100 );

	
	SetMoveObject( &m_uiTitle );
	SetMoveObject( &m_uiTitleBoard );
	SetMovable( TRUE );		

	m_bCreated = TRUE;
}

void
CGUIMiniMapWindow::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );

		l_spriteInfo.wActionNo = 10;
		
		l_spriteInfo.wFrameNo = 0;
		m_uiTitleBoard.SetSpriteInfo( &l_spriteInfo );		

		// avatar		
		l_spriteInfo.wFrameNo = 1;
		m_uiAvatarObject.SetSpriteInfo( &l_spriteInfo );		

		// close
		m_uiCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_uiCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_uiCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );


		// minimap
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_MINIMAP );
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 0;
		m_uiMiniMap.SetSpriteInfo( &l_spriteInfo );

		if( l_spriteInfo.pSprite )
		{
			m_ptSpriteMapSize.x = l_spriteInfo.pSprite->GetSpriteOrgXL( l_spriteInfo.wActionNo, l_spriteInfo.wFrameNo );
			m_ptSpriteMapSize.y = l_spriteInfo.pSprite->GetSpriteOrgYL( l_spriteInfo.wActionNo, l_spriteInfo.wFrameNo );		
		}
		else
		{
			m_ptSpriteMapSize.x = 0;
			m_ptSpriteMapSize.y = 0;
		}

		SetPixelPerMeter();	
	}
	else
	{	
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		if( !l_pSprite )
			return;

		m_uiTitleBoard.GetSpriteInfo()->pSprite = l_pSprite;

		// avatar
		m_uiAvatarObject.GetSpriteInfo()->pSprite = l_pSprite;		

		// minimap object
		CGUIMiniMapObjList::iterator it;
		for( it=m_listMiniMapObject.begin(); it!=m_listMiniMapObject.end(); ++it )	
		{
			if( (*it)->uiObject.GetSpriteInfo()->pSprite != NULL )
				(*it)->uiObject.GetSpriteInfo()->pSprite = l_pSprite;
		}
		
		// close
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		m_uiCloseButton.SetSprite( l_pSprite );		

		// minimap
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_MINIMAP );
		m_uiMiniMap.GetSpriteInfo()->pSprite = l_pSprite;						
	}
}

void
CGUIMiniMapWindow::SetMapName( char * pi_pName )
{
	m_uiTitle.SetString( pi_pName );

	m_uiTitle.SetPos( m_uiTitleBoard.m_ptPos.x + (m_uiTitleBoard.m_ptSize.x - m_uiTitle.m_ptSize.x)/2,
					  m_uiTitleBoard.m_ptPos.y + (m_uiTitleBoard.m_ptSize.y - m_uiTitle.m_ptSize.y)/2 );
}

void
CGUIMiniMapWindow::SetMapIndex( BYTE pi_byMapIndex, BYTE pi_byMiniMapIndex )
{
	m_byMapIndex		= pi_byMapIndex;
	m_byMiniMapIndex	= pi_byMiniMapIndex;		

	if( m_byMapIndex > 7 )
		m_byMapIndex = 0;
	if( m_byMiniMapIndex > 7 )
		m_byMiniMapIndex = 0;

	// 실제 미니맵 크기 설정
	POINT l_ptRealMinimapSize[8] = { { 387, 540 }, { 454, 499 }, { 410, 345 }, { 400, 259 },
									{ 387, 540 }, { 387, 540 }, { 387, 540 }, { 387, 540 } };

	m_ptRealMinimapSize = l_ptRealMinimapSize[m_byMiniMapIndex];

	SetPixelPerMeter();

	
	// left, top에 잘리는 부분
	POINT l_ptCutSize[8] = { { -13, 140 }, { 27, 99 }, { 0, -27 }, { 0, -70 },
							{ -13, 140 }, { -13, 140 }, { -13, 140 }, { -13, 140 } };
	// 실제 미니맵에서 잘려서 안나오는 부분 설정
//	m_ptCutSize.x = m_ptRealMinimapSize.x - m_ptSpriteMapSize.x;
//	m_ptCutSize.y = m_ptRealMinimapSize.y - m_ptSpriteMapSize.y;
	m_ptCutSize = l_ptCutSize[pi_byMiniMapIndex];
	

	// 맵인덱스가 같으면 object를 뿌려준다.
	if( m_byMapIndex == m_byMiniMapIndex )
	{	
		m_uiObjectBoard.SetPos( m_uiMiniMap.m_ptPos );
		m_uiObjectBoard.Show( IsShow() );		
		Add( &m_uiObjectBoard );

		m_uiAvatarObject.Show( IsShow() );
		Add( &m_uiAvatarObject );
		
		// update avatar pos
		SetAvatarPosition( m_ptAvatarPos.x, m_ptAvatarPos.y );


		m_uiAvatarPosition.SetPos( m_ptPos.x + 16, m_ptPos.y + 6 );	
		m_uiAvatarPosition.Show( IsShow() );
		Add( &m_uiAvatarPosition );	

		// tooltip event가 일어나기 위해 최상위 layer에 추가한다.
		m_uiTooltipObjBoard.SetPos( m_uiMiniMap.m_ptPos );
		m_uiTooltipObjBoard.Show( IsShow() );
		Add( &m_uiTooltipObjBoard );
	}
	else
	{
		Remove( &m_uiObjectBoard );
		Remove( &m_uiAvatarObject );
		
		Remove( &m_uiAvatarPosition );	

		Remove( &m_uiTooltipObjBoard );
	}
}

void
CGUIMiniMapWindow::SetMapBound( RECT & pi_rtBound )
{
	memcpy( &m_rtMapBound, &pi_rtBound, sizeof( RECT ) );	

	SetPixelPerMeter();
}

void
CGUIMiniMapWindow::SetMapRotate( int pi_nAngle )
{
// 현재 map rotate 적용안됨
/*
	m_nMapRotate = pi_nAngle;

	SetPixelPerMeter();

	if( g_pUiRadarWindow )
		g_pUiRadarWindow->SetMapRotate( pi_nAngle );
*/
}

void
CGUIMiniMapWindow::SetPixelPerMeter( void )
{
	if( m_rtMapBound.right - m_rtMapBound.left > 0 && 
		m_rtMapBound.bottom - m_rtMapBound.top > 0 )
	{		
		/*
		if( m_nMapRotate == 270 ||
			m_nMapRotate == -90 )
		{
			// x, z가 바뀐다.
			m_fPixelPerMeter[0] = (float)m_ptRealMinimapSize.x / ( m_rtMapBound.bottom - m_rtMapBound.top );
			m_fPixelPerMeter[1] = (float)m_ptRealMinimapSize.y / ( m_rtMapBound.right - m_rtMapBound.left );			
		}
		else
		{
			m_fPixelPerMeter[0] = (float)m_ptRealMinimapSize.x / ( m_rtMapBound.right - m_rtMapBound.left );
			m_fPixelPerMeter[1] = (float)m_ptRealMinimapSize.y / ( m_rtMapBound.bottom - m_rtMapBound.top );			
		}
		*/
		m_fPixelPerMeter[0] = (float)m_ptRealMinimapSize.x / ( m_rtMapBound.right - m_rtMapBound.left );
		m_fPixelPerMeter[1] = (float)m_ptRealMinimapSize.y / ( m_rtMapBound.bottom - m_rtMapBound.top );
	}	
}	


void
CGUIMiniMapWindow::SetAvatarPosition( int pi_nPosX, int pi_nPosZ )
{
	m_ptAvatarPos.x = pi_nPosX;
	m_ptAvatarPos.y = pi_nPosZ;

	char buf[16];
	sprintf( buf, "%5d %5d", pi_nPosX, pi_nPosZ );

	m_uiAvatarPosition.SetString( buf );	

	POINT l_ptNewPos;
	/*
	if( m_nMapRotate == 180 )
	{
		l_ptNewPos.x = m_uiMiniMap.m_ptPos.x + (m_ptRealMinimapSize.x - (m_ptAvatarPos.x - m_rtMapBound.left) * m_fPixelPerMeter[0]) - m_uiAvatarObject.m_ptSize.x/2 - m_ptCutSize.x;
		l_ptNewPos.y = m_uiMiniMap.m_ptPos.y + (m_ptRealMinimapSize.y - (m_ptAvatarPos.y - m_rtMapBound.top) * m_fPixelPerMeter[1]) - m_uiAvatarObject.m_ptSize.y/2 - m_ptCutSize.y;
	}	
	else if( m_nMapRotate == 270 || m_nMapRotate == -90 )
	{
		l_ptNewPos.x = m_uiMiniMap.m_ptPos.x + (m_ptRealMinimapSize.x - (m_ptAvatarPos.y - m_rtMapBound.top) * m_fPixelPerMeter[0]) - m_uiAvatarObject.m_ptSize.x/2 - m_ptCutSize.x;
		l_ptNewPos.y = m_uiMiniMap.m_ptPos.y + (m_ptRealMinimapSize.y - (m_ptAvatarPos.x - m_rtMapBound.left) * m_fPixelPerMeter[1]) - m_uiAvatarObject.m_ptSize.y/2 - m_ptCutSize.y;
	}
	else
	{
		l_ptNewPos.x = m_uiMiniMap.m_ptPos.x + (m_ptAvatarPos.x - m_rtMapBound.left) * m_fPixelPerMeter[0]
							- m_uiAvatarObject.m_ptSize.x/2 - m_ptCutSize.x;
		l_ptNewPos.y = m_uiMiniMap.m_ptPos.y + (m_ptRealMinimapSize.y - (m_ptAvatarPos.y - m_rtMapBound.top) * m_fPixelPerMeter[1])
							- m_uiAvatarObject.m_ptSize.y/2 - m_ptCutSize.y;
	}
	*/
	l_ptNewPos.x = m_uiMiniMap.m_ptPos.x + (m_ptAvatarPos.x - m_rtMapBound.left) * m_fPixelPerMeter[0]
						- m_uiAvatarObject.m_ptSize.x/2 - m_ptCutSize.x;
	l_ptNewPos.y = m_uiMiniMap.m_ptPos.y + (m_ptRealMinimapSize.y - (m_ptAvatarPos.y - m_rtMapBound.top) * m_fPixelPerMeter[1])
						- m_uiAvatarObject.m_ptSize.y/2 - m_ptCutSize.y;

	m_uiAvatarObject.SetPos( l_ptNewPos );	
}

void
CGUIMiniMapWindow::SetAvatarRotate( float pi_fAngle )
{
//	m_uiAvatarObject.SetRotate( pi_fAngle + m_nMapRotate );
}

void
CGUIMiniMapWindow::AddMiniMapObject( MiniMapObjType pi_eObjType, DWORD pi_dwID, int pi_nPosX, int pi_nPosZ )									 
{
	// 중복 체크
	MINIMAP_OBJ_INFO * l_pObjInfo = ExistMiniMapObject( pi_dwID );
	if( l_pObjInfo )
	{
		if( l_pObjInfo->byObjType != pi_eObjType )
		{
			l_pObjInfo->byObjType	=	pi_eObjType;
			SetMinimapObjectSprite( l_pObjInfo );
		}		

		l_pObjInfo->ptObjPos.x	=	pi_nPosX;
		l_pObjInfo->ptObjPos.y	=	pi_nPosZ;

		UpdateObjectPosition( l_pObjInfo );	

		return;	
	}

	// 새 오브젝트
	if( pi_eObjType < PartyLeader ||
		pi_eObjType > Gate )
		return;

	l_pObjInfo = new MINIMAP_OBJ_INFO;

	l_pObjInfo->byObjType	=	pi_eObjType;
	l_pObjInfo->dwID		=	pi_dwID;
	l_pObjInfo->ptObjPos.x	=	pi_nPosX;
	l_pObjInfo->ptObjPos.y	=	pi_nPosZ;	

	// set object sprite
	SetMinimapObjectSprite( l_pObjInfo );	
	

	// add object
	m_listMiniMapObject.push_back( l_pObjInfo );

	// udpate positoin
	UpdateObjectPosition( l_pObjInfo );	
}

MINIMAP_OBJ_INFO *
CGUIMiniMapWindow::ExistMiniMapObject( DWORD pi_dwID )
{
	CGUIMiniMapObjList::iterator it;
	for( it=m_listMiniMapObject.begin(); it!=m_listMiniMapObject.end(); ++it )
	{
		if( (*it)->dwID == pi_dwID )
			return (*it);
	}

	return NULL;
}

void
CGUIMiniMapWindow::RemoveMiniMapObject( DWORD pi_dwID )
{
	CGUIMiniMapObjList::iterator it;
	for( it=m_listMiniMapObject.begin(); it!=m_listMiniMapObject.end(); ++it )
	{
		if( (*it)->dwID == pi_dwID )
		{
			MINIMAP_OBJ_INFO * l_pObjInfo;
			l_pObjInfo = *it;
			
			m_listMiniMapObject.erase( it );

			// minimap에 보이면 삭제
			m_uiObjectBoard.Remove( &l_pObjInfo->uiObject );

			delete l_pObjInfo;
				
			return;
		}
	}
}

void
CGUIMiniMapWindow::RemoveAllMiniMapObject( void )
{
	CGUIMiniMapObjList::iterator it;
	for( it=m_listMiniMapObject.begin(); it!=m_listMiniMapObject.end(); ++it )
	{
		delete *it;
	}

	m_listMiniMapObject.clear();	

	// minimap ui clear
	m_uiObjectBoard.RemoveAll();
}

void
CGUIMiniMapWindow::UpdateMiniMapObject( DWORD pi_dwID, int pi_nPosX, int pi_nPosZ )
{
	CGUIMiniMapObjList::iterator it;
	for( it=m_listMiniMapObject.begin(); it!=m_listMiniMapObject.end(); ++it )
	{
		if( (*it)->dwID == pi_dwID )
		{
			(*it)->ptObjPos.x = pi_nPosX;
			(*it)->ptObjPos.y = pi_nPosZ;

			UpdateObjectPosition( *it );

			return;
		}
	}
}

void
CGUIMiniMapWindow::AddTooltipObject( DWORD pi_dwID, int pi_nPosX, int pi_nPosY, int pi_nSizeX, int pi_nSizeY, char * pi_pTooltipStr )
{
	// 중복 체크
	if( ExistTooltipObject( pi_dwID ) )
		return;	

	MINIMAP_TOOLTIP_OBJ_INFO *	l_pTooltipObjInfo;

	l_pTooltipObjInfo = new MINIMAP_TOOLTIP_OBJ_INFO;
	
	l_pTooltipObjInfo->dwID		=	pi_dwID;	
	l_pTooltipObjInfo->uiObject.SetPos( m_uiTooltipObjBoard.m_ptPos.x + pi_nPosX, m_uiTooltipObjBoard.m_ptPos.y + pi_nPosY );
	l_pTooltipObjInfo->uiObject.SetSize( pi_nSizeX, pi_nSizeY );
	if( pi_pTooltipStr )
	{
		strncpy( l_pTooltipObjInfo->pTooltipStr, pi_pTooltipStr, MINIMAP_TOOLTIP_STR_LEN - 1 );
		l_pTooltipObjInfo->pTooltipStr[MINIMAP_TOOLTIP_STR_LEN - 1] = '\0';
	}
	else
	{
		l_pTooltipObjInfo->pTooltipStr[0] = '\0';
	}
	
	
	l_pTooltipObjInfo->uiObject.AddMouseEventHandler( this );
	

	// add object
	// 면적이 큰 순으로 삽입
	CGUIMiniMapTooltipList::iterator it;
	CGUIObject * l_pObj;
	for( it=m_listTooltipList.begin(); it!=m_listTooltipList.end(); ++it )
	{
		l_pObj = &((*it)->uiObject);
		if( l_pObj->m_ptSize.x * l_pObj->m_ptSize.y < pi_nSizeX * pi_nSizeY )
			break;
	}

	m_listTooltipList.insert( it, l_pTooltipObjInfo );


	l_pTooltipObjInfo->uiObject.Show( IsShow() );
	m_uiTooltipObjBoard.Add( &l_pTooltipObjInfo->uiObject );	

	// test
//	l_pTooltipObjInfo->uiObject.SetARGB( 0x90F00000 );
}

BOOL
CGUIMiniMapWindow::ExistTooltipObject( DWORD pi_dwID )
{
	CGUIMiniMapTooltipList::iterator it;
	for( it=m_listTooltipList.begin(); it!=m_listTooltipList.end(); ++it )
	{
		if( (*it)->dwID == pi_dwID )
			return TRUE;
	}

	return FALSE;
}

void
CGUIMiniMapWindow::RemoveTooltipObject( DWORD pi_dwID )
{
	CGUIMiniMapTooltipList::iterator it;
	for( it=m_listTooltipList.begin(); it!=m_listTooltipList.end(); ++it )
	{
		if( (*it)->dwID == pi_dwID )
		{
			MINIMAP_TOOLTIP_OBJ_INFO * l_pObjInfo;
			l_pObjInfo = *it;
			
			m_listTooltipList.erase( it );

			m_uiTooltipObjBoard.Remove( &l_pObjInfo->uiObject );			

			delete l_pObjInfo;				
			
			return;
		}		
	}
}

void
CGUIMiniMapWindow::RemoveAllTooltipObject( void )
{
	CGUIMiniMapTooltipList::iterator it;
	for( it=m_listTooltipList.begin(); it!=m_listTooltipList.end(); ++it )
	{
		delete *it;
	}

	m_listTooltipList.clear();	

	// minimap ui clear
	m_uiTooltipObjBoard.RemoveAll();
}

void
CGUIMiniMapWindow::UpdateObjectPosition( MINIMAP_OBJ_INFO * pi_pObjInfo )
{
	POINT l_ptPos;		
	
	l_ptPos.x = m_uiMiniMap.m_ptPos.x + 
				( pi_pObjInfo->ptObjPos.x - m_rtMapBound.left ) * m_fPixelPerMeter[0] - pi_pObjInfo->uiObject.m_ptSize.x/2 - m_ptCutSize.x;
	
	// z좌표가 뒤집혔다.	
	l_ptPos.y = m_uiMiniMap.m_ptPos.y +				
				( m_rtMapBound.bottom - pi_pObjInfo->ptObjPos.y ) * m_fPixelPerMeter[1] - pi_pObjInfo->uiObject.m_ptSize.y/2 - m_ptCutSize.y;

	pi_pObjInfo->uiObject.SetPos( l_ptPos );


	// ----------------------------------------------------------------------------------

	// minimap에 보이는 객체이면 m_uiObjectBoard에 추가하고 아니면 삭제한다
	if( pi_pObjInfo->uiObject.m_ptPos.x > m_uiObjectBoard.m_ptPos.x &&
		pi_pObjInfo->uiObject.m_ptPos.x + pi_pObjInfo->uiObject.m_ptSize.x < m_uiObjectBoard.m_ptPos.x + m_uiObjectBoard.m_ptSize.x &&
		pi_pObjInfo->uiObject.m_ptPos.y > m_uiObjectBoard.m_ptPos.y &&
		pi_pObjInfo->uiObject.m_ptPos.y + pi_pObjInfo->uiObject.m_ptSize.y < m_uiObjectBoard.m_ptPos.y + m_uiObjectBoard.m_ptSize.y )
	{
		// add object to objectboard
		pi_pObjInfo->uiObject.Show( IsShow() );
		m_uiObjectBoard.Add( &pi_pObjInfo->uiObject );		
	}
	else
	{	
		m_uiObjectBoard.Remove( &pi_pObjInfo->uiObject );
	}	
}

void
CGUIMiniMapWindow::SetMinimapObjectSprite( MINIMAP_OBJ_INFO * po_pObjInfo )
{
	SPRITE_INFO	l_spriteInfo;	
	
	if( po_pObjInfo->byObjType == PartyLeader )	
	{
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		l_spriteInfo.wActionNo = 10;
		l_spriteInfo.wFrameNo = 7;
		po_pObjInfo->uiObject.SetSpriteInfo( &l_spriteInfo );

		if( l_spriteInfo.pSprite )
		{
			po_pObjInfo->uiObject.SetSize( l_spriteInfo.pSprite->GetSpriteOrgXL( l_spriteInfo.wActionNo, l_spriteInfo.wFrameNo ),
										  l_spriteInfo.pSprite->GetSpriteOrgYL( l_spriteInfo.wActionNo, l_spriteInfo.wFrameNo ) );
		}
		else
		{
			po_pObjInfo->uiObject.SetSize( 8, 8 );
		}
	}
	else if( po_pObjInfo->byObjType == PartyMember )	
	{
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		l_spriteInfo.wActionNo = 10;
		l_spriteInfo.wFrameNo = 8;
		po_pObjInfo->uiObject.SetSpriteInfo( &l_spriteInfo );

		if( l_spriteInfo.pSprite )
		{
			po_pObjInfo->uiObject.SetSize( l_spriteInfo.pSprite->GetSpriteOrgXL( l_spriteInfo.wActionNo, l_spriteInfo.wFrameNo ),
										  l_spriteInfo.pSprite->GetSpriteOrgYL( l_spriteInfo.wActionNo, l_spriteInfo.wFrameNo ) );
		}
		else
		{
			po_pObjInfo->uiObject.SetSize( 8, 8 );
		}
	}		
	// else 포탈 이미지는 미니맵에 들어가 있어서 안쓴다.		
}

void
CGUIMiniMapWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiCloseButton )
		{
			Close();
		}
	}
	else if( event.id == MOUSE_HOVERED )
	{
		CGUIMiniMapTooltipList::iterator it;
		for( it=m_listTooltipList.begin(); it!=m_listTooltipList.end(); ++it )
		{
			// set tooltip
			if( event.source == &(*it)->uiObject )
			{	
				if( (*it)->pTooltipStr[0] != '\0' )
					SetTooltip( &((*it)->uiObject), (*it)->pTooltipStr );

				return;
			}				
		}
	}	
}

BOOL
CGUIMiniMapWindow::Draw( void )
{
	if( !IsShow() )
		return FALSE;

	if( m_byMiniMapIndex == 0xFF )
		return FALSE;

	CGUIContainer::Draw();

	// draw outline
	// left
	Draw2DRectangle( m_ptPos.x, m_ptPos.y + m_uiTitleBoard.m_ptSize.y, 
					 m_ptPos.x + 1, m_ptPos.y + m_ptSize.y, 0xCCE8E8E8 );
	// right
	Draw2DRectangle( m_ptPos.x + m_ptSize.x - 1, m_ptPos.y + m_uiTitleBoard.m_ptSize.y,
					 m_ptPos.x + m_ptSize.x, m_ptPos.y + m_ptSize.y, 0xCCE8E8E8 );
	// bottom
	Draw2DRectangle( m_ptPos.x, m_ptPos.y + m_ptSize.y - 1,
					 m_ptPos.x + m_ptSize.x, m_ptPos.y + m_ptSize.y, 0xCCE8E8E8 );	

	DrawDialogBox();
	return TRUE;
}