#include "GUIPartyWindow.h"
#include "../../GUIController/GUIScreen.h"
#include "../../GUIController/GUIDialogBox.h"
#include "../../SpriteMgr.h"
#include "../../UIGlobal.h"
#include "../../GameWindow/EtcWin/GUIRadarWindow.h"

#include "../../../DefineMessage.h"

#define	MARGIN_X			7
#define	COLUMN_GAP			3

#define	NONE_MEMBER_ID		0xFFFFFFFF
#define	NONE_ORDER			0xFF

void	EP_OpenDialogBox( CGUIWindow * pi_pOwnerWindow, CGUIDialogBox * pi_pDialogBox );
void	EP_CloseDialogBox( CGUIWindow * pi_pOwnerWindow );

/*
void	EP_InviteToParty( void );
void	EP_Accept_InviteToParty( void );

void	EP_JoinToParty( void );
void	EP_Accept_JoinToParty( void );

void	EP_BanishPlayerFromParty( DWORD pi_dwIndex );
void	EP_ChangeLeader( DWORD pi_dwIndex );

void	EP_LeaveFromParty( void );

void	EP_LockParty( bool pi_bIsLock );
*/

////////////////////////////////////////////////////////////////////////////
//
// CGUIPartyMemberInfo Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIPartyMemberInfo::CGUIPartyMemberInfo()
{
	m_dwMemberID = NONE_MEMBER_ID;

	m_bCreated = FALSE;
}

CGUIPartyMemberInfo::~CGUIPartyMemberInfo()
{

}

void
CGUIPartyMemberInfo::Init( int pi_nSizeX, int pi_nSizeY )
{	
	SetSize( pi_nSizeX, pi_nSizeY );

	m_uiEventCover.SetSize( pi_nSizeX, pi_nSizeY );

	m_uiHPGauge.SetSize( 82, 10 );
	m_uiFPGauge.SetSize( 82, 10 );
	m_uiSPGauge.SetSize( 82, 10 );
	m_nGaugeSizeX = m_uiHPGauge.m_ptSize.x;

	for( int i=0; i<5; ++i )
	{
		m_uiDamageSF[i].SetSize( 16, 16 );
		m_uiHelpSF[i].SetSize( 16, 16 );
	}

	m_uiRemoveButton.SetSize( 19, 13 );
	m_uiChangeReaderButton.SetSize( 19, 13 );	

	// -----------------------------------------------------------------
	m_uiCharName.SetPos( m_ptPos.x + 26, m_ptPos.y + 3 );
	m_uiLevel.SetPos( m_ptPos.x + 2, m_ptPos.y + 3 );

	m_uiHPGauge.SetPos( m_ptPos.x, m_ptPos.y + 19 );
	m_uiFPGauge.SetPos( m_ptPos.x, m_ptPos.y + 31 );
	m_uiSPGauge.SetPos( m_ptPos.x, m_ptPos.y + 43 );

	for( i=0; i<5; ++i )
	{
		m_uiHelpSF[i].SetPos( m_ptPos.x + 85 + i * (m_uiHelpSF[0].m_ptSize.x+2), m_ptPos.y + 19 );
		m_uiDamageSF[i].SetPos( m_ptPos.x + 85 + i * (m_uiHelpSF[0].m_ptSize.x+2), m_ptPos.y + 37 );
	}

	m_uiChangeReaderButton.SetPos( m_ptPos.x + 134, m_ptPos.y + 2 );
	m_uiRemoveButton.SetPos( m_ptPos.x + 154, m_ptPos.y + 2 );


	// -----------------------------------------------------------------	
	Add( &m_uiCharName );
	Add( &m_uiLevel );
	Add( &m_uiHPGauge );
	Add( &m_uiFPGauge );
	Add( &m_uiSPGauge );	

	for( i=0; i<5; ++i )
	{
		Add( &m_uiDamageSF[i] );
		Add( &m_uiHelpSF[i] );
	}

	m_uiEventCover.AddMouseEventHandler( this );
	Add( &m_uiEventCover );

	m_uiRemoveButton.AddMouseEventHandler( this );
	m_uiChangeReaderButton.AddMouseEventHandler( this );
	Add( &m_uiChangeReaderButton );
	Add( &m_uiRemoveButton );

	// -----------------------------------------------------------------
	SetSprite();

	SetARGB( 0x90FFFFFF );

	// char name font color
	m_uiCharName.GetFont()->dwColor = UI_PARTY_MEMBER_COLOR;


	// init info data
	ClearMemberInfo();
	

	m_bCreated = TRUE;
}

