////////////////////////////////////////////////////////////////////////////
//
// CGUIOptionWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIRadarWindow.h"
#include "../../SpriteMgr.h"
#include "../../GUIController/GUIScreen.h"
#include "R3Util.h"

#define	MAX_ZOOM_OUT_RATE	1.75

CGUIRadarWindow::CGUIRadarWindow()
{
	m_fMapRotate		= 0.f;			

	m_vecAvatarPos[0]	= 0.f;
	m_vecAvatarPos[1]	= 0.f;
	m_vecAvatarPos[2]	= 0.f;

	m_fCameraRotate		= 0.f;	
	m_fRotateSinTheta	= 0.f;
	m_fRotateCosTheta	= 0.f;

	m_fYAxisViewRange	= 30000;

	m_bCreated			= FALSE;
}

CGUIRadarWindow::~CGUIRadarWindow()
{
	for( CGUIRadarCharList::iterator it=m_listChar.begin(); it!=m_listChar.end(); ++it )
	{
		delete *it;
	}	
}

void	
CGUIRadarWindow::Init( void )
{
	
	SetSize( 159, 158 );
	m_uiRadarBoard.SetSize( 159, 158 );
	m_uiCompassBase.SetSize( 159, 158 );	
	
	m_uiAvatarObject.SetSize( 158, 158 );	
	
	//--------------------------------------------------------
	m_uiRadarBoard.SetPos( m_ptPos );
	m_uiCompassBase.SetPos( m_ptPos );

	m_uiAvatarObject.SetPos( m_uiRadarBoard.m_ptPos.x + (m_uiRadarBoard.m_ptSize.x - m_uiAvatarObject.m_ptSize.x) / 2,
							 m_uiRadarBoard.m_ptPos.y + (m_uiRadarBoard.m_ptSize.y - m_uiAvatarObject.m_ptSize.y) / 2 );
	

	//--------------------------------------------------------	
	Add( &m_uiCompassBase );	
	Add( &m_uiAvatarObject );
	Add( &m_uiRadarBoard );

	//--------------------------------------------------------

	SetSprite();
	
	m_uiCompassBase.SetARGB( 0xc0ffffff );

	m_uiAvatarObject.SetARGB( 0x88ffffff );

	//--------------------------------------------------------

	SetMoveObject( this );	
	SetMoveObject( &m_uiRadarBoard );	
	SetMovable( TRUE );	

	m_ptCharObjSize.x = 4;
	m_ptCharObjSize.y = 4;
	

	m_bCreated = TRUE;
}

void
CGUIRadarWindow::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );

		l_spriteInfo.wActionNo = 8;		

		l_spriteInfo.wFrameNo = 0;	
		m_uiCompassBase.SetSpriteInfo( &l_spriteInfo );

		float uv[2][2];
		uv[0][0] = 0.f;
		uv[0][1] = (float)m_uiCompassBase.m_ptSize.x / l_spriteInfo.pSprite->GetSpriteXL( 8, 0 );
		uv[1][0] = 0.f;
		uv[1][1] = (float)m_uiCompassBase.m_ptSize.y / l_spriteInfo.pSprite->GetSpriteYL( 8, 0 );
		m_uiCompassBase.SetSpriteUV( uv );
		
		l_spriteInfo.wActionNo = 9;				
		l_spriteInfo.wFrameNo = 1;
		m_uiAvatarObject.SetSpriteInfo( &l_spriteInfo );

		m_uiAvatarObject.SetSpriteU( 0, 1.f );
		m_uiAvatarObject.SetSpriteV( 0, 1.f );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		if( !l_pSprite )
			return;	
		
		m_uiCompassBase.GetSpriteInfo()->pSprite = l_pSprite;

		m_uiAvatarObject.GetSpriteInfo()->pSprite = l_pSprite;

		// character obj 
		for( CGUIRadarCharList::iterator it=m_listChar.begin(); it!=m_listChar.end(); ++it )
		{		
			if( (*it)->uiCharObj.GetSpriteInfo()->pSprite != NULL )
				(*it)->uiCharObj.GetSpriteInfo()->pSprite = l_pSprite;

			if( (*it)->uiCharDir.GetSpriteInfo()->pSprite != NULL )
				(*it)->uiCharDir.GetSpriteInfo()->pSprite = l_pSprite;
		}		
	}
}

