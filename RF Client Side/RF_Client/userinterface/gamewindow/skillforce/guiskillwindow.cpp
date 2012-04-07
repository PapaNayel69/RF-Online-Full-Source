#include "GUISkillWindow.h"
#include "../../GUIController/GUIScreen.h"
#include "../../SpriteMgr.h"
#include "../../../DefineMessage.h"

#define	TITLE_COLOR			0xFFC3C1BE
#define	SUBTITLE_COLOR		0xFFDAD8D4
#define	INFO_COLOR			0xFFD4BA94

#define	HOVERED_UNIT_ARGB	0x9027593e
#define	DISABLED_UNIT_ARGB	0x80b9b9b9
#define	NONE_UNIT_ARGB		0x808b8b8b

#define	SF_LEVEL_NUM		4


////////////////////////////////////////////////////////////////////////////
BOOL	EP_LinkSkill( DWORD pi_dwIndex );
BOOL	EP_UnlinkSkill( DWORD pi_dwIndex );

void	EP_SetSFInfoString( BOOL pi_bIsForce, DWORD pi_dwSFID, CGUIItem * pi_pGuiItem );

////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//
// CGUISkFcUnit Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUISkFcUnit::CGUISkFcUnit()
{
	m_cItem.SetSize( 32, 32 );	
	m_cExp.SetSize( 0, 4 );
	m_cExpBase.SetSize( 120, 8 );
	m_cExpPercent.SetSize( 7, 9 );
	m_cLink.SetSize( 11, 11 );			

	m_cItem.SetPos( m_ptPos.x + 7, m_ptPos.y + 1 );
	m_cExpBase.SetPos( m_cItem.m_ptPos.x + m_cItem.m_ptSize.x + 7, m_ptPos.y + 24 );	
	m_cExp.SetPos( m_cItem.m_ptPos.x + m_cItem.m_ptSize.x + 6, m_ptPos.y + 26 );
	m_cExpNumber.SetPos( m_cExpBase.m_ptPos.x + m_cExpBase.m_ptSize.x + 3, m_cExpBase.m_ptPos.y - 1 );
	m_cExpPercent.SetPos( m_cExpBase.m_ptPos.x + m_cExpBase.m_ptSize.x + 33, m_cExpNumber.m_ptPos.y );
	m_cItemName.SetPos( m_ptPos.x + 48, m_ptPos.y + 8 );
	m_cLevelStr.SetPos( m_ptPos.x + 170, m_ptPos.y + 8 );
	m_cUnderline.SetPos( m_ptPos.x, m_ptPos.y + m_ptSize.y - m_cUnderline.m_ptSize.y );

	Add( &m_cItem );
	Add( &m_cExpBase );
	Add( &m_cExp );	
	Add( &m_cExpNumber );	
	Add( &m_cExpPercent );	
	Add( &m_cItemName );
	Add( &m_cLevelStr );
	Add( &m_cUnderline );
	Add( &m_cCover );

	m_cUnderline.SetARGB( 0xa0000000 );

	m_cItemName.GetFont()->dwColor = TITLE_COLOR;

	m_cExpNumber.SetNumberColor( 0xFFFFFFC8 );

	m_cExp.SetTileSize( 4, 4 );
	m_cExp.SetGap( 3, 0 );
	

	m_eUnitState = Enabled;	

	m_bHovered = FALSE;

	m_bLoadedSprite = FALSE;
}

CGUISkFcUnit::~CGUISkFcUnit()
{

}

void
CGUISkFcUnit::SetSprite( void )
{
	if( !m_bLoadedSprite )
	{
		SPRITE_INFO l_spriteInfo;

		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		l_spriteInfo.wActionNo = 7;
		
		l_spriteInfo.wFrameNo = 3;
		m_cExp.SetSpriteInfo( &l_spriteInfo ); 

		l_spriteInfo.wFrameNo = 4;
		m_cExpBase.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 5;
		m_cLink.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 7;
		m_cGM.SetSpriteInfo( &l_spriteInfo );
		
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 6;
		POINT l_ptNumberSize = { 7, 9 };
		m_cExpNumber.SetNumberSprite( &l_spriteInfo, l_ptNumberSize );
		l_spriteInfo.wFrameNo = 10;
		m_cExpPercent.SetSpriteInfo( &l_spriteInfo );

		// init
		SetExp( 0 );

		m_bLoadedSprite = TRUE;
	}
	else
	{
		CSprite * l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );

		m_cExp.GetSpriteInfo()->pSprite = l_pSprite;
		m_cExpBase.GetSpriteInfo()->pSprite = l_pSprite;
		m_cLink.GetSpriteInfo()->pSprite = l_pSprite;	
		m_cGM.GetSpriteInfo()->pSprite = l_pSprite;	

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		m_cExpNumber.GetSpriteInfo()->pSprite = l_pSprite;	
		m_cExpPercent.GetSpriteInfo()->pSprite = l_pSprite;	

		m_cItem.GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
	}
}

void
CGUISkFcUnit::SetSize( LONG pi_nWidth, LONG pi_nHeight )
{
	CGUIObject::SetSize( pi_nWidth, pi_nHeight );

	m_cCover.SetSize( pi_nWidth, pi_nHeight );
	m_cUnderline.SetSize( pi_nWidth, 1 );

	m_cUnderline.SetPos( m_ptPos.x, m_ptPos.y + m_ptSize.y - m_cUnderline.m_ptSize.y );
}

void
CGUISkFcUnit::SetUnitState( UnitState pi_eState )
{	 
	if( pi_eState == m_eUnitState )
		return;

	if( m_eUnitState == Linked )	
		Remove( &m_cLink );
	
	m_eUnitState = pi_eState;
	if( m_eUnitState == Enabled )
	{
		m_cCover.SetARGB( 0xFFFFFFFF );
	}
	else if( m_eUnitState == Linked )
	{
		m_cCover.SetARGB( 0xFFFFFFFF );

		m_cLink.Show( IsShow() );
		m_cLink.SetPos( m_cItem.m_ptPos.x,
						m_cItem.m_ptPos.y + m_cItem.m_ptSize.y - m_cLink.m_ptSize.y );		
		Add( &m_cLink );
	}
	else if( m_eUnitState == Disabled )
	{
		m_cCover.SetARGB( 0x80A05050 );
	}
	else if( m_eUnitState == NoneUnit )
	{
		m_cCover.SetARGB( 0xB0000000 );
	}
}