void
CGUIPartyMemberInfo::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );

		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 2;
		m_uiHPGauge.SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 3;
		m_uiFPGauge.SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 4;
		m_uiSPGauge.SetSpriteInfo( &l_spriteInfo );


		m_uiHPGauge.SetSpriteV( 0.f, 1.f );
		m_uiFPGauge.SetSpriteV( 0.f, 1.f );
		m_uiSPGauge.SetSpriteV( 0.f, 1.f );


		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_PARTY );
		
		l_spriteInfo.wActionNo = 0;

		l_spriteInfo.wFrameNo = 1;
		SetSpriteInfo( &l_spriteInfo );
		
		l_spriteInfo.wFrameNo = 2;
		m_uiChangeReaderButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 3;
		m_uiChangeReaderButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 4;
		m_uiChangeReaderButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );	

		
		l_spriteInfo.wFrameNo = 5;
		m_uiRemoveButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 6;
		m_uiRemoveButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 7;
		m_uiRemoveButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		if( !l_pSprite )
			return;		

		m_uiHPGauge.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiFPGauge.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiSPGauge.GetSpriteInfo()->pSprite = l_pSprite;

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_PARTY );

		GetSpriteInfo()->pSprite = l_pSprite;

		m_uiChangeReaderButton.SetSprite( l_pSprite );
		m_uiRemoveButton.SetSprite( l_pSprite );

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
		for( int i=0; i<5; ++i )
		{
			if( m_uiDamageSF[i].GetSpriteInfo()->pSprite != NULL )
				m_uiDamageSF[i].GetSpriteInfo()->pSprite = l_pSprite;

			if( m_uiHelpSF[i].GetSpriteInfo()->pSprite != NULL )
				m_uiHelpSF[i].GetSpriteInfo()->pSprite = l_pSprite;
		}
	
	}
}

void
CGUIPartyMemberInfo::ClearMemberInfo( void )
{
	SetMemberID( NONE_MEMBER_ID );

	SetLevel( 0 );

	SetHPGauge( 0.f );
	SetFPGauge( 0.f );
	SetSPGauge( 0.f );

	// init skill force
	for( int i=0; i<5; ++i )
	{
		m_uiDamageSF[i].SetItemID( NONE_ITEM_ID );
		m_uiDamageSF[i].GetSpriteInfo()->pSprite = NULL;

		m_uiHelpSF[i].SetItemID( NONE_ITEM_ID );		
		m_uiHelpSF[i].GetSpriteInfo()->pSprite = NULL;
	}
	
}

void
CGUIPartyMemberInfo::SetCharName( char * pi_pName )
{
	m_uiCharName.SetString( pi_pName );
}

void
CGUIPartyMemberInfo::SetLevel( int pi_nLevel )
{
	char buf[8];
	sprintf( buf, "L%d", pi_nLevel );
	m_uiLevel.SetString( buf );	
}

void	
CGUIPartyMemberInfo::SetHPGauge( float pi_fHPRate )
{
	m_uiHPGauge.SetSpriteU( 0.f, pi_fHPRate );

	m_uiHPGauge.SetSize( m_nGaugeSizeX * pi_fHPRate, m_uiHPGauge.m_ptSize.y );
}

void
CGUIPartyMemberInfo::SetFPGauge( float pi_fFPRate )
{
	m_uiFPGauge.SetSpriteU( 0.f, pi_fFPRate );

	m_uiFPGauge.SetSize( m_nGaugeSizeX * pi_fFPRate, m_uiFPGauge.m_ptSize.y );
}

void
CGUIPartyMemberInfo::SetSPGauge( float pi_fSPRate )
{
	m_uiSPGauge.SetSpriteU( 0.f, pi_fSPRate );

	m_uiSPGauge.SetSize( m_nGaugeSizeX * pi_fSPRate, m_uiSPGauge.m_ptSize.y );
}

void
CGUIPartyMemberInfo::AddDamageSF( BYTE pi_bySFType, DWORD pi_dwItemID, DWORD pi_dwSpriteID )
{
	int l_nAddIndex = -1;
	for( int i=0; i<5; ++i )
	{
		if( m_uiDamageSF[i].GetItemID() == NONE_ITEM_ID )
		{
			l_nAddIndex = i;
			break;
		}
	}

	if( l_nAddIndex == -1 )
		return;

	if( pi_bySFType == 0 ) 
		m_uiDamageSF[l_nAddIndex].SetIconType( SKILL_ICON );
	else
		m_uiDamageSF[l_nAddIndex].SetIconType( FORCE_ICON );	
	
	m_uiDamageSF[l_nAddIndex].SetItemID( pi_dwItemID );
	m_uiDamageSF[l_nAddIndex].SetSpriteID( pi_dwSpriteID );

	m_uiDamageSF[l_nAddIndex].SetSprite();
}

void
CGUIPartyMemberInfo::AddHelpSF( BYTE pi_bySFType, DWORD pi_dwItemID, DWORD pi_dwSpriteID )
{
	int l_nAddIndex = -1;
	for( int i=0; i<5; ++i )
	{
		if( m_uiHelpSF[i].GetItemID() == NONE_ITEM_ID )
		{
			l_nAddIndex = i;
			break;
		}
	}

	if( l_nAddIndex == -1 )
		return;

	if( pi_bySFType == 0 ) 
		m_uiHelpSF[l_nAddIndex].SetIconType( SKILL_ICON );
	else
		m_uiHelpSF[l_nAddIndex].SetIconType( FORCE_ICON );	
	
	m_uiHelpSF[l_nAddIndex].SetItemID( pi_dwItemID );
	m_uiHelpSF[l_nAddIndex].SetSpriteID( pi_dwSpriteID );

	m_uiHelpSF[l_nAddIndex].SetSprite();	
}