void
CGUIRadarWindow::SetMapSize( POINT pi_ptSize )
{
	m_fPixelPerMeter[0] = (float)m_uiRadarBoard.m_ptSize.x / pi_ptSize.x;
	m_fPixelPerMeter[1] = (float)m_uiRadarBoard.m_ptSize.y / pi_ptSize.y;
}

void
CGUIRadarWindow::SetMapRotate( float pi_fAngle )
{
	m_fMapRotate = pi_fAngle;
}

void
CGUIRadarWindow::SetYAxisViewRange( float pi_fViewRange )
{
	m_fYAxisViewRange = pi_fViewRange;

	// update radar
	for( CGUIRadarCharList::iterator it=m_listChar.begin(); it!=m_listChar.end(); ++it )
	{
		if( fabs( m_vecAvatarPos[1] - (*it)->vecCharPos[1] ) > m_fYAxisViewRange )		
		{
			m_uiRadarBoard.Remove( &((*it)->uiCharObj) );			
		}
	}
}

void
CGUIRadarWindow::SetAvatarPosition( float pi_fPosX, float pi_fPosY, float pi_fPosZ )
{
	m_vecAvatarPos[0] = pi_fPosX;
	m_vecAvatarPos[1] = pi_fPosY;
	m_vecAvatarPos[2] = pi_fPosZ;
	
	// 아바타 위치가 바뀌면 레이다상에 보여지는 주변 캐릭터의 상대적 위치도 바뀐다.	
	for( CGUIRadarCharList::iterator it=m_listChar.begin(); it!=m_listChar.end(); ++it )
	{		
		UpdateCharPosition( *it );
	}
}

void
CGUIRadarWindow::SetCameraRotate( float pi_fAngle )
{		
	m_fCameraRotate = m_fMapRotate + pi_fAngle;

	// rotate compass
	m_uiCompassBase.SetRotate( m_fCameraRotate );
	
	m_fRotateSinTheta = sin( D3DXToRadian( m_fCameraRotate ) );
	m_fRotateCosTheta = cos( D3DXToRadian( m_fCameraRotate ) );	

	// 아바타 위치가 바뀌면 레이다상에 보여지는 주변 캐릭터의 상대적 위치도 바뀐다.	
	for( CGUIRadarCharList::iterator it=m_listChar.begin(); it!=m_listChar.end(); ++it )
	{		
		UpdateCharPosition( *it );
	}		
}

void
CGUIRadarWindow::SetAvatarRotate( float pi_fAngle )
{
	m_uiAvatarObject.SetRotate( pi_fAngle + m_fCameraRotate );
}

BOOL
CGUIRadarWindow::IsExistChar( DWORD pi_dwID )
{
	for( CGUIRadarCharList::iterator it=m_listChar.begin(); it!=m_listChar.end(); ++it )
	{
		if( ( *it )->dwCharID == pi_dwID )
		{
			return TRUE;
		}
	}

	return FALSE;
}

RADAR_CHAR_INFO	* 
CGUIRadarWindow::GetCharInfo( DWORD pi_dwID )
{
	for( CGUIRadarCharList::iterator it=m_listChar.begin(); it!=m_listChar.end(); ++it )
	{
		if( ( *it )->dwCharID == pi_dwID )
		{
			return *it;
		}
	}

	return NULL;
}