void
CGUISkFcUnit::SetItemName( char *pi_pName )
{
	m_cItemName.SetString( pi_pName );
}

#define	GM_LEVEL	7

void
CGUISkFcUnit::SetLevel( BYTE pi_byLevel )
{
	if( pi_byLevel > GM_LEVEL )
		return;

/*
	// GM 
	if( pi_byLevel == GM_LEVEL )
	{
		Remove( &m_cLevelStr );
		Remove( &m_cExpNumber );
		Remove( &m_cExpPercent );
		
		// GM 이미지 처리
		m_cGM.SetSize( 31, 10 );
		m_cGM.SetPos( m_ptPos.x + 170, m_ptPos.y + 11 );

		m_cGM.Show( IsShow() );
		// m_cCover바로 아래에 추가한다
		Add( &m_cGM, GetAddedObjectNum() - 2 );
	}
	else
	{
		char buf[16];
		sprintf( buf, "Lv %d", pi_byLevel );
		
		m_cLevelStr.SetString( buf );	
	}
*/
	char buf[16];
	if( pi_byLevel == GM_LEVEL )
	{
		// GM일때 경험치는 안보여준다
		m_cExpNumber.GetSpriteInfo()->pSprite = NULL;
		m_cExpPercent.GetSpriteInfo()->pSprite = NULL;

		strcpy( buf, "G M" );
		m_cLevelStr.SetString( buf );

		m_cLevelStr.GetFont()->dwColor = D3DCOLOR_XRGB( 188, 192, 250 );

		m_cLevelStr.SetPos( m_ptPos.x + 175, m_ptPos.y + 12 );
	}
	else
	{
		if( m_cExpNumber.GetSpriteInfo()->pSprite == NULL )
		{
			CSprite * l_pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
			m_cExpNumber.GetSpriteInfo()->pSprite = l_pSprite;
			m_cExpPercent.GetSpriteInfo()->pSprite = l_pSprite;
		}

		sprintf( buf, "Lv %d", pi_byLevel );
		m_cLevelStr.SetString( buf );

		m_cLevelStr.GetFont()->dwColor = D3DCOLOR_XRGB( 255, 184, 71 );

		m_cLevelStr.SetPos( m_ptPos.x + 170, m_ptPos.y + 8 );
	}
}

void
CGUISkFcUnit::SetItem( CGUIItem * pi_pItem )
{
	m_cItem.SetData( pi_pItem );
	m_cItem.SetSize( 32, 32 );
	m_cItem.Show( IsShow() );
}

void
CGUISkFcUnit::SetItem( DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byIconType )
{
	m_cItem.SetItemID( pi_dwItemID );
	m_cItem.SetSpriteID( pi_dwSpriteID );
	m_cItem.SetIconType( pi_byIconType );
	
	m_cItem.SetSprite();
	m_cItem.Show( IsShow() );
}

void
CGUISkFcUnit::SetExp( float pi_fExp )
{
	int l_nTileNum = 17;

	POINT l_ptTileSize;
	m_cExp.GetTileSize( l_ptTileSize );	

	float l_fExpRate = pi_fExp / 99.99f;	
	if( l_fExpRate > 1.f )
		l_fExpRate = 1.f;

	m_cExp.SetSize( l_nTileNum * l_fExpRate * ( l_ptTileSize.x + m_cExp.GetGapW() ) + m_cExp.GetGapW(),
					m_cExp.m_ptSize.y );
		
	m_cExpNumber.SetNumber( pi_fExp, 2 );

	// repos
	m_cExpNumber.SetPos( m_cExpPercent.m_ptPos.x - m_cExpNumber.m_ptSize.x - 1, m_cExpBase.m_ptPos.y - 1 );
}

void
CGUISkFcUnit::Hovered( void )
{
	m_bHovered = TRUE;
	//m_cCover.SetARGB( 0xA040A0A0 );
}