void
CGUIPartyMemberInfo::RemoveDamageSF( BYTE pi_bySFType, DWORD pi_dwItemID )
{
	if( pi_dwItemID == NONE_ITEM_ID )
		return;

	for( int i=0; i<5; ++i )
	{
		if( m_uiDamageSF[i].GetItemID() == pi_dwItemID )
		{
			m_uiDamageSF[i].SetItemID( NONE_ITEM_ID );
			m_uiDamageSF[i].GetSpriteInfo()->pSprite = NULL;

			// shift
			for( int j=i; j<4; ++j )
			{
				if( m_uiDamageSF[j+1].GetItemID() == NONE_ITEM_ID )
					break;

				m_uiDamageSF[j].SetData( &m_uiDamageSF[j+1] );
				m_uiDamageSF[j].SetSize( 16, 16 );

				m_uiDamageSF[j+1].SetItemID( NONE_ITEM_ID );
				m_uiDamageSF[j+1].GetSpriteInfo()->pSprite = NULL;
			}

			break;
		}
	}
}

void
CGUIPartyMemberInfo::RemoveHelpSF( BYTE pi_bySFType, DWORD pi_dwItemID )
{
	if( pi_dwItemID == NONE_ITEM_ID )
		return;

	for( int i=0; i<5; ++i )
	{
		if( m_uiHelpSF[i].GetItemID() == pi_dwItemID )
		{
			m_uiHelpSF[i].SetItemID( NONE_ITEM_ID );
			m_uiHelpSF[i].GetSpriteInfo()->pSprite = NULL;

			// shift
			for( int j=i; j<4; ++j )
			{
				if( m_uiHelpSF[j+1].GetItemID() == NONE_ITEM_ID )
					break;

				m_uiHelpSF[j].SetData( &m_uiDamageSF[j+1] );

				m_uiHelpSF[j+1].SetItemID( NONE_ITEM_ID );
				m_uiHelpSF[j+1].GetSpriteInfo()->pSprite = NULL;
			}

			break;
		}
	}
}

void
CGUIPartyMemberInfo::RemoveAllSF( void )
{
	for( int i=0; i<5; ++i )
	{
		m_uiDamageSF[i].SetItemID( NONE_ITEM_ID );
		m_uiDamageSF[i].GetSpriteInfo()->pSprite = NULL;

		m_uiHelpSF[i].SetItemID( NONE_ITEM_ID );		
		m_uiHelpSF[i].GetSpriteInfo()->pSprite = NULL;
	}
}

void
CGUIPartyMemberInfo::EnableRemveButton( BOOL pi_bEnable )
{
	if( pi_bEnable )
	{			
		m_uiRemoveButton.Enable();
	}
	else
	{
		m_uiRemoveButton.Disable();
	}
}

void
CGUIPartyMemberInfo::EnableChLeaderButton( BOOL pi_bEnable )
{
	if( pi_bEnable )
	{		
		m_uiChangeReaderButton.Enable();
	}
	else
	{
		m_uiChangeReaderButton.Disable();
	}
}


void
CGUIPartyMemberInfo::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiRemoveButton )
		{
			// ################## 코드 추가 시킬 곳 ###########################
			// 강퇴
			//EP_BanishPlayerFromParty( m_dwMemberID );
			PostMessage( gScreen.GetHWND(), WM_PARTY_BANISH_PLAYER, m_dwMemberID, 0 );
		}
		else if( event.source == &m_uiChangeReaderButton )
		{
			// ################## 코드 추가 시킬 곳 ###########################
			// 파티리더 바꿈
			//EP_ChangeLeader( m_dwMemberID );
			PostMessage( gScreen.GetHWND(), WM_PARTY_CHANGE_LEADER, m_dwMemberID, 0 );
		}
	}	
	else if( event.id == MOUSE_STROKED )
	{
		// ################## 코드 추가 시킬 곳 ###########################
		// 파티원 선택
		PostMessage( gScreen.GetHWND(), WM_PARTY_SELECT_MEMBER, m_dwMemberID, 0 );
	}
}

////////////////////////////////////////////////////////////////////////////
//
// CGUIPartyWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIPartyWindow::CGUIPartyWindow()
{
	m_byMemberType = UI_PMT_NONE;

	m_byTotalMember = 0;

	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{
		m_byMemberOrder[i] = NONE_ORDER;
	}

	m_bIsLocked = FALSE;

	m_bCreated = FALSE;
}

CGUIPartyWindow::~CGUIPartyWindow()
{

}

void
CGUIPartyWindow::Init( void )
{
	SetSize( 185, 80 );

	m_uiTitleBoard.SetSize( 177, 18 );	
	
	m_uiLockNLeaveButton.SetSize( 72, 20 );

	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{		
		m_uiMember[i].Init( 173, 53 );
	}
	
	m_uiTitleBoard.SetPos( m_ptPos.x + 4, m_ptPos.y + 4 );

	m_uiLockNLeaveButton.SetPos( m_ptPos.x + (m_ptSize.x - m_uiLockNLeaveButton.m_ptSize.x)/2,
								 m_ptPos.y + m_ptSize.y - m_uiLockNLeaveButton.m_ptSize.y - 10 );	


	// ------------------------------------------------------------------------
	m_uiLockNLeaveButton.AddMouseEventHandler( this );

	Add( &m_uiTitleBoard );
	Add( &m_uiTitle );	
//	Add( &m_uiLockNLeaveButton );

	
	SetSprite();

	SetARGB( 0xC0252525 );

	m_uiTitleBoard.SetARGB( 0xC0FFFFFF );

	m_uiTitle.GetFont()->dwColor = D3DCOLOR_XRGB( 195, 193, 190 );

	m_uiLockNLeaveButton.SetLabelStyle( R3_HAN_OUTLINE );	

	// -------------------------------------------------
	SetMoveObject( &m_uiTitleBoard );
	SetMoveObject( &m_uiTitle );
	SetMovable( true );	

	SetTotalMemberNum( 0 );
	

	m_bCreated = TRUE;	
}