void
CGUIRadarWindow::AddCharacter( BYTE	pi_byCharType, DWORD pi_dwID,
							   CharType pi_eCharType,
							   float pi_fPosX, float pi_fPosY, float pi_fPosZ,
							   BOOL pi_bDrawCharDir )
{	

	pi_dwID |= pi_byCharType << 28;	

/*
	if( IsExistChar( pi_dwID ) )
	{
		UpdateCharacterType( pi_byCharType, pi_dwID, pi_eCharType );
		UpdateCharacterDirDrawFlag( pi_byCharType, pi_dwID, pi_bDrawCharDir );
		UpdateCharacterPos( pi_byCharType, pi_dwID, pi_fPosX, pi_fPosY, pi_fPosZ );
		return;
	}
*/
	
	RADAR_CHAR_INFO * l_pCharInfo;

	// check if character exist
	l_pCharInfo = GetCharInfo( pi_dwID );	

	if( l_pCharInfo )
	{
		// update character info

		if( l_pCharInfo->byType != pi_eCharType )
			UpdateCharacterType( pi_byCharType, pi_dwID, pi_eCharType );		
	}	
	else
	{
		// add new character
		
		l_pCharInfo = new RADAR_CHAR_INFO;

		l_pCharInfo->dwCharID = pi_dwID;		
	
		l_pCharInfo->byType = pi_eCharType;	

		l_pCharInfo->uiCharObj.SetSize( m_ptCharObjSize );
		l_pCharInfo->uiCharDir.SetSize( 11, 14 );

		// set char sprite
		SetSprite_CharObj( pi_eCharType, &l_pCharInfo->uiCharObj );

		SetSprite_CharDir( pi_eCharType, &l_pCharInfo->uiCharDir );	

		// add char list
		m_listChar.push_back( l_pCharInfo );		
	}


	// set char position
	l_pCharInfo->vecCharPos[0] = pi_fPosX;
	l_pCharInfo->vecCharPos[1] = pi_fPosY;
	l_pCharInfo->vecCharPos[2] = pi_fPosZ;

	l_pCharInfo->bDrawCharDir = pi_bDrawCharDir;		
	
	UpdateCharPosition( l_pCharInfo );
}

void
CGUIRadarWindow::RemoveCharacter( BYTE pi_byCharType, DWORD pi_dwID )
{
	pi_dwID |= pi_byCharType << 28;

	for( CGUIRadarCharList::iterator it=m_listChar.begin(); it!=m_listChar.end(); ++it )
	{
		if( (*it)->dwCharID == pi_dwID )
		{
			RADAR_CHAR_INFO * l_pCharInfo;
			l_pCharInfo = *it;			

			// 레이다에 캐릭터가 있으면 삭제
			m_uiRadarBoard.Remove( &l_pCharInfo->uiCharObj );

			if( l_pCharInfo->bDrawCharDir )
				Remove( &l_pCharInfo->uiCharDir );

			// remove character to list
			m_listChar.erase( it );

			delete l_pCharInfo;			
			
			return;
		}
	}
}

void
CGUIRadarWindow::RemoveAllCharacter( void )
{	
	m_uiRadarBoard.RemoveAll();

	for( CGUIRadarCharList::iterator it=m_listChar.begin(); it!=m_listChar.end(); ++it )
	{
		if( (*it)->bDrawCharDir )
		{
			Remove( &(*it)->uiCharDir );
		}

		delete *it;
	}
	
	m_listChar.clear();
}

void
CGUIRadarWindow::UpdateCharacterPos( BYTE pi_byCharType, DWORD pi_dwID, float pi_fPosX, float pi_fPosY, float pi_fPosZ )
{
	pi_dwID |= pi_byCharType << 28;
	
	for( CGUIRadarCharList::iterator it=m_listChar.begin(); it!=m_listChar.end(); ++it )
	{
		if( (*it)->dwCharID == pi_dwID )
		{
			(*it)->vecCharPos[0] = pi_fPosX;
			(*it)->vecCharPos[1] = pi_fPosY;
			(*it)->vecCharPos[2] = pi_fPosZ;

			UpdateCharPosition( *it );

			return;
		}
	}
}

void
CGUIRadarWindow::UpdateCharacterType( BYTE pi_byCharType, DWORD pi_dwID, CharType pi_eNewType )
{
	pi_dwID |= pi_byCharType << 28;

	for( CGUIRadarCharList::iterator it=m_listChar.begin(); it!=m_listChar.end(); ++it )
	{
		if( (*it)->dwCharID == pi_dwID )
		{	
			RADAR_CHAR_INFO * l_pCharInfo;

			l_pCharInfo = *it;

			// update char type
			l_pCharInfo->byType = pi_eNewType;			

			SetSprite_CharObj( (CharType)pi_eNewType, &(l_pCharInfo->uiCharObj) );

			SetSprite_CharDir( (CharType)pi_eNewType, &(l_pCharInfo->uiCharDir) );

			return;
		}
	}	
}