void
CGUISkFcUnit::UnHovered( void )
{
	m_bHovered = FALSE;
	//m_cCover.SetARGB( 0xFFFFFFFF );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//
// CGUISkillForceBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUISkillForceBoard::CGUISkillForceBoard()
{
	m_pHoveredUnit	= NULL;	
	m_byTabNum		= TRUE;

	m_bCreated		= FALSE;
}

CGUISkillForceBoard::~CGUISkillForceBoard()
{
	
}

void
CGUISkillForceBoard::Init( POINT & pi_ptSize, BYTE pi_byTabNum )
{
	SetSize( pi_ptSize );

	POINT l_ptSize;
	POINT l_ptPos;	

	// scrollbar size
	l_ptSize.x = 15;	l_ptSize.y = 10;
	m_uiScrollbar.SetTopArrowSize( l_ptSize );
	m_uiScrollbar.SetBottomArrowSize( l_ptSize );	
	l_ptSize.x = 15;	l_ptSize.y = 18;
	m_uiScrollbar.SetBarSize( l_ptSize );	
	l_ptSize.x = 15; l_ptSize.y = m_ptSize.y;
	m_uiScrollbar.SetSize( l_ptSize );

	m_uiTabBase.SetSize( m_ptSize.x - m_uiScrollbar.m_ptSize.x, m_ptSize.y );

	// position
	l_ptPos.x = m_ptPos.x + m_ptSize.x - m_uiScrollbar.m_ptSize.x;
	l_ptPos.y = m_ptPos.y;
	m_uiScrollbar.SetPos( l_ptPos );

	m_uiTabBase.SetPos( m_ptPos );
	
	// add controller
	m_uiScrollbar.SetAdjustmentUnit( UNIT_SIZE_Y / 2 );
	m_uiScrollbar.AddScrollHandler( this );	
	Add( &m_uiScrollbar );

	Add( &m_uiTabBase );

	m_byTabNum = pi_byTabNum;

	if( m_byTabNum )
	{
		for( int i = 0; i < m_byTabNum; ++i )
		{
			m_uiTab[i].SetSize( m_uiTabBase.m_ptSize.x, 18 );
			m_uiTabBoard[i].SetSize( m_uiTab[i].m_ptSize );

			m_uiTabBoard[i].SetPos( m_uiTabBase.m_ptPos.x,
									m_uiTabBase.m_ptPos.y + i * m_uiTabBoard[i].m_ptSize.y );

			m_uiTab[i].AddMouseEventHandler( this ); 
			m_uiTabName[i].AddMouseEventHandler( this );

			m_uiTabName[i].GetFont()->dwColor = SUBTITLE_COLOR;

			m_uiTab[i].SetPos( m_uiTabBoard[i].m_ptPos );
			m_uiTabName[i].SetPos( m_uiTab[i].m_ptPos.x + 40, m_uiTab[i].m_ptPos.y + 3 );
			
			m_uiTabBoard[i].Add( &m_uiTab[i] );
			m_uiTabBoard[i].Add( &m_uiTabName[i] );

			m_uiTabBase.Add( &m_uiTabBoard[i] );
		}
	}	
	

	SetSprite();			

	m_bCreated = TRUE;	
}

void
CGUISkillForceBoard::SetSprite( void )
{
	if( !m_bCreated )
	{	
		// scrollbar	
		SPRITE_INFO l_sScrollbarSprite[2];
		l_sScrollbarSprite[0].pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_sScrollbarSprite[0].wActionNo = 4;
		memcpy( &l_sScrollbarSprite[1], &l_sScrollbarSprite[0], sizeof( SPRITE_INFO ) );
		
		l_sScrollbarSprite[0].wFrameNo = 0;
		m_uiScrollbar.SetSpriteInfo( &l_sScrollbarSprite[0] );
		l_sScrollbarSprite[0].wFrameNo = 1;
		m_uiScrollbar.SetBarSpriteInfo( &l_sScrollbarSprite[0] );

		l_sScrollbarSprite[0].wFrameNo = 2;
		l_sScrollbarSprite[1].wFrameNo = 3;
		m_uiScrollbar.SetTopArrowSpriteInfo( l_sScrollbarSprite );

		l_sScrollbarSprite[0].wFrameNo = 4;
		l_sScrollbarSprite[1].wFrameNo = 5;
		m_uiScrollbar.SetBottomArrowSpriteInfo( l_sScrollbarSprite );	

		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );		

		l_spriteInfo.wActionNo = 7;
		l_spriteInfo.wFrameNo = 1;
		for( int i=0; i<4; ++i )
		{
			m_uiTab[i].SetSpriteInfo( &l_spriteInfo );
		}
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		if( !l_pSprite )
			return;

		m_uiScrollbar.SetSprite( l_pSprite );

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		for( int i=0; i<4; ++i )
		{
			m_uiTab[i].GetSpriteInfo()->pSprite = l_pSprite;
		}
		
		for( SkillForceUnitList::iterator it=m_listUnit.begin(); it!=m_listUnit.end(); ++it )
		{
			(*it)->SetSprite();
		}		
	}
}

void
CGUISkillForceBoard::SetBound( RECT & pi_rtBound )
{
	CGUIObject::SetBound( pi_rtBound );
	
	RECT l_rtBound;
	l_rtBound.left = pi_rtBound.right - m_uiScrollbar.m_ptSize.x;
	l_rtBound.right = pi_rtBound.right;
	l_rtBound.top = pi_rtBound.top;
	l_rtBound.bottom = pi_rtBound.bottom;
	
	m_uiScrollbar.SetBound( l_rtBound );	

	// setamount scrollbar	
	int l_nCurAmount;
	int l_nTotalAmount;
	l_nTotalAmount = m_ptPos.y - m_uiTabBase.m_ptPos.y;
	if( m_uiTabBase.m_ptPos.y + m_uiTabBase.m_ptSize.y > m_ptPos.y + m_ptSize.y )
		l_nTotalAmount += m_uiTabBase.m_ptPos.y + m_uiTabBase.m_ptSize.y - ( m_ptPos.y + m_ptSize.y );

	if( m_uiScrollbar.GetCurrentAmount() > l_nTotalAmount )
		l_nCurAmount = l_nTotalAmount;
	else
		l_nCurAmount = m_uiScrollbar.GetCurrentAmount();

	m_uiScrollbar.SetAmount( l_nTotalAmount, l_nCurAmount );


	m_uiTabBase.SetPos( m_ptPos.x, m_uiTabBase.m_ptPos.y );	
	
	UpdateBoard();
}

BOOL
CGUISkillForceBoard::IsOpenedTab( BYTE pi_byTab )
{
	if( !m_byTabNum )
		return TRUE;
	else
		return m_uiTabBoard[pi_byTab].IsAddedObject( &m_uiSFBoard[pi_byTab] );
}