void
CGUIPartyWindow::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_PARTY );

		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 0;
		m_uiTitleBoard.SetSpriteInfo( &l_spriteInfo );					


		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );			
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 15;
		m_uiLockNLeaveButton.SetSpriteInfoEx( &l_spriteInfo );	
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_PARTY );
		if( !l_pSprite )
			return;

		m_uiTitleBoard.GetSpriteInfo()->pSprite = l_pSprite;		

		m_uiLockNLeaveButton.SetSprite( gSpriteMgr.GetSprite( SP_ID_COMMON ) );

		for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )		
			m_uiMember[i].SetSprite();
	}
}

void
CGUIPartyWindow::SetWindowType( BYTE pi_byMemberType )
{
	BYTE l_byPrevMemberType = m_byMemberType;

	m_byMemberType = pi_byMemberType;

	if( m_byMemberType == UI_PMT_NONE )
	{
		// remove 
		RemoveAllMember();
		
		Remove( &m_uiLockNLeaveButton );		
	}
	else if( m_byMemberType == UI_PMT_MEMBER )
	{
		// 탈퇴 버튼 setting
		m_uiLockNLeaveButton.SetLabel( "탈  퇴" );		
		if( m_byTotalMember > 0 )
			EnableLockNLeaveButton( true );
		else 
			EnableLockNLeaveButton( false );

		m_uiLockNLeaveButton.SetPos( m_ptPos.x + (m_ptSize.x - m_uiLockNLeaveButton.m_ptSize.x)/2,
									 m_ptPos.y + m_ptSize.y - m_uiLockNLeaveButton.m_ptSize.y - 10 );
		m_uiLockNLeaveButton.Show( IsShow() );
		Add( &m_uiLockNLeaveButton );


		// member의 changeLeader button, remove button을 비활성화 시킨다.
		for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
		{
			if( m_uiMember[i].GetMemberID() == NONE_MEMBER_ID )
				continue;

			m_uiMember[i].EnableChLeaderButton( false );
			m_uiMember[i].EnableRemveButton( false );
		}
	}  
	else if( m_byMemberType == UI_PMT_LEADER )
	{
		// 잠금 버튼 setting
		m_uiLockNLeaveButton.SetLabel( "잠  금" );
		if( m_byTotalMember > 0 )
			EnableLockNLeaveButton( true );
		else
			EnableLockNLeaveButton( false );

		m_uiLockNLeaveButton.SetPos( m_ptPos.x + (m_ptSize.x - m_uiLockNLeaveButton.m_ptSize.x)/2,
									 m_ptPos.y + m_ptSize.y - m_uiLockNLeaveButton.m_ptSize.y - 10 );
		m_uiLockNLeaveButton.Show( IsShow() );
		Add( &m_uiLockNLeaveButton );

		// member의 changeLeader button, remove button을 활성화 시킨다.
		for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
		{
			if( m_uiMember[i].GetMemberID() == NONE_MEMBER_ID )
				continue;

			m_uiMember[i].EnableChLeaderButton( true );
			m_uiMember[i].EnableRemveButton( true );
		}
	}
}