void
CGUIRadarWindow::UpdateCharacterDirDrawFlag( BYTE pi_byCharType, DWORD pi_dwID, BOOL pi_bDrawDir )
{
	pi_dwID |= pi_byCharType << 28;

	for( CGUIRadarCharList::iterator it=m_listChar.begin(); it!=m_listChar.end(); ++it )
	{
		if( (*it)->dwCharID == pi_dwID )
		{	
			RADAR_CHAR_INFO * l_pCharInfo;

			l_pCharInfo = *it;

			BOOL l_bPrevDrawDir = l_pCharInfo->bDrawCharDir;

			// update dir draw flag
			l_pCharInfo->bDrawCharDir = pi_bDrawDir;			

			if( l_bPrevDrawDir && !l_pCharInfo->bDrawCharDir )
			{
				Remove( &l_pCharInfo->uiCharDir );				
			}
			else if( !l_bPrevDrawDir && l_pCharInfo->bDrawCharDir )
			{
				// 방향을 표시해주기 위해 update한다.
				UpdateCharPosition( l_pCharInfo );		
			}			

			return;
		}
	}	
}

void
CGUIRadarWindow::UpdateCharPosition( RADAR_CHAR_INFO * pi_pCharInfo )
{	
	float l_ptInitPos[2];
	l_ptInitPos[0] = ( pi_pCharInfo->vecCharPos[0] - m_vecAvatarPos[0] ) * m_fPixelPerMeter[0];
	l_ptInitPos[1] = -( pi_pCharInfo->vecCharPos[2] - m_vecAvatarPos[2] ) * m_fPixelPerMeter[1]; // z좌표는 뒤집혔다

	POINT l_ptPos;
	l_ptPos.x = m_uiAvatarObject.m_ptPos.x + ( m_uiAvatarObject.m_ptSize.x - m_ptCharObjSize.x )/2 +
				m_fRotateCosTheta * l_ptInitPos[0] - m_fRotateSinTheta * l_ptInitPos[1];
	l_ptPos.y = m_uiAvatarObject.m_ptPos.y + ( m_uiAvatarObject.m_ptSize.y - m_ptCharObjSize.y )/2 +
				m_fRotateSinTheta * l_ptInitPos[0] + m_fRotateCosTheta * l_ptInitPos[1];

	pi_pCharInfo->uiCharObj.SetPos( l_ptPos );

	SetViewedCharInRadarBoard( pi_pCharInfo );
}

void
CGUIRadarWindow::SetViewedCharInRadarBoard( RADAR_CHAR_INFO * pi_pCharInfo )
{
	POINT l_ptOrgPos;
	POINT l_ptCharPos;

	l_ptOrgPos.x = m_uiRadarBoard.m_ptPos.x + m_uiRadarBoard.m_ptSize.x / 2;
	l_ptOrgPos.y = m_uiRadarBoard.m_ptPos.y + m_uiRadarBoard.m_ptSize.y / 2;

	l_ptCharPos.x = pi_pCharInfo->uiCharObj.m_ptPos.x + pi_pCharInfo->uiCharObj.m_ptSize.x / 2;
	l_ptCharPos.y = pi_pCharInfo->uiCharObj.m_ptPos.y + pi_pCharInfo->uiCharObj.m_ptSize.y / 2;

	// 나침반 원 안에 object가 들어가는지 검사해서 들어가면 보여준다.	
	if( fabs( m_vecAvatarPos[1] - pi_pCharInfo->vecCharPos[1] ) <= m_fYAxisViewRange && // 높이 체크
		pow( l_ptCharPos.x - l_ptOrgPos.x, 2 ) + pow( l_ptCharPos.y - l_ptOrgPos.y, 2 ) <= 4356 ) // 66 * 66 : 나침반 반지름 
	{
		pi_pCharInfo->uiCharObj.Show( IsShow() );
		m_uiRadarBoard.Add( &pi_pCharInfo->uiCharObj );		

		if( pi_pCharInfo->bDrawCharDir )
		{
			Remove( &pi_pCharInfo->uiCharDir );
		}
	}
	else
	{
		// 방향표시를 하는 경우
		if( pi_pCharInfo->bDrawCharDir )
		{
			Vector3f orig_vec = { 0, 0, -1 };
			Vector3f tar_vec = { l_ptCharPos.x - l_ptOrgPos.x, 0, l_ptCharPos.y - l_ptOrgPos.y };						
			Normalize( tar_vec );

			float l_fDot = Vector3fDot(orig_vec,tar_vec);
			float l_fLen = sqrt(tar_vec[0]*tar_vec[0]+tar_vec[1]*tar_vec[1]+tar_vec[2]*tar_vec[2]);

			float l_fRotate = (float)acos( l_fDot/l_fLen );

			Vector3f dir_vec;
			Vector3fCross( orig_vec, tar_vec, dir_vec );
			if( dir_vec[1] > 0 )
			{
				l_fRotate = -l_fRotate;
			}			

			float l_fNewRotate = PiToAngle( l_fRotate );

			if( !IsAddedObject( &pi_pCharInfo->uiCharDir ) || // -> 방향표시가 처음인지 본다.
				fabs( pi_pCharInfo->uiCharDir.GetRotate() - l_fNewRotate ) >= 0.5f )
			{			
				pi_pCharInfo->uiCharDir.SetRotate( l_fNewRotate );

				pi_pCharInfo->uiCharDir.SetPos( l_ptOrgPos.x + tar_vec[0] * (m_uiCompassBase.m_ptSize.x/2 - 19) - pi_pCharInfo->uiCharDir.m_ptSize.x/2,
												l_ptOrgPos.y + tar_vec[2] * (m_uiCompassBase.m_ptSize.y/2 - 19) - pi_pCharInfo->uiCharDir.m_ptSize.y/2 );			
			}

			pi_pCharInfo->uiCharDir.Show( IsShow() );
			Add( &pi_pCharInfo->uiCharDir );
		}
		

		m_uiRadarBoard.Remove( &pi_pCharInfo->uiCharObj );		
	}
}