void
CGUISkillForceBoard::SetTab( BYTE pi_byTabIndex, BOOL pi_bOpen )
{
	if( !m_byTabNum )
		return;

	if( IsOpenedTab( pi_byTabIndex ) )
	{
		if( pi_bOpen ) return;
	}
	else
	{
		if( !pi_bOpen ) return;
	}

	if( pi_bOpen )
	{
		// add sfboard
		m_uiSFBoard[pi_byTabIndex].SetPos( m_uiTab[pi_byTabIndex].m_ptPos.x,
										   m_uiTab[pi_byTabIndex].m_ptPos.y + m_uiTab[pi_byTabIndex].m_ptSize.y );
		m_uiSFBoard[pi_byTabIndex].Show( IsShow() );
		m_uiTabBoard[pi_byTabIndex].Add( &m_uiSFBoard[pi_byTabIndex] );

		// resize tabboard
		m_uiTabBoard[pi_byTabIndex].SetSize( m_uiTab[pi_byTabIndex].m_ptSize.x,
											 m_uiTab[pi_byTabIndex].m_ptSize.y + m_uiSFBoard[pi_byTabIndex].m_ptSize.y );			

		// sprite
		m_uiTab[pi_byTabIndex].GetSpriteInfo()->wFrameNo = 0;
	}
	else
	{
		m_uiTabBoard[pi_byTabIndex].Remove( &m_uiSFBoard[pi_byTabIndex] );		

		// resize tabboard
		m_uiTabBoard[pi_byTabIndex].SetSize( m_uiTab[pi_byTabIndex].m_ptSize );

		// sprite
		m_uiTab[pi_byTabIndex].GetSpriteInfo()->wFrameNo = 1;
	}

	// repos tabboard
	for( int i=pi_byTabIndex+1; i<4; ++i )
	{
		m_uiTabBoard[i].SetPos( m_uiTabBoard[i-1].m_ptPos.x,
								m_uiTabBoard[i-1].m_ptPos.y + m_uiTabBoard[i-1].m_ptSize.y );
	}

	// resize tabbase
	m_uiTabBase.SetSize( m_uiTabBase.m_ptSize.x, 
						 m_uiTabBoard[3].m_ptPos.y + m_uiTabBoard[3].m_ptSize.y - m_uiTabBoard[0].m_ptPos.y );

	int l_nCurAmount;
	int l_nTotalAmount;
	l_nTotalAmount = m_ptPos.y - m_uiTabBase.m_ptPos.y;
	if( m_uiTabBase.m_ptPos.y + m_uiTabBase.m_ptSize.y > m_ptPos.y + m_ptSize.y )
		l_nTotalAmount += m_uiTabBase.m_ptPos.y + m_uiTabBase.m_ptSize.y - ( m_ptPos.y + m_ptSize.y );

	if( m_uiScrollbar.GetCurrentAmount() > l_nTotalAmount )
		l_nCurAmount = l_nTotalAmount;
	else
		l_nCurAmount = m_uiScrollbar.GetCurrentAmount();

	m_uiScrollbar.SetAmount( l_nTotalAmount, l_nCurAmount );
}

void
CGUISkillForceBoard::SetLevelMastery( BYTE pi_byStep, int pi_nMastery )
{
	if( pi_byStep >= 4 )
		return;

	char l_pStepName[4][32] = { "저  급", "중  급", "고  급", "최고급" };	

	char l_pBuf[32];
	sprintf( l_pBuf, "%s 마스터리 %2d pt", l_pStepName[pi_byStep], pi_nMastery );

	m_uiTabName[pi_byStep].SetString( l_pBuf );
}

void
CGUISkillForceBoard::SetTabName( BYTE pi_byStep, char * pi_pTabName )
{
	if( pi_byStep >= 4 )
		return;

	m_uiTabName[pi_byStep].SetString( pi_pTabName );
	m_uiTabName[pi_byStep].SetPos( m_uiTab[pi_byStep].m_ptPos.x + 
										( m_uiTab[pi_byStep].m_ptSize.x - m_uiTabName[pi_byStep].m_ptSize.x ) / 2,
									m_uiTabName[pi_byStep].m_ptPos.y );
}

BOOL
CGUISkillForceBoard::AddUnit( CGUISkFcUnit * pi_pUnit )
{
	pi_pUnit->SetSize( m_ptSize.x - m_uiScrollbar.m_ptSize.x, UNIT_SIZE_Y );

	m_listUnit.push_back( pi_pUnit );

	if( m_byTabNum )
	{	
		pi_pUnit->SetPos( m_uiSFBoard[pi_pUnit->GetStep()].m_ptPos.x,
						  m_uiSFBoard[pi_pUnit->GetStep()].m_ptPos.y + 
								m_uiSFBoard[pi_pUnit->GetStep()].GetAddedObjectNum() * pi_pUnit->m_ptSize.y );
		pi_pUnit->Show( m_uiSFBoard[pi_pUnit->GetStep()].IsShow() );
		m_uiSFBoard[pi_pUnit->GetStep()].Add( pi_pUnit );

		// resize sfboard
		m_uiSFBoard[pi_pUnit->GetStep()].SetSize( m_uiTab[0].m_ptSize.x,
												pi_pUnit->m_ptSize.y * m_uiSFBoard[pi_pUnit->GetStep()].GetAddedObjectNum() );

		if( IsOpenedTab( pi_pUnit->GetStep() ) )
		{
			// resize tabboard
			m_uiTabBoard[pi_pUnit->GetStep()].SetSize( m_uiTabBoard[pi_pUnit->GetStep()].m_ptSize.x,
													 m_uiTab[pi_pUnit->GetStep()].m_ptSize.y + m_uiSFBoard[pi_pUnit->GetStep()].m_ptSize.y );

			// repos tabboard
			for( int i=pi_pUnit->GetStep()+1; i<4; ++i )
			{
				m_uiTabBoard[i].SetPos( m_uiTabBoard[i].m_ptPos.x,
										m_uiTabBoard[i-1].m_ptPos.y + m_uiTabBoard[i-1].m_ptSize.y );
			}
		}	

		// resize tabbase
		m_uiTabBase.SetSize( m_uiTabBase.m_ptSize.x,
							 m_uiTabBoard[3].m_ptPos.y + m_uiTabBoard[3].m_ptSize.y - m_uiTabBoard[0].m_ptPos.y );
	}
	else
	{
		pi_pUnit->SetPos( m_uiTabBase.m_ptPos.x,
						  m_uiTabBase.m_ptPos.y + 
							m_uiTabBase.GetAddedObjectNum() * pi_pUnit->m_ptSize.y );	
		pi_pUnit->Show( m_uiTabBase.IsShow() );
		m_uiTabBase.Add( pi_pUnit );

		m_uiTabBase.SetSize( m_uiTabBase.m_ptSize.x,
							 pi_pUnit->m_ptSize.y * m_uiTabBase.GetAddedObjectNum() );							 
	}

	// setamount scrollbar	
	int l_nCurAmount;
	int l_nTotalAmount;
	l_nTotalAmount = m_ptPos.y - m_uiTabBase.m_ptPos.y;
	if( m_uiTabBase.m_ptPos.y + m_uiTabBase.m_ptSize.y > m_ptPos.y + m_ptSize.y )
		l_nTotalAmount += m_uiTabBase.m_ptPos.y + m_uiTabBase.m_ptSize.y - ( m_ptPos.y + m_ptSize.y );

	if( m_uiScrollbar.GetCurrentAmount() > l_nTotalAmount )
		l_nCurAmount = l_nTotalAmount;
	else
		l_nCurAmount = m_uiScrollbar.GetCurrentAmount();

	m_uiScrollbar.SetAmount( l_nTotalAmount, l_nCurAmount );	

	// update ui
	UpdateBoard();
		
	return true;
}