BOOL
CGUIPartyWindow::AddMember( GROUP_MEMBER_INFO * pi_pMemberInfo )
{
	if( m_byMemberType == UI_PMT_NONE ||
		m_byTotalMember == MAX_PARTY_MEMBER_NUM )
		return false;

	// 빈곳을 찾는다.
	int l_nAddIndex = -1;
	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{
		if( m_uiMember[i].GetMemberID() == NONE_MEMBER_ID )
		{
			l_nAddIndex = i;
			break;
		}
	}

	// 빈곳이 없다
	if( l_nAddIndex == -1 )
		return false;


	// order를 저장해둔다.	
	int l_nOrder = -1;
	for( i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{
		if( m_byMemberOrder[i] == NONE_ORDER )
		{
			m_byMemberOrder[i] = l_nAddIndex;	
			l_nOrder = i;
			break;
		}
	}

	// error
	if( l_nOrder == -1 )	
		return false;

	// ------------------------------------------------------------
	// set member info
	m_uiMember[l_nAddIndex].SetMemberID( pi_pMemberInfo->dwMemberID );
	m_uiMember[l_nAddIndex].SetCharName( pi_pMemberInfo->pCharName );
	m_uiMember[l_nAddIndex].SetLevel( pi_pMemberInfo->wLevel );
	m_uiMember[l_nAddIndex].SetHPGauge( pi_pMemberInfo->fHPRate );
	m_uiMember[l_nAddIndex].SetFPGauge( pi_pMemberInfo->fFPRate );

	// add skill, force
	for( i=0; i<5; ++i )
	{
		if( pi_pMemberInfo->sfDamage[i].dwItemID != NONE_ITEM_ID )
		{
			m_uiMember[l_nAddIndex].AddDamageSF( pi_pMemberInfo->sfDamage[i].bySFType,
											     pi_pMemberInfo->sfDamage[i].dwItemID, 
											     pi_pMemberInfo->sfDamage[i].dwSpriteID );
		}
		if( pi_pMemberInfo->sfHelp[i].dwItemID != NONE_ITEM_ID )
		{
			m_uiMember[l_nAddIndex].AddHelpSF( pi_pMemberInfo->sfHelp[i].bySFType,
											   pi_pMemberInfo->sfHelp[i].dwItemID, 
											   pi_pMemberInfo->sfHelp[i].dwSpriteID );
		}
	}


	// 파티리더 권한 버튼 setting
	if( m_byMemberType == UI_PMT_LEADER )
	{
		m_uiMember[l_nAddIndex].EnableRemveButton( true );
		m_uiMember[l_nAddIndex].EnableChLeaderButton( true );
	}
	else
	{
		m_uiMember[l_nAddIndex].EnableRemveButton( false );
		m_uiMember[l_nAddIndex].EnableChLeaderButton( false );
	}


	// add member
	m_uiMember[l_nAddIndex].SetPos( m_ptPos.x + MARGIN_X, 
									m_uiTitleBoard.m_ptPos.y + m_uiTitleBoard.m_ptSize.y + l_nOrder * (m_uiMember[l_nAddIndex].m_ptSize.y + COLUMN_GAP) );
	m_uiMember[l_nAddIndex].Show( IsShow() );
	Add( &m_uiMember[l_nAddIndex] );


	// total member setting
	SetTotalMemberNum( m_byTotalMember + 1 );

	
	// 정원이 다 차면 잠금버튼을 disable시킨다.
	if( m_byTotalMember == MAX_PARTY_MEMBER_NUM )
	{
		if( m_byMemberType == UI_PMT_LEADER )
			EnableLockNLeaveButton( false );
	}	
	// 첫번째 멤버이면 탈퇴버튼을 enable한다.
	else if( m_byTotalMember == 1 )
	{
		if( m_byMemberType == UI_PMT_LEADER )
			EnableLockNLeaveButton( true );
		else if( m_byMemberType == UI_PMT_MEMBER )
			EnableLockNLeaveButton( true );
	}
	

	// resize window
	SetSize( m_ptSize.x,			 
			 m_uiTitleBoard.m_ptPos.y + m_uiTitleBoard.m_ptSize.y - m_ptPos.y +
			 m_byTotalMember * (m_uiMember[0].m_ptSize.y + 3) + m_uiLockNLeaveButton.m_ptSize.y + 20 );
	m_uiLockNLeaveButton.SetPos( m_ptPos.x + (m_ptSize.x - m_uiLockNLeaveButton.m_ptSize.x)/2,
								 m_ptPos.y + m_ptSize.y - m_uiLockNLeaveButton.m_ptSize.y - 10 );

	return true;
}

void
CGUIPartyWindow::RemoveMember( DWORD pi_dwMemberID )
{
	if( m_byMemberType == UI_PMT_NONE )
		return;


	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{
		if( m_byMemberOrder[i] == NONE_ORDER )
			continue;

		if( m_uiMember[m_byMemberOrder[i]].GetMemberID() == pi_dwMemberID )
		{	
			// 멤버 정보를 모두 지운다.
			m_uiMember[m_byMemberOrder[i]].ClearMemberInfo();
			Remove( &m_uiMember[m_byMemberOrder[i]] );			

			m_byMemberOrder[i] = NONE_ORDER;


			// total member setting
			SetTotalMemberNum( m_byTotalMember - 1 );
			
			// 파티원일 때 아무도 없으면 탈퇴버튼을 disable한다.
			if( m_byTotalMember == 0 )
			{
				if( m_byMemberType == UI_PMT_MEMBER )
					EnableLockNLeaveButton( false );
			}


			// 뒤에 있는 member를 한칸씩 shift시켜준다.
			for( int j=i; j<MAX_PARTY_MEMBER_NUM - 1 ; ++j )
			{
				if( m_byMemberOrder[j+1] == NONE_ORDER )
					break;

				m_byMemberOrder[j] = m_byMemberOrder[j+1];				
				m_byMemberOrder[j+1] = NONE_ORDER;

				// repos member
				m_uiMember[m_byMemberOrder[j]].SetPos( m_ptPos.x + MARGIN_X,
											m_uiTitleBoard.m_ptPos.y + m_uiTitleBoard.m_ptSize.y + j * ( m_uiMember[j].m_ptSize.y + COLUMN_GAP ) );
			}

			// resize window
			SetSize( m_ptSize.x,
					 m_uiTitleBoard.m_ptPos.y + m_uiTitleBoard.m_ptSize.y - m_ptPos.y +
					 m_byTotalMember * (m_uiMember[0].m_ptSize.y + 3) + m_uiLockNLeaveButton.m_ptSize.y + 20 );					 
					
			m_uiLockNLeaveButton.SetPos( m_ptPos.x + (m_ptSize.x - m_uiLockNLeaveButton.m_ptSize.x)/2,
										 m_ptPos.y + m_ptSize.y - m_uiLockNLeaveButton.m_ptSize.y - 10 );			
			break;
		}
	}
		
}

void
CGUIPartyWindow::RemoveAllMember( void )
{
	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{		
		m_uiMember[i].ClearMemberInfo();
		Remove( &m_uiMember[i] );

		m_byMemberOrder[i] = NONE_ORDER;
	}

	// total member setting	
	SetTotalMemberNum( 0 );
	
	EnableLockNLeaveButton( false );	

	// resize window
	SetSize( m_ptSize.x,
			 m_uiTitleBoard.m_ptSize.y + m_uiLockNLeaveButton.m_ptSize.y + 20 );

	m_uiLockNLeaveButton.SetPos( m_ptPos.x + (m_ptSize.x - m_uiLockNLeaveButton.m_ptSize.x)/2,
								 m_ptPos.y + m_ptSize.y - m_uiLockNLeaveButton.m_ptSize.y - 10 );			
}

void
CGUIPartyWindow::SetLeader( GROUP_MEMBER_INFO * pi_pMemberInfo )
{
	// 먼저 이미 파티원으로 틍록되어 있는 사람인지 검사한다.
	int l_nLeaderIndex = -1;
	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{
		if( m_byMemberOrder[i] == NONE_ORDER )
			continue;

		if( m_uiMember[m_byMemberOrder[i]].GetMemberID() == pi_pMemberInfo->dwMemberID )
		{
			l_nLeaderIndex = i;
			break;
		}
	}

	// 파티원이 아니면
	if( l_nLeaderIndex == -1 )
	{
		// 일단 파티원으로 등록 시킨다.
		if( !AddMember( pi_pMemberInfo ) )
			return;

		SetLeader( pi_pMemberInfo->dwMemberID );
	}
	else
	{
		SetLeader( pi_pMemberInfo->dwMemberID );
	}	
}

void
CGUIPartyWindow::SetLeader( DWORD pi_dwMemberID )
{
	if( m_byMemberOrder[0] == NONE_ORDER )
		return;

	if( m_uiMember[m_byMemberOrder[0]].GetMemberID() != pi_dwMemberID )
	{
		// 파티원을 찾는다.
		int l_nLeaderIndex = -1;
		for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
		{
			if( m_byMemberOrder[i] == NONE_ORDER )
				continue;

			if( m_uiMember[m_byMemberOrder[i]].GetMemberID() == pi_dwMemberID )
			{
				l_nLeaderIndex = i;
				break;
			}
		}

		// 파티원이 없다.
		if( l_nLeaderIndex == -1 )
			return;


		// leader가 맨앞이 아니면 맨앞에 오도록 정렬한다.
		if( l_nLeaderIndex != 0 )
		{
			BYTE l_byTempOrderIndex[MAX_PARTY_MEMBER_NUM];

			// backup leader index
			l_byTempOrderIndex[0] = m_byMemberOrder[l_nLeaderIndex];
			m_byMemberOrder[l_nLeaderIndex] = NONE_ORDER;

			// 빈칸이 없도록 shift한다.
			int l_nTempIndex=1;
			for( i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
			{
				if( m_byMemberOrder[i] != NONE_ORDER )
				{
					l_byTempOrderIndex[l_nTempIndex] = m_byMemberOrder[i];
					++l_nTempIndex;
				}						
			}
			
			// 빈칸 채우기
			for( i=l_nTempIndex; i<MAX_PARTY_MEMBER_NUM; ++i )
			{
				l_byTempOrderIndex[i] = NONE_ORDER;
			}

			// member color setting
			if( m_byMemberOrder[0] != NONE_ORDER )			
				m_uiMember[m_byMemberOrder[0]].GetCharNameInfo()->GetFont()->dwColor = UI_PARTY_MEMBER_COLOR;

			// copy order			
			memcpy( m_byMemberOrder, l_byTempOrderIndex, sizeof( m_byMemberOrder ) );

			// 정렬된 order로 repos한다.
			for( i=0; i<MAX_PARTY_MEMBER_NUM; ++i )			
			{
				if( m_byMemberOrder[i] == NONE_ORDER )
					continue;

				m_uiMember[m_byMemberOrder[i]].SetPos( m_ptPos.x + MARGIN_X,
												m_uiTitleBoard.m_ptPos.y + m_uiTitleBoard.m_ptSize.y + i * ( m_uiMember[i].m_ptSize.y + COLUMN_GAP ) );												
			}
		}
	}

	// leader char name color
	CGUIText * l_pText;
	l_pText = m_uiMember[m_byMemberOrder[0]].GetCharNameInfo();
	l_pText->GetFont()->dwColor = UI_PARTY_LEADER_COLOR;
}

void
CGUIPartyWindow::SetMember( DWORD pi_dwMemberID )
{	
	// error check
	if( m_byMemberOrder[0] >= MAX_PARTY_MEMBER_NUM )
		return;

	if( m_uiMember[m_byMemberOrder[0]].GetMemberID() != pi_dwMemberID )
	{
		// 파티원을 찾는다.
		int l_nLeaderIndex = -1;
		for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
		{
			if( m_byMemberOrder[i] == NONE_ORDER )
				continue;

			if( m_uiMember[m_byMemberOrder[i]].GetMemberID() == pi_dwMemberID )
			{
				l_nLeaderIndex = i;
				break;
			}
		}

		// 파티원이 없다.
		if( l_nLeaderIndex == -1 )
			return;

		m_uiMember[m_byMemberOrder[l_nLeaderIndex]].GetCharNameInfo()->GetFont()->dwColor = UI_PARTY_MEMBER_COLOR;
	}
	else
	{
		m_uiMember[m_byMemberOrder[0]].GetCharNameInfo()->GetFont()->dwColor = UI_PARTY_MEMBER_COLOR;
	}	
}

void
CGUIPartyWindow::SetLevel( DWORD pi_dwMemberID, int pi_nLevel )
{
	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{
		if( m_uiMember[i].GetMemberID() == NONE_MEMBER_ID )
			continue;

		if( m_uiMember[i].GetMemberID() == pi_dwMemberID )
		{
			m_uiMember[i].SetLevel( pi_nLevel );
			break;
		}
	}
}

void
CGUIPartyWindow::SetHPGauge( DWORD pi_dwMemberID, float pi_fHPRate )
{
	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{
		if( m_uiMember[i].GetMemberID() == NONE_MEMBER_ID )
			continue;

		if( m_uiMember[i].GetMemberID() == pi_dwMemberID )
		{
			m_uiMember[i].SetHPGauge( pi_fHPRate );
			break;
		}
	}
}

void
CGUIPartyWindow::SetFPGauge( DWORD pi_dwMemberID, float pi_fFPRate )
{
	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{
		if( m_uiMember[i].GetMemberID() == NONE_MEMBER_ID )
			continue;

		if( m_uiMember[i].GetMemberID() == pi_dwMemberID )
		{
			m_uiMember[i].SetFPGauge( pi_fFPRate );
			break;
		}
	}
}

void
CGUIPartyWindow::SetSPGauge( DWORD pi_dwMemberID, float pi_fSPRate )
{
	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{
		if( m_uiMember[i].GetMemberID() == NONE_MEMBER_ID )
			continue;

		if( m_uiMember[i].GetMemberID() == pi_dwMemberID )
		{
			m_uiMember[i].SetSPGauge( pi_fSPRate );
			break;
		}
	}
}

void
CGUIPartyWindow::AddSkillForce( DWORD	pi_dwMemberID,
							    BYTE	pi_bySFType,		// Skill or force
							    BYTE	pi_byContType,		// 피해 or 보조
							    DWORD	pi_dwItemID,
							    DWORD	pi_dwSpriteID )
{
	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{
		if( m_uiMember[i].GetMemberID() == NONE_MEMBER_ID )
			continue;

		if( m_uiMember[i].GetMemberID() == pi_dwMemberID )
		{
			if( pi_byContType == SF_CONT_DAMAGE )
				m_uiMember[i].AddDamageSF( pi_bySFType, pi_dwItemID, pi_dwSpriteID );
			else
				m_uiMember[i].AddHelpSF( pi_bySFType, pi_dwItemID, pi_dwSpriteID );
			break;
		}
	}	
}

void
CGUIPartyWindow::RemoveSkillForce( DWORD pi_dwMemberID, BYTE pi_bySFType, BYTE pi_byContType, DWORD pi_dwItemID )
{
	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{
		if( m_uiMember[i].GetMemberID() == NONE_MEMBER_ID )
			continue;

		if( m_uiMember[i].GetMemberID() == pi_dwMemberID )
		{
			if( pi_byContType == SF_CONT_DAMAGE )
				m_uiMember[i].RemoveDamageSF( pi_bySFType, pi_dwItemID );
			else
				m_uiMember[i].RemoveHelpSF( pi_bySFType, pi_dwItemID );
			break;
		}
	}
}

void
CGUIPartyWindow::RemoveAllSkillForce( DWORD	pi_dwMemberID )
{
	for( int i=0; i<MAX_PARTY_MEMBER_NUM; ++i )
	{
		if( m_uiMember[i].GetMemberID() == NONE_MEMBER_ID )
			continue;

		if( m_uiMember[i].GetMemberID() == pi_dwMemberID )
		{
			m_uiMember[i].RemoveAllSF();			
			break;
		}
	}
}

void
CGUIPartyWindow::Lock( BOOL pi_bLock )
{
	if( m_byMemberType != UI_PMT_LEADER )
		return;	

	m_bIsLocked = pi_bLock;

	if( m_bIsLocked )
	{
		m_uiLockNLeaveButton.SetLabel( "잠금해제" );
	}
	else
	{
		m_uiLockNLeaveButton.SetLabel( "잠  금" );
	}
	
}

void
CGUIPartyWindow::EnableLockNLeaveButton( BOOL pi_bEnable )
{
	SPRITE_INFO l_spriteInfo;
	memcpy( &l_spriteInfo, m_uiLockNLeaveButton.GetSpriteInfo(), sizeof( SPRITE_INFO ) );

	if( pi_bEnable )
	{
		l_spriteInfo.wFrameNo = 12;
		m_uiLockNLeaveButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_uiLockNLeaveButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );	
		l_spriteInfo.wFrameNo = 14;
		m_uiLockNLeaveButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );	
		
		m_uiLockNLeaveButton.Enable();	
	}
	else
	{
		l_spriteInfo.wFrameNo = 15;
		m_uiLockNLeaveButton.SetSpriteInfoEx( &l_spriteInfo );

		m_uiLockNLeaveButton.Disable();		
	}
}