void
CGUIRadarWindow::SetSprite_CharObj( CharType pi_eCharType, CGUIObject * po_pCharObj )
{
	static DWORD s_dwCharObjColor[] = { D3DCOLOR_XRGB( 65, 187, 74 ),	// same race
										UI_PARTY_LEADER_COLOR,
										UI_PARTY_MEMBER_COLOR,
										D3DCOLOR_XRGB( 134, 151, 66 ),
										D3DCOLOR_XRGB( 190, 214, 94 ),	
										
										D3DCOLOR_XRGB( 255, 0, 0 ),		// other race
										D3DCOLOR_XRGB( 255, 0, 0 ),	
										D3DCOLOR_XRGB( 255, 0, 0 ),

										D3DCOLOR_XRGB( 232, 232, 232 ),	// npc
										D3DCOLOR_XRGB( 232, 232, 232 ),
										D3DCOLOR_XRGB( 232, 232, 232 ),
										
										D3DCOLOR_XRGB( 255, 214, 94 ),	// mob
										D3DCOLOR_XRGB( 230, 128, 255 ),
										D3DCOLOR_XRGB( 168, 65, 255 ),

										D3DCOLOR_XRGB( 255, 255, 255 )	// stealth - 객체를 안보이게 한다.
									};

	if( pi_eCharType < TOTAL_CHAR_TYPE_NUM )
		po_pCharObj->SetARGB( s_dwCharObjColor[pi_eCharType] );
	else
		po_pCharObj->SetARGB( s_dwCharObjColor[0] );
}

void
CGUIRadarWindow::SetSprite_CharDir( CharType pi_eCharType, CGUIObjectEx * po_pCharDir )
{
	if( pi_eCharType == Stealth )
		return;
	
	po_pCharDir->GetSpriteInfo()->pSprite = m_uiCompassBase.GetSpriteInfo()->pSprite;

	po_pCharDir->GetSpriteInfo()->wActionNo = 9;
	
	if( pi_eCharType == PartyLeader )
	{		
		po_pCharDir->GetSpriteInfo()->wFrameNo = 2;
	}
	else if( pi_eCharType == PartyMember )
	{	
		po_pCharDir->GetSpriteInfo()->wFrameNo = 3;
	}
	else
	{
		po_pCharDir->GetSpriteInfo()->wFrameNo = 2;
	}

	po_pCharDir->SetSpriteU( 0, 1.f );
	po_pCharDir->SetSpriteV( 0, 1.f );
}