CGUISkFcUnit *	
CGUISkillForceBoard::RemoveUnit( WORD pi_wIndex )
{
	CGUISkFcUnit * l_pUnit;
	for( SkillForceUnitList::iterator it=m_listUnit.begin(); it!=m_listUnit.end(); ++it )
	{
		if( (*it)->GetIndex() == pi_wIndex )
			break;
	}

	if( it == m_listUnit.end() )
		return NULL;

	l_pUnit = *it;

	// remove unit
	m_listUnit.erase( it );

	if( m_byTabNum )
	{
		m_uiSFBoard[l_pUnit->GetStep()].Remove( l_pUnit );

		// resize sfboard
		m_uiSFBoard[l_pUnit->GetStep()].SetSize( m_uiSFBoard[l_pUnit->GetStep()].m_ptSize.x,
												l_pUnit->m_ptSize.y * m_uiSFBoard[l_pUnit->GetStep()].GetAddedObjectNum() );

		if( IsOpenedTab( l_pUnit->GetStep() ) )
		{
			// resize tabboard
			m_uiTabBoard[l_pUnit->GetStep()].SetSize( m_uiTabBoard[l_pUnit->GetStep()].m_ptSize.x,
													 m_uiTab[l_pUnit->GetStep()].m_ptSize.y + m_uiSFBoard[l_pUnit->GetStep()].m_ptSize.y );

			// repos tabboard
			for( int i=l_pUnit->GetStep()+1; i<4; ++i )
			{
				m_uiTabBoard[i].SetPos( m_uiTabBoard[i].m_ptPos.x,
										m_uiTabBoard[i-1].m_ptPos.y + m_uiTabBoard[i-1].m_ptSize.y );
			}
		}	

		// resize tabbase
		m_uiTabBase.SetSize( m_uiTabBase.m_ptSize.x,
							 m_uiTabBoard[3].m_ptPos.y + m_uiTabBoard[3].m_ptSize.y - m_uiTabBoard[0].m_ptPos.y );
	}
	else
	{
		m_uiTabBase.Remove( l_pUnit );

		m_uiTabBase.SetSize( m_uiTabBase.m_ptSize.x,
							 l_pUnit->m_ptSize.y * m_uiTabBase.GetAddedObjectNum() );
	}

	// setamount scrollbar	
	int l_nCurAmount;
	int l_nTotalAmount;
	l_nTotalAmount = m_ptPos.y - m_uiTabBase.m_ptPos.y;
	if( m_uiTabBase.m_ptPos.y + m_uiTabBase.m_ptSize.y > m_ptPos.y + m_ptSize.y )
		l_nTotalAmount += m_uiTabBase.m_ptPos.y + m_uiTabBase.m_ptSize.y - ( m_ptPos.y + m_ptSize.y );

	if( m_uiScrollbar.GetCurrentAmount() > l_nTotalAmount )
		l_nCurAmount = l_nTotalAmount;
	else
		l_nCurAmount = m_uiScrollbar.GetCurrentAmount();

	m_uiScrollbar.SetAmount( l_nTotalAmount, l_nCurAmount );	

	// update ui
	UpdateBoard();

	return l_pUnit;
}

void
CGUISkillForceBoard::RemoveAllUnit( SkillForceUnitList * po_pListUnit )
{	
	po_pListUnit->insert( po_pListUnit->begin(), m_listUnit.begin(), m_listUnit.end() );

	// clear
	m_listUnit.clear();
	m_uiScrollbar.SetAmount( 0, 0 );

	// update ui
	UpdateBoard();
}

CGUISkFcUnit *
CGUISkillForceBoard::GetUnit( DWORD pi_dwIndex, BOOL pi_bByIndex )
{	
	CGUISkFcUnit * pSFUnit;
	for( SkillForceUnitList::iterator i=m_listUnit.begin(); i!=m_listUnit.end(); ++i )
	{
		pSFUnit = *i;

		if( pi_bByIndex )
		{
			if( pSFUnit->GetIndex() == pi_dwIndex )
				return (*i);
		}
		else
		{
			if( pSFUnit->GetItem()->m_dwItemID == pi_dwIndex )
				return (*i);
		}
	}

	return NULL;
}

CGUISkFcUnit	*	
CGUISkillForceBoard::GetUnit( CGUIItem * pi_pItem )
{
	for( SkillForceUnitList::iterator i=m_listUnit.begin(); i!=m_listUnit.end(); ++i )
	{
		if( (*i)->GetItem() == pi_pItem )
			return (*i);
	}

	return NULL;
}

CGUISkFcUnit	*	
CGUISkillForceBoard::GetUnitByID( DWORD pi_dwItemID )
{
	for( SkillForceUnitList::iterator i=m_listUnit.begin(); i!=m_listUnit.end(); ++i )
	{
		DWORD a = (*i)->GetItem()->m_dwItemID;
		if( a == pi_dwItemID )
			return (*i);		
	}

	return NULL;
}

CGUISkFcUnit	*	
CGUISkillForceBoard::GetSource( CGUIObject * pi_pSrcObj )
{
	for( SkillForceUnitList::iterator i=m_listUnit.begin(); i!=m_listUnit.end(); ++i )
	{
		if( (*i)->GetCover() == pi_pSrcObj )			
			return (*i);
	}

	return NULL;
}

void
CGUISkillForceBoard::SetHoverUnit( CGUISkFcUnit * pi_pUnit )
{
	if( m_pHoveredUnit )
		m_pHoveredUnit->UnHovered();

	m_pHoveredUnit = pi_pUnit;

	if( m_pHoveredUnit )
		m_pHoveredUnit->Hovered();
}

void
CGUISkillForceBoard::UpdateBoard( void )
{
	m_uiTabBase.SetPos( m_uiTabBase.m_ptPos.x, 
						m_ptPos.y - m_uiScrollbar.GetCurrentAmount() );
}

void
CGUISkillForceBoard::ScrollEvent( _scroll_event & event )
{
	UpdateBoard();
}