void
CGUIPartyWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiLockNLeaveButton )
		{			
			if( m_byMemberType == UI_PMT_LEADER )
			{
				// ################## 코드 추가 시킬 곳 ###########################
				if( m_bIsLocked )
				{
					// 잠금 해제 
					//EP_LockParty( false );
					PostMessage( gScreen.GetHWND(), WM_PARTY_LOCKON_ADDMEMBER, FALSE, 0 );
				}
				else
				{
					// 잠금
					//EP_LockParty( true );
					PostMessage( gScreen.GetHWND(), WM_PARTY_LOCKON_ADDMEMBER, TRUE, 0 );
				}
			}
			else if( m_byMemberType == UI_PMT_MEMBER )
			{
				// ################## 코드 추가 시킬 곳 ###########################
				// 탈퇴
				//EP_LeaveFromParty();
				PostMessage( gScreen.GetHWND(), WM_PARTY_LEAVE_PARTY, 0, 0 );
			}
		}				
	}
	
}

#define OUT_LINE_COLOR	D3DCOLOR_XRGB(2,2,2)

BOOL
CGUIPartyWindow::Draw( void )
{
	if( !IsShow() )
	{
		DrawDialogBox();
		return false;
	}	

	// left
	Draw2DRectangle( m_ptPos.x + 1,
					 m_ptPos.y + 1,
					 m_ptPos.x + 3,
					 m_ptPos.y + m_ptSize.y - 1,
					 OUT_LINE_COLOR );
	// right
	Draw2DRectangle( m_ptPos.x + m_ptSize.x - 3,
					 m_ptPos.y + 1,
					 m_ptPos.x + m_ptSize.x - 1,
					 m_ptPos.y + m_ptSize.y - 1,
					 OUT_LINE_COLOR );

	// top
	Draw2DRectangle( m_ptPos.x + 1,
					 m_ptPos.y + 1,
					 m_ptPos.x + m_ptSize.x - 1,
					 m_ptPos.y + 3,
					 OUT_LINE_COLOR );

	// bottom
	Draw2DRectangle( m_ptPos.x + 1,
					 m_ptPos.y + m_ptSize.y - 3,
					 m_ptPos.x + m_ptSize.x - 1,
					 m_ptPos.y + m_ptSize.y - 1,
					 OUT_LINE_COLOR );	

	CGUIContainer::Draw();

	DrawDialogBox();

	return true;
}