void
CGUISkillForceBoard::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_STROKED )
	{
		for( int i=0; i<4; ++i )
		{
			if( event.source == &m_uiTab[i] ||
				event.source == &m_uiTabName[i] )
			{
				SetTab( i, !IsOpenedTab( i ) );

				return;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
// CGUISFWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUISFWindow::CGUISFWindow()
{
	m_pSFBoardList	= NULL;
	m_bySFBoardNum	= 0;

	m_uiSFBoardTab	= NULL;
	m_byCurBoardTab	= 0xFF;

	m_bIsCreated	= FALSE;
}

CGUISFWindow::~CGUISFWindow()
{
	if( m_pSFBoardList )
	{
		SkillForceUnitList * l_pList;
		SkillForceUnitList::iterator it;

		for( int i=0; i<m_bySFBoardNum; ++i )
		{
			l_pList = m_pSFBoardList[i].GetList();

			for( it=l_pList->begin(); it!=l_pList->end(); ++it )
				delete *it;
		}

		delete[] m_pSFBoardList;
		m_pSFBoardList = NULL;
	}

	if( m_uiSFBoardTab )
	{
		delete[] m_uiSFBoardTab;
		m_uiSFBoardTab = NULL;
	}
}

void	
CGUISFWindow::CreateSFBoard( BYTE pi_bySFBoardNum, BYTE * pi_pTabNumList )
{
	if( !pi_bySFBoardNum )
		return;

	m_bySFBoardNum	= pi_bySFBoardNum;
	m_pSFBoardList	= new CGUISkillForceBoard[m_bySFBoardNum];
	m_uiSFBoardTab	= new CGUIButton[m_bySFBoardNum];

	POINT	l_ptSize;
	l_ptSize.x = 222;
	l_ptSize.y = 298;
	for( int i = 0; i < m_bySFBoardNum; ++i )
	{
		m_pSFBoardList[i].Init( l_ptSize, pi_pTabNumList[i] );
	}
}

void
CGUISFWindow::Init( void )
{
	if( !m_pSFBoardList )
		return;
	
	// Size setting
	SetSize( 228, 395 );

	m_uiAniTitleBar.SetSize( 228, 28 );	
	m_uiCloseButton.SetSize( 16, 13 );	
	m_uiHelpButton.SetSize( 16, 13 );
	
	m_uiResizeUnderBar.SetSize( 228, 17 );	

	m_uiLOutline.SetSize( 1, m_ptSize.y - m_uiAniTitleBar.m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );
	m_uiROutline.SetSize( m_uiLOutline.m_ptSize );

	m_uiTabUnderLine.SetSize( 221, 1 );

	m_uiBoard.SetSize( 228, m_ptSize.y - m_uiAniTitleBar.m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );
	
	for( int i=0; i<m_bySFBoardNum; ++i )
		m_uiSFBoardTab[i].SetSize( 36, 36 );

	
	// position setting	
	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 7 );
	m_uiAniTitleBar.SetPos( m_ptPos.x, m_ptPos.y );
	m_uiCloseButton.SetPos( m_ptPos.x + 211, m_ptPos.y + 4 );
	m_uiHelpButton.SetPos( m_ptPos.x + 194, m_ptPos.y + 4 );

	m_uiBoard.SetPos( m_ptPos.x, m_uiAniTitleBar.m_ptPos.y + m_uiAniTitleBar.m_ptSize.y );

	m_uiLOutline.SetPos( m_ptPos.x + 2, m_uiAniTitleBar.m_ptPos.y + m_uiAniTitleBar.m_ptSize.y );
	m_uiROutline.SetPos( m_ptPos.x + m_ptSize.x - 2 - m_uiROutline.m_ptSize.x,
						 m_uiAniTitleBar.m_ptPos.y + m_uiAniTitleBar.m_ptSize.y );

	int l_nTabWidth = m_ptSize.x / m_bySFBoardNum;
	for( i=0; i<m_bySFBoardNum; ++i )
	{
		m_uiSFBoardTab[i].SetPos( m_ptPos.x + l_nTabWidth * i + ( l_nTabWidth - m_uiSFBoardTab[i].m_ptSize.x ) / 2 ,
								  m_uiAniTitleBar.m_ptPos.y + m_uiAniTitleBar.m_ptSize.y + TAB_MARGIN_H );
	}

	m_uiTabUnderLine.SetPos( m_uiLOutline.m_ptPos.x + m_uiLOutline.m_ptSize.x + 1,
							 m_uiSFBoardTab[0].m_ptPos.y + m_uiSFBoardTab[0].m_ptSize.y + TAB_MARGIN_H - m_uiTabUnderLine.m_ptSize.y );

	m_uiResizeUnderBar.SetPos( m_ptPos.x, m_ptPos.y + m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );
	

	// add controller	
	Add( &m_uiAniTitleBar );
	Add( &m_uiTitle );

	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );
	Add( &m_uiCloseButton );
	Add( &m_uiHelpButton );

	Add( &m_uiBoard );

	Add( &m_uiLOutline );
	Add( &m_uiROutline );

	Add( &m_uiTabUnderLine );
	
	for( i=0; i<m_bySFBoardNum; ++i )
	{
		m_pSFBoardList[i].AddMouseEventHandler( this );

		m_uiSFBoardTab[i].AddMouseEventHandler( this );	

		Add( &m_uiSFBoardTab[i] );
	}	

	m_uiResizeUnderBar.AddMouseEventHandler( this );	
	Add( &m_uiResizeUnderBar );	

	
	m_uiTitle.GetFont()->dwColor = TITLE_COLOR;
	m_uiBoard.SetARGB( D3DCOLOR_ARGB( 204, 22, 28, 28 ) );
	m_uiLOutline.SetARGB( D3DCOLOR_XRGB( 175, 180, 186 ) );
	m_uiROutline.SetARGB( D3DCOLOR_XRGB( 175, 180, 186 ) );
	m_uiTabUnderLine.SetARGB( 0xa0000000 );	
	

	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );

	
	// Set Resize Object;
	SetReSizeObject( CGUIWindow::BOTTOM, &m_uiResizeUnderBar );
	SetReSizable( true );

	SetMoveObject( &m_uiAniTitleBar );	
	SetMoveObject( &m_uiTitle );			
	SetMovable( true );	
	
	SoundOn( TRUE );	
	
	SetCurBoardTab( 0 );
}

void
CGUISFWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{		
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );

		// ani title bar	
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );
		
		// tab	


		l_spriteInfo.wActionNo = 7;
		l_spriteInfo.wFrameNo = 2;
		m_uiResizeUnderBar.SetSpriteInfo( &l_spriteInfo );
		

		// close button			
		m_uiCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_uiCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_uiCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );			

		// help button
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_spriteInfo.wActionNo = 5;
		l_spriteInfo.wFrameNo = 0;
		m_uiHelpButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 1;
		m_uiHelpButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiHelpButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		if( !l_pSprite )
			return;	
		
		// ani title bar	
		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;		
		

		// sfboard
		for( int i=0; i<m_bySFBoardNum; ++i )
		{
			m_pSFBoardList[i].SetSprite();
		}		

		m_uiResizeUnderBar.GetSpriteInfo()->pSprite = l_pSprite;

		// close button	
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );		 
		m_uiCloseButton.SetSprite( l_pSprite );		
		// help button
		m_uiHelpButton.SetSprite( l_pSprite );
	}
}

void
CGUISFWindow::SetBound( RECT & pi_rtBound )
{
	if( pi_rtBound.bottom - pi_rtBound.top < m_uiAniTitleBar.m_ptSize.y + m_uiResizeUnderBar.m_ptSize.y + 137 )
		return;	

	//CGUIObject::SetBound( pi_rtBound );	
	SetPos( pi_rtBound.left, pi_rtBound.top );
	SetSize( pi_rtBound.right - pi_rtBound.left, pi_rtBound.bottom - pi_rtBound.top );

	m_uiBoard.SetSize( m_uiBoard.m_ptSize.x, m_ptSize.y - m_uiAniTitleBar.m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );

	m_uiLOutline.SetSize( m_uiLOutline.m_ptSize.x, m_uiBoard.m_ptSize.y );
	m_uiROutline.SetSize( m_uiLOutline.m_ptSize );	

	// repos underbar
	m_uiResizeUnderBar.SetPos( m_ptPos.x, m_ptPos.y + m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );	
	

	// force board bound setting
	if( !m_bySFBoardNum )
		return;
	RECT	l_rtBoardBound;
	l_rtBoardBound.left = m_uiLOutline.m_ptPos.x + m_uiLOutline.m_ptSize.x;
	l_rtBoardBound.right = m_uiROutline.m_ptPos.x;
	l_rtBoardBound.top = m_uiSFBoardTab[0].m_ptPos.y + m_uiSFBoardTab[0].m_ptSize.y + TAB_MARGIN_H;
	l_rtBoardBound.bottom = m_uiResizeUnderBar.m_ptPos.y;	
	for( int i=0; i<m_bySFBoardNum; ++i )
	{
		m_pSFBoardList[i].SetBound( l_rtBoardBound ); 
	}		
}

BOOL
CGUISFWindow::TakeInItem( BYTE pi_bySFBoardIndex, _skill_force_info * pi_pSFInfo )
{
	if( pi_bySFBoardIndex >= m_bySFBoardNum )
		return FALSE;

	
	CGUISkFcUnit	* l_pSFUnit;
	l_pSFUnit = new CGUISkFcUnit;	

	// sprite setting
	l_pSFUnit->SetSprite();	

	// set data
	l_pSFUnit->SetIndex( pi_pSFInfo->dwIndex );
	
	if( pi_pSFInfo->byStep > STEP_HIGHESST )
		l_pSFUnit->SetStep( STEP_LOW );
	else
		l_pSFUnit->SetStep( pi_pSFInfo->byStep );

	if( pi_pSFInfo->bySFType == 0 )
		l_pSFUnit->SetItem( pi_pSFInfo->dwItemID, pi_pSFInfo->dwSpriteID, SKILL_ICON );
	else if( pi_pSFInfo->bySFType == 1 )
		l_pSFUnit->SetItem( pi_pSFInfo->dwItemID, pi_pSFInfo->dwSpriteID, FORCE_ICON );
	else if( pi_pSFInfo->bySFType == 2 )
		l_pSFUnit->SetItem( pi_pSFInfo->dwItemID, pi_pSFInfo->dwSpriteID, CLASS_SKILL_ICON );

	l_pSFUnit->SetItemName( pi_pSFInfo->pName );
	l_pSFUnit->SetLevel( pi_pSFInfo->byLevel );		
	l_pSFUnit->SetExp( pi_pSFInfo->fExpByPercent );

	// event
	l_pSFUnit->GetCover()->AddMouseEventHandler( this );


	if( m_pSFBoardList[pi_bySFBoardIndex].AddUnit( l_pSFUnit ) )
	{
		return TRUE;
	}
	else
	{
		delete l_pSFUnit;
		return FALSE;
	}

	return TRUE;
}

BOOL
CGUISFWindow::TakeOutItem( BYTE pi_bySFBoardIndex, DWORD pi_dwItemIndex )
{
	if( pi_bySFBoardIndex >= m_bySFBoardNum )
		return FALSE;

	CGUISkFcUnit * l_pUnit = m_pSFBoardList[pi_bySFBoardIndex].RemoveUnit( pi_dwItemIndex );
	if( l_pUnit )
	{
		delete l_pUnit;

		return TRUE;
	}

	return FALSE;	
}

void	
CGUISFWindow::SetLevelMastery( BYTE pi_bySFBoardIndex, BYTE pi_byStep, int pi_nMastery )
{
	if( pi_bySFBoardIndex >= m_bySFBoardNum )
		return;

	m_pSFBoardList[pi_bySFBoardIndex].SetLevelMastery( pi_byStep, pi_nMastery );
}

void
CGUISFWindow::SetTabName( BYTE pi_bySFBoardIndex, BYTE pi_byStep, char * pi_pTabName )
{
	if( pi_bySFBoardIndex >= m_bySFBoardNum )
		return;

	m_pSFBoardList[pi_bySFBoardIndex].SetTabName( pi_byStep, pi_pTabName );
}