void	
CGUIPartyWindow::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{
	/*
	_GUIMessageBox( &s_uiPartyWindow, "파티를 만들겠습니까?", 0xb0ffffff, MSG_PARTY_CREATE_ID, DLG_STYLE_OK_CANCEL );
	EP_OpenDialogBox( &s_uiPartyWindow, NULL );
	*/

	if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_PARTY_INVITE )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
		{
			// ################## 코드 추가 시킬 곳 ###########################				
			// 파티 생성
			//EP_InviteToParty();
		}
		else
		{
			// ################## 코드 추가 시킬 곳 ###########################
			// 파티 생성 취소
		}
	}
	else if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_PARTY_ACCEPT_INVITE )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
		{
			// ################## 코드 추가 시킬 곳 ###########################				
			// 파티 초대 수락
			//EP_Accept_InviteToParty();
		}
		else
		{
			// ################## 코드 추가 시킬 곳 ###########################
			// 파티 초대 거절
		}
	}
	else if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_PARTY_JOIN )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
		{
			// ################## 코드 추가 시킬 곳 ###########################				
			// 파티 가입
			//EP_JoinToParty();
		}
		else
		{
			// ################## 코드 추가 시킬 곳 ###########################
			// 파티 가입 취소
		}
	}
	else if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_PARTY_ACCEPT_JOIN )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
		{
			// ################## 코드 추가 시킬 곳 ###########################				
			// 파티 가입 수락
			//EP_Accept_JoinToParty();
		}
		else
		{
			// ################## 코드 추가 시킬 곳 ###########################
			// 파티 가입 거절
		}
	}

	CGUIWindow::ClosingDialogBox( pi_pDlg );	
}

void
CGUIPartyWindow::SetTotalMemberNum( int pi_nNum )
{
	m_byTotalMember = pi_nNum;

	char buf[32];
	if( m_byTotalMember == 0 )
		sprintf( buf, "파 티  0/%d", MAX_PARTY_MEMBER_NUM + 1 ); // 파티상태가 아닌경우
	else
		sprintf( buf, "파 티  %d/%d", m_byTotalMember + 1, MAX_PARTY_MEMBER_NUM + 1 ); // 자신을 포함한 멤버수

	m_uiTitle.SetString( buf );

	m_uiTitle.SetPos( m_uiTitleBoard.m_ptPos.x + (m_uiTitleBoard.m_ptSize.x - m_uiTitle.m_ptSize.x)/2, 
					  m_uiTitleBoard.m_ptPos.y + (m_uiTitleBoard.m_ptSize.y - m_uiTitle.m_ptSize.y)/2 );
}