CGUISkFcUnit * 
CGUISFWindow::GetIconBoard( BYTE pi_bySFBoardIndex, DWORD pi_dwItemIndex, BOOL pi_bByIndex )
{
	if( pi_bySFBoardIndex >= m_bySFBoardNum )
		return NULL;

	return m_pSFBoardList[pi_bySFBoardIndex].GetUnit( pi_dwItemIndex, pi_bByIndex );	
}

void			
CGUISFWindow::SetCurBoardTab( BYTE pi_byBoardTab )
{
	if( pi_byBoardTab >= m_bySFBoardNum )
		return;

	if( pi_byBoardTab == m_byCurBoardTab )
		return;

	BYTE l_byPrevBoardTab = m_byCurBoardTab;
	m_byCurBoardTab = pi_byBoardTab;

	// tab sprite setting
	if( l_byPrevBoardTab < m_bySFBoardNum )
	{
		m_uiSFBoardTab[l_byPrevBoardTab].Enable();

		// remove prev board
		Remove( &m_pSFBoardList[l_byPrevBoardTab] );
	}
	m_uiSFBoardTab[m_byCurBoardTab].Disable();


	// set pos	
	m_pSFBoardList[m_byCurBoardTab].SetPos( m_uiLOutline.m_ptPos.x + m_uiLOutline.m_ptSize.x,
											m_uiSFBoardTab[0].m_ptPos.y + m_uiSFBoardTab[0].m_ptSize.y + TAB_MARGIN_H );
	m_pSFBoardList[m_byCurBoardTab].Show( IsShow() );
	Add( &m_pSFBoardList[m_byCurBoardTab] );
	SetWheelScrollObject( &m_pSFBoardList[m_byCurBoardTab].m_uiScrollbar );		
}

void
CGUISFWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_STROKED )
	{
		// sfboard tab
		for( int i = 0; i < m_bySFBoardNum; ++i )
		{
			if( event.source == &m_uiSFBoardTab[i] )
			{
				SetCurBoardTab( i );
				return;
			}
		}
		if( m_byCurBoardTab >= m_bySFBoardNum )
			return;


		// add
		if( gScreen.ExistObject() )
		{
			PostUIMessage( WM_BOARD_ADD_OBJECT, 0 );
		}
		// remove
		else
		{
			CGUISkFcUnit *	pOrgIconBoard	= m_pSFBoardList[m_byCurBoardTab].GetSource( event.source );
			if( !pOrgIconBoard )
				return;			

			SetQuickEvent();

			if( g_bIsQuickEvent_Equip )
				PostUIMessage( WM_BOARD_EQUIP_HOT_KEY, pOrgIconBoard->GetIndex() );
			else
				PostUIMessage( WM_BOARD_REMOVE_OBJECT, pOrgIconBoard->GetIndex() );
		}	

	}
	else if( event.id == LMOUSE_STROKED )
	{
		if( m_byCurBoardTab >= m_bySFBoardNum )
			return;

		CGUISkFcUnit *	pOrgIconBoard	= m_pSFBoardList[m_byCurBoardTab].GetSource( event.source );
		if( !pOrgIconBoard )
			return;

		PostUIMessage( WM_BOARD_USE_OBJECT, pOrgIconBoard->GetIndex() );
	}
	else if( event.id == MOUSE_HOVERED )
	{
		// board tab tooltip
		for( int i = 0; i < m_bySFBoardNum; ++i )
		{
			if( event.source == &m_uiSFBoardTab[i] )
			{
				SetTooltip( &m_uiSFBoardTab[i], m_uiSFBoardTab[i].GetName() );				
				return;
			}
		}
		if( m_byCurBoardTab >= m_bySFBoardNum )
			return;


		CGUISkFcUnit * pOrgIconBoard = NULL;
		pOrgIconBoard = m_pSFBoardList[m_byCurBoardTab].GetSource( event.source );
		if( !pOrgIconBoard )
			return;

		if( !gScreen.ExistObject() )
		{
			PostUIMessage( WM_BOARD_HOVER_OBJECT, pOrgIconBoard->GetIndex() );
		}

		// mouse hovered unit setting
		m_pSFBoardList[m_byCurBoardTab].SetHoverUnit( pOrgIconBoard );		
	}
	else if( event.id == MOUSE_CLICKED )
	{		
		if( event.source == &m_uiCloseButton )
		{
			Close();
		}		
		else if( event.source == &m_uiHelpButton )
		{
			// 도움말 창 열기
			PostMessage( gScreen.GetHWND(), WM_OPEN_WINDOW_HELP, m_dwID, 0 );
		}
	}	
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
// CGUISkillWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUISkillWindow::CGUISkillWindow()
{
	
}

CGUISkillWindow::~CGUISkillWindow()
{	

}

void
CGUISkillWindow::Init( void )
{		
	BYTE byTabNum[] = { 4, 4, 3 };
	CreateSFBoard( 3, byTabNum );


	CGUISFWindow::Init();	

	SetSprite();

	m_uiTitle.SetString( "스킬" );

	m_uiSFBoardTab[Melee].SetName( "근거리 스킬" );
	m_uiSFBoardTab[Missile].SetName( "원거리 스킬" );
	m_uiSFBoardTab[Knowledge].SetName( "클래스 스킬" );	
	
	for( int i=0; i<m_bySFBoardNum; ++i ) 
		m_pSFBoardList[i].SetTab( 0, TRUE );

	SetName( "SkillWindow" );	
	

	m_bIsCreated = TRUE;
}
	
void	
CGUISkillWindow::SetSprite( void )
{
	CGUISFWindow::SetSprite();

	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );

		// skill tab
		l_spriteInfo.wActionNo = 5;
		for( int i=0; i<m_bySFBoardNum; ++i )
		{
			l_spriteInfo.wFrameNo = i * 2 + 1;
			m_uiSFBoardTab[i].SetSpriteInfoEx( &l_spriteInfo );
			l_spriteInfo.wFrameNo = i * 2;
			m_uiSFBoardTab[i].SetSpriteInfoEx( DISABLED, &l_spriteInfo );			
		}				
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		if( !l_pSprite )
			return;

		for( int i=0; i<m_bySFBoardNum; ++i )
			m_uiSFBoardTab[i].SetSprite( l_pSprite );
	}
}

